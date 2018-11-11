#include "Stdafx.h"
#include "ServiceLoader.h"
#include "ServiceLoaderDlg.h"

//应用程序对象
CServiceLoaderApp theApp;

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CServiceLoaderApp, CWinApp)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CServiceLoaderApp::CServiceLoaderApp() : CWinApp(TEXT("游戏服务"))
{
}

//初始化函数
BOOL CServiceLoaderApp::InitInstance()
{
	__super::InitInstance();

	TCHAR buf[126] = {0};
	CMD5Encrypt::EncryptData("2jiw6lvj82011-11-15 11:43:13192.168.1.91", buf);

	//内部初始化
	AfxInitRichEdit2();
	InitCommonControls();
	AfxEnableControlContainer();
	SetRegistryKey(szSystemRegKey);

	//设置工作目录
	TCHAR szDirWork[MAX_PATH]=TEXT("");
	GetModuleFileName(AfxGetInstanceHandle(),szDirWork,sizeof(szDirWork));
	int nModuleLen=lstrlen(szDirWork);
	int nProcessLen=lstrlen(AfxGetApp()->m_pszExeName)+lstrlen(TEXT(".EXE"));
	szDirWork[nModuleLen-nProcessLen]=0;
	SetCurrentDirectory(szDirWork);

	//创建窗口
	CServiceLoaderDlg ServiceLoaderDlg;
	ServiceLoaderDlg.DoModal();




	return FALSE;
}

//////////////////////////////////////////////////////////////////////////
