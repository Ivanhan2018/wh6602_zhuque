#include "StdAfx.h"
#include "Resource.h"
#include "GameClientDlg.h"
#include "GameClientView.h"
#include ".\gameclientview.h"

//////////////////////////////////////////////////////////////////////////
//宏定义

//按钮标识
#define IDC_START						100								//开始按钮
#define IDC_MAX_SCORE					104								//最大按钮
#define IDC_MIN_SCORE					105								//最少按钮
#define IDC_CONFIRM						106								//确定按钮
#define	IDC_CANCEL_ADD					119								//取消按钮
#define IDC_ADD_SCORE					110								//加注按钮
#define IDC_LOOK_CARD					111								//看牌消息
#define IDC_COMPARE_CARD				112								//比牌消息
#define IDC_FOLLOW						113								//跟牌消息
#define IDC_GIVE_UP						114								//放弃消息
#define	IDC_OPEN_CARD					115								//开牌消息

//定时器标识
#define IDI_CARTOON						99								//动画定时器
#define	TIME_CARTOON					100								//动画时间
#define IDI_MOVE_JETTON					101								//移动筹码定时器
#define IDI_MOVE_NUMBER					102								//滚动数字

//动画速度  //(SPEED)10 (TIME_CARTOON)70 /CPU UP
#define	SPEED							5								//减慢速度参数
#define	ROCK							10								//振动副度
#define TIME_MOVE_JETTON				20								//移动筹码时间
#define TIME_MOVE_NUMBER				50								//滚动数字时间

//位置变量
#define	MY_SELF_CHAIRID					2								//自己位置

//比牌变量
#define VSCARD_TIME						3								//比牌次数
#define VSCARD_COUNT					6								//比牌数目
#define COMPARE_RADII					115								//比牌宽度

//发牌变量
#define SEND_PELS						10								//发牌象素
#define LESS_SEND_COUNT					2								//发送次数
#define MAX_SEND_COUNT					50								//最大次数
//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CGameClientView, CGameFrameView)							  
	ON_WM_CREATE()
	ON_WM_TIMER()
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CGameClientView::CGameClientView() : CGameFrameView(true,32)
{
	//动画变量
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		m_lDrawGold[i].RemoveAll();
		m_ptKeepJeton[i].RemoveAll();
		m_lStopUpdataScore[i] = 0;
		m_wFlashUser[i]=INVALID_CHAIR;	
	}
	m_bCartoon =false;
	m_SendCardPos.SetPoint(0,0);
	m_SendCardItemArray.RemoveAll();

	//比牌变量
	m_wConmareIndex=0;
	m_wConmareCount=0;
	m_wLoserUser=INVALID_CHAIR;
	ZeroMemory(m_wCompareChairID,sizeof(m_wCompareChairID));

	//数据变量
	ZeroMemory(m_tcBuffer,sizeof(m_tcBuffer));
	ZeroMemory(m_lTableScore,sizeof(m_lTableScore));
	ZeroMemory(m_bCompareUser,sizeof(m_bCompareUser));
	m_wBankerUser=INVALID_CHAIR;
	m_lMaxCellScore=0;
	m_lCellScore=0;	
	m_wFalshCount=0;
	m_bStopDraw=false;
	m_bFalsh=false;
	m_bInvest=false;		
	m_bShow=false;
	m_bCompareCard=false;
	m_wWaitUserChoice=INVALID_CHAIR;
	ZeroMemory(m_cbPlayStatus,sizeof(m_cbPlayStatus));

	//加载位图
	HINSTANCE hInstance=AfxGetInstanceHandle();
	m_ImageJeton.SetLoadInfo(IDB_GOLD,hInstance);
	m_ImageCard.LoadImage(hInstance,"CARD");
	m_ImageTitle.SetLoadInfo(IDB_TITLE,hInstance);
	m_ImageViewBack.SetLoadInfo(IDB_VIEW_BACK,hInstance);
	m_ImageViewCenter.SetLoadInfo(IDB_VIEW_FILL,hInstance);
	m_ImageBanker.LoadImage(hInstance,"BANKER_FLAG");
	m_ImageArrowhead.SetLoadInfo(IDB_ARROWHEAD,hInstance);
	m_ImageReady.SetLoadInfo(IDB_READY,hInstance);
	m_ImageNumber.LoadImage(hInstance,"NUMBER");
	m_ImagePocket.SetLoadInfo(IDB_POCKET,hInstance);
	m_ImageBalance.SetLoadInfo(IDB_SCORE_VIEW,hInstance);
	m_ImageWait.LoadImage(hInstance,"WAIT_TIP");
	m_ImageCompareCard.LoadImage(hInstance,"COMPARE_TIP");

	return;
}

//析构函数
CGameClientView::~CGameClientView()
{
}

//建立消息
int CGameClientView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (__super::OnCreate(lpCreateStruct)==-1) return -1;

	//创建控件
	CRect rcCreate(0,0,0,0);
	m_GoldControl.Create(NULL,NULL,WS_CHILD|WS_CLIPSIBLINGS,CRect(50,50,66,66),this,8);
	//m_ScoreView.Create(NULL,NULL,WS_CHILD|WS_CLIPSIBLINGS|WS_CLIPCHILDREN,rcCreate,this,10);

	//创建按钮
	m_btStart.Create(TEXT(""),WS_CHILD|WS_CLIPSIBLINGS,rcCreate,this,IDC_START);
	m_btMaxScore.Create(TEXT(""),WS_CHILD,rcCreate,this,IDC_MAX_SCORE);
	m_btMinScore.Create(TEXT(""),WS_CHILD,rcCreate,this,IDC_MIN_SCORE);
	m_btConfirm.Create(TEXT(""),WS_CHILD,rcCreate,this,IDC_CONFIRM);
	m_btCancel.Create(TEXT(""),WS_CHILD,rcCreate,this,IDC_CANCEL_ADD);
	m_btOpenCard.Create(TEXT(""),WS_CHILD,rcCreate,this,IDC_OPEN_CARD);

	m_btAddScore.Create(TEXT(""),WS_CHILD|WS_DISABLED,rcCreate,this,IDC_ADD_SCORE);
	m_btCompareCard.Create(TEXT(""),WS_CHILD|WS_DISABLED,rcCreate,this,IDC_COMPARE_CARD);
	m_btGiveUp.Create(TEXT(""),WS_CHILD|WS_DISABLED,rcCreate,this,IDC_GIVE_UP);
	m_btLookCard.Create(TEXT(""),WS_CHILD|WS_DISABLED,rcCreate,this,IDC_LOOK_CARD);
	m_btFollow.Create(TEXT(""),WS_CHILD|WS_DISABLED,rcCreate,this,IDC_FOLLOW);

	//加载位图
	HINSTANCE hInstance=AfxGetInstanceHandle();
	m_btStart.LoadRgnImage(hInstance,IDB_START,RGB(255,0,255));
	m_btMaxScore.LoadRgnImage(hInstance,IDB_USERCONTROL_MAX,RGB(255,0,255), 4);
	m_btMinScore.LoadRgnImage(hInstance,IDB_USERCONTROL_MIN,RGB(255,0,255), 4);
	m_btConfirm.LoadRgnImage(hInstance,IDB_CONFIRM,RGB(255,0,255), 4);
	m_btCancel.LoadRgnImage(hInstance,IDB_USERCONTROL_CLEAR,RGB(255,0,255), 4);
	m_btCompareCard.LoadRgnImage(hInstance,IDB_COMPARE_CARD,RGB(255,0,255));
	m_btOpenCard.LoadRgnImage(hInstance,IDB_OPEN_CARD,RGB(255,0,255));
	m_btGiveUp.LoadRgnImage(hInstance,IDB_GIVE_UP,RGB(255,0,255));
	m_btLookCard.LoadRgnImage(hInstance,IDB_LO0K_CARD,RGB(255,0,255));
	m_btFollow.LoadRgnImage(hInstance,IDB_FOLLOW,RGB(255,0,255));
	m_btAddScore.LoadRgnImage(hInstance,IDB_USERCONTROL_INVEST,RGB(255,0,255));

	//创建控件
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		m_CardControl[i].Create(NULL,NULL,WS_VISIBLE|WS_CHILD,rcCreate,this,20+i);
		m_CardControl[i].SetCommandID(i,IDM_COMPARE_USER);
	}

	return 0;
}

