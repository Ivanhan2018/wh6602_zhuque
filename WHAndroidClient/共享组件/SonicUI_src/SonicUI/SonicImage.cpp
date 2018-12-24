#include "StdAfx.h"
#include ".\sonicimage.h"
#include "ximage.h"
#include "ConstDefine.h"
#include "SSE.h"
#include "SonicUI.h"
#include "shlobj.h"
#include "shlwapi.h"

int Trigonometry[361][2];
#define SIN(x) Trigonometry[(x)][0]
#define COS(x) Trigonometry[(x)][1]

CSonicImage::CSonicImage(void)
{
	m_dwType = BASE_TYPE_IMAGE;
	m_nWidth = 0;
	m_nHeight = 0;
	m_bAlphaChannel = FALSE;
}

CSonicImage::~CSonicImage(void)
{
	Clear();
}

BOOL CSonicImage::Load(LPCTSTR lpszFileName)
{
	Clear();
	HGLOBAL hGlobal;
	DWORD dwSize;

	FILE *fp = _tfopen(lpszFileName, _T("rb"));
	if(fp == NULL)
	{
		return FALSE;
	}
	fseek(fp, 0L, SEEK_END);
	dwSize = ftell(fp);
	rewind(fp);

	hGlobal = GlobalAlloc(GMEM_MOVEABLE | GMEM_NODISCARD,dwSize);
	if (!hGlobal)
	{
		fclose(fp);
		return FALSE;
	}

	char *pData = (char *)(GlobalLock(hGlobal));
	if (!pData)
	{
		GlobalFree(hGlobal);
		fclose(fp);
		return FALSE;
	};

	fread(pData, dwSize, 1, fp);
	GlobalUnlock(hGlobal);
	fclose(fp);

	BOOL bRetValue = Load(hGlobal,dwSize);
	GlobalFree(hGlobal);
	return bRetValue;
}

BOOL CSonicImage::Load(UINT nResId, HMODULE hResModule /* = NULL */, LPCTSTR szResourceType /* = NULL */)
{
	Clear();
	HRSRC hPicture;
	if(szResourceType == NULL)
	{
		hPicture = FindResource(hResModule, MAKEINTRESOURCE(nResId), IMAGE_NAME);
	}
	else
	{
		hPicture = FindResource(hResModule, MAKEINTRESOURCE(nResId), szResourceType);
	}
	HGLOBAL hResData;
	if (!hPicture || !(hResData = LoadResource(hResModule,hPicture)))
	{
		return FALSE;
	};
	DWORD dwSize = SizeofResource(hResModule,hPicture);

	// hResData is not the real HGLOBAL (we can't lock it)
	// let's make it real

	HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE | GMEM_NODISCARD, dwSize);
	if (!hGlobal)
	{
		FreeResource(hResData);
		return FALSE;
	};

	char *pDest = (char *) (GlobalLock(hGlobal));
	char *pSrc = (char *) (LockResource(hResData));
	if (!pSrc || !pDest)
	{
		GlobalFree(hGlobal);
		FreeResource(hResData);
		return FALSE;
	};
	memcpy(pDest,pSrc,dwSize);
	FreeResource(hResData);
	GlobalUnlock(hGlobal);

	BOOL bRetValue = Load(hGlobal,dwSize);
	GlobalFree(hGlobal);
	return bRetValue;
}

BOOL CSonicImage::Load(HGLOBAL hGlobal, DWORD dwSize)
{
	if(m_gif.LoadGif(hGlobal, dwSize) == 1)
	{		
		if(PrepareMemDC(m_gif.GetWidth(), m_gif.GetHeight()) == FALSE)
		{
			m_gif.Clear();
			return FALSE;
		}
		m_gif.Draw(m_Dib.GetSafeHdc());
	}
	else
	{
		BYTE * pData = (BYTE *)GlobalLock(hGlobal);
		CxImage img;
		img.Decode(pData, dwSize, 0);
		GlobalUnlock(hGlobal);
		if(PrepareMemDC(img.GetWidth(), img.GetHeight()) == FALSE)
		{
			img.Clear();
			return FALSE;
		}
		if(!img.AlphaIsValid())
		{
			img.Draw(m_Dib.GetSafeHdc());
			CSSE::DoOr(0xff000000, m_Dib.GetBits(), m_Dib.GetSize());
		}
		else
		{
			if(img.GetBpp() != 24)
			{
				img.Clear();
				return FALSE;
			}
			BYTE * pSrc = img.GetBits();
			BYTE * pAlpha = img.AlphaGetBits();
			BYTE * pMyBits = m_Dib.GetBits();
			int nLineTail = m_nWidth % 4;
			for(int i = 0; i < m_nHeight; i++)
			{
				for(int j = 0; j < m_nWidth; j++)
				{
					*pMyBits++ = *pSrc++;
					*pMyBits++ = *pSrc++;
					*pMyBits++ = *pSrc++;
					*pMyBits++ = *pAlpha++;
				}
				pSrc += nLineTail;
			}
			EnableAlphaChannel();
		}
		img.Clear();
	}	
	return TRUE;
}

