#include "http.h"
#include "field.h"
#include <iostream>
#include "mysql.h"
#include <algorithm>
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/reader.h>
#include "rapidjson/filereadstream.h"
#include "rapidjson/filewritestream.h"
#include <time.h>
#include <fstream>

int us_cms_http_get_search(
	const std::string& path,
	const std::multimap< std::string, std::string >& headers,
	const std::multimap< std::string, std::string >& query_parameters,
	const std::map< std::string, std::string >& path_parameters,
	const std::vector<uint8_t>& request_body,
	const std::string& dest_endpoint,
	std::multimap< std::string, std::string >& response_headers,
	std::vector<uint8_t>& response_body)
{
	response_headers.insert(std::make_pair(std::string("Content-type"), std::string("text/json")));

	/*连接数据库*/
	std::string hostip;
	int port;
	std::string user;
	std::string password;
	std::string database;
	std::string tablename;

	/*读取配置文件（json文件）获取ip和端口以及数据库名等*/
	rapidjson::Document doc_cfg;
	FILE *myFile = NULL;
	fopen_s(&myFile, "./ngcc_metadata_config.json", "rb");
	if (myFile) 
	{
		char buf_cfg[4096] = { 0 };
		rapidjson::FileReadStream inputStream(myFile, buf_cfg, sizeof(buf_cfg));  //创建一个输入流
		doc_cfg.ParseStream(inputStream); //将读取的内容转换为dom元素
		fclose(myFile); //关闭文件，很重要
	}
	else 
	{
		std::string rntStr = "{\"status\": -1, \"msg\": \"can't find config file\"}";
		response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));
		return 400;
	}
	if (doc_cfg.HasParseError())
	{
		std::string rntStr = "{\"status\": -1, \"msg\": \"config json is error\"}";
		response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));
		return 400;
	}
	if (doc_cfg.HasMember("hostip"))
	{
		if (!doc_cfg["hostip"].IsString())
		{
			hostip = doc_cfg["hostip"].GetString();
		}
	}
	if (doc_cfg.HasMember("port"))
	{
		if (doc_cfg["port"].IsInt())
		{
			port = doc_cfg["port"].GetInt();
		}
	}
	if (doc_cfg.HasMember("user"))
	{
		if (doc_cfg["user"].IsString())
		{
			user = doc_cfg["user"].GetString();
		}
	}
	if (doc_cfg.HasMember("password"))
	{
		if (doc_cfg["password"].IsString())
		{
			password = doc_cfg["password"].GetString();
		}
	}
	if (doc_cfg.HasMember("database"))
	{
		if (doc_cfg["database"].IsString())
		{
			database = doc_cfg["database"].GetString();
		}
	}
	if (doc_cfg.HasMember("tablename"))
	{
		if (doc_cfg["tablename"].IsString())
		{
			tablename = doc_cfg["tablename"].GetString();
		}
	}
	
	/*连接数据库*/
	MYSQL * m_mysql = mysql_init(NULL);  //在程序使用Mysql的最开始必须调用mysql_int()初始化
	if (m_mysql == NULL)
	{
		std::string rntStr = "{\"status\": -1, \"msg\": \"mysql init failed\"}";
		response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));
		return 500;
	}
	int arg = 1;
	mysql_options(m_mysql, MYSQL_OPT_RECONNECT, &arg);

	if (mysql_real_connect(m_mysql, hostip.c_str(),
		user.c_str(), password.c_str(), database.c_str(), port, NULL, 0) == NULL)
	{
		std::string rntStr = "{\"status\": -1, \"msg\": \"mysql_real_connect failed\"}";
		response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));
		return 500;
	}

	/*获取字段映射表*/
	int out_size = 0;
	const us_cms_metdata_map *metamap = us_get_cms_metadata_map_table(&out_size);
	std::vector<us_cms_metdata_map> metamaps;
	metamaps.reserve(out_size);
	if (metamap != nullptr)
	{
		for (int i = 0; i < out_size; ++i)
		{
			metamaps.push_back(*metamap);
			metamap++;
		}
	}
	else
	{
		std::string rntStr = "{\"status\": -1, \"msg\": \"获取字段映射表失败\"}";
		response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));
		return 500;
	}

	/*sql语句*/
	std::string filterField = "url";
	for (auto itr = metamaps.begin(); itr != metamaps.end(); ++itr)
	{
		if (itr->m_custon_field == "url")
		{
			char buf_ff[10] = { 0 };
			sprintf_s(buf_ff, "%d", itr->m_db_field);
			filterField = std::string(buf_ff, strlen(buf_ff));
		}
	}
	std::string sql = "select ";
	sql.append("`").append(filterField).append("` from ").append(tablename).append(" where ");

	/*解析request_body中的json字符串*/
	std::string jsonstr;
	for (auto itr = request_body.begin(); itr != request_body.end(); ++itr)
	{
		jsonstr.push_back(*itr);
	}
	rapidjson::Document doc1;
	doc1.Parse(jsonstr.c_str());  
	if (doc1.HasParseError())
	{
		std::string rntStr = "{\"status\": -1, \"msg\": \"请求实体json格式不正确\"}";
		response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));
		return 400;
	}
	/*query参数*/
	if (doc1.HasMember("query"))
	{
		if (doc1["query"].IsObject())
		{
			rapidjson::Value &queryobj = doc1["query"];
			
			/*match参数（精确查询和字符串的正则查询）*/
			if (queryobj.HasMember("match"))
			{
				if (queryobj["match"].IsArray())
				{
					for (int i = 0; i < queryobj["match"].Capacity(); ++i)
					{
						/*获取字段名*/
						std::string fieldname;
						if (queryobj["match"][i].HasMember("field"))
						{
							if (queryobj["match"][i]["field"].IsString())
							{
								fieldname = queryobj["match"][i]["field"].GetString();
							}
						}
						/*获取查询模式*/
						int mode = 0;
						if (queryobj["match"][i].HasMember("mode"))
						{
							if (queryobj["match"][i]["mode"].IsInt())
							{
								mode = queryobj["match"][i]["mode"].GetInt();
							}
						}
						/*获取匹配串*/
						std::string pattern;
						if (queryobj["match"][i].HasMember("pattern"))
						{
							if (queryobj["match"][i]["pattern"].IsString())
							{
								pattern = queryobj["match"][i]["pattern"].GetString();
							}
							if (queryobj["match"][i]["pattern"].IsInt())
							{
								int pattern_int = queryobj["match"][i]["pattern"].GetInt();
								char buf[100] = { 0 };
								sprintf_s(buf, "%d", pattern_int);
								pattern = std::string(buf, strlen(buf));
							}
							if (queryobj["match"][i]["pattern"].IsDouble())
							{
								double pattern_db = queryobj["match"][i]["pattern"].GetDouble();
								char buf[100] = { 0 };
								sprintf_s(buf, "%lf", pattern_db);
								pattern = std::string(buf, strlen(buf));
							}
						}
						/*检查pattern，field和mode的合法性*/
						std::string dbfield;
						auto itr = metamaps.begin();
						for (; itr != metamaps.end(); ++itr)
						{
							if (itr->m_custon_field == fieldname)
							{
								char buf_db_filed[10] = { 0 };
								sprintf_s(buf_db_filed, "%d", itr->m_db_field);
								dbfield = std::string(buf_db_filed, strlen(buf_db_filed));
								if ((itr->m_mode & mode) >= 1)
								{
									break;
								}
							}
						}
						if (itr == metamaps.end())
						{
							std::string rntStr = "{\"status\": -1, \"msg\": \"请求实体filed或者mode有误\"}";
							response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));
							return 400;
						}
						if (pattern.empty())
						{
							continue; //如果匹配串为空，跳过该字段不作查询
						}
						/*根据不同查询模式填写sql语句*/
						if (mode == 0x01)	//用正则表达式查询
						{
							pattern.push_back(' ');
							size_t posspc;
							size_t posbeg = 0;
							std::string regexp = "^.*?[";
							do
							{
								posspc = pattern.find_first_of(" ", posbeg);
								std::string substr = pattern.substr(posbeg, posspc - posbeg);
								std::string buf_regx;
								buf_regx.resize(100);
								int ret = mysql_real_escape_string(m_mysql, (char*)buf_regx.data(), substr.data(), substr.size());
								regexp.append(buf_regx.data(), ret).append("|");
								posbeg = posspc + 1;
							} while (posspc < pattern.size() - 1);
							regexp = regexp.substr(0, regexp.size() - 1); //去掉结尾的 |
							regexp.append("].*$");
							sql.append("(`").append(dbfield).append("` regexp '").append(regexp).append("')");
						}
						else if (mode == 0x02)	//字符串精确查询
						{
							sql.append("(`").append(dbfield).append("` = '").append(pattern).append("')");
						}
						else if (mode == 0x04)	//日期的精确查询
						{
							time_t tm_t;
							tm t_s;
							t_s.tm_year = atoi(pattern.substr(0, 4).c_str()) - 1900;
							t_s.tm_mon = atoi(pattern.substr(5, 2).c_str()) - 1;
							t_s.tm_mday = atoi(pattern.substr(6, 2).c_str());
							t_s.tm_hour = 0;
							t_s.tm_min = 0;
							t_s.tm_sec = 0;
							tm_t = mktime(&t_s);
							char buf_tm[100] = { 0 };
							sprintf_s(buf_tm, "%llu", tm_t);
							pattern = std::string(buf_tm, strlen(buf_tm));
							sql.append("(`").append(dbfield).append("` = ").append(pattern).append(")");
						}
						else if (mode == 0x10)	//数值的精确查询
						{
							sql.append("(`").append(dbfield).append("` = ").append(pattern).append(")");
						}
						else
						{
							continue;
						}
						sql.append(" and ");
					}
				}
			}

			/*range参数 （数值或者日期的范围查询）*/
			if (queryobj.HasMember("range"))
			{
				if (queryobj["range"].IsArray())
				{
					for (int i = 0; i < queryobj["range"].Capacity(); ++i)
					{
						/*字段名*/
						std::string fieldname;
						if (queryobj["range"][i].HasMember("field"))
						{
							if (queryobj["range"][i]["field"].IsString())
							{
								fieldname = queryobj["range"][i]["field"].GetString();
							}
						}
						/*最小值*/
						std::string min_s;
						if (queryobj["range"][i].HasMember("min"))
						{
							if (queryobj["range"][i]["min"].IsInt())
							{
								char buf_min[100] = { 0 };
								sprintf_s(buf_min, "%d", queryobj["range"][i]["min"].GetInt());
								min_s = std::string(buf_min, strlen(buf_min));
							}
							if (queryobj["range"][i]["min"].IsDouble())
							{
								char buf_min[100] = { 0 };
								sprintf_s(buf_min, "%lf", queryobj["range"][i]["min"].GetDouble());
								min_s = std::string(buf_min, strlen(buf_min));
							}
						}
						/*最大值*/
						std::string max_s;
						if (queryobj["range"][i].HasMember("max"))
						{
							if (queryobj["range"][i]["max"].IsInt())
							{
								char buf_max[100] = { 0 };
								sprintf_s(buf_max, "%d", queryobj["range"][i]["max"].GetInt());
								max_s = std::string(buf_max, strlen(buf_max));
							}
							if (queryobj["range"][i]["max"].IsDouble())
							{
								char buf_max[100] = { 0 };
								sprintf_s(buf_max, "%lf", queryobj["range"][i]["max"].GetDouble());
								max_s = std::string(buf_max, strlen(buf_max));
							}
						}
						/*检查field和mode的合法性*/
						std::string dbfield;
						int dateflg = 0;
						auto itr = metamaps.begin();
						for (; itr != metamaps.end(); ++itr)
						{
							if (itr->m_custon_field == fieldname)
							{
								char buf_db_filed[10] = { 0 };
								sprintf_s(buf_db_filed, "%d", itr->m_db_field);
								dbfield = std::string(buf_db_filed, strlen(buf_db_filed));
								if ((itr->m_mode & 0x08) >= 1 || (itr->m_mode & 0x20) >= 1)
								{
									if ((itr->m_mode & 0x08) >= 1)
									{
										dateflg = 1;
									}
									break;
								}
							}
						}
						if (itr == metamaps.end())
						{
							std::string rntStr = "{\"status\": -1, \"msg\": \"请求实体有误\"}";
							response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));
							return 400;
						}
						if (dateflg == 1)
						{
							if (!min_s.empty())
							{
								time_t tm_t;
								tm t_s;
								t_s.tm_year = atoi(min_s.substr(0, 4).c_str()) - 1900;
								t_s.tm_mon = atoi(min_s.substr(5, 2).c_str()) - 1;
								t_s.tm_mday = atoi(min_s.substr(6, 2).c_str());
								t_s.tm_hour = 0;
								t_s.tm_min = 0;
								t_s.tm_sec = 0;
								tm_t = mktime(&t_s);
								char buf_tm1[100] = { 0 };
								sprintf_s(buf_tm1, "%llu", tm_t);
								min_s = std::string(buf_tm1, strlen(buf_tm1));
							}
							if (!max_s.empty())
							{
								time_t tm_t;
								tm t_s;
								t_s.tm_year = atoi(max_s.substr(0, 4).c_str()) - 1900;
								t_s.tm_mon = atoi(max_s.substr(5, 2).c_str()) - 1;
								t_s.tm_mday = atoi(max_s.substr(6, 2).c_str());
								t_s.tm_hour = 0;
								t_s.tm_min = 0;
								t_s.tm_sec = 0;
								tm_t = mktime(&t_s);
								char buf_tm2[100] = { 0 };
								sprintf_s(buf_tm2, "%llu", tm_t);
								max_s = std::string(buf_tm2, strlen(buf_tm2));
							}
						}
						if (min_s.empty() && !max_s.empty())
						{
							sql.append("(`").append(dbfield).append("` <= ").append(max_s).append(")");
							sql.append(" and ");
						}
						else if (!min_s.empty() && max_s.empty())
						{
							sql.append("(`").append(dbfield).append("` >= ").append(min_s).append(")");
							sql.append(" and ");
						}
						else if (!min_s.empty() && !max_s.empty())
						{
							sql.append("(`").append(dbfield).append("` between ").append(min_s).append(" and ").append(max_s).append(")");
							sql.append(" and ");
						}
					}
				}
			}

			/*geometry参数 （地理范围的查询[x,y,x,y,x,y...]）*/
			if (queryobj.HasMember("geometry"))
			{
				if (queryobj["geometry"].IsObject())
				{
					if (queryobj["geometry"].HasMember("polygon"))
					{
						if (queryobj["geometry"]["polygon"].IsArray())
						{
							std::vector<double> x_v;
							std::vector<double> y_v;
							for (int i = 0; i < queryobj["geometry"]["polygon"].Capacity(); ++i)
							{
								if (i % 2 == 0)
								{
									if (queryobj["geometry"]["polygon"][i].IsDouble())
									{
										x_v.push_back(queryobj["geometry"]["polygon"][i].GetDouble());
									}
									if (queryobj["geometry"]["polygon"][i].IsInt())
									{
										x_v.push_back(queryobj["geometry"]["polygon"][i].GetInt());
									}
								}
								else
								{
									if (queryobj["geometry"]["polygon"][i].IsDouble())
									{
										y_v.push_back(queryobj["geometry"]["polygon"][i].GetDouble());
									}
									if (queryobj["geometry"]["polygon"][i].IsInt())
									{
										y_v.push_back(queryobj["geometry"]["polygon"][i].GetInt());
									}
								}
							}
							if (x_v.empty() || y_v.empty())
							{
								std::string rntStr = "{\"status\": -1, \"msg\": \"请求实体polygon有误\"}";  //至少有个x,y
								response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));
								return 400;
							}
							/*求出外包框*/
							std::vector<double>::iterator x_max_itr = std::max_element(x_v.begin(), x_v.end());
							double x_max = *x_max_itr;
							std::vector<double>::iterator x_min_itr = std::min_element(x_v.begin(), x_v.end());
							double x_min = *x_min_itr;
							std::vector<double>::iterator y_max_itr = std::max_element(y_v.begin(), y_v.end());
							double y_max = *y_max_itr;
							std::vector<double>::iterator y_min_itr = std::min_element(y_v.begin(), y_v.end());
							double y_min = *y_min_itr;

							char buf_geometry[100] = { 0 };
							sprintf_s(buf_geometry, "%lf", x_max);
							std::string x_max_s = std::string(buf_geometry, strlen(buf_geometry));
							memset(buf_geometry, 0, sizeof(buf_geometry));
							sprintf_s(buf_geometry, "%lf", x_min);
							std::string x_min_s = std::string(buf_geometry, strlen(buf_geometry));
							memset(buf_geometry, 0, sizeof(buf_geometry));
							sprintf_s(buf_geometry, "%lf", y_max);
							std::string y_max_s = std::string(buf_geometry, strlen(buf_geometry));
							memset(buf_geometry, 0, sizeof(buf_geometry));
							sprintf_s(buf_geometry, "%lf", y_min);
							std::string y_min_s = std::string(buf_geometry, strlen(buf_geometry));

							/*获取数据库中的字段名*/
							auto itr = metamaps.begin();
							std::string dbfiled_max_x, dbfiled_min_x, dbfiled_max_y, dbfiled_min_y;
							for (; itr != metamaps.end(); ++itr)
							{
								if (itr->m_custon_field == "MaxX")
								{
									char buf_max_x[10] = { 0 };
									sprintf_s(buf_max_x, "%d", itr->m_db_field);
									dbfiled_max_x = std::string(buf_max_x, strlen(buf_max_x));
								}
								if (itr->m_custon_field == "MinX")
								{
									char buf_min_x[10] = { 0 };
									sprintf_s(buf_min_x, "%d", itr->m_db_field);
									dbfiled_min_x = std::string(buf_min_x, strlen(buf_min_x));
								}
								if (itr->m_custon_field == "MaxY")
								{
									char buf_max_y[10] = { 0 };
									sprintf_s(buf_max_y, "%d", itr->m_db_field);
									dbfiled_max_y = std::string(buf_max_y, strlen(buf_max_y));
								}
								if (itr->m_custon_field == "MinY")
								{
									char buf_min_y[10] = { 0 };
									sprintf_s(buf_min_y, "%d", itr->m_db_field);
									dbfiled_min_y = std::string(buf_min_y, strlen(buf_min_y));
								}
							}

							/*填写sql查询语句*/
							sql.append("(((`").append(dbfiled_max_x).append("` > ").append(x_min_s).append(" and `").append(dbfiled_max_x).append("` < ").append(x_max_s).append(")");
							sql.append(" and ").append("((`").append(dbfiled_max_y).append("` > ").append(y_min_s).append(" and `").append(dbfiled_max_y).append("` < ").append(y_max_s).append(")");
							sql.append(" or ").append("(`").append(dbfiled_min_y).append("` > ").append(y_min_s).append(" and `").append(dbfiled_min_y).append("` < ").append(y_max_s).append(")))");

							sql.append(" or ");

							sql.append("((`").append(dbfiled_min_x).append("` > ").append(x_min_s).append(" and `").append(dbfiled_min_x).append("` < ").append(x_max_s).append(")");
							sql.append(" and ").append("((`").append(dbfiled_max_y).append("` > ").append(y_min_s).append(" and `").append(dbfiled_max_y).append("` < ").append(y_max_s).append(")");
							sql.append(" or ").append("(`").append(dbfiled_min_y).append("` > ").append(y_min_s).append(" and `").append(dbfiled_min_y).append("` < ").append(y_max_s).append(")))");

							sql.append(" or ");

							sql.append("((`").append(dbfiled_max_y).append("` > ").append(y_min_s).append(" and `").append(dbfiled_max_y).append("` < ").append(y_max_s).append(")");
							sql.append(" and ").append("((`").append(dbfiled_max_x).append("` > ").append(x_min_s).append(" and `").append(dbfiled_max_x).append("` < ").append(x_max_s).append(")");
							sql.append(" or ").append("(`").append(dbfiled_min_x).append("` > ").append(x_min_s).append(" and `").append(dbfiled_min_x).append("` < ").append(x_max_s).append(")))");

							sql.append(" or ");

							sql.append("((`").append(dbfiled_min_y).append("` > ").append(y_min_s).append(" and `").append(dbfiled_min_y).append("` < ").append(y_max_s).append(")");
							sql.append(" and ").append("((`").append(dbfiled_max_x).append("` > ").append(x_min_s).append(" and `").append(dbfiled_max_x).append("` < ").append(x_max_s).append(")");
							sql.append(" or ").append("(`").append(dbfiled_min_x).append("` > ").append(x_min_s).append(" and `").append(dbfiled_min_x).append("` < ").append(x_max_s).append(")))");
						
							sql.append(" or ");

							sql.append("(`").append(dbfiled_min_x).append("` >= ").append(x_min_s).append(" and `").append(dbfiled_max_x).append("` <= ").append(x_max_s);
							sql.append(" and `").append(dbfiled_min_y).append("` >= ").append(y_min_s).append(" and `").append(dbfiled_max_y).append("` <= ").append(y_max_s).append("))");

							sql.append(" and ");
						}
					}
				}
			}
		}
	}
	/*对sql语句结尾部分处理*/
	size_t posand = sql.rfind("and");
	if (posand == sql.size() - 4)
	{
		sql = sql.substr(0, sql.size() - 5); //去掉结尾的and
	}
	else
	{
		sql = sql.substr(0, sql.size() - 6); //去掉结尾的where
	}

	/*size参数，获取几条记录,默认100*/
	int size = 100;
	if (doc1.HasMember("size"))
	{
		if (doc1["size"].IsInt())
		{
			size = doc1["size"].GetInt();
		}
	}
	char buf_size[100] = { 0 };
	sprintf_s(buf_size, "%d", size);
	std::string s_size = std::string(buf_size, strlen(buf_size));
	s_size = " limit " + s_size;
	sql.append(s_size);

	/*from参数，从第几条记录开始获取*/
	int from = 0;
	if (doc1.HasMember("from"))
	{
		if (doc1["from"].IsInt())
		{
			from = doc1["from"].GetInt();
		}
	}
	char buf_from[100] = { 0 };
	sprintf_s(buf_from, "%d", from);
	std::string s_from = std::string(buf_from, strlen(buf_from));
	s_from = " offset " + s_from;
	sql.append(s_from);

	std::cout << sql << std::endl;

	/*执行sql查询，并返回查询结果*/
	if (mysql_real_query(m_mysql, sql.c_str(), sql.size()) != 0)
	{
		printf("mysql_real_query failed: error %u ( %s )",
			mysql_errno(m_mysql), mysql_error(m_mysql));
		return -1;
	}
	MYSQL_RES *result = mysql_store_result(m_mysql);
	if (result == NULL)
	{
		printf("mysql_store_result failed: error %u ( %s )",
			mysql_errno(m_mysql), mysql_error(m_mysql));
		return 200;
	}

	/*返回了多少行*/
	unsigned long long rows = mysql_num_rows(result);
	std::cout << rows << std::endl;
	char bufrows[100] = { 0 };
	sprintf_s(bufrows, "%llu", rows);
	std::string rowstr = std::string(bufrows, strlen(bufrows));
	if (rows < 1)
	{
		std::string rntStr = "{\"msg\": \"未找到对应的记录\"}";
		response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));
		return 404;
	}

	MYSQL_ROW rown; //一行数据的类型安全表示
	std::string rntstr = "{\"urls\": [ ";
	response_body.insert(response_body.end(), (uint8_t*)rntstr.data(), (uint8_t*)(rntstr.data() + rntstr.size()));
	int numrow = 0;
	while ((rown = mysql_fetch_row(result)) != NULL)
	{
		numrow++;
		for (int i = 0; i < mysql_num_fields(result); ++i)
		{
			/*MYSQL_FIELD *field = mysql_fetch_field_direct(result, i);
			rntstr = field->name;
			for (auto itr = metamaps.begin(); itr != metamaps.end(); ++itr)
			{
				if (atoi(rntstr.c_str()) == itr->m_db_field)
				{
					char buf_db[10] = { 0 };
					sprintf_s(buf_db, "%d", itr->m_db_field);
					rntstr = std::string(buf_db, strlen(buf_db));
				}
			}*/
			std::string rows_s = rown[i];
			
			rntstr = "\"" + rows_s + "\"";
		}
		if (numrow < rows)
		{
			rntstr.append(", ");
		}
		response_body.insert(response_body.end(), (uint8_t*)rntstr.data(), (uint8_t*)(rntstr.data() + rntstr.size()));
	}
	rntstr = " ] }";
	response_body.insert(response_body.end(), (uint8_t*)rntstr.data(), (uint8_t*)(rntstr.data() + rntstr.size()));

	mysql_free_result(result);
	
	if (m_mysql != NULL)
	{
		mysql_close(m_mysql);
	}

	return 200;
}




