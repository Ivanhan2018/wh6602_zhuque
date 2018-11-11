#ifndef GAME_HELP_DLG_HEAD_FILE
#define GAME_HELP_DLG_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "Resource.h"
#include "SkinButtonEx.h"

//////////////////////////////////////////////////////////////////////////

//帮助类型
enum enHelpKind
{
	enHelpKind_Game,
	enHelpKind_Bullet,
	enHelpKind_FishScore,
	enHelpKind_Canno
};

//帮助对话框
class CGameHelpDlg : public CDialog
{
	//状态变量
private:
	enHelpKind							m_HelpKind;						//帮助类型

	//资源变量
protected:
	CPngImage							m_PngGameHelp;					//背景图案
	CPngImage							m_PngBulletHelp;				//背景图案
	CPngImage							m_PngFishScore;					//背景图案
	CPngImage							m_PngCannoHelp;					//背景图案

	//组件变量
protected:
	CWnd								* m_pViewWnd;					//视图窗体

	//按钮变量
protected:
	CSkinButtonEx						m_btClose;						//关闭按钮
	CSkinButtonEx						m_btGameHelp;					//帮助按钮
	CSkinButtonEx						m_btBulletHelp;					//帮助按钮
	CSkinButtonEx						m_btFishScore;					//帮助按钮
	CSkinButtonEx						m_btCannoHelp;					//帮助按钮

	//函数定义
public:
	//构造函数
	CGameHelpDlg(CWnd	*pViewWnd);
	//析构函数
	virtual ~CGameHelpDlg();

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