//命令函数
BOOL CGameClientView::OnCommand(WPARAM wParam, LPARAM lParam)
{
	switch (LOWORD(wParam))
	{
	case IDC_START:				//开始按钮
		{
			//发送消息
			AfxGetMainWnd()->SendMessage(IDM_START,0,0);
			return TRUE;
		}
	case IDC_ADD_SCORE:			//下注按钮
		{
			//发送消息
			AfxGetMainWnd()->SendMessage(IDM_ADD_SCORE,0,0);
			return TRUE;
		}
	case IDC_CONFIRM:			//确定按钮
		{
			//发送消息
			AfxGetMainWnd()->SendMessage(IDM_CONFIRM,0,0);
			return TRUE;
		}
	case IDC_CANCEL_ADD:		//取消按钮
		{
			//发送消息
			AfxGetMainWnd()->SendMessage(IDM_CANCEL_ADD,0,0);
			return TRUE;
		}
	case IDC_FOLLOW:			//跟注按钮
		{
			//发送消息
			AfxGetMainWnd()->SendMessage(IDM_CONFIRM,1,1);
			return TRUE;
		}
	case IDC_MIN_SCORE:			//最少按钮
		{
			//发送消息
			AfxGetMainWnd()->SendMessage(IDM_MIN_SCORE,0,0);
			return TRUE;
		}
	case IDC_MAX_SCORE:			//最大按钮
		{
			//发送消息
			AfxGetMainWnd()->SendMessage(IDM_MAX_SCORE,0,0);
			return TRUE;
		}
	case IDC_COMPARE_CARD:		//比牌按钮
		{
			//发送消息
			AfxGetMainWnd()->SendMessage(IDM_COMPARE_CARD,0,0);
			return TRUE;
		}
	case IDC_LOOK_CARD:			//看牌按钮
		{
			//发送消息
			AfxGetMainWnd()->SendMessage(IDM_LOOK_CARD,0,0);
			return TRUE;
		}
	case IDC_OPEN_CARD:			//开牌消息
		{
			//发送消息
			AfxGetMainWnd()->SendMessage(IDM_OPEN_CARD,0,0);
			return TRUE;
		}
	case IDC_GIVE_UP:			//放弃按钮
		{
			//发送消息
			AfxGetMainWnd()->SendMessage(IDM_GIVE_UP,0,0);
			return TRUE;
		}
	}

	return __super::OnCommand(wParam, lParam);
}

//重置界面
void CGameClientView::ResetGameView()
{
	//删除时间
	KillTimer(IDI_CARTOON);
	KillTimer(IDI_MOVE_JETTON);
	KillTimer(IDI_MOVE_NUMBER);

	//动画变量
	m_bCartoon =false;
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		m_lDrawGold[i].RemoveAll();
		m_ptKeepJeton[i].RemoveAll();
		m_wFlashUser[i]=INVALID_CHAIR;
		m_lStopUpdataScore[i] = 0;
	}
	m_SendCardPos.SetPoint(0,0);
	m_SendCardItemArray.RemoveAll();

	//比牌变量
	m_wConmareIndex = 0;
	m_wConmareCount = 0;
	m_wLoserUser=INVALID_CHAIR;
	ZeroMemory(m_wCompareChairID,sizeof(m_wCompareChairID));

	//数据变量
	ZeroMemory(m_tcBuffer,sizeof(m_tcBuffer));
	ZeroMemory(m_lTableScore,sizeof(m_lTableScore));
	m_wBankerUser=INVALID_CHAIR;			
	ZeroMemory(m_bCompareUser,sizeof(m_bCompareUser));
	m_lMaxCellScore=0;					
	m_lCellScore=0;		
	m_wFalshCount=0;
	m_bStopDraw=false;
	m_bFalsh=false;
	m_bInvest=false;
	m_bShow=false;
	m_bCompareCard=false;
	m_wWaitUserChoice=INVALID_CHAIR;
	ZeroMemory(m_cbPlayStatus,sizeof(m_cbPlayStatus));

	//处理按钮
	m_btStart.ShowWindow(SW_HIDE);
	m_btCancel.ShowWindow(SW_HIDE);
	m_btConfirm.ShowWindow(SW_HIDE);
	m_btMaxScore.ShowWindow(SW_HIDE);
	m_btMinScore.ShowWindow(SW_HIDE);
	m_btOpenCard.ShowWindow(SW_HIDE);
	m_GoldControl.ShowWindow(SW_HIDE);
	m_btAddScore.EnableWindow(FALSE);
	m_btCompareCard.EnableWindow(FALSE);
	m_btGiveUp.EnableWindow(FALSE);
	m_btLookCard.EnableWindow(FALSE);
	m_btFollow.EnableWindow(FALSE);

	//隐藏控件
	for (WORD i=0;i<GAME_PLAYER;i++) 
	{
		m_NumberControl[i].ResetControl();
	}
	m_JettonControl.ResetControl();
	//m_ScoreView.ShowWindow(SW_SHOW);  //show12.06
	for (WORD i=0;i<GAME_PLAYER;i++) m_CardControl[i].SetCardData(NULL,0);

	return;
}

