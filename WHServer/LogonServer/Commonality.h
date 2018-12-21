#ifndef COMMONALITY_HEAD_FILE
#define COMMONALITY_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include <vector>
using std::vector;

//////////////////////////////////////////////////////////////////////////
//数据库对象定义

//数据库请求标识
#define	DBR_GP_LOGON_BY_ACCOUNTS	1									//帐号登录
#define	DBR_GP_LOGON_BY_USERID		2									//I D 登录
#define DBR_GP_REGISTER_ACCOUNTS	3									//注册帐号
#define DBR_GP_USER_UPLOAD_FACE		4									//上传头像
#define DBR_GP_USER_LEAVE			5									//玩家离开
#define DBR_GP_DOWNLOAD_FACE		6									//下载头像
#define DBR_GP_DELETE_FACE			7									//删除头像
#define DBR_GP_MODIFY_INDIVIDUAL	8									//修改资料
#define DBR_GP_USER_SIGNIN			9									//签到
#define DBR_GP_USER_EXCHANGE_INFO			10							//兑换的个人信息
#define DBR_GP_USER_EXCHANGE_PRODUCT		11							//兑换的产品列表
#define DBR_GP_USER_EXCHANGE				12							//兑换
#define DBR_GP_USER_EXCHANGE_RECORD			13							//兑换的记录
#define DBR_GP_USER_GETAWARD		14									//领奖
#define DBR_GP_CHECKID				15									//校验名称
#define DBR_GP_CHECK_NICKNAME		16									//校验昵称
#define DBR_GP_REGISTER				17									//注册
#define DBR_GP_ID_UPDATE			18									//帐号升级
#define DBR_GP_RESET_PWD			19									//重设密码
#define DBR_GP_USER_INFO			20									//查询用户信息
#define DBR_GP_USER_CAN_SPREAD		21									//用户能否领取推广奖励
#define DBR_GP_USER_GET_SPREAD		22									//用户领取推广奖励
#define DBR_GP_USER_SPREADER_INFO	23									//推广员信息
#define DBR_GP_USER_GET_BBS_POP		24									//查询弹窗公告信息
#define DBR_GP_USER_GET_BBS_SCROLL		25								//查询滚动公告信息
#define DBR_GP_USER_GET_BBS_LIST		26								//查询列表公告信息
#define DBR_GP_USER_GET_TASK_INFO		27								//查询玩家的任务统计信息
#define DBR_GP_USER_GET_BANKING			28								//查询玩家的排行榜
#define DBR_GP_USER_FEEDBACK			29								//玩家反馈意见
#define DBR_GP_USER_GET_FEEDBACK		30								//查询玩家意见
#define DBR_GP_USER_INFO_ID_CARD		31								//查询用户身份证账号信息
#define DBR_GP_USER_AUTHENTICATION		32								//请求提交身份证信息(姓名及身份证号)
#define DBR_GP_USER_MODIFY_RANKINGSTATUS		33						//修改玩家是否入排行榜
#define DBR_GP_USER_BANK_INFO			34								//查询玩家银行信息
#define DBR_GP_USER_BANK_CHARGE			35								//玩家操作银行
#define DBR_GP_USER_MALL_PRODUCT		36								//请求商城商品
#define DBR_GP_USER_MALL_PLACE_ORDER	37								//请求商城下订单
#define DBR_GP_USER_CANCEL_ORDER		38								//请求取消订单号
#define DBR_GP_USER_MALL_BUY			39								//请求商城商品购买结果
#define DBR_GP_USER_MALL_UPDATE			40								//请求商城查询订单号更新结果
#define DBR_GP_USER_MALL_PRODUCT_IOS	41								//请求商城商品(ios)

#define DBR_GP_USER_ACTIVITY_LIST			43							//活动列表
#define DBR_GP_USER_ACTIVITY				44							//指定活动
#define DBR_GP_USER_ACTIVITY_LUCKY			45							//活动抽奖
#define DBR_GP_USER_ACTIVITY_RECORD			46							//中奖记录

#define DBR_GP_USER_EXCHANGE_PRODUCT_V2		47							//兑换的产品列表
#define DBR_GP_USER_EXCHANGE_V2				48							//兑换

#define DBR_GP_USER_SPEAKER_SEND			49							//发送喇叭
#define DBR_GP_USER_QUERY_CONFIG			50							//查询配置项

#define DBR_GP_MODIFY_UNDERWRITE	            51							//修改个性签名
#define DBR_GP_REGISTER_ACCOUNTS_WX			52						    //注册帐号WX

#define DBR_GP_USER_GET_BANKING_V2			53							//查询玩家的排行榜

