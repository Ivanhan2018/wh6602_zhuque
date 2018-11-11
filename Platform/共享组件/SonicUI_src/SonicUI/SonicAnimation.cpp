#include "StdAfx.h"
#include ".\sonicanimation.h"
#include "ConstDefine.h"
#include "math.h"
#include "SonicImage.h"

#define TIMER_SHOWING_GENTLY	0x1
#define TIMER_MOVE_GENTLY		0x2
#define TIMER_TRANSFORM			0x4

void CSonicAnimation::tagTransformParam::Clear()
{
	if(pImg)
	{
		g_UI.DestroyObject(pImg);
		pImg = NULL;
	}
	if(pImgOrigin)
	{
		g_UI.DestroyObject(pImgOrigin);
		pImgOrigin = NULL;
	}
	InitValue();
}

void CSonicAnimation::tagTransformParam::InitValue()
{
	pImg = NULL;
	pImgOrigin = NULL;
	nAngle = 0;
	bRestore = TRUE;
	nFrame = 0;
	nCurrFrame = 0;
}

void CSonicAnimation::tagShowParam::Clear()
{
	if(pImgOrigin)
	{
		g_UI.DestroyObject(pImgOrigin);
		pImgOrigin = NULL;
	}
	nCurrAlpha = 0;
	nAlphaStep = 0;
}

//////////////////////////////////////////////////////////////////////////
CSonicAnimation::CSonicAnimation(void)
{
	m_dwType = BASE_TYPE_ANIMATION;
	InitValue();
}

CSonicAnimation::~CSonicAnimation(void)
{
	Clear();
}

void CSonicAnimation::InitValue()
{
	m_pPaint = NULL;
	m_EffectMask = 0;
	m_rtAni.SetRect(0, 0, 0, 0);
	memset(&m_Move, 0, sizeof(m_Move));
	memset(&m_Show, 0, sizeof(m_Show));
	m_Transform.InitValue();
	m_nStatus = 0;
	SwitchRender(TRUE);
}

void CSonicAnimation::Clear()
{
	if(IsValid() == FALSE)
	{
		return;
	}
	m_Show.Clear();
	m_Transform.Clear();
	g_UI.DestroyObject(m_pPaint);
	m_pPaint = NULL;
	g_UI.Detach(this);
	ClearBaseData();
	InitValue();
}

BOOL CSonicAnimation::IsValid()
{
	return m_pPaint != NULL;
}

BOOL CSonicAnimation::Create(HWND hWnd, int x, int y, int cx, int cy)
{
	if(IsValid())
	{
		Clear();
	}
	if(cx == 0 || cy == 0)
	{
		return FALSE;
	}
	m_hWnd = hWnd;
	m_rtAni.SetRect(x, y, x + cx, y + cy);
	m_pPaint = g_UI.CreatePaint();
	m_pPaint->Create(FALSE, cx, cy, hWnd);
	return g_UI.Attach(hWnd, this);
}

BOOL CSonicAnimation::MoveGently(int x, int y, int nFrame, BOOL bEvenSpeed /* = FALSE */)
{
	if(IsValid() == FALSE)
	{
		return FALSE;
	}
	m_Move.bEven = bEvenSpeed;
	m_Move.nFrame = nFrame;
	m_Move.nCurrFrame = 0;
	m_Move.x = x;
	m_Move.y = y;
	m_Move.rtOrigin = m_rtAni;
	m_EffectMask |= ANI_STATUS_MOVE_GENTLY;
	if(nFrame == 0)
	{
		InternalMove();
	}
	else
	{		
		SetInternalTimer(TIMER_MOVE_GENTLY, ANIMATION_INTERVAL);
	}
	return TRUE;
}

BOOL CSonicAnimation::ShowGently(BOOL bShow, int nFrame)
{
	if(IsValid() == FALSE)
	{
		return FALSE;
	}
	if(nFrame > 255)
	{
		return FALSE;
	}
	if(nFrame == 0)
	{
		m_pPaint->ShowPaint(bShow);
	}
	else
	{
		m_EffectMask |= ANI_STATUS_SHOW_GENTYLE;
		m_Show.nAlphaStep = 255 / nFrame;
		if(bShow == FALSE)
		{
			m_Show.nAlphaStep = -m_Show.nAlphaStep;
			if(m_Show.nCurrAlpha == 0)
			{
				m_Show.nCurrAlpha = 255;
			}
		}
		else
		{
			if(m_pPaint->IsVisible() == FALSE)
			{
				m_pPaint->ShowPaint(TRUE, FALSE);
				m_Show.nCurrAlpha = 0;
			}
		}
		m_Show.pImgOrigin = SnapShot();
		for(int i = 0; i < m_pPaint->GetCount(); i++)
		{
			ShowObject(FALSE, FALSE, i, TRUE);
		}
		m_pPaint->Delegate(DELEGATE_EVENT_PAINT, NULL, this, &CSonicAnimation::OnDrawShowGently);
		SetInternalTimer(TIMER_SHOWING_GENTLY, ANIMATION_INTERVAL);
	}
	return TRUE;
}

