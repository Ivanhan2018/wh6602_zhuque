#ifndef CMD_DG4_HEAD_FILE
#define CMD_DG4_HEAD_FILE

#include "biostream.h"

namespace DG4
{
//////////////////////////////////////////////////////////////////////////
//公共宏定义

#define KIND_ID							999								    //游戏 I D
#define GAME_PLAYER						4									//游戏人数
#define GAME_NAME						TEXT("DG4")					        //游戏名字
#define GAME_GENRE						(GAME_GENRE_GOLD|GAME_GENRE_MATCH)	//游戏类型

///////////////////////////////////
#define ALLCARD							108
#define MAXCARD							27
#define MAX_PLAYER						4					
//////////////////////////////////////////////////////////////////////////
//服务器命令结构
#define SUB_S_GAME_START				100									//游戏开始
#define SUB_S_TIMER_START				101									//开始倒计时
#define SUB_S_POWER						102									//权限
#define SUB_S_ENDROAR					105									//结束吼牌
#define SUB_S_RELINK					106									//重连
#define SUB_S_ENDOUT					107									//结束一轮
#define SUB_S_TEAMERPAI					108									//发送队友的牌
#define SUB_S_PLAYERSCORE				109									//发送每个人得的分数
#define SUB_S_SENDCOUNT					110									//发送牌数
#define SUB_S_TURNSCORE					111									//发送局分
#define SUB_S_SENDTURN					112									//发送顺序
#define SUB_S_ENDGAME					113									//游戏结束
#define SUB_S_SHOW						114									//显示叫的牌或者其他信息

#define SUB_CS_OUT						115									//出牌
#define SUB_CS_ROAR						116									//吼牌
#define SUB_S_TUOGUAN					117									//托管
#define SUB_CS_PLAYSOUND				118									//播放声音
#define SUB_CS_TEAMERPAI				119									//队友的牌
#define SUB_CS_GAMERULE					120									//底银
#define SUB_C_RELINK_TIP				121			        //断线重连提示，提示其它三个人

#define SUB_CS_RAND_TASK_ID				122									//游戏随机任务
#define SUB_CS_RAND_FINISHED_TASK		123									//游戏随机任务完成

inline const char * GetPorkCardName(unsigned char cardid)
{
	bool bLegal =(cardid>0 && cardid<55);
	if (bLegal)
	{
		static const char * szCardName[54] = {
				"方块A", "方块2", "方块3", "方块4", "方块5", "方块6", "方块7", "方块8", "方块9", "方块10", "方块J", "方块Q", "方块K",
				"梅花A", "梅花2", "梅花3", "梅花4", "梅花5", "梅花6", "梅花7", "梅花8", "梅花9", "梅花10", "梅花J", "梅花Q", "梅花K",
				"红心A", "红心2", "红心3", "红心4", "红心5", "红心6", "红心7", "红心8", "红心9", "红心10", "红心J", "红心Q", "红心K",
				"黑桃A", "黑桃2", "黑桃3", "黑桃4", "黑桃5", "黑桃6", "黑桃7", "黑桃8", "黑桃9", "黑桃10", "黑桃J", "黑桃Q", "黑桃K",
				"小王", "大王",
		};
		return szCardName[cardid-1];
	}
	else
		return "";
}

//游戏状态
struct CMD_S_StatusFree
{
	unsigned char								cbStatus;							//当前状态
	int								lCellMinScore;						//最小下注
	int								lCellMaxScore;						//最大下注
};


//游戏状态
struct CMD_S_StatusPlay
{
	unsigned char								cbStatus;							//当前状态
	int								lCellMinScore;						//最小下注
	int								lCellMaxScore;						//最大下注
	//标志变量
	//unsigned char								bShowHand[GAME_PLAYER];				//梭哈标志
	//unsigned char								bAddScore;							//加注标志

	//加注信息
	int								lCellScore;							//单元下注
	int								lTurnMaxScore;						//最大下注
	int								lTurnLessScore;						//最小下注
//	int								lCellMaxScore;						//最大下注
	int								lAddLessScore;						//加最小注
	int								lTableScore[GAME_PLAYER];			//下注数目
	int								lTotalScore[GAME_PLAYER];			//累计下注
	int								lCenterScore;						//中心筹码

