#ifndef __US_CMS_HTTP_REQUEST_HANDLER_HPP__
#define __US_CMS_HTTP_REQUEST_HANDLER_HPP__


#pragma once
#include <string>
#include <map>
#include <vector>
#include <set>
#include "mysql.h"


// ���泣�� literal constant
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
	ʹ�ò���˵����

	1. init�ǽ�SQL����MySQL����ʵ������һ��ģ����Ҽ��sql����к��ж��ٸ���������Ȼ���Ӧ�Ĵ���bind���飬������������ݸ�ֵ�����൱�ڶ�ÿ����������ֵ��

	2. set_param_bind���Ƕ�SQL����е��ʺŽ��и�ֵ����һ������indexΪ0���������±����һ����

	3. ִ��execute����ִ��SQL��䣬���ǰ�֮ǰ��ֵ�˵�bind���뵽SQL����У�Ȼ��executeִ�У��������Ƿ�����ݿ����������Ӱ�졣

	4. �����ʱSQL��䲻����ɾ�ģ����ǲ飬��ô���ǾͲ��ǲ���3����execute�˶��ǵ���query��Ҳ���Ȱ�֮ǰ��ֵ�˵�bind���뵽SQL����У�Ȼ��metadata����������ѯ���õ��Ľ������һ��Ԫ���ݣ��������ݵ�ֻ�ǽ�����������б�������ͨ�����Ԫ����Ҫ���������е���Ŀ�����Ծ�num_fields��

	5. Ϊ�������bind��ʹ��set_param_result�������bufferͨ�������ִ�У���õ���ֵ��

	6. ִ��get_result��ȡ�������

	7. ִ��fetch_result�Խ�������������л�ȡ��

	8. ���ر�Ԥ������䣬�ͷ�bind���ڴ棬�ͷŽ������
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