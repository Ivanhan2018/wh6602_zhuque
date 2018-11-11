// ComboBoxEx.cpp : 实现文件
//
#include "Stdafx.h"
#include "Resource.h"
#include "ComboBoxEx.h"
#include "CEditEx.h"
#include "SkinImage.h"

#define ARROWBTN_NORMAL		0	//正常状态
#define	ARROWBTN_MOUSEIN	1	//鼠标经过
#define ARROWBTN_MOUSEDOWN	2	//鼠标按下
#define ARROWBTN_DISABLE	3	//不可用
#define ARROWBTN_NO			4	//无下拉菜单

#define DRAW_ARROWBTN_OFFSET_WIDTH	0	//下拉按钮的宽偏移值(new 0)-(old 2)     // --ComboTrouble
#define DRAW_ARROWBTN_OFFSET_HEIGHT	0	//下拉按钮的高偏移值(new 1)-(old 2)

#define DefBkColor		RGB(255,255,255)
#define DefTextColor	RGB(0,0,0)
#define DisableBKColor	RGB(212, 208, 200)

class CSkinComBoboxRes{
public:
	CSkinComBoboxRes(){
	//AFX_MANAGE_STATE(AfxGetStaticModuleState());
	m_bFirstIn		= FALSE;
	m_ArrowBtnType	= ARROWBTN_NO;
	m_BKColor		= DefBkColor;
	m_EditBKColor	= DefBkColor;
	m_TextColor		= DefTextColor;
	m_iBTabsX		= DRAW_ARROWBTN_OFFSET_WIDTH;
	m_iBTabsY		= DRAW_ARROWBTN_OFFSET_HEIGHT;
	m_Brush.CreateSolidBrush(m_BKColor);
//	m_ArrowBK.LoadImage(IDB_ARROW, GetModuleHandle(GAME_DEV_DLL_NAME));

	TCHAR szResourcePath[MAX_PATH];
	TCHAR szResourcePath2[MAX_PATH];
	GetCurrentDirectory(MAX_PATH,szResourcePath);

	_snprintf(szResourcePath2,sizeof(szResourcePath),TEXT("%s\\Data\\ComBg.png"),szResourcePath);
	if(m_ArrowBK.IsNull())
		m_ArrowBK.Load(szResourcePath2);
	}

public:
	BOOL		m_bFirstIn;		//鼠标第一次进入下拉按钮区域
	BYTE		m_ArrowBtnType;	//下拉按钮状态	
	COLORREF	m_BKColor;		//背景颜色
	COLORREF	m_EditBKColor;
	COLORREF	m_TextColor;	//字体颜色
	CBrush		m_Brush;		//背景画刷
	//CImageEx	m_ArrowBK;		//下拉按钮背景图
	CSkinImage	m_ArrowBK;		//下拉按钮背景图
	int			m_iBTabsX;		//下按按钮偏移X
	int			m_iBTabsY;		//下按按钮偏移Y
};


// ComboBoxEx
IMPLEMENT_DYNAMIC(ComboBoxEx, CComboBox)
BEGIN_MESSAGE_MAP(ComboBoxEx, CComboBox)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_MESSAGE(WM_MOUSELEAVE,OnMouseLeave)
	ON_WM_CTLCOLOR()
	ON_WM_CTLCOLOR_REFLECT()
    ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()

ComboBoxEx::ComboBoxEx()
{	
	pData = new CSkinComBoboxRes;
}

ComboBoxEx::~ComboBoxEx()
{	
	pData->m_ArrowBK.Destroy();//	DestroyImage();
	delete pData;
}

// ComboBoxEx 消息处理程序
void ComboBoxEx::OnPaint()
{		
	CPaintDC dc(this);	
	DefWindowProc(WM_PAINT, (WPARAM)dc.m_hDC, (LPARAM)0L);

	CDC* pDC = GetWindowDC();
	DrawBorder(pDC);
	ReleaseDC(pDC);
}


void ComboBoxEx::PreSubclassWindow()
{
	CComboBox::PreSubclassWindow();		
	SetWindowLong(GetSafeHwnd(), GWL_STYLE, GetWindowLong(GetSafeHwnd(), GWL_STYLE) | WS_BORDER);
	pData->m_ArrowBtnType = ARROWBTN_NORMAL;
}

HBRUSH ComboBoxEx::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{	
	if( nCtlColor == CTLCOLOR_LISTBOX || nCtlColor == CTLCOLOR_EDIT || nCtlColor == CTLCOLOR_MSGBOX )
	{
		pDC->SetBkMode(TRANSPARENT);
		pDC->SetTextColor(pData->m_TextColor);
		pDC->SetBkColor(pData->m_BKColor);
	}

	if (!IsWindowEnabled())
		return CComboBox::OnCtlColor(pDC, pWnd, pData->m_TextColor);	
	else
		return pData->m_Brush;
}

