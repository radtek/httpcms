#if 1
#include "../parsexml/pugixml.hpp"
#include "field.h"
#include <geo_coord_transform/us_spatial_reference.hpp>

#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <algorithm>
#include <vector>
#include <time.h>

#include <geos/io/WKTReader.h>
#include <geos/geom/Polygon.h>

#include <iostream>
#include <fstream>
#include <cassert>

static us_cms_metdata_map metadata_map_array[] =
{
	"MaxX"					, 1, 3, 0x00,     //地理范围MaxX
	"MinX"					, 2, 3, 0x00,	  //地理范围MinX
	"MaxY"					, 3, 3, 0x00,	  //地理范围MaxY
	"MinY"					, 4, 3, 0x00,	  //地理范围MinY
	"ImgRange"				, 5, 1, 0x00,     //地理范围((西南，东南，东北，西北))
	"ImgCorner"				, 6, 1, 0x00,     //地理角点

	"MetaDataFileName"		, 7, 1, 0x03,	  //元数据文件名称

	"typename"				, 8, 2, 0x02,     //影像类型
	"ProduceDate"			, 9, 2, 0x0C,	  //影像获取时间
	"GroundResolution"      , 10, 3, 0x30,    //影像分辨率
	"DataFormat"			, 11, 1, 0x02,	  //数据格式

	"LongerRadius"			, 12, 1, 0x02,	  //椭球长半径
	"OblatusRatio"			, 13, 1, 0x02,	  //椭球扁率
	"GeodeticDatum"			, 14, 1, 0x02,	  //所采用大地基准
	"MapProjection"			, 15, 1, 0x02,	  //地图投影
	"CentralMederian"		, 16, 2, 0x10,	  //中央子午线
	"ZoneDivisionMode"		, 17, 1, 0x02,	  //分带方式
	"GaussKrugerZoneNo"		, 18, 2, 0x30,	  //高斯 - 克吕格投影带号
	"CoordinationUnit"		, 19, 1, 0x02,	  //坐标单位
	"HeightSystem"			, 20, 1, 0x02,	  //高程系统名
	"HeightDatum"			, 21, 1, 0x02,	  //高程基准
};

const us_cms_metdata_map * us_get_cms_metadata_map_table(int * out_size)
{
	*out_size = sizeof(metadata_map_array) / sizeof(us_cms_metdata_map);
	return metadata_map_array;
}

#ifdef _WIN32
#include <Windows.h>
inline std::wstring UTF8ToUTF16LE(const std::string& strUTF8)
{
	int dwUnicodeLen = MultiByteToWideChar(CP_UTF8, 0, strUTF8.c_str(), strUTF8.size(), NULL, 0);
	std::wstring utf16le(dwUnicodeLen, 0);
	MultiByteToWideChar(CP_UTF8, 0, strUTF8.c_str(), strUTF8.size(),
		(wchar_t*)utf16le.data(), dwUnicodeLen);
	return utf16le;
}
#endif // _WIN32

#define _US_MAX(a,b) (((a) > (b)) ? (a) : (b))
#define _US_MIN(a,b) (((a) < (b)) ? (a) : (b))

inline time_t shandong_timestr_to_utc(const char* tstr)
{
	std::string s(tstr);
	time_t tm_t;
	tm t_s;
	t_s.tm_year = atoi(s.substr(0, 4).c_str()) - 1900;
	t_s.tm_mon = atoi(s.substr(4, 2).c_str()) - 1;
	t_s.tm_mday = atoi(s.substr(6, 2).c_str());
	t_s.tm_hour = 0;
	t_s.tm_min = 0;
	t_s.tm_sec = 0;
	tm_t = mktime(&t_s);
	return tm_t;
}

