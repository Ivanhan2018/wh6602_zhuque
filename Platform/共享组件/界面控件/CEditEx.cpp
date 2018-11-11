// CEditEx.cpp : 实现文件
//

#include "stdafx.h"
#include "CEditEx.h"

#define DefTextColor	RGB(0, 0, 0);
#define DefBkColor		RGB(255,255,255)
#define DisableBKColor	RGB(212, 208, 200)


class CSkinEditRes{
public:
	CSkinEditRes()
	{
		m_TextColor = DefTextColor;
		m_BKColor	= DefBkColor;
		m_Brush.CreateSolidBrush(m_BKColor);
	}

public:
	COLORREF	m_TextColor;	//字体颜色	
	COLORREF	m_BKColor;		//背景颜色	
	CBrush		m_Brush;		//背景画刷
};

IMPLEMENT_DYNAMIC(CEditEx, CEdit)
BEGIN_MESSAGE_MAP(CEditEx, CEdit)		
	ON_WM_NCPAINT()	
	ON_WM_PAINT()
	ON_WM_CTLCOLOR_REFLECT()
END_MESSAGE_MAP()

// CEditEx
CEditEx::CEditEx()
{	
	pData = new CSkinEditRes;
}

CEditEx::~CEditEx()
{
	delete pData;
}

void CEditEx::PreSubclassWindow()
{	
	CEdit::PreSubclassWindow();
	SetWindowLong(GetSafeHwnd(), GWL_STYLE, GetWindowLong(GetSafeHwnd(),GWL_STYLE) | WS_BORDER);
}

BOOL CEditEx::OnChildNotify(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pLResult) 
{
	if (message != WM_CTLCOLOREDIT && message != WM_CTLCOLORSTATIC)
		return CEdit::OnChildNotify(message, wParam, lParam, pLResult);
	
	HDC hdcChild = (HDC)wParam;	
	SetBkMode(hdcChild, TRANSPARENT);

	if (!IsWindowEnabled())
	{		
		::SetTextColor(hdcChild, DisableBKColor);
		::SetBkColor(hdcChild, DisableBKColor);
		*pLResult = (LRESULT)(CBrush(RGB(212, 208, 200)).GetSafeHandle());
	}
	else
	{		
		::SetTextColor(hdcChild, pData->m_TextColor);
		::SetBkColor(hdcChild, pData->m_BKColor);		
		*pLResult = (LRESULT)(pData->m_Brush.GetSafeHandle());
	}	

	return TRUE;
}

HBRUSH CEditEx::CtlColor(CDC* pDC, UINT nCtlColor) 
{
	switch(nCtlColor)
	{
	case CTLCOLOR_EDIT:
		{
			SetBkMode(pDC->m_hDC, TRANSPARENT);

			if (!IsWindowEnabled())
			{		
				::SetTextColor(pDC->m_hDC, DisableBKColor);
				::SetBkColor(pDC->m_hDC, DisableBKColor);
				return NULL;
			}
			else
			{		
				::SetTextColor(pDC->m_hDC, pData->m_TextColor);
				::SetBkColor(pDC->m_hDC, pData->m_BKColor);		
				return pData->m_Brush;
			}	
		}
		break;

	default:
		return NULL;
	}	
}


void CEditEx::SetDefault()
{
	pData->m_TextColor = DefTextColor;
	pData->m_BKColor	= DefBkColor;			
	pData->m_Brush.DeleteObject();
	pData->m_Brush.CreateSolidBrush(pData->m_BKColor);	

	Invalidate(FALSE);
}

void CEditEx::SetBKColor(COLORREF bkcolor)
{
	pData->m_BKColor = bkcolor;
	pData->m_Brush.DeleteObject();
	pData->m_Brush.CreateSolidBrush(bkcolor);	
	Invalidate(FALSE);
}

void CEditEx::SetFontColor(COLORREF fontcolor)
{
	pData->m_TextColor = fontcolor;
	Invalidate(FALSE);
}

void CEditEx::OnNcPaint()
{	
	if (GetWindowLong(GetSafeHwnd(), GWL_STYLE) & WS_BORDER)
		DrawBorder();	
	else
		CEdit::OnNcPaint();
}

void CEditEx::OnPaint()
{
	CEdit::OnPaint();
	if (GetWindowLong(GetSafeHwnd(), GWL_STYLE) & WS_BORDER)
		DrawBorder();
}

void CEditEx::DrawBorder()
{	
	CRect rc;
	GetWindowRect(&rc);
	rc.OffsetRect(-rc.left, -rc.top);

	CDC *pDC = GetWindowDC();
	const int nBorder = GetSystemMetrics(SM_CXSIZEFRAME);

	if (!IsWindowEnabled())
	{
		pDC->FillSolidRect(0, 0, rc.right, nBorder, DisableBKColor);
		pDC->FillSolidRect(rc.right - nBorder, 0, rc.right, rc.bottom, DisableBKColor);
		pDC->FillSolidRect(0, 0, nBorder, rc.bottom, DisableBKColor);
		pDC->FillSolidRect(0, rc.bottom - nBorder, rc.right, rc.bottom, DisableBKColor);
	}
	else
	{
		pDC->FillSolidRect(0, 0, rc.right, nBorder, pData->m_BKColor);
		pDC->FillSolidRect(rc.right - nBorder, 0, rc.right, rc.bottom, pData->m_BKColor);
		pDC->FillSolidRect(0, 0, nBorder, rc.bottom, pData->m_BKColor);
		pDC->FillSolidRect(0, rc.bottom - nBorder, rc.right, rc.bottom, pData->m_BKColor);
	}

	pDC->Draw3dRect(&rc, RGB(113,111,99), RGB(255,255,255));
	pDC->Draw3dRect(1, 1, rc.right - 2, rc.bottom - 2, RGB(158,157,162), RGB(157,156,161));
	
//	pDC->MoveTo(0,0);
//	pDC->LineTo(rc.right, 0);
//
//	pDC->MoveTo(rc.right-1, 0);
//	pDC->LineTo(rc.right - 1, rc.bottom);
//
//	pDC->MoveTo(rc.right - 1, rc.bottom - 1);
//	pDC->LineTo(0, rc.bottom - 1);
//
//	pDC->MoveTo(0, 0);
//	pDC->LineTo(0, rc.bottom);

//	pDC->SelectObject(pOldPen);
//	DeleteObject(pOldPen);
	ReleaseDC(pDC);
}
