#ifndef GLOBAL_DEF_HEAD_FILE
#define GLOBAL_DEF_HEAD_FILE

#pragma once

#include "biostream.h"
#include "GlobalProperty.h"

//////////////////////////////////////////////////////////////////////////
//公共定义

#define MAX_CHAIR						100								//最大椅子
#define MAX_CHAIR_NORMAL				8								//最大人数

#define MAX_ANDROID						256								//最大机器
#define MAX_CHAT_LEN					128								//聊天长度
#define LIMIT_CHAT_TIMES				1200							//限时聊天

/////////////////////////////////////////////////////////////////////////////////////////
//宏定义

//端口定义
#define PORT_VIDEO_SERVER				7600								//视频服务器
#define PORT_LOGON_SERVER				9001								//登陆服务器
#define PORT_CENTER_SERVER				9010								//中心服务器

//网络数据定义
#define SOCKET_VER						0x66								//网络版本
#define SOCKET_BUFFER					8192								//网络缓冲
#define SOCKET_PACKET					(SOCKET_BUFFER-sizeof(CMD_Head)-2*sizeof(unsigned long))

/////////////////////////////////////////////////////////////////////////////////////////

//内核命令码
#define MDM_KN_COMMAND					0									//内核命令
#define SUB_KN_DETECT_SOCKET			1									//检测命令
#define SUB_KN_SHUT_DOWN_SOCKET			2									//中断网络

//网络内核
struct CMD_Info
{
	unsigned char								cbVersion;							//版本标识
	unsigned char								cbCheckCode;						//效验字段
	unsigned short								wPacketSize;						//数据大小
};

//网络命令
struct CMD_Command
{
	unsigned short								wMainCmdID;							//主命令码
	unsigned short								wSubCmdID;							//子命令码
};

//网络包头
struct CMD_Head
{
	CMD_Info							CmdInfo;							//基础结构
	CMD_Command							CommandInfo;						//命令信息
};

//网络包缓冲
struct CMD_Buffer
{
	CMD_Head							Head;								//数据包头
	unsigned char								cbBuffer[SOCKET_PACKET];			//数据缓冲
};

//检测结构信息
struct CMD_KN_DetectSocket
{
	unsigned int								dwSendTickCount;					//发送时间
	unsigned int								dwRecvTickCount;					//接收时间
};

/////////////////////////////////////////////////////////////////////////////////////////

//IPC 数据定义
#define IPC_VER							0x0001								//IPC 版本
#define IPC_IDENTIFIER					0x0001								//标识号码
#define IPC_PACKAGE						4096								//最大 IPC 包
#define IPC_BUFFER						(sizeof(IPC_Head)+IPC_PACKAGE)		//缓冲长度

//IPC 数据包头
struct IPC_Head
{
	unsigned short								wVersion;							//IPC 版本
	unsigned short								wDataSize;							//数据大小
	unsigned short								wMainCmdID;							//主命令码
	unsigned short								wSubCmdID;							//子命令码
};

//IPC 缓冲结构
struct IPC_Buffer
{
	IPC_Head							Head;								//数据包头
	unsigned char								cbBuffer[IPC_PACKAGE];				//数据缓冲
};


//长度宏定义
#define TYPE_LEN						32									//种类长度
#define KIND_LEN						32									//类型长度
#define STATION_LEN						32									//站点长度
#define SERVER_LEN						32									//房间长度
#define MODULE_LEN						32									//进程长度
#define RULE_LEN						512									//规则长度

//性别定义
#define GENDER_NULL						0									//未知性别
#define GENDER_BOY						1									//男性性别
#define GENDER_GIRL						2									//女性性别

//游戏类型
#define GAME_GENRE_SCORE				0x0001								//点值类型
#define GAME_GENRE_GOLD					0x0002								//金币类型
#define GAME_GENRE_MATCH				0x0004								//比赛类型
#define GAME_GENRE_EDUCATE				0x0008								//训练类型

//游戏级别结构
struct tagLevelItem
{
	int								lLevelScore;						//级别积分
	char								szLevelName[16];					//级别描述
};

//用户状态定义
#define US_NULL							0x00								//没有状态
#define US_FREE							0x01								//站立状态
#define US_SIT							0x02								//坐下状态
#define US_READY						0x03								//同意状态
#define US_LOOKON						0x04								//旁观状态
#define US_PLAY							0x05								//游戏状态
#define US_OFFLINE						0x06								//断线状态

//长度宏定义
#define NAME_LEN						32									//名字长度
#define PASS_LEN						33									//密码长度
#define EMAIL_LEN						32									//邮箱长度
#define GROUP_LEN						32									//社团长度
#define COMPUTER_ID_LEN					33									//机器序列
#define UNDER_WRITE_LEN					63									//个性签名

//用户积分信息
struct tagUserScore
{
	int								lScore;								//用户分数
	int								lGameGold;							//游戏金币//
	int								lInsureScore;						//存储金币
	int								lWinCount;							//胜利盘数
	int								lLostCount;							//失败盘数
	int								lDrawCount;							//和局盘数
	int								lFleeCount;							//断线数目
	int								lExperience;						//用户经验

