#ifndef __GAMELOGIC_H__
#define __GAMELOGIC_H__

#include <vector>
#include <list>
#include <map>
#include "DG4.h"
#include "../XY/CMD_DG4.h"
#include "TableFrameSink.h"

namespace DG4
{
//#define INVALID_CHAIR 0xFFFF

// 游戏状态
typedef enum tagGameState
{
	gsNull = 0,
	gsRoarPai,	// 吼牌
	gsPlay,		// 打牌
}GameState;

class CGameLogic : public CTableFrameSink
{
	//////////////////////////////////////////////////////////////////////////
	// 构造
public:
	CGameLogic(void);
	virtual ~CGameLogic(void);

	//////////////////////////////////////////////////////////////////////////
	// 游戏接口
	//定时器事件
	virtual bool __cdecl OnTimerMessage(WORD wTimerID, WPARAM wBindParam);

	//框架判定游戏结束
	virtual bool __cdecl OnEventGameEnd(WORD wChairID, IServerUserItem * pIServerUserItem, unsigned char cbReason);

	virtual bool __cdecl SendGameScene(WORD wChiarID, IServerUserItem * pIServerUserItem, unsigned char cbGameStatus, bool bSendSecret);
	
	//用户断线
	virtual bool __cdecl OnActionUserOffLine(WORD wChairID, IServerUserItem * pIServerUserItem); 
	//用户重入
	virtual bool __cdecl OnActionUserReConnect(WORD wChairID, IServerUserItem * pIServerUserItem);

public:								
	// 游戏开始
	void OnStartGame();	
	void OnGameAction(WORD wSubCmdID, const void * pDataBuffer, WORD wDataSize);
	

	// 清空所有状态与数据,此函数请谨慎调用。
	void Reset();
	void InitGame();
	long GetCPUTickCount( void );

public:
	/*一些纸牌游戏共有的服务器数据*/
	enum ActionStep		// 游戏步骤
	{//修改步骤
		AS_NULL = 0,
		AS_ROAR,		// 硬牌
		AS_ENDROAR,		// 结束硬牌
		AS_PLAY,		// 打牌
		AS_ENDPLAY,		// 打牌结束
		AS_ENDGAME,		// 结束游戏
		AS_COUNT
	};
	enum TGameType
	{
		GT_NULL,
		GT_NORMAL,		//普通模式，一般为找朋友，就是2vs2,这个为取分数
		GT_ROAR,		//吼牌模式，一般为1vs3,这个为争上游
	};
	enum GameOverType
	{
		GOT_NULL = 0,
		GOT_ESCAPE,		//有人逃跑，游戏结束
		GOT_NORMAL,		//正常的结束游戏，即：在找朋友的模式下，三家走完，在吼牌模式下，一家走完
		GOT_ZHONGTU,    //中途结束游戏，即满足一定的条件，结束了游戏
		GOT_DOUBLEKILL,	//“绑”，意为双杀，在找朋友的模式下，一方为一游和二游走完
	};

	std::string m_helpUrl;				// 帮助网址
	std::string m_purchaseUrl;			// 充值网址
	std::string m_shopUrl;				// 商城网址
	std::string m_ADUrl;				// 广告网址

	GameState	m_GameState;			// 游戏状态
	int			m_tDownTime;			// 权限停止时间
	int			m_tPowerStartTime;		// 权限开始时间
	ActionStep	m_theActionStep;		// 倒计时
	ActionStep	m_nowActionStep;	

	//一些共有的游戏数据

	unsigned char		m_whoplay;              // 当前玩家	1--MAXPLAYER
	unsigned char		m_whoplayout;			// 上一个玩家
	unsigned char		m_banker;				// 庄家
	unsigned char		m_nextbanker;			// 下一个庄家

	unsigned char		m_whoMakeCards;			//谁做牌			

	//托管数据
	unsigned char        m_TuoGuanPlayer[MAX_PLAYER];	  // 谁托管了？
	unsigned char		m_TrustCounts[MAX_PLAYER];		  // 托管次数

