#include "StdAfx.h"
#include "TableFrameSink.h"
#include "..\客户端组件\Mathaide.h"

//////////////////////////////////////////////////////////////////////////
//静态变量
const WORD			CTableFrameSink::m_wPlayerCount=GAME_PLAYER;			//游戏人数
const enStartMode	CTableFrameSink::m_GameStartMode=enStartMode_TimeControl;//开始模式

//////////////////////////////////////////////////////////////////////////
//结束时间
#define IDI_BUILD_TRACE				1									//产生轨迹
#define IDI_CLEAR_TRACE				2									//销毁轨迹
#define IDI_CHANGE_SCENE			3									//切换场景
#define IDI_REGULAR_FISH			4									//规则鱼群
#define IDI_BONUS					5									//递增彩金
#define IDI_SYS_MESSAGE				6									//系统消息

#ifndef _DEBUG

#define TIME_BUILD_TRACE			3									//产生轨迹
#define TIME_CLEAR_TRACE			15									//销毁轨迹
#define TIME_CHANGE_SCENE			10*60								//切换场景
#define TIME_REGULAR_FISH			2									//规则鱼群
#define TIME_SYS_MESSAGE			5*60								//系统消息

#else

#define TIME_BUILD_TRACE			3									//产生轨迹
#define TIME_CLEAR_TRACE			15									//销毁轨迹
#define TIME_CHANGE_SCENE			10*60								//切换场景
#define TIME_REGULAR_FISH			2									//规则鱼群
#define TIME_SYS_MESSAGE			60									//系统消息

#endif

#define FISH_ALIVE_TIME				20*1000								//存活时间
//////////////////////////////////////////////////////////////////////////

//构造函数
CTableFrameSink::CTableFrameSink()
{
	//组件变量
	m_pITableFrame=NULL;
	m_pGameServiceOption=NULL;
	m_pITableFrameControl=NULL;
	m_CureSeaSceneKind=enSeaSceneKind_1;
	m_bWaveLToR=true;
	
	return;
}

//析构函数
CTableFrameSink::~CTableFrameSink()
{
}

//开始模式
enStartMode __cdecl CTableFrameSink::GetGameStartMode()
{
	return m_GameStartMode;
}

//接口查询
VOID * __cdecl CTableFrameSink::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(ITableFrameSink,Guid,dwQueryVer);
	QUERYINTERFACE(ITableUserAction,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(ITableFrameSink,Guid,dwQueryVer);
	return NULL;
}

// GameSerivce.dll 通向游戏服务端的通道，可以传递任何简易数据或成员指针
bool CTableFrameSink::channel_to_game_service(int iMainCode, int iAssistantCode, void* pData, int iDataLen)
{
	switch(iMainCode)
	{
	case CTGS_MAIN_LOG:	// 日志
		{
			if(CTGS_ASS_LOG_POINT==iAssistantCode)	// 日志组件的指针
			{
				if(pData)
				{
					m_pRoomLog = (cnd_ILogMgr*)pData;

					// 由于每张桌子都会到此，但只能写一次
					static bool bArrow = true;
					if(bArrow && m_pRoomLog)
					{
						bArrow = !bArrow;

						m_pRoomLog->AddMsg(0, 0, TRUE, "***** 这是【斗地主】 游戏服务端日志");
						m_pRoomLog->AddMsg(0, 0, TRUE, "***** 此文件在启动服务时开始记录");
						m_pRoomLog->AddMsg(0, 0, TRUE, "***** 在停止服务或者挂掉时终止");
						m_pRoomLog->AddMsg(0, 0, TRUE, "***** 每条记录前面是记录时间");
						m_pRoomLog->AddMsg(0, 0, TRUE, "***** 下面开始真正有意义的记录");
					}
				}
			}

			break;
		}
	default:
		{
			break;
		}
	}

	return true;
}

//配置桌子
bool __cdecl CTableFrameSink::InitTableFrameSink(IUnknownEx * pIUnknownEx)
{
	//查询接口
	ASSERT(pIUnknownEx!=NULL);
	m_pITableFrame=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,ITableFrame);
	if (m_pITableFrame==NULL) return false;

	//控制接口
	m_pITableFrameControl=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,ITableFrameControl);
	if (m_pITableFrameControl==NULL) return false;

	//获取参数
	m_pGameServiceOption=m_pITableFrame->GetGameServiceOption();
	ASSERT(m_pGameServiceOption!=NULL);

	//游戏变量
	m_dwFishID=0L;
	ZeroMemory(m_lBulletCount,sizeof(m_lBulletCount));
	ZeroMemory(m_lCoinCount,sizeof(m_lCoinCount));
	ZeroMemory(m_lWinCoinCount,sizeof(m_lWinCoinCount));
	ZeroMemory(m_lUserShootCount,sizeof(m_lUserShootCount));
	ZeroMemory(m_wCptrFishCount,sizeof(m_wCptrFishCount));
	ZeroMemory(m_dwPlayTime,sizeof(m_dwPlayTime));
	m_bySysIndex=0;

	//设置文件名
	lstrcpyn(m_szGameRoomName,m_pGameServiceOption->szGameRoomName,CountArray(m_szGameRoomName));
	TCHAR szPath[MAX_PATH]=TEXT("");
	GetCurrentDirectory(CountArray(szPath),szPath);	
	_sntprintf(m_szIniFileName,CountArray(m_szIniFileName),TEXT("%s\\Fish.ini"),szPath);
		
	return true;
}

//复位桌子
VOID __cdecl CTableFrameSink::RepositTableFrameSink()
{
	return;
}

//游戏开始
bool __cdecl CTableFrameSink::OnEventGameStart()
{
	//初始数组
	m_ServerFishTrace.FreeAllItem();
	for (WORD wChairID=0; wChairID<GAME_PLAYER; ++wChairID) 
	{
		//释放子弹
		POSITION ps=m_ShootBullet[wChairID].GetStartPosition();
		DWORD dwBulletID=0L;
		tagBulletInfo *pBulletInfo=NULL;
		while (NULL!=ps)
		{
			m_ShootBullet[wChairID].GetNextAssoc(ps, dwBulletID, pBulletInfo);
			if (NULL!=pBulletInfo) m_ShootBulletFactory.FreeItem(pBulletInfo);
		}
		m_ShootBullet[wChairID].RemoveAll();
	}

	//设置变量
	m_dwFishID=0L;
	m_dwSharkAppearTime=GetTickCount();
	m_dwGoldSharkAppearTime=GetTickCount();
	m_byRegFishColCount=0;
	m_nRegularSign=-1;
	m_byRegularForm=0;

	//读取配置
	ReadConfig();
	
	return true;
}

//游戏结束
bool __cdecl CTableFrameSink::OnEventGameEnd(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason)
{
	switch (cbReason)
	{
	case GER_NORMAL:		//常规结束
		{
			//结束游戏
			m_pITableFrame->ConcludeGame();
		
			return true;
		}
	case GER_DISMISS:		//游戏解散
		{
			//关闭定时
			m_pITableFrame->KillGameTimer(IDI_BONUS);
			m_pITableFrame->KillGameTimer(IDI_BUILD_TRACE);
			m_pITableFrame->KillGameTimer(IDI_REGULAR_FISH);
			m_pITableFrame->KillGameTimer(IDI_CHANGE_SCENE);
					
			//结束游戏
			m_pITableFrame->ConcludeGame();

			return true;
		}
	case GER_USER_LEFT:		//用户强退
		{
			//结束游戏
			//m_pITableFrame->ConcludeGame();

			return true;
		}
	}

	//错误断言
	ASSERT(FALSE);

	return false;
}

//发送场景
bool __cdecl CTableFrameSink::SendGameScene(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbGameStatus, bool bSendSecret)
{
	switch (cbGameStatus)
	{
	case GS_FREE:		//空闲状态
	case GS_PLAYING:	//游戏状态
		{
			//消息变量
			BYTE byBuffer[2048]={0};
			WORD wSendSize=0;
			CMD_S_FishTrace *pFishTrace=(CMD_S_FishTrace *)byBuffer;

			//枚举变量
			INT_PTR nEnumIndex=0;
			tagServerFishTrace *pServerFishTrace=NULL;

			do
			{
				//枚举轨迹
				pServerFishTrace=m_ServerFishTrace.EnumActiveObject(nEnumIndex++);
				if (NULL==pServerFishTrace) break;
				if ((wSendSize+sizeof(CMD_S_FishTrace))>sizeof(byBuffer)) break;

				//拷贝数据
				CopyMemory(pFishTrace->nInitX,pServerFishTrace->nInitX,sizeof(pFishTrace->nInitX));
				CopyMemory(pFishTrace->nInitY,pServerFishTrace->nInitY,sizeof(pFishTrace->nInitY));
				pFishTrace->dwFishID=pServerFishTrace->dwFishID;
				pFishTrace->FishKind=pServerFishTrace->FishKind;
				pFishTrace->nInitCount=pServerFishTrace->nInitCount;
				pFishTrace->bRegular=false;
				wSendSize+=sizeof(CMD_S_FishTrace);
				++pFishTrace;

				//终止判断
				if (nEnumIndex>=10) break;

			} while(true);

			//发送数据
			if (wSendSize>0) m_pITableFrame->SendUserData(pIServerUserItem,SUB_S_TRACE_POINT,byBuffer,wSendSize);

			//发送信息
			if (US_LOOKON!=pIServerUserItem->GetUserStatus())
			{
				CMD_S_BulletCount BulletCount={0};
				CMD_S_CoinCount CoinCount={0};
				for (WORD i=0; i<GAME_PLAYER; ++i)
				{
					//存在判断
					if (NULL==m_pITableFrame->GetServerUserItem(i)) continue;

					////add by Ivan_han
					////发送子弹
					//SendBulletCount(i, enBulletCountKind_100, true);
					//SendBulletCount(i, enBulletCountKind_600, true);
					//SendBulletCount(i, enBulletCountKind_800, true);
					m_lBulletCount[i][enBulletCountKind_100]=1000;
					m_lBulletCount[i][enBulletCountKind_600]=6000;
					m_lBulletCount[i][enBulletCountKind_800]=8000;

					//发送子弹
					BulletCount.wChairID=i;
					BulletCount.BulletCountKind=enBulletCountKind_100;
					BulletCount.lBulletCount=m_lBulletCount[i][enBulletCountKind_100];
					m_pITableFrame->SendUserData(pIServerUserItem,SUB_S_BULLET_COUNT,&BulletCount,sizeof(BulletCount));

					BulletCount.BulletCountKind=enBulletCountKind_600;
					BulletCount.lBulletCount=m_lBulletCount[i][enBulletCountKind_600];
					m_pITableFrame->SendUserData(pIServerUserItem,SUB_S_BULLET_COUNT,&BulletCount,sizeof(BulletCount));

					BulletCount.BulletCountKind=enBulletCountKind_800;
					BulletCount.lBulletCount=m_lBulletCount[i][enBulletCountKind_800];
					m_pITableFrame->SendUserData(pIServerUserItem,SUB_S_BULLET_COUNT,&BulletCount,sizeof(BulletCount));

					//发送金币
					CoinCount.wChairID=i;
					CoinCount.lCoinCount=m_lCoinCount[i];

					//发送消息
					m_pITableFrame->SendUserData(pIServerUserItem,SUB_S_COIN_COUNT,&CoinCount,sizeof(CoinCount));
				}

				//发送鱼群
				for (enFishKind FishKind=enFishKind_1; FishKind<enFishKind_No; FishKind=enFishKind(FishKind+1))
				{
					if (m_wCptrFishCount[wChairID][FishKind]>0) SendFishCount(wChairID, FishKind);
				}
			}

			//彩金信息
			if (m_lBonusOrigin>0L)
			{
				CMD_S_BonusInfo BonusInfo={0};
				BonusInfo.lBonus=m_lBonusOrigin+m_lBonusAccumulate[wChairID];
				m_pITableFrame->SendUserData(pIServerUserItem,SUB_S_BONUS_INFO,&BonusInfo,sizeof(BonusInfo));
			}

			//发送场景
			CMD_S_GameScene GameScene={0};
			CopyMemory(GameScene.lBulletCharge,m_lBulletCharge,sizeof(GameScene.lBulletCharge));
			GameScene.CureSeaSceneKind=m_CureSeaSceneKind;
			CopyMemory(GameScene.lCellScore,m_lCellScore,sizeof(GameScene.lCellScore));
			CopyMemory(GameScene.lUserCellScore,m_lUserCellScore,sizeof(GameScene.lUserCellScore));
			return m_pITableFrame->SendGameScene(pIServerUserItem,&GameScene,sizeof(GameScene));
		}
	}

	//错误断言
	ASSERT(FALSE);

	return false;
}