#define DBR_GP_MODIFY_PHONE	                54							//修改手机号
#define DBR_GP_WXUPDATE			            55						    //升级成WX帐号

//数据库输出标识
#define DBR_GP_LOGON_SUCCESS		100									//登录成功
#define DBR_GP_LOGON_ERROR			101									//登录失败
#define DBR_GP_DOWNLOADFACE_SUCCESS	102									//下载成功
#define DBR_GP_UPLOAD_FACE_RESULT	103									//上传结果
#define DBR_GP_DELETE_FACE_RESULT	104									//删除结果
#define DBR_GP_MODIFY_RESULT		105									//修改结果
#define DBR_GP_SIGNIN_RESULT		106									//签到结果
#define DBR_GP_EXCHANGE_INFO_RESULT			107							//况换的个人信息
#define DBR_GP_EXCHANGE_PRODUCT_RESULT		108							//兑换的产品
#define DBR_GP_EXCHANGE_RESULT				109							//兑换
#define DBR_GP_EXCHANGE_RECORD_RESULT		110							//兑换的记录
#define DBR_GP_GETAWARD_RESULT		111									//领奖结果
#define DBR_GP_CHECKID_RESULT		112									//校验名称结果
#define DBR_GP_CHECK_NICKNAME_RESULT	113								//校验昵称
#define DBR_GP_REGISTER_RESULT			114								//注册
#define DBR_GP_ID_UPDATE_RESULT			115								//帐号升级
#define DBR_GP_RESET_PWD_RESULT			116								//重设密码
#define DBR_GP_USER_INFO_RESULT			117								//查询用户信息
#define DBR_GP_USER_CAN_SPREAD_RESULT			118						//用户能否领取推广奖励
#define DBR_GP_USER_GET_SPREAD_RESULT			119						//用户领取推广奖励
#define DBR_GP_USER_SPREADER_INFO_RESULT		120						//推广员信息
#define DBR_GP_USER_GET_BBS_POP_RESULT			121						//查询弹窗公告信息
#define DBR_GP_USER_GET_BBS_SCROLL_RESULT		122						//查询滚动公告信息
#define DBR_GP_USER_GET_BBS_LIST_RESULT			123						//查询列表公告信息
#define DBR_GP_USER_GET_TASK_INFO_RESULT		124						//查询玩家的任务统计信息
#define DBR_GP_USER_GET_RANKING_RESULT			125						//排行榜的记录
#define DBR_GP_USER_FEEDBACK_RESULT				126						//玩家反馈意见的结果
#define DBR_GP_USER_GET_FEEDBACK_RESULT			127						//查询玩家意见的结果
#define DBR_GP_USER_INFO_ID_CARD_RESULT			128						//查询用户身份证账号信息的结果
#define DBR_GP_USER_AUTHENTICATION_RESULT		129						//请求提交身份证信息的结果
#define DBR_GP_USER_GET_RANKINGSTATUS			130						//查询玩家是否入排行榜
#define DBR_GP_USER_GET_MODIFY_RANKINGSTATUS	131						//修改玩家是否入排行榜返回信息

#define DBR_GP_USER_MALL_PRODUCT_RESULT			132						//商城商品列表信息结果
#define DBR_GP_USER_MALL_PLACE_ORDER_RESULT		133						//商城下订单结果
#define DBR_GP_USER_CANCEL_ORDER_NUM			134						//请求取消订单号
#define DBR_GP_USER_MALL_BUY_RESULT				135						//商城商品购买结果
#define DBR_GP_USER_MALL_UPDATE_RESULT			136						//商城查询订单号更新结果

#define DBR_GP_USER_BANK_INFO_RESULT			137						//查询玩家银行信息结果
#define DBR_GP_USER_BANK_CHARGE_RESULT			138						//玩家操作银行结果
#define DBR_GP_EXCHANGE_PRODUCT_RESULT_V2		139						//兑换的产品
#define DBR_GP_EXCHANGE_RESULT_V2				140						//兑换

#define DBR_GP_USER_ACTIVITY_LIST_RESULT		141						//查询活动列表信息结果
#define DBR_GP_USER_ACTIVITY_RESULT				142						//查询活动信息结果
#define DBR_GP_USER_ACTIVITY_LUCKY_RESULT		143						//活动抽奖结果
#define DBR_GP_USER_ACTIVITY_RECORD_RESULT		144						//查询活动列表信息结果

#define DBR_GP_USER_SPEAKER_SEND_RESULT			145						//发送喇叭结果
#define DBR_GP_USER_QUERY_CONFIG_RESULT			146						//查询配置项结果

#define DBR_GP_MODIFY_UNDERWRITE_RESULT		    147						//修改个性签名结果
#define DBR_GP_USER_GET_RANKING_RESULT_V2		148						//排行榜的记录

