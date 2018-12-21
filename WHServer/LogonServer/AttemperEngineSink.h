#ifndef ATTEMPER_ENGINE_SINK_HEAD_FILE
#define ATTEMPER_ENGINE_SINK_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "Commonality.h"
#include "ServerList.h"
#include "InitParamter.h"
#include <map>

//////////////////////////////////////////////////////////////////////////

//绑定参数
struct tagBindParameter
{
	DWORD							dwSocketID;							//网络标识
	DWORD							dwClientIP;							//连接地址
	DWORD							dwActiveTime;						//激活时间
};

//////////////////////////////////////////////////////////////////////////

//调度引擎钩子
class CAttemperEngineSink : public IAttemperEngineSink
{
	//友元定义
	friend class CLogonService;

	//变量定义
protected:
	CInitParamter *					m_pInitParamter;					//配置参数
	tagBindParameter *				m_pBindParameter;					//连接信息

	//辅助组件
protected:
	CServerList						m_ServerList;						//服务器列表

	//接口变量
protected:
	ITimerEngine *					m_pITimerEngine;					//时间引擎
	IDataBaseEngine *				m_pIDataBaseEngine;					//数据引擎
	ITCPNetworkEngine *				m_pITCPNetworkEngine;				//网络引擎
	ITCPSocketService *				m_pITCPSocketCorrespond;			//协调服务

private:
	std::map<unsigned int , DWORD> m_mapRechargeUserSocketID;
	std::map<DWORD, unsigned int> m_mapRechargeSocketIDUser;

	std::vector<std::string>   m_vecInValidWord;
	std::vector<std::string>   m_vecWordReplaced;

	//函数定义
public:
	//构造函数
	CAttemperEngineSink();
	//析构函数
	virtual ~CAttemperEngineSink();

	//基础接口
public:
	//释放对象
	virtual void __cdecl Release() { return; }
	//接口查询
	virtual void * __cdecl QueryInterface(const IID & Guid, DWORD dwQueryVer);

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
	//用户登录失败
	bool OnDBLogonError(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//用户登录成功
	bool OnDBLogonSuccess(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//用户签到
	bool OnDBUserSignInResult(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//用户领奖
	bool OnDBUserGetAwardResult(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//兑换的个人信息
	bool OnDBUserExchangeInfoResult(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//兑换的商品
	bool OnDBUserExchangeProductResult(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//兑换的商品
	bool OnDBUserExchangeProductResult_V2(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//兑换
	bool OnDBUserExchangeResult(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//兑换
	bool OnDBUserExchangeResult_V2(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//兑换的记录
	bool OnDBUserExchangeRecordResult(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//下载成功
	bool OnDBDownloadFaceSuccess(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//成功消息
	bool OnDBRCustomFaceUploadResult(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//删除结果
	bool OnDBRCustomFaceDeleteResult(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//修改结果
	bool OnDBRModifyIndividual(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//修改个性签名结果
	bool OnDBRModifyUnderWrite(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//修改手机号结果
	bool OnDBRModifyPhone(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//校验名称
	bool OnDBRCheckID(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//校验昵称
	bool OnDBRCheckNickName(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//注册
	bool OnDBRRegister(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//升级
	bool OnDBRIDUpdate(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//重置密码
	bool OnDBRResetPwd(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//查询用户信息
	bool OnDBRUserInfo(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//查询用户是否可以领取推广奖励
	bool OnDBRUserCanSpread(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//用户领取推广奖励
	bool OnDBRUserGetSpread(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//查询推广员信息
	bool OnDBRUserSpreaderInfo(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//弹窗公告信息
	bool OnDBRUserBbsPop(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//滚动公告信息
	bool OnDBRUserBbsScroll(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//列表公告信息
	bool OnDBRUserBbsList(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//玩家任务信息
	bool OnDBRUserTaskInfo(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//玩家排行榜
	bool OnDBRUserGetRanking(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//玩家排行榜
	bool OnDBRUserGetRanking_V2(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//玩家是否入排行榜信息
	bool OnDBRUserGetRankingStatus(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//修改玩家是否入排行榜返回信息
	bool OnDBRUserGetRankingStatusResult(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//意见反馈结果
	bool OnDBRUserFeedBack(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//查询意见反馈结果
	bool OnDBRUserGetFeedBack(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//查询用户身份证账号信息的结果
	bool OnDBRUserInfoIDCard(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//请求提交身份证信息的结果
	bool OnDBRUserAuthentication(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//请求商城商品列表信息的结果
	bool OnDBRUserMallProductResult(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//请求商城购买下订单的结果
	bool OnDBRUserMallPlaceOrderResult(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//请求查询用户银行信息的结果
	bool OnDBRUserBankInfoResult(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//用户操作银行结果
	bool OnDBRUserBankChargeResult(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//用户取消订单
	bool OnDBRUserCancelOrderResult(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//用户购买结果
	bool OnDBRUserMallBuyResult(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//用户更新结果
	bool OnDBRUserMallUpdateResult(DWORD dwContextID, VOID * pData, WORD wDataSize);
	
	//活动列表结果
	bool OnDBRUserActivityListResult(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//活动结果
	bool OnDBRUserActivityResult(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//抽奖结果
	bool OnDBRUserActivityLuckyResult(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//抽奖记录
	bool OnDBRUserActivityRecordResult(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//发喇叭
	bool OnDBRUserSpeakerSendResult(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//查询配置结果
	bool OnDBRUserQueryConfigResult(DWORD dwContextID, VOID * pData, WORD wDataSize);
	
	
	//网络函数
private:
	//登录消息处理
	bool OnSocketMainLogon(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//用户信息处理
	bool OnSocketMainUser(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//服务器列表相关处理
	bool OnSocketServerList(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);

	//soap事件
	bool OnSoapRequest(WORD wSubCmdID, VOID * pData, WORD wDataSize);

	//中心网络
private:
	//列表消息处理
	bool OnCenterMainServerList(WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//服务器管理消息处理
	bool OnCenterMainServerManager(WORD wSubCmdID, VOID * pData, WORD wDataSize);

private:
	std::string CheckWord(char* data, short len);
};

//////////////////////////////////////////////////////////////////////////

#endif