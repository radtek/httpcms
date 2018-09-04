#pragma once
#include <string>
#include <map>
#include <vector>
#include "usMysql.h"
#include "filed.h"

#include "custom_metadata_process.hpp"

class MetaQuery
{
public:
	MetaQuery();
	~MetaQuery();

public:
	/*从xml中读取数据，执行sql语句存放在数据库中*/
	int addFiledValuesFromXML(const std::string path);

	/**
	* @Brief: 查询.
	*
	* @param: query_parameters：请求参数键值对;
	*		  response_body：返回的记录以json格式存放.
	*
	* @query_parameters：#fileds：待查询的字段及值，对于可模糊查询的字段，其值可以有多个并以空格隔开，以或关系进行查询,例如：fileds=Producer = 山西 陕西 上海；
	*							对于日期或者数值，可以范围查询 例如：fileds=ProduceDate=[200901, 2018]， fileds=GroundResolution=[2, 2.4]；
	*							其余都会进行精确查询。
	*					 #_polygons：范围查询，可用rect，或者circle对进行地理范围的查询
	*								例如：_polygons=rect=[4888200, 364900, 4931300, 607840]；
	*									  _polygons=circle={point=[5000000, 500000] ,r=80000}；
	*					 #from：对于所有符合要求的结果，从第from条记录开始获取，默认为0。
	*					 #size：获取size条记录，默认为100。
	*					 #filter：过滤获取字段，例如：filter=ProduceDate。
	*
	* @return: 200 成功, 400 请求参数格式有误。	   
	*/
	int metadata_http_get_query(
		const std::string& path,
		const std::multimap< std::string, std::string >& headers,
		const std::multimap< std::string, std::string >& query_parameters,
		const std::map< std::string, std::string >& path_parameters,
		const std::vector<uint8_t>& request_body,
		const std::string& dest_endpoint,
		std::multimap< std::string, std::string >& response_headers,
		std::vector<uint8_t>& response_body);

	int us_cms_http_get_search(
		const std::string& path,
		const std::multimap< std::string, std::string >& headers,
		const std::multimap< std::string, std::string >& query_parameters,
		const std::map< std::string, std::string >& path_parameters,
		const std::vector<uint8_t>& request_body,
		const std::string& dest_endpoint,
		std::multimap< std::string, std::string >& response_headers,
		std::vector<uint8_t>& response_body);


	/**
	*
	* @Brief: 连接服务器并选择数据库
	*
	*	hostip = "localhost";
	*	port = 3306;/
	*	user = "root";
	*	db = "metadata";
	*/
	int connectServer();

private:
	UsMySql * m_usMysql;
	Filed *m_filed;

private:
	/**
	* @Brief: 从请求参数中获取键为fileds，值为查询字段以及值的部分.
	*
	* @param: query_parameters：请求参数键值对;
	*		  fileds：存放键为fileds的键值对.
	*
	* @return: 0 成功,
	*		   -1 格式不正确, 查询字段名后面没有‘=’,
	*		   -2 fileds的键（查询字段名）为空.
	*/
	int getQueryparaFileds(const std::multimap< std::string, std::string > query_parameters, std::multimap< std::string, std::string > &fileds);

	/**
	* @Brief: 从请求参数中获取键为_polygons（范围查询）的部分，获取外包框.
	*
	* @param: query_parameters：请求参数键值对;
	*		  rectv：存放每个_polygons的外包框四个坐标值.
	*
	* @return: 0 成功,
	*		   -1 格式不正确, rect或者circle后面没有‘=’,
	*		   -2 _polygons的键为空,
	*		   -3 rect的值不是[...]形式
	*		   -4 rect的值格式不正确，[]中的值为x,y,x,y，没有用逗号隔开或者逗号数量少于3个
	*		   -5 circle没有圆心point
	*		   -6 circle没有半径r
	*		   -7 point或者r后面没有等号
	*		   -8 point后面没有[]格式的值
	*		   -9 point值的格式错误([x,y])
	*		   -10 circle的值格式不正确，circle值格式为{point=[x,y],r=123}或者{r=123,point=[x,y]}
	*/
	int getQueryparaPolygons(const std::multimap< std::string, std::string > query_parameters, std::vector<std::vector<double>> &rectv);

	/**
	* @Brief: 从请求参数中获取键为from（跳过哪些记录）的部分
	*
	* @return: 0，默认为0； 其他值，from的值
	*/
	int getFrom(const std::multimap< std::string, std::string > query_parameters);

	/**
	* @Brief: 从请求参数中获取键为size（获取多少条记录）的部分
	*
	* @return: 100，默认为100； 其他值，size的值
	*/
	int getSize(const std::multimap< std::string, std::string > query_parameters);

	/**
	* @Brief: 从请求参数中获取键为filters（过滤列）的部分
	*
	* @return: 0，成功
	*/
	int getFilters(const std::multimap< std::string, std::string > query_parameters, std::vector<std::string> &vct);
};