//调整控件
void CGameClientView::RectifyGameView(int nWidth, int nHeight)
{
	//CImageHandle ImageHandleBanker(&m_ImageBanker);

	//调整坐标
	m_ptFace[0].x=nWidth/2+356;
	m_ptFace[0].y=nHeight/2-321;
	m_ptFace[1].x=nWidth/2+356;
	m_ptFace[1].y=nHeight/2-92;
	m_ptFace[2].x=8;
	m_ptFace[2].y=nHeight/2+142;
	m_ptFace[3].x=8;
	m_ptFace[3].y=nHeight/2-92;
	m_ptFace[4].x=8;
	m_ptFace[4].y=nHeight/2-321;

	//时间位置   //cp12.03
	m_ptTimer[0].x=nWidth/2+150;
	m_ptTimer[0].y=nHeight/2-200;
	m_ptTimer[1].x=nWidth/2+150;
	m_ptTimer[1].y=nHeight/2+25;
	m_ptTimer[2].x=nWidth/2-315;  
	m_ptTimer[2].y=nHeight/2+140; 
	m_ptTimer[3].x=nWidth/2-150;
	m_ptTimer[3].y= nHeight/2+25;
	m_ptTimer[4].x=nWidth/2-150;
	m_ptTimer[4].y=nHeight/2-200;

	//准备位置
	m_ptReady[0].x=nWidth/2+230;
	m_ptReady[0].y=nHeight/2-170;
	m_ptReady[1].x=nWidth/2+230;
	m_ptReady[1].y=nHeight/2+45;
	m_ptReady[2].x=nWidth/2+10;
	m_ptReady[2].y=nHeight/2+96;
	m_ptReady[3].x=nWidth/2-230;
	m_ptReady[3].y=nHeight/2+45;
	m_ptReady[4].x=nWidth/2-230;
	m_ptReady[4].y=nHeight/2-170;

	//用户扑克  cp12.03
	m_CardControl[0].SetBenchmarkPos(nWidth/2+260,nHeight/2-260,enXCenter,enYCenter);
	m_CardControl[1].SetBenchmarkPos(nWidth/2+260,nHeight/2-32,enXCenter,enYCenter);
	m_CardControl[2].SetBenchmarkPos(nWidth/2,nHeight-100,enXCenter,enYBottom);
	m_CardControl[3].SetBenchmarkPos(nWidth/2-260,nHeight/2-32,enXCenter,enYCenter);
	m_CardControl[4].SetBenchmarkPos(nWidth/2-260,nHeight/2-260,enXCenter,enYCenter);

	//BYTE cbCardData[ 3 ] = { 0x01,0x01,0x01 };
	//m_CardControl[ 0 ].SetCardData( cbCardData, sizeof( cbCardData ) );
	//m_CardControl[ 1 ].SetCardData( cbCardData, sizeof( cbCardData ) );
	//m_CardControl[ 2 ].SetCardData( cbCardData, sizeof( cbCardData ) );
	//m_CardControl[ 3 ].SetCardData( cbCardData, sizeof( cbCardData ) );
	//m_CardControl[ 4 ].SetCardData( cbCardData, sizeof( cbCardData ) );

	//数字控件位置
	m_NumberControl[0].SetBencbmarkPos(nWidth/2+175,nHeight/2-190,enXRight);
	m_NumberControl[1].SetBencbmarkPos(nWidth/2+175,nHeight/2+36,enXRight);
	m_NumberControl[2].SetBencbmarkPos(nWidth/2-80,nHeight/2+270,enXRight);  //12.06
	m_NumberControl[3].SetBencbmarkPos(nWidth/2-175,nHeight/2+36,enXLeft);
	m_NumberControl[4].SetBencbmarkPos(nWidth/2-175,nHeight/2-190,enXLeft);

	//庄家标志
	m_PointBanker[0].x=nWidth/2+439;
	m_PointBanker[0].y=nHeight/2-250-2;
	m_PointBanker[1].x=nWidth/2+439;
	m_PointBanker[1].y=nHeight/2-20-2;
	m_PointBanker[2].x=nWidth/2-388;
	m_PointBanker[2].y=nHeight/2+212-1;
	m_PointBanker[3].x=nWidth/2-388;
	m_PointBanker[3].y= nHeight/2-20-1;
	m_PointBanker[4].x=nWidth/2-388;
	m_PointBanker[4].y=nHeight/2-250-1;

	//筹码位置  
	m_ptJettons[0].x=m_ptFace[0].x;
	m_ptJettons[0].y=m_ptFace[0].y+50+10;
	m_ptJettons[1].x=m_ptFace[1].x;
	m_ptJettons[1].y=m_ptFace[1].y+30+5;
	m_ptJettons[2].x=m_ptFace[2].x;
	m_ptJettons[2].y=m_ptFace[2].y+30;   
	m_ptJettons[3].x=m_ptFace[3].x;
	m_ptJettons[3].y=m_ptFace[3].y+30+5;
	m_ptJettons[4].x=m_ptFace[4].x;
	m_ptJettons[4].y=m_ptFace[4].y+50+10;

	m_ptUserCard[0].x=m_CardControl[0].GetCardPos().x;
	m_ptUserCard[0].y=m_CardControl[0].GetCardPos().y;
	m_ptUserCard[1].x=m_CardControl[1].GetCardPos().x-40;
	m_ptUserCard[1].y=m_CardControl[1].GetCardPos().y+60;
	m_ptUserCard[2].x=m_CardControl[2].GetCardPos().x;
	m_ptUserCard[2].y=m_CardControl[2].GetCardPos().y-35;
	m_ptUserCard[3].x=m_CardControl[3].GetCardPos().x+40;
	m_ptUserCard[3].y=m_CardControl[3].GetCardPos().y+39;
	m_ptUserCard[4].x=m_CardControl[4].GetCardPos().x+80;
	m_ptUserCard[4].y=m_CardControl[4].GetCardPos().y+20;


	//宝石
	m_ptlGems[0].x = nWidth/2+402;
	m_ptlGems[0].y = nHeight/2-215;
	m_ptlGems[1].x = nWidth/2+402;
	m_ptlGems[1].y = nHeight/2+16;
	m_ptlGems[2].x = 54;
	m_ptlGems[2].y = nHeight/2+249;
	m_ptlGems[3].x = 54;
	m_ptlGems[3].y = nHeight/2+16;
	m_ptlGems[4].x = 54;
	m_ptlGems[4].y = nHeight/2-215;


	//等级
	m_ptClass[0].x = nWidth/2+402;
	m_ptClass[0].y = nHeight/2-196;
	m_ptClass[1].x = nWidth/2+402;
	m_ptClass[1].y = nHeight/2+34;
	m_ptClass[2].x = 54;
	m_ptClass[2].y = nHeight/2+268;
	m_ptClass[3].x = 54;
	m_ptClass[3].y = nHeight/2+34;
	m_ptClass[4].x = 54;
	m_ptClass[4].y = nHeight/2-196;

	//按钮控件
	CRect rcButton;
	HDWP hDwp=BeginDeferWindowPos(32);
	const UINT uFlags=SWP_NOACTIVATE|SWP_NOZORDER|SWP_NOCOPYBITS|SWP_NOSIZE;

	//开始按钮
	m_btStart.GetWindowRect(&rcButton);
	DeferWindowPos(hDwp,m_btStart,NULL,nWidth/2-50,nHeight/2+90,0,0,uFlags);

	//控制按钮  cp12.03
	m_btAddScore.GetWindowRect(&rcButton);
	//INT n = m_nYBorder;
	DeferWindowPos(hDwp,m_btAddScore,NULL,nWidth/2-180,nHeight/2+80,0,0,uFlags);        //加注
	DeferWindowPos(hDwp,m_btFollow,NULL,nWidth/2-100,nHeight/2+80,0,0,uFlags);          //跟注
	DeferWindowPos(hDwp,m_btLookCard,NULL,nWidth/2-20,nHeight/2+80,0,0,uFlags);         //看牌
	DeferWindowPos(hDwp,m_btGiveUp,NULL,nWidth/2+60,nHeight/2+80,0,0,uFlags);           //弃牌
	DeferWindowPos(hDwp,m_btCompareCard,NULL,nWidth/2+140,nHeight/2+80,0,0,uFlags);     //比牌
	DeferWindowPos(hDwp,m_btOpenCard,NULL,nWidth/2+140-160,nHeight/2+80,0,0,uFlags);     //开牌

	//动画位置
	m_ptUserCompare.SetPoint(nWidth/2,nHeight*2/5);

	int iX=nWidth/2-285;
	int iY=nHeight/2+185;

	m_btMaxScore.GetWindowRect(&rcButton);
	DeferWindowPos(hDwp,m_btMinScore,NULL,iX+140,iY+8,0,0,uFlags);//取消
	DeferWindowPos(hDwp,m_btMaxScore,NULL,iX+20,iY+80,0,0,uFlags);
	DeferWindowPos(hDwp,m_btCancel,NULL,iX+73,iY+80,0,0,uFlags);
	DeferWindowPos(hDwp,m_btConfirm,NULL,iX+125,iY+80,0,0,uFlags);

	//结束移动
	EndDeferWindowPos(hDwp);

	//加注窗口
	m_GoldControl.SetBasicPoint(iX,iY);

	//积分视图
	CRect rcControl;
	//m_ScoreView.GetWindowRect(&rcControl);
	//m_ScoreView.SetWindowPos(NULL,(nWidth-rcControl.Width())/2,nHeight/2-190,0,0,SWP_NOZORDER|SWP_NOSIZE); 
	//m_ScoreView.ShowWindow(SW_HIDE);

	//筹码控件
	m_JettonControl.SetBenchmarkPos(nWidth/2,nHeight/2-50); // -50 cp12.03

	//调整动画
	SetFlowerControlInfo(0,m_ptFace[0].x-BIG_FACE_WIDTH/2,m_ptFace[0].y);
	SetFlowerControlInfo(1,m_ptFace[1].x-BIG_FACE_WIDTH/2,m_ptFace[1].y);
	SetFlowerControlInfo(2,m_ptFace[2].x-BIG_FACE_WIDTH,m_ptFace[2].y-40);
	SetFlowerControlInfo(3,m_ptFace[3].x,m_ptFace[3].y);
	SetFlowerControlInfo(4,m_ptFace[4].x,m_ptFace[4].y);
	return;
}

