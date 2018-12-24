#include "Stdafx.h"
#include "Resource.h"
#include "DlgLogon.h"
#include "DlgOption.h"
#include "Mmsystem.h"
#include "GlobalUnits.h"
#include "RoomViewItem.h"
#include "PlazaDlg.h"
#include "webpath.h"
#include "InitData.h"
#include ".\roomviewitem.h"

//#include "TableListDlg.h"

//////////////////////////////////////////////////////////////////////////

//按钮区域
#define	BUTTON_AREA_WIDTH			200									//按钮区域


//当前排名
#define GAME_USER_CURRENT_SORT_W		10000							//万名



#define IDI_GAME_FRAME_IS_EXIST			20001							//检测客户端是否存在
#define IDI_REQUEST_SERVER_EXIT_MSG		20002							//请求服务退出
#define IDI_LOADING_ANIMACTION			20003							//Loading动画

#define TIMER_GAMEFRAME_EXIST			10000							//
#define TIMER_REQUEST_SERVER_INTER		10000							

//////////////////////////////////////////////////////////////////////////

//启动结果
#define	SR_CREATE_ERROR				0									//启动错误
#define	SR_CREATE_SUCCESS			1									//启动成功
#define	SR_ALREADY_EXIST			2									//已经存在

//////////////////////////////////////////////////////////////////////////
//菜单命令

//常规菜单
#define	IDM_UM_WISPER				WM_USER+100							//私聊菜单
#define	IDM_UM_COPY_NAME			WM_USER+101							//拷贝名字
#define	IDM_UM_SET_CHAT				WM_USER+102							//设置聊天
#define	IDM_UM_CANECL_CHAT			WM_USER+103							//取消聊天
#define	IDM_UM_LOOK_LOCATION		WM_USER+104							//查看位置
#define	IDM_UM_INVITE_PLAY			WM_USER+105							//邀请游戏
#define	IDM_UM_SEARCH_USER			WM_USER+106							//寻找用户
#define	IDM_UM_SET_FRIEND			WM_USER+107							//设为好友
#define	IDM_UM_SET_DETEST			WM_USER+108							//设为厌恶

//管理命令
#define IDM_UM_MANAGER_USER			WM_USER+150							//权限管理
#define IDM_UM_ISSUE_MESSAGE		WM_USER+151							//发布消息
#define IDM_IM_SYSTEM_OPTION		WM_USER+152							//系统设置
#define IDM_UM_SEND_WARNNING		WM_USER+153							//发送警告
#define IDM_UM_LOOK_USER_IP			WM_USER+154							//查看地址
#define IDM_UM_CUT_USER				WM_USER+155							//用户下线
#define IDM_UM_LIMIT_ACCOUN			WM_USER+156							//禁止帐户
#define IDM_UM_EXIT_TABLE			WM_USER+157							//退出桌子
#define IDM_UM_DISABLE_USER_IP		WM_USER+158							//禁用玩家IP


//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CRoomViewItem, CDialog)
	ON_WM_TIMER()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_COPYDATA()
	ON_WM_ERASEBKGND()
	ON_WM_SHOWWINDOW()
	ON_WM_LBUTTONDOWN()
	ON_WM_WINDOWPOSCHANGING()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_SETCURSOR()
	ON_WM_DESTROY()
	ON_WM_MOUSEMOVE()
	ON_MESSAGE(WM_HIT_EXPMESSTION,	OnHitExpression)
	ON_MESSAGE(WM_UPDATE_FACE, OnUpdateFace)
	ON_MESSAGE(WM_FLASH_CHAIR, OnFlashChair)

	//ON_BN_CLICKED(IDC_BANK,	OnBnClickedBank)
	//ON_BN_CLICKED(IDC_AUTO_SIT,	OnBnClickedAutoSit)
	//ON_BN_CLICKED(IDC_FIND_USER, OnBnClickedFindUser)
	//ON_BN_CLICKED(IDC_QUIT_ROOM,	OnBnClickedQuitRoom)
	ON_BN_CLICKED(IDB_SEND_MSG, OnBnClickedSendChat)
	ON_BN_CLICKED(IDC_COLOR_SET, OnBnClickedColorSet)
	ON_BN_CLICKED(IDB_FACE, OnBnClickedExpression)
	ON_BN_CLICKED(IDC_CLEAN_SCREEN,	OnBnClickedCleanScreen)
	//ON_BN_CLICKED(IDC_BUGLE,	OnBnClickedBugle)
	// 	ON_BN_CLICKED(IDC_CLOSE,OnBnClose)
	// 	ON_BN_CLICKED(IDC_RETURN,OnBnReturn)
	ON_BN_CLICKED(IDC_PGDWON,OnBnDwon)
	ON_BN_CLICKED(IDC_PGUP,OnBnUp)
	// 	ON_NOTIFY(NM_DBLCLK, IDC_USER_LIST, OnNMDblclkUserList)
	// 	ON_NOTIFY(NM_RCLICK, IDC_USER_LIST, OnNMRclickUserList)
	// 	ON_NOTIFY(NM_CLICK, IDC_USER_LIST, OnNMclickUserList)
	ON_BN_CLICKED(IDC_AUTO_QUEUE, OnAutoQueue)

	// 	ON_BN_CLICKED(IDC_BT_CLOSE,&CRoomViewItem::OnBnClickedClose)
	// 	ON_BN_CLICKED(IDC_BT_RETURN,&CRoomViewItem::OnBnClickedReturn)
	// 	ON_BN_CLICKED(IDC_BT_RE_LOGON,&CRoomViewItem::OnBnClickedReLogon)
	// 	ON_BN_CLICKED(IDC_BT_SET,&CRoomViewItem::OnBnClickedSet)
	// 	ON_BN_CLICKED(IDC_MINSIZE,&CRoomViewItem::OnBnClickedMinsize) 
	ON_BN_CLICKED(IDB_BT_FRIEND,&CRoomViewItem::OnBnClickedFindUser) 

	ON_COMMAND(IDM_CANCEL_CONNECT,	OnCommandCancelConnect)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

CRoomViewItem::CRoomViewItem() : CDialog(IDD_GAME_ROOM)
,m_hGameLoadOver(m_hEventGameLoadOver[0])
, m_hClosePromtWndr(m_hEventGameLoadOver[1])
{
	//辅助变量
	m_wGameGenre=0;
	m_dwVideoAddr=0L;
	m_bRectifyHor=false;
	m_bRectifyVor=false;
	m_bInitDialog=false;
	m_bHideUserList=false;
	m_bQueueState=true;
	m_dwLimitChatTimes=0;
	m_dwMenuUserID=0L;
	m_wReqTableID=INVALID_TABLE;
	m_wReqChairID=INVALID_CHAIR;
	m_wFindTableID=INVALID_TABLE;
	m_nListIndex=0;
	memset(&m_ListColumnInfo,0,sizeof(m_ListColumnInfo));
	m_QueueNum=0;
	m_bLogonFull=false;
	m_btExitTag = 0;
	m_bRequestServerExit = false;

	//房间配置
	m_cbHideUserInfo=FALSE;
	//m_gameListManager = NULL;

	//配置变量
	m_pGameOption=NULL;
	m_pServerOption=NULL;

	//聊天用户
	m_dwChatUserID=0L;
	m_szChatName[0]=0;

	//进程信息
	m_szShareName[0]=0;
	m_hWndChannel=NULL;
	m_hShareMemory=NULL;
	m_pShareMemory=NULL;
	memset(&m_GameProcessInfo,0,sizeof(m_GameProcessInfo));
	::memset(&m_tagGameFrameStatus, 0, sizeof(m_tagGameFrameStatus));

	//数据缓冲
	m_wPacketTableID=INVALID_TABLE;
	m_wPacketChairID=INVALID_CHAIR;

	//设置变量
	m_pMeUserItem=NULL;
	m_pListServer=NULL;
	m_ServiceStatus=ServiceStatus_Null;
	memset(&m_OptionBuffer,0,sizeof(m_OptionBuffer));
	m_pFindUserDlg=NULL;

	//cxfm_dwWinGemsToday = 0;
	//ZeroMemory(&m_ptOrder, sizeof(m_ptOrder));
	//ZeroMemory(&m_ptWinGemsToday, sizeof(m_ptWinGemsToday));


	//初始化公告信息
	m_strSaveGameAffiche.clear();

	m_memdc.m_hDC = NULL;

	//cxf 11-26 m_RoomViewAffiche = NULL;

	m_wAnimAction=0;

	m_bIsDestroyStatus = false;

	m_wCloseState=0;
	
	// add by HouGuoJiang 2011-11-25
	memset(m_hEventGameLoadOver, 0, sizeof(m_hEventGameLoadOver));
	m_strGameLoadOver = "MY_GAME_LOAD_OVER";	// 注意：斗地主游戏中的这个值要与此处一致
	m_hEventGameLoadOver[0] = CreateEvent(NULL, FALSE, FALSE, m_strGameLoadOver);
	m_hEventGameLoadOver[1] = CreateEvent(NULL, FALSE, FALSE, NULL);

	//Author<cxf>;DateTime<2012-03-01>;Description<屏蔽>
	//m_bMasterRight=false;//Author<cxf>;DateTime<2012-02-27>

	return;
}

CRoomViewItem::~CRoomViewItem()
{

	//清理内存
	m_fontGameTitleInfo.DeleteObject();

	/*cxf 11-26 if (NULL != m_RoomViewAffiche)
	{
	m_RoomViewAffiche->DestroyWindow();
	delete m_RoomViewAffiche;
	m_RoomViewAffiche = NULL;
	}*/
	if (m_pShareMemory!=NULL)
	{
		UnmapViewOfFile(m_pShareMemory);
		m_pShareMemory=NULL;
	}
	if (m_hShareMemory!=NULL)
	{
		CloseHandle(m_hShareMemory);
		m_hShareMemory=NULL;
	}

	/*
	for (INT_PTR i=0;i<m_ShortMessage.GetCount();i++) 
	{
	delete ((CShortMessage *)m_ShortMessage.GetAt(i));
	m_ShortMessage.SetAt(i,NULL);
	}
	*/

	SafeDelete(m_pFindUserDlg);
	//m_PropertyBar.DestroyBar();

	return;
}

//接口查询
void * __cdecl CRoomViewItem::QueryInterface(const IID & Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IViewItem,Guid,dwQueryVer);
	QUERYINTERFACE(IRoomViewItem,Guid,dwQueryVer);
	//QUERYINTERFACE(IPurchaseInfo,Guid,dwQueryVer);
	QUERYINTERFACE(ITCPSocketSink,Guid,dwQueryVer);
	QUERYINTERFACE(ITableFrameSink,Guid,dwQueryVer);
	QUERYINTERFACE(IUserManagerSink,Guid,dwQueryVer);
	QUERYINTERFACE(IChannelMessageSink,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IViewItem,Guid,dwQueryVer);
	return NULL;
}

//房间类型
WORD CRoomViewItem::GetGameGenre()
{
	return m_wGameGenre;
}

//枚举玩家
tagUserData * CRoomViewItem::EnumLookonUser(WORD wIndex)
{
	return NULL;
}

//查找用户
tagUserData * CRoomViewItem::SearchUserItem(DWORD dwUserID)
{
	IUserItem * pIUserItem=m_ClientUserManager.SearchUserByUserID(dwUserID);

	return (pIUserItem!=NULL)?(pIUserItem->GetUserData()):NULL;
}

//查找用户
tagUserData * CRoomViewItem::SearchUserItem(LPCTSTR pszUserName)
{
	WORD wEnumIndex=0;
	while (true)
	{
		//获取用户
		IUserItem * pIUserItem=m_ClientUserManager.EnumUserItem(wEnumIndex++);
		if (pIUserItem==NULL) break;
		tagUserData * pUserData=pIUserItem->GetUserData();
		if(pUserData==NULL)continue;

		//比较名字
		CString strName,strFineUser;
		strName.Format(TEXT("%s"),pUserData->szName);
		strFineUser.Format(TEXT("%s"),pszUserName);
		if (strName==strFineUser) return pUserData;
	};
	return NULL;
}

//获取玩家
const tagUserData * CRoomViewItem::GetUserInfo(WORD wChairID)
{
	return NULL;
}

//获取自己
const tagUserData * CRoomViewItem::GetMeUserInfo()
{
	return (m_pMeUserItem!=NULL)?(m_pMeUserItem->GetUserData()):NULL;
}

//发送信息
void CRoomViewItem::SendData(WORD wMainCmdID, WORD wSubCmdID, void * pData, WORD wDataSize)
{
	m_ClientSocket->SendData(wMainCmdID, wSubCmdID, pData, wDataSize);

	return;
}

//控件绑定
void CRoomViewItem::DoDataExchange(CDataExchange * pDX)
{
	__super::DoDataExchange(pDX);
	//DDX_Control(pDX, IDC_BUGLE, m_btBugle);
	////DDX_Control(pDX, IDC_PHRASE, m_btPhrase);
	//DDX_Control(pDX, IDC_COLOR_SET, m_btColorSet);
	//DDX_Control(pDX, IDC_SEND_CHAT, m_btSendChat);
	//DDX_Control(pDX, IDC_EXPRESSION, m_btExpression);
	//DDX_Control(pDX, IDC_CLEAN_SCREEN, m_btCleanScreen);
	//DDX_Control(pDX, IDC_CHAT_INPUT, m_ChatInput);
	//DDX_Control(pDX, IDC_USER_LIST, m_UserListView);
	//DDX_Control(pDX, IDC_VOR_SPLITER, m_VorSplitter);
	//DDX_Control(pDX, IDC_CHAT_MESSAGE, m_ChatMessage);
	//DDX_Control(pDX, IDC_AUTO_SIT, m_btAutoSit);
	//DDX_Control(pDX, IDC_BANK,m_btBank);
	//DDX_Control(pDX, IDC_FIND_USER, m_btFindUser);
	//DDX_Control(pDX, IDC_CHAT_OBJECT, m_ChatObject);
	//DDX_Control(pDX, IDC_QUIT_ROOM, m_btQuitRoom);
	//DDX_Control(pDX, IDC_HOR_SPLITER, m_HorSplitter);
	// 	DDX_Control(pDX,IDC_BT_RE_LOGON,m_btnReLogon);
	// 	DDX_Control(pDX,IDC_BT_RETURN,m_btnReturn);
	// 	DDX_Control(pDX,IDC_BT_SET,m_btnSet);
	// 	DDX_Control(pDX,IDC_BT_CLOSE,m_btnClose);  
	// 	DDX_Control(pDX,IDC_MINSIZE,m_btnMinsize); 
	// 	DDX_Control(pDX,IDB_BT_DEPOSITORY,m_btnDepository);
	// 	DDX_Control(pDX,IDB_BT_FRIEND,m_btnFriend);
	// 	DDX_Control(pDX,IDB_BT_SHOP,m_btnShop);
	// 	DDX_Control(pDX,IDB_BT_TASK,m_btnTask);
	// 	DDX_Control(pDX,IDB_BTN_MY_TOOL,m_btnTool);
	DDX_Control(pDX,IDC_PGDWON,m_btnPgDn);
	DDX_Control(pDX,IDC_PGUP,m_btnPgUp);
	DDX_Control(pDX,IDC_RICHEDIT2_OUTPUT,m_ChatMessage);
	DDX_Control(pDX,IDC_EDIT_INPUT,m_editInput);
	DDX_Control(pDX,IDC_COMBO1,m_ChatObject);
	DDX_Control(pDX,IDB_FACE,m_btExpression);
	DDX_Control(pDX,IDB_SEND_MSG,m_btSendChat);
	//DDX_Control(pDX,IDB_BT_DESPOKEN_TABLE,m_btnDespokenTable);
}
void CRoomViewItem::OnOK()
{
	//AfxMessageBox(TEXT("CRoomViewItem::OnOK"));
}
//初始化函数
BOOL CRoomViewItem::OnInitDialog()
{
	__super::OnInitDialog();

	// add by HouGuoJiang 2011-11-25
	// 建立单线程，响应游戏的初始化完毕事件以便于隐藏提示窗口
	// 针对的问题：提示窗口隐藏太快了，游戏加载还未完成，进程如同突然消失了，客户不知所措
	bool bResult = createThreads();

	//SetWindowLong(this->GetSafeHwnd(), GWL_EXSTYLE, GetWindowLong(this->GetSafeHwnd(), GWL_EXSTYLE)^0x80000);
	//设置控件
	//m_UserListView.InitUserListView();
	//m_VorSplitter.SetSplitterSink(this);
	//m_HorSplitter.SetSplitterSink(this);

	//m_ChatInput.LimitText(MAX_CHAT_LEN-1);
	//m_ChatMessage.SetBackgroundColor(FALSE,RGB(230,249,255));

	//创建控件
	//m_BrowerAD.Create(NULL,NULL,WS_VISIBLE|WS_CHILD,CRect(0,0,0,0),this,100,NULL);
	//m_BrowerAD.Navigate(TEXT("http://qp.foxuc.net/AD/GameRoomAD.asp"),NULL,NULL,NULL,NULL);

	//道具控件
	// 	m_PropertyBar.SetSkinResource(GetResInstanceHandle(),&g_GlobalUnits.m_PropertyViewImage);
	// 	m_PropertyBar.CreateBar(this);
	// 	m_PropertyBar.SetPurchaseInfoSink(this);

	////建立提示控件
	//m_ToolTipCtrl.Create(this);
	//m_ToolTipCtrl.Activate(TRUE);
	//m_ToolTipCtrl.AddTool(GetDlgItem(IDC_AUTO_SIT),TEXT("自动寻找游戏位置"));
	//m_ToolTipCtrl.AddTool(GetDlgItem(IDC_FIND_USER),TEXT("查找用户"));
	//m_ToolTipCtrl.AddTool(GetDlgItem(IDC_QUIT_ROOM),TEXT("退出房间"));
	//m_ToolTipCtrl.AddTool(GetDlgItem(IDC_EXPRESSION),TEXT("表情"));
	//m_ToolTipCtrl.AddTool(GetDlgItem(IDC_COLOR_SET),TEXT("颜色"));
	//m_ToolTipCtrl.AddTool(GetDlgItem(IDC_CLEAN_SCREEN),TEXT("清屏"));
	//m_ToolTipCtrl.AddTool(GetDlgItem(IDC_SEND_CHAT),TEXT("发送"));
	//m_ToolTipCtrl.AddTool(GetDlgItem(IDC_BUGLE),TEXT("小喇叭"));

	////聊天短语
	//m_ChatInput.AddString(TEXT("快点吧我等的花儿也谢了"));
	//m_ChatInput.AddString(TEXT("大家好，很高兴见到各位！"));
	//m_ChatInput.AddString(TEXT("怎么又断线了，网络太差了..."));
	//m_ChatInput.AddString(TEXT("不要走，决战到天亮。"));
	//m_ChatInput.AddString(TEXT("和您合作真是太愉快了..."));
	//m_ChatInput.AddString(TEXT("我们交个朋友，能告诉我您的联系方式吗？"));
	//m_ChatInput.AddString(TEXT("您是MM,还是GG?"));
	//m_ChatInput.AddString(TEXT("快点，花儿都凋谢了！"));
	//m_ChatInput.AddString(TEXT("痛快，痛快，来十斤牛肉，八碗好酒！"));
	//m_ChatInput.AddString(TEXT("不要吵了，有什么好吵的..."));
	//m_ChatInput.AddString(TEXT("再见了，我会想念大家的！BYE"));

	////聊天对象
	//m_ChatObject.AddString(TEXT("所有人"));
	//m_ChatObject.SetCurSel(0);

	//cxf
	CString strPath;
	GetImagePlazaPath(strPath);
	bool bAdministration = IsAdministrationHavePurview();

	if (bAdministration)
	{
		m_ImageBack.LoadImage(TEXT(strPath+"\\Tablelist\\table_bg.bmp"));
		//m_ImageBackGM.LoadImage(TEXT(strPath+"GM_TABLE_LIST.png"));

		m_btnPgDn.ShowWindow(SW_SHOW);
		m_btnPgUp.ShowWindow(SW_SHOW);
	}
	else 
	{
		m_ImageBack.LoadImage(TEXT(strPath+"ImageLoading.png"));
		m_ImgAnimAction.LoadImage(TEXT(strPath+"ImageLoadingAnimation.png"));
		m_ImgClose.LoadImage(TEXT(strPath+"BTCloseTableList.png"));

		m_btnPgDn.ShowWindow(SW_HIDE);
		m_btnPgUp.ShowWindow(SW_HIDE);
	}

	HINSTANCE hInstance=g_GlobalUnits.m_PlatformResourceModule->GetResInstance();	
	// 	m_btnReLogon.LoadRgnImage(hInstance,g_GlobalUnits.m_PlatformFrameImage.uBtFrameRelogon);
	// 	m_btnReturn.LoadRgnImage(hInstance,g_GlobalUnits.m_PlatformFrameImage.uBtFrameRetrue);
	// 	m_btnSet.LoadRgnImage(hInstance,g_GlobalUnits.m_PlatformFrameImage.uBtFrameOption);
	// 	m_btnClose.LoadRgnImage(hInstance,g_GlobalUnits.m_PlatformFrameImage.uBtFrameClose);
	// 	m_btnMinsize.LoadRgnImage(hInstance,g_GlobalUnits.m_PlatformFrameImage.uBtFrameMin);
	// 	m_btnDepository.LoadRgnImage(hInstance,g_GlobalUnits.m_PlatformFrameImage.uBtDepository);
	// 	m_btnFriend.LoadRgnImage(hInstance,g_GlobalUnits.m_PlatformFrameImage.uBtFriend);
	// 	m_btnShop.LoadRgnImage(hInstance,g_GlobalUnits.m_PlatformFrameImage.uBtShop);
	// 	m_btnTask.LoadRgnImage(hInstance,g_GlobalUnits.m_PlatformFrameImage.uBtTask);
	// 	m_btnTool.LoadRgnImage(hInstance,g_GlobalUnits.m_PlatformFrameImage.uBtTool);

	//cxf 1108 m_btExpression.LoadRgnImage(hInstance,g_GlobalUnits.m_ServerViewImage.uBtExpression);
	//cxf 1108 m_btSendChat.LoadRgnImage(hInstance,g_GlobalUnits.m_ServerViewImage.uBtChatChannel);

	//创建游戏公告窗口

	/*cxf 11-26 m_RoomViewAffiche = new CRoomViewAffiche();
	if (NULL == m_RoomViewAffiche)
	{
	AfxMessageBox("游戏公告窗口创建失败！");
	return false;
	}
	CRect wndRect(0, 0, 0, 0);
	m_RoomViewAffiche->Create(NULL,NULL,WS_VISIBLE|WS_CHILD|WS_CLIPSIBLINGS|WS_CLIPCHILDREN,wndRect,this,1929);
	m_RoomViewAffiche->SetWindowPos(NULL, 150, 5, 659, 34, SWP_SHOWWINDOW);*/


	//创建控件
	//cxf 1108 CRect CreateRect(0,0,0,0);
	//cxf 1108 m_AutoQueue.Create(TEXT(""),WS_CHILD|WS_VISIBLE,CreateRect,this,IDC_AUTO_QUEUE);

	//按钮设置
	m_bQueueState=true;
	hInstance=AfxGetInstanceHandle();
	//cxf 1108 m_AutoQueue.LoadRgnImage(hInstance,IDB_AUTO_QUEUE);
	//cxf 1108 m_AutoQueue.EnableWindow(FALSE);
	//m_btnDespokenTable.LoadRgnImage(hInstance, IDB_DESPOKEN_TABLE);
	m_btnPgDn.LoadRgnImage(hInstance, IDB_TABLE_LIST_PGDN);
	m_btnPgUp.LoadRgnImage(hInstance, IDB_TABLE_LIST_PGUP);
	//数字
	//cxf 1108 m_pngNumber.LoadImage(hInstance, TEXT("ROOM_NUMBER"));
	//cxf 1108 m_pngText1.LoadImage(hInstance, TEXT("ROOM_TEXT_1"));
	//cxf 1108 m_pngText2.LoadImage(hInstance, TEXT("ROOM_TEXT_2"));
	//后
	//cxf 1108 m_pngImageHou.LoadImage(hInstance, TEXT("ROOM_IMAGE_HOU"));
	//当前排名
	//cxf 1108 m_pngImageCurSort.LoadImage(hInstance, TEXT("ROOM_IMAGE_CURSORT"));
	//今日赢收
	//cxf 1108 m_pngImageDayWin.LoadImage(hInstance, TEXT("ROOM_IMAGE_DAYWIN"));

	m_ChatMessage.SetBackgroundColor(FALSE, RGB(193,187,163));
	m_editInput.SetBackgroundColor(FALSE, RGB(60,23,1));

	//cxf 2011-10-22
	//cxf 1108 m_AutoQueue.ShowWindow(SW_HIDE);
	

	CFont font;
	LOGFONT lf;
	memset(&lf, 0, sizeof(lf));
	lf.lfHeight=12;
	lstrcpy(lf.lfFaceName, TEXT("宋体"));   // Request a face name "Arial".
	VERIFY(font.CreateFontIndirect(&lf));  // Create the font.
	m_ChatMessage.SetFont(&font);
	m_editInput.LimitText(MAX_CHAT_LEN-1);
	m_editInput.SetFont(&font);
	font.DeleteObject();

	//游戏公告字体
	m_fontGameTitleInfo.CreateFont(20,0,0,0,400,0,0,0,134,3,2,1,2,TEXT("宋体"));

	CString strChatObject[] = {TEXT("喊话"), TEXT("私聊"), TEXT("公会")};
	for (int i=0; i<CountArray(strChatObject); i++)
	{
		m_ChatObject.AddString(strChatObject[i]);
	}
	m_ChatObject.SetCurSel(0);

	//设置位置
	CRect rcButton;

	//HDWP hDwp=BeginDeferWindowPos(32);
	//const UINT uFlags=SWP_NOACTIVATE|SWP_NOZORDER|SWP_NOCOPYBITS|SWP_NOSIZE;
	// // 	DeferWindowPos(hDwp,m_btnReLogon,NULL,821,680,0,0,uFlags);
	// // 	DeferWindowPos(hDwp,m_btnReturn,NULL,847,680,0,0,uFlags);
	// // 	DeferWindowPos(hDwp,m_btnMinsize,NULL,872,680,0,0,uFlags);
	// // 	DeferWindowPos(hDwp,m_btnSet,NULL,897,680,0,0,uFlags);
	// // 	DeferWindowPos(hDwp,m_btnClose,NULL,921,680,0,0,uFlags);
	// // 	DeferWindowPos(hDwp,m_btnFriend,NULL,292,29,0,0,uFlags);
	// // 	DeferWindowPos(hDwp,m_btnTool,NULL,423,29,0,0,uFlags);
	// // 	DeferWindowPos(hDwp,m_btnTask,NULL,554,29,0,0,uFlags);
	// // 	DeferWindowPos(hDwp,m_btnDepository,NULL,684,29,0,0,uFlags);
	// // 	DeferWindowPos(hDwp,m_btnShop,NULL,812,29,0,0,uFlags);
	// 	DeferWindowPos(hDwp,m_ChatMessage,NULL,19,511,263,161,SWP_NOACTIVATE|SWP_NOZORDER|SWP_NOCOPYBITS);
	// 	DeferWindowPos(hDwp,m_editInput,NULL,22,677,169,19,SWP_NOACTIVATE|SWP_NOZORDER|SWP_NOCOPYBITS);
	// 	DeferWindowPos(hDwp,m_btSendChat,NULL,194,676,0,0,uFlags);
	// 	DeferWindowPos(hDwp,m_btExpression,NULL,218,676,0,0,uFlags);
	// 	DeferWindowPos(hDwp,m_ChatObject,NULL,242,676,38,21,SWP_NOACTIVATE|SWP_NOZORDER|SWP_NOCOPYBITS);
	// 	DeferWindowPos(hDwp,m_AutoQueue,NULL,516,126,0,0,uFlags);
	// 	DeferWindowPos(hDwp,m_btnDespokenTable,NULL,516,177,0,0,uFlags);
	//EndDeferWindowPos(hDwp);


	//设置变量
	m_bInitDialog=true;
	//更新资源
	UpdateSkinResource();



	m_ToolTipCtrl.Create(this);
	m_ToolTipCtrl.Activate(TRUE);
	// 	m_ToolTipCtrl.AddTool(&m_btnDepository,TEXT("藏宝阁"));
	// 	m_ToolTipCtrl.AddTool(&m_btnFriend,TEXT("好友"));
	// 	m_ToolTipCtrl.AddTool(&m_btnShop,TEXT("商城"));
	// 	m_ToolTipCtrl.AddTool(&m_btnTool,TEXT("道具"));
	// 	m_ToolTipCtrl.AddTool(&m_btnTask,TEXT("任务"));
	// 	m_ToolTipCtrl.AddTool(&m_btnReLogon,TEXT("重新登陆"));
	// 	m_ToolTipCtrl.AddTool(&m_btnReturn,TEXT("返回"));
	// 	m_ToolTipCtrl.AddTool(&m_btnSet,TEXT("个人设置"));
	// 	m_ToolTipCtrl.AddTool(&m_btnClose,TEXT("关闭游戏大厅"));
	// 	m_ToolTipCtrl.AddTool(&m_btnMinsize,TEXT("最小化窗口"));
	//cxf 1108 m_ToolTipCtrl.AddTool(&m_btExpression,TEXT("聊天头像"));
	//cxf 1108 m_ToolTipCtrl.AddTool(&m_btSendChat,TEXT("发送"));
	//cxf 1108 m_ToolTipCtrl.AddTool(&m_AutoQueue,TEXT("自动排队"));
	//	m_ToolTipCtrl.AddTool(&m_btnDespokenTable,TEXT("预订桌子"));

	m_bIsDestroyStatus = false;
	m_wAnimAction=0;				//动画帧数
	//动画位置
	m_rcAnimAction.left=395;
	m_rcAnimAction.top=379;
	m_rcAnimAction.right=m_rcAnimAction.left+m_ImgAnimAction.GetWidth()/12;
	m_rcAnimAction.bottom=m_rcAnimAction.top+m_ImgAnimAction.GetHeight();

	m_rcCloseRect.left=908;
	m_rcCloseRect.top=8;
	m_rcCloseRect.right=m_rcCloseRect.left+m_ImgClose.GetWidth()/5;
	m_rcCloseRect.bottom=m_rcCloseRect.top+m_ImgClose.GetHeight();
	//初始化公告信息
	m_strSaveGameAffiche.clear();
	//Author<cxf>;DateTime<2012-03-01>;Description<屏蔽>
	//m_bMasterRight=false;//Author<cxf>;DateTime<2012-02-27>

	return TRUE;
}

//消息过虑
BOOL CRoomViewItem::PreTranslateMessage(MSG * pMsg)
{
	if (WM_KEYDOWN == pMsg->message)
	{
		if (VK_RETURN == pMsg->wParam)
		{
			if (ServiceStatus_Serviceing == m_ServiceStatus)
			{
				OnBnClickedSendChat();
				return TRUE;
			}
		}
		else if (::GetKeyState(VK_CONTROL)<0)
		{
			if (70==pMsg->wParam || 102==pMsg->wParam)
			{
				SetCtrolAddF();
				return TRUE;
			}
		}
	}

	if (m_ToolTipCtrl.m_hWnd != NULL)
	{
		m_ToolTipCtrl.RelayEvent(pMsg);
	}

	return __super::PreTranslateMessage(pMsg);
}

