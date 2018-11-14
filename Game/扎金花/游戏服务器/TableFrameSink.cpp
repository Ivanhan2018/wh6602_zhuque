#include "StdAfx.h"
#include "TableFrameSink.h"
#include "TraceService.h"

//////////////////////////////////////////////////////////////////////////

//静态变量
const WORD			CTableFrameSink::m_wPlayerCount=GAME_PLAYER;			//游戏人数
const enStartMode	CTableFrameSink::m_GameStartMode=enStartMode_AllReady;	//开始模式

//定时器 0~30
#define IDI_GAME_COMPAREEND					1									//结束定时器
#define IDI_GAME_OPENEND					2									//结束定时器

#define TIME_GAME_COMPAREEND				6000								//结束定时器
#define TIME_GAME_OPENEND					6000								//结束定时器

//////////////////////////////////////////////////////////////////////////

//构造函数
CTableFrameSink::CTableFrameSink()
{
	//游戏变量
	m_bOperaCount=0;
	m_wBankerUser=INVALID_CHAIR;
	m_wCurrentUser=INVALID_CHAIR;
	m_lCompareCount=0;
	m_bGameEnd=false;
	ZeroMemory(m_wFlashUser,sizeof(m_wFlashUser));

	//用户状态
	ZeroMemory(&m_StGameEnd,sizeof(m_StGameEnd));
	ZeroMemory(m_cbPlayStatus,sizeof(m_cbPlayStatus));
	for(int i=0;i<m_wPlayerCount;i++)
	{
		m_bMingZhu[i]=false;
		m_wCompardUser[i].RemoveAll();
	}

	//扑克变量
	ZeroMemory(m_cbHandCardData,sizeof(m_cbHandCardData));

	//下注信息
	m_lMaxCellScore=0L;
	m_lCellScore=0L;
	m_lCurrentTimes=0L;		
	ZeroMemory(m_lTableScore,sizeof(m_lTableScore));
	ZeroMemory(m_lUserMaxScore,sizeof(m_lUserMaxScore));

	//组件变量
	m_pITableFrame=NULL;
	m_pGameServiceOption=NULL;

	return;
}

//析构函数
CTableFrameSink::~CTableFrameSink(void)
{
}

//接口查询--检测相关信息版本
void * __cdecl CTableFrameSink::QueryInterface(const IID & Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(ITableFrameSink,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(ITableFrameSink,Guid,dwQueryVer);
	return NULL;
}

//初始化
bool __cdecl CTableFrameSink::InitTableFrameSink(IUnknownEx * pIUnknownEx)
{
	//查询接口
	ASSERT(pIUnknownEx!=NULL);
	m_pITableFrame=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,ITableFrame);
	if (m_pITableFrame==NULL) return false;

	//获取参数
	m_pGameServiceOption=m_pITableFrame->GetGameServiceOption();
	ASSERT(m_pGameServiceOption!=NULL);

	return true;
}

//复位桌子
void __cdecl CTableFrameSink::RepositTableFrameSink()
{
	//游戏变量
	m_bOperaCount=0;
	m_wCurrentUser=INVALID_CHAIR;
	m_lCompareCount=0;
	ZeroMemory(m_wFlashUser,sizeof(m_wFlashUser));

	//用户状态
	ZeroMemory(m_cbPlayStatus,sizeof(m_cbPlayStatus));
	for(int i=0;i<m_wPlayerCount;i++)
	{
		m_bMingZhu[i]=false;	
		m_wCompardUser[i].RemoveAll();
	}

	//扑克变量
	ZeroMemory(m_cbHandCardData,sizeof(m_cbHandCardData));

	//下注信息
	m_lMaxCellScore=0L;						
	m_lCellScore=0L;						
	m_lCurrentTimes=0L;		
	ZeroMemory(m_lTableScore,sizeof(m_lTableScore));
	ZeroMemory(m_lUserMaxScore,sizeof(m_lUserMaxScore));

	return;
}

//开始模式
enStartMode __cdecl CTableFrameSink::GetGameStartMode()
{
	return m_GameStartMode;
}

//游戏状态
bool __cdecl CTableFrameSink::IsUserPlaying(WORD wChairID)
{
	ASSERT(wChairID<m_wPlayerCount);   
	return (m_cbPlayStatus[wChairID]==TRUE)?true:false;
}

