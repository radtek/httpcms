#pragma once

namespace meta_manage
{
	namespace config
	{
		bool init(const char* strjson);

		const char* get_mysql_host();

		const char* get_mysql_user();

		const char* get_mysql_psw();

		const char* get_mysql_database();

		unsigned int get_mysql_port();
	}
}
