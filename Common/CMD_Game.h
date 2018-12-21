#ifndef CMD_GAME_HEAD_FILE
#define CMD_GAME_HEAD_FILE

#include "biostream.h"

//////////////////////////////////////////////////////////////////////////
//登录数据包定义

#define MDM_GR_LOGON				1									//房间登录

#define SUB_GR_LOGON_ACCOUNTS		1									//帐户登录
#define SUB_GR_LOGON_USERID			2									//I D 登录

#define SUB_GR_HEART_BEAT			10									//心跳

#define SUB_GR_LOGON_SUCCESS		100									//登录成功
#define SUB_GR_LOGON_ERROR			101									//登录失败
#define SUB_GR_LOGON_FINISH			102									//登录完成

#define SUB_GR_LOGON_CHECK_ROOM		103									//登录完成后，校验房间号是否存在

//房间帐号登录
struct CMD_GR_LogonByAccounts
{
	enum {M_ID = MDM_GR_LOGON};
	enum {S_ID = SUB_GR_LOGON_ACCOUNTS};

	unsigned int							dwPlazaVersion;						//广场版本
	unsigned int							dwProcessVersion;					//进程版本
	char							szAccounts[NAME_LEN];				//登录帐号
	char							szPassWord[PASS_LEN];				//登录密码

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GR_LogonByAccounts() { reset(); }
	friend bostream& operator<<(bostream& bos,const CMD_GR_LogonByAccounts& pl)
	{
		bos << pl.dwPlazaVersion;
		bos << pl.dwProcessVersion;

		bos.write((char*)pl.szAccounts,sizeof(pl.szAccounts));
		bos.write((char*)pl.szPassWord,sizeof(pl.szPassWord));

		return bos;
	}
	friend bistream& operator>>(bistream& bis,CMD_GR_LogonByAccounts& pl)
	{	
		pl.reset();
		bis >> pl.dwPlazaVersion;
		bis >> pl.dwProcessVersion;

		bis.read((char*)pl.szAccounts,sizeof(pl.szAccounts));
		bis.read((char*)pl.szPassWord,sizeof(pl.szPassWord));

		return bis;
	}
};

//房间 ID 登录
struct CMD_GR_LogonByUserID
{
	enum {M_ID = MDM_GR_LOGON};
	enum {S_ID = SUB_GR_LOGON_USERID};

	unsigned int							dwPlazaVersion;						//广场版本
	unsigned int							dwProcessVersion;					//进程版本
	unsigned int							dwUserID;							//用户 I D
	char									szPassWord[PASS_LEN];				//登录密码

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GR_LogonByUserID() { reset(); }
	friend bostream& operator<<(bostream& bos,const CMD_GR_LogonByUserID& pl)
	{
		bos << pl.dwPlazaVersion;
		bos << pl.dwProcessVersion;
		bos << pl.dwUserID;

		bos.write((char*)pl.szPassWord,sizeof(pl.szPassWord));

		return bos;
	}
	friend bistream& operator>>(bistream& bis,CMD_GR_LogonByUserID& pl)
	{	
		pl.reset();
		bis >> pl.dwPlazaVersion;
		bis >> pl.dwProcessVersion;
		bis >> pl.dwUserID;

		bis.read((char*)pl.szPassWord,sizeof(pl.szPassWord));

		return bis;
	}
};

//登录成功消息
struct CMD_GR_LogonSuccess
{
	enum {M_ID = MDM_GR_LOGON};
	enum {S_ID = SUB_GR_LOGON_SUCCESS};

	unsigned int							dwUserID;							//用户 I D

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GR_LogonSuccess() { reset(); }
	friend bostream& operator<<(bostream& bos,const CMD_GR_LogonSuccess& pl)
	{
		bos << pl.dwUserID;

		return bos;
	}
	friend bistream& operator>>(bistream& bis,CMD_GR_LogonSuccess& pl)
	{	
		pl.reset();

		bis >> pl.dwUserID;

		return bis;
	}
};

//登录失败
struct CMD_GR_LogonError
{
	enum {M_ID = MDM_GR_LOGON};
	enum {S_ID = SUB_GR_LOGON_ERROR};

	int							lErrorCode;							//错误代码
	char							szErrorDescribe[128];				//错误消息

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GR_LogonError() { reset(); }
	friend bostream& operator<<(bostream& bos,const CMD_GR_LogonError& pl)
	{
		bos << pl.lErrorCode;

		bos.write((char*)pl.szErrorDescribe,sizeof(pl.szErrorDescribe));

		return bos;
	}
	friend bistream& operator>>(bistream& bis,CMD_GR_LogonError& pl)
	{	
		pl.reset();
		bis >> pl.lErrorCode;

		bis.read((char*)pl.szErrorDescribe,sizeof(pl.szErrorDescribe));

		return bis;
	}
};

//校验房号是否存在
struct CMD_GR_LogonCheckRoom
{
	enum { M_ID = MDM_GR_LOGON };
	enum { S_ID = SUB_GR_LOGON_CHECK_ROOM };

	unsigned int							dwUserID;							//用户 I D
	unsigned int							dwRoomID;							//房间号
	unsigned int							dwTableID;							//桌号

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GR_LogonCheckRoom() { reset(); }
	friend bostream& operator<<(bostream& bos, const CMD_GR_LogonCheckRoom& pl)
	{
		bos << pl.dwUserID;
		bos << pl.dwRoomID;
		bos << pl.dwTableID;
		
		return bos;
	}
	friend bistream& operator>>(bistream& bis, CMD_GR_LogonCheckRoom& pl)
	{
		pl.reset();
		bis >> pl.dwUserID;
		bis >> pl.dwRoomID;
		bis >> pl.dwTableID;

		return bis;
	}
};

//////////////////////////////////////////////////////////////////////////
//用户数据包定义

#define MDM_GR_USER					2									//用户信息

#define SUB_GR_USER_SIT_REQ			1									//坐下请求
#define SUB_GR_USER_SIT_REQ_V2		5									//坐下请求
#define SUB_GR_USER_LOOKON_REQ		2									//旁观请求
#define SUB_GR_USER_STANDUP_REQ		3									//起立请求
#define SUB_GR_USER_LEFT_GAME_REQ	4									//离开游戏

#define SUB_GR_USER_STATUS_REQ		10									//查询玩家状态，可作心跳使用
#define SUB_GR_USER_STATUS_REP		11									//查询玩家状态，可作心跳使用

#define SUB_GR_USER_STATUS_HAOYOU	12									//好友房用户暂时离开状态

