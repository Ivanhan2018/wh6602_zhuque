#include "StdAfx.h"
#include "GameHelpDlg.h"
#include "resource.h"

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CGameHelpDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CGameHelpDlg::CGameHelpDlg(CWnd	*pViewWnd):CDialog(IDD_GAME_HELP),m_pViewWnd(pViewWnd)
{
	//设置变量
	m_HelpKind=enHelpKind_Game;

	//加载图片
	HINSTANCE hInstance=AfxGetInstanceHandle();
	m_PngGameHelp.LoadImage(hInstance,TEXT("GAME_HELP"));
	m_PngBulletHelp.LoadImage(hInstance,TEXT("BULLET_EFFECT"));
	m_PngFishScore.LoadImage(hInstance,TEXT("FISH_SCORE"));
	m_PngCannoHelp.LoadImage(hInstance,TEXT("CANNO_HELP"));

	return;
}

//析构函数
CGameHelpDlg::~CGameHelpDlg()
{
}

//控件绑定
void CGameHelpDlg::DoDataExchange(CDataExchange* pDX)
{
	DDX_Control(pDX, IDC_CLOSE, m_btClose);
	DDX_Control(pDX, IDC_GAME_HELP, m_btGameHelp);
	DDX_Control(pDX, IDC_BULLET_HELP, m_btBulletHelp);
	DDX_Control(pDX, IDC_CANNO_HELP, m_btCannoHelp);
	DDX_Control(pDX, IDC_FISH_SCORE, m_btFishScore);
	__super::DoDataExchange(pDX);
}

//重画函数
void CGameHelpDlg::OnPaint() 
{
	//绘画DC
	CPaintDC dc(this);

	//缓冲资源
	CDC dcMemory;
	CBitmap bmpMemory;

	//获取大小
	CRect rcClient;
	GetClientRect(rcClient);

	//创建资源
	dcMemory.CreateCompatibleDC(&dc);
	bmpMemory.CreateCompatibleBitmap(&dc,rcClient.Width(),rcClient.Height());
	CBitmap *pOldBmp=dcMemory.SelectObject(&bmpMemory);

	//绘画背景
	dcMemory.FillSolidRect(rcClient,RGB(0,0,0));
	m_PngGameHelp.DrawImage(&dcMemory,0,0);
	int nExcursionX=17,nExcursionY=42;
	if (enHelpKind_Bullet==m_HelpKind)
		m_PngBulletHelp.DrawImage(&dcMemory,nExcursionX,nExcursionY);
	else if (enHelpKind_Canno==m_HelpKind)
		m_PngCannoHelp.DrawImage(&dcMemory,nExcursionX,nExcursionY);
	else if (enHelpKind_FishScore==m_HelpKind)
		m_PngFishScore.DrawImage(&dcMemory,nExcursionX,nExcursionY);
	else if (enHelpKind_Game==m_HelpKind)
		m_PngGameHelp.DrawImage(&dcMemory,0,0);

	////设置字体
	//CFont DrawFont;
	//DrawFont.CreateFont(-12,0,0,0,FW_BOLD,0,0,0,134,3,2,ANTIALIASED_QUALITY,2,TEXT("宋体"));
	//CFont * pOldFont=dcMemory.SelectObject(&DrawFont);
	//dcMemory.SetTextColor(RGB(225,227,161));
	//dcMemory.SetBkMode(TRANSPARENT);

	//绘画界面
	CRect rcClip;
	dc.GetClipBox(&rcClip);
	dc.BitBlt(rcClip.left,rcClip.top,rcClip.Width(),rcClip.Height(),&dcMemory,rcClip.left,rcClip.top,SRCCOPY);

	//清除资源
	/*dcMemory.SelectObject(pOldFont);
	DrawFont.DeleteObject();*/
	dcMemory.SelectObject(pOldBmp);
	bmpMemory.DeleteObject();
	dcMemory.DeleteDC();

	return;
}

