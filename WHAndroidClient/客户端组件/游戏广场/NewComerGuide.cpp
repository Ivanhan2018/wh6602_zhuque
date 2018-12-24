// NewComerGuide.cpp : 实现文件
//

#include "stdafx.h"
#include "NewComerGuide.h"
#include ".\newcomerguide.h"

//#define IDI_WEB_NEWCOMERGUIDE					5001					//网页

// CNewComerGuide 对话框

IMPLEMENT_DYNAMIC(CNewComerGuide, CDialog)
CNewComerGuide::CNewComerGuide(CWnd* pParent /*=NULL*/)
	: CDialog(CNewComerGuide::IDD, pParent)
{
	m_wTagPos=0;
}

CNewComerGuide::~CNewComerGuide()
{
}

void CNewComerGuide::OnDestroy()
{
	CDialog::OnDestroy();

	if (!m_ImgNewComerGuide.IsNull()) m_ImgNewComerGuide.DestroyImage();
	if (!m_ImgTag.IsNull()) m_ImgTag.DestroyImage();

	if (!m_ImgGameTask.IsNull()) m_ImgGameTask.DestroyImage();
	if (!m_ImgGameCurrency.IsNull()) m_ImgGameCurrency.DestroyImage();
	if (!m_ImgPropsShop.IsNull()) m_ImgPropsShop.DestroyImage();
	if (!m_ImgPrizeExcharge.IsNull()) m_ImgPrizeExcharge.DestroyImage();
	if (!m_ImgGameHelp.IsNull()) m_ImgGameHelp.DestroyImage();
	if (!m_ImgNewComerGift.IsNull()) m_ImgNewComerGift.DestroyImage();
}

void CNewComerGuide::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDCANCEL, m_btnClose);
	DDX_Control(pDX, IDC_NEWCOMER_GIFT, m_btnNewComerGift);
	DDX_Control(pDX, IDC_NEWCOMER_GAMETASK, m_btnGameTask);
	DDX_Control(pDX, IDC_NEWCOMER_GAMECURRENCY, m_btnGameCurrency);
	DDX_Control(pDX, IDC_NEWCOMER_PROPSSHOP, m_btnPropsShop);
	DDX_Control(pDX, IDC_NEWCOMER_PRIZEEXCHARGE, m_btnPrizeExcharge);
	DDX_Control(pDX, IDC_NEWCOMER_GAMEHELP, m_btnGameHelp);
}

BEGIN_MESSAGE_MAP(CNewComerGuide, CDialog)
	ON_WM_DESTROY()
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_NEWCOMER_GIFT, OnBnClickedNewcomerGift)
	ON_BN_CLICKED(IDC_NEWCOMER_GAMETASK, OnBnClickedNewcomerGametask)
	ON_BN_CLICKED(IDC_NEWCOMER_GAMECURRENCY, OnBnClickedNewcomerGamecurrency)
	ON_BN_CLICKED(IDC_NEWCOMER_PROPSSHOP, OnBnClickedNewcomerPropsshop)
	ON_BN_CLICKED(IDC_NEWCOMER_PRIZEEXCHARGE, OnBnClickedNewcomerPrizeexcharge)
	ON_BN_CLICKED(IDC_NEWCOMER_GAMEHELP, OnBnClickedNewcomerGamehelp)
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
END_MESSAGE_MAP()


// CNewComerGuide 消息处理程序