//定时器事件
bool __cdecl CTableFrameSink::OnTimerMessage(WORD wTimerID, WPARAM wBindParam)
{
	//事件处理
	switch (wTimerID)
	{
	case IDI_SYS_MESSAGE:				//系统消息
		{
			//重置判断
			const static BYTE byMaxIndex=100;
			if (m_bySysIndex>byMaxIndex) m_bySysIndex=0;

			//读取消息
			static TCHAR szSysMessage[512], szIndex[32];
			ZeroMemory(szSysMessage,sizeof(szSysMessage));
			while (true)
			{
				_sntprintf(szIndex,CountArray(szIndex),TEXT("Message%d"),m_bySysIndex++);
				GetPrivateProfileString(m_szGameRoomName,szIndex,NULL,szSysMessage,CountArray(szSysMessage),m_szIniFileName);
				if (0!=szSysMessage[0] || m_bySysIndex>byMaxIndex) break;
			}

			//发送消息
			if (0!=szSysMessage[0])
			{
				//发送消息
				SendGameMessage(NULL,szSysMessage);
			}
			
			return true;
		}
	case IDI_BONUS:						//递增彩金
		{
			//消息变量
			CMD_S_BonusInfo BonusInfo={0};

			//累计彩金
			for (WORD wIndex=0; wIndex<GAME_PLAYER; ++wIndex)
			{
				//获取玩家
				IServerUserItem *pIServerUserItem=m_pITableFrame->GetServerUserItem(wIndex);
				if (NULL==pIServerUserItem) continue;

				//增加彩金
				if (m_lShootBulletCount[wIndex]>=m_lBulletLimit && m_lBonusMultipleUser[wIndex]<=m_lBonusMultipleMax)
				{				
					//设置倍数
					++m_lBonusMultipleUser[wIndex];
					m_lBonusAccumulate[wIndex]=m_lBonusMultiple*m_lBonusMultipleUser[wIndex];

					//重置数目
					m_lShootBulletCount[wIndex]=0L;
				}

				//发送消息
				BonusInfo.lBonus=m_lBonusAccumulate[wIndex]+m_lBonusOrigin;
				m_pITableFrame->SendUserData(pIServerUserItem,SUB_S_BONUS_INFO,&BonusInfo,sizeof(BonusInfo));
			}

			//获取新项
			tagServerFishTrace *pServerFishTrace=m_ServerFishTrace.ActiveItem();
			ASSERT(NULL!=pServerFishTrace);
			if (NULL==pServerFishTrace) return true;

			//产生轨迹
			bool bResult=BuildTracePointLinearBonus(pServerFishTrace);

			//成功判断
			ASSERT(bResult);
			if (false==bResult) return true;

			//消息变量
			CMD_S_FishTrace FishTrace={0};

			//拷贝数据
			CopyMemory(FishTrace.nInitX,pServerFishTrace->nInitX,sizeof(FishTrace.nInitX));
			CopyMemory(FishTrace.nInitY,pServerFishTrace->nInitY,sizeof(FishTrace.nInitY));
			FishTrace.dwFishID=pServerFishTrace->dwFishID;
			FishTrace.FishKind=pServerFishTrace->FishKind;
			FishTrace.nInitCount=pServerFishTrace->nInitCount;

			//发送数据
			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_TRACE_POINT,&FishTrace,sizeof(FishTrace));
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_TRACE_POINT,&FishTrace,sizeof(FishTrace));

			return true;
		}
	case IDI_BUILD_TRACE:			//产生轨迹
		{
			//消息变量
			BYTE byBuffer[2048]={0};
			WORD wSendSize=0;
			CMD_S_FishTrace *pFishTrace=(CMD_S_FishTrace *)byBuffer;

			int nSameCount=rand()%6;
			tagServerFishTrace *pServerFishTraceLast=NULL;
			for (int i=0; i<15; ++i)
			{
				//中断判断
				if ((wSendSize+sizeof(CMD_S_FishTrace))>sizeof(byBuffer)) break;

				//获取新项
				tagServerFishTrace *pServerFishTrace=m_ServerFishTrace.ActiveItem();
				ASSERT(NULL!=pServerFishTrace);
				if (NULL==pServerFishTrace) break;

				//产生轨迹
				bool bResult=BuildTracePointLinear(pServerFishTrace,pServerFishTraceLast);

				//成功判断
				ASSERT(bResult);
				if (true==bResult) 
				{
					//拷贝数据
					CopyMemory(pFishTrace->nInitX,pServerFishTrace->nInitX,sizeof(pFishTrace->nInitX));
					CopyMemory(pFishTrace->nInitY,pServerFishTrace->nInitY,sizeof(pFishTrace->nInitY));
					pFishTrace->dwFishID=pServerFishTrace->dwFishID;
					pFishTrace->FishKind=pServerFishTrace->FishKind;
					pFishTrace->nInitCount=pServerFishTrace->nInitCount;
					wSendSize+=sizeof(CMD_S_FishTrace);
					++pFishTrace;
				}
				else
				{
					m_ServerFishTrace.FreeItem(pServerFishTrace);
					break;
				}

				//相同判断
				if (nSameCount>0 && pServerFishTrace->FishKind<=enFishKind_2)
				{
					--nSameCount;
					pServerFishTraceLast=pServerFishTrace;
				}
				else
				{
					pServerFishTraceLast=NULL;
				}
			}

			//发送数据
			if (wSendSize>0)
			{
				m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_TRACE_POINT,byBuffer,wSendSize);
				m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_TRACE_POINT,byBuffer,wSendSize);
			}

			//重置时间
			if (0!=wBindParam) m_pITableFrame->SetGameTimer(IDI_BUILD_TRACE,TIME_BUILD_TRACE*1000,TIMES_INFINITY,0L);

			return true;
		}
	case IDI_CLEAR_TRACE:			//销毁轨迹
		{
			//枚举变量
			INT_PTR nEnumIndex=0;
			tagServerFishTrace *pServerFishTrace=NULL;
			DWORD dwNowTime=GetTickCount();

			do
			{
				//枚举轨迹
				pServerFishTrace=m_ServerFishTrace.EnumActiveObject(nEnumIndex);
				if (NULL==pServerFishTrace) break;

				//判断时间
				if (dwNowTime>=(pServerFishTrace->dwBuildTime+FISH_ALIVE_TIME))
				{
					m_ServerFishTrace.FreeItem(pServerFishTrace);
					continue;
				}

				//递增索引
				++nEnumIndex;
			} while(true);

			return true;
		}
	case IDI_CHANGE_SCENE:			//切换场景
		{
			//改变场景
			m_CureSeaSceneKind=enSeaSceneKind((m_CureSeaSceneKind+1)%enSeaSceneKind_No);
			m_bWaveLToR=rand()%2?true:false;

			//变量定义
			CMD_S_ChangeScene ChangeScene;

			//设置变量
			ChangeScene.bChange=true;
			ChangeScene.CureSeaSceneKind=m_CureSeaSceneKind;
			ChangeScene.bLToR=m_bWaveLToR;

			//发送消息
			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_CHANGE_SCENE,&ChangeScene,sizeof(ChangeScene));
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_CHANGE_SCENE,&ChangeScene,sizeof(ChangeScene));

			//关闭时间(防止刚切换场景就发送新的鱼群)
			m_pITableFrame->KillGameTimer(IDI_BUILD_TRACE);

			//规则鱼群
			m_pITableFrame->SetGameTimer(IDI_REGULAR_FISH,10*1000,1,1L);

			//规则数目
			m_byRegFishColCount=rand()%5+8;

			//规则方向
			m_nRegularSign=(rand()%2)?(1):(-1);

			//规则形式
			m_byRegularForm=rand()%3;

			return true;
		}
	case IDI_REGULAR_FISH:			//规则鱼群
		{
			//枚举变量
			INT_PTR nEnumIndex=m_ServerFishTrace.GetActiveCount();//之前的鱼群不需要重复发生
			tagServerFishTrace *pServerFishTrace=NULL;

			//产生群鱼
			BuildRegularFish((wBindParam<m_byRegFishColCount)?false:true);

			//消息变量
			BYTE byBuffer[2048]={0};
			WORD wSendSize=0;
			CMD_S_FishTrace *pFishTrace=(CMD_S_FishTrace *)byBuffer;

			do
			{
				//枚举轨迹
				pServerFishTrace=m_ServerFishTrace.EnumActiveObject(nEnumIndex++);
				if (NULL==pServerFishTrace) break;
				if ((wSendSize+sizeof(CMD_S_FishTrace))>sizeof(byBuffer))
				{
					m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_TRACE_POINT,byBuffer,wSendSize);
					m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_TRACE_POINT,byBuffer,wSendSize);
					wSendSize=0;
					pFishTrace=(CMD_S_FishTrace *)byBuffer;
				}

				//拷贝数据
				CopyMemory(pFishTrace->nInitX,pServerFishTrace->nInitX,sizeof(pFishTrace->nInitX));
				CopyMemory(pFishTrace->nInitY,pServerFishTrace->nInitY,sizeof(pFishTrace->nInitY));
				pFishTrace->dwFishID=pServerFishTrace->dwFishID;
				pFishTrace->FishKind=pServerFishTrace->FishKind;
				pFishTrace->nInitCount=pServerFishTrace->nInitCount;
				pFishTrace->bRegular=true;
				wSendSize+=sizeof(CMD_S_FishTrace);
				++pFishTrace;

			} while(true);

			//发送数据
			if (wSendSize>0)
			{
				m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_TRACE_POINT,byBuffer,wSendSize);
				m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_TRACE_POINT,byBuffer,wSendSize);
			}

			//结束判断
			if (wBindParam>=m_byRegFishColCount)
			{
				//设置时间
				m_pITableFrame->SetGameTimer(IDI_BUILD_TRACE,20*1000,TIMES_INFINITY,1L);

				//关闭时间
				m_pITableFrame->KillGameTimer(IDI_REGULAR_FISH);
			}
			else
			{
				m_pITableFrame->SetGameTimer(IDI_REGULAR_FISH,TIME_REGULAR_FISH*1000,1,wBindParam+1);
			}

			return true;
		}
	}
	return false;
}

