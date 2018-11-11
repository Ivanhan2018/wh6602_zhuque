#include "Stdafx.h"
#include "GameOption.h"
#include "GameClientDlg.h"
#include <fstream>

#include "TraceCatch.h"

using namespace std;

//////////////////////////////////////////////////////////////////////////

//std::ofstream out("client-info.txt", std::ios::trunc); 
//
//static void WriteInfo( char* p)
//{
//	out << p << std::endl;
//}
//
//void PrintArray(const BYTE* byData, int iNums)
//{
//	char chData[256]={0};
//	for (int i=0; i<iNums; i++)
//	{
//		sprintf(chData, "%s-%d-", chData, byData[i]);
//	}
//	WriteInfo(chData);
//}

//////////////////////////////////////////////////////////////////////////
//宏定义

//游戏定时器
#define IDI_OUT_CARD					200								//出牌定时器
#define IDI_MOST_CARD					201								//最大定时器
#define IDI_START_GAME					202								//开始定时器
#define IDI_LAND_SCORE					203								//叫地主定时器
#define IDI_SNATCH_LAND                 204                             //抢地主定时器
#define IDI_DISPATCH_CARD				205							    //发牌定时器
#define IDI_DOUBLE_SCORE				206								//加倍定时器
#define IDI_ENDSTART_GAME               301                             //游戏结束开始定时器

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CGameClientDlg, CGameFrameDlg)
	ON_WM_TIMER()
	ON_MESSAGE(IDM_START,OnStart)
	ON_MESSAGE(IDM_OUT_CARD,OnOutCard)
	ON_MESSAGE(IDM_PASS_CARD,OnPassCard)
	ON_MESSAGE(IDM_CALL_LAND,OnCallLand)
	ON_MESSAGE(IDM_SNATCH_LAND,OnSnatchLand)
	ON_MESSAGE(IDM_AUTO_OUTCARD,OnAutoOutCard)
	ON_MESSAGE(IDM_LEFT_HIT_CARD,OnLeftHitCard)
	ON_MESSAGE(IDM_RIGHT_HIT_CARD,OnRightHitCard)
	ON_MESSAGE(IDM_LAND_AUTOPLAY,OnAutoPlay)
	ON_MESSAGE(IDM_SORT_HAND_CARD,OnMessageSortCard)
	ON_MESSAGE(IDM_BRIGHT_START,OnBrightStart)
	ON_MESSAGE(IDM_DOUBLE_SCORE, OnDoubleScore)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CGameClientDlg::CGameClientDlg() : CGameFrameDlg(&m_GameClientView, &m_ScoreView)
{	
	__ENTER_FUNCTION

	
	//游戏变量
	m_wBombTime=1;
	memset(m_bHandCardData,0,sizeof(m_bHandCardData));
	memset(m_bHandCardCount,0,sizeof(m_bHandCardCount));
	m_wLandUser=INVALID_CHAIR;
	memset(m_bCardCount,0,sizeof(m_bCardCount));
	memset(m_bHandCardData,0,sizeof(m_bHandCardData));
	m_bAutoPlay = false ;
	m_cbSortType=ST_COUNT;

	//配置变量
	m_dwCardHSpace=DEFAULT_PELS;

	//出牌变量
	m_bTurnCardCount=0;
	m_bTurnOutType=CT_ERROR;
	memset(m_bTurnCardData,0,sizeof(m_bTurnCardData));

	//辅助变量
	m_wTimeOutCount=0;
	m_wMostUser=INVALID_CHAIR;
	m_wCurrentUser=INVALID_CHAIR;
	m_bSystemTrustee=false;
	m_wLastMostUser=INVALID_CHAIR;
    
	//辅助变量
	m_cbRemnantCardCount=0;
	m_cbDispatchCardCount=0;
	ZeroMemory(m_cbDispatchCardData,sizeof(m_cbDispatchCardData));

	//疯狂变量
	m_cbCallScorePhase = CSD_NORMAL;
	m_sVernacular="GY_";//方言

	//提示牌变量
	ZeroMemory(&m_tagNewCard,sizeof(m_tagNewCard));
	ZeroMemory(&m_tagOldCard,sizeof(m_tagOldCard));

	__LEAVE_FUNCTION
}

//析构函数
CGameClientDlg::~CGameClientDlg()
{
	__ENTER_FUNCTION
	__LEAVE_FUNCTION
}

//初始函数
bool CGameClientDlg::InitGameFrame()
{
	__ENTER_FUNCTION

	//设置标题
	//SetWindowText(TEXT("斗地主游戏  --  Ver：6.0.1.0"));	
	//m_sVernacular.Format("%s_",m_strLanguage.c_str()); 

	//设置图标
	HICON hIcon=LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME));
	SetIcon(hIcon,TRUE);
	SetIcon(hIcon,FALSE);

	//读取配置
	//m_dwCardHSpace=AfxGetApp()->GetProfileInt(TEXT("GameOption"),TEXT("CardSpace"),DEFAULT_PELS);
	m_bDeasilOrder=AfxGetApp()->GetProfileInt(TEXT("GameOption"),TEXT("DeasilOrder"),FALSE)?true:false;
	m_bAllowLookon = AfxGetApp()->GetProfileInt(TEXT("GameOption"),TEXT("AllowLookon"),FALSE)?true:false;

	//调整参数
	if ((m_dwCardHSpace>MAX_PELS)||(m_dwCardHSpace<LESS_PELS)) m_dwCardHSpace=DEFAULT_PELS;

	//配置控件
	//m_GameClientView.m_HandCardControl[1].SetCardSpace(17.5,0,20);

	//加载资源
	//m_GameClientView.m_btCardDao.ShowWindow(SW_SHOW);
	SetChatShowTag(true, true);
    m_GameClientView.SetCardBackDisPlay(false);
	m_GameClientView.SetCardBackData(NULL,0);
	//ShowScoreView(true);
	SetTaskWndShow(false);
    //播放游戏背景音乐
    PlayMusic("BACKGROUND_MUSIC.mp3");

	//初始化动画窗口
	m_GameClientView.InitializeAction();

    __LEAVE_FUNCTION
	return true;	
}

//重置框架
void CGameClientDlg::ResetGameFrame()
{
	__ENTER_FUNCTION
		
		
	//游戏变量
	m_wBombTime=1;
	memset(m_bHandCardData,0,sizeof(m_bHandCardData));
	memset(m_bHandCardCount,0,sizeof(m_bHandCardCount));
	m_wLandUser=INVALID_CHAIR;
	memset(m_bCardCount,0,sizeof(m_bCardCount));
	memset(m_bHandCardData,0,sizeof(m_bHandCardData));
	m_bAutoPlay = false ;
	m_cbSortType=ST_COUNT;

	//出牌变量
	m_bTurnCardCount=0;
	m_bTurnOutType=CT_ERROR;
	memset(m_bTurnCardData,0,sizeof(m_bTurnCardData));

	//辅助变量
	m_wTimeOutCount=0;
	m_wMostUser=INVALID_CHAIR;
	m_wCurrentUser=INVALID_CHAIR;
	m_bSystemTrustee=false;
    m_wLastMostUser=INVALID_CHAIR;
	//辅助变量
	m_cbRemnantCardCount=0;
	m_cbDispatchCardCount=0;
	ZeroMemory(m_cbDispatchCardData,sizeof(m_cbDispatchCardData));

	//疯狂变量
	m_cbCallScorePhase = CSD_NORMAL;

	//春天和反春
	m_IsFanSpring=false;
	m_IsSpring=false;
	memset(m_LargessCount,0,sizeof(m_LargessCount));
    m_bchuntian=false;
    m_bfanchun=false;
    m_bbaoshi=false;
    m_bjieshu=false;

	//删除定时
	KillGameTimer(0);
	KillTimer(IDI_MOST_CARD);
	KillTimer(IDI_DISPATCH_CARD);

	__LEAVE_FUNCTION
}

//游戏设置(老大厅的东西)9-23
void CGameClientDlg::OnGameOptionSet()
{
	//构造数据
	CGameOption GameOption;
//	GameOption.m_dwCardHSpace=m_dwCardHSpace;
	GameOption.m_bEnableSound=IsEnableSound();
	//GameOption.m_bDeasilOrder=m_GameClientView.IsDeasilOrder();
	GameOption.m_bAllowLookOn=IsAllowUserLookon();

	//配置数据
	if (GameOption.DoModal()==IDOK)
	{
		//获取参数
//		m_bDeasilOrder=GameOption.m_bDeasilOrder;
//		m_dwCardHSpace=GameOption.m_dwCardHSpace;

		//设置控件
		EnableSound(GameOption.m_bEnableSound);
//		m_GameClientView.SetUserOrder(GameOption.m_bDeasilOrder);
		//m_GameClientView.m_HandCardControl[1].SetCardSpace(m_dwCardHSpace+50,0,20);
		AllowUserLookon(0,GameOption.m_bAllowLookOn);

		//保存配置
		AfxGetApp()->WriteProfileInt(TEXT("GameOption"),TEXT("CardSpace"),m_dwCardHSpace);
		AfxGetApp()->WriteProfileInt(TEXT("GameOption"),TEXT("DeasilOrder"),m_bDeasilOrder?TRUE:FALSE);
		AfxGetApp()->WriteProfileInt(TEXT("GameOption"),TEXT("AllowLookon"),m_bDeasilOrder?TRUE:FALSE);
		AfxGetApp()->WriteProfileInt(TEXT("GameOption"),TEXT("CardSpace"),m_dwCardHSpace);
	}
	return;
}

//时间消息
bool CGameClientDlg::OnTimerMessage(WORD wChairID, UINT nElapse,UINT nTimerID )
{
	__ENTER_FUNCTION
		
	switch (nTimerID)
	{
        case IDI_OUT_CARD:			//用户出牌
		{
			WORD wMeChairID=GetMeChairID();
			//自动处理 
			if ((((nElapse==0)||((m_bAutoPlay==true)&&(nElapse<15))))&&(wChairID==wMeChairID))
			{
				//超时判断
				if ((IsLookonMode()==false)&&(m_bAutoPlay==false)&&((++m_wTimeOutCount)>=3)) 
				{
					//状态切换
					m_wTimeOutCount=0;
					m_bSystemTrustee=true;
					
					//托管功能
					SafeShowOrHideWnd(&m_GameClientView.m_btAutoPlayOn, SW_HIDE);
					SafeShowOrHideWnd(&m_GameClientView.m_btAutoPlayOff, SW_SHOW);
					InsertSystemString( TEXT( "由于您多次超时，切换为“系统托管”模式" ) );
					m_bAutoPlay = true;
			
					CMD_C_UserTrustee UserTrustee;
					ZeroMemory( &UserTrustee, sizeof( UserTrustee ) );
					UserTrustee.bTrustee = true;
					UserTrustee.wUserChairID = wMeChairID;

					SendData( SUB_C_TRUSTEE, &UserTrustee, sizeof( UserTrustee ));
				}
		        if (m_bSystemTrustee==true && m_wLastMostUser !=wChairID)
				{
					OnPassCard(0,0);
				}
				else if(m_wLastMostUser != wChairID && m_bAutoPlay==false)
				{
					OnPassCard(0,0);
				}
				else
				{
					//自动出牌
					OnAutoOutCard( 0, 0 ) ;
					OnOutCard( 0, 0 ) ;
				}
				return false;
			}

			//播放声音
			const tagUserData *pUserData = GetUserData(wChairID);//用户信息
			const tagUserData *pUserData2 = GetUserData((wChairID+1)%3);//用户信息
			if (IsLookonMode() ==false && m_bHandCardCount[wMeChairID]<m_bTurnCardCount) return true;
			if ((nElapse<=5)&&(wChairID==wMeChairID)&&(IsLookonMode()==false))
			{
				PlaySound("GAME_WARN",false);
			}

			return true;
		}
	case IDI_START_GAME:		//开始游戏
		{
			if (nElapse==0)
			{
				if ((IsLookonMode()==false)&&(wChairID==GetMeChairID())) 
				{
					PostMessage(WM_CLOSE,0,0);
				}
				return false;
			}
			if ((nElapse<=5)&&(wChairID==GetMeChairID())&&(IsLookonMode()==false)) 
			{
				PlaySound("GAME_WARN",false);
			}

			return true;
		}
	case IDI_ENDSTART_GAME:         //游戏结束开始定时器
		{
			BYTE byChairID = GetMeChairID();
			if (nElapse==29)
			{
				for (WORD i=0;i<GAME_PLAYER;i++) 
				{
					m_GameClientView.m_UserCardControl[i].SetCardData(NULL,0);
				}
			}
			else if (nElapse==28)
			{
				if (m_IsSpring)
				{//春天
					CString strPath ="";
					GetPicPath(strPath);
					strPath.Append("SPRING_EFFECT.png");
					m_GameClientView.m_SpringAction.SetActionImage(strPath,false,15);
					m_GameClientView.SetSpring();
					PlaySound("TSPRING",false);
					m_GameClientView.UpdateGameView(NULL);
					m_bchuntian=true;
				}
				else if (m_IsFanSpring)
				{//反春
					CString strPath = "";
					GetPicPath(strPath);
					strPath.Append("FANSPRING_EFFECT.png");
					m_GameClientView.m_SpringAction.SetActionImage(strPath,false,15);
					m_GameClientView.SetSpring();
					PlaySound("TSPRING",false);	
					m_GameClientView.UpdateGameView(NULL);
					m_bfanchun=true;
				}
				else if (m_LargessCount[byChairID] >0)
				{
					PlayTaskFinishFlash(true,m_LargessCount[byChairID]);
					m_bbaoshi=true;
				}
				else
				{
					KillGameTimer(IDI_ENDSTART_GAME);
					ShowScoreView(true);
					m_ScoreView.GetWinLose(true); 
					m_ScoreView.SetCountDown(true);
					m_bjieshu=true;
				}		
			}
			else if (nElapse==25)
			{
			    if (m_LargessCount[byChairID] >0 && m_bbaoshi==false)
			     {
				    PlayTaskFinishFlash(true,m_LargessCount[byChairID]);	
			     }
			     else if(m_bjieshu==false)
			       {
					   KillGameTimer(IDI_ENDSTART_GAME);
				      ShowScoreView(true);
				      m_ScoreView.GetWinLose(true);
					  m_ScoreView.SetCountDown(true);
					  m_bjieshu=true;
			       }
			}
			else if (nElapse==23 && m_bjieshu==false)
			{
				KillGameTimer(IDI_ENDSTART_GAME);
				ShowScoreView(true);
				m_ScoreView.GetWinLose(true);
				m_ScoreView.SetCountDown(true);
				m_bjieshu=true;
			}
			return true;
		}
	case IDI_LAND_SCORE:		//叫地主
		{
			WORD wMeChairID=GetMeChairID();
			if ( nElapse==0  )
			{//时间到,默认为不叫地主
				if ((IsLookonMode()==false)&&(wChairID==wMeChairID)) OnCallLand(0,0);
				return false;
			}
			if ((nElapse<=5)&&(wChairID==wMeChairID)&&(IsLookonMode()==false)) 
			{
				PlaySound("GAME_WARN",false);
			}

			return true;
		}
	case IDI_SNATCH_LAND:        //抢地主
		{
			WORD wMeChairID=GetMeChairID();
			if ( nElapse==0  )
			{//时间到,默认为不抢地主
				if ((IsLookonMode()==false)&&(wChairID==wMeChairID)) OnSnatchLand(0,0);
				return false;
			}
			if ((nElapse<=5)&&(wChairID==wMeChairID)&&(IsLookonMode()==false)) 
			{
				PlaySound("GAME_WARN",false);
			}

			return true;
		}
	case IDI_DOUBLE_SCORE:		//加倍定时
		{
			if (nElapse==0 || ( nElapse <= 0 && m_bAutoPlay ))
			{
				if ((IsLookonMode()==false)&&(wChairID==GetMeChairID())) 
				{//时间到，默认为不加倍。
					//发送消息
					SendMessage( IDM_DOUBLE_SCORE, 1, 1 );
				}
				return false;
			}
			if ((nElapse<=5)&&(wChairID==GetMeChairID())&&(IsLookonMode()==false)) PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_WARN"));

			return true;
		}
	}

	__LEAVE_FUNCTION
	return false;
}

//旁观状态
void CGameClientDlg::OnLookonChanged(bool bLookonUser, const void * pBuffer, WORD wDataSize)
{
	__ENTER_FUNCTION
		
	//旁观设置
	//m_GameClientView.m_HandCardControl[1].SetDisplayFlag( ( IsLookonMode() == true ) && ( IsAllowLookon() == true ) );
	//m_GameClientView.m_GameCard.SetCardShow( true );

	__LEAVE_FUNCTION
}

//网络消息
bool CGameClientDlg::OnGameMessage(WORD wSubCmdID, const void * pBuffer, WORD wDataSize)
{
	__ENTER_FUNCTION
		
	switch (wSubCmdID)
	{
	case SUB_S_SEND_CARD:		//发送扑克
		{
			return OnSubSendCard(pBuffer,wDataSize);
		}
	case SUB_S_LAND_SCORE:	//用户叫地主
		{
			return OnSubLandScore(pBuffer,wDataSize);
		}
	case SUB_S_USER_SNATCH_SCORE: //用户抢地主
		{
			return onSubSnatchLand(pBuffer,wDataSize);
		}
	case SUB_S_GAME_START:		//游戏开始
		{
			return OnSubGameStart(pBuffer,wDataSize);
		}
	case SUB_S_OUT_CARD:		//用户出牌
		{
			return OnSubOutCard(pBuffer,wDataSize);
		}
	case SUB_S_PASS_CARD:		//放弃出牌
		{
			return OnSubPassCard(pBuffer,wDataSize);
		}
	case SUB_S_GAME_END:		//游戏结束
		{
			return OnSubGameEnd(pBuffer,wDataSize);
		}
	case SUB_S_BRIGHT_START:	//明牌开始
		{
			return OnSubBrightStart(pBuffer,wDataSize);			
		}
	case SUB_S_BRIGHT_CARD:	//玩家明牌
		{
			return OnSubBrightCard(pBuffer,wDataSize);
		}
	case SUB_C_TRUSTEE:			//玩家托管
		{
			return OnSubPlayerTrustee(pBuffer,wDataSize);
		}
	case SUB_S_DOUBLE_SCORE:	//加倍消息
		{
			return OnSubDoubleScore(pBuffer, wDataSize);
		}
	case SUB_S_USER_DOUBLE:		//玩家加倍
		{
			return OnSubUserDouble(pBuffer, wDataSize);
		}
	}

   __LEAVE_FUNCTION
	return false;		
}

