#pragma once
#include <windows.h>
#include "mysql.h"
#include <vector>
#include <map>

class UsMySql
{
public:
	UsMySql();
	~UsMySql();

	/*连接服务器*/
	int connect_server(std::string hostIP, int port, std::string user, std::string password, std::string database);

	/*创建数据库*/
	int create_database(std::string database);

	/*选择数据库*/
	int select_database(std::string database);

	/*创建表格*/
	int create_table(std::string tablename, std::map<std::string, std::string> filed);

	/*创建表格*/
	int create_table(std::string tablename, std::map<int, std::string> filed);

	/*插入数据*/
	int insert_data(std::vector<std::map<std::string, std::string>> value);

	int delete_data(std::string CoordinationUnit);

public:
	MYSQL * m_mysql;
};


/*mysql预准备语句的使用*/
int mysql_stmt();