#define DBR_GP_MODIFY_PHONE_RESULT		        149						//修改手机号结果

//帐号登录
struct DBR_GP_LogonByAccounts
{
	DWORD							dwClientIP;							//连接地址
	TCHAR							szAccounts[NAME_LEN];				//登录帐号
	TCHAR							szPassWord[PASS_LEN];				//登录密码
	TCHAR							szComputerID[COMPUTER_ID_LEN];		//机器序列
};

//ID 登录
struct DBR_GP_LogonByUserID
{
	DWORD							dwClientIP;							//连接地址
	DWORD							dwUserID;							//用户 I D
	TCHAR							szPassWord[PASS_LEN];				//登录密码
	TCHAR							szComputerID[COMPUTER_ID_LEN];		//机器序列
};

//帐号注册
struct DBR_GP_RegisterAccounts
{
	WORD							wFaceID;							//注册头像
	BYTE							cbGender;							//用户性别
	DWORD							dwClientIP;							//连接地址
	TCHAR							szSpreader[NAME_LEN];				//推荐人名
	TCHAR							szAccounts[NAME_LEN];				//登录帐号
	TCHAR							szPassWord[PASS_LEN];				//登录密码
	TCHAR							szComputerID[COMPUTER_ID_LEN];		//机器序列
};

//WX帐号注册
struct DBR_GP_RegisterAccountsWX
{
	BYTE							cbGender;							//用户性别
	DWORD							dwClientIP;							//连接地址

	TCHAR							szSpreader[NAME_LEN];				//推荐人名

	TCHAR							szNickName[NAME_LEN];				//昵称
	TCHAR							szOpenID[32];						//微信唯一标识
	TCHAR							szUnionID[64];						//微信唯一标识
	TCHAR							szFaceUrl[256];						//头像地址

	TCHAR							szAccounts[NAME_LEN];				//登录帐号
	TCHAR							szPassWord[PASS_LEN];				//登录密码
	TCHAR							szWXComputerID[COMPUTER_ID_LEN];	//机器码(微信信息的唯一编码)
	TCHAR							szComputerID[COMPUTER_ID_LEN];		//机器序列
};

//升级成WX帐号
struct DBR_GP_WXUpdate
{
	DWORD							dwUserID;							//用户标识
	BYTE							cbGender;							//用户性别
	DWORD							dwClientIP;							//连接地址
	TCHAR							szNickName[NAME_LEN];				//昵称
	TCHAR							szOpenID[32];						//微信唯一标识
	TCHAR							szUnionID[64];						//微信唯一标识
	TCHAR							szFaceUrl[256];						//头像地址
	TCHAR							szWXComputerID[COMPUTER_ID_LEN];	//机器码(微信信息的唯一编码)
	TCHAR							szComputerID[COMPUTER_ID_LEN];		//机器序列
};

//登录成功
struct DBR_GP_LogonSuccess
{
	//属性资料
	DWORD							dwUserID;							//用户标识
	DWORD							dwGameID;							//游戏标识
	DWORD							dwExperience;						//经验数值
	LONG							lGoldScore;						//财富
	LONG							lWelfare;						//低保
	WORD							wWelfareLost;						//低保剩余次数
	TCHAR							szPassword[PASS_LEN];				//登录密码
	TCHAR							szAccounts[NAME_LEN];				//登录帐号
	TCHAR							szNickName[NAME_LEN];				//昵称号
	TCHAR							szGroupName[GROUP_LEN];				//社团名字

	//用户资料
	WORD							wFaceID;							//头像标识
	BYTE							cbGender;							//用户性别
	DWORD							dwCustomFaceVer;					//头像版本
	BYTE							cbIsGuest;							//是否为游客帐号
	BYTE							cbHasGuest;							//是否有游客帐号
	BYTE							cbMember;							//会员等级
	TCHAR							szUnderWrite[UNDER_WRITE_LEN];		//个性签名

	//描述信息
	TCHAR							szDescribeString[128];				//描述消息
	TCHAR							szRule[512];						//配置规则
};

//登录失败
struct DBR_GP_LogonError
{
	LONG							lErrorCode;							//错误代码
	TCHAR							szErrorDescribe[128];				//错误消息
};

//上传头像
struct DBR_GP_UploadCustomFace
{
	DWORD							dwUserID;							//玩家 ID
};

//上传结果
struct DBR_GP_UploadFaceResult
{
	TCHAR							szDescribeMsg[128];					//描述信息
	DWORD							dwFaceVer;							//头像版本
	bool							bOperateSuccess;					//成功标识
};

//删除结果
struct DBR_GP_DeleteFaceResult
{
	TCHAR							szDescribeMsg[128];					//描述信息
	DWORD							dwFaceVer;							//头像版本
	bool							bOperateSuccess;					//成功标识
};

