#include "StdAfx.h"
#include "SonicUI.h"
#include ".\sonicskin.h"
#include "DibMgr.h"
#include "ConstDefine.h"

CSonicSkin::MAP_NAME_TO_CTRL CSonicSkin::m_mapName;
CSonicSkin::MAP_VALUE_TO_CTRL CSonicSkin::m_mapValue;

CSonicSkin::CSonicSkin(void)
{
	InitValue();
	m_dwType = BASE_TYPE_SKIN;
	m_pPaint = GetSonicUI()->CreatePaint();	
}

CSonicSkin::~CSonicSkin(void)
{
}

BOOL CSonicSkin::Init()
{
	// name
	m_mapName[_T("bg")] = &CSonicSkin::Name_bg;
	m_mapName[_T("style")] = &CSonicSkin::Name_style;
	m_mapName[_T("button")] = &CSonicSkin::Name_button;

	// value
	m_mapValue[_T("button_image")] = &CSonicSkin::Value_button_image;
	m_mapValue[_T("button_text_ctrl")] = &CSonicSkin::Value_button_text_ctrl;
	m_mapValue[_T("button_btn_ctrl")] = &CSonicSkin::Value_button_btn_ctrl;
	m_mapValue[_T("bg_image")] = &CSonicSkin::Value_bg_image;
	m_mapValue[_T("bg_close")] = &CSonicSkin::Value_bg_close;
	m_mapValue[_T("bg_min")] = &CSonicSkin::Value_bg_min;
	m_mapValue[_T("bg_max")] = &CSonicSkin::Value_bg_max;
	m_mapValue[_T("bg_restore")] = &CSonicSkin::Value_bg_restore;
	m_mapValue[_T("bg_icon")] = &CSonicSkin::Value_bg_icon;
	m_mapValue[_T("bg_top_height")] = &CSonicSkin::Value_bg_top_height;
	m_mapValue[_T("bg_title_height")] = &CSonicSkin::Value_bg_title_height;
	m_mapValue[_T("bg_title_text")] = &CSonicSkin::Value_bg_title_text;
	m_mapValue[_T("bg_color_key")] = &CSonicSkin::Value_bg_color_key;
	m_mapValue[_T("bg_btn_space")] = &CSonicSkin::Value_bg_btn_space;
	m_mapValue[_T("bg_btn_top_margin")] = &CSonicSkin::Value_bg_btn_top_margin;
	m_mapValue[_T("bg_btn_right_margin")] = &CSonicSkin::Value_bg_btn_right_margin;
	m_mapValue[_T("bg_enable_drag")] = &CSonicSkin::Value_bg_enable_drag;

	return TRUE;
}

BOOL CSonicSkin::IsValid()
{
	return (m_hWnd != NULL);
}

void CSonicSkin::Clear()
{
	m_bg.Clear();
	m_style.Clear();
	m_button.Clear();
	GetSonicUI()->DestroyObject(m_pPaint);
	InitValue();
}

void CSonicSkin::InitValue()
{
	m_pPaint = NULL;
}

BOOL CSonicSkin::Attach(HWND hWnd)
{
	m_hWnd = hWnd;

	if(g_UI.Attach(hWnd, this) == FALSE)
	{
		return FALSE;
	}

	// erase NC rect
	DWORD dwStyle = GetWindowLong(hWnd, GWL_STYLE);
	DWORD dwClsStyle = GetClassLong(hWnd, GCL_STYLE);
	DWORD dwExStyle = GetWindowLong(hWnd, GWL_EXSTYLE);
	SetClassLong(hWnd, GCL_STYLE, dwClsStyle | CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS);
	SetWindowLong(hWnd, GWL_STYLE, (dwStyle & ~WS_CAPTION & ~DS_MODALFRAME & ~DS_3DLOOK & ~DS_FIXEDSYS & ~DS_SETFONT) | WS_CLIPCHILDREN);
	SetWindowLong(hWnd, GWL_EXSTYLE, (dwExStyle & ~WS_EX_DLGMODALFRAME));
	SetWindowPos(hWnd, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOZORDER | SWP_DRAWFRAME | SWP_NOCOPYBITS);

	// initialize
	if(m_bg.pText->IsValid() == FALSE)
	{
		TCHAR szText[256];
		GetWindowText(m_hWnd, szText, Tsizeof(szText));
		m_bg.pText->Format(_T("%s"), szText);
	}
	else
	{
		SetWindowText(m_hWnd, m_bg.pText->GetTextWithoutCtrl());
	}

	CRect rtClient;
	GetClientRect(m_hWnd, &rtClient);
	m_pPaint->Create(FALSE, rtClient.Width(), rtClient.Height(), m_hWnd);
	return TRUE;
}

