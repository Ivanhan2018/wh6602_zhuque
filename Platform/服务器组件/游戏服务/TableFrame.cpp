#include "StdAfx.h"
#include "TableFrame.h"

//定时器 ID
#define IDI_OFFLINE					(IDI_MAX_TIME_ID+1)					//断线定时器

//////////////////////////////////////////////////////////////////////////

//构造函数
CTableFrame::CTableFrame()
{
	m_pRoomLog = NULL;

    m_endTime = (DWORD)time(NULL);
	//属性变量
	m_wChairCount=0;
	m_wTableID=INVALID_TABLE;

	//状态变量
	m_bTableLocked=false;
	m_bGameStarted=false;

	//状态变量
	m_dwTimeStart=0L;
	m_bGameStatus=GS_FREE;
	ZeroMemory(m_szPassword,sizeof(m_szPassword));

	//分数信息
	m_lGameTaxScore=0L;
	ZeroMemory(m_ScoreInfo,sizeof(m_ScoreInfo));
	ZeroMemory(m_dwPlayerID,sizeof(m_dwPlayerID));
	ZeroMemory(m_OriginGold,sizeof(m_OriginGold));

	//用户信息
	ZeroMemory(m_pIUserItem,sizeof(m_pIUserItem));
	ZeroMemory(m_bAllowLookon,sizeof(m_bAllowLookon));
	ZeroMemory(m_wOffLineCount,sizeof(m_wOffLineCount));

	//配置变量
	m_pGameServiceAttrib=NULL;
	m_pGameServiceOption=NULL;

	//接口变量
	m_pITableFrameSink=NULL;
	m_pITableUserAction=NULL;
	m_pIGameServiceFrame=NULL;
	m_pITableUserActionEX=NULL;

	return;
}

//析构函数
CTableFrame::~CTableFrame()
{
	//释放对象
	if (m_pITableFrameSink!=NULL)
	{
		m_pITableFrameSink->Release();
		m_pITableFrameSink=NULL;
	}

	return;
}