//下载头像
struct DBR_GP_DownloadCustomFace
{
	DWORD							dwUserID;							//玩家 ID
};

//签到
struct DBR_GP_UserSignIn
{
	DWORD							dwUserID;							//玩家 ID
};

//签到结果
struct DBR_GP_UserSignInResult
{
	TCHAR							szDescribeMsg[128];					//描述信息
	unsigned short					wRankAward;
	unsigned short					wRankNum;							
	unsigned short					wAllSend;						
	unsigned short					wTodaySend;						
	unsigned short					wTimes;							
	unsigned char					cbTodaySign;
	DWORD							dwGoldScore;
	bool							bOperateSuccess;					//成功标识
};

//领奖
struct DBR_GP_UserGetAward
{
	DWORD							dwUserID;							//玩家 ID
	DWORD							dwTaskID;							//任务 ID
};

//领奖结果
struct DBR_GP_UserGetAwardResult
{
	DWORD							dwTaskID;
	DWORD							dwScore;				
	unsigned char					cbStatus;
	TCHAR							szDescribeMsg[128];					//描述信息
};

//兑换个人信息请求
struct DBR_GP_UserExchangeInfo
{
	DWORD							dwUserID;						//玩家 ID
	TCHAR							szPhone[32];					//电话
};

//兑换个人信息请求结果
struct DBR_GP_UserExchangeInfoResult
{
	char							szPhone[32];					//电话
	DWORD							dwGold;							//金豆
	bool							bOperateSuccess;				//成功标识
};

//兑换产品请求
struct DBR_GP_UserExchangeProduct
{
	DWORD							dwUserID;						//玩家 ID
};

//产品信息
struct DBR_GP_ExchangeProduct
{
	DWORD							dwAwardID;					//产品 ID
	DWORD							dwPrice;						//价格
	DWORD							dwLeft;						//剩余量
	TCHAR							szAwardImg[128];			//图片地址
	TCHAR							szAwardname[32];			//图片地址
	DWORD							dwType;						//商品类型:1为可自动发放的，默认为0	
};

//兑换个人信息请求结果
struct DBR_GP_UserExchangeProductResult
{
	vector<DBR_GP_ExchangeProduct>	vecProducts;					//产品列表
	bool							bOperateSuccess;				//成功标识
};

//兑换
struct DBR_GP_UserExchange
{
	DWORD							dwUserID;						//玩家 ID
	DWORD							dwAwardID;						//产品 ID
};

//兑换个人信息请求结果
struct DBR_GP_UserExchangeResult
{
	DWORD							dwGold;							//金豆
	DWORD							dwAwardID;						//产品 ID
	bool							bOperateSuccess;				//成功标识
	DWORD							dwScore;						//当前欢乐豆数量	
};

//兑换产品请求
struct DBR_GP_UserExchangeRecord
{
	DWORD							dwUserID;						//玩家 ID
	int								nPage;							//页码
};

//兑换记录
struct DBR_GP_ExchangeRecord
{
	DWORD							dwTotal;						//总计
	DWORD							dwID;							//ID
	DWORD							dwAwardID;						//产品 ID
	//DWORD							dwDate;							//兑换时间
	TCHAR							szStatus[32];					//状态
	TCHAR							szAwardName[32];				//名称
	TCHAR							szDate[32];						//兑换时间
};

//兑换个人信息请求结果
struct DBR_GP_UserExchangeRecordResult
{
	vector<DBR_GP_ExchangeRecord>	vecRecords;						//记录列表
	bool							bOperateSuccess;				//成功标识
};


//下载成功
struct DBR_GP_DownloadFaceSuccess
{
	DWORD							dwToltalSize;						//总共大小
	DWORD							dwCurrentSize;						//当前大小
	DWORD							dwUserID;							//玩家 ID
	BYTE							bFaceData[2048];					//头像数据
};

//删除消息
struct DBR_GP_CustomFaceDelete
{
	DWORD							dwUserID;							//玩家 ID
	DWORD							dwFaceVer;							//头像版本
};

//个人资料
struct DBR_GP_ModifyIndividual
{
	TCHAR							szAccount[NAME_LEN];				//玩家昵称
	TCHAR							szNickname[NAME_LEN];				//玩家昵称
	int								nGender;							//玩家性别
	TCHAR							szPhone[NAME_LEN];					//玩家电话
};

//修改结果
struct DBR_GP_ModifyIndividualResult
{
	TCHAR							szDescribeMsg[128];					//描述信息
	TCHAR							szAccount[NAME_LEN];				//玩家名称
	int								iCode;								//返回码。 0为成功
};

