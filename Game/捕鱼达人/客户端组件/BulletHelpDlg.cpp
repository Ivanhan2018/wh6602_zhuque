#include "StdAfx.h"
#include "BulletHelpDlg.h"
#include "resource.h"

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CBulletHelpDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CBulletHelpDlg::CBulletHelpDlg(CWnd	*pViewWnd):CDialog(IDD_BULLET_HELP),m_pViewWnd(pViewWnd)
{
	//加载图片
	HINSTANCE hInstance=AfxGetInstanceHandle();
	m_PngBack.LoadImage(hInstance,TEXT("BULLET_HELP"));

	return;
}

//析构函数
CBulletHelpDlg::~CBulletHelpDlg()
{
}

//控件绑定
void CBulletHelpDlg::DoDataExchange(CDataExchange* pDX)
{
	DDX_Control(pDX, IDC_CLOSE, m_btClose);
	__super::DoDataExchange(pDX);
}

//重画函数
void CBulletHelpDlg::OnPaint() 
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
	m_PngBack.DrawImage(&dcMemory,0,0);

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
BOOL CBulletHelpDlg::OnInitDialog()
{
	__super::OnInitDialog();

	//设置大小
	MoveWindow(0,0,m_PngBack.GetWidth(),m_PngBack.GetHeight());

	//设置区域
	HRGN hRGN=CreateRoundRectRgn(0,0,m_PngBack.GetWidth(),m_PngBack.GetHeight(),10,10);
	if (NULL!=hRGN) SetWindowRgn(hRGN,TRUE);

	//按钮图片
	HINSTANCE hInstance=AfxGetInstanceHandle();
	m_btClose.SetButtonImage(IDB_BT_ISEE,hInstance,3,false);

	//设置位置
	CRect rcClient,rcButton;
	GetClientRect(rcClient);
	m_btClose.GetWindowRect(rcButton);
	m_btClose.MoveWindow((rcClient.Width()-rcButton.Width())/2,rcClient.Height()-rcButton.Height()-5,rcButton.Width(),rcButton.Height());

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
BOOL CBulletHelpDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
	//变量定义
	UINT nCommandID=LOWORD(wParam);

	//消息处理
	switch(nCommandID)
	{
	case IDC_CLOSE:
		{
			__super::OnOK();

			//发送消息
			//AfxGetMainWnd()->PostMessage(WM_SHOW_BULLET_SHOP,0,0);

			return TRUE;
		}
	}
	return __super::OnCommand(wParam,lParam);
}


//鼠标消息
void CBulletHelpDlg::OnLButtonDown(UINT nFlags, CPoint Point)
{
	__super::OnLButtonDown(nFlags,Point);

	//消息模拟
	PostMessage(WM_NCLBUTTONDOWN,HTCAPTION,MAKELPARAM(Point.x,Point.y));	

	return;
}
//////////////////////////////////////////////////////////////////////////


