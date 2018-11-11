// TaskFinishFlash.cpp : 实现文件
//

#include "stdafx.h"
#include "TaskFinishFlash.h"
#include ".\taskfinishflash.h"
#include "TraceCatch.h"


//总帧数
const int INUMS = 10;


//定时器定义
const int IFRAMEFLASH = 1001;
const int IALPHAFLASH = 1003;


//时间间隔
const int IFRAMETIME = 50;
const int IALPHATIME = 100;


//数字显示位置
const int INUMSHOWX = 590;
const int INUMSHOWY = 110; 

//数字宽与高
const int INUMSWIDTH = 50;
const int INUMSHEIGHT = 65;


// CTaskFinishFlash 对话框

IMPLEMENT_DYNAMIC(CTaskFinishFlash, CDialog)
CTaskFinishFlash::CTaskFinishFlash(CWnd* pParent /*=NULL*/)
	: CDialog(CTaskFinishFlash::IDD, pParent)
{
	m_vecNumList.clear();

	m_bIsShowText = false;

	m_hdcMemory.m_hDC = NULL;
}

CTaskFinishFlash::~CTaskFinishFlash()
{
	::FreeLibrary(hFuncInst);
}

void CTaskFinishFlash::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CTaskFinishFlash, CDialog)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CTaskFinishFlash 消息处理程序

void CTaskFinishFlash::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 在此处添加消息处理程序代码
	// 不为绘图消息调用 CDialog::OnPaint()
}

BOOL CTaskFinishFlash::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	return TRUE;
	//return CDialog::OnEraseBkgnd(pDC);
}