//个性签名
struct DBR_GP_ModifyUnderWrite
{
	unsigned int	dwUserID;		//玩家ID
	TCHAR			szDescribe[UNDER_WRITE_LEN];				//个性签名
};

//修改结果
struct DBR_GP_ModifyUnderWriteResult
{
	TCHAR							szDescribeMsg[128];					//描述信息
	TCHAR							szAccount[NAME_LEN];				//玩家名称
	int								iCode;								//返回码。 0为成功
};

//手机号
struct DBR_GP_ModifyPhone
{
	unsigned int	dwUserID;		//玩家ID
	TCHAR			szPhone[31];	//手机号
};

//修改结果
struct DBR_GP_ModifyPhoneResult
{
	TCHAR							szDescribeMsg[128];					//描述信息
	TCHAR							szAccount[NAME_LEN];				//玩家名称
	TCHAR							szPhone[31];				        //手机号
	int								iCode;								//返回码。 0为成功
};

//名称校验
struct DBR_GP_CheckID
{
	TCHAR							szAccount[NAME_LEN];				//玩家名称
};

//昵称校验
struct DBR_GP_CheckNickName
{
	TCHAR							szNickName[NAME_LEN];				//玩家昵称
};

//注册
struct DBR_GP_Register
{
	TCHAR							szAccount[NAME_LEN];				//玩家名称
	TCHAR							szNickName[NAME_LEN];				//玩家昵称
	TCHAR							szPhone[NAME_LEN];					//玩家手机
	TCHAR							szPassword[PASS_LEN];				//密码
	int								nGender;							//玩家性别
	DWORD							dwClientIP;							//连接地址
	TCHAR							szComputerID[COMPUTER_ID_LEN];		//机器序列
	TCHAR							szChannel[NAME_LEN];		//渠道号
};

//升级
struct DBR_GP_IDUpdate
{
	TCHAR							szAccount[NAME_LEN];				//玩家名称
	TCHAR							szAccountOld[NAME_LEN];				//玩家名称
	TCHAR							szNickName[NAME_LEN];				//玩家昵称
	TCHAR							szPassword[PASS_LEN];				//密码
	int								nGender;							//玩家性别
};

//设置密码
struct DBR_GP_ResetPwd
{
	TCHAR							szAccount[NAME_LEN];				//玩家名称
	TCHAR							szPassword[PASS_LEN];				//密码
};

//查询用户信息
struct DBR_GP_UserInfo
{
	TCHAR							szAccount[NAME_LEN];				//玩家名称
};
struct DBR_GP_UserInfo_Result
{
	TCHAR							szAccount[NAME_LEN];				//玩家名称
	TCHAR							szNickName[NAME_LEN];				//玩家昵称
	TCHAR							szPhone[NAME_LEN];					//玩家手机
	int								nGender;							//玩家性别
	TCHAR							szDescribeMsg[128];					//描述信息
	int								iCode;								//返回码。 0为成功
};

//通用结果
struct DBR_GP_CommonResult
{
	TCHAR						szDescribeMsg[128];		//描述信息
	TCHAR						szAccount[NAME_LEN];				//玩家名称
	int							iCode;					//返回码。 0为成功
	DWORD						dwUserID;							//玩家 ID
};

//用户能否领取推广奖励
struct DBR_GP_UserCanSpread
{
	DWORD							dwUserID;							//玩家 ID
	TCHAR							szComputerID[COMPUTER_ID_LEN];		//机器序列
};

//用户领取推广奖励
struct DBR_GP_UserGetSpread
{
	DWORD							dwUserID;							//玩家 ID
	DWORD							dwSpreaderID;						//推广员 ID
	TCHAR							szComputerID[COMPUTER_ID_LEN];		//机器序列
};
//用户领取推广奖励结果
struct DBR_GP_UserGetSpreadResult
{
	TCHAR						szDescribeMsg[128];		//描述信息
	int							iCode;					//返回码。 0为成功
	DWORD						dwUserID;				//玩家 ID
	DWORD						dwSpreaderID;				//推广员 ID
	DWORD						dwAmount;				//数量
	DWORD						dwType;				//类型
};

//推广员信息
struct DBR_GP_UserSpreaderInfo
{
	DWORD							dwSpreaderID;						//推广员 ID
};
//推广员信息结果
struct DBR_GP_UserSpreaderInfoResult
{
	TCHAR						szDescribeMsg[128];		//描述信息
	int							iCode;					//返回码。 0为成功
	DWORD						dwSpreaderID;			//推广员 ID
	DWORD						dwAmount;				//总数量
	DWORD						dwUsers;					//总人数
	DWORD						dwNum;					//返回昵称人数
	TCHAR						vecNickNames[10][NAME_LEN];	//返回昵称  --最多10个
	short						iAward[10];					//每人的奖励数
};


