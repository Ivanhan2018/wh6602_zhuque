// PlazaCloseConfirm.cpp : 实现文件
//

#include "stdafx.h"
#include "PlazaCloseConfirm.h"
#include ".\plazacloseconfirm.h"
#include "PlazaDlg.h"


// CPlazaCloseConfirm 对话框

IMPLEMENT_DYNAMIC(CPlazaCloseConfirm, CDialog)
CPlazaCloseConfirm::CPlazaCloseConfirm(CWnd* pParent /*=NULL*/)
	: CDialog(CPlazaCloseConfirm::IDD, pParent)
{
	m_byCloseType = 0;
}

CPlazaCloseConfirm::~CPlazaCloseConfirm()
{
	m_memdc.DeleteDC();
	m_imgBK.DestroyImage();
	m_imgFT.DestroyImage();
}

void CPlazaCloseConfirm::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PLAZA_CLOSE_CONFIRM_OVER, m_btnOver);
	DDX_Control(pDX, IDC_PLAZA_CLOSE_CONFIRM_RESET_LOGON, m_btnResetLogon);
	DDX_Control(pDX, IDC_PLAZA_CLOSE_CONFIRM_CANCEL, m_btnCancel);
	DDX_Control(pDX, IDC_PLAZA_CLOSE_CONFIRM_CLOSE, m_btnClose);
}


BEGIN_MESSAGE_MAP(CPlazaCloseConfirm, CDialog)
	ON_BN_CLICKED(IDC_PLAZA_CLOSE_CONFIRM_OVER, OnBnClickedPlazaCloseConfirmOver)
	ON_BN_CLICKED(IDC_PLAZA_CLOSE_CONFIRM_RESET_LOGON, OnBnClickedPlazaCloseConfirmResetLogon)
	ON_BN_CLICKED(IDC_PLAZA_CLOSE_CONFIRM_CANCEL, OnBnClickedPlazaCloseConfirmCancel)
	ON_BN_CLICKED(IDC_PLAZA_CLOSE_CONFIRM_CLOSE, OnBnClickedPlazaCloseConfirmClose)
	ON_WM_PAINT()
	ON_WM_DESTROY()
END_MESSAGE_MAP()

