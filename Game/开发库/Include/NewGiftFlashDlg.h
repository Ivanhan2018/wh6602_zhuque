#pragma once

#include "Resource.h"
#include "GlobalFrame.h"

// CNewGiftFlashDlg 对话框

class CNewGiftFlashDlg : public CDialog
{
	DECLARE_DYNAMIC(CNewGiftFlashDlg)

public:
	CNewGiftFlashDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CNewGiftFlashDlg();

// 对话框数据
	enum { IDD = IDD_NEWGIFT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

private:
	CMD_GF_NEW_GIFT						m_tagNewGift;						//新手礼包
	CPngImage							m_ImgBK;							// 背景
	CPngImage							m_imgFireworks;						// 烟花
	CPngImage							m_imgUpgrade;						// 宝石
	int									m_iUpgradeCount;					// 宝石帧数
	int									m_iFrameCount;						// 帧数
	BOOL								m_bShowVal;							// 显示的值
	CImageNums							m_imgnumVal;						// 显示的值

public:
	//新手礼包	Author<cxf>; DateTime<2012-01-16>
	void InitDataRes(CMD_GF_NEW_GIFT * pNewGift);
	//设置显示的值
	void SetNum(int iVal);

private:
	//获取当前路径
	void GetCurrentDirectoryPath(CString &szPath);
	//大厅路径
	void GetImagePlazaPath(CString& strRetValue);

public:
	afx_msg void OnPaint();
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnClose();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
};
