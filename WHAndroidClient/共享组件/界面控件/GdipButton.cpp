//
// GdipButton.cpp : Version 1.0 - see article at CodeProject.com
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

#include "stdafx.h"
#include "MemDC.h"
#include "GdipButton.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//五态图
const int IBTSTATENUMS = 5;

//状态序列
//1:正常状态
//2:鼠标经过状态
//3:鼠标按下状态
//4:获得焦点状态
//5:不激活状态

/////////////////////////////////////////////////////////////////////////////
// CGdipButton

CGdipButton::CGdipButton()
{
	m_bIsHovering = FALSE;
	m_bIsTracking = FALSE;

	m_pToolTip = NULL;

	m_dcBk.m_hDC = NULL;
	m_dcStd.m_hDC = NULL;
	m_dcStdP.m_hDC = NULL;
	m_dcStdH.m_hDC = NULL;
	m_dcStdF.m_hDC = NULL;
	m_dcGS.m_hDC = NULL;

	m_pCurBtn = NULL;
	m_pStdImage = NULL;

	m_bIsResetDC = FALSE;

	m_fAlpha = 1.0f;

	GdiplusStartupInput GdiplusStart;
	GdiplusStartup(&m_dwToken, &GdiplusStart, NULL );
}

CGdipButton::~CGdipButton()
{
	if(m_pToolTip)
	{
		delete m_pToolTip;
	}
	m_pToolTip = NULL;

	delete m_pStdImage;
	m_pStdImage = NULL;

	SetDCNull();
	if (NULL != m_dcBk.m_hDC)
	{
		m_dcBk.DeleteDC();
	}
	m_dcBk.m_hDC = NULL;

	GdiplusShutdown(m_dwToken);
}

//句柄初始化为空
void CGdipButton::SetDCNull()
{
	if (NULL != m_dcStd.m_hDC)
	{
		m_dcStd.DeleteDC();
	}
	m_dcStd.m_hDC = NULL;
	if (NULL != m_dcStdP.m_hDC)
	{
		m_dcStdP.DeleteDC();
	}
	m_dcStdP.m_hDC = NULL;
	if (NULL != m_dcStdH.m_hDC)
	{
		m_dcStdH.DeleteDC();
	}
	m_dcStdH.m_hDC = NULL;
	if (NULL != m_dcStdF.m_hDC)
	{
		m_dcStdF.DeleteDC();
	}
	m_dcStdF.m_hDC = NULL;
	if (NULL != m_dcGS.m_hDC)
	{
		m_dcGS.DeleteDC();
	}
	m_dcGS.m_hDC = NULL;
}


BEGIN_MESSAGE_MAP(CGdipButton, CButton)
	//{{AFX_MSG_MAP(CGdipButton)
	ON_WM_DRAWITEM()
	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR_REFLECT()
	ON_WM_MOUSEMOVE()
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
	ON_MESSAGE(WM_MOUSEHOVER, OnMouseHover)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


BOOL CGdipButton::LoadImage(const WCHAR * pwzFilePath)
{
	if (NULL != pwzFilePath)
	{
		if (m_pStdImage)
		{
			delete m_pStdImage;
		}
		m_pStdImage = NULL;
		m_pStdImage = Gdiplus::Image::FromFile(pwzFilePath);
		if (NULL!=m_pStdImage && !IsNull())
		{
			ResetBT();
			SetWndSize();
			return TRUE;
		}
		return FALSE;
	}
	return FALSE;
}


BOOL CGdipButton::LoadStream(IStream * pStream)
{
	if (NULL != pStream)
	{
		if (m_pStdImage)
		{
			delete m_pStdImage;
		}
		m_pStdImage = NULL;
		m_pStdImage = Gdiplus::Image::FromStream(pStream);
		if (NULL!=m_pStdImage && !IsNull())
		{
			ResetBT();
			SetWndSize();
			return TRUE;
		}
		return FALSE;
	}
	return FALSE;
}

