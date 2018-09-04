#include "SqlHttpInterface.h"
#include "pugixml.hpp"
#include <iostream>
#include <regex>
#include "Compenent.h"

MetadataQuery::MetadataQuery()
{
	m_usMysql = new UsMySql();
	m_filed = new Filed();

	std::string hostip = "localhost";
	int port = 3306;
	std::string user = "root";
	std::string password = "";
	std::string database = "";
	/*连接服务器*/
	int rnt;
	rnt = m_usMysql->connect_server(hostip, port, user, password, database);

	/*创建数据库*/
	std::string db = "metadata";
	//rnt = m_usMysql->create_database(db);

	/*选择数据库*/
	rnt = m_usMysql->select_database(db);

	/*创建数据表*/
	/*std::map<int, std::string> filed;
	m_filed->insertFileds(filed);
	std::string tablename = "metatb";
	rnt = m_usMysql->create_table(tablename, filed);*/
}

MetadataQuery::~MetadataQuery()
{
	if (m_usMysql != NULL)
	{
		delete m_usMysql;
		m_usMysql = NULL;
	}

	if (m_filed != NULL)
	{
		delete m_usMysql;
		m_usMysql = NULL;
	}
}

int MetadataQuery::metadata_http_get_query(
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

	std::string sql = "select `*`";
	sql.append(" from metatb where ");
	std::vector<std::string> filters;

	for (auto itr = query_parameters.begin(); itr != query_parameters.end(); ++itr)
	{
		/*键不能为空*/
		std::string s_first = itr->first;
		std::string s_second = itr->second;
		wipeStrSpace(s_first);
		wipeStrSpace(s_second);
		if (s_first.empty())
		{
			std::string rntStr = "{\"status\": -1, \"data\": \"请求参数有误\"}";
			response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));
			return 400;
		}
		/*过滤项*/
		if (s_first == "filter")
		{
			if (!s_second.empty())
			{
				std::string index = m_filed->getFildNameIndex(s_second);
				if (index.empty())
				{
					std::string rntStr = "{\"status\": -1, \"data\": \"请求参数有误,未找到该字段\"}";
					response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));
					return 400;
				}
				filters.push_back(index);
			}
		}
		/*数字(整数，可能是字符串的一部分，可能是某个字段的值，也可能是一个日期的年份，也可能是日期的年月)*/
		else if (std::regex_match(s_second, std::regex("^[\\d]{1,9}")))
		{
			if (m_filed->isdate(s_first))
			{
				if (std::regex_match(s_second, std::regex("^[\\d]{4}")) || 
					std::regex_match(s_second, std::regex("^[\\d]{6}")) ||
					std::regex_match(s_second, std::regex("^[\\d]{8}")))
				{
					std::string index = m_filed->getFildNameIndex(s_first);
					if (s_second.size() == 4)
					{
						sql.append("(year(`").append(index).append("`) = ").append(s_second).append(")");
					}
					if (s_second.size() == 6)
					{
						sql.append("((year(`").append(index).append("`) = ").append(s_second.substr(0, 4));
						sql.append(") and (").append("month(`").append(index).append("`) = ").append(s_second.substr(4, 2)).append("))");
					}
					if (s_second.size() == 8)
					{
						std::string date8s = s_second.substr(0, 4) + "-" + s_second.substr(4, 2) + "-" + s_second.substr(6, 2);
						sql.append("(date(`").append(index).append("`) = '").append(date8s).append("')");
					}
					sql.append(" and ");
				}
				else
				{
					std::string rntStr = "{\"status\": -1, \"msg\": \"请求参数有误\"}";
					response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));
					return 400;
				}
			}
			else
			{
				if (s_second.empty())
				{
					continue;
				}
				std::string index = m_filed->getFildNameIndex(s_first);
				if (index.empty())
				{
					std::string rntStr = "{\"status\": -1, \"msg\": \"请求参数有误,未找到该字段\"}";
					response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));
					return 400;
				}
				sql.append("(`").append(index).append("` = ").append(s_second).append(")");//.append(" or ");
				//sql.append("`").append(index).append("` Like '%").append(s_second).append("%')");

				sql.append(" and ");
			}
		}
		/*浮点数*/
		else if (std::regex_match(s_second, std::regex("^[\\d]+[\\.]{1}[\\d]+")))
		{
			if (s_second.empty())
			{
				continue;
			}
			std::string index = m_filed->getFildNameIndex(s_first);
			if (index.empty())
			{
				std::string rntStr = "{\"status\": -1, \"data\": \"请求参数有误,未找到该字段\"}";
				response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));
				return 400;
			}
			sql.append("(`").append(index).append("` = ").append(s_second).append(")");//.append(" or ");
			//sql.append("`").append(index).append("` Like '%").append(s_second).append("%')");
			sql.append(" and ");
		}
		/*{最小值，最大值}*/
		else if (*s_second.begin() == '{' && s_second.back() == '}')
		{
			if (s_first == "ProduceDate" || s_first == "ConfidentialLevel" ||
				s_first == "GroundResolution" || s_first == "ImgSize" ||
				s_first == "GaussKrugerZoneNo" || s_first == "SateResolution" ||
				s_first == "PbandDate" || s_first == "MultiBandResolution" ||
				s_first == "MultiBandDate" || s_first == "GridInterval" ||
				s_first == "SateOriXRMS" || s_first == "SateOriYRMS" ||
				s_first == "SateOriZRMS" || s_first == "InstituteCheckDate" ||
				s_first == "BureauCheckDate")
			{
				size_t pos = s_second.find_first_of(",");
				if (pos != std::string::npos)
				{
					std::string beg = s_second.substr(1, pos - 1);
					wipeStrSpace(beg);
					std::string end = s_second.substr(pos + 1, s_second.size() - pos - 2);
					wipeStrSpace(end);
					std::string index = m_filed->getFildNameIndex(s_first);
					if (index.empty())
					{
						std::string rntStr = "{\"status\": -1, \"data\": \"请求参数有误,未找到该字段\"}";
						response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));
						return 400;
					}

					if (beg.empty() && !end.empty())
					{
						if (m_filed->isdate(s_first))
						{
							sql.append("(");
							if (end.size() == 4)
							{
								sql.append("year(`").append(index).append("`) < ").append(end).append(")");
							}
							else if (end.size() == 6)
							{
								end = "'" + end.substr(0, 4).append("-").append(end.substr(4, 2)) + "-00" + "'";
								sql.append("date(`").append(index).append("`) < ").append(end).append(")");
							}
							else if (end.size() == 8)
							{
								end = "'" + end.substr(0, 4).append("-").append(end.substr(4, 2)).append("-").append(end.substr(6,2)) + "'";
								sql.append("date(`").append(index).append("`) < ").append(end).append(")");
							}
						}
						else
						{
							sql.append("(`").append(index).append("` < ").append(end).append(")");
						}
					}
					else if (end.empty() && !beg.empty())
					{
						if (m_filed->isdate(s_first))
						{
							sql.append("(");
							if (beg.size() == 4)
							{
								sql.append("year(`").append(index).append("`) > ").append(beg).append(")");
							}
							else if (beg.size() == 6)
							{
								beg = "'" + beg.substr(0, 4).append("-").append(beg.substr(4, 2)) + "-01" + "'";
								sql.append("date(`").append(index).append("`) < ").append(beg).append(")");
							}
							else if (end.size() == 8)
							{
								beg = "'" + beg.substr(0, 4).append("-").append(beg.substr(4, 2)).append("-").append(beg.substr(6, 2)) + "'";
								sql.append("date(`").append(index).append("`) < ").append(beg).append(")");
							}
						}
						else
						{
							sql.append("(`").append(index).append("` > ").append(beg).append(")");
						}
					}
					else if (!beg.empty() && !end.empty())
					{
						if (m_filed->isdate(s_first))
						{
							sql.append("(");
							if (beg.size() == 4 && end.size() == 4)
							{
								sql.append("year(`").append(index).append("`) between ").append(beg).append(" and ").append(end).append(")");
							}
							else if (beg.size() == 8 || end.size() == 8)
							{
								std::string dateoryearb = "year(`";
								std::string dateoryeare = "year(`";

								if (beg.size() == 6)
								{
									beg = "'" + beg.substr(0, 4).append("-").append(beg.substr(4, 2)) + "-00" + "'";
									dateoryearb = "date(`";
								}
								if (end.size() == 6)
								{
									end = "'" + end.substr(0, 4).append("-").append(end.substr(4, 2)) + "-00" + "'";
									dateoryeare = "date(`";
								}

								if (beg.size() == 8)
								{
									beg = "'" + beg.substr(0, 4).append("-").append(beg.substr(4, 2)).append("-").append(beg.substr(6, 2)) + "'";
									dateoryearb = "date(`";
								}
								if (end.size() == 8)
								{
									end = "'" + end.substr(0, 4).append("-").append(end.substr(4, 2)).append("-").append(beg.substr(6, 2)) + "'";
									dateoryeare = "date(`";
								}

								sql.append("(").append(dateoryearb).append(index).append("`)").append(" > ").append(beg);
								sql.append(") and (").append(dateoryeare).append(index).append("`)").append(" < ").append(end).append("))");
							}		
							else if (beg.size() == 6 || end.size() == 6)
							{
								std::string dateoryearb = "year(`";
								std::string dateoryeare = "year(`";

								if (beg.size() == 6)
								{
									beg = "'" + beg.substr(0, 4).append("-").append(beg.substr(4, 2)) + "-00" + "'";
									dateoryearb = "date(`";
								}
								if (end.size() == 6)
								{
									end = "'" + end.substr(0, 4).append("-").append(end.substr(4, 2)) + "-00" + "'";
									dateoryeare = "date(`";
								}

								sql.append("(").append(dateoryearb).append(index).append("`)").append(" > ").append(beg);
								sql.append(") and (").append(dateoryeare).append(index).append("`)").append(" < ").append(end).append("))");
								//sql.append(" or ").append("date(`").append(index).append("`) between ").append(" and ").append(end).append(")");
							}
							else
							{
								std::string rntStr = "{\"status\": -1, \"msg\": \"请求参数有误\"}";
								response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));
								return 400;
							}
						}
						else
						{
							sql.append("(`").append(index).append("` between ").append(beg).append(" and ").append(end).append(")");
						}
					}
					else if (beg.empty() && end.empty())
					{
						continue;
					}
				}
				else
				{
					std::string rntStr = "{\"status\": -1, \"msg\": \"请求参数有误\"}";
					response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));
					return 400;
				}
				sql.append(" and ");
			}
			else
			{
				std::string rntStr = "{\"status\": -1, \"msg\": \"请求参数有误，请求字段不可以范围查询\"}";
				response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));
				return 400;
			}
		}
		/*空间范围查找*/
		else if (s_first == "_polygons")
		{
			if (*s_second.begin() == '[' && s_second.back() == ']')
			{
				std::string s_second_str = s_second.substr(1, s_second.size() - 2);
				wipeStrSpace(s_second_str);
				if (s_second_str.empty())
				{
					continue;
				}
				size_t pos1 = s_second_str.find_first_of(",");
				std::string x1, x2, y1, y2;
				if (pos1 != std::string::npos)
				{
					x1 = s_second_str.substr(0, pos1);
					wipeStrSpace(x1);
				}
				else
				{
					std::string rntStr = "{\"status\": -1, \"msg\": \"请求参数有误\"}";
					response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));
					return 400;
				}
				size_t pos2 = s_second_str.find_first_of(",", pos1 + 1);
				if (pos2 != std::string::npos)
				{
					y1 = s_second_str.substr(pos1 + 1, pos2 - pos1 - 1);
					wipeStrSpace(y1);
				}
				size_t pos3 = s_second_str.find_first_of(",", pos2 + 1);
				if (pos3 != std::string::npos)
				{
					x2 = s_second_str.substr(pos2 + 1, pos3 - pos2 - 1);
					wipeStrSpace(x2);
				}

				y2 = s_second_str.substr(pos3 + 1);
				wipeStrSpace(y2);

				std::string x_min = atof(x1.c_str()) < atof(x2.c_str()) ? x1 : x2;
				std::string x_max = atof(x1.c_str()) > atof(x2.c_str()) ? x1 : x2;
				std::string y_min = atof(y1.c_str()) < atof(y2.c_str()) ? y1 : y2;
				std::string y_max = atof(y1.c_str()) > atof(y2.c_str()) ? y1 : y2;
				if (!x_min.empty() && !x_max.empty() && !y_min.empty() && !y_max.empty())
				{
					sql.append("(`14` < ").append(x_max).append(" and ").append("`15` > ").append(x_min).append(" and ");
					sql.append("`16` < ").append(y_max).append(" and ").append("`17` > ").append(y_min).append(")");
				}
				else if (!x_min.empty() && !x_max.empty() && y_min.empty() && y_max.empty())
				{
					sql.append("(`14` < ").append(x_max).append(" and ").append("`15` > ").append(x_min).append(")");
				}
				else if (x_min.empty() && x_max.empty() && !y_min.empty() && !y_max.empty())
				{
					sql.append("(`16` < ").append(y_max).append(" and ").append("`17` > ").append(y_min).append(")");
				}
				else
				{
					std::string rntStr = "{\"status\": -1, \"msg\": \"请求参数有误\"}";
					response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));
					return 400;
				}

				sql.append(" and ");
			}
			else
			{
				std::string rntStr = "{\"status\": -1, \"msg\": \"请求参数有误\"}";
				response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));
				return 400;
			}
		}
		/*字符串的模糊查询*/
		else if (s_first == "ProductName" || s_first == "Owner" ||
			s_first == "Producer" || s_first == "Publisher" ||
			s_first == "InstituteCheckUnit" || s_first == "BureauCheckUnit" ||
			s_first == "MetaDataFileName")
		{
			
			std::string index = m_filed->getFildNameIndex(s_first);
			if (index.empty())
			{
				std::string rntStr = "{\"status\": -1, \"data\": \"请求参数有误,未找到该字段\"}";
				response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));
				return 400;
			}
			if (s_second.empty())
			{
				continue;
			}
			/*
			std::vector<std::string> sv = getstrsub(s_second, 5);
			sql.append("(");
			for (auto itr = sv.begin(); itr != sv.end(); ++itr)
			{
				std::string buf;
				buf.resize(100);
				int ret = mysql_real_escape_string(m_usMysql->m_mysql, (char*)buf.data(), itr->data(), itr->size());
				sql.append("(`").append(index).append("` LIKE '%").append(buf.data(), ret).append("%')").append(" or ");
			}
			sql = sql.substr(0, sql.size() - 4); // 去掉结尾的or
			sql.append(")");
			*/

			/*
			s_second.push_back(' ');
			size_t posspc;
			size_t posbeg = 0;
			sql.append("(");
			do
			{
				posspc = s_second.find_first_of(" ", posbeg);
				std::string substr = s_second.substr(posbeg, posspc - posbeg);
				std::string buf;
				buf.resize(100);
				int ret = mysql_real_escape_string(m_usMysql->m_mysql, (char*)buf.data(), substr.data(), substr.size());
				sql.append("(`").append(index).append("` LIKE '%").append(buf.data(), ret).append("%')").append(" or ");
				posbeg = posspc + 1;
			} while (posspc < s_second.size()-1);
			sql = sql.substr(0, sql.size() - 4); // 去掉结尾的or
			sql.append(")");
			*/

			std::string buf;
			buf.resize(100);
			int ret = mysql_real_escape_string(m_usMysql->m_mysql, (char*)buf.data(), s_second.data(), s_second.size());
			sql.append("(`").append(index).append("` LIKE '%");
			sql.append(buf.data(), ret).append("%') ");
			//sql.append(s_second).append("%')");/* COLLATE utf8_general_ci*/
			sql.append(" and ");
		}
		else
		{
			std::string index = m_filed->getFildNameIndex(s_first);
			if (index.empty())
			{
				std::string rntStr = "{\"status\": -1, \"data\": \"请求参数有误,未找到该字段\"}";
				response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));
				return 400;
			}
			if (s_second.empty())
			{
				continue;
			}
			std::string buf;
			buf.resize(100);
			int ret = mysql_real_escape_string(m_usMysql->m_mysql, (char*)buf.data(), s_second.data(), s_second.size());
			sql.append("(`").append(index).append("` = '").append(buf.data(), ret).append("')");
			//sql.append(s_second).append("%') ");
			sql.append(" and ");
		}
	}

	size_t posand = sql.rfind("and");
	if (posand == sql.size() - 4)
	{
		sql = sql.substr(0, sql.size() - 5); //去掉结尾的and
	}
	else
	{
		sql = sql.substr(0, sql.size() - 6); //去掉结尾的where
	}
	

	/*对过滤项处理*/
	if (!filters.empty())
	{
		std::string filterstr;
		for (auto it = filters.begin(); it != filters.end(); ++it)
		{
			filterstr.append("`").append(*it).append("`, ");
		}
		filterstr = filterstr.substr(0, filterstr.size() - 2);
		sql = sql.substr(0, 6).append(" ").append(filterstr).append(" ").append(sql.substr(11));
	}

	std::cout << sql << std::endl;

	/*执行查询*/
	if (mysql_real_query(m_usMysql->m_mysql, sql.c_str(), sql.size()) != 0)
	{
		printf("mysql_real_query failed: error %u ( %s )",
			mysql_errno(m_usMysql->m_mysql), mysql_error(m_usMysql->m_mysql));
		return -1;
	}
	MYSQL_RES *result = mysql_store_result(m_usMysql->m_mysql);
	if (result == NULL)
	{
		printf("mysql_store_result failed: error %u ( %s )",
			mysql_errno(m_usMysql->m_mysql), mysql_error(m_usMysql->m_mysql));
		return -2;
	}
	
	/*返回了多少行*/
	unsigned long long rows = mysql_num_rows(result); 
	std::cout << rows << std::endl;
	char bufrows[100] = { 0 };
	sprintf_s(bufrows, "%llu", rows);
	std::string rowstr = std::string(bufrows, strlen(bufrows));
	if (rows < 1)
	{
		std::string rntStr = "{\"count\": 0, \"msg\": \"未找到对应的记录\"}";
		response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));
		return 404;
	}

	MYSQL_ROW rown; //一行数据的类型安全表示
	//unsigned long *row_len;
	std::string rntstr = "{ \"count\": \"" + rowstr + "\", \"meta_list\": [ ";
	response_body.insert(response_body.end(), (uint8_t*)rntstr.data(), (uint8_t*)(rntstr.data() + rntstr.size()));
	int numrow = 0;
	while ((rown = mysql_fetch_row(result)) != NULL)
	{
		numrow++;
		rntstr = "{ ";
		response_body.insert(response_body.end(), (uint8_t*)rntstr.data(), (uint8_t*)(rntstr.data() + rntstr.size()));
		//row_len = mysql_fetch_lengths(result);  //返回当前行的列的长度
		for (int i = 0; i < mysql_num_fields(result); ++i)
		{
			MYSQL_FIELD *field = mysql_fetch_field_direct(result, i);
			rntstr = field->name;
			rntstr = m_filed->getFiledName(atoi(rntstr.c_str()));
			std::string s = m_filed->changeformat(rntstr, rown[i]);
			if (i == mysql_num_fields(result) - 1)
			{
				rntstr = "\"" + rntstr + "\": " + "\"" + s + "\"";
			}
			else
			{
				rntstr = "\"" + rntstr + "\": " + "\"" + s + "\", ";
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

	return 200;
}


int MetadataQuery::addFiledValuesFromXML(const std::string path)
{
	pugi::xml_document doc;

	pugi::xml_parse_result result = doc.load_file(path.c_str());
	if (!result)
	{
		return -1;
	}
	/*
	pugi::xml_node chnode = doc.child("Metadatafile").child("BasicDataContent").child("MetaDataFileName");
	std::string MetaDataFileName = chnode.text().as_string();

	chnode = doc.child("Metadatafile").child("BasicDataContent").child("ProductName");
	std::string ProductName = chnode.text().as_string();

	chnode = doc.child("Metadatafile").child("BasicDataContent").child("Owner");
	std::string Owner = chnode.text().as_string();

	chnode = doc.child("Metadatafile").child("BasicDataContent").child("Producer");
	std::string Producer = chnode.text().as_string();

	chnode = doc.child("Metadatafile").child("BasicDataContent").child("ProduceDate");
	std::string ProduceDate = chnode.text().as_string();
	ProduceDate = ProduceDate.substr(0, 4) + "-" + ProduceDate.substr(4, 2) + "-01";

	chnode = doc.child("Metadatafile").child("BasicDataContent").child("ConfidentialLevel");
	std::string ConfidentialLevel = chnode.text().as_string();

	chnode = doc.child("Metadatafile").child("BasicDataContent").child("GroundResolution");
	float GroundResolution = chnode.text().as_float();

	chnode = doc.child("Metadatafile").child("BasicDataContent").child("ImgColorModel");
	std::string ImgColorModel = chnode.text().as_string();

	chnode = doc.child("Metadatafile").child("BasicDataContent").child("PixelBits");
	int PixelBits = chnode.text().as_int();

	chnode = doc.child("Metadatafile").child("BasicDataContent").child("ImgSize");
	float ImgSize = chnode.text().as_float();

	chnode = doc.child("Metadatafile").child("BasicDataContent").child("DataFormat");
	std::string DataFormat = chnode.text().as_string();

	chnode = doc.child("Metadatafile").child("BasicDataContent").child("ImgRange").child("SouthWestAbs");
	float SouthWestAbs = chnode.text().as_float();

	chnode = doc.child("Metadatafile").child("BasicDataContent").child("ImgRange").child("SouthWestOrd");
	float SouthWestOrd = chnode.text().as_float();

	chnode = doc.child("Metadatafile").child("BasicDataContent").child("ImgRange").child("NorthWestAbs");
	float NorthWestAbs = chnode.text().as_float();

	chnode = doc.child("Metadatafile").child("BasicDataContent").child("ImgRange").child("NorthWestOrd");
	float NorthWestOrd = chnode.text().as_float();

	chnode = doc.child("Metadatafile").child("BasicDataContent").child("ImgRange").child("NorthEastAbs");
	float NorthEastAbs = chnode.text().as_float();

	chnode = doc.child("Metadatafile").child("BasicDataContent").child("ImgRange").child("NorthEastOrd");
	float NorthEastOrd = chnode.text().as_float();

	chnode = doc.child("Metadatafile").child("BasicDataContent").child("ImgRange").child("SouthEastAbs");
	float SouthEastAbs = chnode.text().as_float();

	chnode = doc.child("Metadatafile").child("BasicDataContent").child("ImgRange").child("SouthEastOrd");
	float SouthEastOrd = chnode.text().as_float();

	chnode = doc.child("Metadatafile").child("BasicDataContent").child("MathFoundation").child("LongerRadius");
	float LongerRadius = chnode.text().as_float();

	chnode = doc.child("Metadatafile").child("BasicDataContent").child("MathFoundation").child("OblatusRatio");
	std::string OblatusRatio = chnode.text().as_string();

	chnode = doc.child("Metadatafile").child("BasicDataContent").child("MathFoundation").child("GeodeticDatum");
	std::string GeodeticDatum = chnode.text().as_string();

	chnode = doc.child("Metadatafile").child("BasicDataContent").child("MathFoundation").child("MapProjection");
	std::string MapProjection = chnode.text().as_string();

	chnode = doc.child("Metadatafile").child("BasicDataContent").child("MathFoundation").child("CentralMederian");
	int CentralMederian = chnode.text().as_int();

	chnode = doc.child("Metadatafile").child("BasicDataContent").child("MathFoundation").child("ZoneDivisionMode");
	std::string ZoneDivisionMode = chnode.text().as_string();

	chnode = doc.child("Metadatafile").child("BasicDataContent").child("MathFoundation").child("GaussKrugerZoneNo");
	int GaussKrugerZoneNo = chnode.text().as_int();

	chnode = doc.child("Metadatafile").child("BasicDataContent").child("MathFoundation").child("CoordinationUnit");
	std::string CoordinationUnit = chnode.text().as_string();

	chnode = doc.child("Metadatafile").child("BasicDataContent").child("MathFoundation").child("HeightSystem");
	std::string HeightSystem = chnode.text().as_string();

	chnode = doc.child("Metadatafile").child("BasicDataContent").child("MathFoundation").child("HeightDatum");
	std::string HeightDatum = chnode.text().as_string();

	chnode = doc.child("Metadatafile").child("ImgSource").child("SateName");
	std::string SateName = chnode.text().as_string();

	chnode = doc.child("Metadatafile").child("ImgSource").child("PanBand").child("PBandSensorType");
	std::string PBandSensorType = chnode.text().as_string();

	chnode = doc.child("Metadatafile").child("ImgSource").child("PanBand").child("SateResolution");
	float SateResolution = chnode.text().as_float();

	chnode = doc.child("Metadatafile").child("ImgSource").child("PanBand").child("PbandOrbitCode");
	std::string PbandOrbitCode = chnode.text().as_string();

	chnode = doc.child("Metadatafile").child("ImgSource").child("PanBand").child("PbandDate");
	std::string PbandDate = chnode.text().as_string();
	PbandDate = PbandDate.substr(0, 4) + "-" + PbandDate.substr(4, 2) + "-" + PbandDate.substr(6, 2);

	chnode = doc.child("Metadatafile").child("ImgSource").child("MultiBand").child("MultiBandSensorType");
	std::string MultiBandSensorType = chnode.text().as_string();

	chnode = doc.child("Metadatafile").child("ImgSource").child("MultiBand").child("MultiBandNum");
	int MultiBandNum = chnode.text().as_int();

	chnode = doc.child("Metadatafile").child("ImgSource").child("MultiBand").child("MultiBandName");
	std::string MultiBandName = chnode.text().as_string();

	chnode = doc.child("Metadatafile").child("ImgSource").child("MultiBand").child("MultiBandResolution");
	float MultiBandResolution = chnode.text().as_float();

	chnode = doc.child("Metadatafile").child("ImgSource").child("MultiBand").child("MultiBandOrbitCode");
	std::string MultiBandOrbitCode = chnode.text().as_string();

	chnode = doc.child("Metadatafile").child("ImgSource").child("MultiBand").child("MultiBandDate");
	std::string MultiBandDate = chnode.text().as_string();
	MultiBandDate = MultiBandDate.substr(0, 4) + "-" + MultiBandDate.substr(4, 2) + "-" + MultiBandDate.substr(6, 2);

	chnode = doc.child("Metadatafile").child("ImgSource").child("MultiBand").child("SateImgQuality");
	std::string SateImgQuality = chnode.text().as_string();

	chnode = doc.child("Metadatafile").child("ProduceInfomation").child("GridInterval");
	float GridInterval = chnode.text().as_float();

	chnode = doc.child("Metadatafile").child("ProduceInfomation").child("DEMPrecision");
	std::string DEMPrecision = chnode.text().as_string();

	chnode = doc.child("Metadatafile").child("ProduceInfomation").child("ControlSource");
	std::string ControlSource = chnode.text().as_string();

	chnode = doc.child("Metadatafile").child("ProduceInfomation").child("ImgOrientation").child("SateOriXRMS");
	std::string SateOriXRMS = chnode.text().as_string();

	chnode = doc.child("Metadatafile").child("ProduceInfomation").child("ImgOrientation").child("SateOriYRMS");
	std::string SateOriYRMS = chnode.text().as_string();

	chnode = doc.child("Metadatafile").child("ProduceInfomation").child("ImgOrientation").child("SateOriZRMS");
	std::string SateOriZRMS = chnode.text().as_string();

	chnode = doc.child("Metadatafile").child("ProduceInfomation").child("ImgOrientation").child("ATProducerName");
	std::string ATProducerName = chnode.text().as_string();

	chnode = doc.child("Metadatafile").child("ProduceInfomation").child("ImgOrientation").child("ATCheckerName");
	std::string ATCheckerName = chnode.text().as_string();

	chnode = doc.child("Metadatafile").child("ProduceInfomation").child("ManufactureType");
	std::string ManufactureType = chnode.text().as_string();

	chnode = doc.child("Metadatafile").child("ProduceInfomation").child("SteroEditQuality");
	std::string SteroEditQuality = chnode.text().as_string();

	chnode = doc.child("Metadatafile").child("ProduceInfomation").child("OrthoRectifySoftWare");
	std::string OrthoRectifySoftWare = chnode.text().as_string();

	chnode = doc.child("Metadatafile").child("ProduceInfomation").child("ResampleMethod");
	std::string ResampleMethod = chnode.text().as_string();

	chnode = doc.child("Metadatafile").child("ProduceInfomation").child("OrthoRectifyQuality");
	std::string OrthoRectifyQuality = chnode.text().as_string();

	chnode = doc.child("Metadatafile").child("ProduceInfomation").child("OrthoRectifyName");
	std::string OrthoRectifyName = chnode.text().as_string();

	chnode = doc.child("Metadatafile").child("ProduceInfomation").child("OrthoCheckName");
	std::string OrthoCheckName = chnode.text().as_string();

	chnode = doc.child("Metadatafile").child("ProduceInfomation").child("MosaicInfo").child("WestMosaicMaxError");
	float WestMosaicMaxError = chnode.text().as_float();

	chnode = doc.child("Metadatafile").child("ProduceInfomation").child("MosaicInfo").child("NorthMosaicMaxError");
	float NorthMosaicMaxError = chnode.text().as_float();

	chnode = doc.child("Metadatafile").child("ProduceInfomation").child("MosaicInfo").child("EastMosaicMaxError");
	float EastMosaicMaxError = chnode.text().as_float();

	chnode = doc.child("Metadatafile").child("ProduceInfomation").child("MosaicInfo").child("SouthMosaicMaxError");
	float SouthMosaicMaxError = chnode.text().as_float();

	chnode = doc.child("Metadatafile").child("ProduceInfomation").child("MosaicInfo").child("MosaicQuality");
	std::string MosaicQuality = chnode.text().as_string();

	chnode = doc.child("Metadatafile").child("ProduceInfomation").child("MosaicInfo").child("MosaicProducerName");
	std::string MosaicProducerName = chnode.text().as_string();

	chnode = doc.child("Metadatafile").child("ProduceInfomation").child("MosaicInfo").child("MosaicCheckerName");
	std::string MosaicCheckerName = chnode.text().as_string();

	chnode = doc.child("Metadatafile").child("ProduceInfomation").child("MosaicInfo").child("MultiBRectifyXRMS");
	std::string MultiBRectifyXRMS = chnode.text().as_string();

	chnode = doc.child("Metadatafile").child("ProduceInfomation").child("MosaicInfo").child("MultiBRectifyYRMS");
	std::string MultiBRectifyYRMS = chnode.text().as_string();

	chnode = doc.child("Metadatafile").child("ProduceInfomation").child("QualityCheckInfo").child("CheckPointNum");
	int CheckPointNum = chnode.text().as_int();

	chnode = doc.child("Metadatafile").child("ProduceInfomation").child("QualityCheckInfo").child("CheckRMS");
	float CheckRMS = chnode.text().as_float();

	chnode = doc.child("Metadatafile").child("ProduceInfomation").child("QualityCheckInfo").child("CheckMAXErr");
	float CheckMAXErr = chnode.text().as_float();

	chnode = doc.child("Metadatafile").child("ProduceInfomation").child("QualityCheckInfo").child("ConclusionInstitute");
	std::string ConclusionInstitute = chnode.text().as_string();

	chnode = doc.child("Metadatafile").child("ProduceInfomation").child("QualityCheckInfo").child("InstituteCheckUnit");
	std::string InstituteCheckUnit = chnode.text().as_string();

	chnode = doc.child("Metadatafile").child("ProduceInfomation").child("QualityCheckInfo").child("InstituteCheckName");
	std::string InstituteCheckName = chnode.text().as_string();

	chnode = doc.child("Metadatafile").child("ProduceInfomation").child("QualityCheckInfo").child("InstituteCheckDate");
	std::string InstituteCheckDate = chnode.text().as_string();
	InstituteCheckDate = InstituteCheckDate.substr(0, 4) + "-" + InstituteCheckDate.substr(4, 2) + "-01";

	chnode = doc.child("Metadatafile").child("ProduceInfomation").child("QualityCheckInfo").child("BureauCheckName");
	std::string BureauCheckName = chnode.text().as_string();

	chnode = doc.child("Metadatafile").child("ProduceInfomation").child("QualityCheckInfo").child("BureauCheckUnit");
	std::string BureauCheckUnit = chnode.text().as_string();

	chnode = doc.child("Metadatafile").child("ProduceInfomation").child("QualityCheckInfo").child("ConclusionBureau");
	std::string ConclusionBureau = chnode.text().as_string();

	chnode = doc.child("Metadatafile").child("ProduceInfomation").child("QualityCheckInfo").child("BureauCheckDate");
	std::string BureauCheckDate = chnode.text().as_string();
	BureauCheckDate = BureauCheckDate.substr(0, 4) + "-" + BureauCheckDate.substr(4, 2) + "-01";
	*/
	std::string sql = "insert into metatb(";
	for (int i = 1; i < 75; i++)
	{
		std::string buf;
		buf.resize(3);
		char buff[3] = { 0 };
		sprintf_s(buff, "%d", i);
		int ret = mysql_real_escape_string(m_usMysql->m_mysql, (char*)buf.data(), buff, strlen(buff));
		sql.append("`").append(buf.data(), ret).append("`, ");
	}
	sql = sql.substr(0, sql.size() - 2);
	sql.append(") values(");
	
	pugi::xml_node root = doc.child("Metadatafile").child("BasicDataContent");
	for (pugi::xml_node chnode = root.first_child() ;chnode; chnode = chnode.next_sibling())
	{
		std::string chnode_name = chnode.name();
		if (chnode_name == "ImgRange")
		{
			std::vector<double> x;
			std::vector<double> y;
			sql.append("'");
			for (pugi::xml_node subcd = chnode.first_child(); subcd; subcd = subcd.next_sibling())
			{
				std::string subcd_name = subcd.name();
				if (subcd_name == "SouthWestAbs" || subcd_name == "NorthWestAbs" ||
					subcd_name == "NorthEastAbs" || subcd_name == "SouthEastAbs")
				{
					std::string s = subcd.text().as_string();
					x.push_back(atof(s.c_str()));
				}
				if (subcd_name == "SouthWestOrd" || subcd_name == "NorthWestOrd" ||
					subcd_name == "NorthEastOrd" || subcd_name == "SouthEastOrd")
				{
					std::string s = subcd.text().as_string();
					y.push_back(atof(s.c_str()));
				}

				std::string buf;
				buf.resize(100);
				std::string s = subcd_name + ": " + subcd.text().as_string();
				int ret = mysql_real_escape_string(m_usMysql->m_mysql, (char*)buf.data(), s.data(), s.size());
				sql.append(buf.data(), ret).append("  ");
			}
			std::cout << sql << std::endl;
			sql.append("', ");
			std::vector<double>::iterator x_max_itr = std::max_element(x.begin(), x.end());
			double x_max = *x_max_itr;
			std::vector<double>::iterator x_min_itr = std::min_element(x.begin(), x.end());
			double x_min = *x_min_itr;
			std::vector<double>::iterator y_max_itr = std::max_element(y.begin(), y.end());
			double y_max = *y_max_itr;
			std::vector<double>::iterator y_min_itr = std::min_element(y.begin(), y.end());
			double y_min = *y_min_itr;

			std::string buf;
			buf.resize(100);
			char buff[100] = { 0 };
			sprintf_s(buff, "%lf", x_max);
			std::string s = std::string(buff, strlen(buff));
			int ret = mysql_real_escape_string(m_usMysql->m_mysql, (char*)buf.data(), s.data(), s.size());
			sql.append(buf.data(), ret).append(", ");
			memset(buff, 0, 100);
			sprintf_s(buff, "%lf", x_min);
			s = std::string(buff, strlen(buff));
			ret = mysql_real_escape_string(m_usMysql->m_mysql, (char*)buf.data(), s.data(), s.size());
			sql.append(buf.data(), ret).append(", ");
			memset(buff, 0, 100);
			sprintf_s(buff, "%lf", y_max);
			s = std::string(buff, strlen(buff));
			ret = mysql_real_escape_string(m_usMysql->m_mysql, (char*)buf.data(), s.data(), s.size());
			sql.append(buf.data(), ret).append(", ");
			memset(buff, 0, 100);
			sprintf_s(buff, "%lf", y_min);
			s = std::string(buff, strlen(buff));
			ret = mysql_real_escape_string(m_usMysql->m_mysql, (char*)buf.data(), s.data(), s.size());
			sql.append(buf.data(), ret).append(", ");
		}
		else if (chnode_name == "MathFoundation")
		{
			for (pugi::xml_node subcd = chnode.first_child(); subcd; subcd = subcd.next_sibling())
			{
				std::string buf;
				buf.resize(100);
				std::string s = subcd.text().as_string();
				int ret = mysql_real_escape_string(m_usMysql->m_mysql, (char*)buf.data(), s.data(), s.size());
				if (subcd.name() == "LongerRadius" || subcd.name() == "CentralMederian" ||
					subcd.name() == "GaussKrugerZoneNo")
				{
					sql.append(buf.data(), ret).append(", ");
				}
				else
				{
					sql.append("'").append(buf.data(), ret).append("', ");
				}
			}
		}
		else
		{
			std::string buf;
			buf.resize(1000);
			std::string s = chnode.text().as_string();
			
			if (chnode_name == "ProduceDate")
			{
				if (s.size() == 6)
				{
					s = s.substr(0, 4) + "-" + s.substr(4, 2) + "-01";
				}
				else if (s.size() == 8)
				{
					s = s.substr(0, 4) + "-" + s.substr(4, 2) + "-" + s.substr(6, 2);
				}
			}

			int ret = mysql_real_escape_string(m_usMysql->m_mysql, (char*)buf.data(), s.data(), s.size());

			if (chnode_name == "GroundResolution" || chnode_name == "PixelBits" || chnode_name == "ImgSize")
			{
				sql.append(buf.data(), ret).append(", ");
			}
			else
			{
				sql.append("'").append(buf.data(), ret).append("', ");
			}
		}
	}

	root = doc.child("Metadatafile").child("ImgSource");
	for (pugi::xml_node chnode = root.first_child(); chnode; chnode = chnode.next_sibling())
	{
		std::string chnode_name = chnode.name();
		if (chnode_name == "PanBand" || chnode_name == "MultiBand")
		{
			for (pugi::xml_node subcd = chnode.first_child(); subcd; subcd = subcd.next_sibling())
			{
				std::string buf;
				buf.resize(100);
				std::string s = subcd.text().as_string();
				std::string subcd_name = subcd.name();
				if (subcd_name == "PBandDate" || subcd_name == "MultiBandDate")
				{
					//s = s.substr(0, 4) + "-" + s.substr(4, 2) + "-" + s.substr(6, 2);
					s = "2018-08-27";
				}

				int ret = mysql_real_escape_string(m_usMysql->m_mysql, (char*)buf.data(), s.data(), s.size());
				
				if (subcd_name == "SateResolution" || subcd_name == "MultiBandNum" || subcd_name == "MultiBandResolution")
				{
					sql.append(buf.data(), ret).append(", ");
				}
				else
				{
					sql.append("'").append(buf.data(), ret).append("', ");
				}
			}
		}
		else
		{
			std::string buf;
			buf.resize(100);
			std::string s = chnode.text().as_string();
			int ret = mysql_real_escape_string(m_usMysql->m_mysql, (char*)buf.data(), s.data(), s.size());
			sql.append("'").append(buf.data(), ret).append("', ");
		}
	}

	root = doc.child("Metadatafile").child("ProduceInfomation");
	for (pugi::xml_node chnode = root.first_child(); chnode; chnode = chnode.next_sibling())
	{
		std::string chnode_name = chnode.name();
		if (chnode_name == "ImgOrientation" || chnode_name == "MosaicInfo" || chnode_name == "QualityCheckInfo")
		{
			for (pugi::xml_node subcd = chnode.first_child(); subcd; subcd = subcd.next_sibling())
			{
				std::string buf;
				buf.resize(100);
				std::string s = subcd.text().as_string();
				std::string subcd_name = subcd.name();
				if (subcd_name == "InstituteCheckDate" || subcd_name == "BureauCheckDate")
				{
					s = s.substr(0, 4) + "-" + s.substr(4, 2) + "-01";
				}
				int ret = mysql_real_escape_string(m_usMysql->m_mysql, (char*)buf.data(), s.data(), s.size());

				if (subcd_name == "WestMosaicMaxError" || subcd_name == "NorthMosaicMaxError" ||
					subcd_name == "EastMosaicMaxError" || subcd_name == "SouthMosaicMaxError" ||
					subcd_name == "CheckPointNum" || subcd_name == "CheckRMS" ||
					subcd_name == "CheckMAXErr")
				{
					sql.append(buf.data(), ret).append(", ");
				}
				else
				{
					sql.append("'").append(buf.data(), ret).append("', ");
				}
			}
		}
		else
		{
			std::string buf;
			buf.resize(100);
			std::string s = chnode.text().as_string();
			int ret = mysql_real_escape_string(m_usMysql->m_mysql, (char*)buf.data(), s.data(), s.size());
			sql.append("'").append(buf.data(), ret).append("', ");
		}
	}

	sql = sql.substr(0, sql.size() - 2);
	sql.append(")");

	//std::cout << "\n" << sql << "\n" << std::endl;
	if (mysql_real_query(m_usMysql->m_mysql, sql.c_str(), sql.size()) != 0)
	{
		printf("mysql_real_query failed: error %u ( %s )",
			mysql_errno(m_usMysql->m_mysql), mysql_error(m_usMysql->m_mysql));
		return -1;
	}
}