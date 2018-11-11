#include "gameclient.h"

#define _DDZ_ANDROID_DEBUG_

#ifdef _DDZ_ANDROID_DEBUG_
#include "FlashEnterDlg.h"
#include ".\gameclient.h"
#endif


//定时器
//玩家准备
#define				IDI_GT_GAME_USER_READY					3001
//玩家叫分
#define				IDI_GT_GAME_USER_JIAOFEN				3002
//玩家加倍
#define				IDI_GT_GAME_USER_JIABEI					3003
//玩家出牌
#define				IDI_GT_GAME_USER_OUTCARD				3004
//玩家不出定时器
#define				IDI_GT_GAME_USER_NO_OUTCARD				3010


//时间间隔
#define				TIME_GAME_USER_READY_INTERVAL			5
#define				TIME_GAME_USER_JIAOFEN_INTERVAL			5
#define				TIME_GAME_USER_JIABEI_INTERVAL			3
#define				TIME_GAME_USER_OUTCARD_INTERVAL			3
#define				TIME_GAME_USER_NO_OUTCARD_INTERVAL		12000



BEGIN_MESSAGE_MAP(CGameClient, CGameClientFrame)
	ON_WM_TIMER()
END_MESSAGE_MAP()



CGameClient::CGameClient(void)
{
	m_bBankerUserID = -1;
	m_bCurrentUserID = -1;
	m_bHandCardCount = 0;
	m_bJiaBeiTag = -1;
	m_cbCallScorePhase = CSD_NORMAL;
	::memset(m_bHandCardData, 0, sizeof(m_bHandCardData));
	m_bIsNewTurn = false;
	::srand((unsigned int)::time(NULL));
}

CGameClient::~CGameClient(void)
{
}


//初始函数
bool CGameClient::InitGameFrame()
{
	m_bBankerUserID = -1;
	m_bCurrentUserID = -1;
	m_bHandCardCount = 0;
	::memset(m_bHandCardData, 0, sizeof(m_bHandCardData));
	m_bJiaBeiTag = -1;
	m_cbCallScorePhase = CSD_NORMAL;
	m_bMeCanOutCard = false;
	m_bIsNewTurn = false;
	return true;
}

