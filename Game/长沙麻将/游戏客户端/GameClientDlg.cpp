#include "Stdafx.h"
#include "GameClient.h"
#include "GameOption.h"
#include "GameClientDlg.h"
#include "GameLogic.h"
//////////////////////////////////////////////////////////////////////////

//游戏定时器
#define IDI_START_GAME				200									//开始定时器
#define IDI_OPERATE_CARD			202									//操作定时器
#define IDI_OPERATE_HAI_DI			203									//海底定时器
#define IDI_OTHER_USEROUTCARD		205									//其实出牌定时器
#define IDI_GAME_END                206                                 //结束定时器
#define IDI_SHOW_SZ                 207                                 //骰子定时器
#define IDI_HUANGZHUANG             208                                 //荒庄定时器
#define IDI_BIRD                    209                                 //扎鸟定时器
#define IDI_SOUND                   210                                 //声音定时器
//游戏定时器
#define TIME_START_GAME				15									//开始定时器
#define TIME_HEAR_STATUS			15									//出牌定时器
#define TIME_OPERATE_CARD			10									//操作定时器
#define TIME_OPERATE_HAI_DI			10									//操作定时器
#define TIME_USERTINGCARD			5									//操作定时器
#define TIME_GAME_END				5000								//结束框显示时间
#define TIME_SHOW_SZ				800								    //骰子时间


//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CGameClientDlg, CGameFrameDlg)
	ON_MESSAGE(IDM_START,OnStart)
	ON_MESSAGE(IDM_OUT_CARD,OnOutCard)
	ON_MESSAGE(IDM_CARD_OPERATE,OnCardOperate)
	ON_MESSAGE(IDM_OPERATE_HAI_DI,OnOperateHaiDi)
	ON_MESSAGE(IDM_TRUSTEE_CONTROL,OnStusteeControl)
	ON_MESSAGE(IDM_SHOW_SZ,OnShowSZ)
	ON_WM_TIMER()
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CGameClientDlg::CGameClientDlg() : CGameFrameDlg(&m_GameClientView, &m_ScoreView)
{
	//游戏变量
	m_wBankerUser=INVALID_CHAIR;
	m_wCurrentUser=INVALID_CHAIR;
	m_bBirdUserID=4;

	//状态变量
	m_bHearStatus=false;
	m_bIsMeCanOutCard = false;

	//托管变量
	m_bStustee=false;
	m_wTimeOutCount =0;

	//声音变量
	m_cbGender=0;
	m_InfoSound="";

	//堆立变量
	m_wHeapHand=0;
	m_wHeapTail=0;
	ZeroMemory(m_cbHeapCardInfo,sizeof(m_cbHeapCardInfo));

	//出牌信息
	m_cbOutCardData=0;
	m_wOutCardUser=INVALID_CHAIR;
	ZeroMemory(m_cbDiscardCard,sizeof(m_cbDiscardCard));
	ZeroMemory(m_cbDiscardCount,sizeof(m_cbDiscardCount));

	//组合扑克
	ZeroMemory(m_cbWeaveCount,sizeof(m_cbWeaveCount));
	ZeroMemory(m_WeaveItemArray,sizeof(m_WeaveItemArray));

	//扑克变量
	m_cbLeftCardCount=0;
	ZeroMemory(m_cbCardIndex,sizeof(m_cbCardIndex));

	//方言变量
	m_sDialect="GY"; 

	return;
}

//析构函数
CGameClientDlg::~CGameClientDlg()
{
}

//初始函数
bool CGameClientDlg::InitGameFrame()
{
	//设置标题
	//SetWindowText(TEXT("长沙麻将  --  Ver：1.0.5.3"));

	//方言格式
	//m_sDialect.Format("%s_",m_strLanguage.c_str()); 

	//设置图标
	HICON hIcon=LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME));
	SetIcon(hIcon,TRUE);
	SetIcon(hIcon,FALSE);

	//加载资源
	m_GameClientView.m_btStart.ShowWindow(SW_SHOW);
	m_GameClientView.m_btHaiDi.ShowWindow(SW_HIDE);
	m_GameClientView.m_btNoHaiDi.ShowWindow(SW_HIDE);

	m_GameClientView.m_btAutoPlayOn.ShowWindow(SW_SHOW);
	m_GameClientView.m_btAutoPlayOff.ShowWindow(SW_HIDE);
	//m_ScoreView.ShowWindow(SW_SHOW);

	m_GameClientView.SetGameClientDlgPtr(this);
	//m_GameClientView.m_FlashControl.ShowWindow(SW_HIDE);
	//m_GameClientView.m_ControlWnd.ShowWindow(SW_SHOW);


	//m_ScoreView.ShowWindow(SW_SHOW);
	//m_ScoreView.SetCountDown(true);


	//
	////显示界面
	//BYTE byMask = WIK_NULL;
	//byMask = WIK_CHI_HU;
	//byMask |= WIK_PENG;
	//byMask |= WIK_LEFT;

	//tagGangCardResult bucardresult;
	//BYTE cbCardData[4]={0x11, 0x11, 0x11,0x11 };
	//::memset(&bucardresult, 0, sizeof(tagGangCardResult));
	//memcpy(bucardresult.cbCardData,cbCardData,sizeof(cbCardData));
	//bucardresult.cbCardCount =1;
	//m_GameClientView.m_ControlWnd.SetControlInfo(0x12,byMask,bucardresult);
	//m_GameClientView.m_ControlWnd.AddGangCardData(NULL, 0);
	//m_GameClientView.m_ControlWnd.ShowWindow(SW_SHOW);


	//HWND hwnd=AfxGetMainWnd()->m_hWnd;
	//DWORD proID;//保存进程ID
	//GetWindowThreadProcessId(hwnd,&proID);//获取进程ID
	//char chData[128]={0};
	//sprintf(chData, "%d", proID);
	//InsertSystemString(chData);

	/*tagGangCardResult bucardresult;
	::memset(&bucardresult, 0, sizeof(tagGangCardResult));
	::memcpy(bucardresult.cbCardData, pSendCard->byBuCard, sizeof(BYTE)*pSendCard->byBuCardNums);
	bucardresult.cbCardCount = 5;
	m_GameClientView.m_ControlWnd.SetControlInfo(NULL,cbActionMask,bucardresult);
	m_GameClientView.m_ControlWnd.AddGangCardData(pSendCard->byGangCard, pSendCard->byGangCardNums);
	m_GameClientView.m_ControlWnd.ShowWindow(SW_SHOW);*/


	m_GameClientView.InitViewFrame();

	//SetTaskWndShow(true);
	//播放游戏背景音乐
	PlayMusic("SparrowCS.mp3");	

	SetChatShowTag(true,false);
	return true;
}

//重置框架
void CGameClientDlg::ResetGameFrame()
{
	//游戏变量
	m_wBankerUser=INVALID_CHAIR;
	m_wCurrentUser=INVALID_CHAIR;
	m_bBirdUserID=4;

	//状态变量
	m_bHearStatus=false;
	m_bIsMeCanOutCard = false;

	//托管变量
	m_bStustee=false;
	m_wTimeOutCount =0;

	//声音变量
	m_cbGender=0;
	m_InfoSound="";


	//堆立变量
	m_wHeapHand=0;
	m_wHeapTail=0;
	ZeroMemory(m_cbHeapCardInfo,sizeof(m_cbHeapCardInfo));

	//出牌信息
	m_cbOutCardData=0;
	m_wOutCardUser=INVALID_CHAIR;
	ZeroMemory(m_cbDiscardCard,sizeof(m_cbDiscardCard));
	ZeroMemory(m_cbDiscardCount,sizeof(m_cbDiscardCount));

	//组合扑克
	ZeroMemory(m_cbWeaveCount,sizeof(m_cbWeaveCount));
	ZeroMemory(m_WeaveItemArray,sizeof(m_WeaveItemArray));

	//扑克变量
	m_cbLeftCardCount=0;
	ZeroMemory(m_cbCardIndex,sizeof(m_cbCardIndex));

	return;
}

//游戏设置
void CGameClientDlg::OnGameOptionSet()
{
	//构造数据
	CGameOption GameOption;
	GameOption.m_bEnableSound=IsEnableSound();

	//配置数据
	if (GameOption.DoModal()==IDOK)
	{
		EnableSound(GameOption.m_bEnableSound);
	}

	return;
}

//时间消息
bool CGameClientDlg::OnTimerMessage(WORD wChairID, UINT nElapse, UINT nTimerID)
{
	//return true;

	if (IsLookonMode())
	{
		return true;
	}

	switch (nTimerID)
	{
	case IDI_START_GAME:		//开始游戏
		{
			if (nElapse==0)
			{
				PostMessage(WM_CLOSE);
			//	m_ScoreView.ShowWindow(false);
				return false;
			}
			else if (nElapse<=5) 
			{
				PlaySound("GAME_WARN",false);
			}

			return true;
		}
	case IDI_OPERATE_CARD:		//操作定时器
		{
			//超时判断
			if ((nElapse==0)||(m_bStustee==true))
			{
				//获取位置
				WORD wMeChairID=GetMeChairID();

				//动作处理
				if (wChairID==wMeChairID)
				{
					if(m_bStustee==false)
					{
						m_wTimeOutCount++;
						if(m_wTimeOutCount==3)
						{
							InsertSystemString(TEXT("由于您多次超时，切换为“系统托管”模式."));
							OnStusteeControl(0,0);
							m_GameClientView.UpdateGameView(NULL);
						}
					}

					if( m_GameClientView.m_ControlWnd.m_btChiHu.IsWindowVisible() 
						&& m_GameClientView.m_ControlWnd.m_btChiHu.IsWindowEnabled())
					{
						OnCardOperate(WIK_CHI_HU,0);
					}
					else if (m_wCurrentUser==wMeChairID)
					{
						if (WIK_NULL != m_byUserAction[wMeChairID])
						{
							OnCardOperate(WIK_NULL,0);
						}
						else
						{
							BYTE cbCardData=m_GameClientView.m_HandCardControl.GetCurrentCard();
							OnOutCard(cbCardData,cbCardData);
						}
					}
					else
					{
					  OnCardOperate(WIK_NULL,0);
					}
					return true;
				}
			}

			//播放声音
			if ((nElapse<=3)&&(wChairID==GetMeChairID())) 
			{
				PlaySound("GAME_WARN",false);
			}

			return true;
		}
	case IDI_OTHER_USEROUTCARD:		//其它出牌定时器
		{
			//超时判断
			if ((nElapse==0)||(m_bStustee==true))
			{
				//获取位置
				WORD wMeChairID=GetMeChairID();

				//动作处理
				if (wChairID==wMeChairID && wMeChairID==m_wCurrentUser)
				{
					if(m_bStustee==false)
					{
						m_wTimeOutCount++;
						if(m_wTimeOutCount==3)
						{
							InsertSystemString(TEXT("由于您多次超时，切换为“系统托管”模式."));
							OnStusteeControl(0,0);
						}
					}
					if (m_wCurrentUser==wMeChairID)
					{
						BYTE cbCardData=m_GameClientView.m_HandCardControl.GetCurrentCard();
						OnOutCard(cbCardData,cbCardData);
					}
					return true;
				}
			}

			//播放声音
			if ((nElapse<=3)&&(wChairID==GetMeChairID())) 
			{
				PlaySound("GAME_WARN",false);
			}

			return true;
		}
	case IDI_OPERATE_HAI_DI:	//海底定时器
		{
			//超时判断
			if (nElapse==0 || m_bStustee==true)
			{
				if (wChairID==m_wCurrentUser && wChairID==GetMeChairID())
				{
                   OnOperateHaiDi(0,0);
				}
				return false;
			}

			//播放声音
			if ((nElapse<=3)&&(wChairID==GetMeChairID())) 
			{
				PlaySound("GAME_WARN",false);
			}

			return true;
		}
	}

	return false;
}