//游戏场景
bool CGameClientDlg::OnGameSceneMessage(BYTE cbGameStation, bool bLookonOther, const void * pBuffer, WORD wDataSize)
{
	__ENTER_FUNCTION
		
	switch (cbGameStation)
	{
	case GS_WK_FREE:	//空闲状态
		{
			//效验数据
			if (wDataSize!=sizeof(CMD_S_StatusFree)) return false;
			CMD_S_StatusFree * pStatusFree=(CMD_S_StatusFree *)pBuffer;

			//设置界面
			//
			m_GameClientView.SetBaseScore(pStatusFree->lBaseScore);
			for ( WORD wChairID = 0; wChairID < GAME_PLAYER; ++wChairID)
			{
				WORD wViewID = SwitchViewChairID(wChairID) ;
				bool bBrightStart = pStatusFree->bBrightStart[wChairID];
				m_GameClientView.SetBrightStart(wViewID, bBrightStart );
				if ( bBrightStart )
				{
					m_GameClientView.SetBrightTime( BRIGHT_START_TIME );
					m_GameClientView.ShowLandTime( true );
				}
			}

			//设置控件
			if (IsLookonMode()==false)
			{
				SafeShowOrHideWnd(&m_GameClientView.m_btStart, SW_SHOW);
				//lq
				//m_GameClientView.m_btBrightStart.ShowWindow(SW_SHOW);
				m_GameClientView.m_btStart.SetFocus();
			}
			//SafeShowOrHideWnd(&m_GameClientView.m_btStart, SW_SHOW);
			//重置数据
			m_GameClientView.SetResetShouPaiNums();
			//设置扑克
			//m_GameClientView.m_HandCardControl[1].SetDisplayFlag((IsLookonMode()==false)||(IsAllowLookon()==true));
			m_GameClientView.m_GameCard.SetCardShow( true );

			//设置时间
			SetGameTimer(GetMeChairID(),IDI_START_GAME,15);

			m_GameClientView.UpdateGameView(NULL);
			return true;
		}
	case GS_WK_SCORE:	//叫分状态
		{
			//效验数据
			if (wDataSize!=sizeof(CMD_S_StatusScore)) return false;
			CMD_S_StatusScore * pStatusScore=(CMD_S_StatusScore *)pBuffer;

			SetTaskWndShow(true);
			//设置变量
			for (WORD i=0;i<GAME_PLAYER;i++) 
			{
				m_bCardCount[i]=17;
				m_bHandCardCount[i]=17;
				CopyMemory(m_bHandCardData[i],pStatusScore->bCardData[i],sizeof(BYTE)*m_bHandCardCount[i]);

				//排序扑克
				m_GameLogic.SortCardList(m_bHandCardData[i], 17, ST_ORDER);
			}
			//设置界面
			for (WORD i=0;i<GAME_PLAYER;i++)	
			{
				WORD wViewChairID=SwitchViewChairID(i);
				if(IsLookonMode()==false)
				{
					m_GameClientView.SetCardCount(wViewChairID,m_bCardCount[i]);
				}
				m_GameClientView.SetLandScore(wViewChairID,pStatusScore->bScoreInfo[i],pStatusScore->bCallScorePhase,pStatusScore->bCallScorePhase);
			}

			m_GameClientView.ShowLandTitle(true);
			m_GameClientView.SetBaseScore(pStatusScore->lBaseScore);
			m_GameClientView.SetBrightTime(pStatusScore->bBrightTime);
			m_GameClientView.ShowLandTime(true);

			//按钮控制
			if ((IsLookonMode()==false)&&(pStatusScore->wCurrentUser==GetMeChairID()))
			{
				/*SafeShowOrHideWnd(&m_GameClientView.m_btGiveUpScore, SW_SHOW);*/
				//m_GameClientView.m_btGiveUpScore.LoadRgnImage(AfxGetInstanceHandle(), IDB_GIVE_UP,RGB(255,0,255));

				if (pStatusScore->bCallScorePhase==CSD_NORMAL)
				{
			/*		SafeShowOrHideWnd(&m_GameClientView.m_btOneScore, SW_SHOW);
					SafeShowOrHideWnd(&m_GameClientView.m_btTwoScore, SW_SHOW);
					SafeShowOrHideWnd(&m_GameClientView.m_btThreeScore, SW_SHOW);

					SafeEnableWnd(&m_GameClientView.m_btOneScore, pStatusScore->bLandScore==0?TRUE:FALSE);
					SafeEnableWnd(&m_GameClientView.m_btTwoScore, pStatusScore->bLandScore<=1?TRUE:FALSE);
					SafeEnableWnd(&m_GameClientView.m_btThreeScore, pStatusScore->bLandScore<=2?TRUE:FALSE);*/
				}
				else if (pStatusScore->bCallScorePhase==CSD_SNATCHLAND)
				{
				/*	m_GameClientView.m_btSnatchLand.ShowWindow(SW_HIDE);*/
					//m_GameClientView.m_btGiveUpScore.LoadRgnImage(AfxGetInstanceHandle(),IDB_NOT_SNATCH_LAND,RGB(255,0,255));
				}
				//lq
				else if (pStatusScore->bCallScorePhase==CSD_BRIGHTCARD)
				{
					//m_GameClientView.m_btBrightCard.ShowWindow(SW_SHOW);
					//m_GameClientView.m_btGiveUpScore.SetButtonImage(IDB_NOT_BRIGHT_CARD,AfxGetInstanceHandle(),false);
				}
				//lq
			}

			//设置扑克
			BYTE bCardData[3]={0,0,0};
			WORD wMeChairID=GetMeChairID();
			m_GameClientView.SetCardBackData(bCardData,3);
			//m_GameClientView.m_HandCardControl[1].SetCardData(m_bHandCardData[wMeChairID],m_bHandCardCount[wMeChairID]);
			//m_GameClientView.m_HandCardControl[1].SetDisplayFlag((IsLookonMode()==false)||(IsAllowLookon()==true));
			m_GameClientView.m_GameCard.SetCardData(m_bHandCardData[wMeChairID],m_bHandCardCount[wMeChairID]);
			
			m_GameClientView.m_GameCard.SetCardShow( true );

			for ( WORD wChairID = 0; wChairID < GAME_PLAYER; ++wChairID )
			{
				m_GameClientView.SetUserTrustee( SwitchViewChairID( wChairID ), pStatusScore->bUserTrustee[ wChairID ] );
				
				//自己判断
				if ( ! IsLookonMode() && wChairID == GetMeChairID() && pStatusScore->bUserTrustee[ wChairID ] )
				{
					//托管功能
					SafeShowOrHideWnd(&m_GameClientView.m_btAutoPlayOn, SW_HIDE);
					SafeShowOrHideWnd(&m_GameClientView.m_btAutoPlayOff, SW_SHOW);
					OnAutoPlay( 1, 1 );
				}
			}

			//设置扑克
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				WORD wViewChairID = SwitchViewChairID(i);
				m_GameClientView.m_GameCard.SetCardData(m_bHandCardData[i],m_bHandCardCount[i]);
			}

			//明牌判断
			//for ( WORD wChairID = 0; wChairID < GAME_PLAYER; ++wChairID )
			//{
			//	WORD wViewChairID = SwitchViewChairID( wChairID );

			//	//显示扑克
			//	if ( pStatusScore->bUserBrightCard[wChairID] )
			//	{
			//		m_GameClientView.m_HandCardControl[wViewChairID].ShowWindow(SW_SHOW);
			//		m_GameClientView.m_HandCardControl[wViewChairID].SetDisplayFlag(true);
			//	}
			//}

			if ( IsLookonMode() )
			{
				SafeShowOrHideWnd(&m_GameClientView.m_btAutoPlayOff, SW_HIDE);
				SafeShowOrHideWnd(&m_GameClientView.m_btAutoPlayOn, SW_HIDE);
				SafeEnableWnd(&m_GameClientView.m_btAutoPlayOff, FALSE);
				SafeEnableWnd(&m_GameClientView.m_btAutoPlayOn, FALSE);
				SafeShowOrHideWnd(&m_GameClientView.m_btSortCard, SW_HIDE);
			}
			else 
			{
				SafeShowOrHideWnd(&m_GameClientView.m_btSortCard, SW_SHOW);
				SafeEnableWnd(&m_GameClientView.m_btSortCard, TRUE);
				SafeShowOrHideWnd(&m_GameClientView.m_btAutoPlayOff, SW_HIDE);
				SafeShowOrHideWnd(&m_GameClientView.m_btAutoPlayOn, SW_SHOW);
				SafeEnableWnd(&m_GameClientView.m_btAutoPlayOff, FALSE);
				SafeEnableWnd(&m_GameClientView.m_btAutoPlayOn, FALSE);
			}
			//旁观玩家
			if (IsLookonMode()==true)
			{
				for (WORD wChairID=0; wChairID<GAME_PLAYER;++wChairID)
				{
					WORD wViewChairID = SwitchViewChairID( wChairID );
					if (1!=wViewChairID)
					{
						m_GameClientView.m_HandCardControl[wViewChairID].ShowWindow(SW_SHOW);
						m_GameClientView.m_HandCardControl[wViewChairID].SetDisplayFlag(true);
						m_GameClientView.m_HandCardControl[wViewChairID].SetCardData(m_bHandCardData[wViewChairID],m_bHandCardCount[wViewChairID]);
					}       
				}
			}
			//设置时间
			if ( pStatusScore->bCallScorePhase == CSD_NORMAL )
				SetGameTimer(pStatusScore->wCurrentUser,IDI_LAND_SCORE,10);
			else
				SetGameTimer(pStatusScore->wCurrentUser,IDI_LAND_SCORE,10);

			return true;
		}
	case GS_WK_DOUBLE_SCORE:	//加倍状态
			{
				//效验数据
				if (wDataSize!=sizeof(CMD_S_StatusDoubleScore)) return false;
				CMD_S_StatusDoubleScore * pStatusDoubleScore=(CMD_S_StatusDoubleScore *)pBuffer;

				SetTaskWndShow(true);
				//cxf 删除加倍按钮  用(倒-跟-反)代替
				//设置按钮
				//if ( ! IsLookonMode() )
				//{
					//bool bAllowDouble = pStatusDoubleScore->bAllowDouble;
					//m_GameClientView.m_btDoubleScore.ShowWindow( SW_SHOW );
					//m_GameClientView.m_btDoubleScore.EnableWindow( bAllowDouble ? TRUE : FALSE );
					//m_GameClientView.m_btNotDoubleScore.ShowWindow( SW_SHOW );
				//}
				//cxf end
				//地主设置
				//m_wLandUser = pStatusDoubleScore->wLandUser;
				m_GameClientView.SetLandUser(SwitchViewChairID(pStatusDoubleScore->wLandUser), pStatusDoubleScore->bLandScore );
				m_GameClientView.ShowLandTime(true);

				//状态设置
				m_GameClientView.SetLandScore(INVALID_CHAIR,0,CSD_GAMESTART,CSD_GAMESTART);

				//基础设置
				m_GameClientView.SetBaseScore(pStatusDoubleScore->lBaseScore);

				//设置扑克
				for (WORD i=0;i<GAME_PLAYER;i++)
				{
					m_bHandCardCount[i]=pStatusDoubleScore->bCardCount[i];
					CopyMemory(m_bHandCardData[i],pStatusDoubleScore->bCardData[i],m_bHandCardCount[i]);
					
					//排序扑克
					m_GameLogic.SortCardList(m_bHandCardData[i], m_bHandCardCount[i], ST_ORDER);

					WORD wViewChairID = SwitchViewChairID(i);
					if (IsLookonMode()==false)
					{
						m_GameClientView.SetCardCount(wViewChairID, m_bHandCardCount[i]);
					}	
				}
				m_GameClientView.SetCardBackData(pStatusDoubleScore->bBackCard,3);
				m_GameClientView.SetCardBackDisPlay(true);
				WORD wMeChairID = GetMeChairID();
				//m_GameClientView.m_HandCardControl[1].SetCardData(m_bHandCardData[wMeChairID],m_bHandCardCount[wMeChairID]);
			
				m_GameClientView.m_GameCard.SetCardData(m_bHandCardData[wMeChairID],m_bHandCardCount[wMeChairID]);
				
				if ( ! IsLookonMode() )
				{
					m_GameClientView.m_GameCard.SetCardShow(true);
					m_GameClientView.m_GameCard.SetCardPositively(true);

					//m_GameClientView.m_HandCardControl[1].SetDisplayFlag(true);
					//m_GameClientView.m_HandCardControl[1].SetPositively(true);
				}
				
				//加倍玩家
				for ( WORD wChairID = 0; wChairID < GAME_PLAYER; wChairID++ )
				{
					WORD wViewChairID = SwitchViewChairID( wChairID );
					m_GameClientView.SetDoubleScoreUser( wViewChairID, pStatusDoubleScore->bDoubleUser[wChairID] );
				}

				//托管设置
				for ( WORD wChairID = 0; wChairID < GAME_PLAYER; ++wChairID )
				{
					m_GameClientView.SetUserTrustee( SwitchViewChairID( wChairID ), pStatusDoubleScore->bUserTrustee[ wChairID ] );
					
					//自己判断
					if ( ! IsLookonMode() && wChairID == GetMeChairID() && pStatusDoubleScore->bUserTrustee[ wChairID ] )
					{
						//托管功能
						SafeShowOrHideWnd(&m_GameClientView.m_btAutoPlayOn, SW_HIDE);
						SafeShowOrHideWnd(&m_GameClientView.m_btAutoPlayOff, SW_SHOW);
						OnAutoPlay( 1, 1 );
					}
				}

				//设置消息
				//if ( ! IsLookonMode() ) SetGameTimer(GetMeChairID(), IDI_DOUBLE_SCORE, 30);

				//设置扑克
				for (WORD i=0;i<GAME_PLAYER;i++)
				{
					WORD wViewChairID = SwitchViewChairID(i);
					//m_GameClientView.m_HandCardControl[wViewChairID].SetCardData(m_bHandCardData[i],m_bHandCardCount[i]);
					m_GameClientView.m_GameCard.SetCardData(m_bHandCardData[i],m_bHandCardCount[i]);
				}

				//明牌判断
				//for ( WORD wChairID = 0; wChairID < GAME_PLAYER; ++wChairID )
				//{
				//	WORD wViewChairID = SwitchViewChairID( wChairID );

				//	//显示扑克
				//	if ( pStatusDoubleScore->bUserBrightCard[wChairID] )
				//	{
				//		m_GameClientView.m_HandCardControl[wViewChairID].ShowWindow(SW_SHOW);
				//		m_GameClientView.m_HandCardControl[wViewChairID].SetDisplayFlag(true);
				//	}
				//}

				if ( IsLookonMode() )
				{
					SafeShowOrHideWnd(&m_GameClientView.m_btAutoPlayOff, SW_HIDE);
					SafeShowOrHideWnd(&m_GameClientView.m_btAutoPlayOn, SW_HIDE);
					SafeEnableWnd(&m_GameClientView.m_btAutoPlayOff, FALSE);
					SafeEnableWnd(&m_GameClientView.m_btAutoPlayOn, FALSE);
					SafeShowOrHideWnd(&m_GameClientView.m_btSortCard, SW_HIDE);
				}
				else 
				{
					SafeShowOrHideWnd(&m_GameClientView.m_btSortCard, SW_SHOW);
					SafeEnableWnd(&m_GameClientView.m_btSortCard, TRUE);
					SafeShowOrHideWnd(&m_GameClientView.m_btAutoPlayOff, SW_HIDE);
					SafeShowOrHideWnd(&m_GameClientView.m_btAutoPlayOn, SW_SHOW);
					SafeEnableWnd(&m_GameClientView.m_btAutoPlayOff, FALSE);
					SafeEnableWnd(&m_GameClientView.m_btAutoPlayOn, FALSE);
				}
				//旁观玩家
				if (IsLookonMode()==true)
				{
					for (WORD wChairID=0; wChairID<GAME_PLAYER;++wChairID)
					{
						WORD wViewChairID = SwitchViewChairID( wChairID );
						if (1!=wViewChairID)
						{
							m_GameClientView.m_HandCardControl[wViewChairID].ShowWindow(SW_SHOW);
							m_GameClientView.m_HandCardControl[wViewChairID].SetDisplayFlag(true);
							m_GameClientView.m_HandCardControl[wViewChairID].SetCardData(m_bHandCardData[wViewChairID],m_bHandCardCount[wViewChairID]);
						}       
					}
				}
				return true;
			}
	case GS_WK_PLAYING:		//游戏状态
		{
			//效验数据
			if (wDataSize!=sizeof(CMD_S_StatusPlay)) return false;
			CMD_S_StatusPlay * pStatusPlay=(CMD_S_StatusPlay *)pBuffer;

			SetTaskWndShow(true);

			//设置变量
			m_bTurnCardCount=pStatusPlay->bTurnCardCount;
			m_bTurnOutType=m_GameLogic.GetCardType(pStatusPlay->bTurnCardData,m_bTurnCardCount);
			CopyMemory(m_bTurnCardData,pStatusPlay->bTurnCardData,pStatusPlay->bTurnCardCount);

			//设置扑克
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				m_bHandCardCount[i]=pStatusPlay->bCardCount[i];
				CopyMemory(m_bHandCardData[i],pStatusPlay->bCardData[i],m_bHandCardCount[i]);

				//排序扑克
				m_GameLogic.SortCardList(m_bHandCardData[i], m_bHandCardCount[i], ST_ORDER);
			}

			//设置界面
			for (BYTE i=0;i<GAME_PLAYER;i++)
			{
				WORD wViewChairID=SwitchViewChairID(i);
				m_bCardCount[i]=pStatusPlay->bCardCount[i];
				if (IsLookonMode()==false)
				{
					m_GameClientView.SetCardCount(wViewChairID,pStatusPlay->bCardCount[i]);
				}			
				m_GameClientView.m_HandCardControl[wViewChairID].SetCardData(m_bHandCardData[i],m_bHandCardCount[i]);
			}

			m_GameClientView.m_cbCallScorePhase = CSD_GAMESTART;
			//for (int k=0; k<GAME_PLAYER; k++)
			//{
			//	//m_GameClientView.SetDoubleScoreUser(k, pStatusPlay->bDoubleUser[k]);
			//}
			m_GameClientView.SetBombTime(pStatusPlay->wBombNums);
			m_GameClientView.SetBaseScore(pStatusPlay->lBaseScore);
			m_GameClientView.SetCardBackData(pStatusPlay->bBackCard,3);
			m_GameClientView.SetCardBackDisPlay(true);
			WORD wMeChairID = GetMeChairID();
			//m_GameClientView.m_HandCardControl[1].SetCardData(m_bHandCardData[wMeChairID],m_bHandCardCount[wMeChairID]);
			m_GameClientView.m_GameCard.SetCardData(m_bHandCardData[wMeChairID],m_bHandCardCount[wMeChairID]);
			m_GameClientView.SetLandUser(SwitchViewChairID(pStatusPlay->wLandUser),pStatusPlay->bLandScore);
			m_GameClientView.SetBrightTime(pStatusPlay->bBrightTime);
			m_GameClientView.ShowLandTime(false);

			//玩家设置
			if ((IsLookonMode()==false)&&(pStatusPlay->wCurrentUser==GetMeChairID()))
			{
				SafeEnableWnd(&m_GameClientView.m_btOutCard, FALSE);
				SafeShowOrHideWnd(&m_GameClientView.m_btOutCard, SW_SHOW);
				SafeShowOrHideWnd(&m_GameClientView.m_btPassCard, SW_SHOW);
				SafeShowOrHideWnd(&m_GameClientView.m_btAutoOutCard, SW_SHOW);
				SafeEnableWnd(&m_GameClientView.m_btPassCard, (m_bTurnCardCount!=0)?TRUE:FALSE);
				SafeEnableWnd(&m_GameClientView.m_btAutoOutCard, TRUE);
			}

			//桌面设置
			if (m_bTurnCardCount!=0)
			{
				WORD wViewChairID=SwitchViewChairID(pStatusPlay->wLastOutUser);
				m_GameClientView.m_UserCardControl[wViewChairID].SetCardData(m_bTurnCardData,m_bTurnCardCount);
			}

			if ( IsLookonMode() )
			{
				SafeShowOrHideWnd(&m_GameClientView.m_btAutoPlayOff, SW_HIDE);
				SafeShowOrHideWnd(&m_GameClientView.m_btAutoPlayOn, SW_HIDE);
				SafeEnableWnd(&m_GameClientView.m_btAutoPlayOff, FALSE);
				SafeEnableWnd(&m_GameClientView.m_btAutoPlayOn, FALSE);
				SafeShowOrHideWnd(&m_GameClientView.m_btSortCard, SW_HIDE);
			}
			else
			{
				SafeShowOrHideWnd(&m_GameClientView.m_btSortCard, SW_SHOW);
				SafeEnableWnd(&m_GameClientView.m_btSortCard, TRUE);
				SafeShowOrHideWnd(&m_GameClientView.m_btAutoPlayOff, SW_HIDE);
				SafeShowOrHideWnd(&m_GameClientView.m_btAutoPlayOn, SW_SHOW);
				SafeEnableWnd(&m_GameClientView.m_btAutoPlayOff, FALSE);
				SafeEnableWnd(&m_GameClientView.m_btAutoPlayOn, FALSE);
			}

			for ( WORD wChairID = 0; wChairID < GAME_PLAYER; ++wChairID )
			{
				m_GameClientView.SetUserTrustee( SwitchViewChairID( wChairID ), pStatusPlay->bUserTrustee[ wChairID ] );
				
				//自己判断
				if ( ! IsLookonMode() && wChairID == GetMeChairID() &&pStatusPlay->bUserTrustee[ wChairID ] )
				{
					//托管功能
					SafeShowOrHideWnd(&m_GameClientView.m_btAutoPlayOn, SW_HIDE);
					SafeShowOrHideWnd(&m_GameClientView.m_btAutoPlayOff, SW_SHOW);
					OnAutoPlay( 1, 1 );
				}
			}

			//设置定时器
			SetGameTimer(pStatusPlay->wCurrentUser,IDI_OUT_CARD,15);

			//设置扑克
			//m_GameClientView.m_HandCardControl[1].SetPositively(IsLookonMode()==false);
			//m_GameClientView.m_HandCardControl[1].SetDisplayFlag((IsLookonMode()==false)||(IsAllowLookon()==true));
	     	m_GameClientView.m_GameCard.SetCardShow(true);
			m_GameClientView.m_GameCard.SetCardPositively(IsLookonMode()==false);
			
			//明牌判断
			for ( WORD wChairID = 0; wChairID < GAME_PLAYER; ++wChairID )
			{
				WORD wViewChairID = SwitchViewChairID( wChairID );

				//显示扑克
				if ( pStatusPlay->bUserBrightCard[wChairID] )
				{
					/*m_GameClientView.m_HandCardControl[wViewChairID].ShowWindow(SW_SHOW);
					m_GameClientView.m_HandCardControl[wViewChairID].SetDisplayFlag(true);*/
				}
				else if ( wViewChairID != 1 ) m_GameClientView.m_HandCardControl[wViewChairID].SetDisplayFlag(false);
			}
			//旁观玩家
			if (IsLookonMode()==true)
			{
				for (WORD wChairID=0; wChairID<GAME_PLAYER;++wChairID)
				{
					WORD wViewChairID = SwitchViewChairID( wChairID );
					if (1!=wViewChairID)
					{
						m_GameClientView.m_HandCardControl[wViewChairID].ShowWindow(SW_SHOW);
						m_GameClientView.m_HandCardControl[wViewChairID].SetDisplayFlag(true);
						m_GameClientView.m_HandCardControl[wViewChairID].SetCardData(m_bHandCardData[wViewChairID],m_bHandCardCount[wViewChairID]);
					}       
				}
			}

			return true;
		}
	}

	__LEAVE_FUNCTION
	return false;
}

