#include "http.h"
#include "field.h"
#include <iostream>
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


#define BUFMAXSIZE 50

#if 0

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

	/*�������ݿ�*/
	std::string hostip = "localhost";
	int port = 3306;
	std::string user = "root";
	std::string password = "";
	std::string database = "metadata2";
	std::string tablename = "ngcc_metadata";

	///*��ȡ�����ļ���json�ļ�����ȡip�Ͷ˿��Լ����ݿ�����*/
	//rapidjson::Document doc_cfg;
	//FILE *myFile = NULL;
	//fopen_s(&myFile, "./ngcc_metadata_config.json", "rb");
	//if (myFile) 
	//{
	//	char buf_cfg[4096] = { 0 };
	//	rapidjson::FileReadStream inputStream(myFile, buf_cfg, sizeof(buf_cfg));  //����һ��������
	//	doc_cfg.ParseStream(inputStream); //����ȡ������ת��ΪdomԪ��
	//	fclose(myFile); //�ر��ļ�������Ҫ
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
	
	/*�������ݿ�*/
	MYSQL * m_mysql = mysql_init(NULL);  //�ڳ���ʹ��Mysql���ʼ�������mysql_int()��ʼ��
	if (m_mysql == NULL)
	{
		std::string rntStr = u8"{\"status\": 1, \"msg\": \"mysql init failed\"}";
		response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));
		return 500;
	}
	int arg = 1;
	mysql_options(m_mysql, MYSQL_OPT_RECONNECT, &arg);  //�Ͽ�����

	if (mysql_real_connect(m_mysql, hostip.c_str(),
		user.c_str(), password.c_str(), database.c_str(), port, NULL, 0) == NULL)
	{
		std::string rntStr =u8"{\"status\": 2, \"msg\": \"mysql_real_connect failed\"}";
		response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));
		return 500;
	}

	/*��ȡ�ֶ�ӳ���*/
	int out_size = 0;
	const us_cms_metdata_map *metamap = us_get_cms_metadata_map_table(&out_size);
	if (metamap == nullptr || out_size < 0)
	{
		std::string rntStr = u8"{\"status\": 3, \"msg\": \"��ȡ�ֶ�ӳ���ʧ��\"}";
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

	/*sql��䣬��ȡ����������url�ֶ�*/
	std::string sql = "select `url`";
	sql.append(" from ").append(tablename).append(" where ");

	/*����request_body�е�json�ַ���*/
	std::string jsonstr;
	for (auto itr = request_body.begin(); itr != request_body.end(); ++itr)
	{
		jsonstr.push_back(*itr);
	}
	rapidjson::Document doc1;
	doc1.Parse(jsonstr.c_str());  
	if (doc1.HasParseError())
	{
		std::string rntStr = u8"{\"status\": 4, \"msg\": \"����ʵ��json��ʽ����ȷ\"}";
		response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));
		return 400;
	}

	/*query����*/
	if (doc1.HasMember("query"))
	{
		if (doc1["query"].IsObject())
		{
			rapidjson::Value &queryobj = doc1["query"];

			/*match��������ȷ��ѯ���ַ����������ѯ��*/
			if (queryobj.HasMember("match"))
			{
				if (queryobj["match"].IsArray())
				{
					for (int i = 0; i < queryobj["match"].Size(); ++i)
					{
						/*��ȡ�ֶ���*/
						std::string fieldname;
						if (queryobj["match"][i].HasMember("field"))
						{
							if (queryobj["match"][i]["field"].IsString())
							{
								fieldname = queryobj["match"][i]["field"].GetString();
							}
						}
						/*��ȡ��ѯģʽ*/
						int mode = 0;
						if (queryobj["match"][i].HasMember("mode"))
						{
							if (queryobj["match"][i]["mode"].IsInt64())
							{
								mode = queryobj["match"][i]["mode"].GetInt64();
							}
						}
						/*��ȡƥ�䴮*/
						std::string pattern;
						if (queryobj["match"][i].HasMember("pattern"))
						{
							if (queryobj["match"][i]["pattern"].IsString()) //�ַ�������
							{
								pattern = queryobj["match"][i]["pattern"].GetString();
							}
							if (queryobj["match"][i]["pattern"].IsInt64())  //��������
							{
								int pattern_int = queryobj["match"][i]["pattern"].GetInt64();
								pattern = std::to_string(pattern_int);
							}
							if (queryobj["match"][i]["pattern"].IsDouble())  //����������
							{
								double pattern_db = queryobj["match"][i]["pattern"].GetDouble();
								pattern = std::to_string(pattern_db);
							}
						}
						/*���pattern��field��mode�ĺϷ���*/
						std::string dbfield;
						bool mode_flg = 0;
						bool find_field_flg = 0;
						auto itr = metamaps.begin();
						for (; itr != metamaps.end(); ++itr)
						{
							/*�����û��ɼ����ֶ�����ȡ���ݿ��ж�Ӧ���ֶ����� ���ж����Ӧ�Ĳ�ѯģʽ*/
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
						/*û���ҵ��ֶ����������ǲ�ѯģʽ����ȷ*/
						if (itr == metamaps.end())
						{
							/*��ѯģʽ����ȷ*/
							if (find_field_flg == 1 && mode_flg == 0)
							{
								std::string rntStr = u8"{\"status\": 4, \"msg\": \"" + fieldname + " ��ѯģʽ����ȷ\"}";
								response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));
								return 400;
							}
							/*δ�ҵ��ֶ�*/
							else if (find_field_flg == 0)
							{
								std::string rntStr = u8"{\"status\": 5, \"msg\": \"δ�ҵ� '" + fieldname + "' �ֶ�\"}";
								response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));
								return 400;
							}
						}
						if (pattern.empty())
						{
							continue; //���ƥ�䴮Ϊ�գ��������ֶβ�����ѯ
						}
						/*���ݲ�ͬ��ѯģʽ��дsql���*/
						if (mode == 0x01)	//��������ʽ��ѯ
						{
							pattern.push_back(' ');
							size_t posspc;
							size_t posbeg = 0;
							/*�����ѯ��ƥ�䴮�Կո�ָ����д��������ʽ*/
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
							regexp = regexp.substr(0, regexp.size() - 1); //ȥ����β�� |
							regexp.append("].*$");
							sql.append("(`").append(dbfield).append("` regexp '").append(regexp).append("')");
						}
						else if (mode == 0x02)	//�ַ�����ȷ��ѯ
						{
							sql.append("(`").append(dbfield).append("` = '").append(pattern).append("')");
						}
						else if (mode == 0x04)	//���ڵľ�ȷ��ѯ
						{
							sql.append("(`").append(dbfield).append("` = ").append(pattern).append(")");
						}
						else if (mode == 0x10)	//��ֵ�ľ�ȷ��ѯ
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

			/*range���� ����ֵ�������ڵķ�Χ��ѯ��*/
			if (queryobj.HasMember("range"))
			{
				if (queryobj["range"].IsArray())
				{
					for (int i = 0; i < queryobj["range"].Size(); ++i)
					{
						/*�ֶ���*/
						std::string fieldname;
						if (queryobj["range"][i].HasMember("field"))
						{
							if (queryobj["range"][i]["field"].IsString())
							{
								fieldname = queryobj["range"][i]["field"].GetString();
							}
						}
						/*��Сֵ*/
						std::string min_s;
						if (queryobj["range"][i].HasMember("min"))
						{
							if (queryobj["range"][i]["min"].IsInt64())   //��������
							{
								min_s = std::to_string(queryobj["range"][i]["min"].GetInt64());
							}
							if (queryobj["range"][i]["min"].IsDouble())  //����������
							{
								min_s = std::to_string(queryobj["range"][i]["min"].GetDouble());
							}
						}
						/*���ֵ*/
						std::string max_s;
						if (queryobj["range"][i].HasMember("max"))
						{
							if (queryobj["range"][i]["max"].IsInt64())  //��������
							{
								max_s = std::to_string(queryobj["range"][i]["max"].GetInt64());
							}
							if (queryobj["range"][i]["max"].IsDouble())  //����������
							{
								max_s = std::to_string(queryobj["range"][i]["max"].GetDouble());
							}
						}
						/*���field��mode�ĺϷ���*/
						std::string dbfield;
						bool mode_flg = 0;  //��ѯģʽ ��ȷΪ1
						bool find_field_flg = 0;  //�ֶ��Ƿ���ȷ����ȷΪ1
						auto itr = metamaps.begin();
						for (; itr != metamaps.end(); ++itr)
						{
							if (itr->m_custon_field == fieldname)
							{
								find_field_flg = 1;
								dbfield = std::to_string(itr->m_db_field); //��ȡ���ֶζ�Ӧ�����ݿ��ֶ�
								if ((itr->m_mode & 0x08) >= 1 || (itr->m_mode & 0x20) >= 1)   //0x08 �����ڵķ�Χ���� 0x20����ֵ�ķ�Χ����
								{
									mode_flg = 1;
									break;
								}
							}
						}
						if (itr == metamaps.end())
						{
							/*��ѯģʽ����ȷ*/
							if (find_field_flg == 1 && mode_flg == 0)
							{
								std::string rntStr = "{\"status\": 6, \"msg\": \"" + fieldname + u8"�޷����з�Χ��ѯ\"}";
								response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));
								return 400;
							}
							/*δ�ҵ��ֶ�*/
							else if (find_field_flg == 0)
							{
								std::string rntStr = u8"{\"status\": 7, \"msg\": \"δ�ҵ� '" + fieldname + u8"' �ֶ�\"}";
								response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));
								return 400;
							}
						}
						/*ֻ�����ֵ�����*/
						if (min_s.empty() && !max_s.empty())
						{
							sql.append("(`").append(dbfield).append("` <= ").append(max_s).append(")");
							sql.append(" and ");
						}
						/*ֻ����Сֵ�����*/
						else if (!min_s.empty() && max_s.empty())
						{
							sql.append("(`").append(dbfield).append("` >= ").append(min_s).append(")");
							sql.append(" and ");
						}
						/*�����Сֵ������*/
						else if (!min_s.empty() && !max_s.empty())
						{
							sql.append("(`").append(dbfield).append("` between ").append(min_s).append(" and ").append(max_s).append(")");
							sql.append(" and ");
						}
					}
				}
			}

			/*geometry���� ������Χ�Ĳ�ѯ[x,y,x,y,x,y...]��*/
			rapidjson::Value::ConstMemberIterator iter = queryobj.FindMember("geometry");  //ʹ�õ���������geometry����
			if (iter != queryobj.MemberEnd() && iter->value.IsObject())
			{
				const rapidjson::Value &geometryObj = iter->value; //��ȡgeometry��ֵ
				rapidjson::Value::ConstMemberIterator iter_polygon = geometryObj.FindMember("polygon");  //ʹ�õ�����ѭ������polygon����
				for (; iter_polygon != queryobj.MemberEnd() && iter_polygon->name == "polygon" && iter_polygon->value.IsArray(); ++iter_polygon)
				{
					std::vector<double> x_v;
					std::vector<double> y_v;
					const rapidjson::Value &polygonArray = iter_polygon->value;  //��ȡpolygon��ֵ
					for (int i = 0; i < polygonArray.Size(); ++i)
					{
						if (i % 2 == 0)  //��ȡ�������ֵ
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
						else   //��ȡ�������ֵ
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
					/*��������*/
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

					/*��ȡ���ݿ��е��ֶ���*/
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

					/*��дsql��ѯ���*/
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






	/*query����*/
	//if (doc1.HasMember("query"))
	//{
	//	const rapidjson::Value &queryobj = doc1["query"];  //��ȡquery��ֵ

	//	/*match��������ȷ��ѯ���ַ����������ѯ��*/
	//	rapidjson::Value::ConstMemberIterator iter = queryobj.FindMember("match");  //ʹ�õ���������match����
	//	if (iter != queryobj.MemberEnd() && iter->value.IsArray())
	//	{
	//		const rapidjson::Value &matchArray = iter->value;  //��ȡmatch��ֵ
	//		for (int i = 0; i < queryobj["match"].Size(); ++i)
	//		{
	//			/*��ȡ�ֶ���*/
	//			std::string fieldname;
	//			rapidjson::Value::ConstMemberIterator iter_match = matchArray[i].FindMember("field");
	//			if (iter_match != matchArray[i].MemberEnd() && iter_match->value.IsString())
	//			{
	//				fieldname = iter_match->value.GetString();
	//			}
	//			/*��ȡ��ѯģʽ*/
	//			int mode = 0;
	//			iter_match = matchArray[i].FindMember("mode");
	//			if (iter_match != matchArray[i].MemberEnd() && iter_match->value.IsInt())
	//			{
	//				mode = iter_match->value.GetInt();
	//			}
	//			/*��ȡƥ�䴮*/
	//			std::string pattern;
	//			iter_match = matchArray[i].FindMember("pattern");
	//			if (iter_match != matchArray[i].MemberEnd())
	//			{
	//				if (iter_match->value.IsString()) //�ַ�������
	//				{
	//					pattern = iter_match->value.GetString();
	//				}
	//				if (iter_match->value.IsInt())  //��������
	//				{
	//					int pattern_int = iter_match->value.GetInt();
	//					pattern = std::to_string(pattern_int);
	//				}
	//				if (iter_match->value.IsDouble())  //����������
	//				{
	//					double pattern_db = iter_match->value.GetDouble();
	//					pattern = std::to_string(pattern_db);
	//				}
	//			}
	//			/*���pattern��field��mode�ĺϷ���*/
	//			std::string dbfield;
	//			bool mode_flg = 0;
	//			bool find_field_flg = 0;
	//			auto itr = metamaps.begin();
	//			for (; itr != metamaps.end(); ++itr)
	//			{
	//				/*�����û��ɼ����ֶ�����ȡ���ݿ��ж�Ӧ���ֶ����� ���ж����Ӧ�Ĳ�ѯģʽ*/
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
	//			/*û���ҵ��ֶ����������ǲ�ѯģʽ����ȷ*/
	//			if (itr == metamaps.end())
	//			{
	//				/*��ѯģʽ����ȷ*/
	//				if (find_field_flg == 1 && mode_flg == 0)
	//				{
	//					std::string rntStr = "{\"status\": 5, \"msg\": \"" + fieldname + " ��ѯģʽ����ȷ\"}";
	//					response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));
	//					return 400;
	//				}
	//				/*δ�ҵ��ֶ�*/
	//				else if (find_field_flg == 0)
	//				{
	//					std::string rntStr = "{\"status\": 6, \"msg\": \"δ�ҵ� '" + fieldname + "' �ֶ�\"}";
	//					response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));
	//					return 400;
	//				}
	//			}
	//			if (pattern.empty())
	//			{
	//				continue; //���ƥ�䴮Ϊ�գ��������ֶβ�����ѯ
	//			}
	//			/*���ݲ�ͬ��ѯģʽ��дsql���*/
	//			if (mode == 0x01)	//��������ʽ��ѯ
	//			{
	//				pattern.push_back(' ');
	//				size_t posspc;
	//				size_t posbeg = 0;
	//				/*�����ѯ��ƥ�䴮�Կո�ָ����д��������ʽ*/
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
	//				regexp = regexp.substr(0, regexp.size() - 1); //ȥ����β�� |
	//				regexp.append("].*$");
	//				sql.append("(`").append(dbfield).append("` regexp '").append(regexp).append("')");
	//			}
	//			else if (mode == 0x02)	//�ַ�����ȷ��ѯ
	//			{
	//				sql.append("(`").append(dbfield).append("` = '").append(pattern).append("')");
	//			}
	//			else if (mode == 0x04)	//���ڵľ�ȷ��ѯ
	//			{
	//				sql.append("(`").append(dbfield).append("` = ").append(pattern).append(")");
	//			}
	//			else if (mode == 0x10)	//��ֵ�ľ�ȷ��ѯ
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

	//	/*range���� ����ֵ�������ڵķ�Χ��ѯ��*/
	//	iter = queryobj.FindMember("range");   //ʹ�õ���������range����
	//	if (iter != queryobj.MemberEnd() && iter->value.IsArray())
	//	{
	//		const rapidjson::Value &rangeArray = iter->value; //��ȡrange������ֵ
	//		for (int i = 0; i < rangeArray.Size(); ++i)
	//		{
	//			/*�ֶ���*/
	//			std::string fieldname;
	//			rapidjson::Value::ConstMemberIterator iter_range = rangeArray[i].FindMember("field");
	//			if (iter_range != rangeArray[i].MemberEnd() && iter_range->value.IsString())
	//			{
	//				fieldname = iter_range->value.GetString();
	//			}
	//			/*��Сֵ*/
	//			std::string min_s;
	//			iter_range = rangeArray[i].FindMember("min");
	//			if (iter_range != rangeArray[i].MemberEnd())
	//			{
	//				if (iter_range->value.IsInt())   //��������
	//				{
	//					min_s = std::to_string(iter_range->value.GetInt());
	//				}
	//				if (iter_range->value.IsDouble())  //����������
	//				{
	//					min_s = std::to_string(iter_range->value.GetDouble());
	//				}
	//			}
	//			/*���ֵ*/
	//			std::string max_s;
	//			iter_range = rangeArray[i].FindMember("max");
	//			if (iter_range != rangeArray[i].MemberEnd())
	//			{
	//				if (iter_range->value.IsInt())  //��������
	//				{
	//					max_s = std::to_string(iter_range->value.GetInt());
	//				}
	//				if (iter_range->value.IsDouble())  //����������
	//				{
	//					max_s = std::to_string(iter_range->value.GetDouble());
	//				}
	//			}
	//			/*���field��mode�ĺϷ���*/
	//			std::string dbfield;
	//			//int dateflg = 0;
	//			bool mode_flg = 0;  //��ѯģʽ ��ȷΪ1
	//			bool find_field_flg = 0;  //�ֶ��Ƿ���ȷ����ȷΪ1
	//			auto itr = metamaps.begin();
	//			for (; itr != metamaps.end(); ++itr)
	//			{
	//				if (itr->m_custon_field == fieldname)
	//				{
	//					find_field_flg = 1;
	//					dbfield = std::to_string(itr->m_db_field); //��ȡ���ֶζ�Ӧ�����ݿ��ֶ�
	//					if ((itr->m_mode & 0x08) >= 1 || (itr->m_mode & 0x20) >= 1)   //0x08 �����ڵķ�Χ���� 0x20����ֵ�ķ�Χ����
	//					{
	//						mode_flg = 1;
	//						break;
	//					}
	//				}
	//			}
	//			if (itr == metamaps.end())
	//			{
	//				/*��ѯģʽ����ȷ*/
	//				if (find_field_flg == 1 && mode_flg == 0)
	//				{
	//					std::string rntStr = "{\"status\": 7, \"msg\": \"" + fieldname + "�޷����з�Χ��ѯ\"}";
	//					response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));
	//					return 400;
	//				}
	//				/*δ�ҵ��ֶ�*/
	//				else if (find_field_flg == 0)
	//				{
	//					std::string rntStr = "{\"status\": 8, \"msg\": \"δ�ҵ� '" + fieldname + "' �ֶ�\"}";
	//					response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));
	//					return 400;
	//				}
	//			}
	//			
	//			/*ֻ�����ֵ�����*/
	//			if (min_s.empty() && !max_s.empty())
	//			{
	//				sql.append("(`").append(dbfield).append("` <= ").append(max_s).append(")");
	//				sql.append(" and ");
	//			}
	//			/*ֻ����Сֵ�����*/
	//			else if (!min_s.empty() && max_s.empty())
	//			{
	//				sql.append("(`").append(dbfield).append("` >= ").append(min_s).append(")");
	//				sql.append(" and ");
	//			}
	//			/*�����Сֵ������*/
	//			else if (!min_s.empty() && !max_s.empty())
	//			{
	//				sql.append("(`").append(dbfield).append("` between ").append(min_s).append(" and ").append(max_s).append(")");
	//				sql.append(" and ");
	//			}
	//		}
	//		
	//	}

	//	/*geometry���� ������Χ�Ĳ�ѯ[x,y,x,y,x,y...]��*/
	//	iter = queryobj.FindMember("geometry");  //ʹ�õ���������geometry����
	//	if (iter != queryobj.MemberEnd() && iter->value.IsObject())
	//	{

	//		const rapidjson::Value &geometryObj = iter->value; //��ȡgeometry��ֵ
	//		rapidjson::Value::ConstMemberIterator iter_geometry = geometryObj.FindMember("polygon");  //ʹ�õ�����ѭ������polygon����
	//		for (; iter_geometry != queryobj.MemberEnd() && iter_geometry->name == "polygon" && iter_geometry->value.IsArray(); ++iter_geometry)
	//		{
	//			std::vector<double> x_v;
	//			std::vector<double> y_v;
	//			const rapidjson::Value &polygonArray = iter_geometry->value;  //��ȡpolygon��ֵ
	//			for (int i = 0; i < polygonArray.Size(); ++i)
	//			{
	//				if (i % 2 == 0)  //��ȡ�������ֵ
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
	//				else   //��ȡ�������ֵ
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
	//				std::string rntStr = "{\"status\": 9, \"msg\": \"����ʵ��polygon����\"}";  //�����и�x,y
	//				response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));
	//				return 400;
	//			}
	//			/*��������*/
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

	//			/*��ȡ���ݿ��е��ֶ���*/
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

	//			/*��дsql��ѯ���*/
	//			std::string minx_miny = x_min_s + " " + y_min_s;
	//			std::string maxx_miny = x_max_s + " " + y_min_s;
	//			std::string maxx_maxy = x_max_s + " " + y_max_s;
	//			std::string minx_maxy = x_min_s + " " + y_max_s;
	//			sql.append("MBRIntersects(ST_GeomFromText('Polygon((").append(minx_miny).append(",").append(maxx_miny).append(",").
	//				append(maxx_maxy).append(",").append(minx_maxy).append(",").append(minx_miny).append("))'), `5`)");   //������Ƿ��ཻ
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


	/*query����*/
	//if (doc1.HasMember("query"))
	//{
	//	if (doc1["query"].IsObject())
	//	{
	//		rapidjson::Value &queryobj = doc1["query"];
	//		
	//		/*match��������ȷ��ѯ���ַ����������ѯ��*/
	//		if (queryobj.HasMember("match"))
	//		{
	//			if (queryobj["match"].IsArray())
	//			{
	//				for (int i = 0; i < queryobj["match"].Capacity(); ++i)
	//				{
	//					/*��ȡ�ֶ���*/
	//					std::string fieldname;
	//					if (queryobj["match"][i].HasMember("field"))
	//					{
	//						if (queryobj["match"][i]["field"].IsString())
	//						{
	//							fieldname = queryobj["match"][i]["field"].GetString();
	//						}
	//					}
	//					/*��ȡ��ѯģʽ*/
	//					int mode = 0;
	//					if (queryobj["match"][i].HasMember("mode"))
	//					{
	//						if (queryobj["match"][i]["mode"].IsInt())
	//						{
	//							mode = queryobj["match"][i]["mode"].GetInt();
	//						}
	//					}
	//					/*��ȡƥ�䴮*/
	//					std::string pattern;
	//					if (queryobj["match"][i].HasMember("pattern"))
	//					{
	//						if (queryobj["match"][i]["pattern"].IsString()) //�ַ�������
	//						{
	//							pattern = queryobj["match"][i]["pattern"].GetString();
	//						}
	//						if (queryobj["match"][i]["pattern"].IsInt())  //��������
	//						{
	//							int pattern_int = queryobj["match"][i]["pattern"].GetInt();
	//							pattern = std::to_string(pattern_int);
	//						}
	//						if (queryobj["match"][i]["pattern"].IsDouble())  //����������
	//						{
	//							double pattern_db = queryobj["match"][i]["pattern"].GetDouble();
	//							pattern = std::to_string(pattern_db);
	//						}
	//					}
	//					/*���pattern��field��mode�ĺϷ���*/
	//					std::string dbfield;
	//					bool mode_flg = 0;
	//					bool find_field_flg = 0;
	//					auto itr = metamaps.begin();
	//					for (; itr != metamaps.end(); ++itr)
	//					{
	//						/*�����û��ɼ����ֶ�����ȡ���ݿ��ж�Ӧ���ֶ����� ���ж����Ӧ�Ĳ�ѯģʽ*/
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
	//					/*û���ҵ��ֶ����������ǲ�ѯģʽ����ȷ*/
	//					if (itr == metamaps.end())  
	//					{
	//						/*��ѯģʽ����ȷ*/
	//						if (find_field_flg == 1 && mode_flg == 0)
	//						{
	//							std::string rntStr = "{\"status\": 5, \"msg\": \"" + fieldname + " ��ѯģʽ����ȷ\"}";
	//							response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));
	//							return 400;
	//						}
	//						/*δ�ҵ��ֶ�*/
	//						else if (find_field_flg == 0)
	//						{
	//							std::string rntStr = "{\"status\": 6, \"msg\": \"δ�ҵ� '" + fieldname + "' �ֶ�\"}";
	//							response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));
	//							return 400;
	//						}
	//					}
	//					if (pattern.empty())
	//					{
	//						continue; //���ƥ�䴮Ϊ�գ��������ֶβ�����ѯ
	//					}
	//					/*���ݲ�ͬ��ѯģʽ��дsql���*/
	//					if (mode == 0x01)	//��������ʽ��ѯ
	//					{
	//						pattern.push_back(' ');
	//						size_t posspc;
	//						size_t posbeg = 0;
	//						/*�����ѯ��ƥ�䴮�Կո�ָ����д��������ʽ*/
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
	//						regexp = regexp.substr(0, regexp.size() - 1); //ȥ����β�� |
	//						regexp.append("].*$");
	//						sql.append("(`").append(dbfield).append("` regexp '").append(regexp).append("')");
	//					}
	//					else if (mode == 0x02)	//�ַ�����ȷ��ѯ
	//					{
	//						sql.append("(`").append(dbfield).append("` = '").append(pattern).append("')");
	//					}
	//					else if (mode == 0x04)	//���ڵľ�ȷ��ѯ
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
	//					else if (mode == 0x10)	//��ֵ�ľ�ȷ��ѯ
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

	//		/*range���� ����ֵ�������ڵķ�Χ��ѯ��*/
	//		if (queryobj.HasMember("range"))
	//		{
	//			if (queryobj["range"].IsArray())
	//			{
	//				for (int i = 0; i < queryobj["range"].Capacity(); ++i)
	//				{
	//					/*�ֶ���*/
	//					std::string fieldname;
	//					if (queryobj["range"][i].HasMember("field"))
	//					{
	//						if (queryobj["range"][i]["field"].IsString())
	//						{
	//							fieldname = queryobj["range"][i]["field"].GetString();
	//						}
	//					}
	//					/*��Сֵ*/
	//					std::string min_s;
	//					if (queryobj["range"][i].HasMember("min"))
	//					{
	//						if (queryobj["range"][i]["min"].IsInt())   //��������
	//						{
	//							min_s = std::to_string(queryobj["range"][i]["min"].GetInt());
	//						}
	//						if (queryobj["range"][i]["min"].IsDouble())  //����������
	//						{
	//							min_s = std::to_string(queryobj["range"][i]["min"].GetDouble());
	//						}
	//					}
	//					/*���ֵ*/
	//					std::string max_s;
	//					if (queryobj["range"][i].HasMember("max"))
	//					{
	//						if (queryobj["range"][i]["max"].IsInt())  //��������
	//						{
	//							max_s = std::to_string(queryobj["range"][i]["max"].GetInt());
	//						}
	//						if (queryobj["range"][i]["max"].IsDouble())  //����������
	//						{
	//							max_s = std::to_string(queryobj["range"][i]["max"].GetDouble());
	//						}
	//					}
	//					/*���field��mode�ĺϷ���*/
	//					std::string dbfield;
	//					//int dateflg = 0;
	//					bool mode_flg = 0;  //��ѯģʽ ��ȷΪ1
	//					bool find_field_flg = 0;  //�ֶ��Ƿ���ȷ����ȷΪ1
	//					auto itr = metamaps.begin();
	//					for (; itr != metamaps.end(); ++itr)
	//					{
	//						if (itr->m_custon_field == fieldname)
	//						{
	//							find_field_flg = 1;
	//							dbfield = std::to_string(itr->m_db_field); //��ȡ���ֶζ�Ӧ�����ݿ��ֶ�
	//							if ((itr->m_mode & 0x08) >= 1 || (itr->m_mode & 0x20) >= 1)   //0x08 �����ڵķ�Χ���� 0x20����ֵ�ķ�Χ����
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
	//						/*��ѯģʽ����ȷ*/
	//						if (find_field_flg == 1 && mode_flg == 0)
	//						{
	//							std::string rntStr = "{\"status\": 7, \"msg\": \"" + fieldname + "�޷����з�Χ��ѯ\"}";
	//							response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));
	//							return 400;
	//						}
	//						/*δ�ҵ��ֶ�*/
	//						else if (find_field_flg == 0)
	//						{
	//							std::string rntStr = "{\"status\": 8, \"msg\": \"δ�ҵ� '" + fieldname + "' �ֶ�\"}";
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
	//					/*ֻ�����ֵ�����*/
	//					if (min_s.empty() && !max_s.empty())
	//					{
	//						sql.append("(`").append(dbfield).append("` <= ").append(max_s).append(")");
	//						sql.append(" and ");
	//					}
	//					/*ֻ����Сֵ�����*/
	//					else if (!min_s.empty() && max_s.empty())
	//					{
	//						sql.append("(`").append(dbfield).append("` >= ").append(min_s).append(")");
	//						sql.append(" and ");
	//					}
	//					/*�����Сֵ������*/
	//					else if (!min_s.empty() && !max_s.empty())
	//					{
	//						sql.append("(`").append(dbfield).append("` between ").append(min_s).append(" and ").append(max_s).append(")");
	//						sql.append(" and ");
	//					}
	//				}
	//			}
	//		}

	//		/*geometry���� ������Χ�Ĳ�ѯ[x,y,x,y,x,y...]��*/
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
	//							if (i % 2 == 0)  //��ȡ�������ֵ
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
	//							else   //��ȡ�������ֵ
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
	//							std::string rntStr = "{\"status\": 9, \"msg\": \"����ʵ��polygon����\"}";  //�����и�x,y
	//							response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));
	//							return 400;
	//						}
	//						/*��������*/
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

	//						/*��ȡ���ݿ��е��ֶ���*/
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

	//						/*��дsql��ѯ���*/
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
	/*��sql����β���ִ���*/
	size_t posand = sql.rfind("and");
	if (posand == sql.size() - 4)
	{
		sql = sql.substr(0, sql.size() - 5); //ȥ����β��and
	}
	else
	{
		sql = sql.substr(0, sql.size() - 6); //ȥ����β��where
	}

	/*size��������ȡ������¼,Ĭ��100*/
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

	/*from�������ӵڼ�����¼��ʼ��ȡ*/
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

	/*ִ��sql��ѯ�������ز�ѯ���*/
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

	/*�����˶�����*/
	unsigned long long rows = mysql_num_rows(result);
	std::cout << rows << std::endl;
	std::string rowstr = std::to_string(rows);
	if (rows < 1)
	{
		std::string rntStr = u8"{\"status\": 12, \"msg\": \"δ�ҵ���Ӧ�ļ�¼\"}";
		response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));
		return 404;
	}

	rapidjson::StringBuffer strbuf;
	strbuf.Reserve(rows * 2000);
	rapidjson::Writer<rapidjson::StringBuffer> writer(strbuf);
	/*����ȡ���Ľ��д��json*/
	writer.StartObject();
	writer.Key("urls");
	writer.StartArray();

	MYSQL_ROW rown; //һ�����ݵ����Ͱ�ȫ��ʾ
					/*std::string rntstr = "{\"urls\": [ ";
					response_body.insert(response_body.end(), (uint8_t*)rntstr.data(), (uint8_t*)(rntstr.data() + rntstr.size()));*/
	int numrow = 0;
	while ((rown = mysql_fetch_row(result)) != NULL)
	{
		numrow++;
		unsigned long * row_len = mysql_fetch_lengths(result);
		for (int i = 0; i < mysql_num_fields(result); ++i)  //ֻ��һ���ֶ�url
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

#endif

#if 0
int us_cms_http_post_rebuild1(
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

	/*�������ݿ�*/
	std::string hostip = "192.168.0.200";
	int port = 3306;
	std::string user = "root";
	std::string password = "123456";
	std::string database = "cmstest";
	std::string tablename = "ngcc_metadata";

	/*��ȡ�����ļ���json�ļ�����ȡip�Ͷ˿��Լ����ݿ�����*/
	//rapidjson::Document doc_cfg;
	//FILE *myFile = NULL;
	//fopen_s(&myFile, "./ngcc_metadata_config.json", "rb");
	//if (myFile)
	//{
	//	char buf_cfg[4096] = { 0 };
	//	rapidjson::FileReadStream inputStream(myFile, buf_cfg, sizeof(buf_cfg));  //����һ��������
	//	doc_cfg.ParseStream(inputStream); //����ȡ������ת��ΪdomԪ��
	//	fclose(myFile); //�ر��ļ�������Ҫ
	//}
	//else
	//{
	//	std::string rntStr = "{\"status\": -1, \"msg\": \"�Ҳ��������ļ�\"}";
	//	response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));
	//	return 400;
	//}
	//if (doc_cfg.HasParseError())
	//{
	//	std::string rntStr = "{\"status\": -1, \"msg\": \"�����ļ��е�json��ʽ����ȷ\"}";
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

	/*��ʼ��mysql*/
	MYSQL * m_mysql = mysql_init(NULL);  //�ڳ���ʹ��Mysql���ʼ�������mysql_int()��ʼ��
	if (m_mysql == NULL)
	{
		std::string rntStr = u8"{\"status\": 1, \"msg\": \"mysql init failed\"}";
		response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));
		return 200;
	}
	/*�������ݿ������*/
	int arg = 1;
	mysql_options(m_mysql, MYSQL_OPT_RECONNECT, &arg);

	if (mysql_real_connect(m_mysql, hostip.c_str(),
		user.c_str(), password.c_str(), database.c_str(), port, NULL, 0) == NULL)
	{
		std::string rntStr = u8"{\"status\": 2, \"msg\": \"mysql_real_connect failed\"}";
		response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));
		return 500;
	}


	/*��ʼ��������image���ݱ��������ݿ�*/
	//MYSQL * m_mysql_img = mysql_init(NULL);  //�ڳ���ʹ��Mysql���ʼ�������mysql_int()��ʼ��
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

	/*��ȡ�ֶ�ӳ���*/
	int out_size = 0;
	const us_cms_metdata_map *metamap = us_get_cms_metadata_map_table(&out_size);
	if (metamap == nullptr || out_size < 0)
	{
		std::string rntStr = u8"{\"status\": 3, \"msg\": \"��ȡ�ֶ�ӳ���ʧ��\"}";
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

	/*���Ԫ���ݴ洢��*/
	std::string sql_clear = "truncate table " + tablename;
	if (mysql_real_query(m_mysql, sql_clear.c_str(), sql_clear.size()) != 0)
	{
		std::string rntStr = u8"{\"status\": 4, \"msg\": \"���Ԫ���ݲ��¼��ʧ��\"}";
		response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));
		return 500;
	}

	/*��ȡimageԪ���ݱ��е�ȫ����¼���ؽ��û�Ԫ���ݵĴ洢��¼*/
	std::string sql_read = "select _id, url from image";
	if (mysql_real_query(m_mysql, sql_read.c_str(), sql_read.size()) != 0)
	{
		std::string rntStr = u8"{\"status\": 5, \"msg\": \"��ȡimage��ʧ��\"}";
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

	/*���صļ�¼��Ŀ�����С��1����imageԪ���ݱ�Ϊ�գ������ؽ�*/
	unsigned long long rows = mysql_num_rows(result);
	if (rows < 1)
	{
		std::string rntStr = u8"{\"status\": 7, \"msg\": \"image Ԫ���ݱ�Ϊ��\"}";
		response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));
		return 500;
	}
	MYSQL_ROW rown; //һ�����ݵ����Ͱ�ȫ��ʾ
	while ((rown = mysql_fetch_row(result)) != NULL)
	{
		std::string fd_ids_str;
		std::string out_json;
		std::string fd_url_str;
		unsigned long *row_len = mysql_fetch_lengths(result);

		/*��ȡimageԪ���ݱ��id��url�ֶΣ���ͨ��url�ֶε�ֵ���Ȱ�xml·��������ȡ��Ԫ���ݵ�json��ʽ*/
		for (int i = 0; i < mysql_num_fields(result); ++i)
		{
			MYSQL_FIELD *field = mysql_fetch_field_direct(result, i);  //��ȡ�ֶ�
			std::string fd_name = field->name;
			if (fd_name == "_id")
			{
				//��ȡ_id
				std::string buf_ids;
				buf_ids.resize(row_len[i] + 1);
				//����һ���Ϸ���SQL�ַ������Ա���SQL�����ʹ��
				int ret = mysql_real_escape_string(m_mysql,
					(char*)buf_ids.data(), rown[i], row_len[i]);
				fd_ids_str = std::string(buf_ids.data(), ret);
			}
			if (fd_name == "url")
			{
				//��ȡurl
				std::string buf_url;
				buf_url.resize(row_len[i] + 1);
				//����һ���Ϸ���SQL�ַ������Ա���SQL�����ʹ��
				int ret = mysql_real_escape_string(m_mysql,
					(char*)buf_url.data(), rown[i], row_len[i]);
				buf_url.resize(ret);
				fd_url_str = std::string(buf_url.data(), ret);
				// ��ȡʵ���ļ�·��
				unispace::us_ustring realpath = r_storage_mgr.get_image_real_path(buf_url);
				//����image�е�url��ȡԪ���ݼ�¼����json��ʽ�����out_json
				int ret_read = us_read_cms_metadata_record(realpath, &out_json);
				if (ret_read < 0)
				{
					std::string rntStr = u8"{\"status\": 8, \"msg\": \"�޷���url��ȡԪ����\"}";
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
		/*�������ݣ���id��url��status������Ԫ����������Ϊ0����Ԫ����ȫ�����룩*/
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
		sql_insert.append(") values('").append(fd_ids_str).append("', '").append(fd_url_str).append("', ").append("0, ");   //�ؽ�ʱstatus��Ϊ0
		
		

		rapidjson::Document doc1;
		doc1.Parse(out_json.c_str());
		if (doc1.HasParseError())
		{
			std::string rntStr = u8"{\"status\": 9, \"msg\": \"��ȡ��Ԫ����json��ʽ����\"}";
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
					/*ֻ������������*/
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
				std::string rntStr = u8"{\"status\": 10, \"msg\": \"��xml�н�������json����\"}";
				response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));
				return 500;
			}
		}

		sql_insert = sql_insert.substr(0, sql_insert.size() - 2);
		sql_insert.append(")");
		if (mysql_real_query(m_mysql, sql_insert.data(), sql_insert.size()) != 0)
		{
			std::string rntStr = u8"{\"status\": 11, \"msg\": \"����Ԫ���ݼ�¼ʧ��\"}";
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

	std::string rntStr = u8"{\"status\": 0, \"msg\": \"�ؽ��ɹ�\"}";
	response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));

	return 200;
}
#endif

