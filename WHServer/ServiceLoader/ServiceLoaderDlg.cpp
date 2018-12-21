#include "Stdafx.h"
#include "ServiceLoader.h"
#include "ServiceLoaderDlg.h"

//////////////////////////////////////////////////////////////////////////

//全局系统参数
CInitParamter g_InitParamer;

BEGIN_MESSAGE_MAP(CSystemOptionDlg, CDialog)
END_MESSAGE_MAP()

BEGIN_MESSAGE_MAP(CServiceLoaderDlg, CDialog)
	ON_BN_CLICKED(IDC_START, OnBnClickedStart)
	ON_BN_CLICKED(IDC_STOP, OnBnClickedStop)
	ON_BN_CLICKED(IDC_CREATE, OnBnClickedCreate)
	ON_BN_CLICKED(IDC_OPEN, OnBnClickedOpen)
	ON_BN_CLICKED(IDC_OPTION, OnBnClickedOption)
	ON_BN_CLICKED(IDC_SYSTEM, OnBnClickedSystem)
//	ON_BN_CLICKED(IDC_SAVE_OPTION, OnBnClickedSaveOption)
	ON_WM_QUERYENDSESSION()
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
	((CEdit *)GetDlgItem(IDC_USER_DATABASE_PORT))->LimitText(5);
	((CEdit *)GetDlgItem(IDC_USER_DATABASE_USER))->LimitText(31);
	((CEdit *)GetDlgItem(IDC_USER_DATABASE_PASS))->LimitText(31);
	((CEdit *)GetDlgItem(IDC_USER_DATABASE_NAME))->LimitText(31);
	((CEdit *)GetDlgItem(IDC_SERVER_DATABASE_PORT))->LimitText(5);
	((CEdit *)GetDlgItem(IDC_SERVER_DATABASE_USER))->LimitText(31);
	((CEdit *)GetDlgItem(IDC_SERVER_DATABASE_PASS))->LimitText(31);
	((CEdit *)GetDlgItem(IDC_SERVER_DATABASE_NAME))->LimitText(31);

	//加载参数