//游戏开始
bool __cdecl CTableFrameSink::OnEventGameStart()
{

	ZeroMemory(&m_StGameEnd,sizeof(m_StGameEnd));
	char chData[100]={0};
	sprintf(chData, "CTableFrameSink::OnEventGameStart");
	CTraceService::TraceString(chData, TraceLevel_Info);

	//设置状态
	m_pITableFrame->SetGameStatus(GS_PLAYING);

	m_bGameEnd=false;

	//最大下注
	//LONGLONG lTimes=6L;
	for (WORD i=0;i<m_wPlayerCount;i++)
	{
		m_cbPlayStatus[i]=FALSE;
		//获取用户
		IServerUserItem * pIServerUserItem=m_pITableFrame->GetServerUserItem(i);
		if (pIServerUserItem==NULL) continue;

		const tagUserScore * pUserScore=pIServerUserItem->GetUserScore();
		ASSERT(pUserScore->lGem>=m_pGameServiceOption->lCellScore);

		//设置变量
		m_cbPlayStatus[i]=TRUE;
		m_lUserMaxScore[i]=pUserScore->lGem;

		////判断单注
		//LONGLONG Temp=lTimes;
		//if(m_lUserMaxScore[i]<10001)Temp=1L;
		//else if(m_lUserMaxScore[i]<100001)Temp=2L;
		//else if(m_lUserMaxScore[i]<1000001)Temp=3L;
		//else if(m_lUserMaxScore[i]<10000001)Temp=4L;
		//else if(m_lUserMaxScore[i]<100000001)Temp=5L;
		//if(lTimes>Temp)lTimes=Temp;
	}

	//下注变量
	m_lCellScore=m_pGameServiceOption->lCellScore;
	//while((lTimes--)>0)m_lCellScore*=10;
	m_lMaxCellScore=m_lCellScore*5;
	m_lCurrentTimes=1;

	//最大下注(输分限制)
	for (WORD i=0;i<m_wPlayerCount;i++)
	{
		if(m_cbPlayStatus[i]==TRUE)
		{
			//m_lUserMaxScore[i] = __min(m_lUserMaxScore[i],m_lMaxCellScore*101);
			DOUBLE  lRestrictScore = m_pGameServiceOption->lRestrictScore;
            m_lUserMaxScore[i] = lRestrictScore;
		}
	}

	//分发扑克
	m_GameLogic.RandCardList(m_cbHandCardData[0],sizeof(m_cbHandCardData)/sizeof(m_cbHandCardData[0][0]));

	//用户设置
	for (WORD i=0;i<m_wPlayerCount;i++)
	{
		if (m_cbPlayStatus[i]==TRUE)
		{
			m_lTableScore[i]=m_lCellScore;
		}
	}

	//设置庄家
	if(m_wBankerUser==INVALID_CHAIR)m_wBankerUser=rand()%m_wPlayerCount;

	//庄家离开
	if(m_wBankerUser<m_wPlayerCount && m_cbPlayStatus[m_wBankerUser]==FALSE)m_wBankerUser=rand()%m_wPlayerCount;

	//确定庄家
	while(m_cbPlayStatus[m_wBankerUser]==FALSE)
	{
		m_wBankerUser=(m_wBankerUser+1)%m_wPlayerCount;
	}

	//当前用户
	m_wCurrentUser=(m_wBankerUser+1)%m_wPlayerCount;
	while(m_cbPlayStatus[m_wCurrentUser]==FALSE)
	{
		m_wCurrentUser=(m_wCurrentUser+1)%m_wPlayerCount;
	}

	//构造数据
	CMD_S_GameStart GameStart;
	GameStart.lMaxScore=m_lMaxCellScore;
	GameStart.lCellScore=m_lCellScore;
	GameStart.lCurrentTimes=m_lCurrentTimes;
	GameStart.wCurrentUser=m_wCurrentUser;
	GameStart.wBankerUser=m_wBankerUser;



	for (WORD k=0;k<m_wPlayerCount;k++)
	{
		GameStart.bPlayStatus[k] = false;
		if (m_cbPlayStatus[k])
		{
			GameStart.bPlayStatus[k] = true;
		}
	}
	//发送数据
	for (WORD i=0;i<m_wPlayerCount;i++)
	{
		if (m_cbPlayStatus[i]==TRUE)
		{
			GameStart.lUserMaxScore=m_lUserMaxScore[i];
			m_pITableFrame->SendTableData(i,SUB_S_GAME_START,&GameStart,sizeof(GameStart));
		}
		m_pITableFrame->SendLookonData(i,SUB_S_GAME_START,&GameStart,sizeof(GameStart));
	}

	return true;
}