BOOL CSonicImage::Draw(HDC hdc, int x /* = 0 */, int y /* = 0 */, DRAW_PARAM * pDp /* = NULL */)
{
	if(m_Dib.IsValid() == FALSE)
	{
		return FALSE;
	}

	if(pDp)
	{
		if(pDp->dwMask & DP_TILE)
		{
			DRAW_PARAM dp = *pDp;
			if(pDp->nTileLength == m_nWidth)
			{
				dp.dwMask &= ~DP_TILE;
				return InternalDraw(hdc, x, y, &dp);
			}
			dp.dwMask &= ~DP_VER_TILE;
			if(!dp.nTileDivider)
			{
				dp.nTileDivider = 4;
			}
			CRect rtSrc, rtDest;
			rtDest = CalculateRectByDrawParam(x, y, &dp, &rtSrc);
			int nEdgeWidth = rtSrc.Width() / dp.nTileDivider;
			int nMidWidth = rtSrc.Width() - nEdgeWidth * 2;
			if(dp.nTileLength < nEdgeWidth * 2)
			{
				dp.nTileLength = nEdgeWidth * 2;
			}
			dp.dwMask &= ~DP_TILE;
			dp.dwMask |= DP_SRC_CLIP;
			if(dp.dwMask & DP_SCALE)
			{
				dp.fScaleX = 1.0f;
			}
			// draw left edge
			SetRect(&dp.rtSrc, rtSrc.left, rtSrc.top, rtSrc.left + nEdgeWidth, rtSrc.bottom);
			dp.cx = nEdgeWidth;
			InternalDraw(hdc, rtDest.left, rtDest.top, &dp);

			// tile draw
			int x = rtDest.left + nEdgeWidth;
			int cx;
			do 
			{
				cx = rtDest.right - nEdgeWidth - x > nMidWidth ? nMidWidth : rtDest.right - nEdgeWidth - x;
				SetRect(&dp.rtSrc, rtSrc.left + nEdgeWidth, rtSrc.top, rtSrc.left + nEdgeWidth + cx, rtSrc.bottom);
				dp.cx = cx;
				InternalDraw(hdc, x, rtDest.top, &dp);
				x += cx;
			} while (x < rtDest.right - nEdgeWidth);

			// draw right edge
			SetRect(&dp.rtSrc, rtSrc.right - nEdgeWidth, rtSrc.top, rtSrc.right, rtSrc.bottom);
			dp.cx = nEdgeWidth;
			return InternalDraw(hdc, rtDest.right - nEdgeWidth, rtDest.top, &dp);
		}
		else if(pDp->dwMask & DP_VER_TILE)
		{
			DRAW_PARAM dp = *pDp;
			if(pDp->nTileLength == m_nHeight)
			{
				dp.dwMask &= ~DP_VER_TILE;
				return InternalDraw(hdc, x, y, &dp);
			}
			dp.dwMask &= ~DP_TILE;
			if(!dp.nTileDivider)
			{
				dp.nTileDivider = 4;
			}
			CRect rtSrc, rtDest;
			rtDest = CalculateRectByDrawParam(x, y, &dp, rtSrc);
			int nEdgeHeight = rtSrc.Height() / dp.nTileDivider;
			int nMidHeight = rtSrc.Height() - nEdgeHeight * 2;
			if(dp.nTileLength < nEdgeHeight * 2)
			{
				dp.nTileLength = nEdgeHeight * 2;
			}
			dp.dwMask &= ~DP_VER_TILE;
			dp.dwMask |= DP_SRC_CLIP;
			if(dp.dwMask & DP_SCALE)
			{
				dp.fScaleY = 1.0f;
			}
			// draw top edge
			SetRect(&dp.rtSrc, rtSrc.left, rtSrc.top, rtSrc.right, rtSrc.top + nEdgeHeight);
			dp.cy = nEdgeHeight;
			InternalDraw(hdc, rtDest.left, rtDest.top, &dp);

			// vertically tile draw
			int y = rtDest.top + nEdgeHeight;
			int cy;
			do 
			{
				cy = rtDest.bottom - nEdgeHeight - y > nMidHeight ? nMidHeight : rtDest.bottom - nEdgeHeight - y;
				SetRect(&dp.rtSrc, rtSrc.left, rtSrc.top + nEdgeHeight, rtSrc.right, rtSrc.top + nEdgeHeight + cy);
				dp.cy = cy;
				InternalDraw(hdc, rtDest.left, y, &dp);
				y += cy;
			} while (y < rtDest.bottom - nEdgeHeight);

			// draw bottom edge
			SetRect(&dp.rtSrc, rtSrc.left, rtSrc.bottom - nEdgeHeight, rtSrc.right, rtSrc.bottom);
			dp.cy = nEdgeHeight;
			return InternalDraw(hdc, rtDest.left, rtDest.bottom - nEdgeHeight, &dp);
		}
	}
	return InternalDraw(hdc, x, y, pDp);
}

