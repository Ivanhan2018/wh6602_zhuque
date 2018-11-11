#include "Stdafx.h"
#include "GameOption.h"
#include "GameClient.h"
#include "GameClientDlg.h"
#include ".\gameclientdlg.h"

//////////////////////////////////////////////////////////////////////////

//定时器标识
#define IDI_START_GAME				200									//开始定时器
#define IDI_USER_ADD_SCORE			201									//加注定时器
#define IDI_USER_COMPARE_CARD		202									//选比牌用户定时器
#define IDI_DISABLE					203									//过滤定时器
#define IDI_CHECK_COMPARE_CARD		204									//检测比牌定时器


//时间标识
//#define TIME_START_GAME			99									//开始定时器
//#define TIME_USER_ADD_SCORE		99									//加注定时器
//#define TIME_USER_COMPARE_CARD	99									//比牌定时器
#define TIME_START_GAME				30									//开始定时器
#define TIME_USER_ADD_SCORE			30									//加注定时器
#define	TIME_USER_COMPARE_CARD		30									//比牌定时器
#define TIME_AUTO_CHECK_COMPARE_CARD	8000							//检测比牌定时器

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CGameClientDlg, CGameFrameDlg)
	ON_MESSAGE(IDM_START,OnStart)
	ON_MESSAGE(IDM_ADD_SCORE,OnAddScore)
	ON_MESSAGE(IDM_MIN_SCORE,OnMinScore)
	ON_MESSAGE(IDM_MAX_SCORE,OnMaxScore)
	ON_MESSAGE(IDM_CONFIRM,OnConfirmScore)
	ON_MESSAGE(IDM_CANCEL_ADD,OnCancelAdd)
	ON_MESSAGE(IDM_LOOK_CARD,OnLookCard)
	ON_MESSAGE(IDM_COMPARE_CARD,OnCompareCard)
	ON_MESSAGE(IDM_OPEN_CARD,OnOpenCard)
	ON_MESSAGE(IDM_GIVE_UP,OnGiveUp)
	ON_MESSAGE(IDM_SEND_CARD_FINISH,OnSendCardFinish)
	ON_MESSAGE(IDM_FALSH_CARD_FINISH,OnFlashCardFinish)
	ON_MESSAGE(IDM_COMPARE_USER,OnChooseCUser)
	ON_WM_TIMER()
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CGameClientDlg::CGameClientDlg() : CGameFrameDlg(&m_GameClientView, &m_ScoreView)
{
	//配置变量
	m_dwCardHSpace=DEFAULT_PELS;

	//用户信息
	m_wCurrentUser=INVALID_CHAIR;
	m_wBankerUser=INVALID_CHAIR;
	m_wWinnerUser=INVALID_CHAIR;

	//加注信息
	m_lMaxScore=0L;
	m_lMaxCellScore=0L;
	m_lCellScore=0L;
	m_lCurrentTimes=1L;
	m_lUserMaxScore=0L;
	ZeroMemory(m_lTableScore,sizeof(m_lTableScore));
	ZeroMemory(m_cbHandCardData,sizeof(m_cbHandCardData));

	//椅子信息
	for (int i=0;i<GAME_PLAYER;i++)
	{
		m_wLostUserID[i]=INVALID_CHAIR;
		m_wViewChairID[i]=INVALID_CHAIR;
		m_bMingZhu[i]=false;
	}

	//状态变量
	m_wBankerUser=INVALID_CHAIR;
	ZeroMemory(m_szAccounts,sizeof(m_szAccounts));
	ZeroMemory(m_cbPlayStatus,sizeof(m_cbPlayStatus));

	//声音
	m_sDialect="";
	m_wIsAddScore=0;
	return;
}

//析构函数
CGameClientDlg::~CGameClientDlg()
{
}

//初始函数
bool CGameClientDlg::InitGameFrame()
{
//	//设置图标
//	HICON hIcon=LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME));

	//方言格式
	m_sDialect.Format("%s_",m_strLanguage.c_str()); 

	m_wIsAddScore=0;

	//加载资源
	SetChatShowTag(TRUE);

	//读取配置
	m_dwCardHSpace=AfxGetApp()->GetProfileInt(TEXT("GameOption"),TEXT("CardSpace"),DEFAULT_PELS);

	//调整参数
	if ((m_dwCardHSpace>MAX_PELS)||(m_dwCardHSpace<LESS_PELS)) m_dwCardHSpace=DEFAULT_PELS;

	//配置控件
	for (int i=0;i<GAME_PLAYER;i++)
	{
		m_GameClientView.m_CardControl[i].SetCardSpace(m_dwCardHSpace);
	}

	//SetIcon(hIcon,TRUE);
	//SetIcon(hIcon,FALSE);

	//测试代码
//#ifdef _DEBUG
	//TCHAR szBuffer[32];
	//_snprintf(szBuffer,CountArray(szBuffer),TEXT("%s：%+I64d"),TEXT("玩家名字"),-5);
	//InsertGeneralString(szBuffer,RGB(0,0,255),true);
	//m_GameClientView.m_btStart.ShowWindow(5);
	//m_GameClientView.m_ScoreView.ResetScore();
	//LONGLONG lGameScore = 10;
	//LONGLONG lGameTax = 10;
	//for (WORD i=0;i<GAME_PLAYER;i++)
	//{	
	//	BYTE bTemp[3]={1,2,3};
	//	m_GameClientView.m_CardControl[i].SetCardData(bTemp,3);
	//	m_GameClientView.m_CardControl[i].SetDisplayHead(true);
	//	//设置信息
	//	m_GameClientView.m_ScoreView.SetGameTax(lGameTax,i);
	//	if (m_szAccounts[i][0]==0) m_GameClientView.m_ScoreView.SetGameScore(i,TEXT("已离开"),lGameScore);
	//	else m_GameClientView.m_ScoreView.SetGameScore(i,m_szAccounts[i],lGameScore);
	//}

	//m_GameClientView.m_ScoreView.ShowWindow(SW_SHOW);
	//m_GameClientView.m_btMinScore.ShowWindow(5);
	//BYTE bTemp[3]={1,2,3};
	//m_GameClientView.m_CardControl[2].SetCardData(bTemp,3);
	//m_GameClientView.m_CardControl[2].SetPositively(true);
	//0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,	//方块 A - K
	//0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1A,0x1B,0x1C,0x1D,	//梅花 A - K
	//0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2A,0x2B,0x2C,0x2D,	//红桃 A - K
	//0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3A,0x3B,0x3C,0x3D	//黑桃 A - K

	//BYTE bFirstTemp[3]={0x32,0x11,0x23};
	//BYTE bNextTemp[3]={0x2b,0x22,0x26};
	//if(m_GameLogic.CompareCard(bFirstTemp,bNextTemp,3))
	//	MessageBox("FirstTemp Win!");
	//else MessageBox("NextTemp Win!");
	/*
	m_GameClientView.m_ScoreView.SetGameScore(2,"abc",1122);
	m_GameClientView.m_ScoreView.SetGameScore(1,"abc",1221);
	m_GameClientView.m_ScoreView.SetGameScore(0,"ab122221c",122222221);
	m_GameClientView.m_ScoreView.SetGameScore(3,"67",11);
	for (WORD i=0;i<GAME_PLAYER;i++)m_GameClientView.m_ScoreView.SetGameTax(12222223,i);
	m_GameClientView.m_ScoreView.ShowWindow(SW_SHOW);
	m_GameClientView.m_btStart.ShowWindow(SW_SHOW);
	m_GameClientView.m_btConfirm.ShowWindow(SW_SHOW);
	m_GameClientView.m_btMaxScore.ShowWindow(SW_SHOW);
	m_GameClientView.m_btMinScore.ShowWindow(SW_SHOW);
	m_GameClientView.m_btOpenCard.ShowWindow(SW_SHOW);
	*/
	//BYTE bFirstTemp[3]={0x31,0x23,0x03};

	//for(int i=0;i<GAME_PLAYER;i++)
	//{
	//	//m_GameClientView.SetUserTableScore(2,50,999990);
	//	//m_GameClientView.m_CardControl[i].SetDisplayHead(true);
	//	m_GameClientView.m_CardControl[i].SetCardData(bFirstTemp,3);
	//}
	//m_GameClientView.m_CardControl[2].SetCardColor(2);

	//m_GameClientView.m_btConfirm.ShowWindow(SW_SHOW);
	//m_GameClientView.m_btCancel.ShowWindow(SW_SHOW);
	//m_GameClientView.m_btStart.ShowWindow(SW_SHOW);
	//m_GameClientView.m_btMinScore.ShowWindow(SW_SHOW);
	//m_GameClientView.m_btMaxScore.ShowWindow(SW_SHOW);
	//m_GameClientView.m_GoldControl.SetGoldCount(10000L);
	//m_GameClientView.m_GoldControl.SetGoldCount(10000);
	//m_GameClientView.m_GoldControl.SetGold(44000L);
	//m_GameClientView.m_GoldControl.SetGoldCount(10);
	//m_GameClientView.m_GoldControl.ShowWindow(SW_SHOW);
	//m_GameClientView.m_ScoreView.ShowWindow(SW_SHOW);
	//m_GameClientView.SetScoreInfo(100,100);

	//m_GameClientView.m_btOpenCard.ShowWindow(SW_SHOW);
	//m_GameClientView.m_btAddScore.ShowWindow(SW_SHOW);
	//m_GameClientView.m_btCompareCard.ShowWindow(SW_SHOW);
	//m_GameClientView.m_btGiveUp.ShowWindow(SW_SHOW);
	//m_GameClientView.m_btLookCard.ShowWindow(SW_SHOW);
	//m_GameClientView.m_btFollow.ShowWindow(SW_SHOW);

	//m_GameClientView.m_btMinScore.ShowWindow(5);
//#endif

	return true;
}

