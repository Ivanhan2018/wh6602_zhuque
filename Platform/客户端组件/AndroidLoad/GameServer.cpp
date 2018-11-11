// GameServer.cpp : 实现文件
//

#include "stdafx.h"
#include "FlashEnterDlg.h"
#include "GameServer.h"
#include "GlobalUnits.h"

//定时器
#define			IDI_TIMER_GAME_SERVER_LOOK				3021
#define			TIMER_GAME_SERVER_LOOK_INTERVAL			10000

//一次连接数
#define			GAME_CONNECT_ONCE_NUMS				100

// CGameServer

IMPLEMENT_DYNAMIC(CGameServer, CWnd)
CGameServer::CGameServer()
{
	m_pMainWnd = NULL;
	m_pGameUserInfo = NULL;
	::memset(&m_LogonInfo, 0, sizeof(m_LogonInfo));
	m_dwQueueSuccNums = 0;
	m_dwQueueFailNums = 0;
}

CGameServer::~CGameServer()
{
}


BEGIN_MESSAGE_MAP(CGameServer, CWnd)
	ON_WM_CREATE()
	ON_WM_TIMER()
END_MESSAGE_MAP()

//接口查询
void * __cdecl CGameServer::QueryInterface(const IID & Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IViewItem,Guid,dwQueryVer);
	QUERYINTERFACE(ITCPSocketSink,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IViewItem,Guid,dwQueryVer);
	return NULL;
}

//连接游戏服务器之前初始化数据
bool CGameServer::InitGameLocalData()
{
	KillTimer(IDI_TIMER_GAME_SERVER_LOOK);

	m_dwQueueSuccNums = 0;
	m_dwQueueFailNums = 0;
	::memset(&m_LogonInfo, 0, sizeof(m_LogonInfo));

	m_pGameUserInfo = CGameUserInfo::GetInstance();
	if (NULL == m_pGameUserInfo)
	{
		return false;
	}
	m_pGameUserInfo->InitGameUserShareData();
	return true;
}

//玩家宝石不够,不能登录游戏服务器
bool CGameServer::UserCanLogonGame(LONG lRestrictScore)
{
	if (lRestrictScore <=0)
	{
		return false;
	}
	if (g_GlobalUnits.m_GameOption.dwRoomID==0 || g_GlobalUnits.m_GameOption.dwRoomID>6)
	{
		return false;
	}
	DWORD dwGameServerListNums = g_GlobalUnits.m_GameKindList.size();
	if (dwGameServerListNums <= 0 || dwGameServerListNums>100)
	{
		return false;
	}

	for (int i=0; i<dwGameServerListNums; i++)
	{
		if (g_GlobalUnits.m_GameKindList[i].wSortID == g_GlobalUnits.m_GameOption.dwRoomID)
		{
			return (lRestrictScore>=g_GlobalUnits.m_GameKindList[i].lRestrictScore)?true:false;
		}
		
	}
	return false;
}

//开始分配服务器IP和端口号
bool CGameServer::StartAllotServerIPAndPort()
{
	if (m_pGameUserInfo->m_vecUserGameInfo.size()<=0)
	{
		return false;
	}
	DWORD dwCountConnect = 0;
	std::map<DWORD, strUserGameInfo*>::iterator iter = m_pGameUserInfo->m_vecUserGameInfo.begin();
	for (; iter!=m_pGameUserInfo->m_vecUserGameInfo.end(); ++iter)
	{
		if ((NULL!=(*iter).second) && (TAG_LINK_SUC == ((*iter).second)->dwLogonLinkState))
		{//登录服务器成功了.
			++dwCountConnect;
		}
	}
	DWORD dwGameServerListNums = g_GlobalUnits.m_GameKindList.size();
	if (dwGameServerListNums <= 0 || dwGameServerListNums>100)
	{
		return false;
	}
	//把登录成功的玩家平分至各个游戏服务器.
	DWORD dwOnceServerUserNums = dwCountConnect/dwGameServerListNums;
	if (dwOnceServerUserNums<=0 || dwOnceServerUserNums>dwCountConnect)
	{
		return false;
	}
	iter = m_pGameUserInfo->m_vecUserGameInfo.begin();
	for (int i=0; i<dwGameServerListNums; i++)
	{
		for (int j=0; j<dwOnceServerUserNums; j++)
		{
			for (; iter!=m_pGameUserInfo->m_vecUserGameInfo.end(); ++iter)
			{
				if ((NULL!=(*iter).second) && (TAG_LINK_SUC == ((*iter).second)->dwLogonLinkState))
				{//登录服务器成功了.
					DWORD aa = g_GlobalUnits.m_GameKindList[i].dwGameServerIP;
					DWORD bb = g_GlobalUnits.m_GameKindList[i].dwGameServerport;
					((*iter).second)->dwGameServerIP = g_GlobalUnits.m_GameKindList[i].dwGameServerIP;
					((*iter).second)->dwGameServerport = g_GlobalUnits.m_GameKindList[i].dwGameServerport;
					++iter;
					break;
				}
			}
		}
	}
	return true;
}

