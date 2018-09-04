#include "field.h"
#include "http.h"
#include <iostream>
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/reader.h>
#include "rapidjson/filereadstream.h"
#include "rapidjson/filewritestream.h"
#include "mysql.h"

int main()
{
	/*const std::string index = "E:/ZhangYuxin/元数据管理/GF124099420140601Y.XML";
	std::string out_json;
	int ret = us_read_cms_metadata_record(index, &out_json);

	std::cout << out_json << std::endl;*/
	/*{\"field\": \"ProduceDate\", \"mode\":4 ,\"pattern\":201608} \*/

	std::string jsonStr = u8"{															\
								\"query\":												\
								{														\
									\"match\":[											\
											  {\"field\":\"Producer\", \"mode\":1, \"pattern\": \"山西 地理 信息\"},  \
											  {\"field\" :\"ConfidentialLevel\", \"mode\":2, \"pattern\": \"秘密\"}, \
											  {\"field\": \"PixelBits\", \"mode\":16 ,\"pattern\":16}
									],													\
									\"range\":[											\
											  {\"field\":\"GroundResolution\", \"min\":0, \"max\":3 },\
											  {\"field\":\"ProduceDate\", \"min\":200902, \"max\":201809 }\
									],													\
									\"geometry\":{										\
											 \"polygon\":[3650266, 662044, 3912570, 662044, 3912570, 724062]\
									}													\
								},														\
								\"from\": 0,											\
								\"size\":2											    \
						   }";

	std::string jsonStr1 = u8"{															\
								\"query\":												\
								{														\
									\"match\":[											\
											  {\"field\":\"Producer\", \"mode\":1, \"pattern\": \"山西 地理 信息\"},  \
											  {\"field\" :\"ConfidentialLevel\", \"mode\":2, \"pattern\": \"秘密\"}, \
											  {\"field\": \"PixelBits\", \"mode\":16 ,\"pattern\":16} \
									],														\
									\"range\":[											\
												  {\"field\":\"GroundResolution\", \"min\":0, \"max\":3 },\
												  {\"field\":\"ProduceDate\", \"min\":200902, \"max\":201809 }\
										],													\
									\"geometry\":{										\
											 \"polygon\":[3550266, 662044, 3912570, 662044, 3912570, 724062]\
									}	\
								},\
		\
								\"from\": 1,											\
								\"size\":100											    \
						   }";

	/*rapidjson::Document doc1;
	doc1.Parse(jsonStr.c_str());               ///< 通过Parse方法将Json数据解析出来
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

/*创建数据库数据表插入数据*/
//
//	///*连接数据库*/
//	//std::string hostip;
//	//int port;
//	//std::string user;
//	//std::string password;
//	//std::string database;
//	//std::string tablename;
//	//std::string metadata_file_index;
//
//	///*读取配置文件（json文件）获取ip和端口以及数据库名等*/
//	//rapidjson::Document doc_cfg;
//	//FILE *myFile = NULL;
//	//fopen_s(&myFile, "./ngcc_metadata_config.json", "rb");
//	//if (myFile)
//	//{
//	//	char buf_cfg[4096] = { 0 };
//	//	rapidjson::FileReadStream inputStream(myFile, buf_cfg, sizeof(buf_cfg));  //创建一个输入流
//	//	doc_cfg.ParseStream(inputStream); //将读取的内容转换为dom元素
//	//	fclose(myFile); //关闭文件，很重要
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
//	//MYSQL * m_mysql = mysql_init(NULL);  //在程序使用Mysql的最开始必须调用mysql_int()初始化
//	//if (m_mysql == NULL)
//	//{
//	//	return -1;
//	//}
//	///*连接数据库服务器*/
//	//int arg = 1;
//	//mysql_options(m_mysql, MYSQL_OPT_RECONNECT, &arg);
//
//	//if (mysql_real_connect(m_mysql, hostip.c_str(),
//	//	user.c_str(), password.c_str(), NULL, port, NULL, 0) == NULL)
//	//{
//	//	return -1;
//	//}
//
//	///*创建数据库*/
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
//	///*选择数据库*/
//	//ret = mysql_select_db(m_mysql, database.c_str());
//	//if (ret != 0)
//	//{
//	//	return -1;
//	//}
//
//
//	///*创建数据表*/
//	///*获取字段映射表*/
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
//	///*创建表格并存储数据*/
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
//	///*插入数据*/
//	///*读取用户元数据*/
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

	/*查询测试*/
	for (auto itr = jsonStr.begin(); itr != jsonStr.end(); ++itr)
	{
		request_body.push_back(*itr);
	}

	int ret = us_cms_http_get_search(path, headers, query_parameters, path_parameters, request_body, dest_endpoint, response_headers, response_body);
	
	for (auto itr = response_body.begin(); itr != response_body.end(); ++itr)
	{
		std::cout << *itr;
	}


	/*重建测试*/
	/*int ret = us_cms_http_post_rebuild(path, headers, query_parameters, path_parameters, request_body, dest_endpoint, response_headers, response_body);
	for (auto itr = response_body.begin(); itr != response_body.end(); ++itr)
	{
		std::cout << *itr;
	}*/


	/*获取元数据记录测试*/
	/*std::string jsons = "{\"ids\": [\"2\"], \"urls\": [\"E:/ZhangYuxin/us/us/customer_metadata_service/doc/NGCC/ZY300613720160624Y.XML\"]}";
	for (auto itr = jsons.begin(); itr != jsons.end(); ++itr)
	{
		request_body.push_back(*itr);
	}
	int ret = us_cms_http_get_getm(path, headers, query_parameters, path_parameters, request_body, dest_endpoint, response_headers, response_body);
	for (auto itr = response_body.begin(); itr != response_body.end(); ++itr)
	{
		std::cout << *itr;
	}*/

	/*刷新测试*/
	/*int ret = us_cms_http_post_refresh(path, headers, query_parameters, path_parameters, request_body, dest_endpoint, response_headers, response_body);
	for (auto itr = response_body.begin(); itr != response_body.end(); ++itr)
	{
		std::cout << *itr;
	}*/


	while (1) {}
	return 0;
}