//重置框架
void CGameClientDlg::ResetGameFrame()
{
	//用户信息
	m_wCurrentUser=INVALID_CHAIR;
	m_wBankerUser=INVALID_CHAIR;
	m_wWinnerUser=INVALID_CHAIR;

	//加注信息
	m_lMaxScore=0L;
	m_lMaxCellScore=0L;
	m_lCellScore=0L;
	m_lCurrentTimes=1L;
	m_lUserMaxScore=0L;
	ZeroMemory(m_lTableScore,sizeof(m_lTableScore));
	ZeroMemory(m_cbHandCardData,sizeof(m_cbHandCardData));

	//椅子信息
	for (int i=0;i<GAME_PLAYER;i++)
	{
		m_wLostUserID[i]=INVALID_CHAIR;
		m_wViewChairID[i]=INVALID_CHAIR;
		m_bMingZhu[i]=false;
	}

	//状态变量
	m_wBankerUser=INVALID_CHAIR;
	ZeroMemory(m_szAccounts,sizeof(m_szAccounts));
	ZeroMemory(m_cbPlayStatus,sizeof(m_cbPlayStatus));

	return;
}

//游戏设置
void CGameClientDlg::OnGameOptionSet()
{
	//构造数据
	CGameOption GameOption;
	GameOption.m_dwCardHSpace=m_dwCardHSpace;
	GameOption.m_bEnableSound=IsEnableSound();
	GameOption.m_bAllowLookon=IsAllowUserLookon();

	//配置数据
	if (GameOption.DoModal()==IDOK)
	{
		//设置扑克
		m_dwCardHSpace=GameOption.m_dwCardHSpace;
		for (int i=0;i<GAME_PLAYER;i++) m_GameClientView.m_CardControl[i].SetCardSpace(m_dwCardHSpace);

		//设置配置
		EnableSound(GameOption.m_bEnableSound);
		AllowUserLookon(0,GameOption.m_bAllowLookon);

		//保存配置
		AfxGetApp()->WriteProfileInt(TEXT("GameOption"),TEXT("CardSpace"),m_dwCardHSpace);
	}

	return;
}

//时间消息 
bool CGameClientDlg::OnTimerMessage(WORD wChairID, UINT nElapse, UINT nTimerID)
{
	switch (nTimerID)
	{
	case IDI_START_GAME:		//开始定时器
		{
			//中止判断
			if (nElapse==0)
			{
				PostMessage(WM_CLOSE);
				//OnStart(0,0);
				return false;
			}

			//警告通知
			if ((nElapse<=5)&&(IsLookonMode()==false)) PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_WARN"));

			return true;
		}
	case IDI_USER_ADD_SCORE:	//加注定时器
		{
			//中止判断
			if (nElapse==0)
			{
				WORD wMeChairID=GetMeChairID();
				//最少下注
				if ((IsLookonMode()==false)&&(wMeChairID==m_wCurrentUser))
				{
					//自动放弃
					OnGiveUp(0,0);
				}
				return true;
			}

			//警告通知
			if ((nElapse<=5)&&(IsLookonMode()==false))
			{
				PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_WARN"));
				return true;
			}

			return true;
		}
	case IDI_DISABLE:			//过滤定时器
		{
			if (nElapse==0)
			{
				//删除定时器
				KillGameTimer(IDI_DISABLE);
			}
			return true;
		}
	case IDI_USER_COMPARE_CARD:	//选择定时器
		{
			//中止判断
			if (nElapse==0)
			{
				//删除定时器
				KillGameTimer(IDI_USER_COMPARE_CARD);
				WORD wMeChairID=GetMeChairID();

				//清理界面
				m_GameClientView.SetCompareCard(false,NULL);

				//构造变量
				CMD_C_CompareCard CompareCard;
				ZeroMemory(&CompareCard,sizeof(CompareCard));

				//查找上家
				BYTE iCompareUserID = (GAME_PLAYER+wMeChairID-1)%GAME_PLAYER;
				for (BYTE i=0; i<GAME_PLAYER; i++)
				{
					BYTE byTemplateUserID = (GAME_PLAYER+iCompareUserID-i)%GAME_PLAYER;
					const tagUserData *pUserData=GetUserData(byTemplateUserID);
					if (NULL!=pUserData && m_cbPlayStatus[byTemplateUserID])
					{
						CompareCard.wCompareUser=byTemplateUserID;
						break;
					}
				}
				//发送消息
				SendData(SUB_C_COMPARE_CARD,&CompareCard,sizeof(CMD_C_CompareCard));

				return true;
			}

			//警告通知
			if ((nElapse<=5)&&(GetMeChairID()==m_wCurrentUser)&&(IsLookonMode()==false))
			{
				PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_WARN"));
				return true;
			}

			return true;
		}
	}

	return false;
}

//旁观状态
void CGameClientDlg::OnLookonChanged(bool bLookonUser, const void * pBuffer, WORD wDataSize)
{
	//控件控制
	if(bLookonUser)
	{
		if(IsAllowLookon() && m_GameClientView.m_CardControl[m_wViewChairID[GetMeChairID()]].GetLookCard())
		{
		     m_GameClientView.m_CardControl[m_wViewChairID[GetMeChairID()]].SetDisplayHead(true);
		}
		else 
		{
		    m_GameClientView.m_CardControl[m_wViewChairID[GetMeChairID()]].SetDisplayHead(false);
		}	
	}
	return;
}

//网络消息
bool CGameClientDlg::OnGameMessage(WORD wSubCmdID, const void * pBuffer, WORD wDataSize)
{
#ifdef ZJH_GAME_PRINT_INFO
	char chInfo[100]={0};
	::sprintf(chInfo, "子消息ID:%d", wSubCmdID);
	InsertSystemString(chInfo);
#endif

	bool bRet = false;
	switch (wSubCmdID)
	{
	case SUB_S_GAME_START:		//游戏开始
		{
			//消息处理
			bRet = OnSubGameStart(pBuffer,wDataSize);
			break;
		}
	case SUB_S_ADD_SCORE:		//用户下注
		{
			//m_GameClientView.StopMoveJettons();
			m_GameClientView.FinishDispatchCard();
			m_GameClientView.StopCompareCard();
			m_GameClientView.StopFlashCard();

			//消息处理
			bRet = OnSubAddScore(pBuffer,wDataSize);
			break;
		}
	case SUB_S_LOOK_CARD:		//看牌消息
		{
			//m_GameClientView.StopMoveJettons();
			m_GameClientView.FinishDispatchCard();
			m_GameClientView.StopCompareCard();
			m_GameClientView.StopFlashCard();

			//消息处理
			bRet = OnSubLookCard(pBuffer,wDataSize);
			break;
		}
	case SUB_S_COMPARE_CARD:	//比牌消息
		{
			//m_GameClientView.StopMoveJettons();
			m_GameClientView.StopCompareCard();
			m_GameClientView.StopFlashCard();

			//消息处理
			bRet = OnSubCompareCard(pBuffer,wDataSize);
			break;
		}
	case SUB_S_OPEN_CARD:		//开牌消息
		{
			//m_GameClientView.StopMoveJettons();
			m_GameClientView.StopCompareCard();
			m_GameClientView.StopFlashCard();

			//消息处理
			bRet = OnSubOpenCard(pBuffer,wDataSize);
			break;
		}
	case SUB_S_GIVE_UP:			//用户放弃
		{
			m_GameClientView.FinishDispatchCard();
			m_GameClientView.StopCompareCard();
			m_GameClientView.StopFlashCard();

			//消息处理
			bRet = OnSubGiveUp(pBuffer,wDataSize);
			break;
		}
	case SUB_S_PLAYER_EXIT:		//用户强退
		{
			m_GameClientView.FinishDispatchCard();
			//消息处理
			bRet = OnSubPlayerExit(pBuffer,wDataSize);
			break;
		}
	case SUB_S_GAME_END:		//游戏结束
		{
			m_GameClientView.StopMoveJettons();
			m_GameClientView.FinishDispatchCard();
			m_GameClientView.StopCompareCard();
			m_GameClientView.StopFlashCard();

			//消息处理
			bRet = OnSubGameEnd(pBuffer,wDataSize);
			break;
		}
	case SUB_S_WAIT_COMPARE:	//等待比牌
		{
			//消息处理
			if (wDataSize!=sizeof(CMD_S_WaitCompare))
			{
				bRet = false;
				break;
			}
			CMD_S_WaitCompare * pWaitCompare=(CMD_S_WaitCompare *)pBuffer;	
			ASSERT(pWaitCompare->wCompareUser==m_wCurrentUser);

			WORD wMeChairID=GetMeChairID();

			if(wMeChairID!=m_wCurrentUser)
			{
				//比牌背景
				m_GameClientView.m_CardControl[m_wViewChairID[m_wCurrentUser]].SetCompareBack(true);

				SetGameTimer(m_wCurrentUser,IDI_DISABLE,TIME_USER_COMPARE_CARD);
			}

			//提示标志
			if(wMeChairID!=m_wCurrentUser || IsLookonMode())
			{
				m_GameClientView.SetWaitUserChoice(TRUE);
			}

			bRet = true;
			break;
		}
	case SUB_S_SEND_CARD:		//用户扑克
		{
			bRet = true;
			break;
		}
	}
	if (bRet)
	{
		return true;
	}
	else
	{
		//char chData[100]={0};
		//sprintf(chData, "CGameClientDlg::OnGameMessage--%d", wSubCmdID);
		//AfxMessageBox(chData);
		return true;
	}
}

