// GameFrameBorder.cpp : 实现文件
//

#include "stdafx.h"
#include "GameFrameBorder.h"
#include ".\gameframeborder.h"
#include "MemDC.h"

// CGameFrameBorder 对话框

IMPLEMENT_DYNAMIC(CGameFrameBorder, CDialog)
CGameFrameBorder::CGameFrameBorder(CWnd* pParent /*=NULL*/)
	: CDialog(CGameFrameBorder::IDD, pParent)
{
}

CGameFrameBorder::~CGameFrameBorder()
{
	m_ImageBack.DestroyImage();
}

void CGameFrameBorder::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CGameFrameBorder, CDialog)
	ON_WM_PAINT()
END_MESSAGE_MAP()


// CGameFrameBorder 消息处理程序

BOOL CGameFrameBorder::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化

	TCHAR szResourcePath[MAX_PATH];	GetCurrentDirectory(MAX_PATH,szResourcePath);
	CString strPath(szResourcePath);
	strPath.Append("\\data\\gameframe\\plaza\\");

	//背景
	m_ImageBack.LoadImage(TEXT(strPath + "GameFrameBorder.png"));

	SetImageRgn(m_hWnd, &m_ImageBack, SIRT_SOLID_DOT_LESS);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CGameFrameBorder::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 在此处添加消息处理程序代码
	// 不为绘图消息调用 CDialog::OnPaint()

	CRect rect;
	GetClientRect(&rect);
	int iWidth = rect.Width();
	int iHeight = rect.Height();

	GetClientRect(&rect);
	CMemDC dcMem(&dc, rect, true);	// 用单色填充背景

	dcMem.SetBkMode(TRANSPARENT);

	if (!m_ImageBack.IsNull())
	{
		m_ImageBack.DrawImage(&dcMem, 0, 0);
	}
}
