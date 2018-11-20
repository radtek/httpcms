#include "config.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/reader.h"

namespace meta_manage
{
	namespace config
	{
		static char mysql_host[1024] = "";
		static unsigned int mysql_port = 3306;
		static char mysql_user[256] = "";
		static char mysql_psw[256] = "";
		static char mysql_database[256] = "";
	}
}

bool meta_manage::config::init(const char* strjson)
{
	// 解析strjson
	rapidjson::Document doc;
	doc.Parse(strjson);
	if (doc.GetParseError() != rapidjson::kParseErrorNone) {
		// 解析失败
		return false;
	}
	// 读取MySQL连接参数信息
	if (doc.HasMember("mysql")) {
		const rapidjson::Value& mysql = doc["mysql"];
		rapidjson::Value::ConstMemberIterator iter = mysql.FindMember("host");
		if (iter != mysql.MemberEnd() && iter->value.IsString()) {
			memset(mysql_host, 0, sizeof(mysql_host));
			memcpy(mysql_host, iter->value.GetString(), iter->value.GetStringLength());
		}
		iter = mysql.FindMember("port");
		if (iter != mysql.MemberEnd() && iter->value.IsInt()) {
			mysql_port = iter->value.GetInt();
		}
		iter = mysql.FindMember("user");
		if (iter != mysql.MemberEnd() && iter->value.IsString()) {
			memset(mysql_user, 0, sizeof(mysql_user));
			memcpy(mysql_user, iter->value.GetString(), iter->value.GetStringLength());
		}
		iter = mysql.FindMember("psw");
		if (iter != mysql.MemberEnd() && iter->value.IsString()) {
			memset(mysql_psw, 0, sizeof(mysql_psw));
			memcpy(mysql_psw, iter->value.GetString(), iter->value.GetStringLength());
		}
		iter = mysql.FindMember("dbname");
		if (iter != mysql.MemberEnd() && iter->value.IsString()) {
			memset(mysql_database, 0, sizeof(mysql_database));
			memcpy(mysql_database, iter->value.GetString(), iter->value.GetStringLength());
		}
	}

	return true;
}

const char* meta_manage::config::get_mysql_host()
{
	return mysql_host;
}

const char* meta_manage::config::get_mysql_user()
{
	return mysql_user;
}

const char* meta_manage::config::get_mysql_psw()
{
	return mysql_psw;
}

const char* meta_manage::config::get_mysql_database()
{
	return mysql_database;
}

unsigned int meta_manage::config::get_mysql_port()
{
	return mysql_port;
}