//	CInitParamter InitParamter;
//	InitParamter.LoadInitParamter();

	//登录数据库
	SetDlgItemInt(IDC_USER_DATABASE_PORT,g_InitParamer.m_wGameUserDBPort,FALSE);
	SetDlgItemText(IDC_USER_DATABASE_USER,g_InitParamer.m_szGameUserDBUser);
	SetDlgItemText(IDC_USER_DATABASE_PASS,g_InitParamer.m_szGameUserDBPass);
	SetDlgItemText(IDC_USER_DATABASE_NAME,g_InitParamer.m_szGameUserDBName);

	//信息数据库
	SetDlgItemInt(IDC_SERVER_DATABASE_PORT,g_InitParamer.m_wServerInfoDBPort,FALSE);
	SetDlgItemText(IDC_SERVER_DATABASE_USER,g_InitParamer.m_szServerInfoDBUser);
	SetDlgItemText(IDC_SERVER_DATABASE_PASS,g_InitParamer.m_szServerInfoDBPass);
	SetDlgItemText(IDC_SERVER_DATABASE_NAME,g_InitParamer.m_szServerInfoDBName);

	//登录数据库地址
	DWORD dwDataBaseIP=inet_addr(g_InitParamer.m_szGameUserDBAddr);
	if (dwDataBaseIP==INADDR_NONE)
	{
		LPHOSTENT lpHost=gethostbyname(g_InitParamer.m_szGameUserDBAddr);
		if (lpHost!=NULL) dwDataBaseIP=((LPIN_ADDR)lpHost->h_addr)->s_addr;
	}
	CIPAddressCtrl * pDataBaseIP=(CIPAddressCtrl *)GetDlgItem(IDC_USER_DATABASE_IP);
	pDataBaseIP->SetAddress(ntohl(dwDataBaseIP));

	//信息数据库地址
	dwDataBaseIP=inet_addr(g_InitParamer.m_szServerInfoDBAddr);
	if (dwDataBaseIP==INADDR_NONE)
	{
		LPHOSTENT lpHost=gethostbyname(g_InitParamer.m_szServerInfoDBAddr);
		if (lpHost!=NULL) dwDataBaseIP=((LPIN_ADDR)lpHost->h_addr)->s_addr;
	}
	pDataBaseIP=(CIPAddressCtrl *)GetDlgItem(IDC_SERVER_DATABASE_IP);
	pDataBaseIP->SetAddress(ntohl(dwDataBaseIP));

	//中心服务器
	dwDataBaseIP=inet_addr(g_InitParamer.m_szCenterServerAddr[0]);
	if (dwDataBaseIP==INADDR_NONE)
	{
		LPHOSTENT lpHost=gethostbyname(g_InitParamer.m_szCenterServerAddr[0]);
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
//	CInitParamter InitParamter;

	//登录数据库 
	g_InitParamer.m_wGameUserDBPort=GetDlgItemInt(IDC_USER_DATABASE_PORT);
	GetDlgItemText(IDC_USER_DATABASE_USER,g_InitParamer.m_szGameUserDBUser,sizeof(g_InitParamer.m_szGameUserDBUser));
	GetDlgItemText(IDC_USER_DATABASE_PASS,g_InitParamer.m_szGameUserDBPass,sizeof(g_InitParamer.m_szGameUserDBPass));
	GetDlgItemText(IDC_USER_DATABASE_NAME,g_InitParamer.m_szGameUserDBName,sizeof(g_InitParamer.m_szGameUserDBName));

	//登录数据库地址
	DWORD dwDataBaseIP=INADDR_NONE;
	BYTE * pAddrByte=(BYTE *)&dwDataBaseIP;
	((CIPAddressCtrl *)GetDlgItem(IDC_USER_DATABASE_IP))->GetAddress(dwDataBaseIP);
	_snprintf(g_InitParamer.m_szGameUserDBAddr,sizeof(g_InitParamer.m_szGameUserDBAddr),TEXT("%d.%d.%d.%d"),
		pAddrByte[3],pAddrByte[2],pAddrByte[1],pAddrByte[0]);

	//信息数据库 
	g_InitParamer.m_wServerInfoDBPort=GetDlgItemInt(IDC_SERVER_DATABASE_PORT);
	GetDlgItemText(IDC_SERVER_DATABASE_USER,g_InitParamer.m_szServerInfoDBUser,sizeof(g_InitParamer.m_szServerInfoDBUser));
	GetDlgItemText(IDC_SERVER_DATABASE_PASS,g_InitParamer.m_szServerInfoDBPass,sizeof(g_InitParamer.m_szServerInfoDBPass));
	GetDlgItemText(IDC_SERVER_DATABASE_NAME,g_InitParamer.m_szServerInfoDBName,sizeof(g_InitParamer.m_szServerInfoDBName));

	//信息数据库地址
	dwDataBaseIP=INADDR_NONE;
	pAddrByte=(BYTE *)&dwDataBaseIP;
	((CIPAddressCtrl *)GetDlgItem(IDC_SERVER_DATABASE_IP))->GetAddress(dwDataBaseIP);
	_snprintf(g_InitParamer.m_szServerInfoDBAddr,sizeof(g_InitParamer.m_szServerInfoDBAddr),TEXT("%d.%d.%d.%d"),
		pAddrByte[3],pAddrByte[2],pAddrByte[1],pAddrByte[0]);

	//中心服务器
	dwDataBaseIP=INADDR_NONE;
	pAddrByte=(BYTE *)&dwDataBaseIP;
	((CIPAddressCtrl *)GetDlgItem(IDC_CENTER_SERVER_IP))->GetAddress(dwDataBaseIP);
	_snprintf(g_InitParamer.m_szCenterServerAddr[0],sizeof(g_InitParamer.m_szCenterServerAddr[0]),TEXT("%d.%d.%d.%d"),
		pAddrByte[3],pAddrByte[2],pAddrByte[1],pAddrByte[0]);

	//保存设置
	g_InitParamer.SaveInitParamter(false);

	__super::OnOK();
}

//////////////////////////////////////////////////////////////////////////

//构造函数
CServiceLoaderDlg::CServiceLoaderDlg() : CDialog(IDD_SERVICELOADER_DIALOG)
{
}

//析构函数
CServiceLoaderDlg::~CServiceLoaderDlg()
{
}

//控件绑定
void CServiceLoaderDlg::DoDataExchange(CDataExchange* pDX)
{
	__super::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SERVICE_EVENT, m_RichEditCtrl);
}

