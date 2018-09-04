#include "filed.h"
#include "pugixml.hpp"
#include "usMysql.h"
#include <regex>

Filed::Filed()
{
	if (!m_filedNames.empty())
	{
		m_filedNames.clear();
	}
	m_filedNames.reserve(100);
	m_filedNames.push_back("MetaDataFileName");
	m_filedNames.push_back("ProductName");
	m_filedNames.push_back("Owner");
	m_filedNames.push_back("Producer");
	m_filedNames.push_back("Publisher");
	m_filedNames.push_back("ProduceDate");
	m_filedNames.push_back("ConfidentialLevel");
	m_filedNames.push_back("GroundResolution");
	m_filedNames.push_back("ImgColorModel");
	m_filedNames.push_back("PixelBits");
	m_filedNames.push_back("ImgSize");
	m_filedNames.push_back("DataFormat");
	/*m_filedNames.push_back("SouthWestAbs");
	m_filedNames.push_back("SouthWestOrd");
	m_filedNames.push_back("NorthWestAbs");
	m_filedNames.push_back("NorthWestOrd");
	m_filedNames.push_back("NorthEastAbs");
	m_filedNames.push_back("NorthEastOrd");
	m_filedNames.push_back("SouthEastAbs");
	m_filedNames.push_back("SouthEastOrd");*/
	m_filedNames.push_back("Range");
	m_filedNames.push_back("xRangMax");
	m_filedNames.push_back("xRangMix");
	m_filedNames.push_back("yRangMan");
	m_filedNames.push_back("yRangMix");

	m_filedNames.push_back("LongerRadius");
	m_filedNames.push_back("OblatusRatio");
	m_filedNames.push_back("GeodeticDatum");
	m_filedNames.push_back("MapProjection");
	m_filedNames.push_back("CentralMederian");
	m_filedNames.push_back("ZoneDivisionMode");
	m_filedNames.push_back("GaussKrugerZoneNo");
	m_filedNames.push_back("CoordinationUnit");
	m_filedNames.push_back("HeightSystem");
	m_filedNames.push_back("HeightDatum");
	m_filedNames.push_back("SateName");
	m_filedNames.push_back("PBandSensorType");
	m_filedNames.push_back("SateResolution");
	m_filedNames.push_back("PbandOrbitCode");
	m_filedNames.push_back("PbandDate");
	m_filedNames.push_back("MultiBandSensorType");
	m_filedNames.push_back("MultiBandNum");
	m_filedNames.push_back("MultiBandName");
	m_filedNames.push_back("MultiBandResolution");
	m_filedNames.push_back("MultiBandOrbitCode");
	m_filedNames.push_back("MultiBandDate");
	m_filedNames.push_back("SateImgQuality");
	m_filedNames.push_back("GridInterval");
	m_filedNames.push_back("DEMPrecision");
	m_filedNames.push_back("ControlSource");
	m_filedNames.push_back("SateOriXRMS");
	m_filedNames.push_back("SateOriYRMS");
	m_filedNames.push_back("SateOriZRMS");
	m_filedNames.push_back("ATProducerName");
	m_filedNames.push_back("ATCheckerName");
	m_filedNames.push_back("ManufactureType");
	m_filedNames.push_back("SteroEditQuality");
	m_filedNames.push_back("OrthoRectifySoftWare");
	m_filedNames.push_back("ResampleMethod");
	m_filedNames.push_back("OrthoRectifyQuality");
	m_filedNames.push_back("OrthoRectifyName");
	m_filedNames.push_back("OrthoCheckName");
	m_filedNames.push_back("WestMosaicMaxError");
	m_filedNames.push_back("NorthMosaicMaxError");
	m_filedNames.push_back("EastMosaicMaxError");
	m_filedNames.push_back("SouthMosaicMaxError");
	m_filedNames.push_back("MosaicQuality");
	m_filedNames.push_back("MosaicProducerName");
	m_filedNames.push_back("MosaicCheckerName");
	m_filedNames.push_back("MultiBRectifyXRMS");
	m_filedNames.push_back("MultiBRectifyYRMS");
	m_filedNames.push_back("CheckPointNum");
	m_filedNames.push_back("CheckRMS");
	m_filedNames.push_back("CheckMAXErr");
	m_filedNames.push_back("ConclusionInstitute");
	m_filedNames.push_back("InstituteCheckUnit");
	m_filedNames.push_back("InstituteCheckName");
	m_filedNames.push_back("InstituteCheckDate");
	m_filedNames.push_back("BureauCheckName");
	m_filedNames.push_back("BureauCheckUnit");
	m_filedNames.push_back("ConclusionBureau");
	m_filedNames.push_back("BureauCheckDate");
}

