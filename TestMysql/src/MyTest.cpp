#include "MyTest.h"
#include <iostream>

MyTest::MyTest()
{
	m_testMysql = new Mymysql();
}

MyTest::~MyTest()
{
	if (m_testMysql != NULL)
	{
		delete m_testMysql;
		m_testMysql = NULL;
	}
}

int MyTest::connect_server(std::string hostIP, int port, std::string user, std::string password, std::string database)
{
	m_testMysql->set_mysql_options(MYSQL_OPT_RECONNECT, 1); //断开重连
	if (mysql_real_connect(m_testMysql->m_mysql, hostIP.c_str(),
		user.c_str(), password.c_str(), database.c_str(), port, NULL, 0) == NULL)
	{
		printf("mysql_real_connect failed: error %u ( %s )",
			mysql_errno(m_testMysql->m_mysql), mysql_error(m_testMysql->m_mysql));
		
		return -1;
	}

	return 0;
}

int MyTest::create_database(std::string database)
{
	std::string sql = "create database ";
	std::string buf;
	buf.resize(database.size()+1);

	int ret = mysql_real_escape_string(m_testMysql->m_mysql, (char*)buf.data(), database.c_str(), database.size());
	if (ret < 0)
	{
		printf("mysql_real_escape_string failed: error %u ( %s )",
			mysql_errno(m_testMysql->m_mysql), mysql_error(m_testMysql->m_mysql));
		return -1;
	}
	sql.append(buf.data(), ret);

	if (mysql_real_query(m_testMysql->m_mysql, sql.c_str(), sql.size()) != 0)
	{
		printf("mysql_real_query failed: error %u ( %s )",
			mysql_errno(m_testMysql->m_mysql), mysql_error(m_testMysql->m_mysql));
		return -1;
	}

	return 0;
}

int MyTest::select_database(std::string database)
{
	int ret = mysql_select_db(m_testMysql->m_mysql, database.c_str());
	if (ret != 0)
	{
		printf("mysql_select_db failed: error %u ( %s )",
			mysql_errno(m_testMysql->m_mysql), mysql_error(m_testMysql->m_mysql));
		return -1;
	}

	return 0;
}

int MyTest::create_table(std::string tablename, std::map<std::string, std::string> filed)
{
	std::string sql = "create table if not exists ";
	std::string buf;
	buf.resize(tablename.size() + 1);
	int ret = mysql_real_escape_string(m_testMysql->m_mysql, (char*)buf.data(), tablename.c_str(), tablename.size());
	if (ret < 0)
	{
		printf("mysql_real_escape_string failed: error %u ( %s )",
			mysql_errno(m_testMysql->m_mysql), mysql_error(m_testMysql->m_mysql));
		return -1;
	}
	sql.append(buf.data(), ret).append("(");
	
	for (auto itr = filed.begin(); itr != filed.end(); ++itr)
	{
		buf.resize(itr->first.size() + 1);
		ret = mysql_real_escape_string(m_testMysql->m_mysql, (char*)buf.data(), itr->first.c_str(), itr->first.size());
		sql.append("").append(buf.data(), ret).append(" ");

		buf.resize(itr->second.size() + 1);
		ret = mysql_real_escape_string(m_testMysql->m_mysql, (char*)buf.data(), itr->second.c_str(), itr->second.size());
		sql.append(buf.data(), ret).append(", ");
	}
	sql = sql.substr(0, sql.size() - 2);

	/*if (!primary.empty())   //Mysql数据库对于BLOB/TEXT这样类型的数据结构只能索引前N个字符，所以不能做主键
	{
		buf.resize(primary.size() + 1);
		ret = mysql_real_escape_string(m_testMysql->m_mysql, (char*)buf.data(), primary.c_str(), primary.size());
		sql.append("primary key(").append(buf.data(), ret).append(")");
	}*/  

	sql.append(")").append("ENGINE=InnoDB DEFAULT CHARSET=utf8");

	std::cout << sql << std::endl;

	if (mysql_real_query(m_testMysql->m_mysql, sql.c_str(), sql.size()) != 0)
	{
		printf("mysql_real_query failed: error %u ( %s )",
			mysql_errno(m_testMysql->m_mysql), mysql_error(m_testMysql->m_mysql));
		return -1;
	}

	return 0;

}