//初始化函数
BOOL CServiceLoaderDlg::OnInitDialog()
{
	__super::OnInitDialog();

	//设置图标
	HICON hIcon=LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME));
	SetIcon(hIcon,TRUE);
	SetIcon(hIcon,FALSE);

	//加载参数
	g_InitParamer.LoadInitParamter();

	//加载配置
	LPCTSTR pszCmdLine=AfxGetApp()->m_lpCmdLine;
	if (pszCmdLine[0]==0)
	{
	     pszCmdLine=".\\gameserver.ini";
	}
	if (pszCmdLine[0]!=0)
	{
		TCHAR szDescribe[128]=TEXT("");
		bool bSuccess=m_ServiceParameter.LoadParameter(pszCmdLine);
		if (bSuccess==true)
		{
			CFileFind finder;   //查找是否存在ini文件，若不存在，则生成一个新的默认设置的ini文件
			BOOL ifFind = finder.FindFile(pszCmdLine);
			if( !ifFind )
			{
				//输出事件
				CTraceService::TraceString(TEXT("ini文件不存在"),TraceLevel_Normal);	
			}

			//by leon 初始化系统参数
			g_InitParamer.InitParamter();

			CString szRetTmp;
			LOAD_STRING(pszCmdLine, "CenterServer", "IP", szRetTmp);
			lstrcpyn(g_InitParamer.m_szCenterServerAddr[0], szRetTmp, CountArray(g_InitParamer.m_szCenterServerAddr[0]));		
			g_InitParamer.m_wCenterPort[0] = LOAD_INT(pszCmdLine, "CenterServer", "Port");
			g_InitParamer.m_wCenterServerCount = 1;

			int iServerCount = LOAD_INT(pszCmdLine, "CenterServer", "Count");
			for (int i=1; i<iServerCount && i<MAX_CENTER_SERVER; i++)
			{
				char key[32];
				memset(key, 0, 32);
				sprintf(key, "CenterServer%d", i);

				szRetTmp = "";
				LOAD_STRING(pszCmdLine, key, "IP", szRetTmp);
				if (szRetTmp == "")break;   //错误参数，终止

				lstrcpyn(g_InitParamer.m_szCenterServerAddr[i], szRetTmp, CountArray(g_InitParamer.m_szCenterServerAddr[i]));		
				g_InitParamer.m_wCenterPort[i] = LOAD_INT(pszCmdLine, key, "Port");

				g_InitParamer.m_wCenterServerCount = i+1;
			}
			
			g_InitParamer.m_wGameUserDBPort = LOAD_INT(pszCmdLine, "GameUserDB", "Port");
			szRetTmp = "";
			LOAD_STRING(pszCmdLine, "GameUserDB", "IP", szRetTmp);
			lstrcpyn(g_InitParamer.m_szGameUserDBAddr, szRetTmp, CountArray(g_InitParamer.m_szGameUserDBAddr));
			szRetTmp = "";
			LOAD_STRING(pszCmdLine, "GameUserDB", "DBName", szRetTmp);
			lstrcpyn(g_InitParamer.m_szGameUserDBName, szRetTmp, CountArray(g_InitParamer.m_szGameUserDBName));
			szRetTmp = "";
			LOAD_STRING(pszCmdLine, "GameUserDB", "DBUser", szRetTmp);
			lstrcpyn(g_InitParamer.m_szGameUserDBUser, szRetTmp, CountArray(g_InitParamer.m_szGameUserDBUser));
			szRetTmp = "";
			LOAD_STRING(pszCmdLine, "GameUserDB", "DBPass", szRetTmp);
			CXOREncrypt::CrevasseData(szRetTmp, g_InitParamer.m_szGameUserDBPass,sizeof(g_InitParamer.m_szGameUserDBPass));
			
			g_InitParamer.m_wServerInfoDBPort = LOAD_INT(pszCmdLine, "ServerInfoDB", "Port");
			szRetTmp = "";
			LOAD_STRING(pszCmdLine, "ServerInfoDB", "IP", szRetTmp);
			lstrcpyn(g_InitParamer.m_szServerInfoDBAddr, szRetTmp, CountArray(g_InitParamer.m_szServerInfoDBAddr));
			szRetTmp = "";
			LOAD_STRING(pszCmdLine, "ServerInfoDB", "DBName", szRetTmp);
			lstrcpyn(g_InitParamer.m_szServerInfoDBName, szRetTmp, CountArray(g_InitParamer.m_szServerInfoDBName));
			szRetTmp = "";
			LOAD_STRING(pszCmdLine, "ServerInfoDB", "DBUser", szRetTmp);
			lstrcpyn(g_InitParamer.m_szServerInfoDBUser, szRetTmp, CountArray(g_InitParamer.m_szServerInfoDBUser));
			szRetTmp = "";
			LOAD_STRING(pszCmdLine, "ServerInfoDB", "DBPass", szRetTmp);
			CXOREncrypt::CrevasseData(szRetTmp, g_InitParamer.m_szServerInfoDBPass,sizeof(g_InitParamer.m_szServerInfoDBPass));

			//控制界面
			GetDlgItem(IDC_START)->EnableWindow(TRUE);
			GetDlgItem(IDC_OPTION)->EnableWindow(TRUE);
//			GetDlgItem(IDC_SAVE_OPTION)->EnableWindow(TRUE);

			//提示信息
			_snprintf(szDescribe,sizeof(szDescribe),TEXT("“%s”游戏房间加载成功"),pszCmdLine);
			ShowErrorMessasge(szDescribe,TraceLevel_Normal);

			int iAutoStart = LOAD_INT(pszCmdLine, "Default", "AutoStart");
			if (iAutoStart > 0)
			{
				//自动启动服务
				this->OnBnClickedStart();
			}
		}
		else 
		{
			_snprintf(szDescribe,sizeof(szDescribe),TEXT("“%s”游戏房间加载失败"),pszCmdLine);
			ShowErrorMessasge(szDescribe,TraceLevel_Exception);
		}
	}

	return TRUE;
}

