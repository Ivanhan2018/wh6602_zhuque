#include "GameClient.h"
#include "AndroidAI.h"

//定时器
#define				IDI_GT_GAME_USER_READY					1000					//玩家准备
#define				IDI_OPERATE_CARD						1001					//操作牌时间
#define				IDI_GT_GAME_USER_NO_OUTCARD				1002					//玩家不出定时器

#define				TIME_OPERATE_CARD						4						//操作定时器
#define				TIME_LESS								2						//最少时间
#define				TIME_GAME_USER_NO_OUTCARD_INTERVAL		30						//最大出牌时间

BEGIN_MESSAGE_MAP(CGameClient, CGameClientFrame)
	ON_WM_TIMER()
END_MESSAGE_MAP()


CGameClient::CGameClient(void)
{
	//游戏变量
	m_wBankerUser=INVALID_CHAIR;
	m_wCurrentUser=INVALID_CHAIR;
	m_cbActionMask = 0;
	m_cbActionCard = 0;

	//组合扑克
	ZeroMemory(m_cbWeaveCount,sizeof(m_cbWeaveCount));
	ZeroMemory(m_WeaveItemArray,sizeof(m_WeaveItemArray));

	//扑克变量
	ZeroMemory(m_cbCardIndex,sizeof(m_cbCardIndex));

	return;
}

CGameClient::~CGameClient(void)
{
}

//初始函数
bool CGameClient::InitGameFrame()
{
	//游戏变量
	m_wBankerUser=INVALID_CHAIR;
	m_wCurrentUser=INVALID_CHAIR;
	m_cbActionMask = 0;
	m_cbActionCard = 0;
	m_cbLeftCardCount=0;
	m_cbOutCardData=0;
	//组合扑克
	ZeroMemory(m_cbWeaveCount,sizeof(m_cbWeaveCount));
	ZeroMemory(m_WeaveItemArray,sizeof(m_WeaveItemArray));

	//扑克变量
	ZeroMemory(m_cbCardIndex,sizeof(m_cbCardIndex));

	return true;
}

//重置框架
void CGameClient::ResetGameFrame()
{
}

//取消所有定时器
void CGameClient::CancelAllTimer()
{
	KillGameTimer(IDI_GT_GAME_USER_READY);
	KillGameTimer(IDI_OPERATE_CARD);
	KillTimer(IDI_GT_GAME_USER_NO_OUTCARD);
}

//发送之前初始化
void CGameClient::InitSendCard()
{
	//游戏变量
	m_wBankerUser=INVALID_CHAIR;
	m_wCurrentUser=INVALID_CHAIR;
	m_cbActionMask = 0;
	m_cbActionCard = 0;
	m_cbLeftCardCount=0;
	m_cbOutCardData=0;
	//组合扑克
	ZeroMemory(m_cbWeaveCount,sizeof(m_cbWeaveCount));
	ZeroMemory(m_WeaveItemArray,sizeof(m_WeaveItemArray));

	//扑克变量
	ZeroMemory(m_cbCardIndex,sizeof(m_cbCardIndex));

	CancelAllTimer();//删除定时器
}

