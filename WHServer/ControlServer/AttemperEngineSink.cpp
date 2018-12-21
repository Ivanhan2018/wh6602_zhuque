#include "StdAfx.h"
#include "AttemperEngineSink.h"
#include "Zip.h"
#include "ControlServer.h"


#include "biostream.h"

//声明变量
extern CControlServerApp theApp;
//////////////////////////////////////////////////////////////////////////

//宏定义
#define TIME_RECONNECT						15000L						//重连时间
#define TIME_UPDATE_LIST					30000L						//更新时间
#define TIME_DOWNLOAD_CUSTOM_FACE			200L						//下载头像
#define TIME_CHECK_GM_TASK					120000L						//检查GM任务

//定时器 ID
#define IDI_CONNECT_CENTER_SERVER			1							//连接服务器
#define IDI_UPDATE_SERVER_LIST				2							//更新列表
#define IDI_DOWNLOAD_CUSTOM_FACE			3							//下载头像
#define IDI_CHECK_GM_TASK					4							//检查GM任务

//////////////////////////////////////////////////////////////////////////

#define SEND_USER_DATA(msg)		do{char cbBuffer[SOCKET_PACKET];\
	bostream bos;\
	bos.attach(cbBuffer, SOCKET_PACKET);\
	bos<<msg;\
	if(m_pITCPNetworkEngine)m_pITCPNetworkEngine->SendData(dwContextID, msg.M_ID, msg.S_ID, cbBuffer, bos.length());}while(0)


//输出信息
void TraceMessage(LPCTSTR pszMessage)
{
	if(0 == lstrlen(pszMessage))
	{
		return ;
	}
	
	SYSTEMTIME sys; 
	GetLocalTime( &sys ); 
	char buf[128];
	memset(buf, 0, 128);
	sprintf(buf, "%4d/%02d/%02d %02d:%02d:%02d.%03d ",sys.wYear,sys.wMonth,sys.wDay,sys.wHour,sys.wMinute, sys.wSecond,sys.wMilliseconds);

	//按天输出日志
	char filename[128];
	memset(filename, 0, 128);
	sprintf(filename, "TraceData_Control_%4d%02d%02d.txt",sys.wYear,sys.wMonth,sys.wDay);

	CFile File;
	if ((File.Open(filename,CFile::modeWrite)==FALSE)&&
		(File.Open(filename,CFile::modeWrite|CFile::modeCreate)==FALSE))
	{
		ASSERT(FALSE);
		return;
	}

	File.SeekToEnd();
	File.Write(buf,lstrlen(buf));
	File.Write(pszMessage,lstrlen(pszMessage));
	File.Write(TEXT("\r\n"),lstrlen(TEXT("\r\n")));

	File.Flush();
	File.Close();

	return;
}

//构造函数
CAttemperEngineSink::CAttemperEngineSink()
{
	//设置变量
	m_pInitParamter=NULL;
	m_pBindParameter=NULL;

	//设置变量
	m_pITimerEngine=NULL;
	m_pIDataBaseEngine=NULL;
//	m_pITCPNetworkEngine=NULL;
	m_pITCPSocketCorrespond=NULL;

	return;
}

//析构函数
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
	//设置组件
//	m_ServerList.SetSocketEngine(m_pITCPNetworkEngine);

	//绑定参数
	m_pBindParameter=new tagBindParameter[m_pInitParamter->m_wMaxConnect];
	ZeroMemory(m_pBindParameter,sizeof(tagBindParameter)*m_pInitParamter->m_wMaxConnect);

	//中心连接
	DWORD dwServerIP=inet_addr(m_pInitParamter->m_szCenterServerAddr);
	m_pITCPSocketCorrespond->Connect(htonl(dwServerIP),m_pInitParamter->m_wCenterServerPort);

	//初始数据
	//m_DownloadFaceInfoMap.InitHashTable(503);
	//m_DownloadFaceInfoMap.RemoveAll();

	//设置定时器
	//m_pITimerEngine->SetTimer(IDI_DOWNLOAD_CUSTOM_FACE,TIME_DOWNLOAD_CUSTOM_FACE,TIMES_INFINITY,NULL);

	return true;
}

