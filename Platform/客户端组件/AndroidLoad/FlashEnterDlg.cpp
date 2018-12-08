// FlashEnterDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "FlashEnter.h"
#include "FlashEnterDlg.h"
#include ".\flashenterdlg.h"
#include "GlobalUnits.h"
#include "GameUserInfo.h"
//#include "XMLManager.h"
#include <string>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CFlashEnterDlg 对话框

using namespace std;


CFlashEnterDlg::CFlashEnterDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFlashEnterDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_pGameLogon = NULL;
	m_dwListCountItem = 0;
	//m_XmlFile = NULL;
}

void CFlashEnterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_ACCOUNT, m_UserID);
	DDX_Control(pDX, IDC_EDIT_PASSWORD, m_UserPassWord);
	DDX_Control(pDX, IDC_IPADDRESS, m_ServerIP);
	DDX_Control(pDX, IDC_EDIT_GAMEID, m_GameID);
	DDX_Control(pDX, IDC_EDIT_ROOMID, m_RoomID);
	DDX_Control(pDX, IDC_EDIT_USERNUMS, m_TotalUserNums);
	DDX_Control(pDX, IDC_EDIT_PORT, m_ServerPort);
	DDX_Control(pDX, IDC_EDIT_USER_INDEX, m_UserIndex);
	DDX_Control(pDX, IDC_LIST1, m_ErrorList);
	DDX_Control(pDX, IDC_BT_LOGON_CONNECT, m_ConnectLogonServer);
	DDX_Control(pDX, IDC_BT_LOGON_EXIT_SERVER, m_ResetUserData);
	DDX_Control(pDX, IDC_BT_GAME_CONNECT, m_ConnectGameServer);
	DDX_Control(pDX, IDC_BT_EXIT_GAME_SERVER, m_ExitGameServer);
	DDX_Control(pDX, IDC_BT_AUTO_USER_QUEUE, m_AutoQueue);
	DDX_Control(pDX, IDC_BT_ALL_USER_EXIT_GAME, m_AllExitGame);
	DDX_Control(pDX, IDC_EDIT1, m_GameDLL);
	DDX_Control(pDX, IDC_IPADDRESS_GAME_SERVER, m_GameServerIP);
	DDX_Control(pDX, IDC_EDIT_GAME_SERVER_PORT, m_GameServerPort);
}

