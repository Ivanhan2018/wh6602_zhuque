// PlazaDlg.cpp : 实现文件
//

#include "stdafx.h" 
#include <math.h>
#include "GlobalUnits.h"
#include "Zip.h"
#include "GameInfo.h"
#include <afxtempl.h>
#include <afxctl.h>
#include "webpath.h"
#include "Mmsystem.h"
#include "GamePlaza.h"
#include "InitData.h"
#include "NewComerGuide.h"
#include "Active.h"
#include "PlazaDlg.h"
#include ".\plazadlg.h"

// CPlazaDlg 对话框

#define INNO_ISS_MSG_CLOSE			1124			//Inno 安装脚本发来的消息 关闭大厅消息
#define INNO_ISS_MSG_SELECT_GAME	1125			//Inno 安装脚本发来的消息 选择游戏

#define IDI_WEB_PLAZA_ADVERTISEMENT	1109281405		//大厅广告
#define IDI_WEB_PLAZA_NOTICE		1109281406		//大厅公告
#define IDI_WEB_PLAZA_MESSAGE		1110241427		//大厅消息

//透明定时器
#define		IDI_TIME_SET_DLG_ALPHA_VALUE		20315
#define		IDI_TIME_SET_DLG_ALPHA_END			20316
#define		TIME_SET_DLG_ALPHA_INTERVAL			20
#define		TIME_SET_DLG_ALPHA_END_INTERVAL		5000

//播放背景音乐
#define		IDI_TIMER_PLAY_BACK_MUSIC			20317
#define		TIME_PLAY_BACK_MUSIC_INTERVAL		1000

//游戏消息定时器
#define		IDI_TIMER_GAME_MSG_FLASH			20318
#define		TIME_GAME_MSG_FLASH_INTERVAL		500
#define		TIMER_WEB_LOADING_PLAZA				1111031445			//加载网页

//更新按钮定时器
#define		TIMER_UPDATEUSERINFO_OPERATE		1001

//新手指引定时器
#define		IDI_ITMER_MEWCOMER_GUIDE			4002

// 等待结束
#define IDI_TIMER_WAIT_END						4003

// 等待检查线路
#define TIME_WAIT_CHECK_LINE					4004

//数值
#define		DEF_NUM_VALUE			0.95


IMPLEMENT_DYNAMIC(CPlazaDlg, CDialog)
CPlazaDlg::CPlazaDlg(CWnd* pParent /*=NULL*/)
: CDialog(CPlazaDlg::IDD, pParent),m_pGradeInfo(NULL)
//CPlazaDlg::CPlazaDlg() : CDialog(CPlazaDlg::IDD)
{
	EnableAutomation();//必须有,否则等会用GetIDispatch()时会失败.

	m_bInitDialog	= false;
	m_bLogonPlaza	= false;
	m_bDownloadConnect=false;
	m_bStartDownloadFace=false;
	m_gameListManager.Init();
	//cxf m_gameListDlg.m_gameListManager=&m_gameListManager;//游戏列表
	//cxf 1022 m_roomListDlg.m_gameListManager=&m_gameListManager;//房间列表
	m_curTab = 0;	//当前页面
	m_pRoomViewItemCreate=NULL;
	m_bIsShowXJInfo = false;

	m_hUser32Instance = NULL;
	m_pFunShowLayer = NULL;
	m_uiCurAlphaValue = 0;
	m_hSaveNewCurWnd = NULL;
	m_hSaveOldCurWnd = NULL;
	m_byCurWndTag = 0;
	m_wTitlePos=0;

	m_memdc.m_hDC = NULL;
	m_pActivity = NULL;

	//m_bIsDrawBack = false;
	m_bIsSound = false;
	m_bIsUpdateUserInfo=false; //是否是点击更新用户信息
	m_bIsGetGlod=false;//点击获取金币
	m_bCheckLine=false;
	m_bDeleteMachineCode=false;
	/*1024
	for (int i=0; i<4; ++i)
	{
	m_TitleButtonList[i].BTImage.DestroyImage();
	m_TitleButtonList[i].rcRect.SetRect(0, 0, 0, 0);
	m_TitleButtonList[i].btStatus = 0;
	}
	*/
	m_iGradeInfoDiv = 0;
	m_bNewComerGuide=false;
	m_wNewComerGuideActionNumber=1;
	/*cxf m_bIsFlashMsg = false;
	m_dwGameMsgIndex = 0;
	m_byFlashMsgStatus = 0;*/

	m_bIsGameSoftUpdate = false;

   memset(m_hEventGameLoadOver, 0, sizeof(m_hEventGameLoadOver));
	m_strGameLoadOver = "SISEQIU_GAME_CLOSE";	// 注意：四色球游戏中的这个值要与此处一致
	m_hEventGameLoadOver[0] = CreateEvent(NULL, FALSE, FALSE, m_strGameLoadOver);

	m_pNewHandTask = NULL;

	m_dwLastConnectTime = 0l;
	m_dwConnectIntervalTime = 3l;

	CString strModuleFileName("");
	GetCurrentDirectoryPath(strModuleFileName);
	strModuleFileName.Append("\\config\\config.ini");
	m_dwConnectIntervalTime = GetPrivateProfileInt(_T("HallSet"), _T("RoomConnectIntervalTime"), 3, strModuleFileName);
	if (1 > m_dwConnectIntervalTime  ||  5 <m_dwConnectIntervalTime)
		m_dwConnectIntervalTime = 3;
}

CPlazaDlg::~CPlazaDlg()
{
	m_fontNickName.DeleteObject();
	m_fontXPValue.DeleteObject();
	m_fontDrawXJ.DeleteObject();

	SafeDelete(m_pRoomViewItemCreate);
	m_pRoomViewItemCreate = NULL;

	if (NULL != m_hUser32Instance)
	{
		FreeLibrary(m_hUser32Instance); 
	}
	m_hUser32Instance = NULL;
	m_pFunShowLayer = NULL;

	m_memdc.DeleteDC();
	//等级信息
	if (NULL != m_pGradeInfo)
	{
		SafeDelete(m_pGradeInfo);
		m_pGradeInfo = NULL;
	}
	m_pGradeInfo = NULL;

	if ( m_CustomFace.pFaceData != NULL ) 
	{
		SafeDeleteArray(m_CustomFace.pFaceData);
	}

	if (NULL != m_pActivity)
	{
		SafeDelete(m_pActivity);
		m_pActivity = NULL;
	}
}

void CPlazaDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	// 	DDX_Control(pDX,IDC_BT_SHOP,m_btnGameShop);
	// 	DDX_Control(pDX,IDC_BT_BEAUTY_SALON,m_btnBeautySalon);
	DDX_Control(pDX,IDC_BT_RE_LOGON,m_btnReLogon);
	DDX_Control(pDX,IDC_BT_RETURN,m_btnReturn);
	DDX_Control(pDX,IDC_BT_SET,m_btnSound);
	DDX_Control(pDX,IDCANCEL,m_btnClose);  
	DDX_Control(pDX,IDC_MINSIZE,m_btnMinsize); 
	DDX_Control(pDX,IDC_VISIT_WEB,m_btnVisitWeb);
	DDX_Control(pDX,IDC_ACCOUNTS_MANAGE,m_btnAccountsManage);
	DDX_Control(pDX,IDC_GAME_HELP,m_btnGameHelp);
	DDX_Control(pDX,IDC_QUESTION_FEEDBACK,m_btnQuestionFeedBack);
	DDX_Control(pDX,IDC_CONTACT_SERVISE,m_btnContactServise);
	DDX_Control(pDX,IDC_GAME_LIST,m_btnGameList);
	DDX_Control(pDX,IDC_TOUNAMENT,m_btnTounament);
	DDX_Control(pDX,IDC_ACCOUNTS_RECHARGE,m_btnAccountsRecharge);
	DDX_Control(pDX,IDC_PRIZE_EXCHARGE,m_btnPrizeExcharge);
	DDX_Control(pDX,IDC_PROPS_SHOP,m_btnPropsShop);
	DDX_Control(pDX,IDC_NOVICE_TASK,m_btnNoviceTask);
	DDX_Control(pDX, IDC_BACK_PACK, m_btnBackpack);
	DDX_Control(pDX, IDC_USER_INFO, m_btnUserInfo);
	DDX_Control(pDX, IDC_BANK, m_btnBank);
	DDX_Control(pDX, IDC_EMAIL_MESSAGE, m_btnEmailMessage);
	DDX_Control(pDX, IDC_PLAZA_ROLLBACK, m_btnRollback);
	DDX_Control(pDX, IDC_PLAZA_NEXT_PAGE, m_btnNextPage);
	DDX_Control(pDX, IDC_UPDATE_USERINFO, m_btnUpdateUserInfo);
	DDX_Control(pDX, IDC_GUIDE, m_btnNewComerGuide);
}


BEGIN_MESSAGE_MAP(CPlazaDlg, CDialog) 
	ON_COMMAND(IDM_CONNECT_SERVER,	OnCommandConnect)
	ON_COMMAND(IDM_CANCEL_CONNECT,	OnCommandCancelConnect)
	ON_COMMAND(IDM_LOGON_PLAZA,		OnCommandLogon)
	ON_COMMAND(IDM_CALL_ROOM_LIST,	OnCallRoomList)
	ON_COMMAND(IDM_CALL_GAME_LIST,	OnCallGameList)
	ON_COMMAND(IDM_CALL_TABLE_LIST,	OnCallTableList)
	ON_COMMAND(IDM_CALL_EXIT_GAME,  OnBnExitLogoUI)
	ON_COMMAND(IDM_CALL_CANCEL_GAME,  OnBnCancelGame)
	ON_COMMAND(IDB_CALL_UPDATE_USERIMAGE, OnCannUpdateUserImage)

	ON_WM_SYSCOMMAND()
	ON_WM_TIMER()
	ON_WM_MOUSEMOVE()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOVE()
	ON_WM_PAINT()
	ON_WM_SIZE()
	//	ON_WM_WINDOWPOSCHANGED()
	ON_BN_CLICKED(IDC_BT_RETURN,&CPlazaDlg::OnBnClickedReturn)
	ON_BN_CLICKED(IDC_BT_SET,&CPlazaDlg::OnBnClickedSet)
	ON_BN_CLICKED(IDC_MINSIZE,&CPlazaDlg::OnBnClickedMinsize) 
	ON_BN_CLICKED(IDCANCEL,&CPlazaDlg::ConfirmClose)
	ON_BN_CLICKED(IDC_BT_RE_LOGON,&CPlazaDlg::OnBnClickedReLogon)
	ON_BN_CLICKED(IDC_VISIT_WEB,&CPlazaDlg::OnBnClickedVisitWeb)
	ON_BN_CLICKED(IDC_ACCOUNTS_MANAGE,&CPlazaDlg::OnBnClickedAccountsManage)
	ON_BN_CLICKED(IDC_GAME_HELP,&CPlazaDlg::OnBnClickedGameHelp)
	ON_BN_CLICKED(IDC_QUESTION_FEEDBACK,&CPlazaDlg::OnBnClickedQuestionFeedBack)
	ON_BN_CLICKED(IDC_CONTACT_SERVISE,&CPlazaDlg::OnBnClickedContactServise)
	ON_BN_CLICKED(IDC_BACK_PACK,&CPlazaDlg::OnBnClickedBackPack)
	ON_BN_CLICKED(IDC_USER_INFO,&CPlazaDlg::OnBnClickedUserInfo)
	ON_BN_CLICKED(IDC_BANK,&CPlazaDlg::OnBnClickedBank)
	ON_BN_CLICKED(IDC_EMAIL_MESSAGE,&CPlazaDlg::OnBnClickedEmailMessage)

	ON_BN_CLICKED(IDC_GAME_LIST,&CPlazaDlg::OnBnClickedGameList)
	ON_BN_CLICKED(IDC_TOUNAMENT,&CPlazaDlg::OnBnClickedTounament)
	ON_BN_CLICKED(IDC_ACCOUNTS_RECHARGE,&CPlazaDlg::OnBnClickedAccountsRecharge)
	ON_BN_CLICKED(IDC_PRIZE_EXCHARGE,&CPlazaDlg::OnBnClickedPrizeExcharge)
	ON_BN_CLICKED(IDC_PROPS_SHOP,&CPlazaDlg::OnBnClickedPropsShop)
	ON_BN_CLICKED(IDC_NOVICE_TASK,&CPlazaDlg::OnBnClickedNoviceTask)

	ON_BN_CLICKED(IDC_PLAZA_ROLLBACK,&CPlazaDlg::OnBnClickedRollBack)
	ON_BN_CLICKED(IDC_PLAZA_NEXT_PAGE,&CPlazaDlg::OnBnClickedNextPage)
	ON_BN_CLICKED(IDC_UPDATE_USERINFO,&CPlazaDlg::OnBnClickedUpdateUserInfo)
	ON_BN_CLICKED(IDC_GUIDE, &CPlazaDlg::OnBnClickedNewComerGuide)
	// 	ON_BN_CLICKED(IDC_BT_BEAUTY_SALON,&CPlazaDlg::OnBnClickedBeautySalon)
	// 	ON_BN_CLICKED(IDC_BT_SHOP,&CPlazaDlg::OnBnClickedGameShop)
	//ON_BN_CLICKED(IDB_BT_DEPOSITORY,&CPlazaDlg::OnBnClickedDepository)
	//ON_BN_CLICKED(IDB_BT_SHOP,&CPlazaDlg::OnBnClickedShop)
	//ON_BN_CLICKED(IDB_BT_TASK,&CPlazaDlg::OnBnClickedTask)
	//ON_BN_CLICKED(IDC_BT_RE_GAME_PLAZY, &CPlazaDlg::OnBnClickedGamePlazy)

	ON_MESSAGE(INNO_ISS_MSG_CLOSE,	OnCallInnoIssMsgClose)
	ON_MESSAGE(INNO_ISS_MSG_SELECT_GAME, OnCallInnoIssMsgSelectGame)
	ON_MESSAGE(IDM_CALL_SHOW_CUSTOMER, OnCallShowCustomer)
	ON_MESSAGE(IDM_CHECK_LINE, OnCallCheckConnect)

END_MESSAGE_MAP()


BEGIN_DISPATCH_MAP(CPlazaDlg, CDialog)
	//DISP_FUNCTION_ID(CPlazaDlg, "oncontextmenu", DISPID_HTMLDOCUMENTEVENTS2_ONCONTEXTMENU, onHtmlContextMenu, VT_BOOL, VTS_DISPATCH)
END_DISPATCH_MAP()

BEGIN_EVENTSINK_MAP(CPlazaDlg, CDialog)
	ON_EVENT(CPlazaDlg, 23, 259, OnDocumentCompleteExplorer3, VTS_DISPATCH VTS_PVARIANT)
END_EVENTSINK_MAP()

BOOL CPlazaDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	//cxf
	int width   =   GetSystemMetrics(SM_CXSCREEN); 
	int height   =   GetSystemMetrics(SM_CYSCREEN);

	SetWindowPos(NULL, (width-960)/2, (height-720)/2, 960, 720, 0);
	CRect rtClientRect;
	GetClientRect(&rtClientRect);
	//end
	m_hUser32Instance = NULL;
	m_pFunShowLayer = NULL;
	m_uiCurAlphaValue = 0;
	m_hSaveNewCurWnd = NULL;
	m_hSaveOldCurWnd = NULL;
	m_byCurWndTag = 0;
	m_bIsUpdateUserInfo=false;//是否是点击更新用户信息
	m_bIsGetGlod=false;//点击获取金币
	m_bCheckLine=false;
	m_bDeleteMachineCode=false;
	m_bNewComerGuide=false;
	m_wNewComerGuideActionNumber=1;

	// 添加库
	m_hUser32Instance = LoadLibrary("User32.DLL");
	if(m_hUser32Instance)
	{
		// 读取函数指针
		m_pFunShowLayer = (ShowLayer)GetProcAddress(m_hUser32Instance, "SetLayeredWindowAttributes");
	}

	//创建字体
	BOOL bIsRet = m_fontNickName.CreateFont(20,0,0,0,200,0,0,0,134,3,2,1,2,TEXT("微软雅黑"));
	if (!bIsRet)
	{
		m_fontNickName.CreateFont(12,0,0,0,200,0,0,0,134,3,2,1,2,TEXT("宋体"));
	}
	bIsRet = m_fontXPValue.CreateFont(16,0,0,0,200,0,0,0,134,3,2,1,2,TEXT("微软雅黑"));
	if (!bIsRet)
	{
		m_fontXPValue.CreateFont(12,0,0,0,200,0,0,0,134,3,2,1,2,TEXT("宋体"));
	}
	m_fontDrawXJ.CreateFont(12,0,0,0,200,0,0,0,134,3,2,1,2,TEXT("宋体"));

	m_ToolTipCtrl.Create(this);
	m_ToolTipCtrl.Activate(TRUE);

	m_ServerItemView.InitServerItemView(/*this*/);
	g_GlobalUnits.m_ServerListManager.InitServerListManager(&m_ServerItemView);

	//初始化界面 	
	//cxf m_pDlgLogon->Create(IDD_LOGON, this);
	//cxf m_gameListDlg.Create(IDD_GAME_LIST, this);
	//cxf 1022 m_roomListDlg.Create(IDD_ROOM_LIST, this);
	//m_friendlistDlg.Create(IDD_FRIEND_LIST);
	//cxf 1022 m_TaskDlg.Create(CTaskDialog::IDD, this);
	//cxf 1022 m_shopDlg.Create(CShopDialog::IDD, this);
	//cxf 1022 m_doteyBoxDlg.Create(CDoteyBoxDialog::IDD, this);
	m_GameListHot.Create(CGameListHot::IDD, this);
	m_PlazaGameWeb.Create(CPlazaGameWeb::IDD, this);
	m_GameListCenterDlg.Create(IDD_GAME_LIST_CENTER,this);
	m_RoomListCenterDlg.Create(CRoomListCenterDlg::IDD,this);
	/*cxf for (int i=0; i<4; ++i)
	{
	m_TitleButtonList[i].BTImage.DestroyImage();
	m_TitleButtonList[i].rcRect.SetRect(0, 0, 0, 0);
	m_TitleButtonList[i].btStatus = 0;
	}*/

	//初始化背景信息
	InitDlgBK();
	SetWindowText("藏宝湾");

	m_UserXJRect.left = 755;
	m_UserXJRect.top = 397;
	m_UserXJRect.right = m_UserXJRect.left + 60;
	m_UserXJRect.bottom = m_UserXJRect.top + 14;
	m_UserXJInfoRect.left = 753;
	m_UserXJInfoRect.top = 315;
	m_UserXJInfoRect.right = m_UserXJInfoRect.left + 114;
	m_UserXJInfoRect.bottom = m_UserXJInfoRect.top + 77;

	m_rcTitleArea.left=428;
	m_rcTitleArea.top=148;
	m_rcTitleArea.right=m_rcTitleArea.left+84;
	m_rcTitleArea.bottom=m_rcTitleArea.top+22;

	//新手指引区域
	m_rcNewComerGuideArea.left = 705;
	m_rcNewComerGuideArea.top = 30;
	m_rcNewComerGuideArea.right = m_rcNewComerGuideArea.left + 94;
	m_rcNewComerGuideArea.bottom = m_rcNewComerGuideArea.top + 59;

	//cxf 
	//获取声音配置
	UINT uiReturn = GetCommonConfig("Sound","EnablePlazeBGMusic",1);

	//获取当前路径
	m_strCurrentPathPlaza="";
	GetResPath("\\data\\plaza\\",m_strCurrentPathPlaza);

	if (1==uiReturn)
	{//开启声音
		m_bIsSound=true;
		m_btnSound.LoadRgnImage(TEXT(m_strCurrentPathPlaza+"BTSoundOpen.png"),false);
	}else
	{//关闭声音
		m_bIsSound=false;
		m_btnSound.LoadRgnImage(TEXT(m_strCurrentPathPlaza+"BTSoundClose.png"),false);
	}
	//按钮
	m_btnClose.LoadRgnImage(TEXT(m_strCurrentPathPlaza+"BTClose.png"),false);
	m_btnMinsize.LoadRgnImage(TEXT(m_strCurrentPathPlaza+"BTMinsize.png"),false);
	m_btnVisitWeb.LoadRgnImage(TEXT(m_strCurrentPathPlaza+"BTVisitWeb.png"),false);
	m_btnAccountsManage.LoadRgnImage(TEXT(m_strCurrentPathPlaza+"BTAccountsManage.png"),false);
	m_btnGameHelp.LoadRgnImage(TEXT(m_strCurrentPathPlaza+"BTGameHelp.png"),false);
	m_btnQuestionFeedBack.LoadRgnImage(TEXT(m_strCurrentPathPlaza+"BTQuestionFeedBack.png"),false);
	m_btnContactServise.LoadRgnImage(TEXT(m_strCurrentPathPlaza+"BTContactServise.png"),false);
	m_btnGameList.LoadRgnImage(TEXT(m_strCurrentPathPlaza+"BTGameList.png"),true);
	m_btnTounament.LoadRgnImage(TEXT(m_strCurrentPathPlaza+"BTTounament.png"),true);
	m_btnAccountsRecharge.LoadRgnImage(TEXT(m_strCurrentPathPlaza+"BTAccountsRecharge.png"),true);
	m_btnPrizeExcharge.LoadRgnImage(TEXT(m_strCurrentPathPlaza+"BTPrizeExcharge.png"),true);
	m_btnPropsShop.LoadRgnImage(TEXT(m_strCurrentPathPlaza+"BTPropsShop.png"),true);
	m_btnNoviceTask.LoadRgnImage(TEXT(m_strCurrentPathPlaza+"BTNoviceTask.png"),true);
	m_btnBackpack.LoadRgnImage(TEXT(m_strCurrentPathPlaza+"BTBackPack.png"),false);
	m_btnUserInfo.LoadRgnImage(TEXT(m_strCurrentPathPlaza+"BTUserInfo.png"),false);
	m_btnBank.LoadRgnImage(TEXT(m_strCurrentPathPlaza+"BTBank.png"),false);
	m_btnEmailMessage.LoadRgnImage(TEXT(m_strCurrentPathPlaza+"BTEmailMessage.png"),false);
	m_btnRollback.LoadRgnImage(TEXT(m_strCurrentPathPlaza+"BTRollBack.png"),false);
	m_btnNextPage.LoadRgnImage(TEXT(m_strCurrentPathPlaza+"BTNextPage.png"),false);
	m_btnUpdateUserInfo.LoadRgnImage(TEXT(m_strCurrentPathPlaza+"BTUpdateUserInfo.png"), false);
	m_btnNewComerGuide.LoadRgnImage(TEXT(m_strCurrentPathPlaza+"BTGuide.png"), false);
	//贴图
	m_ImageTitle.LoadImage(TEXT(m_strCurrentPathPlaza+"ImageTitle.png"));
	m_ImageNewComerGuide.LoadImage(TEXT(m_strCurrentPathPlaza+"ImageNewComerGuide.png"));//新手指引

	/*cxf HINSTANCE hInstance=g_GlobalUnits.m_PlatformResourceModule->GetResInstance();	
	//	m_btnGameShop.LoadRgnImage(hInstance,g_GlobalUnits.m_PlatformFrameImage.uFrameNavigation[3]);
	// 	m_btnBeautySalon.LoadRgnImage(hInstance,g_GlobalUnits.m_PlatformFrameImage.uFrameNavigation[7]);
	m_btnReLogon.LoadRgnImage(hInstance,g_GlobalUnits.m_PlatformFrameImage.uBtFrameRelogon);
	m_btnReturn.LoadRgnImage(hInstance,g_GlobalUnits.m_PlatformFrameImage.uBtFrameRetrue);
	m_btnSound.LoadRgnImage(hInstance,g_GlobalUnits.m_PlatformFrameImage.uBtFrameOption);
	m_btnClose.LoadRgnImage(hInstance,g_GlobalUnits.m_PlatformFrameImage.uBtFrameClose);
	m_btnMinsize.LoadRgnImage(hInstance,g_GlobalUnits.m_PlatformFrameImage.uBtFrameMin);

	m_ToolTipCtrl.AddTool(&m_btnReLogon,TEXT("重新登陆"));
	m_ToolTipCtrl.AddTool(&m_btnReturn,TEXT("返回"));
	*/
	//提示
	m_ToolTipCtrl.AddTool(&m_btnSound,TEXT("播放背景音乐"));
	m_ToolTipCtrl.AddTool(&m_btnClose,TEXT("关闭游戏大厅"));
	m_ToolTipCtrl.AddTool(&m_btnMinsize,TEXT("最小化窗口"));
	m_ToolTipCtrl.AddTool(&m_btnUserInfo,TEXT("玩家信息"));
	m_ToolTipCtrl.AddTool(&m_btnBank,TEXT("银行"));
	m_ToolTipCtrl.AddTool(&m_btnEmailMessage,TEXT("系统信息"));
	m_ToolTipCtrl.AddTool(&m_btnBackpack,TEXT("背包"));
	//网页
	CString strPath("");
	GetResPath("\\Url\\plaza\\",strPath);
	m_webPlazaAdvertisement.Create(NULL,NULL,WS_VISIBLE | WS_CHILD,CRect(0,0,0,0),this,IDI_WEB_PLAZA_ADVERTISEMENT,NULL);
	m_webPlazaNotice.Create(NULL,NULL,WS_VISIBLE | WS_CHILD,CRect(0,0,0,0),this,IDI_WEB_PLAZA_NOTICE,NULL);
	m_webPlazaMessage.Create(NULL,NULL,WS_VISIBLE | WS_CHILD,CRect(0,0,0,0),this,IDI_WEB_PLAZA_MESSAGE,NULL);
	m_webPlazaTitle.Create(NULL,NULL,WS_VISIBLE | WS_CHILD,CRect(0,0,0,0),this,IDI_WEB_PLAZA_ADVERTISEMENT,NULL);
	m_webPlazaAdvertisement.Navigate(TEXT(strPath+"webPlazaAdvertisement.html"),NULL,NULL,NULL,NULL);
	m_webPlazaNotice.Navigate(TEXT(strPath+"webPlazaNotice.html"),NULL,NULL,NULL,NULL);
	m_webPlazaMessage.Navigate(TEXT(strPath+"webPlazaMessage.html"),NULL,NULL,NULL,NULL);
	m_webPlazaTitle.Navigate(TEXT(strPath+"webPlazaTitle.html"),NULL,NULL,NULL,NULL);
	//end

	m_GameFlash.Create(IDD_STATUS, this);
	m_GameFlash.InitFlash("data\\status\\BKStatus.png", 12, CPoint(560/2, 320/2), 100);

	//锁定屏幕
	LockWindowUpdate();

	HDWP hDwp=BeginDeferWindowPos(32);
	const UINT uFlags=SWP_NOACTIVATE|SWP_NOZORDER|SWP_NOCOPYBITS|SWP_NOSIZE;
	//cxf DeferWindowPos(hDwp,m_btnReLogon,NULL,800,13,0,0,uFlags);
	//cxf DeferWindowPos(hDwp,m_btnReturn,NULL,826,13,0,0,uFlags);
	DeferWindowPos(hDwp,m_btnSound,NULL,786,5,0,0,uFlags);
	DeferWindowPos(hDwp,m_btnMinsize,NULL,838,5,0,0,uFlags);
	DeferWindowPos(hDwp,m_btnClose,NULL,888,5,0,0,uFlags);
	DeferWindowPos(hDwp,m_btnVisitWeb,NULL,350,7,0,0,uFlags);
	DeferWindowPos(hDwp,m_btnAccountsManage,NULL,427,7,0,0,uFlags);
	DeferWindowPos(hDwp,m_btnGameHelp,NULL,504,7,0,0,uFlags);
	DeferWindowPos(hDwp,m_btnNewComerGuide,NULL,581,7,0,0,uFlags);
	DeferWindowPos(hDwp,m_btnQuestionFeedBack,NULL,656,7,0,0,uFlags);
	DeferWindowPos(hDwp,m_btnContactServise,NULL,rtClientRect.Width()-119,58,0,0,uFlags);

	DeferWindowPos(hDwp,m_btnGameList,NULL,160,90,0,0,uFlags);
	DeferWindowPos(hDwp,m_btnTounament,NULL,292,90,0,0,uFlags);
	DeferWindowPos(hDwp,m_btnAccountsRecharge,NULL,384,90,0,0,uFlags);
	DeferWindowPos(hDwp,m_btnPrizeExcharge,NULL,476,90,0,0,uFlags);
	DeferWindowPos(hDwp,m_btnPropsShop,NULL,568,90,0,0,uFlags);
	DeferWindowPos(hDwp,m_btnNoviceTask,NULL,660,90,0,0,uFlags);

	DeferWindowPos(hDwp,m_btnUserInfo,NULL,758,512,0,0,uFlags);
	DeferWindowPos(hDwp,m_btnEmailMessage,NULL,805,512,0,0,uFlags);
	DeferWindowPos(hDwp,m_btnBank,NULL,852,512,0,0,uFlags);
	DeferWindowPos(hDwp,m_btnBackpack,NULL,899,512,0,0,uFlags);
	DeferWindowPos(hDwp,m_btnRollback,NULL,217,153,0,0,uFlags);
	DeferWindowPos(hDwp,m_btnNextPage,NULL,701,153,0,0,uFlags);
	DeferWindowPos(hDwp,m_btnUpdateUserInfo,NULL,909,392,0,0,uFlags);

	m_webPlazaMessage.MoveWindow(211,538,536,21);
	m_webPlazaAdvertisement.MoveWindow(15,578,727,128);
	m_webPlazaNotice.MoveWindow(761,605,185,103);
	m_webPlazaTitle.MoveWindow(204,32,480,58);

	EndDeferWindowPos(hDwp);
	ResizeFrame();
	//解除锁定
	UnlockWindowUpdate();

	//cxf 1022 m_TaskDlg.ShowWindow(SW_HIDE);
	//cxf 1022 m_shopDlg.ShowWindow(SW_HIDE);
	//cxf 1022 m_doteyBoxDlg.ShowWindow(SW_HIDE);
	//cxf m_gameListDlg.ShowWindow(SW_HIDE);
	//cxf 1022 m_roomListDlg.ShowWindow(SW_HIDE);
	m_btnReturn.ShowWindow(SW_HIDE);
	m_btnReLogon.ShowWindow(SW_HIDE);
	m_GameListHot.ShowWindow(SW_SHOW);
	m_PlazaGameWeb.ShowWindow(SW_HIDE);
	m_btnSound.ShowWindow(SW_SHOW);
	m_btnClose.ShowWindow(SW_SHOW);
	m_btnMinsize.ShowWindow(SW_SHOW);
	m_btnVisitWeb.ShowWindow(SW_SHOW);
	m_btnAccountsManage.ShowWindow(SW_SHOW);
	m_btnGameHelp.ShowWindow(SW_SHOW);
	m_btnQuestionFeedBack.ShowWindow(SW_SHOW);
	m_btnContactServise.ShowWindow(SW_SHOW);
	m_btnBackpack.ShowWindow(SW_SHOW);
	m_btnUserInfo.ShowWindow(SW_SHOW);
	m_btnBank.ShowWindow(SW_SHOW);
	m_btnEmailMessage.ShowWindow(SW_SHOW);
	m_GameListCenterDlg.ShowWindow(SW_SHOW);
	m_RoomListCenterDlg.ShowWindow(SW_HIDE);
	m_btnRollback.ShowWindow(SW_HIDE);
	m_btnNextPage.ShowWindow(SW_HIDE);
	m_btnUpdateUserInfo.ShowWindow(SW_SHOW);
	m_btnNewComerGuide.ShowWindow(SW_SHOW);

	//cxf 1024 m_TitleButtonList[0].btStatus = 3;
	SetTimer(TIMER_WEB_LOADING_PLAZA,20,NULL);
	//cxf ShowTab(0);

	// 检查线路
	GetResPath("\\config\\",strPath);
	m_IpFinder.Open(strPath + _T("ipwry.dat"));
	return TRUE;
}

