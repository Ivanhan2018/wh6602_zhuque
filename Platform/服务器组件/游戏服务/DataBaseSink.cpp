#include "StdAfx.h"
#include "Afxinet.h"
#include "DataBaseSink.h"
#include "CGlobalData.h"


//////////////////////////////////////////////////////////////////////////

//构造函数
CDataBaseSink::CDataBaseSink()
{
	m_pRoomLog = NULL;		// 日志

	//设置变量
	m_pGameUserDBInfo=NULL;
	m_pGameScoreDBInfo=NULL;
	m_pGameServiceAttrib=NULL;
	m_pGameServiceOption=NULL;
	m_pIDataBaseEngineEvent=NULL;

	return;
}

//析构函数
CDataBaseSink::~CDataBaseSink()
{
	CCGlobalData::UnAllocGradeInfoMemory();
}

//接口查询
void * __cdecl CDataBaseSink::QueryInterface(const IID & Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IDataBaseEngineSink,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IDataBaseEngineSink,Guid,dwQueryVer);
	return NULL;
}

//调度模块启动
bool __cdecl CDataBaseSink::OnDataBaseEngineStart(IUnknownEx * pIUnknownEx)
{
	//创建对象
	if ((m_AccountsDBModule.GetInterface()==NULL)&&(m_AccountsDBModule.CreateInstance()==false))
	{
		ASSERT(FALSE);
		return false;
	}

	//创建对象
	if ((m_GameScoreDBModule.GetInterface()==NULL)&&(m_GameScoreDBModule.CreateInstance()==false))
	{
		ASSERT(FALSE);
		return false;
	}

	CString strDBName;

	try
	{
		//变量定义
		BYTE * pcbAddr=NULL;
		TCHAR szDataBaseAddr[16]=TEXT("");

		//连接用户数据库
		pcbAddr=(BYTE *)&m_pGameUserDBInfo->dwDataBaseAddr;
		_snprintf(szDataBaseAddr,sizeof(szDataBaseAddr),TEXT("%d.%d.%d.%d"),pcbAddr[0],pcbAddr[1],pcbAddr[2],pcbAddr[3]);
		m_AccountsDBModule->SetConnectionInfo(szDataBaseAddr,m_pGameUserDBInfo->wDataBasePort,m_pGameUserDBInfo->szDataBaseName,
			m_pGameUserDBInfo->szDataBaseUser,m_pGameUserDBInfo->szDataBasePass);

		strDBName = m_pGameUserDBInfo->szDataBaseName;

		//发起连接
		m_AccountsDBModule->OpenConnection();
		m_AccountsDBAide.SetDataBase(m_AccountsDBModule.GetInterface());

		//连接游戏数据库
		pcbAddr=(BYTE *)&m_pGameScoreDBInfo->dwDataBaseAddr;
		_snprintf(szDataBaseAddr,sizeof(szDataBaseAddr),TEXT("%d.%d.%d.%d"),pcbAddr[0],pcbAddr[1],pcbAddr[2],pcbAddr[3]);
		m_GameScoreDBModule->SetConnectionInfo(szDataBaseAddr,m_pGameScoreDBInfo->wDataBasePort,m_pGameScoreDBInfo->szDataBaseName,
			m_pGameScoreDBInfo->szDataBaseUser,m_pGameScoreDBInfo->szDataBasePass);

		strDBName = m_pGameScoreDBInfo->szDataBaseName;

		//发起连接
		m_GameScoreDBModule->OpenConnection();
		m_GameScoreDBAide.SetDataBase(m_GameScoreDBModule.GetInterface());

		if (m_pRoomLog)
			m_pRoomLog->AddMsg(0, 0, TRUE,"***** 数据库模块启动");
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);

		// add by HouGuoJiang 2012-03-27
		if(m_pRoomLog)
		{
			CString strErrorLine;
			strErrorLine.Format("函数名：%s  所在行：%d 文件名：%s",  __FUNCTION__ ,  __LINE__  , 	__FILE__ );

			CString strPrompt;
			strPrompt.Format("%s , 出错原因：%s, %s , 库连接失败，用户名与密码不符是产生错误的原因之一", 
				strErrorLine.GetBuffer(), pszDescribe, strDBName.GetBuffer());

			m_pRoomLog->AddMsg(0, 0, TRUE, strPrompt.GetBuffer());
		}

		return false;
	}

	return true;
}

//调度模块关闭
bool __cdecl CDataBaseSink::OnDataBaseEngineStop(IUnknownEx * pIUnknownEx)
{
	try
	{
		//关闭连接
		if (m_AccountsDBModule.GetInterface()) m_AccountsDBModule->CloseConnection();
		if (m_GameScoreDBModule.GetInterface()) m_GameScoreDBModule->CloseConnection();
		
		if (m_pRoomLog)
			m_pRoomLog->AddMsg(0, 0, TRUE,"***** 数据库模块关闭");
		
		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);

		// add by HouGuoJiang 2012-03-27
		if(m_pRoomLog)
		{
			CString strErrorLine;
			strErrorLine.Format("函数名：%s  所在行：%d 文件名：%s",  __FUNCTION__ ,  __LINE__  , 	__FILE__ );

			CString strPrompt;
			strPrompt.Format("%s , 出错原因：%s", 
										strErrorLine.GetBuffer(), pszDescribe);

			m_pRoomLog->AddMsg(0, 0, TRUE, strPrompt.GetBuffer());
		}

		return false;
	}

	return false;
}

//数据操作处理
bool __cdecl CDataBaseSink::OnDataBaseEngineRequest(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	switch (wRequestID)
	{
	case DBR_GR_LOGON_BY_USERID:		//I D 登录
		{
			return OnRequestLogon(dwContextID,pData,wDataSize);
		}
	case DBR_GR_WRITE_GAME_SCORE:		//写分操作
		{
			return OnRequestWriteUserScore(dwContextID,pData,wDataSize);
		}
	case DBR_GR_LEAVE_GAME_SERVER:		//离开房间
		{
			return OnRequestLeaveGameServer(dwContextID,pData,wDataSize);
		}
	case DBR_GR_LIMIT_ACCOUNTS:			//禁用帐号
		{
			return OnRequestLimitAccounts(dwContextID,pData,wDataSize);
		}
	case DBR_GR_SET_USER_RIGHT:			//设置权限
		{
			return OnRequestSetUserRight(dwContextID,pData,wDataSize);
		}
	case DBR_GR_BANK_GET_GOLD:			//提取金币
		{
			return OnBankDrawoutGold(dwContextID,pData,wDataSize);
		}
	case DBR_GR_BANK_STORAGE_GOLD:		//存储金币
		{
			return OnBankStorageGold(dwContextID,pData,wDataSize);
		}
	case DBR_GR_LOAD_AFFICHE:  //公告
		{
			return OnAffiche(dwContextID, pData, wDataSize);
		}
	case DBR_GR_LOAD_HAPPY_MESS: //喜报
		{
			return OnHappyMess(dwContextID, pData, wDataSize);
		}
	case DBR_GR_UNLOCK_USER: //用户解锁
		{
			return OnUnLockUser(dwContextID, pData, wDataSize);
		}
	case DBR_GP_READ_GRADE_INFO:  //读取等级信息
		{
			return OnReadGradeInfo(dwContextID,pData,wDataSize);
		}
	case DBR_GR_USER_FIND_GEMS_TASK_PERFORM_FINISH:       //任务完成
		{
			return OnFindGemsTaskPerformFinish(dwContextID,pData,wDataSize);
		}
	case DBR_GR_CONTINUE_CONNECT:	//保持数据库连接一直存在	Author:<cxf>
		{
			ContinueConnect();
			return true;
		}
	case DBR_GR_ONLINE_GIFT://在线赠送<2012-02-07>
		{
			NewGiftUser(pData,wDataSize);
		}
	}

	return false;
}


