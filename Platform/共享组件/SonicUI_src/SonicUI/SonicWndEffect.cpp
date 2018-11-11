#include "StdAfx.h"
#include ".\sonicwndeffect.h"
#include "ConstDefine.h"
#include "math.h"
#include "mmsystem.h"
#include "SonicUI.h"
#include "SSE.h"
#include "SonicImage.h"

void CSonicWndEffect::FRAME_PARAM::Clear()
{
	for(int i = 0; i < 8; i++)
	{
		if(pImg[i])
		{
			g_UI.DestroyObject(pImg[i]);
			pImg[i] = NULL;
		}
	}
}

void CSonicWndEffect::TRANSFORM_PARAM::Transform(int &x, int &y)
{
	pImgTransform->CloneFrom(pImgOrigin);
	x = rtOrigin.left;
	y = rtOrigin.top;
	int cx = rtOrigin.Width();
	int cy = rtOrigin.Height();
	if(rtDest.Width() && rtDest.Height())
	{		
		if(bRestore)
		{
			x = rtDest.left + (rtOrigin.left - rtDest.left) * nFrame / nMaxFrame;
			y = rtDest.top + (rtOrigin.top - rtDest.top) * nFrame / nMaxFrame;
			cx = (rtOrigin.Width() - rtDest.Width()) * nFrame / nMaxFrame + rtDest.Width();
			cy = (rtOrigin.Height() - rtDest.Height()) * nFrame / nMaxFrame + rtDest.Height();
		}
		else
		{
			x = rtOrigin.left + (rtDest.left - rtOrigin.left) * nFrame / nMaxFrame;
			y = rtOrigin.top + (rtDest.top - rtOrigin.top) * nFrame / nMaxFrame;
			cx = (rtDest.Width() - rtOrigin.Width()) * nFrame / nMaxFrame + rtOrigin.Width();
			cy = (rtDest.Height() - rtOrigin.Height()) * nFrame / nMaxFrame + rtOrigin.Height();
		}
		pImgTransform->Stretch(cx, cy);
	}
	if(nAngle)
	{
		int nAngleTemp = 0;
		if(bRestore)
		{
			nAngleTemp = nAngle * (nMaxFrame - nFrame) / nMaxFrame;
		}
		else
		{
			nAngleTemp = (nAngle * nFrame) / nMaxFrame;
		}
		pImgTransform->Rotate(nAngleTemp);
		x += cx / 2- pImgTransform->GetWidth() / 2;
		y += cy / 2- pImgTransform->GetHeight() / 2;
	}
}

void CSonicWndEffect::TRANSFORM_PARAM::Clear()
{
	if(hHelp && IsWindow(hHelp))
	{
		DestroyWindow(hHelp);
	}
	if(pImgOrigin)
	{
		g_UI.DestroyObject(pImgOrigin);
		pImgOrigin = NULL;
	}
	if(pImgTransform)
	{
		g_UI.DestroyObject(pImgTransform);
		pImgTransform = NULL;
	}
	bDirect = FALSE;
	bRestore = FALSE;	
}

void CSonicWndEffect::PRE_DRAG_PARAM::Clear()
{
	for(int i = 0; i < 4; i++)
	{
		if(pImg[i])
		{
			g_UI.DestroyObject(pImg[i]);
			pImg[i] = NULL;
		}
	}
	if(hHelp && IsWindow(hHelp))
	{
		DestroyWindow(hHelp);
	}
}

void CSonicWndEffect::SLIDE_PARAM::Clear()
{
	if(hHelp && IsWindow(hHelp))
	{
		DestroyWindow(hHelp);
		hHelp = NULL;
	}
}

void CSonicWndEffect::SHUTTER_PARAM::Clear()
{
	if(hHelp && IsWindow(hHelp))
	{
		DestroyWindow(hHelp);
		hHelp = NULL;
	}
	if(pMask)
	{
		g_UI.DestroyObject(pMask);
		pMask = NULL;
	}
	if(pOrigin)
	{
		g_UI.DestroyObject(pOrigin);
		pOrigin = NULL;
	}
}
//////////////////////////////////////////////////////////////////////////

CSonicWndEffect::CSonicWndEffect(void)
{
	m_dwType = BASE_TYPE_WND_EFFECT;
	InitValue();
}

CSonicWndEffect::~CSonicWndEffect(void)
{
	Clear();
}

BOOL CSonicWndEffect::Attach(HWND hWnd, BOOL bAlphaPerPixel /* = FALSE */)
{
	if(hWnd == NULL || IsWindow(hWnd) == FALSE)
	{
		return FALSE;
	}

	Clear();
	if(g_UI.Attach(hWnd, this) == FALSE)
	{
		return FALSE;
	}

	m_hWnd = hWnd;
	m_dwOldExStyle = GetWindowLong(hWnd, GWL_EXSTYLE);
	if((m_dwOldExStyle & WS_EX_LAYERED) == FALSE)
	{
		SetWindowLong(hWnd, GWL_EXSTYLE, WS_EX_LAYERED | m_dwOldExStyle);
	}

	if(bAlphaPerPixel)
	{
		CRect rt;
		GetWindowRect(hWnd, rt);
		PrepareLayer(rt.Width(), rt.Height());
		Redraw();
	}
	else
	{
		SetLayeredAttributes(0, 255, LWA_ALPHA);
	}
	return TRUE;
}

void CSonicWndEffect::Clear()
{
	if(m_hWnd == NULL)
	{
		return;
	}
	if(!(m_dwOldExStyle & WS_EX_LAYERED))
	{
		LONG lExStyle = GetWindowLong(m_hWnd, GWL_EXSTYLE);
		SetWindowLong(m_hWnd, GWL_EXSTYLE, (lExStyle & ~WS_EX_LAYERED));
	}
	RestoreChild();
	if(m_pBg)
	{
		g_UI.DestroyObject(m_pBg);
		m_pBg = NULL;
	}
	if(m_Effect.pImg)
	{
		g_UI.DestroyObject(m_Effect.pImg);
		m_Effect.pImg = NULL;
	}
	m_Dib.Clear();
	m_Effect.frame.Clear();
	m_Effect.transform.Clear();
	m_Effect.predrag.Clear();
	m_Effect.shutter.Clear();
	InvalidateRect(m_hWnd, NULL, TRUE);

	g_UI.Detach(this);
	m_hWnd = NULL;
	ClearBaseData();
	InitValue();
}

BOOL CSonicWndEffect::ShowGently(BOOL bShow, BOOL bActivted /*= FALSE*/, int nStep /* = 0 */)
{
	if(m_hWnd == NULL)
	{
		return FALSE;
	}

	if(QueryInternalTimer(TIMER_SHOWING_GENTLY))
	{
		if((bShow && m_Effect.fadeout.nAlphaStep < 0)
			|| (!bShow && m_Effect.fadeout.nAlphaStep > 0))
		{
			m_Effect.fadeout.nAlphaStep = -m_Effect.fadeout.nAlphaStep;
		}
	}
	else
	{
		if(bShow == IsWindowVisible(m_hWnd))
		{
			return FALSE;
		}
		m_Effect.fadeout.nAlphaStep = nStep ? abs(nStep) : DEFAULT_ALPHA_STEP;
		if(bShow)
		{
			m_Effect.fadeout.nShowType = bActivted ? SW_SHOW : SW_SHOWNA;
			m_Effect.fadeout.nCurrAlpha = 0;
		}
		else
		{
			m_Effect.fadeout.nCurrAlpha = 255;
			m_Effect.fadeout.nAlphaStep = -m_Effect.fadeout.nAlphaStep;
		}
		SetInternalTimer(TIMER_SHOWING_GENTLY, ANIMATION_INTERVAL);

	}
	return TRUE;
}

void CSonicWndEffect::InitValue()
{
	memset(&m_Effect, 0, sizeof(m_Effect));
	m_Effect.move.nFrame = 0;
	m_Effect.fadeout.nCurrAlpha = 0;
	m_Effect.fadeout.nShowType = 0;
	m_dwOldExStyle = 0;
	m_Effect.move.nMoveTimerId = 0;
	m_pBg = NULL;
	m_nKeyColor = -1;
	m_bAlpha = 0;
	m_bDelayShowSwitch = 0;
	m_szSrc.cx = 0;
	m_szSrc.cy = 0;
	m_dwBkColor = 0;
}