//旁观状态
void CGameClientDlg::OnLookonChanged(bool bLookonUser, const void * pBuffer, WORD wDataSize)
{
}

//网络消息
bool CGameClientDlg::OnGameMessage(WORD wSubCmdID, const void * pBuffer, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_S_GAME_START:		//游戏开始
		{
			return OnSubGameStart(pBuffer,wDataSize);
		}
	case SUB_S_OUT_CARD:		//用户出牌
		{
			return OnSubOutCard(pBuffer,wDataSize);
		}
	case SUB_S_SEND_CARD:		//发牌消息
		{
			return OnSubSendCard(pBuffer,wDataSize);
		}
	case SUB_S_OPERATE_NOTIFY:	//操作提示
		{
			return OnSubOperateNotify(pBuffer,wDataSize);
		}
	case SUB_S_OPERATE_RESULT:	//操作结果
		{
			return OnSubOperateResult(pBuffer,wDataSize);
		}
	case SUB_S_OPERATE_HAI_DI:	//海底操作
		{
			return OnSubOperateHaiDi(pBuffer,wDataSize);
		}
	case SUB_S_GAME_END:		//游戏结束
		{
			return OnSubGameEnd(pBuffer,wDataSize);
		}
	case SUB_S_TRUSTEE:			//用户托管
		{
			return OnSubTrustee(pBuffer,wDataSize);
		}
	}

	return true;
}

//游戏场景
bool CGameClientDlg::OnGameSceneMessage(BYTE cbGameStation, bool bLookonOther, const void * pBuffer, WORD wDataSize)
{
	switch (cbGameStation)
	{
	case GS_MJ_FREE:	//空闲状态
		{
			//效验数据
			if (wDataSize!=sizeof(CMD_S_StatusFree)) return false;
			CMD_S_StatusFree * pStatusFree=(CMD_S_StatusFree *)pBuffer;

			//设置数据
			m_wBankerUser=pStatusFree->wBankerUser;
			m_GameClientView.SetCellScore(pStatusFree->lCellScore);
			m_GameClientView.m_HandCardControl.SetDisplayItem(true);
			m_bIsMeCanOutCard = false;

			////设置界面
			//for (WORD i=0;i<GAME_PLAYER;i++)
			//{
			//	m_cbHeapCardInfo[i][0]=0;
			//	m_cbHeapCardInfo[i][1]=HEAP_FULL_COUNT-(((i==m_wBankerUser)||((i+2)%4==m_wBankerUser))?28:26);
			//	m_GameClientView.m_HeapCard[SwitchViewChairID(i)].SetCardData(m_cbHeapCardInfo[i][0],m_cbHeapCardInfo[i][1],HEAP_FULL_COUNT);
			//}

			//设置控件
			if (IsLookonMode()==false)
			{
				m_ScoreView.ShowWindow(SW_HIDE);
				m_GameClientView.m_btStart.ShowWindow(SW_SHOW);
				m_GameClientView.m_btStart.SetFocus();
				SetGameTimer(GetMeChairID(),IDI_START_GAME,TIME_START_GAME);
			}
			//托管设置
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				m_GameClientView.SetTrustee(SwitchViewChairID(i),pStatusFree->bTrustee[i]);
			}

			// 托管按钮隐藏
			if(IsLookonMode()==true)
			{
				m_GameClientView.m_btAutoPlayOn.ShowWindow(SW_HIDE);
				m_GameClientView.m_btAutoPlayOff.ShowWindow(SW_HIDE);
				m_GameClientView.m_btStart.ShowWindow(SW_HIDE);
			}

			//更新界面
			m_GameClientView.UpdateGameView(NULL);

			return true;
		}
	case GS_MJ_PLAY:	//游戏状态
	case GS_MJ_HAI_DI:	//海底状态
		{
			//效验数据
			if (wDataSize!=sizeof(CMD_S_StatusPlay))
			{
				return false;
			}
			CMD_S_StatusPlay * pStatusPlay=(CMD_S_StatusPlay *)pBuffer;
			if (NULL==pStatusPlay)
			{
				return false;
			}
			
			WORD byMeChairID = GetMeChairID();
			bool bIsMeIsLookonMode = IsLookonMode();

			//设置变量
			m_bIsMeCanOutCard = false;
			m_wBankerUser=pStatusPlay->wBankerUser;
			m_wCurrentUser=pStatusPlay->wCurrentUser;
			m_cbLeftCardCount=pStatusPlay->cbLeftCardCount;
			m_bHearStatus=(pStatusPlay->cbHearStatus>0)?true:false;
			memset(m_byUserAction, WIK_NULL, sizeof(m_byUserAction));
		

			//历史变量
			m_wOutCardUser=pStatusPlay->wOutCardUser;
			m_cbOutCardData=pStatusPlay->cbOutCardData;
			CopyMemory(m_cbDiscardCard,pStatusPlay->cbDiscardCard,sizeof(m_cbDiscardCard));
			CopyMemory(m_cbDiscardCount,pStatusPlay->cbDiscardCount,sizeof(m_cbDiscardCount));

			//扑克变量
			CopyMemory(m_cbWeaveCount,pStatusPlay->cbWeaveCount,sizeof(m_cbWeaveCount));
			CopyMemory(m_WeaveItemArray,pStatusPlay->WeaveItemArray,sizeof(m_WeaveItemArray));
			m_GameLogic.SwitchToCardIndex(pStatusPlay->cbCardData,pStatusPlay->cbCardCount,m_cbCardIndex);

			//辅助变量
			WORD wViewChairID[GAME_PLAYER]={0,0,0,0};
			for (WORD i=0;i<GAME_PLAYER;i++) 
			{
				wViewChairID[i]=SwitchViewChairID(i);
			}

			//界面设置
			m_GameClientView.SetCellScore(pStatusPlay->lCellScore);
			m_GameClientView.SetBankerUser(wViewChairID[m_wBankerUser]);



			//托管设置
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				m_GameClientView.SetTrustee(SwitchViewChairID(i),pStatusPlay->bTrustee[i]);
			}

			//组合扑克
			BYTE cbWeaveCard[4]={0,0,0,0};
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				for (BYTE j=0;j<m_cbWeaveCount[i];j++)
				{
					BYTE cbWeaveKind=m_WeaveItemArray[i][j].cbWeaveKind;
					BYTE cbCenterCard=m_WeaveItemArray[i][j].cbCenterCard;
					BYTE cbWeaveCardCount=m_GameLogic.GetWeaveCard(cbWeaveKind,cbCenterCard,cbWeaveCard);
					m_GameClientView.m_WeaveCard[wViewChairID[i]][j].SetCardData(cbWeaveCard,cbWeaveCardCount);
					if (cbWeaveKind&(WIK_GANG|WIK_FILL|WIK_TING))
					{
						if (m_WeaveItemArray[i][j].wProvideUser==i)
						{
							m_GameClientView.m_WeaveCard[wViewChairID[i]][j].SetDisplayItem(false);
						}
					}
				}
			}

			//用户扑克
			if (m_wCurrentUser==byMeChairID && cbGameStation!=GS_MJ_HAI_DI)
			{
				BYTE cbCardCount=pStatusPlay->cbCardCount;
				m_GameClientView.m_HandCardControl.SetCardData(pStatusPlay->cbCardData,cbCardCount-1,pStatusPlay->cbCardData[0]);
			}
			else
			{
				m_GameClientView.m_HandCardControl.SetCardData(pStatusPlay->cbCardData,pStatusPlay->cbCardCount,0); 
			}

			//扑克设置
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				//用户扑克
				if (i!=byMeChairID)
				{
					BYTE cbCardCount=13-m_cbWeaveCount[i]*3;
					WORD wUserCardIndex=(wViewChairID[i]<2)?wViewChairID[i]:2;
					m_GameClientView.m_UserCard[wUserCardIndex].SetCardData(cbCardCount,(m_wCurrentUser==i));
				}

				//丢弃扑克
				WORD wViewChairID=SwitchViewChairID(i);
				m_GameClientView.m_DiscardCard[wViewChairID].SetCardData(m_cbDiscardCard[i],m_cbDiscardCount[i]);
			}

			//控制设置
			if (bIsMeIsLookonMode==false) 
			{
				m_GameClientView.m_HandCardControl.SetPositively(true);
				m_GameClientView.m_HandCardControl.SetDisplayItem(true);
			}

			//堆立扑克
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				m_cbHeapCardInfo[i][0]=0;
				m_cbHeapCardInfo[i][1]=HEAP_FULL_COUNT-(((i==m_wBankerUser)||((i+2)%4==m_wBankerUser))?28:26);
			}

			//分发扑克
			BYTE cbTakeCount=108-m_cbLeftCardCount;   
			BYTE cbSiceFirst=HIBYTE(pStatusPlay->wSiceCount);   
			BYTE cbSiceSecond=LOBYTE(pStatusPlay->wSiceCount);
			WORD wTakeChairID=(m_wBankerUser+7-cbSiceFirst)%GAME_PLAYER;
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				//计算数目
				BYTE cbValidCount=HEAP_FULL_COUNT-m_cbHeapCardInfo[wTakeChairID][1]-((i==0)?(cbSiceSecond-1)*2:0);
				BYTE cbRemoveCount=__min(cbValidCount,cbTakeCount);

				//提取扑克
				cbTakeCount-=cbRemoveCount;
				m_cbHeapCardInfo[wTakeChairID][(i==0)?1:0]+=cbRemoveCount;
 
                //确定断线重回最后一张牌的位置
				//if ((cbGameStation==GS_MJ_HAI_DI) && (0==i) && (cbRemoveCount>0 && cbRemoveCount<HEAP_FULL_COUNT))
				//{
				//	m_cbHeapCardInfo[wTakeChairID][0]=HEAP_FULL_COUNT-cbRemoveCount-1;
				//	m_cbHeapCardInfo[wTakeChairID][1]=cbRemoveCount;
				//}

				//完成判断
				if (cbTakeCount==0)
				{
					m_wHeapHand=wTakeChairID;
					m_wHeapTail=(m_wBankerUser+7-cbSiceFirst)%GAME_PLAYER;
					break;
				}

				//切换索引
				wTakeChairID=(wTakeChairID+1)%GAME_PLAYER;
			}

			//堆立界面
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				m_GameClientView.m_HeapCard[wViewChairID[i]].SetCardData(m_cbHeapCardInfo[i][0],m_cbHeapCardInfo[i][1],HEAP_FULL_COUNT);
			}

			//历史扑克
			if (m_wOutCardUser!=INVALID_CHAIR)
			{
				WORD wOutChairID=SwitchViewChairID(m_wOutCardUser);
				m_GameClientView.SetOutCardInfo(wOutChairID,m_cbOutCardData);
			}

			//托管按钮
			m_bStustee = pStatusPlay->bTrustee[GetMeChairID()];
			if (m_bStustee)
			{
				m_GameClientView.m_btAutoPlayOff.ShowWindow(true);
			}
			else
			{
				m_GameClientView.m_btAutoPlayOn.ShowWindow(true);
			}
			//m_GameClientView.m_btStusteeControl.InvalidateRect(NULL);



			//出牌设置
			if (cbGameStation==GS_MJ_PLAY && !bIsMeIsLookonMode)
			{
			     if (byMeChairID == m_wCurrentUser)
				 {
					{//主动玩家
						if (pStatusPlay->cbActionMask!=WIK_NULL)
						{//操作,主动玩家
							m_byUserAction[byMeChairID] = pStatusPlay->cbActionMask;
							//获取变量
							BYTE cbActionMask=pStatusPlay->cbActionMask;
							BYTE cbActionCard=pStatusPlay->cbActionCard;

							//变量定义
							tagGangCardResult GangCardResult;
							ZeroMemory(&GangCardResult,sizeof(GangCardResult));
							//变量定义
							tagGangCardResult bucardresult;
							ZeroMemory(&bucardresult,sizeof(tagGangCardResult));
							//显示界面
							::memcpy(bucardresult.cbCardData, pStatusPlay->byBuCard, sizeof(BYTE)*pStatusPlay->byBuCardNums);
							bucardresult.cbCardCount = pStatusPlay->byBuCardNums;
							m_GameClientView.m_ControlWnd.SetControlInfo(pStatusPlay->cbActionCard,cbActionMask,bucardresult);
							m_GameClientView.m_ControlWnd.AddGangCardData(pStatusPlay->byGangCard, pStatusPlay->byGangCardNums);

							if (m_bHearStatus)
							{
								if (pStatusPlay->cbActionMask&WIK_CHI_HU)
								{
									//点击胡
									OnCardOperate(WIK_CHI_HU,pStatusPlay->cbActionCard);
									return true;
								}
								//取消,并且出牌,并设置定时器出牌.
								OnCardOperate(WIK_NULL,0);
								BYTE cbCardData=m_GameClientView.m_HandCardControl.GetCurrentCard();
								OnOutCard(cbCardData,cbCardData);
							}
							else
							{
								//设置定时器操作
								m_bIsMeCanOutCard  = true;
								SetGameTimer(byMeChairID,IDI_OPERATE_CARD,TIME_OPERATE_CARD);
							}
						}
						else
						{//出牌
							m_byUserAction[byMeChairID] = WIK_NULL;
							m_wCurrentUser = byMeChairID;
							memset(m_byUserAction, 0, sizeof(m_byUserAction));
							SetGameTimer(byMeChairID,IDI_OTHER_USEROUTCARD,TIME_HEAR_STATUS-10);
							//出牌提示
							m_GameClientView.SetStatusFlag(true,false);
						}
					}
				 }
				 else
				 {//被动
						if ((pStatusPlay->cbActionMask!=WIK_NULL) /*&& (!pStatusPlay->bIsAlreadyOper)*/)
						{//操作
							//获取变量
							BYTE cbActionMask=pStatusPlay->cbActionMask;
							BYTE cbActionCard=pStatusPlay->cbActionCard;

							m_byUserAction[byMeChairID] = pStatusPlay->cbActionMask;
							//变量定义
							tagGangCardResult GangCardResult;
							ZeroMemory(&GangCardResult,sizeof(GangCardResult));
							//变量定义
							tagGangCardResult bucardresult;
							ZeroMemory(&bucardresult,sizeof(tagGangCardResult));
							//显示界面
							::memcpy(bucardresult.cbCardData, pStatusPlay->byBuCard, sizeof(BYTE)*pStatusPlay->byBuCardNums);
							bucardresult.cbCardCount = pStatusPlay->byBuCardNums;
							m_GameClientView.m_ControlWnd.SetControlInfo(pStatusPlay->cbActionCard,cbActionMask,bucardresult);
							m_GameClientView.m_ControlWnd.AddGangCardData(pStatusPlay->byGangCard, pStatusPlay->byGangCardNums);

							//设置定时器操作
							m_wCurrentUser = byMeChairID;
							SetGameTimer(byMeChairID,IDI_OPERATE_CARD,TIME_OPERATE_CARD);
						}
				}
			}


			//海底设置
			else if (cbGameStation==GS_MJ_HAI_DI && !bIsMeIsLookonMode)
			{
				m_GameClientView.m_btHaiDi.ShowWindow(SW_HIDE);
				m_GameClientView.m_btNoHaiDi.ShowWindow(SW_HIDE);
				//设置控制
				if (byMeChairID==m_wCurrentUser)
				{
					m_GameClientView.m_btHaiDi.ShowWindow(SW_SHOW);
					m_GameClientView.m_btNoHaiDi.ShowWindow(SW_SHOW);
				}

				//设置时间
				SetGameTimer(m_wCurrentUser,IDI_OPERATE_HAI_DI,TIME_OPERATE_HAI_DI);
			}

			// 托管按钮隐藏
			if(bIsMeIsLookonMode==true)
			{
				m_GameClientView.m_btHaiDi.ShowWindow(SW_HIDE);
				m_GameClientView.m_btNoHaiDi.ShowWindow(SW_HIDE);
				m_GameClientView.m_btAutoPlayOn.ShowWindow(SW_HIDE);
				m_GameClientView.m_btAutoPlayOff.ShowWindow(SW_HIDE);
			}
			m_GameClientView.m_btStart.ShowWindow(SW_HIDE);

			//更新界面
			m_GameClientView.UpdateGameView(NULL);
			return true;
		}
	}

	return false;
}

