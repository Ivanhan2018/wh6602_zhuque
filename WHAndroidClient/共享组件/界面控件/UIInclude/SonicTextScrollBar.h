#pragma once
#include "ISonicUI.h"
using namespace sonic_ui;
#include "ISonicBaseData.h"
#include "DibMgr.h"

#define TIMER_SCROLL_TEXT	0x1
#define TIMER_PAUSE			0x2

class CSonicTextScrollBar : public ISonicTextScrollBar, public ISonicBaseData
{
public:
	typedef vector<ISonicPaint * > SONIC_PAINT_LIST;
public:
	CSonicTextScrollBar(void);
	~CSonicTextScrollBar(void);

	virtual BOOL Create(HWND hWnd, int x, int y, int cx, int cy, BOOL bVerScroll = TRUE, int nSpeed = 50, int nMidPause = 0);
	virtual BOOL AddText(DWORD dwObjectId);
	virtual BOOL DelText(int nIndex, BOOL bDestoryObject = FALSE);
	virtual BOOL DoScroll(BOOL bScroll = TRUE);
	virtual BOOL Move(int x, int y, int cx, int cy);
	virtual void Clear();
	virtual BOOL IsValid();
	virtual void OnInternalTimer(DWORD dwTimerId);
	virtual BOOL EnableOpt(DWORD dwOpt);
	virtual int GetCount();
	virtual BOOL Render(HDC hdc);
	virtual const RECT * GetRect();
	virtual BOOL ShowScroll(BOOL b, BOOL bRedraw = TRUE);
	virtual BOOL SetBGColor(DWORD dwColor);

protected:
	BOOL IsValidItem(DWORD dwObjectId);
	void InitValue();
	void SwitchTo(int nIndex = -1);
	void OnTimerScrollText();
	void OnTimerPause();
	void OnRender(ISonicPaint * pPaint, LPVOID);
protected:
	BOOL m_bColor;
	DWORD m_dwColor;
	CRect m_rtScroll;
	BOOL m_bVerScroll;
	int m_nSpeed;
	int m_nMidPause;
	BOOL m_bScroll;
	ISonicPaint * m_pBg;
	SONIC_PAINT_LIST m_PaintList;
	int m_nCurrStr;
	int m_nCurrX;
	int m_nCurrY;
	DWORD m_dwOpt;
};