void CPlazaDlg::ResizeFrame()
{
	HDWP hDwp=BeginDeferWindowPos(32);
	//cxf if (::IsWindow(m_gameListDlg.GetSafeHwnd()))
	//{
	//	DeferWindowPos(hDwp, m_gameListDlg.GetSafeHwnd(), NULL, 287, 81, 659, 626, SWP_NOREDRAW|SWP_NOZORDER);
	//	//::MoveWindow(m_gameListDlg.GetSafeHwnd(), wndrect.left+287,wndrect.top+81, 659, 626, FALSE);
	//	//m_gameListDlg.SetWindowPos(NULL, wndrect.left+287,wndrect.top+81, 659, 626, SWP_NOSENDCHANGING | SWP_NOREDRAW);
	//}
	////cxf 1022  if (::IsWindow(m_roomListDlg.GetSafeHwnd()))
	//{
	//	DeferWindowPos(hDwp, m_roomListDlg.GetSafeHwnd(), NULL, 287, 81, 659, 626, SWP_NOREDRAW|SWP_NOZORDER);
	//	//::MoveWindow(m_roomListDlg.GetSafeHwnd(), wndrect.left+287,wndrect.top+81, 659, 626, FALSE);
	//	//m_roomListDlg.SetWindowPos(NULL, wndrect.left+287,wndrect.top+81, 659, 626, SWP_NOSENDCHANGING | SWP_NOREDRAW);
	//}
	if (m_pRoomViewItemCreate && ::IsWindow(m_pRoomViewItemCreate->GetSafeHwnd()))
	{
		DeferWindowPos(hDwp, m_pRoomViewItemCreate->GetSafeHwnd(), NULL, 0, 0, 960, 720, SWP_NOREDRAW|SWP_NOZORDER);
		//cxf 1022 DeferWindowPos(hDwp, m_pRoomViewItemCreate->GetSafeHwnd(), NULL, 287, 81, 659, 626, SWP_NOREDRAW|SWP_NOZORDER);
		//::MoveWindow(m_pRoomViewItemCreate->GetSafeHwnd(),wndrect.left+287,wndrect.top+81, 659, 626, FALSE);
		//m_pRoomViewItemCreate->SetWindowPos(NULL, wndrect.left+287,wndrect.top+81, 659, 626, SWP_NOSENDCHANGING | SWP_NOREDRAW);
	}
	////cxf 1022 if (::IsWindow(m_shopDlg.GetSafeHwnd()))
	//{
	//	DeferWindowPos(hDwp, m_shopDlg.GetSafeHwnd(), NULL, 287, 81, 659, 626, SWP_NOREDRAW|SWP_NOZORDER);
	//	//::MoveWindow(m_shopDlg.GetSafeHwnd(), wndrect.left+287,wndrect.top+81, 659, 626, FALSE);
	//	//m_shopDlg.SetWindowPos(NULL, wndrect.left+287,wndrect.top+81, 659, 626, SWP_NOSENDCHANGING | SWP_NOREDRAW);
	//}
	////cxf 1022 if (::IsWindow(m_doteyBoxDlg.GetSafeHwnd()))
	//{
	//	DeferWindowPos(hDwp, m_doteyBoxDlg.GetSafeHwnd(), NULL, 287, 81, 659, 626, SWP_NOREDRAW|SWP_NOZORDER);
	//	//::MoveWindow(m_doteyBoxDlg.GetSafeHwnd(), wndrect.left+287,wndrect.top+81, 659, 626, FALSE);
	//	//m_doteyBoxDlg.SetWindowPos(NULL, wndrect.left+287,wndrect.top+81, 659, 626, SWP_NOSENDCHANGING | SWP_NOREDRAW);
	//}
	////cxf 1022 if (::IsWindow(m_TaskDlg.GetSafeHwnd()))
	//{
	//	DeferWindowPos(hDwp, m_TaskDlg.GetSafeHwnd(), NULL, 287, 81, 659, 626, SWP_NOREDRAW|SWP_NOZORDER);
	//	//::MoveWindow(m_TaskDlg.GetSafeHwnd(), wndrect.left+287,wndrect.top+81, 659, 626, FALSE);
	//	//m_TaskDlg.SetWindowPos(NULL, wndrect.left+287,wndrect.top+81, 659, 626, SWP_NOSENDCHANGING | SWP_NOREDRAW);
	//}
	if (::IsWindow(m_GameListHot.GetSafeHwnd()))
	{
		DeferWindowPos(hDwp, m_GameListHot.GetSafeHwnd(), NULL, 15, 177, 189, 382, SWP_NOREDRAW|SWP_NOZORDER);
	}
	//大厅中间网页
	if (::IsWindow(m_PlazaGameWeb.GetSafeHwnd()))
	{
		DeferWindowPos(hDwp, m_PlazaGameWeb.GetSafeHwnd(), NULL, 211, 177, 536, 361, SWP_NOREDRAW|SWP_NOZORDER);
	}
	//新游戏列表
	if (::IsWindow(m_GameListCenterDlg.GetSafeHwnd()))
	{
		DeferWindowPos(hDwp, m_GameListCenterDlg.GetSafeHwnd(), NULL, 211, 177, 536, 361, SWP_NOREDRAW|SWP_NOZORDER);
	}
	//房间列表
	if (::IsWindow(m_RoomListCenterDlg.GetSafeHwnd()))
	{
		DeferWindowPos(hDwp, m_RoomListCenterDlg.GetSafeHwnd(), NULL, 211, 177, 536, 361, SWP_NOREDRAW|SWP_NOZORDER);
	}
	EndDeferWindowPos(hDwp);
}


//初始化图片信息
void CPlazaDlg::InitDlgBK()
{
	HINSTANCE hInstance = AfxGetInstanceHandle();

	//cxf
	//装载图片
	CString strPath("");
	GetResPath("\\data\\plaza\\",strPath);
	m_ImageBack.LoadImage(TEXT(strPath+"BKPlaza.png"));
	m_Exp2Image.LoadImage(TEXT(strPath+"experience.png"));//前景经验条
	m_XJInfoBackImage.LoadImage(TEXT(strPath+"BKGrade.png"));
	//m_ImageBack.SetLoadInfo(IDB_PLAZA_BG, hInstance, false);
	//m_ImageNums.SetImageResource("PNG_NUM_IMAGE");

	//装载数字图片
	CString szPath("");
	GetResPath("\\data\\",szPath);
	szPath.Append(TEXT("Number_Hei.png"));
	char* chData = szPath.GetBuffer();
	m_ImageNums.SetImageResource(chData);

	m_ImageNums.SetSeparator(false);
	m_ImageNums.SetSign(false);
	m_ImageNums.SetNumAlign(IA_CENTER);
	m_ImageNums.SetSeparatorWH(7, 14);

	//装载数字图片
	szPath="";
	GetResPath("\\data\\",szPath);
	szPath.Append(TEXT("Number_Cai.png"));
	chData = szPath.GetBuffer();
	m_ImageNumsTel.SetImageResource(chData);

	//m_PngHat.LoadImage(hInstance, TEXT("PNG_IMAGE_HAT"));
	//m_UserInfo.LoadImage(hInstance, TEXT("PNG_USER_INFO_BACK_IMAGE"));

	//m_Exp1Image.LoadImage(hInstance, TEXT("PNG_EXPERIENCEL_IMAGE1"));//背景经验条
	//m_Exp2Image.LoadImage(hInstance, TEXT("PNG_EXPERIENCEL_IMAGE2"));//前景经验条
	//end

	/*cxf
	m_TitleButtonList[0].BTImage.LoadImage(hInstance, TEXT("BT_PNG_IMAGE_GAMEPLAZY"));
	m_TitleButtonList[1].BTImage.LoadImage(hInstance, TEXT("BT_PNG_IMAGE_TASK"));
	m_TitleButtonList[2].BTImage.LoadImage(hInstance, TEXT("BT_PNG_IMAGE_CBG"));
	m_TitleButtonList[3].BTImage.DestroyImage();
	m_TitleButtonList[3].BTImage.LoadImage(hInstance, TEXT("BT_PNG_IMAGE_SHOP"));

	m_TitleButtonList[0].rcRect.left = 327;
	m_TitleButtonList[0].rcRect.top = 24;
	m_TitleButtonList[0].rcRect.right = m_TitleButtonList[0].rcRect.left + m_TitleButtonList[0].BTImage.GetWidth()/4;
	m_TitleButtonList[0].rcRect.bottom = m_TitleButtonList[0].rcRect.top + m_TitleButtonList[0].BTImage.GetHeight();

	m_TitleButtonList[1].rcRect.left = 439;
	m_TitleButtonList[1].rcRect.top = 24;
	m_TitleButtonList[1].rcRect.right = m_TitleButtonList[1].rcRect.left + m_TitleButtonList[1].BTImage.GetWidth()/4;
	m_TitleButtonList[1].rcRect.bottom = m_TitleButtonList[1].rcRect.top + m_TitleButtonList[1].BTImage.GetHeight();

	m_TitleButtonList[2].rcRect.left = 551;
	m_TitleButtonList[2].rcRect.top = 24;
	m_TitleButtonList[2].rcRect.right = m_TitleButtonList[2].rcRect.left + m_TitleButtonList[2].BTImage.GetWidth()/4;
	m_TitleButtonList[2].rcRect.bottom = m_TitleButtonList[2].rcRect.top + m_TitleButtonList[2].BTImage.GetHeight();

	m_TitleButtonList[3].rcRect.left = 663;
	m_TitleButtonList[3].rcRect.top = 24;
	m_TitleButtonList[3].rcRect.right = m_TitleButtonList[3].rcRect.left + m_TitleButtonList[3].BTImage.GetWidth()/4;
	m_TitleButtonList[3].rcRect.bottom = m_TitleButtonList[3].rcRect.top + m_TitleButtonList[3].BTImage.GetHeight();*/


	//消息
	/*cxf m_byFlashMsgStatus = 0;
	m_bIsFlashMsg = false;
	m_GameMsgImage.LoadImage(hInstance, TEXT("PNG_IMAGE_GAME_MSG"));
	m_rcGameMsgArea.SetRect(0, 0, 0, 0);
	if (!m_GameMsgImage.IsNull())
	{
	m_rcGameMsgArea.left = 581;
	m_rcGameMsgArea.top = 460;
	m_rcGameMsgArea.right = m_rcGameMsgArea.left + m_GameMsgImage.GetWidth();
	m_rcGameMsgArea.bottom = m_rcGameMsgArea.top + m_GameMsgImage.GetHeight();
	}*/
}


//连接服务器
void CPlazaDlg::OnCommandConnect()
{
	//创建组件
	if (m_ClientSocket.GetInterface()==NULL)
	{
		try
		{
			IUnknownEx * pIUnknownEx=(IUnknownEx *)QueryInterface(IID_IUnknownEx,VER_IUnknownEx);
			if (m_ClientSocket.CreateInstance()==false) 
			{
				throw TEXT("网络组件创建失败");
			}
			if (m_ClientSocket->SetTCPSocketSink(pIUnknownEx)==false) 
			{
				throw TEXT("网络组件回调接口设置失败");
			}
		}
		catch (...)
		{
			ShowMessageBox(TEXT("网络组件创建失败，请重新下载游戏大厅！"),MB_ICONSTOP);
			PostMessage(WM_COMMAND,IDM_LOGON_PLAZA,0);
			return;
		}
	}

	//地址解释
	CRegKey RegServer;
	LPCTSTR pszServerIP=m_pDlgLogon->GetLogonServer();
	TCHAR szRegServer[256]=TEXT(""),szServerAddr[64]=TEXT("");
	_snprintf(szRegServer,sizeof(szRegServer),TEXT("%s\\%s"),REG_LOGON_SERVER,pszServerIP);

	if (RegServer.Open(HKEY_CURRENT_USER,szRegServer,KEY_READ)==ERROR_SUCCESS)
	{
		TCHAR szReadData[1024]=TEXT("");
		DWORD dwReadData=0L,dwDataType=0L,dwDataSize=sizeof(szReadData);
		LONG lErrorCode=RegServer.QueryValue(TEXT("ServerAddr"),&dwDataType,szReadData,&dwDataSize);
		if (lErrorCode==ERROR_SUCCESS) 
		{
			CXOREncrypt::CrevasseData(szReadData,szServerAddr,sizeof(szServerAddr));
			pszServerIP=szServerAddr;
		}
	}

	//连接服务器
	try
	{
		//连接服务器
		m_ClientSocket->CloseSocket();

		//代理判断
		if ( m_pDlgLogon->EnableProxy() == true )
		{
			//获取代理
			enProxyServerType ProxyServerType;
			tagProxyServerInfo ProxyServerInfo;
			m_pDlgLogon->GetProxyInfo(ProxyServerType, ProxyServerInfo);	

			//设置代理
			m_ClientSocket->SetProxyServerInfo(ProxyServerType,ProxyServerInfo);
		}

		if (m_ClientSocket->Connect(pszServerIP,PORT_LOGON_SERVER)!=CONNECT_SUCCESS)
		{
			throw TEXT("服务器连接错误，可能是您的系统还没有成功连接上网络！");
		}
	}
	catch (LPCTSTR pszError)
	{
		ShowMessageBox(pszError,MB_ICONINFORMATION);
		PostMessage(WM_COMMAND,IDM_LOGON_PLAZA,0);
		return;
	}

	//显示提示框
	/*cxf 1116 CString strMessage=TEXT("正在连接服务器，请稍候...");
	g_GlobalAttemper.ShowStatusMessage(strMessage,this);*/
	if (!m_bIsUpdateUserInfo  &&  !m_bCheckLine  && !m_bDeleteMachineCode)
	{
		m_GameFlash.Play(true, 0, 0);
	}

	return;
}
//取消连接
void CPlazaDlg::OnCommandCancelConnect()
{
	g_GlobalAttemper.DestroyStatusWnd(this);
	m_ClientSocket->CloseSocket();
	SendLogonMessage();
	return;
}

//接口查询
void * __cdecl CPlazaDlg::QueryInterface(const IID & Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IViewItem,Guid,dwQueryVer);
	QUERYINTERFACE(ITCPSocketSink,Guid,dwQueryVer);
	QUERYINTERFACE(IPlazaViewItem,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IViewItem,Guid,dwQueryVer);
	return NULL;
}


int CPlazaDlg::ShowMessageBox(LPCTSTR pszMessage, UINT nType)
{			
	//关闭提示
	g_GlobalAttemper.DestroyStatusWnd(this);
	m_GameFlash.Play(false);
	int nResult=ShowInformationEx(pszMessage,0,nType,TEXT("游戏大厅"));

	return nResult;
}

//登录消息
bool __cdecl CPlazaDlg::SendLogonMessage()
{
	m_bLogonPlaza=false;
	//((CGameFrame*)AfxGetMainWnd())->m_UserInfoView.ShowUserInfo(false);
	PostMessage(WM_COMMAND,IDM_LOGON_PLAZA,0);
	return true;
}

bool __cdecl CPlazaDlg::OnEventTCPSocketLink( WORD wSocketID, INT nErrorCode )
{
	//错误处理
	if (nErrorCode!=0)
	{
		//cxf 1116 g_GlobalAttemper.DestroyStatusWnd(this);
		m_GameFlash.Play(false);

		ShowMessageBox(TEXT("登录服务器连接失败，请稍后再试或留意网站公告！"),MB_ICONINFORMATION);
		SendLogonMessage();
		m_pDlgLogon->m_edUserAccounts.SetFocus();
		return true;
	}
	
	if (m_bIsUpdateUserInfo)
	{//点击更新按钮
		m_bIsUpdateUserInfo=false;
		BYTE cbBuffer[SOCKET_PACKET];
		tagGlobalUserData & UserData = g_GlobalUnits.GetGolbalUserData();

		//构造数据
		CMD_GP_UpdateUserInfo * pUpdateUserInfo=(CMD_GP_UpdateUserInfo *)cbBuffer;
		memset(pUpdateUserInfo,0,sizeof(CMD_GP_UpdateUserInfo));
		pUpdateUserInfo->dwUserID = UserData.dwUserID;
		CopyMemory(pUpdateUserInfo->szPassWord, UserData.szPassWord, sizeof(UserData.szPassWord));

		//发送数据包
		CSendPacketHelper Packet(cbBuffer+sizeof(CMD_GP_UpdateUserInfo),sizeof(cbBuffer)-sizeof(CMD_GP_UpdateUserInfo));
		m_ClientSocket.GetInterface()->SendData(MDM_GP_USER,SUB_GP_UPDATE_USERINFO,cbBuffer,sizeof(CMD_GP_UpdateUserInfo)+Packet.GetDataSize());

		return true;
	}
	
	if (m_bIsGetGlod)
	{//点击获取金币
		m_bIsGetGlod=false;
		BYTE cbBuffer[SOCKET_PACKET];
		tagGlobalUserData & UserData = g_GlobalUnits.GetGolbalUserData();

		//构造数据
		CMD_GP_GiftGold * pUpdateUserInfo=(CMD_GP_GiftGold *)cbBuffer;
		memset(pUpdateUserInfo,0,sizeof(CMD_GP_GiftGold));
		pUpdateUserInfo->dwUserID = UserData.dwUserID;
		CopyMemory(pUpdateUserInfo->szPassWord, UserData.szPassWord, sizeof(UserData.szPassWord));

		//发送数据包
		CSendPacketHelper Packet(cbBuffer+sizeof(CMD_GP_GiftGold),sizeof(cbBuffer)-sizeof(CMD_GP_GiftGold));
		m_ClientSocket.GetInterface()->SendData(MDM_GP_USER, SUB_GP_GET_GIFT_GOLD,cbBuffer,sizeof(CMD_GP_GiftGold)+Packet.GetDataSize());

		return true;
	}

	if (m_bCheckLine)
	{
		m_bCheckLine = false;

		BYTE cbBuffer[SOCKET_PACKET];
		tagGlobalUserData & UserData = g_GlobalUnits.GetGolbalUserData();

		//构造数据
		CMD_GP_CheckLine * pCheckLine=(CMD_GP_CheckLine *)cbBuffer;
		memset(pCheckLine,0,sizeof(CMD_GP_CheckLine));
		pCheckLine->dwUserID = UserData.dwUserID;

		//发送数据包
		CSendPacketHelper Packet(cbBuffer+sizeof(CMD_GP_CheckLine),sizeof(cbBuffer)-sizeof(CMD_GP_CheckLine));
		m_ClientSocket.GetInterface()->SendData(MDM_GP_USER, SUB_GP_CHECK_LINE,cbBuffer,sizeof(CMD_GP_CheckLine)+Packet.GetDataSize());
		return true;
	}

	if (m_bDeleteMachineCode)
	{
		BYTE cbBuffer[SOCKET_PACKET];
		m_bDeleteMachineCode=false;
		tagGlobalUserData & UserData = g_GlobalUnits.GetGolbalUserData();

		CMD_GP_DeleteMachineCode * pDeleteMachinCode=(CMD_GP_DeleteMachineCode *)cbBuffer;
		memset(pDeleteMachinCode,0,sizeof(CMD_GP_CheckLine));
		lstrcpy(pDeleteMachinCode->szAccounts, UserData.szAccounts);
		
		//机器序列号
		g_GlobalUnits.GetClientSerial(pDeleteMachinCode->clientSerial);

		CSendPacketHelper Packet(cbBuffer+sizeof(CMD_GP_CheckLine),sizeof(cbBuffer)-sizeof(CMD_GP_CheckLine));
		m_ClientSocket.GetInterface()->SendData(MDM_GP_USER, SUB_GP_DELETE_MACHINE_CODE, &cbBuffer, sizeof(CMD_GP_DeleteMachineCode));
		return true;
	}
	 
	//发送登录包
	if (m_bLogonPlaza==false)
	{
		//cxf 1022 m_ShowGameMsg.ClearAllMsg();
		/*cxf m_bIsFlashMsg = false;
		m_dwGameMsgIndex = 0;*/
		m_pDlgLogon->SendLogonPacket(m_ClientSocket.GetInterface());
		//cxf 1116 g_GlobalAttemper.ShowStatusMessage(TEXT("正在验证用户登录信息..."),this);
	}

	//自定义头像
	//	CGameFrame *pGameFrame = (CGameFrame *)AfxGetMainWnd() ;
	//	CDlgCustomFace &DlgCustomFace = pGameFrame->m_DlgCustomFace;
	//	if ( DlgCustomFace.m_hWnd != NULL && DlgCustomFace.IsWindowVisible() )
	//	{
	//		DlgCustomFace.SendData();
	//	}

	//下载判断
	//if ( m_bDownloadConnect )
	//{
	//	//设置变量
	//	m_bDownloadConnect = false;

	//	//投递请求
	//	tagDownloadInfo &DownloadInfo = m_DownloadInfoArrary[0];

	//	CMD_GP_DownloadFace DownloadFace;
	//	DownloadFace.dwUserID = DownloadInfo.dwUserID;
	//	m_ClientSocket->SendData(MDM_GP_USER, SUB_GP_USER_DOWNLOAD_FACE, &DownloadFace, sizeof(DownloadFace));
	//}
	return true;
}

bool __cdecl CPlazaDlg::OnEventTCPSocketShut( WORD wSocketID, BYTE cbShutReason )
{
	m_bIsUpdateUserInfo=false;
	m_bIsGetGlod=false;
	m_bCheckLine=false;
	m_bDeleteMachineCode=false;

	if (m_bLogonPlaza==false)
	{
		if (cbShutReason!=SHUT_REASON_NORMAL)
		{
			//cxf 1116 g_GlobalAttemper.DestroyStatusWnd(this);
			PostMessage(WM_COMMAND,IDM_LOGON_PLAZA,0);
			/*
			ShowMessageBox(TEXT("登录服务器连接失败，请稍后再试或留意网站公告！"),MB_ICONINFORMATION);
			SendLogonMessage();*/

			return true;
		}
	}
	m_GameFlash.Play(false);

	//自定义头像
	//	CGameFrame *pGameFrame = (CGameFrame *)AfxGetMainWnd() ;
	//	CDlgCustomFace &DlgCustomFace = pGameFrame->m_DlgCustomFace;
	//	if ( DlgCustomFace.m_hWnd != NULL ) DlgCustomFace.SetStatus(enOperateStatus_NULL);

	//释放内存

	//	if ( m_CustomFace.pFaceData != NULL ) m_CustomFace.Clear();

	return true;
}