//游戏场景
bool CGameClientDlg::OnGameSceneMessage(BYTE cbGameStation, bool bLookonOther, const void * pBuffer, WORD wDataSize)
{
	switch (cbGameStation)
	{
	case GS_FREE:		//空闲状态
		{
			//效验数据
			if (wDataSize!=sizeof(CMD_S_StatusFree)) return false;
			CMD_S_StatusFree * pStatusFree=(CMD_S_StatusFree *)pBuffer;

			//设置控件
			if (IsLookonMode()==false)
			{
				m_ScoreView.ShowWindow(SW_HIDE);
				m_GameClientView.m_btStart.ShowWindow(SW_SHOW);
				m_GameClientView.m_btStart.SetFocus();
				SetGameTimer(GetMeChairID(),IDI_START_GAME,TIME_START_GAME);	
			}

			return true;
		}
	case GS_PLAYING:	//游戏状态
		{
			for (WORD i=0;i<GAME_PLAYER;i++)m_wViewChairID[i]=SwitchViewChairID(i);

			//效验数据
			if (wDataSize!=sizeof(CMD_S_StatusPlay)) return false;
			CMD_S_StatusPlay * pStatusPlay=(CMD_S_StatusPlay *)pBuffer;
			WORD wMeChairID=GetMeChairID();

			//加注信息
			m_lMaxCellScore=pStatusPlay->lMaxCellScore;
			m_lCellScore=pStatusPlay->lCellScore;
			m_lCurrentTimes=pStatusPlay->lCurrentTimes;
			m_lUserMaxScore=pStatusPlay->lUserMaxScore;
			m_GameClientView.m_GoldControl.SetGoldCount(m_lCellScore);

			//设置变量
			m_wBankerUser=pStatusPlay->wBankerUser;
			m_wCurrentUser=pStatusPlay->wCurrentUser;
			CopyMemory(m_bMingZhu,pStatusPlay->bMingZhu,sizeof(pStatusPlay->bMingZhu));
			CopyMemory(m_lTableScore,pStatusPlay->lTableScore,sizeof(pStatusPlay->lTableScore));
			CopyMemory(m_cbPlayStatus,pStatusPlay->cbPlayStatus,sizeof(pStatusPlay->cbPlayStatus));

			//设置扑克
			CopyMemory(&m_cbHandCardData[wMeChairID],&pStatusPlay->cbHandCardData,MAX_COUNT);

			//设置变量
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				m_GameClientView.m_cbPlayStatus[m_wViewChairID[i]]=m_cbPlayStatus[i];
				m_GameClientView.m_CardControl[i].SetDisplayHead(false);
				if(m_lTableScore[i]>0L)m_GameClientView.SetUserTableScore(m_wViewChairID[i],m_lTableScore[i],m_lTableScore[i]);

				//获取用户
				const tagUserData * pUserData=GetUserData(i);
				if (pUserData==NULL) continue;

				//效验状态
				//if(pUserData->cbUserStatus==US_PLAY) ASSERT(m_cbPlayStatus[i]==TRUE);
				//不能效验,特殊:在结束时,赢家断线重新接上,状态SIT

				//用户名字
				lstrcpyn(m_szAccounts[i],pUserData->szName,CountArray(m_szAccounts[i]));
			}

			//停止筹码动画
			m_GameClientView.StopMoveJettons();

			//庄家标志
			WORD wID=m_wViewChairID[m_wBankerUser];
			m_GameClientView.SetBankerUser(wID);

			//左上信息
			m_GameClientView.SetScoreInfo(m_lMaxCellScore,m_lCellScore);

			//设置界面
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				//设置位置
				WORD wViewChairID=m_wViewChairID[i];

				//设置扑克
				if (m_cbPlayStatus[i]) 
				{
					//char chData[100]={0};
					//::sprintf(chData, "%x, %x, %x", m_cbHandCardData[i][0], m_cbHandCardData[i][1], m_cbHandCardData[i][2]);
					//InsertSystemString(chData);
					m_GameClientView.m_CardControl[wViewChairID].SetCardData(m_cbHandCardData[i],MAX_COUNT);
					if((i!=wMeChairID ||IsLookonMode()) && m_bMingZhu[i])
					{
						m_GameClientView.m_CardControl[wViewChairID].SetLookCard(true);
					}
				}
			}
			if(m_bMingZhu[wMeChairID] && (!IsLookonMode() || IsAllowLookon()))
			{
				m_GameClientView.m_CardControl[m_wViewChairID[wMeChairID]].SetDisplayHead(true);
			}

			//控件处理
			m_GameClientView.m_btStart.ShowWindow(SW_HIDE);
			

			//判断控件
			if(m_wCurrentUser<GAME_PLAYER)
			{
				if(!(pStatusPlay->bCompareState))
				{
					//控件信息
					if(!IsLookonMode() && wMeChairID==m_wCurrentUser)
					{
						UpdataControl();
					}

					//设置时间
					SetGameTimer(m_wCurrentUser,IDI_USER_ADD_SCORE,TIME_USER_ADD_SCORE);
				}
				else
				{	
					if(!IsLookonMode() && wMeChairID==m_wCurrentUser)
					{
						//选择玩家比牌
						BOOL bCompareUser[GAME_PLAYER];
						for(int i=0;i<GAME_PLAYER;i++)
						{
							if(m_cbPlayStatus[i]==TRUE && wMeChairID!=i)
							{
								bCompareUser[m_wViewChairID[i]]=TRUE;
							}
							else bCompareUser[m_wViewChairID[i]]=FALSE;
						}
						//设置箭头
						m_GameClientView.SetCompareCard(true,bCompareUser);

						//等待比牌
						SendData(SUB_C_WAIT_COMPARE,NULL,0);

						//设置时间
						SetGameTimer(wMeChairID,IDI_USER_COMPARE_CARD,TIME_USER_COMPARE_CARD);

						//提示标志
						m_GameClientView.SetWaitUserChoice(FALSE);
					}
					else 
					{
						//比牌背景
						m_GameClientView.m_CardControl[m_wViewChairID[m_wCurrentUser]].SetCompareBack(true);

						SetGameTimer(m_wCurrentUser,IDI_DISABLE,TIME_USER_COMPARE_CARD);

						//提示标志
						m_GameClientView.SetWaitUserChoice(TRUE);
					}
				}
			}
			return true;
		}
	}

	return false;
}

//初始化游戏数据
void CGameClientDlg::OnInitZJHGameData()
{
	//用户信息
	m_wCurrentUser=INVALID_CHAIR;
	m_wBankerUser=INVALID_CHAIR;
	m_wWinnerUser=INVALID_CHAIR;

	//加注信息
	m_lMaxScore=0L;
	m_lMaxCellScore=0L;
	m_lCellScore=0L;
	m_lCurrentTimes=1L;
	m_lUserMaxScore=0L;
	ZeroMemory(m_lTableScore,sizeof(m_lTableScore));
	ZeroMemory(m_cbHandCardData,sizeof(m_cbHandCardData));

	//椅子信息
	for (int i=0;i<GAME_PLAYER;i++)
	{
		m_wLostUserID[i]=INVALID_CHAIR;
		m_wViewChairID[i]=INVALID_CHAIR;
		m_bMingZhu[i]=false;
	}

	//状态变量
	m_wBankerUser=INVALID_CHAIR;
	ZeroMemory(m_szAccounts,sizeof(m_szAccounts));
	ZeroMemory(m_cbPlayStatus,sizeof(m_cbPlayStatus));

	m_ScoreView.ShowWindow(false);
}

//游戏开始
bool CGameClientDlg::OnSubGameStart(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	if (wDataSize!=sizeof(CMD_S_GameStart)) return false;
	CMD_S_GameStart * pGameStart=(CMD_S_GameStart *)pBuffer;

	KillTimer(TIME_AUTO_CHECK_COMPARE_CARD);

	OnInitZJHGameData();

	//旁观界面
	if (IsLookonMode()==true)
	{
		OnStart(0,0);
		m_ScoreView.ShowWindow(false);
	}

	//数据信息
	m_lCellScore=pGameStart->lCellScore;
	m_lMaxCellScore=pGameStart->lMaxScore;
	m_lCurrentTimes=pGameStart->lCurrentTimes;
	m_wCurrentUser=pGameStart->wCurrentUser;
	m_wBankerUser=pGameStart->wBankerUser;
	m_lUserMaxScore=pGameStart->lUserMaxScore;
	m_GameClientView.m_GoldControl.SetGoldCount(m_lCellScore);

	//设置变量
	LONGLONG lTableScore=0L;
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		m_wLostUserID[i]=INVALID_CHAIR;
		m_wViewChairID[i]=SwitchViewChairID(i);
		m_GameClientView.m_cbPlayStatus[m_wViewChairID[i]]=FALSE;

		//获取用户
		const tagUserData * pUserData=GetUserData(i);
		if (pUserData==NULL) continue;

		//游戏信息		
		m_cbPlayStatus[i]=TRUE;
		m_GameClientView.m_cbPlayStatus[m_wViewChairID[i]]=TRUE;
		lTableScore+=m_lCellScore;
		m_lTableScore[i]=m_lCellScore;
		m_GameClientView.SetUserTableScore(m_wViewChairID[i],m_lTableScore[i],m_lTableScore[i]);

		//用户信息
		lstrcpyn(m_szAccounts[i],pUserData->szName,CountArray(m_szAccounts[i]));
	}
	//m_GameClientView.BeginMoveJettons();
	m_GameClientView.StopMoveJettons();

	//界面设置
	m_GameClientView.SetBankerUser(m_wViewChairID[m_wBankerUser]);
	m_GameClientView.SetScoreInfo(m_lMaxCellScore,m_lCellScore);
	m_ScoreView.ShowWindow(SW_HIDE);

	//派发扑克
	for(WORD i=0;i<MAX_COUNT;i++)
	{
		for (WORD j=m_wBankerUser;j<m_wBankerUser+GAME_PLAYER;j++)
		{
			WORD w=j%GAME_PLAYER;
			if (m_cbPlayStatus[w]==TRUE)
			{
				m_GameClientView.DispatchUserCard(m_wViewChairID[w],0);
			}
		}
	}

	//声音效果
	PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_START"));

	return true;
}

