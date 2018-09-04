#include "usReadXml.h"
#include "usFileds.h"

int usReadXml::ReadXml::readXml(std::string xmlPath)
{
	pugi::xml_document doc;
	pugi::xml_parse_result result = doc.load_file(xmlPath.c_str());  //¼ÓÔØxmlÎÄ¼þ
	if (!result)
	{
		return -1;
	}
	
	int dbPreface = 1;
	
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
				if (subcd_name == "SouthWestAbs" || subcd_name == "NorthWestAbs" ||
					subcd_name == "NorthEastAbs" || subcd_name == "SouthEastAbs")
				{
					std::string s = subcd.text().as_string();
					x.push_back(atof(s.c_str()));
				}
				if (subcd_name == "SouthWestOrd" || subcd_name == "NorthWestOrd" ||
					subcd_name == "NorthEastOrd" || subcd_name == "SouthEastOrd")
				{
					std::string s = subcd.text().as_string();
					y.push_back(atof(s.c_str()));
				}
			}
		}
	}
}