//登录请求处理
bool CDataBaseSink::OnRequestLogon(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//效验参数
	ASSERT(wDataSize==sizeof(DBR_GR_LogonByUserID));
	if (wDataSize!=sizeof(DBR_GR_LogonByUserID)) return false;

	//登陆处理
	try
	{
		//执行查询
		DBR_GR_LogonByUserID * pLogonByUserID=(DBR_GR_LogonByUserID *)pData;
		LONG lReturnValue=SPLogonByUserID(pLogonByUserID->dwUserID,pLogonByUserID->szPassWord,pLogonByUserID->dwClientIP,pLogonByUserID->szComputerID);

		//登录失败
		if (lReturnValue!=0L)
		{
			DBR_GR_LogonError LogonError;
			LogonError.lErrorCode=lReturnValue;
			m_GameScoreDBAide.GetValue_String(TEXT("ErrorDescribe"),LogonError.szErrorDescribe,CountArray(LogonError.szErrorDescribe));
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_GR_LOGON_ERROR,dwContextID,&LogonError,sizeof(LogonError));
			m_GameScoreDBModule->CloseRecordset();
			return true;
		}

		//变量定义
		DBR_GR_LogonSuccess LogonSuccess;
		memset(&LogonSuccess,0,sizeof(LogonSuccess));

		//读取用户信息
		LogonSuccess.dwUserID=m_GameScoreDBAide.GetValue_DWORD(TEXT("UserID"));
		LogonSuccess.dwCustomFaceVer=m_GameScoreDBAide.GetValue_DWORD(TEXT("CustomFaceVer"));
		LogonSuccess.dwGameID=m_GameScoreDBAide.GetValue_DWORD(TEXT("GameID"));
		LogonSuccess.wFaceID=m_GameScoreDBAide.GetValue_WORD(TEXT("FaceID"));
		LogonSuccess.dwGroupID=m_GameScoreDBAide.GetValue_DWORD(TEXT("GroupID"));
		LogonSuccess.lExperience=m_GameScoreDBAide.GetValue_LONG(TEXT("Experience"));
		LogonSuccess.dwUserRight=m_GameScoreDBAide.GetValue_DWORD(TEXT("UserRight"));
		LogonSuccess.lLoveliness=m_GameScoreDBAide.GetValue_DWORD(TEXT("Loveliness"));
		LogonSuccess.dwMasterRight=m_GameScoreDBAide.GetValue_DWORD(TEXT("MasterRight"));

		//彭文添加
		LogonSuccess.lGold        = m_GameScoreDBAide.GetValue_LONGLONG("lGold");
		LogonSuccess.lGem         = m_GameScoreDBAide.GetValue_LONGLONG("lGem");
		LogonSuccess.dwGrade        = m_GameScoreDBAide.GetValue_DWORD("dwGrade");

		
		m_GameScoreDBAide.GetValue_String(TEXT("NickName"),LogonSuccess.szNickName,CountArray(LogonSuccess.szNickName));
		LogonSuccess.utIsAndroid     =  (USER_TYPE)m_GameScoreDBAide.GetValue_INT("IsAndroid");
		//添加结束
		m_GameScoreDBAide.GetValue_String(TEXT("Accounts"),LogonSuccess.szAccounts,CountArray(LogonSuccess.szAccounts));
		m_GameScoreDBAide.GetValue_String(TEXT("GroupName"),LogonSuccess.szGroupName,CountArray(LogonSuccess.szGroupName));
		m_GameScoreDBAide.GetValue_String(TEXT("UnderWrite"),LogonSuccess.szUnderWrite,CountArray(LogonSuccess.szUnderWrite));
		m_GameScoreDBAide.GetValue_String(TEXT("BankPassword"),LogonSuccess.szBankPassword,CountArray(LogonSuccess.szBankPassword));

		//用户属性
		LogonSuccess.cbGender=m_GameScoreDBAide.GetValue_BYTE(TEXT("Gender"));
		LogonSuccess.cbMemberOrder=m_GameScoreDBAide.GetValue_BYTE(TEXT("MemberOrder"));
		LogonSuccess.cbMasterOrder=m_GameScoreDBAide.GetValue_BYTE(TEXT("MasterOrder"));

		//读取游戏信息
		LogonSuccess.lMoney=m_GameScoreDBAide.GetValue_LONGLONG(TEXT("lMoney"));
		LogonSuccess.lWinCount=m_GameScoreDBAide.GetValue_LONG(TEXT("WinCount"));
		LogonSuccess.lLostCount=m_GameScoreDBAide.GetValue_LONG(TEXT("LostCount"));
		LogonSuccess.lDrawCount=m_GameScoreDBAide.GetValue_LONG(TEXT("DrawCount"));
		LogonSuccess.lFleeCount=m_GameScoreDBAide.GetValue_LONG(TEXT("FleeCount"));
		LogonSuccess.lOnlineTime=m_GameScoreDBAide.GetValue_LONG(TEXT("OnlineTime"));//Author<cxf>
		LogonSuccess.wOnlineGiftCount=m_GameScoreDBAide.GetValue_LONG(TEXT("OnlineGiftCount"));
		//附加信息
		CopyMemory(LogonSuccess.szPassWord,pLogonByUserID->szPassWord,sizeof(LogonSuccess.szPassWord));

		//Author<cxf>;DateTime<2012-03-02>;Description<读取赢取局数>
		if (0!=SPDayWinLostCount(pLogonByUserID->dwUserID))
		{
			LogonSuccess.lKindIDWinCount=m_GameScoreDBAide.GetValue_LONG(TEXT("KindIDWinCount"));
			LogonSuccess.lKindIDLostCount=m_GameScoreDBAide.GetValue_LONG(TEXT("KindIDLostCount"));
			LogonSuccess.lKindIDDrawCount=m_GameScoreDBAide.GetValue_LONG(TEXT("KindIDDrawCount"));
			LogonSuccess.lKindIDFleeCount=m_GameScoreDBAide.GetValue_LONG(TEXT("KindIDFleeCount"));
		}
		//投递调度通知
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_GR_LOGON_SUCCESS,dwContextID,&LogonSuccess,sizeof(LogonSuccess));

		//彭文添加
		LoadFindGamesTask( pLogonByUserID->dwUserID, pLogonByUserID->dwKindID);
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe(); 
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);

		// add by HouGuoJiang 2012-03-27
		if(m_pRoomLog)
		{
			CString strErrorLine;
			strErrorLine.Format("函数名：%s  所在行：%d 文件名：%s",  __FUNCTION__ ,  __LINE__  , 	__FILE__ );

			CString strPrompt;
			strPrompt.Format("%s ,出错原因：%s ", strErrorLine.GetBuffer(), pszDescribe);

			m_pRoomLog->AddMsg(0, 0, TRUE, strPrompt.GetBuffer());
		}

		//操作失败
		DBR_GR_LogonError LogonError;
		LogonError.lErrorCode=-1;
		lstrcpyn(LogonError.szErrorDescribe,TEXT("网络连接失败，请您稍后重试或选择另一游戏服务器！"),CountArray(LogonError.szErrorDescribe));
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_GR_LOGON_ERROR,dwContextID,&LogonError,sizeof(LogonError));
	}

	//关闭记录集)
	m_GameScoreDBModule->CloseRecordset();
	return true;
}

//写分请求
bool CDataBaseSink::OnRequestWriteUserScore(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//效验参数
		ASSERT(wDataSize==sizeof(DBR_GR_WriteUserScore));
		if (wDataSize!=sizeof(DBR_GR_WriteUserScore)) return false;

		//执行查询
		DBR_GR_WriteUserScore * pWriteUserScore=(DBR_GR_WriteUserScore *)pData;
		LONG lReturnValue=SPWriteUserScore(pWriteUserScore->dwUserID,   pWriteUserScore->dwPlayTimeCount, pWriteUserScore->lRevenue,
			                               pWriteUserScore->dwClientIP, pWriteUserScore->ScoreModifyInfo);
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);

		// add by HouGuoJiang 2012-03-27
		if(m_pRoomLog)
		{
			CString strErrorLine;
			strErrorLine.Format("函数名：%s  所在行：%d 文件名：%s",  __FUNCTION__ ,  __LINE__  , 	__FILE__ );

			CString strPrompt;
			strPrompt.Format("%s ,出错原因：%s ", strErrorLine.GetBuffer(), pszDescribe);
			
			m_pRoomLog->AddMsg(0, 0, TRUE, strPrompt.GetBuffer());
		}
	}

	return true;
}