//连接到游戏服务器
void CGameServer::OnConnectGameServer()
{
	//初始化数据
	bool bRet = InitGameLocalData();
	if (!bRet)
	{
		MessageBox("连接至游戏服务器前初始化失败！");
		return;
	}
	
	//设置组件
	IUnknownEx * pIUnknownEx=QUERY_ME_INTERFACE(IUnknownEx);
	bRet = m_pGameUserInfo->SetIUnknownEx(2, pIUnknownEx);
	if (!bRet)
	{
		AfxMessageBox(TEXT("网络组件回调接口设置失败！"));
		return;
	}

	//开始分配服务器IP和端口号
	bool bIsSuc = StartAllotServerIPAndPort();
	if (!bIsSuc)
	{
		AfxMessageBox(TEXT("分配服务器IP和端口号失败！"));
		return;
	}

	if (m_pGameUserInfo->m_vecUserGameInfo.size()>0)
	{
		DWORD dwCountConnect = 0;
		std::map<DWORD, strUserGameInfo*>::iterator iter = m_pGameUserInfo->m_vecUserGameInfo.begin();
		for (; iter!=m_pGameUserInfo->m_vecUserGameInfo.end(); ++iter)
		{
			if ((NULL != (*iter).second)&&(TAG_LINK_SUC == ((*iter).second)->dwLogonLinkState && TAG_LINK_FAIL == ((*iter).second)->dwGameLinkState))
			{//登录成功了.
				if (0!=(*iter).second->dwUserID)
				{
					if (0!=(*iter).second->dwGameServerIP && 0!=(*iter).second->dwGameServerport)
					{
						if (UserCanLogonGame((*iter).second->llUserGems))
						{
							((*iter).second)->dwCountConnect = 0;
							((*iter).second)->TcpSocketHelper->SetTCPSocketSink(pIUnknownEx);
							((*iter).second)->TcpSocketHelper->Connect((*iter).second->dwGameServerIP,(*iter).second->dwGameServerport);
							((*iter).second)->dwGameLinkState = TAG_LINKING;
							((*iter).second)->dwUserGameState = USER_LOGON_SERVER_FAIL;
							++dwCountConnect;
							if (dwCountConnect>=GAME_CONNECT_ONCE_NUMS)
							{
								break;
							}
						}
						else
						{
							char chData[256]={0};
							sprintf(chData, "CGameServer::OnConnectGameServer->玩家[%s]服务器IP:%d,端口号:%d,金币(%d)不够！", \
								(((*iter).second)->chName), ((*iter).second)->dwGameServerIP, ((*iter).second)->dwGameServerport, ((*iter).second)->llUserGems);
							m_pMainWnd->InsertListData((*iter).second->dwSocketID, 0, chData);
						}
					}
				}
			}
		}
	}
	
	SetTimer(IDI_TIMER_GAME_SERVER_LOOK, TIMER_GAME_SERVER_LOOK_INTERVAL, NULL);
}


//连接事件
bool __cdecl CGameServer::OnEventTCPSocketLink(WORD wSocketID, INT nErrorCode)
{
	//连接成功.
	std::map<DWORD, strUserGameInfo*>::iterator iter = m_pGameUserInfo->m_vecUserGameInfo.find(wSocketID);
	if (iter != m_pGameUserInfo->m_vecUserGameInfo.end())
	{
		if (0==nErrorCode)
		{//连接成功.
			UserLogonToGameServer((*iter).second);
		}
		else if (nErrorCode!=0)
		{//连接失败.
			((*iter).second)->dwGameLinkState = TAG_LINK_FAIL;
			++(((*iter).second)->dwCountConnect);
			if (NULL != m_pMainWnd)
			{
				char chData[256]={0};
				sprintf(chData, "CGameServer::OnEventTCPSocketLink->玩家[%s]的TCP层与游戏服务器[IP:%d,PORT:%d]断开连接！", ((*iter).second)->chName, ((*iter).second)->dwGameServerIP, ((*iter).second)->dwGameServerport);
				m_pMainWnd->InsertListData(wSocketID, nErrorCode, chData);
			}
			return true;
		}
	}
	return true;
}

