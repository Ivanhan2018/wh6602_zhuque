#include "Stdafx.h"
#include "GameClient.h"
#include "GameOption.h"
#include "GameClientDlg.h"
#include "GameLogic.h"
#include "CrashRptFaceExp.h"
//////////////////////////////////////////////////////////////////////////

//游戏定时器
#define IDI_START_GAME				200									//开始定时器
#define IDI_OPERATE_CARD			202									//操作定时器
#define IDI_OPERATE_HAI_DI			203									//海底定时器
#define IDI_USER_TING_CARD			204									//听牌定时器
#define IDI_OTHER_USEROUTCARD		205									//其实出牌定时器
#define IDI_GAME_END                206                                 //结束定时器
#define IDI_SHOW_SZ                 207                                 //骰子定时器
#define IDI_HUANGZHUANG             208                                 //荒庄定时器
#define IDI_SOUND                   209                                 //声音定时器
#define IDI_SZSTART                 210                                 //启动筛子定时器
#define IDI_SHOW_DIRECT             211                                 //方为动画
#define	IDI_SELECT_OUT_CARD		212								//选择出牌


//游戏定时器
#define TIME_START_GAME				15									//开始定时器
#define TIME_HEAR_STATUS			15									//出牌定时器
#define TIME_OPERATE_CARD			10									//操作定时器
#define TIME_OPERATE_HAI_DI			10									//操作定时器
#define TIME_USERTINGCARD			5									//操作定时器
#define TIME_GAME_END				3500								//结束框显示时间
#define TIME_SHOW_SZ				100								    //骰子时间
#define TIME_SHOW_DIRECT            100                                 //方为动画时间 
#define TIME_SELECT_OUT_CARD	  5										//选牌时间


//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CGameClientDlg, CGameFrameDlg)
	ON_MESSAGE(IDM_START,OnStart)
	ON_MESSAGE(IDM_OUT_CARD,OnOutCard)
	ON_MESSAGE(IDM_TING_CARD,OnTingCard)
	ON_MESSAGE(IDM_CARD_OPERATE,OnCardOperate)
	//ON_MESSAGE(IDM_OPERATE_HAI_DI,OnOperateHaiDi)
	ON_MESSAGE(IDM_TRUSTEE_CONTROL,OnStusteeControl)
	ON_MESSAGE(IDM_SHOW_SZ,OnShowSZ)
	ON_MESSAGE(IDM_MOPAI,OnMoPai)
	ON_MESSAGE(IDM_SZSTART,OnSZStart)
	ON_MESSAGE(IDM_SZ_BUTTON, OnSZFlashBT)
	ON_WM_TIMER()
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CGameClientDlg::CGameClientDlg() : CGameFrameDlg(&m_GameClientView, &m_ScoreView)
{
	//游戏变量
	m_wBankerUser=INVALID_CHAIR;
	m_wCurrentUser=INVALID_CHAIR;

	//状态变量
	//m_bHearStatus=false;
	m_bIsMeCanOutCard = false;

	//托管变量
	m_bStustee=false;
	m_wTimeOutCount =0;

	//声音变量
	m_cbGender=0;
	m_InfoSound="";

	//堆立变量
	m_wHeapHead=0;
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

	m_byCanSelectType = WIK_NULL;
	m_byCanSelectCount = 0;
	ZeroMemory(m_byCanSelectCard,sizeof(m_byCanSelectCard));

	//方言变量
	m_sDialect="GY"; 

	return;
}

//析构函数
CGameClientDlg::~CGameClientDlg()
{
	//SafeDelete(m_FlashControl);
}

//初始函数
bool CGameClientDlg::InitGameFrame()
{
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//tagWeaveItem WeaveItem[4];
	//memset(WeaveItem, 0, sizeof(tagWeaveItem)*4);
	////WeaveItem[0].cbCenterCard = 0x21;
	////WeaveItem[0].cbPublicCard = FALSE;
	////WeaveItem[0].wProvideUser = -1;
	////WeaveItem[0].cbWeaveKind = WIK_GANG;

	////WeaveItem[1].cbCenterCard = 0x03;
	////WeaveItem[1].cbPublicCard = FALSE;
	////WeaveItem[1].wProvideUser = -1;
	////WeaveItem[1].cbWeaveKind = WIK_LEFT;

	////WeaveItem[2].cbCenterCard = 0x07;
	////WeaveItem[2].cbPublicCard = FALSE;
	////WeaveItem[2].wProvideUser = -1;
	////WeaveItem[2].cbWeaveKind = WIK_GANG;

	//tagChiHuResult result;
	//memset(&result, 0, sizeof(tagChiHuResult));
	//BYTE byCardData[13]={0x02,0x02,0x04,0x04,0x04,0x08,0x08,0x17,0x17,0x17,0x18,0x18,0x18};
	//BYTE m_cbCardIndex[MAX_INDEX]={0};
	//m_GameLogic.SwitchToCardIndex(byCardData,13,m_cbCardIndex);
	//BYTE byHuType = m_GameLogic.AnalyseChiHuCard(m_cbCardIndex,WeaveItem,0,0x02,0,result);
	//BYTE bySaveHuType = byHuType;
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//设置标题
	//SetWindowText(TEXT("转转麻将  --  Ver：1.0.5.3"));

	//方言格式
	//m_sDialect.Format("%s_",m_strLanguage.c_str()); 

	//设置图标
	HICON hIcon=LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME));
	SetIcon(hIcon,TRUE);
	SetIcon(hIcon,FALSE);

	//加载资源
	//g_CardResource.LoadResource();
	m_GameClientView.m_btStart.ShowWindow(SW_HIDE);
	//m_GameClientView.m_btHaiDi.ShowWindow(SW_HIDE);
	//m_GameClientView.m_btNoHaiDi.ShowWindow(SW_HIDE);
	//m_GameClientView.m_btTingCard.ShowWindow(SW_HIDE);
	//m_GameClientView.m_btBuTing.ShowWindow(SW_HIDE);

	m_GameClientView.m_btAutoPlayOn.ShowWindow(SW_HIDE);
	m_GameClientView.m_btAutoPlayOff.ShowWindow(SW_HIDE);
	//m_ScoreView.ShowWindow(SW_SHOW);

	m_GameClientView.SetGameClientDlgPtr(this);
	//m_GameClientView.m_FlashControl.ShowWindow(SW_HIDE);
	//m_GameClientView.m_ControlWnd.ShowWindow(SW_SHOW);


	//m_ScoreView.ShowWindow(SW_SHOW);
	//m_ScoreView.SetCountDown(true);

	////显示界面
	//BYTE byMask = WIK_NULL;
	////byMask = WIK_CHI_HU;
	////byMask |= WIK_PENG;
	//byMask |= WIK_LEFT;


	//tagGangCardResult bucardresult;
	//BYTE cbCardData[4]={0x11, 0x11, 0x11,0x11 };
	//::memset(&bucardresult, 0, sizeof(tagGangCardResult));
	//memcpy(bucardresult.cbCardData,cbCardData,sizeof(cbCardData));
	//bucardresult.cbCardCount =0;
	//m_GameClientView.m_ControlWnd.SetControlInfo(0x12,byMask,bucardresult);
	//m_GameClientView.m_ControlWnd.AddGangCardData(NULL, 0);
	//m_GameClientView.m_ControlWnd.ShowWindow(SW_SHOW);


	//HWND hwnd=AfxGetMainWnd()->m_hWnd;
	//DWORD proID;//保存进程ID
	//GetWindowThreadProcessId(hwnd,&proID);//获取进程ID
	//char chData[128]={0};
	//sprintf(chData, "%d", proID);
	//InsertSystemString(chData);



	CString strPicPath="";
	TCHAR szResourcePath[MAX_PATH]={0};
	GetCurrentDirectory(MAX_PATH,szResourcePath);
	strPicPath.Append(szResourcePath);
	strPicPath.Append(TEXT("\\SparrowZZ\\Picture\\PNG\\Cartoon\\"));
	//m_FlashControl.Create(IDD_DLG_GAME_FLASH, this);
	//m_FlashControl.InitFlash(strPicPath + "ActChi.png", 12, CPoint(250, 200));


	m_GameClientView.InitViewFrame();

	//SetTaskWndShow(true);
	//播放游戏背景音乐
	PlayMusic("SparrowZZ.mp3");	

	SetChatShowTag(true,false);

	m_GameClientView.PlayDirectFlash(false);
	m_GameClientView.PlaySZFlashBT(false);
	return true;
}

