#pragma once

#include "Stdafx.h"
#include "ControlWnd.h"
#include "CardControl.h"
#include "GameScoreWnd.h"
#include "DlgAIPrompt.h"
#include "..\..\开发库\Include\GameFlash.h"

//////////////////////////////////////////////////////////////////////////
//消息定义

#define IDM_START					(WM_USER+100)						//开始消息
#define IDM_OUT_CARD				(WM_USER+101)						//出牌消息
#define IDM_OPERATE_HAI_DI			(WM_USER+102)						//出牌消息
#define IDM_TRUSTEE_CONTROL			(WM_USER+103)						//托管控制
#define IDM_SHOW_SZ                 (WM_USER+104)                       //骰子消息

 class CGameClientDlg;

//////////////////////////////////////////////////////////////////////////

//游戏视图
class CGameClientView : public CGameFrameView
{
	//标志变量
protected:
	bool							m_bOutCard;							//出牌标志
	bool							m_bWaitOther;						//等待标志
	BYTE							m_cbHaiDiCard;						//海底扑克
	bool							m_bTrustee[GAME_PLAYER];			//是否托管
	

	//游戏属性
protected:
	LONGLONG						m_lCellScore;						//基础积分
	WORD							m_wBankerUser;						//庄家用户

	//用户状态
protected:
	BYTE							m_cbCardData;						//出牌扑克
	WORD							m_wOutCardUser;						//出牌用户
	BYTE							m_cbUserAction[4];					//用户动作


public:
	WORD                            m_wChiHuKind[GAME_PLAYER];          //吃胡类型
	WORD                            m_wChiHuRight[GAME_PLAYER];         //胡牌权位
	WORD                            m_wChiHuType;                       //游戏结束类型
	int                             m_iChiHuNums;                       //胡牌玩家个数

	//位置变量
protected:
	CPoint							m_UserFlagPos[4];					//标志位置 南西北
	CPoint							m_UserBankerFlagPos[4];				//标志位置 庄
	CPoint							m_PointTrustee[GAME_PLAYER];		//托管位置
	CPoint							m_Class[GAME_PLAYER];		        //等级位置
	CPoint							m_Gold[GAME_PLAYER];		        //宝石位置
	CPoint                          m_ptBirdPos[GAME_PLAYER];           //扎鸟位置

	CPoint                          m_Info[3];                          //信息框位置(0:对家; 1:右家; 2: 左家)

	

	//位置变量
	POINT								m_ptActionDice;					//骰子动画起始位
	POINT								m_ptActionDice2;				//骰子动画
	CSize								m_csActionDice1;				//骰子动画大小
	CSize								m_csActionDice2;				//骰子大小


public:
	//数据变量
	int									m_nDiceCount1;					//骰子数1
	int									m_nDiceCount2;					//骰子数2
	bool                                m_bIsPlayBird;                  //是否播放扎鸟动画
	BYTE                                m_cbBirdCardData;               //鸟牌

	//位图变量
protected:
	CPngImage						m_ImageBack;						//背景图案
	CPngImage                       m_ImageDirectionBack;               //方位标志背景
	CPngImage						m_ImageUserFlag;					//方位标志
	CPngImage						m_ImageUserBankerFlag;				//庄家标志
	CPngImage						m_ImageOutCardBack;					//出牌背景

	CPngImage						m_ImageHuangZhuang;					//荒庄标志
	CPngImage						m_ImageTrustee;						//托管标志	
	CPngImage						m_ImageOutCard;						//出牌提示
	CPngImage						m_ImageWait;						//等待提示
	CPngImage                       m_ImageMPEffect;                    //筛子动画
	CPngImage                       m_ImageMPBACK;                      //摸牌动画
	CPngImage                       m_ImageMPCARD;                      //摸牌数据

	CPngImage                       m_ImageUserInfo[GAME_PLAYER];       //其余玩家信息框
	CPngImage                       m_ImageUserGradeNum;                //等级数字图片

	CPngImage						m_ActionDice1;					    //骰子动画
	CPngImage						m_ActionDice2;					    //骰子动画定点

	CPngImage                        m_ActionBird[GAME_PLAYER];         //扎鸟动画

	
	//骰子和扎鸟动画
protected:
	bool							m_bDiceEffect;						//骰子效果
	bool							m_bBirdEffect;						//开始效果
	BYTE							m_cbFrameIndex;					    //帧数索引
	int   							m_tPlayBirdIndex;					//摸牌效果
	int                             m_bViewBirdID;                      //扎鸟玩家

   

