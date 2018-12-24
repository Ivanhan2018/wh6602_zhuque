#pragma once
#include "Resource.h"
#include "MoveFun.h"

// CShowDJInfo 对话框
class CGameClientDJInfo;

class CShowDJInfo : public CDialog
{
	DECLARE_DYNAMIC(CShowDJInfo)

public:
	CShowDJInfo(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CShowDJInfo();

// 对话框数据
	enum { IDD = IDD_SHOWDJINFO };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()


public:
	//显示道具信息窗口
	void SetWndShow(bool bIsShow=true, int iIndex=0);
	afx_msg void OnSize(UINT nType, int cx, int cy);

	//显示道具信息
	void SetShowDJDiscription(int idjindex);

public:
	//过滤颜色
	void AddRectRgn(CDC* pDC,CRgn* rgn,CRect* rect,COLORREF col);
	void UpdateRgn(CDC * pDC);
	bool DrawWindow(CDC * pDC);

private:
	
	CSkinImage							m_ImageBack;
	
	MoveFun<float>						m_MoveInfo;						//移动信息
	CPoint								m_rcSaveOldWnd;					//保存旧窗口位置
	CPoint								m_rcSaveNewWnd;					//保存新窗口位置
	int									m_iCountMoveIndex;				//移动帧
	int									m_iCountMoveTotal;				//移动总帧数
	int									m_iSaveMoveIndex;				//保存旧的移动序号

	int									m_iSaveCurDJID;					//保存当前道具ID


	CGameClientDJInfo					* m_pGameClientDJInfo;

public:
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnTimer(UINT nIDEvent);
};
