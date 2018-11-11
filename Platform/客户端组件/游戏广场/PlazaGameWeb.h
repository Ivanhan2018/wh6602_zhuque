#pragma once

#include "resource.h"
// CPlazaGameWeb 对话框

class CPlazaGameWeb : public CDialog
{
	DECLARE_DYNAMIC(CPlazaGameWeb)

public:
	CPlazaGameWeb(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CPlazaGameWeb();

// 对话框数据
	enum { IDD = IDD_PLAZA_GAME_WEB };

private:

	CWebBrowser				m_WebBrowser;				//浏览嵌套网页

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
	virtual BOOL OnInitDialog();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
private:
	//获取当前路径
	void GetCurrentDirectoryPath(CString &szPath);
	//大厅路径
	void GetImagePlazaPath(CString& strRetValue);
	//资源路径
	void GetResPath(CString strClipName, CString& strPath);

public:
	//创建大厅游戏网页
	bool LoadPlazaGameWeb(UINT uiType);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
