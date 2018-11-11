#ifndef ATTEMPER_ENGINE_SINK_HEAD_FILE
#define ATTEMPER_ENGINE_SINK_HEAD_FILE

#pragma once
#include <vector>
#include "Stdafx.h"
#include "ServerList.h"
#include "DataBaseSink.h"
#include "GameServiceExport.h"
#include "ServerUserManager.h"
#include "AndroidUserManager.h"
#include "CAutoQueue.h"
#include "CBusyDeskStat.h"


#include "GlobalChannel.h"

using namespace std;   
//////////////////////////////////////////////////////////////////////////
//结构体定义

//调度参数
struct tagAttemperSinkParameter
{
	//配置变量
	DWORD							dwCenterServer;						//中心地址
	tagGameServiceAttrib			* pGameServiceAttrib;				//服务属性
	tagGameServiceOption			* pGameServiceOption;				//配置信息

	//组件变量
	IAttemperEngine					* pIAttemperEngine;					//调度引擎
	IGameServiceManager				* pIGameServiceManager;				//管理接口
};

//连接信息结构
struct tagConnectItemInfo
{
	DWORD							dwSocketID;							//网络标识
	DWORD							dwClientIP;							//连接地址
	IServerUserItem					* pIServerUserItem;					//用户指针
};

//桌子状况
struct tagTableUserStatus
{
	WORD							wLessUserCount;						//最少人数
	WORD							wTableUserCount;					//用户数目
	WORD							wTableAndroidCount;					//用户数目
};

//用户状况
struct tagAndroidUserStatus
{
	WORD							wFreeUserCount;						//用户数目
	WORD							wPlayUserCount;						//用户数目
	WORD							wSitdownUserCount;					//用户数目
	IServerUserItem *				pServerUserFree[MAX_ANDROID];		//用户接口
	IServerUserItem *				pServerUserPlay[MAX_ANDROID];		//用户接口
	IServerUserItem *				pServerUserSitdown[MAX_ANDROID];	//用户接口
};

//类型定义
typedef CArrayTemplate<tagPropertyInfo> CPropertyAttributeArrary;
typedef CArrayTemplate<tagFlowerInfo> CFlowerAttributeArrary;

//typedef std::vector<std::string> AfficheVec; 
//////////////////////////////////////////////////////////////////////////

//类说明
class CTableFrame;

//调度引擎钩子
class CAttemperEngineSink : public IAttemperEngineSink, public IGameServiceFrame
{
	// add by HouGuoJiang 2011-11-25
	cnd_ILogMgr*	m_pRoomLog;	// 房间日志
	
	//友元定义
	friend class CGameService;

	//变量定义
	//彭文添加
	typedef std::map<DWORD, time_t>            UserLeftDataSet;
	typedef std::map<DWORD, time_t>::iterator  UserLeftDataSetIT;
	UserLeftDataSet                 m_userLeftDataSet;                  //彭文添加 用户断线信息集合
	//添加结束
protected:
	WORD							m_wMaxUserItem;						//最大用户
	CTableFrame						* m_pTableFrame;					//桌子指针
	tagConnectItemInfo				* m_pNormalParameter;				//辅助信息
	tagConnectItemInfo				* m_pAndroidParameter;				//辅助信息
	CCAutoQueue              		m_UserAutoQueue;	                //彭文添加 用户排队机					//排队用户
	CCAutoQueue                     m_AndroidAutoQueue;                 //彭文添加 机器人排队机
	CCBusyDeskStat                  m_BusyDeskStat;                     //彭文添加 繁忙桌子统计

	//发送场所
protected:
	BYTE							m_cbSendLocation;					//发送场所

	//消息变量
protected:
	BYTE							m_cbRoomMsgIndex;					//消息索引			
	BYTE							m_cbGameMsgIndex;					//消息索引			

	//控制变量
protected:
	bool							m_bShallClose;						//关闭标志
	bool							m_bAllowWisper;						//私聊标志
	bool							m_bAllowRoomChat;					//聊天标志
	bool							m_bAllowGameChat;					//聊天标志
	bool							m_bAllowEnterRoom;					//进入标志
	bool							m_bAllowEnterGame;					//进入标志

