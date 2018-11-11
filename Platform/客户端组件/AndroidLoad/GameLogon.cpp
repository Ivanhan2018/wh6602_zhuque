// GameLogon.cpp : 实现文件
//


#include "GameLogon.h"
#include ".\gamelogon.h"
#include "GlobalUnits.h"
#include "FlashEnterDlg.h"
#include <WinSock.h>


//连接定时器
#define			IDI_TIMER_USER_LOGON_LOOK			4000
#define			TIME_USER_LOGON_LOOK_INTERVAL		1000

//一次连接数
#define			LOGON_CONNECT_ONCE_NUMS				100

// CGameLogon

IMPLEMENT_DYNAMIC(CGameLogon, CWnd)
CGameLogon::CGameLogon()
{
	m_pMainWnd = NULL;
	m_pGameUserInfo = NULL;
	::memset(&m_LogonInfo, 0, sizeof(m_LogonInfo));
}

CGameLogon::~CGameLogon()
{
}


BEGIN_MESSAGE_MAP(CGameLogon, CWnd)
	ON_WM_CREATE()
	ON_WM_TIMER()
END_MESSAGE_MAP()

//接口查询
void * __cdecl CGameLogon::QueryInterface(const IID & Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IViewItem,Guid,dwQueryVer);
	QUERYINTERFACE(ITCPSocketSink,Guid,dwQueryVer);
	//QUERYINTERFACE(IPlazaViewItem,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IViewItem,Guid,dwQueryVer);
	return NULL;
}

// CGameLogon 消息处理程序


int CGameLogon::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (__super::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码
	return 0;
}


//连接服务器
bool CGameLogon::OnEventConnectServer()
{
	KillTimer(IDI_TIMER_USER_LOGON_LOOK);

	//把所有玩家注册到登录服务器
	m_pGameUserInfo = CGameUserInfo::GetInstance();
	::memset(&m_LogonInfo, 0, sizeof(m_LogonInfo));

	IUnknownEx * pIUnknownEx=(IUnknownEx *)QueryInterface(IID_IUnknownEx,VER_IUnknownEx);
	bool bRet = m_pGameUserInfo->SetIUnknownEx(1, pIUnknownEx);
	if (!bRet)
	{
		AfxMessageBox(TEXT("网络组件回调接口设置失败！"));
		return false;
	}
	
	if (m_pGameUserInfo->m_vecUserGameInfo.size()>0)
	{
		DWORD dwCountConnectNums = 0;
		std::map<DWORD, strUserGameInfo*>::iterator iter = m_pGameUserInfo->m_vecUserGameInfo.begin();
		for (; iter!=m_pGameUserInfo->m_vecUserGameInfo.end(); ++iter)
		{
			if ((NULL!=(*iter).second)&&(TAG_LINK_FAIL == ((*iter).second)->dwLogonLinkState))
			{
				((*iter).second)->dwCountConnect = 0;
				((*iter).second)->TcpSocketHelper->Connect(g_GlobalUnits.m_GameOption.chServerIP,g_GlobalUnits.m_GameOption.dwServerPort);
				((*iter).second)->dwLogonLinkState = TAG_LINKING;
				((*iter).second)->dwUserLogonState = USER_LOGON_SERVER_FAIL;
				++dwCountConnectNums;
				if (dwCountConnectNums>=LOGON_CONNECT_ONCE_NUMS)
				{
					break;
				}
			}
		}
	}
	

	//设置定时器检测登录失败的.
	//发送连接成功.
	SetTimer(IDI_TIMER_USER_LOGON_LOOK, TIME_USER_LOGON_LOOK_INTERVAL, NULL);
	return true;
}


//网络事件
//连接事件
bool __cdecl CGameLogon::OnEventTCPSocketLink(WORD wSocketID, INT nErrorCode)
{
	std::map<DWORD, strUserGameInfo*>::iterator iter = m_pGameUserInfo->m_vecUserGameInfo.find(wSocketID);
	if (iter != m_pGameUserInfo->m_vecUserGameInfo.end())
	{
		if (nErrorCode!=0)
		{//连接失败.
			((*iter).second)->dwLogonLinkState = TAG_LINK_FAIL;
			++(((*iter).second)->dwCountConnect);
			if (NULL != m_pMainWnd)
			{
				char chData[128]={0};
				sprintf(chData, "玩家[%s]的TCP与登录服务器断开连接！", ((*iter).second)->chName);
				m_pMainWnd->InsertListData(wSocketID, nErrorCode, chData);
			}
			return true;
		}
		else
		{//连接成功.
			UserLogonToLogonServer((*iter).second);
		}
	}
	return true;
}

