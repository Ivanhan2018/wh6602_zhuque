// FlashEnter.cpp : 定义应用程序的类行为。
//

#include "stdafx.h"
#include "FlashEnter.h"
#include "GlobalUnits.h"
#include "FlashEnterDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CFlashEnterApp

BEGIN_MESSAGE_MAP(CFlashEnterApp, CWinApp)
END_MESSAGE_MAP()


// CFlashEnterApp 构造

CFlashEnterApp::CFlashEnterApp()
{
	// TODO: 在此处添加构造代码，
}


// 唯一的一个 CFlashEnterApp 对象

CFlashEnterApp theApp;


// CFlashEnterApp 初始化

BOOL CFlashEnterApp::InitInstance()
{
	// 如果一个运行在 Windows XP 上的应用程序清单指定要
	// 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
	//则需要 InitCommonControls()。否则，将无法创建窗口。
	CWinApp::InitInstance();

	//程序初始化
	AfxOleInit();
	InitCommonControls();

	g_GlobalUnits.InitGlobalUnits();

	CFlashEnterDlg dlg;
	m_pMainWnd=&dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: 在此放置处理何时用“确定”来关闭
		//对话框的代码
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: 在此放置处理何时用“取消”来关闭
		//对话框的代码
	}
	return TRUE;
}

int CFlashEnterApp::ExitInstance()
{
	// TODO: 在此添加专用代码和/或调用基类
	return CWinApp::ExitInstance();
}
