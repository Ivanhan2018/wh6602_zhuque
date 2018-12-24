#pragma once

#include "ISonicUI.h"
using namespace sonic_ui;
#include "SimpleToolTip.h"
#include "ISonicBaseData.h"

// wParam:CSonicString *
// HIWORD(lParam):EVENT
// LOWORD(lParam):SysParam
#define WM_SONIC_INTERNAL_EVENT	WM_USER + 0x8000

// wParam:ISonicBase * 
// lParam:NULL
#define WM_SONIC_DETACH			WM_USER + 0x8001

// wParam:CSonicWndEffect *
// LOWORD(lParam):if redraw
// HIWORD(lParam):if move
#define WM_SONIC_UPDATE_DIB_LAYER	WM_USER + 0x8002

// wParam:0
// lParam:0
#define WM_SONIC_MOUSE_ENTER	WM_USER + 0x8004

// wParam:0
// lParam:0
#define WM_SONIC_MOUSE_LEAVE	WM_USER + 0x8005

typedef list<ISonicBase *>  LIST_SONIC_BASE;

enum enMouseStatus
{
	msNormal = 0,
	msHover,
	msNcHover,
};

typedef struct tagHwndData
{
	LIST_SONIC_BASE BaseList;
	WNDPROC	pOldProc;
	int nMouseStatus;
	int nHandCount;
	HCURSOR hOldCursor;
	tagHwndData() : pOldProc(0), nMouseStatus(0), hOldCursor(0), nHandCount(0){}
}HWND_DATA;

typedef map<HWND, HWND_DATA> HWND_TO_DATA;

class CSonicUI : public ISonicUI
{
public:
	CSonicUI();
	virtual ~CSonicUI();
	virtual ISonicSkin * CreateSkin();
	virtual ISonicImage * CreateImage();
	virtual ISonicWndEffect * CreateWndEffect();
	virtual ISonicString * CreateString();
	virtual ISonicTextScrollBar * CreateTextScrollBar();
	virtual ISonicPaint * CreatePaint();
	virtual ISonicAnimation * CreateAnimation();
	virtual void DestroyObject(ISonicBase * pBase);
	virtual void DestroyAttachedObject(HWND hWnd);
	virtual BOOL IsValidObject(const ISonicBase * pBase);
	virtual BOOL Attach(HWND hWnd, ISonicBase * pBase);
	virtual BOOL Detach(ISonicBase * pBase);
	virtual void DestroyAllObject();
	virtual BOOL IsUpdateLayeredWindow(HWND hWnd);
	virtual BOOL SetTipText(LPCTSTR lpszText, HWND hWnd, RECT &rt, ISonicBase * pBase);
	virtual BOOL ClearTip(ISonicBase * pBase);
	virtual BOOL CreateTip();
	virtual ISonicWndEffect * EffectFromHwnd(HWND hWnd);
	virtual ISonicSkin * SkinFromHwnd(HWND hWnd);
	virtual LPCTSTR HandleRawString(LPCTSTR lpszStr, int nType, LPCTSTR lpszUrlAttr = NULL);
	virtual BOOL DrawWindow(HDC hdc, HWND hWnd, BOOL bRecursion = TRUE);
	virtual HFONT GetDefaultFont();
	virtual BOOL DrawText(HDC hDC, int x, int y, LPCTSTR lpszString, DWORD dwColor = 0, HFONT hFont = NULL);

	BOOL RectInRect(LPRECT pRect1, LPRECT pRect2);
	BOOL IsCursorInWnd(HWND hWnd);
	void SetCursorHand(HWND hWnd, BOOL b);
	void SetCapture(ISonicBase * pBase);
	void ReleaseCapture();
	ISonicBase * GetCapture();
	BOOL Redraw(ISonicPaint * pPaint, RECT * pRt = NULL, BOOL bErase = TRUE);

	CRect m_rtUpdate;
	static HWND m_hWnd;
	static LRESULT CALLBACK InternalWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	static HWND_DATA * DataFromHwnd(HWND hWnd);
	static LRESULT CALLBACK CallOldProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	static BOOL Init();
	static void SetCallWndProcRet(int nRet);
protected:
	static LRESULT CALLBACK SonicWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	static HWND_TO_DATA m_mapHwndToData;
	static LIST_SONIC_BASE m_CreatedList;
	static int m_nRet;
	
	CSimpleToolTip m_Tooltip;
	ISonicBase * m_pOwnCapture;
	

	static HDC CALLBACK MyBeginPaint(
		HWND hwnd,            // handle to window
		LPPAINTSTRUCT lpPaint // paint information
		);
	static BOOL CALLBACK MyEndPaint(
		HWND hWnd,                  // handle to window
		CONST PAINTSTRUCT *lpPaint  // paint data
		);
	static LPVOID m_pOldBeginPaint;
	static LPVOID m_pOldEndPaint;
	static HDC m_hPaintDC;
};

extern CSonicUI g_UI;
extern HINSTANCE g_hDll;