#pragma once
#include <windows.h>
#import "msxml6.dll" 
#include <string>
#include <map>
using namespace std;

// 去掉对闭源C++库IniMgr.dll的依赖

class CXMLParse
{
public:
	CXMLParse();
	virtual ~CXMLParse();

public:
	BOOL ReadXMLFile(TCHAR strXMLFile[256]);
	BOOL WriteXMLFile(TCHAR strXMLFile[256]);

	void	getValue(const std::string& keyName, std::string& strvalue, const std::string& strDefault="");
	void    getValue(const std::string& keyName, float& fvalue, float fDefault=0.0f);
	void	getValue(const std::string& keyName, int& ivalue, int iDefault=0);

	bool	setValue(const std::string& keyName, const std::string& strvalue);
	bool	setValue(const std::string& keyName, float fValue);
	bool	setValue(const std::string& keyName, int iValue);

private:
	map<string,string> m_MapStrStr;
	MSXML2::IXMLDOMDocumentPtr m_pXMLDoc;									//XMLDoc 指针
};