#ifndef CCXML_FILE_INTERFACE_DEF_H_
#define CCXML_FILE_INTERFACE_DEF_H_

#include <string>
#include "xmlhead.h"

using namespace std;

class XML_SCRIPT_EXPORTS CXmlFile
{
public:
	virtual void	getValue(const std::string& keyName, std::string& strvalue, const std::string& strDefault="") = 0;
	virtual void    getValue(const std::string& keyName, float& fvalue, float fDefault=0.0f) = 0;
	virtual void	getValue(const std::string& keyName, int& ivalue, int iDefault=0) = 0;

	virtual bool	setValue(const std::string& keyName, const std::string& strvalue) = 0;
	virtual bool	setValue(const std::string& keyName, float fValue) = 0;
	virtual bool	setValue(const std::string& keyName, int iValue) = 0;

public:
	virtual ~CXmlFile(){};

};

#endif