BOOL CSonicImage::InternalDraw(HDC hdc, int x, int y, DRAW_PARAM * pDp)
{
	CRect rtSrc, rtDest;
	rtDest = CalculateRectByDrawParam(x, y, pDp, rtSrc);

	if(IsAlphaChannelValid() || (pDp && (pDp->dwMask & DP_ALPHA)))
	{
		BLENDFUNCTION blend;
		blend.AlphaFormat = AC_SRC_ALPHA;
		blend.BlendFlags = 0;
		blend.BlendOp = AC_SRC_OVER;
		blend.SourceConstantAlpha = (pDp && (pDp->dwMask & DP_ALPHA)) ? pDp->cAlpha : 255;
		AlphaBlend(hdc, rtDest.left, rtDest.top, rtDest.Width(), rtDest.Height(), m_Dib.GetSafeHdc(), rtSrc.left, rtSrc.top, rtSrc.Width(), rtSrc.Height(), blend);
	}
	else
	{
		if(pDp && (pDp->dwMask & DP_COLOR_KEY))
		{
			TransparentBlt(hdc, rtDest.left, rtDest.top, rtDest.Width(), rtDest.Height(), m_Dib.GetSafeHdc(), rtSrc.left, rtSrc.top, rtSrc.Width(), rtSrc.Height(), pDp->dwColorKey);
		}
		else if(pDp && (pDp->dwMask & DP_DEST_LIMIT || pDp->dwMask & DP_SCALE))
		{
			StretchBlt(hdc, rtDest.left, rtDest.top, rtDest.Width(), rtDest.Height(), m_Dib.GetSafeHdc(), rtSrc.left, rtSrc.top, rtSrc.Width(), rtSrc.Height(), SRCCOPY);
		}
		else
		{
			BitBlt(hdc, rtDest.left, rtDest.top, rtDest.Width(), rtDest.Height(), m_Dib.GetSafeHdc(), rtSrc.left, rtSrc.top, SRCCOPY);
		}
	}
	return TRUE;
}

UINT CSonicImage::GetFrameCount()
{
	return m_gif.GetFrameCount();
}

void CSonicImage::SetCurrentFrame(int nIndex /* = -1 */)
{
	m_gif.SetFrame(nIndex);
	m_gif.Draw(m_Dib.GetSafeHdc());
}

UINT CSonicImage::GetCurrentFrame()
{
	return m_gif.GetCurrentFrame();
}

UINT CSonicImage::GetFrameDelay(int nIndex /* = -1 */)
{
	return m_gif.GetFrameDelay(nIndex);
}

void CSonicImage::Clear()
{
	m_bAlphaChannel = FALSE;
	m_gif.Clear();
	m_nHeight = 0;
	m_nHeight = 0;
	m_Dib.Clear();
}

BOOL CSonicImage::PrepareMemDC(int cx, int cy)
{
	BOOL bRet = FALSE;
	if(cx <= 0 || cy <= 0)
	{
		return FALSE;
	}
	m_nWidth  = cx;
	m_nHeight = cy;
	HDC hdc = ::GetDC(CSonicUI::m_hWnd);
	if(m_Dib.IsValid())
	{
		m_Dib.Clear();
	}	
	bRet = m_Dib.Create(hdc, cx, cy);
	ReleaseDC(CSonicUI::m_hWnd, hdc);
	return bRet;
}

int CSonicImage::GetWidth()
{
	return m_nWidth;
}

int CSonicImage::GetHeight()
{
	return m_nHeight;
}