//离开房间
bool CDataBaseSink::OnRequestLeaveGameServer(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//效验参数
		ASSERT(wDataSize==sizeof(DBR_GR_LeaveGameServer));
		if (wDataSize!=sizeof(DBR_GR_LeaveGameServer)) return false;

		//执行查询
		DBR_GR_LeaveGameServer * pLeaveGameServer=(DBR_GR_LeaveGameServer *)pData;
		LONG lReturnValue=SPLeaveGameServer(pLeaveGameServer->dwUserID,pLeaveGameServer->dwPlayTimeCount,pLeaveGameServer->dwOnlineTimeCount,
			pLeaveGameServer->dwClientIP,pLeaveGameServer->lRevenue,pLeaveGameServer->lLoveliness);
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);

		// add by HouGuoJiang 2012-03-27
		if(m_pRoomLog)
		{
			CString strErrorLine;
			strErrorLine.Format("函数名：%s  所在行：%d 文件名：%s",  __FUNCTION__ ,  __LINE__  , 	__FILE__ );

			CString strPrompt;
			strPrompt.Format("%s ,出错原因：%s ", strErrorLine.GetBuffer(), pszDescribe);

			m_pRoomLog->AddMsg(0, 0, TRUE, strPrompt.GetBuffer());
		}
	}

	return true;
}

//禁用帐户
bool CDataBaseSink::OnRequestLimitAccounts(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//效验参数
		ASSERT(wDataSize==sizeof(DBR_GR_LimitAccounts));
		if (wDataSize!=sizeof(DBR_GR_LimitAccounts)) return false;

		//执行查询
		DBR_GR_LimitAccounts * pLimitAccounts=(DBR_GR_LimitAccounts *)pData;
		LONG lReturnValue=SPCongealAccounts(pLimitAccounts->dwUserID,pLimitAccounts->dwMasterUserID,pLimitAccounts->dwMasterClientIP);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);

		// add by HouGuoJiang 2012-03-27
		if(m_pRoomLog)
		{
			CString strErrorLine;
			strErrorLine.Format("函数名：%s  所在行：%d 文件名：%s",  __FUNCTION__ ,  __LINE__  , 	__FILE__ );

			CString strPrompt;
			strPrompt.Format("%s ,出错原因：%s ", strErrorLine.GetBuffer(), pszDescribe);

			m_pRoomLog->AddMsg(0, 0, TRUE, strPrompt.GetBuffer());
		}
	}

	return true;
}

//设置权限
bool CDataBaseSink::OnRequestSetUserRight(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//效验参数
	ASSERT(wDataSize==sizeof(DBR_GR_SetUserRight));
	if (wDataSize!=sizeof(DBR_GR_SetUserRight)) return false;

	//执行查询
	DBR_GR_SetUserRight * pSetUserRight=(DBR_GR_SetUserRight *)pData;
	LONG lGameFailed = TRUE,lAccountsFailed = TRUE;

	try
	{
		//游戏权限
		if (pSetUserRight->cbGame==TRUE) 
		{
			lGameFailed = SPSetUserGameRight(pSetUserRight->dwUserID,pSetUserRight->dwUserRight,pSetUserRight->dwMasterUserID,pSetUserRight->dwMasterClientIP);
		}

		//帐号权限
		if (pSetUserRight->cbAccounts==TRUE) 
		{
			lAccountsFailed = SPSetUserAccountsRight(pSetUserRight->dwUserID,pSetUserRight->dwUserRight,pSetUserRight->dwMasterUserID,pSetUserRight->dwMasterClientIP);
		}

		//设置消息
		DBR_GR_UserRightResult UserRightResult;
		ZeroMemory(&UserRightResult,sizeof(UserRightResult));

		UserRightResult.dwUserRight = pSetUserRight->dwUserRight;
		UserRightResult.dwUserID = pSetUserRight->dwUserID;
		UserRightResult.dwMasterUserID = pSetUserRight->dwMasterUserID;
		UserRightResult.bGameSuccess=((lGameFailed==FALSE)?true:false);
		UserRightResult.bAccountsSuccess=((lAccountsFailed==FALSE)?true:false);

		//发送信息
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_GR_USER_RIGHT_RESULT,dwContextID,&UserRightResult,sizeof( UserRightResult ));
	
		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);

		// add by HouGuoJiang 2012-03-27
		if(m_pRoomLog)
		{
			CString strErrorLine;
			strErrorLine.Format("函数名：%s  所在行：%d 文件名：%s",  __FUNCTION__ ,  __LINE__  , 	__FILE__ );

			CString strPrompt;
			strPrompt.Format("%s ,出错原因：%s ", strErrorLine.GetBuffer(), pszDescribe);

			m_pRoomLog->AddMsg(0, 0, TRUE, strPrompt.GetBuffer());
		}

		//设置消息
		DBR_GR_UserRightResult UserRightResult;
		ZeroMemory(&UserRightResult,sizeof(UserRightResult));

		UserRightResult.dwUserRight = pSetUserRight->dwUserRight;
		UserRightResult.dwUserID = pSetUserRight->dwUserID;
		UserRightResult.dwMasterUserID = pSetUserRight->dwMasterUserID;
		UserRightResult.bGameSuccess=((lGameFailed==FALSE)?true:false);
		UserRightResult.bAccountsSuccess=((lAccountsFailed==FALSE)?true:false);

		//发送信息
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_GR_USER_RIGHT_RESULT,dwContextID,&UserRightResult,sizeof( UserRightResult ));
	}

	return true;
}


//提取金币
bool CDataBaseSink::OnBankDrawoutGold(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//效验参数
	ASSERT(wDataSize==sizeof(DBR_GR_BankDrawoutGold));
	if (wDataSize!=sizeof(DBR_GR_BankDrawoutGold)) return false;

	//参数转换
	DBR_GR_BankDrawoutGold * pBankGet=(DBR_GR_BankDrawoutGold *)pData;

	LONG lReturnValue = -1;
	try
	{
		//提取金币
		lReturnValue = SPBankDrawoutGold(pBankGet->dwUserID,pBankGet->DrawoutGoldCount,pBankGet->dwClientIP);
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);

		// add by HouGuoJiang 2012-03-27
		if(m_pRoomLog)
		{
			CString strErrorLine;
			strErrorLine.Format("函数名：%s  所在行：%d 文件名：%s",  __FUNCTION__ ,  __LINE__  , 	__FILE__ );

			CString strPrompt;
			strPrompt.Format("%s ,出错原因：%s ", strErrorLine.GetBuffer(), pszDescribe);

			m_pRoomLog->AddMsg(0, 0, TRUE, strPrompt.GetBuffer());
		}
	}

	return true;
}

//存储金币
bool CDataBaseSink::OnBankStorageGold(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//效验参数
	ASSERT(wDataSize==sizeof(DBR_GR_BankStorage));
	if (wDataSize!=sizeof(DBR_GR_BankStorage)) return false;

	//参数转换
	DBR_GR_BankStorage * pBankStorage=(DBR_GR_BankStorage *)pData;

	LONG lReturnValue = -1;
	try
	{
		//提取金币
		lReturnValue = SPBankStorageGold(pBankStorage->dwUserID,pBankStorage->lStorageCount,pBankStorage->dwClientIP);
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);

		// add by HouGuoJiang 2012-03-27
		if(m_pRoomLog)
		{
			CString strErrorLine;
			strErrorLine.Format("函数名：%s  所在行：%d 文件名：%s",  __FUNCTION__ ,  __LINE__  , 	__FILE__ );

			CString strPrompt;
			strPrompt.Format("%s ,出错原因：%s ", strErrorLine.GetBuffer(), pszDescribe);

			m_pRoomLog->AddMsg(0, 0, TRUE, strPrompt.GetBuffer());
		}
	}
	return true;
}

