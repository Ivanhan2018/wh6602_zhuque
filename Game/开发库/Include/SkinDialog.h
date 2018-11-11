#pragma once

#ifndef SKIN_DIALOG_HEAD_FILE
#define SKIN_DIALOG_HEAD_FILE

#include "SkinControls.h"
#include "SkinWndObject.h"

//////////////////////////////////////////////////////////////////////////

//对话框类
class SKIN_CONTROL_CLASS CSkinDialog : public CDialog, public CSkinWndObject
{
protected:
	typedef struct 
	{
		int Caption;		//标题高度
		int left;			//非客户区——左边宽度
		int right;			//非客户区——右边宽度
		int top;			//非客户区——上边宽度
		int bottom;			//非客户区——下边宽度
	}NCRECT;
	NCRECT m_NCRect;
	//函数定义
public:
	//构造函数
	CSkinDialog(UINT nIDTemplate, CWnd * pParentWnd=NULL);
	//析构函数
	virtual ~CSkinDialog();

	//重载函数
public:
	//初始化消息
	virtual BOOL OnInitDialog();

	//设置非客户区域, 标题高度和非客户区左边\右边\上边\下边宽度
	void SetNCRect(int Caption, int left = -1, int right = -1, int top = -1, int bottom = -1);

	//消息映射
protected:
	//激活消息
	afx_msg void OnActivate(UINT nState, CWnd * pWndOther, BOOL bMinimized);
	//激活消息
	afx_msg void OnActivateApp(BOOL bActive, DWORD dwThreadID);
	//鼠标消息
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	//鼠标消息
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//鼠标消息
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	//鼠标消息
	afx_msg void OnRButtonDblClk(UINT nFlags, CPoint point);
	//鼠标消息
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	//鼠标消息
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	//鼠标消息
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//绘画消息
	afx_msg void OnPaint();
	//改变消息
	afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);
	//绘画背景
	afx_msg BOOL OnEraseBkgnd(CDC * pDC);
	//位置改变
	afx_msg void OnWindowPosChanged(WINDOWPOS * lpwndpos);
	//控件颜色
	afx_msg HBRUSH OnCtlColor(CDC * pDC, CWnd * pWnd, UINT nCtlColor);
	//标题消息
	LRESULT	OnSetTextMesage(WPARAM wParam, LPARAM lParam);


	DECLARE_MESSAGE_MAP()
	DECLARE_DYNAMIC(CSkinDialog)
};

//////////////////////////////////////////////////////////////////////////

//对话框类
class SKIN_CONTROL_CLASS CSkinDialogEx : public CSkinDialog
{
	//函数定义
public:
	//构造函数
	CSkinDialogEx(UINT nIDTemplate, CWnd * pParentWnd=NULL);
	//析构函数
	virtual ~CSkinDialogEx();

	//绘画函数
protected:
	//绘画边框
	void DrawBorder(CDC * pDC);
	//绘画标题
	void DrawCaption(CDC * pDC);
	//绘画背景
	void DrawBackGround(CDC * pDC);

	virtual BOOL PreTranslateMessage(MSG * pMsg);

	void DrawPic(CDC * pDC,UINT Uid,CRect *rect);
	//消息函数
protected:
	//重画消息
	afx_msg void OnPaint();
	//位置消息
	afx_msg void OnSize(UINT nType, int cx, int cy);

	DECLARE_MESSAGE_MAP()
	DECLARE_DYNAMIC(CSkinDialogEx)
};

//////////////////////////////////////////////////////////////////////////

//区域对话框类
class SKIN_CONTROL_CLASS CRgnDialog : public CDialog
{
	//变量定义
protected:
	CRgn							m_DialogRgn;						//窗口区域
	CSkinImage						m_ImageBack;						//背景位图

	//函数定义
public:
	//构造函数
	CRgnDialog(UINT uTemplate, CWnd * pParent=NULL);
	//析构函数
	virtual ~CRgnDialog();
	//初始化函数
	virtual BOOL OnInitDialog();

	//功能函数
public:
	//加载位图
	bool LoadRgnImage(LPCTSTR pszFileName, COLORREF crTrans);
	//加载位图
	bool LoadRgnImage(HINSTANCE hInstance, UINT uBitmapID, COLORREF crTrans);

	//内部函数 
private:
	//创建区域
	bool CreateControlRgn(COLORREF crTrans);

	//消息函数
public:
	//重画函数
	afx_msg void OnPaint();
	//鼠标右键按下消息
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//背景消息
	afx_msg BOOL OnEraseBkgnd(CDC * pDC);

	DECLARE_MESSAGE_MAP()
	DECLARE_DYNAMIC(CRgnDialog)
};

//////////////////////////////////////////////////////////////////////////

#endif