Filed::~Filed() 
{
}

void Filed::insertFileds(std::map<int, std::string> &fileds)
{
	fileds[1] = "TEXT not null";
	fileds[2] = "TEXT not null";
	fileds[3] = "TEXT(32) not null";
	fileds[4] = "TEXT(32) not null";
	fileds[5] = "TEXT(32) not null";
	fileds[6] = "DATE not null";
	fileds[7] = "TEXT(12) not null";
	fileds[8] = "FLOAT(6,1) not null";
	fileds[9] = "TEXT(8) not null";
	fileds[10] = "INTEGER not null";
	fileds[11] = "FLOAT(6,1) not null";
	fileds[12] = "TEXT not null";
	//fileds[13] = "DECIMAL(9,2) not null"; //float可能会造成精度损失
	//fileds[14] = "DECIMAL(9,2) not null";
	//fileds[15] = "DECIMAL(9,2) not null";
	//fileds[16] = "DECIMAL(9,2) not null";
	//fileds[17] = "DECIMAL(9,2) not null";
	//fileds[18] = "DECIMAL(9,2) not null";
	//fileds[19] = "DECIMAL(9,2) not null";
	//fileds[20] = "DECIMAL(9,2) not null";
	//fileds[21] = "DECIMAL(11,4) not null";
	fileds[13] = "TEXT not null";
	fileds[14] = "DECIMAL(9,2) not null";
	fileds[15] = "DECIMAL(9,2) not null";
	fileds[16] = "DECIMAL(9,2) not null";
	fileds[17] = "DECIMAL(9,2) not null";
	fileds[18] = "DECIMAL(11,4) not null";
	fileds[19] = "TEXT not null";
	fileds[20] = "TEXT not null";
	fileds[21] = "TEXT not null";
	fileds[22] = "INTEGER not null";
	fileds[23] = "TEXT not null";
	fileds[24] = "INTEGER not null";
	fileds[25] = "TEXT not null";
	fileds[26] = "TEXT not null";
	fileds[27] = "TEXT not null";
	fileds[28] = "TEXT not null";
	fileds[29] = "TEXT not null";
	fileds[30] = "FLOAT(6,1) not null";
	fileds[31] = "TEXT not null";
	fileds[32] = "DATE not null";
	fileds[33] = "TEXT not null";
	fileds[34] = "INTEGER not null";
	fileds[35] = "TEXT not null";
	fileds[36] = "FLOAT(6,1) not null";
	fileds[37] = "TEXT not null";
	fileds[38] = "DATE not null";
	fileds[39] = "TEXT not null";
	fileds[40] = "FLOAT not null";
	fileds[41] = "TEXT not null";
	fileds[42] = "TEXT not null";
	fileds[43] = "TEXT not null";
	fileds[44] = "TEXT not null";
	fileds[45] = "TEXT";
	fileds[46] = "TEXT not null";
	fileds[47] = "TEXT not null";
	fileds[48] = "TEXT not null";
	fileds[49] = "TEXT";
	fileds[50] = "TEXT not null";
	fileds[51] = "TEXT not null";
	fileds[52] = "TEXT not null";
	fileds[53] = "TEXT not null";
	fileds[54] = "TEXT not null";
	fileds[55] = "FLOAT(6,2) not null default -99.00";
	fileds[56] = "FLOAT(6,2) not null default -99.00";
	fileds[57] = "FLOAT(6,2) not null default -99.00";
	fileds[58] = "FLOAT(6,2) not null default -99.00";
	fileds[59] = "TEXT not null";
	fileds[60] = "TEXT not null";
	fileds[61] = "TEXT not null";
	fileds[62] = "TEXT not null";
	fileds[63] = "TEXT not null";
	fileds[64] = "INTEGER not null default 0";
	fileds[65] = "FLOAT(6,2) not null default -99.00";
	fileds[66] = "FLOAT(6,2) not null default -99.00";
	fileds[67] = "TEXT not null";
	fileds[68] = "TEXT not null";
	fileds[69] = "TEXT not null";
	fileds[70] = "DATE not null";
	fileds[71] = "TEXT not null";
	fileds[72] = "TEXT not null";
	fileds[73] = "TEXT not null";
	fileds[74] = "DATE not null";
}		   

