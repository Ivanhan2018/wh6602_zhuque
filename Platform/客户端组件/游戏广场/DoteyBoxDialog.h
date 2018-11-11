#pragma once

#include "resource.h"
#include "afxcmn.h"
#include "exmain.h"
// CDoteyBoxDialog 对话框

class CDoteyBoxDialog : public CDialog
{
	CExmain	* m_exMain;
private:
	DECLARE_DYNAMIC(CDoteyBoxDialog)

public:
	CDoteyBoxDialog(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDoteyBoxDialog();

// 对话框数据
	enum { IDD = IDD_DIALOG_DOTEY_BOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();

public:
	DECLARE_DISPATCH_MAP()//声明dispatch map表
	DECLARE_EVENTSINK_MAP()
	BOOL onHtmlContextMenu(IHTMLEventObj *pEvtObj);
	BOOL onHtmlMouseDown(IHTMLEventObj *pEvtObj);
	void OnDocumentCompleteExplorer1(LPDISPATCH pDisp, VARIANT FAR* URL);
	//事件处理函数,原型可以参考MSDN中关于HTMLDocumentEvents2的说明
	DWORD m_dwCookie;
	//用于标记一个连接点
	IHTMLDocument2* pDoc2;
	//想要处理事件的COM对象的指针
	afx_msg void OnSize(UINT nType, int cx, int cy);
public:
	//重置URL
	void SetResetURL();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
};
