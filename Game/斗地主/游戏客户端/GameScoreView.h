#ifndef SCORE_VIEW_HEAD_FILE_DEF_
#define SCORE_VIEW_HEAD_FILE_DEF_

#pragma once

#include "Stdafx.h"
#include "Resource.h"

//////////////////////////////////////////////////////////////////////////

//积分视图类
class CGameScoreView : public CScoreView
{
	//变量定义
protected:
	LONGLONG						m_lGameTax[3];						  //游戏税收
	LONGLONG						m_lGameScore[GAME_PLAYER];			  //游戏得分
	TCHAR							m_szUserName[GAME_PLAYER][NAME_LEN];  //用户名字
	LONGLONG                        m_RoomScore[GAME_PLAYER];             //房间积分
	int                             m_UserBeiShu[GAME_PLAYER];            //游戏倍数
	WORD                            m_MeId;                               //我的椅子号
	WORD                            m_LandChair;                          //地主椅子号
	BYTE                            m_ActionCount;                        //动画帧数
	bool                            m_IsShow;                             //是否播放
	int                             m_EncourageGem[GAME_PLAYER];          //奖励宝石
	bool                            m_IsUserLeft[GAME_PLAYER];            //玩家是否强退
	CRect							m_rcDrawTimer;

	//资源变量
protected:
	CPngImage						m_ImageWin;						      //胜负标识图片
	CPngImage                       m_ImageLose;                          //胜负标识图片 
	CPngImage                       m_BalanceBei;                         //倍数图片
	CPngImage                       m_ImageRoomScore;                     //房间积分图片 
	CPngImage                       m_ImageServers;                       //服务费图片
	CPngImage                       m_ImageLand;                          //地主
	CPngImage                       m_ImageFarmer;                        //农民
	CPngImage                       m_ImageLandWin;                       //地主赢图片
	CPngImage                       m_ImageLandLose;                      //地主输图片
	CPngImage                       m_ImageFarmerWin;                     //农民赢图片
	CPngImage                       m_ImageFarmerLose;                    //农民输图片
	CPngImage                       m_ImageUserLeft;                      //玩家强退图片
	CPngImage                       m_ImageCountDown;                     //倒计时图片
	//按钮
public:
	CSkinButton                     m_btGoOn;                             //继续按钮
	CSkinButton                     m_btChangeTable;                      //换桌
	CSkinButton                     m_btExit;                             //退出
	CSkinButton                     m_btOverTime;                         //延时

	//函数定义
public:
	//构造函数
	CGameScoreView(void);
	//析构函数
	virtual ~CGameScoreView(void);

	//功能函数
public:
	//重置积分
	void ResetScore();
	//设置税收
	void SetGameTax(WORD wChairID,LONGLONG lGameTax);
	//设置积分
	//void SetGameScore(WORD wChairID, LPCTSTR pszUserName, LONGLONG lScore);
	void CGameScoreView::SetGameScore(WORD wChairID, LPCTSTR pszUserName, LONGLONG lScore,int UserBeiShu,LONGLONG RoomScore);
	void CGameScoreView::GetMeID(WORD MeChair,WORD landchair);
	void CGameScoreView::GetWinLose(bool Istrue);
	void CGameScoreView::SetCountDown(bool Istrue);
	//设置宝石
	void CGameScoreView::SetGem(WORD wChairID ,int iGem);
	//设置强退
	void CGameScoreView::SetUserLeft(WORD wchairID,bool IsLeft);
public:
	//初始化信息
	virtual void InitScoreViewInfo();
	//初始化信息后
	virtual void AlreadyInitInfo();
	//绘制
	virtual void DrawScoreInfo(CDC* pDC);
	//获取路径
	void GetPicPath(CString& strPath);
protected:
	//继续按钮
	afx_msg void OnGoOn();
	//退出按钮
	afx_msg void OnExit();
	//换桌按钮
	afx_msg void OnChangeTable();
	//延时按钮
	afx_msg void OnOverTime();
	DECLARE_MESSAGE_MAP();
protected:
	//是否显示倒计时
	bool   m_IsTrue; 
	//倒计时延长变量
	BYTE   m_SecondCount;


public:
	afx_msg void OnTimer(UINT nIDEvent);
	//刷新区域
	void SafeInvalidate(CRect& rect, BOOL bErase);
};


//////////////////////////////////////////////////////////////////////////

#endif