void CSonicSkin::SetRgn(RECT * pRect /* = NULL */, BOOL bDraw /* = TRUE */)
{
	if(m_bg.nColorKey < 0)
	{
		return;
	}

	static BOOL bEnter = FALSE;
	if(bEnter)
	{
		return;
	}
	bEnter = TRUE;

	CRect rtClient;
	
	if(pRect == NULL)
	{
		GetClientRect(m_hWnd, &rtClient);
	}
	else
	{
		rtClient = *pRect;
	}
	const int nCorner = 20;

	HRGN hOldRgn = m_bg.hMainRgn;
	m_bg.hMainRgn = CreateRectRgn(0, 0, rtClient.Width(),rtClient.Height());
	CRect rtSrc;
	// left top corner
	rtSrc.SetRect(0, 0, nCorner, nCorner);
	m_bg.hRgn[0] = m_bg.pImg->CreateRgn(m_bg.nColorKey, 0, 0, &rtSrc, TRUE);
	CombineRgn(m_bg.hMainRgn, m_bg.hMainRgn, m_bg.hRgn[0], RGN_XOR);

	// right top corner
	rtSrc.SetRect(m_bg.pImg->GetWidth() - nCorner, 0, m_bg.pImg->GetWidth(), nCorner);
	m_bg.hRgn[1] = m_bg.pImg->CreateRgn(m_bg.nColorKey, rtClient.Width() - nCorner, 0, &rtSrc, TRUE);
	CombineRgn(m_bg.hMainRgn, m_bg.hMainRgn, m_bg.hRgn[1], RGN_XOR);

	// left bottom corner
	rtSrc.SetRect(0, m_bg.pImg->GetHeight() - nCorner, nCorner, m_bg.pImg->GetHeight());
	m_bg.hRgn[2] = m_bg.pImg->CreateRgn(m_bg.nColorKey, 0, rtClient.Height() - nCorner, &rtSrc, TRUE);
	CombineRgn(m_bg.hMainRgn, m_bg.hMainRgn, m_bg.hRgn[2], RGN_XOR);

	// right bottom corner
	rtSrc.SetRect(m_bg.pImg->GetWidth() - nCorner, m_bg.pImg->GetHeight() - nCorner, m_bg.pImg->GetWidth(), m_bg.pImg->GetHeight());
	m_bg.hRgn[3] = m_bg.pImg->CreateRgn(m_bg.nColorKey, rtClient.Width() - nCorner, rtClient.Height() - nCorner, &rtSrc, TRUE);
	CombineRgn(m_bg.hMainRgn, m_bg.hMainRgn, m_bg.hRgn[3], RGN_XOR);

	SetWindowRgn(m_hWnd, m_bg.hMainRgn, bDraw);

	for(int i = 0; i < 4; i++)
	{
		if(m_bg.hRgn[i])
		{
			DeleteObject(m_bg.hRgn[i]);
			m_bg.hRgn[i] = NULL;
		}
	}
	if(hOldRgn)
	{
		DeleteObject(hOldRgn);
	}

	bEnter = FALSE;
}

ISonicPaint * CSonicSkin::GetPaint()
{
	return m_pPaint;
}