	//状态信息
	WORD								wDUser;								//D玩家
	WORD				 				wCurrentUser;						//当前玩家
	unsigned char								cbPlayStatus[GAME_PLAYER];			//游戏状态
	unsigned char							    cbBalanceCount;						//平衡次数

	//扑克信息
//	unsigned char								cbCenterCardData[MAX_CENTERCOUNT];	//扑克数目
	unsigned char						cbHandCardData[MAXCARD];			//桌面扑克
};

//发送扑克
struct CMD_S_GameStart_CB
{
	enum {S_ID = SUB_S_GAME_START};
	//theType,之前没有用到，现在增加一个枚举
	enum {GAME_START,GAME_RELINK};

	unsigned char		theIsGameStart;
	unsigned char		theBanker;					// 庄家
	unsigned char		theType;					// 类型(开始游戏或是重连）
	unsigned char		theSeat;					// 谁的牌
	unsigned char		theCount;					// 牌数
	unsigned char		theCards[MAXCARD];			// 牌数据

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_S_GameStart_CB() { reset(); }
	friend bostream& operator<<(bostream& bos,const CMD_S_GameStart_CB& pl)
	{
		bos << pl.theIsGameStart;
		bos << pl.theBanker;
		bos << pl.theType;
		bos << pl.theSeat;
		bos << pl.theCount;

		bos.write((char*)pl.theCards, sizeof(pl.theCards));

		return bos;
	}
	friend bistream& operator>>(bistream& bis,CMD_S_GameStart_CB& pl)
	{	
		pl.reset();
		bis >> pl.theIsGameStart;
		bis >> pl.theBanker;
		bis >> pl.theType;
		bis >> pl.theSeat;
		bis >> pl.theCount;

		bis.read((char*)pl.theCards, sizeof(pl.theCards));

		return bis;
	}
};

// 权限
struct CMD_S_Power_CB
{
	enum { XY_ID = SUB_S_POWER };

	enum tagPower
	{
		p_Null   = 0x00,
		p_Roar	 = 0x01,
		p_Out	 = 0x02,		// 出牌权限
	};

	unsigned char		theWhoPlay;		// 谁的权限
	unsigned int		theDownTime;	// 定时器时间（单位秒）
	unsigned int		thePower;		// 什么权限

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_S_Power_CB() { reset(); }
	friend bostream& operator<<(bostream& bos,const CMD_S_Power_CB& pl)
	{
		bos << pl.theWhoPlay;
		bos << pl.theDownTime;
		bos << pl.thePower;

		return bos;
	}
	friend bistream& operator>>(bistream& bis,CMD_S_Power_CB& pl)
	{	
		pl.reset();
		bis >> pl.theWhoPlay;
		bis >> pl.theDownTime;
		bis >> pl.thePower;

		return bis;
	}
};

// 结束吼牌
struct CMD_S_EndRoar_CB
{
	enum { XY_ID = SUB_S_ENDROAR };

	unsigned char		theWhoRoar; // 1-4,0表示没有吼牌
	unsigned char       theBanker;	// 庄家
	unsigned char		thePai;		// 叫牌

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_S_EndRoar_CB() { reset(); }
	friend bostream& operator<<(bostream& bos,const CMD_S_EndRoar_CB& pl)
	{
		bos << pl.theWhoRoar;
		bos << pl.theBanker;
		bos << pl.thePai;

		return bos;
	}
	friend bistream& operator>>(bistream& bis,CMD_S_EndRoar_CB& pl)
	{	
		pl.reset();
		bis >> pl.theWhoRoar;
		bis >> pl.theBanker;
		bis >> pl.thePai;

		return bis;
	}
};

// 游戏结束
struct CMD_S_EndGame_CB
{
	enum {XY_ID = SUB_S_ENDGAME}; 
	enum{
		TY_NULL,
		TY_SCORE,		// 打分模式，找朋友
		TY_ROAR,		// 吼牌模式，争上游
		TY_ESCAPE,		// 有人逃跑
	};