BOOL CSonicImage::CloneFrom(const ISonicImage * pImg)
{
	if(g_UI.IsValidObject(pImg) == FALSE)
	{
		return FALSE;
	}
	CSonicImage * pImage = (CSonicImage *)pImg;
	Clear();
	if(PrepareMemDC(pImage->GetWidth(), pImage->GetHeight()) == FALSE)
	{
		return FALSE;
	}
	if(pImage->IsAlphaChannelValid())
	{
		EnableAlphaChannel(FALSE);
	}
	memcpy(m_Dib.GetBits(), pImage->m_Dib.GetBits(), pImage->m_Dib.GetSize());
	return TRUE;
}

BOOL CSonicImage::IsValid()
{
	return m_Dib.IsValid();
}

BOOL CSonicImage::Rotate(int nAngle, POINT * pScout /* = NULL */)
{
	if(m_Dib.IsValid() == FALSE)
	{
		return FALSE;
	}
	nAngle = abs(nAngle) % 360;
	int nBitX = nAngle <= 180 ? 1 : 0;
	int nBitY = nAngle > 90 && nAngle <= 270 ? 1 : 0;
	int nSrcW = m_nWidth;
	int nSrcH = m_nHeight;
	const int nBytePixel = 4;
	BYTE * pSrc = m_Dib.GetBits();
	int nNewX = (nSrcW - nBitX) >> 1;
	int nNewY = (nSrcH - nBitY) >> 1;

	double m1 = -nSrcW / 2.0f;
	double m2 = nSrcW / 2.0f;
	double n1 = nSrcH / 2.0f;
	double n2 = -nSrcH / 2.0f;

	POINT pt1 = {(int)(m1 * COS(nAngle) + n1 * SIN(nAngle)) >> 10, (int)(-m1 * SIN(nAngle) + n1 * COS(nAngle)) >> 10};
	POINT pt2 = {(int)(m2 * COS(nAngle) + n1 * SIN(nAngle)) >> 10, (int)(-m2 * SIN(nAngle) + n1 * COS(nAngle)) >> 10};
	POINT pt3 = {(int)(m2 * COS(nAngle) + n2 * SIN(nAngle)) >> 10, (int)(-m2 * SIN(nAngle) + n2 * COS(nAngle)) >> 10};
	POINT pt4 = {(int)(m1 * COS(nAngle) + n2 * SIN(nAngle)) >> 10, (int)(-m1 * SIN(nAngle) + n2 * COS(nAngle)) >> 10};

	int i, j, x1, y1, x11, y11;
	i = abs(pt3.x - pt1.x);
	j = abs(pt4.x - pt2.x);
	int nDestW = max(i, j);
	i = abs(pt3.y - pt1.y);
	j = abs(pt4.y - pt2.y);
	int nDestH = max(i, j);

	m_nWidth = nDestW;
	m_nHeight = nDestH;
	HBITMAP hBmp = NULL;
	m_Dib.Resize(m_nWidth, m_nHeight, &hBmp);
	BYTE * pDest = m_Dib.GetBits();
	
	int temp;
	int nOriginX = nDestW >> 1;
	int nOriginY = nDestH >> 1;

	for(i = 0; i < nDestH; i++)
	{
		for(j = 0; j < nDestW; j++)
		{
			x1 = j - nOriginX;
			y1 = nOriginY - i;

			temp = (x1 * COS(nAngle) + y1 * SIN(nAngle)) >> 10;
			x11 = temp + nNewX;
			temp = (-x1 * SIN(nAngle) + y1 * COS(nAngle)) >> 10;
			y11 = nNewY - temp;
			if(x11 >= 0 && x11 < nSrcW && y11 >= 0 && y11 < nSrcH)
			{
				COLORREF color = *(DWORD *)(pSrc + (nSrcW * nBytePixel * y11) + x11 * nBytePixel);
				*(DWORD *)(pDest + i * nBytePixel * nDestW + j * nBytePixel) = color;
			}
		}
	}
	if(hBmp)
	{
		DeleteObject(hBmp);
	}
	if(pScout)
	{
		x1 = pScout->x - nNewX;
		y1 = nNewY - pScout->y;
        nAngle = 360 - nAngle;
		x11 = (x1 * COS(nAngle) + y1 * SIN(nAngle)) >> 10;
		x11 = x11 + nOriginX;
		y11 = (-x1 * SIN(nAngle) + y1 * COS(nAngle)) >> 10;
		y11 = nOriginY - y11;
		pScout->x = x11;
		pScout->y = y11;
	}
	EnableAlphaChannel(FALSE);
	return TRUE;
}

