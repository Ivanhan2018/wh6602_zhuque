#pragma once
#ifndef _CHEAD_PORTRAIT_H
#define _CHEAD_PORTRAIT_H
#include "stdafx.h"

/// 人物头像动画类

class CHeadPortrait : public CWnd
{
	DECLARE_DYNAMIC(CHeadPortrait)
	friend class CHeadPortraitManager;

private:
	CWnd									* m_pParent;
	CDC										* m_pParentDC;
	CImage									* m_pParentImage;
	CGDIPlus_Image					* m_pImage;

	WORD										m_wID;										///< 动画ID
	UINT										m_uActionCount;						///< 动作数目
	UINT										m_uFrameCount;						///< 帧数目
	vector<BOOL>						m_vFrames;								///< 帧集
	UINT										m_uCurAction;							///< 当前动作
	UINT										m_uCurFrame;							///< 当前帧
	UINT										m_uDefaultAction;					///< 默认动作
	UINT										m_uIntervalTime;						///< 间隔时间
	UINT										m_uFrameTime;						///< 当前时间					
	UINT										m_uRunCount;							///< 动作执次数

	POINT										m_ptCurPos;								///< 当前位置

public:
	CHeadPortrait();
	virtual ~CHeadPortrait();
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, 
		DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);

protected:
	DECLARE_MESSAGE_MAP()

public:
	///  设置动作集
	/// [@param bLine in] 动作数目(一张图片由很多动画组成)，例:普通动作，高兴，哭泣，生气，窃喜，掉线，逃跑
	/// [@param bRow in] 多少帧，一个动作由多少帧组成
	/// [@param pAction in] 动作帧集 （默认值是 7*2 维数组）
	void SetAction(UINT bLine=1, UINT bRow=1, BOOL *pAction=NULL);
	void PlayAction(UINT uIndex, UINT uFrameTime=150, UINT uIntervalTime=0, UINT uResultTime=0, UINT uDefaultAction=0);

protected:
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnTimer(UINT nIDEvent);
};

typedef vector<CHeadPortrait *> VCHeadPortrait;


//////////////////////////////////////////////////////////////////////////////////
/// 人物头像动画管理类

class GAME_FRAME_CLASS CHeadPortraitManager
{
public:
	CHeadPortraitManager();
	virtual ~CHeadPortraitManager();

	/// 初始化人物头像数目
	/// [@param pParent in] 父窗口指针
	/// [@param uCount in] 动画数目
	void Init(CWnd *pParent, UINT uCount);

	/// 调整坐标
	/// [@param pt in] 动画位置集
	void SetPos(CPoint pt[]);

	/// 刷新动画
	/// [@param pDC in] 父窗口 DC
	/// [@param pImage in] 父窗口背景缓存图片指针
	void Refresh(CDC *pDC);
	void Refresh(CImage *pImage);

	/// 播放动作
	/// [@param uID in] 动画 ID（类似玩家ID）
	/// [@param bWoman in] 性别，真为女，假为男
	/// [@param uIndex in] 动作索引
	/// [@param uFrameTime in] 动作每帧的时间，如果默认帧有效，也是默认帧的时间（毫秒）
	/// [@param uIntervalTime in] 动画间隔时间 （毫秒）
	/// [@param uResultTime in] 动作总共时间，小于等于 0 无时间限制（毫秒）
	/// [@param uDefaultAction in] 默认动作（当 uIndex 动作播放完，自动播放默认动作）
	void PlayAction(WORD wID, BOOL bWoman, UINT uIndex, UINT uFrameTime=150, UINT uIntervalTime=0, UINT uResultTime=0, UINT uDefaultAction=0);

	/// 隐藏所有动画
	void Hide(WORD uID, BOOL bAllHide=FALSE);

private:
	VCHeadPortrait	m_vHeadPortrait;				///< 人物头像动画管理指针集
};

#endif