//重置框架
void CGameClientDlg::ResetGameFrame()
{
	//游戏变量
	m_wBankerUser=INVALID_CHAIR;
	m_wCurrentUser=INVALID_CHAIR;

	//状态变量
	//m_bHearStatus=false;
	m_bIsMeCanOutCard = false;

	//托管变量
	m_bStustee=false;
	m_wTimeOutCount =0;

	//声音变量
	m_cbGender=0;
	m_InfoSound="";

	//堆立变量
	m_wHeapHead=0;
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

	m_byCanSelectType = WIK_NULL;
	m_byCanSelectCount = 0;
	ZeroMemory(m_byCanSelectCard,sizeof(m_byCanSelectCard));
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
	case IDI_SZSTART:     //启动筛子定时器
		{
			if (nElapse==0)
			{
				WORD wMeChairID=GetMeChairID();
				if (wChairID == m_wBankerUser && wMeChairID==m_wBankerUser)
				{
					AfxGetMainWnd()->SendMessage(IDM_SZ_BUTTON, 0, 0);
				}
				return false;
			}

			return true;
		}
	case IDI_START_GAME:		//开始游戏
		{
			if (nElapse==0)
			{
				PostMessage(WM_CLOSE);
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
				if (wChairID==wMeChairID && wMeChairID==m_wCurrentUser)
				{
					if((m_bStustee==false)/*&&(m_bHearStatus==false)*/)
					{
						m_wTimeOutCount++;
						if(m_wTimeOutCount==3)
						{
							InsertSystemString(TEXT("由于您多次超时，切换为“系统托管”模式."));
							OnStusteeControl(0,0);
						}
					}

					if( TRUE==m_GameClientView.m_ControlWnd.m_bthu.IsWindowVisible() 
						&& TRUE==m_GameClientView.m_ControlWnd.m_bthu.IsWindowEnabled())
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
					if((m_bStustee==false)/*&&(m_bHearStatus==false)*/)
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
		//case IDI_OPERATE_HAI_DI:	//海底定时器
		//	{
		//		//超时判断
		//		if (nElapse==0 || m_bStustee==true)
		//		{
		//			if (wChairID==m_wCurrentUser && wChairID==GetMeChairID())
		//			{
		//				OnOperateHaiDi(0,0);
		//			}
		//			return false;
		//		}

		//		//播放声音
		//		if ((nElapse<=3)&&(wChairID==GetMeChairID())) 
		//		{
		//			PlaySound("GAME_WARN",false);
		//		}

		//		return true;
		//	}
	case IDI_USER_TING_CARD:		//听牌定时器
		{
			if (0==nElapse || m_bStustee==true)
			{
				if (GetMeChairID()==m_wCurrentUser)
				{
					//发送不听消息
					CMD_UserTingOperate TingOperate;
					TingOperate.bTingCard = false;
					SendData(SUB_C_USERTINGCARD,&TingOperate,sizeof(TingOperate));
					//m_GameClientView.m_btTingCard.ShowWindow(SW_HIDE);
					//m_GameClientView.m_btBuTing.ShowWindow(SW_HIDE);
				}
			}
			//播放声音
			if ((nElapse<=3)&&(wChairID==GetMeChairID())) 
			{
				PlaySound("GAME_WARN",false);
			}
			return true;
		}
	case IDI_SELECT_OUT_CARD:	// 选牌定时器
		{
			if (0==nElapse || m_bStustee==true)
			{
				SendCardCommand(m_byCanSelectType, m_byCanSelectCard[0]);
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
	case SUB_C_SZSTART:			//启动色子动画
		{
			return OnSubStartSZFlash();
		}
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
		//case SUB_S_OPERATE_HAI_DI:	//海底操作
		//	{
		//		return OnSubOperateHaiDi(pBuffer,wDataSize);
		//	}
	case SUB_S_GAME_END:		//游戏结束
		{
			return OnSubGameEnd(pBuffer,wDataSize);
		}
	case SUB_S_TRUSTEE:			//用户托管
		{
			return OnSubTrustee(pBuffer,wDataSize);
		}
		//case SUB_S_TING_CARD:      //听牌标记
		//	{
		//		return OnSubTingCard(pBuffer,wDataSize);			
		//	}
		//case SUB_S_USER_TINGCARD_RESULT:	//听牌的结果
		//	{
		//		return OnSubTingCardResult(pBuffer, wDataSize);
		//	}
	}

	return true;
}

//游戏场景
bool CGameClientDlg::OnGameSceneMessage(BYTE cbGameStation, bool bLookonOther, const void * pBuffer, WORD wDataSize)
{
	CString str;
	str.Format("Test:CGameClientDlg::OnGameSceneMessage -%d", cbGameStation);
	OutputDebugString(str);

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
				//m_ScoreView.ShowWindow(SW_HIDE);
				SafeShowOrHideWnd(&m_ScoreView, SW_HIDE);
				m_GameClientView.m_btStart.ShowWindow(SW_SHOW);
				m_GameClientView.m_btStart.EnableWindow(TRUE);
				m_GameClientView.m_btStart.SetFocus();
				m_GameClientView.m_btAutoPlayOn.ShowWindow(SW_HIDE);
				m_GameClientView.m_btAutoPlayOff.ShowWindow(SW_HIDE);
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
	case GS_MJ_PLAY:	//游戏状态(断线重回)
		//case GS_MJ_HAI_DI:	//海底状态
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
			//m_bHearStatus=(pStatusPlay->cbHearStatus>0)?true:false;
			memset(m_byUserAction, WIK_NULL, sizeof(m_byUserAction));
			//m_cbTempKing[0] = pStatusPlay->byKindCard;

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
			m_GameClientView.SetOutCardInfo(INVALID_CHAIR,0);


			////设置控件
			//if (IsLookonMode()==false)
			//{
			//	m_GameClientView.m_btAutoPlayOn.ShowWindow(SW_SHOW);
			//	m_GameClientView.m_btAutoPlayOff.ShowWindow(SW_HIDE);
			//}
			//
			//托管,听牌设置
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

			////用户扑克
			//if (m_wCurrentUser==byMeChairID && cbGameStation!=GS_MJ_HAI_DI)
			//{
			//	BYTE cbCardCount=pStatusPlay->cbCardCount;
			//	tagCardItem CardItem;
			//	CardItem.cbCardData = pStatusPlay->cbCardData[0];
			//	CardItem.bShoot = false;
			//	CardItem.bKingFlag = false;//(pStatusPlay->cbCardData[0] == m_cbTempKing[0])?true:false;
			//	m_GameClientView.m_HandCardControl.SetCurrentCard(CardItem);
			//	m_GameClientView.m_HandCardControl.SetCardData(pStatusPlay->cbCardData,cbCardCount-1,CardItem.cbCardData/*,pStatusPlay->byKindCard*/);
			//}
			//else
			{
				m_GameClientView.m_HandCardControl.SetCardData(pStatusPlay->cbCardData,pStatusPlay->cbCardCount,0/*,pStatusPlay->byKindCard*/); 
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
				m_GameClientView.m_DiscardCard[wViewChairID[i]].SetCardData(m_cbDiscardCard[i],m_cbDiscardCount[i]);
			}

			m_GameClientView.m_HandCardControl.SetPositively(bIsMeIsLookonMode?false:true);
			m_GameClientView.m_HandCardControl.SetDisplayItem(bIsMeIsLookonMode?false:true);

			//堆立扑克
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				m_cbHeapCardInfo[i][0]=0;
				m_cbHeapCardInfo[i][1]=HEAP_FULL_COUNT-(((i==m_wBankerUser)||((i+2)%4==m_wBankerUser))?28:26);
			}

			//分发扑克
			BYTE cbTakeCount=108-m_cbLeftCardCount;  //53张
			BYTE cbSiceFirst=HIBYTE(pStatusPlay->wSiceCount);
			BYTE cbSiceSecond=LOBYTE(pStatusPlay->wSiceCount);
			BYTE cbSiceCount=__min(cbSiceFirst,cbSiceSecond);
			WORD wTakeChairID=(m_wBankerUser-cbSiceFirst-cbSiceSecond+1+GAME_PLAYER*GAME_PLAYER)%GAME_PLAYER;
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				//计算数目
				BYTE cbValidCount=HEAP_FULL_COUNT-m_cbHeapCardInfo[wTakeChairID][1]-((i==0)?cbSiceCount*2:0);
				BYTE cbRemoveCount=__min(cbValidCount,cbTakeCount);

				//提取扑克
				cbTakeCount-=cbRemoveCount;
				m_cbHeapCardInfo[wTakeChairID][(i==0)?1:0]+=cbRemoveCount;

				//if ((cbGameStation==GS_MJ_HAI_DI) && (0==i) && (cbRemoveCount>0 && cbRemoveCount<HEAP_FULL_COUNT))
				//{
				//	m_cbHeapCardInfo[wTakeChairID][0]=HEAP_FULL_COUNT-cbRemoveCount-1;
				//	m_cbHeapCardInfo[wTakeChairID][1]=cbRemoveCount;
				//}

				//完成判断
				if (cbTakeCount==0)
				{
					m_wHeapHead=wTakeChairID;
					m_wHeapTail=(m_wBankerUser-cbSiceFirst-cbSiceSecond+1+GAME_PLAYER*GAME_PLAYER/*7-cbSiceFirst*/)%GAME_PLAYER;
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

			m_GameClientView.m_iCountDirectIndex=I_DIRECT_FLASH_TOTOL_NUMS;
			//出牌设置
			if (cbGameStation==GS_MJ_PLAY && !bIsMeIsLookonMode)
			{
				if (pStatusPlay->bIsPlayDice)
				{
					KillGameTimer(IDI_SZSTART);
					SetGameTimer(m_wBankerUser,IDI_SZSTART,5);
					if (byMeChairID==m_wBankerUser)
					{
						m_GameClientView.PlaySZFlashBT();
					}
				}
				else
				{
					if (pStatusPlay->m_bTingCard)
					{
						//m_bHearStatus = false;
						//m_GameClientView.m_btTingCard.ShowWindow(SW_SHOW);	
						//m_GameClientView.m_btBuTing.ShowWindow(SW_SHOW);
						m_wCurrentUser = byMeChairID;
						//设置
						SetGameTimer(m_wCurrentUser, IDI_USER_TING_CARD, TIME_USERTINGCARD);
					}
					else
					{
						BYTE byViewID = SwitchViewChairID(m_wOutCardUser);

						if (byMeChairID == m_wCurrentUser)
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
								//m_GameClientView.m_ControlWnd.SetControlInfo(pStatusPlay->cbActionCard,cbActionMask,bucardresult);
								//m_GameClientView.m_ControlWnd.AddGangCardData(pStatusPlay->byGangCard, pStatusPlay->byGangCardNums);

								m_byCanSelectCount = pStatusPlay->byBuCardNums;
								memcpy(m_byCanSelectCard, pStatusPlay->byBuCard, sizeof(m_byCanSelectCard));
								m_GameClientView.m_ControlWnd.SetAction(pStatusPlay->cbActionCard, cbActionMask);

								//if (m_bHearStatus)
								//{
								//	if (pStatusPlay->cbActionMask&WIK_CHI_HU)
								//	{
								//		//点击胡
								//		OnCardOperate(WIK_CHI_HU,pStatusPlay->cbActionCard);
								//		return true;
								//	}
								//	//取消,并且出牌,并设置定时器出牌.
								//	OnCardOperate(WIK_NULL,0);
								//	BYTE cbCardData=m_GameClientView.m_HandCardControl.GetCurrentCard();
								//	OnOutCard(cbCardData,cbCardData);
								//}
								//else
								{

									m_GameClientView.SetOutCardInfo(byViewID,m_cbOutCardData);

									//设置定时器操作
									m_bIsMeCanOutCard  = true;
									SetGameTimer(byMeChairID,IDI_OPERATE_CARD,TIME_OPERATE_CARD);
								}
							}
							else
							{//出牌

								m_GameClientView.SetOutCardInfo(byViewID,m_cbOutCardData);

								//出牌
								m_byUserAction[byMeChairID] = WIK_NULL;
								m_wCurrentUser = byMeChairID;
								memset(m_byUserAction, 0, sizeof(m_byUserAction));
								SetGameTimer(byMeChairID,IDI_OTHER_USEROUTCARD,TIME_HEAR_STATUS-10);
								//出牌提示
								//m_GameClientView.SetStatusFlag(true,false);
							}
						}
						else
						{
							m_GameClientView.SetOutCardInfo(byViewID,m_cbOutCardData);

							if ((pStatusPlay->cbActionMask!=WIK_NULL) && (!pStatusPlay->bIsAlreadyOper))
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
								//m_GameClientView.m_ControlWnd.SetControlInfo(pStatusPlay->cbActionCard,cbActionMask,bucardresult);
								//m_GameClientView.m_ControlWnd.AddGangCardData(pStatusPlay->byGangCard, pStatusPlay->byGangCardNums);
								m_byCanSelectCount = pStatusPlay->byBuCardNums;
								memcpy(m_byCanSelectCard, pStatusPlay->byBuCard, sizeof(m_byCanSelectCard));
								m_GameClientView.m_ControlWnd.SetAction(pStatusPlay->cbActionCard, cbActionMask);

								//设置定时器操作
								m_wCurrentUser = byMeChairID;
								SetGameTimer(byMeChairID,IDI_OPERATE_CARD,TIME_OPERATE_CARD);
							}
						}
					}
				}
			}
			////海底设置
			//else if (cbGameStation==GS_MJ_HAI_DI && !bIsMeIsLookonMode)
			//{
			//	//m_GameClientView.m_btHaiDi.ShowWindow(SW_HIDE);
			//	//m_GameClientView.m_btNoHaiDi.ShowWindow(SW_HIDE);
			//	//设置控制
			//	if (byMeChairID==m_wCurrentUser)
			//	{
			//		//m_GameClientView.m_btHaiDi.ShowWindow(SW_SHOW);
			//		//m_GameClientView.m_btNoHaiDi.ShowWindow(SW_SHOW);
			//	}
			//	//设置时间
			//	SetGameTimer(m_wCurrentUser,IDI_OPERATE_HAI_DI,TIME_OPERATE_HAI_DI);
			//}

			// 托管按钮隐藏
			if(bIsMeIsLookonMode==true)
			{
				//m_GameClientView.m_btHaiDi.ShowWindow(SW_HIDE);
				//m_GameClientView.m_btNoHaiDi.ShowWindow(SW_HIDE);
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
	KillGameTimer(IDI_USER_TING_CARD);
	KillGameTimer(IDI_OTHER_USEROUTCARD);
	KillGameTimer(IDI_SZSTART);
	KillGameTimer(IDI_SELECT_OUT_CARD);
}

void CGameClientDlg::SendCardCommand(BYTE byOperatorCode, BYTE byOperatorCard)
{
	KillGameTimer(IDI_SELECT_OUT_CARD);

	//发送命令
	CMD_C_OperateCard OperateCard;
	OperateCard.cbOperateCode=byOperatorCode;
	OperateCard.cbOperateCard=byOperatorCard;
	if (byOperatorCode==9 && 0!=byOperatorCard)
	{
		OperateCard.cbOperateCode=WIK_GANG;
		OperateCard.cbOperateCard=byOperatorCard;
	}
	//else if (byOperatorCode==10 && 0!=byOperatorCard)
	//{
	//	OperateCard.cbOperateCode=WIK_TING;
	//	OperateCard.cbOperateCard=byOperatorCard;
	//}
	SendData(SUB_C_OPERATE_CARD,&OperateCard,sizeof(CMD_C_OperateCard));

	memset(m_byUserAction, 0, sizeof(m_byUserAction));

	//m_GameClientView.SetStatusFlag(false,false);
	if (0==byOperatorCode)
	{
		if (m_bIsMeCanOutCard)
		{//自己还可以出牌.
			m_byUserAction[GetMeChairID()] = WIK_NULL;
			//出牌
			m_wCurrentUser = GetMeChairID();
			memset(m_byUserAction, 0, sizeof(m_byUserAction));
			SetGameTimer(GetMeChairID(),IDI_OTHER_USEROUTCARD,TIME_HEAR_STATUS-10);
			//出牌提示
			//m_GameClientView.SetStatusFlag(true,false);
		}
	}

	m_byCanSelectType = WIK_NULL;
	m_byCanSelectCount = 0;
	ZeroMemory(m_byCanSelectCard,sizeof(m_byCanSelectCard));
	m_GameClientView.m_HandCardControl.SetSelectStatus(false);
	m_GameClientView.UpdateGameView(NULL);
}

//是否开始骰子动画
bool CGameClientDlg::OnSubStartSZFlash()
{
	KillGameTimer(IDI_SZSTART);
	KillTimer(IDI_SHOW_SZ);
	SetTimer(IDI_SHOW_SZ,TIME_SHOW_SZ,NULL);
	m_GameClientView.UpdateGameView(NULL);
	return true;
}

//游戏开始
bool CGameClientDlg::OnSubGameStart(const void * pBuffer, WORD wDataSize)
{
	PlayWaitFlash(false);

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

	//char sz[500]={0};
	//sprintf(sz,"\n游戏开始：庄家=%d,当前玩家=%d(视图ID=%d,自己位置=%d),麻将数据=%d,%s,%d,%s,%d,%s,%d,%s,%d,%s,%d,%s,%d,%s,%d,%s,%d,%s,%d,%s,%d,%s,%d,%s,%d,%s,%d,%s\n",pGameStart->wBankerUser,pGameStart->wCurrentUser,SwitchViewChairID(pGameStart->wCurrentUser),GetMeChairID(),
	//	pGameStart->cbCardData[0],
	//	CGameLogic::GetMJCardName(pGameStart->cbCardData[0]),
	//	pGameStart->cbCardData[1],
	//	CGameLogic::GetMJCardName(pGameStart->cbCardData[1]),
	//	pGameStart->cbCardData[2],
	//	CGameLogic::GetMJCardName(pGameStart->cbCardData[2]),
	//	pGameStart->cbCardData[3],
	//	CGameLogic::GetMJCardName(pGameStart->cbCardData[3]),
	//	pGameStart->cbCardData[4],
	//	CGameLogic::GetMJCardName(pGameStart->cbCardData[4]),
	//	pGameStart->cbCardData[5],
	//	CGameLogic::GetMJCardName(pGameStart->cbCardData[5]),
	//	pGameStart->cbCardData[6],
	//	CGameLogic::GetMJCardName(pGameStart->cbCardData[6]),
	//	pGameStart->cbCardData[7],
	//	CGameLogic::GetMJCardName(pGameStart->cbCardData[7]),
	//	pGameStart->cbCardData[8],
	//	CGameLogic::GetMJCardName(pGameStart->cbCardData[8]),
	//	pGameStart->cbCardData[9],
	//	CGameLogic::GetMJCardName(pGameStart->cbCardData[9]),
	//	pGameStart->cbCardData[10],
	//	CGameLogic::GetMJCardName(pGameStart->cbCardData[10]),
	//	pGameStart->cbCardData[11],
	//	CGameLogic::GetMJCardName(pGameStart->cbCardData[11]),
	//	pGameStart->cbCardData[12],
	//	CGameLogic::GetMJCardName(pGameStart->cbCardData[12]),
	//	pGameStart->cbCardData[13],
	//	CGameLogic::GetMJCardName(pGameStart->cbCardData[13])
	//	);
	//TRACE(sz);
	//ASSERT(pGameStart->wBankerUser==pGameStart->wCurrentUser);
	//if(GetMeChairID()!=pGameStart->wCurrentUser)//自己不是庄家
	//	ASSERT(pGameStart->cbCardData[13]==0);
	//else//自己是庄家
	//	ASSERT(pGameStart->cbCardData[13]>0);

	m_GameClientView.PlaySZFlashBT(false);
	::memset(&m_GameStartData, 0, sizeof(CMD_S_GameStart));
	::memcpy(&m_GameStartData, pGameStart, sizeof(CMD_S_GameStart));

	//清空所有定时器
	ClearAllTimer();
	m_bIsMeCanOutCard = false;
	m_bStustee = false;

	//设置状态
	SetGameStatus(GS_MJ_PLAY);


	//方位动画
	KillTimer(IDI_SHOW_DIRECT);
	SetTimer(IDI_SHOW_DIRECT,TIME_SHOW_DIRECT,NULL);


	//设置变量
	//m_bHearStatus=false;
	m_wBankerUser=pGameStart->wBankerUser;
	m_wCurrentUser=pGameStart->wCurrentUser;
	m_cbLeftCardCount=MAX_REPERTORY-GAME_PLAYER*(MAX_COUNT-1)-1;

	m_GameClientView.m_nDiceCount1 =HIBYTE( pGameStart->wSiceCount)-1;
	m_GameClientView.m_nDiceCount2 =LOBYTE( pGameStart->wSiceCount)-1;

	//出牌信息
	m_cbOutCardData=0;
	m_wOutCardUser=INVALID_CHAIR;
	ZeroMemory(m_cbDiscardCard,sizeof(m_cbDiscardCard));
	ZeroMemory(m_cbDiscardCount,sizeof(m_cbDiscardCount));

	//组合扑克
	ZeroMemory(m_cbWeaveCount,sizeof(m_cbWeaveCount));
	ZeroMemory(m_WeaveItemArray,sizeof(m_WeaveItemArray));

	//设置扑克
	BYTE byMeChairID = GetMeChairID();
	BYTE cbCardCount=(byMeChairID==m_wBankerUser)?MAX_COUNT:(MAX_COUNT-1);
	m_GameLogic.SwitchToCardIndex(pGameStart->cbCardData,cbCardCount,m_cbCardIndex);

	//设置界面
	bool bPlayerMode=(IsLookonMode()==false);
	m_GameClientView.m_HandCardControl.SetPositively(bPlayerMode);
	m_GameClientView.SetBankerUser(SwitchViewChairID(m_wBankerUser));
	//m_ScoreView.ShowWindow(SW_HIDE);
	SafeShowOrHideWnd(&m_ScoreView, SW_HIDE);
	m_GameClientView.m_btStart.ShowWindow(SW_HIDE);
	m_GameClientView.m_btAutoPlayOn.ShowWindow(SW_HIDE);
	m_GameClientView.m_btAutoPlayOff.ShowWindow(SW_HIDE);

	//m_GameClientView.m_nKingCardIndex=pGameStart->wMPIndex;


	//堆立扑克
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		m_cbHeapCardInfo[i][0]=0;
		m_cbHeapCardInfo[i][1]=HEAP_FULL_COUNT-(((i==m_wBankerUser)||((i+2)%4==m_wBankerUser))?28:26);
		m_GameClientView.m_HeapCard[SwitchViewChairID(i)].SetCardData(m_cbHeapCardInfo[i][0],m_cbHeapCardInfo[i][1],HEAP_FULL_COUNT);
	}


	//char cbData[128]={0};
	//sprintf(cbData,"庄家:%d 本人:%d", m_wBankerUser, byMeChairID);
	//InsertSystemString(cbData);

	return true;
}

//保存上一轮的牌数据
void CGameClientDlg::LastTurnOperateCard()
{
	//添加上一轮打出的牌放到丢弃牌队列,并且隐藏桌面显示牌,销毁动画定时器。


	//丢弃扑克
	if ((m_wOutCardUser!=INVALID_CHAIR)&&(m_cbOutCardData!=0))
	{
		//丢弃扑克 
		WORD wOutViewChairID=SwitchViewChairID(m_wOutCardUser);
		m_GameClientView.m_DiscardCard[wOutViewChairID].AddCardItem(m_cbOutCardData);

		m_cbOutCardData=0;
		m_wOutCardUser=INVALID_CHAIR;

		char cbData[128]={0};
		sprintf(cbData,"上一轮出牌玩家:%d, 数据:%x,%s",m_wOutCardUser,m_cbOutCardData,CGameLogic::GetMJCardName(m_cbOutCardData));
		TRACE(cbData);
	}

	m_GameClientView.SetOutCardInfo(INVALID_CHAIR,0);
	m_GameClientView.UpdateGameView(NULL);

}

//用户出牌
bool CGameClientDlg::OnSubOutCard(const void * pBuffer, WORD wDataSize)
{
	//效验消息
	ASSERT(wDataSize==sizeof(CMD_S_OutCard));
	if (wDataSize!=sizeof(CMD_S_OutCard)) return false;


	//处理上一轮显示的牌
	LastTurnOperateCard();


	//消息处理
	CMD_S_OutCard * pOutCard=(CMD_S_OutCard *)pBuffer;

	//变量定义
	WORD wMeChairID=GetMeChairID();
	WORD wOutViewChairID=SwitchViewChairID(pOutCard->wOutCardUser);

	char sz[100]={0};
	sprintf(sz,"\n用户出牌：%d(视图ID=%d,自己位置=%d),%d,%s\n",pOutCard->wOutCardUser,wOutViewChairID,wMeChairID,pOutCard->cbOutCardData,CGameLogic::GetMJCardName(pOutCard->cbOutCardData));
	TRACE(sz);

	//设置变量
	m_wCurrentUser=INVALID_CHAIR;
	m_wOutCardUser=pOutCard->wOutCardUser;
	m_cbOutCardData=pOutCard->cbOutCardData;
	m_bIsMeCanOutCard = false;

	KillGameTimer(IDI_OPERATE_CARD); 
	KillGameTimer(IDI_OTHER_USEROUTCARD);
	KillGameTimer(IDI_OTHER_USEROUTCARD);

	//m_GameClientView.SetStatusFlag(false,false);

	memset(m_byUserAction, 0 , sizeof(m_byUserAction));


	////丢弃扑克
	//if ((m_wOutCardUser!=INVALID_CHAIR)&&(m_cbOutCardData!=0))
	//{
	//	//丢弃扑克 
	//	WORD wOutViewChairID=SwitchViewChairID(m_wOutCardUser);
	//	m_GameClientView.m_DiscardCard[wOutViewChairID].AddCardItem(m_cbOutCardData);
	//}

	//PlayCardSound(pOutCard->cbOutCardData);
	//出牌界面
	m_GameClientView.SetUserAction(INVALID_CHAIR,0);
	m_GameClientView.SetOutCardInfo(wOutViewChairID,pOutCard->cbOutCardData);

	//其他用户
	//if (IsLookonMode()==true)
	{
		//环境设置
		//设置扑克
		if (wOutViewChairID==2)
		{
			//删除扑克
			BYTE cbCardData[MAX_COUNT]={0};
			m_GameLogic.RemoveCard(m_cbCardIndex,pOutCard->cbOutCardData);

			//设置扑克
			BYTE cbCardCount=m_GameLogic.SwitchToCardData(m_cbCardIndex,cbCardData);
			m_GameClientView.m_HandCardControl.SetCardData(cbCardData,cbCardCount,0/*,m_cbTempKing[0]*/);
		}
		else
		{
			WORD wUserIndex=(wOutViewChairID>2)?2:wOutViewChairID;
			m_GameClientView.m_UserCard[wUserIndex].SetCurrentCard(false);
		}
	} 

	//声音
	const tagUserData *pUserData = GetUserData(pOutCard->wOutCardUser);//用户信息
	if (m_sDialect=="YY_")
	{
		int num=rand()%20;
		if (9==num)
		{
			{
				PlayGameSoundRand(pUserData->cbGender, TEXT("Rand"), true);
			}
		}
		else
		{
			PlayCardSound(pUserData->cbGender, pOutCard->cbOutCardData);
		}

	}
	else
	{
		PlayCardSound(pUserData->cbGender, pOutCard->cbOutCardData);
	}
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

	char sz[100]={0};
	sprintf(sz,"\n发牌消息：当前玩家=%d(视图ID=%d,自己位置=%d),麻将数据=%d,%s,当前命令:%d\n",pSendCard->wCurrentUser,SwitchViewChairID(pSendCard->wCurrentUser),GetMeChairID(),pSendCard->cbCardData,CGameLogic::GetMJCardName(pSendCard->cbCardData),pSendCard->cbActionMask);
	TRACE(sz);

	//m_GameClientView.m_ControlWnd.ShowWindow(SW_HIDE);
	SafeShowOrHideWnd(&m_GameClientView.m_ControlWnd, SW_HIDE);

	//清空所有定时器
	ClearAllTimer();
	m_bIsMeCanOutCard = false;

	//设置变量
	WORD wMeChairID=GetMeChairID();
	m_wCurrentUser=pSendCard->wCurrentUser;

	////丢弃扑克
	//if ((m_wOutCardUser!=INVALID_CHAIR)&&(m_cbOutCardData!=0))
	//{
	//	//丢弃扑克 
	//	WORD wOutViewChairID=SwitchViewChairID(m_wOutCardUser);
	//	m_GameClientView.m_DiscardCard[wOutViewChairID].AddCardItem(m_cbOutCardData);
	//}

	////设置变量
	//m_cbOutCardData=0;
	//m_wOutCardUser=INVALID_CHAIR;

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

			tagCardItem CardItem;
			CardItem.cbCardData = pSendCard->cbCardData;
			CardItem.bShoot = false;
			CardItem.bKingFlag = false;//(pSendCard->cbCardData == m_cbTempKing[0])?true:false;
			m_GameClientView.m_HandCardControl.SetCurrentCard(CardItem);
			//m_GameClientView.m_HandCardControl.SetCurrentCard(pSendCard->cbCardData);
		}

		//扣除扑克
		//by Ivan_han 20111229，解决补张以后，最后的一张牌墙没有消失的BUG
		if ( pSendCard->bSupplyCard==true/* && m_byUserAction[pSendCard->wCurrentUser] != WIK_NULL 
										 && (m_byUserAction[pSendCard->wCurrentUser] & WIK_FILL 
										 ||m_byUserAction[pSendCard->wCurrentUser] & WIK_GANG) */)
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
			//m_GameClientView.m_ControlWnd.SetControlInfo(pSendCard->cbCardData,cbActionMask,bucardresult);
			//m_GameClientView.m_ControlWnd.AddGangCardData(pSendCard->byGangCard, pSendCard->byGangCardNums);

			m_byCanSelectCount = pSendCard->byBuCardNums;
			memcpy(m_byCanSelectCard, pSendCard->byBuCard, sizeof(m_byCanSelectCard));
			m_GameClientView.m_ControlWnd.SetAction(pSendCard->cbCardData, cbActionMask);
			//m_GameClientView.m_ControlWnd.ShowWindow(SW_SHOW);
			SafeShowOrHideWnd(&m_GameClientView.m_ControlWnd, SW_SHOW);

			//设置界面
			//m_GameClientView.m_ControlWnd.SetControlInfo(cbActionCard,cbActionMask,GangCardResult);
			//激活框架
		}

		//if (m_bHearStatus)
		//{//m_GameClientView.m_ControlWnd.m_btChiHu.IsWindowVisible() && m_GameClientView.m_ControlWnd.m_btChiHu.IsWindowEnabled()
		//	if(pSendCard->cbActionMask&WIK_CHI_HU)
		//	{
		//		OnCardOperate(WIK_CHI_HU,0);
		//	}
		//	else
		//	{
		//		if (WIK_NULL != m_byUserAction[wMeChairID])
		//		{
		//			OnCardOperate(WIK_NULL,0);
		//		}
		//		else
		//		{
		//			BYTE cbCardData=m_GameClientView.m_HandCardControl.GetCurrentCard();
		//			OnOutCard(cbCardData,cbCardData);
		//		}
		//	}
		//	return true;
		//}

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

		////出牌提示
		//if (pSendCard->bSupplyCard)
		//{
		//	PlaySound("holdOneCard", false);
		//}
		//else
		//{
		//	//用户信息
		//	const tagUserData *pUserData = GetUserData(pSendCard->wCurrentUser);
		//	if (NULL != pUserData)
		//	{
		//		PlayGameSoundLanguage(pUserData->cbGender, TEXT("SupplyCard"), false);
		//	}
		//}
		PlaySound("holdOneCard", false);
		//m_GameClientView.SetStatusFlag(true,false);
		ActiveGameFrame();
	}
	else
	{
		SetGameTimer(m_wCurrentUser,IDI_OPERATE_CARD,TIME_HEAR_STATUS);
		//出牌提示
		//m_GameClientView.SetStatusFlag(false,false);
	}

	//声音
	//const tagUserData *pUserData = GetUserData(pSendCard->wCurrentUser);//用户信息

	//if (m_sDialect=="YY_")
	//{
	//	if (pSendCard->cbCardData == m_cbTempKing[0])
	//	{
	//		int num=rand()%2;
	//		if (pUserData->cbGender==1 && 0==num)
	//		{
	//			PlaySound("YY_King_W_1",false);
	//		}
	//		else if((pUserData->cbGender==0 && 1==num))
	//		{
	//			PlaySound("YY_King_M_1",false);
	//		}
	//	}
	//}

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
	char sz[200]={0};
	sprintf(sz,"\n操作提示：%d(视图ID=%d,自己位置=%d),动作掩码=%d,动作麻将=%d,%s\n",pOperateNotify->wResumeUser,SwitchViewChairID(pOperateNotify->wResumeUser),GetMeChairID(),pOperateNotify->cbActionMask,pOperateNotify->cbActionCard,CGameLogic::GetMJCardName(pOperateNotify->cbActionCard));
	TRACE(sz);

	m_bIsMeCanOutCard = false;
	KillGameTimer(IDI_OPERATE_CARD);
	WORD byChairID = GetMeChairID();
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

		//杠牌判断
		//if ((cbActionMask&(WIK_GANG|WIK_FILL|WIK_TING))!=0)
		//{
		//	//桌面杆牌
		//	if ((m_wCurrentUser==INVALID_CHAIR)&&(cbActionCard!=0))
		//	{
		//		bucardresult.cbCardCount=1;
		//		bucardresult.cbCardData[0]=cbActionCard;
		//	}
		//	//自己杆牌
		//	else if ((m_wCurrentUser==GetMeChairID())||(cbActionCard==0))
		//	{
		//		WORD wMeChairID=GetMeChairID();
		//		m_GameLogic.AnalyseGangCard(m_cbCardIndex,m_WeaveItemArray[wMeChairID],m_cbWeaveCount[wMeChairID],bucardresult);
		//	}
		//}

		//显示界面
		::memcpy(bucardresult.cbCardData, pOperateNotify->byBuCard, sizeof(BYTE)*pOperateNotify->byBuCardNums);
		bucardresult.cbCardCount = pOperateNotify->byBuCardNums;
		//m_GameClientView.m_ControlWnd.SetControlInfo(pOperateNotify->cbActionCard,cbActionMask,bucardresult);
		//m_GameClientView.m_ControlWnd.AddGangCardData(pOperateNotify->byGangCard, pOperateNotify->byGangCardNums);

		m_byCanSelectCount = pOperateNotify->byBuCardNums;
		memcpy(m_byCanSelectCard, pOperateNotify->byBuCard, sizeof(m_byCanSelectCard));
		m_GameClientView.m_ControlWnd.SetAction(pOperateNotify->cbActionCard, cbActionMask);


		//if ( m_bHearStatus)
		//{//听牌了
		//	if( m_GameClientView.m_ControlWnd.m_btChiHu.IsWindowVisible() 
		//		&& m_GameClientView.m_ControlWnd.m_btChiHu.IsWindowEnabled())
		//	{
		//		OnCardOperate(WIK_CHI_HU,0);
		//	}
		//	else if (m_GameClientView.m_ControlWnd.m_btGiveUp.IsWindowVisible() 
		//				&& m_GameClientView.m_ControlWnd.m_btGiveUp.IsWindowEnabled())
		//	{
		//		OnCardOperate(WIK_NULL,0);
		//	}
		//}
		//else
		//{
		SetGameTimer(byChairID,IDI_OPERATE_CARD,TIME_OPERATE_CARD);
		//}
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
	char sz[300]={0};
	//bool							bBringMuscleCanHu;				//明筋能胡牌
	//BYTE							cbOperateCode;						//操作代码
	//BYTE							cbOperateCard;						//操作扑克
	//BYTE								bMuscle;							// 筋方式, 0 无效值, 1 暗筋，2 明筋，3 放筋
	//WORD							wOperateUser;						//操作用户
	//WORD							wProvideUser;						//供应用户
	//LONGLONG					llMuscleGold[GAME_PLAYER]; //金币更动值
	static const char * szMuscle[4]={"无效值","暗筋","明筋","放筋"};
	sprintf(sz,"\n操作结果消息：bBringMuscleCanHu=%d,自己位置=%d,操作用户=%d(操作用户视图ID=%d),供应用户=%d(供应用户视图ID=%d),麻将数据=%d,%s,操作代码:%d,筋方式=%d,%s\n",pOperateResult->bBringMuscleCanHu,GetMeChairID(),pOperateResult->wOperateUser,SwitchViewChairID(pOperateResult->wOperateUser),pOperateResult->wProvideUser,SwitchViewChairID(pOperateResult->wProvideUser),pOperateResult->cbOperateCard,CGameLogic::GetMJCardName(pOperateResult->cbOperateCard),pOperateResult->cbOperateCode,pOperateResult->bMuscle,szMuscle[pOperateResult->bMuscle]);
	TRACE(sz);

	m_GameClientView.SetOutCardInfo(INVALID_CHAIR,0);

	//变量定义
	BYTE cbPublicCard=TRUE;
	WORD wOperateUser=pOperateResult->wOperateUser;
	BYTE cbOperateCard=pOperateResult->cbOperateCard;
	WORD wOperateViewID=SwitchViewChairID(wOperateUser);

	//出牌变量
	if(pOperateResult->cbOperateCode!=WIK_NULL)
	{
		m_cbOutCardData=0;
		m_wOutCardUser=INVALID_CHAIR;
	}

	KillGameTimer(IDI_OTHER_USEROUTCARD);
	m_GameClientView.SetOutCardInfo(INVALID_CHAIR,0);
	//m_GameClientView.m_ControlWnd.ShowWindow(SW_HIDE);
	SafeShowOrHideWnd(&m_GameClientView.m_ControlWnd, SW_HIDE);

	m_bIsMeCanOutCard = false;
	memset(m_byUserAction, 0, sizeof(m_byUserAction));
	//设置组合
	if ((pOperateResult->cbOperateCode&(WIK_GANG|WIK_FILL|WIK_TING))>0)
	{//主动玩家,并且是补、杠、听
		//为当前操作玩家
		m_wCurrentUser=INVALID_CHAIR;
		//设置变量
		//if ((pOperateResult->cbOperateCode==WIK_TING)&&(wOperateUser==GetMeChairID())) 
		//{
		//	m_bHearStatus = true;
		//}

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

		CString strPath="";
		m_GameClientView.GetPicPath(strPath);
		strPath.Append("Cartoon\\");

		// 筋动画
		if ( !pOperateResult->bBringMuscleCanHu  &&  0!=pOperateResult->bMuscle)
		{
			LONGLONG llVal[GAME_PLAYER];
			ZeroMemory(llVal, sizeof(llVal));

			for (BYTE i=0; i<GAME_PLAYER; i++)
			{
				llVal[SwitchViewChairID(i)] = pOperateResult->llMuscleGold[i];
			}

			CString strGold;
			strGold.Format("client: CGameClientDlg::OnSubOperateResult  %I64d,%I64d,%I64d,%I64d", llVal[0], llVal[1], llVal[2], llVal[3]);
			OutputDebugString(strGold);

			//筋的结算显示在即时聊天对话框（add by Ivan_han 20111227）
			const tagUserData *pUserData = GetUserData(GetMeChairID());//用户信息
			ASSERT(pUserData!=NULL);
			ASSERT(pOperateResult->bMuscle>=1 && pOperateResult->bMuscle<=3);
			TCHAR szBuffer[64];
			_snprintf(szBuffer,CountArray(szBuffer),TEXT("\n本次筋结算,成绩统计:"));
			InsertGeneralString(szBuffer,RGB(255,0,0),true);

			static const char *	szMuscle[4]={"无效值","暗筋","明筋","放筋"};
			static const char *	szDirect[4]={"庄家","南方","西方","北方"};
			char sz[100]={0};
			if(pOperateResult->bMuscle==3)//"放筋"
			{

				WORD wIndex=((pOperateResult->wProvideUser+GAME_PLAYER)-m_wBankerUser)%GAME_PLAYER;
				strcpy(sz,szDirect[wIndex]);
			}
			else
			{
				if(pOperateResult->wOperateUser==GetMeChairID())//是自己"暗筋","明筋"
					strcpy(sz,pUserData->szName);
				else
				{

					WORD wIndex=((pOperateResult->wOperateUser+GAME_PLAYER)-m_wBankerUser)%GAME_PLAYER;
					strcpy(sz,szDirect[wIndex]);
				}	
			}
			strcat(sz,szMuscle[pOperateResult->bMuscle]);
			//strcat(sz,"\n");
			InsertGeneralString(sz,RGB(255,255,0),true);
			TRACE(sz);

			for	(WORD i=0;i<GAME_PLAYER;i++)
			{
				WORD wIndex=((i+GAME_PLAYER)-m_wBankerUser)%GAME_PLAYER;
				WORD wViewID=SwitchViewChairID(i);
				ASSERT(wIndex>=0 &&	wIndex<4);
				ASSERT(wViewID>=0 && wViewID<4);
				if (2==wViewID)
				{
					_snprintf(szBuffer,CountArray(szBuffer),TEXT("%s："),pUserData->szName);
				} 
				else
				{
					_snprintf(szBuffer,CountArray(szBuffer),TEXT("%s："),szDirect[wIndex]);
				}
				InsertGeneralString(szBuffer,RGB(0,255,0),false);
				_snprintf(szBuffer,CountArray(szBuffer),TEXT("%+I64d"),llVal[wViewID]);
				InsertGeneralString(szBuffer,RGB(255,255,0),true);
			}
			//服务费
			LONGLONG lltax=-(llVal[0]+llVal[1]+llVal[2]+llVal[3]);
			_snprintf(szBuffer,CountArray(szBuffer),TEXT("\n服务费:%d"),lltax);
			InsertGeneralString(szBuffer,RGB(255,255,0),true);

			m_GameClientView.PlayMuscleFlash(wOperateViewID, 255, pOperateResult->bMuscle, llVal);
			PlaySound(TEXT("muscle"), false);
		}

		//扑克设置
		if (GetMeChairID()==wOperateUser)
		{
			m_cbCardIndex[m_GameLogic.SwitchToCardIndex(pOperateResult->cbOperateCard)]=0;
		}

		//设置扑克
		if (GetMeChairID()==wOperateUser)
		{
			BYTE cbCardData[MAX_COUNT]={0};
			BYTE cbCardCount=m_GameLogic.SwitchToCardData(m_cbCardIndex,cbCardData);
			m_GameClientView.m_HandCardControl.SetCardData(cbCardData,cbCardCount,0/*,m_cbTempKing[0]*/);
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

		//设置扑克
		if (GetMeChairID()==wOperateUser)
		{
			//删除扑克
			m_GameLogic.RemoveCard(cbWeaveCard,cbWeaveCardCount,&cbOperateCard,1);
			m_GameLogic.RemoveCard(m_cbCardIndex,cbWeaveCard,cbWeaveCardCount-1);


			BYTE cbCardData[MAX_COUNT]={0};
			BYTE cbCardCount=m_GameLogic.SwitchToCardData(m_cbCardIndex,cbCardData);
			tagCardItem CardItem;
			CardItem.cbCardData = cbCardData[cbCardCount-1];
			CardItem.bShoot = false;
			CardItem.bKingFlag = false;//(cbCardData[cbCardCount-1] == m_cbTempKing[0])?true:false;
			m_GameClientView.m_HandCardControl.SetCurrentCard(CardItem);
			m_GameClientView.m_HandCardControl.SetCardData(cbCardData,cbCardCount-1,cbCardData[cbCardCount-1]/*,m_cbTempKing[0]*/);
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
	//m_GameClientView.SetStatusFlag((IsLookonMode()==false)&&(m_wCurrentUser==GetMeChairID()),false);
	m_GameClientView.PlayOperationFlash(wOperateViewID,pOperateResult->cbOperateCode);
	//char cbData[128]={0};
	//sprintf(cbData,"%d.....%x",wOperateViewID,pOperateResult->cbOperateCode);
	//InsertSystemString(cbData);

	//更新界面
	m_GameClientView.UpdateGameView(NULL);

	//环境设置
	const tagUserData *pUserData = GetUserData(pOperateResult->wOperateUser);//用户信息
	PlayActionSound(pUserData->cbGender, pOperateResult->cbOperateCode);

	return true;
}

////海底操作
//bool CGameClientDlg::OnSubOperateHaiDi(const void * pBuffer, WORD wDataSize)
//{
//	//效验数据
//	ASSERT(wDataSize==sizeof(CMD_S_OperateHaiDi));
//	if (wDataSize!=sizeof(CMD_S_OperateHaiDi)) return false;
//
//	//消息处理
//	CMD_S_OperateHaiDi * pOperateHaiDi=(CMD_S_OperateHaiDi *)pBuffer;
//
//	//设置状态
//	SetGameStatus(GS_MJ_HAI_DI);
//
//	m_GameClientView.SetOutCardInfo(INVALID_CHAIR,0);
//
//	//设置变量
//	m_bIsMeCanOutCard = false;
//	m_wCurrentUser=INVALID_CHAIR;
//	//m_GameClientView.SetStatusFlag(false,false);
//	//m_GameClientView.m_ControlWnd.ShowWindow(SW_HIDE);
//	SafeShowOrHideWnd(&m_GameClientView.m_ControlWnd, SW_HIDE);
//
//	//丢弃扑克
//	if ((m_wOutCardUser!=INVALID_CHAIR)&&(m_cbOutCardData!=0))
//	{
//		//丢弃扑克
//		WORD wOutViewChairID=SwitchViewChairID(m_wOutCardUser);
//		m_GameClientView.m_DiscardCard[wOutViewChairID].AddCardItem(m_cbOutCardData);
//
//		//设置变量
//		m_cbOutCardData=0;
//		m_wOutCardUser=INVALID_CHAIR;
//	}
//
//	//界面设置
//	if ((IsLookonMode()==false)&&(GetMeChairID()==pOperateHaiDi->wCurrentUser))
//	{
//		m_wCurrentUser = GetMeChairID();
//		//m_GameClientView.m_btHaiDi.ShowWindow(SW_SHOW);
//		//m_GameClientView.m_btNoHaiDi.ShowWindow(SW_SHOW);
//		ActiveGameFrame();
//	}
//
//	m_GameClientView.UpdateGameView(NULL);
//	//时间设置
//	SetGameTimer(pOperateHaiDi->wCurrentUser,IDI_OPERATE_HAI_DI,TIME_OPERATE_HAI_DI);
//
//	return true;
//}



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

	m_GameClientView.PlaySZFlashBT(false);
	//设置状态
	SetGameStatus(GS_MJ_FREE);
	//m_GameClientView.SetStatusFlag(false,false);

	//设置控件
	m_bIsMeCanOutCard = false;
	//m_GameClientView.SetStatusFlag(false,false);
	//m_GameClientView.m_btHaiDi.ShowWindow(SW_HIDE);
	//m_GameClientView.m_btNoHaiDi.ShowWindow(SW_HIDE);
	//m_GameClientView.m_ControlWnd.ShowWindow(SW_HIDE);
	SafeShowOrHideWnd(&m_GameClientView.m_ControlWnd, SW_HIDE);
	m_GameClientView.m_HandCardControl.SetPositively(false);

	//初始化出牌信息
	m_GameClientView.SetOutCardInfo(INVALID_CHAIR, 0);	

	//结束设置
	//char chEndData[100]={0};
	//sprintf(chEndData, "pGameEnd->cbHaiDiCard=%d", pGameEnd->cbHaiDiCard);
	//InsertGeneralString(chEndData, RGB(0, 255, 0), true);

	WORD MyChairId=GetMeChairID();
	m_ScoreView.GetViewID(GetMeChairID(),m_wBankerUser);

	m_GameClientView.m_wChiHuType=pGameEnd->iChiHuType;  //  0:荒庄 ;  1:自摸 ; 2:放炮;  3:海底捞月;  4:海底炮；5（海底）荒庄
	m_GameScoreInfo.cbKingCard=pGameEnd->bKingCard;
	m_GameScoreInfo.wBankerUser=m_wBankerUser;
	m_GameScoreInfo.iCellScore=pGameEnd->iCellScore[MyChairId];
	m_GameScoreInfo.iBasicScore=m_GameClientView.GetCellScore();
	//m_GameScoreInfo.cbBirdCard=pGameEnd->cbBirdCard;
	m_GameScoreInfo.cbChiHuCard=pGameEnd->cbChiHuCard;
	m_GameScoreInfo.wProvideUser=pGameEnd->wProvideUser;
	//m_GameScoreInfo.wBirdHitUser=pGameEnd->wBirdHitUser;
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		const tagUserData * pUserData=GetUserData(i);
		WORD byViewID = SwitchViewChairID(i);
		m_GameClientView.m_wChiHuKind[byViewID]=pGameEnd->wChiHuKind[i];	//胡牌种类
		m_GameClientView.m_wChiHuRight[byViewID]=pGameEnd->wChiHuRight[i];	//胡牌权位

		m_GameScoreInfo.iMuscle[i]=pGameEnd->iMuscle[i];
		m_GameScoreInfo.cbTimes[i]=pGameEnd->cbTimes[i];
		m_GameScoreInfo.lScore[i]=pGameEnd->lGameScore[i];
		m_GameScoreInfo.wCardType[i]=pGameEnd->wChiHuKind[i];
		m_GameScoreInfo.cbKingCount[i]=pGameEnd->cbKingCount[i];
		m_GameScoreInfo.iChiHuKind[i]=pGameEnd->iChiHuKind[i];
		lstrcpyn(m_GameScoreInfo.szUserName[i],pUserData->szName,CountArray(m_GameScoreInfo.szUserName[i]));

		// 海底清空
		if (5==m_GameClientView.m_wChiHuType  ||  3==m_GameClientView.m_wChiHuType  ||  4==m_GameClientView.m_wChiHuType)
		{
			m_GameClientView.m_HeapCard[i].SetCardData(0, 0, 0);
		}
	}


	//判断是否有人强退
	bool bIsLeft=false;
	for (int i=0;i<GAME_PLAYER;++i)
	{
		if (pGameEnd->IsUserLeft[i])
		{
			bIsLeft=true;
			break;
		}	
	}

	//成绩显示在即时聊天对话框
	if (!pGameEnd->bIsHuangzhuang)
	{
		TCHAR szBuffer[64];
		_snprintf(szBuffer,CountArray(szBuffer),TEXT("\n本局结束,成绩统计:"));
		InsertGeneralString(szBuffer,RGB(255,0,0),true);
		for (WORD i=0;i<GAME_PLAYER;i++)
		{//变量定义
			if (m_GameScoreInfo.szUserName[i]!=0)
			{
				WORD wIndex=((i+GAME_PLAYER)-m_GameScoreInfo.wBankerUser)%GAME_PLAYER;
				WORD wViewID=SwitchViewChairID(i);
				if (2==wViewID)
				{
					_snprintf(szBuffer,CountArray(szBuffer),TEXT("%s："),m_GameScoreInfo.szUserName[i]);
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

		//服务费
		_snprintf(szBuffer,CountArray(szBuffer),TEXT("\n服务费:%d"),m_GameScoreInfo.iCellScore);
		InsertGeneralString(szBuffer,RGB(255,255,0),true);

		m_ScoreView.SetScoreInfo(m_GameScoreInfo);
		m_GameClientView.m_btStart.ShowWindow(SW_HIDE);
		m_GameClientView.m_bIsHuangZhuang=false;
		m_GameClientView.SetHaiDiCard(pGameEnd->cbHaiDiCard);

		if (!bIsLeft)
		{
			//判断是否自摸、放炮
			m_bZM=false;//自摸
			m_bFP=false;//放炮
			if (m_GameScoreInfo.wProvideUser != INVALID_CHAIR)
			{
				if(CHK_NULL != m_GameScoreInfo.wCardType[m_GameScoreInfo.wProvideUser])
				{
					m_bZM=true;
				}
				else 
				{
					m_bFP=true;
				}	
			}

			//////////////////////////////////////////////////////////////////////////////////
			//播放胡牌动画
			//统计胡牌玩家个数
			BYTE bySaveHuPaiUser[GAME_PLAYER]={-1};
			int iChiHuNum=0;
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				if (pGameEnd->wChiHuKind[i]!=CHK_NULL) 
				{
					bySaveHuPaiUser[iChiHuNum++] = i;
				}
			}
			m_GameClientView.m_iChiHuNums=iChiHuNum;
			if (3==iChiHuNum)
			{//通炮
				//动画
				m_GameClientView.PlayChiHuFlash(0);

				//声音
				PlaySound("fangpao",false);
				for (BYTE i=0; i<GAME_PLAYER;i++)
				{
					// 放炮玩家需要听其他胡牌玩家一次胡牌声音（且只有一家），胡牌玩家只听自己胡牌声音
					if (i==GetMeChairID()  && i==m_GameScoreInfo.wProvideUser)
						continue;

					const tagUserData *pOtherUserData = GetUserData(bySaveHuPaiUser[0]);//用户信息
					if (NULL != pOtherUserData)
					{
						PlayGameSoundLanguage(pOtherUserData->cbGender,TEXT("chihu"),2);
						break;
					}
				}
			}
			else if(m_bZM)
			{//自摸
				const tagUserData *pUserData = GetUserData(m_GameScoreInfo.wProvideUser);//用户信息
				WORD wViewChairID=SwitchViewChairID(m_GameScoreInfo.wProvideUser);
				m_GameClientView.SetUserAction(wViewChairID,WIK_CHI_HU);
				m_byUserAction[m_GameScoreInfo.wProvideUser] = WIK_CHI_HU;

				//动画
				m_GameClientView.PlayChiHuFlash(wViewChairID);

				//声音
				PlayGameSoundLanguage(pUserData->cbGender,TEXT("zimo"));
			}
			else if (iChiHuNum > 0)
			{//放炮
				BYTE byMeChairID = GetMeChairID();
				BYTE byViewChairID = SwitchViewChairID(byMeChairID);
				const tagUserData *pUserData = GetUserData(byMeChairID);//用户信息
				
				//放炮方的动画
				m_GameClientView.PlayFangPaoFlash(SwitchViewChairID(m_GameScoreInfo.wProvideUser));
				//声音
				PlaySound("fangpao",false);

				//别人的胡牌动画
				for (int i=0; i<iChiHuNum; ++i)
				{
					BYTE byOtherViewChairID = SwitchViewChairID(bySaveHuPaiUser[i]);
					m_GameClientView.SetUserAction(byOtherViewChairID,WIK_CHI_HU);
					m_byUserAction[bySaveHuPaiUser[i]] = WIK_CHI_HU;
					m_GameClientView.PlayChiHuFlash(byOtherViewChairID);
				}
				
				// 播放胡牌声音
				for (BYTE i=0; i<GAME_PLAYER;i++)
				{
					if (i==byMeChairID)
						continue;

					const tagUserData *pOtherUserData = GetUserData(bySaveHuPaiUser[0]);//用户信息
					if (NULL != pOtherUserData  &&  CHK_NULL != pGameEnd->wChiHuKind[i])
					{
						PlayGameSoundLanguage(pUserData->cbGender, TEXT("chihu"), 2);
						break;
					}
				}
				
				//if(byMeChairID==m_GameScoreInfo.wProvideUser)
				//{//本人放炮（别人胡了）
				//	for (int i=0; i<iChiHuNum; ++i)
				//	{//别人的胡牌动画
				//		BYTE byOtherViewChairID = SwitchViewChairID(bySaveHuPaiUser[i]);
				//		m_GameClientView.SetUserAction(byOtherViewChairID,WIK_CHI_HU);
				//		m_byUserAction[bySaveHuPaiUser[i]] = WIK_CHI_HU;
				//		m_GameClientView.PlayChiHuFlash(byOtherViewChairID);
				//	}

				//	//自己的动画
				//	//MessageBox("别人胡");
				//	m_GameClientView.PlayFangPaoFlash(byViewChairID);

				//	//声音
				//	PlaySound("fangpao",false);
				//	for (BYTE i=0; i<GAME_PLAYER;i++)
				//	{
				//		if (i==byMeChairID)
				//			continue;

				//		const tagUserData *pOtherUserData = GetUserData(bySaveHuPaiUser[0]);//用户信息
				//		if (NULL != pOtherUserData)
				//		{
				//			PlayGameSoundLanguage(pUserData->cbGender,TEXT("chihu"),2);
				//			break;
				//		}
				//	}
				//}
				//else
				//{
				//	if (pGameEnd->wChiHuKind[byMeChairID]!=CHK_NULL) 
				//	{//自己胡牌了
				//		//MessageBox("自己胡");

				//		for (int i=0; i<iChiHuNum; ++i)
				//		{//自己和别人的胡牌动画
				//			BYTE byOtherViewChairID = SwitchViewChairID(bySaveHuPaiUser[i]);
				//			m_GameClientView.SetUserAction(byOtherViewChairID,WIK_CHI_HU);
				//			m_byUserAction[bySaveHuPaiUser[i]] = WIK_CHI_HU;
				//			m_GameClientView.PlayChiHuFlash(byOtherViewChairID);
				//		}

				//		//放炮方的动画
				//		m_GameClientView.PlayFangPaoFlash(SwitchViewChairID(m_GameScoreInfo.wProvideUser));

				//		//声音
				//		PlaySound("fangpao",false);
				//		PlayGameSoundLanguage(pUserData->cbGender,TEXT("chihu"), 2);
				//	}
				//	else
				//	{//自己没有胡牌.
				//		for (int i=0; i<iChiHuNum; ++i)
				//		{
				//			//动画
				//			BYTE byOtherViewChairID = SwitchViewChairID(bySaveHuPaiUser[i]);
				//			m_GameClientView.SetUserAction(byOtherViewChairID,WIK_CHI_HU);
				//			m_byUserAction[bySaveHuPaiUser[i]] = WIK_CHI_HU;
				//			m_GameClientView.PlayChiHuFlash(byOtherViewChairID);
				//		}

				//		//放炮方的动画
				//		m_GameClientView.PlayFangPaoFlash(SwitchViewChairID(m_GameScoreInfo.wProvideUser));

				//		//声音
				//		const tagUserData *pOtherUserData = GetUserData(bySaveHuPaiUser[0]);//用户信息
				//		PlaySound("fangpao",false);
				//		PlayGameSoundLanguage(pOtherUserData->cbGender,TEXT("chihu"),2);
				//	}
				//}
			}
		}
	}
	else 
	{//海底荒庄
		DeductionTableCard(true);
		m_GameClientView.SetHaiDiCard(pGameEnd->cbHaiDiCard);
		m_GameClientView.m_bIsHuangZhuang=true;
	}


	//设置扑克
	m_GameClientView.m_UserCard[0].SetCardData(0,false);
	m_GameClientView.m_UserCard[1].SetCardData(0,false);
	m_GameClientView.m_UserCard[2].SetCardData(0,false);
	m_GameClientView.m_HandCardControl.SetCardData(NULL,0,0/*,NULL*/);


	//桌面扑克
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		WORD wViewChairID=SwitchViewChairID(i);
		m_GameClientView.m_TableCard[wViewChairID].SetCardData(pGameEnd->cbCardData[i],pGameEnd->cbCardCount[i]);

		if (1==pGameEnd->iChiHuKind[i]  ||  2==pGameEnd->iChiHuKind[i])
		{
			m_GameClientView.m_TableCard[wViewChairID].SetWinCardData(pGameEnd->cbChiHuCard);
		}
	}

	m_GameClientView.m_btAutoPlayOn.ShowWindow(SW_HIDE);
	m_GameClientView.m_btAutoPlayOff.ShowWindow(SW_HIDE);
	m_GameClientView.m_btStart.ShowWindow(SW_HIDE);

	//设置旁观界面
	if (IsLookonMode()==true)
	{
		PlayWaitFlash(false);
		//m_ScoreView.ShowWindow(SW_SHOW);
		SafeShowOrHideWnd(&m_ScoreView, SW_SHOW);

		if (pGameEnd->cbChiHuCard == 0)
		{
			//m_ScoreView.ShowWindow(SW_HIDE);
			SafeShowOrHideWnd(&m_ScoreView, SW_HIDE);
		}
	}

	//设置界面
	if (IsLookonMode()==false)
	{
		const tagUserData *pUserData = GetUserData(GetMeChairID());//用户信息
		if (pGameEnd->bIsHuangzhuang)
		{//荒庄
			//m_ScoreView.ShowWindow(SW_HIDE);
			SafeShowOrHideWnd(&m_ScoreView, SW_HIDE);
			SetTimer(IDI_HUANGZHUANG,1500,NULL);
			SetGameTimer(GetMeChairID(),IDI_START_GAME,TIME_START_GAME);
			m_GameClientView.m_btStart.ShowWindow(SW_SHOW);
			PlaySound("liuju", false);
			//PlayGameSoundLanguage(pUserData->cbGender,TEXT("liuju"),false);
			m_GameClientView.PlayChiHuFlash(SwitchViewChairID(pUserData->wChairID));
		}
		else
		{//延时3.5秒显示结算框
			if (bIsLeft)
			{
				//MessageBox("逃跑胜利");
				PlaySound("GAME_WIN",false);
				SetTimer(IDI_GAME_END,TIME_GAME_END-1500,NULL);
			}
			else
			{
				SetTimer(IDI_GAME_END,TIME_GAME_END,NULL);
			}
			m_GameClientView.m_btStart.ShowWindow(SW_HIDE);
		}
	}

	//设置托管,听牌标识
	m_bStustee = false;
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		m_GameClientView.m_bTrustee[i]=false;
		m_GameClientView.m_bTingFlag[i] = false;
	}

	//if(m_bStustee)
	//{
	//	OnStusteeControl(0,0);
	//}

	//更新界面
	m_GameClientView.UpdateGameView(NULL);

	return true;
}

