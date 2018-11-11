#ifndef CMD_GAME_HEAD_FILE
#define CMD_GAME_HEAD_FILE
#include <d3dx9.h> 
//////////////////////////////////////////////////////////////////////////
/*全局座位号：

				1			0
-----------------------------------


-----------------------------------
				2			3

0和1同一侧，2和3同一侧

设自己的座位号为C，则D和自己在同一侧的判断为(C/2)==(D/2)

如自己的坐标为(x,y)，则传送到对方后坐标为
1.自己同侧：(x,y)
2.自己对面：(VIEW_WIDTH-x,VIEW_HEIGHT-y)

如自己的炮弹角度为Angle，则传送到对方后坐标为
1.自己同侧：Angle
2.自己对面：D3DX_PI+Angle

自己的大炮角度范围：3*D3DX_PI/2 <--> 5*D3DX_PI/2
对面的大炮角度范围：1*D3DX_PI/2 <--> 3*D3DX_PI/2

*/

//相关宏定义
#define IsSameSide(wMeChairID,wTestChairID)	(((wMeChairID)/2==(wTestChairID)/2)?true:false)
#define SwitchAngle(fAngle)	(D3DX_PI+(fAngle))
#define SwitchCoorX(nPosX)	(VIEW_WIDTH-(nPosX))
#define SwitchCoorY(nPosY)	(VIEW_HEIGHT-(nPosY))

//炮台角度
#define ME_MAX_CAN_ANGLE				(5*D3DX_PI/2)							//最大角度
#define ME_MIN_CAN_ANGLE				(3*D3DX_PI/2)							//最小角度

#define UP_MAX_CAN_ANGLE				(3*D3DX_PI/2)							//最大角度
#define UP_MIN_CAN_ANGLE				(D3DX_PI/2)								//最小角度
/////////////////////////////////////////////////////////////////////////////////
//服务定义
#define KIND_ID						601									//游戏 I D

#define GAME_NAME					TEXT("捕鱼达人")					//游戏名字

#define VIEW_WIDTH					1024								//视图宽度
#define VIEW_HEIGHT					595									//视图高度

#define TRACE_POINT_SPACE_NORMAL	5									//轨迹点距离
#define TRACE_POINT_SPACE_BIG		7									//轨迹点距离

#define CELL_SCORE_COUNT			5									//单元数目

#define MAX_SHOOT_BULLET_COUNT		5									//子弹数目

//组件属性
#define GAME_PLAYER					4									//游戏人数
#define VERSION_SERVER				PROCESS_VERSION(5,1,0)				//程序版本
#define VERSION_CLIENT				PROCESS_VERSION(5,1,0)				//程序版本

//////////////////////////////////////////////////////////////////////////////////

//鱼群种类
enum enFishKind
{
	enFishKind_1,					//鱼群种类
	enFishKind_2,					//鱼群种类
	enFishKind_3,					//鱼群种类
	enFishKind_4,					//鱼群种类
	enFishKind_5,					//鱼群种类
	enFishKind_6,					//鱼群种类
	enFishKind_7,					//鱼群种类
	enFishKind_8,					//鱼群种类
	enFishKind_9,					//鱼群种类
	enFishKind_10,					//鱼群种类
	enFishKind_11,					//鱼群种类
	enFishKind_12,					//鱼群种类
	enFishKind_No					//错误种类
};

//海底场景
enum enSeaSceneKind
{
	enSeaSceneKind_1,					//场景种类
	enSeaSceneKind_2,					//场景种类
	enSeaSceneKind_3,					//场景种类
	enSeaSceneKind_No					//错误种类
};
////////////////////////////////////////////////////////////////////////////////
//子弹种类(6.1没有这个枚举)
enum enBulletCountKind
{
	enBulletCountKind_100,
	enBulletCountKind_600,
	enBulletCountKind_800,
	enBulletCountKind_End
};

//鱼群分值
int const g_nFishScore[enFishKind_No]={1,2,4,7,10,20,30,40,50,60,100,200};

///////////////////////////////////////////////////////////////////////////////

//命令定义
enum enServerSubCmd
{
	enServerSubCmd_Begin=100,

	SUB_S_GAME_SCENE,									//场景消息
	SUB_S_TRACE_POINT,									//轨迹坐标
	SUB_S_USER_SHOOT,									//发射炮弹
	SUB_S_CAPTURE_FISH,									//捕获鱼群
	SUB_S_BULLET_COUNT,									//子弹数目
	SUB_S_COIN_COUNT,									//金币数目
	SUB_S_FISH_COUNT,									//鱼群数目
	SUB_S_CHANGE_SCENE,									//切换场景
	SUB_S_SCORE_INFO,									//分数信息
	SUB_S_SET_CELL_SCORE,								//单元积分
	SUB_S_BONUS_INFO,									//彩金信息
	SUB_S_GAME_MESSAGE,										//游戏消息