//用户放弃
bool CGameClientDlg::OnSubGiveUp(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	if (wDataSize!=sizeof(CMD_S_GiveUp)) return false;
	CMD_S_GiveUp * pGiveUp=(CMD_S_GiveUp *)pBuffer;

	//删除定时器
	if(IsLookonMode() || pGiveUp->wGiveUpUser!=GetMeChairID())KillGameTimer(IDI_USER_ADD_SCORE);	

	//设置变量
	m_cbPlayStatus[pGiveUp->wGiveUpUser]=FALSE;
	m_GameClientView.m_cbPlayStatus[m_wViewChairID[pGiveUp->wGiveUpUser]]=FALSE;

	//变量定义
	WORD wGiveUpUser=pGiveUp->wGiveUpUser;
	WORD wViewChairID=m_wViewChairID[wGiveUpUser];

	//扑克变灰
	m_GameClientView.m_CardControl[wViewChairID].SetCardColor(1);

	//重新设置被选比牌用户
	if(m_wCurrentUser==GetMeChairID() && m_GameClientView.GetCompareInfo())
	{
		BOOL bCompareUser[GAME_PLAYER];
		for(int i=0;i<GAME_PLAYER;i++)
		{
			if(m_cbPlayStatus[i]==TRUE && GetMeChairID()!=i)
			{
				bCompareUser[m_wViewChairID[i]]=TRUE;
			}
			else bCompareUser[m_wViewChairID[i]]=FALSE;
		}

		//设置箭头
		m_GameClientView.SetCompareCard(true,bCompareUser);
	}

	//状态设置
	if ((IsLookonMode()==false)&&(pGiveUp->wGiveUpUser==GetMeChairID())) SetGameStatus(GS_FREE);

	//环境设置
	if ((IsLookonMode()==true)||(wGiveUpUser!=GetMeChairID())) PlayGameSound(AfxGetInstanceHandle(),TEXT("GIVE_UP"));

	return true;
}

//用户下注
bool CGameClientDlg::OnSubAddScore(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_AddScore));
	if (wDataSize!=sizeof(CMD_S_AddScore))
	{
		return false;
	}
	CMD_S_AddScore * pAddScore=(CMD_S_AddScore *)pBuffer;
	if (NULL == pAddScore)
	{
		return false;
	}
	ASSERT(pAddScore->wCurrentUser < GAME_PLAYER);

	//删除定时器
	if(IsLookonMode() || pAddScore->wAddScoreUser!=GetMeChairID())KillGameTimer(IDI_USER_ADD_SCORE);

	//变量定义
	WORD wMeChairID=GetMeChairID();
	WORD wAddScoreUser=pAddScore->wAddScoreUser;
	WORD wViewChairID=m_wViewChairID[wAddScoreUser];
	m_lCurrentTimes=pAddScore->lCurrentTimes;
	ASSERT(m_lCurrentTimes<=m_lMaxCellScore/m_lCellScore);

	//加注处理
	if ((IsLookonMode()==true)||(wAddScoreUser!=wMeChairID))
	{
		//下注金币
		m_lTableScore[pAddScore->wAddScoreUser]+=pAddScore->lAddScoreCount;

		//加注界面
		m_GameClientView.SetUserTableScore(wViewChairID,m_lTableScore[pAddScore->wAddScoreUser],pAddScore->lAddScoreCount);
		m_GameClientView.BeginMoveJettons();

		//播放声音
		if (m_cbPlayStatus[wAddScoreUser]==TRUE)
		{
			 //PlayGameSound(AfxGetInstanceHandle(),TEXT("ADD_SCORE"));
			 //播放声音
			 const tagUserData *pUserData = GetUserData(wAddScoreUser);//用户信息
			 if (m_wIsAddScore==0)
			 {//加注
				 PlayGameSoundLanguage(pUserData->cbGender,TEXT("jiazhu"),true);
			 }
			 else
			 {//跟注
				 PlayGameSoundLanguage(pUserData->cbGender,TEXT("gengzhu"),true);
			 }
		}
	}

	//当前用户
	m_wCurrentUser=pAddScore->wCurrentUser;

	//控件信息
	if(pAddScore->wCompareState==FALSE && (!IsLookonMode()) && wMeChairID==m_wCurrentUser)
	{
		UpdataControl();
	}

	//设置时间
	if(pAddScore->wCompareState==FALSE)
	{//if(IsLookonMode() || wMeChairID!=m_wCurrentUser)
		SetGameTimer(m_wCurrentUser,IDI_USER_ADD_SCORE,TIME_USER_ADD_SCORE);
	}

	return true;
}

//用户看牌
bool CGameClientDlg::OnSubLookCard(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	if (wDataSize!=sizeof(CMD_S_LookCard)) return false;
	CMD_S_LookCard * pLookCard=(CMD_S_LookCard *)pBuffer;

	//重新定时
	if(IsLookonMode() || m_wCurrentUser!=GetMeChairID())
	{
		//删除定时
		KillGameTimer(IDI_USER_ADD_SCORE);
		SetGameTimer(m_wCurrentUser,IDI_USER_ADD_SCORE,TIME_USER_ADD_SCORE);
	}
	else
	{
		//设置扑克
		CopyMemory(m_cbHandCardData[GetMeChairID()],pLookCard->cbCardData,sizeof(pLookCard->cbCardData));
		m_GameClientView.m_CardControl[m_wViewChairID[GetMeChairID()]].SetCardData(m_cbHandCardData[GetMeChairID()],MAX_COUNT);
		m_GameClientView.m_CardControl[m_wViewChairID[GetMeChairID()]].SetDisplayHead(true);
	}

	//设置界面
	WORD wId=pLookCard->wLookCardUser;
	if(GetMeChairID()!=wId || IsLookonMode())m_GameClientView.m_CardControl[m_wViewChairID[wId]].SetLookCard(true);

	//允许旁观
	if(IsLookonMode() && IsAllowLookon() && m_GameClientView.m_CardControl[m_wViewChairID[GetMeChairID()]].GetLookCard())
	{
		m_GameClientView.m_CardControl[m_wViewChairID[GetMeChairID()]].SetDisplayHead(true);
	}

	//环境设置
//	if ((IsLookonMode()==true)||(wId!=GetMeChairID())) PlayGameSound(AfxGetInstanceHandle(),TEXT("CENTER_SEND_CARD"));
	//播放声音
	//const tagUserData *pUserData = GetUserData(GetMeChairID());//用户信息
	//if ((IsLookonMode()==true)||(wId!=GetMeChairID())) PlayGameSoundLanguage(pUserData->cbGender,TEXT("kanpai"),true);

	return true;
}

//用户比牌
bool CGameClientDlg::OnSubCompareCard(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	if (wDataSize!=sizeof(CMD_S_CompareCard))
	{
		return false;
	}
	CMD_S_CompareCard * pCompareCard=(CMD_S_CompareCard *)pBuffer;
	if (NULL == pCompareCard)
	{
		return false;
	}

	//删除时间
	KillTimer(IDI_CHECK_COMPARE_CARD);
	KillGameTimer(IDI_DISABLE);
	KillGameTimer(IDI_USER_COMPARE_CARD);

	//取消等待图片
	m_GameClientView.SetWaitUserChoice(INVALID_CHAIR);

#ifdef ZJH_GAME_PRINT_INFO
	char chInfo[100]={0};
	::sprintf(chInfo, "比牌玩家:%d,被比牌玩家:%d,输家:%d,当前玩家:%d", pCompareCard->wCompareUser[0], pCompareCard->wCompareUser[1], pCompareCard->wLostUser, pCompareCard->wCurrentUser);
	InsertSystemString(chInfo);
#endif


	//输牌用户
	m_wLostUser=pCompareCard->wLostUser;
	for(WORD i=0;i<GAME_PLAYER;i++)
	{
		if(m_wLostUserID[i]==INVALID_CHAIR)			
		{
			m_wLostUserID[i] = m_wLostUser;
			break;
		}
	}

	//设置变量
	BYTE byLostViewID = SwitchViewChairID(m_wLostUser);
	m_cbPlayStatus[m_wLostUser]=FALSE;
	if (byLostViewID<GAME_PLAYER)
	{
		m_GameClientView.m_cbPlayStatus[byLostViewID]=FALSE;
	}
	

	//状态设置
	if ((IsLookonMode()==false)&&(m_wLostUser==GetMeChairID()))
	{
		SetGameStatus(GS_FREE);
	}

	//当前用户
	m_wCurrentUser=pCompareCard->wCurrentUser;

	//取消比牌背景
	WORD wViewChair1=m_wViewChairID[pCompareCard->wCompareUser[0]];//比牌玩家
	WORD wViewChair2=m_wViewChairID[pCompareCard->wCompareUser[1]];//被比牌玩家
	m_GameClientView.m_CardControl[wViewChair1].SetCompareBack(false);
	m_GameClientView.m_CardControl[wViewChair2].SetCompareBack(false);

	//游戏结束
	if(m_wCurrentUser==INVALID_CHAIR)
	{
	   m_GameClientView.StopUpdataScore(true);
	}
	
	//播放声音
	const tagUserData *pUserData = GetUserData(pCompareCard->wCompareUser[0]);//用户信息
	PlayGameSoundLanguage(pUserData->cbGender,TEXT("bipai"),true);

	//过滤动画效果
	//OnFlashCardFinish(0,0);

	//隐藏控件
	ScoreControl(SW_HIDE);
	WORD wFalshUser[]={wViewChair1,wViewChair2};

	//动画效果
	m_GameClientView.PerformCompareCard(wFalshUser,byLostViewID);

	if (!IsLookonMode())
	{
		SetTimer(IDI_CHECK_COMPARE_CARD, TIME_AUTO_CHECK_COMPARE_CARD, NULL);
	}

	return true;
}