//游戏消息
bool __cdecl CTableFrameSink::OnGameMessage(WORD wSubCmdID, const void * pData, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	//消息处理
	switch (wSubCmdID)
	{
	case SUB_C_BUY_BULLET:				//购买子弹
		{
			//合法判断
			ASSERT(wDataSize==sizeof(CMD_C_BuyBullet));
			if (wDataSize!=sizeof(CMD_C_BuyBullet)) return false;

			//类型转换
			CMD_C_BuyBullet *pBuyBullet=(CMD_C_BuyBullet*)pData;

			//合法判断
			ASSERT(pBuyBullet->BulletCountKind<enBulletCountKind_End && pBuyBullet->BulletCountKind>=enBulletCountKind_100);
			if (!(pBuyBullet->BulletCountKind<enBulletCountKind_End && pBuyBullet->BulletCountKind>=enBulletCountKind_100)) 
			{
				TraceString(TEXT("(!(pBuyBullet->BulletCountKind<enBulletCountKind_End && pBuyBullet->BulletCountKind>=enBulletCountKind_100))"));
				return false;
			}
			
			return OnSubBuyBullet(pIServerUserItem, pBuyBullet->BulletCountKind);
		}
	case SUB_C_USER_SHOOT:
		{
			//合法判断
			ASSERT(wDataSize==sizeof(CMD_C_UserShoot));
			if (wDataSize!=sizeof(CMD_C_UserShoot)) return false;

			//类型转换
			CMD_C_UserShoot *pUserShoot=(CMD_C_UserShoot*)pData;

			////合法判断
			//ASSERT(pUserShoot->fAngle>=ME_MIN_CAN_ANGLE && pUserShoot->fAngle<=ME_MAX_CAN_ANGLE);
			//if (!(pUserShoot->fAngle>=ME_MIN_CAN_ANGLE && pUserShoot->fAngle<=ME_MAX_CAN_ANGLE)) return false;

			//发射炮弹
			return OnSubUserShoot(pIServerUserItem, pUserShoot->dwBulletID, pUserShoot->fAngle, pUserShoot->BulletCountKind, pUserShoot->byShootCount);
		}
	case SUB_C_HIT_FISH:
		{
			//合法判断
			ASSERT(wDataSize==sizeof(CMD_C_HitFish));
			if (wDataSize!=sizeof(CMD_C_HitFish)) return false;

			//类型转换
			CMD_C_HitFish *pHitFish=(CMD_C_HitFish*)pData;			

			//发射炮弹
			return OnSubHitFish(pIServerUserItem, pHitFish->dwFishID, pHitFish->dwBulletID);
		}
	//case SUB_C_CAPTURE_FISH:
	//	{
	//		//合法判断
	//		ASSERT(wDataSize==sizeof(CMD_C_CaptureFish));
	//		if (wDataSize!=sizeof(CMD_C_CaptureFish)) return false;

	//		//类型转换
	//		CMD_C_CaptureFish *pCaptureFish=(CMD_C_CaptureFish*)pData;

	//		//合法判断
	//		ASSERT(pCaptureFish->byBulletCount<=5);
	//		if (!(pCaptureFish->byBulletCount<=5)) 
	//		{
	//			TraceString(TEXT("(!(pCaptureFish->byBulletCount<=5))"));
	//			return false;
	//		}

	//		//发射炮弹
	//		return OnSubCaptureFish(pIServerUserItem, pCaptureFish->dwFishID, pCaptureFish->dwBulletID, pCaptureFish->byBulletCount, pCaptureFish->byCptrProbability);
	//	}
	case SUB_C_CALC_SCORE:
		{
			//计算分数
			return OnSubCalcScore(pIServerUserItem);
		}
	case SUB_C_GET_SCORE_INFO:
		{
			//获取分数
			return OnSubGetScoreInfo(pIServerUserItem);
		}		
	case SUB_C_SET_CELL_SCORE:
		{
			//合法判断
			ASSERT(wDataSize==sizeof(CMD_C_SetCellScore));
			if (wDataSize!=sizeof(CMD_C_SetCellScore)) return false;

			//类型转换
			CMD_C_SetCellScore *pSetCellScore=(CMD_C_SetCellScore*)pData;

			//设置单元
			return OnSubSetCellScore(pIServerUserItem,pSetCellScore->lCellScore);
		}
	case SUB_C_SET_PROPABILITY:
		{
			//合法判断
			ASSERT(wDataSize==sizeof(CMD_C_SetProbability));
			if (wDataSize!=sizeof(CMD_C_SetProbability)) return false;

			//类型转换
			CMD_C_SetProbability *pSetProbability=(CMD_C_SetProbability*)pData;

			for (int i=0; i<5; ++i)
			{
				for (int j=0; j<enFishKind_No; ++j)
				{
					m_byCptrProbability[i][j]=pSetProbability->byCptrProbability[i][j];
				}
			}
			return true;
		}
	}
	return false;
}

//框架消息
bool __cdecl CTableFrameSink::OnFrameMessage(WORD wSubCmdID, const void * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	return false;
}

//用户坐下
bool __cdecl CTableFrameSink::OnActionUserSitDown(WORD wChairID,IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	//开始判断
	if (false==bLookonUser && m_pITableFrame->GetGameStatus()==GS_FREE)
	{
		//开始游戏
		m_pITableFrameControl->StartGame();

		//设置时间
		m_pITableFrame->SetGameTimer(IDI_BUILD_TRACE,TIME_BUILD_TRACE*1000,TIMES_INFINITY,0L);
		m_pITableFrame->SetGameTimer(IDI_CLEAR_TRACE,TIME_CLEAR_TRACE*1000,TIMES_INFINITY,0L);
		m_pITableFrame->SetGameTimer(IDI_CHANGE_SCENE,TIME_CHANGE_SCENE*1000,TIMES_INFINITY,0L);
						
		//设置状态
		m_pITableFrame->SetGameStatus(GS_PLAYING);
	}

	//设置时间
	if (false==bLookonUser)
	{
		m_dwPlayTime[wChairID]=(DWORD)time(NULL);
		m_lWinCoinCount[wChairID]=0L;
		m_lUserShootCount[wChairID]=0L;
	}

	return true;
}

//用户起立
bool __cdecl CTableFrameSink::OnActionUserStandUp(WORD wChairID,IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	//结束判断
	if (false==bLookonUser)
	{
		//计算分数
		CalcScore(pIServerUserItem, wChairID);

		//玩家数目
		WORD wUserCount=0;
		for (WORD wIndex=0; wIndex<GAME_PLAYER; ++wIndex)
		{
			if (NULL!=m_pITableFrame->GetServerUserItem(wIndex)) ++wUserCount;
		}

		//没有玩家
		if (0==wUserCount) 
		{
			//结束游戏
			m_pITableFrame->ConcludeGame();

			//取消时间
			m_pITableFrame->KillGameTimer(IDI_BUILD_TRACE);	
			m_pITableFrame->KillGameTimer(IDI_CLEAR_TRACE);
			m_pITableFrame->KillGameTimer(IDI_CHANGE_SCENE);
			
			//设置状态
			m_pITableFrame->SetGameStatus(GS_FREE);

			//关闭定时
			m_pITableFrame->KillGameTimer(IDI_BONUS);
			m_pITableFrame->KillGameTimer(IDI_SYS_MESSAGE);			
		}

		//设置变量
		m_lUserCellScore[wChairID]=0L;
		m_dwPlayTime[wChairID]=(DWORD)time(NULL);
		m_lBonusAccumulate[wChairID]=0L;
		m_lShootBulletCount[wChairID]=0L;
		m_lBonusMultipleUser[wChairID]=0L;
	}

	return true;
}

//购买子弹
bool CTableFrameSink::OnSubBuyBullet(IServerUserItem * pIServerUserItem, enBulletCountKind BulletCountKind)
{
	//合法判断
	ASSERT(NULL!=pIServerUserItem);
	if (NULL==pIServerUserItem) return false;

	//椅子号码
	WORD wChairID=pIServerUserItem->GetChairID();
	if (wChairID>=GAME_PLAYER) return false;

	//类型判断
	LONG lBulletCount=0;
	LONGLONG lCharge=m_lBulletCharge[BulletCountKind]*m_lUserCellScore[wChairID];
	if (enBulletCountKind_100==BulletCountKind)
	{
		lBulletCount=100;
	}
	else if (enBulletCountKind_600==BulletCountKind)
	{
		lBulletCount=600;
	}
	else if (enBulletCountKind_800==BulletCountKind)
	{
		lBulletCount=800;
	}
	else
	{
		ASSERT(FALSE);
		TraceString(TEXT("OnSubBuyBullet else"));
		return false;
	}

	//扣费处理
	LONGLONG lGold=pIServerUserItem->GetUserScore()->lGold;//Update<cxf>;DateTime<2012-03-27>;Description<更新金币名字>
	ASSERT(lGold>=lCharge && lCharge>0L);
	if (!(lGold>=lCharge && lCharge>0L))
	{
		//TraceString(TEXT("(!(lGold>=lCharge))"));
		return true;
	}
	m_pITableFrame->WriteUserScore(pIServerUserItem,-lCharge,0L,enScoreKind_Service,0L);

	//设置数目	
	m_lBulletCount[wChairID][BulletCountKind]+=lBulletCount;

	//发送子弹
	SendBulletCount(wChairID, BulletCountKind);

	return true;
}

