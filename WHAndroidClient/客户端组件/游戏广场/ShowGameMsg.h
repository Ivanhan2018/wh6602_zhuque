#pragma once
#include "Stdafx.h"
#include "resource.h"
#include <string>
#include "afxwin.h"


using namespace std;

// CShowGameMsg 对话框
struct GameMsg
{
	BYTE byMsgType;	//消息类型
	std::string strTime;//时间
	std::string strContex;//内容
};

class CShowGameMsg : public CDialog
{
	DECLARE_DYNAMIC(CShowGameMsg)

public:
	CShowGameMsg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CShowGameMsg();

// 对话框数据
	enum { IDD = IDD_SHOWGAMEMSG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);

	//插入消息
	bool InsertGameMsg(const char* pMsgType, const char* pTimeMsg, const char* pContexMsg);
	//清空所有信息
	void ClearAllMsg();

private:
	CDC							m_memdc;
	std::vector<GameMsg>		m_vecMsgList;
	CFont						m_font;


	CPngImage					m_ImageBack;
	CPngImage					m_ImageICO[6];
	CSkinButton					m_BTLookMore;
	CSkinButton					m_BTLookOK;

public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnBnClickedBtExit();
	afx_msg void OnBnClickedBtMore();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);

private:
	//返回消息类型
	BYTE GetMsgType(const char* pMsgType);
};