BEGIN_MESSAGE_MAP(CFlashEnterDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_WM_CREATE()
	ON_BN_CLICKED(IDC_BT_LOGON_CONNECT, OnBnClickedBtLogonConnect)
	ON_BN_CLICKED(IDC_BT_LOGON_EXIT_SERVER, OnBnClickedBtLogonExitServer)
	ON_BN_CLICKED(IDC_BT_GAME_CONNECT, OnBnClickedBtGameConnect)
	ON_BN_CLICKED(IDC_BT_EXIT_GAME_SERVER, OnBnClickedBtExitGameServer)
	ON_BN_CLICKED(IDC_BT_AUTO_USER_QUEUE, OnBnClickedBtAutoUserQueue)
	ON_BN_CLICKED(IDC_BT_ALL_USER_EXIT_GAME, OnBnClickedBtAllUserExitGame)
END_MESSAGE_MAP()


// CFlashEnterDlg 消息处理程序

BOOL CFlashEnterDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	
	//****************************************************************************************************************
	//登录服务器
	m_pGameLogon = NULL;
	m_pGameLogon = new CGameLogon();
	if (NULL == m_pGameLogon)
	{
		AfxMessageBox(TEXT("登录服务器窗口创建失败，程序即将退出！"));
		return FALSE;
	}
	CRect rcGameView(0,0,0,0);
	m_pGameLogon->Create(NULL,NULL,WS_CHILD|WS_CLIPSIBLINGS|WS_CLIPCHILDREN,rcGameView,this,1000);
	m_pGameLogon->SetWindowPos(NULL, 0, 0, 1, 1, 0);
	m_pGameLogon->ShowWindow(SW_SHOW);
	m_pGameLogon->SetMainWnd(this);
	//****************************************************************************************************************


	//****************************************************************************************************************
	//游戏服务器
	m_pGameServer = NULL;
	m_pGameServer = new CGameServer();
	if (NULL == m_pGameServer)
	{
		AfxMessageBox(TEXT("游戏服务器窗口创建失败，程序即将退出！"));
		return FALSE;
	}
	m_pGameServer->Create(NULL,NULL,WS_CHILD|WS_CLIPSIBLINGS|WS_CLIPCHILDREN,rcGameView,this,1001);
	m_pGameServer->SetWindowPos(NULL, 2, 2, 3, 3, 0);
	m_pGameServer->ShowWindow(SW_SHOW);
	m_pGameServer->SetMainWnd(this);
	//****************************************************************************************************************
	

	//连接配置
	m_UserID.SetWindowText("test");
	m_UserPassWord.SetWindowText("a");
	m_ServerIP.SetAddress(192, 168, 1, 108);
	m_ServerPort.SetWindowText("10000");
	m_GameID.SetWindowText("10");
	m_RoomID.SetWindowText("1");
	m_TotalUserNums.SetWindowText("512");
	m_UserIndex.SetWindowText("0");
	m_GameDLL.SetWindowText("");
	m_GameDLL.SetLimitText(64);
	m_GameServerIP.SetAddress(192, 168, 1, 108);
	m_GameServerPort.SetWindowText("10000");

	//m_XmlFile = NULL;
	//m_XmlFile = CXMLManager::GetInstance()->OpenXMLFile("AndroidInI.xml");
	//if (NULL != m_XmlFile)
	if(xmlParse.ReadXMLFile("AndroidInI.xml")==TRUE)
	{//获取信息
		std:string strValue = "";
		//m_XmlFile->getValue("UserID", strValue, "test");
		xmlParse.getValue("UserID", strValue, "test");
		m_UserID.SetWindowText(strValue.c_str());

		strValue = "";
		xmlParse.getValue("PassWord", strValue, "a");
		m_UserPassWord.SetWindowText(strValue.c_str());

		strValue = "";
		xmlParse.getValue("ServerIP", strValue, "192.168.1.108");
		m_ServerIP.SetWindowText(strValue.c_str());

		strValue = "";
		xmlParse.getValue("ServerPort", strValue, "10000");
		m_ServerPort.SetWindowText(strValue.c_str());

		strValue = "";
		xmlParse.getValue("GameID", strValue, "10");
		m_GameID.SetWindowText(strValue.c_str());

		strValue = "";
		xmlParse.getValue("RoomID", strValue, "1");
		m_RoomID.SetWindowText(strValue.c_str());

		strValue = "";
		xmlParse.getValue("UserIndex", strValue, "0");
		m_UserIndex.SetWindowText(strValue.c_str());

		strValue = "";
		xmlParse.getValue("UserNums", strValue, "0");
		m_TotalUserNums.SetWindowText(strValue.c_str());

		strValue = "";
		xmlParse.getValue("GameDll", strValue, "0");
		m_GameDLL.SetWindowText(strValue.c_str());

		strValue = "";
		xmlParse.getValue("GameServerIP", strValue, "192.168.1.108");
		m_GameServerIP.SetWindowText(strValue.c_str());

		strValue = "";
		xmlParse.getValue("GameServerPort", strValue, "10000");
		m_GameServerPort.SetWindowText(strValue.c_str());
	}

	m_ConnectLogonServer.EnableWindow(TRUE);
	m_ResetUserData.EnableWindow(FALSE);
	m_ConnectGameServer.EnableWindow(FALSE);
	m_ExitGameServer.EnableWindow(FALSE);
	m_AutoQueue.EnableWindow(FALSE);
	m_AllExitGame.EnableWindow(FALSE);

	DWORD dwStyle = m_ErrorList.GetExtendedStyle(); //获取当前扩展样式
	dwStyle |= LVS_EX_FULLROWSELECT; //选中某行使整行高亮（report风格时）
	dwStyle |= LVS_EX_GRIDLINES; //网格线（report风格时）
	dwStyle |= LVS_SHOWSELALWAYS; //item前生成checkbox控件
	m_ErrorList.SetExtendedStyle(dwStyle); //设置扩展风格

	m_ErrorList.InsertColumn(0,"Index",LVCFMT_LEFT,50); //插入列
	m_ErrorList.InsertColumn(1,"SocketID",LVCFMT_LEFT,100); //插入列
	m_ErrorList.InsertColumn(2,"ErrorID",LVCFMT_LEFT,100);
	m_ErrorList.InsertColumn(3,"ErrorInfo",LVCFMT_LEFT,430);
	m_ErrorList.InsertColumn(4,"time",LVCFMT_LEFT,100);

	m_dwListCountItem = 0;

	return TRUE;  // 除非设置了控件的焦点，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CFlashEnterDlg::OnPaint() 
{
	CPaintDC dc(this); // 用于绘制的设备上下文
	
	dc.SetBkMode(TRANSPARENT);
	dc.SetTextColor(RGB(0, 128, 0));

	if (NULL != m_pGameLogon)
	{
		DWORD dwLogonSucNums = 0;
		DWORD dwLogonFailNums = 0;
		DWORD dwTCPConNums = 0;
		DWORD dwUserConNums = 0;
		m_pGameLogon->GetLogonInfo(dwLogonSucNums, dwLogonFailNums, dwTCPConNums, dwUserConNums);
		char chData[20] = {0};
		//登录用户总数
		sprintf(chData, "%d", g_GlobalUnits.m_GameOption.dwUserNums);
		dc.TextOut(200, 193, chData);
		//登录成功用户数
		::memset(chData, 0, sizeof(chData));
		sprintf(chData, "%d", dwLogonSucNums);
		dc.TextOut(200, 218, chData);
		//登录失败用户数
		::memset(chData, 0, sizeof(chData));
		sprintf(chData, "%d", dwLogonFailNums);
		dc.TextOut(200, 242, chData);
		//TCP重连数
		::memset(chData, 0, sizeof(chData));
		sprintf(chData, "%d", dwTCPConNums);
		dc.TextOut(143, 269, chData);
		//User重连数
		::memset(chData, 0, sizeof(chData));
		sprintf(chData, "%d", dwUserConNums);
		dc.TextOut(285, 269, chData);
	}

	if (NULL != m_pGameServer)
	{
		DWORD dwLogonSucNums = 0;
		DWORD dwLogonFailNums = 0;
		DWORD dwTCPConNums = 0;
		DWORD dwUserConNums = 0;
		m_pGameServer->GetLogonInfo(dwLogonSucNums, dwLogonFailNums, dwTCPConNums, dwUserConNums);
		char chData[20] = {0};
		//登录用户总数
		sprintf(chData, "%d", g_GlobalUnits.m_GameOption.dwUserNums);
		dc.TextOut(560, 193, chData);
		//登录成功用户数
		::memset(chData, 0, sizeof(chData));
		sprintf(chData, "%d", dwLogonSucNums);
		dc.TextOut(560, 218, chData);
		//登录失败用户数
		::memset(chData, 0, sizeof(chData));
		sprintf(chData, "%d", dwLogonFailNums);
		dc.TextOut(560, 242, chData);
		//TCP重连数
		::memset(chData, 0, sizeof(chData));
		sprintf(chData, "%d", dwTCPConNums);
		dc.TextOut(492, 269, chData);
		//User重连数
		::memset(chData, 0, sizeof(chData));
		sprintf(chData, "%d", dwUserConNums);
		dc.TextOut(635, 269, chData);


		//排队信息
		dwLogonSucNums = 0;
		dwLogonFailNums = 0;
		m_pGameServer->GetQueueInfo(dwLogonSucNums, dwLogonFailNums);
		//排队成功用户数
		::memset(chData, 0, sizeof(chData));
		sprintf(chData, "%d", dwLogonSucNums);
		dc.TextOut(265, 330, chData);
		//排队失败用户数
		::memset(chData, 0, sizeof(chData));
		sprintf(chData, "%d", dwLogonFailNums);
		dc.TextOut(510, 330, chData);
	}

	

}

//当用户拖动最小化窗口时系统调用此函数取得光标显示。
HCURSOR CFlashEnterDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

int CFlashEnterDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码
	ShowWindow(SW_HIDE);
	return 0;
}

