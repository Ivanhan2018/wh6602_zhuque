#include "Stdafx.h"
#include "AtlBase.h"
#include "ControlServerDlg.h"

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CSystemOptionDlg, CDialog)
END_MESSAGE_MAP()

BEGIN_MESSAGE_MAP(CControlServerDlg, CDialog)
	ON_BN_CLICKED(IDC_START_SERVICE, OnBnClickedStartService)
	ON_BN_CLICKED(IDC_STOP_SERVICE, OnBnClickedStopService)
	ON_BN_CLICKED(IDC_INIT_SERVICE, OnBnClickedInitService)
	ON_BN_CLICKED(IDC_SEND_COMMAND, OnBnClickedSendCommand)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CSystemOptionDlg::CSystemOptionDlg() : CDialog(IDD_SYSTEM_OPTION)
{
}

//析构函数
CSystemOptionDlg::~CSystemOptionDlg()
{
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
//	((CEdit *)GetDlgItem(IDC_LISTEN_PORT))->LimitText(5);
//	((CEdit *)GetDlgItem(IDC_MAX_CONNECT))->LimitText(3);
//	((CEdit *)GetDlgItem(IDC_MAIN_PAGE))->LimitText(31);
	((CEdit *)GetDlgItem(IDC_USER_DATABASE_PORT))->LimitText(5);
	((CEdit *)GetDlgItem(IDC_USER_DATABASE_USER))->LimitText(31);
	((CEdit *)GetDlgItem(IDC_USER_DATABASE_PASS))->LimitText(31);
	((CEdit *)GetDlgItem(IDC_USER_DATABASE_NAME))->LimitText(31);

	//加载参数
//	CInitParamter InitParamter;
//	InitParamter.LoadInitParamter();

	//设置控件
//	SetDlgItemInt(IDC_LISTEN_PORT,InitParamter.m_wListenPort,FALSE);
//	SetDlgItemInt(IDC_MAX_CONNECT,InitParamter.m_wMaxConnect,FALSE);
	
	SetDlgItemInt(IDC_CENTER_PORT,g_InitParamter.m_wCenterServerPort,FALSE);

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
//	if (InitParamter.m_szMainPage[0]==0) SetDlgItemText(IDC_MAIN_PAGE,szStationPage);
//	else SetDlgItemText(IDC_MAIN_PAGE,InitParamter.m_szMainPage);

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
//	InitParamter.m_wListenPort=GetDlgItemInt(IDC_LISTEN_PORT);
//	InitParamter.m_wMaxConnect=GetDlgItemInt(IDC_MAX_CONNECT);

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
//	GetDlgItemText(IDC_MAIN_PAGE,InitParamter.m_szMainPage,sizeof(InitParamter.m_szMainPage));
//	if (lstrcmp(InitParamter.m_szMainPage,szStationPage)==0) InitParamter.m_szMainPage[0]=0;

	//中心服务器
	dwDataBaseIP=INADDR_NONE;
	pAddrByte=(BYTE *)&dwDataBaseIP;
	((CIPAddressCtrl *)GetDlgItem(IDC_CENTER_SERVER_IP))->GetAddress(dwDataBaseIP);
	_snprintf(InitParamter.m_szCenterServerAddr,sizeof(InitParamter.m_szCenterServerAddr),TEXT("%d.%d.%d.%d"),
		pAddrByte[0],pAddrByte[1],pAddrByte[2],pAddrByte[3]);

	//保存设置
	InitParamter.SaveInitParamter(false);

	__super::OnOK();
}

//////////////////////////////////////////////////////////////////////////

//构造函数
CControlServerDlg::CControlServerDlg() : CDialog(IDD_CONTROLSERVER_DIALOG)
{
}

//析构函数
CControlServerDlg::~CControlServerDlg()
{
}

//控件子类化
void CControlServerDlg::DoDataExchange(CDataExchange * pDX)
{
	__super::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SERVICE_EVENT, m_RichEditTrace);
}