void CSonicWndEffect::OnTimerShowingGently()
{
	if(m_Effect.fadeout.nCurrAlpha == 0 && m_Effect.fadeout.nAlphaStep > 0 && IsWindowVisible(m_hWnd) == FALSE)
	{
		DWORD flags = SWP_NOZORDER | SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW;
		if(m_Effect.fadeout.nShowType == SW_SHOWNA)
		{
			flags |= SWP_NOACTIVATE;
		}
		SetWindowPos(m_hWnd, NULL, 0, 0, 0, 0, flags);
	}
	m_Effect.fadeout.nCurrAlpha += m_Effect.fadeout.nAlphaStep;
	if(m_Effect.fadeout.nCurrAlpha <= 0)
	{
		m_Effect.fadeout.nCurrAlpha = 0;
		ShowWindow(m_hWnd, SW_HIDE);
		EndShowTimer();
		return;
	}
	if(m_Effect.fadeout.nCurrAlpha >= 255)
	{
		m_Effect.fadeout.nCurrAlpha = 255;
		EndShowTimer();
	}

	m_bAlpha = m_Effect.fadeout.nCurrAlpha;
	UpdateLayer();

	for(WND_LIST::iterator it = m_ChildList.begin(); it != m_ChildList.end(); it++)
	{
		CSonicWndEffect * pEffect = (CSonicWndEffect *)g_UI.EffectFromHwnd(*it);
		if(pEffect)
		{
			pEffect->SetLayeredAttributes(0, m_Effect.fadeout.nCurrAlpha, LWA_ALPHA);
		}
	}
}

void CSonicWndEffect::EndShowTimer()
{
	KillInternalTimer(TIMER_SHOWING_GENTLY);
	m_Effect.fadeout.nAlphaStep = 0;
	FireDelegate(DELEGATE_EVENT_SHOWGENTLY_OVER);
}

BOOL CSonicWndEffect::SetShapeByImage(ISonicImage * pImg, DWORD dwColorKey /* = -1 */)
{
	if(m_hWnd == NULL || pImg == NULL)
	{
		return FALSE;
	}
	if(m_pBg == NULL)
	{
		m_pBg = g_UI.CreateImage();
	}
	m_pBg->CloneFrom(pImg);
	m_nKeyColor = dwColorKey;
	SetWindowPos(m_hWnd, NULL, 0, 0, pImg->GetWidth(), pImg->GetHeight(), SWP_NOMOVE);
	if(m_Dib.IsValid() == FALSE)
	{
		UpdateLayer();
	}
	Redraw();
	if(m_Effect.dwEffectMask & WND_EFFECT_TRANSFORM)
	{
		g_UI.DestroyObject(m_Effect.transform.pImgOrigin);
		m_Effect.transform.pImgOrigin = Snapshot();
	}
	return TRUE;
}

BOOL CSonicWndEffect::MoveGently(int x, int y, BOOL bEvenSpeed /* = FALSE */)
{
	if(IsValid() == FALSE)
	{
		return FALSE;
	}
	m_Effect.move.nFrame = 0;
	CRect rtWnd;
	GetWindowRect(m_hWnd, rtWnd);
	double x1 = rtWnd.left;
	double y1 = rtWnd.top;
	double fStepX = (double)(x - rtWnd.left) / DEFAULT_MOVE_FRAME;
	double fStepY = (double)(y - rtWnd.top) / DEFAULT_MOVE_FRAME;
	const int nMe = 2;
	double fCoefficientX = (double)(x - rtWnd.left) / pow((double)DEFAULT_MOVE_FRAME, nMe);
	double fCoefficientY = (double)(y - rtWnd.top) / pow((double)DEFAULT_MOVE_FRAME, nMe);
	int i;
	for(i = 0; i < DEFAULT_MOVE_FRAME; i++)
	{
		if(!bEvenSpeed)
		{
			fStepX = fCoefficientX * (pow((double)DEFAULT_MOVE_FRAME - i, nMe) - pow((double)DEFAULT_MOVE_FRAME - i - 1, nMe));
			fStepY = fCoefficientY * (pow((double)DEFAULT_MOVE_FRAME - i, nMe) - pow((double)DEFAULT_MOVE_FRAME - i - 1, nMe));
		}
		x1 += fStepX;
		y1 += fStepY;
		m_Effect.move.rtMove[i].left = ROUND(x1);
		m_Effect.move.rtMove[i].top = ROUND(y1);
		m_Effect.move.rtMove[i].right = m_Effect.move.rtMove[i].left + rtWnd.Width();
		m_Effect.move.rtMove[i].bottom = m_Effect.move.rtMove[i].top + rtWnd.Height();
	}
	m_Effect.move.rtMove[i].SetRect(x, y, x + rtWnd.Width(), y + rtWnd.Height());

	if(m_Effect.move.nMoveTimerId)
	{
		timeKillEvent(m_Effect.move.nMoveTimerId);
	}
	m_Effect.move.nMoveTimerId = timeSetEvent(ANIMATION_INTERVAL, 10, TimerProc, (DWORD_PTR)this, TIME_PERIODIC | TIME_CALLBACK_FUNCTION);
	return TRUE;
}