BOOL CSonicAnimation::Transform(BOOL b, int nAngle /* = 0 */, RECT * pRect /* = NULL */, int nFrame /* = 0 */)
{
	if(IsValid() == FALSE)
	{
		return FALSE;
	}
	if((m_EffectMask & ANI_STATUS_TRANSFROM) && b)
	{
		return FALSE;
	}
	if(!(m_EffectMask & ANI_STATUS_TRANSFROM) && !b)
	{
		return FALSE;
	}
	m_Transform.nFrame = nFrame;
	if(b == FALSE)
	{
		m_Transform.bRestore = TRUE;
	}
	else
	{
		m_Transform.nCurrFrame = 0;
		m_Transform.nAngle = nAngle;
		if(pRect)
		{
			m_Transform.rtTrans = *pRect;
		}
		else
		{
			m_Transform.rtTrans.SetRect(0, 0, 0, 0);
		}
		m_Transform.pImgOrigin = SnapShot();
		m_Transform.bRestore = FALSE;
		m_Transform.rtOrigin = m_rtAni;
		m_Transform.pImg = g_UI.CreateImage();
		m_Transform.pImg->CloneFrom(m_Transform.pImgOrigin);
		for(int i = 0; i < m_pPaint->GetCount(); i++)
		{
			m_pPaint->ShowObject(FALSE, FALSE, i, TRUE);
		}
		m_pPaint->Delegate(DELEGATE_EVENT_PAINT, NULL, this, &CSonicAnimation::OnDrawTransform);
	}
	m_EffectMask |= ANI_STATUS_TRANSFROM;
	if(nFrame == 0)
	{
		InternalTransform();
	}
	else
	{
		SetInternalTimer(TIMER_TRANSFORM, GIF_INTERVAL);
	}
	return TRUE;
}

const RECT * CSonicAnimation::GetRect()
{
	if(IsValid() == FALSE)
	{
		return NULL;
	}
	return &m_rtAni;
}

BOOL CSonicAnimation::Redraw()
{
	if(IsValid() == FALSE)
	{
		return FALSE;
	}
	g_UI.Redraw(m_pPaint, m_rtAni, TRUE);
	return TRUE;
}

BOOL CSonicAnimation::Render(HDC hdc)
{
	if(IsValid() == FALSE)
	{
		return FALSE;
	}
	if(GetRenderSwitch() == FALSE)
	{
		return FALSE;
	}

	if(m_EffectMask & ANI_STATUS_MOVE_GENTLY)
	{
		if(m_Move.nCurrFrame >= m_Move.nFrame)
		{
			m_EffectMask &= ~ANI_STATUS_MOVE_GENTLY;
			FireDelegate(DELEGATE_EVENT_MOVEGENTLY_OVER);
		}
	}
	if(m_EffectMask & ANI_STATUS_SHOW_GENTYLE)
	{
		if(m_Show.nAlphaStep == 0)
		{
			m_EffectMask &= ~ANI_STATUS_SHOW_GENTYLE;
			FireDelegate(DELEGATE_EVENT_SHOWGENTLY_OVER);
		}
	}
	if(m_EffectMask & ANI_STATUS_TRANSFROM)
	{
		if(m_Transform.nCurrFrame >= m_Transform.nFrame)
		{			
			if(m_Transform.bRestore)
			{
				m_EffectMask &= ~ANI_STATUS_TRANSFROM;
			}
			FireDelegate(DELEGATE_EVENT_TRANSFORM_OVER);
			m_Transform.nCurrFrame = 0;
		}
	}
	SwitchRender(FALSE);
	return m_pPaint->Draw(hdc, m_rtAni.left, m_rtAni.top, m_hWnd, NULL);
}