//产生轨迹
bool CTableFrameSink::BuildTracePointLinearBonus(tagServerFishTrace *pServerFishTraceNew)
{
	//合法判断
	ASSERT(NULL!=pServerFishTraceNew);
	if (NULL==pServerFishTraceNew) return false;

	//初始变量
	ZeroMemory(pServerFishTraceNew,sizeof(pServerFishTraceNew));

	//产生鱼类
	pServerFishTraceNew->FishKind=enFishKind_12;

	//产生关键点
	int const nShootRoundW[enFishKind_No]={40,56,68,56,68,88,72,80,166,180,354,368};
	int const nExcursioinX=nShootRoundW[pServerFishTraceNew->FishKind]/2,nExcursioinY=nShootRoundW[pServerFishTraceNew->FishKind]/2;
	bool bLeftToRight=(rand()%2)?true:false;
	if (bLeftToRight)
	{
		pServerFishTraceNew->nInitX[0]=0-nExcursioinX;
		pServerFishTraceNew->nInitX[1]=VIEW_WIDTH+nExcursioinX;
		pServerFishTraceNew->nInitY[0]=rand()%VIEW_HEIGHT;
		pServerFishTraceNew->nInitY[1]=rand()%VIEW_HEIGHT;
		pServerFishTraceNew->nInitCount=2;
	}
	else
	{
		pServerFishTraceNew->nInitX[0]=rand()%VIEW_WIDTH;
		pServerFishTraceNew->nInitX[1]=rand()%VIEW_WIDTH;
		pServerFishTraceNew->nInitY[0]=0-nExcursioinY;
		pServerFishTraceNew->nInitY[1]=VIEW_HEIGHT+nExcursioinY;
		pServerFishTraceNew->nInitCount=2;
	}

	//交换判断
	bool bChangePoint=(rand()%2)?true:false;
	if (bChangePoint)
	{
		int nTmp=pServerFishTraceNew->nInitX[0];
		pServerFishTraceNew->nInitX[0]=pServerFishTraceNew->nInitX[1];
		pServerFishTraceNew->nInitX[1]=nTmp;

		nTmp=pServerFishTraceNew->nInitY[0];
		pServerFishTraceNew->nInitY[0]=pServerFishTraceNew->nInitY[1];
		pServerFishTraceNew->nInitY[1]=nTmp;
	}

	//调整位置
	if (pServerFishTraceNew->nInitY[0]>=VIEW_HEIGHT) RectifyTracePosX(pServerFishTraceNew->nInitX[0]);
	if (pServerFishTraceNew->nInitY[1]>=VIEW_HEIGHT) RectifyTracePosX(pServerFishTraceNew->nInitX[1]);
	if (pServerFishTraceNew->nInitY[0]<=0) RectifyTracePosX(pServerFishTraceNew->nInitX[0]);
	if (pServerFishTraceNew->nInitY[1]<=0) RectifyTracePosX(pServerFishTraceNew->nInitX[1]);

	//设置时间
	pServerFishTraceNew->dwBuildTime=GetTickCount();

	//鱼群标识
	pServerFishTraceNew->dwFishID=GetNewFishID();

	return true;
}

//产生轨迹
bool CTableFrameSink::BuildTracePointLinear(tagServerFishTrace *pServerFishTraceNew, tagServerFishTrace *pServerFishTraceLast)
{
	//合法判断
	ASSERT(NULL!=pServerFishTraceNew);
	if (NULL==pServerFishTraceNew) return false;

	//初始变量
	ZeroMemory(pServerFishTraceNew,sizeof(pServerFishTraceNew));

	if (NULL==pServerFishTraceLast)
	{
		//产生鱼类
		pServerFishTraceNew->FishKind=enFishKind(enFishKind_1+rand()%enFishKind_12);

		//鲨鱼判断
		DWORD dwNowTime=GetTickCount();
		if (enFishKind_11==pServerFishTraceNew->FishKind)
		{
			if ((m_dwSharkAppearTime+20*1000)>dwNowTime)
				pServerFishTraceNew->FishKind=enFishKind(enFishKind_1+rand()%enFishKind_11);
			else
				m_dwSharkAppearTime=dwNowTime;
		}
		/*else if (enFishKind_12==pServerFishTraceNew->FishKind)
		{
			if ((m_dwGoldSharkAppearTime+60*1000)>dwNowTime)
				pServerFishTraceNew->FishKind=enFishKind(enFishKind_1+rand()%enFishKind_11);
			else
				m_dwGoldSharkAppearTime=dwNowTime;
		}*/

		//产生关键点
		int const nShootRoundW[enFishKind_No]={40,56,68,56,68,88,72,80,166,180,354,368};
		int const nExcursioinX=nShootRoundW[pServerFishTraceNew->FishKind]/2,nExcursioinY=nShootRoundW[pServerFishTraceNew->FishKind]/2;
		bool bLeftToRight=(rand()%2)?true:false;
		if (bLeftToRight)
		{
			pServerFishTraceNew->nInitX[0]=0-nExcursioinX;
			pServerFishTraceNew->nInitX[1]=VIEW_WIDTH+nExcursioinX;
			pServerFishTraceNew->nInitY[0]=rand()%VIEW_HEIGHT;
			pServerFishTraceNew->nInitY[1]=rand()%VIEW_HEIGHT;
			pServerFishTraceNew->nInitCount=2;
		}
		else
		{
			pServerFishTraceNew->nInitX[0]=rand()%VIEW_WIDTH;
			pServerFishTraceNew->nInitX[1]=rand()%VIEW_WIDTH;
			pServerFishTraceNew->nInitY[0]=0-nExcursioinY;
			pServerFishTraceNew->nInitY[1]=VIEW_HEIGHT+nExcursioinY;
			pServerFishTraceNew->nInitCount=2;
		}

		//交换判断
		bool bChangePoint=(rand()%2)?true:false;
		if (bChangePoint)
		{
			int nTmp=pServerFishTraceNew->nInitX[0];
			pServerFishTraceNew->nInitX[0]=pServerFishTraceNew->nInitX[1];
			pServerFishTraceNew->nInitX[1]=nTmp;

			nTmp=pServerFishTraceNew->nInitY[0];
			pServerFishTraceNew->nInitY[0]=pServerFishTraceNew->nInitY[1];
			pServerFishTraceNew->nInitY[1]=nTmp;
		}

		//调整位置
		if (pServerFishTraceNew->nInitY[0]>=VIEW_HEIGHT) RectifyTracePosX(pServerFishTraceNew->nInitX[0]);
		if (pServerFishTraceNew->nInitY[1]>=VIEW_HEIGHT) RectifyTracePosX(pServerFishTraceNew->nInitX[1]);
		if (pServerFishTraceNew->nInitY[0]<=0) RectifyTracePosX(pServerFishTraceNew->nInitX[0]);
		if (pServerFishTraceNew->nInitY[1]<=0) RectifyTracePosX(pServerFishTraceNew->nInitX[1]);
	}
	else
	{
		int const static nSpace=15;
		if (0==pServerFishTraceLast->nInitX[0] && VIEW_WIDTH==pServerFishTraceLast->nInitX[1])
		{
			pServerFishTraceNew->nInitX[0]=0;
			pServerFishTraceNew->nInitX[1]=VIEW_WIDTH;
			pServerFishTraceNew->nInitY[0]=pServerFishTraceLast->nInitY[0]+nSpace;
			pServerFishTraceNew->nInitY[1]=pServerFishTraceLast->nInitY[1]+nSpace;
			pServerFishTraceNew->nInitCount=2;
		}
		else
		{
			pServerFishTraceNew->nInitX[0]=pServerFishTraceLast->nInitX[0]+nSpace;
			pServerFishTraceNew->nInitX[1]=pServerFishTraceLast->nInitX[1]+nSpace;
			pServerFishTraceNew->nInitY[0]=0;
			pServerFishTraceNew->nInitY[1]=VIEW_HEIGHT;
			pServerFishTraceNew->nInitCount=2;
		}

		//鱼群种类
		pServerFishTraceNew->FishKind=pServerFishTraceLast->FishKind;
	}

	//设置时间
	pServerFishTraceNew->dwBuildTime=GetTickCount();

	//鱼群标识
	pServerFishTraceNew->dwFishID=GetNewFishID();

#ifdef _DEBUG
	//pServerFishTraceNew->FishKind=enFishKind_11;
#endif
	return true;
}

//发射炮弹
bool CTableFrameSink::OnSubUserShoot(IServerUserItem * pIServerUserItem, DWORD dwBulletID, float fAngle, enBulletCountKind BulletCountKind, BYTE byShootCount)
{
	//合法判断
	ASSERT(NULL!=pIServerUserItem);
	ASSERT(0L!=dwBulletID);	
	//ASSERT(m_lBulletCount[pIServerUserItem->GetChairID()][BulletCountKind]>=byShootCount);
	if (NULL==pIServerUserItem) return false;
	if (0L==dwBulletID) 
	{
		TraceString(TEXT("(0L==dwBulletID)"));
		return false;
	}
	if (!(m_lBulletCount[pIServerUserItem->GetChairID()][BulletCountKind]>=byShootCount))
	{
		//发送子弹
		SendBulletCount(pIServerUserItem->GetChairID(), BulletCountKind);

		//TraceString(TEXT("if (!(m_lBulletCount[pIServerUserItem->GetChairID()][BulletCountKind]>=byShootCount))"));

		return true;
	}

	//玩家椅子
	WORD wChairID=pIServerUserItem->GetChairID();

	//递减数目
	m_lBulletCount[wChairID][BulletCountKind]-=byShootCount;

	//增加数目
	m_lShootBulletCount[wChairID]+=byShootCount;
	m_lUserShootCount[wChairID]+=byShootCount;

	//发送消息
	CMD_S_UserShoot UserShoot={0};
	UserShoot.wChairID=wChairID;
	UserShoot.fAngle=fAngle;
	UserShoot.byShootCount=byShootCount;
	UserShoot.BulletCountKind=BulletCountKind;
	SendDataExcludeSourceUser(pIServerUserItem,SUB_S_USER_SHOOT,&UserShoot,sizeof(UserShoot));

	//发送子弹
	SendBulletCount(wChairID, BulletCountKind, false);

	//记录时间
	tagBulletInfo *pBulletInfo=m_ShootBulletFactory.ActiveItem();
	ZeroMemory(pBulletInfo, sizeof(tagBulletInfo));
	pBulletInfo->dwShootTime=GetTickCount();
	pBulletInfo->byBulletCount=byShootCount;
	m_ShootBullet[wChairID][dwBulletID]=pBulletInfo;

	return true;
}

