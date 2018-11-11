#include "StdAfx.h"
#include "TableFrame.h"
#include "DataBaseSink.h"
#include "AttemperEngineSink.h"
#include "CGlobalData.h"

#define  MAX_DELAY   (3)
#define  TWO_MIN     (60*2)

//发送分数
bool __cdecl CAttemperEngineSink::SendUserScore(IServerUserItem * pIServerUserItem)
{
	__ENTER_FUNCTION_LOG_CATCH

	tagServerUserData * pUserData=pIServerUserItem->GetUserData();

	//发送数据
	CMD_GR_UserScore UserScore;
	UserScore.dwUserID=pUserData->dwUserID;
	UserScore.UserScore=pUserData->UserScoreInfo;
	UserScore.lLoveliness = pUserData->lLoveliness;
	UserScore.UserScore.lGold   = pUserData->UserScoreInfo.lGold+pUserData->lStorageGold;

	m_pITCPNetworkEngine->SendDataBatch(MDM_GR_USER,SUB_GR_USER_SCORE,&UserScore,sizeof(UserScore));
	m_AndroidUserManager.SendDataToClient(MDM_GR_USER,SUB_GR_USER_SCORE,&UserScore,sizeof(UserScore));

	__LEAVE_FUNCTION_LOG_CATCH

	return true;
}

//////////////////////////////////////////////////////////////////////////
void Debug(char *text,...)
{
	static DWORD num=0;
	char buf[1024];
	FILE *fp=NULL;

	va_list ap;

	va_start(ap,text);
	vsprintf(buf,text,ap);
	va_end(ap);	

	if(num == 0)
	{
		fp=fopen("debug.log","w");
	}
	else
	{

		fp=fopen("debug.log","a");
	}

	if(fp == NULL)
		return ;

	num++;

	SYSTEMTIME time;

	GetLocalTime(&time);

	fprintf(fp, "%d:%s - %d/%d/%d %d:%d:%d \n", num, buf, time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond);

	fclose(fp);
} 

//构造函数
CAttemperEngineSink::CAttemperEngineSink()
{
	m_pRoomLog = NULL;		// 日志
	
	//游戏变量
	m_wMaxUserItem=0;
	m_pTableFrame=NULL;
	m_pNormalParameter=NULL;
	m_pAndroidParameter=NULL;
	m_UserAutoQueue.Clear();
	m_AndroidAutoQueue.Clear();

	//发送场所
	m_cbSendLocation = 0;

	//消息变量
	m_cbRoomMsgIndex=0;
	m_cbGameMsgIndex=0;

	//配置变量
	m_dwCenterServer=0L;
	m_pGameServiceAttrib=NULL;
	m_pGameServiceOption=NULL;

	//控制变量
	m_bShallClose=false;
	m_bAllowWisper=true;
	m_bAllowRoomChat=true;
	m_bAllowGameChat=true;
	m_bAllowEnterRoom=true;
	m_bAllowEnterGame=true;

	//辅助变量
	m_wColumnInfoSize=0;
	m_wOnLineInfoSize=0;
	memset(&m_ColumnInfo,0,sizeof(m_ColumnInfo));
	memset(&m_OnLineCountInfo,0,sizeof(m_OnLineCountInfo));

	//游戏组件
	m_hDllInstance=NULL;
	m_pIGameServiceManager=NULL;

	//接口变量
	m_pITimerEngine=NULL;
	m_pIAttemperEngine=NULL;
	m_pIDataBaseEngine=NULL;
	m_pITCPNetworkEngine=NULL;
	m_pITCPSocketCorrespond=NULL;

	return;
}

//析构函数
CAttemperEngineSink::~CAttemperEngineSink()
{
	//删除内存
	SafeDeleteArray(m_pTableFrame);
	SafeDeleteArray(m_pNormalParameter);
	SafeDeleteArray(m_pAndroidParameter);

	return;
}

//接口查询
void * __cdecl CAttemperEngineSink::QueryInterface(const IID & Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IAttemperEngineSink,Guid,dwQueryVer);
	QUERYINTERFACE(IGameServiceFrame,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IAttemperEngineSink,Guid,dwQueryVer);
	return NULL;
}

//调度模块启动
bool __cdecl CAttemperEngineSink::OnAttemperEngineStart(IUnknownEx * pIUnknownEx)
{
	__ENTER_FUNCTION_LOG_CATCH

	//效验参数
	ASSERT(pIUnknownEx!=NULL);

	//设置变量
	m_wColumnInfoSize=0;
	m_wOnLineInfoSize=0;
	memset(&m_ColumnInfo,0,sizeof(m_ColumnInfo));
	memset(&m_OnLineCountInfo,0,sizeof(m_OnLineCountInfo));
	m_UserAutoQueue.Clear();
	m_AndroidAutoQueue.Clear();

	//创建辅助变量
	m_wMaxUserItem=m_pGameServiceOption->wMaxConnect;
	m_pNormalParameter=new tagConnectItemInfo [m_wMaxUserItem];
	m_pAndroidParameter=new tagConnectItemInfo [MAX_ANDROID];
	memset(m_pNormalParameter,0,sizeof(tagConnectItemInfo)*m_wMaxUserItem);
	memset(m_pAndroidParameter,0,sizeof(tagConnectItemInfo)*MAX_ANDROID);

	//属性变量
	m_PropertyAttributeArrary.RemoveAll();
	m_FlowerAttributeArrary.RemoveAll();

	//加载游戏组件
	if (m_pIGameServiceManager->RectifyServiceOption(m_pGameServiceOption)==false) return false;

	//创建游戏桌子
	ASSERT(m_pGameServiceOption->wTableCount!=0);
	m_pTableFrame=new CTableFrame [m_pGameServiceOption->wTableCount];

	//彭文添加
	for (int i=0; i<m_pGameServiceOption->wTableCount; ++i)
	{
		(m_pTableFrame + i)->m_pAttemperEngineSink = this;
		(m_pTableFrame + i)->SetRoomLog(m_pRoomLog);
	}
	//添加结束
	if (m_pTableFrame==NULL) return false;

	//初始化桌子
	bool bSuccess=false;
	tagTableFrameParameter TableFrameParameter;
	TableFrameParameter.pGameServiceAttrib=m_pGameServiceAttrib;
	TableFrameParameter.pGameServiceOption=m_pGameServiceOption;
	TableFrameParameter.pIGameServiceFrame=QUERY_ME_INTERFACE(IUnknownEx);
	TableFrameParameter.pIGameServiceManager=QUERY_OBJECT_PTR_INTERFACE(m_pIGameServiceManager,IUnknownEx);
	for (WORD i=0;i<m_pGameServiceOption->wTableCount;i++)
	{
		bSuccess=(m_pTableFrame+i)->InitTableFrame(i,&TableFrameParameter);

		if(bSuccess && m_pRoomLog)
		{
			// 异步日志  add by HouGuoJiang 2011-11-25
			(m_pTableFrame+i)->m_pITableFrameSink->channel_to_game_service(CTGS_MAIN_LOG, CTGS_ASS_LOG_POINT, 
																														 (void*)m_pRoomLog, sizeof(m_pRoomLog));
			////随机数组件
			//// @yexiuxuan add
			//(m_pTableFrame+i)->m_pITableFrameSink->channel_to_game_service(CTCS_MAIN_GOODRAND, CTCS_MAIN_GOODRAND_POINT, (void*)m_pGoodRand, sizeof(m_pGoodRand));
		}

		if (bSuccess==false) return false;
	}

	m_ServerList.SetRoomLog(m_pRoomLog);	//房间列表
	m_ServerUserManager.SetRoomLog(m_pRoomLog);	//房间列表

	//机器人类
	m_AndroidUserManager.SetRoomLog(m_pRoomLog);
	m_AndroidUserManager.m_pITimerEngine=m_pITimerEngine;
	m_AndroidUserManager.m_pGameServiceAttrib=m_pGameServiceAttrib;
	m_AndroidUserManager.m_pGameServiceOption=m_pGameServiceOption;
	m_AndroidUserManager.m_pIGameServiceManager=m_pIGameServiceManager;
	m_AndroidUserManager.m_pIServerUserManager=QUERY_OBJECT_INTERFACE(m_ServerUserManager,IServerUserManager);
	m_AndroidUserManager.m_pITCPNetworkEngineEvent=QUERY_OBJECT_PTR_INTERFACE(m_pIAttemperEngine,ITCPNetworkEngineEvent);

	//构造列表解释
	switch (m_pGameServiceOption->wServerType)
	{
	case GAME_GENRE_GOLD:	//金币类型
		{
			//变量定义
			tagColumnItem ColumnItem[]=
			{
				115,DTP_USER_ACCOUNTS,TEXT("用户名"),
					60,DTP_GAME_ID,TEXT("ID 号码"),
					60,DTP_USER_SCORE,TEXT("金币"),
					40,DTP_USER_TABLE,TEXT("桌号"),
					60,DTP_LOVE_LINESS,TEXT("魅力"),
					85,DTP_GAME_LEVEL,TEXT("游戏级别"),
					50,DTP_USER_EXPERIENCE,TEXT("经验值"),
					60,DTP_WIN_RATE,TEXT("胜率"),
					60,DTP_FLEE_RATE,TEXT("逃率"),
					60,DTP_USER_PLAY_COUNT,TEXT("总局"),
					60,DTP_USER_WIN,TEXT("赢局"),
					60,DTP_USER_LOST,TEXT("输局"),
					60,DTP_USER_DRAW,TEXT("和局"),
					60,DTP_USER_FLEE,TEXT("逃局"),
					200,DTP_UNDER_WRITE,TEXT("个性签名"),
			};

			//设置变量
			m_ColumnInfo.wColumnCount=CountArray(ColumnItem);
			CopyMemory(m_ColumnInfo.ColumnItem,ColumnItem,sizeof(ColumnItem));

			break;
		}
	case GAME_GENRE_MATCH:	//比赛类型
		{
			//变量定义
			tagColumnItem ColumnItem[]=
			{
				115,DTP_USER_ACCOUNTS,TEXT("用户名"),
					60,DTP_GAME_ID,TEXT("ID 号码"),
					60,DTP_USER_SCORE,TEXT("赛币"),
					40,DTP_USER_TABLE,TEXT("桌号"),
					60,DTP_LOVE_LINESS,TEXT("魅力"),
					85,DTP_GAME_LEVEL,TEXT("游戏级别"),
					50,DTP_USER_EXPERIENCE,TEXT("经验值"),
					60,DTP_WIN_RATE,TEXT("胜率"),
					60,DTP_FLEE_RATE,TEXT("逃率"),
					60,DTP_USER_PLAY_COUNT,TEXT("总局"),
					60,DTP_USER_WIN,TEXT("赢局"),
					60,DTP_USER_LOST,TEXT("输局"),
					60,DTP_USER_DRAW,TEXT("和局"),
					60,DTP_USER_FLEE,TEXT("逃局"),
					200,DTP_UNDER_WRITE,TEXT("个性签名"),
			};

			//设置变量
			m_ColumnInfo.wColumnCount=CountArray(ColumnItem);
			CopyMemory(m_ColumnInfo.ColumnItem,ColumnItem,sizeof(ColumnItem));

			break;
		}
	default:
		{
			//变量定义
			tagColumnItem ColumnItem[]=
			{
				115,DTP_USER_ACCOUNTS,TEXT("用户名"),
					60,DTP_GAME_ID,TEXT("ID 号码"),
					60,DTP_USER_SCORE,TEXT("积分"),
					40,DTP_USER_TABLE,TEXT("桌号"),
					60,DTP_LOVE_LINESS,TEXT("魅力"),
					85,DTP_GAME_LEVEL,TEXT("游戏级别"),
					50,DTP_USER_EXPERIENCE,TEXT("经验值"),
					60,DTP_WIN_RATE,TEXT("胜率"),
					60,DTP_FLEE_RATE,TEXT("逃率"),
					60,DTP_USER_PLAY_COUNT,TEXT("总局"),
					60,DTP_USER_WIN,TEXT("赢局"),
					60,DTP_USER_LOST,TEXT("输局"),
					60,DTP_USER_DRAW,TEXT("和局"),
					60,DTP_USER_FLEE,TEXT("逃局"),
					200,DTP_UNDER_WRITE,TEXT("个性签名"),
			};

			//设置变量
			m_ColumnInfo.wColumnCount=CountArray(ColumnItem);
			CopyMemory(m_ColumnInfo.ColumnItem,ColumnItem,sizeof(ColumnItem));

			break;
		}
	}

	//设置列表
	WORD wColumnInfoHandSize=sizeof(m_ColumnInfo)-sizeof(m_ColumnInfo.ColumnItem);
	m_wColumnInfoSize=wColumnInfoHandSize+m_ColumnInfo.wColumnCount*sizeof(m_ColumnInfo.ColumnItem[0]);

	//随机数字
	srand((DWORD)time(NULL));

	//连接中心服务器
	m_pITCPSocketCorrespond->Connect(ntohl(m_dwCenterServer),PORT_CENTER_SERVER);

	//系统消息
	m_pITimerEngine->SetTimer(IDI_SEND_SYSTEM_MESSAGE,300000L,(DWORD)(-1),0);

	//启动机器
	if (m_AndroidUserManager.StartService()==false) return false;

	//加载公告
	m_pITimerEngine->SetTimer(IDI_UPDATE_AFFICHE ,TIME_UPDATE_AFFICHE,(DWORD)(-1),NULL);
	//更新繁忙的桌子数量
	m_pITimerEngine->SetTimer(IDI_UPDATE_BUSY_DESK, TIME_UPDATE_BUSY_DESK, (DWORD)(-1),NULL);
	//更新机器人的自动增加
	m_pITimerEngine->SetTimer(IDI_UPDATE_ADD_ANDROID,TIME_UPDATE_ADD_ANDROID,(DWORD)(-1), NULL);
	//更新用户加入
	m_pITimerEngine->SetTimer(IDI_UPDATE_ADD_USER, TIME_UPDATE_ADD_USER, (DWORD)(-1), NULL);
	//更新站位机器人
	m_pITimerEngine->SetTimer(IDI_KEEP_SITTING_ANDROID, TIME_KEEP_SITTING_ANDROID, (DWORD)(-1), NULL);


	//更新喜报
	m_pITimerEngine->SetTimer(IDI_UPDATE_HAPPY_MESS, TIME_UPDATE_HAPPY_MESS, DWORD(-1), 0);
	//更新等级信息
	m_pITimerEngine->SetTimer(IDI_LOAD_GRADE_INFO, TIME_LOAD_GRADE_INFO, DWORD(1), 0);  //只要更新一次
	//发送喜报
	m_pITimerEngine->SetTimer(IDI_SEND_HAPPY_MESS, TIME_SEND_HAPPY_MESS, DWORD(-1), 0);
	//更新强退
	m_pITimerEngine->SetTimer(IDI_UPDATE_EXIT_USER, TIME_UPDATE_EXIT_USER, DWORD(-1), 0);
	//保持数据库连接一直存在	Author:<cxf>
	m_pITimerEngine->SetTimer(IDI_CONTINUE_CONNECT, TIME_CONTINUE_CONNECT, DWORD(-1), 0);
	//新手礼包
	m_pITimerEngine->SetTimer(IDI_NEWGIFT, TIME_NEWGIFT, DWORD(-1), 0);

	if (m_pRoomLog)
		m_pRoomLog->AddMsg(0, 0, TRUE,"***** 调度模块启动");

	//throw(11);	// 测试用，固意抛出异常

	__LEAVE_FUNCTION_LOG_CATCH

	return true;
}

//调度模块关闭
bool __cdecl CAttemperEngineSink::OnAttemperEngineStop(IUnknownEx * pIUnknownEx)
{
	try
	{
		m_UserAutoQueue.Clear();
		m_AndroidAutoQueue.Clear();

		//注销房间
		CMD_CS_UnRegGameServer UnRegGameServer;
		memset(&UnRegGameServer,0,sizeof(UnRegGameServer));
		UnRegGameServer.wKindID=m_pGameServiceAttrib->wKindID;
		UnRegGameServer.wServerID=m_pGameServiceOption->wServerID;
		m_pITCPSocketCorrespond->SendData(MDM_CS_SERVER_MANAGER,SUB_CS_UNREG_GAME_SERVER,&UnRegGameServer,sizeof(UnRegGameServer));

		//关闭组件
		m_AndroidUserManager.StopService();
		m_ServerUserManager.ResetUserManager();

		//删除数据
		m_wMaxUserItem=0;
		SafeDeleteArray(m_pTableFrame);
		SafeDeleteArray(m_pNormalParameter);
		SafeDeleteArray(m_pAndroidParameter);

		//控制变量
		m_bShallClose=false;
		m_bAllowWisper=true;
		m_bAllowRoomChat=true;
		m_bAllowGameChat=true;
		m_bAllowEnterRoom=true;
		m_bAllowEnterGame=true;

		//消息变量
		m_cbRoomMsgIndex=0;
		m_cbGameMsgIndex=0;

		//设置变量
		m_wColumnInfoSize=0;
		m_wOnLineInfoSize=0;
		memset(&m_ColumnInfo,0,sizeof(m_ColumnInfo));
		memset(&m_OnLineCountInfo,0,sizeof(m_OnLineCountInfo));

		if (m_pRoomLog)
			m_pRoomLog->AddMsg(0, 0, TRUE,"***** 调度模块关闭");
	}
	catch (...)
	{
		int iTmp = 0;
		int iOther = 1;
	}

	return true;
}

//发送状态
bool __cdecl CAttemperEngineSink::SendTableStatus(WORD wTableID)
{
	__ENTER_FUNCTION_LOG_CATCH

	//效验参数
	ASSERT(wTableID<m_pGameServiceOption->wTableCount);
	if (wTableID>=m_pGameServiceOption->wTableCount) return false;

	//构造变量
	CMD_GR_TableStatus TableStatus;
	memset(&TableStatus,0,sizeof(TableStatus));
	TableStatus.wTableID=wTableID;
	TableStatus.bTableLock=(m_pTableFrame+wTableID)->IsTableLocked()?TRUE:FALSE;
	TableStatus.bPlayStatus=(m_pTableFrame+wTableID)->IsGameStarted()?TRUE:FALSE;

	//发送数据
	m_pITCPNetworkEngine->SendDataBatch(MDM_GR_STATUS,SUB_GR_TABLE_STATUS,&TableStatus,sizeof(TableStatus));
	m_AndroidUserManager.SendDataToClient(MDM_GR_STATUS,SUB_GR_TABLE_STATUS,&TableStatus,sizeof(TableStatus));

	__LEAVE_FUNCTION_LOG_CATCH

	return true;
}


//发送状态
bool __cdecl CAttemperEngineSink::SendUserStatus(IServerUserItem * pIServerUserItem)
{
	__ENTER_FUNCTION_LOG_CATCH

	//效验参数
	ASSERT(pIServerUserItem!=NULL);
	ASSERT(pIServerUserItem->IsActive()==true);
	if (pIServerUserItem->IsActive()==false) return false;

	//变量定义
	CMD_GR_UserStatus UserStatus;
	memset(&UserStatus,0,sizeof(UserStatus));
	tagServerUserData * pUserData=pIServerUserItem->GetUserData();

	//构造数据
	UserStatus.dwUserID=pUserData->dwUserID;
	UserStatus.wTableID=pUserData->wTableID;
	UserStatus.wChairID=pUserData->wChairID;
	UserStatus.cbUserStatus=pUserData->cbUserStatus;

	//发送数据
	m_pITCPNetworkEngine->SendDataBatch(MDM_GR_USER,SUB_GR_USER_STATUS,&UserStatus,sizeof(UserStatus));
	m_AndroidUserManager.SendDataToClient(MDM_GR_USER,SUB_GR_USER_STATUS,&UserStatus,sizeof(UserStatus));

	__LEAVE_FUNCTION_LOG_CATCH

	return true;
}


//更新用户的游戏数据
bool __cdecl CAttemperEngineSink::UpdateUserItem(IServerUserItem * pIServerUserItem)
{
	__ENTER_FUNCTION_LOG_CATCH

	DBR_GR_WriteUserScore  writeUserScore;
	ZeroMemory(&writeUserScore,sizeof(writeUserScore));

	//设置变量
	writeUserScore.dwUserID  =pIServerUserItem->GetUserID();
	writeUserScore.dwClientIP=pIServerUserItem->GetClientIP();
	pIServerUserItem->GetUserScoreInfo(writeUserScore.ScoreModifyInfo);//

	//附加信息
	writeUserScore.lRevenue=pIServerUserItem->GetRoundRevenue();
	writeUserScore.dwPlayTimeCount=pIServerUserItem->GetRoundPlayTime();
	//投递请求


	//测试使用

	tagServerUserData * pServerUserData=pIServerUserItem->GetUserData();
	if (writeUserScore.ScoreModifyInfo.lExperience > 999999 ||
		pIServerUserItem->GetUserRevenue() > 999999 )
	{

		char info[BUFSIZ] = {0};
		sprintf(info, "写入数据库，检测发现经验异常, id：%i, 写入等级:%i, 写入经验：%i, 玩家的税收:%i,  玩家等级: %i， 玩家经验：%i", 
			pServerUserData->dwUserID,
			writeUserScore.ScoreModifyInfo.dwGrade,
			writeUserScore.ScoreModifyInfo.lExperience,
			pIServerUserItem->GetUserRevenue(),
			pServerUserData->UserScoreInfo.dwGrade,
			pServerUserData->UserScoreInfo.lExperience
			);
		CTraceService::TraceString(info,TraceLevel_Normal);
	}

	if ( pServerUserData->UserScoreInfo.lExperience < 0 && writeUserScore.ScoreModifyInfo.lExperience < 0)
	{
		char info[BUFSIZ] = {0};
		sprintf(info, "写入数据库，检测发现玩家经验不一致, id：%i, 写入等级:%i, 写入经验：%i, 玩家的税收:%i,  玩家等级: %i， 玩家经验：%i", 
			pServerUserData->dwUserID,
			writeUserScore.ScoreModifyInfo.dwGrade,
			writeUserScore.ScoreModifyInfo.lExperience,
			pIServerUserItem->GetUserRevenue(),
			pServerUserData->UserScoreInfo.dwGrade,
			pServerUserData->UserScoreInfo.lExperience
			);
	}
	//情况：在线送礼 时间误差
	int iOnlineTime=pIServerUserItem->GetUserData()->dwOnlineTime;
	int iOnlineGiftCount=pIServerUserItem->GetUserData()->wOnlineGiftCount;
	int dwMaxVal[]={600, 1800, 3600, 7200};
	for (int i=0;i<4;i++)
	{
		if (i==iOnlineGiftCount-1 && iOnlineTime<dwMaxVal[i])
		{
			int iTimeOn=dwMaxVal[i]-iOnlineTime+1;
			if (iTimeOn>0)
			{
				writeUserScore.dwPlayTimeCount+=iTimeOn;
				break;
			}
		}
	}
	//
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_WRITE_GAME_SCORE, 0, &writeUserScore, sizeof(writeUserScore));
	pIServerUserItem->ClearScorModify();

	__LEAVE_FUNCTION_LOG_CATCH

	return true;
}

