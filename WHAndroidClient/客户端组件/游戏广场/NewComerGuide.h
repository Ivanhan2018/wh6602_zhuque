#pragma once
#include "resource.h"

// CNewComerGuide 对话框

class CNewComerGuide : public CDialog
{
	DECLARE_DYNAMIC(CNewComerGuide)

public:
	CNewComerGuide(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CNewComerGuide();

// 对话框数据
	enum { IDD = IDD_NEWCOMER_GUIDE };

private:
	//CWebBrowser					m_webNewComerGuide;						// 新手指引(网页)
	CPngImage					m_ImgNewComerGuide;						// 背景
	CPngImage					m_ImgTag;								// 选择标志
	CPngImage					m_ImgNewComerGift;						// 新手礼包图片
	CPngImage					m_ImgGameTask;							// 游戏任务
	CPngImage					m_ImgGameCurrency;						// 游戏货币
	CPngImage					m_ImgPropsShop;							// 商城
	CPngImage					m_ImgPrizeExcharge;						// 兑换实物
	CPngImage					m_ImgGameHelp;							// 帮助
	CSkinButton					m_btnClose;								// 关闭
	CSkinButton					m_btnNewComerGift;						// 新手礼包
	CSkinButton					m_btnGameTask;							// 游戏任务
	CSkinButton					m_btnGameCurrency;						// 游戏货币
	CSkinButton					m_btnPropsShop;							// 商城
	CSkinButton					m_btnPrizeExcharge;						// 兑换实物
	CSkinButton					m_btnGameHelp;							// 帮助

	CRect						m_rcTagPos;								// 标志位置
	WORD						m_wTagPos;								// 点击按钮

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	//初始化
	virtual BOOL OnInitDialog();

private:
	//获取当前路径
	void GetCurrentDirectoryPath(CString &szPath);
	//资源路径
	void GetResPath(CString strClipName, CString& strPath);
	//转向
	void ExplainSwerve(CDC &memdc);
	//圆角
	BOOL SetImageRgn(CPngImage * img);
public:
	afx_msg void OnDestroy();
	afx_msg void OnPaint();
	afx_msg void OnBnClickedNewcomerGift();
	afx_msg void OnBnClickedNewcomerGametask();
	afx_msg void OnBnClickedNewcomerGamecurrency();
	afx_msg void OnBnClickedNewcomerPropsshop();
	afx_msg void OnBnClickedNewcomerPrizeexcharge();
	afx_msg void OnBnClickedNewcomerGamehelp();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnBnClickedCancel();
};