#define SUB_GR_USER_COME			100									//用户进入
#define SUB_GR_USER_COME_V2			110									//用户进入
#define SUB_GR_USER_STATUS			101									//用户状态
#define SUB_GR_USER_SCORE			102									//用户分数
#define SUB_GR_SIT_FAILED			103									//坐下失败
#define SUB_GR_USER_RIGHT			104									//用户权限
#define SUB_GR_MEMBER_ORDER			105									//会员等级

#define SUB_GR_USER_CHAT			200									//聊天消息
#define SUB_GR_USER_WISPER			201									//私语消息
#define SUB_GR_USER_RULE			202									//用户规则

#define SUB_GR_USER_INVITE			300									//邀请消息
#define SUB_GR_USER_INVITE_REQ		301									//邀请请求

#define SUB_GR_USER_SPEAKER_MSG_REP		302								//接收到喇叭消息

#define SUB_GR_USER_SEND_LABA_REQ				304							//请求发送喇叭
#define SUB_GR_USER_SEND_LABA_REP				305							//请求发送喇叭结果

#define SUB_GR_USER_CHECK_LABA_REQ				306							//请求校验喇叭内容
#define SUB_GR_USER_CHECK_LABA_REP				307							//校验喇叭结果


#define SUB_GR_USER_MASTER_DISMISS_FRIEND_REQ			308						//房主要求解散好友房
#define SUB_GR_USER_MASTER_DISMISS_FRIEND_REP			309						//房主要求解散好友房广播

#define SUB_GR_USER_GUEST_DISMISS_FRIEND_REQ			310						//游戏中请求提前结束好友房
#define SUB_GR_USER_GUEST_DISMISS_FRIEND_RESULT			311						//游戏中请求提前结束好友房的用户选择

#define SUB_GR_USER_FACE_URL			320						//玩家自定义头像
#define SUB_GR_USER_OFFLINE_REMAIN_TIME			321						//玩家断线剩余时间
#define SUB_GR_USER_EXT_INFO			322						//玩家附加信息

#define SUB_GR_USER_YY_SEQ			330							//玩家发送语音
#define SUB_GR_USER_YY_REP			331							//玩家语音广播

//玩家发送语音
struct CMD_GR_UserYYInfo
{
	enum { M_ID = MDM_GR_USER };
	enum { S_ID = SUB_GR_USER_YY_SEQ };

	unsigned int dwUserID;	//数据库 ID
	unsigned short	nLen;			
	char			szExtInfo[256];	//语音地址

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GR_UserYYInfo() { reset(); }
	friend bostream& operator<<(bostream& bos, const CMD_GR_UserYYInfo& pl)
	{
		bos << pl.dwUserID;
		bos << pl.nLen;
		bos.write((char*)pl.szExtInfo, pl.nLen);

		return bos;
	}
	friend bistream& operator>>(bistream& bis, CMD_GR_UserYYInfo& pl)
	{
		bis >> pl.dwUserID;
		bis >> pl.nLen;
		bis.read((char*)pl.szExtInfo, pl.nLen);

		return bis;
	}
};

//玩家语音广播
struct CMD_GR_UserYYREPInfo
{
	enum { M_ID = MDM_GR_USER };
	enum { S_ID = SUB_GR_USER_YY_REP };

	unsigned int dwUserID;	//数据库 ID
	unsigned short	nLen;			
	char			szExtInfo[256];	//语音地址

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GR_UserYYREPInfo() { reset(); }
	friend bostream& operator<<(bostream& bos, const CMD_GR_UserYYREPInfo& pl)
	{
		bos << pl.dwUserID;
		bos << pl.nLen;
		bos.write((char*)pl.szExtInfo, pl.nLen);

		return bos;
	}
	friend bistream& operator>>(bistream& bis, CMD_GR_UserYYREPInfo& pl)
	{
		bis >> pl.dwUserID;
		bis >> pl.nLen;
		bis.read((char*)pl.szExtInfo, pl.nLen);

		return bis;
	}
};

//玩家附加信息
struct CMD_GR_UserExtInfo
{
	enum { M_ID = MDM_GR_USER };
	enum { S_ID = SUB_GR_USER_EXT_INFO };

	unsigned int dwUserID;	//数据库 ID
	unsigned short	nLen;			
	char			szExtInfo[256];	//附加信息，格式 k:v|k:v,  目前支持ip信息

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GR_UserExtInfo() { reset(); }
	friend bostream& operator<<(bostream& bos, const CMD_GR_UserExtInfo& pl)
	{
		bos << pl.dwUserID;
		bos << pl.nLen;
		bos.write((char*)pl.szExtInfo, pl.nLen);

		return bos;
	}
	friend bistream& operator>>(bistream& bis, CMD_GR_UserExtInfo& pl)
	{
		bis >> pl.dwUserID;
		bis >> pl.nLen;
		bis.read((char*)pl.szExtInfo, pl.nLen);

		return bis;
	}
};

//好友房用户暂时离开状态
struct CMD_GR_UserStatus_HaoYou
{
	enum { M_ID = MDM_GR_USER };
	enum { S_ID = SUB_GR_USER_STATUS_HAOYOU };

	unsigned int	dwUserID;							//数据库 ID
	unsigned short	wChairID;							//椅子位置
	unsigned char	cbUserStatus;						//用户状态: 1离开 2回来

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GR_UserStatus_HaoYou() { reset(); }
	friend bostream& operator<<(bostream& bos, const CMD_GR_UserStatus_HaoYou& pl)
	{
		bos << pl.dwUserID;
		bos << pl.wChairID;
		bos << pl.cbUserStatus;

		return bos;
	}
	friend bistream& operator>>(bistream& bis, CMD_GR_UserStatus_HaoYou& pl)
	{
		bis >> pl.dwUserID;
		bis >> pl.wChairID;
		bis >> pl.cbUserStatus;

		return bis;
	}
};


//玩家断线的剩余时间
struct CMD_GR_UserOfflineRemainTime
{
	enum { M_ID = MDM_GR_USER };
	enum { S_ID = SUB_GR_USER_OFFLINE_REMAIN_TIME };
		
	unsigned short	nRemainTime[8];				//最多8个玩家，每个人的剩余时间。单位： 分钟

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GR_UserOfflineRemainTime() { reset(); }
	friend bostream& operator<<(bostream& bos, const CMD_GR_UserOfflineRemainTime& pl)
	{
		bos.write((char*)pl.nRemainTime, sizeof(pl.nRemainTime));

		return bos;
	}
	friend bistream& operator>>(bistream& bis, CMD_GR_UserOfflineRemainTime& pl)
	{
		bis.read((char*)pl.nRemainTime, sizeof(pl.nRemainTime));

		return bis;
	}
};

