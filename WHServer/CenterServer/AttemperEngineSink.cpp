#include "StdAfx.h"
#include "DataBaseSink.h"
#include "AttemperEngineSink.h"

//////////////////////////////////////////////////////////////////////////

CAttemperEngineSink::CAttemperEngineSink()
{
	//设置变量
	m_pInitParamter=NULL;
	m_pBindParameter=NULL;

	//接口变量
	m_pITimerEngine=NULL;
	m_pIDataBaseEngine=NULL;
	m_pITCPNetworkEngine=NULL;

	return;
}

CAttemperEngineSink::~CAttemperEngineSink()
{
}

//接口查询
void * __cdecl CAttemperEngineSink::QueryInterface(const IID & Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IAttemperEngineSink,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IAttemperEngineSink,Guid,dwQueryVer);
	return NULL;
}

//调度模块启动
bool __cdecl CAttemperEngineSink::OnAttemperEngineStart(IUnknownEx * pIUnknownEx)
{
	//绑定参数
	m_pBindParameter=new tagBindParameter[m_pInitParamter->m_wMaxConnect];
	ZeroMemory(m_pBindParameter,sizeof(tagBindParameter)*m_pInitParamter->m_wMaxConnect);

	//创建组件
	if ((m_ServerListCenter.GetInterface()==NULL)&&(m_ServerListCenter.CreateInstance()==false))
	{
		CTraceService::TraceString(TEXT("中心服务器列表组件创建失败"),TraceLevel_Exception);
		return false;
	}

	//加载列表
	tagDataBaseInfo DataBaseInfo;
	DataBaseInfo.wDataBasePort=m_pInitParamter->m_wServerDataBasePort;
	DataBaseInfo.dwDataBaseAddr=inet_addr(m_pInitParamter->m_szServerDataBaseAddr);
	lstrcpyn(DataBaseInfo.szDataBaseUser,m_pInitParamter->m_szServerDataBaseUser,CountArray(DataBaseInfo.szDataBaseUser));
	lstrcpyn(DataBaseInfo.szDataBasePass,m_pInitParamter->m_szServerDataBasePass,CountArray(DataBaseInfo.szDataBasePass));
	lstrcpyn(DataBaseInfo.szDataBaseName,m_pInitParamter->m_szServerDataBaseName,CountArray(DataBaseInfo.szDataBaseName));
	if (m_ServerListCenter->LoadServerList(DataBaseInfo)==false) 
	{
		CTraceService::TraceString(TEXT("游戏列表加载失败"),TraceLevel_Exception);
		return false;
	}

	return true;
}

//调度模块关闭
bool __cdecl CAttemperEngineSink::OnAttemperEngineStop(IUnknownEx * pIUnknownEx)
{
	//设置变量
	m_pITimerEngine=NULL;
	m_pIDataBaseEngine=NULL;
	m_pITCPNetworkEngine=NULL;

	//删除数据
	SafeDeleteArray(m_pBindParameter);

	//重置列表
	if (m_ServerListCenter.GetInterface()!=NULL)
	{
		m_ServerListCenter->ResetServerList();
	}

	return true;
}

//应答事件
bool __cdecl CAttemperEngineSink::OnEventTCPNetworkBind(DWORD dwClientIP, DWORD dwSocketID)
{
	//获取索引
	ASSERT(LOWORD(dwSocketID)<m_pInitParamter->m_wMaxConnect);
	if (LOWORD(dwSocketID)>=m_pInitParamter->m_wMaxConnect) return false;

	//变量定义
	WORD wBindIndex=LOWORD(dwSocketID);
	tagBindParameter * pBindParameter=(m_pBindParameter+wBindIndex);

	//设置变量
	pBindParameter->dwSocketID=dwSocketID;
	pBindParameter->dwClientIP=dwClientIP;
	pBindParameter->dwActiveTime=(DWORD)time(NULL);

	return true;
}

//关闭事件
bool __cdecl CAttemperEngineSink::OnEventTCPNetworkShut(DWORD dwClientIP, DWORD dwActiveTime, DWORD dwSocketID)
{
	//清除信息
	WORD wBindIndex=LOWORD(dwSocketID);
	ZeroMemory((m_pBindParameter+wBindIndex),sizeof(tagBindParameter));

	return true;
}

