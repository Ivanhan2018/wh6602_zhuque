#include "Stdafx.h"
#include "AtlBase.h"
#include "LogonServerDlg.h"

#include "SoapRecharge/soapH.h"

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CSystemOptionDlg, CDialog)
	ON_EN_CHANGE(IDC_MAIN_PAGE, OnEnChangeEditCity)
END_MESSAGE_MAP()

BEGIN_MESSAGE_MAP(CLogonServerDlg, CDialog)
	ON_BN_CLICKED(IDC_START_SERVICE, OnBnClickedStartService)
	ON_BN_CLICKED(IDC_STOP_SERVICE, OnBnClickedStopService)
	ON_BN_CLICKED(IDC_INIT_SERVICE, OnBnClickedInitService)
	ON_MESSAGE(WM_SOAP_MESSAGE, OnSoapMessage)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//soapRecharge
UINT SoapThread(LPVOID pParam) 
{
// 	RechargeResultService rechSer;
// 	if (rechSer.run((int)pParam))
// 	{
// 		rechSer.soap_stream_fault(std::cerr);
// 	}
	
	int m, s;
	struct soap add_soap;
	soap_init(&add_soap);
		
	m = soap_bind(&add_soap, NULL, (int)pParam, 100);
	if (m < 0)
	{
		soap_print_fault(&add_soap, stderr);
		exit(-1);
	}

	fprintf(stderr, "Socket connection successful: master socket = %d\n", m);
	for ( ; ; )
	{
		s = soap_accept(&add_soap);
		if (s < 0)
		{
			soap_print_fault(&add_soap, stderr);
			exit(-1);
		}
		fprintf(stderr, "Socket connection successful: slave socket = %d\n", s);

		soap_serve(&add_soap);//该句说明该server的服务
		soap_end(&add_soap);
	}
	
 	return true;
}

//构造函数
CSystemOptionDlg::CSystemOptionDlg() : CDialog(IDD_SYSTEM_OPTION)
{
}

//析构函数
CSystemOptionDlg::~CSystemOptionDlg()
{
}

//编辑框变化
void CSystemOptionDlg::OnEnChangeEditCity()
{
	//加载参数
//	CInitParamter InitParamter;

	//区域
//	GetDlgItemText(IDC_MAIN_PAGE,InitParamter.m_szMainPage,sizeof(InitParamter.m_szMainPage));

	//根据区域刷新区域相关数据
//	InitParamter.LoadAreaData();

	//更新界面数据
//	SetDlgItemInt(IDC_LISTEN_PORT,InitParamter.m_wListenPort,FALSE);

	return;
}

//控件子类化
void CSystemOptionDlg::DoDataExchange(CDataExchange * pDX)
{
	__super::DoDataExchange(pDX);
}

