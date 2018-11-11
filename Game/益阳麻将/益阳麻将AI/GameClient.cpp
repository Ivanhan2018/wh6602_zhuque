#include "GameClient.h"
#include "AndroidAI.h"
#include "..\..\开发库\Include\GlobalChannel.h"

//定时器
#define				IDI_GT_GAME_USER_READY					1000					//玩家准备
#define				IDI_OPERATE_CARD						1001					//操作牌时间
#define				IDI_GT_GAME_USER_OUTCARD				1002					//玩家出牌定时器
#define				IDI_USER_TING_CARD						1003					//听牌定时器
#define				IDI_USER_HAI_DI_CARD					1004					//用户海底牌
#define             IDI_SZSTART                             1005                    //启动筛子定时器


#define				TIME_GAME_READY							3						//准备时间
#define				TIME_OPERATE_CARD						2						//操作牌时间
#define				TIME_LESS								2						//最少时间
#define				TIME_GAME_USER_NO_OUTCARD_INTERVAL		5						//最大出牌时间
#define				TIME_TING_CARD_INTERVAL					2						//听牌定时器
#define				TIME_USER_HAI_DI_CARD_INTERVAL			2						//海底牌定时器
#define             TIME_USER_SZ_INTERVAL                   5                       //启动筛子时间


BEGIN_MESSAGE_MAP(CGameClient, CGameClientFrame)
END_MESSAGE_MAP()


CGameClient::CGameClient(void)
{
	m_pRoomLog = NULL;		// 日志
	//初始化
	InitData();
}

CGameClient::~CGameClient(void)
{
}

void CGameClient::InitSome(DWORD dwWndID)
{
		HRESULT hResult;
		hResult = ::CoCreateInstance(CLSID_DefLogMgr, NULL, CLSCTX_INPROC_SERVER, IID_ILogMgr, (void**)&m_pRoomLog);

		if(S_OK==hResult && m_pRoomLog)
		{
			CString strPath,strTmp; 
			GetModuleFileName(NULL, strPath.GetBufferSetLength(256), 256);
			strTmp.Format(".\\YYMJAndroidLog\\%d\\",dwWndID);
			strPath = strPath.Left(strPath.Find("AndroidLoad.exe")) + strTmp;//YYMJAndroid.dll

			m_pRoomLog->Init();
			m_pRoomLog->Start();
			m_pRoomLog->Register(strPath, 50000);
			m_pRoomLog->AddMsg(0, 0, TRUE,"***** 益阳麻将机器人日志开始服务");
		}
}

void CGameClient::UninitSome()
{
	// 日志
	if (m_pRoomLog)
	{
		if (m_pRoomLog)
			m_pRoomLog->AddMsg(0, 0, TRUE,"***** 益阳麻将机器人日志停止服务");

		m_pRoomLog->Stop();
		m_pRoomLog->Uninit();
		m_pRoomLog = NULL; 
	}
}

//初始化
void CGameClient::InitData()
{
	//游戏变量
	m_wBankerUser=INVALID_CHAIR;
	m_wCurrentUser=INVALID_CHAIR;
	m_cbActionMask = 0;
	m_cbActionCard = 0;
	m_bHearStatus = false;
	m_bIsMeCanOutCard = false;

	::memset(m_byGangCard, 0, sizeof(m_byGangCard));
	::memset(m_byBuCard, 0, sizeof(m_byBuCard));
	m_byGangCardNums = 0;
	m_byBuCardNums = 0;

	//扑克变量
	ZeroMemory(m_cbCardIndex,sizeof(m_cbCardIndex));
	m_TempKing = 0;
}

//初始函数
bool CGameClient::InitGameFrame()
{
	//初始化
	InitData();

	return true;
}

//重置框架
void CGameClient::ResetGameFrame()
{
	//初始化
	InitData();
}