BOOL CNewComerGuide::OnInitDialog()
{
	CDialog::OnInitDialog();

	CPoint systemMeTrice;
	int x = GetSystemMetrics(SM_CXSCREEN);
	int y = GetSystemMetrics(SM_CYSCREEN);
	systemMeTrice.x = x;
	systemMeTrice.y = y;
	ClientToScreen(&systemMeTrice);
	SetWindowPos(NULL, systemMeTrice.x-x, systemMeTrice.y-y, 700, 500, 0);
	
	m_rcTagPos.left=170;
	m_rcTagPos.top=95;
	m_rcTagPos.right=m_rcTagPos.left+516;
	m_rcTagPos.bottom=m_rcTagPos.top+392;

	m_wTagPos= 122;

	CString sTemp("");
	GetResPath("\\data\\guide\\", sTemp);
	m_ImgNewComerGuide.LoadImage(TEXT(sTemp+"BKNewComerGuide.png"));
	m_ImgTag.LoadImage(TEXT(sTemp+"BTFlag.png"));

	m_ImgGameTask.LoadImage(TEXT(sTemp+"FTTask.png"));
	m_ImgGameCurrency.LoadImage(TEXT(sTemp+"FTGameCurrency.png"));
	m_ImgPropsShop.LoadImage(TEXT(sTemp+"FTPropshop.png"));
	m_ImgPrizeExcharge.LoadImage(TEXT(sTemp+"FTPrizeExcharge.png"));
	m_ImgGameHelp.LoadImage(TEXT(sTemp+"FTGameHelp.png"));
	m_ImgNewComerGift.LoadImage(TEXT(sTemp+"FTGift.png"));

	m_btnClose.SetButtonImage(TEXT(sTemp+"BTClose.png"), false);
	m_btnGameTask.SetButtonImage(TEXT(sTemp+"BTTask.png"), false);
	m_btnGameCurrency.SetButtonImage(TEXT(sTemp+"BTGameCurrency.png"), false);
	m_btnPropsShop.SetButtonImage(TEXT(sTemp+"BTPropshop.png"), false);
	m_btnPrizeExcharge.SetButtonImage(TEXT(sTemp+"BTPrizeExcharge.png"), false);
	m_btnGameHelp.SetButtonImage(TEXT(sTemp+"BTGameHelp.png"), false);
	m_btnNewComerGift.SetButtonImage(TEXT(sTemp+"BTGift.png"), false);

	/*sTemp="";
	GetResPath("\\Url\\guide\\", sTemp);
	m_webNewComerGuide.Create(NULL,NULL,WS_VISIBLE | WS_CHILD,CRect(0,0,0,0),this,IDI_WEB_NEWCOMERGUIDE,NULL);
	m_webNewComerGuide.Navigate(TEXT(sTemp+"guide_index.html"),NULL,NULL,NULL,NULL);
	m_webNewComerGuide.MoveWindow(168, 64, 324, 328);*/

	//调整按钮位置
	HDWP hDwp=BeginDeferWindowPos(32);
	const UINT uFlags=SWP_NOACTIVATE|SWP_NOZORDER|SWP_NOCOPYBITS|SWP_NOSIZE;
		DeferWindowPos(hDwp, m_btnGameTask,			NULL, 23, 171, 0, 0, uFlags);
		DeferWindowPos(hDwp, m_btnGameCurrency,		NULL, 23, 233, 0, 0, uFlags);
		DeferWindowPos(hDwp, m_btnPropsShop,		NULL, 23, 295, 0, 0, uFlags);
		DeferWindowPos(hDwp, m_btnPrizeExcharge,	NULL, 23, 359, 0, 0, uFlags);
		DeferWindowPos(hDwp, m_btnGameHelp,			NULL, 23, 422, 0, 0, uFlags);
		DeferWindowPos(hDwp, m_btnClose,			NULL, 640, 4, 0, 0, uFlags);
		DeferWindowPos(hDwp, m_btnNewComerGift,		NULL, 23, 108, 0, 0, uFlags);
	EndDeferWindowPos(hDwp);

	m_btnGameTask.ShowWindow(SW_SHOW);
	m_btnGameCurrency.ShowWindow(SW_SHOW);
	m_btnPropsShop.ShowWindow(SW_SHOW);
	m_btnPrizeExcharge.ShowWindow(SW_SHOW);
	m_btnGameHelp.ShowWindow(SW_SHOW);
	m_btnClose.ShowWindow(SW_SHOW);
	m_btnNewComerGift.ShowWindow(SW_SHOW);

	SetImageRgn(&m_ImgNewComerGuide);

	m_btnNewComerGift.GetFocus();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}