#if 0
int us_cms_http_get_getm1(
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

	/*��ȡ����ʵ��*/
	std::string jsonstr;
	for (auto itr = request_body.begin(); itr != request_body.end(); ++itr)
	{
		jsonstr.push_back(*itr);
	}
	rapidjson::Document doc_getm;
	doc_getm.Parse(jsonstr.c_str());
	if (doc_getm.HasParseError())
	{
		std::string rntStr = u8"{\"status\": 4, \"msg\": \"����ʵ��json��ʽ����ȷ\"}";
		response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));
		return 400;
	}

	/*������ʵ���л�ȡid��url����*/
	/*std::set<std::string> set_ids;
	std::set<std::string> set_urls;*/
	std::vector<std::string> vect_ids;
	std::vector<std::string> vect_urls;
	//rapidjson::Value::ConstMemberIterator iter = doc_getm.FindMember("ids");  //����id
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
	rapidjson::Value::ConstMemberIterator iter = doc_getm.FindMember("urls");  //����urls
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
		//�޷�������ʵ���л�ȡid��url
		return 400;
	}



	/*�������ݿ�*/
	std::string hostip = "192.168.0.200";
	int port = 3306;
	std::string user = "root";
	std::string password = "123456";
	std::string database = "unispace";
	std::string tablename = "ngcc_metadata";

	/*��ȡ�����ļ���json�ļ�����ȡip�Ͷ˿��Լ����ݿ�����*/
	//rapidjson::Document doc_cfg;
	//FILE *myFile = NULL;
	//fopen_s(&myFile, "./ngcc_metadata_config.json", "rb");
	//if (myFile)
	//{
	//	char buf_cfg[4096] = { 0 };
	//	rapidjson::FileReadStream inputStream(myFile, buf_cfg, sizeof(buf_cfg));  //����һ��������
	//	doc_cfg.ParseStream(inputStream); //����ȡ������ת��ΪdomԪ��
	//	fclose(myFile); //�ر��ļ�������Ҫ
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

	MYSQL * m_mysql = mysql_init(NULL);  //�ڳ���ʹ��Mysql���ʼ�������mysql_int()��ʼ��
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

	/*��ȡ�ֶ�ӳ���*/
	int out_size = 0;
	const us_cms_metdata_map *metamap = us_get_cms_metadata_map_table(&out_size);
	if (metamap == nullptr || out_size < 0)
	{
		//��ȡ�ֶ�ӳ���ʧ��
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
	sql.reserve(size_sql); //ΪsqlԤ����洢�ռ�

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


	//Ϊid��urlԤ���仺����
	//std::string buf_ids;
	//buf_ids.reserve(set_ids.size() * 32); //һ��id����Ϊ16
	//const char *pbuf_ids = buf_ids.c_str();
	//std::string buf_urls;
	//buf_ids.reserve(set_urls.size() * 1534);  //һ��url����Ϊ767
	//const char *pbuf_urls = buf_urls.c_str();

	/*where�Ӿ�*/
	//if (!set_ids.empty() || !set_urls.empty())
	//{
		sql.append(" where ");
		if (!vect_ids.empty())
		{
			//for (auto itr = set_ids.begin(); itr != set_ids.end(); ++itr)
			//{
			//	if (itr->empty())
			//	{
			//		continue;  //�����ֵ������������ѯ
			//	}
			//	char buf_ids[32];
			//	int ret = mysql_real_escape_string(m_mysql, buf_ids, itr->c_str(), itr->size()); //����һ���Ϸ���SQL�ַ������Ա���SQL�����ʹ��
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
					continue; //�����ֵ������������ѯ
				}
				char buf_url[1534];
				int ret = mysql_real_escape_string(m_mysql, buf_url, itr->c_str(), itr->size()); //����һ���Ϸ���SQL�ַ������Ա���SQL�����ʹ��
				std::string fd_url_str = std::string(buf_url, ret);
				sql.append("`url` = '").append(fd_url_str).append("' or ");
			}
		}
		sql = sql.substr(0, sql.size() - 4);
	/*}*/

	/*ִ��sql��ѯ�������ز�ѯ���*/
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

	/*�����˶�����*/
	unsigned long long rows = mysql_num_rows(result);
	std::string rowstr = std::to_string(rows);
	if (rows < 1)
	{
		std::string rntStr = u8"{\"status\": 7,\"msg\": \"δ�ҵ���Ӧ�ļ�¼\"}";
		response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));
		return 404;
	}

	/*��ѯ���Ľ��*/
	rapidjson::StringBuffer strbuf;
	strbuf.Reserve(rows * 5120);
	rapidjson::Writer<rapidjson::StringBuffer> writer(strbuf);
	/*����ȡ���Ľ��д��json*/
	writer.StartObject();

	/*״ֵ̬Ϊ0*/
	writer.Key("status");
	writer.Int64(0);

	/*�ɹ�����Ŀ*/
	writer.Key("okn");
	writer.Int64(rows);

	MYSQL_ROW rown; //һ�����ݵ����Ͱ�ȫ��ʾ

	/*���سɹ���Ԫ���ݼ�¼*/
	writer.Key("metas");
	writer.StartArray();  //������ʽ

	/*std::vector<std::string> ok_ids;
	std::vector<std::string> ok_urls;*/

	int numrow = 0;
	while ((rown = mysql_fetch_row(result)) != NULL)
	{
		numrow++;

		writer.StartObject();
		
		unsigned long *row_lens = mysql_fetch_lengths(result); //��ȡÿһ�еĳ���
		for (int i = 0; i < mysql_num_fields(result); ++i)
		{
			//MYSQL_FIELD *field = mysql_fetch_field_direct(result, i);   //��ȡ�ֶ�
			std::string fd_s = std::string(rown[i], row_lens[i]);  //��ȡ�ֶ�ֵ
			
			std::string fd_name = metamaps[i].m_custon_field;  //�û��ɼ����ֶ���
			uint32_t fd_type = metamaps[i].m_type;  //�ֶ�����

			//std::string fd_db_name = field->name;  //���ݿ�����ֶ���
			//std::string fd_name = fd_db_name;  //�û��ɼ����ֶ���
			//uint32_t fd_type = 0;  //�ֶ�����
			//for (auto itr = metamaps.begin(); itr != metamaps.end(); ++itr)
			//{
			//	if (itr->m_db_field == atoi(fd_db_name.c_str()))
			//	{
			//		fd_name = itr->m_custon_field;  //��ȡ�û��ɼ����ֶ���
			//		fd_type = itr->m_type; //�ֶ�����
			//	}
			//}
			
			/*����ʧ�ܵ�_id��url*/
			//if (fd_name == "_id")
			//{
			//	std::set<std::string>::iterator itr = set_ids.find( fd_s); //����_id
			//	if (itr != set_ids.end()) //�ҵ�
			//	{
			//		set_ids.erase(fd_s);  //set_ids���ʧ�ܵ�id
			//	}
			//}
			if (i == 0) //url
			{
				std::vector<std::string>::iterator itr = std::find(vect_urls.begin(), vect_urls.end(), fd_s); //����url
				if (itr != vect_urls.end()) //�ҵ�
				{
					//set_urls.erase(fd_s);  //set_urls���ʧ�ܵ�url
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

			/*���������*/
			if (fd_name == "MaxX" || fd_name == "MinX" || fd_name == "MaxY" || fd_name == "MinY")
			{
				continue;
			}
			/*����Χ((x0 y0,x1 y1,x2 y2,x3 y3,x0 y0))���ϣ����ϣ�����������*/
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
				//range_s = range_s.substr(2, range_s.size() - 4);  //ȥ�����˵�����
				//												  /*����*/
				//size_t pos_spce_sw = range_s.find_first_of(",");
				//std::string s_south_west = range_s.substr(0, pos_spce_sw);  //��������
				//size_t pos_in = s_south_west.find_first_of(" ");
				//std::string SouthWestAbs = s_south_west.substr(0, pos_in);  //���Ϻ�����
				//std::string SouthWestOrd = s_south_west.substr(pos_in + 1, s_south_west.size() - pos_in - 1);  //����������
				//writer.Key("SouthWestAbs");
				//writer.Double(atof(SouthWestAbs.c_str()));
				//writer.Key("SouthWestOrd");
				//writer.Double(atof(SouthWestOrd.c_str()));
				///*rntstr = "\"SouthWestAbs\": " + SouthWestAbs + ", \"SouthWestOrd\": " + SouthWestOrd + ", ";*/

				///*����*/
				//size_t pos_spce_se = range_s.find_first_of(",", pos_spce_sw + 1);
				//std::string s_south_east = range_s.substr(pos_spce_sw + 1, pos_spce_se - pos_spce_sw - 1); //��������
				//pos_in = s_south_east.find_first_of(" ");
				//std::string SouthEastAbs = s_south_east.substr(0, pos_in);  //���Ϻ�����
				//std::string SouthEastOrd = s_south_east.substr(pos_in + 1, s_south_east.size() - pos_in - 1);  //����������
				//writer.Key("SouthEastAbs");
				//writer.Double(atof(SouthEastAbs.c_str()));
				//writer.Key("SouthEastOrd");
				//writer.Double(atof(SouthEastOrd.c_str()));
				///*rntstr.append("\"SouthEastAbs\": ").append(SouthEastAbs).append(", \"SouthEastOrd\": ").append(SouthEastOrd).append(", ");*/

				///*����*/
				//size_t pos_spce_ne = range_s.find_first_of(",", pos_spce_se + 1);
				//std::string s_north_east = range_s.substr(pos_spce_se + 1, pos_spce_ne - pos_spce_se - 1); //��������
				//pos_in = s_north_east.find_first_of(" ");
				//std::string NorthEastAbs = s_north_east.substr(0, pos_in);  //����������
				//std::string NorthEastOrd = s_north_east.substr(pos_in + 1, s_north_east.size() - pos_in - 1);  //����������
				//writer.Key("NorthEastAbs");
				//writer.Double(atof(NorthEastAbs.c_str()));
				//writer.Key("NorthEastOrd");
				//writer.Double(atof(NorthEastOrd.c_str()));
				///*rntstr.append("\"NorthEastAbs\": ").append(NorthEastAbs).append(", \"NorthEastOrd\": ").append(NorthEastOrd).append(", ");*/

				///*����*/
				//size_t pos_spce_nw = range_s.find_first_of(",", pos_spce_ne + 1);
				//std::string s_north_west = range_s.substr(pos_spce_ne + 1, pos_spce_nw - pos_spce_ne - 1); //��������
				//pos_in = s_north_west.find_first_of(" ");
				//std::string NorthWestAbs = s_north_west.substr(0, pos_in);  //����������
				//std::string NorthWestOrd = s_north_west.substr(pos_in + 1, s_north_west.size() - pos_in - 1);  //����������
				//writer.Key("NorthWestAbs");
				//writer.Double(atof(NorthWestAbs.c_str()));
				//writer.Key("NorthWestOrd");
				//writer.Double(atof(NorthWestOrd.c_str()));
				/*rntstr.append("\"NorthWestAbs\": ").append(NorthWestAbs).append(", \"NorthWestOrd\": ").append(NorthWestOrd);*/

				//if (i < mysql_num_fields(result) - 1)  //�������һ���ֶ�
				//{
				//	rntstr.append(", ");
				//}

				//response_body.insert(response_body.end(), (uint8_t*)rntstr.data(), (uint8_t*)(rntstr.data() + rntstr.size()));
			/*}*/

			/*������ֵ���ʹ洢���ֶ�*/
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
			/*��������ֵ���ʹ洢���ֶ�*/
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
				//if (i < mysql_num_fields(result) - 1)  //�������һ���ֶ�
				//{
				//	rntstr.append(", ");
				//}
				//response_body.insert(response_body.end(), (uint8_t*)rntstr.data(), (uint8_t*)(rntstr.data() + rntstr.size()));
			}

			/*���ַ������ʹ洢���ֶ�*/
			else
			{
				writer.Key(fd_name.c_str());
				writer.String(fd_s.data(), row_lens[i]);
				//rntstr = "\"" + fd_name + "\": \"" + fd_s + "\"";
				//if (i < mysql_num_fields(result) - 1)  //�������һ���ֶ�
				//{
				//	rntstr.append(", ");
				//}
				//response_body.insert(response_body.end(), (uint8_t*)rntstr.data(), (uint8_t*)(rntstr.data() + rntstr.size()));
			}
		}
		writer.EndObject();
		//if (numrow == rows) //���һ��
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

	/*��ѯʧ�ܵĲ���*/
	int failn = 0;
	/*��ѯʧ�ܵ�id*/

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
	////for (auto itr = vect_ids.begin(); itr != vect_ids.end(); ++itr) //vect_ids ���û�����ѯʱ�����id
	////{
	////	auto it = get_ids.begin();
	////	for (; it != get_ids.end(); ++it)   //get_ids�ǲ�ѯ����е�id
	////	{
	////		if (*itr == *it)
	////		{
	////			break;
	////		}
	////	}
	////	if (it == get_ids.end())
	////	{
	////		writer.String(itr->c_str());
	////		//rntstr.append("\"").append(*itr).append("\", ");   //��ѯʧ�ܵ�id
	////		failn++;
	////	}
	////}
	//writer.EndArray();

	//int failn_id = failn;
	//if (failn > 0)
	//{
	//	rntstr = rntstr.substr(0, rntstr.size() - 2); //ȥ��ĩβ��', '
	//}
	//rntstr.append("], ");
	//response_body.insert(response_body.end(), (uint8_t*)rntstr.data(), (uint8_t*)(rntstr.data() + rntstr.size()));

	/*��ѯʧ�ܵ�url*/
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
	//for (auto itr = vect_urls.begin(); itr != vect_urls.end(); ++itr)  //vect_urls�û�����ѯʱ�����url
	//{
	//	auto it = get_urls.begin();
	//	for (; it != get_urls.end(); ++it)  //get_urls�ǲ�ѯ����е�url
	//	{
	//		if (*itr == *it)
	//		{
	//			break;
	//		}
	//	}
	//	if (it == get_urls.end())
	//	{
	//		writer.String(itr->c_str());
	//		//rntstr.append("\"").append(*itr).append("\", ");  //��ѯʧ�ܵ�url
	//		failn++;
	//	}
	//}
	writer.EndArray();
	writer.EndObject();
	//if (failn > failn_id)
	//{
	//	rntstr = rntstr.substr(0, rntstr.size() - 2); //ȥ��ĩβ��', '
	//}
	//rntstr.append("]}, ");
	//response_body.insert(response_body.end(), (uint8_t*)rntstr.data(), (uint8_t*)(rntstr.data() + rntstr.size()));

	/*��ѯʧ�ܵ���Ŀ*/
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
#endif

#if 0
int us_cms_http_post_refresh22(
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

	/*�������ݿ�*/
	std::string hostip = "localhost";
	int port = 3306;
	std::string user = "root";
	std::string password = "";
	std::string database = "metadata2";
	std::string tablename = "ngcc_metadata";

	/*��ȡ�����ļ���json�ļ�����ȡip�Ͷ˿��Լ����ݿ�����*/
	//rapidjson::Document doc_cfg;
	//FILE *myFile = NULL;
	//fopen_s(&myFile, "./ngcc_metadata_config.json", "rb");
	//if (myFile)
	//{
	//	char buf_cfg[4096] = { 0 };
	//	rapidjson::FileReadStream inputStream(myFile, buf_cfg, sizeof(buf_cfg));  //����һ��������
	//	doc_cfg.ParseStream(inputStream); //����ȡ������ת��ΪdomԪ��
	//	fclose(myFile); //�ر��ļ�������Ҫ
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

	MYSQL * m_mysql = mysql_init(NULL);  //�ڳ���ʹ��Mysql���ʼ�������mysql_int()��ʼ��
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

	/*��ȡ�ֶ�ӳ���*/
	int out_size = 0;
	const us_cms_metdata_map *metamap = us_get_cms_metadata_map_table(&out_size);
	if (metamap == nullptr || out_size < 0)
	{
		std::string rntStr = u8"{\"status\": 3, \"msg\": \"��ȡ�ֶ�ӳ���ʧ��\"}";
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

	/*����statusΪ1��Ӧ��url*/
	std::string sql_search_status = "select _id, url from " + tablename + " where status = 1";
	if (mysql_real_query(m_mysql, sql_search_status.c_str(), sql_search_status.size()) != 0)
	{
		std::string rntStr = u8"{\"status\": 4, \"msg\": \"mysql_real_query failed\"}";
		response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));
		return 500;
	}

	/*��ȡurl����ȡ���ݲ���Ԫ���ݼ�¼��*/
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
		std::string rntStr = u8"{\"status\": 0, \"msg\": \"�����޸���\"}";
		response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));
		return 200;
	}
	MYSQL_ROW rown; //һ�����ݵ����Ͱ�ȫ��ʾ
	while ((rown = mysql_fetch_row(result)) != NULL) 
	{
		std::string url_s;
		std::string ids_s;
		unsigned long *row_len = mysql_fetch_lengths(result);  //��ǰ�������еĳ���
		for (int i = 0; i < mysql_num_fields(result); ++i)
		{
			MYSQL_FIELD *field = mysql_fetch_field_direct(result, i);  //��ȡ�ֶ�
			std::string fd_name = field->name;
			if (fd_name == "url")
			{
				url_s = std::string(rown[i], row_len[i]);   //��ȡurl
				std::string buf_urls;
				buf_urls.resize(url_s.size() + 1);
				int ret = mysql_real_escape_string(m_mysql, (char*)buf_urls.data(), url_s.c_str(), url_s.size());  //����һ���Ϸ���SQL�ַ������Ա���SQL�����ʹ��
				url_s = std::string(buf_urls.data(), ret);
			}
			if (fd_name == "_id")
			{
				ids_s = std::string(rown[i], row_len[i]);  //��ȡ_id
				std::string buf_ids;
				buf_ids.resize(ids_s.size() + 1);
				int ret = mysql_real_escape_string(m_mysql, (char*)buf_ids.data(), ids_s.c_str(), ids_s.size()); //����һ���Ϸ���SQL�ַ������Ա���SQL�����ʹ��
				ids_s = std::string(buf_ids.data(), ret);
			}
		}
		/*����Ԫ����*/
		std::string sql_update_insert;
		sql_update_insert.reserve(7168);
		sql_update_insert = "update " + tablename + " set status = 0, ";   //��������ʱ��status��Ϊ0
		for (int i_v = 1; i_v <= out_size; ++i_v)
		{
			std::string i_v_s = std::to_string(i_v);   //���ݿ��ֶ���
			std::string jsonstr;
			/*��ȡԪ���ݼ�¼*/
			int ret_js = us_read_cms_metadata_record(url_s, &jsonstr);
			rapidjson::Document doc1;
			doc1.Parse(jsonstr.c_str());
			if (doc1.HasParseError())
			{
				std::string rntStr = u8"{\"status\": 6, \"msg\": \"��url�л�ȡ��json��ʽ����ȷ\"}";
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
					/*ֻ������������*/
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

	std::string rntStr = u8"{\"status\": 0, \"msg\": \"���³ɹ�\"}";
	response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));

	return 200;
}
#endif