	//断线数据
	unsigned char        m_whoBreak[MAX_PLAYER];			  // 谁断线了？
	int			m_WireBreakCounts[MAX_PLAYER];	  // 断线次数

	//逃跑数据
	int			m_EscapePlayerNum;			      // 逃跑的玩家数
	BOOL		m_isPlayerEscape[MAX_PLAYER];     // 谁逃跑了？
	long		GameWinLostSR[MAX_PLAYER];	      // 逃跑玩家输赢
	long		GameWinLostJF[MAX_PLAYER];	      // 逃跑玩家输赢

	//牌数据
	unsigned char		m_tempdbp[ALLCARD];				  // 所有牌 做牌所用
	unsigned char		m_dbp[ALLCARD];					  // 所有牌
	unsigned char		m_dbplay[MAX_PLAYER][MAXCARD];    // 分到的牌

	unsigned char        m_allPaiOut[MAX_PLAYER][MAXCARD]; // 出的牌
	unsigned char        m_lastPaiOut[MAX_PLAYER][MAXCARD];// 上一轮出的牌
	unsigned char		m_thePaiCount[MAX_PLAYER];		  // 牌数

	//银子，积分数据
	int			m_lastScore[MAX_PLAYER];		  // 上一轮输赢
	int			m_total[MAX_PLAYER];			  // 总输赢
	long		m_playerrich[MAX_PLAYER];		  // 玩家的财富

	//一些共有的规则
	int			m_nAutoOutTime;					// 自动出牌时间，托管的时候出牌时间
	int			m_nTimeStart;					// 坐下之后，按开始的等待时间
	int			m_nPlayTime;					// 出牌时间
	int			m_nSerPay;						// 茶水费千分比
	int			m_nVipSerPay;					// vip茶水费千分比

	int			m_nIsSysTrust;					// 是否是托管游戏
	int			m_nWireBreakTimes;				// 允许断线重连的次数
	int			m_nTrustCounts;					// 允许托管的次数

	int			m_nConditionEnter;				// 是否有进入房间的条件
	int			m_nEnter;						// 进入房间的条件
	int			m_nConditionSit;				// 是否有做小的条件
	int			m_nPlayerSit;					// 坐下的条件

	time_t	    nowTime; 

	// 设置多少秒后进入指定动作
	void		SetActionStep(ActionStep as,unsigned long nSecond=0);  

	// 立即进入指定动作,
	void		OnActionStep(ActionStep as);
public://自己添加的
	long		m_nBase;					// 底
	long		m_nNeedMinrich;				// 需要玩的最少的财富
	int			m_isSpecial;				// 是否为方言
	int			m_theGift;					// 是否允许领奖
	std::string	m_szGiftUrl;				// 领奖地址
	std::string m_szGiftCall;				// 领奖电话
	std::string m_szDURL;					// 底部广告

	int			m_nRoarTime;				// 吼牌时间
	CDG4		m_dagun;

	bool		m_bMingJi;					// 是否已经显示明鸡了
	TGameType	m_GameType;					// 游戏的类型
	int			m_score;					// 每一轮牌的分
	unsigned char			m_turn;						// 次序
	unsigned char		m_RoarPai;					// 叫的牌
	unsigned char		m_WhoRoar;					// 谁吼牌了？
	unsigned char		m_parter;					// 庄家的对家
	int			m_playerscore[MAX_PLAYER];	// 每个人最后的分数，有可能没出完的也加进去了

	long		m_spay[MAX_PLAYER];			// 服务费

	unsigned char		m_whoPass[MAX_PLAYER];			// 谁放弃了
	BOOL		m_playerWin[MAX_PLAYER];		// 谁出完了？
	BOOL		m_StartMakePai;					// 做牌功能开关