//当前路径
void CNewComerGuide::GetCurrentDirectoryPath(CString &szPath)
{
	szPath="";
	TCHAR szPathTemp[MAX_PATH]={0};
	GetCurrentDirectory(MAX_PATH, szPathTemp);
	szPath.Append(szPathTemp);
}
//大厅资源路径
void CNewComerGuide::GetResPath(CString strClipName, CString& strPath)
{
	CString strModuleFileName("");
	GetCurrentDirectoryPath(strModuleFileName);
	strModuleFileName.Append(strClipName);
	strPath = strModuleFileName;
}

void CNewComerGuide::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	CDC memdc;
	CBitmap bmp;
	bmp.CreateCompatibleBitmap(&dc, 700, 500);
	memdc.CreateCompatibleDC(&dc);
	memdc.SelectObject(bmp);

	if (!m_ImgNewComerGuide.IsNull())
	{//背景
		m_ImgNewComerGuide.DrawImage(&memdc,0,0);
	}
	if (!m_ImgTag.IsNull())
	{//标志位
		m_ImgTag.DrawImage(&memdc,m_rcTagPos.left,m_wTagPos,m_ImgTag.GetWidth(),m_ImgTag.GetHeight(),0,0);
	}

	ExplainSwerve(memdc);

	CRect rect;
	GetClientRect(&rect);
	dc.BitBlt(0, 0, rect.Width(), rect.Height(), &memdc, 0, 0, SRCCOPY);
	bmp.DeleteObject();
	memdc.DeleteDC();
}
//礼包
void CNewComerGuide::OnBnClickedNewcomerGift()
{
	m_wTagPos=122;
	InvalidateRect(&m_rcTagPos);
}
//游戏任务
void CNewComerGuide::OnBnClickedNewcomerGametask()
{
	m_wTagPos=186;
	InvalidateRect(&m_rcTagPos);
}
//游戏货币
void CNewComerGuide::OnBnClickedNewcomerGamecurrency()
{
	m_wTagPos=249;
	InvalidateRect(&m_rcTagPos);
}
//道具商城
void CNewComerGuide::OnBnClickedNewcomerPropsshop()
{
	m_wTagPos=312;
	InvalidateRect(&m_rcTagPos);
}
//兑换
void CNewComerGuide::OnBnClickedNewcomerPrizeexcharge()
{
	m_wTagPos=375;
	InvalidateRect(&m_rcTagPos);
}
//游戏帮助
void CNewComerGuide::OnBnClickedNewcomerGamehelp()
{
	m_wTagPos=437;
	InvalidateRect(&m_rcTagPos);
}
//转向
void CNewComerGuide::ExplainSwerve(CDC &memdc)
{
	switch(m_wTagPos)
	{
	case 122://礼包
		if (!m_ImgNewComerGift.IsNull())
		{
			m_ImgNewComerGift.DrawImage(&memdc,186,97,m_ImgNewComerGift.GetWidth(),m_ImgNewComerGift.GetHeight(),0,0);
		}
		break;
	case 186://任务
		if (!m_ImgGameTask.IsNull())
		{
			m_ImgGameTask.DrawImage(&memdc,186,97,m_ImgGameTask.GetWidth(),m_ImgGameTask.GetHeight(),0,0);
		}
		break;
	case 249://货币
		if (!m_ImgGameCurrency.IsNull())
		{
			m_ImgGameCurrency.DrawImage(&memdc,186,97,m_ImgGameCurrency.GetWidth(),m_ImgGameCurrency.GetHeight(),0,0);
		}
		break;
	case 312://商城
		if (!m_ImgPropsShop.IsNull())
		{
			m_ImgPropsShop.DrawImage(&memdc,186,97,m_ImgPropsShop.GetWidth(),m_ImgPropsShop.GetHeight(),0,0);
		}
		break;
	case 375://兑换
		if (!m_ImgPrizeExcharge.IsNull())
		{
			m_ImgPrizeExcharge.DrawImage(&memdc,186,97,m_ImgPrizeExcharge.GetWidth(),m_ImgPrizeExcharge.GetHeight(),0,0);
		}
		break;
	case 437://帮助
		if (!m_ImgGameHelp.IsNull())
		{
			m_ImgGameHelp.DrawImage(&memdc,186,97,m_ImgGameHelp.GetWidth(),m_ImgGameHelp.GetHeight(),0,0);
		}
		break;
	}
	/*CString sTemp("");
	GetResPath("\\Url\\guide\\", sTemp);
	switch(wNumber)
	{
	case 1:
		m_webNewComerGuide.Navigate(TEXT(sTemp+"guide_index.html"),NULL,NULL,NULL,NULL);
		break;
	case 2:
		m_webNewComerGuide.Navigate(TEXT(sTemp+"guide_index.html"),NULL,NULL,NULL,NULL);
		break;
	case 3:
		m_webNewComerGuide.Navigate(TEXT(sTemp+"guide_index.html"),NULL,NULL,NULL,NULL);
		break;
	case 4:
		m_webNewComerGuide.Navigate(TEXT(sTemp+"guide_index.html"),NULL,NULL,NULL,NULL);
		break;
	case 5:
		m_webNewComerGuide.Navigate(TEXT(sTemp+"guide_index.html"),NULL,NULL,NULL,NULL);
		break;
	case 6:
		m_webNewComerGuide.Navigate(TEXT(sTemp+"guide_index.html"),NULL,NULL,NULL,NULL);
		break;
	}*/
}

