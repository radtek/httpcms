#if 0

#include "field.h"
#include "../parsexml/pugixml.hpp"
#include <geo_coord_transform/us_spatial_reference.hpp>
#include <time.h>
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <algorithm>
#include <vector>

#define MAXMAPLEN 200

us_cms_metdata_map metadata_map[MAXMAPLEN];

/*Ԫ����ӳ���*/
static us_cms_metdata_map metadata_map_array[] =
{
	"MaxX"                , 1, 3, 0x00,     //����ΧMaxX
	"MinX"                , 2, 3, 0x00,		//����ΧMinX
	"MaxY"                , 3, 3, 0x00,		//����ΧMaxY
	"MinY"                , 4, 3, 0x00,		//����ΧMinY
	"ImgRange"            , 5, 1, 0x00,     //����Χ((���ϣ����ϣ�����������))
	"ImgCorner"			  , 75, 1, 0x00,    //����ǵ�
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

inline time_t ngcc_timestr_to_utc(const char* tstr)
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
	/*����xml�ļ�*/
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

	/*��ȡxml�ļ���д��json*/
	writer.StartObject();

	/*��Ʒ�����������*/
	int GaussKrugerZoneNo = -1;
	float CentralMederian = 0;
	/*������Ĳ���*/
	pugi::xml_node nodeBasicDataContent = doc.child("Metadatafile").child("BasicDataContent");
	if (nodeBasicDataContent.empty()) { return -10000; }
	{
		{
			/*����ֱ��ʺ�ͼ���С�����������*/
			pugi::xml_node node = nodeBasicDataContent.child("GroundResolution");
			if (node.empty()) { return -13; }
			writer.Key("13");
			writer.Double(node.text().as_double());

			node = nodeBasicDataContent.child("ImgSize");
			if (node.empty()) { return -16; } //������������С
			writer.Key("16");
			writer.Double(node.text().as_double());

			/*����λ�����������*/
			node = nodeBasicDataContent.child("PixelBits");
			if (node.empty()) { return -15; }  //����λ��
			{
				writer.Key("15");
				writer.Int(node.text().as_int());
			}
			/*����ת��Ϊutc*/
			node = nodeBasicDataContent.child("ProduceDate");
			if (node.empty()) { return -11; } //��Ʒ����ʱ��
			writer.Key("11");
			writer.Int64(ngcc_timestr_to_utc(node.text().as_string()));

			/*�����ֶ��ַ������*/
			node = nodeBasicDataContent.child("MetaDataFileName");
			if (node.empty()) { return -6; } //Ԫ�����ļ�����
			writer.Key("6");
			writer.String(node.text().as_string());

			node = nodeBasicDataContent.child("ProductName");
			if (node.empty()) { return -7; } //��Ʒ����
			writer.Key("7");
			writer.String(node.text().as_string());

			node = nodeBasicDataContent.child("Owner");
			if (node.empty()) { return -8; }  //��Ʒ��Ȩ��λ��
			writer.Key("8");
			writer.String(node.text().as_string());

			node = nodeBasicDataContent.child("Producer");
			if (node.empty()) { return -9; }  //��Ʒ������λ��
			writer.Key("9");
			writer.String(node.text().as_string());

			node = nodeBasicDataContent.child("Publisher");
			if (node.empty()) { return -10; }  //��Ʒ���浥λ��
			writer.Key("10");
			writer.String(node.text().as_string());

			node = nodeBasicDataContent.child("ConfidentialLevel");
			if (node.empty()) { return -12; } //�ܼ�
			writer.Key("12");
			writer.String(node.text().as_string());

			node = nodeBasicDataContent.child("ImgColorModel");
			if (node.empty()) { return -14; } //Ӱ��ɫ��ģʽ
			writer.Key("14");
			writer.String(node.text().as_string());

			node = nodeBasicDataContent.child("DataFormat");
			if (node.empty()) { return -17; } //���ݸ�ʽ
			writer.Key("17");
			writer.String(node.text().as_string());
		}
		/* MathFoundation */
		pugi::xml_node nodeMathFoundation = nodeBasicDataContent.child("MathFoundation");
		if (nodeMathFoundation.empty()) { return -10001; }
		{
			pugi::xml_node node = nodeMathFoundation.child("LongerRadius");
			if (node.empty()) { return -18; }
			writer.Key("18");
			writer.String(node.text().as_string());
			node = nodeMathFoundation.child("OblatusRatio");
			if (node.empty()) { return -19; }
			writer.Key("19");
			writer.String(node.text().as_string());
			node = nodeMathFoundation.child("GeodeticDatum");
			if (node.empty()) { return -20; }
			writer.Key("20");
			writer.String(node.text().as_string());
			node = nodeMathFoundation.child("MapProjection");
			if (node.empty()) { return -21; }
			writer.Key("21");
			writer.String(node.text().as_string());

			/*���������ߺʹ���*/
			node = nodeMathFoundation.child("CentralMederian");
			if (node.empty()) { return -22; }
			CentralMederian = node.text().as_float();
			writer.Key("22");
			writer.Int(node.text().as_int());

			node = nodeMathFoundation.child("ZoneDivisionMode");
			if (node.empty()) { return -23; }
			writer.Key("23");
			writer.String(node.text().as_string());

			node = nodeMathFoundation.child("GaussKrugerZoneNo");
			if (node.empty()) { return -24; }
			GaussKrugerZoneNo = node.text().as_int();
			if (GaussKrugerZoneNo < 0 || GaussKrugerZoneNo > 120) { return -7; }
			writer.Key("24");
			writer.Int(GaussKrugerZoneNo);

			node = nodeMathFoundation.child("CoordinationUnit");
			if (node.empty()) { return -25; }
			writer.Key("25");
			writer.String(node.text().as_string());

			node = nodeMathFoundation.child("HeightSystem");
			if (node.empty()) { return -26; }
			writer.Key("26");
			writer.String(node.text().as_string());
			node = nodeMathFoundation.child("HeightDatum");
			if (node.empty()) { return -27; }
			writer.Key("27");
			writer.String(node.text().as_string());

		}
		/* ����Χ���ҳ��������������������Сֵ�����������ʹ�� */
		pugi::xml_node nodeImgRange = nodeBasicDataContent.child("ImgRange");
		if (nodeImgRange.empty()) { return -10002; }
		{
			pugi::xml_node cornernode;
			double cornerXY[8];
			/* ���� ���� ���� ���� */
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

			/* ԭʼͶӰ������Ч��Χ�ǵ� */
			char buffer[1024];
			int len = sprintf(buffer, "[%g %g,%g %g,%g %e,%g %g]", cornerXY[0], cornerXY[4],
				cornerXY[1], cornerXY[5], cornerXY[2], cornerXY[6], cornerXY[3], cornerXY[7]);
			writer.Key("75");
			writer.String(buffer, len);
			// ����ת��--ͶӰת��γ��
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

			/*�������귶Χ��((x0 y0,x1 y1,x2 y2,x3 y3,x0 y0))��ʽ����json*/
			len = sprintf(buffer, "((%e %e,%e %e,%e %e,%e %e,%e %e))",
				cornerXY[0], cornerXY[4], cornerXY[1], cornerXY[5], cornerXY[2], cornerXY[6],
				cornerXY[3], cornerXY[7], cornerXY[0], cornerXY[4]);
			writer.Key("5");
			writer.String(buffer, len);
		}
	}

	/*����Դ�������*/
	pugi::xml_node nodeImgSource = doc.child("Metadatafile").child("ImgSource");
	if (nodeImgSource.empty()) { return -10003; }
	{
		// ��������
		pugi::xml_node node = nodeImgSource.child("SateName");
		if (node.empty()) { return -28; }
		writer.Key("28");
		writer.String(node.text().as_string());
		// ����Ӱ��������������
		node = nodeImgSource.child("SateImgQuality");
		if (node.empty()) { return -39; }
		writer.Key("39");
		writer.String(node.text().as_string());

		pugi::xml_node nodePanBand = nodeImgSource.child("PanBand");
		//ȫɫӰ�񴫸�������
		node = nodePanBand.child("PBandSensorType");
		if (node.empty()) { return -29; }
		writer.Key("29");
		writer.String(node.text().as_string());
		//ȫɫ����Ӱ��ֱ���
		node = nodePanBand.child("SateResolution");
		if (node.empty()) { return -30; }
		writer.Key("30");
		writer.Double(node.text().as_double());
		// ȫɫ����Ӱ������
		node = nodePanBand.child("PBandOrbitCode");
		if (node.empty()) { return -31; }
		writer.Key("31");
		writer.String(node.text().as_string());
		//ȫɫ����Ӱ���ȡʱ��
		node = nodePanBand.child("PBandDate");
		if (node.empty()) { return -32; }
		writer.Key("32");
		writer.Int64(ngcc_timestr_to_utc(node.text().as_string()));

		pugi::xml_node nodeMultiBand = nodeImgSource.child("MultiBand");
		// �����Ӱ�񴫸�������
		node = nodeMultiBand.child("MultiBandSensorType");
		if (node.empty()) { return -33; }
		writer.Key("33");
		writer.String(node.text().as_string());
		// ����ײ�������
		node = nodeMultiBand.child("MultiBandNum");
		if (node.empty()) { return -34; }
		writer.Key("34");
		writer.Int(node.text().as_int());
		// ����ײ�������
		node = nodeMultiBand.child("MultiBandName");
		if (node.empty()) { return -35; }
		writer.Key("35");
		writer.String(node.text().as_string());
		//���������Ӱ��ֱ���
		node = nodeMultiBand.child("MultiBandResolution");
		if (node.empty()) { return -36; }
		writer.Key("36");
		writer.Double(node.text().as_double());
		// ���������Ӱ������
		node = nodeMultiBand.child("MultiBandOrbitCode");
		if (node.empty()) { return -37; }
		writer.Key("37");
		writer.String(node.text().as_string());
		//���������Ӱ���ȡʱ��
		node = nodeMultiBand.child("MultiBandDate");
		if (node.empty()) { return -38; }
		writer.Key("38");
		writer.Int64(ngcc_timestr_to_utc(node.text().as_string()));

	}


	/*��������������Ϣ����*/
	pugi::xml_node nodeProduceInfomation = doc.child("Metadatafile").child("ProduceInfomation");
	{
		// DEM�������
		pugi::xml_node node = nodeProduceInfomation.child("GridInterval");
		if (node.empty()) { return -40; }
		writer.Key("40");
		writer.Int64(node.text().as_llong());
		// DEM�������
		node = nodeProduceInfomation.child("DEMPrecision");
		if (node.empty()) { return -41; }
		writer.Key("41");
		writer.String(node.text().as_string());
		// ����������Դ
		node = nodeProduceInfomation.child("ControlSource");
		if (node.empty()) { return -42; }
		writer.Key("42");
		writer.String(node.text().as_string());
		// ����������ʽ
		node = nodeProduceInfomation.child("ManufactureType");
		if (node.empty()) { return -48; }
		writer.Key("48");
		writer.String(node.text().as_string());
		// ����ģ�ͱ༭���
		node = nodeProduceInfomation.child("SteroEditQuality");
		if (node.empty()) { return -49; }
		writer.Key("49");
		writer.String(node.text().as_string());
		// ����������
		node = nodeProduceInfomation.child("OrthoRectifySoftware");
		if (node.empty()) { return -50; }
		writer.Key("50");
		writer.String(node.text().as_string());
		// �ز�������
		node = nodeProduceInfomation.child("ResampleMethod");
		if (node.empty()) { return -51; }
		writer.Key("51");
		writer.String(node.text().as_string());
		// ��������ܽ�
		node = nodeProduceInfomation.child("OrthoRectifyQuality");
		if (node.empty()) { return -52; }
		writer.Key("52");
		writer.String(node.text().as_string());
		// ���������ҵԱ
		node = nodeProduceInfomation.child("OrthoRectifyName");
		if (node.empty()) { return -53; }
		writer.Key("53");
		writer.String(node.text().as_string());
		// ����������Ա
		node = nodeProduceInfomation.child("OrthoCheckName");
		if (node.empty()) { return -54; }
		writer.Key("54");
		writer.String(node.text().as_string());

		pugi::xml_node nodeImgOrientation = nodeProduceInfomation.child("ImgOrientation");
		// ���������ƽ�������
		node = nodeImgOrientation.child("SateOriXRMS");
		writer.Key("43");
		writer.Double((node.empty() || node.text().empty()) ? 0.0 : node.text().as_double());
		node = nodeImgOrientation.child("SateOriYRMS");
		writer.Key("44");
		writer.Double((node.empty() || node.text().empty()) ? 0.0 : node.text().as_double());
		node = nodeImgOrientation.child("SateOriZRMS");
		writer.Key("45");
		writer.Double((node.empty() || node.text().empty()) ? 0.0 : node.text().as_double());
		// ����������ҵԱ
		node = nodeImgOrientation.child("ATProducerName");
		if (node.empty()) { return -46; }
		writer.Key("46");
		writer.String(node.text().as_string());
		// ����������Ա
		node = nodeImgOrientation.child("ATCheckerName");
		if (node.empty()) { return -47; }
		writer.Key("47");
		writer.String(node.text().as_string());

		pugi::xml_node nodeMosaicInfo = nodeProduceInfomation.child("MosaicInfo");
		// ���ӱ����
		node = nodeMosaicInfo.child("WestMosaicMaxError");
		writer.Key("55");
		writer.Double((node.empty() || node.text().empty()) ? 0.0 : node.text().as_double());
		node = nodeMosaicInfo.child("NorthMosaicMaxError");
		writer.Key("56");
		writer.Double((node.empty() || node.text().empty()) ? 0.0 : node.text().as_double());
		node = nodeMosaicInfo.child("EastMosaicMaxError");
		writer.Key("57");
		writer.Double((node.empty() || node.text().empty()) ? 0.0 : node.text().as_double());
		node = nodeMosaicInfo.child("SouthMosaicMaxError");
		writer.Key("58");
		writer.Double((node.empty() || node.text().empty()) ? 0.0 : node.text().as_double());
		// �ӱ���������
		node = nodeMosaicInfo.child("MosaicQuality");
		if (node.empty()) { return -59; }
		writer.Key("59");
		writer.String(node.text().as_string());
		// �ӱ���ҵԱ
		node = nodeMosaicInfo.child("MosaicProducerName");
		if (node.empty()) { return -60; }
		writer.Key("60");
		writer.String(node.text().as_string());
		// �ӱ߼��Ա
		node = nodeMosaicInfo.child("MosaicCheckerName");
		if (node.empty()) { return -61; }
		writer.Key("61");
		writer.String(node.text().as_string());
		// �������׼���������
		node = nodeMosaicInfo.child("MultiBRectifyXRMS");
		writer.Key("62");
		writer.Double((node.empty() || node.text().empty()) ? 0.0 : node.text().as_double());
		node = nodeMosaicInfo.child("MultiBRectifyYRMS");
		writer.Key("63");
		writer.Double((node.empty() || node.text().empty()) ? 0.0 : node.text().as_double());

		pugi::xml_node nodeQualityCheckInfo = nodeProduceInfomation.child("QualityCheckInfo");
		// �������
		node = nodeQualityCheckInfo.child("CheckPointNum");
		writer.Key("64");
		writer.Double((node.empty() || node.text().empty()) ? 0.0 : node.text().as_double());
		// ����ƽ�������
		node = nodeQualityCheckInfo.child("CheckRMS");
		writer.Key("65");
		writer.Double((node.empty() || node.text().empty()) ? 0.0 : node.text().as_double());
		// ����������
		node = nodeQualityCheckInfo.child("CheckMAXErr");
		writer.Key("66");
		writer.Double((node.empty() || node.text().empty()) ? 0.0 : node.text().as_double());
		// Ժ��������
		node = nodeQualityCheckInfo.child("ConclusionInstitute");
		if (node.empty()) { return -67; }
		writer.Key("67");
		writer.String(node.text().as_string());
		// Ժ����鵥λ
		node = nodeQualityCheckInfo.child("InstituteCheckUnit");
		if (node.empty()) { return -68; }
		writer.Key("68");
		writer.String(node.text().as_string());
		// Ժ�������
		node = nodeQualityCheckInfo.child("InstituteCheckName");
		if (node.empty()) { return -69; }
		writer.Key("69");
		writer.String(node.text().as_string());
		// Ժ�����ʱ��
		node = nodeQualityCheckInfo.child("InstituteCheckDate");
		if (node.empty()) { return -70; }
		writer.Key("70");
		writer.Int64(ngcc_timestr_to_utc(node.text().as_string()));

		// �ּ�������
		node = nodeQualityCheckInfo.child("BureauCheckName");
		if (node.empty()) { return -71; }
		writer.Key("71");
		writer.String(node.text().as_string());
		// �ּ����յ�λ
		node = nodeQualityCheckInfo.child("BureauCheckUnit");
		if (node.empty()) { return -72; }
		writer.Key("72");
		writer.String(node.text().as_string());
		// �ּ��������
		node = nodeQualityCheckInfo.child("ConclusionBureau");
		if (node.empty()) { return -73; }
		writer.Key("73");
		writer.String(node.text().as_string());
		// �ּ�����ʱ��
		node = nodeQualityCheckInfo.child("BureauCheckDate");
		if (node.empty()) { return -74; }
		writer.Key("74");
		writer.Int64(ngcc_timestr_to_utc(node.text().as_string()));

	}

	writer.EndObject();

	out_json->assign(strbuf.GetString(), strbuf.GetSize());

	return 0;
}


