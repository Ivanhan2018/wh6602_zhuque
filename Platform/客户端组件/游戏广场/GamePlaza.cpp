#include "Stdafx.h"
#include "GamePlaza.h"
//#include "GameFrame.h"
#include "GlobalUnits.h"
#include "PlazaDlg.h"
#include "CustSite.h"
#include ".\gameplaza.h"
#include "DlgLogon.h"


#define REGEDIT_PLAZA (TEXT("Software\\WHQPGame\\Plaza"))
#define REGEDIT_PLAZA_PATH (TEXT("Path"))
#define REGEDIT_OptionLogon (TEXT("Software\\WHQPGame\\GamePlaza\\OptionLogon"))
#define REGEDIT_LogonServer (TEXT("Software\\WHQPGame\\GamePlaza\\LogonServer"))


//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CGamePlazaApp, CWinApp)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//应用程序对象说明
CGamePlazaApp theApp;

//构造函数
CGamePlazaApp::CGamePlazaApp()
{
}

//初始化函数
BOOL CGamePlazaApp::InitInstance()
{
	__super::InitInstance();

	UpdateRegedit();

#ifndef _DEBUG
#ifndef	ADMIN_PLAZA				//如果没定义，则不能多开版本
	//   用应用程序名创建信号量     
	HANDLE   hSem   =   ::CreateSemaphore(NULL,   1,   1,   MAIN_DLG_CLASSNAME);     
	//   信号量已存在？     
	//   信号量存在，则程序已有一个实例运行     
	if   (GetLastError()   ==   ERROR_ALREADY_EXISTS)     
	{
		//   关闭信号量句柄 
		::CloseHandle(hSem);
		//   寻找先前实例的主窗口     
		HWND   hWndPrevious   =   ::GetWindow(::GetDesktopWindow(),GW_CHILD);     
		while   (::IsWindow(hWndPrevious))     
		{     
			//   检查窗口是否有预设的标记?     
			//   有，则是我们寻找的主窗     
			if   (::GetProp(hWndPrevious,   MAIN_DLG_CLASSNAME))     
			{     
				//   主窗口已最小化，则恢复其大小     
				if   (::IsIconic(hWndPrevious))     
					::ShowWindow(hWndPrevious,SW_RESTORE);     

				//   将主窗激活     
				::SetForegroundWindow(hWndPrevious);     

				//   将主窗的对话框激活     
				::SetForegroundWindow(     
					::GetLastActivePopup(hWndPrevious));     

				//   退出本实例     
				return   0;     
			}     
			//   继续寻找下一个窗口     
			hWndPrevious   =   ::GetWindow(hWndPrevious,GW_HWNDNEXT);   
		}
	}
#endif
#endif
//
//#ifndef ADMIN_PLAZA
//#ifndef _DEBUG
//	bool bExistIGame=false;
//	CMutex Mutex(FALSE,MAIN_DLG_CLASSNAME,NULL);
//	if (Mutex.Lock(0)==FALSE) bExistIGame=true;
//	CWnd * pIGameWnd=CWnd::FindWindow(MAIN_DLG_CLASSNAME,NULL);
//	if (pIGameWnd!=NULL)
//	{
//		bExistIGame=true;
//		if (pIGameWnd->IsIconic()) pIGameWnd->ShowWindow(SW_RESTORE);
//		pIGameWnd->SetActiveWindow();
//		pIGameWnd->BringWindowToTop();
//		pIGameWnd->SetForegroundWindow();
//	}
//	if (bExistIGame==true) return FALSE;
//#endif
//#endif

	GdiplusStartupInput StartupInput;
	GdiplusStartup(&m_lGdiPlusToken,&StartupInput,NULL);

	//程序初始化
	AfxOleInit();
	AfxInitRichEdit();
	AfxInitRichEdit2();
	InitCommonControls();
	AfxEnableControlContainer();
	
	CCustomOccManager *pMgr = new CCustomOccManager;//br
	AfxEnableControlContainer( pMgr );

	//设置注册表
	SetRegistryKey(szSystemRegKey);


	//加载配置
	g_GlobalOption.LoadOptionParameter();

	//全局信息
	if (g_GlobalUnits.InitGlobalUnits()==false)
	{
		ShowInformation(TEXT("游戏广场初始化失败，程序即将退出！"),0,MB_ICONSTOP);
		return FALSE;
	}

	//变量定义
	WNDCLASS WndClasss;
	ZeroMemory(&WndClasss,sizeof(WndClasss));

	//注册窗口
	WndClasss.style=CS_DBLCLKS;
	WndClasss.lpfnWndProc=DefWindowProc;
	WndClasss.lpszClassName=MAIN_DLG_CLASSNAME;
	WndClasss.hInstance=AfxGetInstanceHandle();
	WndClasss.hIcon=LoadIcon(MAKEINTRESOURCE(IDR_MAINFRAMEICO));
	WndClasss.hCursor=LoadStandardCursor(MAKEINTRESOURCE(IDC_ARROW));
	if (AfxRegisterClass(&WndClasss)==FALSE) AfxThrowResourceException();

	CStringArray arr;
	GetCmdLinePara(arr);//获得命令行

	if (2 == arr.GetSize())
	{
		CString strAccount=arr.GetAt(0);
		strAccount.Replace("/account:", "");
		CString strPW=arr.GetAt(1);
		strPW.Replace("/pw:", "");
		arr.RemoveAll();
		arr.Add(strAccount);
		arr.Add(strPW);
		g_GlobalUnits.SaveApplicationCmdLine(arr);
	}

	
	//建立窗口
	//CGameFrame * pMainFrame=new CGameFrame();
	//pMainFrame->Create(MAIN_DLG_CLASSNAME,TEXT(""),/*WS_MINIMIZEBOX|WS_CLIPCHILDREN|WS_CLIPSIBLINGS*/WS_OVERLAPPEDWINDOW,CRect(0,0,0,0));
	//m_pMainWnd=pMainFrame;

	//cxf
	//创建游戏大厅
	m_pPlazaDlg = NULL;
	m_pPlazaDlg = new CPlazaDlg();
	m_external.m_palazaDlg=m_pPlazaDlg;
	if (NULL != m_pPlazaDlg)
	{
		m_pPlazaDlg->Create(IDD_PLAZADLG);
		m_pPlazaDlg->ShowWindow(SW_HIDE);
	}
	else
	{
		m_pPlazaDlg = NULL;
		return TRUE;
	}

	//创建游戏登录窗口
	m_pDlgLogon = NULL;
	m_pDlgLogon = new CDlgLogon();
	if (NULL != m_pDlgLogon)
	{
		m_pDlgLogon->Create(IDD_LOGON);
		m_pPlazaDlg->SetDlgLogonPtr(m_pDlgLogon);
	}
	else
	{
		m_pDlgLogon = NULL;
		return TRUE;
	}

	//设置主窗口 
	m_pMainWnd = m_pPlazaDlg;
	
	//显示窗口
	m_pPlazaDlg->ShowTab(0);

#ifndef	ADMIN_PLAZA				//如果没定义，则不能多开版本
	::SetProp(m_pMainWnd->m_hWnd, MAIN_DLG_CLASSNAME, (HANDLE)1);
#endif

	/*cxf 2011-09-20
	CPlazaDlg dlg; 
	m_external.m_palazaDlg=&dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: 在此处放置处理何时用“确定”来关闭
		//  对话框的代码
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: 在此放置处理何时用“取消”来关闭
		//  对话框的代码
	}
	*/

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//退出程序
void CGamePlazaApp::ExitGame()
{
	if (NULL != m_pDlgLogon)
	{
		m_pDlgLogon->ExitLogon();
	}
	if (NULL != m_pPlazaDlg)
	{
		m_pPlazaDlg->OnBnCancelGame();
	}

	if (NULL != m_pDlgLogon)
	{
		delete m_pDlgLogon;
	}
	m_pDlgLogon = NULL;

	if (NULL != m_pPlazaDlg)
	{
		delete m_pPlazaDlg;
	}
	m_pPlazaDlg = NULL;
}

//退出程序
void CGamePlazaApp::ExitGamePlaza()
{
	if (NULL != m_pDlgLogon)
	{
		m_pDlgLogon->ExitLogon();
	}
	if (NULL != m_pDlgLogon)
	{
		delete m_pDlgLogon;
	}
	m_pDlgLogon = NULL;

	if (NULL != m_pPlazaDlg)
	{
		delete m_pPlazaDlg;
	}
	m_pPlazaDlg = NULL;
}

int CGamePlazaApp::ExitInstance()
{
	// TODO: 在此添加专用代码和/或调用基类
	GdiplusShutdown(m_lGdiPlusToken);

	return CWinApp::ExitInstance();
}

void CGamePlazaApp::CallInnoIssMsgSelectGame(WPARAM wParam, LPARAM lParam)
{
	if (NULL != m_pPlazaDlg)
	{
		m_pPlazaDlg->OnCallInnoIssMsgSelectGame(wParam, lParam);
	}
}
void CGamePlazaApp::UpdateRegedit()
{
	DWORD dwType;
	TCHAR ch[MAX_PATH] = {TEXT("")};
	DWORD dwCount = sizeof(ch);
	GetModuleFileName(NULL, ch, sizeof(ch))  ;
	TCHAR * pCH = strrchr(ch, '\\');
	if (NULL != pCH)
	{
		*pCH = '\0';
	}
	SetCurrentDirectory(ch);

	HKEY hAppKey = NULL;
	HKEY hSoftKey = NULL;
	HKEY hPathKey = NULL;
	if (RegOpenKeyEx(HKEY_CURRENT_USER, REGEDIT_PLAZA, 0, KEY_WRITE|KEY_READ,&hSoftKey) != ERROR_SUCCESS)
	{
		if (RegCreateKey(HKEY_CURRENT_USER, REGEDIT_PLAZA, &hSoftKey) != ERROR_SUCCESS)
		{
			return;
		}
	}

// 	wchar_t * pCh = wcsrchr(ch, '\\');
// 	if (NULL != pCh) *pCh = '\0';
	
	if (NULL != hSoftKey)
	{
		if (RegSetValueEx(hSoftKey, REGEDIT_PLAZA_PATH, NULL, REG_SZ, (LPBYTE)ch, sizeof(ch)) != ERROR_SUCCESS)
		{
			AfxMessageBox(TEXT("写注册表失败~！"));
		}
	}
	if (hSoftKey != NULL)
		RegCloseKey(hSoftKey);
	if (hPathKey != NULL)
		RegCloseKey(hPathKey);
}

//字符串分解
void CGamePlazaApp::GetCmdLinePara(CStringArray &paraArr)
{
	paraArr.RemoveAll();
	CString strLine=::AfxGetApp()->m_lpCmdLine;
	if(strLine.IsEmpty())
		return;

	int nLength=strLine.GetLength();
	char *buf=new char[nLength+1];
	lstrcpyn(buf, strLine, sizeof(char)*(nLength+1));
	char *p=buf;
	for(int i=0;i<128;i++)
	{
		if(buf[i]==0x20)//空格
		{
			buf[i]=0x00;
			paraArr.Add(p);
			i++;
			p=buf+i;
		}
		if(buf[i]==0x00)
		{
			paraArr.Add(p);
			break;
		}
	}

	SafeDeleteArray(buf);
}