//重置框架
void CGameClient::ResetGameFrame()
{
	m_bBankerUserID = -1;
	m_bCurrentUserID = -1;
	m_bHandCardCount = 0;
	::memset(m_bHandCardData, 0, sizeof(m_bHandCardData));
	m_bJiaBeiTag = -1;
	m_cbCallScorePhase = CSD_NORMAL;
	m_bMeCanOutCard = false;
	m_bIsNewTurn = false;
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
	case IDI_GT_GAME_USER_JIAOFEN:
		{//叫分定时器
			if (0==nElapse|| nElapse>100)
			{
				CMD_C_LandScore LandScore;
				if (m_GameLogic.GetLandTag())
				{
					LandScore.bLandScore=3;	//直接叫3分
				}
				else
				{
					LandScore.bLandScore = 255;	//不叫
				}
				SendData(SUB_C_LAND_SCORE,&LandScore,sizeof(LandScore));
			}
			break;
		}
	case IDI_GT_GAME_USER_OUTCARD:
		{//出牌定时器
			if (0==nElapse|| nElapse>100)
			{
				GameUserAutoOutCard();
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
	case GS_WK_FREE:	//空闲状态
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
	case SUB_S_SEND_CARD:	//游戏开始发牌
		{
			bRet = OnSubSendCard(pBuffer,wDataSize);
			break;
		}
	case SUB_S_LAND_SCORE:	//叫分过程
		{
			bRet = OnSubGameUserLandScore(pBuffer,wDataSize);
			break;
		}
	case SUB_S_DOUBLE_SCORE:	//叫分完成
		{
			bRet = OnSubJiaoFenFin(pBuffer,wDataSize);
			break;
		}
	case SUB_S_USER_DOUBLE:		//玩家加倍
		{
			bRet = OnSubUserDoubleResult(pBuffer, wDataSize);
			break;
		}
	case SUB_S_GAME_START:	//游戏开始出牌
		{
			bRet = OnSubGameStartOutCard(pBuffer, wDataSize);
			break;
		}
	case SUB_S_OUT_CARD:		//用户出牌结果
		{
			bRet = OnSubGameUserOutCardResult(pBuffer, wDataSize);
			break;
		}
	case SUB_S_PASS_CARD:		//放弃出牌
		{
			bRet = OnSubGameUserPassCard(pBuffer, wDataSize);
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
#ifdef _DDZ_ANDROID_DEBUG_
	if (!bRet)
	{
		CWnd* pMain = AfxGetMainWnd();
		if (NULL != pMain)
		{
			char chData[32]={0};
			::sprintf(chData, "消息ID:%d 不正确或者未处理", wSubCmdID);
			((CFlashEnterDlg*)pMain)->InsertListData(0, 0, chData);
		}
	}
#endif
	return true;
}


//发送之前初始化
void CGameClient::InitSendCard()
{
	m_bBankerUserID = -1;
	m_bCurrentUserID = -1;
	m_bHandCardCount = 0;
	::memset(m_bHandCardData, 0, sizeof(m_bHandCardData));
	m_bJiaBeiTag = -1;
	m_bMeCanOutCard = false;
	m_cbCallScorePhase = CSD_NORMAL;
	m_bIsNewTurn = false;

	//取消所有定时器
	CancelAllTimer();
	
	BYTE btMeChairID = GetMeChairID();
	if (btMeChairID>=0 && btMeChairID<GAME_PLAYER)
	{
		m_GameLogic.InitLogicData(btMeChairID);
	}
}


//发送扑克
bool CGameClient::OnSubSendCard(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_SendAllCard));
	if (wDataSize!=sizeof(CMD_S_SendAllCard))
	{
		return false;
	}

	//初始化数据
	InitSendCard();

	//变量定义
	CMD_S_SendAllCard * pSendCard=(CMD_S_SendAllCard *)pBuffer;
	if (NULL != pSendCard)
	{
		WORD byMeChairID = GetMeChairID();
		for (BYTE i=0; i<GAME_PLAYER; i++)
		{
			if (i == byMeChairID)
			{
				m_bHandCardCount = 17;
				::memcpy(m_bHandCardData, pSendCard->bCardData[i], sizeof(BYTE)*m_bHandCardCount);
				break;
			}
		}
		if (m_bHandCardCount < 17)
		{
			return false;
		}

#ifdef _DDZ_ANDROID_DEBUG_
		//打印出牌信息
		CWnd* pMain = AfxGetMainWnd();
		if (NULL != pMain)
		{
			std::string strValue = "发牌数据为: ";
			char chData[100] = {0};
			for (int i=0; i<m_bHandCardCount; i++)
			{
				::memset(chData, 0, sizeof(chData));
				::sprintf(chData, "%x,", m_bHandCardData[i]);
				strValue.append(chData);
			}
			::sprintf(chData, "%s", strValue.c_str());
			((CFlashEnterDlg*)pMain)->InsertListData(0, 0, chData);
		}
#endif
		m_GameLogic.SetHandCard(m_bHandCardData, m_bHandCardCount, false);

		//保存当前玩家
		m_bCurrentUserID = pSendCard->wCurrentUser;

		if (byMeChairID == m_bCurrentUserID)
		{//如果我是当前玩家，则直接叫分。
			int iTimeRand = rand()%5+1;
			SetGameTimer(IDI_GT_GAME_USER_JIAOFEN, iTimeRand);
		}
	}

	return true;
}

//叫分过程
bool CGameClient::OnSubGameUserLandScore(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_LandScore));
	if (wDataSize!=sizeof(CMD_S_LandScore))
	{
		return false;
	}

	//消息处理
	CMD_S_LandScore * pLandScore=(CMD_S_LandScore *)pBuffer;
	if (NULL != pLandScore)
	{
		KillGameTimer(IDI_GT_GAME_USER_JIAOFEN);

		//设置变量
		m_cbCallScorePhase = pLandScore->bCallScorePhase;
		
		WORD wMeChairID = GetMeChairID();
		if (pLandScore->wCurrentUser==wMeChairID)
		{
			int iTimeRand = rand()%4+1;
			SetGameTimer(IDI_GT_GAME_USER_JIAOFEN, iTimeRand);
		}
	}
	return true;
}