int us_cms_http_post_rebuild(
	const std::string& path,
	const std::multimap< std::string, std::string >& headers,
	const std::multimap< std::string, std::string >& query_parameters,
	const std::map< std::string, std::string >& path_parameters,
	const std::vector<uint8_t>& request_body,
	const std::string& dest_endpoint,
	std::multimap< std::string, std::string >& response_headers,
	std::vector<uint8_t>& response_body)
{
	response_headers.insert(std::make_pair(std::string("Content-type"), std::string("text/json")));

	/*连接数据库*/
	std::string hostip;
	int port;
	std::string user;
	std::string password;
	std::string database;
	std::string tablename;

	/*读取配置文件（json文件）获取ip和端口以及数据库名等*/
	rapidjson::Document doc_cfg;
	FILE *myFile = NULL;
	fopen_s(&myFile, "./ngcc_metadata_config.json", "rb");
	if (myFile)
	{
		char buf_cfg[4096] = { 0 };
		rapidjson::FileReadStream inputStream(myFile, buf_cfg, sizeof(buf_cfg));  //创建一个输入流
		doc_cfg.ParseStream(inputStream); //将读取的内容转换为dom元素
		fclose(myFile); //关闭文件，很重要
	}
	else
	{
		std::string rntStr = "{\"status\": -1, \"msg\": \"找不到配置文件\"}";
		response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));
		return 400;
	}
	if (doc_cfg.HasParseError())
	{
		std::string rntStr = "{\"status\": -1, \"msg\": \"配置文件中的json格式不正确\"}";
		response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));
		return 400;
	}
	if (doc_cfg.HasMember("hostip"))
	{
		if (!doc_cfg["hostip"].IsString())
		{
			hostip = doc_cfg["hostip"].GetString();
		}
	}
	if (doc_cfg.HasMember("port"))
	{
		if (doc_cfg["port"].IsInt())
		{
			port = doc_cfg["port"].GetInt();
		}
	}
	if (doc_cfg.HasMember("user"))
	{
		if (doc_cfg["user"].IsString())
		{
			user = doc_cfg["user"].GetString();
		}
	}
	if (doc_cfg.HasMember("password"))
	{
		if (doc_cfg["password"].IsString())
		{
			password = doc_cfg["password"].GetString();
		}
	}
	if (doc_cfg.HasMember("database"))
	{
		if (doc_cfg["database"].IsString())
		{
			database = doc_cfg["database"].GetString();
		}
	}
	if (doc_cfg.HasMember("tablename"))
	{
		if (doc_cfg["tablename"].IsString())
		{
			tablename = doc_cfg["tablename"].GetString();
		}
	}

	/*初始化mysql*/
	MYSQL * m_mysql = mysql_init(NULL);  //在程序使用Mysql的最开始必须调用mysql_int()初始化
	if (m_mysql == NULL)
	{
		std::string rntStr = "{\"status\": -1, \"msg\": \"mysql init failed\"}";
		response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));
		return 500;
	}
	/*连接数据库服务器*/
	int arg = 1;
	mysql_options(m_mysql, MYSQL_OPT_RECONNECT, &arg);

	if (mysql_real_connect(m_mysql, hostip.c_str(),
		user.c_str(), password.c_str(), database.c_str(), port, NULL, 0) == NULL)
	{
		std::string rntStr = "{\"status\": -1, \"msg\": \"mysql_real_connect failed\"}";
		response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));
		return 500;
	}

	/*获取字段映射表*/
	int out_size = 0;
	const us_cms_metdata_map *metamap = us_get_cms_metadata_map_table(&out_size);
	std::vector<us_cms_metdata_map> metamaps;
	metamaps.reserve(out_size);
	if (metamap != nullptr)
	{
		for (int i = 0; i < out_size; ++i)
		{
			metamaps.push_back(*metamap);
			metamap++;
		}
	}
	else
	{
		std::string rntStr = "{\"status\": -1, \"msg\": \"获取字段映射表失败\"}";
		response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));
		return 500;
	}

	/*清空元数据存储表*/
	std::string sql_clear = "truncate table " + tablename;
	if (mysql_real_query(m_mysql, sql_clear.c_str(), sql_clear.size()) != 0)
	{
		std::string rntStr = "{\"status\": -1, \"msg\": \"清空元数据查记录表失败\"}";
		response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));
		return 500;
	}

	/*读取image元数据表中的全部记录，重建用户元数据的存储记录*/
	std::string sql_read = "select * from image";
	if (mysql_real_query(m_mysql, sql_read.c_str(), sql_read.size()) != 0)
	{
		std::string rntStr = "{\"status\": -1, \"msg\": \"读取image表失败\"}";
		response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));
		return 500;
	}
	MYSQL_RES *result = mysql_store_result(m_mysql);
	if (result == NULL)
	{
		std::string rntStr = "{\"status\": -1, \"msg\": \"mysql_store_result failed\"}";
		response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));
		return 500;
	}
	/*返回了多少行，如果小于1，则image元数据表为空，不能重建*/
	unsigned long long rows = mysql_num_rows(result);
	if (rows < 1)
	{
		std::string rntStr = "{\"status\": -1, \"msg\": \"image 元数据表为空\"}";
		response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));
		return 500;
	}
	MYSQL_ROW rown; //一行数据的类型安全表示
	while ((rown = mysql_fetch_row(result)) != NULL)
	{
		std::string fd_ids_str;
		std::string out_json;
		std::string fd_url_str;
		/*读取image元数据表的id和url字段，并通过url字段的值（先按xml路径处理）获取到元数据的json格式*/
		for (int i = 0; i < mysql_num_fields(result); ++i)
		{
			MYSQL_FIELD *field = mysql_fetch_field_direct(result, i);
			std::string fd_name = field->name;
			if (fd_name == "ids")
			{
				fd_ids_str = rown[i];
			}
			if (fd_name == "url")
			{
				fd_url_str = rown[i];
				int ret_read = us_read_cms_metadata_record(fd_url_str, &out_json);
				if (ret_read < 0)
				{
					std::string rntStr = "{\"status\": -1, \"msg\": \"无法从url获取元数据\"}";
					response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));
					return 500;
				}
			}
		}
		
		//std::string sql_delete = "delete from " + tablename + " where `ids` = " + fd_ids_str;
		//if (mysql_real_query(m_mysql, sql_delete.c_str(), sql_delete.size()) != 0)
		//{
		//	std::string rntStr = "{\"status\": -1, \"msg\": \"delete image metadata failed\"}";
		//	response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));
		//	return 500;
		//}
		/*插入数据（将id，url，status（插入元数据则它记为0），元数据全部插入）*/
		std::string sql_insert = "insert into " + tablename + "(`ids`, `url`, `status`, ";
		for (int i = 1; i < out_size + 1; i++)
		{
			std::string buf_;
			buf_.resize(3);
			char buff_[3] = { 0 };
			sprintf_s(buff_, "%d", i);
			int ret_ = mysql_real_escape_string(m_mysql, (char*)buf_.data(), buff_, strlen(buff_));
			sql_insert.append("`").append(buf_.data(), ret_).append("`, ");
		}
		sql_insert = sql_insert.substr(0, sql_insert.size() - 2);
		sql_insert.append(") values('").append(fd_ids_str).append("', '").append(fd_url_str).append("', ").append("0, ");
		for (int i = 1; i < out_size + 1; i++)
		{
			char buf_colname[10] = { 0 };
			sprintf_s(buf_colname, "%d", i);
			std::string colname = std::string(buf_colname, strlen(buf_colname));

			rapidjson::Document doc1;
			doc1.Parse(out_json.c_str());
			if (doc1.HasParseError())
			{
				std::string rntStr = "{\"status\": -1, \"msg\": \"获取的元数据json格式有误\"}";
				response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));
				return 500;
			}
			std::string value_db = "";
			if (doc1.HasMember(colname.c_str()))
			{
				if (!doc1[colname.c_str()].IsNull())
				{	/*只有者三种类型*/
					if (doc1[colname.c_str()].IsInt())
					{
						int colv = doc1[colname.c_str()].GetInt();
						char buf1[100] = { 0 };
						sprintf_s(buf1, "%d", colv);
						value_db = std::string(buf1, strlen(buf1));
						sql_insert.append(value_db).append(", ");
					}
					else if (doc1[colname.c_str()].IsDouble())
					{
						double colv = doc1[colname.c_str()].GetDouble();
						char buf1[100] = { 0 };
						sprintf_s(buf1, "%lf", colv);
						value_db = std::string(buf1, strlen(buf1));
						sql_insert.append(value_db).append(", ");
					}
					else if (doc1[colname.c_str()].IsString())
					{
						value_db = doc1[colname.c_str()].GetString();
						sql_insert.append("'").append(value_db).append("', ");
					}
				}
			}
			else
			{
				std::string rntStr = "{\"status\": -1, \"msg\": \"从xml中解析出的json有误\"}";
				response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));
				return 500;
			}
		}

		sql_insert = sql_insert.substr(0, sql_insert.size() - 2);
		sql_insert.append(")");
		if (mysql_real_query(m_mysql, sql_insert.c_str(), sql_insert.size()) != 0)
		{
			std::string rntStr = "{\"status\": -1, \"msg\": \"插入元数据记录失败\"}";
			response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));
			return 500;
		}
	}

	mysql_free_result(result);

	if (m_mysql != NULL)
	{
		mysql_close(m_mysql);
	}

	return 200;
}


