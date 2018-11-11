// ActionWnd.cpp : 实现文件
//

#include "stdafx.h"
#include "ActionWnd.h"
#include "MemDC.h"
#include "TraceCatch.h"

// ActionWnd 对话框

IMPLEMENT_DYNAMIC(ActionWnd, CDialog)
ActionWnd::ActionWnd(CWnd* pParent /*=NULL*/)
	: CDialog(ActionWnd::IDD, pParent)
{
	__ENTER_FUNCTION 

	//m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_strImage = "";

	m_iCountCur = 0;
	m_iTotalFrames = 0;

	m_bIsAutoHide = true;
	__LEAVE_FUNCTION
}

ActionWnd::~ActionWnd()
{
	__ENTER_FUNCTION 
	
	m_ImageBack.DestroyImage();
	__LEAVE_FUNCTION
}

void ActionWnd::DoDataExchange(CDataExchange* pDX)
{
	__ENTER_FUNCTION 

	CDialog::DoDataExchange(pDX);
	__LEAVE_FUNCTION
}


BEGIN_MESSAGE_MAP(ActionWnd, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_WM_TIMER()
END_MESSAGE_MAP()


// ActionWnd 消息处理程序

void ActionWnd::SetActionImage(CString strImage,bool bIsMove,int TotallFrames, bool bIsAutoHide)
{
	__ENTER_FUNCTION 

	bIsMove = true;
	m_strImage = strImage;

	m_bIsAutoHide = bIsAutoHide;
	//初始化gdiplus的环境
	// Initialize GDI+.
	m_Blend.BlendOp=0; 
	m_Blend.BlendFlags=0; 
	m_Blend.AlphaFormat=1; 
	m_Blend.SourceConstantAlpha=255;

	m_iTransparent = 100;
	m_bIsUpTag = true;

	m_iCountCur = 0;
	m_iTotalFrames = TotallFrames;

	m_BakWidth = 0;
	m_BakHeight = 0;

	if (!m_strImage.IsEmpty())
	{
		//加载资源
		m_ImageBack.DestroyImage();
		m_ImageBack.LoadImage(m_strImage);
		if (!m_ImageBack.IsNull())
		{
			m_BakWidth = m_ImageBack.GetWidth()/m_iTotalFrames;
			m_BakHeight = m_ImageBack.GetHeight();
		}
		if(0==m_BakWidth && m_BakWidth==m_BakHeight)
		{
			return;
		}
		SetWindowPos(NULL, 0, 0, m_BakWidth, m_BakHeight, SWP_NOMOVE);

		// TODO: 在此添加额外的初始化代码
		//启动后立刻更新窗口样式为透明窗体
		if (bIsMove)
		{
			if (TotallFrames>10)
			{
				UpdateClockDisplay(m_iTransparent);
				SetTimer(1,70,NULL);
			}
			else
			{
				UpdateClockDisplay(m_iTransparent);
				SetTimer(1, 150, NULL);
			}			
		}
		else
		{
			m_iTransparent=100;
			UpdateClockDisplay(m_iTransparent);
		}
	}
	__LEAVE_FUNCTION
}

BOOL ActionWnd::OnInitDialog()
{
	__ENTER_FUNCTION 

	CDialog::OnInitDialog();

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	hFuncInst = LoadLibrary("User32.DLL"); 
	BOOL bRet=FALSE;
	if(hFuncInst) 
		UpdateLayeredWindow=(MYFUNC)GetProcAddress(hFuncInst, "UpdateLayeredWindow");
	else
	{
		AfxMessageBox("User32.dll ERROR!");
		exit(0);
	}
	__LEAVE_FUNCTION
	return TRUE;  // 除非设置了控件的焦点，否则返回 TRUE	
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void ActionWnd::OnPaint() 
{
	__ENTER_FUNCTION 
	
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
	__LEAVE_FUNCTION
}

//当用户拖动最小化窗口时系统调用此函数取得光标显示。
HCURSOR ActionWnd::OnQueryDragIcon()
{
	__ENTER_FUNCTION 
	__LEAVE_FUNCTION
	return static_cast<HCURSOR>(m_hIcon);
}

void ActionWnd::OnTimer(UINT nIDEvent)
{
	__ENTER_FUNCTION 

	// TODO: 在此添加消息处理程序代码和/或调用默认值
	++m_iCountCur;
	if (m_iCountCur>=m_iTotalFrames)
	{
		m_iCountCur = m_iTotalFrames-1;
		KillTimer(1);

		if (m_bIsAutoHide)
		{
			ShowWindow(SW_HIDE);
		}
	}
	UpdateClockDisplay(m_iTransparent);
	CDialog::OnTimer(nIDEvent);
	__LEAVE_FUNCTION
}

#if 0
BOOL ActionWnd::UpdateClockDisplay(int Transparent)
{
	__ENTER_FUNCTION 

	CClientDC dc(this);
	CRect  rect;
	rect.SetRect(0,0,m_BakWidth,m_BakHeight);
	CMemDC dcMem(&dc,rect,true);

	if(Transparent<0||Transparent>100)	Transparent=0;

	m_Blend.SourceConstantAlpha=int(Transparent*2.55);//1~255
	HDC hdcScreen=dc.GetSafeHdc();
	RECT rct;
	GetWindowRect(&rct);
	POINT ptWinPos={rct.left,rct.top};

	m_ImageBack.DrawImage(&dcMem, 0, 0, m_BakWidth,m_BakHeight, m_iCountCur*m_BakWidth, 0, m_BakWidth,m_BakHeight);
	
	SIZE sizeWindow={m_BakWidth,m_BakHeight};
	POINT ptSrc={0,0};
	DWORD dwExStyle=GetWindowLong(m_hWnd,GWL_EXSTYLE);
	if((dwExStyle&0x80000)!=0x80000)
		SetWindowLong(m_hWnd,GWL_EXSTYLE,dwExStyle^0x80000);

	BOOL bRet=FALSE;
	bRet= UpdateLayeredWindow( m_hWnd,hdcScreen,&ptWinPos,&sizeWindow,dcMem.m_hDC,&ptSrc,0,&m_Blend,2);
	hdcScreen=NULL;

	__LEAVE_FUNCTION
	return 0;
}
#else
BOOL ActionWnd::UpdateClockDisplay(int Transparent)
{
	__ENTER_FUNCTION 

	CDC m_hdcMemory;	// add by HouGuoJiang 2011-11-25
	// 取代类成员

	CDC* pWndDC = GetDC();
	m_hdcMemory.CreateCompatibleDC(pWndDC);
	HBITMAP hBitMap=::CreateCompatibleBitmap(pWndDC->m_hDC,m_BakWidth,m_BakHeight);
	SelectObject(m_hdcMemory,hBitMap);

	if(Transparent<0||Transparent>100)	Transparent=0;

	m_Blend.SourceConstantAlpha=int(Transparent*2.55);//1~255
	HDC hdcScreen=pWndDC->GetSafeHdc();
	RECT rct;
	GetWindowRect(&rct);
	POINT ptWinPos={rct.left,rct.top};

	m_ImageBack.DrawImage(&m_hdcMemory, 0, 0, m_BakWidth,m_BakHeight, m_iCountCur*m_BakWidth, 0, m_BakWidth,m_BakHeight);

	SIZE sizeWindow={m_BakWidth,m_BakHeight};
	POINT ptSrc={0,0};
	DWORD dwExStyle=GetWindowLong(m_hWnd,GWL_EXSTYLE);
	if((dwExStyle&0x80000)!=0x80000)
		SetWindowLong(m_hWnd,GWL_EXSTYLE,dwExStyle^0x80000);

	BOOL bRet=FALSE;
	bRet= UpdateLayeredWindow( m_hWnd,hdcScreen,&ptWinPos,&sizeWindow,m_hdcMemory.m_hDC,&ptSrc,0,&m_Blend,2);
	hdcScreen=NULL;

	::DeleteObject(hBitMap);
	::DeleteDC(m_hdcMemory.GetSafeHdc());

	ReleaseDC(pWndDC);	// add by HouGuoJiang 2011-11-25
	__LEAVE_FUNCTION
	return 0;
}

#endif