//查询弹窗公告信息
struct DBR_GP_UserGetBbsPop
{
	TCHAR							szChannel[NAME_LEN];		//渠道号
};
//弹窗公告信息结果
struct DBR_GP_UserGetBbsPopResult
{
	TCHAR						szDetails[512];			//弹窗公告详细内容
	int							iAction;				//动作码
	int							iCode;					//返回码。 0为成功
	int							iScrollCount;			//滚动公告的条数
	int							iListCount;				//列表公告的条数
	DWORD						dwListID[3];			//列表公告的ID
};

//查询滚动公告信息
struct DBR_GP_UserGetBbsScroll
{
	TCHAR							szChannel[NAME_LEN];		//渠道号
};
//滚动公告信息结果
struct DBR_GP_UserGetBbsScrollResult
{
	TCHAR						szDetails[10][100];		//滚动公告详细内容
	int							iCount;					//条数
	int							iCode;					//返回码。 0为成功
};

//查询列表公告信息
struct DBR_GP_UserGetBbsList
{
	TCHAR							szChannel[NAME_LEN];		//渠道号
};
//列表公告信息结果
struct DBR_GP_UserGetBbsListResult
{
	TCHAR						szTitle[3][100];		//列表公告标题
	TCHAR						szDetails[3][512];		//列表公告详细内容
	TCHAR						szDate[3][64];				//列表公告时间
	int							iID[3];					//列表公告ID
	int							iType[3];				//列表公告类型
	int							iCount;					//条数
	int							iCode;					//返回码。 0为成功
};

//查询玩家的任务统计信息
struct DBR_GP_UserGetTaskInfo
{
	DWORD							dwUserID;							//玩家 ID
};
//玩家的任务统计信息结果
struct DBR_GP_UserGetTaskInfoResult
{
	DWORD						dwUserID;				//玩家 ID
	int							iWinToday3;				//今天赢的盘数
	int							iLostToday3;			//今天输的盘数
	int							iWinTotal3;				//赢的总盘数
	int							iLostTotal3;			//输的总盘数
	int							iAwardToday3;			//每日任务的领奖阶段数
	int							iAwardTotal3;			//系统任务的领奖阶段数
	int							iWinToday4;				//今天赢的盘数
	int							iLostToday4;			//今天输的盘数
	int							iWinTotal4;				//赢的总盘数
	int							iLostTotal4;			//输的总盘数
	int							iAwardToday4;			//每日任务的领奖阶段数
	int							iAwardTotal4;			//系统任务的领奖阶段数
	int							iCode;					//返回码。 0为成功
};

//查询玩家的排行榜信息
struct DBR_GP_UserGetRanking
{
	DWORD						dwUserID;			//玩家 ID
	int							nPage;				//页数
};
//排行榜记录
struct DBR_GP_UserRankingRecord
{
	DWORD							dwUserID;						//ID
	SHORT							nIndex;							//名称
	LONG							lGoldScore;						//财富
	BYTE							cbGender;						//用户性别
	TCHAR							szNickName[32];					//名称
};

//排行榜记录
struct DBR_GP_UserRankingRecord_V2
{
	DWORD							dwUserID;						//ID
	SHORT							nIndex;							//名称
	LONG							lGoldScore;						//财富
	BYTE							cbGender;						//用户性别
	TCHAR							szNickName[32];					//名称
	TCHAR							szDescribe[128];
};

//玩家的排行榜信息结果
struct DBR_GP_UserGetRankingResult
{
	vector<DBR_GP_UserRankingRecord>	vecRecords;						//记录列表
	DWORD							dwTotal;				//总计
	DWORD							dwUserID;				//我的 ID
	SHORT							nMyIndex;				//我的名次
	TCHAR							szDate[64];				//榜单生成时间
	int								iCode;					//返回码。 0为成功
};

//玩家的排行榜信息结果
struct DBR_GP_UserGetRankingResult_V2
{
	vector<DBR_GP_UserRankingRecord_V2>	vecRecords;						//记录列表
	DWORD							dwTotal;				//总计
	DWORD							dwUserID;				//我的 ID
	SHORT							nMyIndex;				//我的名次
	TCHAR							szDate[64];				//榜单生成时间
	int								iCode;					//返回码。 0为成功
};

//玩家排行榜状态信息结果
struct DBR_GP_UserGetRankingStatusResult
{
	unsigned int		dwUserID;					//玩家 ID
	char				isInRanking;				//玩家是否入榜。为1时参加排名，为0时不参加排名
};