//解锁用户
bool  CAttemperEngineSink::UnLockUserItem(IServerUserItem * pIServerUserItem)
{
	//效验参数
	ASSERT((pIServerUserItem!=NULL)&&(pIServerUserItem->IsActive()==true));
	if ((pIServerUserItem==NULL)||(pIServerUserItem->IsActive()==false)) return true;

	//效验状态
	ASSERT(pIServerUserItem->GetTableID()==INVALID_TABLE);
	ASSERT(pIServerUserItem->GetChairID()==INVALID_CHAIR);

	//获取用户
	tagServerUserData *pServerUserData = pIServerUserItem->GetUserData();
	//投递请求
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_UNLOCK_USER,0,&(pServerUserData->dwUserID),sizeof(pServerUserData->dwUserID));
	return true;
}


//删除用户
bool __cdecl CAttemperEngineSink::DeleteUserItem(IServerUserItem * pIServerUserItem)
{
	__ENTER_FUNCTION_LOG_CATCH

	//效验参数
	ASSERT((pIServerUserItem!=NULL)&&(pIServerUserItem->IsActive()==true));
	if ((pIServerUserItem==NULL)||(pIServerUserItem->IsActive()==false)) return false;

	//效验状态
	ASSERT(pIServerUserItem->GetTableID()==INVALID_TABLE);
	ASSERT(pIServerUserItem->GetChairID()==INVALID_CHAIR);

	//获取用户
	tagServerUserData *pServerUserData = pIServerUserItem->GetUserData();

	//写入积分
	DBR_GR_LeaveGameServer LeaveGameServer;
	ZeroMemory(&LeaveGameServer,sizeof(LeaveGameServer));

	//设置变量
	LeaveGameServer.dwUserID     = pIServerUserItem->GetUserID();
	LeaveGameServer.dwClientIP   = pIServerUserItem->GetClientIP();
	pIServerUserItem->GetUserScoreInfo(LeaveGameServer.ScoreModifyInfo);//

	//附加信息
	LeaveGameServer.lLoveliness  = pIServerUserItem->GetUserData()->lLoveliness;
	LeaveGameServer.lRevenue     = pIServerUserItem->GetUserRevenue();
	LeaveGameServer.dwPlayTimeCount  = pIServerUserItem->GetPlayTimeCount();
	LeaveGameServer.dwOnlineTimeCount=pIServerUserItem->GetOnlineTimeCount();

	//投递请求
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_LEAVE_GAME_SERVER,0,&LeaveGameServer,sizeof(LeaveGameServer));

	//发送状态
	CMD_GR_UserStatus UserStatus;
	ZeroMemory(&UserStatus,sizeof(UserStatus));
	tagServerUserData * pUserData=pIServerUserItem->GetUserData();
	UserStatus.dwUserID=pIServerUserItem->GetUserID();
	UserStatus.wTableID=INVALID_TABLE;
	UserStatus.wChairID=INVALID_CHAIR;
	UserStatus.cbUserStatus=US_NULL;
	m_pITCPNetworkEngine->SendDataBatch(MDM_GR_USER,SUB_GR_USER_STATUS,&UserStatus,sizeof(UserStatus));
	m_AndroidUserManager.SendDataToClient(MDM_GR_USER,SUB_GR_USER_STATUS,&UserStatus,sizeof(UserStatus));

	//常规连接
	WORD wBindIndex=pIServerUserItem->GetUserIndex();
	if (wBindIndex<m_pGameServiceOption->wMaxConnect)
	{
		(m_pNormalParameter+wBindIndex)->pIServerUserItem=NULL;
	}
	else if ((wBindIndex>=INDEX_ANDROID)&&(wBindIndex<(INDEX_ANDROID+MAX_ANDROID)))
	{
		(m_pAndroidParameter+(wBindIndex-INDEX_ANDROID))->pIServerUserItem=NULL;
	}

	//删除用户
	m_ServerUserManager.DeleteUserItem(pIServerUserItem);

	__LEAVE_FUNCTION_LOG_CATCH

	return true;
}

//发送数据
bool __cdecl CAttemperEngineSink::SendData(IServerUserItem * pIServerUserItem, WORD wMainCmdID, WORD wSubCmdID)
{
	__ENTER_FUNCTION_LOG_CATCH

	//效验参数
	ASSERT(pIServerUserItem!=NULL);
	ASSERT(pIServerUserItem->IsActive()==true);
	if (pIServerUserItem->IsActive()==false) return false;

	//发送数据
	WORD wIndex=pIServerUserItem->GetUserIndex();
	if (wIndex!=INVALID_WORD)
	{
		tagConnectItemInfo * pConnectItemInfo=GetBindParameter(wIndex);
		if (wIndex>=INDEX_ANDROID)
		{
			DWORD dwID=pConnectItemInfo->dwSocketID;
			m_AndroidUserManager.SendDataToClient(dwID,wMainCmdID,wSubCmdID,NULL,0);
		}
		else
		{
			m_pITCPNetworkEngine->SendData(pConnectItemInfo->dwSocketID,wMainCmdID,wSubCmdID);
		}
	}

	__LEAVE_FUNCTION_LOG_CATCH

	return true;
}

//发送数据
bool __cdecl CAttemperEngineSink::SendData(IServerUserItem * pIServerUserItem, WORD wMainCmdID, WORD wSubCmdID, void * pData, WORD wDataSize)
{
	__ENTER_FUNCTION_LOG_CATCH

	//效验参数
	ASSERT(pIServerUserItem!=NULL);
	ASSERT(pIServerUserItem->IsActive()==true);
	if (pIServerUserItem->IsActive()==false) return false;

	//发送数据
	WORD wIndex=pIServerUserItem->GetUserIndex();
	if (wIndex!=INVALID_WORD)
	{
		tagConnectItemInfo * pConnectItemInfo=GetBindParameter(wIndex);
		if (wIndex>=INDEX_ANDROID)
		{
			DWORD dwID=pConnectItemInfo->dwSocketID;
			m_AndroidUserManager.SendDataToClient(dwID,wMainCmdID,wSubCmdID,pData,wDataSize);
		}
		else
		{
			m_pITCPNetworkEngine->SendData(pConnectItemInfo->dwSocketID,wMainCmdID,wSubCmdID,pData,wDataSize);
		}
	}

	__LEAVE_FUNCTION_LOG_CATCH

	return true;
}

//设置定时器
bool __cdecl CAttemperEngineSink::SetTableTimer(WORD wTableID, DWORD dwTimerID, DWORD dwElapse, DWORD dwRepeat, WPARAM wBindParam)
{
	//效验参数
	ASSERT(dwTimerID<RANGE_TABLE_TIME);
	ASSERT(wTableID<m_pGameServiceOption->wTableCount);

	//设置定时器
	if (wTableID>=m_pGameServiceOption->wTableCount) return false;
	DWORD dwTableTimeID=IDI_TABLE_FRAME_START+wTableID*RANGE_TABLE_TIME+dwTimerID;
	return m_pITimerEngine->SetTimer(dwTableTimeID,dwElapse,dwRepeat,wBindParam);
}

//删除定时器
bool __cdecl CAttemperEngineSink::KillTableTimer(WORD wTableID, DWORD dwTimerID)
{
	//效验参数
	ASSERT(dwTimerID<RANGE_TABLE_TIME);
	ASSERT(wTableID<m_pGameServiceOption->wTableCount);

	//删除定时器
	if (wTableID>=m_pGameServiceOption->wTableCount) return false;
	DWORD dwTableTimeID=IDI_TABLE_FRAME_START+wTableID*RANGE_TABLE_TIME+dwTimerID;
	return m_pITimerEngine->KillTimer(dwTableTimeID);
}

//输出信息
void __cdecl CAttemperEngineSink::ExportInformation(LPCTSTR pszString, enTraceLevel TraceLevel)
{
	CTraceService::TraceString(pszString,TraceLevel);
	return;
}

//发送登录失败
bool CAttemperEngineSink::SendLogonFailed(DWORD dwSocketID, LPCTSTR pszMessage, LONG lErrorCode)
{
	__ENTER_FUNCTION_LOG_CATCH


	//构造数据
	CMD_GR_LogonError LogonError;
	LogonError.lErrorCode=lErrorCode;
	lstrcpyn(LogonError.szErrorDescribe,pszMessage,CountArray(LogonError.szErrorDescribe));
	WORD wDescribeSize=CountStringBuffer(LogonError.szErrorDescribe);

	//发送数据
	WORD wSendSize=sizeof(LogonError)-sizeof(LogonError.szErrorDescribe)+wDescribeSize;
	if (LOWORD(dwSocketID)>=INDEX_ANDROID)
	{
		m_AndroidUserManager.SendDataToClient(dwSocketID,MDM_GR_LOGON,SUB_GR_LOGON_ERROR,&LogonError,wSendSize);
	}
	else m_pITCPNetworkEngine->SendData(dwSocketID,MDM_GR_LOGON,SUB_GR_LOGON_ERROR,&LogonError,wSendSize);

	__LEAVE_FUNCTION_LOG_CATCH

	return true;
}

//发送房间人已满
bool CAttemperEngineSink::SendLogonFull(DWORD dwSocketID, LPCTSTR pszMessage, DWORD userCount)
{
	__ENTER_FUNCTION_LOG_CATCH


	//构造数据
	CMD_GR_LogonFull LogonError;
	LogonError.dwCountUser = userCount;
	lstrcpyn(LogonError.szErrorDescribe,pszMessage,CountArray(LogonError.szErrorDescribe));
	WORD wDescribeSize=CountStringBuffer(LogonError.szErrorDescribe);

	//发送数据
	WORD wSendSize=sizeof(LogonError)-sizeof(LogonError.szErrorDescribe)+wDescribeSize;
	if (LOWORD(dwSocketID)>=INDEX_ANDROID)
	{
		m_AndroidUserManager.SendDataToClient(dwSocketID,MDM_GR_LOGON,SUB_GR_LOGON_FULL,&LogonError,wSendSize);
	}
	else m_pITCPNetworkEngine->SendData(dwSocketID,MDM_GR_LOGON, SUB_GR_LOGON_FULL,&LogonError,wSendSize);

	__LEAVE_FUNCTION_LOG_CATCH

	return true;
}


//发送道具消息
bool CAttemperEngineSink::SendProMessage(DWORD dwSocketID, LPCTSTR lpszMessage, WORD wMessageType)
{
	//tagConnectItemInfo * pConnectItemInfo=GetBindParameter(LOWORD(dwSocketID));
	//IServerUserItem *pIServerUserItem = pConnectItemInfo->pIServerUserItem;
	//if(pIServerUserItem->GetTableID()!=INVALID_TABLE)
	//	SendGameMessage(dwSocketID, lpszMessage, wMessageType);
	//else SendRoomMessage(dwSocketID, lpszMessage, wMessageType);

	//判断场所
	if(m_cbSendLocation==LOCATION_GAME_ROOM)
		SendGameMessage(dwSocketID, lpszMessage, wMessageType);
	else SendRoomMessage(dwSocketID, lpszMessage, wMessageType);

	return true;
}

//发送房间消息
bool CAttemperEngineSink::SendRoomMessage(DWORD dwSocketID, LPCTSTR lpszMessage, WORD wMessageType)
{
	__ENTER_FUNCTION_LOG_CATCH

	//构造数据包
	CMD_GR_Message Message;
	Message.wMessageType=wMessageType;
	lstrcpyn(Message.szContent,lpszMessage,CountArray(Message.szContent));
	Message.wMessageLength=CountStringBuffer(Message.szContent);

	//发送数据
	WORD wSendSize=sizeof(Message)-sizeof(Message.szContent)+Message.wMessageLength*sizeof(TCHAR);
	if (LOWORD(dwSocketID)==INVALID_WORD)
	{
		m_pITCPNetworkEngine->SendDataBatch(MDM_GR_SYSTEM,SUB_GR_MESSAGE,&Message,wSendSize);
		m_AndroidUserManager.SendDataToClient(MDM_GR_SYSTEM,SUB_GR_MESSAGE,&Message,wSendSize);
	}
	else
	{
		if (LOWORD(dwSocketID)>=INDEX_ANDROID)
		{
			m_AndroidUserManager.SendDataToClient(dwSocketID,MDM_GR_SYSTEM,SUB_GR_MESSAGE,&Message,wSendSize);
		}
		else m_pITCPNetworkEngine->SendData(dwSocketID,MDM_GR_SYSTEM,SUB_GR_MESSAGE,&Message,wSendSize);
	}

	__LEAVE_FUNCTION_LOG_CATCH

	return true;
}

//发送游戏消息
bool CAttemperEngineSink::SendGameMessage(DWORD dwSocketID, LPCTSTR lpszMessage, WORD wMessageType)
{
	__ENTER_FUNCTION_LOG_CATCH

	//构造数据包
	CMD_GF_Message Message;
	Message.wMessageType=wMessageType;
	lstrcpyn(Message.szContent,lpszMessage,CountArray(Message.szContent));
	Message.wMessageLength=CountStringBuffer(Message.szContent);

	//发送数据
	WORD wSendSize=sizeof(Message)-sizeof(Message.szContent)+Message.wMessageLength*sizeof(TCHAR);
	if (LOWORD(dwSocketID)==INVALID_WORD)
	{
		m_pITCPNetworkEngine->SendDataBatch(MDM_GF_FRAME,SUB_GF_MESSAGE,&Message,wSendSize);
		m_AndroidUserManager.SendDataToClient(MDM_GF_FRAME,SUB_GF_MESSAGE,&Message,wSendSize);
	}
	else
	{
		if (LOWORD(dwSocketID)>=INDEX_ANDROID)
		{
			m_AndroidUserManager.SendDataToClient(dwSocketID,MDM_GF_FRAME,SUB_GF_MESSAGE,&Message,wSendSize);
		}
		else m_pITCPNetworkEngine->SendData(dwSocketID,MDM_GF_FRAME,SUB_GF_MESSAGE,&Message,wSendSize);
	}

	__LEAVE_FUNCTION_LOG_CATCH

	return true;
}

//发送道具消息
bool CAttemperEngineSink::SendProMessage(IServerUserItem * pIServerUserItem, LPCTSTR lpszMessage, WORD wMessageType)
{
	__ENTER_FUNCTION_LOG_CATCH

	//效验用户
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//发送数据
	WORD wIndex=pIServerUserItem->GetUserIndex();
	if (wIndex!=INVALID_WORD)
	{
		tagConnectItemInfo * pConnectItemInfo=GetBindParameter(wIndex);
		SendProMessage(pConnectItemInfo->dwSocketID,lpszMessage,wMessageType);
		return true;
	}

	__LEAVE_FUNCTION_LOG_CATCH


	return true;
}

//发送房间消息
bool CAttemperEngineSink::SendRoomMessage(IServerUserItem * pIServerUserItem, LPCTSTR lpszMessage, WORD wMessageType)
{
	__ENTER_FUNCTION_LOG_CATCH

	//效验用户
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//发送数据
	WORD wIndex=pIServerUserItem->GetUserIndex();
	if (wIndex!=INVALID_WORD)
	{
		tagConnectItemInfo * pConnectItemInfo=GetBindParameter(wIndex);
		SendRoomMessage(pConnectItemInfo->dwSocketID,lpszMessage,wMessageType);
		return true;
	}

	__LEAVE_FUNCTION_LOG_CATCH

	return false;
}

//发送游戏消息
bool CAttemperEngineSink::SendGameMessage(IServerUserItem * pIServerUserItem, LPCTSTR lpszMessage, WORD wMessageType)
{
	__ENTER_FUNCTION_LOG_CATCH

	//效验用户
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//用户过滤
	if ((pIServerUserItem->GetTableID()==INVALID_TABLE)||(pIServerUserItem->GetChairID()==INVALID_CHAIR)) return false;

	//发送数据
	WORD wIndex=pIServerUserItem->GetUserIndex();
	if (wIndex!=INVALID_WORD)
	{
		tagConnectItemInfo * pConnectItemInfo=GetBindParameter(wIndex);
		SendGameMessage(pConnectItemInfo->dwSocketID,lpszMessage,wMessageType);

		return true;
	}

	__LEAVE_FUNCTION_LOG_CATCH


	return false;
}

//定时器事件
bool __cdecl CAttemperEngineSink::OnEventTimer(DWORD dwTimerID, WPARAM wBindParam)
{
	__ENTER_FUNCTION_LOG_CATCH

	//内核定时器
	if ((dwTimerID>=IDI_ATTEMPER_START)&&(dwTimerID<=IDI_ATTEMPER_CONCLUDE))
	{
		switch (dwTimerID)
		{
			//彭文添加
		case IDI_SEND_HAPPY_MESS: //发送喜报给这个服务器的所有用户
			{
				SendHappyMess();
				return true;
			}
		case IDI_UPDATE_EXIT_USER: //更新强退用户
			{
				UserLeftDataSetIT it    = m_userLeftDataSet.begin();
				UserLeftDataSetIT endit = m_userLeftDataSet.end();
				for (; it != endit;)
				{
					UserLeftDataSetIT cpIt = it;
					++cpIt;
					if (time(NULL) - (*it).second > TWO_MIN)
					{
						m_userLeftDataSet.erase(it);
					}
					it = cpIt;
				}
				return true;
			}

		case IDI_UPDATE_AFFICHE:  //更新公告
			{
				m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_LOAD_AFFICHE, 0 , NULL, 0); //请求更新公告
				return true;
			}
		case IDI_UPDATE_HAPPY_MESS: //更新喜报信息
			{
				m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_LOAD_HAPPY_MESS, 0, NULL, 0);
				return true;
			}
		case IDI_LOAD_GRADE_INFO: //请求等级信息
			{
				m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_READ_GRADE_INFO, 0, 0, 0);  //请求更新等级信息
				return true;
			}

		case IDI_UPDATE_BUSY_DESK:  //更新繁忙的桌子人数
			{
				UpdateBusyDesk();
				return true;
			}
		case IDI_UPDATE_ADD_USER: //尝试将用户加入到游戏 主要是处理2个用户等待，再有一次用户进入就可以游戏的情况
			{
				TryAddUser();
				return true;
			}
		case IDI_KEEP_SITTING_ANDROID: //尝试加入站位机器人
			{
				AddKeepSittingAndroid();
				return true;
			}

		case IDI_UPDATE_ADD_ANDROID: //加入机器人
			{
				UpDateAddAndroid();
				return true;
			}

			//添加结束
		case IDI_CONNECT_CENTER_SERVER:		//连接服务器
			{
				//发起连接
				m_pITCPSocketCorrespond->Connect(ntohl(m_dwCenterServer),PORT_CENTER_SERVER);

				//错误提示
				CTraceService::TraceString(TEXT("正在尝试重新连接中心服务器...."),TraceLevel_Normal);

				return true;
			}
		case IDI_UPDATE_SERVER_LIST:		//更新列表
			{
				m_pITCPSocketCorrespond->SendData(MDM_CS_SERVER_LIST,SUB_CS_GET_SERVER_LIST,NULL,0);
				return true;
			}
		case IDI_UPDATE_ONLINE_COUNT:		//更新人数
			{
				//更新人数
				CMD_CS_ServerOnLineCount ServerOnLineCount;
				ServerOnLineCount.wKindID=m_pGameServiceOption->wKindID;
				ServerOnLineCount.wServerID=m_pGameServiceOption->wServerID;
				ServerOnLineCount.dwOnLineCount=(m_ServerUserManager.GetOnLineCount()+m_ServerUserManager.GetOffLineCount());
				m_pITCPSocketCorrespond->SendData(MDM_CS_SERVER_MANAGER,SUB_CS_SERVER_ONLINE_COUNT,&ServerOnLineCount,sizeof(ServerOnLineCount));

				return true;
			}
		case IDI_SEND_SYSTEM_MESSAGE:		//系统消息
			{
				//获取目录
				TCHAR szPath[MAX_PATH]=TEXT("");
				GetCurrentDirectory(sizeof(szPath),szPath);

				//读取配置
				TCHAR szFileName[MAX_PATH],szMessage[1024];
				_snprintf(szFileName,sizeof(szFileName),TEXT("%s\\GameService.ini"),szPath);

				//循环消息
				TCHAR szRoomMsgName[64],szGameMsgName[64];
				_snprintf(szRoomMsgName,sizeof(szRoomMsgName),TEXT("RoomMessage%d"),m_cbRoomMsgIndex++);
				_snprintf(szGameMsgName,sizeof(szGameMsgName),TEXT("GameMessage%d"),m_cbGameMsgIndex++);

				//系统消息
				GetPrivateProfileString(TEXT("Message"),szRoomMsgName,TEXT(""),szMessage,sizeof(szMessage),szFileName);
				if (szMessage[0]!=0) SendRoomMessage(INVALID_WORD,szMessage,SMT_INFO);
				else m_cbRoomMsgIndex=0;

				//游戏消息
				GetPrivateProfileString(TEXT("Message"),szGameMsgName,TEXT(""),szMessage,sizeof(szMessage),szFileName);
				if (szMessage[0]!=0) SendGameMessage(INVALID_WORD,szMessage,SMT_INFO);
				else m_cbGameMsgIndex=0;

				return true;
			}
		case IDI_CONTINUE_CONNECT:		//保持数据库连接存在	Author:<cxf>
			{
				m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_CONTINUE_CONNECT, 0, 0, 0);
				return true;
			}
		case IDI_NEWGIFT:				//新手礼包	Author<cxf>
			{
				SendNewGift();
				return true;
			}
		}
	}

	//桌子定时器
	if (dwTimerID>=IDI_TABLE_FRAME_START)
	{
		//变量定义
		WORD wTableID=(WORD)((dwTimerID-IDI_TABLE_FRAME_START)/RANGE_TABLE_TIME);
		DWORD dwTableTimeID=(DWORD)((dwTimerID-IDI_TABLE_FRAME_START)%RANGE_TABLE_TIME);

		//定时器处理
		ASSERT(wTableID<m_pGameServiceOption->wTableCount);
		return (m_pTableFrame+wTableID)->OnEventTimer(dwTableTimeID,wBindParam);
	}

	__LEAVE_FUNCTION_LOG_CATCH

	return false;
}