//游戏结束
bool __cdecl CTableFrameSink::OnEventGameEnd(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason)
{
	switch (cbReason)
	{
	case GER_COMPARECARD:	//比牌结束
	case GER_NO_PLAYER:		//没有玩家
		{
			if(m_bGameEnd)return true;
			m_bGameEnd=true;

			//定义变量
			CMD_S_GameEnd GameEnd;
			ZeroMemory(&GameEnd,sizeof(GameEnd));
			for (WORD i=0;i<m_wPlayerCount;i++)
				for (WORD j=0;j<4;j++)
					GameEnd.wCompareUser[i][j]=INVALID_CHAIR;

			//唯一玩家
			WORD wWinner,wUserCount=0;
			for (WORD i=0;i<m_wPlayerCount;i++)
			{	
				if(m_cbPlayStatus[i]==TRUE)
				{
					wUserCount++;
					wWinner=i;
					if(GER_COMPARECARD==cbReason)ASSERT(m_wBankerUser==i);
					m_wBankerUser=i;
				}
			}

			//胜利者强退
			if(wUserCount==0 /*&& GER_COMPARECARD==cbReason*/)
			{
				wWinner=m_wBankerUser;
			}

			//计算总注
			LONGLONG lWinnerScore=0L;
			for (WORD i=0;i<m_wPlayerCount;i++) 
			{
				if(i==wWinner)continue;
                if (m_lTableScore[i] > m_lUserMaxScore[i])
                {
					m_lTableScore[i] = m_lUserMaxScore[i];
                }
				GameEnd.lGameScore[i]=-m_lTableScore[i];
				lWinnerScore+=m_lTableScore[i];
			}

			//处理税收
			LONGLONG lRevenue=(LONGLONG)m_pGameServiceOption->wRevenue;
			GameEnd.lGameTax=lWinnerScore*lRevenue/100;
			GameEnd.lGameScore[wWinner]=lWinnerScore-GameEnd.lGameTax;

			CopyMemory(GameEnd.cbCardData,m_cbHandCardData,sizeof(m_cbHandCardData));

			//扑克数据
			for (WORD i=0;i<m_wPlayerCount;i++) 
			{
				WORD wCount=0;
				while(m_wCompardUser[i].GetCount()>0)
				{
					GameEnd.wCompareUser[i][wCount++] = m_wCompardUser[i].GetAt(0);
					m_wCompardUser[i].RemoveAt(0);
				}
			}

			GameEnd.wEndState=0;

			//胜利者强退
			if(wUserCount==1)
			{
				//修改积分
				//写入积分
				m_pITableFrame->WriteUserScore(wWinner,GameEnd.lGameScore[wWinner],GameEnd.lGameTax,enScoreKind_Win);
			}

			if(wChairID==GAME_PLAYER)
			{
				//结束数据
				CopyMemory(&m_StGameEnd,&GameEnd,sizeof(CMD_S_GameEnd));
				m_pITableFrame->SetGameTimer(IDI_GAME_COMPAREEND,TIME_GAME_COMPAREEND/2,1,0);
			}
			else
			{
				//发送信息
				m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
				m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
			}
			
			//结束游戏
			m_pITableFrame->ConcludeGame();

			return true;
		}
	case GER_USER_LEFT:		//用户强退
		{
			if(m_bGameEnd)return true;

			//效验参数
			ASSERT(pIServerUserItem!=NULL);
			ASSERT(wChairID<m_wPlayerCount);

			//强退处理
			return OnUserGiveUp(wChairID,true);
		}
	case GER_OPENCARD:		//开牌结束   
		{
			if(m_bGameEnd)return true;
			m_bGameEnd = true;

			//定义变量
			CMD_S_GameEnd GameEnd;
			ZeroMemory(&GameEnd,sizeof(GameEnd));
			for (WORD i=0;i<m_wPlayerCount;i++)
				for (WORD j=0;j<4;j++)
					GameEnd.wCompareUser[i][j]=INVALID_CHAIR;

			//胜利玩家
			WORD wWinner=m_wBankerUser;

			//计算分数
			LONGLONG lWinnerScore=0L;
			for (WORD i=0;i<m_wPlayerCount;i++) 
			{
				if(i==wWinner)continue;
				if (m_lTableScore[i] > m_lUserMaxScore[i])
				{
					m_lTableScore[i] = m_lUserMaxScore[i];
				}
				lWinnerScore+=m_lTableScore[i];
				GameEnd.lGameScore[i]=-m_lTableScore[i];
			}

			//处理税收
			LONGLONG lRevenue=(LONGLONG)m_pGameServiceOption->wRevenue;
			GameEnd.lGameTax=lWinnerScore*lRevenue/100;
			GameEnd.lGameScore[wWinner]=lWinnerScore-GameEnd.lGameTax;

			//开牌结束
			GameEnd.wEndState=1;
			CopyMemory(GameEnd.cbCardData,m_cbHandCardData,sizeof(m_cbHandCardData));

			//扑克数据
			for (WORD i=0;i<m_wPlayerCount;i++) 
			{				
				WORD wCount=0;
				while(m_wCompardUser[i].GetCount()>0)
				{
					GameEnd.wCompareUser[i][wCount++] = m_wCompardUser[i].GetAt(0);
					m_wCompardUser[i].RemoveAt(0);
				}
			}

			//修改积分
			tagScoreInfo ScoreInfo[m_wPlayerCount];
			ZeroMemory(ScoreInfo,sizeof(ScoreInfo));
			for (WORD i=0;i<m_wPlayerCount;i++) 
			{				
				if(i==wWinner || m_cbPlayStatus[i]==FALSE)continue;
				ScoreInfo[i].lScore=GameEnd.lGameScore[i];
				ScoreInfo[i].ScoreKind=enScoreKind_Lost;
			}
			ScoreInfo[wWinner].lScore=GameEnd.lGameScore[wWinner];
			ScoreInfo[wWinner].ScoreKind=enScoreKind_Win;

			//写入积分
			for (WORD i=0;i<m_wPlayerCount;i++)
			{
				if(m_cbPlayStatus[i]==TRUE)
				{
					LONGLONG lRevenueValue = ((i==wWinner)?(GameEnd.lGameTax):(0L));
					enScoreKind nScoreKind = ((i==wWinner)?(enScoreKind_Win):(enScoreKind_Lost));
					m_pITableFrame->WriteUserScore(i,GameEnd.lGameScore[i],lRevenueValue,nScoreKind);
				}
			}
			

			if(wChairID==GAME_PLAYER)
			{
				//结束数据
				CopyMemory(&m_StGameEnd,&GameEnd,sizeof(m_StGameEnd));
				//m_pITableFrame->SetGameTimer(IDI_GAME_OPENEND,TIME_GAME_OPENEND,1,0);
				m_pITableFrame->SetGameTimer(IDI_GAME_OPENEND,TIME_GAME_OPENEND/6,1,0);
			}
			else
			{
				//发送信息
				m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
				m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
			}

			//结束游戏
			m_pITableFrame->ConcludeGame();

			return true;
		}
	}

	return false;
}