	void reset() { memset(this, 0, sizeof(*this)); }
	tagUserScore() { reset(); }
	friend bostream& operator<<(bostream& bos,const tagUserScore& pl)
	{
		bos << pl.lScore;
		bos << pl.lGameGold;
		bos << pl.lInsureScore;
		bos << pl.lWinCount;
		bos << pl.lLostCount;
		bos << pl.lDrawCount;
		bos << pl.lFleeCount;
		bos << pl.lExperience;

		return bos;
	}
	friend bistream& operator>>(bistream& bis,tagUserScore& pl)
	{	
		pl.reset();
		bis >> pl.lScore;
		bis >> pl.lGameGold;
		bis >> pl.lInsureScore;
		bis >> pl.lWinCount;
		bis >> pl.lLostCount;
		bis >> pl.lDrawCount;
		bis >> pl.lFleeCount;
		bis >> pl.lExperience;

		return bis;
	}
};

//用户状态信息
struct tagUserStatus
{
	unsigned short								wTableID;							//桌子号码
	unsigned short								wChairID;							//椅子位置
	unsigned char								cbUserStatus;						//用户状态
};

//用户基本信息结构
struct tagUserInfoHead
{
	//用户属性
	unsigned short								wFaceID;							//头像索引
	unsigned int								dwUserID;							//用户 I D
	unsigned int								dwGameID;							//游戏 I D
	unsigned int								dwGroupID;							//社团索引
	unsigned int								dwUserRight;						//用户等级
	int								lLoveliness;						//用户魅力
	unsigned int								dwMasterRight;						//管理权限

	char								szName[NAME_LEN];					//用户名字

	//用户属性
	unsigned char								cbGender;							//用户性别
	unsigned char								cbMemberOrder;						//会员等级
	unsigned char								cbMasterOrder;						//管理等级

	//用户状态
	unsigned short								wTableID;							//桌子号码
	unsigned short								wChairID;							//椅子位置
	unsigned char								cbUserStatus;						//用户状态

	//用户积分
	tagUserScore						UserScoreInfo;						//积分信息

	//扩展信息
	//long								lInsureScore;						//消费金币
	//long								lGameGold;							//游戏金币
	unsigned int								dwCustomFaceVer;					//上传头像
	unsigned int								dwPropResidualTime[PROPERTY_COUNT];	//道具时间
};

//用户基本信息结构
struct tagUserInfoHead_V2
{
	//用户属性
	unsigned short								wFaceID;							//头像索引
	unsigned int								dwUserID;							//用户 I D
	unsigned int								dwGameID;							//游戏 I D
	unsigned int								dwGroupID;							//社团索引
	unsigned int								dwUserRight;						//用户等级
	int								            lLoveliness;						//用户魅力
	unsigned int								dwMasterRight;						//管理权限

	char								        szName[NAME_LEN];					//用户名字
	char								        szUnderWrite[UNDER_WRITE_LEN];		//个性签名
	TCHAR								        szFaceUrl[256];						//头像地址

	//用户属性
	unsigned char								cbGender;							//用户性别
	unsigned char								cbMemberOrder;						//会员等级
	unsigned char								cbMasterOrder;						//管理等级

	//用户状态
	unsigned short								wTableID;							//桌子号码
	unsigned short								wChairID;							//椅子位置
	unsigned char								cbUserStatus;						//用户状态

	//用户积分
	tagUserScore						UserScoreInfo;						//积分信息

	//扩展信息
	//long								lInsureScore;						//消费金币
	//long								lGameGold;							//游戏金币
	unsigned int								dwCustomFaceVer;					//上传头像
	unsigned int								dwPropResidualTime[PROPERTY_COUNT];	//道具时间
};

//用户信息结构
struct tagUserData
{
	//用户属性
	unsigned short								wFaceID;							//头像索引
	unsigned int								dwCustomFaceVer;					//上传头像
	unsigned int								dwUserID;							//用户 I D
	unsigned int								dwGroupID;							//社团索引
	unsigned int								dwGameID;							//用户 I D
	unsigned int								dwUserRight;						//用户等级
	int								lLoveliness;						//用户魅力
	unsigned int								dwMasterRight;						//管理权限
	char								szName[NAME_LEN];					//用户名字
	char								szGroupName[GROUP_LEN];				//社团名字
	char								szUnderWrite[UNDER_WRITE_LEN];		//个性签名

	//用户属性
	unsigned char								cbGender;							//用户性别
	unsigned char								cbMemberOrder;						//会员等级
	unsigned char								cbMasterOrder;						//管理等级

	//用户积分
	int								lInsureScore;						//消费金币
	int								lGameGold;							//游戏金币
	int								lScore;								//用户分数
	int								lWinCount;							//胜利盘数
	int								lLostCount;							//失败盘数
	int								lDrawCount;							//和局盘数
	int								lFleeCount;							//断线数目
	int								lExperience;						//用户经验

	//用户状态
	unsigned short								wTableID;							//桌子号码
	unsigned short								wChairID;							//椅子位置
	unsigned char								cbUserStatus;						//用户状态

	//其他信息
	unsigned char								cbCompanion;						//用户关系
	unsigned int								dwPropResidualTime[PROPERTY_COUNT];	//道具时间
};

//////////////////////////////////////////////////////////////////////////

//机器序列号结构
struct tagClientSerial
{
	unsigned int								dwSystemVer;						//系统版本
	unsigned int								dwComputerID[3];					//机器序列
};

//配置缓冲结构
struct tagOptionBuffer
{
	unsigned char								cbBufferLen;						//数据长度
	unsigned char								cbOptionBuf[32];					//设置缓冲
};

//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////

#endif