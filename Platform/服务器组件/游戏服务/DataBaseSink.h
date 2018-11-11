#ifndef DATA_BASE_SINK_HEAD_FILE
#define DATA_BASE_SINK_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "GameServiceExport.h"
#include "cnd_ILogMgr.h"

//////////////////////////////////////////////////////////////////////////
//结构定义

//数据库参数
struct tagDataBaseSinkParameter
{
	tagDataBaseInfo						* pGameUserDBInfo;				//连接信息
	tagDataBaseInfo						* pGameScoreDBInfo;				//连接信息
	tagGameServiceAttrib				* pGameServiceAttrib;			//服务属性
	tagGameServiceOption				* pGameServiceOption;			//配置信息
};

//////////////////////////////////////////////////////////////////////////
//数据库请求数据包

//数据库请求标识
#define	DBR_GR_LOGON_BY_USERID			1								//I D 登录
#define DBR_GR_WRITE_GAME_SCORE			2								//离开房间
#define DBR_GR_LEAVE_GAME_SERVER		3								//修改积分
#define DBR_GR_GAME_SCORE_RECORD		4								//积分记录
#define DBR_GR_LIMIT_ACCOUNTS			6								//禁用帐户
#define DBR_GR_SET_USER_RIGHT			7								//设置权限
#define DBR_GR_WRITE_PROPERTY			11								//写入结果

#define DBR_GR_BANK_GET_GOLD			13								//提取金币
#define DBR_GR_BANK_STORAGE_GOLD		14								//存储金币
#define DBR_GP_READ_GRADE_INFO          15                              //用户等级 彭文添加

//#define DBR_GR_USER_SIGN_UP				12							//玩家报名

//数据库输出标识
#define DBR_GR_LOGON_SUCCESS			100								//登录成功
#define DBR_GR_LOGON_ERROR				101								//登录失败
//#define DBR_GR_ANDROID_USER			102								//机器信息
#define DBR_GR_USER_RIGHT_RESULT		103								//权限结果

//#define DBR_GR_PROPERTY_ATTRIBUTE		106								//加载成功
//#define DBR_GR_USER_PROPERTY			107								//玩家道具
//#define DBR_GR_EXCHANGE_RESULT		108								//兑换结果
//#define DBR_GR_LOAD_PROP_FINISHI		109								//完成加载
//#define DBR_GR_FLOWER_ATTRIBUTE		110								//完成加载
//#define DBR_GR_SEND_GIFT_RESULT		103								//赠送结果

//彭文添加      
#define  DBR_GR_UNLOCK_USER             18								//用户解锁
//#define  DBR_GR_USER_UPDATE_PROP      19								//用户请求更新道具

#define  DBR_GR_LOAD_AFFICHE            20								//读取公告 
#define  DBR_GR_LOAD_HAPPY_MESS         21								//读取喜报

#define  DBR_GR_HAPPY_MESS_ONE          113								//喜报加载
#define  DBR_GR_HAPPY_MESS_SYNC         114								//喜报同步

#define  DBR_GR_AFFICHE_ONE             115								//公告加载
#define  DBR_GR_AFFICHE_SYNC            116								//公告同步

//#define  DBR_GR_USER_UPDATE_PROP_RESULT 117							//更行道具的结果 

#define  DBR_GR_USER_FIND_GEMS_TASK_LOAD			  120				//任务加载
#define  DBR_GR_USER_FIND_GEMS_TASK_PERFORM_FINISH    122               //任务完成
//任务完成
//#define  DBR_GR_USER_TASK_COUNT					  123				//任务计数
//#define  DBR_GR_USER_TASK_NEW						  124				//新任务
//#define  DBR_GR_RE_USER_TASK_COUNT				  125				//任务调度时候，任务计数
#define  DBR_GR_RE_FIND_GEMS_TASK_PERFORM_FINISH	  200				//任务完成 返回到任务调度
#define DBR_GR_CONTINUE_CONNECT			201								//保持连接数据库		Author:<cxf>
#define DBR_GR_ONLINE_GIFT				202								//在线赠送

typedef		CMD_GF_TASK_tagTask				DBR_FIND_GEMS_TASK;


