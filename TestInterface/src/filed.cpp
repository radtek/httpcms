#include "field.h"
#include "../parsexml/pugixml.hpp"
#include <fstream>
#include <iostream>
#include <cassert>
#include <vector>
#include <algorithm>
#include <time.h>
#include <rapidjson/document.h>
#include <rapidjson/writer.h>

#define MAXMAPLEN 200

us_cms_metdata_map metadata_map[MAXMAPLEN];

/*Ԫ����ӳ���*/
static us_cms_metdata_map metadata_map_array[] =
{
	"MaxX"                , 1, 3, 0x30,     //����ΧMaxX
	"MinX"                , 2, 3, 0x30,		//����ΧMinX
	"MaxY"                , 3, 3, 0x30,		//����ΧMaxY
	"MinY"                , 4, 3, 0x30,		//����ΧMinY
	"ImgRange"            , 5, 1, 0x02,     //����Χ((���ϣ����ϣ�����������))
	"MetaDataFileName"    , 6, 1, 0x03,     //Ԫ�����ļ�����
	"ProductName"         , 7, 1, 0x03,		// ��Ʒ����
	"Owner"               , 8, 1, 0x03,		// ��Ʒ��Ȩ��λ��
	"Producer"            , 9, 1, 0x03,		// ��Ʒ������λ��
	"Publisher"           , 10, 1, 0x03,	// ��Ʒ���浥λ��
	"ProduceDate"         , 11, 2, 0x0C,	// ��Ʒ����ʱ��
	"ConfidentialLevel"   , 12, 1, 0x02,	// �ܼ�
	"GroundResolution"    , 13, 3, 0x30,	// ����ֱ���
	"ImgColorModel"       , 14, 1, 0x02,	// Ӱ��ɫ��ģʽ
	"PixelBits"           , 15, 2, 0x10,	// ����λ��
	"ImgSize"             , 16, 3, 0x30,	// ������������С
	"DataFormat"          , 17, 1, 0x02,	// ���ݸ�ʽ
	"LongerRadius"        , 18, 1, 0x02,	// ���򳤰뾶
	"OblatusRatio"        , 19, 1, 0x02,	// �������
	"GeodeticDatum"       , 20, 1, 0x02,	// �����ô�ػ�׼
	"MapProjection"       , 21, 1, 0x02,	// ��ͼͶӰ
	"CentralMederian"     , 22, 2, 0x10,	// ����������
	"ZoneDivisionMode"    , 23, 1, 0x02,	// �ִ���ʽ
	"GaussKrugerZoneNo"   , 24, 2, 0x30,	// ��˹ - ������ͶӰ����
	"CoordinationUnit"    , 25, 1, 0x02,	// ���굥λ
	"HeightSystem"        , 26, 1, 0x02,	// �߳�ϵͳ��
	"HeightDatum"         , 27, 1, 0x02,	// �̻߳�׼
	"SateName"            , 28, 1, 0x02,	// ��������
	"PBandSensorType"     , 29, 1, 0x02,	// ȫɫӰ�񴫸�������
	"SateResolution"      , 30, 3, 0x30,	// ȫɫ����Ӱ��ֱ���
	"PbandOrbitCode"      , 31, 1, 0x02,	// ȫɫ����Ӱ������
	"PbandDate"           , 32, 2, 0x0C,	// ȫɫ����Ӱ���ȡʱ��
	"MultiBandSensorType" , 33, 1, 0x02,	// �����Ӱ�񴫸�������
	"MultiBandNum"        , 34, 2, 0x30,	// ����ײ�������
	"MultiBandName"       , 35, 1, 0x02,	// ����ײ�������
	"MultiBandResolution" , 36, 3, 0x30,	// ���������Ӱ��ֱ���
	"MultiBandOrbitCode"  , 37, 1, 0x02,	// ���������Ӱ������
	"MultiBandDate"       , 38, 2, 0x0C,	// ���������Ӱ���ȡʱ��
	"SateImgQuality"      , 39, 1, 0x03,	// ����Ӱ��������������
	"GridInterval"        , 40, 2, 0x30,	// DEM�������
	"DEMPrecision"        , 41, 1, 0x02,	// DEM�������
	"ControlSource"       , 42, 1, 0x02,	// ����������Դ
	"SateOriXRMS"         , 43, 3, 0x30,	// ���������ƽ�������(X)
	"SateOriYRMS"         , 44, 3, 0x30,	// ���������ƽ�������(Y)
	"SateOriZRMS"         , 45, 3, 0x30,	// ���������߳������
	"ATProducerName"      , 46, 1, 0x02,	// ����������ҵԱ
	"ATCheckerName"       , 47, 1, 0x02,	// ����������Ա
	"ManufactureType"     , 48, 1, 0x02,	// ����������ʽ
	"SteroEditQuality"    , 49, 1, 0x02,	// ����ģ�ͱ༭���
	"OrthoRectifySoftWare", 50, 1, 0x02,	// ����������
	"ResampleMethod"      , 51, 1, 0x02,	// �ز�������
	"OrthoRectifyQuality" , 52, 1, 0x02,	// ��������ܽ�
	"OrthoRectifyName"    , 53, 1, 0x02,	// ���������ҵԱ
	"OrthoCheckName"      , 54, 1, 0x02,	// ����������Ա
	"WestMosaicMaxError"  , 55, 3, 0x30,	// �������ӱ߲�
	"NorthMosaicMaxError" , 56, 3, 0x30,	// �������ӱ߲�
	"EastMosaicMaxError"  , 57, 3, 0x30,	// �������ӱ߲�
	"SouthMosaicMaxError" , 58, 3, 0x30,	// �ϱ����ӱ߲�
	"MosaicQuality"       , 59, 1, 0x02,	// �ӱ���������
	"MosaicProducerName"  , 60, 1, 0x02,	// �ӱ���ҵԱ
	"MosaicCheckerName"   , 61, 1, 0x02,	// �ӱ߼��Ա
	"MultiBRectifyXRMS"   , 62, 3, 0x30,	// �������׼���������(X)
	"MultiBRectifyYRMS"   , 63, 3, 0x30,	// �������׼���������(Y)
	"CheckPointNum"       , 64, 2, 0x30,	// �������
	"CheckRMS"            , 65, 3, 0x30,	// ����ƽ�������
	"CheckMAXErr"         , 66, 3, 0x30,	// ����������
	"ConclusionInstitute" , 67, 1, 0x02,	// Ժ��������
	"InstituteCheckUnit"  , 68, 1, 0x03,	// Ժ����鵥λ
	"InstituteCheckName"  , 69, 1, 0x02,	// Ժ�������
	"InstituteCheckDate"  , 70, 2, 0x0C,	// Ժ�����ʱ��
	"BureauCheckName"     , 71, 1, 0x02,	// �ּ�������
	"BureauCheckUnit"     , 72, 1, 0x03,	// �ּ����յ�λ
	"ConclusionBureau"    , 73, 1, 0x02,	// �ּ��������
	"BureauCheckDate"     , 74, 2, 0x0C		// �ּ�����ʱ��
};

const us_cms_metdata_map * us_get_cms_metadata_map_table(int * out_size)
{
	*out_size = sizeof(metadata_map_array) / sizeof(us_cms_metdata_map);
	return metadata_map_array;
}

