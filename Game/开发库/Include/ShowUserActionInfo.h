#pragma once

#include "GameFrame.h"
#include <vector>

using namespace std;

// CUserAction

//总共动画数
#define		TOTAL_FLASH_NUMS					20
#define		MAX_FRAME_NUMS						30

//动画
struct UAPlayFlashInfo 
{
	WORD						wTotalFrame;	//共多少帧
	bool						bIsPlay;		//是否播放
	WORD						wTimeInterval;	//时间间隔

	UAPlayFlashInfo()
	{
		wTotalFrame = 0;
		bIsPlay = 0;
		wTimeInterval = 0;
	}
};


// CShowUserActionInfo

class CShowUserActionInfo : public CWnd
{
	DECLARE_DYNAMIC(CShowUserActionInfo)

public:
	CShowUserActionInfo();
	virtual ~CShowUserActionInfo();

	virtual void OnFinalRelease();

protected:
	DECLARE_MESSAGE_MAP()
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()

public:
	//动画数组初始化
	void InitFlashData();
	//初始化数据
	void InitShowData();



public:
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT nIDEvent);

	//设置动画和文本
	void SetPlayFlash(BYTE byFlashIndex, const char* pSendName, const char* pRecvName);
	void SetFlashText(BYTE byFlashIndex, const char* pSendName, const char* pRecvName);
	
public:
	//过滤颜色
	void AddRectRgn(CDC* pDC,CRgn* rgn,CRect* rect,COLORREF col);
	void UpdateRgn(CDC * pDC);
	bool DrawWindow(CDC * pDC);
	
private:
	//动作信息
	char				m_chActionInfo[1024];
	CSkinImage			m_ImageBack;

	CRect				m_rcTextRect;
	CRect				m_rcImageRect;

	WORD				m_wCurPlayFlashIndex;	//当前帧

	//播放动画数组
	UAPlayFlashInfo				m_PlayFlash[TOTAL_FLASH_NUMS];
	CPngImage					m_vecFlashImageArray[MAX_FRAME_NUMS];
	WORD						m_wCountFlashImageNums;	//

	//文本
	std::string					m_strTextList;
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
};