//读取寻宝任务
void CDataBaseSink::LoadFindGamesTask( DWORD userID, DWORD kindID)
{
	LONG lReturnValue = SPGetLoadFindGemsTask(userID, kindID);
	
	try
	{
		if (lReturnValue > 0)
		{
			//读取记录
			DBR_FIND_GEMS_TASK  task;
			memset(&task,  0, sizeof(task));

			task.iTaskID      =   m_AccountsDBAide.GetValue_INT(TEXT("dwTaskID"));
			m_AccountsDBAide.GetValue_String(TEXT("Title"), task.strTitle, CountArray(task.strTitle));
			m_AccountsDBAide.GetValue_String(TEXT("Context"), task.strContext, CountArray(task.strContext));

			task.iLargessCount1 = m_AccountsDBAide.GetValue_INT(TEXT("LargessCount1"));
			task.iLargessCount2 = m_AccountsDBAide.GetValue_INT(TEXT("LargessCount2"));
			task.iLargessCount3 = m_AccountsDBAide.GetValue_INT(TEXT("LargessCount3"));
			task.iLargessCount4 = m_AccountsDBAide.GetValue_INT(TEXT("LargessCount4"));
			task.iTaskUserLogID = m_AccountsDBAide.GetValue_INT(TEXT("TaskUserLogID"));
			m_AccountsDBAide.GetValue_String(TEXT("TaskUserLogTable"), task.strUserLogTable, CountArray(task.strUserLogTable));
			task.dwUserID       = m_AccountsDBAide.GetValue_DWORD(TEXT("dwUserID"));
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_GR_USER_FIND_GEMS_TASK_LOAD,
				0, 
				&task, 
				sizeof(DBR_FIND_GEMS_TASK));
		} 
	}
	catch (IDataBaseException * pIException)
	{
		// add by HouGuoJiang 2012-03-27
		if(m_pRoomLog)
		{
			//错误信息
			LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
			CTraceService::TraceString(pszDescribe,TraceLevel_Exception);

			CString strErrorLine;
			strErrorLine.Format("函数名：%s  所在行：%d 文件名：%s",  __FUNCTION__ ,  __LINE__  , 	__FILE__ );

			CString strPrompt;
			strPrompt.Format("%s ,出错原因：%s ", strErrorLine.GetBuffer(), pszDescribe);

			m_pRoomLog->AddMsg(0, 0, TRUE, strPrompt.GetBuffer());
		}
	}
	
	// add by HouGuoJiang 2012-03-27
	//关闭记录集，只要用到了记录集，就必需关闭
	m_AccountsDBModule->CloseRecordset();
}


//得到寻宝任务
LONG CDataBaseSink::SPGetLoadFindGemsTask(DWORD dwUserID, DWORD kindID)
 {
	try
	{
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@UserID"), dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@KindID"), kindID);
		return m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_GetTask"),true);
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);

		// add by HouGuoJiang 2012-03-27
		if(m_pRoomLog)
		{
			CString strErrorLine;
			strErrorLine.Format("函数名：%s  所在行：%d 文件名：%s",  __FUNCTION__ ,  __LINE__  , 	__FILE__ );

			CString strPrompt;
			strPrompt.Format("%s ,出错原因：%s ", strErrorLine.GetBuffer(), pszDescribe);

			m_pRoomLog->AddMsg(0, 0, TRUE, strPrompt.GetBuffer());
		}
		return -1;
	}
}

bool CDataBaseSink::OnFindGemsTaskPerformFinish(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		DBR_TASK_Finish* pTask = (DBR_TASK_Finish*)pData;
		ASSERT(pTask->dwUserID  !=0L);
		ASSERT(pTask->iTaskID   !=0L);
		
		if (pTask->bFinish)
		{
			m_AccountsDBAide.ResetParameter();
			m_AccountsDBAide.AddParameter(TEXT("@UserID"),			 pTask->dwUserID);
			m_AccountsDBAide.AddParameter(TEXT("@TaskID"),			 pTask->iTaskID);
			m_AccountsDBAide.AddParameter(TEXT("@TaskUserID"),		 pTask->iTaskUserLogID);
			m_AccountsDBAide.AddParameter(TEXT("@SererTypeID"),      pTask->dwKindID);
			m_AccountsDBAide.AddParameter(TEXT("@TaskUserLogTable"), pTask->strUserLogTable);
			m_AccountsDBAide.AddParameter(TEXT("@SererID"),          pTask->dwServerID);
			m_AccountsDBAide.AddParameter(TEXT("@IpAddr"),           pTask->szClientIP);	
			m_AccountsDBAide.AddParameter(TEXT("@SortID"),           pTask->dwSortID);
			m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_SetTaskEnd"),true);
			
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_GR_RE_FIND_GEMS_TASK_PERFORM_FINISH,
																								dwContextID,  pTask, sizeof(CMD_GF_TASK_tagTaskFinish));
		}

		LoadFindGamesTask(pTask->dwUserID, pTask->dwKindID);
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);

		// add by HouGuoJiang 2012-03-27
		if(m_pRoomLog)
		{
			CString strErrorLine;
			strErrorLine.Format("函数名：%s  所在行：%d 文件名：%s",  __FUNCTION__ ,  __LINE__  , 	__FILE__ );

			CString strPrompt;
			strPrompt.Format("%s ,出错原因：%s ", strErrorLine.GetBuffer(), pszDescribe);

			m_pRoomLog->AddMsg(0, 0, TRUE, strPrompt.GetBuffer());
		}
	}

	return true;
}


//I D 存储过程
LONG CDataBaseSink::SPLogonByUserID(DWORD dwUserID, LPCTSTR pszPassword, DWORD dwClientIP, LPCTSTR pszComputerID)
{
	//效验参数
	ASSERT(dwUserID!=0L);
	ASSERT(pszPassword!=NULL);
	try
	{
		//转化地址
		TCHAR szClientIP[16]=TEXT("");
		BYTE * pClientIP=(BYTE *)&dwClientIP;
		_snprintf(szClientIP,sizeof(szClientIP),TEXT("%d.%d.%d.%d"),pClientIP[0],pClientIP[1],pClientIP[2],pClientIP[3]);

		//执行存储过程
		m_GameScoreDBAide.ResetParameter();
		m_GameScoreDBAide.AddParameter(TEXT("@dwUserID"),dwUserID);
		m_GameScoreDBAide.AddParameter(TEXT("@strPassword"),pszPassword);
		m_GameScoreDBAide.AddParameter(TEXT("@strClientIP"),szClientIP);
		m_GameScoreDBAide.AddParameter(TEXT("@strMachineSerial"),pszComputerID);
		m_GameScoreDBAide.AddParameter(TEXT("@wKindID"),m_pGameServiceAttrib->wKindID);
		m_GameScoreDBAide.AddParameter(TEXT("@wServerID"),m_pGameServiceOption->wServerID);

		return m_GameScoreDBAide.ExecuteProcess(TEXT("GSP_GR_EfficacyUserID"),true);
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);

		// add by HouGuoJiang 2012-03-27
		if(m_pRoomLog)
		{
			CString strErrorLine;
			strErrorLine.Format("函数名：%s  所在行：%d 文件名：%s",  __FUNCTION__ ,  __LINE__  , 	__FILE__ );

			CString strPrompt;
			strPrompt.Format("%s ,出错原因：%s ", strErrorLine.GetBuffer(), pszDescribe);

			m_pRoomLog->AddMsg(0, 0, TRUE, strPrompt.GetBuffer());
		}
	}

	return -1;
}