int us_read_cms_metadata_record(const std::string& index, std::string* out_json)
{
	static unispace::us_spatial_reference cgcs2000("+proj=longlat +ellps=GRS80 +no_defs");

	if (index.size() < 6) { return -1; }
	std::string xmlpath = index;
	xmlpath.replace(index.size() - 5, 5, "Y.XML");
	pugi::xml_document doc;
	/*加载xml文件*/
#ifdef _WIN32
	pugi::xml_parse_result result = doc.load_file(UTF8ToUTF16LE(xmlpath).c_str(), 116U, pugi::encoding_utf8);
#else
	pugi::xml_parse_result result = doc.load_file(xmlpath.c_str(), 116U, pugi::encoding_utf8);

#endif // _WIN32

	if (!result) {
		out_json->assign(result.description());
		return -1;
	}

	rapidjson::StringBuffer strbuf;
	strbuf.Reserve(1024);
	rapidjson::Writer<rapidjson::StringBuffer> writer(strbuf);

	writer.StartObject();

	/*产品基本情况部分*/
	int GaussKrugerZoneNo = -1;
	float CentralMederian = 0;
	
	pugi::xml_node nodeBasicDataContent = doc.child("Metadatafile").child("BasicDataContent");
	if (nodeBasicDataContent.empty()) { return -10000; }
	{
		{
			//元数据文件名称
			pugi::xml_node node = nodeBasicDataContent.child("MetaDataFileName");
			if (node.empty()) { return -7; }
			writer.Key("7");
			writer.String(node.text().as_string());

			//影像类型
			node = nodeBasicDataContent.child("typename");
			if (node.empty()) { return -8; }
			writer.Key("8");
			writer.String(node.text().as_string());

			//影像时间
			node = nodeBasicDataContent.child("ProduceDate");
			if (node.empty()) { return -9; }
			writer.Key("9");
			writer.Int64(shandong_timestr_to_utc(node.text().as_string()));

			//影像分辨率
			node = nodeBasicDataContent.child("GroundResolution");
			if (node.empty()) { return -10; }
			writer.Key("10");
			writer.Double(node.text().as_double());

			//影像格式
			node = nodeBasicDataContent.child("DataFormat");
			if (node.empty()) { return -11; }
			writer.Key("11");
			writer.String(node.text().as_string());
		}

		/* 椭球体 */
		pugi::xml_node nodeMathFoundation = nodeBasicDataContent.child("MathFoundation");
		if (nodeMathFoundation.empty()) { return -10001; }
		{
			pugi::xml_node node = nodeMathFoundation.child("LongerRadius");  /*椭球长半径*/
			if (node.empty()) { return -12; }
			writer.Key("12");
			writer.String(node.text().as_string());
			node = nodeMathFoundation.child("OblatusRatio");  /*椭球扁率*/
			if (node.empty()) { return -13; }
			writer.Key("13");
			writer.String(node.text().as_string());
			node = nodeMathFoundation.child("GeodeticDatum");  /*大地基准*/
			if (node.empty()) { return -14; }
			writer.Key("14");
			writer.String(node.text().as_string());
			node = nodeMathFoundation.child("MapProjection");  /*地图投影*/
			if (node.empty()) { return -15; }
			writer.Key("15");
			writer.String(node.text().as_string());

			node = nodeMathFoundation.child("CentralMederian"); /*中央子午线*/
			if (node.empty()) { return -16; }
			CentralMederian = node.text().as_float();
			writer.Key("16");
			writer.Int(node.text().as_int());

			node = nodeMathFoundation.child("ZoneDivisionMode"); /*分带方式*/
			if (node.empty()) { return -17; }
			writer.Key("17");
			writer.String(node.text().as_string());

			node = nodeMathFoundation.child("GaussKrugerZoneNo"); /*带号*/
			if (node.empty()) { return -18; }
			GaussKrugerZoneNo = node.text().as_int();
			if (GaussKrugerZoneNo < 0 || GaussKrugerZoneNo > 120) { return -18; }
			writer.Key("18");
			writer.Int(GaussKrugerZoneNo);

			node = nodeMathFoundation.child("CoordinationUnit"); /*坐标单位*/
			if (node.empty()) { return -19; }
			writer.Key("19");
			writer.String(node.text().as_string());

			node = nodeMathFoundation.child("HeightSystem");   /*高程系统名*/
			if (node.empty()) { return -20; }
			writer.Key("20");
			writer.String(node.text().as_string());
			node = nodeMathFoundation.child("HeightDatum");   /*高程基准*/
			if (node.empty()) { return -21; }
			writer.Key("21");
			writer.String(node.text().as_string());
		}

		/* 地理范围，找出横坐标和纵坐标的最大最小值按浮点数类型存放 */
		pugi::xml_node nodeImgRange = nodeBasicDataContent.child("ImgRange");
		if (nodeImgRange.empty()) { return -10002; }
		{
			pugi::xml_node cornernode;
			double cornerXY[8];
			/* 西南 东南 东北 西北 */
			cornernode = nodeImgRange.child("SouthWestAbs");
			if (cornernode.empty()) { return -5; }
			cornerXY[0] = cornernode.text().as_double();
			cornernode = nodeImgRange.child("SouthEastAbs");
			if (cornernode.empty()) { return -5; }
			cornerXY[1] = cornernode.text().as_double();
			cornernode = nodeImgRange.child("NorthEastAbs");
			if (cornernode.empty()) { return -5; }
			cornerXY[2] = cornernode.text().as_double();
			cornernode = nodeImgRange.child("NorthWestAbs");
			if (cornernode.empty()) { return -5; }
			cornerXY[3] = cornernode.text().as_double();

			cornernode = nodeImgRange.child("SouthWestOrd");
			if (cornernode.empty()) { return -5; }
			cornerXY[4] = cornernode.text().as_double();
			cornernode = nodeImgRange.child("SouthEastOrd");
			if (cornernode.empty()) { return -5; }
			cornerXY[5] = cornernode.text().as_double();
			cornernode = nodeImgRange.child("NorthEastOrd");
			if (cornernode.empty()) { return -5; }
			cornerXY[6] = cornernode.text().as_double();
			cornernode = nodeImgRange.child("NorthWestOrd");
			if (cornernode.empty()) { return -5; }
			cornerXY[7] = cornernode.text().as_double();

			/* 原始投影坐标有效范围角点 */
			char buffer[1024];
			int len = sprintf(buffer, "[%g %g,%g %g,%g %e,%g %g]", cornerXY[0], cornerXY[4],
				cornerXY[1], cornerXY[5], cornerXY[2], cornerXY[6], cornerXY[3], cornerXY[7]);
			writer.Key("6");
			writer.String(buffer, len);

			// 坐标转换--投影转经纬度
			int32_t false_easting = 500000;
			if (cornerXY[0] > 10000000.0) {
				false_easting += GaussKrugerZoneNo * 1000000;
			}
			sprintf(buffer, "+proj=tmerc +lat_0=0 +lon_0=%f +k=1 +x_0=%d +y_0=0 +ellps=GRS80 +units=m +no_defs",
				CentralMederian, false_easting);
			unispace::us_spatial_reference GKProj(buffer);

			int result = unispace::us_spatial_reference::transform(GKProj, cgcs2000, 4, 1,
				&cornerXY[0], &cornerXY[4], NULL);

			writer.Key("1");
			writer.Double(_US_MAX(_US_MAX(cornerXY[0], cornerXY[1]), _US_MAX(cornerXY[2], cornerXY[3])));

			writer.Key("2");
			writer.Double(_US_MIN(_US_MIN(cornerXY[0], cornerXY[1]), _US_MIN(cornerXY[2], cornerXY[3])));

			writer.Key("3");
			writer.Double(_US_MAX(_US_MAX(cornerXY[4], cornerXY[5]), _US_MAX(cornerXY[6], cornerXY[7])));

			writer.Key("4");
			writer.Double(_US_MIN(_US_MIN(cornerXY[4], cornerXY[5]), _US_MIN(cornerXY[6], cornerXY[7])));

			/*地理坐标范围以((x0 y0,x1 y1,x2 y2,x3 y3,x0 y0))形式存入json*/
			len = sprintf(buffer, "((%e %e,%e %e,%e %e,%e %e,%e %e))",
				cornerXY[0], cornerXY[4], cornerXY[1], cornerXY[5], cornerXY[2], cornerXY[6],
				cornerXY[3], cornerXY[7], cornerXY[0], cornerXY[4]);
			writer.Key("5");
			writer.String(buffer, len);
		}
	}
	writer.EndObject();

	out_json->assign(strbuf.GetString(), strbuf.GetSize());

	return 0;
}


#endif