//取消所有定时器
void CGameClient::CancelAllTimer()
{
	KillGameTimer(IDI_GT_GAME_USER_READY);
	KillGameTimer(IDI_OPERATE_CARD);
	KillGameTimer(IDI_GT_GAME_USER_OUTCARD);
	KillGameTimer(IDI_USER_TING_CARD);
	KillGameTimer(IDI_USER_HAI_DI_CARD);
}

//发送之前初始化
void CGameClient::InitSendCard()
{
	//游戏变量
	m_wBankerUser=INVALID_CHAIR;
	m_wCurrentUser=INVALID_CHAIR;
	m_cbActionMask = 0;
	m_cbActionCard = 0;
	m_bHearStatus = false;
	m_bIsMeCanOutCard = false;

	::memset(m_byGangCard, 0, sizeof(m_byGangCard));
	::memset(m_byBuCard, 0, sizeof(m_byBuCard));
	m_byGangCardNums = 0;
	m_byBuCardNums = 0;

	//扑克变量
	ZeroMemory(m_cbCardIndex,sizeof(m_cbCardIndex));
	m_TempKing = 0;

	CancelAllTimer();//删除定时器
}

//系统事件
//时间消息
bool CGameClient::OnTimerMessage(UINT nElapse, UINT nTimerID)
{
	switch (nTimerID)
	{
	case IDI_SZSTART:     //启动筛子定时器
		{
			if (0==nElapse)
			{
				SendData(SUB_C_SZSTART);
			}
			break;
		}
	case IDI_GT_GAME_USER_READY:
		{//游戏准备定时器
			if (0==nElapse|| nElapse>100)
			{
				SendUserReady(NULL, 0);
			}
			break;
		}
	case IDI_OPERATE_CARD:				//操作
		{
			if (0 == nElapse)
			{
				BYTE byChairID = GetMeChairID();
				if (byChairID == m_wCurrentUser)
				{//开始操作
					OnUserOperateCard();
				}
			}
			break;
		}
	case IDI_GT_GAME_USER_OUTCARD:		//出牌定时器
		{
			if (nElapse==0)
			{
				BYTE byChairID = GetMeChairID();
				if (byChairID == m_wCurrentUser)
				{//开始出牌
					OnOutCard();
				}
			}
			break;
		}
	case IDI_USER_HAI_DI_CARD:			//海底牌
		{
			if (0==nElapse)
			{
				BYTE byChairID = GetMeChairID();
				if (byChairID == m_wCurrentUser)
				{//海底牌
					//构造消息
					CMD_C_OperateHaiDi OperateHaiDi;
					OperateHaiDi.cbTakeHaiDi=1;
					SendData(SUB_C_OPERATE_HAI_DI,&OperateHaiDi,sizeof(CMD_C_OperateHaiDi));
				}
			}
			break;
		}
	case IDI_USER_TING_CARD:			//用户听牌
		{
			if (0==nElapse)
			{
				BYTE byChairID = GetMeChairID();
				if (byChairID == m_wCurrentUser)
				{//海底牌
					CMD_UserTingOperate TingOperate;
					TingOperate.bTingCard = true;
					SendData(SUB_C_USERTINGCARD,&TingOperate,sizeof(TingOperate));
				}
			}
			break;
		}
	default:
		break;
	}
	char sz[100]={0};
	sprintf(sz,"CGameClient::OnTimerMessage nElapse=%d,nTimerID=%d",nElapse,nTimerID);
	if (m_pRoomLog)
		m_pRoomLog->AddMsg(0, 0, TRUE,sz);
	return true;
}