void CSonicWndEffect::OnTimerMoveGently()
{
	if(m_hWnd == NULL || IsWindow(m_hWnd) == FALSE)
	{
		return;
	}
	SetWindowPos(m_hWnd, NULL, m_Effect.move.rtMove[m_Effect.move.nFrame].left, m_Effect.move.rtMove[m_Effect.move.nFrame].top, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	m_Effect.move.nFrame++;
	if(m_Effect.move.nFrame > DEFAULT_MOVE_FRAME)
	{
		timeKillEvent(m_Effect.move.nMoveTimerId);
		m_Effect.move.nMoveTimerId = 0;
		m_Effect.move.nFrame = 0;
		FireDelegate(DELEGATE_EVENT_MOVEGENTLY_OVER);
	}
}

void CALLBACK CSonicWndEffect::TimerProc(UINT uTimerID, UINT uMsg, DWORD_PTR dwUser, DWORD_PTR dw1, DWORD_PTR dw2)
{
	CSonicWndEffect * pEffect = (CSonicWndEffect *)dwUser;
	if(pEffect && pEffect->m_Effect.move.nMoveTimerId == uTimerID)
	{
		pEffect->OnTimerMoveGently();
	}
}

void CSonicWndEffect::UpdateLayer()
{
	if(m_Dib.IsValid())
	{
		UpdateDibLayer(DRAW_LAYER_ERASE);
	}
	else
	{
		DWORD dwFlag = LWA_ALPHA;
		if(m_nKeyColor != -1)
		{
			dwFlag |= LWA_COLORKEY;
		}
		SetLayeredWindowAttributes(m_hWnd, m_nKeyColor, m_bAlpha, dwFlag);
	}
}

BOOL CSonicWndEffect::SetLayeredAttributes(COLORREF crKey, BYTE bAlpha, DWORD dwFlag)
{
	if(m_Dib.IsValid())
	{
		CRect rtWnd;
		GetWindowRect(m_hWnd, rtWnd);
		POINT ptDst = {rtWnd.left, rtWnd.top};
		POINT ptSrc = {0, 0};
		BLENDFUNCTION blend;
		blend.AlphaFormat = AC_SRC_ALPHA;
		blend.BlendFlags = 0;
		blend.BlendOp = AC_SRC_OVER;
		blend.SourceConstantAlpha = bAlpha;
		return UpdateLayeredWindow(m_hWnd, NULL, &ptDst, &m_szSrc, m_Dib.GetSafeHdc(), &ptSrc, crKey, &blend, dwFlag);
	}
	else
	{
		if(dwFlag & LWA_ALPHA)
		{
			m_bAlpha = bAlpha;
		}
		if(dwFlag & LWA_COLORKEY)
		{
			m_nKeyColor = crKey;
		}
		return SetLayeredWindowAttributes(m_hWnd, crKey, bAlpha, dwFlag);
	}
}

BOOL CSonicWndEffect::OnWndMsg(UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_NCLBUTTONDOWN:
		{
			if(wParam == HTCAPTION)
			{
				if(m_Effect.dwEffectMask & WND_EFFECT_TRANSFORM)
				{
					return FALSE;
				}
				if(m_Effect.dwEffectMask & WND_EFFECT_PREDRAG)
				{
					CRect rtFake, rtReal;
					GetWindowRect(m_hWnd, rtReal);
					SetWindowPos(m_Effect.predrag.hHelp, NULL, rtReal.left, rtReal.top, rtReal.Width(), rtReal.Height(), SWP_NOZORDER);
					ShowWindow(m_Effect.predrag.hHelp, SW_SHOWNA);
					SendMessage(m_Effect.predrag.hHelp, WM_NCLBUTTONDOWN, HTCAPTION, 0);
					ShowWindow(m_Effect.predrag.hHelp, SW_HIDE);
					GetWindowRect(m_Effect.predrag.hHelp, rtFake);
					SetWindowPos(m_hWnd, NULL, rtFake.left, rtFake.top, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
					return FALSE;
				}
			}
		}
		break;
	case WM_WINDOWPOSCHANGING:
		{
			return OnPosChanging((LPWINDOWPOS)lParam);			
		}
		break;
	case WM_WINDOWPOSCHANGED:
		{
			return OnPosChanged((LPWINDOWPOS)lParam);
		}
		break;
	case WM_PAINT:
		{
			if(m_Dib.IsValid() && wParam == NULL)
			{
				UpdateDibLayer();
				ValidateRect(m_hWnd, NULL);
				return FALSE;
			}
			if(IsAlphaPerPixel() == FALSE)
			{
				if(m_bDelayShowSwitch)
				{
					m_bDelayShowSwitch = FALSE;
					UpdateLayer();					
				}
			}
		}
		break;
	case WM_PRINTCLIENT:
		{
			SendMessage(m_hWnd, WM_PAINT, wParam, 0);
			return FALSE;
		}
		break;
	case WM_LBUTTONDOWN:
		{
			if(m_Effect.dwEffectMask & WND_EFFECT_WHOLE_DRAG)
			{
				SendMessage(m_hWnd, WM_NCLBUTTONDOWN, HTCAPTION, 0);
			}
		}
		break;
	case WM_ERASEBKGND:
		{
			HDC hdc = (HDC)wParam;
			return OnEraseBackground(hdc);
		}
		break;
	case WM_SONIC_MOUSE_ENTER:
		{
			if(m_Effect.dwEffectMask & WND_EFFECT_TRACK_MOUSE)
			{
				break;
			}
			OnMouseEnter();
		}
		break;
	case WM_SONIC_MOUSE_LEAVE:
		{
			if(m_Effect.dwEffectMask & WND_EFFECT_TRACK_MOUSE)
			{
				break;
			}
			OnMouseLeave();
		}
		break;
	case WM_ACTIVATEAPP:
		{
			if(wParam == FALSE)
			{
				if(m_Effect.dwEffectMask & WND_EFFECT_FRAME)
				{
					if(m_Effect.frame.bType == 1)
					{
						AnimateFrame(FALSE);
					}
				}
			}
			else
			{
				if(m_Effect.dwEffectMask & WND_EFFECT_FRAME)
				{
					if(m_Effect.frame.bType == 1)
					{
						AnimateFrame(TRUE);
					}
				}
			}
		}
		break;
	}
	return TRUE;
}

void CSonicWndEffect::PrepareLayer(int cx, int cy)
{
	if(cx == m_szSrc.cx && cy == m_szSrc.cy)
	{
		return;
	}
	m_szSrc.cx = cx;
	m_szSrc.cy = cy;

	HDC hdc = GetDC(m_hWnd);
	if(m_Dib.IsValid() == FALSE)
	{
		m_Dib.Create(hdc, cx, cy);
	}
	else
	{
		m_Dib.Resize(cx, cy);
	}
	ReleaseDC(m_hWnd, hdc);
}

void CSonicWndEffect::DrawLayer(BOOL bErase /* = TRUE */)
{
	if(m_hWnd == NULL)
	{
		return;
	}
	if(m_Dib.IsValid() == FALSE)
	{
		return;
	}
	if(bErase)
	{
		SendMessage(m_hWnd, WM_ERASEBKGND, (WPARAM)m_Dib.GetSafeHdc(), 0);
	}
	SendMessage(m_hWnd, WM_PAINT, (WPARAM)m_Dib.GetSafeHdc(), 0);
}

BOOL CSonicWndEffect::OnPosChanging(LPWINDOWPOS pPos)
{
	if(IsAlphaPerPixel() == FALSE)
	{
		if(pPos->flags & SWP_SHOWWINDOW || pPos->flags & SWP_DRAWFRAME)
		{
			// following code assure no flicker generated by NC paint
			m_bDelayShowSwitch = TRUE;
			SetLayeredWindowAttributes(m_hWnd, 0, 0, LWA_ALPHA);
		}
	}
	return TRUE;
}

BOOL CSonicWndEffect::OnPosChanged(LPWINDOWPOS pPos)
{
	const UINT flags = pPos->flags;
	CRect rt;
    
	// process every child
	if(!(flags & SWP_NOMOVE))
	{
		for(WND_LIST::iterator it = m_ChildList.begin(); it != m_ChildList.end(); it++)
		{
			HWND hWnd = *it;
			GetWindowRect(hWnd, rt);
			POINT pt = {rt.left, rt.top};
			ScreenToClient(m_hWnd, &pt);
			pt.x += pPos->x;
			pt.y += pPos->y;
			SetWindowPos(hWnd, NULL, pt.x, pt.y, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
		}
	}

	if(!(flags & SWP_NOSIZE))
	{
		if(m_Effect.dwEffectMask & WND_EFFECT_FRAME)
		{
			int nCornerWidth = m_Effect.frame.nCornerWidth;
			int nCornerHeight = m_Effect.frame.nCornerHeight;
			if(m_Effect.frame.pImg[0])
			{
				m_Effect.frame.pImg[0]->Extend(0, pPos->cy - nCornerHeight * 2);
			}
			if(m_Effect.frame.pImg[1])
			{
				m_Effect.frame.pImg[1]->Extend(pPos->cx - nCornerWidth * 2);
			}
			if(m_Effect.frame.pImg[2])
			{
				m_Effect.frame.pImg[2]->Extend(0, pPos->cy - nCornerHeight * 2);
			}
			if(m_Effect.frame.pImg[3])
			{
				m_Effect.frame.pImg[3]->Extend(pPos->cx - nCornerWidth * 2);
			}
		}
		if(m_Dib.IsValid())
		{
			PrepareLayer(pPos->cx, pPos->cy);
			CRect rt;
			GetWindowRect(m_hWnd, rt);
			UpdateDibLayer(rt.left, rt.top, rt.Width(), rt.Height(), 0, 0, DRAW_LAYER_ERASE);
		}
	}

	// process every child
	if((flags & SWP_HIDEWINDOW) || (flags & SWP_SHOWWINDOW))
	{
		for(WND_LIST::iterator it = m_ChildList.begin(); it != m_ChildList.end(); it++)
		{
			HWND hWnd = *it;
			SetWindowPos(hWnd, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | (flags & SWP_HIDEWINDOW) | (flags & SWP_SHOWWINDOW) | SWP_NOZORDER | SWP_NOACTIVATE);
		}
	}
	return TRUE;
}

BOOL CSonicWndEffect::Init()
{
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style			= CS_DBLCLKS;
	wcex.lpfnWndProc	= (WNDPROC)EffectHelpProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= NULL;
	wcex.hIcon			= NULL;
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= NULL;
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= EFFECT_HELP_WND;
	wcex.hIconSm		= NULL;
	if(RegisterClassEx(&wcex) == FALSE)
	{
		return FALSE;
	}
	return TRUE;
}

void CSonicWndEffect::OnInternalTimer(DWORD dwTimerId)
{
	if(dwTimerId & TIMER_SHOWING_GENTLY)
	{
		OnTimerShowingGently();
	}
	if(dwTimerId & TIMER_MOVE_GENTLY)
	{
		OnTimerMoveGently();
	}
	if(dwTimerId & TIMER_FRAME)
	{
		OnTimerFrame();
	}
	if(dwTimerId & TIMER_TRANSFORM)
	{
		OnTimerTransform();
	}
	if(dwTimerId & TIMER_SLIDE)
	{
		OnTimerSlide();
	}
	if(dwTimerId & TIMER_TRACK_MOUSE)
	{
		OnTimerTrackMouse();
	}
	if(dwTimerId & TIMER_SHUTTER)
	{
		OnTimerShutter();
	}
}

BOOL CSonicWndEffect::IsValid()
{
	return m_hWnd != NULL;
}

BOOL CSonicWndEffect::EnableWholeDrag(BOOL b)
{
	if(IsValid() == FALSE)
	{
		return FALSE;
	}
	if(b)
	{
		m_Effect.dwEffectMask |= WND_EFFECT_WHOLE_DRAG;
	}
	else
	{
		m_Effect.dwEffectMask &= ~WND_EFFECT_WHOLE_DRAG;
	}
	return TRUE;
}

BOOL CSonicWndEffect::EnableGray(BOOL b)
{
	if(IsValid() == FALSE)
	{
		return FALSE;
	}
	if(b)
	{
		if(m_Effect.pImg == NULL)
		{
			m_Effect.pImg = g_UI.CreateImage();
		}
		m_Effect.dwEffectMask |= WND_EFFECT_GRAY;
	}
	else
	{
		m_Effect.dwEffectMask &= ~WND_EFFECT_GRAY;
		if(m_Effect.dwEffectMask == 0)
		{
			g_UI.DestroyObject(m_Effect.pImg);
			m_Effect.pImg = NULL;
		}
	}
	Redraw();
	return TRUE;
}

void CSonicWndEffect::FinalEffect(HDC hdc)
{
	CRect rtUpdate = g_UI.m_rtUpdate;
	if(m_Effect.dwEffectMask & WND_EFFECT_GRAY)
	{
		m_Effect.pImg->Load(hdc, rtUpdate.left, rtUpdate.top, rtUpdate.Width(), rtUpdate.Height(), TRUE);
		m_Effect.pImg->Gray();
		m_Effect.pImg->SetBitsToDevice(hdc, rtUpdate.left, rtUpdate.top);
	}
	if(m_Effect.dwEffectMask & WND_EFFECT_HSL)
	{
		m_Effect.pImg->Load(hdc, rtUpdate.left, rtUpdate.top, rtUpdate.Width(), rtUpdate.Height(), TRUE);
		m_Effect.pImg->SetHsl(m_Effect.h, m_Effect.s, m_Effect.l);
		m_Effect.pImg->SetBitsToDevice(hdc, rtUpdate.left, rtUpdate.top);
	}
	if(m_Effect.dwEffectMask & WND_EFFECT_FRAME)
	{
		CRect rt;
		GetWindowRect(m_hWnd, rt);
		int nCornerWidth = m_Effect.frame.nCornerWidth;
		int nCornerHeight = m_Effect.frame.nCornerHeight;
		int nSideWidth = m_Effect.frame.nSideWidth;
		if(m_Effect.frame.nCurrAlpha)
		{
			DRAW_PARAM dp;
			dp.dwMask = DP_ALPHA;
			dp.cAlpha = m_Effect.frame.nCurrAlpha;
			if(m_Effect.frame.pImg[0])
			{
				m_Effect.frame.pImg[0]->Draw(hdc, 0, nCornerHeight, &dp);
			}
			if(m_Effect.frame.pImg[1])
			{
				m_Effect.frame.pImg[1]->Draw(hdc, nCornerWidth, 0, &dp);
			}
			if(m_Effect.frame.pImg[2])
			{
				m_Effect.frame.pImg[2]->Draw(hdc, rt.Width() - nSideWidth, nCornerHeight, &dp);
			}
			if(m_Effect.frame.pImg[3])
			{
				m_Effect.frame.pImg[3]->Draw(hdc, nCornerWidth, rt.Height() - nSideWidth, &dp);
			}
		}
	}
}

BOOL CSonicWndEffect::EnableHSLAdjust(BOOL b, int h /* = -1 */, int s /* = -1 */, int l /* = -1 */)
{
	if(IsValid() == FALSE)
	{
		return FALSE;
	}
	if(b)
	{
		if(h == s && s == l && h == -1)
		{
			return FALSE;
		}
		m_Effect.dwEffectMask |= WND_EFFECT_HSL;
		m_Effect.h = h;
		m_Effect.s = s;
		m_Effect.l = l;
		if(m_Effect.pImg == NULL)
		{
			m_Effect.pImg = g_UI.CreateImage();
		}
	}
	else
	{
		if(!(m_Effect.dwEffectMask & WND_EFFECT_HSL))
		{
			return FALSE;
		}
		m_Effect.dwEffectMask &= ~WND_EFFECT_HSL;
		if(m_Effect.dwEffectMask == 0)
		{
			g_UI.DestroyObject(m_Effect.pImg);
			m_Effect.pImg = NULL;
		}
	}
	Redraw();
	return TRUE;
}

void CSonicWndEffect::ModifyChild(HWND hWnd)
{
	if(GetProp(hWnd, CHILD_PROC))
	{
		return;
	}
	LONG lOldProc = SetWindowLong(hWnd, GWL_WNDPROC, (LONG)(LONG_PTR)ChildProc);
	if(lOldProc == 0)
	{
		return;
	}
	SetProp(hWnd, CHILD_PROC, (HANDLE)(LONG_PTR)lOldProc);
	m_ChildList.push_back(hWnd);
}

void CSonicWndEffect::RestoreChild()
{
	if(m_ChildList.size())
	{
		for(WND_LIST::iterator it = m_ChildList.begin(); it != m_ChildList.end(); it++)
		{
			HWND hWnd = *it;
			DestroyWindow(hWnd);
		}
		m_ChildList.clear();
	}
}

LRESULT CSonicWndEffect::ChildProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	WNDPROC pOld = (WNDPROC)GetProp(hWnd, CHILD_PROC);
	if(pOld == NULL)
	{
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	switch (message)
	{
	case WM_NCDESTROY:
		{
			RemoveProp(hWnd, CHILD_PROC);
		}
		break;
	case WM_SYSCOMMAND:
		{
			PostMessage(GetParent(hWnd), message, wParam, lParam);
		}
		break;
	}
	return CallWindowProc(pOld, hWnd, message, wParam, lParam);
}

HWND CSonicWndEffect::CreateChildCtrl(LPCTSTR lpszClassName, LPCTSTR lpszText, DWORD dwStyle, int x, int y, int cx, int cy, int nAttach /* = -1 */)
{
	if(IsValid() == FALSE)
	{
		return NULL;
	}
	BOOL bChild = FALSE;
	if(dwStyle & WS_CHILD)
	{
		bChild = TRUE;
		dwStyle = (dwStyle & ~WS_CHILD) | WS_POPUP;
		POINT pt ={x, y};
		ClientToScreen(m_hWnd, &pt);
		x = pt.x;
		y = pt.y;
	}
	HWND hRet = CreateWindow(lpszClassName, lpszText, dwStyle, x, y, cx, cy, m_hWnd, NULL, NULL, NULL);
	if(hRet == NULL)
	{
		return NULL;
	}
	if(bChild)
	{
		ModifyChild(hRet);
	}
	if(nAttach >= 0)
	{
		ISonicWndEffect * pEffect = g_UI.CreateWndEffect();
		pEffect->Attach(hRet, nAttach);
	}
	return hRet;
}

BOOL CSonicWndEffect::EnableFrame(BOOL b, BYTE bSideMask /* = 0xf */, BYTE bType /* = 0 */, int nSpace /* = 0 */,
								  int nAlphaSpeed /* = 10 */, ISonicImage * pSide /* = NULL */, ISonicImage * pCorner /* = NULL */)
{
	if(IsValid() == FALSE)
	{
		return FALSE;
	}
	CRect rt;
	GetWindowRect(m_hWnd, rt);
	m_Effect.frame.Clear();
	if(b)
	{
		if(m_Effect.dwEffectMask & WND_EFFECT_FRAME)
		{
			return FALSE;
		}
		if(pSide == NULL && pCorner == NULL)
		{
			return FALSE;
		}
		m_Effect.dwEffectMask |= WND_EFFECT_FRAME;
		m_Effect.frame.bSideMask = bSideMask;
		m_Effect.frame.bType = bType;
		m_Effect.frame.nAlphaSpeed = nAlphaSpeed;
		m_Effect.frame.nSpace = nSpace;
		m_Effect.frame.nCurrAlpha = bType == 2 ? 255 : 0;

		int cx = 0;
		int cy = 0;
		if(pCorner)
		{
			m_Effect.frame.nCornerWidth = pCorner->GetWidth();
			m_Effect.frame.nCornerHeight = pCorner->GetHeight();
		}
		else
		{
			m_Effect.frame.nCornerWidth = 0;
			m_Effect.frame.nCornerHeight = 0;
		}
		if(pSide)
		{
			cx = pSide->GetWidth();
			cy = pSide->GetHeight();
			m_Effect.frame.nSideWidth = pSide->GetWidth();
			// left
			if(bSideMask & 0x1)
			{
				m_Effect.frame.pImg[0] = g_UI.CreateImage();
				m_Effect.frame.pImg[0]->CloneFrom(pSide);
				m_Effect.frame.pImg[0]->Extend(0, rt.Height() - m_Effect.frame.nCornerHeight * 2);
			}

			// top
			if(bSideMask & 0x2)
			{
				m_Effect.frame.pImg[1] = g_UI.CreateImage();
				m_Effect.frame.pImg[1]->CloneFrom(pSide);
				m_Effect.frame.pImg[1]->Rotate(90);
				m_Effect.frame.pImg[1]->Extend(rt.Width() - m_Effect.frame.nCornerWidth * 2);
			}

			// right
			if(bSideMask & 0x4)
			{
				m_Effect.frame.pImg[2] = g_UI.CreateImage();
				m_Effect.frame.pImg[2]->CloneFrom(pSide);
				m_Effect.frame.pImg[2]->MirrorX();
				m_Effect.frame.pImg[2]->Extend(0, rt.Height() - m_Effect.frame.nCornerHeight * 2);
			}

			// bottom
			if(bSideMask & 0x8)
			{
				m_Effect.frame.pImg[3] = g_UI.CreateImage();
				m_Effect.frame.pImg[3]->CloneFrom(pSide);
				m_Effect.frame.pImg[3]->Rotate(270);
				m_Effect.frame.pImg[3]->Extend(rt.Width() - m_Effect.frame.nCornerWidth * 2);
			}
		}		
	}
	else
	{
		if((m_Effect.dwEffectMask & WND_EFFECT_FRAME) == FALSE)
		{
			return FALSE;
		}
		m_Effect.dwEffectMask &= ~WND_EFFECT_FRAME;
	}
	Redraw();
	return TRUE;
}

BOOL CSonicWndEffect::SetBkColor(DWORD dwColor)
{
	if(IsValid() == FALSE)
	{
		return FALSE;
	}
	m_dwBkColor = dwColor;
	if(m_Dib.IsValid())
	{
		CSSE::AdjustAlpha(m_dwBkColor);
	}
	else
	{
		m_dwBkColor &= 0xffffff;
	}
	m_Effect.dwEffectMask |= WND_EFFECT_BKCOLOR;
	Redraw();
	return TRUE;
}

LRESULT CSonicWndEffect::EffectHelpProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	CSonicWndEffect * pEffect = (CSonicWndEffect *)g_UI.EffectFromHwnd(GetParent(hWnd));
	DWORD dwEhtType = (DWORD)(DWORD_PTR)GetProp(hWnd, EHT_PROP);
	if(pEffect == NULL || dwEhtType == 0)
	{
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	BOOL bRet = TRUE;
	switch (dwEhtType)
	{
	case EHT_FRAME:
		bRet = pEffect->OnFrameMsg(hWnd, message, wParam, lParam);
		break;
	case EHT_TRANSFORM:
		bRet = pEffect->OnTransformMsg(hWnd, message, wParam, lParam);
		break;
	case EHT_PREDRAG:
		bRet = pEffect->OnPreDragMsg(hWnd, message, wParam, lParam);
		break;
	case EHT_SHUTTER:
		bRet = pEffect->OnShutterMsg(hWnd, message, wParam, lParam);
		break;
	}
	switch (message)
	{
	case WM_NCDESTROY:
		{
			RemoveProp(hWnd, EHT_PROP);
		}
		break;
	}
	if(!bRet)
	{
		return 0;
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}

void CSonicWndEffect::AnimateFrame(BOOL b)
{
	if((m_Effect.dwEffectMask & WND_EFFECT_FRAME) == FALSE)
	{
		return;
	}
	if(m_Effect.frame.bType == 2)
	{
		return;
	}
	m_Effect.frame.nAlphaSpeed = b ? abs(m_Effect.frame.nAlphaSpeed) : -abs(m_Effect.frame.nAlphaSpeed);
	if(!QueryInternalTimer(TIMER_FRAME))
	{
		SetInternalTimer(TIMER_FRAME, ANIMATION_INTERVAL);
	}
}

void CSonicWndEffect::OnTimerFrame()
{
	m_Effect.frame.nCurrAlpha += m_Effect.frame.nAlphaSpeed;
	if(m_Effect.frame.nCurrAlpha >= 255)
	{
		m_Effect.frame.nCurrAlpha = 255;
		KillInternalTimer(TIMER_FRAME);
	}
	else if(m_Effect.frame.nCurrAlpha <= 0)
	{
		m_Effect.frame.nCurrAlpha = 0;
		KillInternalTimer(TIMER_FRAME);
	}
	InvalidateRect(m_hWnd, NULL, TRUE);
}

HWND CSonicWndEffect::CreateEffectHelpWnd(enEffectHelpType eht, DWORD dwStyle)
{
	HWND hWnd = CreateChildCtrl(EFFECT_HELP_WND, NULL, dwStyle, 0, 0, 10, 10, TRUE);
	if(hWnd)
	{
		if(CSonicWndEffect * pEffect = (CSonicWndEffect *)g_UI.EffectFromHwnd(hWnd))
		{
			pEffect->SetBkColor(0);
		}
		SetProp(hWnd, EHT_PROP, (HANDLE)(DWORD_PTR)eht);
	}
	return hWnd;
}

BOOL CSonicWndEffect::OnFrameMsg(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_WINDOWPOSCHANGING:
		{
			LPWINDOWPOS pPos = (LPWINDOWPOS)lParam;
			if(m_Effect.dwEffectMask & WND_EFFECT_FRAME)
			{
				if(!(pPos->flags & SWP_NOSIZE))
				{					
				}
			}
		}
		break;
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hWnd, &ps);
			
			EndPaint(hWnd, &ps);
		}
		break;
	}
	return TRUE;
}

