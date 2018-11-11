#include "StdAfx.h"
#include "Resource.h"
#include "GameClientView.h"
#include <math.h>
#include "GameClient.h"
#include "ActionWnd.h"
#include ".\gameclientview.h"
#include "GameClientDlg.h"

#include <math.h>

#include "TraceCatch.h"

//#include <fstream>
//
//using namespace std;
//std::ofstream out("land_title.txt", std::ios::trunc); 
//extern void WriteInfo( char* p)
//{
//	out << p << std::endl;
//}

//////////////////////////////////////////////////////////////////////////
//按钮标识 

#define IDC_START						100								//开始按钮
#define IDC_CALLLAND					101								//叫地主按钮
#define IDC_GIVE_UP_CALLLAND			102								//不叫按钮
#define IDC_SNATCHLAND					103								//抢地主按钮
#define IDC_GIVE_UP_SANTCHLAND			104								//不抢按钮
#define IDC_OUT_CARD					105								//出牌按钮
#define IDC_PASS_CARD					106								//PASS按钮
#define IDC_AUTO_OUTCARD                107                             //提示按纽
#define IDC_AUTOPLAY_ON					108								//托管按钮
#define IDC_AUTOPLAY_OFF				109								//取消按钮
#define IDC_SORT_CARD			        110                             //排序按纽

//#define IDC_SNATCH_LAND					111							//抢庄按钮
#define IDC_BRIGHT_CARD					112								//明牌按钮
#define IDC_BRIGHT_START				113								//明牌开始
//cxf 倒-跟-反 替代加倍
//#define IDC_DOUBLE_SCORE				114								//加倍消息
//#define IDC_NOT_DOUBLE_SCORE			115								//不加倍消息
//#define IDC_CARD_DAO					116								//倒牌
//#define IDC_CARD_GEN					117								//跟牌
//#define IDC_CARD_FAN					118								//反牌
//cxf end 
#define IDC_LAST_ROUND                  119                             //上一轮 lq

//动画数目
#define BOMB_EFFECT_COUNT				6								//爆炸数目
#define SPRING_EFFECT_COUNT             15                              //春天*反春数目 //lq

//动画标识
#define IDI_BOMB_EFFECT					101								//爆炸标识
#define IDI_SPRING_EFFECT               102                             //春天标识  //lq
#define IDI_FANSPRING_EFFECT            103                             //反春标识  //lq
#define IDI_ACTION_EFFECT               105                             //特效标识	//lq
#define IDI_HUOJIAN_EFFECT              106                             //火箭标识
//
#define IDP_SHUNZI                       0                              //顺子类型
#define IDP_LIANDUI                      1                              //连对类型

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CGameClientView, CGameFrameView)
	ON_WM_TIMER()
	ON_WM_CREATE()
	ON_WM_RBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_BN_CLICKED(IDC_START, OnStart)
	ON_BN_CLICKED(IDC_OUT_CARD, OnOutCard)
	ON_BN_CLICKED(IDC_PASS_CARD, OnPassCard)
	ON_BN_CLICKED(IDC_CALLLAND, OnCallLand)
	ON_BN_CLICKED(IDC_GIVE_UP_CALLLAND, OnGiveUpCallLand)
	ON_BN_CLICKED(IDC_SNATCHLAND, OnSnatchLand)
	ON_BN_CLICKED(IDC_AUTO_OUTCARD, OnAutoOutCard)
	ON_BN_CLICKED(IDC_GIVE_UP_SANTCHLAND, OnGiveUpSnatchLand)
	ON_BN_CLICKED(IDC_AUTOPLAY_ON, OnAutoPlayerOn)
	ON_BN_CLICKED(IDC_AUTOPLAY_OFF,OnAutoPlayerOff)
	ON_BN_CLICKED(IDC_SORT_CARD, OnBnClickedSortCard)
	//ON_BN_CLICKED(IDC_SNATCH_LAND, OnSnatchLand)
	ON_BN_CLICKED(IDC_BRIGHT_CARD, OnBrightCard)
	ON_BN_CLICKED(IDC_BRIGHT_START, OnBrightStart)
	//ON_BN_CLICKED(IDC_DOUBLE_SCORE, OnDoubleScore)
	//ON_BN_CLICKED(IDC_NOT_DOUBLE_SCORE, OnNotDoubleScore)
	/*ON_BN_CLICKED(IDC_CARD_DAO, OnDoubleScoreDao)
	ON_BN_CLICKED(IDC_CARD_GEN, OnDoubleScoreGen)
	ON_BN_CLICKED(IDC_CARD_FAN, OnDoubleScoreFan)*/
	ON_BN_CLICKED(IDC_LAST_ROUND, OnLastRound)
	ON_WM_LBUTTONUP()
	ON_WM_SHOWWINDOW()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CGameClientView::CGameClientView(void) : CGameFrameView(true,24)
{
//#if _DEBUG
//	//DebugBreak();	// 测试用
//#endif
	__ENTER_FUNCTION 
	//游戏变量
	m_lBaseScore=0;
	m_wBombTime=0;
	m_cbLandScore=0;

	m_bDisplay=false;
	m_wLandUser=INVALID_CHAIR;
	memset(m_bCardBackDate,0,sizeof(m_bCardBackDate));
	ZeroMemory(m_bCardData,sizeof(m_bCardData));
	ZeroMemory(m_bHandCardCount,sizeof(m_bHandCardCount));
	
	//状态变量
	m_bLandTitle=false;
	memset(m_bPass,0,sizeof(m_bPass));
	memset(m_bScore,0,sizeof(m_bScore));
	memset(m_bCardCount,0,sizeof(m_bCardCount));
	memset(m_bUserTrustee, 0, sizeof( m_bUserTrustee ) );
	for(BYTE i=0; i<GAME_PLAYER; ++i) m_bShowWarming[i] = false ;

	//疯狂变量
	m_cbCallScorePhase = CSD_NORMAL;
	ZeroMemory( m_cbUserPhase, sizeof( m_cbUserPhase ) );
	ZeroMemory( m_bBrightStart, sizeof( m_bBrightStart ) );
	m_cbBrightTime=0;
	m_bShowLandTime=false;
	ZeroMemory( m_bDoubleScore, sizeof( m_bDoubleScore ) );
	
	m_bTXEffect=false;
	m_bTXFrameIndex=0;
	m_bHuoJian=false;
	m_bHuojianIndex=0;


	//位置信息
	memset(m_ptScore,0,sizeof(m_ptScore));
	memset(m_ptLand,0,sizeof(m_ptLand));
	m_bDeasilOrder = false;

	//加载资源
	HINSTANCE hInstance=AfxGetInstanceHandle();
	m_ImageBomb.SetLoadInfo(IDB_BOMB,hInstance);

	
	CString strPath = "";
	GetPicPath(strPath);
	//PNG资源
	m_ImageWaitScore.LoadImage(TEXT(strPath + "WAITSCORE.png"));
	m_ImageScore.LoadImage(TEXT(strPath+"SCORE.png") );
	m_ImageLand.LoadImage(TEXT(strPath+"BANKER.png"));
	m_ImageUserTrustee.LoadImage(TEXT(strPath+"USER_TRUSTEE.png"));
	m_ImageLandTimes.LoadImage(TEXT(strPath+"LAND_TIMES.png"));
    m_PngCardBack.LoadImage(TEXT(strPath+"CARD.png"));
	m_Imagebigness.LoadImage(TEXT(strPath+"BIGNESS.png") );
	m_ImageBack.LoadImage(TEXT(strPath+"BACK.png"));
	m_ImageCardNum.LoadImage(TEXT(strPath+"CARDNUMIMAGE.png"));
	m_ImageTCard.LoadImage(TEXT(strPath+("CARDIMAGE.png")));
	m_ImageLandbei.LoadImage(TEXT(strPath+"LANDBEI.png"));
	m_XieCard.LoadImage(TEXT(strPath+"XIECARD.png"));
	m_XieCardThree.LoadImage(TEXT(strPath+"XIECARDTHREE.png"));
	m_XieCardTwo.LoadImage(TEXT(strPath+"XIECARDTWO.png"));
	m_XieCardOne.LoadImage(TEXT(strPath+"XIECARDONE.png"));
	m_ImageGrand.LoadImage(TEXT(strPath+"GRAND.png"));
	m_ImageGrandFrame.LoadImage(TEXT(strPath+"GRANDFRAME.png"));
	m_ImagePass.LoadImage(TEXT(strPath+"PASSBY.png"));

	//计算位置
	m_LandSize.cx=m_ImageLand.GetWidth()/4;
	m_LandSize.cy=m_ImageLand.GetHeight();

	//清空剩余记录
	memset(&m_UserTSCardNums, 0, sizeof(m_UserTSCardNums));
	for (int i=0; i<3; i++)
	{
		m_UserTSCardNums[i].bIsTS = true;
	}

    //图片资源是否显示变量初始化
	m_bIsShowbigness = false;
	m_bIsShowZhaDan=false;
	m_bIsShowSpring=false;

	//新的牌控件
	m_GameCard.SetViewWndPtr(this);
	m_GameCard.SetCardImage(TEXT(strPath+"CARD.png"));
	m_GameCard.SetCardSelImage(TEXT(strPath+"CARD_SELECT.png"));
	m_GameCard.SetCardBackImage(&m_ImageBack);
	m_GameCard.SetCardPosition(CPoint(478,463));
	m_GameCard.SetCardShow(true);

	/*m_PngCard.LoadImage(strPath+"abc.png");
	int iWidth = m_PngCard.GetWidth();
	int iHeight = m_PngCard.GetHeight();*/

	//osc
	m_bIsInitBT = false;
	m_bThreeIsInitBT=false;
    __LEAVE_FUNCTION
}

void CGameClientView::GetPicPath(CString& strPath)
{
	__ENTER_FUNCTION 
		
	strPath = "";
	TCHAR szResourcePath[MAX_PATH]={0};
	GetCurrentDirectory(MAX_PATH,szResourcePath);
	strPath.Append(szResourcePath);
	strPath.Append("\\LandCrazy\\picture\\");
	__LEAVE_FUNCTION 
}
//析构函数
CGameClientView::~CGameClientView(void)
{
	__ENTER_FUNCTION 
	__LEAVE_FUNCTION 
}