struct DBR_TASK_Finish
{
	DWORD                               dwKindID;                       //游戏类型id;  m_pGameServiceOption->wKindID
	DWORD                               dwUserID;                       //用户id      
	DWORD           					dwGems;       					//奖励的宝石
	int                                 iTaskID;                        //任务id
	int									iTaskUserLogID;					//任务记录id
	TCHAR								strUserLogTable[24];			//任务的表名	
	bool                                bFinish;                        //是否完成

	DWORD                               dwServerID;               //房间的id
	TCHAR                               szClientIP[24];           //ip地址
	DWORD                               dwSortID;                 //序号id
};



//公告结构题
struct DBR_GR_affiche
{
	//DWORD dwGameID;    //游戏类型名字
	DWORD dwID;        //序号
	TCHAR szInfo[256]; //公告信息
};

//喜报结构
typedef DBR_GR_affiche DBR_GR_happyMess; 


//用户请求更新道具的结构
struct DBR_GR_User_Update_Prop
{
	DWORD dwUserID; //用户id
	DWORD propID;   //道具id
};


//数据库返回的结果
struct DBR_GR_User_Update_Result
{
	DWORD dwUserID; //用户id
	DWORD propID;   //道具id
	DWORD curCount; //当前数量
};

//添加结束



//赠送鲜花
struct DBR_GR_SendGift
{
	DWORD								dwSendUserID;					//赠送者ID
	DWORD								dwRcvUserID;					//接受者ID
	WORD								wGiftID;						//礼物	ID
	DWORD								dwClientIP;						//玩家地址
	WORD								wFlowerCount;					//鲜花数目
	LONG								lSendUserCharm;					//赠送魅力
	LONG								lRcvUserCharm;					//接受魅力
	LONG								lFlowerPrice;					//赠送付费
};

//道具结构
struct DBR_GR_Property
{
	DWORD								dwUserID;						//玩家ID
	DWORD								dwTargetUserID;					//目标玩家
	int									nPropertyID;					//道具ID
	DWORD								dwCurCount;						//当前数目
	DWORD								dwPachurseCount;				//购买数目
	DWORD								dwOnceCount;					//单次数目
	LONG								lPrice;							//道具价格
	DWORD								dwClientIP;						//玩家IP
};

//道具记录
struct DBR_GR_WriteProperty
{
	DWORD								dwUserID;						//玩家ID
	int									nPropertyID;					//道具ID
	DWORD								lUseableTime;					//可用时间
	DWORD								dwClientIP;						//连接地址

	//彭文添加
	DWORD                               dwLeaveTime;                        //剩下的时间     单位为秒
	bool                                bUse;                               //是否这个类型的道具使用中
	//添加结束
};

//赠送结果
struct DBR_GR_SendGiftResult
{
	DWORD								dwSendUserID;					//赠送者ID
	DWORD								dwRcvUserID;					//接受者ID
	WORD								wGiftID;						//礼物	ID
	bool								bSuccess;						//成功标识
};

//提取金币
struct DBR_GR_BankDrawoutGold
{
	LONGLONG							DrawoutGoldCount;						//提取数目
	DWORD								dwUserID;						//玩家ID
	DWORD								dwClientIP;						//玩家IP
};

//存储金币
struct DBR_GR_BankStorage
{
	LONGLONG							lStorageCount;					//存储数目
	DWORD								dwUserID;						//玩家ID
	DWORD								dwClientIP;						//玩家IP
};

//ID 登录
struct DBR_GR_LogonByUserID
{
	DWORD								dwUserID;						//用户 I D
	DWORD								dwClientIP;						//连接地址
	TCHAR								szPassWord[PASS_LEN];			//登录密码
	TCHAR								szComputerID[COMPUTER_ID_LEN];	//机器序列
	DWORD                               dwKindID;                       //游戏的id 彭文增加
};

//禁用帐户
struct DBR_GR_LimitAccounts
{
	DWORD								dwUserID;						//用户 I D
	DWORD								dwMasterUserID;					//管理 I D
	DWORD								dwMasterClientIP;				//连接地址
};

//设置权限
struct DBR_GR_SetUserRight
{
	//权限范围
	BYTE								cbGame;							//设置游戏
	BYTE								cbAccounts;						//设置帐号

	//权限信息
	DWORD								dwUserID;						//用户 I D
	DWORD								dwUserRight;					//用户权限
	DWORD								dwMasterUserID;					//管理 I D
	DWORD								dwMasterClientIP;				//连接地址
};

