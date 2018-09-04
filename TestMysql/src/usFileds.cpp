#include "usFileds.h"

int usFiled::Fileds::getFiledsTable(std::vector<usFiled::FiledsTable> &filedsTable)
{
	for (auto itr = m_fts.begin(); itr != m_fts.end(); ++itr)
	{
		filedsTable.push_back(*itr);
	}

	return 0;
}

int usFiled::Fileds::addFiledsTable(FiledsTable ft)
{
	m_fts.push_back(ft);

	return 0;
}

#include "custom_metadata_process.hpp"
#include <fstream>
#include <iostream>
#include <cassert>
#include <vector>
#include<algorithm>
#include "pugixml.hpp"
#include "pugiconfig.hpp"

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
		*out_json = "load xml failed";
		return -1;
	}

	*out_json = "{ ";

	pugi::xml_node root = doc.child("Metadatafile").child("BasicDataContent");
	for (pugi::xml_node chnode = root.first_child(); chnode; chnode = chnode.next_sibling())
	{
		std::string chnode_name = chnode.name();
		if (chnode_name == "ImgRange")
		{
			std::vector<double> x;
			std::vector<double> y;

			out_json->append("\"ImgRange\": ");
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
			s_imgrange = s_imgrange.substr(0, s_imgrange.size() - 2);
			out_json->append("\"").append(s_imgrange).append("\", ");

			std::vector<double>::iterator x_max_itr = std::max_element(x.begin(), x.end());
			double x_max = *x_max_itr;
			std::vector<double>::iterator x_min_itr = std::min_element(x.begin(), x.end());
			double x_min = *x_min_itr;
			std::vector<double>::iterator y_max_itr = std::max_element(y.begin(), y.end());
			double y_max = *y_max_itr;
			std::vector<double>::iterator y_min_itr = std::min_element(y.begin(), y.end());
			double y_min = *y_min_itr;

			char buf[100] = { 0 };
			sprintf_s(buf, "%.2lf", x_max);
			std::string s_x_max = std::string(buf, strlen(buf));
			memset(buf, 0, 100);
			sprintf_s(buf, "%.2lf", x_min);
			std::string s_x_min = std::string(buf, strlen(buf));
			memset(buf, 0, 100);
			sprintf_s(buf, "%.2lf", y_max);
			std::string s_y_max = std::string(buf, strlen(buf));
			memset(buf, 0, 100);
			sprintf_s(buf, "%.2lf", y_min);
			std::string s_y_min = std::string(buf, strlen(buf));

			out_json->append("\"MaxX\": ").append(s_x_max).append(", ");
			out_json->append("\"MinX\": ").append(s_x_min).append(", ");
			out_json->append("\"MaxY\": ").append(s_y_max).append(", ");
			out_json->append("\"MinY\": ").append(s_y_min).append(", ");
		}
		else if (chnode_name == "MathFoundation")
		{
			for (pugi::xml_node subcd = chnode.first_child(); subcd; subcd = subcd.next_sibling())
			{
				std::string subcd_name = subcd.name();
				std::string s = subcd.text().as_string();
				if (subcd_name == "CentralMederian" || subcd_name == "GaussKrugerZoneNo")
				{
					out_json->append("\"").append(subcd_name).append("\": ").append(s).append(", ");
				}
				else
				{
					out_json->append("\"").append(subcd_name).append("\": \"").append(s).append("\", ");
				}
			}
		}
		else
		{
			std::string s = chnode.text().as_string();
			if (chnode_name == "GroundResolution" || chnode_name == "PixelBits" || chnode_name == "ImgSize")
			{
				out_json->append("\"").append(chnode_name).append("\": ").append(s).append(", ");
			}
			else
			{
				out_json->append("\"").append(chnode_name).append("\": \"").append(s).append("\", ");
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

				if (subcd_name == "SateResolution" || subcd_name == "MultiBandNum" || subcd_name == "MultiBandResolution")
				{
					out_json->append("\"").append(subcd_name).append("\": ").append(s).append(", ");
				}
				else
				{
					out_json->append("\"").append(chnode_name).append("\": \"").append(s).append("\", ");
				}
			}
		}
		else
		{
			std::string s = chnode.text().as_string();
			out_json->append("\"").append(chnode_name).append("\": \"").append(s).append("\", ");
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

				if (subcd_name == "SateOriXRMS" || subcd_name == "SateOriYRMS" || subcd_name == "SateOriZRMS" ||
					subcd_name == "WestMosaicMaxError" || subcd_name == "NorthMosaicMaxError" ||
					subcd_name == "EastMosaicMaxError" || subcd_name == "SouthMosaicMaxError" ||
					subcd_name == "MultiBRectifyXRMS" || subcd_name == "MultiBRectifyYRMS" ||
					subcd_name == "CheckPointNum" || subcd_name == "CheckRMS" ||
					subcd_name == "CheckMAXErr")
				{
					out_json->append("\"").append(subcd_name).append("\": ").append(s).append(", ");
				}
				else
				{
					out_json->append("\"").append(subcd_name).append("\": \"").append(s).append("\", ");
				}
			}
		}
		else
		{
			std::string s = chnode.text().as_string();
			if (chnode_name == "GridInterval")
			{
				out_json->append("\"").append(chnode_name).append("\": ").append(s).append(", ");
			}
			else
			{
				out_json->append("\"").append(chnode_name).append("\": \"").append(s).append("\", ");
			}
		}
	}

	*out_json = out_json->substr(0, out_json->size() - 2);
	out_json->append(" }");

	return 0;
}
