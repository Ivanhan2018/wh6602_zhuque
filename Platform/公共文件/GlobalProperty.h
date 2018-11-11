#ifndef GLOBAL_PROPERTY_HEAD_FILE
#define GLOBAL_PROPERTY_HEAD_FILE

////////////////////////////////////////////////////////////////////////////////////////////////

//鲜花定义
#define FLOWER_1					0
#define FLOWER_2					1
#define FLOWER_3					2
#define FLOWER_4					3
#define FLOWER_5					4
#define FLOWER_6					5
#define FLOWER_7					6
#define FLOWER_8					7
#define FLOWER_9					8
#define FLOWER_10					9
#define FLOWER_11					10

BYTE const g_FlowerTypeList[]=
{
	FLOWER_1,
	FLOWER_2,
	FLOWER_3,
	FLOWER_4,
	FLOWER_5,
	FLOWER_6,
	FLOWER_7,
	FLOWER_8,
	FLOWER_9,
	FLOWER_10,
	FLOWER_11,
};

//鲜花数目
#define FLOWER_COUNT		(sizeof(g_FlowerTypeList)/sizeof(g_FlowerTypeList[0]))//礼物数目

//////////////////////////////////////////////////////////////////////////
//宏定义
#define CHARM_EXCHANGE_RATE			50									//兑换比率
#define MAX_FLOWER_COUNT			100									//最大数目

//鲜花结构
struct tagGiftInfo
{
	TCHAR							szName[64];							//鲜花名称
	TCHAR							szAction[128];						//动作描述
	TCHAR							szQuantifier1[20];					//数量词
	TCHAR							szResult[128];						//结果描述
	TCHAR							szQuantifier2[6];					//数量词
	TCHAR							szCharmResult[128];					//结果描述
};

//鲜花定义
tagGiftInfo const g_FlowerDescribe[FLOWER_COUNT]=
{
	{TEXT("掌声"),TEXT(" 深情地赠送 "),TEXT(" 掌声1次"),TEXT("，恭喜 "),TEXT("次 "),TEXT(" 魅力值增加 %ld 点！")},
	{TEXT("香吻"),TEXT(" 深情地赠送 "),TEXT(" 香吻1个"),TEXT("，恭喜 "),TEXT("个 "),TEXT(" 魅力值增加 %ld 点！")},
	{TEXT("鲜花"),TEXT(" 深情地赠送 "),TEXT(" 鲜花1朵"),TEXT("，恭喜 "),TEXT("朵 "),TEXT(" 魅力值增加 %ld 点！")},
	{TEXT("啤酒"),TEXT(" 深情地赠送 "),TEXT(" 啤酒1杯"),TEXT("，恭喜 "),TEXT("杯 "),TEXT(" 魅力值增加 %ld 点！")},
	{TEXT("香烟"),TEXT(" 深情地赠送 "),TEXT(" 香烟1包"),TEXT("，恭喜 "),TEXT("包 "),TEXT(" 魅力值增加 %ld 点！")},
	{TEXT("钻戒"),TEXT(" 深情地赠送 "),TEXT(" 钻戒1枚"),TEXT("，恭喜 "),TEXT("枚 "),TEXT(" 魅力值增加 %ld 点！")},
	{TEXT("轿车"),TEXT(" 深情地赠送 "),TEXT(" 轿车1辆"),TEXT("，恭喜 "),TEXT("辆 "),TEXT(" 魅力值增加 %ld 点！")},
	{TEXT("别墅"),TEXT(" 深情地赠送 "),TEXT(" 别墅1座"),TEXT("，恭喜 "),TEXT("座 "),TEXT(" 魅力值增加 %ld 点！")},
	{TEXT("臭蛋"),TEXT(" 狠狠地砸了 "),TEXT(" 臭蛋1个"),TEXT("，抱歉 "),TEXT("个 "),TEXT(" 魅力值减少 %ld 点！")},
	{TEXT("砖头"),TEXT(" 狠狠地砸了 "),TEXT(" 砖头1块"),TEXT("，抱歉 "),TEXT("块 "),TEXT(" 魅力值减少 %ld 点！")},
	{TEXT("炸弹"),TEXT(" 狠狠地扔了 "),TEXT(" 炸弹1枚"),TEXT("，抱歉 "),TEXT("枚 "),TEXT(" 魅力值减少 %ld 点！")}
};

//////////////////////////////////////////////////////////////////////////////////////////////////////
//道具索引
#define PROP_DOUBLE						0									//双倍积分卡
#define PROP_FOURDOLD					1									//四倍积分卡
#define PROP_NEGAGIVE					2									//负分清零
#define PROP_FLEE						3									//清逃跑率
#define PROP_BUGLE						4									//小喇叭
#define PROP_KICK						5									//防踢卡
#define PROP_SHIELD						6									//护身符
#define PROP_MEMBER_1					7									//会员道具
#define PROP_MEMBER_2					8									//会员道具
#define PROP_MEMBER_3					9									//会员道具
#define PROP_MEMBER_4					10									//会员道具