int MyTest::insert_data(std::vector<std::map<std::string, std::string>> value)
{
	for (auto itr = value.begin(); itr != value.end(); ++itr)
	{
		std::string sql = "insert into yxmeta(MetaDataFileName, DataFormat, CornerLonRange, MapProjection, CoordinationUnit) values(";

		for (auto it = itr->begin(); it != itr->end(); ++it)
		{
			std::string buf;
			buf.resize(100);
			int ret = mysql_real_escape_string(m_testMysql->m_mysql, (char*)buf.data(), it->second.c_str(), it->second.size());
			sql.append("'").append(buf.data(), ret).append("', ");
		}
		sql = sql.substr(0, sql.size() - 2);
		sql.append(")");

		std::cout << "\n" << sql << "\n" << std::endl;
		if (mysql_real_query(m_testMysql->m_mysql, sql.c_str(), sql.size()) != 0)
		{
			printf("mysql_real_query failed: error %u ( %s )",
				mysql_errno(m_testMysql->m_mysql), mysql_error(m_testMysql->m_mysql));
			return -1;
		}
	}

	return 0;
}

int MyTest::delete_data(std::string CoordinationUnit)
{
	
}


int main()
{
	MyTest *test = new MyTest();

	std::string hostip = "localhost";
	int port = 3306;
	std::string user = "root";
	std::string password = "";
	std::string database = "";
	int rnt = test->connect_server(hostip, port, user, password, database);
	if (rnt != 0)
	{
		std::cout << "connect_server falied" << std::endl;
	}

	std::string db = "meta";
	/*rnt = test->create_database(db);
	if (rnt != 0)
	{
		std::cout << "create_database failed" << std::endl;
	}*/
	
	/*选择数据库*/
	rnt = test->select_database(db);
	if (rnt != 0)
	{
		std::cout << "select_database failed" << std::endl;
	}

	/*创建表*/
	/*std::string tablename = "yxmeta";
	std::map < std::string, std::string > mtmap;
	mtmap.insert(std::pair<std::string, std::string>(std::string("MetaDataFileName"), std::string("TEXT")));
	mtmap.insert(std::pair<std::string, std::string>(std::string("DataFormat"), std::string("TEXT")));
	mtmap.insert(std::pair<std::string, std::string>(std::string("CornerLonRange"), std::string("TEXT")));
	mtmap.insert(std::pair<std::string, std::string>(std::string("MapProjection"), std::string("TEXT")));
	mtmap.insert(std::pair<std::string, std::string>(std::string("CoordinationUnit"), std::string("TEXT")));
	mtmap.insert(std::pair<std::string, std::string>(std::string("ImgSource"), std::string("TEXT")));
	mtmap.insert(std::pair<std::string, std::string>(std::string("PhotoDate"), std::string("TEXT")));
	mtmap.insert(std::pair<std::string, std::string>(std::string("DigitalPhotoResolution"), std::string("FLOAT"))); 
	mtmap.insert(std::pair<std::string, std::string>(std::string("SateName"), std::string("TEXT")));
	mtmap.insert(std::pair<std::string, std::string>(std::string("URL"), std::string("TEXT")));
	mtmap.insert(std::pair<std::string, std::string>(std::string("ID"), std::string("TEXT")));

	rnt = test->create_table(tablename, mtmap);*/

	std::vector<std::map<std::string, std::string>> valuevct;
	std::map<std::string, std::string> valuemap;
	valuemap.insert(std::pair<std::string, std::string>(std::string("MetaDataFileName"), std::string("mtflnm")));
	valuemap.insert(std::pair<std::string, std::string>(std::string("DataFormat"), std::string("dtfmt")));
	valuemap.insert(std::pair<std::string, std::string>(std::string("CornerLonRange"), std::string("crnrlnrg")));
	valuemap.insert(std::pair<std::string, std::string>(std::string("MapProjection"), std::string("mpprjct")));
	valuemap.insert(std::pair<std::string, std::string>(std::string("CoordinationUnit"), std::string("cdntunt")));
	valuevct.push_back(valuemap);
	
	valuemap.clear();
	valuemap.insert(std::pair<std::string, std::string>(std::string("MetaDataFileName"), std::string("mtflnm1")));
	valuemap.insert(std::pair<std::string, std::string>(std::string("DataFormat"), std::string("dtfmt1")));
	valuemap.insert(std::pair<std::string, std::string>(std::string("CornerLonRange"), std::string("crnrlnrg1")));
	valuemap.insert(std::pair<std::string, std::string>(std::string("MapProjection"), std::string("mpprjct1")));
	valuemap.insert(std::pair<std::string, std::string>(std::string("CoordinationUnit"), std::string("cdntunt1")));
	valuevct.push_back(valuemap);

	rnt = test->insert_data(valuevct);
	if (rnt != 0)
	{
		std::cout << "insert_data failed" << std::endl;
	}

	std::cout << "\n\nfinish" << std::endl;

	if (test != NULL)
	{
		delete test;
		test = NULL;
	}
	while (1) {}
	return 0;
}