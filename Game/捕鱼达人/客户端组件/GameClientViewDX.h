#ifndef DX_GAME_VIEW_HEAD_FILE
#define DX_GAME_VIEW_HEAD_FILE
#pragma once

#include "Stdafx.h"
#include "DxSkin.h"
#include "DxDialog.h"
#include "Fish.h"
#include "cannon.h"
#include "Bullet.h"
#include "Coin.h"
#include "SkinButtonEx.h"
#include "ScoreView.h"
#include "Seascene.h"
#include "AudioPlayer.h"
#include "Bonus.h"

/////////////////////////////////////////////////////////////
//#define WM_SHOOT				WM_USER+100								//发射炮弹

////////////////////////////////////////////////////////////

#define TEXTURE_INDEX_BULLET_COUNT		0								//纹理索引
#define TEXTURE_INDEX_COIN_COUNT		1								//纹理索引
#define TEXTURE_INDEX_NONE				2								//纹理索引

//轨迹信息
struct tagFishTraceInfo
{
	int							nInitX[5];
	int							nInitY[5];
	int							nInitCount;
	BYTE						byMoveSpace;
	tagFishInfo					*pFishInfo;
};

//位置类型
enum enDrawPos
{
	enDrawPos_MeGoldBack,
	enDrawPos_MeGold,
	enDrawPos_AccountBack,
	enDrawPos_Account,
	enDrawPos_BulletBack,
	enDrawPos_Bullet,
	enDrawPos_CoinBack,
	enDrawPos_Coin
};

//类声明
class CGameClientDlg;

//d3d窗口类
class CGameClientViewDX : public CDxDialog
{
	//用户信息
private:
	LONG							m_lBulletCount[GAME_PLAYER][enBulletCountKind_End];//子弹数目
	LONGLONG						m_lCoinCount[GAME_PLAYER];			//金币数目
	WORD							m_wCptrFishCount[enFishKind_No];	//鱼群数目
	LONG							m_lBulletCharge[enBulletCountKind_End];	//子弹费用
	bool							m_bLookonMode;						//旁观标识

	//涨潮变量
private:
	int								m_nSceneMovePosX;					//移动位置
	enSeaSceneKind					m_NewSeaScene;						//新的场景
	bool							m_bWaveLToR;						//改变方向
	bool							m_bWaveEffect;						//涨潮标识
	bool							m_bWaveComing;						//涨潮标识

	//线程变量
public:
	HANDLE							m_hEventFinish;						//事件句柄
	HANDLE 							m_hCalcThread;						//线程句柄
	bool							m_bExist;							//退出标识
	bool							m_bResetArray;						//重置标识
	bool							m_bLoadResFinish;					//加载标识
	bool							m_bDeviceReset;						//重置标识

	//游戏变量
protected:
	bool							m_bReleaseKey;						//释放键盘
	bool							m_bWaitScoreInfo;					//等待分数
	bool							m_bInitControlTag;					//初始标志
	DWORD							m_dwDelayTimes;						//延时时间
	DWORD							m_dwCurTimes;						//当前时间
	WORD							m_wMeChairID;						//自己座位
	DWORD							m_dwLastShootTime;					//射击时间
	WORD							m_wIdleTime;						//空闲时间
	enSeaSceneKind					m_CurSeaScene;						//当前场景
	bool							m_bNoBulletWarn;					//没有警告
	DWORD							m_dwNoBulletWarnTime;				//警告时间
	LONG							m_lCellScore[GAME_PLAYER];			//单元积分
	DWORD							m_dwLastCalcTime;					//计算时间
	DWORD							m_dwLastSwitchTime;					//切换时间

	//彩金变量
protected:
	LONG							m_lBonusCount;						//彩金数目
	CBonus							m_Bonus;							//彩金效果
	
	//轨迹变量
public:
	CArrayTemplate<tagFishTraceInfo>m_FishArrayBuffer;					//缓冲鱼群
	CArrayTemplate<tagFishInfo*>	m_FishArrayActive;					//激活鱼群
	
	//资源变量
protected:	
	CCannon							m_Cannon;							//大炮变量
	CBullet							m_Bullet;							//炮弹变量
	CCoin							m_Coin;								//金币变量