//清空所有定时器
void CGameClientDlg::ClearAllTimer()
{
	KillGameTimer(IDI_START_GAME);
	KillGameTimer(IDI_OPERATE_CARD);
	KillGameTimer(IDI_OPERATE_HAI_DI);
	KillGameTimer(IDI_OTHER_USEROUTCARD);
}

//游戏开始
bool CGameClientDlg::OnSubGameStart(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_GameStart));
	if (wDataSize!=sizeof(CMD_S_GameStart))
	{
		return false;
	}

	//变量定义
	CMD_S_GameStart * pGameStart=(CMD_S_GameStart *)pBuffer;
	if (NULL == pGameStart)
	{
		return false;
	}


	::memset(&m_GameStartData, 0, sizeof(CMD_S_GameStart));
	::memcpy(&m_GameStartData, pGameStart, sizeof(CMD_S_GameStart));
	
	//清空所有定时器
	ClearAllTimer();
	m_bIsMeCanOutCard = false;
	//设置状态
	SetGameStatus(GS_MJ_PLAY);
	//m_GameClientView.SetDiceEffect(true);
	SetTimer(IDI_SHOW_SZ,TIME_SHOW_SZ,NULL);

	//设置变量
	m_bHearStatus=false;
	m_wBankerUser=pGameStart->wBankerUser;
	m_wCurrentUser=pGameStart->wCurrentUser;
	m_cbLeftCardCount=MAX_REPERTORY-GAME_PLAYER*(MAX_COUNT-1)-1;

	m_GameClientView.m_nDiceCount1 =HIBYTE( pGameStart->wSiceCount-1);
	m_GameClientView.m_nDiceCount2 =LOBYTE( pGameStart->wSiceCount-1);

	//出牌信息
	m_cbOutCardData=0;
	m_wOutCardUser=INVALID_CHAIR;
	ZeroMemory(m_cbDiscardCard,sizeof(m_cbDiscardCard));
	ZeroMemory(m_cbDiscardCount,sizeof(m_cbDiscardCount));

	//组合扑克
	ZeroMemory(m_cbWeaveCount,sizeof(m_cbWeaveCount));
	ZeroMemory(m_WeaveItemArray,sizeof(m_WeaveItemArray));

	//设置扑克
	BYTE cbCardCount=(GetMeChairID()==m_wBankerUser)?MAX_COUNT:(MAX_COUNT-1);
	m_GameLogic.SwitchToCardIndex(pGameStart->cbCardData,cbCardCount,m_cbCardIndex);

	//设置界面
	bool bPlayerMode=(IsLookonMode()==false);
	m_GameClientView.m_HandCardControl.SetPositively(bPlayerMode);
	m_GameClientView.SetBankerUser(SwitchViewChairID(m_wBankerUser));
	m_ScoreView.ShowWindow(SW_HIDE);
	m_GameClientView.m_btStart.ShowWindow(SW_HIDE);

	return true;

}

//用户出牌
bool CGameClientDlg::OnSubOutCard(const void * pBuffer, WORD wDataSize)
{
	//效验消息
	ASSERT(wDataSize==sizeof(CMD_S_OutCard));
	if (wDataSize!=sizeof(CMD_S_OutCard)) return false;

	//消息处理
	CMD_S_OutCard * pOutCard=(CMD_S_OutCard *)pBuffer;

	//变量定义
	WORD wMeChairID=GetMeChairID();
	WORD wOutViewChairID=SwitchViewChairID(pOutCard->wOutCardUser);

	//设置变量
	m_wCurrentUser=INVALID_CHAIR;
	m_wOutCardUser=pOutCard->wOutCardUser;
	m_cbOutCardData=pOutCard->cbOutCardData;
	m_bIsMeCanOutCard = false;

	KillGameTimer(IDI_OPERATE_CARD); 
	KillGameTimer(IDI_OTHER_USEROUTCARD);


	m_GameClientView.SetStatusFlag(false,false);

	memset(m_byUserAction, 0 , sizeof(m_byUserAction));

	////丢弃扑克
	//if ((m_wOutCardUser!=INVALID_CHAIR)&&(m_cbOutCardData!=0))
	//{
	//	//丢弃扑克 
	//	WORD wOutViewChairID=SwitchViewChairID(m_wOutCardUser);
	//	m_GameClientView.m_DiscardCard[wOutViewChairID].AddCardItem(m_cbOutCardData);
	//}

	//其他用户
	if ((IsLookonMode()==true)||(pOutCard->wOutCardUser!=wMeChairID))
	{
		//环境设置
		
		//PlayCardSound(pOutCard->cbOutCardData);
		//出牌界面
		m_GameClientView.SetUserAction(INVALID_CHAIR,0);
		m_GameClientView.SetOutCardInfo(wOutViewChairID,pOutCard->cbOutCardData);

		//设置扑克
		if (wOutViewChairID==2)
		{
			//删除扑克
			BYTE cbCardData[MAX_COUNT];
			m_GameLogic.RemoveCard(m_cbCardIndex,pOutCard->cbOutCardData);

			//设置扑克
			BYTE cbCardCount=m_GameLogic.SwitchToCardData(m_cbCardIndex,cbCardData);
			m_GameClientView.m_HandCardControl.SetCardData(cbCardData,cbCardCount,0);
		}
		else
		{
			WORD wUserIndex=(wOutViewChairID>2)?2:wOutViewChairID;
			m_GameClientView.m_UserCard[wUserIndex].SetCurrentCard(false);
		}
	} 
	//声音
	const tagUserData *pUserData = GetUserData(pOutCard->wOutCardUser);//用户信息
	PlayCardSound(pUserData->cbGender, pOutCard->cbOutCardData);
	m_GameClientView.UpdateGameView(NULL);

	return true;
}

