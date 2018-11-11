#pragma once

#include "Stdafx.h"
#include "CardControl.h"
#include "ActionWnd.h"
#include "GameCard.h"
#include "..\..\开发库\Include\GameFlash.h"

//////////////////////////////////////////////////////////////////////////
//消息定义

#define IDM_START					(WM_USER+100)						//开始消息
#define IDM_OUT_CARD				(WM_USER+101)						//出牌消息
#define IDM_PASS_CARD				(WM_USER+102)						//放弃出牌
#define IDM_CALL_LAND				(WM_USER+103)						//叫地主消息
#define IDM_AUTO_OUTCARD			(WM_USER+104)                       //提示消息
#define IDM_LAND_AUTOPLAY			(WM_USER+105)                       //托管消息
#define IDM_SORT_HAND_CARD			(WM_USER+106)						//排列扑克
#define IDM_BRIGHT_START			(WM_USER+107)						//明牌开始
#define IDM_DOUBLE_SCORE			(WM_USER+108)						//加倍消息
#define IDM_SNATCH_LAND             (WM_USER+109)                       //抢地主消息


//动画类型
#define MARK_ACTION_SHUNLIAN          0x01                               //顺子,连对
#define MARK_ACTION_ZHADAN            0x02                               //炸弹
//////////////////////////////////////////////////////////////////////////

//游戏视图
class CGameClientView : public CGameFrameView
{
	//游戏变量
protected:
	WORD							m_wLandUser;						//地主用户
	WORD							m_wBombTime;						//炸弹倍数
	LONGLONG						m_lBaseScore;						//基础分数
	BYTE							m_cbLandScore;						//地主分数
	BYTE							m_bCardBackDate[3];					//底牌数据
	bool							m_bDisplay;							//底牌显示
	BYTE                            m_bCardData[3][20];                 //玩家手牌数据
	BYTE                            m_bHandCardCount[GAME_PLAYER];      //玩家手牌数目
	BYTE                            m_DiPaiCount;                       //底牌张数


	//状态变量
public:
	bool							m_bLandTitle;						//地主标志
	bool							m_bPass[GAME_PLAYER];				//放弃数组
	BYTE							m_bScore[GAME_PLAYER];				//用户叫分
	BYTE							m_bCardCount[GAME_PLAYER];			//扑克数目
	bool							m_bDeasilOrder;						//顺时针出
	bool							m_bUserTrustee[GAME_PLAYER];		//玩家托管
	bool							m_bShowWarming[GAME_PLAYER];		//显示报牌
	BYTE                            CardData[54];

	//疯狂变量
public:
	BYTE							m_cbCallScorePhase;					//抢牌阶段
	BYTE							m_cbUserPhase[GAME_PLAYER];			//玩家阶段
	bool							m_bBrightStart[GAME_PLAYER];		//明牌开始
	CPoint							m_ptBrightStart[GAME_PLAYER];		//明牌开始
	BYTE							m_cbBrightTime;						//明牌倍数
	bool							m_bShowLandTime;					//显示倍数
	bool							m_bDoubleScore[GAME_PLAYER];		//玩家加倍
	bool                            m_bLeftUser;                          //左边玩家
	bool                            m_bTXEffect;                        //特效效果
	BYTE                            m_bTXFrameIndex;                    //特效索引
	bool                            m_bHuoJian;                         //火箭效果
	BYTE                            m_bHuojianIndex;                    //火箭索引
//lq* 
	//位置信息
public:
	CSize							m_LandSize;							//挖坑标志
	CPoint							m_ptScore[GAME_PLAYER];				//叫分位置
	CPoint							m_ptLand[GAME_PLAYER];				//反牌位置
	CPoint							m_ptWarmingCard[GAME_PLAYER];		//报牌位置
	CPoint							m_ptUserTrustee[GAME_PLAYER];		//玩家托管位置
	CPoint							m_ptUserBaoShi[GAME_PLAYER];		//玩家宝石
	CPoint							m_ptUserGrade[GAME_PLAYER];			//玩家等级
	CPoint							m_ptUserSYPTS[GAME_PLAYER];			//剩余牌提示
	CPoint                          m_GrandFrame[GAME_PLAYER];          //等级框
	CPoint                          m_UserDuanXian[GAME_PLAYER];        //玩家断线
	CPoint                          m_Rating[GAME_PLAYER];              //等级称谓 
	CPoint                          m_OutCardPass[GAME_PLAYER];         //不出

	//位图变量
protected:
	CSkinImage						m_ImageBomb;						//炸弹图片
	CSkinImage						m_ImageCenter;						//背景资源
	CPngImage						m_ImageWaitScore;					//等待叫分
	CPngImage                       m_ImageBack;                        //背景