//菜单命令
BOOL CRoomViewItem::OnCommand(WPARAM wParam, LPARAM lParam)
{
	switch (LOWORD(wParam))
	{
	case IDM_UM_WISPER:			//私聊消息
		{
			//寻找玩家
			if (m_dwMenuUserID==0L) return TRUE;
			IUserItem * pIUserItem=m_ClientUserManager.SearchUserByUserID(m_dwMenuUserID);
			if (pIUserItem==NULL) return TRUE;

			//发送私聊
			if (m_dwMenuUserID!=m_pMeUserItem->GetUserID()) SendUserWhisper(pIUserItem);

			return TRUE;
		}
	case IDM_UM_COPY_NAME:		//拷贝名字
		{
			//寻找玩家
			if (m_dwMenuUserID==0L) return TRUE;
			IUserItem * pIUserItem=m_ClientUserManager.SearchUserByUserID(m_dwMenuUserID);
			if (pIUserItem==NULL) return TRUE;

			//打开剪切板
			if (OpenClipboard()==FALSE) return TRUE;
			if (EmptyClipboard()==FALSE) { CloseClipboard(); return TRUE; }

			//复制数据
			HANDLE hData=GlobalAlloc(GMEM_MOVEABLE|GMEM_ZEROINIT,NAME_LEN);
			if (hData==NULL) 
			{
				CloseClipboard();
				return TRUE;
			}
			LPTSTR szMemName=(LPTSTR)GlobalLock(hData);
			lstrcpy(szMemName,pIUserItem->GetUserName());
			SetClipboardData(CF_TEXT,hData);
			GlobalUnlock(hData);
			CloseClipboard();

			//显示名字
			// 			m_ChatInput.Paste();
			// 			m_ChatInput.SetFocus();
			m_editInput.Paste();
			m_editInput.SetFocus();
			return TRUE;
		}
	case IDM_UM_SET_CHAT:		//设置聊天
		{
			//寻找玩家
			if (m_dwMenuUserID==0L) return TRUE;
			IUserItem * pIUserItem=m_ClientUserManager.SearchUserByUserID(m_dwMenuUserID);
			if (pIUserItem==NULL) return TRUE;

			//设置对象
			if (m_dwMenuUserID!=m_pMeUserItem->GetUserID())
			{
				SetChatObject(pIUserItem);
				//m_ChatInput.SetFocus();
				m_editInput.SetFocus();
			}

			return TRUE;
		}
	case IDM_UM_CANECL_CHAT:	//取消聊天
		{
			//设置对象
			SetChatObject(NULL);
			//m_ChatInput.SetFocus();
			m_editInput.SetFocus();
			return TRUE;
		}
	case IDM_UM_LOOK_LOCATION:	//查看位置
		{
			//寻找玩家
			if (m_dwMenuUserID==0L) return TRUE;
			IUserItem * pIUserItem=m_ClientUserManager.SearchUserByUserID(m_dwMenuUserID);
			if (pIUserItem==NULL) return TRUE;

			//定位位置
			tagUserData * pUserData=pIUserItem->GetUserData();
			if ((pUserData->wTableID!=INVALID_TABLE)&&(pUserData->wChairID!=INVALID_CHAIR))
			{
				m_TableFrame.EnsureVisibleTable(pUserData->wTableID);
				if (pUserData->cbUserStatus==US_LOOKON)
				{
					m_TableFrame.FlashTable(pUserData->wTableID);
				}
				else
				{
					m_TableFrame.FlashChair(pUserData->wTableID,pUserData->wChairID);
				}

			}

			return TRUE;
		}
		//case IDM_UM_SEARCH_USER:	//寻找玩家
		//	{
		//		//点击按钮
		//		OnBnClickedFindUser();

		//		return TRUE;
		//	}
	case IDM_UM_INVITE_PLAY:	//邀请玩家
		{
			//状态判断
			if (m_ServiceStatus!=ServiceStatus_Serviceing)
			{
				return TRUE;
			}


			//过虑消息
			tagUserData * pUserData=m_pMeUserItem->GetUserData();
			if (pUserData->cbUserStatus==US_PLAY)
			{
				return TRUE;
			}

			if (pUserData->wTableID==INVALID_TABLE) 
			{
				return TRUE;
			}


			//寻找玩家
			if (m_dwMenuUserID==0L) 
			{
				return TRUE;
			}

			IUserItem * pIUserItem=m_ClientUserManager.SearchUserByUserID(m_dwMenuUserID);
			if (pIUserItem==NULL) 
			{
				return TRUE;
			}


			//发送命令
			CMD_GR_UserInviteReq UserInviteReq;
			UserInviteReq.wTableID=pUserData->wTableID;
			UserInviteReq.dwUserID=pIUserItem->GetUserID();
			m_ClientSocket->SendData(MDM_GR_USER,SUB_GR_USER_INVITE_REQ,&UserInviteReq,sizeof(UserInviteReq));

			//提示消息
			m_MessageProxyHelper->InsertSystemString(TEXT("成功发送用户邀请命令"),0);

			return TRUE;
		}
	case IDM_UM_SET_FRIEND:		//设为好友
		{
			//寻找玩家
			if (m_dwMenuUserID==0L) return TRUE;
			IUserItem * pIUserItem=m_ClientUserManager.SearchUserByUserID(m_dwMenuUserID);
			if (pIUserItem==NULL) return TRUE;

			//变量定义
			tagCompanionItem CompanionItem;
			memset(&CompanionItem,0,sizeof(CompanionItem));
			tagUserData * pUserData=pIUserItem->GetUserData();

			//设置关系
			CompanionItem.wFaceID=pUserData->wFaceID;
			CompanionItem.dwUserID=pUserData->dwUserID;
			CompanionItem.dwGameID=pUserData->dwGameID;
			if (pUserData->cbCompanion==enCompanion_Friend)
			{
				pUserData->cbCompanion=enCompanion_UnKnow;
				CompanionItem.Companion=enCompanion_UnKnow;
			}
			else
			{
				pUserData->cbCompanion=enCompanion_Friend;
				CompanionItem.Companion=enCompanion_Friend;
			}
			lstrcpyn(CompanionItem.szAccounts,pUserData->szName,CountArray(CompanionItem.szAccounts));
			g_GlobalUnits.m_CompanionManager->InsertCompanionItem(CompanionItem);
			//((CGameFrame*)AfxGetMainWnd())->m_UserCompanionList.OnCompanionUpdate(&CompanionItem);

			//改变通知
			OnUserItemUpdate(pIUserItem);

			return TRUE;
		}
	case IDM_UM_SET_DETEST:		//设为厌恶
		{
			//寻找玩家
			if (m_dwMenuUserID==0L) return TRUE;
			IUserItem * pIUserItem=m_ClientUserManager.SearchUserByUserID(m_dwMenuUserID);
			if (pIUserItem==NULL) return TRUE;

			//变量定义
			tagCompanionItem CompanionItem;
			memset(&CompanionItem,0,sizeof(CompanionItem));
			tagUserData * pUserData=pIUserItem->GetUserData();

			//设置关系
			CompanionItem.wFaceID=pUserData->wFaceID;
			CompanionItem.dwUserID=pUserData->dwUserID;
			CompanionItem.dwGameID=pUserData->dwGameID;
			if (pUserData->cbCompanion==enCompanion_Detest)
			{
				pUserData->cbCompanion=enCompanion_UnKnow;
				CompanionItem.Companion=enCompanion_UnKnow;
			}
			else
			{
				pUserData->cbCompanion=enCompanion_Detest;
				CompanionItem.Companion=enCompanion_Detest;
			}
			lstrcpyn(CompanionItem.szAccounts,pUserData->szName,CountArray(CompanionItem.szAccounts));
			g_GlobalUnits.m_CompanionManager->InsertCompanionItem(CompanionItem);
			//			((CGameFrame*)AfxGetMainWnd())->m_UserCompanionList.OnCompanionUpdate(&CompanionItem);

			//改变通知
			OnUserItemUpdate(pIUserItem);

			//提示消息
			if (pUserData->cbCompanion==enCompanion_Detest)
			{
				TCHAR szMessage[256]=TEXT("");
				_sntprintf(szMessage,CountArray(szMessage),TEXT("[ %s ] 设置为厌恶玩家，系统将屏蔽他的一切聊天消息"),pUserData->szName);
				m_MessageProxyHelper->InsertSystemString(szMessage,0);
			}

			return TRUE;
		}
	case IDM_UM_SEND_WARNNING:	//发送警告
		{
			//寻找玩家
			if (m_dwMenuUserID==0L) return TRUE;
			IUserItem * pIUserItem=m_ClientUserManager.SearchUserByUserID(m_dwMenuUserID);
			if (pIUserItem==NULL) return TRUE;

			//权限判断
			tagUserData * pMeUserData=m_pMeUserItem->GetUserData();
			if (CUserRight::CanSendWarning(pMeUserData->dwMasterRight)==false) return TRUE;

			//创建窗口
			if (m_ManagerSendWarning.m_hWnd==NULL) 
			{
				m_ManagerSendWarning.Create(IDD_SEND_WARNING,this);
				m_ManagerSendWarning.SetClientUserManager(QUERY_OBJECT_INTERFACE(m_ClientUserManager,IUnknownEx));
				m_ManagerSendWarning.SetClientSocket(QUERY_OBJECT_PTR_INTERFACE(m_ClientSocket.GetInterface(),IUnknownEx));
			}

			//设置变量
			m_ManagerSendWarning.SetUserItem(pIUserItem);

			//显示窗口
			m_ManagerSendWarning.ShowWindow(SW_SHOW);
			m_ManagerSendWarning.BringWindowToTop();

			return TRUE;
		}
	case IDM_UM_LOOK_USER_IP:	//查看地址
		{
			//寻找玩家
			if (m_dwMenuUserID==0L) return TRUE;
			IUserItem * pIUserItem=m_ClientUserManager.SearchUserByUserID(m_dwMenuUserID);
			if (pIUserItem==NULL) return TRUE;

			//权限判断
			tagUserData * pMeUserData=m_pMeUserItem->GetUserData();
			if (CUserRight::CanSeeUserIP(pMeUserData->dwMasterRight)==false) return TRUE;

			//发送命令
			CMD_GR_LookUserIP LookUserIP;
			LookUserIP.dwTargetUserID=pIUserItem->GetUserID();
			m_ClientSocket->SendData(MDM_GR_MANAGER,SUB_GR_LOOK_USER_IP,&LookUserIP,sizeof(LookUserIP));

			return TRUE;
		}
	case IDM_UM_EXIT_TABLE: // 踢出桌子
	case IDM_UM_CUT_USER:		//踢出用户
		{
			//寻找玩家
			if (m_dwMenuUserID==0L) return TRUE;
			IUserItem * pIUserItem=m_ClientUserManager.SearchUserByUserID(m_dwMenuUserID);
			if (pIUserItem==NULL) return TRUE;

			//权限判断
			tagUserData * pMeUserData=m_pMeUserItem->GetUserData();
			if (CUserRight::CanCutUser(pMeUserData->dwMasterRight)==false) return TRUE;

			//发送命令
			CMD_GR_KillUser KillUser;
			KillUser.dwTargetUserID=pIUserItem->GetUserID();
			KillUser.bCutOnline = (IDM_UM_EXIT_TABLE==LOWORD(wParam)) ? FALSE : TRUE;
			m_ClientSocket->SendData(MDM_GR_MANAGER,SUB_GR_KILL_USER,&KillUser,sizeof(KillUser));

			return TRUE;
		}
	case IDM_UM_LIMIT_ACCOUN:	//禁用帐户
		{
			//寻找玩家
			if (m_dwMenuUserID==0L) return TRUE;
			IUserItem * pIUserItem=m_ClientUserManager.SearchUserByUserID(m_dwMenuUserID);
			if (pIUserItem==NULL) return TRUE;

			//权限判断
			tagUserData * pMeUserData=m_pMeUserItem->GetUserData();
			if (CUserRight::CanForbidAccounts(pMeUserData->dwMasterRight)==false) return TRUE;

			//确认提示
			TCHAR szMessage[128]=TEXT("");
			_sntprintf(szMessage,CountArray(szMessage),TEXT("确实要冻结 [ %s ] 的帐号吗？"),pIUserItem->GetUserName());
			if (ShowMessageBox(szMessage,MB_ICONQUESTION|MB_YESNO)!=IDYES) return TRUE;

			//用户判断
			if (pIUserItem->GetUserID()!=m_dwMenuUserID) 
			{
				_sntprintf(szMessage,CountArray(szMessage),TEXT("用户 [ %s ] 已经离开了游戏房间，帐号冻结失败"),pIUserItem->GetUserName());
				m_MessageProxyHelper->InsertSystemString(szMessage,0);

				return TRUE;
			}

			//发送命令
			CMD_GR_LimitAccounts LimitAccounts;
			LimitAccounts.dwTargetUserID=pIUserItem->GetUserID();
			m_ClientSocket->SendData(MDM_GR_MANAGER,SUB_GR_LIMIT_ACCOUNS,&LimitAccounts,sizeof(LimitAccounts));

			return TRUE;
		}
	case IDM_UM_MANAGER_USER:	//权限管理
		{
			//寻找玩家
			if (m_dwMenuUserID==0L) return TRUE;
			IUserItem * pIUserItem=m_ClientUserManager.SearchUserByUserID(m_dwMenuUserID);
			if (pIUserItem==NULL) return TRUE;

			//创建窗口
			if (m_ManagerUserRight.m_hWnd==NULL) 
			{
				m_ManagerUserRight.Create(IDD_MANAGER_RIGHT,this);
				m_ManagerUserRight.SetMasterRight(m_pMeUserItem->GetUserData()->dwMasterRight);
				m_ManagerUserRight.SetClientUserManager(QUERY_OBJECT_INTERFACE(m_ClientUserManager,IUnknownEx));
				m_ManagerUserRight.SetClientSocket(QUERY_OBJECT_PTR_INTERFACE(m_ClientSocket.GetInterface(),IUnknownEx));
			}

			//设置变量
			m_ManagerUserRight.SetUserItem(pIUserItem);

			//显示窗口
			m_ManagerUserRight.ShowWindow(SW_SHOW);
			m_ManagerUserRight.BringWindowToTop();

			return TRUE;
		}
	case IDM_UM_ISSUE_MESSAGE:	//发布消息
		{
			//权限判断
			tagUserData * pMeUserData=m_pMeUserItem->GetUserData();
			if (CUserRight::CanIssueMessage(pMeUserData->dwMasterRight)==false)
			{
				return TRUE;
			}


			//创建窗口
			if (m_ManagerMessage.m_hWnd==NULL) 
			{
				m_ManagerMessage.Create(IDD_MANAGER_MESSAGE,this);
				m_ManagerMessage.SetClientSocket(QUERY_OBJECT_PTR_INTERFACE(m_ClientSocket.GetInterface(),IUnknownEx));
			}

			//显示窗口
			m_ManagerMessage.ShowWindow(SW_SHOW);
			m_ManagerMessage.BringWindowToTop();

			return TRUE;
		}
	case IDM_IM_SYSTEM_OPTION:	//房间管理
		{
			//权限判断
			tagUserData * pMeUserData=m_pMeUserItem->GetUserData();
			if (CUserRight::CanServerOption(pMeUserData->dwMasterRight)==false)
			{
				return TRUE;
			}


			//创建窗口
			if (m_ManagerServer.m_hWnd==NULL) 
			{
				m_ManagerServer.Create(IDD_MANAGER_SERVER,this);
				m_ManagerServer.SetClientSocket(QUERY_OBJECT_PTR_INTERFACE(m_ClientSocket.GetInterface(),IUnknownEx));
			}

			//显示窗口
			m_ManagerServer.ShowWindow(SW_SHOW);
			m_ManagerServer.BringWindowToTop();

			return TRUE;
		}
	}

	return __super::OnCommand(wParam,lParam);
}

//计算数字的位数
WORD CRoomViewItem::GetNumsBitNums(ULONG ulNums)
{
	if (ulNums < 10)
	{
		return 1;
	}
	else if (ulNums < 100)
	{
		return 2;
	}
	else if (ulNums < 1000)
	{
		return 3;
	}
	else
	{
		TCHAR tc[NAME_LEN]=TEXT("");
		_sntprintf(tc,sizeof(tc),TEXT("%d"),ulNums);
		LPCTSTR pszString = tc;
		WORD iCountBit = 0;
		while (*pszString != '\0')
		{
			++iCountBit;
			pszString++;
		}
		return iCountBit;
	}
}
CPoint CRoomViewItem::DrawNumber(CDC * pDC, CPoint pt, LONG number)
{
	if (NULL == pDC || m_pngNumber.IsNull())
		return CSize(0, 0);

	TCHAR tc[NAME_LEN]=TEXT("");
	_sntprintf(tc,sizeof(tc),TEXT("%d"),number);
	LPCTSTR pszString = tc;
	int nImgWidth = m_pngNumber.GetWidth()/10;
	int nImgHeight =  m_pngNumber.GetHeight();
	int nOffsetX = pt.x, nOffsetY = pt.y;
	int nLen = strlen(pszString);
	CRect rc(0, 0, 0, 0);
	rc.bottom = rc.top + nImgHeight;
	rc.right = rc.left + (nLen * nImgWidth);

	int n = 0;
	while (*pszString != '\0')
	{
		CString str;
		str =  *pszString;
		n = atoi((LPCTSTR)str);
		m_pngNumber.DrawImage(pDC, nOffsetX, nOffsetY,nImgWidth,nImgHeight,n*nImgWidth,0);
		nOffsetX += nImgWidth;
		pszString++;
	}
	return CPoint(nOffsetX, nOffsetY);
}

//绘制当前排名
void CRoomViewItem::DrawCurSort(DWORD wSort)
{
	if (0 == wSort) return;
	//绘制当前排名
	CRect sortarea(20, 593, 205, 621);
	ULONG ulCountWidth = 0;
	if (!m_pngImageCurSort.IsNull())
	{
		ulCountWidth = m_pngImageCurSort.GetWidth();
	}
	int nImgNumWidth = 0;
	int nImgNumHeight = 0;
	if (!m_pngNumber.IsNull())
	{
		nImgNumWidth = m_pngNumber.GetWidth()/10;
		nImgNumHeight =  m_pngNumber.GetHeight();
	}
	if (wSort > GAME_USER_CURRENT_SORT_W)
	{//显示10000后
		//计算数字宽度
		WORD wBit = GetNumsBitNums(GAME_USER_CURRENT_SORT_W);
		ulCountWidth += wBit*nImgNumWidth;

		if (!m_pngImageHou.IsNull())
		{
			ulCountWidth += m_pngImageHou.GetWidth();
		}
	}
	else
	{
		WORD wBit = GetNumsBitNums(wSort);
		ulCountWidth += wBit*nImgNumWidth;
		if (!m_pngText1.IsNull())
		{//位
			ulCountWidth += m_pngText1.GetWidth();
		}
	}
	CPoint sortPoint;
	sortPoint.x = sortarea.left + (sortarea.Width() - ulCountWidth)/2;
	sortPoint.y = sortarea.top + 7;
	if (!m_pngImageCurSort.IsNull())
	{
		m_pngImageCurSort.DrawImage(&m_memdc, sortPoint.x, sortPoint.y);
		sortPoint.x += m_pngImageCurSort.GetWidth();
	}
	if (wSort > GAME_USER_CURRENT_SORT_W)
	{
		CPoint numpoint;
		numpoint = sortPoint;
		numpoint.y += 3;
		numpoint =  DrawNumber(&m_memdc, numpoint, GAME_USER_CURRENT_SORT_W);
		if (!m_pngImageHou.IsNull())
		{
			sortPoint.x = numpoint.x;
			m_pngImageHou.DrawImage(&m_memdc, sortPoint.x, sortPoint.y);
		}
	}
	else
	{
		CPoint numpoint;
		numpoint = sortPoint;
		numpoint.y += 3;
		numpoint =  DrawNumber(&m_memdc, numpoint, wSort);
		if (!m_pngText1.IsNull())
		{
			sortPoint.x = numpoint.x;
			m_pngText1.DrawImage(&m_memdc, sortPoint.x, sortPoint.y);
		}
	}
}

//绘制今日赢取
void CRoomViewItem::DrawDayWin(DWORD wWinNums)
{
	CRect sortarea(205, 593, 466, 621);
	ULONG ulCountWidth = 0;
	if (!m_pngImageDayWin.IsNull())
	{
		ulCountWidth = m_pngImageDayWin.GetWidth();
	}
	int nImgNumWidth = 0;
	int nImgNumHeight = 0;
	if (!m_pngNumber.IsNull())
	{
		nImgNumWidth = m_pngNumber.GetWidth()/10;
		nImgNumHeight =  m_pngNumber.GetHeight();
	}
	WORD wBit = GetNumsBitNums(wWinNums);
	ulCountWidth += wBit*nImgNumWidth;
	if (!m_pngText2.IsNull())
	{//宝石
		ulCountWidth += m_pngText2.GetWidth();
	}

	CPoint sortPoint;
	sortPoint.x = sortarea.left + (sortarea.Width() - ulCountWidth)/2;
	sortPoint.y = sortarea.top + 7;
	if (!m_pngImageDayWin.IsNull())
	{
		m_pngImageDayWin.DrawImage(&m_memdc, sortPoint.x, sortPoint.y);
		sortPoint.x += m_pngImageDayWin.GetWidth();
	}
	CPoint numpoint;
	numpoint = sortPoint;
	numpoint.y += 3;
	numpoint =  DrawNumber(&m_memdc, numpoint, wWinNums);
	if (!m_pngText2.IsNull())
	{
		sortPoint.x = numpoint.x;
		m_pngText2.DrawImage(&m_memdc, sortPoint.x, sortPoint.y);
	}
}
//绘画消息
void CRoomViewItem::OnPaint()
{
	CPaintDC dc(this);

	if (m_memdc.m_hDC)
	{
		//绘制桌子
		//m_TableFrame.OnPaint(&m_memdc);

		//绘制游戏图标
		if (!m_pngGameICO.IsNull())
		{
			CRect rcico(260, 46, 390, 70);
			CPoint ptIco;
			ptIco.x = rcico.left+(rcico.Width()-m_pngGameICO.GetWidth())/2;
			ptIco.y = rcico.top;
			m_pngGameICO.DrawImage(&m_memdc, ptIco.x, ptIco.y);
		}

		/* cxf 2011-10-09 if (m_pMeUserItem)
		{
		//获取用户
		tagUserData * pUserData=m_pMeUserItem->GetUserData();
		if (NULL != pUserData)
		{
		DWORD dwOrder = 0;
		if (pUserData->dwOrder < 0)
		{
		dwOrder = 100001;
		}
		else
		{
		dwOrder = pUserData->dwOrder;
		}
		//绘制当前排名
		DrawCurSort(dwOrder);

		if (m_dwWinGemsToday < 0)
		{
		m_dwWinGemsToday = 0;
		}
		//绘制今日赢取
		DrawDayWin(m_dwWinGemsToday);

		}
		}*/
		//===================================Author<cxf>;Datetime<2012-02-23>

		//Author<cxf>;DateTime<2012-03-01>;Description<屏蔽>
		if (!IsAdministrationHavePurview()) // 玩家视图
		{
			if (!m_ImageBack.IsNull())
			{
					m_ImageBack.DrawImage(&m_memdc, 0, 0);
			}

			if (!m_ImgAnimAction.IsNull())
			{//动画
				WORD wWidth=m_ImgAnimAction.GetWidth()/12;
				m_ImgAnimAction.DrawImage(&m_memdc,m_rcAnimAction.left,m_rcAnimAction.top,wWidth,
					m_ImgAnimAction.GetHeight(),m_wAnimAction*wWidth,0);
			}

			if (!m_ImgClose.IsNull())
			{//关闭按钮
				WORD wWidth=m_ImgClose.GetWidth()/5;
				m_ImgClose.DrawImage(&m_memdc,m_rcCloseRect.left,m_rcCloseRect.top,wWidth,
					m_ImgClose.GetHeight(),m_wCloseState*wWidth,0);
			}

			if (!m_ImageBack.IsNull())
			{
				m_ImageBack.DrawImage(&m_memdc, 0, 0);
			}
			if (!m_ImgAnimAction.IsNull())
			{//动画
				WORD wWidth=m_ImgAnimAction.GetWidth()/12;
				m_ImgAnimAction.DrawImage(&m_memdc,m_rcAnimAction.left,m_rcAnimAction.top,wWidth,
					m_ImgAnimAction.GetHeight(),m_wAnimAction*wWidth,0);
			}
			if (!m_ImgClose.IsNull())
			{//关闭按钮
				WORD wWidth=m_ImgClose.GetWidth()/5;
				m_ImgClose.DrawImage(&m_memdc,m_rcCloseRect.left,m_rcCloseRect.top,wWidth,
					m_ImgClose.GetHeight(),m_wCloseState*wWidth,0);
			}
		}
		else  // 管理员视图
		{
			CRect rcWnd;
			GetClientRect(&rcWnd);
			if (!m_ImageBack.IsNull())
			{
				for (int i=0; i<rcWnd.Width(); i+=m_ImageBack.GetWidth())
				{
					for (int j=0; j<rcWnd.Height(); j+=m_ImageBack.GetHeight())
					{
						m_ImageBack.DrawImage(&m_memdc, i, j);
					}
				}
			}

			/*if (!m_ImageBackGM.IsNull())
			{
				m_ImageBackGM.DrawImage(&m_memdc, 0, 0);
			}*/

			//绘制桌子
			m_TableFrame.OnPaint(&m_memdc);

			//绘制游戏图标
			if (!m_pngGameICO.IsNull())
			{
				CRect rcico(260, 46, 390, 70);
				CPoint ptIco;
				ptIco.x = rcico.left+(rcico.Width()-m_pngGameICO.GetWidth())/2;
				ptIco.y = rcico.top;
				m_pngGameICO.DrawImage(&m_memdc, ptIco.x, ptIco.y);
			}

			//输出标题
			CFont* def_font = m_memdc.SelectObject(&m_fontGameTitleInfo);
			m_memdc.SetTextColor(RGB(255,203,104));
			TCHAR szRoomTitle[128]=TEXT("");
			_sntprintf(szRoomTitle,sizeof(szRoomTitle),TEXT("共有%d人排队"),m_QueueNum);
			m_memdc.DrawText(szRoomTitle,lstrlen(szRoomTitle),&m_rcQueue,DT_END_ELLIPSIS|DT_CENTER|DT_VCENTER|DT_SINGLELINE);

			_sntprintf(szRoomTitle,sizeof(szRoomTitle),TEXT("%d\/%d"), m_TableFrame.GetPage()+1, m_TableFrame.GetMaxPage());
			m_memdc.DrawText(szRoomTitle, lstrlen(szRoomTitle), &CRect(380,590,500,620), DT_END_ELLIPSIS|DT_CENTER|DT_VCENTER|DT_SINGLELINE);
			m_memdc.SelectObject(def_font);
		}

		CRect rect;
		GetClientRect(&rect);
		dc.BitBlt(0, 0, rect.Width(), rect.Height(), &m_memdc, 0, 0, SRCCOPY);
	}

	return;
}

//绘画背景
BOOL CRoomViewItem::OnEraseBkgnd(CDC * pDC)
{
	//RectifyControl(960,720);
	//m_TableFrame.OnPaint();
	/*
	CRect rect;
	GetClientRect(&rect);

	DrawPic(pDC,IDB_TABLE_LIST_BG,&rect);

	Invalidate(FALSE);
	UpdateWindow();*/


	return TRUE;
}

//位置消息
void CRoomViewItem::OnSize(UINT nType, int cx, int cy)
{
	__super::OnSize(nType, cx, cy);

	//设置变量
	m_bRectifyVor=false;

	CPaintDC dc(this);
	m_memdc.DeleteDC();
	CBitmap bmp;
	m_memdc.CreateCompatibleDC(&dc);
	bmp.CreateCompatibleBitmap(&dc, cx, cy);
	m_memdc.SelectObject(bmp);	
	m_memdc.SetBkMode(TRANSPARENT);

	//调整控件
	RectifyControl(cx,cy);
	m_TableFrame.OnSize(nType, cx, cy);

	bmp.DeleteObject();

	return;
}

//显示消息
void CRoomViewItem::OnShowWindow(BOOL bShow, UINT nStatus)
{
	__super::OnShowWindow(bShow, nStatus);

	//设置标题
	//if (bShow) 
	//{
	//	UpdateTitleText();
	//}
	return;
}

void CRoomViewItem::OnWindowPosChanging(WINDOWPOS* lpwndpos)
{
	if (IsAdministrationHavePurview())
	{
		lpwndpos->x += 6;
		lpwndpos->y += 30;
		lpwndpos->cx -= 12;
		lpwndpos->cy -= 35;
	}

	return 	__super::OnWindowPosChanging(lpwndpos);
}

/*
//双击了鼠标左键
void CRoomViewItem::OnNMDblclkUserList(NMHDR * pNMHDR, LRESULT * pResult)
{
NMITEMACTIVATE * pListNotify=(NMITEMACTIVATE *)pNMHDR;
if ((pListNotify->iItem!=-1)&&(m_cbHideUserInfo==FALSE))
{

IUserItem * pIUserItem=(IUserItem *)m_UserListView.GetItemData(pListNotify->iItem);
if ((pIUserItem!=NULL)&&(pIUserItem!=m_pMeUserItem))
{
SendUserWhisper(pIUserItem);
}



}

*pResult=0;
}

//右键点击用户列表
void CRoomViewItem::OnNMRclickUserList(NMHDR * pNMHDR, LRESULT *pResult)
{
NMITEMACTIVATE * pListNotify = (NMITEMACTIVATE *)pNMHDR;

if (pListNotify->iItem != -1)
{
CPoint MousePoint;
GetCursorPos(&MousePoint);

IUserItem * pIUserItem=(IUserItem *)m_UserListView.GetItemData(pListNotify->iItem);
if (pIUserItem!=NULL) ShowUserInfoMenu(pIUserItem,MousePoint);
}

*pResult=0;

return ;
}

//点击用户列表
void CRoomViewItem::OnNMclickUserList(NMHDR * pNMHDR, LRESULT * pResult)
{
NMITEMACTIVATE * pListNotify = (NMITEMACTIVATE *)pNMHDR;

if (pListNotify->iItem != -1)
{
CPoint MousePoint;
GetCursorPos(&MousePoint);

//选择用户
IUserItem * pIUserItem=(IUserItem *)m_UserListView.GetItemData(pListNotify->iItem);
if (pIUserItem!=NULL)
{
m_PropertyBar.SetUserDataInfo(pIUserItem->GetUserData());
}


}

*pResult=0;
return ;
}
*/
//更新头像
LRESULT CRoomViewItem::OnUpdateFace(WPARAM wParam, LPARAM lParam)
{
	DWORD dwUserID = DWORD(lParam);
	DWORD cbFaceVer = DWORD(wParam);

	//更新信息
	tagUserData *pUserData = m_ClientUserManager.SearchUserByUserID(dwUserID)->GetUserData();
	if(pUserData!=NULL)pUserData->dwCustomFaceVer = cbFaceVer;

	//更新头像
	if (pUserData->wTableID!=INVALID_TABLE)
	{
		IPC_UpdateFace UpdateFace;
		memset(&UpdateFace,0,sizeof(UpdateFace));
		UpdateFace.dwCustomFace = cbFaceVer;
		SendProcessData(IPC_MAIN_USER,IPC_SUB_UPDATE_FACE,&UpdateFace,sizeof(UpdateFace));
	}

	return 0;
}

//IPC 消息
BOOL CRoomViewItem::OnCopyData(CWnd *pWnd, COPYDATASTRUCT *pCopyDataStruct)
{
	if (m_IPCRecvCopyData.OnCopyData(pWnd->GetSafeHwnd(), pCopyDataStruct)==true) return TRUE;
	return __super::OnCopyData(pWnd, pCopyDataStruct);
}

//表情消息
LRESULT CRoomViewItem::OnHitExpression(WPARAM wParam, LPARAM lParam)
{
	CExpressionItem * pExpressionItem=m_Expression.GetExpressItem((WORD)wParam);
	if (pExpressionItem!=NULL)
	{
		long nStartChar=0, nEndChar;
		CString strBuffer=TEXT(""), strBufLeft, strBufRight;
		m_editInput.GetSel(nStartChar, nEndChar);

		m_editInput.GetWindowText(strBuffer);
		strBufLeft=strBuffer.Left(nStartChar);
		strBufRight=strBuffer.Right(strBuffer.GetLength()-nStartChar);

		strBufLeft+=pExpressionItem->m_szTrancelate;
		m_editInput.SetWindowText(strBufLeft+strBufRight);
		m_editInput.SetFocus();
		m_editInput.SetSel(strBufLeft.GetLength(),strBufLeft.GetLength());
	}

	return 0;
}

LRESULT CRoomViewItem::OnFlashChair(WPARAM wParam, LPARAM lParam)
{
	DWORD wUserID = (DWORD)wParam;
	if (0L==wUserID) 
		return TRUE;

	IUserItem * pIUserItem=m_ClientUserManager.SearchUserByUserID(wUserID);
	if (pIUserItem==NULL) 
		return TRUE;

	//定位位置
	tagUserData * pUserData=pIUserItem->GetUserData();
	if ((INVALID_TABLE!=pUserData->wTableID)  &&  (INVALID_CHAIR!=pUserData->wChairID))
	{
		m_TableFrame.EnsureVisibleTable(pUserData->wTableID);
		if (US_LOOKON == pUserData->cbUserStatus)
		{
			m_TableFrame.FlashTable(pUserData->wTableID);
		}
		else
		{
			m_TableFrame.FlashChair(pUserData->wTableID,pUserData->wChairID);
		}

	}
}

//发送按钮
void CRoomViewItem::OnBnClickedSendChat()
{
	//聊天信息
	CString strBuf;
	//m_ChatInput.GetWindowText(szChatMessage,CountArray(szChatMessage));
	//m_editInput.GetWindowText(szChatMessage,CountArray(szChatMessage));
	m_editInput.GetWindowText(strBuf);
	if (!strBuf.IsEmpty())
	{
		TCHAR szChatMessage[MAX_CHAT_LEN*4]=TEXT("");
		m_editInput.GetWindowText(szChatMessage,CountArray(szChatMessage));
		szChatMessage[CountArray(szChatMessage)-1]='\0';
		if (lstrlen(szChatMessage) < MAX_CHAT_LEN )
		{
			//限时聊天
			DWORD dwCurTimes = GetTickCount();
			if(m_dwLimitChatTimes==0 || dwCurTimes-m_dwLimitChatTimes>=LIMIT_CHAT_TIMES)
			{

				m_dwLimitChatTimes = dwCurTimes;
				//m_ChatInput.SetWindowText(TEXT(""));
				m_editInput.SetWindowText(TEXT(""));
				SendChatPacket(m_dwChatUserID,szChatMessage,g_GlobalOption.m_crChatTX);
			}else
			{
				m_MessageProxyHelper->InsertGeneralString(TEXT("您说话太快了,请稍候!"),RGB(255,0,0),MS_NORMAL,true);
			}
		}else
		{
			m_MessageProxyHelper->InsertGeneralString(TEXT("您说的话太长了!"),RGB(255,0,0),MS_NORMAL,true);
		}

	}else
	{
		m_MessageProxyHelper->InsertGeneralString(TEXT("聊天内容不能为空!"),RGB(255,0,0),MS_NORMAL,true);
	}

	//设置界面
	//m_ChatInput.SetFocus();
	m_editInput.SetFocus();
	return;
}

