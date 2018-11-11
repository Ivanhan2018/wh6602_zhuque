#include "stdafx.h"
#include "gameuserinfo.h"
#include "GlobalUnits.h"


CGameUserInfo*	CGameUserInfo::m_Instance = NULL;

CGameUserInfo::CGameUserInfo(void)
{
	m_ptrFreeGameClient = NULL;
	m_ptrNewGameClient = NULL;
	m_hGameDllInstance = NULL;
	m_vecUserGameInfo.clear();
	::memset(&m_GameServerInfo, 0, sizeof(m_GameServerInfo));
}

CGameUserInfo::~CGameUserInfo(void)
{
	m_ptrNewGameClient = NULL;
	m_ptrFreeGameClient = NULL;
	if (NULL != m_hGameDllInstance)
	{
		::FreeLibrary(m_hGameDllInstance);
	}
	if (NULL != m_Instance)
	{
		delete m_Instance;
	}
	m_Instance = NULL;
}

CGameUserInfo* CGameUserInfo::GetInstance()
{
	if (NULL == m_Instance)
	{
		m_Instance = new CGameUserInfo();
	}
	return m_Instance;
}

//加载DLL
bool CGameUserInfo::LoadGameClientDll(const std::string& strDllName)
{
	if (!strDllName.empty())
	{
		m_ptrFreeGameClient = NULL;
		m_ptrNewGameClient = NULL;
		m_hGameDllInstance = NULL;

		m_hGameDllInstance = ::LoadLibrary(strDllName.c_str());
		if (NULL != m_hGameDllInstance)
		{
			m_ptrNewGameClient = (NewGameClient*)::GetProcAddress(m_hGameDllInstance, TEXT("NewGameClient"));
			if (NULL == m_ptrNewGameClient)
			{
				return false;
			}
			m_ptrFreeGameClient = (FreeGameClient*)::GetProcAddress(m_hGameDllInstance, TEXT("FreeGameClient"));
			if (NULL == m_ptrFreeGameClient)
			{
				return false;
			}
			return true;
		}
	}
	
	return false;
}

//初始化信息
WORD CGameUserInfo::InitGameUserInfo()
{
	m_vecUserGameInfo.clear();
	for (WORD i=0; i<g_GlobalUnits.m_GameOption.dwUserNums; i++)
	{
		strUserGameInfo* info = new strUserGameInfo();
		if (NULL==info)continue;
		::memset(info->chName, 0, sizeof(info->chName));
		::sprintf(info->chName, "%s_%d", g_GlobalUnits.m_GameOption.chUserID, g_GlobalUnits.m_GameOption.dwUserIndex+i+1);
		info->dwSocketID = g_GlobalUnits.m_GameOption.dwSockStartID+i;
		info->dwKindID = g_GlobalUnits.m_GameOption.dwGameID;
		info->dwGameServerIP = 0;
		info->dwGameServerport = 0;
		info->dwCountConnect = 0;
		info->dwLogonLinkState = TAG_LINK_FAIL;
		info->dwGameLinkState = TAG_LINK_FAIL;
		info->dwUserLogonState = USER_LOGON_SERVER_FAIL;
		info->dwUserGameState = USER_LOGON_SERVER_FAIL;
		info->dwUserID = 0;

		//创建游戏客户端
		bool bIsCreateSuc = false;
		info->pIGameClient = NULL;
		
		CWnd* pMain = AfxGetMainWnd();
		if (NULL != pMain)
		{
			if (NULL != m_ptrNewGameClient)
			{
				info->pIGameClient = m_ptrNewGameClient(pMain, info->dwSocketID);
				if (NULL != info->pIGameClient)
				{
					bIsCreateSuc = true;
				}
			}
		}
		
		if (!bIsCreateSuc)
		{//申请空间失败
			return i+1;
		}

		//创建Socket
		if (!info->TcpSocketHelper.CreateInstance()) 
		{
			return i+1;
		}
		info->TcpSocketHelper->SetSocketID(g_GlobalUnits.m_GameOption.dwSockStartID+i);
		
		//插入信息树
		pair<DWORD, strUserGameInfo*> value(info->dwSocketID, info);
		m_vecUserGameInfo.insert(value);
	}

	return 0;
}

//释放初始化信息
bool CGameUserInfo::FreeGameUserInfo()
{
	std::map<DWORD, strUserGameInfo*>::iterator iter = m_vecUserGameInfo.begin();
	for (; iter!=m_vecUserGameInfo.end(); ++iter)
	{
		if (NULL != (*iter).second)
		{
			//释放socket组件.
			(*iter).second->TcpSocketHelper.CloseInstance();
			//释放游戏客户端
			if (NULL != (*iter).second->pIGameClient)
			{
				if (NULL != m_ptrFreeGameClient)
				{
					m_ptrFreeGameClient((*iter).second->pIGameClient);
				}
				(*iter).second->pIGameClient = NULL;
			}
			delete (*iter).second;
		}
		(*iter).second=NULL;
	}
	m_vecUserGameInfo.clear();
	return true;
}