//接口查询
void * __cdecl CTableFrame::QueryInterface(const IID & Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(ITableFrame,Guid,dwQueryVer);
	QUERYINTERFACE(ITableFrameManager,Guid,dwQueryVer);
	QUERYINTERFACE(ITableFrameControl,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(ITableFrameManager,Guid,dwQueryVer);
	return NULL;
}

// 设置日志指针
void __cdecl CTableFrame::SetRoomLog(cnd_ILogMgr*	pRoomLog)
{
	m_pRoomLog = pRoomLog;;	// 房间日志

	if(!m_pRoomLog)		// 此段测试用
	{
		CString strPromptNull;	
		strPromptNull.Format("问题：m_pRoomLog=NULL ，函数名：%s  所在行：%d ",  __FUNCTION__ ,  __LINE__  );	
		//AfxMessageBox(strPromptNull);
	}
}

//初始化
bool __cdecl CTableFrame::InitTableFrame(WORD wTableID, tagTableFrameParameter * pTableFrameParameter)
{
	__ENTER_FUNCTION_LOG_CATCH

	//效验参数
	ASSERT(pTableFrameParameter!=NULL);

	//辅助变量
	m_ClientReadyUser.RemoveAll();

	//设置变量
	m_wTableID=wTableID;
	m_pGameServiceAttrib=pTableFrameParameter->pGameServiceAttrib;
	m_pGameServiceOption=pTableFrameParameter->pGameServiceOption;
	m_wChairCount=pTableFrameParameter->pGameServiceAttrib->wChairCountMax;

	//查询接口
	m_pIGameServiceFrame=QUERY_OBJECT_PTR_INTERFACE(pTableFrameParameter->pIGameServiceFrame,IGameServiceFrame);

	//错误判断
	if (m_pIGameServiceFrame==NULL)
	{
		ASSERT(FALSE);
		return false;
	}

	//创建桌子
	IGameServiceManager * pIGameServiceManager=QUERY_OBJECT_PTR_INTERFACE(pTableFrameParameter->pIGameServiceManager,IGameServiceManager);
	m_pITableFrameSink=(ITableFrameSink *)pIGameServiceManager->CreateTableFrameSink(IID_ITableFrameSink,VER_ITableFrameSink);

	//错误判断
	if (m_pITableFrameSink==NULL)
	{
		ASSERT(FALSE);
		return false;
	}

	//设置接口
	IUnknownEx * pIUnknownEx=QUERY_ME_INTERFACE(IUnknownEx);
	if (m_pITableFrameSink->InitTableFrameSink(pIUnknownEx)==false) return false;

	//扩展接口
	m_pITableUserAction=QUERY_OBJECT_PTR_INTERFACE(m_pITableFrameSink,ITableUserAction);
	m_pITableUserActionEX=QUERY_OBJECT_PTR_INTERFACE(m_pITableFrameSink,ITableUserActionEX);

	__LEAVE_FUNCTION_LOG_CATCH

	return true;
}

//游戏状态
bool __cdecl CTableFrame::IsUserPlaying(IServerUserItem * pIServerUserItem)
{
	//游戏状态
	if (m_bGameStarted==false) return false;

	//用户状态
	BYTE cbUserStatus=pIServerUserItem->GetUserStatus();
	if ((cbUserStatus!=US_PLAY)&&(cbUserStatus!=US_OFFLINE)) return false;

	//逻辑判断
	WORD wChairID=pIServerUserItem->GetChairID();
	return m_pITableFrameSink->IsUserPlaying(wChairID);
}

//离开动作
bool __cdecl CTableFrame::PerformStandUpAction(IServerUserItem * pIServerUserItem)
{
	__ENTER_FUNCTION_LOG_CATCH

	//效验参数
	ASSERT(pIServerUserItem!=NULL);
	ASSERT(pIServerUserItem->GetTableID()==m_wTableID);
	ASSERT(pIServerUserItem->GetChairID()<m_wChairCount);

	//变量定义
	WORD wChairID=pIServerUserItem->GetChairID();
	BYTE cbUserStatus=pIServerUserItem->GetUserStatus();
	IServerUserItem * pITableUserItem=GetServerUserItem(wChairID);

	//用户处理
	if (pITableUserItem==pIServerUserItem)
	{
		//变量定义
		bool bTableLocked=IsTableLocked();
		bool bGameStarted=IsGameStarted();

		//设置变量
		m_bAllowLookon[wChairID]=false;
		m_ClientReadyUser.RemoveKey(pIServerUserItem->GetUserID());

		//结束游戏
		
		{
			//结束游戏
			if(bGameStarted)
				m_pITableFrameSink->OnEventGameEnd(wChairID,pIServerUserItem,GER_NORMAL/*GER_USER_LEFT*/);//by Ivan_han

			//离开判断
			tagServerUserData * pUserData=pIServerUserItem->GetUserData();
			if ((pUserData->wTableID==INVALID_TABLE)||(pUserData->wChairID==INVALID_CHAIR)) return true;
		}

		//变量定义
		CMD_GF_LookonControl LookonControl;
		ZeroMemory(&LookonControl,sizeof(LookonControl));

		//设置变量
		LookonControl.dwUserID=0L;
		LookonControl.bAllowLookon=FALSE;

		//发送消息
		for (INT_PTR i=0;i<m_LookonUserItemPtr.GetCount();i++)
		{
			//获取用户
			IServerUserItem * pILookonUserItem=m_LookonUserItemPtr[i];

			//发送消息
			if ((pILookonUserItem->GetChairID()==wChairID)&&(IsClientReady(pILookonUserItem)==true))
			{
				m_pIGameServiceFrame->SendData(pILookonUserItem,MDM_GF_FRAME,SUB_GF_LOOKON_CONTROL,&LookonControl,sizeof(LookonControl));
			}
		}

		//设置用户
		m_pIUserItem[wChairID]=NULL;
		pIServerUserItem->SetUserStatus(US_FREE,INVALID_TABLE,INVALID_CHAIR);
		m_pIGameServiceFrame->SendUserStatus(pIServerUserItem);

		//统计人数
		WORD wUserCount=0;
		for (WORD i=0;i<m_wChairCount;i++)
		{
			if (m_pIUserItem[i]!=NULL) wUserCount++;
		}

		//设置密码
		if (wUserCount==0)
		{
			m_bTableLocked=false;
			ZeroMemory(m_szPassword,sizeof(m_szPassword));
		}

		//踢走旁观
		if (wUserCount==0)
		{
			for (INT_PTR i=0;i<m_LookonUserItemPtr.GetCount();i++)
			{
				SendGameMessage(m_LookonUserItemPtr[i],TEXT("此游戏桌的所有玩家已经离开了！"),SMT_CLOSE_GAME|SMT_EJECT|SMT_INFO);
			}
		}

		//发送状态
		if ((bTableLocked!=IsTableLocked())||(bGameStarted!=IsGameStarted()))
		{
			m_pIGameServiceFrame->SendTableStatus(m_wTableID);
		}

		//起立处理
		if (m_pITableUserAction!=NULL) m_pITableUserAction->OnActionUserStandUp(wChairID,pIServerUserItem,false);

		//变量定义
		bool bMatchServer=((m_pGameServiceOption->wServerType&GAME_GENRE_MATCH)!=0);
		bool bControlStart=((bMatchServer==true)&&m_pGameServiceOption->cbControlStart==TRUE);

		//开始判断
		if ((bControlStart==false)&&(StartVerdict()==true))
		{
			StartGame();
			m_pIGameServiceFrame->SendUserStatus(pIServerUserItem);
			return true;
		}
	}
	else
	{
		//旁观用户
		for (INT_PTR i=0;i<m_LookonUserItemPtr.GetCount();i++)
		{
			if (pIServerUserItem==m_LookonUserItemPtr[i])
			{
				//设置变量
				m_ClientReadyUser.RemoveKey(pIServerUserItem->GetUserID());

				//设置用户
				m_LookonUserItemPtr.RemoveAt(i);
				pIServerUserItem->SetUserStatus(US_FREE,INVALID_TABLE,INVALID_CHAIR);
				m_pIGameServiceFrame->SendUserStatus(pIServerUserItem);

				//起立处理
				if (m_pITableUserAction!=NULL) m_pITableUserAction->OnActionUserStandUp(wChairID,pIServerUserItem,true);

				return true;
			}
		}
	}

	__LEAVE_FUNCTION_LOG_CATCH


	return true;
}

//旁观动作
bool __cdecl CTableFrame::PerformLookonAction(WORD wChairID, IServerUserItem * pIServerUserItem,LPCTSTR szPassword)
{
	__ENTER_FUNCTION_LOG_CATCH

	//效验参数
	ASSERT(pIServerUserItem!=NULL);
	ASSERT(wChairID<m_wChairCount);
	ASSERT(pIServerUserItem->GetTableID()==INVALID_TABLE);
	ASSERT(pIServerUserItem->GetChairID()==INVALID_CHAIR);

	//变量定义
	DWORD dwUserRight=pIServerUserItem->GetUserData()->dwUserRight;
	BYTE cbMasterOrder=pIServerUserItem->GetUserData()->cbMasterOrder;

	//权限判断
	if (CUserRight::CanLookon(dwUserRight)==false)
	{
		//发送消息
		SendSitFailedPacket(pIServerUserItem,TEXT("抱歉，您没有进行旁观游戏的权限，若需要帮助，请联系游戏客服咨询！"));

		return false;
	}

	//关闭查询
	if ((cbMasterOrder==0L)&&(m_pIGameServiceFrame->IsShallClose()==true))
	{
		SendSitFailedPacket(pIServerUserItem,TEXT("由于此游戏房间即将暂停服务，玩家不允许再进入游戏桌！"));
		return false;
	}

	//禁止查询
	if ((cbMasterOrder==0L)&&(m_pIGameServiceFrame->IsAllowEnterGame()==false))
	{
		SendSitFailedPacket(pIServerUserItem,TEXT("抱歉，此游戏桌现在不允许玩家进入！"));
		return false;
	}

	//位置判断
	if (m_pIUserItem[wChairID]==NULL)
	{
		SendSitFailedPacket(pIServerUserItem,TEXT("所请求旁观的位置已经没有玩家了，不能旁观！"));
		return false;
	}

	//密码效验
	if ((m_bTableLocked==true)&&(cbMasterOrder==0L))
	{
		//密码效验
		if (szPassword==NULL || lstrcmp(m_szPassword,szPassword)!=0)
		{
			LPCTSTR pszDescribe=TEXT("密码错误，不能加入游戏！");
			SendSitFailedPacket(pIServerUserItem,pszDescribe);
			return false;
		}
	}

	//状态判断
	if ((m_bGameStarted==false)&&(cbMasterOrder==0L))
	{
		SendSitFailedPacket(pIServerUserItem,TEXT("游戏还没有开始，暂时不能旁观！"));
		return false;
	}

	//设置玩家
	m_LookonUserItemPtr.Add(pIServerUserItem);
	pIServerUserItem->SetUserStatus(US_LOOKON,m_wTableID,wChairID);
	m_pIGameServiceFrame->SendUserStatus(pIServerUserItem);

	//旁观处理
	if (m_pITableUserAction!=NULL) m_pITableUserAction->OnActionUserSitDown(wChairID,pIServerUserItem,true);

	__LEAVE_FUNCTION_LOG_CATCH

	return true;
}

//坐下动作
bool __cdecl CTableFrame::PerformSitDownAction(WORD wChairID, IServerUserItem * pIServerUserItem, LPCTSTR szPassword)
{
	__ENTER_FUNCTION_LOG_CATCH


	//效验参数
	ASSERT(pIServerUserItem!=NULL);
	ASSERT(wChairID<m_wChairCount);
	ASSERT(pIServerUserItem->GetTableID()==INVALID_TABLE);
	ASSERT(pIServerUserItem->GetChairID()==INVALID_CHAIR);

	//变量定义
	const tagUserScore * pUserScore=pIServerUserItem->GetUserScore();
	const tagUserRule * pUserRule=pIServerUserItem->GetUserRule(),* pTableUserRule=NULL;
	tagServerUserData * pUserData=pIServerUserItem->GetUserData(),* pTableUserData=NULL;

	//关闭查询
	if ((pUserData->dwMasterRight==0L)&&(m_pIGameServiceFrame->IsShallClose()==true))
	{
		SendSitFailedPacket(pIServerUserItem,TEXT("由于此游戏房间即将暂停服务，玩家不允许再进入游戏桌！"));
		return false;
	}

	//禁止查询
	if ((pUserData->dwMasterRight==0L)&&(m_pIGameServiceFrame->IsAllowEnterGame()==false))
	{
		SendSitFailedPacket(pIServerUserItem,TEXT("抱歉，此游戏桌现在不允许玩家进入！"));
		return false;
	}

	//位置判断
	if (m_pIUserItem[wChairID]!=NULL)
	{
		//存在判断
		if ((m_pIUserItem[wChairID]->GetChairID()==wChairID)&&(m_pIUserItem[wChairID]->GetTableID()==m_wTableID))
		{
			//发送消息
			TCHAR szDescribe[128]=TEXT("");
			pTableUserData=m_pIUserItem[wChairID]->GetUserData();
			_snprintf(szDescribe,sizeof(szDescribe),TEXT("椅子已经被 [ 玩家 ] 捷足先登了，下次动作要快点了！"),pTableUserData->szAccounts);
			SendSitFailedPacket(pIServerUserItem,szDescribe);
		}
		else
		{
			ASSERT(FALSE);
		}

		return false;
	}

	//游戏状态
	if ((m_bGameStarted==true)&&(m_pGameServiceAttrib->cbJoinInGame==FALSE))
	{
		SendSitFailedPacket(pIServerUserItem,TEXT("游戏已经开始了，暂时不能进入游戏桌！"));
		return false;
	}

	//比赛判断
	if (m_pGameServiceOption->wServerType==GAME_GENRE_MATCH)
	{
		//比赛权限
		if (CUserRight::IsMatchUser(pUserData->dwUserRight)==false)
		{
			TCHAR szDescribe[128]=TEXT("");
			lstrcpyn(szDescribe,TEXT("这是游戏比赛房间，您不是比赛选手，不能坐到此位置上！ "),CountArray(szDescribe));
			SendSitFailedPacket(pIServerUserItem,szDescribe);
			return false;
		}

		//地址效验
		DWORD dwUserIP=pIServerUserItem->GetClientIP();
		for (WORD i=0;i<m_wChairCount;i++)
		{
			if ((m_pIUserItem[i]!=NULL)&&(m_pIUserItem[i]->GetClientIP()==dwUserIP))
			{
				if (pUserRule->bCheckSameIP)
				{
					LPCTSTR pszDescribe=TEXT("您设置了不跟相同 IP 地址的玩家游戏，此游戏桌存在与您 IP 地址相同的玩家，不能加入游戏！");
					SendSitFailedPacket(pIServerUserItem,pszDescribe);
					return false;
				}
				else if (m_pGameServiceOption->cbUnSameIPPlay==TRUE)
				{
					LPCTSTR pszDescribe=TEXT("此桌设置了不跟相同 IP 地址的玩家游戏，此游戏桌存在与您 IP 地址相同的玩家，不能加入游戏！");
					SendSitFailedPacket(pIServerUserItem,pszDescribe);
					return false;
				}
			}
		}
		if (pUserRule->bCheckSameIP)
		{
			for (WORD i=0;i<m_wChairCount-1;i++)
			{
				if (m_pIUserItem[i]!=NULL)
				{
					for (WORD j=i+1;j<m_wChairCount;j++)
					{
						if ((m_pIUserItem[j]!=NULL)&&(m_pIUserItem[i]->GetClientIP()==m_pIUserItem[j]->GetClientIP()))
						{
							LPCTSTR pszDescribe=TEXT("您设置了不跟相同 IP 地址的玩家游戏，此游戏桌存在 IP 地址相同的玩家，不能加入游戏！");
							SendSitFailedPacket(pIServerUserItem,pszDescribe);
							return false;
						}
					}
				}
			}
		}

		//对局局数
		if (m_pGameServiceOption->lMatchDraw>0)
		{
			LONG lPlayCount=pUserData->UserScoreInfo.lWinCount+pUserData->UserScoreInfo.lLostCount+pUserData->UserScoreInfo.lDrawCount;
			if (lPlayCount>=m_pGameServiceOption->lMatchDraw)
			{
				TCHAR szDescribe[128]=TEXT("");
				lstrcpyn(szDescribe,TEXT("恭喜您，您的比赛局数已经完成了，不需要再继续比赛，请耐心等待赛果公布！ "),CountArray(szDescribe));
				SendSitFailedPacket(pIServerUserItem,szDescribe);
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
			LPCTSTR pszMessage=TEXT("抱歉，您没有进行游戏的权限，若需要帮助，请联系游戏客服咨询！");
			SendSitFailedPacket(pIServerUserItem,pszMessage);
			return true;
		}

		//积分限制//修改
		if (pUserData->UserScoreInfo.lGold < m_pGameServiceOption->lRestrictScore)
		{
			//发送消息
			TCHAR szDescribe[128]=TEXT("");
			if (m_pGameServiceOption->wServerType==GAME_GENRE_GOLD)
			{
				_snprintf(szDescribe,sizeof(szDescribe),TEXT("加入游戏至少需要 %I64d 个金币，您的金币不够，不能加入！"),m_pGameServiceOption->lEnterMoney);
			}
			SendSitFailedPacket(pIServerUserItem,szDescribe);
			m_pIGameServiceFrame->SendWarningPacket(pIServerUserItem, MDM_GR_USER, SUB_GR_NO_GOLD, szDescribe);
			return false;
		}

		//等级限制
		if ((LONG)pUserData->UserScoreInfo.dwGrade > m_pGameServiceOption->lRestrictGrade)
		{
			//发送消息
			TCHAR szDescribe[128]=TEXT("");
			if (m_pGameServiceOption->wServerType==GAME_GENRE_GOLD)
			{
				_snprintf(szDescribe,sizeof(szDescribe),TEXT("此房间不允许高于%I64d级的用户进入！"),m_pGameServiceOption->lRestrictGrade);
			}
			SendSitFailedPacket(pIServerUserItem,szDescribe);
			m_pIGameServiceFrame->SendWarningPacket(pIServerUserItem, MDM_GR_USER, SUB_GR_LOW_GRADE, szDescribe);
			return false;
		}


		//密码效验
		if ((m_bTableLocked==true)&&(pUserData->dwMasterRight==0L))
		{
			if (szPassword==NULL || lstrcmp(m_szPassword,szPassword)!=0)
			{
				LPCTSTR pszDescribe=TEXT("密码错误，不能加入游戏！");
				SendSitFailedPacket(pIServerUserItem,pszDescribe);
				return false;
			}
		}

		//积分范围
		WORD wWinRate=pIServerUserItem->GetUserWinRate();
		WORD wFleeRate=pIServerUserItem->GetUserFleeRate();
		for (WORD i=0;i<m_wChairCount;i++)
		{
			if (m_pIUserItem[i]!=NULL)
			{
				//获取规则
				pTableUserRule=m_pIUserItem[i]->GetUserRule();

				//逃率效验
				if ((pTableUserRule->bLimitFlee)&&(wFleeRate>pTableUserRule->wFleeRate))
				{
					TCHAR szDescribe[128]=TEXT("");
					_snprintf(szDescribe,sizeof(szDescribe),TEXT("您的逃跑率太高，与 %s 设置的设置不符，不能加入游戏！"),m_pIUserItem[i]->GetAccounts());
					SendSitFailedPacket(pIServerUserItem,szDescribe);
					return false;
				}

				//胜率效验
				if ((pTableUserRule->bLimitWin)&&(wWinRate<pTableUserRule->wWinRate))
				{
					TCHAR szDescribe[128]=TEXT("");
					_snprintf(szDescribe,sizeof(szDescribe),TEXT("您的胜率太低，与 %s 设置的设置不符，不能加入游戏！"),m_pIUserItem[i]->GetAccounts());
					SendSitFailedPacket(pIServerUserItem,szDescribe);
					return false;
				}

				//积分效验
				if (pTableUserRule->bLimitScore==true)
				{
					if (pUserScore->lGold>pTableUserRule->lMaxScore)
					{
						TCHAR szDescribe[128]=TEXT("");
						_snprintf(szDescribe,sizeof(szDescribe),TEXT("您的积分太高，与 %s 设置的设置不符，不能加入游戏！"),m_pIUserItem[i]->GetAccounts());
						SendSitFailedPacket(pIServerUserItem,szDescribe);
						return false;
					}
					if (pUserScore->lGold<pTableUserRule->lLessScore)
					{
						TCHAR szDescribe[128]=TEXT("");
						_snprintf(szDescribe,sizeof(szDescribe),TEXT("您的积分太低，与 %s 设置的设置不符，不能加入游戏！"),m_pIUserItem[i]->GetAccounts());
						SendSitFailedPacket(pIServerUserItem,szDescribe);
						return false;
					}
				}
			}
		}

		//限制判断
		if (m_pGameServiceOption->cbUnSameIPPlay==TRUE)
		{
			bool bPlay = true;
			DWORD dwUserIP=pIServerUserItem->GetClientIP();
			for (WORD i=0;i<m_wChairCount;i++)
			{
				if ((m_pIUserItem[i]!=NULL)&&(m_pIUserItem[i]->GetClientIP()==dwUserIP))
				{
					bPlay = false;
				}
			}
			if(bPlay)
			{
				for (WORD i=0;i<m_wChairCount-1;i++)
				{
					if (m_pIUserItem[i]==NULL)continue;

					for (WORD j=i+1;j<m_wChairCount;j++)
					{
						if ((m_pIUserItem[j]!=NULL)&&(m_pIUserItem[i]->GetClientIP()==m_pIUserItem[j]->GetClientIP()))
						{
							bPlay = false;
						}
					}
				}
			}

			if(!bPlay)
			{
				LPCTSTR pszDescribe=TEXT("本房间设置了相同IP不可坐在同一桌,请选择其它坐位!");
				SendSitFailedPacket(pIServerUserItem,pszDescribe);
				return false;
			}
		}

		//地址效验
		bool bCheckSameIP=pUserRule->bCheckSameIP;
		for (WORD i=0;i<m_wChairCount;i++)
		{
			if (m_pIUserItem[i]!=NULL)
			{
				pTableUserRule=m_pIUserItem[i]->GetUserRule();
				if (pTableUserRule->bCheckSameIP==true) 
				{
					bCheckSameIP=true;
					break;
				}
			}
		}
		if (bCheckSameIP==true)
		{
			DWORD dwUserIP=pIServerUserItem->GetClientIP();
			for (WORD i=0;i<m_wChairCount;i++)
			{
				if ((m_pIUserItem[i]!=NULL)&&(m_pIUserItem[i]->GetClientIP()==dwUserIP))
				{
					if (pUserRule->bCheckSameIP)
					{
						LPCTSTR pszDescribe=TEXT("您设置了不跟相同 IP 地址的玩家游戏，此游戏桌存在与您 IP 地址相同的玩家，不能加入游戏！");
						SendSitFailedPacket(pIServerUserItem,pszDescribe);
						return false;
					}
				}
			}
			for (WORD i=0;i<m_wChairCount-1;i++)
			{
				if (m_pIUserItem[i]!=NULL)
				{
					for (WORD j=i+1;j<m_wChairCount;j++)
					{
						if ((m_pIUserItem[j]!=NULL)&&(m_pIUserItem[i]->GetClientIP()==m_pIUserItem[j]->GetClientIP()))
						{
							LPCTSTR pszDescribe=TEXT("您设置了不跟相同 IP 地址的玩家游戏，此游戏桌存在 IP 地址相同的玩家，不能加入游戏！");
							SendSitFailedPacket(pIServerUserItem,pszDescribe);
							return false;
						}
					}
				}
			}
		}
	}

	//更新密码
	WORD wUserCount=0;
	for (WORD i=0;i<m_wChairCount;i++)
	{
		if (m_pIUserItem[i]!=NULL) wUserCount++;
	}
	if (wUserCount==0 && m_pGameServiceAttrib->wChairCountMax <= MAX_CHAIR_NORMAL)
	{
		m_bTableLocked=pUserRule->bPassword;
		if (m_bTableLocked==true) lstrcpyn(m_szPassword,pUserRule->szPassword,CountArray(m_szPassword));
	}

	//设置玩家
	m_bAllowLookon[wChairID]=false;
	m_pIUserItem[wChairID]=pIServerUserItem;

	//设置状态
	if (m_bGameStarted==true)
	{
		pIServerUserItem->SetUserStatus(US_PLAY,m_wTableID,wChairID);
	}
	else
	{
		pIServerUserItem->SetUserStatus(US_SIT,m_wTableID,wChairID);
	}

	//发送状态
	m_pIGameServiceFrame->SendTableStatus(m_wTableID);
	m_pIGameServiceFrame->SendUserStatus(pIServerUserItem);

	//坐下处理
	if (m_pITableUserAction!=NULL)
	{	
		m_pITableUserAction->OnActionUserSitDown(wChairID,pIServerUserItem,false);
	}

	__LEAVE_FUNCTION_LOG_CATCH

	return true;
}


//断线事件
bool __cdecl CTableFrame::OnUserOffLine(WORD wChairID)
{
	__ENTER_FUNCTION_LOG_CATCH


	//效验状态
	ASSERT(wChairID<m_wChairCount);
	//ASSERT(m_pIUserItem[wChairID]!=NULL);
	if(m_pIUserItem[wChairID]==NULL)
		return false;

	//设置状态
	m_wOffLineCount[wChairID]++;
	m_pIUserItem[wChairID]->SetUserStatus(US_OFFLINE,m_wTableID,wChairID);
	m_pIGameServiceFrame->SendUserStatus(m_pIUserItem[wChairID]);

	//设置用户
	m_ClientReadyUser.RemoveKey(m_pIUserItem[wChairID]->GetUserID());

	//断线处理
	if (m_pITableUserAction!=NULL) m_pITableUserAction->OnActionUserOffLine(wChairID,m_pIUserItem[wChairID]);
    //注释掉by Ivan_han
	////设置定时器
	//DWORD dwTimerID=IDI_OFFLINE+wChairID;
	//WPARAM wBindParam=m_pIUserItem[wChairID]->GetUserID();
	//m_pIGameServiceFrame->SetTableTimer(m_wTableID,dwTimerID,90000L,1,wBindParam);

	////构造消息
	//TCHAR szMessage[512]=TEXT("");
	//_sntprintf(szMessage,sizeof(szMessage),TEXT("[玩家] 断线了，请耐心等待90秒。"),m_pIUserItem[wChairID]->GetAccounts());

	////发送消息
	//for (WORD i=0;i<MAX_CHAIR;i++)
	//{
	//	if ((i!=wChairID)&&(m_pIUserItem[i]!=NULL))
	//	{
	//		SendGameMessage(m_pIUserItem[i],szMessage,SMT_INFO);
	//	}
	//}

	__LEAVE_FUNCTION_LOG_CATCH


	return true;
}

//重进事件
bool __cdecl CTableFrame::OnUserReConnect(WORD wChairID)
{
	__ENTER_FUNCTION_LOG_CATCH

	//效验状态
	ASSERT(wChairID<m_wChairCount);
	ASSERT(m_pIUserItem[wChairID]!=NULL);

	//删除定时器
	m_pIGameServiceFrame->KillTableTimer(m_wTableID,IDI_OFFLINE+wChairID);

	//设置用户
	m_pIUserItem[wChairID]->SetUserStatus(US_PLAY,m_wTableID,wChairID);
	m_pIGameServiceFrame->SendUserStatus(m_pIUserItem[wChairID]);

	//重入处理
	if (m_pITableUserAction!=NULL) m_pITableUserAction->OnActionUserReConnect(wChairID,m_pIUserItem[wChairID]);

	__LEAVE_FUNCTION_LOG_CATCH

	return true;
}

//请求断线
bool __cdecl CTableFrame::OnUserReqOffLine(WORD wChairID)
{
	//效验参数
	ASSERT(wChairID<m_wChairCount);
	ASSERT(m_pIUserItem[wChairID]!=NULL);
	if (wChairID>=m_wChairCount) return false;
	if (m_pIUserItem[wChairID]==NULL) return false;

	//比赛模式
	if (m_pGameServiceOption->wServerType==GAME_GENRE_MATCH)
	{
		return m_wOffLineCount[wChairID]<5;
	}

	//常规模式
	return m_wOffLineCount[wChairID]<3;
}

//定时器事件
bool __cdecl CTableFrame::OnEventTimer(DWORD dwTimerID, WPARAM wBindParam)
{
	//桌子定时器
	if (dwTimerID>IDI_MAX_TIME_ID)
	{
		//断线处理
		if ((dwTimerID>=IDI_OFFLINE)&&(dwTimerID<(DWORD)(IDI_OFFLINE+m_wChairCount)))
		{
			//效验状态
			ASSERT(m_bGameStarted==true);
			if (m_bGameStarted==false) return false;

			//变量定义
			WORD wChairID=(WORD)(dwTimerID-IDI_OFFLINE);
			IServerUserItem * pIServerUserItem=m_pIUserItem[wChairID];

			//断线处理
			if (pIServerUserItem!=NULL)
			{
				//状态判断
				if (pIServerUserItem->GetUserID()!=wBindParam) return false;
				if (pIServerUserItem->GetUserStatus()!=US_OFFLINE) return false;

				//用户起来
				PerformStandUpAction(pIServerUserItem);

				//清理用户
				if (pIServerUserItem->IsActive()==true)
				{
					m_pIGameServiceFrame->DeleteUserItem(pIServerUserItem);
				}

				return true;
			}

			return false;
		}

		return false;
	}

	//游戏定时器
	return m_pITableFrameSink->OnTimerMessage((WORD)dwTimerID,wBindParam);
}

//游戏事件处理
bool __cdecl CTableFrame::OnEventSocketGame(WORD wSubCmdID, const void * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	//效验参数
	ASSERT(pIServerUserItem!=NULL);
	ASSERT(m_pITableFrameSink!=NULL);

	//消息处理
	return m_pITableFrameSink->OnGameMessage(wSubCmdID,pDataBuffer,wDataSize,pIServerUserItem);
}

//框架事件处理
bool __cdecl CTableFrame::OnEventSocketFrame(WORD wSubCmdID, const void * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	//效验参数
	ASSERT(pIServerUserItem!=NULL);
	ASSERT(m_pITableFrameSink!=NULL);

	//消息处理
	bool bSuccess=m_pITableFrameSink->OnFrameMessage(wSubCmdID,pDataBuffer,wDataSize,pIServerUserItem);
	if (bSuccess==false)
	{
		switch (wSubCmdID)
		{
		case SUB_GF_INFO:	//游戏信息
			{
				//效验参数
				ASSERT(wDataSize==sizeof(CMD_GF_Info));
				if (wDataSize<sizeof(CMD_GF_Info)) return false;

				//变量定义
				CMD_GF_Info * pInfo=(CMD_GF_Info *)pDataBuffer;
				tagServerUserData * pUserData=pIServerUserItem->GetUserData();
				bool bLookonUser=(pUserData->cbUserStatus==US_LOOKON);

				//效验状态
				ASSERT(pUserData->wChairID<m_wChairCount);
				if (pUserData->wChairID>=m_wChairCount) return false;

				//设置用户
				m_ClientReadyUser[pUserData->dwUserID]=pUserData->dwUserID;

				//设置变量
				if (bLookonUser==false) m_bAllowLookon[pUserData->wChairID]=pInfo->bAllowLookon?true:false;

				//发送配置
				CMD_GF_Option Option;
				Option.bGameStatus=m_bGameStatus;
				Option.bAllowLookon=m_bAllowLookon[pUserData->wChairID]?TRUE:FALSE;
				m_pIGameServiceFrame->SendData(pIServerUserItem,MDM_GF_FRAME,SUB_GF_OPTION,&Option,sizeof(Option));
//=======================
				//断线重回
				if (100<=m_bGameStatus)
				{
					// 5级以下及120分钟内，游戏界面需要显示在线计时 Author<cxf>
					DWORD dwUserOnlineTime = pIServerUserItem->GetUserData()->dwOnlineTime;
					int iPlayTimeCount = GetPlayTimeCount();
					if (5 > pIServerUserItem->GetUserScore()->dwGrade  &&  
						120 > (dwUserOnlineTime+iPlayTimeCount)/60)
					{
						DWORD	dwMaxTimer[]={600, 1800, 3600, 7200};//15、30、60、120分钟
						int		iSurplus = 0;
						for (int j=0;j<4;j++)
						{
							if (dwUserOnlineTime<dwMaxTimer[j])
							{
								iSurplus=dwMaxTimer[j];
								break;
							}
						}
						CMD_GF_ONLINE onlineContent;
						onlineContent.dwChairID = pUserData->wChairID;		//桌子ID
						onlineContent.bStart = TRUE;		//开始、停止计时(TRUE:开始计时; false:停止计时)
						onlineContent.iToDayOnlineTime = iSurplus-(dwUserOnlineTime+iPlayTimeCount);			//在线时长
						m_pIGameServiceFrame->SendData(pIServerUserItem,MDM_GF_PRESENT,SUB_GF_ONLINE_TIME,&onlineContent,sizeof(CMD_GF_ONLINE));  //开始计时消息
					}
				}
//===========================
				//m_pAttemperEngineSink->SendResidualProperty(pIServerUserItem); //彭文添加 发送道具信息
				//m_pAttemperEngineSink->SendFindGemsTask(pIServerUserItem); //彭文添加 发送任务列表
				//m_pAttemperEngineSink->SendAffiche(pIServerUserItem); //彭文添加，发送游戏公告
				//发送场景
				bool bSendSecret=((bLookonUser==false)||(m_bAllowLookon[pUserData->wChairID]==true));
				return m_pITableFrameSink->SendGameScene(pUserData->wChairID,pIServerUserItem,m_bGameStatus,bSendSecret);
			}
		case SUB_GF_USER_READY:	//用户同意
			{
				//变量定义
				tagServerUserData * pUserData=pIServerUserItem->GetUserData();
				bool bLookonUser=(pUserData->cbUserStatus==US_LOOKON);

				//状态效验
				ASSERT(bLookonUser==false);
				ASSERT(m_pIUserItem[pUserData->wChairID]==pIServerUserItem);
				if (bLookonUser==true) return false;
				if (pUserData->cbUserStatus>=US_PLAY) return true;

				//设置变量
				pUserData->cbUserStatus=US_READY;

				//同意处理
				if (m_pITableUserAction!=NULL) 
				{
					m_pITableUserAction->OnActionUserReady(pUserData->wChairID,m_pIUserItem[pUserData->wChairID],(VOID *)pDataBuffer,wDataSize);
				}

				//变量定义
				bool bMatchServer=((m_pGameServiceOption->wServerType&GAME_GENRE_MATCH)!=0);
				bool bControlStart=((bMatchServer==true)&&m_pGameServiceOption->cbControlStart==TRUE);

				//开始判断
				if ((bControlStart==false)&&(StartVerdict()==true))
				{
					StartGame();
					return true;
				}

				//发送状态
				m_pIGameServiceFrame->SendUserStatus(pIServerUserItem);

				return true;
			}
		case SUB_GF_USER_CHAT:	//用户聊天
			{
				//效验参数
				CMD_GF_UserChat * pUserChat=(CMD_GF_UserChat *)pDataBuffer;
				ASSERT(wDataSize>=(sizeof(CMD_GF_UserChat)-sizeof(pUserChat->szChatMessage)));
				ASSERT(wDataSize==(sizeof(CMD_GF_UserChat)-sizeof(pUserChat->szChatMessage)+pUserChat->wChatLength));
				if (wDataSize<(sizeof(CMD_GF_UserChat)-sizeof(pUserChat->szChatMessage))) return false;
				if (wDataSize!=(sizeof(CMD_GF_UserChat)-sizeof(pUserChat->szChatMessage)+pUserChat->wChatLength)) return false;

				//变量定义
				bool bMatchServer=((m_pGameServiceOption->wServerType&GAME_GENRE_MATCH)!=0);
				bool bControlStart=((bMatchServer==true)&&m_pGameServiceOption->cbControlStart==TRUE);

				//命令过虑
				const tagServerUserData * pUserData=pIServerUserItem->GetUserData();
				if (pUserData->dwMasterRight!=0L)
				{
					//比赛开始
					if ((bMatchServer==true)&&(bControlStart=true)&&(lstrcmp(pUserChat->szChatMessage,TEXT("/:StartGame"))==0))
					{
						if (StartVerdict()==true) StartGame();
						return true;
					}

					//游戏解散
					if (lstrcmp(pUserChat->szChatMessage,TEXT("/:DismissGame"))==0)
					{
						DismissGame();
						return true;
					}
				}

				//比赛旁观
				if (pUserData->dwMasterRight==0L)
				{
					if ((bMatchServer==true)&&(pIServerUserItem->GetUserStatus()==US_LOOKON))
					{
						LPCTSTR pszMessage=TEXT("为了不影响比赛选手比赛，旁观的用户禁止发言！");
						SendGameMessage(pIServerUserItem,pszMessage,SMT_EJECT|SMT_INFO);
						return true;
					}
				}

				//权限判断
				if (CUserRight::CanGameChat(pIServerUserItem->GetUserData()->dwUserRight)==false)
				{
					LPCTSTR pszMessage=TEXT("您暂时没有发送房间消息的权限，只能与管理员私聊！");
					SendGameMessage(pIServerUserItem,pszMessage,SMT_EJECT|SMT_INFO);
					return true;
				}

				//状态查询
				if ((pUserData->dwMasterRight==0L)&&(m_pIGameServiceFrame->IsAllowGameChat()==false))
				{
					LPCTSTR pszMessage=TEXT("抱歉，本游戏房间不允许发送聊天信息！");
					SendGameMessage(pIServerUserItem,pszMessage,SMT_EJECT|SMT_INFO);
					return true;
				}

				//游戏玩家
				for (WORD i=0;i<m_wChairCount;i++)
				{
					if ((m_pIUserItem[i]!=NULL)&&(IsClientReady(m_pIUserItem[i])==true))
					{
						m_pIGameServiceFrame->SendData(m_pIUserItem[i],MDM_GF_FRAME,SUB_GF_USER_CHAT,pUserChat,wDataSize);
					}
				}

				//旁观玩家
				for (INT_PTR i=0;i<m_LookonUserItemPtr.GetCount();i++)
				{
					if (IsClientReady(m_LookonUserItemPtr[i])==true)
					{
						m_pIGameServiceFrame->SendData(m_LookonUserItemPtr[i],MDM_GF_FRAME,SUB_GF_USER_CHAT,pUserChat,wDataSize);
					}
				}

				return true;
			}
		case SUB_GF_LOOKON_CONTROL:	//旁观控制
			{
				//效验参数
				ASSERT(wDataSize==sizeof(CMD_GF_LookonControl));
				if (wDataSize<sizeof(CMD_GF_LookonControl)) return false;

				//用户效验
				tagServerUserData * pUserData=pIServerUserItem->GetUserData();
				if (pUserData->cbUserStatus==US_LOOKON) return false;

				//变量定义
				CMD_GF_LookonControl * pLookonControl=(CMD_GF_LookonControl *)pDataBuffer;

				//旁观处理
				if (pLookonControl->dwUserID!=0L)
				{
					for (INT_PTR i=0;i<m_LookonUserItemPtr.GetCount();i++)
					{
						//获取用户
						IServerUserItem * pILookonUserItem=m_LookonUserItemPtr[i];
						if (pILookonUserItem->GetUserID()!=pLookonControl->dwUserID) continue;
						if (pILookonUserItem->GetChairID()!=pIServerUserItem->GetChairID()) continue;

						//构造消息
						CMD_GF_LookonControl LookonControl;
						LookonControl.dwUserID=pLookonControl->dwUserID;
						LookonControl.bAllowLookon=pLookonControl->bAllowLookon;

						//发送消息
						if (IsClientReady(pILookonUserItem)==true)
						{
							m_pIGameServiceFrame->SendData(pILookonUserItem,MDM_GF_FRAME,SUB_GF_LOOKON_CONTROL,&LookonControl,sizeof(LookonControl));
						}

						break;
					}
				}
				else
				{
					//设置判断
					bool bAllowLookon=(pLookonControl->bAllowLookon==TRUE)?true:false;
					if (bAllowLookon==m_bAllowLookon[pUserData->wChairID]) return true;
					
					//设置变量
					m_bAllowLookon[pUserData->wChairID]=bAllowLookon;

					//构造消息
					CMD_GF_LookonControl LookonControl;
					LookonControl.dwUserID=pLookonControl->dwUserID;
					LookonControl.bAllowLookon=pLookonControl->bAllowLookon;

					//发送消息
					for (INT_PTR i=0;i<m_LookonUserItemPtr.GetCount();i++)
					{
						//获取用户
						IServerUserItem * pILookonUserItem=m_LookonUserItemPtr[i];
						if (pILookonUserItem->GetChairID()!=pIServerUserItem->GetChairID()) continue;

						//发送消息
						if (IsClientReady(pILookonUserItem)==true)
						{
							m_pIGameServiceFrame->SendData(pILookonUserItem,MDM_GF_FRAME,SUB_GF_LOOKON_CONTROL,&LookonControl,sizeof(LookonControl));
						}
					}
				}

				return true;
			}
		case SUB_GF_KICK_TABLE_USER:	//踢走用户
			{
				//效验参数
				ASSERT(wDataSize==sizeof(CMD_GF_KickTableUser));
				if (wDataSize!=sizeof(CMD_GF_KickTableUser)) return false;                
			
				//效验用户
				tagServerUserData * pUserData=pIServerUserItem->GetUserData();
				if((pUserData->cbMemberOrder==0)&&(pUserData->cbMasterOrder==0)) return false;

				//游戏判断
				if ( pUserData->cbMasterOrder==0 && m_pGameServiceAttrib->wChairCountMax > MAX_CHAIR_NORMAL )
				{
					SendGameMessage(pIServerUserItem, TEXT("对不起，此房间不可以踢人！"), SMT_EJECT|SMT_INFO);
					return true;
				}

				//变量定义
				CMD_GF_KickTableUser * pKickTableUser=(CMD_GF_KickTableUser *)pDataBuffer;

				//查找用户
				IServerUserItem * pIKickUserItem=NULL;
				for (WORD i=0;i<m_wChairCount;i++)
				{
					if ((m_pIUserItem[i]!=NULL)&&(m_pIUserItem[i]->GetUserID()==pKickTableUser->dwUserID))
					{
						pIKickUserItem=m_pIUserItem[i];
						break;
					}
				}
				if (pIKickUserItem==NULL)
				{
					IServerUserItem * pTempUserItem=NULL;
					for (INT_PTR i=0;i<m_LookonUserItemPtr.GetCount();i++)
					{
						pTempUserItem=m_LookonUserItemPtr[i];
						if (pTempUserItem->GetUserID()==pKickTableUser->dwUserID)
						{
							pIKickUserItem=pTempUserItem;
							break;
						}
					}
				}

				//踢走用户
				if (pIKickUserItem!=NULL)
				{	
					//变量定义
					tagServerUserData * pKickUserData=pIKickUserItem->GetUserData();
					tagServerUserData * pActionUserData=pIServerUserItem->GetUserData();

					//状态判断
					if (pKickUserData->cbUserStatus>=US_PLAY) return true;

					//权限判断
					bool bKickUser=false;
					if (bKickUser==false) bKickUser=(pActionUserData->cbMasterOrder>pKickUserData->cbMasterOrder);
					if (m_pGameServiceAttrib->wChairCountMax<=MAX_CHAIR_NORMAL && bKickUser==false)
						bKickUser=(pActionUserData->cbMemberOrder>pKickUserData->cbMemberOrder);
					if ( pActionUserData->cbMasterOrder<pKickUserData->cbMasterOrder ) bKickUser= false; 

					//踢走用户
					if (bKickUser==true)
					{
						//防踢判断
						tagServerUserData *pKickUserData = pIKickUserItem->GetUserData();
						DWORD dwPassTime = pKickUserData->PropertyInfo[PROP_KICK].dwConsumedCount ;
						DWORD dwUseableTime = pKickUserData->PropertyInfo[PROP_KICK].dwPropCount;
						if ( 0 < dwUseableTime && dwPassTime <= dwUseableTime && pUserData->cbMasterOrder==0 )
						{
							TCHAR szDescribe[128]=TEXT("");
							_snprintf(szDescribe,sizeof(szDescribe),TEXT("[ %s ] 使用了防踢卡，踢人失败！"),pKickUserData->szAccounts);
							SendGameMessage(pIServerUserItem,szDescribe,SMT_EJECT|SMT_INFO);

							_snprintf(szDescribe,sizeof(szDescribe),TEXT("由于您使用了防踢卡，成功阻止了[ %s ]对您的踢人动作！"),pUserData->szAccounts);
							SendGameMessage(pIKickUserItem,szDescribe,SMT_EJECT|SMT_INFO);

							return true;
						}

						//机器人判断
						if (pIKickUserItem->IsAndroidUser()==false)
						{
							TCHAR szDescribe[128]=TEXT("");
							_snprintf(szDescribe,sizeof(szDescribe),TEXT("您被 [ %s ] 踢出游戏桌！"),pUserData->szAccounts);
							SendGameMessage(pIKickUserItem,szDescribe,SMT_EJECT|SMT_INFO|SMT_CLOSE_GAME);	
						}
						else PerformStandUpAction(pIKickUserItem);
					}
				}    

				return true;
			}

		case SUB_SEND_SOUND_MESS:
			{
				 BroadcastMessage( MDM_GF_FRAME,  SUB_RECV_SOUND_MESS, pDataBuffer, wDataSize);
				 return true;
			}

		}
	}

	return bSuccess;
}

//视频事件处理
bool CTableFrame::OnEventSocketVideo(WORD wSubCmdID, const void * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	__ENTER_FUNCTION_LOG_CATCH

	switch (wSubCmdID)
	{
	case SUB_GF_C_VIDEO_CONFIG:		//视频配置
		{
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_C_VideoConfig));
			if (wDataSize!=sizeof(CMD_C_VideoConfig)) return false;

			//变量定义
			CMD_C_VideoConfig * pVideoConfig=(CMD_C_VideoConfig *)pDataBuffer;

			//查找用户
			IServerUserItem * pIRemoteUserItem=NULL;
			for (WORD i=0;i<m_wChairCount;i++)
			{
				if ((m_pIUserItem[i]!=NULL)&&(m_pIUserItem[i]->GetUserID()==pVideoConfig->dwRemoteUserID))
				{
					pIRemoteUserItem=m_pIUserItem[i];
					break;
				}
			}

			//消息处理
			if (pIRemoteUserItem!=NULL)
			{
				//变量定义
				CMD_S_VideoConfig VideoConfig;
				ZeroMemory(&VideoConfig,sizeof(VideoConfig));

				//设置变量
				VideoConfig.wNatPort=pVideoConfig->wNatPort;
				VideoConfig.dwNatAddr=pVideoConfig->dwNatAddr;
				VideoConfig.wLocalPort=pVideoConfig->wLocalPort;
				VideoConfig.dwLocalAddr=pVideoConfig->dwLocalAddr;
				VideoConfig.dwLocalUserID=pIServerUserItem->GetUserID();

				//发送消息
				m_pIGameServiceFrame->SendData(pIRemoteUserItem,MDM_GF_VIDEO,SUB_GF_S_VIDEO_CONFIG,&VideoConfig,sizeof(VideoConfig));
			}

			return true;
		}
	case SUB_GF_C_VIDEO_OPEN:		//打开视频
		{
			//合法验证
			WORD wOpenUserChairID=pIServerUserItem->GetChairID();
			if (pIServerUserItem->GetUserStatus()==US_LOOKON) return false;

			//变量定义
			CMD_S_VideoOpen VideoOpen;
			ZeroMemory(&VideoOpen,sizeof(VideoOpen));

			//设置变量
			VideoOpen.wOpenUser=wOpenUserChairID;

			//发送消息
			for (WORD i=0;i<m_wChairCount;i++)
			{
				//获取玩家
				IServerUserItem * pTargetServerUserItem=GetServerUserItem(i);
				if ((pTargetServerUserItem==NULL)||(pIServerUserItem==pTargetServerUserItem)) continue;

				//发送消息
				m_pIGameServiceFrame->SendData(pTargetServerUserItem,MDM_GF_VIDEO,SUB_GF_S_VIDEO_OPEN,&VideoOpen,sizeof(VideoOpen));
			}

			return true;
		}
	case SUB_GF_C_VIDEO_CLOSE:		//关闭视频
		{			
			//合法验证
			WORD wCloseUserChairID=pIServerUserItem->GetChairID();
			if (pIServerUserItem->GetUserStatus()==US_LOOKON) return false;

			//变量定义
			CMD_S_VideoClose VideoClose;
			ZeroMemory( &VideoClose,sizeof(VideoClose));

			//设置变量
			VideoClose.wCloseUser=wCloseUserChairID;

			//发送消息
			for (WORD i=0;i<m_wChairCount;i++)
			{
				//获取玩家
				IServerUserItem * pTargetServerUserItem=GetServerUserItem(i);
				if ((pTargetServerUserItem==NULL)||(pIServerUserItem==pTargetServerUserItem)) continue;

				//发送消息
				m_pIGameServiceFrame->SendData(pTargetServerUserItem,MDM_GF_VIDEO,SUB_GF_S_VIDEO_CLOSE,&VideoClose,sizeof(VideoClose));
			}

			return true;
		}
	}

	__LEAVE_FUNCTION_LOG_CATCH

	return true;
}