//发送场景
bool __cdecl CTableFrameSink::SendGameScene(WORD wChiarID, IServerUserItem * pIServerUserItem, BYTE cbGameStatus, bool bSendSecret)
{
	switch (cbGameStatus)
	{
	case GS_FREE:		//空闲状态
		{
			//构造数据
			CMD_S_StatusFree StatusFree;
			ZeroMemory(&StatusFree,sizeof(StatusFree));

			//设置变量
			StatusFree.lCellScore=0L;

			//发送场景
			return m_pITableFrame->SendGameScene(pIServerUserItem,&StatusFree,sizeof(StatusFree));
		}
	case GS_PLAYING:	//游戏状态
		{
			//构造数据
			CMD_S_StatusPlay StatusPlay;
			memset(&StatusPlay,0,sizeof(StatusPlay));

			//加注信息
			StatusPlay.lMaxCellScore=m_lMaxCellScore;
			StatusPlay.lCellScore=m_lCellScore;
			StatusPlay.lCurrentTimes=m_lCurrentTimes;
			StatusPlay.lUserMaxScore=m_lUserMaxScore[wChiarID];

			//设置变量
			StatusPlay.wBankerUser=m_wBankerUser;
			StatusPlay.wCurrentUser=m_wCurrentUser;
			CopyMemory(StatusPlay.bMingZhu,m_bMingZhu,sizeof(m_bMingZhu));
			CopyMemory(StatusPlay.lTableScore,m_lTableScore,sizeof(m_lTableScore));

			for (int i=0; i<GAME_PLAYER; i++)
			{
				StatusPlay.cbPlayStatus[i] = false;
				if (m_cbPlayStatus[i])
				{
					StatusPlay.cbPlayStatus[i] = true;
				}
			}

			//当前状态
			StatusPlay.bCompareState=(m_lCompareCount<=0)?false:true;

			//设置扑克
			CopyMemory(&StatusPlay.cbHandCardData,&m_cbHandCardData[wChiarID],MAX_COUNT);

			//发送场景
			return m_pITableFrame->SendGameScene(pIServerUserItem,&StatusPlay,sizeof(StatusPlay));
		}
	}

	//效验错误
	ASSERT(FALSE);

	return false;
}

//定时器事件 
bool __cdecl CTableFrameSink::OnTimerMessage(WORD wTimerID, WPARAM wBindParam)
{
	//结束定时
	if(wTimerID==IDI_GAME_COMPAREEND || IDI_GAME_OPENEND==wTimerID)
	{
		//删除时间
		if(wTimerID==IDI_GAME_COMPAREEND)m_pITableFrame->KillGameTimer(IDI_GAME_COMPAREEND);
		else m_pITableFrame->KillGameTimer(IDI_GAME_OPENEND);

		//发送信息
		m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_END,&m_StGameEnd,sizeof(CMD_S_GameEnd));
		m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_END,&m_StGameEnd,sizeof(CMD_S_GameEnd));

		//结束游戏
		m_pITableFrame->ConcludeGame();
	}

	return false;
}

