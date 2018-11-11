#ifndef SCORE_VIEW_HEAD_FILE
#define SCORE_VIEW_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "Resource.h"
#include "SkinButtonEx.h"

//#define WM_QUIT_GAME		WM_USER+301								//退出游戏

//////////////////////////////////////////////////////////////////////////

//积分视图类
class CFishScoreView : public CDialog
{
	//变量定义
protected:
	HRGN								m_hRGN;								//区域句柄
	LONG								m_lBulletCount[enBulletCountKind_End];//子弹数目
	LONG								m_lBulletCharge[enBulletCountKind_End];	//子弹费用
	WORD								m_wCptrFishCount[enFishKind_No];	//鱼群数目
	BYTE								m_byAlphaDepth[enFishKind_No];		//透明系数
	LONGLONG							m_lCoinCount;						//金币数目
	enFishKind							m_AlphaIndex;						//透明索引
	bool								m_bShowNumber;						//显示标识
	LONG								m_lCellScore;						//单元积分

	//资源变量
protected:
	CPngImage							m_PngBackUp;						//背景图案
	CPngImage							m_PngBackDown;						//背景图案
	CPngImage							m_PngNumber;						//数字图案
	CPngImage							m_PngNumberScore;					//数字图案
	CPngImage							m_PngFish;							//鱼群图片
	CSkinImage							m_ImageFish;						//鱼群图片

	//组件变量
protected:
	CWnd								* m_pViewWnd;						//视图窗体

	//按钮变量
	CSkinButtonEx						m_btQuitGame;						//退出游戏
	CSkinButtonEx						m_btCalcScore;						//计算分数
	CSkinButtonEx						m_btContinueGame;					//继续游戏

	//函数定义
public:
	//构造函数
	CFishScoreView(CWnd	*pViewWnd, LONG lCellScore);
	//析构函数
	virtual ~CFishScoreView();

	//重载函数
public:
	//初始函数
	virtual BOOL OnInitDialog();
	//命令消息
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

	//信息接口
public:
	//设置信息
	void SetScoreInfo(LONG lBulletCount[enBulletCountKind_End],WORD wCptrFishCount[enFishKind_No],LONGLONG lCoinCount, LONG lBulletCharge[enBulletCountKind_End]);

	//消息函数
protected:
	//重画函数
	afx_msg void OnPaint();
	//建立消息
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//鼠标消息
	afx_msg void OnLButtonDown(UINT nFlags, CPoint Point);
	//大小消息
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//定时消息
	afx_msg void OnTimer(UINT nIDEvent);

	//辅助函数
private:
	//绘画数字
	void DrawNumber(CDC *pDC, int nXPos, int nYPos, LONGLONG lNumber, bool bShowQuantifier);
	//绘画数字
	void DrawNumber(CDC *pDC, int nXPos, int nYPos, LONGLONG lNumber);

	DECLARE_MESSAGE_MAP()	
};

//////////////////////////////////////////////////////////////////////////

#endif