//绘画界面
void CGameClientView::DrawGameView(CDC * pDC, int nWidth, int nHeight)
{
	//地震效果
	//if(m_bFalsh)
	//{
	//	CImageHandle ImageHandleBack(&m_ImageViewBack);
	//	int xValue=rand()%ROCK;
	//	int yValue=rand()%ROCK;
	//	if(xValue%2==0)xValue*=(-1);
	//	if(yValue%2==0)yValue*=(-1);
	//	m_ImageViewBack.AlphaDrawImage(pDC,(nWidth/2-512)+xValue,(nHeight/2-397)+yValue,
	//		m_ImageViewBack.GetWidth(),m_ImageViewBack.GetHeight(),0,0,RGB(255,0,255));
	//}
	//else 
	//{
	//	CImageHandle ImageHandleBack(&m_ImageViewBack);
	//	m_ImageViewBack.AlphaDrawImage(pDC,(nWidth/2-512+6+106),(nHeight/2-383+65),  //106;65
	//		m_ImageViewBack.GetWidth(),m_ImageViewBack.GetHeight(),0,0,RGB(255,0,255));

		//绘画背景
		//DrawViewImage(pDC,m_ImageViewCenter,enMode_Spread);//用于最大化窗口
		//DrawViewImage(pDC,m_ImageViewBack,enMode_Spread/*enMode_Centent*/);
	//}

	DrawViewImage(pDC,m_ImageViewBack,enMode_Spread/*enMode_Centent*/);

	if(!m_bStopDraw)
	{
		//绘画用户
		for (WORD i=0;i<GAME_PLAYER;i++)
		{
			//变量定义
			WORD wUserTimer=GetUserTimer(i);
			const tagUserData * pUserData=GetUserInfo(i);
//#ifdef _DEBUG
			////测试代码
			//tagUserData Obj;
			//Obj.cbGender = 1;
			//_sntprintf(Obj.szName,sizeof(Obj.szName),TEXT("用户的名字"));
			//Obj.cbUserStatus=US_READY;
			////Obj.wFaceID = 2;
			//pUserData = &Obj;
            //wUserTimer = 30;
			
			////庄家标志
			//CImageHandle ImageHandleBanker(&m_ImageBanker);
			//m_ImageBanker.DrawImage(pDC,m_PointBanker[i].x,m_PointBanker[i].y,m_ImageBanker.GetWidth(),m_ImageBanker.GetHeight(),0,0);
//#endif
			//绘画用户
			if (pUserData!=NULL)
			{

				DrawUserFace(pDC,pUserData,m_ptFace[i].x,m_ptFace[i].y,pUserData->cbUserStatus==US_OFFLINE); //头像

				//用户名字
				UINT uOldAlign = pDC->SetTextAlign(TA_LEFT);
				pDC->SetTextColor((wUserTimer>0)?RGB(250,250,250):RGB(220,220,220));
				DrawTextString(pDC,pUserData->szName,RGB(255,255,255),RGB(0,0,0),m_ptFace[i].x+20,m_ptFace[i].y-15);
				//pDC->SetTextColor((wUserTimer>0)?RGB(250,250,250):RGB(220,220,220));


				//用户金币 宝石               
				pDC->SetTextAlign(TA_LEFT);
				TCHAR szBuffer[64]=TEXT("");
				//LONGLONG lTempScore=((m_ScoreView.IsWindowVisible()==TRUE || m_NumberControl[i].GetScore()!=0)?(0):(m_lTableScore[i]));
				LONGLONG lTempScore = 0;
				if(m_btStart.IsWindowVisible()==TRUE || m_cbPlayStatus[i]==FALSE)lTempScore=0;
				LONGLONG lLeaveScore=pUserData->lGems-lTempScore;
				if(m_lStopUpdataScore[i]>0)lLeaveScore=m_lStopUpdataScore[i]-m_lTableScore[i];
				_snprintf(szBuffer,sizeof(szBuffer),TEXT("%d"),lLeaveScore);
				DrawTextString(pDC,szBuffer,RGB(255,255,255),RGB(0,0,0),m_ptlGems[i].x,m_ptlGems[i].y);

				//等级
				::memset(szBuffer, 0, sizeof(szBuffer));
				sprintf(szBuffer, TEXT("%d"), pUserData->dwClass);
				DrawTextString(pDC,szBuffer,RGB(255,255,255),RGB(0,0,0),m_ptClass[i].x,m_ptClass[i].y);
				pDC->SetTextAlign(TA_CENTER);


				//其他信息
				//wUserTimer =30;
				if (wUserTimer!=0) 
				{
					if(m_CardControl[MY_SELF_CHAIRID].GetCardCount()==0)
						DrawUserTimer(pDC,m_ptTimer[i].x/*nWidth/2-100*//*m_ptFace[i].x+m_nXFace/2*/,m_ptTimer[i].y,wUserTimer);  //游戏开始前的准备时间cp12.06
					else DrawUserTimer(pDC,m_ptTimer[i].x,m_ptTimer[i].y,wUserTimer);
				}

				//准备标志
				if (pUserData->cbUserStatus==US_READY) 
				{
					DrawUserReady(pDC,m_ptReady[i].x,m_ptReady[i].y);
				}
				
				
				pDC->SetTextAlign(uOldAlign);
			}
		}

		//庄家信息
		if (m_wBankerUser!=INVALID_CHAIR)
		{
			//庄家标志
			//CImageHandle ImageHandleBanker(&m_ImageBanker);
			m_ImageBanker.DrawImage(pDC,m_PointBanker[m_wBankerUser].x,m_PointBanker[m_wBankerUser].y,m_ImageBanker.GetWidth(),m_ImageBanker.GetHeight(),0,0);
		}

		//绘画筹码
		m_JettonControl.DrawJettonControl(pDC);

		//计算总注
		LONGLONG lTableScore=0L;
		for (WORD i=0;i<GAME_PLAYER;i++) lTableScore+=m_lTableScore[i];

		//总注信息
		if (lTableScore>0L)
		{	
			//CImageHandle ImageHandle3(&m_ImageNumber);
			int iCount=0;
			LONGLONG lCell[11];
			ZeroMemory(lCell,sizeof(lCell));
			while(lTableScore>0L)
			{
				LONGLONG lScore=lTableScore%10;
				lCell[iCount++] = lScore;
				lTableScore=lTableScore/10;
			}
			int iTemp=0;
			int iX=m_ImageNumber.GetWidth();
			for (int i=iCount-1;i>=0;i--)
			{
				m_ImageNumber.DrawImage(pDC,nWidth/2+(iX/10)*(iTemp++),nHeight/2-345, //cp12.03
					iX/10,m_ImageNumber.GetHeight(),iX/10*(int)lCell[i],0);
			}
		}

		//得分数字滚动动画  
		for( WORD i = 0; i < GAME_PLAYER; i++ )
		{
			m_NumberControl[i].DrawNumberControl(pDC);
		}

		//绘画筹码
		for (BYTE i=0;i<GAME_PLAYER;i++)
		{
			if (m_lTableScore[i]==0L) continue;

			//绘画数目
			TCHAR szBuffer[64];
			_snprintf(szBuffer,CountArray(szBuffer),TEXT("%d"),m_lTableScore[i]);

			//计算位置 牌左上角的数字
			CRect rcDrawRect;
			rcDrawRect.top=m_CardControl[i].GetCardPos().y;
			rcDrawRect.left=m_CardControl[i].GetCardPos().x+((i>2)?(135):(-50));
			rcDrawRect.right=rcDrawRect.left+30;
			rcDrawRect.bottom=rcDrawRect.top+12-25;

			//创建字体
			CFont ViewFont;
			ViewFont.CreateFont(-14,0,0,0,700,0,0,0,134,3,2,1,2,TEXT("宋体"));

			//设置字体
			CFont * pOldFont=pDC->SelectObject(&ViewFont);

			//绘画数目
			UINT uOldAlign_a = pDC->SetTextAlign(TA_LEFT);
			DrawTextString(pDC,szBuffer,RGB(250,200,40),RGB(0,0,0),&rcDrawRect,DT_CENTER|DT_VCENTER|DT_NOCLIP|DT_SINGLELINE);
			
			
			//清理字体
			pDC->SelectObject(pOldFont);
			ViewFont.DeleteObject();
			pDC->SetTextAlign(uOldAlign_a);
		}

		//绘画信息
		if(m_lCellScore>0)
		{
			//创建字体
			CFont ViewFont;
			ViewFont.CreateFont(-12,0,0,0,700,0,0,0,134,3,2,1,2,TEXT("宋体"));

			//设置字体
			CFont * pOldFont=pDC->SelectObject(&ViewFont);

			UINT uOldAlign_b = pDC->SetTextAlign(TA_LEFT);
			pDC->SetTextColor(RGB(255,255,0));

			//计算位置
			CRect rcDrawRect;

			//绘画数目
			TCHAR szBuffer[64];
			_snprintf(szBuffer,CountArray(szBuffer),TEXT("%d"),m_lMaxCellScore);/*最大单注：*/
			rcDrawRect.top=nHeight/2-302;
			rcDrawRect.left=nWidth/2+95;
			rcDrawRect.right=rcDrawRect.left+40;
			rcDrawRect.bottom=rcDrawRect.top+12;
			pDC->DrawText(szBuffer,lstrlen(szBuffer),&rcDrawRect,DT_CENTER|DT_VCENTER|DT_NOCLIP|DT_SINGLELINE);

			_snprintf(szBuffer,CountArray(szBuffer),TEXT("%d"),m_lCellScore); /*底注：*/
			rcDrawRect.top=nHeight/2-302;
			rcDrawRect.left=nWidth/2-80;
			rcDrawRect.right=rcDrawRect.left+50;
			rcDrawRect.bottom=rcDrawRect.top+12;
			pDC->DrawText(szBuffer,lstrlen(szBuffer),&rcDrawRect,DT_CENTER|DT_VCENTER|DT_NOCLIP|DT_SINGLELINE);

			//清理字体
			pDC->SelectObject(pOldFont);		
			ViewFont.DeleteObject();
			pDC->SetTextAlign(uOldAlign_b);
		}

		//选比标志
		if(m_bCompareCard)
		{
			for (int i=0;i<GAME_PLAYER;i++)
			{
				if(m_bCompareUser[i]==TRUE)
				{
					CPoint cPost=m_CardControl[i].GetCardPos();
					CImageHandle ImageArrowhead(&m_ImageArrowhead);
					m_ImageArrowhead.AlphaDrawImage(pDC,cPost.x,cPost.y-m_ImageArrowhead.GetHeight(),
						m_ImageArrowhead.GetWidth(),m_ImageArrowhead.GetHeight(),0,0,RGB(255,0,255));
				}
			}
		}
	}

	//发牌绘画
	if (m_SendCardItemArray.GetCount()>0)
	{
		//变量定义
		//CImageHandle HandleCard(&m_ImageCard);
		//tagSendCardItem * pSendCardItem=&m_SendCardItemArray[0];

		//获取大小
		int nItemWidth=m_ImageCard.GetWidth()/13;
		int nItemHeight=m_ImageCard.GetHeight()/5;

		//绘画扑克
		int nXPos=m_SendCardPos.x-nItemWidth/2;
		int nYPos=m_SendCardPos.y-nItemHeight/2+10;
		m_ImageCard.DrawImage(pDC,nXPos,nYPos,nItemWidth,nItemHeight,nItemWidth*2,nItemHeight*4);
	}

	//比牌扑克
	if (m_wLoserUser!=INVALID_CHAIR)
	{
		//送牌状态
		if (m_wConmareIndex<m_wConmareCount)
		{//
			//绘画扑克
			for (BYTE i=0;i<2;i++)
			{
				//变量定义
				CPoint PointCard;
				CPoint PointStart=m_ptUserCard[m_wCompareChairID[i]];

				//计算位置
				INT nQuotiety=(i==0)?-1:1;
				PointCard.y=PointStart.y+(m_ptUserCompare.y-PointStart.y)*m_wConmareIndex/m_wConmareCount;
				PointCard.x=PointStart.x+(m_ptUserCompare.x-PointStart.x+COMPARE_RADII*nQuotiety)*m_wConmareIndex/m_wConmareCount;

				//绘画扑克
				DrawCompareCard(pDC,PointCard.x,PointCard.y,false);
			}
		}

		//比牌状态
		if ((m_wConmareIndex>=m_wConmareCount)&&((m_wConmareIndex<m_wConmareCount+VSCARD_COUNT*VSCARD_TIME)))
		{
			//绘画扑克
			for (BYTE i=0;i<2;i++)
			{
				//变量定义
				CPoint PointCard;
				CPoint PointStart=m_ptUserCard[m_wCompareChairID[i]];

				//计算位置
				INT nQuotiety=(i==0)?-1:1;
				PointCard.y=m_ptUserCompare.y;
				PointCard.x=m_ptUserCompare.x+COMPARE_RADII*nQuotiety;

				//绘画扑克
				DrawCompareCard(pDC,PointCard.x,PointCard.y,false);
			}

			//比牌标志
			CSize SizeVSCard;
			CPngImage ImageVSCard;
			ImageVSCard.LoadImage(AfxGetInstanceHandle(),TEXT("VS_CARD"));
			SizeVSCard.SetSize(ImageVSCard.GetWidth()/VSCARD_COUNT,ImageVSCard.GetHeight());

			//绘画标志
			INT nImageIndex=(m_wConmareIndex-m_wConmareCount)%VSCARD_COUNT;
			ImageVSCard.DrawImage(pDC,m_ptUserCompare.x-SizeVSCard.cx/2,m_ptUserCompare.y-SizeVSCard.cy/2,
				SizeVSCard.cx,SizeVSCard.cy,nImageIndex*SizeVSCard.cx,0);

			//声音效果
			CGameClientDlg * pGameClientDlg=CONTAINING_RECORD(this,CGameClientDlg,m_GameClientView);
			pGameClientDlg->PlayGameSound(AfxGetInstanceHandle(),TEXT("COMPARE_CARD"));

		}

		//回牌状态
		if (m_wConmareIndex>=m_wConmareCount+VSCARD_COUNT*VSCARD_TIME)
		{
			//变量定义
			WORD wConmareIndex=m_wConmareIndex-m_wConmareCount-VSCARD_COUNT*VSCARD_TIME;

			//绘画扑克
			for (BYTE i=0;i<2;i++)
			{
				//变量定义
				CPoint PointCard;
				CPoint PointStart=m_ptUserCard[m_wCompareChairID[i]];

				//计算位置
				INT nQuotiety=(i==0)?-1:1;
				PointCard.y=m_ptUserCompare.y+(PointStart.y-m_ptUserCompare.y)*wConmareIndex/m_wConmareCount;
				PointCard.x=m_ptUserCompare.x+COMPARE_RADII*nQuotiety+(PointStart.x-m_ptUserCompare.x-COMPARE_RADII*nQuotiety)*wConmareIndex/m_wConmareCount;

				//绘画扑克
				DrawCompareCard(pDC,PointCard.x,PointCard.y,(m_wLoserUser==m_wCompareChairID[i])?true:false);
			}

			//声音效果
			if(m_wConmareIndex==m_wConmareCount+VSCARD_COUNT*VSCARD_TIME)
			{
				CGameClientDlg * pGameClientDlg=CONTAINING_RECORD(this,CGameClientDlg,m_GameClientView);
				pGameClientDlg->PlayGameSound(AfxGetInstanceHandle(),TEXT("CENTER_SEND_CARD"));
			}
		}
	}

	//等待选择
	if(m_wWaitUserChoice!=INVALID_CHAIR)
	{
		//TCHAR szBuffer[64]=TEXT("");

		////创建字体
		//CFont ViewFont;
		//ViewFont.CreateFont(-15,0,0,0,700,0,0,0,134,3,2,1,2,TEXT("宋体"));

		////设置字体
		//CFont * pOldFont=pDC->SelectObject(&ViewFont);
		//UINT uOldAlign_c = pDC->SetTextAlign(TA_CENTER);

		//输出信息
		if(m_wWaitUserChoice==TRUE)
			 m_ImageWait.DrawImage(pDC,(nWidth-m_ImageWait.GetWidth())/2,nHeight/2);
			//_sntprintf(szBuffer,sizeof(szBuffer),TEXT("等待玩家选择比牌"));
		else m_ImageCompareCard.DrawImage(pDC,(nWidth-m_ImageCompareCard.GetWidth())/2,nHeight/2);
			//_sntprintf(szBuffer,sizeof(szBuffer),TEXT("请选择玩家进行比牌"));
		//DrawTextString(pDC,szBuffer,RGB(50,50,50),RGB(0,170,0),nWidth/2+10,nHeight/2+45);

		////清理字体
		//pDC->SelectObject(pOldFont);
		//ViewFont.DeleteObject();
		//pDC->SetTextAlign(uOldAlign_c);
	}

	return;
}

