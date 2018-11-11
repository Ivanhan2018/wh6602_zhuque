// GameSoundSet.cpp : 实现文件
//

#include "stdafx.h"
#include "ActionWind.h"
#include "GameFrameDlg.h"
#include ".\ActionWind.h"



// CGameSoundSet 对话框

IMPLEMENT_DYNAMIC(CGameSoundSet, CImgDialogBase)

CGameSoundSet::CGameSoundSet(LPCTSTR lpszFile, CWnd* pParent)
				: CImgDialogBase(CGameSoundSet::IDD
				 , lpszFile
				 , pParent
				 ), m_pGameFrameDlg(NULL)
{
}


CGameSoundSet::~CGameSoundSet()
{
	m_pGameFrameDlg = NULL;
}

void CGameSoundSet::DoDataExchange(CDataExchange* pDX)
{
	CImgDialogBase::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CGameSoundSet, CImgDialogBase)
	ON_WM_PAINT()
END_MESSAGE_MAP()


// CGameSoundSet 消息处理程序

BOOL CGameSoundSet::OnInitDialog()
{
	CImgDialogBase::OnInitDialog();

	// TODO:  在此添加额外的初始化
	
	TCHAR szResourcePath[MAX_PATH];	GetCurrentDirectory(MAX_PATH,szResourcePath);
	CString strPath(szResourcePath);

	//背景图
	m_BackImage.LoadImage(TEXT(strPath + "SHUNZI.png"));
	//if (!m_BackImage.IsNull())
	//{
	//	SetImageRgn(&m_BackImage);
	//	SetWindowPos(NULL, 0, 0, m_BackImage.GetWidth(), m_BackImage.GetHeight(), SWP_NOMOVE); 
	//}
	
	m_nAlpha = 255;
	m_bShown = TRUE;
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


BOOL CGameSoundSet::SetImageRgn(CPngImage * pImage)
{
	if (NULL==pImage) return FALSE;
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

	SetWindowRgn((HRGN)rgnImage.m_hObject, TRUE);
	rgnImage.DeleteObject();
	return TRUE;
}

void CGameSoundSet::OnDraw(Graphics & graphics)
{
}

void CGameSoundSet::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 在此处添加消息处理程序代码
	// 不为绘图消息调用 CImgDialogBase::OnPaint()
	if (!m_BackImage.IsNull())
	{
		m_BackImage.DrawImage(&dc, 0, 0);
	}
}
