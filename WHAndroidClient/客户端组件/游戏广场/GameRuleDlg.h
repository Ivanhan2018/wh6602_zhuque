#pragma once

#include "resource.h"
// CGameRuleDlg 对话框

class CGameRuleDlg : public CDialog
{
	DECLARE_DYNAMIC(CGameRuleDlg)

public:
	CGameRuleDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CGameRuleDlg();

// 对话框数据
	enum { IDD = IDD_GAME_RULE };

private:
	CPngImage				m_ImgBK;					//背景
	CRgnButton				m_btnClose;					//关闭
	CString					m_sRuleUrl;					//规则URL
	CWebBrowser				m_webBrowser;				//规则网页

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

public:
	//设置URL
	void SetRuleUrl(CString sUrl);

private:
	//获取当前路径
	void GetCurrentDirectoryPath(CString &szPath);
	//资源路径
	void GetResPath(CString strClipName, CString& strPath);
	//圆角
	BOOL SetImageRgn(CPngImage * img);

public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnPaint();
	virtual BOOL OnInitDialog();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnDestroy();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
};