HBRUSH ComboBoxEx::CtlColor(CDC* pDC, UINT nCtlColor) 
{
	switch(nCtlColor) 
	{
	case CTLCOLOR_EDIT:	
		{
			pDC->SetBkMode(TRANSPARENT);
			pDC->SetTextColor(pData->m_TextColor);
			pDC->SetBkColor(pData->m_BKColor);

			if (!IsWindowEnabled())
				return NULL;
			else
				return pData->m_Brush;
		}
		break;

	default:
		return NULL;
	}
}



void ComboBoxEx::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (pData->m_ArrowBtnType != ARROWBTN_NO)
	{
		pData->m_ArrowBtnType = ARROWBTN_MOUSEDOWN;
		DrawArrow();
	}
	
	CComboBox::OnLButtonDown(nFlags, point);
}

void ComboBoxEx::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (pData->m_ArrowBtnType != ARROWBTN_NO)
	{
		pData->m_ArrowBtnType = ARROWBTN_MOUSEIN;
		DrawArrow();
	}
	
	CComboBox::OnLButtonUp(nFlags, point);
}

void ComboBoxEx::OnMouseMove(UINT nFlags, CPoint point)
{
	if (!pData->m_bFirstIn && pData->m_ArrowBtnType != ARROWBTN_NO)
	{			
		pData->m_bFirstIn	= TRUE;
		pData->m_ArrowBtnType = ARROWBTN_MOUSEIN;
		TRACKMOUSEEVENT TrackMouseEvent;
		TrackMouseEvent.cbSize=sizeof(TrackMouseEvent);
		TrackMouseEvent.dwFlags=TME_LEAVE;
		TrackMouseEvent.hwndTrack=GetSafeHwnd();
		TrackMouseEvent.dwHoverTime=HOVER_DEFAULT;
        _TrackMouseEvent(&TrackMouseEvent);
		DrawArrow();
	}

	CComboBox::OnMouseMove(nFlags, point);
}

LRESULT ComboBoxEx::OnMouseLeave(WPARAM wparam, LPARAM lparam)
{
	pData->m_bFirstIn	= FALSE;
	pData->m_ArrowBtnType = ARROWBTN_NORMAL;
	DrawArrow();
	return 0;
}


void ComboBoxEx::SetDefault()
{	
	pData->m_BKColor	= DefBkColor;
	pData->m_Brush.DeleteObject();
	pData->m_Brush.CreateSolidBrush(pData->m_BKColor);
	pData->m_TextColor	= DefTextColor;
	pData->m_ArrowBK.Destroy();//Image();	
//	pData->m_ArrowBK.LoadImage(IDB_ARROW, GetModuleHandle(GAME_DEV_DLL_NAME));

	//TCHAR szResourcePath[MAX_PATH];
	//TCHAR szResourcePath2[MAX_PATH];
	//GetCurrentDirectory(MAX_PATH,szResourcePath);

	//_snprintf(szResourcePath2,sizeof(szResourcePath),TEXT("%s\\BgM.png"),szResourcePath);
	//if(pData->m_ArrowBK.IsNull())
	//	pData->m_ArrowBK.Load(szResourcePath2);

	Invalidate(FALSE);
}

void ComboBoxEx::DrawBorder(CDC* pDC)
{
/////////////////////////////////// -- old
//	CRect rect;
//	GetWindowRect(&rect);
//	rect.OffsetRect(-rect.left, -rect.top);
//	pDC->FillSolidRect(CRect(0, 0, rect.Width(), 2), pData->m_BKColor);
//	pDC->FillSolidRect(CRect(0, rect.Height() - 2, rect.Width(), rect.Height()), pData->m_BKColor);
//	pDC->FillSolidRect(CRect(0, 0, 2, rect.Height()), pData->m_BKColor);
//	pDC->FillSolidRect(CRect(rect.Width() - pData->m_ArrowBK.GetWidth()/4 - DRAW_ARROWBTN_OFFSET_WIDTH*2,
//		0, rect.Width(), rect.Height()), pData->m_BKColor);
//
//	pDC->Draw3dRect(&rect, RGB(113,111,99), RGB(255,255,255));
//	pDC->Draw3dRect(1, 1, rect.right - 2, rect.bottom - 2, RGB(158,157,162), RGB(157,156,161));	
//	DrawArrow();
/////////////////////////////////// -- new
	CRect rect;
    GetClientRect(&rect);
    pDC->FillSolidRect(rect.left+1, rect.top+1, rect.right+5, rect.bottom+5, pData->m_EditBKColor);
    CPen pen1(PS_SOLID,1, pData->m_EditBKColor), pen2(PS_SOLID,1, pData->m_EditBKColor);
    pDC->SelectObject(&pen1);
    pDC->MoveTo(rect.left, rect.top);
    pDC->LineTo(rect.right + 2, rect.top);
    pDC->MoveTo(rect.left, rect.top);
    pDC->LineTo(rect.left, rect.bottom + 2);
    pDC->SelectObject(&pen2);
    pDC->MoveTo(rect.left  + 1, rect.bottom + 1);
    pDC->LineTo(rect.right + 1, rect.bottom + 1);
    pDC->MoveTo(rect.right + 1, rect.top    + 1);
    pDC->LineTo(rect.right + 1, rect.bottom + 2);
	DrawArrow();
}

