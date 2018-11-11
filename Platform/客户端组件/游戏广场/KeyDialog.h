#pragma once

#include "stdafx.h"
#include "resource.h"
#include "afxwin.h"
#define KEY_NUM   50
// CKeyDialog 对话框

class CKeyDialog : public CSkinDialogEx
{
	DECLARE_DYNAMIC(CKeyDialog)

public:
	CKeyDialog(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CKeyDialog();

// 对话框数据
	enum { IDD = IDD_KEYDIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()


public:
	CRgnButton    m_KeyBtn[KEY_NUM];			//键
	BOOL       m_bShift;					//Shift 
	BOOL       m_bCapsLoc;					//Caps Lock
	int		   m_nRandKey[26];
	HWND		m_hParent;
	CSkinImage  m_backImage;
	//随机排列
	void	   RandKey(int key[], int count);
	//更新按钮皮肤
	void	   UpdateKeySkin();
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickKey();

public:
	virtual void OnOK();
	virtual void OnCancel();
public:
	afx_msg void OnDestroy();
	afx_msg void OnPaint();
	CRgnButton m_btnClose;
};