//初始化函数
BOOL CSystemOptionDlg::OnInitDialog()
{
	__super::OnInitDialog();

	//限制输入
	((CEdit *)GetDlgItem(IDC_LISTEN_PORT))->LimitText(5);
	((CEdit *)GetDlgItem(IDC_MAX_CONNECT))->LimitText(3);
	((CEdit *)GetDlgItem(IDC_MAIN_PAGE))->LimitText(31);
	((CEdit *)GetDlgItem(IDC_USER_DATABASE_PORT))->LimitText(5);
	((CEdit *)GetDlgItem(IDC_USER_DATABASE_USER))->LimitText(31);
	((CEdit *)GetDlgItem(IDC_USER_DATABASE_PASS))->LimitText(31);
	((CEdit *)GetDlgItem(IDC_USER_DATABASE_NAME))->LimitText(31);

	//加载参数
//	CInitParamter InitParamter;
//	InitParamter.LoadInitParamter();

	//设置控件
	SetDlgItemInt(IDC_CENTER_PORT,g_InitParamter.m_wCenterServerPort,FALSE);
	SetDlgItemInt(IDC_LISTEN_PORT,g_InitParamter.m_wListenPort,FALSE);
	SetDlgItemInt(IDC_MAX_CONNECT,g_InitParamter.m_wMaxConnect,FALSE);

	//登录数据库
	SetDlgItemInt(IDC_USER_DATABASE_PORT,g_InitParamter.m_wUserDataBasePort,FALSE);
	SetDlgItemText(IDC_USER_DATABASE_USER,g_InitParamter.m_szUserDataBaseUser);
	SetDlgItemText(IDC_USER_DATABASE_PASS,g_InitParamter.m_szUserDataBasePass);
	SetDlgItemText(IDC_USER_DATABASE_NAME,g_InitParamter.m_szUserDataBaseName);

	//登录数据库地址
	DWORD dwDataBaseIP=inet_addr(g_InitParamter.m_szUserDataBaseAddr);
	if (dwDataBaseIP==INADDR_NONE)
	{
		LPHOSTENT lpHost=gethostbyname(g_InitParamter.m_szUserDataBaseAddr);
		if (lpHost!=NULL) dwDataBaseIP=((LPIN_ADDR)lpHost->h_addr)->s_addr;
	}
	CIPAddressCtrl * pDataBaseIP=(CIPAddressCtrl *)GetDlgItem(IDC_USER_DATABASE_IP);
	pDataBaseIP->SetAddress(ntohl(dwDataBaseIP));

	//主站地址
	if (g_InitParamter.m_szMainPage[0]==0) SetDlgItemText(IDC_MAIN_PAGE,szStationPage);
	else SetDlgItemText(IDC_MAIN_PAGE,g_InitParamter.m_szMainPage);

	//中心服务器
	dwDataBaseIP=inet_addr(g_InitParamter.m_szCenterServerAddr);
	if (dwDataBaseIP==INADDR_NONE)
	{
		LPHOSTENT lpHost=gethostbyname(g_InitParamter.m_szCenterServerAddr);
		if (lpHost!=NULL) dwDataBaseIP=((LPIN_ADDR)lpHost->h_addr)->s_addr;
	}
	pDataBaseIP=(CIPAddressCtrl *)GetDlgItem(IDC_CENTER_SERVER_IP);
	pDataBaseIP->SetAddress(ntohl(dwDataBaseIP));

	return TRUE;
}

//确定函数
void CSystemOptionDlg::OnOK()
{
	//获取输入
	CInitParamter InitParamter;
	InitParamter.m_wListenPort=GetDlgItemInt(IDC_LISTEN_PORT);
	InitParamter.m_wMaxConnect=GetDlgItemInt(IDC_MAX_CONNECT);

	//登录数据库 
	InitParamter.m_wUserDataBasePort=GetDlgItemInt(IDC_USER_DATABASE_PORT);
	GetDlgItemText(IDC_USER_DATABASE_USER,InitParamter.m_szUserDataBaseUser,sizeof(InitParamter.m_szUserDataBaseUser));
	GetDlgItemText(IDC_USER_DATABASE_PASS,InitParamter.m_szUserDataBasePass,sizeof(InitParamter.m_szUserDataBasePass));
	GetDlgItemText(IDC_USER_DATABASE_NAME,InitParamter.m_szUserDataBaseName,sizeof(InitParamter.m_szUserDataBaseName));

	//登录数据库地址
	DWORD dwDataBaseIP=INADDR_NONE;
	BYTE * pAddrByte=(BYTE *)&dwDataBaseIP;
	((CIPAddressCtrl *)GetDlgItem(IDC_USER_DATABASE_IP))->GetAddress(dwDataBaseIP);
	_snprintf(InitParamter.m_szUserDataBaseAddr,sizeof(InitParamter.m_szUserDataBaseAddr),TEXT("%d.%d.%d.%d"),
		pAddrByte[3],pAddrByte[2],pAddrByte[1],pAddrByte[0]);

	//主站地址
	GetDlgItemText(IDC_MAIN_PAGE,InitParamter.m_szMainPage,sizeof(InitParamter.m_szMainPage));
//	if (lstrcmp(InitParamter.m_szMainPage,szStationPage)==0) InitParamter.m_szMainPage[0]=0;

	//中心服务器
	dwDataBaseIP=INADDR_NONE;
	pAddrByte=(BYTE *)&dwDataBaseIP;
	((CIPAddressCtrl *)GetDlgItem(IDC_CENTER_SERVER_IP))->GetAddress(dwDataBaseIP);
	_snprintf(InitParamter.m_szCenterServerAddr,sizeof(InitParamter.m_szCenterServerAddr),TEXT("%d.%d.%d.%d"),
		pAddrByte[0],pAddrByte[1],pAddrByte[2],pAddrByte[3]);

	//保存设置
	InitParamter.SaveInitParamter(true);

	__super::OnOK();
}