//玩家加倍结果
bool CGameClient::OnSubUserDoubleResult(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_UserDouble));
	if (wDataSize != sizeof(CMD_S_UserDouble)) 
	{
		return false;
	}

	KillTimer(IDI_GT_GAME_USER_JIABEI);
	//变量定义
	CMD_S_UserDouble *pUserDouble = ( CMD_S_UserDouble * )pBuffer;
	if (NULL != pUserDouble)
	{
		WORD byMeChairID = GetMeChairID();
		if (byMeChairID == pUserDouble->bNextUser)
		{//加倍选择
			bool bIsAddBei = false;
			if (byMeChairID == m_bBankerUserID)
			{
				if (m_GameLogic.GetLandTag())
				{
					bIsAddBei = true;
				}
			}
			else
			{
				int iTag = rand()%2;
				if (iTag)
				{
					bIsAddBei = true;
				}
			}
			if (bIsAddBei)
			{
				m_bJiaBeiTag = pUserDouble->bState;
				int iTimeRand = rand()%3+1;
				SetTimer(IDI_GT_GAME_USER_JIABEI, iTimeRand*1000, NULL);
			}
		}
	}
	return true;
}

//叫分完成
bool CGameClient::OnSubJiaoFenFin(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_DoubleScore));
	if (wDataSize != sizeof(CMD_S_DoubleScore))
	{
		return false;
	}

	KillGameTimer(IDI_GT_GAME_USER_JIAOFEN);
	m_bBankerUserID = -1;	//地主
	m_bCurrentUserID = -1;	//当前玩家
	m_bMeCanOutCard = false;

	//变量定义
	CMD_S_DoubleScore *pDoubleScore = ( CMD_S_DoubleScore * )pBuffer;
	if (NULL != pDoubleScore)
	{
		m_bBankerUserID = pDoubleScore->wLandUser;
		m_bCurrentUserID = pDoubleScore->wLandUser;

		WORD byMeChairID = GetMeChairID();
		if (byMeChairID == m_bBankerUserID)
		{//我是地主
			//增加三张底牌
			CopyMemory(&m_bHandCardData[17],pDoubleScore->bBackCard,sizeof(BYTE)*3);
			m_bHandCardCount+=3;
			m_GameLogic.SetHandCard(m_bHandCardData, m_bHandCardCount, true);
		}
		else if (byMeChairID == (pDoubleScore->wLandUser+1)%GAME_PLAYER)
		{//如果我是庄家的下一家，则我可以倒。
			int iTag = rand()%2;
			if (iTag)
			{
				m_bJiaBeiTag = 1;
				int iTimeRand = rand()%3+1;
				SetTimer(IDI_GT_GAME_USER_JIABEI, iTimeRand*1000, NULL);
			}
		}
	}

	return true;
}

//游戏开始出牌
bool CGameClient::OnSubGameStartOutCard(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_GameStart));
	if (wDataSize!=sizeof(CMD_S_GameStart))
	{
		return false;
	}

	KillTimer(IDI_GT_GAME_USER_JIABEI);
	KillGameTimer(IDI_GT_GAME_USER_JIAOFEN);
	KillTimer(IDI_GT_GAME_USER_NO_OUTCARD);

	//消息处理
	CMD_S_GameStart * pGameStart=(CMD_S_GameStart *)pBuffer;
	if (NULL != pGameStart)
	{
		m_bIsNewTurn = true;
		m_bMeCanOutCard = false;
		m_bBankerUserID = pGameStart->wLandUser;
		m_bCurrentUserID = pGameStart->wLandUser;
		
#ifdef _DDZ_ANDROID_DEBUG_
		CWnd* pMain = AfxGetMainWnd();
		if (NULL != pMain)
		{
			char chData[100] = {0};
			::sprintf(chData, "游戏开始出牌; 地主:%d", m_bBankerUserID);
			((CFlashEnterDlg*)pMain)->InsertListData(0, 0, chData);
		}
#endif
		m_GameLogic.SetLandTag(m_bCurrentUserID);
		m_GameLogic.SetWhoOutCard(m_bCurrentUserID, true);
		WORD byMeChairID = GetMeChairID();
		if (byMeChairID == m_bCurrentUserID)
		{//如果我是当前玩家，则出牌。
			m_bMeCanOutCard = true;

			int iTimeRand = rand()%6 + 1;
			SetGameTimer(IDI_GT_GAME_USER_OUTCARD, iTimeRand);

			SetTimer(IDI_GT_GAME_USER_NO_OUTCARD, TIME_GAME_USER_NO_OUTCARD_INTERVAL, NULL);

#ifdef _DDZ_ANDROID_DEBUG_
			CWnd* pMain = AfxGetMainWnd();
			if (NULL != pMain)
			{
				((CFlashEnterDlg*)pMain)->InsertListData(0, 0, TEXT("0游戏开始出牌状态--该本人出牌了！"));
			}
#endif
			
		}
	}

	return true;
}

