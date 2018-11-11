#pragma once

#include "resource.h"

// CDlgLogonLogo 对话框

class CDlgLogonLogo : public CDialog
{
	DECLARE_DYNAMIC(CDlgLogonLogo)

private:
	BLENDFUNCTION			m_Blend;
	HDC						m_hdcMemory;
	int						m_BakWidth , m_BakHeight ;
	Gdiplus::Image			*m_pImageBack;
	HINSTANCE				hFuncInst ;
	typedef BOOL			(WINAPI *MYFUNC)(HWND,HDC,POINT*,SIZE*,HDC,POINT*,COLORREF,BLENDFUNCTION*,DWORD);          
	MYFUNC UpdateLayeredWindow;

public:
	CDlgLogonLogo(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgLogonLogo();

// 对话框数据
	enum { IDD = IDD_LOGONLOGO };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnWindowPosChanging(WINDOWPOS* lpwndpos);
	afx_msg void OnMove(int x, int y);

public:
	//画图logo图
	BOOL DrawLogo(int Transparent=255);

	afx_msg void OnPaint();
};