	enServerSubCmd_End
};

//消息数据包
struct CMD_S_GameMessage
{
	WORD								wMessageLength;					//消息长度
	TCHAR								szContent[1024];				//消息内容
};

//彩金信息
struct CMD_S_BonusInfo
{
	LONG							lBonus;									//彩金数目
};

//单元积分
struct CMD_S_SetCellScore
{
	WORD							wChairID;								//设置玩家
	LONG							lCellScore;								//单元积分
};

//切换场景
struct CMD_S_ChangeScene
{
	enSeaSceneKind					CureSeaSceneKind;						//当前场景
	bool							bChange;								//改变标识
	bool							bLToR;									//改变方向
};

//子弹数目
struct CMD_S_BulletCount
{
	WORD							wChairID;								//玩家椅子
	enBulletCountKind				BulletCountKind;						//子弹类型
	LONG							lBulletCount;							//子弹数目
};
////6.1子弹数目
//struct CMD_S_BulletCount
//{
//	WORD							wChairID;								//玩家椅子
//	LONG							lBulletCount;							//子弹数目
//};

//鱼群数目
struct CMD_S_CptrFishCount
{
	enFishKind						FishKind;								//鱼群类型
	WORD							wFishCount;								//鱼群数目
};

//金币数目
struct CMD_S_CoinCount
{
	WORD							wChairID;								//玩家椅子
	LONG							lCoinCount;								//金币数目
};

//场景消息
struct CMD_S_GameScene
{
	LONG							lBulletCharge[enBulletCountKind_End];	//子弹费用
	enSeaSceneKind					CureSeaSceneKind;						//当前场景
	LONG							lCellScore[CELL_SCORE_COUNT];			//单元积分
	LONG							lUserCellScore[GAME_PLAYER];			//单元积分
};
////6.1场景消息
//struct CMD_S_GameScene
//{
//	LONG							lBulletCharge;							//子弹费用
//	enSeaSceneKind					CureSeaSceneKind;						//当前场景
//	LONG							lCellScore;								//倍率
//	WORD							wCellMedal;								//单元奖牌
//};

//游戏状态
struct CMD_S_FishTrace
{
	//关键坐标
	int								nInitX[5];								//关键坐标
	int								nInitY[5];								//关键坐标
	int								nInitCount;								//坐标数目

	//鱼群属性
	enFishKind						FishKind;								//鱼群种类
	DWORD							dwFishID;								//鱼类标识
	bool							bRegular;								//规则标识
};


//发射炮弹
struct CMD_S_UserShoot
{
	WORD							wChairID;								//玩家椅子
	float							fAngle;									//发射角度
	enBulletCountKind				BulletCountKind;						//子弹类型
	BYTE							byShootCount;							//子弹数目	
};

//捕获鱼群
struct CMD_S_CaptureFish
{
	WORD							wChairID;								//玩家椅子
	DWORD							dwFishID;								//鱼群标识
	enFishKind						FishKind;								//鱼群种类
	LONG							lFishScore;								//鱼群得分
};
//////////////////////////////////////////////////////////////////////////
//命令定义

enum enClientSubCmd
{
	enClientSubCmd_Begin=200,

	SUB_C_BUY_BULLET,									//购买子弹
	SUB_C_USER_SHOOT,									//发射炮弹
	SUB_C_CAPTURE_FISH,									//捕获鱼群
	SUB_C_GET_SCORE_INFO,								//分数信息
	SUB_C_SET_PROPABILITY,								//设置概率
	SUB_C_CALC_SCORE,									//计算分数
	SUB_C_SET_CELL_SCORE,								//单元积分
	SUB_C_HIT_FISH,										//捕获鱼群

	enClientSubCmd_End
};

//单元积分
struct CMD_C_SetCellScore
{
	LONG							lCellScore;								//单元积分
};

//购买子弹
struct CMD_C_BuyBullet
{
	enBulletCountKind				BulletCountKind;						//数目种类
};

//发射炮弹
struct CMD_C_UserShoot
{
	enBulletCountKind				BulletCountKind;						//子弹类型
	BYTE							byShootCount;							//射击数目
	float							fAngle;									//发射角度
	DWORD							dwBulletID;								//子弹标识
};

//击中鱼群
struct CMD_C_HitFish
{
	DWORD							dwFishID;								//鱼群标识
	DWORD							dwBulletID;								//子弹标识
};

//设置概率
struct CMD_C_SetProbability
{
	BYTE							byCptrProbability[5][enFishKind_No];	//捕获概率
	BYTE							byError;								//错误提示
};
////捕获鱼群
//struct CMD_C_CaptureFish
//{
//	DWORD							dwFishID;								//鱼群标识
//	BYTE							byCptrProbability;						//捕获概率
//	DWORD							dwBulletID;								//子弹标识
//	BYTE							byBulletCount;							//子弹数目
//};
//////////////////////////////////////////////////////////////////////////

#endif