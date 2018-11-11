#ifndef COMMONALITY_HEAD_FILE
#define COMMONALITY_HEAD_FILE

#pragma once

#include "Stdafx.h"

//////////////////////////////////////////////////////////////////////////
//数据库对象定义

//数据库请求标识
#define	DBR_GP_LOGON_BY_ACCOUNTS	1									//帐号登录
//#define	DBR_GP_LOGON_BY_USERID	2									//I D 登录
//#define DBR_GP_REGISTER_ACCOUNTS	3									//注册帐号    
#define DBR_GP_USER_UPLOAD_FACE		4									//上传头像
#define DBR_GP_USER_LEAVE			5									//玩家离开
#define DBR_GP_DOWNLOAD_FACE		6									//下载头像
#define DBR_GP_DELETE_FACE			7									//删除头像
#define DBR_GP_MODIFY_INDIVIDUAL	8									//修改资料

#define DBR_GP_READ_GRADE_INFO      10                                  //用户等级        彭文添加
#define DBR_GP_UPDATE_USERINFO      11                                  //更新用户信息    彭文添加

#define DBR_GP_GET_GIFT_GLOD		12                                  //得到免费的金币  彭文添加
#define DBR_GP_CONTINUE_CONNECT		13									//启动常连接数据库	Author:<cxf>

//数据库输出标识
#define DBR_GP_LOGON_SUCCESS		100									//登录成功
#define DBR_GP_LOGON_ERROR			101									//登录失败
#define DBR_GP_DOWNLOADFACE_SUCCESS	102									//下载成功
#define DBR_GP_UPLOAD_FACE_RESULT	103									//上传结果
#define DBR_GP_DELETE_FACE_RESULT	104									//删除结果
#define DBR_GP_MODIFY_RESULT		105									//修改结果

#define DBR_GP_UPDATE_USER_SUCC     106                                 //更新用户信息成功 彭文添加
#define DBR_GP_UPDATE_USER_FAIL     107                                 //更新用户信息失败 彭文添加


#define DBR_GR_GRADE_INFO_NUM       120                                 //用户等级信息     彭文添加
#define DBR_GR_GRADE_INFO_ONE       121                                 //一条用户等级信息 彭文添加

#define DBR_GP_GIFT_MONEY_RESULT    125                                 //免费领取结果


//免费申请
//免费申请
typedef    CMD_GP_GiftGold          DBR_GP_GiftGold;
typedef    CMD_GP_GiftGoldResult    DBR_GP_GiftGoldResult;   

//帐号登录
struct DBR_GP_LogonByAccounts
{
	DBR_GP_LogonByAccounts()
	{
			memset(this, 0, sizeof(*this));
	}
	DWORD							dwClientIP;							//连接地址
	TCHAR							szAccounts[NAME_LEN];				//登录帐号
	TCHAR							szPassWord[PASS_LEN];				//登录密码
	TCHAR							szComputerID[COMPUTER_ID_LEN];		//机器序列
};

//ID 登录
struct DBR_GP_LogonByUserID
{
	DBR_GP_LogonByUserID()
	{
			memset(this, 0, sizeof(*this));
	}
	DWORD							dwClientIP;							//连接地址
	DWORD							dwUserID;							//用户 I D
	TCHAR							szPassWord[PASS_LEN];				//登录密码
	TCHAR							szComputerID[COMPUTER_ID_LEN];		//机器序列
};

//用户信息查询结构
typedef CMD_GP_UpdateUserInfo       DBR_GP_UpdateUserInfo;

//用户信息更新成功结构
struct DBR_GP_UserInfoSucc
{
	DBR_GP_UserInfoSucc()
	{
			memset(this, 0, sizeof(*this));
	}
	WORD								wFaceID;						//头像索引
	BYTE								cbGender;						//用户性
	TCHAR								szNickName[NAME_LEN];	        //昵称
	LONGLONG							lMoney;							//游戏金币
	LONGLONG                            lGold;                          //用户的金币 
	LONGLONG                            lGem;                           //用户的宝石 
};

//帐号注册
struct DBR_GP_RegisterAccounts
{
	DBR_GP_RegisterAccounts()
	{
			memset(this, 0, sizeof(*this));
	}
	WORD							wFaceID;							//注册头像
	BYTE							cbGender;							//用户性别
	DWORD							dwClientIP;							//连接地址
	TCHAR							szSpreader[NAME_LEN];				//推荐人名
	TCHAR							szAccounts[NAME_LEN];				//登录帐号
	TCHAR							szPassWord[PASS_LEN];				//登录密码
	TCHAR							szComputerID[COMPUTER_ID_LEN];		//机器序列
	TCHAR                           szEmail[EMAIL_LEN];                 //email长度  彭文添加
};


//登录成功
struct DBR_GP_LogonSuccess
{
	DBR_GP_LogonSuccess()
	{
			memset(this, 0, sizeof(*this));
	}
	