//建立消息
int CGameClientView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	__ENTER_FUNCTION 
	
	if (__super::OnCreate(lpCreateStruct)==-1) return -1;

	//创建控件
	CRect CreateRect(0,0,0,0);
	//ActionWnd * pDlg = new ActionWnd();
	//pDlg->Create( ActionWnd::IDD, this);
    //pDlg->SetWindowPos(NULL, 0, 0, 220, 120, SWP_NOMOVE);
	//pDlg->ShowWindow(SW_SHOW);

	////创建扑克
	for (WORD i=0;i<3;i++)
	{
		//用户扑克  
		//出牌显示均为横向显示9.21
		m_UserCardControl[i].SetDirection(true);
		
		m_UserCardControl[i].SetDisplayFlag(true);  	
		m_UserCardControl[i].SetCardSpace( 20, 24, 16 );

		m_UserCardControl[i].Create(NULL,NULL,WS_VISIBLE|WS_CHILD,CreateRect,this,20+i);

		//用户扑克
		if (i!=1)
		{
			m_HandCardControl[i].SetCardSpace(28,23,0);
			m_HandCardControl[i].SetDirection(false);
			m_HandCardControl[i].SetDisplayFlag(false);
			m_HandCardControl[i].Create(NULL,NULL,WS_VISIBLE|WS_CHILD,CreateRect,this,30+i);
		}
		else
		{
			//m_HandCardControl[i].SetCardSpace( 40, 24, 16 );
			//m_HandCardControl[i].SetDirection(true);
			//m_HandCardControl[i].SetDisplayFlag(false);
			//m_HandCardControl[i].Create(NULL,NULL,WS_VISIBLE|WS_CHILD,CreateRect,this,30+i);
		}
	}

	//设置扑克
	//m_HandCardControl[1].SetSinkWindow(AfxGetMainWnd());

	//创建按钮
	m_btStart.Create(TEXT(""),WS_CHILD,CreateRect,this,IDC_START);
	m_btOutCard.Create(TEXT(""),WS_CHILD,CreateRect,this,IDC_OUT_CARD);
	m_btPassCard.Create(TEXT(""),WS_CHILD,CreateRect,this,IDC_PASS_CARD);
	m_btAutoOutCard.Create(TEXT(""),WS_CHILD,CreateRect,this,IDC_AUTO_OUTCARD);
	m_btCallLand.Create(TEXT(""),WS_CHILD,CreateRect,this,IDC_CALLLAND);
	m_btGiveUpCallLand.Create(TEXT(""),WS_CHILD,CreateRect,this,IDC_GIVE_UP_CALLLAND);
	m_btSnatchLand.Create(TEXT(""),WS_CHILD,CreateRect,this,IDC_SNATCHLAND);
	m_btGiveUpSnatchLand.Create(TEXT(""),WS_CHILD,CreateRect,this,IDC_GIVE_UP_SANTCHLAND);
	m_btAutoPlayOn.Create(TEXT(""), WS_CHILD ,CreateRect,this,IDC_AUTOPLAY_ON);
	m_btAutoPlayOff.Create(TEXT(""),WS_CHILD,CreateRect,this,IDC_AUTOPLAY_OFF);
	m_btSortCard.Create(NULL,WS_CHILD,CreateRect,this,IDC_SORT_CARD);
	/*m_btCardDao.Create(TEXT(""),WS_CHILD,CreateRect,this,IDC_CARD_DAO);
	m_btCardGen.Create(TEXT(""),WS_CHILD,CreateRect,this,IDC_CARD_GEN);
	m_btCardFan.Create(TEXT(""),WS_CHILD,CreateRect,this,IDC_CARD_FAN);*/
	/*m_btSnatchLand.Create(TEXT(""),WS_CHILD,CreateRect,this,IDC_SNATCH_LAND);*/
	//m_btBrightCard.Create(TEXT(""),WS_CHILD,CreateRect,this,IDC_BRIGHT_CARD);	
	/*m_btBrightStart.Create(TEXT(""),WS_CHILD,CreateRect,this,IDC_BRIGHT_START);*/
	//cxf 删除加倍按钮  用(倒-跟-反)代替
	//m_btDoubleScore.Create(TEXT(""),WS_CHILD,CreateRect,this,IDC_DOUBLE_SCORE);
	//m_btNotDoubleScore.Create(TEXT(""),WS_CHILD,CreateRect,this,IDC_NOT_DOUBLE_SCORE);
	//m_bLastRound.Create(TEXT(""),WS_CHILD,CreateRect,this,IDC_LAST_ROUND);//lq
	
    //cxf end
	//设置按钮
	CString strPath = "";
	GetPicPath(strPath);
	/*COLORREF btColor = RGB(255,0,255);
	HINSTANCE hInstance=AfxGetInstanceHandle();*/
	m_btStart.SetButtonImage(TEXT(strPath + "START.png"), false, 4);
	m_btOutCard.SetButtonImage(TEXT(strPath+"OUT_CARD.png"),false,4);
	m_btPassCard.SetButtonImage(TEXT(strPath+"PASS.png"),false,4);
	m_btAutoOutCard.SetButtonImage(TEXT(strPath+"AUTO_OUT_CARD.png"),false,4);
	m_btCallLand.SetButtonImage(TEXT(strPath+"CALLLAND.png"),false,4);
	m_btGiveUpCallLand.SetButtonImage(TEXT(strPath+"GIVEUP_CALLLAND.png"),false,4);
    m_btSnatchLand.SetButtonImage(TEXT(strPath+"SNATCHLAND.png"),false,4);
    m_btGiveUpSnatchLand.SetButtonImage(TEXT(strPath + "GIVEUP_SNATCHLAND.png"),false,4);
    m_btAutoPlayOn.SetButtonImage(TEXT(strPath+"AUTOPLAYON.png"),false,4);
	m_btAutoPlayOff.SetButtonImage(TEXT(strPath+"AUTOPLAYOFF.png"),false,4);
    m_btSortCard.SetButtonImage(TEXT(strPath+"SORTCARD.png"),false,4);
 /*   m_btCardDao.SetButtonImage(TEXT(strPath+"DAO.png"),false,4);
    m_btCardGen.SetButtonImage(TEXT(strPath+"GEN.png"),false,4);
    m_btCardFan.SetButtonImage(TEXT(strPath+"FAN.png"),false,4);*/

	//m_bLastRound.SetButtonImage(hInstance,IDB_LAST_ROUND,btColor,5);//上一轮 lq
	//cxf end
	//m_btStart.ShowWindow(SW_SHOW);
	//m_btAutoPlayOn.ShowWindow(SW_SHOW);
	//m_btAutoPlayOn.EnableWindow(FALSE);
	//m_btAutoPlayOff.ShowWindow(SW_HIDE);
	//m_btOutCard.ShowWindow(SW_SHOW);
	//m_btPassCard.ShowWindow(SW_SHOW);
	//m_btAutoOutCard.ShowWindow(SW_SHOW);
	//m_btOneScore.ShowWindow(SW_SHOW);
	//m_btTwoScore.ShowWindow(SW_SHOW);
	//m_btGiveUpScore.ShowWindow(SW_SHOW);
	//m_btThreeScore.ShowWindow(SW_SHOW);


	/*BYTE byCardData[20]={0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x11,0x12,0x13,0x14,0x15,0x16,0x17};
	m_HandCardControl[1].SetCardData(byCardData, 20);
	m_HandCardControl[1].SetDisplayFlag(true);
	m_HandCardControl[1].SetPositively(true);
	m_HandCardControl[1].ShowWindow(SW_SHOW);*/

	__LEAVE_FUNCTION
	return 0;
	 
}

//重置界面
void CGameClientView::ResetGameView()
{
    __ENTER_FUNCTION 

	//游戏变量
	m_lBaseScore=0;
	m_wBombTime=0;
	m_cbLandScore=0;
	m_wLandUser=INVALID_CHAIR;
	m_bDisplay=false;
	memset(m_bCardBackDate,0,sizeof(m_bCardBackDate));

	//疯狂变量
	ZeroMemory( m_cbUserPhase, sizeof( m_cbUserPhase ) );
	ZeroMemory( m_bBrightStart, sizeof( m_bBrightStart ) );
	m_cbBrightTime=0;
	m_bShowLandTime=false;
	ZeroMemory( m_bDoubleScore, sizeof( m_bDoubleScore ) );

	m_bTXEffect=false;
	m_bTXFrameIndex=0;
	m_bHuoJian=false;
	m_bHuojianIndex=0;
	
	//状态变量
	m_bLandTitle=false;
	memset(m_bPass,0,sizeof(m_bPass));
	memset(m_bScore,0,sizeof(m_bScore));
	memset(m_bCardCount,0,sizeof(m_bCardCount));
	memset(m_bUserTrustee, 0, sizeof( m_bUserTrustee ) );
	for(BYTE i=0; i<GAME_PLAYER; ++i) m_bShowWarming[i] = false ;

	//隐藏按钮
	m_btStart.ShowWindow(SW_HIDE);
	m_btOutCard.ShowWindow(SW_HIDE);
	m_btPassCard.ShowWindow(SW_HIDE);
	/*m_btGiveUpScore.ShowWindow(SW_HIDE);*/
	m_btAutoOutCard.ShowWindow(SW_HIDE);
	//m_btThreeScore.ShowWindow(SW_HIDE);
	m_btAutoPlayOn.ShowWindow( SW_SHOW );
	m_btAutoPlayOn.EnableWindow(FALSE);
	m_btAutoPlayOff.ShowWindow( SW_HIDE );

	/*m_btSnatchLand.ShowWindow(SW_HIDE);*/
	//m_btBrightCard.ShowWindow(SW_HIDE);
	/*m_btBrightStart.ShowWindow(SW_HIDE);*/

	//禁用控件
	m_btOutCard.EnableWindow(FALSE);
	m_btPassCard.EnableWindow(FALSE);
	m_btAutoOutCard.EnableWindow(FALSE);
	m_btSortCard.EnableWindow(FALSE);
	

	HINSTANCE hInstance = AfxGetInstanceHandle();
	//m_btSortCard.LoadRgnImage(hInstance,IDB_COUNT_SORT,RGB(255,0,255));
	//m_btGiveUpScore.LoadRgnImage(hInstance,IDB_GIVE_UP,RGB(255,0,255));

	//扑克控件
	//m_HandCardControl[1].SetPositively(false);
	//m_HandCardControl[1].SetDisplayFlag(false);
	m_GameCard.SetCardPositively(false);
	m_GameCard.SetCardShow(false);

	//设置扑克
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		m_UserCardControl[i].SetCardData(NULL,0);
		m_HandCardControl[i].SetCardData(NULL,0);
	}

	//隐藏扑克
	//m_HandCardControl[ 0 ].ShowWindow( SW_HIDE );
	//m_HandCardControl[ 2 ].ShowWindow( SW_HIDE );

	//清空剩余记录
	memset(&m_UserTSCardNums, 0, sizeof(m_UserTSCardNums));
	for (int i=0; i<3; i++)
	{
		m_UserTSCardNums[i].bIsTS = true;
	}

	__LEAVE_FUNCTION
}