//发送扑克
bool CGameClientDlg::OnSubSendCard(const void * pBuffer, WORD wDataSize)
{
	__ENTER_FUNCTION
			
	PlayWaitFlash(false);

	//发牌前不显示等待叫分字样

	KillGameTimer(IDI_LAND_SCORE);
	m_GameClientView.ShowLandTitle(false);
    //发牌前对家的牌不显示
	m_GameClientView.SetCardCount(INVALID_CHAIR,0);
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_SendAllCard));
	if (wDataSize!=sizeof(CMD_S_SendAllCard)) return false;

	//变量定义
	CMD_S_SendAllCard * pSendCard=(CMD_S_SendAllCard *)pBuffer;

	
	ShowScoreView(false);

	////旁观玩家 对家扑克正面显示
	if (IsLookonMode()==true)
	{
		m_GameClientView.m_HandCardControl[0].ShowWindow(SW_SHOW);
		m_GameClientView.m_HandCardControl[0].SetDisplayFlag(true);
		m_GameClientView.m_HandCardControl[2].ShowWindow(SW_SHOW);
		m_GameClientView.m_HandCardControl[2].SetDisplayFlag(true);
	}
	//设置数据
	m_wCurrentUser=pSendCard->wCurrentUser;
	m_bAutoPlay = false ;

	//设置数据
	for (WORD i=0;i<GAME_PLAYER;i++) 
	{
		m_bCardCount[i]=17;
		m_bHandCardCount[i]=17;
		CopyMemory(m_bHandCardData[i],pSendCard->bCardData[i],sizeof(pSendCard->bCardData[i]));

		//排序扑克
		m_GameLogic.SortCardList(m_bHandCardData[i], 17, ST_ORDER);
	}
	

	//设置界面
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		m_GameClientView.SetLandScore(i,0,CSD_NORMAL,CSD_NORMAL);
		m_GameClientView.SetPassFlag(i,false);
		m_GameClientView.m_UserCardControl[i].SetCardData(NULL,0);
		m_GameClientView.m_HandCardControl[i].SetCardData(NULL,0);
	}
	m_GameClientView.m_GameCard.SetCardData();
	WORD wMeChairID = GetMeChairID();
	//if ( ! IsLookonMode() && ! pSendCard->bUserBrightCard[wMeChairID] ) 
	//m_GameClientView.m_btBrightCard.ShowWindow( SW_SHOW );
	//else m_GameClientView.m_btBrightCard.ShowWindow( SW_HIDE );

	//设置界面
	m_GameClientView.SetBombTime(0);
	m_GameClientView.SetLandUser(INVALID_CHAIR,0);

	//设置扑克
	BYTE bBackCard[3]={0,0,0};
	m_GameClientView.SetCardBackDisPlay(false);//cxf 设置底牌为背面显示
	m_GameClientView.SetCardBackData(bBackCard,CountArray(bBackCard));
	//m_GameClientView.m_HandCardControl[1].SetDisplayFlag(((IsLookonMode()==false)||(IsAllowLookon()==true)));
	//m_GameClientView.m_HandCardControl[1].ShowWindow(SW_SHOW);
	

	//排列扑克
	//wMeChairID = GetMeChairID();
	
	DispatchUserCard(m_bHandCardData[wMeChairID],m_bHandCardCount[wMeChairID]);
	m_GameLogic.SortCardList(m_bHandCardData[wMeChairID],m_bHandCardCount[wMeChairID],ST_ORDER);

	////明牌判断
	//for ( WORD wChairID = 0; wChairID < GAME_PLAYER; ++wChairID )
	//{
	//	WORD wViewChairID = SwitchViewChairID( wChairID );

	//	//显示扑克
	//	if ( pSendCard->bUserBrightCard[wChairID] )
	//	{
	//		m_GameClientView.m_HandCardControl[wViewChairID].ShowWindow(SW_SHOW);
	//		m_GameClientView.m_HandCardControl[wViewChairID].SetDisplayFlag(true);
	//	}
	//	else if ( wChairID != GetMeChairID() )
	//	{
	////		m_GameClientView.m_HandCardControl[wViewChairID].ShowWindow(SW_HIDE);
	//		m_GameClientView.m_HandCardControl[wViewChairID].SetDisplayFlag(false);
	//	}
	//}
    m_GameClientView.UpdateGameView(NULL);

	__LEAVE_FUNCTION
	return true;	
}
//用户抢地主
bool CGameClientDlg::onSubSnatchLand(const void *pBuffer,WORD wDataSize)
{
	KillGameTimer(IDI_SNATCH_LAND);
	//校验数据
	ASSERT(wDataSize==sizeof(CMD_S_SnatchLand));
	if (wDataSize!=sizeof(CMD_S_SnatchLand)) return false;
	//消息处理
	CMD_S_SnatchLand *pSnatchLand=(CMD_S_SnatchLand *)pBuffer;
    //获取玩家ID
	WORD WmechairID=GetMeChairID();
	WORD wViewChairID=SwitchViewChairID(pSnatchLand->wSnatchUser);
	const tagUserData *pUserData = GetUserData(pSnatchLand->wSnatchUser);
	//玩家设置
	m_GameClientView.SetMeBeiShu(pSnatchLand->bUserBeiShu[WmechairID]); //设置倍数
	//按钮显示
	SafeShowOrHideWnd(&m_GameClientView.m_btSnatchLand,SW_HIDE);
	SafeEnableWnd(&m_GameClientView.m_btSnatchLand,FALSE);
	SafeShowOrHideWnd(&m_GameClientView.m_btGiveUpSnatchLand,SW_HIDE);
	SafeEnableWnd(&m_GameClientView.m_btGiveUpSnatchLand,FALSE);
	if (pSnatchLand->wCurrentUser != INVALID_CHAIR)
	{
		//不是旁观玩家并且《》
		if (IsLookonMode()==false && pSnatchLand->wCurrentUser==WmechairID)
		{//按钮显示
			SafeShowOrHideWnd(&m_GameClientView.m_btSnatchLand,SW_SHOW);
			SafeEnableWnd(&m_GameClientView.m_btSnatchLand,TRUE);
			SafeShowOrHideWnd(&m_GameClientView.m_btGiveUpSnatchLand,SW_SHOW);
			SafeEnableWnd(&m_GameClientView.m_btGiveUpSnatchLand,TRUE);
		}
		//设置时间
		SetGameTimer(pSnatchLand->wCurrentUser,IDI_SNATCH_LAND,10);
	}
	if (0==pSnatchLand->bSnatchLand)
	{
		//设置抢地主标识
		m_GameClientView.SetLandScore(wViewChairID,3,pSnatchLand->bPreCallScorePhase,pSnatchLand->bCallScorePhase);
		//播放声音
		PlayGameSoundLanguage(pUserData->cbGender, TEXT("GIVEUP_SNATCH"),false);
	}
	else
	{
		//设置抢地主标识
		m_GameClientView.SetLandScore(wViewChairID,4,pSnatchLand->bPreCallScorePhase,pSnatchLand->bCallScorePhase);
		//播放动画
		CString strPath ="";
		GetPicPath(strPath);
	    PlaySound("BEI",false);
		strPath.Append("DOUBLE.png");
		m_GameClientView.m_DoubleAction.SetActionImage(strPath,true,15);
		m_GameClientView.PlayDoubelFlash();
		if(pSnatchLand->bSnatchLandCount==1)
		{
			PlayGameSoundLanguage(pUserData->cbGender,TEXT("SNATCH_1"),false);
		}
		else if(pSnatchLand->bSnatchLandCount==2)
		{
			PlayGameSoundLanguage(pUserData->cbGender,TEXT("SNATCH_2"),false);
		}
		else if (pSnatchLand->bSnatchLandCount==3)
		{
			PlayGameSoundLanguage(pUserData->cbGender,TEXT("SNATCH_3"),false);
		}
	}
	m_GameClientView.UpdateGameView(NULL);
	return true;
}
//用户叫地主
bool CGameClientDlg::OnSubLandScore(const void * pBuffer, WORD wDataSize)
{
	KillGameTimer(IDI_LAND_SCORE);
	//校验数据
	ASSERT(wDataSize==sizeof(CMD_S_LandScore));
	if (wDataSize!=sizeof(CMD_S_LandScore)) return false;
	//消息处理
	CMD_S_LandScore * pLandScore=(CMD_S_LandScore *)pBuffer;
	WORD wViewChairID=SwitchViewChairID(pLandScore->bLandUser);
	const tagUserData *pUserData = GetUserData(pLandScore->bLandUser);
	//设置倍数
	m_GameClientView.SetMeBeiShu(pLandScore->bUserBeiShu[GetMeChairID()]);
	if (pLandScore->wCurrentUser!= INVALID_CHAIR)
	{
		if (IsLookonMode()==false  && pLandScore->wCurrentUser==GetMeChairID())
		{//按钮显示
			if (0==pLandScore->bLandScore)
			{
				SafeShowOrHideWnd(&m_GameClientView.m_btCallLand,SW_SHOW);
				SafeEnableWnd(&m_GameClientView.m_btCallLand,TRUE);
				SafeShowOrHideWnd(&m_GameClientView.m_btGiveUpCallLand,SW_SHOW);
				SafeEnableWnd(&m_GameClientView.m_btGiveUpCallLand,TRUE);
			}
			else
			{
				SafeShowOrHideWnd(&m_GameClientView.m_btSnatchLand,SW_SHOW);
				SafeEnableWnd(&m_GameClientView.m_btSnatchLand,TRUE);
				SafeShowOrHideWnd(&m_GameClientView.m_btGiveUpSnatchLand,SW_SHOW);
				SafeEnableWnd(&m_GameClientView.m_btGiveUpSnatchLand,TRUE);
			}
		}
		if(0==pLandScore->bLandScore)
		{
			//设置时间
			SetGameTimer(pLandScore->wCurrentUser,IDI_LAND_SCORE,10);
			//设置叫地主标识
			m_GameClientView.SetLandScore(wViewChairID,1,pLandScore->bPreCallScorePhase,pLandScore->bCallScorePhase);
			//播放声音
			PlayGameSoundLanguage(pUserData->cbGender, TEXT("GIVEUP_CALLLAND"),false);
		}
		else
		{
			//设置时间
			SetGameTimer(pLandScore->wCurrentUser,IDI_SNATCH_LAND,10);
			//设置叫地主标识
			m_GameClientView.SetLandScore(wViewChairID,2,pLandScore->bPreCallScorePhase,pLandScore->bCallScorePhase);
			//播放声音
			PlayGameSoundLanguage(pUserData->cbGender, TEXT("CALLLAND"),false);
		}
		
		m_GameClientView.UpdateGameView(NULL);
	}

	return true;
}
//用户叫分
//bool CGameClientDlg::OnSubLandScore(const void * pBuffer, WORD wDataSize)
//{
//	__ENTER_FUNCTION
//		
//	KillGameTimer(IDI_LAND_SCORE);
//	//效验数据
//	ASSERT(wDataSize==sizeof(CMD_S_LandScore));
//	if (wDataSize!=sizeof(CMD_S_LandScore)) return false;
//
//	//消息处理
//	CMD_S_LandScore * pLandScore=(CMD_S_LandScore *)pBuffer;
//	//设置界面
//	WORD wViewChairID=SwitchViewChairID(pLandScore->bLandUser);
//
//	m_GameClientView.SetLandScore(wViewChairID,pLandScore->bLandScore,pLandScore->bCallScorePhase,pLandScore->bPreCallScorePhase);
//	m_GameClientView.SetLandUser(wViewChairID, pLandScore->bCurrentScore);
//
//	//当前玩家倍数
//	m_GameClientView.SetMeBeiShu(pLandScore->bUserBeiShu[GetMeChairID()]);
//	/*char chData[128]={0};
//	sprintf(chData, "%d", pLandScore->bUserBeiShu[GetMeChairID()]);
//	InsertSystemString(chData);*/
//	//m_GameClientView.SetBrightStart(INVALID_CHAIR, false);
//
//	//设置变量
//	m_cbCallScorePhase = pLandScore->bCallScorePhase;
//
//	//玩家明牌
//	bool bUserBrightCard=false;
//	//if (pLandScore->bPreCallScorePhase==CSD_BRIGHTCARD && pLandScore->bCallScorePhase==CSD_BRIGHTCARD) 
//	//{
//	//	bUserBrightCard=true;
//	//}
//
//	//玩家设置
//	if (bUserBrightCard==false)
//	{
//		if ((IsLookonMode()==false)&&( ( pLandScore->wCurrentUser == GetMeChairID() && pLandScore->bCallScorePhase!=CSD_BRIGHTCARD) || ( pLandScore->bCallScorePhase==CSD_BRIGHTCARD && 
//			! pLandScore->bBrightCardUser[GetMeChairID()] )))
//		{
//			ActiveGameFrame();
//			SafeShowOrHideWnd(&m_GameClientView.m_btGiveUpScore, SW_SHOW);
//			//m_GameClientView.m_btGiveUpScore.LoadRgnImage(AfxGetInstanceHandle(),IDB_GIVE_UP,RGB(255,0,255));
//
//			if (pLandScore->bCallScorePhase==CSD_NORMAL)
//			{
//				SafeShowOrHideWnd(&m_GameClientView.m_btOneScore, SW_SHOW);
//				SafeShowOrHideWnd(&m_GameClientView.m_btTwoScore, SW_SHOW);
//				SafeShowOrHideWnd(&m_GameClientView.m_btThreeScore, SW_SHOW);
//
//				SafeEnableWnd(&m_GameClientView.m_btOneScore, pLandScore->bCurrentScore==0?TRUE:FALSE);
//				SafeEnableWnd(&m_GameClientView.m_btTwoScore, pLandScore->bCurrentScore<=1?TRUE:FALSE);
//				SafeEnableWnd(&m_GameClientView.m_btThreeScore, pLandScore->bCurrentScore<=2?TRUE:FALSE);
//			}
//			//else if (pLandScore->bCallScorePhase==CSD_SNATCHLAND)
//			//{
//			//	m_GameClientView.m_btSnatchLand.ShowWindow(SW_SHOW);
//			//	m_GameClientView.m_btGiveUpScore.SetButtonImage(IDB_NOT_SNATCH_LAND,AfxGetInstanceHandle(),false);
//			//}
//			//else if (pLandScore->bCallScorePhase==CSD_BRIGHTCARD)
//			//{
//			//	//m_GameClientView.m_btBrightCard.ShowWindow(SW_SHOW);
//			//	//m_GameClientView.m_btGiveUpScore.SetButtonImage(IDB_NOT_BRIGHT_CARD,AfxGetInstanceHandle(),false);
//			//}
//		}
//
//		//设置时间
//		if ( pLandScore->bCallScorePhase==CSD_BRIGHTCARD )
//			SetGameTimer(GetMeChairID(),IDI_LAND_SCORE,10);
//		else if ( pLandScore->bCallScorePhase != CSD_NORMAL )
//			SetGameTimer(pLandScore->wCurrentUser,IDI_LAND_SCORE,10);
//		else 
//			SetGameTimer(pLandScore->wCurrentUser,IDI_LAND_SCORE,15);
//	}
//
//	//播放声音
//	const tagUserData *pUserData = GetUserData(pLandScore->bLandUser);
//
//	if ( pLandScore->bLandScore == 1 )
//	{
//		PlayGameSoundLanguage(pUserData->cbGender, TEXT("JIAOFEN_1"),false);
//	}
//	else if ( pLandScore->bLandScore == 2 )
//	{
//		PlayGameSoundLanguage(pUserData->cbGender, TEXT("JIAOFEN_2"),false);
//	}
//	else if ( pLandScore->bLandScore == 3 )
//	{
//		PlayGameSoundLanguage(pUserData->cbGender, TEXT("JIAOFEN_3"),false);
//	}
//	else
//	{
//		PlayGameSoundLanguage(pUserData->cbGender, TEXT("JIAOFEN_0"),false);
//	}
//	m_GameClientView.UpdateGameView(NULL);
//
//	__LEAVE_FUNCTION
//	return true;
//}

