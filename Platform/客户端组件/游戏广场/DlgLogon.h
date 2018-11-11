#pragma once

#include "Stdafx.h"
#include "Resource.h"
#include "KeyDialog.h"
#include "DlgLogonLogo.h"

//////////////////////////////////////////////////////////////////////////

//登录模式
enum enLogonMode
{
	LogonMode_Accounts,			//帐号登录
	LogonMode_UserID,			//I D 登录
};

#define KEY_DOWN			WM_USER + 0x300
//////////////////////////////////////////////////////////////////////////

//登录 TAB 控件
class CTabCtrlLogon : public CTabCtrl
{
	//函数定义
public:
	//构造函数
	CTabCtrlLogon();
	//析够函数
	virtual ~CTabCtrlLogon();

	//消息函数
protected:
	//重画函数	
	afx_msg void OnPaint();

	DECLARE_MESSAGE_MAP()
public:
	//	afx_msg void OnWindowPosChanging(WINDOWPOS* lpwndpos);
};

//////////////////////////////////////////////////////////////////////////

//用户注册
class CDlgRegister : public CSkinDialogEx
{
	friend class CDlgLogon;

	//登录信息
public:
	WORD								m_wFaceID;						//头像标识
	BYTE								m_cbGender;						//用户性别
	TCHAR								m_szSpreader[NAME_LEN];			//电子邮箱
	TCHAR								m_szAccounts[NAME_LEN];			//游戏帐号
	TCHAR								m_szPassword[PASS_LEN];			//游戏密码
	TCHAR								m_szMail[NAME_LEN];				//邮箱帐号
	TCHAR								m_szMailSuffix[NAME_LEN];		//邮箱后缀
	//控件变量
public:
	CImageList							m_ImageList;
	CComboBoxEx							m_FaceSelect;
	CSkinHyperLink						m_LineRegWeb;					//网站注册
	CSkinHyperLink						m_LineMainPage;					//游戏主页
	CSkinHyperLink						m_LinePassWord;					//密码保护
	CRgnButton							m_btLogon;						//登录\注册按钮
	CRgnButton							m_btCancel;						//取消按钮
	CRgnButton							m_btUserDocu;					//用户协议
	CRgnButton							m_btUserSexMan;					//性别—男
	CRgnButton							m_btUserSexWoman;				//性别—女

	CString								m_strOutput;						//用户提示
	CRect								m_rcOutput;
	//函数定义
public:
	//构造函数
	CDlgRegister();
	//析构函数
	virtual ~CDlgRegister();


	//重载函数
protected:
	//控件绑定
	virtual void DoDataExchange(CDataExchange * pDX);
	//初始化函数
	virtual BOOL OnInitDialog();
	//确定函数
	virtual void OnOK();
	//取消消息
	virtual void OnCancel();

	virtual void OnPaint();

	//修改用户性别——男
	afx_msg void OnEnMakeUserSexMan();
	//修改用户性别——女
	afx_msg void OnEnMakeUserSexWoman();
	//查看用户协议
	afx_msg void OnBnClickedUserDocu();
	//激活帐号
	afx_msg void OnEnSetfocusAccouts();
	//激活密码
	afx_msg void OnEnSetfocusPassword();
	//激活确认密码
	afx_msg void OnEnSetfocusPassword2();
	//激活邮箱
	afx_msg void OnEnSetfocusMail();
	//激活邮箱后缀
	afx_msg void OnEnSetfocusMail2();
	//激活推荐人
	afx_msg void OnEnSetfocusSpreader();

	DECLARE_MESSAGE_MAP()
protected:
	void UpdateUserSex();
	BOOL IsOK();
	virtual void PreSubclassWindow();
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);	
};

//////////////////////////////////////////////////////////////////////////

//登录对话框
class CDlgLogon : public CSkinDialogEx
{
	friend class CRoomViewItem;
	friend class CPlazaViewItem;
	friend class CPlazaDlg;

