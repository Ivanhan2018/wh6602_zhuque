#include "StdAfx.h"
#include "Resource.h"
#include "GameClientView.h"
#include "GameClientDlg.h"
#include ".\gameclientview.h"
#include <math.h>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//按钮标识 

#define IDC_START						100								//开始按钮
#define IDC_HAI_DI						101								//海底按钮
#define IDC_NO_HAI_DI					102								//放弃按钮
#define IDC_TRUSTEE_CONTROL				104								//托管控制
#define IDC_TING_CARD                   105                             //听牌按钮
#define IDC_NO_TING_CARD				106								//不听按钮
#define IDC_AUTOPLAY_ON                 111                             //托管按钮
#define IDC_AUTOPLAY_OFF                112                             //取消托管
#define IDC_SORT_CARD                   113                             //排序按钮

//动画标识  
#define MP_EFFECT_COUNT				    12							    //摸牌动作
#define DICE_EFFECT_COUNT               9                               //骰子数目
#define START_EFFECT_COUNT              4                               //开始数目

#define IDI_DICE_EFFECT					106							    //骰子标识
#define IDI_MP_EFFECT					107							    //摸牌标识
#define IDI_ACTION_EFFECT				108							    //特效标识
#define IDI_START_EFFECT                109                             //开始标识
#define IDI_SHOW_CARD                   110                             //出牌显示
#define IDI_SZFLASH_EFFECT				120								//启动筛子动画
#define IDI_DIRECT_EFFECT               121                             //方位动画
#define IDI_MUSCLE_GOLD					122							//筋动画



//定时器时间间隔
#define  IDI_TIME_SHOW_CARD_INTEVAL		 500                           //出牌显示
#define  IDI_TIME_SZ_FLASH_INTEVAL		 300						   //筛子启动
#define  IDI_TIME_DIRECT_FLASH_INTERCAL  100                           //方位动画

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CGameClientView, CGameFrameView)
	ON_WM_CREATE()
	ON_WM_SETCURSOR()
	ON_WM_LBUTTONDOWN()
	ON_BN_CLICKED(IDC_START, OnStart)
	//ON_BN_CLICKED(IDC_HAI_DI, OnHaiDi)
	//ON_BN_CLICKED(IDC_NO_HAI_DI, OnNoHaiDi)
	ON_BN_CLICKED(IDC_AUTOPLAY_ON, OnAutoPlayerOn)
	ON_BN_CLICKED(IDC_AUTOPLAY_OFF,OnAutoPlayerOff)
	ON_BN_CLICKED(IDC_TRUSTEE_CONTROL,OnStusteeControl)
	//ON_BN_CLICKED(IDC_TING_CARD,OnTingCard)
	//ON_BN_CLICKED(IDC_NO_TING_CARD, OnNoTingCard)
	ON_WM_TIMER()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CGameClientView::TestClient(int iType)
{
	switch (iType)
	{
	case 1: // 显示组合牌
		{
			// 组合牌数据测试 start
			BYTE cbWCardData11[4]={0x01,0x02,0x03,0x04};
			m_WeaveCard[0][0].SetCardData(cbWCardData11,4);
			m_WeaveCard[1][0].SetCardData(cbWCardData11,4);
			m_WeaveCard[2][0].SetCardData(cbWCardData11,4);
			m_WeaveCard[3][0].SetCardData(cbWCardData11,4);

			m_WeaveCard[0][1].SetCardData(cbWCardData11,4);
			m_WeaveCard[1][1].SetCardData(cbWCardData11,4);
			m_WeaveCard[2][1].SetCardData(cbWCardData11,4);
			m_WeaveCard[3][1].SetCardData(cbWCardData11,4);
			m_WeaveCard[0][1].SetDisplayItem(false);
			m_WeaveCard[1][1].SetDisplayItem(false);
			m_WeaveCard[2][1].SetDisplayItem(false);
			m_WeaveCard[3][1].SetDisplayItem(false);
			//end 组合牌数据测试
		}break;
	case 2:
		{
			//if (NULL != m_pGameClientDlg)
			//{
			//tagGameScoreInfo ScoreInfo;
			//m_pGameClientDlg->m_ScoreView.SetCountDown(true);
			//m_pGameClientDlg->m_ScoreView.SetScoreInfo(ScoreInfo);
			//m_pGameClientDlg->m_ScoreView.ShowWindow(SW_SHOW);
			//}
		}break;
	case 3: // 播放胡牌动画
		{
			m_wChiHuType = 1;
			PlayChiHuFlash(2);
		}break;
	case 4: // 播放筋动画
		{
			static int aa=0;
			LONGLONG llGold[GAME_PLAYER]={0};
			llGold[0] = 3;
			llGold[1] = -1;
			llGold[2] = -1;
			llGold[3] = -1;
			PlayMuscleFlash(0, 0, 1, llGold);
		}break;
	case 5:	// 桌面扑克
		{
			BYTE cbTCardData[14]={0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x11,0x12,0x13,0x14,0x15};
			//BYTE cbTCardData[14]={0x03,0x03,0x06,0x06,0x06,0x12,0x13,0x14,0x27,0x28,0x29};
			m_TableCard[0].SetCardData(cbTCardData,14);
			m_TableCard[1].SetCardData(cbTCardData,14);
			m_TableCard[2].SetCardData(cbTCardData,14);
			m_TableCard[3].SetCardData(cbTCardData,14);	

			static int i=0;
			m_TableCard[0].SetWinCardData(cbTCardData[i]);
			m_TableCard[1].SetWinCardData(cbTCardData[i]);
			m_TableCard[2].SetWinCardData(cbTCardData[i]);
			m_TableCard[3].SetWinCardData(cbTCardData[i]);
			//i++;
			if (i==14)
			{
				i=0;
			}
		}break;
	case 6:	// 堆立扑克
		{
			//m_HeapCard[0].SetCardData(21,3,28);//top
			m_HeapCard[0].SetCardData(3,9,28);//top
			m_HeapCard[1].SetCardData(0,6,26);//right
			m_HeapCard[2].SetCardData(0,19,28);//bottom
			//m_HeapCard[2].SetCardData(0,5,28);//bottom
			//m_HeapCard[2].SetCardData(3,17,28);//bottom
			m_HeapCard[3].SetCardData(1,9,26);//left
			//m_HeapCard[3].SetCardData(1,8,26);//left
			break;
		}
	default: break;
	}	

	//BYTE cbCardData[MAX_INDEX]={0x12,0x18,0x27,0x27,0x27,0x03,0x04,0x05,0x11,0x12,0x13,0x16,0x17};
	//m_HandCardControl.SetCardData(cbCardData,13,0x29,0x01);
	//m_HandCardControl.SetDisplayItem(true);
	//m_HandCardControl.SetPositively(true);

	//m_UserCard[0].SetCardData(mCardCount,true);
	//m_UserCard[1].SetCardData(mCardCount,true);
	//m_UserCard[2].SetCardData(mCardCount,true);

	BYTE cbDCardData[18]={0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x02,0x12,0x11,0x25};
	//m_DiscardCard[0].SetCardData(cbDCardData,7);
	//m_DiscardCard[1].SetCardData(cbDCardData,14);
	//m_DiscardCard[2].SetCardData(cbDCardData,14);
	//m_DiscardCard[3].SetCardData(cbDCardData,18);


	BYTE cbWCardData[3]={0x12,0x01,0x12};
	//m_WeaveCard[0][0].SetCardData(cbWCardData,3);
	//m_WeaveCard[0][1].SetCardData(cbWCardData,3);
	//m_WeaveCard[0][2].SetCardData(cbWCardData,3);
	//m_WeaveCard[0][3].SetCardData(cbWCardData,3);

	//m_WeaveCard[1][0].SetCardData(cbWCardData,3);
	//m_WeaveCard[1][1].SetCardData(cbWCardData,3);
	//m_WeaveCard[1][2].SetCardData(cbWCardData,3);
	//m_WeaveCard[1][3].SetCardData(cbWCardData,3);

	//m_WeaveCard[2][0].SetCardData(cbWCardData,3);
	//m_WeaveCard[2][1].SetCardData(cbWCardData,3);
	//m_WeaveCard[2][2].SetCardData(cbWCardData,3);
	//m_WeaveCard[2][3].SetCardData(cbWCardData,3);

	//m_WeaveCard[3][0].SetCardData(cbWCardData,3);
	//m_WeaveCard[3][1].SetCardData(cbWCardData,3);
	//m_WeaveCard[3][2].SetCardData(cbWCardData,3);
	//m_WeaveCard[3][3].SetCardData(cbWCardData,3);

	//m_btTingCard.ShowWindow(SW_SHOW);
	//m_btBuTing.ShowWindow(SW_SHOW);

	//BYTE cbData[2]={0x21,0x18};
	//m_CardHuPaiList.SetCardData(cbData,2);
	//m_CardHuPaiList.ShowWindow(SW_SHOW);

	/////////////
	//BYTE bCard[4]={0x01,0x01,0x01,0x01};
	//m_WeaveCard[0][0].SetCardData(bCard, 4);
	//m_WeaveCard[0][0].SetDisplayItem(true);

	//m_WeaveCard[1][0].SetCardData(bCard, 3);
	//m_WeaveCard[1][0].SetDisplayItem(true);

	//m_WeaveCard[2][0].SetCardData(bCard, 3);
	//m_WeaveCard[2][0].SetDisplayItem(true);

	//m_WeaveCard[3][0].SetCardData(bCard, 3);
	//m_WeaveCard[3][0].SetDisplayItem(true);
	///////////////////

	UpdateGameView(NULL);
}