//登录至登录服务器
void CGameLogon::UserLogonToLogonServer(strUserGameInfo* pUserInfo)
{
	if (NULL != pUserInfo)
	{
		pUserInfo->dwUserID = 0;
		//变量定义
		TCHAR szPassword[33] = {0};
		BYTE cbBuffer[SOCKET_PACKET] = {0};
		CMD5Encrypt::EncryptData(g_GlobalUnits.m_GameOption.chUserPassWord,szPassword);

		//构造数据
		CMD_GP_LogonByAccounts * pLogonByAccounts=(CMD_GP_LogonByAccounts *)cbBuffer;
		memset(pLogonByAccounts,0,sizeof(CMD_GP_LogonByAccounts));
		pLogonByAccounts->dwPlazaVersion=g_GlobalUnits.GetPlazaVersion();
		CopyMemory(pLogonByAccounts->szPassWord,szPassword,sizeof(pLogonByAccounts->szPassWord));
		lstrcpyn(pLogonByAccounts->szAccounts,pUserInfo->chName,CountArray(pLogonByAccounts->szAccounts));

		//机器序列号
		tagClientSerial ClientSerial;
		g_GlobalUnits.GetClientSerial(ClientSerial);

		//发送数据包
		CSendPacketHelper Packet(cbBuffer+sizeof(CMD_GP_LogonByAccounts),sizeof(cbBuffer)-sizeof(CMD_GP_LogonByAccounts));
		Packet.AddPacket(&ClientSerial,sizeof(ClientSerial),DTP_COMPUTER_ID);
		pUserInfo->TcpSocketHelper->SendData(MDM_GP_LOGON,SUB_GP_LOGON_ACCOUNTS,cbBuffer,sizeof(CMD_GP_LogonByAccounts)+Packet.GetDataSize());
		pUserInfo->dwLogonLinkState = TAG_LINK_SUC;
		pUserInfo->dwUserLogonState = USER_LOGON_SERVER_LINKING;
	}
}

//关闭事件
bool __cdecl CGameLogon::OnEventTCPSocketShut(WORD wSocketID, BYTE cbShutReason)
{
	if (cbShutReason!=SHUT_REASON_NORMAL)
	{
		//关闭连接
		std::map<DWORD, strUserGameInfo*>::iterator iter = m_pGameUserInfo->m_vecUserGameInfo.find(wSocketID);
		if (iter != m_pGameUserInfo->m_vecUserGameInfo.end())
		{
			((*iter).second)->dwLogonLinkState = TAG_LINK_FAIL;
			++(((*iter).second)->dwCountConnect);
			if (NULL != m_pMainWnd)
			{
				char chData[128]={0};
				sprintf(chData, "玩家[%s]的TCP与登录服务器断开连接！", ((*iter).second)->chName);
				m_pMainWnd->InsertListData(wSocketID, cbShutReason, chData);
			}
		}
	}
	return true;
}

//读取事件
bool __cdecl CGameLogon::OnEventTCPSocketRead(WORD wSocketID, CMD_Command Command, VOID * pData, WORD wDataSize)
{
	switch (Command.wMainCmdID)
	{
	case MDM_GP_LOGON:				//登录消息
		{
			return OnSocketMainLogon(wSocketID, Command,pData,wDataSize);		//用户验证、登录成功与否信息
		}
	case MDM_GP_SERVER_LIST:		//列表消息
		{
			return OnSocketMainServerList(wSocketID, Command,pData,wDataSize);	//下载游戏种类列表
		}
	default:
		break;
	}
	return true;
}