	//登录信息
protected:
	WORD								m_wFaceID;						//头像标识
	BYTE								m_cbGender;						//用户性别
	DWORD								m_dwUserID;						//用户 I D
	TCHAR								m_szSpreader[NAME_LEN];			//推广人名
	TCHAR								m_szAccounts[NAME_LEN];			//游戏帐号
	TCHAR								m_szPassword[PASS_LEN];			//游戏密码
	TCHAR								m_szMail[NAME_LEN*3];	//邮箱

	//位置变量
protected:
	INT									m_nFullWidth;					//全部宽度
	INT									m_nFullHeight;					//全部高度
	bool								m_bNetOption;					//网络设置
	CRect								m_rcNormalFrame;				//框架位置

	//登录模式
protected:
	enLogonMode							m_LogonMode;					//登录模式
	DWORD								m_enRule;						//帐号模式
	//辅助信息
protected:
	bool								m_bRegister;					//注册标志
	bool								m_bChangePassWord;				//改变标志
	bool								m_bRemPassword;					//记住密码标志
	//连接信息
protected:
	CString								m_strLogonServer;				//服务器地址

public:
	CPngImage							m_pngLogon;						//登陆界面(包括注册界面)

	//按钮变量
public:
	CSkinButton							m_btLogon;						//登录按钮
	CSkinButton							m_btCancel;						//取消按钮
	CRgnButton							m_btDelete;						//删除按钮
	CRgnButton							m_btRegister;					//注册按钮
	CRgnButton							m_btNetOption;					//网络按钮
	CRgnButton							m_btGetBackPassword;            //找回密码
	CRgnButton							m_btVisitWeb;					//访问官网
	CRgnButton							m_checkPassword;				//记住密码
	CRgnButton							m_btnGameHelp;					//游戏帮助
	CRgnButton							m_btnGameShop;					//商场充值
	CRgnButton							m_btnGameClinet;				//客户中心
	CRgnButton							m_btnKey;						//软键盘
	CToolTipCtrl						m_ToolTipCtrl;					//提示控件
	CRgnButton							m_btTelecom;					//电信
	CRgnButton							m_btMeshWork;					//网通
	CRgnButton							m_btAuto;						//自动
	CRgnButton							m_btAccountsManage;				//帐号管理
	CRgnButton							m_btGameWeb;					//游戏网站
	CRgnButton							m_btExit;						//退出登录
	//控件变量
public:
	CWebBrowser							m_BrowerAD;						//浏览窗口
	CWebBrowser							m_LogonAdvertisement;			//登录广告
	CWebBrowser							m_LogonNotice;					//登录公告
	CTabCtrlLogon						m_TabLogonMode;					//登录选择

	CSkinHyperLink						m_LineMainPage;					//游戏主页
	CSkinHyperLink						m_LinePassWord;					//密码保护
	CSkinHyperLink						m_LineGetPassWord;				//取回密码

	//代理相关
public:
	CSkinEdit							m_edProxyPort;					//代理端口
	CSkinEdit							m_edProxyServer;				//代理地址
	CSkinEdit							m_edProxyUserName;				//代理用户
	CSkinEdit							m_edProxyPassword;				//代理密码
	CComboBox							m_cmProxyServerType;			//代理类型
	CRgnButton							m_btProxyTest;					//代理测试
	CKeyDialog							m_keydlg;						//软件盘
	bool								m_bIsKeyDlgShow;				//软件盘是否显示
	//编辑控件
protected:
	ComboBoxEx							m_edUserAccounts;				//帐号
	CSkinEdit							m_edUserPassword;				//密码

private:
	CDlgLogonLogo						* m_DlgLogonLogo;				//登录Logo图窗口
	bool								m_bIsLogoCreate;

	//函数定义
public:
	//构造函数
	CDlgLogon();
	//析构函数
	virtual ~CDlgLogon();

	//重载函数
public:
	//控件绑定
	virtual void DoDataExchange(CDataExchange * pDX);
	//消息解释
	virtual BOOL PreTranslateMessage(MSG * pMsg);
	//初始化函数
	virtual BOOL OnInitDialog();

	virtual void OnLButtonDown(UINT nFlags, CPoint point);

public:
	//确定函数
	virtual void OnOK();
	//取消消息
	virtual void OnCancel();
	//退出登录
	void OnBnClickedOnExit();
	/// 控件禁止
	void EnableControlWindow(BOOL bEnable = 1);

public:
	void ExitLogon();

