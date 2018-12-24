#pragma once

#include "GameFrame.h"

class CUserAction : public CWnd
{
	DECLARE_DYNAMIC(CUserAction)

public:
	CUserAction();
	virtual ~CUserAction();

	virtual void OnFinalRelease();

public:
	//获得当前用户点击的动作标识
	BYTE GetCurUserActionTag();
	//是否可以发送
	bool GetCurSendStatus();
	//设置是否可以发送
	void SetCurSendStatus(bool bIsSend=true);
	//设置是否可以发送
	void SetTimeIntervalSend(bool bIsSend=false);
	//获得超时是否可以发送
	bool GetTimeIntervalSendStatus();
	//重置数据
	void SetResetData();

protected:
	DECLARE_MESSAGE_MAP()
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//鼠标离开消息
	LRESULT OnMouseLeave(WPARAM wparam, LPARAM lparam);

public:
	//过滤颜色
	void AddRectRgn(CDC* pDC,CRgn* rgn,CRect* rect,COLORREF col);
	void UpdateRgn(CDC * pDC);
	bool DrawWindow(CDC * pDC);

private:
	bool			m_bIsSend;
	bool			m_bIsAlreadySend;
	bool			m_bHovering;
	BYTE			m_btCurUserActionTag;
	CSkinImage		m_ImageBack;

	CSkinButton		m_btUserAction[20];
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
protected:
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
};


