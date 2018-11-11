#pragma once

#include "ISonicUI.h"
#include "GifHandler.h"
using namespace sonic_ui;
#include "DibMgr.h"
#include "ISonicBaseData.h"

class CSonicImage : public ISonicImage
{
public:
	CSonicImage(void);
	~CSonicImage(void);
	virtual BOOL Load(LPCTSTR lpszFileName);
	virtual BOOL Load(UINT nResId, HMODULE hResModule = NULL, LPCTSTR szResourceType = NULL);
	virtual BOOL Load(HDC hdc, int x, int y, int cx, int cy, BOOL bAlphaChannel = FALSE);
	virtual BOOL Draw(HDC hdc, int x = 0, int y = 0, DRAW_PARAM * pDp = NULL);
	virtual BOOL SetBitsToDevice(HDC hdc, int x = 0, int y = 0);
	virtual UINT GetFrameCount();
	virtual void SetCurrentFrame(int nIndex = -1);
	virtual UINT GetCurrentFrame();
	virtual UINT GetFrameDelay(int nIndex = -1);
	virtual void Clear();
	virtual int GetWidth();
	virtual int GetHeight();
	virtual BOOL CloneFrom(const ISonicImage * pImg);
	virtual BOOL IsValid();
	virtual BOOL Rotate(int nAngle, POINT * pScout = NULL);
	virtual BOOL SaveAsFile(LPCTSTR lpszFileName, enImageType imgType, int nQuality = 0);
	virtual BOOL Gray();
	virtual HRGN CreateRgn(DWORD dwColorKey = DEFAULT_COLOR_KEY, int x = 0, int y = 0, RECT * pRtSrc = NULL, BOOL bReverse = FALSE);
	virtual BOOL IsAlphaChannelValid();
	virtual DWORD GetPixel(int x, int y);
	virtual BOOL SetPixel(int x, int y, DWORD dwColor);
	virtual BYTE * GetBits();
	virtual RECT CalculateRectByDrawParam(int x, int y, const DRAW_PARAM * pDp, RECT * pSrc = NULL);
	virtual BOOL SetColorKey(DWORD dwColor);
	virtual BOOL Create(int cx, int cy, DWORD dwColor = 0, BOOL bWithAlpha = FALSE);
	virtual BOOL Flush(DWORD dwColor);
	virtual BOOL SetHsl(int h = -1, int s = -1, int l = -1);
	virtual BOOL MirrorX();
	virtual BOOL MirrorY();
	virtual BOOL Stretch(int cx, int cy);
	virtual BOOL Extend(int cx = 0, int cy = 0);
	virtual HDC GetDC();
	virtual BOOL DestroyAlphaChannel();
	virtual BOOL Blur(int nType = 0);
	virtual BOOL SetAlphaChannel(BYTE bAlpha);
    
	CDibMgr & GetImageDib(){return m_Dib;}
	static BOOL Init();
private:
	BOOL InternalDraw(HDC hdc, int x, int y, DRAW_PARAM * pDp); 
	virtual BOOL Load(HGLOBAL hGlobal, DWORD dwSize);
	BOOL PrepareMemDC(int cx, int cy);
	BOOL EnableAlphaChannel(BOOL bModifyRgb = TRUE);
	CGifHandler m_gif;
	BOOL m_bAlphaChannel;
	CDibMgr m_Dib;

	int m_nWidth;
	int m_nHeight;
};
