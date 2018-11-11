#pragma once
#include "Resource.h"

// ActionWnd 对话框

class ActionWnd : public CDialog
{
	DECLARE_DYNAMIC(ActionWnd)

public:
	ActionWnd(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~ActionWnd();

// 对话框数据
	enum { IDD = IDD_GAME_FLASH };


public:
	void SetActionImage(CString strImage,bool bIsMove,int TotallFrames, bool bIsAutoHide=true);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

	BOOL ImageFromIDResource(UINT nID,LPCTSTR sTR,Image * &pImg);
	BOOL UpdateClockDisplay(int Transparent=255);

	BLENDFUNCTION m_Blend;
	//CDC m_hdcMemory;		// add by HouGuoJiang 2011-11-25
											// 若多个线程调用了 UpdateClockDisplay 函数，m_hdcMemory 会产生竟争，导致m_hdcMemory=NULL的崩溃
	int m_BakWidth , m_BakHeight ;
	HINSTANCE hFuncInst ;
	typedef BOOL (WINAPI *MYFUNC)(HWND,HDC,POINT*,SIZE*,HDC,POINT*,COLORREF,BLENDFUNCTION*,DWORD);          
	MYFUNC UpdateLayeredWindow;

	int		m_iTransparent;
	bool	m_bIsUpTag;
	DWORD   m_dwToken;
	CString m_strImage;

	//
	int		m_iCountCur;
	int		m_iTotalFrames;

	CPngImage	m_ImageBack;

	bool	m_bIsAutoHide;

	// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnTimer(UINT nIDEvent);


};
