#ifndef SCORE_VIEW_HEAD_FILE_CLIENT_DEF
#define SCORE_VIEW_HEAD_FILE_CLIENT_DEF

#pragma once

#include "Stdafx.h"
#include "Resource.h"
#include "CardControl.h"

//////////////////////////////////////////////////////////////////////////

//结束信息
struct tagScoreViewInfo
{
	WORD							wGameScore;							//游戏得分
};

//////////////////////////////////////////////////////////////////////////

//积分视图类
class CGameScoreView : public CScoreView
{
	//积分信息
protected:
	LONGLONG						m_lTax[GAME_PLAYER];				//游戏税收
	LONGLONG						m_lScore[GAME_PLAYER];				//游戏得分
	TCHAR							m_szUserName[GAME_PLAYER][NAME_LEN];//用户名字

	//资源变量
protected:
	CPngImage						m_ImageWinLose;						//胜负标志

	//函数定义
public:
	//构造函数
	CGameScoreView();
	//析构函数
	virtual ~CGameScoreView();
	
	//功能函数
public:
	//设置积分
	void SetGameScore(WORD wChairID, LPCTSTR pszUserName, LONGLONG lScore);
	//设置税收
	void SetGameTax(LONGLONG lTax,WORD wChairID);
	//清理数据
	void ResetScore();

	//消息函数
//protected:
	////重画函数
	//afx_msg void OnPaint();
	////建立消息
	//afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	////鼠标消息
	//afx_msg void OnLButtonDown(UINT nFlags, CPoint Point);

	//DECLARE_MESSAGE_MAP()
//public:
//	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
//	afx_msg void OnMove(int x, int y);


public:
	//初始化信息
	virtual void InitScoreViewInfo();
	//初始化信息后
	virtual void AlreadyInitInfo();
	//绘制
	virtual void DrawScoreInfo(CDC* pDC);
};

//////////////////////////////////////////////////////////////////////////

#endif