//写分存储过程
LONG CDataBaseSink::SPWriteUserScore(DWORD dwUserID, DWORD dwPlayTimeCount, LONGLONG lRevenue,
									 DWORD dwClientIP, tagUserScore & UserScore)
{
	//效验参数
	ASSERT(dwUserID!=0L);

	try
	{
		//转化地址
		TCHAR szClientIP[16]=TEXT("");
		BYTE * pClientIP=(BYTE *)&dwClientIP;
		_snprintf(szClientIP,sizeof(szClientIP),TEXT("%d.%d.%d.%d"),pClientIP[0],pClientIP[1],pClientIP[2],pClientIP[3]);

		//执行存储过程
		m_GameScoreDBAide.ResetParameter();
		m_GameScoreDBAide.AddParameter(TEXT("@dwUserID"),dwUserID);
		m_GameScoreDBAide.AddParameter(TEXT("@lWinCount"),UserScore.lWinCount);
		m_GameScoreDBAide.AddParameter(TEXT("@lLostCount"),UserScore.lLostCount);
		m_GameScoreDBAide.AddParameter(TEXT("@lDrawCount"),UserScore.lDrawCount);
		m_GameScoreDBAide.AddParameter(TEXT("@lFleeCount"),UserScore.lFleeCount);
		m_GameScoreDBAide.AddParameter(TEXT("@lExperience"),UserScore.lExperience);
		m_GameScoreDBAide.AddParameter(TEXT("@wKindID"),m_pGameServiceAttrib->wKindID);
		m_GameScoreDBAide.AddParameter(TEXT("@wServerID"),m_pGameServiceOption->wServerID);
		m_GameScoreDBAide.AddParameter(TEXT("@strClientIP"),szClientIP);
		
		//彭文添加
		m_GameScoreDBAide.AddParameter(TEXT("@lGold"),    UserScore.lGold);
		m_GameScoreDBAide.AddParameter(TEXT("@lGem"),     UserScore.lGem);
		m_GameScoreDBAide.AddParameter(TEXT("@dwGrade"),  UserScore.dwGrade);

		m_GameScoreDBAide.AddParameter(TEXT("@lRevenue"),        lRevenue);
		m_GameScoreDBAide.AddParameter(TEXT("@dwPlayTimeCount"), dwPlayTimeCount);
		//添加结束
		return m_GameScoreDBAide.ExecuteProcess(TEXT("GSP_GR_WriteGameScore"),false);
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);

		// add by HouGuoJiang 2012-03-27
		if(m_pRoomLog)
		{
			CString strErrorLine;
			strErrorLine.Format("函数名：%s  所在行：%d 文件名：%s",  __FUNCTION__ ,  __LINE__  , 	__FILE__ );

			CString strPrompt;
			strPrompt.Format("%s ,出错原因：%s ", strErrorLine.GetBuffer(), pszDescribe);

			m_pRoomLog->AddMsg(0, 0, TRUE, strPrompt.GetBuffer());
		}
	}
	return -1;
}

//离开存储过程
LONG CDataBaseSink::SPLeaveGameServer(DWORD dwUserID, DWORD dwPlayTimeCount, DWORD dwOnLineTimeCount, DWORD dwClientIP, LONGLONG lRevenue, LONG lLoveliness)
{
	//效验参数
	ASSERT(dwUserID!=0L);

	try
	{
		//转化地址
		TCHAR szClientIP[16]=TEXT("");
		BYTE * pClientIP=(BYTE *)&dwClientIP;
		_snprintf(szClientIP,sizeof(szClientIP),TEXT("%d.%d.%d.%d"),pClientIP[0],pClientIP[1],pClientIP[2],pClientIP[3]);

		//执行存储过程
		m_GameScoreDBAide.ResetParameter();
		m_GameScoreDBAide.AddParameter(TEXT("@dwUserID"),dwUserID);
		m_GameScoreDBAide.AddParameter(TEXT("@lLoveliness"),lLoveliness);
		m_GameScoreDBAide.AddParameter(TEXT("@lRevenue"),lRevenue);
		m_GameScoreDBAide.AddParameter(TEXT("@dwPlayTimeCount"),dwPlayTimeCount);
		m_GameScoreDBAide.AddParameter(TEXT("@dwOnLineTimeCount"),dwOnLineTimeCount);
		m_GameScoreDBAide.AddParameter(TEXT("@wKindID"),m_pGameServiceAttrib->wKindID);
		m_GameScoreDBAide.AddParameter(TEXT("@wServerID"),m_pGameServiceOption->wServerID);
		m_GameScoreDBAide.AddParameter(TEXT("@strClientIP"),szClientIP);
		return m_GameScoreDBAide.ExecuteProcess(TEXT("GSP_GR_LeaveGameServer"),false);
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);

		// add by HouGuoJiang 2012-03-27
		if(m_pRoomLog)
		{
			CString strErrorLine;
			strErrorLine.Format("函数名：%s  所在行：%d 文件名：%s",  __FUNCTION__ ,  __LINE__  , 	__FILE__ );

			CString strPrompt;
			strPrompt.Format("%s ,出错原因：%s ", strErrorLine.GetBuffer(), pszDescribe);

			m_pRoomLog->AddMsg(0, 0, TRUE, strPrompt.GetBuffer());
		}
	}

	return -1;
}

//禁号存储过程
LONG CDataBaseSink::SPCongealAccounts(DWORD dwUserID, DWORD dwMasterUserID, DWORD dwClientIP)
{
	//效验参数
	ASSERT(dwUserID!=0L);

	try
	{
		//转化地址
		TCHAR szClientIP[16]=TEXT("");
		BYTE * pClientIP=(BYTE *)&dwClientIP;
		_snprintf(szClientIP,sizeof(szClientIP),TEXT("%d.%d.%d.%d"),pClientIP[0],pClientIP[1],pClientIP[2],pClientIP[3]);

		//执行存储过程
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"),dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@dwMasterUserID"),dwMasterUserID);
		m_AccountsDBAide.AddParameter(TEXT("@strClientIP"),szClientIP);
		return m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GR_CongealAccounts"),false);
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);

		// add by HouGuoJiang 2012-03-27
		if(m_pRoomLog)
		{
			CString strErrorLine;
			strErrorLine.Format("函数名：%s  所在行：%d 文件名：%s",  __FUNCTION__ ,  __LINE__  , 	__FILE__ );

			CString strPrompt;
			strPrompt.Format("%s ,出错原因：%s ", strErrorLine.GetBuffer(), pszDescribe);

			m_pRoomLog->AddMsg(0, 0, TRUE, strPrompt.GetBuffer());
		}
	}
	return -1;
}

//权限存储过程
LONG CDataBaseSink::SPSetUserGameRight(DWORD dwUserID, DWORD dwUserRight, DWORD dwMasterUserID, DWORD dwClientIP)
{
	//效验参数
	ASSERT(dwUserID!=0L);
	
	try
	{
		//转化地址
		TCHAR szClientIP[16]=TEXT("");
		BYTE * pClientIP=(BYTE *)&dwClientIP;
		_snprintf(szClientIP,sizeof(szClientIP),TEXT("%d.%d.%d.%d"),pClientIP[0],pClientIP[1],pClientIP[2],pClientIP[3]);

		//执行存储过程
		m_GameScoreDBAide.ResetParameter();
		m_GameScoreDBAide.AddParameter(TEXT("@dwUserID"),dwUserID);
		m_GameScoreDBAide.AddParameter(TEXT("@dwUserRight"),dwUserRight);
		m_GameScoreDBAide.AddParameter(TEXT("@dwMasterUserID"),dwMasterUserID);
		m_GameScoreDBAide.AddParameter(TEXT("@strClientIP"),szClientIP);

		return m_GameScoreDBAide.ExecuteProcess(TEXT("GSP_GR_SetUserRight"),false);
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);

		// add by HouGuoJiang 2012-03-27
		if(m_pRoomLog)
		{
			CString strErrorLine;
			strErrorLine.Format("函数名：%s  所在行：%d 文件名：%s",  __FUNCTION__ ,  __LINE__  , 	__FILE__ );

			CString strPrompt;
			strPrompt.Format("%s ,出错原因：%s ", strErrorLine.GetBuffer(), pszDescribe);

			m_pRoomLog->AddMsg(0, 0, TRUE, strPrompt.GetBuffer());
		}
	}
	return -1;
}