//游戏开始
bool CGameClientDlg::OnSubGameStart(const void * pBuffer, WORD wDataSize)
{
	__ENTER_FUNCTION
			
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_GameStart));
	if (wDataSize!=sizeof(CMD_S_GameStart)) return false;

	//消息处理
	CMD_S_GameStart * pGameStart=(CMD_S_GameStart *)pBuffer;

	SetTaskWndShow(true);

	//设置变量
	m_wBombTime=1;
	m_bTurnCardCount=0;
	m_bTurnOutType=CT_ERROR;
	m_wLandUser=pGameStart->wLandUser;
	m_bCardCount[pGameStart->wLandUser]=20; 
	ZeroMemory(m_bTurnCardData,sizeof(m_bTurnCardData));
	//m_GameClientView.ShowLandTime(false);
	//m_GameClientView.SetBrightStart(INVALID_CHAIR, false);

	KillTimer(IDI_DISPATCH_CARD);
	m_GameClientView.ShowLandTitle(false);
	m_wLastMostUser=pGameStart->wLandUser;

	//设置界面
	m_GameClientView.SetLandScore(INVALID_CHAIR,0,CSD_GAMESTART,CSD_GAMESTART);
	m_GameClientView.SetDoubleScoreUser(INVALID_CHAIR, false);
	m_GameClientView.SetCardBackData(pGameStart->bBackCardData,CountArray(pGameStart->bBackCardData));
	m_GameClientView.SetCardBackDisPlay(true);
	if (IsLookonMode()==false)
	{
		m_GameClientView.SetCardCount(SwitchViewChairID(pGameStart->wLandUser),20);
	}
	SafeShowOrHideWnd(&m_GameClientView.m_btCallLand,SW_HIDE);
	SafeShowOrHideWnd(&m_GameClientView.m_btGiveUpCallLand,SW_HIDE);
	SafeShowOrHideWnd(&m_GameClientView.m_btSnatchLand,SW_HIDE);
	SafeShowOrHideWnd(&m_GameClientView.m_btGiveUpSnatchLand,SW_HIDE);

    //设置地主的牌数据
	WORD wLandUser=pGameStart->wLandUser;
	m_bHandCardCount[wLandUser]=20;
	CopyMemory(&m_bHandCardData[wLandUser][17],pGameStart->bBackCardData,sizeof(pGameStart->bBackCardData));
	m_GameLogic.SortCardList(m_bHandCardData[wLandUser],m_bHandCardCount[wLandUser],ST_ORDER);
	m_GameClientView.m_HandCardControl[SwitchViewChairID(wLandUser)].SetCardData(m_bHandCardData[wLandUser],m_bHandCardCount[wLandUser]);

	m_GameClientView.SetLandUser(SwitchViewChairID(pGameStart->wLandUser),pGameStart->bLandScore);
	if(pGameStart->wLandUser == GetMeChairID())
	{
		m_GameClientView.m_GameCard.SetCardData(m_bHandCardData[wLandUser],m_bHandCardCount[wLandUser]);
	}

	//玩家设置
	if (IsLookonMode()==false) 
	{
	   m_GameClientView.m_GameCard.SetCardShow(true);

	   BYTE byMeViewID = SwitchViewChairID(GetMeChairID());
	   m_GameClientView.m_GameCard.SetCardData(pGameStart->bCardData,pGameStart->bCardNum);
	}
	//当前玩家
	if ((IsLookonMode()==false)&&(pGameStart->wCurrentUser==GetMeChairID()))
	{
		ActiveGameFrame();
		SafeEnableWnd(&m_GameClientView.m_btOutCard, FALSE);
		SafeShowOrHideWnd(&m_GameClientView.m_btOutCard, SW_SHOW);
		SafeEnableWnd(&m_GameClientView.m_btPassCard, FALSE);
		SafeShowOrHideWnd(&m_GameClientView.m_btPassCard, SW_SHOW);
		SafeShowOrHideWnd(&m_GameClientView.m_btAutoOutCard, SW_SHOW);
		SafeEnableWnd(&m_GameClientView.m_btAutoOutCard,FALSE);
	}
	//设置出牌时间
	SetGameTimer(pGameStart->wCurrentUser,IDI_OUT_CARD,15);

	//重置数据
	m_GameClientView.SetResetShouPaiNums();

	m_GameClientView.UpdateGameView(NULL);

	__LEAVE_FUNCTION
	return true;
}