//玩家自定义头像
struct CMD_GR_UserFaceUrl
{
	enum { M_ID = MDM_GR_USER };
	enum { S_ID = SUB_GR_USER_FACE_URL };

	unsigned int dwUserID;	//数据库 ID
	unsigned short	nLen;			
	char			szFaceUrl[256];	//头像地址

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GR_UserFaceUrl() { reset(); }
	friend bostream& operator<<(bostream& bos, const CMD_GR_UserFaceUrl& pl)
	{
		bos << pl.dwUserID;
		bos << pl.nLen;
		bos.write((char*)pl.szFaceUrl, pl.nLen);

		return bos;
	}
	friend bistream& operator>>(bistream& bis, CMD_GR_UserFaceUrl& pl)
	{
		bis >> pl.dwUserID;
		bis >> pl.nLen;
		bis.read((char*)pl.szFaceUrl, pl.nLen);

		return bis;
	}
};

//房主要求解散好友房
struct CMD_GR_UserMasterDismissFriendReq
{
	enum { M_ID = MDM_GR_USER };
	enum { S_ID = SUB_GR_USER_MASTER_DISMISS_FRIEND_REQ };

	unsigned short	nTableID;			//桌号
	char			szRoomID[NAME_LEN];	//房间号

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GR_UserMasterDismissFriendReq() { reset(); }
	friend bostream& operator<<(bostream& bos, const CMD_GR_UserMasterDismissFriendReq& pl)
	{
		bos << pl.nTableID;
		bos.write((char*)pl.szRoomID, sizeof(pl.szRoomID));

		return bos;
	}
	friend bistream& operator>>(bistream& bis, CMD_GR_UserMasterDismissFriendReq& pl)
	{
		bis >> pl.nTableID;
		bis.read((char*)pl.szRoomID, sizeof(pl.szRoomID));

		return bis;
	}
};

//房主要求解散好友房广播
struct CMD_GR_UserMasterDismissFriendRep
{
	enum { M_ID = MDM_GR_USER };
	enum { S_ID = SUB_GR_USER_MASTER_DISMISS_FRIEND_REP };

	unsigned short nCode;		//0-允许解散，1-不允许解散游戏已经开始
	char szContent[NAME_LEN];	//描述信息

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GR_UserMasterDismissFriendRep() { reset(); }
	friend bostream& operator<<(bostream& bos, const CMD_GR_UserMasterDismissFriendRep& pl)
	{
		bos << pl.nCode;
		bos.write((char*)pl.szContent, sizeof(pl.szContent));

		return bos;
	}
	friend bistream& operator>>(bistream& bis, CMD_GR_UserMasterDismissFriendRep& pl)
	{
		bis >> pl.nCode;
		bis.read((char*)pl.szContent, sizeof(pl.szContent));

		return bis;
	}
};


//游戏中请求提前结束好友房
struct CMD_GR_UserGuestDismissFriendReq
{
	enum { M_ID = MDM_GR_USER };
	enum { S_ID = SUB_GR_USER_GUEST_DISMISS_FRIEND_REQ };

	unsigned int dwUserID;	//数据库 ID

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GR_UserGuestDismissFriendReq() { reset(); }
	friend bostream& operator<<(bostream& bos, const CMD_GR_UserGuestDismissFriendReq& pl)
	{
		bos << pl.dwUserID;

		return bos;
	}
	friend bistream& operator>>(bistream& bis, CMD_GR_UserGuestDismissFriendReq& pl)
	{
		bis >> pl.dwUserID;

		return bis;
	}
};

//游戏中请求提前结束好友房的用户选择
struct CMD_GR_UserGuestDismissFriendResult
{
	enum { M_ID = MDM_GR_USER };
	enum { S_ID = SUB_GR_USER_GUEST_DISMISS_FRIEND_RESULT };

	unsigned int	dwUserID;	//数据库 ID
	char			bFlag;		//1:同意  0：拒绝

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GR_UserGuestDismissFriendResult() { reset(); }
	friend bostream& operator<<(bostream& bos, const CMD_GR_UserGuestDismissFriendResult& pl)
	{
		bos << pl.dwUserID;
		bos << pl.bFlag;

		return bos;
	}
	friend bistream& operator>>(bistream& bis, CMD_GR_UserGuestDismissFriendResult& pl)
	{
		bis >> pl.dwUserID;
		bis >> pl.bFlag;

		return bis;
	}
};


//会员等级
struct CMD_GR_MemberOrder
{
	enum {M_ID = MDM_GR_USER};
	enum {S_ID = SUB_GR_MEMBER_ORDER};

	unsigned int							dwUserID;							//数据库 ID
	unsigned char							cbMemberOrder;						//会员等级

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GR_MemberOrder() { reset(); }
	friend bostream& operator<<(bostream& bos,const CMD_GR_MemberOrder& pl)
	{
		bos << pl.dwUserID;
		bos << pl.cbMemberOrder;

		return bos;
	}
	friend bistream& operator>>(bistream& bis,CMD_GR_MemberOrder& pl)
	{	
		pl.reset();
		bis >> pl.dwUserID;
		bis >> pl.cbMemberOrder;

		return bis;
	}
};

//用户权限
struct CMD_GR_UserRight
{
	enum {M_ID = MDM_GR_USER};
	enum {S_ID = SUB_GR_USER_RIGHT};

	unsigned int							dwUserID;							//数据库 ID
	unsigned int							dwUserRight;						//用户权限

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GR_UserRight() { reset(); }
	friend bostream& operator<<(bostream& bos,const CMD_GR_UserRight& pl)
	{
		bos << pl.dwUserID;
		bos << pl.dwUserRight;

		return bos;
	}
	friend bistream& operator>>(bistream& bis,CMD_GR_UserRight& pl)
	{	
		pl.reset();
		bis >> pl.dwUserID;
		bis >> pl.dwUserRight;

		return bis;
	}
};

//用户状态
struct CMD_GR_UserStatus
{
	enum {M_ID = MDM_GR_USER};
	enum {S_ID = SUB_GR_USER_STATUS};