//表情按钮
void CRoomViewItem::OnBnClickedExpression()
{
	//建立表情窗口
	if (m_Expression.GetSafeHwnd()==NULL) m_Expression.CreateExpression(this);

	//移动窗口
	CRect rcButton;
	CSize ExpressSize;
	m_Expression.GetFixSize(ExpressSize);
	//cxf 1108 m_btExpression.GetWindowRect(&rcButton);
	m_Expression.MoveWindow(rcButton.right-ExpressSize.cx,rcButton.top-ExpressSize.cy,ExpressSize.cx,ExpressSize.cy);
	m_Expression.ShowWindow(SW_SHOW);
	m_Expression.SetFocus();

	return;
}

//颜色按钮
void CRoomViewItem::OnBnClickedColorSet()
{
	//设置颜色
	CColorDialog ColorDialog(g_GlobalOption.m_crChatTX,CC_FULLOPEN);
	if (ColorDialog.DoModal()==IDOK) g_GlobalOption.m_crChatTX = ColorDialog.GetColor();

	//设置界面
	//m_ChatInput.SetFocus();
	m_editInput.SetFocus();
	return;
}

//清屏按钮
void CRoomViewItem::OnBnClickedCleanScreen()
{
	m_MessageProxyHelper->CleanScreen();
	return;
}

//喇叭按钮
//void CRoomViewItem::OnBnClickedBugle()
//{
//	if(m_ClientSocket.GetInterface()==NULL)return;
//
//	//获取用户
//	tagGlobalUserData &GlobalUserData = g_GlobalUnits.GetGolbalUserData();
//	IUserItem *pMeUserItem = m_ClientUserManager.SearchUserByUserID(GlobalUserData.dwUserID);
//	tagUserData *pMeUserData = pMeUserItem->GetUserData();
//
//	//设置对话框
//	ShowBugleDlg(NULL,m_ClientSocket.GetInterface(),pMeUserData);
//
//	return ;
//}

//类型信息
tagGameKind * __cdecl CRoomViewItem::GetKindInfo()
{
	if (m_pListServer!=NULL)
	{
		CListKind * pListKind=m_pListServer->GetListKind();
		ASSERT(pListKind!=NULL);
		if (pListKind!=NULL)
		{
			return pListKind->GetItemInfo();
		}
	}
	return NULL;
}

//银行按钮
//void CRoomViewItem::OnBnClickedBank()
//{
//	//获取用户
//	tagGlobalUserData &GlobalUserData = g_GlobalUnits.GetGolbalUserData();
//	IUserItem *pMeUserItem = m_ClientUserManager.SearchUserByUserID(GlobalUserData.dwUserID);
//	tagUserData *pMeUserData = pMeUserItem->GetUserData();
//
//	//显示银行
//	ShowBankStorageDlg(NULL,m_ClientSocket.GetInterface(),pMeUserData);
//
//	return;
//}

//加入按钮
//void CRoomViewItem::OnBnClickedAutoSit()
//{
//	//判断状态
//	if (m_ServiceStatus!=ServiceStatus_Serviceing) return;
//
//	if ((m_wReqTableID!=INVALID_TABLE)||(m_wReqChairID!=INVALID_CHAIR))
//	{
//		TCHAR szMessage[]=TEXT("上次请求还没有收到服务器回应,请稍候！");
//		ShowMessageBox(szMessage,MB_ICONINFORMATION);
//		return;
//	}
//
//	//判断状态
//	tagUserData * pUserData=m_pMeUserItem->GetUserData();
//	if (pUserData->cbUserStatus==US_PLAY)
//	{
//		TCHAR szMessage[]=TEXT("您正在游戏中，不能换位置，请先结束游戏！");
//		ShowMessageBox(szMessage,MB_ICONINFORMATION);
//		return;
//	}
//
//	//防作弊房
//	if(m_cbHideUserInfo==TRUE)
//	{
//		//模拟查找
//		m_TableFrame.EnsureVisibleTable(0);
//		m_TableFrame.FlashChair(0,rand()%m_TableFrame.GetChairCount());
//
//		//自动坐下
//		SendSitDownPacket(INVALID_TABLE,INVALID_CHAIR,NULL);
//		return;
//	}
//
//	//先搜索桌子不全空的
//	tagFindTable FindInfo;
//	FindInfo.bAllNull=false;
//	FindInfo.bFilterPass=true;
//	FindInfo.bNotFull=true;
//	FindInfo.bOneNull=true;
//	FindInfo.bTwoNull=(m_TableFrame.GetChairCount()!=2);
//	FindInfo.wBeginTableID=m_wFindTableID+1;
//	FindInfo.wResultTableID=INVALID_TABLE;
//	FindInfo.wResultChairID=INVALID_CHAIR;
//	bool bSuccess=FindGameTable(FindInfo);
//	m_wFindTableID=FindInfo.wResultTableID;
//
//	//搜索全部游戏桌
//	if (bSuccess==false)
//	{
//		FindInfo.bAllNull=true;
//		FindInfo.bFilterPass=true;
//		FindInfo.bNotFull=true;
//		FindInfo.bOneNull=true;
//		FindInfo.bTwoNull=true;
//		FindInfo.wBeginTableID=m_wFindTableID+1;
//		FindInfo.wResultTableID=INVALID_TABLE;
//		FindInfo.wResultChairID=INVALID_CHAIR;
//		bSuccess=FindGameTable(FindInfo);
//		m_wFindTableID=FindInfo.wResultTableID;
//	}
//
//	//结果判断
//	if (bSuccess==true)
//	{
//		//效验数据
//		ASSERT(FindInfo.wResultTableID!=INVALID_TABLE);
//		ASSERT(FindInfo.wResultChairID!=INVALID_CHAIR);
//
//		//设置数据
//		WORD wChairID=FindInfo.wResultChairID;
//		m_TableFrame.EnsureVisibleTable(m_wFindTableID);
//		m_TableFrame.FlashChair(m_wFindTableID,wChairID);
//
//		//自动坐下
//		PerformSitDownAction(m_wFindTableID,wChairID);
//	}
//	else
//	{
//		TCHAR szMessage[]=TEXT("找不到符号条件的游戏桌！");
//		ShowMessageBox(szMessage,MB_ICONINFORMATION);
//	}
//
//	return;
//}

//查找按钮
void CRoomViewItem::OnBnClickedFindUser()
{
	if (m_pFindUserDlg==NULL) 
	{
		m_pFindUserDlg=new CFindUserDlg(this,0);
		m_pFindUserDlg->m_UserList.m_wGameGenre=m_wGameGenre;
		m_pFindUserDlg->m_UserList.m_wKindID=m_pListServer->GetItemInfo()->wKindID;
	}
	if (m_pFindUserDlg->GetSafeHwnd()==NULL)
	{
		m_pFindUserDlg->Create(IDD_FIND_USER,this);
		m_pFindUserDlg->m_UserList.SetColumnDescribe(m_ListColumnInfo.ColumnItem,m_ListColumnInfo.wColumnCount);
	}
	
	m_pFindUserDlg->ShowWindow(SW_SHOW);
	return;
}

//退出按钮
//void CRoomViewItem::OnBnClickedQuitRoom()
//{
////	((CGameFrame*)AfxGetMainWnd())->CloseRoomViewItem(this);
//	return;
//}

//返回按钮
void CRoomViewItem::OnBnReturn()
{
	SendRequestCloseRoomMsg(1);
}

//关闭按钮
void CRoomViewItem::OnBnClose()
{
	CloseRoomViewItem();
	DestroyWindow();
	AfxGetMainWnd()->PostMessage(WM_CLOSE,0,0);
}
//上一页
void CRoomViewItem::OnBnUp()
{
	m_TableFrame.OnBnUp();
	// 	int iPage=m_TableFrame.GetPage();
	// 	int temp=m_TableFrame.GetPageNum();
	// 
	// 	//首页判断
	// 	if(m_TableFrame.GetPageNum() == 0 && iPage == 0)return;
	// 
	// 	if(iPage > 0)
	// 	{
	// 		
	// 		iPage--;
	// 		m_TableFrame.SetPage(iPage);
	// 
	// // 		CRect rect;
	// // 		m_TableFrame.GetWindowRect(&rect);
	// // 		//ClientToScreen(&rect);
	// // 		InvalidateRect(rect, FALSE);
	// 		Invalidate(FALSE);
	// 		//m_TableFrame.Invalidate(FALSE);
	// //		UpdateWindow();
	// 	}
	// 	m_btnPgUp.EnableWindow(iPage > 0 ? TRUE: FALSE);
	//else
	//{
	//	WORD wTableCount=0;						//游戏桌数

	//	//连接房间
	//	CRoomViewItem * pRoomViewItem=NULL;
	//	CListServer * pListServer=NULL;

	//	try
	//	{
	//		do 
	//		{
	//			if(m_nListIndex > 0)
	//				m_nListIndex--;
	//			else
	//			{
	//				ASSERT(FALSE);
	//				break;
	//			}

	//			pListServer=g_GlobalUnits.m_ServerListManager.EnumServerItem(m_nListIndex);
	//			if(pListServer == NULL)return;

	//			if(m_pListServer->m_GameServer.wSortID == pListServer->m_GameServer.wSortID && 
	//				m_pListServer->m_GameServer.wServerPort != pListServer->m_GameServer.wServerPort)
	//			{
	//				//关闭连接
	//				if (m_ClientSocket.GetInterface()!=NULL) m_ClientSocket->CloseSocket();

	//				//关闭游戏
	//				CloseGameClient();

	//				//清理内存
	//				if (m_pShareMemory!=NULL)
	//				{
	//					UnmapViewOfFile(m_pShareMemory);
	//					m_pShareMemory=NULL;
	//				}
	//				if (m_hShareMemory!=NULL)
	//				{
	//					CloseHandle(m_hShareMemory);
	//					m_hShareMemory=NULL;
	//				}

	//				wTableCount=m_TableFrame.GetAllLine();
	//				this->m_TableFrame.DestroyTableFrame();

	//				//初始化房间
	//				if (this->InitRoomViewItem(pListServer)==false) 
	//					throw TEXT("游戏房间初始化失败！");
	//				else
	//					break;
	//			}
	//		} while (true);
	//	}
	//	catch (...) { SafeDelete(pRoomViewItem); }

	//	//连接游戏
	//	bool bSuccess=this->ConnectGameServer();
	//	if (bSuccess==false)
	//	{
	//		//DestroyWindow();
	//		//SafeDelete(this);
	//		ASSERT(FALSE);
	//		return;
	//	}

	//	//设置桌号
	//	int iPageNum=m_TableFrame.GetPageNum();

	//	if(iPageNum > 0)
	//		iPageNum--;

	//	if(iPageNum-(wTableCount-8)/9 > 0)
	//		m_TableFrame.SetPageNum(iPageNum-(wTableCount-8)/9);
	//	else
	//		m_TableFrame.SetPageNum(0);

	//	m_TableFrame.SetPage((wTableCount-8)/9);
	//}

	//Invalidate(FALSE);
	//UpdateWindow();
}
//下一页
void CRoomViewItem::OnBnDwon()
{
	m_TableFrame.OnBnDwon();
	// 	int iPage=m_TableFrame.GetPage();
	// 	if((m_TableFrame.GetAllLine()-8)/9 > iPage)
	// 	{
	// 		iPage++;
	// 		m_TableFrame.SetPage(iPage);
	// 	}
	// 
	// 	m_btnPgUp.EnableWindow(iPage > 0 ? TRUE: FALSE);

	//else
	//{
	//	//连接房间
	//	CRoomViewItem * pRoomViewItem=NULL;
	//	CListServer * pListServer=NULL;

	//	try
	//	{
	//		do 
	//		{
	//			pListServer=g_GlobalUnits.m_ServerListManager.EnumServerItem(m_nListIndex++);
	//			if(pListServer == NULL)return;

	//			if(m_pListServer->m_GameServer.wSortID == pListServer->m_GameServer.wSortID && 
	//				m_pListServer->m_GameServer.wServerPort != pListServer->m_GameServer.wServerPort)
	//			{
	//				//关闭连接
	//				if (m_ClientSocket.GetInterface()!=NULL) m_ClientSocket->CloseSocket();

	//				//关闭游戏
	//				CloseGameClient();

	//				//清理内存
	//				if (m_pShareMemory!=NULL)
	//				{
	//					UnmapViewOfFile(m_pShareMemory);
	//					m_pShareMemory=NULL;
	//				}
	//				if (m_hShareMemory!=NULL)
	//				{
	//					CloseHandle(m_hShareMemory);
	//					m_hShareMemory=NULL;
	//				}

	//				this->m_TableFrame.DestroyTableFrame();

	//				//初始化房间
	//				if (this->InitRoomViewItem(pListServer)==false) 
	//					throw TEXT("游戏房间初始化失败！");
	//				else
	//					break;
	//			}

	//		} while (true);
	//	}
	//	catch (...) { SafeDelete(pRoomViewItem); }

	//	//连接游戏
	//	bool bSuccess=this->ConnectGameServer();
	//	if (bSuccess==false)
	//	{
	//		//DestroyWindow();
	//		//SafeDelete(this);
	//		ASSERT(FALSE);
	//		return;
	//	}

	//	//设置桌号
	//	iPage++;
	//	m_TableFrame.SetPageNum(iPage);
	//}

	//更新界面
	//CRect rect;
	//m_TableFrame.GetWindowRect(&rect);
	//ClientToScreen(&rect);
	//m_TableFrame.Invalidate(FALSE);
	//UpdateWindow();
	Invalidate(FALSE);
}

//点击自动排队
void CRoomViewItem::OnAutoQueue()
{
	//cxf 1108 if (!m_AutoQueue.IsWindowEnabled())
	//cxf 1108 {
	//cxf 1108 	return;
	//cxf 1108 }
	if (m_bQueueState)
	{
		m_ClientSocket->SendData(MDM_GR_USER,SUB_GR_AUTO_QUEUE);
	}
	else
	{
		m_ClientSocket->SendData(MDM_GR_USER,SUB_GR_AUTO_QUEUE_CANCEL);
	}
	//cxf 1108 m_AutoQueue.EnableWindow(FALSE);
	//cxf 1108 m_AutoQueue.Invalidate(FALSE);
	m_bQueueState = !m_bQueueState;

}

//房间信息
tagGameServer * __cdecl CRoomViewItem::GetServerInfo()
{
	if (m_pListServer==NULL) return NULL;
	return m_pListServer->GetItemInfo();
}

//老板事件
void CRoomViewItem::NotifyBossCome(bool bBossCome)
{
	return;
	//发送消息
	if (m_pShareMemory==NULL) return;
	if (bBossCome==true) 
	{
		::ShowWindow(m_pShareMemory->hWndGameFrame,SW_HIDE);
	}
	else 
	{
		::ShowWindow(m_pShareMemory->hWndGameFrame,SW_SHOW);
	}

	return;
}

//关闭询问
bool __cdecl CRoomViewItem::QueryCloseRoom()
{
	//状态判断
	if (m_pMeUserItem==NULL) return true;
	if (m_ServiceStatus!=ServiceStatus_Serviceing) return true;
	if (m_pMeUserItem->GetUserData()->cbUserStatus!=US_PLAY) return true;

#ifndef _DEBUG

	//提示消息
	int iRet=ShowInformation(TEXT("您正在游戏中，强行退出将被扣分，确实要强退吗？"),0,MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2);

	//重新判断
	if (m_pMeUserItem==NULL) return true;
	if (m_ServiceStatus!=ServiceStatus_Serviceing) return true;
	if (m_pMeUserItem->GetUserData()->cbUserStatus!=US_PLAY) return true;

	//结果判断
	if (iRet==IDYES)
	{
		SendLeftGamePacket();
		return true;
	}

	return false;

#else

	return true;

#endif

}

//关闭通知
void __cdecl CRoomViewItem::CloseRoomViewItem()
{
	//关闭连接
	if (m_ClientSocket.GetInterface()!=NULL) 
	{
		m_ClientSocket->CloseSocket();
	}

	CloseGameClient();

	//清理内存
	if (m_pShareMemory!=NULL)
	{
		UnmapViewOfFile(m_pShareMemory);
		m_pShareMemory=NULL;
	}
	if (m_hShareMemory!=NULL)
	{
		CloseHandle(m_hShareMemory);
		m_hShareMemory=NULL;
	}

	//关闭请求定时器
	m_bRequestServerExit = false;
	KillTimer(IDI_REQUEST_SERVER_EXIT_MSG);

	//关闭窗口
	DestroyWindow();

	return;
}

//按钮消息
bool CRoomViewItem::OnSplitterButton(CSkinSplitter * pSkinSplitter, CSkinButton * pSplitterButton)
{
	/*
	//设置变量
	m_bHideUserList=!m_bHideUserList;

	//控制按钮
	UINT uControlId = ((!m_bHideUserList)?GetServerViewRes().uBtHideUserList:GetServerViewRes().uBtShowUserList);
	m_VorSplitter.ShowSplitterButton(uControlId,GetResInstanceHandle());

	//调整界面
	CRect rcClient;
	GetClientRect(&rcClient);
	RectifyControl(rcClient.Width(),rcClient.Height());
	*/

	return true;
}

//拆分条消息
bool CRoomViewItem::OnSplitterEvent(CSkinSplitter * pSkinSplitter, UINT uSplitID, int nXPos, int nYPos)
{
	/*
	//显示控件
	if (m_bHideUserList==true)
	{
	//设置变量
	m_bRectifyHor=true;
	m_bHideUserList=false;

	//设置按钮
	UINT uControlId = ((!m_bHideUserList)?GetServerViewRes().uBtHideUserList:GetServerViewRes().uBtShowUserList);
	m_VorSplitter.ShowSplitterButton(uControlId,GetResInstanceHandle());
	}

	//调整界面
	CRect rcClient;
	GetClientRect(&rcClient);
	RectifyControl(rcClient.Width(),rcClient.Height());
	*/

	return true;
}

//初始化
bool CRoomViewItem::InitRoomViewItem(CListServer * pListServer)
{
	//设置变量
	ASSERT(pListServer!=NULL);
	m_pListServer=pListServer;
	//信息代理
	if (m_MessageProxyHelper.CreateInstance()==false) 
	{
		return false;
	}
	if (m_MessageProxyHelper->SetRichEditHwnd(&m_ChatMessage)==false) 
	{
		return false;
	}

	//设置组件
	IUnknownEx * pIUnknownEx=QUERY_ME_INTERFACE(IUnknownEx);
	/*
	if (m_UserListView.m_GameRankManager.CreateInstance()==false)
	{
	return false;
	}
	*/

	if (m_ClientSocket.CreateInstance()==false) 
	{
		return false;
	}
	if (m_ClientSocket->SetTCPSocketSink(pIUnknownEx)==false)
	{
		return false;
	}
	if (m_ClientUserManager.SetUserManagerSink(pIUnknownEx)==false)
	{
		return false;
	}
	if (m_IPCRecvCopyData.SetChannelMessageSink(pIUnknownEx)==false) 
	{
		return false;
	}


	//设置颜色
	tagMessageOption MessageOption;
	memset(&MessageOption,0,sizeof(MessageOption));
	MessageOption.crName=g_GlobalOption.m_crMsgName;
	MessageOption.crSystemHead=g_GlobalOption.m_crMsgSysHead;
	MessageOption.crSystemString=g_GlobalOption.m_crMsgSysString;
	m_MessageProxyHelper->SetMessageOption(MessageOption);

	//建立共享内存
	tagGameServer * pGameServer=m_pListServer->GetItemInfo();
	_sntprintf(m_szShareName,sizeof(m_szShareName),TEXT("0x8%ld%ld%ld%ld"),pGameServer->dwServerAddrCNN,pGameServer->wServerPort,time(NULL),rand()%100);
	m_hShareMemory=(HANDLE)CreateFileMapping((HANDLE)0xFFFFFFFFFFFF,NULL,PAGE_READWRITE,0,sizeof(tagShareMemory),m_szShareName);
	if ((m_hShareMemory==NULL)||(GetLastError()==ERROR_ALREADY_EXISTS))
	{
		return false;
	}
	m_pShareMemory=(tagShareMemory *)MapViewOfFile(m_hShareMemory,FILE_MAP_ALL_ACCESS,0,0,0);

	//设置共享内存
	memset(m_pShareMemory,0,sizeof(tagShareMemory));
	m_pShareMemory->wDataSize=sizeof(tagShareMemory);
	m_pShareMemory->hWndGameServer=m_hWnd;
	m_pShareMemory->hWndGamePlaza=AfxGetMainWnd()->m_hWnd;

	//配置信息
	m_pServerOption=g_GlobalOption.GetServerOption(pListServer);
	m_pGameOption=g_GlobalOption.GetGameOption(pListServer->GetListKind());
	ASSERT(m_pGameOption!=NULL);
	ASSERT(m_pServerOption!=NULL);

	//CArray<CGameRoomInfo,CGameRoomInfo> & m_arrayGameRoomInfo = m_gameListManager->m_arrayGameRoomInfo;
	//CArray<CGameInfo,CGameInfo> & m_arrayGameInfoCollection = m_gameListManager->m_arrayGameInfoCollection;
	//int nCount = m_arrayGameRoomInfo.GetCount();
	//CString strGamename="";
	//WORD wKindID=0;

	//for(BYTE j=0;j<m_arrayGameInfoCollection.GetCount()&&nCount>0;j++)
	//{
	//	//查找 游戏相关资源信息
	//	if(m_arrayGameInfoCollection[j].m_wKindID == m_arrayGameRoomInfo[0].GetKindID())
	//	{
	//		strGamename = TEXT("Games/") + m_arrayGameInfoCollection[j].m_strName+TEXT(".dll");
	//		break;
	//	}
	//}
	//HINSTANCE  hInstance = LoadLibrary(strGamename);
	//if (hInstance)
	//{
	//	if (!m_pngGameICO.IsNull())
	//	{
	//		m_pngGameICO.DestroyImage();
	//	}
	//	m_pngGameICO.LoadImage(hInstance, TEXT("DESK_UNIT_ICO"));
	//}
	//FreeLibrary(hInstance);

	// 	//属性变量
	// 	m_PropertyAttributeArrary.RemoveAll();
	// 	m_FlowerAttributeArrary.RemoveAll();



	return true;
}

//连接服务器
bool CRoomViewItem::ConnectGameServer()
{
	//服务器把IP地址转换成十进制pGameServer->dwServerAddr.
	m_ServiceStatus=ServiceStatus_Connecting;
	tagGameServer * pGameServer=m_pListServer->GetItemInfo();

	// 0电信，2自动
	if (0 == g_GlobalOption.m_byNetWorkTag  || 2 == g_GlobalOption.m_byNetWorkTag)
	{
		return (m_ClientSocket->Connect(pGameServer->dwServerAddrCNN,pGameServer->wServerPort)==CONNECT_SUCCESS);
	}
	return (m_ClientSocket->Connect(pGameServer->dwServerAddrCUN,pGameServer->wServerPort)==CONNECT_SUCCESS);
}

//发送规则
bool CRoomViewItem::SendUserRule()
{
	if (m_ServiceStatus!=ServiceStatus_Serviceing) return false;
	return SendUserRulePacket();
}

//私语信息
bool CRoomViewItem::SendUserWhisper(IUserItem * pIUserItem)
{
	//获取用户
	ASSERT(pIUserItem!=NULL);
	if (pIUserItem==NULL) return false;

	//激活窗口
	CShortMessage *pShortMessageWnd = ActiveShortWnd(pIUserItem->GetUserData()->dwUserID,pIUserItem,true);
	if (pShortMessageWnd==NULL)	
	{
		ShowMessageBox(TEXT("短信息窗口太多了，请关闭部分短信息窗口再试！"),MB_ICONINFORMATION);
	}


	return true;
}

//用户菜单
bool CRoomViewItem::ShowUserInfoMenu(IUserItem * pIUserItem, CPoint Point)
{
	//判断状态
	if (m_ServiceStatus!=ServiceStatus_Serviceing) return false;

	//用户判断
	if (pIUserItem==NULL) 
	{
		m_MessageProxyHelper->InsertGeneralString(TEXT("此用户已经离开房间了 "),RGB(0,0,0),MS_NORMAL,true);
		return false;
	}

	//构造菜单
	CMenu UserInfoMenu;
	if (UserInfoMenu.CreatePopupMenu()==FALSE) return false;

	//获取玩家
	tagUserData * pUserData=pIUserItem->GetUserData();
	tagUserData * pMeUserData=m_pMeUserItem->GetUserData();

	//用户信息
	WORD wTableID=pUserData->wTableID;
	WORD wChiarID=pUserData->wChairID;
	BYTE cbUserStatus=pUserData->cbUserStatus;

	//玩家信息
	WORD wMeTableID=pMeUserData->wTableID;
	WORD wMeChiarID=pMeUserData->wChairID;
	BYTE cbMeUserStatus=pMeUserData->cbUserStatus;

	//设置变量
	TCHAR szBuffer[1024]=TEXT("");
	m_dwMenuUserID=pUserData->dwUserID;
	bool bMySelf=(pIUserItem==m_pMeUserItem);

	////常用菜单
	//if ((bMySelf==false)&&(m_cbHideUserInfo==FALSE)) 
	//{
	//	UserInfoMenu.AppendMenu(0,IDM_UM_WISPER,TEXT("发短私聊信息..."));
	//	UserInfoMenu.SetDefaultItem(IDM_UM_WISPER,MF_BYCOMMAND);
	//}
	//UserInfoMenu.AppendMenu(0,IDM_UM_COPY_NAME,TEXT("拷贝用户名"));

	////聊天对象
	//if ((bMySelf==false)&&(m_cbHideUserInfo==FALSE)&&(m_dwChatUserID!=pUserData->dwUserID))
	//{
	//	_sntprintf(szBuffer,CountArray(szBuffer),TEXT("与 [ %s ] 交谈"),pUserData->szName);
	//	UserInfoMenu.AppendMenu(0,IDM_UM_SET_CHAT,szBuffer);
	//}
	//if (m_szChatName[0]!=0)
	//{
	//	_sntprintf(szBuffer,CountArray(szBuffer),TEXT("取消与 [ %s ] 交谈"),m_szChatName);
	//	UserInfoMenu.AppendMenu(0,IDM_UM_CANECL_CHAT,szBuffer);
	//}

	////查看位置
	//UserInfoMenu.AppendMenu(MF_SEPARATOR);
	//if (bMySelf==false) UserInfoMenu.AppendMenu(0,IDM_UM_LOOK_LOCATION,TEXT("查看玩家的位置"));
	//else UserInfoMenu.AppendMenu(0,IDM_UM_LOOK_LOCATION,TEXT("查看我的位置"));
	//if (pUserData->wTableID==INVALID_TABLE) UserInfoMenu.EnableMenuItem(IDM_UM_LOOK_LOCATION,MF_BYCOMMAND|MF_GRAYED);

	////邀请游戏
	//if ((bMySelf==false)&&(m_cbHideUserInfo==FALSE)&&(cbMeUserStatus!=US_PLAY)&&(wMeTableID!=INVALID_TABLE)&&(cbUserStatus<US_PLAY))
	//{
	//	_sntprintf(szBuffer,CountArray(szBuffer),TEXT("邀请 [ %s ] 一起游戏"),pUserData->szName);
	//	UserInfoMenu.AppendMenu(0,IDM_UM_INVITE_PLAY,szBuffer);
	//}

	////功能菜单
	//UserInfoMenu.AppendMenu(MF_SEPARATOR);
	//UserInfoMenu.AppendMenu(0,IDM_UM_SEARCH_USER,TEXT("寻找玩家..."));
	//if ((bMySelf==false)&&(m_cbHideUserInfo==FALSE))
	//{
	//	UserInfoMenu.AppendMenu(0,IDM_UM_SET_FRIEND,TEXT("设为好友"));
	//	UserInfoMenu.AppendMenu(0,IDM_UM_SET_DETEST,TEXT("设为厌恶"));
	//	if (pUserData->cbCompanion==enCompanion_Friend) 
	//	{
	//		UserInfoMenu.CheckMenuItem(IDM_UM_SET_FRIEND,MF_BYCOMMAND|MF_CHECKED);
	//	}
	//	if (pUserData->cbCompanion==enCompanion_Detest) 
	//	{
	//		UserInfoMenu.CheckMenuItem(IDM_UM_SET_DETEST,MF_BYCOMMAND|MF_CHECKED);
	//	}
	//}

	////菜单分割
	//UserInfoMenu.AppendMenu(0,MF_SEPARATOR);

//游戏 ID
	_sntprintf(szBuffer,CountArray(szBuffer),TEXT("帐号：%s"),pUserData->szAccounts);
	UserInfoMenu.AppendMenu(0,0,szBuffer);
	//玩家名字
	LPCTSTR pszMember[]={TEXT("普通会员"),TEXT("中级会员"),TEXT("高级会员")};
	if ((m_cbHideUserInfo==FALSE)&&(pUserData->cbMemberOrder>0)&&(pUserData->cbMemberOrder<CountArray(pszMember)))
	{
		BYTE cbMemberOrder=pUserData->cbMemberOrder;
		_sntprintf(szBuffer,CountArray(szBuffer),TEXT("昵称：%s [ %s ]"),pUserData->szName, pszMember[cbMemberOrder]);
	}
	else _sntprintf(szBuffer,CountArray(szBuffer),TEXT("昵称：%s"),pUserData->szName);
	UserInfoMenu.AppendMenu(0,0,szBuffer);

	//游戏 ID
	_sntprintf(szBuffer,CountArray(szBuffer),TEXT("用户 ID：%ld"),pUserData->dwUserID);
	UserInfoMenu.AppendMenu(0,0,szBuffer);

	//个性签名
	if (pUserData->szUnderWrite[0]!=0)
	{
		_sntprintf(szBuffer,CountArray(szBuffer),TEXT("个性签名：%s"),pUserData->szUnderWrite);
		UserInfoMenu.AppendMenu(0,0,szBuffer);
	}

	// 性别
	_sntprintf(szBuffer,CountArray(szBuffer),TEXT("性别：%s"), (0 == pUserData->cbGender) ? _T("男") : _T("女"));
	UserInfoMenu.AppendMenu(0,0,szBuffer);

	// 等级
	//_sntprintf(szBuffer,CountArray(szBuffer),TEXT("等级：%ld 等级称谓：%s"), pUserData->dwUserRight, pUserData->chGradeName);
	_sntprintf(szBuffer,CountArray(szBuffer),TEXT("等级：%ld 等级称谓：%s"), pUserData->dwGrade, pUserData->chGradeName);//Author<cxf>;DateTime<2012-03-15>
	UserInfoMenu.AppendMenu(0,0,szBuffer);

	//玩家位置
	if (pUserData->wTableID!=INVALID_TABLE)
	{
		_sntprintf(szBuffer,CountArray(szBuffer),TEXT("桌号：%d"), pUserData->wTableID+1);
		UserInfoMenu.AppendMenu(0,0,szBuffer);
	}

	// 金币
	_sntprintf(szBuffer,CountArray(szBuffer),TEXT("金币：%I64d"), pUserData->lGold);
	UserInfoMenu.AppendMenu(0,0,szBuffer);

	// 宝石
	_sntprintf(szBuffer,CountArray(szBuffer),TEXT("宝石：%I64d"), pUserData->lGem);
	UserInfoMenu.AppendMenu(0,0,szBuffer);

	//菜单分割
	UserInfoMenu.AppendMenu(0,MF_SEPARATOR);

	////经验数值
	//_sntprintf(szBuffer,CountArray(szBuffer),TEXT("经验值：%ld"),pUserData->lExperience);
	//UserInfoMenu.AppendMenu(0,0,szBuffer);

	// 计算逃跑率
	double dFleeRate=0.0;
	LONG lPlayCount=pIUserItem->GetUserPlayCount();
	if (pUserData->lFleeCount>0)
	{
		dFleeRate=(double)(pUserData->lFleeCount*100)/(double)lPlayCount;
		if (dFleeRate<0.01) dFleeRate=0.0;
	}
	//输赢信息
	//_sntprintf(szBuffer,CountArray(szBuffer),TEXT("总局数：%ld  赢局：%ld  输局：%ld  和局：%ld"),lPlayCount,pUserData->lWinCount,pUserData->lLostCount,pUserData->lDrawCount);
	_sntprintf(szBuffer,CountArray(szBuffer),TEXT("总局数：%ld  逃跑率：%5.2f%%"), lPlayCount, dFleeRate);
	UserInfoMenu.AppendMenu(0,0,szBuffer);

	//积分信息
	if (m_wGameGenre==GAME_GENRE_GOLD)
	{
		//_sntprintf(szBuffer,1024,TEXT("金币：%I64d  逃跑率：%5.2f%%"),pUserData->lScore,dFleeRate);
		_sntprintf(szBuffer,1024,TEXT("赢局：%ld  输局：%ld  和局：%ld  逃局：%ld"), 
			pUserData->lWinCount, pUserData->lLostCount, pUserData->lDrawCount, pUserData->lFleeCount);
		UserInfoMenu.AppendMenu(0,0,szBuffer);
	}
	else
	{
		_sntprintf(szBuffer,CountArray(szBuffer),TEXT("积分：%I64d  总局数：%ld  逃跑率：%5.2f%%"),pUserData->lScore,lPlayCount,dFleeRate);
		UserInfoMenu.AppendMenu(0,0,szBuffer);
	}

	//社团信息
	if (pUserData->szGroupName[0]!=0)
	{
		_sntprintf(szBuffer,CountArray(szBuffer),TEXT("社团：%s"),pUserData->szGroupName);
		UserInfoMenu.AppendMenu(0,0,szBuffer);
	}

	//用户管理
	if ((pMeUserData->dwUserID!=m_dwMenuUserID)&&(pMeUserData->dwMasterRight!=0L))
	{
		//插入菜单
		UserInfoMenu.AppendMenu(0,MF_SEPARATOR);
		UserInfoMenu.AppendMenu(0,IDM_UM_EXIT_TABLE,TEXT("踢出桌子"));
		UserInfoMenu.AppendMenu(0,IDM_UM_CUT_USER,TEXT("踢下线"));
		//UserInfoMenu.AppendMenu(0,IDM_UM_DISABLE_USER_IP,TEXT("禁用玩家IP..."));
		UserInfoMenu.AppendMenu(0,IDM_UM_LOOK_USER_IP,TEXT("查看用户 IP..."));
		UserInfoMenu.AppendMenu(0,IDM_UM_LIMIT_ACCOUN,TEXT("禁用用户帐号..."));
		UserInfoMenu.AppendMenu(0,IDM_UM_SEND_WARNNING,TEXT("发送警告信息..."));
		//UserInfoMenu.AppendMenu(0,IDM_UM_MANAGER_USER,TEXT("用户权限设置..."));

		//禁止菜单
		DWORD dwMasterRight=pMeUserData->dwMasterRight;
		if (CUserRight::CanSendWarning(dwMasterRight)==false) UserInfoMenu.EnableMenuItem(IDM_UM_SEND_WARNNING,MF_BYCOMMAND|MF_GRAYED);
		if (CUserRight::CanSeeUserIP(dwMasterRight)==false) UserInfoMenu.EnableMenuItem(IDM_UM_LOOK_USER_IP,MF_BYCOMMAND|MF_GRAYED);
		if (CUserRight::CanCutUser(dwMasterRight)==false) UserInfoMenu.EnableMenuItem(IDM_UM_CUT_USER,MF_BYCOMMAND|MF_GRAYED);
		if (CUserRight::CanForbidAccounts(dwMasterRight)==false) UserInfoMenu.EnableMenuItem(IDM_UM_LIMIT_ACCOUN,MF_BYCOMMAND|MF_GRAYED);
	}

	//房间管理
	if (pMeUserData->dwMasterRight!=0L)
	{
		//插入菜单
		UserInfoMenu.AppendMenu(0,MF_SEPARATOR);
		UserInfoMenu.AppendMenu(0,IDM_UM_ISSUE_MESSAGE,TEXT("发布系统消息..."));
		UserInfoMenu.AppendMenu(0,IDM_IM_SYSTEM_OPTION,TEXT("游戏房间配置..."));

		//禁止菜单
		DWORD dwMasterRight=pMeUserData->dwMasterRight;
		if (CUserRight::CanIssueMessage(dwMasterRight)==false) UserInfoMenu.EnableMenuItem(IDM_UM_ISSUE_MESSAGE,MF_BYCOMMAND|MF_GRAYED);
		if (CUserRight::CanServerOption(dwMasterRight)==false) UserInfoMenu.EnableMenuItem(IDM_IM_SYSTEM_OPTION,MF_BYCOMMAND|MF_GRAYED);
	}

	//显示菜单
	UserInfoMenu.TrackPopupMenu(TPM_LEFTALIGN|TPM_LEFTBUTTON,Point.x,Point.y,this);

	return true;
}