//用户开牌
bool CGameClientDlg::OnSubOpenCard(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	if (wDataSize!=sizeof(CMD_S_OpenCard)) return false;
	CMD_S_OpenCard* pOpenCard=(CMD_S_OpenCard *)pBuffer;
	if (NULL == pOpenCard)
	{
		return false;
	}

	//取消等待
	m_GameClientView.SetWaitUserChoice(INVALID_CHAIR);

	//胜利用户
	m_wWinnerUser=pOpenCard->wWinner;

#ifdef ZJH_GAME_PRINT_INFO
	char chInfo[100]={0};
	::sprintf(chInfo, "开牌操作,胜利玩家:%d", m_wWinnerUser);
	InsertSystemString(chInfo);
#endif

	//隐藏控件
	ScoreControl(SW_HIDE);
	m_GameClientView.m_btOpenCard.ShowWindow(SW_HIDE);

	//游戏结束
	m_GameClientView.StopUpdataScore(true);

	//过滤动画效果
	//OnFlashCardFinish(0,0);

	////动画效果  cp删除
	//WORD wFalshUser[GAME_PLAYER];
	//for (LONGLONG i=0;i<GAME_PLAYER;i++)
	//{
 //       if(m_cbPlayStatus[i]==TRUE)
	//	wFalshUser[i]=m_wViewChairID[i];
	//}
	//m_GameClientView.bFalshCard(wFalshUser);
	return true;
}

//用户强退
bool CGameClientDlg::OnSubPlayerExit(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	if (wDataSize!=sizeof(CMD_S_PlayerExit)) return false;
	CMD_S_PlayerExit * pPlayerExit=(CMD_S_PlayerExit *)pBuffer;

	WORD wID=pPlayerExit->wPlayerID;

	//游戏信息
	ASSERT(m_cbPlayStatus[wID]==TRUE);
	m_cbPlayStatus[wID]=FALSE;
	m_GameClientView.m_cbPlayStatus[m_wViewChairID[wID]]=FALSE;

	//用户名字
	for(WORD i=0;i<32;i++)m_szAccounts[wID][i]=0;

	return true;
}

//游戏结束
bool CGameClientDlg::OnSubGameEnd(const void * pBuffer, WORD wDataSize)
{
	//InsertShtickString("fffff",RGB(200,200,200),true);
	//效验参数
	if (wDataSize!=sizeof(CMD_S_GameEnd))
	{
		return false;
	}
	CMD_S_GameEnd * pGameEnd=(CMD_S_GameEnd *)pBuffer;
	WORD wMeChiar=GetMeChairID();

	//删除定时器
	KillGameTimer(IDI_USER_ADD_SCORE);
	KillTimer(IDI_CHECK_COMPARE_CARD);

	//状态设置
	m_GameClientView.SetWaitUserChoice(INVALID_CHAIR);
	SetGameStatus(GS_FREE);

	//处理控件
	AddScoreControl(SW_HIDE);
	ScoreControl(SW_HIDE);
	m_GameClientView.m_btOpenCard.ShowWindow(SW_HIDE);
	m_GameClientView.m_btAddScore.EnableWindow(FALSE);
	m_GameClientView.m_btCompareCard.EnableWindow(FALSE);
	m_GameClientView.m_btGiveUp.EnableWindow(FALSE);
	m_GameClientView.m_btLookCard.EnableWindow(FALSE);
	m_GameClientView.m_btFollow.EnableWindow(FALSE);
	m_GameClientView.m_GoldControl.SetGoldCount(0);


    //正常结束
	m_ScoreView.ResetScore();
	LONGLONG lTax = m_lCellScore*pGameEnd->lGameTax/1000;
	WORD wWinner=2;
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		//设置信息
		if (pGameEnd->lGameScore[i]!=0L)
		{
			if(pGameEnd->lGameScore[i]>0L) 
			{
				wWinner = i;
			}
			m_ScoreView.SetGameTax(lTax,i);
			//if (m_szAccounts[i][0]==0)
			//{
			//	m_ScoreView.SetGameScore(i,TEXT("已离开"),pGameEnd->lGameScore[i]);
			//}
			//else
			{
				m_ScoreView.SetGameScore(i,m_szAccounts[i],pGameEnd->lGameScore[i]);
			}
		}
	}

	//清理效果
	//m_ScoreView.ResetScore();
	m_GameClientView.StopUpdataScore(false);

	//比牌与被比牌用户所看到的数据
	for (WORD j=0;j<4;j++)
	{
		/*const tagUserData * pUserData=GetUserData(j);
		if(NULL != pUserData) 
		{
			if (m_szAccounts[j][0]==0) m_ScoreView.SetGameScore(j,TEXT("已离开"),pGameEnd->lGameScore[j]);
			else m_ScoreView.SetGameScore(j,m_szAccounts[j],pGameEnd->lGameScore[j]);
		}
		*/

		WORD wUserID=pGameEnd->wCompareUser[wMeChiar][j];
		if(wUserID==INVALID_CHAIR)continue;

		WORD wViewChiar=m_wViewChairID[wUserID];
		m_GameClientView.m_CardControl[wViewChiar].SetDisplayHead(true);
		m_GameClientView.m_CardControl[wViewChiar].SetLookCard(false);
		m_GameClientView.m_CardControl[wViewChiar].SetCardColor(INVALID_CHAIR);
		m_GameClientView.m_CardControl[wViewChiar].SetCardData(pGameEnd->cbCardData[wUserID],MAX_COUNT);
	}


	//自己扑克
	if(pGameEnd->wCompareUser[wMeChiar][0]!=INVALID_CHAIR || m_bMingZhu[wMeChiar])
	{
		//InsertGeneralString("-------------------------------------------", RGB(255, 0, 255), true);
		WORD wViewChiar = SwitchViewChairID(wMeChiar);
		//for (int i=0; i<MAX_COUNT; i++)
		//{
		//	char chData[10]={0};
		//	sprintf(chData, "%d", pGameEnd->cbCardData[wMeChiar][i]);
		//	InsertGeneralString(chData, RGB(255, 0, 255), true);
		//}
		//InsertGeneralString("-------------------------------------------", RGB(255, 0, 255), true);
		m_GameClientView.m_CardControl[wViewChiar].SetCardData(pGameEnd->cbCardData[wMeChiar],MAX_COUNT);
		m_GameClientView.m_CardControl[wViewChiar].SetCardColor(INVALID_CHAIR);
		m_GameClientView.m_CardControl[wViewChiar].SetDisplayHead(true);
		m_GameClientView.m_CardControl[wViewChiar].ShowWindow(SW_SHOW);
	}

	//过滤动态加入用户扑克
	//const tagUserData *pUserData=GetUserData(wMeChiar);
	//if(pUserData!=NULL && pUserData->cbUserStatus!=US_PLAY && pUserData->cbUserStatus!=US_OFFLINE)
	//{
	//	WORD wViewChiar=m_wViewChairID[wMeChiar];
	//	m_GameClientView.m_CardControl[wViewChiar].SetCardData(NULL,0);
	//}


	//赢家扑克信息  cp5.19
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		//设置信息
		if (pGameEnd->lGameScore[i]!=0L)
		{
			if(pGameEnd->lGameScore[i]>0L) 
			{
				WORD wViewChiar = SwitchViewChairID(i);
				m_GameClientView.m_CardControl[wViewChiar].SetDisplayHead(true);
				m_GameClientView.m_CardControl[wViewChiar].SetLookCard(false);
				m_GameClientView.m_CardControl[wViewChiar].SetCardColor(INVALID_CHAIR);
				m_GameClientView.m_CardControl[wViewChiar].SetCardData(pGameEnd->cbCardData[i],MAX_COUNT);
			}

		}
	}

	//开牌结束
	if(pGameEnd->wEndState==1)
	{
		//所有比牌用户
		if(m_cbPlayStatus[wMeChiar]==TRUE)
		{
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				if(m_cbPlayStatus[i]==TRUE)
				{
					WORD wViewChiar=SwitchViewChairID(i);
					m_GameClientView.m_CardControl[wViewChiar].SetLookCard(false);
					m_GameClientView.m_CardControl[wViewChiar].SetCardColor(INVALID_CHAIR);
					m_GameClientView.m_CardControl[wViewChiar].SetDisplayHead(true);
					m_GameClientView.m_CardControl[wViewChiar].SetCardData(pGameEnd->cbCardData[i],MAX_COUNT);
					m_GameClientView.m_CardControl[wViewChiar].ShowWindow(SW_SHOW);
				}
			}
		}
	}

	//数字滚动动画
	for( WORD i = 0; i < GAME_PLAYER; i++ )
	{
		if( pGameEnd->lGameScore[i] == 0 ) continue;
		WORD wViewId = m_wViewChairID[i];	
		m_GameClientView.m_NumberControl[wViewId].SetScore( pGameEnd->lGameScore[i] );
	}
	m_GameClientView.BeginMoveNumber();

	//回收筹码
	if(wWinner<GAME_PLAYER)
		m_GameClientView.SetGameEndInfo(m_wViewChairID[wWinner]);

	//播放声音
	if (IsLookonMode()==false)
	{
		if (pGameEnd->lGameScore[GetMeChairID()]>0L) PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_WIN"));
		else PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_LOST"));
	}
	else PlayGameSound(GetModuleHandle(NULL),TEXT("GAME_END"));

	//开始按钮
	if (IsLookonMode()==false)
	{
		m_GameClientView.m_btStart.ShowWindow(false);
		SetGameTimer(GetMeChairID(),IDI_START_GAME,TIME_START_GAME);
	}


	//成绩显示在即时聊天对话框
	TCHAR szBuffer[64];
	_snprintf(szBuffer,CountArray(szBuffer),TEXT("\n本局结束,成绩统计:"));
	InsertGeneralString(szBuffer,RGB(255,0,0),true);
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		//变量定义
		if (m_szAccounts[i][0]!=0)
		{
			_snprintf(szBuffer,CountArray(szBuffer),TEXT("%s："),m_szAccounts[i]);
			InsertGeneralString(szBuffer,RGB(0,255,0),false);
           _snprintf(szBuffer,CountArray(szBuffer),TEXT("%+I64d"),pGameEnd->lGameScore[i]);
		   InsertGeneralString(szBuffer,RGB(255,255,0),true);
			//_snprintf(szBuffer,CountArray(szBuffer),TEXT("扑克数据：%d,%d,%d;颜色数据：%d,%d,%d "),
			//	pGameEnd->cbCardData[i][0]&0x0f,pGameEnd->cbCardData[i][1]&0x0f,pGameEnd->cbCardData[i][2]&0x0f,
			//	(pGameEnd->cbCardData[i][0]&0xf0)>>4,(pGameEnd->cbCardData[i][1]&0xf0)>>4,
			//	(pGameEnd->cbCardData[i][2]&0xf0)>>4);
			//InsertGeneralString(szBuffer,RGB(0,123,183),true);
			//strScore.Format( TEXT( " %s ：" ),pUserData->szName);
			//InsertShtickString( strScore, RGB( 0, 255, 0 ), false );
			//strScore.Format( TEXT( " %d" ),pGameEnd->lGameScore[i]);
			//InsertShtickString( strScore, RGB( 255, 255, 0 ), true );
		}
	}

	m_ScoreView.ShowWindow(SW_SHOW);
	

	if (IsLookonMode()==true)
	{
		m_GameClientView.m_btStart.ShowWindow(SW_HIDE);
		m_ScoreView.ShowWindow(true);
		m_ScoreView.HideOptionBT(0x0000);
	}


	return true;
}

