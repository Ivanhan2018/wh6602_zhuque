#ifndef CMD_PLAZA_HEAD_FILE
#define CMD_PLAZA_HEAD_FILE

#include "biostream.h"

#include "GlobalDef.h"

//////////////////////////////////////////////////////////////////////////

//广场版本
#define VER_PLAZA_LOW					1								//广场版本
#define VER_PLAZA_HIGH					1								//广场版本
#define VER_PLAZA_FRAME					MAKELONG(VER_PLAZA_LOW,VER_PLAZA_HIGH)

//////////////////////////////////////////////////////////////////////////
//登录错误标识

#define ERC_GP_LOGON_SUCCESS			0								//登陆成功
#define ERC_GP_ACCOUNTS_NOT_EXIST		1								//帐号不存在
#define ERC_GP_LONG_NULLITY				2								//禁止登录
#define ERC_GP_PASSWORD_ERCOR			3								//密码错误

//////////////////////////////////////////////////////////////////////////
//登录命令码

#define MDM_GP_LOGON					1								//广场登录

#define SUB_GP_LOGON_ACCOUNTS			1								//帐号登录
#define SUB_GP_LOGON_USERID				2								//I D 登录
#define SUB_GP_REGISTER_ACCOUNTS		3								//注册帐号
#define SUB_GP_UPLOAD_CUSTOM_FACE		4								//定义头像
#define SUB_GP_LOGON_WX					6								//微信
#define SUB_GP_WXUPDATE					7								//升级成WX帐号


#define SUB_GP_HEART_BEAT				10								//心跳

#define SUB_GP_LOGON_SUCCESS			100								//登陆成功
#define SUB_GP_LOGON_ERROR				101								//登陆失败
#define SUB_GP_LOGON_FINISH				102								//登陆完成

//微信信息登录
struct CMD_GP_LogonByWX
{
	enum { M_ID = MDM_GP_LOGON };
	enum { S_ID = SUB_GP_LOGON_WX };
	unsigned int						dwPlazaVersion;					//广场版本
	char								cSex;							//性别
	unsigned short						cUrlSize;						//URL长度
	char								szFaceUrl[256];					//头像地址
	char								szSpreader[NAME_LEN];			//推广人名
	char								szNickName[NAME_LEN];			//昵称
	char								szOpenID[32];					//微信唯一标识
	char								szUnionID[64];					//微信唯一标识
	char								szAccounts[NAME_LEN];			//登录帐号
	char								szPassWord[PASS_LEN];			//登录密码
	char								szWXComputerID[COMPUTER_ID_LEN];//机器码(微信信息的唯一编码)
	char								szComputerID[COMPUTER_ID_LEN];	//硬件标识

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GP_LogonByWX() { reset(); }
	friend bostream& operator<<(bostream& bos, const CMD_GP_LogonByWX& pl)
	{
		bos << pl.dwPlazaVersion;
		bos << pl.cSex;
		bos << pl.cUrlSize;

		bos.write((char*)pl.szFaceUrl, pl.cUrlSize);
		bos.write((char*)pl.szSpreader, sizeof(pl.szSpreader));
		bos.write((char*)pl.szNickName, sizeof(pl.szNickName));
		bos.write((char*)pl.szOpenID, sizeof(pl.szOpenID));
		bos.write((char*)pl.szUnionID, sizeof(pl.szUnionID));
		bos.write((char*)pl.szAccounts, sizeof(pl.szAccounts));
		bos.write((char*)pl.szPassWord, sizeof(pl.szPassWord));
		bos.write((char*)pl.szWXComputerID, sizeof(pl.szWXComputerID));
		bos.write((char*)pl.szComputerID, sizeof(pl.szComputerID));

		return bos;
	}
	friend bistream& operator>>(bistream& bis, CMD_GP_LogonByWX& pl)
	{
		pl.reset();
		bis >> pl.dwPlazaVersion;
		bis >> pl.cSex;
		bis >> pl.cUrlSize;

		bis.read((char*)pl.szFaceUrl, pl.cUrlSize);
		bis.read((char*)pl.szSpreader, sizeof(pl.szSpreader));
		bis.read((char*)pl.szNickName, sizeof(pl.szNickName));
		bis.read((char*)pl.szOpenID, sizeof(pl.szOpenID));
		bis.read((char*)pl.szUnionID, sizeof(pl.szUnionID));
		bis.read((char*)pl.szAccounts, sizeof(pl.szAccounts));
		bis.read((char*)pl.szPassWord, sizeof(pl.szPassWord));
		bis.read((char*)pl.szWXComputerID, sizeof(pl.szWXComputerID));
		bis.read((char*)pl.szComputerID, sizeof(pl.szComputerID));

		return bis;
	}
};

//升级成WX帐号
struct CMD_GP_WXUpdate
{
	enum { M_ID = MDM_GP_LOGON };
	enum { S_ID = SUB_GP_WXUPDATE };
	unsigned int						dwUserID;						//用户 I D
	char								cSex;							//性别
	unsigned short						cUrlSize;						//URL长度
	char								szFaceUrl[256];					//头像地址
	char								szNickName[NAME_LEN];			//昵称
	char								szOpenID[32];					//微信唯一标识
	char								szUnionID[64];					//微信唯一标识
	char								szWXComputerID[COMPUTER_ID_LEN];//机器码(微信信息的唯一编码)
	char								szComputerID[COMPUTER_ID_LEN];			//硬件标识


	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GP_WXUpdate() { reset(); }
	friend bostream& operator<<(bostream& bos, const CMD_GP_WXUpdate& pl)
	{
		bos << pl.dwUserID;
		bos << pl.cSex;
		bos << pl.cUrlSize;

		bos.write((char*)pl.szFaceUrl, pl.cUrlSize);
		bos.write((char*)pl.szNickName, sizeof(pl.szNickName));
		bos.write((char*)pl.szOpenID, sizeof(pl.szOpenID));
		bos.write((char*)pl.szUnionID, sizeof(pl.szUnionID));
		bos.write((char*)pl.szWXComputerID, sizeof(pl.szWXComputerID));
		bos.write((char*)pl.szComputerID, sizeof(pl.szComputerID));

		return bos;
	}
	friend bistream& operator>>(bistream& bis, CMD_GP_WXUpdate& pl)
	{
		pl.reset();
		bis >> pl.dwUserID;
		bis >> pl.cSex;
		bis >> pl.cUrlSize;

		bis.read((char*)pl.szFaceUrl, pl.cUrlSize);
		bis.read((char*)pl.szNickName, sizeof(pl.szNickName));
		bis.read((char*)pl.szOpenID, sizeof(pl.szOpenID));
		bis.read((char*)pl.szUnionID, sizeof(pl.szUnionID));
		bis.read((char*)pl.szWXComputerID, sizeof(pl.szWXComputerID));
		bis.read((char*)pl.szComputerID, sizeof(pl.szComputerID));

		return bis;
	}
};

//帐号登录
struct CMD_GP_LogonByAccounts_CB
{
	enum {M_ID = MDM_GP_LOGON};
	enum {S_ID = SUB_GP_LOGON_ACCOUNTS};
	unsigned int						dwPlazaVersion;					//广场版本
	char								szAccounts[NAME_LEN];			//登录帐号
	char								szPassWord[PASS_LEN];			//登录密码
	char								szComputerID[COMPUTER_ID_LEN];			//硬件标识
	

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GP_LogonByAccounts_CB() { reset(); }
	friend bostream& operator<<(bostream& bos,const CMD_GP_LogonByAccounts_CB& pl)
	{
		bos << pl.dwPlazaVersion;

		bos.write((char*)pl.szAccounts,sizeof(pl.szAccounts));
		bos.write((char*)pl.szPassWord,sizeof(pl.szPassWord));
		bos.write((char*)pl.szComputerID,sizeof(pl.szComputerID));

		return bos;
	}
	friend bistream& operator>>(bistream& bis,CMD_GP_LogonByAccounts_CB& pl)
	{	
		pl.reset();
		bis >> pl.dwPlazaVersion;

		bis.read((char*)pl.szAccounts,sizeof(pl.szAccounts));
		bis.read((char*)pl.szPassWord,sizeof(pl.szPassWord));
		bis.read((char*)pl.szComputerID,sizeof(pl.szComputerID));

		return bis;
	}
};

//I D 登录
struct CMD_GP_LogonByUserID_CB
{
	enum {M_ID = MDM_GP_LOGON};
	enum {S_ID = SUB_GP_LOGON_USERID};

	unsigned int						dwPlazaVersion;					//广场版本
	unsigned int						dwUserID;						//用户 I D
	char								szPassWord[PASS_LEN];			//登录密码

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GP_LogonByUserID_CB() { reset(); }
	friend bostream& operator<<(bostream& bos,const CMD_GP_LogonByUserID_CB& pl)
	{
		bos << pl.dwPlazaVersion;
		bos << pl.dwUserID;

		bos.write((char*)pl.szPassWord,sizeof(pl.szPassWord));

		return bos;
	}
	friend bistream& operator>>(bistream& bis,CMD_GP_LogonByUserID_CB& pl)
	{	
		pl.reset();
		bis >> pl.dwPlazaVersion;
		bis >> pl.dwUserID;

		bis.read((char*)pl.szPassWord,sizeof(pl.szPassWord));

		return bis;
	}
};

//注册帐号
struct CMD_GP_RegisterAccounts_CB
{
	enum {M_ID = MDM_GP_LOGON};
	enum {S_ID = SUB_GP_REGISTER_ACCOUNTS};

	unsigned short						wFaceID;						//头像标识
	unsigned char						cbGender;						//用户性别
	unsigned int						dwPlazaVersion;					//广场版本
	char								szSpreader[NAME_LEN];			//推广人名
	char								szAccounts[NAME_LEN];			//登录帐号
	char								szPassWord[PASS_LEN];			//登录密码
	char								szComputerID[COMPUTER_ID_LEN];			//硬件标识

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GP_RegisterAccounts_CB() { reset(); }
	friend bostream& operator<<(bostream& bos,const CMD_GP_RegisterAccounts_CB& pl)
	{
		bos << pl.wFaceID;
		bos << pl.cbGender;
		bos << pl.dwPlazaVersion;

		bos.write((char*)pl.szSpreader,sizeof(pl.szSpreader));
		bos.write((char*)pl.szAccounts,sizeof(pl.szAccounts));
		bos.write((char*)pl.szPassWord,sizeof(pl.szPassWord));
		bos.write((char*)pl.szComputerID,sizeof(pl.szComputerID));

		return bos;
	}
	friend bistream& operator>>(bistream& bis,CMD_GP_RegisterAccounts_CB& pl)
	{	
		pl.reset();
		bis >> pl.wFaceID;
		bis >> pl.cbGender;
		bis >> pl.dwPlazaVersion;

		bis.read((char*)pl.szSpreader,sizeof(pl.szSpreader));
		bis.read((char*)pl.szAccounts,sizeof(pl.szAccounts));
		bis.read((char*)pl.szPassWord,sizeof(pl.szPassWord));
		bis.read((char*)pl.szComputerID,sizeof(pl.szComputerID));

		return bis;
	}
};

//#define USE_UNDERWRITE
//登陆成功
struct CMD_GP_LogonSuccess_CB
{
	enum {M_ID = MDM_GP_LOGON};
	enum {S_ID = SUB_GP_LOGON_SUCCESS};

	char										szAccounts[NAME_LEN];			//游客注册时返回登录帐号
	char										szNickName[NAME_LEN];			//返回昵称
	char										szPassWord[PASS_LEN];			//游客注册时返回明文登录密码
#ifdef USE_UNDERWRITE
	char							            szUnderWrite[UNDER_WRITE_LEN];	//个性签名
#endif

	unsigned short								wFaceID;						//头像索引
	unsigned char								cbGender;						//用户性别
	unsigned char								cbIsGuest;						//是否游客
	unsigned char								cbMember;						//会员等级
	unsigned int								dwUserID;						//用户 I D
	unsigned int								dwGameID;						//游戏 I D
	unsigned int								dwExperience;					//用户经验
	unsigned int								dwGoldScore;					//用户财富
	int											lWelfare;						//用户低保
	unsigned short								wWelfareLost;					//用户低保剩余次数
	
	//扩展信息
	unsigned int								dwCustomFaceVer;				//头像版本

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GP_LogonSuccess_CB() { reset(); }
	friend bostream& operator<<(bostream& bos,const CMD_GP_LogonSuccess_CB& pl)
	{

		bos.write((char*)pl.szAccounts,sizeof(pl.szAccounts));
		bos.write((char*)pl.szNickName,sizeof(pl.szNickName));
		bos.write((char*)pl.szPassWord,sizeof(pl.szPassWord));
#ifdef USE_UNDERWRITE
		bos.write((char*)pl.szUnderWrite,sizeof(pl.szUnderWrite));
#endif

		bos << pl.wFaceID;
		bos << pl.cbGender;
		bos << pl.cbIsGuest;
		bos << pl.cbMember;
		bos << pl.dwUserID;
		bos << pl.dwGameID;
		bos << pl.dwExperience;
		bos << pl.dwGoldScore;
		bos << pl.lWelfare;
		bos << pl.wWelfareLost;
		bos << pl.dwCustomFaceVer;

		return bos;
	}
	friend bistream& operator>>(bistream& bis,CMD_GP_LogonSuccess_CB& pl)
	{	
		pl.reset(); 


		bis.read((char*)pl.szAccounts,sizeof(pl.szAccounts));
		bis.read((char*)pl.szNickName,sizeof(pl.szNickName));
		bis.read((char*)pl.szPassWord,sizeof(pl.szPassWord));
#ifdef USE_UNDERWRITE
		bis.read((char*)pl.szUnderWrite,sizeof(pl.szUnderWrite));
#endif

		bis >> pl.wFaceID;
		bis >> pl.cbGender;
		bis >> pl.cbIsGuest;
		bis >> pl.cbMember;
		bis >> pl.dwUserID;
		bis >> pl.dwGameID;
		bis >> pl.dwExperience;
		bis >> pl.dwGoldScore;
		bis >> pl.lWelfare;
		bis >> pl.wWelfareLost;
		bis >> pl.dwCustomFaceVer;

		return bis;
	}
};

//登陆失败
struct CMD_GP_LogonError_CB
{
	enum {M_ID = MDM_GP_LOGON};
	enum {S_ID = SUB_GP_LOGON_ERROR};

	int								lErrorCode;						//错误代码
	char							szErrorDescribe[128];			//错误消息

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GP_LogonError_CB() { reset(); }
	friend bostream& operator<<(bostream& bos,const CMD_GP_LogonError_CB& pl)
	{
		bos << pl.lErrorCode;

		bos.write((char*)pl.szErrorDescribe,sizeof(pl.szErrorDescribe));

		return bos;
	}
	friend bistream& operator>>(bistream& bis,CMD_GP_LogonError_CB& pl)
	{	
		pl.reset();
		bis >> pl.lErrorCode;

		bis.read((char*)pl.szErrorDescribe,sizeof(pl.szErrorDescribe));

		return bis;
	}
};

//////////////////////////////////////////////////////////////////////////
//游戏列表命令码

#define MDM_GP_SERVER_LIST				2								//列表信息

#define SUB_GP_LIST_TYPE				100								//类型列表
#define SUB_GP_LIST_KIND				101								//种类列表
#define SUB_GP_LIST_STATION				102								//站点列表
#define SUB_GP_LIST_SERVER				103								//房间列表
#define SUB_GP_LIST_FINISH				104								//发送完成
#define SUB_GP_LIST_CONFIG				105								//列表配置
#define SUB_GP_CREATE_SELF_REQ			106								//请求创建自建场
#define SUB_GP_CREATE_SELF_REP			107								//创建自建场结果
#define SUB_GP_QUERY_SELF_REQ			108								//请求查询自建场
#define SUB_GP_QUERY_SELF_REP			109								//查询自建场结果
#define SUB_GP_CHECK_PWD_SELF_REQ		110								//请求自建场校验密码
#define SUB_GP_CHECK_PWD_SELF_REP		111								//自建场校验密码结果
#define SUB_GP_LIST_CONFIG_V2			112								//列表配置


#define SUB_GP_CREATE_FRIEND_REQ		120								//请求创建好友房
#define SUB_GP_CREATE_FRIEND_REP		121								//创建好友房结果
#define SUB_GP_ENTER_FRIEND_REQ			122								//请求进入好友房
#define SUB_GP_ENTER_FRIEND_REP			123								//进入好友房结果


//请求创建好友房
struct CMD_GP_CreateFriend
{
	enum { M_ID = MDM_GP_SERVER_LIST };
	enum { S_ID = SUB_GP_CREATE_FRIEND_REQ };

	unsigned short						nKindID;					//游戏类型
	unsigned short						nLen;						//规则长度
	char								szRule[512];				//配置参数:名值对，不建议太长

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GP_CreateFriend() { reset(); }
	friend bostream& operator<<(bostream& bos, const CMD_GP_CreateFriend& pl)
	{
		bos << pl.nKindID;
		bos << pl.nLen;

		bos.write((char*)pl.szRule, pl.nLen);

		return bos;
	}
	friend bistream& operator>>(bistream& bis, CMD_GP_CreateFriend& pl)
	{
		pl.reset();
		bis >> pl.nKindID;
		bis >> pl.nLen;

		bis.read((char*)pl.szRule, pl.nLen);

		return bis;
	}
};

//创建好友房结果
struct CMD_GP_CreateFriendRep
{
	enum { M_ID = MDM_GP_SERVER_LIST };
	enum { S_ID = SUB_GP_CREATE_FRIEND_REP };

	unsigned short						nCode;					//结果0：成功， 1：失败， 2：用户已在房间中
	unsigned int						dwUserID;				//房主id
	unsigned short						nServerID;				//房间ID
	unsigned short						nTableID;				//桌号
	char								szRoomID[NAME_LEN];		//房间唯一ID
	unsigned short						nLen;					//规则长度
	char								szRule[512];			//配置参数:名值对，不建议太长

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GP_CreateFriendRep() { reset(); }
	friend bostream& operator<<(bostream& bos, const CMD_GP_CreateFriendRep& pl)
	{
		bos << pl.nCode;
		bos << pl.dwUserID;
		bos << pl.nServerID;
		bos << pl.nTableID;

		bos.write((char*)pl.szRoomID, sizeof(pl.szRoomID));

		bos << pl.nLen;
		bos.write((char*)pl.szRule, pl.nLen);

		return bos;
	}
	friend bistream& operator>>(bistream& bis, CMD_GP_CreateFriendRep& pl)
	{
		pl.reset();
		bis >> pl.nCode;
		bis >> pl.dwUserID;
		bis >> pl.nServerID;
		bis >> pl.nTableID;

		bis.read((char*)pl.szRoomID, sizeof(pl.szRoomID));

		bis >> pl.nLen;
		bis.read((char*)pl.szRule, pl.nLen);

		return bis;
	}
};


//请求进入好友房
struct CMD_GP_EnterFriend
{
	enum { M_ID = MDM_GP_SERVER_LIST };
	enum { S_ID = SUB_GP_ENTER_FRIEND_REQ };

	char								szRoomID[NAME_LEN];			//房间唯一ID
	char								szPassword[PASS_LEN];		//密码

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GP_EnterFriend() { reset(); }
	friend bostream& operator<<(bostream& bos, const CMD_GP_EnterFriend& pl)
	{
		bos.write((char*)pl.szRoomID, sizeof(pl.szRoomID));
		bos.write((char*)pl.szPassword, sizeof(pl.szPassword));

		return bos;
	}
	friend bistream& operator>>(bistream& bis, CMD_GP_EnterFriend& pl)
	{
		pl.reset();

		bis.read((char*)pl.szRoomID, sizeof(pl.szRoomID));
		bis.read((char*)pl.szPassword, sizeof(pl.szPassword));

		return bis;
	}
};

//进入好友房结果
struct CMD_GP_EnterFriendRep
{
	enum { M_ID = MDM_GP_SERVER_LIST };
	enum { S_ID = SUB_GP_ENTER_FRIEND_REP };

	unsigned short						nCode;					//结果0：成功， 1：失败， 2：用户已在房间中
	char								cType;					//表示消息的发起者。1：表示为登录后台自动发起
	unsigned int						dwUserID;				//房主id
	unsigned short						nServerID;				//房间ID
	unsigned short						nTableID;				//桌号
	char								szRoomID[NAME_LEN];		//房间唯一ID
	unsigned short						nLen;					//规则长度
	char								szRule[512];			//配置参数:名值对，不建议太长

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GP_EnterFriendRep() { reset(); }
	friend bostream& operator<<(bostream& bos, const CMD_GP_EnterFriendRep& pl)
	{
		bos << pl.nCode;
		bos << pl.cType;
		bos << pl.dwUserID;
		bos << pl.nServerID;
		bos << pl.nTableID;

		bos.write((char*)pl.szRoomID, sizeof(pl.szRoomID));
		bos << pl.nLen;
		bos.write((char*)pl.szRule, pl.nLen);

		return bos;
	}
	friend bistream& operator>>(bistream& bis, CMD_GP_EnterFriendRep& pl)
	{
		pl.reset();
		bis >> pl.nCode;
		bis >> pl.cType;
		bis >> pl.dwUserID;
		bis >> pl.nServerID;
		bis >> pl.nTableID;

		bis.read((char*)pl.szRoomID, sizeof(pl.szRoomID));
		bis >> pl.nLen;
		bis.read((char*)pl.szRule, pl.nLen);

		return bis;
	}
};


//请求创建自建场
struct CMD_GP_CreateSelf
{
	enum {M_ID = MDM_GP_SERVER_LIST};
	enum {S_ID = SUB_GP_CREATE_SELF_REQ};

	unsigned short						nServerID;					//房间ID
	char								szName[NAME_LEN];			//房间名称
	char								szPassword[PASS_LEN];		//密码
	char								szRule[128];				//配置参数:名值对，不建议太长

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GP_CreateSelf() { reset(); }
	friend bostream& operator<<(bostream& bos,const CMD_GP_CreateSelf& pl)
	{
		bos << pl.nServerID;

		bos.write((char*)pl.szName,sizeof(pl.szName));
		bos.write((char*)pl.szPassword,sizeof(pl.szPassword));
		bos.write((char*)pl.szRule,sizeof(pl.szRule));

		return bos;
	}
	friend bistream& operator>>(bistream& bis,CMD_GP_CreateSelf& pl)
	{	
		pl.reset();
		bis >> pl.nServerID;

		bis.read((char*)pl.szName,sizeof(pl.szName));
		bis.read((char*)pl.szPassword,sizeof(pl.szPassword));
		bis.read((char*)pl.szRule,sizeof(pl.szRule));

		return bis;
	}
};

//创建自建场结果
struct CMD_GP_CreateSelfRep
{
	enum {M_ID = MDM_GP_SERVER_LIST};
	enum {S_ID = SUB_GP_CREATE_SELF_REP};