#if 0
/*ʹ�ö��̵߳ķ���ȥ�ؽ����ݼ�¼��*/
std::mutex m_mutex_img;
std::mutex m_mutex_ngcc;

/*��ȡimage������ngcc��¼���߳�*/
void read_and_insert(MYSQL *m_mysql, std::string tablename, int out_size, int from, int size, int &rnt)
{
	std::unique_lock<std::mutex> lok(m_mutex_img);

	/*��ѯimage��*/
	std::string sql_read = "select _id, url from image limit ";
	sql_read.append(std::to_string(size)).append(" offset ").append(std::to_string(from));
	if (mysql_real_query(m_mysql, sql_read.c_str(), sql_read.size()) != 0)
	{
		rnt = -1;  //��ȡʧ��
		return;
	}
	MYSQL_RES *result = mysql_store_result(m_mysql);
	if (result == NULL)
	{
		rnt = -2;  //��ȡ��ѯ���ʧ��
		return;
	}
	/*�����˶�����*/
	unsigned long long rows = mysql_num_rows(result);
	std::string rowstr = std::to_string(rows);
	if (rows < 1)
	{
		rnt = -3; //û���ҵ�
		return;
	}

	/*����ÿһ������*/
	MYSQL_ROW rown; //һ�����ݵ����Ͱ�ȫ��ʾ
	while ((rown = mysql_fetch_row(result)) != NULL)
	{
		std::string fd_ids_str;
		std::string out_json;
		std::string fd_url_str;
		unsigned long *row_len = mysql_fetch_lengths(result);

		/*��ȡimageԪ���ݱ��id��url�ֶΣ���ͨ��url�ֶε�ֵ���Ȱ�xml·��������ȡ��Ԫ���ݵ�json��ʽ*/
		for (int i = 0; i < mysql_num_fields(result); ++i)
		{
			MYSQL_FIELD *field = mysql_fetch_field_direct(result, i);  //��ȡ�ֶ�
			std::string fd_name = field->name;
			if (fd_name == "_id")
			{
				fd_ids_str = std::string(rown[i], row_len[i]);  //��ȡ_id
				std::string buf_ids;
				buf_ids.resize(fd_ids_str.size() + 1);
				int ret = mysql_real_escape_string(m_mysql, (char*)buf_ids.data(), fd_ids_str.c_str(), fd_ids_str.size());  //����һ���Ϸ���SQL�ַ������Ա���SQL�����ʹ��
				fd_ids_str = std::string(buf_ids.data(), ret);
				std::cout << "thread----" << fd_ids_str << "\t";
			}
			if (fd_name == "url")
			{
				fd_url_str = std::string(rown[i], row_len[i]);  //��ȡurl
				std::string buf_url;
				buf_url.resize(fd_url_str.size() + 1);
				int ret = mysql_real_escape_string(m_mysql, (char*)buf_url.data(), fd_url_str.c_str(), fd_url_str.size());  //����һ���Ϸ���SQL�ַ������Ա���SQL�����ʹ��
				fd_url_str = std::string(buf_url.data(), ret);
				int ret_read = us_read_cms_metadata_record(fd_url_str, &out_json);  //����image�е�url��ȡԪ���ݼ�¼����json��ʽ�����out_json
				if (ret_read < 0)
				{
					rnt = -4; //��url�л�ȡjsonʧ��
					return;
				}
			}
		}

		std::unique_lock<std::mutex> lok_ngcc(m_mutex_ngcc);

		/*�������ݣ���id��url��status������Ԫ����������Ϊ0����Ԫ����ȫ�����룩*/
		std::string sql_insert = "insert into " + tablename + "(`_id`, `url`, `status`, ";
		for (int i = 1; i < out_size + 1; i++)
		{
			std::string buf_i = std::to_string(i);
			sql_insert.append("`").append(buf_i).append("`, ");
		}
		sql_insert = sql_insert.substr(0, sql_insert.size() - 2);
		sql_insert.append(") values('").append(fd_ids_str).append("', '").append(fd_url_str).append("', ").append("0, ");   //�ؽ�ʱstatus��Ϊ0
		for (int i = 1; i < out_size + 1; i++)
		{
			std::string colname = std::to_string(i);

			rapidjson::Document doc1;
			doc1.Parse(out_json.c_str());
			if (doc1.HasParseError())
			{
				rnt = -5; //��ȡ��Ԫ����json��ʽ����
				return ;
			}
			std::string value_db = "";
			if (doc1.HasMember(colname.c_str()))
			{
				if (!doc1[colname.c_str()].IsNull())
				{	/*ֻ������������*/
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
				rnt = -6; //��xml�н�������json����
				return ;
			}
		}

		sql_insert = sql_insert.substr(0, sql_insert.size() - 2);
		sql_insert.append(")");
		if (mysql_real_query(m_mysql, sql_insert.data(), sql_insert.size()) != 0)
		{
			rnt = -7; //����Ԫ���ݼ�¼ʧ��
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

	/*�������ݿ�*/
	std::string hostip = "localhost";
	int port = 3306;
	std::string user = "root";
	std::string password = "";
	std::string database = "metadata2";
	std::string tablename = "ngcc_metadata";

	/*��ʼ��mysql*/
	MYSQL * m_mysql = mysql_init(NULL);  //�ڳ���ʹ��Mysql���ʼ�������mysql_int()��ʼ��
	if (m_mysql == NULL)
	{
		std::string rntStr = "{\"status\": 1, \"msg\": \"mysql init failed\"}";
		response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));
		return 200;
	}
	/*�������ݿ������*/
	int arg = 1;
	mysql_options(m_mysql, MYSQL_OPT_RECONNECT, &arg);

	if (mysql_real_connect(m_mysql, hostip.c_str(),
		user.c_str(), password.c_str(), database.c_str(), port, NULL, 0) == NULL)
	{
		std::string rntStr = "{\"status\": 2, \"msg\": \"mysql_real_connect failed\"}";
		response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));
		return 500;
	}

	/*��ȡ�ֶ�ӳ���*/
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
		std::string rntStr = "{\"status\": 3, \"msg\": \"��ȡ�ֶ�ӳ���ʧ��\"}";
		response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));
		return 200;
	}

	/*���Ԫ���ݴ洢��*/
	std::string sql_clear = "truncate table " + tablename;
	if (mysql_real_query(m_mysql, sql_clear.c_str(), sql_clear.size()) != 0)
	{
		std::string rntStr = "{\"status\": 4, \"msg\": \"���Ԫ���ݲ��¼��ʧ��\"}";
		response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));
		return 200;
	}

	/*��ȡimage���ݱ������뵽ngccԭ���ݼ�¼����*/
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

	std::string rntStr = "{\"status\": 0, \"msg\": \"�ؽ��ɹ�\"}";
	response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));

	return 200;
}


