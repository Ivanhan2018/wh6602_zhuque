#ifndef GLOBAL_FRAME_HEAD_FILE
#define GLOBAL_FRAME_HEAD_FILE

#pragma once

#include "GlobalDef.h"
#include "GlobalProperty.h"

//////////////////////////////////////////////////////////////////////////
//宏定义

//游戏状态
#define GS_FREE							0								//空闲状态
#define GS_PLAYING						100								//游戏状态

//////////////////////////////////////////////////////////////////////////
//IPC 网络事件

#define IPC_MAIN_SOCKET					1								//网络消息

#define IPC_SUB_SOCKET_SEND				1								//网络发送
#define IPC_SUB_SOCKET_RECV				2								//网络接收

//IPC 网络包结构
struct IPC_SocketPackage
{
	CMD_Command							Command;						//命令信息
	BYTE								cbBuffer[SOCKET_PACKET];		//数据缓冲
};

//////////////////////////////////////////////////////////////////////////
//IPC 配置信息

#define IPC_MAIN_CONFIG					2								//配置信息

#define IPC_SUB_SERVER_INFO				1								//房间信息
#define IPC_SUB_COLUMN_INFO				2								//列表信息
#define IPC_SUB_GAMEAFFICHE_INFO		3								//公告信息

//游戏信息
struct IPC_GF_ServerInfo
{
	DWORD								dwUserID;						//用户 I D
	WORD								wTableID;						//桌子号码
	WORD								wChairID;						//椅子号码
	WORD								wKindID;						//类型标识
	WORD								wServerID;						//房间标识
	WORD								wGameGenre;						//游戏类型
	WORD								wChairCount;					//椅子数目
	BYTE								cbHideUserInfo;					//隐藏信息
	DWORD								dwVideoAddr;					//视频地址
	TCHAR								szKindName[KIND_LEN];			//类型名字
	TCHAR								szServerName[SERVER_LEN];		//房间名称
	WORD								wSortID;						//排序号码
	LONG								lCellScore;						//进入房间最低金币
};

//////////////////////////////////////////////////////////////////////////
//IPC 用户信息

#define IPC_MAIN_USER					3								//用户信息

#define IPC_SUB_USER_COME				1								//用户信息
#define IPC_SUB_USER_STATUS				2								//用户状态
#define IPC_SUB_USER_SCORE				3								//用户积分
#define IPC_SUB_GAME_START				4								//游戏开始
#define IPC_SUB_GAME_FINISH				5								//游戏结束
#define IPC_SUB_UPDATE_FACE				6								//更新头像
#define IPC_SUB_MEMBERORDER				7								//更新头像
#define IPC_SUB_GETUSERPASSWORD			8								//获取用户md5密码
#define IPC_SUB_RECVACTIONINFO			9								//接受用户动作消息
#define IPC_SUB_RECVGLINFO				11								//接受用户语音消息


//用户状态
struct IPC_UserStatus
{
	DWORD								dwUserID;						//用户 I D
	WORD								wNetDelay;						//网络延时
	BYTE								cbUserStatus;					//用户状态
};

//用户分数
struct IPC_UserScore
{
	LONG								lLoveliness;					//用户魅力
	DWORD								dwUserID;						//用户 I D
	tagUserScore						UserScore;						//用户积分
};

//会员等级
struct IPC_MemberOrder
{
	BYTE								cbMember;					//会员等级
	DWORD								dwUserID;						//用户 I D
};

//用户分数
struct IPC_UpdateFace
{
	DWORD								dwCustomFace;					//用户 I D
};

//用户MD5密码
struct IPC_UserPassWord
{
	char								chUserPassWord[PASS_LEN];		//用户MD5密码
};


//////////////////////////////////////////////////////////////////////////
//IPC 控制信息

#define IPC_MAIN_CONCTROL				4								//控制信息