//构造函数
CGameClientView::CGameClientView():CGameFrameView(true, 32)
{
	#if _DEBUG
		DebugBreak();	// 测试用
	#endif

	//标志变量
	m_cbHaiDiCard=0;
	//m_bOutCard=false;
	//m_bWaitOther=false;
	m_bIsInitBT=false;
	m_bIsShowOutCardFlash=false;


	m_nDiceCount1=0;
	m_nDiceCount2=0;

	//游戏属性
	m_lCellScore=0L;
	m_wBankerUser=INVALID_CHAIR;

	//用户状态
	m_tDice=0;
	m_tStart=0;
	m_cbCardData=0;
	m_wOutCardUser=INVALID_CHAIR;
	ZeroMemory(m_cbUserAction,sizeof(m_cbUserAction));
	ZeroMemory(m_bTrustee,sizeof(m_bTrustee));

	m_iSaveCheckPaiIndex = 0;
	m_pGameClientDlg  = NULL;

	//爆炸动画
	m_bDiceEffect=false;
	m_bStartEffect=false;
	m_cbFrameIndex=0;
	m_bMPEffect=false;

	//加载位图
	HINSTANCE hInstance=AfxGetInstanceHandle();

	CString strPath;
	GetPicPath(strPath);
	

	m_ImageBack.SetLoadInfo(TEXT(strPath+ "VIEW_BACK.png"));
	//m_ImageWait.LoadImage(TEXT(strPath+"WAIT_TIP.png"));
	m_ImageUserFlag.LoadImage(TEXT(strPath+"USER_FLAG.png"));
	m_ImageDirectionBack.LoadImage(TEXT(strPath+"DIRECTIONBACK.png"));
	m_ImageUserBankerFlag.LoadImage(TEXT(strPath+"BANKER.png"));
	m_ImageOutCard.LoadImage(TEXT(strPath+"OUT_CARD_TIP.png"));
	m_ImageHuangZhuang.LoadImage(TEXT(strPath+"HUANG_ZHUANG.png"));
	//m_ImageMPEffect.LoadImage(TEXT(strPath+"MOPAI.png"));
	m_ImageTrustee.LoadImage(TEXT(strPath+"TRUSTEE.png"));
	//m_ImageMPBACK.LoadImage(TEXT(strPath+"BACKMP.png"));
	//m_ImageMPCARD.LoadImage(TEXT(strPath+"CARD.png"));
	m_ImageUserGradeNum.LoadImage(TEXT(strPath+"GRADENUM.png"));
	m_ActionSZFlash.LoadImage(TEXT(strPath+"SZPROMPT.png"));
	

	//其余玩家人物信息框
	for (int i=0; i<4; ++i)
	{
		m_ImageUserInfo[i].LoadImage(TEXT(strPath+"USER_INFO.png"));
	}

	////////////////////////////////////////////
	//按钮资源



	////////////////////////////////////////////
	//胡牌类型
	//m_ImageKindTian.LoadImage(hInstance,"TIAN_HU");
	//m_ImageKindDi.LoadImage(hInstance,"DI_HU");
	//m_ImageKindPengPeng.LoadImage(hInstance,"PENGPENG_HU");
	//   m_ImageKindQiXiaoDui.LoadImage(hInstance,"QI_XIAO_DUI");
	//m_ImageKindQingYiSe.LoadImage(hInstance,"QING_YI_SE");
	//m_ImageKindJiangJiang.LoadImage(hInstance,"JIANGJIANG_HU");
	//m_ImageKindHaiDi.LoadImage(hInstance,"HAI_DI");

	////////////////////////////////////////////
	//动画-骰子
	m_ActionDice1.LoadImage(TEXT(strPath+"Dice.png"));
	m_csActionDice1.cx=m_ActionDice1.GetWidth()/9;
	m_csActionDice1.cy=m_ActionDice1.GetHeight();

	m_ActionDice2.LoadImage(TEXT(strPath+"Dice2.png"));
	m_csActionDice2.cx=m_ActionDice2.GetWidth()/6;
	m_csActionDice2.cy=m_ActionDice2.GetHeight();


	//方位动画参数初始化
	m_bIsPlayDirect=false;
	m_iCountDirectIndex=0;
	m_rcDirectFlashDraw=CRect(0, 0, 0, 0);	

	//启动筛子参数初始化
	m_bIsOpenSZFlash=false;		
	m_iCountSZFCurIndex=0;			
	m_rcSZFlashDraw=CRect(0, 0, 0, 0);	
	m_rcSZBTRect=CRect(0, 0, 0, 0);	


	// 筋动画
	m_pTAFlashMuscle = NULL;
	m_byShowMuscle = 0;
	ZeroMemory(m_llGoldMuscle, sizeof(m_llGoldMuscle));
	m_ImageGoldChar.LoadImage(TEXT(strPath + "Cartoon\\muscle\\GoldChar.png"));
	m_INMuscleNumGray.SetSeparatorWH(25, 29);
	m_INMuscleNumGray.SetImageResource(TEXT(strPath + "Cartoon\\muscle\\grayNum.png"));	
	m_INMuscleNumGray.SetSign(true);
	m_INMuscleNumGreen.SetSeparatorWH(25, 29);
	m_INMuscleNumGreen.SetImageResource(TEXT(strPath + "Cartoon\\muscle\\greenNum.png"));
	m_INMuscleNumGreen.SetSign(true);


	m_TestNums.SetSeparatorWH(25, 29);
	m_TestNums.SetImageResource(TEXT(strPath + "Cartoon\\muscle\\greenNum.png"));	
	m_TestNums.SetSign(true);


	strPath.Empty();
	GetMahjonPath(strPath);
	m_ImageOutCardBack.LoadImage(TEXT(strPath+"OUTCARDBACK.png"));
	m_ActionDirectFlash.LoadImage(TEXT(strPath+"ACTIONDIRECT.png"));
	return;
}

