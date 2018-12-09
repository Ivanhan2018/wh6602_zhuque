#include "StdAfx.h"
#include "ServerParameter.h"
#include "WHIniData.h"
//////////////////////////////////////////////////////////////////////////////////

//服务时间
#define TIME_CONNECT				15									//重连时间
#define TIME_COLLECT				300									//统计时间
#define TIME_LOAD_LIST				300									//加载列表
#define TIME_RELOAD_LIST			600									//加载列表
#define TIME_VIRTUAL_COUNT          60                                  //虚拟人数

//客户时间
#define TIME_INTERMIT				0									//中断时间
#define TIME_ONLINE_COUNT			600									//人数时间

//////////////////////////////////////////////////////////////////////////////////
//构造函数
CServerParameter::CServerParameter()
{ 
	ServerParameter();
}

//析构函数
CServerParameter::~CServerParameter()
{
}

//初始化
VOID CServerParameter::ServerParameter()
{
	//时间定义
	m_wConnectTime=TIME_CONNECT;
	m_wCollectTime=TIME_COLLECT;
	m_wLoadListTime=TIME_LOAD_LIST;
	m_wReLoadListTime=TIME_RELOAD_LIST;
	m_wLoadVCountTime = TIME_VIRTUAL_COUNT;

	//客户时间
	m_wIntermitTime=TIME_INTERMIT;
	m_wOnLineCountTime=TIME_ONLINE_COUNT;

	//系统配置
	m_wMaxConnect=512;
	m_wServicePort=PORT_LOGON_SERVER;
	ZeroMemory(m_szServerName,sizeof(m_szServerName));

	m_dwMainVer = 18;
	m_dwSubVer = 0;
	m_dwProductVer = 6;
	//协调信息
	m_wCorrespondPort=PORT_CENTER_SERVER;
	ZeroMemory(&m_CorrespondAddress,sizeof(m_CorrespondAddress));

	//连接信息
	ZeroMemory(&m_ServiceAddress,sizeof(m_ServiceAddress));
	ZeroMemory(&m_AccountsDBParameter,sizeof(m_AccountsDBParameter));
	ZeroMemory(&m_TreasureDBParameter,sizeof(m_TreasureDBParameter));
	ZeroMemory(&m_PlatformDBParameter,sizeof(m_PlatformDBParameter));
	ZeroMemory(&m_GameSSCDBParameter,sizeof(m_GameSSCDBParameter));

	return;
}