#if 0
int us_read_cms_metadata_record0(const std::string & index, std::string * out_json)
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

			writer.Key("75");
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
#endif

#if 0
#ifdef _WIN32
#include <Windows.h>
inline std::string UTF8toANSI(const std::string &strUTF8)
{
	//��ȡת��Ϊ���ֽں���Ҫ�Ļ�������С���������ֽڻ�����
	UINT nLen = MultiByteToWideChar(CP_UTF8, NULL, strUTF8.data(), -1, NULL, NULL);
	std::vector<WCHAR> wszBuffer(nLen + 1);
	nLen = MultiByteToWideChar(CP_UTF8, NULL, strUTF8.data(), -1, wszBuffer.data(), nLen);
	wszBuffer[nLen] = 0;

	nLen = WideCharToMultiByte(CP_ACP, NULL, wszBuffer.data(), -1, NULL, NULL, NULL, NULL);
	std::vector<CHAR> szBuffer(nLen + 1);
	nLen = WideCharToMultiByte(CP_ACP, NULL, wszBuffer.data(), -1,
		szBuffer.data(), nLen, NULL, NULL);
	szBuffer[nLen] = 0;
	std::string strANSI(szBuffer.data(), nLen);
	//�����ڴ�
	return strANSI;
}
#else
#define UTF8toANSI(strUTF8) strUTF8
#endif // _WIN32
#endif

