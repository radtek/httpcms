#pragma once
#include "TestMysql.h"
#include <string>
#include <map>
#include <vector>

class MyTest
{
public:
	MyTest();
	~MyTest();

	int connect_server(std::string hostIP, int port, std::string user, std::string password, std::string database);
	int create_database(std::string database);
	int select_database(std::string database);
	int create_table(std::string tablename, std::map<std::string, std::string> filed);
	int insert_data(std::vector<std::map<std::string, std::string>> value);
	int delete_data(std::string CoordinationUnit);

private:
	Mymysql *m_testMysql;
};