//数据库事件
bool __cdecl CAttemperEngineSink::OnEventDataBase(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	switch (wRequestID)
	{
	case DBR_GR_LOGON_SUCCESS:		//登录成功
		{
			return OnDBLogonSuccess(dwContextID,pData,wDataSize);
		}
	case DBR_GR_LOGON_ERROR:		//登录失败
		{
			return OnDBLogonError(dwContextID,pData,wDataSize);
		}
	case DBR_GR_USER_RIGHT_RESULT:	//用户权限
		{
			return OnDBUserRightResult(dwContextID,pData,wDataSize);
		}
		//case DBR_GR_PROPERTY_ATTRIBUTE:	//加载道具
		//	{
		//		return OnDBLoadProperty(dwContextID,pData,wDataSize);
		//	}
	case DBR_GR_USER_FIND_GEMS_TASK_LOAD: //读取寻宝任务
		{
			return OnDBFindGemsTaskLoad(pData,wDataSize);
		}
	case DBR_GR_RE_FIND_GEMS_TASK_PERFORM_FINISH: //寻宝任务结束
		{
			return SendTaskFinish(pData, wDataSize );
		}

	case DBR_GR_AFFICHE_ONE: //加载公告
		{
			return OnDBLoadAffiche(dwContextID, pData, wDataSize);
		}
	case DBR_GR_HAPPY_MESS_ONE: //加载喜报
		{
			return OnDBLoadHappyMess(dwContextID, pData, wDataSize);
		}

	case DBR_GR_AFFICHE_SYNC: //同步公告
		{
			m_strAffiche = "";
			return true;
		}
	case DBR_GR_HAPPY_MESS_SYNC   : //同步喜报
		{
			m_strHappyMess = "";
			return true;
		}
	default:
		return true;
	}

	return false;
}

//用户登录成功
bool CAttemperEngineSink::OnDBLogonSuccess(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	__ENTER_FUNCTION_LOG_CATCH

	//效验参数
	ASSERT(wDataSize==sizeof(DBR_GR_LogonSuccess));
	if (wDataSize!=sizeof(DBR_GR_LogonSuccess)) 
		return true;

	//判断在线
	tagConnectItemInfo * pConnectItemInfo=GetBindParameter(LOWORD(dwContextID));
	if (pConnectItemInfo==NULL)
		return true;
	if (pConnectItemInfo->dwSocketID!=dwContextID)
		return true;
	ASSERT(pConnectItemInfo->pIServerUserItem==NULL);

	//消息处理
	DBR_GR_LogonSuccess * pLogonSuccess=(DBR_GR_LogonSuccess *)pData;

	//在线判断
	IServerUserItem * pIServerUserItem=m_ServerUserManager.SearchOnLineUser(pLogonSuccess->dwUserID);
	if (pIServerUserItem!=NULL)
		return true;

	//变量定义
	WORD wIndex=LOWORD(dwContextID);

	//状态判断
	if ((m_bAllowEnterRoom==false)&&(pLogonSuccess->dwMasterRight==0L))
	{
		//发送消息
		LPCTSTR pszMessage=TEXT("抱歉，由于系统维护的原因，本游戏房间不允许任何玩家登录进入！");
		SendRoomMessage(dwContextID,pszMessage,SMT_EJECT|SMT_INFO|SMT_INTERMIT_LINE|SMT_CLOSE_ROOM);

		//解除锁定
		DBR_GR_LeaveGameServer LeaveGameServer;
		ZeroMemory(&LeaveGameServer,sizeof(LeaveGameServer));
		LeaveGameServer.dwUserID=pLogonSuccess->dwUserID;
		LeaveGameServer.dwClientIP=pConnectItemInfo->dwClientIP;
		LeaveGameServer.lLoveliness=pLogonSuccess->lLoveliness;
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_LEAVE_GAME_SERVER,0,&LeaveGameServer,sizeof(LeaveGameServer));

		return true;
	}

	//是否强退超时 彭文
#if 1
	UserLeftDataSetIT it = m_userLeftDataSet.find(pLogonSuccess->dwUserID);
	if (it != m_userLeftDataSet.end())
	{
		time_t passTime = time(NULL) - (*it).second;
		if (passTime <=  TWO_MIN)
		{
			int minu = (TWO_MIN - passTime) / 60;
			int sec  = (TWO_MIN - passTime) % 60;

			DBR_GR_LogonError LogonError;
			LogonError.lErrorCode=0;
			if (minu > 0)
			{
				_snprintf(LogonError.szErrorDescribe,sizeof(LogonError.szErrorDescribe),TEXT("强退后，%i分%i秒内不能登陆游戏！"), minu, sec);
			}else
			{
				_snprintf(LogonError.szErrorDescribe,sizeof(LogonError.szErrorDescribe),TEXT("强退后，%i秒内不能登陆游戏！"), sec);
			}
		
			SendLogonFailed(dwContextID, LogonError.szErrorDescribe, LogonError.lErrorCode);
			//解除锁定
			DBR_GR_LeaveGameServer LeaveGameServer;
			ZeroMemory(&LeaveGameServer,sizeof(LeaveGameServer));
			LeaveGameServer.dwUserID=pLogonSuccess->dwUserID;
			LeaveGameServer.dwClientIP=pConnectItemInfo->dwClientIP;
			LeaveGameServer.lLoveliness=pLogonSuccess->lLoveliness;
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_LEAVE_GAME_SERVER,0,&LeaveGameServer,sizeof(LeaveGameServer));
			return true;
		}else 
		{
			m_userLeftDataSet.erase(it);
		}
	}
#endif


	if (pLogonSuccess->lGold < m_pGameServiceOption->lRestrictScore)
	{
		DBR_GR_LogonError LogonError;
		LogonError.lErrorCode=0;
		_snprintf(LogonError.szErrorDescribe,sizeof(LogonError.szErrorDescribe),TEXT("您的金币不足,不能进入房间！"));
		SendLogonFailed(dwContextID, LogonError.szErrorDescribe, LogonError.lErrorCode);
		//解除锁定
		DBR_GR_LeaveGameServer LeaveGameServer;
		ZeroMemory(&LeaveGameServer,sizeof(LeaveGameServer));
		LeaveGameServer.dwUserID=pLogonSuccess->dwUserID;
		LeaveGameServer.dwClientIP=pConnectItemInfo->dwClientIP;
		LeaveGameServer.lLoveliness=pLogonSuccess->lLoveliness;
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_LEAVE_GAME_SERVER,0,&LeaveGameServer,sizeof(LeaveGameServer));
		return true;
	}


	//判断数目
	DWORD dwUserCount=m_ServerUserManager.GetOnLineCount()+m_ServerUserManager.GetOffLineCount();
	if ((pLogonSuccess->cbMemberOrder==0)&&(pLogonSuccess->cbMasterOrder==0)&&((dwUserCount+LESS_CONNECT_COUNT)>=m_pGameServiceOption->wMaxConnect))
	{
		//发送消息
		LPCTSTR pszMessage=TEXT("抱歉，本游戏房间已经满人，禁止非会员玩家登录进入！");
		SendRoomMessage(dwContextID,pszMessage,SMT_EJECT|SMT_INFO|SMT_INTERMIT_LINE|SMT_CLOSE_ROOM);

		//解除锁定
		DBR_GR_LeaveGameServer LeaveGameServer;
		ZeroMemory(&LeaveGameServer,sizeof(LeaveGameServer));
		LeaveGameServer.dwUserID=pLogonSuccess->dwUserID;
		LeaveGameServer.dwClientIP=pConnectItemInfo->dwClientIP;
		LeaveGameServer.lLoveliness=pLogonSuccess->lLoveliness;
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_LEAVE_GAME_SERVER,0,&LeaveGameServer,sizeof(LeaveGameServer));

		return true;
	}

	//构造用户信息
	tagServerUserData ServerUserData;
	memset(&ServerUserData,0,sizeof(ServerUserData));
	ServerUserData.wFaceID=pLogonSuccess->wFaceID;
	ServerUserData.dwCustomFaceVer=pLogonSuccess->dwCustomFaceVer;
	ServerUserData.cbGender=pLogonSuccess->cbGender;
	ServerUserData.dwUserID=pLogonSuccess->dwUserID;
	ServerUserData.dwGameID=pLogonSuccess->dwGameID;
	ServerUserData.dwGroupID=pLogonSuccess->dwGroupID;
	ServerUserData.dwUserRight=pLogonSuccess->dwUserRight;
	ServerUserData.lLoveliness=pLogonSuccess->lLoveliness;
	ServerUserData.dwMasterRight=pLogonSuccess->dwMasterRight;
	ServerUserData.cbMemberOrder=pLogonSuccess->cbMemberOrder;
	ServerUserData.cbMasterOrder=pLogonSuccess->cbMasterOrder;
	ServerUserData.UserScoreInfo.lWinCount=pLogonSuccess->lWinCount;
	ServerUserData.UserScoreInfo.lLostCount=pLogonSuccess->lLostCount;
	ServerUserData.UserScoreInfo.lDrawCount=pLogonSuccess->lDrawCount;
	ServerUserData.UserScoreInfo.lFleeCount=pLogonSuccess->lFleeCount;
	ServerUserData.UserScoreInfo.lExperience=pLogonSuccess->lExperience;

	//彭文添加
	ServerUserData.UserScoreInfo.lGold      = pLogonSuccess->lGold;
	ServerUserData.UserScoreInfo.lGem      = pLogonSuccess->lGem;
	ServerUserData.UserScoreInfo.dwGrade     = pLogonSuccess->dwGrade;

	lstrcpyn(ServerUserData.szNickName,pLogonSuccess->szNickName,CountArray(ServerUserData.szNickName));
	ServerUserData.utUserType                   =  pLogonSuccess->utIsAndroid;
	//添加结束
	lstrcpyn(ServerUserData.szAccounts,pLogonSuccess->szAccounts,CountArray(ServerUserData.szAccounts));
	lstrcpyn(ServerUserData.szGroupName,pLogonSuccess->szGroupName,CountArray(ServerUserData.szGroupName));
	lstrcpyn(ServerUserData.szUnderWrite,pLogonSuccess->szUnderWrite,CountArray(ServerUserData.szUnderWrite));
	lstrcpyn(ServerUserData.szBankPassword,pLogonSuccess->szBankPassword,CountArray(ServerUserData.szBankPassword));

	//提取积分
	ServerUserData.lStorageGold=0L;


	//存储积分
	if ((m_pGameServiceOption->wServerType==GAME_GENRE_GOLD)&&(m_pGameServiceOption->lRestrictScore>0L))
	{
		if (pLogonSuccess->lGold>m_pGameServiceOption->lRestrictScore && m_pGameServiceOption->wServerType==GAME_GENRE_GOLD)
		{
			ServerUserData.UserScoreInfo.lGold=m_pGameServiceOption->lRestrictScore;
			ServerUserData.lStorageGold = pLogonSuccess->lGold - m_pGameServiceOption->lRestrictScore;

		}
	}


	//消费金币
	ServerUserData.UserScoreInfo.lMoney=pLogonSuccess->lMoney;
	//状态信息
	ServerUserData.cbUserStatus=US_FREE;
	ServerUserData.wTableID=INVALID_TABLE;
	ServerUserData.wChairID=INVALID_CHAIR;
	ServerUserData.dwOnlineTime=pLogonSuccess->lOnlineTime;				//用户一天在线时长
	ServerUserData.wOnlineGiftCount=pLogonSuccess->wOnlineGiftCount;				//用户在线送礼次数

	//Author<cxf>;DateTime<2012-03-02>
	ServerUserData.UserScoreInfo.lKindIDWinCount=pLogonSuccess->lKindIDWinCount;
	ServerUserData.UserScoreInfo.lKindIDLostCount=pLogonSuccess->lKindIDLostCount;
	ServerUserData.UserScoreInfo.lKindIDDrawCount=pLogonSuccess->lKindIDDrawCount;
	ServerUserData.UserScoreInfo.lKindIDFleeCount=pLogonSuccess->lKindIDFleeCount;
	//END

	//激活用户
	pConnectItemInfo->pIServerUserItem=m_ServerUserManager.ActiveUserItem(ServerUserData,pConnectItemInfo->dwClientIP,wIndex,pLogonSuccess->szPassWord,m_pGameServiceOption->wServerType);

	//发送房间信息
	SendGameServerInfo(pConnectItemInfo->pIServerUserItem,dwContextID);

	//广播用户信息
	SendUserItem(pConnectItemInfo->pIServerUserItem,INVALID_WORD);
	//发送队列人数 //彭文添加
	SendQueueCount(pConnectItemInfo->pIServerUserItem);

	//彭文测试
	//OnUserUpdateProp(12, pConnectItemInfo->pIServerUserItem, 0);

	SendAffiche(pConnectItemInfo->pIServerUserItem); //彭文添加，发送游戏公告
	//发送消息
	SendRoomMessage(dwContextID,TEXT("亲爱的用户，欢迎您多提宝贵建议！"),SMT_INFO);

	//比赛判断
	if (m_pGameServiceOption->wServerType&GAME_GENRE_MATCH)
	{
		LPCTSTR pszMessage=TEXT("请注意，这是游戏比赛房间，非比赛选手将不能在此房间进行游戏！");
		SendRoomMessage(dwContextID,pszMessage,SMT_INFO);
	}

	//防作弊房间
	if (m_pGameServiceOption->cbHideUserInfo==TRUE)
	{
		LPCTSTR pszMessage=TEXT("请注意，这是防作弊游戏房间，请点击 [ 1 ] 号桌子进行进入游戏！");
		SendRoomMessage(dwContextID,pszMessage,SMT_INFO);
	}

	//发送消息
	if (m_bShallClose==true)
	{
		LPCTSTR pszMessage=TEXT("请注意，由于需要进行游戏房间维护更新工作，此游戏房间即将暂停服务！");
		SendRoomMessage(dwContextID,pszMessage,SMT_INFO);
	}


	//设置群发
	if (wIndex<INDEX_ANDROID) m_pITCPNetworkEngine->AllowBatchSend(dwContextID,true);

	__LEAVE_FUNCTION_LOG_CATCH

	return true;
}

//用户登录失败
bool CAttemperEngineSink::OnDBLogonError(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	__ENTER_FUNCTION_LOG_CATCH

	//效验参数
	ASSERT(wDataSize==sizeof(DBR_GR_LogonError));
	if (wDataSize!=sizeof(DBR_GR_LogonError))
		return true;

	//判断在线
	tagConnectItemInfo * pConnectItemInfo=GetBindParameter(LOWORD(dwContextID));
	if (pConnectItemInfo==NULL)
		return true;
	if (pConnectItemInfo->dwSocketID!=dwContextID) 
		return true;

	//发送消息
	DBR_GR_LogonError * pLogonError=(DBR_GR_LogonError *)pData;
	pLogonError->szErrorDescribe[CountArray(pLogonError->szErrorDescribe)-1]=0;
	SendLogonFailed(dwContextID,pLogonError->szErrorDescribe,pLogonError->lErrorCode);

	//关闭网络
	if (LOWORD(dwContextID)>=INDEX_ANDROID)
	{
		m_AndroidUserManager.DeleteAndroidUserItem(dwContextID);
	}
	else
	{
		m_pITCPNetworkEngine->ShutDownSocket(dwContextID);
	}

	__LEAVE_FUNCTION_LOG_CATCH

	return true;
}



//用户权限
bool CAttemperEngineSink::OnDBUserRightResult(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	__ENTER_FUNCTION_LOG_CATCH

	//效验参数
	ASSERT(wDataSize==sizeof(DBR_GR_UserRightResult));
	if (wDataSize!=sizeof(DBR_GR_UserRightResult)) return false;

	//消息处理
	DBR_GR_UserRightResult * pUserRightResult=(DBR_GR_UserRightResult *)pData;

	//获取玩家
	IServerUserItem * pSendIServerUserItem=m_ServerUserManager.SearchOnLineUser(pUserRightResult->dwMasterUserID);
	IServerUserItem * pRcvIServerUserItem=m_ServerUserManager.SearchOnLineUser(pUserRightResult->dwUserID);
	if(pSendIServerUserItem==NULL || pRcvIServerUserItem==NULL)return false;
	tagServerUserData * pSendUserData=pSendIServerUserItem->GetUserData();

	//成功判断
	if (pUserRightResult->bGameSuccess || pUserRightResult->bAccountsSuccess)
	{
		//发送管理员
		WORD wEnumIndex=0;
		IServerUserItem * pIServerUserItemSend=NULL;
		while (true)
		{
			pIServerUserItemSend=m_ServerUserManager.EnumOnLineUser(wEnumIndex++);
			if (pIServerUserItemSend==NULL) break;
			if (pIServerUserItemSend->GetUserData()->cbMasterOrder==FALSE) continue;

			//构造数据
			CMD_GR_UserRight UserRight;
			memset(&UserRight,0,sizeof(UserRight));
			UserRight.dwUserRight = pUserRightResult->dwUserRight;
			UserRight.dwUserID = pRcvIServerUserItem->GetUserData()->dwUserID;

			//发送数据
			SendData(pIServerUserItemSend,MDM_GR_USER,SUB_GR_USER_RIGHT,&UserRight,sizeof(UserRight));
		}
	}
	else
	{
		//发送消息
		TCHAR szDescribe[128]=TEXT("");
		_snprintf(szDescribe,sizeof(szDescribe),TEXT("设置用户权限失败！"));
		SendGameMessage(pSendIServerUserItem,szDescribe,SMT_EJECT|SMT_INFO);		
		return true;
	}

	__LEAVE_FUNCTION_LOG_CATCH

	return true;
}





//网络应答事件
bool __cdecl CAttemperEngineSink::OnEventTCPNetworkBind(DWORD dwClientIP, DWORD dwSocketID)
{
#if 0	// 测试用，2012-03-29
	CString strPrompt;
	strPrompt.Format("OnEventTCPNetworkBind dwClientIP=%d ", dwClientIP);
	CTraceService::TraceString(strPrompt.GetBuffer(), TraceLevel_Exception);	
#endif

	//变量定义
	WORD wBindIndex=LOWORD(dwSocketID);
	tagConnectItemInfo * pConnectItemInfo=GetBindParameter(wBindIndex);

	//设置变量
	pConnectItemInfo->dwSocketID=dwSocketID;
	pConnectItemInfo->dwClientIP=dwClientIP;
	return true;
}

//网络事件
bool CAttemperEngineSink::OnEventTCPNetworkRead(CMD_Command Command, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch (Command.wMainCmdID)
	{
	case MDM_GR_LOGON:	//登录消息
		{
			return OnSocketMainLogon(Command.wSubCmdID,pData,wDataSize,dwSocketID);
		}
	case MDM_GR_USER:	//用户消息
		{
			return OnSocketMainUser(Command.wSubCmdID,pData,wDataSize,dwSocketID);
		}
	case MDM_GR_MANAGER://管理消息
		{
			return OnSocketMainManager(Command.wSubCmdID,pData,wDataSize,dwSocketID);
		}
	case MDM_GF_FRAME:	//框架消息
		{
			return OnSocketMainFrame(Command.wSubCmdID,pData,wDataSize,dwSocketID);
		}
	case MDM_GF_GAME:	//游戏消息
		{
			return OnSocketMainGame(Command.wSubCmdID,pData,wDataSize,dwSocketID);
		}
	case MDM_GF_VIDEO:	//视频消息
		{
			return OnSocketMainVideo(Command.wSubCmdID,pData,wDataSize,dwSocketID);
		}
	case MDM_GF_PRESENT://礼物消息  包括了道具，鲜花
		{
			return OnSocketPresent(Command.wSubCmdID,pData,wDataSize,dwSocketID);
		}
	case MDM_GF_BANK:	//银行消息
		{
			return OnSocketBank(Command.wSubCmdID,pData,wDataSize,dwSocketID);
		}
	default:
		return true;
	}

	return true;
}

//网络关闭事件
bool CAttemperEngineSink::OnEventTCPNetworkShut(DWORD dwClientIP, DWORD dwActiveTime, DWORD dwSocketID)
{

	__ENTER_FUNCTION_LOG_CATCH

	CString strPrompt;
	strPrompt.Format("OnEventTCPNetworkShut dwClientIP=%d ", dwClientIP);
	//CTraceService::TraceString(strPrompt.GetBuffer(), TraceLevel_Exception);	// 测试用，2012-03-29

	//用户处理
	tagConnectItemInfo * pConnectItemInfo=GetBindParameter(LOWORD(dwSocketID));
	if (pConnectItemInfo==NULL) return true;
	IServerUserItem * pIServerUserItem=pConnectItemInfo->pIServerUserItem;
	if (pIServerUserItem!=NULL)
	{
		//判断游戏
		bool bOffLine=false;
		if (pIServerUserItem->GetTableID()!=INVALID_TABLE)
		{
			//变量定义
			CTableFrame * pTableFrame=(m_pTableFrame+pIServerUserItem->GetTableID());
			//断线注册
			WORD wChairID=pIServerUserItem->GetChairID();
			if ((pTableFrame->IsUserPlaying(pIServerUserItem)==true)&&(pTableFrame->OnUserReqOffLine(wChairID)==true)) //彭文删除
			{
				bOffLine=m_ServerUserManager.RegOffLineUserItem(pIServerUserItem);
			}

			//断线处理
			if (bOffLine==true) 
				pTableFrame->OnUserOffLine(wChairID);
			else 
				pTableFrame->PerformStandUpAction(pIServerUserItem);
		}

		//删除用户
		if (bOffLine==false)//彭文删除
		{
			DeleteUserItem(pIServerUserItem);
		}

		//删除排队
		char info[BUFSIZ] = {0};
		tagServerUserData * pUserData=pIServerUserItem->GetUserData();
		if (pUserData != NULL)
		{
#if _SHOW_MESSAGE_WARN
			sprintf(info, _T("%s OnEventTCPNetworkShut 用户断线了，退出队列: 队列共：%i人"),  pUserData->szAccounts, m_UserAutoQueue.Size());
			CTraceService::TraceString(info, TraceLevel_Normal);
#endif		
		}
		m_UserAutoQueue.Erase(pIServerUserItem);
		m_AndroidAutoQueue.Erase(pIServerUserItem);
	}

	//设置数据
	ZeroMemory(pConnectItemInfo,sizeof(tagConnectItemInfo));

	__LEAVE_FUNCTION_LOG_CATCH


	return true;
}