BOOL CSonicWndEffect::EnableTransform(BOOL b, int nAngle /* = 0 */, RECT * pDest /* = NULL */, int nFrame /* = 0 */)
{
	if(IsValid() == FALSE)
	{
		return FALSE;
	}
	if(QueryInternalTimer(TIMER_TRANSFORM))
	{
		return FALSE;
	}
	if(b)
	{
		if(m_Effect.dwEffectMask & WND_EFFECT_TRANSFORM)
		{
			return FALSE;
		}
		m_Effect.transform.Clear();
		m_Effect.dwEffectMask |= WND_EFFECT_TRANSFORM;
		m_Effect.transform.hHelp = CreateEffectHelpWnd(EHT_TRANSFORM, WS_POPUP);
		m_Effect.transform.bRestore = FALSE;
		m_Effect.transform.bDirect = FALSE;
		GetWindowRect(m_hWnd, m_Effect.transform.rtOrigin);
		m_Effect.transform.nAngle = nAngle;
		m_Effect.transform.pImgOrigin = Snapshot();
		m_Effect.transform.pImgTransform = g_UI.CreateImage();
		m_Effect.transform.pImgTransform->CloneFrom(m_Effect.transform.pImgOrigin);
		if(pDest)
		{
			m_Effect.transform.rtDest = *pDest;
		}
		SetWindowPos(m_Effect.transform.hHelp, NULL, m_Effect.transform.rtOrigin.left, m_Effect.transform.rtOrigin.top, 
			m_Effect.transform.pImgTransform->GetWidth(), m_Effect.transform.pImgTransform->GetHeight(), SWP_NOZORDER | SWP_SHOWWINDOW | SWP_NOACTIVATE);
		SetLayeredWindowAttributes(m_hWnd, 0, 0, LWA_ALPHA);
		
		if(nFrame == 0)
		{
			m_Effect.transform.nFrame = 1;
			m_Effect.transform.nMaxFrame = 1;
			Transform();
		}
		else
		{
			m_Effect.transform.nFrame = 0;
			m_Effect.transform.nMaxFrame = nFrame;
			SetInternalTimer(TIMER_TRANSFORM, GIF_INTERVAL);
		}
	}
	else
	{
		if(!(m_Effect.dwEffectMask & WND_EFFECT_TRANSFORM))
		{
			return FALSE;
		}
		m_Effect.transform.bRestore = TRUE;
		if(pDest)
		{
			m_Effect.transform.rtOrigin = *pDest;
		}
		if(nFrame == 0)
		{
			RestoreTransform();
		}
		else
		{
			m_Effect.transform.nFrame = 0;
			m_Effect.transform.nMaxFrame = nFrame;
			SetInternalTimer(TIMER_TRANSFORM, GIF_INTERVAL);
		}
	}
	return TRUE;
}

