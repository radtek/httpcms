#include "writexml.h"
#include "pugixml.hpp"
#include <iostream>

static int times;
static int interval;

int writexml(std::string path, std::string destpath, std::string datedate, std::string producer)
{
	pugi::xml_document xmlDoc;
	pugi::xml_node nodeRoot = xmlDoc.append_child("Metadatafile");
	// 声明
	pugi::xml_node pre = xmlDoc.prepend_child(pugi::node_declaration);
	pre.append_attribute("version") = "1.0";
	pre.append_attribute("encoding") = "utf-8";
	pre.append_attribute("standalone") = "no";


	//创建子节点
	pugi::xml_node nodeBDC = nodeRoot.append_child("BasicDataContent");
		pugi::xml_node nodeMDFN = nodeBDC.append_child("MetaDataFileName");
		pugi::xml_node nodePN = nodeBDC.append_child("ProductName");
		pugi::xml_node nodeO = nodeBDC.append_child("Owner");
		pugi::xml_node nodePC = nodeBDC.append_child("Producer");
		pugi::xml_node nodePS = nodeBDC.append_child("Publisher");
		pugi::xml_node nodePCD = nodeBDC.append_child("ProduceDate");
		pugi::xml_node nodeCfL = nodeBDC.append_child("ConfidentialLevel");
		pugi::xml_node nodeGR = nodeBDC.append_child("GroundResolution");
		pugi::xml_node nodeICM = nodeBDC.append_child("ImgColorModel"); 
		pugi::xml_node nodePB = nodeBDC.append_child("PixelBits");
		pugi::xml_node nodeIS = nodeBDC.append_child("ImgSize");
		pugi::xml_node nodeDF = nodeBDC.append_child("DataFormat");
		pugi::xml_node nodeIM = nodeBDC.append_child("ImgRange");
			pugi::xml_node nodeSouthWestAbs = nodeIM.append_child("SouthWestAbs");
			pugi::xml_node nodeSouthWestOrd = nodeIM.append_child("SouthWestOrd");
			pugi::xml_node nodeNorthWestAbs = nodeIM.append_child("NorthWestAbs");
			pugi::xml_node nodeNorthWestOrd = nodeIM.append_child("NorthWestOrd");
			pugi::xml_node nodeNorthEastAbs = nodeIM.append_child("NorthEastAbs");
			pugi::xml_node nodeNorthEastOrd = nodeIM.append_child("NorthEastOrd");
			pugi::xml_node nodeSouthEastAbs = nodeIM.append_child("SouthEastAbs");
			pugi::xml_node nodeSouthEastOrd = nodeIM.append_child("SouthEastOrd");
		pugi::xml_node nodeMathFoundation = nodeBDC.append_child("MathFoundation");
			pugi::xml_node nodeLongerRadius = nodeMathFoundation.append_child("LongerRadius");
			pugi::xml_node nodeOblatusRatio = nodeMathFoundation.append_child("OblatusRatio");
			pugi::xml_node nodeGeodeticDatum = nodeMathFoundation.append_child("GeodeticDatum");
			pugi::xml_node nodeMapProjection = nodeMathFoundation.append_child("MapProjection");
			pugi::xml_node nodeCentralMederian = nodeMathFoundation.append_child("CentralMederian");
			pugi::xml_node nodeZoneDivisionMode = nodeMathFoundation.append_child("ZoneDivisionMode");
			pugi::xml_node nodeGaussKrugerZoneNo = nodeMathFoundation.append_child("GaussKrugerZoneNo");
			pugi::xml_node nodeCoordinationUnit = nodeMathFoundation.append_child("CoordinationUnit");
			pugi::xml_node nodeHeightSystem = nodeMathFoundation.append_child("HeightSystem");
			pugi::xml_node nodeHeightDatum = nodeMathFoundation.append_child("HeightDatum");
	
	pugi::xml_node nodeImgSource = nodeRoot.append_child("ImgSource");
		pugi::xml_node nodeSateName = nodeImgSource.append_child("SateName");
		pugi::xml_node nodePanBand = nodeImgSource.append_child("PanBand");
			pugi::xml_node nodePBandSensorType = nodePanBand.append_child("PBandSensorType");
			pugi::xml_node nodeSateResolution = nodePanBand.append_child("SateResolution");
			pugi::xml_node nodePBandOrbitCode = nodePanBand.append_child("PBandOrbitCode");
			pugi::xml_node nodePBandDate = nodePanBand.append_child("PBandDate");
		pugi::xml_node nodeMultiBand = nodeImgSource.append_child("MultiBand");
			pugi::xml_node nodeMultiBandSensorType = nodeMultiBand.append_child("MultiBandSensorType");
			pugi::xml_node nodeMultiBandNum = nodeMultiBand.append_child("MultiBandNum");
			pugi::xml_node nodeMultiBandName = nodeMultiBand.append_child("MultiBandName");
			pugi::xml_node nodeMultiBandResolution = nodeMultiBand.append_child("MultiBandResolution");
			pugi::xml_node nodeMultiBandOrbitCode = nodeMultiBand.append_child("MultiBandOrbitCode");
			pugi::xml_node nodeMultiBandDate = nodeMultiBand.append_child("MultiBandDate");
		pugi::xml_node nodeSateImgQuality = nodeImgSource.append_child("SateImgQuality");

	pugi::xml_node nodeProduceInfomation = nodeRoot.append_child("ProduceInfomation");
		pugi::xml_node nodeGridInterval = nodeProduceInfomation.append_child("GridInterval");
		pugi::xml_node nodeDEMPrecision = nodeProduceInfomation.append_child("DEMPrecision");
		pugi::xml_node nodeControlSource = nodeProduceInfomation.append_child("ControlSource");
		pugi::xml_node nodeImgOrientation = nodeProduceInfomation.append_child("ImgOrientation");
			pugi::xml_node nodeSateOriXRMS = nodeImgOrientation.append_child("SateOriXRMS");
			pugi::xml_node nodeSateOriYRMS = nodeImgOrientation.append_child("SateOriYRMS");
			pugi::xml_node nodeSateOriZRMS = nodeImgOrientation.append_child("SateOriZRMS");
			pugi::xml_node nodeATProducerName = nodeImgOrientation.append_child("ATProducerName");
			pugi::xml_node nodeATCheckerName = nodeImgOrientation.append_child("ATCheckerName");
		pugi::xml_node nodeManufactureType = nodeProduceInfomation.append_child("ManufactureType");
		pugi::xml_node nodeSteroEditQuality = nodeProduceInfomation.append_child("SteroEditQuality");
		pugi::xml_node nodeOrthoRectifySoftware = nodeProduceInfomation.append_child("OrthoRectifySoftware");
		pugi::xml_node nodeResampleMethod = nodeProduceInfomation.append_child("ResampleMethod");
		pugi::xml_node nodeOrthoRectifyQuality = nodeProduceInfomation.append_child("OrthoRectifyQuality");
		pugi::xml_node nodeOrthoRectifyName = nodeProduceInfomation.append_child("OrthoRectifyName");
		pugi::xml_node nodeOrthoCheckName = nodeProduceInfomation.append_child("OrthoCheckName");
		pugi::xml_node nodeMosaicInfo = nodeProduceInfomation.append_child("MosaicInfo");
			pugi::xml_node nodeWestMosaicMaxError = nodeMosaicInfo.append_child("WestMosaicMaxError");
			pugi::xml_node nodeNorthMosaicMaxError = nodeMosaicInfo.append_child("NorthMosaicMaxError");
			pugi::xml_node nodeEastMosaicMaxError = nodeMosaicInfo.append_child("EastMosaicMaxError");
			pugi::xml_node nodeSouthMosaicMaxError = nodeMosaicInfo.append_child("SouthMosaicMaxError");
			pugi::xml_node nodeMosaicQuality = nodeMosaicInfo.append_child("MosaicQuality");
			pugi::xml_node nodeMosaicProducerName = nodeMosaicInfo.append_child("MosaicProducerName");
			pugi::xml_node nodeMosaicCheckerName = nodeMosaicInfo.append_child("MosaicCheckerName");
			pugi::xml_node nodeMultiBRectifyXRMS = nodeMosaicInfo.append_child("MultiBRectifyXRMS");
			pugi::xml_node nodeMultiBRectifyYRMS = nodeMosaicInfo.append_child("MultiBRectifyYRMS");
		pugi::xml_node nodeQualityCheckInfo = nodeProduceInfomation.append_child("QualityCheckInfo");
			pugi::xml_node nodeCheckPointNum = nodeQualityCheckInfo.append_child("CheckPointNum");
			pugi::xml_node nodeCheckRMS = nodeQualityCheckInfo.append_child("CheckRMS");
			pugi::xml_node nodeCheckMAXErr = nodeQualityCheckInfo.append_child("CheckMAXErr");
			pugi::xml_node nodeConclusionInstitute = nodeQualityCheckInfo.append_child("ConclusionInstitute");
			pugi::xml_node nodeInstituteCheckUnit = nodeQualityCheckInfo.append_child("InstituteCheckUnit");
			pugi::xml_node nodeInstituteCheckName = nodeQualityCheckInfo.append_child("InstituteCheckName");
			pugi::xml_node nodeInstituteCheckDate = nodeQualityCheckInfo.append_child("InstituteCheckDate");
			pugi::xml_node nodeBureauCheckName = nodeQualityCheckInfo.append_child("BureauCheckName");
			pugi::xml_node nodeBureauCheckUnit = nodeQualityCheckInfo.append_child("BureauCheckUnit");
			pugi::xml_node nodeConclusionBureau = nodeQualityCheckInfo.append_child("ConclusionBureau");
			pugi::xml_node nodeBureauCheckDate = nodeQualityCheckInfo.append_child("BureauCheckDate");


	nodePN.append_child(pugi::node_pcdata).set_value(u8"地理国情普查数字正射影像");
	nodeO.append_child(pugi::node_pcdata).set_value(u8"国家测绘地理信息局");
	nodePS.append_child(pugi::node_pcdata).set_value(u8"国家测绘地理信息局");
	nodeICM.append_child(pugi::node_pcdata).set_value(u8"多光谱");
	nodePB.append_child(pugi::node_pcdata).set_value("64");
	nodeDF.append_child(pugi::node_pcdata).set_value("Erdas Img");
	nodeCfL.append_child(pugi::node_pcdata).set_value(u8"秘密");

	nodeLongerRadius.append_child(pugi::node_pcdata).set_value("6378137.000000");
	nodeOblatusRatio.append_child(pugi::node_pcdata).set_value("1/298.257222101");
	nodeGeodeticDatum.append_child(pugi::node_pcdata).set_value(u8"2000国家大地坐标系");
	nodeMapProjection.append_child(pugi::node_pcdata).set_value(u8"高斯-克吕格投影");
	nodeCentralMederian.append_child(pugi::node_pcdata).set_value("129");
	nodeZoneDivisionMode.append_child(pugi::node_pcdata).set_value(u8"6度带");
	nodeGaussKrugerZoneNo.append_child(pugi::node_pcdata).set_value("22");
	nodeCoordinationUnit.append_child(pugi::node_pcdata).set_value(u8"米");
	nodeHeightSystem.append_child(pugi::node_pcdata).set_value(u8"正常高");
	nodeHeightDatum.append_child(pugi::node_pcdata).set_value(u8"1985国家高程基准");


	nodeSateName.append_child(pugi::node_pcdata).set_value("GF1");
	nodePBandSensorType.append_child(pugi::node_pcdata).set_value("PMS1");
	nodeSateResolution.append_child(pugi::node_pcdata).set_value("2.0");
	nodePBandOrbitCode.append_child(pugi::node_pcdata).set_value("240994");
	//nodePBandDate.append_child(pugi::node_pcdata).set_value("20140601");
	nodeMultiBandSensorType.append_child(pugi::node_pcdata).set_value("PMS1");
	nodeMultiBandNum.append_child(pugi::node_pcdata).set_value("4");
	nodeMultiBandName.append_child(pugi::node_pcdata).set_value("R/G/B/NIR");
	nodeMultiBandResolution.append_child(pugi::node_pcdata).set_value("8.0");
	nodeMultiBandOrbitCode.append_child(pugi::node_pcdata).set_value("240994");
	//nodeMultiBandDate.append_child(pugi::node_pcdata).set_value("20140601");
	nodeSateImgQuality.append_child(pugi::node_pcdata).set_value(u8"合格");


	nodeDEMPrecision.append_child(pugi::node_pcdata).set_value("1:50000DEM");
	nodeControlSource.append_child(pugi::node_pcdata).set_value(u8"1:10000正射影像");
	
	nodeSateOriXRMS.append_child(pugi::node_pcdata).set_value("0.38");
	nodeSateOriYRMS.append_child(pugi::node_pcdata).set_value("0.42");
	nodeATProducerName.append_child(pugi::node_pcdata).set_value(u8"李玫萱");
	nodeATCheckerName.append_child(pugi::node_pcdata).set_value(u8"张国祯");
	nodeManufactureType.append_child(pugi::node_pcdata).set_value(u8"单片纠正");

	nodeOrthoRectifySoftware.append_child(pugi::node_pcdata).set_value("VirtuoZoNGC Version 1.0");
	nodeResampleMethod.append_child(pugi::node_pcdata).set_value(u8"双线性");
	nodeOrthoRectifyQuality.append_child(pugi::node_pcdata).set_value(u8"合格");
	nodeOrthoRectifyName.append_child(pugi::node_pcdata).set_value(u8"李玫萱");
	nodeOrthoCheckName.append_child(pugi::node_pcdata).set_value(u8"张国祯");

	nodeWestMosaicMaxError.append_child(pugi::node_pcdata).set_value("2.00");
	nodeNorthMosaicMaxError.append_child(pugi::node_pcdata).set_value("2.00");
	nodeEastMosaicMaxError.append_child(pugi::node_pcdata).set_value("2.00");
	nodeSouthMosaicMaxError.append_child(pugi::node_pcdata).set_value("2.00");
	nodeMosaicQuality.append_child(pugi::node_pcdata).set_value(u8"接边符合精度要求");
	nodeMosaicProducerName.append_child(pugi::node_pcdata).set_value(u8"李玫萱");
	nodeMosaicCheckerName.append_child(pugi::node_pcdata).set_value(u8"李玫萱");
	nodeMultiBRectifyXRMS.append_child(pugi::node_pcdata).set_value("0.41");
	nodeMultiBRectifyYRMS.append_child(pugi::node_pcdata).set_value("0.47");

	nodeCheckPointNum.append_child(pugi::node_pcdata).set_value("50");
	nodeCheckRMS.append_child(pugi::node_pcdata).set_value("1.86");
	nodeCheckMAXErr.append_child(pugi::node_pcdata).set_value("6.81");
	nodeConclusionInstitute.append_child(pugi::node_pcdata).set_value(u8"合格");
	//nodeInstituteCheckUnit.append_child(pugi::node_pcdata).set_value("合格");
	nodeInstituteCheckName.append_child(pugi::node_pcdata).set_value(u8"张国祯");
	nodeInstituteCheckDate.append_child(pugi::node_pcdata).set_value("201505");
	nodeBureauCheckName.append_child(pugi::node_pcdata).set_value(u8"张新凤");
	//nodeBureauCheckUnit.append_child(pugi::node_pcdata).set_value("吉林省测绘产品质量监督检查站");
	nodeConclusionBureau.append_child(pugi::node_pcdata).set_value(u8"合格");
	nodeBureauCheckDate.append_child(pugi::node_pcdata).set_value("201505");


	//descr.append_child(pugi::node_pcdata).set_value("GF124099420140601Y.XML");


	/*加载xml文件*/
	//std::string path = "E:/ZhangYuxin/元数据管理/GF124099420140601Y.XML";
	pugi::xml_document doc;
	pugi::xml_parse_result result = doc.load_file(path.c_str());  //加载xml文件
	if (!result)
	{
		return -1;
	}
	/*读取xml文件*/
	pugi::xml_node root = doc.child("Metadatafile").child("BasicDataContent");
	for (pugi::xml_node chnode = root.first_child(); chnode; chnode = chnode.next_sibling())
	{
		std::string chnode_name = chnode.name();
		if (chnode_name == "ImgRange")
		{
			std::vector<double> x;
			std::vector<double> y;
			
			for (pugi::xml_node subcd = chnode.first_child(); subcd; subcd = subcd.next_sibling())
			{
				std::string subcd_name = subcd.name();
				if (subcd_name == "SouthWestAbs")
				{
					std::string s = subcd.text().as_string();
					double db = atof(s.c_str()) + 5;
					std::string buf;
					buf.resize(100);
					char buff[100] = { 0 };
					sprintf_s(buff, "%lf", db);
					std::string s2 = std::string(buff, strlen(buff));
					nodeSouthWestAbs.append_child(pugi::node_pcdata).set_value(s2.c_str());
				}
				if (subcd_name == "SouthWestOrd")
				{
					std::string s = subcd.text().as_string();
					double db = atof(s.c_str()) + 5;
					std::string buf;
					buf.resize(100);
					char buff[100] = { 0 };
					sprintf_s(buff, "%lf", db);
					std::string s2 = std::string(buff, strlen(buff));
					nodeSouthWestOrd.append_child(pugi::node_pcdata).set_value(s2.c_str());
				}
				if (subcd_name == "NorthWestAbs")
				{
					std::string s = subcd.text().as_string();
					double db = atof(s.c_str()) + 5;
					std::string buf;
					buf.resize(100);
					char buff[100] = { 0 };
					sprintf_s(buff, "%lf", db);
					std::string s2 = std::string(buff, strlen(buff));
					nodeNorthWestAbs.append_child(pugi::node_pcdata).set_value(s2.c_str());
				}
				if (subcd_name == "NorthWestOrd")
				{
					std::string s = subcd.text().as_string();
					double db = atof(s.c_str()) + 5;
					std::string buf;
					buf.resize(100);
					char buff[100] = { 0 };
					sprintf_s(buff, "%lf", db);
					std::string s2 = std::string(buff, strlen(buff));
					nodeNorthWestOrd.append_child(pugi::node_pcdata).set_value(s2.c_str());
				}
				if (subcd_name == "NorthEastAbs")
				{
					std::string s = subcd.text().as_string();
					double db = atof(s.c_str()) + 5;
					std::string buf;
					buf.resize(100);
					char buff[100] = { 0 };
					sprintf_s(buff, "%lf", db);
					std::string s2 = std::string(buff, strlen(buff));
					nodeNorthEastAbs.append_child(pugi::node_pcdata).set_value(s2.c_str());
				}
				if (subcd_name == "NorthEastOrd")
				{
					std::string s = subcd.text().as_string();
					double db = atof(s.c_str()) + 5;
					std::string buf;
					buf.resize(100);
					char buff[100] = { 0 };
					sprintf_s(buff, "%lf", db);
					std::string s2 = std::string(buff, strlen(buff));
					nodeNorthEastOrd.append_child(pugi::node_pcdata).set_value(s2.c_str());
				}
				if (subcd_name == "SouthEastAbs")
				{
					std::string s = subcd.text().as_string();
					double db = atof(s.c_str()) + 5;
					std::string buf;
					buf.resize(100);
					char buff[100] = { 0 };
					sprintf_s(buff, "%lf", db);
					std::string s2 = std::string(buff, strlen(buff));
					nodeSouthEastAbs.append_child(pugi::node_pcdata).set_value(s2.c_str());
				}
				if (subcd_name == "SouthEastOrd")
				{
					std::string s = subcd.text().as_string();
					double db = atof(s.c_str()) + 5;
					std::string buf;
					buf.resize(100);
					char buff[100] = { 0 };
					sprintf_s(buff, "%lf", db);
					std::string s2 = std::string(buff, strlen(buff));
					nodeSouthEastOrd.append_child(pugi::node_pcdata).set_value(s2.c_str());
				}
			}
		}
		else if (chnode_name == "MathFoundation")
		{
			for (pugi::xml_node subcd = chnode.first_child(); subcd; subcd = subcd.next_sibling())
			{
			}
		}
		else if (chnode_name == "MetaDataFileName")
		{
			std::string s = "GF1240994" + datedate + "Y.XML";
			nodeMDFN.append_child(pugi::node_pcdata).set_value(s.c_str());
		}
		else if (chnode_name == "Producer")
		{
			std::string s = producer + u8"省测绘地理信息局";
			nodePC.append_child(pugi::node_pcdata).set_value(s.c_str());
		}
		else if (chnode_name == "ProduceDate")
		{
			nodePCD.append_child(pugi::node_pcdata).set_value(datedate.c_str());
		}
		else if (chnode_name == "GroundResolution")
		{
			std::string s0 = "2.0";
			std::string s1 = "2.5";
			std::string s2 = "2.3";
			if (times == 0)
			{
				nodeGR.append_child(pugi::node_pcdata).set_value(s0.c_str());
			}
			if (times == 1)
			{
				nodeGR.append_child(pugi::node_pcdata).set_value(s1.c_str());
			}
			if (times == 2)
			{
				nodeGR.append_child(pugi::node_pcdata).set_value(s2.c_str());
			}
			times++;
			times %= 3;
			
		}
		else if (chnode_name == "ImgSize")
		{
			std::string s = chnode.text().as_string();
			double db = atof(s.c_str()) + 2.1;
			std::string buf;
			buf.resize(100);
			char buff[100] = { 0 };
			sprintf_s(buff, "%lf", db);
			std::string s2 = std::string(buff, strlen(buff));
			nodeIS.append_child(pugi::node_pcdata).set_value(s2.c_str());
		}
		
	}

	root = doc.child("Metadatafile").child("ImgSource");
	for (pugi::xml_node chnode = root.first_child(); chnode; chnode = chnode.next_sibling())
	{
		std::string chnode_name = chnode.name();
		if (chnode_name == "PanBand")
		{
			for (pugi::xml_node subcd = chnode.first_child(); subcd; subcd = subcd.next_sibling())
			{
				if (subcd.name() == "PBandDate")
				{
					std::string s = datedate;
					nodePBandDate.append_child(pugi::node_pcdata).set_value(s.c_str());
				}
			}
		}
		if (chnode_name == "MultiBand")
		{
			for (pugi::xml_node subcd = chnode.first_child(); subcd; subcd = subcd.next_sibling())
			{
				if (subcd.name() == "MultiBandDate")
				{
					std::string s = datedate;
					nodePBandDate.append_child(pugi::node_pcdata).set_value(s.c_str());
				}
			}
		}
	}

	root = doc.child("Metadatafile").child("ProduceInfomation");
	for (pugi::xml_node chnode = root.first_child(); chnode; chnode = chnode.next_sibling())
	{
		std::string chnode_name = chnode.name();
		if (chnode_name == "GridInterval")
		{
			interval++;
			interval %= 100;

			std::string buf;
			buf.resize(100);
			char buff[100] = { 0 };
			sprintf_s(buff, "%d", interval);
			std::string s2 = std::string(buff, strlen(buff));
			nodeGridInterval.append_child(pugi::node_pcdata).set_value(s2.c_str());
		}
		if (chnode_name == "QualityCheckInfo")
		{
			for (pugi::xml_node subcd = chnode.first_child(); subcd; subcd = subcd.next_sibling())
			{
				if (subcd.name() == "InstituteCheckUnit")
				{
					std::string s = producer + u8"航测遥感院";
					nodeInstituteCheckUnit.append_child(pugi::node_pcdata).set_value(s.c_str());
				}
				if (subcd.name() == "BureauCheckUnit")
				{
					std::string s = producer + u8"省测绘产品质量监督检查站";
					nodeBureauCheckUnit.append_child(pugi::node_pcdata).set_value(s.c_str());
				}
			}
		}
	}


	xmlDoc.save_file(destpath.c_str(), "\t", 1U, pugi::encoding_utf8);
}