	unsigned short						nCode;					//结果
	unsigned short						nServerID;				//房间ID
	char								szName[NAME_LEN];		//房间名称
	char								szRoomID[NAME_LEN];		//房间唯一ID
	char								szRule[128];			//配置参数:名值对，不建议太长

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GP_CreateSelfRep() { reset(); }
	friend bostream& operator<<(bostream& bos,const CMD_GP_CreateSelfRep& pl)
	{
		bos << pl.nCode;
		bos << pl.nServerID;

		bos.write((char*)pl.szName,sizeof(pl.szName));
		bos.write((char*)pl.szRoomID,sizeof(pl.szRoomID));
		bos.write((char*)pl.szRule,sizeof(pl.szRule));

		return bos;
	}
	friend bistream& operator>>(bistream& bis,CMD_GP_CreateSelfRep& pl)
	{	
		pl.reset();
		bis >> pl.nCode;
		bis >> pl.nServerID;

		bis.read((char*)pl.szName,sizeof(pl.szName));
		bis.read((char*)pl.szRoomID,sizeof(pl.szRoomID));
		bis.read((char*)pl.szRule,sizeof(pl.szRule));

		return bis;
	}
};

//请求校验房间密码
struct CMD_GP_CheckPwdSelf
{
	enum {M_ID = MDM_GP_SERVER_LIST};
	enum {S_ID = SUB_GP_CHECK_PWD_SELF_REQ};

	unsigned short						nServerID;					//房间ID
	char								szRoomID[NAME_LEN];			//房间唯一ID
	char								szPassword[PASS_LEN];		//密码

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GP_CheckPwdSelf() { reset(); }
	friend bostream& operator<<(bostream& bos,const CMD_GP_CheckPwdSelf& pl)
	{
		bos << pl.nServerID;

		bos.write((char*)pl.szRoomID,sizeof(pl.szRoomID));
		bos.write((char*)pl.szPassword,sizeof(pl.szPassword));

		return bos;
	}
	friend bistream& operator>>(bistream& bis,CMD_GP_CheckPwdSelf& pl)
	{	
		pl.reset();
		bis >> pl.nServerID;

		bis.read((char*)pl.szRoomID,sizeof(pl.szRoomID));
		bis.read((char*)pl.szPassword,sizeof(pl.szPassword));

		return bis;
	}
};

//请求校验房间密码结果
struct CMD_GP_CheckPwdSelfRep
{
	enum {M_ID = MDM_GP_SERVER_LIST};
	enum {S_ID = SUB_GP_CHECK_PWD_SELF_REP};

	unsigned short						nCode;						//结果
	unsigned short						nServerID;					//房间ID
	char								szRoomID[NAME_LEN];			//房间唯一ID

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GP_CheckPwdSelfRep() { reset(); }
	friend bostream& operator<<(bostream& bos,const CMD_GP_CheckPwdSelfRep& pl)
	{
		bos << pl.nCode;
		bos << pl.nServerID;

		bos.write((char*)pl.szRoomID,sizeof(pl.szRoomID));

		return bos;
	}
	friend bistream& operator>>(bistream& bis,CMD_GP_CheckPwdSelfRep& pl)
	{	
		pl.reset();
		bis >> pl.nCode;
		bis >> pl.nServerID;

		bis.read((char*)pl.szRoomID,sizeof(pl.szRoomID));

		return bis;
	}
};

//请求查询自建场
struct CMD_GP_QuerySelf
{
	enum {M_ID = MDM_GP_SERVER_LIST};
	enum {S_ID = SUB_GP_QUERY_SELF_REQ};

	unsigned short						nServerID;					//游戏ID
	unsigned short						nPage;						//分页数，从0开始，每页的条数，由服务端控制
	char								szName[NAME_LEN];			//房间名称：模糊查询条件
	char								szRoomID[NAME_LEN];			//房间编号：模糊查询条件
	char								szFilter[128];				//扩展的查询参数:名值对，不建议太长

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GP_QuerySelf() { reset(); }
	friend bostream& operator<<(bostream& bos,const CMD_GP_QuerySelf& pl)
	{
		bos << pl.nServerID;
		bos << pl.nPage;

		bos.write((char*)pl.szName,sizeof(pl.szName));
		bos.write((char*)pl.szRoomID,sizeof(pl.szRoomID));
		bos.write((char*)pl.szFilter,sizeof(pl.szFilter));

		return bos;
	}
	friend bistream& operator>>(bistream& bis,CMD_GP_QuerySelf& pl)
	{	
		pl.reset();
		bis >> pl.nServerID;
		bis >> pl.nPage;

		bis.read((char*)pl.szName,sizeof(pl.szName));
		bis.read((char*)pl.szRoomID,sizeof(pl.szRoomID));
		bis.read((char*)pl.szFilter,sizeof(pl.szFilter));

		return bis;
	}
};

//查询自建场结果
struct CMD_GP_QuerySelfRep
{
	enum {M_ID = MDM_GP_SERVER_LIST};
	enum {S_ID = SUB_GP_QUERY_SELF_REP};

	unsigned short						nCode;					//结果
	unsigned short						nServerID;				//房间ID
	unsigned short						nPage;					//页码
	unsigned short						nTotal;						//总的记录条数
	unsigned short						nCount;					//返回的实际记录条数，最多不能超过10个
	char								szName[10][NAME_LEN];	//房间名称。独立出来，因为可能有保留字符
	char								szExtend[640];			//附加参数，格式：id:1,min:100,max:20000,key:1|id:2,min:100,max:20000,key:0|id:5,min:200,max:20000,key:1
																//用|分隔不同的房间，用逗号分隔不同的属性

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GP_QuerySelfRep() { reset(); }
	friend bostream& operator<<(bostream& bos,const CMD_GP_QuerySelfRep& pl)
	{
		bos << pl.nCode;
		bos << pl.nServerID;
		bos << pl.nPage;
		bos << pl.nTotal;
		bos << pl.nCount;

		bos.write((char*)pl.szName,sizeof(pl.szName));
		bos.write((char*)pl.szExtend,sizeof(pl.szExtend));

		return bos;
	}
	friend bistream& operator>>(bistream& bis,CMD_GP_QuerySelfRep& pl)
	{	
		pl.reset();
		bis >> pl.nCode;
		bis >> pl.nServerID;
		bis >> pl.nPage;
		bis >> pl.nTotal;
		bis >> pl.nCount;

		bis.read((char*)pl.szName,sizeof(pl.szName));
		bis.read((char*)pl.szExtend,sizeof(pl.szExtend));

		return bis;
	}
};

//列表配置
struct CMD_GP_ListConfig_CB
{
	enum {M_ID = MDM_GP_SERVER_LIST};
	enum {S_ID = SUB_GP_LIST_CONFIG};

	unsigned char						bShowOnLineCount;		//显示人数
	char								szRule[512];			//配置参数。目前有微信分享任务、签到、每日任务配置，推广

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GP_ListConfig_CB() { reset(); }
	friend bostream& operator<<(bostream& bos,const CMD_GP_ListConfig_CB& pl)
	{
		bos << pl.bShowOnLineCount;

		bos.write((char*)pl.szRule,sizeof(pl.szRule));

		return bos;
	}
	friend bistream& operator>>(bistream& bis,CMD_GP_ListConfig_CB& pl)
	{	
		pl.reset();
		bis >> pl.bShowOnLineCount;

		bis.read((char*)pl.szRule,sizeof(pl.szRule));

		return bis;
	}
};

//////////////////////////////////////////////////////////////////////////



//游戏类型结构
struct tagGameType
{
	enum {M_ID = MDM_GP_SERVER_LIST};
	enum {S_ID = SUB_GP_LIST_TYPE};

	unsigned short								wSortID;							//排序号码
	unsigned short								wTypeID;							//种类号码
	char								szTypeName[TYPE_LEN];				//种类名字

	void reset() { memset(this, 0, sizeof(*this)); }
	tagGameType() { reset(); }
	friend bostream& operator<<(bostream& bos,const tagGameType& pl)
	{
		bos << pl.wSortID;
		bos << pl.wTypeID;

		bos.write((char*)pl.szTypeName,sizeof(pl.szTypeName));

		return bos;
	}
	friend bistream& operator>>(bistream& bis,tagGameType& pl)
	{	
		pl.reset();
		bis >> pl.wSortID;
		bis >> pl.wTypeID;

		bis.read((char*)pl.szTypeName,sizeof(pl.szTypeName));

		return bis;
	}
};

//游戏名称结构
struct tagGameKind
{
	enum {M_ID = MDM_GP_SERVER_LIST};
	enum {S_ID = SUB_GP_LIST_KIND};

	unsigned short								wSortID;							//排序号码
	unsigned short								wTypeID;							//类型号码
	unsigned short								wKindID;							//名称号码
	unsigned int								dwMaxVersion;						//最新版本
	unsigned int								dwOnLineCount;						//在线数目
	char								szKindName[KIND_LEN];				//游戏名字
	char								szProcessName[MODULE_LEN];			//进程名字

	void reset() { memset(this, 0, sizeof(*this)); }
	tagGameKind() { reset(); }
	friend bostream& operator<<(bostream& bos,const tagGameKind& pl)
	{
		bos << pl.wSortID;
		bos << pl.wTypeID;
		bos << pl.wKindID;
		bos << pl.dwMaxVersion;
		bos << pl.dwOnLineCount;

		bos.write((char*)pl.szKindName,sizeof(pl.szKindName));
		bos.write((char*)pl.szProcessName,sizeof(pl.szProcessName));

		return bos;
	}
	friend bistream& operator>>(bistream& bis,tagGameKind& pl)
	{	
		pl.reset();
		bis >> pl.wSortID;
		bis >> pl.wTypeID;
		bis >> pl.wKindID;
		bis >> pl.dwMaxVersion;
		bis >> pl.dwOnLineCount;

		bis.read((char*)pl.szKindName,sizeof(pl.szKindName));
		bis.read((char*)pl.szProcessName,sizeof(pl.szProcessName));

		return bis;
	}
};

//游戏站点结构
struct tagGameStation
{
	enum {M_ID = MDM_GP_SERVER_LIST};
	enum {S_ID = SUB_GP_LIST_STATION};

	unsigned short								wSortID;							//排序号码
	unsigned short								wKindID;							//名称号码
	unsigned short								wJoinID;							//挂接号码
	unsigned short								wStationID;							//站点号码
	char								szStationName[STATION_LEN];			//站点名称

	void reset() { memset(this, 0, sizeof(*this)); }
	tagGameStation() { reset(); }
	friend bostream& operator<<(bostream& bos,const tagGameStation& pl)
	{
		bos << pl.wSortID;
		bos << pl.wKindID;
		bos << pl.wJoinID;
		bos << pl.wStationID;

		bos.write((char*)pl.szStationName,sizeof(pl.szStationName));

		return bos;
	}
	friend bistream& operator>>(bistream& bis,tagGameStation& pl)
	{	
		pl.reset();
		bis >> pl.wSortID;
		bis >> pl.wKindID;
		bis >> pl.wJoinID;
		bis >> pl.wStationID;

		bis.read((char*)pl.szStationName,sizeof(pl.szStationName));

		return bis;
	}
};

//游戏房间列表结构
struct tagGameServer
{
	enum {M_ID = MDM_GP_SERVER_LIST};
	enum {S_ID = SUB_GP_LIST_SERVER};

	unsigned short								wSortID;							//排序号码
	unsigned short								wKindID;							//名称号码
	unsigned short								wServerID;							//房间号码
	unsigned short								wStationID;							//站点号码
	unsigned int								dwOnLineCount;						//在线人数
	unsigned short								wServerPort;						//房间端口
	unsigned int								dwServerAddr;						//房间地址
	char								szServerAddr[SERVER_LEN];			//房间地址
	char								szServerName[SERVER_LEN];			//房间名称
	char								szRule[128];				//房间配置

	void reset() { memset(this, 0, sizeof(*this)); }
	tagGameServer() { reset(); }
	friend bostream& operator<<(bostream& bos,const tagGameServer& pl)
	{
		bos << pl.wSortID;
		bos << pl.wKindID;
		bos << pl.wServerID;
		bos << pl.wStationID;
		bos << pl.wServerPort;
		bos << pl.dwServerAddr;
		bos << pl.dwOnLineCount;

		bos.write((char*)pl.szServerAddr,sizeof(pl.szServerAddr));
		bos.write((char*)pl.szServerName,sizeof(pl.szServerName));
		bos.write((char*)pl.szRule,sizeof(pl.szRule));

		return bos;
	}
	friend bistream& operator>>(bistream& bis,tagGameServer& pl)
	{	
		pl.reset();
		bis >> pl.wSortID;
		bis >> pl.wKindID;
		bis >> pl.wServerID;
		bis >> pl.wStationID;
		bis >> pl.wServerPort;
		bis >> pl.dwServerAddr;
		bis >> pl.dwOnLineCount;

		bis.read((char*)pl.szServerAddr,sizeof(pl.szServerAddr));
		bis.read((char*)pl.szServerName,sizeof(pl.szServerName));
		bis.read((char*)pl.szRule,sizeof(pl.szRule));

		return bis;
	}
};

//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//系统命令码

#define MDM_GP_SYSTEM					3								//系统命令

#define SUB_GP_VERSION					100								//版本通知
//#define SUB_SP_SYSTEM_MSG				101								//系统消息

//版本通知
struct CMD_GP_Version
{
	enum {M_ID = MDM_GP_SYSTEM};
	enum {S_ID = SUB_GP_VERSION};

	unsigned char								bNewVersion;					//更新版本
	unsigned char								bAllowConnect;					//允许连接

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GP_Version() { reset(); }
	friend bostream& operator<<(bostream& bos,const CMD_GP_Version& pl)
	{
		bos << pl.bNewVersion;
		bos << pl.bAllowConnect;

		return bos;
	}
	friend bistream& operator>>(bistream& bis,CMD_GP_Version& pl)
	{	
		pl.reset();
		bis >> pl.bNewVersion;
		bis >> pl.bAllowConnect;

		return bis;
	}
};

//////////////////////////////////////////////////////////////////////////

#define MDM_GP_USER						4								//用户信息

#define SUB_GP_USER_UPLOAD_FACE			100								//上传头像
#define SUB_GP_USER_DOWNLOAD_FACE		101								//下载头像
#define SUB_GP_UPLOAD_FACE_RESULT		102								//上传结果
#define SUB_GP_DELETE_FACE_RESULT		103								//删除结果
#define SUB_GP_CUSTOM_FACE_DELETE		104								//删除头像
#define SUB_GP_MODIFY_INDIVIDUAL		105								//个人资料
#define SUB_GP_MODIFY_INDIVIDUAL_RESULT	106								//修改结果

#define SUB_GP_CHECK_ID_REQ				107								//校验ID
#define SUB_GP_CHECK_ID_REP				108								//校验ID结果
#define SUB_GP_CHECK_NICKNAME_REQ		109								//校验昵称
#define SUB_GP_CHECK_NICKNAME_REP		110								//校验昵称结果
#define SUB_GP_REGISTER_REQ				111								//注册
#define SUB_GP_REGISTER_REP				112								//注册结果
#define SUB_GP_ID_UPDATE_REQ			113								//账号升级
#define SUB_GP_ID_UPDATE_REP			114								//账号升级结果
#define SUB_GP_RESET_PWD_REQ			115								//重置密码
#define SUB_GP_RESET_PWD_REP			116								//重置密码结果
#define SUB_GP_USER_INFO_REQ			117								//查询用户信息
#define SUB_GP_USER_INFO_REP			118								//查询用户信息结果
#define SUB_GP_REGISTER_CHANNEL_REQ		119								//注册--带渠道号

#define SUB_GP_MODIFY_UNDERWRITE		120							    //个性签名
#define SUB_GP_MODIFY_UNDERWRITE_RESULT	121								//修改结果
#define SUB_GP_UNDERWRITE		        122							    //个性签名

#define SUB_GP_MODIFY_PHONE		        123							    //微信账号绑定手机号
#define SUB_GP_MODIFY_PHONE_RESULT	    124								//修改手机号

#define SUB_GP_HASGUEST		            125							    //是否有游客账号

#define SUB_GP_USER_SIGNIN				200								//签到消息
#define SUB_GP_USER_SIGNIN_REQ			201								//签到请求
#define SUB_GP_USER_GETAWARD			202								//领奖消息
#define SUB_GP_USER_GETAWARD_REQ		203								//领奖请求

#define SUB_GP_USER_CAN_SPREAD_REQ		204								//查询用户是否能领推广奖励
#define SUB_GP_USER_CAN_SPREAD_REP		205								//查询用户是否能领推广奖励结果
#define SUB_GP_USER_GET_SPREAD_REQ		206								//用户领推广奖励
#define SUB_GP_USER_GET_SPREAD_REP		207								//用户领推广奖励结果
#define SUB_GP_USER_SPREADER_INFO_REQ	208								//查询推广信息
#define SUB_GP_USER_SPREADER_INFO_REP	209								//查询推广信息结果

#define SUB_GP_USER_BBS_POP_REQ			210								//弹窗公告查询
#define SUB_GP_USER_BBS_POP_REP			211								//弹窗公告信息结果
#define SUB_GP_USER_BBS_SCROLL_REQ		212								//滚动公告查询
#define SUB_GP_USER_BBS_SCROLL_REP		213								//滚动公告信息结果
#define SUB_GP_USER_BBS_LIST_REQ		214								//列表公告查询
#define SUB_GP_USER_BBS_LIST_REP		215								//列表公告信息结果
#define SUB_GP_USER_BBS_POP_REQ_V2		230								//弹窗公告查询V2
#define SUB_GP_USER_BBS_SCROLL_REQ_V2	231								//滚动公告查询V2
#define SUB_GP_USER_BBS_LIST_REQ_V2		232								//列表公告查询V2

#define SUB_GP_USER_TASK_INFO_REQ		216								//玩家任务信息查询
#define SUB_GP_USER_TASK_INFO_REP		217								//玩家任务信息结果

#define SUB_GP_USER_RANKING_REQ					220							//玩家排行榜查询
#define SUB_GP_USER_RANKING_RECORD				221							//玩家排行榜详细
#define SUB_GP_USER_RANKING_RECORD_FINISH		222							//玩家排行榜结束

#define SUB_GP_USER_FEEDBACK_REQ				223							//提交问题反馈
#define SUB_GP_USER_FEEDBACK_REP				224							//提交问题反馈结果
#define SUB_GP_USER_GET_FEEDBACK_REQ			225							//查询问题反馈
#define SUB_GP_USER_FEEDBACK_RECORD				226							//查询问题反馈详细
#define SUB_GP_USER_FEEDBACK_RECORD_FINISH		227							//查询问题反馈结束

#define SUB_GP_USER_RANKING_REQ_V2				235							//玩家排行榜查询
#define SUB_GP_USER_RANKING_RECORD_V2			236							//玩家排行榜详细

#define SUB_GP_USER_EXCHANGE_INFO				300							//兑换个人消息（返回金豆、手机号）
#define SUB_GP_USER_EXCHANGE_INFO_REQ			301							//兑换个人信息请求（查询金豆、手机号、更新手机号）
#define SUB_GP_USER_EXCHANGE_PRODUCT			302							//兑换产品列表 
#define SUB_GP_USER_EXCHANGE_PRODUCT_REQ		303							//兑换产品查询请求
#define SUB_GP_USER_EXCHANGE_PRODUCT_FINISH		304							//兑换产品列表结束
#define SUB_GP_USER_EXCHANGE					305							//兑换产品列表 
#define SUB_GP_USER_EXCHANGE_REQ				306							//兑换产品查询请求
#define SUB_GP_USER_EXCHANGE_RECORD				307							//兑换记录列表 
#define SUB_GP_USER_EXCHANGE_RECORD_REQ			308							//兑换记录查询请求
#define SUB_GP_USER_EXCHANGE_RECORD_FINISH		309							//兑换记录列表结束

#define SUB_GP_USER_EXCHANGE_PRODUCT_REQ_V2		340							//兑换产品查询请求v2
#define SUB_GP_USER_EXCHANGE_PRODUCT_V2			341							//兑换产品列表 V2
#define SUB_GP_USER_EXCHANGE_REQ_V2				342							//兑换产品查询请求V2
#define SUB_GP_USER_EXCHANGE_V2					343							//兑换产品列表 V2

#define SUB_GP_USER_INFO_ID_CARD_REQ			310							//查询用户身份证账号信息
#define SUB_GP_USER_INFO_ID_CARD_REP			311							//查询用户身份证账号信息结果

#define SUB_GP_USER_AUTHENTICA_REQ				312
#define SUB_GP_USER_AUTHENTICA_REP				313

#define SUB_GP_USER_MODIFY_RANKINGSTATUS		314							//设置玩家是否入排行榜
#define SUB_GP_USER_REP_MODIFY_RANKINGSTATUS	315							//设置玩家是否入排行榜返回信息结果
#define SUB_GP_USER_REP_RANKINGSTATUS			316							//玩家是否入排行榜信息结果

#define SUB_GP_USER_MALL_PRODUCT_REQ			317							//商城商品请求
#define SUB_GP_USER_MALL_PRODUCT_RESP			318							//商城商品列表
#define SUB_GP_USER_MALL_PRODUCT_RESP_FINISH	319							//商城商品列表结束
#define SUB_GP_USER_MALL_PLACE_ORDER_REQ		320							//购买商品下订单请求
#define SUB_GP_USER_MALL_PLACE_ORDER_RESP		321							//购买商品下订单返回结果
#define SUB_GP_USER_CANCEL_ORDER_REQ			322							//取消订单号请求
#define SUB_GP_USER_CANCEL_ORDER_REP			323							//取消订单结果
#define SUB_GP_USER_MALL_BUY_REQ				324							//请求购买商品结果
#define SUB_GP_USER_MALL_BUY_RESP				325							//购买商品结果
#define SUB_GP_USER_MALL_UPDATE_REQ				326							//请求购买商品结果
#define SUB_GP_USER_MALL_UPDATE_RESP			327							//购买商品结果
#define SUB_GP_USER_MALL_PRODUCT_REQ_IOS		328							//商城商品请求

#define SUB_GP_USER_BANK_INFO_REQ				330							//查询用户银行信息
#define SUB_GP_USER_BANK_INFO_REP				331							//查询用户银行信息结果
#define SUB_GP_USER_BANK_CHARGE_REQ				332							//用户操作银行请求
#define SUB_GP_USER_BANK_CHARGE_REP				333							//用户操作银行请求结果


#define SUB_GP_USER_MALL_PRODUCT_REQ_V2			360							//商城商品请求
#define SUB_GP_USER_MALL_PRODUCT_RESP_V2		361							//商城商品列表

#define SUB_GP_USER_ACTIVITY_LIST_REQ			350							//查询活动列表信息
#define SUB_GP_USER_ACTIVITY_LIST_REP			351							//查询活动列表信息结果
#define SUB_GP_USER_ACTIVITY_REQ				352							//查询指定活动信息
#define SUB_GP_USER_ACTIVITY_REP				353							//查询指定活动信息结果
#define SUB_GP_USER_ACTIVITY_LUCKY_REQ			354							//活动抽奖
#define SUB_GP_USER_ACTIVITY_LUCKY_REP			355							//活动抽奖结果
#define SUB_GP_USER_ACTIVITY_RECORD				356							//中奖记录列表 
#define SUB_GP_USER_ACTIVITY_RECORD_REQ			357							//中奖记录查询请求
#define SUB_GP_USER_ACTIVITY_RECORD_FINISH		358							//中奖记录列表结束

#define SUB_GP_USER_SEND_LABA_REQ				362							//请求发送喇叭
#define SUB_GP_USER_SEND_LABA_REP				363							//请求发送喇叭结果