//析构函数
CGameClientView::~CGameClientView(void)
{
	m_fontDrawGameInfo.DeleteObject();
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//获取路径
void CGameClientView::GetMahjonPath(CString& strPicPath)
{
	strPicPath="";
	TCHAR szResourcePath[MAX_PATH]={0};
	GetCurrentDirectory(MAX_PATH,szResourcePath);
	strPicPath.Append(szResourcePath);
	strPicPath.Append(TEXT("\\SparrowZZ\\Picture\\Card\\"));
}

//获取路径
void CGameClientView::GetPicPath(CString& strPicPath)
{
	strPicPath="";
	TCHAR szResourcePath[MAX_PATH]={0};
	GetCurrentDirectory(MAX_PATH,szResourcePath);
	strPicPath.Append(szResourcePath);
	strPicPath.Append(TEXT("\\SparrowZZ\\Picture\\PNG\\"));
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//建立消息
int CGameClientView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (__super::OnCreate(lpCreateStruct)==-1) return -1;

	//变量定义
	enDirection Direction[]={Direction_North,Direction_East,Direction_South,Direction_West};

	//设置控件
	for (WORD i=0;i<4;i++)
	{
		//用户扑克
		m_HeapCard[i].SetDirection(Direction[i]);
		m_TableCard[i].SetDirection(Direction[i]);
		m_DiscardCard[i].SetDirection(Direction[i]);

		//组合扑克
		m_WeaveCard[i][0].SetDisplayItem(true);
		m_WeaveCard[i][1].SetDisplayItem(true);
		m_WeaveCard[i][2].SetDisplayItem(true);
		m_WeaveCard[i][3].SetDisplayItem(true);
		//m_WeaveCard[i][0].SetDisplayItem(false);
		//m_WeaveCard[i][1].SetDisplayItem(false);
		//m_WeaveCard[i][2].SetDisplayItem(false);
		//m_WeaveCard[i][3].SetDisplayItem(false);
		m_WeaveCard[i][0].SetDirection(Direction[i]);
		m_WeaveCard[i][1].SetDirection(Direction[i]);
		m_WeaveCard[i][2].SetDirection(Direction[i]);
		m_WeaveCard[i][3].SetDirection(Direction[i]);
	}

	//设置控件
	m_UserCard[0].SetDirection(Direction_North);
	m_UserCard[1].SetDirection(Direction_East);
	m_UserCard[2].SetDirection(Direction_West);

	//创建控件
	CRect rcCreate(0,0,0,0);
	m_ControlWnd.Create(NULL,NULL,WS_CHILD|WS_CLIPCHILDREN,rcCreate,this,10,NULL);


	//创建控件
	CString strPath="";
	GetPicPath(strPath);
	m_btStart.Create(NULL,WS_CHILD,rcCreate,this,IDC_START);
	m_btAutoPlayOn.Create(NULL,WS_CHILD,rcCreate,this,IDC_AUTOPLAY_ON);
	m_btAutoPlayOff.Create(NULL,WS_CHILD,rcCreate,this,IDC_AUTOPLAY_OFF);
	//m_btHaiDi.Create(NULL,WS_CHILD,rcCreate,this,IDC_HAI_DI);
	//m_btNoHaiDi.Create(NULL,WS_CHILD,rcCreate,this,IDC_NO_HAI_DI);
	//m_btTingCard.Create(NULL,WS_CHILD,rcCreate,this,IDC_TING_CARD);
	//m_btBuTing.Create(NULL,WS_CHILD,rcCreate,this,IDC_NO_TING_CARD);

	CString str("Test:"+strPath);
	OutputDebugString(str);
	m_btStart.SetButtonImage(TEXT(strPath+"BEGIN.png"),false,5);
	m_btAutoPlayOn.SetButtonImage(TEXT(strPath+"AUTOPLAYON.png"),false,5);
	m_btAutoPlayOff.SetButtonImage(TEXT(strPath+"AUTOPLAYOFF.png"),false,5);
	//m_btHaiDi.SetButtonImage(TEXT(strPath+"HAIDI.png"),false,5);
	//m_btNoHaiDi.SetButtonImage(TEXT(strPath+"NO_HAIDI.png"),false,5);
	//m_btTingCard.SetButtonImage(TEXT(strPath+"TING.png"),false,5);
	//m_btBuTing.SetButtonImage(TEXT(strPath+"NO_TING.png"),false,5);
	//托管按钮
	//m_btStusteeControl.Create(TEXT(""),WS_CHILD|WS_VISIBLE,rcCreate,this,IDC_TRUSTEE_CONTROL);
	//m_btStusteeControl.LoadRgnImage(AfxGetInstanceHandle(),IDB_BT_START_TRUSTEE,RGB(255,0,255),5);


	//BYTE cbData[3]={0x01,0x21,0x18};
	//SetCanHuPaiData(cbData, 3, 300);
	//UpdateGameView(NULL);

	m_fontDrawGameInfo.CreateFont(14,0,0,0,400,0,0,0,134,3,2,1,2,TEXT("华文细黑"));
	return 0;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//重置界面
void CGameClientView::ResetGameView()
{
	//标志变量
	m_cbHaiDiCard=0;
	//m_bOutCard=false;
	//m_bWaitOther=false;
	m_bIsInitBT=false;
	m_bIsShowOutCardFlash=false;


	//方位动画参数初始化
	m_bIsPlayDirect=false;
	m_iCountDirectIndex=0;
	m_rcDirectFlashDraw=CRect(0, 0, 0, 0);	

	//启动筛子参数初始化
	m_bIsOpenSZFlash=false;		
	m_iCountSZFCurIndex=0;			
	m_rcSZFlashDraw=CRect(0, 0, 0, 0);	
	m_rcSZBTRect=CRect(0, 0, 0, 0);	

	m_nDiceCount1=0;
	m_nDiceCount2=0;

	m_cbFrameIndex=0;

	//游戏属性
	m_lCellScore=0l;
	m_wBankerUser=INVALID_CHAIR;

	//用户状态
	m_cbCardData=0;
	m_wOutCardUser=INVALID_CHAIR;
	ZeroMemory(m_cbUserAction,sizeof(m_cbUserAction));

	//界面设置
	m_btStart.ShowWindow(SW_HIDE);
	//m_ControlWnd.ShowWindow(SW_HIDE);
	SafeShowOrHideWnd(&m_ControlWnd,SW_HIDE);
	//m_FlashControl.ShowWindow(SW_HIDE);


	//禁用控件
	//m_btStusteeControl.EnableWindow(FALSE);
	//开启
	//m_btStusteeControl.EnableWindow(TRUE);
	m_btAutoPlayOn.ShowWindow(TRUE);
	m_btAutoPlayOff.ShowWindow(SW_HIDE);

	//扑克设置
	m_UserCard[0].SetCardData(0,false);
	m_UserCard[1].SetCardData(0,false);
	m_UserCard[2].SetCardData(0,false);
	m_HandCardControl.SetPositively(false);
	m_HandCardControl.SetDisplayItem(false);
	m_HandCardControl.SetCardData(NULL,0,0/*,NULL*/);
	//王牌

	//扑克设置
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		m_HeapCard[i].SetCardData(0,0,0);
		m_TableCard[i].SetCardData(NULL,0);
		m_DiscardCard[i].SetCardData(NULL,0);
		m_WeaveCard[i][0].SetCardData(NULL,0);
		m_WeaveCard[i][1].SetCardData(NULL,0);
		m_WeaveCard[i][2].SetCardData(NULL,0);
		m_WeaveCard[i][3].SetCardData(NULL,0);
	}

	return;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//调整控件
void CGameClientView::RectifyGameView(int nWidth, int nHeight)
{
	//设置坐标
	//动画坐标
	//m_ptOperationFlash[0].x=380;
	//m_ptOperationFlash[0].y=160;
	//m_ptOperationFlash[1].x=630;
	//m_ptOperationFlash[1].y=270;
	//m_ptOperationFlash[2].x=380;
	//m_ptOperationFlash[2].y=380;
	//m_ptOperationFlash[3].x=160;
	//m_ptOperationFlash[3].y=220;

	//语音框位置
	m_ptSayPos[0].x = 482;
	m_ptSayPos[0].y = 105;
	m_ptSayPos[1].x = 738;
	m_ptSayPos[1].y = 237;
	m_ptSayPos[2].x = 138;
	m_ptSayPos[2].y = 514;
	m_ptSayPos[3].x = 83;
	m_ptSayPos[3].y = 198;

	//信息框坐标
	m_Info[0].x=305;
	m_Info[0].y=134-27-5;
	m_Info[1].x=843;
	m_Info[1].y=334-29;
	m_Info[3].x=6;
	m_Info[3].y=334-29;

	//头像名字坐标    
	m_ptFace[0].x=nWidth/2-64;
	m_ptFace[0].y=m_nYBorder-8;
	m_ptName[0].x=nWidth/2+270;
	m_ptName[0].y=m_nYBorder+112;
	m_ptFace[1].x=nWidth-m_nXBorder-222;
	m_ptFace[1].y=nHeight/2-291;
	m_ptName[1].x=nWidth-m_nXBorder-80;
	m_ptName[1].y=nHeight/2-135;
	m_ptFace[2].x=-12;
	m_ptFace[2].y=nHeight-m_nYBorder-272+9;
	m_ptName[2].x=40;
	m_ptName[2].y=nHeight-m_nYBorder-82;
	m_ptFace[3].x=m_nXBorder;
	m_ptFace[3].y=nHeight/2-291;
	m_ptName[3].x=38;
	m_ptName[3].y=nHeight/2-135;

	m_UserDuanXian[0].x = nWidth/2-64+115;
	m_UserDuanXian[0].y = m_nYBorder-8+15;
	m_UserDuanXian[1].x = nWidth-m_nXBorder-222;
	m_UserDuanXian[1].y = nHeight/2-291;
	m_UserDuanXian[2].x = 8;
	m_UserDuanXian[2].y = nHeight-m_nYBorder-250;
	m_UserDuanXian[3].x = m_nXBorder+110;
	m_UserDuanXian[3].y = nHeight/2-291;

	//准备坐标
	m_ptReady[0].x=nWidth/2+10;
	m_ptReady[0].y=m_nYBorder+m_nYTimer/2+180;
	m_ptReady[1].x=nWidth-m_nXBorder-m_nXTimer-155;
	m_ptReady[1].y=nHeight/2-35;
	m_ptReady[2].x=nWidth/2+10;
	m_ptReady[2].y=nHeight-m_nYBorder-m_nYTimer/2-250;
	m_ptReady[3].x=m_nXBorder+m_nXTimer+155;
	m_ptReady[3].y=nHeight/2-35;


	//时间坐标
	m_ptTimer[0].x=555-30;
	m_ptTimer[0].y=120+85;
	m_ptTimer[1].x=835-65;
	m_ptTimer[1].y=249+71;
	m_ptTimer[2].x=876;
	m_ptTimer[2].y=530;
	m_ptTimer[3].x=115+142;
	m_ptTimer[3].y=249+55;

	//托管坐标
	m_PointTrustee[0].x=549;
	m_PointTrustee[0].y=120-28;	
	m_PointTrustee[1].x=768;
	m_PointTrustee[1].y=212-28;	
	m_PointTrustee[2].x=138;
	m_PointTrustee[2].y=479-20;	
	m_PointTrustee[3].x=82;
	m_PointTrustee[3].y=212-28;

	//庄家坐标
	m_UserBankerFlagPos[0].x=517;
	m_UserBankerFlagPos[0].y=26;
	m_UserBankerFlagPos[1].x=900;
	m_UserBankerFlagPos[1].y=110;
	m_UserBankerFlagPos[2].x=9;
	m_UserBankerFlagPos[2].y=400;
	m_UserBankerFlagPos[3].x=10;
	m_UserBankerFlagPos[3].y=110;

	//刚打出的牌坐标
	m_ptLastMahjong[0].SetPoint(415,180);
	m_ptLastMahjong[1].SetPoint(680,250);
	m_ptLastMahjong[2].SetPoint(415,380);
	m_ptLastMahjong[3].SetPoint(150,250);


	//用户扑克(立牌)
	m_UserCard[0].SetControlPoint(nWidth/2-180,m_nYBorder+155);
	m_UserCard[1].SetControlPoint(nWidth-m_nXBorder-230,nHeight/2-150);
	m_UserCard[2].SetControlPoint(m_nXBorder+102,nHeight/2-125);
	m_HandCardControl.SetBenchmarkPos(nWidth/2-30,nHeight-m_nYBorder-135,enXCenter,enYBottom); 

	//结束后四家立牌摆到桌面扑克(逆时针 本家是2)
	m_TableCard[0].SetControlPoint(nWidth/2-40,m_nYBorder+175);
	m_TableCard[1].SetControlPoint(nWidth-m_nXBorder-110,nHeight/2-130);
	m_TableCard[2].SetControlPoint(nWidth/2+30,nHeight-m_nYBorder-195);
	m_TableCard[3].SetControlPoint(m_nXBorder+55,nHeight/2-100);

	//堆积扑克(初始牌)
	int nXCenter=nWidth/2;
	int nYCenter=nHeight/2;
	m_HeapCard[0].SetControlPoint(nXCenter-165,nYCenter-160);
	m_HeapCard[1].SetControlPoint(nXCenter+185,nYCenter-140);
	m_HeapCard[2].SetControlPoint(nXCenter-260,nYCenter+80);
	m_HeapCard[3].SetControlPoint(nXCenter-240,nYCenter-135);


	//丢弃扑克
	m_DiscardCard[0].SetControlPoint(nXCenter-90,nYCenter-116);
	m_DiscardCard[1].SetControlPoint(nXCenter+220,nYCenter-50);
	m_DiscardCard[2].SetControlPoint(nXCenter-125,nYCenter+42);
	m_DiscardCard[3].SetControlPoint(nXCenter-148,nYCenter+50);

	//组合扑克
	m_WeaveCard[0][0].SetControlPoint(0, nWidth/2+200,m_nYBorder+175);
	m_WeaveCard[0][1].SetControlPoint(1, nWidth/2+145,m_nYBorder+175);
	m_WeaveCard[0][2].SetControlPoint(2, nWidth/2+90,m_nYBorder+175);
	m_WeaveCard[0][3].SetControlPoint(3, nWidth/2+35,m_nYBorder+175);

	//组合扑克
	m_WeaveCard[1][0].SetControlPoint(0, nWidth-m_nXBorder-115+7,nHeight/2+63);
	m_WeaveCard[1][1].SetControlPoint(1, nWidth-m_nXBorder-152+7,nHeight/2-22);
	m_WeaveCard[1][2].SetControlPoint(2, nWidth-m_nXBorder-185+7+3,nHeight/2-97+3);
	m_WeaveCard[1][3].SetControlPoint(3, nWidth-m_nXBorder-207+7+3,nHeight/2-157+3);

	//组合扑克
	m_WeaveCard[2][0].SetControlPoint(0, nWidth/2-370,nHeight-m_nYBorder-195);
	m_WeaveCard[2][1].SetControlPoint(1, nWidth/2-280,nHeight-m_nYBorder-195);
	m_WeaveCard[2][2].SetControlPoint(2, nWidth/2-190,nHeight-m_nYBorder-195);
	m_WeaveCard[2][3].SetControlPoint(3, nWidth/2-100,nHeight-m_nYBorder-195);

	//组合扑克
	m_WeaveCard[3][0].SetControlPoint(0, m_nXBorder+204,nHeight/2-120);
	m_WeaveCard[3][1].SetControlPoint(1, m_nXBorder+182,nHeight/2-60);
	m_WeaveCard[3][2].SetControlPoint(2, m_nXBorder+155,nHeight/2+10);
	m_WeaveCard[3][3].SetControlPoint(3, m_nXBorder+125,nHeight/2+90);

	m_rcMuscleNum[0].SetRect((nWidth-220)/2+100, 250, (nWidth-220)/2+320, 250+50);
	m_rcMuscleNum[1].SetRect(nWidth-10-220, 300, nWidth-10, 300+50);
	m_rcMuscleNum[2].SetRect((nWidth-220)/2+100, nHeight-280, (nWidth-220)/2+320, nHeight-280+50);
	m_rcMuscleNum[3].SetRect(270, 300, 270+220, 300+50);


	//控制窗口
	//m_ControlWnd.SetBenchmarkPos(nWidth/2+150,nHeight-m_nYBorder-210);

	CRect rcControlWnd;
	m_ControlWnd.GetClientRect(&rcControlWnd);
	m_ControlWnd.SetWindowPos(&wndTop, nWidth-rcControlWnd.Width()-5, nHeight-rcControlWnd.Height()-250, 0, 0, SWP_NOSIZE);


	//m_FlashControl.SetBenchmarkPos(nWidth/2,nHeight-m_nYBorder-160);

	//移动按钮
	CRect rcButton;
	HDWP hDwp=BeginDeferWindowPos(32);
	m_btStart.GetWindowRect(&rcButton);
	const UINT uFlags=SWP_NOACTIVATE|SWP_NOZORDER|SWP_NOCOPYBITS|SWP_NOSIZE;

	//移动调整
	DeferWindowPos(hDwp,m_btStart,NULL,402,329-29,0,0,uFlags);
	//DeferWindowPos(hDwp,m_btHaiDi,NULL,595,487-29,0,0,uFlags);
	//DeferWindowPos(hDwp,m_btNoHaiDi,NULL,661,487-29,0,0,uFlags);
	DeferWindowPos(hDwp,m_btAutoPlayOn,NULL,571,616-29,0,0,uFlags);
	DeferWindowPos(hDwp,m_btAutoPlayOff,NULL,571,616-29,0,0,uFlags);
	//DeferWindowPos(hDwp,m_btTingCard,NULL,403,616-29,0,0,uFlags);
	//DeferWindowPos(hDwp,m_btBuTing,NULL,469,616-29,0,0,uFlags);
	//移动调整
	DeferWindowPos(hDwp,m_btStusteeControl,NULL,nWidth-m_nXBorder-(rcButton.Width()+5),nHeight-m_nYBorder-rcButton.Height()-180,0,0,uFlags);
	//结束移动
	EndDeferWindowPos(hDwp);

	for(WORD i=0;i<GAME_PLAYER;i++)
	{
		if(i!=3&&i!=2) SetFlowerControlInfo(i,m_ptFace[i].x, m_ptFace[i].y);
		else if(i==2) SetFlowerControlInfo(i,m_ptFace[i].x, m_ptFace[i].y-110);
		else SetFlowerControlInfo(i,m_ptFace[i].x-70, m_ptFace[i].y);
	}

	RectifyKeepTimePos(735, 539);

	TestClient(0);
	return;
}


////////动画///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//骰子动画1
void CGameClientView::ActionDice1( CDC *pDC, int nFrame )
{
	LONG lWidth, lHeight;
	lWidth = m_csActionDice1.cx;
	lHeight = m_csActionDice1.cy;
	m_ActionDice1.DrawImage( pDC, PS_BX_DICE, PS_BY_DICE, lWidth, lHeight, lWidth*nFrame, 0);
}
//骰子动画2
void CGameClientView::ActionDice2( CDC *pDC,int pos1, int pos2 )
{
	LONG lWidth, lHeight;
	lWidth = m_csActionDice2.cx;
	lHeight = m_csActionDice2.cy;

	m_ActionDice2.DrawImage( pDC, PS_BX_DICE1, PS_BY_DICE1, lWidth, lHeight, lWidth*pos1, 0);
	m_ActionDice2.DrawImage( pDC, PS_BX_DICE2, PS_BY_DICE2, lWidth, lHeight, lWidth*pos2, 0);
}
//摸牌动画
void CGameClientView::ActionMP( CDC *pDC, int nFrame)
{
	if (nFrame>=MP_EFFECT_COUNT)
	{
		nFrame = MP_EFFECT_COUNT;
	}
	//绘画摸牌背景
	//INT nHeight=m_ImageMPBACK.GetHeight();
	//INT nWidth=m_ImageMPBACK.GetWidth();
	//m_ImageMPBACK.DrawImage(pDC,340,190,nWidth,nHeight,0,0);

	//绘画效果
	//if (nFrame < MP_EFFECT_COUNT)
	//{
	//	INT nImageHeight=m_ImageMPEffect.GetHeight();
	//	INT nImageWidth=m_ImageMPEffect.GetWidth()/MP_EFFECT_COUNT;
	//	m_ImageMPEffect.DrawImage(pDC,340,190,nImageWidth,nImageHeight,
	//		nImageWidth*(nFrame%MP_EFFECT_COUNT),0);
	//}

	//if (nFrame == MP_EFFECT_COUNT)
	//{
	//	INT nImageHeight=m_ImageMPCARD.GetHeight();
	//	INT nImageWidth=m_ImageMPCARD.GetWidth()/27;
	//	m_ImageMPCARD.DrawImage(pDC,340,190,nImageWidth,nImageHeight,
	//		nImageWidth*m_nKingCardIndex,0);		
	//}
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//绘画界面
void CGameClientView::DrawGameView(CDC * pDC, int nWidth, int nHeight)
{
	//绘画背景
	CImageHandle ImageHandleBack(&m_ImageBack);
	m_ImageBack.BitBlt(pDC->GetSafeHdc(),0,0,SRCCOPY);

	//绘画用户
	pDC->SetTextColor(RGB(255,255,0));
	for (BYTE i=0;i<GAME_PLAYER;i++)
	{
		//变量定义
		const tagUserData * pUserData=GetUserInfo(i);

		if (pUserData!=NULL)
		{
			//char chUserDataInfo[100]={0};
			//::memset(chUserDataInfo, 0, sizeof(chUserDataInfo));
			//sprintf(chUserDataInfo, "%d", pUserData->wFaceID);
			
			DrawCustomFace(pDC, i, pUserData, m_ptFace[i].x, m_ptFace[i].y);
			if (US_OFFLINE == pUserData->cbUserStatus)
			{
				DrawBreakLineTag(pDC, m_UserDuanXian[i].x, m_UserDuanXian[i].y);
			}
			 
			//其余用户信息框
			if (2!=i)
			{
				m_ImageUserInfo[i].DrawImage(pDC,m_Info[i].x,m_Info[i].y);
			}

			//用户名字
			//DrawTextString(pDC,pUserData->szName,RGB(240,240,240),RGB(50,50,50),m_ptName[i].x-10,m_ptName[i].y);

			////时间
			//DrawUserTimer(pDC, i);

			//等级称谓
			if (i!=2)
			{
				CFont * pOldFont=pDC->SelectObject(&m_fontDrawGameInfo);
				pDC->TextOut(m_Info[i].x+37,m_Info[i].y+28,pUserData->chGradeName,strlen(pUserData->chGradeName));
				pDC->SelectObject(pOldFont);
				pOldFont = NULL;
			}


			//等级
			if (i!=2)
			{
				INT GrandWith=m_ImageUserGradeNum.GetWidth()/12;
				INT GrandHeighe=m_ImageUserGradeNum.GetHeight();
				INT GrandCount=0;
				INT Grand=pUserData->dwGrade;
				INT Fan=Grand;
				while(1<=Grand)
				{
					GrandCount++;
					Grand/=10;
				}
				if (1<=GrandCount)
				{
					INT xPos=m_Info[i].x+40;
					INT yPos=m_Info[i].y+7;
					while (0<GrandCount)
					{
						xPos+=GrandWith;
						--GrandCount;
						INT n_Mutiple=(INT)pow(10,GrandCount);
						INT DrawGrand=(INT)(Fan/n_Mutiple);
						m_ImageUserGradeNum.DrawImage(pDC,xPos,yPos,GrandWith,GrandHeighe,DrawGrand*GrandWith,0);
						Fan-=(n_Mutiple*DrawGrand);
					}
				}
			}
		}

		//托管
		if(m_bTrustee[i])
		{
			m_ImageTrustee.DrawImage(pDC,m_PointTrustee[i].x,m_PointTrustee[i].y);
		}

		//海底牌显示
		if((m_cbUserAction[i]&WIK_CHI_HU) && (m_wChiHuRight[i] & CHR_HAI_DI))
		{
			//变量定义
			int nXImagePos=0;
			int nItemWidth=m_ImageHuangZhuang.GetWidth();
			int nItemHeight=m_ImageHuangZhuang.GetHeight()/2;

			//绘画动作
			BYTE byIndex = m_CardControl.GetImageIndex(m_cbHaiDiCard);
			m_CardControl.m_ImageUserBottom.DrawImage(pDC,450,450,48,75,byIndex*48,0,48,75);
			m_ImageHuangZhuang.DrawImage(pDC,390,410,nItemWidth,45,nXImagePos,0,nItemWidth, 45);
		}
	}

	// 绘制麻将
	OnDrawMahjong(pDC);
	
	//绘画时间
	for (BYTE i=0;i<GAME_PLAYER;i++)
	{
		//变量定义
		const tagUserData * pUserData=GetUserInfo(i);
		if (pUserData!=NULL)
		{//时间
			DrawUserTimer(pDC, i);
			//准备标识
			if (pUserData->cbUserStatus==US_READY)
			{
				DrawUserReady(pDC,m_ptReady[i].x,m_ptReady[i].y);
			}
		}
	}

	//播放方位动画
	if (m_bIsPlayDirect)
	{
		if (!m_ActionDirectFlash.IsNull())
		{
			m_ActionDirectFlash.DrawImage(pDC, m_rcDirectFlashDraw.left, m_rcDirectFlashDraw.top, I_DIRECT_FLASH_WIDTH, I_DIRECT_FLASH_HEIGHT, \
				m_iCountDirectIndex*I_DIRECT_FLASH_WIDTH, 0, I_DIRECT_FLASH_WIDTH, I_DIRECT_FLASH_HEIGHT);
		}	
	}

	//用户方位标志
	if (m_iCountDirectIndex>=I_DIRECT_FLASH_TOTOL_NUMS)
	{
		if (m_wBankerUser!=INVALID_CHAIR)
		{
			//加载位图
			int nImageWidth=m_ImageUserFlag.GetWidth()/4;
			int nImageHeight=m_ImageUserFlag.GetHeight();

			int nBankerWidth = m_ImageUserBankerFlag.GetWidth();
			int nBankerHeight = m_ImageUserBankerFlag.GetHeight();

			//绘画标志
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				WORD wIndex=((i+GAME_PLAYER)-m_wBankerUser)%GAME_PLAYER;
				//变量定义
				const tagUserData * pUserData=GetUserInfo(wIndex);
				if (NULL==pUserData)
				{
					continue;
				}
				if (2==i)
				{
					if (0==wIndex)
					{//庄家标识
						m_ImageUserBankerFlag.DrawImage(pDC, m_UserBankerFlagPos[i].x,m_UserBankerFlagPos[i].y,nBankerWidth,nBankerHeight,0,0);
					}
					else
					{
						m_ImageDirectionBack.DrawImage(pDC, m_UserBankerFlagPos[i].x,m_UserBankerFlagPos[i].y);
						m_ImageUserFlag.DrawImage(pDC, m_UserBankerFlagPos[i].x+10,m_UserBankerFlagPos[i].y+17,nImageWidth,nImageHeight,nImageWidth*wIndex,0,nImageWidth,nImageHeight);
					}		
				}
				else
				{
					m_ImageUserFlag.DrawImage(pDC, m_Info[i].x+84,m_Info[i].y+3,nImageWidth,nImageHeight,nImageWidth*wIndex,0,nImageWidth,nImageHeight);
				}
			}
		}
	}



	//启动筛子动画
	if (m_bIsOpenSZFlash)
	{
		if (!m_ActionSZFlash.IsNull())
		{
			m_ActionSZFlash.DrawImage(pDC, m_rcSZFlashDraw.left, m_rcSZFlashDraw.top, I_SZ_FLASH_WIDTH, I_SZ_FLASH_HEIGHT, \
				m_iCountSZFCurIndex*I_SZ_FLASH_WIDTH, 0, I_SZ_FLASH_WIDTH, I_SZ_FLASH_HEIGHT);
		}
	}

	//骰子动画
	if (m_nDiceCount1<0 || m_nDiceCount1>5)
	{
		m_nDiceCount1=0;
	}
	if (m_nDiceCount2<0 || m_nDiceCount2>5)
	{
		m_nDiceCount2=0;
	}
	if (m_bDiceEffect==true)
	{
		if( m_cbFrameIndex>=0 && m_cbFrameIndex<9)
		{
			ActionDice1( pDC,m_cbFrameIndex);
		}
		else if( m_cbFrameIndex>= 9)
		{
			ActionDice2( pDC,m_nDiceCount1,m_nDiceCount2);
		}
	}

	//摸牌动画 
	if (m_bMPEffect==true)
	{
		ActionMP( pDC,m_cbFrameIndex);
	}

	////等待提示
	//if (m_bWaitOther==true)
	//{
	//	m_ImageWait.DrawImage(pDC,(nWidth-m_ImageWait.GetWidth())/2,nHeight-150-30);
	//}

	//荒庄标志
	if (true == m_bIsHuangZhuang && m_cbHaiDiCard!=0)                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                    
	{
		BYTE byIndex = m_CardControl.GetImageIndex(m_cbHaiDiCard);
		m_CardControl.m_ImageUserBottom.DrawImage(pDC,450,450,48,75,byIndex*48,0,48,75);
		m_ImageHuangZhuang.DrawImage(pDC,390,411,180,45,0,0,180, 45);
	}

	// 绘制刚打出的麻将
	OnDrawLastMahjong(pDC);

	// 绘制筋动画
	OnDrawMuscle(pDC);
	return;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//基础积分
void CGameClientView::SetCellScore(LONGLONG lCellScore)
{
	//设置扑克
	if (lCellScore!=m_lCellScore)
	{
		//设置变量
		m_lCellScore=lCellScore;

		//更新界面
		UpdateGameView(NULL);
	}

	return;
}

LONGLONG CGameClientView::GetCellScore()
{
	return m_lCellScore;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//海底扑克
void CGameClientView::SetHaiDiCard(BYTE cbHaiDiCard)
{
	//设置扑克
	if (cbHaiDiCard!=m_cbHaiDiCard)
	{
		//设置变量
		m_cbHaiDiCard=cbHaiDiCard;

		//更新界面
		UpdateGameView(NULL);
	}

	return;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//庄家用户
void CGameClientView::SetBankerUser(WORD wBankerUser)
{
	//设置用户
	if (wBankerUser!=m_wBankerUser)
	{
		//设置变量
		m_wBankerUser=wBankerUser;

		//更新界面
		UpdateGameView(NULL);
	}


	if (INVALID_CHAIR == wBankerUser)
	{
		m_iCountDirectIndex = 0;
	}
	return;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//状态标志
//void CGameClientView::SetStatusFlag(bool bOutCard, bool bWaitOther)
//{
	//设置变量
	//m_bOutCard=bOutCard;
	//m_bWaitOther=bWaitOther;

	//更新界面
//	UpdateGameView(NULL);

//	return;
//}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//出牌信息
void CGameClientView::SetOutCardInfo(WORD wViewChairID, BYTE cbCardData)
{
	if (INVALID_CHAIR!=m_wOutCardUser && m_cbCardData>0)
	{
		if (INVALID_CHAIR==wViewChairID && 0==cbCardData)
		{
			//播放动画
			CPoint pt;
			pt.x =  m_rcShowDrawArea.left;
			pt.y = m_rcShowDrawArea.top;
			m_OutCardFlash.SetFlashPosition(pt);
			m_OutCardFlash.Play(true);
		}
	}
	//设置变量
	m_cbCardData=cbCardData;
	m_wOutCardUser=wViewChairID;

	//启动显示牌动画定时器
	KillTimer(IDI_SHOW_CARD);
	if (INVALID_CHAIR!=wViewChairID && cbCardData>0)
	{
		int nXPos=0, nYPos = 0;
		switch (wViewChairID)
		{
		case 0:	//北向
			{
				nXPos=415;
				nYPos=180;
				break;
			}
		case 1:	//东向
			{
				nXPos=680;
				nYPos=250;
				break;
			}
		case 2:	//南向
			{
				nXPos=410;
				nYPos=380;
				break;
			}
		case 3:	//西向
			{
				nXPos=150;
				nYPos=250;
				break;
			}
		}
		m_rcShowDrawArea.left = nXPos;
		m_rcShowDrawArea.top = nYPos;
		m_rcShowDrawArea.right = m_rcShowDrawArea.left + 150;
		m_rcShowDrawArea.bottom = m_rcShowDrawArea.top + 150;

		SetTimer(IDI_SHOW_CARD,IDI_TIME_SHOW_CARD_INTEVAL,NULL);
	}

	//更新界面
	UpdateGameView(NULL);

	return;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//动作信息
void CGameClientView::SetUserAction(WORD wViewChairID, BYTE bUserAction)
{
	//设置变量
	if (wViewChairID<GAME_PLAYER)
		m_cbUserAction[wViewChairID]=bUserAction;
	else ZeroMemory(m_cbUserAction,sizeof(m_cbUserAction));

	//更新界面
	UpdateGameView(NULL);

	return;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//设置托管
void CGameClientView::SetTrustee(WORD wTrusteeUser,bool bTrustee)
{
	//校验数据 
	ASSERT(wTrusteeUser>=0&&wTrusteeUser<GAME_PLAYER);

	if(m_bTrustee[wTrusteeUser] !=bTrustee)	
	{
		//设置数据
		m_bTrustee[wTrusteeUser]=bTrustee;

		//更新界面
		UpdateGameView(NULL);
	}
	return;

}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//艺术字体
void CGameClientView::DrawTextString(CDC * pDC, LPCTSTR pszString, COLORREF crText, COLORREF crFrame, int nXPos, int nYPos)
{
	//变量定义
	int nStringLength=lstrlen(pszString);
	int nXExcursion[8]={1,1,1,0,-1,-1,-1,0};
	int nYExcursion[8]={-1,0,1,1,1,0,-1,-1};

	//绘画边框
	pDC->SetTextColor(crFrame);
	for (int i=0;i<CountArray(nXExcursion);i++)
	{
		pDC->TextOut(nXPos+nXExcursion[i],nYPos+nYExcursion[i],pszString,nStringLength);
	}

	//绘画字体
	pDC->SetTextColor(crText);
	pDC->TextOut(nXPos,nYPos,pszString,nStringLength);

	return;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//光标消息
BOOL CGameClientView::OnSetCursor(CWnd * pWnd, UINT nHitTest, UINT uMessage)
{
	//获取光标
	CPoint MousePoint;
	GetCursorPos(&MousePoint);
	ScreenToClient(&MousePoint);

	//点击测试
	bool bRePaint=false;
	bool bHandle=m_HandCardControl.OnEventSetCursor(MousePoint,bRePaint);

	//重画控制
	if (bRePaint==true)
	{
		CRect rect;
		rect.left = 100;
		rect.top = 500;
		rect.right = rect.left + 700;
		rect.bottom = rect.top + 90;
		UpdateGameView(&rect);
	}

	//光标控制
	if (bHandle==false) __super::OnSetCursor(pWnd,nHitTest,uMessage);

	return TRUE;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//鼠标消息
void CGameClientView::OnLButtonDown(UINT nFlags, CPoint Point)
{
	__super::OnLButtonDown(nFlags, Point);

	if (TRUE == m_ControlWnd.IsWindowVisible())
	{
		return;
	}

	//tagGangCardResult bucardresult;
	//::memset(&bucardresult, 0, sizeof(tagGangCardResult));
	//::memcpy(bucardresult.cbCardData, pSendCard->byBuCard, sizeof(BYTE)*pSendCard->byBuCardNums);
	//bucardresult.cbCardCount = 5;
	//m_ControlWnd.SetControlInfo(NULL,cbActionMask,bucardresult);
	//m_ControlWnd.AddGangCardData(pSendCard->byGangCard, pSendCard->byGangCardNums);

	//获取扑克
	BYTE cbHoverCard=m_HandCardControl.GetHoverCard();
	bool bCanSelectCard = false;

	if (m_HandCardControl.GetSelectStatus())
	{
		for (BYTE i=0; i<m_pGameClientDlg->m_byCanSelectCount; i++)
		{
			if (cbHoverCard == m_pGameClientDlg->m_byCanSelectCard[i])
			{
				bCanSelectCard = true;
				break;
			}
		}
	}

	// 弹起的牌有效  或  能选择进入杠，碰的牌
	if (bCanSelectCard)
	{
		m_pGameClientDlg->SendCardCommand(m_pGameClientDlg->m_byCanSelectType, cbHoverCard);
	}
	else if (cbHoverCard!=0)
	{
		AfxGetMainWnd()->PostMessage(IDM_OUT_CARD,cbHoverCard,cbHoverCard);
	}

	//m_CardHuPaiList.ShowWindow(SW_HIDE);
	SafeShowOrHideWnd(&m_CardHuPaiList, SW_HIDE);

	CString strBuffer;
	strBuffer.Format("client:CGameClientView::OnLButtonDown() %d,  bCanSelectCard=%d", cbHoverCard, bCanSelectCard);
	OutputDebugString(strBuffer);
	return;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//骰子动画
bool CGameClientView::SetDiceEffect(bool bDiceEffect)
{
	if (bDiceEffect==true)
	{
		//设置变量
		m_bDiceEffect=true;
		m_cbFrameIndex=0;

		//启动时间
		SetTimer(IDI_DICE_EFFECT,200,NULL);
	}
	else
	{
		//停止动画
		if (m_bDiceEffect==true)
		{
			//删除时间
			KillTimer(IDI_DICE_EFFECT);

			//设置变量
			m_bDiceEffect=false;
			m_cbFrameIndex=0;

			//更新界面
			UpdateGameView(NULL);
		}
	}

	return true;
}

//设置摸牌 2011.8.3
bool CGameClientView::SetMPEffect(bool bMPEffect)
{
	AfxGetMainWnd()->SendMessage(IDM_MOPAI,0,0);
	return true;

	if (bMPEffect==true)
	{
		//设置变量
		m_bMPEffect=true;
		m_cbFrameIndex=0;

		//启动时间
		SetTimer(IDI_MP_EFFECT,100,NULL);
		((CGameClientDlg*)(AfxGetMainWnd()))->PlaySound("fanwang",false);
	}
	else
	{
		//停止动画
		if (m_bMPEffect==true)
		{
			//删除时间
			KillTimer(IDI_MP_EFFECT);

			//设置变量
			m_bMPEffect=false;
			m_cbFrameIndex=0;

			//更新界面
			UpdateGameView(NULL);
		}
	}

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//时间消息 2011.8.3
VOID CGameClientView::OnTimer(UINT nIDEvent)
{
	__super::OnTimer(nIDEvent);

	//方位动画
	if (IDI_DIRECT_EFFECT == nIDEvent)
	{

		if (m_iCountDirectIndex>=I_DIRECT_FLASH_TOTOL_NUMS-1)
		{
			//删除时间
			KillTimer(IDI_DIRECT_EFFECT);

			m_iCountDirectIndex = I_DIRECT_FLASH_TOTOL_NUMS;
			//设置变量
			m_bIsPlayDirect=false;
			m_cbFrameIndex=0;
			//播放声音

			AfxGetMainWnd()->SendMessage(IDM_SZSTART,0,0);
		}
		else
		{
			++m_iCountDirectIndex;
		}
		UpdateGameView(&m_rcDirectFlashDraw);
	}
	//启动筛子动画
	else if (IDI_SZFLASH_EFFECT == nIDEvent)
	{
		++m_iCountSZFCurIndex;
		if (m_iCountSZFCurIndex>=I_SZ_FLASH_TOTOL_NUMS-1)
		{
			m_iCountSZFCurIndex = 0;
		}
		UpdateGameView(&m_rcSZFlashDraw);
	}
	//骰子动画
	else if (nIDEvent==IDI_DICE_EFFECT)
	{
		//设置变量
		if ((m_cbFrameIndex+1)>=DICE_EFFECT_COUNT+6)
		{
			//删除时间
			KillTimer(IDI_DICE_EFFECT);

			//设置变量
			m_bDiceEffect=false;
			m_cbFrameIndex=0;
			SetMPEffect(true);
		}
		else 
		{
			m_cbFrameIndex++;
		}

		//更新界面
		UpdateGameView(NULL);

		return;
	}
	//摸牌动画
	else if (nIDEvent==IDI_MP_EFFECT)
	{
		//设置变量
		if ((m_cbFrameIndex+1)>=(MP_EFFECT_COUNT+6))
		{
			//删除时间
			KillTimer(IDI_MP_EFFECT);

			//设置变量
			m_bMPEffect=false;
			m_cbFrameIndex=0;
			AfxGetMainWnd()->SendMessage(IDM_MOPAI,0,0);
		}
		else 
		{
			m_cbFrameIndex++;
		}

		//更新界面
		UpdateGameView(NULL);

		return;
	}	
	else if (nIDEvent==IDI_SHOW_CARD)//出牌显示
	{
		m_bIsShowOutCardFlash = !m_bIsShowOutCardFlash;
		UpdateGameView(&m_rcShowDrawArea);
	}
	else if (IDI_MUSCLE_GOLD == nIDEvent)
	{
		if (0 >= m_byShowMuscle)
		{
			KillTimer(IDI_MUSCLE_GOLD);
			if (NULL != m_pTAFlashMuscle)
			{
				m_pTAFlashMuscle->Stop();
				delete m_pTAFlashMuscle;
				m_pTAFlashMuscle = NULL;
			}
		}
		else
		{
			--m_byShowMuscle;
		}
		UpdateGameView(NULL);
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//开始按钮
void CGameClientView::OnStart()
{
	//发送消息
	AfxGetMainWnd()->SendMessage(IDM_START,0,0);
	return;
}

////海底按钮
//void CGameClientView::OnHaiDi()
//{
//	//发送消息
//	AfxGetMainWnd()->SendMessage(IDM_OPERATE_HAI_DI,1,1);
//
//	return;
//}
//
////放弃按钮
//void CGameClientView::OnNoHaiDi()
//{
//	//发送消息
//	AfxGetMainWnd()->SendMessage(IDM_OPERATE_HAI_DI,0,0);
//
//	return;
//}
//拖管控制
void CGameClientView::OnStusteeControl()
{
	AfxGetMainWnd()->SendMessage(IDM_TRUSTEE_CONTROL,0,0);
	return;
}

////听牌控制
//void CGameClientView::OnTingCard()
//{
//	AfxGetMainWnd()->SendMessage(IDM_TING_CARD,1,0);
//	return;
//}
//
////不听按钮
//void CGameClientView::OnNoTingCard()
//{
//	AfxGetMainWnd()->SendMessage(IDM_TING_CARD,0,0);
//}


// 自动托管
void CGameClientView::OnAutoPlayerOn()
{
	m_btAutoPlayOn.ShowWindow(SW_HIDE);
	m_btAutoPlayOff.ShowWindow(SW_SHOW);
	AfxGetMainWnd()->SendMessage(IDM_TRUSTEE_CONTROL,1,1);
	return;
}

// 取消托管
void CGameClientView::OnAutoPlayerOff()
{
	m_btAutoPlayOn.ShowWindow(SW_SHOW);
	m_btAutoPlayOff.ShowWindow(SW_HIDE);
	AfxGetMainWnd()->SendMessage(IDM_TRUSTEE_CONTROL,0,0);
	return;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//获取胡牌数据
void CGameClientView::SetCanHuPaiData(const BYTE* pCardData, int iCardNums, int iX)
{
	if (NULL != pCardData &&  iCardNums>0)
	{
		CRect rect;
		POINT ptLocal;
		ptLocal.x = iX;
		ptLocal.y = 480;
		m_CardHuPaiList.SetCardData(pCardData, iCardNums);
		m_CardHuPaiList.ShowWin(ptLocal);
		//m_CardHuPaiList.ShowWindow(SW_SHOW);
		SafeShowOrHideWnd(&m_CardHuPaiList, SW_SHOW);
	}
	else
	{
		m_CardHuPaiList.SetCardData(NULL, 0);
		//m_CardHuPaiList.ShowWindow(SW_HIDE);
		SafeShowOrHideWnd(&m_CardHuPaiList, SW_HIDE);
	}
}

void CGameClientView::SetGameClientDlgPtr(CGameClientDlg* pGameClient)
{
	if (NULL != pGameClient)
	{
		m_pGameClientDlg = pGameClient;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//鼠标移动智能提示
void CGameClientView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	CPoint MousePoint;
	GetCursorPos(&MousePoint);
	ScreenToClient(&MousePoint);

	//点击测试
	int ibHandle=m_HandCardControl.CheckIndex(MousePoint);

	if (m_iSaveCheckPaiIndex == ibHandle)
	{
		CGameFrameView::OnMouseMove(nFlags, point);
		return;
	}

	if (TRUE == m_ControlWnd.IsWindowVisible())
	{
		return;
	}

	m_iSaveCheckPaiIndex = ibHandle;
	//if (ibHandle < 0)
	//{
	//	m_HandCardControl.InitCurCardDataX();
	//	//m_CardHuPaiList.ShowWindow(SW_HIDE);
	//	SafeShowOrHideWnd(&m_CardHuPaiList, SW_HIDE);
	//	UpdateGameView(NULL);
	//}
	//else if (ibHandle>=0 && ibHandle<=13)
	//{
	//	BYTE byCardData=0;
	//	int iX = 0;
	//	m_HandCardControl.GetCurCardDataAndX(byCardData, iX);
	//	if (0 != byCardData)
	//	{
	//		m_pGameClientDlg->CheckCanHuPai(byCardData, iX);
	//	}
	//}
	CGameFrameView::OnMouseMove(nFlags, point);
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////启动听牌动画
//void CGameClientView::PlayTingCardFlash(BYTE byViewID)
//{
//	CString strPath="";
//	GetPicPath(strPath);
//	strPath.Append("Cartoon\\");
//	CPoint pt;
//	pt.x = 0;
//	pt.y = 0; 
//	if (0==byViewID)
//	{
//		pt.x=373;
//		pt.y=32;
//	}
//	else if (1==byViewID)
//	{
//		pt.x=678;
//		pt.y=191;
//	}
//	else if (2==byViewID)
//	{
//		pt.x=357;
//		pt.y=377-30;
//	}
//	else
//	{
//		pt.x=74;
//		pt.y=182;
//	}
//	m_TingCardFlash.InitFlash(strPath+"ActTing.png", 10, pt);
//	m_TingCardFlash.Play(true, 0);
//
//}

//播放操作动画
void CGameClientView::PlayOperationFlash(BYTE byViewID,BYTE byUserAction)
{
	CString strPath="";
	GetPicPath(strPath);
	strPath.Append("Cartoon\\");
	CPoint pt(0, 0);

	if (WIK_PENG&byUserAction)
	{
		if (0==byViewID)
		{
			pt.x=293;
			pt.y=109;
		}
		else if (1==byViewID)
		{
			pt.x=569;
			pt.y=158;
		}
		else if (2==byViewID)
		{
			pt.x=290;
			pt.y=387-30;
		}
		else
		{
			pt.x=63;
			pt.y=134;
		}

		//m_GameOperationFlash.InitFlash(strPath+"ActPeng.png", 12, pt);
		//m_GameOperationFlash.Play(true, 0);
		CThreadAnimationManager::Play((strPath+"ActPeng.png").AllocSysString(), pt, 150, 12);
	}
}

void CGameClientView::PlayMuscleFlash(BYTE byTargetViewID, BYTE bySourceViewID, BYTE byUserAction, const LONGLONG llGold[])
{
	CopyMemory(m_llGoldMuscle, llGold, sizeof(m_llGoldMuscle));

	if (NULL != m_pTAFlashMuscle)
	{
		m_pTAFlashMuscle->Stop();
		m_pTAFlashMuscle->DestroyWindow();
		delete m_pTAFlashMuscle;
	}

	m_pTAFlashMuscle = NULL;
	m_pTAFlashMuscle = new CThreadAnimation;

	ASSERT(NULL != m_pTAFlashMuscle);
	if (NULL == m_pTAFlashMuscle)
	{
		return;
	}

	CPoint pt(0, 0);
	CString strPath="";
	GetPicPath(strPath);
	strPath.Append("Cartoon\\muscle\\");

	if (0==byTargetViewID)
	{
		pt.x=373;
		pt.y=32;
	}
	else if (1==byTargetViewID)
	{
		pt.x=678;
		pt.y=191;
	}
	else if (2==byTargetViewID)
	{
		pt.x=357;
		pt.y=377-30;
	}
	else
	{
		pt.x=74;
		pt.y=182;
	}

	switch (byUserAction)
	{
	case 1://1 暗筋
		{
			strPath.Append("darkMuscle.png");
			break;
		}
	case 2://2 明筋
		{
			strPath.Append("brightMuscle.png");
			break;
		}
	case 3://3 放筋
		{
			strPath.Append("putMuscle.png");
			break;
		}
	default: break;//0 无效值
	}

	m_byShowMuscle = 8;
	SetTimer(IDI_MUSCLE_GOLD, 500, NULL);
	m_pTAFlashMuscle->Create(this, 1000, strPath.AllocSysString());
	
	CRect rcWnd, rcSelf;
	GetClientRect(&rcWnd);
	m_pTAFlashMuscle->GetClientRect(&rcSelf);
	m_pTAFlashMuscle->Play(CPoint((rcWnd.Width()-498)/2, (rcWnd.Height()-250)/2), 150, 4, FALSE, FALSE, 10, 2);
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//播放胡牌动画
void CGameClientView::PlayFangPaoFlash(BYTE byViewID)
{
	CString strPath="";
	GetPicPath(strPath);
	strPath.Append("Cartoon\\");
	CPoint pt(0,0);

	//放炮动画
	if (0==byViewID)
	{
		pt.x=384;
		pt.y=147;
	}
	else if (1==byViewID)
	{
		pt.x=682;
		pt.y=255;
	}
	else if (2==byViewID)
	{
		pt.x=388;
		pt.y=427;
	}
	else
	{
		pt.x=106;
		pt.y=255;
	}
	//MessageBox("放炮");
	//m_ChiHuKindFlashFangPao.InitFlash(strPath+"ActFangPao.png", 8, pt);
	//m_ChiHuKindFlashFangPao.Play(true, 0);
	CThreadAnimationManager::Play((strPath+"ActFangPao.png").AllocSysString(), pt, 150, 10, FALSE, TRUE);//by han 20120109

}


//播放胡牌动画
void CGameClientView::PlayChiHuFlash(BYTE byViewID)
{
	CString strPath="";
	GetPicPath(strPath);
	strPath.Append("Cartoon\\");
	CPoint pt(0,0);

	if (0!=m_wChiHuType  &&  5!=m_wChiHuType)
	{
		if (3!=m_iChiHuNums && (m_cbUserAction[byViewID]&WIK_CHI_HU))
		{
			//if(m_wChiHuRight[byViewID] & CHR_HAI_DI)
			//{//海底
				//if (4==m_wChiHuType)
				//{//海底炮
				//	//MessageBox("海底炮");
				//	//m_ChiHuKindFlashHaidi.InitFlash(strPath+"ActHaiDiPao.png", 8, CPoint(308,260));
				//	CThreadAnimationManager::Play((strPath+"ActHaiDiPao.png").AllocSysString(), CPoint(308,260), 150, 8, FALSE, TRUE);
				//}
				//else if(3==m_wChiHuType)
				//{//海底捞月(自摸)
				//	//MessageBox("海底捞月");
				//	//m_ChiHuKindFlashHaidi.InitFlash(strPath+"ActHaiDiLaoYue.png", 8, CPoint(200,110));
				//	CThreadAnimationManager::Play((strPath+"ActHaiDiLaoYue.png").AllocSysString(), CPoint(200,110), 150, 8, FALSE, TRUE);
				//}
				//m_ChiHuKindFlashHaidi.Play(true, 0);
			//} else  
			//if((2==m_wChiHuType)&&(m_wChiHuRight[byViewID] & CHR_QIANG_GANG))
			//{//抢杠胡（杠上炮）
			//	if (0==byViewID)
			//	{
			//		pt.x=328;
			//		pt.y=100;
			//	}
			//	else if (1==byViewID)
			//	{
			//		pt.x=664;
			//		pt.y=255;
			//	}
			//	else if (2==byViewID)
			//	{
			//		pt.x=300;
			//		pt.y=463;
			//	}
			//	else
			//	{
			//		pt.x=72;
			//		pt.y=238;
			//	}
			//	//MessageBox("抢杠胡");
			//	//m_ChiHuKindFlashGang.InitFlash(strPath+"ActQiangGangHu.png", 7, pt);
			//	//m_ChiHuKindFlashGang.Play(true, 0);
			//	CThreadAnimationManager::Play((strPath+"ActQiangGangHu.png").AllocSysString(), pt, 150, 7, FALSE, TRUE);
			//}
			//else  if((1==m_wChiHuType)&&(m_wChiHuRight[byViewID] & CHR_QIANG_GANG))
			//{//杠上开花（自摸）
			//	if (0==byViewID)
			//	{
			//		pt.x=334;
			//		pt.y=43;
			//	}
			//	else if (1==byViewID)
			//	{
			//		pt.x=646;
			//		pt.y=200;
			//	}
			//	else if (2==byViewID)
			//	{
			//		pt.x=305;
			//		pt.y=378;
			//	}
			//	else
			//	{
			//		pt.x=75;
			//		pt.y=186;
			//	}
			//	//MessageBox("杠上开花");
			//	//m_ChiHuKindFlashGang.InitFlash(strPath+"ActGangShangHua.png", 23, pt);
			//	//m_ChiHuKindFlashGang.Play(true, 0);
			//	CThreadAnimationManager::Play((strPath+"ActGangShangHua.png").AllocSysString(), pt, 150, 23, FALSE, TRUE);
			//}
			//else 
			if(1==m_wChiHuType)
			{//平胡自摸
				//MessageBox("平胡自摸");
				if (0==byViewID)
				{
					pt.x=302;
					pt.y=69;
				}
				else if (1==byViewID)
				{
					pt.x=603;
					pt.y=205;
				}
				else if (2==byViewID)
				{
					pt.x=274;
					pt.y=396;
				}
				else
				{
					pt.x=54;
					pt.y=205;
				}
				//m_ChiHuKindFlashZiMo.InitFlash(strPath+"ActZiMo.png", 9, pt);
				//m_ChiHuKindFlashZiMo.Play(true, 0);
				CThreadAnimationManager::Play((strPath+"ActZiMo.png").AllocSysString(), pt, 150, 8, FALSE, TRUE);//by han 20120109
			}
			else  if(2==m_wChiHuType)
			{//平胡胡牌
				//MessageBox("平胡胡牌");
				if (0==byViewID)
				{
					pt.x=355;
					pt.y=26;
				}
				else if (1==byViewID)
				{
					pt.x=696;
					pt.y=240;
				}
				else if (2==byViewID)
				{
					pt.x=328;
					pt.y=410;
				}
				else
				{
					pt.x=63;
					pt.y=240;
				}
				//m_ChiHuKindFlashDianPao.InitFlash(strPath+"ActPingHu.png", 15, pt);
				//m_ChiHuKindFlashDianPao.Play(true, 0);
				CThreadAnimationManager::Play((strPath+"ActPingHu.png").AllocSysString(), pt, 150, 17, FALSE, TRUE);//by han 20120109
				
				CString strBuffer;
				strBuffer.Format("client: CGameClientView::PlayChiHuFlash(BYTE byViewID) %d", byViewID);
				OutputDebugString(strBuffer);
			}
		}
		else //通炮（一炮三响）
		{
			//MessageBox("通炮");
			//m_ChiHuKindFlashTongPao.InitFlash(strPath+"ActTongPao.png", 11, CPoint(376,242));
			//m_ChiHuKindFlashTongPao.Play(true, 0);
			CThreadAnimationManager::Play((strPath+"ActTongPao.png").AllocSysString(), CPoint(376,242), 150, 11, FALSE, TRUE);
		}
	}
	else
	{//荒庄
		//MessageBox("荒庄");
		//m_HuangZhuangFlash.InitFlash(strPath+"LiuJu.png", 6, CPoint(375,300));
		//m_HuangZhuangFlash.Play(true, 0);
		CThreadAnimationManager::Play((strPath+"LiuJu.png").AllocSysString(), CPoint(375,300), 150, 6, FALSE, TRUE);
	}
}


void CGameClientView::InitViewFrame()
{
	m_OutCardFlash.Create(IDD_AI_PROMPT,this);
	//m_TingCardFlash.Create(IDD_AI_PROMPT,this);
	m_CardHuPaiList.Create(IDD_AI_PROMPT,this);
	m_GameOperationFlash.Create(IDD_AI_PROMPT, this);

	//m_ChiHuKindFlashZiMo.Create(IDD_AI_PROMPT, this);
	//m_ChiHuKindFlashDianPao.Create(IDD_AI_PROMPT, this);
	//m_ChiHuKindFlashFangPao.Create(IDD_AI_PROMPT, this);
	//m_ChiHuKindFlashTongPao.Create(IDD_AI_PROMPT, this);
	//m_ChiHuKindFlashHaidi.Create(IDD_AI_PROMPT, this);
	//m_ChiHuKindFlashGang.Create(IDD_AI_PROMPT, this);

	CThreadAnimationManager::Init(this);
	
	//m_ChiHuKindFlashPengPeng.Create(IDD_AI_PROMPT, this);
	//m_ChiHuKindFlashQingYiSe.Create(IDD_AI_PROMPT, this);
	//m_ChiHuKindFlashQiXiaoDui.Create(IDD_AI_PROMPT, this);
	//m_ChiHuKindFlashJiangJiang.Create(IDD_AI_PROMPT, this);
	//m_HuangZhuangFlash.Create(IDD_AI_PROMPT, this);

	bool bRect = m_CardHuPaiList.LoadImage("SparrowZZ\\Picture\\Card\\");

	CString strPath="";
	GetPicPath(strPath);
	strPath.Append("Cartoon\\");
	CPoint pt(0,0);
	m_OutCardFlash.InitFlash(strPath+"OUTCARDHIDE.png", 9, pt);
}

void CGameClientView::OnDrawMahjong(CDC *pDC)
{
	//桌面扑克
	for (WORD i=0;i<4;i++)
	{
		m_TableCard[i].DrawCardControl(pDC);
		m_DiscardCard[i].DrawCardControl(pDC);
		m_WeaveCard[i][0].DrawCardControl(pDC);
		m_WeaveCard[i][1].DrawCardControl(pDC);
		m_WeaveCard[i][2].DrawCardControl(pDC);
		m_WeaveCard[i][3].DrawCardControl(pDC);
	}

	//堆积扑克
	m_HeapCard[3].DrawCardControl(pDC);
	m_HeapCard[0].DrawCardControl(pDC);
	m_HeapCard[1].DrawCardControl(pDC);
	m_HeapCard[2].DrawCardControl(pDC);

	//用户扑克
	m_UserCard[0].DrawCardControl(pDC);
	m_UserCard[1].DrawCardControl(pDC);
	m_UserCard[2].DrawCardControl(pDC);
	m_HandCardControl.DrawCardControl(pDC);
}

void CGameClientView::OnDrawLastMahjong(CDC *pDC)
{
	if (m_cbCardData>0)
	{
		//用户状态
		for (WORD i=0;i<GAME_PLAYER;i++)
		{
			//变量定义
			const tagUserData * pUserData=GetUserInfo(i);
			if (NULL==pUserData)
			{
				continue;
			}

			if (m_wOutCardUser==i)   //出牌用户
			{
				//绘画动作，动作背景
				int nWidth = m_ImageOutCardBack.GetWidth()/2;
				int nHeight = m_ImageOutCardBack.GetHeight();
				if (m_bIsShowOutCardFlash)	//有背景
				{
					m_ImageOutCardBack.DrawImage(pDC,m_ptLastMahjong[i].x, m_ptLastMahjong[i].y,nWidth,nHeight,nWidth,0,nWidth,nHeight);
				}
				else	//没有背景
				{
					m_ImageOutCardBack.DrawImage(pDC,m_ptLastMahjong[i].x+3, m_ptLastMahjong[i].y,nWidth,nHeight,0,0,nWidth,nHeight);//by han 20120106

				}

				//绘画扑克
				BYTE byIndex = m_CardControl.GetImageIndex(m_cbCardData);
				m_CardControl.m_ImageOutCardNum.DrawImage(pDC,m_ptLastMahjong[i].x+52, m_ptLastMahjong[i].y+50,43,58,43*byIndex,0,43,58);

			}
		}
	}
}

void CGameClientView::OnDrawMuscle(CDC *pDC)
{
	//用户状态
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		//变量定义
		const tagUserData * pUserData=GetUserInfo(i);
		if (NULL==pUserData)
		{
			continue;
		}

		// 筋动画 金币数字
		if (0 != m_byShowMuscle%2  &&  0!=m_llGoldMuscle[i])
		{
			m_ImageGoldChar.DrawImage(pDC, m_rcMuscleNum[i].left-m_ImageGoldChar.GetWidth()/2, m_rcMuscleNum[i].top, 
				m_ImageGoldChar.GetWidth()/2, m_ImageGoldChar.GetHeight(),
				(0 < m_llGoldMuscle[i])?0:m_ImageGoldChar.GetWidth()/2, 0, m_ImageGoldChar.GetWidth()/2, m_ImageGoldChar.GetHeight());
			if (0 < m_llGoldMuscle[i])
			{
				m_INMuscleNumGreen.SetNumAlign(IA_LEFT);
				m_INMuscleNumGreen.SetShowRect(m_rcMuscleNum[i]);
				m_INMuscleNumGreen.SetNums(m_llGoldMuscle[i]);
				m_INMuscleNumGreen.DrawImageNums(pDC);
			}
			else if (0 > m_llGoldMuscle[i])
			{
				m_INMuscleNumGray.SetNumAlign(IA_LEFT);
				m_INMuscleNumGray.SetShowRect(m_rcMuscleNum[i]);
				m_INMuscleNumGray.SetNums(m_llGoldMuscle[i]);
				m_INMuscleNumGray.DrawImageNums(pDC);
			}
		}
	}
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//启动方位动画
void CGameClientView::PlayDirectFlash(bool bIsPlay)
{
	m_bIsPlayDirect=bIsPlay;		
	m_iCountDirectIndex=0;			
	m_rcDirectFlashDraw.left = 160;
	m_rcDirectFlashDraw.top = 166;
	m_rcDirectFlashDraw.right = m_rcDirectFlashDraw.left + I_DIRECT_FLASH_WIDTH;
	m_rcDirectFlashDraw.bottom = m_rcDirectFlashDraw.top + I_DIRECT_FLASH_HEIGHT;

	KillTimer(IDI_DIRECT_EFFECT);
	if (bIsPlay)
	{
		SetTimer(IDI_DIRECT_EFFECT, IDI_TIME_DIRECT_FLASH_INTERCAL, NULL);
	}

	UpdateGameView(&m_rcDirectFlashDraw);
}

//启动色子动画
void CGameClientView::PlaySZFlashBT(bool bIsPlay)
{
	m_bIsOpenSZFlash=bIsPlay;		
	m_iCountSZFCurIndex=0;			
	m_rcSZFlashDraw.left = 380;
	m_rcSZFlashDraw.top = 180;
	m_rcSZFlashDraw.right = m_rcSZFlashDraw.left + I_SZ_FLASH_WIDTH;
	m_rcSZFlashDraw.bottom = m_rcSZFlashDraw.top + I_SZ_FLASH_HEIGHT;

	m_rcSZBTRect.left = m_rcSZFlashDraw.left+5;
	m_rcSZBTRect.top = m_rcSZFlashDraw.top + 85;
	m_rcSZBTRect.right = m_rcSZBTRect.left + 163;
	m_rcSZBTRect.bottom = m_rcSZBTRect.top + 152;

	KillTimer(IDI_SZFLASH_EFFECT);
	if (bIsPlay)
	{
		SetTimer(IDI_SZFLASH_EFFECT, IDI_TIME_SZ_FLASH_INTEVAL, NULL);
	}
	else
	{
		m_rcSZBTRect.SetRect(0, 0, 0, 0);
	}
	UpdateGameView(&m_rcSZFlashDraw);
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//启动筛子动画
void CGameClientView::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	
	CGameFrameView::OnLButtonUp(nFlags, point);

	if (m_bIsOpenSZFlash)
	{
		if (m_rcSZBTRect.PtInRect(point))
		{
			AfxGetMainWnd()->SendMessage(IDM_SZ_BUTTON, 0, 0);
		}
	}
}

// 安全显示或隐藏窗口
void CGameClientView::SafeShowOrHideWnd(CWnd* pWnd, int nCmdShow)
{
	if(pWnd && pWnd->GetSafeHwnd())
	{
		pWnd->ShowWindow(nCmdShow);
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////