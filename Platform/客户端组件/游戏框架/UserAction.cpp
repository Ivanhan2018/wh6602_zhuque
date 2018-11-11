// UserAction.cpp : 实现文件
//

#include "stdafx.h"
#include "UserAction.h"
#include ".\useraction.h"
#include "Resource.h"
#include "GameFrameDlg.h"

#include "MemDC.h"

#define			CREATE_BT_USER_ACTION_INDEX			2200
#define			BT_X								12
#define			BT_Y								10
#define			BT_WIDTH							62
#define			BT_HEIGHT							29
#define			BT_WIDTH_INTERVAL					2
#define			BT_HEIGHT_INTERVAL					2

//点击动作后设置的定时器
#define			IDI_TIME_SEND_USER_ACTION			12350
//不能频繁的发送动作
#define			IDI_TIME_SEND_TIME_INTERVAL			12351


#define			TIME_SEND_USER_ACTION_INTERVAL		8000
#define			TIME_SEND_TIME_INTERVAL_INTERVAL	3000

// CUserAction

IMPLEMENT_DYNAMIC(CUserAction, CWnd)
CUserAction::CUserAction()
{
	EnableAutomation();
	m_bHovering = false;
	m_btCurUserActionTag = -1;
	m_bIsSend = false;
	m_bIsAlreadySend = true;
}

CUserAction::~CUserAction()
{
}

void CUserAction::OnFinalRelease()
{
	// 释放了对自动化对象的最后一个引用后，将调用
	// OnFinalRelease。基类将自动
	// 删除该对象。在调用该基类之前，请添加您的
	// 对象所需的附加清除代码。

	CWnd::OnFinalRelease();
}


BEGIN_MESSAGE_MAP(CUserAction, CWnd)
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_MESSAGE(WM_MOUSELEAVE,OnMouseLeave)
	ON_WM_SIZE()
END_MESSAGE_MAP()


BEGIN_DISPATCH_MAP(CUserAction, CWnd)
END_DISPATCH_MAP()

// 注意: 我们添加 IID_IUserAction 支持
//以支持来自 VBA 的类型安全绑定。此 IID 必须同附加到 .IDL 文件中的
//调度接口的 GUID 匹配。

// {4CD84A6A-40A4-458F-9085-FD461E1FCB90}
static const IID IID_IUserAction =
{ 0x4CD84A6A, 0x40A4, 0x458F, { 0x90, 0x85, 0xFD, 0x46, 0x1E, 0x1F, 0xCB, 0x90 } };

BEGIN_INTERFACE_MAP(CUserAction, CWnd)
	INTERFACE_PART(CUserAction, IID_IUserAction, Dispatch)
END_INTERFACE_MAP()


// CUserAction 消息处理程序


int CUserAction::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码
	// 添加透明扩展样式
	//SetWindowLong(this->GetSafeHwnd(), GWL_EXSTYLE, GetWindowLong(this->GetSafeHwnd(), GWL_EXSTYLE)^0x80000);
	//HINSTANCE hInst = LoadLibrary("User32.DLL");
	//if(hInst)
	//{
	//	typedef BOOL (WINAPI *MYFUNC)(HWND,COLORREF,BYTE,DWORD);
	//	MYFUNC fun = NULL;
	//	//取得SetLayeredWindowAttributes函数指针
	//	fun=(MYFUNC)GetProcAddress(hInst, "SetLayeredWindowAttributes");
	//	if(fun)
	//	{
	//		fun(this->GetSafeHwnd(),100,10,2);
	//	}
	//	FreeLibrary(hInst);
	//}

	// TODO:  在此添加额外的初始化
	HINSTANCE hInstance=GetModuleHandle(GAME_FRAME_DLL_NAME);
	m_ImageBack.SetLoadInfo(IDB_USER_ACTION_BACK_IMAGE, hInstance, false);

	int iCountLine = 0;
	CRect CreateRect(0,0,0,0);
	//创建按钮
	for (int i=0; i<20; i++)
	{
		m_btUserAction[i].Create(TEXT(""),WS_CHILD|WS_VISIBLE,CreateRect,this,CREATE_BT_USER_ACTION_INDEX+i);
		m_btUserAction[i].SetButtonImage(IDB_BT_USER_ACTION0+i, hInstance, false, 4);
		if ((i>0) && (0==i%5))
		{
			++iCountLine;
		}
		int iX = BT_X+(i%5)*(BT_WIDTH+BT_WIDTH_INTERVAL);
		int iY = BT_Y+iCountLine*(BT_HEIGHT+BT_HEIGHT_INTERVAL);
		m_btUserAction[i].SetWindowPos(NULL, iX, iY, BT_WIDTH, BT_HEIGHT, 0);
	}
	m_bIsSend = true;
	m_bIsAlreadySend = true;
	return 0;
}