//权限存储过程
LONG CDataBaseSink::SPSetUserAccountsRight(DWORD dwUserID, DWORD dwUserRight, DWORD dwMasterUserID, DWORD dwClientIP)
{
	//效验参数
	ASSERT(dwUserID!=0L);
	try
	{
		//转化地址
		TCHAR szClientIP[16]=TEXT("");
		BYTE * pClientIP=(BYTE *)&dwClientIP;
		_snprintf(szClientIP,sizeof(szClientIP),TEXT("%d.%d.%d.%d"),pClientIP[0],pClientIP[1],pClientIP[2],pClientIP[3]);

		//执行存储过程
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"),dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@dwUserRight"),dwUserRight);
		m_AccountsDBAide.AddParameter(TEXT("@dwMasterUserID"),dwMasterUserID);
		m_AccountsDBAide.AddParameter(TEXT("@strClientIP"),szClientIP);

		return m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GR_SetUserRight"),false);
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);

		// add by HouGuoJiang 2012-03-27
		if(m_pRoomLog)
		{
			CString strErrorLine;
			strErrorLine.Format("函数名：%s  所在行：%d 文件名：%s",  __FUNCTION__ ,  __LINE__  , 	__FILE__ );

			CString strPrompt;
			strPrompt.Format("%s ,出错原因：%s ", strErrorLine.GetBuffer(), pszDescribe);

			m_pRoomLog->AddMsg(0, 0, TRUE, strPrompt.GetBuffer());
		}
	}
	return -1;
}

//记录赠送
LONG CDataBaseSink::SPRecordGiftGrant(DWORD dwSendUserID, DWORD dwRcvUserID, WORD wGiftID, DWORD dwClientIP, 
									  DWORD dwSendUserLoveliness, DWORD dwRcvUserLoveliness, LONG lValue, int nFlowerCount)
{
	ASSERT( dwSendUserID != 0 && dwRcvUserID != 0 );
	try
	{
		//转化地址
		TCHAR szClientIP[16]=TEXT("");
		BYTE * pClientIP=(BYTE *)&dwClientIP;
		_snprintf(szClientIP,sizeof(szClientIP),TEXT("%d.%d.%d.%d"),pClientIP[0],pClientIP[1],pClientIP[2],pClientIP[3]);

		//执行存储过程
		m_GameScoreDBAide.ResetParameter();
		m_GameScoreDBAide.AddParameter(TEXT("@dwSendUserID"),dwSendUserID);
		m_GameScoreDBAide.AddParameter(TEXT("@dwRcvUserID"),dwRcvUserID);
		m_GameScoreDBAide.AddParameter(TEXT("@dwSendUserLoveliness"),dwSendUserLoveliness);
		m_GameScoreDBAide.AddParameter(TEXT("@dwRcvUserLoveliness"),dwRcvUserLoveliness);
		m_GameScoreDBAide.AddParameter(TEXT("@dwFlowerCount"),(DWORD)nFlowerCount);
		m_GameScoreDBAide.AddParameter(TEXT("@strFlowerName"),g_FlowerDescribe[wGiftID].szName);
		m_GameScoreDBAide.AddParameter(TEXT("@lFlowerPay"),lValue);
		m_GameScoreDBAide.AddParameter(TEXT("@wKindID"),m_pGameServiceAttrib->wKindID);
		m_GameScoreDBAide.AddParameter(TEXT("@wServerID"),m_pGameServiceOption->wServerID);
		m_GameScoreDBAide.AddParameter(TEXT("@strClientIP"),szClientIP);

		return m_GameScoreDBAide.ExecuteProcess(TEXT("GSP_GR_RecordFlowerGrant"),true);
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);

		// add by HouGuoJiang 2012-03-27
		if(m_pRoomLog)
		{
			CString strErrorLine;
			strErrorLine.Format("函数名：%s  所在行：%d 文件名：%s",  __FUNCTION__ ,  __LINE__  , 	__FILE__ );

			CString strPrompt;
			strPrompt.Format("%s ,出错原因：%s ", strErrorLine.GetBuffer(), pszDescribe);

			m_pRoomLog->AddMsg(0, 0, TRUE, strPrompt.GetBuffer());
		}
	}
	return -1;
}

//购买道具
LONG CDataBaseSink::SPBuyProperty(DWORD dwUserID, DWORD dwTargetUserID, int nPropertyID, DWORD dwCurCount, DWORD dwOnceCount, 
								  DWORD dwPachurCount, LONG lPrice, DWORD dwClientIP)
{
	ASSERT( dwUserID != 0 );

	try
	{
		//转化地址
		TCHAR szClientIP[16]=TEXT("");
		BYTE * pClientIP=(BYTE *)&dwClientIP;
		_snprintf(szClientIP,sizeof(szClientIP),TEXT("%d.%d.%d.%d"),pClientIP[0],pClientIP[1],pClientIP[2],pClientIP[3]);

		//执行存储过程
		m_GameScoreDBAide.ResetParameter();
		m_GameScoreDBAide.AddParameter(TEXT("@dwUserID"),dwUserID);
		m_GameScoreDBAide.AddParameter(TEXT("@dwRcvUserID"),dwTargetUserID);
		m_GameScoreDBAide.AddParameter(TEXT("@dwKindID"),(DWORD)m_pGameServiceAttrib->wKindID);
		m_GameScoreDBAide.AddParameter(TEXT("@wServerID"),m_pGameServiceOption->wServerID);
		m_GameScoreDBAide.AddParameter(TEXT("@dwCateID"),(DWORD)nPropertyID);
		m_GameScoreDBAide.AddParameter(TEXT("@dwCurCount"),dwCurCount);	
		m_GameScoreDBAide.AddParameter(TEXT("@dwOnceCount"),dwOnceCount);	
		m_GameScoreDBAide.AddParameter(TEXT("@dwPachurseCount"),dwPachurCount);
		m_GameScoreDBAide.AddParameter(TEXT("@bwSpendScore"),lPrice);	
		m_GameScoreDBAide.AddParameter(TEXT("@strClientIP"),szClientIP);

		return m_GameScoreDBAide.ExecuteProcess(TEXT("GSP_GR_PurchaseProp"),true);
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);

		// add by HouGuoJiang 2012-03-27
		if(m_pRoomLog)
		{
			CString strErrorLine;
			strErrorLine.Format("函数名：%s  所在行：%d 文件名：%s",  __FUNCTION__ ,  __LINE__  , 	__FILE__ );

			CString strPrompt;
			strPrompt.Format("%s ,出错原因：%s ", strErrorLine.GetBuffer(), pszDescribe);

			m_pRoomLog->AddMsg(0, 0, TRUE, strPrompt.GetBuffer());
		}
	}
	return -1;
}

//写入道具
LONG CDataBaseSink::SPWriteProperty(DWORD dwUserID, int  nPropertyID, DWORD lUseableTime, 
									DWORD dwLeftTime, bool bUse,   
									DWORD dwClientIP)
{
	ASSERT( dwUserID != 0 );

	try
	{
		//转化地址
		TCHAR szClientIP[16]=TEXT("");
		BYTE * pClientIP=(BYTE *)&dwClientIP;
		_snprintf(szClientIP,sizeof(szClientIP),TEXT("%d.%d.%d.%d"),pClientIP[0],pClientIP[1],pClientIP[2],pClientIP[3]);

		//执行存储过程
		m_GameScoreDBAide.ResetParameter();
		m_GameScoreDBAide.AddParameter(TEXT("@dwUserID"),dwUserID);

		//喇叭判断
		if ( nPropertyID == PROP_BUGLE )
			m_GameScoreDBAide.AddParameter(TEXT("@dwKindID"),0);
		else
			m_GameScoreDBAide.AddParameter(TEXT("@dwKindID"),0); //彭文添加

		m_GameScoreDBAide.AddParameter(TEXT("@dwCateID"),nPropertyID);
		m_GameScoreDBAide.AddParameter(TEXT("@dwPropCount"),lUseableTime);
		m_GameScoreDBAide.AddParameter(TEXT("@strClientIP"),szClientIP);
		m_GameScoreDBAide.AddParameter(TEXT("bUse"), (int)bUse);
		m_GameScoreDBAide.AddParameter(TEXT("dwLeftTime"), dwLeftTime);

		return m_GameScoreDBAide.ExecuteProcess(TEXT("GSP_GR_WriteUserProp"),false);
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);

		// add by HouGuoJiang 2012-03-27
		if(m_pRoomLog)
		{
			CString strErrorLine;
			strErrorLine.Format("函数名：%s  所在行：%d 文件名：%s",  __FUNCTION__ ,  __LINE__  , 	__FILE__ );

			CString strPrompt;
			strPrompt.Format("%s ,出错原因：%s ", strErrorLine.GetBuffer(), pszDescribe);

			m_pRoomLog->AddMsg(0, 0, TRUE, strPrompt.GetBuffer());
		}
	}
	return -1;
}