//资源句柄
HINSTANCE CRoomViewItem::GetResInstanceHandle()
{
	return g_GlobalUnits.m_PlatformResourceModule->GetResInstance();
}

//获取资源
tagServerViewImage CRoomViewItem::GetServerViewRes()
{
	return g_GlobalUnits.m_ServerViewImage;
}

//更新资源
bool CRoomViewItem::UpdateSkinResource()
{
	//获取资源
	HINSTANCE hInstance=GetResInstanceHandle();

	//列表区域
	m_EncircleList.ImageML.SetLoadInfo(GetServerViewRes().uUserListML,hInstance);
	m_EncircleList.ImageMR.SetLoadInfo(GetServerViewRes().uUserListMR,hInstance);
	m_EncircleList.ImageTL.SetLoadInfo(GetServerViewRes().uUserListTL,hInstance);
	m_EncircleList.ImageTM.SetLoadInfo(GetServerViewRes().uUserListTM,hInstance);
	m_EncircleList.ImageTR.SetLoadInfo(GetServerViewRes().uUserListTR,hInstance);
	m_EncircleList.ImageBL.SetLoadInfo(GetServerViewRes().uUserListBL,hInstance);
	m_EncircleList.ImageBM.SetLoadInfo(GetServerViewRes().uUserListBM,hInstance);
	m_EncircleList.ImageBR.SetLoadInfo(GetServerViewRes().uUserListBR,hInstance);

	//聊天区域
	m_EncircleChat.ImageML.SetLoadInfo(GetServerViewRes().uChatML,hInstance);
	m_EncircleChat.ImageMR.SetLoadInfo(GetServerViewRes().uChatMR,hInstance);
	m_EncircleChat.ImageTL.SetLoadInfo(GetServerViewRes().uChatTL,hInstance);
	m_EncircleChat.ImageTM.SetLoadInfo(GetServerViewRes().uChatTM,hInstance);
	m_EncircleChat.ImageTR.SetLoadInfo(GetServerViewRes().uChatTR,hInstance);
	m_EncircleChat.ImageBL.SetLoadInfo(GetServerViewRes().uChatBL,hInstance);
	m_EncircleChat.ImageBM.SetLoadInfo(GetServerViewRes().uChatBM,hInstance);
	m_EncircleChat.ImageBR.SetLoadInfo(GetServerViewRes().uChatBR,hInstance);

	//桌子区域
	m_EncircleTable.ImageML.SetLoadInfo(GetServerViewRes().uTableML,hInstance);
	m_EncircleTable.ImageMR.SetLoadInfo(GetServerViewRes().uTableMR,hInstance);
	m_EncircleTable.ImageTL.SetLoadInfo(GetServerViewRes().uTableTL,hInstance);
	m_EncircleTable.ImageTM.SetLoadInfo(GetServerViewRes().uTableTM,hInstance);
	m_EncircleTable.ImageTR.SetLoadInfo(GetServerViewRes().uTableTR,hInstance);
	m_EncircleTable.ImageBL.SetLoadInfo(GetServerViewRes().uTableBL,hInstance);
	m_EncircleTable.ImageBM.SetLoadInfo(GetServerViewRes().uTableBM,hInstance);
	m_EncircleTable.ImageBR.SetLoadInfo(GetServerViewRes().uTableBR,hInstance);

	//获取信息
	CSkinAide::GetEncircleInfo(m_ImageInfoChat,m_EncircleChat);
	CSkinAide::GetEncircleInfo(m_ImageInfoList,m_EncircleList);
	CSkinAide::GetEncircleInfo(m_ImageInfoTable,m_EncircleTable);

	////拆分控件
	//UINT uControlId = ((!m_bHideUserList)?GetServerViewRes().uBtHideUserList:GetServerViewRes().uBtShowUserList);
	////m_VorSplitter.ShowSplitterButton(uControlId,GetResInstanceHandle());

	////设置按钮
	//m_btAutoSit.SetButtonImage(GetServerViewRes().uBtAutoJoin,hInstance,false);
	//m_btBank.SetButtonImage(GetServerViewRes().uBtBank,hInstance,false);
	//m_btFindUser.SetButtonImage(GetServerViewRes().uBtSearchUser,hInstance,false);
	//m_btQuitRoom.SetButtonImage(GetServerViewRes().uBtQuitRoom,hInstance,false);
	////m_btPhrase.SetButtonImage(GetServerViewRes().uBtCleanScreen,hInstance,false);
	//m_btExpression.SetButtonImage(GetServerViewRes().uBtExpression,hInstance,false);
	//m_btColorSet.SetButtonImage(GetServerViewRes().uBtColorSet,hInstance,false);
	//m_btCleanScreen.SetButtonImage(GetServerViewRes().uBtCleanScreen,hInstance,false);
	//m_btSendChat.SetButtonImage(GetServerViewRes().uBtSendChat,hInstance,false);
	//m_btBugle.SetButtonImage(GetServerViewRes().uBtBugle,hInstance,false);

	//更新道具
	//m_PropertyBar.UpdateSkinResource();

	return true;
}

bool CRoomViewItem::IsAdministrationHavePurview(int iGrade)
{
	const tagGlobalUserData & UserData=g_GlobalUnits.GetGolbalUserData();
	if (CUserRight::CanSendWarning(UserData.nMasterRight))
	{
		return true;
	}

	return false;
}

void CRoomViewItem::SetCtrolAddF()
{
	if (IsAdministrationHavePurview())
	{
		OnBnClickedFindUser();
	}
}

//发送登录包
bool CRoomViewItem::SendLogonPacket()
{
	//获取信息
	BYTE cbBuffer[SOCKET_PACKET];
	tagGlobalUserData & GlobalUserData=g_GlobalUnits.GetGolbalUserData();

	//登录数据包
	CMD_GR_LogonByUserID * pLogonByUserID=(CMD_GR_LogonByUserID *)cbBuffer;
	pLogonByUserID->dwUserID=GlobalUserData.dwUserID;
	pLogonByUserID->dwPlazaVersion=g_GlobalUnits.GetPlazaVersion();
	pLogonByUserID->dwKindID = m_pListServer->GetListKind()->GetItemInfo()->wKindID;
	lstrcpyn(pLogonByUserID->szPassWord,GlobalUserData.szPassWord,sizeof(pLogonByUserID->szPassWord));

	//机器序列号
	tagClientSerial ClientSerial;
	g_GlobalUnits.GetClientSerial(ClientSerial);

	//发送数据包
	CSendPacketHelper Packet(cbBuffer+sizeof(CMD_GR_LogonByUserID),sizeof(cbBuffer)-sizeof(CMD_GR_LogonByUserID));
	Packet.AddPacket(&ClientSerial,sizeof(ClientSerial),DTP_COMPUTER_ID);
	m_ClientSocket->SendData(MDM_GR_LOGON,SUB_GR_LOGON_USERID,cbBuffer,sizeof(CMD_GR_LogonByUserID)+Packet.GetDataSize());

	return true;
}

//发送设置命令
bool CRoomViewItem::SendOptionsPacket()
{
	return true;
}

//发送起来命令
bool CRoomViewItem::SendStandUpPacket()
{
	m_ClientSocket->SendData(MDM_GR_USER,SUB_GR_USER_STANDUP_REQ);

	return true;
}

//发送强退命令
bool CRoomViewItem::SendLeftGamePacket()
{
	m_ClientSocket->SendData(MDM_GR_USER,SUB_GR_USER_LEFT_GAME_REQ);

	return true;
}
//发送取消排除消息 MDM_GP_USER  SUB_GR_USER_CANCEL
bool CRoomViewItem::SendCancelQueue()
{
	//m_ClientSocket->SendData(MDM_GP_USER,SUB_GR_USER_CANCEL);
	return true;
}

//发送房间规则
bool CRoomViewItem::SendUserRulePacket()
{
	//构造数据包
	CMD_GR_UserRule UserRule;
	UserRule.bLimitWin=m_pGameOption->m_bLimitWin;
	UserRule.bLimitFlee=m_pGameOption->m_bLimitFlee;
	UserRule.wWinRate=m_pGameOption->m_wWinRate;
	UserRule.wFleeRate=m_pGameOption->m_wFleeRate;
	UserRule.lMaxScore=m_pGameOption->m_lMaxScore;
	UserRule.lLessScore	=m_pGameOption->m_lLessScore;
	UserRule.bLimitScore=m_pGameOption->m_bLimitScore;
	UserRule.bPassword=m_pServerOption->m_bPassword;
	UserRule.bCheckSameIP=g_GlobalOption.m_bCheckSameIP;
	lstrcpyn(UserRule.szPassword,m_pServerOption->m_szPassword,CountArray(UserRule.szPassword));

	//发送数据包
	m_ClientSocket->SendData(MDM_GR_USER,SUB_GR_USER_RULE,&UserRule,sizeof(UserRule));

	return true;
}

//发送旁观命令
bool CRoomViewItem::SendLookonPacket(WORD wTableID, WORD wChairID, LPCTSTR pszTablePass)
{
	//构造数据包
	CMD_GR_UserSitReq UserUserSitReq;
	memset(&UserUserSitReq,0,sizeof(UserUserSitReq));
	UserUserSitReq.wTableID=wTableID;
	UserUserSitReq.wChairID=wChairID;
	lstrcpyn(UserUserSitReq.szTablePass,pszTablePass,sizeof(UserUserSitReq.szTablePass));
	UserUserSitReq.cbPassLen=CountStringBuffer(UserUserSitReq.szTablePass);

	//发送数据包
	WORD wSendSize=sizeof(UserUserSitReq)-sizeof(UserUserSitReq.szTablePass)+UserUserSitReq.cbPassLen;
	m_ClientSocket->SendData(MDM_GR_USER,SUB_GR_USER_LOOKON_REQ,&UserUserSitReq,wSendSize);

	return true;
}

//发送坐下命令
bool CRoomViewItem::SendSitDownPacket(WORD wTableID, WORD wChairID, LPCTSTR pszTablePass)
{
	//构造数据包
	CMD_GR_UserSitReq UserSitReq;
	memset(&UserSitReq,0,sizeof(UserSitReq));
	UserSitReq.wTableID=wTableID;
	UserSitReq.wChairID=wChairID;
	lstrcpyn(UserSitReq.szTablePass,pszTablePass,sizeof(UserSitReq.szTablePass));
	UserSitReq.cbPassLen=CountStringBuffer(UserSitReq.szTablePass);

	//发送数据包
	WORD wSendSize=sizeof(UserSitReq)-sizeof(UserSitReq.szTablePass)+UserSitReq.cbPassLen;
	m_ClientSocket->SendData(MDM_GR_USER,SUB_GR_USER_SIT_REQ,&UserSitReq,wSendSize);

	return true;
}

//旁观动作
bool CRoomViewItem::PerformLookonAction(WORD wTableID, WORD wChairID)
{
	//效验数据
	ASSERT(wTableID!=INVALID_TABLE);
	ASSERT(wChairID!=INVALID_CHAIR);

	//动作效验
	if (CanSitDownTable(wTableID,wChairID,true,true)==false) return false;

	//清理界面
	if ((m_wReqTableID!=INVALID_TABLE)&&(m_wReqChairID!=INVALID_CHAIR))
	{
		IUserItem * pIUserItem=m_TableFrame.GetUserInfo(m_wReqTableID,m_wReqChairID);
		if (pIUserItem==m_pMeUserItem)
		{
			m_TableFrame.SetUserInfo(m_wReqTableID,m_wReqChairID,NULL);
		}

	}

	//动作处理
	m_wReqTableID=wTableID;
	m_wReqChairID=wChairID;
	m_wFindTableID=INVALID_TABLE;
	m_TableFrame.EnsureVisibleTable(wTableID);
	SendLookonPacket(wTableID,wChairID,m_pServerOption->m_szEnterPassword);

	return true;
}

//坐下动作
bool CRoomViewItem::PerformSitDownAction(WORD wTableID, WORD wChairID)
{
	//效验数据
	ASSERT(wTableID!=INVALID_TABLE);
	ASSERT(wChairID!=INVALID_CHAIR);

	//动作效验
	if (CanSitDownTable(wTableID,wChairID,false,true)==false) return false;

	//清理界面
	if ((m_wReqTableID!=INVALID_TABLE)&&(m_wReqChairID!=INVALID_CHAIR))
	{
		IUserItem * pIUserItem=m_TableFrame.GetUserInfo(m_wReqTableID,m_wReqChairID);
		if (pIUserItem==m_pMeUserItem)
		{
			m_TableFrame.SetUserInfo(m_wReqTableID,m_wReqChairID,NULL);
		}
	}

	//动作处理
	m_wReqTableID=wTableID;
	m_wReqChairID=wChairID;
	m_wFindTableID=INVALID_TABLE;
	m_TableFrame.EnsureVisibleTable(wTableID);
	SendSitDownPacket(wTableID,wChairID,m_pServerOption->m_szEnterPassword);

	//预先显示
	if (m_TableFrame.GetChairCount()<=MAX_CHAIR_NORMAL)
	{
		IUserItem * pIUserItem=m_TableFrame.GetUserInfo(wTableID,wChairID);
		if(pIUserItem==NULL)
		{
			m_TableFrame.SetUserInfo(wTableID,wChairID,m_pMeUserItem);
		}
	}

	return true;
}

//发送聊天命令
bool CRoomViewItem::SendChatPacket(DWORD dwTargetUserID, LPCTSTR pszChatMessage, COLORREF crFontColor)
{
	//获取用户
	ASSERT(m_pMeUserItem!=NULL);
	tagUserData * pUserData=m_pMeUserItem->GetUserData();

	//构造数据
	CMD_GR_UserChat UserChat;
	UserChat.crFontColor=crFontColor;
	UserChat.dwTargetUserID=dwTargetUserID;
	UserChat.dwSendUserID=pUserData->dwUserID;
	lstrcpyn(UserChat.szChatMessage,pszChatMessage,CountArray(UserChat.szChatMessage));
	UserChat.wChatLength=CountStringBuffer(UserChat.szChatMessage);

	//发送命令
	WORD wSendSize=sizeof(UserChat)-sizeof(UserChat.szChatMessage)+UserChat.wChatLength;
	m_ClientSocket->SendData(MDM_GR_USER,SUB_GR_USER_CHAT,&UserChat,wSendSize);

	return true;
}

//发送游戏信息
bool CRoomViewItem::IPCSendGameInfo(CIPCSendCopyData * pSendCopyData)
{
	//效益参数
	ASSERT(pSendCopyData!=NULL);
	ASSERT(m_pMeUserItem!=NULL);

	//定义变量
	tagUserData * pUserData=m_pMeUserItem->GetUserData();
	CListKind * pListKind=m_pListServer->GetListKind();
	tagGameKind * pGameKind=pListKind->GetItemInfo();
	tagGameServer * pGameServer=m_pListServer->GetItemInfo();

	//构造数据
	IPC_GF_ServerInfo ServerInfo;
	memset(&ServerInfo,0,sizeof(ServerInfo));
	ServerInfo.dwUserID=pUserData->dwUserID;
	ServerInfo.wTableID=pUserData->wTableID;
	ServerInfo.wChairID=pUserData->wChairID;
	ServerInfo.wKindID=pGameKind->wKindID;
	ServerInfo.wServerID=pGameServer->wServerID;
	ServerInfo.wGameGenre=m_wGameGenre;
	ServerInfo.dwVideoAddr=m_dwVideoAddr;
	ServerInfo.cbHideUserInfo=m_cbHideUserInfo;
	ServerInfo.wChairCount=m_TableFrame.GetChairCount();
	lstrcpyn(ServerInfo.szKindName,pGameKind->szKindName,CountArray(ServerInfo.szKindName));
	lstrcpyn(ServerInfo.szServerName,pGameServer->szServerName,CountArray(ServerInfo.szServerName));
	//cxf
	ServerInfo.wSortID=pGameServer->wSortID;//游戏排序
	ServerInfo.lCellScore=pGameServer->lCellScore;//最低进入游戏所需金币

	//发送数据
	pSendCopyData->SendData(IPC_MAIN_CONFIG,IPC_SUB_SERVER_INFO,&ServerInfo,sizeof(ServerInfo));

	return true;
}

//发送用户
bool CRoomViewItem::IPCSendTableUsers(CIPCSendCopyData * pSendCopyData)
{
	//效益参数
	ASSERT(pSendCopyData!=NULL);

	//发送自己信息
	ASSERT(m_pMeUserItem!=NULL);
	tagUserData * pMeUserData=m_pMeUserItem->GetUserData();
	ASSERT(pMeUserData->wTableID!=(WORD)INVALID_TABLE);
	if (pMeUserData->wTableID==(WORD)INVALID_TABLE) return false;
	SendTableUser(m_pMeUserItem,pSendCopyData);

	//发送其他用户
	IUserItem * pIUserItem=NULL;
	tagUserData * pUserData=NULL;

	//游戏用户
	WORD wEnumIndex=0;
	while (true)
	{
		pIUserItem=m_ClientUserManager.EnumUserItem(wEnumIndex++);
		if (pIUserItem==NULL) break;
		if (pIUserItem==m_pMeUserItem) continue;
		pUserData=pIUserItem->GetUserData();
		if ((pUserData->wTableID==pMeUserData->wTableID)&&(pUserData->cbUserStatus!=US_LOOKON))
			SendTableUser(pIUserItem,pSendCopyData);
	};

	//旁观用户
	wEnumIndex=0;
	while (true)
	{
		pIUserItem=m_ClientUserManager.EnumUserItem(wEnumIndex++);
		if (pIUserItem==NULL) break;
		if (pIUserItem==m_pMeUserItem) continue;
		pUserData=pIUserItem->GetUserData();
		if ((pUserData->wTableID==pMeUserData->wTableID)&&(pUserData->cbUserStatus==US_LOOKON))
			SendTableUser(pIUserItem,pSendCopyData);
	};

	return true;
}

//启动游戏
int CRoomViewItem::StartGameClient()
{
	//判断是否已经启动
	ASSERT(m_pShareMemory!=NULL);
	if ((m_GameProcessInfo.hProcess!=NULL)&&(WaitForSingleObject(m_GameProcessInfo.hProcess,0)==WAIT_TIMEOUT))
	{
		if (m_pShareMemory->hWndGameFrame!=NULL)
		{
			//隐藏窗口
			ShowWindow(SW_HIDE);
			GetParent()->ShowWindow(SW_HIDE);


			//显示窗口
			::ShowWindow(m_pShareMemory->hWndGameFrame,SW_SHOW);
			::SetForegroundWindow(m_pShareMemory->hWndGameFrame);

			//发送信息
			CIPCSendCopyData SendCopyData(m_hWndChannel,m_hWnd);
			IPCSendGameInfo(&SendCopyData);
			IPCSendTableUsers(&SendCopyData);
			SendCopyData.SendData(IPC_MAIN_CONCTROL,IPC_SUB_START_FINISH);

			((CPlazaDlg*)GetParent())->UpdateMusicSound(false);
			return SR_ALREADY_EXIST;
		}
		else
		{
			::TerminateProcess(m_GameProcessInfo.hProcess,0);
			//显示窗口
			GetParent()->ShowWindow(SW_SHOW);
			ShowWindow(SW_SHOW);
		}
	}

	//清理数据
	m_hWndChannel=NULL;
	m_pShareMemory->hWndGameFrame=NULL;
	CloseHandle(m_GameProcessInfo.hThread);
	CloseHandle(m_GameProcessInfo.hProcess);
	memset(&m_GameProcessInfo,0,sizeof(m_GameProcessInfo));

	

	// del by HouGuoJiang 2011-11-25
	////隐藏窗口
	if (IsAdministrationHavePurview())
	{
		ShowWindow(SW_HIDE);							// 隐藏提示窗口
	}
	GetParent()->ShowWindow(SW_HIDE);		// 隐藏大厅选择房间的窗口

	//构造命令行
	CString strCommonLine;
	CListKind * pListKind=m_pListServer->GetListKind();
	tagGameKind * pGameKind=pListKind->GetItemInfo();

	// strCommonLine	{0x03e1ae90 "LandCrazy.exe /RoomToken:0x810235189125010132394129526 /ServerType:2 /HideUserInfo:0"}	ATL::CStringT<char,StrTraitMFC_DLL<char,ATL::ChTraitsCRT<char> > >
	strCommonLine.Format(TEXT("%s /RoomToken:%s /ServerType:%ld /HideUserInfo:%d"),pGameKind->szProcessName,m_szShareName,m_wGameGenre,m_cbHideUserInfo);

	//启动游戏客户端
	STARTUPINFO StartInfo;
	memset(&StartInfo,0,sizeof(StartInfo));
	StartInfo.cb=sizeof(StartInfo);
	StartInfo.wShowWindow=SW_SHOWMAXIMIZED;
	BOOL bSuccess=CreateProcess(NULL,strCommonLine.GetBuffer(),NULL,NULL,FALSE,CREATE_DEFAULT_ERROR_MODE,NULL,NULL,&StartInfo,&m_GameProcessInfo);
	strCommonLine.ReleaseBuffer();
	if (bSuccess==FALSE)
	{
		memset(&m_GameProcessInfo,0,sizeof(m_GameProcessInfo));
		return SR_CREATE_ERROR;
	}
	::memset(&m_tagGameFrameStatus, 0, sizeof(m_tagGameFrameStatus));

	((CPlazaDlg*)GetParent())->UpdateMusicSound(false);

	return SR_CREATE_SUCCESS;
}

// 线程工作函数
// 针对的问题：提示窗口隐藏太快了，游戏加载还未完成，进程如同突然消失了，客户不知所措
unsigned __stdcall  CRoomViewItem::thread_game_load_over(void* pParam) 	
{
	CRoomViewItem* pThis = (CRoomViewItem*)pParam;
	//DWORD  iWaitTime = 30000;	// ms ，最长的等待时间，
	DWORD dwResult;

	dwResult = WaitForMultipleObjects(GAME_LOAD_OVER_COUNT,  pThis->m_hEventGameLoadOver,  FALSE,  INFINITE);
	switch(dwResult)
	{
	case WAIT_TIMEOUT:			// 超时
	case WAIT_OBJECT_0 + 0:	// 接到游戏客户端的加载完毕的通知
		{
			if (pThis && pThis->GetSafeHwnd())
			{
				pThis->ShowWindow(SW_HIDE);		// 隐藏提示窗口
			}

			break;
		}
	case WAIT_OBJECT_0 + 1:	// 接到关提示框的通知，直接结束线程
		{
			return 0;	
		}
	}

	return 0;
}

