#pragma once

#ifndef __AFXWIN_H__
	#error 在包含用于 PCH 的此文件之前包含“stdafx.h”
#endif

#include "GlobalUnits.h"
#include "External.h"
//////////////////////////////////////////////////////////////////////////

//应用程序对象
class CGamePlazaApp : public CWinApp
{
//	//变量定义
//public:
//	CSkinRecordFile							m_SkinRecordFile;				//界面设置

	//函数定义
public:
	//构造函数
	CGamePlazaApp();

	//重载函数
public:
	//初始化函数
	virtual BOOL InitInstance();
	CExternal m_external;
	ULONG_PTR m_lGdiPlusToken;					//GID+ 标识

	DECLARE_MESSAGE_MAP()
	virtual int ExitInstance();

public:
	void CallInnoIssMsgSelectGame(WPARAM wParam, LPARAM lParam);

public:
	//退出程序
	void ExitGame();
	void ExitGamePlaza();

public:
	void UpdateRegedit();
	/// 获得命令行
	void GetCmdLinePara(CStringArray &paraArr);

protected:
	//cxf
	//游戏大厅
	CPlazaDlg*				m_pPlazaDlg;
	//游戏登录
	CDlgLogon*				m_pDlgLogon;
	//end
};

//////////////////////////////////////////////////////////////////////////

//应用程序对象说明
extern CGamePlazaApp theApp;

//////////////////////////////////////////////////////////////////////////
