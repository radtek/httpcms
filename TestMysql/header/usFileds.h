#pragma once
#include <string>
#include <vector>

namespace usFiled
{
	enum FiledTypes
	{
		TYPE_INTEGER = 0,  //整型
		TYPE_FLOAT = 1,	   //浮点型
		TYPE_TEXT = 2,     //文本类型
		TYPE_DATE = 3      //日期类型
	};

	enum Mode
	{
		MODE_RANGE = 0,           //数值日期的范围查询
		MODE_GEOMETRY = 1,        //地理范围查询
		MODE_MATCH_BLUR = 2,	  //模糊查询
		MODE_MATCH_ACCURACY = 3   //精确查询
	};

	struct FiledsTable
	{
		std::string filedName;    //字段的名称
		int dbPreface;			  //数据库中的字段名
		FiledTypes filedType;	  //字段的类型
		Mode queryMode;			  //字段查询模式
	};

	class Fileds
	{
	public:
		int getFiledsTable(std::vector<FiledsTable> &filedsTable);

	public:
		int addFiledsTable(FiledsTable ft);

	private:
		std::vector<FiledsTable> m_fts;
	};
}