////播放声音
//void CGameClientDlg::PlayCardSound(BYTE cbGender, BYTE cbCardData)
//{
//	//变量定义
//	TCHAR szSoundName[16]=TEXT("");
//	TCHAR szKind[]={TEXT('W'),TEXT('S'),TEXT('T'),TEXT('F')};
//
//	//构造名字
//	BYTE cbValue=(cbCardData&MASK_VALUE);
//	BYTE cbColor=(cbCardData&MASK_COLOR)>>4;
//	_snprintf(szSoundName,CountArray(szSoundName),TEXT("%c_%d"),szKind[cbColor],cbValue);
//
//	//播放声音
//	PlayGameSoundLanguage(cbGender,szSoundName);
//
//	return;
//}


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
	CString strBuffer;
	strBuffer.Format("client:CGameClientDlg::PlayActionSound cbGender%d, cbAction%d",cbGender, cbAction);
	OutputDebugString(strBuffer);

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
			//PlayGameSoundLanguage(cbGender,TEXT("chi"));
			break;
		}
	case WIK_PENG:		//碰牌
		{
			PlayGameSoundLanguage(cbGender,TEXT("peng"), 3);
			break;
		}
	case WIK_FILL:	//补牌
	case WIK_GANG:	
		{
			PlayGameSoundLanguage(cbGender,TEXT("SupplyCard"));
			break;
		}
		//case WIK_TING:		//听牌状态（杠牌显示）
		//	{

		//		PlayGameSoundLanguage(cbGender,TEXT("gang"));
		//		break;
		//	}
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
		BYTE cbHeapCount=m_cbHeapCardInfo[m_wHeapHead][0]+m_cbHeapCardInfo[m_wHeapHead][1];
		if (cbHeapCount==HEAP_FULL_COUNT) 
			m_wHeapHead=(m_wHeapHead+1)%CountArray(m_cbHeapCardInfo);

		//减少扑克
		m_cbLeftCardCount--;
		m_cbHeapCardInfo[m_wHeapHead][0]++;

		//堆立扑克
		WORD wHeapViewID=SwitchViewChairID(m_wHeapHead);
		WORD wMinusHeadCount=m_cbHeapCardInfo[m_wHeapHead][0];
		WORD wMinusLastCount=m_cbHeapCardInfo[m_wHeapHead][1];
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
	if (cbUserAction==WIK_TING)
	{
		return true;
	}
	//显示界面
	tagGangCardResult bucardresult;
	::memset(&bucardresult, 0, sizeof(tagGangCardResult));
	::memcpy(bucardresult.cbCardData, m_GameStartData.byBuCard, sizeof(BYTE)*m_GameStartData.byBuCardNums);
	bucardresult.cbCardCount = m_GameStartData.byBuCardNums;
	//m_GameClientView.m_ControlWnd.SetControlInfo(cbStartData,cbUserAction,bucardresult);
	//m_GameClientView.m_ControlWnd.AddGangCardData(m_GameStartData.byGangCard, m_GameStartData.byGangCardNums);
	m_byCanSelectCount = m_GameStartData.byBuCardNums;
	memcpy(m_byCanSelectCard, m_GameStartData.byBuCard, sizeof(m_byCanSelectCard));
	m_GameClientView.m_ControlWnd.SetAction(cbStartData, cbUserAction);

	return true;
}