//权限结果
struct DBR_GR_UserRightResult
{
	//权限范围
	bool								bGameSuccess;					//成功标识
	bool								bAccountsSuccess;				//成功标识

	//权限信息
	DWORD								dwUserID;						//用户 I D
	DWORD								dwMasterUserID;					//管理 I D
	DWORD								dwUserRight;					//用户权限

};

//机器用户
struct DBO_GR_AndroidUser
{
	LONG								lResultCode;					//结果代码
	WORD								wAndroidCount;					//用户数目
	DWORD								dwAndroidID[MAX_ANDROID];		//用户 I D
};

//登录成功
struct DBR_GR_LogonSuccess
{
	//基本信息
	WORD								wFaceID;						//头像索引
	DWORD								dwCustomFaceVer;				//自定头像
	DWORD								dwUserID;						//用户 I D
	DWORD								dwGameID;						//用户 I D
	DWORD								dwGroupID;						//社团索引
	DWORD								dwUserRight;					//用户等级
	LONG								lLoveliness;					//用户魅力
	DWORD								dwMasterRight;					//管理权限
	TCHAR								szAccounts[NAME_LEN];			//登录帐号
	TCHAR								szPassWord[PASS_LEN];			//登录密码
	TCHAR								szGroupName[GROUP_LEN];			//社团名字
	TCHAR								szUnderWrite[UNDER_WRITE_LEN];	//个性签名
	TCHAR								szBankPassword[PASS_LEN];		//银行密码

	//用户属性
	BYTE								cbGender;						//用户性别
	BYTE								cbMemberOrder;					//会员等级
	BYTE								cbMasterOrder;					//管理等级

	//积分信息
	LONGLONG 							lMoney;					//存储金币
	LONG								lWinCount;						//胜利盘数
	LONG								lLostCount;						//失败盘数
	LONG								lDrawCount;						//和局盘数
	LONG								lFleeCount;						//断线数目
	LONG								lExperience;					//用户经验

	//彭文添加
	LONGLONG                            lGold;    //用户的金币 2011-9-30前名字为lGems
	LONGLONG                            lGem;     //用户的 宝石  2011-9-30前名字为lHonor
	DWORD                               dwGrade;  //用户的等级（不超过256） 2011-9-30前为dwClass                           
	TCHAR                               szNickName[NAME_LEN]; //昵称
	USER_TYPE                           utIsAndroid; //是否是机器人
	LONG								lOnlineTime;					//一天在线时长	Author<cxf>
	LONG								wOnlineGiftCount;				//一天在线赠送次数
	//添加结束
	//=====Author<cxf>;DateTime<2012-03-02>
	LONG								lKindIDWinCount;				//某游戏胜利盘数
	LONG								lKindIDLostCount;				//某游戏失败盘数
	LONG								lKindIDDrawCount;				//某游戏和局盘数
	LONG								lKindIDFleeCount;				//某游戏断线数目
	//=====END
};

//登录失败
struct DBR_GR_LogonError
{
	LONG								lErrorCode;						//错误代码
	TCHAR								szErrorDescribe[128];			//错误消息
};

//修改积分
struct DBR_GR_WriteUserScore
{
	//用户属性
	DWORD								dwUserID;						//用户地址
	DWORD								dwClientIP;						//连接地址
	DWORD								dwPlayTimeCount;				//游戏时间
	DWORD								dwOnlineTimeCount;				//在线时间

	//积分信息
	LONGLONG							lRevenue;						//游戏税收
	tagUserScore						ScoreModifyInfo;				//修改积分
};

//离开房间
struct DBR_GR_LeaveGameServer
{
	//用户属性
	DWORD								dwUserID;						//用户地址
	DWORD								dwClientIP;						//连接地址
	DWORD								dwPlayTimeCount;				//游戏时间
	DWORD								dwOnlineTimeCount;				//在线时间
	LONG								lLoveliness;					//用户魅力

	//积分信息
	LONGLONG							lRevenue;						//游戏税收
	tagUserScore						ScoreModifyInfo;				//修改积分
};