//游戏场景
bool CGameClient::OnGameSceneMessage(BYTE cbGameStatus, const void * pBuffer, WORD wDataSize)
{
	switch (cbGameStatus)
	{
	case GS_MJ_FREE:	//空闲状态
		{
			WORD wTime = rand()%TIME_GAME_READY+TIME_LESS;
			SetGameTimer(IDI_GT_GAME_USER_READY,wTime);
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
	case SUB_C_SZSTART:			//启动色子动画
		{
			bRet = OnSubStartSZFlash();
			break;
		}
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
	case SUB_S_TING_CARD:		//听牌标记
		{
			bRet =  OnSubTingCard(pBuffer,wDataSize);
			break;
		}
	case SUB_S_USER_TINGCARD_RESULT:	//听牌的结果
		{
			bRet =  OnSubTingCardResult(pBuffer, wDataSize);
			break;
		}
	default:
		break;
	}

	return true;
}

//是否听牌操作了
bool CGameClient::bIsTingCardOpera()
{
	BYTE byChairID = GetMeChairID();
	if (m_wCurrentUser!=byChairID)
	{
		return false;
	}
	if (m_bHearStatus)
	{//听牌了
		if ((m_cbActionMask&WIK_CHI_HU)>0 && 0!=m_cbActionCard)
		{//可以胡牌了
			OnCardOperate(WIK_CHI_HU, m_cbActionCard);
		}
		else
		{
			//取消
			OnCardOperate(WIK_NULL, 0);
		}
		return true;
	}
	return false;
}

//用户操作牌
void CGameClient::OnUserOperateCard()
{
	bool bIsTingCard = bIsTingCardOpera();
	if (!bIsTingCard)
	{//没有听牌
		if ((m_cbActionMask&WIK_CHI_HU)>0 && 0!=m_cbActionCard)
		{//可以胡牌了
			OnCardOperate(WIK_CHI_HU, m_cbActionCard);
			return;
		}
		else if ((m_cbActionMask&WIK_TING)>0)
		{//开始听牌
			if (m_byGangCardNums>0 && 0!=m_byGangCard[0])
			{
				OnCardOperate(WIK_TING, m_byGangCard[0]);
				return;
			}
		}
		//取消
		OnCardOperate(WIK_NULL, 0);
	}
}

//扑克操作
void CGameClient::OnCardOperate(BYTE byOperaType, BYTE byCardData)
{
	KillGameTimer(IDI_OPERATE_CARD);
	KillGameTimer(IDI_GT_GAME_USER_OUTCARD);

	BYTE byChairID = GetMeChairID();
	if (m_wCurrentUser!=byChairID)
	{
		return;
	}

	//发送命令
	CMD_C_OperateCard OperateCard;
	OperateCard.cbOperateCode=byOperaType;
	OperateCard.cbOperateCard=byCardData;
	SendData(SUB_C_OPERATE_CARD,&OperateCard,sizeof(CMD_C_OperateCard));

	if (0==byOperaType)
	{//如果取消了
		if (m_bIsMeCanOutCard)
		{//自己还可以出牌.
			//出牌
			m_wCurrentUser = byChairID;
			UINT nElapse = rand()%TIME_GAME_USER_NO_OUTCARD_INTERVAL+TIME_LESS;
			SetGameTimer(IDI_GT_GAME_USER_OUTCARD,nElapse);
		}
	}
	return;
}

//是否开始骰子动画
bool CGameClient::OnSubStartSZFlash()
{
	KillGameTimer(IDI_SZSTART);

	BYTE byMeChairID = GetMeChairID();
	if (byMeChairID==m_wBankerUser)
	{
		if (WIK_NULL != m_cbActionMask)
		{//10秒操作
			m_bIsMeCanOutCard = true;
			UINT nElapse = rand()%TIME_OPERATE_CARD+TIME_LESS+12;
			SetGameTimer(IDI_OPERATE_CARD, nElapse);
		}
		else
		{//30秒出牌
			UINT nElapse = rand()%TIME_GAME_USER_NO_OUTCARD_INTERVAL+TIME_LESS+10;
			SetGameTimer(IDI_GT_GAME_USER_OUTCARD,nElapse);
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
	if (NULL==pGameStart)
	{
		return false;
	}
	
	SetGameStatus(GS_MJ_PLAY);

	//保存王牌
	m_TempKing = pGameStart->cbKingCard;

	BYTE byMeChairID = GetMeChairID();
	//设置变量
	m_wBankerUser=pGameStart->wBankerUser;					//庄家用户
	m_wCurrentUser=pGameStart->wCurrentUser;				//当前出牌用户
	m_cbActionMask=pGameStart->cbUserAction;				//自己动作类型

	//设置扑克
	BYTE cbCardCount=(byMeChairID==m_wBankerUser)?MAX_COUNT:(MAX_COUNT-1);
	m_GameLogic.SwitchToCardIndex(pGameStart->cbCardData,cbCardCount,m_cbCardIndex);

	//动作处理
	if (m_wCurrentUser==byMeChairID)
	{
		::memcpy(m_byGangCard, pGameStart->byGangCard, sizeof(BYTE)*4);
		::memcpy(m_byBuCard, pGameStart->byBuCard, sizeof(BYTE)*4);
		m_byGangCardNums = pGameStart->byGangCardNums;
		m_byBuCardNums = pGameStart->byBuCardNums;

		UINT nElapse = rand()%TIME_USER_SZ_INTERVAL+1+2;
		SetGameTimer(IDI_SZSTART,nElapse);
	}

	if (m_pRoomLog)
		m_pRoomLog->AddMsg(0, 0, TRUE,"***** 游戏开始CGameClient::OnSubGameStart");
	return true;
}


//判断是否手中有这张牌
bool CGameClient::bIsHaveCard(BYTE byCard, BYTE& byLastCard)
{
	byLastCard = 0;
	BYTE byIndex = m_GameLogic.SwitchToCardIndex(byCard);
	if (0==m_cbCardIndex[byIndex])
	{//没有
		for (int i=MAX_INDEX; i>=0; i--)
		{
			if (m_cbCardIndex[i]>0)
			{
				byLastCard = m_GameLogic.SwitchToCardData(i);
				break;
			}
		}
		return false;
	}
	return true;
}

//出牌
void CGameClient::OnOutCard()
{
	//删除定时器
	KillGameTimer(IDI_GT_GAME_USER_OUTCARD);
	KillGameTimer(IDI_OPERATE_CARD);

	BYTE byChairID = GetMeChairID();
	if (byChairID!=m_wCurrentUser)
	{
		return;
	}

	BYTE byOutCard = 0;
	if (m_bHearStatus)
	{//听牌了
		if ((m_cbActionMask&WIK_CHI_HU)>0 && 0!=m_cbActionCard)
		{//可以胡牌了
			OnCardOperate(WIK_CHI_HU, m_cbActionCard);
			return;
		}
		else
		{
			byOutCard = m_cbActionCard;
		}
	}
	//发送数据
	if (0==byOutCard)
	{
		SearchOutCard(byOutCard);
	}
	if (0==byOutCard || !m_GameLogic.IsValidCard(byOutCard))
	{//如果出牌数据有问题，则选择手中最后一张牌。
		BYTE byLastCard = 0;
		bool bIsHave = bIsHaveCard(byOutCard, byLastCard);
		if (!bIsHave)
		{
			byOutCard = byLastCard;
		}
	}
	CMD_C_OutCard OutCard;
	OutCard.cbCardData = byOutCard;
	SendData(SUB_C_OUT_CARD,&OutCard,sizeof(CMD_C_OutCard));

	//设置变量
	m_wCurrentUser = INVALID_CHAIR;
	m_cbActionMask = WIK_NULL;
	m_cbActionCard = 0;
}

//用户发牌
bool CGameClient::OnSubSendCard( const void *pBuffer,WORD wDataSize )
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_SendCard));
	if (wDataSize!=sizeof(CMD_S_SendCard)) return false;

	KillGameTimer(IDI_OPERATE_CARD);
	KillGameTimer(IDI_GT_GAME_USER_OUTCARD);

	//变量定义
	CMD_S_SendCard * pSendCard=(CMD_S_SendCard *)pBuffer;
	if (NULL != pSendCard)
	{
		//设置变量
		WORD wMeChairID=GetMeChairID();
		m_wCurrentUser=pSendCard->wCurrentUser;

		//设置变量
		m_bIsMeCanOutCard = false;

		::memset(m_byGangCard, 0, sizeof(BYTE)*4);
		::memset(m_byBuCard, 0, sizeof(BYTE)*4);
		m_byGangCardNums = 0;
		m_byBuCardNums = 0;
		m_cbActionCard = 0;

		//设置时间
		if( wMeChairID == m_wCurrentUser )
		{
			//手中增加一张扑克
			m_cbCardIndex[m_GameLogic.SwitchToCardIndex(pSendCard->cbCardData)]++;	

			m_cbActionMask = pSendCard->cbActionMask;
			m_cbActionCard = pSendCard->cbCardData;

			if (pSendCard->cbActionMask != WIK_NULL)
			{//有操作
				::memcpy(m_byGangCard, pSendCard->byGangCard, sizeof(BYTE)*4);
				::memcpy(m_byBuCard, pSendCard->byBuCard, sizeof(BYTE)*4);
				m_byGangCardNums = pSendCard->byGangCardNums;
				m_byBuCardNums = pSendCard->byBuCardNums;

				m_bIsMeCanOutCard = true;
				UINT nElapse = rand()%TIME_OPERATE_CARD+TIME_LESS;
				SetGameTimer(IDI_OPERATE_CARD, nElapse);
			}
			else
			{//出牌
				UINT nElapse = rand()%TIME_GAME_USER_NO_OUTCARD_INTERVAL+TIME_LESS;
				SetGameTimer(IDI_GT_GAME_USER_OUTCARD,nElapse);
			}
		}
	}

	return true;
}


