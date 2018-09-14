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

/*元数据映射表*/
static us_cms_metdata_map metadata_map_array[] =
{
	"MaxX"                , 1, 3, 0x30,     //地理范围MaxX
	"MinX"                , 2, 3, 0x30,		//地理范围MinX
	"MaxY"                , 3, 3, 0x30,		//地理范围MaxY
	"MinY"                , 4, 3, 0x30,		//地理范围MinY
	"ImgRange"            , 5, 1, 0x02,     //地理范围((西南，东南，东北，西北))
	"MetaDataFileName"    , 6, 1, 0x03,     //元数据文件名称
	"ProductName"         , 7, 1, 0x03,		// 产品名称
	"Owner"               , 8, 1, 0x03,		// 产品版权单位名
	"Producer"            , 9, 1, 0x03,		// 产品生产单位名
	"Publisher"           , 10, 1, 0x03,	// 产品出版单位名
	"ProduceDate"         , 11, 2, 0x0C,	// 产品生产时间
	"ConfidentialLevel"   , 12, 1, 0x02,	// 密级
	"GroundResolution"    , 13, 3, 0x30,	// 地面分辨率
	"ImgColorModel"       , 14, 1, 0x02,	// 影像色彩模式
	"PixelBits"           , 15, 2, 0x10,	// 像素位数
	"ImgSize"             , 16, 3, 0x30,	// 整景数据量大小
	"DataFormat"          , 17, 1, 0x02,	// 数据格式
	"LongerRadius"        , 18, 1, 0x02,	// 椭球长半径
	"OblatusRatio"        , 19, 1, 0x02,	// 椭球扁率
	"GeodeticDatum"       , 20, 1, 0x02,	// 所采用大地基准
	"MapProjection"       , 21, 1, 0x02,	// 地图投影
	"CentralMederian"     , 22, 2, 0x10,	// 中央子午线
	"ZoneDivisionMode"    , 23, 1, 0x02,	// 分带方式
	"GaussKrugerZoneNo"   , 24, 2, 0x30,	// 高斯 - 克吕格投影带号
	"CoordinationUnit"    , 25, 1, 0x02,	// 坐标单位
	"HeightSystem"        , 26, 1, 0x02,	// 高程系统名
	"HeightDatum"         , 27, 1, 0x02,	// 高程基准
	"SateName"            , 28, 1, 0x02,	// 卫星名称
	"PBandSensorType"     , 29, 1, 0x02,	// 全色影像传感器类型
	"SateResolution"      , 30, 3, 0x30,	// 全色卫星影像分辨率
	"PbandOrbitCode"      , 31, 1, 0x02,	// 全色卫星影像轨道号
	"PbandDate"           , 32, 2, 0x0C,	// 全色卫星影像获取时间
	"MultiBandSensorType" , 33, 1, 0x02,	// 多光谱影像传感器类型
	"MultiBandNum"        , 34, 2, 0x30,	// 多光谱波段数量
	"MultiBandName"       , 35, 1, 0x02,	// 多光谱波段名称
	"MultiBandResolution" , 36, 3, 0x30,	// 多光谱卫星影像分辨率
	"MultiBandOrbitCode"  , 37, 1, 0x02,	// 多光谱卫星影像轨道号
	"MultiBandDate"       , 38, 2, 0x0C,	// 多光谱卫星影像获取时间
	"SateImgQuality"      , 39, 1, 0x03,	// 卫星影像数据质量评价
	"GridInterval"        , 40, 2, 0x30,	// DEM格网间距
	"DEMPrecision"        , 41, 1, 0x02,	// DEM精度情况
	"ControlSource"       , 42, 1, 0x02,	// 控制资料来源
	"SateOriXRMS"         , 43, 3, 0x30,	// 外参数解算平面中误差(X)
	"SateOriYRMS"         , 44, 3, 0x30,	// 外参数解算平面中误差(Y)
	"SateOriZRMS"         , 45, 3, 0x30,	// 外参数解算高程中误差
	"ATProducerName"      , 46, 1, 0x02,	// 参数解算作业员
	"ATCheckerName"       , 47, 1, 0x02,	// 参数解算检查员
	"ManufactureType"     , 48, 1, 0x02,	// 数据生产方式
	"SteroEditQuality"    , 49, 1, 0x02,	// 立体模型编辑情况
	"OrthoRectifySoftWare", 50, 1, 0x02,	// 正射纠正软件
	"ResampleMethod"      , 51, 1, 0x02,	// 重采样方法
	"OrthoRectifyQuality" , 52, 1, 0x02,	// 正射纠正总结
	"OrthoRectifyName"    , 53, 1, 0x02,	// 正射纠正作业员
	"OrthoCheckName"      , 54, 1, 0x02,	// 正射纠正检查员
	"WestMosaicMaxError"  , 55, 3, 0x30,	// 西边最大接边差
	"NorthMosaicMaxError" , 56, 3, 0x30,	// 北边最大接边差
	"EastMosaicMaxError"  , 57, 3, 0x30,	// 东边最大接边差
	"SouthMosaicMaxError" , 58, 3, 0x30,	// 南边最大接边差
	"MosaicQuality"       , 59, 1, 0x02,	// 接边质量评价
	"MosaicProducerName"  , 60, 1, 0x02,	// 接边作业员
	"MosaicCheckerName"   , 61, 1, 0x02,	// 接边检查员
	"MultiBRectifyXRMS"   , 62, 3, 0x30,	// 多光谱配准纠正中误差(X)
	"MultiBRectifyYRMS"   , 63, 3, 0x30,	// 多光谱配准纠正中误差(Y)
	"CheckPointNum"       , 64, 2, 0x30,	// 检查点个数
	"CheckRMS"            , 65, 3, 0x30,	// 检查点平面中误差
	"CheckMAXErr"         , 66, 3, 0x30,	// 检查点最大误差
	"ConclusionInstitute" , 67, 1, 0x02,	// 院级检查结论
	"InstituteCheckUnit"  , 68, 1, 0x03,	// 院级检查单位
	"InstituteCheckName"  , 69, 1, 0x02,	// 院级检查人
	"InstituteCheckDate"  , 70, 2, 0x0C,	// 院级检查时间
	"BureauCheckName"     , 71, 1, 0x02,	// 局级验收人
	"BureauCheckUnit"     , 72, 1, 0x03,	// 局级验收单位
	"ConclusionBureau"    , 73, 1, 0x02,	// 局级验收意见
	"BureauCheckDate"     , 74, 2, 0x0C		// 局级验收时间
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
	/*加载xml文件*/
	pugi::xml_parse_result result = doc.load_file(xmlpath.c_str());
	if (!result)
	{
		out_json->assign("load file failed");
		return -1;
	}

	rapidjson::StringBuffer strbuf;
	strbuf.Reserve(1024);
	rapidjson::Writer<rapidjson::StringBuffer> writer(strbuf);

	/*读取xml文件，写入json*/
	writer.StartObject();

	/*产品基本情况部分*/
	pugi::xml_node root = doc.child("Metadatafile").child("BasicDataContent");
	for (pugi::xml_node chnode = root.first_child(); chnode; chnode = chnode.next_sibling())
	{
		std::string chnode_name = chnode.name();

		/*地理范围，找出横坐标和纵坐标的最大最小值按浮点数类型存放*/
		if (_stricmp(chnode_name.c_str(), "ImgRange") == 0)
		{
			std::vector<double> x;  //存放横坐标的值
			std::vector<double> y;  //存放纵坐标的值

			std::string s_imgrange = "";    //地理坐标范围
			std::string s_south_west = "";  //西南
			std::string s_south_east = "";  //东南
			std::string s_north_east = "";  //东北
			std::string s_north_west = "";  //西北

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
					/*西南x0*/
					if (_stricmp(subcd_name.c_str(), "SouthWestAbs") == 0)
					{
						s_south_west.append(s);
					}
					/*东南x1*/
					if (_stricmp(subcd_name.c_str(), "SouthEastAbs") == 0)
					{
						s_south_east.append(s);
					}
					/*东北x2*/
					if (_stricmp(subcd_name.c_str(), "NorthEastAbs") == 0)
					{
						s_north_east.append(s);
					}
					/*西北x3*/
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
					/*西南y0*/
					if (_stricmp(subcd_name.c_str(), "SouthWestOrd") == 0)
					{
						s_south_west.append(" ").append(s);
					}
					/*东南y1*/
					if (_stricmp(subcd_name.c_str(), "SouthEastOrd") == 0)
					{
						s_south_east.append(" ").append(s);
					}
					/*东北y2*/
					if (_stricmp(subcd_name.c_str(), "NorthEastOrd") == 0)
					{
						s_north_east.append(" ").append(s);
					}
					/*西北y3*/
					if (_stricmp(subcd_name.c_str(), "NorthWestOrd") == 0)
					{
						s_north_west.append(" ").append(s);
					}
				}
			}

			/*地理坐标范围以((x0 y0,x1 y1,x2 y2,x3 y3,x0 y0))形式存入json*/
			s_imgrange.append("((").append(s_south_west).append(",").append(s_south_east).append(",");
			s_imgrange.append(s_north_east).append(",").append(s_north_west).append(",").append(s_south_west).append("))");

			/*获取x和y的最大最小值*/
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

		/*椭球体的部分*/
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
				/*中央子午线和带号按整数存放*/
				if (_stricmp(subcd_name.c_str(), "CentralMederian") == 0)   //中央子午线
				{
					writer.Key("22");
					writer.Int64(atoi(s.c_str()));
				}
				else if (_stricmp(subcd_name.c_str(), "GaussKrugerZoneNo") == 0)  //带号
				{
					writer.Key("24");
					writer.Int64(atoi(s.c_str()));
				}
				else if (_stricmp(subcd_name.c_str(), "LongerRadius") == 0)  //椭球长半径
				{
					writer.Key("18");
					writer.String(s.c_str());
				}
				else if (_stricmp(subcd_name.c_str(), "OblatusRatio") == 0)  //椭球扁率
				{
					writer.Key("19");
					writer.String(s.c_str());
				}
				else if (_stricmp(subcd_name.c_str(), "GeodeticDatum") == 0) //所采用大地基准
				{
					writer.Key("20");
					writer.String(s.c_str());
				}
				else if (_stricmp(subcd_name.c_str(), "MapProjection") == 0)  //地图投影
				{
					writer.Key("21");
					writer.String(s.c_str());
				}
				else if (_stricmp(subcd_name.c_str(), "ZoneDivisionMode") == 0)  //分带方式
				{
					writer.Key("23");
					writer.String(s.c_str());
				}
				else if (_stricmp(subcd_name.c_str(), "CoordinationUnit") == 0)  //坐标单位
				{
					writer.Key("25");
					writer.String(s.c_str());
				}
				else if (_stricmp(subcd_name.c_str(), "HeightSystem") == 0)  //高程系统名
				{
					writer.Key("26");
					writer.String(s.c_str());
				}
				else if (_stricmp(subcd_name.c_str(), "HeightDatum") == 0) //高程基准
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

			/*地面分辨率和图像大小按浮点数存放*/
			if (_stricmp(chnode_name.c_str(), "GroundResolution") == 0)  //地面分辨率
			{
				writer.Key("13");
				writer.Double(atof(s.c_str()));
			}
			else if (_stricmp(chnode_name.c_str(), "ImgSize") == 0)  //整景数据量大小
			{
				writer.Key("16");
				writer.Double(atof(s.c_str()));
			}
			/*像素位数按整数存放*/
			else if (_stricmp(chnode_name.c_str(), "PixelBits") == 0)  //像素位数
			{
				writer.Key("15");
				writer.Int64(atoi(s.c_str()));
			}
			/*日期转化为utc*/
			else if (_stricmp(chnode_name.c_str(), "ProduceDate") == 0)  //产品生产时间
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
			/*其他字段字符串存放*/
			else if (_stricmp(chnode_name.c_str(), "MetaDataFileName") == 0) //元数据文件名称
			{
				writer.Key("6");
				writer.String(s.c_str());
			}
			else if (_stricmp(chnode_name.c_str(), "ProductName") == 0) //产品名称
			{
				writer.Key("7");
				writer.String(s.c_str());
			}
			else if (_stricmp(chnode_name.c_str(), "Owner") == 0)  //产品版权单位名
			{
				writer.Key("8");
				writer.String(s.c_str());
			}
			else if (_stricmp(chnode_name.c_str(), "Producer") == 0)  //产品生产单位名
			{
				writer.Key("9");
				writer.String(s.c_str());
			}
			else if (_stricmp(chnode_name.c_str(), "Publisher") == 0)  //产品出版单位名
			{
				writer.Key("10");
				writer.String(s.c_str());
			}
			else if (_stricmp(chnode_name.c_str(), "ConfidentialLevel") == 0) //密级
			{
				writer.Key("12");
				writer.String(s.c_str());
			}
			else if (_stricmp(chnode_name.c_str(), "ImgColorModel") == 0)  //影像色彩模式
			{
				writer.Key("14");
				writer.String(s.c_str());
			}
			else if (_stricmp(chnode_name.c_str(), "DataFormat") == 0)  //数据格式
			{
				writer.Key("17");
				writer.String(s.c_str());
			}
		}
	}

	/*数据源情况部分*/
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

				/*全色卫星影像分辨率和多光谱卫星影像分辨率 按浮点数存放*/
				if (_stricmp(subcd_name.c_str(), "SateResolution") == 0)   //全色卫星影像分辨率
				{
					writer.Key("30");
					writer.Double(atof(s.c_str()));
				}
				else if (_stricmp(subcd_name.c_str(), "MultiBandResolution") == 0)  //多光谱卫星影像分辨率
				{
					writer.Key("36");
					writer.Double(atof(s.c_str()));
				}
				/*多光谱波段数量，按整数存放*/
				else if (_stricmp(subcd_name.c_str(), "MultiBandNum") == 0)  //多光谱波段数量
				{
					writer.Key("34");
					writer.Int64(atoi(s.c_str()));
				}
				/*日期 UTC时间存放*/
				else if (_stricmp(subcd_name.c_str(), "PbandDate") == 0)  //全色卫星影像获取时间
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
				else if (_stricmp(subcd_name.c_str(), "MultiBandDate") == 0)  //多光谱卫星影像获取时间
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
				/*其他字段字符串形式存放*/
				else if (_stricmp(subcd_name.c_str(), "PBandSensorType") == 0)  //全色影像传感器类型
				{
					writer.Key("29");
					writer.String(s.c_str());
				}
				else if (_stricmp(subcd_name.c_str(), "PbandOrbitCode") == 0)  //全色卫星影像轨道号
				{
					writer.Key("31");
					writer.String(s.c_str());
				}
				else if (_stricmp(subcd_name.c_str(), "MultiBandSensorType") == 0)  //多光谱影像传感器类型
				{
					writer.Key("33");
					writer.String(s.c_str());
				}
				else if (_stricmp(subcd_name.c_str(), "MultiBandName") == 0) //多光谱波段名称
				{
					writer.Key("35");
					writer.String(s.c_str());
				}
				else if (_stricmp(subcd_name.c_str(), "MultiBandOrbitCode") == 0)  //多光谱卫星影像轨道号
				{
					writer.Key("37");
					writer.String(s.c_str());
				}
			}
		}
		else if (_stricmp(chnode_name.c_str(), "SateName") == 0) //卫星名称
		{
			std::string s = "";
			if (!chnode.text().empty())
			{
				s = chnode.text().as_string();
			}
			writer.Key("28");
			writer.String(s.c_str());
		}
		else if (_stricmp(chnode_name.c_str(), "SateImgQuality") == 0) //卫星影像数据质量评价
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

	/*数据生产过程信息部分*/
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

				/*浮点数存放的字段 包括 误差 接边差*/
				if (_stricmp(subcd_name.c_str(), "SateOriXRMS") == 0) //外参数解算平面中误差(X)
				{
					writer.Key("43");
					writer.Double(atof(s.c_str()));
				}
				else if (_stricmp(subcd_name.c_str(), "SateOriYRMS") == 0)  //外参数解算平面中误差(Y)
				{
					writer.Key("44");
					writer.Double(atof(s.c_str()));
				}
				else if (_stricmp(subcd_name.c_str(), "SateOriZRMS") == 0)  //外参数解算高程中误差(Z)
				{
					writer.Key("45");
					writer.Double(atof(s.c_str()));
				}
				else if (_stricmp(subcd_name.c_str(), "WestMosaicMaxError") == 0)  //西边最大接边差
				{
					writer.Key("55");
					writer.Double(atof(s.c_str()));
				}
				else if (_stricmp(subcd_name.c_str(), "NorthMosaicMaxError") == 0)  //北边最大接边差
				{
					writer.Key("56");
					writer.Double(atof(s.c_str()));
				}
				else if (_stricmp(subcd_name.c_str(), "EastMosaicMaxError") == 0)  //东边最大接边差
				{
					writer.Key("57");
					writer.Double(atof(s.c_str()));
				}
				else if (_stricmp(subcd_name.c_str(), "SouthMosaicMaxError") == 0)  //南边最大接边差
				{
					writer.Key("58");
					writer.Double(atof(s.c_str()));
				}
				else if (_stricmp(subcd_name.c_str(), "MultiBRectifyXRMS") == 0)  //多光谱配准纠正中误差(X)
				{
					writer.Key("62");
					writer.Double(atof(s.c_str()));
				}
				else if (_stricmp(subcd_name.c_str(), "MultiBRectifyYRMS") == 0)  //多光谱配准纠正中误差(Y)
				{
					writer.Key("63");
					writer.Double(atof(s.c_str()));
				}
				else if (_stricmp(subcd_name.c_str(), "CheckPointNum") == 0)  //检查点个数
				{
					writer.Key("64");
					writer.Double(atof(s.c_str()));
				}
				else if (_stricmp(subcd_name.c_str(), "CheckRMS") == 0)  //检查点平面中误差
				{
					writer.Key("65");
					writer.Double(atof(s.c_str()));
				}
				else if (_stricmp(subcd_name.c_str(), "CheckMAXErr") == 0)  //检查点最大误差
				{
					writer.Key("66");
					writer.Double(atof(s.c_str()));
				}
				/*院级检查时间和局级验收时间存放为UTC时间*/
				else if (_stricmp(subcd_name.c_str(), "InstituteCheckDate") == 0)  //院级检查时间
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
				else if (_stricmp(subcd_name.c_str(), "BureauCheckDate") == 0)  //局级验收时间
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
				/*其他按字符串存放*/
				else if (_stricmp(subcd_name.c_str(), "ATProducerName") == 0)  //参数解算作业员
				{
					writer.Key("46");
					writer.String(s.c_str());
				}
				else if (_stricmp(subcd_name.c_str(), "ATCheckerName") == 0)  //参数解算检查员
				{
					writer.Key("47");
					writer.String(s.c_str());
				}
				else if (_stricmp(subcd_name.c_str(), "MosaicQuality") == 0)  //接边质量评价
				{
					writer.Key("59");
					writer.String(s.c_str());
				}
				else if (_stricmp(subcd_name.c_str(), "MosaicProducerName") == 0)  //接边作业员
				{
					writer.Key("60");
					writer.String(s.c_str());
				}
				else if (_stricmp(subcd_name.c_str(), "MosaicCheckerName") == 0)  //接边检查员
				{
					writer.Key("61");
					writer.String(s.c_str());
				}
				else if (_stricmp(subcd_name.c_str(), "ConclusionInstitute") == 0)  //院级检查结论
				{
					writer.Key("67");
					writer.String(s.c_str());
				}
				else if (_stricmp(subcd_name.c_str(), "InstituteCheckUnit") == 0)  //院级检查单位
				{
					writer.Key("68");
					writer.String(s.c_str());
				}
				else if (_stricmp(subcd_name.c_str(), "InstituteCheckName") == 0)  //院级检查人
				{
					writer.Key("69");
					writer.String(s.c_str());
				}
				else if (_stricmp(subcd_name.c_str(), "BureauCheckName") == 0)  //局级验收人
				{
					writer.Key("71");
					writer.String(s.c_str());
				}
				else if (_stricmp(subcd_name.c_str(), "BureauCheckUnit") == 0)  //局级验收单位
				{
					writer.Key("72");
					writer.String(s.c_str());
				}
				else if (_stricmp(subcd_name.c_str(), "ConclusionBureau") == 0)  //局级验收意见
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

			/*DEM格网间距按整数存放*/
			if (_stricmp(chnode_name.c_str(), "GridInterval") == 0)  //DEM格网间距
			{
				writer.Key("40");
				writer.Int64(atoi(s.c_str()));
			}
			else if (_stricmp(chnode_name.c_str(), "DEMPrecision") == 0) //DEM精度情况
			{
				writer.Key("41");
				writer.String(s.c_str());
			}
			else if (_stricmp(chnode_name.c_str(), "ControlSource") == 0) //控制资料来源
			{
				writer.Key("42");
				writer.String(s.c_str());
			}
			else if (_stricmp(chnode_name.c_str(), "ManufactureType") == 0) //数据生产方式
			{
				writer.Key("48");
				writer.String(s.c_str());
			}
			else if (_stricmp(chnode_name.c_str(), "SteroEditQuality") == 0) //立体模型编辑情况
			{
				writer.Key("49");
				writer.String(s.c_str());
			}
			else if (_stricmp(chnode_name.c_str(), "OrthoRectifySoftware") == 0)  //正射纠正软件
			{
				writer.Key("50");
				writer.String(s.c_str());
			}
			else if (_stricmp(chnode_name.c_str(), "ResampleMethod") == 0) //重采样方法
			{
				writer.Key("51");
				writer.String(s.c_str());
			}
			else if (_stricmp(chnode_name.c_str(), "OrthoRectifyQuality") == 0) //正射纠正总结
			{
				writer.Key("52");
				writer.String(s.c_str());
			}
			else if (_stricmp(chnode_name.c_str(), "OrthoRectifyName") == 0) //正射纠正作业员
			{
				writer.Key("53");
				writer.String(s.c_str());
			}
			else if (_stricmp(chnode_name.c_str(), "OrthoCheckName") == 0) //正射纠正检查员
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






