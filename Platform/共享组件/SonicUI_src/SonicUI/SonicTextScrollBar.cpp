#include "StdAfx.h"
#include ".\sonictextscrollbar.h"
#include "SonicUI.h"
#include "ConstDefine.h"

CSonicTextScrollBar::CSonicTextScrollBar(void)
{
	m_dwType = BASE_TYPE_TEXT_SCROLL_BAR;
	InitValue();
}

CSonicTextScrollBar::~CSonicTextScrollBar(void)
{
	Clear();
}

void CSonicTextScrollBar::InitValue()
{
	m_bColor = FALSE;
	m_dwColor = 0;
	m_bScroll = FALSE;
	m_bVerScroll = FALSE;
	m_nSpeed = 0;
	m_nMidPause = 0;
	m_nCurrStr = -1;
	m_nCurrX = 0;
	m_nCurrY = 0;
	m_dwOpt = 0;
	m_pBg = NULL;
	SwitchRender(TRUE);
}

void CSonicTextScrollBar::Clear()
{
	if(IsValid() == FALSE)
	{
		return;
	}
	g_UI.Detach(this);
	ClearBaseData();
	for(SONIC_PAINT_LIST::iterator it = m_PaintList.begin(); it != m_PaintList.end(); it++)
	{
		ISonicPaint * pPaint = *it;
		pPaint->DelAllObject(TRUE);
		g_UI.DestroyObject(*it);
	}
	m_PaintList.clear();
	g_UI.DestroyObject(m_pBg);
	InitValue();
}

BOOL CSonicTextScrollBar::IsValid()
{
	return m_hWnd != NULL;
}

BOOL CSonicTextScrollBar::Create(HWND hWnd, int x, int y, int cx, int cy, BOOL bVerScroll /* = TRUE */, int nSpeed /* = 50 */, int nMidPause /* = 0 */)
{
	Clear();
	m_hWnd = hWnd;
	if(cx == 0 || cy == 0)
	{
		return FALSE;
	}
	if(nSpeed <= 0)
	{
		return FALSE;
	}
	m_rtScroll.SetRect(x, y, x + cx, y + cy);
	m_bVerScroll = (bVerScroll != 0);
	m_nSpeed = ((nSpeed + BASE_INTERVAL - 1) / BASE_INTERVAL) * BASE_INTERVAL;
	m_nMidPause = ((nMidPause + BASE_INTERVAL - 1) / BASE_INTERVAL) * BASE_INTERVAL;
	m_pBg = g_UI.CreatePaint();
	m_pBg->Create(TRUE, m_rtScroll.Width(), m_rtScroll.Height(), hWnd);
	m_pBg->Delegate(DELEGATE_EVENT_PAINT, NULL, this, &CSonicTextScrollBar::OnRender);
	m_pBg->EnableDrawingBackup(TRUE);
	return g_UI.Attach(hWnd, this);
}

BOOL CSonicTextScrollBar::AddText(DWORD dwObjectId)
{
	if(IsValid() == FALSE)
	{
		return FALSE;
	}
	if(IsValidItem(dwObjectId) == FALSE)
	{
		return FALSE;
	}
	ISonicPaint * pPaint = g_UI.CreatePaint();
	pPaint->Create(FALSE);
	pPaint->AddObject(dwObjectId);
	m_PaintList.push_back(pPaint);
	return TRUE;
}

BOOL CSonicTextScrollBar::DelText(int nIndex, BOOL bDestoryObject /* = FALSE */)
{
	if(IsValid() == FALSE)
	{
		return FALSE;
	}
	if(nIndex < 0 || nIndex >= (int)m_PaintList.size())
	{
		return FALSE;
	}
	m_PaintList[nIndex]->DelAllObject(bDestoryObject);
	m_PaintList.erase(m_PaintList.begin() + nIndex);
	if(m_PaintList.size())
	{
		SwitchTo(nIndex);
	}
	return TRUE;
}

BOOL CSonicTextScrollBar::DoScroll(BOOL bScroll /* = TRUE */)
{
	if(IsValid() == FALSE)
	{
		return FALSE;
	}
	if(m_PaintList.empty())
	{
		return FALSE;
	}
	m_bScroll = bScroll;
	if(bScroll)
	{
		KillInternalTimer(TIMER_PAUSE);
		if(QueryInternalTimer(TIMER_SCROLL_TEXT) == FALSE)
		{
			SwitchTo(0);
			if((m_PaintList.size() == 1 && (m_dwOpt & TSBO_SINGLE_NO_SCROLL))
				|| (m_dwOpt & TSBO_FIRST_NO_SCROLL))
			{
				ISonicPaint * pPaint = m_PaintList.front();
				if(m_bVerScroll)
				{
					m_nCurrY = (m_rtScroll.Height() - pPaint->GetHeight()) / 2;
				}
				else
				{
					m_nCurrX = (m_rtScroll.Width() - pPaint->GetWidth()) / 2;
				}
				if(m_PaintList.size() == 1 && (m_dwOpt & TSBO_SINGLE_NO_SCROLL))
				{
					return TRUE;
				}
				if(m_nMidPause && (m_dwOpt & TSBO_FIRST_NO_SCROLL))
				{
					m_bScroll = FALSE;
					SetInternalTimer(TIMER_PAUSE, m_nMidPause, TRUE);
				}
			}
			SetInternalTimer(TIMER_SCROLL_TEXT, m_nSpeed);
		}
	}
	return TRUE;
}