//用户出牌
bool CGameClient::OnSubOutCard( const void *pBuffer,WORD wDataSize )
{
	//效验消息
	ASSERT(wDataSize==sizeof(CMD_S_OutCard));
	if (wDataSize!=sizeof(CMD_S_OutCard)) return false;

	KillGameTimer(IDI_OPERATE_CARD);
	KillGameTimer(IDI_GT_GAME_USER_OUTCARD);
	//消息处理
	CMD_S_OutCard * pOutCard=(CMD_S_OutCard *)pBuffer;
	if (NULL != pOutCard)
	{
		//变量定义
		WORD wMeChairID=GetMeChairID();

		//设置变量
		m_wCurrentUser=INVALID_CHAIR;
		m_bIsMeCanOutCard = false;

		//删除扑克
		if( wMeChairID == pOutCard->wOutCardUser )
		{
			m_GameLogic.RemoveCard(m_cbCardIndex,pOutCard->cbOutCardData);
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

	KillGameTimer(IDI_OPERATE_CARD);
	KillGameTimer(IDI_GT_GAME_USER_OUTCARD);

	//变量定义
	CMD_S_OperateNotify * pOperateNotify=(CMD_S_OperateNotify *)pBuffer;
	//用户界面
	if (NULL!=pOperateNotify)
	{
		::memset(m_byGangCard, 0, sizeof(m_byGangCard));
		::memset(m_byBuCard, 0, sizeof(m_byBuCard));
		m_byGangCardNums = 0;
		m_byBuCardNums = 0;

		m_bIsMeCanOutCard = false;

		BYTE byChairID = GetMeChairID();
		m_wCurrentUser = byChairID;
		m_cbActionMask = pOperateNotify->cbActionMask;
		m_cbActionCard = pOperateNotify->cbActionCard;

		if (pOperateNotify->cbActionMask!=WIK_NULL)
		{//可以操作
			::memcpy(m_byGangCard, pOperateNotify->byGangCard, sizeof(BYTE)*4);
			::memcpy(m_byBuCard, pOperateNotify->byBuCard, sizeof(BYTE)*4);
			m_byGangCardNums = pOperateNotify->byGangCardNums;
			m_byBuCardNums = pOperateNotify->byBuCardNums;

			UINT nElapse = rand()%TIME_OPERATE_CARD+TIME_LESS;
			SetGameTimer(IDI_OPERATE_CARD, nElapse);
		}
	}
	return true;
}


//操作结果通知
bool CGameClient::OnSubOperateResult( const void *pBuffer,WORD wDataSize )
{
	//效验消息
	if (wDataSize!=sizeof(CMD_S_OperateResult)) return false;

	//消息处理
	CMD_S_OperateResult * pOperateResult=(CMD_S_OperateResult *)pBuffer;
	if (NULL==pOperateResult)
	{
		return false;
	}
	KillGameTimer(IDI_OPERATE_CARD);
	KillGameTimer(IDI_GT_GAME_USER_OUTCARD);

	m_bIsMeCanOutCard = false;

	if (NULL != pOperateResult)
	{
		//设置变量
		m_cbActionMask = WIK_NULL;
		m_cbActionCard = 0;

		//变量定义
		BYTE cbPublicCard=TRUE;
		WORD wOperateUser=pOperateResult->wOperateUser;
		BYTE cbOperateCard=pOperateResult->cbOperateCard;
		BYTE byChairID = GetMeChairID();
		//设置组合
		if ((pOperateResult->cbOperateCode&(WIK_GANG|WIK_FILL|WIK_TING))>0)
		{//主动玩家,并且是补、杠、听
			//为当前操作玩家
			m_wCurrentUser=INVALID_CHAIR;
			//设置变量
			if ((pOperateResult->cbOperateCode==WIK_TING)&&(wOperateUser==byChairID)) 
			{
				m_bHearStatus = true;
			}

			//扑克设置
			if (byChairID==wOperateUser)
			{
				BYTE byIndex = m_GameLogic.SwitchToCardIndex(cbOperateCard);
				if (byIndex<MAX_INDEX)
				{
					m_cbCardIndex[byIndex]=0;
				}
			}
		}
		else if (pOperateResult->cbOperateCode!=WIK_NULL)
		{//被动玩家，吃、碰的情况
			//为当前操作玩家
			m_wCurrentUser=pOperateResult->wOperateUser;

			//组合界面
			BYTE cbWeaveCard[4]={0,0,0,0},cbWeaveKind=pOperateResult->cbOperateCode;
			BYTE cbWeaveCardCount=m_GameLogic.GetWeaveCard(cbWeaveKind,cbOperateCard,cbWeaveCard);

			//设置扑克
			if (byChairID==wOperateUser)
			{
				//删除扑克
				m_GameLogic.RemoveCard(cbWeaveCard,cbWeaveCardCount,&cbOperateCard,1);
				m_GameLogic.RemoveCard(m_cbCardIndex,cbWeaveCard,cbWeaveCardCount-1);
			}

			if (byChairID==m_wCurrentUser)
			{//我是操作玩家,要打出一张牌
				//设置时间
				UINT nElapse = rand()%TIME_GAME_USER_NO_OUTCARD_INTERVAL+TIME_LESS;
				SetGameTimer(IDI_GT_GAME_USER_OUTCARD,nElapse);
			}
		}
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
	if (NULL==pOperateHaiDi)
	{
		return true;
	}

	CancelAllTimer();

	//设置状态
	SetGameStatus(GS_MJ_HAI_DI);

	//设置变量
	m_bIsMeCanOutCard = false;
	m_wCurrentUser=INVALID_CHAIR;

	BYTE byChairID = GetMeChairID();
	if (NULL != pOperateHaiDi && byChairID==pOperateHaiDi->wCurrentUser)
	{
		m_wCurrentUser = pOperateHaiDi->wCurrentUser;
		WORD wTime = rand()%TIME_USER_HAI_DI_CARD_INTERVAL + TIME_LESS;
		SetGameTimer(IDI_USER_HAI_DI_CARD, wTime);
	}
	
	return true;
}


//听牌操作
bool CGameClient::OnSubTingCard(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_GameTingCard));
	if (wDataSize!=sizeof(CMD_S_GameTingCard)) return false;

	KillGameTimer(IDI_USER_TING_CARD);

	//消息处理
	CMD_S_GameTingCard * pGameTingCard=(CMD_S_GameTingCard *)pBuffer;
	if (NULL == pGameTingCard)
	{
		return false;
	}
	BYTE byChairID = GetMeChairID();
	if (pGameTingCard->wTingCardUser == byChairID)
	{
		m_bHearStatus = false;
		m_wCurrentUser = pGameTingCard->wTingCardUser;

		//设置
		WORD wTime = rand()%TIME_TING_CARD_INTERVAL + TIME_LESS;
		SetGameTimer(IDI_USER_TING_CARD, wTime);
	}

	return true;

}

//听牌结果
bool CGameClient::OnSubTingCardResult(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_GameTingCard));
	if (wDataSize!=sizeof(CMD_S_GameTingCard)) return false;

	//消息处理
	CMD_S_GameTingCard * pGameTingCard=(CMD_S_GameTingCard *)pBuffer;
	if (NULL != pGameTingCard)
	{
		BYTE byChairID = GetMeChairID();
		if (pGameTingCard->wTingCardUser == byChairID)
		{
			KillGameTimer(IDI_USER_TING_CARD);
			m_bHearStatus = true;
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
		
		SetGameStatus(GS_MJ_FREE);

		//准备定时器
		WORD wTime = rand()%TIME_GAME_READY+TIME_LESS;
		SetGameTimer(IDI_GT_GAME_USER_READY,wTime);
	}
	if (m_pRoomLog)
		m_pRoomLog->AddMsg(0, 0, TRUE,"***** 游戏结束CGameClient::OnSubGameEnd");
	return true;
}


