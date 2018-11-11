//
// GdipButton.h : Version 1.0 - see article at CodeProject.com
//
// Author:  Darren Sessions
//          
//
// Description:
//     GdipButton is a CButton derived control that uses GDI+ 
//     to support alternate image formats
//
// History
//     Version 1.0 - 2008 June 10
//     - Initial public release
//
// License:
//     This software is released under the Code Project Open License (CPOL),
//     which may be found here:  http://www.codeproject.com/info/eula.aspx
//     You are free to use this software in any way you like, except that you 
//     may not sell this source code.
//
//     This software is provided "as is" with no expressed or implied warranty.
//     I accept no liability for any damage or loss of business that this 
//     software may cause.
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include "SkinControls.h"

// GdipButton.h : header file
//
/////////////////////////////////////////////////////////////////////////////
// CGdipButton window
#include <gdiplus.h>
using namespace Gdiplus;  
#pragma comment(lib, "gdiplus.lib")


class SKIN_CONTROL_CLASS CGdipButton : public CButton
{
public:

	CGdipButton();
	virtual ~CGdipButton();

	BOOL LoadImage(const WCHAR * pwzFilePath);
	BOOL LoadStream(IStream * pStream);
	BOOL LoadImage(UINT uResourceID, HINSTANCE hResourceDLL, const LPCTSTR szResType=_TEXT("PNG"));

	void SetBkGnd(CDC* pDC);

	void SetToolTipText(CString spText, BOOL bActivate = TRUE);
	void SetToolTipText(UINT nId, BOOL bActivate = TRUE);
	void SetHorizontal(bool ImagesAreLaidOutHorizontally = FALSE);
	void DeleteToolTip();
	
	//设置透明度
	void SetAlpha(float falpha=1.00000f);

	//获得按钮的宽度
	UINT GetWidth();
	//获得按钮的高度
	UINT GetHeight();

	//是否为空
	bool IsNull();

	//重置按钮
	void ResetBT();

protected:

	void PaintBk(CDC* pDC);
	void PaintBtn(CDC* pDC);

	BOOL	m_bIsHovering;
	BOOL	m_bIsTracking;

	DWORD   m_dwToken;

	int		m_nCurType;

	float	m_fAlpha;

	Gdiplus::Image* m_pStdImage;

	CString			m_tooltext;
	CToolTipCtrl*	m_pToolTip;
	
	void	InitToolTip();

	virtual void PreSubclassWindow();
	virtual void DrawItem(LPDRAWITEMSTRUCT /*lpDrawItemStruct*/);
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	//{{AFX_MSG(CGdipButton)
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg LRESULT OnMouseLeave(WPARAM wparam, LPARAM lparam);
	afx_msg LRESULT OnMouseHover(WPARAM wparam, LPARAM lparam) ;
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()


	//通过流加载文件
	IStream * GetStreamFromRes(UINT uResourceID, HINSTANCE hResourceDLL, LPCTSTR szResType=_TEXT("PNG"));
	//设置窗口大小
	void SetWndSize();
	//句柄初始化为空
	void SetDCNull();
	//更新
	void Update();

private:

	CDC		m_dcBk;			// button background
	
	CDC		m_dcStd;		// standard button
	CDC		m_dcStdP;		// standard button pressed
	CDC		m_dcStdH;		// standard button hot
	CDC		m_dcStdF;		// standard button get fosucs
	CDC		m_dcGS;			// grayscale button (does not have a hot or pressed state)

	CDC*	m_pCurBtn;		// current pointer to one of the above

	BOOL	m_bIsResetDC;

};