//转换信息
void CGameClientDlg::ChangeUserInfo(BYTE bCardData[],BYTE bCardCount,CString &CardInfo)
{
	//转换信息
	for(BYTE i=0;i<bCardCount;i++)
	{
		//扑克花色
		BYTE bCardColor = bCardData[i]&LOGIC_MASK_COLOR;
		switch(bCardColor)
		{
		case 0x00:
			{
				CardInfo+=TEXT("方块");
			}
			break;
		case 0x10:
			{
				CardInfo+=TEXT("梅花");
			}
			break;
		case 0x20:
			{
				CardInfo+=TEXT("红心");
			}
			break;
		case 0x30:
			{
				CardInfo+=TEXT("黑桃");
			}
			break;
		}

		//扑克数据
		BYTE bTempCardData = bCardData[i]&LOGIC_MASK_VALUE;
		switch(bTempCardData)
		{
		case 0x01:CardInfo+=TEXT("［A］.");break;
		case 0x02:CardInfo+=TEXT("［2］.");break;
		case 0x03:CardInfo+=TEXT("［3］.");break;
		case 0x04:CardInfo+=TEXT("［4］.");break;
		case 0x05:CardInfo+=TEXT("［5］.");break;
		case 0x06:CardInfo+=TEXT("［6］.");break;
		case 0x07:CardInfo+=TEXT("［7］.");break;
		case 0x08:CardInfo+=TEXT("［8］.");break;
		case 0x09:CardInfo+=TEXT("［9］.");break;
		case 0x0a:CardInfo+=TEXT("［10］.");break;
		case 0x0b:CardInfo+=TEXT("［J］.");break;
		case 0x0c:CardInfo+=TEXT("［Q］.");break;
		case 0x0d:CardInfo+=TEXT("［K］.");break;
		}
	}

	return ;
}

//处理控制
void CGameClientDlg::ScoreControl(BOOL bShow)
{
	m_GameClientView.m_btAddScore.ShowWindow(bShow);
	m_GameClientView.m_btCompareCard.ShowWindow(bShow);
	m_GameClientView.m_btGiveUp.ShowWindow(bShow);
	m_GameClientView.m_btLookCard.ShowWindow(bShow);
	m_GameClientView.m_btFollow.ShowWindow(bShow);

	return ;
}

//加注控制
void CGameClientDlg::AddScoreControl(BOOL bShow)
{
	m_GameClientView.m_btMinScore.ShowWindow(bShow);
	m_GameClientView.m_btMaxScore.ShowWindow(bShow);
	m_GameClientView.m_btCancel.ShowWindow(bShow);
	m_GameClientView.m_btConfirm.ShowWindow(bShow);
	m_GameClientView.m_GoldControl.ShowWindow(bShow);
	m_GameClientView.m_GoldControl.m_bMingTag=m_bMingZhu[GetMeChairID()];

	return;
}

//处理控制
void CGameClientDlg::UpdataControl()
{
	ScoreControl(SW_SHOW);
	ActiveGameFrame();

	//放弃按钮
	WORD wMeChairID = GetMeChairID();
	m_GameClientView.m_btGiveUp.EnableWindow(TRUE);

	//看牌按钮
	m_GameClientView.m_btLookCard.EnableWindow((m_bMingZhu[wMeChairID])?FALSE:TRUE);

	//判断开牌
	//LONGLONG lTemp=(m_bMingZhu[wMeChairID])?6:5;
    LONGLONG lTemp=(m_bMingZhu[wMeChairID])?2:1;
	if((m_lUserMaxScore-m_lTableScore[wMeChairID]) >= (m_lMaxCellScore*lTemp))
	{
		//查找上家
		for (LONGLONG j=wMeChairID-1;;j--)
		{
			if(j==-1)j=GAME_PLAYER-1;
			if(m_cbPlayStatus[j]==TRUE)break;
		}

		//跟注按钮
		m_GameClientView.m_btFollow.EnableWindow((m_lTableScore[j]==m_lCellScore)?FALSE:TRUE);

		//加注按钮
		m_GameClientView.m_btAddScore.EnableWindow(((m_lCurrentTimes*m_lCellScore) < m_lMaxCellScore)?TRUE:FALSE);

		//比牌按钮
		if(wMeChairID==m_wBankerUser || m_lTableScore[wMeChairID]>=2*m_lCellScore)
		{
			m_GameClientView.m_btCompareCard.EnableWindow(TRUE);
		}
		else m_GameClientView.m_btCompareCard.EnableWindow(FALSE);
	}
	else 
	{
		m_GameClientView.m_btFollow.EnableWindow(FALSE);
		m_GameClientView.m_btAddScore.EnableWindow(FALSE);
		m_GameClientView.m_btCompareCard.EnableWindow(FALSE);
		m_GameClientView.m_btOpenCard.ShowWindow(SW_SHOW);
		m_GameClientView.m_btLookCard.EnableWindow(FALSE);
		m_GameClientView.m_btCompareCard.ShowWindow(SW_HIDE);
	}

	return;
}

//开始按钮
LRESULT	CGameClientDlg::OnStart(WPARAM wParam, LPARAM lParam)
{
	//删除定时器
	if(!IsLookonMode())KillGameTimer(IDI_START_GAME);

	//停止动画
	m_GameClientView.StopMoveJettons();
	m_GameClientView.StopMoveNumber();

	//设置界面
	m_wBankerUser=INVALID_CHAIR;
	m_GameClientView.SetBankerUser(INVALID_CHAIR);
	m_GameClientView.SetScoreInfo(0,0);
	m_GameClientView.m_btStart.ShowWindow(SW_HIDE);
	//m_GameClientView.m_ScoreView.ShowWindow(SW_HIDE);
	m_GameClientView.SetUserTableScore(INVALID_CHAIR,0L,0L);

	for (WORD i=0;i<GAME_PLAYER;i++) 
	{
		m_bMingZhu[i]=false;
		m_GameClientView.m_NumberControl[i].ResetControl();
		m_GameClientView.m_CardControl[i].SetCardData(NULL,0);
		m_GameClientView.m_CardControl[i].SetDisplayHead(false);
		m_GameClientView.m_CardControl[i].SetCardColor(INVALID_CHAIR);
		m_GameClientView.m_CardControl[i].SetLookCard(false);
		m_GameClientView.m_CardControl[i].SetCompareCard(false);
	}
	m_GameClientView.m_JettonControl.ResetControl();

	//状态变量
	m_wCurrentUser=INVALID_CHAIR;
	m_wBankerUser=INVALID_CHAIR;
	ZeroMemory(m_szAccounts,sizeof(m_szAccounts));
	ZeroMemory(m_cbPlayStatus,sizeof(m_cbPlayStatus));
	ZeroMemory(m_lTableScore,sizeof(m_lTableScore));

	//加注信息
	m_lMaxCellScore=0L;
	m_lCellScore=0L;

	//发送消息
	if(!IsLookonMode())SendUserReady(NULL,0);

	return 0;
}

//加注按钮 
LRESULT	CGameClientDlg::OnAddScore(WPARAM wParam, LPARAM lParam)
{
	ASSERT(m_lCellScore>0L);

	//处理控件
	m_GameClientView.m_GoldControl.SetMaxGold(m_lMaxCellScore);
	m_GameClientView.m_GoldControl.SetMinGold(m_lCellScore*m_lCurrentTimes);
	m_GameClientView.m_GoldControl.SetGold(m_lCellScore*m_lCurrentTimes);
	AddScoreControl(SW_SHOW);
	ScoreControl(SW_HIDE);

	return 0;
}

//取消消息
LRESULT CGameClientDlg::OnCancelAdd(WPARAM wParam, LPARAM lParam)
{
	//处理控件
	AddScoreControl(SW_HIDE);
	ScoreControl(SW_SHOW);

	return 0;
}