#if 0
int us_read_cms_metadata_record(const std::string& index, std::string* out_json)
{
	static unispace::us_spatial_reference cgcs2000("proj=longlat +ellps=GRS80 +no_defs");

	if (index.size() < 6) { return -1; }
	std::string xmlpath = index;
	xmlpath.replace(index.size() - 5, 5, "Y.XML");
	puts(xmlpath.c_str());

	pugi::xml_document doc;
	/*����xml�ļ�*/
	pugi::xml_parse_result result = doc.load_file(UTF8toANSI(xmlpath).c_str(), 116U, pugi::encoding_utf8);
	if (!result)
	{
		out_json->assign(result.description());
		return -1;
	}

	rapidjson::StringBuffer strbuf;
	strbuf.Reserve(1024);
	rapidjson::Writer<rapidjson::StringBuffer> writer(strbuf);

	/*��ȡxml�ļ���д��json*/
	writer.StartObject();

	/*��Ʒ�����������*/
	int GaussKrugerZoneNo = -1;
	pugi::xml_node nodeBasicDataContent = doc.child("Metadatafile").child("BasicDataContent");
	if (nodeBasicDataContent.empty()) { return -10000; }
	{
		{
			//����ֱ���
			pugi::xml_node node = nodeBasicDataContent.child("GroundResolution");
			if (node.empty()) { return -13; }
			writer.Key("13");
			writer.Double(node.text().as_double());

			//������������С
			node = nodeBasicDataContent.child("ImgSize");
			if (node.empty()) { return -16; }
			writer.Key("16");
			writer.Double(node.text().as_double());

			//����λ��
			node = nodeBasicDataContent.child("PixelBits");
			if (node.empty()) { return -15; }
			{
				writer.Key("15");
				writer.Int(node.text().as_int());
			}

			//��Ʒ����ʱ��
			node = nodeBasicDataContent.child("ProduceDate");
			if (node.empty()) { return -11; }
			{
				std::string s = node.text().as_string();
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

			//Ԫ�����ļ�����
			node = nodeBasicDataContent.child("MetaDataFileName");
			if (node.empty()) { return -6; }
			writer.Key("6");
			writer.String(node.text().as_string());

			//��Ʒ����
			node = nodeBasicDataContent.child("ProductName");
			if (node.empty()) { return -7; }
			writer.Key("7");
			writer.String(node.text().as_string());

			//��Ʒ��Ȩ��λ��
			node = nodeBasicDataContent.child("Owner");
			if (node.empty()) { return -8; }
			writer.Key("8");
			writer.String(node.text().as_string());

			//��Ʒ������λ��
			node = nodeBasicDataContent.child("Producer");
			if (node.empty()) { return -9; }
			writer.Key("9");
			writer.String(node.text().as_string());

			//��Ʒ���浥λ��
			node = nodeBasicDataContent.child("Publisher");
			if (node.empty()) { return -10; }
			writer.Key("10");
			writer.String(node.text().as_string());

			//�ܼ�
			node = nodeBasicDataContent.child("ConfidentialLevel");
			if (node.empty()) { return -12; }
			writer.Key("12");
			writer.String(node.text().as_string());

			//Ӱ��ɫ��ģʽ
			node = nodeBasicDataContent.child("ImgColorModel");
			if (node.empty()) { return -14; }
			writer.Key("14");
			writer.String(node.text().as_string());

			//���ݸ�ʽ
			node = nodeBasicDataContent.child("DataFormat");
			if (node.empty()) { return -17; }
			writer.Key("17");
			writer.String(node.text().as_string());
		}

		/*������Ĳ���*/
		pugi::xml_node nodeMathFoundation = doc.child("MathFoundation");
		if (nodeMathFoundation.empty()) { return -10001; }
		{
			//���򳤰뾶
			pugi::xml_node node = nodeMathFoundation.child("LongerRadius");
			if (node.empty()) { return -18; }
			writer.Key("18");
			writer.String(node.text().as_string());

			//�������
			node = nodeMathFoundation.child("OblatusRatio");
			if (node.empty()) { return -19; }
			writer.Key("19");
			writer.String(node.text().as_string());

			//�����ô�ػ�׼
			node = nodeMathFoundation.child("GeodeticDatum");
			if (node.empty()) { return -20; }
			writer.Key("20");
			writer.String(node.text().as_string());

			//��ͼͶӰ
			node = nodeMathFoundation.child("MapProjection");
			if (node.empty()) { return -21; }
			writer.Key("21");
			writer.String(node.text().as_string());

			//����������
			node = nodeMathFoundation.child("CentralMederian");
			if (node.empty()) { return -22; }
			writer.Key("22");
			writer.Int(node.text().as_int());

			//�ִ���ʽ
			node = nodeMathFoundation.child("ZoneDivisionMode");
			if (node.empty()) { return -23; }
			writer.Key("23");
			writer.String(node.text().as_string());

			//��˹ - ������ͶӰ����
			node = nodeMathFoundation.child("GaussKrugerZoneNo");
			if (node.empty()) { return -24; }
			GaussKrugerZoneNo = node.text().as_int();
			if (GaussKrugerZoneNo < 0 || GaussKrugerZoneNo > 120) { return -7; }
			writer.Key("24");
			writer.Int(GaussKrugerZoneNo);

			//���굥λ
			node = nodeMathFoundation.child("CoordinationUnit");
			if (node.empty()) { return -25; }
			writer.Key("25");
			writer.String(node.text().as_string());

			//�߳�ϵͳ��
			node = nodeMathFoundation.child("HeightSystem");
			if (node.empty()) { return -26; }
			writer.Key("26");
			writer.String(node.text().as_string());

			//�̻߳�׼
			node = nodeMathFoundation.child("HeightDatum");
			if (node.empty()) { return -27; }
			writer.Key("27");
			writer.String(node.text().as_string());

		}

		/* ����Χ���ҳ��������������������Сֵ�����������ʹ�� */
		pugi::xml_node nodeImgRange = doc.child("ImgRange");
		if (nodeImgRange.empty()) { return -10002; }
		{
			pugi::xml_node cornernode;
			double cornerXY[8];
			/* ���� ���� ���� ���� */
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

			/* ԭʼͶӰ������Ч��Χ�ǵ� */
			char buffer[1024];
			int len = sprintf(buffer, "[%g %g,%g %g,%g %e,%g %g]", cornerXY[0], cornerXY[4],
				cornerXY[1], cornerXY[5], cornerXY[2], cornerXY[6], cornerXY[3], cornerXY[7]);
			writer.Key("75");
			writer.String(buffer, len);
			// ����ת��--ͶӰת��γ��
			int32_t false_easting = 500000;
			if (cornerXY[0] > 10000000.0) {
				false_easting += GaussKrugerZoneNo * 1000000;
			}
			sprintf(buffer, "proj=tmerc +lat_0=0 +lon_0=120 +k=1 +x_0=%d +y_0=0 +ellps=GRS80 +units=m +no_defs", false_easting);
			unispace::us_spatial_reference GKProj(buffer);

			unispace::us_spatial_reference::transform(GKProj, cgcs2000, 4, 1,
				&cornerXY[0], &cornerXY[4], NULL);

			writer.Key("1");
			writer.Double(_US_MAX(_US_MAX(cornerXY[0], cornerXY[1]), _US_MAX(cornerXY[2], cornerXY[3])));

			writer.Key("2");
			writer.Double(_US_MIN(_US_MIN(cornerXY[0], cornerXY[1]), _US_MIN(cornerXY[2], cornerXY[3])));

			writer.Key("3");
			writer.Double(_US_MAX(_US_MAX(cornerXY[4], cornerXY[5]), _US_MAX(cornerXY[6], cornerXY[7])));

			writer.Key("4");
			writer.Double(_US_MIN(_US_MIN(cornerXY[4], cornerXY[5]), _US_MIN(cornerXY[6], cornerXY[7])));

			/*�������귶Χ��((x0 y0,x1 y1,x2 y2,x3 y3,x0 y0))��ʽ����json*/
			len = sprintf(buffer, "((%e %e,%e %e,%e %e,%e %e,%e %e))",
				cornerXY[0], cornerXY[4], cornerXY[1], cornerXY[5], cornerXY[2], cornerXY[6],
				cornerXY[3], cornerXY[7], cornerXY[0], cornerXY[4]);
			writer.Key("5");
			writer.String(buffer, len);
		}
	}


	/*����Դ�������*/
	pugi::xml_node nodeImgSource = doc.child("Metadatafile").child("ImgSource");
	if (nodeImgSource.empty()) { return -10003; }
	{
		{
			//��������
			pugi::xml_node node = nodeImgSource.child("SateName");
			if (node.empty()) { return -28; }
			writer.Key("28");
			writer.String(node.text().as_string());
		}

		pugi::xml_node nodePanBand = nodeImgSource.child("PanBand");
		if (nodePanBand.empty()) { return -10004; }
		{
			//ȫɫӰ�񴫸�������
			pugi::xml_node node = nodePanBand.child("PBandSensorType");
			if (node.empty()) { return -29; }
			writer.Key("29");
			writer.String(node.text().as_string());

			//ȫɫ����Ӱ��ֱ���
			node = nodePanBand.child("SateResolution");
			if (node.empty()) { return -30; }
			writer.Key("30");
			writer.Double(node.text().as_double());

			//ȫɫ����Ӱ������
			node = nodePanBand.child("PbandOrbitCode");
			if (node.empty()) { return -31; }
			writer.Key("31");
			writer.String(node.text().as_string());

			//ȫɫ����Ӱ���ȡʱ��
			node = nodePanBand.child("PbandDate");
			if (node.empty()) { return -32; }
			{
				std::string s = node.text().as_string();
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
		}

		pugi::xml_node nodeMultiBand = nodeImgSource.child("MultiBand");
		if (nodeMultiBand.empty()) { return -10005; }
		{
			//�����Ӱ�񴫸�������
			pugi::xml_node node = nodeMultiBand.child("MultiBandSensorType");
			if (nodePanBand.empty()) { return -33; }
			writer.Key("33");
			writer.String(node.text().as_string());

			//����ײ�������
			node = nodeMultiBand.child("MultiBandNum");
			if (nodePanBand.empty()) { return -34; }
			writer.Key("34");
			writer.Int(node.text().as_int());

			//����ײ�������
			node = nodeMultiBand.child("MultiBandName");
			if (nodePanBand.empty()) { return -35; }
			writer.Key("35");
			writer.String(node.text().as_string());

			//���������Ӱ��ֱ���
			node = nodeMultiBand.child("MultiBandResolution");
			if (nodePanBand.empty()) { return -36; }
			writer.Key("36");
			writer.Double(node.text().as_double());

			//���������Ӱ������
			node = nodeMultiBand.child("MultiBandOrbitCode");
			if (nodePanBand.empty()) { return -37; }
			writer.Key("37");
			writer.String(node.text().as_string());

			//���������Ӱ���ȡʱ��
			node = nodeMultiBand.child("MultiBandDate");
			if (nodePanBand.empty()) { return -38; }
			{
				std::string s = node.text().as_string();
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
		}

		{
			//����Ӱ��������������
			pugi::xml_node nodeMultiBand = nodeImgSource.child("SateImgQuality");
			if (nodeMultiBand.empty()) { return -39; }
			writer.Key("39");
			writer.String(nodeMultiBand.text().as_string());
		}
	}

	/*��������������Ϣ����*/
	pugi::xml_node nodeProduceInfomation = doc.child("Metadatafile").child("ProduceInfomation");
	if (nodeProduceInfomation.empty()) { return -10006; }
	{
		{
			//DEM�������
			pugi::xml_node node = nodeProduceInfomation.child("GridInterval");
			if (node.empty()) { return -40; }
			writer.Key("40");
			writer.Int64(node.text().as_llong());

			//DEM�������
			node = nodeProduceInfomation.child("DEMPrecision");
			if (node.empty()) { return -41; }
			writer.Key("41");
			writer.String(node.text().as_string());

			//����������Դ
			node = nodeProduceInfomation.child("ControlSource");
			if (node.empty()) { return -42; }
			writer.Key("42");
			writer.String(node.text().as_string());
		}

		pugi::xml_node nodeImgOrientation = nodeProduceInfomation.child("ImgOrientation");
		if (nodeImgOrientation.empty()) { return -10007; }
		{
			//���������ƽ�������(X)
			pugi::xml_node node = nodeImgOrientation.child("SateOriXRMS");
			if (node.empty()) { return -43; }
			writer.Key("43");
			writer.Double(node.text().as_double());

			//���������ƽ�������(Y)
			node = nodeImgOrientation.child("SateOriYRMS");
			if (node.empty()) { return -44; }
			writer.Key("44");
			writer.Double(node.text().as_double());

			//���������߳������(Z)
			node = nodeImgOrientation.child("SateOriZRMS");
			if (node.empty()) { return -45; }
			writer.Key("45");
			writer.Double(node.text().as_double());

			//����������ҵԱ
			node = nodeImgOrientation.child("ATProducerName");
			if (node.empty()) { return -46; }
			writer.Key("46");
			writer.String(node.text().as_string());

			//����������Ա
			node = nodeImgOrientation.child("ATCheckerName");
			if (node.empty()) { return -47; }
			writer.Key("47");
			writer.String(node.text().as_string());
		}

		{
			//����������ʽ
			pugi::xml_node node = nodeProduceInfomation.child("ManufactureType");
			if (node.empty()) { return -48; }
			writer.Key("48");
			writer.String(node.text().as_string());

			//����ģ�ͱ༭���
			node = nodeProduceInfomation.child("SteroEditQuality");
			if (node.empty()) { return -49; }
			writer.Key("49");
			writer.String(node.text().as_string());

			//����������
			node = nodeProduceInfomation.child("OrthoRectifySoftware");
			if (node.empty()) { return -50; }
			writer.Key("50");
			writer.String(node.text().as_string());

			//�ز�������
			node = nodeProduceInfomation.child("ResampleMethod");
			if (node.empty()) { return -51; }
			writer.Key("51");
			writer.String(node.text().as_string());

			//��������ܽ�
			node = nodeProduceInfomation.child("OrthoRectifyQuality");
			if (node.empty()) { return -52; }
			writer.Key("52");
			writer.String(node.text().as_string());

			//���������ҵԱ
			node = nodeProduceInfomation.child("OrthoRectifyName");
			if (node.empty()) { return -53; }
			writer.Key("53");
			writer.String(node.text().as_string());

			//����������Ա
			node = nodeProduceInfomation.child("OrthoCheckName");
			if (node.empty()) { return -54; }
			writer.Key("54");
			writer.String(node.text().as_string());
		}

		pugi::xml_node nodeMosaicInfo = nodeProduceInfomation.child("MosaicInfo");
		if (nodeMosaicInfo.empty()) { return -10008; }
		{
			//�������ӱ߲�
			pugi::xml_node node = nodeMosaicInfo.child("WestMosaicMaxError");
			if (node.empty()) { return -55; }
			writer.Key("55");
			writer.Double(node.text().as_double());

			//�������ӱ߲�
			node = nodeMosaicInfo.child("NorthMosaicMaxError");
			if (node.empty()) { return -56; }
			writer.Key("56");
			writer.Double(node.text().as_double());

			//�������ӱ߲�
			node = nodeMosaicInfo.child("EastMosaicMaxError");
			if (node.empty()) { return -57; }
			writer.Key("57");
			writer.Double(node.text().as_double());

			//�ϱ����ӱ߲�
			node = nodeMosaicInfo.child("SouthMosaicMaxError");
			if (node.empty()) { return -58; }
			writer.Key("58");
			writer.Double(node.text().as_double());

			//�ӱ���������
			node = nodeMosaicInfo.child("MosaicQuality");
			if (node.empty()) { return -59; }
			writer.Key("59");
			writer.String(node.text().as_string());

			//�ӱ���ҵԱ
			node = nodeMosaicInfo.child("MosaicProducerName");
			if (node.empty()) { return -60; }
			writer.Key("60");
			writer.String(node.text().as_string());

			//�ӱ߼��Ա
			node = nodeMosaicInfo.child("MosaicCheckerName");
			if (node.empty()) { return -61; }
			writer.Key("61");
			writer.String(node.text().as_string());

			//�������׼���������(X)
			node = nodeMosaicInfo.child("MultiBRectifyXRMS");
			if (node.empty()) { return -62; }
			writer.Key("62");
			writer.Double(node.text().as_double());

			//�������׼���������(Y)
			node = nodeMosaicInfo.child("MultiBRectifyYRMS");
			if (node.empty()) { return -63; }
			writer.Key("63");
			writer.Double(node.text().as_double());
		}

		pugi::xml_node nodeQualityCheckInfo = nodeProduceInfomation.child("QualityCheckInfo");
		if (nodeQualityCheckInfo.empty()) { return -10009; }
		{
			//�������
			pugi::xml_node node = nodeQualityCheckInfo.child("CheckPointNum");
			if (node.empty()) { return -64; }
			writer.Key("64");
			writer.Int(node.text().as_int());

			//����ƽ�������
			node = nodeQualityCheckInfo.child("CheckRMS");
			if (node.empty()) { return -65; }
			writer.Key("65");
			writer.Double(node.text().as_double());

			//����������
			node = nodeQualityCheckInfo.child("CheckMAXErr");
			if (node.empty()) { return -66; }
			writer.Key("66");
			writer.Double(node.text().as_double());

			//Ժ��������
			node = nodeQualityCheckInfo.child("ConclusionInstitute");
			if (node.empty()) { return -67; }
			writer.Key("67");
			writer.String(node.text().as_string());

			//Ժ����鵥λ
			node = nodeQualityCheckInfo.child("InstituteCheckUnit");
			if (node.empty()) { return -68; }
			writer.Key("68");
			writer.String(node.text().as_string());

			//Ժ�������
			node = nodeQualityCheckInfo.child("InstituteCheckName");
			if (node.empty()) { return -69; }
			writer.Key("69");
			writer.String(node.text().as_string());

			//Ժ�����ʱ��
			node = nodeQualityCheckInfo.child("InstituteCheckDate");
			if (node.empty()) { return -70; }
			{
				std::string s = node.text().as_string();
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

			//�ּ�������
			node = nodeQualityCheckInfo.child("BureauCheckName");
			if (node.empty()) { return -71; }
			writer.Key("71");
			writer.String(node.text().as_string());

			//�ּ����յ�λ
			node = nodeQualityCheckInfo.child("BureauCheckUnit");
			if (node.empty()) { return -72; }
			writer.Key("72");
			writer.String(node.text().as_string());

			//�ּ��������
			node = nodeQualityCheckInfo.child("ConclusionBureau");
			if (node.empty()) { return -73; }
			writer.Key("73");
			writer.String(node.text().as_string());

			//�ּ�����ʱ��
			node = nodeQualityCheckInfo.child("BureauCheckDate");
			if (node.empty()) { return -74; }
			{
				std::string s = node.text().as_string();
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
		}
	}

	writer.EndObject();

	out_json->assign(strbuf.GetString(), strbuf.GetSize());

	return 0;
}
#endif

#if 0
#include "../custom_metadata_process.hpp"
#include "../parsexml/pugixml.hpp"
#include <geo_coord_transform/us_spatial_reference.hpp>
#include <time.h>
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <algorithm>
#include <vector>

static us_cms_metdata_map metadata_map_array[] =
{
	"MaxX"                , 1, 3, 0x00,     //����ΧMaxX
	"MinX"                , 2, 3, 0x00,		//����ΧMinX
	"MaxY"                , 3, 3, 0x00,		//����ΧMaxY
	"MinY"                , 4, 3, 0x00,		//����ΧMinY
	"ImgRange"            , 5, 1, 0x00,     //����Χ((���ϣ����ϣ�����������))
	"ImgCorner"			  , 75, 1, 0x00,    //����ǵ�
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

#ifdef _WIN32
#include <Windows.h>
inline std::string UTF8toANSI(const std::string &strUTF8)
{
	//��ȡת��Ϊ���ֽں���Ҫ�Ļ�������С���������ֽڻ�����
	UINT nLen = MultiByteToWideChar(CP_UTF8, NULL, strUTF8.data(), -1, NULL, NULL);
	std::vector<WCHAR> wszBuffer(nLen + 1);
	nLen = MultiByteToWideChar(CP_UTF8, NULL, strUTF8.data(), -1, wszBuffer.data(), nLen);
	wszBuffer[nLen] = 0;

	nLen = WideCharToMultiByte(CP_ACP, NULL, wszBuffer.data(), -1, NULL, NULL, NULL, NULL);
	std::vector<CHAR> szBuffer(nLen + 1);
	nLen = WideCharToMultiByte(CP_ACP, NULL, wszBuffer.data(), -1,
		szBuffer.data(), nLen, NULL, NULL);
	szBuffer[nLen] = 0;
	std::string strANSI(szBuffer.data(), nLen);
	//�����ڴ�
	return strANSI;
}
#else
#define UTF8toANSI(strUTF8) strUTF8
#endif // _WIN32


#define _US_MAX(a,b) (((a) > (b)) ? (a) : (b))
#define _US_MIN(a,b) (((a) < (b)) ? (a) : (b))


int us_read_cms_metadata_record(const std::string& index, std::string* out_json)
{
	static unispace::us_spatial_reference cgcs2000("proj=longlat +ellps=GRS80 +no_defs");

	if (index.size() < 6) { puts("index < 6"); return -1; }
	std::string xmlpath = index;
	xmlpath.replace(index.size() - 5, 5, "Y.XML");
	puts(xmlpath.c_str());

	pugi::xml_document doc;
	/*����xml�ļ�*/
	pugi::xml_parse_result result = doc.load_file(UTF8toANSI(xmlpath).c_str(), 116U, pugi::encoding_utf8);
	if (!result)
	{
		out_json->assign(result.description());
		puts("load");
		puts(UTF8toANSI(xmlpath).c_str());
		return -1;
	}

	rapidjson::StringBuffer strbuf;
	strbuf.Reserve(1024);
	rapidjson::Writer<rapidjson::StringBuffer> writer(strbuf);

	/*��ȡxml�ļ���д��json*/
	writer.StartObject();

	/*��Ʒ�����������*/
	int GaussKrugerZoneNo = -1;
	pugi::xml_node nodeBasicDataContent = doc.child("Metadatafile").child("BasicDataContent");
	if (nodeBasicDataContent.empty()) { puts("10000"); return -10000; }
	{
		{
			//����ֱ���
			pugi::xml_node node = nodeBasicDataContent.child("GroundResolution");
			if (node.empty()) { puts("13"); return -13; }
			writer.Key("13");
			writer.Double(node.text().as_double());

			//������������С
			node = nodeBasicDataContent.child("ImgSize");
			if (node.empty()) { puts("16"); return -16; }
			writer.Key("16");
			writer.Double(node.text().as_double());

			//����λ��
			node = nodeBasicDataContent.child("PixelBits");
			if (node.empty()) { puts("15"); return -15; }
			{
				writer.Key("15");
				writer.Int(node.text().as_int());
			}

			//��Ʒ����ʱ��
			node = nodeBasicDataContent.child("ProduceDate");
			if (node.empty()) { puts("11"); return -11; }
			{
				std::string s = node.text().as_string();
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

			//Ԫ�����ļ�����
			node = nodeBasicDataContent.child("MetaDataFileName");
			if (node.empty()) { puts("6"); return -6; }
			writer.Key("6");
			writer.String(node.text().as_string());

			//��Ʒ����
			node = nodeBasicDataContent.child("ProductName");
			if (node.empty()) { puts("7"); return -7; }
			writer.Key("7");
			writer.String(node.text().as_string());

			//��Ʒ��Ȩ��λ��
			node = nodeBasicDataContent.child("Owner");
			if (node.empty()) { puts("8"); return -8; }
			writer.Key("8");
			writer.String(node.text().as_string());

			//��Ʒ������λ��
			node = nodeBasicDataContent.child("Producer");
			if (node.empty()) { puts("9"); return -9; }
			writer.Key("9");
			writer.String(node.text().as_string());

			//��Ʒ���浥λ��
			node = nodeBasicDataContent.child("Publisher");
			if (node.empty()) { puts("10"); return -10; }
			writer.Key("10");
			writer.String(node.text().as_string());

			//�ܼ�
			node = nodeBasicDataContent.child("ConfidentialLevel");
			if (node.empty()) { puts("12"); return -12; }
			writer.Key("12");
			writer.String(node.text().as_string());

			//Ӱ��ɫ��ģʽ
			node = nodeBasicDataContent.child("ImgColorModel");
			if (node.empty()) { puts("14"); return -14; }
			writer.Key("14");
			writer.String(node.text().as_string());

			//���ݸ�ʽ
			node = nodeBasicDataContent.child("DataFormat");
			if (node.empty()) { puts("17"); return -17; }
			writer.Key("17");
			writer.String(node.text().as_string());
		}

		/*������Ĳ���*/
		pugi::xml_node nodeMathFoundation = nodeBasicDataContent.child("MathFoundation");
		if (nodeMathFoundation.empty()) { puts("10001"); return -10001; }
		{
			//���򳤰뾶
			pugi::xml_node node = nodeMathFoundation.child("LongerRadius");
			if (node.empty()) { puts("18"); return -18; }
			writer.Key("18");
			writer.String(node.text().as_string());

			//�������
			node = nodeMathFoundation.child("OblatusRatio");
			if (node.empty()) { puts("19"); return -19; }
			writer.Key("19");
			writer.String(node.text().as_string());

			//�����ô�ػ�׼
			node = nodeMathFoundation.child("GeodeticDatum");
			if (node.empty()) { puts("20"); return -20; }
			writer.Key("20");
			writer.String(node.text().as_string());

			//��ͼͶӰ
			node = nodeMathFoundation.child("MapProjection");
			if (node.empty()) { puts("21"); return -21; }
			writer.Key("21");
			writer.String(node.text().as_string());

			//����������
			node = nodeMathFoundation.child("CentralMederian");
			if (node.empty()) { puts("22"); return -22; }
			writer.Key("22");
			writer.Int(node.text().as_int());

			//�ִ���ʽ
			node = nodeMathFoundation.child("ZoneDivisionMode");
			if (node.empty()) { puts("23"); return -23; }
			writer.Key("23");
			writer.String(node.text().as_string());

			//��˹ - ������ͶӰ����
			node = nodeMathFoundation.child("GaussKrugerZoneNo");
			if (node.empty()) { puts("24"); return -24; }
			GaussKrugerZoneNo = node.text().as_int();
			if (GaussKrugerZoneNo < 0 || GaussKrugerZoneNo > 120) { puts("7777"); return -7; }
			writer.Key("24");
			writer.Int(GaussKrugerZoneNo);

			//���굥λ
			node = nodeMathFoundation.child("CoordinationUnit");
			if (node.empty()) { puts("25"); return -25; }
			writer.Key("25");
			writer.String(node.text().as_string());

			//�߳�ϵͳ��
			node = nodeMathFoundation.child("HeightSystem");
			if (node.empty()) { puts("26"); return -26; }
			writer.Key("26");
			writer.String(node.text().as_string());

			//�̻߳�׼
			node = nodeMathFoundation.child("HeightDatum");
			if (node.empty()) { puts("27"); return -27; }
			writer.Key("27");
			writer.String(node.text().as_string());

		}

		/* ����Χ���ҳ��������������������Сֵ�����������ʹ�� */
		pugi::xml_node nodeImgRange = nodeBasicDataContent.child("ImgRange");
		if (nodeImgRange.empty()) { puts("10002"); return -10002; }
		{
			pugi::xml_node cornernode;
			double cornerXY[8];
			/* ���� ���� ���� ���� */
			cornernode = nodeImgRange.child("SouthWestAbs");
			if (cornernode.empty()) { puts("5"); return -5; }
			cornerXY[0] = cornernode.text().as_double();
			cornernode = nodeImgRange.child("SouthEastAbs");
			if (cornernode.empty()) { puts("5"); return -5; }
			cornerXY[1] = cornernode.text().as_double();
			cornernode = nodeImgRange.child("NorthEastAbs");
			if (cornernode.empty()) { puts("5"); return -5; }
			cornerXY[2] = cornernode.text().as_double();
			cornernode = nodeImgRange.child("NorthWestAbs");
			if (cornernode.empty()) { puts("5"); return -5; }
			cornerXY[3] = cornernode.text().as_double();

			cornernode = nodeImgRange.child("SouthWestOrd");
			if (cornernode.empty()) { puts("5"); return -5; }
			cornerXY[4] = cornernode.text().as_double();
			cornernode = nodeImgRange.child("SouthEastOrd");
			if (cornernode.empty()) { puts("5"); return -5; }
			cornerXY[5] = cornernode.text().as_double();
			cornernode = nodeImgRange.child("NorthEastOrd");
			if (cornernode.empty()) { puts("5"); return -5; }
			cornerXY[6] = cornernode.text().as_double();
			cornernode = nodeImgRange.child("NorthWestOrd");
			if (cornernode.empty()) { puts("5"); return -5; }
			cornerXY[7] = cornernode.text().as_double();

			/* ԭʼͶӰ������Ч��Χ�ǵ� */
			char buffer[1024];
			int len = sprintf(buffer, "[%g %g,%g %g,%g %e,%g %g]", cornerXY[0], cornerXY[4],
				cornerXY[1], cornerXY[5], cornerXY[2], cornerXY[6], cornerXY[3], cornerXY[7]);
			writer.Key("75");
			writer.String(buffer, len);
			// ����ת��--ͶӰת��γ��
			int32_t false_easting = 500000;
			if (cornerXY[0] > 10000000.0) {
				false_easting += GaussKrugerZoneNo * 1000000;
			}
			sprintf(buffer, "proj=tmerc +lat_0=0 +lon_0=120 +k=1 +x_0=%d +y_0=0 +ellps=GRS80 +units=m +no_defs", false_easting);
			unispace::us_spatial_reference GKProj(buffer);

			unispace::us_spatial_reference::transform(GKProj, cgcs2000, 4, 1,
				&cornerXY[0], &cornerXY[4], NULL);

			writer.Key("1");
			writer.Double(_US_MAX(_US_MAX(cornerXY[0], cornerXY[1]), _US_MAX(cornerXY[2], cornerXY[3])));

			writer.Key("2");
			writer.Double(_US_MIN(_US_MIN(cornerXY[0], cornerXY[1]), _US_MIN(cornerXY[2], cornerXY[3])));

			writer.Key("3");
			writer.Double(_US_MAX(_US_MAX(cornerXY[4], cornerXY[5]), _US_MAX(cornerXY[6], cornerXY[7])));

			writer.Key("4");
			writer.Double(_US_MIN(_US_MIN(cornerXY[4], cornerXY[5]), _US_MIN(cornerXY[6], cornerXY[7])));

			/*�������귶Χ��((x0 y0,x1 y1,x2 y2,x3 y3,x0 y0))��ʽ����json*/
			len = sprintf(buffer, "((%e %e,%e %e,%e %e,%e %e,%e %e))",
				cornerXY[0], cornerXY[4], cornerXY[1], cornerXY[5], cornerXY[2], cornerXY[6],
				cornerXY[3], cornerXY[7], cornerXY[0], cornerXY[4]);
			writer.Key("5");
			writer.String(buffer, len);
		}
	}


	/*����Դ�������*/
	pugi::xml_node nodeImgSource = doc.child("Metadatafile").child("ImgSource");
	if (nodeImgSource.empty()) { puts("10003"); return -10003; }
	{
		{
			//��������
			pugi::xml_node node = nodeImgSource.child("SateName");
			if (node.empty()) { puts("28"); return -28; }
			writer.Key("28");
			writer.String(node.text().as_string());
		}

		pugi::xml_node nodePanBand = nodeImgSource.child("PanBand");
		if (nodePanBand.empty()) { puts("10004"); return -10004; }
		{
			//ȫɫӰ�񴫸�������
			pugi::xml_node node = nodePanBand.child("PBandSensorType");
			if (node.empty()) { puts("29"); return -29; }
			writer.Key("29");
			writer.String(node.text().as_string());

			//ȫɫ����Ӱ��ֱ���
			node = nodePanBand.child("SateResolution");
			if (node.empty()) { puts("30"); return -30; }
			writer.Key("30");
			writer.Double(node.text().as_double());

			//ȫɫ����Ӱ������
			node = nodePanBand.child("PBandOrbitCode");
			if (node.empty()) { puts("31"); return -31; }
			writer.Key("31");
			writer.String(node.text().as_string());

			//ȫɫ����Ӱ���ȡʱ��
			node = nodePanBand.child("PBandDate");
			if (node.empty()) { puts("32"); return -32; }
			{
				std::string s = node.text().as_string();
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
		}

		pugi::xml_node nodeMultiBand = nodeImgSource.child("MultiBand");
		if (nodeMultiBand.empty()) { puts("10005"); return -10005; }
		{
			//�����Ӱ�񴫸�������
			pugi::xml_node node = nodeMultiBand.child("MultiBandSensorType");
			if (nodePanBand.empty()) { puts("33"); return -33; }
			writer.Key("33");
			writer.String(node.text().as_string());

			//����ײ�������
			node = nodeMultiBand.child("MultiBandNum");
			if (nodePanBand.empty()) { puts("34"); return -34; }
			writer.Key("34");
			writer.Int(node.text().as_int());

			//����ײ�������
			node = nodeMultiBand.child("MultiBandName");
			if (nodePanBand.empty()) { puts("35"); return -35; }
			writer.Key("35");
			writer.String(node.text().as_string());

			//���������Ӱ��ֱ���
			node = nodeMultiBand.child("MultiBandResolution");
			if (nodePanBand.empty()) { puts("36"); return -36; }
			writer.Key("36");
			writer.Double(node.text().as_double());

			//���������Ӱ������
			node = nodeMultiBand.child("MultiBandOrbitCode");
			if (nodePanBand.empty()) { puts("37"); return -37; }
			writer.Key("37");
			writer.String(node.text().as_string());

			//���������Ӱ���ȡʱ��
			node = nodeMultiBand.child("MultiBandDate");
			if (nodePanBand.empty()) { puts("38"); return -38; }
			{
				std::string s = node.text().as_string();
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
		}

		{
			//����Ӱ��������������
			pugi::xml_node nodeMultiBand = nodeImgSource.child("SateImgQuality");
			if (nodeMultiBand.empty()) { puts("39"); return -39; }
			writer.Key("39");
			writer.String(nodeMultiBand.text().as_string());
		}
	}

	/*��������������Ϣ����*/
	pugi::xml_node nodeProduceInfomation = doc.child("Metadatafile").child("ProduceInfomation");
	if (nodeProduceInfomation.empty()) { puts("10006"); return -10006; }
	{
		{
			//DEM�������
			pugi::xml_node node = nodeProduceInfomation.child("GridInterval");
			if (node.empty()) { puts("40"); return -40; }
			writer.Key("40");
			writer.Int64(node.text().as_llong());

			//DEM�������
			node = nodeProduceInfomation.child("DEMPrecision");
			if (node.empty()) { puts("41"); return -41; }
			writer.Key("41");
			writer.String(node.text().as_string());

			//����������Դ
			node = nodeProduceInfomation.child("ControlSource");
			if (node.empty()) { puts("42"); return -42; }
			writer.Key("42");
			writer.String(node.text().as_string());
		}

		pugi::xml_node nodeImgOrientation = nodeProduceInfomation.child("ImgOrientation");
		if (nodeImgOrientation.empty()) { puts("10007"); return -10007; }
		{
			//���������ƽ�������(X)
			pugi::xml_node node = nodeImgOrientation.child("SateOriXRMS");
			if (node.empty()) { puts("43"); return -43; }
			writer.Key("43");
			writer.Double(node.text().as_double());

			//���������ƽ�������(Y)
			node = nodeImgOrientation.child("SateOriYRMS");
			if (node.empty()) { puts("44"); return -44; }
			writer.Key("44");
			writer.Double(node.text().as_double());

			//���������߳������(Z)
			node = nodeImgOrientation.child("SateOriZRMS");
			if (node.empty()) { puts("45"); return -45; }
			writer.Key("45");
			writer.Double(node.text().as_double());

			//����������ҵԱ
			node = nodeImgOrientation.child("ATProducerName");
			if (node.empty()) { puts("46"); return -46; }
			writer.Key("46");
			writer.String(node.text().as_string());

			//����������Ա
			node = nodeImgOrientation.child("ATCheckerName");
			if (node.empty()) { puts("47"); return -47; }
			writer.Key("47");
			writer.String(node.text().as_string());
		}

		{
			//����������ʽ
			pugi::xml_node node = nodeProduceInfomation.child("ManufactureType");
			if (node.empty()) { puts("48"); return -48; }
			writer.Key("48");
			writer.String(node.text().as_string());

			//����ģ�ͱ༭���
			node = nodeProduceInfomation.child("SteroEditQuality");
			if (node.empty()) { puts("49"); return -49; }
			writer.Key("49");
			writer.String(node.text().as_string());

			//����������
			node = nodeProduceInfomation.child("OrthoRectifySoftware");
			if (node.empty()) { puts("50"); return -50; }
			writer.Key("50");
			writer.String(node.text().as_string());

			//�ز�������
			node = nodeProduceInfomation.child("ResampleMethod");
			if (node.empty()) { puts("51"); return -51; }
			writer.Key("51");
			writer.String(node.text().as_string());

			//��������ܽ�
			node = nodeProduceInfomation.child("OrthoRectifyQuality");
			if (node.empty()) { puts("52"); return -52; }
			writer.Key("52");
			writer.String(node.text().as_string());

			//���������ҵԱ
			node = nodeProduceInfomation.child("OrthoRectifyName");
			if (node.empty()) { puts("53"); return -53; }
			writer.Key("53");
			writer.String(node.text().as_string());

			//����������Ա
			node = nodeProduceInfomation.child("OrthoCheckName");
			if (node.empty()) { puts("54"); return -54; }
			writer.Key("54");
			writer.String(node.text().as_string());
		}

		pugi::xml_node nodeMosaicInfo = nodeProduceInfomation.child("MosaicInfo");
		if (nodeMosaicInfo.empty()) { puts("10008"); return -10008; }
		{
			//�������ӱ߲�
			pugi::xml_node node = nodeMosaicInfo.child("WestMosaicMaxError");
			if (node.empty()) { puts("55"); return -55; }
			writer.Key("55");
			writer.Double(node.text().as_double());

			//�������ӱ߲�
			node = nodeMosaicInfo.child("NorthMosaicMaxError");
			if (node.empty()) { puts("56"); return -56; }
			writer.Key("56");
			writer.Double(node.text().as_double());

			//�������ӱ߲�
			node = nodeMosaicInfo.child("EastMosaicMaxError");
			if (node.empty()) { puts("57"); return -57; }
			writer.Key("57");
			writer.Double(node.text().as_double());

			//�ϱ����ӱ߲�
			node = nodeMosaicInfo.child("SouthMosaicMaxError");
			if (node.empty()) { puts("58"); return -58; }
			writer.Key("58");
			writer.Double(node.text().as_double());

			//�ӱ���������
			node = nodeMosaicInfo.child("MosaicQuality");
			if (node.empty()) { puts("59"); return -59; }
			writer.Key("59");
			writer.String(node.text().as_string());

			//�ӱ���ҵԱ
			node = nodeMosaicInfo.child("MosaicProducerName");
			if (node.empty()) { puts("60"); return -60; }
			writer.Key("60");
			writer.String(node.text().as_string());

			//�ӱ߼��Ա
			node = nodeMosaicInfo.child("MosaicCheckerName");
			if (node.empty()) { puts("61"); return -61; }
			writer.Key("61");
			writer.String(node.text().as_string());

			//�������׼���������(X)
			node = nodeMosaicInfo.child("MultiBRectifyXRMS");
			if (node.empty()) { puts("62"); return -62; }
			writer.Key("62");
			writer.Double(node.text().as_double());

			//�������׼���������(Y)
			node = nodeMosaicInfo.child("MultiBRectifyYRMS");
			if (node.empty()) { puts("63"); return -63; }
			writer.Key("63");
			writer.Double(node.text().as_double());
		}

		pugi::xml_node nodeQualityCheckInfo = nodeProduceInfomation.child("QualityCheckInfo");
		if (nodeQualityCheckInfo.empty()) { puts("10009"); return -10009; }
		{
			//�������
			pugi::xml_node node = nodeQualityCheckInfo.child("CheckPointNum");
			if (node.empty()) { puts("64"); return -64; }
			writer.Key("64");
			writer.Int(node.text().as_int());

			//����ƽ�������
			node = nodeQualityCheckInfo.child("CheckRMS");
			if (node.empty()) { puts("65"); return -65; }
			writer.Key("65");
			writer.Double(node.text().as_double());

			//����������
			node = nodeQualityCheckInfo.child("CheckMAXErr");
			if (node.empty()) { puts("66"); return -66; }
			writer.Key("66");
			writer.Double(node.text().as_double());

			//Ժ��������
			node = nodeQualityCheckInfo.child("ConclusionInstitute");
			if (node.empty()) { puts("67"); return -67; }
			writer.Key("67");
			writer.String(node.text().as_string());

			//Ժ����鵥λ
			node = nodeQualityCheckInfo.child("InstituteCheckUnit");
			if (node.empty()) { puts("68"); return -68; }
			writer.Key("68");
			writer.String(node.text().as_string());

			//Ժ�������
			node = nodeQualityCheckInfo.child("InstituteCheckName");
			if (node.empty()) { puts("69"); return -69; }
			writer.Key("69");
			writer.String(node.text().as_string());

			//Ժ�����ʱ��
			node = nodeQualityCheckInfo.child("InstituteCheckDate");
			if (node.empty()) { puts("70"); return -70; }
			{
				std::string s = node.text().as_string();
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

			//�ּ�������
			node = nodeQualityCheckInfo.child("BureauCheckName");
			if (node.empty()) { puts("71"); return -71; }
			writer.Key("71");
			writer.String(node.text().as_string());

			//�ּ����յ�λ
			node = nodeQualityCheckInfo.child("BureauCheckUnit");
			if (node.empty()) { puts("72"); return -72; }
			writer.Key("72");
			writer.String(node.text().as_string());

			//�ּ��������
			node = nodeQualityCheckInfo.child("ConclusionBureau");
			if (node.empty()) { puts("73"); return -73; }
			writer.Key("73");
			writer.String(node.text().as_string());

			//�ּ�����ʱ��
			node = nodeQualityCheckInfo.child("BureauCheckDate");
			if (node.empty()) { puts("74"); return -74; }
			{
				std::string s = node.text().as_string();
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
		}
	}

	writer.EndObject();

	out_json->assign(strbuf.GetString(), strbuf.GetSize());

	return 0;
}



#endif


#endif