// 建立单线程
bool CRoomViewItem::createThreads()
{
	if(m_hEventGameLoadOver[0] && m_hEventGameLoadOver[1])
	{
		m_hThread = (HANDLE)_beginthreadex( NULL, 0, &thread_game_load_over, (void*)this, 0, &m_dwThreadID);

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

//关闭游戏
void CRoomViewItem::CloseGameClient()
{
	//关闭检测窗口存在
	m_tagGameFrameStatus.bIsOpen = false;
	KillTimer(IDI_GAME_FRAME_IS_EXIST);

	//显示窗口
	GetParent()->SetForegroundWindow();
	GetParent()->ShowWindow(SW_SHOW);

	//发送消息
	if (m_GameProcessInfo.hProcess!=NULL)
	{
		DWORD dwResult=::WaitForSingleObject(m_GameProcessInfo.hProcess,1000);
		if (dwResult==WAIT_TIMEOUT) 
			::TerminateProcess(m_GameProcessInfo.hProcess,0);

		::memset(&m_tagGameFrameStatus, 0, sizeof(m_tagGameFrameStatus));
	}

	//清理数据
	if (NULL != m_hWndChannel)
	{
		m_hWndChannel=NULL;
	}
	if (NULL != m_pShareMemory->hWndGameFrame)
	{
		m_pShareMemory->hWndGameFrame=NULL;
	}

	CloseHandle(m_GameProcessInfo.hThread);
	CloseHandle(m_GameProcessInfo.hProcess);
	memset(&m_GameProcessInfo,0,sizeof(m_GameProcessInfo));
	//cxf 1108 m_AutoQueue.EnableWindow(TRUE);
	return;
}

//更新标题
void CRoomViewItem::UpdateTitleText()
{
	//获取位置
	//CRect rcVorSplitter;
	//m_VorSplitter.GetWindowRect(&rcVorSplitter);
	//ScreenToClient(&rcVorSplitter);

	//刷新界面
	//CRect rcInvalidate;
	//rcInvalidate.SetRect(0,0,rcVorSplitter.left-BUTTON_AREA_WIDTH,m_ImageInfoTable.nTBorder);
	//InvalidateRect(&rcInvalidate);

	return;
}

//绘画桌子
void CRoomViewItem::DrawTableFrame(CDC * pDC)
{
	//获取位置
	CRect rcClient,rcVorSplitter;
	GetClientRect(&rcClient);
	//m_VorSplitter.GetWindowRect(&rcVorSplitter);
	//ScreenToClient(&rcVorSplitter);

	//绘画框架
	CRect rcTable;
	rcTable.right=rcClient.right;
	rcTable.left=0;
	rcTable.bottom=rcClient.Height();
	rcTable.top=0;
	CSkinAide::DrawEncircleFrame(pDC,rcTable,m_EncircleTable);

	//房间标题
	TCHAR szRoomTitle[128]=TEXT("");
	CListKind * pListKind=m_pListServer->GetListKind();
	_sntprintf(szRoomTitle,sizeof(szRoomTitle),TEXT("[ %s ] - [ %s ] （%ld 人）"),pListKind->GetItemInfo()->szKindName,
		m_pListServer->GetItemInfo()->szServerName,m_ClientUserManager.GetOnLineCount());

	//输出标题
	pDC->SetBkMode(TRANSPARENT);
	pDC->SetTextColor(RGB(64,64,64));
	pDC->SelectObject(CSkinResourceManager::GetDefaultFont());
	CRect rcTitle(10,5,rcClient.left-BUTTON_AREA_WIDTH-8,m_ImageInfoTable.nTBorder);
	pDC->DrawText(szRoomTitle,lstrlen(szRoomTitle),&rcTitle,DT_END_ELLIPSIS|DT_VCENTER|DT_SINGLELINE);

	return;
}

//左视图区
//void CRoomViewItem::DrawLeftViewFrame(CDC * pDC)
//{
//	//获取位置
//	CRect rcClient,rcVorSplitter,rcHorSplitter;
//	GetClientRect(&rcClient);
//	m_VorSplitter.GetWindowRect(&rcVorSplitter);
//	m_HorSplitter.GetWindowRect(&rcHorSplitter);
//	ScreenToClient(&rcVorSplitter);
//	ScreenToClient(&rcHorSplitter);
//
//	//绘画聊框
//	CRect rcChat;
//	rcChat.left=rcVorSplitter.right;
//	rcChat.right=rcClient.Width();
//	rcChat.top=rcHorSplitter.bottom-rcHorSplitter.Height()/2;
//	rcChat.bottom=rcClient.Height();
//	CSkinAide::DrawEncircleFrame(pDC,rcChat,m_EncircleChat);
//
//	//绘画列表
//	CRect rcList;
//	rcList.left=rcVorSplitter.right;
//	rcList.right=rcClient.Width();
//	rcList.top=0;
//	rcList.bottom=rcHorSplitter.top+rcHorSplitter.Height()/2;
//	CSkinAide::DrawEncircleFrame(pDC,rcList,m_EncircleList);
//
//	return;
//}

//查找桌子
bool CRoomViewItem::FindGameTable(tagFindTable & FindInfo)
{
	//变量定义
	WORD wNullCount=0;
	ITableView * pTableView=NULL;
	WORD wChairUser=m_TableFrame.GetChairCount();
	WORD wMeTableID=m_pMeUserItem->GetUserData()->wTableID;

	//搜索桌子
	for (WORD i=0;i<m_TableFrame.GetTableCount();i++)
	{
		FindInfo.wResultTableID=(FindInfo.wBeginTableID+i)%m_TableFrame.GetTableCount();
		if (wMeTableID!=FindInfo.wResultTableID)
		{
			//获取桌子
			pTableView=m_TableFrame.GetTableArrayPtr(FindInfo.wResultTableID);
			ASSERT(pTableView!=NULL);

			//判断是否开始游戏
			if (pTableView->QueryPlayFlag()==true) continue;

			//过滤密码
			bool bTablePass=pTableView->QueryPassFlag();
			if ((FindInfo.bFilterPass==true)&&(bTablePass==true)) continue;

			//寻找空位置
			wNullCount=pTableView->GetNullChairCount(FindInfo.wResultChairID);

			//效验规则
			if (CanSitDownTable(FindInfo.wResultTableID,FindInfo.wResultChairID,false,false)==false) continue;

			//判断数目
			if (wNullCount>0)
			{
				if ((FindInfo.bNotFull==true)&&(wNullCount<wChairUser))
				{
					return true;
				}
				if ((FindInfo.bOneNull==true)&&(wNullCount==1)) 
				{
					return true;
				}
				if ((FindInfo.bTwoNull==true)&&(wNullCount==2)) 
				{
					return true;
				}
				if ((FindInfo.bAllNull==true)&&(wNullCount==wChairUser)) 
				{
					return true;
				}
			}
		}
	}

	//设置数据
	FindInfo.wResultTableID=INVALID_TABLE;
	FindInfo.wResultChairID=INVALID_CHAIR;

	return false;
}

//调整控件
void CRoomViewItem::RectifyControl(int nWidth, int nHeight)
{
	//状态判断
	if (m_bInitDialog==false) return;
	if ((nWidth==0)||(nHeight==0)) return;

	//变量定义
	UINT uFlags=SWP_NOACTIVATE|SWP_NOZORDER|SWP_NOCOPYBITS;
	const INT nXScreen=GetSystemMetrics(SM_CXSCREEN);

	HDWP hDwp=BeginDeferWindowPos(32);
	//桌子区域
	//DeferWindowPos(hDwp,m_TableFrame,NULL,0,35,659,558,uFlags);

	uFlags=SWP_NOACTIVATE|SWP_NOZORDER|SWP_NOCOPYBITS|SWP_NOSIZE;
	// 	DeferWindowPos(hDwp,m_btnReLogon,NULL,821,680,0,0,uFlags);
	// 	DeferWindowPos(hDwp,m_btnReturn,NULL,847,680,0,0,uFlags);
	// 	DeferWindowPos(hDwp,m_btnMinsize,NULL,872,680,0,0,uFlags);
	// 	DeferWindowPos(hDwp,m_btnSet,NULL,897,680,0,0,uFlags);
	// 	DeferWindowPos(hDwp,m_btnClose,NULL,921,680,0,0,uFlags);
	// 	DeferWindowPos(hDwp,m_btnFriend,NULL,292,29,0,0,uFlags);
	// 	DeferWindowPos(hDwp,m_btnTool,NULL,423,29,0,0,uFlags);
	// 	DeferWindowPos(hDwp,m_btnTask,NULL,554,29,0,0,uFlags);
	// 	DeferWindowPos(hDwp,m_btnDepository,NULL,684,29,0,0,uFlags);
	// 	DeferWindowPos(hDwp,m_btnShop,NULL,812,29,0,0,uFlags);
	// 	DeferWindowPos(hDwp,m_ChatMessage,NULL,19,511,263,161,SWP_NOACTIVATE|SWP_NOZORDER|SWP_NOCOPYBITS);
	// 	DeferWindowPos(hDwp,m_editInput,NULL,22,677,169,19,SWP_NOACTIVATE|SWP_NOZORDER|SWP_NOCOPYBITS);
	// 	DeferWindowPos(hDwp,m_btSendChat,NULL,194,676,0,0,uFlags);
	// 	DeferWindowPos(hDwp,m_btExpression,NULL,218,676,0,0,uFlags);
	// 	DeferWindowPos(hDwp,m_ChatObject,NULL,242,676,38,21,SWP_NOACTIVATE|SWP_NOZORDER|SWP_NOCOPYBITS);
	DeferWindowPos(hDwp,m_btnPgDn,NULL,500,593,0,0,uFlags);
	DeferWindowPos(hDwp,m_btnPgUp,NULL,300,593,0,0,uFlags);
	//cxf 1108 DeferWindowPos(hDwp,m_AutoQueue,NULL,229,76,0,0,uFlags);
	//DeferWindowPos(hDwp,m_btnDespokenTable,NULL,228,126,0,0,uFlags);
	EndDeferWindowPos(hDwp);

	m_rcQueue.left = nWidth/2-100;
	m_rcQueue.top = 0;
	m_rcQueue.right = m_rcQueue.left + 200;
	m_rcQueue.bottom = m_rcQueue.top + 48;

	/*cxf m_ptOrder.x = 125;
	m_ptOrder.y = 602;
	m_ptWinGemsToday.y = 602;
	m_ptWinGemsToday.x = 333;*/
	//重画界面
	Invalidate(FALSE);
	//UpdateWindow();
	return;
}

//显示消息
int CRoomViewItem::ShowMessageBox(LPCTSTR pszMessage, UINT nType)
{
	//关闭提示
	g_GlobalAttemper.DestroyStatusWnd(this);
	tagGameServer * pGameServer=m_pListServer->GetItemInfo();
	int nResult=ShowInformationEx(pszMessage,0,nType,pGameServer->szServerName);
	return nResult;
}

//发送用户
bool CRoomViewItem::SendTableUser(IUserItem * pIUserItem, CIPCSendCopyData * pSendCopyData)
{
	//效验参数
	ASSERT(pIUserItem!=NULL);
	ASSERT(pSendCopyData!=NULL);

	//定义变量
	BYTE cbBuffer[IPC_PACKAGE];
	tagUserData * pUserData=pIUserItem->GetUserData();
	tagUserInfoHead * pUserInfoHead=(tagUserInfoHead *)cbBuffer;

	//构造数据
	memset(cbBuffer,0,sizeof(cbBuffer));
	pUserInfoHead->dwUserID=pUserData->dwUserID;
	pUserInfoHead->dwGameID=pUserData->dwGameID;
	pUserInfoHead->dwGroupID=pUserData->dwGroupID;
	pUserInfoHead->dwUserRight=pUserData->dwUserRight;
	pUserInfoHead->lLoveliness=pUserData->lLoveliness;
	pUserInfoHead->dwMasterRight=pUserData->dwMasterRight;
	pUserInfoHead->wFaceID=pUserData->wFaceID;
	pUserInfoHead->dwCustomFaceVer=pUserData->dwCustomFaceVer;
	pUserInfoHead->wTableID=pUserData->wTableID;
	pUserInfoHead->wChairID=pUserData->wChairID;
	pUserInfoHead->cbGender=pUserData->cbGender;
	pUserInfoHead->cbUserStatus=pUserData->cbUserStatus;
	pUserInfoHead->cbMemberOrder=pUserData->cbMemberOrder;
	pUserInfoHead->cbMasterOrder=pUserData->cbMasterOrder;
	/*pUserInfoHead->UserScoreInfo.lScore=pUserData->lScore;
	pUserInfoHead->UserScoreInfo.lGameGold=pUserData->lGameGold;*/
	pUserInfoHead->UserScoreInfo.lMoney=pUserData->lMoney;
	pUserInfoHead->UserScoreInfo.lWinCount=pUserData->lWinCount;
	pUserInfoHead->UserScoreInfo.lLostCount=pUserData->lLostCount;
	pUserInfoHead->UserScoreInfo.lDrawCount=pUserData->lDrawCount;
	pUserInfoHead->UserScoreInfo.lFleeCount=pUserData->lFleeCount;
	pUserInfoHead->UserScoreInfo.lExperience=pUserData->lExperience;

	pUserInfoHead->UserScoreInfo.dwGrade=pUserData->dwGrade;
	//pUserInfoHead->UserScoreInfo.dwOrder=pUserData->dwOrder;
	pUserInfoHead->UserScoreInfo.lGem=pUserData->lGem;
	pUserInfoHead->UserScoreInfo.lGold=pUserData->lGold;

	//等级称谓
	::memcpy(pUserInfoHead->UserScoreInfo.chGradeName, ((CPlazaDlg*)GetParent())->m_pGradeInfo[pUserData->dwGrade-1].strName, sizeof(pUserInfoHead->UserScoreInfo.chGradeName));

	//用户游戏局数 Author<cxf>;DateTime<2012-03-05>
	pUserInfoHead->UserScoreInfo.lKindIDWinCount=pUserData->lKindIDWinCount;
	pUserInfoHead->UserScoreInfo.lKindIDLostCount=pUserData->lKindIDLostCount;
	pUserInfoHead->UserScoreInfo.lKindIDFleeCount=pUserData->lKindIDFleeCount;
	pUserInfoHead->UserScoreInfo.lKindIDDrawCount=pUserData->lKindIDDrawCount;

	//pUserInfoHead->UserScoreInfo.lHonor=pUserData->lHonor;
	//pUserInfoHead->UserScoreInfo.lCredit=pUserData->lCredit;


	//pUserInfoHead->lMoney = pUserData->lMoney;
	for ( WORD wPropID = 0; wPropID < PROPERTY_COUNT; ++wPropID )
	{
		pUserInfoHead->dwPropResidualTime[wPropID] = pUserData->dwPropResidualTime[wPropID];
	}

	//叠加数据
	WORD wHeadSize=sizeof(tagUserInfoHead);
	CSendPacketHelper SendPacketHelper(cbBuffer+wHeadSize,sizeof(cbBuffer)-wHeadSize);
	SendPacketHelper.AddPacket(pUserData->szName,CountStringBuffer(pUserData->szName),DTP_USER_ACCOUNTS);
	SendPacketHelper.AddPacket(&pUserData->cbCompanion,sizeof(pUserData->cbCompanion),DTP_USER_COMPANION);
	SendPacketHelper.AddPacket(pUserData->szUnderWrite,CountStringBuffer(pUserData->szUnderWrite),DTP_UNDER_WRITE);
	SendPacketHelper.AddPacket(pUserData->szGroupName,CountStringBuffer(pUserData->szGroupName),DTP_USER_GROUP_NAME);

	//发送数据
	WORD wSendSize=wHeadSize+SendPacketHelper.GetDataSize();
	pSendCopyData->SendData(IPC_MAIN_USER,IPC_SUB_USER_COME,cbBuffer,wSendSize);


	const tagGlobalUserData & userdata = g_GlobalUnits.GetGolbalUserData();
	if (userdata.dwUserID == pUserData->dwUserID)
	{
		//添加密码
		IPC_UserPassWord userPassword;
		CopyMemory(userPassword.chUserPassWord, userdata.szPassWord, sizeof(userPassword.chUserPassWord));
		pSendCopyData->SendData(IPC_MAIN_USER,IPC_SUB_GETUSERPASSWORD,&userPassword,sizeof(userPassword));
	}
	return true;
}

//发送消息
bool CRoomViewItem::SendProcessData(WORD wMainCmdID, WORD wSubCmdID, void * pData, WORD wDataSize)
{
	//发送消息
	CIPCSendCopyData IPCSendCopyData(m_hWndChannel,m_hWnd);
	return IPCSendCopyData.SendData(wMainCmdID,wSubCmdID,pData,wDataSize);
}

//能否坐下
bool CRoomViewItem::CanSitDownTable(WORD wTableID, WORD wChairID, bool bLookon, bool bShowError)
{
	//效验参数
	ASSERT(wTableID!=INVALID_TABLE);
	ASSERT(wChairID!=INVALID_CHAIR);
	if (wTableID>=m_TableFrame.GetTableCount())
	{
		return false;
	}
	if (wChairID>=m_TableFrame.GetChairCount()) 
	{
		return false;
	}
	if (m_ServiceStatus!=ServiceStatus_Serviceing) 
	{
		return false;
	}


	//判断响应
	if ((m_wReqTableID!=INVALID_TABLE)||(m_wReqChairID!=INVALID_CHAIR))
	{
		if (bShowError==true)
		{
			LPCTSTR pszDescribe=TEXT("上次请求还没有收到服务器回应,请稍候！");
			ShowMessageBox(pszDescribe,MB_ICONINFORMATION);
			return false;
		}
	}

	//判断玩家
	ITableView * pITableView=m_TableFrame.GetTableArrayPtr(wTableID);
	IUserItem * pITableUserItem=pITableView->GetUserInfo(wChairID);
	if ((bLookon==true)&&(pITableUserItem==NULL))
	{
		return false;
	}
	if ((bLookon==false)&&(pITableUserItem!=NULL)) 
	{
		return false;
	}


	//判断状态
	BYTE cbUserStatus=m_pMeUserItem->GetUserData()->cbUserStatus;
	if (cbUserStatus==US_PLAY)
	{
		if (bShowError==true)
		{
			LPCTSTR pszDescribe=TEXT("您正在游戏中，暂时不能换位置，请先结束游戏！");
			ShowMessageBox(pszDescribe,MB_ICONINFORMATION);
		}
		return false;
	}

	//判断游戏状态
	bool bGameStart=pITableView->QueryPlayFlag();
	if ((bGameStart==false)&&(bLookon==true)&&(m_pMeUserItem->GetUserData()->dwMasterRight==0)) return false;

	//判断密码
	if ((pITableView->QueryPassFlag()==true))
	{
		if (bShowError==true)
		{
			//创建窗口
			CDlgEnterTablePass DlgEnterTablePass;
			ZeroMemory(m_pServerOption->m_szEnterPassword,sizeof(m_pServerOption->m_szEnterPassword));
			DlgEnterTablePass.SetPassword(m_pServerOption->m_szEnterPassword,CountArray(m_pServerOption->m_szEnterPassword));

			//显示窗口
			DlgEnterTablePass.DoModal();
			if(m_pServerOption->m_szEnterPassword[0]==0)
			{
				return false;
			}
		}
		else 
		{
			return false;
		}

	}

	//旁观判断
	if (bLookon==true)
	{
		return true;
	}


	//规则判断
	tagUserData * pTableUserData=NULL;
	for (WORD i=0;i<m_TableFrame.GetTableCount();i++)
	{
		//获取用户
		pITableUserItem=pITableView->GetUserInfo(i);
		if ((pITableUserItem==NULL)||(pITableUserItem==m_pMeUserItem)) continue;
		pTableUserData=pITableUserItem->GetUserData();

		//厌恶玩家
		if (g_GlobalOption.m_bLimitDetest==true)
		{
			if (pTableUserData->cbCompanion==enCompanion_Detest)
			{
				if (bShowError==true)
				{
					TCHAR szDescribe[128]=TEXT("");
					_sntprintf(szDescribe,sizeof(szDescribe),TEXT("您设置了不与不受欢迎的玩家游戏，此桌有您不欢迎的玩家 [ %s ] ！"),pTableUserData->szName);
					ShowMessageBox(szDescribe,MB_ICONINFORMATION);
				}
				return false;
			}
		}

		//胜率效验
		if (m_pGameOption->m_bLimitWin)
		{
			WORD wWinRate=0;
			LONG lAllCount=pITableUserItem->GetUserPlayCount();
			if (lAllCount>0) wWinRate=(WORD)(pTableUserData->lWinCount*10000/lAllCount);
			if (wWinRate<m_pGameOption->m_wWinRate)
			{
				if (bShowError==true)
				{
					TCHAR szDescribe[128]=TEXT("");
					_sntprintf(szDescribe,sizeof(szDescribe),TEXT("[ %s ] 的胜率太低了，与您的设置不符！"),pTableUserData->szName);
					ShowMessageBox(szDescribe,MB_ICONINFORMATION);
				}
				return false;
			}
		}

		//逃率效验
		if (m_pGameOption->m_bLimitFlee)
		{
			WORD wFillRate=0;
			LONG lAllCount=pITableUserItem->GetUserPlayCount();
			if (lAllCount>0) wFillRate=(WORD)(pTableUserData->lFleeCount*10000/lAllCount);
			if ((wFillRate>0)&&(wFillRate>m_pGameOption->m_wFleeRate))
			{
				if (bShowError==true)
				{
					TCHAR szDescribe[128]=TEXT("");
					_sntprintf(szDescribe,sizeof(szDescribe),TEXT("[ %s ] 的逃跑率太高了，与您的设置不符！"),pTableUserData->szName);
					ShowMessageBox(szDescribe,MB_ICONINFORMATION);
				}
				return false;
			}
		}

		//积分效验
		if (m_pGameOption->m_bLimitScore)
		{
			LONGLONG lTableUserScore=pTableUserData->lScore;
			if (lTableUserScore>m_pGameOption->m_lMaxScore)
			{
				if (bShowError==true)
				{
					TCHAR szDescribe[128]=TEXT("");
					_sntprintf(szDescribe,sizeof(szDescribe),TEXT("[ %s ] 的积分太高了，与您的设置不符！"),pTableUserData->szName);
					ShowMessageBox(szDescribe,MB_ICONINFORMATION);
				}
				return false;
			}
			if (lTableUserScore<m_pGameOption->m_lLessScore)
			{
				if (bShowError==true)
				{
					TCHAR szDescribe[128]=TEXT("");
					_sntprintf(szDescribe,sizeof(szDescribe),TEXT("[ %s ] 的积分太低了，与您的设置不符！"),pTableUserData->szName);
					ShowMessageBox(szDescribe,MB_ICONINFORMATION);
				}
				return false;
			}
		}
	}

	return true;
}

//网络连接消息
bool __cdecl CRoomViewItem::OnEventTCPSocketLink(WORD wSocketID, INT nErrorCode)
{
	//错误处理
	if (nErrorCode!=0)
	{
		ShowMessageBox(TEXT("游戏房间连接失败，您暂时不能进入此游戏房间！"),MB_ICONINFORMATION);
		g_GlobalAttemper.DestroyStatusWnd(this);
		//关闭房间
		//cxf 1108 GetParent()->PostMessage(WM_COMMAND,IDM_CALL_EXIT_GAME,0);
		CloseRoomViewItem();

		//		((CGameFrame*)AfxGetMainWnd())->CloseRoomViewItem(this);
		return true;
	}

	//发送登录信息
	SendLogonPacket();
	m_ServiceStatus=ServiceStatus_EfficacyUser;

	return true;
}

//网络读取消息
bool __cdecl CRoomViewItem::OnEventTCPSocketRead(WORD wSocketID, CMD_Command Command, VOID * pData, WORD wDataSize)
{
	if (!m_bIsDestroyStatus)
	{//销毁等待框
		g_GlobalAttemper.DestroyStatusWnd(this);
		m_bIsDestroyStatus =true;
	}

	bool bRet = false;
	switch (Command.wMainCmdID)
	{
	case MDM_GR_LOGON:			//登录消息
		{
			bRet = OnSocketMainLogon(Command,pData,wDataSize);
			break;
		}
	case MDM_GR_USER:			//用户消息
		{
			bRet = OnSocketMainUser(Command,pData,wDataSize);
			break;
		}
	case MDM_GR_INFO:			//配置信息
		{
			bRet = OnSocketMainInfo(Command,pData,wDataSize);
			break;
		}
	case MDM_GR_STATUS:			//状态信息
		{
			bRet = OnSocketMainStatus(Command,pData,wDataSize);
			break;
		}
	case MDM_GR_SYSTEM:			//系统消息
		{
			bRet = OnSocketMainSystem(Command,pData,wDataSize);
			break;
		}
	case MDM_GR_SERVER_INFO:	//房间信息
		{
			bRet = OnSocketMainServerInfo(Command,pData,wDataSize);
			break;
		}
	case MDM_GF_GAME:			//游戏消息
	case MDM_GF_FRAME:			//框架消息
	case MDM_GF_VIDEO:			//视频消息
	case MDM_GF_TASK:			//任务消息
	case MDM_GF_PRESENT:		//礼物消息
		{
			bRet = OnSocketMainGameFrame(Command,pData,wDataSize);
			break;
		}
		// 	case MDM_GF_TASK:			//任务消息
		// 		{ 
		// 			return OnSocketTask(Command,pData,wDataSize);
		// 		}
	}
	if (!bRet)
	{
#ifdef ADMIN_PLAZA
#ifdef _DEBUG
		char chData[100]={0};
		sprintf(chData, "OnEventTCPSocketRead:%d", Command.wMainCmdID);
		AfxMessageBox(chData);
#endif
#endif
	}
	return true;
}

//网络关闭消息
bool __cdecl CRoomViewItem::OnEventTCPSocketShut(WORD wSocketID, BYTE cbShutReason)
{
	//判断关闭
	bool bCloseRoomView=(m_ServiceStatus!=ServiceStatus_Serviceing && cbShutReason != SHUT_REASON_NORMAL);

	//关闭处理
	g_GlobalAttemper.DestroyStatusWnd(this);
	m_ServiceStatus=ServiceStatus_NetShutDown;
	if (cbShutReason!=SHUT_REASON_NORMAL)
	{
		if ((cbShutReason==SHUT_REASON_REMOTE)||(cbShutReason==SHUT_REASON_TIME_OUT))
		{
			//m_MessageProxyHelper->InsertSystemString("由于网络问题，您已经与服务器断开连接，请重新连接",0);
			ShowMessageBox("由于网络问题，您已经与服务器断开连接，请重新连接",MB_ICONINFORMATION);
		}
		else
		{
			ShowMessageBox("由于网络数据包处理失败，网络中断了",MB_ICONINFORMATION);
		}
		//关闭房间
		//GetParent()->SendMessage(WM_COMMAND,IDM_CALL_GAME_LIST,0);

		//SendProcessData(IPC_MAIN_CONCTROL,IPC_SUB_CLOSE_FRAME,NULL,0);
		//CloseRoomViewItem();
		SendRequestCloseRoomMsg(1);
	}
	else
	{
		//SendProcessData(IPC_MAIN_CONCTROL,IPC_SUB_CLOSE_FRAME,NULL,0);
		CloseGameClient();
	}

	//关闭房间
	//	if (bCloseRoomView==true) ((CGameFrame*)AfxGetMainWnd())->CloseRoomViewItem(this);

	return true;
}

//登录消息
bool CRoomViewItem::OnSocketMainLogon(CMD_Command Command, void * pData, WORD wDataSize)
{
	ASSERT(Command.wMainCmdID==MDM_GR_LOGON);
	switch (Command.wSubCmdID)
	{
	case SUB_GR_LOGON_SUCCESS:		//登录成功
		{
			//设置变量
			m_ServiceStatus=ServiceStatus_RecvConfigInfo;

			return true;
		}
	case SUB_GR_LOGON_ERROR:		//登录失败
		{
			//效验参数
			CMD_GR_LogonError * pLogonError=(CMD_GR_LogonError *)pData;
			ASSERT(wDataSize>=(sizeof(CMD_GR_LogonError)-sizeof(pLogonError->szErrorDescribe)));
			if (wDataSize<(sizeof(CMD_GR_LogonError)-sizeof(pLogonError->szErrorDescribe))) return false;

			//关闭连接
			//cxf 11-18 g_GlobalAttemper.DestroyStatusWnd(this);
			m_ClientSocket->CloseSocket();
			m_ServiceStatus=ServiceStatus_NetShutDown;
			//cxf 1108 m_AutoQueue.EnableWindow(FALSE);
			//关闭房间
			GetParent()->ShowWindow(SW_SHOW);
			GetParent()->PostMessage(WM_COMMAND,IDM_CALL_ROOM_LIST,0);
			CloseRoomViewItem();

			//显示消息
			WORD wDescribeSize=wDataSize-(sizeof(CMD_GR_LogonError)-sizeof(pLogonError->szErrorDescribe));
			if (wDescribeSize>0)
			{
				pLogonError->szErrorDescribe[wDescribeSize-1]=0;

				tagGameServer * pGameServer=m_pListServer->GetItemInfo();
				int nResult=ShowInformationEx(pLogonError->szErrorDescribe,0,MB_ICONINFORMATION,pGameServer->szServerName);
			}

			return true;
		} 
	case SUB_GR_LOGON_FINISH:		//登录完成
		{
			//设置界面
			if (m_TableFrame.GetChairCount()<=MAX_CHAIR_NORMAL)
			{
				m_TableFrame.ShowUserInfo(true);
			}

			//cxf 11-18 g_GlobalAttemper.DestroyStatusWnd(this);
			//((CTableListDlg*)AfxGetMainWnd())->ActiveRoomViewItem(this);

			//设置变量
			m_ServiceStatus=ServiceStatus_Serviceing;
			RectifyControl(960,720);

			//发送规则
			SendUserRulePacket();

			//cxf 1108 m_AutoQueue.EnableWindow(TRUE);

			//重入判断
			ASSERT(m_pMeUserItem!=NULL);
			tagUserData * pUserData=m_pMeUserItem->GetUserData();
			//Author<cxf>;DateTime<2012-03-01>;Description<屏蔽>
			//m_bMasterRight=(pUserData->dwMasterRight>0)?true:false;//Author<cxf>;DateTime<2012-02-27>


			if (pUserData->wTableID!=INVALID_TABLE)
			{
				int iResult=StartGameClient();
			}

			if (!IsAdministrationHavePurview())
			{
				if (INVALID_TABLE == pUserData->wTableID)
				{//自动排队...
					m_bQueueState=true;
					OnAutoQueue();
				}

				//启动动画
				SetTimer(IDI_LOADING_ANIMACTION,100,NULL);
			}

			return true;
		}
	case SUB_GR_LOGON_FULL:
		{
			//效验参数
			ASSERT(Command.wMainCmdID==MDM_GR_LOGON);
			ASSERT(Command.wSubCmdID==SUB_GR_LOGON_FULL);
			//cxf 11-18 g_GlobalAttemper.DestroyStatusWnd(this);
			m_bLogonFull=true;
			return true;
		}
	}

	return true;
}

//用户消息
bool CRoomViewItem::OnSocketMainUser(CMD_Command Command, void * pData, WORD wDataSize)
{
	ASSERT(Command.wMainCmdID==MDM_GR_USER);
	switch (Command.wSubCmdID)
	{
	case SUB_GR_USER_COME:			//用户进入
		{
			return OnSocketSubUserCome(Command,pData,wDataSize);
		}
	case SUB_GR_USER_STATUS:		//用户状态
		{
			return OnSocketSubStatus(Command,pData,wDataSize);
		}
	case SUB_GR_USER_SCORE:			//用户分数
		{
			return OnSocketSubScore(Command,pData,wDataSize);
		}
	case SUB_GR_USER_RIGHT:			//用户权限
		{
			return OnSocketSubRight(Command,pData,wDataSize);
		}
	case SUB_GR_MEMBER_ORDER:		//会员等级
		{
			return OnSocketSubMemberOrder(Command,pData,wDataSize);
		}
	case SUB_GR_SIT_FAILED:			//坐下失败
		{
			return OnSocketSubSitFailed(Command,pData,wDataSize);
		}
	case SUB_GR_USER_CHAT:			//用户聊天
		{
			return OnSocketSubChat(Command,pData,wDataSize);
		}
	case SUB_GR_USER_WISPER:		//用户私语
		{
			return OnSocketSubWisper(Command,pData,wDataSize);
		}
	case SUB_GR_USER_INVITE:		//邀请玩家
		{
			return OnSocketSubUserInvite(Command,pData,wDataSize);
		}
		//排队成功      彭文添加   
	case SUB_GR_AUTO_QUEUE_SUCC:
	case SUB_GR_AUTO_QUEUE:			//自动排队
		{
			//效验参数
			ASSERT(Command.wMainCmdID==MDM_GR_USER);
			//ASSERT(Command.wSubCmdID==SUB_GR_AUTO_QUEUE);

			//消息处理
			CMD_GR_AutoQueue * pAutoQueue=(CMD_GR_AutoQueue *)pData;
			m_QueueNum=pAutoQueue->iAutoQueueNum; //排队人数
			//AfxMessageBox(str);

			//更新界面
			InvalidateRect(m_rcQueue, FALSE);

			m_bQueueState = false;
			HINSTANCE hInstance=AfxGetInstanceHandle();
			//cxf 1108 m_AutoQueue.LoadRgnImage(hInstance,IDB_CANCEL_AUTO_QUEUE);
			//cxf 1108 m_ToolTipCtrl.AddTool(&m_AutoQueue,TEXT("取消排队"));
			//cxf 1108 m_AutoQueue.EnableWindow(TRUE);
			//cxf 1108 m_AutoQueue.Invalidate(TRUE);
			//UpdateWindow();
			return true;
		}
	case SUB_GR_QUEUE_COUNT:	//排队人数
		{
			return OnSocketSubQueueCount(Command,pData,wDataSize);
		}
	case SUB_GR_AUTO_QUEUE_FAIL:	//排队失败
		{
			return OnSocketSubQueueFail(Command,pData,wDataSize);
		}
	case SUB_RECVACTIONINFO:		//接受动作
		{
			//效验参数
			ASSERT(wDataSize>=sizeof(IPC_UserActionInfo));
			if (wDataSize<sizeof(IPC_UserActionInfo)) return false;

			if (NULL != pData)
			{
				SendProcessData(IPC_MAIN_USER, IPC_SUB_RECVACTIONINFO, pData, wDataSize);
			}
			return true;
		}
	case SUB_RECV_SOUND_MESS:		//接受语音
		{
			//效验参数
			ASSERT(wDataSize>=sizeof(IPC_UserLanguageInfo));
			if (wDataSize<sizeof(IPC_UserLanguageInfo)) return false;

			if (NULL != pData)
			{
				SendProcessData(IPC_MAIN_USER, IPC_SUB_RECVGLINFO, pData, wDataSize);
			}
			return true;
		}
	case SUB_GR_NO_GOLD:		//金币不足，退回房间列表
		{
			//ASSERT(wDataSize>=sizeof(CMD_GR_SendWarning));
			//if (wDataSize<sizeof(CMD_GR_SendWarning))
			//{
			//	return false;
			//}
			CMD_GR_SendWarning * warning=(CMD_GR_SendWarning *)pData;
			if (NULL != warning)
			{
				//退出自动排队
				m_bQueueState=false;
				OnAutoQueue();
				//cxf 1101ShowMessageBox(warning->szWarningMessage,MB_ICONINFORMATION);
				m_ConfirmGold.DoModal();
				//退出排队
				SendRequestCloseRoomMsg(1);
			}

			//返回游戏大厅
			//GetParent()->ShowWindow(SW_SHOW);
			//GetParent()->SendMessage(WM_COMMAND,IDM_CALL_GAME_LIST,0);
			//CloseRoomViewItem();
			return true;
		}
	case SUB_GR_LOW_GRADE:		//等级太高，退回房间列表
		{
			CMD_GR_SendWarning * warning=(CMD_GR_SendWarning *)pData;
			if (NULL != warning)
			{
				//退出自动排队
				m_bQueueState=false;
				OnAutoQueue();
				//cxf 1101 ShowMessageBox(warning->szWarningMessage,MB_ICONINFORMATION);
				m_ConfirmGrade.DoModal();
				//退出排队
				SendRequestCloseRoomMsg(1);
			}

			return true;
		}
	case SUB_GR_AUTO_QUEUE_CANCEL_FALL:		//取消排队失败
		{
			return true;
		}
	case SUB_GR_AUTO_QUEUE_CANCEL_SUCC:		//取消排队成功
		{
			m_bQueueState = true;
			HINSTANCE hInstance=AfxGetInstanceHandle();
			//cxf 1108 m_AutoQueue.LoadRgnImage(hInstance,IDB_AUTO_QUEUE);
			//cxf 1108 m_ToolTipCtrl.AddTool(&m_AutoQueue,TEXT("自动排队"));
			//cxf 1108 m_AutoQueue.EnableWindow(TRUE);
			//cxf 1108 m_AutoQueue.Invalidate(TRUE);
			return true;
		}
	case SUB_GF_QUIT_GAME_SERVER_SUCC:		//返回到房间列表
		{
			m_bRequestServerExit = false;
			KillTimer(IDI_GAME_FRAME_IS_EXIST);
			KillTimer(IDI_REQUEST_SERVER_EXIT_MSG);
			if (0 == m_btExitTag)
			{//直接返回到登录界面
				//关闭房间
				GetParent()->PostMessage(WM_COMMAND,IDM_CALL_EXIT_GAME,0);
				CloseRoomViewItem();
			}
			else if (1 == m_btExitTag)
			{//从桌子列表返回到房间列表

				GetParent()->ShowWindow(SW_SHOW);
				GetParent()->PostMessage(WM_COMMAND,IDM_CALL_ROOM_LIST,0);
				CloseRoomViewItem();
			}
			else if (2 == m_btExitTag)
			{//返回到游戏大厅

				GetParent()->ShowWindow(SW_SHOW);
				GetParent()->PostMessage(WM_COMMAND,IDM_CALL_GAME_LIST,0);
				CloseRoomViewItem();
			}
			else if (3 == m_btExitTag)
			{//退出整个游戏
				GetParent()->PostMessage(WM_COMMAND,IDM_CALL_CANCEL_GAME,0);
			}
			return true;
		}
	case SUB_GR_GO_HALL:	//返回大厅
		{
			KillTimer(IDI_GAME_FRAME_IS_EXIST);
			KillTimer(IDI_REQUEST_SERVER_EXIT_MSG);
			GetParent()->ShowWindow(SW_SHOW);
			GetParent()->PostMessage(WM_COMMAND,IDM_CALL_GAME_LIST,0);
			CloseRoomViewItem();
			break;
		}
	default:
		{
			break;
		}
	}

	return true;
}

//配置消息
bool CRoomViewItem::OnSocketMainInfo(CMD_Command Command, void * pData, WORD wDataSize)
{
	ASSERT(Command.wMainCmdID==MDM_GR_INFO);
	switch (Command.wSubCmdID)
	{
	case SUB_GR_SERVER_INFO:	//房间信息
		{
			//效验数据
			ASSERT(wDataSize>=sizeof(CMD_GR_ServerInfo));
			if (wDataSize<sizeof(CMD_GR_ServerInfo))
			{
				return false;
			}


			//消息处理
			CMD_GR_ServerInfo * pServerInfo=(CMD_GR_ServerInfo *)pData;

			//设置变量
			m_wGameGenre=pServerInfo->wGameGenre;//游戏类型
			m_dwVideoAddr=pServerInfo->dwVideoAddr;
			m_cbHideUserInfo=pServerInfo->cbHideUserInfo;

			//获取信息
			CListKind * pListKind=m_pListServer->GetListKind();
			tagGameKind * pGameKind=pListKind->GetItemInfo();
			IUnknownEx * pIUnknownEx=QUERY_ME_INTERFACE(IUnknownEx);
			// 			m_UserListView.m_wKindID=pServerInfo->wKindID;
			// 			m_UserListView.m_wGameGenre=pServerInfo->wGameGenre;

			//创建桌子
			try
			{
				m_TableFrame.CreateTableFrame(this,100);
				m_TableFrame.InitTableFrame(pServerInfo->wTableCount,pServerInfo->wChairCount,(pServerInfo->cbHideUserInfo==TRUE),pGameKind,pIUnknownEx);
				CRect rect;
				GetClientRect(rect);
				m_TableFrame.OnSize(0, rect.Width(), rect.Height());
			}
			catch (...)
			{
				//关闭网络
				m_ClientSocket->CloseSocket();
				//cxf 11-18 g_GlobalAttemper.DestroyStatusWnd(this);

				//提示消息
				int nResult=ShowMessageBox(TEXT("游戏房间资源加载失败，重新下载安装可能会解决问题，是否立即下载？"),MB_ICONQUESTION|MB_YESNO);
				if (nResult==IDYES) 
				{
					CListKind * pListKind=m_pListServer->GetListKind();
					tagGameKind * pGameKind=pListKind->GetItemInfo();
					TCHAR szProcessName[MODULE_LEN];
					CopyMemory(szProcessName, pGameKind->szProcessName, sizeof(szProcessName));
					TCHAR * pCh = strrchr(szProcessName, '.');
					if (pCh) *pCh = '\0';
					g_GlobalAttemper.DownLoadClient(szProcessName,pGameKind->wKindID,true,1);
					//g_GlobalAttemper.DownLoadClient(pGameKind->szKindName,pGameKind->wKindID,true);

					GetParent()->ShowWindow(SW_SHOW);
					GetParent()->PostMessage(WM_COMMAND,IDM_CALL_GAME_LIST,0);
					CloseRoomViewItem();
				}
				else
				{
					OnBnReturn();
				}

				//关闭房间
				IRoomViewItem * pIRoomViewItem=QUERY_ME_INTERFACE(IRoomViewItem);
				//((CGameFrame*)AfxGetMainWnd())->CloseRoomViewItem(pIRoomViewItem);

				return false;
			}

			return true;
		}
	case SUB_GR_COLUMN_INFO:	//列表解释
		{
			//变量定义
			CMD_GR_ColumnInfo * pColumnInfo=(CMD_GR_ColumnInfo *)pData;
			WORD wHeadSize=sizeof(CMD_GR_ColumnInfo)-sizeof(pColumnInfo->ColumnItem);

			//效验参数
			ASSERT(wDataSize>=wHeadSize);
			ASSERT((wHeadSize+pColumnInfo->wColumnCount*sizeof(pColumnInfo->ColumnItem[0]))==wDataSize);
			if (wDataSize<wHeadSize) 
			{
				return false;
			}

			if ((wHeadSize+pColumnInfo->wColumnCount*sizeof(pColumnInfo->ColumnItem[0]))!=wDataSize) 
			{
				return false;
			}


			//设置列表
			CopyMemory(&m_ListColumnInfo,pColumnInfo,__min(wDataSize,sizeof(m_ListColumnInfo)));
			//m_UserListView.SetColumnDescribe(pColumnInfo->ColumnItem,pColumnInfo->wColumnCount);

			return true;
		}
	case SUB_GR_CONFIG_FINISH:	//配置完成
		{
			//资源目录
			TCHAR szResDirectory[128]=TEXT("");
			WORD wStringIndex=0,wBufferCount=CountArray(szResDirectory);

			//构造目录
			const tagGameKind * pGameKind = GetKindInfo();
			ZeroMemory(szResDirectory,wBufferCount*sizeof(TCHAR));
			lstrcpyn(szResDirectory,(LPCTSTR)pGameKind->szProcessName,wBufferCount);
			while ((szResDirectory[wStringIndex]!=0)&&(szResDirectory[wStringIndex]!=TEXT('.'))) wStringIndex++;

			//字符终止
			szResDirectory[wStringIndex]=0;

			//游戏等级
			// 			ASSERT(m_UserListView.m_GameRankManager.GetInterface()!=NULL);
			// 			/*bool bSuccess=*/m_UserListView.m_GameRankManager->LoadGameLevel(szResDirectory,m_wGameGenre);

			//显示房间
			ShowWindow(SW_SHOW);
			//((CTableListDlg*)AfxGetMainWnd())->ActiveRoomViewItem(this);

			//设置变量
			m_ServiceStatus=ServiceStatus_RecvRoomInfo;

			return true;
		}
	}

	return true;
}

//状态消息
bool CRoomViewItem::OnSocketMainStatus(CMD_Command Command, void * pData, WORD wDataSize)
{
	ASSERT(Command.wMainCmdID==MDM_GR_STATUS);
	switch (Command.wSubCmdID)
	{
	case SUB_GR_TABLE_INFO:		//桌子信息
		{
			//变量定义
			CMD_GR_TableInfo * pTableInfo=(CMD_GR_TableInfo *)pData;
			const WORD wHeadSize=sizeof(CMD_GR_TableInfo)-sizeof(pTableInfo->TableStatus);

			//效验数据
			ASSERT(wDataSize>=wHeadSize);
			ASSERT((wHeadSize+pTableInfo->wTableCount*sizeof(pTableInfo->TableStatus[0]))==wDataSize);
			if (wDataSize<wHeadSize)
			{
				return false;
			}

			if ((wHeadSize+pTableInfo->wTableCount*sizeof(pTableInfo->TableStatus[0]))!=wDataSize) 
			{
				return false;
			}


			//消息处理
			for (WORD i=0;i<pTableInfo->wTableCount;i++)
			{
				m_TableFrame.SetPassFlag(i,pTableInfo->TableStatus[i].bTableLock?true:false);
				m_TableFrame.SetPlayFlag(i,pTableInfo->TableStatus[i].bPlayStatus?true:false);
			}

			return true;
		}
	case SUB_GR_TABLE_STATUS:	//桌子状态
		{
			//效验数据
			ASSERT(wDataSize>=sizeof(CMD_GR_TableStatus));
			if (wDataSize<sizeof(CMD_GR_TableStatus))
			{
				return false;
			}


			//消息处理
			CMD_GR_TableStatus * pTableStatus=(CMD_GR_TableStatus *)pData;
			ASSERT(pTableStatus->wTableID<m_TableFrame.GetTableCount());
			if (pTableStatus->wTableID<m_TableFrame.GetTableCount())
			{
				//变量定义
				IUserItem * pIUserItem=NULL;
				tagUserData * pUserData=NULL;
				BYTE bPlayStatus=m_TableFrame.QueryPlayFlag(pTableStatus->wTableID);

				//设置用户
				if (bPlayStatus!=pTableStatus->bPlayStatus)
				{
					BYTE cbUserStatus=pTableStatus->bPlayStatus?US_PLAY:US_SIT;
					for (WORD i=0;i<m_TableFrame.GetChairCount();i++)
					{
						pIUserItem=m_TableFrame.GetUserInfo(pTableStatus->wTableID,i);
						if (pIUserItem!=NULL)
						{
							pUserData=pIUserItem->GetUserData();
							if (pUserData->cbUserStatus!=US_OFFLINE) 
							{
								pUserData->cbUserStatus=cbUserStatus;
								OnUserItemUpdate(pIUserItem);
							}
						}
					}
				}

				//设置桌子
				m_TableFrame.SetPassFlag(pTableStatus->wTableID,pTableStatus->bTableLock?true:false);
				m_TableFrame.SetPlayFlag(pTableStatus->wTableID,pTableStatus->bPlayStatus?true:false);

				//通知游戏
				if (bPlayStatus!=pTableStatus->bPlayStatus)
				{
					tagUserData * pMeUserData=m_pMeUserItem->GetUserData();
					if (pMeUserData->wTableID==pTableStatus->wTableID)
					{
						WORD wSubCmdID=pTableStatus->bPlayStatus?IPC_SUB_GAME_START:IPC_SUB_GAME_FINISH;
						SendProcessData(IPC_MAIN_USER,wSubCmdID,NULL,0);
					}
				}
			}

			return true;
		}
	}

	return true;
}

//系统消息
bool CRoomViewItem::OnSocketMainSystem(CMD_Command Command, void * pData, WORD wDataSize)
{
	ASSERT(Command.wMainCmdID==MDM_GR_SYSTEM);
	switch (Command.wSubCmdID)
	{
	case SUB_GR_MESSAGE:		//系统消息
		{
			//效验参数
			CMD_GR_Message * pMessage=(CMD_GR_Message *)pData;
			ASSERT(wDataSize>(sizeof(CMD_GR_Message)-sizeof(pMessage->szContent)));
			if (wDataSize<=(sizeof(CMD_GR_Message)-sizeof(pMessage->szContent)))
			{
				return false;
			}

			//消息处理
			WORD wHeadSize=sizeof(CMD_GR_Message)-sizeof(pMessage->szContent);
			ASSERT(wDataSize==(wHeadSize+pMessage->wMessageLength*sizeof(TCHAR)));
			if (wDataSize!=(wHeadSize+pMessage->wMessageLength*sizeof(TCHAR))) 
			{
				return false;
			}

			pMessage->szContent[pMessage->wMessageLength-1]=0;
			if (pMessage->wMessageType&SMT_INFO) m_MessageProxyHelper->InsertSystemString(pMessage->szContent,MS_NORMAL);
			if (pMessage->wMessageType&SMT_EJECT) GetParent()->PostMessage(IDM_CALL_SHOW_CUSTOMER, (WPARAM)pData, (LPARAM)wDataSize);

			//关闭连接
			if (pMessage->wMessageType&SMT_INTERMIT_LINE) 
			{
				m_ClientSocket->CloseSocket();
				CloseGameClient();
				SendRequestCloseRoomMsg(1);				
			}
			else if (SMT_CLOSE_ROOM&pMessage->wMessageType)
			{
				CloseGameClient();
				SendRequestCloseRoomMsg(1);
			}

			return true;
		}
	case SUB_GR_LOGON_AFFICHE:			//系统公告
		{
			char* chSysData = new char[wDataSize+1];
			if (NULL != chSysData)
			{
				::memcpy(chSysData, pData, wDataSize);
				chSysData[wDataSize] = '\0';

				/*cxf 11-26 if ((NULL!=m_RoomViewAffiche) && (::IsWindow(m_RoomViewAffiche->GetSafeHwnd())))
				{
				m_RoomViewAffiche->SetGameAfficheInfo(chSysData);
				}*/
				m_strSaveGameAffiche.clear();
				m_strSaveGameAffiche.append(chSysData);
				delete chSysData;

				//发送游戏公告信息
				SendToGameFrameAfficheInfo();
			}
			chSysData = NULL;

			return true;
		}
	}

	return true;
}

//房间消息
bool CRoomViewItem::OnSocketMainServerInfo(CMD_Command Command, void * pData, WORD wDataSize)
{
	ASSERT(Command.wMainCmdID==MDM_GR_SERVER_INFO);
	switch (Command.wSubCmdID)
	{
	case SUB_GR_ONLINE_COUNT_INFO:		//在线信息
		{
			//效验数据
			ASSERT(wDataSize%sizeof(tagOnLineCountInfo)==0);
			if (wDataSize%sizeof(tagOnLineCountInfo)!=0) 
			{
				return false;
			}

			//消息处理
			WORD wKindID=0;
			DWORD dwKindOnLineCount=0,dwAllOnLineCount=0L;
			WORD wInfoCount=wDataSize/sizeof(tagOnLineCountInfo);
			tagOnLineCountInfo * pOnLineCountInfo=(tagOnLineCountInfo *)pData;
			for (int i=0;i<wInfoCount;i++)
			{
				wKindID=(pOnLineCountInfo+i)->wKindID;
				dwKindOnLineCount=(pOnLineCountInfo+i)->dwOnLineCount;
				dwAllOnLineCount+=dwKindOnLineCount;
				g_GlobalUnits.m_ServerListManager.UpdateGameKindOnLine(wKindID,dwKindOnLineCount);
			}

			//更新总数
			g_GlobalUnits.m_ServerListManager.UpdateGameOnLineCount(dwAllOnLineCount);

			return true;
		}
	}

	return true;
}

//游戏消息
bool CRoomViewItem::OnSocketMainGameFrame(CMD_Command Command, void * pData, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize<=SOCKET_PACKET);
	if (wDataSize>SOCKET_PACKET)
	{
		return false;
	}


	//构造数据
	IPC_SocketPackage SocketPackage;
	memset(&SocketPackage,0,sizeof(SocketPackage));
	SocketPackage.Command=Command;
	if (wDataSize>0)
	{
		ASSERT(pData!=NULL);
		CopyMemory(SocketPackage.cbBuffer,pData,wDataSize);
	}

	//数据缓冲
	if (m_hWndChannel==NULL)
	{
		//状态判断
		ASSERT(m_pMeUserItem!=NULL);
		if (m_pMeUserItem==NULL)
		{
			return false;
		}


		//获取用户
		tagUserData * pUserData=m_pMeUserItem->GetUserData();

		//数据缓冲
		if ((pUserData->wTableID!=INVALID_TABLE)&&(pUserData->wChairID!=INVALID_CHAIR))
		{
			//设置变量
			m_wPacketTableID=pUserData->wTableID;
			m_wPacketChairID=pUserData->wChairID;

			//构造数据
			BYTE cbBuffer[SOCKET_BUFFER];
			CopyMemory(cbBuffer,&Command,sizeof(Command));
			CopyMemory(&cbBuffer[sizeof(Command)],pData,wDataSize);

			//插入数据
			m_PacketDataStorage.InsertData(0,cbBuffer,sizeof(Command)+wDataSize);
		}
	}
	else
	{
		//发送数据
		WORD wSendSize=sizeof(SocketPackage.Command)+wDataSize;
		SendProcessData(IPC_MAIN_SOCKET,IPC_SUB_SOCKET_RECV,&SocketPackage,wSendSize);
	}

	/*

	//道具数目
	if ( Command.wSubCmdID == SUB_GF_RESIDUAL_PROPERTY )
	{
	//效验参数
	ASSERT(wDataSize==sizeof(CMD_GF_ResidualProperty));
	if (wDataSize!=sizeof(CMD_GF_ResidualProperty)) return false;

	//变量定义
	CMD_GF_ResidualProperty * ResidualProperty=(CMD_GF_ResidualProperty *)pData;

	//获取玩家
	tagUserData *pMeUserData = m_pMeUserItem->GetUserData();

	//设置道具
	for ( WORD wPropID = 0; wPropID < PROPERTY_COUNT; ++wPropID )
	{
	pMeUserData->dwPropResidualTime[wPropID] = ResidualProperty->dwResidual[wPropID].dwResidualTime;
	}
	}
	//道具消息
	else if ( Command.wSubCmdID == SUB_GF_PROP_ATTRIBUTE )
	{
	//校验参数
	ASSERT( wDataSize % sizeof(tagPropertyInfo) == 0 );
	if ( wDataSize % sizeof(tagPropertyInfo) != 0 ) return true;

	//类型转换
	int nInfoCount = wDataSize / sizeof(tagPropertyInfo);
	tagPropertyInfo *pPropertyInfo = (tagPropertyInfo*)pData;

	//保存属性
	for ( INT_PTR nIndex = 0; nIndex < nInfoCount; ++nIndex )
	m_PropertyAttributeArrary.Add(pPropertyInfo[nIndex]);

	/ *
	//获取对象
	CPropertyBar *pPropertyBar = CPropertyBar::GetInstance();
	ASSERT( pPropertyBar != NULL );
	if ( pPropertyBar == NULL ) return false;

	//初始结构
	for ( int nIndex = 0; nIndex < nInfoCount; ++nIndex )
	{
	pPropertyBar->SetPropertyInfo((pPropertyInfo+nIndex)->nPropertyID, *(pPropertyInfo+nIndex));
	}* /

	}
	//鲜花属性
	else if ( Command.wSubCmdID == SUB_GF_FLOWER_ATTRIBUTE )
	{
	//校验参数
	ASSERT( wDataSize % sizeof(tagFlowerInfo) == 0 );
	if ( wDataSize % sizeof(tagFlowerInfo) != 0 ) return true;

	//类型转换
	int nInfoCount = wDataSize / sizeof(tagFlowerInfo);
	tagFlowerInfo *pFlowerInfo= (tagFlowerInfo*)pData;

	//保存属性
	for ( INT_PTR nIndex = 0; nIndex < nInfoCount; ++nIndex )
	m_FlowerAttributeArrary.Add(pFlowerInfo[nIndex]);

	/ *
	//获取对象
	CPropertyBar *pPropertyBar = CPropertyBar::GetInstance();
	ASSERT( pPropertyBar != NULL );
	if ( pPropertyBar == NULL ) return false;

	//初始结构
	for ( int nIndex = 0; nIndex < nInfoCount; ++nIndex )
	{
	pPropertyBar->SetFlowerInfo((pFlowerInfo+nIndex)->nFlowerID, *(pFlowerInfo+nIndex));
	}* /


	}
	//喇叭消息
	else if ( Command.wSubCmdID == SUB_GF_PROP_BUGLE )
	{
	ASSERT( sizeof(CMD_GF_BugleProperty) == wDataSize );
	if ( sizeof(CMD_GF_BugleProperty) != wDataSize ) return false;

	//类型转换
	CMD_GF_BugleProperty *pBugleProperty = (CMD_GF_BugleProperty*)pData;

	//喇叭图片
	CBitmap Bitmap;
	AfxSetResourceHandle(GetResInstanceHandle());
	if ( Bitmap.LoadBitmap(g_GlobalUnits.m_ChatExpViewImage.uChatBugle) )
	{
	m_MessageProxyHelper->InsertImage(&Bitmap);
	Bitmap.DeleteObject();
	}
	AfxSetResourceHandle(GetModuleHandle(NULL));

	//喇叭内容
	m_MessageProxyHelper->InsertUserChat(pBugleProperty->szUserName,pBugleProperty->szContext,pBugleProperty->crText,MS_NORMAL);

	//播放声音
	HINSTANCE hInstance = AfxGetInstanceHandle();
	HRSRC hResour=FindResource(hInstance,TEXT("BUGLE"),TEXT("WAVE"));
	if (hResour!=NULL)
	{
	HGLOBAL hGlobal=LoadResource(hInstance,hResour);
	if (hGlobal!=NULL) 
	{
	//播放声音
	LPCTSTR pszMemory=(LPCTSTR)LockResource(hGlobal);
	BOOL bResult = ::PlaySound(pszMemory,hInstance,SND_ASYNC|SND_MEMORY|SND_NODEFAULT);

	//清理资源
	UnlockResource(hGlobal);
	}
	FreeResource(hGlobal);
	}
	}
	//赠送礼物
	else if( Command.wSubCmdID == SUB_GF_FLOWER )
	{
	//效验参数
	ASSERT( sizeof(CMD_GF_GiftNotify) == wDataSize );
	if ( sizeof(CMD_GF_GiftNotify) != wDataSize ) return false;

	//变量定义
	CMD_GF_GiftNotify * pGiftNotify=(CMD_GF_GiftNotify *)pData;

	//大厅房间
	if(pGiftNotify->cbSendLocation==LOCATION_PLAZA_ROOM)
	{
	//获取玩家
	IUserItem * pISendUserItem=m_ClientUserManager.SearchUserByUserID(pGiftNotify->dwSendUserID);
	IUserItem * pIRcvUserItem=m_ClientUserManager.SearchUserByUserID(pGiftNotify->dwRcvUserID);

	ASSERT( pISendUserItem != NULL && pIRcvUserItem != NULL );
	if ( pISendUserItem == NULL || pIRcvUserItem == NULL ) return true;

	tagUserData const *pSendUserData = pISendUserItem->GetUserData();
	tagUserData const *pRcvUserData = pIRcvUserItem->GetUserData();

	//结果判断
	ASSERT( pSendUserData != NULL && pRcvUserData != NULL );
	if ( pSendUserData == NULL || pRcvUserData == NULL ) return true;
	ASSERT( pGiftNotify->wGiftID < FLOWER_COUNT ) ;
	if ( pGiftNotify->wGiftID >= FLOWER_COUNT ) return true;

	//构造消息
	CString strGiftMsg;
	int nFlowerID = pGiftNotify->wGiftID;

	strGiftMsg.Format(TEXT("[%s]"),pSendUserData->szName);
	m_MessageProxyHelper->InsertShtickString(strGiftMsg,RGB(0,0,255), strGiftMsg.GetLength(), false);

	strGiftMsg = g_FlowerDescribe[nFlowerID].szAction;
	m_MessageProxyHelper->InsertShtickString(strGiftMsg,RGB(255,0,255), strGiftMsg.GetLength(), false);

	strGiftMsg.Format(TEXT("[%s]"),pRcvUserData->szName);
	m_MessageProxyHelper->InsertShtickString(strGiftMsg,RGB(0,0,255),strGiftMsg.GetLength(),false);

	strGiftMsg = g_FlowerDescribe[nFlowerID].szQuantifier1;
	m_MessageProxyHelper->InsertShtickString(strGiftMsg,RGB(255,0,255), strGiftMsg.GetLength(), false);

	//插入图片
	CBitmap Bitmap;
	AfxSetResourceHandle(GetResInstanceHandle());
	if ( Bitmap.LoadBitmap(g_GlobalUnits.m_ChatExpViewImage.uChatGift[nFlowerID]) )
	{
	m_MessageProxyHelper->InsertImage(&Bitmap);
	Bitmap.DeleteObject();
	}
	AfxSetResourceHandle(GetModuleHandle(NULL));

	WORD wFlowerCount = pGiftNotify->wFlowerCount;
	strGiftMsg.Format(TEXT("，共%d%s%s[ %s ]"), pGiftNotify->wFlowerCount, g_FlowerDescribe[nFlowerID].szQuantifier2,
	g_FlowerDescribe[nFlowerID].szResult, pRcvUserData->szName);
	m_MessageProxyHelper->InsertShtickString(strGiftMsg,RGB(255,0,255), strGiftMsg.GetLength(), false);	

	strGiftMsg.Format(g_FlowerDescribe[nFlowerID].szCharmResult, wFlowerCount * abs(CPropertyBar::m_FlowerInfo[nFlowerID].lRcvUserCharm));
	m_MessageProxyHelper->InsertShtickString(strGiftMsg,RGB(255,0,255), strGiftMsg.GetLength(), true);	
	}

	}
	*/

	return true;
}
/*

bool CRoomViewItem::OnSocketTask(CMD_Command Command, void * pBuffer, WORD wDataSize)
{
ASSERT(wDataSize<=SOCKET_PACKET);
if (wDataSize>SOCKET_PACKET)return false;

//构造数据
IPC_SocketPackage SocketPackage;
memset(&SocketPackage,0,sizeof(SocketPackage));
SocketPackage.Command=Command;
if (wDataSize>0)
{
ASSERT(pData!=NULL);
CopyMemory(SocketPackage.cbBuffer,pData,wDataSize);
}

switch (Command.wSubCmdID)
{
//任务接收
case SUB_GF_TASK_ACCEPT:
{
//效验数据
ASSERT(wDataSize%sizeof(CMD_GF_TASK_tagTask)==0);
if (wDataSize%sizeof(CMD_GF_TASK_tagTask)!=0)return false;
CMD_GF_TASK_tagTask * pTagTask = (CMD_GF_TASK_tagTask *)pBuffer;
CopyMemory(&m_tagTask, pTagTask, sizeof(m_tagTask));
//获取用户
tagUserData * pUserData=m_pMeUserItem->GetUserData();
//数据缓冲
if ((pUserData->wTableID!=INVALID_TABLE)&&(pUserData->wChairID!=INVALID_CHAIR))
{
//设置变量
m_wPacketTableID=pUserData->wTableID;
m_wPacketChairID=pUserData->wChairID;
//发送数据
WORD wSendSize=sizeof(SocketPackage.Command)+wDataSize;
SendProcessData(IPC_MAIN_SOCKET,IPC_SUB_SOCKET_RECV,&SocketPackage,wSendSize);
}
return true;
}
//任务进度
case SUB_GF_TASK_SAVEJD:
{
ASSERT(wDataSize%sizeof(CMD_GF_TASK_CountNums)==0);
if (wDataSize%sizeof(CMD_GF_TASK_CountNums)!=0)	return false;
return true;
}
//单个任务完成
case SUB_GF_TASK_ONCEEND:
{
ASSERT(wDataSize%sizeof(CMD_GF_TASK_tagTaskFinish)==0);
if (wDataSize%sizeof(CMD_GF_TASK_tagTaskFinish)!=0)	return false;


return true;
}
}

return true;
}*/

//用户进入
bool CRoomViewItem::OnSocketSubUserCome(CMD_Command Command, void * pData, WORD wDataSize)
{
	//效验参数
	ASSERT(Command.wMainCmdID==MDM_GR_USER);
	ASSERT(Command.wSubCmdID==SUB_GR_USER_COME);
	ASSERT(wDataSize>=sizeof(tagUserInfoHead));
	if (wDataSize<sizeof(tagUserInfoHead))
	{
		return false;
	}


	//读取基本信息
	tagUserData UserData;
	memset(&UserData,0,sizeof(UserData));
	tagUserInfoHead * pUserInfoHead=(tagUserInfoHead *)pData;

	//读取信息
	UserData.dwUserID=pUserInfoHead->dwUserID;
	UserData.wTableID=pUserInfoHead->wTableID;
	UserData.wChairID=pUserInfoHead->wChairID;
	UserData.cbUserStatus=pUserInfoHead->cbUserStatus;
	UserData.dwUserRight=pUserInfoHead->dwUserRight;
	UserData.dwMasterRight=pUserInfoHead->dwMasterRight;
	//UserData.lMoney = pUserInfoHead->lMoney;

	//管理判断
	if ((pUserInfoHead->dwUserID==g_GlobalUnits.GetGolbalUserData().dwUserID)&&(pUserInfoHead->cbMasterOrder>=2))
	{
		m_cbHideUserInfo=FALSE;
	}

	//读取信息
	if ((m_cbHideUserInfo==FALSE)||(pUserInfoHead->dwUserID==g_GlobalUnits.GetGolbalUserData().dwUserID))
	{
		UserData.wFaceID=pUserInfoHead->wFaceID;
		UserData.dwCustomFaceVer=pUserInfoHead->dwCustomFaceVer;
		UserData.cbGender=pUserInfoHead->cbGender;
		UserData.cbMemberOrder=pUserInfoHead->cbMemberOrder;
		UserData.cbMasterOrder=pUserInfoHead->cbMasterOrder;
		UserData.dwGameID=pUserInfoHead->dwGameID;
		UserData.dwGroupID=pUserInfoHead->dwGroupID;
		UserData.lLoveliness=pUserInfoHead->lLoveliness;
		//UserData.lScore=pUserInfoHead->UserScoreInfo.lScore;
		//UserData.lGameGold=pUserInfoHead->UserScoreInfo.lGameGold;
		UserData.lMoney=pUserInfoHead->UserScoreInfo.lMoney;
		UserData.lWinCount=pUserInfoHead->UserScoreInfo.lWinCount;
		UserData.lLostCount=pUserInfoHead->UserScoreInfo.lLostCount;
		UserData.lDrawCount=pUserInfoHead->UserScoreInfo.lDrawCount;
		UserData.lFleeCount=pUserInfoHead->UserScoreInfo.lFleeCount;
		UserData.lExperience=pUserInfoHead->UserScoreInfo.lExperience;

		UserData.dwGrade = pUserInfoHead->UserScoreInfo.dwGrade;
		//UserData.dwOrder = pUserInfoHead->UserScoreInfo.dwOrder;

		UserData.lGem = pUserInfoHead->UserScoreInfo.lGem;   //用户的宝石
		UserData.lGold = pUserInfoHead->UserScoreInfo.lGold;//金币
		//UserData.lHonor = pUserInfoHead->UserScoreInfo.lHonor;  //用户的荣誉
		//UserData.lCredit = pUserInfoHead->UserScoreInfo.lCredit; //用户的声望

		//用户游戏局数 Author<cxf>;DateTime<2012-03-05>
		UserData.lKindIDWinCount= pUserInfoHead->UserScoreInfo.lKindIDWinCount;
		UserData.lKindIDLostCount= pUserInfoHead->UserScoreInfo.lKindIDLostCount;
		UserData.lKindIDFleeCount= pUserInfoHead->UserScoreInfo.lKindIDFleeCount;
		UserData.lKindIDDrawCount= pUserInfoHead->UserScoreInfo.lKindIDDrawCount;

		//等级称谓 Author<cxf>;DateTime<2012-03-15>
		::memcpy(UserData.chGradeName, ((CPlazaDlg*)GetParent())->m_pGradeInfo[UserData.dwGrade-1].strName, sizeof(UserData.chGradeName));
		::memcpy(UserData.szAccounts, pUserInfoHead->szAccounts, sizeof(UserData.szAccounts));
		if (pUserInfoHead->dwUserID==g_GlobalUnits.GetGolbalUserData().dwUserID)
		{
			//更新全局本人用户信息	--osc update
			tagGlobalUserData& tagUserInfo = g_GlobalUnits.GetGolbalUserData();
			tagUserInfo.lGem = pUserInfoHead->UserScoreInfo.lGem;	//宝石
			tagUserInfo.lGold = pUserInfoHead->UserScoreInfo.lGold;	//金币
			tagUserInfo.lMoney=pUserInfoHead->UserScoreInfo.lMoney;//藏宝币
			tagUserInfo.dwExperience = pUserInfoHead->UserScoreInfo.lExperience;	//经验
			tagUserInfo.dwGrade = pUserInfoHead->UserScoreInfo.dwGrade;	//等级
			//tagUserInfo.lHonor = pUserInfoHead->UserScoreInfo.lHonor;	//荣誉
			tagUserInfo.lWinCount = pUserInfoHead->UserScoreInfo.lWinCount; //胜利盘数
			tagUserInfo.lLostCount = pUserInfoHead->UserScoreInfo.lLostCount;	//失败盘数
			tagUserInfo.lDrawCount = pUserInfoHead->UserScoreInfo.lDrawCount;	//和局盘数
			tagUserInfo.lFleeCount = pUserInfoHead->UserScoreInfo.lFleeCount;	//断线盘数
			GetParent()->PostMessage(WM_COMMAND, IDB_CALL_UPDATE_USERIMAGE, 0);
		}
	}

	/*cxf if (pUserInfoHead->dwUserID==g_GlobalUnits.GetGolbalUserData().dwUserID)
	{
	m_dwWinGemsToday = pUserInfoHead->UserScoreInfo.dwWinGemsToday;
	}*/

	//读取扩展信息
	void * pDataBuffer=NULL;
	tagDataDescribe DataDescribe;
	CRecvPacketHelper RecvPacket(pUserInfoHead+1,wDataSize-sizeof(tagUserInfoHead));
	while (true)
	{
		pDataBuffer=RecvPacket.GetData(DataDescribe);
		if (DataDescribe.wDataDescribe==DTP_NULL)
		{
			break;
		}
		switch (DataDescribe.wDataDescribe)
		{
		case DTP_USER_ACCOUNTS:		//用户昵称
			{
				if ((m_cbHideUserInfo==FALSE)||(pUserInfoHead->dwUserID==g_GlobalUnits.GetGolbalUserData().dwUserID))
				{
					ASSERT(pDataBuffer!=NULL);
					ASSERT(DataDescribe.wDataSize<=sizeof(UserData.szName));
					if (DataDescribe.wDataSize<=sizeof(UserData.szName))
					{
						CopyMemory(&UserData.szName,pDataBuffer,DataDescribe.wDataSize);
						UserData.szName[CountArray(UserData.szName)-1]=0;
					}
				}
				else
				{
					lstrcpyn(UserData.szName,TEXT("游戏用户"),CountArray(UserData.szName));
				}
				break;
			}
		case DTP_UNDER_WRITE:		//个性签名
			{
				if ((m_cbHideUserInfo==FALSE)||(pUserInfoHead->dwUserID==g_GlobalUnits.GetGolbalUserData().dwUserID))
				{
					ASSERT(pDataBuffer!=NULL);
					ASSERT(DataDescribe.wDataSize<=sizeof(UserData.szUnderWrite));
					if (DataDescribe.wDataSize<=sizeof(UserData.szUnderWrite))
					{
						CopyMemory(&UserData.szUnderWrite,pDataBuffer,DataDescribe.wDataSize);
						UserData.szUnderWrite[CountArray(UserData.szUnderWrite)-1]=0;
					}
				}
				break;
			}
		case DTP_USER_GROUP_NAME:	//用户社团
			{
				if ((m_cbHideUserInfo==FALSE)||(pUserInfoHead->dwUserID==g_GlobalUnits.GetGolbalUserData().dwUserID))
				{
					ASSERT(pDataBuffer!=NULL);
					ASSERT(DataDescribe.wDataSize<=sizeof(UserData.szGroupName));
					if (DataDescribe.wDataSize<=sizeof(UserData.szGroupName))
					{
						CopyMemory(&UserData.szGroupName,pDataBuffer,DataDescribe.wDataSize);
						UserData.szGroupName[CountArray(UserData.szGroupName)-1]=0;
					}
				}
				break;
			}
		}
	}

	//查找用户
	IUserItem * pIUserItem=m_ClientUserManager.SearchUserByUserID(UserData.dwUserID);
	if (pIUserItem==NULL) 
	{
		const tagCompanionItem * pCompanionItem=NULL;
		pCompanionItem=g_GlobalUnits.m_CompanionManager->SearchCompanionItem(UserData.dwUserID);
		if (pCompanionItem!=NULL) 
		{
			UserData.cbCompanion=pCompanionItem->Companion;
		}
		pIUserItem=m_ClientUserManager.ActiveUserItem(UserData);
	}
	else
	{
		OnUserItemUpdate(pIUserItem);
	}


	//加入用户信息
	ASSERT(pIUserItem!=NULL);
	if (pIUserItem!=NULL)
	{
		//判断自己
		if (m_pMeUserItem==NULL)
		{
			m_pMeUserItem=pIUserItem;
		}

		//设置界面
		BYTE cbUserStatus = UserData.cbUserStatus;
		if (  ( cbUserStatus >= US_SIT ) 
			&& ( cbUserStatus != US_LOOKON ) )
		{
			m_TableFrame.SetUserInfo(UserData.wTableID,UserData.wChairID,pIUserItem);
		}

		//提示信息
		if ((m_cbHideUserInfo==FALSE)&&(m_ServiceStatus==ServiceStatus_Serviceing))
		{
			if ((UserData.cbCompanion==enCompanion_Friend)||(UserData.dwMasterRight!=0L))
			{
				TCHAR szMessage[256]=TEXT("");
				_sntprintf(szMessage,sizeof(szMessage),TEXT("[ %s ] 进来了"),UserData.szName);
				m_MessageProxyHelper->InsertSystemString(szMessage,0);
			}
			else if (g_GlobalOption.m_bShowInOutMessage==true)
			{
				TCHAR szMessage[256]=TEXT("");
				_sntprintf(szMessage,sizeof(szMessage),TEXT("[ %s ] 进来了"),UserData.szName);
				m_MessageProxyHelper->InsertGeneralString(szMessage,0,0,true);
			}
		}
	}

	//自定头像
	if(UserData.dwCustomFaceVer!=0)
	{
		//头像名称
		CString strDirName = CString(g_GlobalUnits.GetWorkDirectory()) + TEXT("\\CustomFace");
		CString strFileName;
		strFileName.Format(TEXT("\\%ld_%d.bmp"), pUserInfoHead->dwUserID, pUserInfoHead->dwCustomFaceVer);

		//读取文件
		CImage FaceImage;
		HRESULT hResult = FaceImage.Load(strDirName + strFileName);
		if (SUCCEEDED(hResult))
		{
			FaceImage.Destroy();
		}
		//下载头像
		else
		{
			////投递消息
			//CPlazaViewItem *pPlazaViewItem = ((CGameFrame *)AfxGetMainWnd())->GetPlazaViewItem();
			//pPlazaViewItem->PostMessage(WM_DOWN_LOAD_FACE, LPARAM(pUserInfoHead->dwCustomFaceVer), WPARAM(pUserInfoHead->dwUserID));
		}
	}

	//更新标题
	UpdateTitleText();

	//更新人数
	DWORD dwOnLineCount=m_ClientUserManager.GetOnLineCount();
	g_GlobalUnits.m_ServerListManager.UpdateGameServerOnLine(m_pListServer,dwOnLineCount);

	return true;
}

//用户状态
bool CRoomViewItem::OnSocketSubStatus(CMD_Command Command, void * pData, WORD wDataSize)
{
	//效验参数
	ASSERT(Command.wMainCmdID==MDM_GR_USER);
	ASSERT(Command.wSubCmdID==SUB_GR_USER_STATUS);
	ASSERT(wDataSize>=sizeof(CMD_GR_UserStatus));
	if (wDataSize<sizeof(CMD_GR_UserStatus))
	{
		return false;
	}

	//处理数据
	CMD_GR_UserStatus * pUserStatus=(CMD_GR_UserStatus *)pData;
	IUserItem * pIUserItem=m_ClientUserManager.SearchUserByUserID(pUserStatus->dwUserID);
	ASSERT(pIUserItem!=NULL);
	if (pIUserItem==NULL)
	{
		return true;
	}


	//定义变量
	tagUserData * pUserData=pIUserItem->GetUserData();
	tagUserData * pMeUserData=m_pMeUserItem->GetUserData();
	WORD wNowTableID=pUserStatus->wTableID,wLastTableID=pUserData->wTableID;
	WORD wNowChairID=pUserStatus->wChairID,wLastChairID=pUserData->wChairID;
	BYTE cbNowStatus=pUserStatus->cbUserStatus,cbLastStatus=pUserData->cbUserStatus;

	//清理状态
	if (pUserData==pMeUserData)
	{
		m_wReqTableID=INVALID_TABLE;
		m_wReqChairID=INVALID_CHAIR;
	}

	//清理旧状态
	if ((wLastTableID!=INVALID_TABLE)&&((wNowTableID!=wLastTableID)||(wNowChairID!=wLastChairID)))
	{
		ASSERT(wLastChairID!=INVALID_CHAIR);
		IUserItem * pITableUserItem=m_TableFrame.GetUserInfo(wLastTableID,wLastChairID);
		if (pITableUserItem==pIUserItem) m_TableFrame.SetUserInfo(wLastTableID,wLastChairID,NULL);
	}

	//用户离开
	if (cbNowStatus==US_NULL)
	{
		//通知游戏
		if ((pMeUserData->wTableID!=INVALID_TABLE)&&(pMeUserData->wTableID==wLastTableID))
		{
			IPC_UserStatus UserStatus;
			memset(&UserStatus,0,sizeof(UserStatus));
			UserStatus.dwUserID=pUserData->dwUserID;
			UserStatus.cbUserStatus=pUserData->cbUserStatus;
			SendProcessData(IPC_MAIN_USER,IPC_SUB_USER_STATUS,&UserStatus,sizeof(UserStatus));
		}

		//删除用户
		m_ClientUserManager.DeleteUserItem(pIUserItem);

		//更新标题
		UpdateTitleText();

		//更新人数
		DWORD dwOnLineCount=m_ClientUserManager.GetOnLineCount();
		g_GlobalUnits.m_ServerListManager.UpdateGameServerOnLine(m_pListServer,dwOnLineCount);

		return true;
	}

	//更新状态
	tagUserStatus UserStatus;
	UserStatus.wTableID=wNowTableID;
	UserStatus.wChairID=wNowChairID;
	UserStatus.cbUserStatus=cbNowStatus;
	m_ClientUserManager.UpdateUserItemStatus(pIUserItem,&UserStatus);

	//设置新状态
	if ((wNowTableID!=INVALID_TABLE)&&((wNowTableID!=wLastTableID)||(wNowChairID!=wLastChairID)))
	{
		//设置桌子
		if (cbNowStatus!=US_LOOKON)
		{
			ASSERT(wNowChairID!=INVALID_CHAIR);
			m_TableFrame.SetUserInfo(wNowTableID,wNowChairID,pIUserItem);
		}

		//发送用户
		if ((m_pMeUserItem!=pIUserItem)&&(pMeUserData->wTableID==wNowTableID))
		{
			CIPCSendCopyData SendCopyData(m_hWndChannel,m_hWnd);
			SendTableUser(pIUserItem,&SendCopyData);
		}
	}

	//更新界面
	if ((wNowTableID!=INVALID_TABLE)&&(cbNowStatus!=US_LOOKON)
		&&(wNowTableID==wLastTableID)&&(wNowChairID==wLastChairID))
	{
		ASSERT(wNowChairID!=INVALID_CHAIR);
		m_TableFrame.UpdateTableView(wNowTableID,false);
	}

	//判断发送
	bool bNotifyGame=false;
	if (pIUserItem==m_pMeUserItem)
	{
		bNotifyGame=true;
	}
	else if ((pMeUserData->wTableID!=INVALID_TABLE)&&(pMeUserData->wTableID==wNowTableID)) 
	{
		bNotifyGame=true;
	}
	else if ((pMeUserData->wTableID!=INVALID_TABLE)&&(pMeUserData->wTableID==wLastTableID)) 
	{
		bNotifyGame=true;
	}


	//发送状态
	if (bNotifyGame==true)
	{
		IPC_UserStatus UserStatus;
		memset(&UserStatus,0,sizeof(UserStatus));
		UserStatus.dwUserID=pUserData->dwUserID;
		UserStatus.cbUserStatus=pUserData->cbUserStatus;
		SendProcessData(IPC_MAIN_USER,IPC_SUB_USER_STATUS,&UserStatus,sizeof(UserStatus));
	}

	//切换位置判断
	if (cbLastStatus==US_FREE 
		&& cbNowStatus==US_PLAY 
		&& pMeUserData->cbUserStatus==US_LOOKON 
		&& wNowTableID==pMeUserData->wTableID 
		&& wNowChairID==pMeUserData->wChairID) 
	{
		IPC_UserStatus UserStatus;
		memset(&UserStatus,0,sizeof(UserStatus));
		UserStatus.dwUserID=pMeUserData->dwUserID;
		UserStatus.cbUserStatus=US_FREE;
		SendProcessData(IPC_MAIN_USER,IPC_SUB_USER_STATUS,&UserStatus,sizeof(UserStatus));
		StartGameClient();
	}

	//判断自己
	if (pIUserItem==m_pMeUserItem)
	{
		//设置变量
		if ((wNowTableID==m_wReqTableID)&&(wNowChairID==m_wReqChairID))
		{
			m_wReqTableID=INVALID_TABLE;
			m_wReqChairID=INVALID_CHAIR;
		}

		//百家乐判断
		if (m_TableFrame.GetChairCount()>MAX_CHAIR_NORMAL && US_SIT==cbNowStatus)
		{
			m_wReqTableID=INVALID_TABLE;
			m_wReqChairID=INVALID_CHAIR;
		}

		//缓冲清理
		if ((m_wPacketTableID!=INVALID_TABLE)&&((m_wPacketTableID!=wNowTableID)||(m_wPacketChairID!=wNowChairID)))
		{
			m_wPacketTableID=INVALID_TABLE;
			m_wPacketChairID=INVALID_CHAIR;
			m_PacketDataStorage.RemoveData(false);
		}

		//启动游戏
		if ((wNowTableID!=INVALID_TABLE)&&((wNowTableID!=wLastTableID)||(wNowChairID!=wLastChairID)))
		{
			int iResult=StartGameClient();
		}
	}

	return true;
}

//用户分数
bool CRoomViewItem::OnSocketSubScore(CMD_Command Command, void * pData, WORD wDataSize)
{
	//效验参数
	ASSERT(Command.wMainCmdID==MDM_GR_USER);
	ASSERT(Command.wSubCmdID==SUB_GR_USER_SCORE);
	ASSERT(wDataSize>=sizeof(CMD_GR_UserScore));
	if (wDataSize<sizeof(CMD_GR_UserScore))
	{
		return false;
	}


	//处理数据
	CMD_GR_UserScore * pUserScore=(CMD_GR_UserScore *)pData;
	IUserItem * pIUserItem=m_ClientUserManager.SearchUserByUserID(pUserScore->dwUserID);
	ASSERT(pIUserItem!=NULL);
	if (pIUserItem==NULL) 
	{
		return true;
	}


	//更新判断
	if ((m_cbHideUserInfo==FALSE)||(pIUserItem==m_pMeUserItem))
	{
		//更新分数
		tagUserData * pUserData=pIUserItem->GetUserData();
		pUserData->lLoveliness = pUserScore->lLoveliness;
		pUserData->lGold = pUserScore->UserScore.lGold;
		pUserData->lMoney = pUserScore->UserScore.lMoney;
		pUserData->dwGrade = pUserScore->UserScore.dwGrade;
		//pUserData->dwOrder = pUserScore->UserScore.dwOrder;
		pUserData->lGem = pUserScore->UserScore.lGem;
		//pUserData->lHonor = pUserScore->UserScore.lHonor;
		//pUserData->lCredit = pUserScore->UserScore.lCredit;

		//用户游戏局数 Author<cxf>;DateTime<2012-03-05>
		pUserData->lKindIDWinCount=pUserScore->UserScore.lKindIDWinCount;
		pUserData->lKindIDLostCount=pUserScore->UserScore.lKindIDLostCount;
		pUserData->lKindIDFleeCount=pUserScore->UserScore.lKindIDFleeCount;
		pUserData->lKindIDDrawCount=pUserScore->UserScore.lKindIDDrawCount;

		if (pUserScore->dwUserID==g_GlobalUnits.GetGolbalUserData().dwUserID)
		{
			//m_dwWinGemsToday = pUserScore->UserScore.dwWinGemsToday;

			//更新全局本人用户信息	--osc update
			tagGlobalUserData& tagUserInfo = g_GlobalUnits.GetGolbalUserData();
			tagUserInfo.lGem = pUserScore->UserScore.lGem;	//宝石
			tagUserInfo.lGold = pUserScore->UserScore.lGold;	//金币
			tagUserInfo.lMoney=pUserScore->UserScore.lMoney; //藏宝币
			tagUserInfo.dwExperience = pUserScore->UserScore.lExperience;	//经验
			tagUserInfo.dwGrade = pUserScore->UserScore.dwGrade;	//等级
			//tagUserInfo.lHonor = pUserScore->UserScore.lHonor;	//荣誉
			tagUserInfo.lWinCount = pUserScore->UserScore.lWinCount; //胜利盘数
			tagUserInfo.lLostCount = pUserScore->UserScore.lLostCount;	//失败盘数
			tagUserInfo.lDrawCount = pUserScore->UserScore.lDrawCount;	//和局盘数
			tagUserInfo.lFleeCount = pUserScore->UserScore.lFleeCount;	//断线盘数
			GetParent()->PostMessage(WM_COMMAND, IDB_CALL_UPDATE_USERIMAGE, 0);
		}

		m_ClientUserManager.UpdateUserItemScore(pIUserItem,&pUserScore->UserScore);

		//更新游戏
		tagUserData * pMeUserData=m_pMeUserItem->GetUserData();
		if ((pMeUserData->wTableID!=INVALID_TABLE)&&(pMeUserData->wTableID==pUserData->wTableID))
		{
			IPC_UserScore UserScore;
			memset(&UserScore,0,sizeof(UserScore));
			UserScore.dwUserID=pUserScore->dwUserID;
			UserScore.UserScore=pUserScore->UserScore;
			UserScore.lLoveliness = pUserScore->lLoveliness;
			//用户游戏局数 Author<cxf>;DateTime<2012-03-05>
			UserScore.UserScore.lKindIDWinCount=pUserScore->UserScore.lKindIDWinCount;
			UserScore.UserScore.lKindIDLostCount=pUserScore->UserScore.lKindIDLostCount;
			UserScore.UserScore.lKindIDFleeCount=pUserScore->UserScore.lKindIDFleeCount;
			UserScore.UserScore.lKindIDDrawCount=pUserScore->UserScore.lKindIDDrawCount;

			//等级称谓
			::memcpy(UserScore.UserScore.chGradeName,((CPlazaDlg*)GetParent())->m_pGradeInfo[pUserScore->UserScore.dwGrade-1].strName, sizeof(UserScore.UserScore.chGradeName));

			SendProcessData(IPC_MAIN_USER,IPC_SUB_USER_SCORE,&UserScore,sizeof(UserScore));
		}
	}

	return true;
}

//用户权限
bool CRoomViewItem::OnSocketSubRight(CMD_Command Command, void * pData, WORD wDataSize)
{
	//效验参数
	ASSERT(Command.wMainCmdID==MDM_GR_USER);
	ASSERT(Command.wSubCmdID==SUB_GR_USER_RIGHT);
	ASSERT(wDataSize==sizeof(CMD_GR_UserRight));
	if (wDataSize!=sizeof(CMD_GR_UserRight))
	{
		return false;
	}


	//消息处理
	CMD_GR_UserRight * pUserRight=(CMD_GR_UserRight *)pData;

	//用户权限
	IUserItem * pIUserItem=m_ClientUserManager.SearchUserByUserID(pUserRight->dwUserID);
	pIUserItem->GetUserData()->dwUserRight = pUserRight->dwUserRight;

	return true;
}

//会员等级
bool CRoomViewItem::OnSocketSubMemberOrder(CMD_Command Command, void * pData, WORD wDataSize)
{
	//效验参数
	ASSERT(Command.wMainCmdID==MDM_GR_USER);
	ASSERT(Command.wSubCmdID==SUB_GR_MEMBER_ORDER);
	ASSERT(wDataSize==sizeof(CMD_GR_MemberOrder));
	if (wDataSize!=sizeof(CMD_GR_MemberOrder))
	{
		return false;
	}


	//消息处理
	CMD_GR_MemberOrder * pMemberOrder=(CMD_GR_MemberOrder *)pData;

	//会员等级
	IUserItem * pIUserItem=m_ClientUserManager.SearchUserByUserID(pMemberOrder->dwUserID);
	tagUserData * pUserData=pIUserItem->GetUserData();
	pUserData->cbMemberOrder = pMemberOrder->cbMemberOrder;
	tagGlobalUserData & GlobalUserInfo=g_GlobalUnits.GetGolbalUserData();
	GlobalUserInfo.cbMember = pMemberOrder->cbMemberOrder;
	GetParent()->PostMessage(WM_COMMAND, IDB_CALL_UPDATE_USERIMAGE, 0);

	//更新游戏
	tagUserData * pMeUserData=m_pMeUserItem->GetUserData();
	if ((pMeUserData->wTableID!=INVALID_TABLE)&&(pMeUserData->wTableID==pUserData->wTableID))
	{
		IPC_MemberOrder MemberOrder;
		memset(&MemberOrder,0,sizeof(MemberOrder));
		MemberOrder.dwUserID=pUserData->dwUserID;
		MemberOrder.cbMember=pUserData->cbMemberOrder;
		SendProcessData(IPC_MAIN_USER,IPC_SUB_MEMBERORDER,&MemberOrder,sizeof(MemberOrder));
	}

	return true;
}

//坐下失败
bool CRoomViewItem::OnSocketSubSitFailed(CMD_Command Command, void * pData, WORD wDataSize)
{
	//效验参数
	ASSERT(Command.wMainCmdID==MDM_GR_USER);
	ASSERT(Command.wSubCmdID==SUB_GR_SIT_FAILED);

	//消息处理
	CMD_GR_SitFailed * pSitFailed=(CMD_GR_SitFailed *)pData;
	IUserItem * pISendUserItem=m_TableFrame.GetUserInfo(m_wReqTableID,m_wReqChairID);
	if (pISendUserItem==m_pMeUserItem) 
	{
		m_TableFrame.SetUserInfo(m_wReqTableID,m_wReqChairID,NULL);
	}


	//设置变量
	m_wReqTableID=INVALID_TABLE;
	m_wReqChairID=INVALID_CHAIR;

	//显示消息
	ShowMessageBox(pSitFailed->szFailedDescribe,MB_ICONINFORMATION);

	//GetParent()->ShowWindow(SW_SHOW);
	//GetParent()->PostMessage(WM_COMMAND,IDM_CALL_GAME_LIST,0);
	//CloseRoomViewItem();

	return true;
}

//用户聊天
bool CRoomViewItem::OnSocketSubChat(CMD_Command Command, void * pData, WORD wDataSize)
{
	//效验参数
	ASSERT(Command.wMainCmdID==MDM_GR_USER);
	ASSERT(Command.wSubCmdID==SUB_GR_USER_CHAT);

	//效验参数
	CMD_GR_UserChat * pUserChat=(CMD_GR_UserChat *)pData;
	ASSERT(wDataSize>=(sizeof(CMD_GR_UserChat)-sizeof(pUserChat->szChatMessage)));
	ASSERT(wDataSize==(sizeof(CMD_GR_UserChat)-sizeof(pUserChat->szChatMessage)+pUserChat->wChatLength));
	if (wDataSize<(sizeof(CMD_GR_UserChat)-sizeof(pUserChat->szChatMessage))) 
	{
		return false;
	}
	if (wDataSize!=(sizeof(CMD_GR_UserChat)-sizeof(pUserChat->szChatMessage)+pUserChat->wChatLength)) 
	{
		return false;
	}

	//寻找用户
	IUserItem * pISendUserItem=m_ClientUserManager.SearchUserByUserID(pUserChat->dwSendUserID);
	if (pISendUserItem==NULL)
	{
		return true;
	}
	tagUserData * pSendUserData=pISendUserItem->GetUserData();

	//消息过滤
	if ((pISendUserItem!=m_pMeUserItem)&&(pSendUserData->cbCompanion==enCompanion_Detest)) 
	{
		return true;
	}


	//显示消息
	if (pUserChat->dwTargetUserID!=0L)
	{
		IUserItem * pIRecvUserItem=m_ClientUserManager.SearchUserByUserID(pUserChat->dwTargetUserID);
		if (pIRecvUserItem==NULL) 
		{
			return true;
		}
		tagUserData * pRecvUserData=pIRecvUserItem->GetUserData();
		m_MessageProxyHelper->InsertUserChat(pSendUserData->szName,pRecvUserData->szName,pUserChat->szChatMessage,pUserChat->crFontColor,MS_NORMAL);
	}
	else
	{
		m_MessageProxyHelper->InsertUserChat(pSendUserData->szName,pUserChat->szChatMessage,pUserChat->crFontColor,MS_NORMAL);
	}

	return true;
}

//用户私语
bool CRoomViewItem::OnSocketSubWisper(CMD_Command Command, void * pData, WORD wDataSize)
{
	//效验参数
	ASSERT(Command.wMainCmdID==MDM_GR_USER);
	ASSERT(Command.wSubCmdID==SUB_GR_USER_WISPER);

	//效验参数
	CMD_GR_Wisper * pWisper=(CMD_GR_Wisper *)pData;
	ASSERT(wDataSize>=(sizeof(CMD_GR_Wisper)-sizeof(pWisper->szChatMessage)));
	ASSERT(wDataSize==(sizeof(CMD_GR_Wisper)-sizeof(pWisper->szChatMessage)+pWisper->wChatLength));
	if (wDataSize<(sizeof(CMD_GR_Wisper)-sizeof(pWisper->szChatMessage)))
	{
		return false;
	}
	if (wDataSize!=(sizeof(CMD_GR_Wisper)-sizeof(pWisper->szChatMessage)+pWisper->wChatLength)) 
	{
		return false;
	}


	//寻找用户
	IUserItem * pISendUserItem=m_ClientUserManager.SearchUserByUserID(pWisper->dwSendUserID);
	IUserItem * pIRecvUserItem=m_ClientUserManager.SearchUserByUserID(pWisper->dwTargetUserID);
	if ((pISendUserItem==NULL)||(pIRecvUserItem==NULL))
	{
		return true;
	}

	tagUserData * pUserDataSend=pISendUserItem->GetUserData();
	tagUserData * pUserDataRecv=pIRecvUserItem->GetUserData();

	//消息过滤
	if ((pUserDataSend->cbMasterOrder==0) 
		&& (pISendUserItem!=m_pMeUserItem)
		&&(pUserDataSend->cbCompanion==enCompanion_Detest))
	{
		return true;
	}


	//显示信息
	CShortMessage * pShortMessageWnd=NULL;
	if (pWisper->dwSendUserID==m_pMeUserItem->GetUserID())
	{
		//自己发送的消息
		pShortMessageWnd=ActiveShortWnd(pWisper->dwTargetUserID,pIRecvUserItem,true);
		if (pShortMessageWnd!=NULL) 
		{
			pShortMessageWnd->OnRecvMessage(pUserDataSend->szName,pWisper->szChatMessage,pWisper->crFontColor,true);
		}
	}
	else	//其他游戏者发送信息
	{
		pShortMessageWnd=ActiveShortWnd(pWisper->dwSendUserID,pISendUserItem,true);
		if (pShortMessageWnd!=NULL)
		{
			pShortMessageWnd->OnRecvMessage(pUserDataSend->szName,pWisper->szChatMessage,pWisper->crFontColor,false);
		}
	}

	return true;
}

//用户邀请
bool CRoomViewItem::OnSocketSubUserInvite(CMD_Command Command, void * pData, WORD wDataSize)
{
	//效验参数
	ASSERT(Command.wMainCmdID==MDM_GR_USER);
	ASSERT(Command.wSubCmdID==SUB_GR_USER_INVITE);

	//效验参数
	ASSERT(wDataSize==sizeof(CMD_GR_UserInvite));
	if (wDataSize!=sizeof(CMD_GR_UserInvite)) 
	{
		return false;
	}


	//消息处理
	CMD_GR_UserInvite * pUserInvite=(CMD_GR_UserInvite *)pData;
	ASSERT(pUserInvite->wTableID<m_TableFrame.GetTableCount());
	if (m_TableFrame.QueryPlayFlag(pUserInvite->wTableID)==true) 
	{
		return true;
	}
	//寻找用户
	IUserItem * pIUserItem=m_ClientUserManager.SearchUserByUserID(pUserInvite->dwUserID);
	if (pIUserItem==NULL) 
	{
		return true;
	}

	tagUserData * pUserData=pIUserItem->GetUserData();
	if (pUserData->wTableID==INVALID_TABLE) 
	{
		return true;
	}


	//用户过虑
	if (pUserData->cbCompanion==enCompanion_Detest)
	{
		return true;
	}
	if (g_GlobalOption.m_InviteMode==enInviteMode_None) 
	{
		return true;
	}
	if ((g_GlobalOption.m_InviteMode==enInviteMode_Friend)
		&&(pUserData->cbCompanion!=enCompanion_Friend))
	{
		return true;
	}


	//提示消息
	TCHAR szMessage[256]=TEXT("");
	_sntprintf(szMessage,sizeof(szMessage),TEXT("[ %s ] 邀请您加入第 [ %ld ] 游戏桌进行游戏，是否同意？"),pUserData->szName,pUserInvite->wTableID+1);
	if (ShowMessageBox(szMessage,MB_ICONINFORMATION|MB_YESNO)==IDYES)
	{
		WORD wChairID=INVALID_CHAIR;
		WORD wNullCount=m_TableFrame.GetNullChairCount(pUserInvite->wTableID,wChairID);
		if (wNullCount==0) 
		{
			ShowMessageBox(TEXT("此游戏桌已经没有空位置了！"),MB_ICONINFORMATION);
			return true;
		}
		PerformSitDownAction(pUserInvite->wTableID,wChairID);
	}

	return true;
}
//更新排队人数
bool CRoomViewItem::OnSocketSubQueueCount(CMD_Command Command, void * pBuffer, WORD wDataSize)
{
	//效验参数
	ASSERT(Command.wMainCmdID==MDM_GR_USER);
	ASSERT(Command.wSubCmdID==SUB_GR_QUEUE_COUNT);

	//效验参数
	ASSERT(wDataSize==sizeof(CMD_GR_AutoQueue));
	if (wDataSize!=sizeof(CMD_GR_AutoQueue)) 
	{
		return false;
	}
	CMD_GR_AutoQueue * pUserInvite=(CMD_GR_AutoQueue *)pBuffer;
	m_QueueNum = pUserInvite->iAutoQueueNum;
	InvalidateRect(m_rcQueue, FALSE);
	return true;
}
//自动排队失败 SUB_GR_USER_QUEUE_FAIL:
bool CRoomViewItem::OnSocketSubQueueFail(CMD_Command Command, void * pBuffer, WORD wDataSize)
{
	//效验参数
	ASSERT(Command.wMainCmdID==MDM_GR_USER);
	//ASSERT(Command.wSubCmdID==SUB_GR_USER_QUEUE_FAIL);

	//效验参数
	//ASSERT(wDataSize==sizeof(CMD_GR_SendWarning));
	//if (wDataSize!=sizeof(CMD_GR_SendWarning)) 
	//{
	//	return false;
	//}

	CMD_GR_SendWarning * pUserInvite=(CMD_GR_SendWarning *)pBuffer;
	if (g_GlobalUnits.GetGolbalUserData().dwUserID == pUserInvite->dwTargetUserID)
	{
		CString str = pUserInvite->szWarningMessage;
		ShowMessageBox(str, MB_OK);

		GetParent()->ShowWindow(SW_SHOW);
		GetParent()->PostMessage(WM_COMMAND,IDM_CALL_GAME_LIST,0);
		CloseRoomViewItem();
	}
	return true;
}

//IPC 内核命令
bool CRoomViewItem::OnIPCKernel(const IPC_Head * pHead, const void * pIPCBuffer, WORD wDataSize, HWND hWndSend)
{
	ASSERT(pHead->wMainCmdID==IPC_MIAN_IPC_KERNEL);
	switch (pHead->wSubCmdID)
	{
	case IPC_SUB_IPC_CLIENT_CONNECT:	//连接消息
		{
			//判断连接
			if (::IsChild(m_pShareMemory->hWndGameFrame,hWndSend))
			{
				return false;
			}


			//设置变量
			ASSERT(m_hWndChannel==NULL);
			m_hWndChannel=hWndSend;

			//发送信息
			CIPCSendCopyData SendCopyData(m_hWndChannel,m_hWnd);
			IPCSendGameInfo(&SendCopyData);
			IPCSendTableUsers(&SendCopyData);
			SendCopyData.SendData(IPC_MAIN_CONCTROL,IPC_SUB_START_FINISH);

			//缓冲数据
			if ((m_wPacketTableID!=INVALID_TABLE)
				&&(m_wPacketChairID!=INVALID_CHAIR))
			{
				//变量定义
				tagDataHead DataHead;
				ZeroMemory(&DataHead,sizeof(DataHead));

				//变量定义
				BYTE cbBuffer[SOCKET_BUFFER];
				IPC_SocketPackage SocketPackage;
				ZeroMemory(cbBuffer,sizeof(cbBuffer));
				ZeroMemory(&SocketPackage,sizeof(SocketPackage));

				//提取数据
				do
				{
					//获取负荷
					tagBurthenInfo BurthenInfo;
					m_PacketDataStorage.GetBurthenInfo(BurthenInfo);

					//提取数据
					if (BurthenInfo.dwDataPacketCount==0L)
					{
						break;
					}
					if (m_PacketDataStorage.DistillData(DataHead,cbBuffer,sizeof(cbBuffer))==false)
					{
						break;
					}

					//构造数据
					CopyMemory(&SocketPackage.Command,cbBuffer,sizeof(CMD_Command));
					CopyMemory(SocketPackage.cbBuffer,&cbBuffer[sizeof(CMD_Command)],DataHead.wDataSize-sizeof(CMD_Command));

					//数据处理
					SendProcessData(IPC_MAIN_SOCKET,IPC_SUB_SOCKET_RECV,&SocketPackage,DataHead.wDataSize);

				} while (true);

				//设置变量
				m_wPacketTableID=INVALID_TABLE;
				m_wPacketChairID=INVALID_CHAIR;
			}

			//开始检测游戏框架是否存在
			m_tagGameFrameStatus.bIsOpen = true;
			m_tagGameFrameStatus.byStatus = 1;
			SendCopyData.SendData(IPC_MAIN_CONCTROL,IPC_SUB_GAMEFRAME_EXIST);
			SetTimer(IDI_GAME_FRAME_IS_EXIST, TIMER_GAMEFRAME_EXIST, NULL);
			CPlazaDlg * plazaDlg = ((CPlazaDlg*)GetParent());
			plazaDlg->UpdateMusicSound(false);
			SendToGameFrameAfficheInfo();
			/*
			//发送等级列表
			WORD wGradeSize=plazaDlg->m_iGradeInfoDiv;
			CMD_GP_UserGradeInfo UserGradeInfo[128]={0};
			::memcpy(UserGradeInfo,plazaDlg->m_pGradeInfo,sizeof(CMD_GP_UserGradeInfo)*wGradeSize);
			SendCopyData.SendData(IPC_MAIN_USER,IPC_SUB_GAME_GRADE_LIST,UserGradeInfo,sizeof(CMD_GP_UserGradeInfo)*128);
			*/
			/*			//属性数据
			IPC_SocketPackage SocketPackage;
			ZeroMemory(&SocketPackage,sizeof(SocketPackage));
			WORD wSendSize=0;


			//设置消息
			SocketPackage.Command.wMainCmdID = MDM_GF_PRESENT;
			SocketPackage.Command.wSubCmdID = SUB_GF_PROP_ATTRIBUTE;

			//道具属性
			for (INT_PTR nIndex =0; nIndex < m_PropertyAttributeArrary.GetCount(); ++nIndex)
			{
			if ((wSendSize+sizeof(tagPropertyInfo))>SOCKET_PACKET)
			{
			SendProcessData(IPC_MAIN_SOCKET,IPC_SUB_SOCKET_RECV,&SocketPackage,wSendSize+sizeof(CMD_Command));
			wSendSize=0;
			}
			tagPropertyInfo &PropertyInfo = m_PropertyAttributeArrary[nIndex];
			CopyMemory(SocketPackage.cbBuffer+wSendSize,&PropertyInfo,sizeof(PropertyInfo));
			wSendSize+=sizeof(PropertyInfo);
			}

			if (wSendSize>0) 
			{
			SendProcessData(IPC_MAIN_SOCKET,IPC_SUB_SOCKET_RECV,&SocketPackage,wSendSize+sizeof(CMD_Command));
			}


			//设置消息
			SocketPackage.Command.wSubCmdID = SUB_GF_FLOWER_ATTRIBUTE;

			//鲜花属性
			wSendSize=0L;
			for (INT_PTR nIndex =0; nIndex < m_FlowerAttributeArrary.GetCount(); ++nIndex)
			{
			if ((wSendSize+sizeof(tagFlowerInfo))>SOCKET_PACKET)
			{
			SendProcessData(IPC_MAIN_SOCKET,IPC_SUB_SOCKET_RECV,&SocketPackage,wSendSize+sizeof(CMD_Command));
			wSendSize=0;
			}
			tagFlowerInfo &FlowerInfo = m_FlowerAttributeArrary[nIndex];
			CopyMemory(SocketPackage.cbBuffer+wSendSize,&FlowerInfo,sizeof(FlowerInfo));
			wSendSize+=sizeof(FlowerInfo);
			}
			if (wSendSize>0)
			{
			SendProcessData(IPC_MAIN_SOCKET,IPC_SUB_SOCKET_RECV,&SocketPackage,wSendSize+sizeof(CMD_Command));
			}
			*/


			return true;
		}
	case IPC_SUB_IPC_CLIENT_CLOSE:		//关闭消息
		{
			//判断连接
			if (hWndSend!=m_hWndChannel)
			{
				return false;
			}

			//发送消息
			tagUserData * pUserData=m_pMeUserItem->GetUserData();
			if (pUserData->wTableID!=WORD(INVALID_TABLE))
			{
				SendStandUpPacket();
			}
			else
			{
				//确定是自己关闭了游戏
				if (pUserData->dwUserID == g_GlobalUnits.GetGolbalUserData().dwUserID)
				{
					SendCancelQueue();
				}
			}

			((CPlazaDlg*)GetParent())->UpdateMusicSound(true);

			//设置变量
			m_hWndChannel=NULL;
			m_pShareMemory->hWndGameFrame=NULL;
			CloseHandle(m_GameProcessInfo.hThread);
			CloseHandle(m_GameProcessInfo.hProcess);
			memset(&m_GameProcessInfo,0,sizeof(m_GameProcessInfo));
			::memset(&m_tagGameFrameStatus, 0, sizeof(m_tagGameFrameStatus));
			HINSTANCE hInstance=AfxGetInstanceHandle();
			//cxf 1108 m_AutoQueue.LoadRgnImage(hInstance,IDB_AUTO_QUEUE);
			//cxf 1108 m_ToolTipCtrl.AddTool(&m_AutoQueue,TEXT("自动排队"));
			//cxf 1108 m_AutoQueue.EnableWindow(TRUE);
			//cxf 1108 m_AutoQueue.Invalidate(TRUE);

			return true;
		}

	}
	return true;
}

//IPC 发包命令
bool CRoomViewItem::OnIPCSocket(const IPC_Head * pHead, const void * pIPCBuffer, WORD wDataSize, HWND hWndSend)
{
	ASSERT(pHead->wMainCmdID==IPC_MAIN_SOCKET);
	switch (pHead->wSubCmdID)
	{
	case IPC_SUB_SOCKET_SEND:	//发包消息
		{
			//效验数据
			ASSERT(wDataSize>=sizeof(CMD_Command));
			if (wDataSize<sizeof(CMD_Command)) 
			{
				return true;
			}

			//处理数据
			IPC_SocketPackage * pSocketPackage=(IPC_SocketPackage *)pIPCBuffer;
			CMD_Command *pCommand= &pSocketPackage->Command;
			WORD wSendSize=wDataSize-sizeof(CMD_Command);


			if (wSendSize==0) 
			{
				m_ClientSocket->SendData(pCommand->wMainCmdID,
					pCommand->wSubCmdID);
			}
			else 
			{
				m_ClientSocket->SendData(pCommand->wMainCmdID,pCommand->wSubCmdID,pSocketPackage->cbBuffer,wSendSize);
			}
			return true;
		}
	}

	return false;
}

//信道数据处理
bool __cdecl CRoomViewItem::OnChannelMessage(const IPC_Head * pHead, const void * pIPCBuffer, WORD wDataSize, HWND hWndSend)
{
	switch (pHead->wMainCmdID)
	{
	case IPC_MIAN_IPC_KERNEL:		//内核消息
		{
			return OnIPCKernel(pHead,pIPCBuffer,wDataSize,hWndSend);
		}
	case IPC_MAIN_SOCKET:			//网络事件
		{
			return OnIPCSocket(pHead,pIPCBuffer,wDataSize,hWndSend);
		}
	case IPC_MAIN_CONCTROL:			//控制消息
		{
			/*if (pHead->wSubCmdID != IPC_SUB_GAMEFRAME_EXIST)
			{
			TCHAR aa[16]={""};
			_snprintf(aa,sizeof(aa),"%d---%d",pHead->wMainCmdID,pHead->wSubCmdID);
			AfxMessageBox(aa);
			}*/

			switch(pHead->wSubCmdID)
			{
			case IPC_SUB_GAMEFRAME_EXIST:	//游戏框架还存在
				{
					m_tagGameFrameStatus.byStatus = 2;
					break;
				}
			case IPC_SUB_GAME_FRAME_CLOSED:	//游戏框架关闭消息
				{
					KillTimer(IDI_GAME_FRAME_IS_EXIST);
					m_tagGameFrameStatus.bIsOpen = false;

					GetParent()->SetForegroundWindow();
					((CPlazaDlg*)GetParent())->UpdateMusicSound(true);

					if (ServiceStatus_NetShutDown == m_ServiceStatus)
					{
						GetParent()->ShowWindow(SW_SHOW);
						GetParent()->PostMessage(WM_COMMAND,IDM_CALL_ROOM_LIST,0);
						break;
					}

					GetParent()->PostMessage(WM_COMMAND, IDB_CALL_UPDATE_USERIMAGE, 0);
					SendRequestCloseRoomMsg(1);

					/*cxf 1022 KillTimer(IDI_GAME_FRAME_IS_EXIST);
					m_tagGameFrameStatus.bIsOpen = false;
					//显示窗口
					m_bQueueState = true;
					HINSTANCE hInstance=AfxGetInstanceHandle();
					m_AutoQueue.LoadRgnImage(hInstance,IDB_AUTO_QUEUE);
					m_ToolTipCtrl.AddTool(&m_AutoQueue,TEXT("自动排队"));
					m_AutoQueue.EnableWindow(TRUE);
					m_AutoQueue.Invalidate(TRUE);

					GetParent()->SetForegroundWindow();
					GetParent()->ShowWindow(SW_SHOW);
					ShowWindow(SW_SHOW);
					GetParent()->PostMessage(WM_COMMAND, IDB_CALL_UPDATE_USERIMAGE, 0);

					((CPlazaDlg*)GetParent())->UpdateMusicSound(true);
					*/
					break;
				}
			case IPC_SUB_JOIN_IN_GAME:	//加入游戏
				{
					IPC_JoinInGame * pJoinInGame=(IPC_JoinInGame *)pIPCBuffer;
					if ((pJoinInGame->wTableID!=INVALID_TABLE)&&(pJoinInGame->wChairID!=INVALID_CHAIR))
					{
						PerformSitDownAction(pJoinInGame->wTableID,pJoinInGame->wChairID);
					}
					break;
				}
			case IPC_SUB_GO_PLAZA:		//返回大厅(游戏列表)
				{
					KillTimer(IDI_GAME_FRAME_IS_EXIST);
					m_tagGameFrameStatus.bIsOpen = false;
					SendRequestCloseRoomMsg(2);

					((CPlazaDlg*)GetParent())->UpdateMusicSound(true);
					break;
				}
			case IPC_SUB_GO_ROOM:		//返回房间列表
				{
					KillTimer(IDI_GAME_FRAME_IS_EXIST);
					m_tagGameFrameStatus.bIsOpen = false;
					SendRequestCloseRoomMsg(1);

					((CPlazaDlg*)GetParent())->UpdateMusicSound(true);
					break;
				}
			case IPC_SUB_USER_UPDATE_DJINFO:	//更新道具信息
				{
					IPC_UserUpdateDJInfo* pDJInfo = (IPC_UserUpdateDJInfo*)pIPCBuffer;
					if (NULL != pDJInfo)
					{
						UpdateUserDJInfo(pDJInfo->iDJID, false);
					}
					break;
				}
			case IPC_SUB_SENDACTIONINFO:		//发送动作
				{
					//效验参数
					ASSERT(wDataSize>=sizeof(IPC_UserActionInfo));
					if (wDataSize<sizeof(IPC_UserActionInfo)) return false;

					IPC_UserActionInfo* pAction = (IPC_UserActionInfo*)pIPCBuffer;
					if (NULL != pAction)
					{//发送动作消息
						m_ClientSocket->SendData(MDM_GR_USER,SUB_SENDACTIONINFO,pAction,sizeof(IPC_UserActionInfo));
					}
					break;
				}
			case IPC_SUB_SENDLANLISTINFO:		//语音消息
				{
					//AfxMessageBox("e");
					//效验参数
					ASSERT(wDataSize>=sizeof(IPC_UserLanguageInfo));
					if (wDataSize<sizeof(IPC_UserLanguageInfo)) return false;

					IPC_UserLanguageInfo* pLInfo = (IPC_UserLanguageInfo*)pIPCBuffer;
					if (NULL != pLInfo)
					{//发送动作消息
						m_ClientSocket->SendData(MDM_GR_USER,SUB_SEND_SOUND_MESS,pLInfo,sizeof(IPC_UserLanguageInfo));
					}
					break;
				}
			case IPC_SUB_CMD_CONNECTWEB:		//打开网站
				{
					if (wDataSize < sizeof(IPC_ConnWeb)) return false;
					IPC_ConnWeb * pConnWeb = (IPC_ConnWeb * ) pIPCBuffer;
					if (NULL != pConnWeb)
					{
						tagGlobalUserData UserData=g_GlobalUnits.GetGolbalUserData();//用户信息
						CString strUrl("");
						
						switch (pConnWeb->byConWebTag)
						{
						case 1:	//官方网站
							{
								strUrl.Format(CInitData::s_cLoginHomePage, UserData.dwUserID, UserData.szKey);
								break;
							}
						case 2:	//道具商城
							{
								strUrl.Format(CInitData::s_cLoginPropShopOpen, UserData.dwUserID, UserData.szKey);
								break;
							}
						case 3:	//客服中心
							{
								strUrl.Format(CInitData::s_cClientCenter);
								break;
							}
						}
						
						ShellExecute(NULL,"open",strUrl,"",NULL,SW_SHOW);
					}
					break;
				}

			default:
				break;
			}
			return true;
		}
	}
	return false;
}

//发送请求关闭房间消息
void CRoomViewItem::SendRequestCloseRoomMsg(BYTE btTag)
{
	//0 == m_btExitTag-->直接返回到登录界面)
	//1 == m_btExitTag-->从桌子列表返回到房间列表
	//2 == m_btExitTag-->返回到游戏大厅
	//3 == m_btExitTag-->退出整个游戏

	//删除动画定时器
	KillTimer(IDI_LOADING_ANIMACTION);

	m_btExitTag = btTag;

	if ((ServiceStatus_NetShutDown == m_ServiceStatus) ||
		(m_ServiceStatus < ServiceStatus_RecvConfigInfo) || (m_bRequestServerExit))
	{//如果SOCKET是处于网络中断状态或者没有状态，则直接执行状态。

		m_bRequestServerExit = false;
		KillTimer(IDI_REQUEST_SERVER_EXIT_MSG);
		switch(m_btExitTag)
		{
		case 0:	//直接返回到登录界面
			{
				//关闭房间
				GetParent()->PostMessage(WM_COMMAND,IDM_CALL_EXIT_GAME,0);
				CloseRoomViewItem();
			}
			break;
		case 1:	//从桌子列表返回到房间列表
			{
				GetParent()->ShowWindow(SW_SHOW);
				GetParent()->PostMessage(WM_COMMAND,IDM_CALL_ROOM_LIST,0);
				CloseRoomViewItem();
			}
			break;
		case 2:	//返回到游戏大厅
			{
				GetParent()->ShowWindow(SW_SHOW);
				GetParent()->PostMessage(WM_COMMAND,IDM_CALL_GAME_LIST,0);
				CloseRoomViewItem();
			}
			break;
		case 3:	//退出整个游戏
			{
				GetParent()->PostMessage(WM_COMMAND,IDM_CALL_CANCEL_GAME,0);
			}
			break;
		default:
			break;
		}
	}
	else
	{
		//发送消息
		tagUserData * pUserData=m_pMeUserItem->GetUserData();
		if (pUserData->wTableID!=WORD(INVALID_TABLE))
		{
			if (pUserData->cbUserStatus==US_PLAY) 
			{
				SendLeftGamePacket();
			}
		}
		//退出发送给服务器。
		m_ClientSocket->SendData(MDM_GR_USER,SUB_GF_QUIT_GAME_SERVER);

		//设置定时器
		m_bRequestServerExit = false;
		SetTimer(IDI_REQUEST_SERVER_EXIT_MSG, TIMER_REQUEST_SERVER_INTER, NULL);
	}

}

//鼠标左键按下
void __cdecl CRoomViewItem::OnLButtonHitTable(WORD wTableID, WORD wChairID, bool bMirror)
{
	//判断状态
	if (m_ServiceStatus==ServiceStatus_NetShutDown)
	{
		if ((wTableID!=INVALID_TABLE)&&(wChairID!=INVALID_CHAIR))
		{
			LPCTSTR szMessage=TEXT("与游戏服务器的连接已经中断，是否退出当前游戏房间？");
			int iCode=ShowMessageBox(szMessage,MB_ICONINFORMATION|MB_YESNO);
			//if (iCode==IDYES) ((CGameFrame*)AfxGetMainWnd())->CloseRoomViewItem(this);
			GetParent()->ShowWindow(SW_SHOW);
			GetParent()->PostMessage(WM_COMMAND,IDM_CALL_GAME_LIST,0);
			CloseRoomViewItem();
		}
		return;
	}

	//动作过虑
	if (m_ServiceStatus!=ServiceStatus_Serviceing) 
	{
		return;
	}
	if ((wTableID==m_wReqTableID)&&(wChairID==m_wReqChairID)) 
	{
		return;
	}
	if ((wTableID==INVALID_TABLE)||(wChairID==INVALID_CHAIR)) 
	{
		return;
	}
	//消息处理
	IUserItem * pTableUserItem=m_TableFrame.GetUserInfo(wTableID,wChairID);
	if (pTableUserItem!=NULL)
	{
		//旁观或者起来
		if (pTableUserItem==m_pMeUserItem)
		{
			if (m_pMeUserItem->GetUserData()->cbUserStatus>=US_PLAY) 
			{
				return;
			}
			if ((m_wReqTableID!=INVALID_TABLE)||(m_wReqChairID!=INVALID_CHAIR)) 
			{
				return;
			}
			m_wFindTableID=INVALID_TABLE;
			SendStandUpPacket();
		}
		else 
		{
			tagUserData * pUserData=m_pMeUserItem->GetUserData();
			if((pUserData->wTableID==wTableID)
				&& (pUserData->wChairID==wChairID)) 
			{
				return;
			}
			PerformLookonAction(wTableID,wChairID);
		}
	}
	else 
	{
		//坐下处理
		if (m_cbHideUserInfo==TRUE)
		{
			SendSitDownPacket(INVALID_TABLE,INVALID_CHAIR,NULL);
		}
		else
		{
			PerformSitDownAction(wTableID, wChairID);
		}
	}

	return;
}

//鼠标右键按下
void __cdecl CRoomViewItem::OnRButtonHitTable(WORD wTableID, 
											  WORD wChairID, 
											  bool bMirror)
{
	//获取桌子
	ITableView * pITableView=m_TableFrame.GetTableArrayPtr(wTableID);
	if (pITableView==NULL) 
	{
		return;
	}

	//获取用户
	IUserItem * pIUserItem=pITableView->GetUserInfo(wChairID);
	if (pIUserItem==NULL) 
	{
		return;
	}

	//显示菜单
	CPoint Point;
	GetCursorPos(&Point);
	ShowUserInfoMenu(pIUserItem,Point);

	return;
}

//鼠标双击
void __cdecl CRoomViewItem::OnDoubleHitTable(WORD wTableID, bool bMirror)
{
}

//用户激活
void __cdecl CRoomViewItem::OnUserItemAcitve(IUserItem * pIUserItem)
{
	//m_UserListView.InsertUserItem(pIUserItem);
	return ;
}

//用户更新
void __cdecl CRoomViewItem::OnUserItemUpdate(IUserItem * pIUserItem)
{
	//m_UserListView.UpdateUserItem(pIUserItem);
	if (m_pFindUserDlg!=NULL) 
	{
		m_pFindUserDlg->UpdateUserItem(pIUserItem);
	}

	return ;
}

//用户删除
void __cdecl CRoomViewItem::OnUserItemDelete(IUserItem * pIUserItem)
{
	//获取用户
	ASSERT(pIUserItem!=NULL);
	DWORD dwUserID=pIUserItem->GetUserID();

	//聊天对象
	if (m_dwChatUserID==dwUserID)
	{
		SetChatObject(NULL);
		TCHAR szMessage[256]=TEXT("");
		_sntprintf(szMessage,sizeof(szMessage),TEXT("[ %s ] 离开了，聊天对象设置为所有人"),pIUserItem->GetUserName());
		m_MessageProxyHelper->InsertSystemString(szMessage,0);
	}

	//聊天对象
	// 	int nChatObjectCount=m_ChatObject.GetCount();
	// 	for (int i=1;i<nChatObjectCount;i++)
	// 	{
	// 		if (m_ChatObject.GetItemData(i)==dwUserID)
	// 		{
	// 			if (m_ChatObject.GetCurSel()==i) 
	// 			{
	// 				m_dwChatUserID=0;
	// 				m_szChatName[0]=0;
	// 				m_ChatObject.SetCurSel(0);
	// 			}
	// 			m_ChatObject.DeleteString(i);
	// 			break;
	// 		}
	// 	}

	//控制视图
	//m_UserListView.DeleteUserItem(pIUserItem);

	if (m_pFindUserDlg!=NULL)
	{
		m_pFindUserDlg->DeleteUserItem(pIUserItem);
	}
	return;
}

//激活信息窗口
CShortMessage * CRoomViewItem::ActiveShortWnd(long int dwUserID, IUserItem * pUserItem, bool bCreate)
{
	/*
	//变量定义
	INT_PTR iCloseIndex=-1,iHideIndex=-1;

	//寻找敏合窗口
	for (INT_PTR i=0;i<m_ShortMessage.GetCount();i++)
	{
	CShortMessage * pShortMessageWnd=(CShortMessage *)m_ShortMessage.GetAt(i);
	ASSERT(pShortMessageWnd!=NULL);
	if (dwUserID==pShortMessageWnd->m_dwTargetUserID)
	{
	if (pShortMessageWnd->GetSafeHwnd()==NULL)
	{
	AfxSetResourceHandle(GetModuleHandle(NULL));
	pShortMessageWnd->Create(IDD_SHORT_MESSAGE,GetDesktopWindow());
	}
	pShortMessageWnd->ShowWindow(SW_SHOW);
	return pShortMessageWnd;
	}
	if (bCreate==true)
	{
	if ((iCloseIndex==-1)
	&&  (pShortMessageWnd->GetSafeHwnd()==NULL)) 
	{
	iCloseIndex=i;
	}
	if ((iHideIndex==-1)
	&& (pShortMessageWnd->GetSafeHwnd()!=NULL)
	&& (pShortMessageWnd->IsWindowVisible()==FALSE)) 
	{
	iHideIndex=i;
	}
	}
	}

	//使用关闭的窗口
	if ((bCreate==true)&&(iCloseIndex>=0))
	{
	CShortMessage *pShortMessageWnd = (CShortMessage *)m_ShortMessage.GetAt(iCloseIndex);
	ASSERT(pShortMessageWnd!=NULL);
	if (pShortMessageWnd->GetSafeHwnd()==NULL)
	{
	AfxSetResourceHandle(GetModuleHandle(NULL));
	pShortMessageWnd->Create(IDD_SHORT_MESSAGE,GetDesktopWindow());
	}
	pShortMessageWnd->SetTalkInfo(pUserItem,m_pListServer->GetItemInfo()->szServerName);
	pShortMessageWnd->ShowWindow(SW_SHOW);
	//置顶窗口
	pShortMessageWnd->SetActiveWindow();
	pShortMessageWnd->BringWindowToTop();
	return pShortMessageWnd;
	}

	//使用隐藏窗口
	if ((bCreate==true)&&(iHideIndex>=0))
	{
	CShortMessage * pShortMessageWnd=(CShortMessage *)m_ShortMessage.GetAt(iHideIndex);
	ASSERT(pShortMessageWnd!=NULL);
	pShortMessageWnd->SetTalkInfo(pUserItem,m_pListServer->GetItemInfo()->szServerName);
	pShortMessageWnd->ShowWindow(SW_SHOW);
	//置顶窗口
	pShortMessageWnd->SetActiveWindow();
	pShortMessageWnd->BringWindowToTop();
	return pShortMessageWnd;
	}

	//建立新窗口
	if ((bCreate==true)&&(m_ShortMessage.GetCount()<10))
	{
	try
	{
	CShortMessage * pShortMessageWnd=new CShortMessage(this);
	AfxSetResourceHandle(GetModuleHandle(NULL));
	pShortMessageWnd->SetTalkInfo(pUserItem,m_pListServer->GetItemInfo()->szServerName);
	pShortMessageWnd->Create(IDD_SHORT_MESSAGE,GetDesktopWindow());
	pShortMessageWnd->ShowWindow(SW_SHOW);
	//置顶窗口
	pShortMessageWnd->SetActiveWindow();
	pShortMessageWnd->BringWindowToTop();

	m_ShortMessage.Add(pShortMessageWnd);

	return pShortMessageWnd;
	}
	catch (...) 
	{ 
	return NULL; 
	}
	}
	*/

	return NULL;
}

//聊天对象
bool CRoomViewItem::SetChatObject(IUserItem * pIUserItem)
{
	if (pIUserItem!=NULL)
	{
		//设置变量
		tagUserData * pUserData=pIUserItem->GetUserData();
		m_dwChatUserID=pUserData->dwUserID;
		lstrcpyn(m_szChatName,pUserData->szName,CountArray(m_szChatName));

		//寻找玩家
		int nItemCount=m_ChatObject.GetCount();
		for (int i=0;i<nItemCount;i++)
		{
			if (m_ChatObject.GetItemData(i)==m_dwChatUserID)
			{
				m_ChatObject.SetCurSel(i);
				return true;
			}
		}

		//插入对象
		int iItem=m_ChatObject.AddString(m_szChatName);
		m_ChatObject.SetItemData(iItem,m_dwChatUserID);
		m_ChatObject.SetCurSel(iItem);
	}
	else
	{
		//设置变量
		m_dwChatUserID=0L;
		m_szChatName[0]=0;

		//设置界面
		m_ChatObject.SetCurSel(0);
	}

	return true;
}


void CRoomViewItem::OnBnClickedReLogon()
{
	// 	tagGlobalUserData & GlobalUserData=g_GlobalUnits.GetGolbalUserData();
	// 	//删除记录
	// 	g_GlobalUnits.DeleteUserCookie();
	// 	//设置变量
	// 	memset(&GlobalUserData,0,sizeof(GlobalUserData));
	// 	//投递消息
	// 	GetParent()->PostMessage(WM_COMMAND,IDM_LOGON_PLAZA,0);
	CloseRoomViewItem();
}

void CRoomViewItem::OnLButtonDown( UINT nFlags, CPoint point )
{	

	if (IsAdministrationHavePurview())
	{
		m_TableFrame.OnLButtonDown(nFlags, point);
	}

	if (m_rcCloseRect.PtInRect(point))
	{//按下关闭按钮
		m_wCloseState=1;
		InvalidateRect(&m_rcCloseRect,FALSE);
		return;
	}
	GetParent()->PostMessage(WM_NCLBUTTONDOWN, HTCAPTION, MAKELPARAM(point.x,point.y));

	CDialog::OnLButtonDown(nFlags, point );
}

//鼠标消息
void CRoomViewItem::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	if (IsAdministrationHavePurview())
	{	
		m_TableFrame.OnLButtonDblClk(nFlags, point);
	}

	CDialog::OnLButtonDblClk(nFlags, point );
}
//鼠标消息
void CRoomViewItem::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (IsAdministrationHavePurview())
	{
		m_TableFrame.OnLButtonUp(nFlags, point);
	}

	if (m_rcCloseRect.PtInRect(point))
	{
		//关闭窗口
		OnTableClose();
		return;
	}

	CDialog::OnLButtonUp(nFlags, point );
}
//鼠标消息
void CRoomViewItem::OnRButtonDown(UINT nFlags, CPoint point)
{
	if (IsAdministrationHavePurview())
	{
		m_TableFrame.OnRButtonDown(nFlags, point);
	}
	CDialog::OnRButtonDown(nFlags, point );
}
//鼠标消息
void CRoomViewItem::OnRButtonUp(UINT nFlags, CPoint point)
{
	if (IsAdministrationHavePurview())
	{
		m_TableFrame.OnRButtonUp(nFlags, point);
	}
	CDialog::OnRButtonUp(nFlags, point );
}
//光标消息
BOOL CRoomViewItem::OnSetCursor(CWnd * pWnd, UINT nHitTest, UINT uMessage)
{
	if (IsAdministrationHavePurview())
	{
		if (m_TableFrame.OnSetCursor(pWnd, nHitTest, uMessage))
			return TRUE;
	}
	return CDialog::OnSetCursor(pWnd, nHitTest, uMessage);
}
void CRoomViewItem::OnDestroy()
{
	CDialog::OnDestroy();
	
	m_ImgAnimAction.DestroyImage();
	m_ImgClose.DestroyImage();
	m_pngNumber.DestroyImage();
	m_pngText1.DestroyImage();
	m_pngText2.DestroyImage();
	m_pngImageHou.DestroyImage();
	m_pngImageCurSort.DestroyImage();
	m_pngImageDayWin.DestroyImage();
	m_pngGameICO.DestroyImage();
	m_ImageBack.DestroyImage();
	//m_ImageBackGM.DestroyImage();

	ClearMemDC();
}
//清空句柄
void CRoomViewItem::ClearMemDC()
{
	if (m_memdc.m_hDC)
		m_memdc.DeleteDC();
	m_memdc.m_hDC = NULL;
}