bool __cdecl CPlazaDlg::OnEventTCPSocketRead( WORD wSocketID, CMD_Command Command, VOID * pData, WORD wDataSize )
{
	switch (Command.wMainCmdID)
	{
	case MDM_GP_LOGON:				//登录消息
		{
			return OnSocketMainLogon(Command,pData,wDataSize);		//用户验证、登录成功与否信息
		}
	case MDM_GP_SERVER_LIST:		//列表消息
		{
			return OnSocketMainServerList(Command,pData,wDataSize);	//下载游戏种类列表
		}
	case MDM_GP_SYSTEM:				//系统消息
		{
			return OnSocketMainSystem(Command,pData,wDataSize);		//更新游戏客户端版本下载信息
		}
	case MDM_GP_USER:				//用户消息
		{
			return OnSocketMainUser(Command,pData,wDataSize);		//用户头像下载信息
		}
	}

	return true;
}

bool __cdecl CPlazaDlg::SendConnectMessage()
{
	PostMessage(WM_COMMAND,IDM_CONNECT_SERVER,0);
	return true;
}

bool CPlazaDlg::OnSocketMainLogon(CMD_Command Command, void * pData, WORD wDataSize)
{
	switch (Command.wSubCmdID)
	{
	case SUB_GP_LOGON_SUCCESS:		//登录成功
		{
			//效验参数
			ASSERT(wDataSize>=sizeof(CMD_GP_LogonSuccess));
			if (wDataSize<sizeof(CMD_GP_LogonSuccess))
			{
				return false;
			}
			ShowWindow(SW_HIDE);
			//关闭提示
			//g_GlobalAttemper.DestroyStatusWnd(this);

			//保存信息
			int iSize = sizeof(CMD_GP_LogonSuccess);

			CMD_GP_LogonSuccess * pLogonSuccess=(CMD_GP_LogonSuccess *)pData;
			if (NULL == pLogonSuccess)
			{
				return false;
			}
			if (ANDROID == pLogonSuccess->utIsAndroid)
			{//机器人
				m_ClientSocket->CloseSocket();
				ShowMessageBox(TEXT("您的账号不存在或者密码输入有误,请查证后再次登录!"),MB_ICONQUESTION);
				m_pDlgLogon->m_edUserAccounts.SetFocus();
				////隐藏窗口
				//SetAllWndShow(false);

				//KillTimer(TIME_SET_DLG_ALPHA_INTERVAL);
				//m_uiCurAlphaValue = 0;
				//m_hSaveNewCurWnd = NULL;
				//m_hSaveOldCurWnd = NULL;
				//m_byCurWndTag = 0;

				//CRect rect;
				//GetWindowRect(&rect);
				//::SetWindowPos(m_pDlgLogon->GetSafeHwnd(), 0, rect.left, rect.top, rect.Width(), rect.Height(), SWP_SHOWWINDOW);
				//m_pDlgLogon->OnPopRegisterDlg();
				return true;
			}
			//cxf 1116 g_GlobalAttemper.ShowStatusMessage(TEXT("正在登录游戏大厅..."),this);
			tagGlobalUserData & UserData=g_GlobalUnits.GetGolbalUserData();
			UserData.wFaceID=pLogonSuccess->wFaceID;
			UserData.cbGender=pLogonSuccess->cbGender;
			UserData.cbMember=pLogonSuccess->cbMember;
			UserData.dwUserID=pLogonSuccess->dwUserID;
			UserData.dwGameID=pLogonSuccess->dwGameID;
			UserData.dwExperience=pLogonSuccess->dwExperience;
			UserData.dwCustomFaceVer=pLogonSuccess->dwCustomFaceVer;
			UserData.dwLockServerID=pLogonSuccess->dwLockServerID;
			::memset(UserData.szNickName, 0, sizeof(UserData.szNickName));
			::memcpy(UserData.szNickName, pLogonSuccess->szNickName, strlen(pLogonSuccess->szNickName));
			UserData.lGold = pLogonSuccess->lGold;
			UserData.dwGrade = pLogonSuccess->dwGrade;
			UserData.lGem = pLogonSuccess->lGem;
			UserData.lMoney = pLogonSuccess->lMoney;
			//UserData.lHonor = pLogonSuccess->lHonor;
			UserData.lWinCount = pLogonSuccess->lWinCount;
			UserData.lLostCount = pLogonSuccess->lLostCount;
			UserData.lDrawCount = pLogonSuccess->lDrawCount;
			UserData.lFleeCount = pLogonSuccess->lFleeCount;

			::memcpy(UserData.szKey, pLogonSuccess->szKey, sizeof(pLogonSuccess->szKey));//登陆网页Key
			UserData.wGiftCount=pLogonSuccess->lGiftNum;
			UserData.nMasterRight=pLogonSuccess->nMasterRight;//权限管理
			UserData.nMasterOrder=pLogonSuccess->nMasterOrder;//权限等级

			//扩展信息
			void * pDataBuffer=NULL;
			tagDataDescribe DataDescribe;
			CRecvPacketHelper RecvPacket(pLogonSuccess+1,wDataSize-sizeof(CMD_GP_LogonSuccess));
			while (true)
			{
				pDataBuffer=RecvPacket.GetData(DataDescribe);
				if (DataDescribe.wDataDescribe==DTP_NULL)
				{
					break;
				}

				switch (DataDescribe.wDataDescribe)
				{
				case DTP_USER_ACCOUNTS:		//用户帐户
					{
						ASSERT(pDataBuffer!=NULL);
						ASSERT(DataDescribe.wDataSize>0);
						ASSERT(DataDescribe.wDataSize<=sizeof(UserData.szAccounts));
						if (DataDescribe.wDataSize<=sizeof(UserData.szAccounts))
						{
							CopyMemory(UserData.szAccounts,pDataBuffer,DataDescribe.wDataSize);
							UserData.szAccounts[CountArray(UserData.szAccounts)-1]=0;
						}
						break;
					}
				case DTP_USER_PASS:			//用户密码
					{
						ASSERT(pDataBuffer!=NULL);
						ASSERT(DataDescribe.wDataSize>0);
						ASSERT(DataDescribe.wDataSize<=sizeof(UserData.szPassWord));
						if (DataDescribe.wDataSize<=sizeof(UserData.szPassWord))
						{
							CopyMemory(UserData.szPassWord,pDataBuffer,DataDescribe.wDataSize);
							UserData.szPassWord[CountArray(UserData.szPassWord)-1]=0;
						}
						break;
					}
				case DTP_UNDER_WRITE:		//个性签名
					{
						ASSERT(pDataBuffer!=NULL);
						ASSERT(DataDescribe.wDataSize>0);
						ASSERT(DataDescribe.wDataSize<=sizeof(UserData.szUnderWrite));
						if (DataDescribe.wDataSize<=sizeof(UserData.szUnderWrite))
						{
							CopyMemory(UserData.szUnderWrite,pDataBuffer,DataDescribe.wDataSize);
							UserData.szUnderWrite[CountArray(UserData.szUnderWrite)-1]=0;
						}
						break;
					}
				case DTP_USER_GROUP_NAME:	//社团名字
					{
						ASSERT(pDataBuffer!=NULL);
						ASSERT(DataDescribe.wDataSize>0);
						ASSERT(DataDescribe.wDataSize<=sizeof(UserData.szGroupName));
						if (DataDescribe.wDataSize<=sizeof(UserData.szGroupName))
						{
							CopyMemory(UserData.szGroupName,pDataBuffer,DataDescribe.wDataSize);
							UserData.szGroupName[CountArray(UserData.szGroupName)-1]=0;
						}
						break;
					}
				case DTP_STATION_PAGE:		//游戏主站
					{
						ASSERT(pDataBuffer!=NULL);
						if (pDataBuffer!=NULL) 
						{
							g_GlobalUnits.SetStationPage((LPCTSTR)pDataBuffer);
							//m_pHtmlBrower->Navigate(g_GlobalUnits.GetStationPage());
						}
						break;
					}
				default: { ASSERT(FALSE); }
				}
			}

			//重置URL
			//cxf 1022 m_doteyBoxDlg.SetResetURL();
			//cxf 1022 m_TaskDlg.ResetDlgURL();
			//cxf 1022 m_shopDlg.SetResetURL();

			//m_bIsDrawBack = true;
			m_UserImage.DestroyImage();
			char chImageData[32] = {0};
			//cxf
			//::sprintf(chImageData, "PNG_USER_%d_IMAGE", UserData.wFaceID);
			//HINSTANCE hInstance = AfxGetInstanceHandle();
			//m_UserImage.LoadImage(hInstance, chImageData);
			CString szPath("");
			GetResPath("\\data\\image\\",szPath);
			if (0<UserData.wFaceID){
				::sprintf(chImageData, "Image_p_%d", UserData.wFaceID);
			}else{
				if (1==UserData.cbGender){
					::sprintf(chImageData, "Image_p_%d_w", UserData.wFaceID);
				}else{
					::sprintf(chImageData, "Image_p_%d_m", UserData.wFaceID);
				}
			}
			szPath.Append(chImageData);
			szPath.Append(TEXT(".png"));
			m_UserImage.LoadImage(szPath);

			//m_bIsFlashMsg = false;
			////注册表字符
			//CString strRegKey;
			//strRegKey.Format(TEXT("%s\\%ld"),REG_USER_INFO,UserData.dwUserID);
			////打开注册表
			//CRegKey RegPassWord;
			//RegPassWord.Open(HKEY_CURRENT_USER,strRegKey,KEY_READ);
			//if (RegPassWord!=NULL)
			//{
			//	if (RegPassWord.QueryValue(m_dwGameMsgIndex, TEXT("GameMsgIndex"))!=ERROR_SUCCESS)
			//	{
			//		m_dwGameMsgIndex = 0;
			//	}
			//}
			//else m_dwGameMsgIndex = 0;

			//SetResetAlphaValue(m_gameListDlg.GetSafeHwnd());

			//			CWnd * pMainWnd=((CGamePlazaApp*)AfxGetApp())->m_pMainWnd;
			//			pMainWnd->ShowWindow(SW_SHOW);

			m_bIsGameSoftUpdate = CheckGameSoftHash(pLogonSuccess->szHashID);
			if (2 != g_GlobalUnits.GetApplicationCmdLine().GetSize())
			{
				m_pDlgLogon->EnableControlWindow(TRUE);
			}
			return true;
		}
	case SUB_GP_LOGON_ERROR:		//登录失败
		{
			//效验参数
			CMD_GP_LogonError *pLogonError = (CMD_GP_LogonError *)pData;
			ASSERT(wDataSize>=(sizeof(CMD_GP_LogonError)-sizeof(pLogonError->szErrorDescribe)));
			if (wDataSize<(sizeof(CMD_GP_LogonError)-sizeof(pLogonError->szErrorDescribe))) return false;
			ShowWindow(SW_HIDE);
			//关闭连接
			//cxf 1116 g_GlobalAttemper.DestroyStatusWnd(this);
			m_GameFlash.Play(false);
			if (2 != g_GlobalUnits.GetApplicationCmdLine().GetSize())
			{
				m_pDlgLogon->EnableControlWindow(TRUE);
			}

			m_ClientSocket->CloseSocket();

			//显示消息
			WORD wDescribeSize=wDataSize-(sizeof(CMD_GP_LogonError)-sizeof(pLogonError->szErrorDescribe));
			if (wDescribeSize>0)
			{
				pLogonError->szErrorDescribe[wDescribeSize-1]=0;
				ShowMessageBox(pLogonError->szErrorDescribe,MB_ICONINFORMATION);
			}

			//发送登录
			SendLogonMessage();
			m_pDlgLogon->m_edUserAccounts.SetFocus();
			return true;
		}
	case SUB_GP_LOGON_USER_GRADE_INFO:
		{
			m_iGradeInfoDiv = wDataSize / sizeof(CMD_GP_UserGradeInfo);
			int mod = wDataSize % sizeof(CMD_GP_UserGradeInfo);

			if (mod != 0) return false;
			if (NULL != pData)
			{
				if (NULL != m_pGradeInfo)
				{
					delete m_pGradeInfo;
				}
				m_pGradeInfo = NULL;
				m_pGradeInfo = new CMD_GP_UserGradeInfo[m_iGradeInfoDiv];
				memcpy(m_pGradeInfo, pData, sizeof(CMD_GP_UserGradeInfo)*m_iGradeInfoDiv);
			}
			return true;
		}
	case SUB_GP_LOGON_FINISH:		//登录完成
		{
			m_GameFlash.Play(false);

			// 更新判断
			if (m_bIsGameSoftUpdate)
			{
				if (2 == g_GlobalUnits.GetApplicationCmdLine().GetSize())
				{
					ShowMessageBox("游戏尝试多次更新失败！请联系客服。", MB_ICONSTOP);
				}
				else
				{
					CallExternalUpdateProgram();
				}

				m_ClientSocket->CloseSocket();
				OnCancel();
				return true;
			}
			

			//SetAllWndAlpha(0);
			//SetAllWndShow(true);
			//cxf 1116 g_GlobalAttemper.DestroyStatusWnd(this);

			m_pDlgLogon->EnableControlWindow(TRUE);
			ShowWindow(SW_SHOW);

			//cxf m_gameListDlg.SetHtml();
			//cxf m_hSaveOldCurWnd = m_gameListDlg.GetSafeHwnd();
			//创建游戏列表 cxf
			m_GameListCenterDlg.SetGameList();

			//cxf m_pFunShowLayer(m_hSaveOldCurWnd, 0, 255, 2);
			m_curTab = 1;
			//cxf 1025 SetShowTab(1);
			//cxf m_gameListDlg.SetFocus();
			//cxf 1022 m_roomListDlg.SetResetHtml();
			m_bIsGamePlazyStatus = true;

			if (1 == m_bIsSound)
			{//开启声音
				SetTimer(IDI_TIMER_PLAY_BACK_MUSIC, TIME_PLAY_BACK_MUSIC_INTERVAL, NULL);
			}
			else
			{//禁止声音
				::PlaySound(NULL, AfxGetInstanceHandle(),SND_PURGE);
			}

			/*cxfif (m_bIsFlashMsg)
			{
			tagGlobalUserData & UserData=g_GlobalUnits.GetGolbalUserData();
			//构造信息
			TCHAR szBuffer[256];
			_snprintf(szBuffer,sizeof(szBuffer),TEXT("%s\\%ld"),REG_USER_INFO,UserData.dwUserID);
			//写入信息
			CRegKey RegUserInfo;
			if (RegUserInfo.Create(HKEY_CURRENT_USER,szBuffer)==ERROR_SUCCESS)
			{
			RegUserInfo.SetDWORDValue(TEXT("GameMsgIndex"),m_dwGameMsgIndex);
			}
			//设置定时器
			SetTimer(IDI_TIMER_GAME_MSG_FLASH, TIME_GAME_MSG_FLASH_INTERVAL, NULL);
			::PlaySound(TEXT("WAVE_NEW_MSG"), AfxGetInstanceHandle(),SND_ASYNC|SND_NODEFAULT|SND_RESOURCE);
			}
			else
			{
			m_bIsFlashMsg = true;
			}*/

			/*
			//展开类型
			INT_PTR nIndex=0;
			CListType * pListType=NULL;
			do
			{
			pListType=g_GlobalUnits.m_ServerListManager.EnumTypeItem(nIndex++);
			if (pListType==NULL) break;
			g_GlobalUnits.m_ServerListManager.ExpandListItem(pListType);
			} while (true);

			//展开列表
			nIndex=0;
			CListInside * pListInside=NULL;
			do
			{
			pListInside=g_GlobalUnits.m_ServerListManager.EnumInsideItem(nIndex++);
			if (pListInside==NULL) break;
			g_GlobalUnits.m_ServerListManager.ExpandListItem(pListInside);
			} while (true);
			*/


			//记录信息
			m_bLogonPlaza=true;
			m_pDlgLogon->OnLogonSuccess();
			//m_pHtmlBrower->EnableBrowser(true);

			//记录信息
			g_GlobalUnits.WriteUserCookie();

			//显示头像 TODO：
			//			((CGameFrame*)AfxGetMainWnd())->m_UserInfoView.ShowUserInfo(true);

			//自定义判断
			//tagGlobalUserData &GlobalUserData = g_GlobalUnits.GetGolbalUserData();
			//if ( GlobalUserData.dwCustomFaceVer!=0)
			//{
			//	//头像名称
			//	CString strDirName = CString(g_GlobalUnits.GetWorkDirectory()) + TEXT("\\CustomFace");
			//	CString strFileName;
			//	strFileName.Format(TEXT("\\%ld_%d.bmp"), GlobalUserData.dwUserID, GlobalUserData.dwCustomFaceVer);

			//	//读取文件
			//	CImage FaceImage;
			//	HRESULT hResult = FaceImage.Load(strDirName + strFileName);
			//	if (SUCCEEDED(hResult))
			//	{
			//		//关闭连接
			//		m_ClientSocket->CloseSocket();

			//		FaceImage.Destroy();
			//	}
			//	//下载头像
			//	else
			//	{
			//		PostMessage(WM_DOWN_LOAD_FACE, LPARAM(GlobalUserData.dwCustomFaceVer), WPARAM(GlobalUserData.dwUserID));
			//	}
			//}
			//else
			//{
			//关闭连接
			m_ClientSocket->CloseSocket();
			/*cxf HINSTANCE hInstance = AfxGetInstanceHandle();
			m_TitleButtonList[3].BTImage.DestroyImage();
			m_TitleButtonList[3].BTImage.LoadImage(hInstance, TEXT("BT_PNG_IMAGE_SHOP"));
			m_TitleButtonList[0].btStatus = 3;
			m_TitleButtonList[1].btStatus = 0;
			m_TitleButtonList[2].btStatus = 0;
			m_TitleButtonList[3].btStatus = 0;*/

			//}
			//关闭提示
			tagGlobalUserData & UserData=g_GlobalUnits.GetGolbalUserData();
			if (UserData.dwExperience<=0)
			{//新手用户
				m_bNewComerGuide=true;
				SetTimer(IDI_ITMER_MEWCOMER_GUIDE,350,NULL);
				DoModalNewComerGuide();
			}
			if (0 != UserData.dwLockServerID)
			{//直接登录房间
				OnFlashEnterRoom();
			}
			return true;
		}
		/*case SUB_GP_LOGON_USER_PLACARD:		//游戏消息
		{
		if (wDataSize > sizeof(CMD_GP_UserPlacard))
		{
		return false;
		}

		CMD_GP_UserPlacard* pGameMsg = (CMD_GP_UserPlacard*)pData;
		if (NULL == pGameMsg)
		{
		return false;
		}

		m_ShowGameMsg.InsertGameMsg(pGameMsg->szTitle, pGameMsg->szDate, pGameMsg->szContex);
		if (0 == m_dwGameMsgIndex)
		{
		m_bIsFlashMsg = false;
		//注册表字符
		CString strRegKey;
		strRegKey.Format(TEXT("%s\\%ld"),REG_USER_INFO,pGameMsg->dwUserId);
		//打开注册表
		CRegKey RegPassWord;
		RegPassWord.Open(HKEY_CURRENT_USER,strRegKey,KEY_READ);
		if (RegPassWord!=NULL)
		{
		if (RegPassWord.QueryValue(m_dwGameMsgIndex, TEXT("GameMsgIndex"))!=ERROR_SUCCESS)
		{
		m_dwGameMsgIndex = 0;
		}
		}
		else
		{
		m_dwGameMsgIndex = 0;
		}
		}
		if (pGameMsg->dwIndex > m_dwGameMsgIndex)
		{
		m_dwGameMsgIndex = pGameMsg->dwIndex;
		m_bIsFlashMsg = true;


		return true;
		}}*/
	}

	return true;
}


//直接进入房间
void CPlazaDlg::OnFlashEnterRoom()
{
	if (FALSE == IsCanIntervalConnect())
		return;

	tagGlobalUserData & UserData=g_GlobalUnits.GetGolbalUserData();
	INT_PTR nIndex=0;
	CListServer * pListServer = g_GlobalUnits.m_ServerListManager.SearchServerItem(UserData.dwLockServerID);
	WORD dwKind = pListServer->GetListKind()->m_GameKind.wKindID;
	if (0 != dwKind)
	{
		m_RoomListCenterDlg.m_wKindID = dwKind;
	}

	if (NULL != m_pRoomViewItemCreate)
	{
		SafeDelete(m_pRoomViewItemCreate);
	}
	//寻找房间
	CRoomViewItem * pRoomViewItem=m_pRoomViewItemCreate;
	//创建房间视图(桌子列表)
	try
	{
		pRoomViewItem=new CRoomViewItem;
		if (pRoomViewItem==NULL) 
		{
			return;
		}
		pRoomViewItem->Create(IDD_TABLE_LIST,this);//创建桌子列表对话框

		//pRoomViewItem->m_gameListManager = &m_gameListManager;//桌子列表
		if (pRoomViewItem->InitRoomViewItem(pListServer)==false) 
		{
			throw TEXT("游戏房间初始化失败！");
		}
	}
	catch (...) 
	{ 
		SafeDelete(pRoomViewItem); 
	}
	if (pRoomViewItem==NULL)
	{
		ShowMessageBox(TEXT("游戏房间创建失败"),MB_ICONQUESTION);
		PostMessage(WM_COMMAND,IDM_LOGON_PLAZA,0);
		return;
	}

	//连接游戏
	bool bSuccess=pRoomViewItem->ConnectGameServer();
	if (bSuccess==false || pRoomViewItem->GetLogonFull())//判断房间是否满人
	{
		pRoomViewItem->DestroyWindow();
		SafeDelete(pRoomViewItem);
		return;
	}

	//设置变量
	m_pRoomViewItemCreate=pRoomViewItem;
	CRect plazarect;
	GetWindowRect(&plazarect);
	::MoveWindow(m_pRoomViewItemCreate->GetSafeHwnd(), plazarect.left, plazarect.top, 960, 720, FALSE);
	//HWND hWnd=NULL;
	//hWnd=m_pRoomViewItemCreate->GetSafeHwnd();

	//设置提示
	CString strBuffer;
	strBuffer.Format(TEXT("正在连接房间，请稍候..."));
	g_GlobalAttemper.ShowStatusMessage(strBuffer,pRoomViewItem);

	/*cxf 1022 tagGlobalUserData & UserData=g_GlobalUnits.GetGolbalUserData();
	INT_PTR nIndex=0;
	CListServer * pListServer = g_GlobalUnits.m_ServerListManager.SearchServerItem(UserData.dwLockServerID);
	WORD dwKind = pListServer->GetListKind()->m_GameKind.wKindID;
	if (0 != dwKind)
	{
	m_roomListDlg.m_wKindID = dwKind;
	}

	//寻找房间
	CRoomViewItem * pRoomViewItem=m_pRoomViewItemCreate;
	//创建房间视图(桌子列表)
	try
	{
	pRoomViewItem=new CRoomViewItem;
	if (pRoomViewItem==NULL) 
	{
	return;
	}
	pRoomViewItem->Create(IDD_TABLE_LIST,this);//创建桌子列表对话框
	//pRoomViewItem->m_gameListManager = &m_gameListManager;//桌子列表
	if (pRoomViewItem->InitRoomViewItem(pListServer)==false) 
	{
	throw TEXT("游戏房间初始化失败！");
	}
	}
	catch (...) 
	{ 
	SafeDelete(pRoomViewItem); 
	}
	if (pRoomViewItem==NULL)
	{
	ShowMessageBox(TEXT("游戏房间创建失败"),MB_ICONQUESTION);
	PostMessage(WM_COMMAND,IDM_LOGON_PLAZA,0);
	return;
	}

	//连接游戏
	bool bSuccess=pRoomViewItem->ConnectGameServer();
	if (bSuccess==false || pRoomViewItem->GetLogonFull())//判断房间是否满人
	{
	pRoomViewItem->DestroyWindow();
	SafeDelete(pRoomViewItem);
	return;
	}

	//设置变量
	m_pRoomViewItemCreate=pRoomViewItem;

	::MoveWindow(m_pRoomViewItemCreate->GetSafeHwnd(),287,81, 659, 626, FALSE);
	ShowTab(3);
	//HWND hWnd=NULL;
	//hWnd=m_pRoomViewItemCreate->GetSafeHwnd();

	//设置提示
	CString strBuffer;
	strBuffer.Format(TEXT("正在连接房间，请稍候..."));
	g_GlobalAttemper.ShowStatusMessage(strBuffer,pRoomViewItem);
	*/
}