//开始数字滚动
void CGameClientView::BeginMoveNumber()
{
	BOOL bSuccess = FALSE;
	for( WORD i = 0; i < GAME_PLAYER; i++ )
	{
		if( m_NumberControl[i].BeginScrollNumber() )
			bSuccess = TRUE;
	}
	if( bSuccess )
	{
		SetTimer( IDI_MOVE_NUMBER,TIME_MOVE_NUMBER,NULL );
		UpdateGameView( NULL );
	}

	return;
}

//停止数字滚动
void CGameClientView::StopMoveNumber()
{
	BOOL bMoving = FALSE;
	for( WORD i = 0; i < GAME_PLAYER; i++ )
	{
		if( m_NumberControl[i].FinishScrollNumber() )
			bMoving = TRUE;
	}
	if( bMoving )
	{
		KillTimer( IDI_MOVE_NUMBER );
		UpdateGameView( NULL );
	}

	return;
}

//设置下注
void CGameClientView::SetUserTableScore(WORD wChairID, LONGLONG lTableScore,LONGLONG lCurrentScore)
{
	//设置数据
	if (wChairID!=INVALID_CHAIR) 
	{
		m_lTableScore[wChairID]=lTableScore;
		m_JettonControl.AddScore(lCurrentScore,m_ptJettons[wChairID]);
	}
	else 
	{
		ZeroMemory(m_tcBuffer,sizeof(m_tcBuffer));
		for (WORD i=0;i<GAME_PLAYER;i++)
		{
			m_lDrawGold[i].RemoveAll();
			m_ptKeepJeton[i].RemoveAll();
		}
		ZeroMemory(m_lTableScore,sizeof(m_lTableScore));
	}

	//更新界面
	UpdateGameView(NULL);

	return;
}

