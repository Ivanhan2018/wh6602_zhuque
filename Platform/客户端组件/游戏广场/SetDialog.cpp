// SetDialog.cpp : 实现文件
//

#include "stdafx.h"
#include "PlazaDlg.h"
#include "SetDialog.h"
#include ".\setdialog.h"


// CSetDialog 对话框

IMPLEMENT_DYNAMIC(CSetDialog, CSkinDialogEx)
CSetDialog::CSetDialog(CWnd* pParent /*=NULL*/)
	: CSkinDialogEx(CSetDialog::IDD, pParent)
{
}

CSetDialog::~CSetDialog()
{
}

void CSetDialog::DoDataExchange(CDataExchange* pDX)
{
	CSkinDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDOK, m_btnOk);
	//DDX_Control(pDX, IDCANCEL, m_btnCancel);
	//DDX_Control(pDX, IDC_BTN_RESUME, m_btnResume);
	//DDX_Control(pDX, IDC_BTN_CLOSE, m_btnClose);
	DDX_Control(pDX, IDC_COMBO1, m_combo);
	//DDX_Control(pDX, IDC_SLIDER1, m_slider1);
	//DDX_Control(pDX, IDC_SLIDER2, m_slider2);
	//DDX_Control(pDX, IDC_SLIDER3, m_slider3);
	//DDX_Control(pDX, IDC_SLIDER4, m_slider4);
}


BEGIN_MESSAGE_MAP(CSetDialog, CSkinDialogEx)
// 	ON_BN_CLICKED(IDC_RADIO2, OnBnClickedRadio2)
// 	ON_BN_CLICKED(IDC_RADIO1, OnBnClickedRadio1)
//	ON_BN_CLICKED(IDC_BTN_CLOSE, OnBnClickedBtnClose)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_BTN_RESUME, OnBnClickedBtnResume)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_CHECK_BACK_MUSIC, OnBnClickedCheckBackMusic)
	ON_BN_CLICKED(IDC_CHECK_GAME_MUSIC, OnBnClickedCheckGameMusic)
	ON_BN_CLICKED(IDC_CHECK_MAN_MUSIC, OnBnClickedCheckManMusic)
	ON_BN_CLICKED(IDC_CHECK_CARTOON_MUSIC, OnBnClickedCheckCartoonMusic)
	ON_WM_DESTROY()
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// CSetDialog 消息处理程序

BOOL CSetDialog::OnInitDialog()
{
	CSkinDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化

	TCHAR ch[][64] = { TEXT("普通话"),
					   TEXT("长沙话"),
					   TEXT("益阳话") };
	for (WORD i=0; i<CountArray(ch); i++)
        m_combo.AddString(ch[i]);
	Init();

	HINSTANCE hInstance=AfxGetInstanceHandle();
	m_btnOk.LoadRgnImage(hInstance, IDB_SET_DLG_OK);
	//m_btnCancel.LoadRgnImage(hInstance, IDB_SET_DLG_CANCEL);
	//m_btnResume.LoadRgnImage(hInstance, IDB_SET_DLG_RESUME);
	//m_btnClose.LoadRgnImage(hInstance, IDB_KEY_CLOSE);


	SetWindowPos(NULL, 0, 0, 343, 225, SWP_NOMOVE);

	HDWP hDwp=BeginDeferWindowPos(32);
	const UINT uFlags=SWP_NOACTIVATE|SWP_NOZORDER|SWP_NOCOPYBITS|SWP_NOSIZE;
	DeferWindowPos(hDwp,m_btnOk,NULL,120,160,0,0,uFlags);
	//DeferWindowPos(hDwp,m_btnCancel,NULL,373,249,0,0,uFlags);
	//DeferWindowPos(hDwp,m_btnResume,NULL,66,249,0,0,uFlags);
	//DeferWindowPos(hDwp,m_btnClose,NULL,518,6,0,0,uFlags);
	DeferWindowPos(hDwp,*GetDlgItem(IDC_CHECK_BACK_MUSIC),NULL,110,75,0,0,uFlags);
	//DeferWindowPos(hDwp,*GetDlgItem(IDC_CHECK_GAME_MUSIC),NULL,116,100,0,0,uFlags);
	//DeferWindowPos(hDwp,*GetDlgItem(IDC_CHECK_MAN_MUSIC),NULL,116,136,0,0,uFlags);
	//DeferWindowPos(hDwp,*GetDlgItem(IDC_CHECK_CARTOON_MUSIC),NULL,116,172,0,0,uFlags);
	DeferWindowPos(hDwp,m_combo,NULL,110,120,0,0,uFlags);
// 	DeferWindowPos(hDwp,*GetDlgItem(IDC_RADIO1),NULL,114,165,0,0,uFlags);
// 	DeferWindowPos(hDwp,*GetDlgItem(IDC_RADIO2),NULL,194,165,0,0,uFlags);
	//DeferWindowPos(hDwp,m_slider1,NULL,160,58,0,0,uFlags);
	//DeferWindowPos(hDwp,m_slider2,NULL,160,94,0,0,uFlags);
	//DeferWindowPos(hDwp,m_slider3,NULL,160,131,0,0,uFlags);
	//DeferWindowPos(hDwp,m_slider4,NULL,160,168,0,0,uFlags);

	EndDeferWindowPos(hDwp);

	SetNCRect(301);

	m_ctrBrush.CreateSolidBrush(RGB(255, 254, 232));

	//SetTimer(1, 10, NULL);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

// void CSetDialog::OnBnClickedRadio2()
// {
// 	// TODO: 在此添加控件通知处理程序代码
// 	((CButton*)GetDlgItem(IDC_RADIO1))->SetCheck(0);
// 	((CButton*)GetDlgItem(IDC_RADIO2))->SetCheck(1);
// 	m_combo.ShowWindow(SW_SHOW);
// }
// 
// void CSetDialog::OnBnClickedRadio1()
// {
// 	// TODO: 在此添加控件通知处理程序代码
// 	((CButton*)GetDlgItem(IDC_RADIO1))->SetCheck(1);
// 	((CButton*)GetDlgItem(IDC_RADIO2))->SetCheck(0);
// 	m_combo.ShowWindow(SW_HIDE);
// }

void CSetDialog::OnBnClickedBtnClose()
{
	// TODO: 在此添加控件通知处理程序代码
	m_ctrBrush.DeleteObject();
	OnCancel();
}

void CSetDialog::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 在此处添加消息处理程序代码
	// 不为绘图消息调用 CSkinDialog::OnPaint()

	CRect rect;
	GetClientRect(&rect);
	DrawPic(&dc,IDB_SET_BACK,&rect);
}