int us_read_cms_metadata_record(const std::string & index, std::string * out_json)
{
	if (index.size() < 6) { return -1; }
	std::string xmlpath = index;
	xmlpath.replace(index.size() - 5, 5, "Y.XML");

	pugi::xml_document doc;
	/*����xml�ļ�*/
	pugi::xml_parse_result result = doc.load_file(xmlpath.c_str());
	if (!result)
	{
		out_json->assign("load file failed");
		return -1;
	}

	rapidjson::StringBuffer strbuf;
	strbuf.Reserve(1024);
	rapidjson::Writer<rapidjson::StringBuffer> writer(strbuf);

	/*��ȡxml�ļ���д��json*/
	writer.StartObject();

	/*��Ʒ�����������*/
	pugi::xml_node root = doc.child("Metadatafile").child("BasicDataContent");
	for (pugi::xml_node chnode = root.first_child(); chnode; chnode = chnode.next_sibling())
	{
		std::string chnode_name = chnode.name();

		/*����Χ���ҳ��������������������Сֵ�����������ʹ��*/
		if (_stricmp(chnode_name.c_str(), "ImgRange") == 0)
		{
			std::vector<double> x;  //��ź������ֵ
			std::vector<double> y;  //����������ֵ

			std::string s_imgrange = "";    //�������귶Χ
			std::string s_south_west = "";  //����
			std::string s_south_east = "";  //����
			std::string s_north_east = "";  //����
			std::string s_north_west = "";  //����

			for (pugi::xml_node subcd = chnode.first_child(); subcd; subcd = subcd.next_sibling())
			{
				std::string subcd_name = subcd.name();
				if ((_stricmp(subcd_name.c_str(), "SouthWestAbs") == 0) || (_stricmp(subcd_name.c_str(), "NorthWestAbs") == 0) ||
					(_stricmp(subcd_name.c_str(), "NorthEastAbs") == 0) || (_stricmp(subcd_name.c_str(), "SouthEastAbs") == 0))
				{
					std::string s = "";
					if (!subcd.text().empty())
					{
						s = subcd.text().as_string();
					}
					x.push_back(atof(s.c_str()));
					/*����x0*/
					if (_stricmp(subcd_name.c_str(), "SouthWestAbs") == 0)
					{
						s_south_west.append(s);
					}
					/*����x1*/
					if (_stricmp(subcd_name.c_str(), "SouthEastAbs") == 0)
					{
						s_south_east.append(s);
					}
					/*����x2*/
					if (_stricmp(subcd_name.c_str(), "NorthEastAbs") == 0)
					{
						s_north_east.append(s);
					}
					/*����x3*/
					if (_stricmp(subcd_name.c_str(), "NorthWestAbs") == 0)
					{
						s_north_west.append(s);
					}
				}
				if ((_stricmp(subcd_name.c_str(), "SouthWestOrd") == 0) || (_stricmp(subcd_name.c_str(), "NorthWestOrd") == 0) ||
					(_stricmp(subcd_name.c_str(), "NorthEastOrd") == 0) || (_stricmp(subcd_name.c_str(), "SouthEastOrd") == 0))
				{
					std::string s = "";
					if (!subcd.text().empty())
					{
						s = subcd.text().as_string();
					}
					y.push_back(atof(s.c_str()));
					/*����y0*/
					if (_stricmp(subcd_name.c_str(), "SouthWestOrd") == 0)
					{
						s_south_west.append(" ").append(s);
					}
					/*����y1*/
					if (_stricmp(subcd_name.c_str(), "SouthEastOrd") == 0)
					{
						s_south_east.append(" ").append(s);
					}
					/*����y2*/
					if (_stricmp(subcd_name.c_str(), "NorthEastOrd") == 0)
					{
						s_north_east.append(" ").append(s);
					}
					/*����y3*/
					if (_stricmp(subcd_name.c_str(), "NorthWestOrd") == 0)
					{
						s_north_west.append(" ").append(s);
					}
				}
			}

			/*�������귶Χ��((x0 y0,x1 y1,x2 y2,x3 y3,x0 y0))��ʽ����json*/
			s_imgrange.append("((").append(s_south_west).append(",").append(s_south_east).append(",");
			s_imgrange.append(s_north_east).append(",").append(s_north_west).append(",").append(s_south_west).append("))");

			/*��ȡx��y�������Сֵ*/
			std::vector<double>::iterator x_max_itr = std::max_element(x.begin(), x.end());
			double x_max = *x_max_itr;
			std::vector<double>::iterator x_min_itr = std::min_element(x.begin(), x.end());
			double x_min = *x_min_itr;
			std::vector<double>::iterator y_max_itr = std::max_element(y.begin(), y.end());
			double y_max = *y_max_itr;
			std::vector<double>::iterator y_min_itr = std::min_element(y.begin(), y.end());
			double y_min = *y_min_itr;

			writer.Key("1");
			writer.Double(x_max);

			writer.Key("2");
			writer.Double(x_min);

			writer.Key("3");
			writer.Double(y_max);

			writer.Key("4");
			writer.Double(y_min);

			writer.Key("5");
			writer.String(s_imgrange.c_str());
		}

		/*������Ĳ���*/
		else if (_stricmp(chnode_name.c_str(), "MathFoundation") == 0)
		{
			for (pugi::xml_node subcd = chnode.first_child(); subcd; subcd = subcd.next_sibling())
			{
				std::string subcd_name = subcd.name();
				std::string s = "";
				if (!subcd.text().empty())
				{
					s = subcd.text().as_string();
				}
				/*���������ߺʹ��Ű��������*/
				if (_stricmp(subcd_name.c_str(), "CentralMederian") == 0)   //����������
				{
					writer.Key("22");
					writer.Int64(atoi(s.c_str()));
				}
				else if (_stricmp(subcd_name.c_str(), "GaussKrugerZoneNo") == 0)  //����
				{
					writer.Key("24");
					writer.Int64(atoi(s.c_str()));
				}
				else if (_stricmp(subcd_name.c_str(), "LongerRadius") == 0)  //���򳤰뾶
				{
					writer.Key("18");
					writer.String(s.c_str());
				}
				else if (_stricmp(subcd_name.c_str(), "OblatusRatio") == 0)  //�������
				{
					writer.Key("19");
					writer.String(s.c_str());
				}
				else if (_stricmp(subcd_name.c_str(), "GeodeticDatum") == 0) //�����ô�ػ�׼
				{
					writer.Key("20");
					writer.String(s.c_str());
				}
				else if (_stricmp(subcd_name.c_str(), "MapProjection") == 0)  //��ͼͶӰ
				{
					writer.Key("21");
					writer.String(s.c_str());
				}
				else if (_stricmp(subcd_name.c_str(), "ZoneDivisionMode") == 0)  //�ִ���ʽ
				{
					writer.Key("23");
					writer.String(s.c_str());
				}
				else if (_stricmp(subcd_name.c_str(), "CoordinationUnit") == 0)  //���굥λ
				{
					writer.Key("25");
					writer.String(s.c_str());
				}
				else if (_stricmp(subcd_name.c_str(), "HeightSystem") == 0)  //�߳�ϵͳ��
				{
					writer.Key("26");
					writer.String(s.c_str());
				}
				else if (_stricmp(subcd_name.c_str(), "HeightDatum") == 0) //�̻߳�׼
				{
					writer.Key("27");
					writer.String(s.c_str());
				}
			}
		}

		else
		{
			std::string s = "";
			if (!chnode.text().empty())
			{
				s = chnode.text().as_string();
			}

			/*����ֱ��ʺ�ͼ���С�����������*/
			if (_stricmp(chnode_name.c_str(), "GroundResolution") == 0)  //����ֱ���
			{
				writer.Key("13");
				writer.Double(atof(s.c_str()));
			}
			else if (_stricmp(chnode_name.c_str(), "ImgSize") == 0)  //������������С
			{
				writer.Key("16");
				writer.Double(atof(s.c_str()));
			}
			/*����λ�����������*/
			else if (_stricmp(chnode_name.c_str(), "PixelBits") == 0)  //����λ��
			{
				writer.Key("15");
				writer.Int64(atoi(s.c_str()));
			}
			/*����ת��Ϊutc*/
			else if (_stricmp(chnode_name.c_str(), "ProduceDate") == 0)  //��Ʒ����ʱ��
			{
				time_t tm_t;
				tm t_s;
				t_s.tm_year = atoi(s.substr(0, 4).c_str()) - 1900;
				t_s.tm_mon = atoi(s.substr(5, 2).c_str()) - 1;
				t_s.tm_mday = atoi(s.substr(6, 2).c_str());
				t_s.tm_hour = 0;
				t_s.tm_min = 0;
				t_s.tm_sec = 0;
				tm_t = mktime(&t_s);
				writer.Key("11");
				writer.Int64(tm_t);
			}
			/*�����ֶ��ַ������*/
			else if (_stricmp(chnode_name.c_str(), "MetaDataFileName") == 0) //Ԫ�����ļ�����
			{
				writer.Key("6");
				writer.String(s.c_str());
			}
			else if (_stricmp(chnode_name.c_str(), "ProductName") == 0) //��Ʒ����
			{
				writer.Key("7");
				writer.String(s.c_str());
			}
			else if (_stricmp(chnode_name.c_str(), "Owner") == 0)  //��Ʒ��Ȩ��λ��
			{
				writer.Key("8");
				writer.String(s.c_str());
			}
			else if (_stricmp(chnode_name.c_str(), "Producer") == 0)  //��Ʒ������λ��
			{
				writer.Key("9");
				writer.String(s.c_str());
			}
			else if (_stricmp(chnode_name.c_str(), "Publisher") == 0)  //��Ʒ���浥λ��
			{
				writer.Key("10");
				writer.String(s.c_str());
			}
			else if (_stricmp(chnode_name.c_str(), "ConfidentialLevel") == 0) //�ܼ�
			{
				writer.Key("12");
				writer.String(s.c_str());
			}
			else if (_stricmp(chnode_name.c_str(), "ImgColorModel") == 0)  //Ӱ��ɫ��ģʽ
			{
				writer.Key("14");
				writer.String(s.c_str());
			}
			else if (_stricmp(chnode_name.c_str(), "DataFormat") == 0)  //���ݸ�ʽ
			{
				writer.Key("17");
				writer.String(s.c_str());
			}
		}
	}

	/*����Դ�������*/
	root = doc.child("Metadatafile").child("ImgSource");
	for (pugi::xml_node chnode = root.first_child(); chnode; chnode = chnode.next_sibling())
	{
		std::string chnode_name = chnode.name();
		if ((_stricmp(chnode_name.c_str(), "PanBand") == 0) || (_stricmp(chnode_name.c_str(), "MultiBand") == 0))
		{
			for (pugi::xml_node subcd = chnode.first_child(); subcd; subcd = subcd.next_sibling())
			{
				std::string subcd_name = subcd.name();
				std::string s = "";
				if (!subcd.text().empty())
				{
					s = subcd.text().as_string();
				}

				/*ȫɫ����Ӱ��ֱ��ʺͶ��������Ӱ��ֱ��� �����������*/
				if (_stricmp(subcd_name.c_str(), "SateResolution") == 0)   //ȫɫ����Ӱ��ֱ���
				{
					writer.Key("30");
					writer.Double(atof(s.c_str()));
				}
				else if (_stricmp(subcd_name.c_str(), "MultiBandResolution") == 0)  //���������Ӱ��ֱ���
				{
					writer.Key("36");
					writer.Double(atof(s.c_str()));
				}
				/*����ײ������������������*/
				else if (_stricmp(subcd_name.c_str(), "MultiBandNum") == 0)  //����ײ�������
				{
					writer.Key("34");
					writer.Int64(atoi(s.c_str()));
				}
				/*���� UTCʱ����*/
				else if (_stricmp(subcd_name.c_str(), "PbandDate") == 0)  //ȫɫ����Ӱ���ȡʱ��
				{
					time_t tm_t;
					tm t_s;
					t_s.tm_year = atoi(s.substr(0, 4).c_str()) - 1900;
					t_s.tm_mon = atoi(s.substr(5, 2).c_str()) - 1;
					t_s.tm_mday = atoi(s.substr(6, 2).c_str());
					t_s.tm_hour = 0;
					t_s.tm_min = 0;
					t_s.tm_sec = 0;
					tm_t = mktime(&t_s);
					writer.Key("32");
					writer.Int64(tm_t);
				}
				else if (_stricmp(subcd_name.c_str(), "MultiBandDate") == 0)  //���������Ӱ���ȡʱ��
				{
					time_t tm_t;
					tm t_s;
					t_s.tm_year = atoi(s.substr(0, 4).c_str()) - 1900;
					t_s.tm_mon = atoi(s.substr(5, 2).c_str()) - 1;
					t_s.tm_mday = atoi(s.substr(6, 2).c_str());
					t_s.tm_hour = 0;
					t_s.tm_min = 0;
					t_s.tm_sec = 0;
					tm_t = mktime(&t_s);
					writer.Key("38");
					writer.Int64(tm_t);
				}
				/*�����ֶ��ַ�����ʽ���*/
				else if (_stricmp(subcd_name.c_str(), "PBandSensorType") == 0)  //ȫɫӰ�񴫸�������
				{
					writer.Key("29");
					writer.String(s.c_str());
				}
				else if (_stricmp(subcd_name.c_str(), "PbandOrbitCode") == 0)  //ȫɫ����Ӱ������
				{
					writer.Key("31");
					writer.String(s.c_str());
				}
				else if (_stricmp(subcd_name.c_str(), "MultiBandSensorType") == 0)  //�����Ӱ�񴫸�������
				{
					writer.Key("33");
					writer.String(s.c_str());
				}
				else if (_stricmp(subcd_name.c_str(), "MultiBandName") == 0) //����ײ�������
				{
					writer.Key("35");
					writer.String(s.c_str());
				}
				else if (_stricmp(subcd_name.c_str(), "MultiBandOrbitCode") == 0)  //���������Ӱ������
				{
					writer.Key("37");
					writer.String(s.c_str());
				}
			}
		}
		else if (_stricmp(chnode_name.c_str(), "SateName") == 0) //��������
		{
			std::string s = "";
			if (!chnode.text().empty())
			{
				s = chnode.text().as_string();
			}
			writer.Key("28");
			writer.String(s.c_str());
		}
		else if (_stricmp(chnode_name.c_str(), "SateImgQuality") == 0) //����Ӱ��������������
		{
			std::string s = "";
			if (!chnode.text().empty())
			{
				s = chnode.text().as_string();
			}
			writer.Key("39");
			writer.String(s.c_str());
		}
	}

	/*��������������Ϣ����*/
	root = doc.child("Metadatafile").child("ProduceInfomation");
	for (pugi::xml_node chnode = root.first_child(); chnode; chnode = chnode.next_sibling())
	{
		std::string chnode_name = chnode.name();
		if ((_stricmp(chnode_name.c_str(), "ImgOrientation") == 0) || (_stricmp(chnode_name.c_str(), "MosaicInfo") == 0) ||
			(_stricmp(chnode_name.c_str(), "QualityCheckInfo") == 0))
		{
			for (pugi::xml_node subcd = chnode.first_child(); subcd; subcd = subcd.next_sibling())
			{
				std::string subcd_name = subcd.name();
				std::string s = "";
				if (!subcd.text().empty())
				{
					s = subcd.text().as_string();
				}

				/*��������ŵ��ֶ� ���� ��� �ӱ߲�*/
				if (_stricmp(subcd_name.c_str(), "SateOriXRMS") == 0) //���������ƽ�������(X)
				{
					writer.Key("43");
					writer.Double(atof(s.c_str()));
				}
				else if (_stricmp(subcd_name.c_str(), "SateOriYRMS") == 0)  //���������ƽ�������(Y)
				{
					writer.Key("44");
					writer.Double(atof(s.c_str()));
				}
				else if (_stricmp(subcd_name.c_str(), "SateOriZRMS") == 0)  //���������߳������(Z)
				{
					writer.Key("45");
					writer.Double(atof(s.c_str()));
				}
				else if (_stricmp(subcd_name.c_str(), "WestMosaicMaxError") == 0)  //�������ӱ߲�
				{
					writer.Key("55");
					writer.Double(atof(s.c_str()));
				}
				else if (_stricmp(subcd_name.c_str(), "NorthMosaicMaxError") == 0)  //�������ӱ߲�
				{
					writer.Key("56");
					writer.Double(atof(s.c_str()));
				}
				else if (_stricmp(subcd_name.c_str(), "EastMosaicMaxError") == 0)  //�������ӱ߲�
				{
					writer.Key("57");
					writer.Double(atof(s.c_str()));
				}
				else if (_stricmp(subcd_name.c_str(), "SouthMosaicMaxError") == 0)  //�ϱ����ӱ߲�
				{
					writer.Key("58");
					writer.Double(atof(s.c_str()));
				}
				else if (_stricmp(subcd_name.c_str(), "MultiBRectifyXRMS") == 0)  //�������׼���������(X)
				{
					writer.Key("62");
					writer.Double(atof(s.c_str()));
				}
				else if (_stricmp(subcd_name.c_str(), "MultiBRectifyYRMS") == 0)  //�������׼���������(Y)
				{
					writer.Key("63");
					writer.Double(atof(s.c_str()));
				}
				else if (_stricmp(subcd_name.c_str(), "CheckPointNum") == 0)  //�������
				{
					writer.Key("64");
					writer.Double(atof(s.c_str()));
				}
				else if (_stricmp(subcd_name.c_str(), "CheckRMS") == 0)  //����ƽ�������
				{
					writer.Key("65");
					writer.Double(atof(s.c_str()));
				}
				else if (_stricmp(subcd_name.c_str(), "CheckMAXErr") == 0)  //����������
				{
					writer.Key("66");
					writer.Double(atof(s.c_str()));
				}
				/*Ժ�����ʱ��;ּ�����ʱ����ΪUTCʱ��*/
				else if (_stricmp(subcd_name.c_str(), "InstituteCheckDate") == 0)  //Ժ�����ʱ��
				{
					time_t tm_t;
					tm t_s;
					t_s.tm_year = atoi(s.substr(0, 4).c_str()) - 1900;
					t_s.tm_mon = atoi(s.substr(5, 2).c_str()) - 1;
					t_s.tm_mday = atoi(s.substr(6, 2).c_str());
					t_s.tm_hour = 0;
					t_s.tm_min = 0;
					t_s.tm_sec = 0;
					tm_t = mktime(&t_s);
					writer.Key("70");
					writer.Int64(tm_t);
				}
				else if (_stricmp(subcd_name.c_str(), "BureauCheckDate") == 0)  //�ּ�����ʱ��
				{
					time_t tm_t;
					tm t_s;
					t_s.tm_year = atoi(s.substr(0, 4).c_str()) - 1900;
					t_s.tm_mon = atoi(s.substr(5, 2).c_str()) - 1;
					t_s.tm_mday = atoi(s.substr(6, 2).c_str());
					t_s.tm_hour = 0;
					t_s.tm_min = 0;
					t_s.tm_sec = 0;
					tm_t = mktime(&t_s);
					writer.Key("74");
					writer.Int64(tm_t);
				}
				/*�������ַ������*/
				else if (_stricmp(subcd_name.c_str(), "ATProducerName") == 0)  //����������ҵԱ
				{
					writer.Key("46");
					writer.String(s.c_str());
				}
				else if (_stricmp(subcd_name.c_str(), "ATCheckerName") == 0)  //����������Ա
				{
					writer.Key("47");
					writer.String(s.c_str());
				}
				else if (_stricmp(subcd_name.c_str(), "MosaicQuality") == 0)  //�ӱ���������
				{
					writer.Key("59");
					writer.String(s.c_str());
				}
				else if (_stricmp(subcd_name.c_str(), "MosaicProducerName") == 0)  //�ӱ���ҵԱ
				{
					writer.Key("60");
					writer.String(s.c_str());
				}
				else if (_stricmp(subcd_name.c_str(), "MosaicCheckerName") == 0)  //�ӱ߼��Ա
				{
					writer.Key("61");
					writer.String(s.c_str());
				}
				else if (_stricmp(subcd_name.c_str(), "ConclusionInstitute") == 0)  //Ժ��������
				{
					writer.Key("67");
					writer.String(s.c_str());
				}
				else if (_stricmp(subcd_name.c_str(), "InstituteCheckUnit") == 0)  //Ժ����鵥λ
				{
					writer.Key("68");
					writer.String(s.c_str());
				}
				else if (_stricmp(subcd_name.c_str(), "InstituteCheckName") == 0)  //Ժ�������
				{
					writer.Key("69");
					writer.String(s.c_str());
				}
				else if (_stricmp(subcd_name.c_str(), "BureauCheckName") == 0)  //�ּ�������
				{
					writer.Key("71");
					writer.String(s.c_str());
				}
				else if (_stricmp(subcd_name.c_str(), "BureauCheckUnit") == 0)  //�ּ����յ�λ
				{
					writer.Key("72");
					writer.String(s.c_str());
				}
				else if (_stricmp(subcd_name.c_str(), "ConclusionBureau") == 0)  //�ּ��������
				{
					writer.Key("73");
					writer.String(s.c_str());
				}
			}
		}
		else
		{
			std::string s = "";
			if (!chnode.text().empty())
			{
				s = chnode.text().as_string();
			}

			/*DEM������ఴ�������*/
			if (_stricmp(chnode_name.c_str(), "GridInterval") == 0)  //DEM�������
			{
				writer.Key("40");
				writer.Int64(atoi(s.c_str()));
			}
			else if (_stricmp(chnode_name.c_str(), "DEMPrecision") == 0) //DEM�������
			{
				writer.Key("41");
				writer.String(s.c_str());
			}
			else if (_stricmp(chnode_name.c_str(), "ControlSource") == 0) //����������Դ
			{
				writer.Key("42");
				writer.String(s.c_str());
			}
			else if (_stricmp(chnode_name.c_str(), "ManufactureType") == 0) //����������ʽ
			{
				writer.Key("48");
				writer.String(s.c_str());
			}
			else if (_stricmp(chnode_name.c_str(), "SteroEditQuality") == 0) //����ģ�ͱ༭���
			{
				writer.Key("49");
				writer.String(s.c_str());
			}
			else if (_stricmp(chnode_name.c_str(), "OrthoRectifySoftware") == 0)  //����������
			{
				writer.Key("50");
				writer.String(s.c_str());
			}
			else if (_stricmp(chnode_name.c_str(), "ResampleMethod") == 0) //�ز�������
			{
				writer.Key("51");
				writer.String(s.c_str());
			}
			else if (_stricmp(chnode_name.c_str(), "OrthoRectifyQuality") == 0) //��������ܽ�
			{
				writer.Key("52");
				writer.String(s.c_str());
			}
			else if (_stricmp(chnode_name.c_str(), "OrthoRectifyName") == 0) //���������ҵԱ
			{
				writer.Key("53");
				writer.String(s.c_str());
			}
			else if (_stricmp(chnode_name.c_str(), "OrthoCheckName") == 0) //����������Ա
			{
				writer.Key("54");
				writer.String(s.c_str());
			}
		}
	}

	writer.EndObject();

	out_json->assign(strbuf.GetString(), strbuf.GetSize());

	return 0;
}