	unsigned int							dwUserID;							//数据库 ID
	unsigned short							wTableID;							//桌子位置
	unsigned short							wChairID;							//椅子位置
	unsigned char							cbUserStatus;						//用户状态

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GR_UserStatus() { reset(); }
	friend bostream& operator<<(bostream& bos,const CMD_GR_UserStatus& pl)
	{
		bos << pl.dwUserID;
		bos << pl.wTableID;
		bos << pl.wChairID;
		bos << pl.cbUserStatus;

		return bos;
	}
	friend bistream& operator>>(bistream& bis,CMD_GR_UserStatus& pl)
	{	
		pl.reset();
		bis >> pl.dwUserID;
		bis >> pl.wTableID;
		bis >> pl.wChairID;
		bis >> pl.cbUserStatus;

		return bis;
	}
};

//用户分数
struct CMD_GR_UserScore
{
	enum {M_ID = MDM_GR_USER};
	enum {S_ID = SUB_GR_USER_SCORE};

	int							lLoveliness;						//用户魅力
	//LONG							lInsureScore;						//消费金币
	//LONG							lGameGold;							//游戏金币
	unsigned int							dwUserID;							//用户 I D
	tagUserScore					UserScore;							//积分信息

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GR_UserScore() { reset(); }
	friend bostream& operator<<(bostream& bos,const CMD_GR_UserScore& pl)
	{
		bos << pl.lLoveliness;
		bos << pl.dwUserID;
		bos << pl.UserScore;

		return bos;
	}
	friend bistream& operator>>(bistream& bis,CMD_GR_UserScore& pl)
	{	
		pl.reset();
		bis >> pl.lLoveliness;
		bis >> pl.dwUserID;
		bis >> pl.UserScore;

		return bis;
	}
};

//请求坐下
struct CMD_GR_UserSitReq
{
	enum {M_ID = MDM_GR_USER};
	enum {S_ID = SUB_GR_USER_SIT_REQ};
	
	unsigned short							wTableID;							//桌子位置
	unsigned short							wChairID;							//椅子位置
	unsigned char							cbPassLen;							//密码长度
	char							szTablePass[PASS_LEN];				//桌子密码

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GR_UserSitReq() { reset(); }
	friend bostream& operator<<(bostream& bos,const CMD_GR_UserSitReq& pl)
	{
		bos << pl.wTableID;
		bos << pl.wChairID;
		bos << pl.cbPassLen;

		bos.write((char*)pl.szTablePass,sizeof(pl.cbPassLen));

		return bos;
	}
	friend bistream& operator>>(bistream& bis,CMD_GR_UserSitReq& pl)
	{	
		pl.reset();
		bis >> pl.wTableID;
		bis >> pl.wChairID;
		bis >> pl.cbPassLen;

		bis.read((char*)pl.szTablePass,sizeof(pl.cbPassLen));

		return bis;
	}
};


//请求坐下。因为原协议有bug，只能传1位的密码
struct CMD_GR_UserSitReq_V2
{
	enum { M_ID = MDM_GR_USER };
	enum { S_ID = SUB_GR_USER_SIT_REQ_V2 };

	unsigned short							wTableID;							//桌子位置
	unsigned short							wChairID;							//椅子位置
	unsigned char							cbPassLen;							//密码长度
	char							szTablePass[PASS_LEN];				//桌子密码

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GR_UserSitReq_V2() { reset(); }
	friend bostream& operator<<(bostream& bos, const CMD_GR_UserSitReq_V2& pl)
	{
		bos << pl.wTableID;
		bos << pl.wChairID;
		bos << pl.cbPassLen;

		bos.write((char*)pl.szTablePass, pl.cbPassLen);

		return bos;
	}
	friend bistream& operator>>(bistream& bis, CMD_GR_UserSitReq_V2& pl)
	{
		pl.reset();
		bis >> pl.wTableID;
		bis >> pl.wChairID;
		bis >> pl.cbPassLen;

		bis.read((char*)pl.szTablePass, pl.cbPassLen);

		return bis;
	}
};

//邀请用户 
struct CMD_GR_UserInviteReq
{
	enum {M_ID = MDM_GR_USER};
	enum {S_ID = SUB_GR_USER_INVITE_REQ};

	unsigned short							wTableID;							//桌子号码
	unsigned int							dwUserID;							//用户 I D

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GR_UserInviteReq() { reset(); }
	friend bostream& operator<<(bostream& bos,const CMD_GR_UserInviteReq& pl)
	{
		bos << pl.wTableID;
		bos << pl.dwUserID;

		return bos;
	}
	friend bistream& operator>>(bistream& bis,CMD_GR_UserInviteReq& pl)
	{	
		pl.reset();
		bis >> pl.wTableID;
		bis >> pl.dwUserID;

		return bis;
	}
};

//坐下失败
struct CMD_GR_SitFailed
{
	enum {M_ID = MDM_GR_USER};
	enum {S_ID = SUB_GR_SIT_FAILED};

	char							szFailedDescribe[256];				//错误描述

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GR_SitFailed() { reset(); }
	friend bostream& operator<<(bostream& bos,const CMD_GR_SitFailed& pl)
	{
		bos.write((char*)pl.szFailedDescribe,sizeof(pl.szFailedDescribe));

		return bos;
	}
	friend bistream& operator>>(bistream& bis,CMD_GR_SitFailed& pl)
	{	
		pl.reset();
		
		bis.read((char*)pl.szFailedDescribe,sizeof(pl.szFailedDescribe));

		return bis;
	}
};

//聊天结构 
struct CMD_GR_UserChat
{
	enum {M_ID = MDM_GR_USER};
	enum {S_ID = SUB_GR_USER_CHAT};

	unsigned short							wChatLength;						//信息长度
	unsigned int						crFontColor;						//信息颜色
	unsigned int							dwSendUserID;						//发送用户
	unsigned int							dwTargetUserID;						//目标用户
	char							szChatMessage[MAX_CHAT_LEN];		//聊天信息

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GR_UserChat() { reset(); }
	friend bostream& operator<<(bostream& bos,const CMD_GR_UserChat& pl)
	{
		bos << pl.wChatLength;
		bos << pl.crFontColor;
		bos << pl.dwSendUserID;
		bos << pl.dwTargetUserID;

		bos.write((char*)pl.szChatMessage,sizeof(pl.szChatMessage));

		return bos;
	}
	friend bistream& operator>>(bistream& bis,CMD_GR_UserChat& pl)
	{	
		pl.reset();
		bis >> pl.wChatLength;
		bis >> pl.crFontColor;
		bis >> pl.dwSendUserID;
		bis >> pl.dwTargetUserID;

		bis.read((char*)pl.szChatMessage,sizeof(pl.szChatMessage));

		return bis;
	}
};

//私语结构 
struct CMD_GR_Wisper
{
	enum {M_ID = MDM_GR_USER};
	enum {S_ID = SUB_GR_USER_WISPER};