BOOL CSonicImage::SaveAsFile(LPCTSTR lpszFileName, enImageType imgType, int nQuality /* = 0 */)
{
	if(m_Dib.IsValid() == FALSE)
	{
		return FALSE;
	}
	CxImage img;

	if(img.CreateFromHBITMAP(m_Dib.GetHBitmap()) == FALSE)
	{
		return FALSE;
	}
	if(IsAlphaChannelValid()
		&& (imgType == IMAGE_TYPE_BMP
		|| imgType == IMAGE_TYPE_PNG
		|| imgType == IMAGE_TYPE_TGA
		|| imgType == IMAGE_TYPE_ICO))
	{
		img.AlphaCreate();
		for(int i = 0; i < m_nHeight; i++)
		{
			for(int j = 0; j < m_nWidth; j++)
			{
				BYTE bAlpha = *(m_Dib.GetBits() + i * m_nWidth * 4 + j * 4 + 3);
				img.AlphaSet(j, i, bAlpha);
			}
		}
	}
	DWORD dwType = 0;
	switch (imgType)
	{
	case IMAGE_TYPE_BMP:
		dwType = CXIMAGE_FORMAT_BMP;
		break;
	case IMAGE_TYPE_JPEG:
		dwType = CXIMAGE_FORMAT_JPG;
		nQuality = nQuality ? nQuality : DEFAULT_JPEG_QULITY;
		img.SetJpegQuality(nQuality);
		break;
	case IMAGE_TYPE_TGA:
		dwType = CXIMAGE_FORMAT_TGA;
		break;
	case IMAGE_TYPE_PNG:
		dwType = CXIMAGE_FORMAT_PNG;
		break;
	case IMAGE_TYPE_ICO:
		dwType = CXIMAGE_FORMAT_ICO;
		break;
	}
	CString strPath = lpszFileName;
	int nFind1 = strPath.ReverseFind(_T('\\'));
	int nFind2 = strPath.ReverseFind(_T('/'));
	int nFind = max(nFind1, nFind2);
	if(nFind != -1)
	{
		strPath = strPath.Left(nFind);
		if(PathFileExists(strPath) == FALSE)
		{
			SHCreateDirectoryEx(NULL, strPath, NULL);
		}
	}
#ifdef _UNICODE
    USES_CONVERSION;	
	if(img.Save(W2A(lpszFileName), dwType) == FALSE)
	{
		return FALSE;
	}
#else
	if(img.Save(lpszFileName, dwType) == FALSE)
	{
		return FALSE;
	}
#endif	
	img.Clear();
	return TRUE;
}

BOOL CSonicImage::Gray()
{
	if(m_Dib.IsValid() == FALSE)
	{
		return FALSE;
	}
	int nSrcSize = m_nWidth * m_nHeight * 4;
	CSSE::DoGray(m_Dib.GetBits(), nSrcSize);
	return TRUE;
}

HRGN CSonicImage::CreateRgn(DWORD dwColorKey /* = DEFAULT_COLOR_KEY */, int x /* = 0 */, int y /* = 0 */, RECT * pRtSrc /* = NULL */, BOOL bReverse /* = FALSE */)
{
	if(m_Dib.IsValid() == FALSE)
	{
		return FALSE;
	}

	int nBytePixel = 4;
	BYTE * pSrc = m_Dib.GetBits();

	CRect rtSrc(0, 0, m_nWidth, m_nHeight);
	if(pRtSrc)
	{
		rtSrc = *pRtSrc;
	}

	int nLeftx = -1;
	HRGN hAll = CreateRectRgn(x, y, x + rtSrc.Width(), y + rtSrc.Height());

	for(int i = rtSrc.top; i < rtSrc.bottom; i++)
	{
		for(int j = rtSrc.left; j < rtSrc.right; j++)
		{
			DWORD dwColor = *(DWORD *)(pSrc + (i * m_nWidth * nBytePixel) + j * nBytePixel);
			if(!bReverse)
			{
				if(EQUAL_24COLOR(dwColor, dwColorKey) && nLeftx < 0)
				{
					nLeftx = j;
				}
				if(nLeftx >= 0 && (!EQUAL_24COLOR(dwColor, dwColorKey) || j + 1 == rtSrc.right))
				{
					if(EQUAL_24COLOR(dwColor, dwColorKey) && j + 1 == rtSrc.right)j++;
					HRGN hTemp = CreateRectRgn(x + nLeftx - rtSrc.left, y + i - rtSrc.top, x + j - rtSrc.left, y + i + 1 - rtSrc.top);
					CombineRgn(hAll, hAll, hTemp, RGN_XOR);
					DeleteObject(hTemp);
					nLeftx = -1;
				}
			}
			else
			{
				if(!EQUAL_24COLOR(dwColor, dwColorKey) && nLeftx < 0)
				{
					nLeftx = j;
				}
				if(nLeftx >= 0 && (EQUAL_24COLOR(dwColor, dwColorKey) || j + 1 == rtSrc.right))
				{
					if(!EQUAL_24COLOR(dwColor, dwColorKey) && j + 1 == rtSrc.right)j++;
					HRGN hTemp = CreateRectRgn(x + nLeftx - rtSrc.left, y + i - rtSrc.top, x + j - rtSrc.left, y + i + 1 - rtSrc.top);
					CombineRgn(hAll, hAll, hTemp, RGN_XOR);
					DeleteObject(hTemp);
					nLeftx = -1;
				}
			}
		}
	}
	return hAll;
}