//道具属性
struct DBR_GR_PropertyAttribute
{
	WORD							wPropertyID;						//道具ID
	DWORD							dwPropCount1;						//道具数目
	DWORD							dwPropCount2;						//道具数目
	DWORD							dwPropCount3;						//道具数目
	DWORD							dwPropCount4;						//道具数目
	DWORD							dwPropCount5;						//道具数目
	DWORD							dwPropCount6;						//道具数目
	DWORD							dwPropCount7;						//道具数目
	DWORD							dwPropCount8;						//道具数目
	DWORD							dwPropCount9;						//道具数目
	DWORD							dwPropCount10;						//道具数目
	LONG							lPrice1;							//道具价格
	LONG							lPrice2;							//道具价格
	LONG							lPrice3;							//道具价格
	LONG							lPrice4;							//道具价格
	LONG							lPrice5;							//道具价格
	LONG							lPrice6;							//道具价格
	LONG							lPrice7;							//道具价格
	LONG							lPrice8;							//道具价格
	LONG							lPrice9;							//道具价格
	LONG							lPrice10;							//道具价格
	BYTE							cbDiscount;							//会员折扣
	//彭文添加
	DWORD                           dwLastingTime;                      //道具持续的时间
	//添加结束

};

//鲜花属性
struct DBR_GR_FlowerAttribute
{
	int							nFlowerID;							//鲜花ID
	LONG						lPrice;								//鲜花价格
	LONG						lSendUserCharm;						//赠送魅力
	LONG						lRcvUserCharm;						//接受魅力
	BYTE						cbDiscount;							//会员折扣
};


//兑换结构
struct DBR_GR_ExchangeLoveliness
{
	DWORD								dwUserID;						//玩家ID
	LONG								lLoveliness;					//魅力数值
	DWORD								lGoldValue;						//金币数值
	DWORD								dwClientIP;						//玩家IP
};


//玩家道具
struct DBR_GR_UserProperty
{
	DWORD								dwUserID;						//玩家ID
	INT									nPropertyID;					//道具ID
	DWORD								dwCurCount;						//当前数目
	//彭文添加
	bool                                bUse;                           //这类道具是否在使用状态
    DWORD                               dwLeaveTime;                    //剩下的时间
	//添加结束
};



//////////////////////////////////////////////////////////////////////////

//数据库引擎钩子
class CDataBaseSink : public IDataBaseEngineSink
{
public:
	// add by HouGuoJiang 2011-11-25
	cnd_ILogMgr*	m_pRoomLog;	// 房间日志

	//友元定义
	friend class CGameService;

	//配置变量
protected:
	tagDataBaseInfo						* m_pGameUserDBInfo;			//连接信息
	tagDataBaseInfo						* m_pGameScoreDBInfo;			//连接信息
	tagGameServiceAttrib				* m_pGameServiceAttrib;			//服务属性
	tagGameServiceOption				* m_pGameServiceOption;			//配置信息
	IDataBaseEngineEvent *				m_pIDataBaseEngineEvent;		//数据事件

	//组件变量
protected:
	CDataBaseAide						m_AccountsDBAide;				//用户数据库
	CDataBaseAide						m_GameScoreDBAide;				//积分数据库
	CDataBaseHelper						m_AccountsDBModule;				//用户数据库
	CDataBaseHelper						m_GameScoreDBModule;			//积分数据库

	//函数定义
public:
	//构造函数
	CDataBaseSink();
	//析构函数
	virtual ~CDataBaseSink();

	//基础接口
public:
	//释放对象
	virtual void __cdecl Release() { delete this; }
	//接口查询
	virtual void * __cdecl QueryInterface(const IID & Guid, DWORD dwQueryVer);

	//系统事件
public:
	//启动事件
	virtual bool __cdecl OnDataBaseEngineStart(IUnknownEx * pIUnknownEx);
	//停止事件
	virtual bool __cdecl OnDataBaseEngineStop(IUnknownEx * pIUnknownEx);

	//内核事件
public:
	//时间事件
	virtual bool __cdecl OnDataBaseEngineTimer(DWORD dwTimerID, WPARAM dwBindParameter) { return true; }
	//控制事件
	virtual bool __cdecl OnDataBaseEngineControl(WORD wControlID, VOID * pData, WORD wDataSize) { return true; }
	//请求事件
	virtual bool __cdecl OnDataBaseEngineRequest(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize);