//游戏消息处理
bool __cdecl CTableFrameSink::OnGameMessage(WORD wSubCmdID, const void * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	bool bRet = false;
	switch (wSubCmdID)
	{
	case SUB_C_GIVE_UP:			//用户放弃
		{
			//用户效验
			tagServerUserData * pUserData=pIServerUserItem->GetUserData();
			if (pUserData->cbUserStatus!=US_PLAY)
			{
				bRet = false;
				break;
			}

			//状态判断
			ASSERT(m_cbPlayStatus[pUserData->wChairID]==TRUE);
			if (m_cbPlayStatus[pUserData->wChairID]==FALSE)
			{
				bRet = false;
				break;
			}

			//消息处理
			bRet = OnUserGiveUp(pUserData->wChairID);
			break;
		}
	case SUB_C_LOOK_CARD:		//用户看牌
	case SUB_C_OPEN_CARD:		//用户开牌
		{
			//用户效验
			tagServerUserData * pUserData=pIServerUserItem->GetUserData();
			if (pUserData->cbUserStatus!=US_PLAY)
			{
				bRet = false;
				break;
			}

			//状态判断
			ASSERT(m_cbPlayStatus[pUserData->wChairID]==TRUE);
			if (m_cbPlayStatus[pUserData->wChairID]==FALSE)
			{
				bRet = false;
				break;
			}

			//消息处理
			if(wSubCmdID==SUB_C_LOOK_CARD)
			{//看牌
				bRet = OnUserLookCard(pUserData->wChairID);
			}
			else if(wSubCmdID==SUB_C_OPEN_CARD)
			{//开牌
				bRet = OnUserOpenCard(pUserData->wChairID);
			}
			break;
		}
	case SUB_C_COMPARE_CARD:	//用户比牌
		{
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_C_CompareCard));
			if (wDataSize!=sizeof(CMD_C_CompareCard))
			{
				bRet = false;
				break;
			}

			//变量定义
			CMD_C_CompareCard * pCompareCard=(CMD_C_CompareCard *)pDataBuffer;
			if (NULL != pCompareCard)
			{
				//用户效验
				tagServerUserData * pUserData=pIServerUserItem->GetUserData();
				if (pUserData->cbUserStatus!=US_PLAY)
				{
					bRet = false;
					break;
				}

				//参数效验
				ASSERT(pUserData->wChairID==m_wCurrentUser);
				if(pUserData->wChairID!=m_wCurrentUser)
				{
					bRet = false;
					break;
				}
				if (pCompareCard->wCompareUser >= GAME_PLAYER)
				{
					bRet = false;
					break;
				}

				//状态判断
				ASSERT(m_cbPlayStatus[pUserData->wChairID]==TRUE && m_cbPlayStatus[pCompareCard->wCompareUser]==TRUE);
				if(m_cbPlayStatus[pUserData->wChairID]==FALSE || m_cbPlayStatus[pCompareCard->wCompareUser]==FALSE)
				{
					bRet = false;
					break;
				}

				//消息处理
				bRet = OnUserCompareCard(pUserData->wChairID,pCompareCard->wCompareUser);
			}
			break;
		}
	case SUB_C_ADD_SCORE:		//用户加注
		{
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_C_AddScore));
			if (wDataSize!=sizeof(CMD_C_AddScore))
			{
				bRet = false;
				break;
			}
			//变量定义
			CMD_C_AddScore * pAddScore=(CMD_C_AddScore *)pDataBuffer;
			if (NULL==pAddScore)
			{
				bRet = false;
				break;
			}

			//用户效验
			tagServerUserData * pUserData=pIServerUserItem->GetUserData();
			if (NULL!=pUserData && pUserData->cbUserStatus!=US_PLAY)
			{
				bRet = false;
				break;
			}

			//状态判断
			ASSERT(m_cbPlayStatus[pUserData->wChairID]==TRUE);
			if (m_cbPlayStatus[pUserData->wChairID]==FALSE)
			{
				bRet = false;
				break;
			}

			//当前状态
			//if(pAddScore->wState>0)
			//{
			//	m_lCompareCount=pAddScore->lScore;
			//}
			m_lCompareCount = pAddScore->wState;

			//消息处理
			bRet = OnUserAddScore(pUserData->wChairID,pAddScore->lScore,false,((pAddScore->wState>0)?true:false));
			break;
		}
	case SUB_C_WAIT_COMPARE:	//等待比牌
		{
			//用户效验
			tagServerUserData * pUserData=pIServerUserItem->GetUserData();
			if (NULL==pUserData || pUserData->cbUserStatus!=US_PLAY)
			{
				bRet = false;
				break;
			}
			//状态判断
			ASSERT(m_cbPlayStatus[pUserData->wChairID]==TRUE);
			if (m_cbPlayStatus[pUserData->wChairID]==FALSE)
			{
				bRet = false;
				break;
			}

			//消息处理
			CMD_S_WaitCompare WaitCompare;
			WaitCompare.wCompareUser = pUserData->wChairID;
			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_WAIT_COMPARE,&WaitCompare,sizeof(CMD_S_WaitCompare));
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_WAIT_COMPARE,&WaitCompare,sizeof(CMD_S_WaitCompare));
			bRet = true;
			break;
		}
	case SUB_C_FINISH_FLASH:	//完成动画 
		{
			//用户效验
			tagServerUserData * pUserData=pIServerUserItem->GetUserData();
			if (NULL == pUserData)
			{
				bRet = false;
				break;
			}
			if (!m_wFlashUser[pUserData->wChairID])
			{
				bRet = true;
				break;
			}

			//设置变量
			m_wFlashUser[pUserData->wChairID] = FALSE;

			//结束游戏
			WORD i = 0;
			for(;i<GAME_PLAYER;i++)
			{
				if(m_wFlashUser[i])
				{
					break;
				}
			}
			if(i==GAME_PLAYER)
			{
				//删除时间
				m_pITableFrame->KillGameTimer(IDI_GAME_COMPAREEND);
				m_pITableFrame->KillGameTimer(IDI_GAME_OPENEND);

				//发送信息
				m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_END,&m_StGameEnd,sizeof(m_StGameEnd));
				m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_END,&m_StGameEnd,sizeof(m_StGameEnd));

				//结束游戏
				m_pITableFrame->ConcludeGame();
			}
			bRet = true;
			break;
		}
	}
	return true;

	//char chData[100]={0};
	//sprintf(chData, "CTableFrameSink::OnGameMessage--%d", wSubCmdID);
	//AfxMessageBox(chData);
	//return false;
}