//调整控件
void CGameClientView::RectifyGameView(int nWidth, int nHeight)
{
   __ENTER_FUNCTION 

	CRect rect;
	GetWindowRect(&rect);
	nWidth=rect.right-rect.left;
	nHeight=rect.bottom-rect.top;

	////顺时针出
	//if ( m_bDeasilOrder ) 
	//{
	//	//设置坐标
	//	m_ptFace[2].x=m_nXBorder+5;
	//	m_ptFace[2].y=nHeight/2-m_nYFace;
	//	m_ptName[2].x=m_nXBorder+5;
	//	m_ptTimer[2].x=nWidth-m_nXBorder-m_nXTimer/2+20;
	//	m_ptTimer[2].y=nHeight/2-m_nYFace-m_nYTimer/2-105;
	//	m_ptReady[2].x=m_nXBorder+5+m_nXTimer;
	//	m_ptReady[2].y=nHeight/2-m_nYFace-m_nYTimer/2-5;
	//	m_ptScore[2].x=m_nXBorder+m_nXFace+147-10;
	//	m_ptScore[2].y=nHeight/2-m_nYFace-30;
	//	m_ptLand[2].x=m_nXBorder+5;
	//	m_ptLand[2].y=nHeight/2+40;
	//	m_ptWarmingCard[2].x = m_ptLand[2].x;
	//	m_ptWarmingCard[2].y = m_ptLand[2].y + 40;

	//	//设置坐标
	//	m_ptFace[0].x=nWidth-m_nXBorder-m_nXFace-5;
	//	m_ptFace[0].y=nHeight/2-m_nYFace;
	//	m_ptName[0].x=nWidth-m_nXBorder-5;
	//	m_ptName[0].y=nHeight/2+5;
	//	m_ptTimer[0].x=m_nXBorder-20+m_nXTimer/2;
	//	m_ptTimer[0].y=nHeight/2-m_nYFace-105-m_nYTimer/2;
	//	m_ptReady[0].x=nWidth-m_nXBorder-m_nXTimer-5;
	//	m_ptReady[0].y=nHeight/2-m_nYFace-m_nYTimer/2-5;
	//	m_ptScore[0].x=nWidth-m_nXBorder-m_nXFace-190-7-m_ImageScore.GetWidth()/11;
	//	m_ptScore[0].y=nHeight/2-m_nYFace-30;
	//	m_ptLand[0].x=nWidth-m_nXBorder-m_LandSize.cx-5;
	//	m_ptLand[0].y=nHeight/2+40;
	//	m_ptWarmingCard[0].x = m_ptLand[0].x-40;
	//	m_ptWarmingCard[0].y =m_ptLand[0].y + 40;

	//	//用户扑克
	//	m_UserCardControl[0].SetBenchmarkPos(nWidth-m_nXFace-m_nXBorder-137-120,nHeight/2-80,enXRight,enYCenter);
	//	m_UserCardControl[2].SetBenchmarkPos(m_nXBorder+m_nXFace+137+120,nHeight/2-80,enXLeft,enYCenter);
	//	m_HandCardControl[0].SetBenchmarkPos(nWidth-m_nXBorder-m_nXFace-57,nHeight/2-50,enXRight,enYCenter);
	//	m_HandCardControl[2].SetBenchmarkPos(m_nXBorder+m_nXFace+57,nHeight/2-55,enXLeft,enYCenter);
	//}
	//else 
	//{
		//设置坐标
		//0:左边; 1:下面; 2:右边
		//头像
		m_ptFace[0].x=17;
		m_ptFace[0].y=89;
		//名字
		m_ptName[0].x=m_nXBorder+52;
		m_ptName[0].y=14;
		//时间
		m_ptTimer[0].x=326;
		m_ptTimer[0].y=266;
		//准备
		m_ptReady[0].x=250;
		m_ptReady[0].y=250;
		//托管
		m_ptUserTrustee[0].x = 130;
		m_ptUserTrustee[0].y = 373;
		//
		m_ptScore[0].x=240;
		m_ptScore[0].y=290;
		//pass
		m_OutCardPass[0].x=250;
		m_OutCardPass[0].y=260;
		//地主农民图标位置
		m_ptLand[0].x=153;
		m_ptLand[0].y=111;
		////报牌位置
		m_ptWarmingCard[0].x = m_ptLand[0].x;
		m_ptWarmingCard[0].y = m_ptTimer[0].y + 40;
		//宝石
		m_ptUserBaoShi[0].x=55;
		m_ptUserBaoShi[0].y=137;
		//等级
		m_ptUserGrade[0].x=60;
		m_ptUserGrade[0].y=337;
		//等级框
        m_GrandFrame[0].x=8;
		m_GrandFrame[0].y=324;
		//断线位置
		m_UserDuanXian[0].x=280;
		m_UserDuanXian[0].y=250;
		//报牌位置
		m_ptUserSYPTS[0].x = 80;
		m_ptUserSYPTS[0].y = 40;
		//开始明牌
		m_ptBrightStart[0] = m_ptScore[0];
		//语音框位置
		m_ptSayPos[0].x = m_ptName[0].x;
		m_ptSayPos[0].y = m_ptName[0].y+180;
		//等级称谓
		m_Rating[0].x=40;
		m_Rating[0].y=365;

		//设置坐标
		m_ptFace[1].x=16;
		m_ptFace[1].y=456;
		m_ptName[1].x=52;
		m_ptName[1].y=nHeight-m_nYBorder-234;
		m_ptTimer[1].x=202;
		m_ptTimer[1].y=nHeight-m_nYBorder-m_nYTimer-230+45;
		m_ptReady[1].x=nWidth/2-30;
		m_ptReady[1].y=nHeight-217-m_nYBorder;
		m_ptScore[1].x=nWidth/2-51;
		m_ptScore[1].y=nHeight-m_nYBorder-m_nYFace-180;
		//pass
		m_OutCardPass[1].x=nWidth/2-30;
		m_OutCardPass[1].y=nHeight-m_nYBorder-m_nYFace-180;
		m_ptLand[1].x=160;
		m_ptLand[1].y=486;
		//托管
		m_ptUserTrustee[1].x = 430;
		m_ptUserTrustee[1].y =320;
		//宝石
		m_ptUserBaoShi[1].x=55;
		m_ptUserBaoShi[1].y=nHeight-m_nYBorder-112;
		//等级
		m_ptUserGrade[1].x=55;
		m_ptUserGrade[1].y=nHeight-m_nYBorder-94;
		//报牌位置
		m_ptUserSYPTS[1].x = 120;
		m_ptUserSYPTS[1].y = nHeight-300;
		
		//开始明牌
		m_ptBrightStart[1] = m_ptScore[1];
        //语音框位置
		m_ptSayPos[1].x = m_ptName[1].x;
		m_ptSayPos[1].y = m_ptName[1].y;
		//等级称谓
		m_Rating[1].x=80;
		m_Rating[1].y=500;


		//设置坐标
		m_ptFace[2].x=722;
		m_ptFace[2].y=92;
		m_ptName[2].x=nWidth-m_nXBorder-80;
		m_ptName[2].y=14;
		m_ptTimer[2].x=658;
		m_ptTimer[2].y=266;
		m_ptReady[2].x=nWidth-m_nXBorder-250;
		m_ptReady[2].y=250;
		m_ptScore[2].x=m_ptReady[2].x-95;
		m_ptScore[2].y=290;
		m_OutCardPass[2].x=m_ptReady[2].x-95;
		m_OutCardPass[2].y=260;
		m_ptLand[2].x=nWidth-207;
		m_ptLand[2].y=111;
		m_ptWarmingCard[2].x = m_ptLand[2].x-40;
		m_ptWarmingCard[2].y =m_ptTimer[2].y + 40;
		//托管
		m_ptUserTrustee[2].x = 748;
		m_ptUserTrustee[2].y = 373;
		//宝石
		m_ptUserBaoShi[2].x=nWidth-77;
		m_ptUserBaoShi[2].y=137;
		//等级
		m_ptUserGrade[2].x=900;
		m_ptUserGrade[2].y=337;
		//等级框
		m_GrandFrame[2].x=847;
		m_GrandFrame[2].y=323;
		//断线位置
		m_UserDuanXian[2].x=550;
		m_UserDuanXian[2].y=250;
		//报牌位置
		m_ptUserSYPTS[2].x = nWidth-220;
		m_ptUserSYPTS[2].y = 40;
		//开始明牌
		m_ptBrightStart[2] = m_ptScore[2];
        //语音框位置
		m_ptSayPos[2].x = m_ptName[2].x;
		m_ptSayPos[2].y = m_ptName[2].y+180;
		//等级称谓
		m_Rating[2].x=880;
		m_Rating[2].y=364;

		//用户扑克基准位置
		m_UserCardControl[2].SetBenchmarkPos(690,270,enXRight,enYCenter);
		m_UserCardControl[0].SetBenchmarkPos(281,270,enXLeft,enYCenter);
		m_HandCardControl[2].SetBenchmarkPos(900,nHeight/2-80,enXRight,enYCenter);
		m_HandCardControl[0].SetBenchmarkPos(40,nHeight/2-80,enXLeft,enYCenter);

		//m_UserCardControl[2].SetBenchmarkPos(nWidth-m_nXFace-m_nXBorder-100,nHeight/2-100,enXCenter,enYBottom);
		//m_UserCardControl[0].SetBenchmarkPos(m_nXBorder+m_nXFace+100,nHeight/2-100,enXCenter,enYBottom);
		

		m_HandCardControl[2].ShowWindow(SW_HIDE);
		m_HandCardControl[0].ShowWindow(SW_HIDE);

		m_ptBrightStart[1].y -= 35;
		m_ptBrightStart[1].x -= 20;
	//}
		
	//移动按钮
	CRect rcButton;
	HDWP hDwp=BeginDeferWindowPos(32);
	m_btStart.GetWindowRect(&rcButton);
	const UINT uFlags=SWP_NOACTIVATE|SWP_NOZORDER|SWP_NOCOPYBITS|SWP_NOSIZE; 
	DeferWindowPos(hDwp,m_btStart,NULL,405,292,0,0,uFlags);             //开始
	DeferWindowPos(hDwp,m_btOutCard,NULL,315,410,0,0,uFlags);           //出牌
	DeferWindowPos(hDwp,m_btAutoOutCard,NULL,437,410,0,0,uFlags);       //提示
	DeferWindowPos(hDwp,m_btPassCard,NULL,557,410,0,0,uFlags);		    //不出
	DeferWindowPos(hDwp,m_btCallLand,NULL,392,414,0,0,uFlags);          //叫地主
	DeferWindowPos(hDwp,m_btGiveUpCallLand,NULL,502,414,0,0,uFlags);    //不叫地主
	DeferWindowPos(hDwp,m_btSnatchLand,NULL,392,414,0,0,uFlags);        // 抢地主
	DeferWindowPos(hDwp,m_btGiveUpSnatchLand,NULL,502,414,0,0,uFlags);  //不抢地主
	//DeferWindowPos(hDwp,m_btCardDao,NULL,676,400,0,0,uFlags);    //倒
	//DeferWindowPos(hDwp,m_btCardGen,NULL,676,400,0,0,uFlags);    //跟
	//DeferWindowPos(hDwp,m_btCardFan,NULL,676,400,0,0,uFlags);    //反
	DeferWindowPos(hDwp,m_btAutoPlayOn,NULL,355,587,0,0,uFlags);        //托管
	DeferWindowPos(hDwp,m_btAutoPlayOff, NULL,370,587,0,0,uFlags );     //取消托管
	DeferWindowPos(hDwp,m_btSortCard, NULL,532,587,0,0,uFlags );        //排序
	
	//扩展按钮
	//DeferWindowPos(hDwp,m_btSnatchLand,NULL,nWidth/2-rcButton.Width()*2-60,nHeight-217-m_nYBorder-50,0,0,uFlags);   //抢庄
	//DeferWindowPos(hDwp,m_btBrightCard,NULL,nWidth/2-rcButton.Width()*2-30,nHeight-217-m_nYBorder,0,0,uFlags);
	//DeferWindowPos(hDwp,m_btBrightStart,NULL,(nWidth-rcButton.Width())/2+10,nHeight-217-m_nYBorder,0,0,uFlags);
	//cxf 删除加倍按钮  用(倒-跟-反)代替
	//DeferWindowPos(hDwp,m_btDoubleScore,NULL,nWidth/2-rcButton.Width()-97,nHeight-217-m_nYBorder-50,0,0,uFlags);    //加倍
	//DeferWindowPos(hDwp,m_btNotDoubleScore,NULL,nWidth/2+rcButton.Width()-80,nHeight-217-m_nYBorder-50,0,0,uFlags);  //不加倍
	//cxf end
	//DeferWindowPos(hDwp,m_bLastRound,NULL,nWidth-rcButton.Width()-167,nHeight-217-m_nYBorder-80,0,0,uFlags);   //上一轮

	EndDeferWindowPos(hDwp);

	//CRect rcBtAutoPlay;
	//m_btAutoPlayOn.GetWindowRect( &rcBtAutoPlay );

	//移动扑克
	//m_HandCardControl[1].SetBenchmarkPos(nWidth/2-5,nHeight-m_nYBorder-135,enXCenter,enYBottom);
	m_UserCardControl[1].SetBenchmarkPos(nWidth/2-20,nHeight-m_nYBorder-270,enXCenter,enYBottom);

	//调整动画
	//SetFlowerControlInfo((m_bDeasilOrder?2:0),m_nXBorder+5,nHeight/2-BIG_FACE_HEIGHT/2);
	//SetFlowerControlInfo((m_bDeasilOrder?0:2),nWidth-m_nXBorder-5-BIG_FACE_WIDTH,nHeight/2-BIG_FACE_HEIGHT/2);
	//SetFlowerControlInfo(1,nWidth/2-BIG_FACE_WIDTH,nHeight-BIG_FACE_HEIGHT-m_nYBorder-5,BIG_FACE_WIDTH, BIG_FACE_HEIGHT, 30);

	RectifyKeepTimePos(735, 432);
	SetWinLostDrawPoint(CPoint(600,578));
    __LEAVE_FUNCTION 
}