//发牌消息
bool CGameClientDlg::OnSubSendCard(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_SendCard));
	if (wDataSize!=sizeof(CMD_S_SendCard)) return false;

	//变量定义
	CMD_S_SendCard * pSendCard=(CMD_S_SendCard *)pBuffer;
	if (NULL == pSendCard)
	{
		return false;
	}

	m_GameClientView.m_ControlWnd.ShowWindow(SW_HIDE);

	//清空所有定时器
	ClearAllTimer();
	m_bIsMeCanOutCard = false;
	
	//设置变量
	WORD wMeChairID=GetMeChairID();
	m_wCurrentUser=pSendCard->wCurrentUser;

	//丢弃扑克
	if ((m_wOutCardUser!=INVALID_CHAIR)&&(m_cbOutCardData!=0))
	{
		//丢弃扑克 
		WORD wOutViewChairID=SwitchViewChairID(m_wOutCardUser);
		m_GameClientView.m_DiscardCard[wOutViewChairID].AddCardItem(m_cbOutCardData);

		////设置变量
		//m_cbOutCardData=0;
		//m_wOutCardUser=INVALID_CHAIR;
	}

	//设置变量
	m_cbOutCardData=0;
	m_wOutCardUser=INVALID_CHAIR;

	//发牌处理
	if (pSendCard->cbCardData!=0)
	{
		//取牌界面
		WORD wViewChairID=SwitchViewChairID(m_wCurrentUser);
		if (wViewChairID!=2)
		{
			WORD wUserIndex=(wViewChairID>2)?2:wViewChairID;
			m_GameClientView.m_UserCard[wUserIndex].SetCurrentCard(true);
		}
		else
		{
			m_cbCardIndex[m_GameLogic.SwitchToCardIndex(pSendCard->cbCardData)]++;
			m_GameClientView.m_HandCardControl.SetCurrentCard(pSendCard->cbCardData);
		}

		//扣除扑克
		if ( m_byUserAction[pSendCard->wCurrentUser] != WIK_NULL 
			&& (m_byUserAction[pSendCard->wCurrentUser] & WIK_FILL 
				||m_byUserAction[pSendCard->wCurrentUser] & WIK_GANG) )
		{
			DeductionTableCard(false);
		}
		else
		{
			DeductionTableCard(true);
		}	
	}

	::memset(m_byUserAction, 0, sizeof(m_byUserAction));

	//当前用户
	if ((IsLookonMode()==false)&&(m_wCurrentUser==wMeChairID))
	{
		m_byUserAction[wMeChairID] = pSendCard->cbActionMask;
		//动作处理
		if (pSendCard->cbActionMask!=WIK_NULL)
		{
			//获取变量
			BYTE cbActionCard=pSendCard->cbCardData;
			BYTE cbActionMask=pSendCard->cbActionMask;

			//变量定义

			tagGangCardResult GangCardResult;
			ZeroMemory(&GangCardResult,sizeof(GangCardResult));

			////杠牌判断
			//if ((cbActionMask&(WIK_GANG|WIK_FILL))!=0)
			//{
			//	WORD wMeChairID=GetMeChairID();
			//	m_GameLogic.AnalyseGangCard(m_cbCardIndex,m_WeaveItemArray[wMeChairID],m_cbWeaveCount[wMeChairID],GangCardResult);
			//}
			//tagChiHuResult ChiHuResult;
			//m_GameLogic.AnalyseChiHuCard(m_cbCardIndex[wMeChairID],
			//m_WeaveItemArray[wMeChairID],m_cbWeaveCount[wMeChairID],0,wChiHuRight,ChiHuResult);

			//显示界面
			tagGangCardResult bucardresult;
			::memset(&bucardresult, 0, sizeof(tagGangCardResult));
			::memcpy(bucardresult.cbCardData, pSendCard->byBuCard, sizeof(BYTE)*pSendCard->byBuCardNums);
			bucardresult.cbCardCount = pSendCard->byBuCardNums;
			m_GameClientView.m_ControlWnd.SetControlInfo(pSendCard->cbCardData,cbActionMask,bucardresult);
			m_GameClientView.m_ControlWnd.AddGangCardData(pSendCard->byGangCard, pSendCard->byGangCardNums);
			m_GameClientView.m_ControlWnd.ShowWindow(SW_SHOW);

			//设置界面
			//m_GameClientView.m_ControlWnd.SetControlInfo(cbActionCard,cbActionMask,GangCardResult);
			//激活框架
		}

		if (m_bHearStatus)
		{//m_GameClientView.m_ControlWnd.m_btChiHu.IsWindowVisible() && m_GameClientView.m_ControlWnd.m_btChiHu.IsWindowEnabled()
			if(pSendCard->cbActionMask&WIK_CHI_HU)
			{
				OnCardOperate(WIK_CHI_HU,0);
			}
			else
			{
				if (WIK_NULL != m_byUserAction[wMeChairID])
				{
					OnCardOperate(WIK_NULL,0);
				}
				else
				{
					BYTE cbCardData=m_GameClientView.m_HandCardControl.GetCurrentCard();
					OnOutCard(cbCardData,cbCardData);
				}
			}
			return true;
		}

		if (pSendCard->cbActionMask!=WIK_NULL)
		{
			m_bIsMeCanOutCard = true;
			//操作牌
			SetGameTimer(m_wCurrentUser,IDI_OPERATE_CARD,TIME_OPERATE_CARD);
		}
		else
		{
			//出牌
			SetGameTimer(m_wCurrentUser,IDI_OPERATE_CARD,TIME_HEAR_STATUS);
		}
		//出牌提示
		m_GameClientView.SetStatusFlag(true,false);
		ActiveGameFrame();
	}
	else
	{
		SetGameTimer(m_wCurrentUser,IDI_OPERATE_CARD,TIME_HEAR_STATUS);
		//出牌提示
		m_GameClientView.SetStatusFlag(false,false);
	}

	if ((IsLookonMode()==false)&&(m_wCurrentUser==wMeChairID))
	{
		if ((WIK_CHI_HU&pSendCard->cbActionMask)>0)
		{
			tagGangCardResult GangCardResult;
			ZeroMemory(&GangCardResult,sizeof(GangCardResult));
			m_GameClientView.m_ControlWnd.SetControlInfo(pSendCard->cbCardData,pSendCard->cbActionMask,GangCardResult);
			InsertSystemString("success!");
		}
		else if ( m_bHearStatus)
		{//听牌
			OnOutCard(m_GameClientView.m_HandCardControl.GetCurrentCard(), pSendCard->cbCardData );
		}
	}

	//更新界面
	m_GameClientView.UpdateGameView(NULL);
	
	return true;
}

//操作提示
bool CGameClientDlg::OnSubOperateNotify(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_OperateNotify));
	if (wDataSize!=sizeof(CMD_S_OperateNotify)) return false;

	//变量定义
	CMD_S_OperateNotify * pOperateNotify=(CMD_S_OperateNotify *)pBuffer;

	m_bIsMeCanOutCard = false;
	KillGameTimer(IDI_OPERATE_CARD);
	BYTE byChairID = GetMeChairID();
	m_byUserAction[byChairID] = WIK_NULL;
	//用户界面
	if ((IsLookonMode()==false)&&(pOperateNotify->cbActionMask!=WIK_NULL))
	{
		m_wCurrentUser = byChairID;
		m_byUserAction[byChairID] = pOperateNotify->cbActionMask;
		//获取变量
		BYTE cbActionMask=pOperateNotify->cbActionMask;
		BYTE cbActionCard=pOperateNotify->cbActionCard;

		//变量定义
		tagGangCardResult bucardresult;
		ZeroMemory(&bucardresult,sizeof(tagGangCardResult));


		//显示界面
		::memcpy(bucardresult.cbCardData, pOperateNotify->byBuCard, sizeof(BYTE)*pOperateNotify->byBuCardNums);
		bucardresult.cbCardCount = pOperateNotify->byBuCardNums;
		m_GameClientView.m_ControlWnd.SetControlInfo(pOperateNotify->cbActionCard,cbActionMask,bucardresult);
		m_GameClientView.m_ControlWnd.AddGangCardData(pOperateNotify->byGangCard, pOperateNotify->byGangCardNums);

		SetGameTimer(byChairID,IDI_OPERATE_CARD,TIME_OPERATE_CARD);

		//设置界面
		ActiveGameFrame();
	}

	return true;
}