void CSonicAnimation::InternalMove()
{
	g_UI.Redraw(m_pPaint, m_rtAni);
	if(m_Move.nCurrFrame >= m_Move.nFrame)
	{
		m_rtAni.MoveToXY(m_Move.x, m_Move.y);
	}
	else
	{
		if(m_Move.bEven)
		{
			int x = (m_Move.x - m_Move.rtOrigin.left) * m_Move.nCurrFrame / m_Move.nFrame + m_Move.rtOrigin.left;
			int y = (m_Move.y - m_Move.rtOrigin.top) * m_Move.nCurrFrame / m_Move.nFrame + m_Move.rtOrigin.top;
			m_rtAni.MoveToXY(x, y);
		}
		else
		{
			const int nMe = 2;
			double fAx = 2.f * (m_Move.x - m_Move.rtOrigin.left) / pow((double)m_Move.nFrame, nMe);
			double fAy = 2.f * (m_Move.y -m_Move.rtOrigin.top) / pow((double)m_Move.nFrame, nMe);
			double fV1 = fAx * m_Move.nFrame;
			double fV2 = fAy * m_Move.nFrame;
			fAx = -fAx;
			fAy = -fAy;
			double fStepX = fV1 * m_Move.nCurrFrame + 0.5 * fAx * pow((double)m_Move.nCurrFrame, nMe);
			double fStepY = fV2 * m_Move.nCurrFrame + 0.5 * fAy * pow((double)m_Move.nCurrFrame, nMe);
			int x = m_Move.rtOrigin.left + ROUND(fStepX);
			int y = m_Move.rtOrigin.top + ROUND(fStepY);
			m_rtAni.MoveToXY(x, y);
		}
	}
	g_UI.Redraw(m_pPaint, m_rtAni);
}

void CSonicAnimation::OnTimerMove()
{
	m_Move.nCurrFrame++;
	InternalMove();
	if(m_Move.nCurrFrame >= m_Move.nFrame)
	{
		KillInternalTimer(TIMER_MOVE_GENTLY);
	}
}

DWORD CSonicAnimation::GetAnimationStatus()
{
	if(IsValid() == FALSE)
	{
		return 0;
	}
	return m_EffectMask;
}

void CSonicAnimation::OnInternalTimer(DWORD dwTimerId)
{
	if(dwTimerId & TIMER_MOVE_GENTLY)
	{
		OnTimerMove();
	}
	if(dwTimerId & TIMER_SHOWING_GENTLY)
	{
		OnTimerShow();
	}
	if(dwTimerId & TIMER_TRANSFORM)
	{
        OnTimerTransform();
	}
}

void CSonicAnimation::OnTimerTransform()
{
	m_Transform.nCurrFrame++;
	InternalTransform();
	if(m_Transform.nCurrFrame >= m_Transform.nFrame)
	{
		KillInternalTimer(TIMER_TRANSFORM);
	}
}

void CSonicAnimation::OnTimerShow()
{
	m_Show.nCurrAlpha += m_Show.nAlphaStep;
	if(m_Show.nCurrAlpha >= 255)
	{
		m_Show.nCurrAlpha = 255;
		m_Show.nAlphaStep = 0;
		for(int i = 0; i < m_pPaint->GetCount(); i++)
		{
			m_pPaint->ShowObject(TRUE, FALSE, i, TRUE);
		}
		
	}
	if(m_Show.nCurrAlpha <= 0)
	{
		m_Show.nCurrAlpha = 0;
		m_pPaint->ShowPaint(FALSE);
		m_Show.nAlphaStep = 0;
	}
	if(m_Show.nAlphaStep == 0)
	{
		KillInternalTimer(TIMER_SHOWING_GENTLY);
		m_Show.Clear();
	}
    m_pPaint->Redraw();
}