//=============================================================================
//
// LoadStdImage()
//
// Purpose:     The LoadStdImage() Loads the image for the button.  This 
//				function must be called at a minimum or the button wont do 
//				anything.
//
// Parameters:  
//		[IN]	id
//				resource id, one of the resources already imported with the 
//				resource editor, usually begins with IDR_  
//
//		[IN]	pType
//				pointer to string describing the resource type
//				
// Returns:     BOOL
//				Non zero if successful, otherwise zero
//
//=============================================================================
BOOL CGdipButton::LoadImage(UINT uResourceID, HINSTANCE hResourceDLL,const LPCTSTR szResType)
{
	IStream *pStream = GetStreamFromRes(uResourceID, hResourceDLL, szResType);
	if (NULL != pStream)
	{
		if (m_pStdImage)
		{
			delete m_pStdImage;
		}
		m_pStdImage = NULL;
		m_pStdImage = Gdiplus::Image::FromStream(pStream);
		pStream->Release();
		if (NULL!=m_pStdImage && !IsNull())
		{
			ResetBT();
			SetWndSize();
			return TRUE;
		}
		return FALSE;
	}
	return FALSE;
}

IStream * CGdipButton::GetStreamFromRes(UINT uResourceID, HINSTANCE hResourceDLL, LPCTSTR szResType)
{
	HRSRC hSource = FindResource(hResourceDLL, MAKEINTRESOURCE(uResourceID), szResType);
	ASSERT(hSource);
	HGLOBAL hGlobal = LoadResource(hResourceDLL, hSource);
	ASSERT(hGlobal);
	LPVOID lpVoid = ::LockResource(hGlobal);
	ASSERT(lpVoid);

	int nSize = (UINT)SizeofResource(hResourceDLL, hSource);
	HGLOBAL hGlobal2 = GlobalAlloc(GMEM_MOVEABLE, nSize);

	void* pData = GlobalLock(hGlobal2);
	memcpy(pData, (void *)hGlobal, nSize);
	GlobalUnlock(hGlobal2);

	IStream* pStream = NULL;

	if(S_OK == CreateStreamOnHGlobal(hGlobal2, TRUE, &pStream))
	{
		FreeResource(hGlobal2);
		return pStream;
	}
	return NULL;
}


//设置窗口大小
void CGdipButton::SetWndSize()
{
	UINT uiWidth = 0;
	UINT uiHeight = 0;
	if (NULL!=m_pStdImage && !IsNull())
	{
		uiWidth = m_pStdImage->GetWidth()/IBTSTATENUMS;
		uiHeight = m_pStdImage->GetHeight();
		SetWindowPos(NULL,0,0,uiWidth,uiHeight,SWP_NOMOVE);
	}
	else
	{
		SetWindowPos(NULL,0,0,uiWidth,uiHeight,SWP_NOMOVE);
	}
	m_bIsResetDC = TRUE;
	Invalidate();
}

//设置透明度
void CGdipButton::SetAlpha(float falpha)
{
	m_fAlpha = falpha;
	m_bIsResetDC = TRUE;
	Invalidate();
}

//获得按钮的宽度
UINT CGdipButton::GetWidth()
{
	if (NULL!=m_pStdImage && !IsNull())
	{
		UINT uiWidth = m_pStdImage->GetWidth()/IBTSTATENUMS;
		return uiWidth;
	}
	return 0;
}
//获得按钮的高度
UINT CGdipButton::GetHeight()
{
	if (NULL!=m_pStdImage && !IsNull())
	{
		UINT uiHeight = m_pStdImage->GetHeight();
		return uiHeight;
	}
	return 0;
}