//捕捉鱼群
bool CTableFrameSink::OnSubHitFish(IServerUserItem * pIServerUserItem, DWORD dwFishID, DWORD dwBulletID)
{
	//合法判断
	ASSERT(NULL!=pIServerUserItem);
	if (NULL==pIServerUserItem) return true;

	//合法判断
	ASSERT(0L!=dwFishID && 0L!=dwBulletID);
	if (!(0L!=dwFishID && 0L!=dwBulletID)) return true;

	//鱼群存在
	tagServerFishTrace *pServerFishTrace=GetFishTraceInfo(dwFishID);
	if (NULL==pServerFishTrace) return true;
	if (pServerFishTrace->FishKind>=enFishKind_No) return true;

	//玩家椅子
	WORD wChairID=pIServerUserItem->GetChairID();

	//子弹存在
	tagBulletInfo *pBulletInfo=NULL;
	if (FALSE==m_ShootBullet[wChairID].Lookup(dwBulletID,pBulletInfo)) return true;

	//合法判断
	ASSERT(pBulletInfo->byBulletCount<=MAX_SHOOT_BULLET_COUNT);
	if (!(pBulletInfo->byBulletCount<=MAX_SHOOT_BULLET_COUNT)) return true;

	//合法判断
	ASSERT(pBulletInfo->byBulletCount<=MAX_SHOOT_BULLET_COUNT);
	if (!(pBulletInfo->byBulletCount<=MAX_SHOOT_BULLET_COUNT)) return true;

	//单个子弹击中数目
	if (pBulletInfo->byCptrFishCount>=4) 
	{
		m_ShootBulletFactory.FreeItem(pBulletInfo);
		m_ShootBullet[wChairID].RemoveKey(dwBulletID);

		return true;
	}

	//种类判断
	if (pBulletInfo->FishKind>=enFishKind_6)
	{
		m_ShootBulletFactory.FreeItem(pBulletInfo);
		m_ShootBullet[wChairID].RemoveKey(dwBulletID);

		return true;
	}

	//时间判断
	if ((pBulletInfo->dwShootTime+15000)<=GetTickCount()) 
	{
		m_ShootBulletFactory.FreeItem(pBulletInfo);
		m_ShootBullet[wChairID].RemoveKey(dwBulletID);		

		TraceString(pIServerUserItem->GetAccounts());

		return true;
	}
	
	//击中概率
	BYTE byCptrProbability=m_byCptrProbability[pBulletInfo->byBulletCount-1][pServerFishTrace->FishKind];

	//调整概率
	if (true==m_bControlScore)
	{
		//彩金判断
		if (enFishKind_12==pServerFishTrace->FishKind && m_lWinCoinCount[wChairID]>=m_byWindMulLimitBonus*m_lUserShootCount[wChairID]) return true;

		bool bRectify=false;
		if (m_lUserShootCount[wChairID]>=100)
		{
			//调高概率
			if (m_lWinCoinCount[wChairID]<=((m_byWindMulLimitMin/100.f)*m_lUserShootCount[wChairID])) 
			{
				byCptrProbability+=(byCptrProbability/5);
				bRectify=true;
			}
			else if (m_lWinCoinCount[wChairID]>=5*m_lUserShootCount[wChairID]) 
			{
				TraceString(pIServerUserItem->GetAccounts());

				byCptrProbability=0;//作弊判断
				bRectify=true;
			}
			else if (m_lWinCoinCount[wChairID]>=m_byWindMulLimitMax*m_lUserShootCount[wChairID]) 
			{
				byCptrProbability-=(byCptrProbability/5);//调低概率
				bRectify=true;
			}			
		}

		if (false==bRectify) 
		{
			//单元积分
			int nCellIndex=0;
			for (int i=0; i<CountArray(m_lCellScore); ++i)
			{
				if (m_lUserCellScore[wChairID]==m_lCellScore[i])
				{
					nCellIndex=i;
					break;
				}
			}

			if (nCellIndex<CountArray(m_byCellScoreRectify) && m_byCellScoreRectify[nCellIndex]>0)
			{
				float fPrecent=m_byCellScoreRectify[nCellIndex]/100.f;
				byCptrProbability-=BYTE((byCptrProbability*fPrecent));//调低概率
			}
		}
	}

	//击中概率
	BYTE byRandPrecent=rand()%200+1;
	if (byRandPrecent>byCptrProbability) return true;

	//增加数目
	if (pBulletInfo->byBulletCount<255) ++pBulletInfo->byCptrFishCount;
	pBulletInfo->FishKind=max(pBulletInfo->FishKind, pServerFishTrace->FishKind);
	
	//发送消息
	CMD_S_CaptureFish CaptureFish={0};
	CaptureFish.dwFishID=dwFishID;
	CaptureFish.wChairID=wChairID;
	CaptureFish.FishKind=pServerFishTrace->FishKind;
	CaptureFish.lFishScore=(enFishKind_12==pServerFishTrace->FishKind)?(m_lBonusOrigin+m_lBonusAccumulate[wChairID]):(g_nFishScore[pServerFishTrace->FishKind]);
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_CAPTURE_FISH,&CaptureFish,sizeof(CaptureFish));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_CAPTURE_FISH,&CaptureFish,sizeof(CaptureFish));

	//销毁鱼群
	m_ServerFishTrace.FreeItem(pServerFishTrace);

	//金币数目
	if (enFishKind_12==pServerFishTrace->FishKind)
	{
		//设置金币
		m_lCoinCount[wChairID]+=(m_lBonusOrigin+m_lBonusAccumulate[wChairID]);
		m_lWinCoinCount[wChairID]+=(m_lBonusOrigin+m_lBonusAccumulate[wChairID]);

		//消息变量
		CMD_S_BonusInfo BonusInfo={0};

		//发送消息
		BonusInfo.lBonus=m_lBonusOrigin;
		m_pITableFrame->SendUserData(pIServerUserItem,SUB_S_BONUS_INFO,&BonusInfo,sizeof(BonusInfo));
		m_pITableFrame->SendLookonData(wChairID,SUB_S_BONUS_INFO,&BonusInfo,sizeof(BonusInfo));

		//重置倍数
		m_lBonusMultipleUser[wChairID]=0L;
		m_lBonusAccumulate[wChairID]=0L;
	}
	else
	{
		m_lCoinCount[wChairID]+=g_nFishScore[pServerFishTrace->FishKind];
		m_lWinCoinCount[wChairID]+=g_nFishScore[pServerFishTrace->FishKind];
	}			

	//保留数目
	m_wCptrFishCount[wChairID][pServerFishTrace->FishKind]+=1;

	//发送金币
	SendCoinCount(wChairID);

	//发送鱼群
	SendFishCount(wChairID, pServerFishTrace->FishKind);

	return true;
}

////捕捉鱼群
//bool CTableFrameSink::OnSubCaptureFish(IServerUserItem * pIServerUserItem, DWORD dwFishID, DWORD dwBulletID, BYTE byBulletCount, BYTE byCptrProbability)
//{
//	//合法判断
//	ASSERT(NULL!=pIServerUserItem);
//	ASSERT(0L!=dwBulletID && 0L!=dwFishID);
//	if (!(0L!=dwBulletID && 0L!=dwFishID)) return 0L;
//	if (NULL==pIServerUserItem) return false;
//
//	//玩家椅子
//	WORD wChairID=pIServerUserItem->GetChairID();
//
//	//时间判断
//	DWORD dwShootTime=0L;
//	if (FALSE==m_ShootBullet[wChairID].Lookup(dwBulletID,dwShootTime))
//	{
//	/*	ASSERT(FALSE);
//		TraceString(TEXT("(FALSE==m_ShootBullet[wChairID].Lookup(dwBulletID,dwShootTime))"));*/
//
//		return true;
//	}
//	if ((dwShootTime+10000)<=GetTickCount()) return true;
//
//	//枚举变量
//	INT_PTR nEnumIndex=0;
//	tagServerFishTrace *pServerFishTrace=NULL;
//	bool bFind=false;
//
//	//TraceString(TEXT("收到信息"));
//
//	do
//	{
//		//枚举轨迹
//		pServerFishTrace=m_ServerFishTrace.EnumActiveObject(nEnumIndex++);
//		if (NULL==pServerFishTrace) break;
//
//		//相等判断
//		if (dwFishID==pServerFishTrace->dwFishID)
//		{
//			//TraceString(TEXT("有此鱼"));
//
//			////位置判断
//			//for (int i=0; i<pServerFishTrace->nTracePtCount; ++i)
//			//{
//			//	if (lFishPos==MAKELPARAM(pServerFishTrace->ptTrace[i].x,pServerFishTrace->ptTrace[i].y))
//			//	{
//			//		//设置标志
//			//		bFind=true;
//			//	}
//			//}
//
//			////合法判断
//			//ASSERT(bFind);
//			//if (false==bFind) return false;
//
//			//设置标志
//			bFind=true;
//
//			//中断循环
//			break;
//		}	
//
//	} while(true);
//
//	//击中处理
//	if (true==bFind && NULL!=pServerFishTrace)
//	{
//		//概率判断
//		ASSERT(byCptrProbability==m_byCptrProbability[byBulletCount-1][pServerFishTrace->FishKind]);
//		if (!(byCptrProbability==m_byCptrProbability[byBulletCount-1][pServerFishTrace->FishKind])) 
//		{
//			TraceString(TEXT("(!(byCptrProbability==m_byCptrProbability[byBulletCount-1][pServerFishTrace->FishKind]))"));
//			return true;
//		}
//
//		//TraceString(TEXT("真有此鱼"));
//
//		//发送消息
//		CMD_S_CaptureFish CaptureFish={0};
//		CaptureFish.dwFishID=dwFishID;
//		CaptureFish.wChairID=wChairID;
//		CaptureFish.FishKind=pServerFishTrace->FishKind;
//		CaptureFish.lFishScore=(enFishKind_12==pServerFishTrace->FishKind)?(m_lBonusOrigin+m_lBonusAccumulate[wChairID]):(g_nFishScore[pServerFishTrace->FishKind]);
//		SendDataExcludeSourceUser(pIServerUserItem,SUB_S_CAPTURE_FISH,&CaptureFish,sizeof(CaptureFish));		
//
//		//销毁鱼群
//		m_ServerFishTrace.FreeItem(pServerFishTrace);
//
//		//金币数目
//		if (enFishKind_12==pServerFishTrace->FishKind)
//		{
//			//设置金币
//			m_lCoinCount[wChairID]+=(m_lBonusOrigin+m_lBonusAccumulate[wChairID]);
//
//			//消息变量
//			CMD_S_BonusInfo BonusInfo={0};
//
//			//发送消息
//			BonusInfo.lBonus=m_lBonusOrigin;
//			m_pITableFrame->SendUserData(pIServerUserItem,SUB_S_BONUS_INFO,&BonusInfo,sizeof(BonusInfo));
//			m_pITableFrame->SendLookonData(wChairID,SUB_S_BONUS_INFO,&BonusInfo,sizeof(BonusInfo));
//			/*m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_BONUS_INFO,&BonusInfo,sizeof(BonusInfo));
//			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_BONUS_INFO,&BonusInfo,sizeof(BonusInfo));*/
//
//			//重置倍数
//			m_lBonusMultipleUser[wChairID]=0L;
//			m_lBonusAccumulate[wChairID]=0L;
//		}
//		else
//		{
//			m_lCoinCount[wChairID]+=g_nFishScore[pServerFishTrace->FishKind];
//		}			
//
//		//保留数目
//		m_wCptrFishCount[wChairID][pServerFishTrace->FishKind]+=1;
//
//		//发送金币
//		SendCoinCount(wChairID);
//
//		//发送鱼群
//		SendFishCount(wChairID, pServerFishTrace->FishKind);
//	}
//
//	return true;
//}