BOOL CSonicSkin::SetSkin(LPCTSTR lpszSkinName, LPCTSTR lpszValue, ...)
{
	if(lpszSkinName == NULL || lpszValue == NULL)
	{
		return FALSE;
	}

	try
	{
		TCHAR message[1024];
		va_list argList;
		va_start(argList, lpszValue);
		_vsntprintf(message, sizeof(message), lpszValue, argList);
		va_end(argList);

		CString strValue = message;
		CString strVName, strOne, strV;
		while (strValue.IsEmpty() == FALSE)
		{
			int nFind = strValue.Find(_T(';'));
			if(nFind >= 0)
			{
				strOne = strValue.Left(nFind);
				strValue = strValue.Mid(nFind + 1);
			}
			else
			{
				strOne = strValue;
				strValue.Empty();
			}
			strOne.TrimLeft();
			strOne.TrimRight();
			if(strOne.IsEmpty())
			{
				break;
			}
            
			nFind = strOne.Find(_T(':'));
			if(nFind >= 0)
			{
				strVName = strOne.Left(nFind);
				strV = strOne.Mid(nFind + 1);
				strV.TrimLeft(_T('\''));
				strV.TrimRight(_T('\''));
			}
			else
			{
				strVName = strOne;
				strV.Empty();
			}
			CString strCombine;
			strCombine.Format(_T("%s_%s"), lpszSkinName, strVName);
			MAP_VALUE_TO_CTRL::iterator pt = m_mapValue.find(strCombine);
			if(pt == m_mapValue.end())
			{
				throw 0;
			}
			(this->*(pt->second))(strVName, strV);
		}

		MAP_NAME_TO_CTRL::iterator it = m_mapName.find(lpszSkinName);
		if(it == m_mapName.end())
		{
			throw 0;
		}
		(this->*(it->second))(lpszSkinName);
	}
	catch(...)
	{
		return FALSE;
	}
	return TRUE;
}

void CSonicSkin::OnBtnClose(ISonicSkin *, LPVOID)
{
	SendMessage(m_hWnd, WM_SYSCOMMAND, SC_CLOSE, 0);
}

void CSonicSkin::OnBtnMaximize(ISonicSkin *, LPVOID)
{
	SendMessage(m_hWnd, WM_SYSCOMMAND, SC_MAXIMIZE, 0);
}

void CSonicSkin::OnBtnMinimize(ISonicSkin *, LPVOID)
{
	SendMessage(m_hWnd, WM_SYSCOMMAND, SC_MINIMIZE, 0);
}

void CSonicSkin::OnBtnRestore(ISonicSkin *, LPVOID)
{
	SendMessage(m_hWnd, WM_SYSCOMMAND, SC_RESTORE, 0);
}

BOOL CSonicSkin::OnWndMsg(UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
	case WM_DRAWITEM:
		{
			return OnDrawItem((LPDRAWITEMSTRUCT)lParam);
		}
		break;
	case WM_CTLCOLORSTATIC:
		{
			return OnCtlColorStatic((HDC)wParam, (HWND)lParam);
		}
		break;
	case WM_CTLCOLORBTN:
		{
			return OnCtlColorBtn((HDC)wParam, (HWND)lParam);
		}
		break;
	case WM_LBUTTONDBLCLK:
		{
			int x = LOWORD(lParam);
			int y = HIWORD(lParam);
			if(y < m_bg.wTitleHeight)
			{
				SendMessage(m_hWnd, WM_NCLBUTTONDBLCLK, HTCAPTION, 0);
				return FALSE;
			}
		}
		break;
	case WM_LBUTTONDOWN:
		{
			int x = LOWORD(lParam);
			int y = HIWORD(lParam);
			if(y < m_bg.wTitleHeight && m_bg.bEnableDrag)
			{
				SendMessage(m_hWnd, WM_NCLBUTTONDOWN, HTCAPTION, 0);
				return FALSE;
			}
		}
		break;
	case WM_NCHITTEST:
		{
			return OnNCHittest(LOWORD(lParam), HIWORD(lParam));
		}
		break;
	// assure the none-border window wont override the tray bar
	case WM_GETMINMAXINFO:
		{
			return OnGetMinMaxInfo((MINMAXINFO *)lParam);
		}
		break;
	case WM_WINDOWPOSCHANGING:
		{
			return OnPosChanging((LPWINDOWPOS)lParam);
		}
		break;
	case WM_WINDOWPOSCHANGED:
		{
			return OnPosChanged((LPWINDOWPOS)lParam);
		}
		break;
	case WM_ERASEBKGND:
		{
			return OnEraseBackground((HDC)wParam);
		}
		break;
	// erase NC paint
	case WM_NCACTIVATE:
		g_UI.SetCallWndProcRet(TRUE);
		return FALSE;
	case WM_NCPAINT:
		SetRgn();
	case WM_NCCALCSIZE:
		g_UI.SetCallWndProcRet(0);
		return FALSE;
	}
	return TRUE;
}