//枚举用户
IServerUserItem * __cdecl CTableFrame::EnumLookonUserItem(WORD wIndex)
{
	if (wIndex>=m_LookonUserItemPtr.GetCount()) return NULL;
	return m_LookonUserItemPtr[wIndex];
}

//获取用户
IServerUserItem * __cdecl CTableFrame::GetServerUserItem(WORD wChairID)
{
	ASSERT(wChairID<m_wChairCount);
	if (wChairID>=m_wChairCount) return NULL;
	return m_pIUserItem[wChairID];
}

//设置定时器
bool __cdecl CTableFrame::SetGameTimer(DWORD dwTimerID, DWORD dwElapse, DWORD dwRepeat, WPARAM wBindParam)
{
	//效验参数
	ASSERT(dwTimerID<=IDI_MAX_TIME_ID);
	if (dwTimerID>IDI_MAX_TIME_ID) return false;

	//设置定时器
	return m_pIGameServiceFrame->SetTableTimer(m_wTableID,dwTimerID,dwElapse,dwRepeat,wBindParam);
}

//删除定时器
bool __cdecl CTableFrame::KillGameTimer(DWORD dwTimerID)
{
	//效验参数
	ASSERT(dwTimerID<=IDI_MAX_TIME_ID);
	if (dwTimerID>IDI_MAX_TIME_ID) return false;

	//删除定时器
	return m_pIGameServiceFrame->KillTableTimer(m_wTableID,dwTimerID);
}

