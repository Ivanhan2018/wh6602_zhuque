#include "StdAfx.h"
#include "ControlService.h"

//////////////////////////////////////////////////////////////////////////
CInitParamter					g_InitParamter;						//配置参数

//构造函数
CControlService::CControlService()
{
}

//析构函数
CControlService::~CControlService()
{
}

//开始服务
bool CControlService::StartService()
{
	//停止服务
	ASSERT(m_ServiceStatus!=ServiceStatus_Service);
	if (m_ServiceStatus==ServiceStatus_Service) return false;
	
	//终止服务
	StopService();

	//加载参数
//	m_InitParamter.LoadInitParamter();

	//创建组件
	if ((m_TimerEngine.GetInterface()==NULL)&&(m_TimerEngine.CreateInstance()==false)) return false;
	if ((m_DataBaseEngine.GetInterface()==NULL)&&(m_DataBaseEngine.CreateInstance()==false)) return false;
	if ((m_AttemperEngine.GetInterface()==NULL)&&(m_AttemperEngine.CreateInstance()==false)) return false;
//	if ((m_TCPNetworkEngine.GetInterface()==NULL)&&(m_TCPNetworkEngine.CreateInstance()==false)) return false;
	if ((m_TCPSocketCorrespond.GetInterface()==NULL)&&(m_TCPSocketCorrespond.CreateInstance()==false)) return false;

	//组件接口
	IUnknownEx * pIAttemperEngine=m_AttemperEngine.GetInterface();
//	IUnknownEx * pITCPNetworkEngine=m_TCPNetworkEngine.GetInterface();

	//回调接口
	IUnknownEx * pIDataBaseEngineSink=QUERY_OBJECT_INTERFACE(m_DataBaseEngineSink,IUnknownEx);
	IUnknownEx * pIAttemperEngineSink=QUERY_OBJECT_INTERFACE(m_AttemperEngineSink,IUnknownEx);

	//绑定接口
	if (m_AttemperEngine->SetAttemperEngineSink(pIAttemperEngineSink)==false) return false;
	if (m_DataBaseEngine->SetDataBaseEngineSink(pIDataBaseEngineSink)==false) return false;

	//内核组件
	if (m_TimerEngine->SetTimerEngineEvent(pIAttemperEngine)==false) return false;
//	if (m_AttemperEngine->SetNetworkEngine(pITCPNetworkEngine)==false) return false;
	if (m_TCPSocketCorrespond->SetTCPSocketEvent(pIAttemperEngine)==false) return false;
//	if (m_TCPNetworkEngine->SetTCPNetworkEngineEvent(pIAttemperEngine)==false) return false;

	//调度回调
	m_AttemperEngineSink.m_pInitParamter=&g_InitParamter;
	m_AttemperEngineSink.m_pITimerEngine=m_TimerEngine.GetInterface();
	m_AttemperEngineSink.m_pIDataBaseEngine=m_DataBaseEngine.GetInterface();
//	m_AttemperEngineSink.m_pITCPNetworkEngine=m_TCPNetworkEngine.GetInterface();
	m_AttemperEngineSink.m_pITCPSocketCorrespond=m_TCPSocketCorrespond.GetInterface();

	//数据库回调
	m_DataBaseEngineSink.m_pInitParamter=&g_InitParamter;
	m_DataBaseEngineSink.m_pIDataBaseEngineEvent=QUERY_OBJECT_PTR_INTERFACE(pIAttemperEngine,IDataBaseEngineEvent);

	//配置网络
	WORD wMaxConnect=g_InitParamter.m_wMaxConnect;
	WORD wServicePort=g_InitParamter.m_wListenPort;
	//Flag置为1表示消息不用加密
//	if (m_TCPNetworkEngine->SetServiceParameter(wServicePort,wMaxConnect,1)==false) return false;

	//时间引擎
	if (m_TimerEngine->StartService()==false)
	{
		ASSERT(FALSE);
		return false;
	}

	//网络引擎
	if (m_TCPSocketCorrespond->StartService()==false)
	{
		ASSERT(FALSE);
		return false;
	}

	//调度引擎
	if (m_AttemperEngine->StartService()==false)
	{
		ASSERT(FALSE);
		return false;
	}

	//数据引擎
	if (m_DataBaseEngine->StartService()==false)
	{
		ASSERT(FALSE);
		return false;
	}

	//网络引擎
//	if (m_TCPNetworkEngine->StartService()==false)
//	{
//		ASSERT(FALSE);
//		return false;
//	}

	//设置变量
	m_ServiceStatus=ServiceStatus_Service;

	return true;
}

//停止服务
bool CControlService::StopService()
{
	//设置变量
	m_ServiceStatus=ServiceStatus_Stop;

	//停止服务
	if (m_TimerEngine.GetInterface()!=NULL) m_TimerEngine->ConcludeService();
	if (m_AttemperEngine.GetInterface()!=NULL) m_AttemperEngine->ConcludeService();
	if (m_DataBaseEngine.GetInterface()!=NULL) m_DataBaseEngine->ConcludeService();
//	if (m_TCPNetworkEngine.GetInterface()!=NULL) m_TCPNetworkEngine->ConcludeService();
	if (m_TCPSocketCorrespond.GetInterface()!=NULL) m_TCPSocketCorrespond->ConcludeService();

	return true;
}

//下发命令
bool CControlService::SendCommand(VOID * pData, WORD wDataSize)
{
	if (m_AttemperEngine.GetInterface()!=NULL)
	{
		return m_TCPSocketCorrespond->SendData(MDM_CS_CONTROL_COMMAND,SUB_CS_CONTROL_COMMAND_REQ,pData, wDataSize);
	}

	return true;
}
//////////////////////////////////////////////////////////////////////////