//调度模块关闭
bool __cdecl CAttemperEngineSink::OnAttemperEngineStop(IUnknownEx * pIUnknownEx)
{
	//设置变量
	m_pITimerEngine=NULL;
	m_pIDataBaseEngine=NULL;
//	m_pITCPNetworkEngine=NULL;
	m_pITCPSocketCorrespond=NULL;

	//删除数据
	SafeDeleteArray(m_pBindParameter);

	return true;
}

//定时器事件
bool __cdecl CAttemperEngineSink::OnEventTimer(DWORD dwTimerID, WPARAM wBindParam)
{
	switch (dwTimerID)
	{
	case IDI_CONNECT_CENTER_SERVER:		//连接中心服务器
		{
			//发起连接
			DWORD dwServerIP=inet_addr(m_pInitParamter->m_szCenterServerAddr);
			m_pITCPSocketCorrespond->Connect(htonl(dwServerIP),m_pInitParamter->m_wCenterServerPort);

			//错误提示
			CTraceService::TraceString(TEXT("正在尝试重新连接中心服务器...."),TraceLevel_Normal);

			return true;
		}
	case IDI_UPDATE_SERVER_LIST:		//更新服务器列表
		{
			m_pITCPSocketCorrespond->SendData(MDM_CS_SERVER_LIST,SUB_CS_GET_SERVER_LIST,NULL,0);

			return true;
		}
	case IDI_CHECK_GM_TASK:		//检查GM任务
		{
			//投递数据库
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_CHECK_GM_TASK,0,NULL,0);

			//排行榜
			SYSTEMTIME sys; 
			GetLocalTime( &sys ); 

			static SYSTEMTIME lastTime;  //上一次生成的时间
			
			//每天3点
			if (sys.wHour == 3 
				&& lastTime.wDay != sys.wDay )
			{
				//投递数据库
				DBR_GR_Online online;
				online.dwTypeID = 16;   //今天最高在线
				online.dwValue = m_iMaxOnLineToday;
				lstrcpy(online.szCollectDate, m_strCollectDateToday.c_str());
				m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_WRITE_ONLINE,0,&online,sizeof(online));

				//三人拱今天最高在线
				if (m_mapMaxToday.find(998) != m_mapMaxToday.end() && m_mapMaxToday[998].first > 0)
				{
					online.dwTypeID = 17;   //今天最高在线
					online.dwValue = m_mapMaxToday[998].first;
					lstrcpy(online.szCollectDate, m_mapMaxToday[998].second.c_str());
					m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_WRITE_ONLINE,0,&online,sizeof(online));
				}
				//四人拱今天最高在线
				if (m_mapMaxToday.find(997) != m_mapMaxToday.end() && m_mapMaxToday[997].first > 0)
				{
					online.dwTypeID = 18;   //今天最高在线
					online.dwValue = m_mapMaxToday[997].first;
					lstrcpy(online.szCollectDate, m_mapMaxToday[997].second.c_str());
					m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_WRITE_ONLINE,0,&online,sizeof(online));
				}
				//鄂州五十K今天最高在线
				if (m_mapMaxToday.find(995) != m_mapMaxToday.end() && m_mapMaxToday[995].first > 0)
				{
					online.dwTypeID = 22;   //今天最高在线
					online.dwValue = m_mapMaxToday[995].first;
					lstrcpy(online.szCollectDate, m_mapMaxToday[995].second.c_str());
					m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_WRITE_ONLINE,0,&online,sizeof(online));
				}
				//黄石五十K今天最高在线
				if (m_mapMaxToday.find(996) != m_mapMaxToday.end() && m_mapMaxToday[996].first > 0)
				{
					online.dwTypeID = 25;   //今天最高在线
					online.dwValue = m_mapMaxToday[996].first;
					lstrcpy(online.szCollectDate, m_mapMaxToday[996].second.c_str());
					m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_WRITE_ONLINE,0,&online,sizeof(online));
				}
				//潜江千分今天最高在线
				if (m_mapMaxToday.find(990) != m_mapMaxToday.end() && m_mapMaxToday[990].first > 0)
				{
					online.dwTypeID = 28;   //今天最高在线
					online.dwValue = m_mapMaxToday[990].first;
					lstrcpy(online.szCollectDate, m_mapMaxToday[990].second.c_str());
					m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_WRITE_ONLINE,0,&online,sizeof(online));
				}
				//赤壁今天最高在线
				if (m_mapMaxToday.find(999) != m_mapMaxToday.end() && m_mapMaxToday[999].first > 0)
				{
					online.dwTypeID = 31;   //今天最高在线
					online.dwValue = m_mapMaxToday[999].first;
					lstrcpy(online.szCollectDate, m_mapMaxToday[999].second.c_str());
					m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_WRITE_ONLINE,0,&online,sizeof(online));
				}
				//监利开机今天最高在线
				if (m_mapMaxToday.find(988) != m_mapMaxToday.end() && m_mapMaxToday[988].first > 0)
				{
					online.dwTypeID = 34;   //今天最高在线
					online.dwValue = m_mapMaxToday[988].first;
					lstrcpy(online.szCollectDate, m_mapMaxToday[988].second.c_str());
					m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_WRITE_ONLINE,0,&online,sizeof(online));
				}
				//崇阳今天最高在线
				if (m_mapMaxToday.find(987) != m_mapMaxToday.end() && m_mapMaxToday[987].first > 0)
				{
					online.dwTypeID = 37;   //今天最高在线
					online.dwValue = m_mapMaxToday[987].first;
					lstrcpy(online.szCollectDate, m_mapMaxToday[987].second.c_str());
					m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_WRITE_ONLINE,0,&online,sizeof(online));
				}
				//通山今天最高在线
				if (m_mapMaxToday.find(986) != m_mapMaxToday.end() && m_mapMaxToday[986].first > 0)
				{
					online.dwTypeID = 40;   //今天最高在线
					online.dwValue = m_mapMaxToday[986].first;
					lstrcpy(online.szCollectDate, m_mapMaxToday[986].second.c_str());
					m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_WRITE_ONLINE,0,&online,sizeof(online));
				}
				//仙桃今天最高在线
				if (m_mapMaxToday.find(989) != m_mapMaxToday.end() && m_mapMaxToday[989].first > 0)
				{
					online.dwTypeID = 43;   //今天最高在线
					online.dwValue = m_mapMaxToday[989].first;
					lstrcpy(online.szCollectDate, m_mapMaxToday[989].second.c_str());
					m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_WRITE_ONLINE,0,&online,sizeof(online));
				}
				//牛牛今天最高在线
				if (m_mapMaxToday.find(994) != m_mapMaxToday.end() && m_mapMaxToday[994].first > 0)
				{
					online.dwTypeID = 19;   //今天最高在线
					online.dwValue = m_mapMaxToday[994].first;
					lstrcpy(online.szCollectDate, m_mapMaxToday[994].second.c_str());
					m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_WRITE_ONLINE,0,&online,sizeof(online));
				}
				//三张今天最高在线
				if (m_mapMaxToday.find(961) != m_mapMaxToday.end() && m_mapMaxToday[961].first > 0)
				{
					online.dwTypeID = 60;   //今天最高在线
					online.dwValue = m_mapMaxToday[961].first;
					lstrcpy(online.szCollectDate, m_mapMaxToday[961].second.c_str());
					m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_WRITE_ONLINE,0,&online,sizeof(online));
				}

				//清掉今天的最高在线
				m_mapMaxToday.clear();
				m_iMaxOnLineToday = 0;
				m_strCollectDateToday.assign("");

				//投递数据库
				m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_CREATE_RANKING,0,NULL,0);
				lastTime = sys;
			}

			return true;
		}
	}

	return false;
}