bool CPlazaDlg::OnSocketMainServerList(CMD_Command Command, void * pData, WORD wDataSize)
{
	ASSERT(Command.wMainCmdID==MDM_GP_SERVER_LIST);

	switch (Command.wSubCmdID)
	{
	case SUB_GP_LIST_TYPE:			//类型信息
		{
			//效验参数
			ASSERT(wDataSize%sizeof(tagGameType)==0);
			if (wDataSize%sizeof(tagGameType)!=0) return false;

			//处理消息
			tagGameType * pGameType=(tagGameType *)pData;
			WORD wItemCount=wDataSize/sizeof(tagGameType);
			//TODO:结构要改
			g_GlobalUnits.m_ServerListManager.InsertTypeItem(pGameType,wItemCount);

			return true;
		}
	case SUB_GP_LIST_KIND:			//种类消息
		{
			//效验参数
			ASSERT(wDataSize%sizeof(tagGameKind)==0);
			if (wDataSize%sizeof(tagGameKind)!=0) return false;

			//处理消息
			tagGameKind * pGameKind=(tagGameKind *)pData;
			WORD wItemCount=wDataSize/sizeof(tagGameKind);

			//TODO:结构要改 
			g_GlobalUnits.m_ServerListManager.InsertKindItem(pGameKind,wItemCount);


			CString	sFileName("");
			CString sFilePath("");
			CString sFieldName("");
			TCHAR tchReturnVal[MAX_PATH]=_T("");
			GetModuleFileName(NULL, sFileName.GetBufferSetLength(MAX_PATH), MAX_PATH);
			sFilePath=sFileName.Left(sFileName.ReverseFind('\\') + 1) + TEXT("config\\roomlist.ini");

			for (WORD i=0; i<wItemCount; i++)
			{
				tagGameKind * pSiSeQiuKind = (tagGameKind *)(pGameKind+i);
				if (NULL == pSiSeQiuKind)
					continue;

				if (SISEQIU_ID == pSiSeQiuKind->wKindID)
				{
					tagGameServer GameServer;
					GameServer.wKindID = SISEQIU_ID;
					GameServer.wServerID = SISEQIU_ID;
					GameServer.dwMaxCount = 400;
					GameServer.dwOnLineCount = 100;
					_snprintf(GameServer.szServerName, sizeof(GameServer.szServerName), _T("四色球房间"));
					int iEnable = GetPrivateProfileInt(TEXT("siseqiu"), TEXT("Enable"), 1, sFilePath);
					if (0 == iEnable)
						continue;

					GameServer.wSortID = GetPrivateProfileInt(TEXT("siseqiu"), TEXT("SortID"), 1, sFilePath);
					GameServer.lCellScore = GetPrivateProfileInt(TEXT("siseqiu"), TEXT("lCellScore"), 0, sFilePath);
					GameServer.lRestrictScore = GetPrivateProfileInt(TEXT("siseqiu"), TEXT("lRestrictScore"), 0, sFilePath);
					GameServer.lEnterMoney = GetPrivateProfileInt(TEXT("siseqiu"), TEXT("lEnterMoney"), 0, sFilePath);

					g_GlobalUnits.m_ServerListManager.InsertServerItem(&GameServer, 1);
				}
				else if (ACTIVE_ID == pSiSeQiuKind->wKindID)
				{
					int iIndex = 1;
					while (1)
					{
						tagGameServer GameServer;
						sFieldName.Format(_T("Active_%d"), iIndex);
						GameServer.wKindID = ACTIVE_ID;
						GameServer.wServerID = GetPrivateProfileInt(sFieldName, TEXT("ServerID"), 1, sFilePath);
						GameServer.wSortID = GetPrivateProfileInt(sFieldName, TEXT("SortID"), 1, sFilePath);
						GetPrivateProfileString(sFieldName, _T("Name"), _T(""), tchReturnVal, sizeof(tchReturnVal), sFilePath);

						if (0 >= lstrlen(tchReturnVal))
							break;

						g_GlobalUnits.m_ServerListManager.InsertServerItem(&GameServer, 1);
						iIndex++;
					}
				}
			}
			return true;
		}
	case SUB_GP_LIST_STATION:		//站点消息
		{
			//效验参数
			ASSERT(wDataSize%sizeof(tagGameStation)==0);
			if (wDataSize%sizeof(tagGameStation)!=0) return false;

			//处理消息
			tagGameStation * pGameStation=(tagGameStation *)pData;
			WORD wItemCount=wDataSize/sizeof(tagGameStation);
			//TODO:结构要改
			g_GlobalUnits.m_ServerListManager.InsertStationItem(pGameStation,wItemCount);

			return true;
		}
	case SUB_GP_LIST_SERVER:		//服务器房间
		{
			//效验参数
			ASSERT(wDataSize%sizeof(tagGameServer)==0);
			if (wDataSize%sizeof(tagGameServer)!=0) return false;

			//处理消息
			tagGameServer * pGameServer=(tagGameServer *)pData;
			WORD wItemCount=wDataSize/sizeof(tagGameServer);
			//TODO:结构要改
			g_GlobalUnits.m_ServerListManager.InsertServerItem(pGameServer,wItemCount);
			return true;
		}
	case SUB_GP_LIST_FINISH:		//列表发送完成
		{
			//更新人数
			INT_PTR nIndex=0;
			DWORD dwAllOnLineCount=0L;
			CListKind * pListKind=NULL;
			do
			{
				pListKind=g_GlobalUnits.m_ServerListManager.EnumKindItem(nIndex++);
				if (pListKind==NULL) break;
				dwAllOnLineCount+=pListKind->GetItemInfo()->dwOnLineCount;
			} while (true);
			g_GlobalUnits.m_ServerListManager.UpdateGameOnLineCount(dwAllOnLineCount);

			return true;
		}
	case SUB_GP_LIST_CONFIG:		//列表配置
		{
			//效验参数
			ASSERT(wDataSize%sizeof(CMD_GP_ListConfig)==0);
			if (wDataSize%sizeof(CMD_GP_ListConfig)!=0) return false;

			//处理消息
			CMD_GP_ListConfig * pListConfig=(CMD_GP_ListConfig *)pData;

			//			((CGameFrame*)AfxGetMainWnd())->m_ServerItemView.ShowOnLineCount(pListConfig->bShowOnLineCount?true:false);

			return true;
		}
	}

	return true;
}

bool CPlazaDlg::OnSocketMainSystem(CMD_Command Command, void * pData, WORD wDataSize)
{
	ASSERT(Command.wMainCmdID==MDM_GP_SYSTEM);
	switch (Command.wSubCmdID)
	{
	case SUB_GP_VERSION:			//版本信息
		{
			//效验参数
			ASSERT(wDataSize>=sizeof(CMD_GP_Version));
			if (wDataSize<sizeof(CMD_GP_Version)) return false;

			//消息处理
			CMD_GP_Version * pVersion=(CMD_GP_Version *)pData;
			if (NULL==pVersion)
			{
				return false;
			}
			//cxf
			m_bLogonPlaza = false;
			m_pDlgLogon->m_DlgLogonLogo->ShowWindow(SW_HIDE);
			m_pDlgLogon->ShowWindow(SW_HIDE);
			this->ShowWindow(SW_HIDE);
			//end
			//cxf 1116 g_GlobalAttemper.DestroyStatusWnd(this);
			m_GameFlash.Play(false);

			m_ClientSocket->CloseSocket();
			tagGlobalUserData & GlobalUserData=g_GlobalUnits.GetGolbalUserData();
			memset(&GlobalUserData,0,sizeof(GlobalUserData));
			g_GlobalAttemper.DownLoadClient(TEXT("GamePlaza"),0,true,2);


			//if (pVersion->bAllowConnect)
			//{
			//	TCHAR szMessage[]=TEXT("游戏大厅版本已经升级，现在的版本还可以继续使用，现在是否马上下载新版本？");

			//	int iResult=ShowInformationEx(szMessage,0,MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON1,TEXT("游戏大厅"));
			//	if (iResult==IDYES)
			//	{
			//		g_GlobalAttemper.DestroyStatusWnd(this);
			//		m_ClientSocket->CloseSocket();
			//		tagGlobalUserData & GlobalUserData=g_GlobalUnits.GetGolbalUserData();
			//		memset(&GlobalUserData,0,sizeof(GlobalUserData));
			//		g_GlobalAttemper.DownLoadClient(TEXT("GamePlaza"),0,true);
			//	}
			//}
			//else
			//{
			//	g_GlobalAttemper.DestroyStatusWnd(this);
			//	m_ClientSocket->CloseSocket();
			//	TCHAR szMessage[]=TEXT("游戏大厅版本已经升级，现在的版本不可以继续使用，现在是否马上下载新版本？");

			//	int iResult=ShowInformationEx(szMessage,0,MB_ICONSTOP|MB_YESNO|MB_DEFBUTTON1,TEXT("游戏大厅"));
			//	if(iResult != IDYES)
			//	{
			//		tagGlobalUserData & GlobalUserData=g_GlobalUnits.GetGolbalUserData();
			//		memset(&GlobalUserData,0,sizeof(GlobalUserData));
			//		AfxGetMainWnd()->PostMessage(WM_CLOSE);
			//	}
			//	else 
			//	{
			//		g_GlobalAttemper.DownLoadClient(TEXT("GamePlaza"),0,true);
			//	}
			//}

			return true;
		}
	}
	return true;
}

bool CPlazaDlg::OnSocketMainUser( CMD_Command Command, void * pBuffer, WORD wDataSize )
{
	ASSERT(Command.wMainCmdID == MDM_GP_USER);
	switch(Command.wSubCmdID)
	{
	case SUB_GP_USER_DOWNLOAD_FACE:			//下载头像
		{
			return true;

			//类型转换
			CMD_GP_DownloadFaceSuccess *pDownloadFaceSuccess = (CMD_GP_DownloadFaceSuccess*)pBuffer;

			//参数验证
			WORD wSendSize = WORD(pDownloadFaceSuccess->dwCurrentSize + sizeof(CMD_GP_DownloadFaceSuccess) - sizeof(pDownloadFaceSuccess->bFaceData));
			ASSERT(wDataSize == wSendSize);
			if ( wDataSize != wSendSize )
			{
				return false;
			}


			//第一次判断
			if ( m_CustomFace.pFaceData == NULL )
			{
				if ( m_CustomFace.pFaceData != NULL ) 
				{
					delete[] m_CustomFace.pFaceData;
				}
				m_CustomFace.pFaceData = new BYTE[pDownloadFaceSuccess->dwToltalSize];
				if ( m_CustomFace.pFaceData == NULL ) 
				{
					return true;
				}

				m_CustomFace.dwFaceSize = pDownloadFaceSuccess->dwToltalSize;
				m_CustomFace.dwUserID = pDownloadFaceSuccess->dwUserID;
			}

			//拷贝数据
			CopyMemory(m_CustomFace.pFaceData+m_CustomFace.dwCurrentSize, pDownloadFaceSuccess->bFaceData, pDownloadFaceSuccess->dwCurrentSize);
			m_CustomFace.dwCurrentSize += pDownloadFaceSuccess->dwCurrentSize;

			//下载完毕
			if ( m_CustomFace.dwFaceSize == m_CustomFace.dwCurrentSize )
			{
				tagDownloadInfo &DownloadInfo = m_DownloadInfoArrary[0];

				//创建目录
				CString strDirName = CString(g_GlobalUnits.GetWorkDirectory()) + TEXT("\\CustomFace");
				CreateDirectory(strDirName, NULL) ;

				//写入文件
				CFile fileFace;
				CString strZipFileName, strBmpFileName;
				strZipFileName.Format(TEXT("\\%ld_%d.zip"), DownloadInfo.dwUserID, DownloadInfo.bFaceVer);
				strBmpFileName.Format(TEXT("\\%ld_%d.bmp"), DownloadInfo.dwUserID, DownloadInfo.bFaceVer);
				if ( fileFace.Open(strDirName + strZipFileName, CFile::modeCreate|CFile::modeWrite|CFile::typeBinary) )
				{
					fileFace.Write(m_CustomFace.pFaceData, m_CustomFace.dwFaceSize);
					fileFace.Close();

					//解压文件
					CUnzip oUnzip( strDirName + strZipFileName );
					oUnzip.SwapSize( strDirName + strBmpFileName );

					//删除文件
					CFile::Remove(strDirName + strZipFileName);
				}

				//自己判断
				tagGlobalUserData &GlobalUserData = g_GlobalUnits.GetGolbalUserData();
				if ( GlobalUserData.dwUserID ==  DownloadInfo.dwUserID )
				{
					//更新界面
					//					((CGameFrame*)AfxGetMainWnd())->m_UserInfoView.UpdateUserInfo();
				}

				//重置变量
				m_CustomFace.Clear();

				//删除元素
				m_DownloadInfoArrary.RemoveAt(0);

				//继续判断
				if ( 0 < m_DownloadInfoArrary.GetCount() )
				{
					//状态判断
					if ( m_ClientSocket->GetSocketStatus() != SOCKET_STATUS_CONNECT )
					{
						//链接网络
						if ( ! ConnectServer() )
						{
							return true;
						}


						//设置标识
						m_bDownloadConnect = true;
					}

					//投递请求
					tagDownloadInfo &DownloadInfo = m_DownloadInfoArrary[0];

					CMD_GP_DownloadFace DownloadFace;
					DownloadFace.dwUserID = DownloadInfo.dwUserID;
					m_ClientSocket->SendData(MDM_GP_USER, SUB_GP_USER_DOWNLOAD_FACE, &DownloadFace, sizeof(DownloadFace));
				}
				else
				{
					//关闭链接
					m_ClientSocket->CloseSocket();

					//设置变量
					m_bStartDownloadFace=false;
				}
			}
			return true;
		}
	case SUB_GP_UPLOAD_FACE_RESULT:		//上传结果
		{
			return true;

			ASSERT(sizeof(CMD_GP_UploadFaceResult) == wDataSize);
			if ( sizeof(CMD_GP_UploadFaceResult) != wDataSize)
			{
				return true;
			}


			//显示消息
			CMD_GP_UploadFaceResult *pUploadFaceResult = (CMD_GP_UploadFaceResult*)pBuffer;
			ShowMessageBox(pUploadFaceResult->szDescribeMsg, MB_ICONINFORMATION);

			//成功判断
			if ( pUploadFaceResult->bOperateSuccess )
			{
				//头像目录
				CString strDirName = CString(g_GlobalUnits.GetWorkDirectory()) + TEXT("\\CustomFace");
				CString strOldFileName;
				strOldFileName = TEXT("\\MyFace.bmp");

				//命名文件
				tagGlobalUserData &GlobalUserData = g_GlobalUnits.GetGolbalUserData();
				CString strNemFileName;
				strNemFileName.Format(TEXT("\\%ld_%d.bmp"), GlobalUserData.dwUserID, pUploadFaceResult->dwFaceVer);

				//删除文件
				try
				{
					CFile::Remove(strDirName + strNemFileName);
				}
				catch(...){}

				//删除文件
				CString strOldFile;
				strOldFile.Format(TEXT("\\%ld_%ld.bmp"), GlobalUserData.dwUserID, GlobalUserData.dwCustomFaceVer);
				try
				{
					CFile::Remove(strDirName + strOldFile);
				}
				catch(...){}

				try
				{
					CFile::Rename(strDirName + strOldFileName, strDirName + strNemFileName);

					//设置版本号
					GlobalUserData.dwCustomFaceVer = pUploadFaceResult->dwFaceVer;
					ASSERT(GlobalUserData.dwCustomFaceVer!=0);

					//投递消息
					for ( INT nIndex = 0; nIndex < MAX_SERVER; nIndex++ ) 
					{
						//		CRoomViewItem *pRoomViewItem = ((CGameFrame*)AfxGetMainWnd())->m_pRoomViewItem[nIndex];
						//		if ( pRoomViewItem == NULL ) continue;
						//		pRoomViewItem->SendMessage(WM_UPDATE_FACE, GlobalUserData.dwCustomFaceVer, GlobalUserData.dwUserID);
					}
				}
				catch(...)
				{
					ShowMessageBox(TEXT("头像文件命名失败，需要重新登录才可以显示新头像！"), MB_ICONINFORMATION);
				}

				//更新界面
				//				((CGameFrame*)AfxGetMainWnd())->m_UserInfoView.UpdateUserInfo();

				//设置界面
				//	CGameFrame *pGameFrame = (CGameFrame *)AfxGetMainWnd() ;
				//	CDlgCustomFace &DlgCustomFace = pGameFrame->m_DlgCustomFace;
				//	if ( DlgCustomFace.m_hWnd != NULL && DlgCustomFace.IsWindowVisible() )
				{
					//		DlgCustomFace.UpdateControls();
					//		DlgCustomFace.SetFocus();
				}
			}

			//关闭链接
			m_ClientSocket->CloseSocket();

			return true;
		}
	case SUB_GP_DELETE_FACE_RESULT:		//删除结果
		{
			return true;

			ASSERT(sizeof(CMD_GP_DeleteFaceResult) == wDataSize);
			if ( sizeof(CMD_GP_DeleteFaceResult) != wDataSize)
			{
				return true;
			}


			//显示消息
			CMD_GP_DeleteFaceResult *pDeleteFaceResult = (CMD_GP_DeleteFaceResult*)pBuffer;
			ShowMessageBox(pDeleteFaceResult->szDescribeMsg, MB_ICONINFORMATION);

			//成功判断
			if ( pDeleteFaceResult->bOperateSuccess )
			{
				//获取信息
				tagGlobalUserData &GlobalUserData = g_GlobalUnits.GetGolbalUserData();

				//文件目录
				CString strDirName = CString(g_GlobalUnits.GetWorkDirectory()) + TEXT("\\CustomFace");

				//文件名字
				CString strFileName;
				strFileName.Format(TEXT("\\%ld_%d.bmp"), GlobalUserData.dwUserID, pDeleteFaceResult->dwFaceVer);

				//删除文件
				try
				{
					CFile::Remove(strDirName + strFileName);
				}
				catch(...){}

				//更新界面
				//				((CGameFrame*)AfxGetMainWnd())->m_UserInfoView.UpdateUserInfo();

				//设置界面
				//				CGameFrame *pGameFrame = (CGameFrame *)AfxGetMainWnd() ;
				//				CDlgCustomFace &DlgCustomFace = pGameFrame->m_DlgCustomFace;
				//				if ( DlgCustomFace.m_hWnd != NULL && DlgCustomFace.IsWindowVisible() )
				{
					//					DlgCustomFace.UpdateControls();
					//					DlgCustomFace.SetFocus();
				}

				//投递消息
				for ( INT nIndex = 0; nIndex < MAX_SERVER; nIndex++ ) 
				{
					//					CRoomViewItem *pRoomViewItem = ((CGameFrame*)AfxGetMainWnd())->m_pRoomViewItem[nIndex];
					//					if ( pRoomViewItem == NULL ) continue;
					//					pRoomViewItem->SendMessage(WM_UPDATE_FACE, GlobalUserData.dwCustomFaceVer, GlobalUserData.dwUserID);
				}
			}

			//关闭链接
			m_ClientSocket->CloseSocket();

			return true;
		}
	case SUB_GP_MODIFY_INDIVIDUAL_RESULT:	//修改结果
		{
			return true;
		}
	case SUB_GP_UPDATE_USERINFO_SUCC:		//更新用户信息
		{
			if (wDataSize != sizeof(CMD_GP_UserInfoSucc)) return true;

			CMD_GP_UserInfoSucc * pUserInfoSucc = (CMD_GP_UserInfoSucc *) pBuffer;
			if (NULL != pUserInfoSucc)
			{
				tagGlobalUserData & UserData = g_GlobalUnits.GetGolbalUserData();
				UserData.cbGender = pUserInfoSucc->cbGender;
				UserData.lGem = pUserInfoSucc->lGem;
				UserData.lMoney = pUserInfoSucc->lMoney;
				UserData.lGold = pUserInfoSucc->lGold;
				UserData.wFaceID = pUserInfoSucc->wFaceID;
				CopyMemory(UserData.szNickName, pUserInfoSucc->szNickName, sizeof(pUserInfoSucc->szNickName));

				m_UserImage.DestroyImage();
				char chImageData[32] = {0};
				CString szPath("");
				GetResPath("\\data\\image\\",szPath);
				if (0<UserData.wFaceID){
					::sprintf(chImageData, "Image_p_%d", UserData.wFaceID);
				}else{
					if (1==UserData.cbGender){
						::sprintf(chImageData, "Image_p_%d_w", UserData.wFaceID);
					}else{
						::sprintf(chImageData, "Image_p_%d_m", UserData.wFaceID);
					}
				}
				szPath.Append(chImageData);
				szPath.Append(TEXT(".png"));
				m_UserImage.LoadImage(szPath);
				//更新用户形象
				UpdateUserImage();
			}

			//关闭连接
			m_ClientSocket->CloseSocket();

			return true;
		}
	case SUB_GP_GET_GIFT_GOLD_RESULT:
		{
			if (wDataSize != sizeof(CMD_GP_GiftGoldResult)) return true;
			CMD_GP_GiftGoldResult * pGiftGoldResult = (CMD_GP_GiftGoldResult *) pBuffer;
			if (NULL != pGiftGoldResult)
			{
				if (1==pGiftGoldResult->lRet)
				{
					tagGlobalUserData & UserData = g_GlobalUnits.GetGolbalUserData();
					UserData.lGold = UserData.lGold+pGiftGoldResult->dwGold;
					UserData.wGiftCount++;
					//更新用户形象
					UpdateUserImage();
				}
				ShowMessageBox(TEXT(pGiftGoldResult->szDescribe),MB_ICONINFORMATION);
			}
			//关闭连接
			m_ClientSocket->CloseSocket();
			m_GameFlash.Play(false);
			return true;
		}
	case SUB_GP_CHECK_LINE:		// 检查线路
		{
			ASSERT(wDataSize == sizeof(CMD_GP_CheckLine));
			if (wDataSize != sizeof(CMD_GP_CheckLine)) 
				return true;

			KillTimer(TIME_WAIT_CHECK_LINE);
			CMD_GP_CheckLine * pCheckLine = (CMD_GP_CheckLine *) pBuffer;

			if (NULL != pCheckLine  &&  NULL != m_pDlgLogon  &&  NULL != m_pDlgLogon->GetSafeHwnd())
			{
				//转化地址
				TCHAR szClientIP[16]=TEXT("");
				BYTE * pClientIP=(BYTE *)&pCheckLine->dwClientIP;
				_snprintf(szClientIP,sizeof(szClientIP),TEXT("%d.%d.%d.%d"),pClientIP[0],pClientIP[1],pClientIP[2],pClientIP[3]);

				string strCountry, strLocation;
				m_IpFinder.GetAddressByIp(szClientIP, strCountry, strLocation);
				int iIpType = m_IpFinder.GetIpType(strLocation.c_str());
				m_pDlgLogon->SetCheckLineResult(iIpType);
			}

			//关闭连接
			m_ClientSocket->CloseSocket();
			return true;
		}
	case SUB_GP_DELETE_MACHINE_CODE:		// 删除机器码成功
		{
			//关闭连接
			m_ClientSocket->CloseSocket();
			// 成功删除，响应游戏退出
			KillTimer(IDI_TIMER_WAIT_END);
			SetTimer(IDI_TIMER_WAIT_END, 10, NULL);
			return true;
		}
	default:
		{
			ASSERT(FALSE);
			return false;
		}
	}
	return true;
}

bool CPlazaDlg::ConnectServer()
{
	//合法判断
	if ( m_ClientSocket.GetInterface() == NULL ) 
	{
		return false;
	}

	//地址解释
	CRegKey RegServer;
	LPCTSTR pszServerIP=m_pDlgLogon->GetLogonServer();
	TCHAR szRegServer[256]=TEXT(""),szServerAddr[64]=TEXT("");
	_snprintf(szRegServer,sizeof(szRegServer),TEXT("%s\\%s"),REG_LOGON_SERVER,pszServerIP);

	if (RegServer.Open(HKEY_CURRENT_USER,szRegServer,KEY_READ)==ERROR_SUCCESS)
	{
		TCHAR szReadData[1024]=TEXT("");
		DWORD dwReadData=0L,dwDataType=0L,dwDataSize=sizeof(szReadData);
		LONG lErrorCode=RegServer.QueryValue(TEXT("ServerAddr"),&dwDataType,szReadData,&dwDataSize);
		if (lErrorCode==ERROR_SUCCESS) 
		{
			CXOREncrypt::CrevasseData(szReadData,szServerAddr,sizeof(szServerAddr));
			pszServerIP=szServerAddr;
		}
	}

	//连接服务器
	try
	{
		//连接服务器
		m_ClientSocket->CloseSocket();
		if (m_ClientSocket->Connect(pszServerIP,PORT_LOGON_SERVER)!=ERROR_SUCCESS)
		{
			throw TEXT("服务器连接错误，可能是您的系统还没有成功连接上网络！");
		}
	}
	catch (LPCTSTR pszError)
	{
		UNREFERENCED_PARAMETER(pszError);
		return false;
	}

	return true;
}

void CPlazaDlg::OnCommandLogon()
{//直接返回到登录界面
	//m_pDlgLogon->ShowWindow(SW_SHOW);
	if (3 != m_curTab)
	{//可以直接返回到登录界面
		m_bLogonPlaza=false;
		ShowTab(0);
	}
	else
	{//已经在桌子列表当中了，则要请求服务器才能退回到登录界面.
		if (NULL != m_pRoomViewItemCreate && ::IsWindow(m_pRoomViewItemCreate->GetSafeHwnd()))
		{
			m_pRoomViewItemCreate->SendRequestCloseRoomMsg(0);
		}
	}
}

//返回到登录界面
void CPlazaDlg::OnBnExitLogoUI()
{
	tagGlobalUserData & GlobalUserData=g_GlobalUnits.GetGolbalUserData();
	//删除记录
	g_GlobalUnits.DeleteUserCookie();

	//设置变量
	memset(&GlobalUserData,0,sizeof(GlobalUserData));

	if (NULL != m_pRoomViewItemCreate)
	{
		m_pRoomViewItemCreate->ClearMemDC();
		delete m_pRoomViewItemCreate;
		m_pRoomViewItemCreate = NULL;
	}
	m_bLogonPlaza=false;
	ShowTab(0);
}