#define SUB_GP_USER_CHECK_LABA_REQ				364							//请求校验喇叭内容
#define SUB_GP_USER_CHECK_LABA_REP				365							//校验喇叭结果

#define SUB_GP_USER_SPEAKER_MSG_REP				366							//接收到喇叭消息

#define SUB_GP_USER_QUERY_CONFIG_REQ			368							//查询某个配置
#define SUB_GP_USER_QUERY_CONFIG_REP			369							//返回某个配置

#define SUB_GP_USER_FRIEND_RESULT_REQ				370						//好友战绩排行榜查询
#define SUB_GP_USER_FRIEND_RESULT_RECORD			371						//好友战绩排行榜记录
#define SUB_GP_USER_FRIEND_RESULT_RECORD_FINISH		372						//好友战绩排行榜结束
#define SUB_GP_USER_FRIEND_RESULT_DETAIL_REQ		373						//好友战绩单次房间查询
#define SUB_GP_USER_FRIEND_RESULT_DETAIL_REP		374						//好友战绩单次房间查询结果
#define SUB_GP_USER_FRIEND_RESULT_DETAIL_REQ_V2		375						//好友战绩单次房间查询
#define SUB_GP_USER_FRIEND_RESULT_DETAIL_REP_V2		376						//好友战绩单次房间查询结果(增加分享码字段)

#define SUB_GP_USER_FRIEND_RESULT_REPLAY_REQ			377						//房间单局回放记录查询请求
#define SUB_GP_USER_FRIEND_RESULT_REPLAY_USER_REP		378						//房间单局回放记录查询结果，用户信息
#define SUB_GP_USER_FRIEND_RESULT_REPLAY_RULE_REP		379						//房间单局回放记录查询结果，规则及手牌
#define SUB_GP_USER_FRIEND_RESULT_REPLAY_REP			380						//房间单局回放记录查询结果，行牌过程


#define	SUB_GP_USER_EXCHANGE_FANGKA					400
#define SUB_GP_USER_EXCHANGE_REQ_Fangka				401
#define SUB_GP_USER_EXCHANGE_FANGKA_RST				402						//兑换产品列表 V2
#define SUB_GP_USER_EXCHANGE_RESULT					403							//兑换产品列表 V2
#define SUB_GP_USER_EXCHANGE_FANGKA_FINISH			404						// 结果

//好友战绩记录查询请求
struct CMD_GP_UserFriendResultReq
{
	enum { M_ID = MDM_GP_USER };
	enum { S_ID = SUB_GP_USER_FRIEND_RESULT_REQ };

	unsigned int					nPage;			//页码	
	unsigned int					dwUserID;		//玩家 ID

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GP_UserFriendResultReq() { reset(); }
	friend bostream& operator<<(bostream& bos, const CMD_GP_UserFriendResultReq& pl)
	{
		bos << pl.nPage;
		bos << pl.dwUserID;

		return bos;
	}
	friend bistream& operator>>(bistream& bis, CMD_GP_UserFriendResultReq& pl)
	{
		pl.reset();
		bis >> pl.nPage;
		bis >> pl.dwUserID;

		return bis;
	}
};

//好友战绩记录列表
struct CMD_GP_UserFriendResultRecord
{
	enum { M_ID = MDM_GP_USER };
	enum { S_ID = SUB_GP_USER_FRIEND_RESULT_RECORD };

	char							szGameNum[32];			//牌局的全局唯一编号	
	char							szDate[32];				//对战时间
	int								nPlayLen;				//玩家数:最多支持8个玩家
	char							szPlayName[8][32];		//玩家名称:最多支持8个玩家
	unsigned int					dwTotalScore[8];		//结束后的总积分:最多支持8个玩家
	unsigned int					nPage;					//页码	
	unsigned int					dwRoomID;				//房间号


	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GP_UserFriendResultRecord() { reset(); }
	friend bostream& operator<<(bostream& bos, const CMD_GP_UserFriendResultRecord& pl)
	{
		bos.write((char*)pl.szGameNum, sizeof(pl.szGameNum));
		bos.write((char*)pl.szDate, sizeof(pl.szDate));

		bos << pl.nPlayLen;
		bos.write((char*)pl.szPlayName, pl.nPlayLen * 32);
		bos.write((char*)pl.dwTotalScore, pl.nPlayLen * sizeof(int));

		bos << pl.nPage;
		bos << pl.dwRoomID;

		return bos;
	}
	friend bistream& operator>>(bistream& bis, CMD_GP_UserFriendResultRecord& pl)
	{
		pl.reset();

		bis.read((char*)pl.szGameNum, sizeof(pl.szGameNum));
		bis.read((char*)pl.szDate, sizeof(pl.szDate));

		bis >> pl.nPlayLen;
		bis.read((char*)pl.szPlayName, pl.nPlayLen * 32);
		bis.read((char*)pl.dwTotalScore, pl.nPlayLen * sizeof(int));

		bis >> pl.nPage;
		bis >> pl.dwRoomID;

		return bis;
	}
};

//好友战绩记录列表结束
struct CMD_GP_UserFriendResultRecordFinish
{
	enum { M_ID = MDM_GP_USER };
	enum { S_ID = SUB_GP_USER_FRIEND_RESULT_RECORD_FINISH };

	unsigned int					dwTotal;					//总数
	unsigned int					dwTotalofCurPage;			//当前页总数
	unsigned int					dwUserID;					//玩家 ID

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GP_UserFriendResultRecordFinish() { reset(); }
	friend bostream& operator<<(bostream& bos, const CMD_GP_UserFriendResultRecordFinish& pl)
	{
		bos << pl.dwTotal;
		bos << pl.dwTotalofCurPage;
		bos << pl.dwUserID;

		return bos;
	}
	friend bistream& operator>>(bistream& bis, CMD_GP_UserFriendResultRecordFinish& pl)
	{
		pl.reset();

		bis >> pl.dwTotal;
		bis >> pl.dwTotalofCurPage;
		bis >> pl.dwUserID;

		return bis;
	}
};

//好友战绩单次房间查询请求
struct CMD_GP_UserFriendResultDetailReq
{
	enum { M_ID = MDM_GP_USER };
	enum { S_ID = SUB_GP_USER_FRIEND_RESULT_DETAIL_REQ };

	char							szGameNum[32];		//牌局的全局唯一编号

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GP_UserFriendResultDetailReq() { reset(); }
	friend bostream& operator<<(bostream& bos, const CMD_GP_UserFriendResultDetailReq& pl)
	{
		bos.write((char*)pl.szGameNum, sizeof(pl.szGameNum));

		return bos;
	}
	friend bistream& operator>>(bistream& bis, CMD_GP_UserFriendResultDetailReq& pl)
	{
		pl.reset();
		bis.read((char*)pl.szGameNum, sizeof(pl.szGameNum));

		return bis;
	}
};

//好友战绩单次房间查询结果
struct CMD_GP_UserFriendResultDetailRep
{
	enum { M_ID = MDM_GP_USER };
	enum { S_ID = SUB_GP_USER_FRIEND_RESULT_DETAIL_REP };

	unsigned int					dwRoomID;			//房间号

	int								nPlayLen;			//玩家数:最多支持8个玩家
	int								nDetailLen;			//记录条数:最多32局记录

	char							szPlayName[8][32];	//玩家名称:最多支持8个玩家
	char							szGameNum[32];		//牌局的全局唯一编号
	char							szDate[32][32];		//对战时间:最多32局记录
	int								dwScore[32][8];		//每局输赢分数:最多32局记录、最多支持8个玩家

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GP_UserFriendResultDetailRep() { reset(); }
	friend bostream& operator<<(bostream& bos, const CMD_GP_UserFriendResultDetailRep& pl)
	{
		bos << pl.dwRoomID;
		bos << pl.nPlayLen;
		bos << pl.nDetailLen;

		bos.write((char*)pl.szPlayName, pl.nPlayLen * 32);
		bos.write((char*)pl.szGameNum, sizeof(pl.szGameNum));

		bos.write((char*)pl.szDate, pl.nDetailLen * 32);
		for (int i = 0; i < pl.nDetailLen; i++)
		{
			bos.write((char*)pl.dwScore[i], pl.nPlayLen * sizeof(int));
		}

		return bos;
	}
	friend bistream& operator>>(bistream& bis, CMD_GP_UserFriendResultDetailRep& pl)
	{
		pl.reset();

		bis >> pl.dwRoomID;
		bis >> pl.nPlayLen;
		bis >> pl.nDetailLen;

		bis.read((char*)pl.szPlayName, pl.nPlayLen * 32);
		bis.read((char*)pl.szGameNum, sizeof(pl.szGameNum));

		bis.read((char*)pl.szDate, pl.nDetailLen * 32);
		for (int i = 0; i < pl.nDetailLen; i++)
		{
			bis.read((char*)pl.dwScore[i], pl.nPlayLen * sizeof(int));
		}

		return bis;
	}
};

//好友战绩单次房间查询请求
struct CMD_GP_UserFriendResultDetailReq_V2
{
	enum { M_ID = MDM_GP_USER };
	enum { S_ID = SUB_GP_USER_FRIEND_RESULT_DETAIL_REQ_V2 };

	char							szGameNum[32];		//牌局的全局唯一编号

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GP_UserFriendResultDetailReq_V2() { reset(); }
	friend bostream& operator<<(bostream& bos, const CMD_GP_UserFriendResultDetailReq_V2& pl)
	{
		bos.write((char*)pl.szGameNum, sizeof(pl.szGameNum));

		return bos;
	}
	friend bistream& operator>>(bistream& bis, CMD_GP_UserFriendResultDetailReq_V2& pl)
	{
		pl.reset();
		bis.read((char*)pl.szGameNum, sizeof(pl.szGameNum));

		return bis;
	}
};

//好友战绩单次房间查询结果
struct CMD_GP_UserFriendResultDetailRep_V2
{
	enum { M_ID = MDM_GP_USER };
	enum { S_ID = SUB_GP_USER_FRIEND_RESULT_DETAIL_REP_V2 };

	unsigned int					dwRoomID;			//房间号

	int								nPlayLen;			//玩家数:最多支持8个玩家
	int								nDetailLen;			//记录条数:最多32局记录

	char							szPlayName[8][32];	//玩家名称:最多支持8个玩家
	char							szGameNum[32];		//牌局的全局唯一编号
	char							szDate[32][32];		//对战时间:最多32局记录
	int								dwScore[32][8];		//每局输赢分数:最多32局记录、最多支持8个玩家
	int								iReplayID[32];		//每局的分享码ID，最多支持32局

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GP_UserFriendResultDetailRep_V2() { reset(); }
	friend bostream& operator<<(bostream& bos, const CMD_GP_UserFriendResultDetailRep_V2& pl)
	{
		bos << pl.dwRoomID;
		bos << pl.nPlayLen;
		bos << pl.nDetailLen;

		bos.write((char*)pl.szPlayName, pl.nPlayLen * 32);
		bos.write((char*)pl.szGameNum, sizeof(pl.szGameNum));

		bos.write((char*)pl.szDate, pl.nDetailLen * 32);
		for (int i = 0; i < pl.nDetailLen; i++)
		{
			bos.write((char*)pl.dwScore[i], pl.nPlayLen * sizeof(int));
		}
		bos.write((char*)pl.iReplayID, pl.nDetailLen * sizeof(int));

		return bos;
	}
	friend bistream& operator>>(bistream& bis, CMD_GP_UserFriendResultDetailRep_V2& pl)
	{
		pl.reset();

		bis >> pl.dwRoomID;
		bis >> pl.nPlayLen;
		bis >> pl.nDetailLen;

		bis.read((char*)pl.szPlayName, pl.nPlayLen * 32);
		bis.read((char*)pl.szGameNum, sizeof(pl.szGameNum));

		bis.read((char*)pl.szDate, pl.nDetailLen * 32);
		for (int i = 0; i < pl.nDetailLen; i++)
		{
			bis.read((char*)pl.dwScore[i], pl.nPlayLen * sizeof(int));
		}
		bis.read((char*)pl.iReplayID, pl.nDetailLen * sizeof(int));

		return bis;
	}
};


//房间单局回放记录查询请求
struct CMD_GP_UserFriendResultReplayReq
{
	enum { M_ID = MDM_GP_USER };
	enum { S_ID = SUB_GP_USER_FRIEND_RESULT_REPLAY_REQ };

	unsigned int					iReplayID;			//唯一分享码ID

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GP_UserFriendResultReplayReq() { reset(); }
	friend bostream& operator<<(bostream& bos, const CMD_GP_UserFriendResultReplayReq& pl)
	{
		bos << pl.iReplayID;

		return bos;
	}
	friend bistream& operator>>(bistream& bis, CMD_GP_UserFriendResultReplayReq& pl)
	{
		pl.reset();

		bis >> pl.iReplayID;

		return bis;
	}
};

//房间单局回放记录查询结果，用户信息
struct CMD_GP_UserFriendResultReplayUserRep
{
	enum { M_ID = MDM_GP_USER };
	enum { S_ID = SUB_GP_USER_FRIEND_RESULT_REPLAY_USER_REP };

	unsigned int					iReplayID;			//唯一分享码ID

	int								nPlayLen;			//玩家数:最多支持8个玩家
	unsigned int					dwUserID[8];		//用户id
	unsigned char					cGender[8];			//用户性别
	int								iScore[8];			//用户本局输赢分数
	char							szPlayName[8][32];	//玩家名称:最多支持8个玩家
	char							szFaceUrl[8][256];	//玩家头像下载地址:最多支持8个玩家

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GP_UserFriendResultReplayUserRep() { reset(); }
	friend bostream& operator<<(bostream& bos, const CMD_GP_UserFriendResultReplayUserRep& pl)
	{
		bos << pl.iReplayID;

		bos << pl.nPlayLen;
		bos.write((char*)pl.dwUserID, sizeof(pl.dwUserID));
		bos.write((char*)pl.cGender, sizeof(pl.cGender));
		bos.write((char*)pl.iScore, sizeof(pl.iScore));
		bos.write((char*)pl.szPlayName, pl.nPlayLen * 32);
		bos.write((char*)pl.szFaceUrl, pl.nPlayLen * 256);

		return bos;
	}
	friend bistream& operator>>(bistream& bis, CMD_GP_UserFriendResultReplayUserRep& pl)
	{
		pl.reset();

		bis >> pl.iReplayID;

		bis >> pl.nPlayLen;
		bis.read((char*)pl.dwUserID, sizeof(pl.dwUserID));
		bis.read((char*)pl.cGender, sizeof(pl.cGender));
		bis.read((char*)pl.iScore, sizeof(pl.iScore));
		bis.read((char*)pl.szPlayName, pl.nPlayLen * 32);
		bis.read((char*)pl.szFaceUrl, pl.nPlayLen * 256);
		
		return bis;
	}
};

//房间单局回放记录查询结果，规则及手牌
struct CMD_GP_UserFriendResultReplayRuleRep
{
	enum { M_ID = MDM_GP_USER };
	enum { S_ID = SUB_GP_USER_FRIEND_RESULT_REPLAY_RULE_REP };

	unsigned int					iReplayID;			//唯一分享码ID

	unsigned int					dwRoomID;			//房间号
	unsigned int					dwKindID;			//游戏类型

	unsigned short					nRuleLen;			//配置长度
	char							szRule[512];		//配置参数:名值对，不建议太长

	unsigned short					nHandDataLen;		//手牌数据长度
	char							szHandData[512];	//手牌数据

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GP_UserFriendResultReplayRuleRep() { reset(); }
	friend bostream& operator<<(bostream& bos, const CMD_GP_UserFriendResultReplayRuleRep& pl)
	{
		bos << pl.iReplayID;

		bos << pl.dwRoomID;
		bos << pl.dwKindID;

		bos << pl.nRuleLen;
		bos.write((char*)pl.szRule, pl.nRuleLen);

		bos << pl.nHandDataLen;
		bos.write((char*)pl.szHandData, pl.nHandDataLen);

		return bos;
	}
	friend bistream& operator>>(bistream& bis, CMD_GP_UserFriendResultReplayRuleRep& pl)
	{
		pl.reset();

		bis >> pl.iReplayID;

		bis >> pl.dwRoomID;
		bis >> pl.dwKindID;
		
		bis >> pl.nRuleLen;
		bis.read((char*)pl.szRule, pl.nRuleLen);

		bis >> pl.nHandDataLen;
		bis.read((char*)pl.szHandData, pl.nHandDataLen);

		return bis;
	}
};

//房间单局回放记录查询结果
struct CMD_GP_UserFriendResultReplayRep
{
	enum { M_ID = MDM_GP_USER };
	enum { S_ID = SUB_GP_USER_FRIEND_RESULT_REPLAY_REP };
	
	unsigned int					iCode;			//0正确，>0错误码

	unsigned int					iReplayID;			//唯一分享码ID

	unsigned short					nOrderDataLen;		//操作数据长度
	char							szOrderData[1920];	//操作数据

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GP_UserFriendResultReplayRep() { reset(); }
	friend bostream& operator<<(bostream& bos, const CMD_GP_UserFriendResultReplayRep& pl)
	{
		bos << pl.iCode;
		bos << pl.iReplayID;

		bos << pl.nOrderDataLen;
		bos.write((char*)pl.szOrderData, pl.nOrderDataLen);

		return bos;
	}
	friend bistream& operator>>(bistream& bis, CMD_GP_UserFriendResultReplayRep& pl)
	{
		pl.reset();

		bis >> pl.iCode;
		bis >> pl.iReplayID;

		bis >> pl.nOrderDataLen;
		bis.read((char*)pl.szOrderData, pl.nOrderDataLen);

		return bis;
	}
};


//查询活动列表信息
struct CMD_GP_UserActivityListReq
{
	enum {M_ID = MDM_GP_USER};
	enum {S_ID = SUB_GP_USER_ACTIVITY_LIST_REQ};

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GP_UserActivityListReq() { reset(); }
	friend bostream& operator<<(bostream& bos,const CMD_GP_UserActivityListReq& pl)
	{
		return bos;
	}
	friend bistream& operator>>(bistream& bis,CMD_GP_UserActivityListReq& pl)
	{	
		pl.reset();
		return bis;
	}
};

struct stUserActivity
{
	unsigned short	wType;				//类型：1-web 2-抽奖
	unsigned short	wStatus;				//活动状态：1开始前，2进行中，3已结束
	unsigned int	dwActivityID;		//ID
	char			szTitle[50];			//标题
	char			szText[100];			//内容简介
	char			szLinkUrl[50];		//链接地址
	char			szIconUrl[50];		//图标地址

	void reset() { memset(this, 0, sizeof(*this)); }
	stUserActivity() { reset(); }
	friend bostream& operator<<(bostream& bos,const stUserActivity& pl)
	{
		bos << pl.wType;
		bos << pl.wStatus;
		bos << pl.dwActivityID;

		bos.write((char*)pl.szTitle,sizeof(pl.szTitle));
		bos.write((char*)pl.szText,sizeof(pl.szText));
		bos.write((char*)pl.szLinkUrl,sizeof(pl.szLinkUrl));
		bos.write((char*)pl.szIconUrl,sizeof(pl.szIconUrl));

		return bos;
	}
	friend bistream& operator>>(bistream& bis,stUserActivity& pl)
	{	
		pl.reset();

		bis >> pl.wType;
		bis >> pl.wStatus;
		bis >> pl.dwActivityID;

		bis.read((char*)pl.szTitle,sizeof(pl.szTitle));
		bis.read((char*)pl.szText,sizeof(pl.szText));
		bis.read((char*)pl.szLinkUrl,sizeof(pl.szLinkUrl));
		bis.read((char*)pl.szIconUrl,sizeof(pl.szIconUrl));

		return bis;
	}
};

//查询活动列表信息结果
struct CMD_GP_UserActivityList
{
	enum {M_ID = MDM_GP_USER};
	enum {S_ID = SUB_GP_USER_ACTIVITY_LIST_REP};
	
	unsigned short	wCode;					//返回查询结果：0成功，其他错误码
	unsigned short	wCount;					//记录条数，最多5条
	stUserActivity	activity[5];			//最多5条

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GP_UserActivityList() { reset(); }
	friend bostream& operator<<(bostream& bos,const CMD_GP_UserActivityList& pl)
	{
		bos << pl.wCode;
		bos << pl.wCount;
		for (int i=0; i<pl.wCount; i++)
		{
			bos << pl.activity[i];
		}

		return bos;
	}
	friend bistream& operator>>(bistream& bis,CMD_GP_UserActivityList& pl)
	{	
		pl.reset();

		bis >> pl.wCode;
		bis >> pl.wCount;
		for (int i=0; i<pl.wCount; i++)
		{
			bis >> pl.activity[i];
		}

		return bis;
	}
};

//查询指定活动信息
struct CMD_GP_UserActivityReq
{
	enum {M_ID = MDM_GP_USER};
	enum {S_ID = SUB_GP_USER_ACTIVITY_REQ};

	unsigned int	dwActivityID;	//活动id
	unsigned int	dwUserID;		//用户id

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GP_UserActivityReq() { reset(); }
	friend bostream& operator<<(bostream& bos,const CMD_GP_UserActivityReq& pl)
	{
		bos << pl.dwActivityID;
		bos << pl.dwUserID;
		return bos;
	}
	friend bistream& operator>>(bistream& bis,CMD_GP_UserActivityReq& pl)
	{	
		pl.reset();

		bis >> pl.dwActivityID;
		bis >> pl.dwUserID;
		return bis;
	}
};

//查询指定活动信息结果
struct CMD_GP_UserActivity
{
	enum {M_ID = MDM_GP_USER};
	enum {S_ID = SUB_GP_USER_ACTIVITY_REP};
	
	unsigned int	dwActivityID;			//活动id
	unsigned int	dwUserID;				//用户id
	unsigned short	wCode;					//返回查询结果：0成功，其他错误码
	unsigned short	wCount;					//走马灯条数，最多5条
	char			szText[5][100];			//走马灯内容
	char			szResourceUrl[50];		//资源下载地址
	char			szParam[128];			//参数：n:v|n:v格式，目前支持m:10|n:5|t:200|u:30 其中m表示我的总抽奖次数，n表示已抽次数，t表示总局数，u表示多少局的一次


	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GP_UserActivity() { reset(); }
	friend bostream& operator<<(bostream& bos,const CMD_GP_UserActivity& pl)
	{
		bos << pl.dwActivityID;
		bos << pl.dwUserID;
		bos << pl.wCode;
		bos << pl.wCount;
		bos.write((char*)pl.szText,sizeof(pl.szText[0])*pl.wCount);
		bos.write((char*)pl.szResourceUrl,sizeof(pl.szResourceUrl));
		bos.write((char*)pl.szParam,sizeof(pl.szParam));

		return bos;
	}
	friend bistream& operator>>(bistream& bis,CMD_GP_UserActivity& pl)
	{	
		pl.reset();

		bis >> pl.dwActivityID;
		bis >> pl.dwUserID;
		bis >> pl.wCode;
		bis >> pl.wCount;
		bis.read((char*)pl.szText,sizeof(pl.szText[0])*pl.wCount);
		bis.read((char*)pl.szResourceUrl,sizeof(pl.szResourceUrl));
		bis.read((char*)pl.szParam,sizeof(pl.szParam));

		return bis;
	}
};