//消息解释
BOOL CServiceLoaderDlg::PreTranslateMessage(MSG * pMsg)
{
	if ((pMsg->message==WM_KEYDOWN)&&(pMsg->wParam==VK_ESCAPE)) return TRUE;
	return __super::PreTranslateMessage(pMsg);
}

//确定消息
void CServiceLoaderDlg::OnOK()
{
	return;
}

//取消函数
void CServiceLoaderDlg::OnCancel()
{
	//停止服务
	if (m_GameService.GetInterface()!=NULL)
	{
		//询问退出
		bool bRuning=m_GameService->IsService();
		if (bRuning==true)
		{
			CString strMessage=TEXT("游戏房间服务正在运行中，确实要退出吗？");
			int iRetCode=AfxMessageBox(strMessage,MB_YESNO|MB_ICONQUESTION|MB_DEFBUTTON2);
			if (iRetCode!=IDYES) return;
		}

		//停止服务
		if (bRuning==true) m_GameService->StopService();
	}

	__super::OnCancel();
}

//启动服务
void CServiceLoaderDlg::OnBnClickedStart()
{
	//效验状态
	ASSERT(m_ServiceParameter.IsLoaded()==true);
	if (m_ServiceParameter.IsLoaded()==false) return;

	//加载参数
	g_InitParamer.LoadInitParamter();

	//变量定义
	tagGameServiceParameter GameServiceParameter;
	memset(&GameServiceParameter,0,sizeof(GameServiceParameter));
	const tagOptionParameter & OptionParameter=m_ServiceParameter.GetOptionParameter();

	//获取密码
/*	const DWORD dwDataBaseAddr=inet_addr(OptionParameter.szDataBaseAddr);
	const tagDBConnectInfo * pDBConnectInfo=GetDBConnectInfo(dwDataBaseAddr);
	if (pDBConnectInfo==NULL)
	{
		ShowErrorMessasge(TEXT("游戏数据库信息不存在，游戏房间服务启动失败"),TraceLevel_Exception);
		return;
	}
*/
	//构造参数
	GameServiceParameter.GameServiceOption=OptionParameter.GameServiceOption;
	
	GameServiceParameter.wCenterCount = g_InitParamer.m_wCenterServerCount;
	for (int i=0; i<MAX_CENTER_SERVER; i++)
	{
		GameServiceParameter.dwCenterAddr[i]=inet_addr(g_InitParamer.m_szCenterServerAddr[i]);
		GameServiceParameter.wCenterPort[i]=g_InitParamer.m_wCenterPort[i];
	}
	lstrcpyn(GameServiceParameter.szModule,OptionParameter.szModuleName,CountArray(GameServiceParameter.szModule));

	//用户数据库
	GameServiceParameter.GameUserDBInfo.wDataBasePort=g_InitParamer.m_wGameUserDBPort;
	GameServiceParameter.GameUserDBInfo.dwDataBaseAddr=inet_addr(g_InitParamer.m_szGameUserDBAddr);
	lstrcpyn(GameServiceParameter.GameUserDBInfo.szDataBaseUser,g_InitParamer.m_szGameUserDBUser,CountArray(GameServiceParameter.GameUserDBInfo.szDataBaseUser));
	lstrcpyn(GameServiceParameter.GameUserDBInfo.szDataBasePass,g_InitParamer.m_szGameUserDBPass,CountArray(GameServiceParameter.GameUserDBInfo.szDataBasePass));
	lstrcpyn(GameServiceParameter.GameUserDBInfo.szDataBaseName,g_InitParamer.m_szGameUserDBName,CountArray(GameServiceParameter.GameUserDBInfo.szDataBaseName));

	//获取密码
	GameServiceParameter.wTreasureDBCount = OptionParameter.m_wTreasureDBCount;
	for (int i=0; i<OptionParameter.m_wTreasureDBCount; i++)
	{
		//const DWORD dwDataBaseAddr=inet_addr(OptionParameter.szDataBaseAddr[i]);
		const tagDBConnectInfo * pDBConnectInfo=GetDBConnectInfo(OptionParameter.szDataBaseAddr[i]);
		if (pDBConnectInfo==NULL)
		{
			ShowErrorMessasge(TEXT("游戏数据库信息不存在，游戏房间服务启动失败"),TraceLevel_Exception);
			return;
		}

		//游戏数据库
		GameServiceParameter.GameScoreDBInfo[i].dwDataBaseAddr=pDBConnectInfo->dwDataBaseAddr;
		GameServiceParameter.GameScoreDBInfo[i].wDataBasePort=pDBConnectInfo->wDataBasePort;
		lstrcpyn(GameServiceParameter.GameScoreDBInfo[i].szDataBaseUser,pDBConnectInfo->szDataBaseUser,CountArray(GameServiceParameter.GameScoreDBInfo[i].szDataBaseUser));
		lstrcpyn(GameServiceParameter.GameScoreDBInfo[i].szDataBasePass,pDBConnectInfo->szDataBasePass,CountArray(GameServiceParameter.GameScoreDBInfo[i].szDataBasePass));
		lstrcpyn(GameServiceParameter.GameScoreDBInfo[i].szDataBaseName,OptionParameter.szDataBaseName,CountArray(GameServiceParameter.GameScoreDBInfo[i].szDataBaseName));
	}
	//创建服务
	if ((m_GameService.GetInterface()==NULL)&&(m_GameService.CreateInstance()==false))
	{
		ShowErrorMessasge(TEXT("游戏服务组件创建失败"),TraceLevel_Exception);
		return;
	}

	//启动服务
	TCHAR szDescribe[128]=TEXT("");
	if (m_GameService->StartService(&GameServiceParameter)==true)
	{
		//界面控制
		GetDlgItem(IDC_STOP)->EnableWindow(TRUE);
		GetDlgItem(IDC_START)->EnableWindow(FALSE);
//		GetDlgItem(IDC_CREATE)->EnableWindow(FALSE);
//		GetDlgItem(IDC_OPEN)->EnableWindow(FALSE);
		GetDlgItem(IDC_OPTION)->EnableWindow(FALSE);

		//提示信息
		_snprintf(szDescribe,sizeof(szDescribe),TEXT("【%s - %ld】服务启动成功"),GameServiceParameter.GameServiceOption.szGameRoomName,GameServiceParameter.GameServiceOption.wServerID);
		ShowErrorMessasge(szDescribe,TraceLevel_Normal);

		//设置标题
		TCHAR szTitle[128]=TEXT("");
		_snprintf(szTitle,sizeof(szTitle),TEXT("游戏服务 [运行] - %s - %s"),GameServiceParameter.GameServiceOption.szGameRoomName,GameServiceParameter.szModule);
		SetWindowText(szTitle);
	}
	else 
	{
		_snprintf(szDescribe,sizeof(szDescribe),TEXT("【%s】服务启动失败"),GameServiceParameter.GameServiceOption.szGameRoomName);
		ShowErrorMessasge(szDescribe,TraceLevel_Exception);
	}

	return;
}