BOOL CSonicImage::IsAlphaChannelValid()
{
	return m_bAlphaChannel;
}

BOOL CSonicImage::EnableAlphaChannel(BOOL bModifyRgb /* = TRUE */)
{
	if(m_bAlphaChannel)
	{
		return FALSE;
	}
	if(m_Dib.IsValid() == FALSE)
	{
		return FALSE;
	}
	m_bAlphaChannel = TRUE;
	if(bModifyRgb)
	{
		int nSrc = m_nWidth * m_nHeight;
		DWORD * pSrc = (DWORD *)m_Dib.GetBits();
		for(int i = 0; i < nSrc; i++)
		{
			CSSE::AdjustAlpha(*pSrc);
			pSrc++;
		}
	}
	return TRUE;
}

DWORD CSonicImage::GetPixel(int x, int y)
{
	if(m_Dib.IsValid() == FALSE)
	{
		return 0;
	}
	if(y < 0 || y >= m_nHeight)
	{
		return 0;
	}
	if(x < 0 || x >= m_nWidth)
	{
		return 0;
	}
	return *(DWORD *)(m_Dib.GetBits() + (m_nHeight - y - 1) * m_nWidth * 4 + x * 4);
}

BOOL CSonicImage::SetPixel(int x, int y, DWORD dwColor)
{
	if(m_Dib.IsValid() == FALSE)
	{
		return 0;
	}
	if(IsAlphaChannelValid())
	{
		CSSE::AdjustAlpha(dwColor);
	}
	else
	{
		dwColor |= 0xff000000;
	}
	if(y < 0 || y >= m_nHeight)
	{
		return 0;
	}
	if(x < 0 || x >= m_nWidth)
	{
		return 0;
	}
	*(DWORD *)(m_Dib.GetBits() + (m_nHeight - y - 1) * m_nWidth * 4 + x * 4) = dwColor;
	return TRUE;
}

BYTE * CSonicImage::GetBits()
{
	return m_Dib.GetBits();
}

BOOL CSonicImage::Init()
{
	double radiant;
	for(int i = 0; i < 361; i++)
	{
		radiant = (i * PAI) / 180;
		double temp = sin(radiant) * 1024;
		Trigonometry[i][0] = ROUND(temp);
		temp = cos(radiant) * 1024;
		Trigonometry[i][1] = ROUND(temp);
	}
	return TRUE;
}

BOOL CSonicImage::SetBitsToDevice(HDC hdc, int x /* = 0 */, int y /* = 0 */)
{
	return SetDIBitsToDevice(hdc, x, y, m_nWidth, m_nHeight, 0, 0, 0, m_nHeight, m_Dib.GetBits(), m_Dib.GetBitmapInfo(), DIB_RGB_COLORS) != 0;
}

RECT CSonicImage::CalculateRectByDrawParam(int x, int y, const DRAW_PARAM * pDp, RECT * pSrc /* = NULL */)
{
	CRect rtSrc, rtDest;
	int cx = m_nWidth;
	int cy = m_nHeight;

	if(pDp && (pDp->dwMask & DP_SRC_CLIP))
	{
		rtSrc = pDp->rtSrc;
	}
	else
	{
		rtSrc.SetRect(0, 0, m_nWidth, m_nHeight);
	}
	cx = rtSrc.Width();
	cy = rtSrc.Height();
	if(pDp)
	{
		if(pDp->dwMask & DP_SCALE)
		{
			cx = (int)(rtSrc.Width() * pDp->fScaleX);
			cy = (int)(rtSrc.Height() * pDp->fScaleY);
		}
		if(pDp->dwMask & DP_DEST_LIMIT)
		{
			cx = pDp->cx;
			cy = pDp->cy;
		}
		if(pDp->dwMask & DP_TILE)
		{
			cx = pDp->nTileLength;
		}
		if(pDp->dwMask & DP_VER_TILE)
		{
			cy = pDp->nTileLength;
		}
	}
	rtDest.SetRect(x, y, x + cx, y + cy);
	if(pSrc)
	{
		*pSrc = rtSrc;
	}
	return rtDest;
}

