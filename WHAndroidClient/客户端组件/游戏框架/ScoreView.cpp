#include "StdAfx.h"
#include "ScoreView.h"
#include "Resource.h"
#include ".\scoreview.h"
#include "GameFrameDlg.h"


//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CScoreView, CDialog)
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_LBUTTONDOWN()
	ON_WM_WINDOWPOSCHANGING()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CScoreView::CScoreView() : CDialog()
{
	m_szWndSize.SetSize(0, 0);
	m_ImageBuffer.Destroy();
	m_ImageBuffer.Create(100, 100, 24);
	m_pGameFrameDlg = NULL;
	m_bIsMoveWnd = true;
}
CScoreView::CScoreView(UINT nIDTemplate, CWnd* pParentWnd ): CDialog(IDD_SCORE_VIEW_DLG,pParentWnd)
{
	m_szWndSize.SetSize(0, 0);
	m_ImageBuffer.Destroy();
	m_ImageBuffer.Create(100, 100, 24);
	m_pGameFrameDlg = NULL;
	m_bIsMoveWnd = true;
}

//析构函数
CScoreView::~CScoreView()
{
	m_ImageBuffer.Destroy();
	m_pGameFrameDlg = NULL;
}

//设置游戏框架指针
void CScoreView::SetGameFramePtr(CGameFrameDlg* pGameFrameDlg)
{
	if (NULL != pGameFrameDlg)
	{
		m_pGameFrameDlg = pGameFrameDlg;
	}
}

//设置可否移动窗口
void CScoreView::SetDisableMoveWnd(bool bIsMove)
{
	m_bIsMoveWnd = bIsMove;
}

//建立消息
int CScoreView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (__super::OnCreate(lpCreateStruct)==-1) return -1;

	m_bIsMoveWnd = true;

	//初始化数据
	InitScoreViewInfo();

	
	if (!m_ImagePngBack.IsNull())
	{
		m_ImageBuffer.Destroy();
		m_ImageBuffer.Create(m_ImagePngBack.GetWidth(), m_ImagePngBack.GetHeight(), 24);
		m_szWndSize.SetSize(m_ImagePngBack.GetWidth(), m_ImagePngBack.GetHeight());
		SetImageRgn(m_hWnd, &m_ImagePngBack, SIRT_DEBURRING_DOT_LESS);
		return 0;
	}
	return -1;
}

//加载背景图
bool CScoreView::LoadBackImage(const char* pImagePath)
{
	if (NULL != pImagePath)
	{
		m_ImagePngBack.DestroyImage();
		//加载资源
		m_ImagePngBack.LoadImage(pImagePath);
		m_szWndSize.SetSize(m_ImagePngBack.GetWidth(), m_ImagePngBack.GetHeight());
		return true;
	}
	return false;
}

//鼠标消息
void CScoreView::OnLButtonDown(UINT nFlags, CPoint Point)
{
	__super::OnLButtonDown(nFlags,Point);

	if (true == m_bIsMoveWnd)
	{
		//消息模拟
		PostMessage(WM_NCLBUTTONDOWN,HTCAPTION,MAKELPARAM(Point.x,Point.y));
	}
	return;
}

//重画函数
void CScoreView::OnPaint() 
{
	if (!m_ImageBuffer.IsNull() && !m_ImagePngBack.IsNull())
	{
		//变量定义
		CPaintDC dc(this);

		CDC * pDC=CDC::FromHandle(m_ImageBuffer.GetDC());

		pDC->SetBkMode(TRANSPARENT);
		pDC->FillSolidRect(0,0,m_szWndSize.cx,m_szWndSize.cy,RGB(0,0,0));

		//绘画背景图片
		m_ImagePngBack.DrawImage(pDC, 0, 0);

		DrawScoreInfo(pDC);

		//绘画界面
		m_ImageBuffer.BitBlt(dc,0,0,m_szWndSize.cx,m_szWndSize.cy,0,0);
		m_ImageBuffer.ReleaseDC();

		CDC::DeleteTempMap();	// add by HouGuoJiang 2011-11-25
	}
}

