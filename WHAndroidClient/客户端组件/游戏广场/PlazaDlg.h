#pragma once
#include "resource.h"
#include "afxcmn.h"
#include "DlgLogon.h"

#include "UserInfoView.h"
#include "ServerItemView.h"

#include "GameListManager.h"
#include "IGamePlaza.h"

#include "ImageNums.h"

#include "GameListHot.h"
#include "PlazaGameWeb.h"
#include "afxwin.h"
#include "GameListCenterDlg.h"
#include "RoomListCenterDlg.h"
#include "PlazaCloseConfirm.h"
#include "ConfirmGold.h"
#include "ConfirmGrade.h"
#include "SkinControls.h"

#include "NewHandTask.h"
#include "IpFinder.h"

// CPlazaDlg 对话框

using namespace std;

//函数指针
typedef BOOL (WINAPI *ShowLayer)(HWND,COLORREF,BYTE,DWORD); 

//////////////////////////////////////////////////////////////////////////

//消息定义
#define		WM_DOWN_LOAD_FACE		WM_USER+100				//下载消息
#define		MAX_SERVER				3	
#define		IDI_HOT_KEY_BOSS		0x0100					//老板热键
#define		WAIT_END_EVENT			1						//等待结束事件

class CActive;

//自定义头像
struct tagCustomFace
{
	DWORD						dwUserID;				//玩家 ID
	BYTE						*pFaceData;				//头像数据
	DWORD						dwFaceSize;				//头像大小
	DWORD						dwCurrentSize;			//当前大小

	//构造函数
	tagCustomFace():dwUserID(0),pFaceData(NULL),dwFaceSize(0),dwCurrentSize(0){}

	//清除内存
	void Clear(void)
	{
		delete[]pFaceData;
		pFaceData=NULL;
		dwUserID=0;
		dwFaceSize=0;
		dwCurrentSize=0;
	}

	//析构函数
	~tagCustomFace()
	{
		Clear();
	}

};


//图片按钮
struct PicButton 
{
	CPngImage	BTImage;
	CRect		rcRect;
	BYTE		btStatus;	//0:默认正常状态; 1:鼠标移上去状态; 2:鼠标按下状态; 3:高亮显示状态.

	PicButton()
	{
		btStatus = 0;
	}
};


//下载信息
struct tagDownloadInfo
{
	DWORD						dwUserID;				//玩家 ID
	BYTE						bFaceVer;				//头像版本
	BYTE						bReference;				//引用计数

	//初始化
	tagDownloadInfo():dwUserID(0),bFaceVer(0),bReference(1){}
};

//类型定义
typedef CArrayTemplate<tagDownloadInfo>			CDownloadInfoArrary;
//////////////////////////////////////////////////////////////////////////

class CPlazaDlg : public CDialog, public IPlazaViewItem,public ITCPSocketSink
{
	DECLARE_DYNAMIC(CPlazaDlg)

public:
	CPlazaDlg(CWnd* pParent =NULL);   // 标准构造函数
	virtual ~CPlazaDlg();


// 对话框数据
	enum { IDD = IDD_PLAZADLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	virtual int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnEraseBkgnd(CDC * pDC);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
//	afx_msg void OnWindowPosChanged(WINDOWPOS* lpwndpos);

	//时间消息
	afx_msg void OnTimer(UINT nIDEvent);

	DECLARE_MESSAGE_MAP()

	//界面
protected:
	//重排控件
	void ResizeFrame();	 
	//连接桌子
	bool CreateTableList(CListServer * pListServer);

protected:
	//连接服务器
	afx_msg void OnCommandConnect();
	//取消连接
	afx_msg void OnCommandCancelConnect();
	 
protected:
	//调用RoomList
	void OnCallRoomList();
	//调用GameList
	void OnCallGameList();
	//
	void OnCallTableList();
	//更新用户形象
	void OnCannUpdateUserImage();
	//直接进入房间
	void OnFlashEnterRoom();

	// 弹出提示框
	LRESULT OnCallShowCustomer(WPARAM wParam, LPARAM lParam);

	//Inno 安装脚本发来的消息 关闭大厅消息
    LRESULT OnCallInnoIssMsgClose(WPARAM wParam, LPARAM lParam);

