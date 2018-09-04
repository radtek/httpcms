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
	/*��xml�ж�ȡ���ݣ�ִ��sql����������ݿ���*/
	int addFiledValuesFromXML(const std::string path);

	/**
	* @Brief: ��ѯ.
	*
	* @param: query_parameters�����������ֵ��;
	*		  response_body�����صļ�¼��json��ʽ���.
	*
	* @query_parameters��#fileds������ѯ���ֶμ�ֵ�����ڿ�ģ����ѯ���ֶΣ���ֵ�����ж�����Կո�������Ի��ϵ���в�ѯ,���磺fileds=Producer = ɽ�� ���� �Ϻ���
	*							�������ڻ�����ֵ�����Է�Χ��ѯ ���磺fileds=ProduceDate=[200901, 2018]�� fileds=GroundResolution=[2, 2.4]��
	*							���඼����о�ȷ��ѯ��
	*					 #_polygons����Χ��ѯ������rect������circle�Խ��е���Χ�Ĳ�ѯ
	*								���磺_polygons=rect=[4888200, 364900, 4931300, 607840]��
	*									  _polygons=circle={point=[5000000, 500000] ,r=80000}��
	*					 #from���������з���Ҫ��Ľ�����ӵ�from����¼��ʼ��ȡ��Ĭ��Ϊ0��
	*					 #size����ȡsize����¼��Ĭ��Ϊ100��
	*					 #filter�����˻�ȡ�ֶΣ����磺filter=ProduceDate��
	*
	* @return: 200 �ɹ�, 400 ���������ʽ����	   
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
	* @Brief: ���ӷ�������ѡ�����ݿ�
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
	* @Brief: ����������л�ȡ��Ϊfileds��ֵΪ��ѯ�ֶ��Լ�ֵ�Ĳ���.
	*
	* @param: query_parameters�����������ֵ��;
	*		  fileds����ż�Ϊfileds�ļ�ֵ��.
	*
	* @return: 0 �ɹ�,
	*		   -1 ��ʽ����ȷ, ��ѯ�ֶ�������û�С�=��,
	*		   -2 fileds�ļ�����ѯ�ֶ�����Ϊ��.
	*/
	int getQueryparaFileds(const std::multimap< std::string, std::string > query_parameters, std::multimap< std::string, std::string > &fileds);

	/**
	* @Brief: ����������л�ȡ��Ϊ_polygons����Χ��ѯ���Ĳ��֣���ȡ�����.
	*
	* @param: query_parameters�����������ֵ��;
	*		  rectv�����ÿ��_polygons��������ĸ�����ֵ.
	*
	* @return: 0 �ɹ�,
	*		   -1 ��ʽ����ȷ, rect����circle����û�С�=��,
	*		   -2 _polygons�ļ�Ϊ��,
	*		   -3 rect��ֵ����[...]��ʽ
	*		   -4 rect��ֵ��ʽ����ȷ��[]�е�ֵΪx,y,x,y��û���ö��Ÿ������߶�����������3��
	*		   -5 circleû��Բ��point
	*		   -6 circleû�а뾶r
	*		   -7 point����r����û�еȺ�
	*		   -8 point����û��[]��ʽ��ֵ
	*		   -9 pointֵ�ĸ�ʽ����([x,y])
	*		   -10 circle��ֵ��ʽ����ȷ��circleֵ��ʽΪ{point=[x,y],r=123}����{r=123,point=[x,y]}
	*/
	int getQueryparaPolygons(const std::multimap< std::string, std::string > query_parameters, std::vector<std::vector<double>> &rectv);

	/**
	* @Brief: ����������л�ȡ��Ϊfrom��������Щ��¼���Ĳ���
	*
	* @return: 0��Ĭ��Ϊ0�� ����ֵ��from��ֵ
	*/
	int getFrom(const std::multimap< std::string, std::string > query_parameters);

	/**
	* @Brief: ����������л�ȡ��Ϊsize����ȡ��������¼���Ĳ���
	*
	* @return: 100��Ĭ��Ϊ100�� ����ֵ��size��ֵ
	*/
	int getSize(const std::multimap< std::string, std::string > query_parameters);

	/**
	* @Brief: ����������л�ȡ��Ϊfilters�������У��Ĳ���
	*
	* @return: 0���ɹ�
	*/
	int getFilters(const std::multimap< std::string, std::string > query_parameters, std::vector<std::string> &vct);
};