BOOL CTaskFinishFlash::OnInitDialog()
{
	CDialog::OnInitDialog();

	__ENTER_FUNCTION

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
	m_Blend.AlphaFormat=1; //...
	m_Blend.SourceConstantAlpha=255;//AC_SRC_ALPHA

	// TODO:  在此添加额外的初始化

	char pPath[64] = "data\\gameframe\\plaza";
	TCHAR szResourcePath[MAX_PATH];	GetCurrentDirectory(MAX_PATH,szResourcePath);
	CString strTalks = "";
	strTalks.Format("%s\\%s\\ImageTFFGG.png", szResourcePath, pPath);
	m_pImageBack.LoadImage(strTalks);

	strTalks = "";
	strTalks.Format("%s\\%s\\ImageTFFGGText.png", szResourcePath, pPath);
	m_pImageText.LoadImage(strTalks);

	strTalks = "";
	strTalks.Format("%s\\%s\\ImageTFFGGNums.png", szResourcePath, pPath);
	m_pImageNums.LoadImage(strTalks);

	m_BakWidth = 0;
	m_BakHeight = 0;
	if (!m_pImageBack.IsNull())
	{
		m_BakWidth=m_pImageBack.GetWidth()/INUMS;
		m_BakHeight=m_pImageBack.GetHeight();
	}
	if (0==m_BakWidth && m_BakWidth==m_BakHeight)
	{
		return TRUE;
	}
	SetWindowPos(NULL, 0, 0, m_BakWidth, m_BakHeight, SWP_NOMOVE);

	m_iCurFrame = 0;
	m_iTransparent = 100;
	UpdateClockDisplay(m_iTransparent);

	m_vecNumList.clear();
	m_bIsShowText = false;

	__LEAVE_FUNCTION
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


BOOL CTaskFinishFlash::UpdateClockDisplay(int Transparent)
{
	BOOL bRet=FALSE;

	__ENTER_FUNCTION

	CThreadGuard Guard(&m_CriticalSpection);

	CDC* pWndDC = GetDC();
	if (NULL==pWndDC)
	{
		ReleaseDC(pWndDC);
		pWndDC->m_hDC = NULL;
		pWndDC = NULL;
		return false;
	}
	m_hdcMemory.CreateCompatibleDC(pWndDC);
	HBITMAP hBitMap=::CreateCompatibleBitmap(pWndDC->m_hDC,m_BakWidth,m_BakHeight);
	SelectObject(m_hdcMemory,hBitMap);

	if(Transparent<0||Transparent>100)	Transparent=0;

	m_Blend.SourceConstantAlpha=int(Transparent*2.55);//1~255
	HDC hdcScreen=pWndDC->GetSafeHdc();
	RECT rct;
	GetWindowRect(&rct);
	POINT ptWinPos={rct.left,rct.top};

	if (!m_pImageBack.IsNull())
	{
		m_pImageBack.DrawImage(&m_hdcMemory, 0, 0, m_BakWidth,m_BakHeight, m_iCurFrame*m_BakWidth, 0, m_BakWidth,m_BakHeight);
	}
	if (m_bIsShowText)
	{//绘制文本
		DrawText(&m_hdcMemory);
	}

	SIZE sizeWindow={m_BakWidth,m_BakHeight};
	POINT ptSrc={0,0};
	DWORD dwExStyle=GetWindowLong(m_hWnd,GWL_EXSTYLE);
	if((dwExStyle&0x80000)!=0x80000)
		SetWindowLong(m_hWnd,GWL_EXSTYLE,dwExStyle^0x80000);

	
	bRet= UpdateLayeredWindow( m_hWnd,hdcScreen,&ptWinPos,&sizeWindow,m_hdcMemory.m_hDC,&ptSrc,0,&m_Blend,2);
	hdcScreen=NULL;

	DeleteObject(hBitMap);
	//DeleteDC(m_hdcMemory.GetSafeHdc());
	ReleaseDC(pWndDC);
	pWndDC->m_hDC = NULL;
	pWndDC = NULL;
	m_hdcMemory.DeleteDC();
	m_hdcMemory.m_hDC = NULL;

	__LEAVE_FUNCTION

	return bRet;
}

//绘制文本
void CTaskFinishFlash::DrawText(CDC* pDC)
{
	__ENTER_FUNCTION

	CThreadGuard Guard(&m_CriticalSpection);

	if (!m_pImageText.IsNull())
	{
		m_pImageText.DrawImage(&m_hdcMemory, 160, 86);
	}
	int iNumBit = m_vecNumList.size();
	if (!m_pImageNums.IsNull() && iNumBit>0)
	{
		UINT uiTotalLen = iNumBit*INUMSWIDTH;
		UINT uiNumStartX = INUMSHOWX - uiTotalLen/2;

		std::vector<int>::reverse_iterator iter = m_vecNumList.rbegin();
		std::vector<int>::reverse_iterator iterend = m_vecNumList.rend();
		while(iter != iterend)
		{
			m_pImageNums.DrawImage(pDC, uiNumStartX, INUMSHOWY, INUMSWIDTH, INUMSHEIGHT, (*iter)*INUMSWIDTH, 0, INUMSWIDTH, INUMSHEIGHT);
			++iter;
			uiNumStartX += INUMSWIDTH;
		}
	}

	__LEAVE_FUNCTION
}

//播放动画
void CTaskFinishFlash::Play(bool bIsPlay, UINT uiNums)
{

	__ENTER_FUNCTION

	m_bIsShowText = false;
	m_vecNumList.clear();
	if (bIsPlay)
	{
		KillTimer(IFRAMEFLASH);
		KillTimer(IALPHAFLASH);

		do 
		{
			m_vecNumList.push_back(uiNums%10);
			uiNums = uiNums/10;
		} while (0!=uiNums);

		CRect rect;
		CWnd *pMain = AfxGetMainWnd();
		if (NULL != pMain)
		{
			pMain->GetWindowRect(&rect);
		}
		m_pClientPos.x = (rect.Width()-m_BakWidth)/2;
		m_pClientPos.y = (rect.Height()-m_BakHeight)/2;
		SetWindowPos(NULL, rect.left+m_pClientPos.x, rect.top+m_pClientPos.y, 0, 0, SWP_NOSIZE);
		m_iCurFrame = 0;
		
		m_iTransparent = 100;
		UpdateClockDisplay(m_iTransparent);

		ShowWindow(SW_SHOW);
		SetTimer(IFRAMEFLASH, IFRAMETIME, NULL);
	}
	else
	{
		KillTimer(IFRAMEFLASH);
		KillTimer(IALPHAFLASH);

		ShowWindow(SW_HIDE);
		m_iTransparent = 100;
	}

	__LEAVE_FUNCTION
}

void CTaskFinishFlash::OnTimer(UINT nIDEvent)
{
	__ENTER_FUNCTION

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
				if (m_iCurFrame < INUMS-1)
				{
					++m_iCurFrame;
					m_bIsShowText = false;
				}
				else
				{
					m_iCurFrame = INUMS-1;
					KillTimer(IFRAMEFLASH);
					SetTimer(IALPHAFLASH, IALPHATIME, NULL);
					m_bIsShowText = true;
				}
				m_iTransparent = 100;
				UpdateClockDisplay(m_iTransparent);

				SetWindowPos(NULL, rect.left+m_pClientPos.x, rect.top+m_pClientPos.y, 0, 0, SWP_NOSIZE|SWP_SHOWWINDOW);
			}
			break;
		case IALPHAFLASH:	//ALPHA动画
			{
				if (m_iTransparent>0)
				{
					m_iTransparent -= 10;
					UpdateClockDisplay(m_iTransparent);

					SetWindowPos(NULL, rect.left+m_pClientPos.x, rect.top+m_pClientPos.y, 0, 0, SWP_NOSIZE|SWP_SHOWWINDOW);
				}
				else
				{
					m_iTransparent = 0;
					KillTimer(IFRAMEFLASH);
					KillTimer(IALPHAFLASH);
					ShowWindow(SW_HIDE);
				}
			}
			break;
		default:
			break;
		}
	}
	
	__LEAVE_FUNCTION

	CDialog::OnTimer(nIDEvent);
}