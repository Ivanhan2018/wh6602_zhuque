// Child.cpp : 实现文件
//

#include "stdafx.h"
#include "CHeadPortrait.h"

#define			PICTURE_COUNT			1								// 图片数目

#define			TIMER_PLAY					100							// 播放计时器
#define			TIMER_RESUME			101							// 恢复默认计时器
#define			Timer_INTERVAL			102							// 动画间隔
// CHeadPortrait

IMPLEMENT_DYNAMIC(CHeadPortrait, CWnd)
CHeadPortrait::CHeadPortrait()
{
	m_pParent = NULL;
	m_pParentDC = NULL;
	m_pParentImage = NULL;
	m_pImage = NULL;

	m_uCurFrame = 0;
	m_uCurAction = 0;
	m_uRunCount = 1;
}

CHeadPortrait::~CHeadPortrait()
{
}


BEGIN_MESSAGE_MAP(CHeadPortrait, CWnd)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_TIMER()
END_MESSAGE_MAP()



// CHeadPortrait 消息处理程序

void CHeadPortrait::SetAction(UINT bLine/*=7*/, UINT bRow/*=2*/, BOOL *pAction/*=NULL*/)
{
	m_uActionCount = bLine;
	m_uFrameCount = bRow;
	m_vFrames.clear();

	if (7==bLine  &&  2==bRow  &&  NULL==pAction)
	{
		for (UINT i=0; i<7*2; i++)
		{
			m_vFrames.push_back(TRUE);
		}
	}
	else
	{
		for (UINT i=0; i<bLine; i++)
		{
			for (UINT j=0; j<bRow; j++)
			{
				m_vFrames.push_back( NULL==pAction  ?  TRUE  :  (*(pAction + i*j)) );
			}
		}
	}
}

void CHeadPortrait::PlayAction(UINT uIndex, UINT uFrameTime/*=150*/, UINT uIntervalTime/*=0*/, UINT uResultTime/*=3000*/, UINT uDefaultAction/*=0*/)
{
	ASSERT((0 < uResultTime  &&  uIntervalTime <= uResultTime)  ||  (0 >= uResultTime));
	if (uIntervalTime > uResultTime  &&  0 < uResultTime)
	{
		uResultTime = uIntervalTime;
	}

	m_uRunCount = 1;
	m_uCurFrame = 0;
	m_uCurAction = uIndex;
	m_uDefaultAction = uDefaultAction;
	m_uIntervalTime = uIntervalTime;
	m_uFrameTime = uFrameTime;
	InvalidateRect(NULL);

	if (GetSafeHwnd() && !IsWindowVisible())
	{
		ShowWindow(SW_SHOW);
	}

	KillTimer(TIMER_PLAY);
	KillTimer(TIMER_RESUME);

	SetTimer(TIMER_PLAY, m_uFrameTime, NULL);
	if (0 < uResultTime)
	{
		SetTimer(TIMER_RESUME, uResultTime, NULL);
	}
}

void CHeadPortrait::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	CRect rcWnd;
	GetClientRect(&rcWnd);

	CDC BufferDC;
	CBitmap BufferBitmap;
	BufferDC.CreateCompatibleDC(&dc);
	BufferBitmap.CreateCompatibleBitmap(&dc, rcWnd.Width(), rcWnd.Height());
	BufferDC.SelectObject(&BufferBitmap);
	BufferDC.BitBlt(0, 0, rcWnd.Width(), rcWnd.Height(), &dc, 0, 0, SRCCOPY);

	ASSERT(NULL != m_pParentDC  ||  NULL != m_pParentImage);
	if (NULL != m_pParentImage)
	{
		m_pParentImage->BitBlt(BufferDC.GetSafeHdc(), 0, 0, rcWnd.Width(), rcWnd.Height(), m_ptCurPos.x, m_ptCurPos.y, SRCCOPY);
	}
	else if (NULL != m_pParentDC)
	{
		BufferDC.BitBlt(0, 0, rcWnd.Width(), rcWnd.Height(), m_pParentDC, m_ptCurPos.x, m_ptCurPos.y, SRCCOPY);
	}

	if (NULL != m_pImage)
	{		
		int iX = m_pImage->GetWidth()/m_uFrameCount;
		int iY = m_pImage->GetHeight()/m_uActionCount;		
		m_pImage->DrawImage(&BufferDC, 0, 0, iX, iY, m_uCurFrame*iX, m_uCurAction*iY, iX, iY, 1);
	}

	dc.BitBlt(0, 0, rcWnd.Width(), rcWnd.Height(), &BufferDC, 0, 0, SRCCOPY);
	BufferDC.DeleteDC();
	BufferBitmap.DeleteObject();
}

