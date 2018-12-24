#pragma once
#include "ISonicUI.h"
using namespace sonic_ui;
#include "ISonicBaseData.h"
#include "DibMgr.h"
#include "SonicUI.h"

typedef vector<PAINT_ITEM> LIST_PAINT_ITEM;

class CSonicPaint : public ISonicPaint, public ISonicBaseData
{
public:
	CSonicPaint(void);
	~CSonicPaint(void);
	virtual BOOL Create(BOOL bMemDib, int cx = 0, int cy = 0, HWND hWnd = NULL);
	virtual BOOL Draw(HDC hdc, int x = 0, int y = 0, HWND hWnd = NULL, RECT * pMemDCRect = NULL, int nAlpha = -1);
	virtual BOOL Resize(int cx = 0, int cy = 0);
	virtual BOOL AddObject(DWORD dwObjectId, int x = 0, int y = 0);
	virtual BOOL DelObject(DWORD dwObjectId, BOOL bByIndex = FALSE, BOOL bDestroyObject = FALSE);
	virtual BOOL SetObject(DWORD dwObjectId, int x = 0, int y = 0, BOOL bVisible = TRUE, BOOL bByIndex = FALSE);
	virtual int GetCount();
	virtual BOOL Backup();
	virtual BOOL Restore();
	virtual BOOL Redraw(BOOL bEraseBackground = TRUE);
	virtual int GetWidth();
	virtual int GetHeight();
	virtual HDC GetDC();
	virtual BYTE * GetBits();
	virtual int GetSize();
	virtual HBITMAP GetHBitmap();
	virtual BOOL Flush(DWORD dwColor);
	virtual BOOL IsValid();
	virtual void Clear();
	virtual const RECT * GetPaintRect();
	virtual CURRENT_PAINT * GetCurrentPaint();
	virtual BOOL ShowPaint(BOOL b, BOOL bRedraw = TRUE);
	virtual BOOL IsVisible();
	virtual const RECT * GetMsgRect();
	virtual BOOL ClearBackup();
	virtual BOOL IsBackup();
	virtual BOOL CloneFrom(const ISonicPaint * pPaint);
	virtual BOOL DelAllObject(BOOL bDestroyObject = FALSE);
	virtual PAINT_ITEM * GetPaintItem(DWORD dwObjectId, BOOL bByIndex = FALSE);
	virtual BOOL ShowObject(BOOL b, BOOL bRedraw, DWORD dwObjectId, BOOL bByIndex = FALSE);
	virtual BOOL EnableDrawingBackup(BOOL b);

public:
	void InternalDraw(HDC hdc, int x, int y, HWND hWnd, RECT * pMemDCRect);
protected:
	void InitValue();
	void AutoSize();
	BOOL IsValidItem(ISonicBase * pBase);

protected:
	CRect m_rtPaint;
	CRect m_rtMsg;
	CDibMgr m_Dib;
	LIST_PAINT_ITEM m_ItemList;
	BOOL m_bValid;
	BOOL m_bAutoWidth;
	BOOL m_bAutoHeight;
	BOOL m_bVisible;
	CURRENT_PAINT m_CurrPaint;
	BOOL m_bDrawingBackup;
};
