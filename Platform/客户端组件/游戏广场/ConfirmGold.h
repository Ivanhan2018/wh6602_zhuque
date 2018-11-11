#pragma once

#include "resource.h"

// CConfirmGold 对话框

class CConfirmGold : public CDialog
{
	DECLARE_DYNAMIC(CConfirmGold)

public:
	CConfirmGold(CWnd* pParent = NULL);							// 标准构造函数
	virtual ~CConfirmGold();

	// 对话框数据
	enum { IDD = IDD_CONFIRM_GOLD };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);			// DDX/DDV 支持
	virtual BOOL OnInitDialog();								// 初始化

public:
	afx_msg void OnPaint();										// 绘画
	afx_msg void OnBnClickedGetGold();							// 获取宝石按钮
	afx_msg void OnBnClickedConfirm();							// 确定按钮

public:
	void SetMode(WORD wMode);									//设置模型

	DECLARE_MESSAGE_MAP()

private:
	//获取当前路径
	void GetCurrentDirectoryPath(CString &szPath);
	//大厅路径
	void GetImagePlazaPath(CString& strRetValue);

private:
	CRgnButton						m_btnGetGold;				// 获取金币按钮
	CRgnButton						m_btnConfirm;				// 确定按钮
	CRgnButton						m_btnGetGoldAll;			// 获取更多
	CPngImage						m_ImgBK;					// 背景
	WORD							m_wMode;					// 样式

public:
	afx_msg void OnDestroy();

	afx_msg void OnBnClickedConfirmGoldGetall();
};