//中心连接事件
bool __cdecl CAttemperEngineSink::OnEventTCPSocketLink(WORD wServiceID, INT nErrorCode)
{
	__ENTER_FUNCTION_LOG_CATCH

	CTraceService::TraceString(TEXT("OnEventTCPSocketLink "),TraceLevel_Warning);

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
		//连上提示
		CTraceService::TraceString(TEXT("中心服务器连接成功"),TraceLevel_Normal);
	}

	//注册房间
	CMD_CS_RegGameServer RegGameServer;
	memset(&RegGameServer,0,sizeof(RegGameServer));
	RegGameServer.GameServer.wSortID=m_pGameServiceOption->wSortID;
	RegGameServer.GameServer.wKindID=m_pGameServiceOption->wKindID;
	RegGameServer.GameServer.wServerID=m_pGameServiceOption->wServerID;
	RegGameServer.GameServer.wStationID=m_pGameServiceOption->wNodeID;
	RegGameServer.GameServer.wServerPort=m_pGameServiceOption->wServerPort;
	//RegGameServer.GameServer.dwServerAddr=m_pGameServiceOption->dwServerAddr;

	RegGameServer.GameServer.dwServerAddrCNN=m_pGameServiceOption->dwServerAddrCNN;
	RegGameServer.GameServer.dwServerAddrCUN=m_pGameServiceOption->dwServerAddrCUN;


	RegGameServer.GameServer.dwOnLineCount=(m_ServerUserManager.GetOnLineCount()+m_ServerUserManager.GetOffLineCount());
	RegGameServer.GameServer.dwMaxCount   = m_pGameServiceOption->wMaxConnect - LESS_CONNECT_COUNT;	//彭文添加
	//RegGameServer.GameServer.lLessScore   = m_pGameServiceOption->lLessScore;  //彭文添加
	//RegGameServer.GameServer.lMaxScore    = m_pGameServiceOption->lMaxScore;   //彭文添加
	RegGameServer.GameServer.lRestrictScore = m_pGameServiceOption->lRestrictScore; //彭文添加
	RegGameServer.GameServer.lCellScore     = m_pGameServiceOption->lCellScore;  //彭文添加
	RegGameServer.GameServer.lRestrictGrade = m_pGameServiceOption->lRestrictGrade;//彭文添加
	RegGameServer.GameServer.lEnterMoney    = m_pGameServiceOption->lEnterMoney; //彭文添加


	lstrcpyn(RegGameServer.GameServer.szServerName,m_pGameServiceOption->szGameRoomName,CountArray(RegGameServer.GameServer.szServerName));
	m_pITCPSocketCorrespond->SendData(MDM_CS_SERVER_MANAGER,SUB_CS_REG_GAME_SERVER,&RegGameServer,sizeof(RegGameServer));

	//获取列表
	m_pITCPSocketCorrespond->SendData(MDM_CS_SERVER_LIST,SUB_CS_GET_SERVER_LIST,NULL,0);

	//设置定时器
	m_pITimerEngine->SetTimer(IDI_UPDATE_SERVER_LIST,TIME_UPDATE_LIST,TIMES_INFINITY,NULL);
	m_pITimerEngine->SetTimer(IDI_UPDATE_ONLINE_COUNT,TIME_UPDATE_ONLINE,TIMES_INFINITY,NULL);

	__LEAVE_FUNCTION_LOG_CATCH

	return true;
}

//中心读取事件
bool __cdecl CAttemperEngineSink::OnEventTCPSocketRead(WORD wServiceID, CMD_Command Command, VOID * pData, WORD wDataSize)
{
	switch (Command.wMainCmdID)
	{
	case MDM_CS_SERVER_LIST:	//列表消息
		{
			return OnCenterMainServerList(Command,pData,wDataSize);
		}
	}
	return true;
}

//中心关闭事件
bool __cdecl CAttemperEngineSink::OnEventTCPSocketShut(WORD wServiceID, BYTE cbShutReason)
{
	__ENTER_FUNCTION_LOG_CATCH

	//重新连接
	m_pITimerEngine->KillTimer(IDI_UPDATE_SERVER_LIST);
	m_pITimerEngine->KillTimer(IDI_UPDATE_ONLINE_COUNT);
	m_pITimerEngine->SetTimer(IDI_CONNECT_CENTER_SERVER,TIME_RECONNECT,1,NULL);

	//错误提示
	CTraceService::TraceString(TEXT("中心服务器连接中断，稍后将会再次尝试...."),TraceLevel_Warning);

	__LEAVE_FUNCTION_LOG_CATCH

	return true;
}

//登录消息处理
bool CAttemperEngineSink::OnSocketMainLogon(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	__ENTER_FUNCTION_LOG_CATCH

	switch (wSubCmdID)
	{
	case SUB_GR_LOGON_USERID:		//ID 登录
		{
			//CTraceService::TraceString("进入登录",TraceLevel_Exception);	// 测试用，2012-03-29

			//效验参数
			ASSERT(wDataSize>=sizeof(CMD_GR_LogonByUserID));
			if (wDataSize<sizeof(CMD_GR_LogonByUserID)) 
				return true;

			//处理消息
			CMD_GR_LogonByUserID * pLogonByUserID=(CMD_GR_LogonByUserID *)pData;
			pLogonByUserID->szPassWord[CountArray(pLogonByUserID->szPassWord)-1]=0;   

			//重复判断
			tagConnectItemInfo * pConnectItemInfo=GetBindParameter(LOWORD(dwSocketID));
			if (pConnectItemInfo==NULL) 
				return true;
			IServerUserItem * pIServerUserItem=pConnectItemInfo->pIServerUserItem;
			if (pIServerUserItem!=NULL)
				return true;

			//在线判断
			pIServerUserItem=m_ServerUserManager.SearchOnLineUser(pLogonByUserID->dwUserID);
			if (pIServerUserItem!=NULL)
			{
				//效验密码
				if (lstrcmp(pIServerUserItem->GetPassword(),pLogonByUserID->szPassWord)!=0)
				{
					SendLogonFailed(dwSocketID,TEXT("帐户密码有误，请查证后再次尝试登录！"),0);

					if (LOWORD(dwSocketID)>=INDEX_ANDROID)
					{
						m_AndroidUserManager.DeleteAndroidUserItem(dwSocketID);
					}
					else
					{
						m_pITCPNetworkEngine->ShutDownSocket(dwSocketID);
					}

					return true;
				}

				//变量定义//修改
				WORD wSourceIndex=pIServerUserItem->GetUserIndex();
				tagConnectItemInfo * pConnectItemSource=GetBindParameter(wSourceIndex);
				tagConnectItemInfo * pConnectItemTarger=GetBindParameter(LOWORD(dwSocketID));

				//断开用户
				if (pConnectItemSource!=NULL)
				{
					//设置变量
					pConnectItemSource->pIServerUserItem=NULL;

					//构造消息
					LPCTSTR pszMessage=TEXT("请注意，您的帐号在另一地方进入此游戏房间了，您被迫离开！");
					SendRoomMessage(pConnectItemSource->dwSocketID,pszMessage,SMT_INFO|SMT_EJECT|SMT_GLOBAL|SMT_CLOSE_ROOM);

					//发生消息
					if (wSourceIndex>=INDEX_ANDROID)
					{
						m_AndroidUserManager.DeleteAndroidUserItem(pConnectItemSource->dwSocketID);
					}
					else
					{
						m_pITCPNetworkEngine->ShutDownSocket(pConnectItemSource->dwSocketID);
					}
				}

				//激活用户
				pConnectItemTarger->pIServerUserItem=pIServerUserItem;
				m_ServerUserManager.SwitchOnLineUserItem(pIServerUserItem,pConnectItemTarger->dwClientIP,LOWORD(dwSocketID));

				//发送房间信息
				SendGameServerInfo(pIServerUserItem,dwSocketID);

				//设置群发
				if (LOWORD(dwSocketID)<INDEX_ANDROID)
				{
					m_pITCPNetworkEngine->AllowBatchSend(dwSocketID,true);
				}

				SendFindGemsTask(pIServerUserItem);
				return true;
			}

			//断线判断
			pIServerUserItem=m_ServerUserManager.SearchOffLineUser(pLogonByUserID->dwUserID);
			if (pIServerUserItem!=NULL)
			{
				//效验密码
				if (lstrcmp(pIServerUserItem->GetPassword(),pLogonByUserID->szPassWord)!=0)
				{
					SendLogonFailed(dwSocketID,TEXT("帐户密码有误，请查证后再次尝试登录！"),0);

					if (LOWORD(dwSocketID)>=INDEX_ANDROID)
					{
						m_AndroidUserManager.DeleteAndroidUserItem(dwSocketID);
					}
					else
					{
						m_pITCPNetworkEngine->ShutDownSocket(dwSocketID);
					}

					return true;
				}

				//设置用户
				pConnectItemInfo->pIServerUserItem=pIServerUserItem;
				m_ServerUserManager.ActiveOffLineUserItem(pIServerUserItem,pConnectItemInfo->dwClientIP,LOWORD(dwSocketID));

				//断线重入
				WORD wTableID=pIServerUserItem->GetTableID();
				WORD wChairID=pIServerUserItem->GetChairID();
				(m_pTableFrame+wTableID)->OnUserReConnect(wChairID);

				//发送房间信息
				SendGameServerInfo(pIServerUserItem,dwSocketID);

				//设置群发
				if (LOWORD(dwSocketID)<INDEX_ANDROID)
				{
					m_pITCPNetworkEngine->AllowBatchSend(dwSocketID,true);
				}

				SendFindGemsTask(pIServerUserItem);
				return true;
			}

			//构造数据
			DBR_GR_LogonByUserID LogonByUserID;
			memset(&LogonByUserID,0,sizeof(LogonByUserID));
			LogonByUserID.dwUserID=pLogonByUserID->dwUserID;
			LogonByUserID.dwClientIP=pConnectItemInfo->dwClientIP;
			lstrcpyn(LogonByUserID.szPassWord,pLogonByUserID->szPassWord,sizeof(LogonByUserID.szPassWord));
			LogonByUserID.dwKindID =  pLogonByUserID->dwKindID; //彭文。还要加上


			//房间满人 彭文添加
			DWORD dwUserCount=m_ServerUserManager.GetOnLineCount()+m_ServerUserManager.GetOffLineCount();
			if ((dwUserCount+LESS_CONNECT_COUNT)>=m_pGameServiceOption->wMaxConnect)
			{
				//发送消息
				LPCTSTR pszMessage=TEXT("抱歉，本游戏房间已经满人！");
				SendLogonFull(dwSocketID, pszMessage, dwUserCount);
				return true;
			}
			//添加结束

			//变量定义
			VOID * pData=NULL;
			tagDataDescribe DataDescribe;
			CRecvPacketHelper RecvPacket(pLogonByUserID+1,wDataSize-sizeof(CMD_GR_LogonByUserID));

			//扩展信息
			while (true)
			{
				//提取数据
				pData=RecvPacket.GetData(DataDescribe);
				if (DataDescribe.wDataDescribe==DTP_NULL) break;

				//数据分析
				switch (DataDescribe.wDataDescribe)
				{
				case DTP_COMPUTER_ID:		//机器标识
					{
						ASSERT(pData!=NULL);
						ASSERT(DataDescribe.wDataSize==sizeof(tagClientSerial));
						if (DataDescribe.wDataSize==sizeof(tagClientSerial))
						{
							BYTE * pcbByte=(BYTE *)pData;
							for (INT i=0;i<sizeof(tagClientSerial)/sizeof(BYTE);i++)
							{
								ASSERT(CountArray(LogonByUserID.szComputerID)>((i*2)+1));
								_stprintf(&LogonByUserID.szComputerID[i*2],TEXT("%02X"),*(pcbByte+i));
							}
						}

						break;
					}
				}
			}

			//投递数据库
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_LOGON_BY_USERID,dwSocketID,&LogonByUserID,sizeof(LogonByUserID));
			
			//CTraceService::TraceString("正常离开登录",TraceLevel_Exception);	// 测试用，2012-03-29

			return true;
		}
	}

	__LEAVE_FUNCTION_LOG_CATCH
	
	//CTraceService::TraceString("异常离开登录",TraceLevel_Exception);	// 测试用，2012-03-29

	return true;
}

//用户消息处理
bool CAttemperEngineSink::OnSocketMainUser(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	__ENTER_FUNCTION_LOG_CATCH

	switch (wSubCmdID)
	{
	case SUB_GR_USER_SIT_REQ:		//坐下桌子   
	case SUB_GR_USER_LOOKON_REQ:	//旁观桌子
		{
			//变量定义
			CMD_GR_UserSitReq * pUserSitReq=(CMD_GR_UserSitReq *)pData;
			WORD wHeadSize=sizeof(CMD_GR_UserSitReq)-sizeof(pUserSitReq->szTablePass);

			//效验数据
			ASSERT((wDataSize>=wHeadSize)&&(wDataSize<=sizeof(CMD_GR_UserSitReq)));
			if ((wDataSize<wHeadSize)||(wDataSize>sizeof(CMD_GR_UserSitReq))) 
				return true;
			ASSERT((wHeadSize+pUserSitReq->cbPassLen)==wDataSize);
			if ((wHeadSize+pUserSitReq->cbPassLen)!=wDataSize)
				return true;

			//判断位置
			if ((pUserSitReq->wChairID>=m_pGameServiceAttrib->wChairCountMax)&&(pUserSitReq->wChairID!=INVALID_CHAIR)) 
				return true;
			if ((pUserSitReq->wTableID>=m_pGameServiceOption->wTableCount)&&(pUserSitReq->wTableID!=INVALID_TABLE)) 
				return true;

			//获取用户
			IServerUserItem * pIServerUserItem=GetServerUserItem(LOWORD(dwSocketID));
			if (pIServerUserItem==NULL) 
				return true;
			tagServerUserData * pUserData=pIServerUserItem->GetUserData();

			//彭文添加  不允许坐下座子，因为有了排队系统
			if ((wSubCmdID==SUB_GR_USER_SIT_REQ))
			{
				//构造数据
				CMD_GR_SitFailed SitFailed;
				memset(&SitFailed,0,sizeof(SitFailed));
				_snprintf(SitFailed.szFailedDescribe, sizeof(SitFailed.szFailedDescribe), TEXT("您必须排队进入游戏！"));

				//发送数据
				WORD wSendSize=sizeof(SitFailed)-sizeof(SitFailed.szFailedDescribe)+CountStringBuffer(SitFailed.szFailedDescribe);
				SendData(pIServerUserItem,MDM_GR_USER,SUB_GR_SIT_FAILED,&SitFailed,wSendSize);
				//彭文添加 保证队列中没有这个用户
				return true;
			}

			//旁观设置
			if ((wSubCmdID==SUB_GR_USER_LOOKON_REQ)&&(m_pGameServiceOption->cbUnLookOnTag==TRUE))
			{
				//构造数据
				CMD_GR_SitFailed SitFailed;
				memset(&SitFailed,0,sizeof(SitFailed));
				_snprintf(SitFailed.szFailedDescribe,sizeof(SitFailed.szFailedDescribe),TEXT("本房间限制了旁观游戏!"),pUserData->wTableID+1);

				//发送数据
				WORD wSendSize=sizeof(SitFailed)-sizeof(SitFailed.szFailedDescribe)+CountStringBuffer(SitFailed.szFailedDescribe);
				SendData(pIServerUserItem,MDM_GR_USER,SUB_GR_SIT_FAILED,&SitFailed,wSendSize);
				return true;
			}

			//处理过虑
			if ((pUserData->wTableID==pUserSitReq->wTableID)&&(pUserData->wChairID==pUserSitReq->wChairID))
			{
				if ((pUserData->cbUserStatus!=US_LOOKON)&&(wSubCmdID==SUB_GR_USER_SIT_REQ)) return true;
				if ((pUserData->cbUserStatus==US_LOOKON)&&(wSubCmdID==SUB_GR_USER_LOOKON_REQ)) return true;
			}

			//离开位置  
			if (pUserData->wTableID!=INVALID_TABLE)
			{   //玩家正在玩
				CTableFrame * pTableFrame=(m_pTableFrame+pUserData->wTableID);
				//构造数据
				CMD_GR_SitFailed SitFailed;
				memset(&SitFailed,0,sizeof(SitFailed));
				_snprintf(SitFailed.szFailedDescribe,sizeof(SitFailed.szFailedDescribe),TEXT("您正在的 [ %d ] 号游戏桌游戏，暂时不能离开位置！"),pUserData->wTableID+1);
				//发送数据
				WORD wSendSize=sizeof(SitFailed)-sizeof(SitFailed.szFailedDescribe)+CountStringBuffer(SitFailed.szFailedDescribe);
				SendData(pIServerUserItem,MDM_GR_USER,SUB_GR_SIT_FAILED,&SitFailed,wSendSize);
				return true;
			}


			//彭文添加，用户已经排队了
			if (m_UserAutoQueue.IsExist(pIServerUserItem))
			{
				//构造数据
				CMD_GR_SitFailed SitFailed;
				memset(&SitFailed,0,sizeof(SitFailed));
				_snprintf(SitFailed.szFailedDescribe,sizeof(SitFailed.szFailedDescribe),TEXT("您已经排队了，暂时不能旁观！"));

				//发送数据
				WORD wSendSize=sizeof(SitFailed)-sizeof(SitFailed.szFailedDescribe)+CountStringBuffer(SitFailed.szFailedDescribe);
				SendData(pIServerUserItem,MDM_GR_USER,SUB_GR_SIT_FAILED,&SitFailed,wSendSize);
				return true;
			}


			CTableFrame * pTableFrame=(m_pTableFrame+pUserSitReq->wTableID);
			pTableFrame->PerformLookonAction(pUserSitReq->wChairID,pIServerUserItem,pUserSitReq->szTablePass);
			return true;
		}
	case SUB_GR_USER_STANDUP_REQ:	//起立请求
	case SUB_GR_USER_LEFT_GAME_REQ:	//强退请求
		{
			//获取用户
			IServerUserItem * pIServerUserItem=GetServerUserItem(LOWORD(dwSocketID));
			if (pIServerUserItem==NULL) return true;

			//处理过虑
			tagServerUserData * pUserData=pIServerUserItem->GetUserData();
			if ((pUserData->wTableID==INVALID_TABLE)||(pUserData->wChairID==INVALID_CHAIR)) 
				return true;

			//起立位置
			CTableFrame * pTableFrame=(m_pTableFrame+pUserData->wTableID);
			if ((wSubCmdID==SUB_GR_USER_STANDUP_REQ)&&(pTableFrame->IsUserPlaying(pIServerUserItem)==true))
			{
				TCHAR szMessage[256]=TEXT("");
				_snprintf(szMessage,sizeof(szMessage),TEXT("您正在的 [ %d ] 号游戏桌游戏，暂时不能离开位置！"),pUserData->wTableID+1);
				SendRoomMessage(dwSocketID,szMessage,SMT_EJECT);
				return true;
			}
#if _SHOW_MESSAGE_NOR
			char info[BUFSIZ] = {0};
			sprintf(info, _T("%s 用户离开椅子了，退出队列: 队列共：%i人"), pUserData->szAccounts, m_UserAutoQueue.Size());
			CTraceService::TraceString(info, TraceLevel_Normal);
#endif			
			m_UserAutoQueue.Erase(pIServerUserItem);
			m_AndroidAutoQueue.Erase(pIServerUserItem);		

			pTableFrame->PerformStandUpAction(pIServerUserItem);
			//捕鱼退出不计倒计时 601=捕鱼 Author<cxf>;DateTime<2012-03-31>
			if (601!=m_pGameServiceAttrib->wKindID)
			{
				m_userLeftDataSet.insert(UserLeftDataSet::value_type(pUserData->dwUserID,  time(NULL)));
			}
			//想所有的用法发送队列人数
			SendAllQueueCount();
			return true;
		}


	case SUB_GR_USER_CHAT:			//聊天消息
		{
			//效验参数
			CMD_GR_UserChat * pUserChat=(CMD_GR_UserChat *)pData;
			ASSERT(wDataSize>=(sizeof(CMD_GR_UserChat)-sizeof(pUserChat->szChatMessage)));
			ASSERT(wDataSize==(sizeof(CMD_GR_UserChat)-sizeof(pUserChat->szChatMessage)+pUserChat->wChatLength));
			if (wDataSize<(sizeof(CMD_GR_UserChat)-sizeof(pUserChat->szChatMessage))) 
				return true;
			if (wDataSize!=(sizeof(CMD_GR_UserChat)-sizeof(pUserChat->szChatMessage)+pUserChat->wChatLength)) 
				return true;

			//获取用户
			IServerUserItem * pIServerUserItem=GetServerUserItem(LOWORD(dwSocketID));
			if (pIServerUserItem==NULL)
				return true;

			//寻找用户
			IServerUserItem * pIServerUserItemRecv=NULL;
			if (pUserChat->dwTargetUserID!=0) 
			{
				pIServerUserItemRecv=m_ServerUserManager.SearchOnLineUser(pUserChat->dwTargetUserID);
				if (pIServerUserItemRecv==NULL) 
					return true;
			}

			//状态判断
			if (m_bAllowRoomChat==false)
			{
				LPCTSTR pszMessage=TEXT("抱歉，本游戏房间不允许进行大厅公聊！");
				SendRoomMessage(pIServerUserItem,pszMessage,SMT_EJECT|SMT_INFO);
				return true;
			}

			//权限判断
			if (CUserRight::CanRoomChat(pIServerUserItem->GetUserData()->dwUserRight)==false)
			{
				LPCTSTR pszMessage=TEXT("抱歉，您没有大厅公聊的权限，若需要帮助，请联系游戏客服咨询！");
				SendRoomMessage(pIServerUserItem,pszMessage,SMT_EJECT|SMT_INFO);
				return true;
			}

			//转发消息
			m_pITCPNetworkEngine->SendDataBatch(MDM_GR_USER,SUB_GR_USER_CHAT,pUserChat,wDataSize);
			m_AndroidUserManager.SendDataToClient(MDM_GR_USER,SUB_GR_USER_CHAT,pUserChat,wDataSize);

			return true;
		}
	case SUB_GR_USER_WISPER:		//私语消息
		{
			//效验参数
			CMD_GR_Wisper * pWisper=(CMD_GR_Wisper *)pData;
			ASSERT(wDataSize>=(sizeof(CMD_GR_Wisper)-sizeof(pWisper->szChatMessage)));
			ASSERT(wDataSize==(sizeof(CMD_GR_Wisper)-sizeof(pWisper->szChatMessage)+pWisper->wChatLength));
			if (wDataSize<(sizeof(CMD_GR_Wisper)-sizeof(pWisper->szChatMessage))) 
				return true;
			if (wDataSize!=(sizeof(CMD_GR_Wisper)-sizeof(pWisper->szChatMessage)+pWisper->wChatLength))
				return true;

			//获取用户
			IServerUserItem * pIServerUserItemSend=GetServerUserItem(LOWORD(dwSocketID));
			if (pIServerUserItemSend==NULL) 
				return true;
			IServerUserItem * pIServerUserItemRecv=m_ServerUserManager.SearchOnLineUser(pWisper->dwTargetUserID);
			if (pIServerUserItemRecv==NULL) 
				return true;

			//状态判断
			DWORD dwMasterRightSend=pIServerUserItemSend->GetUserData()->dwMasterRight;
			DWORD dwMasterRightRecv=pIServerUserItemRecv->GetUserData()->dwMasterRight;
			if ((m_bAllowWisper==false)&&(dwMasterRightSend==0L)&&(dwMasterRightRecv==0L))
			{
				LPCTSTR pszMessage=TEXT("抱歉，本游戏房间不能进行玩家私聊！");
				SendRoomMessage(pIServerUserItemSend,pszMessage,SMT_EJECT|SMT_INFO);
				return true;
			}

			//权限判断
			DWORD dwUserRightSend=pIServerUserItemSend->GetUserData()->dwUserRight;
			if ((dwMasterRightRecv==0L)&&(CUserRight::CanWisper(dwUserRightSend)==false))
			{
				TCHAR szMessage[256]=TEXT("您暂时没有向玩家发送私聊消息的权限，只能与管理员私聊！");
				SendRoomMessage(dwSocketID,szMessage,SMT_EJECT);
				return true;
			}

			//转发消息
			SendData(pIServerUserItemSend,MDM_GR_USER,SUB_GR_USER_WISPER,pWisper,wDataSize);
			SendData(pIServerUserItemRecv,MDM_GR_USER,SUB_GR_USER_WISPER,pWisper,wDataSize);

			return true;
		}
	case SUB_GR_USER_RULE:		//用户规则
		{
			//效验参数
			ASSERT(wDataSize==sizeof(CMD_GR_UserRule));
			if (wDataSize!=sizeof(CMD_GR_UserRule)) return true;

			//防作弊判断
			if (m_pGameServiceOption->cbHideUserInfo==TRUE) return true;

			//消息处理
			CMD_GR_UserRule * pUserRule=(CMD_GR_UserRule *)pData;
			pUserRule->szPassword[CountArray(pUserRule->szPassword)-1]=0;

			//获取用户
			IServerUserItem * pIServerUserItem=GetServerUserItem(LOWORD(dwSocketID));
			if (pIServerUserItem==NULL) return true;

			//过滤百人
			if (m_pGameServiceAttrib->wChairCountMax<=MAX_CHAIR_NORMAL)
			{
				//构造规则
				tagUserRule UserRule;
				ZeroMemory(&UserRule,sizeof(UserRule));
				UserRule.bLimitWin=pUserRule->bLimitWin;
				UserRule.bLimitFlee=pUserRule->bLimitFlee;
				UserRule.bLimitScore=pUserRule->bLimitScore;
				UserRule.bCheckSameIP=pUserRule->bCheckSameIP;
				UserRule.wWinRate=pUserRule->wWinRate;
				UserRule.wFleeRate=pUserRule->wFleeRate;
				UserRule.lMaxScore=pUserRule->lMaxScore;
				UserRule.lLessScore=pUserRule->lLessScore;
				if(m_pGameServiceAttrib->wChairCountMax<=MAX_CHAIR_NORMAL)
				{
					UserRule.bPassword=pUserRule->bPassword;
					lstrcpyn(UserRule.szPassword,pUserRule->szPassword,CountArray(UserRule.szPassword));
				}

				//设置规则
				pIServerUserItem->SetUserRule(UserRule);
			}

			return true;
		}

	case SUB_GR_USER_INVITE_REQ:		//邀请用户
		{
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_GR_UserInviteReq));
			if (wDataSize!=sizeof(CMD_GR_UserInviteReq)) return true;

			//消息处理
			CMD_GR_UserInviteReq * pUserInviteReq=(CMD_GR_UserInviteReq *)pData;

			//获取用户
			IServerUserItem * pIServerUserItem=GetServerUserItem(LOWORD(dwSocketID));
			if (pIServerUserItem==NULL) return true;

			//效验状态
			if (pUserInviteReq->wTableID==INVALID_TABLE) return true;
			if (pIServerUserItem->GetTableID()!=pUserInviteReq->wTableID) return true;
			if (pIServerUserItem->GetUserStatus()==US_PLAY) return true;
			if (pIServerUserItem->GetUserStatus()==US_OFFLINE) return true;

			//目标用户
			IServerUserItem * pITargetUserItem=m_ServerUserManager.SearchOnLineUser(pUserInviteReq->dwUserID);
			if (pITargetUserItem==NULL) return true;
			if (pITargetUserItem->GetUserStatus()==US_PLAY) return true;

			//发送消息
			CMD_GR_UserInvite UserInvite;
			memset(&UserInvite,0,sizeof(UserInvite));
			UserInvite.wTableID=pUserInviteReq->wTableID;
			UserInvite.dwUserID=pIServerUserItem->GetUserID();
			SendData(pITargetUserItem,MDM_GR_USER,SUB_GR_USER_INVITE,&UserInvite,sizeof(UserInvite));

			return true;
		}

		//用户取消排队
	case SUB_GR_AUTO_QUEUE_CANCEL:
		{
			//获取用户
			IServerUserItem * pIItem=GetServerUserItem(LOWORD(dwSocketID));
			if (pIItem==NULL) return true;

			if (!m_UserAutoQueue.IsExist(pIItem))
			{
				SendData(pIItem,MDM_GR_USER, SUB_GR_AUTO_QUEUE_CANCEL_FALL);
				return true;
			}
			m_UserAutoQueue.Erase(pIItem);
			m_AndroidAutoQueue.Erase(pIItem);
			SendData(pIItem, MDM_GR_USER, SUB_GR_AUTO_QUEUE_CANCEL_SUCC);
			SendAllQueueCount();
			return true;
		}
	case SUB_GR_AUTO_QUEUE:	//自动排队
		{
			return ArrangeQueue(dwSocketID);;
		}

	case SUB_SENDACTIONINFO:  //转发动作消息
		{
			IServerUserItem * pIServerUserItem=GetServerUserItem(LOWORD(dwSocketID));
			if (pIServerUserItem==NULL) return true;
			return SendUserActionInfo(pIServerUserItem, pData, wDataSize);
		}

	case SUB_GF_QUIT_GAME_SERVER: //用户请求退出游戏服务器
		{
			IServerUserItem * pIServerUserItem=GetServerUserItem(LOWORD(dwSocketID));
			if (pIServerUserItem==NULL) return true;

			if (pIServerUserItem->GetTableID()!=INVALID_TABLE)
			{
				//变量定义
				CTableFrame * pTableFrame=(m_pTableFrame+pIServerUserItem->GetTableID());
				pTableFrame->PerformStandUpAction(pIServerUserItem);
			}

			//SendData(pIServerUserItem, MDM_GR_USER, SUB_GF_QUIT_GAME_SERVER_SUCC);
			UnLockUserItem(pIServerUserItem);
			m_pITCPNetworkEngine->SendData(dwSocketID,  MDM_GR_USER, SUB_GF_QUIT_GAME_SERVER_SUCC);

#if _SHOW_MESSAGE_NOR
			char info[BUFSIZ] = {0};
			tagServerUserData * pUserData=pIServerUserItem->GetUserData();
			if (pUserData != NULL)
			{
				sprintf(info, _T("%s SUB_GF_QUIT_GAME_SERVER 用户请求退出游戏服务器，退出队列: 队列共：%i人"),  pUserData->szAccounts, m_UserAutoQueue.Size());
				CTraceService::TraceString(info, TraceLevel_Normal);
			}
#endif
			m_UserAutoQueue.Erase(pIServerUserItem);
			m_AndroidAutoQueue.Erase(pIServerUserItem);
			return true;
		}
	}

	__LEAVE_FUNCTION_LOG_CATCH

	return true;
}