//登录至游戏服务器
void CGameServer::UserLogonToGameServer(strUserGameInfo* pUserInfo)
{
	if (NULL != pUserInfo)
	{
		if (UserCanLogonGame(pUserInfo->llUserGems))
		{
			//获取信息
			BYTE cbBuffer[SOCKET_PACKET] = {0};
			TCHAR szPassword[33] = {0};
			CMD5Encrypt::EncryptData(g_GlobalUnits.m_GameOption.chUserPassWord,szPassword);

			//登录数据包
			CMD_GR_LogonByUserID * pLogonByUserID=(CMD_GR_LogonByUserID *)cbBuffer;
			pLogonByUserID->dwUserID=pUserInfo->dwUserID;
			pLogonByUserID->dwPlazaVersion=g_GlobalUnits.GetPlazaVersion();
			pLogonByUserID->dwKindID = g_GlobalUnits.m_GameOption.dwGameID;
			lstrcpyn(pLogonByUserID->szPassWord,szPassword,sizeof(pLogonByUserID->szPassWord));

			//机器序列号
			tagClientSerial ClientSerial;
			g_GlobalUnits.GetClientSerial(ClientSerial);

			//发送数据包
			CSendPacketHelper Packet(cbBuffer+sizeof(CMD_GR_LogonByUserID),sizeof(cbBuffer)-sizeof(CMD_GR_LogonByUserID));
			Packet.AddPacket(&ClientSerial,sizeof(ClientSerial),DTP_COMPUTER_ID);
			pUserInfo->TcpSocketHelper->SendData(MDM_GR_LOGON,SUB_GR_LOGON_USERID,cbBuffer,sizeof(CMD_GR_LogonByUserID)+Packet.GetDataSize());
			pUserInfo->dwGameLinkState = TAG_LINK_SUC;
			pUserInfo->dwUserGameState = USER_LOGON_SERVER_LINKING;
		}
	}
}

//关闭事件
bool __cdecl CGameServer::OnEventTCPSocketShut(WORD wSocketID, BYTE cbShutReason)
{
	if (cbShutReason!=SHUT_REASON_NORMAL)
	{
		//关闭连接
		std::map<DWORD, strUserGameInfo*>::iterator iter = m_pGameUserInfo->m_vecUserGameInfo.find(wSocketID);
		if (iter!=m_pGameUserInfo->m_vecUserGameInfo.end())
		{
			if (NULL != ((*iter).second))
			{
				((*iter).second)->dwGameLinkState = TAG_LINK_FAIL;
				++(((*iter).second)->dwCountConnect);
				if (NULL != m_pMainWnd)
				{
					char chData[256]={0};
					sprintf(chData, "CGameServer::OnEventTCPSocketShut->玩家[%s]的网络Sock与游戏服务器[IP:%d,PORT:%d]断开连接！", ((*iter).second)->chName, ((*iter).second)->dwGameServerIP, ((*iter).second)->dwGameServerport);
					m_pMainWnd->InsertListData(wSocketID, cbShutReason, chData);
				}
			}
		}
	}

	return true;
}

//读取事件
bool __cdecl CGameServer::OnEventTCPSocketRead(WORD wSocketID, CMD_Command Command, VOID * pData, WORD wDataSize)
{
	bool bRet = true;
	switch (Command.wMainCmdID)
	{
	case MDM_GF_GAME:			//游戏消息
	case MDM_GF_FRAME:			//框架消息
		{
			bRet =  OnSocketMainGameFrame(wSocketID,Command,pData,wDataSize);
		}
		break;
	default:
		{
			std::map<DWORD, strUserGameInfo*>::iterator iter = m_pGameUserInfo->m_vecUserGameInfo.find(wSocketID);
			if (iter != m_pGameUserInfo->m_vecUserGameInfo.end())
			{
				switch (Command.wMainCmdID)
				{
				case MDM_GR_LOGON:			//登录消息
					{
						bRet =  OnSocketMainLogon(wSocketID,Command,pData,wDataSize);
						break;
					}
				case MDM_GR_USER:			//用户消息
					{
						bRet =  OnSocketMainUser(wSocketID,Command,pData,wDataSize);
						break;
					}
				case MDM_GR_INFO:			//配置信息
					{
						bRet =  OnSocketMainInfo(wSocketID,Command,pData,wDataSize);
						break;
					}
				case MDM_GR_STATUS:			//状态信息
					{
						//return true;
						//return OnSocketMainStatus(wSocketID,Command,pData,wDataSize);
						break;
					}
				case MDM_GR_SYSTEM:			//系统消息
					{
						//return true;
						//return OnSocketMainSystem(wSocketID,Command,pData,wDataSize);
						break;
					}
				case MDM_GR_SERVER_INFO:	//房间信息
					{
						//return true;
						//return OnSocketMainServerInfo(wSocketID,Command,pData,wDataSize);
						break;
					}
				default:
					break;
				}
			}
		}
		break;
	}

	if (!bRet)
	{
		if (NULL != m_pMainWnd)
		{
			char chData[256]={0};
			sprintf(chData, "消息主ID:%d,子消息ID:%d,数据%d,数据Size:%d, 消息可能不正确或者未被处理", Command.wMainCmdID, Command.wSubCmdID, pData, wDataSize);
			m_pMainWnd->InsertListData(wSocketID, 0, chData);
		}
	}

	return true;
}

