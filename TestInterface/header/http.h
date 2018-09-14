#pragma once
#include <string>
#include <map>
#include <vector>
#include <set>

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