//用户出牌
bool CGameClientDlg::OnSubOutCard(const void * pBuffer, WORD wDataSize)
{
	__ENTER_FUNCTION
		
	//////////////////////////////
	//char str[10]={'a'};
	//InsertSystemString(str);
	ASSERT(wDataSize==sizeof(CMD_S_OutCard));
	if (wDataSize!=sizeof(CMD_S_OutCard))
	{
		return false;
	}
	//变量定义
	CMD_S_OutCard * pOutCard=(CMD_S_OutCard *)pBuffer;
	WORD wHeadSize=sizeof(CMD_S_OutCard)-sizeof(pOutCard->bCardData);
	//效验数据
	//ASSERT(wDataSize>=wHeadSize);
	//if (wDataSize<wHeadSize) return false;
	//ASSERT(wDataSize==(wHeadSize+pOutCard->bCardCount*sizeof(pOutCard->bCardData[0])));
	//if (wDataSize!=(wHeadSize+pOutCard->bCardCount*sizeof(pOutCard->bCardData[0]))) return false;

	//删除定时器
	KillTimer(IDI_MOST_CARD);
	KillGameTimer(IDI_OUT_CARD);

	//上轮出牌用户
	m_wLastMostUser=pOutCard->wOutCardUser;
	/*char chData[128]={0};
	sprintf(chData, "%d,%d", m_wLastMostUser ,pOutCard->wCurrentUser);
	InsertSystemString(chData);*/

	//重置报牌
	m_GameClientView.SetResetSYPaiNums();

	//界面设置
	WORD wOutViewChairID=SwitchViewChairID(pOutCard->wOutCardUser);
	m_bCardCount[pOutCard->wOutCardUser]-=pOutCard->bCardCount;
	if (IsLookonMode()==false)
	{
		m_GameClientView.SetCardCount(wOutViewChairID,m_bCardCount[pOutCard->wOutCardUser]);
	}
	m_GameClientView.SetLandScore(INVALID_CHAIR,0,CSD_GAMESTART,CSD_GAMESTART);

	//SafeShowOrHideWnd(&m_GameClientView.m_btCardDao, SW_HIDE);
	//SafeShowOrHideWnd(&m_GameClientView.m_btCardGen, SW_HIDE);
	//SafeShowOrHideWnd(&m_GameClientView.m_btCardFan, SW_HIDE);
	
	//设置当前倍数
	m_GameClientView.SetMeBeiShu(pOutCard->bUserBeiShu[GetMeChairID()]);

	//小于等于四张牌时显示托管按钮 lq;
	if (pOutCard->bHandCardCount<=4 && GetMeChairID()==pOutCard->wOutCardUser)
	{
		SafeEnableWnd(&m_GameClientView.m_btAutoPlayOn,TRUE);
	}

	WORD wCurrentUser=pOutCard->wCurrentUser;
	WORD wMechairID=GetMeChairID();
	////cxf 隐藏（倒-跟-反）按钮
	//if (IsLookonMode() == false && wCurrentUser==wMechairID) 
	//{//下一玩家（点击出牌时的下一个玩家）
	//	if (wMechairID==pOutCard->wBankerUser)
	//	{//庄家
	//		if (1 == pOutCard->bCircuit[wCurrentUser])
	//		{//出牌次数 小于2
	//			if ((pOutCard->bIsDouble[(pOutCard->wBankerUser +1)%GAME_PLAYER] 
	//					|| pOutCard->bIsDouble[(pOutCard->wBankerUser + 2)%GAME_PLAYER]) 
	//					&& !pOutCard->bIsDouble[pOutCard->wBankerUser])
	//			{//农民有 "倒"或"跟" 并且庄家还没有"反牌"                       
	//				SafeShowOrHideWnd(&m_GameClientView.m_btCardFan, SW_SHOW);
	//			}
	//		}
	//	}
	//	else
	//	{//农民
	//		if (pOutCard->bCircuit[wCurrentUser] < 1)
	//		{//出牌次数 小于1
	//			if (wCurrentUser == (pOutCard->wBankerUser+1)%GAME_PLAYER)
	//			{//第一个农民，
	//				if (!pOutCard->bIsDouble[(pOutCard->wBankerUser+1)%GAME_PLAYER])
	//				{//之前没有倒，可以倒。
	//					SafeShowOrHideWnd(&m_GameClientView.m_btCardDao, SW_SHOW);
	//				}
	//			}
	//			else if ((pOutCard->wBankerUser+2)%GAME_PLAYER == wCurrentUser)
	//			{//第二个农民.
	//				if (pOutCard->bIsDouble[(wCurrentUser+2)%GAME_PLAYER] 
	//						&& !pOutCard->bIsDouble[(wCurrentUser)%GAME_PLAYER])
	//				{//加倍了,可以跟.
	//					SafeShowOrHideWnd(&m_GameClientView.m_btCardGen, SW_SHOW);
	//				}
	//				else if(!pOutCard->bIsDouble[(wCurrentUser)%GAME_PLAYER])
	//				{//没加倍,可以倒.
	//					SafeShowOrHideWnd(&m_GameClientView.m_btCardDao, SW_SHOW);
	//				}
	//			}
	//		}
	//	}
	//}
	//if (2 == pOutCard->bCircuit[pOutCard->wBankerUser])
	//{
		for (BYTE i=0; i<GAME_PLAYER; i++)
		{
			m_GameClientView.SetDoubleScoreUser(SwitchViewChairID(i), false);
		}
	//}
	//---end
	//移除底牌
    /*if (2 == pOutCard->bCircuit[pOutCard->wBankerUser])
    {
		m_GameClientView.SetCardBackData(NULL,0);
    }*/
	//出牌设置
	if ((IsLookonMode()==true)||(wMechairID!=pOutCard->wOutCardUser))
	{
		m_GameClientView.m_UserCardControl[wOutViewChairID].SetCardData(pOutCard->bCardData,pOutCard->bCardCount);
	}

	//清理桌面
	if (m_bTurnCardCount==0)
	{
		for (WORD i=0;i<GAME_PLAYER;i++) 
		{
			if (i==pOutCard->wOutCardUser) continue;
			m_GameClientView.m_UserCardControl[SwitchViewChairID(i)].SetCardData(NULL,0);
		}
		m_GameClientView.SetPassFlag(INVALID_CHAIR,false);
	}

	//报牌信息
	const tagUserData *pUserData = GetUserData(pOutCard->wOutCardUser);//用户信息

	bool bWarmingCard = false;
	/*if(m_bCardCount[pOutCard->wOutCardUser]<=2) 
	{
		WORD wViewChairID = SwitchViewChairID(pOutCard->wOutCardUser) ;

		if ( pOutCard->wOutCardUser!=GetMeChairID() )
		{
			PlayGameSound(AfxGetInstanceHandle(),TEXT("WARMING_CARD"));
			bWarmingCard = true;
		}

		m_GameClientView.SetShowWarming(wViewChairID, true) ;
	}*/
	if (m_bCardCount[pOutCard->wOutCardUser]<=2&&wCurrentUser!=INVALID_CHAIR)
	{//报牌提示
		WORD wViewChairID = SwitchViewChairID(pOutCard->wOutCardUser) ;
		/*m_GameClientView.SetUserShouPaiNums(wViewChairID, m_bCardCount[pOutCard->wOutCardUser]);*/
		switch (m_bCardCount[pOutCard->wOutCardUser])
		{
		case 1://1张
			PlayGameSoundLanguage(pUserData->cbGender, TEXT("BAOJING_1"),false);
			break;
		case 2://2张
			PlayGameSoundLanguage(pUserData->cbGender, TEXT("BAOJING_2"),false);
			break;
		}	
		bWarmingCard = true;
		m_GameClientView.SetShowWarming(wViewChairID, true) ;
	}

	//记录出牌
	m_bTurnCardCount=pOutCard->bCardCount;
	m_bTurnOutType=m_GameLogic.GetCardType(pOutCard->bCardData,pOutCard->bCardCount);
	CopyMemory(m_bTurnCardData,pOutCard->bCardData,sizeof(BYTE)*pOutCard->bCardCount);

	m_tagOldCard.cbCardCount = m_bTurnCardCount;
    CopyMemory(m_tagOldCard.cbResultCard, m_bTurnCardData, m_bTurnCardCount);

	//炸弹判断
	if ((m_bTurnOutType==CT_BOMB_CARD)||(m_bTurnOutType==CT_MISSILE_CARD))
	{
		/*if(m_wBombTime > 1)
		{
			PlayGameSoundLanguage(pUserData->cbGender, TEXT("ZHADAN_2"),false);
			bWarmingCard = true;
		}*/
		m_wBombTime*=2;
		m_GameClientView.SetBombTime(pOutCard->wBombNums);
	}

	//自己手上扑克(旁观界面)
	if (IsLookonMode()==true/*||(pOutCard->wOutCardUser!=wMechairID)*/)
	{
		//删除扑克
		WORD wOutCardUser=pOutCard->wOutCardUser;
		BYTE bSourceCount=m_bHandCardCount[wOutCardUser];
		m_bHandCardCount[wOutCardUser]-=pOutCard->bCardCount;
		m_GameLogic.RemoveCard(pOutCard->bCardData,pOutCard->bCardCount,m_bHandCardData[wOutCardUser],bSourceCount);

		//设置界面
		m_GameClientView.m_HandCardControl[SwitchViewChairID(wOutCardUser)].SetCardData(m_bHandCardData[wOutCardUser],m_bHandCardCount[wOutCardUser]);
		if (wOutCardUser==wMechairID)
		{
			m_GameClientView.m_GameCard.SetCardData(m_bHandCardData[wMechairID],m_bHandCardCount[wMechairID]);
		}
	}

	//最大判断(火箭)
	if (wCurrentUser==pOutCard->wOutCardUser)
	{
		//设置变量
		m_bTurnCardCount=0;
		m_bTurnOutType=CT_ERROR;
		m_wMostUser=wCurrentUser;
		memset(m_bTurnCardData,0,sizeof(m_bTurnCardData));
		m_tagOldCard.cbCardCount=0;
		memset(m_tagOldCard.cbResultCard, 0,sizeof(m_tagOldCard.cbResultCard));

		//设置界面
		for (WORD i=0;i<GAME_PLAYER;i++)
		{
			if (i!=pOutCard->wOutCardUser)
			{
				WORD wViewChairID=SwitchViewChairID(i);
				m_GameClientView.SetPassFlag(wViewChairID,true);
				m_GameClientView.m_UserCardControl[wViewChairID].SetCardData(NULL,0);
			}
		}

		//播放声音
		if ( !bWarmingCard )
		{
			CString strPath = "";
			GetPicPath(strPath);
			PlayGameSoundLanguage(pUserData->cbGender, TEXT("HUOJIAN"),false);
			PlaySound("THUOJIAN",false);
			strPath.Append("HUOJIAN.png");
			m_GameClientView.m_HuoJianAction.SetActionImage(strPath,false,1,false);
			m_GameClientView.SetHuoJian(true);
		}
		
		//设置定时器
		SetTimer(IDI_MOST_CARD,3000,NULL);

		m_GameClientView.UpdateGameView(NULL);
		return true;
	}
	//显示大不上图片  lq
	if (wCurrentUser==wMechairID && wCurrentUser != pOutCard->wOutCardUser)
	{//当前操作用户
		tagOutCardResult OutCardResult;
		WORD wMeChairID = GetMeChairID();
		m_GameLogic.SearchOutCard(m_bHandCardData[wMeChairID],m_bHandCardCount[wMeChairID],m_bTurnCardData,m_bTurnCardCount,OutCardResult);

		//设置界面
		if (OutCardResult.cbCardCount<=0)
		{//没有大起牌
			m_GameClientView.SetShowBigness(true);
		}
		else
		{//隐藏
			m_GameClientView.SetShowBigness(false);
		}
	}
	
	//
	//播放声音
	if (!bWarmingCard&& wCurrentUser!=INVALID_CHAIR )
	{
		BYTE cbOutCardType = m_GameLogic.GetCardType(pOutCard->bCardData, pOutCard->bCardCount );
		BYTE cbOutCardData=m_GameLogic.GetCardValue(pOutCard->bCardData[0]);
		CString strPath = "";
		GetPicPath(strPath);
		switch (cbOutCardType)
		{
		case CT_BOMB_CARD://炸弹
			PlayGameSoundLanguage(pUserData->cbGender, TEXT("ZHADAN_1"),false);
			PlaySound("TZHADAN",false);
			strPath.Append("BOMB_EFFECT.png");
			m_GameClientView.m_ActionWnd[wOutViewChairID].SetActionImage(strPath,true,8);
			m_GameClientView.PlayActionFlash(wOutViewChairID,MARK_ACTION_ZHADAN);
			break;
		case CT_DOUBLE://对子
			{
				switch (cbOutCardData)
				{
				case 1:
					PlayGameSoundLanguage(pUserData->cbGender,TEXT("MALE_DB_1"),false);
					break;
				case 2:
					PlayGameSoundLanguage(pUserData->cbGender,TEXT("MALE_DB_2"),false);
					break;
				case 3:
					PlayGameSoundLanguage(pUserData->cbGender,TEXT("MALE_DB_3"),false);
					break;
				case 4:
					PlayGameSoundLanguage(pUserData->cbGender,TEXT("MALE_DB_4"),false);
					break;
				case 5:
					PlayGameSoundLanguage(pUserData->cbGender,TEXT("MALE_DB_5"),false);
					break;
				case 6:
					PlayGameSoundLanguage(pUserData->cbGender,TEXT("MALE_DB_6"),false);
					break;
				case 7:
					PlayGameSoundLanguage(pUserData->cbGender,TEXT("MALE_DB_7"),false);
					break;
				case 8:
					PlayGameSoundLanguage(pUserData->cbGender,TEXT("MALE_DB_8"),false);
					break;
				case 9:
					PlayGameSoundLanguage(pUserData->cbGender,TEXT("MALE_DB_9"),false);
					break;
				case 10:
					PlayGameSoundLanguage(pUserData->cbGender,TEXT("MALE_DB_10"),false);
					break;
				case 11:
					PlayGameSoundLanguage(pUserData->cbGender,TEXT("MALE_DB_11"),false);
					break;
				case 12:
					PlayGameSoundLanguage(pUserData->cbGender,TEXT("MALE_DB_12"),false);
					break;
				case 13:
					PlayGameSoundLanguage(pUserData->cbGender,TEXT("MALE_DB_13"),false);
					break;
				}

			}
			break;
		case CT_THREE://三张
			PlayGameSoundLanguage(pUserData->cbGender, TEXT("SANZHANG"),false);
			break;
		case CT_SINGLE_LINE://顺子
			PlayGameSoundLanguage(pUserData->cbGender, TEXT("SHUN"),false);
			PlaySound("TSHUNLIAN",false);
			if (pOutCard->bCardCount>=7)
			{
				strPath.Append("CHANGSHUNZI.png");
                m_GameClientView.m_ActionWnd[wOutViewChairID].SetActionImage(strPath,true,15);
			}
			else
			{
				strPath.Append("SHUNZI.png");
				m_GameClientView.m_ActionWnd[wOutViewChairID].SetActionImage(strPath,true,15);
			}			
            m_GameClientView.PlayActionFlash(wOutViewChairID,MARK_ACTION_SHUNLIAN);
			break;
		case CT_DOUBLE_LINE://连对
			PlayGameSoundLanguage(pUserData->cbGender, TEXT("LIANDUI"),false);
			PlaySound("TSHUNLIAN",false);
			strPath.Append("LIANDUI.png");
			m_GameClientView.m_ActionWnd[wOutViewChairID].SetActionImage(strPath,true,15);
			m_GameClientView.PlayActionFlash(wOutViewChairID,MARK_ACTION_SHUNLIAN);
			break;
		case CT_THREE_LINE://三连(飞机)
			if(4<pOutCard->bCardCount)
			{
				PlayGameSoundLanguage(pUserData->cbGender, TEXT("FEIJIDAICHIBANG"),false);
				PlaySound("TFEIJI",false);
				strPath.Append("FEIJI.png");
				m_GameClientView.m_BActionWnd.SetActionImage(strPath,true,1,false);
				m_GameClientView.SetFeiJi(true);
			}		
			break;
		case CT_THREE_LINE_TAKE_ONE://三张带一张
			if(4<pOutCard->bCardCount)
			{
				PlayGameSoundLanguage(pUserData->cbGender, TEXT("FEIJIDAICHIBANG"),false);
				PlaySound("TFEIJI",false);
				strPath.Append("FEIJI.png");
				m_GameClientView.m_BActionWnd.SetActionImage(strPath,true,1,false);
				m_GameClientView.SetFeiJi(true);
			}
			else
			{
				PlayGameSoundLanguage(pUserData->cbGender, TEXT("SANDAIYI"),false);
			}
			break;
		case CT_THREE_LINE_TAKE_TWO://三张带一对
			if(5<pOutCard->bCardCount)
			{
				PlayGameSoundLanguage(pUserData->cbGender, TEXT("FEIJIDAICHIBANG"),false);
				PlaySound("TFEIJI",false);
				strPath.Append("FEIJI.png");
				m_GameClientView.m_BActionWnd.SetActionImage(strPath,true,1,false);
				m_GameClientView.SetFeiJi(true);
			}
			else
			{
				PlayGameSoundLanguage(pUserData->cbGender, TEXT("SANDAIRE"),false);
			}
			break;
		case CT_FOUR_LINE_TAKE_ONE://四张带一对
			PlayGameSoundLanguage(pUserData->cbGender, TEXT("SIDAIYI"),false);
			break;
		case CT_FOUR_LINE_TAKE_TWO://四张带两对
			PlayGameSoundLanguage(pUserData->cbGender, TEXT("SIDAIRE"),false);
			break;
		case CT_SINGLE:
			{
				switch (cbOutCardData)
				{
				case 1:
					PlayGameSoundLanguage(pUserData->cbGender,TEXT("MALE_1"),false);
					break;
				case 2:
					PlayGameSoundLanguage(pUserData->cbGender,TEXT("MALE_2"),false);
					break;
				case 3:
					PlayGameSoundLanguage(pUserData->cbGender,TEXT("MALE_3"),false);
					break;
				case 4:
					PlayGameSoundLanguage(pUserData->cbGender,TEXT("MALE_4"),false);
					break;
				case 5:
					PlayGameSoundLanguage(pUserData->cbGender,TEXT("MALE_5"),false);
					break;
				case 6:
					PlayGameSoundLanguage(pUserData->cbGender,TEXT("MALE_6"),false);
					break;
				case 7:
					PlayGameSoundLanguage(pUserData->cbGender,TEXT("MALE_7"),false);
					break;
				case 8:
					PlayGameSoundLanguage(pUserData->cbGender,TEXT("MALE_8"),false);
					break;
				case 9:
					PlayGameSoundLanguage(pUserData->cbGender,TEXT("MALE_9"),false);
					break;
				case 10:
					PlayGameSoundLanguage(pUserData->cbGender,TEXT("MALE_10"),false);
					break;
				case 11:
					PlayGameSoundLanguage(pUserData->cbGender,TEXT("MALE_11"),false);
					break;
				case 12:
					PlayGameSoundLanguage(pUserData->cbGender,TEXT("MALE_12"),false);
					break;
				case 13:
					PlayGameSoundLanguage(pUserData->cbGender,TEXT("MALE_13"),false);
					break;
				case 14:
					PlayGameSoundLanguage(pUserData->cbGender,TEXT("MALE_14"),false);
					break;
				case 15:
					PlayGameSoundLanguage(pUserData->cbGender,TEXT("MALE_15"),false);
					break;
				}				
			}
			break;
		default://默认普通出牌声音
			PlaySound("OUT_CARD",false);
			break;
		}
	}

	//玩家设置
	if (wCurrentUser!=INVALID_CHAIR)
	{
		WORD wViewChairID=SwitchViewChairID(wCurrentUser);
		m_GameClientView.SetPassFlag(wViewChairID,false);
		m_GameClientView.m_UserCardControl[wViewChairID].SetCardData(NULL,0);
	}
	//玩家设置
	if ((IsLookonMode()==false)&&(wCurrentUser==wMechairID))
	{
		if(m_bSystemTrustee==true && m_wLastMostUser!=wMechairID)
		{
			ActiveGameFrame();
			SafeShowOrHideWnd(&m_GameClientView.m_btOutCard, SW_HIDE);
			SafeShowOrHideWnd(&m_GameClientView.m_btPassCard, SW_HIDE);
			SafeShowOrHideWnd(&m_GameClientView.m_btAutoOutCard, SW_HIDE);
		    NoHitPassOutCard();
			m_GameClientView.UpdateGameView(NULL);
			return true;
		}
		else
		{
			ActiveGameFrame();
			SafeEnableWnd(&m_GameClientView.m_btPassCard,TRUE);
			SafeShowOrHideWnd(&m_GameClientView.m_btOutCard, SW_SHOW);
			SafeShowOrHideWnd(&m_GameClientView.m_btPassCard, SW_SHOW);
			SafeEnableWnd(&m_GameClientView.m_btAutoOutCard,TRUE);
			SafeShowOrHideWnd(&m_GameClientView.m_btAutoOutCard, SW_SHOW);
			SafeEnableWnd(&m_GameClientView.m_btOutCard,(VerdictOutCard()==true)?TRUE:FALSE);
		}
		::memset(&m_tagNewCard, 0, sizeof(m_tagNewCard));
		//lq
	}
	//设置时间
	if (wCurrentUser!=INVALID_CHAIR)
	{
		SetGameTimer(pOutCard->wCurrentUser,IDI_OUT_CARD,15);
	}
	m_GameClientView.UpdateGameView(NULL);

	__LEAVE_FUNCTION
	return true;
}

//放弃出牌
bool CGameClientDlg::OnSubPassCard(const void * pBuffer, WORD wDataSize)
{
	__ENTER_FUNCTION
		
	//效验数据
	if (wDataSize!=sizeof(CMD_S_PassCard)) return false;
	CMD_S_PassCard * pPassCard=(CMD_S_PassCard *)pBuffer;

	//删除定时器
	KillGameTimer(IDI_OUT_CARD);

	WORD wMechairID=GetMeChairID();
	//玩家设置
	if ((IsLookonMode()==true)||(pPassCard->wPassUser!=wMechairID))
	{
		WORD wViewChairID=SwitchViewChairID(pPassCard->wPassUser);
		m_GameClientView.SetPassFlag(wViewChairID,true);
		m_GameClientView.m_UserCardControl[wViewChairID].SetCardData(NULL,0);
	}
	//一轮判断
	if (pPassCard->bNewTurn==TRUE)
	{
		m_bTurnCardCount=0;
		m_bTurnOutType=CT_ERROR;
		memset(m_bTurnCardData,0,sizeof(m_bTurnCardData));
		m_tagOldCard.cbCardCount=0;
		memset(m_tagOldCard.cbResultCard, 0,sizeof(m_tagOldCard.cbResultCard));
	}
	//设置界面
	WORD wViewChairID=SwitchViewChairID(pPassCard->wCurrentUser);
	m_GameClientView.SetPassFlag(wViewChairID,false);
	m_GameClientView.m_UserCardControl[wViewChairID].SetCardData(NULL,0);

	//SafeShowOrHideWnd(&m_GameClientView.m_btCardDao, SW_HIDE);
	//SafeShowOrHideWnd(&m_GameClientView.m_btCardGen, SW_HIDE);
	//SafeShowOrHideWnd(&m_GameClientView.m_btCardFan, SW_HIDE);

	//cxf 隐藏（倒-跟-反）按钮
	//if (IsLookonMode() == false && pPassCard->wCurrentUser==wMechairID) 
	//{//下一玩家（点击出牌时的下一个玩家）
	//	if (GetMeChairID()==pPassCard->wBankerUser)
	//	{//庄家
	//		if (1 == pPassCard->bCircuit[pPassCard->wCurrentUser])
	//		{//出牌次数 小于2
	//			if ((pPassCard->bIsDouble[(pPassCard->wBankerUser +1)%GAME_PLAYER] 
	//			|| pPassCard->bIsDouble[(pPassCard->wBankerUser + 2)%GAME_PLAYER]) 
	//				&& !pPassCard->bIsDouble[pPassCard->wBankerUser])
	//			{//农民有 "倒"或"跟" 并且庄家还没有"反牌"
	//				SafeShowOrHideWnd(&m_GameClientView.m_btCardFan, SW_SHOW);
	//			}
	//		}
	//	}
	//	else
	//	{//农民
	//		if (pPassCard->bCircuit[pPassCard->wCurrentUser] < 1)
	//		{//出牌次数 小于1
	//			if (pPassCard->wCurrentUser == (pPassCard->wBankerUser+1)%GAME_PLAYER)
	//			{//第一个农民，可以倒。
	//				SafeShowOrHideWnd(&m_GameClientView.m_btCardDao, SW_SHOW);
	//			}
	//			else if ((pPassCard->wBankerUser+2)%GAME_PLAYER == pPassCard->wCurrentUser)
	//			{//第二个农民.
	//				if (pPassCard->bIsDouble[(pPassCard->wCurrentUser+2)%GAME_PLAYER] 
	//				&& !pPassCard->bIsDouble[(pPassCard->wCurrentUser)%GAME_PLAYER])
	//				{//加倍了,可以跟.
	//					SafeShowOrHideWnd(&m_GameClientView.m_btCardGen, SW_SHOW);
	//				}
	//				else if(!pPassCard->bIsDouble[(pPassCard->wCurrentUser)%GAME_PLAYER])
	//				{//没加倍,可以倒.
	//					SafeShowOrHideWnd(&m_GameClientView.m_btCardDao, SW_SHOW);
	//				}

	//			}
	//		}
	//	}
	//}
	/*if (2 == pPassCard->bCircuit[pPassCard->wBankerUser])
	{*/
		for (BYTE i=0; i<GAME_PLAYER; i++)
		{
			m_GameClientView.SetDoubleScoreUser(SwitchViewChairID(i), false);
		}
	/*}*/
	//---end

	//玩家设置
	if ((IsLookonMode()==false)&&(pPassCard->wCurrentUser==wMechairID))
	{
		tagOutCardResult OutCardResult;
		m_GameLogic.SearchOutCard(m_bHandCardData[wMechairID],m_bHandCardCount[wMechairID],m_bTurnCardData,m_bTurnCardCount,OutCardResult);

		//设置界面
		if (OutCardResult.cbCardCount<=0)
		{//没有大起牌
			m_GameClientView.SetShowBigness(true);
		}
		else
		{//隐藏
			m_GameClientView.SetShowBigness(false);
		}
		if(m_bSystemTrustee==true && m_wLastMostUser!=wMechairID)
		{
			ActiveGameFrame();
			SafeShowOrHideWnd(&m_GameClientView.m_btOutCard, SW_HIDE);
			SafeShowOrHideWnd(&m_GameClientView.m_btPassCard, SW_HIDE);
			SafeShowOrHideWnd(&m_GameClientView.m_btAutoOutCard, SW_HIDE);
			NoHitPassOutCard();
			m_GameClientView.UpdateGameView(NULL);
			return true;
		}
		else
		{
			ActiveGameFrame();
			SafeShowOrHideWnd(&m_GameClientView.m_btOutCard, SW_SHOW);
			SafeShowOrHideWnd(&m_GameClientView.m_btPassCard, SW_SHOW);
			SafeShowOrHideWnd(&m_GameClientView.m_btAutoOutCard, SW_SHOW);
			SafeEnableWnd(&m_GameClientView.m_btPassCard,(m_bTurnCardCount>0)?TRUE:FALSE);
			SafeEnableWnd(&m_GameClientView.m_btOutCard,(VerdictOutCard()==true)?TRUE:FALSE);
			SafeEnableWnd(&m_GameClientView.m_btAutoOutCard,TRUE);
		}
	}

	//重置报牌
	m_GameClientView.SetResetSYPaiNums();

	//播放声音
	//if (IsLookonMode()==true||(pPassCard->wPassUser!=GetMeChairID()))	
	//{
		const tagUserData *pUserData = GetUserData(pPassCard->wPassUser);//用户信息
		PlayGameSoundLanguage(pUserData->cbGender, TEXT("BUYAO"),true);
	//}

	//设置时间
	if (m_bTurnCardCount!=0)
	{
		SetGameTimer(pPassCard->wCurrentUser,IDI_OUT_CARD,15);
	}
	else SetGameTimer(pPassCard->wCurrentUser,IDI_OUT_CARD,15);

	m_GameClientView.UpdateGameView(NULL);

	__LEAVE_FUNCTION
	return true;
}