//更新登录信息
void CFlashEnterDlg::UpdateLogonInfo()
{
	CRect rect;
	rect.left = 200;
	rect.top = 193;
	rect.right = rect.left + 100;
	rect.bottom = rect.top + 70;
	InvalidateRect(&rect);
}

void CFlashEnterDlg::UpdateGameInfo()
{
	CRect rect;
	rect.left = 560;
	rect.top = 193;
	rect.right = rect.left + 100;
	rect.bottom = rect.top + 70;
	InvalidateRect(&rect);
}

void CFlashEnterDlg::UpdateLogonConnInfo()
{
	CRect rect;
	rect.left = 143;
	rect.top = 269;
	rect.right = rect.left + 30;
	rect.bottom = rect.top + 20;
	InvalidateRect(&rect);
	rect.left = 285;
	rect.top = 269;
	rect.right = rect.left + 30;
	rect.bottom = rect.top + 20;
	InvalidateRect(&rect);
}

void CFlashEnterDlg::UpdateGameConnInfo()
{
	CRect rect;
	rect.left = 492;
	rect.top = 269;
	rect.right = rect.left + 30;
	rect.bottom = rect.top + 20;
	InvalidateRect(&rect);
	rect.left = 635;
	rect.top = 269;
	rect.right = rect.left + 30;
	rect.bottom = rect.top + 20;
	InvalidateRect(&rect);
}