	unsigned short							wChatLength;						//信息长度
	unsigned int						crFontColor;						//信息颜色
	unsigned int							dwSendUserID;						//发送用户
	unsigned int							dwTargetUserID;						//目标用户
	char							szChatMessage[MAX_CHAT_LEN];		//聊天信息

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GR_Wisper() { reset(); }
	friend bostream& operator<<(bostream& bos,const CMD_GR_Wisper& pl)
	{
		bos << pl.wChatLength;
		bos << pl.crFontColor;
		bos << pl.dwSendUserID;
		bos << pl.dwTargetUserID;

		bos.write((char*)pl.szChatMessage,sizeof(pl.szChatMessage));

		return bos;
	}
	friend bistream& operator>>(bistream& bis,CMD_GR_Wisper& pl)
	{	
		pl.reset();
		bis >> pl.wChatLength;
		bis >> pl.crFontColor;
		bis >> pl.dwSendUserID;
		bis >> pl.dwTargetUserID;

		bis.read((char*)pl.szChatMessage,sizeof(pl.szChatMessage));

		return bis;
	}
};

//用户规则
struct CMD_GR_UserRule
{
	enum {M_ID = MDM_GR_USER};
	enum {S_ID = SUB_GR_USER_RULE};

	bool							bPassword;							//设置密码
	bool							bLimitWin;							//限制胜率
	bool							bLimitFlee;							//限制断线
	bool							bLimitScore;						//限制分数
	bool							bCheckSameIP;						//效验地址
	unsigned short							wWinRate;							//限制胜率
	unsigned short							wFleeRate;							//限制逃跑
	int							lMaxScore;							//最高分数 
	int							lLessScore;							//最低分数
	char							szPassword[PASS_LEN];				//桌子密码

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GR_UserRule() { reset(); }
	friend bostream& operator<<(bostream& bos,const CMD_GR_UserRule& pl)
	{
		bos << pl.bPassword;
		bos << pl.bLimitWin;
		bos << pl.bLimitFlee;
		bos << pl.bLimitScore;
		bos << pl.bCheckSameIP;
		bos << pl.wWinRate;
		bos << pl.wFleeRate;
		bos << pl.lMaxScore;
		bos << pl.lLessScore;

		bos.write((char*)pl.szPassword,sizeof(pl.szPassword));

		return bos;
	}
	friend bistream& operator>>(bistream& bis,CMD_GR_UserRule& pl)
	{	
		pl.reset();
		bis >> pl.bPassword;
		bis >> pl.bLimitWin;
		bis >> pl.bLimitFlee;
		bis >> pl.bLimitScore;
		bis >> pl.wWinRate;
		bis >> pl.wFleeRate;
		bis >> pl.lMaxScore;
		bis >> pl.lLessScore;

		bis.read((char*)pl.szPassword,sizeof(pl.szPassword));

		return bis;
	}
};

//邀请用户 
struct CMD_GR_UserInvite
{
	enum {M_ID = MDM_GR_USER};
	enum {S_ID = SUB_GR_USER_INVITE};

	unsigned short							wTableID;							//桌子号码
	unsigned int							dwUserID;							//用户 I D

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GR_UserInvite() { reset(); }
	friend bostream& operator<<(bostream& bos,const CMD_GR_UserInvite& pl)
	{
		bos << pl.wTableID;
		bos << pl.dwUserID;

		return bos;
	}
	friend bistream& operator>>(bistream& bis,CMD_GR_UserInvite& pl)
	{	
		pl.reset();
		bis >> pl.wTableID;
		bis >> pl.dwUserID;

		return bis;
	}
};


// 请求校验发送喇叭内容
struct CMD_GR_UserCheckLabaReq
{
	enum { M_ID = MDM_GR_USER };
	enum { S_ID = SUB_GR_USER_CHECK_LABA_REQ };
	
	short			iLen;
	char			strLabaInfo[256];	// 喇叭内容

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GR_UserCheckLabaReq() { reset(); }
	friend bostream& operator<<(bostream& bos, const CMD_GR_UserCheckLabaReq& pl)
	{
		bos << pl.iLen;
		bos.write((char*)pl.strLabaInfo, pl.iLen);

		return bos;
	}
	friend bistream& operator>>(bistream& bis, CMD_GR_UserCheckLabaReq& pl)
	{
		bis >> pl.iLen;
		bis.read((char*)pl.strLabaInfo, pl.iLen);

		return bis;
	}
};

//校验喇叭结果
struct CMD_GR_UserCheckLabaRep
{
	enum { M_ID = MDM_GR_USER };
	enum { S_ID = SUB_GR_USER_CHECK_LABA_REP };

	short			nCode;				// 返回码：0为成功
	short			nLen;
	char			strLabaInfo[256];	// 喇叭内容

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GR_UserCheckLabaRep() { reset(); }
	friend bostream& operator<<(bostream& bos, const CMD_GR_UserCheckLabaRep& pl)
	{
		bos << pl.nCode;
		bos << pl.nLen;
		bos.write((char*)pl.strLabaInfo, pl.nLen);

		return bos;
	}
	friend bistream& operator>>(bistream& bis, CMD_GR_UserCheckLabaRep& pl)
	{
		bis >> pl.nCode;
		bis >> pl.nLen;
		bis.read((char*)pl.strLabaInfo, pl.nLen);

		return bis;
	}
};

// 请求发送喇叭
struct CMD_GR_UserSendLabaReq
{
	enum { M_ID = MDM_GR_USER };
	enum { S_ID = SUB_GR_USER_SEND_LABA_REQ};

	unsigned int					dwUserID;		//玩家 ID
	short							nLen;
	char							strLabaInfo[256];	// 喇叭内容

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GR_UserSendLabaReq() { reset(); }
	friend bostream& operator<<(bostream& bos, const CMD_GR_UserSendLabaReq& pl)
	{
		bos << pl.dwUserID;
		bos << pl.nLen;

		bos.write((char*)pl.strLabaInfo, pl.nLen);

		return bos;
	}
	friend bistream& operator>>(bistream& bis, CMD_GR_UserSendLabaReq& pl)
	{
		bis >> pl.dwUserID;
		bis >> pl.nLen;

		bis.read((char*)pl.strLabaInfo, pl.nLen);
		return bis;
	}
};

// 发送喇叭结果
struct CMD_GR_UserSendLabaRep
{
	enum { M_ID = MDM_GR_USER };
	enum { S_ID = SUB_GR_USER_SEND_LABA_REP };