//最少加注
LRESULT	CGameClientDlg::OnMinScore(WPARAM wParam, LPARAM lParam)
{
#ifdef _DEBUG
	////测试时间
	//BYTE bTemp[]={1,2,3};
	//for (WORD j=0;j<GAME_PLAYER;j++)
	//{
	//	m_GameClientView.m_CardControl[j].SetCardData(bTemp,3);
	//	m_GameClientView.m_CardControl[j].SetDisplayHead(true);
	//}

	//WORD wFalshUser[GAME_PLAYER];
	//for (LONGLONG i=0;i<GAME_PLAYER;i++)wFalshUser[i]=i;
	//m_GameClientView.bFalshCard(wFalshUser);

	//WORD wFalshUser1[]={1,2};

	//动画效果
	//m_GameClientView.PerformCompareCard(wFalshUser1,1);

	//////派发扑克
	////for(WORD i=0;i<MAX_COUNT;i++)
	////{
	//	for (WORD j=0;j<GAME_PLAYER;j++)
	//	{
	//		m_GameClientView.SetUserTableScore(j,1000,1000);
	//		m_GameClientView.BeginMoveJettons();
	//		//m_GameClientView.DispatchUserCard(j,0);
	//	}
	////}
	////回收筹码
	//m_GameClientView.SetGameEndInfo(2);

	////数字滚动动画
	//for( WORD i = 0; i < GAME_PLAYER; i++ )
	//{
	//	LONGLONG lscorre = ((rand()%2)?1:-1)*99999;
	//	m_GameClientView.m_NumberControl[i].SetScore( lscorre );
	//}
	//m_GameClientView.BeginMoveNumber();

	//return 0;
#endif

	ASSERT(m_lCellScore>0L);
	m_GameClientView.m_GoldControl.SetGold(m_lCellScore*m_lCurrentTimes);

	return 0;
}

//最大加注
LRESULT	CGameClientDlg::OnMaxScore(WPARAM wParam, LPARAM lParam)
{
	if(m_lMaxCellScore>0)
	{
		m_GameClientView.m_GoldControl.SetGold(m_lMaxCellScore);
	}
	return 0;
}

//加注消息 && 跟注消息
LRESULT CGameClientDlg::OnConfirmScore(WPARAM wParam, LPARAM lParam)
{
	//一为跟注
	WORD wTemp=LOWORD(wParam);

	//效验数据
	ASSERT(wTemp==1 || m_GameClientView.m_GoldControl.GetGold() >= (m_lCellScore*m_lCurrentTimes)
		&& m_GameClientView.m_GoldControl.GetGold() <= m_lMaxCellScore);

	//删除时间
	KillGameTimer(IDI_USER_ADD_SCORE);

	//获取筹码
	LONGLONG lCurrentScore=(wTemp==0)?(m_GameClientView.m_GoldControl.GetGold()):(m_lCellScore*m_lCurrentTimes);
	WORD wMeChairID=GetMeChairID();
 
	//明注加倍
	if(m_bMingZhu[wMeChairID])lCurrentScore*=2;

	//预先处理
	m_lTableScore[wMeChairID]+=lCurrentScore;
	m_GameClientView.SetUserTableScore(m_wViewChairID[wMeChairID],m_lTableScore[wMeChairID],lCurrentScore);
	m_GameClientView.BeginMoveJettons();

	//处理按钮
	AddScoreControl(SW_HIDE);

	m_GameClientView.m_btOpenCard.ShowWindow(SW_HIDE);
//	m_GameClientView.m_btAddScore.LoadRgnImage(AfxGetInstanceHandle(),IDB_USERCONTROL_INVEST,(255,0,255));  为何加载2011.3.18
	ScoreControl(SW_HIDE);
	m_GameClientView.m_btOpenCard.ShowWindow(SW_HIDE);
	m_GameClientView.m_btAddScore.EnableWindow(FALSE);
	m_GameClientView.m_btCompareCard.EnableWindow(FALSE);
	m_GameClientView.m_btGiveUp.EnableWindow(FALSE);
	m_GameClientView.m_btLookCard.EnableWindow(FALSE);
	m_GameClientView.m_btFollow.EnableWindow(FALSE);

	//界面设置
	
	m_GameClientView.UpdateGameView(NULL);
	//PlayGameSound(AfxGetInstanceHandle(),TEXT("ADD_SCORE"));

	//播放声音
	const tagUserData *pUserData = GetUserData(wMeChairID);//用户信息
	if (wTemp==0)
	{//加注
		PlayGameSoundLanguage(pUserData->cbGender,TEXT("jiazhu"),true);
	}
	else
	{//跟注
		PlayGameSoundLanguage(pUserData->cbGender,TEXT("gengzhu"),true);
	}
	
	m_wIsAddScore=wTemp;

	//发送消息
	CMD_C_AddScore AddScore;
	AddScore.wState=0;
	AddScore.lScore=lCurrentScore;
	SendData(SUB_C_ADD_SCORE,&AddScore,sizeof(AddScore));

	return 0;
}

//看牌消息
LRESULT	CGameClientDlg::OnLookCard(WPARAM wParam, LPARAM lParam)
{
	//设置控件
	WORD wMeChairID=GetMeChairID();
	KillGameTimer(IDI_USER_ADD_SCORE);

	//重新定时
	SetGameTimer(wMeChairID,IDI_USER_ADD_SCORE,TIME_USER_ADD_SCORE);

	//设置数据
	m_bMingZhu[wMeChairID] = true;

	//发送消息
	SendData(SUB_C_LOOK_CARD,NULL,0);
	
	//控件信息
	UpdataControl();

	//环境设置
	//PlayGameSound(AfxGetInstanceHandle(),TEXT("CENTER_SEND_CARD"));
	//播放声音
	const tagUserData *pUserData = GetUserData(GetMeChairID());//用户信息
	PlayGameSoundLanguage(pUserData->cbGender,TEXT("kanpai"),true);

	return 0;
}

//比牌消息
LRESULT	CGameClientDlg::OnCompareCard(WPARAM wParam, LPARAM lParam)
{
	//删除定时器
	KillGameTimer(IDI_USER_ADD_SCORE);

	//处理控件
	AddScoreControl(SW_HIDE);

	m_GameClientView.m_btAddScore.EnableWindow(FALSE);
	m_GameClientView.m_btCompareCard.EnableWindow(FALSE);
	m_GameClientView.m_btGiveUp.EnableWindow(FALSE);
	m_GameClientView.m_btLookCard.EnableWindow(FALSE);
	m_GameClientView.m_btFollow.EnableWindow(FALSE);

	//判断明注
	WORD wMeChairID=GetMeChairID();
	LONGLONG lCurrentScore=(m_bMingZhu[wMeChairID])?(m_lCurrentTimes*m_lCellScore*4):(m_lCurrentTimes*m_lCellScore*2);
	m_lTableScore[wMeChairID]+=lCurrentScore;
	m_GameClientView.SetUserTableScore(m_wViewChairID[wMeChairID],m_lTableScore[wMeChairID],lCurrentScore);
	m_GameClientView.BeginMoveJettons();

	//发送消息
	CMD_C_AddScore AddScore;
	AddScore.wState=1;
	AddScore.lScore=lCurrentScore;
	SendData(SUB_C_ADD_SCORE,&AddScore,sizeof(CMD_C_AddScore));

	//当前人数
	BYTE UserCount=0;
	for (BYTE i=0;i<GAME_PLAYER;i++)
	{
		if(m_cbPlayStatus[i]==TRUE)UserCount++;
	}

	//庄家在第一轮没下注只能跟上家比牌 或 只剩下两人
	if(m_wBankerUser==wMeChairID && (m_lTableScore[wMeChairID]-lCurrentScore)==m_lCellScore || UserCount==2)
	{
		//构造变量
		CMD_C_CompareCard CompareCard;
		CompareCard.wCompareUser = INVALID_CHAIR;

		//查找上家
		BYTE iCompareUserID = (GAME_PLAYER+wMeChairID-1)%GAME_PLAYER;
		for (BYTE i=0; i<GAME_PLAYER; i++)
		{
			BYTE byTemplateUserID = (GAME_PLAYER+iCompareUserID-i)%GAME_PLAYER;
			const tagUserData *pUserData=GetUserData(byTemplateUserID);
			if (NULL!=pUserData && m_cbPlayStatus[byTemplateUserID])
			{
				CompareCard.wCompareUser=byTemplateUserID;
				break;
			}
		}/*

		for (LONGLONG i=(LONGLONG)wMeChairID-1;;i--)
		{
			if(i==-1)i=GAME_PLAYER-1;
			if(m_cbPlayStatus[i]==TRUE)
			{
				CompareCard.wCompareUser=(WORD)i;
				break;
			}
		}*/
		//发送消息
		SendData(SUB_C_COMPARE_CARD,&CompareCard,sizeof(CMD_C_CompareCard));
	}
	else	//选择玩家比牌
	{
		BOOL bCompareUser[GAME_PLAYER] = {FALSE};
		for(int i=0;i<GAME_PLAYER;i++)
		{
			const tagUserData *pUserData=GetUserData(i);
			if (NULL!=pUserData && m_cbPlayStatus[i] && i!=wMeChairID)
			{
				BYTE byViewID = SwitchViewChairID(i);
				if (byViewID<GAME_PLAYER)
				{
					bCompareUser[byViewID]=TRUE;
				}
			}
		}

		//设置箭头
		m_GameClientView.SetCompareCard(true,bCompareUser);

		//发送等待"比牌"信息
		SendData(SUB_C_WAIT_COMPARE,NULL,0);

		//设置时间
		SetGameTimer(wMeChairID,IDI_USER_COMPARE_CARD,TIME_USER_COMPARE_CARD);

		//提示标志
		m_GameClientView.SetWaitUserChoice(FALSE);
	}

	return 0;
}

//被选比牌用户
LRESULT CGameClientDlg::OnChooseCUser(WPARAM wParam, LPARAM lParam)
{
	//效验数据
	WORD wTemp=LOWORD(wParam);
	WORD wChairID=INVALID_CHAIR;
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		if(m_wViewChairID[i]==wTemp)
		{
			wChairID=i;
			break;
		}
	}
	ASSERT(wChairID!=INVALID_CHAIR && m_cbPlayStatus[wChairID]==TRUE && wChairID!=GetMeChairID());

	//删除定时器
	KillGameTimer(IDI_USER_COMPARE_CARD);

	//清理界面
	m_GameClientView.SetCompareCard(false,NULL);

	//构造变量
	CMD_C_CompareCard CompareCard;
	ZeroMemory(&CompareCard,sizeof(CompareCard));
	CompareCard.wCompareUser=wChairID;

	//发送消息
	SendData(SUB_C_COMPARE_CARD,&CompareCard,sizeof(CMD_C_CompareCard));

	return 0;
}