//自动出最后一张牌
void CGameClient::GameAutoLastOneCard()
{
	CMD_C_OutCard OutCard;
	::memset(&OutCard, 0, sizeof(CMD_C_OutCard));
	OutCard.bCardCount = 1;
	OutCard.bCardData[0] = m_bHandCardData[m_bHandCardCount];//出最后一张牌
	//发送出牌信息
	SendData(SUB_C_OUT_CART,&OutCard,sizeof(CMD_C_OutCard));
	m_bMeCanOutCard = false;

#ifdef _DDZ_ANDROID_DEBUG_
	//打印出牌信息
	CWnd* pMain = AfxGetMainWnd();
	if (NULL != pMain)
	{
		std::string strValue = "2本地发送出牌数据为: ";
		char chData[100] = {0};
		for (int i=0; i<OutCard.bCardCount; i++)
		{
			::memset(chData, 0, sizeof(chData));
			::sprintf(chData, "%x,", OutCard.bCardData[i]);
			strValue.append(chData);
		}
		::sprintf(chData, "%s", strValue.c_str());
		((CFlashEnterDlg*)pMain)->InsertListData(0, 0, chData);
	}
#endif
}

//自动出牌
void CGameClient::GameUserAutoOutCard()
{
	if (m_bCurrentUserID != GetMeChairID() || !m_bMeCanOutCard)
	{//不是当前玩家
		return;
	}
	CMD_C_OutCard OutCard;
	::memset(&OutCard, 0, sizeof(CMD_C_OutCard));
	m_GameLogic.OutCard(OutCard.bCardData, OutCard.bCardCount);
	if (OutCard.bCardCount > 0 )
	{
		//发送出牌信息
		SendData(SUB_C_OUT_CART,&OutCard,sizeof(CMD_C_OutCard));
		m_bMeCanOutCard = false;

#ifdef _DDZ_ANDROID_DEBUG_
		//打印出牌信息
		CWnd* pMain = AfxGetMainWnd();
		if (NULL != pMain)
		{
			std::string strValue = "1本地发送出牌数据为: ";
			char chData[100] = {0};
			for (int i=0; i<OutCard.bCardCount; i++)
			{
				::memset(chData, 0, sizeof(chData));
				::sprintf(chData, "%x,", OutCard.bCardData[i]);
				strValue.append(chData);
			}
			::sprintf(chData, "%s", strValue.c_str());
			((CFlashEnterDlg*)pMain)->InsertListData(0, 0, chData);
		}
#endif
		
	}
	else
	{
		KillTimer(IDI_GT_GAME_USER_NO_OUTCARD);
		m_bMeCanOutCard = false;
		if (m_bIsNewTurn)
		{//如果是庄家或赢家，则不能不出。
			GameAutoLastOneCard();
		}
		else
		{
			//放弃出牌
			SendData(SUB_C_PASS_CARD);
#ifdef _DDZ_ANDROID_DEBUG_
			CWnd* pMain = AfxGetMainWnd();
			if (NULL != pMain)
			{
				((CFlashEnterDlg*)pMain)->InsertListData(0, 0, TEXT("玩家已发送不出消息!"));
			}
#endif
		}
	}
}