//活动抽奖
struct CMD_GP_UserActivityLuckyReq
{
	enum {M_ID = MDM_GP_USER};
	enum {S_ID = SUB_GP_USER_ACTIVITY_LUCKY_REQ};

	unsigned int	dwActivityID;	//活动id
	unsigned int	dwUserID;		//用户id

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GP_UserActivityLuckyReq() { reset(); }
	friend bostream& operator<<(bostream& bos,const CMD_GP_UserActivityLuckyReq& pl)
	{
		bos << pl.dwActivityID;
		bos << pl.dwUserID;
		return bos;
	}
	friend bistream& operator>>(bistream& bis,CMD_GP_UserActivityLuckyReq& pl)
	{	
		pl.reset();

		bis >> pl.dwActivityID;
		bis >> pl.dwUserID;
		return bis;
	}
};

//活动抽奖结果
struct CMD_GP_UserActivityLucky
{
	enum {M_ID = MDM_GP_USER};
	enum {S_ID = SUB_GP_USER_ACTIVITY_LUCKY_REP};
	
	unsigned int	dwActivityID;			//活动id
	unsigned int	dwUserID;				//用户id
	unsigned short	wCode;					//抽奖结果： 0成功，其他是错误码
	unsigned int	dwPrizeID;				//奖品id
	unsigned int	dwPrice;				//奖品价值：指欢乐豆，如果>0时，需要同步更新界面的财富信息
	char			szImageUrl[50];			//奖品图片下载地址
	char			szPrizeName[50];		//奖品名称

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GP_UserActivityLucky() { reset(); }
	friend bostream& operator<<(bostream& bos,const CMD_GP_UserActivityLucky& pl)
	{
		bos << pl.dwActivityID;
		bos << pl.dwUserID;
		bos << pl.wCode;
		bos << pl.dwPrizeID;
		bos << pl.dwPrice;

		bos.write((char*)pl.szImageUrl,sizeof(pl.szImageUrl));
		bos.write((char*)pl.szPrizeName,sizeof(pl.szPrizeName));

		return bos;
	}
	friend bistream& operator>>(bistream& bis,CMD_GP_UserActivityLucky& pl)
	{	
		pl.reset();

		bis >> pl.dwActivityID;
		bis >> pl.dwUserID;
		bis >> pl.wCode;
		bis >> pl.dwPrizeID;
		bis >> pl.dwPrice;

		bis.read((char*)pl.szImageUrl,sizeof(pl.szImageUrl));
		bis.read((char*)pl.szPrizeName,sizeof(pl.szPrizeName));

		return bis;
	}
};

//中奖记录查询请求
struct CMD_GP_UserActivityRecordReq
{
	enum {M_ID = MDM_GP_USER};
	enum {S_ID = SUB_GP_USER_ACTIVITY_RECORD_REQ};
	
	unsigned int					nPage;			//页码	
	unsigned int					dwUserID;		//玩家 ID
	unsigned int					dwActivityID;	//活动 ID	

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GP_UserActivityRecordReq() { reset(); }
	friend bostream& operator<<(bostream& bos,const CMD_GP_UserActivityRecordReq& pl)
	{
		bos << pl.nPage;
		bos << pl.dwUserID;
		bos << pl.dwActivityID;

		return bos;
	}
	friend bistream& operator>>(bistream& bis,CMD_GP_UserActivityRecordReq& pl)
	{	
		bis >> pl.nPage;
		bis >> pl.dwUserID;
		bis >> pl.dwActivityID;

		return bis;
	}
};

//中奖记录列表
struct CMD_GP_UserActivityRecord
{
	enum {M_ID = MDM_GP_USER};
	enum {S_ID = SUB_GP_USER_ACTIVITY_RECORD};
	
	char							szStatus[32];		//状态	
	char							szPrizename[32];	//奖品名称	
	char							szExchangeDate[32];	//时间	
	unsigned int					dwID;				//ID
	unsigned int					dwPrizeID;			//奖品 ID
	unsigned int					dwTotal;			//奖品总数
	unsigned int					dwUserID;			//玩家 ID
	unsigned int					dwActivityID;		//活动 ID

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GP_UserActivityRecord() { reset(); }
	friend bostream& operator<<(bostream& bos,const CMD_GP_UserActivityRecord& pl)
	{
		bos.write((char*)pl.szStatus,sizeof(pl.szStatus));
		bos.write((char*)pl.szPrizename,sizeof(pl.szPrizename));
		bos.write((char*)pl.szExchangeDate,sizeof(pl.szExchangeDate));

		bos << pl.dwID;
		bos << pl.dwPrizeID;
		bos << pl.dwTotal;
		bos << pl.dwUserID;
		bos << pl.dwActivityID;

		return bos;
	}
	friend bistream& operator>>(bistream& bis,CMD_GP_UserActivityRecord& pl)
	{	
		pl.reset();

		bis.read((char*)pl.szStatus,sizeof(pl.szStatus));
		bis.read((char*)pl.szPrizename,sizeof(pl.szPrizename));
		bis.read((char*)pl.szExchangeDate,sizeof(pl.szExchangeDate));

		bis >> pl.dwID;
		bis >> pl.dwPrizeID;
		bis >> pl.dwTotal;
		bis >> pl.dwUserID;
		bis >> pl.dwActivityID;

		return bis;
	}
};

//中奖记录列表结束
struct CMD_GP_UserActivityRecordFinish
{
	enum {M_ID = MDM_GP_USER};
	enum {S_ID = SUB_GP_USER_ACTIVITY_RECORD_FINISH};

	unsigned int					dwTotal;					//商品总数
	unsigned int					dwTotalofCurPage;			//当前页商品总数
	unsigned int					dwUserID;					//玩家 ID
	unsigned int					dwActivityID;				//活动 ID

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GP_UserActivityRecordFinish() { reset(); }
	friend bostream& operator<<(bostream& bos,const CMD_GP_UserActivityRecordFinish& pl)
	{
		bos << pl.dwTotal;
		bos << pl.dwTotalofCurPage;
		bos << pl.dwUserID;
		bos << pl.dwActivityID;

		return bos;
	}
	friend bistream& operator>>(bistream& bis,CMD_GP_UserActivityRecordFinish& pl)
	{	
		pl.reset();

		bis >> pl.dwTotal;
		bis >> pl.dwTotalofCurPage;
		bis >> pl.dwUserID;
		bis >> pl.dwActivityID;

		return bis;
	}
};

//签到
struct CMD_GP_UserSignInReq
{
	enum {M_ID = MDM_GP_USER};
	enum {S_ID = SUB_GP_USER_SIGNIN_REQ};

	unsigned int					dwUserID;							//玩家 ID

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GP_UserSignInReq() { reset(); }
	friend bostream& operator<<(bostream& bos,const CMD_GP_UserSignInReq& pl)
	{
		bos << pl.dwUserID;

		return bos;
	}
	friend bistream& operator>>(bistream& bis,CMD_GP_UserSignInReq& pl)
	{	
		bis >> pl.dwUserID;

		return bis;
	}
};

//签到结果
struct CMD_GP_UserSignIn
{
	enum {M_ID = MDM_GP_USER};
	enum {S_ID = SUB_GP_USER_SIGNIN};

	unsigned short							wRankAward;
	unsigned short							wRankNum;							
	unsigned short							wAllSend;						
	unsigned short							wTodaySend;						
	unsigned short							wTimes;			//连续签到了多少次	
	unsigned char							cbTodaySign;	//今天是否签到		
	unsigned int							dwGoldScore;							

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GP_UserSignIn() { reset(); }
	friend bostream& operator<<(bostream& bos,const CMD_GP_UserSignIn& pl)
	{
		bos << pl.wRankAward;
		bos << pl.wRankNum;
		bos << pl.wAllSend;
		bos << pl.wTodaySend;
		bos << pl.wTimes;
		bos << pl.cbTodaySign;
		bos << pl.dwGoldScore;

		return bos;
	}
	friend bistream& operator>>(bistream& bis,CMD_GP_UserSignIn& pl)
	{	
		pl.reset();

		bis >> pl.wRankAward;
		bis >> pl.wRankNum;
		bis >> pl.wAllSend;
		bis >> pl.wTodaySend;
		bis >> pl.wTimes;
		bis >> pl.cbTodaySign;
		bis >> pl.dwGoldScore;

		return bis;
	}
};

//领奖
struct CMD_GP_UserGetAwardReq
{
	enum {M_ID = MDM_GP_USER};
	enum {S_ID = SUB_GP_USER_GETAWARD_REQ};

	unsigned int					dwUserID;							//玩家 ID
	unsigned int					dwTaskID;							//任务 ID:1-朋友圈，2-好友，3~5三人拱的每日任务3个阶段，6~8四人拱的每日任务3个阶段
																		// 10~22为三人拱系统任务13个阶段  30~42为四人拱系统任务13个阶段

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GP_UserGetAwardReq() { reset(); }
	friend bostream& operator<<(bostream& bos,const CMD_GP_UserGetAwardReq& pl)
	{
		bos << pl.dwUserID;
		bos << pl.dwTaskID;

		return bos;
	}
	friend bistream& operator>>(bistream& bis,CMD_GP_UserGetAwardReq& pl)
	{	
		bis >> pl.dwUserID;
		bis >> pl.dwTaskID;

		return bis;
	}
};

//领奖结果
struct CMD_GP_UserGetAward
{
	enum {M_ID = MDM_GP_USER};
	enum {S_ID = SUB_GP_USER_GETAWARD};

	unsigned int					dwTaskID;		//任务 ID		
	unsigned int					dwScore;		//奖励数
	unsigned char					cbStatus;		//是否领奖			

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GP_UserGetAward() { reset(); }
	friend bostream& operator<<(bostream& bos,const CMD_GP_UserGetAward& pl)
	{
		bos << pl.dwTaskID;
		bos << pl.dwScore;
		bos << pl.cbStatus;

		return bos;
	}
	friend bistream& operator>>(bistream& bis,CMD_GP_UserGetAward& pl)
	{	
		pl.reset();

		bis >> pl.dwTaskID;
		bis >> pl.dwScore;
		bis >> pl.cbStatus;

		return bis;
	}
};

//兑换个人信息请求（查询金豆、手机号、更新手机号）
struct CMD_GP_UserExchangeInfoReq
{
	enum {M_ID = MDM_GP_USER};
	enum {S_ID = SUB_GP_USER_EXCHANGE_INFO_REQ};

	unsigned int					dwUserID;		//玩家 ID
	char							szPhone[64];	//电话	

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GP_UserExchangeInfoReq() { reset(); }
	friend bostream& operator<<(bostream& bos,const CMD_GP_UserExchangeInfoReq& pl)
	{
		bos << pl.dwUserID;

		bos.write((char*)pl.szPhone,sizeof(pl.szPhone));

		return bos;
	}
	friend bistream& operator>>(bistream& bis,CMD_GP_UserExchangeInfoReq& pl)
	{	
		bis >> pl.dwUserID;

		bis.read((char*)pl.szPhone,sizeof(pl.szPhone));

		return bis;
	}
};

//兑换个人消息（返回金豆、手机号）
struct CMD_GP_UserExchangeInfo
{
	enum {M_ID = MDM_GP_USER};
	enum {S_ID = SUB_GP_USER_EXCHANGE_INFO};

	char									szPhone[64];	//电话	
	unsigned int							dwGold;			//金豆		
	short									nCode;			//结果		

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GP_UserExchangeInfo() { reset(); }
	friend bostream& operator<<(bostream& bos,const CMD_GP_UserExchangeInfo& pl)
	{
		bos << pl.dwGold;
		bos << pl.nCode;

		bos.write((char*)pl.szPhone,sizeof(pl.szPhone));

		return bos;
	}
	friend bistream& operator>>(bistream& bis,CMD_GP_UserExchangeInfo& pl)
	{	
		pl.reset();

		bis >> pl.dwGold;
		bis >> pl.nCode;

		bis.read((char*)pl.szPhone,sizeof(pl.szPhone));

		return bis;
	}
};

//兑换产品查询请求
struct CMD_GP_UserExchangeProductReq
{
	enum {M_ID = MDM_GP_USER};
	enum {S_ID = SUB_GP_USER_EXCHANGE_PRODUCT_REQ};

	unsigned int					dwUserID;		//玩家 ID

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GP_UserExchangeProductReq() { reset(); }
	friend bostream& operator<<(bostream& bos,const CMD_GP_UserExchangeProductReq& pl)
	{
		bos << pl.dwUserID;

		return bos;
	}
	friend bistream& operator>>(bistream& bis,CMD_GP_UserExchangeProductReq& pl)
	{	
		bis >> pl.dwUserID;

		return bis;
	}
};

//兑换产品列表
struct CMD_GP_UserExchangeProduct
{
	enum {M_ID = MDM_GP_USER};
	enum {S_ID = SUB_GP_USER_EXCHANGE_PRODUCT};

	unsigned int					dwAwardID;			//商品 ID
	unsigned int					dwPrice;			//价格		
	unsigned int					dwLeft;				//剩余数量	
	char							szAwardname[64];	//商品名称	
	char							szAwardImage[128];	//商品图片	

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GP_UserExchangeProduct() { reset(); }
	friend bostream& operator<<(bostream& bos,const CMD_GP_UserExchangeProduct& pl)
	{
		bos << pl.dwAwardID;
		bos << pl.dwPrice;
		bos << pl.dwLeft;

		bos.write((char*)pl.szAwardname,sizeof(pl.szAwardname));
		bos.write((char*)pl.szAwardImage,sizeof(pl.szAwardImage));

		return bos;
	}
	friend bistream& operator>>(bistream& bis,CMD_GP_UserExchangeProduct& pl)
	{	
		pl.reset();

		bis >> pl.dwAwardID;
		bis >> pl.dwPrice;
		bis >> pl.dwLeft;

		bis.read((char*)pl.szAwardname,sizeof(pl.szAwardname));
		bis.read((char*)pl.szAwardImage,sizeof(pl.szAwardImage));

		return bis;
	}
};

//兑换产品查询请求
struct CMD_GP_UserExchangeFangkaQeq
{
	enum { M_ID = MDM_GP_USER };
	enum { S_ID = SUB_GP_USER_EXCHANGE_FANGKA };

	unsigned int					dwUserID;		//玩家 ID

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GP_UserExchangeFangkaQeq() { reset(); }
	friend bostream& operator<<(bostream& bos, const CMD_GP_UserExchangeFangkaQeq& pl)
	{
		bos << pl.dwUserID;

		return bos;
	}
	friend bistream& operator>>(bistream& bis, CMD_GP_UserExchangeFangkaQeq& pl)
	{
		bis >> pl.dwUserID;

		return bis;
	}
};
//兑换产品列表
struct CMD_GP_UserExchangeFangkaRst
{
	enum { M_ID = MDM_GP_USER };
	enum { S_ID = SUB_GP_USER_EXCHANGE_FANGKA_RST };

	unsigned int					dwAwardID;			//商品 ID
	unsigned int					dwPrice;			//价格		
	unsigned int					dwLeft;				//剩余数量	
	unsigned int					dwType;				//商品类型:1为可自动发放的，默认为0	
	char							szAwardname[64];	//商品名称	
	char							szAwardImage[128];	//商品图片	

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GP_UserExchangeFangkaRst() { reset(); }
	friend bostream& operator<<(bostream& bos, const CMD_GP_UserExchangeFangkaRst& pl)
	{
		bos << pl.dwAwardID;
		bos << pl.dwPrice;
		bos << pl.dwLeft;
		bos << pl.dwType;

		bos.write((char*)pl.szAwardname, sizeof(pl.szAwardname));
		bos.write((char*)pl.szAwardImage, sizeof(pl.szAwardImage));

		return bos;
	}
	friend bistream& operator>>(bistream& bis, CMD_GP_UserExchangeFangkaRst& pl)
	{
		pl.reset();

		bis >> pl.dwAwardID;
		bis >> pl.dwPrice;
		bis >> pl.dwLeft;
		bis >> pl.dwType;

		bis.read((char*)pl.szAwardname, sizeof(pl.szAwardname));
		bis.read((char*)pl.szAwardImage, sizeof(pl.szAwardImage));

		return bis;
	}
};

//兑换产品请求
struct CMD_GP_UserExchangeReq_FangKa
{
	enum { M_ID = MDM_GP_USER };
	enum { S_ID = SUB_GP_USER_EXCHANGE_REQ_Fangka };

	unsigned int					dwUserID;		//玩家 ID
	unsigned int					dwAwardID;		//商品 ID
	unsigned int					dwPokerUserId;

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GP_UserExchangeReq_FangKa() { reset(); }
	friend bostream& operator<<(bostream& bos, const CMD_GP_UserExchangeReq_FangKa& pl)
	{
		bos << pl.dwUserID;
		bos << pl.dwAwardID;
		bos << pl.dwPokerUserId;

		return bos;
	}
	friend bistream& operator>>(bistream& bis, CMD_GP_UserExchangeReq_FangKa& pl)
	{
		bis >> pl.dwUserID;
		bis >> pl.dwAwardID;
		bis >> pl.dwPokerUserId;

		return bis;
	}
};

//兑换产品
struct CMD_GP_UserExchange_Fangka
{
	enum { M_ID = MDM_GP_USER };
	enum { S_ID = SUB_GP_USER_EXCHANGE_RESULT };

	short							nCode;				//结果				
	unsigned int					dwAwardID;			//商品 ID
	unsigned int					dwFangka;				//当前房卡
	unsigned int					dwScore;			//当前欢乐豆数量
	char							szDescribeMsg[128];		//描述信息

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GP_UserExchange_Fangka() { reset(); }
	friend bostream& operator<<(bostream& bos, const CMD_GP_UserExchange_Fangka& pl)
	{
		bos << pl.nCode;
		bos << pl.dwAwardID;
		bos << pl.dwFangka;
		bos << pl.dwScore;


		bos.write((char*)pl.szDescribeMsg,sizeof(pl.szDescribeMsg));
		return bos;
	}
	friend bistream& operator>>(bistream& bis, CMD_GP_UserExchange_Fangka& pl)
	{
		pl.reset();

		bis >> pl.nCode;
		bis >> pl.dwAwardID;
		bis >> pl.dwFangka;
		bis >> pl.dwScore;

		bis.read((char*)pl.szDescribeMsg,sizeof(pl.szDescribeMsg));
		return bis;
	}
};

//兑换产品列表结束
struct CMD_GP_UserExchangeFangkaFinish
{
	enum {M_ID = MDM_GP_USER};
	enum { S_ID = SUB_GP_USER_EXCHANGE_FANGKA_FINISH };

	unsigned int					dwTotal;		//总数

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GP_UserExchangeFangkaFinish() { reset(); }
	friend bostream& operator<<(bostream& bos, const CMD_GP_UserExchangeFangkaFinish& pl)
	{
		bos << pl.dwTotal;

		return bos;
	}
	friend bistream& operator>>(bistream& bis, CMD_GP_UserExchangeFangkaFinish& pl)
	{	
		pl.reset();

		bis >> pl.dwTotal;

		return bis;
	}
};
//兑换产品查询请求
struct CMD_GP_UserExchangeProductReq_V2
{
	enum {M_ID = MDM_GP_USER};
	enum {S_ID = SUB_GP_USER_EXCHANGE_PRODUCT_REQ_V2};

	unsigned int					dwUserID;		//玩家 ID

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GP_UserExchangeProductReq_V2() { reset(); }
	friend bostream& operator<<(bostream& bos,const CMD_GP_UserExchangeProductReq_V2& pl)
	{
		bos << pl.dwUserID;

		return bos;
	}
	friend bistream& operator>>(bistream& bis,CMD_GP_UserExchangeProductReq_V2& pl)
	{	
		bis >> pl.dwUserID;

		return bis;
	}
};

//兑换产品列表
struct CMD_GP_UserExchangeProduct_V2
{
	enum {M_ID = MDM_GP_USER};
	enum {S_ID = SUB_GP_USER_EXCHANGE_PRODUCT_V2};

	unsigned int					dwAwardID;			//商品 ID
	unsigned int					dwPrice;			//价格		
	unsigned int					dwLeft;				//剩余数量	
	unsigned int					dwType;				//商品类型:1为可自动发放的，默认为0	
	char							szAwardname[64];	//商品名称	
	char							szAwardImage[128];	//商品图片	

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GP_UserExchangeProduct_V2() { reset(); }
	friend bostream& operator<<(bostream& bos,const CMD_GP_UserExchangeProduct_V2& pl)
	{
		bos << pl.dwAwardID;
		bos << pl.dwPrice;
		bos << pl.dwLeft;
		bos << pl.dwType;

		bos.write((char*)pl.szAwardname,sizeof(pl.szAwardname));
		bos.write((char*)pl.szAwardImage,sizeof(pl.szAwardImage));

		return bos;
	}
	friend bistream& operator>>(bistream& bis,CMD_GP_UserExchangeProduct_V2& pl)
	{	
		pl.reset();

		bis >> pl.dwAwardID;
		bis >> pl.dwPrice;
		bis >> pl.dwLeft;
		bis >> pl.dwType;

		bis.read((char*)pl.szAwardname,sizeof(pl.szAwardname));
		bis.read((char*)pl.szAwardImage,sizeof(pl.szAwardImage));

		return bis;
	}
};

//兑换产品列表结束
struct CMD_GP_UserExchangeProductFinish
{
	enum {M_ID = MDM_GP_USER};
	enum {S_ID = SUB_GP_USER_EXCHANGE_PRODUCT_FINISH};

	unsigned int					dwTotal;		//总数

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GP_UserExchangeProductFinish() { reset(); }
	friend bostream& operator<<(bostream& bos,const CMD_GP_UserExchangeProductFinish& pl)
	{
		bos << pl.dwTotal;

		return bos;
	}
	friend bistream& operator>>(bistream& bis,CMD_GP_UserExchangeProductFinish& pl)
	{	
		pl.reset();

		bis >> pl.dwTotal;

		return bis;
	}
};


//兑换产品请求
struct CMD_GP_UserExchangeReq
{
	enum {M_ID = MDM_GP_USER};
	enum {S_ID = SUB_GP_USER_EXCHANGE_REQ};

	unsigned int					dwUserID;		//玩家 ID
	unsigned int					dwAwardID;		//商品 ID

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GP_UserExchangeReq() { reset(); }
	friend bostream& operator<<(bostream& bos,const CMD_GP_UserExchangeReq& pl)
	{
		bos << pl.dwUserID;
		bos << pl.dwAwardID;

		return bos;
	}
	friend bistream& operator>>(bistream& bis,CMD_GP_UserExchangeReq& pl)
	{	
		bis >> pl.dwUserID;
		bis >> pl.dwAwardID;

		return bis;
	}
};

//兑换产品
struct CMD_GP_UserExchange
{
	enum {M_ID = MDM_GP_USER};
	enum {S_ID = SUB_GP_USER_EXCHANGE};

	short							nCode;				//结果				
	unsigned int					dwAwardID;			//商品 ID
	unsigned int					dwGold;				//当前金豆数

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GP_UserExchange() { reset(); }
	friend bostream& operator<<(bostream& bos,const CMD_GP_UserExchange& pl)
	{
		bos << pl.nCode;
		bos << pl.dwAwardID;
		bos << pl.dwGold;

		return bos;
	}
	friend bistream& operator>>(bistream& bis,CMD_GP_UserExchange& pl)
	{	
		pl.reset();

		bis >> pl.nCode;
		bis >> pl.dwAwardID;
		bis >> pl.dwGold;

		return bis;
	}
};