	unsigned char	theEndType;	    // 游戏模式	
	unsigned char	theDoubleKill;	// 是否被双扣？
	unsigned char	theQing;		// 是否清
	unsigned char	theGuang;		// 是否光头
	unsigned char	theWhoKingBomb; // 谁有天炸?
	int			theFan;			// 多少番

	unsigned char	theLeftPai[MAX_PLAYER][MAXCARD];//剩余牌
	unsigned char	theWinLose[MAX_PLAYER];			// 输还是赢？0输，1赢，2平局
	unsigned char	the7Xi[MAX_PLAYER];				// 7喜炸弹个数
	unsigned char	the8Xi[MAX_PLAYER];				// 8喜炸弹个数
	unsigned char	the510K[MAX_PLAYER];			// 510K的个数   
	long long		theScore[MAX_PLAYER];			// 得分

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_S_EndGame_CB() { reset(); }
	friend bostream& operator<<(bostream& bos,const CMD_S_EndGame_CB& pl)
	{
		bos << pl.theEndType;
		bos << pl.theDoubleKill;
		bos << pl.theQing;
		bos << pl.theGuang;
		bos << pl.theWhoKingBomb;
		bos << pl.theFan;

		bos.write((char*)pl.theLeftPai, sizeof(pl.theLeftPai));
		bos.write((char*)pl.theWinLose, sizeof(pl.theWinLose));
		bos.write((char*)pl.the7Xi, sizeof(pl.the7Xi));
		bos.write((char*)pl.the8Xi, sizeof(pl.the8Xi));
		bos.write((char*)pl.the510K, sizeof(pl.the510K));
		bos.write((char*)pl.theScore, sizeof(pl.theScore));

		return bos;
	}
	friend bistream& operator>>(bistream& bis,CMD_S_EndGame_CB& pl)
	{	
		pl.reset();
		bis >> pl.theEndType;
		bis >> pl.theDoubleKill;
		bis >> pl.theQing;
		bis >> pl.theGuang;
		bis >> pl.theWhoKingBomb;
		bis >> pl.theFan;

		bis.read((char*)pl.theLeftPai, sizeof(pl.theLeftPai));
		bis.read((char*)pl.theWinLose, sizeof(pl.theWinLose));
		bis.read((char*)pl.the7Xi, sizeof(pl.the7Xi));
		bis.read((char*)pl.the8Xi, sizeof(pl.the8Xi));
		bis.read((char*)pl.the510K, sizeof(pl.the510K));
		bis.read((char*)pl.theScore, sizeof(pl.theScore));

		return bis;
	}
};

// 权限
struct CMD_S_Show_CB
{
	enum { XY_ID = SUB_S_SHOW };

	enum{
		F_NULL,
		F_JIAOPAI,				// 叫牌
		F_MINGJI,				// 明鸡
	};
	unsigned char		theFlag;
	unsigned char		thePai;
	unsigned char		theSeat;

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_S_Show_CB() { reset(); }
	friend bostream& operator<<(bostream& bos,const CMD_S_Show_CB& pl)
	{
		bos << pl.theFlag;
		bos << pl.thePai;
		bos << pl.theSeat;

		return bos;
	}
	friend bistream& operator>>(bistream& bis,CMD_S_Show_CB& pl)
	{	
		pl.reset();
		bis >> pl.theFlag;
		bis >> pl.thePai;
		bis >> pl.theSeat;

		return bis;
	}
};

// 结束一轮
struct CMD_S_EndOut_CB
{
	enum {XY_ID = SUB_S_ENDOUT}; 