//玩家出牌结果
bool CGameClient::OnSubGameUserOutCardResult(const void * pBuffer, WORD wDataSize)
{
	ASSERT(wDataSize==sizeof(CMD_S_OutCard));
	if (wDataSize!=sizeof(CMD_S_OutCard))
	{
		return false;
	}
	//变量定义
	CMD_S_OutCard * pOutCard=(CMD_S_OutCard *)pBuffer;
	if (NULL != pOutCard)
	{
		KillGameTimer(IDI_GT_GAME_USER_OUTCARD);
		KillTimer(IDI_GT_GAME_USER_JIABEI);
		KillTimer(IDI_GT_GAME_USER_NO_OUTCARD);
		m_bMeCanOutCard = false;
		m_bIsNewTurn = false;

		WORD byMeChairID = GetMeChairID();
		if (byMeChairID == pOutCard->wOutCardUser)
		{//删除手中的牌
			bool bIsSuc = m_GameLogic.RemoveCard(pOutCard->bCardData,pOutCard->bCardCount,m_bHandCardData,m_bHandCardCount);
			if (bIsSuc)
			{//删除成功.
				m_bHandCardCount-=pOutCard->bCardCount;
			}

#ifdef _DDZ_ANDROID_DEBUG_
			//打印出牌信息
			CWnd* pMain = AfxGetMainWnd();
			if (NULL != pMain)
			{
				std::string strValue = "网络出牌数据信息: ";
				char chData[100] = {0};
				::memset(chData, 0, sizeof(chData));
				::sprintf(chData, "出牌玩家%d, 当前玩家:%d, ", pOutCard->wOutCardUser, pOutCard->wCurrentUser);
				strValue.append(chData);

				for (int i=0; i<pOutCard->bCardCount; i++)
				{
					::memset(chData, 0, sizeof(chData));
					::sprintf(chData, "%x,", pOutCard->bCardData[i]);
					strValue.append(chData);
				}
				::memset(chData, 0, sizeof(chData));
				::sprintf(chData, "%s", strValue.c_str());
				((CFlashEnterDlg*)pMain)->InsertListData(0, 0, chData);
			}
#endif

		}
		m_GameLogic.OutCardResult(pOutCard->wOutCardUser, true, pOutCard->bCardData,pOutCard->bCardCount);

		//设置倒跟反
		SetGameUserJiaBei(pOutCard->wBankerUser, pOutCard->wCurrentUser, pOutCard->bCircuit, pOutCard->bIsDouble);

		//记录当前玩家
		m_bCurrentUserID = pOutCard->wCurrentUser;

		//当前玩家就是要出牌的玩家
		if (pOutCard->wCurrentUser==pOutCard->wOutCardUser)
		{
			m_bIsNewTurn = true;
			m_GameLogic.SetWhoOutCard(pOutCard->wCurrentUser, true);
		}
		else
		{
			m_GameLogic.SetWhoOutCard(pOutCard->wCurrentUser, false);
		}
		
		if (byMeChairID == pOutCard->wCurrentUser)
		{//如果我是当前玩家，则出牌。
			int iTimeRand = rand()%8 + 1;
			SetGameTimer(IDI_GT_GAME_USER_OUTCARD, iTimeRand);

			SetTimer(IDI_GT_GAME_USER_NO_OUTCARD, TIME_GAME_USER_NO_OUTCARD_INTERVAL, NULL);

			m_bMeCanOutCard = true;
#ifdef _DDZ_ANDROID_DEBUG_
			CWnd* pMain = AfxGetMainWnd();
			if (NULL != pMain)
			{
				((CFlashEnterDlg*)pMain)->InsertListData(0, 0, TEXT("1出牌状态--该本人出牌了！"));
			}
#endif
		}
	}
	return true;
}