//登录消息
bool CGameLogon::OnSocketMainLogon(WORD wSocketID, CMD_Command Command, void * pData, WORD wDataSize)
{
	switch (Command.wSubCmdID)
	{
	case SUB_GP_LOGON_SUCCESS:		//登录成功
		{
			//效验参数
			ASSERT(wDataSize>=sizeof(CMD_GP_LogonSuccess));
			if (wDataSize<sizeof(CMD_GP_LogonSuccess))
			{
				return false;
			}
			
			CMD_GP_LogonSuccess * pLogonSuccess=(CMD_GP_LogonSuccess *)pData;
			if (NULL != pLogonSuccess)
			{
				std::map<DWORD, strUserGameInfo*>::iterator iter = m_pGameUserInfo->m_vecUserGameInfo.find(wSocketID);
				if (iter != m_pGameUserInfo->m_vecUserGameInfo.end())
				{
					(*iter).second->dwUserID = pLogonSuccess->dwUserID;
					(*iter).second->llUserGems = pLogonSuccess->lGold;
				}
			}
			return true;
		}
	case SUB_GP_LOGON_ERROR:		//登录失败
		{
			//效验参数
			CMD_GP_LogonError *pLogonError = (CMD_GP_LogonError *)pData;
			ASSERT(wDataSize>=(sizeof(CMD_GP_LogonError)-sizeof(pLogonError->szErrorDescribe)));
			if (wDataSize<(sizeof(CMD_GP_LogonError)-sizeof(pLogonError->szErrorDescribe)))
			{
				return false;
			}
			
			//关闭连接
			std::map<DWORD, strUserGameInfo*>::iterator iter = m_pGameUserInfo->m_vecUserGameInfo.find(wSocketID);
			if (iter != m_pGameUserInfo->m_vecUserGameInfo.end())
			{
				if (NULL != m_pMainWnd)
				{
					char chData[128]={0};
					sprintf(chData, "玩家[%s]登录到登录服务器失败！", ((*iter).second)->chName);
					m_pMainWnd->InsertListData(wSocketID, 0, chData);
				}
				((*iter).second)->dwUserLogonState = USER_LOGON_SERVER_FAIL;
				++(((*iter).second)->dwCountConnect);

				++m_LogonInfo.dwLogonFailNums;
				if (NULL != m_pMainWnd)
				{
					m_pMainWnd->UpdateLogonInfo();
				}
			}
			return true;
		}
	case SUB_GP_LOGON_FINISH:		//登录完成
		{
			std::map<DWORD, strUserGameInfo*>::iterator iter = m_pGameUserInfo->m_vecUserGameInfo.find(wSocketID);
			if (iter != m_pGameUserInfo->m_vecUserGameInfo.end())
			{
				if (TAG_LINK_SUC == ((*iter).second)->dwLogonLinkState)
				{
					//关闭登录连接
					((*iter).second)->TcpSocketHelper->CloseSocket();
					((*iter).second)->dwUserLogonState = USER_LOGON_SERVER_SUC;
					((*iter).second)->dwCountConnect = 0;
					
					++m_LogonInfo.dwLogonSucNums;
					if (NULL != m_pMainWnd)
					{
						m_pMainWnd->UpdateLogonInfo();
					}
				}
			}
			
			return true;
		}
	}
	return true;
}

bool CGameLogon::OnSocketMainServerList(WORD wSocketID, CMD_Command Command, void * pData, WORD wDataSize)
{
	ASSERT(Command.wMainCmdID==MDM_GP_SERVER_LIST);

	switch (Command.wSubCmdID)
	{
	case SUB_GP_LIST_SERVER:		//服务器房间
		{
			if (g_GlobalUnits.m_bInitGameKindList)
			{
				return true;
			}

			//效验参数
			ASSERT(wDataSize%sizeof(tagGameServer)==0);
			if (wDataSize%sizeof(tagGameServer)!=0)
			{
				return false;
			}
			
			//处理消息
			tagGameServer * pGameServer=(tagGameServer *)pData;
			WORD wItemCount=wDataSize/sizeof(tagGameServer);
			if (NULL != pGameServer)
			{
				for (WORD i=0; i<wItemCount; i++)
				{
					if (g_GlobalUnits.m_GameOption.dwGameID==pGameServer[i].wKindID && g_GlobalUnits.m_GameOption.dwRoomID==pGameServer[i].wSortID)
					{//哪个游戏哪个房间
						GameServerList gsl;
						::memset(&gsl, 0, sizeof(GameServerList));
						gsl.wSortID = pGameServer[i].wSortID;
						gsl.wKindID = pGameServer[i].wKindID;
						//gsl.dwGameServerIP = pGameServer[i].dwServerAddrCNN;
						//gsl.dwGameServerport = pGameServer[i].wServerPort; 

						gsl.dwGameServerIP = inet_addr(g_GlobalUnits.m_GameOption.chGameServerIP);
						gsl.dwGameServerport = g_GlobalUnits.m_GameOption.dwGameServerPort; 

						gsl.dwOnLineCount = pGameServer[i].dwOnLineCount;
						gsl.dwMaxCount = pGameServer[i].dwMaxCount;
						gsl.lRestrictScore = pGameServer[i].lRestrictScore;
						g_GlobalUnits.m_GameKindList.push_back(gsl);
						
						if (NULL != m_pMainWnd)
						{
							char chData[256]={0};
							sprintf(chData, "游戏服务列表信息:KindID:%d, SortID:%d, IP:%d, Port:%d, OnLine:%d, MaxLine:%d.", gsl.wKindID, gsl.wSortID, 
								gsl.dwGameServerIP, gsl.dwGameServerport, gsl.dwOnLineCount, gsl.dwMaxCount);
							m_pMainWnd->InsertListData(0, 0, chData);
						}
					}
				}
			}
			g_GlobalUnits.m_bInitGameKindList = true;
			return true;
		}
	}

	return true;
}