std::string Filed::getFiledName(int index)
{
	if (index > 74 || index < 1)
	{
		return "";
	}
	else
	{
		return m_filedNames[index - 1];
	}
}

std::string Filed::getFildNameIndex(std::string name)
{
	std::vector<std::string>::iterator it;

	it = std::find(m_filedNames.begin(), m_filedNames.end(), name);
	if (it == m_filedNames.end())
	{
		return "";
	}
	else
	{
		int ind = it - m_filedNames.begin() + 1;
		char buf[3] = { 0 };
		sprintf_s(buf, "%d", ind);
		return std::string(buf, strlen(buf));
	}
}

std::string Filed::changeformat(std::string filed, std::string filedvalue)
{
	std::string rntstr = filedvalue;
	if (filed == "ProduceDate")
	{
		rntstr = filedvalue.substr(0, 4).append(filedvalue.substr(5, 2));
	}
	if (filed == "GroundResolution" || filed == "ImgSize" ||
		filed == "SateResolution" || filed == "MultiBandResolution")
	{
		char buf[100] = { 0 };
		sprintf_s(buf, "%.1f", atof(filedvalue.c_str()));
		rntstr = std::string(buf, strlen(buf));
	}
	if (filed == "SouthWestOrd" || filed == "NorthWestOrd" ||
		filed == "NorthEastOrd" || filed == "SouthEastOrd" || 
		filed == "SateOriXRMS" || filed == "SateOriYRMS" ||
		filed == "SateOriZRMS" || filed == "WestMosaicMaxError" ||
		filed == "NorthMosaicMaxError" || filed == "EastMosaicMaxError" ||
		filed == "SouthMosaicMaxError" || filed == "MultiBRectifyXRMS" ||
		filed == "MultiBRectifyYRMS" || filed == "CheckRMS" ||
		filed == "CheckMAXErr")
	{
		char buf[100] = { 0 };
		sprintf_s(buf, "%.2f", atof(filedvalue.c_str()));
		rntstr = std::string(buf, strlen(buf));
	}



	return rntstr;
}

bool Filed::isdate(std::string filedname)
{
	if (filedname == "ProduceDate" || filedname == "PbandDate" ||
		filedname == "MultiBandDate" || filedname == "InstituteCheckDate" ||
		filedname == "BureauCheckDate")
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool Filed::isNumQuery(std::string filedname)
{
	if (filedname == "GroundResolution" || filedname == "PixelBits" ||
		filedname == "ImgSize" || filedname == "GaussKrugerZoneNo" ||
		filedname == "SateResolution" || filedname == "MultiBandNum" ||
		filedname == "MultiBandResolution" || filedname == "GridInterval" ||
		filedname == "SateOriXRMS" || filedname == "SateOriYRMS" ||
		filedname == "SateOriZRMS" || filedname =="WestMosaicMaxError" ||
		filedname == "NorthMosaicMaxError" || filedname == "EastMosaicMaxError" ||
		filedname == "SouthMosaicMaxError" || filedname == "MultiBRectifyXRMS" ||
		filedname == "MultiBRectifyYRMS" || filedname == "CheckPointNum" ||
		filedname == "CheckRMS" || filedname == "CheckMAXErr")
	{
		return true;
	}
	else
	{
		return false;
	}
}


bool Filed::isStrView(std::string filedname)
{
	if (filedname == "ProductName" || filedname == "Owner" ||
		filedname == "Producer" || filedname == "Publisher" ||
		filedname == "InstituteCheckUnit" || filedname == "BureauCheckUnit" ||
		filedname == "MetaDataFileName")
	{
		return true;
	}
	else
	{
		return false;
	}
}