	//属性资料
	DWORD			dwUserID;									//用户标识
	DWORD			dwGameID;								//游戏标识
	DWORD			dwExperience;							//经验数值
	TCHAR				szPassword[PASS_LEN];			//登录密码
	TCHAR				szAccounts[NAME_LEN];			//登录帐号
	TCHAR				szGroupName[GROUP_LEN];	//社团名字
	
	//用户资料
	WORD				wFaceID;							//头像标识
	BYTE					cbGender;							//用户性别
	DWORD			dwCustomFaceVer;			//头像版本
	BYTE					cbMember;						//会员等级
	TCHAR				szUnderWrite[UNDER_WRITE_LEN];		//个性签名

	//描述信息
	TCHAR				szDescribeString[128];				//描述消息
	DWORD           dwLockServerID;                     //被锁的服务器id，不为0就是被锁了         彭文添加

	//彭文添加
	TCHAR				szNickName[UNDER_WRITE_LEN];	   //个性签名
	LONGLONG		lMoney;							   //藏宝币
	LONGLONG    lGold;                             //用户的金币  2011-9-30前名字为lGems
	LONGLONG    lGem;                              //用户的宝石  2011-9-30前名字为lHonor
	DWORD          dwGrade;                           //用户的等级（不超过256） 2011-9-30前为dwClass

	LONG				lWinCount;							//胜利盘数
	LONG				lLostCount;							//失败盘数
	LONG				lDrawCount;							//和局盘数
	LONG				lFleeCount;							//断线数目
	USER_TYPE       utIsAndroid;                        //是否是机器人
	TCHAR              szKey[33];                          //网站用的key
	LONG               lGiftNum;                           //领取赠送的次数（目前是一天一次）
	TCHAR				szHashID[33];						// 碎片升的用的版本号
	int					nMasterRight;						//管理权限
	int					nMasterOrder;						//权限等级

	TCHAR				szComputerID[COMPUTER_ID_LEN];		//机器序列
	//添加结束
};


//等级信息结构
struct DBR_GR_gradeInfo
{
	DBR_GR_gradeInfo()
	{
			memset(this, 0, sizeof(*this));
	}
	int   id;
	TCHAR szName[24];
	int   iMaxExp;
};
//添加结束

//登录失败
struct DBR_GP_LogonError
{
	DBR_GP_LogonError()
	{
			memset(this, 0, sizeof(*this));
	}
	LONG							lErrorCode;							//错误代码
	TCHAR							szErrorDescribe[128];				//错误消息
};

//上传头像
struct DBR_GP_UploadCustomFace
{
	DBR_GP_UploadCustomFace()
	{
			memset(this, 0, sizeof(*this));
	}
	DWORD							dwUserID;							//玩家 ID
};

//上传结果
struct DBR_GP_UploadFaceResult
{
	DBR_GP_UploadFaceResult()
	{
			memset(this, 0, sizeof(*this));
	}
	TCHAR							szDescribeMsg[128];					//描述信息
	DWORD							dwFaceVer;							//头像版本
	bool							bOperateSuccess;					//成功标识
};

//删除结果
struct DBR_GP_DeleteFaceResult
{
    DBR_GP_DeleteFaceResult()
	{
		memset(this, 0, sizeof(*this));
	}
	TCHAR							szDescribeMsg[128];					//描述信息
	DWORD							dwFaceVer;							//头像版本
	bool							bOperateSuccess;					//成功标识
};

//下载头像
struct DBR_GP_DownloadCustomFace
{
	DBR_GP_DownloadCustomFace()
	{
		memset(this, 0, sizeof(*this));
	}
	DWORD							dwUserID;							//玩家 ID
};

//下载成功
struct DBR_GP_DownloadFaceSuccess
{
	DBR_GP_DownloadFaceSuccess()
	{
		memset(this, 0, sizeof(*this));
	}
	DWORD							dwToltalSize;						//总共大小
	DWORD							dwCurrentSize;						//当前大小
	DWORD							dwUserID;							//玩家 ID
	BYTE							bFaceData[2048];					//头像数据
};

//删除消息
struct DBR_GP_CustomFaceDelete
{
	DBR_GP_CustomFaceDelete()
	{
		memset(this, 0, sizeof(*this));
	}
	DWORD							dwUserID;							//玩家 ID
	DWORD							dwFaceVer;							//头像版本
};

//个人资料
struct DBR_GP_ModifyIndividual
{
	DBR_GP_ModifyIndividual()
	{
		memset(this, 0, sizeof(*this));
	}
	DWORD							dwUserID;							//玩家 ID
	TCHAR							szNickname[NAME_LEN];				//玩家昵称
	int								nGender;							//玩家性别
	int								nAge;								//玩家年龄
	TCHAR							szAddress[64];						//玩家地址
	TCHAR							szUnderWrite[32];					//个性签名
	TCHAR							szPassword[33];						//玩家密码
};

//修改结果
struct DBR_GP_ModifyIndividualResult
{
	DBR_GP_ModifyIndividualResult()
	{
		memset(this, 0, sizeof(*this));
	}
	TCHAR							szDescribeMsg[128];					//描述信息
	bool							bOperateSuccess;					//成功标识
};

//////////////////////////////////////////////////////////////////////////

#endif