int create()
{
	/*�������ݿ�*/
	std::string hostip = "localhost";
	int port = 3306;
	std::string user = "root";
	std::string password = "";
	std::string database = "metadata2";
	std::string tablename = "metatb";

	/*�������ݿ�*/
	MYSQL * m_mysql = mysql_init(NULL);  //�ڳ���ʹ��Mysql���ʼ�������mysql_int()��ʼ��
	if (m_mysql == NULL)
	{
		return -1;
	}
	int arg = 1;
	mysql_options(m_mysql, MYSQL_OPT_RECONNECT, &arg);  //�Ͽ�����

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
	/*�������ݿ�*/
	std::string hostip = "localhost";
	int port = 3306;
	std::string user = "root";
	std::string password = "";
	std::string database = "metadata2";
	std::string tablename = "image";

	/*�������ݿ�*/
	MYSQL * m_mysql = mysql_init(NULL);  //�ڳ���ʹ��Mysql���ʼ�������mysql_int()��ʼ��
	if (m_mysql == NULL)
	{
		return -1;
	}
	int arg = 1;
	mysql_options(m_mysql, MYSQL_OPT_RECONNECT, &arg);  //�Ͽ�����

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

#endif

#if 0

/*��ѯ*/
int us_cms_http_get_search3(
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


	/*�������ݿ�*/
	std::string hostip = "localhost";
	int port = 3306;
	std::string user = "root";
	std::string password = "";
	std::string database = "metadata2";
	std::string tablename = "ngcc_metadata";
	/*std::string hostip = "192.168.0.200";
	int port = 3306;
	std::string user = "root";
	std::string password = "123456";
	std::string database = "unispace";
	std::string tablename = "ngcc_metadata";*/

	/*��ʼ��mysql*/
	MYSQL * m_mysql = mysql_init(NULL);  //�ڳ���ʹ��Mysql���ʼ�������mysql_int()��ʼ��
	if (m_mysql == NULL)
	{
		std::string rntStr = "{\"status\": 1, \"msg\": \"mysql init failed\"}";
		response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));
		return 200;
	}
	/*�������ݿ������*/
	int arg = 1;
	mysql_options(m_mysql, MYSQL_OPT_RECONNECT, &arg);

	if (mysql_real_connect(m_mysql, hostip.c_str(),
		user.c_str(), password.c_str(), database.c_str(), port, NULL, 0) == NULL)
	{
		std::string rntStr = "{\"status\": 2, \"msg\": \"mysql_real_connect failed\"}";
		response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));
		return 500;
	}

	/*��ȡ�ֶ�ӳ���*/
	int metaMapTableSize = 0;
	const us_cms_metdata_map *pMetaMapTable = us_get_cms_metadata_map_table(&metaMapTableSize);
	if (pMetaMapTable == nullptr || metaMapTableSize < 0) {
		/*bytes_append(response_body,
			LITERAL_STRING_U8(u8"{\"status\":2,\"msg\":\"��ȡ�ֶ�ӳ���ʧ��\"}"));*/
		return 200;
	}

	std::vector<us_cms_metdata_map> metaMapVector;
	metaMapVector.reserve(metaMapTableSize);
	for (int i = 0; i < metaMapTableSize; ++i) {
		metaMapVector.push_back(*pMetaMapTable);
		pMetaMapTable++;
	}

	/*ΪsqlԤ����洢�ռ�*/
	std::string sql;
	sql.reserve(65535);

	/*sql��䣬��ȡ����������url�ֶ�*/
	sql.append("select `url`");
	sql.append(" from ").append(tablename).append(" where ");

	/*����request_body�е�json�ַ���*/
	std::string jsonstr;
	for (auto itr = request_body.begin(); itr != request_body.end(); ++itr) {
		jsonstr.push_back(*itr);
	}
	rapidjson::Document doc1;
	doc1.Parse(jsonstr.c_str());
	if (doc1.HasParseError())
	{
		/*bytes_append(response_body,
		LITERAL_STRING_U8(u8"{\"status\":3,\"msg\":\"����ʵ��json��ʽ����ȷ\"}"));*/
		return 400;
	}
	rapidjson::Value::ConstMemberIterator iter = doc1.FindMember("query");
	if (iter == doc1.MemberEnd() || !iter->value.IsObject()) {
		/*bytes_append(response_body,
			LITERAL_STRING_U8(u8"{\"status\":3,\"msg\":\"����ʵ��json��ʽ����ȷ\"}"));*/
		return 400;
	}

	/*query����*/
	auto queryobj = iter->value.GetObject();
	/*match��������ȷ��ѯ���ַ����������ѯ��*/
	if (queryobj.HasMember("match")) {
		if (queryobj["match"].IsArray()) {
			for (int i = 0; i < queryobj["match"].Size(); ++i) {
				/*��ȡ�ֶ���*/
				std::string fieldname;
				if (queryobj["match"][i].HasMember("field")) {
					if (queryobj["match"][i]["field"].IsString()) {
						fieldname = queryobj["match"][i]["field"].GetString();
					}
				}
				/*��ȡ��ѯģʽ*/
				int mode = 0;
				if (queryobj["match"][i].HasMember("mode")) {
					if (queryobj["match"][i]["mode"].IsInt64()) {
						mode = queryobj["match"][i]["mode"].GetInt64();
					}
				}
				/*��ȡƥ�䴮*/
				std::string pattern;
				if (queryobj["match"][i].HasMember("pattern")) {
					if (queryobj["match"][i]["pattern"].IsString()) //�ַ�������
					{
						pattern = queryobj["match"][i]["pattern"].GetString();
					}
					if (queryobj["match"][i]["pattern"].IsInt64())  //��������
					{
						int pattern_int = queryobj["match"][i]["pattern"].GetInt64();
						pattern = std::to_string(pattern_int);
					}
					if (queryobj["match"][i]["pattern"].IsDouble())  //����������
					{
						double pattern_db = queryobj["match"][i]["pattern"].GetDouble();
						pattern = std::to_string(pattern_db);
					}
				}
				/*���pattern��field��mode�ĺϷ���*/
				std::string dbfield;
				bool mode_flg = 0;
				bool find_field_flg = 0;
				auto itr = metaMapVector.begin();
				for (; itr != metaMapVector.end(); ++itr) {
					/*�����û��ɼ����ֶ�����ȡ���ݿ��ж�Ӧ���ֶ����� ���ж����Ӧ�Ĳ�ѯģʽ*/
					if (itr->m_custon_field == fieldname) {
						find_field_flg = 1;
						dbfield = std::to_string(itr->m_db_field);
						if ((itr->m_mode & mode) >= 1) {
							mode_flg = 1;
							break;
						}
					}
				}
				/*û���ҵ��ֶ����������ǲ�ѯģʽ����ȷ*/
				if (itr == metaMapVector.end()) {
					/*��ѯģʽ����ȷ*/
					if (find_field_flg == 1 && mode_flg == 0) {
						/*std::string rntStr = "{\"status\":4,\"msg\":\"" + fieldname + u8"��ѯģʽ����ȷ\"}";
						bytes_append(response_body, rntStr);*/
						return 400;
					}
					/*δ�ҵ��ֶ�*/
					else if (find_field_flg == 0) {
						/*std::string rntStr = u8"{\"status\":5,\"msg\":\"δ�ҵ�'" + fieldname + u8"'�ֶ�\"}";
						bytes_append(response_body, rntStr);*/
						return 400;
					}
				}
				if (pattern.empty()) {
					continue; //���ƥ�䴮Ϊ�գ��������ֶβ�����ѯ
				}
				/*���ݲ�ͬ��ѯģʽ��дsql���*/
				if (mode == 0x01)	//��������ʽ��ѯ
				{
					pattern.push_back(' ');
					size_t posspc;
					size_t posbeg = 0;
					/*�����ѯ��ƥ�䴮�Կո�ָ����д��������ʽ*/
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
					regexp = regexp.substr(0, regexp.size() - 1); //ȥ����β�� |
					regexp.append("].*$");
					sql.append("(`").append(dbfield).append("` regexp '").append(regexp).append("')");
				}
				else if (mode == 0x02)	//�ַ�����ȷ��ѯ
				{
					sql.append("(`").append(dbfield).append("` = '").append(pattern).append("')");
				}
				else if (mode == 0x04)	//���ڵľ�ȷ��ѯ
				{
					sql.append("(`").append(dbfield).append("` = ").append(pattern).append(")");
				}
				else if (mode == 0x10)	//��ֵ�ľ�ȷ��ѯ
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

	  /*range���� ����ֵ�������ڵķ�Χ��ѯ��*/
	if (queryobj.HasMember("range")) {
		if (queryobj["range"].IsArray()) {
			for (int i = 0; i < queryobj["range"].Size(); ++i) {
				/*�ֶ���*/
				std::string fieldname;
				if (queryobj["range"][i].HasMember("field")) {
					if (queryobj["range"][i]["field"].IsString()) {
						fieldname = queryobj["range"][i]["field"].GetString();
					}
				}
				/*��Сֵ*/
				std::string min_s;
				if (queryobj["range"][i].HasMember("min")) {
					if (queryobj["range"][i]["min"].IsInt64())   //��������
					{
						min_s = std::to_string(queryobj["range"][i]["min"].GetInt64());
					}
					if (queryobj["range"][i]["min"].IsDouble())  //����������
					{
						min_s = std::to_string(queryobj["range"][i]["min"].GetDouble());
					}
				}
				/*���ֵ*/
				std::string max_s;
				if (queryobj["range"][i].HasMember("max")) {
					if (queryobj["range"][i]["max"].IsInt64())  //��������
					{
						max_s = std::to_string(queryobj["range"][i]["max"].GetInt64());
					}
					if (queryobj["range"][i]["max"].IsDouble())  //����������
					{
						max_s = std::to_string(queryobj["range"][i]["max"].GetDouble());
					}
				}
				/*���field��mode�ĺϷ���*/
				std::string dbfield;
				bool mode_flg = 0;  //��ѯģʽ ��ȷΪ1
				bool find_field_flg = 0;  //�ֶ��Ƿ���ȷ����ȷΪ1
				auto itr = metaMapVector.begin();
				for (; itr != metaMapVector.end(); ++itr) {
					if (itr->m_custon_field == fieldname) {
						find_field_flg = 1;
						dbfield = std::to_string(itr->m_db_field); //��ȡ���ֶζ�Ӧ�����ݿ��ֶ�
						if ((itr->m_mode & 0x08) >= 1 || (itr->m_mode & 0x20) >= 1)   //0x08 �����ڵķ�Χ���� 0x20����ֵ�ķ�Χ����
						{
							mode_flg = 1;
							break;
						}
					}
				}
				if (itr == metaMapVector.end()) {
					/*��ѯģʽ����ȷ*/
					if (find_field_flg == 1 && mode_flg == 0) {
						/*std::string rntStr = "{\"status\":6,\"msg\":\"" + fieldname + u8"�޷����з�Χ��ѯ\"}";
						bytes_append(response_body, rntStr);*/
						return 400;
					}
					/*δ�ҵ��ֶ�*/
					else if (find_field_flg == 0) {
						/*std::string rntStr = u8"{\"status\":7,\"msg\":\"δ�ҵ�'" + fieldname + u8"'�ֶ�\"}";
						bytes_append(response_body, rntStr);*/
						return 400;
					}
				}
				/*ֻ�����ֵ�����*/
				if (min_s.empty() && !max_s.empty()) {
					sql.append("(`").append(dbfield).append("` <= ").append(max_s).append(")");
					sql.append(" and ");
				}
				/*ֻ����Сֵ�����*/
				else if (!min_s.empty() && max_s.empty()) {
					sql.append("(`").append(dbfield).append("` >= ").append(min_s).append(")");
					sql.append(" and ");
				}
				/*�����Сֵ������*/
				else if (!min_s.empty() && !max_s.empty()) {
					sql.append("(`").append(dbfield).append("` between ").append(min_s).append(" and ").append(max_s).append(")");
					sql.append(" and ");
				}
			}
		}
	} // end if (queryobj.HasMember("range"))

	  /*geometry���� ������Χ�Ĳ�ѯ[x,y,x,y,x,y...]��*/
	  //ʹ�õ���������geometry����
	iter = queryobj.FindMember("geometry");
	if (iter != queryobj.MemberEnd() && iter->value.IsArray()) {
		auto polyArray = iter->value.GetArray();
		std::vector<double> boxArray;
		int size_ = polyArray.Size();
		boxArray.reserve(polyArray.Size() * 4);
		// �����ȡpolygon
		for (size_t i = 0; i < polyArray.Size(); ++i) {
			// "POLYGON((x1 y1,x2 y2,��,xn yn)��,(x1 y1,x2 y2,��,xm ym),......)"
			const auto& polyStr = polyArray[i];
			if (!polyStr.IsString()) { continue; }
			// ����һ
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
			// ������
		}

		/*��ȡ���ݿ��е��ֶ���*/
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

			/*��дsql��ѯ���*/
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


			/*�ٿ�����һ�����������ѯ��ѡ������Χ�����ݿ���ĳ��Ӱ��ĵ���Χ��*/
			/*sql.append(" or ");

			sql.append("(`").append(dbfiled_min_x).append("` <= ").append(x_min_s).append(" and `").append(dbfiled_max_x).append("` >= ").append(x_max_s);
			sql.append(" and `").append(dbfiled_min_y).append("` <= ").append(y_min_s).append(" and `").append(dbfiled_max_y).append("` >= ").append(y_max_s).append("))");*/

			sql.append(" and ");
		}
	}

	/*��sql����β���ִ���*/
	size_t posand = sql.rfind("and");
	if (posand == sql.size() - 4) {
		sql = sql.substr(0, sql.size() - 5); //ȥ����β��and
	}
	else {
		sql = sql.substr(0, sql.size() - 6); //ȥ����β��where
	}

	/*size��������ȡ������¼,Ĭ��100*/
	int size = 100;
	if (doc1.HasMember("size")) {
		if (doc1["size"].IsInt64()) {
			size = doc1["size"].GetInt64();
		}
	}
	std::string s_size = std::to_string(size);
	s_size = " limit " + s_size;
	sql.append(s_size);

	/*from�������ӵڼ�����¼��ʼ��ȡ*/
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

	/*ִ��sql��ѯ�������ز�ѯ���*/
	if (mysql_real_query(m_mysql, sql.c_str(), sql.size()) != 0) {
		printf("mysql_real_query failed: error %u ( %s )",
			mysql_errno(m_mysql), mysql_error(m_mysql));
		return 200;
	}
	MYSQL_RES *result = mysql_store_result(m_mysql);
	if (result == NULL) {
		/*bytes_append(response_body,
			LITERAL_STRING_U8(u8"{\"status\":10,\"msg\":\"��ȡsql��ѯ���ʧ��\"}"));*/
		return 200;
	}

	/*�����˶�����*/
	unsigned long long rows = mysql_num_rows(result);

	std::string rowstr = std::to_string(rows);
	if (rows < 1) {
		/*bytes_append(response_body,
			LITERAL_STRING_U8(u8"{\"status\":11,\"msg\":\"δ�ҵ���Ӧ�ļ�¼\"}"));*/
		return 404;
	}

	rapidjson::StringBuffer strbuf;
	strbuf.Reserve(rows * 2000);
	rapidjson::Writer<rapidjson::StringBuffer> writer(strbuf);
	/*����ȡ���Ľ��д��json*/
	writer.StartObject();
	writer.Key("urls");
	writer.StartArray();

	MYSQL_ROW rown; //һ�����ݵ����Ͱ�ȫ��ʾ
	int numrow = 0;
	while ((rown = mysql_fetch_row(result)) != NULL) {
		numrow++;
		unsigned long * row_len = mysql_fetch_lengths(result);
		for (int i = 0; i < mysql_num_fields(result); ++i)  //ֻ��һ���ֶ�url
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
#endif

#if 0
int test_mysql_stmt()
{
	std::string hostip = "localhost";
	int port = 3306;
	std::string user = "root";
	std::string password = "";
	std::string database = "mytest_db";
	std::string tablename = "tutorials_tb1";

	/*��ʼ��mysql*/
	MYSQL* pMysql = mysql_init(NULL);  //�ڳ���ʹ��Mysql���ʼ�������mysql_int()��ʼ��
	if (pMysql == NULL)
	{
		std::string rntStr = "{\"status\": 1, \"msg\": \"mysql init failed\"}";
		std::cout << rntStr << std::endl;
		return -1;
	}

	//�������ݿ������
	int arg = 1;
	mysql_options(pMysql, MYSQL_OPT_RECONNECT, &arg);

	if (mysql_real_connect(pMysql, hostip.c_str(),
		user.c_str(), password.c_str(), database.c_str(), port, NULL, 0) == NULL)
	{
		std::string rntStr = "{\"status\": 2, \"msg\": \"mysql_real_connect failed\"}";
		std::cout << rntStr << std::endl;
		return -1;
	}

	/*
	*���ڶ��ִ�е���䣬Ԥ����ִ�б�ֱ��ִ�п죬
	*��Ҫԭ���� �������Բ�ѯִ��һ�ν���������
	*ÿ��ִ��Ԥ��������ǣ����跢�Ͳ��������ݣ��Ӷ���������ͨ����
	**/
	MYSQL_STMT* pStmtMysql = mysql_stmt_init(pMysql);  //��ʼ��Ԥ������ ����һ��Ԥ������
	if (!pStmtMysql)
	{
		std::cout << " mysql_stmt_init(), out of memory\n" << std::endl;
		return -1;
	}

	/**
	* ���ʺ��ַ�Ƕ�뵽�ʵ�λ�õ�sql�ַ�����
	* ��Ǿ���sql�����ĳЩλ�úϷ����磺vlues()�б���ָ���е���ֵ������where�Ӿ���н��бȽ���ָ���ıȽ�ֵ
	**/
	std::string sqlInsert = "insert into tutorials_tb1(title, author) values(?,?)";
	if (mysql_stmt_prepare(pStmtMysql, sqlInsert.c_str(), sqlInsert.size()))
	{
		std::cout << "mysql_stmt_prepare failed, " << mysql_stmt_error(pStmtMysql) << std::endl;
		return -1;
	}

	//����Ԥ׼������д��ڵĲ����������
	int paraCount = mysql_stmt_param_count(pStmtMysql);
	if (paraCount != 2)
	{
		std::cout << "mysql_stmt_param_count failed, " << mysql_stmt_error(pStmtMysql) << std::endl;
		return -1;
	}

	unsigned long strLength1 = 0;
	unsigned long strLength2 = 0;

	std::string title;

	//�˽ṹ����������루���͵�������������ֵ����������ӷ��������صĽ��ֵ��
	MYSQL_BIND bind[3];
	memset(bind, 0, sizeof(bind));
	bind[0].buffer_type = MYSQL_TYPE_STRING;
	bind[0].buffer = (char*)title.data();
	bind[0].buffer_length = BUFMAXSIZE;
	bind[0].is_null = (bool*)0;
	bind[0].length = &strLength1;

	std::string author;
	bind[1].buffer_type = MYSQL_TYPE_STRING;
	bind[1].buffer = (char*)author.data();
	bind[1].buffer_length = BUFMAXSIZE;
	bind[1].is_null = (bool*)0;
	bind[1].length = &strLength2;

	/*std::string date;
	bind[2].buffer_type = MYSQL_TYPE_DATE;
	bind[2].buffer = (char*)date.data();
	bind[2].buffer_length = BUFMAXSIZE;
	bind[2].is_null = (bool*)0;
	bind[2].length = 0; */ /*�������ֺ�ʱ������ length���ᱻ���ԣ���Ϊbuffer_typeֵȷ������ֵ�ĳ���*/

	//�Ѱ󶨱������õ� Ԥ��������
	if (mysql_stmt_bind_param(pStmtMysql, bind))
	{
		std::cout << "mysql_stmt_bind_param failed, " << mysql_stmt_error(pStmtMysql) << std::endl;
		return -1;
	}

	//�����һ����¼
	title = "C/C++";
	strLength1 = title.size();
	author = "uuu";
	strLength2 = author.size();
	//date = "2018-01-02";
	if (mysql_stmt_execute(pStmtMysql))
	{
		std::cout << "mysql_stmt_execute, failed, " << mysql_stmt_error(pStmtMysql) << std::endl;
		return -1;
	}


	std::string sqlSelect = "select id, title, author  from tutorials_tb1 where id = ?";
	if (mysql_stmt_prepare(pStmtMysql, sqlSelect.data(), sqlSelect.size()))
	{
		std::cout << "mysql_stmt_prepare failed, " << mysql_stmt_error(pStmtMysql) << std::endl;
		return -1;
	}
	int selectParamCount = mysql_stmt_param_count(pStmtMysql); /*�鿴�м���Ԥ�������*/
	if (selectParamCount != 1)
	{
		std::cout << "mysql_stmt_param_count failed, " << mysql_stmt_error(pStmtMysql) << std::endl;
		return -1;
	}

	int id;
	MYSQL_BIND bind1;
	memset(&bind1, 0, sizeof(bind1));
	bind1.buffer_type = MYSQL_TYPE_LONG;
	bind1.buffer_length = BUFMAXSIZE;
	bind1.buffer = &id;
	bind1.is_null = (bool *)0;
	
	id = 1;
	if (mysql_stmt_bind_param(pStmtMysql, &bind1))
	{
		std::cout << "mysql_stmt_bind_param failed, " << mysql_stmt_error(pStmtMysql) << std::endl;
		return -1;
	}

	id = 1;
	if (mysql_stmt_execute(pStmtMysql))
	{
		std::cout << "mysql_stmt_execute failed, " << mysql_stmt_error(pStmtMysql) << std::endl;
		return -1;
	}

	//�������������й��������ݻ������ͳ��Ȼ�����
	int resultId;
	std::string resultTitle, resultAuthor;
	int resultDate;



	MYSQL_BIND resultBind[3];
	memset(resultBind, 0, sizeof(resultBind));
	resultBind[0].buffer = &resultId;
	resultBind[0].is_null = (bool*)0;
	resultBind[0].buffer_type = MYSQL_TYPE_LONG;

	resultBind[1].buffer = (char*)resultTitle.data();
	resultBind[1].is_null = (bool*)0;
	resultBind[1].buffer_type = MYSQL_TYPE_STRING;

	resultBind[2].buffer = (char*)resultAuthor.data();
	resultBind[2].is_null = (bool*)0;
	resultBind[2].buffer_type = MYSQL_TYPE_STRING;

	if (mysql_stmt_bind_result(pStmtMysql, resultBind))
	{
		std::cout << "mysql_stmt_bind_result failed, " << mysql_stmt_error(pStmtMysql) << std::endl;
		return -1;
	}

	unsigned long rows = mysql_stmt_affected_rows(pStmtMysql);

/*
	mysql_stmt_store_result(pStmtMysql);
	while (!mysql_stmt_fetch(pStmtMysql))
	{
		if (!resultBind[0])
	}
*/

	/*mysql_stmt_fetch(pStmtMysql)*/


	return 0;

	/*���ڰ󶨴��ݸ�sql����еĲ����ݱ�ǵ��������ݣ�
	* ��ʹ��MYSQL_BIND���ṩ����
	* ����������ֵ�󶨵����������Թ�ʹ�� mysql_stmt_execute()
	*/
	//mysql_stmt_bind_param(pStmtMysql, bind);

}


pre_statement::pre_statement()
{
	m_stmt = NULL;
	m_param_bind = NULL;
	m_result_bind = NULL;
	m_result = NULL;
	m_param_count = 0;
	m_result_count = 0;
}

pre_statement::~pre_statement()
{
	free();
}

int pre_statement::init(MYSQL* mysql, std::string& sql)
{
	//��ʼ��Ԥ������
	m_stmt = mysql_stmt_init(mysql);
	if (!m_stmt)
	{
		return 1;
	}

	//��sql������
	if (mysql_stmt_prepare(m_stmt, sql.c_str(), sql.size()))
	{
		return 1;
	}

	// ?��Ԥ��������� �ĸ���
	m_param_count = mysql_stmt_param_count(m_stmt);
	if (m_param_count > 0)
	{
		m_param_bind = new MYSQL_BIND[m_param_count];
		if (!m_param_bind)
		{
			return 1;
		}
		memset(m_param_bind, 0, sizeof(MYSQL_BIND)*m_param_count);
	}
	return 0;
}

//����Ԥ����ֵ��int���ͣ�
int pre_statement::set_param_bind(int index, int& value)
{
	if (index >= m_param_count)
	{
		perror("��������������\n");
		return 1;
	}
	m_param_bind[index].buffer_type = MYSQL_TYPE_LONG;
	m_param_bind[index].buffer = &value;
	return 0;
}

//����Ԥ����ֵ��string���ͣ�
int pre_statement::set_param_bind(int index, std::string& value)
{
	if (index >= m_param_count)
	{
		perror("��������������\n");
		return 1;
	}
	m_param_bind[index].buffer_type = MYSQL_TYPE_STRING;
	m_param_bind[index].buffer = (void*)(value.c_str());
	m_param_bind[index].buffer_length = value.size();
	return 0;
}

//����Ԥ����ֵ��ʱ�����ͣ�
int pre_statement::set_param_bind(int index, MYSQL_TIME& value)
{
	if (index >= m_param_count)
	{
		perror("��������������\n");
		return 1;
	}
	m_param_bind[index].buffer_type = MYSQL_TYPE_TIMESTAMP;
	m_param_bind[index].buffer = &value;
	m_param_bind[index].buffer_length = sizeof(value);
	return 0;
}

//�������
int pre_statement::set_param_result(int index, enum_field_types type, char* buffer, unsigned long buffer_length, unsigned long	*length)
{
	if (index >= m_result_count)
	{
		perror("��������������\n");
		return 1;
	}
	m_result_bind[index].buffer_type = type;
	m_result_bind[index].buffer = buffer;
	m_result_bind[index].buffer_length = buffer_length;
	m_result_bind[index].length = length;
	return 0;
}

//ִ��sql���(��������ɾ�ĵ�sql���)
int pre_statement::execute()
{
	if (!m_stmt) /*�ж�Ԥ������*/
	{
		return 1;
	}
	if (mysql_stmt_bind_param(m_stmt, m_param_bind)) /*��Ԥ�������*/
	{
		//mysql_stmt_errno(m_stmt);
		return 1;
	}
	if (mysql_stmt_execute(m_stmt)) /*���󶨵�Ԥ�������ֵ���͵���������������ʹ�����ṩ�������滻���*/
	{
		//mysql_stmt_errno(m_stmt);
		return 1;
	} 
	if (0 == mysql_stmt_affected_rows(m_stmt))  /*��ɾ�� �����ı������*/
	{
		perror("����ʧ�� û�иı䣡\n");
		return 1;
	}
	return 0;
}

/*���ڷ��ؽ������sql���*/
int pre_statement::query()
{
	if (!m_stmt)
	{
		return 1;
	}
	if (mysql_stmt_bind_param(m_stmt, m_param_bind)) /*��Ԥ�������*/
	{
		//mysql_stmt_errno(m_stmt);
		return 1;
	}
	m_result = mysql_stmt_result_metadata(m_stmt); /*���ڷ��ؽ������sql��䣬���ؽ����*/
	if (NULL == m_result)
	{
		//mysql_stmt_errno(m_stmt);
		return 1;
	}
	else
	{
		m_result_count = mysql_num_fields(m_result);  /*���ؽ�����е�����*/
		m_result_bind = new MYSQL_BIND[m_result_count];  /*����bind�洢�ռ�*/
		if (!m_result_bind)
		{
			return 1;
		}
		memset(m_result_bind, 0, sizeof(MYSQL_BIND)*m_result_count);
	}
	if (mysql_stmt_execute(m_stmt))  /*���ɽ����*/
	{
		//mysql_stmt_errno(m_stmt);
		return 1;
	}
	return 0;
}

int pre_statement::get_result()
{
	// mysql_stmt_fetch() ʹ��mysql_stmt_bind_result�󶨵Ļ����� ����������
	if (mysql_stmt_bind_result(m_stmt, m_result_bind))
	{
		return 1;
	}

	// Ĭ������£��ӷ�������ȡ��ÿһ�����ݲ���ÿ�ζ����л���
	// ���Ե���mysql_stmt_store_result�����еĽ�������ݻ����ڿͻ���
	if (mysql_stmt_store_result(m_stmt))  
	{
		return 1;
	}
	return 0;
}

int pre_statement::fetch_result()
{
	//�ڵ���֮ǰ�����е������ݻ���������Ҫ���� Ҳ����˵���ȵ���mysql_stmt_bind_result
	if (!mysql_stmt_fetch(m_stmt))  /*��ȡ������е���һ��*/
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

void pre_statement::free()
{
	if (m_stmt)
	{
		mysql_stmt_close(m_stmt);
		m_stmt = NULL;
	}
	if (m_param_bind)
	{
		delete[]m_param_bind;
		m_param_bind = NULL;
	}
	if (m_result_bind)
	{
		delete[]m_result_bind;
		m_result_bind = NULL;
	}
	if (NULL != m_result)
	{
		mysql_free_result(m_result);
		m_result = NULL;
	}
	m_param_count = 0;
	m_result_count = 0;
}


#endif


#if 0
/*new*/

/// ********************************************************************************
/// <summary>
/// �����û�Ԫ����
/// </summary>
/// <param name="path">HTTP GET����·��</param>
/// <param name="headers">HTTP�����ͷ</param>
/// <param name="query_parameters">URL�еĲ�ѯ����</param>
/// <param name="path_parameters">URL�е�·������</param>
/// <param name="request_body">�����Content-Body����</param>
/// <param name="dest_endpoint">�ն˵�ַ</param>
/// <param name="response_headers">��Ӧ��HTTPͷ</param>
/// <param name="response_body">��Ӧ��HTTP Content����</param>
/// <returns>HTTP��Ӧ״̬��</returns>
/// <created>solym@sohu.com,2018/8/29</created>
/// ********************************************************************************
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

	/*����request_body�е�json�ַ���*/
	rapidjson::Document jsondoc;
	jsondoc.Parse((char*)request_body.data(), request_body.size());
	if (jsondoc.HasParseError() || !jsondoc.IsObject()) {
		bytes_append(response_body,
			LITERAL_STRING_U8(u8"{\"status\":3,\"msg\":\"json��ʽ����ȷ\"}"));
		return 400;
	}
	rapidjson::Value::ConstMemberIterator queryiter = jsondoc.FindMember("query");
	if (queryiter == jsondoc.MemberEnd() || !queryiter->value.IsObject()) {
		bytes_append(response_body,
			LITERAL_STRING_U8(u8"{\"status\":3,\"msg\":\"json��ʽ����ȷ\"}"));
		return 400;
	}

	/*�������ݿ�*/
	std::string hostip = "localhost";
	int port = 3306;
	std::string user = "root";
	std::string password = "";
	std::string database = "metadata2";
	std::string tablename = "ngcc_metadata";

	/*��ʼ��mysql*/
	MYSQL * pMysql = mysql_init(NULL);  /*�ڳ���ʹ��Mysql���ʼ�������mysql_int()��ʼ��*/
	if (pMysql == NULL)
	{
		std::string rntStr = "{\"status\": 1, \"msg\": \"mysql init failed\"}";
		response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));
		return 200;
	}
	/*�������ݿ������*/
	int arg = 1;
	mysql_options(pMysql, MYSQL_OPT_RECONNECT, &arg);

	if (mysql_real_connect(pMysql, hostip.c_str(),
		user.c_str(), password.c_str(), database.c_str(), port, NULL, 0) == NULL)
	{
		std::string rntStr = "{\"status\": 2, \"msg\": \"mysql_real_connect failed\"}";
		response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));
		return 500;
	}

	/*��ȡ�ֶ�ӳ���*/
	int metaMapTableSize = 0;
	const us_cms_metdata_map* pMetaMapTable = us_get_cms_metadata_map_table(&metaMapTableSize);
	if (pMetaMapTable == nullptr || metaMapTableSize < 0) {
		bytes_append(response_body,
			LITERAL_STRING_U8(u8"{\"status\":2,\"msg\":\"��ȡ�ֶ�ӳ���ʧ��\"}"));
		return 200;
	}

	/*ΪsqlԤ����洢�ռ�*/
	std::string sql;
	sql.reserve(4096 * 4);

	/*sql��䣬��ȡ����������url�ֶ�*/
	sql.append("select `url`");
	sql.append(" from ").append(tablename).append(" where ");


	/*query����*/
	const auto& queryobj = queryiter->value.GetObject();
	/*match��������ȷ��ѯ���ַ����������ѯ��*/
	rapidjson::Document::ConstMemberIterator iter = queryobj.FindMember("match");
	if (iter != queryobj.MemberEnd() && iter->value.IsArray()) {
		const auto& matchArray = iter->value.GetArray();
		for (int i = 0; i < matchArray.Size(); ++i) {
			const rapidjson::Document::ValueType& fieldInfo = matchArray[i];
			/*��ȡ�ֶ���*/
			rapidjson::Document::ConstMemberIterator fielditer = fieldInfo.FindMember("field");
			std::string fieldname;
			if (fielditer == fieldInfo.MemberEnd() || !fielditer->value.IsString()) { continue; }
			fieldname.assign(fielditer->value.GetString(), fielditer->value.GetStringLength());
			/*��ȡ��ѯģʽ*/
			int mode = 0;
			fielditer = fieldInfo.FindMember("mode");
			if (fielditer == fieldInfo.MemberEnd() || !fielditer->value.IsUint()) { continue; }
			mode = fielditer->value.GetUint();

			/*��ȡƥ�䴮*/
			std::string pattern;
			fielditer = fieldInfo.FindMember("pattern");
			if (fielditer == fieldInfo.MemberEnd()) { continue; }
			if (fielditer->value.IsString()) /*�ַ�������*/
			{
				pattern = fielditer->value.GetString();
			}
			if (fielditer->value.IsInt64())  /*��������*/
			{
				int pattern_int = fielditer->value.GetInt64();
				pattern = std::to_string(pattern_int);
			}
			if (fielditer->value.IsDouble())  /*����������*/
			{
				double pattern_db = fielditer->value.GetDouble();
				pattern = std::to_string(pattern_db);
			}

			/*���pattern��field��mode�ĺϷ���*/
			std::string dbfield;
			bool mode_flg = 0;
			bool find_field_flg = 0;
			int i_metatbsize = 0;
			for (; i_metatbsize < metaMapTableSize; ++i_metatbsize)
			{
				/*���ֶ�ӳ����в����ֶ����Ͳ�ѯģʽ���Ƿ�ƥ��*/
				if (pMetaMapTable[i_metatbsize].m_custon_field == fieldname)
				{
					find_field_flg = 1;
					dbfield = std::to_string(pMetaMapTable[i_metatbsize].m_db_field);
					if ((pMetaMapTable[i_metatbsize].m_mode & mode) > 0)
					{
						mode_flg = 1;
						break;
					}
				}
			}
			if (i_metatbsize >= metaMapTableSize)
			{
				/*��ѯģʽ����ȷ*/
				if (find_field_flg == 1 && mode_flg == 0) {
					bytes_append(response_body, LITERAL_STRING_U8("{\"status\":4,\"msg\":\""));
					bytes_append(response_body, fieldname);
					bytes_append(response_body, LITERAL_STRING_U8(u8"��ѯģʽ����ȷ\"}"));
					return 400;
				}
				/*δ�ҵ��ֶ�*/
				else if (find_field_flg == 0) {
					bytes_append(response_body, LITERAL_STRING_U8(u8"{\"status\":5,\"msg\":\"δ�ҵ�'"));
					bytes_append(response_body, fieldname);
					bytes_append(response_body, LITERAL_STRING_U8(u8"'�ֶ�\"}"));
					return 400;
				}
			}
			if (pattern.empty()) {
				continue; /*���ƥ�䴮Ϊ�գ��������ֶβ�����ѯ*/
			}
			/*���ݲ�ͬ��ѯģʽ��дsql���*/
			if (mode == 0x01)	/*��������ʽ��ѯ*/
			{
				pattern.push_back(' ');
				size_t posspc;
				size_t posbeg = 0;
				/*�����ѯ��ƥ�䴮�Կո�ָ����д��������ʽ*/
				std::string regexp;
				do {
					posspc = pattern.find_first_of(" ", posbeg);
					regexp.append(pattern.data(), posbeg, posspc - posbeg).append("|");
					posbeg = posspc + 1;
				} while (posspc < pattern.size() - 1);
				regexp.resize(regexp.size() - 1); /*ȥ����β�� | */
				sql.append("(`").append(dbfield).append("` regexp '").append(regexp).append("')");
			}
			else if (mode == 0x02)	/*�ַ�����ȷ��ѯ*/
			{
				sql.append("(`").append(dbfield).append("` = '").append(pattern).append("')");
			}
			else if (mode == 0x04 || mode == 0x10)	/*���ڻ�����ֵ�ľ�ȷ��ѯ*/
			{
				sql.append("(`").append(dbfield).append("` = ").append(pattern).append(")");
			}
			else {
				continue;
			}
			sql.append(" and ");
		}
	} // end if (queryobj.HasMember("match"))

	  /*range���� ����ֵ�������ڵķ�Χ��ѯ��*/
	iter = queryobj.FindMember("range");
	if (queryobj.HasMember("range") && iter->value.IsArray()) {
		const auto& rangeArray = iter->value.GetArray();
		for (int i = 0; i < rangeArray.Size(); ++i) {
			const rapidjson::Document::ValueType& fieldInfo = rangeArray[i];

			/*�ֶ���*/
			rapidjson::Document::ConstMemberIterator fielditer = fieldInfo.FindMember("field");
			if (fielditer == fieldInfo.MemberEnd() || !fielditer->value.IsString()) { continue; }
			std::string fieldname;
			fieldname.assign(fielditer->value.GetString(), fielditer->value.GetStringLength());

			/*��Сֵ*/
			std::string min_s;
			fielditer = fieldInfo.FindMember("min");
			if (fielditer != fieldInfo.MemberEnd()) {
				if (fielditer->value.IsInt64())   /*��������*/
				{
					min_s = std::to_string(fielditer->value.GetInt64());
				}
				if (fielditer->value.IsDouble())  /*����������*/
				{
					min_s = std::to_string(fielditer->value.GetDouble());
				}
			}
			/*���ֵ*/
			std::string max_s;
			fielditer = fieldInfo.FindMember("max");
			if (fielditer != fieldInfo.MemberEnd()) {
				if (fielditer->value.IsInt64())  /*��������*/
				{
					max_s = std::to_string(fielditer->value.GetInt64());
				}
				if (fielditer->value.IsDouble())  /*����������*/
				{
					max_s = std::to_string(fielditer->value.GetDouble());
				}
			}
			/*��������Сֵ��Ϊ�գ���������ѯ*/
			if (min_s.empty() && max_s.empty())
			{
				continue;
			}

			std::string dbfield;
			bool mode_flg = 0;  /*��ѯģʽ ��ȷΪ1*/
			bool find_field_flg = 0;  /*�ֶ��Ƿ���ȷ����ȷΪ1*/
									  /*���field��mode�ĺϷ���*/
			int i_metatbsize = 0;
			for (; i_metatbsize < metaMapTableSize; ++i_metatbsize)
			{
				/*���ֶ�ӳ����в����ֶ����Ͳ�ѯģʽ���Ƿ�ƥ��*/
				if (pMetaMapTable[i_metatbsize].m_custon_field == fieldname)
				{
					find_field_flg = 1;
					dbfield = std::to_string(pMetaMapTable[i_metatbsize].m_db_field);
					if ((pMetaMapTable[i_metatbsize].m_mode & 0x08) > 0 ||
						(pMetaMapTable[i_metatbsize].m_mode & 0x20) > 0)  /*0x08 �����ڵķ�Χ���� 0x20����ֵ�ķ�Χ����*/
					{
						mode_flg = 1;
						break;
					}
				}
			}
			if (i_metatbsize >= metaMapTableSize)
			{
				/*��ѯģʽ����ȷ*/
				if (find_field_flg == 1 && mode_flg == 0) {
					std::string rntStr = "{\"status\":6,\"msg\":\"";
					bytes_append(response_body, fieldname);
					bytes_append(response_body, LITERAL_STRING_U8(u8"�޷����з�Χ��ѯ\"}"));
					return 400;
				}
				/*δ�ҵ��ֶ�*/
				else if (find_field_flg == 0) {
					std::string rntStr = u8"{\"status\":7,\"msg\":\"δ�ҵ�'";
					bytes_append(response_body, fieldname);
					bytes_append(response_body, LITERAL_STRING_U8(u8"'�ֶ�\"}"));
					return 400;
				}
			}
			/*ֻ�����ֵ�����*/
			if (min_s.empty() && !max_s.empty()) {
				sql.append("(`").append(dbfield).append("` <= ").append(max_s).append(")");
				sql.append(" and ");
			}
			/*ֻ����Сֵ�����*/
			else if (!min_s.empty() && max_s.empty()) {
				sql.append("(`").append(dbfield).append("` >= ").append(min_s).append(")");
				sql.append(" and ");
			}
			/*�����Сֵ������*/
			else if (!min_s.empty() && !max_s.empty()) {
				sql.append("(`").append(dbfield).append("` between ").append(min_s).append(" and ").append(max_s).append(")");
				sql.append(" and ");
			}
		}
	} // end if (queryobj.HasMember("range"))

	  /*geometry���� ������Χ�Ĳ�ѯ[x,y,x,y,x,y...]��*/
	  //ʹ�õ���������geometry����
	iter = queryobj.FindMember("geometry");
	if (iter != queryobj.MemberEnd() && iter->value.IsArray()) {
		const auto& polyArray = iter->value.GetArray();
		std::vector<double> boxArray;
		boxArray.reserve(polyArray.Size() * 4);
		// �����ȡpolygon
		for (size_t i = 0; i < polyArray.Size(); ++i) {
			// "POLYGON((x1 y1,x2 y2,��,xn yn)��,(x1 y1,x2 y2,��,xm ym),......)"
			const auto& polyStr = polyArray[i];
			if (!polyStr.IsString()) { continue; }
			// ����һ
			const geos::geom::GeometryFactory* pFactory = geos::geom::GeometryFactory::getDefaultInstance();
			geos::io::WKTReader wktreader(pFactory);
			geos::geom::Geometry* pPoly =
				wktreader.read(std::string(polyStr.GetString(), polyStr.GetStringLength()));
			if (pPoly == NULL) { continue; }
			const geos::geom::Envelope* pBox = pPoly->getEnvelopeInternal();
			if (pBox == NULL) { pFactory->destroyGeometry(pPoly); continue; }
			boxArray.push_back(pBox->getMinX());
			boxArray.push_back(pBox->getMinY());
			boxArray.push_back(pBox->getMaxX());
			boxArray.push_back(pBox->getMaxY());
			pFactory->destroyGeometry(pPoly);
			// ������
		}

		/*��ȡ���ݿ��е��ֶ���*/
		std::string dbfiled_max_x, dbfiled_min_x, dbfiled_max_y, dbfiled_min_y;
		for (int i = 0; i < metaMapTableSize; ++i) {
			if (pMetaMapTable[i].m_custon_field == "MaxX") {
				dbfiled_max_x = std::to_string(pMetaMapTable[i].m_db_field);
			}
			if (pMetaMapTable[i].m_custon_field == "MinX") {
				dbfiled_min_x = std::to_string(pMetaMapTable[i].m_db_field);
			}
			if (pMetaMapTable[i].m_custon_field == "MaxY") {
				dbfiled_max_y = std::to_string(pMetaMapTable[i].m_db_field);
			}
			if (pMetaMapTable[i].m_custon_field == "MinY") {
				dbfiled_min_y = std::to_string(pMetaMapTable[i].m_db_field);
			}
		}
		std::string x_min_s, y_min_s, x_max_s, y_max_s;
		for (size_t i = 0; i < boxArray.size(); i += 4) {
			/*��дsql��ѯ���*/
			x_min_s = std::to_string(boxArray[i]);
			y_min_s = std::to_string(boxArray[i + 1]);
			x_max_s = std::to_string(boxArray[i + 2]);
			y_max_s = std::to_string(boxArray[i + 3]);

			/*��дsql��ѯ���*/
			sql.append("(not(").append(x_max_s).append(" <= `").append(dbfiled_min_x).append("` or ");
			sql.append(x_min_s).append(" >= `").append(dbfiled_max_x).append("` or ");
			sql.append(y_max_s).append(" <= `").append(dbfiled_min_y).append("` or ");
			sql.append(y_min_s).append(" >= `").append(dbfiled_max_y).append("`)) and ");
		}
	}

	/*��sql����β���ִ���*/
	size_t posand = sql.rfind("and");
	if (posand == sql.size() - 4) {
		sql.resize(sql.size() - 4); //ȥ����β��and
	}
	else {
		sql.resize(sql.size() - 6); //ȥ����β��where
	}

	/*size��������ȡ������¼*/
	if (jsondoc.HasMember("size") && jsondoc["size"].IsInt64()) {
		int size = jsondoc["size"].GetInt64();
		sql.append(" limit ").append(std::to_string(size));
	}

	/*from�������ӵڼ�����¼��ʼ��ȡ*/
	if (jsondoc.HasMember("from") && jsondoc["from"].IsInt64()) {
		int from = jsondoc["from"].GetInt64();
		sql.append(" offset ").append(std::to_string(from));
	}

	std::cout << sql << std::endl;

	/*ִ��sql��ѯ�������ز�ѯ���*/
	if (mysql_real_query(pMysql, sql.c_str(), sql.size()) != 0) {
		bytes_append(response_body,
			LITERAL_STRING_U8(u8"{\"status\":9,\"msg\":\"ִ��sql��ѯʧ��("));
		bytes_append(response_body, mysql_error(pMysql));
		bytes_append(response_body, ")\"}");
		return 200;
	}

	MYSQL_RES *result = mysql_store_result(pMysql);
	if (result == NULL) {
		bytes_append(response_body,
			LITERAL_STRING_U8(u8"{\"status\":10,\"msg\":\"��ȡsql��ѯ���ʧ��\"}"));
		return 200;
	}

	/*�����˶�����*/
	unsigned long long rows = mysql_num_rows(result);
	if (rows < 1) {
		mysql_free_result(result);
		bytes_append(response_body,
			LITERAL_STRING_U8(u8"{\"status\":11,\"url\":[]}"));
		return 200;
	}

	rapidjson::StringBuffer strbuf;
	strbuf.Reserve(rows * 2048);
	rapidjson::Writer<rapidjson::StringBuffer> writer(strbuf);
	/*����ȡ���Ľ��д��json*/
	writer.StartObject();
	writer.Key("status");
	writer.Int(0);
	writer.Key("urls");
	writer.StartArray();

	MYSQL_ROW rown;
	unsigned long* row_len = NULL;
	while ((rown = mysql_fetch_row(result)) != NULL &&
		(row_len = mysql_fetch_lengths(result)) != NULL) {
		for (int i = 0; i < mysql_num_fields(result); ++i)  //ֻ��һ���ֶ�url
		{
			std::string rows_s = std::string(rown[i], row_len[i]);
			writer.String(rows_s.c_str());
		}
	}
	writer.EndArray();
	writer.EndObject();

	bytes_append(response_body, (uint8_t*)strbuf.GetString(), strbuf.GetSize());

	mysql_free_result(result);

	if (pMysql)
	{
		mysql_close(pMysql);
	}

	return 200;
}




