#pragma once
#include <string>
#include <map>
#include <vector>
#include "usMysql.h"
#include "filed.h"

class MetadataQuery
{
public:
	MetadataQuery();
	~MetadataQuery();

public:
	/*从xml中读取数据，执行sql语句存放在数据库中*/
	int addFiledValuesFromXML(const std::string path);

	/*查询操作
	*
	**/
	int metadata_http_get_query(
		const std::string& path,
		const std::multimap< std::string, std::string >& headers,
		const std::multimap< std::string, std::string >& query_parameters,
		const std::map< std::string, std::string >& path_parameters,
		const std::vector<uint8_t>& request_body,
		const std::string& dest_endpoint,
		std::multimap< std::string, std::string >& response_headers,
		std::vector<uint8_t>& response_body);



private:
	UsMySql *m_usMysql;
	Filed *m_filed;
};