//刷新
void CGdipButton::Update()
{
	if(NULL!=m_dcBk.m_hDC && NULL!=m_pStdImage && !IsNull() && m_bIsResetDC)
	{
		SetDCNull();

		CBitmap bmp, *pOldBitmap=NULL;

		CDC *pScreenDC = GetDC();
		if (NULL == pScreenDC)
		{
			return;
		}

		UINT uiWidth = GetWidth();
		UINT uiHeight = GetHeight();
		if (0==uiWidth || 0==uiHeight)
		{
			return;
		}

		Gdiplus::Point points[3];
		memset(points, 0, sizeof (points));
		points[1].X = uiWidth;
		points[2].Y = uiHeight;

		float fColorArray[25] =
		{
			1, 0, 0, 0, 0,
			0, 1, 0, 0, 0,
			0, 0, 1, 0, 0,
			0, 0, 0, m_fAlpha, 0,
			0, 0, 0, 0, 1
		};

		// do everything with mem dc
		CRect rect;
		rect.left = 0;
		rect.right = uiWidth;
		rect.top = 0;
		rect.bottom = uiHeight;
		CMemDC pDC(pScreenDC, rect);

		Gdiplus::Graphics graphics(pDC.m_hDC);

		// standard image
		if (m_dcStd.m_hDC == NULL)
		{
			PaintBk(&pDC);//绘制透明背景

			//正常状态
			ColorMatrix colorMatrix;
			memcpy(&colorMatrix, fColorArray, sizeof (colorMatrix));
			ImageAttributes imageAttr;
			imageAttr.SetColorMatrix(&colorMatrix);
			graphics.DrawImage(m_pStdImage, points, 3, 0, 0, uiWidth, uiHeight, UnitPixel, &imageAttr);

			m_dcStd.CreateCompatibleDC(&pDC);
			bmp.CreateCompatibleBitmap(&pDC, uiWidth, uiHeight);
			pOldBitmap = m_dcStd.SelectObject(&bmp);
			m_dcStd.BitBlt(0, 0, uiWidth, uiHeight, &pDC, 0, 0, SRCCOPY);
			bmp.DeleteObject();

			// standard image hot
			if(m_dcStdH.m_hDC == NULL)
			{//鼠标经过状态
				PaintBk(&pDC);

				ColorMatrix colorMatrix;
				memcpy(&colorMatrix, fColorArray, sizeof (colorMatrix));
				ImageAttributes imageAttr;
				imageAttr.SetColorMatrix(&colorMatrix);
				graphics.DrawImage(m_pStdImage, points, 3, uiWidth, 0, uiWidth, uiHeight, UnitPixel, &imageAttr);

				m_dcStdH.CreateCompatibleDC(&pDC);
				bmp.CreateCompatibleBitmap(&pDC, uiWidth, uiHeight);
				pOldBitmap = m_dcStdH.SelectObject(&bmp);
				m_dcStdH.BitBlt(0, 0, uiWidth, uiHeight, &pDC, 0, 0, SRCCOPY);
				bmp.DeleteObject();
			}

			// standard image pressed
			if (m_dcStdP.m_hDC == NULL)
			{//鼠标按下状态
				PaintBk(&pDC);

				ColorMatrix colorMatrix;
				memcpy(&colorMatrix, fColorArray, sizeof (colorMatrix));
				ImageAttributes imageAttr;
				imageAttr.SetColorMatrix(&colorMatrix);
				graphics.DrawImage(m_pStdImage, points, 3, 2*uiWidth, 0, uiWidth, uiHeight, UnitPixel, &imageAttr);

				m_dcStdP.CreateCompatibleDC(&pDC);
				bmp.CreateCompatibleBitmap(&pDC, uiWidth, uiHeight);
				pOldBitmap = m_dcStdP.SelectObject(&bmp);
				m_dcStdP.BitBlt(0, 0, uiWidth, uiHeight, &pDC, 0, 0, SRCCOPY);
				bmp.DeleteObject();
			}

			// standard image hot
			if(m_dcStdF.m_hDC == NULL)
			{//获得焦点状态
				PaintBk(&pDC);

				ColorMatrix colorMatrix;
				memcpy(&colorMatrix, fColorArray, sizeof (colorMatrix));
				ImageAttributes imageAttr;
				imageAttr.SetColorMatrix(&colorMatrix);
				graphics.DrawImage(m_pStdImage, points, 3, 3*uiWidth, 0, uiWidth, uiHeight, UnitPixel, &imageAttr);

				m_dcStdF.CreateCompatibleDC(&pDC);
				bmp.CreateCompatibleBitmap(&pDC, uiWidth, uiHeight);
				pOldBitmap = m_dcStdF.SelectObject(&bmp);
				m_dcStdF.BitBlt(0, 0, uiWidth, uiHeight, &pDC, 0, 0, SRCCOPY);
				bmp.DeleteObject();
			}

			// grayscale image
			if(m_dcGS.m_hDC == NULL)
			{//不激活状态
				PaintBk(&pDC);

				ColorMatrix colorMatrix;
				memcpy(&colorMatrix, fColorArray, sizeof (colorMatrix));
				ImageAttributes imageAttr;
				imageAttr.SetColorMatrix(&colorMatrix);
				graphics.DrawImage(m_pStdImage, points, 3, 4*uiWidth, 0, uiWidth, uiHeight, UnitPixel, &imageAttr);

				m_dcGS.CreateCompatibleDC(&pDC);
				bmp.CreateCompatibleBitmap(&pDC, uiWidth, uiHeight);
				pOldBitmap = m_dcGS.SelectObject(&bmp);
				m_dcGS.BitBlt(0, 0, uiWidth, uiHeight, &pDC, 0, 0, SRCCOPY);
				bmp.DeleteObject();
			}
		}

		ReleaseDC(pScreenDC);

		if(m_pCurBtn == NULL)
		{
			m_pCurBtn = &m_dcStd;
		}

		m_bIsResetDC = FALSE;
	}
}
//=============================================================================
//
//	The framework calls this member function when a child control is about to 
//	be drawn.  All the bitmaps are created here on the first call. Every thing
//	is done with a memory DC except the background, which get's it's information 
//	from the parent. The background is needed for transparent portions of PNG 
//	images. An always on top app (such as Task Manager) that is in the way can 
//	cause it to get an incorrect background.  To avoid this, the parent should 
//	call the SetBkGnd function with a memory DC when it creates the background.
//				
//=============================================================================
HBRUSH CGdipButton::CtlColor(CDC* pScreenDC, UINT nCtlColor) 
{
	// background透明背景
	if (m_dcBk.m_hDC == NULL)
	{
		CBitmap bmp, *pOldBitmap=NULL;
		UINT uiWidth = GetWidth();
		UINT uiHeight = GetHeight();

		CRect rect1;
		CClientDC clDC(GetParent());
		GetWindowRect(rect1);
		GetParent()->ScreenToClient(rect1);

		m_dcBk.CreateCompatibleDC(&clDC);
		bmp.CreateCompatibleBitmap(&clDC, uiWidth, uiHeight);
		pOldBitmap = m_dcBk.SelectObject(&bmp);
		m_dcBk.BitBlt(0, 0, uiWidth, uiHeight, &clDC, rect1.left, rect1.top, SRCCOPY);
		bmp.DeleteObject();
	}

	return NULL;
}