//停止服务
void CServiceLoaderDlg::OnBnClickedStop()
{
	if ((m_GameService.GetInterface()!=NULL)&&(m_GameService->StopService()==true))
	{
		SetWindowText(TEXT("游戏服务 - [停止]"));
		GetDlgItem(IDC_START)->EnableWindow(TRUE);
		GetDlgItem(IDC_STOP)->EnableWindow(FALSE);
//		GetDlgItem(IDC_CREATE)->EnableWindow(TRUE);
//		GetDlgItem(IDC_OPEN)->EnableWindow(TRUE);
		GetDlgItem(IDC_OPTION)->EnableWindow(TRUE);
		ShowErrorMessasge(TEXT("游戏房间服务停止成功"),TraceLevel_Normal);
	}

	return;
}

//创建房间
void CServiceLoaderDlg::OnBnClickedCreate()
{
	//获取目录
	TCHAR szPath[MAX_PATH]=TEXT("");
	GetCurrentDirectory(sizeof(szPath),szPath);

	//选择模块
	LPCTSTR pszFilter=TEXT("游戏服务组件 （*.DLL）|*.DLL||");
	CFileDialog DlgOpenParameter(TRUE,NULL,NULL,OFN_NOCHANGEDIR|OFN_HIDEREADONLY,pszFilter);
	DlgOpenParameter.m_pOFN->lpstrInitialDir=szPath;
	if (DlgOpenParameter.DoModal()!=IDOK) return;

	//创建房间
	bool bSuccess=m_ServiceParameter.CreateParameter(DlgOpenParameter.GetPathName());
	if (bSuccess==true)	ShowErrorMessasge(TEXT("游戏房间创建成功"),TraceLevel_Normal);

	//控制界面
	GetDlgItem(IDC_START)->EnableWindow(bSuccess?TRUE:FALSE);
	GetDlgItem(IDC_OPTION)->EnableWindow(bSuccess?TRUE:FALSE);
//	GetDlgItem(IDC_SAVE_OPTION)->EnableWindow(bSuccess?TRUE:FALSE);

	return;
}

