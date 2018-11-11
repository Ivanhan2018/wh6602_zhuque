#include "StdAfx.h"
#include ".\sonicpaint.h"

CSonicPaint::CSonicPaint(void)
{
	m_dwType = BASE_TYPE_PAINT;
	InitValue();
}

CSonicPaint::~CSonicPaint(void)
{
	Clear();
}

void CSonicPaint::InitValue()
{
	m_rtPaint.SetRect(0, 0, 0, 0);
	m_rtMsg.SetRect(0, 0, 0, 0);
	m_bAutoHeight = FALSE;
	m_bAutoWidth = FALSE;
	m_bValid = FALSE;
	memset(&m_CurrPaint, 0, sizeof(m_CurrPaint));
	m_bVisible = FALSE;
	m_bDrawingBackup = FALSE;
}

void CSonicPaint::Clear()
{
	if(IsValid() == FALSE)
	{
		return;
	}
	DelAllObject(FALSE);
	Redraw();
	m_Dib.Clear();
	m_ItemList.clear();
	g_UI.Detach(this);
	ClearBaseData();
	InitValue();
}

BOOL CSonicPaint::IsValid()
{
	return m_bValid;
}

BOOL CSonicPaint::Create(BOOL bMemDib, int cx /* = 0 */, int cy /* = 0 */, HWND hWnd /* = NULL */)
{
	if(IsValid())
	{
		Clear();
	}
	if(cx)
	{
		m_rtPaint.right = m_rtPaint.left + cx;
		m_bAutoWidth = FALSE;
	}
	else
	{
		m_bAutoWidth = TRUE;
	}
	if(cy)
	{
		m_rtPaint.bottom = m_rtPaint.top + cy;
		m_bAutoHeight = FALSE;
	}
	else
	{
		m_bAutoHeight = TRUE;
	}
	if(bMemDib)
	{
		cx = cx ? cx : 1;
		cy = cy ? cy : 1;
		HDC hdc = ::GetDC(GetDesktopWindow());
		m_Dib.Create(hdc, cx, cy);
		ReleaseDC(GetDesktopWindow(), hdc);
	}
	m_bValid = TRUE;
	m_bVisible = TRUE;
	m_hWnd = hWnd;
	return TRUE;
}

BOOL CSonicPaint::Draw(HDC hdc, int x /* = 0 */, int y /* = 0 */, HWND hWnd /* = NULL */, RECT * pMemDCRect /* = NULL */, int nAlpha /* = -1 */)
{
	if(m_hWnd == NULL)
	{
		m_hWnd = WindowFromDC(hdc);
		if(!m_hWnd)
		{
			m_hWnd = hWnd;
		}
		if(!m_hWnd)
		{
			return FALSE;
		}
	}

	if(m_bVisible == FALSE)
	{
		return FALSE;
	}
	m_rtPaint.MoveToXY(x, y);
	if(pMemDCRect)
	{
		m_rtPaint.OffsetRect(pMemDCRect->left, pMemDCRect->top);
	}
	CRect rtClient;
	GetClientRect(m_hWnd, rtClient);
	CRect rtUpdate = g_UI.m_rtUpdate;
	if(pMemDCRect)
	{
		m_rtMsg.IntersectRect(m_rtPaint, pMemDCRect);
	}
	else
	{
		m_rtMsg.IntersectRect(m_rtPaint, rtClient);
	}
	CRect rt;
	if(rt.IntersectRect(rtUpdate, m_rtPaint) == FALSE)
	{
		return FALSE;
	}
	
	if(m_Dib.IsValid())
	{
		if(m_bDrawingBackup)
		{
			if(m_Dib.IsBackup() == FALSE)
			{
				BitBlt(m_Dib.GetSafeHdc(), 0, 0, m_rtPaint.Width(), m_rtPaint.Height(), hdc, m_rtPaint.left, m_rtPaint.top, SRCCOPY);
				m_Dib.Backup();
			}
			else
			{
				m_Dib.Restore();
			}
		}
		InternalDraw(m_Dib.GetSafeHdc(), 0, 0, m_hWnd, m_rtPaint);
		if(nAlpha < 0)
		{
			BitBlt(hdc, x, y, m_rtPaint.Width(), m_rtPaint.Height(), m_Dib.GetSafeHdc(), 0, 0, SRCCOPY);
		}
		else
		{
			BLENDFUNCTION blend = {0};
			blend.AlphaFormat = HIWORD(nAlpha) ? 0 : AC_SRC_ALPHA;
			blend.SourceConstantAlpha = (BYTE)LOWORD(nAlpha);
			AlphaBlend(hdc, x, y, m_rtPaint.Width(), m_rtPaint.Height(), m_Dib.GetSafeHdc(), 0, 0, m_rtPaint.Width(), m_rtPaint.Height(), blend);
		}
	}
	else
	{
		InternalDraw(hdc, x, y, hWnd, pMemDCRect);
	}
	return TRUE;
}