	//配置信息
protected:
	DWORD							m_dwCenterServer;					//中心地址
	tagGameServiceAttrib			* m_pGameServiceAttrib;				//服务属性
	tagGameServiceOption			* m_pGameServiceOption;				//服务配置

	//游戏组件
protected:
	HINSTANCE						m_hDllInstance;						//游戏实例
	IGameServiceManager				* m_pIGameServiceManager;			//管理接口

	//列表解释
protected:
	CMD_GR_ColumnInfo				m_ColumnInfo;						//列表解释
	WORD							m_wColumnInfoSize;					//列表长度

	//在线人数
protected:
	WORD							m_wOnLineInfoSize;					//信息长度
	tagOnLineCountInfo				m_OnLineCountInfo[512];				//在线信息

	//组件变量
protected:
	CServerList						m_ServerList;						//房间列表
	CServerUserManager				m_ServerUserManager;				//用户管理
	CAndroidUserManager				m_AndroidUserManager;				//用户管理

	//接口变量
protected:
	ITimerEngine *					m_pITimerEngine;					//时间引擎
	IDataBaseEngine *				m_pIDataBaseEngine;					//数据引擎
	IAttemperEngine *				m_pIAttemperEngine;					//调度引擎
	ITCPNetworkEngine *				m_pITCPNetworkEngine;				//网络引擎
	ITCPSocketService *				m_pITCPSocketCorrespond;			//协调服务

	//扩展变量
protected:
	CPropertyAttributeArrary		m_PropertyAttributeArrary;			//道具信息
	CFlowerAttributeArrary			m_FlowerAttributeArrary;			//鲜花信息
	std::string                     m_strAffiche;    //公告                       //公告信息
	std::string                     m_strHappyMess;  //喜报 

	//函数定义
public:
	//构造函数
	CAttemperEngineSink();
	//析构函数
	virtual ~CAttemperEngineSink();

	//基础接口
public:
	//释放对象
	virtual void __cdecl Release() { delete this; }
	//接口查询
	virtual void * __cdecl QueryInterface(const IID & Guid, DWORD dwQueryVer);

	//状态接口
public:



	//关闭查询
	virtual bool __cdecl IsShallClose() { return m_bShallClose; }
	//私聊查询
	virtual bool __cdecl IsAllowWisper() { return m_bAllowWisper; }
	//聊天查询
	virtual bool __cdecl IsAllowRoomChat() { return m_bAllowRoomChat; }
	//聊天查询
	virtual bool __cdecl IsAllowGameChat() { return m_bAllowGameChat; }
	//进入查询
	virtual bool __cdecl IsAllowEnterRoom() { return m_bAllowEnterRoom; }
	//进入查询
	virtual bool __cdecl IsAllowEnterGame() { return m_bAllowEnterGame; }

	//状态通知
public:
	//发送状态
	virtual bool __cdecl SendTableStatus(WORD wTableID);
	//发送分数
	virtual bool __cdecl SendUserScore(IServerUserItem * pIServerUserItem);
	//发送状态
	virtual bool __cdecl SendUserStatus(IServerUserItem * pIServerUserItem);
	//更新用户的游戏数据; 每一局写到数据库一次
    virtual bool __cdecl UpdateUserItem(IServerUserItem * pIServerUserItem);
	//网络接口
public:
	//发送数据
	virtual bool __cdecl SendData(IServerUserItem * pIServerUserItem, WORD wMainCmdID, WORD wSubCmdID);
	//发送数据
	virtual bool __cdecl SendData(IServerUserItem * pIServerUserItem, WORD wMainCmdID, WORD wSubCmdID, void * pData, WORD wDataSize);

	//定时器接口
public:
	//设置定时器
	virtual bool __cdecl SetTableTimer(WORD wTableID, DWORD dwTimerID, DWORD dwElapse, DWORD dwRepeat, WPARAM wBindParam);
	//删除定时器
	virtual bool __cdecl KillTableTimer(WORD wTableID, DWORD dwTimerID);

	//管理接口
public:
	//解锁用户
	bool  UnLockUserItem(IServerUserItem * pIServerUserItem);
	//删除用户
	virtual bool __cdecl DeleteUserItem(IServerUserItem * pIServerUserItem);
	//输出信息
	virtual void __cdecl ExportInformation(LPCTSTR pszString, enTraceLevel TraceLevel);

