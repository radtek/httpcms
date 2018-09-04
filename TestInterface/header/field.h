#pragma once
#include <string>
#include <stdint.h>

/// ********************************************************************************
/// <summary>
/// �û�Ԫ�����ֶ�����ӳ���.
/// </summary>
/// ********************************************************************************
struct us_cms_metdata_map {
	std::string m_custon_field;	///< �û�Ԫ�����ֶ�����
	int         m_db_field;	    ///< ���ݿ����û�Ԫ���ݱ��ж�Ӧ�ֶ���
	uint32_t    m_type;	        ///< �ֶ�����
	uint32_t    m_mode;	        ///< ����ģʽ
};


/// ********************************************************************************
/// <summary>
/// ��ȡ�û�Ԫ�����ֶ�����ӳ���.
/// </summary>
/// <param name="out_size">����ӳ�����</param>
/// <returns>����ӳ����׵�ַ</returns>
/// <created>solym@sohu.com,2018/8/30</created>
/// ********************************************************************************
const us_cms_metdata_map* us_get_cms_metadata_map_table(int* out_size);

/// ********************************************************************************
/// <summary>
/// ��ȡ�û�Ԫ����ԭʼ��¼.
/// </summary>
/// <param name="index">�����û�Ԫ���ݵļ�</param>
/// <param name="out_json">������ȡ���ļ�¼�������ʱ�Ĵ�����Ϣ</param>
/// <returns>��ȡ�ɹ�����0��ʧ�ܷ��ش�����</returns>
/// <created>solym@sohu.com,2018/8/30</created>
/// ********************************************************************************
int us_read_cms_metadata_record(const std::string& index, std::string* out_json);