//彭文添加
//道具类
#define PROP_USER_LOW                   11     //最低可用道具
#define PROP_FSK                        11     //道具 飞升卡
#define PROP_JBK_S                      12     //道具 进宝卡(小)
#define PROP_MLSS                       13     //道具 名利双收卡
#define PROP_JBK_M                      14     //道具 进宝卡(中)
#define PROP_JBK_L                      15     //道具 进宝卡(大）
#define PROP_HSZFK                      16     //道具 海神祝福卡
#define PROP_GOODLUCK                   17     //道具 幸运女神卡
#define PROP_GAME_RACE                  18     //道具 比赛资格卡
#define PROP_USER_HIGH                  19     //最高可用道具

//形象类
#define PROP_CRYSTAL_RING              19     //水晶戒指
#define PROP_GREEN_GEMS_RING           20     //绿柱石戒指
#define PROP_CAT_EYE_GEMS_RING         21     //猫眼石戒指
#define PROP_BULE_GEMS_RING            22     //蓝宝石戒指
#define PROP_RED_GEMS_RING             23     //红宝石戒指
#define PROP_EMERALD_RING              24     //祖母绿戒指
#define PROP_DIAMOND_5C_RING           25     //5克拉钻石
#define PROP_DIAMOND_10C_RING          26     //10克拉钻石
#define PROP_DIAMOND_20C_RING          27     //20克拉钻石
#define PROP_DIAMOND_50C_RING          28     //50克拉钻石
#define PROP_DIAMOND_100C_RING         29     //100克拉钻石

//添加结束

BYTE const g_PropTypeList[]=
{
	PROP_DOUBLE	,
	PROP_FOURDOLD,
	PROP_NEGAGIVE,
	PROP_FLEE	,
	PROP_BUGLE	,
	PROP_KICK	,
	PROP_SHIELD	,
	PROP_MEMBER_1,
	PROP_MEMBER_2,
	PROP_MEMBER_3,
	PROP_MEMBER_4,

	PROP_FSK,                       
	PROP_JBK_S,                   
	PROP_MLSS,                      
	PROP_JBK_M,                    
	PROP_JBK_L,                     
	PROP_HSZFK ,                  
	PROP_GOODLUCK,                
	PROP_GAME_RACE, 

	PROP_CRYSTAL_RING,             
	PROP_GREEN_GEMS_RING,           
	PROP_CAT_EYE_GEMS_RING,        
	PROP_BULE_GEMS_RING,           
	PROP_RED_GEMS_RING,            
	PROP_EMERALD_RING,        
	PROP_DIAMOND_5C_RING,           
	PROP_DIAMOND_10C_RING,          
	PROP_DIAMOND_20C_RING,       
	PROP_DIAMOND_50C_RING,      
	PROP_DIAMOND_100C_RING
};
#define PROPERTY_COUNT			(sizeof(g_PropTypeList)/sizeof(g_PropTypeList[0]))//道具数目

//道具定义
#define MAX_PROPERTY_COUNT				100									//最大数目
#define BUGLE_MAX_CHAR					120									//喇叭个数
#define BULESSING_MAX_CHAR				100									//祝福个数

//道具描述
struct tagPropertyDescribe
{
	TCHAR							szName[32];							//道具名称
	TCHAR							szDescribe[255];					//道具价值
};