void CSonicAnimation::InternalTransform()
{
	m_Transform.pImg->CloneFrom(m_Transform.pImgOrigin);
	CRect &rtOrigin = m_Transform.rtOrigin;
	if(m_Transform.nFrame == 0)
	{
		m_Transform.nCurrFrame = 1;
		m_Transform.nFrame = 1;
	}

	g_UI.Redraw(m_pPaint, m_rtAni);
	int x = rtOrigin.left;
	int y = rtOrigin.top;
	int cx = rtOrigin.Width();
	int cy = rtOrigin.Height();
	CRect &rtDest = m_Transform.rtTrans;
	if(rtDest.Width() && rtDest.Height())
	{		
		if(m_Transform.bRestore)
		{
			x = rtDest.left + (rtOrigin.left - rtDest.left) * m_Transform.nCurrFrame / m_Transform.nFrame;
			y = rtDest.top + (rtOrigin.top - rtDest.top) * m_Transform.nCurrFrame / m_Transform.nFrame;
			cx = (rtOrigin.Width() - rtDest.Width()) * m_Transform.nCurrFrame / m_Transform.nFrame + rtDest.Width();
			cy = (rtOrigin.Height() - rtDest.Height()) * m_Transform.nCurrFrame / m_Transform.nFrame + rtDest.Height();
		}
		else
		{
			x = rtOrigin.left + (rtDest.left - rtOrigin.left) * m_Transform.nCurrFrame / m_Transform.nFrame;
			y = rtOrigin.top + (rtDest.top - rtOrigin.top) * m_Transform.nCurrFrame / m_Transform.nFrame;
			cx = (rtDest.Width() - rtOrigin.Width()) * m_Transform.nCurrFrame / m_Transform.nFrame + rtOrigin.Width();
			cy = (rtDest.Height() - rtOrigin.Height()) * m_Transform.nCurrFrame / m_Transform.nFrame + rtOrigin.Height();
		}
		m_Transform.pImg->Stretch(cx, cy);
	}
	if(m_Transform.nAngle)
	{
		int nAngleTemp = 0;
		if(m_Transform.bRestore)
		{
			nAngleTemp = m_Transform.nAngle * (m_Transform.nFrame - m_Transform.nCurrFrame) / m_Transform.nFrame;
		}
		else
		{
			nAngleTemp = (m_Transform.nAngle * m_Transform.nCurrFrame) / m_Transform.nFrame;
		}
		m_Transform.pImg->Rotate(nAngleTemp);
		x += cx / 2- m_Transform.pImg->GetWidth() / 2;
		y += cy / 2- m_Transform.pImg->GetHeight() / 2;
	}
	m_rtAni.SetRect(x, y, x + m_Transform.pImg->GetWidth(), y + m_Transform.pImg->GetHeight());
	g_UI.Redraw(m_pPaint, m_rtAni);
	m_pPaint->Resize(m_rtAni.Width(), m_rtAni.Height());
	if(m_Transform.nCurrFrame >= m_Transform.nFrame)
	{
		if(m_Transform.bRestore)
		{
			m_Transform.Clear();
			for(int i = 0; i < m_pPaint->GetCount(); i++)
			{
				m_pPaint->ShowObject(TRUE, FALSE, i, TRUE);
			}
		}
	}
}

BOOL CSonicAnimation::AddObject(DWORD dwObjectId, int x /* = 0 */, int y /* = 0 */)
{
	if(IsValid() == FALSE)
	{
		return FALSE;
	}
	Redraw();
	if(m_pPaint->AddObject(dwObjectId, x, y))
	{
		AdjustTransform();
		return TRUE;
	}
	return FALSE;
}

BOOL CSonicAnimation::SetObject(DWORD dwObjectId, int x /* = 0 */, int y /* = 0 */, BOOL bVisible /* = TRUE */, BOOL bByIndex /* = FALSE */)
{
	if(IsValid() == FALSE)
	{
		return FALSE;
	}
	Redraw();
	if(m_pPaint->SetObject(dwObjectId, x, y, bVisible, bByIndex))
	{
		AdjustTransform();
		return TRUE;
	}
	return FALSE;
}

BOOL CSonicAnimation::DelObject(DWORD dwObjectId, BOOL bByIndex /* = FALSE */, BOOL bDestroyObject /* = FALSE */)
{
	if (IsValid() == FALSE)
	{
		return FALSE;
	}
	Redraw();
	if(m_pPaint->DelObject(dwObjectId, bByIndex, bDestroyObject))
	{
		AdjustTransform();
		return TRUE;
	}
	return FALSE;
}

BOOL CSonicAnimation::DelAllObject(BOOL bDestroyObject /* = FALSE */)
{
	if (IsValid() == FALSE)
	{
		return FALSE;
	}
	return m_pPaint->DelAllObject(bDestroyObject);
}

BOOL CSonicAnimation::GetCount()
{
	if(IsValid() == FALSE)
	{
		return FALSE;
	}
	return m_pPaint->GetCount();
}

BOOL CSonicAnimation::ShowObject(BOOL b, BOOL bRedraw, DWORD dwObjectId, BOOL bByIndex /* = FALSE */)
{
	if(IsValid() == FALSE)
	{
		return FALSE;
	}
	if(m_pPaint->ShowObject(b, bRedraw, dwObjectId, bByIndex))
	{
		AdjustTransform();
		return TRUE;
	}
	return FALSE;
}

PAINT_ITEM * CSonicAnimation::GetPaintItem(DWORD dwObjectId, BOOL bByIndex /* = FALSE */)
{
	if(IsValid() == FALSE)
	{
		return FALSE;
	}
	return m_pPaint->GetPaintItem(dwObjectId, bByIndex);
}