//读取事件
bool __cdecl CAttemperEngineSink::OnEventTCPNetworkRead(CMD_Command Command, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch (Command.wMainCmdID)
	{
	case MDM_CS_SERVER_LIST:		//列表消息
		{
			return OnSocketMainServerList(Command.wSubCmdID,pData,wDataSize,dwSocketID);
		}
	case MDM_CS_SERVER_MANAGER:		//房间管理
		{
			return OnSocketMainServerManager(Command.wSubCmdID,pData,wDataSize,dwSocketID);
		}
	case MDM_CS_CONTROL_COMMAND:	//控制命令，直接透传给所有游戏房间
		{
			return m_pITCPNetworkEngine->SendDataBatch(MDM_CS_CONTROL_COMMAND,Command.wSubCmdID,pData,wDataSize);
		}
	}

	return true;
}

//列表消息处理
bool CAttemperEngineSink::OnSocketMainServerList(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch (wSubCmdID)
	{
	case SUB_CS_GET_SERVER_LIST:	//获取列表
		{
			//变量定义
			CMD_CS_ListInfo ListInfo;
			ZeroMemory(&ListInfo,sizeof(ListInfo));

			//设置变量
			ListInfo.dwTypeCount=m_ServerListCenter->GetGameTypeCount();
			ListInfo.dwKindCount=m_ServerListCenter->GetGameKindCount();
			ListInfo.dwServerCount=m_ServerListCenter->GetGameServerCount();
			ListInfo.dwStationCount=m_ServerListCenter->GetGameStationCount();
			m_pITCPNetworkEngine->SendData(dwSocketID,MDM_CS_SERVER_LIST,SUB_CS_LIST_INFO,&ListInfo,sizeof(ListInfo));

			//类型信息
			POSITION Pos=NULL;
			WORD wSendSize=0L;
			BYTE cbBuffer[SOCKET_PACKET];
			for (DWORD i=0;i<ListInfo.dwTypeCount;i++)
			{
				if ((wSendSize+sizeof(tagGameType))>sizeof(cbBuffer))
				{
					m_pITCPNetworkEngine->SendData(dwSocketID,MDM_CS_SERVER_LIST,SUB_CS_LIST_TYPE,cbBuffer,wSendSize);
					wSendSize=0;
				}
				tagGameTypeItem * pGameTypeItem=m_ServerListCenter->EmunGameTypeItem(Pos);
				CopyMemory(cbBuffer+wSendSize,&pGameTypeItem->GameType,sizeof(tagGameType));
				wSendSize+=sizeof(tagGameType);
			}
			if (wSendSize>0) m_pITCPNetworkEngine->SendData(dwSocketID,MDM_CS_SERVER_LIST,SUB_CS_LIST_TYPE,cbBuffer,wSendSize);

			//种类信息
			Pos=NULL;
			wSendSize=0L;
			for (DWORD i=0;i<ListInfo.dwKindCount;i++)
			{
				if ((wSendSize+sizeof(tagGameKind))>sizeof(cbBuffer))
				{
					m_pITCPNetworkEngine->SendData(dwSocketID,MDM_CS_SERVER_LIST,SUB_CS_LIST_KIND,cbBuffer,wSendSize);
					wSendSize=0;
				}
				tagGameKindItem * pGameKindItem=m_ServerListCenter->EmunGameKindItem(Pos);
				CopyMemory(cbBuffer+wSendSize,&pGameKindItem->GameKind,sizeof(tagGameKind));
				wSendSize+=sizeof(tagGameKind);
			}
			if (wSendSize>0) m_pITCPNetworkEngine->SendData(dwSocketID,MDM_CS_SERVER_LIST,SUB_CS_LIST_KIND,cbBuffer,wSendSize);

			//站点信息
			Pos=NULL;
			wSendSize=0L;
			for (DWORD i=0;i<ListInfo.dwStationCount;i++)
			{
				if ((wSendSize+sizeof(tagGameStation))>sizeof(cbBuffer))
				{
					m_pITCPNetworkEngine->SendData(dwSocketID,MDM_CS_SERVER_LIST,SUB_CS_LIST_STATION,cbBuffer,wSendSize);
					wSendSize=0;
				}
				tagGameStationItem * pGameStationItem=m_ServerListCenter->EmunGameStationItem(Pos);
				CopyMemory(cbBuffer+wSendSize,&pGameStationItem->GameStation,sizeof(tagGameStation));
				wSendSize+=sizeof(tagGameStation);
			}
			if (wSendSize>0) m_pITCPNetworkEngine->SendData(dwSocketID,MDM_CS_SERVER_LIST,SUB_CS_LIST_STATION,cbBuffer,wSendSize);

			//房间信息
			Pos=NULL;
			wSendSize=0L;
			for (DWORD i=0;i<ListInfo.dwServerCount;i++)
			{
				if ((wSendSize+sizeof(tagGameServer))>sizeof(cbBuffer))
				{
					m_pITCPNetworkEngine->SendData(dwSocketID,MDM_CS_SERVER_LIST,SUB_CS_LIST_SERVER,cbBuffer,wSendSize);
					wSendSize=0;
				}
				tagGameServerItem * pGameServerItem=m_ServerListCenter->EmunGameServerItem(Pos);
				CopyMemory(cbBuffer+wSendSize,&pGameServerItem->GameServer,sizeof(tagGameServer));
				wSendSize+=sizeof(tagGameServer);
			}
			if (wSendSize>0) m_pITCPNetworkEngine->SendData(dwSocketID,MDM_CS_SERVER_LIST,SUB_CS_LIST_SERVER,cbBuffer,wSendSize);

			//发送完成
			m_pITCPNetworkEngine->SendData(dwSocketID,MDM_CS_SERVER_LIST,SUB_CS_LIST_FINISH);

			return true;	
		}
	}

	return false;
}