	short							nCode;			// 返回码：0为成功
	unsigned int					dwUserID;		//玩家 ID
	unsigned int					dwCurScore;		//成功后的结果：当前带的游戏分数

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GR_UserSendLabaRep() { reset(); }
	friend bostream& operator<<(bostream& bos, const CMD_GR_UserSendLabaRep& pl)
	{
		bos << pl.nCode;
		bos << pl.dwUserID;
		bos << pl.dwCurScore;

		return bos;
	}
	friend bistream& operator>>(bistream& bis, CMD_GR_UserSendLabaRep& pl)
	{
		bis >> pl.nCode;
		bis >> pl.dwUserID;
		bis >> pl.dwCurScore;

		return bis;
	}
};

//接收到喇叭消息
struct CMD_GR_SpeakerMsgRep
{
	enum {M_ID = MDM_GR_USER};
	enum {S_ID = SUB_GR_USER_SPEAKER_MSG_REP};

	unsigned short						nType;					//消息类型
	unsigned short						nLen;					//消息长度
	char								szMsg[256];				//消息内容

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GR_SpeakerMsgRep() { reset(); }
	friend bostream& operator<<(bostream& bos,const CMD_GR_SpeakerMsgRep& pl)
	{
		bos << pl.nType;
		bos << pl.nLen;

		bos.write((char*)pl.szMsg, pl.nLen);

		return bos;
	}
	friend bistream& operator>>(bistream& bis,CMD_GR_SpeakerMsgRep& pl)
	{	
		pl.reset();
		bis >> pl.nType;
		bis >> pl.nLen;

		bis.read((char*)pl.szMsg, pl.nLen);

		return bis;
	}
};



//////////////////////////////////////////////////////////////////////////
//配置信息数据包

#define MDM_GR_INFO					3									//配置信息

#define SUB_GR_SERVER_INFO			100									//房间配置
#define SUB_GR_ORDER_INFO			101									//等级配置
#define SUB_GR_MEMBER_INFO			102									//会员配置
#define SUB_GR_COLUMN_INFO			103									//列表配置
#define SUB_GR_CONFIG_FINISH		104									//配置完成

//游戏房间信息
struct CMD_GR_ServerInfo
{
	enum {M_ID = MDM_GR_INFO};
	enum {S_ID = SUB_GR_SERVER_INFO};

	//房间属性
	unsigned short							wKindID;							//类型 I D
	unsigned short							wTableCount;						//桌子数目
	unsigned short							wChairCount;						//椅子数目
	unsigned int							dwVideoAddr;						//视频地址

	//扩展配置
	unsigned short							wGameGenre;							//游戏类型
	unsigned char							cbHideUserInfo;						//隐藏信息

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GR_ServerInfo() { reset(); }
	friend bostream& operator<<(bostream& bos,const CMD_GR_ServerInfo& pl)
	{
		bos << pl.wKindID;
		bos << pl.wTableCount;
		bos << pl.wChairCount;
		bos << pl.dwVideoAddr;
		bos << pl.wGameGenre;
		bos << pl.cbHideUserInfo;

		return bos;
	}
	friend bistream& operator>>(bistream& bis,CMD_GR_ServerInfo& pl)
	{	
		pl.reset();
		bis >> pl.wKindID;
		bis >> pl.wTableCount;
		bis >> pl.wChairCount;
		bis >> pl.dwVideoAddr;
		bis >> pl.wGameGenre;
		bis >> pl.cbHideUserInfo;

		return bis;
	}
};

//分数描述信息
struct CMD_GR_ScoreInfo_CB
{
	unsigned short							wDescribeCount;						//描述数目
	unsigned short							wDataDescribe[16];					//数据标志
};

//等级描述结构
struct tagOrderItem
{
	int							lScore;								//等级积分
	unsigned short							wOrderDescribe[16];					//等级描述

	void reset() { memset(this, 0, sizeof(*this)); }
	tagOrderItem() { reset(); }
	friend bostream& operator<<(bostream& bos,const tagOrderItem& pl)
	{
		bos << pl.lScore;

		bos.write((char*)pl.wOrderDescribe,sizeof(pl.wOrderDescribe));

		return bos;
	}
	friend bistream& operator>>(bistream& bis,tagOrderItem& pl)
	{	
		pl.reset();
		bis >> pl.lScore;

		bis.read((char*)pl.wOrderDescribe,sizeof(pl.wOrderDescribe));

		return bis;
	}
};

//等级描述信息
struct CMD_GR_OrderInfo
{
	enum {M_ID = MDM_GR_INFO};
	enum {S_ID = SUB_GR_ORDER_INFO};

	unsigned short							wOrderCount;						//等级数目
	tagOrderItem					OrderItem[128];						//等级描述

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GR_OrderInfo() { reset(); }
	friend bostream& operator<<(bostream& bos,const CMD_GR_OrderInfo& pl)
	{
		bos << pl.wOrderCount;

		bos.write((char*)pl.OrderItem,sizeof(pl.OrderItem));

		return bos;
	}
	friend bistream& operator>>(bistream& bis,CMD_GR_OrderInfo& pl)
	{	
		pl.reset();
		bis >> pl.wOrderCount;

		bis.read((char*)pl.OrderItem,sizeof(pl.OrderItem));

		return bis;
	}
};

//列表项描述结构
struct tagColumnItem
{
	unsigned short							wColumnWidth;						//列表宽度
	unsigned short							wDataDescribe;						//字段类型
	char							szColumnName[16];					//列表名字
};

//列表描述信息
struct CMD_GR_ColumnInfo
{
	enum {M_ID = MDM_GR_INFO};
	enum {S_ID = SUB_GR_COLUMN_INFO};

	unsigned short							wColumnCount;						//列表数目
	tagColumnItem					ColumnItem[32];						//列表描述

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GR_ColumnInfo() { reset(); }
	friend bostream& operator<<(bostream& bos,const CMD_GR_ColumnInfo& pl)
	{
		bos << pl.wColumnCount;
		for (int i=0; i<pl.wColumnCount ; i++ )
		{
			bos << pl.ColumnItem[i].wColumnWidth;
			bos << pl.ColumnItem[i].wDataDescribe;

			bos.write((char*)pl.ColumnItem[i].szColumnName,16);
		}

		return bos;
	}
	friend bistream& operator>>(bistream& bis,CMD_GR_ColumnInfo& pl)
	{	
		pl.reset();
		bis >> pl.wColumnCount;
		for (int i=0; i<pl.wColumnCount ; i++ )
		{
			bis >> pl.ColumnItem[i].wColumnWidth;
			bis >> pl.ColumnItem[i].wDataDescribe;

			bis.read((char*)pl.ColumnItem[i].szColumnName,16);
		}

		return bis;
	}
};

