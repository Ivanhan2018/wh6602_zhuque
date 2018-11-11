// DlgAIPrompt.cpp : 实现文件
//

#include "stdafx.h"
#include "GameClient.h"
//#include "转转麻将客户端.h"
#include "DlgAIPrompt.h"
#include ".\dlgaiprompt.h"



//麻将宽与高
const int IMJWIDTH = 26;
const int IMJHEIGHT = 41;

//////////////////////////////////////////////////////////////////////////
//逻辑掩码

#define	CARD_MASK_COLOR					0xF0								//花色掩码
#define	CARD_MASK_VALUE					0x0F								//数值掩码

// CDlgAIPrompt 对话框

IMPLEMENT_DYNAMIC(CDlgAIPrompt, CDialog)
CDlgAIPrompt::CDlgAIPrompt(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgAIPrompt::IDD, pParent)
{
	m_iAbs_Left = 0;
	m_iAbs_Right = 0;
	m_iAbs_Center_X = 0;
	m_iAbs_Center_Y = 0;

	m_fCardAlpha = 1.0f;

	m_fDrawXY.x = 0;
	m_fDrawXY.y = 0;


	//清空牌列表
	m_vecCardList.clear();

	/*m_memdc.m_hDC = NULL;*/
}

CDlgAIPrompt::~CDlgAIPrompt()
{
	/*ReleaseDC(&m_memdc);
	m_memdc.m_hDC = NULL;*/
}

void CDlgAIPrompt::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CDlgAIPrompt, CDialog)
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


// CDlgAIPrompt 消息处理程序


//加载图片
bool CDlgAIPrompt::LoadImage(const char* pImagePath)
{
	if (NULL != pImagePath)
	{
		WCHAR* pImagePW=NULL;
		CString strImage="";
		strImage.Append(pImagePath);
		strImage.Append("ACTIONCARDNUM.png");
		pImagePW = strImage.AllocSysString();
		bool bRet = m_ImageCard.LoadFromFile(pImagePW);//牌
		if (!bRet)
		{
			return false;
		}

		strImage="";
		strImage.Append(pImagePath);
		strImage.Append("ACTIONCARDBACK.png");
		pImagePW = strImage.AllocSysString();
		bRet = m_ImageCardBack.LoadFromFile(pImagePW);//牌面
		if (!bRet)
		{
			return false;
		}
		//m_iAbs_Left = m_ImageCardBack.GetWidth();

		strImage="";
		strImage.Append(pImagePath);
		strImage.Append("AILEFT.png");
		pImagePW = strImage.AllocSysString();
		bRet = m_ImageLeft.LoadFromFile(pImagePW);//左
		if (!bRet)
		{
			return false;
		}
		m_iAbs_Left = m_ImageLeft.GetWidth();

		strImage="";
		strImage.Append(pImagePath);
		strImage.Append("AIRIGHT.png");
		pImagePW = strImage.AllocSysString();
		bRet = m_ImageRight.LoadFromFile(pImagePW);//右
		if (!bRet)
		{
			return false;
		}
		m_iAbs_Right = m_ImageRight.GetWidth();

		strImage="";
		strImage.Append(pImagePath);
		strImage.Append("AICENTER.png");
		pImagePW = strImage.AllocSysString();
		bRet = m_ImageCenter.LoadFromFile(pImagePW);//中
		if (!bRet)
		{
			return false;
		}

		m_iAbs_Center_X = m_ImageCenter.GetWidth();
		m_iAbs_Center_Y = m_ImageCenter.GetHeight();

		return true;
	}
	return false;
}


//设置牌数据
void CDlgAIPrompt::SetCardData(const BYTE* pCardData, int iCardNums)
{
	m_vecCardList.clear();
	if (NULL!=pCardData && iCardNums>0)
	{
		for (int i=0; i<iCardNums; ++i)
		{
			m_vecCardList.push_back(pCardData[i]);
		}
	}
}

//显示窗口
void CDlgAIPrompt::ShowWin(POINT ptPoint, ST_SHOWTAG showtag)
{
	//计算窗口大小
	int iWidth=0, iHeight=0;
	int iCardNums = m_vecCardList.size();
	if (iCardNums > 0)
	{
		iWidth = iCardNums*(IMJWIDTH+2);
		iWidth+=m_iAbs_Left;
		iWidth+=m_iAbs_Right;

		iHeight = m_iAbs_Center_Y/*?m_iAbs_Center_Y:IMJHEIGHT*/;

		CRect rect;
		if (ST_LEFT == showtag)
		{//左对齐
			rect.left = ptPoint.x;
		}
		else if (ST_RIGHT == showtag)
		{//右对齐
			rect.left = ptPoint.x-iWidth;
		}
		else
		{//居中对齐
			rect.left = ptPoint.x-iWidth/2;
		}

		m_fDrawXY.x = rect.left;
		m_fDrawXY.y = ptPoint.y;
		m_iCurWidth = iWidth;
		m_iCurHeight = iHeight;

		rect.top = ptPoint.y;
		rect.right = rect.left + iWidth;
		rect.bottom = rect.top + iHeight;

		CRect wndRect;
		CWnd* pMain = AfxGetMainWnd();
		if (NULL != pMain)
		{
			pMain->GetWindowRect(&wndRect);
		}

		SetWindowPos(NULL, wndRect.left+rect.left, wndRect.top+rect.top, m_iCurWidth, m_iCurHeight, SWP_SHOWWINDOW);
		if (GetSafeHwnd())
		{
			InvalidateRect(NULL, FALSE);
		}
	}
}


//设置牌透明度
void CDlgAIPrompt::SetCardAlpha(float fAlpha)
{
	if (fAlpha>=1.0f)
	{
		fAlpha = 1.0f;
	}
	m_fCardAlpha = fAlpha;
}