//系统事件
//时间消息
bool CGameClient::OnTimerMessage(UINT nElapse, UINT nTimerID)
{
	switch (nTimerID)
	{
	case IDI_GT_GAME_USER_READY:
		{//游戏准备定时器
			if (0==nElapse|| nElapse>100)
			{
				SendUserReady(NULL, 0);
			}
			break;
		}
	case IDI_OPERATE_CARD:		//操作定时器
		{
			if (nElapse==0)
			{
				KillGameTimer(IDI_OPERATE_CARD);
				tagOutCardResult OutCardResult;
				WORD wMeChairId = GetMeChairID();

				try
				{
					//搜索出牌
					if( SearchOutCard(OutCardResult) )
					{
						if( OutCardResult.cbOperateCode != WIK_NULL )
						{
							//效验
							ASSERT( OutCardResult.cbOperateCode&m_cbActionMask );
							if( !(OutCardResult.cbOperateCode&m_cbActionMask) ) throw 0;

							//响应操作
							OnOperateCard(OutCardResult.cbOperateCode,OutCardResult.cbOperateCard);
						}
						else
						{
							//效验
							ASSERT( m_cbCardIndex[wMeChairId][m_GameLogic.SwitchToCardIndex(OutCardResult.cbOperateCard)] > 0 );
							if( m_cbCardIndex[wMeChairId][m_GameLogic.SwitchToCardIndex(OutCardResult.cbOperateCard)] == 0 ) throw 0;

							//出牌
							OnOutCard(OutCardResult.cbOperateCard);
						}
					}else
					{
						//效验
						ASSERT( wMeChairId != m_wCurrentUser );
						if( wMeChairId == m_wCurrentUser ) throw 0;

						//响应操作
						OnOperateCard(WIK_NULL,0);
					}
				}catch(...)
				{
					ASSERT(FALSE);
					//异常处理
					if( wMeChairId == m_wCurrentUser )
					{
						BYTE cbRand = rand()%MAX_INDEX;
						for( BYTE j = 0; j < MAX_INDEX; j++ )
						{
							BYTE i = (cbRand+j)%MAX_INDEX;
							if( m_cbCardIndex[wMeChairId][i] > 0 )
							{
								OnOutCard( m_GameLogic.SwitchToCardData(i) );
								return true;
							}
						}
					}
					else OnOperateCard( WIK_NULL,0 );
				}
			}
			break;
		}
	}
	return true;
}

//定时器
void CGameClient::OnTimer(UINT nIDEvent)
{
	CGameClientFrame::OnTimer(nIDEvent);
	switch (nIDEvent)
	{
	case IDI_GT_GAME_USER_NO_OUTCARD:
		{
			KillTimer(IDI_GT_GAME_USER_NO_OUTCARD);
			if (m_wCurrentUser == GetMeChairID())
			{
				BYTE cbRand = rand()%MAX_INDEX;
				for( BYTE j = 0; j < MAX_INDEX; j++ )
				{
					BYTE i = (cbRand+j)%MAX_INDEX;
					if( m_cbCardIndex[m_wCurrentUser][i] > 0 )
					{
						OnOutCard( m_GameLogic.SwitchToCardData(i) );
						break;
					}
				}
			}
			break;
		}
	default:
		break;
	}
}

//游戏场景
bool CGameClient::OnGameSceneMessage(BYTE cbGameStatus, const void * pBuffer, WORD wDataSize)
{
	switch (cbGameStatus)
	{
	case GS_MJ_FREE:	//空闲状态
		{
			SetGameTimer(IDI_GT_GAME_USER_READY,rand()%TIME_OPERATE_CARD+TIME_LESS);
			break;
		}
	default:
		{
			break;
		}
	}
	return true;
}

//网络消息
bool CGameClient::OnGameMessage(WORD wSubCmdID, const void * pBuffer, WORD wDataSize)
{
	bool bRet = false;
	switch (wSubCmdID)
	{
	case SUB_S_GAME_START:		//游戏开始
		{
			bRet = OnSubGameStart(pBuffer,wDataSize);
			break;
		}
	case SUB_S_OUT_CARD:		//用户出牌
		{
			bRet = OnSubOutCard(pBuffer,wDataSize);
			break;
		}
	case SUB_S_SEND_CARD:		//发牌消息
		{
			bRet = OnSubSendCard(pBuffer,wDataSize);
			break;
		}
	case SUB_S_OPERATE_NOTIFY:	//操作提示
		{
			bRet = OnSubOperateNotify(pBuffer,wDataSize);
			break;
		}
	case SUB_S_OPERATE_RESULT:	//操作结果
		{
			bRet = OnSubOperateResult(pBuffer,wDataSize);
			break;
		}
	case SUB_S_OPERATE_HAI_DI:	//海底操作
		{
			bRet = OnSubOperateHaiDi(pBuffer,wDataSize);
			break;
		}
	case SUB_S_GAME_END:		//游戏结束
		{
			bRet = OnSubGameEnd(pBuffer,wDataSize);
			break;
		}
	case SUB_S_TRUSTEE:			//用户托管
		{
			bRet = true;
			break;
		}
	}

	return true;
}

