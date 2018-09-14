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
#include <thread>
#include <mutex>

#include <geos/io/WKTReader.h>
#include <geos/geom/Polygon.h>

#include <data_storage/us_data_storage_manager.hpp>


int us_cms_http_get_search2(
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
	std::string hostip = "localhost";
	int port = 3306;
	std::string user = "root";
	std::string password = "";
	std::string database = "metadata2";
	std::string tablename = "ngcc_metadata";

	///*读取配置文件（json文件）获取ip和端口以及数据库名等*/
	//rapidjson::Document doc_cfg;
	//FILE *myFile = NULL;
	//fopen_s(&myFile, "./ngcc_metadata_config.json", "rb");
	//if (myFile) 
	//{
	//	char buf_cfg[4096] = { 0 };
	//	rapidjson::FileReadStream inputStream(myFile, buf_cfg, sizeof(buf_cfg));  //创建一个输入流
	//	doc_cfg.ParseStream(inputStream); //将读取的内容转换为dom元素
	//	fclose(myFile); //关闭文件，很重要
	//}
	//else 
	//{
	//	std::string rntStr = "{\"status\": -1, \"msg\": \"can't find config file\"}";
	//	response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));
	//	return 400;
	//}
	//if (doc_cfg.HasParseError())
	//{
	//	std::string rntStr = "{\"status\": -1, \"msg\": \"config json is error\"}";
	//	response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));
	//	return 400;
	//}
	//if (doc_cfg.HasMember("hostip"))
	//{
	//	if (!doc_cfg["hostip"].IsString())
	//	{
	//		hostip = doc_cfg["hostip"].GetString();
	//	}
	//}
	//if (doc_cfg.HasMember("port"))
	//{
	//	if (doc_cfg["port"].IsInt())
	//	{
	//		port = doc_cfg["port"].GetInt();
	//	}
	//}
	//if (doc_cfg.HasMember("user"))
	//{
	//	if (doc_cfg["user"].IsString())
	//	{
	//		user = doc_cfg["user"].GetString();
	//	}
	//}
	//if (doc_cfg.HasMember("password"))
	//{
	//	if (doc_cfg["password"].IsString())
	//	{
	//		password = doc_cfg["password"].GetString();
	//	}
	//}
	//if (doc_cfg.HasMember("database"))
	//{
	//	if (doc_cfg["database"].IsString())
	//	{
	//		database = doc_cfg["database"].GetString();
	//	}
	//}
	//if (doc_cfg.HasMember("tablename"))
	//{
	//	if (doc_cfg["tablename"].IsString())
	//	{
	//		tablename = doc_cfg["tablename"].GetString();
	//	}
	//}
	
	/*连接数据库*/
	MYSQL * m_mysql = mysql_init(NULL);  //在程序使用Mysql的最开始必须调用mysql_int()初始化
	if (m_mysql == NULL)
	{
		std::string rntStr = u8"{\"status\": 1, \"msg\": \"mysql init failed\"}";
		response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));
		return 500;
	}
	int arg = 1;
	mysql_options(m_mysql, MYSQL_OPT_RECONNECT, &arg);  //断开重连

	if (mysql_real_connect(m_mysql, hostip.c_str(),
		user.c_str(), password.c_str(), database.c_str(), port, NULL, 0) == NULL)
	{
		std::string rntStr =u8"{\"status\": 2, \"msg\": \"mysql_real_connect failed\"}";
		response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));
		return 500;
	}

	/*获取字段映射表*/
	int out_size = 0;
	const us_cms_metdata_map *metamap = us_get_cms_metadata_map_table(&out_size);
	if (metamap == nullptr || out_size < 0)
	{
		std::string rntStr = u8"{\"status\": 3, \"msg\": \"获取字段映射表失败\"}";
		response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));
		return 500;
	}

	std::vector<us_cms_metdata_map> metamaps;
	metamaps.reserve(out_size);
	for (int i = 0; i < out_size; ++i)
	{
		metamaps.push_back(*metamap);
		metamap++;
	}

	/*sql语句，获取符合条件的url字段*/
	std::string sql = "select `url`";
	sql.append(" from ").append(tablename).append(" where ");

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
		std::string rntStr = u8"{\"status\": 4, \"msg\": \"请求实体json格式不正确\"}";
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
					for (int i = 0; i < queryobj["match"].Size(); ++i)
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
							if (queryobj["match"][i]["mode"].IsInt64())
							{
								mode = queryobj["match"][i]["mode"].GetInt64();
							}
						}
						/*获取匹配串*/
						std::string pattern;
						if (queryobj["match"][i].HasMember("pattern"))
						{
							if (queryobj["match"][i]["pattern"].IsString()) //字符串类型
							{
								pattern = queryobj["match"][i]["pattern"].GetString();
							}
							if (queryobj["match"][i]["pattern"].IsInt64())  //整数类型
							{
								int pattern_int = queryobj["match"][i]["pattern"].GetInt64();
								pattern = std::to_string(pattern_int);
							}
							if (queryobj["match"][i]["pattern"].IsDouble())  //浮点数类型
							{
								double pattern_db = queryobj["match"][i]["pattern"].GetDouble();
								pattern = std::to_string(pattern_db);
							}
						}
						/*检查pattern，field和mode的合法性*/
						std::string dbfield;
						bool mode_flg = 0;
						bool find_field_flg = 0;
						auto itr = metamaps.begin();
						for (; itr != metamaps.end(); ++itr)
						{
							/*根据用户可见的字段名获取数据库中对应的字段名， 并判断其对应的查询模式*/
							if (itr->m_custon_field == fieldname)
							{
								find_field_flg = 1;
								dbfield = std::to_string(itr->m_db_field);
								if ((itr->m_mode & mode) >= 1)
								{
									mode_flg = 1;
									break;
								}
							}
						}
						/*没有找到字段名，或者是查询模式不正确*/
						if (itr == metamaps.end())
						{
							/*查询模式不正确*/
							if (find_field_flg == 1 && mode_flg == 0)
							{
								std::string rntStr = u8"{\"status\": 4, \"msg\": \"" + fieldname + " 查询模式不正确\"}";
								response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));
								return 400;
							}
							/*未找到字段*/
							else if (find_field_flg == 0)
							{
								std::string rntStr = u8"{\"status\": 5, \"msg\": \"未找到 '" + fieldname + "' 字段\"}";
								response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));
								return 400;
							}
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
							/*正则查询的匹配串以空格分割，处理并写成正则表达式*/
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
					for (int i = 0; i < queryobj["range"].Size(); ++i)
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
							if (queryobj["range"][i]["min"].IsInt64())   //整数类型
							{
								min_s = std::to_string(queryobj["range"][i]["min"].GetInt64());
							}
							if (queryobj["range"][i]["min"].IsDouble())  //浮点数类型
							{
								min_s = std::to_string(queryobj["range"][i]["min"].GetDouble());
							}
						}
						/*最大值*/
						std::string max_s;
						if (queryobj["range"][i].HasMember("max"))
						{
							if (queryobj["range"][i]["max"].IsInt64())  //整数类型
							{
								max_s = std::to_string(queryobj["range"][i]["max"].GetInt64());
							}
							if (queryobj["range"][i]["max"].IsDouble())  //浮点数类型
							{
								max_s = std::to_string(queryobj["range"][i]["max"].GetDouble());
							}
						}
						/*检查field和mode的合法性*/
						std::string dbfield;
						bool mode_flg = 0;  //查询模式 正确为1
						bool find_field_flg = 0;  //字段是否正确，正确为1
						auto itr = metamaps.begin();
						for (; itr != metamaps.end(); ++itr)
						{
							if (itr->m_custon_field == fieldname)
							{
								find_field_flg = 1;
								dbfield = std::to_string(itr->m_db_field); //获取该字段对应的数据库字段
								if ((itr->m_mode & 0x08) >= 1 || (itr->m_mode & 0x20) >= 1)   //0x08 是日期的范围搜索 0x20是数值的范围搜索
								{
									mode_flg = 1;
									break;
								}
							}
						}
						if (itr == metamaps.end())
						{
							/*查询模式不正确*/
							if (find_field_flg == 1 && mode_flg == 0)
							{
								std::string rntStr = "{\"status\": 6, \"msg\": \"" + fieldname + u8"无法进行范围查询\"}";
								response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));
								return 400;
							}
							/*未找到字段*/
							else if (find_field_flg == 0)
							{
								std::string rntStr = u8"{\"status\": 7, \"msg\": \"未找到 '" + fieldname + u8"' 字段\"}";
								response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));
								return 400;
							}
						}
						/*只有最大值的情况*/
						if (min_s.empty() && !max_s.empty())
						{
							sql.append("(`").append(dbfield).append("` <= ").append(max_s).append(")");
							sql.append(" and ");
						}
						/*只有最小值的情况*/
						else if (!min_s.empty() && max_s.empty())
						{
							sql.append("(`").append(dbfield).append("` >= ").append(min_s).append(")");
							sql.append(" and ");
						}
						/*最大最小值都存在*/
						else if (!min_s.empty() && !max_s.empty())
						{
							sql.append("(`").append(dbfield).append("` between ").append(min_s).append(" and ").append(max_s).append(")");
							sql.append(" and ");
						}
					}
				}
			}

			/*geometry参数 （地理范围的查询[x,y,x,y,x,y...]）*/
			rapidjson::Value::ConstMemberIterator iter = queryobj.FindMember("geometry");  //使用迭代器查找geometry参数
			if (iter != queryobj.MemberEnd() && iter->value.IsObject())
			{
				const rapidjson::Value &geometryObj = iter->value; //获取geometry的值
				rapidjson::Value::ConstMemberIterator iter_polygon = geometryObj.FindMember("polygon");  //使用迭代器循环查找polygon参数
				for (; iter_polygon != queryobj.MemberEnd() && iter_polygon->name == "polygon" && iter_polygon->value.IsArray(); ++iter_polygon)
				{
					std::vector<double> x_v;
					std::vector<double> y_v;
					const rapidjson::Value &polygonArray = iter_polygon->value;  //获取polygon的值
					for (int i = 0; i < polygonArray.Size(); ++i)
					{
						if (i % 2 == 0)  //获取横坐标的值
						{
							if (polygonArray[i].IsDouble())
							{
								x_v.push_back(polygonArray[i].GetDouble());
							}
							if (polygonArray[i].IsInt64())
							{
								x_v.push_back(polygonArray[i].GetInt64());
							}
						}
						else   //获取纵坐标的值
						{
							if (polygonArray[i].IsDouble())
							{
								y_v.push_back(polygonArray[i].GetDouble());
							}
							if (polygonArray[i].IsInt64())
							{
								y_v.push_back(polygonArray[i].GetInt64());
							}
						}
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

					std::string x_max_s = std::to_string(x_max);
					std::string x_min_s = std::to_string(x_min);
					std::string y_max_s = std::to_string(y_max);
					std::string y_min_s = std::to_string(y_min);

					/*获取数据库中的字段名*/
					auto itr = metamaps.begin();
					std::string dbfiled_max_x, dbfiled_min_x, dbfiled_max_y, dbfiled_min_y;
					for (; itr != metamaps.end(); ++itr)
					{
						if (itr->m_custon_field == "MaxX")
						{
							dbfiled_max_x = std::to_string(itr->m_db_field);
						}
						if (itr->m_custon_field == "MinX")
						{
							dbfiled_min_x = std::to_string(itr->m_db_field);
						}
						if (itr->m_custon_field == "MaxY")
						{
							dbfiled_max_y = std::to_string(itr->m_db_field);
						}
						if (itr->m_custon_field == "MinY")
						{
							dbfiled_min_y = std::to_string(itr->m_db_field);
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






	/*query参数*/
	//if (doc1.HasMember("query"))
	//{
	//	const rapidjson::Value &queryobj = doc1["query"];  //获取query的值

	//	/*match参数（精确查询和字符串的正则查询）*/
	//	rapidjson::Value::ConstMemberIterator iter = queryobj.FindMember("match");  //使用迭代器查找match参数
	//	if (iter != queryobj.MemberEnd() && iter->value.IsArray())
	//	{
	//		const rapidjson::Value &matchArray = iter->value;  //获取match的值
	//		for (int i = 0; i < queryobj["match"].Size(); ++i)
	//		{
	//			/*获取字段名*/
	//			std::string fieldname;
	//			rapidjson::Value::ConstMemberIterator iter_match = matchArray[i].FindMember("field");
	//			if (iter_match != matchArray[i].MemberEnd() && iter_match->value.IsString())
	//			{
	//				fieldname = iter_match->value.GetString();
	//			}
	//			/*获取查询模式*/
	//			int mode = 0;
	//			iter_match = matchArray[i].FindMember("mode");
	//			if (iter_match != matchArray[i].MemberEnd() && iter_match->value.IsInt())
	//			{
	//				mode = iter_match->value.GetInt();
	//			}
	//			/*获取匹配串*/
	//			std::string pattern;
	//			iter_match = matchArray[i].FindMember("pattern");
	//			if (iter_match != matchArray[i].MemberEnd())
	//			{
	//				if (iter_match->value.IsString()) //字符串类型
	//				{
	//					pattern = iter_match->value.GetString();
	//				}
	//				if (iter_match->value.IsInt())  //整数类型
	//				{
	//					int pattern_int = iter_match->value.GetInt();
	//					pattern = std::to_string(pattern_int);
	//				}
	//				if (iter_match->value.IsDouble())  //浮点数类型
	//				{
	//					double pattern_db = iter_match->value.GetDouble();
	//					pattern = std::to_string(pattern_db);
	//				}
	//			}
	//			/*检查pattern，field和mode的合法性*/
	//			std::string dbfield;
	//			bool mode_flg = 0;
	//			bool find_field_flg = 0;
	//			auto itr = metamaps.begin();
	//			for (; itr != metamaps.end(); ++itr)
	//			{
	//				/*根据用户可见的字段名获取数据库中对应的字段名， 并判断其对应的查询模式*/
	//				if (itr->m_custon_field == fieldname)
	//				{
	//					find_field_flg = 1;
	//					dbfield = std::to_string(itr->m_db_field);
	//					if ((itr->m_mode & mode) >= 1)
	//					{
	//						mode_flg = 1;
	//						break;
	//					}
	//				}
	//			}
	//			/*没有找到字段名，或者是查询模式不正确*/
	//			if (itr == metamaps.end())
	//			{
	//				/*查询模式不正确*/
	//				if (find_field_flg == 1 && mode_flg == 0)
	//				{
	//					std::string rntStr = "{\"status\": 5, \"msg\": \"" + fieldname + " 查询模式不正确\"}";
	//					response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));
	//					return 400;
	//				}
	//				/*未找到字段*/
	//				else if (find_field_flg == 0)
	//				{
	//					std::string rntStr = "{\"status\": 6, \"msg\": \"未找到 '" + fieldname + "' 字段\"}";
	//					response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));
	//					return 400;
	//				}
	//			}
	//			if (pattern.empty())
	//			{
	//				continue; //如果匹配串为空，跳过该字段不作查询
	//			}
	//			/*根据不同查询模式填写sql语句*/
	//			if (mode == 0x01)	//用正则表达式查询
	//			{
	//				pattern.push_back(' ');
	//				size_t posspc;
	//				size_t posbeg = 0;
	//				/*正则查询的匹配串以空格分割，处理并写成正则表达式*/
	//				std::string regexp = "^.*?[";
	//				do
	//				{
	//					posspc = pattern.find_first_of(" ", posbeg);
	//					std::string substr = pattern.substr(posbeg, posspc - posbeg);
	//					std::string buf_regx;
	//					buf_regx.resize(100);
	//					int ret = mysql_real_escape_string(m_mysql, (char*)buf_regx.data(), substr.data(), substr.size());
	//					regexp.append(buf_regx.data(), ret).append("|");
	//					posbeg = posspc + 1;
	//				} while (posspc < pattern.size() - 1);
	//				regexp = regexp.substr(0, regexp.size() - 1); //去掉结尾的 |
	//				regexp.append("].*$");
	//				sql.append("(`").append(dbfield).append("` regexp '").append(regexp).append("')");
	//			}
	//			else if (mode == 0x02)	//字符串精确查询
	//			{
	//				sql.append("(`").append(dbfield).append("` = '").append(pattern).append("')");
	//			}
	//			else if (mode == 0x04)	//日期的精确查询
	//			{
	//				sql.append("(`").append(dbfield).append("` = ").append(pattern).append(")");
	//			}
	//			else if (mode == 0x10)	//数值的精确查询
	//			{
	//				sql.append("(`").append(dbfield).append("` = ").append(pattern).append(")");
	//			}
	//			else
	//			{
	//				continue;
	//			}
	//			sql.append(" and ");
	//		}
	//	}

	//	/*range参数 （数值或者日期的范围查询）*/
	//	iter = queryobj.FindMember("range");   //使用迭代器查找range参数
	//	if (iter != queryobj.MemberEnd() && iter->value.IsArray())
	//	{
	//		const rapidjson::Value &rangeArray = iter->value; //获取range参数的值
	//		for (int i = 0; i < rangeArray.Size(); ++i)
	//		{
	//			/*字段名*/
	//			std::string fieldname;
	//			rapidjson::Value::ConstMemberIterator iter_range = rangeArray[i].FindMember("field");
	//			if (iter_range != rangeArray[i].MemberEnd() && iter_range->value.IsString())
	//			{
	//				fieldname = iter_range->value.GetString();
	//			}
	//			/*最小值*/
	//			std::string min_s;
	//			iter_range = rangeArray[i].FindMember("min");
	//			if (iter_range != rangeArray[i].MemberEnd())
	//			{
	//				if (iter_range->value.IsInt())   //整数类型
	//				{
	//					min_s = std::to_string(iter_range->value.GetInt());
	//				}
	//				if (iter_range->value.IsDouble())  //浮点数类型
	//				{
	//					min_s = std::to_string(iter_range->value.GetDouble());
	//				}
	//			}
	//			/*最大值*/
	//			std::string max_s;
	//			iter_range = rangeArray[i].FindMember("max");
	//			if (iter_range != rangeArray[i].MemberEnd())
	//			{
	//				if (iter_range->value.IsInt())  //整数类型
	//				{
	//					max_s = std::to_string(iter_range->value.GetInt());
	//				}
	//				if (iter_range->value.IsDouble())  //浮点数类型
	//				{
	//					max_s = std::to_string(iter_range->value.GetDouble());
	//				}
	//			}
	//			/*检查field和mode的合法性*/
	//			std::string dbfield;
	//			//int dateflg = 0;
	//			bool mode_flg = 0;  //查询模式 正确为1
	//			bool find_field_flg = 0;  //字段是否正确，正确为1
	//			auto itr = metamaps.begin();
	//			for (; itr != metamaps.end(); ++itr)
	//			{
	//				if (itr->m_custon_field == fieldname)
	//				{
	//					find_field_flg = 1;
	//					dbfield = std::to_string(itr->m_db_field); //获取该字段对应的数据库字段
	//					if ((itr->m_mode & 0x08) >= 1 || (itr->m_mode & 0x20) >= 1)   //0x08 是日期的范围搜索 0x20是数值的范围搜索
	//					{
	//						mode_flg = 1;
	//						break;
	//					}
	//				}
	//			}
	//			if (itr == metamaps.end())
	//			{
	//				/*查询模式不正确*/
	//				if (find_field_flg == 1 && mode_flg == 0)
	//				{
	//					std::string rntStr = "{\"status\": 7, \"msg\": \"" + fieldname + "无法进行范围查询\"}";
	//					response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));
	//					return 400;
	//				}
	//				/*未找到字段*/
	//				else if (find_field_flg == 0)
	//				{
	//					std::string rntStr = "{\"status\": 8, \"msg\": \"未找到 '" + fieldname + "' 字段\"}";
	//					response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));
	//					return 400;
	//				}
	//			}
	//			
	//			/*只有最大值的情况*/
	//			if (min_s.empty() && !max_s.empty())
	//			{
	//				sql.append("(`").append(dbfield).append("` <= ").append(max_s).append(")");
	//				sql.append(" and ");
	//			}
	//			/*只有最小值的情况*/
	//			else if (!min_s.empty() && max_s.empty())
	//			{
	//				sql.append("(`").append(dbfield).append("` >= ").append(min_s).append(")");
	//				sql.append(" and ");
	//			}
	//			/*最大最小值都存在*/
	//			else if (!min_s.empty() && !max_s.empty())
	//			{
	//				sql.append("(`").append(dbfield).append("` between ").append(min_s).append(" and ").append(max_s).append(")");
	//				sql.append(" and ");
	//			}
	//		}
	//		
	//	}

	//	/*geometry参数 （地理范围的查询[x,y,x,y,x,y...]）*/
	//	iter = queryobj.FindMember("geometry");  //使用迭代器查找geometry参数
	//	if (iter != queryobj.MemberEnd() && iter->value.IsObject())
	//	{

	//		const rapidjson::Value &geometryObj = iter->value; //获取geometry的值
	//		rapidjson::Value::ConstMemberIterator iter_geometry = geometryObj.FindMember("polygon");  //使用迭代器循环查找polygon参数
	//		for (; iter_geometry != queryobj.MemberEnd() && iter_geometry->name == "polygon" && iter_geometry->value.IsArray(); ++iter_geometry)
	//		{
	//			std::vector<double> x_v;
	//			std::vector<double> y_v;
	//			const rapidjson::Value &polygonArray = iter_geometry->value;  //获取polygon的值
	//			for (int i = 0; i < polygonArray.Size(); ++i)
	//			{
	//				if (i % 2 == 0)  //获取横坐标的值
	//				{
	//					if (polygonArray[i].IsDouble())
	//					{
	//						x_v.push_back(polygonArray[i].GetDouble());
	//					}
	//					if (polygonArray[i].IsInt())
	//					{
	//						x_v.push_back(polygonArray[i].GetInt());
	//					}
	//				}
	//				else   //获取纵坐标的值
	//				{
	//					if (polygonArray[i].IsDouble())
	//					{
	//						y_v.push_back(polygonArray[i].GetDouble());
	//					}
	//					if (polygonArray[i].IsInt())
	//					{
	//						y_v.push_back(polygonArray[i].GetInt());
	//					}
	//				}
	//			}
	//			if (x_v.empty() || y_v.empty())
	//			{
	//				std::string rntStr = "{\"status\": 9, \"msg\": \"请求实体polygon有误\"}";  //至少有个x,y
	//				response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));
	//				return 400;
	//			}
	//			/*求出外包框*/
	//			std::vector<double>::iterator x_max_itr = std::max_element(x_v.begin(), x_v.end());
	//			double x_max = *x_max_itr;
	//			std::vector<double>::iterator x_min_itr = std::min_element(x_v.begin(), x_v.end());
	//			double x_min = *x_min_itr;
	//			std::vector<double>::iterator y_max_itr = std::max_element(y_v.begin(), y_v.end());
	//			double y_max = *y_max_itr;
	//			std::vector<double>::iterator y_min_itr = std::min_element(y_v.begin(), y_v.end());
	//			double y_min = *y_min_itr;

	//			std::string x_max_s = std::to_string(x_max);
	//			std::string x_min_s = std::to_string(x_min);
	//			std::string y_max_s = std::to_string(y_max);
	//			std::string y_min_s = std::to_string(y_min);

	//			/*获取数据库中的字段名*/
	//			auto itr = metamaps.begin();
	//			std::string dbfiled_max_x, dbfiled_min_x, dbfiled_max_y, dbfiled_min_y;
	//			for (; itr != metamaps.end(); ++itr)
	//			{
	//				if (itr->m_custon_field == "MaxX")
	//				{
	//					dbfiled_max_x = std::to_string(itr->m_db_field);
	//				}
	//				if (itr->m_custon_field == "MinX")
	//				{
	//					dbfiled_min_x = std::to_string(itr->m_db_field);
	//				}
	//				if (itr->m_custon_field == "MaxY")
	//				{
	//					dbfiled_max_y = std::to_string(itr->m_db_field);
	//				}
	//				if (itr->m_custon_field == "MinY")
	//				{
	//					dbfiled_min_y = std::to_string(itr->m_db_field);
	//				}
	//			}

	//			/*填写sql查询语句*/
	//			std::string minx_miny = x_min_s + " " + y_min_s;
	//			std::string maxx_miny = x_max_s + " " + y_min_s;
	//			std::string maxx_maxy = x_max_s + " " + y_max_s;
	//			std::string minx_maxy = x_min_s + " " + y_max_s;
	//			sql.append("MBRIntersects(ST_GeomFromText('Polygon((").append(minx_miny).append(",").append(maxx_miny).append(",").
	//				append(maxx_maxy).append(",").append(minx_maxy).append(",").append(minx_miny).append("))'), `5`)");   //外包框是否相交
	//			/*sql.append("(((`").append(dbfiled_max_x).append("` > ").append(x_min_s).append(" and `").append(dbfiled_max_x).append("` < ").append(x_max_s).append(")");
	//			sql.append(" and ").append("((`").append(dbfiled_max_y).append("` > ").append(y_min_s).append(" and `").append(dbfiled_max_y).append("` < ").append(y_max_s).append(")");
	//			sql.append(" or ").append("(`").append(dbfiled_min_y).append("` > ").append(y_min_s).append(" and `").append(dbfiled_min_y).append("` < ").append(y_max_s).append(")))");

	//			sql.append(" or ");

	//			sql.append("((`").append(dbfiled_min_x).append("` > ").append(x_min_s).append(" and `").append(dbfiled_min_x).append("` < ").append(x_max_s).append(")");
	//			sql.append(" and ").append("((`").append(dbfiled_max_y).append("` > ").append(y_min_s).append(" and `").append(dbfiled_max_y).append("` < ").append(y_max_s).append(")");
	//			sql.append(" or ").append("(`").append(dbfiled_min_y).append("` > ").append(y_min_s).append(" and `").append(dbfiled_min_y).append("` < ").append(y_max_s).append(")))");

	//			sql.append(" or ");

	//			sql.append("((`").append(dbfiled_max_y).append("` > ").append(y_min_s).append(" and `").append(dbfiled_max_y).append("` < ").append(y_max_s).append(")");
	//			sql.append(" and ").append("((`").append(dbfiled_max_x).append("` > ").append(x_min_s).append(" and `").append(dbfiled_max_x).append("` < ").append(x_max_s).append(")");
	//			sql.append(" or ").append("(`").append(dbfiled_min_x).append("` > ").append(x_min_s).append(" and `").append(dbfiled_min_x).append("` < ").append(x_max_s).append(")))");

	//			sql.append(" or ");

	//			sql.append("((`").append(dbfiled_min_y).append("` > ").append(y_min_s).append(" and `").append(dbfiled_min_y).append("` < ").append(y_max_s).append(")");
	//			sql.append(" and ").append("((`").append(dbfiled_max_x).append("` > ").append(x_min_s).append(" and `").append(dbfiled_max_x).append("` < ").append(x_max_s).append(")");
	//			sql.append(" or ").append("(`").append(dbfiled_min_x).append("` > ").append(x_min_s).append(" and `").append(dbfiled_min_x).append("` < ").append(x_max_s).append(")))");

	//			sql.append(" or ");

	//			sql.append("(`").append(dbfiled_min_x).append("` >= ").append(x_min_s).append(" and `").append(dbfiled_max_x).append("` <= ").append(x_max_s);
	//			sql.append(" and `").append(dbfiled_min_y).append("` >= ").append(y_min_s).append(" and `").append(dbfiled_max_y).append("` <= ").append(y_max_s).append("))");*/

	//			sql.append(" and ");
	//		}
	//	}
	//}


	/*query参数*/
	//if (doc1.HasMember("query"))
	//{
	//	if (doc1["query"].IsObject())
	//	{
	//		rapidjson::Value &queryobj = doc1["query"];
	//		
	//		/*match参数（精确查询和字符串的正则查询）*/
	//		if (queryobj.HasMember("match"))
	//		{
	//			if (queryobj["match"].IsArray())
	//			{
	//				for (int i = 0; i < queryobj["match"].Capacity(); ++i)
	//				{
	//					/*获取字段名*/
	//					std::string fieldname;
	//					if (queryobj["match"][i].HasMember("field"))
	//					{
	//						if (queryobj["match"][i]["field"].IsString())
	//						{
	//							fieldname = queryobj["match"][i]["field"].GetString();
	//						}
	//					}
	//					/*获取查询模式*/
	//					int mode = 0;
	//					if (queryobj["match"][i].HasMember("mode"))
	//					{
	//						if (queryobj["match"][i]["mode"].IsInt())
	//						{
	//							mode = queryobj["match"][i]["mode"].GetInt();
	//						}
	//					}
	//					/*获取匹配串*/
	//					std::string pattern;
	//					if (queryobj["match"][i].HasMember("pattern"))
	//					{
	//						if (queryobj["match"][i]["pattern"].IsString()) //字符串类型
	//						{
	//							pattern = queryobj["match"][i]["pattern"].GetString();
	//						}
	//						if (queryobj["match"][i]["pattern"].IsInt())  //整数类型
	//						{
	//							int pattern_int = queryobj["match"][i]["pattern"].GetInt();
	//							pattern = std::to_string(pattern_int);
	//						}
	//						if (queryobj["match"][i]["pattern"].IsDouble())  //浮点数类型
	//						{
	//							double pattern_db = queryobj["match"][i]["pattern"].GetDouble();
	//							pattern = std::to_string(pattern_db);
	//						}
	//					}
	//					/*检查pattern，field和mode的合法性*/
	//					std::string dbfield;
	//					bool mode_flg = 0;
	//					bool find_field_flg = 0;
	//					auto itr = metamaps.begin();
	//					for (; itr != metamaps.end(); ++itr)
	//					{
	//						/*根据用户可见的字段名获取数据库中对应的字段名， 并判断其对应的查询模式*/
	//						if (itr->m_custon_field == fieldname)
	//						{
	//							find_field_flg = 1;
	//							dbfield = std::to_string(itr->m_db_field);
	//							if ((itr->m_mode & mode) >= 1)
	//							{
	//								mode_flg = 1;
	//								break;
	//							}
	//						}
	//					}
	//					/*没有找到字段名，或者是查询模式不正确*/
	//					if (itr == metamaps.end())  
	//					{
	//						/*查询模式不正确*/
	//						if (find_field_flg == 1 && mode_flg == 0)
	//						{
	//							std::string rntStr = "{\"status\": 5, \"msg\": \"" + fieldname + " 查询模式不正确\"}";
	//							response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));
	//							return 400;
	//						}
	//						/*未找到字段*/
	//						else if (find_field_flg == 0)
	//						{
	//							std::string rntStr = "{\"status\": 6, \"msg\": \"未找到 '" + fieldname + "' 字段\"}";
	//							response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));
	//							return 400;
	//						}
	//					}
	//					if (pattern.empty())
	//					{
	//						continue; //如果匹配串为空，跳过该字段不作查询
	//					}
	//					/*根据不同查询模式填写sql语句*/
	//					if (mode == 0x01)	//用正则表达式查询
	//					{
	//						pattern.push_back(' ');
	//						size_t posspc;
	//						size_t posbeg = 0;
	//						/*正则查询的匹配串以空格分割，处理并写成正则表达式*/
	//						std::string regexp = "^.*?[";
	//						do
	//						{
	//							posspc = pattern.find_first_of(" ", posbeg);
	//							std::string substr = pattern.substr(posbeg, posspc - posbeg);
	//							std::string buf_regx;
	//							buf_regx.resize(100);
	//							int ret = mysql_real_escape_string(m_mysql, (char*)buf_regx.data(), substr.data(), substr.size());
	//							regexp.append(buf_regx.data(), ret).append("|");
	//							posbeg = posspc + 1;
	//						} while (posspc < pattern.size() - 1);
	//						regexp = regexp.substr(0, regexp.size() - 1); //去掉结尾的 |
	//						regexp.append("].*$");
	//						sql.append("(`").append(dbfield).append("` regexp '").append(regexp).append("')");
	//					}
	//					else if (mode == 0x02)	//字符串精确查询
	//					{
	//						sql.append("(`").append(dbfield).append("` = '").append(pattern).append("')");
	//					}
	//					else if (mode == 0x04)	//日期的精确查询
	//					{
	//						/*time_t tm_t;
	//						tm t_s;
	//						t_s.tm_year = atoi(pattern.substr(0, 4).c_str()) - 1900;
	//						t_s.tm_mon = atoi(pattern.substr(5, 2).c_str()) - 1;
	//						t_s.tm_mday = atoi(pattern.substr(6, 2).c_str());
	//						t_s.tm_hour = 0;
	//						t_s.tm_min = 0;
	//						t_s.tm_sec = 0;
	//						tm_t = mktime(&t_s);
	//						char buf_tm[100] = { 0 };
	//						sprintf_s(buf_tm, "%llu", tm_t);
	//						pattern = std::string(buf_tm, strlen(buf_tm));*/
	//						sql.append("(`").append(dbfield).append("` = ").append(pattern).append(")");
	//					}
	//					else if (mode == 0x10)	//数值的精确查询
	//					{
	//						sql.append("(`").append(dbfield).append("` = ").append(pattern).append(")");
	//					}
	//					else
	//					{
	//						continue;
	//					}
	//					sql.append(" and ");
	//				}
	//			}
	//		}

	//		/*range参数 （数值或者日期的范围查询）*/
	//		if (queryobj.HasMember("range"))
	//		{
	//			if (queryobj["range"].IsArray())
	//			{
	//				for (int i = 0; i < queryobj["range"].Capacity(); ++i)
	//				{
	//					/*字段名*/
	//					std::string fieldname;
	//					if (queryobj["range"][i].HasMember("field"))
	//					{
	//						if (queryobj["range"][i]["field"].IsString())
	//						{
	//							fieldname = queryobj["range"][i]["field"].GetString();
	//						}
	//					}
	//					/*最小值*/
	//					std::string min_s;
	//					if (queryobj["range"][i].HasMember("min"))
	//					{
	//						if (queryobj["range"][i]["min"].IsInt())   //整数类型
	//						{
	//							min_s = std::to_string(queryobj["range"][i]["min"].GetInt());
	//						}
	//						if (queryobj["range"][i]["min"].IsDouble())  //浮点数类型
	//						{
	//							min_s = std::to_string(queryobj["range"][i]["min"].GetDouble());
	//						}
	//					}
	//					/*最大值*/
	//					std::string max_s;
	//					if (queryobj["range"][i].HasMember("max"))
	//					{
	//						if (queryobj["range"][i]["max"].IsInt())  //整数类型
	//						{
	//							max_s = std::to_string(queryobj["range"][i]["max"].GetInt());
	//						}
	//						if (queryobj["range"][i]["max"].IsDouble())  //浮点数类型
	//						{
	//							max_s = std::to_string(queryobj["range"][i]["max"].GetDouble());
	//						}
	//					}
	//					/*检查field和mode的合法性*/
	//					std::string dbfield;
	//					//int dateflg = 0;
	//					bool mode_flg = 0;  //查询模式 正确为1
	//					bool find_field_flg = 0;  //字段是否正确，正确为1
	//					auto itr = metamaps.begin();
	//					for (; itr != metamaps.end(); ++itr)
	//					{
	//						if (itr->m_custon_field == fieldname)
	//						{
	//							find_field_flg = 1;
	//							dbfield = std::to_string(itr->m_db_field); //获取该字段对应的数据库字段
	//							if ((itr->m_mode & 0x08) >= 1 || (itr->m_mode & 0x20) >= 1)   //0x08 是日期的范围搜索 0x20是数值的范围搜索
	//							{
	//								/*if ((itr->m_mode & 0x08) >= 1)
	//								{
	//									dateflg = 1;
	//								}*/
	//								mode_flg = 1;
	//								break;
	//							}
	//						}
	//					}
	//					if (itr == metamaps.end())
	//					{
	//						/*查询模式不正确*/
	//						if (find_field_flg == 1 && mode_flg == 0)
	//						{
	//							std::string rntStr = "{\"status\": 7, \"msg\": \"" + fieldname + "无法进行范围查询\"}";
	//							response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));
	//							return 400;
	//						}
	//						/*未找到字段*/
	//						else if (find_field_flg == 0)
	//						{
	//							std::string rntStr = "{\"status\": 8, \"msg\": \"未找到 '" + fieldname + "' 字段\"}";
	//							response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));
	//							return 400;
	//						}
	//					}
	//					/*if (dateflg == 1)
	//					{
	//						if (!min_s.empty())
	//						{
	//							time_t tm_t;
	//							tm t_s;
	//							t_s.tm_year = atoi(min_s.substr(0, 4).c_str()) - 1900;
	//							t_s.tm_mon = atoi(min_s.substr(5, 2).c_str()) - 1;
	//							t_s.tm_mday = atoi(min_s.substr(6, 2).c_str());
	//							t_s.tm_hour = 0;
	//							t_s.tm_min = 0;
	//							t_s.tm_sec = 0;
	//							tm_t = mktime(&t_s);
	//							char buf_tm1[100] = { 0 };
	//							sprintf_s(buf_tm1, "%llu", tm_t);
	//							min_s = std::string(buf_tm1, strlen(buf_tm1));
	//						}
	//						if (!max_s.empty())
	//						{
	//							time_t tm_t;
	//							tm t_s;
	//							t_s.tm_year = atoi(max_s.substr(0, 4).c_str()) - 1900;
	//							t_s.tm_mon = atoi(max_s.substr(5, 2).c_str()) - 1;
	//							t_s.tm_mday = atoi(max_s.substr(6, 2).c_str());
	//							t_s.tm_hour = 0;
	//							t_s.tm_min = 0;
	//							t_s.tm_sec = 0;
	//							tm_t = mktime(&t_s);
	//							char buf_tm2[100] = { 0 };
	//							sprintf_s(buf_tm2, "%llu", tm_t);
	//							max_s = std::string(buf_tm2, strlen(buf_tm2));
	//						}
	//					}*/
	//					/*只有最大值的情况*/
	//					if (min_s.empty() && !max_s.empty())
	//					{
	//						sql.append("(`").append(dbfield).append("` <= ").append(max_s).append(")");
	//						sql.append(" and ");
	//					}
	//					/*只有最小值的情况*/
	//					else if (!min_s.empty() && max_s.empty())
	//					{
	//						sql.append("(`").append(dbfield).append("` >= ").append(min_s).append(")");
	//						sql.append(" and ");
	//					}
	//					/*最大最小值都存在*/
	//					else if (!min_s.empty() && !max_s.empty())
	//					{
	//						sql.append("(`").append(dbfield).append("` between ").append(min_s).append(" and ").append(max_s).append(")");
	//						sql.append(" and ");
	//					}
	//				}
	//			}
	//		}

	//		/*geometry参数 （地理范围的查询[x,y,x,y,x,y...]）*/
	//		if (queryobj.HasMember("geometry"))
	//		{
	//			if (queryobj["geometry"].IsObject())
	//			{
	//				if (queryobj["geometry"].HasMember("polygon"))
	//				{
	//					if (queryobj["geometry"]["polygon"].IsArray())
	//					{
	//						std::vector<double> x_v;
	//						std::vector<double> y_v;
	//						for (int i = 0; i < queryobj["geometry"]["polygon"].Capacity(); ++i)
	//						{
	//							if (i % 2 == 0)  //获取横坐标的值
	//							{
	//								if (queryobj["geometry"]["polygon"][i].IsDouble())
	//								{
	//									x_v.push_back(queryobj["geometry"]["polygon"][i].GetDouble());
	//								}
	//								if (queryobj["geometry"]["polygon"][i].IsInt())
	//								{
	//									x_v.push_back(queryobj["geometry"]["polygon"][i].GetInt());
	//								}
	//							}
	//							else   //获取纵坐标的值
	//							{
	//								if (queryobj["geometry"]["polygon"][i].IsDouble())
	//								{
	//									y_v.push_back(queryobj["geometry"]["polygon"][i].GetDouble());
	//								}
	//								if (queryobj["geometry"]["polygon"][i].IsInt())
	//								{
	//									y_v.push_back(queryobj["geometry"]["polygon"][i].GetInt());
	//								}
	//							}
	//						}
	//						if (x_v.empty() || y_v.empty())
	//						{
	//							std::string rntStr = "{\"status\": 9, \"msg\": \"请求实体polygon有误\"}";  //至少有个x,y
	//							response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));
	//							return 400;
	//						}
	//						/*求出外包框*/
	//						std::vector<double>::iterator x_max_itr = std::max_element(x_v.begin(), x_v.end());
	//						double x_max = *x_max_itr;
	//						std::vector<double>::iterator x_min_itr = std::min_element(x_v.begin(), x_v.end());
	//						double x_min = *x_min_itr;
	//						std::vector<double>::iterator y_max_itr = std::max_element(y_v.begin(), y_v.end());
	//						double y_max = *y_max_itr;
	//						std::vector<double>::iterator y_min_itr = std::min_element(y_v.begin(), y_v.end());
	//						double y_min = *y_min_itr;

	//						std::string x_max_s = std::to_string(x_max);
	//						std::string x_min_s = std::to_string(x_min);
	//						std::string y_max_s = std::to_string(y_max);
	//						std::string y_min_s = std::to_string(y_min);

	//						/*获取数据库中的字段名*/
	//						auto itr = metamaps.begin();
	//						std::string dbfiled_max_x, dbfiled_min_x, dbfiled_max_y, dbfiled_min_y;
	//						for (; itr != metamaps.end(); ++itr)
	//						{
	//							if (itr->m_custon_field == "MaxX")
	//							{
	//								dbfiled_max_x = std::to_string(itr->m_db_field);
	//							}
	//							if (itr->m_custon_field == "MinX")
	//							{
	//								dbfiled_min_x = std::to_string(itr->m_db_field);
	//							}
	//							if (itr->m_custon_field == "MaxY")
	//							{
	//								dbfiled_max_y = std::to_string(itr->m_db_field);
	//							}
	//							if (itr->m_custon_field == "MinY")
	//							{
	//								dbfiled_min_y = std::to_string(itr->m_db_field);
	//							}
	//						}

	//						/*填写sql查询语句*/
	//						sql.append("(((`").append(dbfiled_max_x).append("` > ").append(x_min_s).append(" and `").append(dbfiled_max_x).append("` < ").append(x_max_s).append(")");
	//						sql.append(" and ").append("((`").append(dbfiled_max_y).append("` > ").append(y_min_s).append(" and `").append(dbfiled_max_y).append("` < ").append(y_max_s).append(")");
	//						sql.append(" or ").append("(`").append(dbfiled_min_y).append("` > ").append(y_min_s).append(" and `").append(dbfiled_min_y).append("` < ").append(y_max_s).append(")))");

	//						sql.append(" or ");

	//						sql.append("((`").append(dbfiled_min_x).append("` > ").append(x_min_s).append(" and `").append(dbfiled_min_x).append("` < ").append(x_max_s).append(")");
	//						sql.append(" and ").append("((`").append(dbfiled_max_y).append("` > ").append(y_min_s).append(" and `").append(dbfiled_max_y).append("` < ").append(y_max_s).append(")");
	//						sql.append(" or ").append("(`").append(dbfiled_min_y).append("` > ").append(y_min_s).append(" and `").append(dbfiled_min_y).append("` < ").append(y_max_s).append(")))");

	//						sql.append(" or ");

	//						sql.append("((`").append(dbfiled_max_y).append("` > ").append(y_min_s).append(" and `").append(dbfiled_max_y).append("` < ").append(y_max_s).append(")");
	//						sql.append(" and ").append("((`").append(dbfiled_max_x).append("` > ").append(x_min_s).append(" and `").append(dbfiled_max_x).append("` < ").append(x_max_s).append(")");
	//						sql.append(" or ").append("(`").append(dbfiled_min_x).append("` > ").append(x_min_s).append(" and `").append(dbfiled_min_x).append("` < ").append(x_max_s).append(")))");

	//						sql.append(" or ");

	//						sql.append("((`").append(dbfiled_min_y).append("` > ").append(y_min_s).append(" and `").append(dbfiled_min_y).append("` < ").append(y_max_s).append(")");
	//						sql.append(" and ").append("((`").append(dbfiled_max_x).append("` > ").append(x_min_s).append(" and `").append(dbfiled_max_x).append("` < ").append(x_max_s).append(")");
	//						sql.append(" or ").append("(`").append(dbfiled_min_x).append("` > ").append(x_min_s).append(" and `").append(dbfiled_min_x).append("` < ").append(x_max_s).append(")))");
	//					
	//						sql.append(" or ");

	//						sql.append("(`").append(dbfiled_min_x).append("` >= ").append(x_min_s).append(" and `").append(dbfiled_max_x).append("` <= ").append(x_max_s);
	//						sql.append(" and `").append(dbfiled_min_y).append("` >= ").append(y_min_s).append(" and `").append(dbfiled_max_y).append("` <= ").append(y_max_s).append("))");

	//						sql.append(" and ");
	//					}
	//				}
	//			}
	//		}
	//	}
	//}
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
		if (doc1["size"].IsInt64())
		{
			size = doc1["size"].GetInt64();
		}
	}
	std::string s_size = std::to_string(size);
	s_size = " limit " + s_size;
	sql.append(s_size);

	/*from参数，从第几条记录开始获取*/
	int from = 0;
	if (doc1.HasMember("from"))
	{
		if (doc1["from"].IsInt64())
		{
			from = doc1["from"].GetInt64();
		}
	}
	std::string s_from = std::to_string(from);
	s_from = " offset " + s_from;
	sql.append(s_from);

	std::cout << sql << std::endl;

	/*执行sql查询，并返回查询结果*/
	if (mysql_real_query(m_mysql, sql.c_str(), sql.size()) != 0)
	{
		std::string rntStr = u8"{\"status\": 10, \"msg\": \"mysql_real_query failed\"}";
		response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));
		return 500;
	}
	MYSQL_RES *result = mysql_store_result(m_mysql);
	if (result == NULL)
	{
		std::string rntStr = u8"{\"status\": 11, \"msg\": \"mysql_store_result failed\"}";
		response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));
		return 500;
	}

	/*返回了多少行*/
	unsigned long long rows = mysql_num_rows(result);
	std::cout << rows << std::endl;
	std::string rowstr = std::to_string(rows);
	if (rows < 1)
	{
		std::string rntStr = u8"{\"status\": 12, \"msg\": \"未找到对应的记录\"}";
		response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));
		return 404;
	}

	rapidjson::StringBuffer strbuf;
	strbuf.Reserve(rows * 2000);
	rapidjson::Writer<rapidjson::StringBuffer> writer(strbuf);
	/*将获取到的结果写入json*/
	writer.StartObject();
	writer.Key("urls");
	writer.StartArray();

	MYSQL_ROW rown; //一行数据的类型安全表示
					/*std::string rntstr = "{\"urls\": [ ";
					response_body.insert(response_body.end(), (uint8_t*)rntstr.data(), (uint8_t*)(rntstr.data() + rntstr.size()));*/
	int numrow = 0;
	while ((rown = mysql_fetch_row(result)) != NULL)
	{
		numrow++;
		unsigned long * row_len = mysql_fetch_lengths(result);
		for (int i = 0; i < mysql_num_fields(result); ++i)  //只有一个字段url
		{
			std::string rows_s = std::string(rown[i], row_len[i]);
			writer.String(rows_s.c_str());
			//rntstr = "\"" + rows_s + "\"";
		}
		/*if (numrow < rows)
		{
		rntstr.append(", ");
		}
		response_body.insert(response_body.end(), (uint8_t*)rntstr.data(), (uint8_t*)(rntstr.data() + rntstr.size()));*/
	}
	/*rntstr = " ] }";
	response_body.insert(response_body.end(), (uint8_t*)rntstr.data(), (uint8_t*)(rntstr.data() + rntstr.size()));*/
	writer.EndArray();
	writer.EndObject();

	response_body.insert(response_body.end(), (uint8_t*)strbuf.GetString(), (uint8_t*)(strbuf.GetString() + strbuf.GetSize()));

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

	unispace::us_data_storage_manager& r_storage_mgr =
		unispace::us_data_storage_manager::get_instance();

	/*连接数据库*/
	std::string hostip = "192.168.0.200";
	int port = 3306;
	std::string user = "root";
	std::string password = "123456";
	std::string database = "cmstest";
	std::string tablename = "ngcc_metadata";

	/*读取配置文件（json文件）获取ip和端口以及数据库名等*/
	//rapidjson::Document doc_cfg;
	//FILE *myFile = NULL;
	//fopen_s(&myFile, "./ngcc_metadata_config.json", "rb");
	//if (myFile)
	//{
	//	char buf_cfg[4096] = { 0 };
	//	rapidjson::FileReadStream inputStream(myFile, buf_cfg, sizeof(buf_cfg));  //创建一个输入流
	//	doc_cfg.ParseStream(inputStream); //将读取的内容转换为dom元素
	//	fclose(myFile); //关闭文件，很重要
	//}
	//else
	//{
	//	std::string rntStr = "{\"status\": -1, \"msg\": \"找不到配置文件\"}";
	//	response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));
	//	return 400;
	//}
	//if (doc_cfg.HasParseError())
	//{
	//	std::string rntStr = "{\"status\": -1, \"msg\": \"配置文件中的json格式不正确\"}";
	//	response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));
	//	return 400;
	//}
	//if (doc_cfg.HasMember("hostip"))
	//{
	//	if (!doc_cfg["hostip"].IsString())
	//	{
	//		hostip = doc_cfg["hostip"].GetString();
	//	}
	//}
	//if (doc_cfg.HasMember("port"))
	//{
	//	if (doc_cfg["port"].IsInt())
	//	{
	//		port = doc_cfg["port"].GetInt();
	//	}
	//}
	//if (doc_cfg.HasMember("user"))
	//{
	//	if (doc_cfg["user"].IsString())
	//	{
	//		user = doc_cfg["user"].GetString();
	//	}
	//}
	//if (doc_cfg.HasMember("password"))
	//{
	//	if (doc_cfg["password"].IsString())
	//	{
	//		password = doc_cfg["password"].GetString();
	//	}
	//}
	//if (doc_cfg.HasMember("database"))
	//{
	//	if (doc_cfg["database"].IsString())
	//	{
	//		database = doc_cfg["database"].GetString();
	//	}
	//}
	//if (doc_cfg.HasMember("tablename"))
	//{
	//	if (doc_cfg["tablename"].IsString())
	//	{
	//		tablename = doc_cfg["tablename"].GetString();
	//	}
	//}

	/*初始化mysql*/
	MYSQL * m_mysql = mysql_init(NULL);  //在程序使用Mysql的最开始必须调用mysql_int()初始化
	if (m_mysql == NULL)
	{
		std::string rntStr = u8"{\"status\": 1, \"msg\": \"mysql init failed\"}";
		response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));
		return 200;
	}
	/*连接数据库服务器*/
	int arg = 1;
	mysql_options(m_mysql, MYSQL_OPT_RECONNECT, &arg);

	if (mysql_real_connect(m_mysql, hostip.c_str(),
		user.c_str(), password.c_str(), database.c_str(), port, NULL, 0) == NULL)
	{
		std::string rntStr = u8"{\"status\": 2, \"msg\": \"mysql_real_connect failed\"}";
		response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));
		return 500;
	}


	/*初始化和连接image数据表所在数据库*/
	//MYSQL * m_mysql_img = mysql_init(NULL);  //在程序使用Mysql的最开始必须调用mysql_int()初始化
	//if (m_mysql_img == NULL)
	//{
	//	std::string rntStr = "{\"status\": -1, \"msg\": \"mysql init failed\"}";
	//	response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));
	//	return 500;
	//}
	//if (mysql_real_connect(m_mysql_img, "localhost",
	//	"root", "", "metadata2", port, NULL, 0) == NULL)
	//{
	//	std::string rntStr = "{\"status\": -1, \"msg\": \"mysql_real_connect failed\"}";
	//	response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));
	//	return 500;
	//}
	/*if (mysql_real_connect(m_mysql_img, "192.168.0.200",
		"root", "123456", "unispace", port, NULL, 0) == NULL)
	{
		std::string rntStr = "{\"status\": -1, \"msg\": \"mysql_real_connect failed\"}";
		response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));
		return 500;
	}*/

	/*获取字段映射表*/
	int out_size = 0;
	const us_cms_metdata_map *metamap = us_get_cms_metadata_map_table(&out_size);
	if (metamap == nullptr || out_size < 0)
	{
		std::string rntStr = u8"{\"status\": 3, \"msg\": \"获取字段映射表失败\"}";
		response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));
		return 200;
	}
	std::vector<us_cms_metdata_map> metamaps;
	metamaps.reserve(out_size);

	for (int i = 0; i < out_size; ++i)
	{
		metamaps.push_back(*metamap);
		metamap++;
	}

	/*清空元数据存储表*/
	std::string sql_clear = "truncate table " + tablename;
	if (mysql_real_query(m_mysql, sql_clear.c_str(), sql_clear.size()) != 0)
	{
		std::string rntStr = u8"{\"status\": 4, \"msg\": \"清空元数据查记录表失败\"}";
		response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));
		return 500;
	}

	/*读取image元数据表中的全部记录，重建用户元数据的存储记录*/
	std::string sql_read = "select _id, url from image";
	if (mysql_real_query(m_mysql, sql_read.c_str(), sql_read.size()) != 0)
	{
		std::string rntStr = u8"{\"status\": 5, \"msg\": \"读取image表失败\"}";
		response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));
		return 500;
	}
	MYSQL_RES *result = mysql_store_result(m_mysql);
	if (result == NULL)
	{
		std::string rntStr = u8"{\"status\": 6, \"msg\": \"mysql_store_result failed\"}";
		response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));
		return 500;
	}

	/*返回的记录数目，如果小于1，则image元数据表为空，不能重建*/
	unsigned long long rows = mysql_num_rows(result);
	if (rows < 1)
	{
		std::string rntStr = u8"{\"status\": 7, \"msg\": \"image 元数据表为空\"}";
		response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));
		return 500;
	}
	MYSQL_ROW rown; //一行数据的类型安全表示
	while ((rown = mysql_fetch_row(result)) != NULL)
	{
		std::string fd_ids_str;
		std::string out_json;
		std::string fd_url_str;
		unsigned long *row_len = mysql_fetch_lengths(result);

		/*读取image元数据表的id和url字段，并通过url字段的值（先按xml路径处理）获取到元数据的json格式*/
		for (int i = 0; i < mysql_num_fields(result); ++i)
		{
			MYSQL_FIELD *field = mysql_fetch_field_direct(result, i);  //获取字段
			std::string fd_name = field->name;
			if (fd_name == "_id")
			{
				//获取_id
				std::string buf_ids;
				buf_ids.resize(row_len[i] + 1);
				//创建一个合法的SQL字符串，以便在SQL语句中使用
				int ret = mysql_real_escape_string(m_mysql,
					(char*)buf_ids.data(), rown[i], row_len[i]);
				fd_ids_str = std::string(buf_ids.data(), ret);
			}
			if (fd_name == "url")
			{
				//获取url
				std::string buf_url;
				buf_url.resize(row_len[i] + 1);
				//创建一个合法的SQL字符串，以便在SQL语句中使用
				int ret = mysql_real_escape_string(m_mysql,
					(char*)buf_url.data(), rown[i], row_len[i]);
				buf_url.resize(ret);
				fd_url_str = std::string(buf_url.data(), ret);
				// 获取实际文件路径
				unispace::us_ustring realpath = r_storage_mgr.get_image_real_path(buf_url);
				//根据image中的url读取元数据记录，以json格式存放在out_json
				int ret_read = us_read_cms_metadata_record(realpath, &out_json);
				if (ret_read < 0)
				{
					std::string rntStr = u8"{\"status\": 8, \"msg\": \"无法从url获取元数据\"}";
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
		std::string sql_insert = "insert into " + tablename + "(`_id`, `url`, `status`, ";
		for (int i = 1; i < out_size + 1; i++)
		{
			std::string buf_i = std::to_string(i);
			/*std::string buf_;
			buf_.resize(10);
			char buff_[10] = { 0 };
			sprintf_s(buff_, "%d", i);
			int ret_ = mysql_real_escape_string(m_mysql, (char*)buf_.data(), buff_, strlen(buff_));*/
			sql_insert.append("`").append(buf_i).append("`, ");
		}
		sql_insert = sql_insert.substr(0, sql_insert.size() - 2);
		sql_insert.append(") values('").append(fd_ids_str).append("', '").append(fd_url_str).append("', ").append("0, ");   //重建时status置为0
		
		

		rapidjson::Document doc1;
		doc1.Parse(out_json.c_str());
		if (doc1.HasParseError())
		{
			std::string rntStr = u8"{\"status\": 9, \"msg\": \"获取的元数据json格式有误\"}";
			response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));
			return 500;
		}
		for (int i = 1; i < out_size + 1; i++)
		{
			std::string colname = std::to_string(i);
			std::string value_db = "";
			if (doc1.HasMember(colname.c_str()))
			{
				if (!doc1[colname.c_str()].IsNull())
				{	
					if (metamaps[i-1].m_custon_field == "ImgRange") {
						value_db = doc1[colname.c_str()].GetString();
						value_db = "ST_GeomFromText('Polygon" + value_db + "')";
						sql_insert.append(value_db).append(", ");
					}
					/*只有这三种类型*/
					else if (doc1[colname.c_str()].IsInt64())
					{
						int colv = doc1[colname.c_str()].GetInt64();
						value_db = std::to_string(colv);
						sql_insert.append(value_db).append(", ");
					}
					else if (doc1[colname.c_str()].IsDouble())
					{
						double colv = doc1[colname.c_str()].GetDouble();
						value_db = std::to_string(colv);
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
				std::string rntStr = u8"{\"status\": 10, \"msg\": \"从xml中解析出的json有误\"}";
				response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));
				return 500;
			}
		}

		sql_insert = sql_insert.substr(0, sql_insert.size() - 2);
		sql_insert.append(")");
		if (mysql_real_query(m_mysql, sql_insert.data(), sql_insert.size()) != 0)
		{
			std::string rntStr = u8"{\"status\": 11, \"msg\": \"插入元数据记录失败\"}";
			response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));
			return 500;
		}
	}

	mysql_free_result(result);
	//if (m_mysql_img != NULL)
	//{
	//	mysql_close(m_mysql);
	//}

	if (m_mysql != NULL)
	{
		mysql_close(m_mysql);
	}

	std::string rntStr = u8"{\"status\": 0, \"msg\": \"重建成功\"}";
	response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));

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

	/*读取请求实体*/
	std::string jsonstr;
	for (auto itr = request_body.begin(); itr != request_body.end(); ++itr)
	{
		jsonstr.push_back(*itr);
	}
	rapidjson::Document doc_getm;
	doc_getm.Parse(jsonstr.c_str());
	if (doc_getm.HasParseError())
	{
		std::string rntStr = u8"{\"status\": 4, \"msg\": \"请求实体json格式不正确\"}";
		response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));
		return 400;
	}

	/*从请求实体中获取id和url参数*/
	/*std::set<std::string> set_ids;
	std::set<std::string> set_urls;*/
	std::vector<std::string> vect_ids;
	std::vector<std::string> vect_urls;
	//rapidjson::Value::ConstMemberIterator iter = doc_getm.FindMember("ids");  //查找id
	//if (iter != doc_getm.MemberEnd() && iter->value.IsArray())
	//{
	//	const rapidjson::Value &ids = iter->value;
	//	for (auto itr_ids = ids.Begin(); itr_ids != ids.End(); ++itr_ids)
	//	{
	//		if (itr_ids->IsString())
	//		{
	//			set_ids.insert(std::string(itr_ids->GetString(), itr_ids->GetStringLength()));
	//		}
	//	}
	//}
	rapidjson::Value::ConstMemberIterator iter = doc_getm.FindMember("urls");  //查找urls
	if (iter != doc_getm.MemberEnd() && iter->value.IsArray())
	{
		const rapidjson::Value &urls = iter->value;
		for (auto itr_urls = urls.Begin(); itr_urls != urls.End(); ++itr_urls)
		{
			if (itr_urls->IsString())
			{
				//set_urls.insert(std::string(itr_urls->GetString(), itr_urls->GetStringLength()));
				vect_urls.push_back(std::string(itr_urls->GetString(), itr_urls->GetStringLength()));
			}
		}
	}

	if (vect_ids.empty() && vect_urls.empty())
	{
		//无法从请求实体中获取id或url
		return 400;
	}



	/*连接数据库*/
	std::string hostip = "192.168.0.200";
	int port = 3306;
	std::string user = "root";
	std::string password = "123456";
	std::string database = "unispace";
	std::string tablename = "ngcc_metadata";

	/*读取配置文件（json文件）获取ip和端口以及数据库名等*/
	//rapidjson::Document doc_cfg;
	//FILE *myFile = NULL;
	//fopen_s(&myFile, "./ngcc_metadata_config.json", "rb");
	//if (myFile)
	//{
	//	char buf_cfg[4096] = { 0 };
	//	rapidjson::FileReadStream inputStream(myFile, buf_cfg, sizeof(buf_cfg));  //创建一个输入流
	//	doc_cfg.ParseStream(inputStream); //将读取的内容转换为dom元素
	//	fclose(myFile); //关闭文件，很重要
	//}
	//else
	//{
	//	std::string rntStr = "{\"status\": -1, \"msg\": \"can't find config file\"}";
	//	response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));
	//	return 400;
	//}
	//if (doc_cfg.HasParseError())
	//{
	//	std::string rntStr = "{\"status\": -1, \"msg\": \"config json is error\"}";
	//	response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));
	//	return 400;
	//}
	//if (doc_cfg.HasMember("hostip"))
	//{
	//	if (!doc_cfg["hostip"].IsString())
	//	{
	//		hostip = doc_cfg["hostip"].GetString();
	//	}
	//}
	//if (doc_cfg.HasMember("port"))
	//{
	//	if (doc_cfg["port"].IsInt())
	//	{
	//		port = doc_cfg["port"].GetInt();
	//	}
	//}
	//if (doc_cfg.HasMember("user"))
	//{
	//	if (doc_cfg["user"].IsString())
	//	{
	//		user = doc_cfg["user"].GetString();
	//	}
	//}
	//if (doc_cfg.HasMember("password"))
	//{
	//	if (doc_cfg["password"].IsString())
	//	{
	//		password = doc_cfg["password"].GetString();
	//	}
	//}
	//if (doc_cfg.HasMember("database"))
	//{
	//	if (doc_cfg["database"].IsString())
	//	{
	//		database = doc_cfg["database"].GetString();
	//	}
	//}
	//if (doc_cfg.HasMember("tablename"))
	//{
	//	if (doc_cfg["tablename"].IsString())
	//	{
	//		tablename = doc_cfg["tablename"].GetString();
	//	}
	//}

	MYSQL * m_mysql = mysql_init(NULL);  //在程序使用Mysql的最开始必须调用mysql_int()初始化
	if (m_mysql == NULL)
	{
		std::string rntStr = u8"{\"status\": 1, \"msg\": \"mysql init failed\"}";
		response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));
		return 500;
	}
	int arg = 1;
	mysql_options(m_mysql, MYSQL_OPT_RECONNECT, &arg);

	if (mysql_real_connect(m_mysql, hostip.c_str(),
		user.c_str(), password.c_str(), database.c_str(), port, NULL, 0) == NULL)
	{
		std::string rntStr = u8"{\"status\": 2, \"msg\": \"mysql_real_connect failed\"}";
		response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));
		return 500;
	}

	/*获取字段映射表*/
	int out_size = 0;
	const us_cms_metdata_map *metamap = us_get_cms_metadata_map_table(&out_size);
	if (metamap == nullptr || out_size < 0)
	{
		//获取字段映射表失败
		return 200;
	}

	std::vector<us_cms_metdata_map> metamaps;

	metamaps.reserve(out_size + 2);
	/*metamaps.push_back({ "_id", 0, 1, 0 });*/
	metamaps.push_back({ "url", 0, 1, 0 });

	for (int i = 0; i < out_size; ++i)
	{
		metamaps.push_back(*metamap);
		metamap++;
	}

	std::string sql;
	size_t size_sql = 100 + out_size * 10 + /*vect_ids.size() * 50 +*/ vect_urls.size() * 1600;
	sql.reserve(size_sql); //为sql预分配存储空间

	/*sql = "select _id, url, ";*/
	sql.append("select ");
	for (int i = 0; i < out_size+1; ++i)
	{
		us_cms_metdata_map field_metamap = metamaps[i];

		if (i == 0) //url
		{
			sql.append("url, ");
		}
		else if (field_metamap.m_custon_field == "ImgRange")
		{
			std::string i_str = std::to_string(i);
			sql.append("ST_AsText(`").append(i_str).append("`) as `").append(i_str).append("`, ");
		}
		else
		{
			std::string i_str = std::to_string(i);
			sql.append("`").append(i_str).append("`, ");
		}	
	}
	sql = sql.substr(0, sql.size() - 2);
	sql.append(" from ").append(tablename);


	//为id和url预分配缓存区
	//std::string buf_ids;
	//buf_ids.reserve(set_ids.size() * 32); //一个id长度为16
	//const char *pbuf_ids = buf_ids.c_str();
	//std::string buf_urls;
	//buf_ids.reserve(set_urls.size() * 1534);  //一个url长度为767
	//const char *pbuf_urls = buf_urls.c_str();

	/*where子句*/
	//if (!set_ids.empty() || !set_urls.empty())
	//{
		sql.append(" where ");
		if (!vect_ids.empty())
		{
			//for (auto itr = set_ids.begin(); itr != set_ids.end(); ++itr)
			//{
			//	if (itr->empty())
			//	{
			//		continue;  //如果空值就跳过，不查询
			//	}
			//	char buf_ids[32];
			//	int ret = mysql_real_escape_string(m_mysql, buf_ids, itr->c_str(), itr->size()); //创建一个合法的SQL字符串，以便在SQL语句中使用
			//	std::string fd_ids_str = std::string(buf_ids, ret);
			//	sql.append("`_id` = '").append(fd_ids_str).append("' or ");
			//}
		}
		if (!vect_urls.empty())
		{
			for (auto itr = vect_urls.begin(); itr != vect_urls.end(); ++itr)
			{
				if (itr->empty())
				{
					continue; //如果空值就跳过，不查询
				}
				char buf_url[1534];
				int ret = mysql_real_escape_string(m_mysql, buf_url, itr->c_str(), itr->size()); //创建一个合法的SQL字符串，以便在SQL语句中使用
				std::string fd_url_str = std::string(buf_url, ret);
				sql.append("`url` = '").append(fd_url_str).append("' or ");
			}
		}
		sql = sql.substr(0, sql.size() - 4);
	/*}*/

	/*执行sql查询，并返回查询结果*/
	if (mysql_real_query(m_mysql, sql.c_str(), sql.size()) != 0)
	{
		std::string rntStr = u8"{\"status\": 5, \"msg\": \"mysql_real_query failed\"}";
		response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));
		return 500;
	}
	MYSQL_RES *result = mysql_store_result(m_mysql);
	if (result == NULL)
	{
		std::string rntStr = u8"{\"status\": 6, \"msg\": \"mysql_store_result failed\"}";
		response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));
		return 500;
	}

	/*返回了多少行*/
	unsigned long long rows = mysql_num_rows(result);
	std::string rowstr = std::to_string(rows);
	if (rows < 1)
	{
		std::string rntStr = u8"{\"status\": 7,\"msg\": \"未找到对应的记录\"}";
		response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));
		return 404;
	}

	/*查询到的结果*/
	rapidjson::StringBuffer strbuf;
	strbuf.Reserve(rows * 5120);
	rapidjson::Writer<rapidjson::StringBuffer> writer(strbuf);
	/*将获取到的结果写入json*/
	writer.StartObject();

	/*状态值为0*/
	writer.Key("status");
	writer.Int64(0);

	/*成功的数目*/
	writer.Key("okn");
	writer.Int64(rows);

	MYSQL_ROW rown; //一行数据的类型安全表示

	/*返回成功的元数据记录*/
	writer.Key("metas");
	writer.StartArray();  //数组形式

	/*std::vector<std::string> ok_ids;
	std::vector<std::string> ok_urls;*/

	int numrow = 0;
	while ((rown = mysql_fetch_row(result)) != NULL)
	{
		numrow++;

		writer.StartObject();
		
		unsigned long *row_lens = mysql_fetch_lengths(result); //获取每一列的长度
		for (int i = 0; i < mysql_num_fields(result); ++i)
		{
			//MYSQL_FIELD *field = mysql_fetch_field_direct(result, i);   //获取字段
			std::string fd_s = std::string(rown[i], row_lens[i]);  //获取字段值
			
			std::string fd_name = metamaps[i].m_custon_field;  //用户可见的字段名
			uint32_t fd_type = metamaps[i].m_type;  //字段类型

			//std::string fd_db_name = field->name;  //数据库里的字段名
			//std::string fd_name = fd_db_name;  //用户可见的字段名
			//uint32_t fd_type = 0;  //字段类型
			//for (auto itr = metamaps.begin(); itr != metamaps.end(); ++itr)
			//{
			//	if (itr->m_db_field == atoi(fd_db_name.c_str()))
			//	{
			//		fd_name = itr->m_custon_field;  //获取用户可见的字段名
			//		fd_type = itr->m_type; //字段类型
			//	}
			//}
			
			/*查找失败的_id和url*/
			//if (fd_name == "_id")
			//{
			//	std::set<std::string>::iterator itr = set_ids.find( fd_s); //查找_id
			//	if (itr != set_ids.end()) //找到
			//	{
			//		set_ids.erase(fd_s);  //set_ids存放失败的id
			//	}
			//}
			if (i == 0) //url
			{
				std::vector<std::string>::iterator itr = std::find(vect_urls.begin(), vect_urls.end(), fd_s); //查找url
				if (itr != vect_urls.end()) //找到
				{
					//set_urls.erase(fd_s);  //set_urls存放失败的url
					itr->assign("");
				}
			}

			/*if (fd_name == "_id")
			{
				ok_ids.push_back(fd_s);
			}
			else if (fd_name == "url")
			{
				ok_urls.push_back(fd_s);
			}*/

			/*外包框跳过*/
			if (fd_name == "MaxX" || fd_name == "MinX" || fd_name == "MaxY" || fd_name == "MinY")
			{
				continue;
			}
			/*地理范围((x0 y0,x1 y1,x2 y2,x3 y3,x0 y0))西南，东南，东北，西北*/
			/*else if (fd_name == "ImgRange")
			{
				writer.Key("ImgRange");
				writer.String(fd_s.c_str());*/

				//std::string range_s = fd_s.substr(fd_s.find_first_of("(("));
				//
				//if (range_s.empty())
				//{
				//	continue;
				//}
				//range_s = range_s.substr(2, range_s.size() - 4);  //去掉两端的括号
				//												  /*西南*/
				//size_t pos_spce_sw = range_s.find_first_of(",");
				//std::string s_south_west = range_s.substr(0, pos_spce_sw);  //西南坐标
				//size_t pos_in = s_south_west.find_first_of(" ");
				//std::string SouthWestAbs = s_south_west.substr(0, pos_in);  //西南横坐标
				//std::string SouthWestOrd = s_south_west.substr(pos_in + 1, s_south_west.size() - pos_in - 1);  //西南纵坐标
				//writer.Key("SouthWestAbs");
				//writer.Double(atof(SouthWestAbs.c_str()));
				//writer.Key("SouthWestOrd");
				//writer.Double(atof(SouthWestOrd.c_str()));
				///*rntstr = "\"SouthWestAbs\": " + SouthWestAbs + ", \"SouthWestOrd\": " + SouthWestOrd + ", ";*/

				///*东南*/
				//size_t pos_spce_se = range_s.find_first_of(",", pos_spce_sw + 1);
				//std::string s_south_east = range_s.substr(pos_spce_sw + 1, pos_spce_se - pos_spce_sw - 1); //东南坐标
				//pos_in = s_south_east.find_first_of(" ");
				//std::string SouthEastAbs = s_south_east.substr(0, pos_in);  //东南横坐标
				//std::string SouthEastOrd = s_south_east.substr(pos_in + 1, s_south_east.size() - pos_in - 1);  //东南纵坐标
				//writer.Key("SouthEastAbs");
				//writer.Double(atof(SouthEastAbs.c_str()));
				//writer.Key("SouthEastOrd");
				//writer.Double(atof(SouthEastOrd.c_str()));
				///*rntstr.append("\"SouthEastAbs\": ").append(SouthEastAbs).append(", \"SouthEastOrd\": ").append(SouthEastOrd).append(", ");*/

				///*东北*/
				//size_t pos_spce_ne = range_s.find_first_of(",", pos_spce_se + 1);
				//std::string s_north_east = range_s.substr(pos_spce_se + 1, pos_spce_ne - pos_spce_se - 1); //东北坐标
				//pos_in = s_north_east.find_first_of(" ");
				//std::string NorthEastAbs = s_north_east.substr(0, pos_in);  //东北横坐标
				//std::string NorthEastOrd = s_north_east.substr(pos_in + 1, s_north_east.size() - pos_in - 1);  //东北纵坐标
				//writer.Key("NorthEastAbs");
				//writer.Double(atof(NorthEastAbs.c_str()));
				//writer.Key("NorthEastOrd");
				//writer.Double(atof(NorthEastOrd.c_str()));
				///*rntstr.append("\"NorthEastAbs\": ").append(NorthEastAbs).append(", \"NorthEastOrd\": ").append(NorthEastOrd).append(", ");*/

				///*西北*/
				//size_t pos_spce_nw = range_s.find_first_of(",", pos_spce_ne + 1);
				//std::string s_north_west = range_s.substr(pos_spce_ne + 1, pos_spce_nw - pos_spce_ne - 1); //西北坐标
				//pos_in = s_north_west.find_first_of(" ");
				//std::string NorthWestAbs = s_north_west.substr(0, pos_in);  //西北横坐标
				//std::string NorthWestOrd = s_north_west.substr(pos_in + 1, s_north_west.size() - pos_in - 1);  //西北纵坐标
				//writer.Key("NorthWestAbs");
				//writer.Double(atof(NorthWestAbs.c_str()));
				//writer.Key("NorthWestOrd");
				//writer.Double(atof(NorthWestOrd.c_str()));
				/*rntstr.append("\"NorthWestAbs\": ").append(NorthWestAbs).append(", \"NorthWestOrd\": ").append(NorthWestOrd);*/

				//if (i < mysql_num_fields(result) - 1)  //不是最后一个字段
				//{
				//	rntstr.append(", ");
				//}

				//response_body.insert(response_body.end(), (uint8_t*)rntstr.data(), (uint8_t*)(rntstr.data() + rntstr.size()));
			/*}*/

			/*整数数值类型存储的字段*/
			else if (fd_type == 2)
			/*else if (fd_name == "ProduceDate" || fd_name == "PbandDate" ||
				fd_name == "MultiBandDate" || fd_name == "InstituteCheckDate" || fd_name == "BureauCheckDate" ||
				fd_name == "PixelBits" || fd_name == "CentralMederian" ||
				fd_name == "GaussKrugerZoneNo" || fd_name == "MultiBandNum" ||
				fd_name == "GridInterval" || fd_name == "CheckPointNum")*/
			{
				writer.Key(fd_name.c_str());
				writer.Int64(atoi(fd_s.c_str()));
			}
			/*浮点数数值类型存储的字段*/
			else if (fd_type == 3)
			/*else if (fd_name == "GroundResolution" || fd_name == "ImgSize" || 
				fd_name == "SateResolution" || fd_name == "MultiBandResolution" ||
				fd_name == "SateOriXRMS" || fd_name == "SateOriYRMS" || fd_name == "SateOriZRMS" ||
				fd_name == "WestMosaicMaxError" || fd_name == "NorthMosaicMaxError" || fd_name == "EastMosaicMaxError" ||
				fd_name == "SouthMosaicMaxError" || fd_name == "MultiBRectifyXRMS" || fd_name == "MultiBRectifyYRMS" ||
				fd_name == "CheckRMS" || fd_name == "CheckMAXErr")*/
			{
				writer.Key(fd_name.c_str());
				writer.Double(atof(fd_s.c_str()));

				//rntstr = "\"" + fd_name + "\": " + fd_s;
				//if (i < mysql_num_fields(result) - 1)  //不是最后一个字段
				//{
				//	rntstr.append(", ");
				//}
				//response_body.insert(response_body.end(), (uint8_t*)rntstr.data(), (uint8_t*)(rntstr.data() + rntstr.size()));
			}

			/*按字符串类型存储的字段*/
			else
			{
				writer.Key(fd_name.c_str());
				writer.String(fd_s.data(), row_lens[i]);
				//rntstr = "\"" + fd_name + "\": \"" + fd_s + "\"";
				//if (i < mysql_num_fields(result) - 1)  //不是最后一个字段
				//{
				//	rntstr.append(", ");
				//}
				//response_body.insert(response_body.end(), (uint8_t*)rntstr.data(), (uint8_t*)(rntstr.data() + rntstr.size()));
			}
		}
		writer.EndObject();
		//if (numrow == rows) //最后一行
		//{
		//	rntstr = " }";
		//}
		//else
		//{
		//	rntstr = " }, ";
		//}
		//response_body.insert(response_body.end(), (uint8_t*)rntstr.data(), (uint8_t*)(rntstr.data() + rntstr.size()));
	}
	/*rntstr = " ], ";
	response_body.insert(response_body.end(), (uint8_t*)rntstr.data(), (uint8_t*)(rntstr.data() + rntstr.size()));*/
	writer.EndArray();

	/*查询失败的部分*/
	int failn = 0;
	/*查询失败的id*/

	writer.Key("fails");
	writer.StartObject();
	//writer.Key("ids");
	//writer.StartArray();
	///*rntstr = "\"fails\": { \"ids\": [";
	//response_body.insert(response_body.end(), (uint8_t*)rntstr.data(), (uint8_t*)(rntstr.data() + rntstr.size()));
	//rntstr.clear();*/
	//for (auto itr = set_ids.begin(); itr != set_ids.end(); ++itr)
	//{
	//	writer.String(itr->c_str(), itr->size());
	//}
	////for (auto itr = vect_ids.begin(); itr != vect_ids.end(); ++itr) //vect_ids 是用户做查询时输入的id
	////{
	////	auto it = get_ids.begin();
	////	for (; it != get_ids.end(); ++it)   //get_ids是查询结果中的id
	////	{
	////		if (*itr == *it)
	////		{
	////			break;
	////		}
	////	}
	////	if (it == get_ids.end())
	////	{
	////		writer.String(itr->c_str());
	////		//rntstr.append("\"").append(*itr).append("\", ");   //查询失败的id
	////		failn++;
	////	}
	////}
	//writer.EndArray();

	//int failn_id = failn;
	//if (failn > 0)
	//{
	//	rntstr = rntstr.substr(0, rntstr.size() - 2); //去掉末尾的', '
	//}
	//rntstr.append("], ");
	//response_body.insert(response_body.end(), (uint8_t*)rntstr.data(), (uint8_t*)(rntstr.data() + rntstr.size()));

	/*查询失败的url*/
	writer.Key("urls");
	writer.StartArray();
	/*rntstr = "\"urls\": [";*/

	int len = 0;
	for (auto itr = vect_urls.begin(); itr != vect_urls.end(); ++itr)
	{
		if (!itr->empty())
		{
			writer.String(itr->c_str(), itr->size());
			len++;
		}
	}
	//for (auto itr = vect_urls.begin(); itr != vect_urls.end(); ++itr)  //vect_urls用户做查询时输入的url
	//{
	//	auto it = get_urls.begin();
	//	for (; it != get_urls.end(); ++it)  //get_urls是查询结果中的url
	//	{
	//		if (*itr == *it)
	//		{
	//			break;
	//		}
	//	}
	//	if (it == get_urls.end())
	//	{
	//		writer.String(itr->c_str());
	//		//rntstr.append("\"").append(*itr).append("\", ");  //查询失败的url
	//		failn++;
	//	}
	//}
	writer.EndArray();
	writer.EndObject();
	//if (failn > failn_id)
	//{
	//	rntstr = rntstr.substr(0, rntstr.size() - 2); //去掉末尾的', '
	//}
	//rntstr.append("]}, ");
	//response_body.insert(response_body.end(), (uint8_t*)rntstr.data(), (uint8_t*)(rntstr.data() + rntstr.size()));

	/*查询失败的数目*/
	writer.Key("failn");
	writer.Int64(/*set_ids.size() +*/ len);
	/*std::string failn_s = std::to_string(failn);
	rntstr = "\"failn\": " + failn_s;
	response_body.insert(response_body.end(), (uint8_t*)rntstr.data(), (uint8_t*)(rntstr.data() + rntstr.size()));*/

	writer.EndObject();
	/*rntstr = "} ";
	response_body.insert(response_body.end(), (uint8_t*)rntstr.data(), (uint8_t*)(rntstr.data() + rntstr.size()));*/

	response_body.insert(response_body.end(), (uint8_t*)strbuf.GetString(), (uint8_t*)(strbuf.GetString() + strbuf.GetSize()));

	mysql_free_result(result);

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
	std::string hostip = "localhost";
	int port = 3306;
	std::string user = "root";
	std::string password = "";
	std::string database = "metadata2";
	std::string tablename = "ngcc_metadata";

	/*读取配置文件（json文件）获取ip和端口以及数据库名等*/
	//rapidjson::Document doc_cfg;
	//FILE *myFile = NULL;
	//fopen_s(&myFile, "./ngcc_metadata_config.json", "rb");
	//if (myFile)
	//{
	//	char buf_cfg[4096] = { 0 };
	//	rapidjson::FileReadStream inputStream(myFile, buf_cfg, sizeof(buf_cfg));  //创建一个输入流
	//	doc_cfg.ParseStream(inputStream); //将读取的内容转换为dom元素
	//	fclose(myFile); //关闭文件，很重要
	//}
	//else
	//{
	//	std::string rntStr = "{\"status\": -1, \"msg\": \"can't find config file\"}";
	//	response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));
	//	return 400;
	//}
	//if (doc_cfg.HasParseError())
	//{
	//	std::string rntStr = "{\"status\": -1, \"msg\": \"config json is error\"}";
	//	response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));
	//	return 400;
	//}
	//if (doc_cfg.HasMember("hostip"))
	//{
	//	if (!doc_cfg["hostip"].IsString())
	//	{
	//		hostip = doc_cfg["hostip"].GetString();
	//	}
	//}
	//if (doc_cfg.HasMember("port"))
	//{
	//	if (doc_cfg["port"].IsInt())
	//	{
	//		port = doc_cfg["port"].GetInt();
	//	}
	//}
	//if (doc_cfg.HasMember("user"))
	//{
	//	if (doc_cfg["user"].IsString())
	//	{
	//		user = doc_cfg["user"].GetString();
	//	}
	//}
	//if (doc_cfg.HasMember("password"))
	//{
	//	if (doc_cfg["password"].IsString())
	//	{
	//		password = doc_cfg["password"].GetString();
	//	}
	//}
	//if (doc_cfg.HasMember("database"))
	//{
	//	if (doc_cfg["database"].IsString())
	//	{
	//		database = doc_cfg["database"].GetString();
	//	}
	//}
	//if (doc_cfg.HasMember("tablename"))
	//{
	//	if (doc_cfg["tablename"].IsString())
	//	{
	//		tablename = doc_cfg["tablename"].GetString();
	//	}
	//}

	MYSQL * m_mysql = mysql_init(NULL);  //在程序使用Mysql的最开始必须调用mysql_int()初始化
	if (m_mysql == NULL)
	{
		std::string rntStr = u8"{\"status\": 1, \"msg\": \"mysql init failed\"}";
		response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));
		return 500;
	}
	int arg = 1;
	mysql_options(m_mysql, MYSQL_OPT_RECONNECT, &arg);

	if (mysql_real_connect(m_mysql, hostip.c_str(),
		user.c_str(), password.c_str(), database.c_str(), port, NULL, 0) == NULL)
	{
		std::string rntStr = u8"{\"status\": 2, \"msg\": \"mysql_real_connect failed\"}";
		response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));
		return 500;
	}

	/*获取字段映射表*/
	int out_size = 0;
	const us_cms_metdata_map *metamap = us_get_cms_metadata_map_table(&out_size);
	if (metamap == nullptr || out_size < 0)
	{
		std::string rntStr = u8"{\"status\": 3, \"msg\": \"获取字段映射表失败\"}";
		response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));
		return 200;
	}

	std::vector<us_cms_metdata_map> metamaps;
	metamaps.reserve(out_size);
	for (int i = 0; i < out_size; ++i)
	{
		metamaps.push_back(*metamap);
		metamap++;
	}

	/*查找status为1对应的url*/
	std::string sql_search_status = "select _id, url from " + tablename + " where status = 1";
	if (mysql_real_query(m_mysql, sql_search_status.c_str(), sql_search_status.size()) != 0)
	{
		std::string rntStr = u8"{\"status\": 4, \"msg\": \"mysql_real_query failed\"}";
		response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));
		return 500;
	}

	/*获取url并读取内容插入元数据记录表*/
	MYSQL_RES *result = mysql_store_result(m_mysql);
	if (result == NULL)
	{
		std::string rntStr = u8"{\"status\": 5, \"msg\": \"mysql_store_result failed\"}";
		response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));
		return 500;
	}
	unsigned long long rows = mysql_num_rows(result);
	if (rows < 1)
	{
		std::string rntStr = u8"{\"status\": 0, \"msg\": \"数据无更新\"}";
		response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));
		return 200;
	}
	MYSQL_ROW rown; //一行数据的类型安全表示
	while ((rown = mysql_fetch_row(result)) != NULL) 
	{
		std::string url_s;
		std::string ids_s;
		unsigned long *row_len = mysql_fetch_lengths(result);  //当前行所有列的长度
		for (int i = 0; i < mysql_num_fields(result); ++i)
		{
			MYSQL_FIELD *field = mysql_fetch_field_direct(result, i);  //获取字段
			std::string fd_name = field->name;
			if (fd_name == "url")
			{
				url_s = std::string(rown[i], row_len[i]);   //获取url
				std::string buf_urls;
				buf_urls.resize(url_s.size() + 1);
				int ret = mysql_real_escape_string(m_mysql, (char*)buf_urls.data(), url_s.c_str(), url_s.size());  //创建一个合法的SQL字符串，以便在SQL语句中使用
				url_s = std::string(buf_urls.data(), ret);
			}
			if (fd_name == "_id")
			{
				ids_s = std::string(rown[i], row_len[i]);  //获取_id
				std::string buf_ids;
				buf_ids.resize(ids_s.size() + 1);
				int ret = mysql_real_escape_string(m_mysql, (char*)buf_ids.data(), ids_s.c_str(), ids_s.size()); //创建一个合法的SQL字符串，以便在SQL语句中使用
				ids_s = std::string(buf_ids.data(), ret);
			}
		}
		/*插入元数据*/
		std::string sql_update_insert;
		sql_update_insert.reserve(7168);
		sql_update_insert = "update " + tablename + " set status = 0, ";   //插入数据时将status置为0
		for (int i_v = 1; i_v <= out_size; ++i_v)
		{
			std::string i_v_s = std::to_string(i_v);   //数据库字段名
			std::string jsonstr;
			/*读取元数据记录*/
			int ret_js = us_read_cms_metadata_record(url_s, &jsonstr);
			rapidjson::Document doc1;
			doc1.Parse(jsonstr.c_str());
			if (doc1.HasParseError())
			{
				std::string rntStr = u8"{\"status\": 6, \"msg\": \"从url中获取的json格式不正确\"}";
				response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));
				return 400;
			}
			sql_update_insert.append("`").append(i_v_s).append("` = ").append("");

			std::string value_db;
			if (doc1.HasMember(i_v_s.c_str()))
			{
				if (!doc1[i_v_s.c_str()].IsNull())
				{
					if (metamaps[i_v - 1].m_custon_field == "ImgRange") {
						value_db = doc1[i_v_s.c_str()].GetString();
						value_db = "ST_GeomFromText('Polygon" + value_db + "')";
						sql_update_insert.append(value_db).append(", ");
					}
					/*只有这三种类型*/
					else if (doc1[i_v_s.c_str()].IsInt64())
					{
						int colv = doc1[i_v_s.c_str()].GetInt64();
						value_db = std::to_string(colv);
						sql_update_insert.append(value_db).append(", ");
					}
					else if (doc1[i_v_s.c_str()].IsDouble())
					{
						double colv = doc1[i_v_s.c_str()].GetDouble();
						value_db = std::to_string(colv);
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
		sql_update_insert.append(" where _id = '").append(ids_s).append("'");
		
		if (mysql_real_query(m_mysql, sql_update_insert.c_str(), sql_update_insert.size()) != 0)
		{
			std::string rntStr = u8"{\"status\": 7, \"msg\": \"mysql_real_query failed\"}";
			response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));
			return 500;
		}
	}
		
	mysql_free_result(result);

	if (m_mysql != NULL)
	{
		mysql_close(m_mysql);
	}

	std::string rntStr = u8"{\"status\": 0, \"msg\": \"更新成功\"}";
	response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));

	return 200;
}





