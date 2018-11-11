#ifndef		___CREATE_DEF_GAME_CLIENT_1005_DEF__
#define		___CREATE_DEF_GAME_CLIENT_1005_DEF__

#include "CMD_ZaJinHua.h"
#include "GameLogic.h"
#include "gameclientframe.h"
#include <vector>

using namespace std;

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
	//游戏开始
	bool OnSubGameStart(const void * pBuffer, WORD wDataSize);
	//用户放弃
	bool OnSubGiveUp(const void * pBuffer, WORD wDataSize);
	//用户加注
	bool OnSubAddScore(const void * pBuffer, WORD wDataSize);
	//用户看牌
	bool OnSubLookCard(const void * pBuffer, WORD wDataSize);
	//用户比牌
	bool OnSubCompareCard(const void * pBuffer, WORD wDataSize);
	//用户开牌
	bool OnSubOpenCard(const void * pBuffer, WORD wDataSize);
	//游戏结束
	bool OnSubGameEnd(const void * pBuffer, WORD wDataSize);

private:
	//发送之前初始化
	void InitGameData();
	//取消所有定时器
	void CancelAllTimer();
	//第一次下注
	void FirstXZ(bool bIsFirst=true);
	//玩家操作
	void MeOperator();
	//是否可以开牌
	bool IsOpenCard();
	//比牌操作
	void UserCompareCard();
	//看牌后的操作
	void LookCardLaterOpera();
	//获取手牌内最大点数
	int GetCardMaxPoint();
	//跟注
	void FollowCard();
	//自动判断
	void AutoJudge();

private:
	//游戏逻辑
	CGameLogic						m_GameLogic;

private:
	//当前用户
	WORD							m_wCurrentUser;
	//庄家用户
	WORD							m_wBankerUser;
	//当局赢家
	WORD							m_wWinnerUser;				
	//玩家个数
	WORD							m_wPlayerNums;
	//封顶数目
	LONGLONG						m_lMaxScore;
	//单元上限
	LONGLONG						m_lMaxCellScore;
	//单元下注
	LONGLONG						m_lCellScore;	
	//当前倍数
	LONGLONG						m_lCurrentTimes;
	//最大分数
	LONGLONG						m_lUserMaxScore;
	//下注次数
	WORD							m_wUserXZCount[GAME_PLAYER];
	//玩家下注值
	LONGLONG						m_lTableScore[GAME_PLAYER];
	//是否看牌
	bool							m_bIsLookCard[GAME_PLAYER];
	//是否有效玩家
	bool							m_cbPlayStatus[GAME_PLAYER];
	//玩家手牌
	BYTE							m_byUserCard[MAX_COUNT];
	//看牌的玩家列表
	std::vector<int>				m_vecLookCardUserList;
	//输牌或放弃玩家列表
	std::vector<int>				m_vecLostUserList;
	std::vector<int>				m_vecWinUserList;
	std::vector<int>				m_vecCancelUserList;
	//连续跟注次数
	WORD							m_wContinuouFollowNums;
	//看牌以后不操作
	bool							m_bIsLookCardLaterNoOperation;
	//跟到开牌
	bool							m_bIsFollowToOpenCard;
	//玩家是否有动作
	bool							m_bIsUserAction;

public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnTimer(UINT nIDEvent);
};




#endif