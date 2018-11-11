#include "StdAfx.h"
#include ".\globaluserset.h"


#define REG_USER_INFO			TEXT("Software\\WHQPGame\\GamePlaza\\OptionLogon")
#define REG_OPTION_LOGON		TEXT("OptionLogon")

CGlobalUserSet					g_GolbalUserSet;

CGlobalUserSet::CGlobalUserSet()
{
	m_nBackMusic=100;
	m_nGameMusic=100;
	m_nManMusic=100;
	m_nCartoonMusic=100;
	m_nLanguage=0;
	InitGlobal();
}
CGlobalUserSet::~CGlobalUserSet()
{
	UnitGlobal();
}

void CGlobalUserSet::InitGlobal()
{
	//变量定义
	CRegKey RegKey;
	RegKey.Open(HKEY_CURRENT_USER,REG_USER_INFO,KEY_READ);
	if (RegKey != NULL)
	{
		DWORD dwValue=0;
		RegKey.QueryDWORDValue(TEXT("BackMusic"), dwValue);
		m_nBackMusic = dwValue;

		//dwValue=0;
		//RegKey.QueryDWORDValue(TEXT("GameMusic"), dwValue);
		//m_nGameMusic = dwValue;

		//dwValue=0;
		//RegKey.QueryDWORDValue(TEXT("ManMusic"), dwValue);
		//m_nManMusic = dwValue;

		//dwValue=0;
		//RegKey.QueryDWORDValue(TEXT("CartoonMusic"), dwValue);
		//m_nCartoonMusic = dwValue;

		dwValue=0;
		RegKey.QueryDWORDValue(TEXT("Language"), dwValue);
		m_nLanguage = dwValue;
		RegKey.Close();
	}

}

void CGlobalUserSet::UnitGlobal()
{
	CRegKey RegKey;
	RegKey.Open(HKEY_CURRENT_USER,REG_USER_INFO,KEY_ALL_ACCESS);
	if (RegKey != 0)
	{
		DWORD dwValue=m_nBackMusic;
		RegKey.SetDWORDValue(TEXT("BackMusic"), dwValue);

		//dwValue=m_nGameMusic;
		//RegKey.SetDWORDValue(TEXT("GameMusic"), dwValue);

		//dwValue=m_nManMusic;
		//RegKey.SetDWORDValue(TEXT("ManMusic"), dwValue);

		//dwValue=m_nCartoonMusic;
		//RegKey.SetDWORDValue(TEXT("CartoonMusic"), dwValue);

		dwValue=m_nLanguage;
		RegKey.SetDWORDValue(TEXT("Language"), dwValue);

		RegKey.Close();
	}
}