	//资源变量
protected:
	LPD3DXFONT						m_pFontUserInfo;					//D X 字体
	LPD3DXFONT						m_pFontGameInfo;					//D X 字体

	//界面变量
protected:
	CDxSkin							m_ToolBarBk;						//背景图片
	CDxSkin							m_UserInfo;							//玩家信息
	CDxSkin							m_Number;							//数字图片
	CDxSkin							m_Wave;								//涨潮海浪
	CDxSkin							m_NoBulletWarn;						//涨潮海浪
	CDxSkin							m_WaveEffect;						//海浪效果
	CDxSkin							m_GameTip;							//游戏提示
	CSeaScene						m_SeaScene[enSeaSceneKind_No];		//海底场景

	//组件变量
private:
	CGameClientDlg					* m_pGameClientDlg;					//主窗体类

	//按钮控件
private:
	CSkinButtonEx					m_btShowBulletShop;					//购买子弹
	CSkinButtonEx					m_btQuitGame;						//退出游戏
	CSkinButtonEx					m_btGameHelp;						//游戏帮助

	//声音变量
protected:
	CAudioPlayer						m_AudioPlayerBG;				//背景语音
	CAudioPlayer						m_AudioPlayerSyn;				//同步语音
	CAudioPlayer						m_AudioPlayerAsyn[MAX_SYN_SOUND];//异步语音

	//函数定义
public:
	//构造函数
	CGameClientViewDX();
	//析构函数
	virtual ~CGameClientViewDX();

	//继承函数
public:
	//播放声音
	VOID PlayGameSound(WORD wSoundType);

	//继承函数
public:
	//更新输入
	//virtual VOID UpdateControl();
	//渲染函数
	virtual VOID DrawGameView2D();
	//调整位置
	virtual VOID InitScene(INT nWidth,INT nHeight);
	//释放资源
	virtual VOID ReleaseScene();
	//释放资源
	virtual VOID OnDeviceLost();
	//更新窗口
	bool UpdateDxWindowSize();

	//功能函数
public:
	//加载图片
	VOID LoadDXImage();
	//加载场景
	void LoadSeaScene(enSeaSceneKind SeaSceneKind);
	//加载鱼群
	void LoadFish();
	//释放场景
	void ReleaseSeaScene(enSeaSceneKind SeaSceneKind);
	//重置图片
	VOID OnResetDevice();
	//播放标志
	VOID EnablePlaySound(bool bEnable);
	//艺术字体
	VOID DrawTextString(LPD3DXFONT pFont, LPCWSTR pszString, D3DCOLOR crText, int nXPos, int nYPos);
	//游戏信息
	void DrawGameInfo();

	//设置接口
public:
	//设置座位
	void SetMeChairID(WORD wMeChairID);
	//增加轨迹
	void AddTrace(int nInitX[], int nInitY[], int nInitCount, enFishKind FishKind, DWORD dwFishID, bool bRegular);
	//设置窗体
	void SetGameClientDlg(CGameClientDlg *pGameClientDlg) { m_pGameClientDlg=pGameClientDlg; }
	//设置子弹
	void SetUserBulletCount(WORD wChairID, enBulletCountKind BulletCountKind, LONG lCount);
	//设置金币
	void SetUserCoinCount(WORD wChairID, int nCount);
	//鱼群数目
	void SetCptrFishCount(enFishKind FishKind, WORD wCount) {m_wCptrFishCount[FishKind]=wCount;}
	//子弹数目
	void SetShootBulletCount(WORD wChairID, int nBulletCount) {m_Cannon.SetBulletCount(wChairID,nBulletCount,false);}
	//设置状态
	bool SetFishStatus(enFishKind FishKind, DWORD dwFishID, enFishStatus FishStatus);
	//子弹费用
	void SetBulletCharge(LONG lBulletCharge[enBulletCountKind_End]) {CopyMemory(m_lBulletCharge,lBulletCharge,sizeof(m_lBulletCharge));}
	//旁观模式
	void SetLokonMode(bool bLookonMode);
	//切换场景
	void SetChangeScene(bool bWaveLToR, bool bChange, enSeaSceneKind SeaSceneKind);
	//单元积分
	void SetCellScore(WORD wChairID, LONG lCellScore) {m_lCellScore[wChairID]=lCellScore;}
	//分数信息
	void SetWaitScoreInfo(bool bWaiting) {m_bWaitScoreInfo=bWaiting;}
	//设置彩金
	void SetBonusInfo(LONG lBonusCount) {m_lBonusCount=lBonusCount;}