//提取存储过程
LONG CDataBaseSink::SPBankDrawoutGold(DWORD dwUserID, LONGLONG lSwapScore, DWORD dwClientIP)
{
	ASSERT( dwUserID != 0 );
	try
	{
		//转化地址
		TCHAR szClientIP[16]=TEXT("");
		BYTE * pClientIP=(BYTE *)&dwClientIP;
		_snprintf(szClientIP,sizeof(szClientIP),TEXT("%d.%d.%d.%d"),pClientIP[0],pClientIP[1],pClientIP[2],pClientIP[3]);

		//执行存储过程
		m_GameScoreDBAide.ResetParameter();
		m_GameScoreDBAide.AddParameter(TEXT("@dwUserID"),dwUserID);
		m_GameScoreDBAide.AddParameter(TEXT("@lSwapScore"),lSwapScore);
		m_GameScoreDBAide.AddParameter(TEXT("@dwKindID"),m_pGameServiceAttrib->wKindID);
		m_GameScoreDBAide.AddParameter(TEXT("@dwServerID"),m_pGameServiceOption->wServerID);
		m_GameScoreDBAide.AddParameter(TEXT("@strClientIP"),szClientIP);

		return m_GameScoreDBAide.ExecuteProcess(TEXT("GSP_GR_DrawoutGameGold"),false);
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);

		// add by HouGuoJiang 2012-03-27
		if(m_pRoomLog)
		{
			CString strErrorLine;
			strErrorLine.Format("函数名：%s  所在行：%d 文件名：%s",  __FUNCTION__ ,  __LINE__  , 	__FILE__ );

			CString strPrompt;
			strPrompt.Format("%s ,出错原因：%s ", strErrorLine.GetBuffer(), pszDescribe);

			m_pRoomLog->AddMsg(0, 0, TRUE, strPrompt.GetBuffer());
		}
	}
	return -1;
}

//存金存储过程
LONG CDataBaseSink::SPBankStorageGold(DWORD dwUserID, LONGLONG lSwapScore, DWORD dwClientIP)
{
	ASSERT( dwUserID != 0 );
	try
	{
		//转化地址
		TCHAR szClientIP[16]=TEXT("");
		BYTE * pClientIP=(BYTE *)&dwClientIP;
		_snprintf(szClientIP,sizeof(szClientIP),TEXT("%d.%d.%d.%d"),pClientIP[0],pClientIP[1],pClientIP[2],pClientIP[3]);

		//执行存储过程
		m_GameScoreDBAide.ResetParameter();
		m_GameScoreDBAide.AddParameter(TEXT("@dwUserID"),dwUserID);
		m_GameScoreDBAide.AddParameter(TEXT("@lSwapScore"),lSwapScore);
		m_GameScoreDBAide.AddParameter(TEXT("@dwKindID"),m_pGameServiceAttrib->wKindID);
		m_GameScoreDBAide.AddParameter(TEXT("@dwServerID"),m_pGameServiceOption->wServerID);
		m_GameScoreDBAide.AddParameter(TEXT("@strClientIP"),szClientIP);

		return m_GameScoreDBAide.ExecuteProcess(TEXT("GSP_GR_SavingGameGold"),false);
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);

		// add by HouGuoJiang 2012-03-27
		if(m_pRoomLog)
		{
			CString strErrorLine;
			strErrorLine.Format("函数名：%s  所在行：%d 文件名：%s",  __FUNCTION__ ,  __LINE__  , 	__FILE__ );

			CString strPrompt;
			strPrompt.Format("%s ,出错原因：%s ", strErrorLine.GetBuffer(), pszDescribe);

			m_pRoomLog->AddMsg(0, 0, TRUE, strPrompt.GetBuffer());
		}
	}
	return -1;
}

//处理公告
bool CDataBaseSink::OnAffiche(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_GR_AFFICHE_SYNC, dwContextID, NULL, 0);
	    m_AccountsDBAide.ResetParameter();
       m_AccountsDBAide.AddParameter(TEXT("@state"),  3);
		long ret = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_GetPlacard"), true);
		if (ret > 0)
		{
			WORD dwCount = 1; 
			DBR_GR_affiche  affice;
			ZeroMemory(&affice, sizeof(DBR_GR_affiche));
			//读取记录
			m_AccountsDBModule->MoveToFirst();
			while ( m_AccountsDBModule->IsRecordsetEnd() == false )
			{
				affice.dwID = dwCount++;
				m_AccountsDBAide.GetValue_String(TEXT("Context"),affice.szInfo, CountArray(affice.szInfo));
				//投递调度通知
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_GR_AFFICHE_ONE, dwContextID, &affice, sizeof(DBR_GR_affiche));
				//移动记录
				m_AccountsDBModule->MoveToNext();
			}
		}
		else
		{
			#if _SHOW_MESSAGE_NOR
					CTraceService::TraceString("没有公告", TraceLevel_Normal);
			#endif	
		}
    }
	catch (IDataBaseException * pIException)
	{
		//错误信息
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe(); 
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);

		// add by HouGuoJiang 2012-03-27
		if(m_pRoomLog)
		{
			CString strErrorLine;
			strErrorLine.Format("函数名：%s  所在行：%d 文件名：%s",  __FUNCTION__ ,  __LINE__  , 	__FILE__ );

			CString strPrompt;
			strPrompt.Format("%s ,出错原因：%s ", strErrorLine.GetBuffer(), pszDescribe);

			m_pRoomLog->AddMsg(0, 0, TRUE, strPrompt.GetBuffer());
		}
	}

	//关闭记录集)
	m_AccountsDBModule->CloseRecordset();
	return true;
}

//处理喜报
bool CDataBaseSink::OnHappyMess(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_GR_HAPPY_MESS_SYNC, dwContextID, NULL, 0);
		m_AccountsDBAide.ResetParameter();
	    m_AccountsDBAide.AddParameter(TEXT("@state"),  2);
		long ret = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_GetPlacard"), true);
		if (ret > 0)
		{
			WORD dwCount = 1; 
			DBR_GR_happyMess    affice;
			ZeroMemory(&affice, sizeof(DBR_GR_happyMess));
			//读取记录
			m_AccountsDBModule->MoveToFirst();
			while ( m_AccountsDBModule->IsRecordsetEnd() == false )
			{
				affice.dwID = dwCount++;
				m_AccountsDBAide.GetValue_String(TEXT("Context"),affice.szInfo, CountArray(affice.szInfo));
				//投递调度通知
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_GR_HAPPY_MESS_ONE, dwContextID, &affice, sizeof(DBR_GR_affiche));
				//移动记录
				m_AccountsDBModule->MoveToNext();
			}
		}
		else
		{
			#if _SHOW_MESSAGE_NOR
						CTraceService::TraceString("没有喜报", TraceLevel_Normal);
			#endif	
		}
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe(); 
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);

		// add by HouGuoJiang 2012-03-27
		if(m_pRoomLog)
		{
			CString strErrorLine;
			strErrorLine.Format("函数名：%s  所在行：%d 文件名：%s",  __FUNCTION__ ,  __LINE__  , 	__FILE__ );

			CString strPrompt;
			strPrompt.Format("%s ,出错原因：%s ", strErrorLine.GetBuffer(), pszDescribe);

			m_pRoomLog->AddMsg(0, 0, TRUE, strPrompt.GetBuffer());
		}
	}
	
	//关闭记录集)
	m_AccountsDBModule->CloseRecordset();
	return true;
}