BOOL CSonicTextScrollBar::Render(HDC hdc)
{
	if(IsValid() == FALSE)
	{
		return FALSE;
	}
	if(GetRenderSwitch() == FALSE)
	{
		return FALSE;
	}
	if(m_PaintList.empty() || m_nCurrStr < 0 || m_nCurrStr >= (int)m_PaintList.size())
	{
		return FALSE;
	}
	SwitchRender(FALSE);
	if(m_bColor)
	{
		m_pBg->Flush(m_dwColor);
	}
	return m_pBg->Draw(hdc, m_rtScroll.left, m_rtScroll.top, m_hWnd);
}

void CSonicTextScrollBar::OnTimerScrollText()
{
	if(!m_bScroll)
	{
		return;
	}
	if(m_nCurrStr < 0 || m_nCurrStr >= (int)m_PaintList.size())
	{
		return;
	}
	ISonicPaint * pPaint = m_PaintList[m_nCurrStr];
	if(m_bVerScroll)
	{
		m_nCurrY--;
		if(m_nCurrY == (m_rtScroll.Height() - pPaint->GetHeight()) / 2 && m_nMidPause)
		{
			m_bScroll = FALSE;
			SetInternalTimer(TIMER_PAUSE, m_nMidPause, TRUE);
		}
		if(pPaint->GetHeight() + m_nCurrY + 1 == 0)
		{
			SwitchTo();
		}
	}
	else
	{
		m_nCurrX--;
		if(m_nCurrX == (m_rtScroll.Width() - pPaint->GetWidth()) / 2 && m_nMidPause)
		{
			m_bScroll = FALSE;
			SetInternalTimer(TIMER_PAUSE, m_nMidPause, TRUE);
		}
		if(pPaint->GetWidth() + m_nCurrX  + 1 == 0)
		{
			SwitchTo();
		}
	}
	g_UI.Redraw(m_pBg, m_rtScroll, FALSE);
}

void CSonicTextScrollBar::OnInternalTimer(DWORD dwTimerId)
{
	if(dwTimerId & TIMER_SCROLL_TEXT)
	{
		OnTimerScrollText();
	}
	if(dwTimerId & TIMER_PAUSE)
	{
		OnTimerPause();
	}
}

void CSonicTextScrollBar::OnTimerPause()
{
	m_bScroll = TRUE;
}

void CSonicTextScrollBar::SwitchTo(int nIndex)
{
	if(m_PaintList.empty())
	{
		return;
	}
	if(nIndex >= (int)m_PaintList.size())
	{
		nIndex = nIndex % (int)m_PaintList.size();
	}
	if(nIndex < 0)
	{
		nIndex = (m_nCurrStr + 1) % (int)m_PaintList.size();
	}
	if(m_nCurrStr >= 0)
	{
		m_PaintList[m_nCurrStr]->ShowPaint(FALSE, FALSE);
	}
	m_nCurrStr = nIndex;
	ISonicPaint * pPaint = m_PaintList[m_nCurrStr];
	pPaint->ShowPaint(TRUE, FALSE);
	if(m_bVerScroll)
	{
		if(m_dwOpt & TSBO_LEFT_ALIGN)
		{
			m_nCurrX = 0;
		}
		else
		{
			m_nCurrX = (m_rtScroll.Width() - pPaint->GetWidth()) / 2;
		}
		m_nCurrY = m_rtScroll.Height() + 1;
	}
	else
	{
		m_nCurrX = m_rtScroll.Width() + 1;
		m_nCurrY = (m_rtScroll.Height() - pPaint->GetHeight()) / 2;
	}
}

BOOL CSonicTextScrollBar::Move(int x, int y, int cx, int cy)
{
	if(IsValid() == FALSE)
	{
		return FALSE;
	}
	m_rtScroll.SetRect(x, y, x + cx, y + cy);
	m_pBg->Resize(cx, cy);
	m_pBg->Redraw();
	return TRUE;
}

BOOL CSonicTextScrollBar::EnableOpt(DWORD dwOpt)
{
	if(IsValid() == FALSE)
	{
		return FALSE;
	}
	m_dwOpt |= dwOpt;
	return TRUE;
}

int CSonicTextScrollBar::GetCount()
{
	return (int)m_PaintList.size();
}

BOOL CSonicTextScrollBar::IsValidItem(DWORD dwObjectId)
{
	ISonicBase * pBase = (ISonicBase *)(DWORD_PTR)dwObjectId;
	if(g_UI.IsValidObject(pBase) == FALSE || pBase->IsValid() == FALSE)
	{
		return FALSE;
	}
	if(pBase->GetType() != BASE_TYPE_STRING && pBase->GetType() != BASE_TYPE_PAINT && pBase->GetType() != BASE_TYPE_IMAGE)
	{
		return FALSE;
	}
	return TRUE;
}

void CSonicTextScrollBar::OnRender(ISonicPaint * pPaint, LPVOID)
{
	ISonicPaint * pItem = m_PaintList[m_nCurrStr];
	HDC hdc = pPaint->GetCurrentPaint()->hdc;
	pItem->Draw(hdc, m_nCurrX, m_nCurrY, m_hWnd, m_rtScroll);
}

const RECT * CSonicTextScrollBar::GetRect()
{
	if(IsValid() == FALSE)
	{
		return NULL;
	}
	return &m_rtScroll;
}

BOOL CSonicTextScrollBar::ShowScroll(BOOL b, BOOL bRedraw /* = TRUE */)
{
	if(IsValid() == FALSE)
	{
		return FALSE;
	}
	m_pBg->ShowPaint(b, bRedraw);
	if(m_PaintList.size())
	{
		m_PaintList[m_nCurrStr]->ShowPaint(b, bRedraw);
	}
	return TRUE;
}

BOOL CSonicTextScrollBar::SetBGColor(DWORD dwColor)
{
	if(IsValid() == FALSE)
	{
		return FALSE;
	}
	m_bColor = TRUE;
	m_dwColor = dwColor;
	m_pBg->EnableDrawingBackup(FALSE);
	return TRUE;
}