void ComboBoxEx::SetFontColor(COLORREF fontcolor)
{
	pData->m_TextColor	= fontcolor;
	Invalidate(FALSE);
}

void ComboBoxEx::SetBKColor(COLORREF bkcolor)
{
	pData->m_BKColor	= bkcolor;
	pData->m_Brush.DeleteObject();
	pData->m_Brush.CreateSolidBrush(bkcolor);	
	Invalidate(FALSE);
}

//设置编辑框背景颜色
void ComboBoxEx::SetEditBKColor(COLORREF bkcolor)
{
	pData->m_EditBKColor = bkcolor;
	Invalidate(FALSE);
}

//设置编辑框中的按钮偏移
void ComboBoxEx::SetEditBTX(int iabsX)
{
	pData->m_iBTabsX = iabsX;
	Invalidate(FALSE);
}
void ComboBoxEx::SetEditBTY(int iabxY)
{
	pData->m_iBTabsY = iabxY;
	Invalidate(FALSE);
}

void ComboBoxEx::SetArrowBKImage(PTCHAR Path)
{
	if (Path == NULL)
		return;

	pData->m_ArrowBK.Destroy();//Image();
	
	//if (!pData->m_ArrowBK.LoadImage(Path))	
	//	pData->m_ArrowBK.LoadImage(IDB_ARROW, GetModuleHandle(GAME_DEV_DLL_NAME));	

	//TCHAR szResourcePath[MAX_PATH];
	//TCHAR szResourcePath2[MAX_PATH];
	//GetCurrentDirectory(MAX_PATH,szResourcePath);

	//_snprintf(szResourcePath2,sizeof(szResourcePath),TEXT("%s\\BgM.png"),szResourcePath);
	if(pData->m_ArrowBK.IsNull())
		pData->m_ArrowBK.Load(Path);
}

void ComboBoxEx::SetArrowBKImage(CSkinImage* pTmpImg)
{
	if (pTmpImg->IsNull())
		return;

	pData->m_ArrowBK.Destroy();//Image();

	//if (!pData->m_ArrowBK.CreateImage(pTmpImg->GetWidth(), pTmpImg->GetHeight()) ||
	//	!pData->m_ArrowBK.CopyImage(*pTmpImg, 0, 0))
	//	pData->m_ArrowBK.LoadImage(IDB_ARROW, GetModuleHandle(GAME_DEV_DLL_NAME));	

	//TCHAR szResourcePath[MAX_PATH];
	//TCHAR szResourcePath2[MAX_PATH];
	//GetCurrentDirectory(MAX_PATH,szResourcePath);

	//_snprintf(szResourcePath2,sizeof(szResourcePath),TEXT("%s\\BgM.png"),szResourcePath);

	if(pData->m_ArrowBK.IsNull())
	{
		pData->m_ArrowBK = *pTmpImg;
	}
}

void ComboBoxEx::DrawArrow()
{	
	if (m_hWnd && pData->m_ArrowBtnType != ARROWBTN_NO)
	{
		CClientDC dc(this);
		CDC* pDc = GetDC();
		CRect rect;
		GetClientRect(&rect);

		//if (!IsWindowEnabled())
		//	pData->m_ArrowBtnType = ARROWBTN_DISABLE;
		//
		pData->m_ArrowBK.BitBlt(dc, 
			rect.Width() - pData->m_ArrowBK.GetWidth()/4 - pData->m_iBTabsX, pData->m_iBTabsY, 
			pData->m_ArrowBK.GetWidth()/4, pData->m_ArrowBK.GetHeight(),
			pData->m_ArrowBK.GetWidth() * pData->m_ArrowBtnType/4, 0,
			SRCCOPY);
	}	
}
void ComboBoxEx::OnShowWindow(BOOL bShow, UINT nStatus)
{
    CComboBox::OnShowWindow(bShow, nStatus);
}
