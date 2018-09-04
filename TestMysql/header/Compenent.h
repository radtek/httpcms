#pragma once
#include <string>
#include <vector>
#include "json.h"
#include <map>

std::string WString2String(const std::wstring& ws);

std::wstring String2WString(const std::string& s);



/*去除字符串两端的空白字符*/
void wipeStrSpace(std::string &str);

/*获取字符串一定长度的字串*/
std::vector<std::string> getstrsub(std::string str, int len);

/*解析请求的json字符串，放在multimap中*/
int parseRequestJson(std::string jsonStr, std::multimap<std::string, std::string> &multiMp);