BOOL CSonicPaint::Resize(int cx /* = 0 */, int cy /* = 0 */)
{
	if(IsValid() == FALSE)
	{
		return FALSE;
	}
	if(cx)
	{
		m_rtPaint.right = m_rtPaint.left + cx;
		m_bAutoWidth = FALSE;
	}
	else
	{
		m_bAutoWidth = TRUE;
	}
	if(cy)
	{
		m_rtPaint.bottom = m_rtPaint.top + cy;
		m_bAutoHeight = FALSE;
	}
	else
	{
		m_bAutoHeight = TRUE;
	}
	if(m_bAutoWidth == FALSE && m_bAutoHeight == FALSE)
	{
		if(m_Dib.IsValid())
		{
			m_Dib.Resize(m_rtPaint.Width(), m_rtPaint.Height());
		}
	}
	else
	{
		AutoSize();
	}
	return TRUE;
}

BOOL CSonicPaint::AddObject(DWORD dwObjectId, int x /* = 0 */, int y /* = 0 */)
{
	if(IsValid() == FALSE)
	{
		return FALSE;
	}
	ISonicBase * pBase = (ISonicBase *)(DWORD_PTR)dwObjectId;
	if(IsValidItem(pBase) == FALSE)
	{
		return FALSE;
	}
	PAINT_ITEM item = {pBase, x, y, TRUE};
	m_ItemList.push_back(item);
	AutoSize();
	Redraw();
	return TRUE;
}

BOOL CSonicPaint::DelObject(DWORD dwObjectId, BOOL bByIndex /* = FALSE */, BOOL bDestroyObject /* = FALSE */)
{
	if(IsValid() == FALSE)
	{
		return FALSE;
	}

	if(bByIndex == FALSE)
	{
		ISonicBase * pBase = (ISonicBase *)(DWORD_PTR)dwObjectId;
		if(IsValidItem(pBase) == FALSE)
		{
			return FALSE;
		}
		for(LIST_PAINT_ITEM::iterator it = m_ItemList.begin(); it != m_ItemList.end(); )
		{
			PAINT_ITEM & item = *it;
			if(IsValidItem(item.pBase) == FALSE)
			{
				it = m_ItemList.erase(it);
				continue;
			}
			if(item.pBase->GetObjectId() == dwObjectId)
			{
				switch (item.pBase->GetType())
				{
				case BASE_TYPE_PAINT:
					{
						ISonicPaint * pPaint = (ISonicPaint *)item.pBase;
						pPaint->ShowPaint(FALSE, TRUE);
					}
					break;
				case BASE_TYPE_STRING:
					{
						ISonicString * pStr = (ISonicString *)item.pBase;
						pStr->ShowString(FALSE, TRUE);
					}
					break;
				}
				if(bDestroyObject)
				{
					g_UI.DestroyObject(item.pBase);
				}
				m_ItemList.erase(it);
				AutoSize();
				Redraw();
				return TRUE;
			}
			it++;
		}
	}
	else
	{
		DWORD dwIndex = dwObjectId;
		if(dwIndex >= m_ItemList.size())
		{
			return FALSE;
		}
		PAINT_ITEM & item = m_ItemList[dwIndex];
		if(IsValidItem(item.pBase) == FALSE)
		{
			m_ItemList.erase(m_ItemList.begin() + dwIndex);
			return TRUE;
		}
		switch (item.pBase->GetType())
		{
		case BASE_TYPE_PAINT:
			{
				ISonicPaint * pPaint = (ISonicPaint *)item.pBase;
				pPaint->ShowPaint(FALSE, TRUE);
			}
			break;
		case BASE_TYPE_STRING:
			{
				ISonicString * pStr = (ISonicString *)item.pBase;
				pStr->ShowString(FALSE, TRUE);
			}
			break;
		}
		if(bDestroyObject)
		{
			g_UI.DestroyObject(item.pBase);
		}
		m_ItemList.erase(m_ItemList.begin() + dwIndex);
		AutoSize();
		Redraw();
		return TRUE;
	}
	return FALSE;
}