//时间消息
void CGameClientDlg::OnTimer(UINT nIDEvent)
{
	__super::OnTimer(nIDEvent);

	//方位动画
	if (nIDEvent==IDI_SHOW_DIRECT)
	{
		//环境处理
		KillTimer(IDI_SHOW_DIRECT);
		m_GameClientView.PlayDirectFlash();
		PlaySound("EffectDirect", false);
		//PlaySound("GAME_START",false);
		return;
	}

	//骰子定时器
	else if (nIDEvent==IDI_SHOW_SZ)
	{
		//环境处理
		KillTimer(IDI_SHOW_SZ);
		m_GameClientView.SetDiceEffect(true);
		PlaySound("dice", false);
		return;
	}

	//结束定时器
	else if (nIDEvent==IDI_SOUND)
	{
		KillTimer(IDI_SOUND);
		PlayGameSoundLanguage(m_cbGender,m_InfoSound);
		return;
	}

	//结束定时器
	else if (nIDEvent==IDI_HUANGZHUANG)
	{
		KillTimer(IDI_HUANGZHUANG);
		m_GameClientView.m_btStart.ShowWindow(SW_SHOW);
		return;
	}

	//结束定时器
	else if (nIDEvent==IDI_GAME_END)
	{
		KillGameTimer(IDI_START_GAME);
		KillTimer(IDI_GAME_END);
		PlayWaitFlash(false);
		//m_ScoreView.ShowWindow(SW_SHOW);
		SafeShowOrHideWnd(&m_ScoreView, SW_SHOW);
		m_ScoreView.SetCountDown(true);
		return;
	}
}