/*之前写的*/
const us_cms_metdata_map * us_get_cms_metadata_map_table1(int * out_size)
{
	std::string path = "./ngcc_metadata_table.txt";

	std::ifstream infile;
	infile.open(path.data());   //将文件流对象与文件连接起来 
	assert(infile.is_open());   //若失败,则输出错误消息,并终止程序运行 

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

	infile.close();             //关闭文件输入流 

	return nullptr;
}

int us_read_cms_metadata_record1(const std::string & index, std::string * out_json)
{
	pugi::xml_document doc;
	pugi::xml_parse_result result = doc.load_file(index.c_str());  //加载xml文件
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

/*第二次*/
int us_read_cms_metadata_record2(const std::string & index, std::string * out_json)
{
	pugi::xml_document doc;
	/*加载xml文件*/
	pugi::xml_parse_result result = doc.load_file(index.c_str());
	if (!result)
	{
		out_json->assign("load file failed");
		return -1;
	}

	rapidjson::StringBuffer strbuf;
	strbuf.Reserve(1024);
	rapidjson::Writer<rapidjson::StringBuffer> writer(strbuf);

	/*读取xml文件，写入json*/
	writer.StartObject();

	int db_filed_num = 1;  //存入json的键名，需要转为字符串类型

						   /*产品基本情况部分*/
	pugi::xml_node root = doc.child("Metadatafile").child("BasicDataContent");
	for (pugi::xml_node chnode = root.first_child(); chnode; chnode = chnode.next_sibling())
	{
		std::string chnode_name = chnode.name();

		/*地理范围，找出横坐标和纵坐标的最大最小值按浮点数类型存放*/
		if (_stricmp(chnode_name.c_str(), "ImgRange") == 0)
		{
			std::vector<double> x;  //存放横坐标的值
			std::vector<double> y;  //存放纵坐标的值

			std::string s_imgrange;  //地理坐标范围
			std::string s_south_west;  //西南
			std::string s_south_east;  //东南
			std::string s_north_east;  //东北
			std::string s_north_west;  //西北

			for (pugi::xml_node subcd = chnode.first_child(); subcd; subcd = subcd.next_sibling())
			{
				std::string subcd_name = subcd.name();
				if ((_stricmp(subcd_name.c_str(), "SouthWestAbs") == 0) || (_stricmp(subcd_name.c_str(), "NorthWestAbs") == 0) ||
					(_stricmp(subcd_name.c_str(), "NorthEastAbs") == 0) || (_stricmp(subcd_name.c_str(), "SouthEastAbs") == 0))
				{
					std::string s = subcd.text().as_string();
					x.push_back(atof(s.c_str()));
					/*西南x0*/
					if (_stricmp(subcd_name.c_str(), "SouthWestAbs") == 0)
					{
						s_south_west.append(s);
					}
					/*东南x1*/
					if (_stricmp(subcd_name.c_str(), "SouthEastAbs") == 0)
					{
						s_south_east.append(s);
					}
					/*东北x2*/
					if (_stricmp(subcd_name.c_str(), "NorthEastAbs") == 0)
					{
						s_north_east.append(s);
					}
					/*西北x3*/
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
					/*西南y0*/
					if (_stricmp(subcd_name.c_str(), "SouthWestOrd") == 0)
					{
						s_south_west.append(" ").append(s);
					}
					/*东南y1*/
					if (_stricmp(subcd_name.c_str(), "SouthEastOrd") == 0)
					{
						s_south_east.append(" ").append(s);
					}
					/*东北y2*/
					if (_stricmp(subcd_name.c_str(), "NorthEastOrd") == 0)
					{
						s_north_east.append(" ").append(s);
					}
					/*西北y3*/
					if (_stricmp(subcd_name.c_str(), "NorthWestOrd") == 0)
					{
						s_north_west.append(" ").append(s);
					}
				}
			}

			/*地理坐标范围以((x0 y0,x1 y1,x2 y2,x3 y3,x0 y0))形式存入json*/
			s_imgrange.append("((").append(s_south_west).append(",").append(s_south_east).append(",");
			s_imgrange.append(s_north_east).append(",").append(s_north_west).append(",").append(s_south_west).append("))");

			/*获取x和y的最大最小值*/
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
		/*椭球体的部分*/
		else if (_stricmp(chnode_name.c_str(), "MathFoundation") == 0)
		{
			for (pugi::xml_node subcd = chnode.first_child(); subcd; subcd = subcd.next_sibling())
			{
				std::string subcd_name = subcd.name();
				std::string s = subcd.text().as_string();
				/*中央子午线和带号按整数存放*/
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
			/*地面分辨率和图像大小按浮点数存放*/
			if ((_stricmp(chnode_name.c_str(), "GroundResolution") == 0) || (_stricmp(chnode_name.c_str(), "ImgSize") == 0))
			{
				writer.Key(std::to_string(db_filed_num++).c_str());
				writer.Double(atof(s.c_str()));
			}
			/*像素位数按整数存放*/
			else if (_stricmp(chnode_name.c_str(), "PixelBits") == 0)
			{
				writer.Key(std::to_string(db_filed_num++).c_str());
				writer.Int(atoi(s.c_str()));
			}
			/*日期转化为utc*/
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
			/*其他字段字符串存放*/
			else
			{
				writer.Key(std::to_string(db_filed_num++).c_str());
				writer.String(s.c_str());
			}
		}
	}

	/*数据源情况部分*/
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

				/*全色卫星影像分辨率和多光谱卫星影像分辨率 按浮点数存放*/
				if ((_stricmp(subcd_name.c_str(), "SateResolution") == 0) || (_stricmp(subcd_name.c_str(), "MultiBandResolution") == 0))
				{
					writer.Key(std::to_string(db_filed_num++).c_str());
					writer.Double(atof(s.c_str()));
				}
				/*多光谱波段数量，按整数存放*/
				else if (_stricmp(subcd_name.c_str(), "MultiBandNum") == 0)
				{
					writer.Key(std::to_string(db_filed_num++).c_str());
					writer.Int(atoi(s.c_str()));
				}
				/*日期 UTC时间存放*/
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
				/*其他字段字符串形式存放*/
				else
				{
					writer.Key(std::to_string(db_filed_num++).c_str());
					writer.String(s.c_str());
				}
			}
		}
		/*即卫星名称*/
		else
		{
			std::string s = chnode.text().as_string();
			writer.Key(std::to_string(db_filed_num++).c_str());
			writer.String(s.c_str());
		}
	}

	/*数据生产过程信息部分*/
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

				/*浮点数存放的字段 包括 误差 接边差*/
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
				/*院级检查时间和局级验收时间存放为UTC时间*/
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
				/*其他按字符串存放*/
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
			/*DEM格网间距按整数存放*/
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

