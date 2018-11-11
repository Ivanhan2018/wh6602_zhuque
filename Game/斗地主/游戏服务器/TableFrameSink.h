#ifndef TABLE_FRAME_SINK_HEAD_FILE
#define TABLE_FRAME_SINK_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "GameLogic.h"
#include "cnd_ILogMgr.h"
#include <vector>
using namespace std ;

//////////////////////////////////////////////////////////////////////////

//游戏桌子类
class CTableFrameSink : public ITableFrameSink, public ITableUserAction
{
private:
	cnd_ILogMgr*	m_pRoomLog;	// 日志组件的指针

public:
	// GameSerivce.dll 通向游戏服务端的通道，可以传递任何简易数据或成员指针
	virtual bool channel_to_game_service(int iMainCode, int iAssistantCode, void* pData, int iDataLen);

	//游戏变量
protected:
	WORD							m_wBombTime;						//炸弹倍数
	WORD							m_wBombNums;						//炸弹个数
	BYTE							m_bLandScore;						//地主分数
	WORD							m_wFirstUser;						//首叫用户
	WORD							m_wBankerUser;						//庄家用户
	WORD							m_wCurrentUser;						//当前玩家
	BYTE							m_bScoreInfo[GAME_PLAYER];			//叫分信息
	int                             m_bUserBeiShu[GAME_PLAYER];         //玩家倍数
	bool                            m_IsMissile;                        //是否是火箭
	WORD                            m_OneBombNums[GAME_PLAYER];         //一个人打出的炸弹
	WORD							m_wOperateUser;						//操作用户		Author<cxf>

	//运行信息
protected:
	WORD							m_wTurnWiner;						//胜利玩家
	BYTE							m_bTurnCardCount;					//出牌数目
	BYTE							m_bTurnCardData[20];				//出牌列表
	bool							m_bUserTrustee[GAME_PLAYER];		//玩家托管
	BYTE							m_bCircuit[GAME_PLAYER];			//用户出牌次数
	int                             m_tagTask[GAME_PLAYER];             //任务标识位

	//疯狂变量
protected:
	bool							m_bBrightCard;						//明牌模式
	BYTE							m_bCallScorePhase;					//叫牌阶段
	bool							m_bGiveUpBanker[GAME_PLAYER];		//放弃标识
	bool							m_bBrightStart[GAME_PLAYER];		//明牌开始
	BYTE							m_cbBrightCardTime;					//明牌倍数
	BYTE							m_cbDoubleCount;					//加倍统计
	BYTE							m_cbBrightCardCount;				//明牌统计
	bool							m_bFinishDoubleUser[GAME_PLAYER];	//完成加倍
	bool							m_bDoubleUser[GAME_PLAYER];			//加倍玩家
	bool							m_bAllowDoubleScore;				//允许加倍
	bool							m_bUserBrightCard[GAME_PLAYER];		//玩家亮牌
	BYTE							m_bGiveUpSnatch[GAME_PLAYER];		//抢地主标识<0:没有操作; 1:不抢; 2:抢地主>

	//扑克信息
protected:
	BYTE							m_bBackCard[3];						//游戏底牌
	BYTE							m_bCardCount[GAME_PLAYER];			//扑克数目
	BYTE							m_bOutCardCount[GAME_PLAYER];		//出牌次数
	BYTE							m_bHandCardData[GAME_PLAYER][20];	//手上扑克
	CGameLogic						m_GameLogic;						//游戏逻辑
	BYTE                            m_bSnatchCount;                     //抢地主次数

	
	//断线玩家信息,add by Ivan_han
protected:
	vector<WORD>                    m_vecChairIDOffLine;		            //保存断线玩家的椅子号

	//组件变量
protected:
	ITableFrame						* m_pITableFrame;					//框架接口
	const tagGameServiceOption		* m_pGameServiceOption;				//配置参数

	//属性变量
protected:
	static const WORD				m_wPlayerCount;						//游戏人数
	static const enStartMode		m_GameStartMode;					//开始模式

	//函数定义
public:
	//构造函数
	CTableFrameSink();
	//析构函数
	virtual ~CTableFrameSink();

