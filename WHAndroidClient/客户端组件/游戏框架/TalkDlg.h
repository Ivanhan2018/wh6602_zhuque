#pragma once
#include "Resource.h"
#include "GameFrame.h"
#include "atlimage.h"

// CTalkDlg 对话框


#ifndef IDD_TALK_DLG
#define IDD_TALK_DLG    8081
#endif

class CTalkDlg : public CDialog
{
	DECLARE_DYNAMIC(CTalkDlg)

public:
	CTalkDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CTalkDlg();

	CGDIPlus_Image		m_TalkS;	//聊天框上	
	CGDIPlus_Image		m_TalkX;	//聊天框下	
	CImage				m_ImagePic;//缓冲图
	CGDIPlus_Image		m_ImageExpression;//表情
	CString				m_strTextInfo;//正文

	POINT				m_ptWndPoint;	//窗口位置
	POINT				m_ptSavePoint;	//当前位置

	BYTE				m_btDirectionTag;//方向标志. 1:左上, 2:右上, 3:左下, 4:右下
	BYTE				m_btTalkSkin;	//加载聊天皮肤ID

// 对话框数据
	enum { IDD = IDD_SAY_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	BOOL SetImageRgn(CImage * pImage);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	void SetTalkDlgPos(int iwx, int iwy, int ix, int iy);
	afx_msg void OnPaint();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	CString InterceptString(CString strSource, BYTE &btHavePic);
	void SetTalkText(CString strTalk);
	void SetTalkDlgShow();
	afx_msg void OnTimer(UINT nIDEvent);
	void LoadTalkSkin(BYTE btSkinID=1);
	/// 删除所有计时器
	void OnKillAllTimer();
};