//兑换产品请求
struct CMD_GP_UserExchangeReq_V2
{
	enum {M_ID = MDM_GP_USER};
	enum {S_ID = SUB_GP_USER_EXCHANGE_REQ_V2};

	unsigned int					dwUserID;		//玩家 ID
	unsigned int					dwAwardID;		//商品 ID

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GP_UserExchangeReq_V2() { reset(); }
	friend bostream& operator<<(bostream& bos,const CMD_GP_UserExchangeReq_V2& pl)
	{
		bos << pl.dwUserID;
		bos << pl.dwAwardID;

		return bos;
	}
	friend bistream& operator>>(bistream& bis,CMD_GP_UserExchangeReq_V2& pl)
	{	
		bis >> pl.dwUserID;
		bis >> pl.dwAwardID;

		return bis;
	}
};

//兑换产品
struct CMD_GP_UserExchange_V2
{
	enum {M_ID = MDM_GP_USER};
	enum {S_ID = SUB_GP_USER_EXCHANGE_V2};

	short							nCode;				//结果				
	unsigned int					dwAwardID;			//商品 ID
	unsigned int					dwGold;				//当前金豆数
	unsigned int					dwScore;			//当前欢乐豆数量

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GP_UserExchange_V2() { reset(); }
	friend bostream& operator<<(bostream& bos,const CMD_GP_UserExchange_V2& pl)
	{
		bos << pl.nCode;
		bos << pl.dwAwardID;
		bos << pl.dwGold;
		bos << pl.dwScore;

		return bos;
	}
	friend bistream& operator>>(bistream& bis,CMD_GP_UserExchange_V2& pl)
	{	
		pl.reset();

		bis >> pl.nCode;
		bis >> pl.dwAwardID;
		bis >> pl.dwGold;
		bis >> pl.dwScore;

		return bis;
	}
};

//兑换记录查询请求
struct CMD_GP_UserExchangeRecordReq
{
	enum {M_ID = MDM_GP_USER};
	enum {S_ID = SUB_GP_USER_EXCHANGE_RECORD_REQ};

	
	unsigned int					nPage;			//页码	
	unsigned int					dwUserID;		//玩家 ID	

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GP_UserExchangeRecordReq() { reset(); }
	friend bostream& operator<<(bostream& bos,const CMD_GP_UserExchangeRecordReq& pl)
	{
		bos << pl.nPage;
		bos << pl.dwUserID;

		return bos;
	}
	friend bistream& operator>>(bistream& bis,CMD_GP_UserExchangeRecordReq& pl)
	{	
		bis >> pl.nPage;
		bis >> pl.dwUserID;

		return bis;
	}
};

//兑换记录列表
struct CMD_GP_UserExchangeRecord
{
	enum {M_ID = MDM_GP_USER};
	enum {S_ID = SUB_GP_USER_EXCHANGE_RECORD};
	
	char							szStatus[32];		//状态	
	char							szAwardname[32];	//商品名称	
	char							szExchangeDate[32];	//时间	
	unsigned int					dwID;				//ID
	unsigned int					dwAwardID;			//商品 ID
	unsigned int					dwTotal;			//商品总数
//	unsigned int					dwExchangeDate;		//时间	

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GP_UserExchangeRecord() { reset(); }
	friend bostream& operator<<(bostream& bos,const CMD_GP_UserExchangeRecord& pl)
	{
		bos.write((char*)pl.szStatus,sizeof(pl.szStatus));
		bos.write((char*)pl.szAwardname,sizeof(pl.szAwardname));
		bos.write((char*)pl.szExchangeDate,sizeof(pl.szExchangeDate));

		bos << pl.dwID;
		bos << pl.dwAwardID;
		bos << pl.dwTotal;

		return bos;
	}
	friend bistream& operator>>(bistream& bis,CMD_GP_UserExchangeRecord& pl)
	{	
		pl.reset();

		bis.read((char*)pl.szStatus,sizeof(pl.szStatus));
		bis.read((char*)pl.szAwardname,sizeof(pl.szAwardname));
		bis.read((char*)pl.szExchangeDate,sizeof(pl.szExchangeDate));

		bis >> pl.dwID;
		bis >> pl.dwAwardID;
		bis >> pl.dwTotal;

		return bis;
	}
};

//兑换记录列表结束
struct CMD_GP_UserExchangeRecordFinish
{
	enum {M_ID = MDM_GP_USER};
	enum {S_ID = SUB_GP_USER_EXCHANGE_RECORD_FINISH};

	unsigned int					dwTotal;					//商品总数
	unsigned int					dwTotalofCurPage;			//当前页商品总数

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GP_UserExchangeRecordFinish() { reset(); }
	friend bostream& operator<<(bostream& bos,const CMD_GP_UserExchangeRecordFinish& pl)
	{
		bos << pl.dwTotal;
		bos << pl.dwTotalofCurPage;

		return bos;
	}
	friend bistream& operator>>(bistream& bis,CMD_GP_UserExchangeRecordFinish& pl)
	{	
		pl.reset();

		bis >> pl.dwTotal;
		bis >> pl.dwTotalofCurPage;

		return bis;
	}
};

//名称校验
struct CMD_GP_CheckIDReq
{
	enum {M_ID = MDM_GP_USER};
	enum {S_ID = SUB_GP_CHECK_ID_REQ};
	
	char	szAccount[NAME_LEN];				//玩家ID

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GP_CheckIDReq() { reset(); }
	friend bostream& operator<<(bostream& bos,const CMD_GP_CheckIDReq& pl)
	{
		bos.write((char*)pl.szAccount,sizeof(pl.szAccount));

		return bos;
	}
	friend bistream& operator>>(bistream& bis,CMD_GP_CheckIDReq& pl)
	{	
		pl.reset();

		bis.read((char*)pl.szAccount,sizeof(pl.szAccount));

		return bis;
	}
};

struct CMD_GP_CheckIDRep
{
	enum {M_ID = MDM_GP_USER};
	enum {S_ID = SUB_GP_CHECK_ID_REP};
	
	short	nCode;					//结果
	char	szAccount[NAME_LEN];	//玩家ID
	char	szDescribeMsg[64];		//描述信息

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GP_CheckIDRep() { reset(); }
	friend bostream& operator<<(bostream& bos,const CMD_GP_CheckIDRep& pl)
	{
		bos << pl.nCode;

		bos.write((char*)pl.szAccount,sizeof(pl.szAccount));
		bos.write((char*)pl.szDescribeMsg,sizeof(pl.szDescribeMsg));

		return bos;
	}
	friend bistream& operator>>(bistream& bis,CMD_GP_CheckIDRep& pl)
	{	
		pl.reset();

		bis >> pl.nCode;

		bis.read((char*)pl.szAccount,sizeof(pl.szAccount));
		bis.read((char*)pl.szDescribeMsg,sizeof(pl.szDescribeMsg));

		return bis;
	}
};

//昵称校验
struct CMD_GP_CheckNickNameReq
{
	enum {M_ID = MDM_GP_USER};
	enum {S_ID = SUB_GP_CHECK_NICKNAME_REQ};
	
	char	szNickName[NAME_LEN];				//玩家昵称

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GP_CheckNickNameReq() { reset(); }
	friend bostream& operator<<(bostream& bos,const CMD_GP_CheckNickNameReq& pl)
	{
		bos.write((char*)pl.szNickName,sizeof(pl.szNickName));

		return bos;
	}
	friend bistream& operator>>(bistream& bis,CMD_GP_CheckNickNameReq& pl)
	{	
		pl.reset();

		bis.read((char*)pl.szNickName,sizeof(pl.szNickName));

		return bis;
	}
};

struct CMD_GP_CheckNickNameRep
{
	enum {M_ID = MDM_GP_USER};
	enum {S_ID = SUB_GP_CHECK_NICKNAME_REP};
	
	short	nCode;					//结果
	char	szNickName[NAME_LEN];	//玩家昵称
	char	szDescribeMsg[64];		//描述信息

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GP_CheckNickNameRep() { reset(); }
	friend bostream& operator<<(bostream& bos,const CMD_GP_CheckNickNameRep& pl)
	{
		bos << pl.nCode;

		bos.write((char*)pl.szNickName,sizeof(pl.szNickName));
		bos.write((char*)pl.szDescribeMsg,sizeof(pl.szDescribeMsg));

		return bos;
	}
	friend bistream& operator>>(bistream& bis,CMD_GP_CheckNickNameRep& pl)
	{	
		pl.reset();

		bis >> pl.nCode;

		bis.read((char*)pl.szNickName,sizeof(pl.szNickName));
		bis.read((char*)pl.szDescribeMsg,sizeof(pl.szDescribeMsg));

		return bis;
	}
};

//注册
struct CMD_GP_RegisterReq
{
	enum {M_ID = MDM_GP_USER};
	enum {S_ID = SUB_GP_REGISTER_REQ};
	
	short	nGender;						//玩家性别
	char	szAccount[NAME_LEN];			//玩家ID
	char	szNickName[NAME_LEN];			//玩家昵称
	char	szPassWord[PASS_LEN];			//登录密码
	char	szComputerID[COMPUTER_ID_LEN];	//硬件标识
	char	szPhone[NAME_LEN];			//玩家手机号

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GP_RegisterReq() { reset(); }
	friend bostream& operator<<(bostream& bos,const CMD_GP_RegisterReq& pl)
	{
		bos << pl.nGender;

		bos.write((char*)pl.szAccount,sizeof(pl.szAccount));
		bos.write((char*)pl.szNickName,sizeof(pl.szNickName));
		bos.write((char*)pl.szPassWord,sizeof(pl.szPassWord));
		bos.write((char*)pl.szComputerID,sizeof(pl.szComputerID));
		bos.write((char*)pl.szPhone,sizeof(pl.szPhone));

		return bos;
	}
	friend bistream& operator>>(bistream& bis,CMD_GP_RegisterReq& pl)
	{	
		pl.reset();

		bis >> pl.nGender;

		bis.read((char*)pl.szAccount,sizeof(pl.szAccount));
		bis.read((char*)pl.szNickName,sizeof(pl.szNickName));
		bis.read((char*)pl.szPassWord,sizeof(pl.szPassWord));
		bis.read((char*)pl.szComputerID,sizeof(pl.szComputerID));
		bis.read((char*)pl.szPhone,sizeof(pl.szPhone));

		return bis;
	}
};

//注册,带渠道标识
struct CMD_GP_RegisterWithChannelReq
{
	enum {M_ID = MDM_GP_USER};
	enum {S_ID = SUB_GP_REGISTER_CHANNEL_REQ};
	
	short	nGender;						//玩家性别
	char	szAccount[NAME_LEN];			//玩家ID
	char	szNickName[NAME_LEN];			//玩家昵称
	char	szPassWord[PASS_LEN];			//登录密码
	char	szComputerID[COMPUTER_ID_LEN];	//硬件标识
	char	szPhone[NAME_LEN];			//玩家手机号
	char	szChannel[NAME_LEN];			//客户端渠道号

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GP_RegisterWithChannelReq() { reset(); }
	friend bostream& operator<<(bostream& bos,const CMD_GP_RegisterWithChannelReq& pl)
	{
		bos << pl.nGender;

		bos.write((char*)pl.szAccount,sizeof(pl.szAccount));
		bos.write((char*)pl.szNickName,sizeof(pl.szNickName));
		bos.write((char*)pl.szPassWord,sizeof(pl.szPassWord));
		bos.write((char*)pl.szComputerID,sizeof(pl.szComputerID));
		bos.write((char*)pl.szPhone,sizeof(pl.szPhone));
		bos.write((char*)pl.szChannel,sizeof(pl.szChannel));

		return bos;
	}
	friend bistream& operator>>(bistream& bis,CMD_GP_RegisterWithChannelReq& pl)
	{	
		pl.reset();

		bis >> pl.nGender;

		bis.read((char*)pl.szAccount,sizeof(pl.szAccount));
		bis.read((char*)pl.szNickName,sizeof(pl.szNickName));
		bis.read((char*)pl.szPassWord,sizeof(pl.szPassWord));
		bis.read((char*)pl.szComputerID,sizeof(pl.szComputerID));
		bis.read((char*)pl.szPhone,sizeof(pl.szPhone));
		bis.read((char*)pl.szChannel,sizeof(pl.szChannel));

		return bis;
	}
};

struct CMD_GP_RegisterRep
{
	enum {M_ID = MDM_GP_USER};
	enum {S_ID = SUB_GP_REGISTER_REP};
	
	short	nCode;					//结果
	char	szAccount[NAME_LEN];	//玩家名称
	char	szDescribeMsg[64];		//描述信息

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GP_RegisterRep() { reset(); }
	friend bostream& operator<<(bostream& bos,const CMD_GP_RegisterRep& pl)
	{
		bos << pl.nCode;

		bos.write((char*)pl.szAccount,sizeof(pl.szAccount));
		bos.write((char*)pl.szDescribeMsg,sizeof(pl.szDescribeMsg));

		return bos;
	}
	friend bistream& operator>>(bistream& bis,CMD_GP_RegisterRep& pl)
	{	
		pl.reset();

		bis >> pl.nCode;

		bis.read((char*)pl.szAccount,sizeof(pl.szAccount));
		bis.read((char*)pl.szDescribeMsg,sizeof(pl.szDescribeMsg));

		return bis;
	}
};

//升级
struct CMD_GP_IDUpdateReq
{
	enum {M_ID = MDM_GP_USER};
	enum {S_ID = SUB_GP_ID_UPDATE_REQ};
	
	short	nGender;						//玩家性别
	char	szAccountOld[NAME_LEN];			//旧的玩家名称
	char	szAccount[NAME_LEN];			//玩家名称
	char	szNickName[NAME_LEN];			//玩家昵称
	char	szPassWord[PASS_LEN];			//登录密码
	char	szComputerID[COMPUTER_ID_LEN];	//硬件标识

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GP_IDUpdateReq() { reset(); }
	friend bostream& operator<<(bostream& bos,const CMD_GP_IDUpdateReq& pl)
	{
		bos << pl.nGender;

		bos.write((char*)pl.szAccountOld,sizeof(pl.szAccountOld));
		bos.write((char*)pl.szAccount,sizeof(pl.szAccount));
		bos.write((char*)pl.szNickName,sizeof(pl.szNickName));
		bos.write((char*)pl.szPassWord,sizeof(pl.szPassWord));
		bos.write((char*)pl.szComputerID,sizeof(pl.szComputerID));

		return bos;
	}
	friend bistream& operator>>(bistream& bis,CMD_GP_IDUpdateReq& pl)
	{	
		pl.reset();

		bis >> pl.nGender;

		bis.read((char*)pl.szAccountOld,sizeof(pl.szAccountOld));
		bis.read((char*)pl.szAccount,sizeof(pl.szAccount));
		bis.read((char*)pl.szNickName,sizeof(pl.szNickName));
		bis.read((char*)pl.szPassWord,sizeof(pl.szPassWord));
		bis.read((char*)pl.szComputerID,sizeof(pl.szComputerID));

		return bis;
	}
};

struct CMD_GP_IDUpdateRep
{
	enum {M_ID = MDM_GP_USER};
	enum {S_ID = SUB_GP_ID_UPDATE_REP};
	
	short	nCode;					//结果
	char	szAccount[NAME_LEN];	//玩家名称
	char	szDescribeMsg[128];		//描述信息

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GP_IDUpdateRep() { reset(); }
	friend bostream& operator<<(bostream& bos,const CMD_GP_IDUpdateRep& pl)
	{
		bos << pl.nCode;

		bos.write((char*)pl.szAccount,sizeof(pl.szAccount));
		bos.write((char*)pl.szDescribeMsg,sizeof(pl.szDescribeMsg));

		return bos;
	}
	friend bistream& operator>>(bistream& bis,CMD_GP_IDUpdateRep& pl)
	{	
		pl.reset();

		bis >> pl.nCode;

		bis.read((char*)pl.szAccount,sizeof(pl.szAccount));
		bis.read((char*)pl.szDescribeMsg,sizeof(pl.szDescribeMsg));

		return bis;
	}
};

//查询用户信息
struct CMD_GP_UserInfoReq
{
	enum {M_ID = MDM_GP_USER};
	enum {S_ID = SUB_GP_USER_INFO_REQ};

	unsigned int	dwUserID;		//玩家ID
	char	szAccount[NAME_LEN];			//玩家名称

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GP_UserInfoReq() { reset(); }
	friend bostream& operator<<(bostream& bos,const CMD_GP_UserInfoReq& pl)
	{
		bos << pl.dwUserID;

		bos.write((char*)pl.szAccount,sizeof(pl.szAccount));

		return bos;
	}
	friend bistream& operator>>(bistream& bis,CMD_GP_UserInfoReq& pl)
	{	
		pl.reset();

		bis >> pl.dwUserID;

		bis.read((char*)pl.szAccount,sizeof(pl.szAccount));

		return bis;
	}
};

struct CMD_GP_UserInfoRep
{
	enum {M_ID = MDM_GP_USER};
	enum {S_ID = SUB_GP_USER_INFO_REP};
	
	short	nCode;					//结果
	short	nGender;				//玩家性别
	unsigned int	dwUserID;		//玩家ID
	char	szAccount[NAME_LEN];	//玩家名称
	char	szNickname[NAME_LEN];	//玩家昵称
	char	szPhone[NAME_LEN];		//绑定的手机号
	char	szExt[128];				//附加信息

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GP_UserInfoRep() { reset(); }
	friend bostream& operator<<(bostream& bos,const CMD_GP_UserInfoRep& pl)
	{
		bos << pl.nCode;
		bos << pl.nGender;
		bos << pl.dwUserID;

		bos.write((char*)pl.szAccount,sizeof(pl.szAccount));
		bos.write((char*)pl.szNickname,sizeof(pl.szNickname));
		bos.write((char*)pl.szPhone,sizeof(pl.szPhone));
		bos.write((char*)pl.szExt,sizeof(pl.szExt));

		return bos;
	}
	friend bistream& operator>>(bistream& bis,CMD_GP_UserInfoRep& pl)
	{	
		pl.reset();

		bis >> pl.nCode;
		bis >> pl.nGender;
		bis >> pl.dwUserID;

		bis.read((char*)pl.szAccount,sizeof(pl.szAccount));
		bis.read((char*)pl.szNickname,sizeof(pl.szNickname));
		bis.read((char*)pl.szPhone,sizeof(pl.szPhone));
		bis.read((char*)pl.szExt,sizeof(pl.szExt));

		return bis;
	}
};

//查询用户身份证账号信息
struct CMD_GP_UserInfoIDCARDReq
{
	enum { M_ID = MDM_GP_USER };
	enum { S_ID = SUB_GP_USER_INFO_ID_CARD_REQ };

	unsigned int	dwUserID;		//玩家ID

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GP_UserInfoIDCARDReq() { reset(); }
	friend bostream& operator<<(bostream& bos, const CMD_GP_UserInfoIDCARDReq& pl)
	{
		bos << pl.dwUserID;

		return bos;
	}
	friend bistream& operator>>(bistream& bis, CMD_GP_UserInfoIDCARDReq& pl)
	{
		pl.reset();

		bis >> pl.dwUserID;

		return bis;
	}
};

// 查询身份证号结果
struct CMD_GP_UserInfoIDCARDRep
{
	enum { M_ID = MDM_GP_USER };
	enum { S_ID = SUB_GP_USER_INFO_ID_CARD_REP };

	unsigned int	dwUserID;					//玩家ID
	char	szIdNum[128];				//身份证账号信息

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GP_UserInfoIDCARDRep() { reset(); }
	friend bostream& operator<<(bostream& bos, const CMD_GP_UserInfoIDCARDRep& pl)
	{
		bos << pl.dwUserID;
		bos.write((char*)pl.szIdNum, sizeof(pl.szIdNum));

		return bos;
	}
	friend bistream& operator>>(bistream& bis, CMD_GP_UserInfoIDCARDRep& pl)
	{
		pl.reset();

		bis >> pl.dwUserID;

		bis.read((char*)pl.szIdNum, sizeof(pl.szIdNum));

		return bis;
	}
};

//请求提交身份证信息(姓名及身份证号)
struct CMD_GP_UserAuthenticationReq
{
	enum { M_ID = MDM_GP_USER };
	enum { S_ID = SUB_GP_USER_AUTHENTICA_REQ };

	unsigned int	dwUserID;				//玩家数字账号
	char	szName[NAME_LEN];				// 玩家姓名
	char	szIdCardNum[32];				// 玩家身份证账号

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GP_UserAuthenticationReq() { reset(); }
	friend bostream& operator<<(bostream& bos, const CMD_GP_UserAuthenticationReq& pl)
	{
		bos << pl.dwUserID;

		bos.write((char*)pl.szName, sizeof(pl.szName));
		bos.write((char*)pl.szIdCardNum, sizeof(pl.szIdCardNum));

		return bos;
	}
	friend bistream& operator>>(bistream& bis, CMD_GP_UserAuthenticationReq& pl)
	{
		pl.reset();

		bis >> pl.dwUserID;

		bis.read((char*)pl.szName, sizeof(pl.szName));
		bis.read((char*)pl.szIdCardNum, sizeof(pl.szIdCardNum));

		return bis;
	}
};

// 查询身份证号结果
struct CMD_GP_UserAuthenticationRep
{
	enum { M_ID = MDM_GP_USER };
	enum { S_ID = SUB_GP_USER_AUTHENTICA_REP };

	unsigned int	dwUserID;				//玩家数字账号
	short resultCode;						// 结果信息				

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GP_UserAuthenticationRep() { reset(); }
	friend bostream& operator<<(bostream& bos, const CMD_GP_UserAuthenticationRep& pl)
	{
		bos << pl.dwUserID;
		bos << pl.resultCode;

		return bos;
	}
	friend bistream& operator>>(bistream& bis, CMD_GP_UserAuthenticationRep& pl)
	{
		pl.reset();

		bis >> pl.dwUserID;
		bis >> pl.resultCode;

		return bis;
	}
};

struct CMD_GP_ResetPwdReq
{
	enum {M_ID = MDM_GP_USER};
	enum {S_ID = SUB_GP_RESET_PWD_REQ};

	unsigned int	dwUserID;		//玩家ID
	char			szAccount[NAME_LEN];				//玩家ID
	char			szPassword[PASS_LEN];				//玩家密码
	char			szCode[NAME_LEN];					//校验码

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GP_ResetPwdReq() { reset(); }
	friend bostream& operator<<(bostream& bos,const CMD_GP_ResetPwdReq& pl)
	{
		bos << pl.dwUserID;

		bos.write((char*)pl.szAccount,sizeof(pl.szAccount));
		bos.write((char*)pl.szPassword,sizeof(pl.szPassword));
		bos.write((char*)pl.szCode,sizeof(pl.szCode));

		return bos;
	}
	friend bistream& operator>>(bistream& bis,CMD_GP_ResetPwdReq& pl)
	{	
		pl.reset();
		bis >> pl.dwUserID;

		bis.read((char*)pl.szAccount,sizeof(pl.szAccount));
		bis.read((char*)pl.szPassword,sizeof(pl.szPassword));
		bis.read((char*)pl.szCode,sizeof(pl.szCode));

		return bis;
	}
};