BOOL CNewComerGuide::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	return true;
	//return CDialog::OnEraseBkgnd(pDC);
}

BOOL CNewComerGuide::SetImageRgn(CPngImage * pImage)
{

	/*if (NULL==pImage) return FALSE;
	if (pImage->IsNull()) return FALSE;
	INT wWidth = pImage->GetWidth();
	INT wHeight = pImage->GetHeight();
	CBitmap * pbmp;
	CImage imgBuf;
	imgBuf.Create(wWidth, wHeight, 32);
	CDC *pDCTmp = CDC::FromHandle(imgBuf.GetDC());
	pImage->DrawImage(pDCTmp, 0, 0, wWidth, wHeight, 0, 0, wWidth, wHeight);
	pbmp = ((CBitmap *)(&imgBuf));
	DWORD * pData = new DWORD[wWidth * wHeight];
	pbmp->GetBitmapBits((sizeof (DWORD)* wWidth * wHeight), pData);
	imgBuf.ReleaseDC();
	CRgn rgnImage, tempRgn;
	rgnImage.CreateRectRgn(0, 0, 0, 0);
	DWORD *pDataTmp = pData;
	for (UINT h = 0;h < wHeight;++h)
	{
		UINT w = 0, leftX = 0;
		while(w < wWidth)
		{
			while (w++ < wWidth &&(0 == (*pDataTmp++ & 0xff000000)));
			leftX =(w - 1);
			while (w++ < wWidth &&(0 != (*pDataTmp++ & 0xff000000))); 
			tempRgn.CreateRectRgn(leftX, h,(w - 1),(h + 1));
			rgnImage.CombineRgn(&rgnImage, &tempRgn, RGN_OR);
			tempRgn.DeleteObject();
		}
	}

	SafeDeleteArray(pData);
	imgBuf.Destroy();

	if (NULL == rgnImage.m_hObject)
		return FALSE;

	::SetWindowRgn(m_hWnd, (HRGN)rgnImage.m_hObject, TRUE);

	rgnImage.DeleteObject();

	return TRUE;*/


	if (!pImage) 
		return FALSE;
	if (pImage->IsNull()) 
		return FALSE;

	CBitmap* pBmp;
	CImage imgBuf;

	int iWidth = pImage->GetWidth();
	int iHeight = pImage->GetHeight();
	int iCount = 0;

	imgBuf.Create(iWidth, iHeight, 32);
	CDC *pDCTmp = CDC::FromHandle(imgBuf.GetDC());	// 缺少成对出现的 CDC::DeleteTempMap  ???
	pImage->DrawImage(pDCTmp, 0, 0, iWidth, iHeight, 0, 0, iWidth, iHeight);
	pBmp = ((CBitmap *)(&imgBuf));
	DWORD * pData = new DWORD[iWidth * iHeight];
	pBmp->GetBitmapBits((sizeof (DWORD)* iWidth * iHeight),  pData);

	CRgn rgnImage;			// pImage 的 镜像
	CRgn rgnDecrease;		// 要剪掉的区域

	rgnImage.CreateRectRgn(0,  0,  iWidth,  iHeight);
	rgnDecrease.CreateRectRgn(0,  0,  iWidth,  iHeight);

	DWORD *pDataTmp = pData;

	bool		bStart = true;	// 可以做起点
	CPoint	ptStartTrans;		// 透明的起点
	CPoint	ptEndTrans;			// 透明的终点
	int		iPix;		
	int		iPhalanx = 0;		// 方阵的点

	for (int h=0; h<iHeight; ++h)	// 高度
	{
		iPhalanx = iWidth * h;
		bStart = true;				// 可以做起点
		ptStartTrans.SetPoint(0, h);
		ptEndTrans.SetPoint(0, h);
		for(int w=0;  w<iWidth;  w++, pDataTmp++)
		{
			iPix = (*pDataTmp) & 0xff000000;
			if(0 == iPix)	// 完全透明
			{
				if(bStart)
				{
					ptStartTrans.SetPoint(w, h);
					bStart = false;	// 不能做起点
				}
				// 最后一个起点透明 ,直接裁剪
				if(w==(iWidth-1))
				{
					// 裁剪，透明会比非透明的少，所以裁剪透明比叠加非透明会更有效率 ...... 
					rgnDecrease.SetRectRgn(ptEndTrans.x+2, ptEndTrans.y,  iWidth,  ptStartTrans.y+1);	// 要裁剪的区域
					rgnImage.CombineRgn(&rgnImage, &rgnDecrease, RGN_XOR);	// 两个剪裁区域的全部除去公共部分		
					iCount++;
				}
			}
			else	// 不完全透明
			{
				ptEndTrans.SetPoint(w-1, h);

				// 安全距离并且
				if(ptStartTrans.x <= ptEndTrans.x)
				{
					iPix = pData[iPhalanx + ptEndTrans.x ] & 0xff000000;
					if (0==iPix)	// 该点透明
					{
						// 裁剪，透明会比非透明的少，所以裁剪透明比叠加非透明会更有效率 ...... 
						rgnDecrease.SetRectRgn(ptStartTrans.x, ptStartTrans.y,  ptEndTrans.x,  ptEndTrans.y+1);	// 要裁剪的区域
						rgnImage.CombineRgn(&rgnImage, &rgnDecrease, RGN_XOR);	// 	两个剪裁区域的全部除去公共部分	
						iCount++;
					}
				}
				bStart = true;
			}
		}
	}

#if _DEBUG
	CString strTmp;
	strTmp.Format("iCount=%d \r\n", iCount);
	TRACE(strTmp.GetBuffer());
#endif

	SafeDeleteArray(pData);
	imgBuf.ReleaseDC();
	imgBuf.Destroy();

	BOOL bResult = TRUE;

	if (rgnImage.m_hObject)
	{
		::SetWindowRgn(m_hWnd, (HRGN)rgnImage.m_hObject, TRUE);
		DeleteObject(rgnImage);
	}
	else
	{
		bResult = FALSE;
	}

	DeleteObject(rgnDecrease);

	CDC::DeleteTempMap();	// add by HouGuoJiang 2011-11-25

	return bResult;

}

void CNewComerGuide::OnLButtonDown(UINT nFlags, CPoint point)
{
	PostMessage(WM_NCLBUTTONDOWN,HTCAPTION,MAKELPARAM(point.x,point.y));

	CDialog::OnLButtonDown(nFlags, point);
}

void CNewComerGuide::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	OnCancel();
}
