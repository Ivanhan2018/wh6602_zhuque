#ifndef SCORE_VIEW_HEAD_FILE_FRAME_DEF__
#define SCORE_VIEW_HEAD_FILE_FRAME_DEF__

#pragma once

#include "GameFrame.h"

class CGameFrameDlg;

//////////////////////////////////////////////////////////////////////////

//积分视图类
class GAME_FRAME_CLASS CScoreView : public CDialog
{
	//资源变量
protected:
	CImage							m_ImageBuffer;						//缓冲位图
	CPngImage						m_ImagePngBack;						//
private:
	CGameFrameDlg					*m_pGameFrameDlg;					//游戏框架指针
	bool							m_bIsMoveWnd;						//可否移动窗口

public:
	CSize							m_szWndSize;						//窗口大小
	//函数定义
public:
	//构造函数
	CScoreView();
	CScoreView(UINT nIDTemplate, CWnd* pParentWnd = NULL);
	//析构函数
	virtual ~CScoreView();

	//设置游戏框架指针
	void SetGameFramePtr(CGameFrameDlg* pGameFrameDlg);
	//设置可否移动窗口
	void SetDisableMoveWnd(bool bIsMove=true);

public:
	//加载背景图
	bool	LoadBackImage(const char* pImagePath);
	//初始化信息
	virtual void InitScoreViewInfo() = 0;
	//绘制
	virtual void DrawScoreInfo(CDC* pDC) = 0;

public:
	//界面UI分格
	//BOOL SetImageRgn(CPngImage * pImage);

	//消息函数
protected:
	//重画函数
	afx_msg void OnPaint();
	//建立消息
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//鼠标消息
	afx_msg void OnLButtonDown(UINT nFlags, CPoint Point);

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnWindowPosChanging(WINDOWPOS* lpwndpos);

public:
	//返回大厅
	void OnGoPlaza();
	//继续游戏
	void OnStartGame();
	//返回房间
	void OnGoRoom();
	//退出游戏
	void OnGoExit();
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
};

//////////////////////////////////////////////////////////////////////////

#endif
