// OnlineKeepTime.cpp : 实现文件
//

#include "stdafx.h"
#include "OnlineKeepTime.h"
#include ".\onlinekeeptime.h"


#define	ID_TIME_KEEP_TIME		100	// 计时

// COnlineKeepTime

IMPLEMENT_DYNAMIC(COnlineKeepTime, CWnd)
COnlineKeepTime::COnlineKeepTime()
{
	m_iTotalSecond = 0;
	m_iHour = 0;
	m_iMinuter = 0;
	m_iSecond = 0;
}

COnlineKeepTime::~COnlineKeepTime()
{
}

BEGIN_MESSAGE_MAP(COnlineKeepTime, CWnd)
	ON_WM_CREATE()
	ON_WM_TIMER()
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	ON_WM_DESTROY()
END_MESSAGE_MAP()

void COnlineKeepTime::SetKeepTime(int iTime)
{
	m_iTotalSecond = iTime;
	ReFigureNum(iTime);
	SetTimer(ID_TIME_KEEP_TIME, 1000, NULL);
	InvalidateRect(NULL);
}

void COnlineKeepTime::PlayStatus(BOOL bPlay)
{
	if (TRUE == bPlay)
	{
		SetTimer(ID_TIME_KEEP_TIME, 1000, NULL);
	}
	else
	{
		KillTimer(ID_TIME_KEEP_TIME);
	}

	InvalidateRect(NULL);
}

// COnlineKeepTime 消息处理程序
void COnlineKeepTime::DrawTimeNum(CDC *pDC, CRect rc, CPngImage *pImg, int iFrameCount, int iNum)
{
	if (NULL == pImg || pImg->IsNull() || FALSE==rc.IsRectEmpty() || 0 > iNum)
		return;

	int iFrameWidth = pImg->GetWidth()/iFrameCount;
	int iSignle = iNum%10;
	int iTen	= iNum/10;

	pImg->DrawImage(pDC, rc.left, rc.top, iFrameWidth, pImg->GetHeight(), iFrameWidth*iTen, 0);
	pImg->DrawImage(pDC, rc.left+iFrameWidth, rc.top, iFrameWidth, pImg->GetHeight(), iFrameWidth*iSignle, 0);
}

void COnlineKeepTime::ReFigureNum(int iTime)
{
	int iVal = iTime%3600;
	m_iHour = iTime/3600;
	m_iMinuter = iVal/60;
	m_iSecond = iVal%60;
}

int COnlineKeepTime::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码
	TCHAR szPath[MAX_PATH]={0}, szFileName[MAX_PATH]={0};
	GetCurrentDirectory(MAX_PATH, szPath);
	_snprintf(szFileName, sizeof(szFileName), TEXT("%s\\data\\gameframe\\plaza\\OnlineBackground.png"), szPath);
	m_imgBackground.LoadImage(szFileName);
	
	_snprintf(szFileName, sizeof(szFileName), TEXT("%s\\data\\gameframe\\plaza\\OnlineMinNum.png"), szPath);
	m_imgMinNumber.LoadImage(szFileName);
	_snprintf(szFileName, sizeof(szFileName), TEXT("%s\\data\\gameframe\\plaza\\OnlineMaxNum.png"), szPath);
	m_imgMaxNumber.LoadImage(szFileName);

	_snprintf(szFileName, sizeof(szFileName), TEXT("%s\\data\\gameframe\\plaza\\TimerMaohao.png"), szPath);
	m_imgMaohao.LoadImage(szFileName);

	CenterWindow(GetParent());
	return 0;
}

void COnlineKeepTime::OnTimer(UINT nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (ID_TIME_KEEP_TIME == nIDEvent)
	{
		ReFigureNum(--m_iTotalSecond);
		InvalidateRect(NULL);

		// 超过这个时间没必要再计时咯！
		if (0>=m_iTotalSecond)
		{
			KillTimer(ID_TIME_KEEP_TIME);
		}
		return;
	}

	CWnd::OnTimer(nIDEvent);
}

void COnlineKeepTime::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 在此处添加消息处理程序代码
	// 不为绘图消息调用 CWnd::OnPaint()
	CDC memdc;
	CBitmap bmp;
	CRect rect;
	GetClientRect(&rect);
	bmp.CreateCompatibleBitmap(&dc, rect.Width(), rect.Height());
	memdc.CreateCompatibleDC(&dc);
	memdc.SelectObject(bmp);

	memdc.FillSolidRect(&rect, RGB(0,0,0));

	// 背景
	m_imgBackground.DrawImage(&memdc, 0, 15);
	
	// 小数字--时
	int iStart = m_imgBackground.GetWidth();
	DrawTimeNum(&memdc, CRect(iStart,19,iStart+30,0), &m_imgMinNumber, 10, m_iHour);

	// 冒号
	m_imgMaohao.DrawImage(&memdc, iStart+28, 19);

	// 小数字--分
	iStart += 40;
	DrawTimeNum(&memdc, CRect(iStart,19,iStart+30,0), &m_imgMinNumber, 10, m_iMinuter);

	// 冒号
	m_imgMaohao.DrawImage(&memdc, iStart+28, 19);

	// 大数字--秒
	iStart += 40;
	DrawTimeNum(&memdc, CRect(iStart,0,iStart+60,0), &m_imgMaxNumber, 10, m_iSecond);

	UpdateRgn(&memdc);
	dc.BitBlt(0, 0, rect.Width(), rect.Height(), &memdc, 0, 0, SRCCOPY);
	bmp.DeleteObject();
	memdc.DeleteDC();
}

BOOL COnlineKeepTime::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	return FALSE;
	return CWnd::OnEraseBkgnd(pDC);
}

void COnlineKeepTime::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);
	// TODO: 在此处添加消息处理程序代码
	SetWindowPos(&wndTop, 0, 0, 220, 40, SWP_NOMOVE);
}

void COnlineKeepTime::AddRectRgn(CDC* pDC,CRgn* rgn,CRect* rect,COLORREF col)
{
	for(int y=rect->top;y<rect->bottom;y++)
	{
		for(int x=rect->left;x<rect->right;x++)
		{
			if (pDC->GetPixel(x,y)==col)
			{
				CRgn rgnAdd;
				rgnAdd.CreateRectRgn (x, y, x+1, y+1);
				rgn->CombineRgn(rgn, &rgnAdd, RGN_DIFF);
				DeleteObject(rgnAdd);
			}
		}
	}
}

void COnlineKeepTime::UpdateRgn(CDC * pDC)
{
	CRect rcWnd;
	GetClientRect(&rcWnd);

	CRgn rgn;
	COLORREF col = RGB(0,0,0);  // mask color
	rgn.CreateRectRgn (0, 0, rcWnd.Width(), rcWnd.Height());

	//左上
	AddRectRgn(pDC,&rgn,&rcWnd,col);
	SetWindowRgn(rgn,TRUE);
	rgn.DeleteObject();
	//SetForegroundWindow();
}


void COnlineKeepTime::OnDestroy()
{
	CWnd::OnDestroy();

	if (!m_imgBackground.IsNull())
	{
		m_imgBackground.DestroyImage();
	}
	if (!m_imgMaohao.IsNull())
	{
		m_imgMaohao.DestroyImage();
	}
	if (!m_imgMinNumber.IsNull())
	{
		m_imgMinNumber.DestroyImage();
	}
	if (!m_imgMaxNumber.IsNull())
	{
		m_imgMaxNumber.DestroyImage();
	}
}
