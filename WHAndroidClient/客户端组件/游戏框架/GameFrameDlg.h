#ifndef GAME_FRAME_DLG_HEAD_FILE
#define GAME_FRAME_DLG_HEAD_FILE

#pragma once

#include "GameFrame.h"
#include "GameFrameView.h"
#include <vector>
#include <string>

//类说明
class CGameFrameControl;
class CClientKernelSink;
class CSendMsgDlg;
class CScoreView;
class CGameTaskDialog;
class CShowTimeDJList;
class CGameClientDJInfo;
class CGameGiveDJInfo;
class CUserAction;
class CShowUserActionInfo;
class CUserInfoDlg;
class CGameAffiche;
class CShowDJInfo;
class CLanguageList;
class CTaskFinishFlash;
class CGameFrameBorder;
class CNewGiftFlashDlg;


using namespace std;


//////////////////////////////////////////////////////////////////////////

//游戏主窗口
class GAME_FRAME_CLASS CGameFrameDlg : public CDialog
{
	DECLARE_DYNAMIC(CGameFrameDlg)

	//友元定义
	friend class CClientKernelSink;
	friend class CGameFrameControl;
	friend class CUserDaoJu;

	//界面状态
private:
	bool							m_bRectify;							//调整标志
	bool							m_bHideList;						//隐藏标志
	CRect							m_rcNormalSize;						//正常位置

	//界面辅助
private:
	bool							m_bReset;							//是否重置
	bool							m_bGameSceen;						//游戏场景
	bool							m_bInitDialog;						//初始标志
	bool							m_bIsMouseDown;						//鼠标按下

	//配置变量
private:
	bool							m_bAllowSound;						//允许声音

//	//游戏公告走马灯
//private:
//	bool							m_bIsDisableGameMsg;				//是否激活游戏公告
//	CRect							m_rcGameMsgInfo;					//字体绘制区域
//	std::vector<string>				m_vecGameMsg;						//游戏公告
//	int								m_iCountVecIndex;					//当前公告序号

	//控件变量
protected:
	CSkinButton						m_btControl;						//控制按钮
	CSkinSplitter					m_VorSplitter;						//拆分控件
	CGameFrameView					* m_pGameFrameView;					//视图指针
	CGameFrameControl				* m_pGameFrameControl;				//控制视图
	CSendMsgDlg						* m_pSendMsgDlg;					//发送聊天消息
	CGameClientDJInfo				* m_pUserDJInfo;					//玩家道具列表
	CUserInfoDlg					* m_pUserInfoDlg;					//用户信息框

	//方言
protected:
	std::string						m_strLanguage;						//方言

	//内核变量
private:
	CClientKernelSink				* m_pKernelSink;					//钩子接口
	CUserFaceResHelper				m_UserFaceResHelper;				//头像组件
	CClientKernelHelper				m_ClientKernelHelper;				//游戏逻辑
	CMessageProxyHelper				m_MessageProxyHelper;				//聊天代理

	HANDLE							m_hEventGameLoadOver;				// 句柄，游戏加载完毕
	CString							m_strGameLoadOver;					// 游戏加载完毕

private:
	//用户道具
	CUserDaoJu						* m_pGameUserDaoJu;					//游戏用户道具
	CScoreView						* m_pGameScoreView;					//结算视图指针
	CGameTaskDialog					* m_pGameTaskDlg;					//游戏任务
	CShowTimeDJList					* m_pShowTimeDJList;				//显示时间道具列表
	CShowDJInfo						* m_pShowDJInfo;					//显示道具信息
	CGameGiveDJInfo					* m_pGameGiveDJInfo;				//赠送道具信息
	CUserAction						* m_pGameUserAction;				//游戏用户动作
	CShowUserActionInfo				* m_pShowUserActionInfo;			//显示用户动作信息


private:
	CGameFrameBorder				* m_pGameFrameBorder;				//游戏框架边框
	CGameAffiche					* m_pGameAffiche;					//喜报系统
	CLanguageList					* m_pLanguageList;					//语音列表
	CTaskFinishFlash				* m_pTaskFinishFlash;				//任务完成动画
	CNewGiftFlashDlg				* m_pNewGiftFlash;					//新手礼包动画	Author<cxf>; DateTime<2012-01-17>