//操作结果
bool CGameClientDlg::OnSubOperateResult(const void * pBuffer, WORD wDataSize)
{
	//效验消息
	ASSERT(wDataSize==sizeof(CMD_S_OperateResult));
	if (wDataSize!=sizeof(CMD_S_OperateResult)) return false;

	//消息处理
	CMD_S_OperateResult * pOperateResult=(CMD_S_OperateResult *)pBuffer;

	//变量定义
	BYTE cbPublicCard=TRUE;
	WORD wOperateUser=pOperateResult->wOperateUser;
	BYTE cbOperateCard=pOperateResult->cbOperateCard;
	WORD wOperateViewID=SwitchViewChairID(wOperateUser);

	//出牌变量
	if (pOperateResult->cbOperateCode!=WIK_NULL)
	{
		m_cbOutCardData=0;
		m_wOutCardUser=INVALID_CHAIR;
	}

	KillGameTimer(IDI_OTHER_USEROUTCARD);
	m_GameClientView.SetOutCardInfo(INVALID_CHAIR,0);
	m_GameClientView.m_ControlWnd.ShowWindow(SW_HIDE);

	m_bIsMeCanOutCard = false;
	memset(m_byUserAction, 0, sizeof(m_byUserAction));
	//设置组合
	if ((pOperateResult->cbOperateCode&(WIK_GANG|WIK_FILL|WIK_TING))>0)
	{//主动玩家,并且是补、杠、听
		//为当前操作玩家
		m_wCurrentUser=INVALID_CHAIR;
		//设置变量
		if ((pOperateResult->cbOperateCode==WIK_TING)&&(wOperateUser==GetMeChairID())) 
		{
			m_bHearStatus = true;
		}

		//组合扑克
		BYTE cbWeaveIndex=0xFF;
		for (BYTE i=0;i<m_cbWeaveCount[wOperateUser];i++)
		{
			BYTE cbWeaveKind=m_WeaveItemArray[wOperateUser][i].cbWeaveKind;
			BYTE cbCenterCard=m_WeaveItemArray[wOperateUser][i].cbCenterCard;
			if ((cbCenterCard==cbOperateCard)&&(cbWeaveKind==WIK_PENG))
			{
				cbWeaveIndex=i;
				m_WeaveItemArray[wOperateUser][cbWeaveIndex].cbPublicCard=TRUE;
				m_WeaveItemArray[wOperateUser][cbWeaveIndex].cbWeaveKind=pOperateResult->cbOperateCode;
				m_WeaveItemArray[wOperateUser][cbWeaveIndex].wProvideUser=pOperateResult->wProvideUser;
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
			m_WeaveItemArray[wOperateUser][cbWeaveIndex].cbCenterCard=cbOperateCard;
			m_WeaveItemArray[wOperateUser][cbWeaveIndex].cbWeaveKind=pOperateResult->cbOperateCode;
			m_WeaveItemArray[wOperateUser][cbWeaveIndex].wProvideUser=pOperateResult->wProvideUser;
		}

		//组合界面
		BYTE cbWeaveCard[4]={0,0,0,0},cbWeaveKind=pOperateResult->cbOperateCode;
		BYTE cbWeaveCardCount=m_GameLogic.GetWeaveCard(cbWeaveKind,cbOperateCard,cbWeaveCard);
		m_GameClientView.m_WeaveCard[wOperateViewID][cbWeaveIndex].SetCardData(cbWeaveCard,cbWeaveCardCount);
		m_GameClientView.m_WeaveCard[wOperateViewID][cbWeaveIndex].SetDisplayItem((cbPublicCard==1)?true:false);

		//扑克设置
		if (GetMeChairID()==wOperateUser)
		{
			m_cbCardIndex[m_GameLogic.SwitchToCardIndex(pOperateResult->cbOperateCard)]=0;
		}

		//设置扑克
		if (GetMeChairID()==wOperateUser)
		{
			BYTE cbCardData[MAX_COUNT];
			BYTE cbCardCount=m_GameLogic.SwitchToCardData(m_cbCardIndex,cbCardData);
			m_GameClientView.m_HandCardControl.SetCardData(cbCardData,cbCardCount,0);
		}
		else
		{
			WORD wUserIndex=(wOperateViewID>=3)?2:wOperateViewID;
			BYTE cbCardCount=MAX_COUNT-m_cbWeaveCount[wOperateUser]*3;
			m_GameClientView.m_UserCard[wUserIndex].SetCardData(cbCardCount-1,false);
		}
	}
	else if (pOperateResult->cbOperateCode!=WIK_NULL)
	{//被动玩家，吃、碰的情况

		//为当前操作玩家
		m_wCurrentUser=pOperateResult->wOperateUser;

		//设置组合
		BYTE cbWeaveIndex=m_cbWeaveCount[wOperateUser]++;
		m_WeaveItemArray[wOperateUser][cbWeaveIndex].cbPublicCard=TRUE;
		m_WeaveItemArray[wOperateUser][cbWeaveIndex].cbCenterCard=cbOperateCard;
		m_WeaveItemArray[wOperateUser][cbWeaveIndex].cbWeaveKind=pOperateResult->cbOperateCode;
		m_WeaveItemArray[wOperateUser][cbWeaveIndex].wProvideUser=pOperateResult->wProvideUser;

		//组合界面
		BYTE cbWeaveCard[4]={0,0,0,0},cbWeaveKind=pOperateResult->cbOperateCode;
		BYTE cbWeaveCardCount=m_GameLogic.GetWeaveCard(cbWeaveKind,cbOperateCard,cbWeaveCard);
		m_GameClientView.m_WeaveCard[wOperateViewID][cbWeaveIndex].SetCardData(cbWeaveCard,cbWeaveCardCount);

		//删除扑克
		if (GetMeChairID()==wOperateUser)
		{
			m_GameLogic.RemoveCard(cbWeaveCard,cbWeaveCardCount,&cbOperateCard,1);
			m_GameLogic.RemoveCard(m_cbCardIndex,cbWeaveCard,cbWeaveCardCount-1);
		}

		//设置扑克
		if (GetMeChairID()==wOperateUser)
		{
			BYTE cbCardData[MAX_COUNT];
			BYTE cbCardCount=m_GameLogic.SwitchToCardData(m_cbCardIndex,cbCardData);
			m_GameClientView.m_HandCardControl.SetCardData(cbCardData,cbCardCount-1,cbCardData[cbCardCount-1]);
		}
		else
		{
			WORD wUserIndex=(wOperateViewID>=3)?2:wOperateViewID;
			BYTE cbCardCount=MAX_COUNT-m_cbWeaveCount[wOperateUser]*3;
			m_GameClientView.m_UserCard[wUserIndex].SetCardData(cbCardCount-1,true);
		}

		if (GetMeChairID()==m_wCurrentUser)
		{//我是操作玩家,要打出一张牌
			//设置时间
			SetGameTimer(m_wCurrentUser,IDI_OPERATE_CARD,TIME_HEAR_STATUS);
		}
	}

	//设置界面
	m_GameClientView.SetUserAction(wOperateViewID,pOperateResult->cbOperateCode);
	m_GameClientView.SetStatusFlag((IsLookonMode()==false)&&(m_wCurrentUser==GetMeChairID()),false);
	m_GameClientView.PlayOperationFlash(wOperateViewID);

	//更新界面
	m_GameClientView.UpdateGameView(NULL);

	//环境设置
	const tagUserData *pUserData = GetUserData(pOperateResult->wOperateUser);//用户信息
	PlayActionSound(pUserData->cbGender, pOperateResult->cbOperateCode);

	return true;
}

//海底操作
bool CGameClientDlg::OnSubOperateHaiDi(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_OperateHaiDi));
	if (wDataSize!=sizeof(CMD_S_OperateHaiDi)) return false;

	//消息处理
	CMD_S_OperateHaiDi * pOperateHaiDi=(CMD_S_OperateHaiDi *)pBuffer;

	//设置状态
	SetGameStatus(GS_MJ_HAI_DI);

	//设置变量
	m_bIsMeCanOutCard = false;
	m_wCurrentUser=INVALID_CHAIR;
	m_GameClientView.SetStatusFlag(false,false);
	m_GameClientView.m_ControlWnd.ShowWindow(SW_HIDE);

	//丢弃扑克
	if ((m_wOutCardUser!=INVALID_CHAIR)&&(m_cbOutCardData!=0))
	{
		//丢弃扑克
		WORD wOutViewChairID=SwitchViewChairID(m_wOutCardUser);
		m_GameClientView.m_DiscardCard[wOutViewChairID].AddCardItem(m_cbOutCardData);

		//设置变量
		m_cbOutCardData=0;
		m_wOutCardUser=INVALID_CHAIR;
	}

	//界面设置
	if ((IsLookonMode()==false)&&(GetMeChairID()==pOperateHaiDi->wCurrentUser))
	{
		m_wCurrentUser = GetMeChairID();
		m_GameClientView.m_btHaiDi.ShowWindow(SW_SHOW);
		m_GameClientView.m_btNoHaiDi.ShowWindow(SW_SHOW);
		ActiveGameFrame();
	}

	//时间设置
	SetGameTimer(pOperateHaiDi->wCurrentUser,IDI_OPERATE_HAI_DI,TIME_OPERATE_HAI_DI);

	return true;
}