	unsigned char theWhoPlay;

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_S_EndOut_CB() { reset(); }
	friend bostream& operator<<(bostream& bos,const CMD_S_EndOut_CB& pl)
	{
		bos << pl.theWhoPlay;

		return bos;
	}
	friend bistream& operator>>(bistream& bis,CMD_S_EndOut_CB& pl)
	{	
		pl.reset();
		bis >> pl.theWhoPlay;

		return bis;
	}
};

//////////////////////////////////////////////////////////////////////////
//客户端命令结构


// 吼牌
struct CMD_Roar_CB
{
	enum { XY_ID = SUB_CS_ROAR};
	unsigned char		theSeat;
	unsigned char		theFlag;

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_Roar_CB() { reset(); }
	friend bostream& operator<<(bostream& bos,const CMD_Roar_CB& pl)
	{
		bos << pl.theSeat;
		bos << pl.theFlag;

		return bos;
	}
	friend bistream& operator>>(bistream& bis,CMD_Roar_CB& pl)
	{	
		bis >> pl.theSeat;
		bis >> pl.theFlag;

		return bis;
	}
};

typedef struct TFakeKing_CB
{
	unsigned char        index;		//原来的值
	unsigned char		fakeindex;	//替代值	

	void reset() { memset(this, 0, sizeof(*this)); }
	TFakeKing_CB() { reset(); }
	friend bostream& operator<<(bostream& bos,const TFakeKing_CB& pl)
	{
		bos << pl.index;
		bos << pl.fakeindex;

		return bos;
	}
	friend bistream& operator>>(bistream& bis,TFakeKing_CB& pl)
	{	
		bis >> pl.index;
		bis >> pl.fakeindex;

		return bis;
	}
}TFakeKing;

//出牌
struct CMD_OutCard_CB
{
	enum {S_ID = SUB_CS_OUT};

	unsigned char		theSeat;					// 谁出的牌
	unsigned char		theCount;					// 牌数
	unsigned short		theNoSound;					// 是否禁止本身的声音？
	// add 2010-01-06,这里之所以要加个牌型，是因为跟的牌里面，很可能是多重牌型，单纯判断，是不可靠的
	// 比如A走了334455,B走了44王王王5，C无法判断B走的是444555呢，还是445566，默认是把他识别为444555,会出现bug
	unsigned char		thePaiType; 
	unsigned char		thePai[MAXCARD];		// 扑克
	TFakeKing			fakeking[4];

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_OutCard_CB() { reset(); }
	friend bostream& operator<<(bostream& bos,const CMD_OutCard_CB& pl)
	{
		bos << pl.theSeat;
		bos << pl.theCount;
		bos << pl.theNoSound;
		bos << pl.thePaiType;

		bos.write((char*)pl.thePai, sizeof(pl.thePai));
		bos.write((char*)pl.fakeking, sizeof(pl.fakeking));

		return bos;
	}
	friend bistream& operator>>(bistream& bis,CMD_OutCard_CB& pl)
	{	
		bis >> pl.theSeat;
		bis >> pl.theCount;
		bis >> pl.theNoSound;
		bis >> pl.thePaiType;

		bis.read((char*)pl.thePai, sizeof(pl.thePai));
		bis.read((char*)pl.fakeking, sizeof(pl.fakeking));

		return bis;
	}
};

// 托管
struct CMD_S_Tuoguan_CB
{
	enum { XY_ID = SUB_S_TUOGUAN };

	enum
	{
		T_NULL		= 0x00,
		T_TUOGUAN	= 0x01,
		T_CANCEL	= 0x02,
	};

	unsigned char theSeat;
	unsigned char theFlag;		// 托管标志
	unsigned int theTime;	// 定时器时间（单位秒）

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_S_Tuoguan_CB() { reset(); }
	friend bostream& operator<<(bostream& bos,const CMD_S_Tuoguan_CB& pl)
	{
		bos << pl.theSeat;
		bos << pl.theFlag;
		bos << pl.theTime;

		return bos;
	}
	friend bistream& operator>>(bistream& bis,CMD_S_Tuoguan_CB& pl)
	{	
		pl.reset();
		bis >> pl.theSeat;
		bis >> pl.theFlag;
		bis >> pl.theTime;

		return bis;
	}
};

// 播放声音
struct CMD_CS_msgPlaySound_CB
{
	enum{XY_ID = SUB_CS_PLAYSOUND};
	enum
	{
		TY_KING_BEEP = 1,		//拆开天炸的提示
		TY_8Xi_BEEP,
		TY_7Xi_BEEP,
		TY_JieFeng,				//接风
		TY_ChaDi,				//插底
	};
	unsigned char thePaiType;
	unsigned char theSeat;
	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_CS_msgPlaySound_CB() { reset(); }
	friend bostream& operator<<(bostream& bos,const CMD_CS_msgPlaySound_CB& pl)
	{
		bos << pl.thePaiType;
		bos << pl.theSeat;

		return bos;
	}
	friend bistream& operator>>(bistream& bis,CMD_CS_msgPlaySound_CB& pl)
	{	
		pl.reset();
		bis >> pl.thePaiType;
		bis >> pl.theSeat;

		return bis;
	}
};

//本轮分
struct CMD_CS_TurnScore_CB
{
	enum { XY_ID = SUB_S_TURNSCORE };

