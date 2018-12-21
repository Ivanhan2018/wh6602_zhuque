#include "StdAfx.h"
#include "Commonality.h"
#include "DataBaseSink.h"
#include "ControlServer.h"


//声明变量
extern CControlServerApp theApp;
extern void TraceMessage(LPCTSTR pszMessage);
//////////////////////////////////////////////////////////////////////////

//构造函数
CDataBaseSink::CDataBaseSink()
{
	//设置变量
	m_pInitParamter=NULL;
	m_pIDataBaseEngineEvent=NULL;

	return;
}

//析构函数
CDataBaseSink::~CDataBaseSink()
{
}

//接口查询
VOID * __cdecl CDataBaseSink::QueryInterface(const IID & Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IDataBaseEngineSink,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IDataBaseEngineSink,Guid,dwQueryVer);
	return NULL;
}

//模块启动
bool __cdecl CDataBaseSink::OnDataBaseEngineStart(IUnknownEx * pIUnknownEx)
{
	//创建对象
	if ((m_AccountsDBModule.GetInterface()==NULL)&&(m_AccountsDBModule.CreateInstance()==false))
	{
		ASSERT(FALSE);
		return false;
	}

	try
	{
		//连接用户数据库
		m_AccountsDBModule->SetConnectionInfo(m_pInitParamter->m_szUserDataBaseAddr,m_pInitParamter->m_wUserDataBasePort,
			m_pInitParamter->m_szUserDataBaseName,m_pInitParamter->m_szUserDataBaseUser,m_pInitParamter->m_szUserDataBasePass);
		m_AccountsDBModule->OpenConnection();

		//发起连接
		m_AccountsDBModule->OpenConnection();
		m_AccountsDBAide.SetDataBase(m_AccountsDBModule.GetInterface());

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);

		return false;
	}

	return true;
}

//模块关闭
bool __cdecl CDataBaseSink::OnDataBaseEngineStop(IUnknownEx * pIUnknownEx)
{
	//设置对象
	m_AccountsDBAide.SetDataBase(NULL);

	//关闭连接
	if (m_AccountsDBModule.GetInterface()!=NULL)
	{
		m_AccountsDBModule->CloseConnection();
		m_AccountsDBModule.CloseInstance();
	}

	//组件变量
	m_pInitParamter=NULL;
	m_pIDataBaseEngineEvent=NULL;

	return true;
}

//数据操作处理
bool __cdecl CDataBaseSink::OnDataBaseEngineRequest(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	switch (wRequestID)
	{
	case DBR_GP_CHECK_GM_TASK:
		{
			return OnRequestCheckGMTask(wRequestID,dwContextID,pData,wDataSize);
		}
		break;
	case DBR_GP_SPEAKER_SEND:
		{
			return OnRequestSpeakerSend(wRequestID,dwContextID,pData,wDataSize);
		}
		break;
	case DBR_GP_CREATE_RANKING:
		{
			return OnRequestCreateRanking(wRequestID,dwContextID,pData,wDataSize);
		}
		break;
	case DBR_GP_WRITE_ONLINE:
		{
			return OnRequestWriteOnline(wRequestID,dwContextID,pData,wDataSize);
		}
		break;
	}

	return false;
}


//GMTask请求处理
bool CDataBaseSink::OnRequestCheckGMTask(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//变量定义
	LONG lReturnValue=-1;
	try
	{
		//检查定时任务
		lReturnValue=SPCheckGMTask();
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		CTraceService::TraceString("SPCheckGMTask exception",TraceLevel_Exception);
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);
	}
	//结果判断
	if (lReturnValue!=0L)
	{
		CTraceService::TraceString("SPCheckGMTask fail",TraceLevel_Normal);
	}
	else
	{
		static int iCnt = 0;
		//压缩日志，不要每次都显示
		if (iCnt % 15 == 0)
		{
			CTraceService::TraceString("SPCheckGMTask OK",TraceLevel_Normal);
		}
		iCnt++;
	}

	try
	{
		//检查系统喇叭
		lReturnValue=SPCheckSpeakerTask();
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		CTraceService::TraceString("SPCheckSpeakerTask exception",TraceLevel_Exception);
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);
	}

	//结果判断
	if (lReturnValue!=0L)
	{
		CTraceService::TraceString("SPCheckSpeakerTask fail",TraceLevel_Normal);
	}
	else
	{
		//获取数据
		IDataBase * pIDataBase=(IDataBase *)m_AccountsDBAide.GetDataBase(IID_IDataBase,VER_IDataBase);

		int iCnt=0;
		//读取信息
		while (pIDataBase->IsRecordsetEnd()==false)
		{
			DBR_GP_Speaker_Task_Result record;
			ZeroMemory(&record, sizeof(record));

			//读取信息
			record.dwUserID=m_AccountsDBAide.GetValue_DWORD(TEXT("UserID"));
			m_AccountsDBAide.GetValue_String(TEXT("Params"),record.szMsg,CountArray(record.szMsg));
			record.nLen = std::string(record.szMsg).size();

			iCnt++;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_GP_CHECK_SPEAKER_TASK_RESULT,dwContextID,
							&record, sizeof(record));
			char buf[256];
			memset(buf, 0, 256);
			sprintf(buf, "msg:%s", record.szMsg);
			TraceMessage(buf);

			CTraceService::TraceString("Send Speaker OK",TraceLevel_Normal);

			//移动记录
			pIDataBase->MoveToNext();
		}

		//关闭记录
		pIDataBase->CloseRecordset();
	}

	return true;
}