	unsigned char		m_bTeamOut[MAX_PLAYER];			// 我的对家是否走完了？
	unsigned char        m_whoAllOutted[MAX_PLAYER];     // 谁出完了，
	unsigned char			m_playerTurn[MAX_PLAYER];
	bool		m_whoReady[MAX_PLAYER];			// 谁已经完成了吼牌过程
	int			m_who8Xi[MAX_PLAYER];			// 每个人的8喜个数
	int			m_who7Xi[MAX_PLAYER];			// 每个人的7喜个数
	int			m_whoHasKingBomb;				// 谁有天炸
	int			m_lastOutType;					// 最后出牌的类型


	int			m_iFaOfTao;				//逃跑的惩罚倍数
	int			m_iJiangOfTao;			//逃跑的补偿倍数


	int			m_iRandomTask;						//随机任务,下标
	vector<int>	m_vecAllTaskID;	//所有的任务ID
	map<int,int>	m_mapAllTask;	//任务描述: ID-奖励数
	vector<int>	m_vecFinishedTask[MAX_PLAYER];		//完成的任务
	bool		m_bHaveOutCard;		//是否已经出牌了
	int			m_specialAward; 				// 特殊牌型奖励开关
public:

	unsigned char		GetTeamer(unsigned char who);			// who = 1-4
	int			GetTeamScore(unsigned char seat);		// 得到队伍确定的分数

	void		StartRoar(unsigned char theSeat);		// 开始吼牌
	void		EndRoar(bool bRoar);			// 结束吼牌

	void GoNextPlayer();
	void StartPlay(unsigned char whoplay);		// 开始玩
	int AutoOutCard(unsigned char theSeat,const DG4::CMD_OutCard_CB& msgout);	//出牌动作

	unsigned char		GetJiaoPai();														// 得到叫牌
	void		GameOver(unsigned char winner,GameOverType type);
	bool		JudgeEndGame(unsigned char who);
	void		OnRoarAction(unsigned char seat,unsigned char bRoar);									// 进入硬牌动作

	//修改下面的函数来实现自动托管
	int			AutoTuoGuan(unsigned char theSeat);

	// 结束一轮
	bool EndTurn();
	void EndOut();
	void AddSpecailScore(long Score[MAX_PLAYER],int seat,int base);
	void GetFinalWinLoseScore( long score[MAX_PLAYER] );
	void SendPower(unsigned char whoplay,DWORD aPower,DWORD dwWaitTime = 0);	// 发送权限
	void SendPlaySoundMsg(unsigned char seat,unsigned char type);						// 发送播放音乐的信息

	//int MsgTuoGuan();												//托管

	void SendTurnScore(int wChairID = INVALID_CHAIR);				// 发送每轮的分数
	void SendPlayerScore(int wChairID = INVALID_CHAIR);				// 发送每个人的分数
	void SendPaiCount(int wChairID = INVALID_CHAIR);				// 发送牌数
	void SendPlayerTurn(int wChairID = INVALID_CHAIR);				// 发送玩家结束顺序
	void SendPaiToTeamer(unsigned char seat1,unsigned char seat2);					// 把seat1的牌发送给seat2显示出来
	void sendGameStatus(WORD wChair ,bool isReLink);

	BOOL IsGamePlaying();


	//void stovs( const string& s, const string& sp, vector<string>& sa );
	void ParseTaskConfig(const string& strTaskID);	//解析配置的任务
	void SendTaskID (bool bFirst, unsigned char nSeat = -1); //发送随机任务
	void IsTaskFinished(const TCardType& re1, int iSeat, int iNumOfKing);  //判断任务是否完成
	void IsTaskFinishedOfLastHand(const TCardType& re1, int iSeat, int iNumOfKing);  //判断最后一手的任务是否完成
	void SendFinishTask (int iSeat, int id, int cnt = 1, bool bToAll = true); //发送完成了的任务
	int GetTaskAward(int iSeat); //获取奖励数
	string GetTaskAwardDetails(int iSeat);	//获取奖励详细
};

}; //end of namespace
#endif//__GAMELOGIC_H__