//发送数据
bool __cdecl CTableFrame::SendUserData(IServerUserItem * pIServerUserItem, WORD wSubCmdID)
{
	__ENTER_FUNCTION_LOG_CATCH

	//用户判断
	if (IsClientReady(pIServerUserItem)==false) return false;

	//发送消息
	m_pIGameServiceFrame->SendData(pIServerUserItem,MDM_GF_GAME,wSubCmdID);

	__LEAVE_FUNCTION_LOG_CATCH

	return true;
}

//发送数据
bool __cdecl CTableFrame::SendUserData(IServerUserItem * pIServerUserItem, WORD wSubCmdID, void * pData, WORD wDataSize)
{
	__ENTER_FUNCTION_LOG_CATCH

	//用户判断
	if (IsClientReady(pIServerUserItem)==false) return false;

	//发送消息
	m_pIGameServiceFrame->SendData(pIServerUserItem,MDM_GF_GAME,wSubCmdID,pData,wDataSize);

	__LEAVE_FUNCTION_LOG_CATCH

	return true;
}

//发送数据
bool __cdecl CTableFrame::SendTableData(WORD wChairID, WORD wSubCmdID)
{
	__ENTER_FUNCTION_LOG_CATCH

	//群发用户
	if (wChairID==INVALID_CHAIR)
	{
		for (WORD i=0;i<m_wChairCount;i++)
		{
			if ((m_pIUserItem[i]!=NULL)&&(IsClientReady(m_pIUserItem[i])==true))
			{
				m_pIGameServiceFrame->SendData(m_pIUserItem[i],MDM_GF_GAME,wSubCmdID);
			}
		}

		return true;
	}

	//单一发送
	if ((wChairID<m_wChairCount)&&(m_pIUserItem[wChairID]!=NULL))
	{
		if (IsClientReady(m_pIUserItem[wChairID])==true)
		{
			m_pIGameServiceFrame->SendData(m_pIUserItem[wChairID],MDM_GF_GAME,wSubCmdID);
			return true;
		}
	}

	__LEAVE_FUNCTION_LOG_CATCH

	return false;
}