/// ********************************************************************************
/// <summary>
/// ��ȡ�û�Ԫ����
/// </summary>
/// <param name="path">HTTP GET����·��</param>
/// <param name="headers">HTTP�����ͷ</param>
/// <param name="query_parameters">URL�еĲ�ѯ����</param>
/// <param name="path_parameters">URL�е�·������</param>
/// <param name="request_body">�����Content-Body����</param>
/// <param name="dest_endpoint">�ն˵�ַ</param>
/// <param name="response_headers">��Ӧ��HTTPͷ</param>
/// <param name="response_body">��Ӧ��HTTP Content����</param>
/// <returns>HTTP��Ӧ״̬��</returns>
/// <created>solym@sohu.com,2018/8/29</created>
/// ********************************************************************************
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

	/*��ȡ����ʵ��*/
	rapidjson::Document jsondoc;
	jsondoc.Parse((char*)request_body.data(), request_body.size());
	if (jsondoc.HasParseError() || !jsondoc.IsObject()) {
		bytes_append(response_body,
			LITERAL_STRING_U8(u8"{\"status\":3,\"msg\":\"json��ʽ����ȷ\"}"));
		return 400;
	}

	/*������ʵ���л�ȡurl����*/
	std::vector<std::string> vect_urls;
	rapidjson::Value::ConstMemberIterator iter = jsondoc.FindMember("urls");
	if (iter != jsondoc.MemberEnd() && iter->value.IsArray())
	{
		const auto &urls = iter->value.GetArray();
		for (auto itr_urls = urls.Begin(); itr_urls != urls.End(); ++itr_urls)
		{
			if (itr_urls->IsString())
			{
				vect_urls.push_back(std::string(itr_urls->GetString(), itr_urls->GetStringLength()));
			}
		}
	}

	if (vect_urls.empty())
	{
		bytes_append(response_body,
			LITERAL_STRING_U8(u8"{\"status\":2,\"msg\":\"�޷�������ʵ���л�ȡ��_id����url\"}"));
		return 400;
	}


	/*�������ݿ�*/
	std::string hostip = "localhost";
	int port = 3306;
	std::string user = "root";
	std::string password = "";
	std::string database = "metadata2";
	std::string tablename = "ngcc_metadata";

	/*��ʼ��mysql*/
	MYSQL * pMysql = mysql_init(NULL);  /*�ڳ���ʹ��Mysql���ʼ�������mysql_int()��ʼ��*/
	if (pMysql == NULL)
	{
		std::string rntStr = "{\"status\": 1, \"msg\": \"mysql init failed\"}";
		response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));
		return 200;
	}
	/*�������ݿ������*/
	int arg = 1;
	mysql_options(pMysql, MYSQL_OPT_RECONNECT, &arg);

	if (mysql_real_connect(pMysql, hostip.c_str(),
		user.c_str(), password.c_str(), database.c_str(), port, NULL, 0) == NULL)
	{
		std::string rntStr = "{\"status\": 2, \"msg\": \"mysql_real_connect failed\"}";
		response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));
		return 500;
	}

	/*��ȡ�ֶ�ӳ���*/
	int metaMapTableSize = 0;
	const us_cms_metdata_map *pMetaMapTable = us_get_cms_metadata_map_table(&metaMapTableSize);
	if (pMetaMapTable == nullptr || metaMapTableSize < 0)
	{
		bytes_append(response_body,
			LITERAL_STRING_U8(u8"{\"status\":4,\"msg\":\"��ȡ�ֶ�ӳ���ʧ��\"}"));
		return 200;
	}

	std::vector<us_cms_metdata_map> metamaps;
	metamaps.reserve(metaMapTableSize + 1);
	/*metamaps.push_back({ "_id", 0, 1, 0 });*/
	metamaps.push_back({ "url", 0, 1, 0 });
	for (int i = 0; i < metaMapTableSize; ++i)
	{
		metamaps.push_back(pMetaMapTable[i]);
	}

	/*sql���*/
	std::string sql;
	size_t size_sql = 100 + metaMapTableSize * 10 + vect_urls.size() * 1024;
	sql.reserve(size_sql); /*ΪsqlԤ����洢�ռ�*/

	sql.append("select ");
	for (int i = 0; i < metamaps.size(); ++i)
	{
		const us_cms_metdata_map &field_metamap = metamaps[i];

		if (i == 0) //url
		{
			sql.append("url, ");
		}
		else if (field_metamap.m_custon_field == "ImgRange")
		{
			std::string i_str = std::to_string(field_metamap.m_db_field);
			sql.append("ST_AsText(`").append(i_str).append("`) as `").append(i_str).append("`, ");
		}
		else
		{
			std::string i_str = std::to_string(field_metamap.m_db_field);
			sql.append("`").append(i_str).append("`, ");
		}
	}
	sql = sql.substr(0, sql.size() - 2);
	sql.append(" from ").append(tablename);

	sql.append(" where ");

	if (!vect_urls.empty())
	{
		for (auto itr = vect_urls.begin(); itr != vect_urls.end(); ++itr)
		{
			if (itr->empty())
			{
				continue; /*�����ֵ������������ѯ*/
			}
			char buf_url[1534];
			int ret = mysql_real_escape_string(pMysql, buf_url, itr->c_str(), itr->size());
			std::string fd_url_str = std::string(buf_url, ret);
			sql.append("`url` = '").append(fd_url_str).append("' or ");
		}
	}
	sql.resize(sql.size() - 4); /*ȥ����β��or*/

	/*ִ��sql��ѯ�������ز�ѯ���*/
	if (mysql_real_query(pMysql, sql.c_str(), sql.size()) != 0)
	{
		bytes_append(response_body,
			LITERAL_STRING_U8(u8"{\"status\":5,\"msg\":\"ʵ��sql��ѯʧ��("));
		bytes_append(response_body, mysql_error(pMysql));
		bytes_append(response_body, ")\"}");
		return 200;
	}
	MYSQL_RES *result = mysql_store_result(pMysql);
	if (result == NULL)
	{
		bytes_append(response_body,
			LITERAL_STRING_U8(u8"{\"status\":6,\"msg\":\"��ȡsql��ѯ���ʧ��\"}"));
		return 200;
	}

	/*�����˶�����*/
	unsigned long long rows = mysql_num_rows(result);
	std::string rowstr = std::to_string(rows);
	if (rows < 1)
	{
		mysql_free_result(result);
		bytes_append(response_body,
			LITERAL_STRING_U8(u8"{\"status\":7,\"msg\":\"δ�ҵ���Ӧ�ļ�¼\"}"));
		return 200;
	}

	/*��ѯ���Ľ��*/
	rapidjson::StringBuffer strbuf;
	strbuf.Reserve(rows * 5120);
	rapidjson::Writer<rapidjson::StringBuffer> writer(strbuf);
	/*����ȡ���Ľ��д��json*/
	writer.StartObject();

	/*״ֵ̬Ϊ0*/
	writer.Key("status");
	writer.Int64(0);

	/*�ɹ�����Ŀ*/
	writer.Key("okn");
	writer.Int64(rows);

	/*���سɹ���Ԫ���ݼ�¼*/
	writer.Key("metas");
	writer.StartArray();  /*������ʽ*/

	MYSQL_ROW rown; /*һ�����ݵ����Ͱ�ȫ��ʾ*/
	unsigned long* row_lens = NULL;
	while ((rown = mysql_fetch_row(result)) != NULL &&
		(row_lens = mysql_fetch_lengths(result)) != NULL)
	{
		writer.StartObject();

		for (int i = 0; i < mysql_num_fields(result); ++i)
		{
			std::string strFd = std::string(rown[i], row_lens[i]);  /*��ȡ�ֶ�ֵ*/

			std::string fd_name = metamaps[i].m_custon_field;  /*�û��ɼ����ֶ���*/
			uint32_t fd_type = metamaps[i].m_type;  /*�ֶ�����*/

			if (i == 0) /* url*/
			{
				std::vector<std::string>::iterator itr
					= std::find(vect_urls.begin(), vect_urls.end(), strFd); /*����url*/
				if (itr != vect_urls.end())
				{
					itr->assign(""); /*set_urls���ʧ�ܵ�url*/
				}
				else
				{
					//�û������url��û��fd_s(�����������⵼��)����  
					break;
				}
			}

			/*���������*/
			if (fd_name == "MaxX" || fd_name == "MinX" || fd_name == "MaxY" || fd_name == "MinY")
			{
				continue;
			}

			/*������ֵ���ʹ洢���ֶ�*/
			else if (fd_type == 2)
			{
				writer.Key(fd_name.c_str());
				writer.Int64(atoi(strFd.c_str()));
			}

			/*��������ֵ���ʹ洢���ֶ�*/
			else if (fd_type == 3)
			{
				writer.Key(fd_name.c_str());
				writer.Double(atof(strFd.c_str()));
			}

			/*���ַ������ʹ洢���ֶ�*/
			else
			{
				writer.Key(fd_name.c_str());
				writer.String(strFd.c_str());
			}
		}
		writer.EndObject();
	} // end while (mysql_fetch_row(result))
	writer.EndArray();

	/*��ѯʧ�ܵĲ���*/
	int failn = 0;

	/*��ѯʧ�ܵ�id*/
	writer.Key("fails");
	writer.StartObject();

	/*��ѯʧ�ܵ�url*/
	writer.Key("urls");
	writer.StartArray();
	int failed_urln = 0;
	for (auto itr = vect_urls.begin(); itr != vect_urls.end(); ++itr)
	{
		if (!itr->empty())
		{
			writer.String(itr->c_str(), itr->size());
			failed_urln++;
		}
	}
	writer.EndArray();
	writer.EndObject();

	/*��ѯʧ�ܵ���Ŀ*/
	writer.Key("failn");
	writer.Int64(/*failed_idn +*/ failed_urln);

	writer.EndObject();

	bytes_append(response_body, (uint8_t*)strbuf.GetString(), strbuf.GetSize());

	mysql_free_result(result);

	if (pMysql != NULL)
	{
		mysql_close(pMysql);
	}

	return 200;
}