//登录消息
bool CGameServer::OnSocketMainLogon(WORD wSocketID, CMD_Command Command, void * pBuffer, WORD wDataSize)
{
	strUserGameInfo* pUserGameInfo = m_pGameUserInfo->m_vecUserGameInfo[wSocketID];
	if (NULL == pUserGameInfo)
	{
		return true;
	}
	switch (Command.wSubCmdID)
	{
	case SUB_GR_LOGON_SUCCESS://登录成功
		{
			return true;
		}
	case SUB_GR_LOGON_ERROR://登录失败
	case SUB_GR_LOGON_FULL://人数已满
		{
			//效验参数
			//CMD_GR_LogonError * pLogonError=(CMD_GR_LogonError *)pBuffer;
			//ASSERT(wDataSize>=(sizeof(CMD_GR_LogonError)-sizeof(pLogonError->szErrorDescribe)));
			//if (wDataSize<(sizeof(CMD_GR_LogonError)-sizeof(pLogonError->szErrorDescribe))) return false;

			////显示消息
			//WORD wDescribeSize=wDataSize-(sizeof(CMD_GR_LogonError)-sizeof(pLogonError->szErrorDescribe));
			//if (wDescribeSize>0)
			//{
			//	pLogonError->szErrorDescribe[wDescribeSize-1]=0;
			//	AfxMessageBox(pLogonError->szErrorDescribe);
			//}

			//关闭连接
			if (NULL != pUserGameInfo)
			{
				if (NULL != m_pMainWnd)
				{
					char chData[128]={0};
					sprintf(chData, "玩家[%s]登录到游戏服务器失败！", pUserGameInfo->chName);
					m_pMainWnd->InsertListData(wSocketID, 0, chData);
				}
				pUserGameInfo->dwUserGameState = USER_LOGON_SERVER_FAIL;

				++m_LogonInfo.dwLogonFailNums;
				if (NULL != m_pMainWnd)
				{
					m_pMainWnd->UpdateGameInfo();
				}
			}
			return true;
		} 
	case SUB_GR_LOGON_FINISH://登录完成
		{
			if (TAG_LINK_SUC == pUserGameInfo->dwGameLinkState)
			{//TCP连接成功了.
				pUserGameInfo->dwUserGameState = USER_LOGON_SERVER_SUC;
				++m_LogonInfo.dwLogonSucNums;
				if (NULL != m_pMainWnd)
				{
					m_pMainWnd->UpdateGameInfo();
				}
			}
			//设置界面
			return true;
		}
	}
	return true;
}