void CSetDialog::Init()
{
	//m_slider1.SetRange(0, 100);
	//m_slider2.SetRange(0, 100);
	//m_slider3.SetRange(0, 100);
	//m_slider4.SetRange(0, 100);

	//m_slider1.SetPos(g_GolbalUserSet.m_nBackMusic);
	//m_slider2.SetPos(g_GolbalUserSet.m_nGameMusic);
	//m_slider3.SetPos(g_GolbalUserSet.m_nManMusic);
	//m_slider4.SetPos(g_GolbalUserSet.m_nCartoonMusic);

	if (g_GolbalUserSet.m_nBackMusic > 0)
	{
		//((CButton*)GetDlgItem(IDC_CHECK_BACK_MUSIC))->SetWindowText(TEXT("已开启"));
		((CButton*)GetDlgItem(IDC_CHECK_BACK_MUSIC))->SetCheck(1);
	}
	else
	{
		//((CButton*)GetDlgItem(IDC_CHECK_BACK_MUSIC))->SetWindowText(TEXT("已关闭"));
		((CButton*)GetDlgItem(IDC_CHECK_BACK_MUSIC))->SetCheck(0);
	}
	
	//((CButton*)GetDlgItem(IDC_CHECK_GAME_MUSIC))->SetCheck(g_GolbalUserSet.m_nGameMusic > 0);
	//((CButton*)GetDlgItem(IDC_CHECK_MAN_MUSIC))->SetCheck(g_GolbalUserSet.m_nCartoonMusic > 0);
	//((CButton*)GetDlgItem(IDC_CHECK_CARTOON_MUSIC))->SetCheck(g_GolbalUserSet.m_nCartoonMusic > 0);

	int nSel = g_GolbalUserSet.m_nLanguage;
	m_combo.SetCurSel(g_GolbalUserSet.m_nLanguage);
}
void CSetDialog::OnBnClickedBtnResume()
{
	// TODO: 在此添加控件通知处理程序代码
	Init();
}

void CSetDialog::OnOK()
{
	// TODO: 在此添加专用代码和/或调用基类
	//g_GolbalUserSet.m_nBackMusic = m_slider1.GetPos();
	//g_GolbalUserSet.m_nGameMusic = m_slider2.GetPos();
	//g_GolbalUserSet.m_nManMusic = m_slider3.GetPos();
	//g_GolbalUserSet.m_nCartoonMusic = m_slider4.GetPos();
	//g_GolbalUserSet.m_nLanguage = m_combo.GetCurSel();
	//CPlazaDlg* pParent = (CPlazaDlg*)GetParent();
	//if (NULL != pParent)
	//{
	//	int iSel = ((CButton*)GetDlgItem(IDC_CHECK_BACK_MUSIC))->GetCheck();
	//	if (0 == iSel)
	//	{//关闭
	//		g_GolbalUserSet.m_nBackMusic = 0;
	//		pParent->SetMusicSound(false);
	//	}
	//	else
	//	{//开启
	//		g_GolbalUserSet.m_nBackMusic = 1;
	//		pParent->SetMusicSound(true);
	//	}
	//}
	//
	g_GolbalUserSet.m_nLanguage = m_combo.GetCurSel();
	g_GolbalUserSet.UnitGlobal();//保存在注册表里
	CSkinDialogEx::OnOK();
}