//取消连接
void CRoomViewItem::OnCommandCancelConnect()
{
	g_GlobalAttemper.DestroyStatusWnd(this);

	if(m_ServiceStatus < ServiceStatus_RecvConfigInfo)
	{
		//退出发送给服务器。
		m_ClientSocket->SendData(MDM_GR_USER,SUB_GF_QUIT_GAME_SERVER);

		OnBnReturn();
	}
	else
	{
		GetParent()->ShowWindow(SW_SHOW);
		GetParent()->PostMessage(WM_COMMAND,IDM_CALL_ROOM_LIST,0);
		CloseRoomViewItem();
	}
	return;
}


//时间消息
void CRoomViewItem::OnTimer(UINT nIDEvent)
{
	CDialog::OnTimer(nIDEvent);
	switch(nIDEvent)
	{
	case IDI_GAME_FRAME_IS_EXIST:		//检测游戏框架是否存在
		{
			if (m_tagGameFrameStatus.bIsOpen)
			{//窗口已经打开过了.
				if (::IsWindow(m_hWndChannel) && 2==m_tagGameFrameStatus.byStatus)
				{//存在
					m_tagGameFrameStatus.byStatus = 1;
					CIPCSendCopyData SendCopyData(m_hWndChannel,m_hWnd);
					SendCopyData.SendData(IPC_MAIN_CONCTROL,IPC_SUB_GAMEFRAME_EXIST);
				}
				else
				{//不存在,关闭游戏框架，并退回到游戏列表.
					KillTimer(IDI_GAME_FRAME_IS_EXIST);
					m_bRequestServerExit = true;
					/*GetParent()->SetForegroundWindow();
					GetParent()->ShowWindow(SW_SHOW);
					SendRequestCloseRoomMsg(2);*/

					((CPlazaDlg*)GetParent())->UpdateMusicSound(true);
					m_ClientSocket->CloseSocket();
					m_ServiceStatus=ServiceStatus_NetShutDown;
					//关闭房间
					GetParent()->PostMessage(WM_COMMAND,IDM_CALL_ROOM_LIST,0);
					CloseRoomViewItem();
				}
			}
			else
			{
				m_bQueueState = true;
				//cxf 1108 HINSTANCE hInstance=AfxGetInstanceHandle();
				//cxf 1108 m_AutoQueue.LoadRgnImage(hInstance,IDB_AUTO_QUEUE);
				//cxf 1108 m_ToolTipCtrl.AddTool(&m_AutoQueue,TEXT("自动排队"));
				//cxf 1108 m_AutoQueue.EnableWindow(TRUE);
				//cxf 1108 m_AutoQueue.Invalidate(TRUE);

				KillTimer(IDI_GAME_FRAME_IS_EXIST);
				GetParent()->SetForegroundWindow();
				GetParent()->ShowWindow(SW_SHOW);
				ShowWindow(SW_SHOW);

				((CPlazaDlg*)GetParent())->UpdateMusicSound(true);
			}
			break;
		}
	case IDI_REQUEST_SERVER_EXIT_MSG:		//请求服务器游戏客户端退出
		{
			KillTimer(IDI_REQUEST_SERVER_EXIT_MSG);
			m_bRequestServerExit = true;
			break;
		}
	case IDI_LOADING_ANIMACTION://Loading动画
		{
			//动画帧数（总为12帧）
			if (m_wAnimAction>=11)
			{
				m_wAnimAction=0;
			}
			else
			{
				m_wAnimAction++;
			}
			InvalidateRect(&m_rcAnimAction);
		}
	default:
		break;
	}
}