//修改结果
struct CMD_GP_ResetPwdRep
{
	enum {M_ID = MDM_GP_USER};
	enum {S_ID = SUB_GP_RESET_PWD_REP};
	
	short	nCode;					//结果
	char	szDescribeMsg[128];		//描述信息
	char	szAccount[NAME_LEN];	//玩家ID
	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GP_ResetPwdRep() { reset(); }
	friend bostream& operator<<(bostream& bos,const CMD_GP_ResetPwdRep& pl)
	{
		bos << pl.nCode;

		bos.write((char*)pl.szDescribeMsg,sizeof(pl.szDescribeMsg));
		bos.write((char*)pl.szAccount,sizeof(pl.szAccount));

		return bos;
	}
	friend bistream& operator>>(bistream& bis,CMD_GP_ResetPwdRep& pl)
	{	
		pl.reset();
		bis >> pl.nCode;

		bis.read((char*)pl.szDescribeMsg,sizeof(pl.szDescribeMsg));
		bis.read((char*)pl.szAccount,sizeof(pl.szAccount));

		return bis;
	}
};

//个人资料
struct CMD_GP_ModifyIndividual
{
	enum {M_ID = MDM_GP_USER};
	enum {S_ID = SUB_GP_MODIFY_INDIVIDUAL};

	short							nGender;							//玩家性别
	char							szNickname[NAME_LEN];				//玩家昵称
	char							szPhone[NAME_LEN];					//电话
	char							szAccount[NAME_LEN];				//校验码

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GP_ModifyIndividual() { reset(); }
	friend bostream& operator<<(bostream& bos,const CMD_GP_ModifyIndividual& pl)
	{
		bos << pl.nGender;

		bos.write((char*)pl.szNickname,sizeof(pl.szNickname));
		bos.write((char*)pl.szPhone,sizeof(pl.szPhone));
		bos.write((char*)pl.szAccount,sizeof(pl.szAccount));

		return bos;
	}
	friend bistream& operator>>(bistream& bis,CMD_GP_ModifyIndividual& pl)
	{	
		pl.reset();
		bis >> pl.nGender;

		bis.read((char*)pl.szNickname,sizeof(pl.szNickname));
		bis.read((char*)pl.szPhone,sizeof(pl.szPhone));
		bis.read((char*)pl.szAccount,sizeof(pl.szAccount));

		return bis;
	}
};


//修改结果
struct CMD_GP_ModifyIndividualResult
{
	enum {M_ID = MDM_GP_USER};
	enum {S_ID = SUB_GP_MODIFY_INDIVIDUAL_RESULT};
	
	short	nCode;					//结果
	char	szAccount[NAME_LEN];	//玩家ID
	char	szDescribeMsg[128];		//描述信息
	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GP_ModifyIndividualResult() { reset(); }
	friend bostream& operator<<(bostream& bos,const CMD_GP_ModifyIndividualResult& pl)
	{
		bos << pl.nCode;

		bos.write((char*)pl.szAccount,sizeof(pl.szAccount));
		bos.write((char*)pl.szDescribeMsg,sizeof(pl.szDescribeMsg));

		return bos;
	}
	friend bistream& operator>>(bistream& bis,CMD_GP_ModifyIndividualResult& pl)
	{	
		pl.reset();
		
		bis >> pl.nCode;
		bis.read((char*)pl.szAccount,sizeof(pl.szAccount));
		bis.read((char*)pl.szDescribeMsg,sizeof(pl.szDescribeMsg));

		return bis;
	}
};

//微信账号绑定手机号
struct CMD_GP_ModifyPhone
{
	enum {M_ID = MDM_GP_USER};
	enum {S_ID = SUB_GP_MODIFY_PHONE};

	unsigned int	dwUserID;		//玩家ID
	char			szPhone[31];	//手机号

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GP_ModifyPhone() { reset(); }
	friend bostream& operator<<(bostream& bos,const CMD_GP_ModifyPhone& pl)
	{
		bos << pl.dwUserID;

		bos.write((char*)pl.szPhone,sizeof(pl.szPhone));

		return bos;
	}
	friend bistream& operator>>(bistream& bis,CMD_GP_ModifyPhone& pl)
	{	
		pl.reset();
		bis >> pl.dwUserID;

		bis.read((char*)pl.szPhone,sizeof(pl.szPhone));

		return bis;
	}
};

//修改结果
struct CMD_GP_ModifyPhoneResult
{
	enum {M_ID = MDM_GP_USER};
	enum {S_ID = SUB_GP_MODIFY_PHONE_RESULT};
	
	short	nCode;					//结果
	char	szAccount[NAME_LEN];	//玩家ID
	char	szPhone[31];			//手机号
	char	szDescribeMsg[128];		//描述信息
	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GP_ModifyPhoneResult() { reset(); }
	friend bostream& operator<<(bostream& bos,const CMD_GP_ModifyPhoneResult& pl)
	{
		bos << pl.nCode;

		bos.write((char*)pl.szAccount,sizeof(pl.szAccount));
		bos.write((char*)pl.szPhone,sizeof(pl.szPhone));
		bos.write((char*)pl.szDescribeMsg,sizeof(pl.szDescribeMsg));

		return bos;
	}
	friend bistream& operator>>(bistream& bis,CMD_GP_ModifyPhoneResult& pl)
	{	
		pl.reset();
		
		bis >> pl.nCode;
		bis.read((char*)pl.szAccount,sizeof(pl.szAccount));
		bis.read((char*)pl.szPhone,sizeof(pl.szPhone));
		bis.read((char*)pl.szDescribeMsg,sizeof(pl.szDescribeMsg));

		return bis;
	}
};

//个性签名
struct CMD_GP_ModifyUnderWrite
{
	enum {M_ID = MDM_GP_USER};
	enum {S_ID = SUB_GP_MODIFY_UNDERWRITE};

	unsigned int	dwUserID;		//玩家ID
	char			szDescribe[UNDER_WRITE_LEN];				//个性签名

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GP_ModifyUnderWrite() { reset(); }
	friend bostream& operator<<(bostream& bos,const CMD_GP_ModifyUnderWrite& pl)
	{
		bos << pl.dwUserID;

		bos.write((char*)pl.szDescribe,sizeof(pl.szDescribe));

		return bos;
	}
	friend bistream& operator>>(bistream& bis,CMD_GP_ModifyUnderWrite& pl)
	{	
		pl.reset();
		bis >> pl.dwUserID;

		bis.read((char*)pl.szDescribe,sizeof(pl.szDescribe));

		return bis;
	}
};


//个性签名
struct CMD_GP_UnderWrite
{
	enum {M_ID = MDM_GP_USER};
	enum {S_ID = SUB_GP_UNDERWRITE};
	
	unsigned int	dwUserID;		//玩家数字账号
	char	szUnderWrite[UNDER_WRITE_LEN];		//个性签名
	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GP_UnderWrite() { reset(); }
	friend bostream& operator<<(bostream& bos,const CMD_GP_UnderWrite& pl)
	{
		bos << pl.dwUserID;

		bos.write((char*)pl.szUnderWrite,sizeof(pl.szUnderWrite));

		return bos;
	}
	friend bistream& operator>>(bistream& bis,CMD_GP_UnderWrite& pl)
	{	
		pl.reset();
		
		bis >> pl.dwUserID;
		bis.read((char*)pl.szUnderWrite,sizeof(pl.szUnderWrite));

		return bis;
	}
};

//是否有游客账号
struct CMD_GP_HasGuest
{
	enum {M_ID = MDM_GP_USER};
	enum {S_ID = SUB_GP_HASGUEST};
	
	unsigned int	dwUserID;		//玩家数字账号
	unsigned char	cbHasGuest;		//是否有游客账号
	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GP_HasGuest() { reset(); }
	friend bostream& operator<<(bostream& bos,const CMD_GP_HasGuest& pl)
	{
		bos << pl.dwUserID;
		bos << pl.cbHasGuest;

		return bos;
	}
	friend bistream& operator>>(bistream& bis,CMD_GP_HasGuest& pl)
	{	
		pl.reset();
		
		bis >> pl.dwUserID;
		bis >> pl.cbHasGuest;

		return bis;
	}
};

//修改结果
struct CMD_GP_ModifyUnderWriteResult
{
	enum {M_ID = MDM_GP_USER};
	enum {S_ID = SUB_GP_MODIFY_UNDERWRITE_RESULT};
	
	short	nCode;					//结果
	char	szAccount[NAME_LEN];	//玩家ID
	char	szDescribeMsg[128];		//描述信息
	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GP_ModifyUnderWriteResult() { reset(); }
	friend bostream& operator<<(bostream& bos,const CMD_GP_ModifyUnderWriteResult& pl)
	{
		bos << pl.nCode;

		bos.write((char*)pl.szAccount,sizeof(pl.szAccount));
		bos.write((char*)pl.szDescribeMsg,sizeof(pl.szDescribeMsg));

		return bos;
	}
	friend bistream& operator>>(bistream& bis,CMD_GP_ModifyUnderWriteResult& pl)
	{	
		pl.reset();
		
		bis >> pl.nCode;
		bis.read((char*)pl.szAccount,sizeof(pl.szAccount));
		bis.read((char*)pl.szDescribeMsg,sizeof(pl.szDescribeMsg));

		return bis;
	}
};

//查询用户是否能领推广奖励
struct CMD_GP_UserCanSpread
{
	enum {M_ID = MDM_GP_USER};
	enum {S_ID = SUB_GP_USER_CAN_SPREAD_REQ};

	unsigned int	dwUserID;		//玩家ID
	char	szComputerID[COMPUTER_ID_LEN];	//硬件标识

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GP_UserCanSpread() { reset(); }
	friend bostream& operator<<(bostream& bos,const CMD_GP_UserCanSpread& pl)
	{
		bos << pl.dwUserID;
		bos.write((char*)pl.szComputerID,sizeof(pl.szComputerID));

		return bos;
	}
	friend bistream& operator>>(bistream& bis,CMD_GP_UserCanSpread& pl)
	{	
		pl.reset();
		bis >> pl.dwUserID;
		bis.read((char*)pl.szComputerID,sizeof(pl.szComputerID));

		return bis;
	}
};

struct CMD_GP_UserCanSpreadResult
{
	enum {M_ID = MDM_GP_USER};
	enum {S_ID = SUB_GP_USER_CAN_SPREAD_REP};

	unsigned int	dwUserID;		//玩家ID
	short	nCode;					//结果
	char	szDescribeMsg[128];		//描述信息

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GP_UserCanSpreadResult() { reset(); }
	friend bostream& operator<<(bostream& bos,const CMD_GP_UserCanSpreadResult& pl)
	{
		bos << pl.dwUserID;
		bos << pl.nCode;
		
		bos.write((char*)pl.szDescribeMsg,sizeof(pl.szDescribeMsg));

		return bos;
	}
	friend bistream& operator>>(bistream& bis,CMD_GP_UserCanSpreadResult& pl)
	{	
		pl.reset();
		bis >> pl.dwUserID;
		bis >> pl.nCode;

		bis.read((char*)pl.szDescribeMsg,sizeof(pl.szDescribeMsg));

		return bis;
	}
};

//查询用户是否能领推广奖励
struct CMD_GP_UserGetSpread
{
	enum {M_ID = MDM_GP_USER};
	enum {S_ID = SUB_GP_USER_GET_SPREAD_REQ};

	unsigned int	dwUserID;		//玩家ID
	unsigned int	dwSpreaderID;	//推广员ID
	char	szComputerID[COMPUTER_ID_LEN];	//硬件标识

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GP_UserGetSpread() { reset(); }
	friend bostream& operator<<(bostream& bos,const CMD_GP_UserGetSpread& pl)
	{
		bos << pl.dwUserID;
		bos << pl.dwSpreaderID;

		bos.write((char*)pl.szComputerID,sizeof(pl.szComputerID));

		return bos;
	}
	friend bistream& operator>>(bistream& bis,CMD_GP_UserGetSpread& pl)
	{	
		pl.reset();
		bis >> pl.dwUserID;
		bis >> pl.dwSpreaderID;
		
		bis.read((char*)pl.szComputerID,sizeof(pl.szComputerID));

		return bis;
	}
};

struct CMD_GP_UserGetSpreadResult
{
	enum {M_ID = MDM_GP_USER};
	enum {S_ID = SUB_GP_USER_GET_SPREAD_REP};

	unsigned int	dwUserID;		//玩家ID
	unsigned int	dwSpreaderID;	//推广员ID
	unsigned int	dwAmount;		//奖励数
	unsigned int	dwType;			//奖励类型
	short	nCode;					//结果
	char	szDescribeMsg[128];		//描述信息

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GP_UserGetSpreadResult() { reset(); }
	friend bostream& operator<<(bostream& bos,const CMD_GP_UserGetSpreadResult& pl)
	{
		bos << pl.dwUserID;
		bos << pl.dwSpreaderID;
		bos << pl.dwAmount;
		bos << pl.dwType;
		bos << pl.nCode;
		
		bos.write((char*)pl.szDescribeMsg,sizeof(pl.szDescribeMsg));

		return bos;
	}
	friend bistream& operator>>(bistream& bis,CMD_GP_UserGetSpreadResult& pl)
	{	
		pl.reset();
		bis >> pl.dwUserID;
		bis >> pl.dwSpreaderID;
		bis >> pl.dwAmount;
		bis >> pl.dwType;
		bis >> pl.nCode;

		bis.read((char*)pl.szDescribeMsg,sizeof(pl.szDescribeMsg));

		return bis;
	}
};

//查询推广信息
struct CMD_GP_UserGetSpreaderInfo
{
	enum {M_ID = MDM_GP_USER};
	enum {S_ID = SUB_GP_USER_SPREADER_INFO_REQ};

	unsigned int	dwSpreaderID;	//推广员ID

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GP_UserGetSpreaderInfo() { reset(); }
	friend bostream& operator<<(bostream& bos,const CMD_GP_UserGetSpreaderInfo& pl)
	{
		bos << pl.dwSpreaderID;

		return bos;
	}
	friend bistream& operator>>(bistream& bis,CMD_GP_UserGetSpreaderInfo& pl)
	{	
		pl.reset();
		bis >> pl.dwSpreaderID;

		return bis;
	}
};

struct CMD_GP_UserGetSpreaderInfoResult
{
	enum {M_ID = MDM_GP_USER};
	enum {S_ID = SUB_GP_USER_SPREADER_INFO_REP};

	unsigned int	dwSpreaderID;	//推广员ID
	unsigned int	dwAmount;		//奖励总数
	unsigned int	dwUsers;			//奖励总人数
	short	nCount;					//结果集人数，最大为10
	short	nCode;					//结果
	short	nAward[10];				//已发放的奖励数。最多支持10个
	char	szNickName[10][NAME_LEN];		//昵称。最多支持10个
	char	szDescribeMsg[128];		//描述信息

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GP_UserGetSpreaderInfoResult() { reset(); }
	friend bostream& operator<<(bostream& bos,const CMD_GP_UserGetSpreaderInfoResult& pl)
	{
		bos << pl.dwSpreaderID;
		bos << pl.dwAmount;
		bos << pl.dwUsers;
		bos << pl.nCount;
		bos << pl.nCode;
		
		bos.write((char*)pl.nAward,sizeof(pl.nAward));
		bos.write((char*)pl.szNickName,sizeof(pl.szNickName));
		bos.write((char*)pl.szDescribeMsg,sizeof(pl.szDescribeMsg));

		return bos;
	}
	friend bistream& operator>>(bistream& bis,CMD_GP_UserGetSpreaderInfoResult& pl)
	{	
		pl.reset();
		bis >> pl.dwSpreaderID;
		bis >> pl.dwAmount;
		bis >> pl.dwUsers;
		bis >> pl.nCount;
		bis >> pl.nCode;

		bis.read((char*)pl.nAward,sizeof(pl.nAward));
		bis.read((char*)pl.szNickName,sizeof(pl.szNickName));
		bis.read((char*)pl.szDescribeMsg,sizeof(pl.szDescribeMsg));

		return bis;
	}
};




//定义头像
struct CMD_GP_UploadCustomFace
{
	enum {M_ID = MDM_GP_USER};
	enum {S_ID = SUB_GP_USER_UPLOAD_FACE};

	unsigned int								dwUserID;						//玩家 ID
	unsigned short								wCurrentSize;					//当前大小
	bool										bLastPacket;					//最后标识
	bool										bFirstPacket;					//第一个标识
	unsigned char								bFaceData[2048];				//头像数据

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GP_UploadCustomFace() { reset(); }
	friend bostream& operator<<(bostream& bos,const CMD_GP_UploadCustomFace& pl)
	{
		bos << pl.dwUserID;
		bos << pl.wCurrentSize;
		bos << pl.bLastPacket;
		bos << pl.bFirstPacket;

		bos.write((char*)pl.bFaceData,sizeof(pl.bFaceData));

		return bos;
	}
	friend bistream& operator>>(bistream& bis,CMD_GP_UploadCustomFace& pl)
	{	
		pl.reset();
		bis >> pl.dwUserID;
		bis >> pl.wCurrentSize;
		bis >> pl.bLastPacket;
		bis >> pl.bFirstPacket;

		bis.read((char*)pl.bFaceData,sizeof(pl.bFaceData));

		return bis;
	}
};

//下载成功
struct CMD_GP_DownloadFaceSuccess
{
	enum {M_ID = MDM_GP_USER};
	enum {S_ID = SUB_GP_USER_DOWNLOAD_FACE};

	unsigned int							dwToltalSize;						//总共大小
	unsigned int							dwCurrentSize;						//当前大小
	unsigned int							dwUserID;							//玩家 ID
	unsigned char							bFaceData[2048];					//头像数据

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GP_DownloadFaceSuccess() { reset(); }
	friend bostream& operator<<(bostream& bos,const CMD_GP_DownloadFaceSuccess& pl)
	{
		bos << pl.dwToltalSize;
		bos << pl.dwCurrentSize;
		bos << pl.dwUserID;

		bos.write((char*)pl.bFaceData,sizeof(pl.bFaceData));

		return bos;
	}
	friend bistream& operator>>(bistream& bis,CMD_GP_DownloadFaceSuccess& pl)
	{	
		pl.reset();
		bis >> pl.dwToltalSize;
		bis >> pl.dwCurrentSize;
		bis >> pl.dwUserID;

		bis.read((char*)pl.bFaceData,sizeof(pl.bFaceData));

		return bis;
	}
};

//下载头像
struct CMD_GP_DownloadFace
{
	enum {M_ID = MDM_GP_USER};
	enum {S_ID = SUB_GP_USER_DOWNLOAD_FACE};

	unsigned int							dwUserID;							//玩家 ID

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GP_DownloadFace() { reset(); }
	friend bostream& operator<<(bostream& bos,const CMD_GP_DownloadFace& pl)
	{
		bos << pl.dwUserID;

		return bos;
	}
	friend bistream& operator>>(bistream& bis,CMD_GP_DownloadFace& pl)
	{	
		pl.reset();
		bis >> pl.dwUserID;

		return bis;
	}
};

//上传结果
struct CMD_GP_UploadFaceResult
{
	enum {M_ID = MDM_GP_USER};
	enum {S_ID = SUB_GP_UPLOAD_FACE_RESULT};

	unsigned int							dwFaceVer;							//头像版本
	bool							bOperateSuccess;					//成功标识
	char							szDescribeMsg[128];					//描述信息

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GP_UploadFaceResult() { reset(); }
	friend bostream& operator<<(bostream& bos,const CMD_GP_UploadFaceResult& pl)
	{
		bos << pl.dwFaceVer;
		bos << pl.bOperateSuccess;

		bos.write((char*)pl.szDescribeMsg,sizeof(pl.szDescribeMsg));

		return bos;
	}
	friend bistream& operator>>(bistream& bis,CMD_GP_UploadFaceResult& pl)
	{	
		pl.reset();
		bis >> pl.dwFaceVer;
		bis >> pl.bOperateSuccess;

		bis.read((char*)pl.szDescribeMsg,sizeof(pl.szDescribeMsg));

		return bis;
	}
};

//删除结果
struct CMD_GP_DeleteFaceResult
{
	enum {M_ID = MDM_GP_USER};
	enum {S_ID = SUB_GP_DELETE_FACE_RESULT};

	unsigned int							dwFaceVer;							//头像版本
	bool							bOperateSuccess;					//成功标识
	char							szDescribeMsg[128];					//描述信息

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GP_DeleteFaceResult() { reset(); }
	friend bostream& operator<<(bostream& bos,const CMD_GP_DeleteFaceResult& pl)
	{
		bos << pl.dwFaceVer;
		bos << pl.bOperateSuccess;

		bos.write((char*)pl.szDescribeMsg,sizeof(pl.szDescribeMsg));

		return bos;
	}
	friend bistream& operator>>(bistream& bis,CMD_GP_DeleteFaceResult& pl)
	{	
		pl.reset();
		bis >> pl.dwFaceVer;
		bis >> pl.bOperateSuccess;

		bis.read((char*)pl.szDescribeMsg,sizeof(pl.szDescribeMsg));

		return bis;
	}
};

//删除消息
struct CMD_GP_CustomFaceDelete
{
	enum {M_ID = MDM_GP_USER};
	enum {S_ID = SUB_GP_CUSTOM_FACE_DELETE};

	unsigned int							dwUserID;							//玩家 ID
	unsigned int							dwFaceVer;							//头像版本

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GP_CustomFaceDelete() { reset(); }
	friend bostream& operator<<(bostream& bos,const CMD_GP_CustomFaceDelete& pl)
	{
		bos << pl.dwUserID;
		bos << pl.dwFaceVer;

		return bos;
	}
	friend bistream& operator>>(bistream& bis,CMD_GP_CustomFaceDelete& pl)
	{	
		pl.reset();
		bis >> pl.dwUserID;
		bis >> pl.dwFaceVer;

		return bis;
	}
};

////////////////////////////////////////////////////////////////

//查询弹出公告
struct CMD_GP_UserBbsPop
{
	enum {M_ID = MDM_GP_USER};
	enum {S_ID = SUB_GP_USER_BBS_POP_REQ};

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GP_UserBbsPop() { reset(); }
	friend bostream& operator<<(bostream& bos,const CMD_GP_UserBbsPop& pl)
	{
		return bos;
	}
	friend bistream& operator>>(bistream& bis,CMD_GP_UserBbsPop& pl)
	{
		return bis;
	}
};

//查询弹出公告
struct CMD_GP_UserBbsPop_V2
{
	enum {M_ID = MDM_GP_USER};
	enum {S_ID = SUB_GP_USER_BBS_POP_REQ_V2};
	
	char	szChannel[NAME_LEN];			//渠道号

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GP_UserBbsPop_V2() { reset(); }
	friend bostream& operator<<(bostream& bos,const CMD_GP_UserBbsPop_V2& pl)
	{
		bos.write((char*)pl.szChannel,sizeof(pl.szChannel));

		return bos;
	}
	friend bistream& operator>>(bistream& bis,CMD_GP_UserBbsPop_V2& pl)
	{	
		bis.read((char*)pl.szChannel,sizeof(pl.szChannel));

		return bis;
	}
};

struct CMD_GP_UserBbsPopResult
{
	enum {M_ID = MDM_GP_USER};
	enum {S_ID = SUB_GP_USER_BBS_POP_REP};

