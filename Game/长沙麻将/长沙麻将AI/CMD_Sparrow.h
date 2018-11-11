#ifndef CMD_SPARROW_HEAD_FILE
#define CMD_SPARROW_HEAD_FILE

//////////////////////////////////////////////////////////////////////////
//公共宏定义

#define KIND_ID						303									//游戏 I D
#define GAME_PLAYER					4									//游戏人数
#define GAME_NAME					TEXT("长沙麻将")					//游戏名字
#define GAME_GENRE					(GAME_GENRE_SCORE|GAME_GENRE_MATCH)	//游戏类型

//游戏状态
#define GS_MJ_FREE					GS_FREE								//空闲状态
#define GS_MJ_PLAY					(GS_PLAYING+1)						//游戏状态
#define GS_MJ_HAI_DI				(GS_PLAYING+2)						//海底状态

//////////////////////////////////////////////////////////////////////////

//组合子项
struct CMD_WeaveItem
{
	BYTE							cbWeaveKind;						//组合类型
	BYTE							cbCenterCard;						//中心扑克
	BYTE							cbPublicCard;						//公开标志
	WORD							wProvideUser;						//供应用户
};

//////////////////////////////////////////////////////////////////////////
//服务器命令结构

#define SUB_S_GAME_START			100									//游戏开始
#define SUB_S_OUT_CARD				101									//出牌命令
#define SUB_S_SEND_CARD				102									//发送扑克
#define SUB_S_OPERATE_NOTIFY		103									//操作提示
#define SUB_S_OPERATE_RESULT		104									//操作命令
#define SUB_S_OPERATE_HAI_DI		105									//海底操作
#define SUB_S_GAME_END				106									//游戏结束
#define SUB_S_TRUSTEE				107									//用户托管

//游戏状态
struct CMD_S_StatusFree
{
	LONGLONG						lCellScore;							//基础金币
	WORD							wBankerUser;						//庄家用户
	bool							bTrustee[GAME_PLAYER];				//是否托管

};

//游戏状态
struct CMD_S_StatusPlay
{
	//游戏变量
	LONGLONG						lCellScore;							//单元积分
	WORD							wSiceCount;							//骰子点数
	WORD							wBankerUser;						//庄家用户
	WORD							wCurrentUser;						//当前用户
	bool							bTrustee[GAME_PLAYER];				//是否托管

	//状态变量
	BYTE							cbHearStatus;						//听牌状态
	BYTE							cbActionCard;						//动作扑克
	BYTE							cbActionMask;						//动作掩码
	BYTE							cbLeftCardCount;					//剩余数目

	//出牌信息
	WORD							wOutCardUser;						//出牌用户
	BYTE							cbOutCardData;						//出牌扑克
	BYTE							cbDiscardCount[4];					//丢弃数目
	BYTE							cbDiscardCard[4][55];				//丢弃记录

	//扑克数据
	BYTE							cbCardCount;						//扑克数目
	BYTE							cbCardData[14];						//扑克列表

	//组合扑克
	BYTE							cbWeaveCount[4];					//组合数目
	CMD_WeaveItem					WeaveItemArray[4][4];				//组合扑克
	bool                            m_bTingCard;                         //能否听牌
	BYTE							byGangCard[4];						//杠牌
	BYTE							byGangCardNums;						//个数
	BYTE							byBuCard[4];						//补牌
	BYTE							byBuCardNums;						//个数
};

//游戏开始
struct CMD_S_GameStart
{
	WORD							wSiceCount;							//骰子点数
	WORD							wBankerUser;						//庄家用户
	WORD							wCurrentUser;						//当前用户
	BYTE							cbUserAction;						//用户动作
	BYTE							cbCardData[14*GAME_PLAYER];			//扑克列表
	bool							bTrustee[GAME_PLAYER];						//是否托管

	BYTE							byGangCard[4];						//杠牌
	BYTE							byGangCardNums;						//个数
	BYTE							byBuCard[4];						//补牌
	BYTE							byBuCardNums;						//个数
};

//出牌命令
struct CMD_S_OutCard
{
	WORD							wOutCardUser;						//出牌用户
	BYTE							cbOutCardData;						//出牌扑克
	CMD_S_OutCard()
	{
		wOutCardUser = -1;
		cbOutCardData = -1;
	}
};

//发送扑克
struct CMD_S_SendCard
{
	BYTE							cbCardData;							//扑克数据
	BYTE							cbActionMask;						//动作掩码
	WORD							wCurrentUser;						//当前用户
	BYTE							byGangCard[4];						//杠牌
	BYTE							byGangCardNums;						//个数
	BYTE							byBuCard[4];						//补牌
	BYTE							byBuCardNums;						//个数
};

//操作提示
struct CMD_S_OperateNotify
{
	WORD							wResumeUser;						//还原用户
	BYTE							cbActionMask;						//动作掩码
	BYTE							cbActionCard;						//动作扑克
	BYTE							byGangCard[4];						//杠牌
	BYTE							byGangCardNums;						//个数
	BYTE							byBuCard[4];						//补牌
	BYTE							byBuCardNums;						//个数
};

//操作命令
struct CMD_S_OperateResult
{
	WORD							wOperateUser;						//操作用户
	WORD							wProvideUser;						//供应用户
	BYTE							cbOperateCode;						//操作代码
	BYTE							cbOperateCard;						//操作扑克
};

//海底操作
struct CMD_S_OperateHaiDi
{
	WORD							wCurrentUser;						//当前用户
};

//游戏结束
struct CMD_S_GameEnd
{
	BYTE							cbBirdCard;							//扎鸟扑克
	BYTE							cbChiHuCard;						//吃胡扑克
	BYTE							cbHaiDiCard;						//海底扑克
	WORD							wProvideUser;						//点炮用户
	WORD							wBirdHitUser;						//扎鸟用户
	LONGLONG						lGameScore[4];					    //游戏积分
	WORD							wChiHuKind[4];						//胡牌类型
	WORD                            wChiHuRight[4];                     //胡牌权位
	BYTE							cbCardCount[4];						//扑克数目
	BYTE							cbCardData[4][14];					//扑克数据

	bool							bIsHuangzhuang;						//是否荒庄
	int                             cbTimes[GAME_PLAYER];               //玩家番数
	int                             iCellScore;                         //房间倍率（单元积分）
	bool                            IsUserLeft[GAME_PLAYER];            //用户是否强退
	int                             iChiHuKind[GAME_PLAYER];           //胡牌类型（0：没胡；1：自摸；2：胡牌；3：放炮） 
	int                             iChiHuType;                         //胡牌类型
};
//用户托管
struct CMD_S_Trustee
{
	bool							bTrustee;							//是否托管
	WORD							wChairID;							//托管用户
};
//////////////////////////////////////////////////////////////////////////
//客户端命令结构

#define SUB_C_OUT_CARD				1									//出牌命令
#define SUB_C_OPERATE_CARD			2									//操作扑克
#define SUB_C_OPERATE_HAI_DI		3									//海底操作
#define SUB_C_TRUSTEE				4									//用户托管

//出牌命令
struct CMD_C_OutCard
{
	BYTE							cbCardData;							//扑克数据
};

//操作命令
struct CMD_C_OperateCard
{
	BYTE							cbOperateCode;						//操作代码
	BYTE							cbOperateCard;						//操作扑克
};

//海底操作
struct CMD_C_OperateHaiDi
{
	BYTE							cbTakeHaiDi;						//获取海底
};
//用户托管
struct CMD_C_Trustee
{
	bool							bTrustee;							//是否托管	
};
//////////////////////////////////////////////////////////////////////////

#endif