void CUserAction::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 在此处添加消息处理程序代码
	// 不为绘图消息调用 CWnd::OnPaint()

	CRect rect;
	GetClientRect(&rect);
	CMemDC dcMem(&dc, rect, true);	// 用单色填充背景

	DrawWindow(&dcMem);
}

void CUserAction::OnTimer(UINT nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	switch (nIDEvent)
	{
	case IDI_TIME_SEND_USER_ACTION:
		{//时间到了，取消发送。
			KillTimer(IDI_TIME_SEND_USER_ACTION);
			m_bIsSend = false;
			m_btCurUserActionTag = -1;
			m_bIsAlreadySend = true;
		}
		break;
	case IDI_TIME_SEND_TIME_INTERVAL:
		{
			KillTimer(IDI_TIME_SEND_TIME_INTERVAL);
			m_bIsAlreadySend = true;
		}
		break;
	}
	CWnd::OnTimer(nIDEvent);
}

void CUserAction::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	CWnd::OnLButtonDown(nFlags, point);
}

void CUserAction::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	CWnd::OnLButtonUp(nFlags, point);
}

void CUserAction::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (m_bHovering==false)
	{
		//注册消息
		m_bHovering=true;
		Invalidate(FALSE);
		TRACKMOUSEEVENT TrackMouseEvent;
		TrackMouseEvent.cbSize=sizeof(TrackMouseEvent);
		TrackMouseEvent.dwFlags=TME_LEAVE;
		TrackMouseEvent.hwndTrack=GetSafeHwnd();
		TrackMouseEvent.dwHoverTime=HOVER_DEFAULT;
		_TrackMouseEvent(&TrackMouseEvent);
	}
	CWnd::OnMouseMove(nFlags, point);
}
//鼠标离开消息
LRESULT CUserAction::OnMouseLeave(WPARAM wparam, LPARAM lparam)
{
	//ShowWindow(SW_HIDE);
	m_bHovering=false;
	return 0;
}

//获得当前用户点击的动作标识
BYTE CUserAction::GetCurUserActionTag()
{
	return m_btCurUserActionTag;
}
//是否可以发送
bool CUserAction::GetCurSendStatus()
{
	return m_bIsSend;
}
//设置是否可以发送
void CUserAction::SetCurSendStatus(bool bIsSend)
{
	m_bIsSend = bIsSend;
}
//设置是否可以发送
void CUserAction::SetTimeIntervalSend(bool bIsSend)
{
	m_bIsAlreadySend = bIsSend;
}
//获得超时是否可以发送
bool CUserAction::GetTimeIntervalSendStatus()
{
	return m_bIsAlreadySend;
}
//重置数据
void CUserAction::SetResetData()
{
	m_btCurUserActionTag = -1;
	m_bIsSend = false;
	m_bIsAlreadySend = false;
	KillTimer(IDI_TIME_SEND_USER_ACTION);
	KillTimer(IDI_TIME_SEND_TIME_INTERVAL);
	SetTimer(IDI_TIME_SEND_TIME_INTERVAL, TIME_SEND_TIME_INTERVAL_INTERVAL, NULL);
}



