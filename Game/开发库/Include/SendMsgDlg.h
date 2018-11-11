#pragma once

#include "Stdafx.h"
#include "Resource.h"

// CSendMsgDlg 对话框

class CGameFrameControl;

class CSendMsgDlg : public CRgnDialog
{
public:
	CSendMsgDlg();   // 标准构造函数
	virtual ~CSendMsgDlg();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();

private:
	CRgnButton				m_btWndTag;						//标志按钮
	CComboBox				m_ChatInput;					//聊天输入
	CRgnButton				m_btSendChat;					//发送按钮
	CRgnButton				m_btExpression;					//表情按钮
	CExpression				m_Expression;					//表情窗口
	CRgnButton				m_btWndTagLight;				//标志按钮加亮

	CGameFrameControl*		m_pGameFrameControl;			//框架控件指针
	DWORD					m_dwLimitChatTimes;				//限制聊时
	bool					m_bIsShowGameFrame;				//显示标志
	CImage					m_ImageBuffer;					//缓冲位图
	int						m_iCountMsgNums;				//计算消息个数
	bool					m_bIsShowWndTag;				//响应标志
	bool					m_bIsStartTime;					//是否启动定时器


private:
	CSkinImage				m_ImageWndBack;					//窗口背景

protected:
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

	//表情消息
	LRESULT OnHitExpression(WPARAM wParam, LPARAM lParam);

	//鼠标右键按下消息
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);


	//发送消息
	void SetGameChatMsg();

	//设置显示
	void SetGameChatShow();

public:
	//响应消息
	void SetMsgBeep();

public:
	virtual BOOL OnInitDialog();

	//设置窗口指针
	void SetGameFrameControlPtr(CGameFrameControl* pGameFrameControl);

	//设置显示标志
	void SetGameFrameShowTag(bool bIsShow=true);

	
//	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnTimer(UINT nIDEvent);
};