//发送公告信息
void CRoomViewItem::SendToGameFrameAfficheInfo()
{
	if (::IsWindow(m_hWndChannel) && m_tagGameFrameStatus.bIsOpen && (m_strSaveGameAffiche.size() > 0))
	{
		//发送消息
		CIPCSendCopyData IPCSendCopyData(m_hWndChannel,m_hWnd);
		IPCSendCopyData.SendData(IPC_MAIN_CONFIG,IPC_SUB_GAMEAFFICHE_INFO,(char*)m_strSaveGameAffiche.c_str(),m_strSaveGameAffiche.size());
	}
}
//鼠标移动
void CRoomViewItem::OnMouseMove(UINT nFlags, CPoint point)
{
	if (m_rcCloseRect.PtInRect(point))
	{//移动关闭按钮上
		if (2 != m_wCloseState)
		{
			m_wCloseState=2;
			InvalidateRect(&m_rcCloseRect,FALSE);
		}
	}
	else
	{//移开关闭按钮上
		if (0 != m_wCloseState)
		{
			m_wCloseState=0;
			InvalidateRect(&m_rcCloseRect,FALSE);
		}
	}

	CDialog::OnMouseMove(nFlags, point);
}

//更新道具消息
void CRoomViewItem::UpdateUserDJInfo(int iDJID, bool bIsRoom)
{	
	//发送命令
	//CMD_GF_User_Upate_Prop userdjinfo;
	//userdjinfo.cbSendLocation = LOCATION_PLAZA_ROOM;
	//userdjinfo.nPropertyID = iDJID;
	//m_ClientSocket->SendData(MDM_GF_PRESENT,SUB_GF_USER_UPDATE_PROP,&userdjinfo,sizeof(CMD_GF_User_Upate_Prop));
}

//当前路径
void CRoomViewItem::GetCurrentDirectoryPath(CString &szPath)
{
	szPath="";
	TCHAR szPathTemp[MAX_PATH]={0};
	GetCurrentDirectory(MAX_PATH, szPathTemp);
	szPath.Append(szPathTemp);
}

//大厅资源路径
void CRoomViewItem::GetImagePlazaPath(CString& strRetValue)
{
	CString strModuleFileName("");
	GetCurrentDirectoryPath(strModuleFileName);
	strModuleFileName.Append("\\data\\plaza\\");
	strRetValue = strModuleFileName;
}

void CRoomViewItem::OnTableClose()
{
	SendRequestCloseRoomMsg(1);
}
//////////////////////////////////////////////////////////////////////////

BOOL CRoomViewItem::DestroyWindow()
{
	// TODO: 在此添加专用代码和/或调用基类

	// 发送关闭提示框的事件
	SetEvent(m_hClosePromtWndr);	
	WaitForSingleObject(m_hThread,  30000);

	CloseHandle(m_hClosePromtWndr);
	m_hClosePromtWndr = NULL;
	return __super::DestroyWindow();
}