//开始筹码动画
void CGameClientView::BeginMoveJettons()
{
	if( m_JettonControl.BeginMoveJettons() )
	{
		SetTimer( IDI_MOVE_JETTON,TIME_MOVE_JETTON,NULL );

		//更新界面
		UpdateGameView(NULL);
	}
	return;
}

//停止筹码动画
void CGameClientView::StopMoveJettons()
{
	if( m_JettonControl.FinishMoveJettons() )
	{
		KillTimer( IDI_MOVE_JETTON );

		//更新界面
		UpdateGameView( NULL );
	}
	return;
}

//胜利玩家
void CGameClientView::SetGameEndInfo( WORD wWinner )
{
	if( wWinner == INVALID_CHAIR ) return ;

	m_JettonControl.RemoveAllScore( m_ptJettons[wWinner] );
	if( m_JettonControl.BeginMoveJettons() )
		SetTimer( IDI_MOVE_JETTON,TIME_MOVE_JETTON,NULL );

	//更新界面
	UpdateGameView( NULL );
	return;
}

//等待选择
void CGameClientView::SetWaitUserChoice(WORD wChoice)
{
	m_wWaitUserChoice = wChoice;

	//更新界面
	UpdateGameView(NULL);

	return;
}

//庄家标志
void CGameClientView::SetBankerUser(WORD wBankerUser)
{
	m_wBankerUser=wBankerUser;

	//更新界面
	UpdateGameView(NULL);
	return;
}

////等待标志
//void CGameClientView::SetWaitInvest(bool bInvest)
//{
//	if(m_bInvest!=bInvest)
//	{
//		m_bInvest=bInvest;
//
//		//更新界面
//		UpdateGameView(NULL);
//	}
//	return;
//}

//左上信息
void CGameClientView::SetScoreInfo(LONGLONG lTurnMaxScore,LONGLONG lTurnLessScore)
{
	m_lMaxCellScore=lTurnMaxScore;
	m_lCellScore=lTurnLessScore;

	//更新界面
	UpdateGameView(NULL);
	return;
}

////显示牌型
//void CGameClientView::DisplayType(bool bShow)
//{
//	if(m_bShow!=bShow)
//	{
//		m_bShow=bShow;
//
//		//更新界面
//		UpdateGameView(NULL);
//	}
//}

//比牌标志
void CGameClientView::SetCompareCard(bool bCompareCard,BOOL bCompareUser[])
{
	m_bCompareCard=bCompareCard;
	if(bCompareUser!=NULL)
	{
		for(int i=0;i<GAME_PLAYER;i++)
		{
			m_bCompareUser[i] = bCompareUser[i];
			if(m_bCompareUser[i])
			{
				m_CardControl[i].SetCompareCard(true);
			}
			else
			{
				m_CardControl[i].SetCompareCard(false);
			}
		}
	}
	
	if(!bCompareCard)
	{
		for(int i=0;i<GAME_PLAYER;i++)
		{
			m_CardControl[i].SetCompareCard(false);
		}
	}

	//更新界面
	UpdateGameView(NULL);

	return;
}

//停止更新
void CGameClientView::StopUpdataScore(bool bStop)
{
	//清理数据
	ZeroMemory(m_lStopUpdataScore,sizeof(m_lStopUpdataScore));

	//保存数目
	if(bStop)
	{
		//获取用户
		for (WORD i=0;i<GAME_PLAYER;i++)
		{
			//变量定义
			const tagUserData * pUserData=GetUserInfo(i);
			if(pUserData!=NULL)
			{
				m_lStopUpdataScore[i]=pUserData->lGems;
			}
		}
	}

	//更新界面
	UpdateGameView(NULL);

	return;
}

//绘画扑克
void CGameClientView::DrawCompareCard(CDC * pDC, INT nXPos, INT nYPos, bool bChapped)
{
	//加载资源
	//CImageHandle HandleGameCard(&m_ImageCard);
	CSize SizeGameCard(m_ImageCard.GetWidth()/13,m_ImageCard.GetHeight()/5);

	//调整位置
	nYPos-=SizeGameCard.cy/2;
	nXPos-=(SizeGameCard.cx+DEFAULT_PELS*(MAX_COUNT-1))/2;

	//绘画扑克
	for (BYTE i=0;i<MAX_COUNT;i++)
	{
		INT nXDrawPos=nXPos+DEFAULT_PELS*i;
		m_ImageCard.DrawImage(pDC,nXDrawPos,nYPos,SizeGameCard.cx,SizeGameCard.cy,
			SizeGameCard.cx*((bChapped==true)?4:2),SizeGameCard.cy*4);
	}

	return;
}

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

//艺术字体
void CGameClientView::DrawTextString(CDC * pDC, LPCTSTR pszString, COLORREF crText, COLORREF crFrame, LPRECT lpRect,UINT nFormat)
{
	//变量定义
	int nStringLength=lstrlen(pszString);
	int nXExcursion[8]={1,1,1,0,-1,-1,-1,0};
	int nYExcursion[8]={-1,0,1,1,1,0,-1,-1};

	//绘画边框
	pDC->SetTextColor(crFrame);
	CRect rcDraw;
	for (int i=0;i<CountArray(nXExcursion);i++)
	{
		rcDraw.CopyRect(lpRect);
		rcDraw.OffsetRect(nXExcursion[i],nYExcursion[i]);
		pDC->DrawText(pszString,nStringLength,&rcDraw,nFormat);
	}

	//绘画字体
	rcDraw.CopyRect(lpRect);
	pDC->SetTextColor(crText);
	pDC->DrawText(pszString,nStringLength,&rcDraw,nFormat);

	return;
}