/*使用多线程的方法去重建数据记录表*/
std::mutex m_mutex_img;
std::mutex m_mutex_ngcc;

/*读取image表并插入ngcc记录表线程*/
void read_and_insert(MYSQL *m_mysql, std::string tablename, int out_size, int from, int size, int &rnt)
{
	std::unique_lock<std::mutex> lok(m_mutex_img);

	/*查询image表*/
	std::string sql_read = "select _id, url from image limit ";
	sql_read.append(std::to_string(size)).append(" offset ").append(std::to_string(from));
	if (mysql_real_query(m_mysql, sql_read.c_str(), sql_read.size()) != 0)
	{
		rnt = -1;  //读取失败
		return;
	}
	MYSQL_RES *result = mysql_store_result(m_mysql);
	if (result == NULL)
	{
		rnt = -2;  //获取查询结果失败
		return;
	}
	/*返回了多少行*/
	unsigned long long rows = mysql_num_rows(result);
	std::string rowstr = std::to_string(rows);
	if (rows < 1)
	{
		rnt = -3; //没有找到
		return;
	}

	/*遍历每一行数据*/
	MYSQL_ROW rown; //一行数据的类型安全表示
	while ((rown = mysql_fetch_row(result)) != NULL)
	{
		std::string fd_ids_str;
		std::string out_json;
		std::string fd_url_str;
		unsigned long *row_len = mysql_fetch_lengths(result);

		/*读取image元数据表的id和url字段，并通过url字段的值（先按xml路径处理）获取到元数据的json格式*/
		for (int i = 0; i < mysql_num_fields(result); ++i)
		{
			MYSQL_FIELD *field = mysql_fetch_field_direct(result, i);  //获取字段
			std::string fd_name = field->name;
			if (fd_name == "_id")
			{
				fd_ids_str = std::string(rown[i], row_len[i]);  //获取_id
				std::string buf_ids;
				buf_ids.resize(fd_ids_str.size() + 1);
				int ret = mysql_real_escape_string(m_mysql, (char*)buf_ids.data(), fd_ids_str.c_str(), fd_ids_str.size());  //创建一个合法的SQL字符串，以便在SQL语句中使用
				fd_ids_str = std::string(buf_ids.data(), ret);
				std::cout << "thread----" << fd_ids_str << "\t";
			}
			if (fd_name == "url")
			{
				fd_url_str = std::string(rown[i], row_len[i]);  //获取url
				std::string buf_url;
				buf_url.resize(fd_url_str.size() + 1);
				int ret = mysql_real_escape_string(m_mysql, (char*)buf_url.data(), fd_url_str.c_str(), fd_url_str.size());  //创建一个合法的SQL字符串，以便在SQL语句中使用
				fd_url_str = std::string(buf_url.data(), ret);
				int ret_read = us_read_cms_metadata_record(fd_url_str, &out_json);  //根据image中的url读取元数据记录，以json格式存放在out_json
				if (ret_read < 0)
				{
					rnt = -4; //从url中获取json失败
					return;
				}
			}
		}

		std::unique_lock<std::mutex> lok_ngcc(m_mutex_ngcc);

		/*插入数据（将id，url，status（插入元数据则它记为0），元数据全部插入）*/
		std::string sql_insert = "insert into " + tablename + "(`_id`, `url`, `status`, ";
		for (int i = 1; i < out_size + 1; i++)
		{
			std::string buf_i = std::to_string(i);
			sql_insert.append("`").append(buf_i).append("`, ");
		}
		sql_insert = sql_insert.substr(0, sql_insert.size() - 2);
		sql_insert.append(") values('").append(fd_ids_str).append("', '").append(fd_url_str).append("', ").append("0, ");   //重建时status置为0
		for (int i = 1; i < out_size + 1; i++)
		{
			std::string colname = std::to_string(i);

			rapidjson::Document doc1;
			doc1.Parse(out_json.c_str());
			if (doc1.HasParseError())
			{
				rnt = -5; //获取的元数据json格式有误
				return ;
			}
			std::string value_db = "";
			if (doc1.HasMember(colname.c_str()))
			{
				if (!doc1[colname.c_str()].IsNull())
				{	/*只有这三种类型*/
					if (doc1[colname.c_str()].IsInt())
					{
						int colv = doc1[colname.c_str()].GetInt();
						value_db = std::to_string(colv);
						sql_insert.append(value_db).append(", ");
					}
					else if (doc1[colname.c_str()].IsDouble())
					{
						double colv = doc1[colname.c_str()].GetDouble();
						value_db = std::to_string(colv);
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
				rnt = -6; //从xml中解析出的json有误
				return ;
			}
		}

		sql_insert = sql_insert.substr(0, sql_insert.size() - 2);
		sql_insert.append(")");
		if (mysql_real_query(m_mysql, sql_insert.data(), sql_insert.size()) != 0)
		{
			rnt = -7; //插入元数据记录失败
			return;
		}
	}

	mysql_free_result(result);

	
	rnt = 0;
	std::cout << "thread: " << from << "---" << size << std::endl;
	return;
}

int us_cms_http_post_rebuild_addthread(
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
	std::string hostip = "localhost";
	int port = 3306;
	std::string user = "root";
	std::string password = "";
	std::string database = "metadata2";
	std::string tablename = "ngcc_metadata";

	/*初始化mysql*/
	MYSQL * m_mysql = mysql_init(NULL);  //在程序使用Mysql的最开始必须调用mysql_int()初始化
	if (m_mysql == NULL)
	{
		std::string rntStr = "{\"status\": 1, \"msg\": \"mysql init failed\"}";
		response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));
		return 200;
	}
	/*连接数据库服务器*/
	int arg = 1;
	mysql_options(m_mysql, MYSQL_OPT_RECONNECT, &arg);

	if (mysql_real_connect(m_mysql, hostip.c_str(),
		user.c_str(), password.c_str(), database.c_str(), port, NULL, 0) == NULL)
	{
		std::string rntStr = "{\"status\": 2, \"msg\": \"mysql_real_connect failed\"}";
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
		std::string rntStr = "{\"status\": 3, \"msg\": \"获取字段映射表失败\"}";
		response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));
		return 200;
	}

	/*清空元数据存储表*/
	std::string sql_clear = "truncate table " + tablename;
	if (mysql_real_query(m_mysql, sql_clear.c_str(), sql_clear.size()) != 0)
	{
		std::string rntStr = "{\"status\": 4, \"msg\": \"清空元数据查记录表失败\"}";
		response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));
		return 200;
	}

	/*读取image数据表，并插入到ngcc原数据记录表中*/
	std::thread tids[3];
	int rnt_t = 0;
	tids[0] = std::thread(read_and_insert, m_mysql, tablename, out_size, 0, 1, std::ref(rnt_t));
	tids[1] = std::thread(read_and_insert, m_mysql, tablename, out_size, 1, 2, std::ref(rnt_t));
	tids[2] = std::thread(read_and_insert, m_mysql, tablename, out_size, 3, 2, std::ref(rnt_t));

	for (int i = 0; i < 3; ++i)
	{
		tids[i].join();
	}


	if (m_mysql != NULL)
	{
		mysql_close(m_mysql);
	}

	std::string rntStr = "{\"status\": 0, \"msg\": \"重建成功\"}";
	response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));

	return 200;
}