	//函数定义
public:
	//构造函数
	CGameFrameDlg(CGameFrameView * pGameFrameView, CScoreView* pScoreView=NULL);
	//析构函数
	virtual ~CGameFrameDlg();

	//拆分条接口
public:
	//按钮消息
	virtual bool __cdecl OnSplitterButton(CSkinSplitter * pSkinSplitter, CSkinButton * pSplitterButton);
	//拆分条消息
	virtual bool __cdecl OnSplitterEvent(CSkinSplitter * pSkinSplitter, UINT uSplitID, int nXPos, int nYPos);

	//常规继承
private:
	//初始函数
	virtual bool InitGameFrame()=NULL;
	//重置框架
	virtual void ResetGameFrame()=NULL;
	//游戏设置
	virtual void OnGameOptionSet()=NULL;
	//点击继续游戏
	virtual void OnEventContinueGame()=NULL;

	//系统事件
private:
	//时间消息
	virtual bool OnTimerMessage(WORD wChairID, UINT nElapse, UINT nTimerID)=NULL;
	//旁观状态
	virtual void OnLookonChanged(bool bLookonUser, const void * pBuffer, WORD wDataSize)=NULL;
	//网络消息
	virtual bool OnGameMessage(WORD wSubCmdID, const void * pBuffer, WORD wDataSize)=NULL;
	//游戏场景
	virtual bool OnGameSceneMessage(BYTE cbGameStatus, bool bLookonOther, const void * pBuffer, WORD wDataSize)=NULL;

	//特殊继承
private:
	//调整界面
	virtual void RectifyControl(int nWidth, int nHeight);
	//框架消息
	virtual bool OnFrameMessage(WORD wSubCmdID, const void * pBuffer, WORD wDataSize);

	//用户事件
public:
	//用户进入
	virtual void __cdecl OnEventUserEnter(tagUserData * pUserData, WORD wChairID, bool bLookonUser);
	//用户离开
	virtual void __cdecl OnEventUserLeave(tagUserData * pUserData, WORD wChairID, bool bLookonUser);
	//用户积分
	virtual void __cdecl OnEventUserScore(tagUserData * pUserData, WORD wChairID, bool bLookonUser);
	//用户状态
	virtual void __cdecl OnEventUserStatus(tagUserData * pUserData, WORD wChairID, bool bLookonUser);

	//礼物事件
public:
	//初始道具
	virtual void __cdecl OnEventInitProperty(tagPropertyInfo *pPropertyInfo, int nInfoCount);
	//初始鲜花
	virtual void __cdecl OnEventInitFlower(tagFlowerInfo *pFlowerInfo, int nInfoCount);
	//鲜花消息
	virtual void __cdecl OnEventFlower(const tagUserData * pSendUserData, const tagUserData * pRecvUserData, UINT uFlowerID, UINT uFlowerEffectID);
	//玩家剩余道具
	virtual void __cdecl OnEventUserHaveDJInfo(CMD_GF_ResidualProperty *pUserDJInfo);
	//道具操作信息
	virtual void __cdecl OnEventUserOperationDJInfo(BYTE byTag, void * pBuffer);
	//道具赠送消息
	virtual void __cdecl OnEventGameDJInfo(const char* pInfo);
	//用户信息
	virtual void __cdecl OnEventUserMD5PWMsg(char* chUserData);
	//公告信息
	virtual void __cdecl OnEventGameAfficheInfo(const char* pData, WORD wDataSize);
	//用户动作
	virtual void __cdecl OnEventUserActionInfo(BYTE btSender, BYTE btRecver, BYTE btTag);
	//用户主意
	virtual void __cdecl OnEventUserLanguageInfo(BYTE btSender, DWORD dwIndex);
	//新手礼包	Author<cxf>; DateTime<2012-01-16>
	virtual void __cdecl OnEventNewGift(CMD_GF_NEW_GIFT * pNewGift);
	//在线有礼-计时
	virtual void __cdecl OnEventOnlineKeepTime(CMD_GF_ONLINE * pOnline);

//OSC update
	//任务消息
public:
	//接收新任务
	virtual void __cdecl OnEventTaskAccept(const CMD_GF_TASK_tagTask* pTaskAccept);
	//单个任务完成
	virtual void __cdecl OnEventTaskOnceFinish(const CMD_GF_TASK_tagTaskFinish* pTaskFinish);