void CPlazaDlg::ShowTab( int nPage )
{
	//判断状态
	tagGlobalUserData & GlobalUserData=g_GlobalUnits.GetGolbalUserData();
	if (GlobalUserData.dwUserID==0L && nPage >= 1)
	{
		ShowMessageBox(TEXT("您还没有登录，请先登录游戏大厅！"),MB_ICONQUESTION);
		nPage=0;
	}

	HINSTANCE hInstance=AfxGetInstanceHandle();
	switch(nPage)
	{
	case 0:
		{//返回登录界面
			//m_TaskDlg.ShowWindow(SW_HIDE);
			//m_shopDlg.ShowWindow(SW_HIDE);
			//m_doteyBoxDlg.ShowWindow(SW_HIDE);
			//		m_pDlgLogon->ShowWindow(SW_SHOW);

			//隐藏窗口
			SetAllWndShow(false);

			//隐藏大厅
			ShowWindow(SW_HIDE);

			//显示并刷新
			m_pDlgLogon->ShowWindow(SW_SHOW);
			m_pDlgLogon->m_DlgLogonLogo->ShowWindow(SW_SHOW);
			//cxf 1025 m_pDlgLogon->InvalidateRectControl();

			KillTimer(IDI_TIMER_GAME_MSG_FLASH);
			KillTimer(TIME_SET_DLG_ALPHA_INTERVAL);
			m_uiCurAlphaValue = 0;
			m_hSaveNewCurWnd = NULL;
			m_hSaveOldCurWnd = NULL;
			m_byCurWndTag = 0;

			/*cxf CRect rect;
			GetWindowRect(&rect);
			::SetWindowPos(m_pDlgLogon->GetSafeHwnd(), 0, rect.left, rect.top, rect.Width(), rect.Height(), SWP_SHOWWINDOW);
			*/
			m_pDlgLogon->OnPopRegisterDlg();
			m_pDlgLogon->SetActiveWindow();
			//m_gameListDlg.ShowWindow(SW_HIDE);
			//m_roomListDlg.ShowWindow(SW_HIDE);
			//if (m_pRoomViewItemCreate && ::IsWindow(m_pRoomViewItemCreate->GetSafeHwnd()))
			//{
			//	::ShowWindow(m_pRoomViewItemCreate->GetSafeHwnd(), SW_HIDE);
			//}
			m_pDlgLogon->SetFocus();
			//cxf 1022 m_ShowGameMsg.ClearAllMsg();
			UpdateMusicSound(false);
		}
		break;
	case 1:
		{//游戏列表
			//cxf 1024 m_TitleButtonList[3].BTImage.DestroyImage();
			//cxf 1024 m_TitleButtonList[3].BTImage.LoadImage(hInstance, TEXT("BT_PNG_IMAGE_SHOP"));
			//cxf 1024 InvalidateRect(&m_TitleButtonList[3].rcRect);
			m_pDlgLogon->ShowWindow(SW_HIDE);
			m_pDlgLogon->m_keydlg.ShowWindow(SW_HIDE);
			//m_roomListDlg.ShowWindow(SW_HIDE);
			ShowWindow(SW_SHOW);
			//if (m_pRoomViewItemCreate && ::IsWindow(m_pRoomViewItemCreate->GetSafeHwnd()))
			//{
			//	::ShowWindow(m_pRoomViewItemCreate->GetSafeHwnd(), SW_HIDE);
			//}
			m_bIsGamePlazyStatus = true;
			//::SetWindowPos(m_gameListDlg.GetSafeHwnd(), HWND_BOTTOM, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_SHOWWINDOW);
			SetShowTab(1);
			//SetResetAlphaValue(m_gameListDlg.GetSafeHwnd());
		}
		break;
	case 2:	
		{//返回房间列表
			//cxf 1024 m_TitleButtonList[3].BTImage.DestroyImage();
			//cxf 1024 m_TitleButtonList[3].BTImage.LoadImage(hInstance, TEXT("BT_PNG_IMAGE_SHOP"));
			//cxf 1024 InvalidateRect(&m_TitleButtonList[3].rcRect);
			m_pDlgLogon->ShowWindow(SW_HIDE);
			//m_gameListDlg.ShowWindow(SW_HIDE);
			ShowWindow(SW_SHOW);
			//if (m_pRoomViewItemCreate && ::IsWindow(m_pRoomViewItemCreate->GetSafeHwnd()))
			//{ 
			//	::ShowWindow(m_pRoomViewItemCreate->GetSafeHwnd(), SW_HIDE);
			//}
			//::SetWindowPos(m_roomListDlg.GetSafeHwnd(), HWND_BOTTOM, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_SHOWWINDOW);
			SetShowTab(2);
			m_bIsGamePlazyStatus = true;
			//SetResetAlphaValue(m_roomListDlg.GetSafeHwnd());
		}
		break;
	case 3:
		{//返回桌子列表
			//m_TaskDlg.ShowWindow(SW_HIDE);
			//m_shopDlg.ShowWindow(SW_HIDE);
			//m_doteyBoxDlg.ShowWindow(SW_HIDE);
			if (m_pRoomViewItemCreate && ::IsWindow(m_pRoomViewItemCreate->GetSafeHwnd()))
			{
				//cxf 1024 m_TitleButtonList[3].BTImage.DestroyImage();
				//cxf 1024 m_TitleButtonList[3].BTImage.LoadImage(hInstance, TEXT("BT_PNG_IMAGE_HELP"));
				//cxf 1024 InvalidateRect(&m_TitleButtonList[3].rcRect);

				//CRect rect;
				//GetWindowRect(&rect);
				//m_pRoomViewItemCreate->SetWindowPos(NULL, rect.left+287,rect.top+81, 659, 626, 0);

				//m_roomListDlg.ShowWindow(SW_HIDE);
				//m_pDlgLogon->ShowWindow(SW_HIDE);
				//m_gameListDlg.ShowWindow(SW_HIDE);
				//::SetFocus(m_pRoomViewItemCreate->GetSafeHwnd());
				//::SetWindowPos(m_pRoomViewItemCreate->GetSafeHwnd(), HWND_BOTTOM, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_SHOWWINDOW);
				//SetResetAlphaValue(m_pRoomViewItemCreate->GetSafeHwnd());
				SetShowTab(4);
				ShowWindow(SW_SHOW);
			}
			m_bIsGamePlazyStatus = true;
		}
		break;
	}
	m_curTab = nPage;
	//cxf 1024 m_TitleButtonList[0].btStatus = 3;
	//cxf 1024 InvalidateRect(&m_TitleButtonList[0].rcRect);
	// 	m_btnBeautySalon.ShowWindow(nPage==1?SW_SHOW:SW_HIDE);
	// 	m_btnGameShop.ShowWindow(nPage==1?SW_SHOW:SW_HIDE);
}

void CPlazaDlg::OnCallRoomList()
{//点击进入单个游戏服务器
	if (m_RoomListCenterDlg.m_wKindID!=0) 
	{
		INT_PTR nIndex=0;
		CListServer * pListServer=NULL;
		WORD wSortID[50]={0},wSortCount=0;

		//清空数据
		m_RoomListCenterDlg.m_arrayGameRoomInfo.RemoveAll();
		do
		{ 
			pListServer=g_GlobalUnits.m_ServerListManager.EnumServerItem(nIndex++);
			if (pListServer==NULL)
			{
				//进入房间
				m_RoomListCenterDlg.EnterRoom();
				SetShowHideInfo(3);//显示隐藏窗口

				break;
			}
			if(pListServer->m_pListKind->m_GameKind.wKindID==m_RoomListCenterDlg.m_wKindID) 
			{
				bool bExist=false;
				for (int i=0;i<wSortCount;++i)
				{
					if (pListServer->m_GameServer.wSortID==wSortID[i])
					{//房间序号已经不存在
						bExist=true;
						break;
					}
				}
				if (!bExist)
				{//数组没有相同数据 添加房间类别序号
					wSortID[wSortCount++]=pListServer->m_GameServer.wSortID;

					//房间信息：
					CGameRoomInfo GameRoomInfo;
					GameRoomInfo.m_pListServer=pListServer;
					m_RoomListCenterDlg.m_arrayGameRoomInfo.Add(GameRoomInfo);

					ASSERT(pListServer);
				}
			}			 
		} while (true); 
		//显示返回/下一页按钮
		m_btnRollback.ShowWindow(SW_SHOW);
		m_btnNextPage.ShowWindow(SW_SHOW);
	}

	/*cxf
	if (m_roomListDlg.m_wKindID!=0) 
	{
	INT_PTR nIndex=0;		 
	CListKind * pListKind=NULL;
	CListServer * pListServer=NULL;

	//清空数据
	m_roomListDlg.m_gameListManager->m_arrayGameRoomInfo.RemoveAll();
	do
	{ 
	pListServer=g_GlobalUnits.m_ServerListManager.EnumServerItem(nIndex++);
	if (pListServer==NULL)
	{
	if(m_roomListDlg.m_gameListManager->m_arrayGameRoomInfo.GetCount() != 0)
	{
	//进入房间
	m_roomListDlg.EnterRoom();
	ShowTab(2);
	}
	break;
	}
	if(pListServer->m_pListKind->m_GameKind.wKindID==m_roomListDlg.m_wKindID) 
	{
	//房间信息：
	//pListKind->CListKind
	//pListKind->CListItem.m_ListItemArray

	CGameRoomInfo GameRoomInfo;
	GameRoomInfo.m_pListServer=pListServer;
	m_roomListDlg.m_gameListManager->m_arrayGameRoomInfo.Add(GameRoomInfo);

	ASSERT(pListServer);
	}			 
	//dwAllOnLineCount+=pListKind->GetItemInfo()->dwOnLineCount;
	} while (true); 
	}
	*/
}

void CPlazaDlg::OnCallGameList()
{//返回游戏大厅.即游戏列表
	ShowTab(1);

	//更新用户形象
	UpdateUserImage();
}
LRESULT CPlazaDlg::OnCallInnoIssMsgClose(WPARAM wParam, LPARAM lParam)
{
	OnCancel();

	return 1;
}

LRESULT CPlazaDlg::OnCallCheckConnect(WPARAM wParam, LPARAM lParam)
{
	if (1 == (int)wParam  &&  1==(int)lParam)
	{
		m_bCheckLine = true;
	}

	SetTimer(TIME_WAIT_CHECK_LINE, 5000, NULL);
	OnCommandConnect();
	return 1;
}

//Inno 安装脚本发来的消息 选择游戏
LRESULT CPlazaDlg::OnCallInnoIssMsgSelectGame(WPARAM wParam, LPARAM lParam)
{
	CListKind * pListKind=NULL;
	CListItem * pListItem=NULL;
	INT_PTR nListIndex=0;

	int iKind = (int)wParam;
	CString str;
	if ( m_RoomListCenterDlg.m_wKindID==iKind && m_RoomListCenterDlg.m_wSortID != -1) 
	{
		//房间列表
		CListServer * pListServer=NULL;
		//房间验证
		if(g_GlobalUnits.m_ServerListManager.EnumServerItem(0) != NULL)
		{
			//寻找房间
			do
			{
				pListServer=g_GlobalUnits.m_ServerListManager.EnumServerItem(nListIndex++);
				if(pListServer != NULL)
				{
					if(m_RoomListCenterDlg.m_wKindID == pListServer->m_pListKind->m_GameKind.wKindID)
					{	
						pListKind=pListServer->m_pListKind;
						//目标房间
						if(m_RoomListCenterDlg.m_wSortID != -1)
						{
							if(m_RoomListCenterDlg.m_wSortID == pListServer->m_GameServer.wSortID)
							{
								//房间人数
								if(pListServer->m_GameServer.dwMaxCount <= pListServer->m_GameServer.dwOnLineCount)
									continue;
								//进入房间
								if(pListServer != NULL && !pListKind->m_bInstall)
								{
									CWinFileInfo WinFileInfo;
									tagGameKind * pGameKind=pListKind->GetItemInfo();
									pListKind->m_bInstall=WinFileInfo.OpenWinFile(pGameKind->szProcessName);
								}
								break;
							}
						}
						if(pListServer->m_pListKind->GetItemCount() == 0)break;
					}
				}
				else
					break;
			}while(true);
		}
		OnCallTableList();
		return 1;
	}
	return 1;

	/*cxf 1022
	CListKind * pListKind=NULL;
	CListItem * pListItem=NULL;
	INT_PTR nListIndex=0;

	int iKind = (int)wParam;
	CString str;
	if ( m_roomListDlg.m_wKindID==iKind && m_roomListDlg.m_nRoomNum != -1) 
	{
	//房间列表
	CListServer * pListServer=NULL;
	//房间验证
	if(g_GlobalUnits.m_ServerListManager.EnumServerItem(0) != NULL)
	{
	//寻找房间
	do
	{
	pListServer=g_GlobalUnits.m_ServerListManager.EnumServerItem(nListIndex++);
	if(pListServer != NULL)
	{
	if(m_roomListDlg.m_wKindID == pListServer->m_pListKind->m_GameKind.wKindID)
	{	
	pListKind=pListServer->m_pListKind;
	//目标房间
	if(m_roomListDlg.m_nRoomNum != -1)
	{
	if(m_roomListDlg.m_nRoomNum == pListServer->m_GameServer.wSortID)
	{
	//房间人数
	if(pListServer->m_GameServer.dwMaxCount <= pListServer->m_GameServer.dwOnLineCount)
	continue;
	//进入房间
	if(pListServer != NULL && !pListKind->m_bInstall)
	{
	CWinFileInfo WinFileInfo;
	tagGameKind * pGameKind=pListKind->GetItemInfo();
	pListKind->m_bInstall=WinFileInfo.OpenWinFile(pGameKind->szProcessName);
	}
	break;
	}
	}
	if(pListServer->m_pListKind->GetItemCount() == 0)break;
	}
	}
	else
	break;
	}while(true);
	}
	OnCallTableList();
	return 1;
	}
	return 1;
	*/
}

void CPlazaDlg::OnCallTableList()
{//切换到桌子列表

	if (SISEQIU_ID == m_RoomListCenterDlg.m_wKindID)
	{
		if (!StartSiSeQiuProgram()) 
		{
			ShowMessageBox(_T("四色球启动失败！请联系管理员！"), MB_ICONINFORMATION);
		}
		return;
	}
	else if (ACTIVE_ID == m_RoomListCenterDlg.m_wKindID)
	{
		if (!StartActivity(m_RoomListCenterDlg.m_wSortID))
		{
			ShowMessageBox(_T("活动专区打开失败！请联系管理员！"), MB_ICONINFORMATION);
		}
		return;
	}


	CListKind * pListKind=NULL;
	CListItem * pListItem=NULL;
	INT_PTR nListIndex=0;
	bool bIsLogonSuc = false;

	if (m_RoomListCenterDlg.m_wKindID!=0)
	{
		//房间列表
		CListServer * pListServer=NULL;
		//房间验证
		if(g_GlobalUnits.m_ServerListManager.EnumServerItem(0) != NULL)
		{
			//检查游戏是否存在
			g_GlobalUnits.m_ServerListManager.UpdateGameKind(m_RoomListCenterDlg.m_wKindID);
			//寻找房间
			do
			{
				pListServer=g_GlobalUnits.m_ServerListManager.EnumServerItem(nListIndex++);
				if(pListServer != NULL)
				{
					if(m_RoomListCenterDlg.m_wKindID == pListServer->m_pListKind->m_GameKind.wKindID)
					{	
						pListKind=pListServer->m_pListKind;

						//目标房间
						if(m_RoomListCenterDlg.m_wSortID != -1)
						{
							if(m_RoomListCenterDlg.m_wSortID == pListServer->m_GameServer.wSortID)
							{
								//房间人数
								if((pListServer->m_GameServer.dwMaxCount <= pListServer->m_GameServer.dwOnLineCount)
									|| (pListServer->m_GameServer.dwOnLineCount >= pListServer->m_GameServer.dwMaxCount*DEF_NUM_VALUE))
								{
									//如果在线人数超过最大人数或者在线人数超过了最大人数的百分之95，则直接返回。
									continue;
								}

								//进入房间
								if(pListServer != NULL && pListKind->m_bInstall)
								{
									if(pListKind->m_GameKind.wKindID == pListServer->m_pListKind->m_GameKind.wKindID)
									{
										bIsLogonSuc = true;
										//在此判断个人的宝石够不够进入这个房间的最低限制
										if(CreateTableList(pListServer))
										{
											//cxf ShowTab(3);
											break;
										}
										else
										{
											return;
										}
									}
								}
								else
								{
									//变量定义
									tagGameKind * pGameKind=pListKind->GetItemInfo();

									//安装判断
									if ((!pListKind->m_bInstall)&&(pListKind->m_GameKind.dwMaxVersion!=0L))
									{
										TCHAR szProcessName[MODULE_LEN];
										CopyMemory(szProcessName, pGameKind->szProcessName, sizeof(szProcessName));
										TCHAR * pCh = strrchr(szProcessName, '.');
										if (pCh) *pCh = '\0';
										g_GlobalAttemper.DownLoadClient(szProcessName,pGameKind->wKindID,true,1);
										return;

										//构造提示
										TCHAR szBuffer[512]=TEXT("");
										_snprintf(szBuffer,sizeof(szBuffer),TEXT("【%s】还没有安装，现在是否下载？"),pGameKind->szKindName);
										//提示信息
										if (ShowMessageBox(szBuffer,MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON1)==IDYES)
										{
											TCHAR szProcessName[MODULE_LEN];
											CopyMemory(szProcessName, pGameKind->szProcessName, sizeof(szProcessName));
											TCHAR * pCh = strrchr(szProcessName, '.');
											if (pCh) *pCh = '\0';
											g_GlobalAttemper.DownLoadClient(szProcessName,pGameKind->wKindID,true,1);
											//g_GlobalAttemper.DownLoadClient(pGameKind->szKindName,pGameKind->wKindID,true);
										}
									}

									return;
								}
								break;
							}
						}

						if(pListServer->m_pListKind->GetItemCount() == 0)break;
					}
				}
				else
					break;
			}while(true);

			if (!bIsLogonSuc)
			{
				//提示:房间人数已满
				char szBuffer[100]={0};
				sprintf(szBuffer, "此房间人数已满，请选择其它房间或游戏！");
				ShowMessageBox(szBuffer, MB_ICONINFORMATION);
			}
		}

		/* cxf 
		//变量定义
		CListKind * pListKind=NULL;
		CListItem * pListItem=NULL;
		INT_PTR nListIndex=0;
		bool bIsLogonSuc = false;

		if (m_roomListDlg.m_wKindID!=0)
		{
		//房间列表
		CListServer * pListServer=NULL;
		//房间验证
		if(g_GlobalUnits.m_ServerListManager.EnumServerItem(0) != NULL)
		{
		//寻找房间
		do
		{
		pListServer=g_GlobalUnits.m_ServerListManager.EnumServerItem(nListIndex++);
		if(pListServer != NULL)
		{
		if(m_roomListDlg.m_wKindID == pListServer->m_pListKind->m_GameKind.wKindID)
		{	
		pListKind=pListServer->m_pListKind;

		//目标房间
		if(m_roomListDlg.m_nRoomNum != -1)
		{
		if(m_roomListDlg.m_nRoomNum == pListServer->m_GameServer.wSortID)
		{
		//房间人数
		if((pListServer->m_GameServer.dwMaxCount <= pListServer->m_GameServer.dwOnLineCount)
		|| (pListServer->m_GameServer.dwOnLineCount >= pListServer->m_GameServer.dwMaxCount*DEF_NUM_VALUE))
		{
		//如果在线人数超过最大人数或者在线人数超过了最大人数的百分之95，则直接返回。
		continue;
		}

		//进入房间
		if(pListServer != NULL && pListKind->m_bInstall)
		{
		if(pListKind->m_GameKind.wKindID == pListServer->m_pListKind->m_GameKind.wKindID)
		{
		bIsLogonSuc = true;
		//在此判断个人的宝石够不够进入这个房间的最低限制
		if(CreateTableList(pListServer))
		{
		ShowTab(3);
		break;
		}
		else
		{
		return;
		}
		}
		}
		else
		{
		//变量定义
		tagGameKind * pGameKind=pListKind->GetItemInfo();

		//安装判断
		if ((!pListKind->m_bInstall)&&(pListKind->m_GameKind.dwMaxVersion!=0L))
		{
		TCHAR szProcessName[MODULE_LEN];
		CopyMemory(szProcessName, pGameKind->szProcessName, sizeof(szProcessName));
		TCHAR * pCh = strrchr(szProcessName, '.');
		if (pCh) *pCh = '\0';
		g_GlobalAttemper.DownLoadClient(szProcessName,pGameKind->wKindID,true);
		return;

		//构造提示
		TCHAR szBuffer[512]=TEXT("");
		_snprintf(szBuffer,sizeof(szBuffer),TEXT("【%s】还没有安装，现在是否下载？"),pGameKind->szKindName);
		//提示信息
		if (ShowMessageBox(szBuffer,MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON1)==IDYES)
		{
		TCHAR szProcessName[MODULE_LEN];
		CopyMemory(szProcessName, pGameKind->szProcessName, sizeof(szProcessName));
		TCHAR * pCh = strrchr(szProcessName, '.');
		if (pCh) *pCh = '\0';
		g_GlobalAttemper.DownLoadClient(szProcessName,pGameKind->wKindID,true);
		//g_GlobalAttemper.DownLoadClient(pGameKind->szKindName,pGameKind->wKindID,true);
		}
		}

		return;
		}
		break;
		}
		}

		if(pListServer->m_pListKind->GetItemCount() == 0)break;
		}
		}
		else
		break;
		}while(true);

		if (!bIsLogonSuc)
		{
		//提示:房间人数已满
		char szBuffer[100]={0};
		sprintf(szBuffer, "此房间人数已满，请选择其它房间或游戏！");
		ShowMessageBox(szBuffer, MB_ICONINFORMATION);
		}
		}
		*/

		////进入房间
		//if(pListServer != NULL && pListKind->m_bInstall)
		//{
		//	if(pListKind->m_GameKind.wKindID == pListServer->m_pListKind->m_GameKind.wKindID)
		//	{
		//		if(CreateTableList(pListServer))
		//			ShowTab(3);
		//	}
		//}
		//else
		//	{
		//		//变量定义
		//		tagGameKind * pGameKind=pListKind->GetItemInfo();

		//		//安装判断
		//		if ((pListKind->m_bInstall==false)&&(pListKind->m_GameKind.dwMaxVersion!=0L))
		//		{
		//			//构造提示
		//			TCHAR szBuffer[512]=TEXT("");
		//			_snprintf(szBuffer,sizeof(szBuffer),TEXT("【%s】还没有安装，现在是否下载？"),pGameKind->szKindName);

		//			//提示信息
		//			if (ShowMessageBox(szBuffer,MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON1)==IDYES)
		//			{
		//				g_GlobalAttemper.DownLoadClient(pGameKind->szKindName,pGameKind->wKindID,true);
		//			}
		//		}

		//		return;
		//	}
	}
}

LRESULT CPlazaDlg::OnCallShowCustomer(WPARAM wParam, LPARAM lParam)
{
	CMD_GR_Message * pMessage=(CMD_GR_Message *)wParam;
	TCHAR szContent[1024] = _T("");
	lstrcpy(szContent, pMessage->szContent);

	if (SMT_CUSTOMER_FRAME&pMessage->wMessageType)
	{
		int iResult = ShowMessageBox(szContent, MB_ICONINFORMATION|MB_CUSTOMER);
		if (IDOK == iResult)
		{
			ShellExecute(NULL,"open",CInitData::s_cContactService,"",NULL,SW_SHOW);
		}
	}
	else
	{
		ShowMessageBox(szContent, MB_ICONINFORMATION|MB_OK);
	}

	return 1;
}

int CPlazaDlg::OnCreate( LPCREATESTRUCT lpCreateStruct )
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	//m_ServerItemView.Create(WS_VISIBLE|WS_CHILD,CRect(0,0,0,0),this,IDC_SERVER_ITEM_VIEW);
	//m_ServerItemView.InitServerItemView(/*this*/);
	//g_GlobalUnits.m_ServerListManager.InitServerListManager(&m_ServerItemView);
	return 0;
}

BOOL CPlazaDlg::OnEraseBkgnd(CDC * pDC)
{
	return TRUE;
	/*cxf 1024	if (m_bIsDrawBack)
	{
	CRect rtClientRect;
	GetClientRect(&rtClientRect);
	CDC pMemDC;
	pMemDC.CreateCompatibleDC(pDC);
	CBitmap hBitmap;
	hBitmap.CreateCompatibleBitmap(pDC, rtClientRect.Width(), rtClientRect.Height());
	pMemDC.SelectObject(&hBitmap);
	pMemDC.SetBkMode(TRANSPARENT);
	pMemDC.FillSolidRect(0,0,960, 720,RGB(0,0,0));

	//CImageHandle handle(&m_ImageBack);
	//m_ImageBack.AlphaDrawImage(&pMemDC, 0, 0, RGB(255, 0, 255));
	if (!m_ImageBack.IsNull())
	{
	m_ImageBack.DrawImage(&pMemDC,0,0);
	}

	pDC->BitBlt(287,81, 659, 626, &pMemDC, 287,81, SRCCOPY);
	DeleteObject(&hBitmap);
	pMemDC.DeleteDC();
	}
	*/
}

//位置消息
void CPlazaDlg::OnSize(UINT nType, int cx, int cy)
{
	__super::OnSize(nType, cx, cy);

	m_bIsShowXJInfo = false;
	CPaintDC dc(this);
	m_memdc.DeleteDC();
	CBitmap bmp;
	m_memdc.CreateCompatibleDC(&dc);
	bmp.CreateCompatibleBitmap(&dc, cx, cy);
	m_memdc.SelectObject(bmp);
	InvalidateRect(NULL);
	bmp.DeleteObject();
	return;
}


void CPlazaDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	__super::OnMouseMove(nFlags, point);

	if (m_UserXJRect.PtInRect(point))
	{
		if (!m_bIsShowXJInfo)
		{
			m_bIsShowXJInfo = true;
			InvalidateRect(&m_UserXJInfoRect);
		}
	}
	else
	{
		if (m_bIsShowXJInfo)
		{
			InvalidateRect(&m_UserXJInfoRect);
		}
		m_bIsShowXJInfo = false;
	}
	/*cxf 
	if (m_UserXJRect.PtInRect(point))
	{
	WORD wClass = 0;
	tagGlobalUserData & UserData=g_GlobalUnits.GetGolbalUserData();
	CountXJClass(UserData.lWinCount, (UserData.lWinCount+UserData.lLostCount+UserData.lDrawCount+UserData.lFleeCount), wClass);
	wClass =1 ;
	if (wClass>=1 && wClass<=5)
	{
	m_bIsShowXJInfo = true;
	InvalidateRect(&m_UserXJInfoRect);
	}
	}
	else
	{
	if (m_bIsShowXJInfo)
	{
	InvalidateRect(&m_UserXJInfoRect);
	}
	m_bIsShowXJInfo = false;
	}

	if (m_rcGameMsgArea.PtInRect(point))
	{
	m_byFlashMsgStatus = 0;
	if (m_bIsFlashMsg)
	{
	m_GameMsgImage.DestroyImage();
	HINSTANCE hInstance = AfxGetInstanceHandle();
	m_GameMsgImage.LoadImage(hInstance, TEXT("PNG_GAME_MSG_MSG_MOVE_BACK"));
	m_byFlashMsgStatus = 1;
	}
	InvalidateRect(&m_rcGameMsgArea);
	}
	else
	{
	if (0 != m_byFlashMsgStatus)
	{
	m_GameMsgImage.DestroyImage();
	HINSTANCE hInstance = AfxGetInstanceHandle();
	m_GameMsgImage.LoadImage(hInstance, TEXT("PNG_IMAGE_GAME_MSG"));
	m_byFlashMsgStatus = 0;
	InvalidateRect(&m_rcGameMsgArea);
	}
	}*/
	/*cxf 1024
	BYTE bySaveIndex = 255;
	for (int i=0; i<4; i++)
	{
	if (m_TitleButtonList[i].rcRect.PtInRect(point))
	{
	if (3 != m_TitleButtonList[i].btStatus)
	{
	bySaveIndex = i;
	break;
	}
	}
	else
	{
	if (3 != m_TitleButtonList[i].btStatus)
	{
	if (0 != m_TitleButtonList[i].btStatus)
	{
	m_TitleButtonList[i].btStatus = 0;
	InvalidateRect(&m_TitleButtonList[i].rcRect);
	}
	}
	}
	}
	if (bySaveIndex>=0 && bySaveIndex<4)
	{
	for (int i=0; i<4; i++)
	{
	if (3 != m_TitleButtonList[i].btStatus)
	{
	if (i==bySaveIndex)
	{
	if (2!=m_TitleButtonList[i].btStatus)
	{
	if (1 != m_TitleButtonList[i].btStatus)
	{
	m_TitleButtonList[i].btStatus = 1;
	InvalidateRect(&m_TitleButtonList[i].rcRect);
	}
	}
	}
	else
	{
	if (0 != m_TitleButtonList[i].btStatus)
	{
	m_TitleButtonList[i].btStatus = 0;
	InvalidateRect(&m_TitleButtonList[i].rcRect);
	}
	}
	}
	}
	}
	*/
}

void CPlazaDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (m_bNewComerGuide && m_rcNewComerGuideArea.PtInRect(point))
	{//新手指引
		OnBnClickedNewComerGuide();
		return;
	}
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	/*cxf  1024
	BYTE bySaveIndex = 255;
	bool bIsMove = true;
	for (int i=0; i<4; i++)
	{
	if (m_TitleButtonList[i].rcRect.PtInRect(point))
	{
	bIsMove = false;
	if (3 != m_TitleButtonList[i].btStatus)
	{
	bySaveIndex = i;
	break;
	}
	}
	}
	if (bySaveIndex>=0 && bySaveIndex<4)
	{
	for (int i=0; i<4; i++)
	{
	if (3 != m_TitleButtonList[i].btStatus)
	{
	m_TitleButtonList[i].btStatus = (i==bySaveIndex)?2:0;
	InvalidateRect(&m_TitleButtonList[i].rcRect);
	}
	}
	}

	if (bIsMove)
	{
	if (point.y < 80)
	{
	PostMessage(WM_NCLBUTTONDOWN,HTCAPTION,MAKELPARAM(point.x,point.y));
	}
	}
	*/
	PostMessage(WM_NCLBUTTONDOWN,HTCAPTION,MAKELPARAM(point.x,point.y));

	/*cxf

	if (m_rcGameMsgArea.PtInRect(point))
	{
	KillTimer(IDI_TIMER_GAME_MSG_FLASH);
	m_bIsFlashMsg = true;
	InvalidateRect(&m_rcGameMsgArea);
	m_ShowGameMsg.DoModal();
	}*/

	__super::OnLButtonDown(nFlags, point);
}

void CPlazaDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	__super::OnLButtonUp(nFlags, point);

	/*cxf BYTE bySaveIndex = 255;
	for (int i=0; i<4; i++)
	{
	if (m_TitleButtonList[i].rcRect.PtInRect(point))
	{
	if (2 == m_TitleButtonList[i].btStatus)
	{
	bySaveIndex = i;
	break;
	}
	}
	}
	if (bySaveIndex>=0 && bySaveIndex<4)
	{
	for (int i=0; i<4; i++)
	{
	if (i==bySaveIndex)
	{
	if (3 != m_TitleButtonList[i].btStatus)
	{
	m_TitleButtonList[i].btStatus = 3;
	ExeButtonEvent(i);
	}
	}
	else
	{
	m_TitleButtonList[i].btStatus = 0;
	}
	InvalidateRect(&m_TitleButtonList[i].rcRect);
	}
	}
	*/

	
}

//连接桌子
bool CPlazaDlg::CreateTableList(CListServer * pListServer)
{
	if (FALSE == IsCanIntervalConnect())
		return false;

	//效验参数
	ASSERT(pListServer!=NULL);
	CListKind * pListKind=pListServer->GetListKind();
	tagGameKind * pGameKind=pListKind->GetItemInfo();
	tagGameServer * pGameServer=pListServer->GetItemInfo();

	////进入房间数目
	//if (m_pRoomViewItem[CountArray(m_pRoomViewItem)-1]!=NULL)
	//{
	//	ShowMessageBox(TEXT("进入的游戏房间太多了，请关闭部分游戏房间再试！"),MB_ICONINFORMATION);
	//	return NULL;
	//}

	if (NULL != m_pRoomViewItemCreate)
	{
		SafeDelete(m_pRoomViewItemCreate);
	}

	//寻找房间
	CRoomViewItem * pRoomViewItem=m_pRoomViewItemCreate;

	//获取版本
	CWinFileInfo WinFileInfo;
	if (WinFileInfo.OpenWinFile(pGameKind->szProcessName)==false)
	{

		TCHAR szProcessName[MODULE_LEN];
		CopyMemory(szProcessName, pGameKind->szProcessName, sizeof(szProcessName));
		TCHAR * pCh = strrchr(szProcessName, '.');
		if (pCh) *pCh = '\0';
		g_GlobalAttemper.DownLoadClient(szProcessName,pGameKind->wKindID,true, 1);

		return false;

		TCHAR szBuffer[512]=TEXT("");
		_snprintf(szBuffer,sizeof(szBuffer),TEXT("【%s】还没有安装，现在是否下载？"),pGameKind->szKindName);
		int nResult=ShowMessageBox(szBuffer,MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON1);
		if (nResult==IDYES) 
		{
			TCHAR szProcessName[MODULE_LEN];
			CopyMemory(szProcessName, pGameKind->szProcessName, sizeof(szProcessName));
			TCHAR * pCh = strrchr(szProcessName, '.');
			if (pCh) *pCh = '\0';
			g_GlobalAttemper.DownLoadClient(szProcessName,pGameKind->wKindID,true,1);
			//g_GlobalAttemper.DownLoadClient(pGameKind->szKindName,pGameKind->wKindID,true);
		}
		return false;
	}

	//获取版本
	DWORD dwFileVerMS=0L,dwFileVerLS=0L;
	WinFileInfo.GetFileVersion(dwFileVerMS,dwFileVerLS);

	//版本分析
	BYTE cbFileVer1=(BYTE)(HIWORD(dwFileVerMS));
	BYTE cbFileVer2=(BYTE)(LOWORD(dwFileVerMS));
	BYTE cbFileVer3=(BYTE)(HIWORD(dwFileVerLS));
	BYTE cbFileVer4=(BYTE)(LOWORD(dwFileVerLS));
	BYTE cbListVer1=(BYTE)(LOWORD(LOWORD(pGameKind->dwMaxVersion)));
	BYTE cbListVer2=(BYTE)(HIBYTE(LOWORD(pGameKind->dwMaxVersion)));
	BYTE cbListVer3=(BYTE)(LOBYTE(HIWORD(pGameKind->dwMaxVersion)));
	BYTE cbListVer4=(BYTE)(HIBYTE(HIWORD(pGameKind->dwMaxVersion)));

	//判断版本
	if ( (cbFileVer1!=cbListVer1) || (cbFileVer2!=cbListVer2) || (cbFileVer3!=cbListVer3) )
	{
		TCHAR szProcessName[MODULE_LEN];
		CopyMemory(szProcessName, pGameKind->szProcessName, sizeof(szProcessName));
		TCHAR * pCh = strrchr(szProcessName, '.');
		if (pCh) *pCh = '\0';
		g_GlobalAttemper.DownLoadClient(szProcessName,pGameKind->wKindID,true,1);

		return false;

		TCHAR szBuffer[512]=TEXT("");
		_snprintf(szBuffer,sizeof(szBuffer),TEXT("【%s】已经更新为 %ld.%ld.%ld.%ld 版本，您的版本不能继续使用，现在是否下载？"),pGameKind->szKindName,
			cbListVer1,cbListVer2,cbListVer3,cbListVer4);
		int nResult=ShowMessageBox(szBuffer,MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON1);
		if (nResult==IDYES) 
		{
			TCHAR szProcessName[MODULE_LEN];
			CopyMemory(szProcessName, pGameKind->szProcessName, sizeof(szProcessName));
			TCHAR * pCh = strrchr(szProcessName, '.');
			if (pCh) *pCh = '\0';
			g_GlobalAttemper.DownLoadClient(szProcessName,pGameKind->wKindID,true,1);
			//g_GlobalAttemper.DownLoadClient(pGameKind->szKindName,pGameKind->wKindID,true);
		}
		return false;
	}

	//兼容版本
	if (cbFileVer4!=cbListVer4)
	{
		TCHAR szProcessName[MODULE_LEN];
		CopyMemory(szProcessName, pGameKind->szProcessName, sizeof(szProcessName));
		TCHAR * pCh = strrchr(szProcessName, '.');
		if (pCh) *pCh = '\0';
		g_GlobalAttemper.DownLoadClient(szProcessName,pGameKind->wKindID,true,2);

		return false;

		TCHAR szBuffer[512]=TEXT("");
		_snprintf(szBuffer,
			sizeof(szBuffer),
			TEXT("【%s】已经更新为 %ld.%ld.%ld.%ld 版本，现在是否下载升级？"),
			pGameKind->szKindName,
			cbListVer1,
			cbListVer2,
			cbListVer3,
			cbListVer4);

		int nResult=ShowMessageBox(szBuffer,MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON1);
		if (nResult==IDYES) 
		{
			TCHAR szProcessName[MODULE_LEN];
			CopyMemory(szProcessName, pGameKind->szProcessName, sizeof(szProcessName));
			TCHAR * pCh = strrchr(szProcessName, '.');
			if (pCh) *pCh = '\0';
			g_GlobalAttemper.DownLoadClient(szProcessName,pGameKind->wKindID,true,2);
			//g_GlobalAttemper.DownLoadClient(pGameKind->szKindName,pGameKind->wKindID,true);
			return false;
		}
	}

	//创建房间视图(桌子列表)
	try
	{
		pRoomViewItem=new CRoomViewItem;
		if (pRoomViewItem==NULL) 
		{
			return false;
		}
		pRoomViewItem->Create(IDD_TABLE_LIST,this);//创建桌子列表对话框
		pRoomViewItem->ShowWindow(SW_HIDE);
		//pRoomViewItem->m_gameListManager = &m_gameListManager;//桌子列表
		if (pRoomViewItem->InitRoomViewItem(pListServer)==false) 
		{
			throw TEXT("游戏房间初始化失败！");
		}
	}
	catch (...) 
	{ 
		SafeDelete(pRoomViewItem); 
	}
	if (pRoomViewItem==NULL)
	{
		ShowMessageBox(TEXT("游戏房间创建失败"),MB_ICONQUESTION);
		PostMessage(WM_COMMAND,IDM_LOGON_PLAZA,0);
		return false;
	}

	//osc update
	//判断自己宝石不够的情况
	tagGlobalUserData& tagUserInfo = g_GlobalUnits.GetGolbalUserData();
	if (tagUserInfo.lGold < pGameServer->lEnterMoney)
	{
		if (tagUserInfo.dwGrade<6 && tagUserInfo.wGiftCount<1)
		{//新手保护赠送金币
			if (tagUserInfo.lGold<3000)
			{
				m_ConfirmGold.SetMode(1);
				m_ConfirmGold.DoModal();
				m_bIsGetGlod=true;
				OnCommandConnect();
			}
			else
			{
				ShowMessageBox(TEXT("您的金币不足,不能进入此房间！"),MB_ICONQUESTION);
			}
		}
		else
		{
			m_ConfirmGold.SetMode(0);
			m_ConfirmGold.DoModal();
			//提示:房间人数已满
			/*char szBuffer[128]={0};
			sprintf(szBuffer, "您的金币不足,不能进入此房间！");
			ShowMessageBox(szBuffer, MB_ICONINFORMATION);*/
		}
		return false;
	}

	//判断自己等级太高
	if (tagUserInfo.dwGrade > pGameServer->lRestrictGrade)
	{
		m_ConfirmGrade.DoModal();
		//提示:等级太高
		/*char szBuffer[128]={0};
		sprintf(szBuffer, "您的等级太高,不能进入此房间！");
		ShowMessageBox(szBuffer, MB_ICONINFORMATION);*/
		return false;
	}

	//连接游戏
	bool bSuccess=pRoomViewItem->ConnectGameServer();
	if (bSuccess==false || pRoomViewItem->GetLogonFull())//判断房间是否满人
	{
		pRoomViewItem->DestroyWindow();
		SafeDelete(pRoomViewItem);
		return false;
	}

	//设置变量
	m_pRoomViewItemCreate=pRoomViewItem;
	CRect plazarect;
	GetWindowRect(&plazarect);
	::MoveWindow(m_pRoomViewItemCreate->GetSafeHwnd(),plazarect.left,plazarect.top, 960, 720, FALSE);
	//cxf ::MoveWindow(m_pRoomViewItemCreate->GetSafeHwnd(),287,81, 659, 626, FALSE);

	//HWND hWnd=NULL;
	//hWnd=m_pRoomViewItemCreate->GetSafeHwnd();

	//设置提示
	CString strBuffer;
	strBuffer.Format(TEXT("正在连接房间，请稍候..."));
	g_GlobalAttemper.ShowStatusMessage(strBuffer,pRoomViewItem);

	return true;

}

void CPlazaDlg::OnMove(int x, int y)
{
	__super::OnMove(x, y);

	if (::IsWindow(m_pRoomViewItemCreate->GetSafeHwnd()) && ::IsWindowVisible(m_pRoomViewItemCreate->GetSafeHwnd()))
	{
		CRect rcClientRect;
		GetWindowRect(&rcClientRect);
		m_pRoomViewItemCreate->SetWindowPos(NULL,rcClientRect.left,rcClientRect.top,960,720,SWP_SHOWWINDOW);
		//m_pRoomViewItemCreate->MoveWindow(rcClientRect,false);
	}

	if (NULL != m_pActivity  &&  ::IsWindow(m_pActivity->GetSafeHwnd())  &&  ::IsWindowVisible(m_pActivity->GetSafeHwnd()))
	{
		CRect rcClientRect;
		GetWindowRect(&rcClientRect);
		m_pActivity->SetWindowPos(&wndTop,rcClientRect.left+6,rcClientRect.top+30, 0, 0, SWP_NOSIZE|SWP_SHOWWINDOW);
	}

	// TODO: 在此处添加消息处理程序代码
	/*cxf if (::IsWindow(m_pDlgLogon->GetSafeHwnd()) && ::IsWindowVisible(m_strCurrentPathPlaza->GetSafeHwnd()))
	{
	m_pDlgLogon->UpdateKeyPos();
	}*/
}

void CPlazaDlg::OnOK()
{
	// TODO: 在此添加专用代码和/或调用基类
	//AfxMessageBox(TEXT("CPlazaDlg::OnOK"));
	__super::OnOK();
}

void CPlazaDlg::OnCancel()
{
	// TODO: 在此添加专用代码和/或调用基类
#if !defined(_DEBUG)
	m_bDeleteMachineCode = true;
	OnCommandConnect();
	SetTimer(IDI_TIMER_WAIT_END, 5000, NULL);
#else
	SetTimer(IDI_TIMER_WAIT_END, 10, NULL);
#endif
}

void CPlazaDlg::OnPaint()
{
	// TODO: 在此处添加消息处理程序代码
	// 不为绘图消息调用 __super::OnPaint()
	if (NULL != m_memdc.m_hDC)
	{
		CPaintDC dc(this); // device context for painting
		//cxf
		////绘制背景
		if (!m_ImageBack.IsNull())
		{
			m_ImageBack.DrawImage(&m_memdc,0,0);
		}
		//CImageHandle handle(&m_ImageBack);
		//m_ImageBack.AlphaDrawImage(&m_memdc, 0, 0, RGB(255, 0, 255));

		//绘制个人形象
		tagGlobalUserData & UserData=g_GlobalUnits.GetGolbalUserData();
		if (UserData.wFaceID>=0)
		{
			if (!m_UserImage.IsNull())
			{
				//cxf m_UserImage.DrawImage(&m_memdc, 14, 82);
				m_UserImage.DrawImage(&m_memdc, 756, 181);
			}
		}

		//绘制游戏列表标题
		if (!m_ImageTitle.IsNull())
		{
			WORD wWidth=m_ImageTitle.GetWidth()/16;
			m_ImageTitle.DrawImage(&m_memdc,430,148,wWidth,m_ImageTitle.GetHeight(),m_wTitlePos*wWidth,0);
		}
		//绘画新手指引动画
		if (m_bNewComerGuide && !m_ImageNewComerGuide.IsNull())
		{
			WORD wWidth=m_ImageNewComerGuide.GetWidth()/2;
			m_ImageNewComerGuide.DrawImage(&m_memdc,m_rcNewComerGuideArea.left,m_rcNewComerGuideArea.top,wWidth,m_ImageNewComerGuide.GetHeight(),
				m_wNewComerGuideActionNumber*wWidth,0);
		}
		/*cxf
		//绘制个人信息背景框
		if (!m_UserInfo.IsNull())
		{
		m_UserInfo.DrawImage(&m_memdc, 13, 497);
		}

		//绘制消息
		if (!m_GameMsgImage.IsNull() && m_bIsFlashMsg)
		{
		m_GameMsgImage.DrawImage(&m_memdc, m_rcGameMsgArea.left, m_rcGameMsgArea.top);
		}*/

		m_memdc.SetBkMode(TRANSPARENT);

		//设置 DC
		CFont* pOldFont= m_memdc.SelectObject(&m_fontNickName);
		CRect rcRect;
		rcRect.left = 780+(150-m_memdc.GetTextExtent(UserData.szNickName).cx)/2;
		rcRect.top = 365+(23-m_memdc.GetTextExtent(UserData.szNickName).cy)/2;
		rcRect.right = rcRect.left + 150;
		rcRect.bottom = rcRect.top + 20;
		//绘制昵称
		DrawTextString(&m_memdc, UserData.szNickName, RGB(109, 209, 48), RGB(0, 0, 0), rcRect.left, rcRect.top);
		m_memdc.SelectObject(&pOldFont);
		pOldFont = NULL;

		//绘制等级
		rcRect.left = 770;
		rcRect.top = 397;
		rcRect.right = rcRect.left + 50;
		rcRect.bottom = rcRect.top + 14;

		m_ImageNums.SetNums(UserData.dwGrade);
		m_ImageNums.SetShowRect(rcRect);
		m_ImageNums.DrawImageNums(&m_memdc);

		m_ImageNums.SetNumAlign(IA_LEFT);
		//藏宝币
		rcRect.left = 840;
		rcRect.top = 432;
		rcRect.right = rcRect.left + 100;
		rcRect.bottom = rcRect.top + 14;
		m_ImageNums.SetNums(UserData.lMoney);
		m_ImageNums.SetShowRect(rcRect);
		m_ImageNums.DrawImageNums(&m_memdc);

		//绘制金币
		rcRect.left = 840;
		rcRect.top = 457;
		rcRect.right = rcRect.left + 100;
		rcRect.bottom = rcRect.top + 14;
		m_ImageNums.SetNums(UserData.lGold);
		m_ImageNums.SetShowRect(rcRect);
		m_ImageNums.DrawImageNums(&m_memdc);

		//宝石
		rcRect.left = 840;
		rcRect.top = 482;
		rcRect.right = rcRect.left + 100;
		rcRect.bottom = rcRect.top + 14;
		m_ImageNums.SetNums(UserData.lGem);
		m_ImageNums.SetShowRect(rcRect);
		m_ImageNums.DrawImageNums(&m_memdc);

		m_ImageNums.SetNumAlign(IA_CENTER);

		//经验,当前经验值，总经验值，居中对齐
		rcRect.left = 814;
		rcRect.top = 394;
		rcRect.right = rcRect.left + 92;
		rcRect.bottom = rcRect.top + 18;

		/*cxf
		//绘制背景
		if (!m_Exp1Image.IsNull())
		{
		m_Exp1Image.DrawImage(&m_memdc, rcRect.left, rcRect.top);
		}*/

		//UserData.dwClass = 4;
		//计算总经验值.
		/*DWORD dwTotalExp=m_pGradeInfo[UserData.dwGrade].iMaxExp;
		DWORD dwTotalValue = 0;
		for (DWORD i=1; i<=UserData.dwGrade; i++)
		{
		dwTotalValue += i;
		}
		DWORD dwTotalExp = sqrt((float)(UserData.dwGrade+1))*dwTotalValue+200;
		if (UserData.dwExperience>=dwTotalExp)
		{
		dwTotalExp = UserData.dwExperience;
		}
		if (0==dwTotalExp)
		{
		dwTotalExp = 1;
		}*/
		int dwTotalExp=m_pGradeInfo[UserData.dwGrade-1].iMaxExp;
		//绘制前景
		if (!m_Exp2Image.IsNull())
		{
			WORD wExp2ImageW = m_Exp2Image.GetWidth();
			WORD wExp2ImageH = m_Exp2Image.GetHeight();
			DWORD dwDrawWidth = UserData.dwExperience*(wExp2ImageW/(float)dwTotalExp);
			m_Exp2Image.DrawImage(&m_memdc, rcRect.left, rcRect.top, dwDrawWidth, wExp2ImageH, 0, 0, dwDrawWidth, wExp2ImageH);
		}
		//绘制数字
		char chExpValue[32]={0};
		::sprintf(chExpValue, "%d\/%d", UserData.dwExperience, dwTotalExp);
		//设置 DC
		pOldFont= m_memdc.SelectObject(&m_fontXPValue);

		rcRect.left = 814+(92-m_memdc.GetTextExtent(chExpValue).cx)/2;
		rcRect.top = 395;
		rcRect.right = rcRect.left + 92;
		rcRect.bottom = rcRect.top + 20;
		DrawTextString(&m_memdc, chExpValue, RGB(206, 255, 1), RGB(0, 0, 0), rcRect.left, rcRect.top);
		m_memdc.SelectObject(&pOldFont);
		pOldFont = NULL;

		//绘制星级
		DrawXJInfo(&m_memdc);

		//绘制按钮
		//cxf DrawTitleButton(&m_memdc);

		//绘制区号
		rcRect.left = 799;
		rcRect.top = 97;
		rcRect.right = rcRect.left + 12;
		rcRect.bottom = rcRect.top + 14;
		m_ImageNumsTel.SetSign(false);
		m_ImageNumsTel.SetNumAlign(IA_RIGHT);
		m_ImageNumsTel.SetNums(0);
		m_ImageNumsTel.SetShowRect(rcRect);
		m_ImageNumsTel.DrawImageNums(&m_memdc);
		rcRect.left = 811;
		rcRect.top = 97;
		rcRect.right = rcRect.left + 32;
		rcRect.bottom = rcRect.top + 14;
		m_ImageNumsTel.SetNumAlign(IA_LEFT);
		m_ImageNumsTel.SetNums(755);
		m_ImageNumsTel.SetShowRect(rcRect);
		m_ImageNumsTel.DrawImageNums(&m_memdc);
		//绘制电话
		rcRect.left = 843;
		rcRect.top = 97;
		rcRect.right = rcRect.left + 80;
		rcRect.bottom = rcRect.top + 14;
		m_ImageNumsTel.SetSign(true);
		m_ImageNumsTel.SetNums(-86056703);
		m_ImageNumsTel.SetShowRect(rcRect);
		m_ImageNumsTel.DrawImageNums(&m_memdc);

		CRect rect;
		GetClientRect(&rect);
		dc.BitBlt(0, 0, rect.Width(), rect.Height(), &m_memdc, 0, 0, SRCCOPY);
	}
}

//绘制星级
void CPlazaDlg::DrawXJInfo(CDC* pDC)
{
	if (NULL == pDC)
	{
		return;
	}

	tagGlobalUserData & UserData=g_GlobalUnits.GetGolbalUserData();
	DWORD dwTotalGameNums = (UserData.lWinCount+UserData.lLostCount+UserData.lDrawCount+UserData.lFleeCount);
	HINSTANCE hInstance = AfxGetInstanceHandle();

	DWORD dwGrade = UserData.dwGrade-1;//等级
	if (m_bIsShowXJInfo && !m_XJInfoBackImage.IsNull())
	{
		m_XJInfoBackImage.DrawImage(pDC, m_UserXJInfoRect.left, m_UserXJInfoRect.top);
		//设置 DC
		CFont* pOldFont= pDC->SelectObject(&m_fontDrawXJ);
		//绘制几星玩家
		char chXJData[64]={0};

		if (dwGrade<0 || dwGrade>m_iGradeInfoDiv)
		{
			return;
		}
		pDC->SetTextColor(RGB(255, 0, 255));
		CopyMemory(chXJData,m_pGradeInfo[dwGrade].strName,sizeof(m_pGradeInfo[dwGrade].strName));

		pDC->TextOut(m_UserXJInfoRect.left+43, m_UserXJInfoRect.top+8, chXJData);


		//绘制游戏总次数
		pDC->SetTextColor(RGB(0, 0, 0));
		::memset(chXJData, 0, sizeof(chXJData));
		::sprintf(chXJData, "游戏总次数：");
		pDC->TextOut(m_UserXJInfoRect.left+4, m_UserXJInfoRect.top+25, chXJData);
		pDC->SetTextColor(RGB(255, 0, 255));
		::memset(chXJData, 0, sizeof(chXJData));
		::sprintf(chXJData, "%d", dwTotalGameNums);
		pDC->TextOut(m_UserXJInfoRect.left+78, m_UserXJInfoRect.top+25, chXJData);

		//绘制胜出次数
		pDC->SetTextColor(RGB(0, 0, 0));
		::memset(chXJData, 0, sizeof(chXJData));
		::sprintf(chXJData, "胜出次数：");
		pDC->TextOut(m_UserXJInfoRect.left+15, m_UserXJInfoRect.top+42, chXJData);
		pDC->SetTextColor(RGB(255, 0, 255));
		::memset(chXJData, 0, sizeof(chXJData));
		::sprintf(chXJData, "%d", UserData.lWinCount);
		pDC->TextOut(m_UserXJInfoRect.left+78, m_UserXJInfoRect.top+42, chXJData);

		//绘制胜出比率
		pDC->SetTextColor(RGB(0, 0, 0));
		::memset(chXJData, 0, sizeof(chXJData));
		::sprintf(chXJData, "胜出比率：");
		pDC->TextOut(m_UserXJInfoRect.left+15, m_UserXJInfoRect.top+58, chXJData);
		pDC->SetTextColor(RGB(255, 0, 255));
		::memset(chXJData, 0, sizeof(chXJData));
		if (dwTotalGameNums>0)
		{
			float fTotalValue = ((float)dwTotalGameNums);
			DWORD dwValue = (UserData.lWinCount/fTotalValue)*100;
			::sprintf(chXJData, "%d\%%", dwValue);
			pDC->TextOut(m_UserXJInfoRect.left+78, m_UserXJInfoRect.top+58, chXJData);
		}
		else
		{
			::sprintf(chXJData, "0\%%");
			pDC->TextOut(m_UserXJInfoRect.left+78, m_UserXJInfoRect.top+58, chXJData);
		}
		pDC->SelectObject(&pOldFont);
		pOldFont = NULL;
	}

	/*cxf	WORD wClass = 0;
	tagGlobalUserData & UserData=g_GlobalUnits.GetGolbalUserData();
	DWORD dwTotalGameNums = (UserData.lWinCount+UserData.lLostCount+UserData.lDrawCount+UserData.lFleeCount);
	CountXJClass(UserData.lWinCount, dwTotalGameNums, wClass);
	if (wClass>=0 && wClass<=5)
	{
	HINSTANCE hInstance = AfxGetInstanceHandle();
	char chXJInfo[32] = {0};
	if (0 == wClass)
	{//新手
	::sprintf(chXJInfo, "PNG_IMAGE_XJ_NEW");
	}
	else if (wClass>=1 && wClass<=5)
	{//星级玩家
	::sprintf(chXJInfo, "PNG_IMAGE_XJ_%d", wClass);
	}
	m_UserXJImage.DestroyImage();
	m_UserXJImage.LoadImage(hInstance, chXJInfo);
	//cxf m_UserXJImage.DrawImage(pDC, m_UserXJRect.left, m_UserXJRect.top);

	if (m_bIsShowXJInfo && !m_XJInfoBackImage.IsNull())
	{
	m_XJInfoBackImage.DrawImage(pDC, m_UserXJInfoRect.left, m_UserXJInfoRect.top);
	//设置 DC
	CFont* pOldFont= pDC->SelectObject(&m_fontDrawXJ);
	//绘制几星玩家
	char chXJData[64]={0};
	if (0 == wClass)
	{
	pDC->SetTextColor(RGB(0, 0, 0));
	::memset(chXJData, 0, sizeof(chXJData));
	::sprintf(chXJData, "新兵");
	pDC->TextOut(m_UserXJInfoRect.left+48, m_UserXJInfoRect.top+8, chXJData);
	}
	else
	{
	pDC->SetTextColor(RGB(255, 0, 255));
	::sprintf(chXJData, "%d", wClass);
	pDC->TextOut(m_UserXJInfoRect.left+35, m_UserXJInfoRect.top+8, chXJData);
	pDC->SetTextColor(RGB(0, 0, 0));
	::memset(chXJData, 0, sizeof(chXJData));
	if (2 == wClass)
	{
	::sprintf(chXJData, "星上尉");
	}
	else if (3 == wClass)
	{
	::sprintf(chXJData, "星少校");
	}
	else if (4 == wClass)
	{
	::sprintf(chXJData, "星上校");
	}
	else if (5 == wClass)
	{
	::sprintf(chXJData, "星上将");
	}
	else
	{
	::sprintf(chXJData, "星上士");
	}
	pDC->TextOut(m_UserXJInfoRect.left+43, m_UserXJInfoRect.top+8, chXJData);
	}


	//绘制游戏总次数
	pDC->SetTextColor(RGB(0, 0, 0));
	::memset(chXJData, 0, sizeof(chXJData));
	::sprintf(chXJData, "游戏总次数：");
	pDC->TextOut(m_UserXJInfoRect.left+4, m_UserXJInfoRect.top+25, chXJData);
	pDC->SetTextColor(RGB(255, 0, 255));
	::memset(chXJData, 0, sizeof(chXJData));
	::sprintf(chXJData, "%d", dwTotalGameNums);
	pDC->TextOut(m_UserXJInfoRect.left+78, m_UserXJInfoRect.top+25, chXJData);

	//绘制胜出次数
	pDC->SetTextColor(RGB(0, 0, 0));
	::memset(chXJData, 0, sizeof(chXJData));
	::sprintf(chXJData, "胜出次数：");
	pDC->TextOut(m_UserXJInfoRect.left+15, m_UserXJInfoRect.top+42, chXJData);
	pDC->SetTextColor(RGB(255, 0, 255));
	::memset(chXJData, 0, sizeof(chXJData));
	::sprintf(chXJData, "%d", UserData.lWinCount);
	pDC->TextOut(m_UserXJInfoRect.left+78, m_UserXJInfoRect.top+42, chXJData);

	//绘制胜出比率
	pDC->SetTextColor(RGB(0, 0, 0));
	::memset(chXJData, 0, sizeof(chXJData));
	::sprintf(chXJData, "胜出比率：");
	pDC->TextOut(m_UserXJInfoRect.left+15, m_UserXJInfoRect.top+58, chXJData);
	pDC->SetTextColor(RGB(255, 0, 255));
	::memset(chXJData, 0, sizeof(chXJData));
	if (dwTotalGameNums>0)
	{
	float fTotalValue = ((float)dwTotalGameNums);
	DWORD dwValue = (UserData.lWinCount/fTotalValue)*100;
	::sprintf(chXJData, "%d\%%", dwValue);
	pDC->TextOut(m_UserXJInfoRect.left+78, m_UserXJInfoRect.top+58, chXJData);
	}
	else
	{
	::sprintf(chXJData, "0\%%");
	pDC->TextOut(m_UserXJInfoRect.left+78, m_UserXJInfoRect.top+58, chXJData);
	}
	pDC->SelectObject(&pOldFont);
	pOldFont = NULL;
	}
	}
	*/
}