/// ********************************************************************************
/// <summary>
/// �ؽ��û�Ԫ���ݴ洢
/// </summary>
/// <param name="path">HTTP GET����·��</param>
/// <param name="headers">HTTP�����ͷ</param>
/// <param name="query_parameters">URL�еĲ�ѯ����</param>
/// <param name="path_parameters">URL�е�·������</param>
/// <param name="request_body">�����Content-Body����</param>
/// <param name="dest_endpoint">�ն˵�ַ</param>
/// <param name="response_headers">��Ӧ��HTTPͷ</param>
/// <param name="response_body">��Ӧ��HTTP Content����</param>
/// <returns>HTTP��Ӧ״̬��</returns>
/// <created>solym@sohu.com,2018/8/29</created>
/// ********************************************************************************
int us_cms_http_post_rebuild22(
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

	/*unispace::us_data_storage_manager& r_storage_mgr =
		unispace::us_data_storage_manager::get_instance();*/

	/*�������ݿ�*/
	std::string hostip = "localhost";
	int port = 3306;
	std::string user = "root";
	std::string password = "";
	std::string database = "metadata2";
	std::string tablename = "ngcc_metadata";

	/*��ʼ��mysql*/
	MYSQL * pMysql = mysql_init(NULL);  /*�ڳ���ʹ��Mysql���ʼ�������mysql_int()��ʼ��*/
	if (pMysql == NULL)
	{
		std::string rntStr = "{\"status\": 1, \"msg\": \"mysql init failed\"}";
		response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));
		return 200;
	}
	/*�������ݿ������*/
	int arg = 1;
	mysql_options(pMysql, MYSQL_OPT_RECONNECT, &arg);

	if (mysql_real_connect(pMysql, hostip.c_str(),
		user.c_str(), password.c_str(), database.c_str(), port, NULL, 0) == NULL)
	{
		std::string rntStr = "{\"status\": 2, \"msg\": \"mysql_real_connect failed\"}";
		response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));
		return 500;
	}

	/*��ȡ�ֶ�ӳ���*/
	int metaMapTableSize = 0;
	const us_cms_metdata_map *pMetaMapTable = us_get_cms_metadata_map_table(&metaMapTableSize);
	if (pMetaMapTable == nullptr || metaMapTableSize < 0)
	{
		bytes_append(response_body,
			LITERAL_STRING_U8(u8"{\"status\":2,\"msg\":\"��ȡ�ֶ�ӳ���ʧ��\"}"));
		return 200;
	}

	/*���Ԫ���ݴ洢��*/
	std::string sql_clear = "truncate table " + tablename;
	if (mysql_real_query(pMysql, sql_clear.c_str(), sql_clear.size()) != 0)
	{
		bytes_append(response_body,
			LITERAL_STRING_U8(u8"{\"status\":3,\"msg\":\"���Ԫ���ݲ��¼��ʧ��("));
		bytes_append(response_body, mysql_error(pMysql));
		bytes_append(response_body, ")\"}");
		return 200;
	}

	/*��ȡimageԪ���ݱ��е�ȫ����¼���ؽ��û�Ԫ���ݵĴ洢��¼*/
	std::string sql_read = "select _id, url from image";
	if (mysql_real_query(pMysql, sql_read.c_str(), sql_read.size()) != 0)
	{
		bytes_append(response_body,
			LITERAL_STRING_U8(u8"{\"status\":4,\"msg\":\"��ȡimage��ʧ��("));
		bytes_append(response_body, mysql_error(pMysql));
		bytes_append(response_body, ")\"}");
		return 200;
	}
	MYSQL_RES *result = mysql_store_result(pMysql);
	if (result == NULL)
	{
		bytes_append(response_body,
			LITERAL_STRING_U8(u8"{\"status\":5,\"msg\":\"��ȡsql��ѯ���ʧ��\"}"));
		return 200;
	}

	/*���صļ�¼��Ŀ�����С��1����imageԪ���ݱ�Ϊ�գ������ؽ�*/
	unsigned long long rows = mysql_num_rows(result);
	if (rows < 1)
	{
		bytes_append(response_body,
			LITERAL_STRING_U8(u8"{\"status\":6,\"msg\":\"image Ԫ���ݱ�Ϊ��\"}"));
		mysql_free_result(result);
		return 200;
	}

	/*mysqlԤ����sql���*/
	std::string sql_insert;
	sql_insert.reserve(8192);
	sql_insert.append("insert into ").append(tablename).append("(`_id`, `url`, `status`, ");
	for (int i = 0; i < metaMapTableSize; i++)
	{
		std::string buf_i = std::to_string(pMetaMapTable[i].m_db_field);
		sql_insert.append("`").append(buf_i).append("`, ");
	}
	sql_insert.resize(sql_insert.size() - 2); /*ȥ����β�ġ�, ��*/
	sql_insert.append(") values(?, ?, 0, ");   /*�ؽ�ʱstatus��Ϊ0*/  /*sql��䣬?ΪԤ�������*/
	for (int i = 0; i < metaMapTableSize; i++)
	{
		sql_insert.append("?, ");
	}
	sql_insert.resize(sql_insert.size() - 2);
	sql_insert.append(")");

	//��ʼ��Ԥ������
	MYSQL_STMT* pMysqlStmt = mysql_stmt_init(pMysql);
	if (!pMysqlStmt)
	{
		bytes_append(response_body,
			LITERAL_STRING_U8(u8"{\"status\":6,\"msg\":\"mysqlԤ��������ʼ��ʧ��\"}"));
		return 200;
	}
	//��sql������
	if (mysql_stmt_prepare(pMysqlStmt, sql_insert.c_str(), sql_insert.size()))
	{
		bytes_append(response_body,
			LITERAL_STRING_U8(u8"{\"status\":6,\"msg\":\"mysqlԤ����������ʧ��\"}"));
		return 200;
	}
	// ?��Ԥ��������� �ĸ���
	unsigned long paramCount = mysql_stmt_param_count(pMysqlStmt);
	if (paramCount != 76)
	{
		bytes_append(response_body,
			LITERAL_STRING_U8(u8"{\"status\":6,\"msg\":\"mysqlԤ���������������\"}"));
		return 200;
	}
	MYSQL_BIND* paramBind = new MYSQL_BIND[paramCount];
	if (!paramBind)
	{
		bytes_append(response_body,
			LITERAL_STRING_U8(u8"{\"status\":6,\"msg\":\"mysqlԤ�����������������ʧ��\"}"));
		return 200;
	}
	memset(paramBind, 0, paramCount * sizeof(MYSQL_BIND));

	MYSQL_ROW rown;
	unsigned long *row_len;
	std::vector<std::string> fail_urls;
	while ((rown = mysql_fetch_row(result)) != NULL &&
		(row_len = mysql_fetch_lengths(result)) != NULL)
	{
		std::string image_id(rown[0], row_len[0]);  /*��ȡ_id*/
		std::string image_url(rown[1], row_len[1]);   /*��ȡurl*/

		//����Ԥ�������id
		paramBind[0].buffer_type = MYSQL_TYPE_STRING;
		paramBind[0].buffer = (void*)(image_id.c_str());
		paramBind[0].buffer_length = image_id.size();

		//����Ԥ�������url
		paramBind[1].buffer_type = MYSQL_TYPE_STRING;
		paramBind[1].buffer = (void*)(image_url.c_str());
		paramBind[1].buffer_length = image_url.size();

		//����image�е�url��ȡԪ���ݼ�¼����json��ʽ�����out_json
		std::string out_json;
		int ret_read = us_read_cms_metadata_record(image_url, &out_json);
		if (ret_read < 0) {
			fail_urls.push_back(image_url);
			continue;
		}
		rapidjson::Document doc1;
		doc1.Parse(out_json.c_str());
		if (doc1.HasParseError()) {
			fail_urls.push_back(image_url);
			continue;
		}

		int intValue[15];
		double dbValue[20];
		int i_intvalue = 0;
		int j_dbvalue = 0;
		for (int i = 0; i < metaMapTableSize; ++i)
		{
			std::string colname = std::to_string(pMetaMapTable[i].m_db_field);
			rapidjson::Value::MemberIterator itr = doc1.FindMember(colname.c_str());
			if (itr == doc1.MemberEnd())
			{
				fail_urls.push_back(image_url);
				break;
			}

			std::string strValue = "";
			if (pMetaMapTable[i].m_custon_field == "ImgRange") {
				strValue = doc1[colname.c_str()].GetString();
				strValue = "ST_GeomFromText('Polygon" + strValue + "')";

				paramBind[i + 2].buffer_type = MYSQL_TYPE_STRING;
				paramBind[i + 2].buffer = (void*)(strValue.c_str());
				paramBind[i + 2].buffer_length = strValue.size();
			}
			/*ֻ������������*/
			else if (doc1[colname.c_str()].IsInt64())
			{
				intValue[i_intvalue] = doc1[colname.c_str()].GetInt64();
				paramBind[i + 2].buffer_type = MYSQL_TYPE_LONG;
				paramBind[i + 2].buffer = &intValue[i_intvalue++];
			}
			else if (doc1[colname.c_str()].IsDouble())
			{
				dbValue[j_dbvalue] = doc1[colname.c_str()].GetDouble();
				paramBind[i + 2].buffer_type = MYSQL_TYPE_DOUBLE;
				paramBind[i + 2].buffer = &dbValue[j_dbvalue++];
			}
			else if (doc1[colname.c_str()].IsString())
			{
				paramBind[i + 2].buffer_type = MYSQL_TYPE_STRING;
				paramBind[i + 2].buffer = (void*)(doc1[colname.c_str()].GetString());
				paramBind[i + 2].buffer_length = doc1[colname.c_str()].GetStringLength();
			}
		}
		if (mysql_stmt_bind_param(pMysqlStmt, paramBind)) /*��Ԥ�������*/
		{
			bytes_append(response_body,
				LITERAL_STRING_U8(u8"{\"status\":6,\"msg\":\"mysql_stmt_bind_param failed\"}"));
			bytes_append(response_body, mysql_stmt_error(pMysqlStmt));
			return 200;
		}
		if (mysql_stmt_execute(pMysqlStmt)) /*���󶨵�Ԥ�������ֵ���͵���������������ʹ�����ṩ�������滻���*/
		{
			bytes_append(response_body,
				LITERAL_STRING_U8(u8"{\"status\":6,\"msg\":\"mysql_stmtִ��ʧ��\"}"));
			bytes_append(response_body, mysql_stmt_error(pMysqlStmt));
			return 200;
		}
	}  // end while

	//if (0 == mysql_stmt_affected_rows(pMysqlStmt))  /*��ɾ�� �����ı������*/
	//{
	//	bytes_append(response_body,
	//		LITERAL_STRING_U8(u8"{\"status\":6,\"msg\":\"û�з����ı�ļ�¼\"}"));
	//	return 200;
	//}


	//MYSQL_ROW rown; /*һ�����ݵ����Ͱ�ȫ��ʾ*/
	//std::vector<std::string> fail_urls;
	//unsigned long *row_len;
	//while ((rown = mysql_fetch_row(result)) != NULL &&
	//	(row_len = mysql_fetch_lengths(result)) != NULL) {

	//	std::string image_id(rown[0], row_len[0]);  /*��ȡ_id*/
	//	std::string image_url(rown[1], row_len[1]);   /*��ȡurl*/

	//	// ��ȡʵ���ļ�·��
	//	/*unispace::us_ustring realpath = r_storage_mgr.get_image_real_path(image_url);*/
	//	
	//	//����image�е�url��ȡԪ���ݼ�¼����json��ʽ�����out_json
	//	std::string out_json;
	//	int ret_read = us_read_cms_metadata_record(image_url, &out_json);
	//	if (ret_read < 0) {
	//		fail_urls.push_back(image_url);
	//		continue;
	//	}
	//	rapidjson::Document doc1;
	//	doc1.Parse(out_json.c_str());
	//	if (doc1.HasParseError()) {
	//		fail_urls.push_back(image_url);
	//		continue;
	//	}

	//	/*�������ݣ���id��url��status������Ԫ����������Ϊ0����Ԫ����ȫ�����룩*/
	//	std::string sql_insert;
	//	sql_insert.reserve(8192);
	//	sql_insert = "insert into " + tablename + "(`_id`, `url`, `status`, ";
	//	for (int i = 0; i < metaMapTableSize; i++)
	//	{
	//		std::string buf_i = std::to_string(pMetaMapTable[i].m_db_field);
	//		sql_insert.append("`").append(buf_i).append("`, ");
	//	}
	//	sql_insert.resize(sql_insert.size() - 2);
	//	sql_insert.append(") values('").append(image_id).append("', '").append(image_url)
	//		.append("', ").append("0, ");   /*�ؽ�ʱstatus��Ϊ0*/

	//	for (int i = 0; i < metaMapTableSize; i++)
	//	{
	//		std::string colname = std::to_string(pMetaMapTable[i].m_db_field);

	//		std::string value_db = "";
	//		if (doc1.HasMember(colname.c_str()))
	//		{
	//			if (!doc1[colname.c_str()].IsNull())
	//			{
	//				if (pMetaMapTable[i].m_custon_field == "ImgRange") {
	//					value_db = doc1[colname.c_str()].GetString();
	//					value_db = "ST_GeomFromText('Polygon" + value_db + "')";
	//					sql_insert.append(value_db).append(", ");
	//				}
	//				/*ֻ������������*/
	//				else if (doc1[colname.c_str()].IsInt64())
	//				{
	//					int colv = doc1[colname.c_str()].GetInt64();
	//					value_db = std::to_string(colv);
	//					sql_insert.append(value_db).append(", ");
	//				}
	//				else if (doc1[colname.c_str()].IsDouble())
	//				{
	//					double colv = doc1[colname.c_str()].GetDouble();
	//					value_db = std::to_string(colv);
	//					sql_insert.append(value_db).append(", ");
	//				}
	//				else if (doc1[colname.c_str()].IsString())
	//				{
	//					value_db.assign(doc1[colname.c_str()].GetString(), 
	//						doc1[colname.c_str()].GetStringLength());
	//					sql_insert.append("'").append(value_db).append("', ");
	//				}
	//			}
	//		}
	//		else
	//		{
	//			fail_urls.push_back(image_url);
	//			break;
	//		}
	//	}  //end for

	//	sql_insert.resize(sql_insert.size() - 2);  /*ȥ����β��', '*/
	//	sql_insert.append(")");
	//	if (mysql_real_query(pMysql, sql_insert.data(), sql_insert.size()) != 0)
	//	{
	//		fail_urls.push_back(image_url);
	//		continue;
	//	}
	//}  // end while

	//mysql_free_result(result);

	bytes_append(response_body,
		LITERAL_STRING_U8(u8"{\"status\":0,\"fails\":[\""));
	for (size_t i = 0; i < fail_urls.size(); ++i) {
		bytes_append(response_body, fail_urls[i]);
		bytes_append(response_body, "\",\"");
	}
	if (!fail_urls.empty()) { response_body.pop_back(); }
	response_body.pop_back();

	bytes_append(response_body, "]}");

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

	/*unispace::us_data_storage_manager& r_storage_mgr =
		unispace::us_data_storage_manager::get_instance();*/

	/*�������ݿ�*/
	std::string hostip = "localhost";
	int port = 3306;
	std::string user = "root";
	std::string password = "";
	std::string database = "metadata2";
	std::string tablename = "ngcc_metadata";

	/*��ʼ��mysql*/
	MYSQL * pMysql = mysql_init(NULL);  /*�ڳ���ʹ��Mysql���ʼ�������mysql_int()��ʼ��*/
	if (pMysql == NULL)
	{
		std::string rntStr = "{\"status\": 1, \"msg\": \"mysql init failed\"}";
		response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));
		return 200;
	}
	/*�������ݿ������*/
	int arg = 1;
	mysql_options(pMysql, MYSQL_OPT_RECONNECT, &arg);

	if (mysql_real_connect(pMysql, hostip.c_str(),
		user.c_str(), password.c_str(), database.c_str(), port, NULL, 0) == NULL)
	{
		std::string rntStr = "{\"status\": 2, \"msg\": \"mysql_real_connect failed\"}";
		response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));
		return 500;
	}

	/*��ȡ�ֶ�ӳ���*/
	int metaMapTableSize = 0;
	const us_cms_metdata_map *pMetaMapTable = us_get_cms_metadata_map_table(&metaMapTableSize);
	if (pMetaMapTable == nullptr || metaMapTableSize < 0)
	{
		bytes_append(response_body,
			LITERAL_STRING_U8(u8"{\"status\":2,\"msg\":\"��ȡ�ֶ�ӳ���ʧ��\"}"));
		return 200;
	}

	/*���Ԫ���ݴ洢��*/
	std::string sql_clear = "truncate table " + tablename;
	if (mysql_real_query(pMysql, sql_clear.c_str(), sql_clear.size()) != 0)
	{
		bytes_append(response_body,
			LITERAL_STRING_U8(u8"{\"status\":3,\"msg\":\"���Ԫ���ݲ��¼��ʧ��("));
		bytes_append(response_body, mysql_error(pMysql));
		bytes_append(response_body, ")\"}");
		return 200;
	}

	/*��ȡimageԪ���ݱ��е�ȫ����¼���ؽ��û�Ԫ���ݵĴ洢��¼*/
	std::string sql_read = "select _id, url from image";
	if (mysql_real_query(pMysql, sql_read.c_str(), sql_read.size()) != 0)
	{
		bytes_append(response_body,
			LITERAL_STRING_U8(u8"{\"status\":4,\"msg\":\"��ȡimage��ʧ��("));
		bytes_append(response_body, mysql_error(pMysql));
		bytes_append(response_body, ")\"}");
		return 200;
	}
	MYSQL_RES *result = mysql_store_result(pMysql);
	if (result == NULL)
	{
		bytes_append(response_body,
			LITERAL_STRING_U8(u8"{\"status\":5,\"msg\":\"��ȡsql��ѯ���ʧ��\"}"));
		return 200;
	}

	/*���صļ�¼��Ŀ�����С��1����imageԪ���ݱ�Ϊ�գ������ؽ�*/
	unsigned long long rows = mysql_num_rows(result);
	if (rows < 1)
	{
		bytes_append(response_body,
			LITERAL_STRING_U8(u8"{\"status\":6,\"msg\":\"image Ԫ���ݱ�Ϊ��\"}"));
		mysql_free_result(result);
		return 200;
	}
	MYSQL_ROW rown; //һ�����ݵ����Ͱ�ȫ��ʾ
	std::vector<std::string> fail_urls;
	unsigned long *row_len;
	while ((rown = mysql_fetch_row(result)) != NULL &&
		(row_len = mysql_fetch_lengths(result)) != NULL) {
		std::string out_json;

		std::string image_id(rown[0], row_len[0]);  //��ȡ_id
		std::string image_url(rown[1], row_len[1]);   //��ȡurl

													  // ��ȡʵ���ļ�·��
		/*unispace::us_ustring realpath = r_storage_mgr.get_image_real_path(image_url);*/
		//����image�е�url��ȡԪ���ݼ�¼����json��ʽ�����out_json
		int ret_read = us_read_cms_metadata_record(image_url, &out_json);
		if (ret_read < 0) {
			fail_urls.push_back(image_url);
			continue;
		}
		rapidjson::Document doc1;
		doc1.Parse(out_json.c_str());
		if (doc1.HasParseError()) {
			fail_urls.push_back(image_url);
			continue;
		}

		/*�������ݣ���id��url��status������Ԫ����������Ϊ0����Ԫ����ȫ�����룩*/
		std::string sql_insert;
		sql_insert.reserve(8192);
		sql_insert = "insert into " + tablename + "(`_id`, `url`, `status`, ";
		for (int i = 0; i < metaMapTableSize; i++)
		{
			std::string buf_i = std::to_string(pMetaMapTable[i].m_db_field);
			sql_insert.append("`").append(buf_i).append("`, ");
		}
		sql_insert = sql_insert.substr(0, sql_insert.size() - 2);
		sql_insert.append(") values('").append(image_id).append("', '").append(image_url)
			.append("', ").append("0, ");   //�ؽ�ʱstatus��Ϊ0

		for (int i = 0; i < metaMapTableSize; i++)
		{
			std::string colname = std::to_string(pMetaMapTable[i].m_db_field);

			std::string value_db = "";
			if (doc1.HasMember(colname.c_str()))
			{
				if (!doc1[colname.c_str()].IsNull())
				{
					if (pMetaMapTable[i].m_custon_field == "ImgRange") {
						value_db = doc1[colname.c_str()].GetString();
						value_db = "ST_GeomFromText('Polygon" + value_db + "')";
						sql_insert.append(value_db).append(", ");
					}
					/*ֻ������������*/
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
				fail_urls.push_back(image_url);
				continue;
			}
		}

		sql_insert = sql_insert.substr(0, sql_insert.size() - 2);
		sql_insert.append(")");
		if (mysql_real_query(pMysql, sql_insert.data(), sql_insert.size()) != 0)
		{
			fail_urls.push_back(image_url);
			continue;
		}
	}  // end while

	mysql_free_result(result);

	bytes_append(response_body,
		LITERAL_STRING_U8(u8"{\"status\":0,\"fails\":[\""));
	for (size_t i = 0; i < fail_urls.size(); ++i) {
		bytes_append(response_body, fail_urls[i]);
		bytes_append(response_body, "\",\"");
	}
	if (!fail_urls.empty()) { response_body.pop_back(); }
	response_body.pop_back();

	bytes_append(response_body, "]}");

	if (pMysql)
	{
		mysql_close(pMysql);
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

	unispace::us_data_storage_manager& r_storage_mgr =
		unispace::us_data_storage_manager::get_instance();

	/*�������ݿ�*/
	std::string hostip = "localhost";
	int port = 3306;
	std::string user = "root";
	std::string password = "";
	std::string database = "metadata2";
	std::string tablename = "ngcc_metadata";

	/*��ʼ��mysql*/
	MYSQL * pMysql = mysql_init(NULL);  /*�ڳ���ʹ��Mysql���ʼ�������mysql_int()��ʼ��*/
	if (pMysql == NULL)
	{
		std::string rntStr = "{\"status\": 1, \"msg\": \"mysql init failed\"}";
		response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));
		return 200;
	}
	/*�������ݿ������*/
	int arg = 1;
	mysql_options(pMysql, MYSQL_OPT_RECONNECT, &arg);

	if (mysql_real_connect(pMysql, hostip.c_str(),
		user.c_str(), password.c_str(), database.c_str(), port, NULL, 0) == NULL)
	{
		std::string rntStr = "{\"status\": 2, \"msg\": \"mysql_real_connect failed\"}";
		response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));
		return 500;
	}

	/*��ȡ�ֶ�ӳ���*/
	int metaMapTableSize = 0;
	const us_cms_metdata_map *pMetaMapTable = us_get_cms_metadata_map_table(&metaMapTableSize);
	if (pMetaMapTable == NULL || metaMapTableSize < 0) {
		bytes_append(response_body,
			LITERAL_STRING_U8(u8"{\"status\":2,\"msg\":\"��ȡ�ֶ�ӳ���ʧ��("));
		bytes_append(response_body, mysql_error(pMysql));
		bytes_append(response_body, ")\"}");
		return 200;
	}

	/*����statusΪ1��Ӧ��url*/
	std::string sql_search_status = "select _id, url from " + tablename + " where status = 1";
	if (mysql_real_query(pMysql, sql_search_status.c_str(), sql_search_status.size()) != 0) {
		bytes_append(response_body,
			LITERAL_STRING_U8(u8"{\"status\":2,\"msg\":\"ִ��sql��ѯʧ��("));
		bytes_append(response_body, mysql_error(pMysql));
		bytes_append(response_body, ")\"}");
		return 200;
	}

	/*��ȡurl����ȡ���ݲ���Ԫ���ݼ�¼��*/
	MYSQL_RES *result = mysql_store_result(pMysql);
	if (result == NULL) {
		bytes_append(response_body,
			LITERAL_STRING_U8(u8"{\"status\":4,\"msg\":\"��ȡ��ѯ���ʧ��\"}"));
		return 200;
	}
	unsigned long long rows = mysql_num_rows(result);
	if (rows < 1) {
		mysql_free_result(result);
		bytes_append(response_body,
			LITERAL_STRING_U8(u8"{\"status\":0,\"msg\":\"�����޸���\"}"));
		return 200;
	}
	std::vector<std::string> fail_urls;
	MYSQL_ROW row_first; //һ�����ݵ����Ͱ�ȫ��ʾ
	unsigned long *row_len;
	while ((row_first = mysql_fetch_row(result)) != NULL &&
		(row_len = mysql_fetch_lengths(result)) != NULL) {
		std::string image_id(row_first[0], row_len[0]);  //��ȡ_id
		std::string image_url(row_first[1], row_len[1]);   //��ȡurl

		std::string jsonstr;
		// ��ȡʵ���ļ�·��
		unispace::us_ustring realpath = r_storage_mgr.get_image_real_path(image_url);
		/* ��ȡԪ���ݼ�¼ */
		int ret_js = us_read_cms_metadata_record(realpath, &jsonstr);
		if (ret_js < 0) {
			fail_urls.push_back(image_url);
			continue;
		}
		rapidjson::Document doc1;
		doc1.Parse(jsonstr.c_str());
		if (doc1.HasParseError()) {
			fail_urls.push_back(image_url);
			continue;
		}

		/* ����Ԫ���� */
		std::string sql_update;
		sql_update.reserve(8192);
		sql_update = "update " + tablename + " set status = 0, ";   //��������ʱ��status��Ϊ0
		for (int i_field = 0; i_field < metaMapTableSize; ++i_field) {
			std::string db_field_name = std::to_string(pMetaMapTable[i_field].m_db_field);   //���ݿ��ֶ���
			sql_update.append("`").append(db_field_name).append("` = ").append("");

			std::string db_field_value;
			if (doc1.HasMember(db_field_name.c_str())) {
				if (!doc1[db_field_name.c_str()].IsNull()) {
					if (pMetaMapTable[i_field].m_custon_field == "ImgRange") {
						db_field_value = doc1[db_field_name.c_str()].GetString();
						db_field_value = "ST_GeomFromText('Polygon" + db_field_value + "')";
						sql_update.append(db_field_value).append(", ");
					}
					/*ֻ������������*/
					else if (doc1[db_field_name.c_str()].IsInt64()) {
						int colv = doc1[db_field_name.c_str()].GetInt64();
						db_field_value = std::to_string(colv);
						sql_update.append(db_field_value).append(", ");
					}
					else if (doc1[db_field_name.c_str()].IsDouble()) {
						double colv = doc1[db_field_name.c_str()].GetDouble();
						db_field_value = std::to_string(colv);
						sql_update.append(db_field_value).append(", ");
					}
					else if (doc1[db_field_name.c_str()].IsString()) {
						db_field_value = doc1[db_field_name.c_str()].GetString();
						sql_update.append("'").append(db_field_value).append("', ");
					}
				}
			}
		}
		sql_update = sql_update.substr(0, sql_update.size() - 2);
		sql_update.append(" where url = '").append(image_url).append("'");

		if (mysql_real_query(pMysql, sql_update.c_str(), sql_update.size()) != 0) {
			fail_urls.push_back(image_url);
			continue;
		}
	} //end while
	  // �ͷŲ�ѯ���
	mysql_free_result(result);

	bytes_append(response_body,
		LITERAL_STRING_U8(u8"{\"status\":0,\"fails\":[\""));
	for (size_t i = 0; i < fail_urls.size(); ++i) {
		bytes_append(response_body, fail_urls[i]);
		bytes_append(response_body, "\",\"");
	}
	if (!fail_urls.empty()) { response_body.pop_back(); }
	response_body.pop_back();
	bytes_append(response_body, "]}");
	return 200;
}


