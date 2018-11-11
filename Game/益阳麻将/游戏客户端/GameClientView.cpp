#include "StdAfx.h"
#include "Resource.h"
#include "GameClientView.h"
#include "GameClientDlg.h"
#include ".\gameclientview.h"
#include <math.h>
#include "TraceCatch.h"
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
	ON_BN_CLICKED(IDC_HAI_DI, OnHaiDi)
	ON_BN_CLICKED(IDC_NO_HAI_DI, OnNoHaiDi)
	ON_BN_CLICKED(IDC_AUTOPLAY_ON, OnAutoPlayerOn)
	ON_BN_CLICKED(IDC_AUTOPLAY_OFF,OnAutoPlayerOff)
	ON_BN_CLICKED(IDC_TRUSTEE_CONTROL,OnStusteeControl)
	ON_BN_CLICKED(IDC_TING_CARD,OnTingCard)
	ON_BN_CLICKED(IDC_NO_TING_CARD, OnNoTingCard)
	ON_WM_TIMER()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//构造函数
CGameClientView::CGameClientView()
{
__ENTER_FUNCTION
	//标志变量
	m_cbHaiDiCard=0;
	m_bOutCard=false;
	m_bWaitOther=false;
	m_bIsInitBT=false;
	m_bKingCard=false;
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
	

	m_ImageBack.LoadImage(TEXT(strPath+ "VIEW_BACK.png"));
	m_ImageWait.LoadImage(TEXT(strPath+"WAIT_TIP.png"));
	m_ImageUserFlag.LoadImage(TEXT(strPath+"USER_FLAG.png"));
	m_ImageDirectionBack.LoadImage(TEXT(strPath+"DIRECTIONBACK.png"));
	m_ImageUserBankerFlag.LoadImage(TEXT(strPath+"BANKER.png"));
	m_ImageOutCard.LoadImage(TEXT(strPath+"OUT_CARD_TIP.png"));
	m_ImageHuangZhuang.LoadImage(TEXT(strPath+"HUANG_ZHUANG.png"));
	m_ImageMPEffect.LoadImage(TEXT(strPath+"MOPAI.png"));
	m_ImageTrustee.LoadImage(TEXT(strPath+"TRUSTEE.png"));
	//m_ImageMPBACK.LoadImage(TEXT(strPath+"BACKMP.png"));
	m_ImageMPCARD.LoadImage(TEXT(strPath+"CARD.png"));
	m_ImageTingFlag.LoadImage(TEXT(strPath+"TING_FLAG.png"));
	m_ImageUserGradeNum.LoadImage(TEXT(strPath+"GRADENUM.png"));
	m_ImageOutCardBack.LoadImage(TEXT(strPath+"OUTCARDBACK.png"));
	m_ImageKingCardBack.LoadImage(TEXT(strPath+"KINGCARDBACK.png"));
	m_ImageKingCardNum.LoadImage(TEXT(strPath+"KINGCARDNUM.png"));
	m_ActionSZFlash.LoadImage(TEXT(strPath+"SZPROMPT.png"));
	m_ActionDirectFlash.LoadImage(TEXT(strPath+"ACTIONDIRECT.png"));



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
	///////////////////////////////////////////
	//开始动画
	//m_ActionStart.LoadImage(TEXT(strPath+"Start.png"));
	///////////////////////////////////////////


	//方位动画参数初始化
	m_bIsPlayDirect=false;
	m_iCountDirectIndex=0;
	m_rcDirectFlashDraw=CRect(0, 0, 0, 0);	

	//启动筛子参数初始化
	m_bIsOpenSZFlash=false;		
	m_iCountSZFCurIndex=0;			
	m_rcSZFlashDraw=CRect(0, 0, 0, 0);	
	m_rcSZBTRect=CRect(0, 0, 0, 0);	

__LEAVE_FUNCTION
	return;
}