//游戏结束
bool CGameClientDlg::OnSubGameEnd(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_GameEnd));
	if (wDataSize!=sizeof(CMD_S_GameEnd)) return false;

	//消息处理
	CMD_S_GameEnd * pGameEnd=(CMD_S_GameEnd *)pBuffer;
	if (NULL == pGameEnd )
	{
		return false;
	}


	//删除定时器
	ClearAllTimer();

	//设置状态
	SetGameStatus(GS_MJ_FREE);
	m_GameClientView.SetStatusFlag(false,false);

	
	//设置控件
	m_bIsMeCanOutCard = false;
	//m_GameClientView.SetStatusFlag(false,false);
	m_GameClientView.m_btHaiDi.ShowWindow(SW_HIDE);
	m_GameClientView.m_btNoHaiDi.ShowWindow(SW_HIDE);
	m_GameClientView.m_ControlWnd.ShowWindow(SW_HIDE);
	m_GameClientView.m_HandCardControl.SetPositively(false);
	for (int i=0;i<GAME_PLAYER;i++)
	{
		m_GameClientView.SetOutCardInfo(i, 0);
	}
	


	WORD MychairId=GetMeChairID();
	WORD BankerchairID=SwitchViewChairID(m_wBankerUser);
	m_ScoreView.GetViewID(MychairId,BankerchairID);

	m_GameClientView.m_wChiHuType=pGameEnd->iChiHuType;  //  0:荒庄 ;  1:自摸 ; 2:放炮;  3:海底捞月;  4:海底炮

	if (/*pGameEnd->cbHaiDiCard==0*/false == pGameEnd->bIsHuangzhuang)
	{
		tagGameScoreInfo GameScoreInfo;
		GameScoreInfo.iCellScore=pGameEnd->iCellScore;
		GameScoreInfo.wBankerUser=m_wBankerUser;
		GameScoreInfo.cbBirdCard=pGameEnd->cbBirdCard;
		GameScoreInfo.cbChiHuCard=pGameEnd->cbChiHuCard;
		GameScoreInfo.wProvideUser=pGameEnd->wProvideUser;
		GameScoreInfo.wBirdHitUser=pGameEnd->wBirdHitUser;

		//成绩显示在即时聊天对话框
		TCHAR szBuffer[64];
		_snprintf(szBuffer,CountArray(szBuffer),TEXT("\n本局结束,成绩统计:"));
		InsertGeneralString(szBuffer,RGB(255,0,0),true);
		for (WORD i=0;i<GAME_PLAYER;i++)
		{
			const tagUserData * pUserData=GetUserData(i);
			GameScoreInfo.cbTimes[i]=pGameEnd->cbTimes[i];
			GameScoreInfo.iCellScore=pGameEnd->iCellScore;
			GameScoreInfo.lScore[i]=pGameEnd->lGameScore[i];
			GameScoreInfo.wCardType[i]=pGameEnd->wChiHuKind[i];
			GameScoreInfo.iChiHuKind[i]=pGameEnd->iChiHuKind[i];
			lstrcpyn(GameScoreInfo.szUserName[i],pUserData->szName,CountArray(GameScoreInfo.szUserName[i]));

			//变量定义
			if (GameScoreInfo.szUserName[i]!=0)
			{
				WORD wIndex=((i+GAME_PLAYER)-GameScoreInfo.wBankerUser)%GAME_PLAYER;
				WORD wViewID=SwitchViewChairID(i);
				if (2==wViewID)
				{
					_snprintf(szBuffer,CountArray(szBuffer),TEXT("%s："),GameScoreInfo.szUserName[i]);
				} 
				else
				{
					if (0==wIndex)
					{
						_snprintf(szBuffer,CountArray(szBuffer),TEXT("%s："),"庄家");
					}
					else if (1==wIndex)
					{
						_snprintf(szBuffer,CountArray(szBuffer),TEXT("%s："),"南方");
					}
					else if (2==wIndex)
					{
						_snprintf(szBuffer,CountArray(szBuffer),TEXT("%s："),"西方");
					}
					else
					{
						_snprintf(szBuffer,CountArray(szBuffer),TEXT("%s："),"北方");
					}
					
				}
				InsertGeneralString(szBuffer,RGB(0,255,0),false);
				_snprintf(szBuffer,CountArray(szBuffer),TEXT("%+I64d"),pGameEnd->lGameScore[i]);
				InsertGeneralString(szBuffer,RGB(255,255,0),true);
			}

			m_ScoreView.SetUserLeft(i,pGameEnd->IsUserLeft[i]);
		}

        
		//判断是否自摸、点炮、逃跑
		m_bZM=false;
		m_bFP=false;
		bool m_bTP=false;  
		if (GameScoreInfo.wProvideUser != INVALID_CHAIR)
		{
			if(CHK_NULL != GameScoreInfo.wCardType[GameScoreInfo.wProvideUser])
			{
				m_bZM=true;
			}
			else /* if(CHK_NULL == GameScoreInfo.wCardType[GameScoreInfo.wProvideUser])*/
			{
				m_bFP=true;
			}	
		}
		else /*if (GameScoreInfo.wProvideUser == INVALID_CHAIR)*/
		{
			m_bTP=true;
		}

		for (WORD i=0;i<GAME_PLAYER;i++)
		{
			BYTE byViewID = SwitchViewChairID(i);
			m_GameClientView.m_wChiHuKind[byViewID]=pGameEnd->wChiHuKind[i];
			m_GameClientView.m_wChiHuRight[byViewID]=pGameEnd->wChiHuRight[i];
		}

		m_ScoreView.SetScoreInfo(GameScoreInfo);
		//m_ScoreView.ShowWindow(SW_SHOW);
		m_GameClientView.m_btStart.ShowWindow(SW_HIDE);
		m_GameClientView.m_bIsHuangZhuang=false;
		m_GameClientView.SetHaiDiCard(pGameEnd->cbHaiDiCard);
	}
	else 
	{//海底荒庄
		DeductionTableCard(true);
		m_GameClientView.SetHaiDiCard(pGameEnd->cbHaiDiCard);
		m_GameClientView.m_bIsHuangZhuang=true;
	}



	//判断是否有人逃跑
	bool bIsLeft=false;
	for (int i=0;i<GAME_PLAYER;++i)
	{
		if (pGameEnd->IsUserLeft[i])
		{
			bIsLeft=true;
			break;
		}	
	}


	//用户结束扑克
	int iChiHuNum=0;
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		WORD wViewChairID=SwitchViewChairID(i);
		if (pGameEnd->wChiHuKind[i]!=CHK_NULL) 
		{
			m_GameClientView.SetUserAction(wViewChairID,WIK_CHI_HU);
			m_byUserAction[i] = WIK_CHI_HU;
			iChiHuNum++;
		}
		m_GameClientView.m_TableCard[wViewChairID].SetCardData(pGameEnd->cbCardData[i],pGameEnd->cbCardCount[i]);
	}

	//播放胡牌动画
	m_GameClientView.m_iChiHuNums=iChiHuNum;
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		WORD wViewChairID=SwitchViewChairID(i);
		if (!bIsLeft)
		{
			m_GameClientView.PlayChiHuFlash(wViewChairID);
			m_bBirdUserID=SwitchViewChairID(GameScoreInfo.wBirdHitUser);
			m_GameClientView.m_cbBirdCardData=pGameEnd->cbBirdCard;
		}

		
	}


	//设置扑克
	m_GameClientView.m_UserCard[0].SetCardData(0,false);
	m_GameClientView.m_UserCard[1].SetCardData(0,false);
	m_GameClientView.m_UserCard[2].SetCardData(0,false);
	m_GameClientView.m_HandCardControl.SetCardData(NULL,0,0);


	//播放声音
	const tagUserData *pUserData = GetUserData(GetMeChairID());//用户信息
	LONGLONG lScore=pGameEnd->lGameScore[GetMeChairID()];
	if (lScore>0L) 
	{
		if (m_bZM) //自摸 
		{
			PlayGameSoundLanguage(pUserData->cbGender,TEXT("zimo"),false);
		}
		else if(m_bFP) //点炮
		{
			PlayGameSoundLanguage(pUserData->cbGender,TEXT("chihu"),false);
		}
		//else
		//{
		//	PlaySound("GAME_WIN",false);
		//}
	}
	else if (lScore<0L) 
	{
		PlaySound("GAME_LOST",false);
		PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_LOST"));
	}
	else
	{
		PlaySound("GAME_END",false);
	}


	//设置界面
	if (IsLookonMode()==true)
	{
		m_GameClientView.m_btAutoPlayOn.ShowWindow(SW_HIDE);
		m_GameClientView.m_btAutoPlayOff.ShowWindow(SW_HIDE);
		m_GameClientView.m_btStart.ShowWindow(SW_HIDE);
        m_ScoreView.ShowWindow(true);
		if (pGameEnd->cbChiHuCard == 0)
		{
			m_ScoreView.ShowWindow(false);
		}
	}

	//设置界面
	if (IsLookonMode()==false)
	{
		if (pGameEnd->bIsHuangzhuang)
		{//荒庄
			m_ScoreView.ShowWindow(SW_HIDE);
			SetTimer(IDI_HUANGZHUANG,1500,NULL);
			SetGameTimer(GetMeChairID(),IDI_START_GAME,TIME_START_GAME);
			m_GameClientView.m_btStart.ShowWindow(SW_SHOW);
		}
		else
		{//延时3.5秒显示结算框
			SetTimer(IDI_GAME_END,TIME_GAME_END,NULL);
			m_GameClientView.m_btStart.ShowWindow(SW_HIDE);
		}
	}

	//取消托管
	if(m_bStustee)
	{
        OnStusteeControl(0,0);
	}

	//if (pGameEnd->bIsHuangzhuang)
	//{//荒庄
	//	m_ScoreView.ShowWindow(SW_HIDE);
	//	m_GameClientView.m_btStart.ShowWindow(SW_SHOW);
	//}
	//else
	//{//延时2秒显示结算框
	//    SetTimer(IDI_GAME_END,TIME_GAME_END,NULL);
	//	m_GameClientView.m_btStart.ShowWindow(SW_HIDE);
	//}
		

	//更新界面
	m_GameClientView.UpdateGameView(NULL);


	return true;
}

//播放声音
void CGameClientDlg::PlayCardSound(BYTE cbGender, BYTE cbCardData)
{
	//变量定义
	TCHAR szSoundName[16]=TEXT("");

	//构造名字
	BYTE cbValue=(cbCardData&MASK_VALUE);
	BYTE cbColor=(cbCardData&MASK_COLOR)>>4;
	CString szSound="";
	if (0==cbColor)
	{
		szSound=TEXT("wan");
	}
	else if (1==cbColor)
	{
		szSound=TEXT("tiao");
	}
	else if (2==cbColor)
	{
		szSound=TEXT("tong");
	}
	else
	{
		szSound=TEXT("feng");
	}

	_snprintf(szSoundName,CountArray(szSoundName),TEXT("%d"),cbValue);

	//播放声音
    PlaySound("OUT_CARD",false);
	m_cbGender=cbGender;
	m_InfoSound=szSound+szSoundName;
    SetTimer(IDI_SOUND,200,NULL);	

	return;
}

//播放声音
void CGameClientDlg::PlayActionSound(BYTE cbGender, BYTE cbAction)
{
	switch (cbAction)
	{
	case WIK_NULL:		//取消
		{
			PlaySound("OUT_CARD",false);
			break;
		}
	case WIK_LEFT:     //吃牌
	case WIK_CENTER:
	case WIK_RIGHT:	
		{
			PlayGameSoundLanguage(cbGender,TEXT("chi"),false);
			break;
		}
	case WIK_PENG:		//碰牌
		{
			PlayGameSoundLanguage(cbGender,TEXT("peng"),false);
			break;
		}
	case WIK_FILL:	//补牌
	case WIK_GANG:	
		{
			PlayGameSoundLanguage(cbGender,TEXT("buzhang"),false);
			break;
		}
	case WIK_TING:		//听牌状态（杠牌显示）
		{

			PlayGameSoundLanguage(cbGender,TEXT("gang"),false);
			break;
		}
	case WIK_CHI_HU:	//吃胡
		{
			break;
		}
	}

	return;
}

//扣除扑克
void CGameClientDlg::DeductionTableCard(bool bHeadCard)
{
	//按顺序删除
	if (bHeadCard==true)
	{
		//切换索引
		BYTE cbHeapCount=m_cbHeapCardInfo[m_wHeapHand][0]+m_cbHeapCardInfo[m_wHeapHand][1];
		if (cbHeapCount==HEAP_FULL_COUNT) 
			m_wHeapHand=(m_wHeapHand+1)%CountArray(m_cbHeapCardInfo);

		//减少扑克
		m_cbLeftCardCount--;
		m_cbHeapCardInfo[m_wHeapHand][0]++;

		//堆立扑克
		WORD wHeapViewID=SwitchViewChairID(m_wHeapHand);
		WORD wMinusHeadCount=m_cbHeapCardInfo[m_wHeapHand][0];
		WORD wMinusLastCount=m_cbHeapCardInfo[m_wHeapHand][1];
		m_GameClientView.m_HeapCard[wHeapViewID].SetCardData(wMinusHeadCount,wMinusLastCount,HEAP_FULL_COUNT);
	}
	//从最后一张删除
	else
	{
		//切换索引
		BYTE cbHeapCount=m_cbHeapCardInfo[m_wHeapTail][0]+m_cbHeapCardInfo[m_wHeapTail][1];
		if (cbHeapCount==HEAP_FULL_COUNT) m_wHeapTail=(m_wHeapTail+3)%CountArray(m_cbHeapCardInfo);

		//减少扑克
		m_cbLeftCardCount--;
		m_cbHeapCardInfo[m_wHeapTail][1]++;

		//堆立扑克
		WORD wHeapViewID=SwitchViewChairID(m_wHeapTail);
		WORD wMinusHeadCount=m_cbHeapCardInfo[m_wHeapTail][0];
		WORD wMinusLastCount=m_cbHeapCardInfo[m_wHeapTail][1];
		m_GameClientView.m_HeapCard[wHeapViewID].SetCardData(wMinusHeadCount,wMinusLastCount,HEAP_FULL_COUNT);
	}

	return;
}

//显示控制
bool CGameClientDlg::ShowOperateControl(BYTE cbUserAction,BYTE cbStartData)
{
	//if (cbUserAction==WIK_TING)
	//{
	//	return true;
	//}
	//显示界面
	tagGangCardResult bucardresult;
	::memset(&bucardresult, 0, sizeof(tagGangCardResult));
	::memcpy(bucardresult.cbCardData, m_GameStartData.byBuCard, sizeof(BYTE)*m_GameStartData.byBuCardNums);
	bucardresult.cbCardCount = m_GameStartData.byBuCardNums;
	m_GameClientView.m_ControlWnd.SetControlInfo(cbStartData,cbUserAction,bucardresult);
	m_GameClientView.m_ControlWnd.AddGangCardData(m_GameStartData.byGangCard, m_GameStartData.byGangCardNums);

	return true;
}


//时间消息
void CGameClientDlg::OnTimer(UINT nIDEvent)
{
	//骰子定时器
	if (nIDEvent==IDI_SHOW_SZ)
	{
		//环境处理
		KillTimer(IDI_SHOW_SZ);
		m_GameClientView.SetDiceEffect(true);
		PlaySound("GAME_START",false);
		return;
	}
	
	//结束定时器
	if (nIDEvent==IDI_SOUND)
	{
		KillTimer(IDI_SOUND);
		PlayGameSoundLanguage(m_cbGender,m_InfoSound,false);
		return;
	}
	
	//结束定时器
	if (nIDEvent==IDI_HUANGZHUANG)
	{
		KillTimer(IDI_HUANGZHUANG);
		m_GameClientView.m_btStart.ShowWindow(SW_SHOW);
		return;
	}

	//扎鸟定时器
	if (nIDEvent==IDI_GAME_END)
	{
		KillGameTimer(IDI_START_GAME);
		KillTimer(IDI_GAME_END);
		SetTimer(IDI_BIRD,2000,NULL);
		BYTE byViewID=m_bBirdUserID;
		m_GameClientView.SetBirdEffect(true,byViewID);
		return;
	}

	//结束定时器
	if (nIDEvent==IDI_BIRD)
	{
		KillTimer(IDI_BIRD);
		KillGameTimer(IDI_START_GAME);
		m_ScoreView.ShowWindow(SW_SHOW);
		m_ScoreView.SetCountDown(true);
		return;
	}


	__super::OnTimer(nIDEvent);
}



