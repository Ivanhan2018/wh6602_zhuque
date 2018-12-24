#include "StdAfx.h"
#include ".\simpletooltip.h"
#include "commctrl.h"
#pragma comment(lib, "comctl32.lib")

CSimpleToolTip::CSimpleToolTip(void)
{
	m_hTipWnd = NULL;
	m_hFakeParent = NULL;
	m_dwCurrentHolder = 0;
}

CSimpleToolTip::~CSimpleToolTip(void)
{

}

BOOL CSimpleToolTip::Create(HWND hParent)
{
	INITCOMMONCONTROLSEX iccex; 

	

	/* INITIALIZE COMMON CONTROLS */
	iccex.dwICC = ICC_BAR_CLASSES;
	iccex.dwSize = sizeof(INITCOMMONCONTROLSEX);
	InitCommonControlsEx(&iccex);

	/* CREATE A TOOLTIP WINDOW */
	m_hTipWnd = CreateWindowEx(WS_EX_TOPMOST,
		TOOLTIPS_CLASS,
		NULL,
		WS_POPUP | TTS_ALWAYSTIP,		
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		hParent,
		NULL,
		NULL,
		NULL
		);

	SetWindowPos(m_hTipWnd,
		HWND_TOPMOST,
		0,
		0,
		0,
		0,
		SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
	SendMessage(m_hTipWnd, TTM_SETMAXTIPWIDTH, 0, 240);
	return TRUE;
}

void CSimpleToolTip::Destory()
{
	if(m_hTipWnd)
	{
		DestroyWindow(m_hTipWnd);
	}
}

void CSimpleToolTip::SetTipText(LPCTSTR lpszText, HWND hWnd, RECT& rt, DWORD dwCurrentHolder)
{
	TOOLINFO ti = {0};
	ti.cbSize = sizeof(ti) - sizeof(void *);
	if((m_hFakeParent && m_hFakeParent != hWnd)
		|| (m_dwCurrentHolder && m_dwCurrentHolder != dwCurrentHolder))
	{		
		// need to update the tooltip
		ti.hwnd = m_hFakeParent;
		ti.uId = 0;
		SendMessage(m_hTipWnd, TTM_DELTOOL, 0, (LPARAM)(LPTOOLINFO)&ti);
	}
	m_hFakeParent = hWnd;
	m_dwCurrentHolder = dwCurrentHolder;

	// struct specifying info about tool in ToolTip control

	/* INITIALIZE MEMBERS OF THE TOOLINFO STRUCTURE */
	ti.uFlags = TTF_SUBCLASS;
	ti.hwnd = m_hFakeParent;
	ti.hinst = NULL;
	ti.uId = 0;
	ti.lpszText = (LPTSTR)lpszText;

	// ToolTip control will cover the whole window
	ti.rect.left = rt.left;    
	ti.rect.top = rt.top;
	ti.rect.right = rt.right;
	ti.rect.bottom = rt.bottom;

	/* SEND AN ADDTOOL MESSAGE TO THE TOOLTIP CONTROL WINDOW */
	SendMessage(m_hTipWnd, TTM_ADDTOOL, 0, (LPARAM)(LPTOOLINFO)&ti);
	SetWindowPos(m_hTipWnd,
		HWND_TOPMOST,
		0,
		0,
		0,
		0,
		SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
}

void CSimpleToolTip::SetTipWidth(int nWidth)
{
	SendMessage(m_hTipWnd, TTM_SETMAXTIPWIDTH, 0, nWidth);
}

void CSimpleToolTip::ClearTip()
{
	TOOLINFO ti;
	ti.cbSize = sizeof(ti) - sizeof(void *);
	if(m_hFakeParent)
	{
		ti.hwnd = m_hFakeParent;
		ti.uId = 0;
		SendMessage(m_hTipWnd, TTM_DELTOOL, 0, (LPARAM)(LPTOOLINFO)&ti);
	}
	m_dwCurrentHolder = 0;
}