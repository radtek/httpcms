#include <iostream>
#include "usMysql.h"
#include "SqlHttpInterface.h"
#include "filed.h"
#include "Compenent.h"
#include "queryhttpinterface.h"
#include "pugixml.hpp"
#include "pugiconfig.hpp"

int main7()
{
	//MetadataQuery *metaqury = new MetadataQuery();

	/*for (int i = 2; i < 9; i++)
	{
		char buf[2] = { 0 };
		sprintf_s(buf, "%d", i);
		std::string ss = std::string(buf, strlen(buf));
		std::string s = "GF124099420140601Y" + ss + ".XML";
		std::string path = "E:/ZhangYuxin/元数据管理/"+s;
		int ret;
		ret = metaqury->addFiledValuesFromXML(path);
	}*/
	//std::string sss = "E:/ZhangYuxin/元数据管理/xml1/GF124099420140601Y";
	/*for (int i = 85774; i < 100001; i++)
	{
		std::string buf;
		buf.resize(100);
		char buff[100] = { 0 };
		sprintf_s(buff, "%d", i);
		std::string s2 = std::string(buff, strlen(buff));
		std::string path = sss + s2 + ".XML";

		int ret;
		ret = metaqury->addFiledValuesFromXML(path);

		std::cout << s2 << std::endl;
	}*/

	MetaQuery *metaq = new MetaQuery();
	metaq->connectServer();


	const std::string path;
	const std::multimap< std::string, std::string > headers;
	std::multimap< std::string, std::string > query_parameters;
	const std::map< std::string, std::string > path_parameters;
	const std::vector<uint8_t> request_body;
	const std::string dest_endpoint;
	std::multimap< std::string, std::string > response_headers;
	std::vector<uint8_t> response_body;

	std::string jsonStr = u8"{															\
								\"fileds\":												\
								{														\
									\"Producer\": \"山西 地理 信息\",					\
									\"ProductName\"	: \"地理国情\"	,					\
									\"ProduceDate\" : \"[2009, 20180106]\",             \
									\"GroundResolution\" : \"[2.0, 2.5]\",              \
									\"DataFormat\" : \"Erdas Img\"						\
								},														\
								\"_polygons\":											\
								{														\
									\"rect\": [4888200, 364900, 4931300, 607840],		\
									\"circle\":											\
									{													\
										\"point\":[5000000,500000],						\
										\"r\": 80000									\
									}													\
								},														\
								\"from\": 0,											\
								\"size\":2											    \
						   }";

	//匹配用户输入的模糊查询字符串    ^.*?[\s]*.*?$
	//查询 ^.*?地理.*?陕西.*?北京.*|.*?[地理|陕西|北京].*$

	int ret = parseRequestJson(jsonStr, query_parameters);


	query_parameters.insert(std::pair<std::string, std::string>("_polygons", "rect = [ 4888200 , , 4931300, ]")); //(x_min, y_min, x_max, y_max)
	query_parameters.insert(std::pair<std::string, std::string>("fileds", " GroundResolution = [ 2, 2.4 ]"));
	query_parameters.insert(std::pair<std::string, std::string>("_polygons", " circle = { point= [ 5000000.000000, 500000.000000 ] , r = 80000}"));
	query_parameters.insert(std::pair<std::string, std::string>("fileds", "ProduceDate=[200901, 2018]"));

	std::string s = u8"Producer = 山西 陕西 上海";
	query_parameters.insert(std::pair<std::string, std::string>("fileds", s));

	//query_parameters.insert(std::pair<std::string, std::string>("OblatusRatio", "1/298."));
	//query_parameters.insert(std::pair<std::string, std::string>("OblatusRatio", "1/298"));
	//query_parameters.insert(std::pair<std::string, std::string>("MetaDataFileName", "0140601"));

	query_parameters.insert(std::pair<std::string, std::string>("filter", "Range"));
	/*query_parameters.insert(std::pair<std::string, std::string>("filter", "ProduceDate"));
	query_parameters.insert(std::pair<std::string, std::string>("filter", "GroundResolution"));
	query_parameters.insert(std::pair<std::string, std::string>("filter", "WestMosaicMaxError"));*/

	//query_parameters.insert(std::pair<std::string, std::string>("MultiBandName", "R/G/B/NIR"));

	metaq->metadata_http_get_query(path, headers, query_parameters, path_parameters, request_body, dest_endpoint,
		response_headers, response_body);

	for (auto itr = response_body.begin(); itr != response_body.end(); ++itr)
	{
		std::cout << *itr;
	}

	

	//delete metaqury;
	delete metaq;

	std::cout << "\n\nfinish\n\n";
	
	while (1) {}
	return 0;
}


int main5()
{
	std::string jsonStr = "{															\
								\"fileds\":												\
								{														\
									\"Producer\": \"山西 地理 信息\",					\
									\"ProductName\"	: \"地理国情\"						\
								},														\
								\"_polygons\":											\
								{														\
									\"rect\": [4888200, 564900, 4931300, 607840],		\
									\"circle\":											\
									{													\
										\"point\":[0,0],								\
										\"r\": 4000									\
									}													\
								},														\
								\"from\": 1,											\
								\"size\":100											\
						   }";


	std::multimap< std::string, std::string > query_parameters;
	int ret = parseRequestJson(jsonStr, query_parameters);

	MetaQuery *metaq = new MetaQuery();
	metaq->connectServer();

	delete metaq;

	while (1) {}
	return 0;
}


struct simple_walker : pugi::xml_tree_walker
{
	virtual bool for_each(pugi::xml_node& node)
	{
		int len = depth();
		for (int i = 0; i < len; ++i) std::cout << "  "; // indentation

		std::string sv = node.value();
		if (!sv.empty())
		{
			std::cout << node.type() << ": name='" << node.name() << "', value='" << node.value() << "'\n";
		}
	
		return true; // continue traversal
	}
};

int main()
{
	MetaQuery *metaq = new MetaQuery();
	
	const std::string path;
	const std::multimap< std::string, std::string > headers;
	const std::multimap< std::string, std::string > query_parameters;
	const std::map< std::string, std::string > path_parameters;
	const std::vector<uint8_t> request_body;
	const std::string dest_endpoint;
	std::multimap< std::string, std::string > response_headers;
	std::vector<uint8_t> response_body;
	
	int ret = metaq->us_cms_http_get_search(path, headers, query_parameters, path_parameters, request_body, dest_endpoint, response_headers, response_body);

	delete metaq;
	while (1) {}
	return 0;
}