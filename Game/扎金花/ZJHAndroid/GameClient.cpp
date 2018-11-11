#include "gameclient.h"

//定时器
//玩家准备
#define				IDI_GT_GAME_USER_READY					3001
//玩家下注
#define				IDI_GT_GAME_USER_FIRSTXZ				3002
//玩家加注
#define				IDI_GT_GAME_USER_JIAZHU					3003
//玩家看牌
#define				IDI_GT_GAME_USER_LOOKCARD				3004


//时间间隔
#define				TIME_GAME_USER_READY_INTERVAL			5
#define				TIME_GAME_USER_FIRSTXZ_INTERVAL			2
#define				TIME_GAME_USER_JIAZHU_INTERVAL			4
#define				TIME_GAME_USER_LOOKCARD_INTERVAL		1



BEGIN_MESSAGE_MAP(CGameClient, CGameClientFrame)
	ON_WM_TIMER()
END_MESSAGE_MAP()



CGameClient::CGameClient(void)
{
	InitGameFrame();
	::srand((unsigned int)::time(NULL));
}

CGameClient::~CGameClient(void)
{
}


//初始函数
bool CGameClient::InitGameFrame()
{
	//当前用户
	m_wCurrentUser = INVALID_CHAIR;
	//庄家用户
	m_wBankerUser = INVALID_CHAIR;
	//当局赢家
	m_wCurrentUser = INVALID_CHAIR;
	//玩家个数
	m_wPlayerNums = 0;
	//封顶数目
	m_lMaxScore = 0;
	//单元上限
	m_lMaxCellScore = 0;
	//单元下注
	m_lCellScore = 0;
	//当前倍数
	m_lCurrentTimes = 1;
	//最大分数
	m_lUserMaxScore = 0;
	//下注次数
	::memset(m_wUserXZCount, 0, sizeof(m_wUserXZCount));
	//玩家下注值
	::memset(m_lTableScore, 0, sizeof(m_lTableScore));
	//是否看牌
	::memset(m_bIsLookCard, 0, sizeof(m_bIsLookCard));
	//是否有效玩家
	::memset(m_cbPlayStatus, 0, sizeof(m_cbPlayStatus));
	//玩家手牌
	::memset(m_byUserCard, 0, sizeof(m_byUserCard));
	//看牌的玩家列表
	m_vecLookCardUserList.clear();
	//输牌或放弃玩家列表
	m_vecLostUserList.clear();
	m_vecWinUserList.clear();
	m_vecCancelUserList.clear();
	//连接跟注次数
	m_wContinuouFollowNums = 0;
	//看牌以后不操作
	m_bIsLookCardLaterNoOperation = false;
	//跟到开牌
	m_bIsFollowToOpenCard = false;
	//玩家是否有动作
	m_bIsUserAction = false;
	::srand((unsigned int)::time(NULL));
	return true;
}

//重置框架
void CGameClient::ResetGameFrame()
{
	InitGameFrame();
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
	case IDI_GT_GAME_USER_FIRSTXZ:
		{//第一次下注
			if (0==nElapse|| nElapse>100)
			{
				FirstXZ(true);
			}
			break;
		}
	case IDI_GT_GAME_USER_JIAZHU:
		{//加注定时器
			if (0==nElapse|| nElapse>100)
			{
				AutoJudge();
			}
			break;
		}
	case IDI_GT_GAME_USER_LOOKCARD:
		{//看牌定时器
			if (0==nElapse|| nElapse>100)
			{
				LookCardLaterOpera();
			}
			break;
		}
	}
	return true;
}


