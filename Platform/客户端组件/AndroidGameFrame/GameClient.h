#ifndef		___CREATE_DEF_GAME_CLIENT_1005_DEF__
#define		___CREATE_DEF_GAME_CLIENT_1005_DEF__


#include "ddzandroid\ddzlogic.h"
#include "gameclientframe.h"
#include "CMD_Land.h"

//游戏客户端
class CGameClient : public CGameClientFrame
{
public:
	CGameClient(void);
	virtual ~CGameClient(void);


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
	//发送扑克
	bool OnSubSendCard(const void * pBuffer, WORD wDataSize);
	//叫分过程
	bool OnSubGameUserLandScore(const void * pBuffer, WORD wDataSize);
	//叫分完成
	bool OnSubJiaoFenFin(const void * pBuffer, WORD wDataSize);
	//玩家加倍结果
	bool OnSubUserDoubleResult(const void * pBuffer, WORD wDataSize);
	//游戏开始出牌
	bool OnSubGameStartOutCard(const void * pBuffer, WORD wDataSize);
	//玩家出牌结果
	bool OnSubGameUserOutCardResult(const void * pBuffer, WORD wDataSize);
	//玩家放弃出牌
	bool OnSubGameUserPassCard(const void * pBuffer, WORD wDataSize);
	//游戏结束
	bool OnSubGameEnd(const void * pBuffer, WORD wDataSize);

public:
	//发送之前初始化
	void InitSendCard();
	//自动出牌
	void GameUserAutoOutCard();
	//自动出最后一张牌
	void GameAutoLastOneCard();
	//加倍判断
	void SetGameUserJiaBei(WORD wBankerID, WORD wCurrentID, const BYTE *pCircuit, const bool *pIsDouble);
	//取消所有定时器
	void CancelAllTimer();

private:
	//游戏逻辑
	CDDZLogic				m_GameLogic;

private:
	WORD					m_bBankerUserID;				//地主
	WORD					m_bCurrentUserID;				//当前玩家
	bool					m_bIsNewTurn;					//新一轮
	BYTE					m_bHandCardCount;				//扑克数目
	BYTE					m_bHandCardData[20];			//手上扑克
	BYTE					m_bJiaBeiTag;					//加倍标志
	BYTE					m_cbCallScorePhase;				//叫牌阶段
	bool					m_bMeCanOutCard;				//是否出牌
public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnTimer(UINT nIDEvent);
};




#endif