//开始按钮
LRESULT CGameClientDlg::OnStart(WPARAM wParam, LPARAM lParam)
{
	//m_GameClientView.m_tDice=1;
	//m_GameClientView.SetDiceEffect(true);
	//m_GameClientView.SetStartEffect(true);

	//SetTimer(IDI_SHOW_SZ,TIME_SHOW_SZ,NULL);

	////智能提示
	//BYTE cbCardData[MAX_INDEX]={0x12,0x18,0x27,0x27,0x27,0x03,0x04,0x05,0x11,0x12,0x13,0x16,0x17};
	//m_GameClientView.m_HandCardControl.SetCardData(cbCardData,13,0x29,0x01);
	//m_GameClientView.m_HandCardControl.SetDisplayItem(true);
	//m_GameClientView.m_HandCardControl.SetPositively(true);

	//BYTE byTempCardIndex[MAX_INDEX]={0};
	//for (int i=0;i<MAX_INDEX; i++)
	//{
	//	if (cbCardData[i] > 0)
	//	{
	//		byTempCardIndex[m_GameLogic.SwitchToCardIndex(cbCardData[i])]++;
	//	}		
	//}
	//::memcpy(m_cbCardIndex, byTempCardIndex, MAX_INDEX);

	//m_GameClientView.m_btStart.ShowWindow(SW_HIDE);
	//m_GameClientView.UpdateGameView(NULL);
	//return 0;


	//m_GameClientView.PlayOperationFlash(2);
	//return 0;

	//m_GameClientView.m_wChiHuType=1;
	//m_GameClientView.m_wChiHuKind[1]=0x0001;
	//   m_GameClientView.m_wChiHuRight[1]=0x0400;
	//m_GameClientView.PlayChiHuFlash(1);


	////显示界面
	//BYTE byMask = WIK_NULL;
	////byMask = WIK_CHI_HU;
	////byMask |= WIK_PENG;
	//byMask |= WIK_LEFT;


	//tagGangCardResult bucardresult;
	//BYTE cbCardData[4]={0x11, 0x11, 0x11,0x11 };
	//::memset(&bucardresult, 0, sizeof(tagGangCardResult));
	//memcpy(bucardresult.cbCardData,cbCardData,sizeof(cbCardData));
	//bucardresult.cbCardCount =0;
	//m_GameClientView.m_ControlWnd.SetControlInfo(0x12,byMask,bucardresult);
	//m_GameClientView.m_ControlWnd.AddGangCardData(NULL, 0);
	//m_GameClientView.m_ControlWnd.ShowWindow(SW_SHOW);

	//   m_GameClientView.m_wChiHuType=1;
	//m_GameClientView.PlayFangPaoFlash(0);
	//m_GameClientView.PlayChiHuFlash(1);
	//return 0;

	//环境设置
	CThreadAnimationManager::FreeAll();

	KillGameTimer(IDI_START_GAME);
	m_GameClientView.m_btStart.ShowWindow(SW_HIDE);
	//m_GameClientView.m_ControlWnd.ShowWindow(SW_HIDE);
	SafeShowOrHideWnd(&m_GameClientView.m_ControlWnd, SW_HIDE);

	//设置界面
	//m_ScoreView.ShowWindow(SW_HIDE);
	SafeShowOrHideWnd(&m_ScoreView, SW_HIDE);
	m_GameClientView.SetHaiDiCard(0);
	//m_GameClientView.SetStatusFlag(false,false);
	m_GameClientView.SetBankerUser(INVALID_CHAIR);
	m_GameClientView.SetUserAction(INVALID_CHAIR,0);
	m_GameClientView.SetOutCardInfo(INVALID_CHAIR,0);

	//动作设置
	memset(m_byUserAction, 0 , sizeof(m_byUserAction));

	//扑克设置
	m_GameClientView.m_UserCard[0].SetCardData(0,false);
	m_GameClientView.m_UserCard[1].SetCardData(0,false);
	m_GameClientView.m_UserCard[2].SetCardData(0,false);
	m_GameClientView.m_HandCardControl.SetCardData(NULL,0,0/*,NULL*/);

	//扑克设置
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		m_GameClientView.m_TableCard[i].SetCardData(NULL,0);
		m_GameClientView.m_TableCard[i].SetWinCardData(0);
		m_GameClientView.m_DiscardCard[i].SetCardData(NULL,0);
		m_GameClientView.m_WeaveCard[i][0].SetCardData(NULL,0);
		m_GameClientView.m_WeaveCard[i][1].SetCardData(NULL,0);
		m_GameClientView.m_WeaveCard[i][2].SetCardData(NULL,0);
		m_GameClientView.m_WeaveCard[i][3].SetCardData(NULL,0);
		m_GameClientView.m_HeapCard[i].SetCardData(0,0,0);
	}

	////堆立扑克
	//for (WORD i=0;i<GAME_PLAYER;i++)
	//{
	//	m_cbHeapCardInfo[i][0]=0;
	//	m_cbHeapCardInfo[i][1]=HEAP_FULL_COUNT-(((i==m_wBankerUser)||((i+2)%4==m_wBankerUser))?28:26);
	//	m_GameClientView.m_HeapCard[SwitchViewChairID(i)].SetCardData(m_cbHeapCardInfo[i][0],m_cbHeapCardInfo[i][1],HEAP_FULL_COUNT);
	//}

	//状态变量
	//m_bHearStatus=false;

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
	m_wHeapHead=0;
	m_wHeapTail=0;
	ZeroMemory(m_cbHeapCardInfo,sizeof(m_cbHeapCardInfo));

	//扑克变量
	m_cbLeftCardCount=0;
	ZeroMemory(m_cbCardIndex,sizeof(m_cbCardIndex));

	m_byCanSelectType = WIK_NULL;
	m_byCanSelectCount = 0;
	ZeroMemory(m_byCanSelectCard,sizeof(m_byCanSelectCard));

	//发送消息
	SendUserReady(NULL,0);

	int iCount = GetCurGameUserNums();
	PlayWaitFlash(false);
	if (GAME_PLAYER != iCount)
	{
		PlayWaitFlash(true);
	}

	m_GameClientView.UpdateGameView(NULL);
	return 0;
}
//点击启动筛子动画后
LRESULT CGameClientDlg::OnSZStart(WPARAM wParam, LPARAM lParam)
{
	m_GameClientView.PlaySZFlashBT(false);
	KillGameTimer(IDI_SZSTART);
	SetGameTimer(m_wBankerUser,IDI_SZSTART,5);
	BYTE byMeChairID = GetMeChairID();
	if (byMeChairID == m_wBankerUser)
	{
		m_GameClientView.PlaySZFlashBT(true);
	}

	if (m_wBankerUser == GetMeChairID())
	{
		const tagUserData *pUserData = GetUserData(m_wBankerUser);//用户信息
		PlayGameSoundLanguage(pUserData->cbGender,TEXT("Banker"));
	}

	//char cbData[128]={0};
	//sprintf(cbData,"CGameClientDlg::OnSZStart-->庄家:%d 本人:%d", m_wBankerUser, byMeChairID);
	//InsertSystemString(cbData);

	m_GameClientView.UpdateGameView(NULL);
	return 0;
}