//打开房间
void CServiceLoaderDlg::OnBnClickedOpen()
{
	return;
}

//配置房间
void CServiceLoaderDlg::OnBnClickedOption()
{
	m_ServiceParameter.ConfigParameter();
	return;
}

//系统配置
void CServiceLoaderDlg::OnBnClickedSystem()
{
	CSystemOptionDlg SystemOptionDlg;
	SystemOptionDlg.DoModal();

	return;
}

//保存配置
void CServiceLoaderDlg::OnBnClickedSaveOption()
{
	//获取目录
	TCHAR szPath[MAX_PATH]=TEXT("");
	GetCurrentDirectory(sizeof(szPath),szPath);

	//选择文件
	LPCTSTR pszFilter=TEXT("房间配置文件 （*.ROP）|*.ROP||");
	CFileDialog DlgSaveParameter(FALSE,TEXT("ROP"),NULL,OFN_NOCHANGEDIR|OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,pszFilter);
	DlgSaveParameter.m_pOFN->lpstrInitialDir=szPath;
	if (DlgSaveParameter.DoModal()!=IDOK) return;

	//保存文件
	if (m_ServiceParameter.SaveParameter(DlgSaveParameter.GetPathName())==false)
	{
		ShowErrorMessasge(TEXT("房间配置文件保存失败"),TraceLevel_Exception);
		return;
	}

	return;
}

