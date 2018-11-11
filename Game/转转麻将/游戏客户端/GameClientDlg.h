#pragma once

#include "Stdafx.h"
#include "GameLogic.h"
#include "GameClientView.h"
#include "GameScoreWnd.h"

//////////////////////////////////////////////////////////////////////////

//游戏对话框
class CGameClientDlg : public CGameFrameDlg
{
	//用户变量
protected:
	WORD							m_wBankerUser;						//庄家用户
	WORD							m_wCurrentUser;						//当前用户

	//托管变量
protected:
	bool							m_bStustee;             			//托管标志
	WORD							m_wTimeOutCount;					//超时次数

	//状态变量
protected:
	//bool							m_bHearStatus;						//听牌状态
	bool                            m_bZM;                              //是否自摸 
	bool                            m_bFP;                              //是否吃炮
	bool							m_bIsMeCanOutCard;					//是否自己可以出牌

	//动作信息
protected:
	BYTE							m_byUserAction[4];					//用户动作

	//声音信息
protected:
	BYTE							m_cbGender;					         //用户性别
	CString                         m_InfoSound;                         //声音信息


	//堆立变量
protected:
	WORD							m_wHeapHead;						//堆立头部
	WORD							m_wHeapTail;						//堆立尾部
	BYTE							m_cbHeapCardInfo[4][2];				//堆牌信息

	//扑克变量
protected:
	BYTE							m_cbLeftCardCount;					//剩余数目
	BYTE							m_cbCardIndex[MAX_INDEX];			//手中扑克
	//BYTE                            m_cbTempKing[4];                    //临时变量

	//组合扑克
protected:
	BYTE							m_cbWeaveCount[GAME_PLAYER];		//组合数目
	tagWeaveItem					m_WeaveItemArray[GAME_PLAYER][4];	//组合扑克

	//出牌信息
protected:
	WORD							m_wOutCardUser;						//出牌用户
	BYTE							m_cbOutCardData;					//出牌扑克
	BYTE							m_cbDiscardCount[GAME_PLAYER];		//丢弃数目
	BYTE							m_cbDiscardCard[GAME_PLAYER][55];	//丢弃记录

	//控件变量
protected:
	CGameLogic						m_GameLogic;						//游戏逻辑
	CGameClientView					m_GameClientView;					//游戏视图
	CGameScoreWnd				    m_ScoreView;						//积分视图
   
	tagGameScoreInfo                m_GameScoreInfo;                      //结束变量


	//
	CMD_S_GameStart					m_GameStartData;


	//控件变量
public:
	//CGameFlash      				m_TingCardFlash;						//动画视图
	

public:
	BYTE							m_byCanSelectCard[4];				//能选择的牌数据
	BYTE							m_byCanSelectCount;					//能选择的牌数目
	BYTE							m_byCanSelectType;					//类型，杠，碰

	//声音
protected:
	CString							m_sDialect;						   //方言
	
	//函数定义
public:
	//构造函数
	CGameClientDlg();
	//析构函数
	virtual ~CGameClientDlg();

	//常规继承
private:
	//初始函数
	virtual bool InitGameFrame();
	//重置框架
	virtual void ResetGameFrame();
	//游戏设置
	virtual void OnGameOptionSet();
	//时间消息
	virtual bool OnTimerMessage(WORD wChairID, UINT nElapse, UINT nTimerID);
	//旁观状态
	virtual void OnLookonChanged(bool bLookonUser, const void * pBuffer, WORD wDataSize);
	//网络消息
	virtual bool OnGameMessage(WORD wSubCmdID, const void * pBuffer, WORD wDataSize);
	//游戏场景
	virtual bool OnGameSceneMessage(BYTE cbGameStatus, bool bLookonOther, const void * pBuffer, WORD wDataSize);

	//用户事件
public:
	//用户进入
	virtual void __cdecl OnEventUserEnter(tagUserData * pUserData, WORD wChairID, bool bLookonUser);
	//用户离开
	virtual void __cdecl OnEventUserLeave(tagUserData * pUserData, WORD wChairID, bool bLookonUser);

public:
	//点击继续游戏
	virtual void OnEventContinueGame();

	//检测胡牌数据
	void CheckCanHuPai(BYTE byCardData, int iX);

	//消息处理
protected:
	bool OnSubStartSZFlash();
	//游戏开始
	bool OnSubGameStart(const void * pBuffer, WORD wDataSize);
	//用户出牌
	bool OnSubOutCard(const void * pBuffer, WORD wDataSize);
	//发牌消息
	bool OnSubSendCard(const void * pBuffer, WORD wDataSize);
	//操作提示
	bool OnSubOperateNotify(const void * pBuffer, WORD wDataSize);
	//操作结果
	bool OnSubOperateResult(const void * pBuffer, WORD wDataSize);
	////海底操作
	//bool OnSubOperateHaiDi(const void * pBuffer, WORD wDataSize);
	//游戏结束
	bool OnSubGameEnd(const void * pBuffer, WORD wDataSize);
	//用户托管
	bool OnSubTrustee(const void * pBuffer,WORD wDataSize);
	//听牌
	//bool OnSubTingCard(const void * pBuffer, WORD wDataSize);
	//听牌结果
	//bool OnSubTingCardResult(const void * pBuffer, WORD wDataSize);

private:
	//保存上一轮的牌数据
	void LastTurnOperateCard();

	//辅助函数
public:
	//播放背景音乐
	void PlayMusic(CString szSoundRes);
	//播放声音
	void PlaySound(CString szSoundRes, bool bLoop);
	//播放声音
	void PlayCardSound(BYTE cbGender, BYTE cbCardData);
	//播放声音
	void PlayActionSound(BYTE cbGender, BYTE cbAction);
	//播放随机声音
	void PlayGameSoundRand(BYTE cbGender, CString strSound, bool bSoundType);
	//播放声音
	/// [@param bSoundType in] 多种声音，2种循环请填2，只有一种填0
	void PlayGameSoundLanguage(BYTE cbGender, CString strSound, BYTE bSoundType=1);//参数(1:性别;2:声音;3：是否有双音)
	//扣除扑克
	void DeductionTableCard(bool bHeadCard);
	//显示控制
	bool ShowOperateControl(BYTE cbUserAction,BYTE cbStartData);
	//清空所有定时器
	void ClearAllTimer();
	//发送命令
	void SendCardCommand(BYTE byOperatorCode, BYTE byOperatorCard);

	//消息映射
protected:
	//时间消息
	afx_msg void OnTimer(UINT nIDEvent);
	//启动筛子按钮
	LRESULT OnSZStart(WPARAM wParam, LPARAM lParam);
	//开始消息
	LRESULT OnStart(WPARAM wParam, LPARAM lParam);
	//摸牌消息
	LRESULT OnMoPai(WPARAM wParam, LPARAM lParam);
	//出牌操作
	LRESULT OnOutCard(WPARAM wParam, LPARAM lParam);
	//扑克操作
	LRESULT OnCardOperate(WPARAM wParam, LPARAM lParam);
	////海底操作
	//LRESULT OnOperateHaiDi(WPARAM wParam, LPARAM lParam);
	//拖管控制
	LRESULT OnStusteeControl(WPARAM wParam, LPARAM lParam);
	//骰子消息
	LRESULT OnShowSZ(WPARAM wParam, LPARAM lParam);
	//听牌消息
	LRESULT OnTingCard(WPARAM wParam, LPARAM lParam);
	//色子动画按钮
	LRESULT OnSZFlashBT(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()

private:
	//获得当前游戏玩家个数
	int GetCurGameUserNums();
};

//////////////////////////////////////////////////////////////////////////