	//处理函数
private:
	//登录请求
	bool OnRequestLogon(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//写分请求
	bool OnRequestWriteUserScore(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//离开房间
	bool OnRequestLeaveGameServer(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//禁用帐户
	bool OnRequestLimitAccounts(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//设置权限
	bool OnRequestSetUserRight(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//加载用户
	//bool OnRequestLoadAndroid(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//赠送鲜花
	//bool OnRequestSendGift(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//道具消息
	//bool OnRequestProperty(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//写入道具
	//bool OnWriteProperty(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//加载道具
	//bool OnLoadProperty(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//魅力兑换
	//bool OnExchangeCharm(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//提取金币
	bool OnBankDrawoutGold(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//存储金币
	bool OnBankStorageGold(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//Author:<cxf>; CreateDate:<2011-12-22>; Description:<保持数据库连接池一直连接>
	bool ContinueConnect();
	//新手赠送//新手赠送<2012-02-07>
	bool NewGiftUser(VOID * pData, WORD wDataSize);


	//存储过程
protected:
	//彭文添加
	//读取寻宝任务
	void LoadFindGamesTask( DWORD userID, DWORD kindID);
	//得到寻宝任务
	LONG SPGetLoadFindGemsTask(DWORD dwUserID, DWORD kindID);
	//任务执行结束
	bool OnFindGemsTaskPerformFinish(DWORD dwContextID, VOID * pData, WORD wDataSize);
    //任务执行计数
	//bool OnTaskCount(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//添加结束

	//I D 存储过程
	LONG SPLogonByUserID(DWORD dwUserID, LPCTSTR pszPassword, DWORD dwClientIP, LPCTSTR pszComputerID);
	//写分存储过程
	LONG SPWriteUserScore(DWORD dwUserID, DWORD dwPlayTimeCount, LONGLONG lRevenue, DWORD dwClientIP, tagUserScore & UserScore);
	//离开存储过程
	LONG SPLeaveGameServer(DWORD dwUserID, DWORD dwPlayTimeCount, DWORD dwOnLineTimeCount, DWORD dwClientIP, LONGLONG lRevenue, LONG lLoveliness);
	//禁号存储过程
	LONG SPCongealAccounts(DWORD dwUserID, DWORD dwMasterUserID, DWORD dwClientIP);
	//权限存储过程
	LONG SPSetUserGameRight(DWORD dwUserID, DWORD dwUserRight, DWORD dwMasterUserID, DWORD dwClientIP);
	//权限存储过程
	LONG SPSetUserAccountsRight(DWORD dwUserID, DWORD dwUserRight, DWORD dwMasterUserID, DWORD dwClientIP);
	//记录存储过程
	LONG SPRecordGiftGrant(DWORD dwSendUserID, DWORD dwRcvUserID, WORD wGiftID, DWORD dwClientIP, DWORD dwSendUserLoveliness, DWORD dwRcvUserLoveliness, LONG lValue, int nFlowerCount);
	//购买存储过程
	LONG SPBuyProperty(DWORD dwUserID, DWORD dwTargetUserID, int nPropertyID, DWORD dwCurCount, DWORD dwOnceCount, DWORD dwPachurCount, LONG lPrice, DWORD dwClientIP);
	//写入存储过程
	LONG SPWriteProperty(DWORD dwUserID,   int  nPropertyID, DWORD dwUseableTime,
						 DWORD dwLeftTime, bool bUse,
						 DWORD dwClientIP);
	//输赢存储过程Author<cxf>;DateTime<2012-03-02>
	LONG SPDayWinLostCount(DWORD dwUserID);

	//加载存储过程
	//LONG SPLoadPropertyAttribute();
	//加载存储过程
	//LONG SPLoadFlowerAttribute();

	//兑换存储过程
	LONG SPExchangeCharm(DWORD dwUserID, LONG lLoveliness, LONG lGoldValue, DWORD dwClientIP);
	//提取存储过程
	LONG SPBankDrawoutGold(DWORD dwUserID, LONGLONG lSwapScore, DWORD dwClientIP);
	//提取存储过程
	LONG SPBankStorageGold(DWORD dwUserID, LONGLONG lSwapScore, DWORD dwClientIP);
    //处理公告
	bool OnAffiche(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//处理喜报
	bool OnHappyMess(DWORD dwContextID, VOID * pData, WORD wDataSize);
	bool OnReadGradeInfo(DWORD dwContextID, VOID * pData, WORD wDataSize);	

    //处理用户请求解锁
    bool OnUnLockUser(DWORD dwContextID, VOID * pData, WORD wDataSize);

	// 设置日志指针
	void SetRoomLog(cnd_ILogMgr*	pRoomLog);
};

//////////////////////////////////////////////////////////////////////////

#endif