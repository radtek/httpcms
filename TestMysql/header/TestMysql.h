#pragma once
#include <windows.h>
#include "mysql.h"

class Mymysql
{
public:
	Mymysql();
	~Mymysql();

	//opt是要设置的选项，arg是该选项的值
	//用于设置额外的连接选项并影响连接的行为。可以多次调用此函数以设置多个选项
	//必须在mysql_init之后，在mysql_connect之前调用
	void set_mysql_options(mysql_option opt, int arg);

public:
	MYSQL * m_mysql;
};