	//PNG变量
protected:
	CPngImage                       m_Imagebigness;                     //大不上牌提示图片  lq;
	CPngImage						m_ImageScore;						//分数图片
	CPngImage						m_ImageLand;						//庄家图片
	CPngImage						m_ImageUserTrustee;					//玩家托管
	//CPngImage						m_ImageOnecardWarming;				//报单提示
	//CPngImage						m_ImageTwoCardWarming;				//报双提示
	CPngImage						m_ImageLandTimes;					//叫牌倍数
	CPngImage                       m_ImageLandbei;                     //倍字
	CPngImage						m_PngCardBack;						//底牌
	CPngImage                       m_XieCard;                          //对家手牌背面显示
	CPngImage                       m_XieCardThree;                     //对家手牌背面三张
	CPngImage                       m_XieCardTwo; 
	CPngImage                       m_XieCardOne; 
	CPngImage                       m_ImagePass;                        //pass图片 

	//剩余牌张数
	//CPngImage						m_ImageSYPBack;						//剩余牌张数背景
	//CPngImage						m_ImageSYPNums;						//几张
	CPngImage                       m_ImageCardNum;                     //手牌个数
	CPngImage                       m_ImageTCard;                       //手牌个数背景
	CPngImage                       m_ImageGrand;                        //等级数字图片
	CPngImage                       m_ImageGrandFrame;                   //等级框图片
	//动画图片
	CGDIPlus_Image                  m_ImageShunZi;                              //顺子
    CGDIPlus_Image                  m_ImageLianDui;                              //连对

	bool                            m_BIsshowFourcolor;     

	
	//按钮控件
public:
	CSkinButton						m_btStart;							//开始按钮
	CSkinButton						m_btOutCard;						//出牌按钮
	CSkinButton						m_btPassCard;						//放弃按钮
	CSkinButton						m_btAutoOutCard;					//提示按钮
	CSkinButton						m_btCallLand;						//叫地主按钮
	CSkinButton						m_btGiveUpCallLand;					//不叫按钮
	CSkinButton						m_btSnatchLand;						//抢地主按钮
	CSkinButton						m_btGiveUpSnatchLand;				//不抢按钮
	CSkinButton						m_btSortCard;						//排序按钮
	CSkinButton					    m_btAutoPlayOn;						// 开启自动托管
	CSkinButton						m_btAutoPlayOff;					// 关闭自动托管

	//扩展按钮
public:
	//CSkinButton						m_btSnatchLand;						//抢庄按钮
	//CSkinButton						m_btBrightCard;						//明牌按钮
	//CSkinButton						m_btBrightStart;					//明牌开始
	//cxf 删除加倍按钮  用(倒-跟-反)代替
	//CRgnButton						m_btDoubleScore;					//加倍按钮
	//CRgnButton						m_btNotDoubleScore;					//不加倍按钮
	//CSkinButton						m_btCardDao;							//倒
	//CSkinButton						m_btCardGen;							//跟
	//CSkinButton						m_btCardFan;							//反
	//CSkinButton                      m_bLastRound;                           //上一轮 lq
	//cxf end
	//扑克控件
public:
	CCardControl					m_UserCardControl[GAME_PLAYER];		//出牌：扑克视图
	CCardControl					m_HandCardControl[GAME_PLAYER];		//手上扑克
//动画对象
public:
	ActionWnd                       m_ActionWnd[GAME_PLAYER];           //顺子，连对，炸弹视图
	ActionWnd                       m_BActionWnd;                       //飞机视图
	ActionWnd                       m_HuoJianAction;                    //火箭
	ActionWnd                       m_DoubleAction;                     //加倍
	ActionWnd                       m_SpringAction;                     //春天
	

public:
	//游戏牌控件
	CGameCard						m_GameCard;
	CPngImage						m_PngCard;
	

	//提示剩余几张牌
public:
	CMD_C_UserTSZhangNums			m_UserTSCardNums[GAME_PLAYER];		//每个玩家手中的剩余张数(<=3)

#ifdef VIDEO_GAME

	//视频组件
private:
	CVideoServiceManager			m_VedioServiceManager;				//视频对象
	CVideoServiceControl			m_DlgVedioService[GAME_PLAYER];		//视频组件

#endif

private:
	int								m_iMeBeiShu;                        //当前倍数

private:
	bool							m_bIsShowbigness;					//是否显示大不起牌图片
	bool                            m_bIsShowZhaDan;                    //是否显示炸弹
	bool                            m_bIsShowSpring;                    //是否显示春天

	//函数定义
public:
	//构造函数
	CGameClientView();
	//析构函数
	virtual ~CGameClientView();

	//继承函数
private:
	//重置界面
	virtual void ResetGameView();
	//调整控件
	virtual void RectifyGameView(int nWidth, int nHeight);
	//绘画界面
	virtual void DrawGameView(CDC * pDC, int nWidth, int nHeight);

public:
	void GetPicPath(CString& strPath);