//////////////////////////////////////////////////////////////////////////
//房间状态数据包

#define MDM_GR_STATUS				4									//状态信息

#define SUB_GR_TABLE_INFO			100									//桌子信息
#define SUB_GR_TABLE_STATUS			101									//桌子状态

//桌子状态结构
struct tagTableStatus
{
	unsigned char							bTableLock;							//锁定状态
	unsigned char							bPlayStatus;						//游戏状态

	void reset() { memset(this, 0, sizeof(*this)); }
	tagTableStatus() { reset(); }
	friend bostream& operator<<(bostream& bos,const tagTableStatus& pl)
	{
		bos << pl.bTableLock;
		bos << pl.bPlayStatus;

		return bos;
	}
	friend bistream& operator>>(bistream& bis,tagTableStatus& pl)
	{	
		pl.reset();
		bis >> pl.bTableLock;
		bis >> pl.bPlayStatus;

		return bis;
	}
};

//桌子状态数组
struct CMD_GR_TableInfo
{
	enum {M_ID = MDM_GR_STATUS};
	enum {S_ID = SUB_GR_TABLE_INFO};

	unsigned short							wTableCount;						//桌子数目
	tagTableStatus					TableStatus[512];					//状态数组

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GR_TableInfo() { reset(); }
	friend bostream& operator<<(bostream& bos,const CMD_GR_TableInfo& pl)
	{
		bos << pl.wTableCount;
		for (int i=0; i<pl.wTableCount ; i++ )
		{
			bos << pl.TableStatus[i];
		}

		return bos;
	}
	friend bistream& operator>>(bistream& bis,CMD_GR_TableInfo& pl)
	{	
		pl.reset();
		bis >> pl.wTableCount;
		for (int i=0; i<pl.wTableCount ; i++ )
		{
			bis >> pl.TableStatus[i];
		}

		return bis;
	}
};

//桌子状态信息
struct CMD_GR_TableStatus
{
	enum {M_ID = MDM_GR_STATUS};
	enum {S_ID = SUB_GR_TABLE_STATUS};

	unsigned short							wTableID;							//桌子号码
	unsigned char							bTableLock;							//锁定状态
	unsigned char							bPlayStatus;						//游戏状态

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GR_TableStatus() { reset(); }
	friend bostream& operator<<(bostream& bos,const CMD_GR_TableStatus& pl)
	{
		bos << pl.wTableID;
		bos << pl.bTableLock;
		bos << pl.bPlayStatus;

		return bos;
	}
	friend bistream& operator>>(bistream& bis,CMD_GR_TableStatus& pl)
	{	
		pl.reset();
		bis >> pl.wTableID;
		bis >> pl.bTableLock;
		bis >> pl.bPlayStatus;

		return bis;
	}
};

//////////////////////////////////////////////////////////////////////////
//管理数据包

#define MDM_GR_MANAGER				5									//管理命令

#define SUB_GR_SEND_WARNING			1									//发送警告
#define SUB_GR_SEND_MESSAGE			2									//发送消息
#define SUB_GR_LOOK_USER_IP			3									//查看地址
#define SUB_GR_KILL_USER			4									//踢出用户
#define SUB_GR_LIMIT_ACCOUNS		5									//禁用帐户
#define SUB_GR_SET_USER_RIGHT		6									//权限设置
#define SUB_GR_OPTION_SERVER		7									//房间设置

//发送警告
struct CMD_GR_SendWarning
{
	enum {M_ID = MDM_GR_MANAGER};
	enum {S_ID = SUB_GR_SEND_WARNING};

	unsigned short							wChatLength;						//信息长度
	unsigned int							dwTargetUserID;						//目标用户
	char							szWarningMessage[MAX_CHAT_LEN];		//警告消息

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GR_SendWarning() { reset(); }
	friend bostream& operator<<(bostream& bos,const CMD_GR_SendWarning& pl)
	{
		bos << pl.wChatLength;
		bos << pl.dwTargetUserID;

		bos.write((char*)pl.szWarningMessage,sizeof(pl.szWarningMessage));

		return bos;
	}
	friend bistream& operator>>(bistream& bis,CMD_GR_SendWarning& pl)
	{	
		pl.reset();
		bis >> pl.wChatLength;
		bis >> pl.dwTargetUserID;

		bis.read((char*)pl.szWarningMessage,sizeof(pl.szWarningMessage));

		return bis;
	}
};

//系统消息
struct CMD_GR_SendMessage
{
	enum {M_ID = MDM_GR_MANAGER};
	enum {S_ID = SUB_GR_SEND_MESSAGE};

	unsigned char							cbGame;								//游戏消息
	unsigned char							cbRoom;								//游戏消息
	unsigned short							wChatLength;						//信息长度
	char							szSystemMessage[MAX_CHAT_LEN];		//系统消息

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GR_SendMessage() { reset(); }
	friend bostream& operator<<(bostream& bos,const CMD_GR_SendMessage& pl)
	{
		bos << pl.cbGame;
		bos << pl.cbRoom;
		bos << pl.wChatLength;

		bos.write((char*)pl.szSystemMessage,sizeof(pl.szSystemMessage));

		return bos;
	}
	friend bistream& operator>>(bistream& bis,CMD_GR_SendMessage& pl)
	{	
		pl.reset();
		bis >> pl.cbGame;
		bis >> pl.cbRoom;
		bis >> pl.wChatLength;

		bis.read((char*)pl.szSystemMessage,sizeof(pl.szSystemMessage));

		return bis;
	}
};

//查看地址
struct CMD_GR_LookUserIP
{
	enum {M_ID = MDM_GR_MANAGER};
	enum {S_ID = SUB_GR_LOOK_USER_IP};

	unsigned int							dwTargetUserID;						//目标用户

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GR_LookUserIP() { reset(); }
	friend bostream& operator<<(bostream& bos,const CMD_GR_LookUserIP& pl)
	{
		bos << pl.dwTargetUserID;

		return bos;
	}
	friend bistream& operator>>(bistream& bis,CMD_GR_LookUserIP& pl)
	{	
		pl.reset();
		bis >> pl.dwTargetUserID;

		return bis;
	}
};

//踢出用户
struct CMD_GR_KillUser
{
	enum {M_ID = MDM_GR_MANAGER};
	enum {S_ID = SUB_GR_KILL_USER};