void CSonicAnimation::OnDrawTransform(ISonicPaint * pPaint, LPVOID)
{
	if(!(m_EffectMask & ANI_STATUS_TRANSFROM))
	{
		return;
	}
	if(m_Transform.pImg == NULL)
	{
		return;
	}
	m_Transform.pImg->Draw(pPaint->GetCurrentPaint()->hdc, pPaint->GetCurrentPaint()->x, pPaint->GetCurrentPaint()->y);
}

void CSonicAnimation::AdjustTransform()
{
	if(m_EffectMask & ANI_STATUS_TRANSFROM)
	{
		for(int i = 0; i < m_pPaint->GetCount(); i++)
		{
			m_pPaint->ShowObject(TRUE, FALSE, i, TRUE);
		}
		m_pPaint->Delegate(DELEGATE_EVENT_PAINT, NULL, this, NULL);
		((CSonicPaint *)m_pPaint)->InternalDraw(m_Transform.pImgOrigin->GetDC(), 0, 0, m_hWnd, NULL);
		m_pPaint->Delegate(DELEGATE_EVENT_PAINT, NULL, this, &CSonicAnimation::OnDrawTransform);
		for(int i = 0; i < m_pPaint->GetCount(); i++)
		{
			m_pPaint->ShowObject(FALSE, FALSE, i, TRUE);
		}
	}
}

BOOL CSonicAnimation::OnWndMsg(UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_LBUTTONDOWN:
		{
			return OnLButtonDown(wParam, lParam);
		}
		break;
	case WM_RBUTTONDOWN:
		{
			return OnRButtonDown(wParam, lParam);
		}
		break;
	case WM_MOUSEMOVE:
		{
			return OnMouseMove(wParam, lParam);
		}
		break;
	case WM_MOUSELEAVE:
		{
			return OnMouseLeave(wParam, lParam);
		}
		break;
	}
	return TRUE;
}

BOOL CSonicAnimation::OnLButtonDown(WPARAM wParam, LPARAM lParam)
{
	POINT pt = {LOWORD(lParam), HIWORD(lParam)};
	if(PtInAnimation(pt))
	{
		FireDelegate(DELEGATE_EVENT_CLICK);
		return FALSE;
	}
	return TRUE;
}

BOOL CSonicAnimation::OnRButtonDown(WPARAM wParam, LPARAM lParam)
{
	POINT pt = {LOWORD(lParam), HIWORD(lParam)};
	if(PtInAnimation(pt))
	{
		FireDelegate(DELEGATE_EVENT_RCLICK);
		return FALSE;
	}
	return TRUE;
}

BOOL CSonicAnimation::PtInAnimation(POINT & pt)
{
	return PtInRect(m_pPaint->GetMsgRect(), pt);
}

BOOL CSonicAnimation::OnMouseMove(WPARAM wParam, LPARAM lParam)
{
	POINT pt = {LOWORD(lParam), HIWORD(lParam)};
	if(PtInAnimation(pt))
	{
		if(m_nStatus == msNormal)
		{
			m_nStatus = msHover;
			FireDelegate(DELEGATE_EVENT_MOUSEENTER);
		}
		else
		{
			FireDelegate(DELEGATE_EVENT_MOUSEHOVER);
		}
	}
	else if(m_nStatus)
	{
		m_nStatus = msNormal;
		FireDelegate(DELEGATE_EVENT_MOUSELEAVE);
	}	
	return TRUE;
}

BOOL CSonicAnimation::OnMouseLeave(WPARAM wParam, LPARAM lParam)
{
	if(m_nStatus)
	{
		m_nStatus = msNormal;
		FireDelegate(DELEGATE_EVENT_MOUSELEAVE);
	}
	return TRUE;
}

ISonicImage * CSonicAnimation::SnapShot()
{
	if(IsValid() == FALSE)
	{
		return NULL;
	}
	ISonicImage * pImg = g_UI.CreateImage();
	pImg->Create(m_pPaint->GetWidth(), m_pPaint->GetHeight(), 0, TRUE);
	((CSonicPaint *)m_pPaint)->InternalDraw(pImg->GetDC(), 0, 0, m_hWnd, NULL);
	return pImg;
}

void CSonicAnimation::OnDrawShowGently(ISonicPaint * pPaint, LPVOID)
{
	if(!(m_EffectMask & ANI_STATUS_SHOW_GENTYLE))
	{
		return;
	}
	if(m_Show.pImgOrigin == NULL)
	{
		return;
	}
	DRAW_PARAM dp;
	dp.dwMask = DP_ALPHA;
	dp.cAlpha = m_Show.nCurrAlpha;
	m_Show.pImgOrigin->Draw(pPaint->GetCurrentPaint()->hdc, pPaint->GetCurrentPaint()->x, pPaint->GetCurrentPaint()->y, &dp);
}