	LRESULT OnCallCheckConnect(WPARAM wParam, LPARAM lParam);

public:
	//Inno 安装脚本发来的消息 选择游戏
	LRESULT OnCallInnoIssMsgSelectGame(WPARAM wParam, LPARAM lParam);

public:
	//释放对象
	virtual void __cdecl Release() { delete this; }
	//接口查询
	virtual void * __cdecl QueryInterface(const IID & Guid, DWORD dwQueryVer);
	//视图接口
public:
	//获取句柄
	virtual HWND __cdecl GetWindowHandle() { return GetSafeHwnd(); }
	//网络事件
public:
	//连接事件
	virtual bool __cdecl OnEventTCPSocketLink(WORD wSocketID, INT nErrorCode);
	//关闭事件
	virtual bool __cdecl OnEventTCPSocketShut(WORD wSocketID, BYTE cbShutReason);
	//读取事件
	virtual bool __cdecl OnEventTCPSocketRead(WORD wSocketID, CMD_Command Command, VOID * pData, WORD wDataSize);
	//头像变量
protected:
	bool						m_bStartDownloadFace			;	//下载标识
	bool						m_bDownloadConnect;					//下载标识
	tagCustomFace				m_CustomFace;						//自定义头像
	CDownloadInfoArrary			m_DownloadInfoArrary			;	//下载队列
	int							m_curTab;						;	//当前页面 
public:
	bool						m_bLogonPlaza;						//登录标志
	bool						m_bInitDialog;						//初始标志
public://界面
	//cxf CDlgLogon					m_dlgLogon;						//登陆界面(包括注册界面)
	CDlgLogon					* m_pDlgLogon;						//登陆界面(包括注册界面)
	CActive							* m_pActivity;							//活动专区
	
	//
	//cxf CGameListDlg				m_gameListDlg;						//游戏列表界面(包括广告、游戏列表)
	//cxf1022 CRoomListDlg				m_roomListDlg;						//房间列表界面(服务器列表)
	CRoomViewItem				* m_pRoomViewItemCreate;			//创建房间(房间里所有桌子视图，桌子列表)
	//CFriendListDlg				m_friendlistDlg;					//好友列表
	//cxf 1022 CTaskDialog					m_TaskDlg;							//任务
	//cxf 1022 CShopDialog					m_shopDlg;							//商城
	//cxf 1022 CSetDialog					m_setdlg;							//设置
	//cxf 1022 CDoteyBoxDialog				m_doteyBoxDlg;						//藏宝阁
	//cxf 1022 CShowGameMsg				m_ShowGameMsg;						//显示游戏消息
	CDC							m_memdc;

	CRgnButton					m_btnReturn;						//返回     IDC_BT_RETURN
	CRgnButton					m_btnSound;							//音乐	   IDC_BT_SET
	CRgnButton					m_btnClose;							//关闭     IDC_BT_CLOSE
	CRgnButton					m_btnMinsize;						//最小化   IDCMINSIZE
	CRgnButton					m_btnReLogon;						//重新登录 IDC_BT_RE_LOGON
	CToolTipCtrl				m_ToolTipCtrl;						//提示控件
	//cxf
	CGameListHot				m_GameListHot;						//热门游戏
	CPlazaGameWeb				m_PlazaGameWeb;						//大厅游戏网页
	CGameListCenterDlg			m_GameListCenterDlg;				//游戏列表
	CRoomListCenterDlg			m_RoomListCenterDlg;				//房间列表
	CPlazaCloseConfirm			m_PlazaCloseConfirm;				//退出确认框
	CConfirmGold				m_ConfirmGold;						//金币不足框
	CConfirmGrade				m_ConfirmGrade;						//等级限制
	
	CIpFinder					m_IpFinder;							//IP检查类

public:
	CMD_GP_UserGradeInfo *		m_pGradeInfo;						//等级信息
	int							m_iGradeInfoDiv;					//等级结构大小

	WORD						m_wTitlePos;						//标题位置

	DWORD						m_dwLastConnectTime;				///< 上次连接房间时间
	DWORD						m_dwConnectIntervalTime;			///< 连接间隔时间

