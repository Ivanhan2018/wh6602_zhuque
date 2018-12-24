#pragma once

#include "resource.h"
#include "afxwin.h"

// CPlazaCloseConfirm 对话框

class CPlazaCloseConfirm : public CDialog
{
	DECLARE_DYNAMIC(CPlazaCloseConfirm)

public:
	CPlazaCloseConfirm(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CPlazaCloseConfirm();

// 对话框数据
	enum { IDD = IDD_PLAZA_CLOSE_CONFIRM };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

private:
	CDC							m_memdc;				

private:
	CPngImage					m_imgBK;					// 背景
	CPngImage					m_imgFT;					// 前景(文字)
	BYTE						m_byCloseType;				// 0:退出游戏;1:重新登录
	CRgnButton					m_btnOver;					// 关闭大厅
	CRgnButton					m_btnResetLogon;			// 重新登录
	CRgnButton					m_btnCancel;				// 取消
	CRgnButton					m_btnClose;					// 关闭

public:
	
	afx_msg void OnBnClickedPlazaCloseConfirmOver();

public:
	BYTE GetCloseType();

private:
	//获取当前路径
	void GetCurrentDirectoryPath(CString &szPath);
	//大厅路径
	void GetImagePlazaPath(CString& strRetValue);

public:
	virtual BOOL OnInitDialog();

	afx_msg void OnBnClickedPlazaCloseConfirmResetLogon();
	afx_msg void OnBnClickedPlazaCloseConfirmCancel();
	afx_msg void OnBnClickedPlazaCloseConfirmClose();
	afx_msg void OnPaint();
	afx_msg void OnDestroy();
};
