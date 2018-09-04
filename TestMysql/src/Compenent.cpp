#include "Compenent.h"
#include <iostream>

std::string WString2String(const std::wstring& ws)
{
	std::string strLocale = setlocale(LC_ALL, "");
	const wchar_t* wchSrc = ws.c_str();
	size_t nDestSize = wcstombs(NULL, wchSrc, 0) + 1;
	char *chDest = new char[nDestSize];
	memset(chDest, 0, nDestSize);
	wcstombs(chDest, wchSrc, nDestSize);
	std::string strResult = chDest;
	delete[]chDest;
	setlocale(LC_ALL, strLocale.c_str());
	return strResult;
}

std::wstring String2WString(const std::string& s)
{
	std::string strLocale = setlocale(LC_ALL, "");
	const char* chSrc = s.c_str();
	size_t nDestSize = mbstowcs(NULL, chSrc, 0) + 1;
	wchar_t* wchDest = new wchar_t[nDestSize];
	wmemset(wchDest, 0, nDestSize);
	mbstowcs(wchDest, chSrc, nDestSize);
	std::wstring wstrResult = wchDest;
	delete[]wchDest;
	setlocale(LC_ALL, strLocale.c_str());
	return wstrResult;
}


void wipeStrSpace(std::string &str)
{
	while (*str.begin() == ' ' || *str.begin() == '\n' || *str.begin() == '\t')
	{
		str = str.substr(1);
	}
	while (str.back() == ' ' || str.back() == '\n' || str.back() == '\t')
	{
		str = str.substr(0, str.size() - 1);
	}
}