//初始化
BOOL CPlazaCloseConfirm::OnInitDialog()
{
	CDialog::OnInitDialog();
	int width   =   GetSystemMetrics(SM_CXSCREEN); 
	int height   =   GetSystemMetrics(SM_CYSCREEN);
	//调整窗口大小
	SetWindowPos(NULL, (width-366)/2, (height-366)/2, 400, 200, false);

	m_byCloseType = 0;

	CString strPath("");
	GetImagePlazaPath(strPath);
	m_btnOver.LoadRgnImage(TEXT(strPath+"BTCloseConfirmOver.png"),false);
	m_btnResetLogon.LoadRgnImage(TEXT(strPath+"BTCloseConfirmResetLogon.png"),false);
	m_btnCancel.LoadRgnImage(TEXT(strPath+"BTCloseConfirmCancel.png"),false);
	m_btnClose.LoadRgnImage(TEXT(strPath+"BTCloseConfirmClose.png"),false);
	if (m_imgBK.IsNull()) m_imgBK.LoadImage(TEXT(strPath+"BKCloseConfirm.png"));

	int iRand=rand()%3;
	if (!m_imgBK.IsNull()) m_imgFT.DestroyImage();
	if (0==iRand){
		m_imgFT.LoadImage(TEXT(strPath+"FTCloseConfirmFont1.png"));
	}else if (1==iRand){
		m_imgFT.LoadImage(TEXT(strPath+"FTCloseConfirmFont2.png"));
	}else{
		m_imgFT.LoadImage(TEXT(strPath+"FTCloseConfirmFont3.png"));
	}

	HDWP hDwp=BeginDeferWindowPos(32);
	const UINT uFlags=SWP_NOACTIVATE|SWP_NOZORDER|SWP_NOCOPYBITS|SWP_NOSIZE;
	DeferWindowPos(hDwp,m_btnResetLogon,NULL,44,154,0,0,uFlags);
	DeferWindowPos(hDwp,m_btnOver,NULL,159,154,0,0,uFlags);
	DeferWindowPos(hDwp,m_btnCancel,NULL,275,154,0,0,uFlags);
	DeferWindowPos(hDwp,m_btnClose,NULL,360,5,0,0,uFlags);
	EndDeferWindowPos(hDwp);

	

	CPaintDC dc(this);
	m_memdc.DeleteDC();
	CBitmap bmp;
	bmp.CreateCompatibleBitmap(&dc,400,200);
	m_memdc.CreateCompatibleDC(&dc);
	m_memdc.SelectObject(bmp);
	bmp.DeleteObject();
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}
void CPlazaCloseConfirm::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	
	//背景
	if (!m_imgBK.IsNull())
	{
		m_imgBK.DrawImage(&m_memdc,0,0);
	}
	//前景(提示文字)
	if (!m_imgFT.IsNull())
	{
		m_imgFT.DrawImage(&m_memdc,91,103,m_imgFT.GetWidth(),m_imgFT.GetHeight(),0,0);
	}
	/*
	//提示文字
	CRect rectFont;
	rectFont.left=100;
	rectFont.top=61;
	rectFont.right=rectFont.left+209;
	rectFont.bottom=rectFont.top+46;
	CFont fontCur;
	fontCur.CreateFont(12,0,0,0,200,0,0,0,134,3,2,1,2,TEXT("宋体"));
	CFont * fontOld=m_memdc.SelectObject(&fontCur);
	m_memdc.SetTextColor(RGB(255,0,0));
	m_memdc.SetBkColor(RGB(255,255,0));
	m_memdc.DrawText(m_chClue, &rectFont, DT_CENTER|DT_VCENTER|DT_WORDBREAK|DT_END_ELLIPSIS);
	m_memdc.SelectObject(fontOld);
	*/
	CRect rect;
	GetClientRect(&rect);
	dc.BitBlt(0, 0, rect.Width(), rect.Height(), &m_memdc, 0, 0, SRCCOPY);
}

//关闭大厅
void CPlazaCloseConfirm::OnBnClickedPlazaCloseConfirmOver()
{
	m_byCloseType = 0;
	OnOK();
}
//重新登录
void CPlazaCloseConfirm::OnBnClickedPlazaCloseConfirmResetLogon()
{
	m_byCloseType = 1;//
	OnOK();
}
//关闭本窗口
void CPlazaCloseConfirm::OnBnClickedPlazaCloseConfirmCancel()
{
	m_byCloseType=-1;
	OnOK();
}
//关闭本窗口
void CPlazaCloseConfirm::OnBnClickedPlazaCloseConfirmClose()
{
	m_byCloseType=-1;
	OnOK();
}

//关闭类型
BYTE CPlazaCloseConfirm::GetCloseType()
{
	return m_byCloseType;
}
//当前路径
void CPlazaCloseConfirm::GetCurrentDirectoryPath(CString &szPath)
{
	szPath="";
	TCHAR szPathTemp[MAX_PATH]={0};
	GetCurrentDirectory(MAX_PATH, szPathTemp);
	szPath.Append(szPathTemp);
}

//大厅资源路径
void CPlazaCloseConfirm::GetImagePlazaPath(CString& strRetValue)
{
	CString strModuleFileName("");	GetCurrentDirectoryPath(strModuleFileName);
	strModuleFileName.Append("\\data\\plaza\\");
	strRetValue = strModuleFileName;
}

void CPlazaCloseConfirm::OnDestroy()
{
	CDialog::OnDestroy();

	m_memdc.DeleteDC();
	m_imgBK.DestroyImage();
	m_imgFT.DestroyImage();
}