//获取分数
bool CTableFrameSink::OnSubGetScoreInfo(IServerUserItem * pIServerUserItem)
{
	//合法判断
	ASSERT(NULL!=pIServerUserItem);
	if (NULL==pIServerUserItem) return false;
	
	//获取椅子
	WORD wChairID=pIServerUserItem->GetChairID();
	if (wChairID>=GAME_PLAYER) return true;

	//变量定义
	CMD_S_BulletCount BulletCount={0};

	//设置变量
	BulletCount.wChairID=wChairID;
	BulletCount.BulletCountKind=enBulletCountKind_100;
	BulletCount.lBulletCount=m_lBulletCount[wChairID][enBulletCountKind_100];
	m_pITableFrame->SendUserData(pIServerUserItem,SUB_S_BULLET_COUNT,&BulletCount,sizeof(BulletCount));

	BulletCount.BulletCountKind=enBulletCountKind_600;
	BulletCount.lBulletCount=m_lBulletCount[wChairID][enBulletCountKind_600];
	m_pITableFrame->SendUserData(pIServerUserItem,SUB_S_BULLET_COUNT,&BulletCount,sizeof(BulletCount));

	BulletCount.BulletCountKind=enBulletCountKind_800;
	BulletCount.lBulletCount=m_lBulletCount[wChairID][enBulletCountKind_800];
	m_pITableFrame->SendUserData(pIServerUserItem,SUB_S_BULLET_COUNT,&BulletCount,sizeof(BulletCount));

	//变量定义
	CMD_S_CoinCount CoinCount={0};

	//设置变量
	CoinCount.wChairID=wChairID;
	CoinCount.lCoinCount=m_lCoinCount[wChairID];

	//发送消息
	m_pITableFrame->SendUserData(pIServerUserItem,SUB_S_COIN_COUNT,&CoinCount,sizeof(CoinCount));

	//发送鱼群
	for (enFishKind FishKind=enFishKind_1; FishKind<enFishKind_No; FishKind=enFishKind(FishKind+1))
        SendFishCount(wChairID, FishKind);

	//完成消息
	m_pITableFrame->SendUserData(pIServerUserItem,SUB_S_SCORE_INFO,NULL,0);
	
	return true;
}

//设置单元
bool CTableFrameSink::OnSubSetCellScore(IServerUserItem * pIServerUserItem, LONG lCellScore)
{
	//合法判断
	ASSERT(NULL!=pIServerUserItem);
	if (NULL==pIServerUserItem) return false;

	//获取椅子
	WORD wChairID=pIServerUserItem->GetChairID();
	if (wChairID>=GAME_PLAYER) return true;

	//重置判断
	ASSERT(0==m_lUserCellScore[wChairID]);
	if (!(0==m_lUserCellScore[wChairID]))
	{
		TraceString(TEXT("(!(0==m_lUserCellScore[wChairID]))"));
		TraceString(pIServerUserItem->GetAccounts());
		return true;
	}

	//匹配判断
	bool bMatch=false;
	for (int i=0; i<CountArray(m_lCellScore); ++i)
	{
		if (lCellScore==m_lCellScore[i])
		{
			//设置变量
			m_lUserCellScore[wChairID]=lCellScore;
			bMatch=true;

			//发送消息
			CMD_S_SetCellScore SetCellScore={0};
			SetCellScore.lCellScore=lCellScore;
			SetCellScore.wChairID=wChairID;
			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_SET_CELL_SCORE,&SetCellScore,sizeof(SetCellScore));
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_SET_CELL_SCORE,&SetCellScore,sizeof(SetCellScore));

			break;
		}
	}

	return bMatch;
}

//计算分数
bool CTableFrameSink::OnSubCalcScore(IServerUserItem * pIServerUserItem)
{
	//合法判断
	ASSERT(NULL!=pIServerUserItem);
	if (NULL==pIServerUserItem) return false;
	
	//获取椅子
	WORD wChairID=pIServerUserItem->GetChairID();

	//计算分数
	CalcScore(pIServerUserItem, wChairID);

	//发送子弹
	SendBulletCount(wChairID, enBulletCountKind_100, true);
	SendBulletCount(wChairID, enBulletCountKind_600, true);
	SendBulletCount(wChairID, enBulletCountKind_800, true);

	//发送金币
	SendCoinCount(wChairID);

	//发送鱼群
	for (enFishKind FishKind=enFishKind_1; FishKind<enFishKind_No; FishKind=enFishKind(FishKind+1))
        SendFishCount(wChairID, FishKind);

	return true;
}

//发送消息
void CTableFrameSink::SendDataExcludeSourceUser(IServerUserItem * pIServerUserItemSource, WORD wSubCmdID,void * pData, WORD wDataSize)
{
	//合法判断
	ASSERT(NULL!=pIServerUserItemSource);
	if (NULL==pIServerUserItemSource) return ;

	//发送消息
	IServerUserItem *pIServerUserItemSend=NULL;
	for (WORD wChairID=0; wChairID<GAME_PLAYER; ++wChairID)
	{
		pIServerUserItemSend=m_pITableFrame->GetServerUserItem(wChairID);
		if (NULL==pIServerUserItemSend) continue;
		if (pIServerUserItemSend==pIServerUserItemSource) continue;
		m_pITableFrame->SendUserData(pIServerUserItemSend,wSubCmdID,pData,wDataSize);
	}
	m_pITableFrame->SendLookonData(INVALID_CHAIR,wSubCmdID,pData,wDataSize);
}

//发送子弹
void CTableFrameSink::SendBulletCount(WORD wChairID, enBulletCountKind BulletCountKind, bool bBroadcast)
{
	//合法判断
	ASSERT(wChairID<GAME_PLAYER);
	if (!(wChairID<GAME_PLAYER)) return ;

	//变量定义
	CMD_S_BulletCount BulletCount={0};

	//设置变量
	BulletCount.wChairID=wChairID;
	BulletCount.BulletCountKind=BulletCountKind;
	BulletCount.lBulletCount=m_lBulletCount[wChairID][BulletCountKind];

	//发送消息
	if (false==bBroadcast)
	{
		IServerUserItem *pIServerUserItemSend=NULL;
		for (WORD i=0; i<GAME_PLAYER; ++i)
		{
			pIServerUserItemSend=m_pITableFrame->GetServerUserItem(i);
			if (NULL==pIServerUserItemSend) continue;
			if (i==wChairID) continue;
			m_pITableFrame->SendUserData(pIServerUserItemSend,SUB_S_BULLET_COUNT,&BulletCount,sizeof(BulletCount));
		}
	}
	else
	{
		m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_BULLET_COUNT,&BulletCount,sizeof(BulletCount));
	}
	//m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_BULLET_COUNT,&BulletCount,sizeof(BulletCount));
}

//发送金币
void CTableFrameSink::SendCoinCount(WORD wChairID)
{
	//变量定义
	CMD_S_CoinCount CoinCount={0};

	//设置变量
	CoinCount.wChairID=wChairID;
	CoinCount.lCoinCount=m_lCoinCount[wChairID];

	//发送消息
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_COIN_COUNT,&CoinCount,sizeof(CoinCount));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_COIN_COUNT,&CoinCount,sizeof(CoinCount));
}

//发送鱼群
void CTableFrameSink::SendFishCount(WORD wChairID, enFishKind FishKind)
{
	//合法判断
	ASSERT(wChairID<GAME_PLAYER);
	if (!(wChairID<GAME_PLAYER)) return ;

	//获取玩家
	IServerUserItem *pIServerUserItem=m_pITableFrame->GetServerUserItem(wChairID);
	ASSERT(NULL!=pIServerUserItem);
	if (!(NULL!=pIServerUserItem)) return ;

	//变量定义
	CMD_S_CptrFishCount CptrFishCount;

	//设置变量
	CptrFishCount.FishKind=FishKind;
	CptrFishCount.wFishCount=m_wCptrFishCount[wChairID][FishKind];

	//发送消息
	m_pITableFrame->SendUserData(pIServerUserItem,SUB_S_FISH_COUNT,&CptrFishCount,sizeof(CptrFishCount));
}

//输出信息
void CTableFrameSink::TraceString(LPCTSTR pszMessage)
{
//#ifdef _DEBUG
//	//发送消息
//	IServerUserItem *pIServerUserItemSend=NULL;
//	for (WORD wChairID=0; wChairID<GAME_PLAYER; ++wChairID)
//	{
//		pIServerUserItemSend=m_pITableFrame->GetServerUserItem(wChairID);
//		if (NULL==pIServerUserItemSend) continue;
//		m_pITableFrame->SendGameMessage(pIServerUserItemSend,pszMessage,SMT_INFO);
//	}

	try
	{
		//设置文件名
		TCHAR szPath[MAX_PATH]=TEXT("");
		TCHAR szFileName[MAX_PATH];
		GetCurrentDirectory(CountArray(szPath),szPath);
		_sntprintf(szFileName,CountArray(szFileName),TEXT("%s\\%s.txt"),szPath,m_pGameServiceOption->szGameRoomName);

		//打开文件
		CFile FileRecord;
		if (FileRecord.Open(szFileName,CFile::modeCreate|CFile::modeNoTruncate|CFile::modeWrite))
		{
			FileRecord.SeekToEnd();
			FileRecord.Write(pszMessage,lstrlen(pszMessage));
			FileRecord.Write(TEXT("\r\n"),lstrlen(TEXT("\r\n")));
			FileRecord.Close();
		}
	}
	catch(...){}

//#endif
}

//鱼群标识
DWORD CTableFrameSink::GetNewFishID()
{
	//递增标识
	m_dwFishID+=1;
	if (0L==m_dwFishID) m_dwFishID=1;

	return m_dwFishID;
}