//发送数据
bool __cdecl CTableFrame::SendTableData(WORD wChairID, WORD wSubCmdID, void * pData, WORD wDataSize)
{
	__ENTER_FUNCTION_LOG_CATCH

	//群发用户
	if (wChairID==INVALID_CHAIR)
	{
		for (WORD i=0;i<m_wChairCount;i++)
		{
			if ((m_pIUserItem[i]!=NULL)&&(IsClientReady(m_pIUserItem[i])==true))
			{
				m_pIGameServiceFrame->SendData(m_pIUserItem[i],MDM_GF_GAME,wSubCmdID,pData,wDataSize);
			}
		}

		return true;
	}

	//单一发送
	if ((wChairID<m_wChairCount)&&(m_pIUserItem[wChairID]!=NULL))
	{
		if (IsClientReady(m_pIUserItem[wChairID])==true)
		{
			m_pIGameServiceFrame->SendData(m_pIUserItem[wChairID],MDM_GF_GAME,wSubCmdID,pData,wDataSize);
			return true;
		}
	}

	__LEAVE_FUNCTION_LOG_CATCH

	return false;
}

//发送数据
bool __cdecl CTableFrame::SendLookonData(WORD wChairID, WORD wSubCmdID)
{
	__ENTER_FUNCTION_LOG_CATCH

	for (INT_PTR i=0;i<m_LookonUserItemPtr.GetCount();i++)
	{
		//获取用户
		IServerUserItem * pIServerUserItem=m_LookonUserItemPtr[i];

		//发送消息
		if (IsClientReady(pIServerUserItem)==true)
		{
			//单一发送
			if (wChairID<m_wChairCount)
			{
				tagServerUserData * pUserData=pIServerUserItem->GetUserData();
				if (pUserData->wChairID==wChairID) m_pIGameServiceFrame->SendData(pIServerUserItem,MDM_GF_GAME,wSubCmdID);
			}

			//批量发送
			if (wChairID==INVALID_CHAIR) m_pIGameServiceFrame->SendData(pIServerUserItem,MDM_GF_GAME,wSubCmdID);
		}
	}

	__LEAVE_FUNCTION_LOG_CATCH

	return true;
}

//发送数据
bool __cdecl CTableFrame::SendLookonData(WORD wChairID, WORD wSubCmdID, void * pData, WORD wDataSize)
{
	__ENTER_FUNCTION_LOG_CATCH

	for (INT_PTR i=0;i<m_LookonUserItemPtr.GetCount();i++)
	{
		//获取用户
		IServerUserItem * pIServerUserItem=m_LookonUserItemPtr[i];

		//发送消息
		if (IsClientReady(pIServerUserItem)==true)
		{
			//单一发送
			if (wChairID<m_wChairCount)
			{
				tagServerUserData * pUserData=pIServerUserItem->GetUserData();
				if (pUserData->wChairID==wChairID) m_pIGameServiceFrame->SendData(pIServerUserItem,MDM_GF_GAME,wSubCmdID,pData,wDataSize);
			}

			//批量发送
			if (wChairID==INVALID_CHAIR) m_pIGameServiceFrame->SendData(pIServerUserItem,MDM_GF_GAME,wSubCmdID,pData,wDataSize);
		}
	}

	__LEAVE_FUNCTION_LOG_CATCH


	return true;
}

//发送房间消息
bool __cdecl CTableFrame::SendRoomMessage(IServerUserItem * pIServerUserItem, LPCTSTR lpszMessage, WORD wMessageType)
{
	__ENTER_FUNCTION_LOG_CATCH

	//用户判断
	if (IsClientReady(pIServerUserItem)==false) return false;

	//构造数据包
	CMD_GR_Message Message;
	Message.wMessageType=wMessageType;
	lstrcpyn(Message.szContent,lpszMessage,CountArray(Message.szContent));
	Message.wMessageLength=CountStringBuffer(Message.szContent);

	//发送数据
	WORD wSendSize=sizeof(Message)-sizeof(Message.szContent)+Message.wMessageLength*sizeof(TCHAR);
	m_pIGameServiceFrame->SendData(pIServerUserItem,MDM_GR_SYSTEM,SUB_GR_MESSAGE,&Message,wSendSize);

	__LEAVE_FUNCTION_LOG_CATCH

	return true;
}

//发送游戏消息
bool __cdecl CTableFrame::SendGameMessage(IServerUserItem * pIServerUserItem, LPCTSTR lpszMessage, WORD wMessageType)
{
	__ENTER_FUNCTION_LOG_CATCH

	//用户判断
	if (IsClientReady(pIServerUserItem)==false) return false;

	//构造数据包
	CMD_GF_Message Message;
	Message.wMessageType=wMessageType;
	lstrcpyn(Message.szContent,lpszMessage,CountArray(Message.szContent));
	Message.wMessageLength=CountStringBuffer(Message.szContent);

	//发送数据
	WORD wSendSize=sizeof(Message)-sizeof(Message.szContent)+Message.wMessageLength*sizeof(TCHAR);
	m_pIGameServiceFrame->SendData(pIServerUserItem,MDM_GF_FRAME,SUB_GF_MESSAGE,&Message,wSendSize);

	__LEAVE_FUNCTION_LOG_CATCH

	return true;
}