//房间消息处理
bool CAttemperEngineSink::OnSocketMainServerManager(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch (wSubCmdID)
	{
		case SUB_CS_REG_GAME_SERVER:		//房间注册
		{
			//效验参数
			ASSERT(wDataSize==sizeof(CMD_CS_RegGameServer));
			if (wDataSize!=sizeof(CMD_CS_RegGameServer)) return false;

			//消息处理
			CMD_CS_RegGameServer * pRegGameServer=(CMD_CS_RegGameServer *)pData;
			tagGameServer * pGameServer=&pRegGameServer->GameServer;
			tagGameServerItem * pGameServerItem=m_ServerListCenter->SearchGameServer(pGameServer->wKindID,pGameServer->wServerID);
			if (pGameServerItem!=NULL)
			{
				pGameServerItem->dwUpdateTime=(DWORD)time(NULL);
				CopyMemory(&pGameServerItem->GameServer,pGameServer,sizeof(tagGameServer));
			}
			else m_ServerListCenter->InsertGameServer(pGameServer);

			//更新映射关系
			m_mapServerID2SocketID[pGameServer->wServerID] = dwSocketID;

			return true;
		}
		case SUB_CS_UNREG_GAME_SERVER:		//注销房间
		{
			//效验参数
			ASSERT(wDataSize==sizeof(CMD_CS_UnRegGameServer));
			if (wDataSize!=sizeof(CMD_CS_UnRegGameServer)) return false;

			//消息处理
			CMD_CS_UnRegGameServer * pUnRegGameServer=(CMD_CS_UnRegGameServer *)pData;
			m_ServerListCenter->DeleteGameServer(pUnRegGameServer->wKindID,pUnRegGameServer->wServerID);

			//删除映射关系
			m_mapServerID2SocketID.erase(pUnRegGameServer->wServerID);

			return true;
		}
		case SUB_CS_SERVER_ONLINE_COUNT:	//更新人数
		{
			//效验参数
			ASSERT(wDataSize==sizeof(CMD_CS_ServerOnLineCount));
			if (wDataSize!=sizeof(CMD_CS_ServerOnLineCount)) return false;

			//消息处理
			CMD_CS_ServerOnLineCount * pServerOnLineCount=(CMD_CS_ServerOnLineCount *)pData;
			WORD wKindID=pServerOnLineCount->wKindID;
			WORD wServerID=pServerOnLineCount->wServerID;
			m_ServerListCenter->UpdateServerOnLineCount(wKindID,wServerID,pServerOnLineCount->dwOnLineCount);

			return true;
		}
		case SUB_CS_CREATE_SELF_REQ:	//请求创建自建场
		{
			//效验参数
			ASSERT(wDataSize==sizeof(CMD_CS_CreateSelf));
			if (wDataSize!=sizeof(CMD_CS_CreateSelf)) return false;

			//消息处理
			CMD_CS_CreateSelf * pReq=(CMD_CS_CreateSelf *)pData;

			//根据nServerID找到对应的服务器，转发消息命令
			if (pReq && m_mapServerID2SocketID.find(pReq->nServerID) != m_mapServerID2SocketID.end())
			{
				//转发给对应的游戏服务
				DWORD dwContextID = m_mapServerID2SocketID[pReq->nServerID];

				//判断在线
				if ((m_pBindParameter+LOWORD(dwContextID))->dwSocketID!=dwContextID) return true;

				//转发给指定的游戏服务器
				pReq->dwSocketIDLogin = dwSocketID;  //标识是哪个LoginServer来的
				m_pITCPNetworkEngine->SendData(dwContextID,MDM_CS_SERVER_MANAGER,SUB_CS_CREATE_SELF_REQ,pData,wDataSize);
			}

			return true;
		}
		case SUB_CS_QUERY_SELF_REQ:	//请求查询自建场
		{
			//效验参数
			ASSERT(wDataSize==sizeof(CMD_CS_QuerySelf));
			if (wDataSize!=sizeof(CMD_CS_QuerySelf)) return false;

			//消息处理
			CMD_CS_QuerySelf * pReq=(CMD_CS_QuerySelf *)pData;

			//根据nServerID找到对应的服务器，转发消息命令
			if (pReq && m_mapServerID2SocketID.find(pReq->nServerID) != m_mapServerID2SocketID.end())
			{
				//转发给对应的游戏服务
				DWORD dwContextID = m_mapServerID2SocketID[pReq->nServerID];

				//判断在线
				if ((m_pBindParameter+LOWORD(dwContextID))->dwSocketID!=dwContextID) return true;

				//转发给指定的游戏服务器
				pReq->dwSocketIDLogin = dwSocketID;  //标识是哪个LoginServer来的
				m_pITCPNetworkEngine->SendData(dwContextID,MDM_CS_SERVER_MANAGER,SUB_CS_QUERY_SELF_REQ,pData,wDataSize);
			}

			return true;
		}
		case SUB_CS_CHECK_PWD_SELF_REQ:	//请求自建场校验密码
		{
			//效验参数
			ASSERT(wDataSize==sizeof(CMD_CS_CheckPwdSelf));
			if (wDataSize!=sizeof(CMD_CS_CheckPwdSelf)) return false;

			//消息处理
			CMD_CS_CheckPwdSelf * pReq=(CMD_CS_CheckPwdSelf *)pData;

			//根据nServerID找到对应的服务器，转发消息命令
			if (pReq && m_mapServerID2SocketID.find(pReq->nServerID) != m_mapServerID2SocketID.end())
			{
				//转发给对应的游戏服务
				DWORD dwContextID = m_mapServerID2SocketID[pReq->nServerID];

				//判断在线
				if ((m_pBindParameter+LOWORD(dwContextID))->dwSocketID!=dwContextID) return true;

				//转发给指定的游戏服务器
				pReq->dwSocketIDLogin = dwSocketID;  //标识是哪个LoginServer来的
				m_pITCPNetworkEngine->SendData(dwContextID,MDM_CS_SERVER_MANAGER,SUB_CS_CHECK_PWD_SELF_REQ,pData,wDataSize);
			}

			return true;
		}
		case SUB_CS_CREATE_SELF_REP:	//创建自建场结果
		{
			//效验参数
			ASSERT(wDataSize==sizeof(CMD_CS_CreateSelfRep));
			if (wDataSize!=sizeof(CMD_CS_CreateSelfRep)) return false;

			//消息处理
			CMD_CS_CreateSelfRep * pReq=(CMD_CS_CreateSelfRep *)pData;

			//根据socketid找到对应的服务器，转发消息命令
			if (pReq)
			{
				//转发给对应的游戏服务
				DWORD dwContextID = pReq->dwSocketIDLogin;

				//判断在线
				if ((m_pBindParameter+LOWORD(dwContextID))->dwSocketID!=dwContextID) return true;

				//转发给指定的登录服务器
				pReq->dwSocketIDLogin = dwSocketID;  //标识是哪个LoginServer来的
				m_pITCPNetworkEngine->SendData(dwContextID,MDM_CS_SERVER_MANAGER,SUB_CS_CREATE_SELF_REP,pData,wDataSize);
			}

			return true;
		}
		case SUB_CS_CHECK_PWD_SELF_REP:	//校验密码结果
		{
			//效验参数
			ASSERT(wDataSize==sizeof(CMD_CS_CheckPwdSelfRep));
			if (wDataSize!=sizeof(CMD_CS_CheckPwdSelfRep)) return false;

			//消息处理
			CMD_CS_CheckPwdSelfRep * pReq=(CMD_CS_CheckPwdSelfRep *)pData;

			//根据socketid找到对应的服务器，转发消息命令
			if (pReq)
			{
				//转发给对应的游戏服务
				DWORD dwContextID = pReq->dwSocketIDLogin;

				//判断在线
				if ((m_pBindParameter+LOWORD(dwContextID))->dwSocketID!=dwContextID) return true;

				//转发给指定的登录服务器
				pReq->dwSocketIDLogin = dwSocketID;  //标识是哪个LoginServer来的
				m_pITCPNetworkEngine->SendData(dwContextID,MDM_CS_SERVER_MANAGER,SUB_CS_CHECK_PWD_SELF_REP,pData,wDataSize);
			}

			return true;
		}
		case SUB_CS_QUERY_SELF_REP:	//查询自建场结果
		{
			//效验参数
			ASSERT(wDataSize==sizeof(CMD_CS_QuerySelfRep));
			if (wDataSize!=sizeof(CMD_CS_QuerySelfRep)) return false;

			//消息处理
			CMD_CS_QuerySelfRep * pReq=(CMD_CS_QuerySelfRep *)pData;

			//根据socketid找到对应的服务器，转发消息命令
			if (pReq)
			{
				//转发给对应的游戏服务
				DWORD dwContextID = pReq->dwSocketIDLogin;

				//判断在线
				if ((m_pBindParameter+LOWORD(dwContextID))->dwSocketID!=dwContextID) return true;

				//转发给指定的登录服务器
				pReq->dwSocketIDLogin = dwSocketID;  //标识是哪个LoginServer来的
				m_pITCPNetworkEngine->SendData(dwContextID,MDM_CS_SERVER_MANAGER,SUB_CS_QUERY_SELF_REP,pData,wDataSize);
			}

			return true;
		}
		case SUB_CS_SPEAKER_SEND_MSG_REQ:	//请求发送喇叭
		{
			//效验参数
			ASSERT(wDataSize==sizeof(CMD_CS_SpeakerSendMsgReq));
			if (wDataSize!=sizeof(CMD_CS_SpeakerSendMsgReq)) return false;

			//将消息转发给所有连接到此中心服务器的所有服务
			for (int i=0; i<m_pInitParamter->m_wMaxConnect; i++)
			{
				if (m_pBindParameter[i].dwSocketID != 0)  
				{
					//连接是有效的
					m_pITCPNetworkEngine->SendData(m_pBindParameter[i].dwSocketID,MDM_CS_SERVER_MANAGER,SUB_CS_SPEAKER_SEND_MSG_REQ,pData,wDataSize);
					
					/* TODO:可能有部分房间不想收消息
					//连接是有效的。再判断是否为房间
					for (std::map<WORD, DWORD>::const_iterator cit = m_mapServerID2SocketID.begin();
						 cit != m_mapServerID2SocketID.end();
						 ++cit)
					{
						if (
						//此房间是否不允许收消息
					}
					*/
				}
			}
				
			return true;
		}
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////