#define IPC_SUB_START_FINISH			1								//启动完成
#define IPC_SUB_CLOSE_FRAME				2								//关闭框架
#define IPC_SUB_JOIN_IN_GAME			3								//加入游戏
#define IPC_SUB_GO_PLAZA				4								//返回大厅(游戏列表)
#define IPC_SUB_GO_ROOM					5								//返回房间列表
#define IPC_SUB_GAMEFRAME_EXIST			6								//游戏框架是否存在
#define IPC_SUB_GAME_FRAME_CLOSED		7								//游戏框架窗口关闭消息
#define IPC_SUB_USER_UPDATE_DJINFO		8								//用户更新道具信息
#define IPC_SUB_SENDACTIONINFO			9								//发送用户动作消息
#define IPC_SUB_SENDLANLISTINFO			11								//发送主意消息
#define IPC_SUB_CMD_CONNECTWEB			12								//connect web


//web
struct IPC_ConnWeb
{
	BYTE								byConWebTag;					//0:NO, 1:官方网页; 2:道具商城: 3:客服中心;
};


//加入游戏
struct IPC_JoinInGame
{
	WORD								wTableID;						//桌子号码
	WORD								wChairID;						//椅子号码
};

//用户动作消息
struct IPC_UserActionInfo 
{
	BYTE								dwSendUserID;					//发送者
	BYTE								dwRecvUserID;					//接受者
	BYTE								dwActionTag;					//动作标识
};
//用户语音消息
struct IPC_UserLanguageInfo
{
	BYTE								dwSendUserID;					//发送者
	DWORD								dwIndex;					//序号
};

//更新用户道具ID
struct IPC_UserUpdateDJInfo
{
	int									iDJID;							//道具ID						
};
//////////////////////////////////////////////////////////////////////////
//网络命令码

#define MDM_GF_GAME						100								//游戏消息
#define MDM_GF_FRAME					101								//框架消息
#define	MDM_GF_PRESENT					102								//礼物消息
#define	MDM_GF_BANK						103								//银行消息
#define MDM_GF_TASK						104								//任务消息

#define SUB_GF_INFO						1								//游戏信息
#define SUB_GF_USER_READY				2								//用户同意
#define SUB_GF_LOOKON_CONTROL			3								//旁观控制
#define SUB_GF_KICK_TABLE_USER			4								//踢走用户

#define SUB_GF_OPTION					100								//游戏配置
#define SUB_GF_SCENE					101								//场景信息

#define SUB_GF_USER_CHAT				200								//用户聊天

#define SUB_GF_MESSAGE					300								//系统消息

//#define SUB_GF_GIFT						400								//赠送消息

#define SUB_GF_BANK_STORAGE				450								//银行存储
#define SUB_GF_BANK_GET					451								//银行提取

//#define SUB_GF_FLOWER_ATTRIBUTE			500								//鲜花属性
//#define SUB_GF_FLOWER					501								//鲜花消息
//#define SUB_GF_EXCHANGE_CHARM			502								//兑换魅力

//#define SUB_GF_PROPERTY					550							//道具消息
//#define SUB_GF_PROPERTY_RESULT			551						    //道具结果
//#define SUB_GF_RESIDUAL_PROPERTY		552								//剩余道具
//#define SUB_GF_PROP_ATTRIBUTE			553								//道具属性
#define SUB_GF_PROP_BUGLE				554								//喇叭道具
//彭文添加
//#define SUB_GF_ACTIVE_PROP              560							//道具激活消息
//#define SUB_GF_ACTIVE_SUCC              561							//道具激活成功
//#define SUB_GF_ACTIVE_FAIL_NO_PROP      562							//没有道具     激活失败
//#define SUB_GF_ACTIVE_FAIL_STILL_USE    563							//道具还在使用 激活失败
//#define SUB_GF_USER_UPDATE_PROP         564							//用户请求更新道具

//#define SUB_GF_PROP_OVERDUE             570							//目前的道具已经过期了
//#define SUB_GF_PROP_UPDATE              571							//道具信息更新


#define SUB_SENDACTIONINFO			    572								//发送用户动作消息
#define SUB_RECVACTIONINFO			    573							    //接受用户动作消息
#define SUB_SEND_SOUND_MESS         574                             //发送预定义语音消息
#define SUB_RECV_SOUND_MESS         575                             //接受预定义语音消息