BOOL CSonicPaint::SetObject(DWORD dwObjectId, int x /* = 0 */, int y /* = 0 */, BOOL bVisible /* = TRUE */, BOOL bByIndex /* = FALSE */)
{
	if(IsValid() == FALSE)
	{
		return FALSE;
	}
	PAINT_ITEM * pItem = GetPaintItem(dwObjectId, bByIndex);
	if(pItem == NULL)
	{
		return FALSE;
	}
	pItem->x = x;
	pItem->y = y;
	pItem->bVisible = bVisible;
	AutoSize();
	Redraw();
	return TRUE;
}

int CSonicPaint::GetCount()
{
	return (int)m_ItemList.size();
}

BOOL CSonicPaint::Backup()
{
	return m_Dib.Backup();
}

BOOL CSonicPaint::Restore()
{
	return m_Dib.Restore();
}

BOOL CSonicPaint::Redraw(BOOL bEraseBackground /* = TRUE */)
{
	if(IsValid() == FALSE)
	{
		return FALSE;
	}
	if(m_hWnd == NULL)
	{
		return FALSE;
	}
	if(m_rtPaint.Width() == 0 || m_rtPaint.Height() == 0)
	{
		return FALSE;
	}
	return g_UI.Redraw(this, NULL, bEraseBackground);
}

int CSonicPaint::GetWidth()
{
	return m_rtPaint.Width();
}

int CSonicPaint::GetHeight()
{
	return m_rtPaint.Height();
}

HDC CSonicPaint::GetDC()
{
	return m_Dib.GetSafeHdc();
}

BYTE * CSonicPaint::GetBits()
{
	return m_Dib.GetBits();
}

int CSonicPaint::GetSize()
{
	return m_Dib.GetSize();
}

HBITMAP CSonicPaint::GetHBitmap()
{
	return m_Dib.GetHBitmap();
}

BOOL CSonicPaint::Flush(DWORD dwColor)
{
	if(m_Dib.IsValid() == FALSE)
	{
		return FALSE;
	}
	m_Dib.Flush(dwColor);
	return TRUE;
}

const RECT * CSonicPaint::GetPaintRect()
{
	if(IsValid() == FALSE)
	{
		return FALSE;
	}
	return &m_rtPaint;
}

void CSonicPaint::AutoSize()
{
	if(m_bAutoHeight == FALSE && m_bAutoWidth == FALSE)
	{
		return;
	}
	int cx = 0;
	int cy = 0;
	for(LIST_PAINT_ITEM::iterator it = m_ItemList.begin(); it != m_ItemList.end(); it++)
	{
		PAINT_ITEM &item = *it;
		switch (item.pBase->GetType())
		{
		case BASE_TYPE_STRING:
			{
				ISonicString * pStr = (ISonicString *)item.pBase;
				if(item.x + pStr->GetWidth() > cx)
				{
					cx = item.x + pStr->GetWidth();
				}
				if(item.y + pStr->GetHeight() > cy)
				{
					cy = item.y + pStr->GetHeight();
				}
			}
			break;
		case BASE_TYPE_IMAGE:
			{
				ISonicImage * pImg = (ISonicImage *)item.pBase;
				if(item.x + pImg->GetWidth() > cx)
				{
					cx = item.x + pImg->GetWidth();
				}
				if(item.y + pImg->GetHeight() > cy)
				{
					cy = item.y + pImg->GetHeight();
				}
			}
			break;
		case BASE_TYPE_PAINT:
			{
				ISonicPaint * pPaint = (ISonicPaint *)item.pBase;
				if(item.x + pPaint->GetWidth() > cx)
				{
					cx = item.x + pPaint->GetWidth();
				}
				if(item.y + pPaint->GetHeight() > cy)
				{
					cy = item.y + pPaint->GetHeight();
				}

			}
			break;
		}
	}
	if(m_bAutoWidth)
	{
		m_rtPaint.right = m_rtPaint.left + cx;
	}
	if(m_bAutoHeight)
	{
		m_rtPaint.bottom = m_rtPaint.top + cy;
	}
	if(m_Dib.IsValid())
	{
		m_Dib.Resize(m_rtPaint.Width(), m_rtPaint.Height());
	}
}

