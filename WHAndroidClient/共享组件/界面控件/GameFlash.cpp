// GameFlash.cpp : 实现文件
//

#include "stdafx.h"
#include "GameFlash.h"
#include ".\gameflash.h"

//定时器定义
const int IFRAMEFLASH = 1001;
const int IALPHAFLASH = 1002;
const int ISTOPFLASH  = 1003;

// CGameFlash 对话框

IMPLEMENT_DYNAMIC(CGameFlash, CDialog)
CGameFlash::CGameFlash(CWnd* pParent /*=NULL*/)
: CDialog(CGameFlash::IDD, pParent)
{
	m_iTotalFrame = 0;
	m_iCurFrame = 0;
	m_iPlayMode = 1;
	m_iSaveTransparent = 100;
	m_iTransparent = 100;
	m_iLoopCount = 0;
	m_iLoopNums = 0;

	m_iGFTimeInterval	=	100;
	m_iStopTimeInterval	=	1000;
	m_iAlphaTimeInterval=	100;
}

CGameFlash::~CGameFlash()
{
	m_pImageBack.DestroyImage();
	::FreeLibrary(hFuncInst);
}

void CGameFlash::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CGameFlash, CDialog)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CGameFlash 消息处理程序

void CGameFlash::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 在此处添加消息处理程序代码
	// 不为绘图消息调用 CDialog::OnPaint()
}

BOOL CGameFlash::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	return TRUE;
	//return CDialog::OnEraseBkgnd(pDC);
}