//开始按钮
LRESULT CGameClientDlg::OnStart(WPARAM wParam, LPARAM lParam)
{
	

	//环境设置
	KillGameTimer(IDI_START_GAME);
	m_GameClientView.m_btStart.ShowWindow(SW_HIDE);
	m_GameClientView.m_btHaiDi.ShowWindow(SW_HIDE);
	m_GameClientView.m_btNoHaiDi.ShowWindow(SW_HIDE);
	m_GameClientView.m_ControlWnd.ShowWindow(SW_HIDE);

	//设置界面
	m_ScoreView.ShowWindow(false);
	m_GameClientView.SetHaiDiCard(0);
	m_GameClientView.SetStatusFlag(false,false);
	m_GameClientView.SetBankerUser(INVALID_CHAIR);
	m_GameClientView.SetUserAction(INVALID_CHAIR,0);
	m_GameClientView.SetOutCardInfo(INVALID_CHAIR,0);

	//动作设置
	memset(m_byUserAction, 0 , sizeof(m_byUserAction));

	//扑克设置
	m_GameClientView.m_UserCard[0].SetCardData(0,false);
	m_GameClientView.m_UserCard[1].SetCardData(0,false);
	m_GameClientView.m_UserCard[2].SetCardData(0,false);
	m_GameClientView.m_HandCardControl.SetCardData(NULL,0,0);

	//扑克设置
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		m_GameClientView.m_TableCard[i].SetCardData(NULL,0);
		m_GameClientView.m_DiscardCard[i].SetCardData(NULL,0);
		m_GameClientView.m_WeaveCard[i][0].SetCardData(NULL,0);
		m_GameClientView.m_WeaveCard[i][1].SetCardData(NULL,0);
		m_GameClientView.m_WeaveCard[i][2].SetCardData(NULL,0);
		m_GameClientView.m_WeaveCard[i][3].SetCardData(NULL,0);
	}

	//堆立扑克
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		m_cbHeapCardInfo[i][0]=0;
		m_cbHeapCardInfo[i][1]=HEAP_FULL_COUNT-(((i==m_wBankerUser)||((i+2)%4==m_wBankerUser))?28:26);
		m_GameClientView.m_HeapCard[SwitchViewChairID(i)].SetCardData(m_cbHeapCardInfo[i][0],m_cbHeapCardInfo[i][1],HEAP_FULL_COUNT);
	}

	//状态变量
	m_bHearStatus=false;

	//游戏变量
	m_wCurrentUser=INVALID_CHAIR;

	//出牌信息
	m_cbOutCardData=0;
	m_wOutCardUser=INVALID_CHAIR;
	ZeroMemory(m_cbDiscardCard,sizeof(m_cbDiscardCard));
	ZeroMemory(m_cbDiscardCount,sizeof(m_cbDiscardCount));

	//组合扑克
	ZeroMemory(m_cbWeaveCount,sizeof(m_cbWeaveCount));
	ZeroMemory(m_WeaveItemArray,sizeof(m_WeaveItemArray));

	//堆立扑克
	m_wHeapHand=0;
	m_wHeapTail=0;
	ZeroMemory(m_cbHeapCardInfo,sizeof(m_cbHeapCardInfo));

	//扑克变量
	m_cbLeftCardCount=0;
	ZeroMemory(m_cbCardIndex,sizeof(m_cbCardIndex));

	//发送消息
	SendUserReady(NULL,0);

	m_GameClientView.UpdateGameView(NULL);
	return 0;
}

//骰子动画播完后
LRESULT CGameClientDlg::OnShowSZ(WPARAM wParam, LPARAM lParam)
{
	bool bPlayerMode=IsLookonMode();
	m_GameClientView.m_btStart.ShowWindow(SW_HIDE);
	m_ScoreView.ShowWindow(SW_HIDE);
	//旁观界面
	if (bPlayerMode)
	{
		m_GameClientView.m_btAutoPlayOn.ShowWindow(SW_HIDE);
		m_GameClientView.m_btAutoPlayOff.ShowWindow(SW_HIDE);
		m_ScoreView.ShowWindow(SW_HIDE);
		m_GameClientView.SetHaiDiCard(0);
		m_GameClientView.SetStatusFlag(false,false);
		m_GameClientView.SetUserAction(INVALID_CHAIR,0);
		m_GameClientView.SetOutCardInfo(INVALID_CHAIR,0);
	}

	memset(m_byUserAction, 0 , sizeof(m_byUserAction));

	//堆立扑克
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		m_cbHeapCardInfo[i][0]=0;
		m_cbHeapCardInfo[i][1]=HEAP_FULL_COUNT-(((i==m_wBankerUser)||((i+2)%GAME_PLAYER==m_wBankerUser))?28:26);
	}

	//分发扑克
	BYTE cbTakeCount=(MAX_COUNT-1)*GAME_PLAYER+1;  //刚开始发下去53张
	BYTE cbSiceFirst=HIBYTE(m_GameStartData.wSiceCount);
	BYTE cbSiceSecond=LOBYTE(m_GameStartData.wSiceCount);
	WORD wTakeChairID=(m_wBankerUser+7-cbSiceFirst)%GAME_PLAYER;
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		//计算数目
		BYTE cbValidCount=HEAP_FULL_COUNT-m_cbHeapCardInfo[wTakeChairID][1]-((i==0)?(cbSiceSecond-1)*2:0);
		BYTE cbRemoveCount=__min(cbValidCount,cbTakeCount);

		//提取扑克
		cbTakeCount-=cbRemoveCount;
		m_cbHeapCardInfo[wTakeChairID][(i==0)?1:0]+=cbRemoveCount;

		//完成判断
		if (cbTakeCount==0)
		{
			m_wHeapHand=wTakeChairID;
			m_wHeapTail=(m_wBankerUser+7-cbSiceFirst)%GAME_PLAYER;
			break;
		}

		//切换索引
		wTakeChairID=(wTakeChairID+1)%GAME_PLAYER;
	}

	//扑克设置
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		//变量定义
		WORD wViewChairID=SwitchViewChairID(i);

		//组合界面
		m_GameClientView.m_WeaveCard[i][0].SetDisplayItem(true);
		m_GameClientView.m_WeaveCard[i][1].SetDisplayItem(true);
		m_GameClientView.m_WeaveCard[i][2].SetDisplayItem(true);
		m_GameClientView.m_WeaveCard[i][3].SetDisplayItem(true);
		m_GameClientView.m_HeapCard[wViewChairID].SetCardData(m_cbHeapCardInfo[i][0],m_cbHeapCardInfo[i][1],HEAP_FULL_COUNT);

		//用户扑克
		if (wViewChairID!=2)
		{
			WORD wIndex=(wViewChairID>=3)?2:wViewChairID;
			m_GameClientView.m_UserCard[wIndex].SetCardData(MAX_COUNT-1,(i==m_wBankerUser));
		}
		else
		{
			BYTE cbBankerCard=(i==m_wBankerUser)?m_GameStartData.cbCardData[MAX_COUNT-1]:0;
			m_GameClientView.m_HandCardControl.SetCardData(m_GameStartData.cbCardData,MAX_COUNT-1,cbBankerCard);
		}

		//旁观界面
		if (bPlayerMode==false)
		{
			m_GameClientView.m_TableCard[wViewChairID].SetCardData(NULL,0);
			m_GameClientView.m_DiscardCard[wViewChairID].SetCardData(NULL,0);
			m_GameClientView.m_WeaveCard[wViewChairID][0].SetCardData(NULL,0);
			m_GameClientView.m_WeaveCard[wViewChairID][1].SetCardData(NULL,0);
			m_GameClientView.m_WeaveCard[wViewChairID][2].SetCardData(NULL,0);
			m_GameClientView.m_WeaveCard[wViewChairID][3].SetCardData(NULL,0);
		}
	}

	//WORD wMeChairID=GetMeChairID();
	////动作处理
	//if ((bPlayerMode==true)&&(m_GameStartData.cbUserAction!=WIK_NULL))
	//{
	//	ShowOperateControl(m_GameStartData.cbUserAction,m_GameStartData.cbCardData[MAX_COUNT-1]);
	//	SetGameTimer(wMeChairID,IDI_OPERATE_CARD,TIME_OPERATE_CARD);
	//}

	////出牌提示
	//if ((bPlayerMode==true)&&(m_wCurrentUser==wMeChairID/*!=INVALID_CHAIR*/))
	//{

	//	if (m_wCurrentUser==wMeChairID) 
	//	{
	//		m_GameClientView.SetStatusFlag(true,false);
	//		SetGameTimer(m_wCurrentUser,IDI_OPERATE_CARD,TIME_HEAR_STATUS);
	//	}
	//}

	m_bIsMeCanOutCard = false;
	m_wCurrentUser = INVALID_CHAIR;
	m_byUserAction[GetMeChairID()] = WIK_NULL;
	WORD wMeChairID=GetMeChairID();
	if (!bPlayerMode && m_wBankerUser==wMeChairID)
	{
		m_wCurrentUser =  wMeChairID; 
		m_GameClientView.SetStatusFlag(true,false);
		if (m_GameStartData.cbUserAction!=WIK_NULL)
		{
			m_byUserAction[wMeChairID] = m_GameStartData.cbUserAction;
			ShowOperateControl(m_GameStartData.cbUserAction,m_GameStartData.cbCardData[MAX_COUNT-1]);
			m_bIsMeCanOutCard = true;
			SetGameTimer(GetMeChairID(),IDI_OPERATE_CARD,TIME_OPERATE_CARD);
		}
		else
		{
			SetGameTimer(m_wCurrentUser,IDI_OPERATE_CARD,TIME_HEAR_STATUS);
		}
	}
	else
	{
		SetGameTimer(m_wBankerUser,IDI_OPERATE_CARD,TIME_HEAR_STATUS);
	}


	//托管设置
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		m_GameClientView.SetTrustee(SwitchViewChairID(i),m_GameStartData.bTrustee[i]);
	}

	//激活框架
	if (bPlayerMode==false) ActiveGameFrame();

	//更新界面
	m_GameClientView.UpdateGameView(NULL);

	return 0;
}
//出牌操作
LRESULT CGameClientDlg::OnOutCard(WPARAM wParam, LPARAM lParam)
{
	if (GS_MJ_PLAY != GetGameStatus())
	{
		return 0;
	}
	if (m_GameClientView.m_ControlWnd.IsWindowVisible())
	{
		return 0;
	}

	//出牌判断
	if ((IsLookonMode()==true)||(m_wCurrentUser!=GetMeChairID())) 
	{
		return 0;
	}

	//听牌判断
	//if ((m_bHearStatus==true)&&(m_GameClientView.m_HandCardControl.GetCurrentCard()!=(BYTE)wParam)) return 0;

	//设置变量
	m_wCurrentUser=INVALID_CHAIR;
	BYTE cbOutCardData=(BYTE)wParam;
	m_GameLogic.RemoveCard(m_cbCardIndex,cbOutCardData);

	//设置扑克
	BYTE cbCardData[MAX_COUNT];
	BYTE cbCardCount=m_GameLogic.SwitchToCardData(m_cbCardIndex,cbCardData);
	m_GameClientView.m_HandCardControl.SetCardData(cbCardData,cbCardCount,0);

	//设置界面
	KillGameTimer(IDI_OPERATE_CARD);
	KillGameTimer(IDI_OTHER_USEROUTCARD);

	m_GameClientView.UpdateGameView(NULL);
	m_GameClientView.SetStatusFlag(false,false);
	m_GameClientView.SetUserAction(INVALID_CHAIR,0);
	m_GameClientView.SetOutCardInfo(2,cbOutCardData);
	m_GameClientView.m_ControlWnd.ShowWindow(SW_HIDE);

	//动作设置
	memset(m_byUserAction, 0 , sizeof(m_byUserAction));

	//播放声音
	//PlayCardSound(m_wOutCardUser->cbGender, cbOutCardData);

	//发送数据
	CMD_C_OutCard OutCard;
	OutCard.cbCardData=cbOutCardData;
	SendData(SUB_C_OUT_CARD,&OutCard,sizeof(OutCard));
	return 0;
}