//数据库事件
bool __cdecl CAttemperEngineSink::OnEventDataBase(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	switch (wRequestID)
	{
		case DBR_GP_CHECK_SPEAKER_TASK_RESULT:			//系统喇叭
		{
			return OnDBCheckSpeakerTaskResult(dwContextID,pData,wDataSize);
		}
	}

	return false;
}


//连接事件
bool __cdecl CAttemperEngineSink::OnEventTCPSocketLink(WORD wServiceID, INT nErrorCode)
{
	//错误判断
	if (nErrorCode!=0)
	{
		//设置时间
		m_pITimerEngine->SetTimer(IDI_CONNECT_CENTER_SERVER,TIME_RECONNECT,1,NULL);

		//错误提示
		CTraceService::TraceString(TEXT("中心服务器连接失败，稍后将会再次尝试...."),TraceLevel_Warning);

		return true;
	}
	else
	{
		//错误提示
		CTraceService::TraceString(TEXT("中心服务器连接成功"),TraceLevel_Normal);
	}

	//获取列表
	m_pITCPSocketCorrespond->SendData(MDM_CS_SERVER_LIST,SUB_CS_GET_SERVER_LIST,NULL,0);

	//设置定时器
	m_pITimerEngine->SetTimer(IDI_UPDATE_SERVER_LIST,TIME_UPDATE_LIST,TIMES_INFINITY,NULL);
	m_pITimerEngine->SetTimer(IDI_CHECK_GM_TASK,TIME_CHECK_GM_TASK,TIMES_INFINITY,NULL);

	return true;
}