	//功能函数
public:
	//获取服务器
	LPCTSTR GetLogonServer() { return m_strLogonServer; }
	//发送登录包
	bool SendLogonPacket(ITCPSocket * pIClientSocke);
	//登录成功处理
	bool OnLogonSuccess();

	//内部函数
private:
	//加载服务器
	void LoadLogonServer();
	//读取帐号
	void LoadAccountsInfo();
	//代理信息
	void LoadProxyServerInfo();
	//效验输入
	bool CheckLogonInput(bool bShowError);

	//辅助函数
private:
	//更新密码
	void UpdateUserPassWord(DWORD dwUserDBID);
	//更新网络标志
	void UpdateNetWorkTag(DWORD dwUserDBID);
	//更新播放背景音乐标识
	void UpdateBackMusicTag(DWORD dwUserDBID);
	//代理模式
	void SwitchNetOption(bool bNetOption);
	//查找字符
	int ComboBoxFindString(CComboBox * pComboBox, LPCTSTR pszFindString);
	//获取代理
	void GetProxyInfo(enProxyServerType &ProxyServerType, tagProxyServerInfo &ProxyServerInfo);
	//代理判断
	bool EnableProxy() { return m_bNetOption;}
	//获取当前路径
	void GetCurrentDirectoryPath(CString &szResourcePath);
	//图平路径
	void GetImagePath(CString& strRetValue);
	//
	void GetResPath(CString strClipName, CString &strResPath);
public:
	BOOL SetImageRgn(CPngImage * pImage);
	//退出
	LRESULT OnExitMessage(WPARAM wParam, LPARAM lParam);
	//Inno 安装脚本发来的消息 选择游戏
	LRESULT OnCallInnoIssMsgSelectGame(WPARAM wParam, LPARAM lParam);

	void UpdateKeyPos();
	//消息函数
public:
	//定时器
	afx_msg void OnTimer(UINT nIDEvent);
	//注册帐号
	afx_msg void OnRegisterAccounts();
	//删除用户
	afx_msg void OnDeleteAccounts();
	//网络设置
	afx_msg void OnBnClickedNetOption();
	//代理测试
	afx_msg void OnBnClickedProxyTest();
	//密码改变
	afx_msg void OnEnChangePassword();
	//记住密码
	afx_msg void OnBnClickCheckPassword();
	//游戏帮助
	afx_msg void OnBnClickGameHelp();
	//游戏商场
	afx_msg void OnBnClickGameShop();
	//客户中心
	afx_msg void OnBnClickGameClient();
	//软键盘
	afx_msg void OnBnClickKey();
	//重画消息
	afx_msg void OnPaint();
	//边框
	afx_msg void OnNcPaint();

	afx_msg void OnBnClickedGetBackPassword();

	afx_msg void OnBnClickedVisitWeb();
	//帐号管理
	afx_msg void OnBnClickedAccountsManage();
	//游戏官网
	afx_msg void OnBnClickedGameWeb();

	DECLARE_MESSAGE_MAP()

	afx_msg void OnDestroy();

	//电信
	afx_msg void OnEnTelecomMsg();
	//网通
	afx_msg void OnEnMeshWorkMsg();
	/// 自动
	afx_msg void OnEnAutoMsg();
	/// 设置检查线路结果
	void SetCheckLineResult(BYTE bNetWorkTag);

	virtual  LRESULT OnKeyDownDlg(WPARAM wParam, LPARAM lParam);
	//	afx_msg void OnMoving(UINT fwSide, LPRECT pRect);
	//	afx_msg void OnMove(int x, int y);
protected:
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnMove(int x, int y);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnWindowPosChanging(WINDOWPOS* lpwndpos);

	//检测弹出的窗口
	void OnPopRegisterDlg();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnWindowPosChanged(WINDOWPOS* lpwndpos);

public:
	//隐藏键盘
	void SetKeyHide();
	//刷新控件
	void InvalidateRectControl();
};

//////////////////////////////////////////////////////////////////////////

