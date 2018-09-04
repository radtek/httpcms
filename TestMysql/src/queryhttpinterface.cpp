#include "queryhttpinterface.h"
#include "pugixml.hpp"
#include <iostream>
#include <regex>
#include "Compenent.h"

MetaQuery::MetaQuery()
{
}

MetaQuery::~MetaQuery()
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

int MetaQuery::connectServer()
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

	std::string db = "metadata";
	rnt = m_usMysql->select_database(db);

	return 0;
}

int MetaQuery::metadata_http_get_query(
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
	
	/*对于fileds参数*/
	std::multimap<std::string, std::string> fileds;
	int ret = getQueryparaFileds(query_parameters, fileds);
	if (ret < 0)
	{
		std::string rntStr = "{\"status\": -1, \"msg\": \"请求参数有误\"}";
		response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));
		return 400;
	}
	for (auto itr = fileds.begin(); itr != fileds.end(); ++itr)
	{
		std::string s_first = itr->first;
		wipeStrSpace(s_first);
		std::string s_second = itr->second;
		wipeStrSpace(s_second);
		std::string index = m_filed->getFildNameIndex(s_first);
		if (index.empty())
		{
			std::string rntStr = "{\"status\": -1, \"msg\": \"请求参数有误,未找到该字段\"}";
			response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));
			return 400;
		}

		/*数值或者日期的范围查询*/
		if (*s_second.begin() == '[' && s_second.back() == ']')
		{
			if (m_filed->isdate(s_first) || m_filed->isNumQuery(s_first))
			{
				size_t pos = s_second.find_first_of(",");
				if (pos != std::string::npos)
				{
					std::string beg = s_second.substr(1, pos - 1);
					wipeStrSpace(beg);
					std::string end = s_second.substr(pos + 1, s_second.size() - pos - 2);
					wipeStrSpace(end);

					if (beg.empty() && !end.empty())
					{
						if (m_filed->isdate(s_first))
						{
							sql.append("(");
							if (end.size() == 4)
							{
								std::string s = "'" + end + "-12-31'";
								sql.append("(`").append(index).append("`) <= ").append(s).append(")");
							}
							else if (end.size() == 6)
							{
								std::string s = "'" + end.substr(0, 4).append("-").append(end.substr(4, 2)).append("-31").append("'");
								sql.append("(`").append(index).append("`) <= ").append(s).append(")");
							}
							else if (end.size() == 8)
							{
								std::string s = "'" + end.substr(0, 4).append("-").append(end.substr(4, 2)).append("-").append(end.substr(6, 2)) + "'";
								sql.append("(`").append(index).append("`) <= ").append(s).append(")");
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
							sql.append("(`").append(index).append("` <= ").append(end).append(")");
						}
					}
					else if (end.empty() && !beg.empty())
					{
						if (m_filed->isdate(s_first))
						{
							sql.append("(");
							if (beg.size() == 4)
							{
								std::string s = beg + "-01-01";
								sql.append("(`").append(index).append("`) >= ").append(s).append(")");
							}
							else if (beg.size() == 6)
							{
								std::string s = "'" + beg.substr(0, 4).append("-").append(beg.substr(4, 2)) + "-01" + "'";
								sql.append("(`").append(index).append("`) >= ").append(s).append(")");
							}
							else if (beg.size() == 8)
							{
								std::string s = "'" + beg.substr(0, 4).append("-").append(beg.substr(4, 2)).append("-").append(beg.substr(6, 2)) + "'";
								sql.append("(`").append(index).append("`) >= ").append(s).append(")");
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
							sql.append("(`").append(index).append("` >= ").append(beg).append(")");
						}
					}
					else if (!beg.empty() && !end.empty())
					{
						if (m_filed->isdate(s_first))
						{
							sql.append("(");
							if (beg.size() == 4 && end.size() == 4)
							{
								std::string s1 = "'" + beg + "-01-01'";
								std::string s2 = "'" + end + "-12-31'";
								sql.append("(`").append(index).append("`) between ").append(s1).append(" and ").append(s2).append(")");
							}
							else if (beg.size() == 8 || end.size() == 8)
							{
								std::string s1 = "'" + beg + "-01-01'";
								std::string s2 = "'" + end + "-12-31'";

								if (beg.size() == 6)
								{
									s1 = "'" + beg.substr(0, 4).append("-").append(beg.substr(4, 2)) + "-01" + "'";
								}
								if (end.size() == 6)
								{
									s2 = "'" + end.substr(0, 4).append("-").append(end.substr(4, 2)) + "-31" + "'";
								}

								if (beg.size() == 8)
								{
									s1 = "'" + beg.substr(0, 4).append("-").append(beg.substr(4, 2)).append("-").append(beg.substr(6, 2)) + "'";
								}
								if (end.size() == 8)
								{
									s2 = "'" + end.substr(0, 4).append("-").append(end.substr(4, 2)).append("-").append(end.substr(6, 2)) + "'";
								}

								sql.append("(`").append(index).append("`) between ").append(s1).append(" and ").append(s2).append(")");
							}
							else if (beg.size() == 6 || end.size() == 6)
							{
								std::string s1 = "'" + beg + "-01-01'";
								std::string s2 = "'" + end + "-12-31'";

								if (beg.size() == 6)
								{
									s1 = "'" + beg.substr(0, 4).append("-").append(beg.substr(4, 2)) + "-01" + "'";
								}
								if (end.size() == 6)
								{
									s2 = "'" + end.substr(0, 4).append("-").append(end.substr(4, 2)) + "-31" + "'";
								}

								sql.append("(`").append(index).append("`) between ").append(s1).append(" and ").append(s2).append(")");
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

		/*日期的精确查询*/
		else if (m_filed->isdate(s_first))
		{
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
				sql.append("(`").append(index).append("` = '").append(date8s).append("')");
			}
			sql.append(" and ");
		}
		
		/*字符串的模糊查询*/
		else if (m_filed->isStrView(s_first))
		{
			size_t posSpc = s_second.find_first_of(" ");
			s_second.push_back(' ');
			size_t posspc;
			size_t posbeg = 0;
			std::string regexp = "^.*?[";
			do
			{
				posspc = s_second.find_first_of(" ", posbeg);
				std::string substr = s_second.substr(posbeg, posspc - posbeg);
				std::string buf;
				buf.resize(100);
				int ret = mysql_real_escape_string(m_usMysql->m_mysql, (char*)buf.data(), substr.data(), substr.size());
				regexp.append(buf.data(), ret).append("|");
				//sql.append("(`").append(index).append("` LIKE '%").append(buf.data(), ret).append("%')").append(" or ");
				posbeg = posspc + 1;
			} while (posspc < s_second.size() - 1);
			//sql = sql.substr(0, sql.size() - 4); // 去掉结尾的or
			//sql.append(")");
			regexp = regexp.substr(0, regexp.size() - 1);
			regexp.append("].*$");
			sql.append("(`").append(index).append("` regexp '").append(regexp).append("')");

			sql.append(" and ");
		}
	
		/*其他项精确查询*/
		else
		{
			std::string buf;
			buf.resize(100);
			int ret = mysql_real_escape_string(m_usMysql->m_mysql, (char*)buf.data(), s_second.data(), s_second.size());
			sql.append("(`").append(index).append("` = '").append(buf.data(), ret).append("')");
			sql.append(" and ");
		}
	}
	fileds.clear();

	/*对于polygons参数*/
	std::vector<std::vector<double>> polygonsVect;
	ret = getQueryparaPolygons(query_parameters, polygonsVect);
	if (ret < 0)
	{
		std::string rntStr = "{\"status\": -1, \"msg\": \"请求参数有误\"}";
		response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));
		return 400;
	}
	int i_v = 0;
	for (auto itr = polygonsVect.begin(); itr != polygonsVect.end(); ++itr)
	{
		std::vector<std::string> vct; //vector中的数据按x,y,x,y存放
		for (auto it = itr->begin(); it != itr->end(); ++it)
		{
			char buf[100] = { 0 };
			sprintf_s(buf, "%lf", *it);
			std::string s = std::string(buf, strlen(buf));
			vct.push_back(s);
		}
		while (vct.size() < 4)
		{
			vct.push_back("");
		}
		while (itr->size() < 4)
		{
			itr->push_back(0);
		}

		std::string x_min = itr->at(0) < itr->at(2) ? vct[0] : vct[2];
		std::string x_max = itr->at(0) > itr->at(2) ? vct[0] : vct[2];
		std::string y_min = itr->at(1) < itr->at(3) ? vct[1] : vct[3];
		std::string y_max = itr->at(1) > itr->at(3) ? vct[1] : vct[3];
		vct.clear();
		
		if (!x_min.empty() && !x_max.empty() && itr->at(1) == 0 && itr->at(3) == 0)
		{
			sql.append("((`14` > ").append(x_min).append(" and ").append("`14` < ").append(x_max).append(")");
			sql.append(" or ");
			sql.append("(`15` > ").append(x_min).append(" and ").append("`15` < ").append(x_max).append("))");
		}
		else if (itr->at(0) == 0 && itr->at(2) == 0 && !y_min.empty() && !y_max.empty())
		{
			sql.append("((`16` > ").append(y_min).append(" and ").append("`16` < ").append(y_max).append(")");
			sql.append(" or ");
			sql.append("(`17` > ").append(y_min).append(" and ").append("`17` < ").append(y_max).append("))");
		}
		else if (!x_min.empty() && !x_max.empty() && !y_min.empty() && !y_max.empty())
		{
			sql.append("(((`14` > ").append(x_min).append(" and ").append("`14` < ").append(x_max).append(")");
			sql.append(" and ").append("((`16` > ").append(y_min).append(" and ").append("`16` < ").append(y_max).append(")");
			sql.append(" or ").append("(`17` > ").append(y_min).append(" and ").append("`17` < ").append(y_max).append(")))");

			sql.append(" or ");

			sql.append("((`15` > ").append(x_min).append(" and ").append("`15` < ").append(x_max).append(")");
			sql.append(" and ").append("((`16` > ").append(y_min).append(" and ").append("`16` < ").append(y_max).append(")");
			sql.append(" or ").append("(`17` > ").append(y_min).append(" and ").append("`17` < ").append(y_max).append(")))");

			sql.append(" or ");

			sql.append("((`16` > ").append(y_min).append(" and ").append("`16` < ").append(y_max).append(")");
			sql.append(" and ").append("((`14` > ").append(x_min).append(" and ").append("`14` < ").append(x_max).append(")");
			sql.append(" or ").append("(`15` > ").append(x_min).append(" and ").append("`15` < ").append(x_max).append(")))");

			sql.append(" or ");

			sql.append("((`17` > ").append(y_min).append(" and ").append("`17` < ").append(y_max).append(")");
			sql.append(" and ").append("((`14` > ").append(x_min).append(" and ").append("`14` < ").append(x_max).append(")");
			sql.append(" or ").append("(`15` > ").append(x_min).append(" and ").append("`15` < ").append(x_max).append("))))");
		}
		
		else
		{
			std::string rntStr = "{\"status\": -1, \"msg\": \"请求参数有误\"}";
			response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));
			return 400;
		}
		sql.append(" and ");
		i_v++;
	}
	polygonsVect.clear();
	
	/*对最后的sql语句处理*/
	size_t posand = sql.rfind("and");
	if (posand == sql.size() - 4)
	{
		sql = sql.substr(0, sql.size() - 5); //去掉结尾的and
	}
	else
	{
		sql = sql.substr(0, sql.size() - 6); //去掉结尾的where
	}

	/*对于filter参数*/
	std::vector<std::string> filters;
	ret = getFilters(query_parameters, filters);
	if (ret < 0)
	{
		std::string rntStr = "{\"status\": -1, \"msg\": \"请求参数有误\"}";
		response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));
		return 400;
	}
	if (!filters.empty())
	{
		std::string filterstr;
		for (auto it = filters.begin(); it != filters.end(); ++it)
		{
			std::string s = m_filed->getFildNameIndex(*it);
			filterstr.append("`").append(s).append("`, ");
		}
		filterstr = filterstr.substr(0, filterstr.size() - 2);
		sql = sql.substr(0, 6).append(" ").append(filterstr).append(" ").append(sql.substr(11));
	}
	filters.clear();

	/*对于size参数，默认是100*/
	int size = 100;
	size = getSize(query_parameters);
	char buf_size[100] = { 0 };
	sprintf_s(buf_size, "%d", size);
	std::string s_size = std::string(buf_size, strlen(buf_size));
	s_size = " limit " + s_size;
	sql.append(s_size);

	/*对于from参数，默认是0*/
	int from = 0;
	from = getFrom(query_parameters);
	char buf_from[100] = { 0 };
	sprintf_s(buf_from, "%d", from);
	std::string s_from = std::string(buf_from, strlen(buf_from));
	s_from = " offset " + s_from;
	sql.append(s_from);

	/*打印查询语句*/
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


