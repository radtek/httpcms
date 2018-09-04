#include "pugixml.hpp"
#include <iostream>

//int dispXML()
//{
//	pugi::xml_document doc;
//	std::string path = "E:/ZhangYuxin/元数据管理/GF124099420140601Y.XML";
//
//	pugi::xml_parse_result result = doc.load_file(path.c_str());
//	if (!result)
//	{
//		return -1;
//	}
//
//	pugi::xml_node chnode = doc.child("Metadatafile").child("BasicDataContent").child("MetaDataFileName");
//	std::string MetaDataFileName = chnode.text().as_string();
//	
//	pugi::xml_node chnode = doc.child("Metadatafile").child("BasicDataContent").child("ProductName");
//	std::string ProductName = chnode.text().as_string();
//
//	pugi::xml_node chnode = doc.child("Metadatafile").child("BasicDataContent").child("Owner");
//	std::string Owner = chnode.text().as_string();
//
//	pugi::xml_node chnode = doc.child("Metadatafile").child("BasicDataContent").child("Producer");
//	std::string Producer = chnode.text().as_string();
//
//	pugi::xml_node chnode = doc.child("Metadatafile").child("BasicDataContent").child("ProduceDate");
//	std::string ProduceDate = chnode.text().as_string();
//	ProduceDate = ProduceDate.substr(0, 4) + "-" + ProduceDate.substr(4, 2) + "-01";
//
//	pugi::xml_node chnode = doc.child("Metadatafile").child("BasicDataContent").child("ConfidentialLevel");
//	std::string ConfidentialLevel = chnode.text().as_string();
//
//	pugi::xml_node chnode = doc.child("Metadatafile").child("BasicDataContent").child("GroundResolution");
//	float GroundResolution = chnode.text().as_float();
//
//	pugi::xml_node chnode = doc.child("Metadatafile").child("BasicDataContent").child("ImgColorModel");
//	std::string ImgColorModel = chnode.text().as_string();
//
//	pugi::xml_node chnode = doc.child("Metadatafile").child("BasicDataContent").child("PixelBits");
//	int PixelBits = chnode.text().as_int();
//
//	pugi::xml_node chnode = doc.child("Metadatafile").child("BasicDataContent").child("ImgSize");
//	float ImgSize = chnode.text().as_float();
//
//	pugi::xml_node chnode = doc.child("Metadatafile").child("BasicDataContent").child("DataFormat");
//	std::string DataFormat = chnode.text().as_string();
//
//	pugi::xml_node chnode = doc.child("Metadatafile").child("BasicDataContent").child("ImgRange").child("SouthWestAbs");
//	float SouthWestAbs = chnode.text().as_float();
//
//	pugi::xml_node chnode = doc.child("Metadatafile").child("BasicDataContent").child("ImgRange").child("SouthWestOrd");
//	float SouthWestOrd = chnode.text().as_float();
//
//	pugi::xml_node chnode = doc.child("Metadatafile").child("BasicDataContent").child("ImgRange").child("NorthWestAbs");
//	float NorthWestAbs = chnode.text().as_float();
//
//	pugi::xml_node chnode = doc.child("Metadatafile").child("BasicDataContent").child("ImgRange").child("NorthWestOrd");
//	float NorthWestOrd = chnode.text().as_float();
//
//	pugi::xml_node chnode = doc.child("Metadatafile").child("BasicDataContent").child("ImgRange").child("NorthEastAbs");
//	float NorthEastAbs = chnode.text().as_float();
//
//	pugi::xml_node chnode = doc.child("Metadatafile").child("BasicDataContent").child("ImgRange").child("NorthEastOrd");
//	float NorthEastOrd = chnode.text().as_float();
//
//	pugi::xml_node chnode = doc.child("Metadatafile").child("BasicDataContent").child("ImgRange").child("SouthEastAbs");
//	float SouthEastAbs = chnode.text().as_float();
//
//	pugi::xml_node chnode = doc.child("Metadatafile").child("BasicDataContent").child("ImgRange").child("SouthEastOrd");
//	float SouthEastOrd = chnode.text().as_float();
//
//	pugi::xml_node chnode = doc.child("Metadatafile").child("BasicDataContent").child("MathFoundation").child("LongerRadius");
//	float LongerRadius = chnode.text().as_float();
//
//	pugi::xml_node chnode = doc.child("Metadatafile").child("BasicDataContent").child("MathFoundation").child("OblatusRatio");
//	std::string OblatusRatio = chnode.text().as_string();
//
//	pugi::xml_node chnode = doc.child("Metadatafile").child("BasicDataContent").child("MathFoundation").child("GeodeticDatum");
//	std::string GeodeticDatum = chnode.text().as_string();
//
//	pugi::xml_node chnode = doc.child("Metadatafile").child("BasicDataContent").child("MathFoundation").child("MapProjection");
//	std::string MapProjection = chnode.text().as_string();
//
//	pugi::xml_node chnode = doc.child("Metadatafile").child("BasicDataContent").child("MathFoundation").child("CentralMederian");
//	int CentralMederian = chnode.text().as_int();
//
//	pugi::xml_node chnode = doc.child("Metadatafile").child("BasicDataContent").child("MathFoundation").child("ZoneDivisionMode");
//	std::string MapProjection = chnode.text().as_string();
//
//	pugi::xml_node chnode = doc.child("Metadatafile").child("BasicDataContent").child("MathFoundation").child("GaussKrugerZoneNo");
//	int GaussKrugerZoneNo = chnode.text().as_int();
//
//	pugi::xml_node chnode = doc.child("Metadatafile").child("BasicDataContent").child("MathFoundation").child("CoordinationUnit");
//	std::string CoordinationUnit = chnode.text().as_string();
//
//	pugi::xml_node chnode = doc.child("Metadatafile").child("BasicDataContent").child("MathFoundation").child("HeightSystem");
//	std::string HeightSystem = chnode.text().as_string();
//
//	pugi::xml_node chnode = doc.child("Metadatafile").child("BasicDataContent").child("MathFoundation").child("HeightDatum");
//	std::string HeightDatum = chnode.text().as_string();
//
//	pugi::xml_node chnode = doc.child("Metadatafile").child("ImgSource").child("SateName");
//	std::string SateName = chnode.text().as_string();
//
//	pugi::xml_node chnode = doc.child("Metadatafile").child("ImgSource").child("PanBand").child("PBandSensorType");
//	std::string PBandSensorType = chnode.text().as_string();
//
//	pugi::xml_node chnode = doc.child("Metadatafile").child("ImgSource").child("PanBand").child("SateResolution");
//	float SateResolution = chnode.text().as_float();
//	
//	pugi::xml_node chnode = doc.child("Metadatafile").child("ImgSource").child("PanBand").child("PbandOrbitCode");
//	std::string PbandOrbitCode = chnode.text().as_string();
//
//	pugi::xml_node chnode = doc.child("Metadatafile").child("ImgSource").child("PanBand").child("PbandDate");
//	std::string PbandDate = chnode.text().as_string();
//	PbandDate = PbandDate.substr(0, 4) + "-" + PbandDate.substr(4, 2) + "-" + PbandDate.substr(6,2);
//
//	pugi::xml_node chnode = doc.child("Metadatafile").child("ImgSource").child("MultiBand").child("MultiBandSensorType");
//	std::string MultiBandSensorType = chnode.text().as_string();
//
//	pugi::xml_node chnode = doc.child("Metadatafile").child("ImgSource").child("MultiBand").child("MultiBandNum");
//	int MultiBandNum = chnode.text().as_int();
//
//	pugi::xml_node chnode = doc.child("Metadatafile").child("ImgSource").child("MultiBand").child("MultiBandName");
//	std::string MultiBandName = chnode.text().as_string();
//
//	pugi::xml_node chnode = doc.child("Metadatafile").child("ImgSource").child("MultiBand").child("MultiBandResolution");
//	float MultiBandResolution = chnode.text().as_float();
//
//	pugi::xml_node chnode = doc.child("Metadatafile").child("ImgSource").child("MultiBand").child("MultiBandOrbitCode");
//	std::string MultiBandOrbitCode = chnode.text().as_string();
//
//	pugi::xml_node chnode = doc.child("Metadatafile").child("ImgSource").child("MultiBand").child("MultiBandDate");
//	std::string MultiBandDate = chnode.text().as_string();
//	MultiBandDate = MultiBandDate.substr(0, 4) + "-" + MultiBandDate.substr(4, 2) + "-" + MultiBandDate.substr(6, 2);
//
//	pugi::xml_node chnode = doc.child("Metadatafile").child("ImgSource").child("MultiBand").child("SateImgQuality");
//	std::string SateImgQuality = chnode.text().as_string();
//
//	pugi::xml_node chnode = doc.child("Metadatafile").child("ProduceInfomation").child("GridInterval");
//	float GridInterval = chnode.text().as_float();
//
//	pugi::xml_node chnode = doc.child("Metadatafile").child("ProduceInfomation").child("DEMPrecision");
//	std::string DEMPrecision = chnode.text().as_string();
//
//	pugi::xml_node chnode = doc.child("Metadatafile").child("ProduceInfomation").child("ControlSource");
//	std::string ControlSource = chnode.text().as_string();
//
//	pugi::xml_node chnode = doc.child("Metadatafile").child("ProduceInfomation").child("ImgOrientation").child("SateOriXRMS");
//	std::string SateOriXRMS = chnode.text().as_string();
//
//	pugi::xml_node chnode = doc.child("Metadatafile").child("ProduceInfomation").child("ImgOrientation").child("SateOriYRMS");
//	std::string SateOriYRMS = chnode.text().as_string();
//
//	pugi::xml_node chnode = doc.child("Metadatafile").child("ProduceInfomation").child("ImgOrientation").child("SateOriZRMS");
//	std::string SateOriZRMS = chnode.text().as_string();
//
//	pugi::xml_node chnode = doc.child("Metadatafile").child("ProduceInfomation").child("ImgOrientation").child("ATProducerName");
//	std::string ATProducerName = chnode.text().as_string();
//
//	pugi::xml_node chnode = doc.child("Metadatafile").child("ProduceInfomation").child("ImgOrientation").child("ATCheckerName");
//	std::string ATCheckerName = chnode.text().as_string();
//
//	pugi::xml_node chnode = doc.child("Metadatafile").child("ProduceInfomation").child("ManufactureType");
//	std::string ManufactureType = chnode.text().as_string();
//
//	pugi::xml_node chnode = doc.child("Metadatafile").child("ProduceInfomation").child("SteroEditQuality");
//	std::string SteroEditQuality = chnode.text().as_string();
//
//	pugi::xml_node chnode = doc.child("Metadatafile").child("ProduceInfomation").child("OrthoRectifySoftWare");
//	std::string OrthoRectifySoftWare = chnode.text().as_string();
//
//	pugi::xml_node chnode = doc.child("Metadatafile").child("ProduceInfomation").child("ResampleMethod");
//	std::string ResampleMethod = chnode.text().as_string();
//
//	pugi::xml_node chnode = doc.child("Metadatafile").child("ProduceInfomation").child("OrthoRectifyQuality");
//	std::string OrthoRectifyQuality = chnode.text().as_string();
//
//	pugi::xml_node chnode = doc.child("Metadatafile").child("ProduceInfomation").child("OrthoRectifyName");
//	std::string OrthoRectifyName = chnode.text().as_string();
//
//	pugi::xml_node chnode = doc.child("Metadatafile").child("ProduceInfomation").child("OrthoCheckName");
//	std::string OrthoCheckName = chnode.text().as_string();
//
//	pugi::xml_node chnode = doc.child("Metadatafile").child("ProduceInfomation").child("MosaicInfo").child("WestMosaicMaxError");
//	float WestMosaicMaxError = chnode.text().as_float();
//
//	pugi::xml_node chnode = doc.child("Metadatafile").child("ProduceInfomation").child("MosaicInfo").child("NorthMosaicMaxError");
//	float NorthMosaicMaxError = chnode.text().as_float();
//
//	pugi::xml_node chnode = doc.child("Metadatafile").child("ProduceInfomation").child("MosaicInfo").child("EastMosaicMaxError");
//	float EastMosaicMaxError = chnode.text().as_float();
//
//	pugi::xml_node chnode = doc.child("Metadatafile").child("ProduceInfomation").child("MosaicInfo").child("SouthMosaicMaxError");
//	float SouthMosaicMaxError = chnode.text().as_float();
//
//	pugi::xml_node chnode = doc.child("Metadatafile").child("ProduceInfomation").child("MosaicInfo").child("MosaicQuality");
//	std::string MosaicQuality = chnode.text().as_string();
//
//	pugi::xml_node chnode = doc.child("Metadatafile").child("ProduceInfomation").child("MosaicInfo").child("MosaicProducerName");
//	std::string MosaicProducerName = chnode.text().as_string();
//
//	pugi::xml_node chnode = doc.child("Metadatafile").child("ProduceInfomation").child("MosaicInfo").child("MosaicCheckerName");
//	std::string MosaicCheckerName = chnode.text().as_string();
//
//	pugi::xml_node chnode = doc.child("Metadatafile").child("ProduceInfomation").child("MosaicInfo").child("MultiBRectifyXRMS");
//	std::string MultiBRectifyXRMS = chnode.text().as_string();
//
//	pugi::xml_node chnode = doc.child("Metadatafile").child("ProduceInfomation").child("MosaicInfo").child("MultiBRectifyYRMS");
//	std::string MultiBRectifyYRMS = chnode.text().as_string();
//
//	pugi::xml_node chnode = doc.child("Metadatafile").child("ProduceInfomation").child("QualityCheckInfo").child("CheckPointNum");
//	int CheckPointNum = chnode.text().as_int();
//
//	pugi::xml_node chnode = doc.child("Metadatafile").child("ProduceInfomation").child("QualityCheckInfo").child("CheckRMS");
//	float CheckRMS = chnode.text().as_float();
//
//	pugi::xml_node chnode = doc.child("Metadatafile").child("ProduceInfomation").child("QualityCheckInfo").child("CheckMAXErr");
//	float CheckMAXErr = chnode.text().as_float();
//
//	pugi::xml_node chnode = doc.child("Metadatafile").child("ProduceInfomation").child("QualityCheckInfo").child("ConclusionInstitute");
//	std::string ConclusionInstitute = chnode.text().as_string();
//
//	pugi::xml_node chnode = doc.child("Metadatafile").child("ProduceInfomation").child("QualityCheckInfo").child("InstituteCheckUnit");
//	std::string InstituteCheckUnit = chnode.text().as_string();
//
//	pugi::xml_node chnode = doc.child("Metadatafile").child("ProduceInfomation").child("QualityCheckInfo").child("InstituteCheckName");
//	std::string InstituteCheckName = chnode.text().as_string();
//
//	pugi::xml_node chnode = doc.child("Metadatafile").child("ProduceInfomation").child("QualityCheckInfo").child("InstituteCheckDate");
//	std::string InstituteCheckDate = chnode.text().as_string();
//	InstituteCheckDate = InstituteCheckDate.substr(0, 4) + "-" + InstituteCheckDate.substr(4, 2) + "-01";
//
//	pugi::xml_node chnode = doc.child("Metadatafile").child("ProduceInfomation").child("QualityCheckInfo").child("BureauCheckName");
//	std::string BureauCheckName = chnode.text().as_string();
//
//	pugi::xml_node chnode = doc.child("Metadatafile").child("ProduceInfomation").child("QualityCheckInfo").child("BureauCheckUnit");
//	std::string BureauCheckUnit = chnode.text().as_string();
//
//	pugi::xml_node chnode = doc.child("Metadatafile").child("ProduceInfomation").child("QualityCheckInfo").child("ConclusionBureau");
//	std::string ConclusionBureau = chnode.text().as_string();
//
//	pugi::xml_node chnode = doc.child("Metadatafile").child("ProduceInfomation").child("QualityCheckInfo").child("BureauCheckDate");
//	std::string BureauCheckDate = chnode.text().as_string();
//	BureauCheckDate = BureauCheckDate.substr(0, 4) + "-" + BureauCheckDate.substr(4, 2) + "-01";
//}


