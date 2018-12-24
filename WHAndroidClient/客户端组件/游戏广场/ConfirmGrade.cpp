// ConfirmGrade.cpp : 实现文件
//

#include "stdafx.h"
#include "ConfirmGrade.h"


// CConfirmGrade 对话框

IMPLEMENT_DYNAMIC(CConfirmGrade, CDialog)
CConfirmGrade::CConfirmGrade(CWnd* pParent /*=NULL*/)
	: CDialog(CConfirmGrade::IDD, pParent)
{
}

CConfirmGrade::~CConfirmGrade()
{
	m_ImgBK.DestroyImage();
}

void CConfirmGrade::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CONFIRM_GRADE_OK, m_btnConfirm);
}

BEGIN_MESSAGE_MAP(CConfirmGrade, CDialog)
	ON_BN_CLICKED(IDC_CONFIRM_GRADE_OK,OnBnClickedConfirm)
	ON_WM_PAINT()
END_MESSAGE_MAP()
//初始化
BOOL CConfirmGrade::OnInitDialog()
{
	CDialog::OnInitDialog();

	//调整窗口大小
	int width   =   GetSystemMetrics(SM_CXSCREEN); 
	int height   =   GetSystemMetrics(SM_CYSCREEN);
	SetWindowPos(NULL, (width-366)/2, (height-366)/2, 400, 200, false);

	//获取当前路径
	CString strPath("");
	GetImagePlazaPath(strPath);

	//加载图片
	if(m_ImgBK.IsNull()) m_ImgBK.LoadImage(TEXT(strPath+"BKConfirmGrade.png"));
	m_btnConfirm.LoadRgnImage(TEXT(strPath+"BTConfirmGradeOK.png"),false);

	//移动位置
	HDWP hDwp = BeginDeferWindowPos(32);
		const UINT uFlags=SWP_NOACTIVATE|SWP_NOZORDER|SWP_NOCOPYBITS|SWP_NOSIZE;
		DeferWindowPos(hDwp, m_btnConfirm, NULL,159, 154, 0, 0, uFlags);
	EndDeferWindowPos(hDwp);

	return true;
}
//绘画
void CConfirmGrade::OnPaint()
{
	CPaintDC dc(this);
	CDC memdc;
	CBitmap bmp;

	bmp.CreateCompatibleBitmap(&dc, 400, 200);
	memdc.CreateCompatibleDC(&dc);
	memdc.SelectObject(bmp);

	if (!m_ImgBK.IsNull())
	{
		m_ImgBK.DrawImage(&memdc, 0, 0);
	}

	CRect rect;
	GetClientRect(&rect);
	dc.BitBlt(0, 0, rect.Width(), rect.Height(), &memdc, 0, 0, SRCCOPY);

	bmp.DeleteObject();
	memdc.DeleteDC();
}
//确定
void CConfirmGrade::OnBnClickedConfirm()
{
	OnOK();
}
//当前路径
void CConfirmGrade::GetCurrentDirectoryPath(CString &szPath)
{
	szPath="";
	TCHAR szPathTemp[MAX_PATH]={0};
	GetCurrentDirectory(MAX_PATH, szPathTemp);
	szPath.Append(szPathTemp);
}

//大厅资源路径
void CConfirmGrade::GetImagePlazaPath(CString& strRetValue)
{
	CString strModuleFileName("");	GetCurrentDirectoryPath(strModuleFileName);
	strModuleFileName.Append("\\data\\plaza\\");
	strRetValue = strModuleFileName;
}