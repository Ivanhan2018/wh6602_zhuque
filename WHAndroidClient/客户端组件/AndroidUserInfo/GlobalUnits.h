#ifndef GLOBAL_UNITS_HEAD_FILE
#define GLOBAL_UNITS_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "AndroidUserInfoEx.h"
#include "UserGameDataDef.h"



//////////////////////////////////////////////////////////////////////////


//全局信息类
 class ANDROID_USER_INFO_EX CGlobalUnits
{
	//系统变量
protected:
	DWORD								m_dwPlazaVersion;				//大厅版本
	TCHAR								m_szDirWork[MAX_PATH];			//工作目录

public:
	bool								m_bInitGameKindList;			//已经初始化
	GameOption							m_GameOption;					//游戏配置
	std::vector<GameServerList>			m_GameKindList;					//游戏列表

	//函数定义
public:
	//构造函数
	CGlobalUnits(void);
	//析构函数
	virtual ~CGlobalUnits(void);

	//初始化函数
	bool InitGlobalUnits();

	//功能函数
public:
	//获取序列
	void GetClientSerial(tagClientSerial & ClientSerial);

	//查询函数
public:
	//大厅版本
	DWORD GetPlazaVersion() { return m_dwPlazaVersion; }
	//工作目录
	LPCTSTR GetWorkDirectory() { return m_szDirWork; }
	//获得当前可进入的房间信息
	bool GetCurCanEnterRoomInfo(DWORD& dwServerAddr, DWORD& dwServerPort);

	//内部函数
private:
	//网卡地址
	WORD GetMACAddress(BYTE cbMacBuffer[], WORD wBufferSize);
};

//全局信息 
ANDROID_USER_INFO_EX extern CGlobalUnits	g_GlobalUnits;					//信息组件	
//////////////////////////////////////////////////////////////////////////

#endif