/*
//游戏消息处理
bool __cdecl CTableFrameSink::OnGameMessage(WORD wSubCmdID, const void * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	switch (wSubCmdID)
	{
	case SUB_C_GIVE_UP:			//用户放弃
		{
			//用户效验
			tagServerUserData * pUserData=pIServerUserItem->GetUserData();
			if (pUserData->cbUserStatus!=US_PLAY) return true;

			//状态判断
			ASSERT(m_cbPlayStatus[pUserData->wChairID]==TRUE);
			if (m_cbPlayStatus[pUserData->wChairID]==FALSE) return false;

			//消息处理
			return OnUserGiveUp(pUserData->wChairID);
		}
	case SUB_C_LOOK_CARD:		//用户看牌
	case SUB_C_OPEN_CARD:		//用户开牌
		{
			//用户效验
			tagServerUserData * pUserData=pIServerUserItem->GetUserData();
			if (pUserData->cbUserStatus!=US_PLAY) return true;

			//状态判断
			ASSERT(m_cbPlayStatus[pUserData->wChairID]==TRUE);
			if (m_cbPlayStatus[pUserData->wChairID]==FALSE) return false;

			//消息处理
			if(wSubCmdID==SUB_C_LOOK_CARD)return OnUserLookCard(pUserData->wChairID);
			return OnUserOpenCard(pUserData->wChairID);
		}
	case SUB_C_COMPARE_CARD:	//用户比牌
		{
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_C_CompareCard));
			if (wDataSize!=sizeof(CMD_C_CompareCard)) return false;

			//变量定义
			CMD_C_CompareCard * pCompareCard=(CMD_C_CompareCard *)pDataBuffer;

			//用户效验
			tagServerUserData * pUserData=pIServerUserItem->GetUserData();
			if (pUserData->cbUserStatus!=US_PLAY) return true;

			//参数效验
			ASSERT(pUserData->wChairID==m_wCurrentUser);
			if(pUserData->wChairID!=m_wCurrentUser)return false;

			//状态判断
			ASSERT(m_cbPlayStatus[pUserData->wChairID]==TRUE && m_cbPlayStatus[pCompareCard->wCompareUser]==TRUE);
			if(m_cbPlayStatus[pUserData->wChairID]==FALSE || m_cbPlayStatus[pCompareCard->wCompareUser]==FALSE)return false;

			//消息处理
			return OnUserCompareCard(pUserData->wChairID,pCompareCard->wCompareUser);
		}
	case SUB_C_ADD_SCORE:		//用户加注
		{
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_C_AddScore));
			if (wDataSize!=sizeof(CMD_C_AddScore)) return false;

			//变量定义
			CMD_C_AddScore * pAddScore=(CMD_C_AddScore *)pDataBuffer;

			//用户效验
			tagServerUserData * pUserData=pIServerUserItem->GetUserData();
			if (pUserData->cbUserStatus!=US_PLAY) return true;

			//状态判断
			ASSERT(m_cbPlayStatus[pUserData->wChairID]==TRUE);
			if (m_cbPlayStatus[pUserData->wChairID]==FALSE) return false;

			//当前状态
			if(pAddScore->wState>0)m_lCompareCount=pAddScore->lScore;

			//消息处理
			return OnUserAddScore(pUserData->wChairID,pAddScore->lScore,false,((pAddScore->wState>0)?true:false));
		}
	case SUB_C_WAIT_COMPARE:	//等待比牌
		{
			//用户效验
			tagServerUserData * pUserData=pIServerUserItem->GetUserData();
			if (pUserData->cbUserStatus!=US_PLAY) return true;

			//状态判断
			ASSERT(m_cbPlayStatus[pUserData->wChairID]==TRUE);
			if (m_cbPlayStatus[pUserData->wChairID]==FALSE) return false;

			//消息处理
			CMD_S_WaitCompare WaitCompare;
			WaitCompare.wCompareUser = pUserData->wChairID;
			if(m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_WAIT_COMPARE,&WaitCompare,sizeof(WaitCompare)) &&
				m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_WAIT_COMPARE,&WaitCompare,sizeof(WaitCompare)) )
				return true;
		}
	case SUB_C_FINISH_FLASH:	//完成动画 
		{
			//用户效验
			tagServerUserData * pUserData=pIServerUserItem->GetUserData();
			if (pUserData->cbUserStatus!=US_PLAY) return true;

			//状态判断
			ASSERT(m_wFlashUser[pUserData->wChairID]==TRUE);
			if (m_wFlashUser[pUserData->wChairID]==FALSE) return true;

			//设置变量
			m_wFlashUser[pUserData->wChairID] = FALSE;

			//结束游戏
			for(WORD i=0;i<GAME_PLAYER;i++)
			{
				if(m_wFlashUser[i] != FALSE)break;
			}
			if(i==GAME_PLAYER)
			{
				//删除时间
				m_pITableFrame->KillGameTimer(IDI_GAME_COMPAREEND);
				m_pITableFrame->KillGameTimer(IDI_GAME_OPENEND);

				//发送信息
				m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_END,&m_StGameEnd,sizeof(m_StGameEnd));
				m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_END,&m_StGameEnd,sizeof(m_StGameEnd));

				//结束游戏
				m_pITableFrame->ConcludeGame();
			}
			return true;
		}
	}

	return false;
}
*/

//框架消息处理
bool __cdecl CTableFrameSink::OnFrameMessage(WORD wSubCmdID, const void * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	return false;
}