//游戏结束
bool CGameClientDlg::OnSubGameEnd(const void * pBuffer, WORD wDataSize)
{
	__ENTER_FUNCTION
	
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_GameEnd));
	if (wDataSize!=sizeof(CMD_S_GameEnd)) return false;

	//消息处理
	CMD_S_GameEnd * pGameEnd=(CMD_S_GameEnd *)pBuffer;

	 
	KillTimer(IDI_MOST_CARD);
	KillGameTimer(IDI_OUT_CARD);
	KillGameTimer(IDI_LAND_SCORE);
	KillTimer(IDI_DISPATCH_CARD);
	KillGameTimer(IDI_SNATCH_LAND);

	//疯狂变量
	m_cbCallScorePhase = CSD_NORMAL;
	//m_GameClientView.m_bDouble=1;

	//停止发牌
	DoncludeDispatchCard();

	//隐藏控件
	SafeShowOrHideWnd(&m_GameClientView.m_btOutCard, SW_HIDE);
	SafeShowOrHideWnd(&m_GameClientView.m_btPassCard, SW_HIDE);
	SafeShowOrHideWnd(&m_GameClientView.m_btCallLand, SW_HIDE);
	SafeShowOrHideWnd(&m_GameClientView.m_btGiveUpCallLand, SW_HIDE);
	SafeShowOrHideWnd(&m_GameClientView.m_btSnatchLand, SW_HIDE);
	SafeShowOrHideWnd(&m_GameClientView.m_btAutoOutCard, SW_HIDE);
	SafeShowOrHideWnd(&m_GameClientView.m_btGiveUpSnatchLand, SW_HIDE);
	SafeEnableWnd(&m_GameClientView.m_btSortCard, FALSE);

	//SafeShowOrHideWnd(&m_GameClientView.m_btSnatchLand, SW_HIDE);

	SafeShowOrHideWnd(&m_GameClientView.m_btAutoPlayOff,  SW_HIDE ) ;
	SafeShowOrHideWnd(&m_GameClientView.m_btAutoPlayOn, SW_HIDE);

	/*SafeShowOrHideWnd(&m_GameClientView.m_btCardDao, SW_HIDE);
	SafeShowOrHideWnd(&m_GameClientView.m_btCardGen, SW_HIDE);
	SafeShowOrHideWnd(&m_GameClientView.m_btCardFan, SW_HIDE);*/
    
	//禁用控件
	SafeEnableWnd(&m_GameClientView.m_btOutCard, FALSE);
	SafeEnableWnd(&m_GameClientView.m_btPassCard, FALSE);

	//界面设置
	m_GameClientView.SetShowWarming(INVALID_CHAIR, false) ;
	m_GameClientView.SetLandScore(INVALID_CHAIR,0,CSD_NORMAL,CSD_NORMAL);

	InsertShtickString( TEXT( "本局结束,成绩统计:" ), RGB( 255, 0, 0 ), true );

	//设置积分
	CString strScore;
	WORD IdMe=GetMeChairID();
	WORD landchair=m_wLandUser;
	m_ScoreView.GetMeID(IdMe,landchair);
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		m_GameClientView.m_bUserTrustee[i]=false;//游戏结束,取消托管.
		const tagUserData * pUserData=GetUserData(i);
		m_ScoreView.SetGameScore(i,pUserData->szName,pGameEnd->lGameScore[i],pGameEnd->GameBeiShu[i],pGameEnd->RoomScore[i]);
		strScore.Format( TEXT( "%s:" ),pUserData->szName);

		//add by Ivan_han
		char sz[200]={0};
		sprintf(sz,"游戏结束OnSubGameEnd:chairID=%d,用户名字=%s,游戏积分=%I64d,游戏倍数=%d,房间积分=%I64d\n",i,pUserData->szName,pGameEnd->lGameScore[i],pGameEnd->GameBeiShu[i],pGameEnd->RoomScore[i]);
        OutputDebugStringA(sz);

		if (i==IdMe)
		{
			InsertShtickString( strScore, RGB( 0, 255, 0 ), false );		
		}
		else
		{
			InsertShtickString(TEXT("玩家:"),RGB(0,255,0),false );
		}		
		strScore.Format( TEXT( " %I64d" ),pGameEnd->lGameScore[i]);
		InsertShtickString( strScore, RGB( 255, 255, 0 ), true );
		//设置宝石
		m_ScoreView.SetGem(i,pGameEnd->iLargessCount[i]);
		//设置逃跑
		m_ScoreView.SetUserLeft(i,pGameEnd->IsUserLeft[i]);
		//设置税收
		m_ScoreView.SetGameTax(i,pGameEnd->lGameTax[i]);

		sprintf(sz,"奖励宝石=%d,用户是否强退=%d,游戏税收=%d\n",pGameEnd->iLargessCount[i],pGameEnd->IsUserLeft[i],pGameEnd->lGameTax[i]);
		OutputDebugStringA(sz);
	}

	//游戏类型
	IClientKernel *pIClientKernel = ( IClientKernel * )GetClientKernel( IID_IClientKernel, VER_IClientKernel );
	const tagServerAttribute *pServerAttribute  = pIClientKernel->GetServerAttribute();
	if ( pServerAttribute->wGameGenre == GAME_GENRE_GOLD )
		{
			strScore.Format( TEXT( "服务费：%I64d" ), pGameEnd->lGameTax[IdMe]);
		    InsertShtickString( strScore, RGB( 255, 0, 0 ), true );	
		}
	//旁观玩家不显示-返回大厅-继续游戏-返回房间
	/*if(true==IsLookonMode())
	{
	}*/		
	//显示扑克
	BYTE cbCardPos=0 ;
	for(BYTE i=0; i<GAME_PLAYER; ++i)
	{
		WORD wViewID = SwitchViewChairID(i);
		if(1 == wViewID)
		{
			m_GameClientView.m_HandCardControl[wViewID].SetCardData(pGameEnd->bHandCardData[i], pGameEnd->bCardCount[i]) ;
			m_GameClientView.m_HandCardControl[wViewID].SetDisplayFlag(true) ;
			SafeShowOrHideWnd(&m_GameClientView.m_HandCardControl[wViewID], SW_SHOW);
			//cbCardPos += pGameEnd->bCardCount[i];
		}
		else
		{//对家手牌
			m_GameClientView.SetHandCardData(pGameEnd->bHandCardData[i], pGameEnd->bCardCount[i], wViewID);
		}	
	}
	//for(BYTE i=0; i<GAME_PLAYER; ++i)
	//{
	//	WORD wViewID = SwitchViewChairID(i);
	//	m_GameClientView.m_HandCardControl[wViewID].SetCardData(pGameEnd->bCardData+cbCardPos, pGameEnd->bCardCount[i]) ;
	//	m_GameClientView.m_HandCardControl[wViewID].SetDisplayFlag(true);
	//	m_GameClientView.m_HandCardControl[wViewID].ShowWindow(SW_SHOW);
	//	cbCardPos += pGameEnd->bCardCount[i];
	//}

	//显示自己扑克
	//m_GameClientView.m_HandCardControl[SwitchViewChairID(GetMeChairID())].SetDisplayFlag(true) ;
	//m_GameClientView.m_HandCardControl[SwitchViewChairID(GetMeChairID())].ShowWindow(SW_SHOW);

	//播放声音
	WORD wMeChairID=GetMeChairID();
	LONGLONG lMeScore=pGameEnd->lGameScore[wMeChairID];

	if (lMeScore>0L)
	{
		PlaySound("GAME_WIN",false);
	}
	else if(lMeScore<0L)
	{
		PlaySound("GAME_LOST",false);
	}
	else
	{
		PlaySound("GAME_END",false);
	}
	//宝石
	for (int i=0;i<GAME_PLAYER;++i)
	{
		m_LargessCount[i]=pGameEnd->iLargessCount[i];
	}
	//春天和反春
	m_IsSpring=pGameEnd->IsChunTian;
	m_IsFanSpring=pGameEnd->IsFanChun;
	
	//设置界面
	if (IsLookonMode()==false)
	{
		//春天,反春,宝石,结束
		m_bchuntian=false;
		m_bfanchun=false;
		m_bbaoshi=false;
		m_bjieshu=false;
		SetGameTimer(wMeChairID,IDI_ENDSTART_GAME,30);

		//托管功能
		SafeShowOrHideWnd(&m_GameClientView.m_btAutoPlayOn, SW_HIDE);
		SafeShowOrHideWnd(&m_GameClientView.m_btAutoPlayOff, SW_HIDE);
		SafeShowOrHideWnd(&m_GameClientView.m_btSortCard,  SW_HIDE );
		//OnAutoPlay( 0, 0 );
	}

	//重置数据
	m_GameClientView.SetResetShouPaiNums();
	ZeroMemory(&m_tagOldCard,sizeof(m_tagOldCard));
	m_GameClientView.SetShowBigness(false);

	m_GameClientView.ShowLandTitle(false);
	m_GameClientView.SetMeBeiShu(0);
	m_GameClientView.SetCardCount(INVALID_CHAIR,0);
	m_GameClientView.SetLandUser(INVALID_CHAIR,0);
	for (int i=0;i<GAME_PLAYER;++i)
	{
		m_GameClientView.SetUserTrustee( i, FALSE);
	}
	
	if (IsLookonMode()==true)
	{
		for (WORD i=0;i<GAME_PLAYER;i++)
		{
			m_GameClientView.SetHandCardData(NULL, 0, i);
		}
	}
//#ifdef VIDEO_GAME
//	//显示扑克
//	m_GameClientView.m_HandCardControl[ 0 ].ShowWindow( SW_HIDE );
//	m_GameClientView.m_HandCardControl[ 2 ].ShowWindow( SW_HIDE );
//#endif

	m_GameClientView.UpdateGameView(NULL);

	__LEAVE_FUNCTION
	return true;
}

//出牌判断
bool CGameClientDlg::VerdictOutCard()
{
	__ENTER_FUNCTION
		
	//状态判断
	if (m_GameClientView.m_btOutCard.IsWindowVisible()==FALSE) return false;

	//获取扑克
	BYTE bCardData[20];
	//BYTE bShootCount=(BYTE)m_GameClientView.m_HandCardControl[1].GetShootCard(bCardData,CountArray(bCardData));
     BYTE  bShootCount=(BYTE)m_GameClientView.m_GameCard.GetShootCard(bCardData,CountArray(bCardData));
	
	//出牌判断
	if (bShootCount>0L)
	{
		//排序扑克
		m_GameLogic.SortCardList(bCardData,bShootCount,ST_ORDER);

		//分析类型
		BYTE bCardType=m_GameLogic.GetCardType(bCardData,bShootCount);

		//类型判断
		if (bCardType==CT_ERROR) return false;

		//跟牌判断
		if (m_bTurnCardCount==0) return true;
		return m_GameLogic.CompareCard(m_bTurnCardData,bCardData,m_bTurnCardCount,bShootCount);
	}

	__LEAVE_FUNCTION
	return false;
}

//自动出牌
bool CGameClientDlg::AutomatismOutCard()
{
	__ENTER_FUNCTION
	
	//先出牌者
	if (m_bTurnCardCount==0)
	{
		//控制界面
		KillGameTimer(IDI_OUT_CARD);
		SafeShowOrHideWnd(&m_GameClientView.m_btOutCard, SW_HIDE);
		SafeEnableWnd(&m_GameClientView.m_btOutCard,FALSE);
		SafeShowOrHideWnd(&m_GameClientView.m_btPassCard, SW_HIDE);
		SafeEnableWnd(&m_GameClientView.m_btPassCard,FALSE);
		SafeShowOrHideWnd(&m_GameClientView.m_btAutoOutCard, SW_HIDE);
		SafeEnableWnd(&m_GameClientView.m_btAutoOutCard,FALSE);

		WORD wMeChairID=GetMeChairID();

		//发送数据
		CMD_C_OutCard OutCard;
		OutCard.bCardCount=1;
		OutCard.bCardData[0]=m_bHandCardData[wMeChairID][m_bHandCardCount[wMeChairID]-1];
		SendData(SUB_C_OUT_CART,&OutCard,sizeof(CMD_C_OutCard));

		//预先处理
		PlaySound("OUT_CARD",false);
		m_GameClientView.m_UserCardControl[1].SetCardData(OutCard.bCardData,OutCard.bCardCount);

		//预先删除
		BYTE bSourceCount=m_bHandCardCount[wMeChairID];
		m_bHandCardCount[wMeChairID]-=OutCard.bCardCount;
		m_GameLogic.RemoveCard(OutCard.bCardData,OutCard.bCardCount,m_bHandCardData[wMeChairID],bSourceCount);
		//m_GameClientView.m_HandCardControl[1].SetCardData(m_bHandCardData[wMeChairID],m_bHandCardCount[wMeChairID]);
		m_GameClientView.m_GameCard.SetCardData(m_bHandCardData[wMeChairID],m_bHandCardCount[wMeChairID]);

	}
	else OnPassCard(1,1);

	__LEAVE_FUNCTION
	return true;
}

//停止发牌
bool CGameClientDlg::DoncludeDispatchCard()
{
	__ENTER_FUNCTION
	
	//停止发牌
	if (m_cbRemnantCardCount>0)
	{
		//删除时间
		KillTimer(IDI_DISPATCH_CARD);

		//设置界面
		for (WORD i=0;i<GAME_PLAYER;i++)
		{
			WORD wViewChairID = SwitchViewChairID(i) ;
			if (wViewChairID!=1)
			{
				BYTE cbCardData[20] ;
				ZeroMemory(cbCardData, sizeof(cbCardData)) ;
				m_GameClientView.m_HandCardControl[wViewChairID].SetCardData(cbCardData,17);
			}
			else{
				WORD wMeChairID = GetMeChairID();
				//m_GameClientView.m_HandCardControl[1].SetCardData(m_bHandCardData[wMeChairID],m_bHandCardCount[wMeChairID]); 
				m_GameClientView.m_GameCard.SetCardData(m_bHandCardData[wMeChairID],m_bHandCardCount[wMeChairID]);
			}
		}

		//设置变量
		m_cbRemnantCardCount=0;
		m_cbDispatchCardCount=0;
		ZeroMemory(m_cbDispatchCardData,sizeof(m_cbDispatchCardData));
	}

	__LEAVE_FUNCTION
	return true;
}

//派发扑克
bool CGameClientDlg::DispatchUserCard(BYTE cbCardData[], BYTE cbCardCount)
{
	__ENTER_FUNCTION
		
	//设置变量
	m_cbDispatchCardCount=0;
	m_cbRemnantCardCount=cbCardCount;
	CopyMemory(m_cbDispatchCardData,cbCardData,sizeof(BYTE)*cbCardCount);

	//设置定时器
	SetTimer(IDI_DISPATCH_CARD,120,NULL);

	__LEAVE_FUNCTION
	return true;
}

