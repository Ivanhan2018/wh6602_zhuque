#pragma once

#include "stdafx.h"
#include "resource.h"
#include <string>
#include <vector>

using namespace std;

//游戏公告类
class CGameViewAffiche : public CWnd
{
	DECLARE_DYNAMIC(CGameViewAffiche)

public:
	CGameViewAffiche();
	virtual ~CGameViewAffiche();

public:
	//设置游戏公告
	void SetGameAfficheInfo(char* pInfo);
	//解析公告
	void ParseChar(const char* pData, WORD wDataSize);


protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);

public:
	//设置背景图
	void SetBackImage(const char* pImage, bool bIsRgnWnd=false);


private:
	//BOOL SetImageRgn(CPngImage * pImage);

private:
	CPngImage						m_ImageBack;						//背景
	CFont							m_fontDrawGameInfo;					//窗口字体
	bool							m_bIsDisableGameMsg;				//是否激活游戏公告
	CRect							m_rcGameMsgInfo;					//字体绘制区域
	CRect							m_rcCurMsgInfo;						//当前区域
	std::vector<string>				m_vecGameMsg;						//游戏公告
	int								m_iCountVecIndex;					//当前公告序号
	std::string						m_strSaveGameAffiche;				//保存游戏公告

public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT nIDEvent);
};