//=============================================================================
// paint the background
//=============================================================================
void CGdipButton::PaintBk(CDC *pDC)
{
	CRect rect;
	GetClientRect(rect);
	pDC->BitBlt(0, 0, rect.Width(), rect.Height(), &m_dcBk, 0, 0, SRCCOPY);
}

//=============================================================================
// paint the bitmap currently pointed to with m_pCurBtn
//=============================================================================
void CGdipButton::PaintBtn(CDC *pDC)
{
	CRect rect;
	GetClientRect(rect);
	pDC->BitBlt(0, 0, rect.Width(), rect.Height(), m_pCurBtn, 0, 0, SRCCOPY);
}

//=============================================================================
// set the control to owner draw
//=============================================================================
void CGdipButton::PreSubclassWindow()
{
	// Set control to owner draw
	ModifyStyle(0, BS_OWNERDRAW, SWP_FRAMECHANGED);

	CButton::PreSubclassWindow();
}

//=============================================================================
// disable double click 
//=============================================================================
BOOL CGdipButton::PreTranslateMessage(MSG* pMsg) 
{
	if (pMsg->message == WM_LBUTTONDBLCLK)
		pMsg->message = WM_LBUTTONDOWN;

	if (m_pToolTip != NULL)
	{
		if (::IsWindow(m_pToolTip->m_hWnd))
		{
			m_pToolTip->RelayEvent(pMsg);		
		}
	}

	return CButton::PreTranslateMessage(pMsg);
}


//=============================================================================
// overide the erase function
//=============================================================================
BOOL CGdipButton::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE;
}

//=============================================================================
// Paint the button depending on the state of the mouse
//=============================================================================
void CGdipButton::DrawItem(LPDRAWITEMSTRUCT lpDIS) 
{
	CDC* pDC = CDC::FromHandle(lpDIS->hDC);
	Update();

	BOOL bIsPressed = (lpDIS->itemState & ODS_SELECTED);
	BOOL bIsFocused = (lpDIS->itemState & ODS_FOCUS);
	BOOL bDisable=(lpDIS->itemState&ODS_DISABLED);

	if(bDisable)
	{//不激活状态
		m_pCurBtn = &m_dcGS;
	}
	else if(bIsPressed)
	{//鼠标按下状态
		m_pCurBtn = &m_dcStdP;
	}
	else if(m_bIsHovering)
	{//鼠标经过状态
		m_pCurBtn = &m_dcStdH;
	}
	else if (bIsFocused)
	{//获得焦点状态
		m_pCurBtn = &m_dcStdF;
	}
	else
	{//正常状态
		m_pCurBtn = &m_dcStd;
	}

	// paint the button
	PaintBtn(pDC);
	CDC::DeleteTempMap();  //add lq 2011-12-16
}