//设置主窗口指针
void CGameLogon::SetMainWnd(CFlashEnterDlg* pMainWnd)
{
	if (NULL != pMainWnd)
	{
		m_pMainWnd = pMainWnd;
	}
}

//返回相应值
void CGameLogon::GetLogonInfo(DWORD &dwLogonSucNums, DWORD &dwLogonFailNums, DWORD &dwTCPCoonectNums, DWORD &dwUserConnectNums)
{
	dwLogonSucNums = m_LogonInfo.dwLogonSucNums;
	dwLogonFailNums = m_LogonInfo.dwLogonFailNums;
	dwTCPCoonectNums = m_LogonInfo.dwTCPConnectNums;
	dwUserConnectNums = m_LogonInfo.dwUserConnectNums;
}

//退出登录服务器
bool CGameLogon::OnEventExitServer()
{
	KillTimer(IDI_TIMER_USER_LOGON_LOOK);
	m_pGameUserInfo->ClearGameUserInfo();
	::memset(&m_LogonInfo, 0, sizeof(m_LogonInfo));
	if (NULL != m_pMainWnd)
	{
		m_pMainWnd->UpdateLogonInfo();
		m_pMainWnd->UpdateLogonConnInfo();
	}
	return true;
}

void CGameLogon::OnTimer(UINT nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (IDI_TIMER_USER_LOGON_LOOK == nIDEvent)
	{//检测TCP连接状态
		m_LogonInfo.dwTCPConnectNums = 0;
		m_LogonInfo.dwUserConnectNums = 0;
		std::map<DWORD, strUserGameInfo*>::iterator iter = m_pGameUserInfo->m_vecUserGameInfo.begin();
		for (; iter!=m_pGameUserInfo->m_vecUserGameInfo.end(); ++iter)
		{
			if (NULL==(*iter).second) continue;
			if (((*iter).second)->dwCountConnect >= I_CONNECT_NUMS) continue;
			if (TAG_LINK_FAIL == ((*iter).second)->dwLogonLinkState)
			{
				if (m_LogonInfo.dwTCPConnectNums>=LOGON_CONNECT_ONCE_NUMS)
				{
					continue;
				}
				if (NULL == ((*iter).second)->TcpSocketHelper.GetInterface())
				{
					((*iter).second)->TcpSocketHelper.CreateInstance();
				}
				IUnknownEx * pIUnknownEx=(IUnknownEx *)QueryInterface(IID_IUnknownEx,VER_IUnknownEx);
				if (false == ((*iter).second)->TcpSocketHelper->SetTCPSocketSink(pIUnknownEx))
				{
					KillTimer(IDI_TIMER_USER_LOGON_LOOK);
					break;
				}
				((*iter).second)->TcpSocketHelper->Connect(g_GlobalUnits.m_GameOption.chServerIP,g_GlobalUnits.m_GameOption.dwServerPort);
				((*iter).second)->dwLogonLinkState = TAG_LINKING;
				++m_LogonInfo.dwTCPConnectNums;
			}
			else if (TAG_LINK_SUC == ((*iter).second)->dwLogonLinkState)
			{//TCP连接成功的前提条件下.
				if (USER_LOGON_SERVER_FAIL == ((*iter).second)->dwUserLogonState)
				{//登录失败.
					if (m_LogonInfo.dwUserConnectNums>=LOGON_CONNECT_ONCE_NUMS)
					{
						continue;
					}
					UserLogonToLogonServer((*iter).second);
					++m_LogonInfo.dwUserConnectNums;
				}
			}
			if (m_LogonInfo.dwTCPConnectNums>=LOGON_CONNECT_ONCE_NUMS || m_LogonInfo.dwUserConnectNums>=LOGON_CONNECT_ONCE_NUMS)
			{
				break;
			}
		}
		m_LogonInfo.dwLogonFailNums -= m_LogonInfo.dwUserConnectNums;
		if (NULL != m_pMainWnd)
		{
			m_pMainWnd->UpdateLogonConnInfo();
		}
	}
	__super::OnTimer(nIDEvent);
}