//摸牌动画播完后
LRESULT CGameClientDlg::OnMoPai(WPARAM wParam, LPARAM lParam)
{
	bool bPlayerMode=IsLookonMode();
	m_GameClientView.m_btStart.ShowWindow(SW_HIDE);
	SafeShowOrHideWnd(&m_ScoreView, SW_HIDE);
	memset(m_byUserAction, 0 , sizeof(m_byUserAction));

	// 托管按钮隐藏
	if(bPlayerMode)
	{
		m_GameClientView.m_btAutoPlayOn.ShowWindow(SW_HIDE);
		m_GameClientView.m_btAutoPlayOff.ShowWindow(SW_HIDE);
		m_GameClientView.SetHaiDiCard(0);
		//m_GameClientView.SetStatusFlag(false,false);
		m_GameClientView.SetUserAction(INVALID_CHAIR,0);
		m_GameClientView.SetOutCardInfo(INVALID_CHAIR,0);	
	}
	else
	{
		m_GameClientView.m_btAutoPlayOn.ShowWindow(SW_SHOW);
		m_GameClientView.m_btAutoPlayOff.ShowWindow(SW_HIDE);
	}

	//堆立扑克
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		m_cbHeapCardInfo[i][0]=0;
		m_cbHeapCardInfo[i][1]=HEAP_FULL_COUNT-(((i==m_wBankerUser)||((i+2)%4==m_wBankerUser))?28:26);
	}

	//分发扑克
	BYTE cbTakeCount=(MAX_COUNT-1)*GAME_PLAYER+1;    //53张
	BYTE cbSiceFirst=HIBYTE(m_GameStartData.wSiceCount);
	BYTE cbSiceSecond=LOBYTE(m_GameStartData.wSiceCount);
	BYTE cbSiceCount=__min(cbSiceFirst,cbSiceSecond);
	WORD wTakeChairID=(m_wBankerUser-cbSiceFirst-cbSiceSecond+1+GAME_PLAYER*GAME_PLAYER)%GAME_PLAYER;
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		//计算数目
		BYTE cbValidCount=HEAP_FULL_COUNT-m_cbHeapCardInfo[wTakeChairID][1]-((i==0)?cbSiceCount*2:0);
		BYTE cbRemoveCount=__min(cbValidCount,cbTakeCount);

		//提取扑克
		cbTakeCount-=cbRemoveCount;
		m_cbHeapCardInfo[wTakeChairID][(i==0)?1:0]+=cbRemoveCount;

		//完成判断
		if (0==cbTakeCount)
		{
			m_wHeapHead=wTakeChairID;
			m_wHeapTail=(m_wBankerUser-cbSiceFirst-cbSiceSecond+1+GAME_PLAYER*GAME_PLAYER/*+7-cbSiceFirst*/)%GAME_PLAYER;
			break;
		}

		//切换索引
		wTakeChairID=(wTakeChairID+1)%GAME_PLAYER;
	}

	//绘画王牌
	//m_cbTempKing[0]=m_GameStartData.cbKingCard;
	m_GameClientView.m_HandCardControl.SetCardData(NULL,0,0/*,NULL*/);

	//扑克设置
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		//变量定义
		WORD wViewChairID=SwitchViewChairID(i);

		m_GameClientView.m_TableCard[wViewChairID].SetCardData(NULL,0);
		m_GameClientView.m_DiscardCard[wViewChairID].SetCardData(NULL,0);
		m_GameClientView.m_WeaveCard[wViewChairID][0].SetCardData(NULL,0);
		m_GameClientView.m_WeaveCard[wViewChairID][1].SetCardData(NULL,0);
		m_GameClientView.m_WeaveCard[wViewChairID][2].SetCardData(NULL,0);
		m_GameClientView.m_WeaveCard[wViewChairID][3].SetCardData(NULL,0);

		//组合界面
		m_GameClientView.m_WeaveCard[i][0].SetDisplayItem(true);
		m_GameClientView.m_WeaveCard[i][1].SetDisplayItem(true);
		m_GameClientView.m_WeaveCard[i][2].SetDisplayItem(true);
		m_GameClientView.m_WeaveCard[i][3].SetDisplayItem(true);
		m_GameClientView.m_HeapCard[wViewChairID].SetCardData(m_cbHeapCardInfo[i][0],m_cbHeapCardInfo[i][1],HEAP_FULL_COUNT);

		if (i == GetMeChairID())
		{
			BYTE cbBankerCard = 0;
			if (i == m_wBankerUser)
			{
				cbBankerCard=m_GameStartData.cbCardData[MAX_COUNT-1];
				tagCardItem CardItem;
				CardItem.cbCardData = cbBankerCard;
				CardItem.bShoot = false;
				CardItem.bKingFlag = false;//(cbBankerCard == m_cbTempKing[0])?true:false;
				m_GameClientView.m_HandCardControl.SetCurrentCard(CardItem);
			}
			m_GameClientView.m_HandCardControl.SetCardData(m_GameStartData.cbCardData,MAX_COUNT-1,cbBankerCard/*,m_cbTempKing[0]*/);
		}
		else
		{
			WORD wIndex=(wViewChairID>=3)?2:wViewChairID;
			m_GameClientView.m_UserCard[wIndex].SetCardData(MAX_COUNT-1,(i==m_wBankerUser));
		}

		////用户扑克
		//if (wViewChairID!=2)
		//{
		//	WORD wIndex=(wViewChairID>=3)?2:wViewChairID;
		//	m_GameClientView.m_UserCard[wIndex].SetCardData(MAX_COUNT-1,(i==m_wBankerUser));
		//}
		//else
		//{
		//	BYTE cbBankerCard=(i==m_wBankerUser)?m_GameStartData.cbCardData[MAX_COUNT-1]:0;
		//	tagCardItem CardItem;
		//	CardItem.cbCardData = cbBankerCard;
		//	CardItem.bShoot = false;
		//	CardItem.bKingFlag = (cbBankerCard == m_cbTempKing[0])?true:false;
		//	m_GameClientView.m_HandCardControl.SetCurrentCard(CardItem);
		//	m_GameClientView.m_HandCardControl.SetCardData(m_GameStartData.cbCardData,MAX_COUNT-1,cbBankerCard,m_cbTempKing[0]);
		//}
	}

	////出牌提示
	//if ((bPlayerMode==false)&&(m_wCurrentUser!=INVALID_CHAIR))
	//{
	//	WORD wMeChairID=GetMeChairID();
	//	if (m_wCurrentUser==wMeChairID)
	//	{
	//		m_GameClientView.SetStatusFlag(true,false);
	//	}
	//}

	////动作处理
	//if ((bPlayerMode==false)&&(m_GameStartData.cbUserAction!=WIK_NULL))
	//{
	//	ShowOperateControl(m_GameStartData.cbUserAction);
	//	SetGameTimer(GetMeChairID(),IDI_OPERATE_CARD,TIME_OPERATE_CARD);
	//}

	m_bIsMeCanOutCard = false;
	m_wCurrentUser = INVALID_CHAIR;
	m_byUserAction[GetMeChairID()] = WIK_NULL;
	WORD wMeChairID=GetMeChairID();
	if (!bPlayerMode && m_wBankerUser==wMeChairID)
	{
		m_wCurrentUser =  wMeChairID; 
		//m_GameClientView.SetStatusFlag(true,false);
		if (WIK_NULL != m_GameStartData.cbUserAction)
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
	m_bStustee = false;
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

//骰子动画
LRESULT CGameClientDlg::OnShowSZ(WPARAM wParam, LPARAM lParam)
{
	m_GameClientView.m_nDiceCount1 = wParam;
	m_GameClientView.m_nDiceCount2 = lParam;

	//环境处理
	m_GameClientView.SetDiceEffect(true);

	return 0;
}


////听牌操作
//bool CGameClientDlg::OnSubTingCard(const void * pBuffer, WORD wDataSize)
//{
//	//效验数据
//	ASSERT(wDataSize==sizeof(CMD_S_GameTingCard));
//	if (wDataSize!=sizeof(CMD_S_GameTingCard)) return false;
//
//	KillGameTimer(IDI_USER_TING_CARD);
//	//消息处理
//	CMD_S_GameTingCard * pGameTingCard=(CMD_S_GameTingCard *)pBuffer;
//	if (!IsLookonMode() && NULL!=pGameTingCard && pGameTingCard->wTingCardUser == GetMeChairID())
//	{
//		//m_bHearStatus = false;
//		//m_GameClientView.m_btTingCard.ShowWindow(SW_SHOW);	
//		//m_GameClientView.m_btBuTing.ShowWindow(SW_SHOW);
//		m_wCurrentUser = pGameTingCard->wTingCardUser;
//		//设置
//		SetGameTimer(m_wCurrentUser, IDI_USER_TING_CARD, TIME_USERTINGCARD);
//	}
//
//	return true;
//
//}

//听牌结果
//bool CGameClientDlg::OnSubTingCardResult(const void * pBuffer, WORD wDataSize)
//{
//	//效验数据
//	ASSERT(wDataSize==sizeof(CMD_S_GameTingCard));
//	if (wDataSize!=sizeof(CMD_S_GameTingCard)) return false;
//
//	//消息处理
//	CMD_S_GameTingCard * pGameTingCard=(CMD_S_GameTingCard *)pBuffer;
//	if (NULL != pGameTingCard)
//	{
//		WORD byTingUserViewID  = SwitchViewChairID(pGameTingCard->wTingCardUser);
//		m_GameClientView.m_bTingFlag[byTingUserViewID] = true;
//		//m_GameClientView.PlayTingCardFlash(byTingUserViewID);
//		if (!IsLookonMode() && (pGameTingCard->wTingCardUser == GetMeChairID()))
//		{
//			//m_bHearStatus = true;
//			KillGameTimer(IDI_USER_TING_CARD);
//			//m_GameClientView.m_btTingCard.ShowWindow(SW_HIDE);	
//			//m_GameClientView.m_btBuTing.ShowWindow(SW_HIDE);
//		}
//	}
//	m_GameClientView.UpdateGameView(NULL);
//	return true;
//}

//听牌按钮
LRESULT CGameClientDlg::OnTingCard(WPARAM wParam, LPARAM lParam)
{
	KillGameTimer(IDI_USER_TING_CARD);
	//m_GameClientView.m_btTingCard.ShowWindow(SW_HIDE);
	//m_GameClientView.m_btBuTing.ShowWindow(SW_HIDE);
	BYTE byTingTag = (BYTE)wParam;
	CMD_UserTingOperate TingOperate;
	//设置状态
	if (1 == byTingTag)
	{//听牌
		TingOperate.bTingCard = true;
	}
	else
	{//不听
		TingOperate.bTingCard = false;
	}
	SendData(SUB_C_USERTINGCARD,&TingOperate,sizeof(TingOperate));
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

	CString strBuffer;
	strBuffer.Format("client:CGameClientDlg::OnOutCard ");
	OutputDebugString(strBuffer);

	//听牌判断
	//if ((m_bHearStatus==true)&&(m_GameClientView.m_HandCardControl.GetCurrentCard()!=(BYTE)wParam)) return 0;

	//王牌判断
	//if(m_cbTempKing == (BYTE)wParam) return 0;

	//设置变量
	m_wCurrentUser=INVALID_CHAIR;
	BYTE cbOutCardData=(BYTE)wParam;
	//m_GameLogic.RemoveCard(m_cbCardIndex,cbOutCardData);

	//设置扑克
	//BYTE cbCardData[MAX_COUNT];
	//BYTE cbCardCount=m_GameLogic.SwitchToCardData(m_cbCardIndex,cbCardData);
	//m_GameClientView.m_HandCardControl.SetCardData(cbCardData,cbCardCount,0,m_cbTempKing[0]);

	//设置界面
	KillGameTimer(IDI_OPERATE_CARD);
	KillGameTimer(IDI_OTHER_USEROUTCARD);

	m_GameClientView.UpdateGameView(NULL);
	//m_GameClientView.SetStatusFlag(false,false);
	m_GameClientView.SetUserAction(INVALID_CHAIR,0);
	//m_GameClientView.SetOutCardInfo(2,cbOutCardData);
	//m_GameClientView.m_ControlWnd.ShowWindow(SW_HIDE);
	SafeShowOrHideWnd(&m_GameClientView.m_ControlWnd, SW_HIDE);

	//动作设置
	memset(m_byUserAction, 0 , sizeof(m_byUserAction));

	//播放声音
	//PlayCardSound(cbOutCardData);


	//发送数据
	CMD_C_OutCard OutCard;
	OutCard.cbCardData=cbOutCardData;
	SendData(SUB_C_OUT_CARD,&OutCard,sizeof(OutCard));
	return 0;

}

//扑克操作
LRESULT CGameClientDlg::OnCardOperate(WPARAM wParam, LPARAM lParam)
{
	KillGameTimer(IDI_OPERATE_CARD);
	KillGameTimer(IDI_OTHER_USEROUTCARD);

	//变量定义
	BYTE cbOperateCode=(BYTE)(wParam);
	BYTE cbOperateCard=(BYTE)(LOWORD(lParam));

	//m_GameClientView.SetStatusFlag(false,true);
	//m_GameClientView.m_ControlWnd.ShowWindow(SW_HIDE);
	SafeShowOrHideWnd(&m_GameClientView.m_ControlWnd, SW_HIDE);

	if (m_wCurrentUser!=GetMeChairID())
	{
		return 0;
	}

	CString strBuffer;
	strBuffer.Format("client:CGameClientDlg::OnCardOperate cbOperateCode %d, cbOperateCard %d", cbOperateCode,cbOperateCard);
	OutputDebugString(strBuffer);

	// 碰，补有选择时应该提供玩家选择
	if (WIK_PENG==cbOperateCode  ||  WIK_FILL==cbOperateCode ||  WIK_GANG==cbOperateCode)
	{
		//BYTE bTmpHandCard[MAX_COUNT]={0};
		//CopyMemory(bTmpHandCard, m_cbCardIndex, sizeof(bTmpHandCard));
		m_byCanSelectType = cbOperateCode;

		//// 不是出牌玩家，且自己也不是刚操作的玩家
		//if (m_wOutCardUser != GetMeChairID()  &&  INVALID_CHAIR!=m_wOutCardUser)
		//{
		//	bTmpHandCard[m_GameLogic.SwitchToCardIndex(cbOperateCard)]++;
		//}

		//for(BYTE i=0; i<m_cbWeaveCount[GetMeChairID()]; i++)
		//{
		//	if (WIK_PENG == m_WeaveItemArray[GetMeChairID()][i].cbWeaveKind)
		//	{
		//		// 计算可杠时，已经碰过的牌也应该算进来
		//		BYTE bCardIndex = m_GameLogic.SwitchToCardIndex(m_WeaveItemArray[GetMeChairID()][i].cbCenterCard);
		//		bTmpHandCard[bCardIndex] +=3;
		//	}
		//}

		//m_GameLogic.AnalyseGangPengCountCard(bTmpHandCard, cbOperateCode, cbOperateCard, m_byCanSelectCard, m_byCanSelectCount);
		cbOperateCard = m_byCanSelectCard[0];

		CString strBuffer;
		strBuffer.Format("client: CGameClientDlg::OnCardOperate  cbOperateCode=%d, count=%d", cbOperateCode, m_byCanSelectCount);
		OutputDebugString(strBuffer);

		if (1 < m_byCanSelectCount)
		{
			m_GameClientView.m_HandCardControl.SetSelectStatus(true, m_byCanSelectCard, m_byCanSelectCount);
			m_GameClientView.UpdateGameView(NULL);
			SetGameTimer(GetMeChairID(), IDI_SELECT_OUT_CARD, TIME_SELECT_OUT_CARD);
			return 0;
		}
	}

	SendCardCommand(cbOperateCode, cbOperateCard);	
	return 0;
}

////海底操作
//LRESULT CGameClientDlg::OnOperateHaiDi(WPARAM wParam, LPARAM lParam)
//{
//	//界面设置
//	KillGameTimer(TIME_OPERATE_HAI_DI);
//	//m_GameClientView.m_btHaiDi.ShowWindow(SW_HIDE);
//	//m_GameClientView.m_btNoHaiDi.ShowWindow(SW_HIDE);
//	//m_GameClientView.m_btStart.ShowWindow(SW_SHOW);
//
//
//	//构造消息
//	CMD_C_OperateHaiDi OperateHaiDi;
//	OperateHaiDi.cbTakeHaiDi=(BYTE)wParam;
//	SendData(SUB_C_OPERATE_HAI_DI,&OperateHaiDi,sizeof(OperateHaiDi));
//
//	return 0;
//}

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
		TCHAR szBuffer[256]={0};
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
void CGameClientDlg::PlayGameSoundLanguage(BYTE cbGender, CString strSound, BYTE bSoundType)
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

	if (1 < bSoundType)
	{//多种声音
		TCHAR cBuffer[10]={""};
		int iRand=rand()%bSoundType+1;
		::_snprintf(cBuffer,sizeof(cBuffer),"%d",iRand);
		strGameSound=strGameSound+TEXT(cBuffer);
	}

	CString strBuffer;
	strBuffer.Format("client:CGameClientDlg::PlayGameSoundLanguage path%s",strGameSound);
	OutputDebugString(strBuffer);
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
		TCHAR szResourcePath[MAX_PATH]={0};
		GetCurrentDirectory(MAX_PATH,szResourcePath);
		CString strPath(szResourcePath);
		strPath.Append("\\SparrowZZ\\Sound\\");
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
		TCHAR szResourcePath[MAX_PATH]={0};
		GetCurrentDirectory(MAX_PATH,szResourcePath);
		CString strPath(szResourcePath);
		strPath.Append("\\SparrowZZ\\Sound\\");
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
			//智能提示
			BYTE cbHuCardData[MAX_INDEX]={0};
			BYTE byTempCardIndex[MAX_INDEX]={0};
			::memcpy(byTempCardIndex, m_cbCardIndex, MAX_INDEX);
			m_GameLogic.RemoveCard(byTempCardIndex,byCardData);
			//m_cbTempKing[0]=0x01;
			//m_GameLogic.IsCanTingCard(byTempCardIndex,0,0,m_cbTempKing);
			//m_GameLogic.IsCanTingCard(byTempCardIndex,m_WeaveItemArray[m_wCurrentUser],m_cbWeaveCount[m_wCurrentUser],m_cbTempKing);
			int iCardNums = m_GameLogic.GetHuPaiData(cbHuCardData,MAX_INDEX);

			//int iCardNums = 1;

			if (iCardNums>0)
			{
				m_GameClientView.SetCanHuPaiData(cbHuCardData, iCardNums, iX);
				//char cbData[128]={0};
				//sprintf(cbData,"%x,%x,%x,%x,%x,%d",cbHuCardData[0],cbHuCardData[1],cbHuCardData[2],cbHuCardData[3],cbHuCardData[4],iCardNums);
				//InsertSystemString(cbData);

				//char chDa[32]={0};
				//sprintf(chDa, "----%x", byCardData);
				//InsertSystemString(chDa);
			}
			else
			{
				m_GameClientView.SetCanHuPaiData(NULL, 0, 0);
			}
		}
	}
}