//绘画界面
void CGameClientView::DrawGameView(CDC * pDC, int nWidth, int nHeight)
{
	__ENTER_FUNCTION 

	//绘画背景
	m_ImageBack.DrawImage(pDC,0,0,960,691,0,0);
	
	//绘画输赢局数
	DrawWinLostNumberBk(pDC);

	//绘画游戏结束玩家手牌
	DWORD adwXImagePos=0;
	DWORD adwYImagePos=0;
	for (BYTE i=0; i<GAME_PLAYER;i++)
	{
		if (1 != i)
		{
			if (m_bHandCardCount[i]>0)
			{
				for (BYTE j=0; j<m_bHandCardCount[i]; ++j)
				{
					if ((m_bCardData[i][j]==0x4E)||(m_bCardData[i][j]==0x4F))
					{
						adwXImagePos=((m_bCardData[i][j]&CARD_MASK_VALUE)%14)*m_PngCardBack.GetWidth()/13;
						adwYImagePos=((m_bCardData[i][j]&CARD_MASK_COLOR)>>4)*m_PngCardBack.GetHeight()/5;
					}
					else
					{
						adwXImagePos=((m_bCardData[i][j]&CARD_MASK_VALUE)-1)*m_PngCardBack.GetWidth()/13;
						adwYImagePos=((m_bCardData[i][j]&CARD_MASK_COLOR)>>4)*m_PngCardBack.GetHeight()/5;
					}
					if (0 == i)
					{
						if (j>9)
						{
							m_PngCardBack.DrawImage(pDC,210+(j-10)*20,233,m_PngCardBack.GetWidth()/13,m_PngCardBack.GetHeight()/5,adwXImagePos,adwYImagePos);
						}
						else
						{
							m_PngCardBack.DrawImage(pDC,210+j*20,210,m_PngCardBack.GetWidth()/13,m_PngCardBack.GetHeight()/5,adwXImagePos,adwYImagePos);
						}
							
					}
					else if(2 == i)
					{
						if (j>9)
						{
							m_PngCardBack.DrawImage(pDC,485+(j-10)*20+80,233,m_PngCardBack.GetWidth()/13,m_PngCardBack.GetHeight()/5,adwXImagePos,adwYImagePos);
						}
						else
						{
							m_PngCardBack.DrawImage(pDC,485+j*20+80,210,m_PngCardBack.GetWidth()/13,m_PngCardBack.GetHeight()/5,adwXImagePos,adwYImagePos);
						}		
					}

				}
			}
		}	
	}

	//绘画用户
	TCHAR szBuffer[32];
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		//变量定义
		const tagUserData * pUserData=GetUserInfo(i);

		//绘画用户
		if (pUserData!=NULL)
		{
			//用户名字
			UINT iAlignType = pDC->SetTextAlign( TA_CENTER );
			pDC->SetTextColor(RGB(220,220,220));
			//DrawTextString( pDC,pUserData->szName, RGB( 255, 255, 255 ), RGB( 0, 0, 0 ), m_ptName[i].x,m_ptName[i].y );
			if (US_OFFLINE == pUserData->cbUserStatus)
			{//断线标识
				DrawBreakLineTag(pDC, m_UserDuanXian[i].x,m_UserDuanXian[i].y);

			}
			//游戏中对家剩余扑克
			if (i!=1)
			{
				m_bLeftUser = ((0==i) ? true : false);      
				//显示对家手牌的背面
			    if(m_bCardCount[i]==1)
				{
					if (m_bLeftUser)
					{
						m_XieCardOne.DrawImage(pDC,120,236,140,150,0,0);
					}
					else
					{
						m_XieCardOne.DrawImage(pDC,695,236,140,150,140,0);
					}
				}
				else if (m_bCardCount[i]==2)
				{
					if (m_bLeftUser)
					{
						m_XieCardTwo.DrawImage(pDC,120,236,140,150,0,0);
					}
					else
					{
						m_XieCardTwo.DrawImage(pDC,695,236,140,150,140,0);
					}
				}
				else if (m_bCardCount[i]==3 )
				{
					if (m_bLeftUser)
					{
						m_XieCardThree.DrawImage(pDC,120,236,140,150,0,0);
					}
					else
					{
						m_XieCardThree.DrawImage(pDC,695,236,140,150,140,0);
					}
				}
				else  if(m_bCardCount[i]>0)
				{
					if (m_bLeftUser)
					{
						m_XieCard.DrawImage(pDC,130,221,140,150,0,0);
					}
					else
					{
						m_XieCard.DrawImage(pDC,685,221,140,150,140,0);
					}
				}
				DrawOtherUserHandNums(pDC, nWidth, nHeight, m_bLeftUser, m_bCardCount[i]);
			}

		
			DrawUserTimer(pDC,i);
			//绘画准备
			if (pUserData->cbUserStatus==US_READY)
			{
				DrawUserReady(pDC,m_ptReady[i].x,m_ptReady[i].y);
			}
			//绘画形象
			DrawCustomFace(pDC,i, pUserData,m_ptFace[i].x,m_ptFace[i].y);	
			
			pDC->SetTextAlign( TA_LEFT );
			//宝石
			/*pDC->SetTextColor(RGB(0,0,0));
			memset(szBuffer, 0, sizeof(szBuffer));
			sprintf(szBuffer, "%d", pUserData->lGold);*/
			//pDC->TextOut(m_ptUserBaoShi[i].x, m_ptUserBaoShi[i].y, TEXT(szBuffer));
			
			//等级
			/*memset(szBuffer, 0, sizeof(szBuffer));
			sprintf(szBuffer, "%d", pUserData->dwClass);
			pDC->TextOut(m_ptUserGrade[i].x, m_ptUserGrade[i].y, TEXT(szBuffer));
			memset(szBuffer, 0, sizeof(szBuffer));*/

			//等级框*等级
			if (i!=1)
			{
				INT XPoint=m_GrandFrame[i].x;
				INT yPoint=m_GrandFrame[i].y;
				m_ImageGrandFrame.DrawImage(pDC,XPoint,yPoint,m_ImageGrandFrame.GetWidth(),m_ImageGrandFrame.GetHeight(),0,0);
				INT GrandWith=m_ImageGrand.GetWidth()/12;
				INT GrandHeighe=m_ImageGrand.GetHeight();
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
					INT xPos=m_ptUserGrade[i].x;
					INT yPos=m_ptUserGrade[i].y;
					while (0<GrandCount)
					{
						xPos+=GrandWith;
						--GrandCount;
						INT n_Mutiple=(INT)pow(10,GrandCount);
						INT DrawGrand=(INT)(Fan/n_Mutiple);
						m_ImageGrand.DrawImage(pDC,xPos,yPos,GrandWith,GrandHeighe,DrawGrand*GrandWith,0);
						Fan-=(n_Mutiple*DrawGrand);
					}
				}
				
			}
			//等级称谓
			/*TCHAR tcGradeName[32]={0};
			::memcpy(tcGradeName,pUserData->chGradeName,sizeof(tcGradeName));*/
			if (i!=1)
			{
				DrawTextString( pDC,pUserData->chGradeName, RGB( 0, 255, 0 ), RGB( 0, 0, 0 ), m_Rating[i].x,m_Rating[i].y );
			}
			//pDC->TextOut(m_Rating[i].x,m_Rating[i].y,pUserData->chGradeName,strlen(pUserData->chGradeName));

			//lq
			//亮牌信息
		/*	if ( m_bBrightStart[i] )
			{
				BYTE const SECTION_COUNT = 11; 
				INT nInfoWidth = m_ImageScore.GetWidth() / SECTION_COUNT;
				INT nInfoHeight = m_ImageScore.GetHeight();
				INT nXPos = nInfoWidth * 8 ;
				m_ImageScore.DrawImage(pDC, m_ptBrightStart[i].x, m_ptBrightStart[i].y, nInfoWidth, nInfoHeight, nXPos, 0 );
			}*/
			//lq
			if ( m_bUserTrustee[ i ] ) 
			{//托管标识
				m_ImageUserTrustee.DrawImage( pDC, m_ptUserTrustee[ i ].x, m_ptUserTrustee[ i ].y);
			}

			pDC->SetTextAlign(iAlignType);
		}
	}
    //绘画标识(叫地主,抢地主,pass)
	int nXImagePos,nImageWidth=m_ImageScore.GetWidth(),nImageHeight=m_ImageScore.GetHeight();
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		if (m_bScore[i]!=0)
		{
			BYTE const SECTION_COUNT = 4;

			if (1==m_bScore[i]) nXImagePos=3*(nImageWidth/SECTION_COUNT);
			else if (2==m_bScore[i]) nXImagePos=2*(nImageWidth/SECTION_COUNT);
			else if (3==m_bScore[i]) nXImagePos=nImageWidth/SECTION_COUNT;
			else if (4==m_bScore[i]) nXImagePos=0;

			m_ImageScore.DrawImage(pDC,m_ptScore[i].x,m_ptScore[i].y,nImageWidth/SECTION_COUNT,nImageHeight,nXImagePos,0);
		}
		if (m_bPass[i]==true)
		{
			m_ImagePass.DrawImage(pDC,m_OutCardPass[i].x,m_OutCardPass[i].y,m_ImagePass.GetWidth(),m_ImagePass.GetHeight(),0,0);
		}
	}
	//庄家位置
	//m_wLandUser = 1;
	if (m_wLandUser!=INVALID_CHAIR && m_cbCallScorePhase == CSD_GAMESTART)
	{
		//获取用户
		const tagUserData * pUserData=GetUserInfo(m_wLandUser);
		if (pUserData!=NULL)
		{
			//变量定义 
			TCHAR szBuffer[64];
			INT nXStartPos = m_nXBorder + 62;
			INT nYStartPos = m_nYBorder+10;
			INT nSpace = 20;

			//设置DC
			UINT iAlignType = pDC->SetTextAlign( DT_LEFT );

			//绘画信息
			for(BYTE i=0;i<GAME_PLAYER;i++)
			{//地主农民标识
				if(m_wLandUser == i)
				{
					m_ImageLand.DrawImage(pDC,m_ptLand[m_wLandUser].x,m_ptLand[m_wLandUser].y,m_ImageLand.GetWidth()/2,m_ImageLand.GetHeight(),0,0);
				}
				else
				{
					m_ImageLand.DrawImage(pDC,m_ptLand[i].x,m_ptLand[i].y,m_ImageLand.GetWidth()/2,m_ImageLand.GetHeight(),m_ImageLand.GetWidth()/2,0);
				}
			}
			//_snprintf(szBuffer,sizeof(szBuffer),TEXT("地主：%s"),pUserData->szName);
			//DrawTextString( pDC, szBuffer, RGB( 255, 255, 255 ), RGB( 0, 0, 0 ), nXStartPos,nYStartPos );

			//绘画头像
			//DrawUserFace(pDC,pUserData,m_nXBorder+5,m_nYBorder+5,false);

			//叫牌分数
			//LONGLONG lScore=m_cbLandScore*m_lBaseScore;
			//_snprintf(szBuffer,sizeof(szBuffer),TEXT("底分：%I64d "),lScore);
			//_snprintf(szBuffer,sizeof(szBuffer),TEXT("%I64d "),lScore);
			//_snprintf(szBuffer,sizeof(szBuffer),TEXT("%d "),m_cbLandScore);//cxf 设置底分（1，2，3）
			//DrawTextString( pDC, szBuffer, RGB( 255, 255, 255 ), RGB( 0, 0, 0 ), nXStartPos+505,nYStartPos + nSpace+47  );

			//翻倍计算
			//WORD wDrawTimes=(WORD)m_lBaseScore;
			//for (WORD i=1;i<m_cbLandScore;i++) wDrawTimes*=2;

			////总共倍数
			//WORD wLandTime = 1;
			//if ( m_cbBrightTime != 0 && wDrawTimes == 0 ) wLandTime = m_cbBrightTime;
			//else if ( m_cbBrightTime == 0 && wDrawTimes != 0 ) wLandTime = wDrawTimes;
			//else if ( m_cbBrightTime != 0 && wDrawTimes != 0 ) wLandTime = m_cbBrightTime * wDrawTimes;
			//_snprintf(szBuffer,sizeof(szBuffer),TEXT("倍数：%d"),m_bDouble );
			//_snprintf(szBuffer,sizeof(szBuffer),TEXT("%d"),m_bDouble );
			//_snprintf(szBuffer,sizeof(szBuffer),TEXT("%d"),m_lBaseScore );// cxf 设置基础分数 
			//DrawTextString( pDC, szBuffer, RGB( 255, 255, 255 ), RGB( 0, 0, 0 ), nXStartPos+505,nYStartPos + nSpace+17 );

			//炸弹倍数
			//_snprintf(szBuffer,sizeof(szBuffer),TEXT("炸弹翻倍：%d"),m_wBombTime);
			//_snprintf(szBuffer,sizeof(szBuffer),TEXT("%d"),m_wBombTime);
			//DrawTextString( pDC, szBuffer, RGB( 255, 255, 255 ), RGB( 0, 0, 0 ), nXStartPos+505,nYStartPos + nSpace+77);
			pDC->SetTextAlign(iAlignType);
		}
	}
	

	//描述信息
	if (m_bLandTitle==true)
	{
		////创建资源
		//CFont InfoFont;
		//InfoFont.CreateFont(-36,0,0,0,400,0,0,0,134,3,2,1,2,TEXT("黑体"));

		////设置 DC
		//pDC->SetTextColor(RGB(121,56,13));
		//pDC->SetTextAlign(TA_CENTER|TA_TOP);
		//CFont * pOldFont=pDC->SelectObject(&InfoFont);

		////绘画信息
		//pDC->TextOut(nWidth/2,230,"正等待用户叫分");

		////清理资源
		//pDC->SelectObject(pOldFont);
		//InfoFont.DeleteObject();

		//加载资源
		//CImageHandle m_HandleWaitScore(&m_ImageWaitScore);

		//正等待用户叫分
-		m_ImageWaitScore.DrawImage(pDC,nWidth/2-125,210,m_ImageWaitScore.GetWidth(),m_ImageWaitScore.GetHeight(),0,0);
	}

	//绘制底牌
	DWORD dwXImagePos,dwYImagePos;
	CSize m_CardSize;
	m_CardSize.cx=m_PngCardBack.GetWidth()/13;
	m_CardSize.cy=m_PngCardBack.GetHeight()/5;
	for (DWORD i=0;i<3;i++)
	{
		if(m_bDisplay ==true)
		{
			//图片位置
			if (m_bCardBackDate[i] !=0)
			{
				if ((m_bCardBackDate[i]==0x4E)||(m_bCardBackDate[i]==0x4F))
				{
					dwXImagePos=((m_bCardBackDate[i]&CARD_MASK_VALUE)%14)*m_CardSize.cx;
					dwYImagePos=((m_bCardBackDate[i]&CARD_MASK_COLOR)>>4)*m_CardSize.cy;
				}
				else
				{
					dwXImagePos=((m_bCardBackDate[i]&CARD_MASK_VALUE)-1)*m_CardSize.cx;
					dwYImagePos=((m_bCardBackDate[i]&CARD_MASK_COLOR)>>4)*m_CardSize.cy;
				}
			}
		}
		else 
		{
			dwXImagePos=m_CardSize.cx*2;
			dwYImagePos=m_CardSize.cy*4;
		}
		//绘画底牌(三张)
		if (m_DiPaiCount>0)
		{
			if(i == 0)
				m_PngCardBack.DrawImage(pDC,388,20,m_PngCardBack.GetWidth()/13,m_PngCardBack.GetHeight()/5,dwXImagePos,dwYImagePos);
			else if(i == 1)
				m_PngCardBack.DrawImage(pDC,388+m_CardSize.cx+4,20,m_PngCardBack.GetWidth()/13,m_PngCardBack.GetHeight()/5,dwXImagePos,dwYImagePos);
			else
				m_PngCardBack.DrawImage(pDC,388+2*(m_CardSize.cx+4),20,m_PngCardBack.GetWidth()/13,m_PngCardBack.GetHeight()/5,dwXImagePos,dwYImagePos);
		}
		
	}

	////绘制玩家剩余多少张牌
	//for (int i=0; i<GAME_PLAYER; i++)
	//{
	//	//获取用户
	//	const tagUserData * pUserData=GetUserInfo(i);
	//	if (pUserData!=NULL)
	//	{
	//		if (m_UserTSCardNums[i].wNums>0 && m_UserTSCardNums[i].wNums<=3)
	//		{
	//			if (!m_ImageSYPBack.IsNull())
	//			{//绘制背景
	//				Size backsize;
	//				backsize.Width = m_ImageSYPBack.GetWidth()/3;
	//				backsize.Height= m_ImageSYPBack.GetWidth();
	//				int iSrcPoint = 0;
	//				if (1==i) iSrcPoint = backsize.Width*2;
	//				else if (2==i) iSrcPoint = backsize.Width;
	//				m_ImageSYPBack.DrawImage(pDC, m_ptUserSYPTS[i].x, m_ptUserSYPTS[i].y, backsize.Width, backsize.Height, iSrcPoint, 0, backsize.Width, backsize.Height);
	//				
	//				if (!m_ImageSYPNums.IsNull())
	//				{//绘制几张
	//					backsize.Width = m_ImageSYPNums.GetWidth()/3;
	//					backsize.Height= m_ImageSYPNums.GetWidth();
	//					iSrcPoint = (m_UserTSCardNums[i].wNums-1)*backsize.Width;
	//					POINT point;
	//					if (1 == i)
	//					{
	//						point.x = 28;
	//						point.y = 20;
	//					}
	//					else
	//					{
	//						point.x = 28;
	//						point.y = 32;
	//					}
	//					m_ImageSYPNums.DrawImage(pDC, m_ptUserSYPTS[i].x+point.x, m_ptUserSYPTS[i].y+point.y, backsize.Width, backsize.Height, iSrcPoint, 0, backsize.Width, backsize.Height);
	//				}
	//			}
	//		}
	//	}
	//}

	//绘画倍数
	DrawLandTime(pDC, nWidth, nHeight);

	//绘制游戏牌
	m_GameCard.DrawGameCard(pDC);

	//画飞机动画
	if (m_bTXEffect==true)
	{
		CWnd* pMain = AfxGetMainWnd();
		if (NULL != pMain)
		{
			CRect rcmain, rcflash;
			pMain->GetWindowRect(&rcmain);

			rcflash.left = 1000;
			rcflash.top = 300;	

			rcflash.left += rcmain.left;
			rcflash.top += rcmain.top;

			m_BActionWnd.SetWindowPos(NULL, rcflash.left-m_bTXFrameIndex*150, rcflash.top, 0, 0, SWP_NOSIZE);
			m_BActionWnd.ShowWindow(SW_SHOW);
		}
		if(m_bTXFrameIndex==9)
		{
			m_BActionWnd.ShowWindow(SW_HIDE);
		}
	}
	//画火箭动画
	if(m_bHuoJian==true)
	{
		CWnd* pMian=AfxGetMainWnd();
		if (NULL!=pMian)
		{
			CRect rcmain,rcflash;
			pMian->GetWindowRect(&rcmain);
			rcflash.left=400;
			rcflash.top=700;
			rcflash.left+=rcmain.left;
			rcflash.top+=rcmain.top;
			m_HuoJianAction.SetWindowPos(NULL,rcflash.left,rcflash.top-m_bHuojianIndex*100,0,0,SWP_NOSIZE);
			m_HuoJianAction.ShowWindow(SW_SHOW);
		}
		if (m_bHuojianIndex==9)
		{
			m_HuoJianAction.ShowWindow(SW_HIDE);
		}
	}
	if (m_bIsShowbigness)
	{//显示大不起牌图标
		if (!m_Imagebigness.IsNull())
		{
			m_Imagebigness.DrawImage(pDC, 355, 370, m_Imagebigness.GetWidth(), m_Imagebigness.GetHeight(),NULL,NULL);
		}
	}

	__LEAVE_FUNCTION 
}