	//语音接口
public:
	//获取组件
	CAudioPlayer *GetaudioPlayer(LPCTSTR pszAudioName);
	//播放声音
	bool PlayMp3Sound(LPCTSTR pszSoundName, bool bSyn);
	//播放声音
	bool PlayMp3Sound(LPCTSTR pszPath, LPCTSTR pszSoundName, bool bSyn);
	//播放声音
	bool PlayMp3SoundBG(LPCTSTR pszSoundName, bool bLoop=true);
	//播放声音
	bool PlayMp3SoundBG(LPCTSTR pszPath, LPCTSTR pszSoundName, bool  bLoop=true);
	//停止声音
	bool StopMp3SoundBG();
	//暂停声音
	bool PauseMp3SoundBG();

	//信息接口
public:
	//获取角度
	float GetMeShootAngle();
	//子弹类型
	enBulletCountKind GetCurUseBulletKind(WORD wChairID);
	//玩家发炮
	void UserShoot(WORD wChairID, float fAngle, enBulletCountKind BulletCountKind);
	//增加金币
	void UserAddCoin(WORD wChairID, int nSourcePosX, int nSourcePosY, LONG lCoinCount);
	//增加彩金
	void AddBonus(WORD wChairID, LONG lBonusCount);
	//鱼群种类
	enFishKind GetFishKind(DWORD dwFish);
	//退出查询
	bool QuitGameInquire();
	//显示成绩
	bool ShowScoreView();
	//调整控件
	VOID RectifyGameView(INT nWidth, INT nHeight);
	//子弹数目
	LONG GetBulletCount(WORD wChairID, enBulletCountKind BulletCountKind);
	//椅子位置
	WORD GetMeChairID() {return m_wMeChairID;}
	//设置彩金
	LONG GetBonusCount() { return m_lBonusCount; }
	
	//重载函数 
public:
	//命令信息
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//创建窗口
	virtual bool InitDxWindow();
	//停止渲染
	virtual void StopRender() {m_bDeviceReset=true;}
	//开始渲染
	virtual void StartRender() {m_bDeviceReset=false;}

	//消息映射
protected:
	//鼠标左击
	afx_msg VOID OnLButtonDown(UINT nFlags, CPoint CPoint);
	//鼠标左击
	afx_msg VOID OnLButtonUp(UINT nFlags, CPoint point);
	//鼠标移动
	afx_msg VOID OnMouseMove(UINT nFlags, CPoint point);
	//鱼群消失
	afx_msg LRESULT OnFishDisappear(WPARAM wParam, LPARAM lParam);
	//轨迹消息
	afx_msg LRESULT OnFinishCalcTrace(WPARAM wParam, LPARAM lParam);
	//定时消息
	afx_msg void OnTimer(UINT nIDEvent);
	//右键消息
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	//大小消息
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//打开网
	LRESULT OnNetOpen(WPARAM wParam, LPARAM lParam);

	//内部函数
private:
	//设置位置
	void InitPosition();
	//个人信息
	void DrawUserInfo();
	//绘画数字
	void DrawNumber(int nXPos, int nYPos, LONGLONG lNumber, int nTextIndex);
	//获取位置
	void GetDrawPos(WORD wChairID,enDrawPos DrawPos, int &nPosX, int &nPosY, int nIndex=0);
	//玩家射击
	bool _UserShoot(WORD wChairID, bool bReleaseKey=true);
	//当前数目
	LONG GetCurBulletCount(WORD wChairID);
	//切换炮弹
	void SwitchCannon();	
	
	DECLARE_MESSAGE_MAP()
public:
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//add by Ivan_han
	BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);
};

//////////////////////////////////////////////////////////////////////////
#endif