BOOL CSonicSkin::OnEraseBackground(HDC hdc)
{
	CRect rtUpdate = g_UI.m_rtUpdate;
	CRect rtClient;
	GetClientRect(m_hWnd, &rtClient);

	if(m_bg.pImg)
	{
		CDibMgr dib;
		dib.Create(hdc, rtClient.Width(), rtClient.Height());
		if(m_bg.pImg->GetWidth() == rtClient.Width() && m_bg.pImg->GetHeight() == rtClient.Height())
		{
			m_bg.pImg->Draw(dib.GetSafeHdc());
		}
		else
		{
			ISonicImage * pImg = GetSonicUI()->CreateImage();
			pImg->Create(m_bg.pImg->GetWidth(), rtClient.Height());

			int y = 0;
			DRAW_PARAM dp = {0};
			if(m_bg.wTopHeight)
			{
				dp.dwMask = DP_SRC_CLIP;
				SetRect(&dp.rtSrc, 0, 0, pImg->GetWidth(), m_bg.wTopHeight);
				m_bg.pImg->Draw(pImg->GetDC(), 0, 0, &dp);
				SetRect(&dp.rtSrc, 0, m_bg.wTopHeight, pImg->GetWidth(), m_bg.pImg->GetHeight());
				y += m_bg.wTopHeight;
			}
			dp.dwMask |= DP_VER_TILE;
			dp.nTileDivider = 5;
			dp.nTileLength = rtClient.Height() - m_bg.wTopHeight;
			m_bg.pImg->Draw(pImg->GetDC(), 0, y, &dp);

			dp.dwMask = DP_TILE;
			dp.nTileDivider = 6;
			dp.nTileLength = rtClient.Width();
			pImg->Draw(dib.GetSafeHdc(), 0, 0, &dp);
			GetSonicUI()->DestroyObject(pImg);
		}

		// after draw bg image
		if(m_bg.wTitleHeight)
		{
			int x = 5;
			int y = 0;
			// draw title icon
			if(m_bg.pIcon && m_bg.pIcon->IsValid())
			{
				m_bg.pIcon->Draw(dib.GetSafeHdc(), x, (m_bg.wTitleHeight - m_bg.pIcon->GetHeight()) / 2);
				x += m_bg.pIcon->GetWidth() + 5;
			}
			// draw title text
			if(m_bg.pText->IsValid())
			{
				m_bg.pText->TextOut(dib.GetSafeHdc(), x, (m_bg.wTitleHeight - m_bg.pText->GetHeight()) / 2, m_hWnd);
			}

			// draw close button
			x = rtClient.Width() - m_bg.wBtnRightMargin;
			y = m_bg.wBtnTopMargin;
			if(m_bg.pClose && m_bg.pClose->IsValid())
			{
				x -= m_bg.pClose->GetWidth();
				m_bg.pClose->TextOut(dib.GetSafeHdc(), x, y, m_hWnd);
				x -= m_bg.wBtnSpace;
			}
			DWORD dwStyle = GetWindowLong(m_hWnd, GWL_STYLE);
			if(dwStyle & WS_MAXIMIZEBOX)
			{
				if((dwStyle & WS_MAXIMIZE) && m_bg.pRestore && m_bg.pRestore->IsValid())
				{
					x -= m_bg.pRestore->GetWidth();
					m_bg.pRestore->TextOut(dib.GetSafeHdc(), x, y, m_hWnd);
					x -= m_bg.wBtnSpace;
				}
				else if(m_bg.pMax && m_bg.pMax->IsValid())
				{
					x -= m_bg.pMax->GetWidth();
					m_bg.pMax->TextOut(dib.GetSafeHdc(), x, y, m_hWnd);
					x -= m_bg.wBtnSpace;
				}
			}

			if((dwStyle & WS_MINIMIZEBOX) && m_bg.pMin && m_bg.pMin->IsValid())
			{
				x -= m_bg.pMax->GetWidth();
				m_bg.pMin->TextOut(dib.GetSafeHdc(), x, y, m_hWnd);
				x -= m_bg.wBtnSpace;
			}
		}

		m_pPaint->Draw(dib.GetSafeHdc());
		BitBlt(hdc, rtUpdate.left, rtUpdate.top, rtUpdate.Width(), rtUpdate.Height(), dib.GetSafeHdc(), 
			rtUpdate.left, rtUpdate.top, SRCCOPY);
		dib.Clear();

		return FALSE;
	}
	return TRUE;
}