//产生群鱼
void CTableFrameSink::BuildRegularFish(bool bLastCol)
{
	//初始位置
	int nStartPosX=(-1==m_nRegularSign)?(-100):(VIEW_WIDTH+100);
	int nEndPosX=(-1==m_nRegularSign)?(VIEW_WIDTH+100):(-100);

	//鱼群宽度
	int const nFishWidth[enFishKind_No]={44,91,78,63,74,96,80,117,195,190,447};
	int const nFishHeight[enFishKind_No]={34/2,53/2,77/2,/*57/2*/64/2,54/2,69/2,51/2,111/2,102/2,212/2,329/2,382/2};

	//鱼群行数
	BYTE const byMaxRow=10;

	//偏移位置
	int nSpaceXFish=20,nSpaceXRow=8;

	//产生鱼群
	BYTE byLastKind=0;
	BYTE byLastCount=0;

	//鱼群种类
	enFishKind FishKindRow=enFishKind(enFishKind_1+rand()%enFishKind_11);

	//鱼群个数
	BYTE byFishCount=(FishKindRow<=enFishKind_3)?(rand()%6+10):(rand()%6+5);

	//鲨鱼判断
	if (true==bLastCol && 0==(rand()%4))
	{
		if (byFishCount>4) byFishCount=rand()%4+1;
		FishKindRow=enFishKind_11;
	}

	//高度调整
	if (byFishCount*nFishHeight[FishKindRow]>VIEW_HEIGHT) byFishCount=VIEW_HEIGHT/nFishHeight[FishKindRow];

	//队形判断
	switch (m_byRegularForm)
	{
	case 0:
		{
			//调整位置
			/*if (0!=byLastKind)
			{
				nStartPosX+=(m_nRegularSign*(nSpaceXFish*byLastCount/2));
			}*/

			int nStartPosY=(VIEW_HEIGHT-byFishCount*nFishHeight[FishKindRow]-43+24)/2+nFishHeight[FishKindRow]/2;//要减去底部工具栏高度
			for (int nPosIndex=0; nPosIndex<byFishCount; ++nPosIndex)
			{
				//获取新项
				tagServerFishTrace *pServerFishTrace=m_ServerFishTrace.ActiveItem();
				ASSERT(NULL!=pServerFishTrace);
				if (NULL==pServerFishTrace) return;

				//初始变量
				ZeroMemory(pServerFishTrace,sizeof(tagServerFishTrace));

				//设置信息
				pServerFishTrace->dwBuildTime=GetTickCount();
				pServerFishTrace->dwFishID=GetNewFishID();
				pServerFishTrace->FishKind=FishKindRow;
				pServerFishTrace->nInitCount=2;
				pServerFishTrace->nInitX[0]=nStartPosX;
				pServerFishTrace->nInitX[1]=nEndPosX;
				pServerFishTrace->nInitY[0]=nStartPosY+nPosIndex*nFishHeight[FishKindRow];
				pServerFishTrace->nInitY[1]=pServerFishTrace->nInitY[0];
			}

			//修改坐标
			nStartPosX+=(m_nRegularSign*(nFishWidth[FishKindRow]+nSpaceXRow));

			//设置变量
			byLastKind=0;
			byLastCount=byFishCount;

			break;
		}
	case 1:
		{
			if (byFishCount%2!=0) byFishCount+=1;
			int nStartPosY=(VIEW_HEIGHT-byFishCount*nFishHeight[FishKindRow]-43+24)/2+nFishHeight[FishKindRow]/2;//要减去底部工具栏高度
			for (int nPosIndex=0; nPosIndex<byFishCount; ++nPosIndex)
			{
				//获取新项
				tagServerFishTrace *pServerFishTrace=m_ServerFishTrace.ActiveItem();
				ASSERT(NULL!=pServerFishTrace);
				if (NULL==pServerFishTrace) return;

				//初始变量
				ZeroMemory(pServerFishTrace,sizeof(tagServerFishTrace));

				//设置信息
				pServerFishTrace->dwBuildTime=GetTickCount();
				pServerFishTrace->dwFishID=GetNewFishID();
				pServerFishTrace->FishKind=FishKindRow;
				pServerFishTrace->nInitCount=2;
				if (nPosIndex<byFishCount/2) pServerFishTrace->nInitX[0]=nStartPosX+m_nRegularSign*nPosIndex*nSpaceXFish;
				else pServerFishTrace->nInitX[0]=nStartPosX+m_nRegularSign*(byFishCount-1-nPosIndex)*nSpaceXFish;
				pServerFishTrace->nInitX[1]=nEndPosX;
				pServerFishTrace->nInitY[0]=nStartPosY+nPosIndex*nFishHeight[FishKindRow];
				pServerFishTrace->nInitY[1]=pServerFishTrace->nInitY[0];
			}

			//修改坐标
			nStartPosX+=(m_nRegularSign*(nFishWidth[FishKindRow]+nSpaceXFish*(byFishCount/2-1)+nSpaceXRow));

			//设置变量
			byLastKind=0;
			byLastCount=byFishCount;

			break;
		}
	case 2:
		{
			if (byFishCount%2!=0) byFishCount+=1;
			int nStartPosY=(VIEW_HEIGHT-byFishCount*nFishHeight[FishKindRow]-43+24)/2+nFishHeight[FishKindRow]/2;//要减去底部工具栏高度
			nStartPosX+=(m_nRegularSign*(nFishWidth[FishKindRow]+nSpaceXFish*(byFishCount/2-1)));
			for (int nPosIndex=0; nPosIndex<byFishCount; ++nPosIndex)
			{
				//获取新项
				tagServerFishTrace *pServerFishTrace=m_ServerFishTrace.ActiveItem();
				ASSERT(NULL!=pServerFishTrace);
				if (NULL==pServerFishTrace) return;

				//初始变量
				ZeroMemory(pServerFishTrace,sizeof(tagServerFishTrace));

				//设置信息
				pServerFishTrace->dwBuildTime=GetTickCount();
				pServerFishTrace->dwFishID=GetNewFishID();
				pServerFishTrace->FishKind=FishKindRow;
				pServerFishTrace->nInitCount=2;
				if (nPosIndex<byFishCount/2) pServerFishTrace->nInitX[0]=nStartPosX-m_nRegularSign*nPosIndex*nSpaceXFish;
				else pServerFishTrace->nInitX[0]=nStartPosX-m_nRegularSign*(byFishCount-1-nPosIndex)*nSpaceXFish;
				pServerFishTrace->nInitX[1]=nEndPosX;
				pServerFishTrace->nInitY[0]=nStartPosY+nPosIndex*nFishHeight[FishKindRow];
				pServerFishTrace->nInitY[1]=pServerFishTrace->nInitY[0];
			}

			////修改坐标
			//nStartPosX+=(m_nRegularSign*(nFishWidth[FishKindRow]+nSpaceXFish*(byFishCount-1)+nSpaceXRow));

			//设置变量
			byLastKind=0;
			byLastCount=byFishCount;

			break;
		}
	}
}

//计算分数
void CTableFrameSink::CalcScore(IServerUserItem * pIServerUserItem, WORD wChairID)
{
	//合法判断
	ASSERT(NULL!=pIServerUserItem);
	if (NULL==pIServerUserItem) return ;

	//剩余子弹
	LONGLONG lLeftCoin=0L;
	LONG lBulletCount[enBulletCountKind_End]={100,600,800};
	for (enBulletCountKind BulletCountKind=enBulletCountKind_100; BulletCountKind<enBulletCountKind_End; BulletCountKind=enBulletCountKind(BulletCountKind+1))
		lLeftCoin+=((m_lBulletCharge[BulletCountKind]/lBulletCount[BulletCountKind])*m_lBulletCount[wChairID][BulletCountKind]);

	//计算分数
	LONGLONG lScore=(m_lCoinCount[wChairID]+lLeftCoin)*m_lUserCellScore[wChairID];

	//写入分数
	DWORD dwNowTime=(DWORD)time(NULL);
	DWORD dwPlayTime=0L;
	if (m_lCoinCount[wChairID]>0L) dwPlayTime=dwNowTime-m_dwPlayTime[wChairID];
	enScoreKind ScoreKind=(m_lWinCoinCount[wChairID]>m_lUserShootCount[wChairID])?enScoreKind_Win:enScoreKind_Lost;
	if (lScore!=0L) m_pITableFrame->WriteUserScore(pIServerUserItem,lScore,0L,ScoreKind,dwPlayTime);

	//调试信息
	TraceGameInfo(pIServerUserItem, lScore, lLeftCoin, wChairID);
	
	//清理变量
	ZeroMemory(m_lBulletCount[wChairID],sizeof(m_lBulletCount[wChairID]));
	m_lCoinCount[wChairID]=0L;
	m_lUserShootCount[wChairID]=0L;
	ZeroMemory(m_wCptrFishCount[wChairID],sizeof(m_wCptrFishCount[wChairID]));
	m_dwPlayTime[wChairID]=dwNowTime;

	//释放子弹
	POSITION ps=m_ShootBullet[wChairID].GetStartPosition();
	DWORD dwBulletID=0L;
	tagBulletInfo *pBulletInfo=NULL;
	while (NULL!=ps)
	{
		m_ShootBullet[wChairID].GetNextAssoc(ps, dwBulletID, pBulletInfo);
		if (NULL!=pBulletInfo) m_ShootBulletFactory.FreeItem(pBulletInfo);
	}
	m_ShootBullet[wChairID].RemoveAll();
}

//调整位置
bool CTableFrameSink::RectifyTracePosX(int &nPosX)
{
	if (nPosX>=150 && nPosX<=370)
	{
		nPosX+=100;
		return true;
	}
	else if (nPosX>=640 && nPosX<=860)
	{
		nPosX+=100;
		return true;
	}

	return false;
}