//初始化函数
BOOL CControlServerDlg::OnInitDialog()
{
	__super::OnInitDialog();

	//设置图标
	HICON hIcon=LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME));
	SetIcon(hIcon,TRUE);
	SetIcon(hIcon,FALSE);

	//加载配置
	LPCTSTR pszCmdLine=AfxGetApp()->m_lpCmdLine;
	if (pszCmdLine[0]==0)
	{
	     pszCmdLine=".\\controlserver.ini";
	}
	if (pszCmdLine[0]!=0)
	{
		CFileFind finder;   //查找是否存在ini文件，若不存在，则生成一个新的默认设置的ini文件
		BOOL ifFind = finder.FindFile(pszCmdLine);
		if( !ifFind )
		{
			//输出事件
			CTraceService::TraceString(TEXT("ini文件不存在"),TraceLevel_Normal);	
		}

		g_InitParamter.m_wUserDataBasePort = LOAD_INT(pszCmdLine, "ControlServer", "DBPort");

		CString szRetTmp;
		LOAD_STRING(pszCmdLine, "ControlServer", "DBIP", szRetTmp);
		lstrcpyn(g_InitParamter.m_szUserDataBaseAddr, szRetTmp, CountArray(g_InitParamter.m_szUserDataBaseAddr));

		szRetTmp = "";
		LOAD_STRING(pszCmdLine, "ControlServer", "DBName", szRetTmp);
		lstrcpyn(g_InitParamter.m_szUserDataBaseName, szRetTmp, CountArray(g_InitParamter.m_szUserDataBaseName));

		szRetTmp = "";
		LOAD_STRING(pszCmdLine, "ControlServer", "DBUser", szRetTmp);
		lstrcpyn(g_InitParamter.m_szUserDataBaseUser, szRetTmp, CountArray(g_InitParamter.m_szUserDataBaseUser));

		szRetTmp = "";
		LOAD_STRING(pszCmdLine, "ControlServer", "DBPass", szRetTmp);
		CXOREncrypt::CrevasseData(szRetTmp, g_InitParamter.m_szUserDataBasePass,sizeof(g_InitParamter.m_szUserDataBasePass));	
		
		szRetTmp = "";
		LOAD_STRING(pszCmdLine, "CenterServer", "IP", szRetTmp);
		lstrcpyn(g_InitParamter.m_szCenterServerAddr, szRetTmp, CountArray(g_InitParamter.m_szCenterServerAddr));	

		g_InitParamter.m_wCenterServerPort = LOAD_INT(pszCmdLine, "CenterServer", "Port");
	
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
void CControlServerDlg::OnOK()
{
	return;
}

//取消函数
void CControlServerDlg::OnCancel()
{
	//获取状态
	bool bRuning=m_ControlService.IsService();

	//询问退出
	if (bRuning==true)
	{
		CString strMessage=TEXT("控制服务器正在运行中，确实要退出吗？");
		int iRetCode=AfxMessageBox(strMessage,MB_YESNO|MB_ICONQUESTION|MB_DEFBUTTON2);
		if (iRetCode!=IDYES) return;
	}

	//停止服务
	if (bRuning==true)
	{
		m_ControlService.StopService();
	}

	__super::OnCancel();
}

//消息解释
BOOL CControlServerDlg::PreTranslateMessage(MSG * pMsg)
{
	if ((pMsg->message==WM_KEYDOWN)&&(pMsg->wParam==VK_ESCAPE)) return TRUE;
	return __super::PreTranslateMessage(pMsg);
}

//启动服务
void CControlServerDlg::OnBnClickedStartService()
{
	//启动服务
	if (m_ControlService.StartService()==true)
	{
		//设置标题
		TCHAR szTitle[128]=TEXT("");
		_snprintf(szTitle,sizeof(szTitle),TEXT("控制服务 - %d"), g_InitParamter.m_wUserDataBasePort);
		SetWindowText(szTitle);

		//设置界面
		GetDlgItem(IDC_STOP_SERVICE)->EnableWindow(TRUE);
		GetDlgItem(IDC_START_SERVICE)->EnableWindow(FALSE);

		//输出事件
		CTraceService::TraceString(TEXT("控制服务启动成功"),TraceLevel_Normal);
	}
	else
	{
		//输出事件
		CTraceService::TraceString(TEXT("控制服务启动失败"),TraceLevel_Exception);
	}

	return;
}

//停止服务
void CControlServerDlg::OnBnClickedStopService()
{
	//停止服务
	if (m_ControlService.StopService()==true)
	{
		//设置界面
		GetDlgItem(IDC_STOP_SERVICE)->EnableWindow(FALSE);
		GetDlgItem(IDC_START_SERVICE)->EnableWindow(TRUE);

		//输出事件
		CTraceService::TraceString(TEXT("控制服务停止成功"),TraceLevel_Normal);
	}
	else
	{
		//输出事件
		CTraceService::TraceString(TEXT("控制服务停止失败"),TraceLevel_Exception);
	}

	return;
}

//系统配置
void CControlServerDlg::OnBnClickedInitService()
{
	CSystemOptionDlg SystemOptionDlg;
	SystemOptionDlg.DoModal();

	return;
}

//发送命令字
void CControlServerDlg::OnBnClickedSendCommand()
{
	char command[32];
	memset(command, 0, 32);
	GetDlgItemText(IDC_CONTROL_COMMAND_EDIT2,command, 32);

	CMD_CS_ControlCommandReq req;
	memset(&req, 0, sizeof(req));
	req.wCommandID = atoi(command);
	GetDlgItemText(IDC_CONTROL_COMMAND_PARAM_EDIT, req.szParam, 128);

	//目前只支持2个命令字
	if (req.wCommandID <= 0 || req.wCommandID > 2)
	{
		CTraceService::TraceString(TEXT("无效的命令字"),TraceLevel_Warning);
		return;
	}

	//下发命令
	if (req.wCommandID == 1)  //踢人
	{
		req.dwUserID = atol(req.szParam);
		if (req.dwUserID <= 0)
		{
			CTraceService::TraceString(TEXT("命令参数错误"),TraceLevel_Warning);
			return;
		}
	}
	else if (req.wCommandID == 2)  //房间维护
	{
		req.dwServerID = atol(req.szParam);
		if (req.dwServerID <= 0)
		{
			CTraceService::TraceString(TEXT("命令参数错误"),TraceLevel_Warning);
			return;
		}
	}

	if (m_ControlService.SendCommand(&req, sizeof(req)))
	{
		char buf[256];
		memset(buf, 0, 256);
		sprintf(buf, "下发成功.命令%d 参数%s", req.wCommandID, req.szParam); 
		CTraceService::TraceString(buf,TraceLevel_Normal);
	}
	else
	{
		CTraceService::TraceString(TEXT("命令下发失败"),TraceLevel_Warning);
	}
	
	return;
}

//////////////////////////////////////////////////////////////////////////