//炸弹倍数
void CGameClientView::SetBombTime(WORD wBombTime)
{
	__ENTER_FUNCTION 

	//设置变量
	m_wBombTime=wBombTime;

	//刷新界面
	UpdateGameView(NULL);

   __LEAVE_FUNCTION 
}

//基础分数
void CGameClientView::SetBaseScore(LONGLONG lBaseScore)
{
	__ENTER_FUNCTION 

	//设置变量
	m_lBaseScore=lBaseScore;

	//刷新界面
	UpdateGameView(NULL);

	__LEAVE_FUNCTION 
}

//显示提示
void CGameClientView::ShowLandTitle(bool bLandTitle)
{
	__ENTER_FUNCTION 

	//设置变量
	m_bLandTitle=bLandTitle;

	//刷新界面
	UpdateGameView(NULL);

	__LEAVE_FUNCTION 
}

//设置放弃
void CGameClientView::SetPassFlag(WORD wChairID, bool bPass)
{
	__ENTER_FUNCTION 

	//设置变量
	if (wChairID==INVALID_CHAIR)
	{
		for (WORD i=0;i<GAME_PLAYER;i++) m_bPass[i]=bPass;
	}
	else m_bPass[wChairID]=bPass;

	//更新界面
	UpdateGameView(NULL);

	__LEAVE_FUNCTION 
}