std::vector<std::string> getstrsub(std::string str, int len)
{
	std::vector<std::string> strsubs;
	if (str.size() == 0)
	{
		return strsubs;
	}
	else 
	{
		for (int i = 1; i <= str.size(); i++)
		{
			for (int j = 0; j < i; j++)
			{
				std::string s = str.substr(j, i - j);
				if (s.size() >= len)
				{
					strsubs.push_back(str.substr(j, i - j));
				}
			}
		}
	}
	return strsubs;
}


 int parseRequestJson(std::string jsonStr, std::multimap<std::string, std::string> &multiMp)
{
	 Json::Reader reader;
	 Json::Value root;

	 if (jsonStr.empty())
	 {
		 std::cout << "jsonstr is null" << std::endl;
	 }

	 if (reader.parse(jsonStr, root))
	 {
		 if (root.isMember("fileds"))
		 {
			 if (!root["fileds"].isNull())
			 {
				 if (root["fileds"].type() == Json::objectValue)
				 {
					 Json::Value::Members mem = root["fileds"].getMemberNames();
					 for (auto itr = mem.begin(); itr != mem.end(); ++itr)
					 {
						 if (!root["fileds"][*itr].isNull())
						 {
							 std::string s = root["fileds"][*itr].asString();
							 s = *itr + "=" + s;
							 multiMp.insert(std::pair<std::string, std::string>("fileds", s));
						 }
					 }
				 }
				 
			 }
		 }

		 if (root.isMember("_polygons"))
		 {
			 if (!root["_polygons"].isNull())
			 {
				 if (root["_polygons"].type() == Json::objectValue)
				 {
					 Json::Value::Members mem = root["_polygons"].getMemberNames();
					 for (auto itr = mem.begin(); itr != mem.end(); ++itr)
					 {
						 if (!root["_polygons"][*itr].isNull())
						 {
							 if (*itr == "rect" && root["_polygons"][*itr].type() == Json::arrayValue)
							 {
								 std::vector<double> pv;
								 for (int i = 0; i < root["_polygons"]["rect"].size(); i++)
								 {
									 pv.push_back(root["_polygons"]["rect"][i].asDouble());
								 }
								 std::string s1 = "[";
								 for (auto itr = pv.begin(); itr != pv.end(); ++itr)
								 {
									 char buf[100] = { 0 };
									 sprintf_s(buf, "%lf", *itr);
									 std::string s2 = std::string(buf, strlen(buf));
									 s1.append(s2).append(",");
								 }
								 s1 = s1.substr(0, s1.size() - 1);
								 s1.append("]");
								 
								 s1 = "rect=" + s1;
								 multiMp.insert(std::pair<std::string, std::string>("_polygons", s1));
							 }
							 if (*itr == "circle" && root["_polygons"][*itr].type() == Json::objectValue)
							 { 
								 std::string s;
								 if (root["_polygons"]["circle"].isMember("point"))
								 {
									 if (!root["_polygons"]["circle"]["point"].isNull())
									 {
										 if (root["_polygons"]["circle"]["point"].type() == Json::arrayValue)
										 {
											 std::vector<double> pv;
											 for (int i = 0; i < root["_polygons"]["circle"]["point"].size(); i++)
											 {
												 pv.push_back(root["_polygons"]["circle"]["point"][i].asDouble());
											 }
											 std::string s1 = "[";
											 for (auto itr = pv.begin(); itr != pv.end(); ++itr)
											 {
												 char buf[100] = { 0 };
												 sprintf_s(buf, "%lf", *itr);
												 std::string s2 = std::string(buf, strlen(buf));
												 s1.append(s2).append(",");
											 }
											 s1 = s1.substr(0, s1.size() - 1);
											 s1.append("]");
											 s = "circle={point=" + s1 + ",";
										 }
									 }
								 }
								 if (root["_polygons"]["circle"].isMember("r"))
								 {
									 if (!root["_polygons"]["circle"]["r"].isNull())
									 {
										 double r = root["_polygons"]["circle"]["r"].asDouble();
										 char buf[100] = { 0 };
										 sprintf_s(buf, "%lf", r);
										 std::string s1 = std::string(buf, strlen(buf));
										 s.append("r=").append(s1).append("}");
									 }
								 }
								 multiMp.insert(std::pair<std::string, std::string>("_polygons", s));
							 }
						 }
					 }
				 }
			 }
		 }

		 if (root.isMember("from"))
		 {
			 if (!root["from"].isNull())
			 {
				 int from = root["from"].asInt();
				 char buf[100] = { 0 };
				 sprintf_s(buf, "%d", from);
				 std::string s = std::string(buf, strlen(buf));
				 multiMp.insert(std::pair<std::string, std::string>("from", s));
			 }
		 }

		 if (root.isMember("size"))
		 {
			 if (!root["size"].isNull())
			 {
				 int size = root["size"].asInt();
				 char buf[100] = { 0 };
				 sprintf_s(buf, "%d", size);
				 std::string s = std::string(buf, strlen(buf));
				 multiMp.insert(std::pair<std::string, std::string>("size", s));
			 }
		 }
	 }
	 else
	 {
		 std::cout << "json parse failed" << std::endl;
		 return -1;
	 }

	 return 0;
}


int main3()
{
	/* //测试字符串分割获取所有子串
	std::string str = "GF124099420140601Y.XML";
	std::vector<std::string> sv = getstrsub(str, 10);
	for (auto itr = sv.begin(); itr != sv.end(); itr++)
	{
		if (itr->size() >= 5)
		{
			std::cout << *itr << std::endl;
		}
		
	}*/

	std::string jsonStr = "{															\
								\"fileds\":												\
								{														\
									\"Producer\": \"山西 地理 信息\",					\
									\"ProductName\"	: \"地理国情\"						\
								},														\
								\"_polygons\":											\
								{														\
									\"rect\": [4888200, 564900, 4931300, 607840],		\
									\"circle\":											\
									{													\
										\"point\":[0,0],								\
										\"r\": 4000									\
									}													\
								},														\
								\"from\": 1,											\
								\"size\":100											\
						   }";


	std::multimap<std::string, std::string> multiMp;
	int ret = parseRequestJson(jsonStr, multiMp);

	for (auto itr = multiMp.begin(); itr != multiMp.end(); ++itr)
	{
		std::cout << itr->first << ": " << itr->second << std::endl;
	}

	while (1) {}
	return 0;
}