void CUserAction::AddRectRgn(CDC* pDC,CRgn* rgn,CRect* rect,COLORREF col)
{
	for(int y=rect->top;y<rect->bottom;y++)
		for(int x=rect->left;x<rect->right;x++)
			if (pDC->GetPixel(x,y)==col)
			{
				CRgn rgnAdd;
				rgnAdd.CreateRectRgn (x, y, x+1, y+1);
				rgn->CombineRgn(rgn, &rgnAdd, RGN_DIFF);
				DeleteObject(rgnAdd);
			}
}
void CUserAction::UpdateRgn(CDC * pDC)
{
	CImageHandle ImageHandle(&m_ImageBack);
	CRect rcClient;
	GetClientRect(&rcClient);
	CRgn rgn;
	COLORREF col = RGB(255,0,255);  // mask color
	CRect rc(0,0,0,0);
	rgn.CreateRectRgn (0, 0, rcClient.Width(), rcClient.Height());

	//左上
	rc.bottom =  m_ImageBack.GetHeight();
	rc.right = m_ImageBack.GetWidth();
	AddRectRgn(pDC,&rgn,&rc,col);
	SetWindowRgn (rgn,TRUE);
	//SetForegroundWindow();
}

bool CUserAction::DrawWindow(CDC * pDC)
{
	if (NULL != pDC && NULL != pDC->m_hDC)
	{
		CImageHandle ImageHandle(&m_ImageBack);
		pDC->SetBkMode(TRANSPARENT);
		pDC->FillSolidRect(0,0,m_ImageBack.GetWidth(),m_ImageBack.GetHeight(),RGB(255,0,255));

		//绘画背景图片
		CImageHandle HandleCenter(&m_ImageBack);
		m_ImageBack.AlphaDrawImage(pDC, 0, 0, RGB(255, 0, 255));
		return true;
	}
	return false;
}

void CUserAction::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

#if 0  // 删除这一段会有问题吗？
	CClientDC dc(this);
	CRect rect;
	GetClientRect(&rect);
	CMemDC dcMem(&dc, rect, true);	// 用单色填充背景

	if (DrawWindow(&dcMem))
	{
		UpdateRgn(&dcMem);
		CRect rect,rc,rc2;
		GetClientRect(&rect);
		CString sss;
		if( GetUpdateRect(&rc) )
		{
			rc2 = rc;
			if( rc.left < rc.right )
			{
				rc2.top = rect.top;
				rc2.bottom = rect.bottom;
				InvalidateRect(&rc2,0);
			}
			rc2 = rc;
			if( rc.top < rc.bottom )
			{
				rc2.left = rect.left;
				rc2.right = rect.right;
				InvalidateRect(&rc2,0);
			}
		}
		else
			InvalidateRect(0);
	}
#endif
}

BOOL CUserAction::OnCommand(WPARAM wParam, LPARAM lParam)
{
	CWnd::OnCommand(wParam, lParam);
	// TODO: 在此添加专用代码和/或调用基类
	WORD wIndex = LOWORD(wParam)-CREATE_BT_USER_ACTION_INDEX;
	if (wIndex>=0 && wIndex<=19)
	{
		m_bIsSend = true;
		m_btCurUserActionTag = wIndex;
		KillTimer(IDI_TIME_SEND_USER_ACTION);
		ShowWindow(SW_HIDE);

		CWnd* pMainWnd = AfxGetMainWnd();
		switch (m_btCurUserActionTag)
		{//检测有对象的.
		case 0:
		case 2:
		case 5:
		case 6:
		case 9:
		case 11:
		case 15:
		case 19:
			{
				if (NULL != pMainWnd)
				{
					((CGameFrameDlg*)pMainWnd)->InsertSystemString("请选择要发送的对象！");
				}
				SetTimer(IDI_TIME_SEND_USER_ACTION, TIME_SEND_USER_ACTION_INTERVAL, NULL);
				return true;
			}
			break;
		default:
			break;
		}
		
		//没有对象的，可以直接发送。
		
		if (NULL != pMainWnd)
		{
			((CGameFrameDlg*)pMainWnd)->SendUserActionMsg(-1);
		}
	}
	return true;
}