//开牌消息
LRESULT	CGameClientDlg::OnOpenCard(WPARAM wParam, LPARAM lParam)
{
	//处理控件
	m_GameClientView.m_btGiveUp.EnableWindow(FALSE);
	m_GameClientView.m_btLookCard.EnableWindow(FALSE);
	//m_GameClientView.m_btOpenCard.EnableWindow(FALSE);
	m_GameClientView.m_btOpenCard.ShowWindow(SW_HIDE);

	//删除定时器
	KillGameTimer(IDI_USER_ADD_SCORE);

	//查找人数
	WORD bUserCount =0;
	for(WORD i=0;i<GAME_PLAYER;i++)
	{
		const tagUserData *pUserData=GetUserData(i);
		if (NULL!=pUserData && m_cbPlayStatus[i])
		{
			bUserCount++;
		}
	}

	//两人比牌
	if(bUserCount==2)
	{
		OnCompareCard(0,0);
		return 0;
	}

	
	//判断明注
	WORD wMeChairID=GetMeChairID();
	LONGLONG lCurrentScore=(m_bMingZhu[wMeChairID])?(m_lCurrentTimes*m_lCellScore*4):(m_lCurrentTimes*m_lCellScore*2);
	m_lTableScore[wMeChairID]+=lCurrentScore;
	//if(m_lTableScore[wMeChairID] > m_lUserMaxScore)
	//{
	//  m_lTableScore[wMeChairID] = m_lUserMaxScore;
	//}
	m_GameClientView.SetUserTableScore(m_wViewChairID[wMeChairID],m_lTableScore[wMeChairID],lCurrentScore);
	m_GameClientView.BeginMoveJettons();

	//发送消息
	CMD_C_AddScore AddScore;
	AddScore.wState=2;
	AddScore.lScore=lCurrentScore;
	SendData(SUB_C_ADD_SCORE,&AddScore,sizeof(AddScore));

	//发送消息
	SendData(SUB_C_OPEN_CARD,NULL,0);

	return 0;
}

//放弃消息
LRESULT	CGameClientDlg::OnGiveUp(WPARAM wParam, LPARAM lParam)
{
	//删除时间
	KillGameTimer(IDI_USER_ADD_SCORE);
	PlayGameSound(AfxGetInstanceHandle(),TEXT("GIVE_UP"));

	//处理按钮
	AddScoreControl(SW_HIDE);

	m_GameClientView.m_btOpenCard.ShowWindow(SW_HIDE);
	//m_GameClientView.m_btAddScore.EnableWindow(FALSE);
	//m_GameClientView.m_btCompareCard.EnableWindow(FALSE);
	//m_GameClientView.m_btGiveUp.EnableWindow(FALSE);
	//m_GameClientView.m_btLookCard.EnableWindow(FALSE);
	//m_GameClientView.m_btFollow.EnableWindow(FALSE);
	m_GameClientView.m_btAddScore.ShowWindow(FALSE);
	m_GameClientView.m_btCompareCard.ShowWindow(FALSE);
	m_GameClientView.m_btGiveUp.ShowWindow(FALSE);
	m_GameClientView.m_btLookCard.ShowWindow(FALSE);
	m_GameClientView.m_btFollow.ShowWindow(FALSE);

	//扑克变灰
	m_GameClientView.m_CardControl[m_wViewChairID[GetMeChairID()]].SetCardColor(1);

	//发送消息
	SendData(SUB_C_GIVE_UP,NULL,0);

	return 0;
}

//发牌完成
LRESULT CGameClientDlg::OnSendCardFinish(WPARAM wParam, LPARAM lParam)
{
	//设置时间
	if(m_wCurrentUser==INVALID_CHAIR)return 0;
	SetGameTimer(m_wCurrentUser,IDI_USER_ADD_SCORE,TIME_USER_ADD_SCORE);

	//过虑用户
	WORD wMeChairID=GetMeChairID();
	if (IsLookonMode() || wMeChairID!=m_wCurrentUser)return 0;

	//可用控件
	ActiveGameFrame();
	UpdataControl();

	return 0;
}

//闪牌完成
LRESULT CGameClientDlg::OnFlashCardFinish(WPARAM wParam, LPARAM lParam)
{
	//比牌结束
	if(m_wWinnerUser>=GAME_PLAYER)
	{//
		if(m_wLostUser>GAME_PLAYER)return 0;
		ASSERT(m_wLostUser<GAME_PLAYER);
		KillTimer(IDI_CHECK_COMPARE_CARD);
		m_GameClientView.StopCompareCard();
		m_GameClientView.StopFlashCard();

		//输牌用户
		WORD wLostUser=m_wLostUser;
		WORD wMeChairID=GetMeChairID();

		//变量定义
		WORD wViewChairID=m_wViewChairID[wLostUser];
		WORD wMeViewChairID=m_wViewChairID[wMeChairID];

		//设置扑克
		m_GameClientView.m_CardControl[wMeViewChairID].SetCardData(m_cbHandCardData[wMeChairID],MAX_COUNT);

		//环境设置
		if ((!IsLookonMode()) && wLostUser==wMeChairID) PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_LOST"));

		//玩家人数
		BYTE bCount=0;
		for (WORD i=0;i<GAME_PLAYER;i++)
		{
			if(m_cbPlayStatus[i]==TRUE)
			{
				bCount++;
			}
		}

#ifdef ZJH_GAME_PRINT_INFO
		char chInfo[100]={0};
		::sprintf(chInfo, "比牌动画结束,当前操作玩家:%d", m_wCurrentUser);
		InsertSystemString(chInfo);
#endif

		//判断结束
		if(bCount>1)
		{
			//控件信息
			if((!IsLookonMode()) && wMeChairID==m_wCurrentUser)
			{
				UpdataControl();
			}

			//设置时间
			SetGameTimer(m_wCurrentUser,IDI_USER_ADD_SCORE,TIME_USER_ADD_SCORE);
		}
		else 
		{
			WORD i = INVALID_CHAIR;
			for(i=0;i<GAME_PLAYER;i++)
			{
				if(m_cbPlayStatus[i]==TRUE)break;
			}
			WORD wMeChairID = GetMeChairID();
			if((!IsLookonMode())&&(i==wMeChairID || wMeChairID==m_wLostUser))
			{
				//发送消息
				SendData(SUB_C_FINISH_FLASH,NULL,0);
			}
		}
		m_GameClientView.UpdateGameView(NULL);
	}
	else//开牌结束
	{
		//ASSERT(m_wWinnerUser<GAME_PLAYER);
		if(m_wWinnerUser>=GAME_PLAYER)return 0;		

#ifdef ZJH_GAME_PRINT_INFO
		char chInfo[100]={0};
		::sprintf(chInfo, "开牌结束,赢家:%d, 当前操作玩家:%d", m_wWinnerUser, m_wCurrentUser);
		InsertSystemString(chInfo);
#endif

		//比牌失败用户
		for (int i=0;i<GAME_PLAYER;i++)
		{
			if(m_cbPlayStatus[i]==TRUE && i!=m_wWinnerUser)		
			{
				//扑克变裂
				WORD wViewChairID=m_wViewChairID[i];
				m_GameClientView.m_CardControl[wViewChairID].SetCardColor(2);

				//状态设置
				if ((!IsLookonMode())&&(i==GetMeChairID())) 
				{
					SetGameStatus(GS_FREE);
					//环境设置
					PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_LOST"));
				}
			}
		}
		m_wWinnerUser=INVALID_CHAIR;

		//发送消息
		SendData(SUB_C_FINISH_FLASH,NULL,0);
	}
	
	//过滤动态加入用户扑克
	const tagUserData *pUserData=GetUserData(GetMeChairID());
	if(pUserData!=NULL && pUserData->cbUserStatus!=US_PLAY && pUserData->cbUserStatus!=US_OFFLINE)
	{
		WORD wViewChiar=m_wViewChairID[GetMeChairID()];
		m_GameClientView.m_CardControl[wViewChiar].SetCardData(NULL,0);
	}
	m_GameClientView.UpdateGameView(NULL);
	return 0;
}

//点击继续游戏
void CGameClientDlg::OnEventContinueGame()
{
	OnStart(0, 0);
}

//////////////////////////////////////////////////////////////////////////


void CGameClientDlg::OnTimer(UINT nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CGameFrameDlg::OnTimer(nIDEvent);

	if (IDI_CHECK_COMPARE_CARD == nIDEvent)
	{
		KillTimer(IDI_CHECK_COMPARE_CARD);
#ifdef ZJH_GAME_PRINT_INFO
		char chInfo[100]={0};
		::sprintf(chInfo, "比牌动画应该结束了,当前操作玩家:%d", m_wCurrentUser);
		InsertSystemString(chInfo);
#endif
		OnFlashCardFinish(0, 0);
	}
}

//播放游戏声音
void CGameClientDlg::PlayGameSoundLanguage(BYTE cbGender, CString strSound, bool bSoundType)
{
	CString strGameSound="";
	//性别（播放男女声音）
	if (1==cbGender)
	{//女
		strGameSound=m_sDialect+strSound+TEXT("_W");
	}
	else
	{//男
		strGameSound=m_sDialect+strSound+TEXT("_M");
	}

	if (bSoundType)
	{//多种声音
		TCHAR cBuffer[10]={""};
		int iRand=rand()%2+1;
		::_snprintf(cBuffer,sizeof(cBuffer),"%d",iRand);
		strGameSound=strGameSound+TEXT("_")+TEXT(cBuffer);
	}
	//InsertShtickString(strGameSound,RGB(200,200,200),true);
	PlayGameSound(AfxGetInstanceHandle(),strGameSound);
}