int CSonicSkin::AutoAnylizeTitleHeight()
{
	if(m_bg.pImg == NULL)
	{
		return 0;
	}
	const int nScanX = 10;
	const int nScanCY = 50;
	if(m_bg.pImg->GetWidth() < nScanX || m_bg.pImg->GetHeight() < nScanCY)
	{
		return 0;
	}
	for(int j = 20; j < nScanCY - 1; j++)
	{
		DWORD dwColor1 = m_bg.pImg->GetPixel(nScanX, j);
		DWORD dwColor2 = m_bg.pImg->GetPixel(nScanX, j + 1);
		if(GetColorDifference(dwColor1, dwColor2) > 500)
		{
			return j + 1;
		}
	}
	return 0;
}

void CSonicSkin::InstallChildProc(HWND hChild)
{
	if(GetProp(hChild, SKIN_CHILD_PROC))
	{
		return;
	}
	LONG lOldProc = SetWindowLong(hChild, GWL_WNDPROC, (LONG)(LONG_PTR)ChildProc);
	if(lOldProc == 0)
	{
		return;
	}
	SetProp(hChild, SKIN_CHILD_PROC, (HANDLE)(LONG_PTR)lOldProc);
}


LRESULT CSonicSkin::ChildProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	WNDPROC pOld = (WNDPROC)GetProp(hWnd, SKIN_CHILD_PROC);
	if(pOld == NULL)
	{
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	switch (message)
	{
	case WM_NCDESTROY:
		{
			RemoveProp(hWnd, SKIN_CHILD_PROC);
		}
		break;
	case WM_ERASEBKGND:
		{
			return TRUE;
		}
		break;
	}
	return CallWindowProc(pOld, hWnd, message, wParam, lParam);
}

BOOL CSonicSkin::OnPosChanged(LPWINDOWPOS pPos)
{
	if(!(pPos->flags & SWP_NOSIZE))
	{
		m_pPaint->Resize(pPos->cx, pPos->cy);
	}
	return TRUE;
}

BOOL CSonicSkin::OnPosChanging(LPWINDOWPOS pPos)
{
	if(!(pPos->flags & SWP_NOSIZE))
	{
		m_pPaint->Resize(pPos->cx, pPos->cy);
	}
	return TRUE;
}

BOOL CSonicSkin::OnCtlColorBtn(HDC hdc, HWND hCtrl)
{
	if(m_button.pImg)
	{
		DWORD dwStyle = GetWindowLong(hCtrl, GWL_STYLE);
		if((dwStyle & 0xf) == BS_PUSHBUTTON || (dwStyle & 0xf) == BS_DEFPUSHBUTTON)
		{
			SetWindowLong(hCtrl, GWL_STYLE, (dwStyle & 0xffffff00) | BS_OWNERDRAW);
			InstallChildProc(hCtrl);

			BUTTON_PARAM::MAP_HWND_TO_STRING::iterator it = m_button.mapString.find(hCtrl);
			if(it == m_button.mapString.end())
			{
				// not record yet
				m_button.mapString[hCtrl] = GetSonicUI()->CreateString();
			}
		}
	}
	return TRUE;
}