//扑克数目
void CGameClientView::SetCardCount(WORD wChairID, BYTE bCardCount)
{
	__ENTER_FUNCTION 

	//设置变量
	if (wChairID==INVALID_CHAIR)
	{
		for (WORD i=0;i<GAME_PLAYER;i++) 

		{
			m_bCardCount[i]=bCardCount;
		}
	}
	else m_bCardCount[wChairID]=bCardCount;

	//更新界面
	UpdateGameView(NULL);

	__LEAVE_FUNCTION 
}

//挖坑分数(设置地主) 
void CGameClientView::SetLandUser(WORD wChairID, BYTE bLandScore)
{
	__ENTER_FUNCTION 

	//设置变量
	m_wLandUser=wChairID;
	m_cbLandScore=bLandScore;

	//更新界面
	UpdateGameView(NULL);
	__LEAVE_FUNCTION 
}

//挖坑分数
void CGameClientView::SetLandScore(WORD wChairID, BYTE bLandScore, BYTE cbCallScorePhase, BYTE cbPreCallScorePhase)
{
	__ENTER_FUNCTION 

	//设置变量
	if (wChairID!=INVALID_CHAIR)
	{
		m_bScore[wChairID]=bLandScore;
		m_cbCallScorePhase = cbCallScorePhase;
		m_cbUserPhase[wChairID] = cbPreCallScorePhase;
	}
	else 
	{
		memset(m_bScore,0,sizeof(m_bScore));
		m_cbCallScorePhase = cbCallScorePhase;
		ZeroMemory( m_cbUserPhase, sizeof( m_cbUserPhase ) );
	}	

	//更新界面
	UpdateGameView(NULL);
	__LEAVE_FUNCTION 
}

//开始按钮
void CGameClientView::OnStart()
{
	__ENTER_FUNCTION 

//#ifdef VIDEO_GAME
//	//隐藏扑克
//	m_HandCardControl[ 0 ].ShowWindow( SW_HIDE );
//	m_HandCardControl[ 2 ].ShowWindow( SW_HIDE );
//#endif

	AfxGetMainWnd()->SendMessage(IDM_START,0,0);
	__LEAVE_FUNCTION 
}

//出牌按钮
void CGameClientView::OnOutCard()
{
	__ENTER_FUNCTION 

	AfxGetMainWnd()->SendMessage(IDM_OUT_CARD,1,1);
	__LEAVE_FUNCTION 
}

//放弃按钮
void CGameClientView::OnPassCard()
{
	__ENTER_FUNCTION 

	AfxGetMainWnd()->SendMessage(IDM_PASS_CARD,1,1);
	__LEAVE_FUNCTION 
}

//叫地主按钮
void CGameClientView::OnCallLand()
{
	__ENTER_FUNCTION 

	AfxGetMainWnd()->SendMessage(IDM_CALL_LAND,1,1);
	__LEAVE_FUNCTION 
}

//不叫按钮
void CGameClientView::OnGiveUpCallLand()
{
	__ENTER_FUNCTION 

	AfxGetMainWnd()->SendMessage(IDM_CALL_LAND,0,0);
	__LEAVE_FUNCTION 
}

//抢地主按钮
void CGameClientView::OnSnatchLand()
{
	__ENTER_FUNCTION 

	AfxGetMainWnd()->SendMessage(IDM_SNATCH_LAND,1,1);
	__LEAVE_FUNCTION 
}

//不抢按钮
void CGameClientView::OnGiveUpSnatchLand()
{
	__ENTER_FUNCTION 
		
	AfxGetMainWnd()->SendMessage(IDM_SNATCH_LAND,0,0);
	__LEAVE_FUNCTION 
}
//出牌提示
void CGameClientView::OnAutoOutCard()
{
	__ENTER_FUNCTION 

	AfxGetMainWnd()->SendMessage(IDM_AUTO_OUTCARD,0,0);
	__LEAVE_FUNCTION 
}
//////////////////////////////////////////////////////////////////////////

// 自动托管
void CGameClientView::OnAutoPlayerOn()
{
	__ENTER_FUNCTION 
	
    m_btAutoPlayOn.ShowWindow(SW_HIDE);
    m_btAutoPlayOff.ShowWindow(SW_SHOW);
	AfxGetMainWnd()->SendMessage(IDM_LAND_AUTOPLAY,1,1);
	__LEAVE_FUNCTION 
}

// 取消托管
void CGameClientView::OnAutoPlayerOff()
{
	__ENTER_FUNCTION 
	
    m_btAutoPlayOn.ShowWindow(SW_SHOW);
    m_btAutoPlayOff.ShowWindow(SW_HIDE);
	AfxGetMainWnd()->SendMessage(IDM_LAND_AUTOPLAY,0,0);
	__LEAVE_FUNCTION 
}

//设置顺序
void CGameClientView::SetUserOrder(bool bDeasilOrder) 
{
	__ENTER_FUNCTION 
	 
	//状态判断
	if (m_bDeasilOrder==bDeasilOrder) return;

	//设置变量
	m_bDeasilOrder=bDeasilOrder;
	AfxGetApp()->WriteProfileInt(TEXT("GameOption"),TEXT("DeasilOrder"),m_bDeasilOrder?TRUE:FALSE);

	//设置界面
	CRect rcClient;
	GetClientRect(&rcClient);

	RectifyGameView(rcClient.Width(),rcClient.Height());

	//刷新界面
	UpdateGameView(NULL);

	__LEAVE_FUNCTION 
}
//设置飞机特效
bool CGameClientView::SetFeiJi(bool bTXEffect)
{
	__ENTER_FUNCTION 

	if (bTXEffect==true)
	{
		//设置变量
		m_bTXEffect=true;
		m_bTXFrameIndex=0;

		//启动时间
		SetTimer(IDI_ACTION_EFFECT,100,NULL);
	}
	else
	{
		//停止动画
		if (m_bTXEffect==true)
		{
			//删除时间
			KillTimer(IDI_ACTION_EFFECT);

			//设置变量
			m_bTXEffect=false;
			m_bTXFrameIndex=0;

			//更新界面
			UpdateGameView(NULL);
		}
	}
__LEAVE_FUNCTION
	return true;
	 
}

//设置火箭特效
bool CGameClientView::SetHuoJian(bool bTXEffect)
{
	__ENTER_FUNCTION 

	if (bTXEffect==true)
	{
		//设置变量
		m_bHuoJian=true;
		m_bHuojianIndex=0;

		//启动时间
		SetTimer(IDI_HUOJIAN_EFFECT,100,NULL);
	}
	else
	{
		//停止动画
		if (m_bHuoJian==true)
		{
			//删除时间
			KillTimer(IDI_HUOJIAN_EFFECT);

			//设置变量
			m_bHuoJian=false;
			m_bHuojianIndex=0;

			//更新界面
			UpdateGameView(NULL);
		}
	}
__LEAVE_FUNCTION
	return true;	 
}