/*֮ǰд��*/
const us_cms_metdata_map * us_get_cms_metadata_map_table1(int * out_size)
{
	std::string path = "./ngcc_metadata_table.txt";

	std::ifstream infile;
	infile.open(path.data());   //���ļ����������ļ��������� 
	assert(infile.is_open());   //��ʧ��,�����������Ϣ,����ֹ�������� 

	int rows = 0;
	std::string s;
	uint32_t mode = 0;
	while (getline(infile, s))
	{
		if (!s.empty())
		{
			if (rows < MAXMAPLEN)
			{
				size_t pos = s.find_first_of(", ");
				metadata_map[rows].m_custon_field = s.substr(0, pos);
				s = s.substr(pos + 2);
				pos = s.find_first_of(", ");
				metadata_map[rows].m_db_field = atoi(s.substr(0, pos).c_str());
				s = s.substr(pos + 2);
				pos = s.find_first_of(", ");
				metadata_map[rows].m_type = atoi(s.substr(0, pos).c_str());
				s = s.substr(pos + 2);
				sscanf_s(s.c_str(), "%x", &mode);
				metadata_map[rows].m_mode = mode;
				rows++;
			}
		}
	}
	if (rows > 0)
	{
		*out_size = rows;
		return metadata_map;
	}

	infile.close();             //�ر��ļ������� 

	return nullptr;
}