//用户消息
bool CGameServer::OnSocketMainUser(WORD wSocketID, CMD_Command Command, void * pBuffer, WORD wDataSize)
{
	strUserGameInfo* pUserGameInfo = m_pGameUserInfo->m_vecUserGameInfo[wSocketID];
	if (NULL == pUserGameInfo)
	{
		return true;
	}
	switch (Command.wSubCmdID)
	{
	case SUB_GR_USER_COME:			//用户进入
		{
			return OnSocketSubUserCome(pUserGameInfo, pBuffer, wDataSize);
		}
	case SUB_GR_USER_STATUS:		//用户状态
		{
			return OnSocketSubStatus(pUserGameInfo, pBuffer, wDataSize);
		}
	case SUB_GR_USER_SCORE:			//用户分数
		{
			return OnSocketSubScore(pUserGameInfo, pBuffer, wDataSize);
		}
	case SUB_GR_USER_RIGHT:			//用户权限
		{
			return true;
		}
	case SUB_GR_MEMBER_ORDER:		//会员等级
		{
			return true;
		}
	case SUB_GR_SIT_FAILED:			//坐下失败
		{
			return true;
		}
	case SUB_GR_USER_CHAT:			//用户聊天
		{
			return true;
		}
	case SUB_GR_USER_WISPER:		//用户私语
		{
			return true;
		}
	case SUB_GR_USER_INVITE:		//邀请玩家
		{
			return true;
		}
		//排队成功      彭文添加   
	case SUB_GR_AUTO_QUEUE_SUCC:
	case SUB_GR_AUTO_QUEUE:			//自动排队
		{
			//消息处理
			CMD_GR_AutoQueue * pAutoQueue=(CMD_GR_AutoQueue *)pBuffer;
			if (NULL != pAutoQueue)
			{
				++m_dwQueueSuccNums;
				
				if (NULL != m_pMainWnd)
				{
					m_pMainWnd->UpdateQueueInfo();
				}
			}
			return true;
		}
	case SUB_GR_AUTO_QUEUE_FAIL:	//排队失败
		{
			++m_dwQueueFailNums;

			std::map<DWORD, strUserGameInfo*>::iterator iter = m_pGameUserInfo->m_vecUserGameInfo.find(wSocketID);
			if (iter != m_pGameUserInfo->m_vecUserGameInfo.end())
			{
				((*iter).second)->TcpSocketHelper->SendData(MDM_GR_USER,SUB_GR_AUTO_QUEUE);
			}

			if (NULL != m_pMainWnd)
			{
				m_pMainWnd->UpdateQueueInfo();
			}
			return true;
		}
	case SUB_GR_QUEUE_COUNT:	//排队人数
		{
			return true;
		}
	case SUB_RECVACTIONINFO:		//接受动作
		{
			return true;
		}
	case SUB_GR_NO_GOLD:		//金币不足，退回房间列表
		{
			pUserGameInfo->TcpSocketHelper->CloseSocket();
			pUserGameInfo->dwLogonLinkState = TAG_LINK_FAIL;
			pUserGameInfo->dwUserGameState = USER_LOGON_SERVER_FAIL;
			return true;
		}
	case SUB_GR_AUTO_QUEUE_CANCEL_FALL:		//取消排队失败
		{
			return true;
		}
	case SUB_GR_AUTO_QUEUE_CANCEL_SUCC:		//取消排队成功
		{
			return true;
		}
	case SUB_GF_QUIT_GAME_SERVER_SUCC:		//返回到房间列表
		{
			return true;
		}
	default:
		break;
	}

	if (NULL != m_pMainWnd)
	{
		m_pMainWnd->UpdateQueueInfo();
	}
	return true;
}

//用户进入
bool CGameServer::OnSocketSubUserCome(strUserGameInfo* pUserGameInfo, void * pBuffer, WORD wDataSize)
{
	if (NULL == pUserGameInfo)
	{
		return false;
	}

	//效验参数
	ASSERT(wDataSize>=sizeof(tagUserInfoHead));
	if (wDataSize<sizeof(tagUserInfoHead))
	{
		return false;
	}

	//读取基本信息
	tagUserData UserData;
	memset(&UserData,0,sizeof(UserData));
	tagUserInfoHead * pUserInfoHead=(tagUserInfoHead *)pBuffer;
	if (NULL == pUserInfoHead)
	{
		return false;
	}
	if (pUserGameInfo->dwUserID != pUserInfoHead->dwUserID)
	{//不是本人要接受的信息
		return true;
	}

	//读取信息
	UserData.dwUserID=pUserInfoHead->dwUserID;
	UserData.wTableID=pUserInfoHead->wTableID;
	UserData.wChairID=pUserInfoHead->wChairID;
	UserData.cbUserStatus=pUserInfoHead->cbUserStatus;
	UserData.dwUserRight=pUserInfoHead->dwUserRight;
	UserData.dwMasterRight=pUserInfoHead->dwMasterRight;
	//UserData.lInsureScore = pUserInfoHead->lInsureScore;

	//读取信息
	UserData.wFaceID=pUserInfoHead->wFaceID;
	UserData.dwCustomFaceVer=pUserInfoHead->dwCustomFaceVer;
	UserData.cbGender=pUserInfoHead->cbGender;
	UserData.cbMemberOrder=pUserInfoHead->cbMemberOrder;
	UserData.cbMasterOrder=pUserInfoHead->cbMasterOrder;
	UserData.dwGameID=pUserInfoHead->dwGameID;
	UserData.dwGroupID=pUserInfoHead->dwGroupID;
	UserData.lLoveliness=pUserInfoHead->lLoveliness;
	UserData.lMoney=pUserInfoHead->UserScoreInfo.lMoney;
	UserData.lGameGold=pUserInfoHead->UserScoreInfo.lGold;
	//UserData.lInsureScore=pUserInfoHead->UserScoreInfo.lInsureScore;
	UserData.lWinCount=pUserInfoHead->UserScoreInfo.lWinCount;
	UserData.lLostCount=pUserInfoHead->UserScoreInfo.lLostCount;
	UserData.lDrawCount=pUserInfoHead->UserScoreInfo.lDrawCount;
	UserData.lFleeCount=pUserInfoHead->UserScoreInfo.lFleeCount;
	UserData.lExperience=pUserInfoHead->UserScoreInfo.lExperience;

	UserData.dwGrade = pUserInfoHead->UserScoreInfo.dwGrade;
	//UserData.dwOrder = pUserInfoHead->UserScoreInfo.dwOrder;

	UserData.lGem = pUserInfoHead->UserScoreInfo.lGem;   //用户的宝石
	//UserData.lHonor = pUserInfoHead->UserScoreInfo.lHonor;  //用户的荣誉
	//UserData.lCredit = pUserInfoHead->UserScoreInfo.lCredit; //用户的声望

	//玩家进入
	pUserGameInfo->pIGameClient->OnEventUserEnter(&UserData);
	return true;
}


