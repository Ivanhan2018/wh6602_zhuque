#pragma once

#include "Stdafx.h"
#include <string>
#include "IGamePlaza.h"
#include "TableFrame.h"
#include "UserManager.h"
#include "FindUserDlg.h"
#include "UserListView.h"
#include "SystemOption.h"
#include "ShortMessage.h"
#include "ManagerClass.h"
#include "ServerItemView.h"
#include "GameListManager.h"
#include "RoomViewAffiche.h"
#include "ConfirmGold.h"
#include "ConfirmGrade.h"

//////////////////////////////////////////////////////////////////////////

//房间服务状态
enum enServiceStatus
{
	ServiceStatus_Null,					//没有状态
	ServiceStatus_Connecting,			//连接状态
	ServiceStatus_EfficacyUser,			//效验用户
	ServiceStatus_RecvConfigInfo,		//接收配置
	ServiceStatus_RecvRoomInfo,			//接收信息
	ServiceStatus_Serviceing,			//服务状态
	ServiceStatus_NetShutDown,			//网络中断
};

//查找结构
struct tagFindTable
{
	bool								bOneNull;							//一个空位
	bool								bTwoNull;							//两个空位
	bool								bAllNull;							//全空位置
	bool								bNotFull;							//不全满位
	bool								bFilterPass;						//过滤密码
	WORD								wBeginTableID;						//开始索引
	WORD								wResultTableID;						//结果桌子
	WORD								wResultChairID;						//结果椅子
};

//游戏框架状态信息
struct tagGameFrameStatusInfo
{
	bool				bIsOpen;	//是否打开
	BYTE				byStatus;	//状态.1:send; 2:recv
};

//类型定义
typedef CArrayTemplate<tagPropertyInfo> CPropertyAttributeArrary;
typedef CArrayTemplate<tagFlowerInfo> CFlowerAttributeArrary;
//////////////////////////////////////////////////////////////////////////

//消息定义
#define	WM_UPDATE_FACE					WM_USER+200							//更新头像


//////////////////////////////////////////////////////////////////////////