#define SUB_GF_QUIT_GAME_SERVER         580								//用户请求退出游戏服务器     
#define SUB_GF_QUIT_GAME_SERVER_SUCC    581								//请求退出游戏服务器成功
//添加结束


#define SUB_GF_FIND_GEMS_TASK_ACCEPT				600								//任务接收
//#define SUB_GF_TASK_SAVEJD						601								//任务进度
#define SUB_GF_FIND_GEMS_TASK_ONCEEND				602								//单个任务完成
#define SUB_GF_NEW_GIFT								603								//新手礼包	Author<cxf>; DateTime<2012-01-16>
#define SUB_GF_ONLINE_TIME							604								//在线有礼-计时器

//版本信息
struct CMD_GF_Info
{
	BYTE								bAllowLookon;					//旁观标志
};

//游戏配置
struct CMD_GF_Option
{
	BYTE								bGameStatus;					//游戏状态
	BYTE								bAllowLookon;					//允许旁观
};

//旁观控制
struct CMD_GF_LookonControl
{
	DWORD								dwUserID;						//用户标识
	BYTE								bAllowLookon;					//允许旁观
};

//踢走用户
struct CMD_GF_KickTableUser
{
	DWORD								dwUserID;						//用户 I D
};

//聊天结构
struct CMD_GF_UserChat
{
	WORD								wChatLength;					//信息长度
	COLORREF							crFontColor;					//信息颜色
	DWORD								dwSendUserID;					//发送用户
	DWORD								dwTargetUserID;					//目标用户
	TCHAR								szChatMessage[MAX_CHAT_LEN];	//聊天信息
};
//////////////////////////////////////////////////////////////////////////

//消息类型
#define SMT_INFO						0x0001							//信息消息
#define SMT_EJECT						0x0002							//弹出消息
#define SMT_GLOBAL						0x0004							//全局消息
#define SMT_GAMEDJINFO					0x0008							//道具消息
#define SMT_CLOSE_GAME					0x0010							//关闭游戏
#define SMT_INTERMIT_LINE				0x0020							//中断连接
#define SMT_CLOSE_ROOM					0x0040							//关闭房间
#define SMT_CUSTOMER_FRAME				0x0080							//客服框

//消息数据包
struct CMD_GF_Message
{
	WORD								wMessageType;					//消息类型
	WORD								wMessageLength;					//消息长度
	TCHAR								szContent[1024];				//消息内容

	CMD_GF_Message()
	{
		ZeroMemory(this, sizeof(*this));
	}
};

//////////////////////////////////////////////////////////////////////////

//共享内存定义
struct tagShareMemory
{
	WORD								wDataSize;						//数据大小
	HWND								hWndGameFrame;					//框架句柄
	HWND								hWndGamePlaza;					//广场句柄
	HWND								hWndGameServer;					//房间句柄
};

//////////////////////////////////////////////////////////////////////////

//发送场所
#define	LOCATION_GAME_ROOM				1								//游戏房间
#define	LOCATION_PLAZA_ROOM				2								//大厅房间

//////////////////////////////////////////////////////////////////////////

//赠送结构
struct CMD_GF_Gift
{
	BYTE								cbSendLocation;					//发送场所
	DWORD								dwSendUserID;					//赠送者ID
	DWORD								dwRcvUserID;					//接受者ID
	WORD								wGiftID;						//礼物	ID
	WORD								wFlowerCount;					//鲜花数目
};

//道具结构
struct CMD_GF_Property
{
	BYTE								cbSendLocation;					//发送场所
	int									nPropertyID;					//道具ID
	DWORD								dwPachurseCount;				//购买数目
	DWORD								dwOnceCount;					//单次数目
	DWORD								dwSourceUserID;					//赠送玩家
	DWORD								dwTargetUserID;					//目标玩家
	TCHAR								szRcvUserName[32];				//用户名称
};