BOOL CSonicImage::SetColorKey(DWORD dwColor)
{
	if(IsAlphaChannelValid())
	{
		return FALSE;
	}
	CSSE::SetColorKey(m_Dib.GetBits(), m_Dib.GetSize(), dwColor);
	EnableAlphaChannel(FALSE);
	return TRUE;
}

BOOL CSonicImage::Load(HDC hdc, int x, int y, int cx, int cy, BOOL bAlphaChannel /* = FALSE */)
{
	Clear();
	BITMAP bmp;
	GetObject((HBITMAP)GetCurrentObject(hdc, OBJ_BITMAP), sizeof(bmp), &bmp);
	if(bmp.bmType != 0 || bmp.bmPlanes != 1)
	{
		return FALSE;
	}
	if(bmp.bmBitsPixel != 32)
	{
		return FALSE;
	}

	if(PrepareMemDC(cx, cy) == FALSE)
	{
		return FALSE;
	}
	BitBlt(m_Dib.GetSafeHdc(), 0, 0, cx, cy, hdc, x, y, SRCCOPY);
	if(bAlphaChannel)
	{
		EnableAlphaChannel(FALSE);
		CSSE::DoOr(0xff000000, m_Dib.GetBits(), m_Dib.GetSize());
	}
	return TRUE;
}

BOOL CSonicImage::Create(int cx, int cy, DWORD dwColor /* = 0 */, BOOL bWithAlpha /* = FALSE */)
{
	if(PrepareMemDC(cx, cy) == FALSE)
	{
		return FALSE;
	}
	if(bWithAlpha)
	{
		CSSE::AdjustAlpha(dwColor);
		EnableAlphaChannel(FALSE);
	}
	if(dwColor)
	{
		Flush(dwColor);
	}
	return TRUE;
}

BOOL CSonicImage::Flush(DWORD dwColor)
{
	if(m_Dib.IsValid() == FALSE)
	{
		return FALSE;
	}
	CSSE::MemSetDWord(m_Dib.GetBits(), dwColor, m_Dib.GetSize());
	return TRUE;
}

BOOL CSonicImage::SetHsl(int h /* = -1 */, int s /* = -1 */, int l /* = -1 */)
{
	if(IsValid() == FALSE)
	{
		return FALSE;
	}
	CSSE::SetHSL(m_Dib.GetBits(), m_Dib.GetSize(), h, s, l);
	return TRUE;
}

BOOL CSonicImage::MirrorX()
{
	if(IsValid() == FALSE)
	{
		return FALSE;
	}
	CSSE::MirrorX(m_Dib.GetBits(), m_nWidth, m_nHeight);
	return TRUE;
}

BOOL CSonicImage::MirrorY()
{
	if(IsValid() == FALSE)
	{
		return FALSE;
	}
	CSSE::MirrorY(m_Dib.GetBits(), m_nWidth, m_nHeight);
	return TRUE;
}

BOOL CSonicImage::Stretch(int cx, int cy)
{
	if(IsValid() == FALSE)
	{
		return FALSE;
	}
	if(cx == m_nWidth && cy == m_nHeight)
	{
		return TRUE;
	}
	CSonicImage * pImg = (CSonicImage *)g_UI.CreateImage();
	pImg->CloneFrom(this);
	PrepareMemDC(cx, cy);
	CSSE::Stretch(pImg->m_Dib.GetBits(), pImg->GetWidth(), pImg->GetHeight(), m_Dib.GetBits(), cx, cy);
	g_UI.DestroyObject(pImg);
	return TRUE;
}

BOOL CSonicImage::Extend(int cx /* = 0 */, int cy /* = 0 */)
{
	if(IsValid() == FALSE)
	{
		return FALSE;
	}
	cx = (cx == 0 ? m_nWidth : cx);
	cy = (cy == 0 ? m_nHeight : cy);
	CSonicImage * pImg = (CSonicImage *)g_UI.CreateImage();
	pImg->CloneFrom(this);

	PrepareMemDC(cx, cy);
	for(int i = 0; i < cy; i += pImg->GetHeight())
	{
		for(int j = 0; j < cx; j += pImg->GetWidth())
		{
			pImg->SetBitsToDevice(m_Dib.GetSafeHdc(), j, i);
		}
	}
	g_UI.DestroyObject(pImg);
	return TRUE;
}