//加载配置
VOID CServerParameter::LoadServerParameter()
{
	//重置参数
	ServerParameter();

	//获取路径
	TCHAR szWorkDir[MAX_PATH]=TEXT("");
	CServerParameter::GetWorkDirectory(szWorkDir,CountArray(szWorkDir));

	//构造路径
	TCHAR szIniFile[MAX_PATH]=TEXT("");
	_sntprintf(szIniFile,CountArray(szIniFile),TEXT("%s\\ServerParameter.ini"),szWorkDir);

	//读取配置
	CWHIniData IniData;
	IniData.SetIniFilePath(szIniFile);

	//读取配置
	m_wMaxConnect=IniData.ReadInt(TEXT("LogonServer"),TEXT("ConnectMax"),m_wMaxConnect);
	m_wServicePort=IniData.ReadInt(TEXT("LogonServer"),TEXT("ServicePort"),m_wServicePort);
	IniData.ReadEncryptString(TEXT("ServerInfo"),TEXT("ServiceName"),NULL,m_szServerName,CountArray(m_szServerName));
	IniData.ReadEncryptString(TEXT("ServerInfo"),TEXT("ServiceAddr"),NULL,m_ServiceAddress.szAddress,CountArray(m_ServiceAddress.szAddress));

	//协调信息
	m_wCorrespondPort=IniData.ReadInt(TEXT("Correspond"),TEXT("ServicePort"),m_wCorrespondPort);
	IniData.ReadEncryptString(TEXT("ServerInfo"),TEXT("CorrespondAddr"),NULL,m_CorrespondAddress.szAddress,CountArray(m_CorrespondAddress.szAddress));

	//连接信息
	m_AccountsDBParameter.wDataBasePort=(WORD)IniData.ReadInt(TEXT("AccountsDB"),TEXT("DBPort"),1433);
	IniData.ReadEncryptString(TEXT("AccountsDB"),TEXT("DBAddr"),NULL,m_AccountsDBParameter.szDataBaseAddr,CountArray(m_AccountsDBParameter.szDataBaseAddr));
	IniData.ReadEncryptString(TEXT("AccountsDB"),TEXT("DBUser"),NULL,m_AccountsDBParameter.szDataBaseUser,CountArray(m_AccountsDBParameter.szDataBaseUser));
	IniData.ReadEncryptString(TEXT("AccountsDB"),TEXT("DBPass"),NULL,m_AccountsDBParameter.szDataBasePass,CountArray(m_AccountsDBParameter.szDataBasePass));
	IniData.ReadEncryptString(TEXT("AccountsDB"),TEXT("DBName"),szGameUserDB,m_AccountsDBParameter.szDataBaseName,CountArray(m_AccountsDBParameter.szDataBaseName));

	//连接信息
	m_TreasureDBParameter.wDataBasePort=(WORD)IniData.ReadInt(TEXT("TreasureDB"),TEXT("DBPort"),1433);
	IniData.ReadEncryptString(TEXT("TreasureDB"),TEXT("DBAddr"),NULL,m_TreasureDBParameter.szDataBaseAddr,CountArray(m_TreasureDBParameter.szDataBaseAddr));
	IniData.ReadEncryptString(TEXT("TreasureDB"),TEXT("DBUser"),NULL,m_TreasureDBParameter.szDataBaseUser,CountArray(m_TreasureDBParameter.szDataBaseUser));
	IniData.ReadEncryptString(TEXT("TreasureDB"),TEXT("DBPass"),NULL,m_TreasureDBParameter.szDataBasePass,CountArray(m_TreasureDBParameter.szDataBasePass));
	IniData.ReadEncryptString(TEXT("TreasureDB"),TEXT("DBName"),szTreasureDB,m_TreasureDBParameter.szDataBaseName,CountArray(m_TreasureDBParameter.szDataBaseName));

	//连接信息
	m_PlatformDBParameter.wDataBasePort=(WORD)IniData.ReadInt(TEXT("PlatformDB"),TEXT("DBPort"),1433);
	IniData.ReadEncryptString(TEXT("PlatformDB"),TEXT("DBAddr"),NULL,m_PlatformDBParameter.szDataBaseAddr,CountArray(m_PlatformDBParameter.szDataBaseAddr));
	IniData.ReadEncryptString(TEXT("PlatformDB"),TEXT("DBUser"),NULL,m_PlatformDBParameter.szDataBaseUser,CountArray(m_PlatformDBParameter.szDataBaseUser));
	IniData.ReadEncryptString(TEXT("PlatformDB"),TEXT("DBPass"),NULL,m_PlatformDBParameter.szDataBasePass,CountArray(m_PlatformDBParameter.szDataBasePass));
	IniData.ReadEncryptString(TEXT("PlatformDB"),TEXT("DBName"),szServerInfoDB,m_PlatformDBParameter.szDataBaseName,CountArray(m_PlatformDBParameter.szDataBaseName));

	//游戏开奖库 连接信息
	m_GameSSCDBParameter.wDataBasePort=(WORD)IniData.ReadInt(TEXT("PlatformDB"),TEXT("DBPort"),1433);
	IniData.ReadEncryptString(TEXT("PlatformDB"),TEXT("DBAddr"),NULL,m_GameSSCDBParameter.szDataBaseAddr,CountArray(m_GameSSCDBParameter.szDataBaseAddr));
	IniData.ReadEncryptString(TEXT("PlatformDB"),TEXT("DBUser"),NULL,m_GameSSCDBParameter.szDataBaseUser,CountArray(m_GameSSCDBParameter.szDataBaseUser));
	IniData.ReadEncryptString(TEXT("PlatformDB"),TEXT("DBPass"),NULL,m_GameSSCDBParameter.szDataBasePass,CountArray(m_GameSSCDBParameter.szDataBasePass));
	//IniData.ReadEncryptString(TEXT("PlatformDB"),TEXT("DBName"),TEXT("Lottery"),TEXT("Lottery"),CountArray(m_GameSSCDBParameter.szDataBaseName));
	_sntprintf(m_GameSSCDBParameter.szDataBaseName,CountArray(m_GameSSCDBParameter.szDataBaseName),TEXT("LuckMeDB"));
	_sntprintf(szIniFile,CountArray(szIniFile),TEXT("%s\\ServerConfig.ini"),szWorkDir);

	m_dwMainVer = GetPrivateProfileInt(TEXT("VersionPlaza"),TEXT("MainVer"),18,szIniFile);
	m_dwSubVer =  GetPrivateProfileInt(TEXT("VersionPlaza"),TEXT("SubVer"),0,szIniFile);
	m_dwProductVer = GetPrivateProfileInt(TEXT("VersionPlaza"),TEXT("ProductVer"),6,szIniFile);

	return;
}

//进程目录
bool CServerParameter::GetWorkDirectory(TCHAR szWorkDirectory[], WORD wBufferCount)
{
	//模块路径
	TCHAR szModulePath[MAX_PATH]=TEXT("");
	GetModuleFileName(AfxGetInstanceHandle(),szModulePath,CountArray(szModulePath));

	//分析文件
	for (INT i=lstrlen(szModulePath);i>=0;i--)
	{
		if (szModulePath[i]==TEXT('\\'))
		{
			szModulePath[i]=0;
			break;
		}
	}

	//设置结果
	ASSERT(szModulePath[0]!=0);
	lstrcpyn(szWorkDirectory,szModulePath,wBufferCount);

	return true;
}

tagDataBaseInfo CServerParameter::wh6603_to_2(tagDataBaseParameter * p)
{
	tagDataBaseInfo DataBaseInfo;
	if(p)
	{
		DataBaseInfo.wDataBasePort=p->wDataBasePort;
		DataBaseInfo.dwDataBaseAddr=inet_addr(p->szDataBaseAddr);
		lstrcpyn(DataBaseInfo.szDataBaseUser,p->szDataBaseUser,CountArray(DataBaseInfo.szDataBaseUser));
		lstrcpyn(DataBaseInfo.szDataBasePass,p->szDataBasePass,CountArray(DataBaseInfo.szDataBasePass));
		lstrcpyn(DataBaseInfo.szDataBaseName,p->szDataBaseName,CountArray(DataBaseInfo.szDataBaseName));	
	}
	return DataBaseInfo;
}

//////////////////////////////////////////////////////////////////////////////////