	//cxf
private:
	bool						m_bIsGetGlod;						//获取金币
	bool						m_bIsUpdateUserInfo;				//更新用户信息
	bool						m_bCheckLine;						///< 检查线路
	bool						m_bDeleteMachineCode;				///< 删除登陆服务器机器码
	CRgnButton					m_btnVisitWeb;						//官方网站
	CRgnButton					m_btnAccountsManage;				//帐号管理
	CRgnButton					m_btnGameHelp;						//游戏帮助
	CRgnButton					m_btnQuestionFeedBack;				//问题反馈
	CRgnButton					m_btnContactServise;				//在线客服
	CRgnButton					m_btnGameList;						//游戏列表
	CRgnButton					m_btnTounament;						//比赛专区
	CRgnButton					m_btnAccountsRecharge;				//帐号充值
	CRgnButton					m_btnPrizeExcharge;					//奖品兑换
	CRgnButton					m_btnPropsShop;						//道具商城
	CRgnButton					m_btnNoviceTask;					//新手任务
	CRgnButton					m_btnUpdateUserInfo;				//更新用户信息
	CRgnButton					m_btnBackpack;						//背包
	CRgnButton					m_btnUserInfo;						//用户信息
	CRgnButton					m_btnBank;							//银行
	CRgnButton					m_btnEmailMessage;					//邮件
	CRgnButton					m_btnRollback;						// 返回
	CRgnButton					m_btnNextPage;						// 下一页
	CRgnButton					m_btnNewComerGuide;					//新手指引

	CWebBrowser					m_webPlazaAdvertisement;			//大厅广告
	CWebBrowser					m_webPlazaNotice;					//大厅公告
	CWebBrowser					m_webPlazaMessage;					//大厅消息
	CWebBrowser					m_webPlazaTitle;					//大厅头部广告

	CGameFlash					m_GameFlash;						//动画窗口
	
private:
	CString						m_strCurrentPathPlaza;				//大厅资源路径
	CPngImage					m_ImageTitle;						//游戏列表标题文字
	CRect						m_rcTitleArea;						//标题区域位置

	CRect						m_rcNewComerGuideArea;				//新手指引区域
	CPngImage					m_ImageNewComerGuide;				//新手指引图片
	bool						m_bNewComerGuide;					//是否动画新手指引
	WORD						m_wNewComerGuideActionNumber;		//动作帧

	//绘制背景与形象
private:
	CPngImage		m_ImageBack;				//背景
	CPngImage		m_UserImage;				//用户形象
	//cxf CPngImage		m_UserInfo;					//用户信息背景
	CPngImage		m_Exp1Image;				//经验条
	CPngImage		m_Exp2Image;				//经验条2
	CPngImage		m_UserXJImage;				//玩家星级
	CPngImage		m_XJInfoBackImage;			//星级背景
	//cxf CPngImage		m_PngHat;					//帽子
	CImageNums		m_ImageNums;				//数字
	CRect			m_UserXJRect;				//星级区域
	CRect			m_UserXJInfoRect;			//星级信息显示区域
	bool			m_bIsShowXJInfo;			//是否显示星级信息
	bool			m_bIsGamePlazyStatus;		//是否是游戏大厅状态
	bool			m_bIsSound;					//是否播放背景音乐

	CImageNums		m_ImageNumsTel;				//电话号码
	//绘制消息动画
	/*cxf DWORD			m_dwGameMsgIndex;			//消息序号
	CPngImage		m_GameMsgImage;				//图片
	bool			m_bIsFlashMsg;				//是否闪动消息提示
	CRect			m_rcGameMsgArea;			//游戏消息区域
	BYTE			m_byFlashMsgStatus;			//状态. 0:正常状态; 1:移上去状态
	*/
	//初始化图片信息
	void InitDlgBK();
	//艺术字体
	void DrawTextString(CDC * pDC, LPCTSTR pszString, COLORREF crText, COLORREF crFrame, int nXPos, int nYPos);
	//计算星级
	void CountXJClass(DWORD dwWinNums, DWORD dwTotalNums, WORD& wClass);
	//绘制星级
	void DrawXJInfo(CDC* pDC);
	
public:
	CTCPSocketHelper			m_ClientSocket;						//网络连接

