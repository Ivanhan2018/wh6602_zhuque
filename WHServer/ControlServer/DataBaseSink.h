#ifndef DATA_BASE_SINK_HEAD_FILE
#define DATA_BASE_SINK_HEAD_FILE

#pragma once

#include "Afxinet.h"
#include "Commonality.h"
#include "InitParamter.h"

//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////

//数据库引擎钩子
class CDataBaseSink : public IDataBaseEngineSink
{
	//友元定义
	friend class CControlService;

	//变量定义
protected:
	CInitParamter *					m_pInitParamter;					//配置参数
	IDataBaseEngineEvent *			m_pIDataBaseEngineEvent;			//数据事件

	//组件变量
protected:
	CDataBaseAide					m_AccountsDBAide;					//用户数据库
	CDataBaseHelper					m_AccountsDBModule;					//用户数据库

	//函数定义
public:
	//构造函数
	CDataBaseSink();
	//析构函数
	virtual ~CDataBaseSink();

	//基础接口
public:
	//释放对象
	virtual VOID __cdecl Release() { return; }
	//接口查询
	virtual VOID * __cdecl QueryInterface(REFGUID Guid, DWORD dwQueryVer);

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
	//GM任务请求处理
	bool OnRequestCheckGMTask(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize);
	//生成排行榜
	bool OnRequestCreateRanking(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize);
	//写最高在线
	bool OnRequestWriteOnline(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize);
	//写系统消息
	bool OnRequestSpeakerSend(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize);
	
	
	//任务请求处理
	LONG SPCheckGMTask();
	//系统喇叭请求处理
	LONG SPCheckSpeakerTask();
	//生成排行榜
	LONG SPCreateRanking();
	//写最高在线
	LONG SPWriteOnline(int type, int value, LPCTSTR collectDate);
	//写系统消息
	LONG SPSpeakerSend(DWORD dwUserID, WORD wType, LPCTSTR pszContent);
};

//////////////////////////////////////////////////////////////////////////

#endif