//绘制所有按钮
void CPlazaDlg::DrawTitleButton(CDC *pDC)
{
	if (NULL == pDC)
	{
		return;
	}
	/*cxf 1024
	for (int i=0; i<4; ++i)
	{
	if (!m_TitleButtonList[i].BTImage.IsNull())
	{
	int iImageW = m_TitleButtonList[i].BTImage.GetWidth()/4;
	int iImageH = m_TitleButtonList[i].BTImage.GetHeight();
	int iSrcPos = 0;
	if (0 == m_TitleButtonList[i].btStatus)
	{
	iSrcPos = 0;
	}
	else if (1 == m_TitleButtonList[i].btStatus)
	{
	iSrcPos = 3*iImageW;
	}
	else if (2 == m_TitleButtonList[i].btStatus)
	{
	iSrcPos = iImageW;
	}
	else if (3 == m_TitleButtonList[i].btStatus)
	{
	iSrcPos = 2*iImageW;
	}
	m_TitleButtonList[i].BTImage.DrawImage(pDC, m_TitleButtonList[i].rcRect.left, m_TitleButtonList[i].rcRect.top, iImageW, iImageH,
	iSrcPos, 0, iImageW, iImageH);
	}
	}
	*/
	/*cxf if (!m_PngHat.IsNull())
	{
	m_PngHat.DrawImage(pDC, m_TitleButtonList[0].rcRect.left-32, m_TitleButtonList[0].rcRect.top-8);
	}*/
}


//计算星级
void CPlazaDlg::CountXJClass(DWORD dwWinNums, DWORD dwTotalNums, WORD& wClass)
{
	wClass = 0;
	if ((0 == dwTotalNums) || (dwWinNums>dwTotalNums))
	{
		return;
	}
	DWORD wRate = (dwWinNums/(float)dwTotalNums)*100;
	if ((dwWinNums>=50)&&(wRate>=50))
	{//一星
		wClass = 1;
	}
	else if ((dwWinNums>=150)&&(wRate>=55))
	{//二星
		wClass = 2;
	}
	else if ((dwWinNums>=300)&&(wRate>=60))
	{//三星
		wClass = 3;
	}
	else if ((dwWinNums>=500)&&(wRate>=65))
	{//四星
		wClass = 4;
	}
	else if ((dwWinNums>=800)&&(wRate>=75))
	{//五星
		wClass = 5;
	}
}

//艺术字体
void CPlazaDlg::DrawTextString(CDC * pDC, LPCTSTR pszString, COLORREF crText, COLORREF crFrame, int nXPos, int nYPos)
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

void CPlazaDlg::OnBnCancelGame()
{
	if (NULL != m_pRoomViewItemCreate && ::IsWindow(m_pRoomViewItemCreate->GetSafeHwnd()))
	{
		m_pRoomViewItemCreate->CloseRoomViewItem();
	}
	OnCancel();
}
void CPlazaDlg::ConfirmClose()
{
	if (NULL != m_pActivity  &&  ::IsWindow(m_pActivity->GetSafeHwnd())  &&  ::IsWindowVisible(m_pActivity->GetSafeHwnd()))
	{
		m_pActivity->ShowWindow(SW_HIDE);
		m_pActivity->SendMessage(WM_CLOSE);
		SafeDelete(m_pActivity);
		return;
	}

	//Description<在GM管理界面点击关闭则返回游戏列表>;Author<cxf>;DateTime<2012-03-20>
	if (NULL!=m_pRoomViewItemCreate && NULL!=m_pRoomViewItemCreate->GetSafeHwnd() && m_pRoomViewItemCreate->IsWindowVisible()==TRUE)
	{
		m_pRoomViewItemCreate->SendRequestCloseRoomMsg(1);
		return;
	}

	m_PlazaCloseConfirm.DoModal();

	BYTE byCloseType = m_PlazaCloseConfirm.GetCloseType();
	if (0 == byCloseType)
	{//关闭大厅
		OnBnClickedClose();
	}
	else if (1 == byCloseType)
	{
#if !defined(_DEBUG)
		m_bDeleteMachineCode = true;
		OnCommandConnect();
		SetTimer(IDI_TIMER_WAIT_END, 5000, NULL);
#else
		SetTimer(IDI_TIMER_WAIT_END, 10, NULL);
#endif
	}
}
//重新登录
void CPlazaDlg::ResetLogon()
{
	m_ServerItemView.InitServerItemView(/*this*/);
	g_GlobalUnits.m_ServerListManager.InitServerListManager(&m_ServerItemView);
	m_GameListCenterDlg.SetWndHide();
	OnBnClickedRollBack();
	//关闭桌子Socket
	if (m_pRoomViewItemCreate->GetSafeHwnd())
	{//防止点击进入游戏按钮（还在Socket连接时）点击重新登录按钮。(不关闭Socket会出现大厅和登录框同时显示)
		m_pRoomViewItemCreate->m_ClientSocket->CloseSocket();
		m_pRoomViewItemCreate->ShowWindow(SW_HIDE);
	}
	
	m_bLogonPlaza = false;
	m_ClientSocket->CloseSocket();
	m_pDlgLogon->m_DlgLogonLogo->ShowWindow(SW_SHOW);
	m_pDlgLogon->ShowWindow(SW_SHOW);
	this->ShowWindow(SW_HIDE);
}
//关闭程序
void CPlazaDlg::OnBnClickedClose()
{
	if (NULL != m_pRoomViewItemCreate && ::IsWindow(m_pRoomViewItemCreate->GetSafeHwnd()))
	{
		m_pRoomViewItemCreate->SendRequestCloseRoomMsg(3);
	}
	else
	{
		OnCancel();
	}
	return;
}

void CPlazaDlg::OnBnClickedReLogon()
{
	//投递消息
	PostMessage(WM_COMMAND,IDM_LOGON_PLAZA,0);
	return ;
}

// 我的宝箱 IDC_BT_BEAUTY_SALON
// void CPlazaDlg::OnBnClickedBeautySalon()
// {
// 	ShellExecute(NULL,   "open",   "http://www.acetechcn.com ",     ""   ,NULL,   SW_SHOW);
// 
// }
// 游戏商场 IDC_BT_SHOP
// void CPlazaDlg::OnBnClickedGameShop()
// {
// 	ShellExecute(NULL,   "open",   "http://www.acetechcn.com ",     ""   ,NULL,   SW_SHOW);
// 
// }

//返回     IDC_BT_RETURN
void CPlazaDlg::OnBnClickedReturn()
{
	if (!m_bIsGamePlazyStatus)
	{//如果不是游戏大厅状态，则直接返回。
		return;
	}
	switch (m_curTab)
	{
	case 0:
		{
			return;
		}
		break;
	case 1:
		{//点击返回到登录界面
			OnBnClickedReLogon();
		}
		break;
	case 2:
		{//从房间列表返回到游戏大厅
			break;
		}
	case 3:
		{//从桌子列表返回到房间列表
			if (m_pRoomViewItemCreate && ::IsWindow(m_pRoomViewItemCreate->GetSafeHwnd()))
			{
				m_pRoomViewItemCreate->SendRequestCloseRoomMsg(1);
			}
			return;
		}
	}
	ShowTab(m_curTab-1);
	//更新用户形象
	UpdateUserImage();
}				
//更新声音
void CPlazaDlg::UpdateMusicSound(bool bIsEnable)
{
	if (bIsEnable)
	{
		if (1==m_bIsSound)
		{
			CString strPath("");
			GetCurrentDirectoryPath(strPath);
			strPath.Append(_T("\\data\\plaza\\sound\\BackMusic.wav"));
			::PlaySound(strPath, AfxGetInstanceHandle(),SND_LOOP|SND_ASYNC|SND_NODEFAULT|SND_NOSTOP|SND_FILENAME);
		}
		else
		{
			::PlaySound(NULL, AfxGetInstanceHandle(),SND_PURGE);
		}
	}
	else
	{
		::PlaySound(NULL, AfxGetInstanceHandle(),SND_PURGE);
	}
}

//开启或关闭声音
void CPlazaDlg::SetMusicSound(bool bIsEnable)
{
	if (bIsEnable)
	{//开启声音
		CString strPath("");
		GetCurrentDirectoryPath(strPath);
		strPath.Append(_T("\\data\\plaza\\sound\\BackMusic.wav"));
		::PlaySound(strPath, AfxGetInstanceHandle(),SND_LOOP|SND_ASYNC|SND_NODEFAULT|SND_NOSTOP|SND_FILENAME);
	}
	else
	{//禁止声音
		::PlaySound(NULL, AfxGetInstanceHandle(),SND_PURGE);
	}
	/*cxf
	g_GlobalOption.m_bEnableBackMusic = bIsEnable;
	//播放背景音乐
	if (g_GlobalOption.m_bEnableBackMusic)
	{//开启声音
	CString strPath("");
	GetCurrentDirectoryPath(strPath);
	strPath.Append(_T("\\data\\plaza\\sound\\BackMusic.wav"));
	::PlaySound(strPath, AfxGetInstanceHandle(),SND_LOOP|SND_ASYNC|SND_NODEFAULT|SND_NOSTOP|SND_FILENAME);
	}
	else
	{//禁止声音
	::PlaySound(NULL, AfxGetInstanceHandle(),SND_PURGE);
	}

	tagGlobalUserData & UserData=g_GlobalUnits.GetGolbalUserData();
	//构造信息
	TCHAR szBuffer[256];
	_snprintf(szBuffer,sizeof(szBuffer),TEXT("%s\\%ld"),REG_USER_INFO,UserData.dwUserID);

	//写入信息
	CRegKey RegUserInfo;
	if (RegUserInfo.Create(HKEY_CURRENT_USER,szBuffer)==ERROR_SUCCESS)
	{
	RegUserInfo.SetDWORDValue(TEXT("BackMusicTag"),g_GlobalOption.m_bEnableBackMusic?1:0);
	}
	*/
}

//最小化   IDCMINSIZE
void CPlazaDlg::OnBnClickedMinsize()
{
	::PlaySound(NULL, AfxGetInstanceHandle(),SND_PURGE);
	::ShowWindow(GetSafeHwnd(), SW_MINIMIZE);
}

BOOL CPlazaDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if (m_ToolTipCtrl.m_hWnd!=NULL)
	{
		m_ToolTipCtrl.RelayEvent(pMsg);
	}

	if(pMsg->message==WM_KEYDOWN)
	{
		if (VK_SPACE == pMsg->wParam)
		{
			return TRUE;
		}
		else if (VK_RETURN == pMsg->wParam)
		{
			if (m_pDlgLogon->IsWindowVisible())
			{
				m_pDlgLogon->OnOK();
			}
			return TRUE;
		}

		if (::GetKeyState(VK_CONTROL)<0)
		{
			if (70==pMsg->wParam || 102==pMsg->wParam)
			{
				if (NULL != m_pRoomViewItemCreate && ::IsWindow(m_pRoomViewItemCreate->GetSafeHwnd()) &&
					TRUE == m_pRoomViewItemCreate->IsWindowVisible())
				{
					m_pRoomViewItemCreate->SetCtrolAddF();
				}
				return TRUE;
			}
		}

		/*cxf
		else if (VK_BACK == pMsg->wParam)
		{
		if (IsWindowVisible() && m_bIsGamePlazyStatus && m_btnReturn.IsWindowEnabled())
		{
		OnBnClickedReturn();
		}
		}*/
	}
	return __super::PreTranslateMessage(pMsg);
}

//执行相应按钮
void  CPlazaDlg::ExeButtonEvent(BYTE byEventID)
{
	if (byEventID>=0 && byEventID<4)
	{
		if (0 == byEventID)
		{//游戏大厅
			OnBnClickedGamePlazy();
		}
		else if (1 == byEventID)
		{//任务
			OnBnClickedTask();
		}
		else if (2 == byEventID)
		{//藏宝阁
			OnBnClickedDepository();
		}
		else if (3 == byEventID)
		{//商城
			OnBnClickedShop();
		}
	}
}

void CPlazaDlg::OnBnClickedShop()
{//商城
	/* cxf 1022
	if (3 != m_curTab)
	{//可以去商城买道具
	SetShowTab(32);
	m_bIsGamePlazyStatus = false;
	}
	else
	{//不可以买道具
	TCHAR szRuleUrl[128]={0};
	_snprintf(szRuleUrl,sizeof(szRuleUrl),WEB_MY_GAMEROOM_HELP,m_roomListDlg.m_wKindID);
	ShellExecute(NULL,TEXT("open"),szRuleUrl,NULL,NULL,SW_SHOWDEFAULT);

	if (m_bIsGamePlazyStatus)
	{
	m_TitleButtonList[3].btStatus = 0;
	m_TitleButtonList[0].btStatus = 3;
	InvalidateRect(&m_TitleButtonList[0].rcRect);
	InvalidateRect(&m_TitleButtonList[3].rcRect);
	}

	}
	*/
}

void CPlazaDlg::OnBnClickedTask()
{//任务
	SetShowTab(8);
	m_bIsGamePlazyStatus = false;
}

void CPlazaDlg::OnBnClickedGamePlazy()
{//游戏大厅
	m_bIsGamePlazyStatus = true;
	ShowTab(m_curTab);
}

void CPlazaDlg::OnBnClickedDepository()
{//藏宝阁
	SetShowTab(16);
	m_bIsGamePlazyStatus = false;
}
BOOL CPlazaDlg::onHtmlContextMenu(IHTMLEventObj *pEvtObj)
{ 
	//在成功连接上事件源后,每次用户右击都会调用这个函数,你可以根据pEvtObj来判断当前光标位置等,然后决定是自己弹出菜单,让IE弹出菜单,还是什么都不做...
	return S_OK;
}

void CPlazaDlg::OnDocumentCompleteExplorer3(LPDISPATCH pDisp, VARIANT FAR* URL) 
{
	//处理WebBrowser控件的DocumentComplete事件,并初始化pDoc2指针和连接到事件源
	//HRESULT hr=m_pExUserInfo->get_Document()->QueryInterface(IID_IHTMLDocument2,(void**)&pDoc2);
	//BOOL Ret = AfxConnectionAdvise( 
	//	pDoc2,  //可连接对象的接口指针
	//	DIID_HTMLDocumentEvents2, //连接接口ID
	//	GetIDispatch(FALSE), //把内嵌的IDispatch实现类的一个对象实例m_xDispatch传了出去
	//	FALSE,   //donod addref
	//	&m_dwCookie );  //cookie to break connection later...
	//if(Ret){
	//		//AfxMessageBox("成功挂接上");
	//}
}

void CPlazaDlg::CloseWebWindow(int nIDD)
{
	/*cxf 1022
	switch (nIDD)
	{
	//任务
	case 0:
	{
	//cxf 1022  ::ShowWindow(m_TaskDlg.GetSafeHwnd(), SW_HIDE);
	}
	break;
	//商城
	case 1:
	{
	UpdateUserImage();
	::ShowWindow(m_shopDlg.GetSafeHwnd(), SW_HIDE);
	}
	break;
	//藏宝阁
	case 2:
	{
	::ShowWindow(m_doteyBoxDlg.GetSafeHwnd(), SW_HIDE);
	}
	break;
	}
	*/
}

//更新用户形象
void CPlazaDlg::OnCannUpdateUserImage()
{
	UpdateUserImage();
}

//更新形象
void CPlazaDlg::UpdateUserImage()
{
	/*cxf 1022 CRect rect;
	rect.left = 14;
	rect.top = 82;
	rect.right = rect.left + 230;
	rect.bottom = rect.top + 673;*/
	CRect rect;
	rect.left = 750;
	rect.top = 170;
	rect.right = rect.left + 200;
	rect.bottom = rect.top + 340;
	InvalidateRect(&rect);
}
//更新用户金币
void CPlazaDlg::UpdateUserGold(DWORD dwGold)
{
	tagGlobalUserData & UserData=g_GlobalUnits.GetGolbalUserData();
	UserData.lGold = dwGold;
	CRect rcRect;
	rcRect.left = 103;
	rcRect.top = 574;
	rcRect.right = rcRect.left + 150;
	rcRect.bottom = rcRect.top + 14;
	InvalidateRect(&rcRect);
}
//更新用户藏宝币
void CPlazaDlg::UpdateUserMoney(DWORD dwMoney)
{
	tagGlobalUserData & UserData=g_GlobalUnits.GetGolbalUserData();
	UserData.lMoney = dwMoney;
	CRect rcRect;
	rcRect.left = 103;
	rcRect.top = 607;
	rcRect.right = rcRect.left + 150;
	rcRect.bottom = rcRect.top + 14;
	InvalidateRect(&rcRect);
}

//更新用户道具信息
void CPlazaDlg::UpdateUserDJInfo(int iDJID)
{
	if (NULL != m_pRoomViewItemCreate)
	{
		m_pRoomViewItemCreate->UpdateUserDJInfo(iDJID);
	}
	//更新形象
	UpdateUserImage();
}

//void CPlazaDlg::OnWindowPosChanged(WINDOWPOS* lpwndpos)
//{
//	CDialog::OnWindowPosChanged(lpwndpos);
//	// TODO: 在此处添加消息处理程序代码
//
//	////锁定屏幕
//	//LockWindowUpdate();
//
//	//ResizeFrame();
//
//	////解除锁定
//	//UnlockWindowUpdate();
//}

//重置alpha值
void CPlazaDlg::SetResetAlphaValue(HWND hWnd)
{
	return;
	if (!::IsWindow(hWnd))
	{
		return;
	}
	if (0==m_byCurWndTag && m_hSaveOldCurWnd==hWnd)
	{
		return;
	}
	else if ((1==m_byCurWndTag || 2==m_byCurWndTag) && m_hSaveNewCurWnd==hWnd)
	{
		return;
	}
	KillTimer(IDI_TIME_SET_DLG_ALPHA_END);
	KillTimer(IDI_TIME_SET_DLG_ALPHA_VALUE);
	if (0 != m_byCurWndTag)
	{//如果此前的动画还未完成,则直接隐藏动画中的窗口
		//::ShowWindow(m_hSaveNewCurWnd, SW_HIDE);
		//::ShowWindow(m_hSaveOldCurWnd, SW_HIDE);
		m_pFunShowLayer(m_hSaveNewCurWnd, 0, 0, 2);
		m_pFunShowLayer(m_hSaveOldCurWnd, 0, 0, 2);
	}
	if (!::IsWindow(m_hSaveOldCurWnd))
	{
		m_hSaveOldCurWnd = hWnd;
		m_byCurWndTag = 2;
		m_uiCurAlphaValue = 0;
		m_pFunShowLayer(m_hSaveNewCurWnd, 0, m_uiCurAlphaValue, 2);
	}
	else
	{
		m_byCurWndTag = 1;
		m_uiCurAlphaValue = 255;
		m_pFunShowLayer(m_hSaveOldCurWnd, 0, m_uiCurAlphaValue, 2);
	}
	m_hSaveNewCurWnd = hWnd;
	//::ShowWindow(m_hSaveNewCurWnd, SW_HIDE);
	//::ShowWindow(m_hSaveOldCurWnd, SW_HIDE);
	SetTimer(IDI_TIME_SET_DLG_ALPHA_VALUE, TIME_SET_DLG_ALPHA_INTERVAL, NULL);
	SetTimer(IDI_TIME_SET_DLG_ALPHA_END, TIME_SET_DLG_ALPHA_END_INTERVAL, NULL);
}

//时间消息
void CPlazaDlg::OnTimer(UINT nIDEvent)
{
	if (IDI_TIMER_PLAY_BACK_MUSIC == nIDEvent)
	{//播放背景音乐
		KillTimer(IDI_TIMER_PLAY_BACK_MUSIC);
		CString strPath("");
		GetCurrentDirectoryPath(strPath);
		strPath.Append(_T("\\data\\plaza\\sound\\BackMusic.wav"));
		::PlaySound(strPath, AfxGetInstanceHandle(),SND_LOOP|SND_ASYNC|SND_NODEFAULT|SND_NOSTOP|SND_FILENAME);
	}
	else if (TIMER_WEB_LOADING_PLAZA == nIDEvent)
	{
		KillTimer(TIMER_WEB_LOADING_PLAZA);
		m_webPlazaAdvertisement.Navigate(CInitData::s_cAdverTisement,NULL,NULL,NULL,NULL);
		m_webPlazaNotice.Navigate(CInitData::s_cNotice,NULL,NULL,NULL,NULL);
		m_webPlazaMessage.Navigate(CInitData::s_cXBMessage, NULL,NULL,NULL,NULL);
		m_webPlazaTitle.Navigate(CInitData::s_cTitleNotice,NULL,NULL,NULL,NULL);
	}
	else if (TIMER_UPDATEUSERINFO_OPERATE == nIDEvent)
	{
		KillTimer(TIMER_UPDATEUSERINFO_OPERATE);
		m_btnUpdateUserInfo.EnableWindow(TRUE);
	}
	else if (IDI_ITMER_MEWCOMER_GUIDE == nIDEvent)
	{//新手指引播放帧数
		m_wNewComerGuideActionNumber = m_wNewComerGuideActionNumber==0?1:0;
		InvalidateRect(m_rcNewComerGuideArea);
	}
	else if (IDI_TIMER_WAIT_END == nIDEvent)
	{
		KillTimer(IDI_TIMER_WAIT_END);
		m_ClientSocket->CloseSocket();
		BYTE byCloseType = m_PlazaCloseConfirm.GetCloseType();
		
		if (0 == byCloseType)
		{
			if (NULL!=m_pDlgLogon && ::IsWindow(m_pDlgLogon->GetSafeHwnd()))
			{
				m_pDlgLogon->ExitLogon();
			}
			((CGamePlazaApp*)AfxGetApp())->ExitGamePlaza();
		}
		else
		{
			//重新登录
			ResetLogon();
		}
	}
	else if (TIME_WAIT_CHECK_LINE == nIDEvent) // 检查线路超时
	{
		KillTimer(TIME_WAIT_CHECK_LINE);
		m_pDlgLogon->SetCheckLineResult(0);
	}

	/*cxf 
	else if (IDI_TIMER_GAME_MSG_FLASH == nIDEvent)
	{//游戏消息动画
	m_bIsFlashMsg = !m_bIsFlashMsg;
	InvalidateRect(&m_rcGameMsgArea);
	}*/

	//if (IDI_TIME_SET_DLG_ALPHA_VALUE == nIDEvent)
	//{
	//	if (1 == m_byCurWndTag)
	//	{//淡出旧窗口
	//		if (::IsWindow(m_hSaveOldCurWnd))
	//		{
	//			if (m_uiCurAlphaValue<=100)
	//			{
	//				m_uiCurAlphaValue = 0;
	//				m_pFunShowLayer(m_hSaveOldCurWnd, 0, 0, 2);
	//				//::ShowWindow(m_hSaveOldCurWnd, SW_HIDE);
	//				if (::IsWindow(m_hSaveNewCurWnd))
	//				{
	//					m_pFunShowLayer(m_hSaveNewCurWnd, 0, 0, 2);
	//					m_byCurWndTag = 2;
	//					m_uiCurAlphaValue = 0;
	//				}
	//				else
	//				{
	//					KillTimer(IDI_TIME_SET_DLG_ALPHA_VALUE);
	//				}
	//			}
	//			else
	//			{
	//				//::ShowWindow(m_hSaveOldCurWnd, SW_SHOW);
	//				m_pFunShowLayer(m_hSaveOldCurWnd, 0, m_uiCurAlphaValue, 2);
	//				m_uiCurAlphaValue -= 50;
	//			}
	//		}
	//		else
	//		{
	//			if (::IsWindow(m_hSaveNewCurWnd))
	//			{
	//				m_pFunShowLayer(m_hSaveNewCurWnd, 0, 0, 2);
	//				m_byCurWndTag = 2;
	//				m_uiCurAlphaValue = 0;
	//			}
	//			else
	//			{
	//				KillTimer(IDI_TIME_SET_DLG_ALPHA_VALUE);
	//			}
	//		}
	//	}
	//	else
	//	{//淡入新窗口
	//		if (::IsWindow(m_hSaveNewCurWnd))
	//		{
	//			//::ShowWindow(m_hSaveNewCurWnd, SW_SHOW);
	//			if (m_uiCurAlphaValue>=250)
	//			{
	//				KillTimer(IDI_TIME_SET_DLG_ALPHA_VALUE);
	//				KillTimer(IDI_TIME_SET_DLG_ALPHA_END);
	//				m_pFunShowLayer(m_hSaveNewCurWnd, 0, 255, 2);
	//				::SetWindowPos(m_hSaveNewCurWnd, HWND_BOTTOM, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_SHOWWINDOW);
	//				if (m_hSaveNewCurWnd!=m_hSaveOldCurWnd && ::IsWindow(m_hSaveOldCurWnd))
	//				{
	//					m_pFunShowLayer(m_hSaveOldCurWnd, 0, 0, 2);
	//				}
	//				m_uiCurAlphaValue = 0;
	//				m_byCurWndTag = 0;
	//				m_hSaveOldCurWnd = m_hSaveNewCurWnd;
	//				m_hSaveNewCurWnd = NULL;
	//			}
	//			else
	//			{
	//				m_pFunShowLayer(m_hSaveNewCurWnd, 0, m_uiCurAlphaValue, 2);
	//				m_uiCurAlphaValue += 30;
	//			}
	//		}
	//		else
	//		{
	//			KillTimer(IDI_TIME_SET_DLG_ALPHA_VALUE);
	//		}
	//	}
	//}
	//else if (IDI_TIME_SET_DLG_ALPHA_END == nIDEvent)
	//{
	//	KillTimer(IDI_TIME_SET_DLG_ALPHA_END);
	//	m_pFunShowLayer(m_hSaveNewCurWnd, 0, 255, 2);
	//}
}


