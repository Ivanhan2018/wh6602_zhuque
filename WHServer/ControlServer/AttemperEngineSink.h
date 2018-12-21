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
	friend class CControlService;

	//变量定义
protected:
	CInitParamter *					m_pInitParamter;					//配置参数
	tagBindParameter *				m_pBindParameter;					//连接信息

	//辅助组件
protected:
	CServerList						m_ServerList;						//服务器列表
	std::map<DWORD, DWORD>			m_mapMax;		//最大人数
	std::map<DWORD, std::pair<DWORD, std::string>>			m_mapMaxToday;  //今天最高人数,以及时间点。按kindid
	int								m_iMaxOnLineToday;		//今天最高总在线
	std::string						m_strCollectDateToday;	//今天最高在线发生的时间点

	//接口变量
protected:
	ITimerEngine *					m_pITimerEngine;					//时间引擎
	IDataBaseEngine *				m_pIDataBaseEngine;					//数据引擎
//	ITCPNetworkEngine *				m_pITCPNetworkEngine;				//网络引擎
	ITCPSocketService *				m_pITCPSocketCorrespond;			//协调服务

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

	//网络函数
private:
	//登录消息处理
	bool OnSocketMainControl(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//用户信息处理
	bool OnSocketMainUser(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);

	//中心网络
private:
	//列表消息处理
	bool OnCenterMainServerList(WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//控制命令处理
	bool OnCenterMainControlCommand(CMD_Command Command, VOID * pData, WORD wDataSize);

private:
	//系统喇叭
	bool OnDBCheckSpeakerTaskResult(DWORD dwContextID, VOID * pData, WORD wDataSize);
};

//////////////////////////////////////////////////////////////////////////

#endif