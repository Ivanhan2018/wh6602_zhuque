#pragma once


// CActive 对话框

class CActive : public CDialog
{
	DECLARE_DYNAMIC(CActive)

public:
	CActive(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CActive();
	/// 设置地址
	void SetURL(LPCTSTR lpURL);

// 对话框数据
	enum { IDD = IDD_ACTIVE };

private:
	CWebBrowser							m_webBrower;								///< 浏览器
	TCHAR										m_tchPath[MAX_PATH];				///< 访问地址

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