tagPropertyDescribe const g_PropertyDescribe[PROPERTY_COUNT] =
{
	{TEXT("双倍积分卡"),TEXT("拥有了双倍积分卡，玩游戏赢分翻倍，输了不多扣！购买即生效。多次购买时间累加。（注：只能在购买的游戏房间使用。）")},
	{TEXT("四倍积分卡"),TEXT("拥有了四倍积分卡，玩游戏赢分翻四倍，输了不多扣！购买即生效。多次购买时间累加。（注：只能在购买的游戏房间使用。）")},
	{TEXT("负分清零"),TEXT("使用该道具后您的游戏积分将恢复初始状态，不再为负分而烦恼！")},
	{TEXT("清逃跑率"),TEXT("使用该道具后您的逃跑率将恢复初始状态，不再为逃跑而烦恼！")},
	{TEXT("小喇叭"),TEXT("您发送的消息将在房间及房间内的所有游戏桌子上以醒目的方式显示！")},	
	{TEXT("防踢卡"),TEXT("购买该道具后您不再担心被会员踢出游戏桌！")},
	{TEXT("护身符"),TEXT("购买该道具后您输分不扣分，不再为负分而烦恼！（注：只能在购买的游戏房间使用。）")},
	{TEXT("红钻会员卡"),TEXT("购买礼物/道具时可享受9折优惠，可踢普通玩家，有红钻会员特殊标记，会员期间，名字ID在大厅及房间中红色字体显示,优先进入人满房间！")},
	{TEXT("蓝钻会员卡"),TEXT("购买礼物/道具时可享受9折优惠，可踢普通、红钻会员，有蓝钻会员特殊标记，会员期间，名字ID在大厅及房间中红色字体显示，优先进入人满房间！")},
	{TEXT("黄钻会员卡"),TEXT("购买礼物/道具时可享受9折优惠，可踢普通、红钻、蓝钻玩家，有会员特殊标记，会员期间，名字ID在大厅及房间中红色字体显示，优先进入人满房间！")},
	{TEXT("紫钻会员卡"),TEXT("购买礼物/道具时可享受9折优惠，可踢普通、红钻、蓝钻、黄钻玩家，有会员特殊标记，会员期间，名字ID在大厅及房间中红色字体显示，优先进入人满房间！")},

	{TEXT("飞升卡"),TEXT("使用后可以获得双倍经验，持续2个小时！")},
	{TEXT("进宝卡(小)"),TEXT("使用后在游戏中获胜可以获得双倍宝石,输了不多扣，持续2个小时！")},
	{TEXT("名利双收卡"),TEXT("使用后在游戏中可以获得双倍经验,如果获胜还可以获得双倍宝石,输了不多扣，持续2个小时！")},
	{TEXT("进宝卡(中)"),TEXT("使用后在游戏中获胜获得双倍宝石,输了不多扣，持续4个小时！")},
	{TEXT("进宝卡(大)"),TEXT("使用后在游戏中获胜可以获得双倍宝石,输了不多扣，持续6个小时！")},
	{TEXT("海神祝福卡"),TEXT("在游戏中输了不扣钱，持续2个小时")},
	{TEXT("幸运女神卡"),TEXT("如果在游戏中获胜有20%的概率获得随机的奖励，可能是水晶戒指+1000宝石或者进宝卡(小)，持续2个小时！")},
	{TEXT("比赛资格卡"),TEXT("参与竞技！")},

	{TEXT("水晶戒指"),TEXT("1000金币 赠送 1000 宝石")},
	{TEXT("绿柱石戒指"),TEXT("2500金币 赠送 2500 宝石")},
	{TEXT("猫眼石戒指"),TEXT("4000金币 赠送 4000 宝石")},
	{TEXT("蓝宝石戒指"),TEXT("6500金币 赠送 6500 宝石")},

	{TEXT("红宝石戒指"),TEXT("9000金币 赠送 9000 宝石")},
	{TEXT("祖母绿戒指"),TEXT("12000金币 赠送 12000 宝石")},
	{TEXT("钻石戒指5Ct"),TEXT("50000金币 赠送 50000 宝石")},
	{TEXT("钻石戒指10Ct"),TEXT("100000金币 赠送 100000 宝石")},

	{TEXT("钻石戒指20Ct"),TEXT("200000金币 赠送 200000 宝石")},
	{TEXT("钻石戒指50Ct"),TEXT("500000金币 赠送 500000 宝石")},
	{TEXT("钻石戒指100Ct"),TEXT("1000000金币 赠送 1000000 宝石")},

	//{TEXT("百达翡丽镶钻腕表"),TEXT("150万金币 赠送 1500009宝石")},
	//{TEXT("江诗丹顿钻石女表"),TEXT("150万金币 赠送 1500001 宝石")},
	//{TEXT("R8"),TEXT("350万金币 赠送 1500007 宝石")},
	//{TEXT("幻影"),TEXT("500万金币 赠送 3500008 宝石")}
};

//道具信息结构
struct tagPropertyInfo
{
	int							nPropertyID;						//道具ID
	DWORD						dwPropCount1;						//道具数目
	DWORD						dwPropCount2;						//道具数目
	DWORD						dwPropCount3;						//道具数目
	DWORD						dwPropCount4;						//道具数目
	DWORD						dwPropCount5;						//道具数目
	DWORD						dwPropCount6;						//道具数目
	DWORD						dwPropCount7;						//道具数目
	DWORD						dwPropCount8;						//道具数目
	DWORD						dwPropCount9;						//道具数目
	DWORD						dwPropCount10;						//道具数目
	LONG						lPrice1;							//道具价格
	LONG						lPrice2;							//道具价格
	LONG						lPrice3;							//道具价格
	LONG						lPrice4;							//道具价格
	LONG						lPrice5;							//道具价格
	LONG						lPrice6;							//道具价格
	LONG						lPrice7;							//道具价格
	LONG						lPrice8;							//道具价格
	LONG						lPrice9;							//道具价格
	LONG						lPrice10;							//道具价格
	BYTE						cbDiscount;							//会员折扣
	//彭文添加
    DWORD                       dwLastingTime;                      //道具持续的时间
	//添加结束
	bool						bNullity;							//禁止标识
};

//鲜花信息结构
struct tagFlowerInfo
{
	int							nFlowerID;							//鲜花ID
	LONG						lPrice;								//鲜花价格
	LONG						lSendUserCharm;						//赠送魅力
	LONG						lRcvUserCharm;						//接受魅力
	BYTE						cbDiscount;							//会员折扣
	bool						bNullity;							//禁止标识
};

//////////////////////////////////////////////////////////////////////////////

#endif