//游戏开始
bool CGameClient::OnSubGameStart(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_GameStart));
	if (wDataSize!=sizeof(CMD_S_GameStart))
	{
		return false;
	}
	
	//初始化
	InitSendCard();

	//变量定义
	CMD_S_GameStart * pGameStart=(CMD_S_GameStart *)pBuffer;
	if (NULL != pGameStart)
	{
		BYTE byMeChairID = GetMeChairID();
		//设置变量
		m_wBankerUser=pGameStart->wBankerUser;					//庄家用户
		m_wCurrentUser=pGameStart->wCurrentUser;				//当前出牌用户
		m_cbActionMask=pGameStart->cbUserAction;				//胡牌类型
		m_cbLeftCardCount=MAX_REPERTORY-GAME_PLAYER*(MAX_COUNT-1)-1;
		//出牌信息
		m_wOutCardUser=INVALID_CHAIR;							//出牌用户
		ZeroMemory(m_cbDiscardCard,sizeof(m_cbDiscardCard));	//丢弃牌数据
		ZeroMemory(m_cbDiscardCount,sizeof(m_cbDiscardCount));	//丢弃牌数量

		//组合扑克
		ZeroMemory(m_cbWeaveCount,sizeof(m_cbWeaveCount));
		ZeroMemory(m_WeaveItemArray,sizeof(m_WeaveItemArray));

		//设置扑克
		BYTE cbCardCount=(byMeChairID==m_wBankerUser)?MAX_COUNT:(MAX_COUNT-1);
		m_GameLogic.SwitchToCardIndex(pGameStart->cbCardData,cbCardCount,m_cbCardIndex[byMeChairID]);

		/*
		BYTE bIndex = 0;
		for( WORD i = 0; i < GAME_PLAYER; i++ )
		{
			if( i == byMeChairID ) continue;
			cbCardCount=(i==m_wBankerUser)?MAX_COUNT:(MAX_COUNT-1);
			m_GameLogic.SwitchToCardIndex(&pGameStart->cbCardData[MAX_COUNT*bIndex++],cbCardCount,m_cbCardIndex[i]);
		}
		*/
		//动作处理
		if ((m_cbActionMask!=WIK_NULL) || m_wCurrentUser==byMeChairID)
		{
			UINT nElapse = rand()%TIME_OPERATE_CARD+TIME_LESS;
			SetGameTimer(IDI_OPERATE_CARD,nElapse);
			SetTimer(IDI_GT_GAME_USER_NO_OUTCARD, TIME_GAME_USER_NO_OUTCARD_INTERVAL, NULL);
			return true;
		}
	}
	
	return true;
}

//游戏结束
bool CGameClient::OnSubGameEnd(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_GameEnd));
	if (wDataSize!=sizeof(CMD_S_GameEnd))
	{
		return false;
	}

	//消息处理
	CMD_S_GameEnd * pGameEnd=(CMD_S_GameEnd *)pBuffer;
	if (NULL != pGameEnd)
	{
		//重置所有数据
		InitSendCard();

		//开始准备
		int iTimeRand = rand()%4 + 1;
		SetGameTimer(IDI_GT_GAME_USER_READY, iTimeRand);
	}
	return true;
}

//海底操作
bool CGameClient::OnSubOperateHaiDi(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	if (wDataSize!=sizeof(CMD_S_OperateHaiDi)) return false;

	//消息处理
	CMD_S_OperateHaiDi * pOperateHaiDi=(CMD_S_OperateHaiDi *)pBuffer;

	//设置变量
	m_wCurrentUser=INVALID_CHAIR;

	if (NULL != pOperateHaiDi && GetMeChairID()==pOperateHaiDi->wCurrentUser)
	{
		//构造消息
		CMD_C_OperateHaiDi OperateHaiDi;
		OperateHaiDi.cbTakeHaiDi=1;
		SendData(SUB_C_OPERATE_HAI_DI,&OperateHaiDi,sizeof(OperateHaiDi));
	}
	
	return true;
}

