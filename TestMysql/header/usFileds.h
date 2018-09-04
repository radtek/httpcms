#pragma once
#include <string>
#include <vector>

namespace usFiled
{
	enum FiledTypes
	{
		TYPE_INTEGER = 0,  //����
		TYPE_FLOAT = 1,	   //������
		TYPE_TEXT = 2,     //�ı�����
		TYPE_DATE = 3      //��������
	};

	enum Mode
	{
		MODE_RANGE = 0,           //��ֵ���ڵķ�Χ��ѯ
		MODE_GEOMETRY = 1,        //����Χ��ѯ
		MODE_MATCH_BLUR = 2,	  //ģ����ѯ
		MODE_MATCH_ACCURACY = 3   //��ȷ��ѯ
	};

	struct FiledsTable
	{
		std::string filedName;    //�ֶε�����
		int dbPreface;			  //���ݿ��е��ֶ���
		FiledTypes filedType;	  //�ֶε�����
		Mode queryMode;			  //�ֶβ�ѯģʽ
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