//读取等级信息
bool CDataBaseSink::OnReadGradeInfo( DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		LONG reValue = 0;
		m_AccountsDBAide.ResetParameter();
		reValue = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_GetGradeList"),true);
		
		if (reValue > 0)
		{
			CCGlobalData::AllocGradeInfoMemory(reValue);

			int   id = 0;
			TCHAR szName[24] = {0};
			int   exp = 0;
			//读取记录
			m_AccountsDBModule->MoveToFirst();
			while ( m_AccountsDBModule->IsRecordsetEnd() == false )
			{
				int id = m_AccountsDBAide.GetValue_INT("ID");
				m_AccountsDBAide.GetValue_String(TEXT("Name"), szName, CountArray(szName));
				exp = m_AccountsDBAide.GetValue_INT("Experience");
				CCGlobalData::SetGradeInfoData(id, szName, exp);
				m_AccountsDBModule->MoveToNext();
			}
		}
		else
		{
			CTraceService::TraceString("没有等级信息", TraceLevel_Normal);
		}
	}
	catch( IDataBaseException * pIException)
	{
		//错误信息
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);

		// add by HouGuoJiang 2012-03-27
		if(m_pRoomLog)
		{
			CString strErrorLine;
			strErrorLine.Format("函数名：%s  所在行：%d 文件名：%s",  __FUNCTION__ ,  __LINE__  , 	__FILE__ );

			CString strPrompt;
			strPrompt.Format("%s ,出错原因：%s ", strErrorLine.GetBuffer(), pszDescribe);

			m_pRoomLog->AddMsg(0, 0, TRUE, strPrompt.GetBuffer());
		}
	}

	//关闭记录集)
	m_AccountsDBModule->CloseRecordset();
	return true;
}

//用户解锁
bool CDataBaseSink::OnUnLockUser(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//效验参数
		ASSERT(wDataSize==sizeof(DWORD));
		if (wDataSize!=sizeof(DWORD)) return false;

		//执行查询
		DWORD userID =*((DWORD*)pData);
		m_GameScoreDBAide.ResetParameter();
		m_GameScoreDBAide.AddParameter(TEXT("@UserID"), userID);
		long ret = m_GameScoreDBAide.ExecuteProcess(TEXT("GSP_GR_DeleteUserLocker"), true);
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);

		// add by HouGuoJiang 2012-03-27
		if(m_pRoomLog)
		{
			CString strErrorLine;
			strErrorLine.Format("函数名：%s  所在行：%d 文件名：%s",  __FUNCTION__ ,  __LINE__  , 	__FILE__ );

			CString strPrompt;
			strPrompt.Format("%s ,出错原因：%s ", strErrorLine.GetBuffer(), pszDescribe);

			m_pRoomLog->AddMsg(0, 0, TRUE, strPrompt.GetBuffer());
		}
	}

	return true;
}

//	Author: <cxf>
//	CrateDate: <2011-12-22>
//	Description: <保持数据库连接池一直连接>
bool CDataBaseSink::ContinueConnect()
{
	try
	{
		//执行存储过程
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_ContinueConnect"), true);
		return true;
	}
	catch(IDataBaseException * pIException)
	{
		//输出错误
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);

		// add by HouGuoJiang 2012-03-27
		if(m_pRoomLog)
		{
			CString strErrorLine;
			strErrorLine.Format("函数名：%s  所在行：%d 文件名：%s",  __FUNCTION__ ,  __LINE__  , 	__FILE__ );

			CString strPrompt;
			strPrompt.Format("%s ,出错原因：%s ", strErrorLine.GetBuffer(), pszDescribe);

			m_pRoomLog->AddMsg(0, 0, TRUE, strPrompt.GetBuffer());
		}
	}

	return false;
}

// 设置日志指针
void CDataBaseSink::SetRoomLog(cnd_ILogMgr*	pRoomLog)
{
	m_pRoomLog = pRoomLog;;	// 房间日志
}

//新手赠送<2012-02-07>
bool CDataBaseSink::NewGiftUser(VOID * pData,WORD wDataSize)
{
	try
	{
		//执行存储过程
		//效验参数
		ASSERT(wDataSize==sizeof(CMD_GF_NEW_GIFT));
		if (wDataSize!=sizeof(CMD_GF_NEW_GIFT)) return false;

		//执行查询
		CMD_GF_NEW_GIFT * pNewGift=(CMD_GF_NEW_GIFT *)pData;
		//转化地址
		TCHAR szClientIP[16]=TEXT("");
		BYTE * pClientIP=(BYTE *)&pNewGift->dwClientIP;
		_snprintf(szClientIP,sizeof(szClientIP),TEXT("%d.%d.%d.%d"),pClientIP[0],pClientIP[1],pClientIP[2],pClientIP[3]);

		m_GameScoreDBAide.ResetParameter();
		m_GameScoreDBAide.AddParameter(TEXT("@UserID"),  pNewGift->wUserID);
		m_GameScoreDBAide.AddParameter(TEXT("@Type"),  pNewGift->wGiftType);
		m_GameScoreDBAide.AddParameter(TEXT("@Gold"),  pNewGift->iGold);
		m_GameScoreDBAide.AddParameter(TEXT("@gem"),  pNewGift->iGem);
		m_GameScoreDBAide.AddParameter(TEXT("@ServerID"),m_pGameServiceOption->wServerID);
		m_GameScoreDBAide.AddParameter(TEXT("@IpAddress"),  szClientIP);
		m_GameScoreDBAide.ExecuteProcess(TEXT("GSP_GR_NewGift"), true);
		return true;
	}
	catch(IDataBaseException * pIException)
	{
		//输出错误
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);

		// add by HouGuoJiang 2012-03-27
		if(m_pRoomLog)
		{
			CString strErrorLine;
			strErrorLine.Format("函数名：%s  所在行：%d 文件名：%s",  __FUNCTION__ ,  __LINE__  , 	__FILE__ );

			CString strPrompt;
			strPrompt.Format("%s ,出错原因：%s ", strErrorLine.GetBuffer(), pszDescribe);

			m_pRoomLog->AddMsg(0, 0, TRUE, strPrompt.GetBuffer());
		}
	}
	return false;
}

//Author<cxf>;DateTime<2012-03-02>
//Description<读取某游戏：输赢逃跑总局数>
LONG CDataBaseSink::SPDayWinLostCount(DWORD dwUserID)
{
	//效验参数
	ASSERT(dwUserID!=0L);

	try
	{
		//执行存储过程
		m_GameScoreDBAide.ResetParameter();
		m_GameScoreDBAide.AddParameter(TEXT("@wKindID"),m_pGameServiceAttrib->wKindID);
		m_GameScoreDBAide.AddParameter(TEXT("@dwUserID"),dwUserID);

		return m_GameScoreDBAide.ExecuteProcess(TEXT("GSP_GP_DayWinLoseCount"),true);
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);

		// add by HouGuoJiang 2012-03-27
		if(m_pRoomLog)
		{
			CString strErrorLine;
			strErrorLine.Format("函数名：%s  所在行：%d 文件名：%s",  __FUNCTION__ ,  __LINE__  , 	__FILE__ );

			CString strPrompt;
			strPrompt.Format("%s ,出错原因：%s ", strErrorLine.GetBuffer(), pszDescribe);

			m_pRoomLog->AddMsg(0, 0, TRUE, strPrompt.GetBuffer());
		}
	}

	return -1;
}