//用户状态
bool CGameServer::OnSocketSubStatus(strUserGameInfo* pUserGameInfo, void * pBuffer, WORD wDataSize)
{
	//效验参数
	ASSERT(wDataSize>=sizeof(CMD_GR_UserStatus));
	if (wDataSize<sizeof(CMD_GR_UserStatus))
	{
		return false;
	}

	//处理数据
	CMD_GR_UserStatus * pUserStatus=(CMD_GR_UserStatus *)pBuffer;
	if (NULL != pUserStatus)
	{
		if (pUserGameInfo->dwUserID != pUserStatus->dwUserID)
		{//不是本人要接受的信息
			return true;
		}

		BYTE cbNowStatus=pUserStatus->cbUserStatus;
		if (cbNowStatus<US_SIT)
		{//用户离开
			pUserGameInfo->pIGameClient->OnEventUserLeave();
		}
		else
		{//用户状态更改
			pUserGameInfo->pIGameClient->OnEventUserStatus(pUserStatus->wTableID, pUserStatus->wChairID, pUserStatus->cbUserStatus);
		}
	}

	return true;
}


//用户分数
bool CGameServer::OnSocketSubScore(strUserGameInfo* pUserGameInfo, void * pBuffer, WORD wDataSize)
{
	ASSERT(wDataSize>=sizeof(CMD_GR_UserScore));
	if (wDataSize<sizeof(CMD_GR_UserScore))
	{
		return false;
	}

	//处理数据
	CMD_GR_UserScore * pUserScore=(CMD_GR_UserScore *)pBuffer;
	if (NULL != pUserScore)
	{
		if (pUserGameInfo->dwUserID != pUserScore->dwUserID)
		{//不是本人要接受的信息
			return true;
		}

		IPC_UserScore UserScore;
		memset(&UserScore,0,sizeof(UserScore));
		UserScore.dwUserID=pUserScore->dwUserID;
		UserScore.UserScore=pUserScore->UserScore;
		UserScore.lLoveliness = pUserScore->lLoveliness;
		pUserGameInfo->pIGameClient->OnEventUserScore(&UserScore);
	}

	return true;
}


//配置消息
bool CGameServer::OnSocketMainInfo(WORD wSocketID, CMD_Command Command, void * pBuffer, WORD wDataSize)
{
	strUserGameInfo* pUserGameInfo = m_pGameUserInfo->m_vecUserGameInfo[wSocketID];
	if (NULL == pUserGameInfo)
	{
		return true;
	}
	ASSERT(Command.wMainCmdID==MDM_GR_INFO);
	switch (Command.wSubCmdID)
	{
	case SUB_GR_SERVER_INFO:	//房间信息
		{
			const Local_GR_ServerInfo* pLocalInfo = m_pGameUserInfo->GetLocalRoomInfo();
			if (pLocalInfo->bIsInit) 
			{//已初始化过了。
				return true;
			}

			//效验数据
			ASSERT(wDataSize>=sizeof(CMD_GR_ServerInfo));
			if (wDataSize<sizeof(CMD_GR_ServerInfo))
			{
				return false;
			}

			//消息处理
			CMD_GR_ServerInfo * pServerInfo=(CMD_GR_ServerInfo *)pBuffer;
			if (NULL != pServerInfo)
			{
				m_pGameUserInfo->SetLocalRoomInfo(pServerInfo->wGameGenre, pServerInfo->wKindID, pServerInfo->wTableCount, pServerInfo->wChairCount);
			}
			return true;
		}
	case SUB_GR_COLUMN_INFO:	//列表解释
		{
			return true;
		}
	case SUB_GR_CONFIG_FINISH:	//配置完成
		{
			return true;
		}
	}
	return true;
}