BOOL CHeadPortrait::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext)
{
	m_pParent = pParentWnd;
	return CWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
}

BOOL CHeadPortrait::OnEraseBkgnd(CDC* pDC)
{
	return CWnd::OnEraseBkgnd(pDC);
}

void CHeadPortrait::OnTimer(UINT nIDEvent)
{
	if (TIMER_PLAY == nIDEvent)
	{
		// 动画播完一次，进行间隔
		if (1+m_uCurFrame >= m_uFrameCount)
		{
			if (3<m_uRunCount)
			{
				KillTimer(TIMER_PLAY);
				// 默认动作，随机间隔时间
				if (m_uDefaultAction == m_uCurAction)
				{
					srand((unsigned)time(NULL));
					SetTimer(Timer_INTERVAL, (rand()%5+5)*1000, NULL);
				}
				else
				{
					SetTimer(Timer_INTERVAL, m_uIntervalTime, NULL);
				}
				
				
				m_uCurFrame = 0;
				m_uRunCount = 1;
				InvalidateRect(NULL);
				return;
			}
			else
			{
				++m_uRunCount;
			}
		}

		m_uCurFrame = (1+m_uCurFrame)%m_uFrameCount;
		InvalidateRect(NULL);
		return ;
	}
	else if (Timer_INTERVAL == nIDEvent) // 动画间隔
	{
		KillTimer(Timer_INTERVAL);
		SetTimer(TIMER_PLAY, m_uFrameTime, NULL);

		return;
	}
	else if (TIMER_RESUME == nIDEvent)
	{
		// 动画结束转到默认动画
		KillTimer(TIMER_RESUME);
		KillTimer(TIMER_PLAY);
		KillTimer(Timer_INTERVAL);
		srand((unsigned)time(NULL));
		SetTimer(TIMER_PLAY, m_uFrameTime, NULL);

		m_uCurAction = m_uDefaultAction;
		m_uCurFrame = 0;
		m_uRunCount = 1;
		InvalidateRect(NULL);
		return ;
	}

	CWnd::OnTimer(nIDEvent);
}


//////////////////////////////////////////////////////////////////////////////////////
/// CHeadPortraitManager 人动物头像管理类

CHeadPortraitManager::CHeadPortraitManager()
{
	m_vHeadPortrait.clear();
}

CHeadPortraitManager::~CHeadPortraitManager()
{
	try
	{
		for (VCHeadPortrait::iterator vbeg=m_vHeadPortrait.begin(); vbeg!=m_vHeadPortrait.end(); vbeg++)
		{
			if (NULL != (*vbeg))
			{
				delete (*vbeg);
			}		

			m_vHeadPortrait.erase(vbeg);
		}
	}
	catch (...)
	{
		OutputDebugString("CHeadPortraitManager::~CHeadPortraitManager() 释放资源失败");
	}

	m_vHeadPortrait.clear();
}

void CHeadPortraitManager::Init(CWnd *pParent, UINT uCount)
{
	ASSERT(0 < uCount);
	ASSERT(NULL != pParent);
	if (0 >= uCount  ||  NULL==pParent)
	{
		return;
	}

	try
	{
		for (UINT i=0; i<uCount; i++)
		{
			CHeadPortrait * pObject = new CHeadPortrait;
			if (NULL != pObject)
			{
				pObject->SetAction(6, 3, NULL);
				pObject->Create(NULL, NULL, WS_CHILD, CRect(0,0,0,0), pParent, 500+m_vHeadPortrait.size());
				pObject->m_wID = m_vHeadPortrait.size();
				m_vHeadPortrait.push_back(pObject);
			}
		}
	}
	catch (...)
	{
		OutputDebugString("CHeadPortrait::Init 函数错误");
	}
}