//时间消息
VOID CGameClientView::OnTimer(UINT nIDEvent)
{
	__ENTER_FUNCTION 

	//特效火箭动画
	if (nIDEvent==IDI_HUOJIAN_EFFECT)
	{
		//停止判断
		if (m_bHuoJian==false)
		{
			KillTimer(IDI_HUOJIAN_EFFECT);
			return;
		}

		//设置变量
		if ((m_bHuojianIndex+1)>=10)
		{
			//删除时间
			KillTimer(IDI_HUOJIAN_EFFECT);

			//设置变量
			m_bHuoJian=false;
			m_bHuojianIndex=0;
		}
		else 
		{
			m_bHuojianIndex++;
		}

		//更新界面
		UpdateGameView(NULL);

		return;
	}
	//特效飞机动画
	if (nIDEvent==IDI_ACTION_EFFECT)
	{
		//停止判断
		if (m_bTXEffect==false)
		{
			KillTimer(IDI_ACTION_EFFECT);
			return;
		}

		//设置变量
		if ((m_bTXFrameIndex+1)>=10)
		{
			//删除时间
			KillTimer(IDI_ACTION_EFFECT);

			//设置变量
			m_bTXEffect=false;
			m_bTXFrameIndex=0;
		}
		else 
		{
			m_bTXFrameIndex++;
		}
		//更新界面
		UpdateGameView(NULL);

		return;
	}
   __super::OnTimer(nIDEvent);

	__LEAVE_FUNCTION 
}


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
}

//玩家托管
void CGameClientView::SetUserTrustee( WORD wChairID, bool bTrustee )
{
	__ENTER_FUNCTION 
 
	//设置变量
	if ( INVALID_CHAIR == wChairID )
	{
		for ( WORD wChairIdx = 0; wChairIdx < GAME_PLAYER; wChairIdx++ ) 
		{
			m_bUserTrustee[ wChairIdx ] = bTrustee;
		}
	}
	else {
		m_bUserTrustee[ wChairID ] = bTrustee;
	}

	//更新界面
	UpdateGameView( NULL );

	__LEAVE_FUNCTION 
}

//排序按钮
VOID CGameClientView::OnBnClickedSortCard()
{
	__ENTER_FUNCTION 

	//发送消息
	AfxGetMainWnd()->SendMessage(IDM_SORT_HAND_CARD,0,0);

	__LEAVE_FUNCTION 
}

//叫分按钮
//void CGameClientView::OnSnatchLand()
//{
//	__ENTER_FUNCTION 
//
//	AfxGetMainWnd()->SendMessage(IDM_LAND_SCORE,4,4);
//	return;
//
//	__LEAVE_FUNCTION 
//}

//明牌按钮
void CGameClientView::OnBrightCard()
{
	__ENTER_FUNCTION 

	/*AfxGetMainWnd()->SendMessage(IDM_LAND_SCORE,5,5);*/

	__LEAVE_FUNCTION 
}

//显示报牌
void CGameClientView::SetShowWarming(WORD wChairID, bool bShowWarming)
{
	__ENTER_FUNCTION 

	if(INVALID_CHAIR==wChairID)
	{
		for(BYTE i=0; i<GAME_PLAYER; ++i) m_bShowWarming[i] = false ;
	}
	else m_bShowWarming[wChairID] = bShowWarming ;

	UpdateGameView(NULL) ;

	__LEAVE_FUNCTION 
}

//明牌开始
void CGameClientView::OnBrightStart()
{
	__ENTER_FUNCTION 

	AfxGetMainWnd()->SendMessage(IDM_BRIGHT_START,0,0);

	__LEAVE_FUNCTION 
}

//明牌开始
void CGameClientView::SetBrightStart(WORD wChairID, bool bBrightStart) 
{
	__ENTER_FUNCTION 

	//设置变量
	if (wChairID==INVALID_CHAIR)
	{
		for (WORD i=0;i<GAME_PLAYER;i++) m_bBrightStart[i]=bBrightStart;
	}
	else m_bBrightStart[wChairID]=bBrightStart;

	//更新界面
	UpdateGameView(NULL);

	__LEAVE_FUNCTION 
}


//显示倍数(即时)
void CGameClientView::DrawLandTime(CDC *pDC, INT nViewWidth, INT nViewHeight)
{
	__ENTER_FUNCTION 

	//图片信息
	INT nDigitWidth = m_ImageLandTimes.GetWidth() / 10;
	INT nDigitHeight = m_ImageLandTimes.GetHeight();
	//设置变量
	//bool bBrightStart = false;
	//for ( WORD wChairID = 0; wChairID < GAME_PLAYER; ++wChairID )
	//{
	//	//存在判断
	//	tagUserData const *pUserData = GetUserInfo( wChairID );
	//	if ( pUserData == NULL )  m_bBrightStart[wChairID] = false;
	//}
	//for ( WORD wChairID = 0; wChairID < GAME_PLAYER; ++wChairID )
	//	if ( m_bBrightStart[wChairID] ) bBrightStart = true;

	//重置变量
	//WORD wCardCount = m_bCardCount[0] + m_bCardCount[1] + m_bCardCount[2];
	//if ( ! bBrightStart && wCardCount == 0 ) m_cbBrightTime = 0;

	//翻倍计算
	//WORD wDrawTimes=(WORD)m_lBaseScore;
	//for (WORD i=1;i<m_cbLandScore;i++) wDrawTimes*=2;

	//翻倍
	int wLandTime = m_iMeBeiShu;
	//for(WORD i=0;i<GAME_PLAYER;i++)
	//{
	//	if(m_bDoubleScore[i])
	//		 wLandTime*=2;
	//}

	//总共倍数

	//if ( m_cbBrightTime != 0 && wDrawTimes == 0 ) wLandTime = m_cbBrightTime;
	//else if ( m_cbBrightTime == 0 && wDrawTimes != 0 ) wLandTime = wDrawTimes;
	//else if ( m_cbBrightTime != 0 && wDrawTimes != 0 ) wLandTime = m_cbBrightTime * wDrawTimes;

	//大小过滤
	if ( wLandTime <= 0 ) return ;

	//数字个数
	int nDigitCount = 0;
	int nTmpTime = wLandTime;
	while( 1 <= nTmpTime )
	{
		nDigitCount++;
		nTmpTime /= 10;
	}

	if ( 1 <= nDigitCount )
	{
		//总共长度
		INT nTotalLen = ( nDigitCount +1 ) * nDigitWidth;

		//位置信息
		INT nXPos = 0; 
		INT nYPos = 0;

		nXPos = m_ptLand[1].x-15;
		nYPos = m_ptLand[1].y+100;

		m_ImageLandbei.DrawImage(pDC, nXPos, nYPos, m_ImageLandbei.GetWidth(), m_ImageLandbei.GetHeight(), 0, 0);

		//开始绘画
		nXPos+=40;
		while( 0 < nDigitCount )
		{
			//计算位置
			nXPos += nDigitWidth;

			//计算倍数
			--nDigitCount;
			INT nMutiple = (INT)pow( 10, nDigitCount );

			//绘画数字
			INT nDrawDigit = (INT)(wLandTime / nMutiple) ;
			m_ImageLandTimes.DrawImage(pDC, nXPos, nYPos+2, nDigitWidth, nDigitHeight, nDrawDigit * nDigitWidth, 0);

			//计算变量
			wLandTime -= ( nMutiple * nDrawDigit );
		}
	}

	__LEAVE_FUNCTION 
}

//显示对家手牌个数(即时)
void CGameClientView::DrawOtherUserHandNums(CDC *pDC, INT nViewWidth, INT nViewHeight, BOOL bLeftUser, INT nCardNums)
{
	__ENTER_FUNCTION 

	//图片信息
	INT nDigitWidth = m_ImageCardNum.GetWidth() / 10;
	INT nDigitHeight = m_ImageCardNum.GetHeight();

	//大小过滤
	if ( nCardNums <= 0 ) return ;

	//数字个数
	INT nDigitCount = 0;
	INT nTmpTime = nCardNums;
	while( 1 <= nTmpTime )
	{
		nDigitCount++;
		nTmpTime /= 10;
	}

	if ( 1 <= nDigitCount )
	{
		//总共长度
		INT nTotalLen = ( nDigitCount +1 ) * nDigitWidth;

		//位置信息
		INT nXPos = 0; 
		INT nYPos = 0;

		nXPos = (true==bLeftUser)? 130 : 758;
		nYPos = 343;

		//绘画背景
		INT nTCardWidth = m_ImageTCard.GetWidth();
		INT nTCardHeight = m_ImageTCard.GetHeight();
		m_ImageTCard.DrawImage(pDC, nXPos, nYPos, nTCardWidth, nTCardHeight, 0, 0);


		//开始绘画
		while( 0 < nDigitCount )
		{
			//计算位置
			nXPos += nDigitWidth;

			//计算倍数
			--nDigitCount;
			INT nMutiple = (INT)pow( 10, nDigitCount );

			//绘画数字
			INT nDrawDigit = (INT)(nCardNums / nMutiple) ;
			if (nCardNums>=10)
			{
				m_ImageCardNum.DrawImage(pDC, nXPos+5, nYPos+5, nDigitWidth, nDigitHeight, nDrawDigit * nDigitWidth, 0);
			}
			else
                m_ImageCardNum.DrawImage(pDC, nXPos+10, nYPos+5, nDigitWidth, nDigitHeight, nDrawDigit * nDigitWidth, 0);

			//计算变量
			nCardNums -= ( nMutiple * nDrawDigit );
		}
	}

	__LEAVE_FUNCTION 
}


//明牌倍数
void CGameClientView::SetBrightTime(BYTE cbBrightTime)
{

	//相等判断
	if ( m_cbBrightTime == cbBrightTime ) return ;

	//变量赋值
	m_cbBrightTime = cbBrightTime;

	//更新界面
	UpdateGameView(NULL);

	return;
}

//cxf (倒-跟-反)替代加倍
/*
//加倍消息
void CGameClientView::OnDoubleScore()
{
	//发送消息
	AfxGetMainWnd()->SendMessage(IDM_DOUBLE_SCORE, 0, 0);
}
//加倍消息
void CGameClientView::OnNotDoubleScore()
{
	//发送消息
	AfxGetMainWnd()->SendMessage(IDM_DOUBLE_SCORE, 1, 1);
}
*/
//倒消息
//void CGameClientView::OnDoubleScoreDao()
//{
//	__ENTER_FUNCTION 
//
//	//发送消息
//	AfxGetMainWnd()->SendMessage(IDM_DOUBLE_SCORE, 1, 1);
//
//	__LEAVE_FUNCTION 
//}
////跟消息
//void CGameClientView::OnDoubleScoreGen()
//{
//	__ENTER_FUNCTION 
//
//	//发送消息
//	AfxGetMainWnd()->SendMessage(IDM_DOUBLE_SCORE, 2, 2);
//
//	__LEAVE_FUNCTION 
//}
////反消息
//void CGameClientView::OnDoubleScoreFan()
//{
//	__ENTER_FUNCTION 
//
//	//发送消息
//	AfxGetMainWnd()->SendMessage(IDM_DOUBLE_SCORE, 3, 3);
//
//	__LEAVE_FUNCTION 
//}
//cxf end 
//上一轮
void CGameClientView::OnLastRound()
{
	//AfxGetMainWnd()->SendMessage()
}