//搜索出牌
void CGameClient::SearchOutCard( BYTE& byOutCard)
{
	//初始化
	byOutCard = 0;

	WORD wMeChairId = GetMeChairID();
	if( wMeChairId == INVALID_CHAIR )
	{
		return;
	}

	//转换索引
	BYTE bIndex=m_GameLogic.SwitchToCardIndex(m_TempKing);
	BYTE byCard[MAX_COUNT]={0};
	BYTE byCardCount = 0;
	for( BYTE i = 0; i < MAX_INDEX; i++ )
	{
		if (bIndex==i) continue;
		for( BYTE j = 0; j < m_cbCardIndex[i]; j++ )
		{
			byCard[byCardCount++] = i;
		}
	}

	BYTE cbActionCard = m_cbActionCard;
	//计算原始分
	CAndroidAI AndroidAi;
	AndroidAi.SetEnjoinOutCard(m_TempKing, 1);
	AndroidAi.SetCardData(byCard,byCardCount);
	AndroidAi.Think();

	//搜索废牌
	BYTE cbOutCardData = cbActionCard;
	BYTE byBadlyIndex = AndroidAi.GetBadlyCard();
	if( byBadlyIndex == 0xff )
	{
		byBadlyIndex = AndroidAi.GetBadlyIn2Card();
		if( byBadlyIndex == 0xff )
		{
			byBadlyIndex = AndroidAi.GetBadlyIn3Card();
		}
	}
	cbOutCardData = m_GameLogic.SwitchToCardData(byBadlyIndex);

	if (!m_GameLogic.IsValidCard(cbOutCardData))
	{
		byOutCard = m_cbActionCard;
	}
	else
	{
		byOutCard = cbOutCardData;
	}
}