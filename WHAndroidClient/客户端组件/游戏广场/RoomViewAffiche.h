#pragma once

#include "stdafx.h"
#include "resource.h"
// CRoomViewAffiche

//桌子列表游戏公告类
class CRoomViewAffiche : public CWnd
{
	DECLARE_DYNAMIC(CRoomViewAffiche)

public:
	CRoomViewAffiche();
	virtual ~CRoomViewAffiche();

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

private:
	CSkinImage						m_ImageBack;						//背景
	CFont							m_fontDrawGameInfo;					//窗口字体
	CDC								m_memdc;							//窗口句柄
	bool							m_bIsDisableGameMsg;				//是否激活游戏公告
	CRect							m_rcGameMsgInfo;					//字体绘制区域
	CRect							m_rcCurMsgInfo;						//当前区域
	std::vector<string>				m_vecGameMsg;						//游戏公告
	int								m_iCountVecIndex;					//当前公告序号
	std::string						m_strSaveGameAffiche;				//保存游戏公告

private:
	//获取当前路径
	void GetCurrentDirectoryPath(CString &szPath);
	//大厅路径
	void GetImagePlazaPath(CString& strRetValue);

public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT nIDEvent);
};