int us_cms_http_get_getm(
	const std::string& path,
	const std::multimap< std::string, std::string >& headers,
	const std::multimap< std::string, std::string >& query_parameters,
	const std::map< std::string, std::string >& path_parameters,
	const std::vector<uint8_t>& request_body,
	const std::string& dest_endpoint,
	std::multimap< std::string, std::string >& response_headers,
	std::vector<uint8_t>& response_body)
{
	response_headers.insert(std::make_pair(std::string("Content-type"), std::string("text/json")));

	/*连接数据库*/
	std::string hostip;
	int port;
	std::string user;
	std::string password;
	std::string database;
	std::string tablename;

	/*读取配置文件（json文件）获取ip和端口以及数据库名等*/
	rapidjson::Document doc_cfg;
	FILE *myFile = NULL;
	fopen_s(&myFile, "./ngcc_metadata_config.json", "rb");
	if (myFile)
	{
		char buf_cfg[4096] = { 0 };
		rapidjson::FileReadStream inputStream(myFile, buf_cfg, sizeof(buf_cfg));  //创建一个输入流
		doc_cfg.ParseStream(inputStream); //将读取的内容转换为dom元素
		fclose(myFile); //关闭文件，很重要
	}
	else
	{
		std::string rntStr = "{\"status\": -1, \"msg\": \"can't find config file\"}";
		response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));
		return 400;
	}
	if (doc_cfg.HasParseError())
	{
		std::string rntStr = "{\"status\": -1, \"msg\": \"config json is error\"}";
		response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));
		return 400;
	}
	if (doc_cfg.HasMember("hostip"))
	{
		if (!doc_cfg["hostip"].IsString())
		{
			hostip = doc_cfg["hostip"].GetString();
		}
	}
	if (doc_cfg.HasMember("port"))
	{
		if (doc_cfg["port"].IsInt())
		{
			port = doc_cfg["port"].GetInt();
		}
	}
	if (doc_cfg.HasMember("user"))
	{
		if (doc_cfg["user"].IsString())
		{
			user = doc_cfg["user"].GetString();
		}
	}
	if (doc_cfg.HasMember("password"))
	{
		if (doc_cfg["password"].IsString())
		{
			password = doc_cfg["password"].GetString();
		}
	}
	if (doc_cfg.HasMember("database"))
	{
		if (doc_cfg["database"].IsString())
		{
			database = doc_cfg["database"].GetString();
		}
	}
	if (doc_cfg.HasMember("tablename"))
	{
		if (doc_cfg["tablename"].IsString())
		{
			tablename = doc_cfg["tablename"].GetString();
		}
	}

	MYSQL * m_mysql = mysql_init(NULL);  //在程序使用Mysql的最开始必须调用mysql_int()初始化
	if (m_mysql == NULL)
	{
		std::string rntStr = "{\"status\": -1, \"msg\": \"mysql init failed\"}";
		response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));
		return 500;
	}
	int arg = 1;
	mysql_options(m_mysql, MYSQL_OPT_RECONNECT, &arg);

	if (mysql_real_connect(m_mysql, hostip.c_str(),
		user.c_str(), password.c_str(), database.c_str(), port, NULL, 0) == NULL)
	{
		std::string rntStr = "{\"status\": -1, \"msg\": \"mysql_real_connect failed\"}";
		response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));
		return 500;
	}

	/*获取字段映射表*/
	int out_size = 0;
	const us_cms_metdata_map *metamap = us_get_cms_metadata_map_table(&out_size);
	std::vector<us_cms_metdata_map> metamaps;
	metamaps.reserve(out_size);
	if (metamap != nullptr)
	{
		for (int i = 0; i < out_size; ++i)
		{
			metamaps.push_back(*metamap);
			metamap++;
		}
	}
	else
	{
		std::string rntStr = "{\"status\": -1, \"msg\": \"获取字段映射表失败\"}";
		response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));
		return 500;
	}

	/*读取请求实体*/
	std::string jsonstr;
	for (auto itr = request_body.begin(); itr != request_body.end(); ++itr)
	{
		jsonstr.push_back(*itr);
	}
	rapidjson::Document doc1;
	doc1.Parse(jsonstr.c_str());
	if (doc1.HasParseError())
	{
		std::string rntStr = "{\"status\": -1, \"msg\": \"请求实体json格式不正确\"}";
		response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));
		return 400;
	}
	/*id和url参数*/
	std::vector<std::string> vect_ids;
	std::vector<std::string> vect_urls;
	if (doc1.HasMember("ids"))
	{
		if (doc1["ids"].IsArray())
		{
			for (int i = 0; i < doc1["ids"].Capacity(); ++i)
			{
				if (doc1["ids"][i].IsString())
				{
					vect_ids.push_back(doc1["ids"][i].GetString());
				}
			}
		}
	}
	if (doc1.HasMember("urls"))
	{
		if (doc1["urls"].IsArray())
		{
			for (int i = 0; i < doc1["urls"].Capacity(); ++i)
			{
				if (doc1["urls"][i].IsString())
				{
					vect_urls.push_back(doc1["urls"][i].GetString());
				}
			}
		}
	}

	std::string sql = "select ";
	for (int i = 1; i <= out_size; ++i)
	{
		char buf_i[10] = { 0 };
		sprintf_s(buf_i, "%d", i);
		std::string i_str = std::string(buf_i, strlen(buf_i));
		sql.append("`").append(i_str).append("`, ");
	}
	sql = sql.substr(0, sql.size() - 2);
	sql.append(" from ").append(tablename);

	if (!vect_ids.empty() || !vect_urls.empty())
	{
		sql.append(" where ");
		if (!vect_ids.empty())
		{
			for (auto itr = vect_ids.begin(); itr != vect_ids.end(); ++itr)
			{
				if (itr->empty())
				{
					continue;
				}
				sql.append("`ids` = ").append(*itr).append(" or ");
			}
		}
		if (!vect_urls.empty())
		{
			for (auto itr = vect_urls.begin(); itr != vect_urls.end(); ++itr)
			{
				if (itr->empty())
				{
					continue;
				}
				sql.append("`url` = '").append(*itr).append("' or ");
			}
		}
		sql = sql.substr(0, sql.size() - 4);
	}

	/*执行sql查询，并返回查询结果*/
	if (mysql_real_query(m_mysql, sql.c_str(), sql.size()) != 0)
	{
		std::string rntStr = "{\"status\": -1, \"msg\": \"mysql_real_query failed\"}";
		response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));
		return 500;
	}
	MYSQL_RES *result = mysql_store_result(m_mysql);
	if (result == NULL)
	{
		std::string rntStr = "{\"status\": -1, \"msg\": \"mysql_store_result failed\"}";
		response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));
		return 500;
	}

	/*返回了多少行*/
	unsigned long long rows = mysql_num_rows(result);
	std::cout << rows << std::endl;
	char bufrows[100] = { 0 };
	sprintf_s(bufrows, "%llu", rows);
	std::string rowstr = std::string(bufrows, strlen(bufrows));
	if (rows < 1)
	{
		std::string rntStr = "{\"status\": -1,\"msg\": \"未找到对应的记录\"}";
		response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));
		return 404;
	}

	MYSQL_ROW rown; //一行数据的类型安全表示
	std::string rntstr = "{ \"count\": " + rowstr + ", \"meta_list\": [ ";
	response_body.insert(response_body.end(), (uint8_t*)rntstr.data(), (uint8_t*)(rntstr.data() + rntstr.size()));
	int numrow = 0;
	while ((rown = mysql_fetch_row(result)) != NULL)
	{
		numrow++;
		rntstr = "{ ";
		response_body.insert(response_body.end(), (uint8_t*)rntstr.data(), (uint8_t*)(rntstr.data() + rntstr.size()));
		for (int i = 0; i < mysql_num_fields(result); ++i)
		{
			MYSQL_FIELD *field = mysql_fetch_field_direct(result, i);
			std::string fd_db_name = field->name;
			std::string fd_name;
			for (auto itr = metamaps.begin(); itr != metamaps.end(); ++itr)
			{
				if (itr->m_db_field == atoi(fd_db_name.c_str()))
				{
					fd_name = itr->m_custon_field;
				}
			}
			rntstr = fd_name;
			std::string fd_s = rown[i];
			if (rntstr == "ProduceDate" || rntstr == "PbandDate" ||
				rntstr == "MultiBandDate" || rntstr == "InstituteCheckDate" || rntstr == "BureauCheckDate")
			{
				time_t tm_t = atoi(rown[i]);
				struct tm ptr_tm;
				errno_t ret_tm = localtime_s(&ptr_tm, &tm_t);
				char buf_s[10];
				if (rntstr == "ProduceDate")
				{
					strftime(buf_s, 10, "%Y%m", &ptr_tm);
				}
				else
				{
					strftime(buf_s, 10, "%Y%m%d", &ptr_tm);
				}
				fd_s = std::string(buf_s, strlen(buf_s));
			}
			if (rntstr == "GroundResolution" || rntstr == "SateResolution" ||
				rntstr == "MultiBandResolution" || rntstr == "ImgSize")
			{
				float resolution = atof(rown[i]);
				char resolution_s[100] = { 0 };
				sprintf_s(resolution_s, "%.1f", resolution);
				fd_s = std::string(resolution_s, strlen(resolution_s));
				if (i == mysql_num_fields(result) - 1)
				{
					rntstr = "\"" + rntstr + "\": " + fd_s;
				}
				else
				{
					rntstr = "\"" + rntstr + "\": " + fd_s + ", ";
				}
				response_body.insert(response_body.end(), (uint8_t*)rntstr.data(), (uint8_t*)(rntstr.data() + rntstr.size()));
				continue;
			}
			if (rntstr == "MaxX" || rntstr == "MinX" || rntstr == "MaxY" || rntstr == "MinY")
			{
				continue;
			}
			if (rntstr == "ImgRange")
			{
				std::string range_s = rown[i];
				range_s.append(", ");
				do
				{
					size_t pos_spce = range_s.find_first_of(", ");
					std::string sig_range_s = range_s.substr(0, pos_spce);
					size_t pos_colon = sig_range_s.find_first_of(":");
					std::string key_s = sig_range_s.substr(0, pos_colon);
					std::string value_s = sig_range_s.substr(pos_colon + 1);
					double value_s_n = atof(value_s.c_str());
					char buf_range[100] = { 0 };
					sprintf_s(buf_range, "%.2f", value_s_n);
					fd_s = std::string(buf_range, strlen(buf_range));
					if (i == mysql_num_fields(result) - 1)
					{
						rntstr = "\"" + key_s + "\": " + fd_s;
					}
					else
					{
						rntstr = "\"" + key_s + "\": " + fd_s + ", ";
					}
					response_body.insert(response_body.end(), (uint8_t*)rntstr.data(), (uint8_t*)(rntstr.data() + rntstr.size()));
					range_s = range_s.substr(pos_spce + 2);
				} while (!range_s.empty());
				continue;
			}
			if (rntstr == "PixelBits" || rntstr == "CentralMederian" ||
				rntstr == "GaussKrugerZoneNo" || rntstr == "MultiBandNum" ||
				rntstr == "GridInterval")
			{
				if (i == mysql_num_fields(result) - 1)
				{
					rntstr = "\"" + rntstr + "\": " + fd_s;
				}
				else
				{
					rntstr = "\"" + rntstr + "\": " + fd_s + ", ";
				}
				response_body.insert(response_body.end(), (uint8_t*)rntstr.data(), (uint8_t*)(rntstr.data() + rntstr.size()));
				continue;
			}
			if (rntstr == "SateOriXRMS" || rntstr == "SateOriYRMS" || rntstr == "SateOriZRMS" ||
				rntstr == "WestMosaicMaxError" || rntstr == "NorthMosaicMaxError" || rntstr == "EastMosaicMaxError" ||
				rntstr == "SouthMosaicMaxError" || rntstr == "MultiBRectifyXRMS" || rntstr == "MultiBRectifyYRMS" ||
				rntstr == "CheckRMS" || rntstr == "CheckMAXErr")
			{
				double dobl = atof(rown[i]);
				char dobl_s[100] = { 0 };
				sprintf_s(dobl_s, "%.2f", dobl);
				fd_s = std::string(dobl_s, strlen(dobl_s));
				if (i == mysql_num_fields(result) - 1)
				{
					rntstr = "\"" + rntstr + "\": " + fd_s;
				}
				else
				{
					rntstr = "\"" + rntstr + "\": " + fd_s + ", ";
				}
				response_body.insert(response_body.end(), (uint8_t*)rntstr.data(), (uint8_t*)(rntstr.data() + rntstr.size()));
				continue;
			}

			if (i == mysql_num_fields(result) - 1)
			{
				rntstr = "\"" + rntstr + "\": " + "\"" + fd_s + "\"";
			}
			else
			{
				rntstr = "\"" + rntstr + "\": " + "\"" + fd_s + "\", ";
			}
			response_body.insert(response_body.end(), (uint8_t*)rntstr.data(), (uint8_t*)(rntstr.data() + rntstr.size()));
		}
		if (numrow == rows)
		{
			rntstr = " }";
		}
		else
		{
			rntstr = " }, ";
		}
		response_body.insert(response_body.end(), (uint8_t*)rntstr.data(), (uint8_t*)(rntstr.data() + rntstr.size()));
	}
	rntstr = " ] }";
	response_body.insert(response_body.end(), (uint8_t*)rntstr.data(), (uint8_t*)(rntstr.data() + rntstr.size()));

	mysql_free_result(result);

	if (m_mysql != NULL)
	{
		mysql_close(m_mysql);
	}

	return 200;
}