bool CAttemperEngineSink::ArrangeQueue(DWORD dwSocketID)
{
	__ENTER_FUNCTION_LOG_CATCH

	//获取用户
	IServerUserItem * pIServerUserItem=GetServerUserItem(LOWORD(dwSocketID));
	if (pIServerUserItem==NULL) return true;

	if (!CheckUserPlaying(pIServerUserItem))
	{
		return true;
	}

	if (!JudgeUserFitThisRoom(pIServerUserItem))
	{
		return true;
	}

	//检查以排队用户
	if (!InsertUserInQueue(pIServerUserItem))
	{
		goto Finish;
	}


	//坐下位置
	//if (m_pGameServiceAttrib->wChairCountMin <=1)  //如果只有一个人的话就退出
	//{
	//	 CTraceService::TraceString(_T("一个人可以玩游戏吗？"),TraceLevel_Normal);
	//	return true;
	//}

	//unsigned int busyDesk = m_BusyDeskStat.GetAverage();
	//if (busyDesk < m_pGameServiceOption->wAndroidTableCount)
	//{
	//	
	//}


	//bool fixChair = (m_pGameServiceAttrib->wChairCountMin == m_pGameServiceAttrib->wChairCountMax ? true : false) ;
	/*if (fixChair)
	{*/
	if(!PerformSitDownChair())
	{
		return true;
	}
	/*}else
	{
	if(!PerformSitDownUnFixChair())
	{
	return true;
	}
	}*/

Finish:
	//构造数据
	CMD_GR_AutoQueue AutoQueue;
	ZeroMemory(&AutoQueue,sizeof(AutoQueue));
	AutoQueue.iAutoQueueNum = (int)m_UserAutoQueue.Size();

	//发送数据
	WORD wSendSize=sizeof(AutoQueue);
	SendData(pIServerUserItem,MDM_GR_USER,SUB_GR_AUTO_QUEUE_SUCC,&AutoQueue,wSendSize);
	//想所有的用法发送队列人数
	SendAllQueueCount();

	__LEAVE_FUNCTION_LOG_CATCH

	return true;
}

//彭文添加
//判断这个玩家是否符合房间的条件
bool CAttemperEngineSink::JudgeUserFitThisRoom(IServerUserItem * pIServerUserItem)
{
	__ENTER_FUNCTION_LOG_CATCH


	ASSERT(pIServerUserItem != NULL);
	tagServerUserData * pUserData=pIServerUserItem->GetUserData();
	//比赛判断
	if (m_pGameServiceOption->wServerType==GAME_GENRE_MATCH)
	{
		//比赛权限
		if (CUserRight::IsMatchUser(pUserData->dwUserRight)==false)
		{
			TCHAR szDescribe[128]=TEXT("");
			lstrcpyn(szDescribe,TEXT("这是游戏比赛房间，您不是比赛选手，不能坐到此位置上！ "),CountArray(szDescribe));
			SendWarningPacket(pIServerUserItem, MDM_GR_USER, SUB_GR_AUTO_QUEUE_FAIL, szDescribe);
			return false;
		}


		//对局局数
		if (m_pGameServiceOption->lMatchDraw>0)
		{
			LONG lPlayCount=pUserData->UserScoreInfo.lWinCount+pUserData->UserScoreInfo.lLostCount+pUserData->UserScoreInfo.lDrawCount;
			if (lPlayCount>=m_pGameServiceOption->lMatchDraw)
			{
				TCHAR szDescribe[128]=TEXT("");
				lstrcpyn(szDescribe,TEXT("恭喜您，您的比赛局数已经完成了，不需要再继续比赛，请耐心等待赛果公布！ "),CountArray(szDescribe));
				SendWarningPacket(pIServerUserItem, MDM_GR_USER, SUB_GR_AUTO_QUEUE_FAIL, szDescribe);
				return false;
			}
		}
	}
	else
	{
		//权限判断
		if (CUserRight::CanPlay(pUserData->dwUserRight)==false)
		{
			//发送消息 
			TCHAR szDescribe[128] = TEXT("抱歉，您没有进行游戏的权限，若需要帮助，请联系游戏客服咨询！");
			SendWarningPacket(pIServerUserItem, MDM_GR_USER, SUB_GR_AUTO_QUEUE_FAIL, szDescribe);
			return false;
		}

		if (pUserData->UserScoreInfo.lGold < m_pGameServiceOption->lRestrictScore)
		{
			//发送消息
			TCHAR szDescribe[128]=TEXT("");
			if (m_pGameServiceOption->wServerType==GAME_GENRE_GOLD)
			{
				_snprintf(szDescribe,sizeof(szDescribe),TEXT("加入游戏至少需要 %i 个金币，您的金币不够，不能加入！"),m_pGameServiceOption->lEnterMoney);
			}

			SendWarningPacket(pIServerUserItem, MDM_GR_USER, SUB_GR_NO_GOLD, szDescribe);
			return false;
		}
	}

	__LEAVE_FUNCTION_LOG_CATCH

	return true;
}

//彭文添加    
//发送警告消息
void CAttemperEngineSink::SendWarningPacket(IServerUserItem * pIServerUserItem, int MDM_MES, int SUB_MES, TCHAR* pMes)
{
	CMD_GR_SendWarning SitFailed;
	memset(&SitFailed,0,sizeof(SitFailed));
	strncpy(SitFailed.szWarningMessage,  pMes, sizeof(SitFailed.szWarningMessage) - 1);
#if _SHOW_MESSAGE_NOR
	CTraceService::TraceString(SitFailed.szWarningMessage, TraceLevel_Normal);
#endif
	//发送数据
	WORD wSendSize=sizeof(SitFailed)-sizeof(SitFailed.szWarningMessage)+CountStringBuffer(SitFailed.szWarningMessage);
	SendData(pIServerUserItem,MDM_MES,SUB_MES,&SitFailed,wSendSize);
}


//彭文添加
//执行排队座椅子操作
bool CAttemperEngineSink::PerformSitDownChair()
{
	__ENTER_FUNCTION_LOG_CATCH

	for (int i=1; i<=m_pGameServiceAttrib->wChairCountMax; ++i)
	{
		PerformSitDownOnDesk(i);
	}

	__LEAVE_FUNCTION_LOG_CATCH

	return true;
}

//彭文添加
//向所有的用户发送排队队列的人数
void CAttemperEngineSink::SendAllQueueCount()
{
	__ENTER_FUNCTION_LOG_CATCH

	WORD wEnumIndex=0;
	while (true)
	{
		IServerUserItem*  pIServerUserItemSend=m_ServerUserManager.EnumOnLineUser(wEnumIndex++);
		if (pIServerUserItemSend==NULL) break;
		SendQueueCount(pIServerUserItemSend);
	}

	wEnumIndex=0;
	while (true)
	{
		IServerUserItem*  pIServerUserItemSend=m_ServerUserManager.EnumOffLineUser(wEnumIndex++);
		if (pIServerUserItemSend==NULL) break;
		SendQueueCount(pIServerUserItemSend);
	}

	__LEAVE_FUNCTION_LOG_CATCH
}

//发送队列人数人数
void  CAttemperEngineSink::SendQueueCount(IServerUserItem * pIServerUserItem)
{
	__ENTER_FUNCTION_LOG_CATCH

	//构造数据
	CMD_GR_AutoQueue AutoQueue;
	ZeroMemory(&AutoQueue,sizeof(AutoQueue));
	AutoQueue.iAutoQueueNum = (int)m_UserAutoQueue.Size();

	//发送数据
	WORD wSendSize=sizeof(AutoQueue);
	SendData(pIServerUserItem,MDM_GR_USER,SUB_GR_QUEUE_COUNT,&AutoQueue,wSendSize);

	__LEAVE_FUNCTION_LOG_CATCH
}



//管理消息处理
bool CAttemperEngineSink::OnSocketMainManager(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	__ENTER_FUNCTION_LOG_CATCH

	//消息处理
	switch (wSubCmdID)
	{
	case SUB_GR_SEND_WARNING:	//警告消息
		{
			//效验参数
			CMD_GR_SendWarning * pSendWarning=(CMD_GR_SendWarning *)pData;
			ASSERT(wDataSize>=(sizeof(CMD_GR_SendWarning)-sizeof(pSendWarning->szWarningMessage)));
			ASSERT(wDataSize==(sizeof(CMD_GR_SendWarning)-sizeof(pSendWarning->szWarningMessage)+pSendWarning->wChatLength));
			if (wDataSize<(sizeof(CMD_GR_SendWarning)-sizeof(pSendWarning->szWarningMessage)))
				return true;
			if (wDataSize!=(sizeof(CMD_GR_SendWarning)-sizeof(pSendWarning->szWarningMessage)+pSendWarning->wChatLength))
				return true;

			//管理用户
			IServerUserItem * pIServerUserItemSend=GetServerUserItem(LOWORD(dwSocketID));
			if (pIServerUserItemSend==NULL) return true;

			//权限判断
			DWORD dwMasterRight=pIServerUserItemSend->GetUserData()->dwMasterRight;
			if (CUserRight::CanSendWarning(dwMasterRight)==false) return true;

			//目标用户
			IServerUserItem * pIServerUserItemRecv=m_ServerUserManager.SearchOnLineUser(pSendWarning->dwTargetUserID);
			if (pIServerUserItemRecv==NULL) return true;

			//发送消息
			SendGameMessage(pIServerUserItemRecv,pSendWarning->szWarningMessage,SMT_INFO|SMT_EJECT);
			//SendRoomMessage(pIServerUserItemRecv,pSendWarning->szWarningMessage,SMT_INFO|SMT_EJECT);

			return true;
		}
	case SUB_GR_LOOK_USER_IP:	//查看地址
		{
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_GR_LookUserIP));
			if (wDataSize!=sizeof(CMD_GR_LookUserIP)) return true;

			//消息处理
			CMD_GR_LookUserIP * pLookUserIP=(CMD_GR_LookUserIP *)pData;

			//管理用户
			IServerUserItem * pIServerUserItemSend=GetServerUserItem(LOWORD(dwSocketID));
			if (pIServerUserItemSend==NULL) return true;

			//权限判断
			DWORD dwMasterRight=pIServerUserItemSend->GetUserData()->dwMasterRight;
			if (CUserRight::CanSeeUserIP(dwMasterRight)==false) return true;

			//目标用户
			IServerUserItem * pIServerUserItemRecv=m_ServerUserManager.SearchOnLineUser(pLookUserIP->dwTargetUserID);
			if (pIServerUserItemRecv==NULL) return true;

			//变量定义
			DWORD dwUserIP=pIServerUserItemRecv->GetClientIP();

			//构造信息
			TCHAR szMessage[128]=TEXT("");
			BYTE * pcbUserIP=(BYTE *)(&dwUserIP);
			_snprintf(szMessage,CountArray(szMessage),TEXT("[ %s ]：%d.%d.%d.%d"),pIServerUserItemRecv->GetAccounts(),
				pcbUserIP[0],pcbUserIP[1],pcbUserIP[2],pcbUserIP[3]);

			//发送消息
			SendRoomMessage(pIServerUserItemSend,szMessage,SMT_INFO|SMT_EJECT);

			return true;
		}
	case SUB_GR_KILL_USER:		//踢出用户
		{
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_GR_KillUser));
			if (wDataSize!=sizeof(CMD_GR_KillUser)) return true;

			//消息处理
			CMD_GR_KillUser * pKillUser=(CMD_GR_KillUser *)pData;

			//管理用户
			IServerUserItem * pIServerUserItemSend=GetServerUserItem(LOWORD(dwSocketID));
			if (pIServerUserItemSend==NULL) return true;

			//权限判断
			DWORD dwMasterRight=pIServerUserItemSend->GetUserData()->dwMasterRight;
			if (CUserRight::CanCutUser(dwMasterRight)==false) return true;

			//目标用户
			IServerUserItem * pIServerUserItemRecv=m_ServerUserManager.SearchOnLineUser(pKillUser->dwTargetUserID);
			if (pIServerUserItemRecv==NULL) return true;

			if (pIServerUserItemRecv->IsAndroidUser()==FALSE)
			{
				//发送消息
				if (TRUE == pKillUser->bCutOnline)
				{
					LPCTSTR szMessage=TEXT("亲爱的玩家，您涉嫌违规操作，请您即时联系游戏客服！");
					SendRoomMessage(pIServerUserItemRecv,szMessage,SMT_INFO|SMT_EJECT|SMT_INTERMIT_LINE|SMT_CUSTOMER_FRAME);
				}
				else
				{
					LPCTSTR szMessage=TEXT("亲爱的玩家，您涉嫌违规操作，请您即时联系游戏客服！");
					SendRoomMessage(pIServerUserItemRecv,szMessage,SMT_INFO|SMT_EJECT|SMT_CLOSE_ROOM|SMT_CUSTOMER_FRAME);
				}
			}
			else
			{
				WORD wBindIndex=pIServerUserItemRecv->GetUserIndex();
				tagConnectItemInfo *pConnectItemInfo=m_pAndroidParameter+(wBindIndex-INDEX_ANDROID);
				if (pConnectItemInfo!=NULL&&pConnectItemInfo->pIServerUserItem!=NULL) m_AndroidUserManager.DeleteAndroidUserItem(pConnectItemInfo->dwSocketID);
			}

			return true;
		}
	case SUB_GR_LIMIT_ACCOUNS:	//禁用帐户
		{
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_GR_LimitAccounts));
			if (wDataSize!=sizeof(CMD_GR_LimitAccounts)) return true;

			//消息处理
			CMD_GR_LimitAccounts * pLimitAccounts=(CMD_GR_LimitAccounts *)pData;

			//管理用户
			IServerUserItem * pIServerUserItemSend=GetServerUserItem(LOWORD(dwSocketID));
			if (pIServerUserItemSend==NULL) return true;

			//权限判断
			DWORD dwMasterRight=pIServerUserItemSend->GetUserData()->dwMasterRight;
			if (CUserRight::CanForbidAccounts(dwMasterRight)==false) return true;

			//目标用户
			IServerUserItem * pIServerUserItemRecv=m_ServerUserManager.SearchOnLineUser(pLimitAccounts->dwTargetUserID);
			if (pIServerUserItemRecv==NULL) return true;

			//投递数据库
			DBR_GR_LimitAccounts LimitAccounts;
			LimitAccounts.dwUserID=pLimitAccounts->dwTargetUserID;
			LimitAccounts.dwMasterUserID=pIServerUserItemSend->GetUserID();
			LimitAccounts.dwMasterClientIP=pIServerUserItemSend->GetClientIP();
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_LIMIT_ACCOUNTS,0,&LimitAccounts,sizeof(LimitAccounts));

			//发送消息
			LPCTSTR szMessage=TEXT("抱歉，您的帐号被冻结了，若有任何疑问，请联系游戏客服！");
			SendRoomMessage(pIServerUserItemRecv,szMessage,SMT_INFO|SMT_EJECT|SMT_CLOSE_ROOM|SMT_CUSTOMER_FRAME);

			return true;
		}
	case SUB_GR_SET_USER_RIGHT:	//用户权限
		{
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_GR_SetUserRight));
			if (wDataSize!=sizeof(CMD_GR_SetUserRight)) return true;

			//消息处理
			CMD_GR_SetUserRight * pSetUserRight=(CMD_GR_SetUserRight *)pData;

			//管理用户
			IServerUserItem * pIServerUserItemSend=GetServerUserItem(LOWORD(dwSocketID));
			if (pIServerUserItemSend==NULL) return true;

			//目标用户
			IServerUserItem * pIServerUserItemRecv=m_ServerUserManager.SearchOnLineUser(pSetUserRight->dwTargetUserID);
			if (pIServerUserItemRecv==NULL) return true;

			//权限判断
			DWORD dwMasterRight=pIServerUserItemSend->GetUserData()->dwMasterRight;
			if (dwMasterRight==0L) return true;

			//获取权限
			tagServerUserData * pUserData=pIServerUserItemRecv->GetUserData();

			//权限设置
			if (CUserRight::CanLimitPlay(dwMasterRight)==true)
			{
				if (pSetUserRight->cbLimitPlayGame==TRUE) pUserData->dwUserRight|=UR_CANNOT_PLAY;
				else pUserData->dwUserRight&=~UR_CANNOT_PLAY;
			}
			if (CUserRight::CanLimitLookon(dwMasterRight)==true)
			{
				if (pSetUserRight->cbLimitLookonGame==TRUE) pUserData->dwUserRight|=UR_CANNOT_LOOKON;
				else pUserData->dwUserRight&=~UR_CANNOT_LOOKON;
			}
			if (CUserRight::CanLimitWisper(dwMasterRight)==true)
			{
				if (pSetUserRight->cbLimitSendWisper==TRUE) pUserData->dwUserRight|=UR_CANNOT_WISPER;
				else pUserData->dwUserRight&=~UR_CANNOT_WISPER;
			}
			if (CUserRight::CanLimitRoomChat(dwMasterRight)==true)
			{
				if (pSetUserRight->cbLimitRoomChat==TRUE) pUserData->dwUserRight|=UR_CANNOT_ROOM_CHAT;
				else pUserData->dwUserRight&=~UR_CANNOT_ROOM_CHAT;
			}
			if (CUserRight::CanLimitGameChat(dwMasterRight)==true)
			{
				if (pSetUserRight->cbLimitGameChat==TRUE) pUserData->dwUserRight|=UR_CANNOT_GAME_CHAT;
				else pUserData->dwUserRight&=~UR_CANNOT_GAME_CHAT;
			}

			//设置权限
			DBR_GR_SetUserRight SetUserRight;
			SetUserRight.dwUserRight=pUserData->dwUserRight;
			SetUserRight.dwUserID=pSetUserRight->dwTargetUserID;
			SetUserRight.dwMasterUserID=pIServerUserItemSend->GetUserID();
			SetUserRight.dwMasterClientIP=pIServerUserItemSend->GetClientIP();
			SetUserRight.cbGame=(CUserRight::CanBindGame(dwMasterRight)==true)?TRUE:FALSE;
			SetUserRight.cbAccounts=(CUserRight::CanBindGlobal(dwMasterRight)==true)?TRUE:FALSE;

			//投递数据
			if ((SetUserRight.cbAccounts==TRUE)||(SetUserRight.cbGame==TRUE))
			{
				m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_SET_USER_RIGHT,0,&SetUserRight,sizeof(SetUserRight));
			}

			return true;
		}
	case SUB_GR_SEND_MESSAGE:	//发送消息
		{
			//效验参数
			CMD_GR_SendMessage * pSendMessage=(CMD_GR_SendMessage *)pData;
			ASSERT(wDataSize>=(sizeof(CMD_GR_SendMessage)-sizeof(pSendMessage->szSystemMessage)));
			ASSERT(wDataSize==(sizeof(CMD_GR_SendMessage)-sizeof(pSendMessage->szSystemMessage)+pSendMessage->wChatLength));
			if (wDataSize<(sizeof(CMD_GR_SendMessage)-sizeof(pSendMessage->szSystemMessage))) return true;
			if (wDataSize!=(sizeof(CMD_GR_SendMessage)-sizeof(pSendMessage->szSystemMessage)+pSendMessage->wChatLength)) return true;

			//管理用户
			IServerUserItem * pIServerUserItemSend=GetServerUserItem(LOWORD(dwSocketID));
			if (pIServerUserItemSend==NULL) return true;

			//权限判断
			DWORD dwMasterRight=pIServerUserItemSend->GetUserData()->dwMasterRight;
			if (CUserRight::CanIssueMessage(dwMasterRight)==false) return true;

			//发送消息
			if (pSendMessage->cbGame==TRUE) SendGameMessage(INVALID_WORD,pSendMessage->szSystemMessage,SMT_INFO);
			if (pSendMessage->cbRoom==TRUE) SendRoomMessage(INVALID_WORD,pSendMessage->szSystemMessage,SMT_INFO);

			return true;
		}
	case SUB_GR_OPTION_SERVER:	//房间设置
		{
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_GR_OptionServer));
			if (wDataSize!=sizeof(CMD_GR_OptionServer)) return true;

			//消息处理
			CMD_GR_OptionServer * pOptionServer=(CMD_GR_OptionServer *)pData;

			//管理用户
			IServerUserItem * pIServerUserItemSend=GetServerUserItem(LOWORD(dwSocketID));
			if (pIServerUserItemSend==NULL) return true;

			//权限判断
			DWORD dwMasterRight=pIServerUserItemSend->GetUserData()->dwMasterRight;
			if (CUserRight::CanServerOption(dwMasterRight)==false) return true;

			//大厅聊天
			if (pOptionServer->cbOptionFlags==OSF_ROOM_CHAT)
			{
				m_bAllowRoomChat=(pOptionServer->cbOptionValue==TRUE)?true:false;
			}

			//游戏聊天
			if (pOptionServer->cbOptionFlags==OSF_GAME_CHAT)
			{
				m_bAllowGameChat=(pOptionServer->cbOptionValue==TRUE)?true:false;
			}

			//大厅私聊
			if (pOptionServer->cbOptionFlags==OSF_ROOM_WISPER)
			{
				m_bAllowWisper=(pOptionServer->cbOptionValue==TRUE)?true:false;
			}

			//进入游戏
			if (pOptionServer->cbOptionFlags==OSF_ENTER_GAME)
			{
				m_bAllowEnterGame=(pOptionServer->cbOptionValue==TRUE)?true:false;
			}

			//进入房间
			if (pOptionServer->cbOptionFlags==OSF_ENTER_ROOM)
			{
				m_bAllowEnterRoom=(pOptionServer->cbOptionValue==TRUE)?true:false;
			}

			//即将关闭
			if (pOptionServer->cbOptionFlags==OSF_SHALL_CLOSE)
			{
				m_bShallClose=(pOptionServer->cbOptionValue==TRUE)?true:false;
			}

			//发送消息
			LPCTSTR pszMessage=TEXT("服务器房间配置参数选项设置成功！");
			SendRoomMessage(pIServerUserItemSend,pszMessage,SMT_INFO);

			return true;
		}
	}

	__LEAVE_FUNCTION_LOG_CATCH

	return true;
}