	//基础接口
public:
	//释放对象
	virtual VOID __cdecl Release() { }
	//是否有效
	virtual bool __cdecl IsValid() { return AfxIsValidAddress(this,sizeof(CTableFrameSink))?true:false; }
	//接口查询
	virtual void * __cdecl QueryInterface(const IID & Guid, DWORD dwQueryVer);

	//管理接口
public:
	//初始化
	virtual bool __cdecl InitTableFrameSink(IUnknownEx * pIUnknownEx);
	//复位桌子
	virtual void __cdecl RepositTableFrameSink();

	//信息接口
public:
	//开始模式
	virtual enStartMode __cdecl GetGameStartMode();
	//游戏状态
	virtual bool __cdecl IsUserPlaying(WORD wChairID);

	//动作事件
public:
	//用户断线
	virtual bool __cdecl OnActionUserOffLine(WORD wChairID, IServerUserItem * pIServerUserItem);// { return true; }
	//用户重入
	virtual bool __cdecl OnActionUserReConnect(WORD wChairID, IServerUserItem * pIServerUserItem);// { return true; }
	//用户坐下
	virtual bool __cdecl OnActionUserSitDown(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser) { return true; }
	//用户起来
	virtual bool __cdecl OnActionUserStandUp(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser) ;
	//用户同意
	virtual bool __cdecl OnActionUserReady(WORD wChairID, IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize) { return true; }

	//游戏事件
public:
	//游戏开始
	virtual bool __cdecl OnEventGameStart();

	//游戏常规结束(bLandWin=true表示地主赢了,bLandWin=false表示农民赢了),add by Ivan_han
	bool __cdecl OnEventGameNormalEnd(WORD wChairID,bool bLandWin/*, IServerUserItem * pIServerUserItem, BYTE cbReason*/);
	//游戏结束
	virtual bool __cdecl OnEventGameEnd(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason);
	//发送场景
	virtual bool __cdecl SendGameScene(WORD wChiarID, IServerUserItem * pIServerUserItem, BYTE cbGameStatus, bool bSendSecret);

	//事件接口
public:
	//定时器事件
	virtual bool __cdecl OnTimerMessage(WORD wTimerID, WPARAM wBindParam);
	//游戏消息处理
	virtual bool __cdecl OnGameMessage(WORD wSubCmdID, const void * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem);
	//框架消息处理
	virtual bool __cdecl OnFrameMessage(WORD wSubCmdID, const void * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem);

	//游戏事件
protected:
	//叫分事件
	bool OnUserLandScore(WORD wChairID, BYTE bLandScore);		//Author<cxf>
	//用户出牌
	bool OnUserOutCard(WORD wChairID, BYTE bCardData[], BYTE bCardCount);
	//用户放弃
	bool OnUserPassCard(WORD wChairID);

	//add by Ivan_han
	//服务器端替断线玩家出牌或不出或叫地主或抢地主
	bool OnAutoPassOrOutCard();

	//玩家托管
	bool OnUserTrustee(WORD wChairID, bool bTrustee);
	//抢地主事件
	bool OnUserSnatchLand(WORD wChairID, BYTE bLandScore);		//Author<cxf>

	//疯狂事件
protected:
	//明牌开始
	bool OnBrightStart( WORD wChairID, bool bBright );
	//玩家明牌
	bool  OnBrightCard( WORD wChairID, BYTE cbBrightCardTime);
	//玩家加倍
	bool OnUserDoubleScore( WORD wChairID, bool bDoubleScore, WORD bDoubleType);
	//发送消息
	void SendGameMessage( WORD wChairID, CHAR szMsg[] );

    //设置固定牌或随机牌或好牌
	int SetFixOrRandCardOrGoodCard();
	//随机牌
	void SetRandCard();
	//好牌只发给一个人
	void SetGoodCard1();
	////好牌只发给二个人
	//void SetGoodCard2();

	//OSC add
	//任务判断
protected:
	int OnUserTaskJudgement(WORD wChairID, WORD wBandID);
};

//////////////////////////////////////////////////////////////////////////

#endif
