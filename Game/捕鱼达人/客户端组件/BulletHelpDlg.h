#ifndef BULLET_HELP_DLG_HEAD_FILE
#define BULLET_HELP_DLG_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "Resource.h"
#include "SkinButtonEx.h"

//#define WM_SHOW_BULLET_SHOP		WM_USER+101								//购买子弹
//////////////////////////////////////////////////////////////////////////

//帮助对话框
class CBulletHelpDlg : public CDialog
{
	//资源变量
protected:
	CPngImage							m_PngBack;						//背景图案

	//组件变量
protected:
	CWnd								* m_pViewWnd;					//视图窗体

	//按钮变量
protected:
	CSkinButtonEx						m_btClose;						//关闭按钮

	//函数定义
public:
	//构造函数
	CBulletHelpDlg(CWnd	*pViewWnd);
	//析构函数
	virtual ~CBulletHelpDlg();

	//重载函数
public:
	//初始函数
	virtual BOOL OnInitDialog();
	//命令消息
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//控件绑定
	virtual void DoDataExchange(CDataExchange* pDX);

	//消息函数
protected:
	//重画函数
	afx_msg void OnPaint();
	//鼠标消息
	void OnLButtonDown(UINT nFlags, CPoint Point);

	DECLARE_MESSAGE_MAP()		
};

//////////////////////////////////////////////////////////////////////////

#endif