int create()
{
	/*连接数据库*/
	std::string hostip = "localhost";
	int port = 3306;
	std::string user = "root";
	std::string password = "";
	std::string database = "metadata2";
	std::string tablename = "metatb";

	/*连接数据库*/
	MYSQL * m_mysql = mysql_init(NULL);  //在程序使用Mysql的最开始必须调用mysql_int()初始化
	if (m_mysql == NULL)
	{
		return -1;
	}
	int arg = 1;
	mysql_options(m_mysql, MYSQL_OPT_RECONNECT, &arg);  //断开重连

	if (mysql_real_connect(m_mysql, hostip.c_str(),
		user.c_str(), password.c_str(), database.c_str(), port, NULL, 0) == NULL)
	{
		return -1;
	}


	std::string sql = "create table if not exists ngcc_metadata(_id binary(16), url text, status int, `1` text, `2` text, `3` text, `4` text, `5` text, `6` int, `7` text, `8` double, `9` text, `10` int, `11` double, `12` text, `13` double, `14` double, `15` double, `16` double, `17` text, `18` text, `19` text, `20` text, `21` text, `22` int, `23` text, `24` int, `25` text, `26` text, `27` text, `28` text, `29` text, `30` double, `31` text, `32` int, `33` text, `34` int, `35` text, `36` double, `37` text, `38` int, `39` text, `40` int, `41` text, `42` text, `43` double, `44` double, `45` double, `46` text, `47` text, `48` text, `49` text, `50` text, `51` text, `52` text, `53` text, `54` text, `55` double, `56` double, `57` double, `58` double, `59` text, `60` text, `61` text, `62` double, `63` double, `64` int, `65` double, `66` double, `67` text, `68` text, `69` text, `70` int, `71` text, `72` text, `73` text, `74` int)engine=InnoDB default charset=utf8;";

	if (mysql_real_query(m_mysql, sql.c_str(), sql.size()) != 0)
	{
		printf("mysql_real_query failed: error %u ( %s )",
			mysql_errno(m_mysql), mysql_error(m_mysql));
		return -1;
	}

	return 0;
}

