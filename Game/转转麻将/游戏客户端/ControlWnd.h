#ifndef CONTROL_WND_HEAD_FILE
#define CONTROL_WND_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "Resource.h"
#include "GameLogic.h"

class CGameClientView;
class CGameClientDlg;
//////////////////////////////////////////////////////////////////////////

//消息定义
#define IDM_CARD_OPERATE			(WM_USER+301)						//扑克操作

////////////////////////////////////////////////////////////////////////////
//
//struct ShowGangListData
//{
//	BYTE	byCardData;
//	BYTE	byTag;	//0:补, 1:杠
//};
//
//
//
//
//
////控制窗口
//class CControlWnd : public CWnd
//{
//	//配置变量
//protected:
//	BYTE							m_cbActionMask;						//类型掩码
//	BYTE							m_cbCenterCard;						//中心扑克
//	BYTE							m_cbGangCard[5];					//杠牌数据
//	CPoint							m_PointBenchmark;					//基准位置
//
//	ShowGangListData				m_sgldCardData[8];					//显示杠补牌数据
//	BYTE							m_sgldCardNums;						//个数
//
//
//	//状态变量
//protected:
//	BYTE							m_cbItemCount;						//子项数目
//	BYTE							m_cbCurrentItem;					//当前子项
//
//	//控件变量
//public:
//	CSkinButton						m_btChiHu;							//吃胡按钮
//	CSkinButton						m_btGiveUp;							//放弃按钮
//
//
//	//资源变量
//protected:
//	CPngImage						m_ImageControlTop;					//资源图片
//	CGDIPlus_Image					m_ImageControlMid;					//资源图片
//	CPngImage						m_ImageControlBottom;				//资源图片
//  	CPngImage						m_ImageActionExplain;				//动作解释
//	CPngImage                       m_ImageTableBottom;                 //麻将资源
//	CPngImage                       m_ImageActionBlack;                 //吃碰杠阴影
//	CPngImage                       m_ImageActionCardBack;              //吃碰杠牌背景
//	CPngImage                       m_ImageActionCardNum;               //吃碰杠牌数据
//
//	//函数定义
//public:
//	//构造函数
//	CControlWnd();
//	//析构函数
//	virtual ~CControlWnd();
//
//	//控件控制
//public:
//	//基准位置
//	void SetBenchmarkPos(int nXPos, int nYPos);
//	//设置扑克
//	void SetControlInfo(BYTE cbCenterCard, BYTE cbActionMask, tagGangCardResult & GangCardResult);
//	//杠补信息
//	void AddGangCardData(const BYTE* pGangCardData, BYTE byCardNums );
//	//获取位置
//	int GetImageIndex(BYTE cbCardData);
//
//
//	//内部函数
//protected:
//	//调整控件
//	void RectifyControl();
//
//	//消息映射
//protected:
//	//重画函数
//	afx_msg void OnPaint();
//	//吃胡按钮
//	afx_msg void OnChiHu();
//	//放弃按钮
//	afx_msg void OnGiveUp();
//	//建立消息
//	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
//	//鼠标消息
//	afx_msg void OnLButtonDown(UINT nFlags, CPoint Point);
//	//光标消息
//	afx_msg BOOL OnSetCursor(CWnd * pWnd, UINT nHitTest, UINT uMessage);
//
//	DECLARE_MESSAGE_MAP()
//
//
//public:
//	void AddRectRgn(CDC* pDC,CRgn* rgn,CRect* rect,COLORREF col);
//	void UpdateRgn(CDC * pDC);
//	//bool DrawWindow(CDC * pDC);
//};

//////////////////////////////////////////////////////////////////////////
class CControlWnd: public CWnd
{
	friend class CGameClientView;
	friend class CGameClientDlg;

	DECLARE_MESSAGE_MAP()
public:
	CControlWnd();
	~CControlWnd();
	/// 设置动作
	/// [@param byCardData in] 牌数据
	/// [@param byAction in] 操作动作
	void SetAction(BYTE byCardData, BYTE byAction);

protected:
	BOOL SetImageRgn(HWND hWnd, CPngImage * pImage);
	// 安全显示或隐藏窗口
	void SafeShowOrHideWnd(CWnd* pWnd, int nCmdShow);

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
	afx_msg void OnBtClickPeng();
	afx_msg void OnBtClickBu();
	afx_msg void OnBtClickHu();
	afx_msg void OnBtClickGiveup();


private:
	CSkinButton						m_btPeng;					///< 碰按钮
	CSkinButton						m_btBu;						///< 补按钮
	CSkinButton						m_bthu;						///< 胡按钮
	CSkinButton						m_btGiveUp;				///< 放弃按钮

	CPngImage						m_pngBackground;	///< 背景
	BYTE									m_bAction;					///< 动作
	BYTE									m_bCardData;			///< 牌数据
};
#endif