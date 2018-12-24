// Active.cpp : 实现文件
//

#include "stdafx.h"
#include "Resource.h"
#include "Active.h"
#include ".\active.h"

#define WEB_BROWER_ACTIVE					WM_USER+1000						// WEB浏览器

// CActive 对话框

IMPLEMENT_DYNAMIC(CActive, CDialog)
CActive::CActive(CWnd* pParent /*=NULL*/)
	: CDialog(CActive::IDD, pParent)
{
	ZeroMemory(m_tchPath, sizeof(m_tchPath));
}

CActive::~CActive()
{
}

void CActive::SetURL(LPCTSTR lpURL)
{
	if (NULL == lpURL)
		return;

	CString	sFileName("");
	CString sFilePath("");
	GetModuleFileName(NULL, sFileName.GetBufferSetLength(MAX_PATH), MAX_PATH);
	sFilePath=sFileName.Left(sFileName.ReverseFind('\\') + 1) + TEXT("Url\\load\\load.html");

	_snprintf(m_tchPath, sizeof(m_tchPath), _T("%s"), lpURL);

	m_webBrower.ShowWindow(SW_SHOW);
	m_webBrower.Navigate(sFilePath,NULL,NULL,NULL,NULL);
	m_webBrower.Navigate(lpURL,NULL,NULL,NULL,NULL);
}

void CActive::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CActive, CDialog)
END_MESSAGE_MAP()


// CActive 消息处理程序

BOOL CActive::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	CRect rcMain;
	AfxGetMainWnd()->GetWindowRect(&rcMain);
	SetWindowPos(&wndTop, rcMain.left+6, rcMain.top+30, rcMain.Width()-12, rcMain.Height()-35, NULL);

	m_webBrower.Create(_T("IE_Active"), NULL, WS_CHILD, CRect(0,0,0,0), this, WEB_BROWER_ACTIVE);
	m_webBrower.SetWindowPos(&wndTop, 0, 0, rcMain.Width()-10, rcMain.Height()-35, NULL);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

BOOL CActive::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if (pMsg->message == WM_KEYDOWN)
	{
		switch (pMsg->wParam)
		{
		case VK_RETURN:
		case VK_ESCAPE:
			return TRUE;
		}
	}

	return CDialog::PreTranslateMessage(pMsg);
}
