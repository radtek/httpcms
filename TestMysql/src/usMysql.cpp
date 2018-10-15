#include "usMysql.h"
#include <iostream>
#include <string>

UsMySql::UsMySql()
{
	m_mysql = mysql_init(NULL);  //在程序使用Mysql的最开始必须调用mysql_int()初始化
	if (m_mysql == NULL)
	{
		std::cout << "mysql init failed" << std::endl;
	}
}

UsMySql::~UsMySql()
{
	if (m_mysql != NULL)
	{
		mysql_close(m_mysql);
	}
}

int UsMySql::connect_server(std::string hostIP, int port, std::string user, std::string password, std::string database)
{
	/*设置断开重连*/
	int arg = 1;
	mysql_options(m_mysql, MYSQL_OPT_RECONNECT, &arg); 
	
	/*连接mysql服务器*/
	if (mysql_real_connect(m_mysql, hostIP.c_str(),
		user.c_str(), password.c_str(), database.c_str(), port, NULL, 0) == NULL)
	{
		printf("mysql_real_connect failed: error %u ( %s )",
			mysql_errno(m_mysql), mysql_error(m_mysql));

		return -1;
	}

	return 0;
}

int UsMySql::create_database(std::string database)
{
	std::string sql = "create database ";
	std::string buf;
	buf.resize(database.size() + 1);

	int ret = mysql_real_escape_string(m_mysql, (char*)buf.data(), database.c_str(), database.size());  //from参数中 的字符串经过编码以生成转义的SQL字符串，并考虑连接的当前字符集
	if (ret < 0)
	{
		printf("mysql_real_escape_string failed: error %u ( %s )",
			mysql_errno(m_mysql), mysql_error(m_mysql));
		return -1;
	}
	sql.append(buf.data(), ret);

	if (mysql_real_query(m_mysql, sql.c_str(), sql.size()) != 0)
	{
		printf("mysql_real_query failed: error %u ( %s )",
			mysql_errno(m_mysql), mysql_error(m_mysql));
		return -1;
	}

	return 0;
}

int UsMySql::select_database(std::string database)
{
	int ret = mysql_select_db(m_mysql, database.c_str());
	if (ret != 0)
	{
		printf("mysql_select_db failed: error %u ( %s )",
			mysql_errno(m_mysql), mysql_error(m_mysql));
		return -1;
	}

	return 0;
}