//出牌
void CGameClient::OnOutCard( BYTE cbOutCard )
{
	//删除定时器
	KillGameTimer(IDI_OPERATE_CARD);

	//设置变量
	m_wCurrentUser=INVALID_CHAIR;
	m_cbActionMask = WIK_NULL;
	m_cbActionCard = 0;

	//删除牌
	m_GameLogic.RemoveCard(m_cbCardIndex[GetMeChairID()],cbOutCard);

	//发送数据
	CMD_C_OutCard OutCard;
	OutCard.cbCardData=cbOutCard;
	if( !SendData(SUB_C_OUT_CARD,&OutCard,sizeof(OutCard)) )
	{
		ASSERT( FALSE );
		return ;
	}

	return ;
}

//用户出牌
bool CGameClient::OnSubOutCard( const void *pBuffer,WORD wDataSize )
{
	//效验消息
	ASSERT(wDataSize==sizeof(CMD_S_OutCard));
	if (wDataSize!=sizeof(CMD_S_OutCard)) return false;

	//消息处理
	CMD_S_OutCard * pOutCard=(CMD_S_OutCard *)pBuffer;

	if (NULL != pOutCard)
	{
		//变量定义
		WORD wMeChairID=GetMeChairID();

		//设置变量
		m_wCurrentUser=INVALID_CHAIR;
		m_wOutCardUser=pOutCard->wOutCardUser;
		ASSERT( pOutCard->cbOutCardData != 0 );
		m_cbOutCardData=pOutCard->cbOutCardData;

		//删除扑克
		if( wMeChairID != pOutCard->wOutCardUser )
			m_GameLogic.RemoveCard(m_cbCardIndex[pOutCard->wOutCardUser],pOutCard->cbOutCardData);
	}
	
	return true;
}

//用户发牌
bool CGameClient::OnSubSendCard( const void *pBuffer,WORD wDataSize )
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_SendCard));
	if (wDataSize!=sizeof(CMD_S_SendCard)) return false;

	//变量定义
	CMD_S_SendCard * pSendCard=(CMD_S_SendCard *)pBuffer;

	if (NULL != pSendCard)
	{
		//设置变量
		WORD wMeChairID=GetMeChairID();
		m_wCurrentUser=pSendCard->wCurrentUser;

		//丢弃扑克
		if ((m_wOutCardUser!=INVALID_CHAIR)&&(m_cbOutCardData!=0))
		{
			//丢弃扑克
			m_cbDiscardCard[m_wOutCardUser][m_cbDiscardCount[m_wOutCardUser]++] = m_cbOutCardData;
			//设置变量
			m_cbOutCardData=0;
			m_wOutCardUser=INVALID_CHAIR;
		}

		//发牌处理
		if (pSendCard->cbCardData!=0)
		{
			m_cbCardIndex[pSendCard->wCurrentUser][m_GameLogic.SwitchToCardIndex(pSendCard->cbCardData)]++;	
			//扣除扑克
			m_cbLeftCardCount--;
		}

		//设置时间
		if( wMeChairID == m_wCurrentUser )
		{
			m_cbActionMask = pSendCard->cbActionMask;
			m_cbActionCard = pSendCard->cbCardData;

			//计算时间
			UINT nElapse=rand()%TIME_OPERATE_CARD+TIME_LESS;
			SetGameTimer(IDI_OPERATE_CARD,nElapse);
		}
	}
	
	return true;
}