//定时器消息
void CGameClientDlg::OnTimer(UINT nIDEvent)
{
	__ENTER_FUNCTION
		
	if ((nIDEvent==IDI_MOST_CARD)&&(m_wMostUser!=INVALID_CHAIR))
	{
		//变量定义
		WORD wCurrentUser=m_wMostUser;
		m_wMostUser=INVALID_CHAIR;

		//删除定时器
		KillTimer(IDI_MOST_CARD);

		//设置界面
		m_GameClientView.SetPassFlag(INVALID_CHAIR,false);
		for (WORD i=0;i<GAME_PLAYER;i++) m_GameClientView.m_UserCardControl[i].SetCardData(NULL,0);

		//玩家设置
		if ((IsLookonMode()==false)&&(wCurrentUser==GetMeChairID()))
		{
			ActiveGameFrame();
			SafeShowOrHideWnd(&m_GameClientView.m_btOutCard, SW_SHOW);
			SafeShowOrHideWnd(&m_GameClientView.m_btPassCard, SW_SHOW);
			SafeEnableWnd(&m_GameClientView.m_btPassCard,FALSE);
			SafeShowOrHideWnd(&m_GameClientView.m_btAutoOutCard, SW_SHOW);
			SafeEnableWnd(&m_GameClientView.m_btAutoOutCard,FALSE);
			SafeEnableWnd(&m_GameClientView.m_btOutCard,(VerdictOutCard()==true)?TRUE:FALSE);
		}

		//设置时间
		SetGameTimer(wCurrentUser,IDI_OUT_CARD,15);

		return;
	}

	//派发扑克
	if (nIDEvent==IDI_DISPATCH_CARD)
	{
		//派发控制
		if (m_cbRemnantCardCount>0)
		{
			//扑克数据
			BYTE cbUserCard[20];
			ZeroMemory(cbUserCard,sizeof(cbUserCard));

			//派发扑克
			m_cbRemnantCardCount--;
			m_cbDispatchCardCount++;
			m_GameLogic.SortCardList(m_cbDispatchCardData,m_cbDispatchCardCount,ST_ORDER);

			//设置界面
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				WORD wViewID = SwitchViewChairID(i) ;
				if (wViewID!=1)
				{
					m_GameClientView.m_HandCardControl[wViewID].SetCardData(m_bHandCardData[i],m_cbDispatchCardCount);
					//m_GameClientView.m_HandCardControl[wViewID].ShowWindow(SW_HIDE);
				}
				else
				{
					m_GameClientView.m_GameCard.SetCardData(m_cbDispatchCardData,m_cbDispatchCardCount); 
					m_GameClientView.m_GameCard.SetCardShow(true);
				}
			}

			//播放声音
			PlaySound("DISPATCH_CARD",false);
		}

		//中止判断
		if (m_cbRemnantCardCount==0)
		{
			//删除定时器
			KillTimer(IDI_DISPATCH_CARD);

			SetTaskWndShow(true);

			//发完牌后显示等待叫分字样
			m_GameClientView.ShowLandTitle(true);

			//扑克数目
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				if (IsLookonMode()==false)
				{
					m_GameClientView.SetCardCount(SwitchViewChairID(i),m_bCardCount[i]);
				}		
			}

			//当前玩家
			if ((IsLookonMode()==false)&&(m_wCurrentUser==GetMeChairID()))
			{
				ActiveGameFrame();
				SafeEnableWnd(&m_GameClientView.m_btCallLand,TRUE);
				SafeShowOrHideWnd(&m_GameClientView.m_btCallLand, SW_SHOW);
				SafeEnableWnd(&m_GameClientView.m_btGiveUpCallLand,TRUE);
				SafeShowOrHideWnd(&m_GameClientView.m_btGiveUpCallLand, SW_SHOW);
				//m_GameClientView.m_btGiveUpScore.LoadRgnImage(AfxGetInstanceHandle(),IDB_GIVE_UP,RGB(255,0,255));
			}
			//设置界面
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				WORD wViewID = SwitchViewChairID(i) ;
				if (wViewID!=1)
				{
					BYTE cbCardData[20] ;
					ZeroMemory(cbCardData, sizeof(cbCardData)) ;
					m_GameClientView.m_HandCardControl[wViewID].SetCardData(m_bHandCardData[i],17);
				}
				else 
				{
					WORD wMeChairID = GetMeChairID();
					m_GameClientView.m_HandCardControl[wViewID].SetCardData(m_bHandCardData[wMeChairID],m_bHandCardCount[wMeChairID]); 
					if(false==IsLookonMode())
					{
						m_GameClientView.m_HandCardControl[wViewID].SetDisplayFlag(true) ;
						m_GameClientView.m_HandCardControl[wViewID].SetPositively(true) ;
					}
				}
			}
			if(IsLookonMode()==false) 
			{
				SafeShowOrHideWnd(&m_GameClientView.m_btSortCard, SW_SHOW);
				SafeEnableWnd(&m_GameClientView.m_btSortCard,TRUE);
				SafeShowOrHideWnd(&m_GameClientView.m_btAutoPlayOn, SW_SHOW);
				SafeEnableWnd(&m_GameClientView.m_btAutoPlayOn,FALSE);
				//m_GameClientView.m_btBrightCard.ShowWindow( SW_HIDE );
			}
		   //播放声音
			PlaySound("GAME_START",false);

			//设置时间
			if (m_wCurrentUser!=INVALID_CHAIR) SetGameTimer(m_wCurrentUser,IDI_LAND_SCORE,10);
		}
		return;
	}
	__super::OnTimer(nIDEvent);
	__LEAVE_FUNCTION
}

//开始按钮
LRESULT CGameClientDlg::OnStart(WPARAM wParam, LPARAM lParam)
{
	__ENTER_FUNCTION
			
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
        m_GameClientView.SetHandCardData(NULL, 0, i);
	}	
	//设置变量
	m_bAutoPlay=false;
	m_wBombTime=1;
	m_wTimeOutCount=0;
	m_bSystemTrustee=false;
	m_bTurnCardCount=0;
	m_bTurnOutType=CT_ERROR;
	m_wMostUser=INVALID_CHAIR;
	memset(m_bHandCardData,0,sizeof(m_bHandCardData));
	memset(m_bTurnCardData,0,sizeof(m_bTurnCardData));
	memset(m_bHandCardCount,0,sizeof(m_bHandCardCount));

	//设置界面
	KillGameTimer(IDI_START_GAME);
	KillGameTimer(IDI_ENDSTART_GAME);

	m_GameClientView.ShowLandTitle(false);
	m_GameClientView.SetBombTime(0);
	m_GameClientView.SetCardCount(INVALID_CHAIR,0);
	m_GameClientView.SetLandUser(INVALID_CHAIR,0);
	m_GameClientView.SetLandScore(INVALID_CHAIR,0,CSD_NORMAL,CSD_NORMAL);
	m_GameClientView.SetPassFlag(INVALID_CHAIR,false);
	m_GameClientView.ShowLandTime(true);
	m_GameClientView.SetMeBeiShu(0);

	//隐藏控件
	SafeShowOrHideWnd(&m_GameClientView.m_btStart, SW_HIDE);

	//if(m_GameClientView.m_btBrightStart.GetSafeHwnd())	// add by HouGuoJiang 2011-11-25
	//	m_GameClientView.m_btBrightStart.ShowWindow(SW_HIDE);
	/*m_ScoreView.ShowWindow(SW_HIDE);*/

	//隐藏扑克
//	m_GameClientView.m_HandCardControl[ 0 ].ShowWindow( SW_HIDE );
//	m_GameClientView.m_HandCardControl[ 2 ].ShowWindow( SW_HIDE );
	m_GameClientView.SetCardBackDisPlay(false);

	//设置扑克
	m_GameClientView.SetCardBackData(NULL,0);
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		m_GameClientView.m_HandCardControl[i].SetCardData(NULL,0);
		m_GameClientView.m_UserCardControl[i].SetCardData(NULL,0);
	}
    //清空自己手牌数据
	m_GameClientView.m_GameCard.SetCardData(NULL,0);

	m_IsFanSpring=false;
	m_IsSpring=false;
	memset(m_LargessCount,0,sizeof(m_LargessCount));
	m_bchuntian=false;
	m_bfanchun=false;
	m_bbaoshi=false;
	m_bjieshu=false;

	//发送消息
	SendUserReady(NULL,0);


	int iCount = GetCurGameUserNums();
	PlayWaitFlash(false);
	if (GAME_PLAYER != iCount)
	{
		PlayWaitFlash(true);
	}

	SetTaskWndShow(false);

	__LEAVE_FUNCTION
	return 0;
}

//出牌消息
LRESULT CGameClientDlg::OnOutCard(WPARAM wParam, LPARAM lParam)
{
	__ENTER_FUNCTION
	
	//状态判断
	if ((m_GameClientView.m_btOutCard.IsWindowEnabled()==FALSE)||
		(m_GameClientView.m_btOutCard.IsWindowVisible()==FALSE)) return 0;

	//设置界面
	KillGameTimer(IDI_OUT_CARD);
	SafeShowOrHideWnd(&m_GameClientView.m_btOutCard, SW_HIDE);
	SafeEnableWnd(&m_GameClientView.m_btOutCard,FALSE);
	SafeShowOrHideWnd(&m_GameClientView.m_btPassCard, SW_HIDE);
	SafeEnableWnd(&m_GameClientView.m_btPassCard,FALSE);
	SafeShowOrHideWnd(&m_GameClientView.m_btAutoOutCard, SW_HIDE);
	SafeEnableWnd(&m_GameClientView.m_btAutoOutCard,FALSE);

	if (1 == (BYTE)wParam)
	{
		//重置变量
		m_wTimeOutCount=0;
	}

	//发送数据
	CMD_C_OutCard OutCard;
	//OutCard.bCardCount=(BYTE)m_GameClientView.m_HandCardControl[1].GetShootCard(OutCard.bCardData,CountArray(OutCard.bCardData));
	OutCard.bCardCount=(BYTE)m_GameClientView.m_GameCard.GetShootCard(OutCard.bCardData,CountArray(OutCard.bCardData));
	SendData(SUB_C_OUT_CART,&OutCard,sizeof(CMD_C_OutCard));

	//预先显示
	//BYTE cbOutCardType = m_GameLogic.GetCardType(OutCard.bCardData, OutCard.bCardCount );
	//if ( cbOutCardType == CT_BOMB_CARD )
	//	PlayGameSound(AfxGetInstanceHandle(),TEXT("BOMB_CARD"));
	//else if ( cbOutCardType == CT_MISSILE_CARD )
	//	PlayGameSound(AfxGetInstanceHandle(),TEXT("MOST_CARD"));
	//else
	//	PlayGameSound(AfxGetInstanceHandle(),TEXT("OUT_CARD"));
	m_GameClientView.m_UserCardControl[1].SetCardData(OutCard.bCardData,OutCard.bCardCount);

	//预先删除
	WORD wMeChairID=GetMeChairID();
	BYTE bSourceCount=m_bHandCardCount[wMeChairID];
	m_bHandCardCount[wMeChairID]-=OutCard.bCardCount;
	m_GameLogic.RemoveCard(OutCard.bCardData,OutCard.bCardCount,m_bHandCardData[wMeChairID],bSourceCount);
	//m_GameClientView.m_HandCardControl[1].SetCardData(m_bHandCardData[wMeChairID],m_bHandCardCount[wMeChairID]);
	m_GameClientView.m_GameCard.SetCardData(m_bHandCardData[wMeChairID],m_bHandCardCount[wMeChairID]);
   
	m_GameClientView.UpdateGameView(NULL);

	__LEAVE_FUNCTION
	return 0;
}

//不点击PASS按钮,直接出牌。
void CGameClientDlg::NoHitPassOutCard()
{
	__ENTER_FUNCTION

	//设置界面
	KillGameTimer(IDI_OUT_CARD);
	SafeShowOrHideWnd(&m_GameClientView.m_btOutCard, SW_HIDE);
	SafeEnableWnd(&m_GameClientView.m_btOutCard,FALSE);
	SafeShowOrHideWnd(&m_GameClientView.m_btPassCard, SW_HIDE);
	SafeEnableWnd(&m_GameClientView.m_btPassCard,FALSE);
	SafeShowOrHideWnd(&m_GameClientView.m_btAutoOutCard, SW_HIDE);
	SafeEnableWnd(&m_GameClientView.m_btAutoOutCard,FALSE);

	//发送数据
	SendData(SUB_C_PASS_CARD);

	//预先显示
	m_GameClientView.SetPassFlag(1,true);
	m_GameClientView.SetShowBigness(false);
	m_GameClientView.UpdateGameView(NULL);
	__LEAVE_FUNCTION
}

//放弃出牌
LRESULT CGameClientDlg::OnPassCard(WPARAM wParam, LPARAM lParam)
{
	__ENTER_FUNCTION

	//状态判断
	if (m_GameClientView.m_btPassCard.IsWindowEnabled()==FALSE) return 0;
    WORD wMeChairID = GetMeChairID();

	if (1 == (BYTE)wParam)
	{
		//重置变量
		m_wTimeOutCount=0;
	}

	//设置界面
	KillGameTimer(IDI_OUT_CARD);
	SafeShowOrHideWnd(&m_GameClientView.m_btOutCard, SW_HIDE);
	SafeEnableWnd(&m_GameClientView.m_btOutCard,FALSE);
	SafeShowOrHideWnd(&m_GameClientView.m_btPassCard, SW_HIDE);
	SafeEnableWnd(&m_GameClientView.m_btPassCard,FALSE);
	SafeShowOrHideWnd(&m_GameClientView.m_btAutoOutCard, SW_HIDE);
	SafeEnableWnd(&m_GameClientView.m_btAutoOutCard,FALSE);

	//发送数据
	SendData(SUB_C_PASS_CARD);

	//预先显示
	m_GameClientView.SetPassFlag(1,true);
	//PlayGameSound(AfxGetInstanceHandle(),TEXT("PASS_CARD"));
	
	//m_GameClientView.m_HandCardControl[1].SetCardData(m_bHandCardData[wMeChairID],m_bHandCardCount[wMeChairID]);
    m_GameClientView.m_GameCard.SetCardData(m_bHandCardData[wMeChairID],m_bHandCardCount[wMeChairID]);
	m_GameClientView.SetShowBigness(false);
	m_GameClientView.UpdateGameView(NULL);

	__LEAVE_FUNCTION
	return 0;
}

//叫地主消息
LRESULT CGameClientDlg::OnCallLand(WPARAM wParam, LPARAM lParam)
{
	__ENTER_FUNCTION

	//设置定时器
	KillGameTimer(IDI_LAND_SCORE);

	//设置界面	
	SafeShowOrHideWnd(&m_GameClientView.m_btCallLand, SW_HIDE);
	SafeShowOrHideWnd(&m_GameClientView.m_btGiveUpCallLand, SW_HIDE);

	//发送数据
	CMD_C_LandScore LandScore;
	LandScore.bLandScore=(BYTE)wParam;
	SendData(SUB_C_LAND_SCORE,&LandScore,sizeof(LandScore));

	__LEAVE_FUNCTION
	return 0;
}
//抢地主消息
LRESULT CGameClientDlg::OnSnatchLand(WPARAM wParam,LPARAM lParam)
{
	//设置定时器
	KillGameTimer(IDI_SNATCH_LAND);

	//设置界面
	SafeShowOrHideWnd(&m_GameClientView.m_btSnatchLand,SW_HIDE);
	SafeShowOrHideWnd(&m_GameClientView.m_btGiveUpSnatchLand,SW_HIDE);

	//发送数据
	CMD_C_SnatchLand SnatchLand;
	SnatchLand.bSnatchLand=(BYTE)wParam;
	SendData(SUB_C_SNATCH_SCORE,&SnatchLand,sizeof(SnatchLand));

	return 0;
}

//出牌提示
//LRESULT CGameClientDlg::OnAutoOutCard(WPARAM wParam, LPARAM lParam)
//{
//	//分析结果
//	tagOutCardResult OutCardResult;
//	WORD wMeChairID = GetMeChairID();
//	m_GameLogic.SearchOutCard(m_bHandCardData[wMeChairID],m_bHandCardCount[wMeChairID],m_bTurnCardData,m_bTurnCardCount,OutCardResult);
	//设置界面
//	if (OutCardResult.cbCardCount>0)
//	{
//		//设置界面
//		m_GameClientView.m_HandCardControl[1].SetShootCard(OutCardResult.cbResultCard,OutCardResult.cbCardCount);
//
//		//设置控件
//		bool bOutCard=VerdictOutCard();
//		if(false == bOutCard) OnPassCard(0,0);
//		m_GameClientView.m_btOutCard.EnableWindow((bOutCard==true)?TRUE:FALSE);
//	}
//	else 
//	{
//		//放弃出牌
//		OnPassCard(0,0);
//	}
//
//	return 0;
//}
//出牌提示
LRESULT CGameClientDlg::OnAutoOutCard(WPARAM wParam, LPARAM lParam)
{
	__ENTER_FUNCTION
	
	bool bIsSearchOutCard = false;
	tagOutCardResult OutCardResult;
	WORD wMeChairID = GetMeChairID();
	WORD wViewChairID = SwitchViewChairID(wMeChairID);
	m_GameLogic.SearchOutCard(m_bHandCardData[wMeChairID],m_bHandCardCount[wMeChairID],m_tagOldCard.cbResultCard,m_tagOldCard.cbCardCount,OutCardResult);


	if (OutCardResult.cbCardCount>0)
	{
		bIsSearchOutCard = true;
	}
	else if (m_tagNewCard.cbCardCount > 0)
	{
		m_tagOldCard.cbCardCount = m_bTurnCardCount;
		CopyMemory(m_tagOldCard.cbResultCard, m_bTurnCardData, m_bTurnCardCount);
		::memset(&m_tagNewCard, 0, sizeof(m_tagNewCard));

		m_GameLogic.SearchOutCard(m_bHandCardData[wMeChairID],m_bHandCardCount[wMeChairID],m_tagOldCard.cbResultCard,m_tagOldCard.cbCardCount,OutCardResult);
		bIsSearchOutCard = true;
	}
	else
	{
		OnPassCard(1,1);
	}

	if (bIsSearchOutCard)
	{
		m_tagNewCard=OutCardResult;
		m_tagOldCard=m_tagNewCard;
		//设置界面
		m_GameClientView.m_GameCard.SetShootCard(OutCardResult.cbResultCard,OutCardResult.cbCardCount);

		//设置控件
		bool bOutCard=VerdictOutCard();
		if(false == bOutCard)
		{
            OnPassCard(1,1);
		}
		SafeEnableWnd(&m_GameClientView.m_btOutCard,(bOutCard==true)?TRUE:FALSE);
	}

	__LEAVE_FUNCTION
	return 0;
}


//左键扑克
LRESULT CGameClientDlg::OnLeftHitCard(WPARAM wParam, LPARAM lParam)
{
	__ENTER_FUNCTION

	//设置控件
	bool bOutCard=VerdictOutCard();
	SafeEnableWnd(&m_GameClientView.m_btOutCard,bOutCard?TRUE:FALSE);

	__LEAVE_FUNCTION
	return 0;
}

//右键扑克
LRESULT CGameClientDlg::OnRightHitCard(WPARAM wParam, LPARAM lParam)
{		
	__ENTER_FUNCTION

	//用户出牌
	OnOutCard(1,1);

	__LEAVE_FUNCTION
	return 0;
}

//////////////////////////////////////////////////////////////////////////

LRESULT CGameClientDlg::OnAutoPlay(WPARAM wParam, LPARAM lParam)
{
	__ENTER_FUNCTION

	if (IsLookonMode())
	{
		return 0;
	}
    bool bAutoPlay = (wParam == 1);
    if (bAutoPlay == m_bAutoPlay)
        return 0;

    m_bAutoPlay = bAutoPlay;
	//m_bSystemTrustee=bAutoPlay;

    if (m_bAutoPlay)
	{
		InsertSystemString("使用托管功能, 系统进入托管状态!");
	}	
    else
	{
		m_bSystemTrustee=false;
		InsertSystemString("取消托管功能. ");
	}
	//发送消息
	CMD_C_UserTrustee UserTrustee;
	ZeroMemory( &UserTrustee, sizeof( UserTrustee ) );
	UserTrustee.bTrustee = m_bAutoPlay;
	UserTrustee.wUserChairID = GetMeChairID();

	SendData( SUB_C_TRUSTEE, &UserTrustee, sizeof( UserTrustee ) );

	__LEAVE_FUNCTION
    return 0;
}