BOOL CSonicWndEffect::OnTransformMsg(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_PAINT:
		{
			HDC hdc;
			PAINTSTRUCT ps;
			hdc = BeginPaint(hWnd, &ps);
			if(m_Effect.transform.pImgTransform)
			{
				m_Effect.transform.pImgTransform->SetBitsToDevice(hdc);
			}
			EndPaint(hWnd, &ps);
		}
		break;
	case WM_LBUTTONUP:
	case WM_LBUTTONDOWN:
	case WM_LBUTTONDBLCLK:
	case WM_RBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONDBLCLK:
	case WM_MOUSEWHEEL:
	case WM_KEYDOWN:
	case WM_KEYUP:
	case WM_SYSKEYDOWN:
	case WM_SYSKEYUP:
	case WM_SYSCOMMAND:
		{
			SendMessage(GetParent(hWnd), message, wParam, lParam);
			return FALSE;
		}
		break;
	}
	if(message > WM_USER)
	{
		SendMessage(GetParent(hWnd), message, wParam, lParam);
		return FALSE;
	}
	return TRUE;
}

void CSonicWndEffect::OnTimerTransform()
{
	m_Effect.transform.nFrame++;
	Transform();
	if(m_Effect.transform.nFrame >= m_Effect.transform.nMaxFrame)
	{
		KillInternalTimer(TIMER_TRANSFORM);
		if(m_Effect.transform.bRestore)
		{
			RestoreTransform();
		}
		if(m_Effect.transform.bDirect)
		{
			m_Effect.dwEffectMask &= ~WND_EFFECT_TRANSFORM;
			SetWindowPos(m_hWnd, NULL, m_Effect.transform.rtDest.left, m_Effect.transform.rtDest.top, 
				m_Effect.transform.rtDest.Width(), m_Effect.transform.rtDest.Height(), SWP_NOACTIVATE | SWP_NOZORDER);
			UpdateLayer();
			m_Effect.transform.Clear();
		}
		FireDelegate(DELEGATE_EVENT_TRANSFORM_OVER);
	}
}