//操作结果通知
bool CGameClient::OnSubOperateResult( const void *pBuffer,WORD wDataSize )
{
	//效验消息
	ASSERT(wDataSize==sizeof(CMD_S_OperateResult));
	if (wDataSize!=sizeof(CMD_S_OperateResult)) return false;

	//消息处理
	CMD_S_OperateResult * pOperateResult=(CMD_S_OperateResult *)pBuffer;

	KillGameTimer( IDI_OPERATE_CARD );

	if (NULL != pOperateResult)
	{
		//变量定义
		BYTE cbPublicCard=TRUE;
		WORD wOperateUser=pOperateResult->wOperateUser;
		BYTE cbOperateCard[3]={pOperateResult->cbOperateCard,pOperateResult->cbOperateCard,pOperateResult->cbOperateCard};

		//出牌变量
		if (pOperateResult->cbOperateCode!=WIK_NULL)
		{
			m_cbOutCardData=0;
			m_wOutCardUser=INVALID_CHAIR;
		}

		//设置变量
		m_cbActionMask = WIK_NULL;
		m_cbActionCard = 0;

		//设置组合
		if ((pOperateResult->cbOperateCode&WIK_GANG)!=0)
		{
			//设置变量
			m_wCurrentUser=INVALID_CHAIR;

			//组合扑克
			BYTE cbWeaveIndex=0xFF;
			for (BYTE i=0;i<m_cbWeaveCount[wOperateUser];i++)
			{
				BYTE cbWeaveKind=m_WeaveItemArray[wOperateUser][i].cbWeaveKind;
				BYTE cbCenterCard=m_WeaveItemArray[wOperateUser][i].cbCenterCard;
				if ((cbCenterCard==cbOperateCard[0])&&(cbWeaveKind==WIK_PENG))
				{
					cbWeaveIndex=i;
					m_WeaveItemArray[wOperateUser][cbWeaveIndex].cbPublicCard=TRUE;
					m_WeaveItemArray[wOperateUser][cbWeaveIndex].cbWeaveKind=pOperateResult->cbOperateCode;
					m_WeaveItemArray[wOperateUser][cbWeaveIndex].wProvideUser = pOperateResult->wProvideUser;
					break;
				}
			}

			//组合扑克
			if (cbWeaveIndex==0xFF)
			{
				//暗杠判断
				cbPublicCard=(pOperateResult->wProvideUser==wOperateUser)?FALSE:TRUE;

				//设置扑克
				cbWeaveIndex=m_cbWeaveCount[wOperateUser]++;
				m_WeaveItemArray[wOperateUser][cbWeaveIndex].cbPublicCard=cbPublicCard;
				m_WeaveItemArray[wOperateUser][cbWeaveIndex].cbCenterCard=cbOperateCard[0];
				m_WeaveItemArray[wOperateUser][cbWeaveIndex].cbWeaveKind=pOperateResult->cbOperateCode;
				m_WeaveItemArray[wOperateUser][cbWeaveIndex].wProvideUser=pOperateResult->wProvideUser;
			}

			//扑克设置
			m_cbCardIndex[wOperateUser][m_GameLogic.SwitchToCardIndex(cbOperateCard[0])]=0;
		}
		else if (pOperateResult->cbOperateCode!=WIK_NULL&&pOperateResult->cbOperateCode!=WIK_CHI_HU)
		{
			//设置变量
			m_wCurrentUser=pOperateResult->wOperateUser;

			//设置组合
			BYTE cbWeaveIndex=m_cbWeaveCount[wOperateUser]++;
			m_WeaveItemArray[wOperateUser][cbWeaveIndex].cbPublicCard=TRUE;
			m_WeaveItemArray[wOperateUser][cbWeaveIndex].cbCenterCard=cbOperateCard[0];
			m_WeaveItemArray[wOperateUser][cbWeaveIndex].cbWeaveKind=pOperateResult->cbOperateCode;
			m_WeaveItemArray[wOperateUser][cbWeaveIndex].wProvideUser=pOperateResult->wProvideUser;

			//组合界面
			BYTE cbWeaveCard[4]={0,0,0,0},cbWeaveKind=pOperateResult->cbOperateCode;
			BYTE cbWeaveCardCount=m_GameLogic.GetWeaveCard(cbWeaveKind,cbOperateCard[0],cbWeaveCard);

			//删除扑克
			m_GameLogic.RemoveCard(cbWeaveCard,cbWeaveCardCount,&cbOperateCard[0],1);
			m_GameLogic.RemoveCard(m_cbCardIndex[wOperateUser],cbWeaveCard,cbWeaveCardCount-1);
		}

		//设置时间
		if (m_wCurrentUser==GetMeChairID())
		{
			//计算时间
			UINT nElapse=rand()%TIME_OPERATE_CARD+TIME_LESS;
			//设置时间
			SetGameTimer(IDI_OPERATE_CARD,nElapse);
		}
	}
	
	return true;
}