	//消息函数
public:
	//发送道具消息
	bool SendProMessage(DWORD dwSocketID, LPCTSTR lpszMessage, WORD wMessageType);
	//发送房间消息
	bool SendRoomMessage(DWORD dwSocketID, LPCTSTR lpszMessage, WORD wMessageType);
	//发送游戏消息
	bool SendGameMessage(DWORD dwSocketID, LPCTSTR lpszMessage, WORD wMessageType);
	//发送道具消息
	bool SendProMessage(IServerUserItem * pIServerUserItem, LPCTSTR lpszMessage, WORD wMessageType);
	//发送房间消息
	bool SendRoomMessage(IServerUserItem * pIServerUserItem, LPCTSTR lpszMessage, WORD wMessageType);
	//发送游戏消息
	bool SendGameMessage(IServerUserItem * pIServerUserItem, LPCTSTR lpszMessage, WORD wMessageType);

	//异步接口
public:
	//启动事件
	virtual bool __cdecl OnAttemperEngineStart(IUnknownEx * pIUnknownEx);
	//停止事件
	virtual bool __cdecl OnAttemperEngineStop(IUnknownEx * pIUnknownEx);

	//事件接口
public:
	//控制事件
	virtual bool __cdecl OnEventControl(WORD wIdentifier, VOID * pData, WORD wDataSize) { return true; }
	//自定事件
	virtual bool __cdecl OnEventAttemperData(WORD wRequestID, VOID * pData, WORD wDataSize) { return true; }

	//内核事件
public:
	//时间事件
	virtual bool __cdecl OnEventTimer(DWORD dwTimerID, WPARAM wBindParam);
	//数据库事件
	virtual bool __cdecl OnEventDataBase(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize);

	//连接事件
public:
	//连接事件
	virtual bool __cdecl OnEventTCPSocketLink(WORD wServiceID, INT nErrorCode);
	//关闭事件
	virtual bool __cdecl OnEventTCPSocketShut(WORD wServiceID, BYTE cbShutReason);
	//读取事件
	virtual bool __cdecl OnEventTCPSocketRead(WORD wServiceID, CMD_Command Command, VOID * pData, WORD wDataSize);

	//网络事件
public:
	//应答事件
	virtual bool __cdecl OnEventTCPNetworkBind(DWORD dwClientIP, DWORD dwSocketID);
	//关闭事件
	virtual bool __cdecl OnEventTCPNetworkShut(DWORD dwClientIP, DWORD dwActiveTime, DWORD dwSocketID);
	//读取事件
	virtual bool __cdecl OnEventTCPNetworkRead(CMD_Command Command, VOID * pData, WORD wDataSize, DWORD dwSocketID);