//喇叭结构
struct CMD_GF_BugleProperty
{
	BYTE								cbSendLocation;					//发送场所
	TCHAR								szUserName[32];					//玩家帐号
	COLORREF							crText;							//文字颜色
	TCHAR								szContext[BUGLE_MAX_CHAR];		//喇叭内容
};

//兑换结构
struct CMD_GF_ExchangeCharm
{
	BYTE								cbSendLocation;					//发送场所
	LONG								lLoveliness;					//魅力数值
	DWORD								lGoldValue;						//金币数值
};

//赠送通知
struct CMD_GF_GiftNotify
{
	BYTE								cbSendLocation;					//发送场所
	DWORD								dwSendUserID;					//赠送者ID
	DWORD								dwRcvUserID;					//接受者ID
	WORD								wGiftID;						//礼物	ID
	WORD								wFlowerCount;					//鲜花数目
};

//存储金币
struct CMD_GF_BankStorage
{
	LONGLONG 							lStorageValue;					//存储金币
	BYTE								cbGameAction;					//游戏操作
	TCHAR								szPassword[PASS_LEN];			//用户密码
};

//提取金币
struct CMD_GF_BankGet
{
	LONGLONG							lGetValue;						//提取金币
	BYTE								cbGameAction;					//游戏操作
	TCHAR								szPassword[PASS_LEN];			//用户密码
};

//彭文添加
struct	CMD_GF_UserProperty     //用户道具数据 
{
	DWORD								dwID;                          //道具id
	DWORD							    dwResidualTime;                //还有多少个道具
	DWORD								bUse;                          //是否还有道具正在使用
	DWORD								dwResidualTimeCount;           //还剩下多少时间道具失去作用 时间为秒
};

struct CMD_GF_ResidualProperty  //用户道具容器
{
	CMD_GF_UserProperty					dwResidual[PROPERTY_COUNT];	    //剩余时间
};


//任务说明
struct CMD_GF_TASK_tagTask
{
	DWORD								dwUserID;						//用户的id
	int									iTaskID;						//任务id
	TCHAR								strTitle[24];					//任务的标题
	TCHAR								strContext[64];					//任务的内容
	int									iLargessCount1;                 //新手区奖励的宝石
	int									iLargessCount2;                 //初级区奖励的宝石
	int									iLargessCount3;                 //中级区奖励的宝石
	int									iLargessCount4;                 //高级区奖励的宝石
	int									iTaskUserLogID;					//任务记录id
	TCHAR								strUserLogTable[24];			//任务的表名
};

//完成的任务结构
struct CMD_GF_TASK_tagTaskFinish
{
	DWORD                               dwKindID;                       //游戏类型id;  m_pGameServiceOption->wKindID
	DWORD                               dwUserID;                       //用户id      
	DWORD           					dwGems;       					//奖励的宝石
    int                                 iTaskID;                        //任务id

	int									iTaskUserLogID;					//任务记录id
	TCHAR								strUserLogTable[24];			//任务的表名	
	bool                                bFinish;                        //是否完成
};

//新手礼包	Author<cxf>; DateTime<2012-01-16>
struct CMD_GF_NEW_GIFT
{
	DWORD								wUserID;						//用户ID
	WORD								wGiftType;						//礼品类型(1:开门红;2在线有礼;3:升级赠送)
	int									iGold;							//赠送金币
	int									iGem;							//赠送宝石
	int									iOnlineTime;					//在线时间
	int									iGrade;							//等级 
	int									iGiftTime;						//赠送时间
	DWORD                               dwClientIP;						//用户ip
	
	CMD_GF_NEW_GIFT()
	{
		wGiftType=0;
		iGold=0;
		iGem=0;
		iOnlineTime=0;
		iGrade=0;
		iGiftTime=0;
	}
};

// 在线有礼 - 计时
struct CMD_GF_ONLINE
{
	DWORD								dwChairID;						// 椅子ID
	int									iToDayOnlineTime;				// 今天游戏时间
	BOOL								bStart;							// 开始计时

	CMD_GF_ONLINE()
	{
		ZeroMemory(this, sizeof(*this));
	}
};
//////////////////////////////////////////////////////////////////////////
#endif