//放弃事件
bool CTableFrameSink::OnUserGiveUp(WORD wChairID,bool bExit)
{
	//设置数据
	m_cbPlayStatus[wChairID] = FALSE;

	//发送消息
	CMD_S_GiveUp GiveUp;
	GiveUp.wGiveUpUser=wChairID;
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GIVE_UP,&GiveUp,sizeof(GiveUp));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GIVE_UP,&GiveUp,sizeof(GiveUp));

	//写入积分
	m_pITableFrame->WriteUserScore(wChairID,-m_lTableScore[wChairID],0,(bExit?enScoreKind_Flee:enScoreKind_Lost));

	//人数统计
	WORD wPlayerCount=0;
	for (WORD i=0;i<m_wPlayerCount;i++)
	{
		if (m_cbPlayStatus[i]==TRUE) wPlayerCount++;
	}

	//判断结束
	if (wPlayerCount>=2)
	{
		if (m_wCurrentUser==wChairID)
		{
			OnUserAddScore(wChairID,0L,true,false);
		}
	}
	else OnEventGameEnd(INVALID_CHAIR,NULL,GER_NO_PLAYER);

	return true;
}

//看牌事件
bool CTableFrameSink::OnUserLookCard(WORD wChairID)
{
	//状态效验
	ASSERT(m_wCurrentUser==wChairID);
	if (m_wCurrentUser!=wChairID) return false;

	//参数效验
	ASSERT(!m_bMingZhu[wChairID]);
	if (m_bMingZhu[wChairID]) return true;

	//设置参数
	m_bMingZhu[wChairID]=true;

	//构造数据
	CMD_S_LookCard LookCard;
	CopyMemory(LookCard.cbCardData,m_cbHandCardData[wChairID],sizeof(m_cbHandCardData[0]));
	LookCard.wLookCardUser=wChairID;

	//发送数据
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_LOOK_CARD,&LookCard,sizeof(LookCard));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_LOOK_CARD,&LookCard,sizeof(LookCard));

	return true;
}

//比牌事件
bool CTableFrameSink::OnUserCompareCard(WORD wFirstChairID,WORD wNextChairID)
{
	//效验参数
	ASSERT(m_lCompareCount>0);
	if(m_lCompareCount<=0)return true;
	//LONGLONG lTemp=(m_bMingZhu[m_wCurrentUser])?6:5;
	//ASSERT((m_lUserMaxScore[m_wCurrentUser]-m_lTableScore[m_wCurrentUser]+m_lCompareCount) >= (m_lMaxCellScore*lTemp));
	//if((m_lUserMaxScore[m_wCurrentUser]-m_lTableScore[m_wCurrentUser]+m_lCompareCount) < (m_lMaxCellScore*lTemp))return false;
	//ASSERT(m_wCurrentUser==m_wBankerUser || m_lTableScore[m_wCurrentUser]-m_lCompareCount>=2*m_lCellScore);
	//if(!(m_wCurrentUser==m_wBankerUser || m_lTableScore[m_wCurrentUser]-m_lCompareCount>=2*m_lCellScore))
	//{
	//	return true;
	//}

	//比较大小
	WORD wCompareCardStatus = m_GameLogic.CompareCard(m_cbHandCardData[wFirstChairID],m_cbHandCardData[wNextChairID],MAX_COUNT);

	//状态设置
	m_lCompareCount=0;

	//胜利用户
	WORD wLostUser=INVALID_CHAIR,wWinUser=INVALID_CHAIR;
	if(1==wCompareCardStatus)
	{//First win
		wWinUser=wFirstChairID;
		wLostUser=wNextChairID;
	}
	else
	{//
		wWinUser=wNextChairID;
		wLostUser=wFirstChairID;
	}

	//设置数据
	m_wCompardUser[wLostUser].Add(wWinUser);
	m_wCompardUser[wWinUser].Add(wLostUser);
	m_cbPlayStatus[wLostUser]=FALSE;

	//人数统计
	WORD wPlayerCount=0;
	for (WORD i=0;i<m_wPlayerCount;i++)
	{
		if (m_cbPlayStatus[i]==TRUE) wPlayerCount++;
	}

	//继续游戏
	if(wPlayerCount>=2)
	{
		//用户切换
		WORD wNextPlayer=INVALID_CHAIR;
		for (WORD i=1;i<m_wPlayerCount;i++)
		{
			//设置变量
			wNextPlayer=(m_wCurrentUser+i)%m_wPlayerCount;

			//继续判断
			if (m_cbPlayStatus[wNextPlayer]==TRUE) break;
		}
		//设置用户
		m_wCurrentUser=wNextPlayer;
	}
	else m_wCurrentUser=INVALID_CHAIR;

	//构造数据
	CMD_S_CompareCard CompareCard;
	CompareCard.wCurrentUser = m_wCurrentUser;//next
	CompareCard.wLostUser = wLostUser;
	CompareCard.wCompareUser[0] = wFirstChairID;
	CompareCard.wCompareUser[1] = wNextChairID;

	//发送数据
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_COMPARE_CARD,&CompareCard,sizeof(CMD_S_CompareCard));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_COMPARE_CARD,&CompareCard,sizeof(CMD_S_CompareCard));

	//写入积分
	m_pITableFrame->WriteUserScore(wLostUser,-m_lTableScore[wLostUser],0,enScoreKind_Lost);

	//结束游戏
	if (wPlayerCount<2 || INVALID_CHAIR==m_wCurrentUser)
	{
		m_wBankerUser=wWinUser;
		m_wFlashUser[wNextChairID] = TRUE;
		m_wFlashUser[wFirstChairID] = TRUE;
		OnEventGameEnd(GAME_PLAYER,NULL,GER_COMPARECARD);
	}

	return true;
}

