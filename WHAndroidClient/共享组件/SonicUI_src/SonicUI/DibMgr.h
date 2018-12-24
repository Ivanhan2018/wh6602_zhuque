#pragma once

class CDibMgr
{
public:
	CDibMgr(void);
	CDibMgr(HDC hdc, int cx, int cy);
	~CDibMgr(void);
	BOOL IsValid(){return m_hMemDC != NULL;}
	void Clear();
	BOOL Create(HDC hdc, int cx, int cy);
	void Flush(DWORD dwVal);
	BOOL Resize(int cx, int cy, HBITMAP * pOld = NULL);
	HDC GetSafeHdc(){return m_hMemDC;}
	BYTE * GetBits(){return m_pBits;}
	const BITMAPINFO * GetBitmapInfo(){return (BITMAPINFO *)&m_bmpHeader;}
	int GetSize();
	int GetWidth();
	int GetHeight();
	HBITMAP GetHBitmap(){return m_hMemBmp;}
	BOOL Backup();
	BOOL Restore();
	BOOL ClearBackup();
	BOOL IsBackup();
protected:
	HDC m_hMemDC;
	HBITMAP m_hMemBmp;
	HBITMAP m_hOldBmp;
	BITMAPINFOHEADER m_bmpHeader;
	BYTE * m_pBits;
	BYTE * m_pBackup;
};