	//数据库函数
private:
	//用户登录成功
	bool OnDBLogonSuccess(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//用户登录失败
	bool OnDBLogonError(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//用户权限
	bool OnDBUserRightResult(DWORD dwContextID, VOID * pData, WORD wDataSize);

	//网络函数
private:
	//登录消息处理
	bool OnSocketMainLogon(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//用户消息处理
	bool OnSocketMainUser(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//管理消息处理
	bool OnSocketMainManager(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//框架消息处理
	bool OnSocketMainFrame(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//游戏消息处理
	bool OnSocketMainGame(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//视频消息处理
	bool OnSocketMainVideo(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//礼物消息处理
	bool OnSocketPresent(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//银行消息处理
	bool OnSocketBank(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);


	//辅助函数
private:
	//发送失败
	bool SendLogonFailed(DWORD dwSocketID, LPCTSTR pszMessage, LONG lErrorCode);
	//发送房间人已满
	bool SendLogonFull(DWORD dwSocketID, LPCTSTR pszMessage, DWORD userCount);

	//发送用户
	bool SendUserItem(IServerUserItem * pIServerUserItem, DWORD dwSocketID);
	//发送信息
	bool SendGameServerInfo(IServerUserItem * pIServerUserItem, DWORD dwSocketID);
	//发送属性
	//void SendPresentAttribute(IServerUserItem *pIServerUserItem);

	//彭文添加
	//安排队列
	bool ArrangeQueue(DWORD dwSocketID);
	//执行固定椅子模式下排队座椅子操作
	bool PerformSitDownChair();

	//判断这个玩家是否符合房间的条件
	bool JudgeUserFitThisRoom(IServerUserItem * pIServerUserItem);
	//向所有的用户发送排队队列的人数
	void SendAllQueueCount();
	//发送队列人数人数
	void SendQueueCount(IServerUserItem * pIServerUserItem);
	
public:
	//发送警告消息
	void SendWarningPacket(IServerUserItem * pIServerUserItem, int MDM_MES, int SUB_MES, TCHAR* pMes);
public:
	//请求数据库响应任务完成
	void PerformTaskFinsh(const CMD_GF_TASK_tagTaskFinish& taskFinish);
	//发送寻宝任务
	bool SendFindGemsTask(IServerUserItem * pIServerUserItem);
	//发送任务完成消息
	bool SendTaskFinish(VOID * pData, WORD wDataSize);

	//发送公告信息
	bool OnDBFindGemsTaskLoad( VOID * pData, WORD wDataSize);
	
	//发送新手礼包
	void SendNewGift();
	//
	void NewGiftData(CMD_GF_NEW_GIFT *tagNewGift);

public:
	//群发喜报
	void SendHappyMess();

	//发送公告
	void SendAffiche(IServerUserItem * pIServerUserItem);
	//加载公告
	bool OnDBLoadAffiche(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//转发用户动作消息
	bool SendUserActionInfo(IServerUserItem* pIServerUserItem, void* pData, DWORD wDataSize);
	//加载喜报
	bool OnDBLoadHappyMess(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//添加结束

	

private:
	//列表消息处理
	bool OnCenterMainServerList(CMD_Command Command, VOID * pData, WORD wDataSize);

	//辅助函数
protected:
	//绑定参数
	tagConnectItemInfo * GetBindParameter(WORD wBindIndex);
	//绑定用户
	IServerUserItem * GetServerUserItem(WORD wBindIndex);
	//用户状况
	WORD GetAndroidUserStatus(tagAndroidUserStatus & AndroidUserStatus);
	//桌子状况
	WORD GetTableUserStatus(CTableFrame * pTableFrame, tagTableUserStatus & TableUserStatus);
	//喇叭事件
	bool OnEventBugle(const void * pData, WORD wDataSize, IServerUserItem * pIServerUserItem);
	//提取事件
	bool OnEventBankDrawoutGold(const void * pData, WORD wDataSize, DWORD dwSocketID);
	//存储事件
	bool OnEventBankStorage(const void * pData, WORD wDataSize, DWORD dwSocketID);

	//辅助函数
protected:
	//使用权限
	bool IsPropertyUseRight(INT nPropertyID,IServerUserItem *pIServerUserItem,IServerUserItem *pTargetUserItem);
	//修改金币
	void ModifyGameGold(IServerUserItem * pIServerUserItem,LONGLONG lGameGold);
	//修改帐款
	void ModifyBankStorageGold(IServerUserItem * pIServerUserItem, LONGLONG lStorageCount);
	//消费金币
	LONGLONG GetBankStorageGold(IServerUserItem * pIServerUserItem);

	//彭文添加
	//得到正在玩的桌子的个数  
	unsigned int CountPlayeringDesk();
    //执行缺人桌子的坐下操作
	bool PerformSitDownOnDesk(int lessUser);
	//检查用户是否正在玩
	bool CheckUserPlaying(IServerUserItem * pIServerUserItem);
	//更新定时增加机器人程序
	void UpDateAddAndroid();
	//定时更新繁忙的桌子数目
	void UpdateBusyDesk();
	//尝试向桌子中加用户
	void TryAddUser();
	//尝试向桌子增加机器人
	void AddKeepSittingAndroid();
public:
	//将用户塞入等待队列中
	bool InsertUserInQueue(IServerUserItem * pIServerUserItem);
    //void Test();

	// 设置日志指针
	void SetRoomLog(cnd_ILogMgr*	pRoomLog);

	//添加结束
};

//////////////////////////////////////////////////////////////////////////

#endif