//框架消息处理
bool CAttemperEngineSink::OnSocketMainFrame(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//获取用户
	IServerUserItem * pIServerUserItem=GetServerUserItem(LOWORD(dwSocketID));
	if (pIServerUserItem==NULL) return true;

	//处理过虑
	tagServerUserData * pUserData=pIServerUserItem->GetUserData();
	if ((pUserData->wTableID==INVALID_TABLE)||(pUserData->wChairID==INVALID_CHAIR)) return true;

	//消息处理 
	CTableFrame * pTableFrame=(m_pTableFrame+pUserData->wTableID);
	return pTableFrame->OnEventSocketFrame(wSubCmdID,pData,wDataSize,pIServerUserItem);
}

//游戏消息处理
bool CAttemperEngineSink::OnSocketMainGame(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	__ENTER_FUNCTION_LOG_CATCH

	//获取用户
	IServerUserItem * pIServerUserItem=GetServerUserItem(LOWORD(dwSocketID));
	if (pIServerUserItem==NULL) return true;

	//处理过虑
	tagServerUserData * pUserData=pIServerUserItem->GetUserData();
	if ((pUserData->wTableID==INVALID_TABLE)||(pUserData->wChairID==INVALID_CHAIR)) return true;

	//消息处理 
	CTableFrame * pTableFrame=(m_pTableFrame+pUserData->wTableID);
	//return pTableFrame->OnEventSocketGame(wSubCmdID,pData,wDataSize,pIServerUserItem); //彭文修改
	//彭文添加
	pTableFrame->OnEventSocketGame(wSubCmdID,pData,wDataSize,pIServerUserItem); 

	__LEAVE_FUNCTION_LOG_CATCH

	return true;
}

//视频消息处理
bool CAttemperEngineSink::OnSocketMainVideo(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	__ENTER_FUNCTION_LOG_CATCH

	//获取用户
	IServerUserItem * pIServerUserItem=GetServerUserItem(LOWORD(dwSocketID));
	if (pIServerUserItem==NULL) return true;

	//处理过虑
	tagServerUserData * pUserData=pIServerUserItem->GetUserData();
	if ((pUserData->wTableID==INVALID_TABLE)||(pUserData->wChairID==INVALID_CHAIR)) return true;

	//消息处理 
	CTableFrame * pTableFrame=(m_pTableFrame+pUserData->wTableID);
	pTableFrame->OnEventSocketVideo(wSubCmdID,pData,wDataSize,pIServerUserItem);

	__LEAVE_FUNCTION_LOG_CATCH

	return true;
}

//礼物消息处理
bool CAttemperEngineSink::OnSocketPresent(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	__ENTER_FUNCTION_LOG_CATCH

	//获取用户
	IServerUserItem * pIServerUserItem=GetServerUserItem(LOWORD(dwSocketID));
	if (pIServerUserItem==NULL) return true;

	//比赛房间
	if (m_pGameServiceOption->wServerType==GAME_GENRE_MATCH)
	{
		SendProMessage(pIServerUserItem,TEXT("比赛房间不可以使用此功能！"),SMT_INFO|SMT_EJECT);
		return true;
	}


	//消息处理
	bool bReturnValue = false;
	switch ( wSubCmdID )
	{
	case SUB_GF_PROP_BUGLE:				//喇叭事件
		{
			CMD_GF_BugleProperty *pTemp = (CMD_GF_BugleProperty*)pData;
			m_cbSendLocation = pTemp->cbSendLocation;
			bReturnValue = OnEventBugle(pData,wDataSize,pIServerUserItem);
			break;
		}
	}

	//初始场所
	m_cbSendLocation = 0;

	__LEAVE_FUNCTION_LOG_CATCH

	return true;
}

