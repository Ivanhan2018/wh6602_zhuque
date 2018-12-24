// TaskDialog.cpp : 实现文件
//

#include "stdafx.h"
#include "TaskDialog.h"
#include <afxtempl.h>
#include <afxctl.h>
#include <MsHtmdid.h>
#include "webpath.h"
#include "GlobalUnits.h"
#include ".\taskdialog.h"

// CTaskDialog 对话框

IMPLEMENT_DYNAMIC(CTaskDialog, CDialog)
CTaskDialog::CTaskDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CTaskDialog::IDD, pParent)
{
	m_exMain = NULL;
	EnableAutomation();//必须有,否则等会用GetIDispatch()时会失败.
}

CTaskDialog::~CTaskDialog()
{
}

void CTaskDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

// CTaskDialog 消息处理程序

BEGIN_MESSAGE_MAP(CTaskDialog, CDialog)
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CTaskDialog, CDialog)
	DISP_FUNCTION_ID(CTaskDialog, "oncontextmenu", DISPID_HTMLDOCUMENTEVENTS2_ONCONTEXTMENU, onHtmlContextMenu, VT_BOOL, VTS_DISPATCH)
	DISP_FUNCTION_ID(CTaskDialog, "oncontextMouseDown", DISPID_HTMLDOCUMENTEVENTS2_ONMOUSEDOWN, onHtmlMouseDown, VT_BOOL, VTS_DISPATCH)
END_DISPATCH_MAP()

BEGIN_EVENTSINK_MAP(CTaskDialog, CDialog)
	ON_EVENT(CTaskDialog, 20, 259, OnDocumentCompleteExplorer1, VTS_DISPATCH VTS_PVARIANT)
END_EVENTSINK_MAP()

// CTaskDialog 消息处理程序

BOOL CTaskDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetWindowLong(this->GetSafeHwnd(), GWL_EXSTYLE, GetWindowLong(this->GetSafeHwnd(), GWL_EXSTYLE)^0x80000);

	// TODO:  在此添加额外的初始化
	m_exMain=new CExmain;
	m_exMain->Create(NULL,NULL,WS_VISIBLE|WS_CHILD,CRect(0,0,0,0),this,20,NULL);	 
	m_exMain->Navigate2(&CComVariant("about:blank"),NULL,NULL,NULL,NULL);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

//重置URL
void CTaskDialog::ResetDlgURL()
{
	if (NULL != m_exMain)
	{
		const tagGlobalUserData & userdata = g_GlobalUnits.GetGolbalUserData();
		CString strUrl;
		//strUrl.Format(WEB_MY_TASK, userdata.dwUserID, userdata.szPassWord);
		//m_exMain->Navigate2(&CComVariant(strUrl),NULL,NULL,NULL,NULL);
		CRect rect;
		GetClientRect(&rect);
		::SetWindowPos(m_exMain->GetSafeHwnd(), 0, 0, 0, rect.Width(), rect.Height(), SWP_SHOWWINDOW);
	}
}
BOOL CTaskDialog::onHtmlContextMenu(IHTMLEventObj *pEvtObj)
{ 
	//在成功连接上事件源后,每次用户右击都会调用这个函数,你可以根据pEvtObj来判断当前光标位置等,然后决定是自己弹出菜单,让IE弹出菜单,还是什么都不做...
	return S_OK;
}
BOOL CTaskDialog::onHtmlMouseDown(IHTMLEventObj *pEvtObj)
{
	//PostMessage (WM_NCLBUTTONDOWN, HTCAPTION, MAKELPARAM(0,0));
	return S_OK;
}
void CTaskDialog::OnDocumentCompleteExplorer1(LPDISPATCH pDisp, VARIANT FAR* URL) 
{
	//处理WebBrowser控件的DocumentComplete事件,并初始化pDoc2指针和连接到事件源
	HRESULT hr=m_exMain->get_Document()->QueryInterface(IID_IHTMLDocument2,(void**)&pDoc2);
	BOOL Ret = AfxConnectionAdvise( 
		pDoc2,  //可连接对象的接口指针
		DIID_HTMLDocumentEvents2, //连接接口ID
		GetIDispatch(FALSE), //把内嵌的IDispatch实现类的一个对象实例m_xDispatch传了出去
		FALSE,   //donod addref
		&m_dwCookie );  //cookie to break connection later...
	if(Ret){
		//		AfxMessageBox("成功挂接上");
	}
}

void CTaskDialog::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
}

BOOL CTaskDialog::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	return TRUE;
	return CDialog::OnEraseBkgnd(pDC);
}

void CTaskDialog::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 在此处添加消息处理程序代码
	// 不为绘图消息调用 CDialog::OnPaint()
}

void CTaskDialog::OnLButtonDown(UINT nFlags, CPoint point)
{

	CDialog::OnLButtonDown(nFlags, point);
}