	//扑克控件
public:
	CHeapCard						m_HeapCard[4];						//堆立扑克
	CUserCard						m_UserCard[3];						//用户扑克
	CTableCard						m_TableCard[4];						//桌面扑克
	CWeaveCard						m_WeaveCard[4][4];					//组合扑克
	CDiscardCard					m_DiscardCard[4];					//丢弃扑克
	CCardControl					m_HandCardControl;					//手上扑克

	int								m_tDice;						    //骰子动画时间
	int                             m_tStart;                           //开始动画时间
	bool                            m_bIsHuangZhuang;                   //是否荒庄
	


	//控件变量
public:
	CSkinButton					    m_btStart;							//开始按钮
	CSkinButton						m_btAutoPlayOn;
	CSkinButton						m_btAutoPlayOff;                    //取消托管
	CSkinButton						m_btHaiDi;							//海底按钮
	CSkinButton						m_btNoHaiDi;						//放弃按钮
	CControlWnd						m_ControlWnd;						//控制窗口


	//控件变量
public:
	//CFlash      					m_FlashControl;						//动画视图


private:
	CGameClientDlg*					m_pGameClientDlg;
    CDlgAIPrompt                    m_CardHuPaiList;                    //智能提示框
	CCardControl                    m_CardControl;                      //牌控件
	CGameFlash						m_GameOperationFlash;				//操作动画
	CGameFlash                      m_ChiHuKindFlash;                   //胡牌动画

	int								m_iSaveCheckPaiIndex;
	CPoint                          m_ptChiHuFlash[GAME_PLAYER];       //胡牌动画位置
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
	void SetGameClientDlgPtr(CGameClientDlg* pGameClient);

	//功能函数
public:
	//获取图片路径
	void GetPicPath(CString& strPath);
	//基础积分
	void SetCellScore(LONGLONG lCellScore);
	//海底扑克
	void SetHaiDiCard(BYTE cbHaiDiCard);
	//庄家用户
	void SetBankerUser(WORD wBankerUser);
	//状态标志
	void SetStatusFlag(bool bOutCard, bool bWaitOther);
	//出牌信息
	void SetOutCardInfo(WORD wViewChairID, BYTE cbCardData);
	//动作信息
	void SetUserAction(WORD wViewChairID, BYTE bUserAction);
	//设置托管
	void SetTrustee(WORD wTrusteeUser,bool bTrustee);
	
	//绘图函数
	//特效动画
	void ActionEffect(CDC *pDC,int nWidth, int nHeight,int nFrame);
	//扎鸟动画
	void ActionPlayBird(CDC *pDC,int nFrame,BYTE byViewID, CPoint pt, BYTE cbBirdCardData);
	////开始动画
	//void ActionStart( CDC *pDC,int nFrame);
	//骰子动画
	void ActionDice1( CDC *pDC,int nFrame);
	//骰子动画2
	void ActionDice2( CDC *pDC,int pos1, int pos2 );
	//扎鸟动画
	bool SetBirdEffect(bool bBirdEffect,BYTE bViewID);
	//骰子效果
	bool SetDiceEffect(bool bDiceEffect);
    //动作动画
	void PlayOperationFlash(BYTE byViewID);
	//胡牌动画
	void PlayChiHuFlash(BYTE byViewID);


	void InitViewFrame();


public:
	void SetCanHuPaiData(const BYTE* pCardData, int iCardNums, int iX);


	//辅助函数
protected:
	//艺术字体
	void DrawTextString(CDC * pDC, LPCTSTR pszString, COLORREF crText, COLORREF crFrame, int nXPos, int nYPos);

	//消息映射
protected:
	//时间消息
	VOID OnTimer(UINT nIDEvent);
	//开始按钮
	afx_msg void OnStart();
	//海底按钮
	afx_msg void OnHaiDi();
	//放弃按钮
	afx_msg void OnNoHaiDi();
	//拖管控制
	afx_msg void OnStusteeControl();
	// 自动托管
	afx_msg void OnAutoPlayerOn();
	// 取消托管
	afx_msg void OnAutoPlayerOff();
	//建立函数
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//鼠标消息
	afx_msg void OnLButtonDown(UINT nFlags, CPoint Point);
	//光标消息
	afx_msg BOOL OnSetCursor(CWnd * pWnd, UINT nHitTest, UINT uMessage);

	DECLARE_MESSAGE_MAP()



public:
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
};

//////////////////////////////////////////////////////////////////////////
