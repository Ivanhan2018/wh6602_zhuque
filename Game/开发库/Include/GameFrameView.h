#ifndef GAME_FRAME_VIEW_HEAD_FILE
#define GAME_FRAME_VIEW_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "GameFrame.h"
#include "ThreadDraw.h"
//#include "ImageNums.h"
//#include "CHeadPortrait.h"
#include <string>

class CTalkDlg;
using namespace std;

#define REMOVE_FLOWER 0 // 0为移除鲜花，1为加入鲜花
//////////////////////////////////////////////////////////////////////////

//动画结构
struct tagFlowerEffectInfo
{
	//移动信息
	WORD							wMoveCount;							//移动次数
	WORD							wMoveIndex;							//移动索引
	CPoint							ptFrom;								//出发位置
	CPoint							ptDest;								//目的位置
	CPoint							ptCurPos;							//当前位置

	//属性信息
	WORD							wSourceChairID;						//起始位置
	WORD							wTargetChairID;						//结束位置
	UINT							uFlowerID;							//鲜花标识
	UINT							uFlowerEffectID;					//动画标识
	bool							bBombEffect;						//爆炸标识
};

//绘画模式
enum enImageMode 
{
	enMode_Spread,					//平铺模式
	enMode_Centent,					//居中模式
	enMode_ElongGate,				//拉伸模式
};

//类说明
class CClientKernelSink;

//////////////////////////////////////////////////////////////////////////

//游戏视图框架
class GAME_FRAME_CLASS CGameFrameView : public CWnd, public IGameFrameView
{
	//友元定义
	friend class CGameFrameDlg;
	friend class CClientKernelSink;

	//位置变量
protected:
	UINT							m_nAnimeStep;						//移动间距
	POINT							m_ptName[MAX_CHAIR];				//名字位置
	POINT							m_ptFace[MAX_CHAIR];				//头像位置
	POINT							m_ptTimer[MAX_CHAIR];				//时间位置
	POINT							m_ptReady[MAX_CHAIR];				//准备位置
	POINT							m_ptSayPos[MAX_CHAIR];				//说话位置
	POINT							m_ptOnlineTime;						//在线计时位置

	//聊天框指针
	CTalkDlg						* m_pTalkDlg[10];					//聊天框指针

	//用户变量
private:
	tagUserData						* m_pUserItem[MAX_CHAIR];			//用户信息

	//组件指针
private:
	void							* m_pReserve;						//保留指针
	IUserFaceRes					* m_pIUserFaceRes;					//头像接口

	//画图变量
private:
	bool							m_bReDraw;							//是否重画
	CSize							m_ImageSize;						//缓冲大小
	CImage							m_ImageBuffer;						//缓冲位图
	const bool						m_bDoubleBuf;						//缓冲绘图
	const UINT						m_uColorCount;						//缓冲色深
	CThreadDraw						* const m_pThreadDraw;				//绘图信息
	CWnd							* m_pOnlineKeepTime;				//在线计时
	//CHeadPortraitManager			 m_HeadPortrait;

	//绘制计时器
private:
	CPngImage						m_ImageTimerBack;					//计时器背景
	CPngImage						m_ImageTimerNumber;					//计时器数字
	int								m_iCurTimeFrames[MAX_CHAIR];		//当前帧
	WORD							m_wTimer[MAX_CHAIR];				//用户时间
	bool							m_bIsDrawTimer[MAX_CHAIR];			//绘制时间
	POINT							m_ptTimerCurPT[MAX_CHAIR];			//时间当前位置
	bool							m_bIsStartTimer;					//是否启动定时器
	bool							m_bIsDirectorTag[MAX_CHAIR];		//方向标识

private:
	//绘制等待动画
	bool							m_bIsPlayWaitFlash;					//是否播放等待动画
	CPngImage						m_ImageWaitFlash;					//等待动画
	int								m_iCountWaitFrame;					//计算等待帧数
	CRect							m_rcWaitDrawFlash;					//绘制区域

	//绘制断线
private:
	CPngImage						m_ImageBreakLine;					//断线

	//激活计时器动画
protected:
	bool							m_bIsDisableTimerFlash;				//是否激活计时器动画
	WORD							m_wGameUserNums;					//游戏用户数

	//赢取盘数
public:
	//Author<cxf>;DateTime<2012-03-06>;Description:<绘画赢取信息>
	CImageNums						m_ImageNumber;						//数子
	CRect							m_rcWinNumberArea;					//赢取数字绘画区域
	CPngImage						m_ImageWinLostBk;					//赢取盘数背景
	int								m_nKindIDWinCount;					//赢数
	int								m_nKindIDLostCount;					//输数
	int								m_nKindIDDrawCount;					//平数
	int								m_nKindIDFleeCount;					//逃跑数

	//只读变量
public:
	static const int				m_nXFace;							//头像高度
	static const int				m_nYFace;							//头像宽度
	static const int				m_nXTimer;							//定时器背景宽
	static const int				m_nYTimer;							//定时器背景高
	static const int				m_nXBorder;							//框架宽度
	static const int				m_nYBorder;							//框架高度
	static const int				m_nXTimerNumber;					//定时器数字宽
	static const int				m_nYTimerNumber;					//定时器数字高

	//扩展变量
private:

#if REMOVE_FLOWER	// del by HouGuoJiang 2011-11-25
	CFlowerEffectControl			m_FlowerEffectControl[MAX_CHAIR_NORMAL];//鲜花控件
	tagFlowerEffectInfo				m_FlowerInfo[MAX_FLOWER_COUNT];		//鲜花信息
	int								m_nFlowerCount;						//鲜花数目
#endif
	//函数定义
public:
	//构造函数
	CGameFrameView(bool bDoubleBuf=true ,UINT uColorCount=16, CThreadDraw * pThreadDraw=NULL);
	//析构函数
	virtual ~CGameFrameView();

