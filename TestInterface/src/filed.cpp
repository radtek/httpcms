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

const us_cms_metdata_map * us_get_cms_metadata_map_table(int * out_size)
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

int us_read_cms_metadata_record(const std::string & index, std::string * out_json)
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
				else if (subcd_name == "PbandDate" || subcd_name == "MultiBandDate")
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