void CSonicPaint::InternalDraw(HDC hdc, int x, int y, HWND hWnd, RECT * pMemDCRect)
{
	for(LIST_PAINT_ITEM::iterator it = m_ItemList.begin(); it != m_ItemList.end(); it++)
	{
		PAINT_ITEM &item = *it;
		if(item.bVisible == FALSE)
		{
			continue;
		}
		switch (item.pBase->GetType())
		{
		case BASE_TYPE_STRING:
			{
				ISonicString * pStr = (ISonicString *)item.pBase;
				pStr->TextOut(hdc, x + item.x, y + item.y, hWnd, pMemDCRect);
			}
			break;
		case BASE_TYPE_IMAGE:
			{
				ISonicImage * pImg = (ISonicImage *)item.pBase;
				CRect rt = g_UI.m_rtUpdate;
				if(pMemDCRect)
				{
					rt.IntersectRect(rt, pMemDCRect);
					rt.OffsetRect(-pMemDCRect->left, -pMemDCRect->top);
				}
				CRect rtImg;
				rtImg.SetRect(0, 0, pImg->GetWidth(), pImg->GetHeight());
				rtImg.MoveToXY(x + item.x, y + item.y);
				if(rt.IntersectRect(rtImg, rt))
				{
					rt.OffsetRect(-rtImg.left, -rtImg.top);
					DRAW_PARAM dp;
					dp.dwMask = DP_SRC_CLIP;
					dp.rtSrc = rt;
					pImg->Draw(hdc, rtImg.left + rt.left, rtImg.top + rt.top, &dp);
				}
			}
			break;
		case BASE_TYPE_PAINT:
			{
				ISonicPaint * pPaint = (ISonicPaint *)item.pBase;
				pPaint->Draw(hdc, x + item.x, y + item.y, hWnd, pMemDCRect);
			}
			break;
		}
	}
	m_CurrPaint.hdc = hdc;
	m_CurrPaint.x = x;
	m_CurrPaint.y = y;
	if(pMemDCRect)
	{
		m_CurrPaint.rtMem = *pMemDCRect;
	}
	else
	{
		GetClientRect(m_hWnd, &m_CurrPaint.rtMem);
	}
	m_CurrPaint.hWnd = hWnd;
	FireDelegate(DELEGATE_EVENT_PAINT, TRUE);
	m_CurrPaint.hdc = NULL;
}

CURRENT_PAINT * CSonicPaint::GetCurrentPaint()
{
	if(m_CurrPaint.hdc)
	{
		return &m_CurrPaint;
	}
	else
	{
		return NULL;
	}
}

BOOL CSonicPaint::ShowPaint(BOOL b, BOOL bRedraw /* = TRUE */)
{
	if(IsValid() == FALSE)
	{
		return FALSE;
	}
	if(b == m_bVisible)
	{
		return FALSE;
	}
	for(LIST_PAINT_ITEM::iterator it = m_ItemList.begin(); it != m_ItemList.end(); )
	{
		PAINT_ITEM &item = *it;
		if(IsValidItem(item.pBase) == FALSE)
		{
			it = m_ItemList.erase(it);
			continue;
		}
		item.bVisible = b;
		if(item.pBase->GetType() == BASE_TYPE_PAINT)
		{
			ISonicPaint * pPaint = (ISonicPaint *)item.pBase;
			pPaint->ShowPaint(b, bRedraw);
		}
		else if(item.pBase->GetType() == BASE_TYPE_STRING)
		{
			ISonicString * pStr = (ISonicString *)item.pBase;
			pStr->ShowString(b, bRedraw);
		}
		it++;
	}
	if(bRedraw)
	{
		Redraw();
	}
	m_bVisible = b;
	return TRUE;
}