//玩家放弃出牌
bool CGameClient::OnSubGameUserPassCard(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_PassCard));
	if (wDataSize!=sizeof(CMD_S_PassCard))
	{
		return false;
	}
	
	CMD_S_PassCard * pPassCard=(CMD_S_PassCard *)pBuffer;
	if (NULL != pPassCard)
	{
		KillGameTimer(IDI_GT_GAME_USER_OUTCARD);
		KillTimer(IDI_GT_GAME_USER_JIABEI);
		KillTimer(IDI_GT_GAME_USER_NO_OUTCARD);
		m_bMeCanOutCard = false;
		m_bIsNewTurn = false;

#ifdef _DDZ_ANDROID_DEBUG_
		//打印出牌信息
		CWnd* pMain = AfxGetMainWnd();
		if (NULL != pMain)
		{
			std::string strValue = "网络不出牌数据信息: ";
			char chData[100] = {0};
			::memset(chData, 0, sizeof(chData));
			::sprintf(chData, "出牌玩家%d, 当前玩家:%d, ", pPassCard->wPassUser, pPassCard->wCurrentUser);
			strValue.append(chData);
			::memset(chData, 0, sizeof(chData));
			::sprintf(chData, "%s", strValue.c_str());
			((CFlashEnterDlg*)pMain)->InsertListData(0, 0, chData);
		}
#endif

		m_GameLogic.OutCardResult(pPassCard->wPassUser, false, NULL,0);

		//设置当前玩家
		m_bCurrentUserID = pPassCard->wCurrentUser;

		//设置倒跟反
		SetGameUserJiaBei(pPassCard->wBankerUser, pPassCard->wCurrentUser, pPassCard->bCircuit, pPassCard->bIsDouble);

		//一轮判断
		if (pPassCard->bNewTurn)
		{
			m_bIsNewTurn = true;
			m_GameLogic.SetWhoOutCard(pPassCard->wCurrentUser, true);
		}
		else
		{
			m_GameLogic.SetWhoOutCard(pPassCard->wCurrentUser, false);
		}

		WORD byMeChairID = GetMeChairID();
		if (byMeChairID == pPassCard->wCurrentUser)
		{//如果我是当前玩家，则出牌。
			int iTimeRand = rand()%8 + 1;
			SetGameTimer(IDI_GT_GAME_USER_OUTCARD, iTimeRand);

			SetTimer(IDI_GT_GAME_USER_NO_OUTCARD, TIME_GAME_USER_NO_OUTCARD_INTERVAL, NULL);

			m_bMeCanOutCard = true;
#ifdef _DDZ_ANDROID_DEBUG_
			CWnd* pMain = AfxGetMainWnd();
			if (NULL != pMain)
			{
				((CFlashEnterDlg*)pMain)->InsertListData(0, 0, TEXT("2不出状态--该本人出牌了！"));
			}
#endif

		}
	}

	return true;
}