	//功能函数
public:
	//玩家倍数
	public:
	void SetMeBeiShu(int iBeiShu);
	//炸弹倍数
	void SetBombTime(WORD wBombTime);
	//基础分数
	void SetBaseScore(LONGLONG lBaseScore);
	//显示提示
	void ShowLandTitle(bool bLandTitle);
	//设置放弃
	void SetPassFlag(WORD wChairID, bool bPass);
	//扑克数目
	void SetCardCount(WORD wChairID, BYTE bCardCount);
	//挖坑分数
	void SetLandUser(WORD wChairID, BYTE bLandScore);
	//挖坑分数
	void SetLandScore(WORD wChairID, BYTE bLandScore, BYTE cbCallScorePhase, BYTE cbPreCallScorePhase);
	//设置顺序
	void SetUserOrder(bool bDeasilOrder);
	//获取顺序
	bool IsDeasilOrder() { return m_bDeasilOrder; }
	//设置爆炸
	bool SetBombEffect(bool bBombEffect);
	//设置春天
	void SetSpring(); //lq
	bool setFanSpring(bool bSpring);//lq
	//设置加倍
	bool SetDouble(bool bDouble);//lq

	//艺术字体
	void DrawTextString(CDC * pDC, LPCTSTR pszString, COLORREF crText, COLORREF crFrame, int nXPos, int nYPos);
	//玩家托管
	void SetUserTrustee( WORD wChairID, bool bTrustee );
	//显示报牌
	void SetShowWarming(WORD wChairID, bool bShowWarming) ;
	//设置底牌
	void SetCardBackData(BYTE *bCardDate,BYTE bCardCount)
	{
		CopyMemory(m_bCardBackDate,bCardDate,sizeof(BYTE)*bCardCount);
		m_DiPaiCount=bCardCount;
	};
	//设置显示
	void SetCardBackDisPlay(bool bDisplay){m_bDisplay = bDisplay;};


	//OSC update
	//功能：提示少于3张牌的功能.
public:
	void SetUserShouPaiNums(WORD wViewID, WORD wNums);
	//重置牌张数
	void SetResetShouPaiNums();
	void SetResetSYPaiNums();

	//疯狂函数
public:
	//动画特效
	void ActionEffect(CDC *pDC, int nWidth, int nHeigth, int nFrame, int type);
	bool SetFeiJi(bool bTXEffect);
	bool SetHuoJian(bool bTXEffect);
	//明牌开始
	void SetBrightStart(WORD wChairID, bool bBrightStart) ;
	//显示倍数
	void DrawLandTime(CDC *pDC, INT nViewWidth, INT nViewHeight);
	//显示对家手牌个数
	void DrawOtherUserHandNums(CDC *pDC, INT nViewWidth, INT nViewHeight, BOOL bLeftUser, INT nCardNums);
	//明牌倍数
	void SetBrightTime(BYTE cbBrightTime);
	//显示倍数
	void ShowLandTime(bool bShowLandTime);
	//玩家加倍
	void SetDoubleScoreUser( WORD wChairID, bool bDoubleScore );
	//显示玩家手牌
	void SetHandCardData(const BYTE bCardData[],BYTE bCardCount,WORD wChairID);

	//play shunzi
	void PlayActionFlash(BYTE byViewID,BYTE TyprAction);
	//play feiji
	void PlayFeijiFlash(bool bIsShow);
	// play Double
    void PlayDoubelFlash();

public:
	//设置显示大不起牌图片
	void SetShowBigness(bool bIsShow=false);
    //加载资源
	void Loadres(CGdipButton* pBT, CString strValue);

	//消息映射
protected:
	//时间消息
	VOID OnTimer(UINT nIDEvent);
	//开始按钮
	afx_msg void OnStart();
	//出牌按钮
	afx_msg void OnOutCard();
	//放弃按钮
	afx_msg void OnPassCard();
	//叫地主按钮
	afx_msg void OnCallLand();
	//不叫按钮
	afx_msg void OnGiveUpCallLand();
	//抢地主按钮
	afx_msg void OnSnatchLand();
	//不抢按钮
	afx_msg void OnGiveUpSnatchLand();
	//出牌提示
	afx_msg void OnAutoOutCard();
	//建立函数
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//鼠标消息
	afx_msg void OnRButtonUp(UINT nFlags, CPoint Point);
	//鼠标双击
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	// 自动托管
    afx_msg void OnAutoPlayerOn();
    // 取消托管
    afx_msg void OnAutoPlayerOff();
	//排序按钮
	VOID OnBnClickedSortCard();

	//疯狂消息
protected:
	//叫分按钮
	//afx_msg void OnSnatchLand();
	//叫分按钮
	afx_msg void OnBrightCard();
	//明牌开始
	afx_msg void OnBrightStart();	
	/*cxf 倒跟反 替代加倍
	//加倍消息
	afx_msg void OnDoubleScore();
	//加倍消息
	afx_msg void OnNotDoubleScore();	
	*/
	//倒消息
	afx_msg void OnDoubleScoreDao();
	//跟消息
	afx_msg void OnDoubleScoreGen();
	//反消息
	afx_msg void OnDoubleScoreFan();
	//cxf end 
	afx_msg void OnLastRound();//lq

	DECLARE_MESSAGE_MAP()

	//osc 2011-10-22
public:
  void InitializeAction();
	
private:
	bool	m_bIsInitBT;
	bool    m_bThreeIsInitBT;
public:
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
};

//////////////////////////////////////////////////////////////////////////