	int theScore; //抓分

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_CS_TurnScore_CB() { reset(); }
	friend bostream& operator<<(bostream& bos,const CMD_CS_TurnScore_CB& pl)
	{
		bos << pl.theScore;
		return bos;
	}
	friend bistream& operator>>(bistream& bis,CMD_CS_TurnScore_CB& pl)
	{	
		pl.reset();
		bis >> pl.theScore;
		return bis;
	}
};

struct CMD_CS_PLAYSCORE_CB
{
	enum { XY_ID = SUB_S_PLAYERSCORE };

	int theScore[MAX_PLAYER]; //抓分

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_CS_PLAYSCORE_CB() { reset(); }
	friend bostream& operator<<(bostream& bos,const CMD_CS_PLAYSCORE_CB& pl)
	{
		bos.write((char*)pl.theScore, sizeof(pl.theScore));
		return bos;
	}
	friend bistream& operator>>(bistream& bis,CMD_CS_PLAYSCORE_CB& pl)
	{	
		pl.reset();	
		bis.read((char*)pl.theScore, sizeof(pl.theScore));
		return bis;
	}
};

struct CMD_CS_SendCount_CB
{
	enum { XY_ID = SUB_S_SENDCOUNT };

	int thePaiCount[MAX_PLAYER];

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_CS_SendCount_CB() { reset(); }
	friend bostream& operator<<(bostream& bos,const CMD_CS_SendCount_CB& pl)
	{
		bos.write((char*)pl.thePaiCount, sizeof(pl.thePaiCount));
		return bos;
	}
	friend bistream& operator>>(bistream& bis,CMD_CS_SendCount_CB& pl)
	{	
		pl.reset();
		bis.read((char*)pl.thePaiCount, sizeof(pl.thePaiCount));
		return bis;
	}
};

struct CMD_CS_SendTurn_CB
{
	enum { XY_ID = SUB_S_SENDTURN };

	bool isFirst;//如果是重连，会再发一次这东西，就置为false
    unsigned char	theTurn[MAX_PLAYER];

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_CS_SendTurn_CB(){ reset();isFirst = true; }
	friend bostream& operator<<(bostream& bos,const CMD_CS_SendTurn_CB& pl)
	{
		bos<<pl.isFirst;
		bos.write((char*)pl.theTurn, sizeof(pl.theTurn));
		return bos;
	}
	friend bistream& operator>>(bistream& bis,CMD_CS_SendTurn_CB& pl)
	{	
		pl.reset();
		bis>>pl.isFirst;
		bis.read((char*)pl.theTurn, sizeof(pl.theTurn));
		return bis;
	}
};

struct CMD_CS_TeamerPai
{
	enum { XY_ID = SUB_CS_TEAMERPAI };
	unsigned char theSeat;
	unsigned char theCount;
	unsigned char thePai[MAXCARD];

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_CS_TeamerPai() { reset(); }

	friend bostream& operator<<(bostream& bos,const CMD_CS_TeamerPai& pl)
	{

		bos << pl.theSeat;
		bos << pl.theCount;

		bos.write((char*)pl.thePai, sizeof(pl.thePai));
		return bos;
	}
	friend bistream& operator>>(bistream& bis,CMD_CS_TeamerPai& pl)
	{	
		pl.reset();

		bis >> pl.theSeat;
		bis >> pl.theCount;

		bis.read((char*)pl.thePai, sizeof(pl.thePai));
		return bis;
	}
};

struct CMD_CS_GameRule
{
	