BOOL CSonicSkin::OnCtlColorStatic(HDC hdc, HWND hCtrl)
{
	if(m_bg.pImg)
	{
		g_UI.SetCallWndProcRet((int)(INT_PTR)m_bg.hBrush);
	}
	return FALSE;
}

BOOL CSonicSkin::OnDrawItem(LPDRAWITEMSTRUCT pDI)
{
	if(m_button.pImg == NULL)
	{
		return TRUE;
	}
	ISonicString * pBtn = NULL;
	BUTTON_PARAM::MAP_HWND_TO_STRING::iterator it = m_button.mapString.find(pDI->hwndItem);
	if(it == m_button.mapString.end())
	{
		// no record before
		return TRUE;
	}
	pBtn = it->second;

	CRect rt;
	GetClientRect(pDI->hwndItem, &rt);

	TCHAR szText[256];
	GetWindowText(pDI->hwndItem, szText, Tsizeof(szText));
    
	if(pBtn->GetWidth() != rt.Width() || pBtn->GetHeight() != rt.Height())
	{
		// reformat if size changed
		pBtn->Format(_T("%s/a, p4=%d, pass_msg, btn_width=%d, btn_height=%d, btn_text=%s, btn_type=2%s/"), 
			m_button.strTextCtrl, m_button.pImg->GetObjectId(), rt.Width(), rt.Height(), szText, m_button.strBtnCtrl);
	}

	if(!IsWindowEnabled(pDI->hwndItem))
	{
		pBtn->Enable(FALSE);
	}
	else
	{
		pBtn->Enable(TRUE);
	}

	CDibMgr dib;
	dib.Create(pDI->hDC, rt.Width(), rt.Height());
	pBtn->TextOut(dib.GetSafeHdc(), 0, 0, pDI->hwndItem);
	BitBlt(pDI->hDC, 0, 0, rt.Width(), rt.Height(), dib.GetSafeHdc(), 0, 0, SRCCOPY);
	dib.Clear();
	return FALSE;
}


BOOL CSonicSkin::OnNCHittest(int x, int y)
{
	
	const int nCorner = 20;
	const int nSide = 3;

	int nRet = HTCLIENT;
	if(GetWindowLong(m_hWnd, GWL_STYLE) & WS_SIZEBOX)
	{		
		CRect rtWnd;
		GetWindowRect(m_hWnd, &rtWnd);
		if(x >= rtWnd.left && x <= rtWnd.left + nSide)
		{
			if(y >= rtWnd.top && y <= rtWnd.top + nCorner)
			{
				nRet = HTTOPLEFT;
			}
			else if(y >= rtWnd.bottom - nCorner && y <= rtWnd.bottom)
			{
				nRet = HTBOTTOMLEFT;
			}
			else
			{
				nRet = HTLEFT;
			}
		}
		if(x >= rtWnd.right - nSide && x <= rtWnd.right)
		{
			if(y >= rtWnd.top && y <= rtWnd.top + nCorner)
			{
				nRet = HTTOPRIGHT;
			}
			else if(y >= rtWnd.bottom - nCorner && y <= rtWnd.bottom)
			{
				nRet = HTBOTTOMRIGHT;
			}
			else
			{
				nRet = HTRIGHT;
			}
		}
		if(y >= rtWnd.top && y <= rtWnd.top + nSide)
		{
			if(x >= rtWnd.left && x <= rtWnd.left + nCorner)
			{
				nRet = HTTOPLEFT;
			}
			else if(x >= rtWnd.right - nCorner && x <= rtWnd.right)
			{
				nRet = HTTOPRIGHT;
			}
			else
			{
				nRet = HTTOP;
			}
		}
		if(y >= rtWnd.bottom - nSide && y <= rtWnd.bottom)
		{
			if(x >= rtWnd.left && x <= rtWnd.left + nCorner)
			{
				nRet = HTBOTTOMLEFT;
			}
			else if(x >= rtWnd.right - nCorner && x <= rtWnd.right)
			{
				nRet = HTBOTTOMRIGHT;
			}
			else
			{
				nRet = HTBOTTOM;
			}
		}
	}
	g_UI.SetCallWndProcRet(nRet);
	return FALSE;
}