//银行消息处理
bool CAttemperEngineSink::OnSocketBank(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{

	__ENTER_FUNCTION_LOG_CATCH

	//获取用户
	IServerUserItem * pIServerUserItem=GetServerUserItem(LOWORD(dwSocketID));
	if (pIServerUserItem==NULL) return true;

	//比赛房间
	if (m_pGameServiceOption->wServerType==GAME_GENRE_MATCH)
	{
		SendProMessage(pIServerUserItem,TEXT("比赛房间不可以使用此功能！"),SMT_INFO|SMT_EJECT);
		return true;
	}

	//消息处理
	switch ( wSubCmdID )
	{
	case SUB_GF_BANK_GET:					//提取事件
		{
			OnEventBankDrawoutGold(pData,wDataSize,dwSocketID);
			return true;
		}
	case SUB_GF_BANK_STORAGE:				//存储事件
		{
			OnEventBankStorage(pData,wDataSize,dwSocketID);
			return true;
		}
	}

	__LEAVE_FUNCTION_LOG_CATCH

	return true;
}

//发送用户
bool CAttemperEngineSink::SendUserItem(IServerUserItem * pIServerUserItem, DWORD dwSocketID)
{
	__ENTER_FUNCTION_LOG_CATCH

	//效验参数
	ASSERT(pIServerUserItem!=NULL);

	//构造数据
	BYTE cbBuffer[SOCKET_PACKET];
	tagUserInfoHead * pUserInfoHead=(tagUserInfoHead *)cbBuffer;
	tagServerUserData * pUserData=pIServerUserItem->GetUserData();

	//填写数据
	memset(pUserInfoHead,0,sizeof(tagUserInfoHead));
	pUserInfoHead->wFaceID=pUserData->wFaceID;
	pUserInfoHead->dwCustomFaceVer=pUserData->dwCustomFaceVer;
	pUserInfoHead->wTableID=pUserData->wTableID;
	pUserInfoHead->wChairID=pUserData->wChairID;
	pUserInfoHead->cbGender=pUserData->cbGender;
	pUserInfoHead->cbUserStatus=pUserData->cbUserStatus;
	pUserInfoHead->dwUserID=pUserData->dwUserID;
	pUserInfoHead->dwGameID=pUserData->dwGameID;
	pUserInfoHead->dwGroupID=pUserData->dwGroupID;
	pUserInfoHead->dwUserRight=pUserData->dwUserRight;
	pUserInfoHead->lLoveliness=pUserData->lLoveliness;
	pUserInfoHead->dwMasterRight=pUserData->dwMasterRight;
	pUserInfoHead->cbGender=pUserData->cbGender;
	pUserInfoHead->cbMemberOrder=pUserData->cbMemberOrder;
	pUserInfoHead->cbMasterOrder=pUserData->cbMasterOrder;
	pUserInfoHead->UserScoreInfo=pUserData->UserScoreInfo;
	pUserInfoHead->UserScoreInfo.lGold=pUserData->UserScoreInfo.lGold + pUserData->lStorageGold;
	memcpy(pUserInfoHead->szAccounts, pUserData->szAccounts, sizeof(pUserInfoHead->szAccounts));//Author<cxf>;DateTime<2012-03-15>
	//叠加数据
	CSendPacketHelper SendPacket(cbBuffer+sizeof(tagUserInfoHead),sizeof(cbBuffer)-sizeof(tagUserInfoHead));
	//彭文删除
	//SendPacket.AddPacket(pUserData->szAccounts,CountStringBuffer(pUserData->szAccounts),DTP_USER_ACCOUNTS);
	//彭文修改 将用户的昵称作为账号传出来  因为客户端不需要账号只需要昵称就可以
	SendPacket.AddPacket(pUserData->szNickName,CountStringBuffer(pUserData->szNickName),DTP_USER_ACCOUNTS);

	if (pUserData->szUnderWrite[0]!=0) SendPacket.AddPacket(pUserData->szUnderWrite,CountStringBuffer(pUserData->szUnderWrite),DTP_UNDER_WRITE);
	if (pUserData->szGroupName[0]!=0) SendPacket.AddPacket(pUserData->szGroupName,CountStringBuffer(pUserData->szGroupName),DTP_USER_GROUP_NAME);

	//发送数据
	WORD wSendLength=sizeof(tagUserInfoHead)+SendPacket.GetDataSize();
	if (LOWORD(dwSocketID)!=INVALID_WORD) 
	{
		if (LOWORD(dwSocketID)>=INDEX_ANDROID)
		{
			m_AndroidUserManager.SendDataToClient(dwSocketID,MDM_GR_USER,SUB_GR_USER_COME,cbBuffer,wSendLength);
		}
		else m_pITCPNetworkEngine->SendData(dwSocketID,MDM_GR_USER,SUB_GR_USER_COME,cbBuffer,wSendLength);
	}
	else m_pITCPNetworkEngine->SendDataBatch(MDM_GR_USER,SUB_GR_USER_COME,cbBuffer,wSendLength);

	__LEAVE_FUNCTION_LOG_CATCH

	return true;
}

//发送信息 道具在这里发送的
bool CAttemperEngineSink::SendGameServerInfo(IServerUserItem * pIServerUserItem, DWORD dwSocketID)
{
	__ENTER_FUNCTION_LOG_CATCH

	//发送登录成功
	CMD_GR_LogonSuccess LogonSuccess;
	memset(&LogonSuccess,0,sizeof(LogonSuccess));
	LogonSuccess.dwUserID=pIServerUserItem->GetUserID();

	if (LOWORD(dwSocketID)>=INDEX_ANDROID)
	{
		m_AndroidUserManager.SendDataToClient(dwSocketID,MDM_GR_LOGON,SUB_GR_LOGON_SUCCESS,&LogonSuccess,sizeof(LogonSuccess));
	}
	else m_pITCPNetworkEngine->SendData(dwSocketID,MDM_GR_LOGON,SUB_GR_LOGON_SUCCESS,&LogonSuccess,sizeof(LogonSuccess));

	//发送配置信息
	BYTE cbBuffer[SOCKET_PACKET];
	CMD_GR_ServerInfo * pServerInfo=(CMD_GR_ServerInfo *)cbBuffer;
	memset(pServerInfo,0,sizeof(CMD_GR_ServerInfo));
	pServerInfo->wKindID=m_pGameServiceAttrib->wKindID;
	pServerInfo->wGameGenre=m_pGameServiceOption->wServerType;
	pServerInfo->wTableCount=m_pGameServiceOption->wTableCount;
	pServerInfo->wChairCount=m_pGameServiceAttrib->wChairCountMax;
	pServerInfo->dwVideoAddr=m_pGameServiceOption->dwVideoServer;
	pServerInfo->cbHideUserInfo=m_pGameServiceOption->cbHideUserInfo;

	if (LOWORD(dwSocketID)>=INDEX_ANDROID)
	{
		m_AndroidUserManager.SendDataToClient(dwSocketID,MDM_GR_INFO,SUB_GR_SERVER_INFO,cbBuffer,sizeof(CMD_GR_ServerInfo));
	}
	else m_pITCPNetworkEngine->SendData(dwSocketID,MDM_GR_INFO,SUB_GR_SERVER_INFO,cbBuffer,sizeof(CMD_GR_ServerInfo));

	//发送列表解释
	if (LOWORD(dwSocketID)>=INDEX_ANDROID)
	{
		m_AndroidUserManager.SendDataToClient(dwSocketID,MDM_GR_INFO,SUB_GR_COLUMN_INFO,&m_ColumnInfo,m_wColumnInfoSize);
	}
	else m_pITCPNetworkEngine->SendData(dwSocketID,MDM_GR_INFO,SUB_GR_COLUMN_INFO,&m_ColumnInfo,m_wColumnInfoSize);

	//发送配置完成
	if (LOWORD(dwSocketID)>=INDEX_ANDROID)
	{
		m_AndroidUserManager.SendDataToClient(dwSocketID,MDM_GR_INFO,SUB_GR_CONFIG_FINISH,NULL,0);
	}
	else m_pITCPNetworkEngine->SendData(dwSocketID,MDM_GR_INFO,SUB_GR_CONFIG_FINISH);

	//发送自己信息
	SendUserItem(pIServerUserItem,dwSocketID);

	//发送在线用户
	WORD wEnumIndex=0;
	IServerUserItem * pIServerUserItemSend=NULL;
	while (true)
	{
		pIServerUserItemSend=m_ServerUserManager.EnumOnLineUser(wEnumIndex++);
		if (pIServerUserItemSend==NULL) break;
		if (pIServerUserItemSend==pIServerUserItem) continue;
		SendUserItem(pIServerUserItemSend,dwSocketID);
	}

	//发送断线用户
	wEnumIndex=0;
	while (true)
	{
		pIServerUserItemSend=m_ServerUserManager.EnumOffLineUser(wEnumIndex++);
		if (pIServerUserItemSend==NULL) break;
		if (pIServerUserItemSend==pIServerUserItem) continue;
		SendUserItem(pIServerUserItemSend,dwSocketID);
	}

	//发送桌子信息
	CMD_GR_TableInfo TableInfo;
	TableInfo.wTableCount=m_pGameServiceOption->wTableCount;
	ASSERT(TableInfo.wTableCount<CountArray(TableInfo.TableStatus));
	for (WORD i=0;i<TableInfo.wTableCount;i++)
	{
		CTableFrame * pTableFrame=(m_pTableFrame+i);
		TableInfo.TableStatus[i].bTableLock=pTableFrame->IsTableLocked()?TRUE:FALSE;
		TableInfo.TableStatus[i].bPlayStatus=pTableFrame->IsGameStarted()?TRUE:FALSE;
	}
	WORD wSendSize=sizeof(TableInfo)-sizeof(TableInfo.TableStatus)+TableInfo.wTableCount*sizeof(TableInfo.TableStatus[0]);

	if (LOWORD(dwSocketID)>=INDEX_ANDROID)
	{
		m_AndroidUserManager.SendDataToClient(dwSocketID,MDM_GR_STATUS,SUB_GR_TABLE_INFO,&TableInfo,wSendSize);
	}
	else m_pITCPNetworkEngine->SendData(dwSocketID,MDM_GR_STATUS,SUB_GR_TABLE_INFO,&TableInfo,wSendSize);


	//发送完成
	if (LOWORD(dwSocketID)>=INDEX_ANDROID)
	{
		m_AndroidUserManager.SendDataToClient(dwSocketID,MDM_GR_LOGON,SUB_GR_LOGON_FINISH,NULL,0);
	}
	else m_pITCPNetworkEngine->SendData(dwSocketID,MDM_GR_LOGON,SUB_GR_LOGON_FINISH);

	__LEAVE_FUNCTION_LOG_CATCH

	return true;
}


//列表消息处理
bool CAttemperEngineSink::OnCenterMainServerList(CMD_Command Command, VOID * pData, WORD wDataSize)
{
	__ENTER_FUNCTION_LOG_CATCH

	switch (Command.wSubCmdID)
	{
	case SUB_CS_LIST_INFO:		//列表信息
		{
			//效验参数
			ASSERT(wDataSize==sizeof(CMD_CS_ListInfo));
			if (wDataSize!=sizeof(CMD_CS_ListInfo)) return true;

			//消息处理
			m_ServerList.ResetServerListBuffer();

			return true;
		}
	case SUB_CS_LIST_TYPE:		//种类信息
		{
			//效验参数
			ASSERT(wDataSize>=sizeof(tagGameType));
			ASSERT(wDataSize%sizeof(tagGameType)==0);
			if (wDataSize!=sizeof(tagGameType)) return true;

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
			if (wDataSize!=sizeof(tagGameKind)) return true;


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
			if (wDataSize!=sizeof(tagGameStation)) return true;
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
			if (wDataSize!=sizeof(tagGameServer)) return true;
			//消息处理
			DWORD dwCount=wDataSize/sizeof(tagGameServer);
			tagGameServer* pGameServer=(tagGameServer *)pData;
			m_ServerList.AppendGameServerBuffer(pGameServer,dwCount);

			return true;
		}
	case SUB_CS_LIST_FINISH:	//更新完成
		{
			//激活缓冲
			m_ServerList.ActiveServerListBuffer();

			//更新人数
			WORD wInfoCount=m_ServerList.GetOnLineCountInfo(m_OnLineCountInfo,CountArray(m_OnLineCountInfo));
			m_wOnLineInfoSize=wInfoCount*sizeof(tagOnLineCountInfo);

			//发送信息
			if ((m_wOnLineInfoSize>0)&&(m_ServerUserManager.GetOnLineCount()>0))
			{
				m_pITCPNetworkEngine->SendDataBatch(MDM_GR_SERVER_INFO,SUB_GR_ONLINE_COUNT_INFO,&m_OnLineCountInfo,m_wOnLineInfoSize);
				m_AndroidUserManager.SendDataToClient(MDM_GR_SERVER_INFO,SUB_GR_ONLINE_COUNT_INFO,&m_OnLineCountInfo,m_wOnLineInfoSize);
			}
			return true;
		}
	}

	__LEAVE_FUNCTION_LOG_CATCH


	return true;
}

//绑定参数
tagConnectItemInfo * CAttemperEngineSink::GetBindParameter(WORD wBindIndex)
{
	__ENTER_FUNCTION_LOG_CATCH

	//无效连接
	if (wBindIndex==INVALID_WORD) return NULL;

	//常规连接
	if (wBindIndex<m_pGameServiceOption->wMaxConnect)
	{
		return m_pNormalParameter+wBindIndex;
	}

	//机器连接
	if ((wBindIndex>=INDEX_ANDROID)&&(wBindIndex<(INDEX_ANDROID+MAX_ANDROID)))
	{
		return m_pAndroidParameter+(wBindIndex-INDEX_ANDROID);
	}

	//错误断言
	ASSERT(FALSE);

	__LEAVE_FUNCTION_LOG_CATCH

	return NULL;
}

//绑定用户
IServerUserItem * CAttemperEngineSink::GetServerUserItem(WORD wBindIndex)
{
	__ENTER_FUNCTION_LOG_CATCH

	//获取参数
	tagConnectItemInfo * pBindParameter=GetBindParameter(wBindIndex);

	//获取用户
	if (pBindParameter!=NULL)
	{
		return pBindParameter->pIServerUserItem;
	}

	//错误断言
	ASSERT(FALSE);

	__LEAVE_FUNCTION_LOG_CATCH

	return NULL;
}

//用户状况
WORD CAttemperEngineSink::GetAndroidUserStatus(tagAndroidUserStatus & AndroidUserStatus)
{
	//变量定义
	WORD wEnumIndex=0;
	IServerUserItem * pIServerUserItem=NULL;

	//设置变量
	ZeroMemory(&AndroidUserStatus,sizeof(AndroidUserStatus));

	//用户信息
	do
	{
		//获取用户
		pIServerUserItem=m_ServerUserManager.EnumOnLineUser(wEnumIndex++);

		//统计判断
		if ((pIServerUserItem!=NULL)&&(pIServerUserItem->IsAndroidUser()==true))
		{
			//获取属性
			BYTE cbUserStatus=pIServerUserItem->GetUserStatus();

			//用户统计
			switch (pIServerUserItem->GetUserStatus())
			{
			case US_FREE:
				{
					ASSERT(AndroidUserStatus.wFreeUserCount<CountArray(AndroidUserStatus.pServerUserFree));
					AndroidUserStatus.pServerUserFree[AndroidUserStatus.wFreeUserCount++]=pIServerUserItem;
					break;
				}
			case US_PLAY:
				{
					ASSERT(AndroidUserStatus.wPlayUserCount<CountArray(AndroidUserStatus.pServerUserPlay));
					AndroidUserStatus.pServerUserPlay[AndroidUserStatus.wPlayUserCount++]=pIServerUserItem;
					break;
				}
			case US_SIT:
			case US_READY:
				{
					ASSERT(AndroidUserStatus.wSitdownUserCount<CountArray(AndroidUserStatus.pServerUserSitdown));
					AndroidUserStatus.pServerUserSitdown[AndroidUserStatus.wSitdownUserCount++]=pIServerUserItem;
					break;
				}
			}
		}

	} while (pIServerUserItem!=NULL);

	return AndroidUserStatus.wFreeUserCount+AndroidUserStatus.wPlayUserCount+AndroidUserStatus.wSitdownUserCount;
}

//桌子状况
WORD CAttemperEngineSink::GetTableUserStatus(CTableFrame * pTableFrame, tagTableUserStatus & TableUserStatus)
{
	//设置变量
	ZeroMemory(&TableUserStatus,sizeof(TableUserStatus));

	//用户分析
	for (WORD i=0;i<m_pGameServiceAttrib->wChairCountMax;i++)
	{
		//获取用户
		IServerUserItem * pIServerUserItem=pTableFrame->GetServerUserItem(i);
		if (pIServerUserItem==NULL) continue;

		//用户类型
		if (pIServerUserItem->IsAndroidUser()==false)
		{
			TableUserStatus.wTableUserCount++;
		}
		else
		{
			TableUserStatus.wTableAndroidCount++;
		}
	}

	//最少数目
	switch (pTableFrame->m_pITableFrameSink->GetGameStartMode())
	{
	case enStartMode_AllReady:	//所有准备
		{
			TableUserStatus.wLessUserCount=2;
			break;
		}
	default:					//默认模式
		{
			TableUserStatus.wLessUserCount=m_pGameServiceAttrib->wChairCountMax;
			break;
		}
	}

	return TableUserStatus.wTableAndroidCount+TableUserStatus.wTableUserCount;
}


//喇叭事件
bool CAttemperEngineSink::OnEventBugle(const void * pData, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	return true;
}

//提取事件
bool CAttemperEngineSink::OnEventBankDrawoutGold(const void * pData, WORD wDataSize, DWORD dwSocketID)
{
	return true;
}

//存储事件
bool CAttemperEngineSink::OnEventBankStorage(const void * pData, WORD wDataSize, DWORD dwSocketID)
{
	return true;
}

//使用权限
bool CAttemperEngineSink::IsPropertyUseRight(INT nPropertyID,IServerUserItem *pIServerUserItem,IServerUserItem *pTargetUserItem)
{
	__ENTER_FUNCTION_LOG_CATCH

	//目标玩家
	if ( pTargetUserItem == NULL )
	{
		SendProMessage(pIServerUserItem, TEXT("赠送失败，您要赠送的玩家已经离开！"), SMT_EJECT|SMT_INFO);
		return false;
	}

	//获取玩家
	ASSERT(pIServerUserItem!=NULL);
	tagServerUserData *pConsumeUserData = pIServerUserItem->GetUserData();
	tagServerUserData *pTargetUserData = pTargetUserItem->GetUserData();

	//房间判断
	if ( (m_pGameServiceOption->wServerType == GAME_GENRE_GOLD || m_pGameServiceOption->wServerType == GAME_GENRE_MATCH )
		&& ( nPropertyID == PROP_NEGAGIVE || nPropertyID == PROP_DOUBLE || nPropertyID == PROP_FOURDOLD || nPropertyID == PROP_SHIELD) )
	{
		TCHAR szMessage[] = TEXT("此房间不可以使用此道具！");
		SendProMessage(pIServerUserItem, szMessage, SMT_INFO|SMT_EJECT);
		return false;
	}

	//使用判断
	switch(nPropertyID)
	{
	case PROP_NEGAGIVE :			//负分清零
		{
			LONGLONG lNowScore = pTargetUserData->UserScoreInfo.lGold;
			if( 0 <= lNowScore)
			{
				TCHAR szMessage[128];			

				if ( pIServerUserItem == pTargetUserItem ) 
					_sntprintf(szMessage,CountArray(szMessage),TEXT("您现在的积分已经是非负数，不需要使用负分清零道具！"));
				else
					_sntprintf(szMessage,CountArray(szMessage),TEXT("[ 玩家 ]现在的积分已经是非负数，不需要使用负分清零道具！"), pTargetUserData->szAccounts);

				SendProMessage(pIServerUserItem, szMessage, SMT_INFO|SMT_EJECT);
				return false;
			}
			break;
		}
	case PROP_FLEE :				//逃跑清零
		{
			LONG lNowFleeCount = pTargetUserData->UserScoreInfo.lFleeCount;

			if ( lNowFleeCount == 0 )
			{
				TCHAR szMessage[128];			

				if ( pIServerUserItem == pTargetUserItem ) 
					_sntprintf(szMessage,CountArray(szMessage),TEXT("您现在的逃跑率已经为0，不需要使用逃跑清零道具！"));
				else
					_sntprintf(szMessage,CountArray(szMessage),TEXT("[ 玩家 ]现在的逃跑率已经为0，不需要使用逃跑清零道具！"), pTargetUserData->szAccounts);

				SendProMessage(pIServerUserItem, szMessage, SMT_INFO|SMT_EJECT);
				return false;
			}
			break;
		}
	case PROP_BUGLE :				//喇叭判断
		{
			//变量定义
			bool bMatchServer=((m_pGameServiceOption->wServerType&GAME_GENRE_MATCH)!=0);
			bool bControlStart=((bMatchServer==true)&&m_pGameServiceOption->cbControlStart==TRUE);

			//比赛旁观
			if (pConsumeUserData->dwMasterRight==0L)
			{
				if ((bMatchServer==true)&&(pIServerUserItem->GetUserStatus()==US_LOOKON))
				{
					LPCTSTR pszMessage=TEXT("为了不影响比赛选手比赛，旁观的用户禁止发言！");
					SendProMessage(pIServerUserItem,pszMessage,SMT_EJECT|SMT_INFO);
					return false;
				}
			}

			//权限判断
			if (CUserRight::CanGameChat(pConsumeUserData->dwUserRight)==false)
			{
				LPCTSTR pszMessage=TEXT("您暂时没有发送房间消息的权限，只能与管理员私聊！");
				SendProMessage(pIServerUserItem,pszMessage,SMT_EJECT|SMT_INFO);
				return false;
			}

			//状态查询
			if ((pConsumeUserData->dwMasterRight==0L)&&(IsAllowGameChat()==false))
			{
				LPCTSTR pszMessage=TEXT("抱歉，本游戏房间不允许发送聊天信息！");
				SendProMessage(pIServerUserItem,pszMessage,SMT_EJECT|SMT_INFO);
				return false;
			}
			break;
		}
	}

	__LEAVE_FUNCTION_LOG_CATCH

	return true;
}

//修改金币  银行使用
void CAttemperEngineSink::ModifyGameGold(IServerUserItem * pIServerUserItem,LONGLONG lGameGold)
{
	//用户信息
	return;
}

//获取帐款
LONGLONG CAttemperEngineSink::GetBankStorageGold(IServerUserItem * pIServerUserItem)
{
	//获取结构
	tagServerUserData *pServerUserData = pIServerUserItem->GetUserData();

	return pServerUserData->UserScoreInfo.lMoney;
}

//修改帐款
void CAttemperEngineSink::ModifyBankStorageGold(IServerUserItem * pIServerUserItem, LONGLONG lStorageCount)
{
	__ENTER_FUNCTION_LOG_CATCH

	//修改金币
	pIServerUserItem->ModifyBankStorageGold(lStorageCount);

	//积分通知
	SendUserScore( pIServerUserItem );

	__LEAVE_FUNCTION_LOG_CATCH

	return ;
}

bool CAttemperEngineSink::OnDBFindGemsTaskLoad(VOID * pData, WORD wDataSize)
{

	__ENTER_FUNCTION_LOG_CATCH


	ASSERT(wDataSize==sizeof(DBR_FIND_GEMS_TASK));
	if (wDataSize!=sizeof(DBR_FIND_GEMS_TASK)) return false;

	//消息处理
	DBR_FIND_GEMS_TASK * pUserMiss=(DBR_FIND_GEMS_TASK *)pData;
	//搜索玩家
	IServerUserItem *pServerUserItem = m_ServerUserManager.SearchOnLineUser(pUserMiss->dwUserID);
	if ( pServerUserItem == NULL ) pServerUserItem = m_ServerUserManager.SearchOffLineUser(pUserMiss->dwUserID);
	if ( pServerUserItem == NULL ) return true;

	tagServerUserData *pServerUserData = pServerUserItem->GetUserData();
	memcpy(&(pServerUserData->task),  pUserMiss, sizeof(DBR_FIND_GEMS_TASK));

	__LEAVE_FUNCTION_LOG_CATCH

	return true;
}



bool CAttemperEngineSink::OnDBLoadAffiche(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	__ENTER_FUNCTION_LOG_CATCH

	//效验参数
	ASSERT(wDataSize==sizeof(DBR_GR_affiche));
	if (wDataSize!=sizeof(DBR_GR_affiche)) return false;

	DBR_GR_affiche* pAffiche = (DBR_GR_affiche*)pData;
	if(pAffiche != NULL)
	{
		TCHAR buf[BUFSIZ] = {0};
		sprintf(buf, "公告:%s|**|", pAffiche->szInfo);
		m_strAffiche += buf;
	}

	__LEAVE_FUNCTION_LOG_CATCH

	return true;
}

//加载喜报
bool CAttemperEngineSink::OnDBLoadHappyMess(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	__ENTER_FUNCTION_LOG_CATCH

	//效验参数
	ASSERT(wDataSize==sizeof(DBR_GR_happyMess));
	if (wDataSize!=sizeof(DBR_GR_happyMess)) return false;

	DBR_GR_happyMess* pHappyMess = (DBR_GR_happyMess*)pData;
	if(pHappyMess != NULL)
	{
		TCHAR buf[BUFSIZ] = {0};
		sprintf(buf, "喜报:%s|**|", pHappyMess->szInfo);
		m_strHappyMess += buf;
	}

	__LEAVE_FUNCTION_LOG_CATCH

	return true;
}

//请求数据库响应任务完成
void CAttemperEngineSink::PerformTaskFinsh(const CMD_GF_TASK_tagTaskFinish& taskFinish)
{

	__ENTER_FUNCTION_LOG_CATCH


	IServerUserItem *pServerUserItem = m_ServerUserManager.SearchOnLineUser(taskFinish.dwUserID);
	if ( pServerUserItem == NULL ) pServerUserItem = m_ServerUserManager.SearchOffLineUser(taskFinish.dwUserID);
	if ( pServerUserItem == NULL ) return;

	DBR_TASK_Finish dtf;
	dtf.bFinish = taskFinish.bFinish;
	dtf.dwGems  = taskFinish.dwGems;
	dtf.dwKindID = taskFinish.dwKindID;
	dtf.dwUserID = taskFinish.dwUserID;
	dtf.iTaskID  = taskFinish.iTaskID;
	dtf.iTaskUserLogID = taskFinish.iTaskUserLogID;
	memcpy(dtf.strUserLogTable,taskFinish.strUserLogTable, sizeof(dtf.strUserLogTable));
	dtf.dwServerID = m_pGameServiceOption->wServerID;

	DWORD ip = pServerUserItem->GetClientIP();
	char strIP[24] = {0};	
	sprintf(strIP, TEXT("%d.%d.%d.%d"),  FOURTH_IPADDRESS(ip),THIRD_IPADDRESS(ip),
		SECOND_IPADDRESS(ip),FIRST_IPADDRESS(ip)); 

	memcpy(dtf.szClientIP, strIP, sizeof(dtf.szClientIP));
	dtf.dwSortID  = m_pGameServiceOption->wSortID;

	m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_USER_FIND_GEMS_TASK_PERFORM_FINISH, (DWORD)0, 
		(void*)&dtf,   sizeof(DBR_TASK_Finish));

	__LEAVE_FUNCTION_LOG_CATCH

}


//发送寻宝任务
bool CAttemperEngineSink::SendFindGemsTask(IServerUserItem * pIServerUserItem)
{
	__ENTER_FUNCTION_LOG_CATCH


	//变量定义
	tagServerUserData *pServerUserData = pIServerUserItem->GetUserData();
	//发送消息
	SendData(pIServerUserItem, MDM_GF_TASK , SUB_GF_FIND_GEMS_TASK_ACCEPT, &(pServerUserData->task), sizeof(CMD_GF_TASK_tagTask));

	__LEAVE_FUNCTION_LOG_CATCH

	return true;
}

//发送任务完成消息
bool CAttemperEngineSink::SendTaskFinish(VOID * pData, WORD wDataSize)
{
	__ENTER_FUNCTION_LOG_CATCH

	ASSERT(wDataSize==sizeof(CMD_GF_TASK_tagTaskFinish));
	if (wDataSize!=sizeof(CMD_GF_TASK_tagTaskFinish)) return false;

	CMD_GF_TASK_tagTaskFinish * pTaskFinish=(CMD_GF_TASK_tagTaskFinish *)pData;
	IServerUserItem *pServerUserItem = m_ServerUserManager.SearchOnLineUser(pTaskFinish->dwUserID);
	if ( pServerUserItem == NULL ) pServerUserItem = m_ServerUserManager.SearchOffLineUser(pTaskFinish->dwUserID);
	if ( pServerUserItem == NULL ) return true;

	tagServerUserData* pServerUserData = pServerUserItem->GetUserData();
	if(pServerUserData->wTableID!=INVALID_TABLE)
	{
		CTableFrame * pTableFrame=(m_pTableFrame+pServerUserData->wTableID);
		if (pTableFrame != NULL)
		{
			////发送消息到客户端
			pTableFrame->BroadcastMessage( MDM_GF_TASK,  SUB_GF_FIND_GEMS_TASK_ONCEEND, pData, wDataSize);
		}
	}

	__LEAVE_FUNCTION_LOG_CATCH

	return true;
}

//群发喜报
void CAttemperEngineSink:: SendHappyMess()
{
	__ENTER_FUNCTION_LOG_CATCH

	WORD    wEnumIndex=0;
	size_t  wSendSize = m_strHappyMess.size();
	while (true)
	{
		IServerUserItem*  pIServerUserItemSend=m_ServerUserManager.EnumOnLineUser(wEnumIndex++);
		if (pIServerUserItemSend==NULL) break;
		if (wSendSize > (size_t)0) SendData(pIServerUserItemSend, MDM_GF_FRAME, SUB_GR_HAPPY_MESS,   (char*)m_strHappyMess.c_str(), wSendSize);

	}

	__LEAVE_FUNCTION_LOG_CATCH

}

//发送公告信息
void CAttemperEngineSink::SendAffiche(IServerUserItem * pIServerUserItem)
{
	__ENTER_FUNCTION_LOG_CATCH

	size_t wSendSize = m_strAffiche.size();
	if (wSendSize > (size_t)0) SendData(pIServerUserItem, MDM_GR_SYSTEM, SUB_GR_LOGON_AFFICHE, (char*)m_strAffiche.c_str(), wSendSize);

	__LEAVE_FUNCTION_LOG_CATCH
}

//发送动作
bool CAttemperEngineSink::SendUserActionInfo(IServerUserItem* pIServerUserItem, void* pData, DWORD wDataSize)
{
	__ENTER_FUNCTION_LOG_CATCH

	tagServerUserData * pUserData=pIServerUserItem->GetUserData();
	if (pUserData->wTableID!=INVALID_TABLE)
	{
		CTableFrame * pTableFrame=m_pTableFrame+pUserData->wTableID;
		pTableFrame->BroadcastMessage(MDM_GR_USER,  SUB_RECVACTIONINFO, pData,  wDataSize);
	}

	__LEAVE_FUNCTION_LOG_CATCH

	return true;
}

//得到正在玩的桌子的个数  彭文添加
unsigned int CAttemperEngineSink::CountPlayeringDesk()
{
	CTableFrame * pTableFrame = NULL;
	unsigned int count = 0;
	for (INT_PTR i=0;i<m_pGameServiceOption->wTableCount;i++)
	{
		//获取空闲桌子
		pTableFrame = (m_pTableFrame + i);
		if (pTableFrame->IsGameStarted() || pTableFrame->GetSittingChairCount() >= m_pGameServiceAttrib->wChairCountMin) 
		{
			++count;
		}
	}
	return count;
}



//执行缺人桌子的坐下操作
bool CAttemperEngineSink::PerformSitDownOnDesk(int lessUser)
{
	__ENTER_FUNCTION_LOG_CATCH


	char info[BUFSIZ]={0};
	int count = 0;
	while (m_UserAutoQueue.Size() > 0)
	{
		if(count++ > 5)  //防止无限循环
		{
#if _SHOW_MESSAGE_NOR
			sprintf(info, _T("目前扫描5次，但是无法找到合适的座位， 共有%i个排队用户"), m_UserAutoQueue.Size());
			CTraceService::TraceString(info,TraceLevel_Normal);
#endif		
			break;
		}

		CTableFrame * pTableFrame  = NULL;
		int           needSetUser  = 0;
		DWORD        nowTime       = (DWORD)time(NULL);
		for (int i = 0;i<m_pGameServiceOption->wTableCount;i++)
		{
			//获取空闲桌子
			pTableFrame   =  (m_pTableFrame+i);
			int totalUser =  (int)(pTableFrame->GetAndroidCountInDesk() + m_UserAutoQueue.Size() + pTableFrame->GetNormalCountInDesk());
			if (pTableFrame->IsGameStarted()      == false &&
				nowTime - pTableFrame->GetEndTime() > MAX_DELAY &&
				pTableFrame->GetNormalCountInDesk() == lessUser     &&  //保证桌子上面有人
				pTableFrame->GetNormalCountInDesk() < m_pGameServiceAttrib->wChairCountMax &&
				totalUser >= m_pGameServiceAttrib->wChairCountMin )  //目前的桌子拥有足够的空位并且可以马上开始游戏
			{
#if _SHOW_MESSAGE_NOR
				sprintf(info, _T("目前有普通玩家的桌子序号：%i"), i);
				CTraceService::TraceString(info,TraceLevel_Normal);
#endif				

				int needCancelAndroid = totalUser - m_pGameServiceAttrib->wChairCountMin;
				if (needCancelAndroid > 0)
				{
					pTableFrame->CancelAndroid(needCancelAndroid);
				}
				needSetUser = (int)m_UserAutoQueue.Size();
				break;
			}
			pTableFrame = NULL;
		}

		if (pTableFrame == NULL && lessUser == m_pGameServiceAttrib->wChairCountMax) //没有找到有正常人的桌子 搜索有机器人的桌子或者空桌子
		{
			for (int i = 0;i<m_pGameServiceOption->wTableCount;i++)
			{
				//获取空闲桌子
				pTableFrame   =  (m_pTableFrame+i);
				int totalUser =  (int)(pTableFrame->GetAndroidCountInDesk() + m_UserAutoQueue.Size() + pTableFrame->GetNormalCountInDesk());
				if (pTableFrame->IsGameStarted()       == false && 
					nowTime - pTableFrame->GetEndTime() > MAX_DELAY &&
					pTableFrame->GetNormalCountInDesk() < m_pGameServiceAttrib->wChairCountMax &&
					totalUser >= m_pGameServiceAttrib->wChairCountMin)  //目前的桌子拥有足够的空位并且可以马上开始游戏
				{
#if _SHOW_MESSAGE_NOR
					sprintf(info, _T("目前没有普通玩家的桌子序号：%i"), i);
					CTraceService::TraceString(info,TraceLevel_Normal);
#endif				

					int needCancelAndroid = totalUser - m_pGameServiceAttrib->wChairCountMin;
					if (needCancelAndroid > 0)
					{
						pTableFrame->CancelAndroid(needCancelAndroid);
					}
					needSetUser = (int)m_UserAutoQueue.Size();
					break;
				}
				pTableFrame = NULL;
			}
		}


		if (pTableFrame != NULL)
		{
/*Author<cxf>;DateTime<2012-03-30>;Description<屏蔽:由于捕鱼一个人也可以玩>
			if (m_UserAutoQueue.Size() == 1 &&  !pTableFrame->HasPlayer())  //如果只有一个用户，并且找到的桌子是个空的桌子
			{
#if _SHOW_MESSAGE_NOR
				CTraceService::TraceString(TEXT("只有一个用户，并且找到的桌子是个空的桌子"),TraceLevel_Normal);
#endif				
				break;
			}
*/

			tagServerUserData * pUserData = NULL;
			for(BYTE j=0; j<needSetUser; j++)
			{
				//坐下处理
				WORD wChairID=pTableFrame->GetNullChairID(); 
				if (wChairID!=INVALID_CHAIR)
				{
					IServerUserItem*  pIServerUserItem=m_UserAutoQueue.GetFront();
					if (pIServerUserItem==NULL) 
					{
#if _SHOW_MESSAGE_NOR
						CTraceService::TraceString(TEXT("队列为空"),TraceLevel_Normal);
#endif				
						m_UserAutoQueue.PoPFront();
						return false;
					}else
					{
#if _SHOW_MESSAGE_NOR
						pUserData=pIServerUserItem->GetUserData();
						sprintf(info, _T("得到目前的用户 %s"), pUserData->szAccounts);
						CTraceService::TraceString(info,TraceLevel_Normal);
#endif
					}


					if (pTableFrame->PerformSitDownAction(wChairID,pIServerUserItem,NULL)==true)
					{
						pUserData=pIServerUserItem->GetUserData();
#if _SHOW_MESSAGE_NOR						
						sprintf(info, _T("%s 用户已经坐下了"), pUserData->szAccounts);
						CTraceService::TraceString(info,TraceLevel_Normal);
#endif
						m_UserAutoQueue.PoPFront();
					}else
					{
#if _SHOW_MESSAGE_NOR
						pUserData=pIServerUserItem->GetUserData();
						sprintf(info, _T("%s 坐下失败"), pUserData->szAccounts);
						CTraceService::TraceString(info,TraceLevel_Normal);
#endif				
						m_UserAutoQueue.PoPFront(); //彭文 暂时这样处理,没有必要处理发送警告信息给用户
						//因为在执行PerformSitDownAction中已经发送了
					}
				}
				else
				{
#if _SHOW_MESSAGE_NOR
					sprintf(info, _T("没有椅子了"));
					CTraceService::TraceString(info,TraceLevel_Normal);
#endif				
					break;
				}
			}
		}
	}

	__LEAVE_FUNCTION_LOG_CATCH

	return true;
}