//获取密码
const tagDBConnectInfo * CServiceLoaderDlg::GetDBConnectInfo(const char* szDataBaseAddr)
{
	//寻找现存
	tagDBConnectInfo * pDBConnectInfo=NULL;
	for (INT_PTR i=0;i<m_DBConnectInfoArray.GetCount();i++)
	{
		pDBConnectInfo=&m_DBConnectInfoArray[i];
		if (std::string(pDBConnectInfo->szDataBaseAddr)==szDataBaseAddr) return pDBConnectInfo;
	}

	//创建数据库
	CDataBaseAide ServerInfoAide;
	CDataBaseHelper ServerInfoDB;
	if (ServerInfoDB.CreateInstance()==false)
	{
		ShowErrorMessasge(TEXT("数据库组件创建失败，无法获取数据库信息"),TraceLevel_Exception);
		return NULL;
	}

	try
	{
		//连接数据库
		ServerInfoDB->SetConnectionInfo(g_InitParamer.m_szServerInfoDBAddr,g_InitParamer.m_wServerInfoDBPort,
			g_InitParamer.m_szServerInfoDBName,g_InitParamer.m_szServerInfoDBUser,g_InitParamer.m_szServerInfoDBPass);

		//打开连接
		ServerInfoDB->OpenConnection();
		ServerInfoAide.SetDataBase(ServerInfoDB.GetInterface());

		//转化地址
//		TCHAR szDataBaseAdr[16]=TEXT("");
//		BYTE * pcbDataBaseAdr=(BYTE *)&dwDataBaseAddr;
//		_snprintf(szDataBaseAdr,sizeof(szDataBaseAdr),TEXT("%d.%d.%d.%d"),pcbDataBaseAdr[0],pcbDataBaseAdr[1],pcbDataBaseAdr[2],pcbDataBaseAdr[3]);

		//执行查询
		ServerInfoAide.ResetParameter();
		ServerInfoAide.AddParameter(TEXT("@strDataBaseAddr"),szDataBaseAddr);
		LONG lReturnCode=ServerInfoAide.ExecuteProcess(TEXT("GSP_LoadDataBaseInfo_V2"),true);

		//结果判断
		if (lReturnCode!=0L)
		{
			TCHAR szErrorDescribe[256]=TEXT("");
			ServerInfoAide.GetValue_String(TEXT("ErrorDescribe"),szErrorDescribe,CountArray(szErrorDescribe));
			ShowErrorMessasge(szErrorDescribe,TraceLevel_Exception);
			return NULL;
		}

		//获取信息
		tagDBConnectInfo DBConnectInfo;
		memset(&DBConnectInfo,0,sizeof(DBConnectInfo));
		TCHAR szDBUser[512]=TEXT(""),szDBPass[512]=TEXT(""),szDBAddr[16]=TEXT("");
		ServerInfoAide.GetValue_String(TEXT("DBAddr"),szDBAddr,sizeof(szDBAddr));
		ServerInfoAide.GetValue_String(TEXT("DBUser"),szDBUser,sizeof(szDBUser));
		DBConnectInfo.wDataBasePort=ServerInfoAide.GetValue_WORD(TEXT("DBPort"));
		ServerInfoAide.GetValue_String(TEXT("DBPassword"),szDBPass,sizeof(szDBPass));
		CXOREncrypt::CrevasseData(szDBUser,DBConnectInfo.szDataBaseUser,sizeof(DBConnectInfo.szDataBaseUser));
		CXOREncrypt::CrevasseData(szDBPass,DBConnectInfo.szDataBasePass,sizeof(DBConnectInfo.szDataBasePass));

		//保存信息
		DBConnectInfo.dwDataBaseAddr=inet_addr(szDBAddr);
		lstrcpyn(DBConnectInfo.szDataBaseAddr,szDataBaseAddr,CountArray(DBConnectInfo.szDataBaseAddr));
		INT_PTR nIndex=m_DBConnectInfoArray.Add(DBConnectInfo);
		return &m_DBConnectInfoArray[nIndex];
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);
	}

	return NULL;
}

//显示错误
void CServiceLoaderDlg::ShowErrorMessasge(LPCTSTR pszString, enTraceLevel TraceLevel)
{
	CTraceService::TraceString(pszString,TraceLevel);
}

//////////////////////////////////////////////////////////////////////////

BOOL CServiceLoaderDlg::OnQueryEndSession()
{
	//停止服务
	if ((m_GameService.GetInterface()!=NULL)&&(m_GameService->IsService()==true))
	{
		CString strMessage=TEXT("游戏房间服务正在运行中，系统要求注销回话请求失败");
		ShowErrorMessasge(strMessage,TraceLevel_Warning);

		return FALSE;
	}


	return TRUE;
}