LRESULT CGameClientDlg::OnMessageSortCard(WPARAM wParam, LPARAM lParam)
{
	__ENTER_FUNCTION

	//排列扑克
	WORD wMeChairID=GetMeChairID();
	m_GameLogic.SortCardList(m_bHandCardData[wMeChairID],m_bHandCardCount[wMeChairID],m_cbSortType);

	//获取扑克
	//BYTE cbShootCount=(BYTE)m_GameClientView.m_HandCardControl[1].SetCardData(m_bHandCardData[wMeChairID],m_bHandCardCount[wMeChairID]);
	BYTE cbShootCount=(BYTE)m_GameClientView.m_GameCard.SetCardData(m_bHandCardData[wMeChairID],m_bHandCardCount[wMeChairID]);


	//设置变量
	m_cbSortType=(m_cbSortType==ST_ORDER)?ST_COUNT:ST_ORDER;

	//设置按钮
	UINT uImageID=(m_cbSortType==ST_ORDER)?IDB_ORDER_SORT:IDB_COUNT_SORT;

	//m_GameClientView.m_btSortCard.LoadRgnImage(AfxGetInstanceHandle(),uImageID,RGB(255,0,255));

	//更新界面
	m_GameClientView.UpdateGameView(NULL);

	__LEAVE_FUNCTION
	return 0;
}

//明牌开始
LRESULT CGameClientDlg::OnBrightStart(WPARAM wParam, LPARAM lParam)
{
	//界面设置
	m_GameClientView.ShowLandTime(true);

	//发送消息
	SendData( SUB_C_BRIGHT_START);

	//开始消息
	SendMessage(IDM_START, 0, 0 );

	//播放声音
	//PlayGameSound(AfxGetInstanceHandle(),TEXT("BRIGHT_CARD"));

	return 0;
}

bool CGameClientDlg::OnSubBrightStart(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_BrightStart));
	if (wDataSize != sizeof(CMD_S_BrightStart)) return false;

	//变量定义
	CMD_S_BrightStart *pBrightStart = ( CMD_S_BrightStart * )pBuffer;

	//设置界面
	WORD wViewID = SwitchViewChairID(pBrightStart->wBrightUser) ;
	m_GameClientView.SetBrightStart(wViewID, pBrightStart->bBright);
	if ( pBrightStart->bBright ) m_GameClientView.SetBrightTime(BRIGHT_START_TIME);

	return true;
}

//玩家明牌
bool CGameClientDlg::OnSubBrightCard(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_BrightCard));
	if (wDataSize != sizeof(CMD_S_BrightCard)) return false;

	//变量定义
	CMD_S_BrightCard *pBrightCard = ( CMD_S_BrightCard * )pBuffer;

	if ( pBrightCard->cbCallScorePhase == CSD_BRIGHTCARD )
	{
		//椅子转换
		WORD wViewChairID = SwitchViewChairID(pBrightCard->wBrightUser);

		//清除界面
		m_GameClientView.m_bScore[wViewChairID] = 0 ;
		m_GameClientView.m_bPass[wViewChairID] = false;
		m_GameClientView.m_bDoubleScore[wViewChairID] = false;

		if ( pBrightCard->cbBrightTime != 255 )
		{
			//设置界面
			m_GameClientView.SetBrightTime(pBrightCard->cbCurrenBrightTime);
			m_GameClientView.SetBrightStart(wViewChairID, true);

			//设置扑克
			m_GameClientView.m_HandCardControl[wViewChairID].ShowWindow(SW_SHOW);
			m_GameClientView.m_HandCardControl[wViewChairID].SetDisplayFlag(true);
		}
		else
		{
			m_GameClientView.m_cbUserPhase[wViewChairID] = CSD_BRIGHTCARD;	
			m_GameClientView.m_bScore[wViewChairID] = 255 ;
		}

		m_GameClientView.UpdateGameView(NULL);

		return true;
	}
	else
	{
		//设置界面
		m_GameClientView.SetBrightTime(pBrightCard->cbCurrenBrightTime);
		WORD wViewChairID = SwitchViewChairID(pBrightCard->wBrightUser);
		m_GameClientView.SetBrightStart(wViewChairID, true);

		//设置扑克
		m_GameClientView.m_HandCardControl[wViewChairID].ShowWindow(SW_SHOW);
		m_GameClientView.m_HandCardControl[wViewChairID].SetDisplayFlag(true);

		return true;
	}
	return true;
}

//玩家托管
bool CGameClientDlg::OnSubPlayerTrustee(const void * pBuffer, WORD wDataSize)
{
	__ENTER_FUNCTION
	
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_C_UserTrustee));
	if (wDataSize != sizeof(CMD_C_UserTrustee)) return false;

	CMD_C_UserTrustee *pUserTrustee = ( CMD_C_UserTrustee * )pBuffer;

	m_GameClientView.SetUserTrustee( SwitchViewChairID( pUserTrustee->wUserChairID ), pUserTrustee->bTrustee );

	//显示托管
	if ( pUserTrustee->wUserChairID != GetMeChairID() || IsLookonMode() ) {

		CString strMsg;
		if ( pUserTrustee->bTrustee )
			strMsg.Format(TEXT("玩家选择了托管"));
		else
			strMsg.Format(TEXT("玩家取消了托管"));
		InsertSystemString( strMsg );
	}
	m_GameClientView.UpdateGameView(NULL);

	__LEAVE_FUNCTION
	return true;
}

//加倍消息 (叫分完成)
bool CGameClientDlg::OnSubDoubleScore(const void * pBuffer, WORD wDataSize)
{
	__ENTER_FUNCTION
	
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_DoubleScore));
	if (wDataSize != sizeof(CMD_S_DoubleScore)) return false;

	//变量定义
	CMD_S_DoubleScore *pDoubleScore = ( CMD_S_DoubleScore * )pBuffer;

	//关闭按钮
	//SafeShowOrHideWnd(&m_GameClientView.m_btGiveUpScore, SW_HIDE);
	//SafeShowOrHideWnd(&m_GameClientView.m_btOneScore, SW_HIDE);
	//SafeShowOrHideWnd(&m_GameClientView.m_btTwoScore, SW_HIDE);
	//SafeShowOrHideWnd(&m_GameClientView.m_btThreeScore, SW_HIDE);

	//cxf 删除加倍按钮  用(倒-跟-反)代替
	//设置按钮
	//bool bAllowDouble = pDoubleScore->bAllowDouble;
	//m_GameClientView.m_btDoubleScore.ShowWindow( SW_SHOW );
	//m_GameClientView.m_btDoubleScore.EnableWindow( bAllowDouble ? TRUE : FALSE );
	//m_GameClientView.m_btNotDoubleScore.ShowWindow( SW_SHOW );
	//if ((pDoubleScore->wLandUser+1)%GAME_PLAYER == GetMeChairID())
	//{
	//	SafeShowOrHideWnd(&m_GameClientView.m_btCardDao, SW_SHOW);
	//}
	//cxf end
	//设置控件
	m_GameClientView.SetCardBackData(pDoubleScore->bBackCard,CountArray(pDoubleScore->bBackCard));
	m_GameClientView.SetCardBackDisPlay(true);
	m_GameClientView.SetBrightStart(INVALID_CHAIR, false);

	//设置界面
	m_bCardCount[pDoubleScore->wLandUser]=20;
	//m_GameClientView.SetLandScore(INVALID_CHAIR,0,CSD_GAMESTART,CSD_GAMESTART);
	if (IsLookonMode()==false)
	{
		m_GameClientView.SetCardCount(SwitchViewChairID(pDoubleScore->wLandUser),m_bCardCount[pDoubleScore->wLandUser]);
	}
	WORD wViewChairID=SwitchViewChairID(pDoubleScore->wLandUser);
	//m_GameClientView.SetLandUser(wViewChairID, pDoubleScore->bCurrentScore);

	//地主设置
	WORD wLandUser=pDoubleScore->wLandUser;
	BYTE bCardCound=m_bHandCardCount[wLandUser];
	m_bHandCardCount[wLandUser]+=CountArray(pDoubleScore->bBackCard);
	CopyMemory(&m_bHandCardData[wLandUser][bCardCound],pDoubleScore->bBackCard,sizeof(pDoubleScore->bBackCard));
	m_GameLogic.SortCardList(m_bHandCardData[wLandUser],m_bHandCardCount[wLandUser],ST_ORDER);
	//m_GameClientView.m_HandCardControl[SwitchViewChairID(wLandUser)].SetCardData(m_bHandCardData[wLandUser],m_bHandCardCount[wLandUser]);
	if(pDoubleScore->wLandUser == GetMeChairID())
	{
		m_GameClientView.m_GameCard.SetCardData(m_bHandCardData[wLandUser],m_bHandCardCount[wLandUser]);
	}

	//设置消息
	//SetGameTimer(GetMeChairID(), IDI_DOUBLE_SCORE, 30);
	m_GameClientView.UpdateGameView(NULL);

	__LEAVE_FUNCTION
	return true;
}

//加倍消息 
LRESULT CGameClientDlg::OnDoubleScore(WPARAM wParam, LPARAM lParam)
{
	__ENTER_FUNCTION

	//cxf 删除加倍按钮  用(倒-跟-反)代替
	//加倍判断
	//bool bDoubleScore = (wParam == 0) ? true : false;
	bool bDoubleScore = (wParam > 0) ? true : false;

	//设置界面
	//m_GameClientView.m_btDoubleScore.ShowWindow( SW_HIDE );
	//m_GameClientView.m_btNotDoubleScore.ShowWindow( SW_HIDE );
	/*SafeShowOrHideWnd(&m_GameClientView.m_btCardDao, SW_HIDE);
	SafeShowOrHideWnd(&m_GameClientView.m_btCardGen, SW_HIDE);
	SafeShowOrHideWnd(&m_GameClientView.m_btCardFan, SW_HIDE);*/

	if(IsLookonMode()) return 0;
    //cxf end

	//定义消息
	CMD_C_DoubleScore DoubleScore;
	ZeroMemory( &DoubleScore, sizeof( DoubleScore ) );
	DoubleScore.bDoubleScore = bDoubleScore;
	DoubleScore.bDoubleType=lParam;

	//发送消息
	SendData( SUB_C_DOUBLE_SCORE, &DoubleScore, sizeof( DoubleScore ) );

	//关闭定时器
	//KillGameTimer(IDI_DOUBLE_SCORE);

	__LEAVE_FUNCTION
	return 0;
}

//玩家加倍
bool CGameClientDlg::OnSubUserDouble(const void * pBuffer, WORD wDataSize)
{
	__ENTER_FUNCTION

	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_UserDouble));
	if (wDataSize != sizeof(CMD_S_UserDouble)) return false;

	//变量定义
	CMD_S_UserDouble *pUserDouble = ( CMD_S_UserDouble * )pBuffer;

	//SafeShowOrHideWnd(&m_GameClientView.m_btCardDao, SW_HIDE);
	//SafeShowOrHideWnd(&m_GameClientView.m_btCardGen, SW_HIDE);
	//SafeShowOrHideWnd(&m_GameClientView.m_btCardFan, SW_HIDE);

	//cxf  显示跟或反()
	//if (IsLookonMode() == false && pUserDouble->bNextUser == GetMeChairID())
	//{
	//	switch (pUserDouble->bState)
	//	{
	//	case 1 : //倒牌
	//		{
	//			SafeShowOrHideWnd(&m_GameClientView.m_btCardDao, SW_SHOW);
	//			break;
	//		}
	//	case 2 : //跟牌
	//		{
	//			SafeShowOrHideWnd(&m_GameClientView.m_btCardGen, SW_SHOW);
	//			break;
	//		}
	//	case 3 : //地主反牌
	//		{
	//			SafeShowOrHideWnd(&m_GameClientView.m_btCardFan, SW_SHOW);
	//			break;
	//		}
	//	default :
	//		break;
	//	}
	//}

	const tagUserData *pUserData = GetUserData(pUserDouble->wDoubleUser);//用户信息
	WORD wMeChairID=GetMeChairID();
	CString strPath ="";
	GetPicPath(strPath);
	//if (1==pUserDouble->bDoubleType)
	//{//倒
	//	PlayGameSoundLanguage(pUserData->cbGender, TEXT("DAO"),false);
	//	PlaySound("BEI",false);
	////播放动画
	//	strPath.Append("IMAGE_DAO.png");
	//	m_GameClientView.m_DoubleAction.SetActionImage(strPath,true,15);
	//	m_GameClientView.PlayDoubelFlash();
	//	
	//} 
	//else if(2==pUserDouble->bDoubleType)
	//{//跟
	//	PlayGameSoundLanguage(pUserData->cbGender, TEXT("GEN"),false);
	//	PlaySound("BEI",false);
	//  //播放动画
	//	strPath.Append("IMAGE_GEN.png");
	//	m_GameClientView.m_DoubleAction.SetActionImage(strPath,true,15);
	//	m_GameClientView.PlayDoubelFlash();
	//	
	//}
	//else if(3==pUserDouble->bDoubleType)
	//{//反
	//	PlayGameSoundLanguage(pUserData->cbGender, TEXT("FAN"),false);
	//	PlaySound("BEI",false);
	//	//播放动画
	//	strPath.Append("IMAGE_FAN.png");
	//	m_GameClientView.m_DoubleAction.SetActionImage(strPath,true,15);
	//	m_GameClientView.PlayDoubelFlash();
	//}

	//设置界面
	WORD wViewChairID=SwitchViewChairID(m_wLandUser);
	//m_GameClientView.SetLandUser(wViewChairID, pUserDouble->bCurrentScore);
	m_GameClientView.SetDoubleScoreUser( SwitchViewChairID(pUserDouble->wDoubleUser), pUserDouble->bDoubleScore );
	m_GameClientView.SetBrightStart(wViewChairID, false);

	//设置实时倍数
    /*m_GameClientView.SetMeBeiShu(pUserDouble->bBeiShu[GetMeChairID()]);*/
	
	m_GameClientView.UpdateGameView(NULL);

	__LEAVE_FUNCTION
	return true;
}

//点击继续游戏
void CGameClientDlg::OnEventContinueGame()
{
	OnStart(0, 0);
}

//播放游戏声音
void CGameClientDlg::PlayGameSoundLanguage(BYTE cbGender, CString strSound,bool bSoundType)
{
	__ENTER_FUNCTION

	CString strGameSound="";
	//性别（播放男女声音）
	if (1==cbGender)
	{//女
		strGameSound=m_sVernacular+strSound+TEXT("_W");
	}
	else
	{//男
		strGameSound=m_sVernacular+strSound+TEXT("_M");
	}
	if(bSoundType)
	{//多种声音
		TCHAR cBuffer[10]={""};
		int iRand=rand()%4+1;
		::_snprintf(cBuffer,sizeof(cBuffer),"%d",iRand);
		strGameSound=strGameSound+"_"+cBuffer;
	}
	PlaySound(strGameSound,false);
	__LEAVE_FUNCTION
}

//播放游戏声音 9-22
void CGameClientDlg::PlaySound(CString szSoundRes, bool bLoop)
{
    __ENTER_FUNCTION

	if (!szSoundRes.IsEmpty())
	{
		TCHAR szResourcePath[MAX_PATH]={0};
		GetCurrentDirectory(MAX_PATH,szResourcePath);
		CString strPath(szResourcePath);
		strPath.Append("\\LandCrazy\\sound\\");
		strPath.Append(szSoundRes);
		strPath.Append(".mp3");

		PlayDirSound(strPath, true, bLoop);	//播放游戏音效
	}
	__LEAVE_FUNCTION
}

//播放背景音乐
void CGameClientDlg::PlayMusic(CString szSoundRes)
{
	__ENTER_FUNCTION

	if (!szSoundRes.IsEmpty())
	{
		TCHAR szResourcePath[MAX_PATH];
		GetCurrentDirectory(MAX_PATH,szResourcePath);
		CString strPath(szResourcePath);
		strPath.Append("\\LandCrazy\\sound\\");
		strPath.Append(szSoundRes);

		PlayBGMusic(strPath, 0, true);
	}
	__LEAVE_FUNCTION
}

//获取当前路径
void CGameClientDlg::GetPicPath(CString& strPath)
{
	__ENTER_FUNCTION

	strPath = "";
	TCHAR szResourcePath[MAX_PATH]={0};
	GetCurrentDirectory(MAX_PATH,szResourcePath);
	strPath.Append(szResourcePath);
	strPath.Append("\\LandCrazy\\picture\\");
    __LEAVE_FUNCTION
}

//统计人数
int CGameClientDlg::GetCurGameUserNums()
{
	int iCount = 0;
	__ENTER_FUNCTION

	for (int i=0; i<GAME_PLAYER; ++i)
	{
		const tagUserData *pUserData = GetUserData(i);
		if (NULL != pUserData)
		{
			++iCount;
		}
	}

	__LEAVE_FUNCTION
	return iCount;
}

//用户进入
void __cdecl CGameClientDlg::OnEventUserEnter(tagUserData * pUserData, WORD wChairID, bool bLookonUser)
{
	__ENTER_FUNCTION

	CGameFrameDlg::OnEventUserEnter(pUserData, wChairID, bLookonUser);
	//int iCount = GetCurGameUserNums();
	//if (GAME_PLAYER == iCount)
	//{
	//	PlayWaitFlash(false);
	//}
	__LEAVE_FUNCTION
}
//用户离开
void __cdecl CGameClientDlg::OnEventUserLeave(tagUserData * pUserData, WORD wChairID, bool bLookonUser)
{
	__ENTER_FUNCTION

	CGameFrameDlg::OnEventUserLeave(pUserData, wChairID, bLookonUser);

	int iCount = GetCurGameUserNums();
	PlayWaitFlash(false);
	if (GAME_PLAYER != iCount)
	{
		PlayWaitFlash(true);
	}
	__LEAVE_FUNCTION
}