#include "TestMysql.h"
#include <string>
#include <iostream>

Mymysql::Mymysql()
{
	m_mysql = mysql_init(NULL);  //在程序使用Mysql的最开始必须调用mysql_int()初始化
	if (m_mysql == NULL)
	{
		std::cout << "mysql init failed" << std::endl;
	}
}

Mymysql::~Mymysql()
{
	if (m_mysql != NULL)
	{
		mysql_close(m_mysql);
	}
}

void Mymysql::set_mysql_options(mysql_option opt, int arg)  
{
	mysql_options(m_mysql, opt, (void*)&arg);
}


int main1()
{
	Mymysql *myMysql = new Mymysql();

	bool arg = 1;
	myMysql->set_mysql_options(MYSQL_OPT_RECONNECT, arg);  //设置自动重连，如果发现连接丢失，自动重新连接到服务器

	std::string hostIP = "localhost";
	std::string user = "root";
	std::string password = "";
	std::string database = "e2psg";
	unsigned int port = 3306;
	if (mysql_real_connect(myMysql->m_mysql, hostIP.c_str(), user.c_str(), password.c_str(),
		database.c_str(), port, NULL, 0) == NULL)  //建立与运行的mysql数据库引擎的连接
	{
		printf("Failed to connect to database : Error %u ( %s )",
			mysql_errno(myMysql->m_mysql), mysql_error(myMysql->m_mysql));
		
		mysql_close(myMysql->m_mysql);
		myMysql->m_mysql = NULL;
	}

	//std::string sql("insert into runoob_tb2(runoob_doc, runoob_pers, datetime) values('C++18\n', 'zz', '2016-01-01')");
	
	//std::string sql("insert into runoob_tb2(runoob_doc, runoob_pers, datetime) values('");
	//std::string buffer;

	//int ret = mysql_real_escape_string(myMysql->m_mysql, (char*)buffer.data(), "C++17\n", 6); //创建一个合法的SQL字符串，以便在SQL语句中使用
	//sql.append(buffer.data(), (size_t)ret).append("','");

	//ret = mysql_real_escape_string(myMysql->m_mysql, (char*)buffer.data(), "zz", 2);
	//sql.append(buffer.data(), (size_t)ret).append("','");

	//ret = mysql_real_escape_string(myMysql->m_mysql, (char*)buffer.data(), "2016-01-01", 10);
	//sql.append(buffer.data(), (size_t)ret).append("')");


	/*删除一条记录*/
	//std::string sql = "delete from runoob_tb2 where runoob_doc = 'C++18\n' or runoob_doc = 'C++17\n'";
	/*std::string sql = "delete from runoob_tb2 where runoob_doc = 'C++' limit 1";
	std::cout << sql << std::endl;*/


	//std::string sql = "select * from runoob_tb2 where runoob_doc = 'C'";
	//int rnt = mysql_real_query(myMysql->m_mysql, sql.c_str(), sql.size());
	//if (rnt != 0)
	//{
	//	printf("mysql_real_query failed: error %u ( %s )", 
	//		mysql_errno(myMysql->m_mysql), mysql_error(myMysql->m_mysql));
	//}
	//MYSQL_RES *result = mysql_store_result(myMysql->m_mysql);
	//if (result == NULL)
	//{
	//	printf("mysql_store_result failed: error %u ( %s )",
	//		mysql_errno(myMysql->m_mysql), mysql_error(myMysql->m_mysql));
	//}
	//
	//unsigned long long rows = mysql_num_rows(result); //返回了多少行
	//std::cout << rows << std::endl;

	//MYSQL_ROW rown; //一行数据的类型安全表示

	//unsigned long *row_len;

	//while ((rown = mysql_fetch_row(result)) != NULL)
	//{
	//	row_len = mysql_fetch_lengths(result);  //返回当前行的列的长度
	//	for (int i = 0; i < mysql_num_fields(result); ++i)
	//	{
	//		//std::cout << rown[i] << "\t" ;
	//		std::cout << row_len[i] << "\t";
	//	}
	//	std::cout << "\n";
	//}

	//mysql_free_result(result);
	
	std::cout << "finish" << std::endl;
	if (myMysql != NULL)
	{
		delete myMysql;
	}

	while (1) {}
	return 0;
}