void CFlashEnterDlg::UpdateQueueInfo()
{
	CRect rect;
	rect.left = 265;
	rect.top = 330;
	rect.right = rect.left + 30;
	rect.bottom = rect.top + 20;
	InvalidateRect(&rect);
	rect.left = 510;
	rect.top = 330;
	rect.right = rect.left + 30;
	rect.bottom = rect.top + 20;
	InvalidateRect(&rect);
}

void CFlashEnterDlg::OnBnClickedBtLogonConnect()
{
	m_dwListCountItem = 0;
	ClearAllListData();
	g_GlobalUnits.m_bInitGameKindList = false;
	g_GlobalUnits.m_GameKindList.clear();
	//连接登录服务器
	bool bIsSuc = SetGameOptionInfo();
	if (!bIsSuc)
	{
		return;
	}
	WORD wRet = CGameUserInfo::GetInstance()->InitGameUserInfo();
	if (0 != wRet)
	{
		char chData[64] = {0};
		sprintf(chData, "初始化游戏第%d个用户信息失败！", wRet);
		MessageBox(chData);
		return;
	}

	//if (NULL != m_XmlFile)
	{//获取信息
		CString strValue="";
		m_UserID.GetWindowText(strValue);
		//m_XmlFile->setValue("UserID", strValue.GetBuffer());
		xmlParse.setValue("UserID", strValue.GetBuffer());

		strValue="";
		m_UserPassWord.GetWindowText(strValue);
		xmlParse.setValue("PassWord", strValue.GetBuffer());

		strValue="";
		m_ServerIP.GetWindowText(strValue);
		xmlParse.setValue("ServerIP", strValue.GetBuffer());

		strValue="";
		m_ServerPort.GetWindowText(strValue);
		xmlParse.setValue("ServerPort", strValue.GetBuffer());

		strValue="";
		m_GameID.GetWindowText(strValue);
		xmlParse.setValue("GameID", strValue.GetBuffer());

		strValue="";
		m_RoomID.GetWindowText(strValue);
		xmlParse.setValue("RoomID", strValue.GetBuffer());

		strValue="";
		m_UserIndex.GetWindowText(strValue);
		xmlParse.setValue("UserIndex", strValue.GetBuffer());

		strValue="";
		m_TotalUserNums.GetWindowText(strValue);
		xmlParse.setValue("UserNums", strValue.GetBuffer());

		strValue="";
		m_GameDLL.GetWindowText(strValue);
		xmlParse.setValue("GameDll", strValue.GetBuffer());

		strValue="";
		m_GameServerIP.GetWindowText(strValue);
		xmlParse.setValue("GameServerIP", strValue.GetBuffer());

		strValue="";
		m_GameServerPort.GetWindowText(strValue);
		xmlParse.setValue("GameServerPort", strValue.GetBuffer());

		BOOL iRet=xmlParse.WriteXMLFile("AndroidInI.xml");
	}

	if (NULL != m_pGameLogon)
	{
		m_ConnectLogonServer.EnableWindow(FALSE);
		m_ResetUserData.EnableWindow(TRUE);
		m_ConnectGameServer.EnableWindow(TRUE);
		m_ExitGameServer.EnableWindow(FALSE);
		m_AutoQueue.EnableWindow(FALSE);
		m_AllExitGame.EnableWindow(FALSE);
		m_pGameLogon->OnEventConnectServer();
	}
}