//游戏场景
bool CGameClient::OnGameSceneMessage(BYTE cbGameStatus, const void * pBuffer, WORD wDataSize)
{
	switch (cbGameStatus)
	{
	case GS_FREE:	//空闲状态
		{
			//开始准备
			int iTimeRand = rand()%5+2;
			SetGameTimer(IDI_GT_GAME_USER_READY, iTimeRand);

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
	case SUB_S_GAME_START:	//游戏开始
		{
			bRet = OnSubGameStart(pBuffer,wDataSize);
			break;
		}
	case SUB_S_GIVE_UP:		//用户放弃
		{
			bRet = OnSubGiveUp(pBuffer, wDataSize);
			break;
		}
	case SUB_S_ADD_SCORE:	//用户下注
		{
			bRet = OnSubAddScore(pBuffer,wDataSize);
			break;
		}
	case SUB_S_LOOK_CARD:	//看牌消息
		{
			bRet = OnSubLookCard(pBuffer,wDataSize);
			break;
		}
	case SUB_S_COMPARE_CARD:	//比牌消息
		{
			bRet = OnSubCompareCard(pBuffer, wDataSize);
			break;
		}
	case SUB_S_OPEN_CARD:	//开牌消息
		{
			bRet = OnSubOpenCard(pBuffer, wDataSize);
			break;
		}
	case SUB_S_WAIT_COMPARE:	//等待比牌
		{
			bRet = true;
			break;
		}
	case SUB_S_GAME_END:		//游戏结束
		{
			bRet = OnSubGameEnd(pBuffer,wDataSize);
			break;
		}
	default:
		{
			return true;
			break;
		}
	}
	return true;
}


//发送之前初始化
void CGameClient::InitGameData()
{
	//初始化数据
	InitGameFrame();
	//取消所有定时器
	CancelAllTimer();
}


//游戏开始
bool CGameClient::OnSubGameStart(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	if (wDataSize!=sizeof(CMD_S_GameStart))
	{
		return false;
	}

	//初始化游戏数据
	InitGameData();

	CMD_S_GameStart *pGameStart=(CMD_S_GameStart *)pBuffer;
	if (NULL != pGameStart)
	{
		//设置状态
		SetGameStatus(GS_PLAYING);

		//单元下注
		m_lCellScore=pGameStart->lCellScore;
		//单元上限
		m_lMaxCellScore=pGameStart->lMaxScore;
		//当前倍数
		m_lCurrentTimes=pGameStart->lCurrentTimes;
		//当前用户
		m_wCurrentUser=pGameStart->wCurrentUser;
		//庄家用户
		m_wBankerUser=pGameStart->wBankerUser;
		//最大分数
		m_lUserMaxScore=pGameStart->lUserMaxScore;
		//游戏玩家个数
		for (int i=0; i<GAME_PLAYER; i++)
		{
			m_cbPlayStatus[i] = false;
			m_bIsLookCard[i] = false;
			if (pGameStart->bPlayStatus[i])
			{
				++m_wPlayerNums;	//玩家个数
				m_cbPlayStatus[i] = true; //有效位
				m_bIsLookCard[i] = false; //没有看牌
				m_lTableScore[i] = m_lCellScore;//默认下底注
			}
		}

		//第一个玩家下注
		WORD wMeChairID = GetMeChairID();
		if (wMeChairID>=0 && wMeChairID<GAME_PLAYER)
		{
			if (wMeChairID == m_wCurrentUser)
			{
				int iRand = rand()%9;
				if (iRand<0 || iRand>=8)
				{
					iRand = 8;
				}
				SetGameTimer(IDI_GT_GAME_USER_FIRSTXZ, TIME_GAME_USER_FIRSTXZ_INTERVAL+iRand);
			}
		}

	}
	return true;
}

//用户放弃
bool CGameClient::OnSubGiveUp(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	if (wDataSize!=sizeof(CMD_S_GiveUp))
	{
		return false;
	}

	KillGameTimer(IDI_GT_GAME_USER_FIRSTXZ);
	KillGameTimer(IDI_GT_GAME_USER_JIAZHU);
	KillGameTimer(IDI_GT_GAME_USER_LOOKCARD);

	CMD_S_GiveUp *pGiveUp=(CMD_S_GiveUp *)pBuffer;
	if (NULL != pGiveUp)
	{
		if (pGiveUp->wGiveUpUser>=0 && pGiveUp->wGiveUpUser<GAME_PLAYER)
		{
			m_cbPlayStatus[pGiveUp->wGiveUpUser]=false;
			m_vecCancelUserList.push_back(pGiveUp->wGiveUpUser);
			BYTE byMeChairID = GetMeChairID();
			if (pGiveUp->wGiveUpUser == byMeChairID)
			{
				SetGameStatus(GS_FREE);
			}

			//玩家是否有动作
			m_bIsUserAction = true;
		}
		
	}
	return true;
}

//用户加注
bool CGameClient::OnSubAddScore(const void * pBuffer, WORD wDataSize)
{
	if (wDataSize!=sizeof(CMD_S_AddScore))
	{
		return false;
	}

	KillGameTimer(IDI_GT_GAME_USER_FIRSTXZ);
	KillGameTimer(IDI_GT_GAME_USER_JIAZHU);
	KillGameTimer(IDI_GT_GAME_USER_LOOKCARD);

	CMD_S_AddScore *pAddScore=(CMD_S_AddScore *)pBuffer;
	if (NULL != pAddScore)
	{
		if (pAddScore->wAddScoreUser>=GAME_PLAYER)
		{
			return false;
		}
		//当前倍数
		m_lCurrentTimes=pAddScore->lCurrentTimes;
		//当前用户
		m_wCurrentUser=pAddScore->wCurrentUser;
		//下注金币
		m_lTableScore[pAddScore->wAddScoreUser]+=pAddScore->lAddScoreCount;
		//记录下注次数
		++m_wUserXZCount[pAddScore->wAddScoreUser];

		WORD wMeChairID = GetMeChairID();
		if (wMeChairID>=0 && wMeChairID<GAME_PLAYER)
		{
			//控件信息
			if(!pAddScore->wCompareState && m_cbPlayStatus[wMeChairID] && wMeChairID==m_wCurrentUser)
			{
				int iRand = rand()%7;
				if (iRand<0 || iRand>=7)
				{
					iRand = 7;
				}
				//可以下注，跟注，看牌，比牌，放弃。注意:庄家在第一轮没下注只能跟上家比牌。
				SetGameTimer(IDI_GT_GAME_USER_JIAZHU, TIME_GAME_USER_JIAZHU_INTERVAL+iRand);
			}
		}
	}
	return true;
}

//用户看牌
bool CGameClient::OnSubLookCard(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	if (wDataSize!=sizeof(CMD_S_LookCard))
	{
		return false;
	}

	KillGameTimer(IDI_GT_GAME_USER_FIRSTXZ);
	KillGameTimer(IDI_GT_GAME_USER_JIAZHU);
	KillGameTimer(IDI_GT_GAME_USER_LOOKCARD);

	CMD_S_LookCard * pLookCard=(CMD_S_LookCard *)pBuffer;
	if (NULL != pLookCard)
	{
		if (pLookCard->wLookCardUser>=0 && pLookCard->wLookCardUser<GAME_PLAYER)
		{
			m_bIsLookCard[pLookCard->wLookCardUser] = true;

			//玩家是否有动作
			m_bIsUserAction = true;

			BYTE byMeChairID = GetMeChairID();
			if (pLookCard->wLookCardUser == byMeChairID)
			{//拷贝自己手牌数据
				::memcpy(m_byUserCard, pLookCard->cbCardData, sizeof(m_byUserCard));

				if (m_bIsLookCardLaterNoOperation && m_cbPlayStatus[byMeChairID] && byMeChairID==m_wCurrentUser)
				{
					//可以下注，跟注，比牌，放弃。注意:庄家在第一轮没下注只能跟上家比牌。
					int iRand = rand()%5;
					if (iRand<0 || iRand>=5)
					{
						iRand = 5;
					}
					SetGameTimer(IDI_GT_GAME_USER_LOOKCARD, TIME_GAME_USER_LOOKCARD_INTERVAL+iRand);
				}
			}
			else
			{
				//不记录自己本人
				m_vecLookCardUserList.push_back(pLookCard->wLookCardUser);
			}
		}
	}
	return true;
}

//用户比牌
bool CGameClient::OnSubCompareCard(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	if (wDataSize!=sizeof(CMD_S_CompareCard))
	{
		return false;
	}

	KillGameTimer(IDI_GT_GAME_USER_FIRSTXZ);
	KillGameTimer(IDI_GT_GAME_USER_JIAZHU);
	KillGameTimer(IDI_GT_GAME_USER_LOOKCARD);

	CMD_S_CompareCard * pCompareCard=(CMD_S_CompareCard *)pBuffer;
	if (NULL != pCompareCard)
	{
		BYTE byMeChairID = GetMeChairID();
		//记录输家
		if (pCompareCard->wLostUser>=0 && pCompareCard->wLostUser<GAME_PLAYER)
		{
			m_cbPlayStatus[pCompareCard->wLostUser]=false;
			m_vecLostUserList.push_back(pCompareCard->wLostUser);
			if (pCompareCard->wLostUser == byMeChairID)
			{
				SetGameStatus(GS_FREE);
			}
		}
		//记录赢家,可能会记录多次。
		if (pCompareCard->wCompareUser[0]!=pCompareCard->wLostUser)
		{
			m_vecWinUserList.push_back(pCompareCard->wCompareUser[0]);
		}
		else if (pCompareCard->wCompareUser[1]!=pCompareCard->wLostUser)
		{
			m_vecWinUserList.push_back(pCompareCard->wCompareUser[1]);
		}

		//玩家是否有动作
		m_bIsUserAction = true;

		//当前用户
		m_wCurrentUser=pCompareCard->wCurrentUser;
		if (m_cbPlayStatus[byMeChairID] && byMeChairID==m_wCurrentUser)
		{
			int iRand = rand()%7;
			if (iRand<0 || iRand>=7)
			{
				iRand = 7;
			}
			//可以下注，跟注，看牌，比牌，放弃。注意:庄家在第一轮没下注只能跟上家比牌。
			SetGameTimer(IDI_GT_GAME_USER_JIAZHU, TIME_GAME_USER_JIAZHU_INTERVAL+iRand);
		}
	}
	return true;
}

//用户开牌
bool CGameClient::OnSubOpenCard(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	if (wDataSize!=sizeof(CMD_S_OpenCard)) 
	{
		return false;
	}

	//取消所有定时器
	CancelAllTimer();

	CMD_S_OpenCard* pOpenCard=(CMD_S_OpenCard *)pBuffer;
	if (NULL != pOpenCard)
	{
		//胜利用户
		m_wWinnerUser=pOpenCard->wWinner;
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
		SetGameStatus(GS_FREE);

		//重置所有数据
		InitGameData();
		
		//开始准备
		int iRand = rand()%10+5;
		if (iRand<0 || iRand>=15)
		{
			iRand = 15;
		}
		SetGameTimer(IDI_GT_GAME_USER_READY, iRand);
	}
	return true;
}


//取消所有定时器
void CGameClient::CancelAllTimer()
{
	KillGameTimer(IDI_GT_GAME_USER_READY);
	KillGameTimer(IDI_GT_GAME_USER_FIRSTXZ);
	KillGameTimer(IDI_GT_GAME_USER_JIAZHU);
	KillGameTimer(IDI_GT_GAME_USER_LOOKCARD);
}


void CGameClient::OnTimer(UINT nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CGameClientFrame::OnTimer(nIDEvent);
}



//第一次下注
void CGameClient::FirstXZ(bool bIsFirst)
{
	WORD wMeChairID = GetMeChairID();
	if (wMeChairID>=0 && wMeChairID<GAME_PLAYER)
	{
		if (wMeChairID == m_wCurrentUser)
		{
			//能不能看牌
			bool bIsLook = false;
			if (!m_bIsLookCard[wMeChairID])
			{
				BYTE byValue = rand()%10;
				if (3==byValue || 7==byValue)
				{
					//可以看牌
					//发送消息
					SendData(SUB_C_LOOK_CARD,NULL,0);
					bIsLook = true;
					m_bIsLookCard[wMeChairID] = true;
				}
			}

			if (bIsLook)
			{//看牌以后不操作
				m_bIsLookCardLaterNoOperation = false;
			}
			if (bIsFirst)
			{//第一个下注
				//发送消息
				CMD_C_AddScore AddScore;
				AddScore.wState=0;
				AddScore.lScore=bIsLook?(2*m_lCellScore):(m_lCellScore);
				SendData(SUB_C_ADD_SCORE,&AddScore,sizeof(CMD_C_AddScore));
			}
			else
			{//跟注
				FollowCard();
			}
		}
	}
}

//玩家操作
void CGameClient::MeOperator()
{
	////可以下注，跟注，看牌，比牌，放弃。注意:庄家在第一轮没下注只能跟上家比牌。
	WORD wMeChairID = GetMeChairID();
	if (wMeChairID>=GAME_PLAYER || !m_cbPlayStatus[wMeChairID] || wMeChairID!=m_wCurrentUser)
	{
		return;
	}

	int iCountNums = 0;
	for (int i=0; i<GAME_PLAYER; i++)
	{
		if (m_cbPlayStatus[i])
		{
			++iCountNums;
		}
	}
	if (iCountNums<= 1)
	{
		return;
	}
	//置连续跟牌次数为0.
	m_wContinuouFollowNums = 0;
	//看牌以后不操作
	m_bIsLookCardLaterNoOperation = false;
	//玩家是否有动作
	m_bIsUserAction = false;
	//跟到开牌
	m_bIsFollowToOpenCard = false;

	//判断可不可以看牌
	int iLookCount = m_vecLookCardUserList.size();
	int iLostCount = m_vecLostUserList.size();
	int iWinCount = m_vecWinUserList.size();
	int iCancelCount = m_vecCancelUserList.size();
	bool bIsSendLookMsg = false;
	if (!m_bIsLookCard[wMeChairID])
	{//本人没有看牌，则要判断一下要不要看牌。
		if (iLookCount>0)
		{
			if (m_wPlayerNums<=3)
			{
				bIsSendLookMsg = true;
			}
			else if (iLookCount>=2)
			{
				bIsSendLookMsg = true;
			}
			else
			{
				if (m_vecLookCardUserList[0]>=0 && m_vecLookCardUserList[0]<GAME_PLAYER)
				{
					if (m_cbPlayStatus[m_vecLookCardUserList[0]])
					{
						if (m_wUserXZCount[m_vecLookCardUserList[0]]>=5)
						{//看牌的玩家连接下注五次以上，可以看牌了。
							bIsSendLookMsg = true;
						}
						else if (iWinCount>0)
						{
							int iCountWins = 0;
							for (int i=0; i<iWinCount; i++)
							{
								if (m_vecWinUserList[i] == m_vecLookCardUserList[0])
								{
									++iCountWins;
								}
							}
							if (iCountWins>=1)
							{
								bIsSendLookMsg = true;
							}
						}
					}
				}
			}
		}
		else
		{
			if (iLostCount==0 && iCancelCount==0)
			{//没人放弃也没人输.也没人看,直接跟。
				if (m_wUserXZCount[wMeChairID]<=3)
				{//还没下注,随机看牌,跟注
					FirstXZ(false);
					return;
				}
				else if (m_wUserXZCount[wMeChairID]<10)
				{//随机一定值来跟注.
					m_wContinuouFollowNums = 2+rand()%2;
				}
				else if (m_wUserXZCount[wMeChairID]>=10)
				{
					bIsSendLookMsg = true;
				}
			}
			if (!bIsSendLookMsg && iLostCount>0)
			{
				if (m_wPlayerNums<=3)
				{
					bIsSendLookMsg = true;
				}
				else if (iLostCount>=2)
				{
					bIsSendLookMsg = true;
				}
			}
			if (!bIsSendLookMsg && iCancelCount>0)
			{
				if (m_wPlayerNums<=3)
				{
					bIsSendLookMsg = true;
				}
				else if (iCancelCount>=2)
				{
					bIsSendLookMsg = true;
				}
			}
		}

		if (bIsSendLookMsg)
		{//合适看牌
			m_bIsLookCardLaterNoOperation = true;
			//发送消息
			SendData(SUB_C_LOOK_CARD,NULL,0);
		}
		else
		{//随机判断
			int iRandValue = rand()%10;
			if (m_wContinuouFollowNums || iRandValue<=6)
			{//跟注
				FollowCard();
			}
			else
			{//比牌
				UserCompareCard();
			}
		}
	}
	else
	{//看牌操作
		LookCardLaterOpera();
	}
}


//是否可以开牌
bool CGameClient::IsOpenCard()
{
	WORD wMeChairID = GetMeChairID();
	if (wMeChairID>=GAME_PLAYER || wMeChairID!=m_wCurrentUser)
	{
		return false;
	}

	int iCountNums = 0;
	for (int i=0; i<GAME_PLAYER; i++)
	{
		if (m_cbPlayStatus[i])
		{
			++iCountNums;
		}
	}
	if (iCountNums<= 1)
	{
		return false;
	}

	//判断是否可以开牌
	LONGLONG lTemp=(m_bIsLookCard[wMeChairID])?2:1;
	if((m_lUserMaxScore-m_lTableScore[wMeChairID]) < (m_lMaxCellScore*lTemp))
	{//可以开牌

		if (m_lCurrentTimes<=0)
		{
			m_lCurrentTimes = 1;
		}
		LONGLONG lCurrentScore=(m_bIsLookCard[wMeChairID])?(m_lCurrentTimes*m_lCellScore*4):(m_lCurrentTimes*m_lCellScore*2);
		CMD_C_AddScore AddScore;
		AddScore.wState=(2==iCountNums)?1:2;
		AddScore.lScore = lCurrentScore;
		SendData(SUB_C_ADD_SCORE,&AddScore,sizeof(AddScore));

		//如果是二个玩家，则直接比牌。
		if (2 == iCountNums)
		{
			CMD_C_CompareCard CompareCard;
			CompareCard.wCompareUser = INVALID_CHAIR;
			//查找上家
			BYTE iCompareUserID = (GAME_PLAYER+wMeChairID-1)%GAME_PLAYER;
			for (int i=0; i<GAME_PLAYER; i++)
			{
				BYTE byTemplateUserID = (GAME_PLAYER+iCompareUserID-i)%GAME_PLAYER;
				if (m_cbPlayStatus[byTemplateUserID])
				{
					CompareCard.wCompareUser=byTemplateUserID;
					break;
				}
			}
			//发送消息
			SendData(SUB_C_COMPARE_CARD,&CompareCard,sizeof(CMD_C_CompareCard));
		}
		else
		{//如果是有二个以上玩家，则直接开牌。
			//发送消息
			SendData(SUB_C_OPEN_CARD,NULL,0);
		}
		return true;
	}

	return false;
}


//比牌操作
void CGameClient::UserCompareCard()
{
	WORD wMeChairID = GetMeChairID();
	if (wMeChairID>=GAME_PLAYER || wMeChairID!=m_wCurrentUser)
	{
		return;
	}

	int iCountNums = 0;
	std::vector<BYTE> vecCompareUser;
	vecCompareUser.clear();
	for (int i=0; i<GAME_PLAYER; i++)
	{
		if (m_cbPlayStatus[i])
		{
			if (i!=wMeChairID)
			{
				vecCompareUser.push_back(i);
			}
			++iCountNums;
		}
	}
	if (iCountNums<= 1 || vecCompareUser.size()<=0)
	{
		return;
	}

	if (m_lCurrentTimes<=0)
	{
		m_lCurrentTimes = 1;
	}

	LONGLONG lCurrentScore=(m_bIsLookCard[wMeChairID])?(m_lCurrentTimes*m_lCellScore*4):(m_lCurrentTimes*m_lCellScore*2);
	CMD_C_AddScore AddScore;
	AddScore.wState=1;
	AddScore.lScore = lCurrentScore;
	SendData(SUB_C_ADD_SCORE,&AddScore,sizeof(AddScore));


	//注意:庄家在第一轮没下注只能跟上家比牌。
	CMD_C_CompareCard CompareCard;
	BYTE iCompareUserID = INVALID_CHAIR;
	if ((wMeChairID==m_wBankerUser && 0==m_wUserXZCount[wMeChairID]) || (2==iCountNums))
	{
		CompareCard.wCompareUser = INVALID_CHAIR;
		//查找上家
		BYTE iCompareUserID = (GAME_PLAYER+wMeChairID-1)%GAME_PLAYER;
		for (int i=0; i<GAME_PLAYER; i++)
		{
			BYTE byTemplateUserID = (GAME_PLAYER+iCompareUserID-i)%GAME_PLAYER;
			if (m_cbPlayStatus[byTemplateUserID])
			{
				CompareCard.wCompareUser=byTemplateUserID;
				break;
			}
		}
		if (CompareCard.wCompareUser == INVALID_CHAIR)
		{
			CompareCard.wCompareUser = vecCompareUser[0];
		}
	}
	else
	{//随机一个玩家进行比牌
		int iRandValue = rand()%vecCompareUser.size();
		if (iRandValue>=0 && iRandValue<vecCompareUser.size())
		{
			CompareCard.wCompareUser = vecCompareUser[iRandValue];
		}
		else
		{
			CompareCard.wCompareUser = vecCompareUser[0];
		}
	}
	//发送消息
	SendData(SUB_C_COMPARE_CARD,&CompareCard,sizeof(CMD_C_CompareCard));
}

//获取手牌内最大点数
int CGameClient::GetCardMaxPoint()
{
	//注意A:14
	int iPoint = 0;
	for (int i=0; i<MAX_COUNT; i++)
	{
		if (0 != m_byUserCard[i])
		{
			int iTempPoint = m_byUserCard[i]&LOGIC_MASK_VALUE;
			if (1 == iTempPoint)
			{
				iTempPoint = 14;
			}
			if (iTempPoint>iPoint)
			{
				iPoint = iTempPoint;
			}
		}
		
	}
	return iPoint;
}

//看牌后的操作
void CGameClient::LookCardLaterOpera()
{
	WORD wMeChairID = GetMeChairID();
	if (wMeChairID>=GAME_PLAYER || wMeChairID!=m_wCurrentUser)
	{
		return;
	}

	//连接跟注次数
	m_wContinuouFollowNums = 0;
	//跟到开牌
	m_bIsFollowToOpenCard = false;
	//玩家是否有动作
	m_bIsUserAction = false;

	//获得手牌牌型
	bool bIsCancel = false;
	BYTE byCardType = m_GameLogic.GetCardType(m_byUserCard, MAX_COUNT);
	int iCardMaxPoint = GetCardMaxPoint();
	if (0 == iCardMaxPoint)
	{//放弃
		bIsCancel = true;
	}
	else if (CT_SINGLE==byCardType && iCardMaxPoint<10)
	{//牌是单张并且最大点数又小于10点，则直接放弃。
		bIsCancel = true;
	}
	if (!bIsCancel)
	{//跟一定次数
		if (CT_SINGLE==byCardType)
		{
			m_wContinuouFollowNums = 1+rand()%2;
		}
		else if (CT_DOUBLE==byCardType)
		{
			if (iCardMaxPoint<=10)
			{
				m_wContinuouFollowNums = 2+rand()%3;
			}
			else
			{
				m_wContinuouFollowNums = 4+rand()%4;
			}
		}
		else if ((CT_SHUN_ZI<=byCardType) && (iCardMaxPoint<=10))
		{
			m_wContinuouFollowNums = 5+rand()%6;
		}
		else if (byCardType>=CT_SHUN_ZI)
		{//跟到开牌
			m_bIsFollowToOpenCard = true;
		}

		//跟注
		FollowCard();
	}
	else
	{//放弃
		//发送消息
		SendData(SUB_C_GIVE_UP,NULL,0);
	}
}

//跟注
void CGameClient::FollowCard()
{
	WORD wMeChairID = GetMeChairID();
	if (wMeChairID>=GAME_PLAYER || wMeChairID!=m_wCurrentUser)
	{
		return;
	}
	if (m_lCurrentTimes<=0)
	{
		m_lCurrentTimes = 1;
	}
	CMD_C_AddScore AddScore;
	AddScore.wState=0;
	AddScore.lScore=m_lCellScore*m_lCurrentTimes;
	if (m_bIsLookCard[wMeChairID])
	{
		AddScore.lScore*=2;
	}
	SendData(SUB_C_ADD_SCORE,&AddScore,sizeof(CMD_C_AddScore));
}

//自动判断
void CGameClient::AutoJudge()
{
	//1:首先判断可不可以开牌
	bool bIsOpenCard = IsOpenCard();
	if (bIsOpenCard)
	{//开牌
		return;
	}

	//2:判断是否可以跟到底
	if (m_bIsFollowToOpenCard)
	{//跟到开牌
		FollowCard();
		return;
	}

	//3:判断是否还可以连接跟注
	if (!m_bIsUserAction && m_wContinuouFollowNums>0)
	{//连续跟
		--m_wContinuouFollowNums;
		if (0==m_wContinuouFollowNums)
		{//连续跟的次数已经为零了，现在可以比牌了。
			UserCompareCard();
		}
		else
		{
			FollowCard();
		}
		return;
	}

	//4:判断操作
	MeOperator();
}