	//窗口控制
public:
	//控制列表
	bool ShowViewControl(bool bShowControl);
	//显示任务信息
	void SetTaskWndShow(bool bIsShow=true);
	//显示时间道具列表窗口
	void SetShowTimeDJListWnd(bool bShow=true);
	//显示道具信息窗口
	void SetShowDJInfoWnd(bool bShow=true, int iIndex=0);
	//显示商城窗口
	void SetShowBizCityWnd(bool bShow=true);
	void CloseBizCityWindow(int iID);
	//更新道具信息
	void UpdateDJInfo(int iDJID);
	//显示结算框
	void ShowScoreView(bool bIsView=true);

	//功能函数
public:
	//更新资源
	bool UpdateSkinResource();
	//发送函数
	bool SendData(WORD wSubCmdID);
	//发送函数
	bool SendData(WORD wSubCmdID, void * pData, WORD wDataSize);
	//发送准备
	bool SendUserReady(void * pBuffer, WORD wDataSize);
	//设置定时器
	bool SetGameTimer(WORD wChairID, UINT nTimerID, UINT nElapse);
	//删除定时器
	bool KillGameTimer(UINT nTimerID);
	//设置显示状态
	void SetChatShowTag(bool bIsShow=true, bool bIsUp=true);
	//响应消息
	void SetChatMessageBeep();


public:
	//播放声音
	bool PlayGameSound(LPCTSTR pszSoundName);
	//播放声音
	bool PlayGameSound(HINSTANCE hInstance, LPCTSTR pszSoundName);
	// --播放声音（等待返回）
	bool PlayGameSound(HINSTANCE hInstance, LPCTSTR pszSoundName, bool bWait);
	// --播放声音(DirectSound)
	bool PlayDirSound(LPCTSTR szRes, bool bFromFile = false, bool bLoop = false);
	// --播放背景音乐(cbType: 0=WAVE, 1=MID)
	bool PlayBGMusic(LPCTSTR szRes, BYTE cbType, bool bFromFile = false);
	bool StopBGMusic();
	//是否正在播放
	bool IsPlayingSound();
	//声音状态
	void SetSoundState(bool bIsEnable=true);
	
	//声音控制
public:
	//是否启用
	bool IsEnableSound();
	//启用声音
	void EnableSound(bool bEnable);
	bool IsEnableBGMusic() { return m_bEnableBGMusic; }
	// --启用背景音乐
	void EnableBGMusic(bool bEnable, BYTE cbType);
	// --设置音量
	bool SetSoundVol(BYTE cbVol = 100);
	// --获取音量值
	BYTE GetSoundVol() { return m_cbSoundVol; }
	// --设置背景音乐音量
	bool SetBGMusicVol(BYTE cbVol, BYTE cbType);
	// --获取背景音量值
	BYTE GetBGMusicVol() {return m_cbBGMusicVol; }




	//消息函数
public:
	//插入图片
	bool InsertImage(CBitmap * pBitmap);
	//插入名字
	bool InsertUserName(LPCTSTR pszUserName);
	//系统消息
	bool InsertSystemString(LPCTSTR pszString);
	//特色消息
	bool InsertShtickString(LPCTSTR pszString, COLORREF crColor, bool bNewLine);
	//普通消息
	bool InsertGeneralString(LPCTSTR pszString, COLORREF crColor, bool bNewLine);

	//辅助函数
public:
	//激活界面
	void ActiveGameFrame();
	//切换椅子
	WORD SwitchViewChairID(WORD wChairID);

	//旁观控制
public:
	//允许旁观
	bool IsAllowUserLookon();
	//旁观控制
	bool AllowUserLookon(DWORD dwUserID, bool bAllowLookon);

	//信息函数
public:
	//旁观模式
	bool IsLookonMode();
	//允许旁观
	bool IsAllowLookon();