int MetaQuery::getQueryparaFileds(const std::multimap< std::string, std::string > query_parameters, std::multimap< std::string, std::string > &fileds)
{
	for (auto itr = query_parameters.begin(); itr != query_parameters.end(); ++itr)
	{
		std::string firstStr = itr->first;
		wipeStrSpace(firstStr);
		std::string secondStr = itr->second;
		wipeStrSpace(secondStr);
		if (itr->first == "fileds")
		{
			size_t pos = secondStr.find_first_of("=");
			if (pos == std::string::npos)
			{
				return -1;  //格式不正确, 没有‘=’
			}
			
			std::string filedKey = secondStr.substr(0, pos);
			wipeStrSpace(filedKey);
			std::string filedValue = secondStr.substr(pos+1, secondStr.size()-pos-1);
			wipeStrSpace(filedValue);
			if (filedKey.empty())
			{
				return -2;  //fileds的键不能为空
			}
			fileds.insert(std::pair<std::string, std::string>(filedKey, filedValue));
		}
	}

	return 0;
}

int MetaQuery::getQueryparaPolygons(const std::multimap< std::string, std::string > query_parameters, std::vector<std::vector<double>> &rectv)
{
	for (auto itr = query_parameters.begin(); itr != query_parameters.end(); ++itr)
	{
		std::string firstStr = itr->first;
		wipeStrSpace(firstStr);
		std::string secondStr = itr->second;
		wipeStrSpace(secondStr);
		if (itr->first == "_polygons")
		{
			size_t pos = secondStr.find_first_of("=");
			if (pos == std::string::npos)
			{
				return -1;  //格式不正确, 没有‘=’
			}

			std::string polygonsValueKey = secondStr.substr(0, pos);
			wipeStrSpace(polygonsValueKey);
			std::string polygonsValueValue = secondStr.substr(pos+1, secondStr.size() - pos-1);
			wipeStrSpace(polygonsValueValue);
			if (polygonsValueKey.empty())
			{
				return -2;  //_polygons的键不能为空
			}

			if (polygonsValueKey == "rect")
			{
				if (*polygonsValueValue.begin() == '[' && polygonsValueValue.back() == ']')
				{
					polygonsValueValue = polygonsValueValue.substr(1, polygonsValueValue.size() - 2);
					polygonsValueValue.push_back(',');
					int times_v = 0;
					std::vector<double> rect;
					do
					{
						times_v++;
						size_t posvv = polygonsValueValue.find_first_of(",");
						if (posvv == std::string::npos)
						{
							return -4;  // rect的值不是[...]形式，4个值，里边要有‘,’分割
						}
						std::string s_v = polygonsValueValue.substr(0, posvv);
						rect.push_back(atof(s_v.c_str()));
						polygonsValueValue = polygonsValueValue.substr(posvv + 1, polygonsValueValue.size() - posvv - 1);
					} while (times_v <= 3);
					
					rectv.push_back(rect);
				}
				else
				{
					return -3;  //rect的值不是[...]形式
				}
			}

			if (polygonsValueKey == "circle")
			{
				if (*polygonsValueValue.begin() == '{' && polygonsValueValue.back() == '}')
				{
					size_t posvvp = polygonsValueValue.find_first_of("point");
					if (posvvp == std::string::npos)
					{
						return -5; //circle没有圆心
					}
					size_t posvvr = polygonsValueValue.find_first_of("r");
					if (posvvp == std::string::npos)
					{
						return -6; //circle没有半径
					}
					
					/*获取圆心坐标*/
					int len;
					std::string pointvalue;
					if (posvvr > posvvp)
					{
						len = posvvr - posvvp - 5;
						pointvalue = polygonsValueValue.substr(posvvp + 5, len);
					}
					else if (posvvr < posvvp)
					{
						pointvalue = polygonsValueValue.substr(posvvp + 5);
					}
					wipeStrSpace(pointvalue);
					if (polygonsValueValue.find_first_of("=") == std::string::npos)
					{
						return -7; //point或者r后面没有等号
					}
					size_t pos_1 = pointvalue.find_first_of("[");
					size_t pos_2 = pointvalue.find_first_of("]");
					if (pos_1 == std::string::npos || pos_2 == std::string::npos)
					{
						return -8;   //point后面没有[]的值
					}
					pointvalue = pointvalue.substr(pos_1 + 1, pos_2 - pos_1 - 1);
					size_t pos_3 = pointvalue.find_first_of(",");
					if (pos_3 == std::string::npos)
					{
						return -9;  //圆心不是一个点，横纵坐标用逗号隔开，如果坐标值为空，按0处理
					}
					std::string p_x = pointvalue.substr(0, pos_3);
					wipeStrSpace(p_x);
					std::string p_y = pointvalue.substr(pos_3+1, pointvalue.size()- pos_3-1);
					wipeStrSpace(p_y);
					
					/*获取半径值*/
					std::string rvalue;
					if (posvvr < posvvp) //判断r和point位置的前后
					{
						len = posvvp - posvvr - 1;
						rvalue = polygonsValueValue.substr(posvvr + 1, len);
					}
					else if (posvvr > posvvp)
					{
						rvalue = polygonsValueValue.substr(posvvr + 1);
					}
					wipeStrSpace(rvalue);
					size_t pops_4 = rvalue.find_first_of("=");
					if (pops_4 == std::string::npos)
					{
						return -7; //point或者r后面没有等号 
					}
					rvalue = rvalue.substr(pops_4 + 1, rvalue.size() - 2);
					wipeStrSpace(rvalue);
					double p_xd = atof(p_x.c_str());
					double p_yd = atof(p_y.c_str());
					double rd = atof(rvalue.c_str());
					std::vector<double> circlerect;
					circlerect.push_back(p_xd - rd);
					circlerect.push_back(p_yd - rd);
					circlerect.push_back(p_xd + rd);
					circlerect.push_back(p_yd + rd);
					rectv.push_back(circlerect);
				}
				else
				{
					return -10;  //circle的值不是{...}形式
				}
			}

		}
	}

	return 0;
}