//色子动画按钮
LRESULT CGameClientDlg::OnSZFlashBT(WPARAM wParam, LPARAM lParam)
{
	KillGameTimer(IDI_SZSTART);
	m_GameClientView.PlaySZFlashBT(false);
	SendData(SUB_C_SZSTART);
	return 0;
}
//////////////////////////////////////////////////////////////////////////

//获得当前游戏玩家个数
int CGameClientDlg::GetCurGameUserNums()
{
	int iCount = 0;

	for (int i=0; i<GAME_PLAYER; ++i)
	{
		const tagUserData * pUserData=GetUserData(i);
		if (NULL != pUserData)
		{
			++iCount;
		}
	}

	return iCount;
}

//用户进入
void __cdecl CGameClientDlg::OnEventUserEnter(tagUserData * pUserData, WORD wChairID, bool bLookonUser)
{
	CGameFrameDlg::OnEventUserEnter(pUserData, wChairID, bLookonUser);

	//int iCount = GetCurGameUserNums();
	//if (GAME_PLAYER == iCount)
	//{
	//	PlayWaitFlash(false);
	//}
}
//用户离开
void __cdecl CGameClientDlg::OnEventUserLeave(tagUserData * pUserData, WORD wChairID, bool bLookonUser)
{
	CGameFrameDlg::OnEventUserLeave(pUserData, wChairID, bLookonUser);

	int iCount = GetCurGameUserNums();
	PlayWaitFlash(false);
	if (GAME_PLAYER != iCount)
	{
		for (int i=0;i<GAME_PLAYER;i++)
		{
			m_GameClientView.m_TableCard[i].SetCardData(NULL,0);
			m_GameClientView.m_DiscardCard[i].SetCardData(NULL,0);
			m_GameClientView.m_WeaveCard[i][0].SetCardData(NULL,0);
			m_GameClientView.m_WeaveCard[i][1].SetCardData(NULL,0);
			m_GameClientView.m_WeaveCard[i][2].SetCardData(NULL,0);
			m_GameClientView.m_WeaveCard[i][3].SetCardData(NULL,0);
			m_GameClientView.m_HeapCard[i].SetCardData(0,0,0);
		}		
		PlayWaitFlash(true);
	}
	m_GameClientView.UpdateGameView(NULL);
}