//设置牌绘制起点
void CDlgAIPrompt::SetCardDrawXY(POINT ptPoint)
{
	m_fDrawXY = ptPoint;
}


void CDlgAIPrompt::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 在此处添加消息处理程序代码
	// 不为绘图消息调用 CDialog::OnPaint()

	CRect	rect;
	GetClientRect(&rect);

	CMemDC dcMem(&dc, rect, false);

	if (NULL != dcMem.m_hDC)
	{
		DWORD iWidth = m_iCurWidth;
		DWORD iHeight = m_iCurHeight;
		POINT ptStart;
		ptStart.x = 0;
		ptStart.y = 0;
		//绘制背景
		//中
		if (!m_ImageCenter.IsNull())
		{
			DWORD iCenterX = ptStart.x+m_iAbs_Left;
			for (;iCenterX<=ptStart.x+iWidth-m_iAbs_Right; iCenterX+=m_iAbs_Center_X)
			{
				m_ImageCenter.DrawImage(&dcMem, iCenterX, ptStart.y);
			}
			int iLessW = (ptStart.x+iWidth-m_iAbs_Right)-iCenterX;
			if (iLessW > 0)
			{
				m_ImageCenter.DrawImage(&dcMem, iCenterX, ptStart.y, iLessW, m_iAbs_Center_Y, 0, 0, iLessW, m_iAbs_Center_Y);
			}
		}
		//左
		if (!m_ImageLeft.IsNull())
		{
			m_ImageLeft.DrawImage(&dcMem, ptStart.x, ptStart.y);
		}
		//右
		if (!m_ImageRight.IsNull())
		{
			m_ImageRight.DrawImage(&dcMem, ptStart.x+iWidth-m_iAbs_Right, ptStart.y);
		}

		//绘制牌
		if (!m_ImageCard.IsNull())
		{
			int iCardNums = m_vecCardList.size();
			if (iCardNums > 0)
			{
				for (int i=0; i<iCardNums; ++i)
				{
					BYTE cbValue=m_vecCardList[i]&CARD_MASK_VALUE;
					BYTE cbColor=(m_vecCardList[i]&CARD_MASK_COLOR)>>4;
					BYTE cbCardIndex = (cbColor>=0x03)?(cbValue+27):(cbColor*9+cbValue-1);
					m_ImageCardBack.DrawImage(&dcMem, ptStart.x+i*(IMJWIDTH+2)+12, ptStart.y+8, IMJWIDTH, IMJHEIGHT, 0, 0, IMJWIDTH, IMJHEIGHT, m_fCardAlpha);
					m_ImageCard.DrawImage(&dcMem, ptStart.x+i*(IMJWIDTH+2)+10, ptStart.y+12, 32, 35, cbCardIndex*32, 0, 32, 35, m_fCardAlpha);
				}
			}
		}
		UpdateRgn(&dcMem);
		//dc.BitBlt(0, 0, m_iCurWidth, m_iCurHeight, &dcMem, 0, 0, SRCCOPY);
	}
}


void CDlgAIPrompt::AddRectRgn(CDC* pDC,CRgn* rgn,CRect* rect,COLORREF col)
{
	for(int y=rect->top;y<rect->bottom;y++)
		for(int x=rect->left;x<rect->right;x++)
			if (pDC->GetPixel(x,y)==col)
			{
				CRgn rgnAdd;
				rgnAdd.CreateRectRgn (x, y, x+1, y+1);
				rgn->CombineRgn(rgn, &rgnAdd, RGN_DIFF);
				rgnAdd.DeleteObject();
			}
}
void CDlgAIPrompt::UpdateRgn(CDC * pDC)
{
	CRect rcClient;
	GetClientRect(&rcClient);
	CRgn rgn;
	COLORREF col = RGB(241,254,0);  // mask color
	rgn.CreateRectRgn (0, 0, rcClient.Width(), rcClient.Height());

	CRect rc(0,0,0,0);
	rc.right = rcClient.Width();
	rc.bottom = rcClient.Height();
	AddRectRgn(pDC,&rgn,&rc,col);
	SetWindowRgn (rgn,TRUE);
}

BOOL CDlgAIPrompt::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	m_iAbs_Left = 0;
	m_iAbs_Right = 0;
	m_iAbs_Center_X = 0;
	m_iAbs_Center_Y = 0;

	m_fCardAlpha = 1.0f;

	m_fDrawXY.x = 0;
	m_fDrawXY.y = 0;


	//清空牌列表
	m_vecCardList.clear();

	/*if (NULL != m_memdc.m_hDC)
	{
		ReleaseDC(&m_memdc);
	}
	m_memdc.m_hDC = NULL;*/
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CDlgAIPrompt::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	

	/*CDC* pDC = GetDC();
	if (NULL != m_memdc.m_hDC)
	{
		ReleaseDC(&m_memdc);
	}
	m_memdc.m_hDC = NULL;
	m_memdc.CreateCompatibleDC(pDC);
	CBitmap hBmp;
	hBmp.CreateCompatibleBitmap(pDC, cx, cy);
	m_memdc.SelectObject(&hBmp);
	m_memdc.SetBkMode(TRANSPARENT);
	m_memdc.FillSolidRect(0,0,cx,cy,RGB(241,254,0));
	hBmp.DeleteObject();
	ReleaseDC(pDC);
	pDC = NULL;
	InvalidateRect(NULL, FALSE);*/
	
	// TODO: 在此处添加消息处理程序代码
}

BOOL CDlgAIPrompt::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	return TRUE;
	//return CDialog::OnEraseBkgnd(pDC);
}