BOOL CGameFlash::OnInitDialog()
{
	CDialog::OnInitDialog();

	hFuncInst = LoadLibrary("User32.DLL"); 
	BOOL bRet=FALSE;
	if(hFuncInst) 
		UpdateLayeredWindow=(MYFUNC)GetProcAddress(hFuncInst, "UpdateLayeredWindow");
	else
	{
		AfxMessageBox("User32.dll ERROR!");
		exit(0);
	}
	//初始化gdiplus的环境
	// Initialize GDI+.
	m_Blend.BlendOp=0; //theonlyBlendOpdefinedinWindows2000
	m_Blend.BlendFlags=0; //nothingelseisspecial...
	m_Blend.AlphaFormat=AC_SRC_ALPHA; //...
	m_Blend.SourceConstantAlpha=255;//AC_SRC_ALPHA

	//BYTE bTran = 5;
	//SetLayeredWindowAttributes( m_hWnd, 0, bTran, LWA_ALPHA);
	// TODO:  在此添加额外的初始化

	m_iGFTimeInterval	=	100;
	m_iStopTimeInterval	=	1000;
	m_iAlphaTimeInterval=	100;

	m_iTotalFrame = 0;
	m_iCurFrame = 0;
	m_iPlayMode = 1;
	m_iSaveTransparent = 100;
	m_iTransparent = 100;
	m_iLoopCount = 0;
	m_iLoopNums = 0;

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


void CGameFlash::UpdateClockDisplay()
{
	
	CDC* pWndDC = GetDC();
	CDC memdc;
	memdc.CreateCompatibleDC(pWndDC);
	CBitmap hBitMap;
	hBitMap.CreateCompatibleBitmap(pWndDC,m_BakWidth,m_BakHeight);
	memdc.SelectObject(&hBitMap);
	memdc.SetBkMode(TRANSPARENT);

	if (!m_pImageBack.IsNull())
	{
		m_pImageBack.DrawImage(&memdc, 0, 0, m_BakWidth,m_BakHeight, m_iCurFrame*m_BakWidth, 0, m_BakWidth,m_BakHeight);
	}

	m_Blend.SourceConstantAlpha=int(m_iTransparent*2.55);//1~255
	HDC hdcScreen=pWndDC->m_hDC;
	RECT rct;
	GetWindowRect(&rct);
	POINT ptWinPos={rct.left,rct.top};
	SIZE sizeWindow={m_BakWidth,m_BakHeight};
	POINT ptSrc={0,0};
	DWORD dwExStyle=GetWindowLong(m_hWnd,GWL_EXSTYLE);
	if((dwExStyle&0x80000)!=0x80000)
		SetWindowLong(m_hWnd,GWL_EXSTYLE,dwExStyle^0x80000);

	BOOL bRet=FALSE;
	bRet= UpdateLayeredWindow( m_hWnd,hdcScreen,&ptWinPos,&sizeWindow,memdc.m_hDC,&ptSrc,0,&m_Blend,2);
	::DeleteObject(hBitMap);

	::DeleteDC(memdc.GetSafeHdc());
	memdc.m_hDC = NULL;

	::ReleaseDC(m_hWnd, pWndDC->GetSafeHdc());
	pWndDC = NULL;
}


bool CGameFlash::InitFlash(CString pImage, int iTotalFrame, CPoint ptClient, int iTransparent)
{
	if (!pImage.IsEmpty() && iTotalFrame>0)
	{
		m_pImageBack.DestroyImage();
		m_pImageBack.LoadImage(pImage);
		if (m_pImageBack.IsNull())
		{
			return false;
		}
		m_BakWidth=m_pImageBack.GetWidth()/iTotalFrame;
		m_BakHeight=m_pImageBack.GetHeight();

		if (0==m_BakWidth && m_BakWidth==m_BakHeight)
		{
			return false;
		}
		SetWindowPos(NULL, 0, 0, m_BakWidth, m_BakHeight, SWP_NOMOVE);

		m_iLoopCount = 0;
		m_iLoopNums = 0;
		m_pClientPos = ptClient;
		m_iCurFrame = 0;
		m_iPlayMode = 1;
		m_iTotalFrame = iTotalFrame;
		m_iTransparent = iTransparent;
		m_iSaveTransparent = iTransparent;
		UpdateClockDisplay();
		return true;
	}
	return false;
}

//set pt
void CGameFlash::SetFlashPosition(CPoint ptClient)
{
	m_pClientPos = ptClient;
}

//time interval
void CGameFlash::SetTimeInterval(int iGFTimeInterval, int iStopTimeInterval, int iAlphaTimeInterval)
{
	m_iGFTimeInterval	=	iGFTimeInterval;
	m_iStopTimeInterval	=	iStopTimeInterval;
	m_iAlphaTimeInterval=	iAlphaTimeInterval;
}


//播放动画
void CGameFlash::Play(bool bIsPlay, int iPlayMode, int iLoopCount)
{
	if (iPlayMode<0 || iPlayMode>=3)
	{
		return;
	}
	KillTimer(IFRAMEFLASH);
	KillTimer(IALPHAFLASH);
	KillTimer(ISTOPFLASH);

	m_iLoopCount = 0;
	m_iLoopNums = 0;
	if (bIsPlay)
	{
		if (0==m_BakWidth && m_BakWidth==m_BakHeight)
		{
			return;
		}

		CRect rect;
		CWnd *pMain = AfxGetMainWnd();
		if (NULL != pMain)
		{
			pMain->GetWindowRect(&rect);
		}
		SetWindowPos(NULL, rect.left+m_pClientPos.x, rect.top+m_pClientPos.y, m_BakWidth, m_BakHeight, SWP_NOSIZE);

		m_iCurFrame = 0;
		m_iPlayMode = iPlayMode;
		m_iTransparent = m_iSaveTransparent;
		m_iLoopNums = iLoopCount;
		m_iLoopCount = 0;

		UpdateClockDisplay();

		ShowWindow(SW_SHOW);

		SetTimer(IFRAMEFLASH, m_iGFTimeInterval, NULL);
	}
	else
	{
		ShowWindow(SW_HIDE);
	}
}

void CGameFlash::OnTimer(UINT nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CWnd *pMain = AfxGetMainWnd();
	if (NULL != pMain)
	{
		CRect rect;
		pMain->GetWindowRect(&rect);
		switch(nIDEvent)
		{
		case IFRAMEFLASH:	//帧动画
			{
				if (m_iCurFrame < m_iTotalFrame-1)
				{
					++m_iCurFrame;
				}
				else
				{
					m_iCurFrame = m_iTotalFrame-1;
					KillTimer(IFRAMEFLASH);
					KillTimer(IALPHAFLASH);
					KillTimer(ISTOPFLASH);
					if (0==m_iPlayMode)
					{
						if (m_iLoopNums<0)
						{
							Play(false);
							break;
						}
						else if (0 == m_iLoopNums)
						{//循环无限次
							m_iLoopCount = 0;
						}
						else if (m_iLoopCount == m_iLoopNums)
						{//循环次数
							Play(false);
							break;
						}
						m_iCurFrame = 0;
						++m_iLoopCount;
						SetTimer(IFRAMEFLASH, m_iGFTimeInterval, NULL);
					}
					else if (1 == m_iPlayMode)
					{
						SetTimer(IALPHAFLASH, m_iAlphaTimeInterval, NULL);
					}
					else if (2 == m_iPlayMode)
					{
						SetTimer(ISTOPFLASH, m_iStopTimeInterval, NULL);
					}
				}

				UpdateClockDisplay();
				SetWindowPos(NULL, rect.left+m_pClientPos.x, rect.top+m_pClientPos.y, m_BakWidth, m_BakHeight, SWP_SHOWWINDOW);
			}
			break;
		case IALPHAFLASH:	//ALPHA动画
			{
				if (m_iTransparent>0)
				{
					m_iTransparent -= 10;
					UpdateClockDisplay();

					SetWindowPos(NULL, rect.left+m_pClientPos.x, rect.top+m_pClientPos.y, m_BakWidth, m_BakHeight, SWP_SHOWWINDOW);
				}
				else
				{
					Play(false);
				}
			}
			break;
		case ISTOPFLASH:	//STOP
			{
				Play(false);
			}
			break;
		default:
			break;
		}
	}
	CDialog::OnTimer(nIDEvent);
}