//=============================================================================
LRESULT CGdipButton::OnMouseHover(WPARAM wparam, LPARAM lparam) 
//=============================================================================
{
	m_bIsHovering = TRUE;
	Invalidate();
	DeleteToolTip();

	// Create a new Tooltip with new Button Size and Location
	SetToolTipText(m_tooltext);

	if (m_pToolTip != NULL)
	{
		if (::IsWindow(m_pToolTip->m_hWnd))
		{
			//Display ToolTip
			m_pToolTip->Update();
		}
	}

	return 0;
}


//=============================================================================
LRESULT CGdipButton::OnMouseLeave(WPARAM wparam, LPARAM lparam)
//=============================================================================
{
	m_bIsTracking = FALSE;
	m_bIsHovering = FALSE;
	Invalidate();
	return 0;
}

//=============================================================================
void CGdipButton::OnMouseMove(UINT nFlags, CPoint point) 
//=============================================================================
{
	if (!m_bIsTracking)
	{
		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof(tme);
		tme.hwndTrack = m_hWnd;
		tme.dwFlags = TME_LEAVE|TME_HOVER;
		tme.dwHoverTime = 1;
		m_bIsTracking = _TrackMouseEvent(&tme);
	}
	
	CButton::OnMouseMove(nFlags, point);
}

//=============================================================================
//	
//	Call this member function with a memory DC from the code that paints 
//	the parents background.  Passing the screen DC defeats the purpose of 
//  using this function.
//
//=============================================================================
void CGdipButton::SetBkGnd(CDC* pDC)
{
	CRect rect, rectS;
	CBitmap bmp, *pOldBitmap=NULL;

	GetClientRect(rect);
	GetWindowRect(rectS);
	GetParent()->ScreenToClient(rectS);

	m_dcBk.DeleteDC();

	m_dcBk.CreateCompatibleDC(pDC);
	bmp.CreateCompatibleBitmap(pDC, rect.Width(), rect.Height());
	pOldBitmap = m_dcBk.SelectObject(&bmp);
	m_dcBk.BitBlt(0, 0, rect.Width(), rect.Height(), pDC, rectS.left, rectS.top, SRCCOPY);
	bmp.DeleteObject();
}


//=============================================================================
// Set the tooltip with a string resource
//=============================================================================
void CGdipButton::SetToolTipText(UINT nId, BOOL bActivate)
{
	// load string resource
	m_tooltext.LoadString(nId);

	// If string resource is not empty
	if (m_tooltext.IsEmpty() == FALSE)
	{
		SetToolTipText(m_tooltext, bActivate);
	}

}

//=============================================================================
// Set the tooltip with a CString
//=============================================================================
void CGdipButton::SetToolTipText(CString spText, BOOL bActivate)
{
	// We cannot accept NULL pointer
	if (spText.IsEmpty()) return;

	// Initialize ToolTip
	InitToolTip();
	m_tooltext = spText;

	// If there is no tooltip defined then add it
	if (m_pToolTip->GetToolCount() == 0)
	{
		CRect rectBtn; 
		GetClientRect(rectBtn);
		m_pToolTip->AddTool(this, m_tooltext, rectBtn, 1);
	}

	// Set text for tooltip
	m_pToolTip->UpdateTipText(m_tooltext, this, 1);
	m_pToolTip->SetDelayTime(2000);
	m_pToolTip->Activate(bActivate);
}

//=============================================================================
void CGdipButton::InitToolTip()
//=============================================================================
{
	if (m_pToolTip == NULL)
	{
		m_pToolTip = new CToolTipCtrl;
		// Create ToolTip control
		m_pToolTip->Create(this);
		m_pToolTip->Activate(TRUE);
	}
} 

//=============================================================================
void CGdipButton::DeleteToolTip()
//=============================================================================
{
	// Destroy Tooltip incase the size of the button has changed.
	if (m_pToolTip != NULL)
	{
		delete m_pToolTip;
		m_pToolTip = NULL;
	}
}

bool CGdipButton::IsNull()
{
	if (NULL == m_pStdImage)
	{
		return true;
	}
	if (Ok != m_pStdImage->GetLastStatus())
	{
		delete m_pStdImage;
		m_pStdImage = NULL;
		return true;
	}
	return false;
}

//重置按钮
void CGdipButton::ResetBT()
{
	if (NULL != m_dcBk.m_hDC)
	{
		m_dcBk.DeleteDC();
	}
	m_dcBk.m_hDC = NULL;
	SetDCNull();
	m_bIsResetDC = true;
	Invalidate();
	InvalidateRect(NULL);
}