	//游戏列表管理
	CGameListManager			m_gameListManager;					//游戏列表管理
	CServerItemView				m_ServerItemView;					//服务器视图 
	
public:
	//显示消息
	int ShowMessageBox(LPCTSTR pszMessage, UINT nType);
	//更新形象
	void UpdateUserImage();
	//更新用户宝石
	void UpdateUserGold(DWORD dwGold);
	//更新用户金币
	void UpdateUserMoney(DWORD dwMoney);

	//消息处理
protected:
	//登录消息
	bool OnSocketMainLogon(CMD_Command Command, void * pBuffer, WORD wDataSize);
	//列表消息
	bool OnSocketMainServerList(CMD_Command Command, void * pBuffer, WORD wDataSize);
	//系统消息
	bool OnSocketMainSystem(CMD_Command Command, void * pBuffer, WORD wDataSize);
	//用户消息
	bool OnSocketMainUser(CMD_Command Command, void * pBuffer, WORD wDataSize);
public:
	//网络链接
	bool ConnectServer();
	//登录广场
	afx_msg void OnCommandLogon();
	//广场接口
public:
	//登录消息
	virtual bool __cdecl SendLogonMessage();
	//连接消息
	virtual bool __cdecl SendConnectMessage();

	//好友列表操作
public:
	//CFriendListDlg * GetFriendListInterface() { return &m_friendlistDlg; }
	afx_msg void OnMove(int x, int y);
protected:
	virtual void OnOK();
	virtual void OnCancel();
public:
	afx_msg void OnPaint();
	void OnBnClickedClose();	//关闭程序
	void ResetLogon();			//重新登录
	afx_msg void OnBnClickedReLogon();
// 	//我的宝箱 IDC_BT_BEAUTY_SALON
// 	afx_msg void OnBnClickedBeautySalon();
// 	//游戏商场 IDC_BT_SHOP
// 	afx_msg void OnBnClickedGameShop();
	//返回     IDC_BT_RETURN
	afx_msg void OnBnClickedReturn();
	//设置	   IDC_BT_SET
	afx_msg void OnBnClickedSet();
	//最小化   IDCMINSIZE
	afx_msg void OnBnClickedMinsize();
	afx_msg void OnBnClickedShop();
	afx_msg void OnBnClickedTask();
	afx_msg void OnBnClickedDepository();
	afx_msg void OnBnClickedGamePlazy();

	//返回到登录界面
	afx_msg void OnBnExitLogoUI();
	afx_msg void OnBnCancelGame();

	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//位置消息
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);

	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	//关闭确认
	afx_msg void ConfirmClose();

public:
	DECLARE_DISPATCH_MAP()//声明dispatch map表
	DECLARE_EVENTSINK_MAP()
	BOOL onHtmlContextMenu(IHTMLEventObj *pEvtObj);
	void OnDocumentCompleteExplorer3(LPDISPATCH pDisp, VARIANT FAR* URL);
	//事件处理函数,原型可以参考MSDN中关于HTMLDocumentEvents2的说明
	DWORD m_dwCookie;
	//用于标记一个连接点
	IHTMLDocument2* pDoc2;
	//想要处理事件的COM对象的指针

public:
	//关闭窗口
	void CloseWebWindow(int nIDD);
	//更新用户道具信息
	void UpdateUserDJInfo(int iDJID);
	//更新声音
	void UpdateMusicSound(bool bIsEnable=true);
	//开启或关闭声音
	void SetMusicSound(bool bIsEnable=true);
	//设置登录窗口
	void SetDlgLogonPtr(CDlgLogon * pDlgLogon);
	//显示界面
	void ShowTab(int nPage);
	//切换标题::wPos标题图片的位置
	void SwitchTitle(WORD wPos);
	//创建嵌套网页::wWebMode页面样式,wTitlePos标题位置
	void LoadPlazaGameWeb(WORD wWebMode,WORD wTitlePos);
	/// 检测本地软件HASH值与服务器HASH值否一样，来决定是否需要更新
	/// [return] 为真表示有更新， 为假不需要更新
	BOOL CheckGameSoftHash(LPCTSTR pNewHash);
	// 调用外部升级程序
	BOOL CallExternalUpdateProgram();

private:
	BOOL			m_bIsGameSoftUpdate;	// 游戏软件是否需要更新
	//实例句柄
	HINSTANCE		m_hUser32Instance;
	//函指针
	ShowLayer		m_pFunShowLayer;
	//当前alpha值
	int				m_uiCurAlphaValue;
	//当前窗口
	HWND			m_hSaveNewCurWnd;
	HWND			m_hSaveOldCurWnd;
	//当前窗口标识
	BYTE			m_byCurWndTag;	//0:表示可以执行，1:淡出，2:淡入
	//重置alpha值
	void	SetResetAlphaValue(HWND hWnd);
	//设置所有窗口显示状态
	void	SetAllWndShow(bool bIsShow=false);
	//设置所有窗口透明度
	void	SetAllWndAlpha(BYTE byAlpha=0);
	//是否绘制背景
	//bool			m_bIsDrawBack;
	

private:
	//字体
	CFont			m_fontNickName;
	CFont			m_fontXPValue;
	CFont			m_fontDrawXJ;