//生成排行榜
bool CDataBaseSink::OnRequestCreateRanking(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//变量定义
		LONG lReturnValue=-1;
		lReturnValue=SPCreateRanking();
		
		//结果判断
		if (lReturnValue!=0L)
		{
			CTraceService::TraceString("SPCreateRanking fail",TraceLevel_Normal);
		}
		else
		{
			CTraceService::TraceString("SPCreateRanking OK",TraceLevel_Normal);
		}
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);
	}

	//关闭记录集
	m_AccountsDBModule->CloseRecordset();

	return true;
}

//写最高在线
bool CDataBaseSink::OnRequestWriteOnline(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//数据验证
	ASSERT(wDataSize==sizeof(DBR_GR_Online));
	if (wDataSize!=sizeof(DBR_GR_Online)) return false;

	//数据转换
	DBR_GR_Online *pOnline = (DBR_GR_Online*)pData;

	try
	{
		//变量定义
		LONG lReturnValue=-1;
		lReturnValue=SPWriteOnline(pOnline->dwTypeID, pOnline->dwValue, pOnline->szCollectDate);
		
		//结果判断
		if (lReturnValue!=0L)
		{
			CTraceService::TraceString("SPWriteOnline fail",TraceLevel_Normal);
		}
		else
		{
			CTraceService::TraceString("SPWriteOnline OK",TraceLevel_Normal);
		}
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);
	}

	//关闭记录集
	m_AccountsDBModule->CloseRecordset();

	return true;
}

//写喇叭
bool CDataBaseSink::OnRequestSpeakerSend(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//数据验证
	ASSERT(wDataSize==sizeof(DBR_GP_SpeakerSend));
	if (wDataSize!=sizeof(DBR_GP_SpeakerSend)) return false;

	//数据转换
	DBR_GP_SpeakerSend *pReq = (DBR_GP_SpeakerSend*)pData;

	//执行存储过程
	LONG lReturnValue = -1;
	try
	{
		lReturnValue = SPSpeakerSend(pReq->dwUserID, pReq->iType, pReq->szDate);
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);
	}

	return true;
}

//任务请求处理
LONG CDataBaseSink::SPCheckGMTask()
{
	//执行存储过程
	m_AccountsDBAide.ResetParameter();

	return m_AccountsDBAide.ExecuteProcess(TEXT("WHGameUserDB.dbo.GSP_GP_CheckGMTask"),true);
}

//系统喇叭请求处理
LONG CDataBaseSink::SPCheckSpeakerTask()
{
	//执行存储过程
	m_AccountsDBAide.ResetParameter();

	return m_AccountsDBAide.ExecuteProcess(TEXT("WHGameUserDB.dbo.GSP_GP_CheckSpeakerTask"),true);
}

//生成排行榜
LONG CDataBaseSink::SPCreateRanking()
{
	//执行存储过程
	m_AccountsDBAide.ResetParameter();

	return m_AccountsDBAide.ExecuteProcess(TEXT("WHTreasureDB.dbo.GSP_GR_WriteRanking"),true);
}

//写最高在线
LONG CDataBaseSink::SPWriteOnline(int type, int value, LPCTSTR collectDate)
{
	//执行存储过程
	m_AccountsDBAide.ResetParameter();
	m_AccountsDBAide.AddParameter(TEXT("@dwTypeID"),type);
	m_AccountsDBAide.AddParameter(TEXT("@dwValue"),value);
	m_AccountsDBAide.AddParameter(TEXT("@DateCollect"),collectDate);

	return m_AccountsDBAide.ExecuteProcess(TEXT("WHServerInfoDB.dbo.GSP_GR_CommitOnline"),true);
}

//发喇叭
LONG CDataBaseSink::SPSpeakerSend(DWORD dwUserID, WORD wType, LPCTSTR pszContent)
{
	//执行存储过程
	m_AccountsDBAide.ResetParameter();
	m_AccountsDBAide.AddParameter(TEXT("@dwUserID"),dwUserID);
	m_AccountsDBAide.AddParameter(TEXT("@wType"),wType);
	m_AccountsDBAide.AddParameter(TEXT("@szMsg"),pszContent);

	return m_AccountsDBAide.ExecuteProcess(TEXT("WHServerInfoDB.dbo.GSP_GP_UserSpeakerSend"),true);
}

//////////////////////////////////////////////////////////////////////////
