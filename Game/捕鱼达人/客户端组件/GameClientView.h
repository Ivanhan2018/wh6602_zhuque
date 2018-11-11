#ifndef GAME_CLIENT_HEAD_FILE
#define GAME_CLIENT_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "ScoreView.h"
#include "ScoreView.h"
#include "GameClientViewDx.h"

//////////////////////////////////////////////////////////////////////////
//宏定义

//////////////////////////////////////////////////////////////////////////


//游戏视图
class CGameClientView : public CGameFrameView
{
	//游戏变量
public:
	bool							m_bInitGameTag;						//初始标志

	//控件组件
public:
	CGameClientViewDX				m_GameClientViewDX;					//d3d窗口

	//函数定义
public:
	//构造函数
	CGameClientView();
	//析构函数
	virtual ~CGameClientView();


	//控制接口
public:
	//重置界面
	virtual void ResetGameView();

	//继承函数
protected:
	//调整控件
	virtual void RectifyGameView(INT nWidth, INT nHeight);
	//绘画界面
	virtual void DrawGameView(CDC * pDC, INT nWidth, INT nHeight);

	//消息函数
public:
	//创建函数
	INT OnCreate(LPCREATESTRUCT lpCreateStruct);
	////消息截取
	//virtual BOOL PreTranslateMessage(MSG* pMsg);
	
	DECLARE_MESSAGE_MAP()		
};

//////////////////////////////////////////////////////////////////////////

#endif