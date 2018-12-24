#pragma once

#include "GameFrame.h"
#include "GameFrameDlg.h"
#include "GameClientDJInfo.h"
#include <vector>


using namespace std;
// CUserDaoJu

#define		IDC_BT_USERDJ_LEFT				WM_USER+10000
#define		IDC_BT_USERDJ_RIGHT				WM_USER+10001
#define		IDC_BT_USERDJ_SET				WM_USER+10002
#define		IDC_BT_USERDJ_HELP				WM_USER+10003
#define		IDC_BT_USERDJ_CITY				WM_USER+10004
#define		IDC_BT_USERDJ_TASK				WM_USER+10005
#define		IDC_BT_USERDJ_ACTIVE			WM_USER+10006

class CShowDJInfo;

class CUserDaoJu : public CWnd
{
	DECLARE_DYNAMIC(CUserDaoJu)

public:
	CUserDaoJu(CGameFrameDlg * pGameFrameDlg);
	virtual ~CUserDaoJu();


	//消息映射
protected:
	//绘画函数
	VOID OnPaint();
	//创建消息
	INT OnCreate(LPCREATESTRUCT lpCreateStruct);

protected:
	CGameFrameDlg				* m_pGameFrameDlg;				//对话框
	CGameClientDJInfo			* m_pUserDJInfo;				//道具信息
	CShowDJInfo					* m_pShowDJInfo;				//显示道具信息
	bool						m_bIsTimer;

private:
	CToolTipCtrl	m_ToolTipCtrl;					//提示控件
	CImage			m_DaoJuImageBuffer;					//缓冲位图
	CPngImage		m_ImageWndBack;						//窗口背景

	CSkinButton		m_BTLeft;							//左
	CSkinButton		m_BTRight;							//右

	bool			m_bHovering;

	//翻页判断
	int				m_iTotalPage;			//总页数
	int				m_iCurrentPage;			//当前页
	//当前光标的位置
	int				m_iCurrentMousePoint;	//当前鼠标位置

private:
	//按钮事件
	void OnBnLeftClickedEvent();
	void OnBnRightClickedEvent();

	//BOOL SetImageRgn(CPngImage * pImage);

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTimer(UINT nIDEvent);
	//艺术字体
	void DrawTextString(CDC * pDC, LPCTSTR pszString, COLORREF crText, COLORREF crFrame, int nXPos, int nYPos);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
//	afx_msg void OnNcMouseMove(UINT nHitTest, CPoint point);
	//鼠标离开消息
	LRESULT OnMouseLeave(WPARAM wparam, LPARAM lparam);


	//删除道具
	bool DelUserUseDJFromeIndex(int iIndex);
	//更新道具信息
	void UpdateUserDJPage();
	//重置道具页面
	void UpdateUserDJInfo(int iIndex);

	//设置显示道具信息指针
	void SetShowDJInfo(CShowDJInfo* pShowDJInfoPtr);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};