//比牌动画
void CGameClientView::PerformCompareCard(WORD wCompareUser[2], WORD wLoserUser)
{
	//效验状态
	ASSERT(m_wLoserUser==INVALID_CHAIR);
	if (m_wLoserUser!=INVALID_CHAIR) return;

	//设置变量
	m_wConmareIndex=0;//移动动画序号
	m_wLoserUser=wLoserUser;
	m_wConmareCount=LESS_SEND_COUNT; //计算比牌移动动画的步数

	//对比用户
	m_wCompareChairID[0]=wCompareUser[0];
	m_wCompareChairID[1]=wCompareUser[1];

	//次数调整
	for (BYTE i=0;i<2;i++)
	{
		//位移计算
		INT nXCount=abs(m_ptUserCard[wCompareUser[i]].x-m_ptUserCompare.x)/SEND_PELS;
		INT nYCount=abs(m_ptUserCard[wCompareUser[i]].y-m_ptUserCompare.y)/SEND_PELS;

		//次数调整
		m_wConmareCount=__max(m_wConmareCount,__max(nXCount,nYCount));
	}
	if (m_wConmareCount > MAX_SEND_COUNT)
	{
		m_wConmareCount = MAX_SEND_COUNT;
	}

	//设置扑克
	m_CardControl[wCompareUser[0]].SetCardData(NULL,0);
	m_CardControl[wCompareUser[1]].SetCardData(NULL,0);

	//设置时间
	if (!m_bCartoon)
	{
		m_bCartoon=true;
		SetTimer(IDI_CARTOON,TIME_CARTOON,NULL);
	}

	//声音效果
	//CGameClientDlg * pGameClientDlg=(CGameClientDlg*)GetParent();
	//pGameClientDlg->PlayGameSound(AfxGetInstanceHandle(),TEXT("CENTER_SEND_CARD"));
	return;
}





//发牌动画
void CGameClientView::DispatchUserCard(WORD wChairID, BYTE cbCardData)
{
	//效验参数
	ASSERT(wChairID<GAME_PLAYER);
	if (wChairID>=GAME_PLAYER) return;

	//设置变量
	tagSendCardItem SendCardItem;
	SendCardItem.wChairID=wChairID;
	SendCardItem.cbCardData=cbCardData;
	m_SendCardItemArray.Add(SendCardItem);

	//启动动画
	if (m_SendCardItemArray.GetCount()==1) 
	{
		//获取位置
		CRect rcClient;
		GetClientRect(&rcClient);

		//设置位置 /2
		m_SendCardPos.x=rcClient.Width()/2+200;
		m_SendCardPos.y=rcClient.Height()/2-308;
		m_KeepPos.x =m_SendCardPos.x;
		m_KeepPos.y=m_SendCardPos.y;

		if(!m_bCartoon)
		{
			m_bCartoon = true;
			//设置定时器
			//SetTimer(IDI_SEND_CARD,TIME_SENDSPEED,NULL);
			SetTimer(IDI_CARTOON,TIME_CARTOON/2,NULL);
		}

		//播放声音
		CGameClientDlg * pGameClientDlg=CONTAINING_RECORD(this,CGameClientDlg,m_GameClientView);
		pGameClientDlg->PlayGameSound(AfxGetInstanceHandle(),TEXT("SEND_CARD"));
	}

	return;
}

//闪牌动画 
bool CGameClientView::bFalshCard(WORD wFalshUser[])
{
	for(int i=0;i<GAME_PLAYER;i++)
	{
		if(wFalshUser[i]<GAME_PLAYER)
		{
			m_wFlashUser[i] = wFalshUser[i];
		}
	}

	//设置定时器
	m_bStopDraw=true;

	if(!m_bCartoon)
	{
		m_bCartoon = true;
		//SetTimer(IDI_CARTOON,TIME_CARTOON,NULL);
		//SetTimer(IDI_CARTOON,10,NULL);
	}

	return true;
}

//发牌处理
bool CGameClientView::SendCard()
{
	//动画判断
	if (m_SendCardItemArray.GetCount()==0) return false;

	//获取位置
	CRect rcClient;
	GetClientRect(&rcClient);

	//变量定义
	int nXExcursion[GAME_PLAYER]={-1,1,-1,-1,-1};
	int nYExcursion[GAME_PLAYER]={0,1,1,1,1};

	//设置位置
	tagSendCardItem * pSendCardItem=&m_SendCardItemArray[0];
	WORD bID=pSendCardItem->wChairID;
	BYTE bTimes=0;
	if(bID==0)bTimes=4;
	else if(bID==1)bTimes=3;
	else if(bID==2)bTimes=2;
	else if(bID>=3)bTimes=2;

	m_SendCardPos.x+=nXExcursion[bID]*(abs(m_ptUserCard[bID].x-m_KeepPos.x)/SPEED*bTimes);
	m_SendCardPos.y+=nYExcursion[bID]*(abs(m_ptUserCard[bID].y-m_KeepPos.y)/SPEED*bTimes);

	//更新界面
	UpdateGameView(&rcClient);

	//停止判断
	int bTempX=0,bTempY=0;
	bTempX=m_ptUserCard[bID].x;
	bTempY=m_ptUserCard[bID].y;
	if((pSendCardItem->wChairID==0 && m_SendCardPos.x<=bTempX)||(pSendCardItem->wChairID>=1 && m_SendCardPos.y>=bTempY))
	{
		//获取扑克
		BYTE cbCardData[MAX_COUNT];
		WORD wChairID=pSendCardItem->wChairID;
		BYTE cbCardCount=(BYTE)m_CardControl[wChairID].GetCardData(cbCardData,CountArray(cbCardData));

		//设置扑克&& pSendCardItem->cbCardData!=0
		if (cbCardCount<MAX_COUNT)
		{
			cbCardData[cbCardCount++]=pSendCardItem->cbCardData;
			m_CardControl[wChairID].SetCardData(cbCardData,cbCardCount);
		}

		//删除动画
		m_SendCardItemArray.RemoveAt(0);

		//继续动画
		if (m_SendCardItemArray.GetCount()>0)
		{
			//设置位置 /2
			GetClientRect(&rcClient);
			m_SendCardPos.x=rcClient.Width()/2+200;
			m_SendCardPos.y=rcClient.Height()/2-308;

			//播放声音
			CGameClientDlg * pGameClientDlg=CONTAINING_RECORD(this,CGameClientDlg,m_GameClientView);
			pGameClientDlg->PlayGameSound(AfxGetInstanceHandle(),TEXT("SEND_CARD"));

			return true;
		}
		else
		{
			//完成处理
			AfxGetMainWnd()->SendMessage(IDM_SEND_CARD_FINISH,0,0);

			return false;
		}
	}

	return true;
}

//闪牌处理
bool CGameClientView::FlashCard()
{	
	return true;
	//动画判断
	//BYTE iCount = 0;
	//for(;iCount<GAME_PLAYER;iCount++)
	//{
	//	if(m_wFlashUser[iCount]<GAME_PLAYER)
	//	{
	//		break;
	//	}
	//}
	//if(iCount==GAME_PLAYER)return false;

	////设置变量
	//m_wFalshCount++;
	//WORD wTimes=m_wFalshCount*TIME_CARTOON;

	//m_bFalsh=!m_bFalsh;

	////扑克背景
	//WORD wValue=(rand()%2==0)?((m_wFalshCount+1)%6):((m_wFalshCount+2)%6);
	//for(int i=0;i<GAME_PLAYER;i++)
	//{
	//	if(m_wFlashUser[i]<GAME_PLAYER)
	//	{
	//		m_CardControl[m_wFlashUser[i]].SetCardColor(wValue);
	//	}
	//}

	////停止动画
	//if(wTimes >= 2200)
	//{
	//	//设置参数
	//	m_bFalsh=false;
	//	m_bStopDraw=false;
	//	m_wFalshCount=0;
	//	for(int i=0;i<GAME_PLAYER;i++)
	//	{
	//		if(m_wFlashUser[i]<GAME_PLAYER)
	//		{
	//			m_CardControl[m_wFlashUser[i]].SetCardColor(INVALID_CHAIR);
	//			m_wFlashUser[i]=INVALID_CHAIR;
	//		}
	//	}

	//	//完成处理
	//	//AfxGetMainWnd()->SendMessage(IDM_FALSH_CARD_FINISH,0,0);

	//	//更新界面
	//	UpdateGameView(NULL);

	//	return false; 
	//}

	////更新界面
	//UpdateGameView(NULL);
	//return true; 
}