//修改玩家是否入排行榜
struct DBR_GP_UserModifyRankingStatus
{
	unsigned int		dwUserID;					//玩家 ID
	char				isInRanking;				//玩家是否入榜。为1时参加排名，为0时不参加排名
};

//修改玩家是否入排行榜返回结果
struct DBR_GP_UserModifyRankingStatusResult
{
	DWORD				dwUserID;			//玩家 ID
	int					iCode;				//返回码。 0为成功
};

//玩家反馈问题
struct DBR_GP_UserFeedBack
{
	DWORD				dwUserID;			//玩家 ID
	TCHAR				szPicture[128];		//图片地址
	TCHAR				szContent[372];		//意见内容
};
//玩家反馈问题结果
struct DBR_GP_UserFeedBackResult
{
	int							iCode;				//返回码。 0为成功
	TCHAR						szDetails[128];		//滚动公告详细内容
};
//查询玩家反馈问题的记录
struct DBR_GP_UserGetFeedBack
{
	DWORD						dwUserID;			//玩家 ID
	int							nPage;				//页数
};
//反馈问题的记录
struct DBR_GP_UserFeedBackRecord
{
	TCHAR				szQuestion[512];	//问题描述
	TCHAR				szAnswer[256];		//运维回复
	TCHAR				szDate[32];			//问题提交时间
	BYTE				cbHavePic;			//是否带图片
	TCHAR				szPicture[128];		//图片地址
};
//玩家的排行榜信息结果
struct DBR_GP_UserGetFeedBackResult
{
	vector<DBR_GP_UserFeedBackRecord>	vecRecords;			//记录列表
	DWORD							dwTotal;				//总计
	DWORD							dwUserID;			//玩家 ID
	int								iCode;					//返回码。 0为成功
};
//查询用户身份证账号信息
struct DBR_GP_UserInfoIDCard
{
	DWORD	dwUserID;			//玩家 ID
};
//请求提交身份证信息(姓名及身份证号)
struct DBR_GP_UserAuthentication
{
	DWORD	dwUserID;				//玩家 ID
	TCHAR	szName[NAME_LEN];		//问题描述
	TCHAR	szIdCardNum[NAME_LEN];	//运维回复
};

//商城商品请求
struct DBR_GP_UserMallProduct
{
	DWORD	dwUserID;				//玩家 ID
	DWORD	dwVersion;				//版本
};

//商城商品信息
struct DBR_GP_UserMallProductResult
{
	unsigned int					version;			//版本号
	unsigned int					productID;			//商品 ID
	unsigned int					productPrice;			//价格
	char							cbHotFlag;			//热销标识
	char							szProductName[32];	//商品名称
	char							szProductNameAdd[32];	//商品名称
	char							szProductImage[128];	//商品图片
};

//商城商品请求结果
struct DBR_GP_UserMallProductResultFinish
{
	vector<DBR_GP_UserMallProductResult>	vecProducts;					//产品列表
	bool							bOperateSuccess;				//成功标识
};

struct DBR_Cancel_Order_Num
{
	char orderNum[20];
};

struct DBR_Cancel_Order_Resut
{
	short iCode;
};


//下订单请求
struct DBR_GP_UserMallPlaceOrder
{
	unsigned int					dwUserID;		//玩家 ID
	unsigned int					dwProductID;	//商品 ID
	char							szPayWay[10];	// 支付方式
};
//下订单请求结果
struct DBR_GP_UserMallPlaceOrderResult
{
	short nResult;		//结果
	char dwOrderID[20];	//订单ID
	char token[128];	//token信息
	char productName[32]; //商品名称
	int productPrice; //商品价格
	char billingCode[20]; // 计费码
};
//请求购买结果
struct DBR_GP_UserMallBuy
{		
	char dwOrderID[20];	//订单ID
};
//购买结果
struct DBR_GP_UserMallBuyResult
{
	unsigned int	dwUserID;			//玩家 ID
	int				productID;
	unsigned int	dwGoldScore;	//用户财富
	int				iCode;				//返回码。 0为成功
};
//请求更新购买
struct DBR_GP_UserMallUpdate
{		
	char dwOrderID[20];	//订单ID
};
//更新购买结果
struct DBR_GP_UserMallUpdateResult
{
	unsigned int	dwUserID;			//玩家 ID
	int				productID;
	unsigned int	dwGoldScore;	//用户财富
	int				iCode;				//返回码。 0为成功
};


