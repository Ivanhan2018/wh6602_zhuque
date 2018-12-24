#include "StdAfx.h"
#include ".\dibmgr.h"
#include "SSE.h"

CDibMgr::CDibMgr(void)
{
	m_hMemDC = NULL;
	m_hMemBmp = NULL;
	m_hOldBmp = NULL;
	m_pBits = NULL;
	m_pBackup = NULL;
	memset(&m_bmpHeader, 0, sizeof(m_bmpHeader));
}

CDibMgr::CDibMgr(HDC hdc, int cx, int cy)
{
	m_hMemDC = NULL;
	m_hMemBmp = NULL;
	m_hOldBmp = NULL;
	m_pBits = NULL;
	m_pBackup = NULL;
	memset(&m_bmpHeader, 0, sizeof(m_bmpHeader));
	Create(hdc, cx, cy);
}

CDibMgr::~CDibMgr(void)
{
	Clear();
}

void CDibMgr::Clear()
{
	memset(&m_bmpHeader, 0, sizeof(m_bmpHeader));
	if(m_hMemDC == NULL)
	{
		return;
	}
	if(m_hOldBmp)
	{
		SelectObject(m_hMemDC, m_hOldBmp);
	}
	if(m_hMemBmp)
	{
		DeleteObject(m_hMemBmp);
		m_hMemBmp = NULL;
		m_pBits = NULL;
	}
	DeleteDC(m_hMemDC);
	m_hMemDC = NULL;
	if(m_pBackup)
	{
		delete []m_pBackup;
		m_pBackup = NULL;
	}
}

int CDibMgr::GetSize()
{
	if(m_pBits == NULL)
	{
		return 0;
	}
	return m_bmpHeader.biWidth * m_bmpHeader.biHeight * 4;
}

int CDibMgr::GetWidth()
{
	if(IsValid() == FALSE)
	{
		return 0;
	}
	return m_bmpHeader.biWidth;
}

int CDibMgr::GetHeight()
{
	if(IsValid() == FALSE)
	{
		return 0;
	}
	return m_bmpHeader.biHeight;
}

void CDibMgr::Flush(DWORD dwVal)
{
	if(m_pBits == NULL)
	{
		return;
	}
	CSSE::MemSetDWord(m_pBits, dwVal, GetSize());
}

BOOL CDibMgr::Create(HDC hdc, int cx, int cy)
{
	if(m_hMemDC)
	{
		return FALSE;
	}
	BOOL bRet = FALSE;
	__try
	{
		m_hMemDC = CreateCompatibleDC(hdc);
		if(m_hMemDC == NULL)
		{
			__leave;
		}
		m_bmpHeader.biSize = sizeof(m_bmpHeader);
		m_bmpHeader.biBitCount = 32;
		m_bmpHeader.biCompression = BI_RGB;
		m_bmpHeader.biWidth = cx;
		m_bmpHeader.biHeight = cy;
		m_bmpHeader.biPlanes = 1;
		m_hMemBmp = CreateDIBSection(hdc, (BITMAPINFO *)&m_bmpHeader, DIB_RGB_COLORS, (void **)&m_pBits, NULL, 0);
		if(m_hMemBmp == NULL)
		{
			__leave;
		}
		m_hOldBmp = (HBITMAP)SelectObject(m_hMemDC, m_hMemBmp);
		bRet = TRUE;
	}
	__finally
	{
		if(!bRet)
		{
			Clear();
		}
	}
	return bRet;	
}

BOOL CDibMgr::Resize(int cx, int cy, HBITMAP * pOld /* = NULL */)
{
	if(IsValid() == FALSE)
	{
		return FALSE;
	}
	m_bmpHeader.biWidth = cx;
	m_bmpHeader.biHeight = cy;
	m_hMemBmp = CreateDIBSection(m_hMemDC, (BITMAPINFO *)&m_bmpHeader, DIB_RGB_COLORS, (void **)&m_pBits, NULL, 0);
	if(m_hMemBmp == NULL)
	{
		return FALSE;
	}
	HBITMAP hOldBmp = (HBITMAP)SelectObject(m_hMemDC, m_hMemBmp);
	if(pOld)
	{
		*pOld = hOldBmp;
	}
	else
	{
		DeleteObject(hOldBmp);
	}
	ClearBackup();
	return TRUE;
}

BOOL CDibMgr::Backup()
{
	if(IsValid() == FALSE)
	{
		return FALSE;
	}
	if(m_pBackup == NULL)
	{
		m_pBackup = new BYTE[GetSize()];
	}
	CSSE::MemCopy(m_pBackup, m_pBits, GetSize());
	return TRUE;
}

BOOL CDibMgr::Restore()
{
	if(IsValid() == FALSE)
	{
		return FALSE;
	}
	if(m_pBackup == NULL)
	{
		return FALSE;
	}
	CSSE::MemCopy(m_pBits, m_pBackup, GetSize());
	return TRUE;
}

BOOL CDibMgr::ClearBackup()
{
	if(IsValid() == FALSE)
	{
		return FALSE;
	}
	if(m_pBackup)
	{
		delete []m_pBackup;
		m_pBackup = NULL;
		return TRUE;
	}
	return FALSE;
}

BOOL CDibMgr::IsBackup()
{
	if(IsValid() == FALSE)
	{
		return FALSE;
	}
	return m_pBackup != NULL;
}