int UsMySql::create_table(std::string tablename, std::map<std::string, std::string> filed)
{
	std::string sql = "create table if not exists ";
	std::string buf;
	buf.resize(tablename.size() + 1);
	int ret = mysql_real_escape_string(m_mysql, (char*)buf.data(), tablename.c_str(), tablename.size());
	if (ret < 0)
	{
		printf("mysql_real_escape_string failed: error %u ( %s )",
			mysql_errno(m_mysql), mysql_error(m_mysql));
		return -1;
	}
	sql.append(buf.data(), ret).append("(");

	for (auto itr = filed.begin(); itr != filed.end(); ++itr)
	{
		buf.resize(itr->first.size() + 1);
		ret = mysql_real_escape_string(m_mysql, (char*)buf.data(), itr->first.c_str(), itr->first.size());
		sql.append("`").append(buf.data(), ret).append("` ");

		buf.resize(itr->second.size() + 1);
		ret = mysql_real_escape_string(m_mysql, (char*)buf.data(), itr->second.c_str(), itr->second.size());
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

	if (mysql_real_query(m_mysql, sql.c_str(), sql.size()) != 0)
	{
		printf("mysql_real_query failed: error %u ( %s )",
			mysql_errno(m_mysql), mysql_error(m_mysql));
		return -1;
	}

	return 0;

}


int UsMySql::create_table(std::string tablename, std::map<int, std::string> filed)
{
	std::string sql = "create table if not exists ";
	std::string buf;
	buf.resize(tablename.size() + 1);
	int ret = mysql_real_escape_string(m_mysql, (char*)buf.data(), tablename.c_str(), tablename.size());
	if (ret < 0)
	{
		printf("mysql_real_escape_string failed: error %u ( %s )",
			mysql_errno(m_mysql), mysql_error(m_mysql));
		return -1;
	}
	sql.append(buf.data(), ret).append("(");

	for (auto itr = filed.begin(); itr != filed.end(); ++itr)
	{
		buf.resize(3);
		char buff[3] = { 0 };
		sprintf_s(buff, "%d", itr->first);
		ret = mysql_real_escape_string(m_mysql, (char*)buf.data(), buff, strlen(buff));
		sql.append("`").append(buf.data(), ret).append("` ");

		buf.resize(itr->second.size() + 1);
		ret = mysql_real_escape_string(m_mysql, (char*)buf.data(), itr->second.c_str(), itr->second.size());
		sql.append(buf.data(), ret).append(", ");
	}
	sql = sql.substr(0, sql.size() - 2);

	sql.append(")").append("ENGINE=InnoDB DEFAULT CHARSET=utf8");

	std::cout << sql << std::endl;

	if (mysql_real_query(m_mysql, sql.c_str(), sql.size()) != 0)
	{
		printf("mysql_real_query failed: error %u ( %s )",
			mysql_errno(m_mysql), mysql_error(m_mysql));
		return -1;
	}

	return 0;

}

int UsMySql::insert_data(std::vector<std::map<std::string, std::string>> value)
{
	for (auto itr = value.begin(); itr != value.end(); ++itr)
	{
		std::string sql = "insert into yxmeta(MetaDataFileName, DataFormat, CornerLonRange, MapProjection, CoordinationUnit) values(";

		for (auto it = itr->begin(); it != itr->end(); ++it)
		{
			std::string buf;
			buf.resize(100);
			int ret = mysql_real_escape_string(m_mysql, (char*)buf.data(), it->second.c_str(), it->second.size());
			sql.append("'").append(buf.data(), ret).append("', ");
		}
		sql = sql.substr(0, sql.size() - 2);
		sql.append(")");

		std::cout << "\n" << sql << "\n" << std::endl;
		if (mysql_real_query(m_mysql, sql.c_str(), sql.size()) != 0)
		{
			printf("mysql_real_query failed: error %u ( %s )",
				mysql_errno(m_mysql), mysql_error(m_mysql));
			return -1;
		}
	}

	return 0;
}

int UsMySql::delete_data(std::string CoordinationUnit)
{

}


int mysql_stmt()
{
	MYSQL* pMysql = mysql_init(NULL);
	if (!pMysql) { return -1;}

	/*设置断开重连*/
	int arg = 1;
	mysql_options(pMysql, MYSQL_OPT_RECONNECT, &arg);

	/*连接mysql服务器*/
	if (mysql_real_connect(pMysql, "localhost",
		"root", "123456", "test", 3306, NULL, 0) == NULL){
		printf("mysql_real_connect failed: error %u ( %s )",
			mysql_errno(pMysql), mysql_error(pMysql));
		return -2;
	}

	//创建并返回有一个MYSQL_STMT处理程序
	MYSQL_STMT* pMysqlStmt = mysql_stmt_init(pMysql);
	if (pMysqlStmt == NULL){
		std::cout << "mysql_stmt_init(), out of memory" << std::endl;
		return -3;
	}

	// ? 标记仅在DML中是合法的，在DDL中是不合法的
	std::string sqlStmt = "insert into mytb(`id`, `name`, `age`)values(?, ?, ?)";

	//数据定义语言（DDL）：用于改变数据库结构，包括创建，更改和删除数据库对象
	// 如：create table(创建表)	alter table(修改表)		truncate table(删除表中的数据)	drop table(删除表)
	// truncate和delete区别：truncate快速删除记录并释放空间，不使用事务，无法撤销；
	// delete可以在执行完后使用rollback，撤销删除，因此如果确定数据不用的话，使用truncate效率更高
	//数据操纵语言（DML）：用于检索，插入和修改数据
	// 如：select(查询)， insert(添加)， update(修改)， delete(删除)
	if (mysql_stmt_prepare(pMysqlStmt, sqlStmt.data(), sqlStmt.size()) != 0){
		std::cout << "mysql_stmt_prepare failed(" 
			<< mysql_stmt_error(pMysqlStmt) << ")"<< std::endl;
		return -4;
	}

	//求出标记参数的个数
	if (mysql_stmt_param_count(pMysqlStmt) != 3){
		std::cout << "invalid parameter count returned by MySQL" << std::endl;
		return -5;
	}

	//绑定三个标记参数
	MYSQL_BIND binds[3];
	memset(binds, 0, sizeof(binds));

	int id;
	binds[0].buffer_type = MYSQL_TYPE_LONG;
	binds[0].buffer = &id;
	binds[0].is_null = 0;

	std::string name;
	binds[1].buffer_type = MYSQL_TYPE_STRING;
	binds[1].buffer_length = 255;
	binds[1].buffer = (char*)name.data();
	binds[1].buffer_length = name.size();
	binds[1].is_null = 0;

	int age;
	binds[2].buffer_type = MYSQL_TYPE_LONG;
	binds[2].buffer = &age;
	binds[2].is_null = 0;

	if (!mysql_stmt_bind_param(pMysqlStmt, binds)){
		std::cout << "mysql_stmt_bind_param failed("
			<< mysql_stmt_error(pMysqlStmt) << ")" << std::endl;
		return -6;
	}

	//插入记录
	id = 10;
	name = "kkk";
	age = 39;
	if (mysql_stmt_execute(pMysqlStmt) != 0){
		std::cout << "mysql_stmt_execute failed("
			<< mysql_stmt_error(pMysqlStmt) << ")" << std::endl;
		return -7;
	}

	unsigned long long affectRows = mysql_stmt_affected_rows(pMysqlStmt);
	if (affectRows < 1){
		std::cout << "invalid affected rows" <<  std::endl;
		return -8;
	}

	return affectRows;
}