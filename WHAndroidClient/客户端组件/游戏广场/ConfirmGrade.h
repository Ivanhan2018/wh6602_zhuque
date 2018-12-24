#pragma once

#include "resource.h"

class CConfirmGrade : public CDialog
{
	DECLARE_DYNAMIC(CConfirmGrade)

public:
	CConfirmGrade(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CConfirmGrade();

// 对话框数据
	enum { IDD = IDD_CONFIRM_GRADE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();						// 初始化
	afx_msg void OnPaint();								// 绘画

private: 
	void OnBnClickedConfirm();							//确认
	void GetCurrentDirectoryPath(CString &szPath);		//获取当前路径
	void GetImagePlazaPath(CString& strRetValue);		//大厅路径

	DECLARE_MESSAGE_MAP()

private:
	CRgnButton					m_btnConfirm;			//确定
	CPngImage					m_ImgBK;				//背景

};