//加倍判断
void CGameClient::SetGameUserJiaBei(WORD wBankerID, WORD wCurrentID, const BYTE *pCircuit, const bool *pIsDouble)
{
	if (wBankerID>=GAME_PLAYER || wCurrentID>=GAME_PLAYER || NULL==pCircuit || NULL==pIsDouble)
	{
		return;
	}

	WORD byMeChairID = GetMeChairID();
	m_bJiaBeiTag = 0;
	if (wCurrentID==byMeChairID) 
	{//下一玩家（点击出牌时的下一个玩家）
		if (byMeChairID==wBankerID)
		{//庄家
			if (1 == pCircuit[wCurrentID])
			{//出牌次数 小于2
				if ((pIsDouble[(wBankerID +1)%GAME_PLAYER] 
				|| pIsDouble[(wBankerID + 2)%GAME_PLAYER]) 
					&& !pIsDouble[wBankerID])
				{//农民有 "倒"或"跟" 并且庄家还没有"反牌",可以反。
					m_bJiaBeiTag = 3;
				}
			}
		}
		else
		{//农民
			if (pCircuit[wCurrentID] < 1)
			{//出牌次数 小于1
				if (wCurrentID == (wBankerID+1)%GAME_PLAYER)
				{//第一个农民，
					if (!pIsDouble[(wBankerID+1)%GAME_PLAYER])
					{//之前没有倒，可以倒。
						m_bJiaBeiTag = 1;
					}
				}
				else if ((wBankerID+2)%GAME_PLAYER == wCurrentID)
				{//第二个农民.
					if (pIsDouble[(wCurrentID+2)%GAME_PLAYER] 
					&& !pIsDouble[(wCurrentID)%GAME_PLAYER])
					{//加倍了,可以跟.
						m_bJiaBeiTag = 2;
					}
					else if(!pIsDouble[(wCurrentID)%GAME_PLAYER])
					{//没加倍,可以倒.
						m_bJiaBeiTag = 1;
					}
				}
			}
		}
	}
	if (m_bJiaBeiTag>0)
	{//加倍
		WORD byMeChairID = GetMeChairID();
		bool bIsAddBei = false;
		if (byMeChairID == m_bBankerUserID)
		{
			if (m_GameLogic.GetLandTag())
			{
				bIsAddBei = true;
			}
		}
		else
		{
			int iTag = rand()%2;
			if (iTag)
			{
				bIsAddBei = true;
			}
		}
		if (bIsAddBei)
		{
			KillTimer(IDI_GT_GAME_USER_JIABEI);
			int iTimeRand = rand()%3+1;
			SetTimer(IDI_GT_GAME_USER_JIABEI, iTimeRand*1000, NULL);
		}
	}
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


//取消所有定时器
void CGameClient::CancelAllTimer()
{
	KillGameTimer(IDI_GT_GAME_USER_READY);
	KillGameTimer(IDI_GT_GAME_USER_JIAOFEN);
	KillGameTimer(IDI_GT_GAME_USER_OUTCARD);
	KillTimer(IDI_GT_GAME_USER_NO_OUTCARD);
	KillTimer(IDI_GT_GAME_USER_JIABEI);
}


void CGameClient::OnTimer(UINT nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	CGameClientFrame::OnTimer(nIDEvent);

#ifdef _DDZ_ANDROID_DEBUG_
	CWnd* pMain = AfxGetMainWnd();
	if (NULL != pMain)
	{
		char chData[100]={0};
		::sprintf(chData, "触发时间事件ID:%d", nIDEvent);
		((CFlashEnterDlg*)pMain)->InsertListData(0, 0, chData);
	}
#endif

	switch (nIDEvent)
	{
	case IDI_GT_GAME_USER_NO_OUTCARD:
		{
			KillTimer(IDI_GT_GAME_USER_NO_OUTCARD);
			if ((m_bMeCanOutCard) && (m_bCurrentUserID == GetMeChairID()))
			{
				if (m_bIsNewTurn)
				{//如果是庄家或赢家，则不能不出。
					GameAutoLastOneCard();
				}
				else
				{
					//放弃出牌
					m_bMeCanOutCard = false;
					SendData(SUB_C_PASS_CARD);

#ifdef _DDZ_ANDROID_DEBUG_
					CWnd* pMain = AfxGetMainWnd();
					if (NULL != pMain)
					{
						char chData[100]={0};
						::sprintf(chData, "触发时间事件ID:%d,%d玩家超时已发送放弃消息", nIDEvent, GetMeChairID());
						((CFlashEnterDlg*)pMain)->InsertListData(0, 0, chData);
					}
#endif
				}
			}
			break;
		}
	case IDI_GT_GAME_USER_JIABEI:
		{//倒、跟、反
			KillTimer(IDI_GT_GAME_USER_JIABEI);
			if (m_bJiaBeiTag>=1 && m_bJiaBeiTag<=3)
			{
				//定义消息
				CMD_C_DoubleScore DoubleScore;
				ZeroMemory( &DoubleScore, sizeof( DoubleScore ) );
				DoubleScore.bDoubleScore = true;
				//发送消息
				SendData( SUB_C_DOUBLE_SCORE, &DoubleScore, sizeof( DoubleScore ) );

#ifdef _DDZ_ANDROID_DEBUG_
				CWnd* pMain = AfxGetMainWnd();
				if (NULL != pMain)
				{
					char chData[100]={0};
					::sprintf(chData, "触发时间事件ID:%d,开始倒跟反了", nIDEvent);
					((CFlashEnterDlg*)pMain)->InsertListData(0, 0, chData);
				}
#endif
			}
			break;
		}
	default:
		break;
	}
}