//比牌处理
bool CGameClientView::CompareCard()
{
	//动画判断
	if (m_wLoserUser==INVALID_CHAIR) return false;

	//设置索引
	m_wConmareIndex++;

	//停止判断
	if (m_wConmareIndex>=((m_wConmareCount*2)+VSCARD_COUNT*VSCARD_TIME))
	{
#ifdef ZJH_GAME_PRINT_INFO
		CGameClientDlg* pDlg = (CGameClientDlg*)GetParent();
		char chInfo[100]={0};
		::sprintf(chInfo, "视图比牌动画结束,输家:%d", m_wLoserUser);
		pDlg->InsertSystemString(chInfo);
#endif
		//设置扑克
		BYTE bTemp[]={0,0,0};
		m_CardControl[m_wLoserUser].SetCardColor(2);
		m_CardControl[m_wCompareChairID[0]].SetCardData(bTemp,MAX_COUNT);
		m_CardControl[m_wCompareChairID[1]].SetCardData(bTemp,MAX_COUNT);

		//设置变量
		m_wConmareIndex=0;
		m_wConmareCount=0;
		m_wLoserUser=INVALID_CHAIR;
		ZeroMemory(m_wCompareChairID,sizeof(m_wCompareChairID));

		//发送消息
		AfxGetMainWnd()->SendMessage(IDM_FALSH_CARD_FINISH,0,0);

		//更新界面
		UpdateGameView(NULL);

		return false;
	}

	//更新界面
	UpdateGameView(NULL);
	return true;
}

//停止闪牌
void CGameClientView::StopFlashCard()
{
	//动画判断
	//BYTE byUserID = 0;
	//for(;byUserID<GAME_PLAYER;byUserID++)
	//{
	//	if(m_wFlashUser[byUserID]<GAME_PLAYER)
	//		break;
	//}
	//if(byUserID==GAME_PLAYER)return;

	//设置参数
	m_bFalsh=false;
	m_bStopDraw=false;
	m_wFalshCount=0;
	for(int i=0;i<GAME_PLAYER;i++)
	{
		if(m_wFlashUser[i]<GAME_PLAYER)
		{
			m_CardControl[m_wFlashUser[i]].SetCardColor(INVALID_CHAIR);
			m_wFlashUser[i]=INVALID_CHAIR;
		}
	}

	//完成处理
	//AfxGetMainWnd()->SendMessage(IDM_FALSH_CARD_FINISH,0,0);

	return ;
}

//停止比牌
void CGameClientView::StopCompareCard()
{
	if(m_wLoserUser>GAME_PLAYER)return ;

	//设置扑克

#ifdef ZJH_GAME_PRINT_INFO
	CGameClientDlg* pDlg = (CGameClientDlg*)GetParent();
	char chInfo[100]={0};
	::sprintf(chInfo, "停止视图比牌动画结束,输家:%d", m_wLoserUser);
	pDlg->InsertSystemString(chInfo);
#endif

	BYTE bTemp[]={0,0,0};
	m_CardControl[m_wLoserUser].SetCardColor(2);
	m_CardControl[m_wCompareChairID[0]].SetCardData(bTemp,MAX_COUNT);
	m_CardControl[m_wCompareChairID[1]].SetCardData(bTemp,MAX_COUNT);

	//设置变量
	m_wConmareIndex=0;
	m_wConmareCount=0;
	m_wLoserUser=INVALID_CHAIR;
	ZeroMemory(m_wCompareChairID,sizeof(m_wCompareChairID));

	//发送消息
	//AfxGetMainWnd()->SendMessage(IDM_FALSH_CARD_FINISH,0,0);

	return ;
}


//完成发牌
void CGameClientView::FinishDispatchCard()
{
	//完成判断
	if (m_SendCardItemArray.GetCount()==0) return;

	//完成动画
	for (INT_PTR i=0;i<m_SendCardItemArray.GetCount();i++)
	{
		//获取数据
		tagSendCardItem * pSendCardItem=&m_SendCardItemArray[i];

		//获取扑克
		BYTE cbCardData[MAX_COUNT];
		WORD wChairID=pSendCardItem->wChairID;
		BYTE cbCardCount=(BYTE)m_CardControl[wChairID].GetCardData(cbCardData,CountArray(cbCardData));

		//设置扑克
		if (cbCardCount<MAX_COUNT)
		{
			cbCardData[cbCardCount++]=pSendCardItem->cbCardData;
			m_CardControl[wChairID].SetCardData(cbCardData,cbCardCount);
		}
	}

	//删除处理
	m_SendCardItemArray.RemoveAll();

	//完成处理
	AfxGetMainWnd()->SendMessage(IDM_SEND_CARD_FINISH,0,0);

	//更新界面
	UpdateGameView(NULL);

	return;
}

//定时器消息
void CGameClientView::OnTimer(UINT nIDEvent)
{
	if (nIDEvent==IDI_CARTOON)		//动画到时
	{
		//变量定义
		bool bKillTimer=true;

		//闪牌动画
		//if(m_bStopDraw && (FlashCard()==true))
		//{
		//	bKillTimer=false;
		//}

		//比牌动画
		if ((m_wLoserUser!=INVALID_CHAIR)&&(CompareCard()))
		{
			bKillTimer=false;
		}
		//发牌动画
		else if ((m_SendCardItemArray.GetCount()>0)&&(SendCard()))
		{
			bKillTimer=false;
		}

		//删除时间
		if (bKillTimer==true)
		{
			m_bCartoon=false;
			KillTimer(IDI_CARTOON);
		}

		return;
	}

	//筹码动画
	if( IDI_MOVE_JETTON==nIDEvent)
	{
		if( !m_JettonControl.PlayMoveJettons() )
		{
			KillTimer(IDI_MOVE_JETTON);

			////发送消息
			//AfxGetMainWnd()->PostMessage(IDM_MOVE_JET_FINISH,0,0);

			////显示筹码添加信息
			//if( m_lJettonScore < m_JettonControl.GetScore() )
			//{
			//	SetTimer( IDI_ACTION_ADD_JETTON,TIME_ACTION_ADD_JETTON,NULL );
			//}

			////播放声音
			//if( m_bPlayingActionSound )
			//	m_bMovingJetton = true;
			//else
			//{
			//	CGameClientDlg * pGameDlg=(CGameClientDlg *)AfxGetMainWnd();
			//	pGameDlg->PlayGameSound(AfxGetInstanceHandle(),TEXT("ADD_SCORE"));
			//}

			UpdateGameView( NULL );

			return;
		}

		//获取更新区域
		CRect rcDraw;
		m_JettonControl.GetDrawRect(rcDraw);
		UpdateGameView(&rcDraw);

		return;
	}

	//数字滚动
	if( IDI_MOVE_NUMBER==nIDEvent )
	{
		BOOL bMoving = FALSE;
		for( INT i = 0; i < GAME_PLAYER; i++ )
		{
			if( m_NumberControl[i].PlayScrollNumber() )
			{
				bMoving = TRUE;
				//获取更新区域
				CRect rc;
				m_NumberControl[i].GetDrawRect(rc);
				UpdateGameView(&rc);
			}
		}
		if( !bMoving )
			KillTimer(IDI_MOVE_NUMBER);
		return ;
	}

	__super::OnTimer(nIDEvent);
}

//////////////////////////////////////////////////////////////////////////