//游戏房间
class CRoomViewItem : public CDialog,
	public IRoomViewItem,
	public ITCPSocketSink,
	public IPurchaseInfo,
	public ITableFrameSink,
	public IUserManagerSink,
	public ISplitterSink,
	public IChannelMessageSink
{
	friend class CFindUserDlg;
	friend class CShortMessage;

	//应用变量
protected:
	CListServer	*					m_pListServer;						//房间信息
	tagOptionBuffer					m_OptionBuffer;						//配置信息
	CMD_GR_ColumnInfo				m_ListColumnInfo;					//列表信息

	//房间配置
protected:
	WORD							m_wGameGenre;						//游戏类型
	DWORD							m_dwVideoAddr;						//视频地址
	BYTE							m_cbHideUserInfo;					//隐藏信息

private:
	bool							m_bIsDestroyStatus;					//是否已经销毁等待框
	WORD							m_wAnimAction;						//动画帧数
	CRect							m_rcAnimAction;						//动画区域
	CPngImage						m_ImgAnimAction;					//动画

	CPngImage						m_ImgClose;							//关闭按钮图片
	WORD							m_wCloseState;						//关闭按钮状态
	CRect							m_rcCloseRect;						//关闭按钮位置
	CConfirmGold					m_ConfirmGold;						//金币不足框
	CConfirmGrade					m_ConfirmGrade;						//等级限制
	//Author<xf>;DateTime<2012-03-01>;Description<屏蔽>
	//bool							m_bMasterRight;						//是否管理权限(False:不是; Ture:是) Author<cxf>;DateTime<2012-02-24>

	//辅助变量
protected:
	bool							m_bRectifyHor;						//调整标志
	bool							m_bRectifyVor;						//调整标志
	bool							m_bInitDialog;						//初始标志
	bool							m_bHideUserList;					//隐藏标志
	DWORD							m_dwLimitChatTimes;					//限制聊时
	WORD							m_wReqTableID;						//请求桌子
	WORD							m_wReqChairID;						//请求位置
	WORD							m_wFindTableID;						//查找位置
	DWORD							m_dwMenuUserID;						//菜单用户
	INT_PTR							m_nListIndex;						//服务器索引
	int								m_QueueNum;							//排队人数
	bool							m_bLogonFull;						//登录人满
	CRect							m_rcQueue;							//排队人数显示区(516,228,516+197,228+28);

	//cxfINT32                           m_dwWinGemsToday;					//今日赢取     
	//CPoint                           m_ptOrder;							//用户的名次
	//CPoint                           m_ptWinGemsToday;					//今日赢取     

	CPngImage						m_pngNumber;	//数字
	CPngImage						m_pngText1;		//"位"
	CPngImage						m_pngText2;		//"宝石"
	CPngImage						m_pngImageHou;	//"后"
	CPngImage						m_pngImageCurSort;	//当前排名
	CPngImage						m_pngImageDayWin;	//今日赢取
	CPngImage						m_pngGameICO;		//游戏图标
	CPngImage						m_ImageBack;
	//CPngImage						m_ImageBackGM;

	bool							m_bQueueState;		//排除状态. true:自动排队; false:取消排队.
	BYTE							m_btExitTag;		//退出标志
	bool							m_bRequestServerExit;	//请求服务退出

	//配置变量
protected:
	CGameOption *					m_pGameOption;						//游戏设置
	CServerOption *					m_pServerOption;					//房间设置

	//管理变量
protected:
	IUserItem *						m_pMeUserItem;						//自己信息
	enServiceStatus					m_ServiceStatus;					//服务状态
	CClientUserManager				m_ClientUserManager;				//用户管理
	CDC								m_memdc;

	//进程信息
protected:
	HWND							m_hWndChannel;						//信道窗口
	HANDLE							m_hShareMemory;						//共享内存
	TCHAR							m_szShareName[64];					//共享名字
	tagShareMemory *				m_pShareMemory;						//共享内存
	PROCESS_INFORMATION				m_GameProcessInfo;					//进程信息
	tagGameFrameStatusInfo			m_tagGameFrameStatus;				//游戏框架状态

	//聊天变量
protected:
	DWORD							m_dwChatUserID;						//聊天用户
	TCHAR							m_szChatName[NAME_LEN];				//聊天用户


	//游戏公告走马灯
protected:
	//cxf 11-26 CRoomViewAffiche				*m_RoomViewAffiche;					//公告信息窗口
	std::string						m_strSaveGameAffiche;				//保存游戏公告
	CFont							m_fontGameTitleInfo;				//游戏标题字体

	//系统控件
public:
	// 	CRgnButton		m_btnReLogon;				//重新登录 IDC_BT_RE_LOGON
	// 	CRgnButton		m_btnReturn;				//返回     IDC_BT_RETURN
	// 	CRgnButton		m_btnSet;					//设置	   IDC_BT_SET
	// 	CRgnButton		m_btnClose;					//关闭     IDC_BT_CLOSE
	// 	CRgnButton		m_btnMinsize;				//最小化   IDCMINSIZE

	// 	CRgnButton		m_btnDepository;			//藏宝阁
	// 	CRgnButton		m_btnFriend;				//好友
	// 	CRgnButton		m_btnShop;					//商城
	// 	CRgnButton		m_btnTask;					//任务
	// 	CRgnButton		m_btnTool;					//道具

	CRgnButton		m_btnPgDn;					//下一页 IDC_PGDWON
	CRgnButton		m_btnPgUp;					//上一页 IDC_PGUp

	//聊天控件
public:
	CComboBox						m_ChatInput;						//聊天输入
	//CSkinButton						m_btPhrase;							//短语按钮
	CSkinButton						m_btColorSet;						//颜色设置
	CSkinButton						m_btCleanScreen;					//清屏按钮
	//CSkinButton						m_btBugle;							//喇叭按钮
	//cxf 1108 CRgnButton						m_AutoQueue;						//自动排队
	//	CRgnButton						m_btnDespokenTable;					//预订桌子
	CComboBox						m_ChatObject;						//聊天对象
	CRgnButton						m_btExpression;						//表情按钮
	CRgnButton						m_btSendChat;						//发送按钮
	CSkinRichEdit					m_ChatMessage;						//聊天组件
	CSkinRichEdit					m_editInput;						//输入内容
	CExpression						m_Expression;						//表情窗口

	//功能控件
public:
	//CSkinButton						m_btBank;							//银行按钮
	//CSkinButton						m_btAutoSit;						//加入按钮
	//CSkinButton						m_btFindUser;						//查找按钮
	//CSkinButton						m_btQuitRoom;						//退出按钮

	//用户控件
public:
	//CWebBrowser						m_BrowerAD;							//浏览窗口
	CTableFrame						m_TableFrame;						//桌子框架
	//CUserListView					m_UserListView;						//用户列表
	//CGameListManager *				m_gameListManager;					//房间列表管理 Author<cxf>;DateTime<2012-02-23>;Description<打开屏蔽代码>

	//管理控件
public:
	CManagerServer					m_ManagerServer;					//房间管理
	CManagerMessage					m_ManagerMessage;					//系统消息
	CManagerUserRight				m_ManagerUserRight;					//权限管理
	CManagerSendWarning				m_ManagerSendWarning;				//警告管理

	//环绕信息
protected:
	tagEncircleInfo					m_ImageInfoTable;					//桌子信息
	tagEncircleInfo					m_ImageInfoList;					//列表信息
	tagEncircleInfo					m_ImageInfoChat;					//聊框信息

	//资源变量
protected:
	tagEncircleImage				m_EncircleTable;					//桌子资源
	tagEncircleImage				m_EncircleList;						//列表资源
	tagEncircleImage				m_EncircleChat;						//聊框资源

	//数据缓冲
protected:
	WORD							m_wPacketTableID;					//缓冲位置
	WORD							m_wPacketChairID;					//缓冲位置
	CDataQueue						m_PacketDataStorage;				//数据缓冲

	//其他控件
public:
	CToolTipCtrl					m_ToolTipCtrl;						//提示控件
	// 	CSkinSplitter					m_HorSplitter;						//拆分窗口
	// 	CSkinSplitter					m_VorSplitter;						//拆分窗口
	CIPCRecvCopyData				m_IPCRecvCopyData;					//信道组件
	CTCPSocketHelper				m_ClientSocket;						//网络连接
	CMessageProxyHelper				m_MessageProxyHelper;				//消息代理
	//	CPropertyBar					m_PropertyBar;						//道具控件

	// 	//扩展变量
	// public:
	// 	CPropertyAttributeArrary		m_PropertyAttributeArrary;			//道具信息
	// 	CFlowerAttributeArrary			m_FlowerAttributeArrary;			//鲜花信息

	enum 
	{
		GAME_LOAD_OVER_COUNT = 2,		// 游戏加载完毕句柄的数量
	};

	HANDLE			m_hThread;	// 线程句柄，游戏加载完毕
	unsigned int		m_dwThreadID;	// 线程ID，游戏加载完毕	

	HANDLE		m_hEventGameLoadOver[GAME_LOAD_OVER_COUNT];	// 事件句柄
	HANDLE&   m_hGameLoadOver;		// 事件句柄，游戏加载完毕
	HANDLE&   m_hClosePromtWndr;	// 事件句柄，关闭提示框
	
	CString				m_strGameLoadOver;			// 游戏加载完毕

	//任务相关
public:
	//CMD_GF_TASK_tagTask				m_tagTask;							//任务
	//函数定义
public:
	//构造函数
	CRoomViewItem();
	//析构函数
	virtual ~CRoomViewItem();

	//重载函数 
protected:
	//控件绑定
	virtual void DoDataExchange(CDataExchange * pDX);
	//初始化函数
	virtual BOOL OnInitDialog();
	//确定函数
	virtual void OnOK();
	//取消函数
	virtual void OnCancel() { return; };
	//消息过虑
	virtual BOOL PreTranslateMessage(MSG * pMsg);
	//菜单命令
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

	//基础接口
public:
	//释放对象
	virtual void __cdecl Release() { delete this; }
	//接口查询
	virtual void * __cdecl QueryInterface(const IID & Guid, DWORD dwQueryVer);

	//道具接口 
public:
	//购买场所
	virtual BYTE __cdecl GetLocation(){return LOCATION_PLAZA_ROOM;}
	//房间类型
	virtual WORD __cdecl GetGameGenre();
	//枚举玩家
	virtual tagUserData * __cdecl EnumLookonUser(WORD wIndex);
	//查找用户
	virtual tagUserData * __cdecl SearchUserItem(DWORD dwUserID);
	//查找用户
	virtual tagUserData * __cdecl SearchUserItem(LPCTSTR pszUserName);
	//获取玩家
	virtual const tagUserData * __cdecl GetUserInfo(WORD wChairID);
	//获取自己
	virtual const tagUserData * __cdecl GetMeUserInfo();
	//发送信息
	virtual void __cdecl SendData(WORD wMainCmdID, WORD wSubCmdID, void * pData, WORD wDataSize);

	//视图接口
public:
	//老板事件
	void NotifyBossCome(bool bBossCome);
	//获取句柄
	virtual HWND __cdecl GetWindowHandle() { return GetSafeHwnd(); }

	//房间接口
public:
	//关闭询问
	virtual bool __cdecl QueryCloseRoom();
	//关闭房间
	virtual void __cdecl CloseRoomViewItem();
	//类型信息
	virtual tagGameKind * __cdecl GetKindInfo();
	//房间信息
	virtual tagGameServer * __cdecl GetServerInfo();

	//拆分条接口
public:
	//按钮消息
	virtual bool __cdecl OnSplitterButton(CSkinSplitter * pSkinSplitter, CSkinButton * pSplitterButton);
	//拆分条消息
	virtual bool __cdecl OnSplitterEvent(CSkinSplitter * pSkinSplitter, UINT uSplitID, int nXPos, int nYPos);

	//网络事件
public:
	//连接事件
	virtual bool __cdecl OnEventTCPSocketLink(WORD wSocketID, INT nErrorCode);
	//关闭事件
	virtual bool __cdecl OnEventTCPSocketShut(WORD wSocketID, BYTE cbShutReason);
	//读取事件
	virtual bool __cdecl OnEventTCPSocketRead(WORD wSocketID, CMD_Command Command, VOID * pData, WORD wDataSize);

	//信道接口
public:
	//信道数据处理
	virtual bool __cdecl OnChannelMessage(const IPC_Head * pHead, const void * pIPCBuffer, WORD wDataSize, HWND hWndSend);

	//桌子接口
public:
	//鼠标左键按下
	virtual void __cdecl OnLButtonHitTable(WORD wTableID, WORD wChairID, bool bMirror);
	//鼠标右键按下
	virtual void __cdecl OnRButtonHitTable(WORD wTableID, WORD wChairID, bool bMirror);
	//鼠标双击
	virtual void __cdecl OnDoubleHitTable(WORD wTableID, bool bMirror);

	//用户接口
public:
	//用户激活
	virtual void __cdecl OnUserItemAcitve(IUserItem * pIUserItem);
	//用户更新
	virtual void __cdecl OnUserItemUpdate(IUserItem * pIUserItem);
	//用户删除
	virtual void __cdecl OnUserItemDelete(IUserItem * pIUserItem);

	//网络主命令
protected:
	//登录消息
	bool OnSocketMainLogon(CMD_Command Command, void * pBuffer, WORD wDataSize);
	//用户消息
	bool OnSocketMainUser(CMD_Command Command, void * pBuffer, WORD wDataSize);
	//配置消息
	bool OnSocketMainInfo(CMD_Command Command, void * pBuffer, WORD wDataSize);
	//状态消息
	bool OnSocketMainStatus(CMD_Command Command, void * pBuffer, WORD wDataSize);
	//系统消息
	bool OnSocketMainSystem(CMD_Command Command, void * pBuffer, WORD wDataSize);
	//房间消息
	bool OnSocketMainServerInfo(CMD_Command Command, void * pBuffer, WORD wDataSize);
	//游戏消息
	bool OnSocketMainGameFrame(CMD_Command Command, void * pBuffer, WORD wDataSize);
	//
	//	bool OnSocketTask(CMD_Command Command, void * pBuffer, WORD wDataSize);
	//网络子命令
protected:
	//用户进入
	bool OnSocketSubUserCome(CMD_Command Command, void * pBuffer, WORD wDataSize);
	//用户状态
	bool OnSocketSubStatus(CMD_Command Command, void * pBuffer, WORD wDataSize);
	//用户分数
	bool OnSocketSubScore(CMD_Command Command, void * pBuffer, WORD wDataSize);
	//用户权限
	bool OnSocketSubRight(CMD_Command Command, void * pBuffer, WORD wDataSize);
	//会员等级
	bool OnSocketSubMemberOrder(CMD_Command Command, void * pBuffer, WORD wDataSize);
	//坐下失败
	bool OnSocketSubSitFailed(CMD_Command Command, void * pBuffer, WORD wDataSize);
	//用户聊天
	bool OnSocketSubChat(CMD_Command Command, void * pBuffer, WORD wDataSize);
	//用户私语
	bool OnSocketSubWisper(CMD_Command Command, void * pBuffer, WORD wDataSize);
	//邀请玩家
	bool OnSocketSubUserInvite(CMD_Command Command, void * pBuffer, WORD wDataSize);
	//更新排队人数
	bool OnSocketSubQueueCount(CMD_Command Command, void * pBuffer, WORD wDataSize);
	//自动排队失败 SUB_GR_USER_QUEUE_FAIL:
	bool OnSocketSubQueueFail(CMD_Command Command, void * pBuffer, WORD wDataSize);


	//IPC 事件处理
protected:
	//IPC 内核命令
	bool OnIPCKernel(const IPC_Head * pHead, const void * pIPCBuffer, WORD wDataSize, HWND hWndSend);
	//IPC 发包命令
	bool OnIPCSocket(const IPC_Head * pHead, const void * pIPCBuffer, WORD wDataSize, HWND hWndSend);

	//控制函数
public:
	//初始化房间
	bool InitRoomViewItem(CListServer * pListServer);
	//连接服务器
	bool ConnectGameServer();
	//发送规则
	bool SendUserRule();
	//私语信息
	bool SendUserWhisper(IUserItem * pIUserItem);
	//用户菜单
	bool ShowUserInfoMenu(IUserItem * pIUserItem, CPoint Point);
	//更新道具消息
	void UpdateUserDJInfo(int iDJID, bool bIsRoom=true);
	//发送请求关闭房间消息
	void SendRequestCloseRoomMsg(BYTE btTag=0);
	//清空句柄
	void ClearMemDC();

	//功能函数
public:
	//资源句柄
	HINSTANCE GetResInstanceHandle();
	//获取资源
	tagServerViewImage GetServerViewRes();
	//更新资源
	bool UpdateSkinResource();
	//游戏配置
	CGameOption * GetGameOption() { return m_pGameOption; }
	//房间配置
	CServerOption * GetServerOption() { return m_pServerOption; }
	//房间人数
	bool GetLogonFull(){return m_bLogonFull;};
	/// 权限判断
	bool IsAdministrationHavePurview(int iGrade=0);
	/// 设置Ctrl + F快捷键
	void SetCtrolAddF();

	//网络命令
protected:
	//发送登录命令
	bool SendLogonPacket();
	//发送设置命令
	bool SendOptionsPacket();
	//发送起来命令
	bool SendStandUpPacket();
	//发送强退命令
	bool SendLeftGamePacket();
	//发送房间规则
	bool SendUserRulePacket();
	//发送旁观命令
	bool SendLookonPacket(WORD wTableID, WORD wChairID, LPCTSTR pszTablePass);
	//发送坐下命令
	bool SendSitDownPacket(WORD wTableID, WORD wChairID, LPCTSTR pszTablePass);
	//发送聊天命令
	bool SendChatPacket(DWORD dwTargetUserID, LPCTSTR pszChatMessage, COLORREF crFontColor);
	//发送取消排除消息 MDM_GP_USER  SUB_GR_USER_CANCEL
	bool SendCancelQueue();
	//动作处理
protected:
	//旁观动作
	bool PerformLookonAction(WORD wTableID, WORD wChairID);
	//坐下动作
	bool PerformSitDownAction(WORD wTableID, WORD wChairID);

	//IPC 处理
protected:
	//发送游戏信息
	bool IPCSendGameInfo(CIPCSendCopyData * pSendCopyData);
	//发送用户信息
	bool IPCSendTableUsers(CIPCSendCopyData * pSendCopyData);

	//内部函数
protected:
	//启动游戏
	int StartGameClient();
	//关闭游戏
	void CloseGameClient();
	//发送用户
	bool SendTableUser(IUserItem * pIUserItem, CIPCSendCopyData * pSendCopyData);
	//发送消息
	bool SendProcessData(WORD wMainCmdID, WORD wSubCmdID, void * pBuffer, WORD wDataSize);

	//辅助函数
protected:
	//更新标题
	void UpdateTitleText();
	//绘画列表
	void DrawUserFrame(CDC * pDC);
	//绘画聊天
	void DrawChatFrame(CDC * pDC);
	//绘画桌子
	void DrawTableFrame(CDC * pDC);
	//左视图区
	//void DrawLeftViewFrame(CDC * pDC);
	//聊天对象
	bool SetChatObject(IUserItem * pIUserItem);
	//查找桌子
	bool FindGameTable(tagFindTable & FindInfo);
	//调整控件
	void RectifyControl(int nWidth, int nHeight);
	//显示消息
	int ShowMessageBox(LPCTSTR pszMessage, UINT nType);
	//能否坐下
	bool CanSitDownTable(WORD wTableID, WORD wChairID, bool bLookon, bool bShowError);
	//发送公告信息
	void SendToGameFrameAfficheInfo();
	//获取当前路径
	void GetCurrentDirectoryPath(CString &szPath);
	//大厅路径
	void GetImagePlazaPath(CString& strRetValue);
	//关闭桌子列表
	void OnTableClose();

protected:
	//计算数字的位数
	WORD GetNumsBitNums(ULONG ulNums);
	//绘制当前排名
	void DrawCurSort(DWORD wSort);
	//绘制今日赢取
	void DrawDayWin(DWORD wWinNums);

	//消息映射
protected:
	//绘画消息
	afx_msg void OnPaint();

	CPoint DrawNumber(CDC * pDC, CPoint pt, LONG n);
	//绘画背景
	afx_msg BOOL OnEraseBkgnd(CDC * pDC);
	//位置消息
	afx_msg void OnSize(UINT nType, int cx, int cy);

	afx_msg void OnDestroy();

	//按钮消息
protected:
	//发送按钮
	afx_msg void OnBnClickedSendChat();
	//表情按钮
	afx_msg void OnBnClickedExpression();
	//颜色按钮
	afx_msg void OnBnClickedColorSet();
	//清屏按钮
	afx_msg void OnBnClickedCleanScreen();
	//喇叭按钮
	//afx_msg void OnBnClickedBugle();
	//显示消息
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	// 设子新窗口位置和大小
	afx_msg void OnWindowPosChanging(WINDOWPOS* lpwndpos);
	// 	//双击户列
	// 	afx_msg void OnNMDblclkUserList(NMHDR * pNMHDR, LRESULT * pResult);
	// 	//右键列表
	// 	afx_msg void OnNMRclickUserList(NMHDR * pNMHDR, LRESULT * pResult);
	// 	//点键列表
	// 	afx_msg void OnNMclickUserList(NMHDR * pNMHDR, LRESULT * pResult);
	//更新头像
	LRESULT OnUpdateFace(WPARAM wParam, LPARAM lParam);
	//IPC 消息
	afx_msg BOOL OnCopyData(CWnd* pWnd, COPYDATASTRUCT * pCopyDataStruct);
	//表情消息
	LRESULT OnHitExpression(WPARAM wParam, LPARAM lParam);
	/// 闪动桌子
	LRESULT OnFlashChair(WPARAM wParam, LPARAM lParam);

	//功能按钮
protected:
	//银行按钮
	//afx_msg void OnBnClickedBank();
	//加入按钮
	//afx_msg void OnBnClickedAutoSit();
	//查找按钮
	afx_msg void OnBnClickedFindUser();
	//退出按钮
	//afx_msg void OnBnClickedQuitRoom(); 

	//上一页
	afx_msg void OnBnUp();
	//下一页
	afx_msg void OnBnDwon();
	//自动排队
	afx_msg void OnAutoQueue();

	//CPtrArray						m_ShortMessage;						//信息窗口
	CFindUserDlg					* m_pFindUserDlg;					//好友列表

	//激活信息窗口
	CShortMessage * ActiveShortWnd(long int dwUserID, IUserItem * pUserItem, bool bCreate);


	afx_msg void OnLButtonDown( UINT nFlags, CPoint point );

	//鼠标消息
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint Point);
	//鼠标消息
	afx_msg void OnLButtonUp(UINT nFlags, CPoint Point);
	//鼠标消息
	afx_msg void OnRButtonDown(UINT nFlags, CPoint Point);
	//鼠标消息
	afx_msg void OnRButtonUp(UINT nFlags, CPoint Point);
	//光标消息
	afx_msg BOOL OnSetCursor(CWnd * pWnd, UINT nHitTest, UINT uMessage);
	//鼠标移动消息
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);

	DECLARE_MESSAGE_MAP()

public:
	//时间消息
	afx_msg void OnTimer(UINT nIDEvent);
	//返回按钮
	afx_msg void OnBnReturn();
	//关闭按钮
	afx_msg void OnBnClose();
	//重新登陆
	afx_msg void OnBnClickedReLogon();

	//取消连接
	afx_msg void OnCommandCancelConnect();

	// add by HouGuoJiang 2011-11-25
	// 线程工作函数
	static unsigned __stdcall  thread_game_load_over(void* pParam)  ;	

	// 建立单线程
	bool createThreads();
	virtual BOOL DestroyWindow();
};

//////////////////////////////////////////////////////////////////////////
