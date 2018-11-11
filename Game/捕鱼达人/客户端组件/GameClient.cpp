#include "Stdafx.h"
#include "GameClient.h"
#include "GameClientDlg.h"
#include ".\gameclient.h"

//////////////////////////////////////////////////////////////////////////

//程序对象
CGameClientApp theApp;

//////////////////////////////////////////////////////////////////////////

//构造函数
CGameClientApp::CGameClientApp()
{
	//AfxMessageBox("ok");
	//#if _DEBUG
	//	DebugBreak();	// 测试用
	//#endif
}

//析构函数
CGameClientApp::~CGameClientApp()
{
}

//////////////////////////////////////////////////////////////////////////

BOOL CGameClientApp::InitInstance()
{
	//设置工作目录
	ZeroMemory(g_szWorkDir,sizeof(g_szWorkDir));
	GetModuleFileName(AfxGetInstanceHandle(),g_szWorkDir,sizeof(g_szWorkDir));
	int nModuleLen=lstrlen(g_szWorkDir);
	int nProcessLen=lstrlen(AfxGetApp()->m_pszExeName)+lstrlen(TEXT(".EXE"));
	g_szWorkDir[nModuleLen-nProcessLen]=0;
	SetCurrentDirectory(g_szWorkDir);
	if ('\\'!=g_szWorkDir[lstrlen(g_szWorkDir)-1]) g_szWorkDir[lstrlen(g_szWorkDir)-1]='\\';

	return CGameFrameApp::InitInstance();
}