void CSetDialog::OnTimer(UINT nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	//if (::IsWindow(m_slider1.GetSafeHwnd())  )
	//{
	//	((CButton*)GetDlgItem(IDC_CHECK_BACK_MUSIC))->SetCheck(m_slider1.GetPos() > 0 ? 1 : 0);
	//}

	//if (::IsWindow(m_slider2.GetSafeHwnd())  )
	//{
	//	((CButton*)GetDlgItem(IDC_CHECK_GAME_MUSIC))->SetCheck(m_slider2.GetPos() > 0 ? 1 : 0);
	//}

	//if (::IsWindow(m_slider3.GetSafeHwnd())  )
	//{
	//	((CButton*)GetDlgItem(IDC_CHECK_MAN_MUSIC))->SetCheck(m_slider3.GetPos() > 0 ? 1 : 0);
	//}

	//if (::IsWindow(m_slider4.GetSafeHwnd())  )
	//{
	//	((CButton*)GetDlgItem(IDC_CHECK_CARTOON_MUSIC))->SetCheck(m_slider4.GetPos() > 0 ? 1 : 0);
	//}

	CSkinDialogEx::OnTimer(nIDEvent);
}

void CSetDialog::OnBnClickedCheckBackMusic()
{
	// TODO: 在此添加控件通知处理程序代码

	//if ( 1 == ((CButton*)GetDlgItem(IDC_CHECK_BACK_MUSIC))->GetCheck())
	//{
	//	m_slider1.SetPos(g_GolbalUserSet.m_nBackMusic);
	//}else
	//{
	//	m_slider1.SetPos(0);
	//}
	CPlazaDlg* pParent = (CPlazaDlg*)GetParent();
	if (NULL != pParent)
	{
		int iSel = ((CButton*)GetDlgItem(IDC_CHECK_BACK_MUSIC))->GetCheck();
		if (0 == iSel)
		{//关闭
			g_GolbalUserSet.m_nBackMusic = 0;
			pParent->SetMusicSound(false);
		}
		else
		{//开启
			g_GolbalUserSet.m_nBackMusic = 1;
			pParent->SetMusicSound(true);
		}
	}
	g_GolbalUserSet.UnitGlobal();//保存在注册表里
}

void CSetDialog::OnBnClickedCheckGameMusic()
{
	// TODO: 在此添加控件通知处理程序代码
	//if ( 1 == ((CButton*)GetDlgItem(IDC_CHECK_GAME_MUSIC))->GetCheck())
	//{
	//	m_slider2.SetPos(g_GolbalUserSet.m_nGameMusic);
	//}else
	//{
	//	m_slider2.SetPos(0);
	//}
}

void CSetDialog::OnBnClickedCheckManMusic()
{
	// TODO: 在此添加控件通知处理程序代码
	//if ( 1 == ((CButton*)GetDlgItem(IDC_CHECK_MAN_MUSIC))->GetCheck())
	//{
	//	m_slider3.SetPos(g_GolbalUserSet.m_nManMusic);
	//}else
	//{
	//	m_slider3.SetPos(0);
	//}
}

void CSetDialog::OnBnClickedCheckCartoonMusic()
{
	// TODO: 在此添加控件通知处理程序代码
	//if ( 1 == ((CButton*)GetDlgItem(IDC_CHECK_CARTOON_MUSIC))->GetCheck())
	//{
	//	m_slider4.SetPos(g_GolbalUserSet.m_nCartoonMusic);
	//}else
	//{
	//	m_slider4.SetPos(0);
	//}
}

void CSetDialog::OnDestroy()
{
	//KillTimer(1);
	CSkinDialogEx::OnDestroy();

	// TODO: 在此处添加消息处理程序代码
}

HBRUSH CSetDialog::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{

	switch (nCtlColor)
	{
	case CTLCOLOR_DLG:
	case CTLCOLOR_STATIC:
		{
			pDC->SetBkMode(TRANSPARENT);
			pDC->SetBkColor(COLORREF(RGB(255, 254, 232)));
			pDC->SetTextColor(CSkinDialog::m_SkinAttribute.m_crControlTXColor);
			return m_ctrBrush;
		}
	}

	HBRUSH hbr = CSkinDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  在此更改 DC 的任何属性

	// TODO:  如果默认的不是所需画笔，则返回另一个画笔
	return hbr;
}
