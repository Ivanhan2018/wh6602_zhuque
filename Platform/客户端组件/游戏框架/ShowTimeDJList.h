#pragma once
#include "Resource.h"
#include "GameClientDJInfo.h"
#include <vector>

//总道具数
#define		GAME_DJ_TOTAL_NUMS			PROPERTY_COUNT

//道具信息
struct tag_dj_time_info
{
	int		idjID;		//道具ID
	int		iICOIndex;	//ICO
	unsigned int uiTime;//剩余时间(以秒为单位)
};

using namespace std;

// CShowTimeDJList 对话框

class CShowTimeDJList : public CDialog
{
	DECLARE_DYNAMIC(CShowTimeDJList)

public:
	CShowTimeDJList(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CShowTimeDJList();

// 对话框数据
	enum { IDD = IDD_SHOWTIMEDJLIST };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

public:
	//动态获得窗口宽与高
	CSize GetDlgWH(void);
	//重置窗口大小
	void SetResetWndSize(void);

	CPngImage							m_ImageWndTop;			//上
	CPngImage							m_ImageWndCenter;		//中
	CPngImage							m_ImageWndDown;			//下
	CPngImage							m_ImageDJBackFrame;		//道具背景框
	CPngImage							m_ImageDJTimeNums;		//道具时间数字
	CPngImage							m_ImageNoMinute;		//不足一分钟图片
	
	bool								m_bIsDisableTime;		//激活时间
	CGameClientDJInfo					* m_pUserDJInfo;		//道具信息
public:
	virtual BOOL OnInitDialog();
//	afx_msg void OnNcMouseMove(UINT nHitTest, CPoint point);
	afx_msg void OnPaint();
	//绘制背景
	void DrawWndBack(CDC *pDC);
	//绘制道具时间
	void DrawDJTimeItem(CDC *pDC, int iIndex, int iX, int iY);

	//显示窗口
	void SetShowWnd(bool bShow=true);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnTimer(UINT nIDEvent);
	
	//开始计时
	void SetDisableAndUpdateWnd(bool bIsDisable=true);
};