BOOL CSonicSkin::OnGetMinMaxInfo(MINMAXINFO * pMMI)
{
	MONITORINFO mi;
	mi.cbSize = sizeof(MONITORINFO);
	GetMonitorInfo(MonitorFromWindow(m_hWnd, MONITOR_DEFAULTTONEAREST), &mi);
	pMMI->ptMaxPosition.x = mi.rcWork.left;
	pMMI->ptMaxPosition.y = mi.rcWork.top;
	pMMI->ptMaxSize.x = mi.rcWork.right - mi.rcWork.left;
	pMMI->ptMaxSize.y = mi.rcWork.bottom - mi.rcWork.top;

	return FALSE;
}

void CSonicSkin::Name_bg(LPCTSTR lpszSkinName)
{
	if(m_bg.wTitleHeight == 0)
	{
		m_bg.wTitleHeight = AutoAnylizeTitleHeight();
	}
	if(m_bg.wTopHeight)
	{
		if(m_bg.wTopHeight > m_bg.pImg->GetHeight())
		{
			m_bg.wTopHeight = 0;
		}
	}
	if(m_hWnd)
	{
		InvalidateRect(m_hWnd, NULL, TRUE);
	}
}

void CSonicSkin::Name_style(LPCTSTR lpszSkinName)
{
	if(m_hWnd)
	{
		InvalidateRect(m_hWnd, NULL, TRUE);
	}
}

void CSonicSkin::Name_button(LPCTSTR lpszSkinName)
{
	if(m_hWnd)
	{
		InvalidateRect(m_hWnd, NULL, TRUE);
	}
}

void CSonicSkin::Value_button_image(LPCTSTR lpszValueName, LPCTSTR lpszValue)
{
	_stscanf(lpszValue, _T("%d"), &m_button.pImg);
}

void CSonicSkin::Value_button_text_ctrl(LPCTSTR lpszValueName, LPCTSTR lpszValue)
{
	m_button.strTextCtrl.Format(_T("/global, %s/"), lpszValue);	
}

void CSonicSkin::Value_button_btn_ctrl(LPCTSTR lpszValueName, LPCTSTR lpszValue)
{
	m_button.strBtnCtrl = lpszValue;
	m_button.strBtnCtrl = _T(", ") + m_button.strBtnCtrl;
}

void CSonicSkin::Value_bg_image(LPCTSTR lpszValueName, LPCTSTR lpszValue)
{
	_stscanf(lpszValue, _T("%d"), &m_bg.pImg);

	if(m_bg.hBrush)
	{
		DeleteObject(m_bg.hBrush);
		m_bg.hBrush = NULL;
	}
	m_bg.hBrush = CreateSolidBrush(m_bg.pImg->GetPixel(m_bg.pImg->GetWidth() / 2, m_bg.pImg->GetHeight() / 2) & 0xffffff);
}

void CSonicSkin::Value_bg_btn_right_margin(LPCTSTR lpszValueName, LPCTSTR lpszValue)
{
	int nTemp;
	_stscanf(lpszValue, _T("%d"), &nTemp);
	m_bg.wBtnRightMargin = (WORD)nTemp;
}

void CSonicSkin::Value_bg_enable_drag(LPCTSTR lpszValueName, LPCTSTR lpszValue)
{
	int nTemp;
	_stscanf(lpszValue, _T("%d"), &nTemp);
	m_bg.bEnableDrag = (BYTE)nTemp;
}

void CSonicSkin::Value_bg_btn_space(LPCTSTR lpszValueName, LPCTSTR lpszValue)
{
	int nTemp;
	_stscanf(lpszValue, _T("%d"), &nTemp);
	m_bg.wBtnSpace = (WORD)nTemp;

}