	unsigned int							dwTargetUserID;						//目标用户

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GR_KillUser() { reset(); }
	friend bostream& operator<<(bostream& bos,const CMD_GR_KillUser& pl)
	{
		bos << pl.dwTargetUserID;

		return bos;
	}
	friend bistream& operator>>(bistream& bis,CMD_GR_KillUser& pl)
	{	
		pl.reset();
		bis >> pl.dwTargetUserID;

		return bis;
	}
};

//禁用帐户
struct CMD_GR_LimitAccounts
{
	enum {M_ID = MDM_GR_MANAGER};
	enum {S_ID = SUB_GR_LIMIT_ACCOUNS};

	unsigned int							dwTargetUserID;						//目标用户

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GR_LimitAccounts() { reset(); }
	friend bostream& operator<<(bostream& bos,const CMD_GR_LimitAccounts& pl)
	{
		bos << pl.dwTargetUserID;

		return bos;
	}
	friend bistream& operator>>(bistream& bis,CMD_GR_LimitAccounts& pl)
	{	
		pl.reset();
		bis >> pl.dwTargetUserID;

		return bis;
	}
};

//权限设置
struct CMD_GR_SetUserRight
{
	enum {M_ID = MDM_GR_MANAGER};
	enum {S_ID = SUB_GR_SET_USER_RIGHT};

	//目标用户
	unsigned int							dwTargetUserID;						//目标用户

	//绑定变量
	unsigned char							cbGameRight;						//帐号权限
	unsigned char							cbAccountsRight;					//帐号权限

	//权限变化
	unsigned char							cbLimitRoomChat;					//大厅聊天
	unsigned char							cbLimitGameChat;					//游戏聊天
	unsigned char							cbLimitPlayGame;					//游戏权限
	unsigned char							cbLimitSendWisper;					//发送消息
	unsigned char							cbLimitLookonGame;					//旁观权限

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GR_SetUserRight() { reset(); }
	friend bostream& operator<<(bostream& bos,const CMD_GR_SetUserRight& pl)
	{
		bos << pl.dwTargetUserID;
		bos << pl.cbGameRight;
		bos << pl.cbAccountsRight;
		bos << pl.cbLimitRoomChat;
		bos << pl.cbLimitGameChat;
		bos << pl.cbLimitPlayGame;
		bos << pl.cbLimitSendWisper;
		bos << pl.cbLimitLookonGame;

		return bos;
	}
	friend bistream& operator>>(bistream& bis,CMD_GR_SetUserRight& pl)
	{	
		pl.reset();
		bis >> pl.dwTargetUserID;
		bis >> pl.cbGameRight;
		bis >> pl.cbAccountsRight;
		bis >> pl.cbLimitRoomChat;
		bis >> pl.cbLimitGameChat;
		bis >> pl.cbLimitPlayGame;
		bis >> pl.cbLimitSendWisper;
		bis >> pl.cbLimitLookonGame;

		return bis;
	}

};

//设置标志
#define OSF_ROOM_CHAT				1									//大厅聊天
#define OSF_GAME_CHAT				2									//游戏聊天
#define OSF_ROOM_WISPER				3									//大厅私聊
#define OSF_ENTER_GAME				4									//进入游戏
#define OSF_ENTER_ROOM				5									//进入房间
#define OSF_SHALL_CLOSE				6									//即将关闭

//房间设置
struct CMD_GR_OptionServer
{
	enum {M_ID = MDM_GR_MANAGER};
	enum {S_ID = SUB_GR_OPTION_SERVER};

	unsigned char							cbOptionFlags;						//设置标志
	unsigned char							cbOptionValue;						//设置标志

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GR_OptionServer() { reset(); }
	friend bostream& operator<<(bostream& bos,const CMD_GR_OptionServer& pl)
	{
		bos << pl.cbOptionFlags;
		bos << pl.cbOptionValue;

		return bos;
	}
	friend bistream& operator>>(bistream& bis,CMD_GR_OptionServer& pl)
	{	
		pl.reset();
		bis >> pl.cbOptionFlags;
		bis >> pl.cbOptionValue;

		return bis;
	}
};

//////////////////////////////////////////////////////////////////////////
//系统数据包

#define MDM_GR_SYSTEM				10									//系统信息

#define SUB_GR_MESSAGE				100									//系统消息

//消息类型
#define SMT_INFO					0x0001								//信息消息
#define SMT_EJECT					0x0002								//弹出消息
#define SMT_GLOBAL					0x0004								//全局消息
#define SMT_CLOSE_ROOM				0x1000								//关闭房间
#define SMT_INTERMIT_LINE			0x4000								//中断连接

//消息数据包
struct CMD_GR_Message
{
	enum {M_ID = MDM_GR_SYSTEM};
	enum {S_ID = SUB_GR_MESSAGE};

	unsigned short							wMessageType;						//消息类型
	unsigned short							wMessageLength;						//消息长度
	char							szContent[1024];					//消息内容

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GR_Message() { reset(); }
	friend bostream& operator<<(bostream& bos,const CMD_GR_Message& pl)
	{
		bos << pl.wMessageType;
		bos << pl.wMessageLength;

		bos.write((char*)pl.szContent,sizeof(pl.szContent));

		return bos;
	}
	friend bistream& operator>>(bistream& bis,CMD_GR_Message& pl)
	{	
		pl.reset();
		bis >> pl.wMessageType;
		bis >> pl.wMessageLength;

		bis.read((char*)pl.szContent,sizeof(pl.szContent));

		return bis;
	}
};

//////////////////////////////////////////////////////////////////////////
//房间数据包

#define MDM_GR_SERVER_INFO			11									//房间信息

#define SUB_GR_ONLINE_COUNT_INFO	100									//在线信息

//人数信息
struct tagOnLineCountInfo
{
	enum {M_ID = MDM_GR_SERVER_INFO};
	enum {S_ID = SUB_GR_ONLINE_COUNT_INFO};

	unsigned short							wKindID;							//类型标识
	unsigned int							dwOnLineCount;						//在线人数

	void reset() { memset(this, 0, sizeof(*this)); }
	tagOnLineCountInfo() { reset(); }
	friend bostream& operator<<(bostream& bos,const tagOnLineCountInfo& pl)
	{
		bos << pl.wKindID;
		bos << pl.dwOnLineCount;

		return bos;
	}
	friend bistream& operator>>(bistream& bis,tagOnLineCountInfo& pl)
	{	
		pl.reset();
		bis >> pl.wKindID;
		bis >> pl.dwOnLineCount;

		return bis;
	}
};

//////////////////////////////////////////////////////////////////////////

#endif