void CSonicWndEffect::RestoreTransform()
{
	m_Effect.dwEffectMask &= ~WND_EFFECT_TRANSFORM;
	SetWindowPos(m_hWnd, NULL, m_Effect.transform.rtOrigin.left, m_Effect.transform.rtOrigin.top, 
		m_Effect.transform.rtOrigin.Width(), m_Effect.transform.rtOrigin.Height(), SWP_NOZORDER | SWP_NOACTIVATE);
	UpdateLayer();
	m_Effect.transform.Clear();
}

BOOL CSonicWndEffect::EnablePreDrag(BOOL b)
{
	if(IsValid() == FALSE)
	{
		return FALSE;
	}
	if(b)
	{
		if(m_Effect.dwEffectMask & WND_EFFECT_PREDRAG)
		{
			return FALSE;
		}
		
		ISonicImage * pImg = g_UI.CreateImage();
		pImg->Create(1, 2, 0, TRUE);
		pImg->SetPixel(0, 0, ARGB(240, 0, 0, 0));
		for(int i = 0; i < 4; i++)
		{
			m_Effect.predrag.pImg[i] = g_UI.CreateImage();
			m_Effect.predrag.pImg[i]->CloneFrom(pImg);
		}
		m_Effect.predrag.pImg[1]->Rotate(90);
		m_Effect.predrag.pImg[2]->MirrorX();
		m_Effect.predrag.pImg[3]->Rotate(270);
		g_UI.DestroyObject(pImg);

		m_Effect.dwEffectMask |= WND_EFFECT_PREDRAG;
		m_Effect.predrag.hHelp = CreateEffectHelpWnd(EHT_PREDRAG, WS_POPUP);
	}
	else
	{
		if(!(m_Effect.dwEffectMask & WND_EFFECT_PREDRAG))
		{
			return FALSE;
		}
		m_Effect.dwEffectMask &= ~WND_EFFECT_PREDRAG;
		m_Effect.predrag.Clear();
	}
	return TRUE;
}

BOOL CSonicWndEffect::OnPreDragMsg(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_WINDOWPOSCHANGING:
		{
			LPWINDOWPOS pPos = (LPWINDOWPOS)lParam;
			if(!(pPos->flags & SWP_NOSIZE))
			{
				if(m_Effect.dwEffectMask & WND_EFFECT_PREDRAG)
				{
					m_Effect.predrag.pImg[0]->Extend(0, pPos->cy);
					m_Effect.predrag.pImg[1]->Extend(pPos->cx);
					m_Effect.predrag.pImg[2]->Extend(0, pPos->cy);
					m_Effect.predrag.pImg[3]->Extend(pPos->cx);
				}
			}
		}
		break;
	case WM_PAINT:
		{
			HDC hdc;
			PAINTSTRUCT ps;
			hdc = BeginPaint(hWnd, &ps);
			if(m_Effect.dwEffectMask & WND_EFFECT_PREDRAG)
			{
				int nSideWidth = m_Effect.predrag.pImg[0]->GetWidth();
				CRect rt;
				GetClientRect(hWnd, rt);
				m_Effect.predrag.pImg[0]->Draw(hdc, 0, 0);
				m_Effect.predrag.pImg[1]->Draw(hdc, 0, 0);
				m_Effect.predrag.pImg[2]->Draw(hdc, rt.right - nSideWidth, 0);
				m_Effect.predrag.pImg[3]->Draw(hdc, 0, rt.bottom -nSideWidth);
			}
			EndPaint(hWnd, &ps);
		}
		break;
	}
	return TRUE;
}

void CSonicWndEffect::Redraw()
{
	if(IsWindowVisible(m_hWnd))
	{
		InvalidateRect(m_hWnd, NULL, TRUE);
	}
	else if(m_Dib.IsValid())
	{
		UpdateDibLayer();
	}
}

void CSonicWndEffect::UpdateDibLayer(int x, int y, int cx, int cy, int xSrc /* = 0 */, int ySrc /* = 0 */, BOOL bRedraw)
{
	if(m_Dib.IsValid() == FALSE)
	{
		return;
	}
	if(bRedraw)
	{
		DrawLayer(bRedraw - 1);
	}
	POINT ptDst = {x, y};
	POINT ptSrc = {xSrc, ySrc};
	SIZE sz = {cx, cy};

	DWORD dwFlag = LWA_ALPHA;
	if(m_nKeyColor != -1)
	{
		dwFlag |= LWA_COLORKEY;
	}
	BYTE bConstantAlpha = QueryInternalTimer(TIMER_SHOWING_GENTLY) ? m_Effect.fadeout.nCurrAlpha : 255;
	BLENDFUNCTION blend;
	blend.AlphaFormat = AC_SRC_ALPHA;
	blend.BlendFlags = 0;
	blend.BlendOp = AC_SRC_OVER;
	blend.SourceConstantAlpha = bConstantAlpha;
	UpdateLayeredWindow(m_hWnd, NULL, &ptDst, &sz, m_Dib.GetSafeHdc(), &ptSrc, m_nKeyColor, &blend, dwFlag);
}

void CSonicWndEffect::UpdateDibLayer(BOOL bRedraw)
{
	if(m_Dib.IsValid() == FALSE)
	{
		return;
	}
	CRect rt;
	GetWindowRect(m_hWnd, rt);
	UpdateDibLayer(rt.left, rt.top, rt.Width(), rt.Height(), 0, 0, bRedraw);
}

void CSonicWndEffect::Transform()
{
	if(!(m_Effect.dwEffectMask & WND_EFFECT_TRANSFORM))
	{
		return;
	}
	int x, y;
	m_Effect.transform.Transform(x, y);
	CSonicWndEffect * pEffect = (CSonicWndEffect *)g_UI.EffectFromHwnd(m_Effect.transform.hHelp);
	if(pEffect)
	{
		pEffect->UpdateDibLayer(x, y, m_Effect.transform.pImgTransform->GetWidth(), m_Effect.transform.pImgTransform->GetHeight(), 0, 0, DRAW_LAYER_NOERASE);
	}
}

BOOL CSonicWndEffect::SlideShow(BOOL b, POINT ptFrom, POINT ptTo, int nFrame /* = 15 */, BOOL bActivted /* = FALSE */)
{
	if(IsValid() == FALSE)
	{
		return FALSE;
	}
	if(QueryInternalTimer(TIMER_SLIDE))
	{
		return FALSE;
	}
	m_Effect.slide.Clear();
	m_Effect.slide.nFrame = 0;
	m_Effect.slide.nMaxFrame = nFrame;
	m_Effect.slide.ptFrom = ptFrom;
	m_Effect.slide.ptTo = ptTo;
	m_Effect.slide.hHelp = CreateEffectHelpWnd(EHT_SLIDE, WS_POPUP);
	ISonicImage * pImg = Snapshot();
	if(ISonicWndEffect * pEffect = g_UI.EffectFromHwnd(m_Effect.slide.hHelp))
	{
		pEffect->SetShapeByImage(pImg);
	}

	if(b)
	{
		m_Effect.slide.nShow = bActivted ? SW_SHOW : SW_SHOWNA;
		if(m_Effect.slide.ptTo.x < m_Effect.slide.ptFrom.x && m_Effect.slide.ptFrom.x - m_Effect.slide.ptTo.x < pImg->GetWidth())
		{
			m_Effect.slide.ptTo.x = m_Effect.slide.ptFrom.x - pImg->GetWidth();
		}
		if(m_Effect.slide.ptTo.y < m_Effect.slide.ptFrom.y && m_Effect.slide.ptFrom.y - m_Effect.slide.ptTo.y < pImg->GetHeight())
		{
			m_Effect.slide.ptTo.y = m_Effect.slide.ptFrom.y - pImg->GetHeight();
		}
	}
	else
	{
		m_Effect.slide.nShow = SW_HIDE;
		if(m_Effect.slide.ptTo.x > m_Effect.slide.ptFrom.x && m_Effect.slide.ptTo.x - m_Effect.slide.ptFrom.x < pImg->GetWidth())
		{
			m_Effect.slide.ptFrom.x = m_Effect.slide.ptTo.x - pImg->GetWidth();
		}
		if(m_Effect.slide.ptTo.y > m_Effect.slide.ptFrom.y && m_Effect.slide.ptTo.y - m_Effect.slide.ptFrom.y < pImg->GetHeight())
		{
			m_Effect.slide.ptFrom.y = m_Effect.slide.ptTo.y - pImg->GetHeight();
		}
		SetWindowPos(m_Effect.slide.hHelp, NULL, ptFrom.x, ptFrom.y, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_SHOWWINDOW);
	}
	SetWindowPos(m_hWnd, NULL, 0, 0, 0, 0, SWP_NOZORDER | SWP_HIDEWINDOW | SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE);
	SetInternalTimer(TIMER_SLIDE, GIF_INTERVAL);
	g_UI.DestroyObject(pImg);
	return TRUE;
}

