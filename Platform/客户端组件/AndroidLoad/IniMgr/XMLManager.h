#ifndef CCXML_FILE_MANAGER_CREATE_1556_DEF_H_
#define CCXML_FILE_MANAGER_CREATE_1556_DEF_H_

#include "xmlfile.h"
#include <vector>

using namespace std;

//XML文件管理
class XML_SCRIPT_EXPORTS CXMLManager
{
public:
	~CXMLManager(void);

public:
	static CXMLManager* GetInstance();

public:
	//打开XML文件,并获得文件指针.
	CXmlFile* OpenXMLFile(const std::string strFileName);
	//根据指定XML文件名，获得文件指针.
	CXmlFile* GetXMLFilePtr(const std::string strFileName);
	//根据指定XML文件名，关闭XML文件.
	bool CloseXMLFile(const std::string strFileName);
	//关闭所有XML文件.
	void CloseAllXMLFile();
	//保存所有XML文件.
	void SaveAllXMLFile();

private:
	CXMLManager(void);

private:
	static CXMLManager* s_Instance;
	std::vector<std::pair<std::string, CXmlFile*> >	m_xmlFileList;
};


#endif