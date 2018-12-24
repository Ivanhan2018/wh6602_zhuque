#pragma once
#include "resource.h" 
#include "exmain.h"
#include "WebPage.h"
#include "GameListManager.h"
#include <MsHTML.h>//定义了IHTMLDocument2等接口
#include <mshtmdid.h>//定义了HTMLDocumentEvents2接口的方法DISPID

// CGameListDlg 对话框

class CGameListDlg : public CDialog
{
public:
	//列表
	CExmain		*				m_exMain;
	IStream*		m_iStream;
	IWebBrowser2*	m_pBrowser;
	CWebPage		m_webPage;
	CExmain		*	m_pExAd;					//广告
	//CRgnButton		m_btnHonurRoll;				//玩家排行 IDC_BT_HONOUR_ROLL
	CRgnButton		m_btnSite;					//官方网站 IDC_BT_SITE
	CRgnButton		m_btnCharge;				//充值中心 IDC_BT_CHARGE
	CRgnButton		m_btnClinet;			    //客户中心 IDC_BT_CLINET
	CRgnButton		m_btnAccountsManage;		//帐号管理 IDC_BT_RE_ACCOUNTS_MANAGE

	CToolTipCtrl	m_ToolTipCtrl;					//提示控件
	CSkinImage		m_ImageBack;
private:
	DECLARE_DYNAMIC(CGameListDlg)

public:
	CGameListDlg(/*CWnd* pParent = NULL*/);   // 标准构造函数
	virtual ~CGameListDlg();

// 对话框数据
	
	enum { IDD = IDD_GAME_LIST };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	virtual void OnPaint();
	virtual void OnCancel(){}
	virtual void OnOK();
	virtual void OnLButtonDown(UINT nFlags, CPoint point);

	//按钮事件
// 	//玩家排行 IDC_BT_HONOUR_ROLL
// 	afx_msg void OnBnClickedHonurRoll();
	//官方网站 IDC_BT_SITE
	afx_msg void OnBnClickedSite();
	//充值中心 IDC_BT_CHARGE
	afx_msg void OnBnClickedCharge();
	//客户中心 IDC_BT_CLINET
	afx_msg void OnBnClickedClinet();
	//帐号管理 IDC_BT_RE_ACCOUNTS_MANAGE
	afx_msg void OnBnClickedAccountsManage();		
	DECLARE_MESSAGE_MAP()

public:
	//重置
	void SetHtml();
	void ShowHtml(CString content);

public:
		CGameListManager * m_gameListManager;



		
public:
	DECLARE_DISPATCH_MAP()//声明dispatch map表
	DECLARE_EVENTSINK_MAP()
	BOOL onHtmlContextMenu(IHTMLEventObj *pEvtObj);
	void OnDocumentCompleteExplorer1(LPDISPATCH pDisp, VARIANT FAR* URL);
	void OnDocumentCompleteExplorer2(LPDISPATCH pDisp, VARIANT FAR* URL);
	//事件处理函数,原型可以参考MSDN中关于HTMLDocumentEvents2的说明
	DWORD m_dwCookie;
	//用于标记一个连接点
	IHTMLDocument2* pDoc2;
	//想要处理事件的COM对象的指针
 	
	virtual BOOL PreTranslateMessage(MSG* pMsg);


	afx_msg BOOL OnEraseBkgnd(CDC * pDC);
};