int us_cms_http_post_refresh(
	const std::string& path,
	const std::multimap< std::string, std::string >& headers,
	const std::multimap< std::string, std::string >& query_parameters,
	const std::map< std::string, std::string >& path_parameters,
	const std::vector<uint8_t>& request_body,
	const std::string& dest_endpoint,
	std::multimap< std::string, std::string >& response_headers,
	std::vector<uint8_t>& response_body)
{
	response_headers.insert(std::make_pair(std::string("Content-type"), std::string("text/json")));

	/*连接数据库*/
	std::string hostip;
	int port;
	std::string user;
	std::string password;
	std::string database;
	std::string tablename;

	/*读取配置文件（json文件）获取ip和端口以及数据库名等*/
	rapidjson::Document doc_cfg;
	FILE *myFile = NULL;
	fopen_s(&myFile, "./ngcc_metadata_config.json", "rb");
	if (myFile)
	{
		char buf_cfg[4096] = { 0 };
		rapidjson::FileReadStream inputStream(myFile, buf_cfg, sizeof(buf_cfg));  //创建一个输入流
		doc_cfg.ParseStream(inputStream); //将读取的内容转换为dom元素
		fclose(myFile); //关闭文件，很重要
	}
	else
	{
		std::string rntStr = "{\"status\": -1, \"msg\": \"can't find config file\"}";
		response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));
		return 400;
	}
	if (doc_cfg.HasParseError())
	{
		std::string rntStr = "{\"status\": -1, \"msg\": \"config json is error\"}";
		response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));
		return 400;
	}
	if (doc_cfg.HasMember("hostip"))
	{
		if (!doc_cfg["hostip"].IsString())
		{
			hostip = doc_cfg["hostip"].GetString();
		}
	}
	if (doc_cfg.HasMember("port"))
	{
		if (doc_cfg["port"].IsInt())
		{
			port = doc_cfg["port"].GetInt();
		}
	}
	if (doc_cfg.HasMember("user"))
	{
		if (doc_cfg["user"].IsString())
		{
			user = doc_cfg["user"].GetString();
		}
	}
	if (doc_cfg.HasMember("password"))
	{
		if (doc_cfg["password"].IsString())
		{
			password = doc_cfg["password"].GetString();
		}
	}
	if (doc_cfg.HasMember("database"))
	{
		if (doc_cfg["database"].IsString())
		{
			database = doc_cfg["database"].GetString();
		}
	}
	if (doc_cfg.HasMember("tablename"))
	{
		if (doc_cfg["tablename"].IsString())
		{
			tablename = doc_cfg["tablename"].GetString();
		}
	}

	MYSQL * m_mysql = mysql_init(NULL);  //在程序使用Mysql的最开始必须调用mysql_int()初始化
	if (m_mysql == NULL)
	{
		std::string rntStr = "{\"status\": -1, \"msg\": \"mysql init failed\"}";
		response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));
		return 500;
	}
	int arg = 1;
	mysql_options(m_mysql, MYSQL_OPT_RECONNECT, &arg);

	if (mysql_real_connect(m_mysql, hostip.c_str(),
		user.c_str(), password.c_str(), database.c_str(), port, NULL, 0) == NULL)
	{
		std::string rntStr = "{\"status\": -1, \"msg\": \"mysql_real_connect failed\"}";
		response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));
		return 500;
	}

	/*获取字段映射表*/
	int out_size = 0;
	const us_cms_metdata_map *metamap = us_get_cms_metadata_map_table(&out_size);
	std::vector<us_cms_metdata_map> metamaps;
	metamaps.reserve(out_size);
	if (metamap != nullptr)
	{
		for (int i = 0; i < out_size; ++i)
		{
			metamaps.push_back(*metamap);
			metamap++;
		}
	}
	else
	{
		std::string rntStr = "{\"status\": -1, \"msg\": \"获取字段映射表失败\"}";
		response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));
		return 500;
	}

	/*查找status为1对应的url*/
	std::string sql_search_status = "select ids, url from " + tablename + " where status = 1";
	if (mysql_real_query(m_mysql, sql_search_status.c_str(), sql_search_status.size()) != 0)
	{
		std::string rntStr = "{\"status\": -1, \"msg\": \"mysql_real_query failed\"}";
		response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));
		return 500;
	}

	/*获取url并读取内容插入元数据记录表*/
	MYSQL_RES *result = mysql_store_result(m_mysql);
	if (result == NULL)
	{
		printf("mysql_store_result failed: error %u ( %s )",
			mysql_errno(m_mysql), mysql_error(m_mysql));
		return 500;
	}
	unsigned long long rows = mysql_num_rows(result);
	if (rows < 1)
	{
		std::string rntStr = "{\"msg\": \"数据无更新\"}";
		response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));
		return 200;
	}
	MYSQL_ROW rown; //一行数据的类型安全表示
	while ((rown = mysql_fetch_row(result)) != NULL)
	{
		int status_flg = 0;
		std::string url_s;
		std::string ids_s;
		for (int i = 0; i < mysql_num_fields(result); ++i)
		{
			MYSQL_FIELD *field = mysql_fetch_field_direct(result, i);
			std::string fd_name = field->name;
			std::string fd_value = rown[i];
			if (fd_name == "url")
			{
				url_s = fd_value;
			}
			if (fd_name == "ids")
			{
				ids_s = fd_value;
			}
		}
		/*插入元数据*/
		std::string sql_update_insert = "update " + tablename + " set status = 0, ";
		for (int i_v = 1; i_v <= out_size; ++i_v)
		{
			char buf_iv[10] = { 0 };
			sprintf_s(buf_iv, "%d", i_v);
			std::string i_v_s = std::string(buf_iv, strlen(buf_iv));
			std::string jsonstr;
			int ret_js = us_read_cms_metadata_record(url_s, &jsonstr);
			rapidjson::Document doc1;
			doc1.Parse(jsonstr.c_str());
			if (doc1.HasParseError())
			{
				std::string rntStr = "{\"status\": -1, \"msg\": \"从url中获取的json格式不正确\"}";
				response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));
				return 400;
			}
			sql_update_insert.append("`").append(i_v_s).append("` = ").append("");

			std::string value_db;
			if (doc1.HasMember(i_v_s.c_str()))
			{
				if (!doc1[i_v_s.c_str()].IsNull())
				{	/*只有者三种类型*/
					if (doc1[i_v_s.c_str()].IsInt())
					{
						int colv = doc1[i_v_s.c_str()].GetInt();
						char buf1[100] = { 0 };
						sprintf_s(buf1, "%d", colv);
						value_db = std::string(buf1, strlen(buf1));
						sql_update_insert.append(value_db).append(", ");
					}
					else if (doc1[i_v_s.c_str()].IsDouble())
					{
						double colv = doc1[i_v_s.c_str()].GetDouble();
						char buf1[100] = { 0 };
						sprintf_s(buf1, "%lf", colv);
						value_db = std::string(buf1, strlen(buf1));
						sql_update_insert.append(value_db).append(", ");
					}
					else if (doc1[i_v_s.c_str()].IsString())
					{
						value_db = doc1[i_v_s.c_str()].GetString();
						sql_update_insert.append("'").append(value_db).append("', ");
					}
				}
			}
		}
		sql_update_insert = sql_update_insert.substr(0, sql_update_insert.size() - 2);
		sql_update_insert.append(" where ids = ").append(ids_s);
		if (mysql_real_query(m_mysql, sql_update_insert.c_str(), sql_update_insert.size()) != 0)
		{
			std::string rntStr = "{\"status\": -1, \"msg\": \"mysql_real_query failed\"}";
			response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));
			return 500;
		}
	}
		
	mysql_free_result(result);

	if (m_mysql != NULL)
	{
		mysql_close(m_mysql);
	}

	return 200;
}
