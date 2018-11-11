#pragma once
#include "resource.h"

#include "PublicDef.h"

// CGameGiveDJInfo 对话框

class CGameGiveDJInfo : public CDialog
{
	DECLARE_DYNAMIC(CGameGiveDJInfo)

public:
	CGameGiveDJInfo(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CGameGiveDJInfo();

// 对话框数据
	enum { IDD = IDD_DLG_DJ_GIVE_INFO };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

public:
	//过滤颜色
	void AddRectRgn(CDC* pDC,CRgn* rgn,CRect* rect,COLORREF col);
	void UpdateRgn(CDC * pDC);
	bool DrawWindow(CDC * pDC);


	//设置数值
	void SetGiveDJInfo(const char* chGiveInfo);

private:
	CSkinImage							m_ImageBack;
	char								m_chGiveInfo[1024];

public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnPaint();
	virtual BOOL OnInitDialog();
};