void CFlashEnterDlg::OnBnClickedBtLogonExitServer()
{
	//重置数据
	if (NULL != m_pGameLogon)
	{
		m_ConnectLogonServer.EnableWindow(TRUE);
		m_ResetUserData.EnableWindow(FALSE);
		m_ConnectGameServer.EnableWindow(FALSE);
		m_ExitGameServer.EnableWindow(FALSE);
		m_AutoQueue.EnableWindow(FALSE);
		g_GlobalUnits.m_bInitGameKindList = false;
		g_GlobalUnits.m_GameKindList.clear();

		bool bRet = m_pGameLogon->OnEventExitServer();
		if (!bRet)
		{
			AfxMessageBox(TEXT("退出登录服务器消息失败！"));
		}
	}
	ClearAllListData();
	SetGameOptionInfo();
}

//设置游戏配置信息
bool CFlashEnterDlg::SetGameOptionInfo()
{
	//设置游戏配置
	CString strInfo = "";
	m_UserID.GetWindowText(strInfo);
	if (!strInfo.IsEmpty())
	{
		::memcpy(g_GlobalUnits.m_GameOption.chUserID, strInfo.GetBuffer(), strInfo.GetLength());
	}
	else
	{
		m_UserID.SetFocus();
		AfxMessageBox("UserID不能为空，请输入UserID！");
		return false;
	}
	strInfo = "";
	m_UserPassWord.GetWindowText(strInfo);
	::memset(g_GlobalUnits.m_GameOption.chUserPassWord, 0, sizeof(g_GlobalUnits.m_GameOption.chUserPassWord));
	if (!strInfo.IsEmpty())
	{
		::memcpy(g_GlobalUnits.m_GameOption.chUserPassWord, strInfo.GetBuffer(), strInfo.GetLength());
	}
	else
	{
		m_UserPassWord.SetFocus();
		AfxMessageBox("密码不能为空，请输入密码！");
		return false;
	}
	strInfo = "";
	m_ServerIP.GetWindowText(strInfo);
	if (!strInfo.IsEmpty())
	{
		::memcpy(g_GlobalUnits.m_GameOption.chServerIP, strInfo.GetBuffer(), strInfo.GetLength());
	}
	else
	{
		m_ServerIP.SetFocus();
		AfxMessageBox("登录IP地址不能为空，请输入登录IP地址！");
		return false;
	}

	//游戏服务器IP
	strInfo ="";
	m_GameServerIP.GetWindowText(strInfo);
	if (!strInfo.IsEmpty())
	{
		::memcpy(g_GlobalUnits.m_GameOption.chGameServerIP, strInfo.GetBuffer(), strInfo.GetLength());
	}
	else
	{
		m_GameServerIP.SetFocus();
		AfxMessageBox("游戏服务器IP地址不能为空，请输入游戏服务器IP地址！");
		return false;
	}

	//游戏服务器端口
	m_GameServerPort.GetWindowText(strInfo);
	if (!strInfo.IsEmpty())
	{
		g_GlobalUnits.m_GameOption.dwGameServerPort = ::atoi(strInfo.GetBuffer());
	}
	else
	{
		m_ServerPort.SetFocus();
		AfxMessageBox("游戏服务器端口号不能为空，请输入游戏服务器端口号！");
		return false;
	}
	
	strInfo = "";
	m_GameID.GetWindowText(strInfo);
	if (!strInfo.IsEmpty())
	{
		g_GlobalUnits.m_GameOption.dwGameID = ::atoi(strInfo.GetBuffer());
	}
	else
	{
		m_GameID.SetFocus();
		AfxMessageBox("游戏ID不能为空，请输入游戏ID！");
		return false;
	}
	
	strInfo = "";
	m_UserIndex.GetWindowText(strInfo);
	if (!strInfo.IsEmpty())
	{
		g_GlobalUnits.m_GameOption.dwUserIndex = ::atoi(strInfo.GetBuffer());
	}
	else
	{
		m_UserIndex.SetFocus();
		AfxMessageBox("用户登录起始序号不能为空，请输入用户登录起始序号！");
		return false;
	}
	
	strInfo = "";
	m_RoomID.GetWindowText(strInfo);
	if (!strInfo.IsEmpty())
	{
		g_GlobalUnits.m_GameOption.dwRoomID = ::atoi(strInfo.GetBuffer());
	}
	else
	{
		m_RoomID.SetFocus();
		AfxMessageBox("房间ID不能为空，请输入房间ID！");
		return false;
	}
	
	strInfo = "";
	m_TotalUserNums.GetWindowText(strInfo);
	if (!strInfo.IsEmpty())
	{
		g_GlobalUnits.m_GameOption.dwUserNums = ::atoi(strInfo.GetBuffer());
	}
	else
	{
		m_TotalUserNums.SetFocus();
		AfxMessageBox("总机器人数不能为空，请输入机器人数！");
		return false;
	}
	
	strInfo = "";
	m_ServerPort.GetWindowText(strInfo);
	if (!strInfo.IsEmpty())
	{
		g_GlobalUnits.m_GameOption.dwServerPort = ::atoi(strInfo.GetBuffer());
	}
	else
	{
		m_ServerPort.SetFocus();
		AfxMessageBox("端口号不能为空，请输入端口号！");
		return false;
	}
	
	strInfo = "";
	m_GameDLL.GetWindowText(strInfo);
	bool bIsLoadSuc = false;
	if (!strInfo.IsEmpty())
	{
		bIsLoadSuc = CGameUserInfo::GetInstance()->LoadGameClientDll(strInfo.GetBuffer());
	}
	else
	{
		m_GameDLL.SetFocus();
		AfxMessageBox("游戏DLL不能为空，请输入要加载的游戏DLL！");
		return false;
	}
	if (!bIsLoadSuc)
	{
		AfxMessageBox("游戏客户端DLL加载不成功！");
		return false;
	}
	return true;
}