//开牌事件
bool CTableFrameSink::OnUserOpenCard(WORD wUserID)
{
	//效验参数
	ASSERT(m_lCompareCount>0);
	if(m_lCompareCount<=0)return true;
	ASSERT(m_wCurrentUser==wUserID);
	if(m_wCurrentUser!=wUserID)
	{
		return false;
	}
	//LONGLONG lTemp=(m_bMingZhu[wUserID])?6:5;
	//ASSERT((m_lUserMaxScore[wUserID]-m_lTableScore[wUserID]+m_lCompareCount) < (m_lMaxCellScore*lTemp));
	//if((m_lUserMaxScore[wUserID]-m_lTableScore[wUserID]+m_lCompareCount) >= (m_lMaxCellScore*lTemp))return false;

	//清理数据
	m_wCurrentUser=INVALID_CHAIR;
	m_lCompareCount = 0;

	//保存扑克
	BYTE cbUserCardData[GAME_PLAYER][MAX_COUNT];
	CopyMemory(cbUserCardData,m_cbHandCardData,sizeof(cbUserCardData));

	//比牌玩家
	WORD wWinner=wUserID;

	//查找最大玩家
	for (WORD i=1;i<m_wPlayerCount;i++)
	{
		WORD w=(wUserID+i)%m_wPlayerCount;

		//用户过滤
		if (m_cbPlayStatus[w]==FALSE) continue;

		//对比扑克
		if (m_GameLogic.CompareCard(cbUserCardData[w],cbUserCardData[wWinner],MAX_COUNT)>=TRUE) 
		{
			wWinner=w;
		}
	}
	ASSERT(m_cbPlayStatus[wWinner]==TRUE);
	if(m_cbPlayStatus[wWinner]==FALSE)
	{
		return true;
	}

	//胜利玩家
	m_wBankerUser = wWinner;

	//构造数据
	CMD_S_OpenCard OpenCard;
	OpenCard.wWinner=wWinner;

	//发送数据
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_OPEN_CARD,&OpenCard,sizeof(CMD_S_OpenCard));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_OPEN_CARD,&OpenCard,sizeof(CMD_S_OpenCard));

	//结束游戏
	for(WORD i=0;i<GAME_PLAYER;i++)
	{
		if(m_cbPlayStatus[i]==TRUE)
		{
			m_wFlashUser[i] = TRUE;
			OnEventGameEnd(GAME_PLAYER,NULL,GER_OPENCARD);
		}
	 
	}


	return true;
}

//加注事件
bool CTableFrameSink::OnUserAddScore(WORD wChairID, LONGLONG lScore, bool bGiveUp, bool bCompareCard)
{
	if (bGiveUp==false)				//设置数据
	{
		//状态效验
		ASSERT(m_wCurrentUser==wChairID);
		if (m_wCurrentUser!=wChairID) return false;

		//金币效验
		ASSERT(lScore>=0 && lScore%m_lCellScore==0);
		ASSERT((lScore+m_lTableScore[wChairID])<=m_lUserMaxScore[wChairID]);
		if (lScore<0 || lScore%m_lCellScore!=0) return false;
		if ((lScore+m_lTableScore[wChairID])>m_lUserMaxScore[wChairID])
		{
			m_lTableScore[wChairID] = m_lUserMaxScore[wChairID];
		}

		//当前倍数	bGiveUp过滤了lScore为0
		//看完牌后押注要翻倍，如果还要比牌的话则翻两番
		LONGLONG lTimes=(m_bMingZhu[wChairID] || bCompareCard)?2:1;
		if(m_bMingZhu[wChairID] && bCompareCard)lTimes=4;
		LONGLONG lTemp=lScore/m_lCellScore/lTimes;
		ASSERT(m_lCurrentTimes <= lTemp && m_lCurrentTimes <= m_lMaxCellScore/m_lCellScore);
		if(!(m_lCurrentTimes <= lTemp && m_lCurrentTimes <= m_lMaxCellScore/m_lCellScore))
		{
			return false;
		}
		m_lCurrentTimes = lTemp;

		//用户注金
		m_lTableScore[wChairID]+=lScore;
	}

	//设置用户
	if(!bCompareCard)
	{
		//用户切换
		WORD wNextPlayer=INVALID_CHAIR;
		for (WORD i=1;i<m_wPlayerCount;i++)
		{
			//设置变量
			wNextPlayer=(m_wCurrentUser+i)%m_wPlayerCount;

			//继续判断
			if (m_cbPlayStatus[wNextPlayer]==TRUE) break;
		}
		m_wCurrentUser=wNextPlayer;
	}

	//构造数据
	CMD_S_AddScore AddScore;
	AddScore.lCurrentTimes=m_lCurrentTimes;
	AddScore.wCurrentUser=m_wCurrentUser;
	AddScore.wAddScoreUser=wChairID;
	AddScore.lAddScoreCount=lScore;

	//判断状态
	AddScore.wCompareState=( bCompareCard )?TRUE:FALSE;

	//发送数据
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_ADD_SCORE,&AddScore,sizeof(CMD_S_AddScore));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_ADD_SCORE,&AddScore,sizeof(CMD_S_AddScore));

	return true;
}

//////////////////////////////////////////////////////////////////////////