BOOL CSonicPaint::IsVisible()
{
	return m_bVisible;
}

const RECT * CSonicPaint::GetMsgRect()
{
	if(IsValid() == FALSE)
	{
		return FALSE;
	}
	return &m_rtMsg;
}

BOOL CSonicPaint::ClearBackup()
{
	return m_Dib.ClearBackup();
}

BOOL CSonicPaint::IsBackup()
{
	return m_Dib.IsBackup();
}

BOOL CSonicPaint::CloneFrom(const ISonicPaint * pPaint)
{
	if(IsValid())
	{
		Clear();
	}
	CSonicPaint * pSonicPaint = (CSonicPaint *)pPaint;
	m_rtPaint = pSonicPaint->m_rtPaint;
	if(pSonicPaint->m_Dib.IsValid())
	{
		m_Dib.Create(pSonicPaint->GetDC(), pSonicPaint->GetWidth(), pSonicPaint->GetHeight());
	}
	m_ItemList = pSonicPaint->m_ItemList;
	m_bAutoWidth = pSonicPaint->m_bAutoWidth;
	m_bAutoHeight = pSonicPaint->m_bAutoHeight;
	m_bVisible = pSonicPaint->m_bVisible;
	m_bValid = TRUE;
	return TRUE;
}

BOOL CSonicPaint::IsValidItem(ISonicBase * pBase)
{
	if(g_UI.IsValidObject(pBase) == FALSE)
	{
		return FALSE;
	}
	if(pBase->IsValid() == FALSE)
	{
		return FALSE;
	}
	if(pBase->GetType() != BASE_TYPE_STRING && pBase->GetType() != BASE_TYPE_IMAGE && pBase->GetType() != BASE_TYPE_PAINT)
	{
		return FALSE;
	}
	return TRUE;
}

BOOL CSonicPaint::DelAllObject(BOOL bDestroyObject /* = FALSE */)
{
	if(IsValid() == FALSE)
	{
		return FALSE;
	}
	while (m_ItemList.size())
	{
		DelObject(0, TRUE, bDestroyObject);
	}
	return TRUE;
}

PAINT_ITEM * CSonicPaint::GetPaintItem(DWORD dwObjectId, BOOL bByIndex /* = FALSE */)
{
	if(IsValid() == FALSE)
	{
		return NULL;
	}
	if(bByIndex == FALSE)
	{
		ISonicBase * pBase = (ISonicBase *)(DWORD_PTR)dwObjectId;
		if(IsValidItem(pBase) == FALSE)
		{
			return FALSE;
		}
		for(LIST_PAINT_ITEM::iterator it = m_ItemList.begin(); it != m_ItemList.end(); it++)
		{
			PAINT_ITEM & item = *it;
			if(item.pBase == pBase)
			{
				return &item;
			}
		}
	}
	else
	{
		if(dwObjectId >= m_ItemList.size())
		{
			return NULL;
		}
		return &m_ItemList[dwObjectId];
	}
	return NULL;
}

BOOL CSonicPaint::ShowObject(BOOL b, BOOL bRedraw, DWORD dwObjectId, BOOL bByIndex /* = FALSE */)
{
	if(IsValid() == FALSE)
	{
		return FALSE;
	}
	PAINT_ITEM * pItem = GetPaintItem(dwObjectId, bByIndex);
	if(pItem == NULL)
	{
		return FALSE;
	}
	switch (pItem->pBase->GetType())
	{
	case BASE_TYPE_STRING:
		{
			ISonicString * pStr = (ISonicString *)pItem->pBase;
			pStr->ShowString(b, bRedraw);
		}
		break;
	case BASE_TYPE_PAINT:
		{
			ISonicPaint * pPaint = (ISonicPaint *)pItem->pBase;
			pPaint->ShowPaint(b, bRedraw);
		}
		break;
	}
	pItem->bVisible = b;
	return TRUE;
}

BOOL CSonicPaint::EnableDrawingBackup(BOOL b)
{
	if(IsValid() == FALSE)
	{
		return FALSE;
	}
	m_bDrawingBackup = b;
	return TRUE;
}