ISonicImage * CSonicWndEffect::Snapshot()
{
	if(IsValid() == FALSE)
	{
		return NULL;
	}
	ISonicImage * pImg = g_UI.CreateImage();
	CRect rt;
	GetWindowRect(m_hWnd, rt);
	pImg->Create(rt.Width(), rt.Height());

	//g_UI.DrawWindow(pImg->GetDC(), m_hWnd);
	SendMessage(m_hWnd, WM_PRINT, (WPARAM)pImg->GetDC(), PRF_NONCLIENT | PRF_CLIENT | PRF_ERASEBKGND | PRF_CHILDREN);
	if(m_Dib.IsValid() == FALSE)
	{
		if(m_nKeyColor != -1)
		{
			pImg->SetColorKey(m_nKeyColor);
		}
		else
		{
			CSSE::DoOr(0xff000000, pImg->GetBits(), pImg->GetWidth() * pImg->GetHeight() * 4);
		}
	}
    return pImg;
}

void CSonicWndEffect::OnTimerSlide()
{
	CSonicWndEffect * pEffect = (CSonicWndEffect *)g_UI.EffectFromHwnd(m_Effect.slide.hHelp);
	if(pEffect == NULL)
	{
		return;
	}
	if(pEffect->m_pBg == NULL)
	{
		return;
	}
	m_Effect.slide.nFrame++;
	int x1 = 0;
	int y1 = 0;
	int cx = pEffect->m_pBg->GetWidth();
	int cy = pEffect->m_pBg->GetHeight();
	int x = 0;
	int y = 0;

	if(m_Effect.slide.nShow == SW_HIDE)
	{
		if(m_Effect.slide.ptTo.x >= 0)
		{
			y = m_Effect.slide.ptFrom.y;
			if(m_Effect.slide.ptTo.x < m_Effect.slide.ptFrom.x)
			{
				x = m_Effect.slide.ptTo.x - cx;
				x = (x - m_Effect.slide.ptFrom.x) * m_Effect.slide.nFrame / m_Effect.slide.nMaxFrame + m_Effect.slide.ptFrom.x;	
				if(x < m_Effect.slide.ptTo.x)
				{
					x1 = m_Effect.slide.ptTo.x - x;
					cx -= x1;
					x = m_Effect.slide.ptTo.x;
				}
			}
			else
			{
				x = m_Effect.slide.ptTo.x;
				x = (x - m_Effect.slide.ptFrom.x) * m_Effect.slide.nFrame / m_Effect.slide.nMaxFrame + m_Effect.slide.ptFrom.x;
				if(x + cx > m_Effect.slide.ptTo.x)
				{
					cx -= x + cx  - m_Effect.slide.ptTo.x;
				}
			}
		}
		else if(m_Effect.slide.ptTo.y >= 0)
		{
			x = m_Effect.slide.ptFrom.x;
			if(m_Effect.slide.ptTo.y < m_Effect.slide.ptFrom.y)
			{
				y = m_Effect.slide.ptTo.y - cy;
				y = (y - m_Effect.slide.ptFrom.y) * m_Effect.slide.nFrame / m_Effect.slide.nMaxFrame + m_Effect.slide.ptFrom.y;
				if(y < m_Effect.slide.ptTo.y)
				{
					y1 = m_Effect.slide.ptTo.y - y;
					cy -= y1;
					y = m_Effect.slide.ptTo.y;
				}
			}
			else
			{
				y = m_Effect.slide.ptTo.y;
				y = (y - m_Effect.slide.ptFrom.y) * m_Effect.slide.nFrame / m_Effect.slide.nMaxFrame + m_Effect.slide.ptFrom.y;
				if(y + cy > m_Effect.slide.ptTo.y)
				{
					cy -= y + cy - m_Effect.slide.ptTo.y;
				}
			}
		}
	}
	else
	{
		if(m_Effect.slide.ptFrom.x >= 0)
		{
			y = m_Effect.slide.ptTo.y;
			if(m_Effect.slide.ptTo.x >= m_Effect.slide.ptFrom.x)
			{
				x = m_Effect.slide.ptFrom.x - cx;
				x = (m_Effect.slide.ptTo.x - x) * m_Effect.slide.nFrame / m_Effect.slide.nMaxFrame + x;
				if(x < m_Effect.slide.ptFrom.x)
				{
					x1 = m_Effect.slide.ptFrom.x - x;
					cx -= x1;
					x = m_Effect.slide.ptFrom.x;
				}
			}
			else
			{
				x = m_Effect.slide.ptFrom.x;
				x = (m_Effect.slide.ptTo.x - x) * m_Effect.slide.nFrame / m_Effect.slide.nMaxFrame + x;
				if(x + cx > m_Effect.slide.ptFrom.x)
				{
					cx -= x + cx - m_Effect.slide.ptFrom.x;
				}
			}
		}
		else if(m_Effect.slide.ptFrom.y >= 0)
		{
			x = m_Effect.slide.ptTo.x;
			if(m_Effect.slide.ptTo.y >= m_Effect.slide.ptFrom.y)
			{
				y = m_Effect.slide.ptFrom.y - cy;
				y = (m_Effect.slide.ptTo.y - y) * m_Effect.slide.nFrame / m_Effect.slide.nMaxFrame + y;
				if(y < m_Effect.slide.ptFrom.y)
				{
					y1 = m_Effect.slide.ptFrom.y - y;
					cy -= y1;
					y = m_Effect.slide.ptFrom.y;
				}
			}
			else
			{
				y = m_Effect.slide.ptFrom.y;
				y = (m_Effect.slide.ptTo.y - y) * m_Effect.slide.nFrame / m_Effect.slide.nMaxFrame + y;
				if(y + cy > m_Effect.slide.ptFrom.y)
				{
					cy -= y + cy - m_Effect.slide.ptFrom.y;
				}
			}
		}
	}
	pEffect->UpdateDibLayer(x, y, cx, cy, x1, y1, DRAW_LAYER_NOERASE);
	if(m_Effect.slide.nShow && m_Effect.slide.nFrame == 1)
	{
		ShowWindow(m_Effect.slide.hHelp, m_Effect.slide.nShow);
	}
	if(m_Effect.slide.nFrame >= m_Effect.slide.nMaxFrame)
	{
		KillInternalTimer(TIMER_SLIDE);
		if(m_Effect.slide.nShow)
		{
			SetWindowPos(m_hWnd, NULL, m_Effect.slide.ptTo.x, m_Effect.slide.ptTo.y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
			ShowWindow(m_hWnd, m_Effect.slide.nShow);
		}
		m_Effect.slide.Clear();

		FireDelegate(DELEGATE_EVENT_SLIDE_OVER);
	}
}

BOOL CSonicWndEffect::OnEraseBackground(HDC hdc)
{
	CRect rtUpdate;
	GetClipBox(hdc, rtUpdate);
	BOOL bRet = TRUE;
	if(m_Effect.dwEffectMask & WND_EFFECT_BKCOLOR)
	{
		if(m_Dib.IsValid())
		{
			m_Dib.Flush(m_dwBkColor);
			if(hdc != m_Dib.GetSafeHdc())
			{
				BitBlt(hdc, 0, 0, m_Dib.GetWidth(), m_Dib.GetHeight(), m_Dib.GetSafeHdc(), 0, 0, SRCCOPY);
			}
		}
		else
		{
			::SetBkColor(hdc, m_dwBkColor);
			::ExtTextOut(hdc, 0, 0, ETO_OPAQUE, rtUpdate, NULL, 0, NULL);
		}
		bRet = FALSE;
	}
	if(m_pBg)
	{
		BitBlt(hdc, rtUpdate.left, rtUpdate.top, rtUpdate.Width(), rtUpdate.Height(), m_pBg->GetDC(), 
			rtUpdate.left, rtUpdate.top, SRCCOPY);
		bRet = FALSE;
	}
	return bRet;
}

BOOL CSonicWndEffect::EnableTrackMouse(BOOL b)
{
	if(IsValid() == FALSE)
	{
		return FALSE;
	}
	if(b)
	{
		m_Effect.dwEffectMask |= WND_EFFECT_TRACK_MOUSE;
		SetInternalTimer(TIMER_TRACK_MOUSE, 100);
	}
	else
	{
		m_Effect.dwEffectMask &= ~WND_EFFECT_TRACK_MOUSE;
		KillInternalTimer(TIMER_TRACK_MOUSE);
	}
	return TRUE;
}

void CSonicWndEffect::OnTimerTrackMouse()
{
	POINT pt;
	GetCursorPos(&pt);
	if(GetTopParent(WindowFromPoint(pt)) == m_hWnd)
	{
		if(m_Effect.bHover == FALSE)
		{
			m_Effect.bHover = TRUE;
			OnMouseEnter();
		}
	}
	else
	{
		if(m_Effect.bHover)
		{
			m_Effect.bHover = FALSE;
			OnMouseLeave();
		}
	}
}

void CSonicWndEffect::OnMouseEnter()
{
	if(m_Effect.dwEffectMask & WND_EFFECT_FRAME)
	{
		if(m_Effect.frame.bType != 2 && m_Effect.frame.nCurrAlpha != 255)
		{
			AnimateFrame(TRUE);
		}
	}
	FireDelegate(DELEGATE_EVENT_MOUSEENTER);
}

void CSonicWndEffect::OnMouseLeave()
{
	if(m_Effect.dwEffectMask & WND_EFFECT_FRAME)
	{
		if(m_Effect.frame.bType == 0 ||
			(m_Effect.frame.bType == 1 && GetActiveWindow() == NULL))
		{
			AnimateFrame(FALSE);
		}
	}
	FireDelegate(DELEGATE_EVENT_MOUSELEAVE);
}

BOOL CSonicWndEffect::ConvertChildCtrl(HWND hChild, int nAttach /* = -1 */)
{
	if(IsValid() == FALSE)
	{
		return FALSE;
	}
	if(GetParent(hChild) != m_hWnd)
	{
		return FALSE;
	}
	DWORD dwStyle = GetWindowLong(m_hWnd, GWL_STYLE);
	if(dwStyle & WS_CHILD)
	{
		dwStyle = (dwStyle & ~WS_CHILD) | WS_POPUP;
		SetWindowLong(m_hWnd, GWL_STYLE, dwStyle);
	}
	ModifyChild(hChild);
	if(nAttach >= 0)
	{
		ISonicWndEffect * pEffect = g_UI.CreateWndEffect();
		pEffect->Attach(hChild, nAttach);
	}
	return TRUE;
}

DWORD CSonicWndEffect::GetWndEffectType()
{
	if(IsValid() == FALSE)
	{
		return 0;
	}
	return m_Effect.dwEffectMask;
}

BOOL CSonicWndEffect::DirectTransform(int nAngle, RECT * pDest, int nFrame)
{
	if(IsValid() == FALSE)
	{
		return FALSE;
	}
	if(m_Effect.dwEffectMask & WND_EFFECT_TRANSFORM)
	{
		return FALSE;
	}
	CRect rt(0, 0, 1, 1);
	if(pDest)
	{
		rt = *pDest;
	}
	if(nFrame == 0)
	{
		MoveWindow(m_hWnd, rt.left, rt.top, rt.Width(), rt.Height(), TRUE);
	}
	else
	{
		m_Effect.transform.Clear();
		m_Effect.dwEffectMask |= WND_EFFECT_TRANSFORM;
		m_Effect.transform.hHelp = CreateEffectHelpWnd(EHT_TRANSFORM, WS_POPUP);
		m_Effect.transform.bRestore = FALSE;
		m_Effect.transform.bDirect = TRUE;
		GetWindowRect(m_hWnd, m_Effect.transform.rtOrigin);
		m_Effect.transform.nAngle = nAngle;
		m_Effect.transform.pImgOrigin = Snapshot();
		m_Effect.transform.pImgTransform = g_UI.CreateImage();
		m_Effect.transform.pImgTransform->CloneFrom(m_Effect.transform.pImgOrigin);
		m_Effect.transform.rtDest = rt;
		SetWindowPos(m_Effect.transform.hHelp, NULL, m_Effect.transform.rtOrigin.left, m_Effect.transform.rtOrigin.top, 
			m_Effect.transform.pImgTransform->GetWidth(), m_Effect.transform.pImgTransform->GetHeight(), SWP_NOZORDER | SWP_SHOWWINDOW | SWP_NOACTIVATE);
		SetLayeredWindowAttributes(m_hWnd, 0, 0, LWA_ALPHA);
		m_Effect.transform.nFrame = 0;
		m_Effect.transform.nMaxFrame = nFrame;
		SetInternalTimer(TIMER_TRANSFORM, GIF_INTERVAL);		
	}
	return TRUE;
}

BOOL CSonicWndEffect::Shutter(BOOL bCross /* = TRUE */)
{
	if(IsValid() == FALSE)
	{
		return FALSE;
	}
	if(QueryInternalTimer(TIMER_SHUTTER))
	{
		return FALSE;
	}
	m_Effect.shutter.Clear();
	m_Effect.shutter.hHelp = CreateEffectHelpWnd(EHT_SHUTTER, WS_POPUP);
	m_Effect.shutter.bCross = bCross;
	m_Effect.shutter.pOrigin = Snapshot();
	CRect rt;
	GetWindowRect(m_hWnd, rt);
	m_Effect.shutter.rtOld = rt;
	m_Effect.shutter.pMask = g_UI.CreateImage();
	m_Effect.shutter.pMask->Create(rt.Width(), 2);
	m_Effect.shutter.nFrame = 0;
	SetWindowPos(m_Effect.shutter.hHelp, NULL, rt.left, rt.top, rt.Width(), rt.Height(), SWP_SHOWWINDOW | SWP_NOZORDER | SWP_NOACTIVATE);
	SetWindowPos(m_hWnd, NULL, 0, 0, 0, 0, SWP_HIDEWINDOW | SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE);
	SetInternalTimer(TIMER_SHUTTER, GIF_INTERVAL);
	return TRUE;
}

BOOL CSonicWndEffect::OnShutterMsg(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_PAINT:
		{
			HDC hdc;
			PAINTSTRUCT ps;
			hdc = BeginPaint(hWnd, &ps);
			if(m_Effect.shutter.nFrame == 0)
			{
				m_Effect.shutter.pOrigin->Draw(hdc);
			}
			else
			{
				CRect rt = m_Effect.shutter.rtOld;
				for(int i = 0; i < rt.Height(); i += 30)
				{
					int y = i + (30 - (m_Effect.shutter.nFrame - 1) * m_Effect.shutter.pMask->GetHeight());
					if(y < rt.Height())
					{
						BitBlt(hdc, 0, y, rt.Width(), m_Effect.shutter.pMask->GetHeight(), m_Effect.shutter.pMask->GetDC(), 0, 0, SRCCOPY);
					}
				}
			}
			EndPaint(hWnd, &ps);
		}
		break;
	}
	return TRUE;
}

void CSonicWndEffect::OnTimerShutter()
{
	CSonicWndEffect * pEffect = (CSonicWndEffect *)g_UI.EffectFromHwnd(m_Effect.shutter.hHelp);
	if(pEffect == NULL)
	{
		return;
	}
	m_Effect.shutter.nFrame++;
	pEffect->UpdateDibLayer(DRAW_LAYER_NOERASE);
	if(m_Effect.shutter.nFrame >= 15)
	{
		KillInternalTimer(TIMER_SHUTTER);
		m_Effect.shutter.Clear();
		FireDelegate(DELEGATE_EVENT_SHUTTER_OVER);
	}
}

BOOL CSonicWndEffect::IsAnimating()
{
	if(IsValid() == FALSE)
	{
		return FALSE;
	}
	return (m_dwTimerId & TIMER_SHUTTER) || (m_dwTimerId & TIMER_SHOWING_GENTLY) || (m_dwTimerId & TIMER_TRANSFORM) || (m_dwTimerId & TIMER_SLIDE);
}