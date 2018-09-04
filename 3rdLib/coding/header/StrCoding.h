#pragma once
#ifndef __STRCODING_H_
#define __STRCODING_H_

#include <iostream>
#include <string>
#include <windows.h>

class StrCoding
{
public:
	StrCoding(void);
	~StrCoding(void);

	void UTF_8ToGB2312(std::string &pOut, char *pText, int pLen); //utf_8转为gb2312
	void GB2312ToUTF_8(std::string& pOut, char *pText, int pLen); //gb2312 转utf_8

	std::string UrlGB2312(char * str);                            //urlgb2312编码
	std::string UrlUTF8(char * str);                              //urlutf8 编码
	
	std::string UrlGB2312Decode(std::string str);                 //urlgb2312解码
	std::string UrlUTF8Decode(std::string str);                   //urlutf8解码

private:
	void Gb2312ToUnicode(WCHAR* pOut, char *gbBuffer);
	void UTF_8ToUnicode(WCHAR* pOut, char *pText);
	void UnicodeToUTF_8(char* pOut, WCHAR* pText);
	void UnicodeToGB2312(char* pOut, WCHAR uData);
	char CharToInt(char ch);
	char StrToBin(char *str);
};

#endif
