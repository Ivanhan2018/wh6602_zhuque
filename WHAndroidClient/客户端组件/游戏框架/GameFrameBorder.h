#pragma once

#include "Resource.h"

// CGameFrameBorder 对话框

class CGameFrameBorder : public CDialog
{
	DECLARE_DYNAMIC(CGameFrameBorder)

public:
	CGameFrameBorder(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CGameFrameBorder();

// 对话框数据
	enum { IDD = IDD_DIALOG8 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

private:
	CPngImage		m_ImageBack;		//背景

public:
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
};
