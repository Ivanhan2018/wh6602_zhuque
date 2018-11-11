#include "StdAfx.h"
#include "GameClient.h"
#include "GameClientView.h"
#include "GameClientDlg.h"
#include ".\gameclientview.h"

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CGameClientView, CGameFrameView)
	ON_WM_CREATE()
END_MESSAGE_MAP()

//构造函数
CGameClientView::CGameClientView()
{
	m_bInitGameTag=false;

	return;
}

//析构函数
CGameClientView::~CGameClientView()
{
}

//重置界面
void CGameClientView::ResetGameView()
{
	return ;
}

//调整控件
VOID CGameClientView::RectifyGameView(INT nWidth, INT nHeight)
{
		//调整大小
	if(m_GameClientViewDX.GetSafeHwnd()!=NULL)
	{
		m_GameClientViewDX.MoveWindow(0,0,nWidth,nHeight);
		m_GameClientViewDX.OnDeviceLost();
		if(!m_GameClientViewDX.UpdateDxWindowSize())
		{
			//AfxMessageBox("设备创建失败，程序即将退出。");
			AfxGetMainWnd()->SendMessage(WM_CLOSE);
			return; 
		}
		m_GameClientViewDX.InitScene(nWidth,nHeight);
	}

	//创建dx窗口
	if(false==m_bInitGameTag && m_GameClientViewDX.GetSafeHwnd()==NULL)
	{
		m_GameClientViewDX.Create(AfxRegisterWndClass(0,NULL,NULL,NULL),NULL,WS_CHILD|WS_VISIBLE|WS_CLIPCHILDREN|WS_CLIPSIBLINGS,CRect(0,0,nWidth,nHeight),this,0);
		//m_GameClientViewDX.Create(IDD_DX_BASE,this);
		//SetWindowLong(m_GameClientViewDX.GetSafeHwnd(),GWL_STYLE,GetStyle()|WS_CLIPCHILDREN);
		if(!m_GameClientViewDX.InitDxWindow())
		{
			AfxMessageBox("设备创建失败，程序即将退出。");
			AfxGetMainWnd()->SendMessage(WM_CLOSE);
			return;  
		}
		else m_GameClientViewDX.InitScene(nWidth,nHeight);

		m_GameClientViewDX.MoveWindow(0,0,nWidth,nHeight);
		m_GameClientViewDX.ShowWindow(SW_SHOW);
		m_GameClientViewDX.RectifyGameView(nWidth,nHeight);
	}

	if(!m_bInitGameTag)m_bInitGameTag=true;

	return;
}

//绘画界面
VOID CGameClientView::DrawGameView(CDC * pDC, INT nWidth, INT nHeight)
{
	return;
}

//创建函数
INT CGameClientView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (__super::OnCreate(lpCreateStruct)==-1) return -1;

	//变量定义
	CRect rcCreate(0,0,0,0);
	HINSTANCE hResInstance=AfxGetInstanceHandle();

	//设置窗体
	CGameClientDlg *pGameClientDlg=CONTAINING_RECORD(this,CGameClientDlg,m_GameClientView);
	if (NULL!=pGameClientDlg) m_GameClientViewDX.SetGameClientDlg(pGameClientDlg);

	return 0;
}



////消息截取
//BOOL CGameClientView::PreTranslateMessage(MSG * pMsg)
//{
//	//判断回车
//	if(pMsg->message==WM_KEYDOWN)
//	{
//		BYTE bMesValue = (BYTE)(pMsg->wParam);
//		if(bMesValue==VK_SPACE || bMesValue==VK_DOWN || bMesValue==VK_RIGHT || bMesValue==VK_UP || bMesValue==VK_LEFT) 
//		{
//			return TRUE;
//		}
//	}
//	return __super::PreTranslateMessage(pMsg);
//}

//////////////////////////////////////////////////////////////////////////