//所有玩家退出游戏
void CGameUserInfo::AllUserExitGame()
{
	std::map<DWORD, strUserGameInfo*>::iterator iter = m_vecUserGameInfo.begin();
	for (; iter!=m_vecUserGameInfo.end(); ++iter)
	{
		if ((NULL != (*iter).second->pIGameClient) && ((*iter).second->pIGameClient)->IsEnableWnd())
		{
			const tagUserData* pUserData = ((*iter).second->pIGameClient)->GetMeData();
			if (NULL != pUserData)
			{
				if (US_PLAY == pUserData->cbUserStatus)
				{//如果是游戏中，则强退。
					((*iter).second)->TcpSocketHelper->SendData(MDM_GR_USER,SUB_GR_USER_LEFT_GAME_REQ);
				}
				else
				{//离开游戏,发送起来消息。
					((*iter).second)->TcpSocketHelper->SendData(MDM_GR_USER,SUB_GR_USER_STANDUP_REQ);
				}
			}
		}
	}
}

//关闭所有socket
void CGameUserInfo::CloseAllSocket()
{//退出游戏服务器使用.
	std::map<DWORD, strUserGameInfo*>::iterator iter = m_vecUserGameInfo.begin();
	for (; iter!=m_vecUserGameInfo.end(); ++iter)
	{
		((*iter).second)->dwGameLinkState = TAG_LINK_FAIL;
		((*iter).second)->dwUserGameState = USER_LOGON_SERVER_FAIL;
		((*iter).second)->TcpSocketHelper->CloseSocket();
	}
}

//清空并退出服务器
void CGameUserInfo::ClearGameUserInfo()
{
	std::map<DWORD, strUserGameInfo*>::iterator iter = m_vecUserGameInfo.begin();
	for (; iter!=m_vecUserGameInfo.end(); ++iter)
	{
		((*iter).second)->dwLogonLinkState = TAG_LINK_FAIL;
		((*iter).second)->dwGameLinkState = TAG_LINK_FAIL;
		((*iter).second)->dwUserLogonState = USER_LOGON_SERVER_FAIL;
		((*iter).second)->dwUserGameState = USER_LOGON_SERVER_FAIL;
		((*iter).second)->TcpSocketHelper->CloseSocket();
	}
	FreeGameUserInfo();
}

//设置组件回调关联
bool CGameUserInfo::SetIUnknownEx(BYTE byTag, IUnknownEx * pIUnknownEx)
{
	std::map<DWORD, strUserGameInfo*>::iterator iter = m_vecUserGameInfo.begin();
	for (; iter!=m_vecUserGameInfo.end(); ++iter)
	{
		if (NULL == (*iter).second)
		{
			continue;
		}
		if (1 == byTag)
		{//登录
			if (NULL == ((*iter).second)->TcpSocketHelper.GetInterface())
			{
				((*iter).second)->TcpSocketHelper.CreateInstance();
			}
			if (false == ((*iter).second)->TcpSocketHelper->SetTCPSocketSink(pIUnknownEx))
			{
				return false;
			}
		}
		else if ((2 == byTag) && (0 != (*iter).second->dwUserID) && (TAG_LINK_SUC == ((*iter).second)->dwLogonLinkState))
		{//登录成功.
			if (NULL == ((*iter).second)->TcpSocketHelper.GetInterface())
			{
				((*iter).second)->TcpSocketHelper.CreateInstance();
			}
			if (false == ((*iter).second)->TcpSocketHelper->SetTCPSocketSink(pIUnknownEx))
			{
				return false;
			}
		}
	}
	return true;
}


//初始化用户共享数据
void CGameUserInfo::InitGameUserShareData()
{
	::memset(&m_GameServerInfo, 0, sizeof(m_GameServerInfo));
}

//获得房间信息
const Local_GR_ServerInfo* CGameUserInfo::GetLocalRoomInfo()
{
	return &m_GameServerInfo;
}

//设置房间信息
void CGameUserInfo::SetLocalRoomInfo(WORD wGameGenre, WORD wKindID, WORD wTableCount, WORD wChairCount)
{
	m_GameServerInfo.bIsInit = true;
	m_GameServerInfo.wGameGenre = wGameGenre;	//游戏类型
	m_GameServerInfo.wKindID = wKindID;		//游戏ID
	m_GameServerInfo.wTableCount = wTableCount;	//桌子个数
	m_GameServerInfo.wChairCount = wChairCount;	//椅子个数
}