//显示倍数
void CGameClientView::ShowLandTime(bool bShowLandTime)
{
	__ENTER_FUNCTION 

	//等同判断
	if ( m_bShowLandTime == bShowLandTime ) return ;

	//设置变量
	m_bShowLandTime = bShowLandTime;

	//更新界面
	UpdateGameView( NULL );

	__LEAVE_FUNCTION 
}

//玩家加倍
void CGameClientView::SetDoubleScoreUser( WORD wChairID, bool bDoubleScore )
{
	__ENTER_FUNCTION 

	//char cha[128]={0};
	//::sprintf(cha,"double=%d",bDoubleScore);
	//((CGameClientDlg*)AfxGetMainWnd())->InsertSystemString(cha);

	//设置变量
	if (wChairID==INVALID_CHAIR)
	{
		for (WORD i=0;i<GAME_PLAYER;i++) 
		{
			if(m_bDoubleScore[i])
			m_bDoubleScore[i]=bDoubleScore;

		}
	}
	else 
	{
		if (bDoubleScore)
		{
			m_bScore[wChairID] = 0;
		}
		m_bDoubleScore[wChairID]=bDoubleScore;		
	}
	//更新界面
	UpdateGameView(NULL);

	__LEAVE_FUNCTION 

}

//功能：提示少于3张牌的功能.
void CGameClientView::SetUserShouPaiNums(WORD wViewID, WORD wNums)
{
	__ENTER_FUNCTION 

	if (wViewID>=GAME_PLAYER || wNums>3)
	{
		return;
	}

	for (int i=0; i<GAME_PLAYER; i++)
	{
		if (i == wViewID)
		{
			if (true == m_UserTSCardNums[wViewID].bIsTS)
			{
				m_UserTSCardNums[wViewID].wNums = wNums;
				m_UserTSCardNums[wViewID].bIsTS = false;
				continue;
			}
		}
		m_UserTSCardNums[i].wNums = 0;
	}
	
	//更新界面
	UpdateGameView(NULL);

	__LEAVE_FUNCTION 
}

//重置牌张数
void CGameClientView::SetResetShouPaiNums()
{
	__ENTER_FUNCTION 
	
	//清空剩余记录
	memset(&m_UserTSCardNums, 0, sizeof(m_UserTSCardNums));
	for (int i=0; i<GAME_PLAYER; i++)
	{
		m_UserTSCardNums[i].bIsTS = true;
	}

	__LEAVE_FUNCTION 
}

void CGameClientView::SetResetSYPaiNums()
{
	__ENTER_FUNCTION 

	for (int i=0; i<GAME_PLAYER; i++)
	{
		m_UserTSCardNums[i].wNums = 0;
	}
	
	__LEAVE_FUNCTION 
}

//设置显示大不起牌图片
void CGameClientView::SetShowBigness(bool bIsShow)
{
	__ENTER_FUNCTION 

	m_bIsShowbigness = bIsShow;

	__LEAVE_FUNCTION 
}
//设置倍数
void CGameClientView::SetMeBeiShu(int iBeiShu)
{
	__ENTER_FUNCTION 

	m_iMeBeiShu = iBeiShu;
	UpdateGameView(NULL);

	__LEAVE_FUNCTION 
}
//设置结束手牌
void CGameClientView::SetHandCardData(const BYTE bCardData[],BYTE bCardCount,WORD wChairID)
{
	__ENTER_FUNCTION 

	if (wChairID < GAME_PLAYER)
	{
		m_bHandCardCount[wChairID]=0;
		ZeroMemory(m_bCardData[wChairID],sizeof(BYTE)*20);
		m_bHandCardCount[wChairID]=bCardCount;
		CopyMemory(m_bCardData[wChairID],bCardData,sizeof(BYTE)*bCardCount);
		//更新界面
		UpdateGameView(NULL);
	}

	__LEAVE_FUNCTION 
}

////扑克数目
//void CGameClientView::SetCardCount(WORD wChairID, BYTE bCardCount)
//{
//	//设置变量
//	if (wChairID==INVALID_CHAIR)
//	{
//		for (WORD i=0;i<GAME_PLAYER;i++) 
//		{
//			m_bCardCount[i]=bCardCount;
//		}
//	}
//	else m_bCardCount[wChairID]=bCardCount;
//
//	//更新界面
//	UpdateGameView(NULL);
//
//	return;
//}
void CGameClientView::PlayDoubelFlash()
{//显示加倍动画
	__ENTER_FUNCTION 

	CWnd* pMian=AfxGetMainWnd();
	if (NULL!=pMian)
	{
		CRect rcmain,rcflash;
		pMian->GetWindowRect(&rcmain);
		rcflash.left=395;
		rcflash.top=200;
		rcflash.left+=rcmain.left;
		rcflash.top+=rcmain.top;
		m_DoubleAction.SetWindowPos(NULL,rcflash.left,rcflash.top,0,0,SWP_NOSIZE);
		m_DoubleAction.ShowWindow(SW_SHOW);
	}
    __LEAVE_FUNCTION 
}
void CGameClientView::SetSpring()
{//显示春天和反春
	__ENTER_FUNCTION 

	CWnd* pMain=AfxGetMainWnd();
	if (NULL!=pMain)
	{
		CRect rcmain,rcflash;
		pMain->GetWindowRect(&rcmain);
		rcflash.left=0;
		rcflash.top=0;
		rcflash.left+=rcmain.left;
		rcflash.top+=rcmain.top;
		m_SpringAction.SetWindowPos(NULL,rcflash.left,rcflash.top,0,0,SWP_NOSIZE);
		m_SpringAction.ShowWindow(SW_SHOW);
	}
   __LEAVE_FUNCTION 
}
//顺子,连对,炸弹动画
void CGameClientView::PlayActionFlash(BYTE byViewID,BYTE TypeAction)
{
	__ENTER_FUNCTION 

	if (byViewID < GAME_PLAYER)
	{
		CWnd* pMain = AfxGetMainWnd();
		if (NULL != pMain)
		{
			CRect rcmain, rcflash;
			pMain->GetWindowRect(&rcmain);
			if (TypeAction==MARK_ACTION_ZHADAN)
			{
				if (0 == byViewID)
				{
					rcflash.left = 140;
					rcflash.top = 60;
				}
				else if (1 == byViewID)
				{
					rcflash.left = rcmain.Width()/2-235;
					rcflash.top = rcmain.Height()-550;
				}
				else
				{
					rcflash.left = 420;
					rcflash.top = 60;
				}
			}
			else if (TypeAction==MARK_ACTION_SHUNLIAN)
			{
				if (0 == byViewID)
				{
					rcflash.left = 190;
					rcflash.top = 225;
				}
				else if (1 == byViewID)
				{
					rcflash.left = rcmain.Width()/2-185;
					rcflash.top = rcmain.Height()-375;
				}
				else
				{
					rcflash.left = 470;
					rcflash.top = 225;
				}
			}
			rcflash.left += rcmain.left;
			rcflash.top += rcmain.top;
			
			m_ActionWnd[byViewID].SetWindowPos(NULL, rcflash.left, rcflash.top, 0, 0, SWP_NOSIZE);
			m_ActionWnd[byViewID].ShowWindow(SW_SHOW);
		}		
	}
	__LEAVE_FUNCTION 
}
//按钮加载资源
//void CGameClientView::Loadres(CGdipButton* pBT, CString strValue)
//{
//	if (NULL!=pBT && !strValue.IsEmpty())
//	{
//		WCHAR* pPW = strValue.AllocSysString();
//		if (NULL != pPW)
//		{
//			pBT->LoadImage(pPW);
//		}
//	}
//}

void CGameClientView::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CGameFrameView::OnShowWindow(bShow, nStatus);
	// TODO: 在此处添加消息处理程序代码
}
//创建动画dailog
void CGameClientView::InitializeAction()
{
	__ENTER_FUNCTION 

	//创建连对,顺子,炸弹动画
	for (WORD i=0;i<GAME_PLAYER;++i)
	{
		m_ActionWnd[i].Create(ActionWnd::IDD,this);
	}
	//创建飞机动画
	m_BActionWnd.Create(ActionWnd::IDD,this);
	//创建火箭动画   
	m_HuoJianAction.Create(ActionWnd::IDD,this);
	//创建加倍动画
	m_DoubleAction.Create(ActionWnd::IDD,this);
	//创建春天动画
	m_SpringAction.Create(ActionWnd::IDD,this);

	__LEAVE_FUNCTION 
}
//鼠标消息(移动)
void CGameClientView::OnMouseMove(UINT nFlags, CPoint point)
{
	__ENTER_FUNCTION 

	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CRect rect;
	m_GameCard.GetCardDrawRect(rect);
	if (rect.PtInRect(point))
	{
		m_GameCard.OnMouseMove(point);
	}
	else
	{
		m_GameCard.OnMouseMove(NULL);
	}
	CGameFrameView::OnMouseMove(nFlags, point);

    __LEAVE_FUNCTION 
}
//鼠标消息(左键按下)
void CGameClientView::OnLButtonDown(UINT nFlags, CPoint point)
{
	__ENTER_FUNCTION 

	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CRect rect;
	m_GameCard.GetCardDrawRect(rect);
	if (rect.PtInRect(point))
	{
		m_GameCard.OnLButtonDown(point);
	}
	CGameFrameView::OnLButtonDown(nFlags, point);

	__LEAVE_FUNCTION 
}
//鼠标消息(左键弹起)
void CGameClientView::OnLButtonUp(UINT nFlags, CPoint point)
{
	__ENTER_FUNCTION 

	CRect rect;
	m_GameCard.GetCardDrawRect(rect);
	if (rect.PtInRect(point))
	{
		m_GameCard.OnLButtonUp(point);
		((CGameClientDlg*)AfxGetMainWnd())->PlaySound("XUANPAI",FALSE);
		((CGameClientDlg*)AfxGetMainWnd())->SendMessage(IDM_LEFT_HIT_CARD, 0, 0);
	}
	CGameFrameView::OnLButtonUp(nFlags, point);

	__LEAVE_FUNCTION 
}


//鼠标消息(右键弹起)
void CGameClientView::OnRButtonUp(UINT nFlags, CPoint Point)
{
	__ENTER_FUNCTION 
	
	__super::OnRButtonUp(nFlags, Point);

	////收起扑克
	//	//设置扑克
	//m_btOutCard.EnableWindow(FALSE);
	AfxGetMainWnd()->PostMessage(IDM_RIGHT_HIT_CARD,0,0);

	__LEAVE_FUNCTION 
}

//鼠标双击
void CGameClientView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	__ENTER_FUNCTION 

	__super::OnLButtonDblClk(nFlags, point);

	//设置扑克
	m_btOutCard.EnableWindow(FALSE);
	m_GameCard.SetAllShoot(false);

	__LEAVE_FUNCTION 
}