int MetaQuery::getFrom(const std::multimap< std::string, std::string > query_parameters)
{
	int from = 0;
	for (auto itr = query_parameters.begin(); itr != query_parameters.end(); ++itr)
	{
		std::string firstStr = itr->first;
		wipeStrSpace(firstStr);
		std::string secondStr = itr->second;
		wipeStrSpace(secondStr);
		if (firstStr == "from")
		{
			from = atoi(secondStr.c_str());
		}
	}

	return from;
}

int MetaQuery::getSize(const std::multimap< std::string, std::string > query_parameters)
{
	int size = 100;
	for (auto itr = query_parameters.begin(); itr != query_parameters.end(); ++itr)
	{
		std::string firstStr = itr->first;
		wipeStrSpace(firstStr);
		std::string secondStr = itr->second;
		wipeStrSpace(secondStr);
		if (firstStr == "size")
		{
			size = atoi(secondStr.c_str());
		}
	}

	return size;
}

int MetaQuery::getFilters(const std::multimap< std::string, std::string > query_parameters, std::vector<std::string> &vct)
{
	for (auto itr = query_parameters.begin(); itr != query_parameters.end(); ++itr)
	{
		std::string firstStr = itr->first;
		wipeStrSpace(firstStr);
		std::string secondStr = itr->second;
		wipeStrSpace(secondStr);
		if (firstStr == "filter")
		{
			vct.push_back(secondStr);
		}
	}

	return 0;
}

int MetaQuery::addFiledValuesFromXML(const std::string path)
{
	pugi::xml_document doc;

	pugi::xml_parse_result result = doc.load_file(path.c_str());
	if (!result)
	{
		return -1;
	}
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
	for (pugi::xml_node chnode = root.first_child(); chnode; chnode = chnode.next_sibling())
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
	int out_size = 0;
	const us_cms_metdata_map *metamap = us_get_cms_metadata_map_table(&out_size);

	for (int i = 0; i < out_size; i++)
	{
		std::cout << metamap->m_custon_field << " " << metamap->m_db_field << " " << metamap->m_type << " " << metamap->m_mode << std::endl;
		metamap++;
	}

	return 200;
}
