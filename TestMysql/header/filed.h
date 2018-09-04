#pragma once
#include <map>
#include <vector>

class Filed
{
public:
	Filed();
	~Filed();

	/*每个字段对应的类型以及其他的一些属性，字段-属性 存放在一个map（创建表格时，从map中获取列名及其属性）中*/
	void insertFileds(std::map<int, std::string> &fileds);

	/*根据数据库中的字段名（一个索引）获取真实的字段名称*/
	std::string getFiledName(int index);

	/*根据真实的字段名称获取数据库中的字段名（索引）*/
	std::string getFildNameIndex(std::string name);

	/*将字段值改为要求的格式*/
	std::string changeformat(std::string filed, std::string filedvalue);

	/*根据字段名称判断是否为日期类型*/
	bool isdate(std::string filedname);

	/*数值类型*/
	bool isNumQuery(std::string filedname);

	/*是否可以字符串的模糊查询*/
	bool isStrView(std::string filedname);

private:
	std::vector<std::string> m_filedNames; //存放字段名称
};