    enum { XY_ID = SUB_CS_GAMERULE };
	int bPlaySR;			// 是否是财富游戏
	int theGiveGift;		// 是否可以领奖
	int theMakePai;		// 是否做牌
	unsigned char nTrustCounts;		// 允许托管的次数	
	unsigned char theSpecial;		// 0，普通，1方言
	int theBase;			// 底
	int theMinRich;		// 最少财富，不够这么多就无法开始游戏(改为：逃跑罚倍数)
	
	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_CS_GameRule() { reset(); }

	friend bostream& operator<<(bostream& bos,const CMD_CS_GameRule& pl)
	{
		bos << pl.bPlaySR;
		bos << pl.theGiveGift;
		bos << pl.theMakePai;
		bos << pl.nTrustCounts;
		bos << pl.theSpecial;
		bos << pl.theBase;
		bos << pl.theMinRich;
		return bos;
	}
	friend bistream& operator>>(bistream& bis,CMD_CS_GameRule& pl)
	{	
		pl.reset();
		bis >> pl.bPlaySR;
		bis >> pl.theGiveGift;
		bis >> pl.theMakePai;
		bis >> pl.nTrustCounts;
		bis >> pl.theSpecial;
		bis >> pl.theBase;
		bis >> pl.theMinRich;
		return bis;
	}
};

struct CMD_CS_msgReLink
{
	enum {XY_ID = SUB_S_RELINK};
	WORD        theChair;                 //玩家的座位号，用于初始化座位号，不然转化为location会出问题
	unsigned char	    theGameState;			// 游戏状态(GameState)
	unsigned char		theBanker;				// 庄家
	unsigned char		theLeftTime;			// 剩余时间
	unsigned char		theWhoPlay;				// 当前人
	unsigned char        theWhoPlayOut;			// 上一次谁出的牌
	unsigned char		theTGTime;			    // 已经托管的次数
	unsigned char		theRoarPai;				// 叫的牌
	unsigned char		theWhoRoar;				// 谁吼了牌？
	unsigned char		theWhoMJ;				// 谁是明鸡？
	unsigned char		thePass[MAX_PLAYER];	// 谁放弃了？
	unsigned char        theWhoBreak[MAX_PLAYER];// 谁断线了
	unsigned char        theTuoGuan[MAX_PLAYER]; // 谁托管了？
	unsigned char        theLastOutCard[MAX_PLAYER][MAXCARD];//上一次出的牌
	unsigned char		theOutCard[MAX_PLAYER][MAXCARD];// 出的牌
	bool		theWhoReady[MAX_PLAYER];// 谁已经完成了叫牌过程
	int         theLast[MAX_PLAYER];	// 上一局
	int			theTotal[MAX_PLAYER];	// 总输赢
	int			theTurnScore;			// 本轮分
	unsigned char	    Score[MAX_PLAYER];      //每个玩家的得分
	int			theLastPaiType;			// 最后出牌的类型
	
	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_CS_msgReLink(){reset();}

	friend bostream& operator<<(bostream& bos,const CMD_CS_msgReLink& pl)
	{
		bos << pl.theChair;
		bos << pl.theGameState;
		bos << pl.theBanker;
		bos << pl.theLeftTime;
		bos << pl.theWhoPlay;
		bos << pl.theWhoPlayOut;
		bos << pl.theTGTime;
		bos << pl.theRoarPai;

		bos << pl.theWhoRoar;
		bos << pl.theWhoMJ;
		bos.write((char*)pl.thePass, sizeof(pl.thePass));
		bos.write((char*)pl.theWhoBreak, sizeof(pl.theWhoBreak));
		bos.write((char*)pl.theWhoBreak, sizeof(pl.theWhoBreak));
		bos.write((char*)pl.theTuoGuan, sizeof(pl.theTuoGuan));
		bos.write((char*)pl.theLastOutCard, sizeof(pl.theLastOutCard));
		bos.write((char*)pl.theOutCard, sizeof(pl.theOutCard));
		bos.write((char*)pl.theWhoReady, sizeof(pl.theWhoReady));
		bos.write((char*)pl.theLast, sizeof(pl.theLast));
		bos.write((char*)pl.theTotal, sizeof(pl.theTotal));
		bos << pl.theTurnScore;
		bos.write((char*)pl.Score, sizeof(pl.Score));
		bos << pl.theLastPaiType;
	
		return bos;
	}

