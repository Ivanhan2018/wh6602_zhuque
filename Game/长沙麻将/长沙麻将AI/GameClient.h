#ifndef		___CREATE_DEF_GAME_CLIENT_1005_DEF__
#define		___CREATE_DEF_GAME_CLIENT_1005_DEF__

#include "..\..\开发库\Include\gameclientframe.h"
#include "CMD_Sparrow.h"
#include "GameLogic.h"

//出牌结果
struct tagOutCardResult
{
	BYTE							cbOperateCode;						//出牌代码
	BYTE							cbOperateCard;						//出牌值
};

//听牌分析结果
struct tagTingCardResult
{
	BYTE bAbandonCount;													//可丢弃牌数
	BYTE bAbandonCard[MAX_COUNT];										//可丢弃牌
	BYTE bTingCardCount[MAX_COUNT];										//听牌数目
	BYTE bTingCard[MAX_COUNT][MAX_COUNT-1];								//听牌
	BYTE bRemainCount[MAX_COUNT];										//听的牌的剩余数
};

//游戏客户端
class CGameClient : public CGameClientFrame
{
public:
	CGameClient(void);
	virtual ~CGameClient(void);

private:
	//游戏逻辑
	CGameLogic				m_GameLogic;

private:
	WORD					m_wCurrentUser;								//当前玩家
	WORD					m_wBankerUser;								//庄家
	BYTE					m_cbActionMask;								//动作标识
	BYTE					m_cbActionCard;								//操作牌

	//出牌信息
protected:
	WORD					m_wOutCardUser;								//出牌用户
	BYTE					m_cbOutCardData;							//出牌扑克
	BYTE					m_cbDiscardCount[GAME_PLAYER];				//丢弃数目
	BYTE					m_cbDiscardCard[GAME_PLAYER][60];			//丢弃记录

	//扑克变量
protected:
	BYTE					m_cbCardIndex[GAME_PLAYER][MAX_INDEX];		//手中扑克
	BYTE					m_cbLeftCardCount;							//剩余数目

	//组合扑克
protected:
	BYTE					m_cbWeaveCount[GAME_PLAYER];				//组合数目
	tagWeaveItem			m_WeaveItemArray[GAME_PLAYER][MAX_WEAVE];	//组合扑克

public:
	//初始函数
	virtual bool InitGameFrame();
	//重置框架
	virtual void ResetGameFrame();

	//系统事件
public:
	//时间消息
	virtual bool OnTimerMessage(UINT nElapse, UINT nTimerID);
	//网络消息
	virtual bool OnGameMessage(WORD wSubCmdID, const void * pBuffer, WORD wDataSize);
	//游戏场景
	virtual bool OnGameSceneMessage(BYTE cbGameStatus, const void * pBuffer, WORD wDataSize);

public:
	//发送之前初始化
	void InitSendCard();
	//取消所有定时器
	void CancelAllTimer();

	//消息处理
protected:
	//游戏开始
	bool OnSubGameStart(const void * pBuffer, WORD wDataSize);
	//游戏结束
	bool OnSubGameEnd(const void * pBuffer, WORD wDataSize);
	//海底操作
	bool OnSubOperateHaiDi(const void * pBuffer, WORD wDataSize);
	//游戏开始出牌
	bool OnSubGameStartOutCard(const void * pBuffer, WORD wDataSize);
	//用户出牌
	bool OnSubOutCard(const void * pBuffer, WORD wDataSize);
	//操作提示
	bool OnSubOperateNotify(const void * pBuffer, WORD wDataSize);
	//发牌消息
	bool OnSubSendCard(const void * pBuffer, WORD wDataSize);
	//操作结果
	bool OnSubOperateResult(const void * pBuffer, WORD wDataSize);

	//处理函数
protected:
	//出牌
	void OnOutCard( BYTE cbOutCard );
	//操作牌
	void OnOperateCard( BYTE cbOperateCode,BYTE cbOperateCard );

	//辅助函数
protected:
	//搜索出牌
	bool SearchOutCard( tagOutCardResult &OutCardResult );

public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnTimer(UINT nIDEvent);
};
#endif