int us_read_cms_metadata_record1(const std::string & index, std::string * out_json)
{
	pugi::xml_document doc;
	pugi::xml_parse_result result = doc.load_file(index.c_str());  //����xml�ļ�
	if (!result)
	{
		out_json->assign("load file failed");
		return -1;
	}

	rapidjson::StringBuffer strbuf;
	strbuf.Reserve(1024);
	rapidjson::Writer<rapidjson::StringBuffer> writer(strbuf);

	writer.StartObject();

	int db_filed_num = 1;
	pugi::xml_node root = doc.child("Metadatafile").child("BasicDataContent");
	for (pugi::xml_node chnode = root.first_child(); chnode; chnode = chnode.next_sibling())
	{
		char buf[10] = { 0 };
		sprintf_s(buf, "%d", db_filed_num);
		std::string key = std::string(buf, strlen(buf));

		std::string chnode_name = chnode.name();

		if (chnode_name == "ImgRange")
		{
			std::vector<double> x;
			std::vector<double> y;

			std::string s_imgrange;
			for (pugi::xml_node subcd = chnode.first_child(); subcd; subcd = subcd.next_sibling())
			{
				std::string subcd_name = subcd.name();
				if (subcd_name == "SouthWestAbs" || subcd_name == "NorthWestAbs" ||
					subcd_name == "NorthEastAbs" || subcd_name == "SouthEastAbs")
				{
					std::string s = subcd.text().as_string();
					x.push_back(atof(s.c_str()));
					s_imgrange.append(subcd_name).append(":").append(s).append(", ");
				}
				if (subcd_name == "SouthWestOrd" || subcd_name == "NorthWestOrd" ||
					subcd_name == "NorthEastOrd" || subcd_name == "SouthEastOrd")
				{
					std::string s = subcd.text().as_string();
					y.push_back(atof(s.c_str()));
					s_imgrange.append(subcd_name).append(":").append(s).append(", ");
				}
			}

			std::vector<double>::iterator x_max_itr = std::max_element(x.begin(), x.end());
			double x_max = *x_max_itr;
			std::vector<double>::iterator x_min_itr = std::min_element(x.begin(), x.end());
			double x_min = *x_min_itr;
			std::vector<double>::iterator y_max_itr = std::max_element(y.begin(), y.end());
			double y_max = *y_max_itr;
			std::vector<double>::iterator y_min_itr = std::min_element(y.begin(), y.end());
			double y_min = *y_min_itr;

			char buf_[10] = { 0 };
			sprintf_s(buf_, "%d", db_filed_num);
			std::string key_ = std::string(buf_, strlen(buf_));
			writer.Key(key.c_str());
			writer.Double(x_max);
			db_filed_num++;

			memset(buf_, 0, sizeof(buf_));
			sprintf_s(buf_, "%d", db_filed_num);
			key_ = std::string(buf_, strlen(buf_));
			writer.Key(key_.c_str());
			writer.Double(x_min);
			db_filed_num++;

			memset(buf_, 0, sizeof(buf_));
			sprintf_s(buf_, "%d", db_filed_num);
			key_ = std::string(buf_, strlen(buf_));
			writer.Key(key_.c_str());
			writer.Double(y_max);
			db_filed_num++;

			memset(buf_, 0, sizeof(buf_));
			sprintf_s(buf_, "%d", db_filed_num);
			key_ = std::string(buf_, strlen(buf_));
			writer.Key(key_.c_str());
			writer.Double(y_min);
			db_filed_num++;

			memset(buf_, 0, sizeof(buf_));
			sprintf_s(buf_, "%d", db_filed_num);
			key_ = std::string(buf_, strlen(buf_));
			s_imgrange = s_imgrange.substr(0, s_imgrange.size() - 2);
			writer.Key(key_.c_str());
			writer.String(s_imgrange.c_str());
			db_filed_num++;
		}
		else if (chnode_name == "MathFoundation")
		{
			for (pugi::xml_node subcd = chnode.first_child(); subcd; subcd = subcd.next_sibling())
			{
				std::string subcd_name = subcd.name();
				std::string s = subcd.text().as_string();
				if (subcd_name == "CentralMederian" || subcd_name == "GaussKrugerZoneNo")
				{
					char buf_[10] = { 0 };
					sprintf_s(buf_, "%d", db_filed_num);
					std::string key_ = std::string(buf_, strlen(buf_));
					writer.Key(key_.c_str());
					writer.Double(atof(s.c_str()));
					db_filed_num++;
				}
				else
				{
					char buf_[10] = { 0 };
					sprintf_s(buf_, "%d", db_filed_num);
					std::string key_ = std::string(buf_, strlen(buf_));
					writer.Key(key_.c_str());
					writer.String(s.c_str());
					db_filed_num++;
				}
			}
		}
		else
		{
			std::string s = chnode.text().as_string();
			if (chnode_name == "GroundResolution" || chnode_name == "ImgSize")
			{
				writer.Key(key.c_str());
				writer.Double(atof(s.c_str()));
				db_filed_num++;
			}
			else if (chnode_name == "PixelBits")
			{
				writer.Key(key.c_str());
				writer.Int(atoi(s.c_str()));
				db_filed_num++;
			}
			else if (chnode_name == "ProduceDate")
			{
				time_t tm_t;
				tm t_s;
				t_s.tm_year = atoi(s.substr(0, 4).c_str()) - 1900;
				t_s.tm_mon = atoi(s.substr(5, 2).c_str()) - 1;
				t_s.tm_mday = atoi(s.substr(6, 2).c_str());
				t_s.tm_hour = 0;
				t_s.tm_min = 0;
				t_s.tm_sec = 0;
				tm_t = mktime(&t_s);
				writer.Key(key.c_str());
				writer.Int64(tm_t);
				db_filed_num++;
			}
			else
			{
				writer.Key(key.c_str());
				writer.String(s.c_str());
				db_filed_num++;
			}
		}
	}

	root = doc.child("Metadatafile").child("ImgSource");
	for (pugi::xml_node chnode = root.first_child(); chnode; chnode = chnode.next_sibling())
	{
		std::string chnode_name = chnode.name();
		if (chnode_name == "PanBand" || chnode_name == "MultiBand")
		{
			for (pugi::xml_node subcd = chnode.first_child(); subcd; subcd = subcd.next_sibling())
			{
				std::string subcd_name = subcd.name();
				std::string s = subcd.text().as_string();

				char buf[10] = { 0 };
				sprintf_s(buf, "%d", db_filed_num);
				std::string key = std::string(buf, strlen(buf));

				if (subcd_name == "SateResolution" || subcd_name == "MultiBandResolution")
				{
					writer.Key(key.c_str());
					writer.Double(atof(s.c_str()));
					db_filed_num++;
					//out_json->append("\"").append(subcd_name).append("\": ").append(s).append(", ");
				}
				else if (subcd_name == "MultiBandNum")
				{
					writer.Key(key.c_str());
					writer.Int(atoi(s.c_str()));
					db_filed_num++;
					//out_json->append("\"").append(chnode_name).append("\": \"").append(s).append("\", ");
				}
				else if (subcd_name == "PbandDate" || subcd_name == "PBandDate" || subcd_name == "MultiBandDate")
				{
					time_t tm_t;
					tm t_s;
					t_s.tm_year = atoi(s.substr(0, 4).c_str()) - 1900;
					t_s.tm_mon = atoi(s.substr(5, 2).c_str()) - 1;
					t_s.tm_mday = atoi(s.substr(6, 2).c_str());
					t_s.tm_hour = 0;
					t_s.tm_min = 0;
					t_s.tm_sec = 0;
					tm_t = mktime(&t_s);
					writer.Key(key.c_str());
					writer.Int64(tm_t);
					db_filed_num++;
				}
				else
				{
					writer.Key(key.c_str());
					writer.String(s.c_str());
					db_filed_num++;
				}
			}
		}
		else
		{
			char buf[10] = { 0 };
			sprintf_s(buf, "%d", db_filed_num);
			std::string key = std::string(buf, strlen(buf));
			std::string s = chnode.text().as_string();
			writer.Key(key.c_str());
			writer.String(s.c_str());
			db_filed_num++;
			//out_json->append("\"").append(chnode_name).append("\": \"").append(s).append("\", ");
		}
	}

	root = doc.child("Metadatafile").child("ProduceInfomation");
	for (pugi::xml_node chnode = root.first_child(); chnode; chnode = chnode.next_sibling())
	{
		std::string chnode_name = chnode.name();
		if (chnode_name == "ImgOrientation" || chnode_name == "MosaicInfo" || chnode_name == "QualityCheckInfo")
		{
			for (pugi::xml_node subcd = chnode.first_child(); subcd; subcd = subcd.next_sibling())
			{
				std::string subcd_name = subcd.name();
				std::string s = subcd.text().as_string();

				char buf[10] = { 0 };
				sprintf_s(buf, "%d", db_filed_num);
				std::string key = std::string(buf, strlen(buf));

				if (subcd_name == "SateOriXRMS" || subcd_name == "SateOriYRMS" || subcd_name == "SateOriZRMS" ||
					subcd_name == "WestMosaicMaxError" || subcd_name == "NorthMosaicMaxError" ||
					subcd_name == "EastMosaicMaxError" || subcd_name == "SouthMosaicMaxError" ||
					subcd_name == "MultiBRectifyXRMS" || subcd_name == "MultiBRectifyYRMS" ||
					subcd_name == "CheckPointNum" || subcd_name == "CheckRMS" ||
					subcd_name == "CheckMAXErr")
				{
					writer.Key(key.c_str());
					writer.Double(atof(s.c_str()));
					db_filed_num++;
					//out_json->append("\"").append(subcd_name).append("\": ").append(s).append(", ");
				}
				else if (subcd_name == "InstituteCheckDate" || subcd_name == "BureauCheckDate")
				{
					time_t tm_t;
					tm t_s;
					t_s.tm_year = atoi(s.substr(0, 4).c_str()) - 1900;
					t_s.tm_mon = atoi(s.substr(5, 2).c_str()) - 1;
					t_s.tm_mday = atoi(s.substr(6, 2).c_str());
					t_s.tm_hour = 0;
					t_s.tm_min = 0;
					t_s.tm_sec = 0;
					tm_t = mktime(&t_s);
					writer.Key(key.c_str());
					writer.Int64(tm_t);
					db_filed_num++;
					//out_json->append("\"").append(subcd_name).append("\": \"").append(s).append("\", ");
				}
				else
				{
					writer.Key(key.c_str());
					writer.String(s.c_str());
					db_filed_num++;
				}
			}
		}
		else
		{
			std::string s = chnode.text().as_string();
			if (chnode_name == "GridInterval")
			{
				char buf[10] = { 0 };
				sprintf_s(buf, "%d", db_filed_num);
				std::string key = std::string(buf, strlen(buf));
				std::string s = chnode.text().as_string();
				writer.Key(key.c_str());
				writer.Int(atoi(s.c_str()));
				db_filed_num++;
				//out_json->append("\"").append(chnode_name).append("\": ").append(s).append(", ");
			}
			else
			{
				char buf[10] = { 0 };
				sprintf_s(buf, "%d", db_filed_num);
				std::string key = std::string(buf, strlen(buf));
				std::string s = chnode.text().as_string();
				writer.Key(key.c_str());
				writer.String(s.c_str());
				db_filed_num++;
				//out_json->append("\"").append(chnode_name).append("\": \"").append(s).append("\", ");
			}
		}
	}

	writer.EndObject();

	out_json->assign(strbuf.GetString(), strbuf.GetSize());

	return 0;
}