//析构函数
CGameClientView::~CGameClientView(void)
{
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//获取路径
void CGameClientView::GetPicPath(CString& strPicPath)
{
	strPicPath="";
	TCHAR szResourcePath[MAX_PATH]={0};
	GetCurrentDirectory(MAX_PATH,szResourcePath);
	strPicPath.Append(szResourcePath);
	strPicPath.Append(TEXT("\\SparrowYY\\Picture\\PNG\\"));
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//建立消息
int CGameClientView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
__ENTER_FUNCTION
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
	m_btHaiDi.Create(NULL,WS_CHILD,rcCreate,this,IDC_HAI_DI);
	m_btNoHaiDi.Create(NULL,WS_CHILD,rcCreate,this,IDC_NO_HAI_DI);
	m_btTingCard.Create(NULL,WS_CHILD,rcCreate,this,IDC_TING_CARD);
	m_btBuTing.Create(NULL,WS_CHILD,rcCreate,this,IDC_NO_TING_CARD);

	m_btStart.SetButtonImage(TEXT(strPath+"BEGIN.png"),false,5);
	m_btAutoPlayOn.SetButtonImage(TEXT(strPath+"AUTOPLAYON.png"),false,5);
	m_btAutoPlayOff.SetButtonImage(TEXT(strPath+"AUTOPLAYOFF.png"),false,5);
	m_btHaiDi.SetButtonImage(TEXT(strPath+"HAIDI.png"),false,5);
	m_btNoHaiDi.SetButtonImage(TEXT(strPath+"NO_HAIDI.png"),false,5);
	m_btTingCard.SetButtonImage(TEXT(strPath+"TING.png"),false,5);
	m_btBuTing.SetButtonImage(TEXT(strPath+"NO_TING.png"),false,5);
	//托管按钮
	//m_btStusteeControl.Create(TEXT(""),WS_CHILD|WS_VISIBLE,rcCreate,this,IDC_TRUSTEE_CONTROL);
	//m_btStusteeControl.LoadRgnImage(AfxGetInstanceHandle(),IDB_BT_START_TRUSTEE,RGB(255,0,255),5);


	//BYTE cbData[3]={0x01,0x21,0x18};
	//SetCanHuPaiData(cbData, 3, 300);
	//UpdateGameView(NULL);
__LEAVE_FUNCTION
	return 0;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//重置界面
void CGameClientView::ResetGameView()
{
	//标志变量
	m_cbHaiDiCard=0;
	m_bOutCard=false;
	m_bWaitOther=false;
	m_bIsInitBT=false;
	m_bKingCard=false;
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
	m_ControlWnd.ShowWindow(SW_HIDE);
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
	m_HandCardControl.SetCardData(NULL,0,0,NULL);
	//王牌
	m_KingCard.SetCardData(NULL,0);

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
__ENTER_FUNCTION
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
	m_ptFace[0].x=415;
	m_ptFace[0].y=-13;
	m_ptName[0].x=nWidth/2+270;
	m_ptName[0].y=m_nYBorder+112;

	m_ptFace[1].x=730;
	m_ptFace[1].y=77;
	m_ptName[1].x=nWidth-m_nXBorder-80;
	m_ptName[1].y=nHeight/2-135;

	m_ptFace[2].x=7;
	m_ptFace[2].y=475-19;
	m_ptName[2].x=27;
	m_ptName[2].y=nHeight-m_nYBorder-82;

	m_ptFace[3].x=0;
	m_ptFace[3].y=77;
	m_ptName[3].x=38;
	m_ptName[3].y=nHeight/2-135;

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
	m_ptTimer[2].x=900;
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

	//方位坐标  南西北
	//m_UserFlagPos[0].x=402;
	//m_UserFlagPos[0].y=50;
	//m_UserFlagPos[1].x=864;
	//m_UserFlagPos[1].y=137;
	//m_UserFlagPos[2].x=250;
	//m_UserFlagPos[2].y=660;
	//m_UserFlagPos[3].x=54;
	//m_UserFlagPos[3].y=137;


	//庄家坐标
	m_UserBankerFlagPos[0].x=517;
	m_UserBankerFlagPos[0].y=26;
	m_UserBankerFlagPos[1].x=900;
	m_UserBankerFlagPos[1].y=110;
	m_UserBankerFlagPos[2].x=9;
	m_UserBankerFlagPos[2].y=490;
	m_UserBankerFlagPos[3].x=10;
	m_UserBankerFlagPos[3].y=110;

	//听牌标记坐标
	m_UserTingFlagPos[0].x=m_PointTrustee[0].x+89;
	m_UserTingFlagPos[0].y=m_PointTrustee[0].y-45;
	m_UserTingFlagPos[1].x=m_PointTrustee[1].x+90;
	m_UserTingFlagPos[1].y=m_PointTrustee[1].y-47;
	m_UserTingFlagPos[2].x=m_PointTrustee[2].x+90;
	m_UserTingFlagPos[2].y=m_PointTrustee[2].y-47;
	m_UserTingFlagPos[3].x=m_PointTrustee[3].x+88;
	m_UserTingFlagPos[3].y=m_PointTrustee[3].y-46;

	//用户扑克(立牌)
	m_UserCard[0].SetControlPoint(nWidth/2-180,m_nYBorder+155);
	m_UserCard[1].SetControlPoint(nWidth-m_nXBorder-230,nHeight/2-150);
	m_UserCard[2].SetControlPoint(m_nXBorder+102,nHeight/2-125);
	m_HandCardControl.SetBenchmarkPos(nWidth/2,nHeight-m_nYBorder-135,enXCenter,enYBottom); 

	//王牌位置
	m_KingCard.SetControlPoint(325,335);

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
	m_WeaveCard[2][0].SetControlPoint(0, nWidth/2-360,nHeight-m_nYBorder-195);
	m_WeaveCard[2][1].SetControlPoint(1, nWidth/2-270,nHeight-m_nYBorder-195);
	m_WeaveCard[2][2].SetControlPoint(2, nWidth/2-180,nHeight-m_nYBorder-195);
	m_WeaveCard[2][3].SetControlPoint(3, nWidth/2-90,nHeight-m_nYBorder-195);

	//组合扑克
	m_WeaveCard[3][0].SetControlPoint(0, m_nXBorder+204,nHeight/2-120);
	m_WeaveCard[3][1].SetControlPoint(1, m_nXBorder+182,nHeight/2-60);
	m_WeaveCard[3][2].SetControlPoint(2, m_nXBorder+155,nHeight/2+10);
	m_WeaveCard[3][3].SetControlPoint(3, m_nXBorder+125,nHeight/2+90);


	//控制窗口
	m_ControlWnd.SetBenchmarkPos(nWidth/2+150,nHeight-m_nYBorder-210);


	//m_FlashControl.SetBenchmarkPos(nWidth/2,nHeight-m_nYBorder-160);

	//移动按钮
	CRect rcButton;
	HDWP hDwp=BeginDeferWindowPos(32);
	m_btStart.GetWindowRect(&rcButton);
	const UINT uFlags=SWP_NOACTIVATE|SWP_NOZORDER|SWP_NOCOPYBITS|SWP_NOSIZE;

	//移动调整
	DeferWindowPos(hDwp,m_btStart,NULL,402,329-29,0,0,uFlags);
	DeferWindowPos(hDwp,m_btHaiDi,NULL,595,487-29,0,0,uFlags);
	DeferWindowPos(hDwp,m_btNoHaiDi,NULL,661,487-29,0,0,uFlags);
	DeferWindowPos(hDwp,m_btAutoPlayOn,NULL,571,616-29,0,0,uFlags);
	DeferWindowPos(hDwp,m_btAutoPlayOff,NULL,571,616-29,0,0,uFlags);
	DeferWindowPos(hDwp,m_btTingCard,NULL,403,616-29,0,0,uFlags);
	DeferWindowPos(hDwp,m_btBuTing,NULL,469,616-29,0,0,uFlags);
	//移动调整
	DeferWindowPos(hDwp,m_btStusteeControl,NULL,nWidth-m_nXBorder-(rcButton.Width()+5),nHeight-m_nYBorder-rcButton.Height()-180,0,0,uFlags);
	//结束移动
	EndDeferWindowPos(hDwp);

	RectifyKeepTimePos(735, 539);

	//for(WORD i=0;i<GAME_PLAYER;i++)
	//{
	//	if(i!=3&&i!=2) SetFlowerControlInfo(i,m_ptFace[i].x, m_ptFace[i].y);
	//	else if(i==2) SetFlowerControlInfo(i,m_ptFace[i].x, m_ptFace[i].y-110);
	//	else SetFlowerControlInfo(i,m_ptFace[i].x-70, m_ptFace[i].y);
	//}

	//BYTE cbCardData[MAX_INDEX]={0x12,0x18,0x27,0x27,0x27,0x03,0x04,0x05,0x11,0x12,0x13,0x16,0x17};
	//m_HandCardControl.SetCardData(cbCardData,13,0x29,0x01);
	//m_HandCardControl.SetDisplayItem(true);
	//m_HandCardControl.SetPositively(true);

#define  MJ_H_FULL_COUNT (28)
#define  MJ_V_FULL_COUNT (26)
	//m_HeapCard[0].SetCardData(24,8,34);//top
	//m_HeapCard[1].SetCardData(0,6,34);//right
	//m_HeapCard[2].SetCardData(26,8,34);//bottom
	//m_HeapCard[3].SetCardData(1,8,34);//left

#define   mCardCount  14
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


#define   mTCardCount  14
	BYTE cbTCardData[11]={0x03,0x04,0x05,0x27,0x03,0x04,0x05,0x03,0x04,0x05};

	//m_TableCard[0].SetCardData(cbTCardData,10);
	//m_TableCard[1].SetCardData(cbTCardData,11);
	//m_TableCard[2].SetCardData(cbTCardData,13);
	//m_TableCard[3].SetCardData(cbTCardData,11);
	//m_btTingCard.ShowWindow(SW_SHOW);
	//m_btBuTing.ShowWindow(SW_SHOW);

	//BYTE cbData[2]={0x21,0x18};
	//m_CardHuPaiList.SetCardData(cbData,2);
	//m_CardHuPaiList.ShowWindow(SW_SHOW);
__LEAVE_FUNCTION
	return;
}


////////动画///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//骰子动画1
void CGameClientView::ActionDice1( CDC *pDC, int nFrame )
{
__ENTER_FUNCTION
	LONG lWidth, lHeight;
	lWidth = m_csActionDice1.cx;
	lHeight = m_csActionDice1.cy;
	m_ActionDice1.DrawImage( pDC, PS_BX_DICE, PS_BY_DICE, lWidth, lHeight, lWidth*nFrame, 0);
__LEAVE_FUNCTION
}
//骰子动画2
void CGameClientView::ActionDice2( CDC *pDC,int pos1, int pos2 )
{
__ENTER_FUNCTION
	LONG lWidth, lHeight;
	lWidth = m_csActionDice2.cx;
	lHeight = m_csActionDice2.cy;

	m_ActionDice2.DrawImage( pDC, PS_BX_DICE1, PS_BY_DICE1, lWidth, lHeight, lWidth*pos1, 0);
	m_ActionDice2.DrawImage( pDC, PS_BX_DICE2, PS_BY_DICE2, lWidth, lHeight, lWidth*pos2, 0);
__LEAVE_FUNCTION
}
//摸牌动画
void CGameClientView::ActionMP( CDC *pDC, int nFrame)
{
__ENTER_FUNCTION
	if (nFrame>=MP_EFFECT_COUNT)
	{
		nFrame = MP_EFFECT_COUNT;
	}
	//绘画摸牌背景
	//INT nHeight=m_ImageMPBACK.GetHeight();
	//INT nWidth=m_ImageMPBACK.GetWidth();
	//m_ImageMPBACK.DrawImage(pDC,340,190,nWidth,nHeight,0,0);

	//绘画效果
	if (nFrame < MP_EFFECT_COUNT)
	{
		INT nImageHeight=m_ImageMPEffect.GetHeight();
		INT nImageWidth=m_ImageMPEffect.GetWidth()/MP_EFFECT_COUNT;
		m_ImageMPEffect.DrawImage(pDC,340,190,nImageWidth,nImageHeight,
			nImageWidth*(nFrame%MP_EFFECT_COUNT),0);
	}

	if (nFrame == MP_EFFECT_COUNT)
	{
		INT nImageHeight=m_ImageMPCARD.GetHeight();
		INT nImageWidth=m_ImageMPCARD.GetWidth()/27;
		m_ImageMPCARD.DrawImage(pDC,340,190,nImageWidth,nImageHeight,
			nImageWidth*m_nKingCardIndex,0);		
	}
__LEAVE_FUNCTION
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//绘画界面
void CGameClientView::DrawGameView(CDC * pDC, int nWidth, int nHeight)
{
__ENTER_FUNCTION
	//绘画背景
	m_ImageBack.DrawImage(pDC,0,0,960,691,0,0,960,691);

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
			DrawCustomFace(pDC,i, pUserData, m_ptFace[i].x,m_ptFace[i].y);


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
				pDC->TextOut(m_Info[i].x+37,m_Info[i].y+28,pUserData->chGradeName,strlen(pUserData->chGradeName));
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


	//王牌扑克
	if (true == m_bKingCard)
	{
		m_KingCard.DrawCardControl(pDC);
	}

	//听牌标识
	for (int i=0;i<GAME_PLAYER;i++)
	{
		//变量定义
		const tagUserData * pUserData=GetUserInfo(i);
		if (NULL==pUserData)
		{
			continue;
		}
		if (true==m_bTingFlag[i])
		{
			//计算位置
			int nXPos=0,nYPos=0;
			int nTingWidth=m_ImageTingFlag.GetWidth();
			int nTingHeight=m_ImageTingFlag.GetHeight();
			switch (i)
			{
			case 0:	//北向
				{
					nXPos=500;
					nYPos=32;
					break;
				}
			case 1:	//东向
				{
					nXPos=nWidth-nTingWidth-37;
					nYPos=121;
					break;
				}
			case 2:	//南向
				{
					nXPos=200;
					nYPos=540;
					break;
				}
			case 3:	//西向
				{
					nXPos=37;
					nYPos=121;
					break;
				}
			}
			m_ImageTingFlag.DrawImage(pDC,nXPos,nYPos,nTingWidth,nTingHeight,0,0,nTingWidth,nTingHeight);
		}
	}


	//等待提示
	if (m_bWaitOther==true)
	{
		m_ImageWait.DrawImage(pDC,(nWidth-m_ImageWait.GetWidth())/2,nHeight-150-30);
	}

	//荒庄标志
	if (true == m_bIsHuangZhuang && m_cbHaiDiCard!=0)                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                    
	{
		BYTE byIndex = m_CardControl.GetImageIndex(m_cbHaiDiCard);
		m_CardControl.m_ImageUserBottom.DrawImage(pDC,450,450,48,75,byIndex*48,0,48,75);
		m_ImageHuangZhuang.DrawImage(pDC,390,411,180,45,0,0,180, 45);
	}

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
				//计算位置
				int nXPos=0,nYPos=0;
				switch (i)
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

				//绘画动作
				{//动作背景
					int nWidth = m_ImageOutCardBack.GetWidth()/2;
					int nHeight = m_ImageOutCardBack.GetHeight();
					if (m_bIsShowOutCardFlash)
					{//有背景
						m_ImageOutCardBack.DrawImage(pDC,nXPos,nYPos,nWidth,nHeight,nWidth,0,nWidth,nHeight);
					}
					else
					{//没有背景
						m_ImageOutCardBack.DrawImage(pDC,nXPos,nYPos,nWidth,nHeight,0,0,nWidth,nHeight);
					}

					//绘画扑克
					BYTE byIndex = m_CardControl.GetImageIndex(m_cbCardData);
					m_CardControl.m_ImageOutCardNum.DrawImage(pDC,nXPos+53,nYPos+50,43,58,43*byIndex,0,43,58);
				}
			}
		}
	}
__LEAVE_FUNCTION

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

	return;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//状态标志
void CGameClientView::SetStatusFlag(bool bOutCard, bool bWaitOther)
{
	//设置变量
	m_bOutCard=bOutCard;
	m_bWaitOther=bWaitOther;

	//更新界面
	UpdateGameView(NULL);

	return;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//出牌信息
void CGameClientView::SetOutCardInfo(WORD wViewChairID, BYTE cbCardData)
{
__ENTER_FUNCTION
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
__LEAVE_FUNCTION
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
__ENTER_FUNCTION
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
__LEAVE_FUNCTION
	return;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//光标消息
BOOL CGameClientView::OnSetCursor(CWnd * pWnd, UINT nHitTest, UINT uMessage)
{
__ENTER_FUNCTION
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
		rect.right = rect.left + 800;
		rect.bottom = rect.top + 90;
		UpdateGameView(&rect);
	}

	//光标控制
	if (bHandle==false) __super::OnSetCursor(pWnd,nHitTest,uMessage);
__LEAVE_FUNCTION
	return TRUE;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//鼠标消息
void CGameClientView::OnLButtonDown(UINT nFlags, CPoint Point)
{
	__super::OnLButtonDown(nFlags, Point);


	//获取扑克
	BYTE cbHoverCard=m_HandCardControl.GetHoverCard();
	if (cbHoverCard!=0) AfxGetMainWnd()->PostMessage(IDM_OUT_CARD,cbHoverCard,cbHoverCard);

	m_CardHuPaiList.ShowWindow(SW_HIDE);

	return;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//骰子动画
bool CGameClientView::SetDiceEffect(bool bDiceEffect)
{
__ENTER_FUNCTION
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
__LEAVE_FUNCTION
	return true;
}

//设置摸牌 2011.8.3
bool CGameClientView::SetMPEffect(bool bMPEffect)
{
__ENTER_FUNCTION
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
__LEAVE_FUNCTION
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//时间消息 2011.8.3
VOID CGameClientView::OnTimer(UINT nIDEvent)
{
__ENTER_FUNCTION
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

	//出牌显示
	else if (nIDEvent==IDI_SHOW_CARD)
	{
		m_bIsShowOutCardFlash = !m_bIsShowOutCardFlash;
		UpdateGameView(&m_rcShowDrawArea);
	}
__LEAVE_FUNCTION
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//开始按钮
void CGameClientView::OnStart()
{
	//发送消息
	AfxGetMainWnd()->SendMessage(IDM_START,0,0);
	return;
}

//海底按钮
void CGameClientView::OnHaiDi()
{
	//发送消息
	AfxGetMainWnd()->SendMessage(IDM_OPERATE_HAI_DI,1,1);

	return;
}

//放弃按钮
void CGameClientView::OnNoHaiDi()
{
	//发送消息
	AfxGetMainWnd()->SendMessage(IDM_OPERATE_HAI_DI,0,0);

	return;
}
//拖管控制
void CGameClientView::OnStusteeControl()
{
	AfxGetMainWnd()->SendMessage(IDM_TRUSTEE_CONTROL,0,0);
	return;
}

//听牌控制
void CGameClientView::OnTingCard()
{
	AfxGetMainWnd()->SendMessage(IDM_TING_CARD,1,0);
	return;
}

//不听按钮
void CGameClientView::OnNoTingCard()
{
	AfxGetMainWnd()->SendMessage(IDM_TING_CARD,0,0);
}


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
		m_CardHuPaiList.ShowWindow(SW_SHOW);
	}
	else
	{
		m_CardHuPaiList.SetCardData(NULL, 0);
		m_CardHuPaiList.ShowWindow(SW_HIDE);
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
	m_iSaveCheckPaiIndex = ibHandle;
	if (ibHandle < 0)
	{
		m_HandCardControl.InitCurCardDataX();
		m_CardHuPaiList.ShowWindow(SW_HIDE);
		UpdateGameView(NULL);
	}
	else if (ibHandle>=0 && ibHandle<=13)
	{
		BYTE byCardData=0;
		int iX = 0;
		m_HandCardControl.GetCurCardDataAndX(byCardData, iX);
		if (0 != byCardData)
		{
			m_pGameClientDlg->CheckCanHuPai(byCardData, iX);
		}
	}
	CGameFrameView::OnMouseMove(nFlags, point);
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//启动听牌动画
void CGameClientView::PlayTingCardFlash(BYTE byViewID)
{
__ENTER_FUNCTION
	CString strPath="";
	GetPicPath(strPath);
	strPath.Append("Cartoon\\");
	CPoint pt;
	pt.x = 0;
	pt.y = 0; 
	if (0==byViewID)
	{
		pt.x=373;
		pt.y=32;
	}
	else if (1==byViewID)
	{
		pt.x=678;
		pt.y=191;
	}
	else if (2==byViewID)
	{
		pt.x=357;
		pt.y=377-30;
	}
	else
	{
		pt.x=74;
		pt.y=182;
	}
	m_TingCardFlash.InitFlash(strPath+"ActTing.png", 10, pt);
	m_TingCardFlash.Play(true, 0);
__LEAVE_FUNCTION

}

//播放操作动画
void CGameClientView::PlayOperationFlash(BYTE byViewID,BYTE byUserAction)
{
__ENTER_FUNCTION
	m_GameOperationFlash.Play(false);

	CString strPath="";
	GetPicPath(strPath);
	strPath.Append("Cartoon\\");
	CPoint pt;
	pt.x = 0;
	pt.y = 0; 
	if (byUserAction&WIK_TING) 
	{
		if (0==byViewID)
		{
			pt.x=373;
			pt.y=32;
		}
		else if (1==byViewID)
		{
			pt.x=678;
			pt.y=191;
		}
		else if (2==byViewID)
		{
			pt.x=357;
			pt.y=377-30;
		}
		else
		{
			pt.x=74;
			pt.y=182;
		}
		//MessageBox("杠");
		m_GameOperationFlash.InitFlash(strPath+"ActGang.png", 13, pt);
		m_GameOperationFlash.Play(true, 0);
	} 
	else if (WIK_PENG&byUserAction)
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
		//MessageBox("碰");
		m_GameOperationFlash.InitFlash(strPath+"ActPeng.png", 12, pt);
		m_GameOperationFlash.Play(true, 0);
	}
	else if ((WIK_LEFT&byUserAction)||(WIK_CENTER&byUserAction)||(WIK_RIGHT&byUserAction))
	{
		if (0==byViewID)
		{
			pt.x=293;
			pt.y=109;
		}
		else if (1==byViewID)
		{
			pt.x=602;
			pt.y=251;
		}
		else if (2==byViewID)
		{
			pt.x=290;
			pt.y=438-30;
		}
		else
		{
			pt.x=79;
			pt.y=238;
		}
		//MessageBox("吃");
		m_GameOperationFlash.InitFlash(strPath+"ActChi.png", 12, pt);
		m_GameOperationFlash.Play(true, 0);
	}
	else if ((m_cbUserAction[byViewID]&WIK_FILL) || (m_cbUserAction[byViewID]&WIK_GANG))
	{
		if (0==byViewID)
		{
			pt.x=346;
			pt.y=193;
		}
		else if (1==byViewID)
		{
			pt.x=616;
			pt.y=292;
		}
		else if (2==byViewID)
		{
			pt.x=344;
			pt.y=423;
		}
		else
		{
			pt.x=58;
			pt.y=275;
		}
		//MessageBox("补");
		m_GameOperationFlash.InitFlash(strPath+"ActBu.png", 10, pt);
		m_GameOperationFlash.Play(true, 0);
	}
__LEAVE_FUNCTION
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//播放胡牌动画
void CGameClientView::PlayFangPaoFlash(BYTE byViewID)
{
__ENTER_FUNCTION
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
	m_ChiHuKindFlashFangPao.InitFlash(strPath+"ActFangPao.png", 8, pt);
	m_ChiHuKindFlashFangPao.Play(true, 0);
__LEAVE_FUNCTION
}


//播放胡牌动画
void CGameClientView::PlayChiHuFlash(BYTE byViewID)
{
__ENTER_FUNCTION
	CString strPath="";
	GetPicPath(strPath);
	strPath.Append("Cartoon\\");
	CPoint pt(0,0);

	if (0!=m_wChiHuType)
	{
		if (3!=m_iChiHuNums && (m_cbUserAction[byViewID]&WIK_CHI_HU))
		{
			if(m_wChiHuRight[byViewID] & CHR_HAI_DI)
			{//海底
				if (4==m_wChiHuType)
				{//海底炮
					//MessageBox("海底炮");
					m_ChiHuKindFlashHaidi.InitFlash(strPath+"ActHaiDiPao.png", 8, CPoint(308,260));
				}
				else if(3==m_wChiHuType)
				{//海底捞月(自摸)
					//MessageBox("海底捞月");
					m_ChiHuKindFlashHaidi.InitFlash(strPath+"ActHaiDiLaoYue.png", 8, CPoint(200,110));
				}
				m_ChiHuKindFlashHaidi.Play(true, 0);
			}
			else  if((2==m_wChiHuType)&&(m_wChiHuRight[byViewID] & CHR_QIANG_GANG))
			{//抢杠胡（杠上炮）
				if (0==byViewID)
				{
					pt.x=328;
					pt.y=100;
				}
				else if (1==byViewID)
				{
					pt.x=664;
					pt.y=255;
				}
				else if (2==byViewID)
				{
					pt.x=300;
					pt.y=463;
				}
				else
				{
					pt.x=72;
					pt.y=238;
				}
				//MessageBox("抢杠胡");
				m_ChiHuKindFlashGang.InitFlash(strPath+"ActQiangGangHu.png", 7, pt);
				m_ChiHuKindFlashGang.Play(true, 0);
			}
			else  if((1==m_wChiHuType)&&(m_wChiHuRight[byViewID] & CHR_QIANG_GANG))
			{//杠上开花（自摸）
				if (0==byViewID)
				{
					pt.x=334;
					pt.y=43;
				}
				else if (1==byViewID)
				{
					pt.x=646;
					pt.y=200;
				}
				else if (2==byViewID)
				{
					pt.x=305;
					pt.y=378;
				}
				else
				{
					pt.x=75;
					pt.y=186;
				}
				//MessageBox("杠上开花");
				m_ChiHuKindFlashGang.InitFlash(strPath+"ActGangShangHua.png", 23, pt);
				m_ChiHuKindFlashGang.Play(true, 0);
			}
			else if(m_wChiHuKind[byViewID] & CHK_PENG_PENG)
			{//碰碰胡
				CPoint pt[GAME_PLAYER];
				pt[0].x = 133;
				pt[0].y = 81;
				pt[1].x = 564;
				pt[1].y = 222;
				pt[2].x = 131;
				pt[2].y = 442;
				pt[3].x = 28;
				pt[3].y = 215;
				if ((0==byViewID)||(2==byViewID))
				{
					m_ChiHuKindFlashPengPeng.InitFlash(strPath+"ActPengPengHuH.png", 13, pt[byViewID]);
				}
				else
				{
					m_ChiHuKindFlashPengPeng.InitFlash(strPath+"ActPengPengHuV.png", 13, pt[byViewID]);
				}
				m_ChiHuKindFlashPengPeng.Play(true, 0);
			}
			else  if(m_wChiHuKind[byViewID] & CHK_QI_XIAO_DUI)
			{//七小对
				if (0==byViewID)
				{
					pt.x=288;
					pt.y=100;
				}
				else if (1==byViewID)
				{
					pt.x=589;
					pt.y=247;
				}
				else if (2==byViewID)
				{
					pt.x=274;
					pt.y=461;
				}
				else
				{
					pt.x=44;
					pt.y=238;
				}
				//MessageBox("七小对");
				m_ChiHuKindFlashQiXiaoDui.InitFlash(strPath+"ActQiXiaoDui.png", 13, pt);
				m_ChiHuKindFlashQiXiaoDui.Play(true, 0);
			}
			else  if(m_wChiHuRight[byViewID] & CHR_QING_YI_SE)
			{//清一色
				if (0==byViewID)
				{
					pt.x=271;
					pt.y=29;
				}
				else if (1==byViewID)
				{
					pt.x=511;
					pt.y=197;
				}
				else if (2==byViewID)
				{
					pt.x=261;
					pt.y=367;
				}
				else
				{
					pt.x=24;
					pt.y=157;
				}
				//MessageBox("清一色");
				m_ChiHuKindFlashQingYiSe.InitFlash(strPath+"ActQingYiSe.png", 24, pt);
				m_ChiHuKindFlashQingYiSe.Play(true, 0);
			}
			else  if(m_wChiHuKind[byViewID] & CHK_JIANG_JIANG)
			{//将将胡
				if (0==byViewID)
				{
					pt.x=267;
					pt.y=28;
				}
				else if (1==byViewID)
				{
					pt.x=555;
					pt.y=218;
				}
				else if (2==byViewID)
				{
					pt.x=272;
					pt.y=408;
				}
				else
				{
					pt.x=1;
					pt.y=197;
				}
				//MessageBox("将将胡");
				m_ChiHuKindFlashJiangJiang.InitFlash(strPath+"ActJiangJiangHu.png", 12, pt);
				m_ChiHuKindFlashJiangJiang.Play(true, 0);
			}
			else  if(1==m_wChiHuType)
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
				m_ChiHuKindFlashZiMo.InitFlash(strPath+"ActZiMo.png", 9, pt);
				m_ChiHuKindFlashZiMo.Play(true, 0);
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
				m_ChiHuKindFlashDianPao.InitFlash(strPath+"ActPingHu.png", 15, pt);
				m_ChiHuKindFlashDianPao.Play(true, 0);
			}
		}
		else
		{//通炮（一炮三响）
			//MessageBox("通炮");
			m_ChiHuKindFlashTongPao.InitFlash(strPath+"ActTongPao.png", 11, CPoint(376,242));
			m_ChiHuKindFlashTongPao.Play(true, 0);
		}
	}
	else
	{//荒庄
		//MessageBox("荒庄");
		m_HuangZhuangFlash.InitFlash(strPath+"LiuJu.png", 6, CPoint(375,300));
		m_HuangZhuangFlash.Play(true, 0);
	}
__LEAVE_FUNCTION
}


void CGameClientView::InitViewFrame()
{
__ENTER_FUNCTION
	m_OutCardFlash.Create(IDD_AI_PROMPT,this);
	m_TingCardFlash.Create(IDD_AI_PROMPT,this);
	m_CardHuPaiList.Create(IDD_AI_PROMPT,this);
	m_GameOperationFlash.Create(IDD_AI_PROMPT, this);

	m_ChiHuKindFlashZiMo.Create(IDD_AI_PROMPT, this);
	m_ChiHuKindFlashDianPao.Create(IDD_AI_PROMPT, this);
	m_ChiHuKindFlashFangPao.Create(IDD_AI_PROMPT, this);
	m_ChiHuKindFlashTongPao.Create(IDD_AI_PROMPT, this);
	m_ChiHuKindFlashHaidi.Create(IDD_AI_PROMPT, this);
	m_ChiHuKindFlashGang.Create(IDD_AI_PROMPT, this);
	m_ChiHuKindFlashPengPeng.Create(IDD_AI_PROMPT, this);
	m_ChiHuKindFlashQingYiSe.Create(IDD_AI_PROMPT, this);
	m_ChiHuKindFlashQiXiaoDui.Create(IDD_AI_PROMPT, this);
	m_ChiHuKindFlashJiangJiang.Create(IDD_AI_PROMPT, this);
	m_HuangZhuangFlash.Create(IDD_AI_PROMPT, this);

	bool bRect = m_CardHuPaiList.LoadImage("SparrowYY\\Picture\\PNG\\");

	CString strPath="";
	GetPicPath(strPath);
	strPath.Append("Cartoon\\");
	CPoint pt(0,0);
	m_OutCardFlash.InitFlash(strPath+"OUTCARDHIDE.png", 9, pt);
__LEAVE_FUNCTION
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//启动方位动画
void CGameClientView::PlayDirectFlash(bool bIsPlay)
{
__ENTER_FUNCTION
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
__LEAVE_FUNCTION
}

//启动色子动画
void CGameClientView::PlaySZFlashBT(bool bIsPlay)
{
__ENTER_FUNCTION
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
__LEAVE_FUNCTION
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
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////