//状态消息
bool CGameServer::OnSocketMainStatus(WORD wSocketID, CMD_Command Command, void * pBuffer, WORD wDataSize)
{
	strUserGameInfo* pUserGameInfo = m_pGameUserInfo->m_vecUserGameInfo[wSocketID];
	if (NULL == pUserGameInfo)
	{
		return true;
	}
	ASSERT(Command.wMainCmdID==MDM_GR_STATUS);
	switch (Command.wSubCmdID)
	{
	case SUB_GR_TABLE_INFO:		//桌子信息
		{
			return true;
		}
	case SUB_GR_TABLE_STATUS:	//桌子状态
		{
			return true;
		}
	}
	return true;
}

//系统消息
bool CGameServer::OnSocketMainSystem(WORD wSocketID, CMD_Command Command, void * pBuffer, WORD wDataSize)
{
	strUserGameInfo* pUserGameInfo = m_pGameUserInfo->m_vecUserGameInfo[wSocketID];
	if (NULL == pUserGameInfo)
	{
		return true;
	}
	ASSERT(Command.wMainCmdID==MDM_GR_SYSTEM);
	switch (Command.wSubCmdID)
	{
	case SUB_GR_MESSAGE:		//系统消息
		{
			//效验参数
			CMD_GR_Message * pMessage=(CMD_GR_Message *)pBuffer;
			ASSERT(wDataSize>(sizeof(CMD_GR_Message)-sizeof(pMessage->szContent)));
			if (wDataSize<=(sizeof(CMD_GR_Message)-sizeof(pMessage->szContent)))
			{
				return false;
			}

			//消息处理
			WORD wHeadSize=sizeof(CMD_GR_Message)-sizeof(pMessage->szContent);
			ASSERT(wDataSize==(wHeadSize+pMessage->wMessageLength*sizeof(TCHAR)));
			if (wDataSize!=(wHeadSize+pMessage->wMessageLength*sizeof(TCHAR))) 
			{
				return false;
			}

			//消息内容
			pMessage->szContent[pMessage->wMessageLength-1]=0;

			bool bIntermet=false;
			if (pMessage->wMessageType&SMT_INTERMIT_LINE) 
			{//关闭连接
				bIntermet=true;
			}
			else if (pMessage->wMessageType&SMT_CLOSE_ROOM)
			{//关闭房间
				bIntermet=true;
			}

			if (bIntermet) 
			{
			}
			return true;
		}
	case SUB_GR_LOGON_AFFICHE:	//系统公告
		{
			return true;
		}
	}
	return true;
}

//房间消息
bool CGameServer::OnSocketMainServerInfo(WORD wSocketID, CMD_Command Command, void * pBuffer, WORD wDataSize)
{
	strUserGameInfo* pUserGameInfo = m_pGameUserInfo->m_vecUserGameInfo[wSocketID];
	if (NULL == pUserGameInfo)
	{
		return true;
	}
	ASSERT(Command.wMainCmdID==MDM_GR_SERVER_INFO);
	switch (Command.wSubCmdID)
	{
	case SUB_GR_ONLINE_COUNT_INFO:		//在线信息
		{
			return true;
		}
	}

	return true;
}

//游戏消息
bool CGameServer::OnSocketMainGameFrame(WORD wSocketID, CMD_Command Command, void * pBuffer, WORD wDataSize)
{
	strUserGameInfo* pUserGameInfo = m_pGameUserInfo->m_vecUserGameInfo[wSocketID];
	if (NULL == pUserGameInfo)
	{
		return true;
	}

	if (0==wDataSize)
	{//无数据
		pUserGameInfo->pIGameClient->OnSocketMainGameFrame(Command, NULL, 0);
	}
	else
	{
		pUserGameInfo->pIGameClient->OnSocketMainGameFrame(Command, pBuffer, wDataSize);
	}
	
	return true;
}



// CGameServer 消息处理程序


int CGameServer::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (__super::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码

	return 0;
}

