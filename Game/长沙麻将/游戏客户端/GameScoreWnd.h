#pragma once

#include "Stdafx.h"
#include "ImageNumsCS.h"
#include "Resource.h"

//////////////////////////////////////////////////////////////////////////
//结构定义

//积分信息
struct tagGameScoreInfo
{
	BYTE							cbBirdCard;							//扎鸟扑克
	BYTE							cbChiHuCard;						//吃胡扑克
	WORD							wBankerUser;						//庄家用户
	WORD							wBirdHitUser;						//扎鸟用户
	WORD							wProvideUser;						//点炮用户
	LONGLONG						lScore[GAME_PLAYER];				//用户积分
	WORD							wCardType[GAME_PLAYER];				//胡牌类型
	TCHAR							szUserName[GAME_PLAYER][NAME_LEN];	//用户帐号

	int                             iCellScore;                         //单元积分
	int                             cbTimes[GAME_PLAYER];               //玩家番数
	int                             iChiHuKind[GAME_PLAYER];            //胡牌地位
};

//////////////////////////////////////////////////////////////////////////

//积分报告
//class CGameScoreWnd : public CWnd
class CGameScoreWnd : public CScoreView
{
	//变量定义
protected:
	tagGameScoreInfo				m_GameScoreInfo;					//积分信息
	//CPngImage						m_ImageEndTag;						//胜利标志
	CPngImage                       m_Banker;                           //庄家标识
	CPngImage                       m_Direction;                        //玩家方位标识
	//CPngImage                       m_ImageBirdCount;                   //王牌个数
	CPngImage                       m_ImageEndBirdCard;                 //王牌数据列表
	CPngImage                       m_ImageWin;                         //胜利标识
	CPngImage                       m_ImageLost;                        //失败标识
	CPngImage                       m_ImageBirdTag;                     //扎鸟标识
	CPngImage                       m_ImageChiHuDirection;              //胡牌玩家显示
	CPngImage                       m_ImageNumDown;                     //倒计时数字


	CRect							m_rcDrawTimer;                      //倒计时区域
	WORD                            m_MeChairId;                        //我的椅子号
	WORD                            m_BankerChairID;                    //地主椅子号
	bool                            m_IsUserLeft[GAME_PLAYER];           //玩家是否强退
	CImageNumsCS                    ImageNums1,ImageNums2;             //数字图片

	//按钮
public:
	CSkinButton                     m_btGoOn;                            //继续按钮
	CSkinButton                     m_btChangeTable;                     //换桌
	CSkinButton                     m_btExit;                            //退出
	CSkinButton                     m_btOverTime;                        //延时

	//函数定义
public:
	//构造函数
	CGameScoreWnd();
	//析构函数
	virtual ~CGameScoreWnd();

	//功能函数
public:
	//设置积分
	void SetScoreInfo(tagGameScoreInfo GameScoreInfo);
	//倒计时函数
	void SetCountDown(bool Istrue);
	//获取视图ID
	void GetViewID(WORD MyChairID, WORD BankerChairID);
	//获取位置
	int GetImageIndex(BYTE cbCardData);
	//设置强退
	void SetUserLeft(WORD wChairID,bool IsLeft);

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
	void OnOverTime();

protected:
	//是否显示倒计时
	bool   m_IsTrue; 
	BYTE   m_SecondCount;

public:
	afx_msg void OnTimer(UINT nIDEvent);

	DECLARE_MESSAGE_MAP();
};

//////////////////////////////////////////////////////////////////////////