//扑克操作
LRESULT CGameClientDlg::OnCardOperate(WPARAM wParam, LPARAM lParam)
{
	//变量定义
	BYTE cbOperateCode=(BYTE)(wParam);
	BYTE cbOperateCard=(BYTE)(LOWORD(lParam));


	////状态判断  若打开 取消会返回 0
	//if ((m_wCurrentUser==GetMeChairID())&&(cbOperateCode==WIK_NULL))
	//{
	//	m_GameClientView.m_ControlWnd.ShowWindow(SW_HIDE);
	//	return 0;
	//};


	//考虑起手有多家胡牌
	//if (m_wCurrentUser!=GetMeChairID())
	//{
	//	return 0;
	//}

	//环境设置
	KillGameTimer(IDI_OPERATE_CARD);
	KillGameTimer(IDI_OTHER_USEROUTCARD);

	m_GameClientView.SetStatusFlag(false,true);
	m_GameClientView.m_ControlWnd.ShowWindow(SW_HIDE);

	//发送命令
	CMD_C_OperateCard OperateCard;
	OperateCard.cbOperateCode=cbOperateCode;
	OperateCard.cbOperateCard=cbOperateCard;
	if (cbOperateCode==9 && 0!=cbOperateCard)
	{
		OperateCard.cbOperateCode=WIK_GANG;
		OperateCard.cbOperateCard=cbOperateCard;
	}
	else if (cbOperateCode==10 && 0!=cbOperateCard)
	{
		OperateCard.cbOperateCode=WIK_TING;
		OperateCard.cbOperateCard=cbOperateCard;
	}
	SendData(SUB_C_OPERATE_CARD,&OperateCard,sizeof(CMD_C_OperateCard));

	memset(m_byUserAction, 0, sizeof(m_byUserAction));

	m_GameClientView.SetStatusFlag(false,false);
	if (0==cbOperateCode)
	{
		if (m_bIsMeCanOutCard)
		{//自己还可以出牌.
			m_byUserAction[GetMeChairID()] = WIK_NULL;
			//出牌
			m_wCurrentUser = GetMeChairID();
			memset(m_byUserAction, 0, sizeof(m_byUserAction));
			SetGameTimer(GetMeChairID(),IDI_OTHER_USEROUTCARD,TIME_HEAR_STATUS-10);
			//出牌提示
			m_GameClientView.SetStatusFlag(true,false);
		}
	}
	m_GameClientView.UpdateGameView(NULL);
	return 0;
}

//海底操作
LRESULT CGameClientDlg::OnOperateHaiDi(WPARAM wParam, LPARAM lParam)
{
	//界面设置
	KillGameTimer(TIME_OPERATE_HAI_DI);
	m_GameClientView.m_btHaiDi.ShowWindow(SW_HIDE);
	m_GameClientView.m_btNoHaiDi.ShowWindow(SW_HIDE);
	//m_GameClientView.m_btStart.ShowWindow(SW_SHOW);


	//构造消息
	CMD_C_OperateHaiDi OperateHaiDi;
	OperateHaiDi.cbTakeHaiDi=(BYTE)wParam;
	SendData(SUB_C_OPERATE_HAI_DI,&OperateHaiDi,sizeof(OperateHaiDi));

	return 0;
}

//用户托管
bool CGameClientDlg::OnSubTrustee(const void * pBuffer,WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_Trustee));
	if (wDataSize!=sizeof(CMD_S_Trustee)) return false;

	//消息处理
	CMD_S_Trustee * pTrustee=(CMD_S_Trustee *)pBuffer;
	m_GameClientView.SetTrustee(SwitchViewChairID(pTrustee->wChairID),pTrustee->bTrustee);
	if ((IsLookonMode()==true)||(pTrustee->wChairID!=GetMeChairID()))
	{
		const tagUserData * pUserData=GetUserData(pTrustee->wChairID);
		TCHAR szBuffer[256];
		if(pTrustee->bTrustee==true)
			_snprintf(szBuffer,sizeof(szBuffer),TEXT("玩家选择了托管功能."),pUserData->szName);
		else
			_snprintf(szBuffer,sizeof(szBuffer),TEXT("玩家取消了托管功能."),pUserData->szName);
		InsertSystemString(szBuffer);
	}

	return true;
}

//拖管控制
LRESULT CGameClientDlg::OnStusteeControl(WPARAM wParam, LPARAM lParam)
{
	//设置变量
	m_wTimeOutCount=0;

	//设置状态
	if (m_bStustee==true)
	{
		m_bStustee=false;
		m_GameClientView.m_btAutoPlayOn.ShowWindow(true);
		m_GameClientView.m_btAutoPlayOff.ShowWindow(false);
		InsertSystemString(_T("您取消了托管功能."));
		CMD_C_Trustee Trustee;
		Trustee.bTrustee = false;
		SendData(SUB_C_TRUSTEE,&Trustee,sizeof(Trustee));

	}
	else
	{
		m_bStustee=true;
		m_GameClientView.m_btAutoPlayOff.ShowWindow(true);
		m_GameClientView.m_btAutoPlayOn.ShowWindow(false);
		InsertSystemString(_T("您选择了托管功能."));
		CMD_C_Trustee Trustee;
		Trustee.bTrustee = true;
		SendData(SUB_C_TRUSTEE,&Trustee,sizeof(Trustee));
	}
	//m_GameClientView.m_btStusteeControl.UpdateWindow();

	return 0;
}


//点击继续游戏
void CGameClientDlg::OnEventContinueGame()
{
	OnStart(0, 0);
}


//播放游戏声音
void CGameClientDlg::PlayGameSoundLanguage(BYTE cbGender, CString strSound, bool bSoundType)
{
	CString strGameSound="";
	//性别（播放男女声音）
	if (1==cbGender)
	{//女
		strGameSound=TEXT(m_sDialect+"\\W\\"+strSound);
	}
	else
	{//男
		strGameSound=TEXT(m_sDialect+"\\M\\"+strSound);
	}

	if (bSoundType)
	{//多种声音
		TCHAR cBuffer[10]={""};
		int iRand=rand()%3+1;
		::_snprintf(cBuffer,sizeof(cBuffer),"%d",iRand);
		strGameSound=strGameSound+TEXT("_")+TEXT(cBuffer);
	}
	PlaySound(strGameSound,false);
	//PlayGameSound(AfxGetInstanceHandle(),strGameSound);
}

//播放游戏随机声音
void CGameClientDlg::PlayGameSoundRand(BYTE cbGender, CString strSound, bool bSoundType)
{
	CString strGameSound="";
	//性别（播放男女声音）
	if (1==cbGender)
	{//女
		strGameSound=m_sDialect+TEXT("\\W\\")+strSound;
	}
	else
	{//男
		strGameSound=m_sDialect+TEXT("\\M\\")+strSound;
	}

	if (bSoundType)
	{//多种声音
		TCHAR cBuffer[10]={""};
		int iRand=rand()%4+1;
		::_snprintf(cBuffer,sizeof(cBuffer),"%d",iRand);
		strGameSound=strGameSound+TEXT("_")+TEXT(cBuffer);
	}
	//PlayGameSound(AfxGetInstanceHandle(),strGameSound);
	PlaySound(strGameSound,false);
}

void CGameClientDlg::PlaySound(CString szSoundRes, bool bLoop)
{
	if (!szSoundRes.IsEmpty())
	{
		TCHAR szResourcePath[MAX_PATH];
		GetCurrentDirectory(MAX_PATH,szResourcePath);
		CString strPath(szResourcePath);
		strPath.Append("\\SparrowCS\\Sound\\");
		strPath.Append(szSoundRes);
		strPath.Append(".mp3");

		PlayDirSound(strPath, true, bLoop); //播放游戏音效
	}
}

//播放背景音乐
void CGameClientDlg::PlayMusic(CString szSoundRes)
{
	if (!szSoundRes.IsEmpty())
	{
		TCHAR szResourcePath[MAX_PATH];
		GetCurrentDirectory(MAX_PATH,szResourcePath);
		CString strPath(szResourcePath);
		strPath.Append("\\SparrowCS\\Sound\\");
		strPath.Append(szSoundRes);

		PlayBGMusic(strPath, 0, true);
	}
}


void CGameClientDlg::CheckCanHuPai(BYTE byCardData, int iX)
{
	BYTE byChaiID = GetMeChairID();
	if (NULL==m_byUserAction[byChaiID] && m_wCurrentUser==byChaiID)
	{
		if (0 != byCardData)
		{
			//InsertSystemString("CheckCanHuPai...");

			//智能提示
			BYTE cbHuCardData[MAX_INDEX]={0};
			BYTE byTempCardIndex[MAX_INDEX]={0};
			::memcpy(byTempCardIndex, m_cbCardIndex, MAX_INDEX);
			m_GameLogic.RemoveCard(byTempCardIndex,byCardData);
			//m_cbTempKing[0]=0x01;
			//m_GameLogic.IsCanTingCard(byTempCardIndex,0,0,m_cbTempKing);
			m_GameLogic.IsCanTingCard(byTempCardIndex,m_WeaveItemArray[m_wCurrentUser],m_cbWeaveCount[m_wCurrentUser]);
			int iCardNums = m_GameLogic.GetHuPaiData(cbHuCardData,MAX_INDEX);

			if (iCardNums>0)
			{
				m_GameClientView.SetCanHuPaiData(cbHuCardData, iCardNums, iX);
				//char cbData[128]={0};
				//sprintf(cbData,"%x,%x,%x,%x,%x,%d",cbHuCardData[0],cbHuCardData[1],cbHuCardData[2],cbHuCardData[3],cbHuCardData[4],iCardNums);
				//InsertSystemString(cbData);
			}
			else
			{
				m_GameClientView.SetCanHuPaiData(NULL, 0, 0);
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////