//////////////////////////////////////////////////////////////////////////

//构造函数
CLogonServerDlg::CLogonServerDlg() : CDialog(IDD_LOGONSERVER_DIALOG)
{
}

//析构函数
CLogonServerDlg::~CLogonServerDlg()
{
}

//控件子类化
void CLogonServerDlg::DoDataExchange(CDataExchange * pDX)
{
	__super::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SERVICE_EVENT, m_RichEditTrace);
}

//初始化函数
BOOL CLogonServerDlg::OnInitDialog()
{
	__super::OnInitDialog();

	//设置图标
	HICON hIcon=LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME));
	SetIcon(hIcon,TRUE);
	SetIcon(hIcon,FALSE);

	//加载配置
	LPCTSTR pszCmdLine=AfxGetApp()->m_lpCmdLine;
	if (pszCmdLine[0]!=0)
	{
		CFileFind finder;   //查找是否存在ini文件，若不存在，则生成一个新的默认设置的ini文件
		BOOL ifFind = finder.FindFile(pszCmdLine);
		if( !ifFind )
		{
			//输出事件
			CTraceService::TraceString(TEXT("ini文件不存在"),TraceLevel_Normal);	
		}

		g_InitParamter.m_wListenPort = LOAD_INT(pszCmdLine, "LoginServer", "Port");
		g_InitParamter.m_wMaxConnect = LOAD_INT(pszCmdLine, "LoginServer", "MaxConnect");

		g_InitParamter.m_wUserDataBasePort = LOAD_INT(pszCmdLine, "LoginServer", "DBPort");

		CString szRetTmp;
		LOAD_STRING(pszCmdLine, "LoginServer", "DBIP", szRetTmp);
		lstrcpyn(g_InitParamter.m_szUserDataBaseAddr, szRetTmp, CountArray(g_InitParamter.m_szUserDataBaseAddr));

		szRetTmp = "";
		LOAD_STRING(pszCmdLine, "LoginServer", "MainPage", szRetTmp);
		lstrcpyn(g_InitParamter.m_szMainPage, szRetTmp, CountArray(g_InitParamter.m_szMainPage));
			
		szRetTmp = "";
		LOAD_STRING(pszCmdLine, "LoginServer", "DBName", szRetTmp);
		lstrcpyn(g_InitParamter.m_szUserDataBaseName, szRetTmp, CountArray(g_InitParamter.m_szUserDataBaseName));

		szRetTmp = "";
		LOAD_STRING(pszCmdLine, "LoginServer", "DBUser", szRetTmp);
		lstrcpyn(g_InitParamter.m_szUserDataBaseUser, szRetTmp, CountArray(g_InitParamter.m_szUserDataBaseUser));

		szRetTmp = "";
		LOAD_STRING(pszCmdLine, "LoginServer", "DBPass", szRetTmp);
		CXOREncrypt::CrevasseData(szRetTmp, g_InitParamter.m_szUserDataBasePass,sizeof(g_InitParamter.m_szUserDataBasePass));	
		
		szRetTmp = "";
		LOAD_STRING(pszCmdLine, "CenterServer", "IP", szRetTmp);
		lstrcpyn(g_InitParamter.m_szCenterServerAddr, szRetTmp, CountArray(g_InitParamter.m_szCenterServerAddr));	

		g_InitParamter.m_wCenterServerPort = LOAD_INT(pszCmdLine, "CenterServer", "Port");

		//soap相关
		g_InitParamter.m_wSoapPort = LOAD_INT(pszCmdLine, "SoapRecharge", "SoapPort");
		szRetTmp = "";
		LOAD_STRING(pszCmdLine, "SoapRecharge", "RechargeKey", szRetTmp);
		lstrcpyn(g_InitParamter.m_szRechargeKey, szRetTmp, CountArray(g_InitParamter.m_szRechargeKey));
		//soap线程
		if (g_InitParamter.m_wSoapPort != 0)
		{
			m_pSoapThread = AfxBeginThread(SoapThread, (LPVOID)g_InitParamter.m_wSoapPort);
			SuspendThread(m_pSoapThread->m_hThread);
		}
		
		int iAutoStart = LOAD_INT(pszCmdLine, "Default", "AutoStart");
		if (iAutoStart > 0)
		{
			//自动启动服务
			this->OnBnClickedStartService();
		}
	}

	return TRUE;
}