//////////////////////////////////////////////////////////////////////////


void CScoreView::OnWindowPosChanging(WINDOWPOS* lpwndpos)
{
	CRect parrect;
	CWnd * pWnd= AfxGetMainWnd();
	if (NULL == pWnd)
	{
		return;
	}
	pWnd->GetWindowRect(&parrect);

	if (lpwndpos->y+m_szWndSize.cy > parrect.top+parrect.Height()-70)
	{//下边
		lpwndpos->y = parrect.top+parrect.Height()-70-m_szWndSize.cy;
	}
	if (lpwndpos->x+m_szWndSize.cx > parrect.left+parrect.Width())
	{//右边
		lpwndpos->x = parrect.left+parrect.Width()-m_szWndSize.cx;
	}
	if (lpwndpos->y < parrect.top+29)
	{//上
		lpwndpos->y = parrect.top+29;
	}
	if (lpwndpos->x < parrect.left)
	{//下
		lpwndpos->x = parrect.left;
	}
	CDialog::OnWindowPosChanging(lpwndpos);
	// TODO: 在此处添加消息处理程序代码
}


//返回大厅
void CScoreView::OnGoPlaza()
{
	if (NULL != m_pGameFrameDlg)
	{
		m_pGameFrameDlg->SetGameExitOperation(1);
	}
}

//继续游戏
void CScoreView::OnStartGame()
{
	if (NULL != m_pGameFrameDlg)
	{
		m_pGameFrameDlg->SetGameExitOperation(2);
	}
}

//返回房间
void CScoreView::OnGoRoom()
{
	if (NULL != m_pGameFrameDlg)
	{
		m_pGameFrameDlg->SetGameExitOperation(3);
	}
}
//退出游戏
void CScoreView::OnGoExit()
{
	if (NULL != m_pGameFrameDlg)
	{
		m_pGameFrameDlg->SetGameExitOperation(4);
	}
}

BOOL CScoreView::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	return TRUE;
	return CDialog::OnEraseBkgnd(pDC);
}

void CScoreView::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	CDialog::OnLButtonUp(nFlags, point);
}
//
//BOOL CScoreView::SetImageRgn(CPngImage * pImage)
//{
//	if (NULL==pImage) return FALSE;
//	if (pImage->IsNull()) return FALSE;
//	long wWidth = pImage->GetWidth();
//	long wHeight = pImage->GetHeight();
//
//	CRgn rgnImage, tempRgn;
//	rgnImage.CreateRectRgn(0, 0, 0, 0);
//
//	int leftX = 0;
//	int rightX = 0;
//	for (int i=wHeight-1; i>=0; --i)
//	{
//		for (int j = 0; j<wWidth; ++j)
//		{
//			leftX = j;
//			rightX = j;
//			for (int k=j; k<wWidth; ++k)  //记住左边的x
//			{
//				BYTE byValue = pImage->GetAlphaValue(k, i);
//				if (byValue != 0)
//				{
//					leftX = k;
//					break;
//				}
//			}
//			for (int k=leftX+1; k<wWidth; ++k)
//			{
//				BYTE byValue = pImage->GetAlphaValue(k, i);
//				if (byValue == 0 || (k == wWidth -1 && byValue!=0))
//				{
//					rightX = k;
//					break;
//				}
//			}
//
//			j = leftX > rightX ? leftX : rightX;
//
//			if (rightX > leftX)
//			{
//				tempRgn.CreateRectRgn(leftX, (wHeight - i-1) , rightX, (wHeight -i));
//				rgnImage.CombineRgn(&rgnImage, &tempRgn, RGN_OR);
//				tempRgn.DeleteObject();
//			}
//		}
//	}
//
//	if (NULL == rgnImage.m_hObject)
//		return FALSE;
//
//	SetWindowRgn((HRGN)rgnImage.m_hObject, TRUE);
//	rgnImage.DeleteObject();
//	return TRUE;
//}