void CFlashEnterDlg::OnBnClickedBtGameConnect()
{
	//连接游戏服务器
	ClearAllListData();
	if (NULL != m_pGameServer)
	{
		m_ConnectLogonServer.EnableWindow(FALSE);
		m_ResetUserData.EnableWindow(FALSE);
		m_ConnectGameServer.EnableWindow(FALSE);
		m_ExitGameServer.EnableWindow(TRUE);
		m_AutoQueue.EnableWindow(TRUE);
		m_AllExitGame.EnableWindow(FALSE);

		m_pGameServer->OnConnectGameServer();
	}
}

void CFlashEnterDlg::OnBnClickedBtExitGameServer()
{
	//退出游戏服务器
	if (NULL != m_pGameServer)
	{
		m_ConnectLogonServer.EnableWindow(FALSE);
		m_ResetUserData.EnableWindow(TRUE);
		m_ConnectGameServer.EnableWindow(TRUE);
		m_ExitGameServer.EnableWindow(FALSE);
		m_AutoQueue.EnableWindow(FALSE);
		m_AllExitGame.EnableWindow(FALSE);

		m_pGameServer->OnEventExitServer();
	}
}

//插入列表数据
void CFlashEnterDlg::InsertListData(WORD dwSocketID, int dwErrorID, char* pErrorData)
{
	++m_dwListCountItem;
	if (m_dwListCountItem>=1000)
	{
		ClearAllListData();
	}
	
	char chData[256]={0};

	::memset(chData, 0, sizeof(chData));
	sprintf(chData, "%d", m_dwListCountItem);
	int nRow = m_ErrorList.InsertItem(0, chData);        //插入行

	::memset(chData, 0, sizeof(chData));
	sprintf(chData, "%d", dwSocketID);
	m_ErrorList.SetItemText(nRow,1,chData);  //设置数据

	::memset(chData, 0, sizeof(chData));
	sprintf(chData, "%d", dwErrorID);
	m_ErrorList.SetItemText(nRow,2,chData);  //设置数据

	::memset(chData, 0, sizeof(chData));
	m_ErrorList.SetItemText(nRow,3,pErrorData);  //设置数据

	//时间
	CTime time = CTime::GetCurrentTime(); ///构造CTime对象
	int m_nYear = time.GetYear(); ///年
	int m_nMonth = time.GetMonth(); ///月
	int m_nDay = time.GetDay(); ///日
	int m_nHour = time.GetHour(); ///小时
	int m_nMinute = time.GetMinute(); ///分钟
	int m_nSecond = time.GetSecond(); ///秒
	::memset(chData, 0, sizeof(chData));
	sprintf(chData, "%d/%d/%d-%d/%d/%d", m_nYear, m_nMonth, m_nDay, m_nHour, m_nMinute, m_nSecond);
	m_ErrorList.SetItemText(nRow, 4, chData);

	m_ErrorList.EnsureVisible(nRow, FALSE);

	m_ErrorList.SetRedraw(TRUE);
	m_ErrorList.Invalidate();
	m_ErrorList.UpdateWindow();
}

//清空所有数据
void CFlashEnterDlg::ClearAllListData()
{
	m_dwListCountItem = 0;
	m_ErrorList.DeleteAllItems();
	m_ErrorList.SetRedraw(TRUE);
	m_ErrorList.Invalidate();
	m_ErrorList.UpdateWindow();
}

//退出窗口
void CFlashEnterDlg::OnCloseTool()
{
	OnCancel();
}

void CFlashEnterDlg::OnBnClickedBtAutoUserQueue()
{//自动排队
	if (NULL != m_pGameServer)
	{
		m_pGameServer->SetAllUserQueue();

		m_AllExitGame.EnableWindow(TRUE);
	}
}

void CFlashEnterDlg::OnBnClickedBtAllUserExitGame()
{
	//所有玩家退出游戏
	CGameUserInfo::GetInstance()->AllUserExitGame();
}