void CHeadPortraitManager::SetPos(CPoint pt[])
{
	for (VCHeadPortrait::size_type i=0; i<m_vHeadPortrait.size(); i++)
	{
		CHeadPortrait * pObject = m_vHeadPortrait.at(i);
		if (NULL != pObject  &&  NULL!=pObject->GetSafeHwnd())
		{
			pObject->m_ptCurPos.x = pt[i].x;
			pObject->m_ptCurPos.y = pt[i].y;
			pObject->SetWindowPos(NULL, pt[i].x, pt[i].y, 0, 0, SWP_NOZORDER|SWP_NOSIZE);
		}
	}
}

void CHeadPortraitManager::Refresh(CDC *pDC)
{
	ASSERT(NULL != pDC);
	if (NULL == pDC)
	{
		return;
	}

	for (VCHeadPortrait::size_type i=0; i<m_vHeadPortrait.size(); i++)
	{
		CHeadPortrait * pObject = m_vHeadPortrait.at(i);
		if (NULL != pObject  &&  NULL!=pObject->GetSafeHwnd())
		{
			pObject->m_pParentDC = pDC;
			pObject->InvalidateRect(NULL);
		}
	}
}

void CHeadPortraitManager::Refresh(CImage *pImage)
{
	ASSERT(NULL != pImage);
	if (NULL == pImage)
	{
		return;
	}

	for (VCHeadPortrait::size_type i=0; i<m_vHeadPortrait.size(); i++)
	{
		CHeadPortrait * pObject = m_vHeadPortrait.at(i);
		if (NULL != pObject  &&  NULL!=pObject->GetSafeHwnd())
		{
			pObject->m_pParentImage = pImage;
			pObject->InvalidateRect(NULL);
		}
	}
}
void CHeadPortraitManager::PlayAction(WORD wID, BOOL bWoman, UINT uIndex, UINT uFrameTime/*=150*/, UINT uIntervalTime/*=0*/, UINT uResultTime/*=0*/, UINT uDefaultAction/*=0*/)
{
	ASSERT(0 < uFrameTime);
	if (0 >= uFrameTime)
	{
		return;
	}

	if (0<=wID  &&  m_vHeadPortrait.size()>wID)
	{
		CHeadPortrait * pObject = m_vHeadPortrait.at(wID);
		if (NULL != pObject  &&  NULL!=pObject->GetSafeHwnd())
		{
			if (NULL== pObject->m_pImage)
			{
				srand((unsigned)time(NULL));
				CString strPath, strDirection;
				switch (wID)
				{
				case 0: strDirection.Format(TEXT("right_"));break;
				case 1: 
				case 3: strDirection.Format(TEXT("topbottom_"));break;break;
				case 2: strDirection.Format(TEXT("left_"));break;
				default:ASSERT(0);
				}

				strPath.Format(TEXT("data\\gameframe\\headportrait\\%s%s%d.png"), (bWoman?"woman_":"man_"), strDirection, rand()%PICTURE_COUNT);
				pObject->m_pImage = new CGDIPlus_Image;
				pObject->m_pImage->LoadFromFile(strPath.AllocSysString());
				pObject->SetWindowPos(NULL, 0, 0, 
					pObject->m_pImage->GetWidth()/pObject->m_uFrameCount, 
					pObject->m_pImage->GetHeight()/pObject->m_uActionCount, 
					SWP_NOZORDER|SWP_NOMOVE);
			}

			pObject->PlayAction(uIndex, uFrameTime, uIntervalTime, uResultTime, uDefaultAction);
		}
	}
}

void CHeadPortraitManager::Hide(WORD uID, BOOL bAllHide/*=FALSE*/)
{
	for (VCHeadPortrait::size_type i=0; i<m_vHeadPortrait.size(); i++)
	{
		CHeadPortrait * pObject = m_vHeadPortrait.at(i);
		if (NULL != pObject  &&  NULL!=pObject->GetSafeHwnd())
		{
			if (bAllHide)
			{
				pObject->ShowWindow(SW_HIDE);
			}
			else if (uID  ==  pObject->m_wID)
			{
				pObject->ShowWindow(SW_HIDE);
				break;
			}
		}
	}

	return;
}