	//基础接口
public:
	//释放对象
	virtual void __cdecl Release() { delete this; }
	//接口查询
	virtual void * __cdecl QueryInterface(const IID & Guid, DWORD dwQueryVer);

	//继承函数
private:
	//重置界面
	virtual void ResetGameView()=NULL;
	//调整控件
	virtual void RectifyGameView(int nWidth, int nHeight)=NULL;
	//绘画界面
	virtual void DrawGameView(CDC * pDC, int nWidth, int nHeight)=NULL;

	//功能函数
public:
	//获取时间
	WORD GetUserTimer(WORD wChairID);
	//获取玩家
	const tagUserData * GetUserInfo(WORD wChairID);
	//更新界面
	void UpdateGameView(const CRect * pRect);
	//获取界面
	bool GetGameViewImage(CImage & ImageBuffer, int nXPos, int nYPos, int nWidth, int nHeight);
	//设置位置
	void SetFlowerControlInfo(WORD wChairID,int nXPos, int nYPos, int nWidth=BIG_FACE_WIDTH, int nHeight=BIG_FACE_HEIGHT, UINT nAnimeStep=30);
	//绘画鲜花
	void ShowFlower(WORD wSourceChairID, WORD wTargetChairID, UINT uFlowerID, UINT uFlowerEffectID, bool bBombEffect=false);//?

	/// 人物图像初始化
	void InitHeadPortrait(CWnd *pParent, UINT uCount);
	/// 多个人物图像坐标调整 （注：数目由 InitHeadPortrait 参数决定）
	/// [@param pt in] 动画位置集
	void SetHeadPortraitPos(CPoint pt[]);
	/// 播放动作
	/// [@param uID in] 动画 ID（类似玩家ID）
	/// [@param uIndex in] 动作索引
	/// [@param uFrameTime in] 动作每帧的时间，如果默认帧有效，也是默认帧的时间（毫秒）
	/// [@param uIntervalTime in] 动画间隔时间 （毫秒）
	/// [@param uResultTime in] 动作总共时间，小于等于 0 无时间限制（毫秒）
	/// [@param uDefaultAction in] 默认动作（当 uIndex 动作播放完，自动播放默认动作）
	void PlayHeadPortraitAction(WORD wID, UINT uIndex, UINT uFrameTime=150, UINT uIntervalTime=0, UINT uResultTime=0, UINT uDefaultAction=0);
	/// 隐藏人物头像
	/// [@param uID in] 人物头像ID，为
	void HideHeadPortrait(UINT uID, BOOL bAllHide=FALSE);
	/// 开始在线计时
	void OnlineKeepTime(int iTime, BOOL bStart=TRUE);
	/// 调整在线计时控件坐标
	void RectifyKeepTimePos(int x, int y);
	/// 绘制框架
	void DrawFrameView(CDC * pDC, int nWidth, int nHeight);
	// 绘制局数
	void ResetWinLost(tagUserData* pUserData);
	//绘画赢取局数图片
	void DrawWinLostNumberBk(CDC * pDC);
	//设置绘画赢取局数位置
	void SetWinLostDrawPoint(CPoint cpPoint);
	//画图函数
public:
	//绘画背景
	void DrawViewImage(CDC * pDC, CSkinImage & SkinImage, enImageMode ImageMode);
	//绘画头像
	void DrawUserFace(CDC * pDC,const tagUserData * pUserData, int nXPos, int nYPos, bool bOffLine);
	//绘画头像
	void DrawCustomFace(CDC * pDC,BYTE byViewID, const tagUserData * pUserData, int nXPos, int nYPos);
	//绘画准备
	void DrawUserReady(CDC * pDC, int nXPos, int nYPos);
	//绘制断线标志
	void DrawBreakLineTag(CDC * pDC, int nXPos, int nYPos);
	//绘画时间
	void DrawUserTimer(CDC * pDC , BYTE byViewID);
	//透明绘画
	void AlphaDrawImage(CDC * pDesDC, int nXDes, int nYDes, int nDesWidth, int nDesHeight, CDC * pScrDC, int nXScr, int nYScr, COLORREF crTransColor);
	//透明绘画
	void AlphaDrawImage(CDC * pDesDC, int nXDes, int nYDes, int nDesWidth, int nDesHeight, HBITMAP hBitBmp, int nXScr, int nYScr, COLORREF crTransColor);

	//内部函数
private:
	//重置界面
	void ResetData();
	//设置接口
	bool SetUserFaceRes(IUnknownEx * pIUnknownEx);
	//设置时间
	bool SetUserTimer(WORD wChairID, WORD wTimer);
	//设置用户
	bool SetUserInfo(WORD wChairID, tagUserData * pUserData);
	//设置游戏玩家用户数
	void SetGameUserNums(WORD dwNums);
	//播放游戏等待动画
	void PlayGameWaitFlash(bool bIsPlay=true);

	//消息映射
protected:
	//绘画函数
	afx_msg void OnPaint();
	//位置变化
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//创建消息
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//鼠标消息
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//销毁消息
	afx_msg void OnDestroy();
	//时间消息
	afx_msg void OnTimer(UINT nIDEvent);

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);

public:
	//显示语言框
	void ShowUserSayInfo(BYTE btViewUserID, CString strTalkText);
	void HideUserSayInfo();
	void GetUserImageInfo(BYTE byViewID, int &iPosX, int &iPosY);
};

//////////////////////////////////////////////////////////////////////////

#endif