//读取配置(从m_szIniFileName=fish.ini读取奖池等配置信息)
void CTableFrameSink::ReadConfig()
{
	//系统消息(发送系统消息的时间)
	INT nTimeSysMessage=GetPrivateProfileInt(m_szGameRoomName,TEXT("Time"),5*60,m_szIniFileName);
#ifdef _DEBUG
	if (nTimeSysMessage>0) m_pITableFrame->SetGameTimer(IDI_SYS_MESSAGE,10*1000,TIMES_INFINITY,0L);
#else
	if (nTimeSysMessage>0) m_pITableFrame->SetGameTimer(IDI_SYS_MESSAGE,nTimeSysMessage*60*1000,TIMES_INFINITY,0L);
#endif

	//读取费用(子弹费用)
	TCHAR szBuffer[32];
	LONG lDefaultValue[enBulletCountKind_End]={100,1200,2400};	
	for (int i=0; i<enBulletCountKind_End; ++i)
	{
		_sntprintf(szBuffer,CountArray(szBuffer),TEXT("Charge%d"),i+1);
		m_lBulletCharge[i]=GetPrivateProfileInt(m_szGameRoomName,szBuffer,lDefaultValue[i],m_szIniFileName);
	}

	//设置倍率
	TCHAR szIndex[128];
	for (int i=0; i<CountArray(m_lCellScore); ++i)
	{
		_sntprintf(szIndex,CountArray(szIndex),TEXT("CellScore%d"),i+1);
		m_lCellScore[i]=GetPrivateProfileInt(m_szGameRoomName,szIndex,m_pGameServiceOption->lCellScore,m_szIniFileName);
	}
	ZeroMemory(m_lUserCellScore,sizeof(m_lUserCellScore));
	//add by Ivan_han
	//单元积分
    m_lUserCellScore[0]=5;
    m_lUserCellScore[1]=5;
    m_lUserCellScore[2]=5;
    m_lUserCellScore[3]=5;

	//捕获概率P(子弹型号1-5,鱼的种类1-12)
	BYTE const byCptrProbability[5][enFishKind_No]={{80,50,25,14,10,5,2,1,1,1,1,1},
	{80,50,50,28,20,10,6,5,4,3,2,1},
	{80,70,75,42,30,15,10,7,6,5,3,1},
	{80,70,60,57,40,20,13,10,8,6,4,2},
	{90,85,80,71,50,25,16,12,9,7,5,3}};


	//捕获概率(子弹打鱼的概率)
	for (int i=0; i<5; ++i)
	{
		for (int j=0; j<enFishKind_No; ++j)
		{
			_sntprintf(szBuffer,CountArray(szBuffer),TEXT("Bullet_%d_Fish_%d"),i+1,j+1);
			m_byCptrProbability[i][j]=GetPrivateProfileInt(m_szGameRoomName,szBuffer,byCptrProbability[i][j],m_szIniFileName);
#ifdef _DEBUG
			TCHAR szInt[128];
			WritePrivateProfileString(m_szGameRoomName,szBuffer,_itoa(byCptrProbability[i][j],szInt,10),m_szIniFileName);
#endif
		}
	}

	#ifdef _DEBUG
		for (int i=0; i<5; ++i)
		{
			for (int j=0; j<enFishKind_No; ++j)
			{
				m_byCptrProbability[i][j]=200;
			}
		}
	#endif

	//彩金变量
	//初始数目
	m_lBonusOrigin=GetPrivateProfileInt(m_szGameRoomName,TEXT("Bonus_Origin"),0L,m_szIniFileName);
	//单元数目
	m_lBonusMultiple=GetPrivateProfileInt(m_szGameRoomName,TEXT("Bonus_Multiple"),0L,m_szIniFileName);
	//子弹限制
	m_lBulletLimit=GetPrivateProfileInt(m_szGameRoomName,TEXT("Bonus_BulletLimit"),m_lBonusMultiple,m_szIniFileName);	
	//最大倍数
	m_lBonusMultipleMax=GetPrivateProfileInt(m_szGameRoomName,TEXT("Bonus_MultipleMax"),100L,m_szIniFileName);
	//单元时间
	m_lBonusUnitTime=GetPrivateProfileInt(m_szGameRoomName,TEXT("Bonus_UnitTime"),5,m_szIniFileName);
	ZeroMemory(m_lBonusAccumulate,sizeof(m_lBonusAccumulate));
	ZeroMemory(m_lShootBulletCount,sizeof(m_lShootBulletCount));
	ZeroMemory(m_lBonusMultipleUser,sizeof(m_lBonusMultipleUser));

	//控制参数
	//是否启用以下控制参数，1为启用，0=false=m_bControlScore为禁用
	m_bControlScore=GetPrivateProfileInt(m_szGameRoomName,TEXT("ControlScore"),0,m_szIniFileName)?true:false;

	//当玩家打到的金币数少于购买的子弹数的百分之WindMulLimitMin时，会提高百分之二十的击中率
	m_byWindMulLimitMin=GetPrivateProfileInt(m_szGameRoomName,TEXT("WindMulLimitMin"),40,m_szIniFileName);
	//当玩家打到的金币数大于购买的子弹数的WindMulLimitMax倍时，会降低百分之二十的击中率
	m_byWindMulLimitMax=GetPrivateProfileInt(m_szGameRoomName,TEXT("WindMulLimitMax"),3,m_szIniFileName);
	//当玩家打到的金币数大于购买的子弹数的WindMulLimitBonus倍时，一直都不能击中彩金
	m_byWindMulLimitBonus=GetPrivateProfileInt(m_szGameRoomName,TEXT("WindMulLimitBonus"),2,m_szIniFileName);
	//玩家选择一倍时，击中概率会降低百分之CellScoreRectify_1
	m_byCellScoreRectify[0]=GetPrivateProfileInt(m_szGameRoomName,TEXT("CellScoreRectify_1"),0,m_szIniFileName);
	//玩家选择二倍时，击中概率会降低百分之CellScoreRectify_2
	m_byCellScoreRectify[1]=GetPrivateProfileInt(m_szGameRoomName,TEXT("CellScoreRectify_2"),0,m_szIniFileName);
	//以下类推
	m_byCellScoreRectify[2]=GetPrivateProfileInt(m_szGameRoomName,TEXT("CellScoreRectify_3"),0,m_szIniFileName);
	m_byCellScoreRectify[3]=GetPrivateProfileInt(m_szGameRoomName,TEXT("CellScoreRectify_4"),0,m_szIniFileName);
	m_byCellScoreRectify[4]=GetPrivateProfileInt(m_szGameRoomName,TEXT("CellScoreRectify_5"),0,m_szIniFileName);
	//1为输出游戏信息，0不输出
	m_bTraceGameInfo=GetPrivateProfileInt(m_szGameRoomName,TEXT("TraceGameInfo"),0,m_szIniFileName)?true:false;
	//大于此分数的游戏信息才输出
	m_lTraceScoreLevel=GetPrivateProfileInt(m_szGameRoomName,TEXT("TraceLevel"),50000,m_szIniFileName);
	
	//设置时间
	if (m_lBonusOrigin>0L) m_pITableFrame->SetGameTimer(IDI_BONUS,m_lBonusUnitTime*60*1000,TIMES_INFINITY,0L);
}

//发送游戏消息
bool CTableFrameSink::SendGameMessage(IServerUserItem * pIServerUserItem, LPCTSTR lpszMessage)
{
	//构造数据包
	CMD_S_GameMessage Message;
	lstrcpyn(Message.szContent,lpszMessage,CountArray(Message.szContent));
	Message.wMessageLength=CountStringBuffer(Message.szContent);
	WORD wSendSize=sizeof(Message)-sizeof(Message.szContent)+Message.wMessageLength*sizeof(TCHAR);

	//发送数据
	if (NULL==pIServerUserItem)
	{
		IServerUserItem *pIServerUserItemSend=NULL;
		for (WORD wChairID=0; wChairID<GAME_PLAYER; ++wChairID)
		{
			pIServerUserItemSend=m_pITableFrame->GetServerUserItem(wChairID);
			if (NULL==pIServerUserItemSend) continue;
			m_pITableFrame->SendUserData(pIServerUserItemSend,SUB_S_GAME_MESSAGE,&Message,wSendSize);
		}
		m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_MESSAGE,&Message,wSendSize);
	}
	else
	{
		m_pITableFrame->SendUserData(pIServerUserItem,SUB_S_GAME_MESSAGE,&Message,wSendSize);
		m_pITableFrame->SendLookonData(pIServerUserItem->GetChairID(),SUB_S_GAME_MESSAGE,&Message,wSendSize);
	}

	return true;
}

//用户断线
bool __cdecl CTableFrameSink::OnActionUserOffLine(WORD wChairID, IServerUserItem * pIServerUserItem)
{
	return true;
}

//存在判断
tagServerFishTrace *CTableFrameSink::GetFishTraceInfo(DWORD dwFishID)
{
	//枚举变量
	INT_PTR nEnumIndex=0;
	tagServerFishTrace *pServerFishTrace=NULL;
	do
	{
		//枚举轨迹
		pServerFishTrace=m_ServerFishTrace.EnumActiveObject(nEnumIndex++);
		if (NULL==pServerFishTrace) break;

		//相等判断
		if (dwFishID==pServerFishTrace->dwFishID) return pServerFishTrace;

	} while(true);

	return NULL;
}

//调试信息
void CTableFrameSink::TraceGameInfo(IServerUserItem * pIServerUserItem, LONGLONG lScore, LONGLONG lLeftBullet, WORD wChairID)
{
	//合法判断
	if (NULL==pIServerUserItem) return;
	if (wChairID>=GAME_PLAYER) return;

	//调试判断
	if (true==m_bTraceGameInfo && lScore>=m_lTraceScoreLevel)
	{
		try
		{
			//设置文件名
			TCHAR szPath[MAX_PATH]=TEXT("");
			TCHAR szFileName[MAX_PATH];
			GetCurrentDirectory(CountArray(szPath),szPath);
			_sntprintf(szFileName,CountArray(szFileName),TEXT("%s\\FishGameInfo\\%ld.txt"),szPath,pIServerUserItem->GetUserID());

			//建立目录
			int nExcursion=0;
			TCHAR szDirectory[MAX_PATH]=TEXT("");
			_sntprintf(szDirectory,CountArray(szDirectory),TEXT("%s\\FishGameInfo"),szPath);
			do
			{
				if (szDirectory[nExcursion]==0) 
				{
					::CreateDirectory(szDirectory,NULL);
					break;
				}
				if (szDirectory[nExcursion]==TEXT('\\'))
				{
					szDirectory[nExcursion]=0;
					::CreateDirectory(szDirectory,NULL);
					szDirectory[nExcursion]=TEXT('\\');
				}
				nExcursion++;
			} while (true);

			//打开文件
			CFile FileRecord;
			if (FileRecord.Open(szFileName,CFile::modeCreate|CFile::modeNoTruncate|CFile::modeWrite))
			{
				//移动文件
				FileRecord.SeekToEnd();

				//获取时间
				TCHAR szGameInfo[1024]={0};
				SYSTEMTIME SystemTime;
				GetLocalTime(&SystemTime);
				
				//写入分数
				_sntprintf(szGameInfo,CountArray(szGameInfo),TEXT("[%04d-%02d-%02d %02d:%02d:%02d:%02d]账号：%s，得分：%I64d，鱼获：%ld，子弹:%I64d，倍数：%ld\r\n"),
					SystemTime.wYear,SystemTime.wMonth,SystemTime.wDay,SystemTime.wHour,SystemTime.wMinute,SystemTime.wSecond,SystemTime.wMilliseconds,
					pIServerUserItem->GetAccounts(),lScore,m_lCoinCount[wChairID],lLeftBullet, m_lUserCellScore[wChairID]);
				FileRecord.Write(szGameInfo,lstrlen(szGameInfo));

				//鱼类信息
				LONG lNormalFishScore=0;
				for (enFishKind FishKind=enFishKind_1; FishKind<enFishKind_No; FishKind=enFishKind(FishKind+1))
				{
					if (m_wCptrFishCount[wChairID][FishKind]>0)
					{
						//彩金判断
						if (enFishKind_12==FishKind)
						{
							_sntprintf(szGameInfo,CountArray(szGameInfo),TEXT("彩金得分：%ld，条数：%ld"),m_lCoinCount[wChairID]-lNormalFishScore, m_wCptrFishCount[wChairID][FishKind]);
						}
						else
						{
							_sntprintf(szGameInfo,CountArray(szGameInfo),TEXT("(%ld,%ld)，"),g_nFishScore[FishKind], m_wCptrFishCount[wChairID][FishKind]);
							lNormalFishScore+=(m_wCptrFishCount[wChairID][FishKind]*g_nFishScore[FishKind]);
						}
						FileRecord.Write(szGameInfo,lstrlen(szGameInfo));
					}
				}
				
				
				FileRecord.Write(TEXT("\r\n"),lstrlen(TEXT("\r\n")));

				//关闭文件
				FileRecord.Close();
			}
		}
		catch(...){}

	}
}
//////////////////////////////////////////////////////////////////////////
