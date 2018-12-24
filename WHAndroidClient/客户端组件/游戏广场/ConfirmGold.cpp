// ConfirmGold.cpp : 实现文件
//

#include "stdafx.h"
#include "ConfirmGold.h"
#include "webpath.h"
#include "InitData.h"
#include ".\confirmgold.h"

// CConfirmGold 对话框

IMPLEMENT_DYNAMIC(CConfirmGold, CDialog)
CConfirmGold::CConfirmGold(CWnd* pParent /*=NULL*/)
: CDialog(CConfirmGold::IDD, pParent)
{
	m_wMode=0;
}

CConfirmGold::~CConfirmGold()
{
	m_ImgBK.DestroyImage();
}

BEGIN_MESSAGE_MAP(CConfirmGold, CDialog)
	ON_BN_CLICKED(IDC_CONFIRM_GOLD_GETGOLD, OnBnClickedGetGold)
	ON_BN_CLICKED(IDC_CONFIRM_GOLD_OK, OnBnClickedConfirm)
	ON_WM_PAINT()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_CONFIRM_GOLD_GETALL, OnBnClickedConfirmGoldGetall)
END_MESSAGE_MAP()

void CConfirmGold::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CONFIRM_GOLD_GETGOLD, m_btnGetGold);
	DDX_Control(pDX, IDC_CONFIRM_GOLD_OK, m_btnConfirm);
	DDX_Control(pDX, IDC_CONFIRM_GOLD_GETALL, m_btnGetGoldAll);
}

//初始化
BOOL CConfirmGold::OnInitDialog()
{
	CDialog::OnInitDialog();

	int width   =   GetSystemMetrics(SM_CXSCREEN); 
	int height   =   GetSystemMetrics(SM_CYSCREEN);
	//调整窗口大小
	SetWindowPos(NULL, (width-366)/2, (height-366)/2, 400, 200, false);

	//当前路径
	CString strPath("");
	GetImagePlazaPath(strPath);

	//加载图片
	if (0==m_wMode)
	{//
		m_ImgBK.LoadImage(TEXT(strPath+"BKConfirmGold.png"));
	}
	else
	{//新手保护
		m_ImgBK.LoadImage(TEXT(strPath+"BKConfirmGoldGift.png"));
	}
	m_btnGetGoldAll.LoadRgnImage(TEXT(strPath+"BTConfirmGoldGetGoldAll.png"),false);
	m_btnConfirm.LoadRgnImage(TEXT(strPath+"BTConfirmGoldOK.png"),false);
	m_btnGetGold.LoadRgnImage(TEXT(strPath+"BTConfirmGoldGetGold.png"),false);

	//调整按钮位置
	HDWP hDwp=BeginDeferWindowPos(32);
	const UINT uFlags=SWP_NOACTIVATE|SWP_NOZORDER|SWP_NOCOPYBITS|SWP_NOSIZE;
		DeferWindowPos(hDwp, m_btnGetGold, NULL, 68, 148, 0, 0, uFlags);
		DeferWindowPos(hDwp, m_btnConfirm, NULL, 244, 148, 0, 0, uFlags);
		DeferWindowPos(hDwp, m_btnGetGoldAll, NULL, 68, 148, 0, 0, uFlags);
		if (0==m_wMode)
		{
			DeferWindowPos(hDwp, m_btnGetGold, NULL, 68, 148, 0, 0, uFlags);
			DeferWindowPos(hDwp, m_btnConfirm, NULL, 244, 148, 0, 0, uFlags);
		}
		else
		{
			DeferWindowPos(hDwp, m_btnConfirm, NULL, 160, 148, 0, 0, uFlags);
			DeferWindowPos(hDwp, m_btnGetGoldAll, NULL, 30, 148, 0, 0, uFlags);
		}
	EndDeferWindowPos(hDwp);

	m_btnGetGoldAll.ShowWindow(SW_SHOW);
	m_btnGetGold.ShowWindow(SW_SHOW);
	m_btnConfirm.ShowWindow(SW_SHOW);
	if (0==m_wMode)
	{
		m_btnGetGoldAll.ShowWindow(SW_HIDE);
	}
	else
	{
		m_btnGetGold.ShowWindow(SW_HIDE);
	}
	return true;
}
//绘画
void CConfirmGold::OnPaint()
{
	CPaintDC dc(this);
	CDC memdc;
	CBitmap bmp;
	bmp.CreateCompatibleBitmap(&dc, 400, 200);
	memdc.CreateCompatibleDC(&dc);
	memdc.SelectObject(bmp);

	if (!m_ImgBK.IsNull())
	{
		m_ImgBK.DrawImage(&memdc,0,0);
	}

	CRect rect;
	GetClientRect(&rect);
	dc.BitBlt(0, 0, rect.Width(), rect.Height(), &memdc, 0, 0, SRCCOPY);
	bmp.DeleteObject();
	memdc.DeleteDC();
}
//获取金币按钮
void CConfirmGold::OnBnClickedGetGold()
{
	if (0==m_wMode)
	{
		ShellExecute(NULL,"open",CInitData::s_cConfirmGetGold,"",NULL,SW_SHOW);
	}
	OnOK();
}
//获取更多金币
void CConfirmGold::OnBnClickedConfirmGoldGetall()
{
	ShellExecute(NULL,"open",CInitData::s_cConfirmGetGold,"",NULL,SW_SHOW);
	OnOK();
}
//确定按钮
void CConfirmGold::OnBnClickedConfirm()
{
	OnOK();
}
//当前路径
void CConfirmGold::GetCurrentDirectoryPath(CString &szPath)
{
	szPath="";
	TCHAR szPathTemp[MAX_PATH]={0};
	GetCurrentDirectory(MAX_PATH, szPathTemp);
	szPath.Append(szPathTemp);
}

//大厅资源路径
void CConfirmGold::GetImagePlazaPath(CString& strRetValue)
{
	CString strModuleFileName("");
	GetCurrentDirectoryPath(strModuleFileName);
	strModuleFileName.Append("\\data\\plaza\\");
	strRetValue = strModuleFileName;
}
//设置样式
void CConfirmGold::SetMode(WORD wMode)
{
	m_wMode=wMode;
}

void CConfirmGold::OnDestroy()
{
	CDialog::OnDestroy();

	m_ImgBK.DestroyImage();
}