int ngcc_create_tb_trigger(std::string host, int port,
	std::string user, std::string password, std::string database)
{
	//��ʼ��mysql
	MYSQL * pMysql = mysql_init(NULL); 
	if (pMysql == NULL)
	{
		return -1; /*mysql��ʼ��ʧ��*/
	}

	//�������ݿ������
	int arg = 1;
	mysql_options(pMysql, MYSQL_OPT_RECONNECT, &arg);

	if (mysql_real_connect(pMysql, host.c_str(),
		user.c_str(), password.c_str(), database.c_str(), port, NULL, 0) == NULL)
	{
		return -2; /*�������ݿ�ʧ��*/
	}

	//���ngcc_metadata�����ڵĻ�����ɾ���ٴ���
	std::string sql = "DROP TABLE IF EXISTS `ngcc_metadata`";
	if (mysql_real_query(pMysql, sql.c_str(), sql.size()) != 0)
	{
		printf("mysql_real_query failed: error %u ( %s )",
			mysql_errno(pMysql), mysql_error(pMysql));
		return -3;
	}

	//����ngcc_metadata���
	sql.clear();
	sql.reserve(4096);
	sql.append("CREATE TABLE `ngcc_metadata` (\
		`_id` binary(16) NOT NULL,\
		`url` varchar(767) NOT NULL,\
		`status` int(11) NOT NULL,\
		`1` double DEFAULT NULL COMMENT '����ΧMaxX',\
		`2` double DEFAULT NULL COMMENT '����ΧMinX',\
		`3` double DEFAULT NULL COMMENT '����ΧMaxY',\
		`4` double DEFAULT NULL COMMENT '����ΧMinY',\
		`5` geometry NOT NULL COMMENT '����Χ((���ϣ����ϣ�����������))',\
		`75` geometry NOT NULL COMMENT '����ǵ�((���ϣ����ϣ�����������))',\
		`6` varchar(50) DEFAULT NULL COMMENT 'Ԫ�����ļ�����',\
		`7` varchar(50) DEFAULT NULL COMMENT '��Ʒ����',\
		`8` varchar(50) DEFAULT NULL COMMENT '��Ʒ��Ȩ��λ��',\
		`9` varchar(50) DEFAULT NULL COMMENT '��Ʒ������λ��',\
		`10` varchar(50) DEFAULT NULL COMMENT '��Ʒ���浥λ��',\
		`11` int(11) DEFAULT NULL COMMENT '��Ʒ����ʱ��',\
		`12` varchar(10) DEFAULT NULL COMMENT '�ܼ�',\
		`13` double DEFAULT NULL COMMENT '����ֱ���',\
		`14` varchar(10) DEFAULT NULL COMMENT 'Ӱ��ɫ��ģʽ',\
		`15` int(11) DEFAULT NULL COMMENT '����λ��',\
		`16` double DEFAULT NULL COMMENT '������������С',\
		`17` varchar(30) DEFAULT NULL COMMENT '���ݸ�ʽ',\
		`18` varchar(20) DEFAULT NULL COMMENT '���򳤰뾶',\
		`19` varchar(20) DEFAULT NULL COMMENT '�������',\
		`20` varchar(20) DEFAULT NULL COMMENT '�����ô�ػ�׼',\
		`21` varchar(50) DEFAULT NULL COMMENT '��ͼͶӰ',\
		`22` int(11) DEFAULT NULL COMMENT '����������',\
		`23` varchar(10) DEFAULT NULL COMMENT '�ִ���ʽ',\
		`24` int(11) DEFAULT NULL COMMENT '��˹-������ͶӰ����',\
		`25` varchar(10) DEFAULT NULL COMMENT '���굥λ',\
		`26` varchar(10) DEFAULT NULL COMMENT '�߳�ϵͳ��',\
		`27` varchar(20) DEFAULT NULL COMMENT '�̻߳�׼',\
		`28` varchar(20) DEFAULT NULL COMMENT '��������',\
		`29` varchar(10) DEFAULT NULL COMMENT 'ȫɫӰ�񴫸�������',\
		`30` double DEFAULT NULL COMMENT 'ȫɫ����Ӱ��ֱ���',\
		`31` varchar(30) DEFAULT NULL COMMENT 'ȫɫ����Ӱ������',\
		`32` int(11) DEFAULT NULL COMMENT 'ȫɫ����Ӱ���ȡʱ��',\
		`33` varchar(10) DEFAULT NULL COMMENT '�����Ӱ�񴫸�������',\
		`34` int(11) DEFAULT NULL COMMENT '����ײ�������',\
		`35` varchar(20) DEFAULT NULL COMMENT '����ײ�������',\
		`36` double DEFAULT NULL COMMENT '���������Ӱ��ֱ���',\
		`37` varchar(30) DEFAULT NULL COMMENT '���������Ӱ������',\
		`38` int(11) DEFAULT NULL COMMENT '���������Ӱ���ȡʱ��',\
		`39` text COMMENT '����Ӱ��������������',\
		`40` int(11) DEFAULT NULL COMMENT 'DEM�������',\
		`41` varchar(30) DEFAULT NULL COMMENT 'DEM�������',\
		`42` varchar(30) DEFAULT NULL COMMENT '����������Դ',\
		`43` double DEFAULT NULL COMMENT '���������ƽ�������(X)',\
		`44` double DEFAULT NULL COMMENT '���������ƽ�������(Y)',\
		`45` double DEFAULT NULL COMMENT '���������߳������',\
		`46` varchar(20) DEFAULT NULL COMMENT '����������ҵԱ',\
		`47` varchar(20) DEFAULT NULL COMMENT '����������Ա',\
		`48` varchar(10) DEFAULT NULL COMMENT '����������ʽ',\
		`49` text COMMENT '����ģ�ͱ༭���',\
		`50` varchar(20) DEFAULT NULL COMMENT '����������',\
		`51` varchar(10) DEFAULT NULL COMMENT '�ز�������',\
		`52` text COMMENT '��������ܽ�',\
		`53` varchar(20) DEFAULT NULL COMMENT '���������ҵԱ',\
		`54` varchar(20) DEFAULT NULL COMMENT '����������Ա',\
		`55` double DEFAULT NULL COMMENT '�������ӱ߲�',\
		`56` double DEFAULT NULL COMMENT '�������ӱ߲�',\
		`57` double DEFAULT NULL COMMENT '�������ӱ߲�',\
		`58` double DEFAULT NULL COMMENT '�ϱ����ӱ߲�',\
		`59` text COMMENT '�ӱ���������',\
		`60` varchar(20) DEFAULT NULL COMMENT '�ӱ���ҵԱ',\
		`61` varchar(20) DEFAULT NULL COMMENT '�ӱ߼��Ա',\
		`62` double DEFAULT NULL COMMENT '�������׼���������(X)',\
		`63` double DEFAULT NULL COMMENT '�������׼���������(Y)',\
		`64` int(11) DEFAULT NULL COMMENT '�������',\
		`65` double DEFAULT NULL COMMENT '����ƽ�������',\
		`66` double DEFAULT NULL COMMENT '����������',\
		`67` text COMMENT 'Ժ��������',\
		`68` varchar(50) DEFAULT NULL COMMENT 'Ժ����鵥λ',\
		`69` varchar(20) DEFAULT NULL COMMENT 'Ժ�������',\
		`70` int(11) DEFAULT NULL COMMENT 'Ժ�����ʱ��',\
		`71` varchar(20) DEFAULT NULL COMMENT '�ּ�������',\
		`72` varchar(50) DEFAULT NULL COMMENT '�ּ����յ�λ',\
		`73` text COMMENT '�ּ��������',\
		`74` int(11) DEFAULT NULL COMMENT '�ּ�����ʱ��',\
		PRIMARY KEY(`_id`),\
		UNIQUE KEY `index_url` (`url`),\
		SPATIAL KEY `box` (`5`)\
		) ENGINE = InnoDB DEFAULT CHARSET = utf8");
	if (mysql_real_query(pMysql, sql.c_str(), sql.size()) != 0)
	{
		printf("mysql_real_query failed: error %u ( %s )",
			mysql_errno(pMysql), mysql_error(pMysql));
		return -3;
	}

	/*������������ڵĻ���ɾ���󴴽�*/
	sql = "DROP TRIGGER IF EXISTS `insert_ngcc`";
	if (mysql_real_query(pMysql, sql.c_str(), sql.size()) != 0)
	{
		printf("mysql_real_query failed: error %u ( %s )",
			mysql_errno(pMysql), mysql_error(pMysql));
		return -3;
	}

	/*����insert������*/
	sql = "CREATE TRIGGER `insert_ngcc` AFTER INSERT ON `image` FOR EACH ROW begin \
		insert into ngcc_metadata(_id, url, status, `5`, `75`) \
		values(new._id, new.url, 1, ST_GeomFromText('POLYGON((-1e10 -1e10, -1e10 -1e10, -1e10 -1e10, -1e10 -1e10, -1e10 -1e10))'), \
		ST_GeomFromText('POLYGON((-1e10 -1e10, -1e10 -1e10, -1e10 -1e10, -1e10 -1e10, -1e10 -1e10))'));\
		end ";
	if (mysql_real_query(pMysql, sql.c_str(), sql.size()) != 0)
	{
		printf("mysql_real_query failed: error %u ( %s )",
			mysql_errno(pMysql), mysql_error(pMysql));
		return -3;
	}

	/*���delete���������ڵĻ���ɾ���󴴽�*/
	sql = "DROP TRIGGER IF EXISTS `delete_ngcc`";
	if (mysql_real_query(pMysql, sql.c_str(), sql.size()) != 0)
	{
		printf("mysql_real_query failed: error %u ( %s )",
			mysql_errno(pMysql), mysql_error(pMysql));
		return -3;
	}

	/*����delete������*/
	sql = "CREATE TRIGGER `delete_ngcc` AFTER DELETE ON `image` FOR EACH ROW begin \
		delete from ngcc_metadata where _id = old._id;\
		end";
	if (mysql_real_query(pMysql, sql.c_str(), sql.size()) != 0)
	{
		printf("mysql_real_query failed: error %u ( %s )",
			mysql_errno(pMysql), mysql_error(pMysql));
		return -3;
	}

	if (pMysql != NULL)
	{
		mysql_close(pMysql);
	}

	return 0;
}

#endif



/// ********************************************************************************
/// <summary>
/// �ؽ��û�Ԫ���ݴ洢
/// </summary>
/// <param name="path">HTTP GET����·��</param>
/// <param name="headers">HTTP�����ͷ</param>
/// <param name="query_parameters">URL�еĲ�ѯ����</param>
/// <param name="path_parameters">URL�е�·������</param>
/// <param name="request_body">�����Content-Body����</param>
/// <param name="dest_endpoint">�ն˵�ַ</param>
/// <param name="response_headers">��Ӧ��HTTPͷ</param>
/// <param name="response_body">��Ӧ��HTTP Content����</param>
/// <returns>HTTP��Ӧ״̬��</returns>
/// <created>solym@sohu.com,2018/8/29</created>
/// ********************************************************************************
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

	/*�������ݿ�*/
	unispace::us_data_storage_manager& r_storage_mgr =
		unispace::us_data_storage_manager::get_instance();
	const std::string cmsTableName = "custmeta";

	std::string hostip = "localhost";
	int port = 3306;
	std::string user = "root";
	std::string password = "123456";
	std::string database = "shandong_metadata";

	/*��ʼ��mysql*/
	MYSQL * pMysql = mysql_init(NULL);  /*�ڳ���ʹ��Mysql���ʼ�������mysql_int()��ʼ��*/
	if (pMysql == NULL)
	{
		std::string rntStr = "{\"status\": 1, \"msg\": \"mysql init failed\"}";
		response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));
		return 200;
	}
	/*�������ݿ������*/
	int arg = 1;
	mysql_options(pMysql, MYSQL_OPT_RECONNECT, &arg);

	if (mysql_real_connect(pMysql, hostip.c_str(),
		user.c_str(), password.c_str(), database.c_str(), port, NULL, 0) == NULL)
	{
		std::string rntStr = "{\"status\": 2, \"msg\": \"mysql_real_connect failed\"}";
		response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));
		return 500;
	}

	/*��ȡ�ֶ�ӳ���*/
	int metaMapTableSize = 0;
	const us_cms_metdata_map *pMetaMapTable = us_get_cms_metadata_map_table(&metaMapTableSize);
	if (pMetaMapTable == nullptr || metaMapTableSize < 0)
	{
		bytes_append(response_body,
			LITERAL_STRING_U8(u8"{\"status\":2,\"msg\":\"��ȡ�ֶ�ӳ���ʧ��\"}"));
		return 200;
	}

	/*���Ԫ���ݴ洢��*/
	std::string sql_clear = "truncate table " + cmsTableName;
	if (mysql_real_query(pMysql, sql_clear.c_str(), sql_clear.size()) != 0)
	{
		bytes_append(response_body,
			LITERAL_STRING_U8(u8"{\"status\":3,\"msg\":\"���Ԫ���ݲ��¼��ʧ��("));
		bytes_append(response_body, mysql_error(pMysql));
		bytes_append(response_body, ")\"}");
		return 200;
	}

	/*��ȡimageԪ���ݱ��е�ȫ����¼���ؽ��û�Ԫ���ݵĴ洢��¼*/
	std::string sql_read = "select _id, url from image";
	if (mysql_real_query(pMysql, sql_read.c_str(), sql_read.size()) != 0)
	{
		bytes_append(response_body,
			LITERAL_STRING_U8(u8"{\"status\":4,\"msg\":\"��ȡimage��ʧ��("));
		bytes_append(response_body, mysql_error(pMysql));
		bytes_append(response_body, ")\"}");
		return 200;
	}
	MYSQL_RES *result = mysql_store_result(pMysql);
	if (result == NULL)
	{
		bytes_append(response_body,
			LITERAL_STRING_U8(u8"{\"status\":5,\"msg\":\"��ȡsql��ѯ���ʧ��\"}"));
		return 200;
	}

	/*���صļ�¼��Ŀ�����С��1����imageԪ���ݱ�Ϊ�գ������ؽ�*/
	unsigned long long rows = mysql_num_rows(result);
	if (rows < 1)
	{
		bytes_append(response_body,
			LITERAL_STRING_U8(u8"{\"status\":6,\"msg\":\"image Ԫ���ݱ�Ϊ��\"}"));
		mysql_free_result(result);
		return 200;
	}
	MYSQL_ROW rown; //һ�����ݵ����Ͱ�ȫ��ʾ
	std::vector<std::string> fail_urls;
	unsigned long *row_len;

	std::string sql_pre_insert;
	sql_pre_insert.reserve(metaMapTableSize*16);
	sql_pre_insert.append("insert into ").append(cmsTableName).append("(`_id`, `url`, `status`, ");
	for (int i = 0; i < metaMapTableSize; i++)
	{
		std::string buf_i = std::to_string(pMetaMapTable[i].m_db_field);
		sql_pre_insert.append("`").append(buf_i).append("`, ");
	}
	sql_pre_insert.resize(sql_pre_insert.size() - 2);

	while ((rown = mysql_fetch_row(result)) != NULL &&
		(row_len = mysql_fetch_lengths(result)) != NULL) {
		std::string out_json;

		std::string image_id(rown[0], row_len[0]);  //��ȡ_id
		std::string image_url(rown[1], row_len[1]);   //��ȡurl

		// ��ȡʵ���ļ�·��
		//unispace::us_ustring realpath = r_storage_mgr.get_image_real_path(image_url);
		//����image�е�url��ȡԪ���ݼ�¼����json��ʽ�����out_json
		int ret_read = us_read_cms_metadata_record(image_url, &out_json);
		if (ret_read < 0) {
			fail_urls.push_back(image_url);
			continue;
		}
		rapidjson::Document doc1;
		doc1.Parse(out_json.c_str());
		if (doc1.HasParseError()) {
			fail_urls.push_back(image_url);
			continue;
		}

		/*�������ݣ���id��url��status������Ԫ����������Ϊ0����Ԫ����ȫ�����룩*/
		std::string sql_insert = sql_pre_insert;
		sql_insert.reserve(metaMapTableSize*32);
		sql_insert.append(") values('").append(image_id).append("', '").append(image_url)
			.append("', ").append("0, ");   //�ؽ�ʱstatus��Ϊ0

		for (int i = 0; i < metaMapTableSize; i++)
		{
			std::string colname = std::to_string(pMetaMapTable[i].m_db_field);

			std::string value_db = "";
			rapidjson::Document::ConstMemberIterator iter = doc1.FindMember(colname.c_str());
			if (iter == doc1.MemberEnd())
			{
				break;
			}

			if (pMetaMapTable[i].m_custon_field == "ImgRange" &&
				iter->value.IsString()) {
				value_db = iter->value.GetString();
				value_db = "ST_GeomFromText('Polygon" + value_db + "')";
				sql_insert.append(value_db).append(", ");
			}
			/*ֻ������������*/
			else if (iter->value.IsInt64())
			{
				int colv = iter->value.GetInt64();
				value_db = std::to_string(colv);
				sql_insert.append(value_db).append(", ");
			}
			else if (iter->value.IsDouble())
			{
				double colv = iter->value.GetDouble();
				value_db = std::to_string(colv);
				sql_insert.append(value_db).append(", ");
			}
			else if (iter->value.IsString())
			{
				value_db = iter->value.GetString();
				sql_insert.append("'").append(value_db).append("', ");
			}
		}

		sql_insert.resize(sql_insert.size() - 2);
		sql_insert.append(")");
		if (mysql_real_query(pMysql, sql_insert.data(), sql_insert.size()) != 0)
		{
			fail_urls.push_back(image_url);
			continue;
		}
	}  // end while

	mysql_free_result(result);

	bytes_append(response_body,
		LITERAL_STRING_U8(u8"{\"status\":0,\"fails\":[\""));
	for (size_t i = 0; i < fail_urls.size(); ++i) {
		bytes_append(response_body, fail_urls[i]);
		bytes_append(response_body, "\",\"");
	}
	if (!fail_urls.empty()) { response_body.pop_back(); }
	response_body.pop_back();

	bytes_append(response_body, "]}");


	return 200;
}


/// ********************************************************************************
/// <summary>
/// ˢ���û�Ԫ���ݴ洢
/// </summary>
/// <param name="path">HTTP GET����·��</param>
/// <param name="headers">HTTP�����ͷ</param>
/// <param name="query_parameters">URL�еĲ�ѯ����</param>
/// <param name="path_parameters">URL�е�·������</param>
/// <param name="request_body">�����Content-Body����</param>
/// <param name="dest_endpoint">�ն˵�ַ</param>
/// <param name="response_headers">��Ӧ��HTTPͷ</param>
/// <param name="response_body">��Ӧ��HTTP Content����</param>
/// <returns>HTTP��Ӧ״̬��</returns>
/// <created>solym@sohu.com,2018/8/29</created>
/// ********************************************************************************
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

	unispace::us_data_storage_manager& r_storage_mgr =
		unispace::us_data_storage_manager::get_instance();

	/*�������ݿ�*/
	const std::string cmsTableName = "custmeta";

	std::string hostip = "localhost";
	int port = 3306;
	std::string user = "root";
	std::string password = "123456";
	std::string database = "shandong_metadata";

	/*��ʼ��mysql*/
	MYSQL * pMysql = mysql_init(NULL);  /*�ڳ���ʹ��Mysql���ʼ�������mysql_int()��ʼ��*/
	if (pMysql == NULL)
	{
		std::string rntStr = "{\"status\": 1, \"msg\": \"mysql init failed\"}";
		response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));
		return 200;
	}
	/*�������ݿ������*/
	int arg = 1;
	mysql_options(pMysql, MYSQL_OPT_RECONNECT, &arg);

	if (mysql_real_connect(pMysql, hostip.c_str(),
		user.c_str(), password.c_str(), database.c_str(), port, NULL, 0) == NULL)
	{
		std::string rntStr = "{\"status\": 2, \"msg\": \"mysql_real_connect failed\"}";
		response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));
		return 500;
	}

	/*��ȡ�ֶ�ӳ���*/
	int metaMapTableSize = 0;
	const us_cms_metdata_map *pMetaMapTable = us_get_cms_metadata_map_table(&metaMapTableSize);
	if (pMetaMapTable == NULL || metaMapTableSize < 0) {
		bytes_append(response_body,
			LITERAL_STRING_U8(u8"{\"status\":2,\"msg\":\"��ȡ�ֶ�ӳ���ʧ��("));
		bytes_append(response_body, mysql_error(pMysql));
		bytes_append(response_body, ")\"}");
		return 200;
	}

	/*����statusΪ1��Ӧ��url*/
	std::string sql_search_status = "select _id, url from " + cmsTableName + " where status = 1";
	if (mysql_real_query(pMysql, sql_search_status.c_str(), sql_search_status.size()) != 0) {
		bytes_append(response_body,
			LITERAL_STRING_U8(u8"{\"status\":2,\"msg\":\"ִ��sql��ѯʧ��("));
		bytes_append(response_body, mysql_error(pMysql));
		bytes_append(response_body, ")\"}");
		return 200;
	}

	/*��ȡurl����ȡ���ݲ���Ԫ���ݼ�¼��*/
	MYSQL_RES *result = mysql_store_result(pMysql);
	if (result == NULL) {
		bytes_append(response_body,
			LITERAL_STRING_U8(u8"{\"status\":4,\"msg\":\"��ȡ��ѯ���ʧ��\"}"));
		return 200;
	}
	unsigned long long rows = mysql_num_rows(result);
	if (rows < 1) {
		mysql_free_result(result);
		bytes_append(response_body,
			LITERAL_STRING_U8(u8"{\"status\":0,\"msg\":\"�����޸���\"}"));
		return 200;
	}
	std::vector<std::string> fail_urls;
	MYSQL_ROW row_first; //һ�����ݵ����Ͱ�ȫ��ʾ
	unsigned long *row_len;
	while ((row_first = mysql_fetch_row(result)) != NULL &&
		(row_len = mysql_fetch_lengths(result)) != NULL) {
		std::string image_id(row_first[0], row_len[0]);  //��ȡ_id
		std::string image_url(row_first[1], row_len[1]);   //��ȡurl

		std::string jsonstr;
		// ��ȡʵ���ļ�·��
		//unispace::us_ustring realpath = r_storage_mgr.get_image_real_path(image_url);
		/* ��ȡԪ���ݼ�¼ */
		int ret_js = us_read_cms_metadata_record(image_url, &jsonstr);
		if (ret_js < 0) {
			fail_urls.push_back(image_url);
			continue;
		}
		rapidjson::Document doc1;
		doc1.Parse(jsonstr.c_str());
		if (doc1.HasParseError()) {
			fail_urls.push_back(image_url);
			continue;
		}

		/* ����Ԫ���� */
		std::string sql_update;
		sql_update.reserve(8192);
		sql_update.append("update ").append(cmsTableName).append(" set status = 0, ");   //��������ʱ��status��Ϊ0
		for (int i_field = 0; i_field < metaMapTableSize; ++i_field) {
			std::string db_field_name = std::to_string(pMetaMapTable[i_field].m_db_field);   //���ݿ��ֶ���
			sql_update.append("`").append(db_field_name).append("` = ").append("");

			std::string db_field_value;
			rapidjson::Document::ConstMemberIterator iter = doc1.FindMember(db_field_name.c_str());
			if (iter == doc1.MemberEnd())
			{
				break;
			}
			if (pMetaMapTable[i_field].m_custon_field == "ImgRange" &&
				iter->value.IsString()) {
				db_field_value = iter->value.GetString();
				db_field_value = "ST_GeomFromText('Polygon" + db_field_value + "')";
				sql_update.append(db_field_value).append(", ");
			}
			/*ֻ������������*/
			else if (iter->value.IsInt64()) {
				int colv = iter->value.GetInt64();
				db_field_value = std::to_string(colv);
				sql_update.append(db_field_value).append(", ");
			}
			else if (iter->value.IsDouble()) {
				double colv = iter->value.GetDouble();
				db_field_value = std::to_string(colv);
				sql_update.append(db_field_value).append(", ");
			}
			else if (iter->value.IsString()) {
				db_field_value = iter->value.GetString();
				sql_update.append("'").append(db_field_value).append("', ");
			}
		}
		sql_update.resize(sql_update.size() - 2);
		sql_update.append(" where url = '").append(image_url).append("'");

		if (mysql_real_query(pMysql, sql_update.c_str(), sql_update.size()) != 0) {
			fail_urls.push_back(image_url);
			continue;
		}
	} //end while
	  // �ͷŲ�ѯ���
	mysql_free_result(result);

	bytes_append(response_body,
		LITERAL_STRING_U8(u8"{\"status\":0,\"fails\":[\""));
	for (size_t i = 0; i < fail_urls.size(); ++i) {
		bytes_append(response_body, fail_urls[i]);
		bytes_append(response_body, "\",\"");
	}
	if (!fail_urls.empty()) { response_body.pop_back(); }
	response_body.pop_back();
	bytes_append(response_body, "]}");
	return 200;
}