	//信息函数
public:
	//自己位置
	WORD GetMeChairID();
	//时间位置
	WORD GetTimeChairID();
	//加入游戏
	VOID JoinInGameTable(WORD wChairID);
	//获取用户
	const tagUserData * GetUserData(WORD wChairID);
	//内核接口
	void * GetClientKernel(const IID & Guid, DWORD dwQueryVer);
	//获得桌子椅子数
	WORD GetGameDeskChairNums();
	//获取游戏ID
	WORD GetGameKindID();
	//解析公告
	void ParseChar(const char* pData, WORD wDataSize);
	//隐藏用户动作窗口
	void SetShowUserActionWnd(bool bIsShow=false);
	//发送动作
	void SendUserActionMsg(BYTE btRecvChairID=-1);
	//视图点击操作
	void ViewHitOper();
	
public:
	//播放任务完成动画
	void PlayTaskFinishFlash(bool bIsPlay=true, UINT uiNums=0);
	//播放等待动画
	void PlayWaitFlash(bool bIsPlay=true);

	//游戏状态
public:
	//游戏状态
	BYTE GetGameStatus();
	//游戏状态
	void SetGameStatus(BYTE bGameStatus);

	//游戏名称
public:
	void GetGameName(CString& strName);

	//OSC UPDATE
public:
	//退出游戏方法
	void SetGameExitOperation(BYTE bTag=2);

protected:
	//声音音量
	bool                                m_bEnableBGMusic;
	BYTE                                m_cbSoundVol;
	BYTE                                m_cbBGMusicVol;

private:
	CImage			m_FrameImageBuffer;					//缓冲位图
	CPngImage		m_ImageTitleBack;					//标题栏背景
	CPngImage		m_ImageTitleText;					//标题栏文本
	CPngImage		m_ImageTitleNum;					//数字

	CSkinButton		m_btTitleMinCtBt;						//最小化按钮
	CSkinButton		m_btTitleSoundBt;						//声音按钮
	CSkinButton		m_btTitleWndCloseBt;					//关闭按钮
	CSkinButton		m_btTitlePhoto;							//游戏截图
	CSkinButton		m_btTitleHome;							//官方首页
	CSkinButton		m_btTitleShop;							//道具商城
	CSkinButton		m_btTitleServer;						//客服中心

	//标题栏资源加载
	void LoadTitleResource(void);

	//按钮事件
	void OnBnMinClickedEvent();
	void OnBnSoundClickedEvent();
	void OnBnCloseClieckedEvent();
	void OnBnGamePhotoClieckedEvent();

	//重载函数
protected:
	//控件绑定
	virtual void DoDataExchange(CDataExchange * pDX);
	//初始化函数
	virtual BOOL OnInitDialog();
	//消息解释
	virtual BOOL PreTranslateMessage(MSG * pMsg);
	//确定消息
	virtual void OnOK();
	//取消消息
	virtual void OnCancel();

protected:
	//游戏截图
	void SaveScreenToBmp();
	void WriteBmpToFile(HBITMAP hBitmap,CString strFileName);

	//消息映射
protected:
	//控制按钮
	VOID OnBnClickedControl();
	//位置改变
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//位置消息
	afx_msg void OnGetMinMaxInfo(MINMAXINFO * lpMMI);
	//位置改变
	afx_msg void OnWindowPosChanging(WINDOWPOS * lpwndpos);
	//重画消息
	afx_msg void OnPaint();
	//时间消息
	afx_msg void OnTimer(UINT nIDEvent);

	DECLARE_MESSAGE_MAP()
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg LRESULT OnNcHitTest(CPoint point);//by hxh
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMoving(UINT fwSide, LPRECT pRect);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
protected:
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnWindowPosChanged(WINDOWPOS* lpwndpos);
	//发送语音消息
	void SendLanguageListMsg(UINT uiIndex);

	// 安全显示或隐藏窗口
	void SafeShowOrHideWnd(CWnd* pWnd, int nCmdShow);

	// 安全打亮或打灰窗口
	void SafeEnableWnd(CWnd* pWnd, BOOL bEnable);
	/// 绘制框架
	void DrawFrameView(CDC * pDC, int nWidth, int nHeight);
};

extern CGameFrameDlg * g_GameFrameDlg;
//////////////////////////////////////////////////////////////////////////

#endif