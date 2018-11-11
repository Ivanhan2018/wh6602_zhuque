#ifndef DLG_STATUS_HEAD_FILE
#define DLG_STATUS_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "Resource.h"

//////////////////////////////////////////////////////////////////////////

//游戏提示状态类
class CDlgStatus : public CDialog
{
	//变量定义
protected:
	WORD								m_wImagePos;					//位图位置					
	CWnd								* m_pNotifyWnd;					//通知窗口
	CString								m_strMessage;					//消息字符

	//控件变量
protected:
	CSkinButton							m_btCancel;						//取消按钮
	CGDIPlus_Image						m_ImageBack;					//背景图
	int									m_iAction;						//帧数

private:
	BLENDFUNCTION						m_Blend;
	HINSTANCE							m_hFuncInst;
	int									m_BakWidth;
	int									m_BakHeight;
	CDC									m_hdcMemory;
	typedef BOOL						(WINAPI *MYFUNC)(HWND,HDC,POINT*,SIZE*,HDC,POINT*,COLORREF,BLENDFUNCTION*,DWORD);
	MYFUNC								UpdateLayeredWindow;

	HICON								m_hIcon;
//	CDC m_memdc;
	//函数定义
public:
	//构造函数
	CDlgStatus();
	//析构函数
	virtual ~CDlgStatus();

	//重载函数
protected:
	//控件绑定
	virtual void DoDataExchange(CDataExchange * pDX);
	//确定函数
	virtual void OnOK() { };
	//取消函数
	virtual void OnCancel() { return; };

	//获取当前路径
	void GetCurrentDirectoryPath(CString &szPath);
	//资源路径
	void GetResPath(CString strClipName, CString& strPath);

	BOOL UpdateClockDisplay(int Transparent=255);
	//功能函数
public:
	//设置消息
	bool ShowStatusMessage(LPCTSTR pszMessage, CWnd * pNotifyWnd);
	//销毁窗口
	bool DestroyStatusWnd(CWnd * pNotifyWnd);

	//消息映射
protected:
	//重画消息
	afx_msg void OnPaint();
	//取消按钮
	afx_msg void OnBnClickedClose();
	//定时器消息
	afx_msg void OnTimer(UINT nIDEvent);

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();


	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};

//////////////////////////////////////////////////////////////////////////

#endif