HDC CSonicImage::GetDC()
{
	if(IsValid() == FALSE)
	{
		return NULL;
	}
	return m_Dib.GetSafeHdc();
}

BOOL CSonicImage::DestroyAlphaChannel()
{
	if(IsValid() == FALSE)
	{
		return FALSE;
	}
	CSSE::DoAnd(0xffffff, (char *)m_Dib.GetBits(), m_Dib.GetSize());
	return TRUE;
}

BOOL CSonicImage::SetAlphaChannel(BYTE bAlpha)
{
	if(IsValid() == FALSE)
	{
		return FALSE;
	}
	CSSE::DoOr((bAlpha << 24), (char *)m_Dib.GetBits(), m_Dib.GetSize());
	EnableAlphaChannel(FALSE);
	return TRUE;
}

BOOL CSonicImage::Blur(int nType /* = 0 */)
{
	if(IsValid() == FALSE)
	{
		return FALSE;
	}

	char * pTemp = new char[m_Dib.GetSize()];
	CSSE::MemCopy(pTemp, m_Dib.GetBits(), m_Dib.GetSize());
	int i, j;
	DWORD * pSrc = (DWORD *)m_Dib.GetBits();
	DWORD * pDest = (DWORD *)pTemp;
	for(i = 1; i < m_Dib.GetHeight() - 1; i++)
	{
		for(j = 1; j < m_Dib.GetWidth() - 1; j++)
		{
			DWORD dw1 = *(pDest + i * m_Dib.GetWidth() + j);
			DWORD dw2 = *(pDest + i * m_Dib.GetWidth() + j - 1);
			DWORD dw3 = *(pDest + i * m_Dib.GetWidth() + j + 1);
			DWORD dw4 = *(pDest + (i - 1) * m_Dib.GetWidth() + j);
			DWORD dw5 = *(pDest + (i + 1) * m_Dib.GetWidth() + j);
			DWORD dw6 = *(pDest + (i - 1) * m_Dib.GetWidth() + j - 1);
			DWORD dw7 = *(pDest + (i - 1) * m_Dib.GetWidth() + j + 1);
			DWORD dw8 = *(pDest + (i + 1) * m_Dib.GetWidth() + j - 1);
			DWORD dw9 = *(pDest + (i + 1) * m_Dib.GetWidth() + j + 1);
			char r, g, b;
			r = g = b = 0;
			switch (nType)
			{
			case 0:
				{
					// gauss blur
					r = ((GetRValue(dw1) << 2) +
						((GetRValue(dw2) +
						GetRValue(dw3) +
						GetRValue(dw4) +
						GetRValue(dw5)) << 1) + 
						GetRValue(dw6) +
						GetRValue(dw7) +
						GetRValue(dw8) +
						GetRValue(dw9)) >> 4;
					g = ((GetGValue(dw1) << 2) +
						((GetGValue(dw2) +
						GetGValue(dw3) +
						GetGValue(dw4) +
						GetGValue(dw5)) << 1) + 
						GetGValue(dw6) +
						GetGValue(dw7) +
						GetGValue(dw8) +
						GetGValue(dw9)) >> 4;
					b = ((GetBValue(dw1) << 2) +
						((GetBValue(dw2) +
						GetBValue(dw3) +
						GetBValue(dw4) +
						GetBValue(dw5)) << 1) + 
						GetBValue(dw6) +
						GetBValue(dw7) +
						GetBValue(dw8) +
						GetBValue(dw9)) >> 4;
				}
				break;
			case 1:
				{
					// even blur
					r = (GetRValue(dw1) +
						GetRValue(dw2) +
						GetRValue(dw3) +
						GetRValue(dw4) +
						GetRValue(dw5) + 
						GetRValue(dw6) +
						GetRValue(dw7) +
						GetRValue(dw8) +
						GetRValue(dw9)) / 9;
					g = (GetGValue(dw1) +
						GetGValue(dw2) +
						GetGValue(dw3) +
						GetGValue(dw4) +
						GetGValue(dw5) + 
						GetGValue(dw6) +
						GetGValue(dw7) +
						GetGValue(dw8) +
						GetGValue(dw9)) / 9;
					b = (GetBValue(dw1) +
						GetBValue(dw2) +
						GetBValue(dw3) +
						GetBValue(dw4) +
						GetBValue(dw5) + 
						GetBValue(dw6) +
						GetBValue(dw7) +
						GetBValue(dw8) +
						GetBValue(dw9)) / 9;
				}
				break;
			}
			
			*(pSrc + i * m_Dib.GetWidth() + j) = (0xff000000 | RGB(r, g, b));
		}
	}

	delete []pTemp;
	return TRUE;
}