void CGameServer::OnTimer(UINT nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (IDI_TIMER_GAME_SERVER_LOOK == nIDEvent)
	{
		m_LogonInfo.dwTCPConnectNums = 0;
		m_LogonInfo.dwUserConnectNums = 0;
		//int iCountConnect = 0;
		std::map<DWORD, strUserGameInfo*>::iterator iter = m_pGameUserInfo->m_vecUserGameInfo.begin();
		for (; iter!=m_pGameUserInfo->m_vecUserGameInfo.end(); ++iter)
		{
			if (NULL==(*iter).second) continue;
			if (((*iter).second)->dwCountConnect >= I_CONNECT_NUMS) continue;
			if (TAG_LINK_SUC == (*iter).second->dwLogonLinkState && TAG_LINK_FAIL==(*iter).second->dwGameLinkState)
			{//登录成功了，但游戏服务器还没有登录成功。
				if (0 != (*iter).second->dwUserID)
				{
					if (m_LogonInfo.dwTCPConnectNums>=GAME_CONNECT_ONCE_NUMS)
					{
						continue;
					}
					if (0!=(*iter).second->dwGameServerIP && 0!=(*iter).second->dwGameServerport)
					{
						if (UserCanLogonGame((*iter).second->llUserGems))
						{
							//设置组件
							if (NULL == ((*iter).second)->TcpSocketHelper.GetInterface())
							{
								((*iter).second)->TcpSocketHelper.CreateInstance();
							}
							IUnknownEx * pIUnknownEx=QUERY_ME_INTERFACE(IUnknownEx);
							((*iter).second)->TcpSocketHelper->SetTCPSocketSink(pIUnknownEx);
							((*iter).second)->TcpSocketHelper->Connect((*iter).second->dwGameServerIP,(*iter).second->dwGameServerport);
							(*iter).second->dwGameLinkState = TAG_LINKING;
							++m_LogonInfo.dwTCPConnectNums;
							//++iCountConnect;
						}
					}
				}
			}
			else if (TAG_LINK_SUC==(*iter).second->dwGameLinkState)
			{//TCP已登录至游戏服务器
				if (USER_LOGON_SERVER_FAIL==(*iter).second->dwUserGameState)
				{//用户登录失败.
					if (m_LogonInfo.dwUserConnectNums>=GAME_CONNECT_ONCE_NUMS)
					{
						continue;
					}
					UserLogonToGameServer((*iter).second);
					++m_LogonInfo.dwUserConnectNums;
					//++iCountConnect;
				}
			}
			if (m_LogonInfo.dwTCPConnectNums>=GAME_CONNECT_ONCE_NUMS || m_LogonInfo.dwUserConnectNums>=GAME_CONNECT_ONCE_NUMS)
			{
				break;
			}
		}
		m_LogonInfo.dwLogonFailNums -= m_LogonInfo.dwUserConnectNums;
		if (NULL != m_pMainWnd)
		{
			m_pMainWnd->UpdateGameConnInfo();
		}
		//if (0 == iCountConnect)
		//{
		//	KillTimer(IDI_TIMER_GAME_SERVER_LOOK);
		//}
	}
	__super::OnTimer(nIDEvent);
}

//设置主窗口指针
void CGameServer::SetMainWnd(CFlashEnterDlg* pMainWnd)
{
	if (NULL != pMainWnd)
	{
		m_pMainWnd = pMainWnd;
	}
}

//返回相应值
void CGameServer::GetLogonInfo(DWORD &dwLogonSucNums, DWORD &dwLogonFailNums, DWORD &dwTCPCoonectNums, DWORD &dwUserConnectNums)
{
	dwLogonSucNums = m_LogonInfo.dwLogonSucNums;
	dwLogonFailNums = m_LogonInfo.dwLogonFailNums;
	dwTCPCoonectNums = m_LogonInfo.dwTCPConnectNums;
	dwUserConnectNums = m_LogonInfo.dwUserConnectNums;
}

void CGameServer::GetQueueInfo(DWORD &dwQueueSucNums, DWORD &dwQueueFailNums)
{
	dwQueueSucNums = m_dwQueueSuccNums;
	dwQueueFailNums = m_dwQueueFailNums;
}

//所有玩家排队
void CGameServer::SetAllUserQueue()
{
	m_dwQueueFailNums = 0;
	m_dwQueueSuccNums = 0;
	std::map<DWORD, strUserGameInfo*>::iterator iter = m_pGameUserInfo->m_vecUserGameInfo.begin();
	for (; iter!=m_pGameUserInfo->m_vecUserGameInfo.end(); ++iter)
	{
		if (TAG_LINK_SUC == (*iter).second->dwGameLinkState && USER_LOGON_SERVER_SUC==(*iter).second->dwUserGameState)
		{
			((*iter).second)->TcpSocketHelper->SendData(MDM_GR_USER,SUB_GR_AUTO_QUEUE);
		}
	}
	if (NULL != m_pMainWnd)
	{
		m_pMainWnd->UpdateQueueInfo();
	}
}

//退出游戏服务器
bool CGameServer::OnEventExitServer()
{
	KillTimer(IDI_TIMER_GAME_SERVER_LOOK);
	m_pGameUserInfo->CloseAllSocket();
	::memset(&m_LogonInfo, 0, sizeof(m_LogonInfo));
	m_dwQueueSuccNums = 0;
	m_dwQueueFailNums = 0;
	if (NULL != m_pMainWnd)
	{
		m_pMainWnd->UpdateGameInfo();
		m_pMainWnd->UpdateGameConnInfo();
	}
	return true;
}