//将用户塞入等待队列中
bool CAttemperEngineSink::InsertUserInQueue(IServerUserItem * pIServerUserItem)
{
	__ENTER_FUNCTION_LOG_CATCH


	char info[BUFSIZ]={0};
	tagServerUserData * pUserData=pIServerUserItem->GetUserData();
	pUserData->m_ArrangeQueueTime = (DWORD)time(NULL);
	if (pUserData->utUserType == PERSON)
	{
		if (!m_UserAutoQueue.IsExist(pIServerUserItem))
		{
			m_UserAutoQueue.PushBack(pIServerUserItem);
#if _SHOW_MESSAGE_NOR		
			sprintf(info, _T("%s 用户没有排队，加入队列: 队列共：%i人"), pUserData->szAccounts, m_UserAutoQueue.Size());
			CTraceService::TraceString(info, TraceLevel_Normal);
#endif			
			return true;
		}else
		{
#if _SHOW_MESSAGE_NOR
			//SendData(pIServerUserItem, MDM_GR_USER, SUB_GR_AUTO_QUEUE_ALREADY);
			sprintf(info, _T("%s 用户已经排队了; 队列共：%i人"), pUserData->szAccounts, m_UserAutoQueue.Size());
			CTraceService::TraceString(info,TraceLevel_Normal);
#endif			
			return false;
		}
	}else if (pUserData->utUserType == ANDROID)
	{
		if (!m_AndroidAutoQueue.IsExist(pIServerUserItem))
		{
			m_AndroidAutoQueue.PushBack(pIServerUserItem);
#if _SHOW_MESSAGE_NOR			
			sprintf(info, _T("%s 机器人没有排队，加入队列: 队列共：%i机器人"), pUserData->szAccounts, m_AndroidAutoQueue.Size());
			CTraceService::TraceString(info, TraceLevel_Normal);
#endif			
			return true;
		}else
		{
			//SendData(pIServerUserItem, MDM_GR_USER, SUB_GR_AUTO_QUEUE_ALREADY);
#if _SHOW_MESSAGE_NOR		
			sprintf(info, _T("%s 机器人已经排队了; 队列共：%i机器人"), pUserData->szAccounts, m_AndroidAutoQueue.Size());
			CTraceService::TraceString(info,TraceLevel_Normal);
#endif			
			return false;
		}
	}

	__LEAVE_FUNCTION_LOG_CATCH

	return false;
}

//检查用户是否正在玩
bool CAttemperEngineSink::CheckUserPlaying(IServerUserItem * pIServerUserItem)
{

	__ENTER_FUNCTION_LOG_CATCH


	tagServerUserData * pUserData=pIServerUserItem->GetUserData();
	//离开位置
	if (pUserData->wTableID!=INVALID_TABLE)
	{
		CTableFrame * pTableFrame=(m_pTableFrame+pUserData->wTableID);
		if (pTableFrame->IsUserPlaying(pIServerUserItem)==true)
		{
			//构造数据
			TCHAR szDescribe[128]=TEXT("");
			_snprintf(szDescribe,sizeof(szDescribe),TEXT("您正在的 [ %d ] 号游戏桌游戏，暂时不能排队！"),pUserData->wTableID+1);
			SendWarningPacket(pIServerUserItem, MDM_GR_USER, SUB_GR_AUTO_QUEUE_FAIL, szDescribe);
			return false;
		}
		pTableFrame->PerformStandUpAction(pIServerUserItem);
	}

	__LEAVE_FUNCTION_LOG_CATCH

	return true;
}

//更新定时增加机器人程序
void CAttemperEngineSink::UpDateAddAndroid()
{
	__ENTER_FUNCTION_LOG_CATCH


	#if _SHOW_MESSAGE_NOR
		CTraceService::TraceString(_T("机器人陪玩开始"), TraceLevel_Normal);
	#endif	

	char info[BUFSIZ]={0};
	int busyDesk       = CountPlayeringDesk();
	int needCancelDesk = busyDesk - (int)m_pGameServiceOption->wAndroidTableCount;
	if ( needCancelDesk > 0 ) //如果目前繁忙的桌子大于机器人站位的桌子数目，清除一些桌子
	{
		for (INT_PTR i=0; i<m_pGameServiceOption->wTableCount && needCancelDesk > 0; i++) //桌子上面机器人
		{
			//获取空闲桌子OnlyAndroidInDesk
			CTableFrame * pTableFrame = (m_pTableFrame+i);
			WORD totalChairCount  = pTableFrame->GetSittingChairCount();
			if ( totalChairCount >= m_pGameServiceAttrib->wChairCountMin &&
				pTableFrame->HasPlayer()  && 
				pTableFrame->OnlyAndroidInDesk()
				) //得到一个只有机器人的桌子
			{
#if _SHOW_MESSAGE_NOR
				sprintf(info, _T("发现了一个需要解散的只有机器人的桌子，马上解散"));
				CTraceService::TraceString(info,TraceLevel_Normal);
#endif				
				pTableFrame->CancelOnlyAndroidDesk();
				--needCancelDesk;
			}
		}
	}


	//下面增加机器人和用户玩
	IServerUserItem*  pIServerUserItem = m_UserAutoQueue.GetFront();
	if (pIServerUserItem != NULL)
	{
		tagServerUserData * pUserData      = pIServerUserItem->GetUserData();
		DWORD timeNow  = (DWORD)time(NULL);
		DWORD timeLeft = timeNow - pUserData->m_ArrangeQueueTime;
		if ( timeLeft > m_pGameServiceOption->wUserWaitUntilAndroid &&
			m_AndroidAutoQueue.Size() + m_UserAutoQueue.Size() >=  m_pGameServiceAttrib->wChairCountMin)
		{
			CTableFrame * pTableFrame = NULL;

			for (INT_PTR i=0;i<m_pGameServiceOption->wTableCount;i++)
			{
				//获取空闲桌子
				pTableFrame         = (m_pTableFrame+i);
				WORD totalNullChairCount = pTableFrame->GetTatolNullChairCount();
				if (pTableFrame->IsGameStarted()==false && totalNullChairCount == m_pGameServiceAttrib->wChairCountMax) //得到一个有空位桌子
				{
#if _SHOW_MESSAGE_NOR
					sprintf(info, _T("陪玩机器人， 目前的桌子共有%i空位置"), totalNullChairCount);
					CTraceService::TraceString(info,TraceLevel_Normal);
#endif				
					break;
				}
				pTableFrame = NULL;
			}

			if (pTableFrame  != NULL)
			{
				int needAnroid = (int)(m_pGameServiceAttrib->wChairCountMin) - (int)m_UserAutoQueue.Size();
				if (needAnroid > 0)
				{
					while (m_UserAutoQueue.Size() > 0)
					{
						//坐下处理
						WORD wChairID=pTableFrame->GetNullChairID(); 
						if (wChairID!=INVALID_CHAIR)
						{
							IServerUserItem*  pIServerUserItem= m_UserAutoQueue.GetFront();
							if (pIServerUserItem==NULL) 
							{
#if _SHOW_MESSAGE_NOR
								CTraceService::TraceString(TEXT("玩家队列为空"),TraceLevel_Normal);
#endif				
								m_UserAutoQueue.PoPFront();
							}else
							{
								pUserData=pIServerUserItem->GetUserData();
#if _SHOW_MESSAGE_NOR						
								sprintf(info, _T("玩家, 得到目前的用户 %s"), pUserData->szAccounts);
								CTraceService::TraceString(info,TraceLevel_Normal);
#endif				
							}


							if (pTableFrame->PerformSitDownAction(wChairID,pIServerUserItem,NULL)==true)
							{
								pUserData=pIServerUserItem->GetUserData();
#if _SHOW_MESSAGE_NOR						
								sprintf(info, _T("%s 玩家用户已经坐下了"), pUserData->szAccounts);
								CTraceService::TraceString(info,TraceLevel_Normal);
#endif				
								m_UserAutoQueue.PoPFront();
							}else
							{
								pUserData=pIServerUserItem->GetUserData();
#if _SHOW_MESSAGE_NOR							
								sprintf(info, _T("%s 玩家坐下失败"), pUserData->szAccounts);
								CTraceService::TraceString(info,TraceLevel_Normal);
#endif				
								m_UserAutoQueue.PoPFront(); //彭文 暂时这样处理,没有必要处理发送警告信息给用户
							}
						}
					}


					for (int i=0; i<needAnroid; ++i)
					{	
						//坐下处理
						WORD wChairID=pTableFrame->GetNullChairID(); 
						if (wChairID!=INVALID_CHAIR)
						{
							IServerUserItem*  pIServerUserItem= m_AndroidAutoQueue.GetFront();

							if (pIServerUserItem != NULL)
							{
								pUserData=pIServerUserItem->GetUserData();
								if (pUserData != NULL  && pUserData->UserScoreInfo.lGold >= m_pGameServiceOption->lRestrictScore)
								{
#if _SHOW_MESSAGE_NOR						
									sprintf(info, _T("%s 站位机器人正在坐下....."), pUserData->szAccounts);
									CTraceService::TraceString(info,TraceLevel_Normal);
#endif	
									pTableFrame->PerformSitDownAction(wChairID,pIServerUserItem,NULL);						
								}
								m_AndroidAutoQueue.PoPFront(); //将用户从机器人队列中删除
							}	
						}
					}
				}
			}
		}
	}
#if _SHOW_MESSAGE_NOR
	CTraceService::TraceString(_T("机器人陪玩结束"), TraceLevel_Normal);
#endif

	__LEAVE_FUNCTION_LOG_CATCH

}


void CAttemperEngineSink::UpdateBusyDesk()
{
	DWORD busyDeskCount = CountPlayeringDesk();
	m_BusyDeskStat.Insert(busyDeskCount );  //计算桌子的平均数量
}

void CAttemperEngineSink::TryAddUser()
{
	__ENTER_FUNCTION_LOG_CATCH


	char info[BUFSIZ]={0};
	for (INT_PTR i=0; i<m_pGameServiceOption->wTableCount; i++) //解决机器人空闲的桌子，就是2个机器人站位，但是游戏还需要一个玩家的情况
	{
		//获取空闲桌子OnlyAndroidInDesk
		CTableFrame * pTableFrame = (m_pTableFrame+i);
		WORD totalChairCount  = pTableFrame->GetSittingChairCount();
		if (pTableFrame->IsGameStarted()==false && 
			totalChairCount < m_pGameServiceAttrib->wChairCountMin && 
			pTableFrame->HasPlayer()  &&
			pTableFrame->OnlyAndroidInDesk()) //判断是不是机器人空闲的桌子
		{
			#if _SHOW_MESSAGE_NOR
				sprintf(info, _T("发现一个只有机器人的空闲桌, 马上解散这个桌子"));
				CTraceService::TraceString(info,TraceLevel_Normal);
			#endif			
			pTableFrame->CancelOnlyAndroidDesk();
		}
	}

	PerformSitDownChair();

	__LEAVE_FUNCTION_LOG_CATCH

}

void CAttemperEngineSink::AddKeepSittingAndroid()
{
	__ENTER_FUNCTION_LOG_CATCH

#if _SHOW_MESSAGE_NOR
	CTraceService::TraceString(_T("机器人站位开始"), TraceLevel_Normal);
#endif
	DWORD aveBusyDesk   = m_BusyDeskStat.GetAverage();
	if(aveBusyDesk  < m_pGameServiceOption->wAndroidTableCount && m_AndroidAutoQueue.Size() > 0)
	{
		size_t androidCount   =  m_AndroidAutoQueue.Size();
		size_t needUserCount  = (m_pGameServiceOption->wAndroidTableCount -  m_BusyDeskStat.GetAverage()) * m_pGameServiceAttrib->wChairCountMin;
		size_t smallCount     = androidCount < needUserCount ? androidCount : needUserCount;
		size_t needDesk       = smallCount / m_pGameServiceAttrib->wChairCountMin;


		char info[BUFSIZ]={0};
		int totalNullChairCount=0;
		for (size_t count = 0; count < needDesk; ++count)
		{
			CTableFrame * pTableFrame = NULL;
			for (INT_PTR i=0;i<m_pGameServiceOption->wTableCount;i++)
			{
				//获取空闲桌子
				pTableFrame         = (m_pTableFrame+i);
				totalNullChairCount = pTableFrame->GetTatolNullChairCount();
				if (pTableFrame->IsGameStarted()==false && totalNullChairCount == m_pGameServiceAttrib->wChairCountMax) //找个空桌子
				{
#if _SHOW_MESSAGE_NOR					
					sprintf(info, _T("站位机器人， 目前的桌子共有%i空位置"), totalNullChairCount);
					CTraceService::TraceString(info,TraceLevel_Normal);
#endif				
					break;
				}
				pTableFrame = NULL;
			}


			if (pTableFrame != NULL)
			{
				tagServerUserData * pUserData = NULL;
				for(BYTE j=0; j< m_pGameServiceAttrib->wChairCountMin; j++)
				{
					//坐下处理
					WORD wChairID=pTableFrame->GetNullChairID(); 
					if (wChairID!=INVALID_CHAIR)
					{
						IServerUserItem*  pIServerUserItem= m_AndroidAutoQueue.GetFront();
						if (pIServerUserItem != NULL)
						{
							pUserData=pIServerUserItem->GetUserData();
							if (pUserData != NULL  && pUserData->UserScoreInfo.lGold >= m_pGameServiceOption->lRestrictScore)
							{
								pTableFrame->PerformSitDownAction(wChairID,pIServerUserItem,NULL);
#if _SHOW_MESSAGE_NOR						
								sprintf(info, _T("%s 站位机器人正在坐下....."), pUserData->szAccounts);
								CTraceService::TraceString(info,TraceLevel_Normal);
#endif	
							}
							m_AndroidAutoQueue.PoPFront(); //彭文,没有必要处理发送警告信息给用户	
						}							

					}
				}
			}
		}
	}
#if _SHOW_MESSAGE_NOR	
	CTraceService::TraceString(_T("机器人站位结束"), TraceLevel_Normal);
#endif

	__LEAVE_FUNCTION_LOG_CATCH

}

// 设置日志指针
void CAttemperEngineSink::SetRoomLog(cnd_ILogMgr*	pRoomLog)
{
	m_pRoomLog = pRoomLog;;	// 房间日志
}

// 发送新手礼包	Author<cxf>; DateTime<2012-01-17>
void CAttemperEngineSink::SendNewGift()
{
	WORD    wEnumIndex=0;
	DWORD	dwTime=0;
	// 在线有礼，15，30，60，120范围
	// 最少赠送，满足在线15分钟；最高赠送，120分钟（两小时）
	DWORD	dwMinVal[]={9*60+45, 29*60+45, 59*60+45, 119*60+45};
	DWORD	dwMaxVal[]={11*60-45, 31*60-45, 61*60-45, 121*60-45};
	DWORD	dwMaxTimer[]={600, 1800, 3600, 7200};
	int		iGold[]={200,500,1200,2500};//在线有礼，对应的金币奖励 200，500，1200，2500
	DWORD	dwRand[]={2,3,4,5,6,7,8};// 升级送宝-等级
	int		iGem[]={5,8,12,18,25,36,50};// 升级送宝-宝石
	LONG	lTmpExperience=0l;// 经验值
	int		iTmpGrade=0l;	// 等级

	////Test
	//dwMinVal[0]=1*60;dwMinVal[1]=4*60;dwMinVal[2]=7*60;dwMinVal[3]=10*60;
	//dwMaxVal[0]=3*60;dwMaxVal[1]=6*60;dwMaxVal[2]=9*60;dwMaxVal[3]=12*60;
	////end Test

	CMD_GF_NEW_GIFT tagNewGift;
	while (TRUE)
	{
		// 枚举所有在线玩家
		IServerUserItem*  pIServerUserItemSend = m_ServerUserManager.EnumOnLineUser(wEnumIndex++);
		if (NULL == pIServerUserItemSend)
			break;

		if (US_PLAY == pIServerUserItemSend->GetUserStatus()  &&  PERSON==pIServerUserItemSend->GetUserData()->utUserType)
		{
			CTableFrame * pTableFrame = (m_pTableFrame+pIServerUserItemSend->GetTableID());
			if (m_pGameServiceOption->wTableCount <= pIServerUserItemSend->GetTableID())
				continue;
			if (NULL == pTableFrame)
				continue;

			// 在线有礼
			// 在线有礼的条件是5级以下的玩家，且每天最高赠送不超过2小时
			if (5 > pIServerUserItemSend->GetUserScore()->dwGrade)
			{
				/*for (int k=0;k<pIServerUserItemSend->GetUserData()->wOnlineGiftCount;k++)
				{
					pIServerUserItemSend->SetOnlineTimeGoldSign(k);
				}*/
				// 计算玩家今天的累积游戏时间,(用户今天玩游戏的时间+这局已过去的时间=今天玩游戏时间（实时的时间））
				dwTime=pIServerUserItemSend->GetUserData()->dwOnlineTime + pTableFrame->GetPlayTimeCount();

				for (BYTE i=0; i<4; i++)
				{
					BOOL bResult = pIServerUserItemSend->GetOnlineTimeGolSign(i);
					TRACE("dwMinVal[] %ld,dwMaxVal[] %ld,dwTime %ld --%d\r\n", 
						dwMinVal[i], dwMaxVal[i], dwTime, pIServerUserItemSend->GetOnlineTimeGolSign(i));

					/*if (dwMinVal[i] <= dwTime && dwMaxVal[i] >= dwTime &&
						FALSE==pIServerUserItemSend->GetOnlineTimeGolSign(i))
					{*/
					if (dwMinVal[i] <= dwTime && dwMaxVal[i] >= dwTime &&
						i >= pIServerUserItemSend->GetUserData()->wOnlineGiftCount)
					{
						ZeroMemory(&tagNewGift,sizeof(CMD_GF_NEW_GIFT));
						tagNewGift.wGiftType = 2;
						tagNewGift.iGold = iGold[i];
						tagNewGift.iGiftTime = dwMaxTimer[i];
						SendData(pIServerUserItemSend, MDM_GF_PRESENT, SUB_GF_NEW_GIFT, &tagNewGift, sizeof(CMD_GF_NEW_GIFT));
						
						//写入数据库
						tagNewGift.wUserID = pIServerUserItemSend->GetUserData()->dwUserID;
						tagNewGift.dwClientIP=pIServerUserItemSend->GetClientIP();
						NewGiftData(&tagNewGift);

						pIServerUserItemSend->WriteUserGold(iGold[i]);
						pIServerUserItemSend->GetUserData()->wOnlineGiftCount+=1;

						int		iSurplus = 0;
						for (int j=0;j<4;j++)
						{
							if ((dwTime+21)<dwMaxTimer[j])//21：最大误差
							{
								iSurplus=dwMaxTimer[j];
								break;
							}
						}
						CMD_GF_ONLINE onlineContent;
						onlineContent.dwChairID = pIServerUserItemSend->GetUserData()->wChairID;
						onlineContent.bStart = TRUE;		//是否开始计时<TRUE:开始计时;False:暂停>
						onlineContent.iToDayOnlineTime = iSurplus-dwTime;
						SendData(pIServerUserItemSend,MDM_GF_PRESENT,SUB_GF_ONLINE_TIME,&onlineContent,sizeof(CMD_GF_ONLINE));

						break;
					}
				}
			}

			// 升级送宝  -计算实时经验时，来判断有没有升级
			// 升级送宝的条件是8级及以下，才会赠送宝石
			if (8 >= pIServerUserItemSend->GetUserScore()->dwGrade)
			{
				// 以前的经验值（每局结算都会累加）+当局的经验值（这局还没结算）=总经验值（实时）
				// 经验值 = 游戏时长 / 10
				lTmpExperience = pIServerUserItemSend->GetUserScore()->lExperience + pTableFrame->GetPlayTimeCount()/10;
				iTmpGrade = CCGlobalData::CaluGrade(lTmpExperience);

				if (iTmpGrade != pIServerUserItemSend->GetUserScore()->dwGrade)
				{
					for (BYTE i=0; i<7; i++)
					{
						BOOL bResult = pIServerUserItemSend->GetUpgradeGemSign(i);
						TRACE("dwRand[i] %ld, dwTime %ld, GetUpgradeGemSign --%d\r\n", 
							dwRand[i], iTmpGrade, pIServerUserItemSend->GetUpgradeGemSign(i));

						if (dwRand[i] == iTmpGrade && FALSE==pIServerUserItemSend->GetUpgradeGemSign(i))
						{
							ZeroMemory(&tagNewGift,sizeof(CMD_GF_NEW_GIFT));
							tagNewGift.wGiftType = 3;
							tagNewGift.iGem = iGem[i];
							tagNewGift.iGrade = pIServerUserItemSend->GetUserScore()->dwGrade+1;
							SendData(pIServerUserItemSend, MDM_GF_PRESENT, SUB_GF_NEW_GIFT, &tagNewGift, sizeof(CMD_GF_NEW_GIFT));

							//写入数据库
							tagNewGift.wUserID=pIServerUserItemSend->GetUserData()->dwUserID;
							tagNewGift.dwClientIP=pIServerUserItemSend->GetClientIP();
							NewGiftData(&tagNewGift);

							pIServerUserItemSend->WriteUserGem(iGem[i]);
							pIServerUserItemSend->SetUpgradeGemSign(i);
							break;
						}
					}//end for
				}
			}
		}//end if
	}//end while
}
//
void CAttemperEngineSink::NewGiftData(CMD_GF_NEW_GIFT *tagNewGift)
{
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_ONLINE_GIFT,0,tagNewGift,sizeof(CMD_GF_NEW_GIFT));
}