//解散游戏
bool __cdecl CTableFrame::DismissGame()
{
	__ENTER_FUNCTION_LOG_CATCH

	//状态判断
	ASSERT(m_bGameStarted==true);
	if (m_bGameStarted==false) return false;

	//结束游戏
	if ((m_bGameStarted==true)&&(m_pITableFrameSink->OnEventGameEnd(INVALID_CHAIR,NULL,GER_DISMISS)==false))
	{
		ASSERT(FALSE);
		return false;
	}

	//设置状态
	if (m_bGameStarted==false)
	{
		//设置变量
		m_bGameStarted=false;

		//发送状态
		m_pIGameServiceFrame->SendTableStatus(m_wTableID);
	}

	__LEAVE_FUNCTION_LOG_CATCH

	return true;
}

//结束游戏
bool __cdecl CTableFrame::ConcludeGame()
{
	__ENTER_FUNCTION_LOG_CATCH

    //彭文添加
	for (WORD i=0;i<m_wChairCount;i++)
	{
		if (m_pIUserItem[i]!=NULL)
		{
			m_pIGameServiceFrame->UpdateUserItem(m_pIUserItem[i]);
		}
	}
	//添加结束

	//设置状态
	m_endTime     = (DWORD)time(NULL);
	m_bGameStarted=false;
	m_bGameStatus=GS_FREE;


	//设置变量
	m_dwTimeStart=0L;
	ZeroMemory(m_wOffLineCount,sizeof(m_wOffLineCount));

	//分数校验
	if (GAME_GENRE_GOLD==m_pGameServiceOption->wServerType)
	{
		bool bUserFlee=false;
		LONGLONG lScoreRectify=0;
		for (WORD wIndex=0; wIndex<CountArray(m_ScoreInfo); ++wIndex) 
		{
			lScoreRectify+=(m_ScoreInfo[wIndex].lRevenue+m_ScoreInfo[wIndex].lScore);

			if (enScoreKind_Flee==m_ScoreInfo[wIndex].ScoreKind) bUserFlee=true;

			if ((m_OriginGold[wIndex]+m_ScoreInfo[wIndex].lScore)<0L)
			{
				TCHAR szError[128];
				_sntprintf(szError,CountArray(szError),TEXT("KindName:%s,ServerName:%s,OriginScore:%I64d,ScoreInfo:%I64d"),
					m_pGameServiceAttrib->szKindName,m_pGameServiceOption->szGameRoomName,m_OriginGold[wIndex],m_ScoreInfo[wIndex].lScore);
				WriteErrorRecord(szError);
			}
		}

		if (0!=lScoreRectify)
		{
			TCHAR szError[128];
			_sntprintf(szError,CountArray(szError),TEXT("KindName:%s,ServerName:%s,ErrorCount:%I64d,Flee:%s"),
				m_pGameServiceAttrib->szKindName,m_pGameServiceOption->szGameRoomName,lScoreRectify,bUserFlee?TEXT("YES"):TEXT("NO"));
			WriteErrorRecord(szError);
		}
	}

	//分数信息
	m_lGameTaxScore=0L;
	ZeroMemory(m_ScoreInfo,sizeof(m_ScoreInfo));
	ZeroMemory(m_dwPlayerID,sizeof(m_dwPlayerID));

	//重置桌子
	m_pITableFrameSink->RepositTableFrameSink();

	//设置用户
	for (WORD i=0;i<m_wChairCount;i++)
	{
		if (m_pIUserItem[i]!=NULL)
		{
			tagServerUserData * pUserData=m_pIUserItem[i]->GetUserData();
			if (pUserData->cbUserStatus!=US_OFFLINE) pUserData->cbUserStatus=US_SIT;
		}
	}

	//断线处理
	for (WORD i=0;i<m_wChairCount;i++)
	{
		if (m_pIUserItem[i]!=NULL)
		{
			//获取用户
			IServerUserItem * pIServerUserItem=m_pIUserItem[i];
			if (pIServerUserItem->GetUserStatus()!=US_OFFLINE) continue;
			
			//删除定时器
			DWORD dwTimerID=IDI_OFFLINE+i;
			m_pIGameServiceFrame->KillTableTimer(m_wTableID,dwTimerID);

			//用户离开
			PerformStandUpAction(pIServerUserItem);

			//清理用户
			if (pIServerUserItem->IsActive()==true)
			{
				m_pIGameServiceFrame->DeleteUserItem(pIServerUserItem);
			}
		}
	}

	//赛局判断
	if ((m_pGameServiceOption->wServerType&GAME_GENRE_MATCH)&&(m_pGameServiceOption->lMatchDraw>0))
	{
		for (WORD i=0;i<m_wChairCount;i++)
		{
			if (m_pIUserItem[i]!=NULL)
			{
				//变量定义
				IServerUserItem * pIServerUserItem=m_pIUserItem[i];
				tagServerUserData * pUserData=pIServerUserItem->GetUserData();
				LONG lPlayCount=pUserData->UserScoreInfo.lWinCount+pUserData->UserScoreInfo.lLostCount+pUserData->UserScoreInfo.lDrawCount;

				//比赛判断
				if (lPlayCount>=m_pGameServiceOption->lMatchDraw)
				{
					//发送消息
					TCHAR szDescribe[256]=TEXT("");
					lstrcpyn(szDescribe,TEXT("恭喜您，您的比赛局数已经完成了，不需要再继续比赛，请耐心等待赛果公布！ "),CountArray(szDescribe));
					SendGameMessage(pIServerUserItem,szDescribe,SMT_EJECT|SMT_INFO|SMT_CLOSE_GAME);
					//用户起立
					PerformStandUpAction(pIServerUserItem);
				}
			}
		}
	}

	//检测积分//修改
	/*LONGLONG lMaxScore=m_pGameServiceOption->lMaxScore;
	LONGLONG lLessScore=m_pGameServiceOption->lLessScore;
	if (lLessScore!=0L || lMaxScore>lLessScore)
	{*/
		for (WORD i=0;i<m_wChairCount;i++)
		{
			if (m_pIUserItem[i]!=NULL)
			{
				//获取用户
				IServerUserItem * pIServerUserItem=m_pIUserItem[i];
				tagServerUserData * pUserData=pIServerUserItem->GetUserData();

				//最少判断
				if (pUserData->UserScoreInfo.lGold < m_pGameServiceOption->lRestrictScore)
				{
					//发送信息
					TCHAR szDescribe[256]=TEXT("");
					if (m_pGameServiceOption->wServerType==GAME_GENRE_GOLD)
					{
						_snprintf(szDescribe,sizeof(szDescribe),TEXT("%s的金币少于 %i，不能继续游戏！"), pUserData->szAccounts, m_pGameServiceOption->lEnterMoney);
					}
					SendGameMessage(pIServerUserItem,szDescribe,SMT_EJECT|SMT_INFO|SMT_CLOSE_GAME);
					m_pIGameServiceFrame->SendData(pIServerUserItem,MDM_GR_USER, SUB_GR_GO_HALL);
					
					//弹起用户
					PerformStandUpAction(pIServerUserItem);
				}
			}
		}
	//}

	//关闭判断
	if ((m_pIGameServiceFrame->IsAllowEnterGame()==false)||(m_pIGameServiceFrame->IsShallClose()==true))
	{
		for (WORD i=0;i<m_wChairCount;i++)
		{
			if (m_pIUserItem[i]!=NULL)
			{
				//获取用户
				IServerUserItem * pIServerUserItem=m_pIUserItem[i];
				tagServerUserData * pUserData=pIServerUserItem->GetUserData();

				//用户判断
				if (pUserData->dwMasterRight==0L)
				{
					//用户起立
					LPCTSTR pszMessage=TEXT("请注意，游戏房间即将关闭或者不允许玩家进入，请您离开游戏桌子！");
					SendGameMessage(pIServerUserItem,pszMessage,SMT_EJECT|SMT_INFO|SMT_CLOSE_GAME);
					PerformStandUpAction(pIServerUserItem);

					continue;
				}
			}
		}
	}

	//用户状态
	for (WORD i=0;i<m_wChairCount;i++)
	{
		if (m_pIUserItem[i]!=NULL)
		{
			m_pIGameServiceFrame->SendUserStatus(m_pIUserItem[i]);

			// 暂停在线有礼计时
			DWORD dwUserOnlineTime=m_pIUserItem[i]->GetUserData()->dwOnlineTime;
			if (5 > m_pIUserItem[i]->GetUserScore()->dwGrade  &&  
				120 > dwUserOnlineTime/60)
			{
				DWORD	dwMaxTimer[]={600, 1800, 3600, 7200};//15、30、60、120分钟
				int		iSurplus = 0;
				for (int j=0;j<4;j++)
				{
					if (dwUserOnlineTime<dwMaxTimer[j])
					{
						iSurplus=dwMaxTimer[j];
						break;
					}
				}
				CMD_GF_ONLINE onlineContent;
				onlineContent.dwChairID = i;
				onlineContent.bStart = FALSE;
				onlineContent.iToDayOnlineTime = iSurplus-dwUserOnlineTime;
				m_pIGameServiceFrame->SendData(m_pIUserItem[i],MDM_GF_PRESENT,SUB_GF_ONLINE_TIME,&onlineContent,sizeof(CMD_GF_ONLINE));
			}
		}
	}
	//发送状态
	m_pIGameServiceFrame->SendTableStatus(m_wTableID);


	__LEAVE_FUNCTION_LOG_CATCH

	return true;
}

//写入积分
bool __cdecl CTableFrame::WriteUserScore(WORD wChairID, LONGLONG lScore, LONGLONG lRevenue, enScoreKind ScoreKind, LONG lPlayTimeCount)
{
	__ENTER_FUNCTION_LOG_CATCH

	//效验参数
	ASSERT(wChairID<m_wChairCount);
	ASSERT(m_pIUserItem[wChairID]!=NULL);
	if (wChairID>=m_wChairCount) return false;
	if (m_pIUserItem[wChairID]==NULL) return false;

	//写入积分
	WriteUserScore(m_pIUserItem[wChairID],lScore,lRevenue,ScoreKind, lPlayTimeCount);
   
	__LEAVE_FUNCTION_LOG_CATCH

	return true;
}

//写入积分
bool __cdecl CTableFrame::WriteUserScore(IServerUserItem * pIServerUserItem, LONGLONG lScore, LONGLONG lRevenue, enScoreKind ScoreKind, LONG lPlayTimeCount)
{
	__ENTER_FUNCTION_LOG_CATCH

	//效验参数
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//设置变量
	if (pIServerUserItem->GetChairID()<m_pGameServiceAttrib->wChairCountMax)
	{
		m_ScoreInfo[pIServerUserItem->GetChairID()].lScore=lScore;
		m_ScoreInfo[pIServerUserItem->GetChairID()].lRevenue=lRevenue;
		m_ScoreInfo[pIServerUserItem->GetChairID()].ScoreKind=ScoreKind;
	}

	//变量定义
	tagScoreInfo ScoreInfo;
	ZeroMemory(&ScoreInfo,sizeof(ScoreInfo));
	LONGLONG    xp    = 0; 
	if ( lPlayTimeCount == -1 )
	{
		if (m_dwTimeStart!=0L) 
			xp= ((DWORD)time(NULL)-m_dwTimeStart) / 10;
	}else
	{
		xp = lPlayTimeCount / 10;
	}


	//设置变量
	ScoreInfo.lScore=lScore; //这个就是金币
	ScoreInfo.lRevenue=lRevenue;
	ScoreInfo.ScoreKind=ScoreKind;
	ScoreInfo.lXp = xp;

	//游戏税收
	m_lGameTaxScore+=lRevenue;

	DWORD       dwPlayTimeCount=0L;  //计算经验 
	if ( lPlayTimeCount == -1 )
	{
		if (m_dwTimeStart!=0L) 
			dwPlayTimeCount= ((DWORD)time(NULL)-m_dwTimeStart);
	}else
	{
		dwPlayTimeCount = lPlayTimeCount;
	}
	pIServerUserItem->WriteScore(ScoreInfo,DWORD(dwPlayTimeCount));

	//测试使用
	if (dwPlayTimeCount > 999999 || dwPlayTimeCount < 0)
	{
		tagServerUserData * pServerUserData=pIServerUserItem->GetUserData();
		char info[BUFSIZ] = {0};
		sprintf(info, "结算时候 检测发现经验异常, id：%i, 等级:%i, 经验：%i", 
			pServerUserData->dwUserID,
			pServerUserData->UserScoreInfo.dwGrade,
			pServerUserData->UserScoreInfo.lExperience);
		CTraceService::TraceString(info,TraceLevel_Normal);
	}
	

	//发送信息
	m_pIGameServiceFrame->SendUserScore(pIServerUserItem);

	tagServerUserData * pServerUserData  =  pIServerUserItem->GetUserData();

	pServerUserData->dwOnlineTime += dwPlayTimeCount;	//累计在线时长

	//存储积分
	if (m_pGameServiceOption->wServerType==GAME_GENRE_GOLD&&(m_pGameServiceOption->lRestrictScore>0L))
	{
		//还原积分
		
 		pServerUserData->UserScoreInfo.lGold += pServerUserData->lStorageGold;
		pServerUserData->lStorageGold=0L;

		//存储积分
		if (pServerUserData->UserScoreInfo.lGold>m_pGameServiceOption->lRestrictScore)
		{
			LONGLONG lUserScore=pServerUserData->UserScoreInfo.lGold;
			pServerUserData->UserScoreInfo.lGold=m_pGameServiceOption->lRestrictScore;
			pServerUserData->lStorageGold = lUserScore - m_pGameServiceOption->lRestrictScore;
		}
	}

	__LEAVE_FUNCTION_LOG_CATCH

	return true;
}