//操作通知
bool CGameClient::OnSubOperateNotify( const void *pBuffer,WORD wDataSize )
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_OperateNotify));
	if (wDataSize!=sizeof(CMD_S_OperateNotify)) return false;

	//变量定义
	CMD_S_OperateNotify * pOperateNotify=(CMD_S_OperateNotify *)pBuffer;

	//用户界面
	if (NULL != pOperateNotify)
	{
		if (pOperateNotify->cbActionMask!=WIK_NULL)
		{
			//获取变量
			WORD wMeChairID=GetMeChairID();
			m_cbActionMask = pOperateNotify->cbActionMask;
			m_cbActionCard = pOperateNotify->cbActionCard;

			//设置时间
			UINT nElapse = rand()%TIME_OPERATE_CARD+TIME_LESS;
			SetGameTimer(IDI_OPERATE_CARD,nElapse);
		}
	}
	return true;
}

//操作牌
void CGameClient::OnOperateCard( BYTE cbOperateCode,BYTE cbOperateCard )
{
	//删除时间
	KillGameTimer(IDI_OPERATE_CARD);

	//设置变量
	m_cbActionMask = WIK_NULL;
	m_cbActionCard = 0;

	//变量定义
	BYTE cbCard[3] = {cbOperateCard,0,0};

	//如果是吃牌
	if( cbOperateCode & WIK_LEFT )
	{
		cbCard[1] = cbOperateCard+1;
		cbCard[2] = cbOperateCard+2;
	}
	else if( cbOperateCode & WIK_CENTER )
	{
		cbCard[1] = cbOperateCard-1;
		cbCard[2] = cbOperateCard+1;
	}
	else if( cbOperateCode & WIK_RIGHT )
	{
		cbCard[1] = cbOperateCard-2;
		cbCard[2] = cbOperateCard-1;
	}

	//发送命令
	CMD_C_OperateCard OperateCard;
	OperateCard.cbOperateCode=cbOperateCode;
	OperateCard.cbOperateCard=cbCard[0];
	//CopyMemory( OperateCard.cbOperateCard,cbCard,sizeof(cbCard) );
	if( !SendData(SUB_C_OPERATE_CARD,&OperateCard,sizeof(OperateCard)) )
	{
		ASSERT( FALSE );
		return ;
	}

	return ;
}