/*�ڶ���*/
int us_read_cms_metadata_record2(const std::string & index, std::string * out_json)
{
	pugi::xml_document doc;
	/*����xml�ļ�*/
	pugi::xml_parse_result result = doc.load_file(index.c_str());
	if (!result)
	{
		out_json->assign("load file failed");
		return -1;
	}

	rapidjson::StringBuffer strbuf;
	strbuf.Reserve(1024);
	rapidjson::Writer<rapidjson::StringBuffer> writer(strbuf);

	/*��ȡxml�ļ���д��json*/
	writer.StartObject();

	int db_filed_num = 1;  //����json�ļ�������ҪתΪ�ַ�������

						   /*��Ʒ�����������*/
	pugi::xml_node root = doc.child("Metadatafile").child("BasicDataContent");
	for (pugi::xml_node chnode = root.first_child(); chnode; chnode = chnode.next_sibling())
	{
		std::string chnode_name = chnode.name();

		/*����Χ���ҳ��������������������Сֵ�����������ʹ��*/
		if (_stricmp(chnode_name.c_str(), "ImgRange") == 0)
		{
			std::vector<double> x;  //��ź������ֵ
			std::vector<double> y;  //����������ֵ

			std::string s_imgrange;  //�������귶Χ
			std::string s_south_west;  //����
			std::string s_south_east;  //����
			std::string s_north_east;  //����
			std::string s_north_west;  //����

			for (pugi::xml_node subcd = chnode.first_child(); subcd; subcd = subcd.next_sibling())
			{
				std::string subcd_name = subcd.name();
				if ((_stricmp(subcd_name.c_str(), "SouthWestAbs") == 0) || (_stricmp(subcd_name.c_str(), "NorthWestAbs") == 0) ||
					(_stricmp(subcd_name.c_str(), "NorthEastAbs") == 0) || (_stricmp(subcd_name.c_str(), "SouthEastAbs") == 0))
				{
					std::string s = subcd.text().as_string();
					x.push_back(atof(s.c_str()));
					/*����x0*/
					if (_stricmp(subcd_name.c_str(), "SouthWestAbs") == 0)
					{
						s_south_west.append(s);
					}
					/*����x1*/
					if (_stricmp(subcd_name.c_str(), "SouthEastAbs") == 0)
					{
						s_south_east.append(s);
					}
					/*����x2*/
					if (_stricmp(subcd_name.c_str(), "NorthEastAbs") == 0)
					{
						s_north_east.append(s);
					}
					/*����x3*/
					if (_stricmp(subcd_name.c_str(), "NorthWestAbs") == 0)
					{
						s_north_west.append(s);
					}
				}
				if ((_stricmp(subcd_name.c_str(), "SouthWestOrd") == 0) || (_stricmp(subcd_name.c_str(), "NorthWestOrd") == 0) ||
					(_stricmp(subcd_name.c_str(), "NorthEastOrd") == 0) || (_stricmp(subcd_name.c_str(), "SouthEastOrd") == 0))
				{
					std::string s = subcd.text().as_string();
					y.push_back(atof(s.c_str()));
					/*����y0*/
					if (_stricmp(subcd_name.c_str(), "SouthWestOrd") == 0)
					{
						s_south_west.append(" ").append(s);
					}
					/*����y1*/
					if (_stricmp(subcd_name.c_str(), "SouthEastOrd") == 0)
					{
						s_south_east.append(" ").append(s);
					}
					/*����y2*/
					if (_stricmp(subcd_name.c_str(), "NorthEastOrd") == 0)
					{
						s_north_east.append(" ").append(s);
					}
					/*����y3*/
					if (_stricmp(subcd_name.c_str(), "NorthWestOrd") == 0)
					{
						s_north_west.append(" ").append(s);
					}
				}
			}

			/*�������귶Χ��((x0 y0,x1 y1,x2 y2,x3 y3,x0 y0))��ʽ����json*/
			s_imgrange.append("((").append(s_south_west).append(",").append(s_south_east).append(",");
			s_imgrange.append(s_north_east).append(",").append(s_north_west).append(",").append(s_south_west).append("))");

			/*��ȡx��y�������Сֵ*/
			std::vector<double>::iterator x_max_itr = std::max_element(x.begin(), x.end());
			double x_max = *x_max_itr;
			std::vector<double>::iterator x_min_itr = std::min_element(x.begin(), x.end());
			double x_min = *x_min_itr;
			std::vector<double>::iterator y_max_itr = std::max_element(y.begin(), y.end());
			double y_max = *y_max_itr;
			std::vector<double>::iterator y_min_itr = std::min_element(y.begin(), y.end());
			double y_min = *y_min_itr;

			writer.Key(std::to_string(db_filed_num++).c_str());
			writer.Double(x_max);

			writer.Key(std::to_string(db_filed_num++).c_str());
			writer.Double(x_min);

			writer.Key(std::to_string(db_filed_num++).c_str());
			writer.Double(y_max);

			writer.Key(std::to_string(db_filed_num++).c_str());
			writer.Double(y_min);

			writer.Key(std::to_string(db_filed_num++).c_str());
			writer.String(s_imgrange.c_str());
		}
		/*������Ĳ���*/
		else if (_stricmp(chnode_name.c_str(), "MathFoundation") == 0)
		{
			for (pugi::xml_node subcd = chnode.first_child(); subcd; subcd = subcd.next_sibling())
			{
				std::string subcd_name = subcd.name();
				std::string s = subcd.text().as_string();
				/*���������ߺʹ��Ű��������*/
				if ((_stricmp(subcd_name.c_str(), "CentralMederian") == 0) || (_stricmp(subcd_name.c_str(), "GaussKrugerZoneNo") == 0))
				{
					writer.Key(std::to_string(db_filed_num++).c_str());
					writer.Int(atoi(s.c_str()));
				}
				else
				{
					writer.Key(std::to_string(db_filed_num++).c_str());
					writer.String(s.c_str());
				}
			}
		}
		else
		{
			std::string s = chnode.text().as_string();
			/*����ֱ��ʺ�ͼ���С�����������*/
			if ((_stricmp(chnode_name.c_str(), "GroundResolution") == 0) || (_stricmp(chnode_name.c_str(), "ImgSize") == 0))
			{
				writer.Key(std::to_string(db_filed_num++).c_str());
				writer.Double(atof(s.c_str()));
			}
			/*����λ�����������*/
			else if (_stricmp(chnode_name.c_str(), "PixelBits") == 0)
			{
				writer.Key(std::to_string(db_filed_num++).c_str());
				writer.Int(atoi(s.c_str()));
			}
			/*����ת��Ϊutc*/
			else if (_stricmp(chnode_name.c_str(), "ProduceDate") == 0)
			{
				time_t tm_t;
				tm t_s;
				t_s.tm_year = atoi(s.substr(0, 4).c_str()) - 1900;
				t_s.tm_mon = atoi(s.substr(5, 2).c_str()) - 1;
				t_s.tm_mday = atoi(s.substr(6, 2).c_str());
				t_s.tm_hour = 0;
				t_s.tm_min = 0;
				t_s.tm_sec = 0;
				tm_t = mktime(&t_s);
				writer.Key(std::to_string(db_filed_num++).c_str());
				writer.Int64(tm_t);
			}
			/*�����ֶ��ַ������*/
			else
			{
				writer.Key(std::to_string(db_filed_num++).c_str());
				writer.String(s.c_str());
			}
		}
	}

	/*����Դ�������*/
	root = doc.child("Metadatafile").child("ImgSource");
	for (pugi::xml_node chnode = root.first_child(); chnode; chnode = chnode.next_sibling())
	{
		std::string chnode_name = chnode.name();
		if ((_stricmp(chnode_name.c_str(), "PanBand") == 0) || (_stricmp(chnode_name.c_str(), "MultiBand") == 0))
		{
			for (pugi::xml_node subcd = chnode.first_child(); subcd; subcd = subcd.next_sibling())
			{
				std::string subcd_name = subcd.name();
				std::string s = subcd.text().as_string();

				/*ȫɫ����Ӱ��ֱ��ʺͶ��������Ӱ��ֱ��� �����������*/
				if ((_stricmp(subcd_name.c_str(), "SateResolution") == 0) || (_stricmp(subcd_name.c_str(), "MultiBandResolution") == 0))
				{
					writer.Key(std::to_string(db_filed_num++).c_str());
					writer.Double(atof(s.c_str()));
				}
				/*����ײ������������������*/
				else if (_stricmp(subcd_name.c_str(), "MultiBandNum") == 0)
				{
					writer.Key(std::to_string(db_filed_num++).c_str());
					writer.Int(atoi(s.c_str()));
				}
				/*���� UTCʱ����*/
				else if ((_stricmp(subcd_name.c_str(), "PbandDate") == 0) || (_stricmp(subcd_name.c_str(), "MultiBandDate") == 0))
				{
					time_t tm_t;
					tm t_s;
					t_s.tm_year = atoi(s.substr(0, 4).c_str()) - 1900;
					t_s.tm_mon = atoi(s.substr(5, 2).c_str()) - 1;
					t_s.tm_mday = atoi(s.substr(6, 2).c_str());
					t_s.tm_hour = 0;
					t_s.tm_min = 0;
					t_s.tm_sec = 0;
					tm_t = mktime(&t_s);
					writer.Key(std::to_string(db_filed_num++).c_str());
					writer.Int64(tm_t);
				}
				/*�����ֶ��ַ�����ʽ���*/
				else
				{
					writer.Key(std::to_string(db_filed_num++).c_str());
					writer.String(s.c_str());
				}
			}
		}
		/*����������*/
		else
		{
			std::string s = chnode.text().as_string();
			writer.Key(std::to_string(db_filed_num++).c_str());
			writer.String(s.c_str());
		}
	}

	/*��������������Ϣ����*/
	root = doc.child("Metadatafile").child("ProduceInfomation");
	for (pugi::xml_node chnode = root.first_child(); chnode; chnode = chnode.next_sibling())
	{
		std::string chnode_name = chnode.name();
		if ((_stricmp(chnode_name.c_str(), "ImgOrientation") == 0) || (_stricmp(chnode_name.c_str(), "MosaicInfo") == 0) ||
			(_stricmp(chnode_name.c_str(), "QualityCheckInfo") == 0))
		{
			for (pugi::xml_node subcd = chnode.first_child(); subcd; subcd = subcd.next_sibling())
			{
				std::string subcd_name = subcd.name();
				std::string s = subcd.text().as_string();

				/*��������ŵ��ֶ� ���� ��� �ӱ߲�*/
				if ((_stricmp(subcd_name.c_str(), "SateOriXRMS") == 0) || (_stricmp(subcd_name.c_str(), "SateOriYRMS") == 0) ||
					(_stricmp(subcd_name.c_str(), "SateOriZRMS") == 0) || (_stricmp(subcd_name.c_str(), "WestMosaicMaxError") == 0) ||
					(_stricmp(subcd_name.c_str(), "NorthMosaicMaxError") == 0) || (_stricmp(subcd_name.c_str(), "EastMosaicMaxError") == 0) ||
					(_stricmp(subcd_name.c_str(), "SouthMosaicMaxError") == 0) || (_stricmp(subcd_name.c_str(), "MultiBRectifyXRMS") == 0) ||
					(_stricmp(subcd_name.c_str(), "MultiBRectifyYRMS") == 0) || (_stricmp(subcd_name.c_str(), "CheckPointNum") == 0) ||
					(_stricmp(subcd_name.c_str(), "CheckRMS") == 0) || (_stricmp(subcd_name.c_str(), "CheckMAXErr") == 0))
				{
					writer.Key(std::to_string(db_filed_num++).c_str());
					writer.Double(atof(s.c_str()));
				}
				/*Ժ�����ʱ��;ּ�����ʱ����ΪUTCʱ��*/
				else if ((_stricmp(subcd_name.c_str(), "InstituteCheckDate") == 0) || (_stricmp(subcd_name.c_str(), "BureauCheckDate") == 0))
				{
					time_t tm_t;
					tm t_s;
					t_s.tm_year = atoi(s.substr(0, 4).c_str()) - 1900;
					t_s.tm_mon = atoi(s.substr(5, 2).c_str()) - 1;
					t_s.tm_mday = atoi(s.substr(6, 2).c_str());
					t_s.tm_hour = 0;
					t_s.tm_min = 0;
					t_s.tm_sec = 0;
					tm_t = mktime(&t_s);
					writer.Key(std::to_string(db_filed_num++).c_str());
					writer.Int64(tm_t);
				}
				/*�������ַ������*/
				else
				{
					writer.Key(std::to_string(db_filed_num++).c_str());
					writer.String(s.c_str());
				}
			}
		}
		else
		{
			std::string s = chnode.text().as_string();
			/*DEM������ఴ�������*/
			if (_stricmp(chnode_name.c_str(), "GridInterval") == 0)
			{
				std::string s = chnode.text().as_string();
				writer.Key(std::to_string(db_filed_num++).c_str());
				writer.Int(atoi(s.c_str()));
			}
			else
			{
				std::string s = chnode.text().as_string();
				writer.Key(std::to_string(db_filed_num++).c_str());
				writer.String(s.c_str());
			}
		}
	}

	writer.EndObject();

	out_json->assign(strbuf.GetString(), strbuf.GetSize());

	return 0;
}