//设置所有窗口显示状态
void CPlazaDlg::SetAllWndShow(bool bIsShow)
{
	if (bIsShow)
	{
		//cxf 1022 m_TaskDlg.ShowWindow(SW_SHOW);	//任务
		//cxf 1022 m_shopDlg.ShowWindow(SW_SHOW);	//商城
		//cxf 1022 m_doteyBoxDlg.ShowWindow(SW_SHOW);	//藏宝阁
		//cxf m_gameListDlg.ShowWindow(SW_SHOW);	//游戏列表
		//cxf 1022 m_roomListDlg.ShowWindow(SW_SHOW);	//房间列表
		if (m_pRoomViewItemCreate && ::IsWindow(m_pRoomViewItemCreate->GetSafeHwnd()))	//桌子列表
		{
			::ShowWindow(m_pRoomViewItemCreate->GetSafeHwnd(), SW_SHOW);
		}
	}
	else
	{
		//cxf 1022 m_TaskDlg.ShowWindow(SW_HIDE);	//任务
		//cxf 1022 m_shopDlg.ShowWindow(SW_HIDE);	//商城
		//cxf 1022 m_doteyBoxDlg.ShowWindow(SW_HIDE);	//藏宝阁
		//cxf m_gameListDlg.ShowWindow(SW_HIDE);	//游戏列表
		//cxf 1022 m_roomListDlg.ShowWindow(SW_HIDE);	//房间列表
		if (m_pRoomViewItemCreate && ::IsWindow(m_pRoomViewItemCreate->GetSafeHwnd()))	//桌子列表
		{
			::ShowWindow(m_pRoomViewItemCreate->GetSafeHwnd(), SW_HIDE);
		}
	}
}

//设置所有窗口透明度
void CPlazaDlg::SetAllWndAlpha(BYTE byAlpha)
{
	//cxf 1022 m_pFunShowLayer(m_TaskDlg.GetSafeHwnd(), 0, byAlpha, 2);	//任务
	//cxf 1022 m_pFunShowLayer(m_shopDlg.GetSafeHwnd(), 0, byAlpha, 2);	//商城
	//cxf 1022 m_pFunShowLayer(m_doteyBoxDlg.GetSafeHwnd(), 0, byAlpha, 2);	//藏宝阁
	//cxf m_pFunShowLayer(m_gameListDlg.GetSafeHwnd(), 0, byAlpha, 2);	//游戏列表
	//cxf 1022 m_pFunShowLayer(m_roomListDlg.GetSafeHwnd(), 0, byAlpha, 2);	//房间列表
	//cxf 1022 if (m_pRoomViewItemCreate && ::IsWindow(m_pRoomViewItemCreate->GetSafeHwnd()))	//桌子列表
	//cxf 1022 {
	//cxf 1022 	m_pFunShowLayer(m_pRoomViewItemCreate->GetSafeHwnd(), 0, byAlpha, 2);
	//cxf 1022 }
}

//显示相应按钮
void CPlazaDlg::SetShowTab(BYTE byValue)
{
	SetAllWndShow(false);
	if (1 == byValue)
	{//显示游戏列表
		//cxf m_gameListDlg.ShowWindow(SW_SHOW);	//游戏列表
		//cxf m_btnReturn.EnableWindow(TRUE);
	}
	else if (2 == byValue)
	{//显示房间列表
		//cxf 1022 m_roomListDlg.ShowWindow(SW_SHOW);	//房间列表
		//cxf m_btnReturn.EnableWindow(TRUE);
	}
	else if (4 == byValue)
	{//显示桌子列表
		if (m_pRoomViewItemCreate && ::IsWindow(m_pRoomViewItemCreate->GetSafeHwnd()))	//桌子列表
		{
			::ShowWindow(m_pRoomViewItemCreate->GetSafeHwnd(), SW_SHOW);
			//cxf m_btnReturn.EnableWindow(TRUE);
		}
		else
		{
			//cxf m_btnReturn.EnableWindow(FALSE);
		}
	}
	else if (8 == byValue)
	{//显示任务
		//cxf 1022 m_TaskDlg.ShowWindow(SW_SHOW);	//任务
		//cxf m_btnReturn.EnableWindow(FALSE);
	}
	else if (16 == byValue)
	{//显示藏宝阁
		//cxf 1022 m_doteyBoxDlg.ShowWindow(SW_SHOW);	//藏宝阁
		//cxf m_btnReturn.EnableWindow(FALSE);
	}
	else if (32 == byValue)
	{//显示商城
		//cxf 1022 m_shopDlg.ShowWindow(SW_SHOW);	//商城
		//cxf m_btnReturn.EnableWindow(FALSE);
	}
}

BOOL CPlazaDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
	// TODO: 在此添加专用代码和/或调用基类
	return CDialog::OnCommand(wParam, lParam);
}

void CPlazaDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if(nID == SC_MINIMIZE)
	{
		::PlaySound(NULL, AfxGetInstanceHandle(),SND_PURGE);
	}
	else if (SC_RESTORE == nID)
	{
		UpdateMusicSound(true);
	}
	else if (SC_MAXIMIZE == nID)
	{

	}
	else if (SC_CLOSE == nID)
	{

	}
	CDialog::OnSysCommand(nID, lParam);
}

//设置	   IDC_BT_SET
void CPlazaDlg::OnBnClickedSet()
{
	/*cxf CSetDialog setdlg;
	setdlg.DoModal();*/

	UINT uiReturn = GetCommonConfig("Sound","EnablePlazeBGMusic",1);

	if (0==uiReturn)
	{//开启声音
		m_bIsSound=true;
		m_btnSound.LoadRgnImage(TEXT(m_strCurrentPathPlaza+"BTSoundOpen.png"),false);
	}else
	{//关闭声音
		m_bIsSound=false;
		m_btnSound.LoadRgnImage(TEXT(m_strCurrentPathPlaza+"BTSoundClose.png"),false);
	}
	SetMusicSound(m_bIsSound);//是否播放声音
	SetCommonConfig("Sound","EnablePlazeBGMusic",((true == m_bIsSound)? TEXT("1"): TEXT("0")));
	m_btnSound.Invalidate(FALSE);
}
//登录窗口
void CPlazaDlg::SetDlgLogonPtr(CDlgLogon * pDlgLogon)
{
	if (NULL != pDlgLogon)
	{
		m_pDlgLogon = pDlgLogon;
	}
}
//当前路径
void CPlazaDlg::GetCurrentDirectoryPath(CString &szPath)
{
	szPath="";
	TCHAR szPathTemp[MAX_PATH]={0};
	GetCurrentDirectory(MAX_PATH, szPathTemp);
	szPath.Append(szPathTemp);
}
//大厅资源路径
void CPlazaDlg::GetResPath(CString strClipName, CString& strPath)
{
	CString strModuleFileName("");
	GetCurrentDirectoryPath(strModuleFileName);
	strModuleFileName.Append(strClipName);
	strPath = strModuleFileName;
}
//获取配置
UINT CPlazaDlg::GetCommonConfig(CString strTitle,CString strName, int uiDefault)
{
	CString strLclPth, strFlPth;
	GetModuleFileName(NULL, strLclPth.GetBufferSetLength(256), 256);
	strFlPth = strLclPth.Left(strLclPth.ReverseFind('\\')+1)+ TEXT("config\\Common.ini");
	return GetPrivateProfileInt(TEXT(strTitle), TEXT(strName), uiDefault, strFlPth); 
}
//设置配置
void CPlazaDlg::SetCommonConfig(CString strTitle,CString strName, CString bIsSound)
{
	CString strLclPth, strFlPth;
	GetModuleFileName(NULL, strLclPth.GetBufferSetLength(256), 256);
	strFlPth = strLclPth.Left(strLclPth.ReverseFind('\\')+1)+ TEXT("config\\Common.ini");
	WritePrivateProfileString(TEXT(strTitle), TEXT(strName),bIsSound, strFlPth);
}
//游戏官网
void CPlazaDlg::OnBnClickedVisitWeb()
{
	tagGlobalUserData & UserData=g_GlobalUnits.GetGolbalUserData();
	CString strUrl("");
	strUrl.Format(CInitData::s_cLoginHomePage, UserData.dwUserID, UserData.szKey);
	ShellExecute(NULL,"open",strUrl,"",NULL,SW_SHOW);
}
//帐号管理
void CPlazaDlg::OnBnClickedAccountsManage()
{
	tagGlobalUserData & UserData=g_GlobalUnits.GetGolbalUserData();
	CString strUrl("");
	strUrl.Format(CInitData::s_cLoginAccountsManage, UserData.dwUserID, UserData.szKey);
	ShellExecute(NULL,"open",strUrl,"",NULL,SW_SHOW);
}
//游戏帮助
void CPlazaDlg::OnBnClickedGameHelp()
{
	ShellExecute(NULL,"open",CInitData::s_cGameHelp,"",NULL,SW_SHOW);
}
//问题反馈
void CPlazaDlg::OnBnClickedQuestionFeedBack()
{
	tagGlobalUserData & UserData=g_GlobalUnits.GetGolbalUserData();
	CString strUrl("");
	strUrl.Format(CInitData::s_cErrorTickling, UserData.dwUserID, UserData.szKey);
	ShellExecute(NULL,"open",strUrl,"",NULL,SW_SHOW);
}
//在线客服
void CPlazaDlg::OnBnClickedContactServise()
{
	ShellExecute(NULL,"open",CInitData::s_cContactService,"",NULL,SW_SHOW);
}
//更新用户信息
void CPlazaDlg::OnBnClickedUpdateUserInfo()
{
	m_btnUpdateUserInfo.EnableWindow(FALSE);
	SetTimer(TIMER_UPDATEUSERINFO_OPERATE, 60000, NULL);//限制一分钟点击一次
	UpdateUserInfo();
}
void CPlazaDlg::UpdateUserInfo()
{
	//连接服务器
	m_bIsUpdateUserInfo=true;
	OnCommandConnect();
}
//游戏列表
void CPlazaDlg::OnBnClickedGameList()
{
	SetShowHideInfo(1);//显示隐藏
	SwitchTitle(0);//切换标题
	//设置热门游戏标志位不显示
	HideFlag();
}
//比赛专区
void CPlazaDlg::OnBnClickedTounament()
{
	SetShowHideInfo(0);//显示隐藏
	SwitchTitle(1);//切换标题
	//设置热门游戏标志位不显示
	HideFlag();
}
//帐号充值
void CPlazaDlg::OnBnClickedAccountsRecharge()
{
	LoadPlazaGameWeb(1, 2);
}
//奖品兑换
void CPlazaDlg::OnBnClickedPrizeExcharge()
{
	//LoadPlazaGameWeb(2, 3);
	tagGlobalUserData UserData=g_GlobalUnits.GetGolbalUserData();
	CString strUrl("");
	strUrl.Format(CInitData::s_cLoginExchange, UserData.dwUserID, UserData.szKey);
	ShellExecute(NULL,"open",strUrl,"",NULL,SW_SHOW);
}
//道具商城
void CPlazaDlg::OnBnClickedPropsShop()
{
	LoadPlazaGameWeb(3, 4);
}
//新手任务
void CPlazaDlg::OnBnClickedNoviceTask()
{
	//LoadPlazaGameWeb(4, 5);

#if 0
	m_pNewHandTask = new CNewHandTask;
	if (!m_pNewHandTask->Create(IDD_NEWHANDTASK, this))
		return;
	m_pNewHandTask->ShowWindow(SW_SHOW);
#else
	CNewHandTask newhandtask;
	newhandtask.DoModal();
#endif
}
//背包
void CPlazaDlg::OnBnClickedBackPack()
{
	LoadPlazaGameWeb(5, 13);
}
//用户信息
void CPlazaDlg::OnBnClickedUserInfo()
{
	LoadPlazaGameWeb(6, 10);
}
//银行
void CPlazaDlg::OnBnClickedBank()
{
	LoadPlazaGameWeb(7, 12);
}
//邮件
void CPlazaDlg::OnBnClickedEmailMessage()
{
	LoadPlazaGameWeb(8, 11);
}
//返回按钮
void CPlazaDlg::OnBnClickedRollBack()
{
	SetShowHideInfo(1);//显示隐藏
	SwitchTitle(0);//切换标题
	//设置热门游戏标志位不显示
	HideFlag();
}
//下一页按钮
void CPlazaDlg::OnBnClickedNextPage()
{
}
//隐藏热门游戏标志
void CPlazaDlg::HideFlag()
{
	m_GameListHot.SetFlag(false);
	m_GameListHot.SetInvalidateRect();
}
//切换标题
void CPlazaDlg::SwitchTitle(WORD wPos)
{
	m_wTitlePos=wPos;
	SetInvalidateRect();
}
//创建嵌套网页::wWebMode页面样式,wTitlePos标题位置
void CPlazaDlg::LoadPlazaGameWeb(WORD wWebMode,WORD wTitlePos)
{
	bool bReturn=false;
	bReturn = m_PlazaGameWeb.LoadPlazaGameWeb(wWebMode);
	if (bReturn)
	{
		SetShowHideInfo(2);//显示隐藏
		SwitchTitle(wTitlePos);//切换标题
		//设置热门游戏标志位不显示
		HideFlag();
	}
}

BOOL CPlazaDlg::CheckGameSoftHash(LPCTSTR pNewHash)
{
	CString strLclPth, strFlPth;
	TCHAR chResult[MAX_PATH],chResultURL[MAX_PATH];	
	TCHAR szNamePath[MAX_PATH]=TEXT(""); //本地配置文件路径
	memset(chResult, 0, sizeof(chResult));
	GetModuleFileName(NULL, strLclPth.GetBufferSetLength(256), 256);

	_snprintf(szNamePath, sizeof(szNamePath), TEXT("%sconfig\\update.ini"), strLclPth.Left(strLclPth.ReverseFind('\\')+1));
	GetPrivateProfileString(_T("UpdateAllFile"), _T("GameHaseList"), _T(""), chResult, MAX_PATH, szNamePath);
	GetPrivateProfileString(_T("UpdateAllFile"), _T("URL"), _T("http://www.cbwgame.com/update/hallfile/"), chResultURL, MAX_PATH, szNamePath);

	BOOL bReturn = TRUE;
	if (0 == lstrcmp(pNewHash, chResult))
	{
		bReturn = FALSE;
	}
	else if (32 == lstrlen(pNewHash))
	{
		bReturn = TRUE;
	}
	else
	{
		m_GameFlash.Play(false);
		CString strBuffer;
		strBuffer.Format("请联系客服！游戏版本校验失败！并且将此提示截图发给客服，谢谢合作：\r\n远程：[%s]!=本地：[%s](%ld!=32)", pNewHash, chResult, lstrlen(pNewHash));
		ShowMessageBox(strBuffer, MB_ICONSTOP);
		bReturn = FALSE;
	}

	return bReturn;
}

BOOL CPlazaDlg::CallExternalUpdateProgram()
{
	// 构造命令行
	CString strCommonLine;	
	char acBuffer[MAX_PATH] = {0};
	GetModuleFileName(AfxGetInstanceHandle(), acBuffer, sizeof(acBuffer));
	char *pBackSlash = strrchr(acBuffer, '\\');
	TCHAR szPassword[33];
	if (2 == g_GlobalUnits.GetApplicationCmdLine().GetSize())
	{
		_snprintf(szPassword, sizeof(szPassword), "%s", g_GlobalUnits.GetApplicationCmdLine().GetAt(1));
	}
	else
	{
		CMD5Encrypt::EncryptData(m_pDlgLogon->m_szPassword, szPassword);
	}
	strCommonLine.Format(TEXT("%s /modulefilename:%s /account:%s /pw:%s"), UPDATE_PROJECT_NAME, ++pBackSlash, m_pDlgLogon->m_szAccounts, szPassword);

	// 启动游戏客户端
	STARTUPINFO StartInfo;
	memset(&StartInfo,0,sizeof(StartInfo));
	StartInfo.cb=sizeof(StartInfo);
	StartInfo.wShowWindow=SW_SHOWMAXIMIZED;
	BOOL bSuccess = FALSE;
	PROCESS_INFORMATION m_GameProcessInfo;
	memset(&m_GameProcessInfo, 0, sizeof(m_GameProcessInfo));

	try
	{
		bSuccess = CreateProcess(NULL,strCommonLine.GetBuffer(),NULL,NULL,FALSE,CREATE_DEFAULT_ERROR_MODE,NULL,NULL,&StartInfo,&m_GameProcessInfo);
		if (FALSE == bSuccess)
			throw 0;
	}
	catch (int iResult)
	{
		if (0 == iResult)
		{
			CString strBuffer;
			DWORD dwError = GetLastError();
			if (740 == dwError)
			{
				TCHAR tchBuffer[MAX_PATH] = {0}; 
				GetModuleFileName(AfxGetInstanceHandle(),tchBuffer,sizeof(tchBuffer));
				strBuffer.Format("不能执行文件：\r\n%s\r\n\r\nCreateProcess 失败 代码 %d。\r\n请求的操作需要提升。\r\n\r\n解决方案：\r\n可尝试右键“以管理员身份运行”。", tchBuffer, dwError);
				AfxMessageBox(strBuffer, MB_OK|MB_ICONSTOP);
			}
		}
	}
	catch (...) 
	{ 
		DWORD dwError = GetLastError();
		ASSERT(0); 
		return FALSE;
	}
	strCommonLine.ReleaseBuffer();

	if (FALSE == bSuccess)
	{
		DWORD dwError = GetLastError();
		return FALSE;
	}

	return TRUE;
}

//设置无效
void CPlazaDlg::SetInvalidateRect()
{
	InvalidateRect(m_rcTitleArea,FALSE);
}
//隐藏
void CPlazaDlg::SetShowHideInfo(WORD dwType)
{
	m_btnRollback.ShowWindow(SW_HIDE);
	m_btnNextPage.ShowWindow(SW_HIDE);
	switch (dwType)
	{
	case 0:
		{
			m_RoomListCenterDlg.ShowWindow(SW_HIDE);
			m_PlazaGameWeb.ShowWindow(SW_HIDE);
			m_GameListCenterDlg.ShowWindow(SW_HIDE);
			break;
		}
	case 1:
		{//游戏列表
			m_RoomListCenterDlg.ShowWindow(SW_HIDE);
			m_PlazaGameWeb.ShowWindow(SW_HIDE);
			m_GameListCenterDlg.ShowWindow(SW_SHOW);
			break;
		}
	case 2:
		{//大厅嵌套网页
			m_GameListCenterDlg.ShowWindow(SW_HIDE);
			m_RoomListCenterDlg.ShowWindow(SW_HIDE);
			m_PlazaGameWeb.ShowWindow(SW_SHOW);
			break;
		}
	case 3:
		{//房间列表
			m_GameListCenterDlg.ShowWindow(SW_HIDE);
			m_PlazaGameWeb.ShowWindow(SW_HIDE);
			m_RoomListCenterDlg.ShowWindow(SW_SHOW);
			break;
		}
	}
}
//新手指引
void CPlazaDlg::OnBnClickedNewComerGuide()
{
	KillTimer(IDI_ITMER_MEWCOMER_GUIDE);
	m_wNewComerGuideActionNumber = 1;
	m_bNewComerGuide=false;
	InvalidateRect(m_rcNewComerGuideArea);
	DoModalNewComerGuide();
}
//创建新手指引窗口
void CPlazaDlg::DoModalNewComerGuide()
{
	CNewComerGuide newComerGuide;
	newComerGuide.DoModal();
}
//启动四色球
bool CPlazaDlg::StartSiSeQiuProgram()
{
	tagGlobalUserData & UserData = g_GlobalUnits.GetGolbalUserData();
	// 构造命令行
	CString strCommonLine;	
	strCommonLine.Format(TEXT("%s *%d *%s *%s *%d"), "SiSeQiu.exe", UserData.dwUserID, UserData.szPassWord, UserData.szKey, g_GlobalOption.m_byNetWorkTag);
	// 启动游戏客户端
	STARTUPINFO StartInfo;
	memset(&StartInfo,0,sizeof(StartInfo));
	StartInfo.cb=sizeof(StartInfo);
	StartInfo.wShowWindow=SW_SHOWMAXIMIZED;
	BOOL bSuccess = FALSE;
	PROCESS_INFORMATION m_GameProcessInfo;
	memset(&m_GameProcessInfo, 0, sizeof(m_GameProcessInfo));

	try
	{
		bSuccess = CreateProcess(NULL,strCommonLine.GetBuffer(),NULL,NULL,FALSE,CREATE_DEFAULT_ERROR_MODE,NULL,NULL,&StartInfo,&m_GameProcessInfo);
		if (FALSE == bSuccess)
			throw 0;
	}
	catch (int iResult)
	{
		if (0 == iResult)
		{
			CString strBuffer;
			DWORD dwError = GetLastError();
			if (740 == dwError)
			{
				TCHAR tchBuffer[MAX_PATH] = {0}; 
				GetModuleFileName(AfxGetInstanceHandle(),tchBuffer,sizeof(tchBuffer));
				strBuffer.Format("不能执行文件：\r\n%s\r\n\r\nCreateProcess 失败 代码 %d。\r\n请求的操作需要提升。\r\n\r\n", tchBuffer, dwError);
				AfxMessageBox(strBuffer, MB_OK|MB_ICONSTOP);
			}
		}
	}
	catch (...) 
	{ 
		DWORD dwError = GetLastError();
		ASSERT(0); 
		return FALSE;
	}
	strCommonLine.ReleaseBuffer();

	if (FALSE == bSuccess)
	{
		DWORD dwError = GetLastError();
		return FALSE;
	}
	this->ShowWindow(SW_HIDE);
	
	SetMusicSound(false);//是否播放声音
	
	createThreads();

	return TRUE;

}
// 建立单线程
bool CPlazaDlg::createThreads()
{
	if(m_hEventGameLoadOver[0])
	{
		m_hThread = (HANDLE)_beginthreadex( NULL, 0, &thread_game_close, (void*)this, 0, &m_dwThreadID);

		if(!m_hThread)
		{
			printf("建立线程失败 \n");
			return false;
		}
	}
	else
	{
		return false;
	}

	return true;
}
//线程工作函数(四色球客户端关闭,弹出大厅)
unsigned __stdcall  CPlazaDlg::thread_game_close(void* pParam) 	
{
	CPlazaDlg* pThis = (CPlazaDlg*)pParam;
	DWORD  iWaitTime = 7200000;	// ms;最长的等待时间.
	DWORD dwResult;

	dwResult = WaitForMultipleObjects(1,  pThis->m_hEventGameLoadOver,  FALSE,  iWaitTime);	
	switch(dwResult)
	{
	case WAIT_TIMEOUT:			// 超时
	case WAIT_OBJECT_0 +0:	    // 接到四色球客户端关闭的通知
		{
			if (pThis && pThis->GetSafeHwnd())
			{
				pThis->ShowWindow(SW_SHOW);		// 显示大厅
				pThis->UpdateUserInfo();//更新用户信息
			}
			break;
		}
	}
	return 0;
}

bool CPlazaDlg::StartActivity(WORD wID)
{
	if (NULL != m_pActivity)
	{
		SafeDelete(m_pActivity);
	}

	m_pActivity = new CActive;
	if (NULL != m_pActivity)
	{
		int iIndex = 1;
		CString	sFileName("");
		CString sFilePath("");
		CString sFieldName("");
		TCHAR tchURL[MAX_PATH]=_T("");
		GetModuleFileName(NULL, sFileName.GetBufferSetLength(MAX_PATH), MAX_PATH);
		sFilePath=sFileName.Left(sFileName.ReverseFind('\\') + 1) + TEXT("config\\roomlist.ini");

		for (int iIndex = 1; ; iIndex++)
		{
			sFieldName.Format(_T("Active_%d"), iIndex);
			WORD wIniID = GetPrivateProfileInt(sFieldName, TEXT("ServerID"), -1, sFilePath);
			if (0 >= wIniID)
			{
				break;
			}

			if (wID == wIniID)
			{
				TCHAR tchReturnVal[MAX_PATH]=_T("");
				GetPrivateProfileString(sFieldName, _T("Url"), _T(""), tchReturnVal, sizeof(tchReturnVal), sFilePath);

				if (0 < lstrlen(tchReturnVal))
				{
					tagGlobalUserData UserData=g_GlobalUnits.GetGolbalUserData();
					_snprintf(tchURL, sizeof(tchURL), tchReturnVal, UserData.dwUserID, UserData.szKey);
					break;
				}
			}
		}

		m_pActivity->Create(IDD_ACTIVE, this);
		m_pActivity->SetURL(tchURL);
		m_pActivity->ShowWindow(SW_SHOW);
		return true;
	}

	return false;
}

BOOL CPlazaDlg::IsCanIntervalConnect()
{
	if (GetTickCount() < m_dwLastConnectTime)
		m_dwLastConnectTime = GetTickCount();
	
	TRACE3("间隔时间设置：%ld, %ld, %ld \r\n", GetTickCount(),  m_dwLastConnectTime, GetTickCount() - m_dwLastConnectTime);
	if (m_dwConnectIntervalTime*1000 >= GetTickCount() - m_dwLastConnectTime)
	{
		return FALSE;
	}

	m_dwLastConnectTime = GetTickCount();
	return TRUE;
}