	friend bistream& operator>>(bistream& bis,CMD_CS_msgReLink& pl)
	{	
		pl.reset();
		bis >> pl.theChair;
		bis >> pl.theGameState;
		bis >> pl.theBanker;
		bis >> pl.theLeftTime;
		bis >> pl.theWhoPlay;
		bis >> pl.theWhoPlayOut;
		bis >> pl.theTGTime;
		bis >> pl.theRoarPai;

		bis >> pl.theWhoRoar;
		bis >> pl.theWhoMJ;
		bis.read((char*)pl.thePass, sizeof(pl.thePass));
		bis.read((char*)pl.theWhoBreak, sizeof(pl.theWhoBreak));
		bis.read((char*)pl.theWhoBreak, sizeof(pl.theWhoBreak));
		bis.read((char*)pl.theTuoGuan, sizeof(pl.theTuoGuan));
		bis.read((char*)pl.theLastOutCard, sizeof(pl.theLastOutCard));
		bis.read((char*)pl.theOutCard, sizeof(pl.theOutCard));
		bis.read((char*)pl.theWhoReady, sizeof(pl.theWhoReady));
		bis.read((char*)pl.theLast, sizeof(pl.theLast));
		bis.read((char*)pl.theTotal, sizeof(pl.theTotal));
		bis >> pl.theTurnScore;
		bis.read((char*)pl.Score, sizeof(pl.Score));
		bis >> pl.theLastPaiType;
		
		return bis;

	}

};

//提示其他玩家，我又回来了！
struct CMD_CS_msgReLinkTip
{
	enum {XY_ID = SUB_C_RELINK_TIP};
	unsigned char	theSeat;
	unsigned char	theFlag;

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_CS_msgReLinkTip(){reset();}

	friend bostream& operator<<(bostream& bos,const CMD_CS_msgReLinkTip& pl)
	{
		bos << pl.theSeat;
		bos << pl.theFlag;
		return  bos;
	}

	friend bistream& operator>>(bistream& bis,CMD_CS_msgReLinkTip& pl)
	{	
		pl.reset();
		bis >> pl.theSeat;
		bis >> pl.theFlag;

		return bis;
	}

};

//本局选中的游戏任务
struct CMD_CS_msgTaskID
{
	enum { XY_ID = SUB_CS_RAND_TASK_ID};
	unsigned char		theID;
	int			theAward;	//奖励数
	
	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_CS_msgTaskID(){reset();}

	friend bostream& operator<<(bostream& bos,const CMD_CS_msgTaskID& pl)
	{
		bos << pl.theID;
		bos << pl.theAward;
		return  bos;
	}

	friend bistream& operator>>(bistream& bis,CMD_CS_msgTaskID& pl)
	{	
		pl.reset();
		bis >> pl.theID;
		bis >> pl.theAward;

		return bis;
	}
};

//游戏任务完成状态
struct CMD_CS_msgFinishedTaskID
{
	enum { XY_ID = SUB_CS_RAND_FINISHED_TASK};
	unsigned char	theSeat;		//谁
	unsigned char	theID;			//完成了哪个任务
	unsigned char	theFlag;		//标志位：目前用1来标志是重连后发送的
	int		theCnt;			//完成了几次
	int		theTotal;		//奖励总数

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_CS_msgFinishedTaskID(){reset();}

	friend bostream& operator<<(bostream& bos,const CMD_CS_msgFinishedTaskID& pl)
	{
		bos << pl.theSeat;
		bos << pl.theID;
		bos << pl.theFlag;
		bos << pl.theCnt;
		bos << pl.theTotal;
		return  bos;
	}

	friend bistream& operator>>(bistream& bis,CMD_CS_msgFinishedTaskID& pl)
	{	
		pl.reset();
		bis >> pl.theSeat;
		bis >> pl.theID;
		bis >> pl.theFlag;
		bis >> pl.theCnt;
		bis >> pl.theTotal;

		return bis;
	}
};

}; //end of namespace

#endif