//发送场景
bool __cdecl CTableFrame::SendGameScene(IServerUserItem * pIServerUserItem, void * pData, WORD wDataSize)
{
	__ENTER_FUNCTION_LOG_CATCH

	//系统消息
	TCHAR szMessage[256]=TEXT("");
	if (m_pGameServiceOption->wServerType==GAME_GENRE_MATCH)
	{
		_snprintf(szMessage,sizeof(szMessage),TEXT("亲爱的用户，欢迎您来到“%s”比赛游戏房间，祝比赛选手赛出水平，取得好的成绩！"),m_pGameServiceAttrib->szKindName);
		SendGameMessage(pIServerUserItem,szMessage,SMT_INFO);
	}
	else
	{
		_snprintf(szMessage,sizeof(szMessage),TEXT("亲爱的用户，欢迎您来到“%s”，欢迎您多提宝贵建议！"),m_pGameServiceAttrib->szKindName);
		SendGameMessage(pIServerUserItem,szMessage,SMT_INFO);
	}

	//安全提示
	if ((pIServerUserItem->GetUserStatus()!=US_LOOKON)&&(m_bAllowLookon[pIServerUserItem->GetChairID()]==true))
	{
		lstrcpyn(szMessage,TEXT("请注意，您设置了允许所有玩家观看您游戏。"),CountArray(szMessage));
		SendGameMessage(pIServerUserItem,szMessage,SMT_INFO);
	}

	//发送场景
	m_pIGameServiceFrame->SendData(pIServerUserItem,MDM_GF_FRAME,SUB_GF_SCENE,pData,wDataSize);

	__LEAVE_FUNCTION_LOG_CATCH

	return true;
}

//开始游戏
bool __cdecl CTableFrame::StartGame()
{
	__ENTER_FUNCTION_LOG_CATCH

	//效验状态
	ASSERT(m_bGameStarted==false);
	if (m_bGameStarted==true) return false;

	//设置变量
	m_bGameStarted=true;
	m_dwTimeStart=(DWORD)time(NULL);
	ZeroMemory(m_wOffLineCount,sizeof(m_wOffLineCount));

	//记录分数
	m_lGameTaxScore=0L;
	ZeroMemory(m_ScoreInfo,sizeof(m_ScoreInfo));
	ZeroMemory(m_dwPlayerID,sizeof(m_dwPlayerID));

	//设置玩家
	tagServerUserData * pUserData=NULL;
	for (WORD i=0;i<m_wChairCount;i++)
	{
		if (m_pIUserItem[i]!=NULL)
		{
			pUserData=m_pIUserItem[i]->GetUserData();
			pUserData->cbUserStatus=US_PLAY;
			m_dwPlayerID[i]=pUserData->dwUserID;

			m_OriginGold[i]=m_pIUserItem[i]->GetUserScore()->lGold;
		}
	}

	//发送状态
	m_pIGameServiceFrame->SendTableStatus(m_wTableID);

	//用户状态
	for (WORD i=0;i<m_wChairCount;i++)
	{
		if (NULL != m_pIUserItem[i]  &&  PERSON==m_pIUserItem[i]->GetUserData()->utUserType)
		{
			m_pIGameServiceFrame->SendUserStatus(m_pIUserItem[i]);
			m_pAttemperEngineSink->SendFindGemsTask(m_pIUserItem[i]); //彭文添加 发送任务列表

			//Author:<cxf>;	DateTime:<2012-01-16> Description:<新手礼包>
			tagServerUserData * UserData=m_pIUserItem[i]->GetUserData();//用户数据
			if ((UserData->UserScoreInfo.lWinCount+UserData->UserScoreInfo.lLostCount+
				UserData->UserScoreInfo.lDrawCount+UserData->UserScoreInfo.lFleeCount)==0)
			{//第一局<开门红>送礼包
				CMD_GF_NEW_GIFT tagNewGift;
				ZeroMemory(&tagNewGift,sizeof(CMD_GF_NEW_GIFT));
				tagNewGift.wGiftType=1;			//礼品类型(1:开门红;2在线有礼;3:升级赠送)
				tagNewGift.iGem=50;				//赠送宝石 

				WriteUserGem(UserData->wChairID, tagNewGift.iGem);//加入宝石仓库

				//写入数据库
				tagNewGift.wUserID=UserData->dwUserID;
				tagNewGift.dwClientIP=m_pIUserItem[i] ->GetClientIP();
				m_pAttemperEngineSink->NewGiftData(&tagNewGift);

				m_pIGameServiceFrame->SendData(m_pIUserItem[i],MDM_GF_PRESENT,SUB_GF_NEW_GIFT,&tagNewGift,sizeof(CMD_GF_NEW_GIFT));
			}


			// 5级以下及120分钟内，游戏界面需要显示在线计时
			DWORD dwUserOnlineTime=m_pIUserItem[i]->GetUserData()->dwOnlineTime;//用户当天在线时长
			if (5 > m_pIUserItem[i]->GetUserScore()->dwGrade  &&  
				120 > dwUserOnlineTime/60)
			{
				DWORD	dwMaxTimer[]={600, 1800, 3600, 7200};//15、30、60、120分钟
				int		iSurplus = 0;
				for (int j=0;j<4;j++)
				{
					if (dwUserOnlineTime<dwMaxTimer[j])
					{
						iSurplus=dwMaxTimer[j];
						break;
					}
				}
				CMD_GF_ONLINE onlineContent;
				onlineContent.dwChairID = i;
				onlineContent.bStart = TRUE;		//是否开是计时<TRUE:开始计时;False:暂停>
				onlineContent.iToDayOnlineTime = iSurplus-dwUserOnlineTime;
				m_pIGameServiceFrame->SendData(m_pIUserItem[i],MDM_GF_PRESENT,SUB_GF_ONLINE_TIME,&onlineContent,sizeof(CMD_GF_ONLINE));
			}
		}
	}

	//通知事件
	m_pITableFrameSink->OnEventGameStart();

	__LEAVE_FUNCTION_LOG_CATCH

	return true;
}

//判断开始
bool __cdecl CTableFrame::StartVerdict()
{
	__ENTER_FUNCTION_LOG_CATCH

	//比赛判断
	if (m_bGameStarted==true) return false;

	//时间模式
	enStartMode StartMode=m_pITableFrameSink->GetGameStartMode();
	if (StartMode==enStartMode_TimeControl) return false;

	//准备人数
	WORD wReadyUserCount=0;
	tagServerUserData * pUserData=NULL;
	for (WORD i=0;i<m_wChairCount;i++) 
	{
		if (m_pIUserItem[i]!=NULL)
		{
			wReadyUserCount++;
			pUserData=m_pIUserItem[i]->GetUserData();
			if (pUserData->cbUserStatus!=US_READY) return false;
		}
	}

	//条件判断
	if (wReadyUserCount>1L)
	{
		if (StartMode==enStartMode_Symmetry)
		{
			if ((wReadyUserCount%2)!=0) return false;
			if (wReadyUserCount==m_wChairCount) return true;
			WORD wHalfCount=m_wChairCount/2;
			for (WORD i=0;i<wHalfCount;i++)
			{
				if ((m_pIUserItem[i]==NULL)&&(m_pIUserItem[i+wHalfCount]!=NULL)) return false;
				if ((m_pIUserItem[i]!=NULL)&&(m_pIUserItem[i+wHalfCount]==NULL)) return false;
			}
			return true;
		}
		if (StartMode==enStartMode_ThreeUser)
		{
			if (wReadyUserCount==3) return true;
		}
		else 
		{
			if (StartMode==enStartMode_FullReady) return (wReadyUserCount==m_wChairCount);
			if (StartMode==enStartMode_AllReady) return true;
		}
	}

	//特殊判断
	if ((wReadyUserCount==1)&&(m_pGameServiceAttrib->wChairCountMax==1)) return true;

	__LEAVE_FUNCTION_LOG_CATCH

	return false;
}

//调换位置
bool __cdecl CTableFrame::SwitchUserChair(WORD wSourceID[], WORD wTargetID[], WORD wSwitchCount)
{
	__ENTER_FUNCTION_LOG_CATCH


	//效验数据
	ASSERT((wSwitchCount>0)&&(wSwitchCount<m_wChairCount));
	if ((wSwitchCount==0)||(wSwitchCount>=m_wChairCount)) return false;

	//交换判断
	for (WORD i=0;i<wSwitchCount;i++)
	{
		WORD j=0;
		//交换判断
		for (j=0;j<wSwitchCount;j++)
		{
			if (wSourceID[i]==wTargetID[j]) break;
		}

		//错误判断
		if (j==wSwitchCount)
		{
			ASSERT(FALSE);
			return false;
		}
	}

	//重复判断
	for (WORD i=0;i<wSwitchCount;i++)
	{
		//数据判断
		ASSERT(wSourceID[i]<m_wChairCount);
		if (wSourceID[i]>=m_wChairCount) return false;

		//存在判断
		ASSERT(m_pIUserItem[wSourceID[i]]!=NULL);
		if (m_pIUserItem[wSourceID[i]]==NULL) return false;

		//重复判断
		for (WORD j=0;j<wSwitchCount;j++)
		{
			if ((i!=j)&&(wSourceID[i]==wSourceID[j]))
			{
				ASSERT(FALSE);
				return false;
			}
		}
	}

	//变量定义
	BYTE cbUserStatus[MAX_CHAIR];
	IServerUserItem	* pIUserItem[MAX_CHAIR];

	//用户起来
	for (WORD i=0;i<wSwitchCount;i++)
	{
		//变量定义
		WORD wSourceChairID=wSourceID[i];

		//保存用户
		pIUserItem[wSourceChairID]=m_pIUserItem[wSourceChairID];
		cbUserStatus[wSourceChairID]=m_pIUserItem[wSourceChairID]->GetUserData()->cbUserStatus;

		//设置变量
		m_pIUserItem[wSourceChairID]=NULL;
		pIUserItem[wSourceChairID]->SetUserStatus(US_FREE,INVALID_TABLE,INVALID_CHAIR);

		//发送状态
		m_pIGameServiceFrame->SendUserStatus(pIUserItem[wSourceChairID]);
	}

	//用户坐下
	for (WORD i=0;i<wSwitchCount;i++)
	{
		//变量定义
		WORD wSourceChairID=wSourceID[i];
		WORD wTargetChairID=wTargetID[i];

		//设置变量
		m_pIUserItem[wTargetChairID]=pIUserItem[wSourceChairID];
		pIUserItem[wSourceChairID]->SetUserStatus(cbUserStatus[wSourceChairID],m_wTableID,wTargetChairID);

		//发送状态
		m_pIGameServiceFrame->SendUserStatus(pIUserItem[wSourceChairID]);
	}

	__LEAVE_FUNCTION_LOG_CATCH

	return true;
}

