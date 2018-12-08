#include "stdafx.h"
#include "xmlparse.h"
#include<cassert> 

CXMLParse::CXMLParse()
{
	m_pXMLDoc = NULL;
}

CXMLParse::~CXMLParse()
{

}

BOOL CXMLParse::ReadXMLFile(TCHAR strXMLFile[256])
{
	//打开文件
	HRESULT hr=m_pXMLDoc.CreateInstance("Microsoft.XMLDOM");
	if (SUCCEEDED(hr))
	{
        VARIANT_BOOL  retBool=m_pXMLDoc->load(_bstr_t(strXMLFile));
		if(retBool == VARIANT_TRUE)
		{
			//MSXML2::IXMLDOMNodePtr pCurNode=m_pXMLDoc->GetfirstChild();//xml
		    MSXML2::IXMLDOMNodePtr pCurNode = m_pXMLDoc->selectSingleNode((_bstr_t)"root");
			assert(pCurNode!=NULL);

			if(pCurNode)
			{
				OutputDebugStringA((char *)pCurNode->GetnodeName());
				MSXML2::IXMLDOMNodePtr xNode=pCurNode->GetfirstChild();
				while(xNode)
				{
					string key=xNode->GetnodeName();
                    // 节点属性,放在链表中 
                    MSXML2::IXMLDOMNamedNodeMapPtr pAttrMap=NULL;
                    MSXML2::IXMLDOMNodePtr pAttrItem;
                    _variant_t variantvalue;
                    xNode->get_attributes(&pAttrMap);
                    long count;
                    count=pAttrMap->get_length(&count);
                    pAttrMap->get_item(0,&pAttrItem);
                    // 取得节点的值
                    pAttrItem->get_nodeValue(&variantvalue);
                    string value=(_bstr_t)variantvalue;
					m_MapStrStr[key]=value;
					xNode=xNode->GetnextSibling();

					char sz[100]={0};
					sprintf(sz,"k=%s,v=%s",key.c_str(),value.c_str());
					OutputDebugStringA(sz);
				}
			}
			m_pXMLDoc.Release();
			m_pXMLDoc = NULL;
		}
		else
		{
			char sz[100]={0};
			sprintf(sz,"Can't load xml file %s!", strXMLFile); 
			OutputDebugStringA(sz);	
			return FALSE;
		}
	}
	else
	{
	    char sz[100]={0};
		sprintf(sz,"Can't create instance of Microsoft.XMLDOM!"); 
		OutputDebugStringA(sz);	
		return FALSE;
	}
	return TRUE;
}

BOOL CXMLParse::WriteXMLFile(TCHAR strXMLFile[256])
{
	//打开文件
	HRESULT hr=m_pXMLDoc.CreateInstance("Microsoft.XMLDOM");
	if (SUCCEEDED(hr))
	{
        VARIANT_BOOL  retBool=m_pXMLDoc->load(_bstr_t(strXMLFile));
		if(retBool == VARIANT_TRUE)
		{
			//MSXML2::IXMLDOMNodePtr pCurNode=m_pXMLDoc->GetfirstChild();//xml
		    MSXML2::IXMLDOMNodePtr pCurNode = m_pXMLDoc->selectSingleNode((_bstr_t)"root");
			assert(pCurNode!=NULL);

			if(pCurNode)
			{
				OutputDebugStringA((char *)pCurNode->GetnodeName());
				MSXML2::IXMLDOMNodePtr xNode=pCurNode->GetfirstChild();
				while(xNode)
				{
					string key=xNode->GetnodeName();
                    // 节点属性,放在链表中 
                    MSXML2::IXMLDOMNamedNodeMapPtr pAttrMap=NULL;
                    MSXML2::IXMLDOMNodePtr pAttrItem;

                    xNode->get_attributes(&pAttrMap);
                    long count;
                    count=pAttrMap->get_length(&count);
                    pAttrMap->get_item(0,&pAttrItem);
                    string value=m_MapStrStr[key];
					_variant_t variantvalue=(_variant_t)value.c_str();
                    pAttrItem->put_nodeValue(variantvalue);
					xNode=xNode->GetnextSibling();

					char sz[100]={0};
					sprintf(sz,"k=%s,v=%s",key.c_str(),value.c_str());
					OutputDebugStringA(sz);
				}
			}
			m_pXMLDoc->save(_bstr_t(strXMLFile));
			m_pXMLDoc.Release();
			m_pXMLDoc = NULL;
		}
		else
		{
			char sz[100]={0};
			sprintf(sz,"Can't load xml file %s!", strXMLFile); 
			OutputDebugStringA(sz);	
			return FALSE;
		}
	}
	else
	{
	    char sz[100]={0};
		sprintf(sz,"Can't create instance of Microsoft.XMLDOM!"); 
		OutputDebugStringA(sz);	
		return FALSE;
	}
	return TRUE;
}

void CXMLParse::getValue(const std::string& keyName, std::string& strvalue, const std::string& strDefault/*=""*/)
{
	map<string,string>::const_iterator ptr=m_MapStrStr.find(keyName);
	if(ptr!=m_MapStrStr.end())
	{
	    strvalue=ptr->second;
		return;
	}
	strvalue=strDefault;
	return;
}

void CXMLParse::getValue(const std::string& keyName, float& fvalue, float fDefault/*=0.0f*/)
{
	map<string,string>::const_iterator ptr=m_MapStrStr.find(keyName);
	if(ptr!=m_MapStrStr.end())
	{
		fvalue=atof(ptr->second.c_str());
		return;
	}
	fvalue=fDefault;
	return;
}

void CXMLParse::getValue(const std::string& keyName, int& ivalue, int iDefault/*=0*/)
{
	map<string,string>::const_iterator ptr=m_MapStrStr.find(keyName);
	if(ptr!=m_MapStrStr.end())
	{
		ivalue=atof(ptr->second.c_str());
		return;
	}
	ivalue=iDefault;
	return;
}

bool CXMLParse::setValue(const std::string& keyName, const std::string& strvalue)
{
	m_MapStrStr[keyName]=strvalue;
	return true;
}

bool CXMLParse::setValue(const std::string& keyName, float fValue)
{
	char sz[100]={0};
	sprintf(sz,"%f",fValue);
	m_MapStrStr[keyName]=sz;
	return true;
}

bool CXMLParse::setValue(const std::string& keyName, int iValue)
{
	char sz[100]={0};
	sprintf(sz,"%d",iValue);
	m_MapStrStr[keyName]=sz;
	return true;
}
