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
