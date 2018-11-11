#pragma once

#include "Stdafx.h"
#include "Resource.h"
#include "afxwin.h"

// CChat 对话框

class CChat : public CDialog
{
	DECLARE_DYNAMIC(CChat)

public:
	CChat(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CChat();


// 对话框数据
	enum { IDD = IDD_CHAT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
public:
	CComboBox m_ChatInput;
public:
	//初始化
	virtual BOOL OnInitDialog();
	//设置位置
	void SetPos(int cx,int cy);

	DECLARE_MESSAGE_MAP()
	virtual BOOL PreTranslateMessage(MSG* pMsg);
protected:
	virtual void OnOK();
};