	//功能按钮的枚举值nAction
	enum {	ACTION_IKNOW = 1,		//知道了
			ACTION_CHANGE,			//兑换
			ACTION_ACTIVE,			//活动
			ACTION_EVERYDAYTASK,	//每日任务
			ACTION_SYSTASK,			//系统任务
			ACTION_INVITE,			//邀请
			ACTION_USERCENTER,		//个人中心
			ACTION_FEEDBACK,		//意见反馈
			ACTION_DAGONG3,			//三人拱
			ACTION_DAGONG4,			//四人拱
			ACTION_WEB_MAJIANG,
			ACTION_WEIHU,			//维护公告
			ACTION_NULL
	};

	short	nBbsScroll;				//滚动公告的条数。放在此接口是为了提升性能
	char	nAction;				//按钮动作
	char	szBbsListIDs[64];		//列表公告ID值，|分隔多条
	char	szBbsPop[512];		//弹出公告内容

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GP_UserBbsPopResult() { reset(); }
	friend bostream& operator<<(bostream& bos,const CMD_GP_UserBbsPopResult& pl)
	{
		bos << pl.nBbsScroll;
		bos << pl.nAction;
		
		bos.write((char*)pl.szBbsListIDs,sizeof(pl.szBbsListIDs));
		bos.write((char*)pl.szBbsPop,sizeof(pl.szBbsPop));

		return bos;
	}
	friend bistream& operator>>(bistream& bis,CMD_GP_UserBbsPopResult& pl)
	{	
		pl.reset();
		bis >> pl.nBbsScroll;
		bis >> pl.nAction;

		bis.read((char*)pl.szBbsListIDs,sizeof(pl.szBbsListIDs));
		bis.read((char*)pl.szBbsPop,sizeof(pl.szBbsPop));

		return bis;
	}
};

//查询滚动公告
struct CMD_GP_UserBbsScroll
{
	enum {M_ID = MDM_GP_USER};
	enum {S_ID = SUB_GP_USER_BBS_SCROLL_REQ_V2};

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GP_UserBbsScroll() { reset(); }
	friend bostream& operator<<(bostream& bos,const CMD_GP_UserBbsScroll& pl)
	{
		return bos;
	}
	friend bistream& operator>>(bistream& bis,CMD_GP_UserBbsScroll& pl)
	{
		return bis;
	}
};

//查询滚动公告
struct CMD_GP_UserBbsScroll_V2
{
	enum {M_ID = MDM_GP_USER};
	enum {S_ID = SUB_GP_USER_BBS_SCROLL_REQ};

	char	szChannel[NAME_LEN];			//渠道号

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GP_UserBbsScroll_V2() { reset(); }
	friend bostream& operator<<(bostream& bos,const CMD_GP_UserBbsScroll_V2& pl)
	{
		bos.write((char*)pl.szChannel,sizeof(pl.szChannel));

		return bos;
	}
	friend bistream& operator>>(bistream& bis,CMD_GP_UserBbsScroll_V2& pl)
	{	
		bis.read((char*)pl.szChannel,sizeof(pl.szChannel));

		return bis;
	}
};

struct CMD_GP_UserBbsScrollResult
{
	enum {M_ID = MDM_GP_USER};
	enum {S_ID = SUB_GP_USER_BBS_SCROLL_REP};

	short	nBbsScroll;				//滚动公告的条数.最多10条
	char	szBbsScroll[10][100];	//滚动公告内容

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GP_UserBbsScrollResult() { reset(); }
	friend bostream& operator<<(bostream& bos,const CMD_GP_UserBbsScrollResult& pl)
	{
		bos << pl.nBbsScroll;

		bos.write((char*)pl.szBbsScroll,sizeof(pl.szBbsScroll));

		return bos;
	}
	friend bistream& operator>>(bistream& bis,CMD_GP_UserBbsScrollResult& pl)
	{	
		pl.reset();
		bis >> pl.nBbsScroll;

		bis.read((char*)pl.szBbsScroll,sizeof(pl.szBbsScroll));

		return bis;
	}
};

//查询列表公告
struct CMD_GP_UserBbsList
{
	enum {M_ID = MDM_GP_USER};
	enum {S_ID = SUB_GP_USER_BBS_LIST_REQ_V2};

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GP_UserBbsList() { reset(); }
	friend bostream& operator<<(bostream& bos,const CMD_GP_UserBbsList& pl)
	{
		return bos;
	}
	friend bistream& operator>>(bistream& bis,CMD_GP_UserBbsList& pl)
	{
		return bis;
	}
};

//查询列表公告
struct CMD_GP_UserBbsList_V2
{
	enum {M_ID = MDM_GP_USER};
	enum {S_ID = SUB_GP_USER_BBS_LIST_REQ};

	char	szChannel[NAME_LEN];			//渠道号

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GP_UserBbsList_V2() { reset(); }
	friend bostream& operator<<(bostream& bos,const CMD_GP_UserBbsList_V2& pl)
	{
		bos.write((char*)pl.szChannel,sizeof(pl.szChannel));

		return bos;
	}
	friend bistream& operator>>(bistream& bis,CMD_GP_UserBbsList_V2& pl)
	{	
		bis.read((char*)pl.szChannel,sizeof(pl.szChannel));

		return bis;
	}
};

struct CMD_GP_UserBbsListResult
{
	enum {M_ID = MDM_GP_USER};
	enum {S_ID = SUB_GP_USER_BBS_LIST_REP};

	short	nBbsList;			//列表公告的条数.最多3条
	short	iType[3];			//公告类型
	int		iID[3];				//公告ID
	char	szTitle[3][100];	//公告标题
	char	szDate[3][64];		//公告时间
	char	szBbsList[3][512];	//公告内容

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GP_UserBbsListResult() { reset(); }
	friend bostream& operator<<(bostream& bos,const CMD_GP_UserBbsListResult& pl)
	{
		bos << pl.nBbsList;

		bos.write((char*)pl.iType,sizeof(pl.iType));
		bos.write((char*)pl.iID,sizeof(pl.iID));
		bos.write((char*)pl.szTitle,sizeof(pl.szTitle));
		bos.write((char*)pl.szDate,sizeof(pl.szDate));
		bos.write((char*)pl.szBbsList,sizeof(pl.szBbsList));

		return bos;
	}
	friend bistream& operator>>(bistream& bis,CMD_GP_UserBbsListResult& pl)
	{	
		pl.reset();
		bis >> pl.nBbsList;

		bis.read((char*)pl.iType,sizeof(pl.iType));
		bis.read((char*)pl.iID,sizeof(pl.iID));
		bis.read((char*)pl.szTitle,sizeof(pl.szTitle));
		bis.read((char*)pl.szDate,sizeof(pl.szDate));
		bis.read((char*)pl.szBbsList,sizeof(pl.szBbsList));

		return bis;
	}
};

////////////////////////////////////////////////////
//查询玩家任务信息
struct CMD_GP_UserTaskInfo
{
	enum {M_ID = MDM_GP_USER};
	enum {S_ID = SUB_GP_USER_TASK_INFO_REQ};
	
	unsigned int	dwUserID;		//玩家ID

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GP_UserTaskInfo() { reset(); }
	friend bostream& operator<<(bostream& bos,const CMD_GP_UserTaskInfo& pl)
	{
		bos << pl.dwUserID;

		return bos;
	}
	friend bistream& operator>>(bistream& bis,CMD_GP_UserTaskInfo& pl)
	{	
		pl.reset();

		bis >> pl.dwUserID;

		return bis;
	}
};

struct CMD_GP_UserTaskInfoResult
{
	enum {M_ID = MDM_GP_USER};
	enum {S_ID = SUB_GP_USER_TASK_INFO_REP};

	unsigned int	dwUserID;		//玩家ID
	char	szRule[512];		//任务描述。考虑到任务的多样性，所以此处定义为字符串，具体内容根据需要自定义。
								//目前定义的有：t3:1|a3:5|t4:0|a4:10|st3:1|st4:0|sa3:1|sa4:2
								//具体含义：t3表示三人拱的今天盘数
								//			t4表示四人拱的今天盘数
								//			a3表示三人拱的总盘数
								//			a4表示四人拱的总盘数
								//			st3表示三人拱的每日任务的已领奖励阶段，依次为0,1,2,3
								//			st4表示四人拱的每日任务的已领奖励阶段，依次为0,1,2,3
								//			sa3表示三人拱的系统任务的已领奖励阶段，依次为0,1,2,3....13
								//			sa4表示四人拱的系统任务的已领奖励阶段，依次为0,1,2,3....13

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GP_UserTaskInfoResult() { reset(); }
	friend bostream& operator<<(bostream& bos,const CMD_GP_UserTaskInfoResult& pl)
	{
		bos << pl.dwUserID;

		bos.write((char*)pl.szRule,sizeof(pl.szRule));

		return bos;
	}
	friend bistream& operator>>(bistream& bis,CMD_GP_UserTaskInfoResult& pl)
	{	
		pl.reset();

		bis >> pl.dwUserID;

		bis.read((char*)pl.szRule,sizeof(pl.szRule));

		return bis;
	}
};

///////////////////////////////////////////////////////

//排行榜查询请求.采用分页查询
struct CMD_GP_UserRankingReq
{
	enum {M_ID = MDM_GP_USER};
	enum {S_ID = SUB_GP_USER_RANKING_REQ};
	
	unsigned int					nPage;			//页码,从1开始
	unsigned int					dwUserID;		//玩家 ID	

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GP_UserRankingReq() { reset(); }
	friend bostream& operator<<(bostream& bos,const CMD_GP_UserRankingReq& pl)
	{
		bos << pl.nPage;
		bos << pl.dwUserID;

		return bos;
	}
	friend bistream& operator>>(bistream& bis,CMD_GP_UserRankingReq& pl)
	{	
		bis >> pl.nPage;
		bis >> pl.dwUserID;

		return bis;
	}
};

//排行榜查询请求.采用分页查询
struct CMD_GP_UserRankingReq_V2
{
	enum {M_ID = MDM_GP_USER};
	enum {S_ID = SUB_GP_USER_RANKING_REQ_V2};

	unsigned int					nPage;			//页码,从1开始
	unsigned int					dwUserID;		//玩家 ID	

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GP_UserRankingReq_V2() { reset(); }
	friend bostream& operator<<(bostream& bos,const CMD_GP_UserRankingReq_V2& pl)
	{
		bos << pl.nPage;
		bos << pl.dwUserID;

		return bos;
	}
	friend bistream& operator>>(bistream& bis,CMD_GP_UserRankingReq_V2& pl)
	{	
		bis >> pl.nPage;
		bis >> pl.dwUserID;

		return bis;
	}
};

//排行榜详细
struct CMD_GP_UserRankingRecord
{
	enum {M_ID = MDM_GP_USER};
	enum {S_ID = SUB_GP_USER_RANKING_RECORD};
	
	short							wIndex;				//名次
	unsigned char					cbGender;			//用户性别
	unsigned int					dwGoldScore;		//用户财富	
	unsigned int					dwUserID;			//玩家 ID	
	char							szNickName[32];		//昵称	


	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GP_UserRankingRecord() { reset(); }
	friend bostream& operator<<(bostream& bos,const CMD_GP_UserRankingRecord& pl)
	{
		bos << pl.wIndex;
		bos << pl.cbGender;
		bos << pl.dwGoldScore;
		bos << pl.dwUserID;

		bos.write((char*)pl.szNickName,sizeof(pl.szNickName));

		return bos;
	}
	friend bistream& operator>>(bistream& bis,CMD_GP_UserRankingRecord& pl)
	{	
		pl.reset();

		bis >> pl.wIndex;
		bis >> pl.cbGender;
		bis >> pl.dwGoldScore;
		bis >> pl.dwUserID;

		bis.read((char*)pl.szNickName,sizeof(pl.szNickName));

		return bis;
	}
};

//排行榜详细
struct CMD_GP_UserRankingRecord_V2
{
	enum {M_ID = MDM_GP_USER};
	enum {S_ID = SUB_GP_USER_RANKING_RECORD_V2};

	short							wIndex;				//名次
	unsigned char					cbGender;			//用户性别
	unsigned int					dwGoldScore;		//用户财富	
	unsigned int					dwUserID;			//玩家 ID	
	char							szNickName[32];		//昵称
	char							szDescribeInfo[128];//个性签名


	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GP_UserRankingRecord_V2() { reset(); }
	friend bostream& operator<<(bostream& bos,const CMD_GP_UserRankingRecord_V2& pl)
	{
		bos << pl.wIndex;
		bos << pl.cbGender;
		bos << pl.dwGoldScore;
		bos << pl.dwUserID;

		bos.write((char*)pl.szNickName,sizeof(pl.szNickName));
		bos.write((char*)pl.szDescribeInfo,sizeof(pl.szDescribeInfo));

		return bos;
	}
	friend bistream& operator>>(bistream& bis,CMD_GP_UserRankingRecord_V2& pl)
	{	
		pl.reset();

		bis >> pl.wIndex;
		bis >> pl.cbGender;
		bis >> pl.dwGoldScore;
		bis >> pl.dwUserID;

		bis.read((char*)pl.szNickName,sizeof(pl.szNickName));
		bis.read((char*)pl.szDescribeInfo,sizeof(pl.szDescribeInfo));

		return bis;
	}
};

//排行榜列表结束
struct CMD_GP_UserRankingRecordFinish
{
	enum {M_ID = MDM_GP_USER};
	enum {S_ID = SUB_GP_USER_RANKING_RECORD_FINISH};

	short				nCode;						//结果
	unsigned int		dwTotal;					//参与排名的总玩家数。如果当前页条数为0，则总条数字段也无意义，会为0
	unsigned int		dwTotalofCurPage;			//当前页玩家总数	
	int					dwMyRanking;				//我的排名。为0时，表示未入榜
	unsigned int		dwUserID;					//玩家 ID	
	char				szDate[64];					//排行榜生成时间	

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GP_UserRankingRecordFinish() { reset(); }
	friend bostream& operator<<(bostream& bos,const CMD_GP_UserRankingRecordFinish& pl)
	{
		bos << pl.dwTotal;
		bos << pl.dwTotalofCurPage;
		bos << pl.dwMyRanking;
		bos << pl.dwUserID;

		bos.write((char*)pl.szDate,sizeof(pl.szDate));

		return bos;
	}
	friend bistream& operator>>(bistream& bis,CMD_GP_UserRankingRecordFinish& pl)
	{	
		pl.reset();

		bis >> pl.dwTotal;
		bis >> pl.dwTotalofCurPage;
		bis >> pl.dwMyRanking;
		bis >> pl.dwUserID;

		bis.read((char*)pl.szDate,sizeof(pl.szDate));

		return bis;
	}
};

//玩家排行榜状态信息结果
struct CMD_GP_UserRepRankingStatus
{
	enum { M_ID = MDM_GP_USER };
	enum { S_ID = SUB_GP_USER_REP_RANKINGSTATUS };

	unsigned int		dwUserID;					//玩家 ID
	char				isInRanking;				//玩家是否入榜。为1时参加排名，为0时不参加排名

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GP_UserRepRankingStatus() { reset(); }
	friend bostream& operator<<(bostream& bos, const CMD_GP_UserRepRankingStatus& pl)
	{
		bos << pl.dwUserID;
		bos << pl.isInRanking;

		return bos;
	}
	friend bistream& operator>>(bistream& bis, CMD_GP_UserRepRankingStatus& pl)
	{
		pl.reset();

		bis >> pl.dwUserID;
		bis >> pl.isInRanking;

		return bis;
	}
};

//设置玩家是否入排行榜
struct CMD_GP_UserModifyRankingStatus
{
	enum { M_ID = MDM_GP_USER };
	enum { S_ID = SUB_GP_USER_MODIFY_RANKINGSTATUS };

	unsigned int		dwUserID;					//玩家 ID
	char				isInRanking;				//玩家是否入榜。为1时参加排名，为0时不参加排名

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GP_UserModifyRankingStatus() { reset(); }
	friend bostream& operator<<(bostream& bos, const CMD_GP_UserModifyRankingStatus& pl)
	{
		bos << pl.dwUserID;
		bos << pl.isInRanking;

		return bos;
	}
	friend bistream& operator>>(bistream& bis, CMD_GP_UserModifyRankingStatus& pl)
	{
		pl.reset();

		bis >> pl.dwUserID;
		bis >> pl.isInRanking;

		return bis;
	}
};

//返回设置玩家是否入排行榜结果
struct CMD_GP_UserRepModifyRankingStatus
{
	enum { M_ID = MDM_GP_USER };
	enum { S_ID = SUB_GP_USER_REP_MODIFY_RANKINGSTATUS };

	short				nCode;					//结果
	unsigned int		dwUserID;				//玩家 ID

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GP_UserRepModifyRankingStatus() { reset(); }
	friend bostream& operator<<(bostream& bos, const CMD_GP_UserRepModifyRankingStatus& pl)
	{
		bos << pl.nCode;
		bos << pl.dwUserID;

		return bos;
	}
	friend bistream& operator>>(bistream& bis, CMD_GP_UserRepModifyRankingStatus& pl)
	{
		pl.reset();

		bis >> pl.nCode;
		bis >> pl.dwUserID;

		return bis;
	}
};
////////////////////////////////////////////////////////

//提交问题反馈请求
struct CMD_GP_UserFeedBackReq
{
	enum {M_ID = MDM_GP_USER};
	enum {S_ID = SUB_GP_USER_FEEDBACK_REQ};

	unsigned int		dwUserID;			//玩家 ID
	char				szPicture[128];		//图片地址	
	char				szContent[372];		//意见内容	

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GP_UserFeedBackReq() { reset(); }
	friend bostream& operator<<(bostream& bos,const CMD_GP_UserFeedBackReq& pl)
	{
		bos << pl.dwUserID;
		
		bos.write((char*)pl.szPicture,sizeof(pl.szPicture));
		bos.write((char*)pl.szContent,sizeof(pl.szContent));

		return bos;
	}
	friend bistream& operator>>(bistream& bis,CMD_GP_UserFeedBackReq& pl)
	{	
		bis >> pl.dwUserID;

		bis.read((char*)pl.szPicture,sizeof(pl.szPicture));
		bis.read((char*)pl.szContent,sizeof(pl.szContent));

		return bis;
	}
};

//提交问题反馈的结果
struct CMD_GP_UserFeedBackResult
{
	enum {M_ID = MDM_GP_USER};
	enum {S_ID = SUB_GP_USER_FEEDBACK_REP};

	short				nCode;				//结果	
	char				szDetails[128];		//描述		

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GP_UserFeedBackResult() { reset(); }
	friend bostream& operator<<(bostream& bos,const CMD_GP_UserFeedBackResult& pl)
	{
		bos << pl.nCode;
		
		bos.write((char*)pl.szDetails,sizeof(pl.szDetails));

		return bos;
	}
	friend bistream& operator>>(bistream& bis,CMD_GP_UserFeedBackResult& pl)
	{	
		pl.reset();

		bis >> pl.nCode;

		bis.read((char*)pl.szDetails,sizeof(pl.szDetails));

		return bis;
	}
};

//问题反馈的结果.采用分页查询
struct CMD_GP_UserGetFeedBackReq
{
	enum {M_ID = MDM_GP_USER};
	enum {S_ID = SUB_GP_USER_GET_FEEDBACK_REQ};
	
	unsigned int					nPage;			//页码,从1开始
	unsigned int					dwUserID;		//玩家 ID	

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GP_UserGetFeedBackReq() { reset(); }
	friend bostream& operator<<(bostream& bos,const CMD_GP_UserGetFeedBackReq& pl)
	{
		bos << pl.nPage;
		bos << pl.dwUserID;

		return bos;
	}
	friend bistream& operator>>(bistream& bis,CMD_GP_UserGetFeedBackReq& pl)
	{	
		bis >> pl.nPage;
		bis >> pl.dwUserID;

		return bis;
	}
};

//问题反馈的详细
struct CMD_GP_UserFeedBackRecord
{
	enum {M_ID = MDM_GP_USER};
	enum {S_ID = SUB_GP_USER_FEEDBACK_RECORD};
	
	short							wIndex;				//显示顺序
	unsigned char					cbHavePic;			//是否有图片		
	char							szPicture[128];		//图片地址	
	char							szDate[32];			//问题提交时间
	unsigned char					szQuestion[372];	//问题描述
	unsigned char					szAnswer[256];		//运维回复

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GP_UserFeedBackRecord() { reset(); }
	friend bostream& operator<<(bostream& bos,const CMD_GP_UserFeedBackRecord& pl)
	{
		bos << pl.wIndex;
		bos << pl.cbHavePic;

		bos.write((char*)pl.szPicture,sizeof(pl.szPicture));
		bos.write((char*)pl.szDate,sizeof(pl.szDate));
		bos.write((char*)pl.szQuestion,sizeof(pl.szQuestion));
		bos.write((char*)pl.szAnswer,sizeof(pl.szAnswer));

		return bos;
	}
	friend bistream& operator>>(bistream& bis,CMD_GP_UserFeedBackRecord& pl)
	{	
		pl.reset();

		bis >> pl.wIndex;
		bis >> pl.cbHavePic;

		bis.read((char*)pl.szPicture,sizeof(pl.szPicture));
		bis.read((char*)pl.szDate,sizeof(pl.szDate));
		bis.read((char*)pl.szQuestion,sizeof(pl.szQuestion));
		bis.read((char*)pl.szAnswer,sizeof(pl.szAnswer));

		return bis;
	}
};

//问题反馈列表结束
struct CMD_GP_UserFeedBackRecordFinish
{
	enum {M_ID = MDM_GP_USER};
	enum {S_ID = SUB_GP_USER_FEEDBACK_RECORD_FINISH};

	short				nCode;						//结果	
	unsigned int		dwUserID;					//玩家 ID	
	unsigned int		dwTotal;					//总条数。如果当前页条数为0，则总条数字段也无意义，会为0
	unsigned int		dwTotalofCurPage;			//当前页总数	

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GP_UserFeedBackRecordFinish() { reset(); }
	friend bostream& operator<<(bostream& bos,const CMD_GP_UserFeedBackRecordFinish& pl)
	{
		bos << pl.nCode;
		bos << pl.dwUserID;
		bos << pl.dwTotal;
		bos << pl.dwTotalofCurPage;

		return bos;
	}
	friend bistream& operator>>(bistream& bis,CMD_GP_UserFeedBackRecordFinish& pl)
	{	
		pl.reset();

		bis >> pl.nCode;
		bis >> pl.dwUserID;
		bis >> pl.dwTotal;
		bis >> pl.dwTotalofCurPage;

		return bis;
	}
};


//请求商城商品
struct CMD_GP_UserMallProductReq
{
	enum { M_ID = MDM_GP_USER };
	enum { S_ID = SUB_GP_USER_MALL_PRODUCT_REQ };

	unsigned int					dwUserID;		//玩家 ID

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GP_UserMallProductReq() { reset(); }
	friend bostream& operator<<(bostream& bos, const CMD_GP_UserMallProductReq& pl)
	{
		bos << pl.dwUserID;

		return bos;
	}
	friend bistream& operator>>(bistream& bis, CMD_GP_UserMallProductReq& pl)
	{
		bis >> pl.dwUserID;

		return bis;
	}
};

//请求商城商品
struct CMD_GP_UserMallProductReq_IOS
{
	enum { M_ID = MDM_GP_USER };
	enum { S_ID = SUB_GP_USER_MALL_PRODUCT_REQ_IOS };