void CSonicSkin::Value_bg_btn_top_margin(LPCTSTR lpszValueName, LPCTSTR lpszValue)
{
	int nTemp;
	_stscanf(lpszValue, _T("%d"), &nTemp);
	m_bg.wBtnTopMargin = (WORD)nTemp;
}

void CSonicSkin::Value_bg_close(LPCTSTR lpszValueName, LPCTSTR lpszValue)
{
	ISonicImage * pImg;
	_stscanf(lpszValue, _T("%d"), &pImg);
	
	if(!m_bg.pClose)
	{
		m_bg.pClose = GetSonicUI()->CreateString();
	}
	m_bg.pClose->Format(_T("/a, p4=%d, linkt='关闭', linkc=0/"), pImg);
	m_bg.pClose->Delegate(DELEGATE_EVENT_CLICK, NULL, this, &CSonicSkin::OnBtnClose);
}

void CSonicSkin::Value_bg_color_key(LPCTSTR lpszValueName, LPCTSTR lpszValue)
{
	_stscanf(lpszValue, _T("%d"), &m_bg.nColorKey);
}

void CSonicSkin::Value_bg_icon(LPCTSTR lpszValueName, LPCTSTR lpszValue)
{
	_stscanf(lpszValue, _T("%d"), &m_bg.pIcon);
}

void CSonicSkin::Value_bg_max(LPCTSTR lpszValueName, LPCTSTR lpszValue)
{
	ISonicImage * pImg;
	_stscanf(lpszValue, _T("%d"), &pImg);

	if(!m_bg.pMax)
	{
		m_bg.pMax = GetSonicUI()->CreateString();
	}
	m_bg.pMax->Format(_T("/a, p4=%d, linkt='最大化', linkc=0/"), pImg);
	m_bg.pMax->Delegate(DELEGATE_EVENT_CLICK, NULL, this, &CSonicSkin::OnBtnMaximize);
}

void CSonicSkin::Value_bg_min(LPCTSTR lpszValueName, LPCTSTR lpszValue)
{
	ISonicImage * pImg;
	_stscanf(lpszValue, _T("%d"), &pImg);

	if(!m_bg.pMin)
	{
		m_bg.pMin = GetSonicUI()->CreateString();
	}
	m_bg.pMin->Format(_T("/a, p4=%d, linkt='最小化', linkc=0/"), pImg);
	m_bg.pMin->Delegate(DELEGATE_EVENT_CLICK, NULL, this, &CSonicSkin::OnBtnClose);
}

void CSonicSkin::Value_bg_restore(LPCTSTR lpszValueName, LPCTSTR lpszValue)
{
	ISonicImage * pImg;
	_stscanf(lpszValue, _T("%d"), &pImg);

	if(!m_bg.pRestore)
	{
		m_bg.pRestore = GetSonicUI()->CreateString();
	}
	m_bg.pRestore->Format(_T("/a, p4=%d, linkt='恢复', linkc=0/"), pImg);
	m_bg.pRestore->Delegate(DELEGATE_EVENT_CLICK, NULL, this, &CSonicSkin::OnBtnRestore);
}

void CSonicSkin::Value_bg_title_height(LPCTSTR lpszValueName, LPCTSTR lpszValue)
{
	int nTemp;
	_stscanf(lpszValue, _T("%d"), &nTemp);
	m_bg.wTitleHeight = (WORD)nTemp;
}

void CSonicSkin::Value_bg_top_height(LPCTSTR lpszValueName, LPCTSTR lpszValue)
{
	int nTemp;
	_stscanf(lpszValue, _T("%d"), &nTemp);
	m_bg.wTopHeight = (WORD)nTemp;
}

void CSonicSkin::Value_bg_title_text(LPCTSTR lpszValueName, LPCTSTR lpszValue)
{
	m_bg.pText->Format(_T("%s"), lpszValue);
	if(m_hWnd)
	{
		SetWindowText(m_hWnd, m_bg.pText->GetTextWithoutCtrl());
	}
}