//获取空位
WORD CTableFrame::GetNullChairID()
{
	__ENTER_FUNCTION_LOG_CATCH

	//椅子搜索
	if (m_pGameServiceAttrib->wChairCountMax>MAX_CHAIR_NORMAL)
	{
		WORD wChairID[MAX_CHAIR]={0};
		WORD wNullChairCount=0;
		for (WORD i=1;i<m_wChairCount;i++)
		{
			if (m_pIUserItem[i]==NULL)
			{
				wChairID[wNullChairCount++]=i;
			}
		}

		if (wNullChairCount>0) return wChairID[rand()%wNullChairCount];
	}
	else
	{
		for (WORD i=0;i<m_wChairCount;i++)
		{
			if (m_pIUserItem[i]==NULL)
			{
				return i;
			}
		}
	}

	__LEAVE_FUNCTION_LOG_CATCH

	return INVALID_CHAIR;
}

//彭文添加
WORD CTableFrame::GetTatolNullChairCount()
{
	WORD count = 0;
	for (WORD i=0;i<m_wChairCount;i++)
	{
		if (m_pIUserItem[i]==NULL)
		{
			++count;
		}
	}
	return count;
}
//彭文添加
bool CTableFrame::HasPlayer()
{
	WORD count = 0;
	for (WORD i=0;i<m_wChairCount;i++)
	{
		if (m_pIUserItem[i]!=NULL)
		{
			return true;	
		}
	}
	return false;
}

//得到坐下的人数
WORD CTableFrame::GetSittingChairCount()
{
	WORD count = 0;
	for (WORD i=0;i<m_wChairCount;i++)
	{
		if (m_pIUserItem[i]!=NULL)
		{
			++count;
		}
	}
	return count;
}

//是否这个桌子都是机器人
bool CTableFrame::OnlyAndroidInDesk()
{
	for (WORD i=0;i<m_wChairCount;i++)
	{
		if (m_pIUserItem[i]!=NULL)
		{
			tagServerUserData * pUserData=m_pIUserItem[i]->GetUserData(); 
			if( pUserData->utUserType == PERSON)
			{
				return false;
			}
		}
	}
	return true;
}

//得到机器人的数目
WORD CTableFrame::GetAndroidCountInDesk()
{
    int count = 0;
	for (WORD i=0;i<m_wChairCount;i++)
	{
		if (m_pIUserItem[i]!=NULL)
		{
			tagServerUserData * pUserData=m_pIUserItem[i]->GetUserData(); 
			if( pUserData->utUserType == ANDROID)
			{
			    ++count; 
			}
		}
	}
	return count;
}

//得到机器人的数目
WORD CTableFrame::GetNormalCountInDesk()
{
	int count = 0;
	for (WORD i=0;i<m_wChairCount;i++)
	{
		if (m_pIUserItem[i]!=NULL)
		{
			tagServerUserData * pUserData=m_pIUserItem[i]->GetUserData(); 
			if( pUserData->utUserType == PERSON)
			{
				++count; 
			}
		}
	}
	return count;
}


//是否完毕
bool CTableFrame::IsClientReady(IServerUserItem * pIServerUserItem)
{
	__ENTER_FUNCTION_LOG_CATCH

	//用户判断
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//用户判断
	DWORD dwUserIDMap=0L;
	DWORD dwUserID=pIServerUserItem->GetUserID();
	if (m_ClientReadyUser.Lookup(dwUserID,dwUserIDMap)==FALSE) return false;

	__LEAVE_FUNCTION_LOG_CATCH

	return true;
}

//发送坐下失败
void CTableFrame::SendSitFailedPacket(IServerUserItem * pIServerUserItem, LPCTSTR pszFailedDescribe)
{
	__ENTER_FUNCTION_LOG_CATCH

	//构造数据
	CMD_GR_SitFailed SitFailed;
	ZeroMemory(&SitFailed,sizeof(SitFailed));
	lstrcpyn(SitFailed.szFailedDescribe,pszFailedDescribe,CountArray(SitFailed.szFailedDescribe));

	//发送数据
	WORD wSendSize=sizeof(SitFailed)-sizeof(SitFailed.szFailedDescribe)+CountStringBuffer(SitFailed.szFailedDescribe);
	m_pIGameServiceFrame->SendData(pIServerUserItem,MDM_GR_USER,SUB_GR_SIT_FAILED,&SitFailed,wSendSize);

	__LEAVE_FUNCTION_LOG_CATCH

	return;
}

//写入记录
void CTableFrame::WriteErrorRecord(LPCTSTR pszErrorMsg)
{
	try
	{
		//设置文件名
		TCHAR szPath[MAX_PATH]=TEXT("");
		TCHAR szFileName[MAX_PATH];
		GetCurrentDirectory(sizeof(szPath),szPath);
		_snprintf(szFileName,sizeof(szFileName),TEXT("%s\\游戏算分校验.txt"),szPath);

		//打开文件
		CFile FileRecord;
		if (FileRecord.Open(szFileName,CFile::modeCreate|CFile::modeNoTruncate|CFile::modeWrite))
		{
			FileRecord.SeekToEnd();
			FileRecord.Write(pszErrorMsg,lstrlen(pszErrorMsg));
			FileRecord.Write(TEXT("\r\n"),lstrlen(TEXT("\r\n")));

			FileRecord.Close();
		}
	}
	catch(...){}
}


//彭文添加， 处理任务
//完成当前的一个任务
bool __cdecl CTableFrame::QuestTaskFinish(WORD wChairID, DWORD dwGems)
{
	__ENTER_FUNCTION_LOG_CATCH

	ASSERT(wChairID<m_wChairCount);
	ASSERT(m_pIUserItem[wChairID]!=NULL);
	if (wChairID>=m_wChairCount) return false;
	if (m_pIUserItem[wChairID]==NULL) return false;

    tagServerUserData * pUserData=m_pIUserItem[wChairID]->GetUserData(); 
	CMD_GF_TASK_tagTaskFinish  taskFinish;
	taskFinish.dwUserID    = pUserData->dwUserID;
	taskFinish.dwGems      = dwGems;
	taskFinish.dwKindID	   = m_pGameServiceOption->wKindID;
	taskFinish.iTaskID     = pUserData->task.iTaskID;
	taskFinish.iTaskUserLogID = pUserData->task.iTaskUserLogID;
	
	if (dwGems <= 0 )
	{
		taskFinish.bFinish = false;
	}else
	{
		taskFinish.bFinish = true;
	}
	
	memcpy(taskFinish.strUserLogTable, pUserData->task.strUserLogTable, sizeof(taskFinish.strUserLogTable));
    m_pAttemperEngineSink->PerformTaskFinsh( taskFinish);
	WriteUserGem(wChairID, dwGems);
	
	__LEAVE_FUNCTION_LOG_CATCH

	return true;
}



//写入金币
bool __cdecl CTableFrame::WriteUserGem(WORD  wChairID, DWORD lGem)
{
	__ENTER_FUNCTION_LOG_CATCH

	ASSERT(wChairID<m_wChairCount);
	ASSERT(m_pIUserItem[wChairID]!=NULL);
	if (wChairID>=m_wChairCount) return false;
	if (m_pIUserItem[wChairID]==NULL) return false;

	m_pIUserItem[wChairID]-> WriteUserGem(lGem); 

	__LEAVE_FUNCTION_LOG_CATCH

	return true;
}








//void CTableFrame::SendUserAction(void* pData, DWORD wDataSize)
//{
//	//游戏玩家
//	for (WORD i=0;i<m_wChairCount;i++)
//	{
//		if (m_pIUserItem[i]!=NULL)
//		{
//			m_pIGameServiceFrame->SendData(m_pIUserItem[i], MDM_GR_USER,  SUB_RECVACTIONINFO, pData, wDataSize);
//		}
//	}
//
//	//旁观玩家
//	for (INT_PTR i=0;i<m_LookonUserItemPtr.GetCount();i++)
//	{
//		m_pIGameServiceFrame->SendData(m_LookonUserItemPtr[i], MDM_GR_USER, SUB_RECVACTIONINFO, pData, wDataSize);
//	}
//}

//void CTableFrame::SendUserSoundInfo(const void* pData, DWORD wDataSize)
//{
//	//游戏玩家
//	for (WORD i=0;i<m_wChairCount;i++)
//	{
//		if (m_pIUserItem[i]!=NULL)
//		{
//			m_pIGameServiceFrame->SendData(m_pIUserItem[i], MDM_GF_FRAME,  SUB_RECV_SOUND_MESS, (void*)pData, wDataSize);
//		}
//	}
//
//	//旁观玩家
//	for (INT_PTR i=0;i<m_LookonUserItemPtr.GetCount();i++)
//	{
//		m_pIGameServiceFrame->SendData(m_LookonUserItemPtr[i], MDM_GF_FRAME, SUB_RECV_SOUND_MESS, (void*)pData, wDataSize);
//	}
//}


//void CTableFrame::SendUserTaskFinish(const void* pData, DWORD wDataSize)
//{
//	//游戏玩家
//	for (WORD i=0;i<m_wChairCount;i++)
//	{
//		if (m_pIUserItem[i]!=NULL)
//		{
//			m_pIGameServiceFrame->SendData(m_pIUserItem[i], MDM_GF_TASK,  SUB_GF_FIND_GEMS_TASK_ONCEEND, (void*)pData, wDataSize);
//		}
//	}
//
//	//旁观玩家
//	for (INT_PTR i=0;i<m_LookonUserItemPtr.GetCount();i++)
//	{
//		m_pIGameServiceFrame->SendData(m_LookonUserItemPtr[i], MDM_GF_TASK,  SUB_GF_FIND_GEMS_TASK_ONCEEND, (void*)pData, wDataSize);
//	}
//}

//广播消息
void CTableFrame::BroadcastMessage(WORD wMainCmdID, WORD wSubCmdID, const void* pData, WORD wDataSize)
{
	__ENTER_FUNCTION_LOG_CATCH

	//游戏玩家
	for (WORD i=0;i<m_wChairCount;i++)
	{
		if (m_pIUserItem[i]!=NULL)
		{
			m_pIGameServiceFrame->SendData(m_pIUserItem[i], wMainCmdID,  wSubCmdID, (void*)pData, wDataSize);
		}
	}

	//旁观玩家
	for (INT_PTR i=0;i<m_LookonUserItemPtr.GetCount();i++)
	{
		m_pIGameServiceFrame->SendData(m_LookonUserItemPtr[i], wMainCmdID,  wSubCmdID, (void*)pData, wDataSize);
	}

	__LEAVE_FUNCTION_LOG_CATCH

}

//解除只有机器人的桌子
void CTableFrame::CancelOnlyAndroidDesk()
{
	__ENTER_FUNCTION_LOG_CATCH

	//游戏玩家
	for (WORD i=0;i<m_wChairCount;i++)
	{
		IServerUserItem* pItem = m_pIUserItem[i];
		if (pItem != NULL )
		{
			tagServerUserData *pUserData = pItem->GetUserData();
			if (pUserData->utUserType == ANDROID)
			{
				PerformStandUpAction(pItem);
				m_pIGameServiceFrame->InsertUserInQueue(pItem);
			}
		}
	}

	__LEAVE_FUNCTION_LOG_CATCH

}


//解除固定数目的机器人
void CTableFrame::CancelAndroid(int num)
{
	__ENTER_FUNCTION_LOG_CATCH

	WORD temp = (WORD)num;
	if (temp > GetAndroidCountInDesk())
	{	
	    temp = GetAndroidCountInDesk();
	}

    WORD i     = 0;
	WORD count = 0;
	for (; count<temp && i<m_wChairCount; ++i)
	{
		IServerUserItem* pItem = m_pIUserItem[i];
		if (pItem != NULL )
		{
			tagServerUserData *pUserData = pItem->GetUserData();
			if (pUserData->utUserType == ANDROID)
			{
				++count;
				PerformStandUpAction(pItem);
				m_pIGameServiceFrame->InsertUserInQueue(pItem);
			}
		}
	}

	__LEAVE_FUNCTION_LOG_CATCH

}
//////////////////////////////////////////////////////////////////////////
