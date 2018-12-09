#ifndef SERVER_PARAMETER_HEAD_FILE
#define SERVER_PARAMETER_HEAD_FILE

#pragma once

#include "Stdafx.h"

//////////////////////////////////////////////////////////////////////////////////

//配置参数
class CServerParameter
{
public:
	//地址信息
	struct tagAddressInfo
	{
		TCHAR							szAddress[32];						//服务地址
	};

	//数据信息
	struct tagDataBaseParameter
	{
		WORD							wDataBasePort;						//数据库端口
		TCHAR							szDataBaseAddr[32];					//数据库地址
		TCHAR							szDataBaseUser[32];					//数据库用户
		TCHAR							szDataBasePass[32];					//数据库密码
		TCHAR							szDataBaseName[32];					//数据库名字
	};

	//系统配置
public:
	WORD							m_wMaxConnect;						//最大连接
	WORD							m_wServicePort;						//服务端口
	TCHAR							m_szServerName[32];			        //服务器名

	//组件时间
public:
	WORD							m_wConnectTime;						//重连时间
	WORD							m_wCollectTime;						//统计时间
	WORD                            m_wLoadVCountTime;					//加载时间 
	WORD							m_wLoadListTime;					//列表时间
	WORD							m_wReLoadListTime;					//列表时间

	//客户时间
public:
	WORD							m_wIntermitTime;					//中断时间
	WORD							m_wOnLineCountTime;					//人数时间

	DWORD							m_dwMainVer;						//大厅主版本号   第一位
	DWORD							m_dwSubVer;							//大厅副版本号		第二位
	DWORD							m_dwProductVer;						//大厅主版本号		第三位
	//协调信息
public:
	WORD							m_wCorrespondPort;					//协调端口
	tagAddressInfo					m_CorrespondAddress;				//协调地址

	//服务地址
public:
	tagAddressInfo					m_ServiceAddress;					//服务地址
	tagDataBaseParameter			m_AccountsDBParameter;				//连接地址
	tagDataBaseParameter			m_TreasureDBParameter;				//连接地址
	tagDataBaseParameter			m_PlatformDBParameter;				//连接地址
	tagDataBaseParameter			m_GameSSCDBParameter;				//连接地址

	//函数定义
public:
	//构造函数
	CServerParameter();
	//析构函数
	virtual ~CServerParameter();

	//功能函数
public:
	//初始化
	VOID ServerParameter();
	//加载配置
	VOID LoadServerParameter();

	//工作目录
	static bool GetWorkDirectory(TCHAR szWorkDirectory[], WORD wBufferCount);
	//6603类型转换为6602类型
	static tagDataBaseInfo wh6603_to_2(tagDataBaseParameter * p);
};

//////////////////////////////////////////////////////////////////////////////////

#endif
