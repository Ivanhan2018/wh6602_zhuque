// DlgLogonLogo.cpp : 实现文件
//

#include "stdafx.h"
#include "DlgLogonLogo.h"
#include ".\dlglogonlogo.h"
#include "DlgLogon.h"

// CDlgLogonLogo 对话框

IMPLEMENT_DYNAMIC(CDlgLogonLogo, CDialog)
CDlgLogonLogo::CDlgLogonLogo(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgLogonLogo::IDD, pParent), m_pImageBack(NULL)
{
}

CDlgLogonLogo::~CDlgLogonLogo()
{
	if(NULL != m_pImageBack)
	{
		delete m_pImageBack;
	}
	m_pImageBack = NULL;
}

void CDlgLogonLogo::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgLogonLogo, CDialog)
	ON_WM_CREATE()
	ON_WM_LBUTTONDOWN()
	ON_WM_WINDOWPOSCHANGING()
	ON_WM_MOVE()
	ON_WM_PAINT()
END_MESSAGE_MAP()


// CDlgLogonLogo 消息处理程序

int CDlgLogonLogo::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	//设置标题
	SetWindowText("藏宝湾—"); 

	hFuncInst = LoadLibrary("User32.DLL"); 
	BOOL bRet=FALSE;
	if(hFuncInst)
	{
		UpdateLayeredWindow=(MYFUNC)GetProcAddress(hFuncInst, "UpdateLayeredWindow");
	}
	else
	{
		AfxMessageBox("User32.dll ERROR!");
		exit(0);
	}

	m_Blend.BlendOp=0;
	m_Blend.BlendFlags=0; 
	m_Blend.AlphaFormat=1; 
	m_Blend.SourceConstantAlpha=255;

	if(NULL != m_pImageBack)
	{
		delete m_pImageBack;
	}
	m_pImageBack = NULL;
	m_pImageBack = new Gdiplus::Image(L"data\\logon\\BKLogo.png");
	if (NULL != m_pImageBack)
	{
		m_BakWidth  =m_pImageBack->GetWidth();
		m_BakHeight =m_pImageBack->GetHeight();
	}

	DrawLogo();
	return 0;
}

BOOL CDlgLogonLogo::DrawLogo(int Transparent)
{
	HDC hdcTemp=GetDC()->m_hDC;
	
	m_hdcMemory=CreateCompatibleDC(hdcTemp);
	HBITMAP hBitMap=CreateCompatibleBitmap(hdcTemp,m_BakWidth,m_BakHeight);
	SelectObject(m_hdcMemory,hBitMap);
	if(Transparent<0||Transparent>100)	Transparent=100;

	m_Blend.SourceConstantAlpha=int(Transparent*2.55);//1~255
	HDC hdcScreen=::GetDC (m_hWnd);
	RECT rct;
	GetWindowRect(&rct);
	POINT ptWinPos={rct.left,rct.top};
	Graphics graph(m_hdcMemory);

	Point points[] = { Point(0, 0), 
		Point(m_BakWidth, 0), 
		Point(0, m_BakHeight)
	};
	graph.DrawImage(m_pImageBack, points, 3); 

	SIZE sizeWindow={m_BakWidth,m_BakHeight};
	POINT ptSrc={0,0};
	DWORD dwExStyle=GetWindowLong(m_hWnd,GWL_EXSTYLE);
	if((dwExStyle&0x80000)!=0x80000)
		SetWindowLong(m_hWnd,GWL_EXSTYLE,dwExStyle^0x80000);

	BOOL bRet=FALSE;
	bRet= UpdateLayeredWindow( m_hWnd,hdcScreen,&ptWinPos,&sizeWindow,m_hdcMemory,&ptSrc,0,&m_Blend,2);
	graph.ReleaseHDC(m_hdcMemory);
	::ReleaseDC(m_hWnd,hdcScreen);
	hdcScreen=NULL;
	::ReleaseDC(m_hWnd,hdcTemp);
	hdcTemp=NULL;
	DeleteObject(hBitMap);
	DeleteDC(m_hdcMemory);
	m_hdcMemory=NULL;
	return bRet;
}

void CDlgLogonLogo::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	CDialog::OnLButtonDown(nFlags, point);

	CWnd* pParent = GetParent();
	if (pParent)
	{
		((CDlgLogon*)pParent)->OnLButtonDown(nFlags, point);
	}
	
}

void CDlgLogonLogo::OnWindowPosChanging(WINDOWPOS* lpwndpos)
{
	CDialog::OnWindowPosChanging(lpwndpos);
	// TODO: 在此处添加消息处理程序代码
}

void CDlgLogonLogo::OnMove(int x, int y)
{
	CDialog::OnMove(x, y);
}


void CDlgLogonLogo::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 在此处添加消息处理程序代码
	// 不为绘图消息调用 CDialog::OnPaint()
}
