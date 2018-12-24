// GameRuleDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "GameRuleDlg.h"
#include ".\gameruledlg.h"

#define IDI_WEB_NEWCOMERGUIDE					5001					//网页
// CGameRuleDlg 对话框

IMPLEMENT_DYNAMIC(CGameRuleDlg, CDialog)
CGameRuleDlg::CGameRuleDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CGameRuleDlg::IDD, pParent)
{
}

CGameRuleDlg::~CGameRuleDlg()
{
}

void CGameRuleDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDCANCEL, m_btnClose);
}


BEGIN_MESSAGE_MAP(CGameRuleDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_DESTROY()
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()


void CGameRuleDlg::OnDestroy()
{
	CDialog::OnDestroy();

	if (!m_ImgBK.IsNull()) m_ImgBK.DestroyImage();
}

BOOL CGameRuleDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	/*int width   =   GetSystemMetrics(SM_CXSCREEN); 
	int height   =   GetSystemMetrics(SM_CYSCREEN);
	SetWindowPos(NULL, (width-960)/2, (height-720)/2, 500, 400, 0);*/

	CPoint systemMeTrice;
	int x = GetSystemMetrics(SM_CXSCREEN);
	int y = GetSystemMetrics(SM_CYSCREEN);
	systemMeTrice.x = x;
	systemMeTrice.y = y;
	ClientToScreen(&systemMeTrice);
	SetWindowPos(NULL, systemMeTrice.x-x, systemMeTrice.y-y, 500, 400, 0);

	CString sTemp("");
	GetResPath("\\Url\\rule\\", sTemp);
	m_webBrowser.Create(NULL,NULL,WS_VISIBLE | WS_CHILD,CRect(0,0,0,0),this,IDI_WEB_NEWCOMERGUIDE,NULL);
	m_webBrowser.Navigate(TEXT(sTemp+"guide_index.html"),NULL,NULL,NULL,NULL);
	m_webBrowser.Navigate(TEXT(m_sRuleUrl),NULL,NULL,NULL,NULL);
	m_webBrowser.MoveWindow(7, 36, 486, 356);

	sTemp="";
	GetResPath("\\data\\plaza\\", sTemp); 
	m_ImgBK.LoadImage(TEXT(sTemp+"BKGameRule.png"));
	m_btnClose.LoadRgnImage(TEXT(sTemp+"BTRuleClose.png"), false);
	//调整按钮位置
	HDWP hDwp=BeginDeferWindowPos(32);
		const UINT uFlags=SWP_NOACTIVATE|SWP_NOZORDER|SWP_NOCOPYBITS|SWP_NOSIZE;
		DeferWindowPos(hDwp, m_btnClose,			NULL, 452, 9, 0, 0, uFlags);
	EndDeferWindowPos(hDwp);

	SetImageRgn(&m_ImgBK);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CGameRuleDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	CDC memdc;
	CBitmap bmp;
	bmp.CreateCompatibleBitmap(&dc, 500, 400);
	memdc.CreateCompatibleDC(&dc);
	memdc.SelectObject(bmp);

	if (!m_ImgBK.IsNull())
	{//背景
		m_ImgBK.DrawImage(&memdc,0,0);
	}

	CRect rect;
	GetClientRect(&rect);
	dc.BitBlt(0, 0, rect.Width(), rect.Height(), &memdc, 0, 0, SRCCOPY);
	bmp.DeleteObject();
	memdc.DeleteDC();
}

//设置
void CGameRuleDlg::SetRuleUrl(CString sUrl)
{
	m_sRuleUrl=sUrl;
}
//当前路径
void CGameRuleDlg::GetCurrentDirectoryPath(CString &szPath)
{
	szPath="";
	TCHAR szPathTemp[MAX_PATH]={0};
	GetCurrentDirectory(MAX_PATH, szPathTemp);
	szPath.Append(szPathTemp);
}
//大厅资源路径
void CGameRuleDlg::GetResPath(CString strClipName, CString& strPath)
{
	CString strModuleFileName("");
	GetCurrentDirectoryPath(strModuleFileName);
	strModuleFileName.Append(strClipName);
	strPath = strModuleFileName;
}
// CGameRuleDlg 消息处理程序
BOOL CGameRuleDlg::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	return true;
	//return CDialog::OnEraseBkgnd(pDC);
}

BOOL CGameRuleDlg::SetImageRgn(CPngImage * pImage)
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

void CGameRuleDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	PostMessage(WM_NCLBUTTONDOWN,HTCAPTION,MAKELPARAM(point.x,point.y));

	CDialog::OnLButtonDown(nFlags, point);
}