//关闭事件
bool __cdecl CAttemperEngineSink::OnEventTCPSocketShut(WORD wServiceID, BYTE cbShutReason)
{
	//重新连接
	m_pITimerEngine->KillTimer(IDI_UPDATE_SERVER_LIST);
	m_pITimerEngine->SetTimer(IDI_CONNECT_CENTER_SERVER,TIME_RECONNECT,1,NULL);

	//错误提示
	CTraceService::TraceString(TEXT("中心服务器连接中断，稍后将会再次尝试...."),TraceLevel_Warning);

	return true;
}

//读取事件
bool __cdecl CAttemperEngineSink::OnEventTCPSocketRead(WORD wServiceID, CMD_Command Command, VOID * pData, WORD wDataSize)
{
	switch (Command.wMainCmdID)
	{
	case MDM_CS_SERVER_LIST:	//列表消息
		{
			return OnCenterMainServerList(Command.wSubCmdID,pData,wDataSize);
		}
	case MDM_CS_CONTROL_COMMAND:  //控制命令 
		{
			return OnCenterMainControlCommand(Command,pData,wDataSize);
		}
	}

	return true;
}

//网络应答事件
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



//网络读取事件
bool __cdecl CAttemperEngineSink::OnEventTCPNetworkRead(CMD_Command Command, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch (Command.wMainCmdID)
	{
	case MDM_GP_USER:				//用户信息
		{
			return OnSocketMainUser(Command.wSubCmdID,pData,wDataSize,dwSocketID);
		}
	}

	return false;
}

//网络关闭事件
bool __cdecl CAttemperEngineSink::OnEventTCPNetworkShut(DWORD dwClientIP, DWORD dwActiveTime, DWORD dwSocketID)
{
	//清除信息
	WORD wBindIndex=LOWORD(dwSocketID);
	ZeroMemory((m_pBindParameter+wBindIndex),sizeof(tagBindParameter));

	return true;
}

//用户信息处理
bool CAttemperEngineSink::OnSocketMainUser(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch (wSubCmdID)
	{
	default:
		{
			ASSERT(FALSE);
			return false;
		}
	}

	return true;
}