int main2()
{
	std::vector<std::string> vct;
	vct.push_back(u8"山西");
	vct.push_back(u8"陕西");
	vct.push_back(u8"北京");
	vct.push_back(u8"天津"); 
	vct.push_back(u8"上海");

	std::vector<std::string> date_;
	date_.push_back("20150701");
	date_.push_back("20180208");
	date_.push_back("20090608");
	date_.push_back("20011208");
	date_.push_back("20000108");

	int ttt = 0;

	std::string sss = "E:/ZhangYuxin/元数据管理/xml1/GF124099420140601Y";
	for (int i = 0; i < 100000; i++)
	{
		
		ttt++;
		ttt %= 5;

		std::string buf;
		buf.resize(100);
		char buff[100] = { 0 };
		sprintf_s(buff, "%d", i);
		std::string s2 = std::string(buff, strlen(buff));

		sprintf_s(buff, "%d", i+1);
		std::string s3 = std::string(buff, strlen(buff));

		std::string path = sss + s2 + ".XML";
		std::string destpath = sss + s3 + ".XML";
		std::string datedate = date_[ttt];
		std::string producer = vct[ttt];
		writexml(path, destpath, datedate, producer);

	}
	
	std::cout << "finish" << std::endl;

	while (1) {}
	return 0;
}