/// ********************************************************************************
/// <summary>
/// �����û�Ԫ����
/// </summary>
/// <param name="path">HTTP GET����·��</param>
/// <param name="headers">HTTP�����ͷ</param>
/// <param name="query_parameters">URL�еĲ�ѯ����</param>
/// <param name="path_parameters">URL�е�·������</param>
/// <param name="request_body">�����Content-Body����</param>
/// <param name="dest_endpoint">�ն˵�ַ</param>
/// <param name="response_headers">��Ӧ��HTTPͷ</param>
/// <param name="response_body">��Ӧ��HTTP Content����</param>
/// <returns>HTTP��Ӧ״̬��</returns>
/// <created>solym@sohu.com,2018/8/29</created>
/// ********************************************************************************
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
	// ����û�Ԫ���ݼ�¼�ı�
	const std::string cmsTableName = "custmeta";

	/*����request_body�е�json�ַ���*/
	rapidjson::Document jsondoc;
	jsondoc.Parse((char*)request_body.data(), request_body.size());
	if (jsondoc.HasParseError() || !jsondoc.IsObject()) {
		bytes_append(response_body,
			LITERAL_STRING_U8(u8"{\"status\":3,\"msg\":\"json��ʽ����ȷ\"}"));
		return 400;
	}
	rapidjson::Value::ConstMemberIterator queryiter = jsondoc.FindMember("query");
	if (queryiter == jsondoc.MemberEnd() || !queryiter->value.IsObject()) {
		bytes_append(response_body,
			LITERAL_STRING_U8(u8"{\"status\":3,\"msg\":\"json��ʽ����ȷ\"}"));
		return 400;
	}


	std::string hostip = "localhost";
	int port = 3306;
	std::string user = "root";
	std::string password = "123456";
	std::string database = "shandong_metadata";

	/*��ʼ��mysql*/
	MYSQL * pMysql = mysql_init(NULL);  /*�ڳ���ʹ��Mysql���ʼ�������mysql_int()��ʼ��*/
	if (pMysql == NULL)
	{
		std::string rntStr = "{\"status\": 1, \"msg\": \"mysql init failed\"}";
		response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));
		return 200;
	}
	/*�������ݿ������*/
	int arg = 1;
	mysql_options(pMysql, MYSQL_OPT_RECONNECT, &arg);

	if (mysql_real_connect(pMysql, hostip.c_str(),
		user.c_str(), password.c_str(), database.c_str(), port, NULL, 0) == NULL)
	{
		std::string rntStr = "{\"status\": 2, \"msg\": \"mysql_real_connect failed\"}";
		response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));
		return 500;
	}

	// ��ȡ�ֶ�ӳ���
	int metaMapTableSize = 0;
	const us_cms_metdata_map* pMetaMapTable = us_get_cms_metadata_map_table(&metaMapTableSize);
	if (pMetaMapTable == nullptr || metaMapTableSize < 0) {
		bytes_append(response_body,
			LITERAL_STRING_U8(u8"{\"status\":2,\"msg\":\"��ȡ�ֶ�ӳ���ʧ��\"}"));
		return 200;
	}

	// sql��䣬��ȡ����������url�ֶ�
	std::string sql;
	sql.reserve(4096 * 4);
	sql.append("select `url` from ").append(cmsTableName).append(" where ");

	/*query����*/
	const auto& queryobj = queryiter->value.GetObject();
	// match��������ȷ��ѯ���ַ����������ѯ)
	rapidjson::Document::ConstMemberIterator iter = queryobj.FindMember("match");
	if (iter != queryobj.MemberEnd() && iter->value.IsArray()) {
		const auto& matchArray = iter->value.GetArray();
		for (rapidjson::SizeType i = 0; i < matchArray.Size(); ++i) {
			// �����ȡ����ѯ�ֶ�����
			const rapidjson::Document::ValueType& fieldInfo = matchArray[i];
			std::string fieldname;
			int querymode = 0;
			// ��ȡ�ֶ���
			rapidjson::Document::ConstMemberIterator fielditer = fieldInfo.FindMember("field");
			if (fielditer == fieldInfo.MemberEnd() || !fielditer->value.IsString()) { continue; }
			fieldname.assign(fielditer->value.GetString(), fielditer->value.GetStringLength());
			// ��ȡ��ѯģʽ
			fielditer = fieldInfo.FindMember("mode");
			if (fielditer == fieldInfo.MemberEnd() || !fielditer->value.IsUint()) { continue; }
			querymode = fielditer->value.GetUint();

			// ��ȡƥ�䴮
			std::string pattern;
			fielditer = fieldInfo.FindMember("pattern");
			if (fielditer == fieldInfo.MemberEnd()) { continue; }
			if (fielditer->value.IsString()) { pattern = fielditer->value.GetString(); }
			else if (fielditer->value.IsInt64()) {
				pattern = std::to_string(fielditer->value.GetInt64());
			}
			else if (fielditer->value.IsDouble()) {
				pattern = std::to_string(fielditer->value.GetDouble());
			}
			// ���ƥ�䴮Ϊ�գ��������ֶβ�����ѯ
			if (pattern.empty()) { continue; }

			// ���ֶ�ӳ����в����ֶ�
			const us_cms_metdata_map* pFeild = NULL;
			for (int i = 0; i < metaMapTableSize; ++i) {
				if (pMetaMapTable[i].m_custon_field == fieldname) {
					pFeild = pMetaMapTable + i;
					break;
				}
			}
			if (pFeild == NULL) {
				bytes_append(response_body, LITERAL_STRING_U8(u8"{\"status\":5,\"msg\":\"δ�ҵ�'"));
				bytes_append(response_body, fieldname);
				bytes_append(response_body, LITERAL_STRING_U8(u8"'�ֶ�\"}"));
				return 400;
			}
			if ((pFeild->m_mode & querymode) == 0) {
				bytes_append(response_body, LITERAL_STRING_U8("{\"status\":4,\"msg\":\""));
				bytes_append(response_body, fieldname);
				bytes_append(response_body, LITERAL_STRING_U8(u8"��ѯģʽ����ȷ\"}"));
				return 400;
			}


			// ���ݲ�ͬ��ѯģʽ��дsql���
			switch (querymode) {
			case 0x01: /* ��������ʽ��ѯ */
			{
				// �����ѯ��ƥ�䴮�Կո�ָ����д��������ʽ
				// ���ո��滻Ϊ'|'
				std::string regexp;
				regexp.reserve(pattern.size());
				for (size_t i = 0, space = 0; i < pattern.size(); ++i) {
					if (pattern[i] == ' ') { ++space; continue; }
					if (space != 0) { regexp.push_back('|'); space = 0; }
					regexp.push_back(pattern[i]);
				}
				sql.append("(`").append(std::to_string(pFeild->m_db_field))
					.append("` regexp '").append(regexp).append("')").append(" and ");
			}
			break;
			case 0x02: /* �ַ�����ȷ��ѯ */
				sql.append("(`").append(std::to_string(pFeild->m_db_field))
					.append("` = '").append(pattern).append("')").append(" and ");
				break;
			case 0x04: /* ���ھ�ȷ��ѯ */
			case 0x10: /* ��ֵ��ȷ��ѯ */
				sql.append("(`").append(std::to_string(pFeild->m_db_field))
					.append("` = ").append(pattern).append(")").append(" and ");
				break;
			default:
				break;
			}
		}
	} // end if (queryobj.HasMember("match"))

	  // range���� ����ֵ�������ڵķ�Χ��ѯ
	iter = queryobj.FindMember("range");
	if (queryobj.HasMember("range") && iter->value.IsArray()) {
		const auto& rangeArray = iter->value.GetArray();
		for (rapidjson::SizeType i = 0; i < rangeArray.Size(); ++i) {
			const rapidjson::Document::ValueType& fieldInfo = rangeArray[i];
			std::string fieldname, minvalue, maxvalue;
			// ��ȡ�ֶ���
			rapidjson::Document::ConstMemberIterator fielditer = fieldInfo.FindMember("field");
			if (fielditer == fieldInfo.MemberEnd() || !fielditer->value.IsString()) { continue; }
			fieldname.assign(fielditer->value.GetString(), fielditer->value.GetStringLength());

			// ���ֶ�ӳ����в����ֶ�
			const us_cms_metdata_map* pFeild = NULL;
			for (int i = 0; i < metaMapTableSize; ++i) {
				if (pMetaMapTable[i].m_custon_field == fieldname) {
					pFeild = pMetaMapTable + i;
					break;
				}
			}
			if (pFeild == NULL) {
				bytes_append(response_body, LITERAL_STRING_U8(u8"{\"status\":5,\"msg\":\"δ�ҵ�'"));
				bytes_append(response_body, fieldname);
				bytes_append(response_body, LITERAL_STRING_U8(u8"'�ֶ�\"}"));
				return 400;
			}
			// 0x08 �����ڵķ�Χ���� 0x20����ֵ�ķ�Χ����
			if ((pFeild->m_mode & 0x28) == 0) {
				bytes_append(response_body, LITERAL_STRING_U8("{\"status\":4,\"msg\":\""));
				bytes_append(response_body, fieldname);
				bytes_append(response_body, LITERAL_STRING_U8(u8"��ѯģʽ����ȷ\"}"));
				return 400;
			}
			// ��Сֵ
			fielditer = fieldInfo.FindMember("min");
			if (fielditer != fieldInfo.MemberEnd()) {
				if (fielditer->value.IsInt64()) {
					minvalue = std::to_string(fielditer->value.GetInt64());
				}
				else if (fielditer->value.IsDouble()) {
					minvalue = std::to_string(fielditer->value.GetDouble());
				}
			}
			// ���ֵ
			fielditer = fieldInfo.FindMember("max");
			if (fielditer != fieldInfo.MemberEnd()) {
				if (fielditer->value.IsInt64()) {
					maxvalue = std::to_string(fielditer->value.GetInt64());
				}
				else if (fielditer->value.IsDouble()) {
					maxvalue = std::to_string(fielditer->value.GetDouble());
				}
			}
			// ��������Сֵ��Ϊ�գ���������ѯ
			if (minvalue.empty() && maxvalue.empty()) { continue; }

			// ֻ�����ֵ�����
			if (minvalue.empty()) {
				sql.append("(`").append(std::to_string(pFeild->m_db_field))
					.append("` <= ").append(maxvalue).append(")").append(" and ");
			}
			else if (maxvalue.empty()) {
				sql.append("(`").append(std::to_string(pFeild->m_db_field))
					.append("` >= ").append(minvalue).append(")").append(" and ");
			}
			else {
				sql.append("(`").append(std::to_string(pFeild->m_db_field))
					.append("` between ").append(minvalue).append(" and ")
					.append(maxvalue).append(")").append(" and ");
			}
		}
	} // end if (queryobj.HasMember("range"))

	  /*geometry���� ������Χ�Ĳ�ѯ[x,y,x,y,x,y...]��*/
	  //ʹ�õ���������geometry����
	iter = queryobj.FindMember("geometry");
	if (iter != queryobj.MemberEnd() && iter->value.IsArray()) {
		const auto& polyArray = iter->value.GetArray();
		std::vector<double> boxArray;
		boxArray.reserve(polyArray.Size() * 4);
		// ����һ��������ص�
		if (0) {
			// ��ȡ���ݿ��е��ֶ���
			std::string dbfiled_max_x, dbfiled_min_x, dbfiled_max_y, dbfiled_min_y;
			for (int i = 0; i < metaMapTableSize; ++i) {
				if (pMetaMapTable[i].m_custon_field == "MaxX") {
					dbfiled_max_x = std::to_string(pMetaMapTable[i].m_db_field);
				}
				else if (pMetaMapTable[i].m_custon_field == "MinX") {
					dbfiled_min_x = std::to_string(pMetaMapTable[i].m_db_field);
				}
				else if (pMetaMapTable[i].m_custon_field == "MaxY") {
					dbfiled_max_y = std::to_string(pMetaMapTable[i].m_db_field);
				}
				else if (pMetaMapTable[i].m_custon_field == "MinY") {
					dbfiled_min_y = std::to_string(pMetaMapTable[i].m_db_field);
				}
			}
			// �����ȡpolygon
			for (size_t i = 0; i < polyArray.Size(); ++i) {
				// "POLYGON((x1 y1,x2 y2,��,xn yn)��,(x1 y1,x2 y2,��,xm ym),......)"
				if (!polyArray[i].IsString()) { continue; }
				std::string polyStr(polyArray[i].GetString(), polyArray[i].GetStringLength());
				// ����һ
				const geos::geom::GeometryFactory* pFactory = geos::geom::GeometryFactory::getDefaultInstance();
				geos::io::WKTReader wktreader(pFactory);
				geos::geom::Geometry* pPoly = wktreader.read(polyStr);
				if (pPoly == NULL) { continue; }
				const geos::geom::Envelope* pBox = pPoly->getEnvelopeInternal();
				if (pBox == NULL) { pFactory->destroyGeometry(pPoly); continue; }
				boxArray.push_back(pBox->getMinX());
				boxArray.push_back(pBox->getMinY());
				boxArray.push_back(pBox->getMaxX());
				boxArray.push_back(pBox->getMaxY());
				pFactory->destroyGeometry(pPoly);
				std::string x_min_s, y_min_s, x_max_s, y_max_s;
				for (size_t i = 0; i < boxArray.size(); i += 4) {
					// ��дsql��ѯ���
					sql.append("(not(");
					// boxMaxX < imgMinX
					sql.append(std::to_string(boxArray[i + 2])).append(" < `").append(dbfiled_min_x).append("` or ");
					// boxMinX > imgMaxX
					sql.append(std::to_string(boxArray[i])).append(" > `").append(dbfiled_max_x).append("` or ");
					// boxMaxY < imgMinY
					sql.append(std::to_string(boxArray[i + 3])).append(" < `").append(dbfiled_min_y).append("` or ");
					// boxMinY > imgMaxY
					sql.append(std::to_string(boxArray[i + 1])).append(" > `").append(dbfiled_max_y).append("`)) and ");
				}
			}
		}
		// ��������MySQL�ռ�����
		{
			/*��ȡ���ݿ��е��ֶ���*/
			std::string dbImgRangefiledName("5");
			for (int i = 0; i < metaMapTableSize; ++i) {
				if (pMetaMapTable[i].m_custon_field == "ImgRange") {
					dbImgRangefiledName = std::to_string(pMetaMapTable[i].m_db_field);
				}
			}
			// �����ȡpolygon
			for (size_t i = 0; i < polyArray.Size(); ++i) {
				// "POLYGON((x1 y1,x2 y2,��,xn yn)��,(x1 y1,x2 y2,��,xm ym),......)"
				if (!polyArray[i].IsString()) { continue; }
				std::string polyStr(polyArray[i].GetString(), polyArray[i].GetStringLength());
				sql.append(" MBRIntersects(st_GeomFromText('").append(polyStr);
				sql.append("'),`").append(dbImgRangefiledName).append("`)").append(" and ");
			}
		}
	}

	// ��sql����β���ִ���(ȥ����β��and��where)
	size_t posand = sql.rfind("and");
	if (posand == sql.size() - 4) { sql.resize(sql.size() - 4); }
	else { sql.resize(sql.size() - 6); }

	// size��������ȡ������¼
	if (jsondoc.HasMember("size") && jsondoc["size"].IsInt64()) {
		size_t size = jsondoc["size"].GetInt64();
		sql.append(" limit ").append(std::to_string(size));
	}

	// from�������ӵڼ�����¼��ʼ��ȡ
	if (jsondoc.HasMember("from") && jsondoc["from"].IsInt64()) {
		int from = jsondoc["from"].GetInt64();
		sql.append(" offset ").append(std::to_string(from));
	}
	// ִ��sql��ѯ�������ز�ѯ���
	if (mysql_real_query(pMysql, sql.c_str(), sql.size()) != 0) {
		bytes_append(response_body,
			LITERAL_STRING_U8(u8"{\"status\":9,\"msg\":\"ִ��sql��ѯʧ��("));
		bytes_append(response_body, mysql_error(pMysql));
		bytes_append(response_body, ")\"}");
		return 200;
	}

	MYSQL_RES *result = mysql_store_result(pMysql);
	if (result == NULL) {
		bytes_append(response_body,
			LITERAL_STRING_U8(u8"{\"status\":10,\"msg\":\"��ȡsql��ѯ���ʧ��\"}"));
		return 200;
	}

	// �����˶�����
	unsigned long long numrows = mysql_num_rows(result);
	if (numrows < 1) {
		mysql_free_result(result);
		bytes_append(response_body,
			LITERAL_STRING_U8(u8"{\"status\":11,\"url\":[]}"));
		return 200;
	}

	// ����ȡ���Ľ��д��json
	rapidjson::StringBuffer strbuf;
	strbuf.Reserve(numrows * 2048);
	rapidjson::Writer<rapidjson::StringBuffer> writer(strbuf);
	writer.StartObject();
	writer.Key("status");
	writer.Int(0);
	writer.Key("urls");
	writer.StartArray();

	MYSQL_ROW rown;
	unsigned long* row_len = NULL;
	while ((rown = mysql_fetch_row(result)) != NULL &&
		(row_len = mysql_fetch_lengths(result)) != NULL) {
		unsigned int num_fields = mysql_num_fields(result);
		for (unsigned int i = 0; i < num_fields; ++i) {
			writer.String(rown[i], row_len[i]);
		}
	}
	writer.EndArray();
	writer.EndObject();

	bytes_append(response_body, (uint8_t*)strbuf.GetString(), strbuf.GetSize());

	mysql_free_result(result);

	return 200;
}

/// ********************************************************************************
/// <summary>
/// ��ȡ�û�Ԫ����
/// </summary>
/// <param name="path">HTTP GET����·��</param>
/// <param name="headers">HTTP�����ͷ</param>
/// <param name="query_parameters">URL�еĲ�ѯ����</param>
/// <param name="path_parameters">URL�е�·������</param>
/// <param name="request_body">�����Content-Body����</param>
/// <param name="dest_endpoint">�ն˵�ַ</param>
/// <param name="response_headers">��Ӧ��HTTPͷ</param>
/// <param name="response_body">��Ӧ��HTTP Content����</param>
/// <returns>HTTP��Ӧ״̬��</returns>
/// <created>solym@sohu.com,2018/8/29</created>
/// ********************************************************************************
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

	// ����û�Ԫ���ݼ�¼�ı�
	const std::string cmsTableName = "custmeta";

	/*��ȡ����ʵ��*/
	rapidjson::Document jsondoc;
	jsondoc.Parse((char*)request_body.data(), request_body.size());
	if (jsondoc.HasParseError() || !jsondoc.IsObject()) {
		bytes_append(response_body,
			LITERAL_STRING_U8(u8"{\"status\":3,\"msg\":\"json��ʽ����ȷ\"}"));
		return 400;
	}

	/*������ʵ���л�ȡurl����*/
	std::vector<std::string> vect_urls;

	rapidjson::Value::ConstMemberIterator iter = jsondoc.FindMember("urls");
	if (iter != jsondoc.MemberEnd() && iter->value.IsArray()) {
		const rapidjson::Value &urls = iter->value;
		for (auto itr_urls = urls.Begin(); itr_urls != urls.End(); ++itr_urls) {
			if (itr_urls->IsString()) {
				vect_urls.push_back(std::string(itr_urls->GetString(), itr_urls->GetStringLength()));
			}
		}
	}

	if (vect_urls.empty()) {
		bytes_append(response_body,
			LITERAL_STRING_U8(u8"{\"status\":2,\"msg\":\"�޷�������ʵ���л�ȡ��_id����url\"}"));
		return 400;
	}


	// �������ݿ�
	std::string hostip = "localhost";
	int port = 3306;
	std::string user = "root";
	std::string password = "123456";
	std::string database = "metadata2";

	/*��ʼ��mysql*/
	MYSQL * pMysql = mysql_init(NULL);  /*�ڳ���ʹ��Mysql���ʼ�������mysql_int()��ʼ��*/
	if (pMysql == NULL)
	{
		std::string rntStr = "{\"status\": 1, \"msg\": \"mysql init failed\"}";
		response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));
		return 200;
	}
	/*�������ݿ������*/
	int arg = 1;
	mysql_options(pMysql, MYSQL_OPT_RECONNECT, &arg);

	if (mysql_real_connect(pMysql, hostip.c_str(),
		user.c_str(), password.c_str(), database.c_str(), port, NULL, 0) == NULL)
	{
		std::string rntStr = "{\"status\": 2, \"msg\": \"mysql_real_connect failed\"}";
		response_body.insert(response_body.end(), (uint8_t*)rntStr.data(), (uint8_t*)(rntStr.data() + rntStr.size()));
		return 500;
	}

	// ��ȡ�ֶ�ӳ���
	int metaMapTableSize = 0;
	const us_cms_metdata_map *pMetaMapTable = us_get_cms_metadata_map_table(&metaMapTableSize);
	if (pMetaMapTable == nullptr || metaMapTableSize < 0) {
		bytes_append(response_body,
			LITERAL_STRING_U8(u8"{\"status\":4,\"msg\":\"��ȡ�ֶ�ӳ���ʧ��\"}"));
		return 200;
	}

	std::vector<us_cms_metdata_map> metamaps;
	metamaps.reserve(metaMapTableSize + 1);
	/*metamaps.push_back({ "_id", 0, 1, 0 });*/
	metamaps.push_back({ "url", 0, 1, 0 });
	for (int i = 0; i < metaMapTableSize; ++i) {
		metamaps.push_back(pMetaMapTable[i]);
	}

	// sql���
	std::string sql;
	size_t size_sql = 100 + metaMapTableSize * 10 + vect_urls.size() * 1024;
	sql.reserve(size_sql); //ΪsqlԤ����洢�ռ�
	sql.append("select url, ");
	for (int i = 1; i < metamaps.size(); ++i) {
		const us_cms_metdata_map &field = metamaps[i];
		if (field.m_custon_field == "ImgRange") {
			std::string i_str = std::to_string(field.m_db_field);
			sql.append("ST_AsText(`").append(i_str).append("`) as `").append(i_str).append("`, ");
			continue;
		}
		sql.append("`").append(std::to_string(field.m_db_field)).append("`, ");
	}
	sql = sql.substr(0, sql.size() - 2); // �Ƴ����ය��
	sql.append(" from ").append(cmsTableName).append(" where ");

	if (!vect_urls.empty()) {
		for (auto itr = vect_urls.begin(); itr != vect_urls.end(); ++itr) {
			if (itr->empty()) {
				continue; //�����ֵ������������ѯ
			}
			char buf_url[1534];
			int ret = mysql_real_escape_string(pMysql, buf_url, itr->c_str(), itr->size());
			std::string fd_url_str = std::string(buf_url, ret);
			sql.append("`url` = '").append(fd_url_str).append("' or ");
		}
	}
	sql = sql.substr(0, sql.size() - 4);

	// ִ��sql��ѯ�������ز�ѯ���
	if (mysql_real_query(pMysql, sql.c_str(), sql.size()) != 0) {
		bytes_append(response_body,
			LITERAL_STRING_U8(u8"{\"status\":5,\"msg\":\"ʵ��sql��ѯʧ��("));
		bytes_append(response_body, mysql_error(pMysql));
		bytes_append(response_body, ")\"}");
		return 200;
	}
	MYSQL_RES *result = mysql_store_result(pMysql);
	if (result == NULL) {
		bytes_append(response_body,
			LITERAL_STRING_U8(u8"{\"status\":6,\"msg\":\"��ȡsql��ѯ���ʧ��\"}"));
		return 200;
	}

	/*�����˶�����*/
	unsigned long long rows = mysql_num_rows(result);
	std::string rowstr = std::to_string(rows);
	if (rows < 1) {
		mysql_free_result(result);
		bytes_append(response_body,
			LITERAL_STRING_U8(u8"{\"status\":7,\"msg\":\"δ�ҵ���Ӧ�ļ�¼\"}"));
		return 200;
	}

	/*��ѯ���Ľ��*/
	rapidjson::StringBuffer strbuf;
	strbuf.Reserve(rows * 5120);
	rapidjson::Writer<rapidjson::StringBuffer> writer(strbuf);
	/*����ȡ���Ľ��д��json*/
	writer.StartObject();

	/*״ֵ̬Ϊ0*/
	writer.Key("status");
	writer.Int64(0);

	/*�ɹ�����Ŀ*/
	writer.Key("okn");
	writer.Int64(rows);


	/*���سɹ���Ԫ���ݼ�¼*/
	writer.Key("metas");
	writer.StartArray();  //������ʽ

	MYSQL_ROW rown = NULL;
	while ((rown = mysql_fetch_row(result)) != NULL) {
		writer.StartObject();
		unsigned long *row_lens = mysql_fetch_lengths(result);  //ÿһ�еĳ���

		for (int i = 0; i < mysql_num_fields(result); ++i) {
			const us_cms_metdata_map& field = metamaps[i];
			std::string fieldname = metamaps[i].m_custon_field;  //�û��ɼ����ֶ���
			std::string fieldvalue = std::string(rown[i], row_lens[i]);  //��ȡ�ֶ�ֵ

			if (fieldname == "url") {
				std::vector<std::string>::iterator itr
					= std::find(vect_urls.begin(), vect_urls.end(), fieldvalue); //����url
				if (itr != vect_urls.end()) {
					itr->resize(0); // δ�ÿյ���ʧ�ܵ�url
				}
			}

			// ���������
			if (fieldname == "MaxX" || fieldname == "MinX" ||
				fieldname == "MaxY" || fieldname == "MinY") {
				continue;
			}
			switch (field.m_type) {
			case 2: /* ������ֵ���ʹ洢���ֶ� */
			{
				writer.Key(fieldname.c_str());
				writer.Int64(atoi(fieldvalue.c_str()));
			}
			break;
			case 3: /* ��������ֵ���ʹ洢���ֶ� */
			{
				writer.Key(fieldname.c_str());
				writer.Double(atof(fieldvalue.c_str()));
			}
			break;
			default: /* ���ַ������ʹ洢���ֶ� */
			{
				writer.Key(fieldname.c_str());
				writer.String(fieldvalue.c_str());
			}
			break;
			}
		}
		writer.EndObject();
	}
	writer.EndArray();

	/*��ѯʧ�ܵĲ���*/
	int failn = 0;

	/*��ѯʧ�ܵ�id*/
	writer.Key("fails");
	writer.StartObject();

	/*��ѯʧ�ܵ�url*/
	writer.Key("urls");
	writer.StartArray();
	int failed_urln = 0;
	for (auto itr = vect_urls.begin(); itr != vect_urls.end(); ++itr) {
		if (!itr->empty()) {
			writer.String(itr->c_str(), itr->size());
			failed_urln++;
		}
	}
	writer.EndArray();
	writer.EndObject();

	/*��ѯʧ�ܵ���Ŀ*/
	writer.Key("failn");
	writer.Int64(/*failed_idn +*/ failed_urln);

	writer.EndObject();

	bytes_append(response_body, (uint8_t*)strbuf.GetString(), strbuf.GetSize());

	mysql_free_result(result);

	return 200;
}