	CNewHandTask*	m_pNewHandTask;

private:
	//0:游戏大厅; 1:任务; 2:藏宝阁; 3:商城
	//cxf 1024 PicButton		m_TitleButtonList[4];	//标题栏按钮.

	//执行相应按钮
	void  ExeButtonEvent(BYTE byEventID);
	//绘制所有按钮
	void  DrawTitleButton(CDC *pDC);
	//隐藏热门游戏标志位
	void HideFlag();

private:
	//显示相应按钮
	void SetShowTab(BYTE byValue=1);
	//进入官网
	void OnBnClickedVisitWeb();
	//帐号管理
	void OnBnClickedAccountsManage();
	//游戏帮助
	void OnBnClickedGameHelp();
	//问题反馈
	void OnBnClickedQuestionFeedBack();
	//在线客服
	void OnBnClickedContactServise();

	//游戏列表
	void OnBnClickedGameList();
	//比赛专区
	void OnBnClickedTounament();
	//帐号充值
	void OnBnClickedAccountsRecharge();
	//奖品兑换
	void OnBnClickedPrizeExcharge();
	//道具商城
	void OnBnClickedPropsShop();
	//新手任务
	void OnBnClickedNoviceTask();
	
	//背包
	void OnBnClickedBackPack();
	//用户信息
	void OnBnClickedUserInfo();
	//银行
	void OnBnClickedBank();
	//邮件
	void OnBnClickedEmailMessage();
	//返回按钮
	void OnBnClickedRollBack();
	//下一页按钮
	void OnBnClickedNextPage();
	//更新用户信息按钮
	void OnBnClickedUpdateUserInfo();
	//新手指引
	void OnBnClickedNewComerGuide();
	//创建新手指引
	void DoModalNewComerGuide();
	//启动四色球
	bool StartSiSeQiuProgram();
	//启动活动专区
	bool StartActivity(WORD wID);
	/// 房间连接间隔判断
	BOOL IsCanIntervalConnect();

protected:
	//获取当前路径
	void GetCurrentDirectoryPath(CString &szPath);
	//资源路径
	void GetResPath(CString strClipName, CString& strPath);
	//获取配置
	UINT GetCommonConfig(CString strTitle,CString strName, int uiDefault);
	//设置配置
	void SetCommonConfig(CString strTitle,CString strName, CString bIsSound);

public:
	//显示隐藏
	void SetShowHideInfo(WORD dwType);
	//设置失效
	void SetInvalidateRect();
	//更新用户信息
	void UpdateUserInfo();
public:
	// 建立单线程  
	bool createThreads();
	static unsigned __stdcall  thread_game_close(void* pParam)  ;
public:
	HANDLE			    m_hThread;	// 线程句柄
	unsigned int		m_dwThreadID;	// 线程ID

	HANDLE		        m_hEventGameLoadOver[1];	// 事件句柄,关闭游戏
	CString				m_strGameLoadOver;			// 游戏加载完毕
};
