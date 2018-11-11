// GameUpdate.h : main header file for the GameUpdate application
//

#if !defined(AFX_GameUpdate_H__187D3C3A_DD05_4283_9FCA_1FAF58B23B29__INCLUDED_)
#define AFX_GameUpdate_H__187D3C3A_DD05_4283_9FCA_1FAF58B23B29__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

#include "MultiLanguage.h"

// 命令行参数名
#define PARA_KEY_APP_NAME "/AppName="
#define PARA_KEY_CURRENTVERSION "/CurrentVersion="
#define PARA_KEY_CHECKURL "/CheckURL="
#define PARA_KEY_NOTIFYWINDOW "/NotifyWindow="
#define PARA_KEY_NOTIFYWINDOWTITLE "/NotifyWindowTitle="
#define PARA_KEY_NOTIFYFINISH "/NotifyFinish="
#define PARA_KEY_SILENCE "/Silence="


// 升级配置文件中的区段名
#define SECTION_UPDATE "UPDATE"
#define SECTION_COMMON "COMMON"
#define SECTION_LANGUAGE "LANGUAGE"


/////////////////////////////////////////////////////////////////////////////
// CGameUpdateApp:
// See GameUpdate.cpp for the implementation of this class
//

class CGameUpdateApp : public CWinApp
{
public:
	CGameUpdateApp();
	void InitStringTable(enum enLANGUAGE Language = LANGUAGE_BOTTOM);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGameUpdateApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CGameUpdateApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	//BOOL ParseCommandLine();			// 处理命令行
	BOOL IsAppRunning();
	//BOOL CheckUpdate();					// 从软件网站下载升级配置文件，检查是否有新版本的软件可用
	void GetCmdLinePara(CStringArray &paraArr);

private:
	CString m_sAppName;					// 应用程序名，用以创建互斥量
	//CString m_sVersion;					// 应用程序的当前版本
	//CString m_sURL;						// 下载升级配置文件的URL
	//DWORD m_iNotifyWindow;				// 升级过程中发送消息的目标窗口句柄
	//CString m_sNotifyWindowTitle;		// 升级过程中发送消息的目标窗口标题
	//DWORD m_iNotifyFinish;				// 升级完毕发送的消息代码
	BOOL m_bSilenceMode;				// 静默方式执行升级，不显示升级程序界面，只在升级完毕后提醒用户
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GameUpdate_H__187D3C3A_DD05_4283_9FCA_1FAF58B23B29__INCLUDED_)
