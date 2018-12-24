// GameBizCity.cpp : 实现文件
//

#include "stdafx.h"
#include "GameBizCity.h"
#include ".\gamebizcity.h"
#include <afxtempl.h>
#include <afxctl.h>
#include <MsHtmdid.h>
#include "GameFrameNetDefine.h"

// CGameBizCity 对话框




IMPLEMENT_DYNAMIC(CGameBizCity, CDialog)
CGameBizCity::CGameBizCity(CWnd* pParent /*=NULL*/)
	: CDialog(CGameBizCity::IDD, pParent)
{
	m_exMain = NULL;
	EnableAutomation();
	::memset(m_chUserMD5PassWord, 0, sizeof(m_chUserMD5PassWord));
}

CGameBizCity::~CGameBizCity()
{
}

void CGameBizCity::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CGameBizCity, CDialog)
END_MESSAGE_MAP()


// CGameBizCity 消息处理程序

BOOL CGameBizCity::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	//广告控件
	m_exMain=new CGameFrameExmain;
	m_exMain->Create(NULL,NULL,WS_VISIBLE|WS_CHILD,CRect(0,0,0,0),this,20,NULL);
	m_exMain->Navigate2(&CComVariant("about:blank"),NULL,NULL,NULL,NULL);
	::SetWindowPos(m_exMain->m_hWnd, NULL, 0, 0,960, 720, 0);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

//设置网页数据
void CGameBizCity::SetGameBizCity(int iUserID, char* chUserData)
{
	if ((NULL != chUserData) && (NULL != m_exMain))
	{
		char chNetData[256]={0};
		sprintf(chNetData, WEB_MY_SHOP, iUserID, chUserData);
		m_exMain->Navigate2(&CComVariant(chNetData),NULL,NULL,NULL,NULL);
	}
}



BEGIN_DISPATCH_MAP(CGameBizCity, CDialog)
	DISP_FUNCTION_ID(CGameBizCity, "oncontextmenu", DISPID_HTMLDOCUMENTEVENTS2_ONCONTEXTMENU, onHtmlContextMenu, VT_BOOL, VTS_DISPATCH)

	DISP_FUNCTION_ID(CGameBizCity, "oncontextMouseDown", DISPID_HTMLDOCUMENTEVENTS2_ONMOUSEDOWN, onHtmlMouseDown, VT_BOOL, VTS_DISPATCH)


END_DISPATCH_MAP()

BEGIN_EVENTSINK_MAP(CGameBizCity, CDialog)
	ON_EVENT(CGameBizCity, 20, 259, OnDocumentCompleteExplorer1, VTS_DISPATCH VTS_PVARIANT)
END_EVENTSINK_MAP()



BOOL CGameBizCity::onHtmlContextMenu(IHTMLEventObj *pEvtObj)
{ 
	//在成功连接上事件源后,每次用户右击都会调用这个函数,你可以根据pEvtObj来判断当前光标位置等,然后决定是自己弹出菜单,让IE弹出菜单,还是什么都不做...
	return S_OK;
}
BOOL CGameBizCity::onHtmlMouseDown(IHTMLEventObj *pEvtObj)
{
	PostMessage (WM_NCLBUTTONDOWN, HTCAPTION, MAKELPARAM(0,0));
	return S_OK;
}
void CGameBizCity::OnDocumentCompleteExplorer1(LPDISPATCH pDisp, VARIANT FAR* URL) 
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
		//AfxMessageBox("成功挂接上");
	}
}
