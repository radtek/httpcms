#ifndef __US_CMS_HTTP_REQUEST_HANDLER_HPP__
#define __US_CMS_HTTP_REQUEST_HANDLER_HPP__


#pragma once
#include <string>
#include <map>
#include <vector>
#include <set>
#include "mysql.h"


// 字面常量 literal constant
#define LITERAL_STRING(s) s,(sizeof(s)-1)
#define LITERAL_STRING_U8(s) ((uint8_t*)(s)),(sizeof(s)-1)

inline std::vector<uint8_t>& bytes_append(std::vector<uint8_t>& buffer, uint8_t* data, size_t bytenum)
{
	buffer.insert(buffer.end(), data, data + bytenum);
	return buffer;
}

inline std::vector<uint8_t>& bytes_append(std::vector<uint8_t>& buffer, const std::string& str)
{
	buffer.insert(buffer.end(), (uint8_t*)str.data(), (uint8_t*)(str.data() + str.size()));
	return buffer;
}

int us_cms_http_get_search(
	const std::string& path,
	const std::multimap< std::string, std::string >& headers,
	const std::multimap< std::string, std::string >& query_parameters,
	const std::map< std::string, std::string >& path_parameters,
	const std::vector<uint8_t>& request_body,
	const std::string& dest_endpoint,
	std::multimap< std::string, std::string >& response_headers,
	std::vector<uint8_t>& response_body);


int us_cms_http_post_rebuild(
	const std::string& path,
	const std::multimap< std::string, std::string >& headers,
	const std::multimap< std::string, std::string >& query_parameters,
	const std::map< std::string, std::string >& path_parameters,
	const std::vector<uint8_t>& request_body,
	const std::string& dest_endpoint,
	std::multimap< std::string, std::string >& response_headers,
	std::vector<uint8_t>& response_body);

int us_cms_http_get_getm(
	const std::string& path,
	const std::multimap< std::string, std::string >& headers,
	const std::multimap< std::string, std::string >& query_parameters,
	const std::map< std::string, std::string >& path_parameters,
	const std::vector<uint8_t>& request_body,
	const std::string& dest_endpoint,
	std::multimap< std::string, std::string >& response_headers,
	std::vector<uint8_t>& response_body);

int us_cms_http_post_refresh(
	const std::string& path,
	const std::multimap< std::string, std::string >& headers,
	const std::multimap< std::string, std::string >& query_parameters,
	const std::map< std::string, std::string >& path_parameters,
	const std::vector<uint8_t>& request_body,
	const std::string& dest_endpoint,
	std::multimap< std::string, std::string >& response_headers,
	std::vector<uint8_t>& response_body);


int create();
int create_chufaqi();

int us_cms_http_post_rebuild_addthread(
	const std::string& path,
	const std::multimap< std::string, std::string >& headers,
	const std::multimap< std::string, std::string >& query_parameters,
	const std::map< std::string, std::string >& path_parameters,
	const std::vector<uint8_t>& request_body,
	const std::string& dest_endpoint,
	std::multimap< std::string, std::string >& response_headers,
	std::vector<uint8_t>& response_body);



int test_mysql_stmt();


class pre_statement
{
	/*
	使用步骤说明：

	1. init是将SQL语句和MySQL连接实例绑定在一起的，并且检测sql语句中含有多少个“？”，然后对应的创建bind数组，后面对数组内容赋值，就相当于对每个“？”赋值。

	2. set_param_bind就是对SQL语句中的问号进行赋值，第一个索引index为0，和数组下标规则一样。

	3. 执行execute就是执行SQL语句，先是把之前赋值了的bind带入到SQL语句中，然后execute执行，最后会检测是否对数据库行数造成了影响。

	4. 如果此时SQL语句不是增删改，而是查，那么我们就不是步骤3调用execute了而是调用query，也是先把之前赋值了的bind带入到SQL语句中，然后metadata会计算这个查询语句得到的结果集的一个元数据（不带数据的只是结果集的属性列表），我们通过这个元数据要计算结果集列的数目，所以就num_fields。

	5. 为结果集绑定bind，使用set_param_result，这里的buffer通过后面的执行，会得到列值。

	6. 执行get_result获取结果集。

	7. 执行fetch_result对结果集逐行来进行获取。

	8. 最后关闭预处理语句，释放bind的内存，释放结果集。
	*/

public:
	pre_statement();
	~pre_statement();

	int init(MYSQL* mysql, std::string& sql);
	int set_param_bind(int index, int& value);
	int set_param_bind(int index, std::string& value);
	int set_param_bind(int index, MYSQL_TIME& value);
	int execute();

	int query();
	int set_param_result(int index, enum_field_types type, char* buffer, unsigned long buffer_length, unsigned long	*length);
	int get_result();
	int fetch_result();
	void free();

private:
	MYSQL_STMT* m_stmt;
	MYSQL_BIND* m_param_bind;
	MYSQL_BIND* m_result_bind;
	MYSQL_RES* m_result;
	unsigned long m_param_count;
	unsigned int m_result_count;
};


int ngcc_create_tb_trigger(std::string host, int port, 
	std::string user, std::string password, std::string database);

#endif  // !__US_CMS_HTTP_REQUEST_HANDLER_HPP__