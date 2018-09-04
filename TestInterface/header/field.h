#pragma once
#include <string>
#include <stdint.h>

/// ********************************************************************************
/// <summary>
/// 用户元数据字段属性映射表.
/// </summary>
/// ********************************************************************************
struct us_cms_metdata_map {
	std::string m_custon_field;	///< 用户元数据字段名称
	int         m_db_field;	    ///< 数据库中用户元数据表中对应字段名
	uint32_t    m_type;	        ///< 字段类型
	uint32_t    m_mode;	        ///< 搜索模式
};


/// ********************************************************************************
/// <summary>
/// 获取用户元数据字段属性映射表.
/// </summary>
/// <param name="out_size">传出映射表长度</param>
/// <returns>传出映射表首地址</returns>
/// <created>solym@sohu.com,2018/8/30</created>
/// ********************************************************************************
const us_cms_metdata_map* us_get_cms_metadata_map_table(int* out_size);

/// ********************************************************************************
/// <summary>
/// 读取用户元数据原始记录.
/// </summary>
/// <param name="index">索引用户元数据的键</param>
/// <param name="out_json">传出读取到的记录，或出错时的错误消息</param>
/// <returns>读取成功返回0，失败返回错误码</returns>
/// <created>solym@sohu.com,2018/8/30</created>
/// ********************************************************************************
int us_read_cms_metadata_record(const std::string& index, std::string* out_json);