//查询玩家银行信息请求
struct DBR_GP_UserBankInfo
{
	unsigned int					dwUserID;		//玩家 ID
};
//查询玩家银行信息结果
struct DBR_GP_UserBankInfoResult
{
	short							nResult;		//结果	
	unsigned int					dwUserID;		//玩家 ID
	unsigned int					dwCurScore;		//当前带的游戏分数
	unsigned int					dwBankScore;	//银行分数
};
//玩家操作银行请求
struct DBR_GP_UserBankCharge
{
	short							nOpCode;		//操作码：1为存，2为取	
	unsigned int					dwUserID;		//玩家 ID
	unsigned int					dwCurScore;		//当前带的游戏分数
	unsigned int					dwBankScore;	//银行分数
};
//玩家操作银行结果
struct DBR_GP_UserBankChargeResult
{
	short							nResult;		//结果	
	unsigned int					dwUserID;		//玩家 ID
	unsigned int					dwCurScore;		//当前带的游戏分数
	unsigned int					dwBankScore;	//银行分数
};

//活动列表请求
struct DBR_GP_UserActivityListResult
{
	TCHAR						szTitle[5][50];			//标题
	TCHAR						szDetails[5][100];		//内容
	TCHAR						szLinkUrl[5][50];		//链接地址
	TCHAR						szIconUrl[5][50];		//图标地址
	int							iID[5];					//列表公告ID
	int							iStatus[5];				//状态
	int							iType[5];				//类型
	int							iCount;					//条数
	int							iCode;					//返回码。 0为成功
};

//查询指定活动信息
struct DBR_GP_UserActivity
{
	unsigned int					dwUserID;		//玩家 ID
	unsigned int					dwActivityID;	//活动ID
};
//查询指定活动信息结果
struct DBR_GP_UserActivityResult
{
	int								iCode;			//返回码。 0为成功
	unsigned int					dwUserID;		//玩家 ID
	unsigned int					dwActivityID;	//活动ID
	unsigned int					wCount;			//走马灯条数，最多5条
	TCHAR							szText[5][100];	//走马灯内容
	TCHAR							szResourceUrl[50];	//资源下载地址
	TCHAR							szParam[128];	//参数
};

//抽奖
struct DBR_GP_UserActivityLucky
{
	unsigned int					dwUserID;		//玩家 ID
	unsigned int					dwActivityID;	//活动ID
};
//抽奖结果
struct DBR_GP_UserActivityLuckyResult
{
	int								iCode;			//返回码。 0为成功
	unsigned int					dwUserID;		//玩家 ID
	unsigned int					dwActivityID;	//活动ID
	unsigned int					dwPrizeID;		//奖品id
	unsigned int					dwPrice;		//奖品价值
	TCHAR							szImageUrl[50];	//奖品图片
	TCHAR							szPrizeName[50];//奖品名称
};

//中奖记录请求
struct DBR_GP_UserActivityRecord
{
	DWORD							dwUserID;						//玩家 ID
	DWORD							dwActivityID;					//活动 ID
	int								nPage;							//页码
};

//中奖记录
struct DBR_GP_ActivityRecord
{
	DWORD							dwTotal;						//总计
	DWORD							dwID;							//ID
	DWORD							dwAwardID;						//产品 ID
	//DWORD							dwDate;							//兑换时间
	TCHAR							szStatus[32];					//状态
	TCHAR							szAwardName[32];				//名称
	TCHAR							szDate[32];						//兑换时间
};

//中奖个人信息请求结果
struct DBR_GP_UserActivityRecordResult
{
	vector<DBR_GP_ActivityRecord>	vecRecords;						//记录列表
	bool							bOperateSuccess;				//成功标识
	DWORD							dwUserID;						//玩家 ID
	DWORD							dwActivityID;					//活动 ID
};

//发喇叭请求
struct DBR_GP_UserSpeakerSend
{
	short							iType;					//喇叭类型：1用户消息，2系统消息
	DWORD							dwUserID;				//玩家 ID
	DWORD							nLen;					
	TCHAR							szDate[256];					//喇叭内容
};

//发喇叭请求结果
struct DBR_GP_UserSpeakerSendResult
{
	short							iCode;					//返回码。 0为成功
	short							iType;					//喇叭类型：1用户消息，2系统消息
	unsigned int					dwScore;				//扣完款后，身上的财富数
	DWORD							dwUserID;				//玩家 ID
	DWORD							nLen;					
	TCHAR							szDate[256];				//喇叭内容
};

//查询配置项请求
struct DBR_GP_UserQueryConfig
{
	DWORD							dwUserID;				//玩家 ID
	DWORD							dwVersion;					
	DWORD							dwConfigID;		
	TCHAR							szChannel[NAME_LEN];		//渠道号		
};

//查询配置项结果
struct DBR_GP_UserQueryConfigResult
{
	short							iCode;
	DWORD							dwUserID;				//玩家 ID
	DWORD							dwVersion;					
	DWORD							dwLen;	
	TCHAR							szDate[256];			//内容
};

//////////////////////////////////////////////////////////////////////////

#endif