//确定消息
void CLogonServerDlg::OnOK()
{
	return;
}

//取消函数
void CLogonServerDlg::OnCancel()
{
	//获取状态
	bool bRuning=m_LogonService.IsService();

	//询问退出
	if (bRuning==true)
	{
		CString strMessage=TEXT("登录服务器正在运行中，确实要退出吗？");
		int iRetCode=AfxMessageBox(strMessage,MB_YESNO|MB_ICONQUESTION|MB_DEFBUTTON2);
		if (iRetCode!=IDYES) return;
	}

	//停止服务
	if (bRuning==true)
	{
		m_LogonService.StopService();
	}

	__super::OnCancel();
}

//消息解释
BOOL CLogonServerDlg::PreTranslateMessage(MSG * pMsg)
{
	if ((pMsg->message==WM_KEYDOWN)&&(pMsg->wParam==VK_ESCAPE)) return TRUE;
	return __super::PreTranslateMessage(pMsg);
}

//启动服务
void CLogonServerDlg::OnBnClickedStartService()
{
	//启动服务
	if (m_LogonService.StartService()==true)
	{
		//设置界面
		GetDlgItem(IDC_STOP_SERVICE)->EnableWindow(TRUE);
		GetDlgItem(IDC_START_SERVICE)->EnableWindow(FALSE);	

		//设置标题
		TCHAR szTitle[128]=TEXT("");
		_snprintf(szTitle,sizeof(szTitle),TEXT("登录服务 [运行] - %s"), g_InitParamter.m_szMainPage);
		SetWindowText(szTitle);

		//输出事件
		CTraceService::TraceString(TEXT("登录服务启动成功"),TraceLevel_Normal);
	}
	else
	{
		//输出事件
		CTraceService::TraceString(TEXT("登录服务启动失败"),TraceLevel_Exception);
	}

	//唤醒soap
 	if (g_InitParamter.m_wSoapPort != 0 && m_pSoapThread)
 		ResumeThread(m_pSoapThread->m_hThread);

	return;
}

//停止服务
void CLogonServerDlg::OnBnClickedStopService()
{
	//停止服务
	if (m_LogonService.StopService()==true)
	{
		//设置界面
		GetDlgItem(IDC_STOP_SERVICE)->EnableWindow(FALSE);
		GetDlgItem(IDC_START_SERVICE)->EnableWindow(TRUE);

		//输出事件
		CTraceService::TraceString(TEXT("登录服务停止成功"),TraceLevel_Normal);
	}
	else
	{
		//输出事件
		CTraceService::TraceString(TEXT("登录服务停止失败"),TraceLevel_Exception);
	}

	//挂起soap
	if (g_InitParamter.m_wSoapPort != 0 &&m_pSoapThread)
		m_pSoapThread->SuspendThread();

	return;
}

//系统配置
void CLogonServerDlg::OnBnClickedInitService()
{
	CSystemOptionDlg SystemOptionDlg;
	SystemOptionDlg.DoModal();

	return;
}

LRESULT CLogonServerDlg::OnSoapMessage(WPARAM wParam, LPARAM lParam) 
{
	return m_LogonService.OnSoapMessage(wParam, lParam);
}

//////////////////////////////////////////////////////////////////////////