int create_chufaqi()
{
	/*连接数据库*/
	std::string hostip = "localhost";
	int port = 3306;
	std::string user = "root";
	std::string password = "";
	std::string database = "metadata2";
	std::string tablename = "image";

	/*连接数据库*/
	MYSQL * m_mysql = mysql_init(NULL);  //在程序使用Mysql的最开始必须调用mysql_int()初始化
	if (m_mysql == NULL)
	{
		return -1;
	}
	int arg = 1;
	mysql_options(m_mysql, MYSQL_OPT_RECONNECT, &arg);  //断开重连

	if (mysql_real_connect(m_mysql, hostip.c_str(),
		user.c_str(), password.c_str(), database.c_str(), port, NULL, 0) == NULL)
	{
		return -1;
	}


	std::string sql_d = "drop trigger if exists insert_tg";
	if (mysql_real_query(m_mysql, sql_d.c_str(), sql_d.size()) != 0)
	{
		printf("mysql_real_query failed: error %u ( %s )",
			mysql_errno(m_mysql), mysql_error(m_mysql));
		return -1;
	}

	std::string sql_ = "delimiter $";
	/*std::string buf;
	buf.resize(100);
	int ret = mysql_real_escape_string(m_mysql, (char*)buf.data(), sql_.c_str(), sql_.size());
	sql_ = std::string(buf.data(), ret);*/
	if (mysql_real_query(m_mysql, sql_.c_str(), sql_.size()) != 0)
	{
		printf("mysql_real_query failed: error %u ( %s )",
			mysql_errno(m_mysql), mysql_error(m_mysql));
		return -1;
	}

	std::string sql = "create tigger insert_tg after insert on image for each row begin insert into ngcc_metadata(_id, url, stauts) values(new._id, new.url, 1); end$";
	if (mysql_real_query(m_mysql, sql.c_str(), sql.size()) != 0)
	{
		printf("mysql_real_query failed: error %u ( %s )",
			mysql_errno(m_mysql), mysql_error(m_mysql));
		return -1;
	}

	sql_ = "delimiter ;";
	if (mysql_real_query(m_mysql, sql_.c_str(), sql_.size()) != 0)
	{
		printf("mysql_real_query failed: error %u ( %s )",
			mysql_errno(m_mysql), mysql_error(m_mysql));
		return -1;
	}

	return 0;
}