//列表消息处理
bool CAttemperEngineSink::OnCenterMainServerList(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_CS_LIST_INFO:		//列表信息
		{
			//效验参数
			ASSERT(wDataSize==sizeof(CMD_CS_ListInfo));
			if (wDataSize!=sizeof(CMD_CS_ListInfo)) return false;

			//消息处理
			m_ServerList.ResetServerListBuffer();

			return true;
		}
	case SUB_CS_LIST_TYPE:		//种类信息
		{
			//效验参数
			ASSERT(wDataSize>=sizeof(tagGameType));
			ASSERT(wDataSize%sizeof(tagGameType)==0);

			//消息处理
			DWORD dwCount=wDataSize/sizeof(tagGameType);
			tagGameType * pGameType=(tagGameType *)pData;
			m_ServerList.AppendGameTypeBuffer(pGameType,dwCount);

			return true;
		}
	case SUB_CS_LIST_KIND:		//类型信息
		{
			//效验参数
			ASSERT(wDataSize>=sizeof(tagGameKind));
			ASSERT(wDataSize%sizeof(tagGameKind)==0);

			//消息处理
			DWORD dwCount=wDataSize/sizeof(tagGameKind);
			tagGameKind * pGameKind=(tagGameKind *)pData;
			m_ServerList.AppendGameKindBuffer(pGameKind,dwCount);

			return true;
		}
	case SUB_CS_LIST_STATION:	//站点信息
		{
			//效验参数
			ASSERT(wDataSize>=sizeof(tagGameStation));
			ASSERT(wDataSize%sizeof(tagGameStation)==0);

			//消息处理
			DWORD dwCount=wDataSize/sizeof(tagGameStation);
			tagGameStation * pGameStation=(tagGameStation *)pData;
			m_ServerList.AppendGameStationBuffer(pGameStation,dwCount);

			return true;
		}
	case SUB_CS_LIST_SERVER:	//房间信息
		{
			//效验参数
			ASSERT(wDataSize>=sizeof(tagGameServer));
			ASSERT(wDataSize%sizeof(tagGameServer)==0);

			//消息处理
			DWORD dwCount=wDataSize/sizeof(tagGameServer);
			tagGameServer * pGameServer=(tagGameServer *)pData;
			m_ServerList.AppendGameServerBuffer(pGameServer,dwCount);

			return true;
		}
	case SUB_CS_LIST_FINISH:	//更新完成
		{
			//消息处理
			m_ServerList.ActiveServerListBuffer();

			//写日志,因为人数有更新
			static unsigned int iTotalMax = 0;
			unsigned int iTotal = 0;
			std::map<DWORD, DWORD> mapKindTotal;  //按kindid分类求和
			DWORD dwServerCount=(DWORD)m_ServerList.m_ServerListInfoActive.m_GameServerPtrArrayActive.GetCount();
			for (DWORD i=0;i<dwServerCount;i++)
			{
				tagGameServer * pGameServer=m_ServerList.m_ServerListInfoActive.m_GameServerPtrArrayActive[i];

				mapKindTotal[pGameServer->wKindID] += pGameServer->dwOnLineCount;  //求和

				bool bNewMax = false;
				if (m_mapMax.find(pGameServer->wServerID) != m_mapMax.end()
					&& m_mapMax[pGameServer->wServerID] >= pGameServer->dwOnLineCount)
				{
					//nothing
				}
				else
				{
					bNewMax = true;
					m_mapMax[pGameServer->wServerID] = pGameServer->dwOnLineCount;
				}

				iTotal += pGameServer->dwOnLineCount;

				char buf[128];
				memset(buf, 0, 128);
				sprintf(buf, "房间号=%u 人数=%u 最高峰=%u", pGameServer->wServerID, pGameServer->dwOnLineCount, 
						m_mapMax[pGameServer->wServerID]);
				if (bNewMax)
				{
					CTraceService::TraceString(buf,TraceLevel_Normal);
				}
				TraceMessage(buf);
			}
			
			if (iTotal > iTotalMax)iTotalMax = iTotal;

			char buf2[128];
			memset(buf2, 0, 128);
			sprintf(buf2, "总人数=%u 最高峰=%u", iTotal, iTotalMax);
			TraceMessage(buf2);

			if (iTotalMax==iTotal && iTotalMax!=0)
			{
				CTraceService::TraceString(buf2,TraceLevel_Normal);
			}

			////统计今天最高在线:总在线，各游戏在线
			//当前时间
			SYSTEMTIME sys; 
			GetLocalTime( &sys ); 
			char szNow[32];
			memset(szNow, 0, 32);
			sprintf(szNow, "%04d-%02d-%02d %02d:%02d:%02d", sys.wYear, sys.wMonth, sys.wDay, sys.wHour, sys.wMinute, sys.wSecond);

			if (iTotal > m_iMaxOnLineToday)
			{
				m_iMaxOnLineToday = iTotal;
				m_strCollectDateToday = szNow;
			}
			char buf[128];
			memset(buf, 0, 128);
			sprintf(buf, "今天总最高在线,人数=%u 时间=%s", 
						m_iMaxOnLineToday,
						m_strCollectDateToday.c_str());
			TraceMessage(buf);

			//检查各游戏是否有超人数
			for (std::map<DWORD, DWORD>::const_iterator cit = mapKindTotal.begin();
				cit != mapKindTotal.end(); 
				++cit)
			{
				if (cit->second > m_mapMaxToday[cit->first].first)
				{
					m_mapMaxToday[cit->first].first = cit->second;
					m_mapMaxToday[cit->first].second = szNow;
				}
			}
			//写日志
			for (std::map<DWORD, std::pair<DWORD, std::string>>::const_iterator cit = m_mapMaxToday.begin();
				cit != m_mapMaxToday.end(); 
				++cit)
			{
				char buf[128];
				memset(buf, 0, 128);
				sprintf(buf, "今天各游戏最高在线,kindid=%d 人数=%u 时间=%s", 
							cit->first,
							m_mapMaxToday[cit->first].first, 
							m_mapMaxToday[cit->first].second.c_str());
				
				TraceMessage(buf);
			}

			return true;
		}
	}

	return true;
}

