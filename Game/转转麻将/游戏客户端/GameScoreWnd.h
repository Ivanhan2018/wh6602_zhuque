#pragma once

#include "Stdafx.h"
#include "ImageNumsYY.h"
#include "Resource.h"

//////////////////////////////////////////////////////////////////////////
//结构定义

//积分信息
struct tagGameScoreInfo
{
	BYTE							cbBirdCard;							//扎鸟扑克
	BYTE							cbChiHuCard;						//吃胡扑克
	BYTE                            cbKingCard;                         //王牌数据

	WORD							wBankerUser;						//庄家用户
	WORD							wBirdHitUser;						//扎鸟用户
	WORD							wProvideUser;						//点炮用户
		
	LONGLONG				iCellScore;                         //单元积分
	LONGLONG				iBasicScore;						//基础分

	BYTE                            cbKingCount[GAME_PLAYER];           //王牌数目
	WORD							wCardType[GAME_PLAYER];				//胡牌类型

	
	int                             iChiHuKind[GAME_PLAYER];            //胡牌地位
	LONGLONG								iMuscle[GAME_PLAYER];					// 筋
	LONGLONG                             cbTimes[GAME_PLAYER];               //玩家番数
	LONGLONG						lScore[GAME_PLAYER];				//用户积分
	
	TCHAR							szUserName[GAME_PLAYER][NAME_LEN];	//用户帐号

	tagGameScoreInfo()
	{
		cbBirdCard = 0;
		cbChiHuCard = 0;;
		cbKingCard = 0;
		wBankerUser = 0;
		wBirdHitUser = 0;
		wProvideUser = 0;
		iCellScore = 0;
		iBasicScore = 0;
		memset(cbKingCount, 0, sizeof(cbKingCount));
		memset(wCardType, 0, sizeof(wCardType));
		memset(iMuscle, 0, sizeof(iMuscle));
		memset(cbTimes, 0, sizeof(cbTimes));

		memset(iChiHuKind, 0, sizeof(iChiHuKind));
		memset(lScore, 0, sizeof(lScore));
		memset(szUserName, 0, sizeof(szUserName));
	}
};

//////////////////////////////////////////////////////////////////////////

//积分报告
//class CGameScoreWnd : public CWnd
class CGameScoreWnd : public CScoreView
{
	//变量定义
protected:
	tagGameScoreInfo				m_GameScoreInfo;					//积分信息
	CPngImage                       m_Direction;                        //玩家方位标识
	CPngImage                       m_ImageLeft;                        //逃跑标识
	CPngImage						m_ImageNature;					// 自摸标识
	CPngImage						m_ImageFangPao;					// 放炮标识
	CPngImage						m_ImageOneMuscle;						// 筋杆
	CPngImage                       m_ImageNumDown;                     //倒计时数字


	CRect							m_rcDrawTimer;                      //倒计时区域
	WORD                            m_MeChairId;                        //我的椅子号
	WORD                            m_BankerChairID;                     //地主椅子号
	bool                            m_IsUserLeft[GAME_PLAYER];            //玩家是否强退

	CImageNums						m_NFanNum;									// 番数
	CImageNums						m_NScore;										// 积分

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