//搜索出牌
bool CGameClient::SearchOutCard( tagOutCardResult &OutCardResult )
{
	//初始化
	ZeroMemory(&OutCardResult,sizeof(OutCardResult));

	WORD wMeChairId = GetMeChairID();
	ASSERT( wMeChairId != INVALID_CHAIR );
	if( wMeChairId == INVALID_CHAIR ) return false;

	//判断胡
	if( (m_cbActionMask&WIK_CHI_HU) != WIK_NULL )
	{
		OutCardResult.cbOperateCode = WIK_CHI_HU;
		OutCardResult.cbOperateCard = m_cbActionCard;
		return true;
	}

	//转换索引
	BYTE byCard[MAX_COUNT],byCardCount = 0;
	for( BYTE i = 0; i < MAX_INDEX; i++ )
	{
		for( BYTE j = 0; j < m_cbCardIndex[wMeChairId][i]; j++ )
		{
			byCard[byCardCount++] = i;
		}
	}

	BYTE cbActionCard = m_cbActionCard;

	//计算各种操作得分
	BYTE cbOperateCode[] = { WIK_TING,WIK_PENG,WIK_LEFT,WIK_CENTER,WIK_RIGHT,WIK_NULL };
	int nOperateScore[] = { 0,0,0,0,0,0 };
	//计算原始分
	CAndroidAI AndroidAi;
	AndroidAi.SetEnjoinOutCard( NULL,0 );
	AndroidAi.SetCardData(byCard,byCardCount);
	AndroidAi.Think();
	int nOrgScore = AndroidAi.GetMaxScore();
	//判断杠
	if( m_cbActionMask & WIK_TING )
	{
		if( m_wCurrentUser == wMeChairId )
		{
			tagGangCardResult GangCardResult;
			m_GameLogic.AnalyseGangCard(m_cbCardIndex[wMeChairId],m_WeaveItemArray[wMeChairId],m_cbWeaveCount[wMeChairId],GangCardResult);

			//寻找最高分杠牌
			BYTE cbGangCard = 0;
			int nMaxGangScore = -1;
			for( BYTE i = 0; i < GangCardResult.cbCardCount; i++ )
			{
				ASSERT( m_cbCardIndex[wMeChairId][m_GameLogic.SwitchToCardIndex(GangCardResult.cbCardData[i])] > 0 );
				if( m_cbCardIndex[wMeChairId][m_GameLogic.SwitchToCardIndex(GangCardResult.cbCardData[i])] == 0 ) throw 0;

				//计算杠后得分
				AndroidAi.SetCardData(byCard,byCardCount);
				AndroidAi.SetAction(WIK_TING,m_GameLogic.SwitchToCardIndex(GangCardResult.cbCardData[i]));
				AndroidAi.Think();

				int nScore = AndroidAi.GetMaxScore();
				if( nScore > nMaxGangScore )
				{
					nMaxGangScore = nScore;
					cbGangCard = GangCardResult.cbCardData[i];
				}
			}

			ASSERT(nMaxGangScore!=-1&&cbGangCard!=0);
			cbActionCard = cbGangCard;
			nOperateScore[0] = nMaxGangScore-nOrgScore;
		}
		else
		{
			ASSERT( m_cbCardIndex[wMeChairId][m_GameLogic.SwitchToCardIndex(cbActionCard)] > 0 );
			if( m_cbCardIndex[wMeChairId][m_GameLogic.SwitchToCardIndex(cbActionCard)] == 0 ) throw 0;

			//计算杠后得分
			AndroidAi.SetCardData(byCard,byCardCount);
			AndroidAi.SetAction(WIK_GANG,m_GameLogic.SwitchToCardIndex(cbActionCard));
			AndroidAi.Think();
			nOperateScore[0] = AndroidAi.GetMaxScore()-nOrgScore;
		}
	}
	//判断碰
	if( m_cbActionMask & WIK_PENG )
	{
		AndroidAi.SetCardData(byCard,byCardCount);
		AndroidAi.SetAction(WIK_PENG,m_GameLogic.SwitchToCardIndex(cbActionCard));
		AndroidAi.Think();
		BYTE byBadlyIndex = AndroidAi.GetBadlyCard();
		if( byBadlyIndex == 0xff )
		{
			byBadlyIndex = AndroidAi.GetBadlyIn2Card();
			if( byBadlyIndex == 0xff )
				byBadlyIndex = AndroidAi.GetBadlyIn3Card();
		}
		if( byBadlyIndex != 0xff )
		{
			AndroidAi.RemoveCardData( byBadlyIndex );
			AndroidAi.Think();
			nOperateScore[1] = AndroidAi.GetMaxScore()-nOrgScore;
		}
	}
	//左吃
	if( m_cbActionMask & WIK_LEFT )
	{
		AndroidAi.SetCardData(byCard,byCardCount);
		AndroidAi.SetAction(WIK_LEFT,m_GameLogic.SwitchToCardIndex(cbActionCard));
		AndroidAi.Think();
		BYTE byBadlyIndex = AndroidAi.GetBadlyCard();
		if( byBadlyIndex == 0xff )
		{
			byBadlyIndex = AndroidAi.GetBadlyIn2Card();
			if( byBadlyIndex == 0xff )
				byBadlyIndex = AndroidAi.GetBadlyIn3Card();
		}
		if( byBadlyIndex != 0xff )
		{
			AndroidAi.RemoveCardData( byBadlyIndex );
			AndroidAi.Think();
			nOperateScore[2] = AndroidAi.GetMaxScore()-nOrgScore;
		}
	}
	//中吃
	if( m_cbActionMask & WIK_CENTER )
	{
		AndroidAi.SetCardData(byCard,byCardCount);
		AndroidAi.SetAction(WIK_CENTER,m_GameLogic.SwitchToCardIndex(cbActionCard));
		AndroidAi.Think();
		BYTE byBadlyIndex = AndroidAi.GetBadlyCard();
		if( byBadlyIndex == 0xff )
		{
			byBadlyIndex = AndroidAi.GetBadlyIn2Card();
			if( byBadlyIndex == 0xff )
				byBadlyIndex = AndroidAi.GetBadlyIn3Card();
		}
		if( byBadlyIndex != 0xff )
		{
			AndroidAi.RemoveCardData( byBadlyIndex );
			AndroidAi.Think();
			nOperateScore[3] = AndroidAi.GetMaxScore()-nOrgScore;
		}
	}
	//右吃
	if( m_cbActionMask & WIK_RIGHT )
	{
		AndroidAi.SetCardData(byCard,byCardCount);
		AndroidAi.SetAction(WIK_RIGHT,m_GameLogic.SwitchToCardIndex(cbActionCard));
		AndroidAi.Think();
		BYTE byBadlyIndex = AndroidAi.GetBadlyCard();
		if( byBadlyIndex == 0xff )
		{
			byBadlyIndex = AndroidAi.GetBadlyIn2Card();
			if( byBadlyIndex == 0xff )
				byBadlyIndex = AndroidAi.GetBadlyIn3Card();
		}
		if( byBadlyIndex != 0xff )
		{
			AndroidAi.RemoveCardData( byBadlyIndex );
			AndroidAi.Think();
			nOperateScore[4] = AndroidAi.GetMaxScore()-nOrgScore;
		}
	}

	//搜索废牌
	BYTE cbOutCardData = cbActionCard;
	if( m_wCurrentUser == wMeChairId )
	{
		AndroidAi.SetCardData(byCard,byCardCount);
		AndroidAi.Think();
		BYTE byBadlyIndex = AndroidAi.GetBadlyCard();
		if( byBadlyIndex == 0xff )
		{
			byBadlyIndex = AndroidAi.GetBadlyIn2Card();
			if( byBadlyIndex == 0xff )
			{
				byBadlyIndex = AndroidAi.GetBadlyIn3Card();
				ASSERT( byBadlyIndex != 0xff );
				if( byBadlyIndex == 0xff ) throw 0;
			}
		}
		AndroidAi.RemoveCardData( byBadlyIndex );
		AndroidAi.Think();
		nOperateScore[5] = AndroidAi.GetMaxScore()-nOrgScore;
		cbOutCardData = m_GameLogic.SwitchToCardData(byBadlyIndex);
	}

	//获取最高分操作
	BYTE cbIndex = 0;
	for( BYTE i = 1; i < CountArray(nOperateScore); i++ )
	{
		if( nOperateScore[cbIndex] < nOperateScore[i] )
			cbIndex = i;
	}
	if( (cbOperateCode[cbIndex]&m_cbActionMask) && 
		( cbOperateCode[cbIndex]==WIK_GANG || nOperateScore[cbIndex] > 0 ) )
	{
		OutCardResult.cbOperateCode = cbOperateCode[cbIndex];
		OutCardResult.cbOperateCard = cbActionCard;
		return true;
	}
	else
	{
		if( m_wCurrentUser == wMeChairId )
		{
			OutCardResult.cbOperateCard = cbOutCardData;
			return true;
		}
		else return false;
	}

	ASSERT( FALSE );
	throw 0;
}