//初始函数
BOOL CGameHelpDlg::OnInitDialog()
{
	__super::OnInitDialog();

	//设置大小
	MoveWindow(0,0,m_PngGameHelp.GetWidth(),m_PngGameHelp.GetHeight());

	//设置区域
	HRGN hRGN=CreateRoundRectRgn(0,0,m_PngGameHelp.GetWidth(),m_PngGameHelp.GetHeight(),10,10);
	if (NULL!=hRGN) SetWindowRgn(hRGN,TRUE);

	//按钮图片
	HINSTANCE hInstance=AfxGetInstanceHandle();
	m_btClose.SetButtonImage(TEXT("BT_HELP_CLOSE"),3,false);
	m_btGameHelp.SetButtonImage(TEXT("BT_GAME_HELP"),3,false);
	m_btBulletHelp.SetButtonImage(TEXT("BT_BULLET_EFFECT"),3,false);
	m_btFishScore.SetButtonImage(TEXT("BT_FISH_SCORE"),3,false);
	m_btCannoHelp.SetButtonImage(TEXT("BT_CANNO_HELP"),3,false);

	//设置位置
	CRect rcClient,rcButton;
	GetClientRect(rcClient);
	m_btClose.GetWindowRect(rcButton);
	m_btClose.MoveWindow(rcClient.Width()-rcButton.Width(),0,rcButton.Width(),rcButton.Height());

	//底部按钮
	m_btGameHelp.GetWindowRect(rcButton);
	CButton *pButtons[]={&m_btGameHelp,&m_btBulletHelp,&m_btCannoHelp,&m_btFishScore};
	for (int i=0; i<CountArray(pButtons); ++i)
		pButtons[i]->MoveWindow(10+i*2+i*rcButton.Width(),rcClient.Height()-rcButton.Height()-5,rcButton.Width(),rcButton.Height());

	//透明窗体
	SetWindowLong(this->GetSafeHwnd(),GWL_EXSTYLE, GetWindowLong(this->GetSafeHwnd(),GWL_EXSTYLE) | 0x80000); 
	HINSTANCE hInst=LoadLibrary("User32.DLL"); 
	if(hInst) 
	{ 
		typedef BOOL     (WINAPI * MYFUNC)(HWND,COLORREF,BYTE,DWORD); 
		MYFUNC fun=NULL; 

		//取得SetLayeredWindowAttributes函数指针 
		fun=(MYFUNC)GetProcAddress(hInst,"SetLayeredWindowAttributes"); 
		if(fun) fun(this->GetSafeHwnd(),RGB(39,75,113),240,2);
		FreeLibrary(hInst); 
	}

	//居中窗体
	CenterWindow(m_pViewWnd);
	
	return TRUE;
}

//命令函数
BOOL CGameHelpDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
	//变量定义
	UINT nCommandID=LOWORD(wParam);

	//消息处理
	switch(nCommandID)
	{
	case IDC_CLOSE:
		{
			__super::OnOK();

			return TRUE;
		}
	case IDC_GAME_HELP:
		{
			m_HelpKind=enHelpKind_Game;
			InvalidateRect(NULL,FALSE);

			return TRUE;
		}
	case IDC_BULLET_HELP:
		{
			m_HelpKind=enHelpKind_Bullet;
			InvalidateRect(NULL,FALSE);

			return TRUE;
		}
	case IDC_FISH_SCORE:
		{
			m_HelpKind=enHelpKind_FishScore;
			InvalidateRect(NULL,FALSE);

			return TRUE;
		}
	case IDC_CANNO_HELP:
		{
			m_HelpKind=enHelpKind_Canno;
			InvalidateRect(NULL,FALSE);

			return TRUE;
		}
	}
	return __super::OnCommand(wParam,lParam);
}

//鼠标消息
void CGameHelpDlg::OnLButtonDown(UINT nFlags, CPoint Point)
{
	__super::OnLButtonDown(nFlags,Point);

	//消息模拟
	PostMessage(WM_NCLBUTTONDOWN,HTCAPTION,MAKELPARAM(Point.x,Point.y));	

	return;
}
//////////////////////////////////////////////////////////////////////////