/*查询*/
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
	/*std::string hostip = "localhost";
	int port = 3306;
	std::string user = "root";
	std::string password = "";
	std::string database = "metadata2";
	std::string tablename = "ngcc_metadata";*/
	std::string hostip = "192.168.0.200";
	int port = 3306;
	std::string user = "root";
	std::string password = "123456";
	std::string database = "unispace";
	std::string tablename = "ngcc_metadata";

	/*初始化mysql*/
	MYSQL * m_mysql = mysql_init(NULL);  //在程序使用Mysql的最开始必须调用mysql_int()初始化
	if (m_mysql == NULL)
	{
		std::string rntStr = "{\"status\": 1, \"msg\": \"mysql init failed\"}";
		response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));
		return 200;
	}
	/*连接数据库服务器*/
	int arg = 1;
	mysql_options(m_mysql, MYSQL_OPT_RECONNECT, &arg);

	if (mysql_real_connect(m_mysql, hostip.c_str(),
		user.c_str(), password.c_str(), database.c_str(), port, NULL, 0) == NULL)
	{
		std::string rntStr = "{\"status\": 2, \"msg\": \"mysql_real_connect failed\"}";
		response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));
		return 500;
	}

	/*获取字段映射表*/
	int metaMapTableSize = 0;
	const us_cms_metdata_map *pMetaMapTable = us_get_cms_metadata_map_table(&metaMapTableSize);
	if (pMetaMapTable == nullptr || metaMapTableSize < 0) {
		/*bytes_append(response_body,
			LITERAL_STRING_U8(u8"{\"status\":2,\"msg\":\"获取字段映射表失败\"}"));*/
		return 200;
	}

	std::vector<us_cms_metdata_map> metaMapVector;
	metaMapVector.reserve(metaMapTableSize);
	for (int i = 0; i < metaMapTableSize; ++i) {
		metaMapVector.push_back(*pMetaMapTable);
		pMetaMapTable++;
	}

	/*为sql预分配存储空间*/
	std::string sql;
	sql.reserve(65535);

	/*sql语句，获取符合条件的url字段*/
	sql.append("select `url`");
	sql.append(" from ").append(tablename).append(" where ");

	/*解析request_body中的json字符串*/
	std::string jsonstr;
	for (auto itr = request_body.begin(); itr != request_body.end(); ++itr) {
		jsonstr.push_back(*itr);
	}
	rapidjson::Document doc1;
	doc1.Parse(jsonstr.c_str());
	if (doc1.HasParseError())
	{
		/*bytes_append(response_body,
		LITERAL_STRING_U8(u8"{\"status\":3,\"msg\":\"请求实体json格式不正确\"}"));*/
		return 400;
	}
	rapidjson::Value::ConstMemberIterator iter = doc1.FindMember("query");
	if (iter == doc1.MemberEnd() || !iter->value.IsObject()) {
		/*bytes_append(response_body,
			LITERAL_STRING_U8(u8"{\"status\":3,\"msg\":\"请求实体json格式不正确\"}"));*/
		return 400;
	}

	/*query参数*/
	auto queryobj = iter->value.GetObject();
	/*match参数（精确查询和字符串的正则查询）*/
	if (queryobj.HasMember("match")) {
		if (queryobj["match"].IsArray()) {
			for (int i = 0; i < queryobj["match"].Size(); ++i) {
				/*获取字段名*/
				std::string fieldname;
				if (queryobj["match"][i].HasMember("field")) {
					if (queryobj["match"][i]["field"].IsString()) {
						fieldname = queryobj["match"][i]["field"].GetString();
					}
				}
				/*获取查询模式*/
				int mode = 0;
				if (queryobj["match"][i].HasMember("mode")) {
					if (queryobj["match"][i]["mode"].IsInt64()) {
						mode = queryobj["match"][i]["mode"].GetInt64();
					}
				}
				/*获取匹配串*/
				std::string pattern;
				if (queryobj["match"][i].HasMember("pattern")) {
					if (queryobj["match"][i]["pattern"].IsString()) //字符串类型
					{
						pattern = queryobj["match"][i]["pattern"].GetString();
					}
					if (queryobj["match"][i]["pattern"].IsInt64())  //整数类型
					{
						int pattern_int = queryobj["match"][i]["pattern"].GetInt64();
						pattern = std::to_string(pattern_int);
					}
					if (queryobj["match"][i]["pattern"].IsDouble())  //浮点数类型
					{
						double pattern_db = queryobj["match"][i]["pattern"].GetDouble();
						pattern = std::to_string(pattern_db);
					}
				}
				/*检查pattern，field和mode的合法性*/
				std::string dbfield;
				bool mode_flg = 0;
				bool find_field_flg = 0;
				auto itr = metaMapVector.begin();
				for (; itr != metaMapVector.end(); ++itr) {
					/*根据用户可见的字段名获取数据库中对应的字段名， 并判断其对应的查询模式*/
					if (itr->m_custon_field == fieldname) {
						find_field_flg = 1;
						dbfield = std::to_string(itr->m_db_field);
						if ((itr->m_mode & mode) >= 1) {
							mode_flg = 1;
							break;
						}
					}
				}
				/*没有找到字段名，或者是查询模式不正确*/
				if (itr == metaMapVector.end()) {
					/*查询模式不正确*/
					if (find_field_flg == 1 && mode_flg == 0) {
						/*std::string rntStr = "{\"status\":4,\"msg\":\"" + fieldname + u8"查询模式不正确\"}";
						bytes_append(response_body, rntStr);*/
						return 400;
					}
					/*未找到字段*/
					else if (find_field_flg == 0) {
						/*std::string rntStr = u8"{\"status\":5,\"msg\":\"未找到'" + fieldname + u8"'字段\"}";
						bytes_append(response_body, rntStr);*/
						return 400;
					}
				}
				if (pattern.empty()) {
					continue; //如果匹配串为空，跳过该字段不作查询
				}
				/*根据不同查询模式填写sql语句*/
				if (mode == 0x01)	//用正则表达式查询
				{
					pattern.push_back(' ');
					size_t posspc;
					size_t posbeg = 0;
					/*正则查询的匹配串以空格分割，处理并写成正则表达式*/
					std::string regexp = "^.*?[";
					do {
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
					sql.append("(`").append(dbfield).append("` = ").append(pattern).append(")");
				}
				else if (mode == 0x10)	//数值的精确查询
				{
					sql.append("(`").append(dbfield).append("` = ").append(pattern).append(")");
				}
				else {
					continue;
				}
				sql.append(" and ");
			}
		}
	} // end if (queryobj.HasMember("match"))

	  /*range参数 （数值或者日期的范围查询）*/
	if (queryobj.HasMember("range")) {
		if (queryobj["range"].IsArray()) {
			for (int i = 0; i < queryobj["range"].Size(); ++i) {
				/*字段名*/
				std::string fieldname;
				if (queryobj["range"][i].HasMember("field")) {
					if (queryobj["range"][i]["field"].IsString()) {
						fieldname = queryobj["range"][i]["field"].GetString();
					}
				}
				/*最小值*/
				std::string min_s;
				if (queryobj["range"][i].HasMember("min")) {
					if (queryobj["range"][i]["min"].IsInt64())   //整数类型
					{
						min_s = std::to_string(queryobj["range"][i]["min"].GetInt64());
					}
					if (queryobj["range"][i]["min"].IsDouble())  //浮点数类型
					{
						min_s = std::to_string(queryobj["range"][i]["min"].GetDouble());
					}
				}
				/*最大值*/
				std::string max_s;
				if (queryobj["range"][i].HasMember("max")) {
					if (queryobj["range"][i]["max"].IsInt64())  //整数类型
					{
						max_s = std::to_string(queryobj["range"][i]["max"].GetInt64());
					}
					if (queryobj["range"][i]["max"].IsDouble())  //浮点数类型
					{
						max_s = std::to_string(queryobj["range"][i]["max"].GetDouble());
					}
				}
				/*检查field和mode的合法性*/
				std::string dbfield;
				bool mode_flg = 0;  //查询模式 正确为1
				bool find_field_flg = 0;  //字段是否正确，正确为1
				auto itr = metaMapVector.begin();
				for (; itr != metaMapVector.end(); ++itr) {
					if (itr->m_custon_field == fieldname) {
						find_field_flg = 1;
						dbfield = std::to_string(itr->m_db_field); //获取该字段对应的数据库字段
						if ((itr->m_mode & 0x08) >= 1 || (itr->m_mode & 0x20) >= 1)   //0x08 是日期的范围搜索 0x20是数值的范围搜索
						{
							mode_flg = 1;
							break;
						}
					}
				}
				if (itr == metaMapVector.end()) {
					/*查询模式不正确*/
					if (find_field_flg == 1 && mode_flg == 0) {
						/*std::string rntStr = "{\"status\":6,\"msg\":\"" + fieldname + u8"无法进行范围查询\"}";
						bytes_append(response_body, rntStr);*/
						return 400;
					}
					/*未找到字段*/
					else if (find_field_flg == 0) {
						/*std::string rntStr = u8"{\"status\":7,\"msg\":\"未找到'" + fieldname + u8"'字段\"}";
						bytes_append(response_body, rntStr);*/
						return 400;
					}
				}
				/*只有最大值的情况*/
				if (min_s.empty() && !max_s.empty()) {
					sql.append("(`").append(dbfield).append("` <= ").append(max_s).append(")");
					sql.append(" and ");
				}
				/*只有最小值的情况*/
				else if (!min_s.empty() && max_s.empty()) {
					sql.append("(`").append(dbfield).append("` >= ").append(min_s).append(")");
					sql.append(" and ");
				}
				/*最大最小值都存在*/
				else if (!min_s.empty() && !max_s.empty()) {
					sql.append("(`").append(dbfield).append("` between ").append(min_s).append(" and ").append(max_s).append(")");
					sql.append(" and ");
				}
			}
		}
	} // end if (queryobj.HasMember("range"))

	  /*geometry参数 （地理范围的查询[x,y,x,y,x,y...]）*/
	  //使用迭代器查找geometry参数
	iter = queryobj.FindMember("geometry");
	if (iter != queryobj.MemberEnd() && iter->value.IsArray()) {
		auto polyArray = iter->value.GetArray();
		std::vector<double> boxArray;
		int size_ = polyArray.Size();
		boxArray.reserve(polyArray.Size() * 4);
		// 逐个获取polygon
		for (size_t i = 0; i < polyArray.Size(); ++i) {
			// "POLYGON((x1 y1,x2 y2,…,xn yn)）,(x1 y1,x2 y2,…,xm ym),......)"
			const auto& polyStr = polyArray[i];
			if (!polyStr.IsString()) { continue; }
			// 方法一
			const geos::geom::GeometryFactory* pFactory = geos::geom::GeometryFactory::getDefaultInstance();
			geos::io::WKTReader wktreader(pFactory);
			std::string s = std::string(polyStr.GetString(), polyStr.GetStringLength());
			geos::geom::Geometry* pPoly = 
				wktreader.read(s);
			if (pPoly == NULL) { continue; }
			const geos::geom::Envelope* pBox = pPoly->getEnvelopeInternal();
			if (pBox == NULL) { pFactory->destroyGeometry(pPoly); continue; }
			boxArray.push_back(pBox->getMinX());
			boxArray.push_back(pBox->getMinY());
			boxArray.push_back(pBox->getMaxX());
			boxArray.push_back(pBox->getMaxY());
			pFactory->destroyGeometry(pPoly);
			// 方法二
		}

		/*获取数据库中的字段名*/
		auto itr = metaMapVector.begin();
		std::string dbfiled_max_x, dbfiled_min_x, dbfiled_max_y, dbfiled_min_y;
		for (; itr != metaMapVector.end(); ++itr) {
			if (itr->m_custon_field == "MaxX") {
				dbfiled_max_x = std::to_string(itr->m_db_field);
			}
			if (itr->m_custon_field == "MinX") {
				dbfiled_min_x = std::to_string(itr->m_db_field);
			}
			if (itr->m_custon_field == "MaxY") {
				dbfiled_max_y = std::to_string(itr->m_db_field);
			}
			if (itr->m_custon_field == "MinY") {
				dbfiled_min_y = std::to_string(itr->m_db_field);
			}
		}


		for (size_t i = 0; i < boxArray.size(); i += 4) {
			
			std::string x_min_s = std::to_string(boxArray[i]);
			std::string y_min_s = std::to_string(boxArray[i + 1]);
			std::string x_max_s = std::to_string(boxArray[i + 2]);
			std::string y_max_s = std::to_string(boxArray[i + 3]);

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

	/*对sql语句结尾部分处理*/
	size_t posand = sql.rfind("and");
	if (posand == sql.size() - 4) {
		sql = sql.substr(0, sql.size() - 5); //去掉结尾的and
	}
	else {
		sql = sql.substr(0, sql.size() - 6); //去掉结尾的where
	}

	/*size参数，获取几条记录,默认100*/
	int size = 100;
	if (doc1.HasMember("size")) {
		if (doc1["size"].IsInt64()) {
			size = doc1["size"].GetInt64();
		}
	}
	std::string s_size = std::to_string(size);
	s_size = " limit " + s_size;
	sql.append(s_size);

	/*from参数，从第几条记录开始获取*/
	int from = 0;
	if (doc1.HasMember("from")) {
		if (doc1["from"].IsInt64()) {
			from = doc1["from"].GetInt64();
		}
	}
	std::string s_from = std::to_string(from);
	s_from = " offset " + s_from;
	sql.append(s_from);

	std::cout << sql << std::endl;

	/*执行sql查询，并返回查询结果*/
	if (mysql_real_query(m_mysql, sql.c_str(), sql.size()) != 0) {
		printf("mysql_real_query failed: error %u ( %s )",
			mysql_errno(m_mysql), mysql_error(m_mysql));
		return 200;
	}
	MYSQL_RES *result = mysql_store_result(m_mysql);
	if (result == NULL) {
		/*bytes_append(response_body,
			LITERAL_STRING_U8(u8"{\"status\":10,\"msg\":\"获取sql查询结果失败\"}"));*/
		return 200;
	}

	/*返回了多少行*/
	unsigned long long rows = mysql_num_rows(result);

	std::string rowstr = std::to_string(rows);
	if (rows < 1) {
		/*bytes_append(response_body,
			LITERAL_STRING_U8(u8"{\"status\":11,\"msg\":\"未找到对应的记录\"}"));*/
		return 404;
	}

	rapidjson::StringBuffer strbuf;
	strbuf.Reserve(rows * 2000);
	rapidjson::Writer<rapidjson::StringBuffer> writer(strbuf);
	/*将获取到的结果写入json*/
	writer.StartObject();
	writer.Key("urls");
	writer.StartArray();

	MYSQL_ROW rown; //一行数据的类型安全表示
	int numrow = 0;
	while ((rown = mysql_fetch_row(result)) != NULL) {
		numrow++;
		unsigned long * row_len = mysql_fetch_lengths(result);
		for (int i = 0; i < mysql_num_fields(result); ++i)  //只有一个字段url
		{
			std::string rows_s = std::string(rown[i], row_len[i]);
			writer.String(rows_s.c_str());
		}
	}
	writer.EndArray();
	writer.EndObject();

	response_body.insert(response_body.end(), (uint8_t*)strbuf.GetString(), (uint8_t*)(strbuf.GetString() + strbuf.GetSize()));

	mysql_free_result(result);

	if (m_mysql != NULL)
	{
		mysql_close(m_mysql);
	}
	return 200;
}
