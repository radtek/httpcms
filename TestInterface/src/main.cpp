#include "field.h"
#include "http.h"
#include <iostream>
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/reader.h>
#include "rapidjson/filereadstream.h"
#include "rapidjson/filewritestream.h"
#include "mysql.h"
#include <sstream>
#include <time.h>
#include "../parsexml/pugixml.hpp"

int main()
{
	/*const std::string index = "E:/ZhangYuxin/Ԫ���ݹ���/GF124099420140601Y.XML";
	std::string out_json;
	int ret = us_read_cms_metadata_record(index, &out_json);

	std::cout << out_json << std::endl;*/
	/*{\"field\": \"ProduceDate\", \"mode\":4 ,\"pattern\":201608} \*/

	std::string jsonStr = u8"{															\
								\"query\":												\
								{														\
									\"match\":[											\
											  {\"field\":\"Producer\", \"mode\":1, \"pattern\": \"ɽ�� ���� ��Ϣ\"}   \
									],													\
									\"range\":[											\
											  {\"field\":\"GroundResolution\", \"min\":2, \"max\":3 },\
											  {\"field\":\"ProduceDate\", \"min\" : 1448812800, \"max\" : 1548812800 }\
									],													\
									\"geometry\":[	\
\"POLYGON((115.0267 26.96242,122.9868 26.50936,123.8821 33.55816,115.3364 34.16074,115.0267 26.96242))\" \
									]													\
								},														\
								\"from\": 0,											\
								\"size\":100											    \
						   }";

	/*rapidjson::Document doc1;
	doc1.Parse(jsonStr.c_str());               ///< ͨ��Parse������Json���ݽ�������
	if (doc1.HasParseError())
	{
		std::cout << "GetParseError%s\n" << doc1.GetParseError() << std::endl;
		return 1;
	}
	if (doc1.HasMember("query"))
	{
		if (doc1["query"].IsObject())
		{
			rapidjson::Value &queryobj = doc1["query"];
			if (queryobj.HasMember("match"))
			{
				if (queryobj["match"].IsArray())
				{
					for (int i = 0; i < queryobj["match"].Capacity(); ++i)
					{
						if (queryobj["match"][i].HasMember("field"))
						{
							if (queryobj["match"][i]["field"].IsString())
							{
								std::cout << queryobj["match"][i]["field"].GetString() << "\t";
							}
						}
						if (queryobj["match"][i].HasMember("mode"))
						{
							if (queryobj["match"][i]["mode"].IsInt())
							{
								std::cout << queryobj["match"][i]["mode"].GetInt() << "\t";
							}
						}
						if (queryobj["match"][i].HasMember("pattern"))
						{
							if (queryobj["match"][i]["pattern"].IsString())
							{
								std::cout << queryobj["match"][i]["pattern"].GetString() << "\t";
							}
							if (queryobj["match"][i]["pattern"].IsInt())
							{
								std::cout << queryobj["match"][i]["pattern"].GetInt() << "\t";
							}
							if (queryobj["match"][i]["pattern"].IsDouble())
							{
								std::cout << queryobj["match"][i]["pattern"].GetDouble() << "\t";
							}
						}
					}
				}
			}
			
			if (queryobj.HasMember("range"))
			{
				if (queryobj["range"].IsArray())
				{
					for (int i = 0; i < queryobj["range"].Capacity(); ++i)
					{
						if (queryobj["range"][i].HasMember("field"))
						{
							if (queryobj["range"][i]["field"].IsString())
							{
								std::cout << queryobj["range"][i]["field"].GetString() << "\t";
							}
						}
						if (queryobj["range"][i].HasMember("min"))
						{
							if (queryobj["range"][i]["min"].IsInt())
							{
								std::cout << queryobj["range"][i]["min"].GetInt() << "\t";
							}
							if (queryobj["range"][i]["min"].IsDouble())
							{
								std::cout << queryobj["range"][i]["min"].GetDouble() << "\t";
							}
							if (queryobj["range"][i]["min"].IsString())
							{
								std::cout << queryobj["range"][i]["min"].GetString() << "\t";
							}
						}
						if (queryobj["range"][i].HasMember("max"))
						{
							if (queryobj["range"][i]["max"].IsInt())
							{
								std::cout << queryobj["range"][i]["max"].GetInt() << "\t";
							}
							if (queryobj["range"][i]["max"].IsDouble())
							{
								std::cout << queryobj["range"][i]["max"].GetDouble() << "\t";
							}
							if (queryobj["range"][i]["max"].IsString())
							{
								std::cout << queryobj["range"][i]["max"].GetString() << "\t";
							}
						}
					}
				}
			}
			
			if (queryobj.HasMember("geometry"))
			{
				if (queryobj["geometry"].IsObject())
				{
					if (queryobj["geometry"].HasMember("geometry"))
					{
						if (queryobj["geometry"]["geometry"].IsArray())
						{
							for (int i = 0; i < queryobj["geometry"]["geometry"].Capacity(); ++i)
							{
								if (queryobj["geometry"]["geometry"][i].IsDouble())
								{
									std::cout << queryobj["geometry"]["geometry"][i].GetDouble() << std::endl;
								}
								if (queryobj["geometry"]["geometry"][i].IsInt())
								{
									std::cout << queryobj["geometry"]["geometry"][i].GetInt() << std::endl;
								}
							}
						}
					}
				}
			}
		}
	}
	if (doc1.HasMember("from"))
	{
		if (doc1["from"].IsInt())
		{
			std::cout << doc1["from"].GetInt() << "\n\n";
		}
	}
	if (doc1.HasMember("size"))
	{
		if (doc1["size"].IsInt())
		{
			std::cout << doc1["size"].GetInt() << "\n\n";
		}
	}
*/

/*�������ݿ����ݱ��������*/
//
//	///*�������ݿ�*/
//	//std::string hostip;
//	//int port;
//	//std::string user;
//	//std::string password;
//	//std::string database;
//	//std::string tablename;
//	//std::string metadata_file_index;
//
//	///*��ȡ�����ļ���json�ļ�����ȡip�Ͷ˿��Լ����ݿ�����*/
//	//rapidjson::Document doc_cfg;
//	//FILE *myFile = NULL;
//	//fopen_s(&myFile, "./ngcc_metadata_config.json", "rb");
//	//if (myFile)
//	//{
//	//	char buf_cfg[4096] = { 0 };
//	//	rapidjson::FileReadStream inputStream(myFile, buf_cfg, sizeof(buf_cfg));  //����һ��������
//	//	doc_cfg.ParseStream(inputStream); //����ȡ������ת��ΪdomԪ��
//	//	fclose(myFile); //�ر��ļ�������Ҫ
//	//}
//	//else
//	//{
//	//	return -1;
//	//}
//	//if (doc_cfg.HasParseError())
//	//{
//	//	return -1;
//	//}
//	//if (doc_cfg.HasMember("hostip"))
//	//{
//	//	if (doc_cfg["hostip"].IsString())
//	//	{
//	//		hostip = doc_cfg["hostip"].GetString();
//	//	}
//	//}
//	//if (doc_cfg.HasMember("port"))
//	//{
//	//	if (doc_cfg["port"].IsInt())
//	//	{
//	//		port = doc_cfg["port"].GetInt();
//	//	}
//	//}
//	//if (doc_cfg.HasMember("user"))
//	//{
//	//	if (doc_cfg["user"].IsString())
//	//	{
//	//		user = doc_cfg["user"].GetString();
//	//	}
//	//}
//	//if (doc_cfg.HasMember("password"))
//	//{
//	//	if (doc_cfg["password"].IsString())
//	//	{
//	//		password = doc_cfg["password"].GetString();
//	//	}
//	//}
//	//if (doc_cfg.HasMember("database"))
//	//{
//	//	if (doc_cfg["database"].IsString())
//	//	{
//	//		database = doc_cfg["database"].GetString();
//	//	}
//	//}
//	//if (doc_cfg.HasMember("tablename"))
//	//{
//	//	if (doc_cfg["tablename"].IsString())
//	//	{
//	//		tablename = doc_cfg["tablename"].GetString();
//	//	}
//	//}
//	//if (doc_cfg.HasMember("index"))
//	//{
//	//	if (doc_cfg["index"].IsString())
//	//	{
//	//		metadata_file_index = doc_cfg["index"].GetString();
//	//	}
//	//}
//
//	//MYSQL * m_mysql = mysql_init(NULL);  //�ڳ���ʹ��Mysql���ʼ�������mysql_int()��ʼ��
//	//if (m_mysql == NULL)
//	//{
//	//	return -1;
//	//}
//	///*�������ݿ������*/
//	//int arg = 1;
//	//mysql_options(m_mysql, MYSQL_OPT_RECONNECT, &arg);
//
//	//if (mysql_real_connect(m_mysql, hostip.c_str(),
//	//	user.c_str(), password.c_str(), NULL, port, NULL, 0) == NULL)
//	//{
//	//	return -1;
//	//}
//
//	///*�������ݿ�*/
//	//std::string sql_createdb = "create database if not exists ";
//	//std::string buf_database;
//	//buf_database.resize(database.size() + 1);
//	//int ret = mysql_real_escape_string(m_mysql, (char*)buf_database.data(), database.c_str(), database.size());
//	//if (ret < 0)
//	//{
//	//	return -1;
//	//}
//	//sql_createdb.append(buf_database.data(), ret);
//	//if (mysql_real_query(m_mysql, sql_createdb.c_str(), sql_createdb.size()) != 0)
//	//{
//	//	return -1;
//	//}
//
//	///*ѡ�����ݿ�*/
//	//ret = mysql_select_db(m_mysql, database.c_str());
//	//if (ret != 0)
//	//{
//	//	return -1;
//	//}
//
//
//	///*�������ݱ�*/
//	///*��ȡ�ֶ�ӳ���*/
//	//int out_size = 0;
//	//const us_cms_metdata_map *metamap = us_get_cms_metadata_map_table(&out_size);
//	//std::vector<us_cms_metdata_map> metamaps;
//	//metamaps.reserve(out_size);
//	//if (metamap != nullptr)
//	//{
//	//	for (int i = 0; i < out_size; ++i)
//	//	{
//	//		metamaps.push_back(*metamap);
//	//		metamap++;
//	//	}
//	//}
//	//else
//	//{
//	//	return -1;
//	//}
//
//	///*������񲢴洢����*/
//	//std::string sql = "create table if not exists ";
//	//std::string buf;
//	//buf.resize(tablename.size() + 1);
//	//ret = mysql_real_escape_string(m_mysql, (char*)buf.data(), tablename.c_str(), tablename.size());
//	//if (ret < 0)
//	//{
//	//	return -1;
//	//}
//	//sql.append(buf.data(), ret).append("(");
//
//	//sql.append("`").append("ids").append("` ").append("int not null").append(", ");
//	//sql.append("`").append("url").append("` ").append("text not null").append(", ");
//	//sql.append("`").append("status").append("` ").append("int not null").append(", ");
//
//	//for (auto itr = metamaps.begin(); itr != metamaps.end(); ++itr)
//	//{
//	//	char buf_dbf[10] = { 0 };
//	//	sprintf_s(buf_dbf, "%d", itr->m_db_field);
//	//	std::string dbf_s = std::string(buf_dbf, strlen(buf_dbf));
//	//	sql.append("`").append(dbf_s).append("` ");
//
//	//	std::string type_s;
//	//	if (itr->m_type == 1)
//	//	{
//	//		type_s = "TEXT";
//	//	}
//	//	else if (itr->m_type == 2)
//	//	{
//	//		type_s = "INT";
//	//	}
//	//	else if (itr->m_type == 3)
//	//	{
//	//		type_s = "DOUBLE";
//	//	}
//	//	sql.append(type_s).append(", ");
//	//}
//
//	//sql = sql.substr(0, sql.size() - 2);
//	//sql.append(")").append("ENGINE=InnoDB DEFAULT CHARSET=utf8");
//
//	//if (mysql_real_query(m_mysql, sql.c_str(), sql.size()) != 0)
//	//{
//	//	return -1;
//	//}
//
//
//	///*��������*/
//	///*��ȡ�û�Ԫ����*/
//	//const std::string index = metadata_file_index;
//	//std::string out_json;
//	//ret = us_read_cms_metadata_record(index, &out_json);
//	//if (ret < 0)
//	//{
//	//	return -1;
//	//}
//
//	//rapidjson::Document doc1;
//	//doc1.Parse(out_json.c_str());
//	//if (doc1.HasParseError())
//	//{
//	//	return -1;
//	//}
//
//
//	/*sql = "insert into metatb(`ids`, `url`, `status`) values(1, 'E:/ZhangYuxin/us/us/customer_metadata_service/doc/NGCC/ZY300613720160624Y.XML', 1)";
//	if (mysql_real_query(m_mysql, sql.c_str(), sql.size()) != 0)
//	{
//		return -1;
//	}*/
//
//	/*sql = "insert into metatb(`ids`, `url`, `status`, ";
//	for (int i = 1; i < out_size + 1; i++)
//	{
//		std::string buf_;
//		buf_.resize(3);
//		char buff_[3] = { 0 };
//		sprintf_s(buff_, "%d", i);
//		int ret = mysql_real_escape_string(m_mysql, (char*)buf_.data(), buff_, strlen(buff_));
//		sql.append("`").append(buf_.data(), ret).append("`, ");
//	}
//	sql = sql.substr(0, sql.size() - 2);
//	sql.append(") values(1, 'E:/ZhangYuxin/us/us/customer_metadata_service/doc/NGCC/ZY300613720160624Y.XML', 1, ");
//
//	std::cout << sql << std::endl;
//
//	for (int i = 1; i < out_size + 1; i++)
//	{
//		char buf_colname[10] = { 0 };
//		sprintf_s(buf_colname, "%d", i);
//		std::string colname = std::string(buf_colname, strlen(buf_colname));
//
//		std::string value_db = "";
//		if (doc1.HasMember(colname.c_str()))
//		{
//			if (!doc1[colname.c_str()].IsNull())
//			{
//				if (doc1[colname.c_str()].IsInt())
//				{
//					int colv = doc1[colname.c_str()].GetInt();
//					char buf1[100] = { 0 };
//					sprintf_s(buf1, "%d", colv);
//					value_db = std::string(buf1, strlen(buf1));
//					sql.append(value_db).append(", ");
//				}
//				else if (doc1[colname.c_str()].IsDouble())
//				{
//					double colv = doc1[colname.c_str()].GetDouble();
//					char buf1[100] = { 0 };
//					sprintf_s(buf1, "%lf", colv);
//					value_db = std::string(buf1, strlen(buf1));
//					sql.append(value_db).append(", ");
//				}
//				else if (doc1[colname.c_str()].IsString())
//				{
//					value_db = doc1[colname.c_str()].GetString();
//					sql.append("'").append(value_db).append("', ");
//				}
//			}
//		}
//	}
//
//	sql = sql.substr(0, sql.size() - 2);
//	sql.append(")");
//	std::cout << "\n" << sql << "\n" << std::endl;
//	if (mysql_real_query(m_mysql, sql.c_str(), sql.size()) != 0)
//	{
//		printf("mysql_select_db failed: error %u ( %s )",
//			mysql_errno(m_mysql), mysql_error(m_mysql));
//		return -1;
//	}
//
//	if (m_mysql != NULL)
//	{
//		mysql_close(m_mysql);
//	}
//*/
//

	const std::string path;
	const std::multimap< std::string, std::string > headers;
	const std::multimap< std::string, std::string > query_parameters;
	const std::map< std::string, std::string > path_parameters;
	std::vector<uint8_t> request_body;
	const std::string dest_endpoint;
	std::multimap< std::string, std::string > response_headers;
	std::vector<uint8_t> response_body;



	/*��ѯ����*/
	for (auto itr = jsonStr.begin(); itr != jsonStr.end(); ++itr)
	{
		request_body.push_back(*itr);
	}

	int ret = us_cms_http_get_search(path, headers, query_parameters, path_parameters, request_body, dest_endpoint, response_headers, response_body);
	
	for (auto itr = response_body.begin(); itr != response_body.end(); ++itr)
	{
		std::cout << *itr;
	}


	/*�ؽ�����*/
	/*int ret = us_cms_http_post_rebuild(path, headers, query_parameters, path_parameters, request_body, dest_endpoint, response_headers, response_body);
	for (auto itr = response_body.begin(); itr != response_body.end(); ++itr)
	{
		std::cout << *itr;
	}*/


	/*��ȡԪ���ݼ�¼����*/
	/*std::string jsons = "{\"ids\": [\"2\", \"3\", \"4\"], \"urls\": [\"E:/ZhangYuxin/us/mine_test/zyxces/image/test1F.tif\", \"E:/ZhangYuxin/us/us/customer_metadata_service/doc/NGCC/GF1147121520160316Y.XML\", \"zz\"]}";
	for (auto itr = jsons.begin(); itr != jsons.end(); ++itr)
	{
		request_body.push_back(*itr);
	}
	int ret = us_cms_http_get_getm(path, headers, query_parameters, path_parameters, request_body, dest_endpoint, response_headers, response_body);
	for (auto itr = response_body.begin(); itr != response_body.end(); ++itr)
	{
		std::cout << *itr;
	}*/

	/*ˢ�²���*/
	/*int ret = us_cms_http_post_refresh(path, headers, query_parameters, path_parameters, request_body, dest_endpoint, response_headers, response_body);
	for (auto itr = response_body.begin(); itr != response_body.end(); ++itr)
	{
		std::cout << *itr;
	}*/

	//int ret = us_cms_http_post_rebuild_addthread(path, headers, query_parameters, path_parameters, request_body, dest_endpoint, response_headers, response_body);


	//int ret = create();

	//int ret = create_chufaqi();


	/*std::string str;
	str.reserve(20);
	const char *pc = str.c_str();
	memset((char*)pc, 'z', 12);
	pc += 12;
	memset((char*)pc, 'a', 12);
	pc += 12;
	std::cout << str.c_str() << std::endl;*/


	//rapidjson::StringBuffer strbuf;
	//strbuf.Reserve(10);
	//rapidjson::Writer<rapidjson::StringBuffer> writer(strbuf);
	///*����ȡ���Ľ��д��json*/
	//writer.StartObject();

	//writer.Key("status");
	//writer.String("abcdefghijklmnopqrstuvwxyz");


	//std::string hostip = "localhost";
	//int port = 3306;
	//std::string user = "root";
	//std::string password = "";
	//std::string database = "mytest_db";
	//std::string tablename = "tutorials_tb1";

	///*��ʼ��mysql*/
	//MYSQL* pMysql = mysql_init(NULL);  //�ڳ���ʹ��Mysql���ʼ�������mysql_int()��ʼ��
	//if (pMysql == NULL)
	//{
	//	std::string rntStr = "{\"status\": 1, \"msg\": \"mysql init failed\"}";
	//	std::cout << rntStr << std::endl;
	//	return -1;
	//}

	////�������ݿ������
	//int arg = 1;
	//mysql_options(pMysql, MYSQL_OPT_RECONNECT, &arg);

	//if (mysql_real_connect(pMysql, hostip.c_str(),
	//	user.c_str(), password.c_str(), database.c_str(), port, NULL, 0) == NULL)
	//{
	//	std::string rntStr = "{\"status\": 2, \"msg\": \"mysql_real_connect failed\"}";
	//	std::cout << rntStr << std::endl;
	//	return -1;
	//}

	//pre_statement preStmt;
	//std::string sql = "select * from tutorials_tb1 where id = ?";  /*sql��䣬?ΪԤ�������*/
	//int ret = preStmt.init(pMysql, sql);  /*��ʼ��Ԥ����������ȡԤ��������ĸ��������½�bind�洢�ռ�*/
	//int idValue = 1;
	//ret = preStmt.set_param_bind(0, idValue);  /*����Ԥ�������*/

	////��Ԥ������������ؽ����
	//ret = preStmt.query();  /*��ѯ*/

	//char buf[1024 * 4];
	//unsigned long sizeField;
	//ret = preStmt.set_param_result(0, MYSQL_TYPE_LONG, buf, 1024, &sizeField); /*id*/
	//ret = preStmt.set_param_result(1, MYSQL_TYPE_STRING, buf + 1024, 1024, &sizeField); /*title*/
	//ret = preStmt.set_param_result(2, MYSQL_TYPE_STRING, buf + 2048, 1024, &sizeField); /*author*/
	//ret = preStmt.set_param_result(3, MYSQL_TYPE_TIMESTAMP, buf + 3072, 1024, &sizeField); /*date*/

	//ret = preStmt.get_result();
	//if (preStmt.fetch_result())
	//{
	//	std::string s(buf, 1024);
	//	std::cout << s << std::endl;
	//}


	/*std::string host = "localhost";
	int port = 3306;
	std::string user = "root";
	std::string password = "";
	std::string database = "metadata2";
	std::string tablename = "ngcc_metadata";
	int ret = ngcc_create_tb_trigger(host, port, user, password, database);*/

	//std::cout << "\n\nfinish\n" << std::endl;

	/*pugi::xml_document doc;
	if (!doc.load_file("E:/test111Y.XML", pugi::parse_default, pugi::encoding_utf8))
	{
		std::cout << "load failed.." << std::endl;
		while (1) {}
	}*/


	while (1) {}
	return 0;
}