	unsigned int					dwUserID;		//玩家 ID

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GP_UserMallProductReq_IOS() { reset(); }
	friend bostream& operator<<(bostream& bos, const CMD_GP_UserMallProductReq_IOS& pl)
	{
		bos << pl.dwUserID;

		return bos;
	}
	friend bistream& operator>>(bistream& bis, CMD_GP_UserMallProductReq_IOS& pl)
	{
		bis >> pl.dwUserID;

		return bis;
	}
};

//商城商品列表
struct CMD_GP_UserMallProductResp
{
	enum { M_ID = MDM_GP_USER };
	enum { S_ID = SUB_GP_USER_MALL_PRODUCT_RESP };

	unsigned int					productID;			//商品 ID
	unsigned int					productPrice;			//价格
	char							szProductName[32];	//商品名称
	char							szProductImage[128];	//商品图片

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GP_UserMallProductResp() { reset(); }
	friend bostream& operator<<(bostream& bos, const CMD_GP_UserMallProductResp& pl)
	{
		bos << pl.productID;
		bos << pl.productPrice;

		bos.write((char*)pl.szProductName, sizeof(pl.szProductName));
		bos.write((char*)pl.szProductImage, sizeof(pl.szProductImage));

		return bos;
	}
	friend bistream& operator>>(bistream& bis, CMD_GP_UserMallProductResp& pl)
	{
		pl.reset();

		bis >> pl.productID;
		bis >> pl.productPrice;

		bis.read((char*)pl.szProductName, sizeof(pl.szProductName));
		bis.read((char*)pl.szProductImage, sizeof(pl.szProductImage));

		return bis;
	}


};


//请求商城商品V2
struct CMD_GP_UserMallProductReq_V2
{
	enum { M_ID = MDM_GP_USER };
	enum { S_ID = SUB_GP_USER_MALL_PRODUCT_REQ_V2 };

	unsigned int					dwUserID;		//玩家 ID
	unsigned int					dwVersion;		//版本信息（1-android, 2-ios），如果以后要更新版本呢

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GP_UserMallProductReq_V2() { reset(); }
	friend bostream& operator<<(bostream& bos, const CMD_GP_UserMallProductReq_V2& pl)
	{
		bos << pl.dwUserID;
		bos << pl.dwVersion;

		return bos;
	}
	friend bistream& operator>>(bistream& bis, CMD_GP_UserMallProductReq_V2& pl)
	{
		bis >> pl.dwUserID;
		bis >> pl.dwVersion;

		return bis;
	}
};

//商城商品列表
struct CMD_GP_UserMallProductResp_V2
{
	enum { M_ID = MDM_GP_USER };
	enum { S_ID = SUB_GP_USER_MALL_PRODUCT_RESP_V2 };

	unsigned int					dwVersion;		//版本信息，如果以后要更新版本呢
	unsigned int					productID;			//商品 ID
	unsigned int					productPrice;			//原价格
	unsigned char					cbHotFlag;			//热销标识：0~3
	char							szProductName[32];		//商品名称
	char							szProductNameAdd[32];	//赠送描述
	char							szProductImage[128];	//商品图片
	char							szProductExt[64];		//预留

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GP_UserMallProductResp_V2() { reset(); }
	friend bostream& operator<<(bostream& bos, const CMD_GP_UserMallProductResp_V2& pl)
	{
		bos << pl.dwVersion;
		bos << pl.productID;
		bos << pl.productPrice;
		bos << pl.cbHotFlag;

		bos.write((char*)pl.szProductName, sizeof(pl.szProductName));
		bos.write((char*)pl.szProductNameAdd, sizeof(pl.szProductNameAdd));
		bos.write((char*)pl.szProductImage, sizeof(pl.szProductImage));
		bos.write((char*)pl.szProductExt, sizeof(pl.szProductExt));

		return bos;
	}
	friend bistream& operator>>(bistream& bis, CMD_GP_UserMallProductResp_V2& pl)
	{
		pl.reset();

		bis >> pl.dwVersion;
		bis >> pl.productID;
		bis >> pl.productPrice;
		bis >> pl.cbHotFlag;

		bis.read((char*)pl.szProductName, sizeof(pl.szProductName));
		bis.read((char*)pl.szProductNameAdd, sizeof(pl.szProductNameAdd));
		bis.read((char*)pl.szProductImage, sizeof(pl.szProductImage));
		bis.read((char*)pl.szProductExt, sizeof(pl.szProductExt));

		return bis;
	}
};

//商城商品列表结束
struct CMD_GP_UserMallProductRespFinish
{
	enum { M_ID = MDM_GP_USER };
	enum { S_ID = SUB_GP_USER_MALL_PRODUCT_RESP_FINISH };

	unsigned int					dwTotal;		//总数

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GP_UserMallProductRespFinish() { reset(); }
	friend bostream& operator<<(bostream& bos, const CMD_GP_UserMallProductRespFinish& pl)
	{
		bos << pl.dwTotal;

		return bos;
	}
	friend bistream& operator>>(bistream& bis, CMD_GP_UserMallProductRespFinish& pl)
	{
		pl.reset();

		bis >> pl.dwTotal;

		return bis;
	}

};

//商城购买商品请求下订单
struct CMD_GP_UserMallPlaceOrderReq
{
	enum { M_ID = MDM_GP_USER };
	enum { S_ID = SUB_GP_USER_MALL_PLACE_ORDER_REQ };

	unsigned int					dwUserID;		//玩家 ID
	unsigned int					dwProductID;	//商品 ID
	char							szPayWay[10];	//购买方式		

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GP_UserMallPlaceOrderReq() { reset(); }
	friend bostream& operator<<(bostream& bos, const CMD_GP_UserMallPlaceOrderReq& pl)
	{
		bos << pl.dwUserID;
		bos << pl.dwProductID;
		bos.write((char*)pl.szPayWay, sizeof(pl.szPayWay));

		return bos;
	}
	friend bistream& operator>>(bistream& bis, CMD_GP_UserMallPlaceOrderReq& pl)
	{
		bis >> pl.dwUserID;
		bis >> pl.dwProductID;
		bis.read((char*)pl.szPayWay, sizeof(pl.szPayWay));

		return bis;
	}
};

//返回订单
struct CMD_GP_UserMallPlaceOrderResp
{
	enum { M_ID = MDM_GP_USER };
	enum { S_ID = SUB_GP_USER_MALL_PLACE_ORDER_RESP };

	short nResult;		//结果				
	char dwOrderID[20];	//订单ID
	char token[128];	//token信息
	char productName[32]; //商品名称
	int productPrice; //商品价格
	char billingCode[20];	// 商品计费码

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GP_UserMallPlaceOrderResp() { reset(); }
	friend bostream& operator<<(bostream& bos, const CMD_GP_UserMallPlaceOrderResp& pl)
	{
		bos << pl.nResult;
		bos << pl.productPrice;

		bos.write((char*)pl.dwOrderID, sizeof(pl.dwOrderID));
		bos.write((char*)pl.token, sizeof(pl.token));
		bos.write((char*)pl.productName, sizeof(pl.productName));
		bos.write((char*)pl.billingCode, sizeof(pl.billingCode));
		
		return bos;
	}
	friend bistream& operator>>(bistream& bis, CMD_GP_UserMallPlaceOrderResp& pl)
	{
		pl.reset();

		bis >> pl.nResult;
		bis >> pl.productPrice;

		bis.read((char*)pl.dwOrderID, sizeof(pl.dwOrderID));
		bis.read((char*)pl.token, sizeof(pl.token));
		bis.read((char*)pl.productName, sizeof(pl.productName));
		bis.read((char*)pl.billingCode, sizeof(pl.billingCode));

		return bis;
	}

};

//请求取消订单号
struct CMD_GP_UserCancelOrderNumReq
{
	enum { M_ID = MDM_GP_USER };
	enum { S_ID = SUB_GP_USER_CANCEL_ORDER_REQ };

	char dwOrderID[20];	//订单ID

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GP_UserCancelOrderNumReq() { reset(); }
	friend bostream& operator<<(bostream& bos, const CMD_GP_UserCancelOrderNumReq& pl)
	{
		bos.write((char*)pl.dwOrderID, sizeof(pl.dwOrderID));

		return bos;
	}
	friend bistream& operator>>(bistream& bis, CMD_GP_UserCancelOrderNumReq& pl)
	{
		bis.read((char*)pl.dwOrderID, sizeof(pl.dwOrderID));

		return bis;
	}
};

//返回取消订单结果
struct CMD_GP_UserCancelOrderResp
{
	enum { M_ID = MDM_GP_USER };
	enum { S_ID = SUB_GP_USER_CANCEL_ORDER_REP };

	short nResult;		//结果		

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GP_UserCancelOrderResp() { reset(); }
	friend bostream& operator<<(bostream& bos, const CMD_GP_UserCancelOrderResp& pl)
	{
		bos << pl.nResult;

		return bos;
	}
	friend bistream& operator>>(bistream& bis, CMD_GP_UserCancelOrderResp& pl)
	{
		pl.reset();

		bis >> pl.nResult;

		return bis;
	}
};
//请求商品购买结果
struct CMD_GP_UserMallBuyReq
{
	enum { M_ID = MDM_GP_USER };
	enum { S_ID = SUB_GP_USER_MALL_BUY_REQ };

	//unsigned int userId;
	char dwOrderID[20];	//订单ID

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GP_UserMallBuyReq() { reset(); }
	friend bostream& operator<<(bostream& bos, const CMD_GP_UserMallBuyReq& pl)
	{
		//bos << pl.userId;
		bos.write((char*)pl.dwOrderID, sizeof(pl.dwOrderID));

		return bos;
	}
	friend bistream& operator>>(bistream& bis, CMD_GP_UserMallBuyReq& pl)
	{
		//bis >> pl.userId;
		bis.read((char*)pl.dwOrderID, sizeof(pl.dwOrderID));

		return bis;
	}
};
//返回商品购买结果
struct CMD_GP_UserMallBuyResp
{
	enum { M_ID = MDM_GP_USER };
	enum { S_ID = SUB_GP_USER_MALL_BUY_RESP };

	unsigned int productID;			//商品 ID
	unsigned int nResult;	//购买结果
	unsigned int dwGoldScore;	//用户财富

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GP_UserMallBuyResp() { reset(); }
	friend bostream& operator<<(bostream& bos, const CMD_GP_UserMallBuyResp& pl)
	{
		bos << pl.nResult;
		bos << pl.productID;
		bos << pl.dwGoldScore;

		return bos;
	}
	friend bistream& operator>>(bistream& bis, CMD_GP_UserMallBuyResp& pl)
	{
		bis >> pl.nResult;
		bis >> pl.productID;
		bis >> pl.dwGoldScore;

		return bis;
	}
};
//请求更新商品购买状态
struct CMD_GP_UserMallUpdateReq
{
	enum { M_ID = MDM_GP_USER };
	enum { S_ID = SUB_GP_USER_MALL_UPDATE_REQ };

	//unsigned int userId;
	char dwOrderID[20];	//订单ID

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GP_UserMallUpdateReq() { reset(); }
	friend bostream& operator<<(bostream& bos, const CMD_GP_UserMallUpdateReq& pl)
	{
		//bos << pl.userId;
		bos.write((char*)pl.dwOrderID, sizeof(pl.dwOrderID));

		return bos;
	}
	friend bistream& operator>>(bistream& bis, CMD_GP_UserMallUpdateReq& pl)
	{
		//bis >> pl.userId;
		bis.read((char*)pl.dwOrderID, sizeof(pl.dwOrderID));

		return bis;
	}
};
//返回更新商品购买状态
struct CMD_GP_UserMallUpdateResp
{
	enum { M_ID = MDM_GP_USER };
	enum { S_ID = SUB_GP_USER_MALL_BUY_RESP };

	unsigned int productID;			//商品 ID
	unsigned int nResult;	//购买结果
	unsigned int dwGoldScore;	//用户财富

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GP_UserMallUpdateResp() { reset(); }
	friend bostream& operator<<(bostream& bos, const CMD_GP_UserMallUpdateResp& pl)
	{
		bos << pl.nResult;
		bos << pl.productID;
		bos << pl.dwGoldScore;

		return bos;
	}
	friend bistream& operator>>(bistream& bis, CMD_GP_UserMallUpdateResp& pl)
	{
		bis >> pl.nResult;
		bis >> pl.productID;
		bis >> pl.dwGoldScore;

		return bis;
	}
};

//查询用户的银行信息
struct CMD_GP_UserBankInfoReq
{
	enum { M_ID = MDM_GP_USER };
	enum { S_ID = SUB_GP_USER_BANK_INFO_REQ };

	unsigned int					dwUserID;		//玩家 ID

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GP_UserBankInfoReq() { reset(); }
	friend bostream& operator<<(bostream& bos, const CMD_GP_UserBankInfoReq& pl)
	{
		bos << pl.dwUserID;

		return bos;
	}
	friend bistream& operator>>(bistream& bis, CMD_GP_UserBankInfoReq& pl)
	{
		bis >> pl.dwUserID;

		return bis;
	}
};

//查询用户的银行信息的结果
struct CMD_GP_UserBankInfoRep
{
	enum { M_ID = MDM_GP_USER };
	enum { S_ID = SUB_GP_USER_BANK_INFO_REP };

	short							nCode;			//返回码：0为成功
	unsigned int					dwUserID;		//玩家 ID
	unsigned int					dwCurScore;		//当前带的游戏分数
	unsigned int					dwBankScore;	//银行分数

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GP_UserBankInfoRep() { reset(); }
	friend bostream& operator<<(bostream& bos, const CMD_GP_UserBankInfoRep& pl)
	{
		bos << pl.nCode;
		bos << pl.dwUserID;
		bos << pl.dwCurScore;
		bos << pl.dwBankScore;

		return bos;
	}
	friend bistream& operator>>(bistream& bis, CMD_GP_UserBankInfoRep& pl)
	{
		bis >> pl.nCode;
		bis >> pl.dwUserID;
		bis >> pl.dwCurScore;
		bis >> pl.dwBankScore;

		return bis;
	}
};

//用户操作银行：服务端会校验几个条件(总分是否与数据库端保持一致，存分时是否银行分数大于数据库中分数，取分时是否相反)
struct CMD_GP_UserBankChargeReq
{
	enum { M_ID = MDM_GP_USER };
	enum { S_ID = SUB_GP_USER_BANK_CHARGE_REQ };

	short							nOpCode;		//操作码：1为存，2为取
	unsigned int					dwUserID;		//玩家 ID
	unsigned int					dwCurScore;		//操作后希望：带的游戏分数
	unsigned int					dwBankScore;	//操作后希望：银行分数

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GP_UserBankChargeReq() { reset(); }
	friend bostream& operator<<(bostream& bos, const CMD_GP_UserBankChargeReq& pl)
	{
		bos << pl.nOpCode;
		bos << pl.dwUserID;
		bos << pl.dwCurScore;
		bos << pl.dwBankScore;

		return bos;
	}
	friend bistream& operator>>(bistream& bis, CMD_GP_UserBankChargeReq& pl)
	{
		bis >> pl.nOpCode;
		bis >> pl.dwUserID;
		bis >> pl.dwCurScore;
		bis >> pl.dwBankScore;

		return bis;
	}
};

//用户操作银行的结果
struct CMD_GP_UserBankChargeRep
{
	enum { M_ID = MDM_GP_USER };
	enum { S_ID = SUB_GP_USER_BANK_CHARGE_REP };

	short							nCode;			//返回码：0为成功
	unsigned int					dwUserID;		//玩家 ID
	unsigned int					dwCurScore;		//成功后的结果：当前带的游戏分数
	unsigned int					dwBankScore;	//成功后的结果：银行分数

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GP_UserBankChargeRep() { reset(); }
	friend bostream& operator<<(bostream& bos, const CMD_GP_UserBankChargeRep& pl)
	{
		bos << pl.nCode;
		bos << pl.dwUserID;
		bos << pl.dwCurScore;
		bos << pl.dwBankScore;

		return bos;
	}
	friend bistream& operator>>(bistream& bis, CMD_GP_UserBankChargeRep& pl)
	{
		bis >> pl.nCode;
		bis >> pl.dwUserID;
		bis >> pl.dwCurScore;
		bis >> pl.dwBankScore;

		return bis;
	}
};

// 请求校验发送喇叭内容
struct CMD_GP_UserCheckLabaReq
{
	enum { M_ID = MDM_GP_USER };
	enum { S_ID = SUB_GP_USER_CHECK_LABA_REQ };
	
	short			iLen;
	char			strLabaInfo[256];	// 喇叭内容

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GP_UserCheckLabaReq() { reset(); }
	friend bostream& operator<<(bostream& bos, const CMD_GP_UserCheckLabaReq& pl)
	{
		bos << pl.iLen;
		bos.write((char*)pl.strLabaInfo, pl.iLen);

		return bos;
	}
	friend bistream& operator>>(bistream& bis, CMD_GP_UserCheckLabaReq& pl)
	{
		bis >> pl.iLen;
		bis.read((char*)pl.strLabaInfo, pl.iLen);

		return bis;
	}
};

//校验喇叭结果
struct CMD_GP_UserCheckLabaRep
{
	enum { M_ID = MDM_GP_USER };
	enum { S_ID = SUB_GP_USER_CHECK_LABA_REP };

	short			nCode;				// 返回码：0为成功
	short			nLen;
	char			strLabaInfo[256];	// 喇叭内容

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GP_UserCheckLabaRep() { reset(); }
	friend bostream& operator<<(bostream& bos, const CMD_GP_UserCheckLabaRep& pl)
	{
		bos << pl.nCode;
		bos << pl.nLen;
		bos.write((char*)pl.strLabaInfo, pl.nLen);

		return bos;
	}
	friend bistream& operator>>(bistream& bis, CMD_GP_UserCheckLabaRep& pl)
	{
		bis >> pl.nCode;
		bis >> pl.nLen;
		bis.read((char*)pl.strLabaInfo, pl.nLen);

		return bis;
	}
};

// 请求发送喇叭
struct CMD_GP_UserSendLabaReq
{
	enum { M_ID = MDM_GP_USER };
	enum { S_ID = SUB_GP_USER_SEND_LABA_REQ};

	unsigned int					dwUserID;		//玩家 ID
	short							nLen;
	char							strLabaInfo[256];	// 喇叭内容

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GP_UserSendLabaReq() { reset(); }
	friend bostream& operator<<(bostream& bos, const CMD_GP_UserSendLabaReq& pl)
	{
		bos << pl.dwUserID;
		bos << pl.nLen;

		bos.write((char*)pl.strLabaInfo, pl.nLen);

		return bos;
	}
	friend bistream& operator>>(bistream& bis, CMD_GP_UserSendLabaReq& pl)
	{
		bis >> pl.dwUserID;
		bis >> pl.nLen;

		bis.read((char*)pl.strLabaInfo, pl.nLen);
		return bis;
	}
};

// 发送喇叭结果
struct CMD_GP_UserSendLabaRep
{
	enum { M_ID = MDM_GP_USER };
	enum { S_ID = SUB_GP_USER_SEND_LABA_REP };

	short							nCode;			// 返回码：0为成功
	unsigned int					dwUserID;		//玩家 ID
	unsigned int					dwCurScore;		//成功后的结果：当前带的游戏分数

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GP_UserSendLabaRep() { reset(); }
	friend bostream& operator<<(bostream& bos, const CMD_GP_UserSendLabaRep& pl)
	{
		bos << pl.nCode;
		bos << pl.dwUserID;
		bos << pl.dwCurScore;

		return bos;
	}
	friend bistream& operator>>(bistream& bis, CMD_GP_UserSendLabaRep& pl)
	{
		bis >> pl.nCode;
		bis >> pl.dwUserID;
		bis >> pl.dwCurScore;

		return bis;
	}
};

//接收到喇叭消息
struct CMD_GP_SpeakerMsgRep
{
	enum {M_ID = MDM_GP_USER};
	enum {S_ID = SUB_GP_USER_SPEAKER_MSG_REP};

	unsigned short						nType;					//消息类型:1用户消息，2系统消息
	unsigned short						nLen;					//消息长度
	char								szMsg[256];				//消息内容

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GP_SpeakerMsgRep() { reset(); }
	friend bostream& operator<<(bostream& bos,const CMD_GP_SpeakerMsgRep& pl)
	{
		bos << pl.nType;
		bos << pl.nLen;

		bos.write((char*)pl.szMsg, pl.nLen);

		return bos;
	}
	friend bistream& operator>>(bistream& bis,CMD_GP_SpeakerMsgRep& pl)
	{	
		pl.reset();
		bis >> pl.nType;
		bis >> pl.nLen;

		bis.read((char*)pl.szMsg, pl.nLen);

		return bis;
	}
};


// 用户请求查询配置参数
struct CMD_GP_UserQueryConfigReq
{
	enum { M_ID = MDM_GP_USER };
	enum { S_ID = SUB_GP_USER_QUERY_CONFIG_REQ };

	unsigned int					dwUserID;		//玩家 ID
	unsigned int					dwVersion;		//版本信息:  major<<16 + minor
	unsigned int					dwConfigID;		//配置项的编码：按位取。0表示此用户此版本的所有配置项
	char	szChannel[NAME_LEN];			//渠道号

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GP_UserQueryConfigReq() { reset(); }
	friend bostream& operator<<(bostream& bos, const CMD_GP_UserQueryConfigReq& pl)
	{
		bos << pl.dwUserID;
		bos << pl.dwVersion;
		bos << pl.dwConfigID;
		
		bos.write((char*)pl.szChannel, sizeof(pl.szChannel));

		return bos;
	}
	friend bistream& operator>>(bistream& bis, CMD_GP_UserQueryConfigReq& pl)
	{
		bis >> pl.dwUserID;
		bis >> pl.dwVersion;
		bis >> pl.dwConfigID;

		bis.read((char*)pl.szChannel, sizeof(pl.szChannel));

		return bis;
	}
};

// 返回某个配置
struct CMD_GP_UserQueryConfigRep
{
	enum { M_ID = MDM_GP_USER };
	enum { S_ID = SUB_GP_USER_QUERY_CONFIG_REP };

	unsigned int					dwUserID;		//玩家 ID
	unsigned int					dwVersion;		//版本信息
	unsigned short					nLen;			//消息长度。如果为0表示所有配置项均发送结束
	char							szConfig[256];		//消息内容:key|value   第1个竖线前是key值，后面的全部是对应的值（也可能有竖线）。每条消息只会有一组。

	void reset() { memset(this, 0, sizeof(*this)); }
	CMD_GP_UserQueryConfigRep() { reset(); }
	friend bostream& operator<<(bostream& bos, const CMD_GP_UserQueryConfigRep& pl)
	{
		bos << pl.dwUserID;
		bos << pl.dwVersion;
		bos << pl.nLen;

		bos.write((char*)pl.szConfig, pl.nLen);

		return bos;
	}
	friend bistream& operator>>(bistream& bis, CMD_GP_UserQueryConfigRep& pl)
	{
		bis >> pl.dwUserID;
		bis >> pl.dwVersion;
		bis >> pl.nLen;

		bis.read((char*)pl.szConfig, pl.nLen);

		return bis;
	}
};


//////////////////////////////////////////////////////////////////////////

#endif