//列表消息处理
bool CAttemperEngineSink::OnCenterMainControlCommand(CMD_Command Command, VOID * pData, WORD wDataSize)
{
	switch (Command.wSubCmdID)
	{
	case SUB_CS_CONTROL_COMMAND_REQ:		//请求命令
		{
			//直接忽略掉
			return true;
		}
	case SUB_CS_CONTROL_COMMAND_REP:		//响应命令
		{
			//效验参数
			ASSERT(wDataSize==sizeof(CMD_CS_ControlCommandRep));
			if (wDataSize!=sizeof(CMD_CS_ControlCommandRep)) return false;

			//消息处理
			CMD_CS_ControlCommandRep* rep = (CMD_CS_ControlCommandRep*)pData;
			if (rep == NULL)
			{
				CTraceService::TraceString("命令字回复消息体不正确 SUB_CS_CONTROL_COMMAND_REP", TraceLevel_Warning);
				return false;
			}
				
			if (rep->wCode == 0)
			{
				//成功 
				char buf[128];
				memset(buf, 0, 128);
				if (rep->wCommandID == 1)  //踢人 
				{
					sprintf(buf, "成功。房间号%u, 用户ID=%u 成功给玩家下发了踢出命令。 ", rep->dwServerID, rep->dwUserID);
				}
				else if (rep->wCommandID == 2)  //房间维护 
				{
					sprintf(buf, "成功。房间号%u 进入维护期。",rep->dwServerID); 
				}
				CTraceService::TraceString(buf, TraceLevel_Normal);
			}
			else 
			{
				//失败
				char buf[128];
				memset(buf, 0, 128);
				sprintf(buf, "命令处理失败。房间号%u 用户ID=%u 命令字%d",rep->dwServerID,rep->dwUserID,rep->wCommandID); 
				CTraceService::TraceString(buf, TraceLevel_Warning);
				memset(buf, 0, 128);
				sprintf(buf, "错误码%d 描述%s",rep->wCode, rep->szResult); 
				CTraceService::TraceString(buf, TraceLevel_Warning);
			}

			return true;
		}
		return true;
	}

	return true;
}


//发喇叭
bool CAttemperEngineSink::OnDBCheckSpeakerTaskResult(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//效验参数
	ASSERT(wDataSize==sizeof(DBR_GP_Speaker_Task_Result));
	if (wDataSize!=sizeof(DBR_GP_Speaker_Task_Result)) return false;
	
	DBR_GP_Speaker_Task_Result * pDBResult=(DBR_GP_Speaker_Task_Result *)pData;

	//将消息转给中心服务器，由中心服务器广播消息
	CMD_CS_SpeakerSendMsgReq csReq;
	memset(&csReq, 0, sizeof(csReq));
	csReq.nType = 2;   //系统喇叭
	csReq.nLen = pDBResult->nLen;
	memcpy(csReq.szMsg, pDBResult->szMsg, pDBResult->nLen);

	//转发
	if (m_pITCPSocketCorrespond)m_pITCPSocketCorrespond->SendData(MDM_CS_SERVER_MANAGER, SUB_CS_SPEAKER_SEND_MSG_REQ, &csReq, sizeof(csReq));

	//投递数据库请求，用于写日志
	DBR_GP_SpeakerSend dbReq;
	ZeroMemory(&dbReq, sizeof(dbReq));
	dbReq.iType = 2;   //系统喇叭
	dbReq.dwUserID = 0;   //系统用户
	dbReq.nLen = csReq.nLen;
	memcpy(dbReq.szDate, csReq.szMsg, dbReq.nLen);
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_SPEAKER_SEND, dwContextID, &dbReq, sizeof(dbReq));

	return true;
}

//////////////////////////////////////////////////////////////////////////
