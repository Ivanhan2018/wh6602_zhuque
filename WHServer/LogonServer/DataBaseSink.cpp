#include "StdAfx.h"
#include "Commonality.h"
#include "DataBaseSink.h"
#include "LogonServer.h"


//声明变量
extern CLogonServerApp theApp;
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
	case DBR_GP_LOGON_BY_ACCOUNTS:		//帐号登录
		{
			return OnRequestLogon(wRequestID,dwContextID,pData,wDataSize);
		}
	case DBR_GP_LOGON_BY_USERID:		//I D 登录
		{
			return OnRequestLogon(wRequestID,dwContextID,pData,wDataSize);
		}
	case DBR_GP_REGISTER_ACCOUNTS:		//注册帐号
		{
			return OnRequestLogon(wRequestID,dwContextID,pData,wDataSize);
		}
	case DBR_GP_REGISTER_ACCOUNTS_WX:		//注册帐号
		{
			return OnRequestLogon(wRequestID,dwContextID,pData,wDataSize);
		}
	case DBR_GP_WXUPDATE:		//升级成WX帐号
		{
			return OnRequestLogon(wRequestID,dwContextID,pData,wDataSize);
		}
	case DBR_GP_USER_SIGNIN:		//签到
		{
			return OnRequestUserSignIn(wRequestID,dwContextID,pData,wDataSize);
		}
	case DBR_GP_USER_GETAWARD:		//领奖
		{
			return OnRequestUserGetAward(wRequestID,dwContextID,pData,wDataSize);
		}
	case DBR_GP_USER_EXCHANGE_INFO:		//兑换的个人信息
		{
			return OnRequestUserExchangeInfo(wRequestID,dwContextID,pData,wDataSize);
		}
	case DBR_GP_USER_EXCHANGE_PRODUCT:		//兑换商品
		{
			return OnRequestUserExchangeProduct(wRequestID,dwContextID,pData,wDataSize);
		}
	case DBR_GP_USER_EXCHANGE_PRODUCT_V2:		//兑换商品
		{
			return OnRequestUserExchangeProduct_V2(wRequestID,dwContextID,pData,wDataSize);
		}
	case DBR_GP_USER_EXCHANGE:		//兑换
		{
			return OnRequestUserExchange(wRequestID,dwContextID,pData,wDataSize);
		}
	case DBR_GP_USER_EXCHANGE_V2:		//兑换
		{
			return OnRequestUserExchange_V2(wRequestID,dwContextID,pData,wDataSize);
		}
	case DBR_GP_USER_EXCHANGE_RECORD:		//兑换记录
		{
			return OnRequestUserExchangeRecord(wRequestID,dwContextID,pData,wDataSize);
		}
	case DBR_GP_USER_UPLOAD_FACE:		//上传头像
		{
			return OnRequestUploadFace(wRequestID,dwContextID,pData,wDataSize);
		}
	case DBR_GP_DOWNLOAD_FACE:			//下载头像
		{
			return OnRequestDownloadFace(wRequestID,dwContextID,pData,wDataSize);
		}
	case DBR_GP_DELETE_FACE:			//删除消息
		{
			return OnRequestCustomFaceDelete(wRequestID,dwContextID,pData,wDataSize);
		}
	case DBR_GP_MODIFY_INDIVIDUAL:		//修改资料
		{
			return OnRequestModifyIndividual(wRequestID,dwContextID,pData,wDataSize);
		}
	case DBR_GP_MODIFY_UNDERWRITE:		//修改个性签名
		{
			return OnRequestModifyUnderWrite(wRequestID,dwContextID,pData,wDataSize);
		}
	case DBR_GP_MODIFY_PHONE:		    //修改手机号
		{
			return OnRequestModifyPhone(wRequestID,dwContextID,pData,wDataSize);
		}
	case DBR_GP_CHECKID:				//校验名称
		{
			return OnRequestCheckID(wRequestID,dwContextID,pData,wDataSize);
		}
	case DBR_GP_CHECK_NICKNAME:			//校验昵称
		{
			return OnRequestCheckNickName(wRequestID,dwContextID,pData,wDataSize);
		}
	case DBR_GP_REGISTER:				//注册
		{
			return OnRequestRegister(wRequestID,dwContextID,pData,wDataSize);
		}
	case DBR_GP_ID_UPDATE:				//帐号升级
		{
			return OnRequestIDUpdate(wRequestID,dwContextID,pData,wDataSize);
		}
	case DBR_GP_RESET_PWD:				//设置密码
		{
			return OnRequestResetPwd(wRequestID,dwContextID,pData,wDataSize);
		}
	case DBR_GP_USER_INFO:				//用户信息
		{
			return OnRequestUserInfo(wRequestID,dwContextID,pData,wDataSize);
		}
	case DBR_GP_USER_CAN_SPREAD:				//用户能否领推广奖励
		{
			return OnRequestUserCanSpread(wRequestID,dwContextID,pData,wDataSize);
		}
	case DBR_GP_USER_GET_SPREAD:				//用户领推广奖励
		{
			return OnRequestUserGetSpread(wRequestID,dwContextID,pData,wDataSize);
		}
	case DBR_GP_USER_SPREADER_INFO:				//推广员信息
		{
			return OnRequestSpreaderInfo(wRequestID,dwContextID,pData,wDataSize);
		}
	case DBR_GP_USER_GET_BBS_POP:				//弹出公告信息
		{
			return OnRequestGetBbsPop(wRequestID,dwContextID,pData,wDataSize);
		}
	case DBR_GP_USER_GET_BBS_SCROLL:			//滚动公告信息
		{
			return OnRequestGetBbsScroll(wRequestID,dwContextID,pData,wDataSize);
		}
	case DBR_GP_USER_GET_BBS_LIST:			//列表公告信息
		{
			return OnRequestGetBbsList(wRequestID,dwContextID,pData,wDataSize);
		}
	case DBR_GP_USER_GET_TASK_INFO:			//玩家任务信息
		{
			return OnRequestGetTaskInfo(wRequestID,dwContextID,pData,wDataSize);
		}
	case DBR_GP_USER_GET_BANKING:			//玩家排行榜
		{
			return OnRequestGetRanking(wRequestID,dwContextID,pData,wDataSize);
		}
	case DBR_GP_USER_GET_BANKING_V2:			//玩家排行榜
		{
			return OnRequestGetRanking_V2(wRequestID,dwContextID,pData,wDataSize);
		}
	case DBR_GP_USER_MODIFY_RANKINGSTATUS:			//玩家是否入排行榜
		{
			return OnRequestModifyRankingStatus(wRequestID,dwContextID,pData,wDataSize);
		}
	case DBR_GP_USER_FEEDBACK:			//玩家反馈意见
		{
			return OnRequestFeedBack(wRequestID,dwContextID,pData,wDataSize);
		}
	case DBR_GP_USER_GET_FEEDBACK:		//查询玩家反馈意见
		{
			return OnRequestGetFeedBack(wRequestID,dwContextID,pData,wDataSize);
		}
	case DBR_GP_USER_INFO_ID_CARD:		//查询用户身份证账号信息
		{
			return OnRequestUserInfoIDCard(wRequestID,dwContextID,pData,wDataSize);
		}
	case DBR_GP_USER_AUTHENTICATION:		//请求提交身份证信息(姓名及身份证号)
		{
			return OnRequestUserAuthentication(wRequestID,dwContextID,pData,wDataSize);
		}
	case DBR_GP_USER_MALL_PRODUCT:	//请求商城商品信息
		{
			return OnRequestMallProduct(wRequestID, dwContextID, pData, wDataSize);
		}
	case DBR_GP_USER_MALL_PRODUCT_IOS:	//请求商城商品信息
		{
			return OnRequestMallProduct(wRequestID, dwContextID, pData, wDataSize, true);
		}
	case DBR_GP_USER_MALL_PLACE_ORDER:	//请求商城订单
		{
			return OnRequestMallPlaceOrder(wRequestID, dwContextID, pData, wDataSize);
		}
	case DBR_GP_USER_BANK_INFO:	//查询玩家银行信息
		{
			return OnRequestUserBankInfo(wRequestID, dwContextID, pData, wDataSize);
		}
	case DBR_GP_USER_BANK_CHARGE:	//玩家操作银行
		{
			return OnRequestUserBankCharge(wRequestID, dwContextID, pData, wDataSize);
		}
	case DBR_GP_USER_CANCEL_ORDER:
		{
			return OnRequestUserCancelOrder(wRequestID, dwContextID, pData, wDataSize);
		}
	case DBR_GP_USER_MALL_BUY:
		{
			return OnRequestMallBuy(wRequestID, dwContextID, pData, wDataSize);
		}
	case DBR_GP_USER_MALL_UPDATE:
		{
			return OnRequestMallUpdateBuy(wRequestID, dwContextID, pData, wDataSize);
		}
	case DBR_GP_USER_ACTIVITY_LIST:  //查询活动列表
		{
			return OnRequestActivityList(wRequestID, dwContextID, pData, wDataSize);
		}
	case DBR_GP_USER_ACTIVITY:  //查询活动
		{
			return OnRequestActivity(wRequestID, dwContextID, pData, wDataSize);
		}
	case DBR_GP_USER_ACTIVITY_LUCKY:  //活动抽奖
		{
			return OnRequestActivityLucky(wRequestID, dwContextID, pData, wDataSize);
		}
	case DBR_GP_USER_ACTIVITY_RECORD:		//中奖记录
		{
			return OnRequestUserActivityRecord(wRequestID,dwContextID,pData,wDataSize);
		}
	case DBR_GP_USER_SPEAKER_SEND:		//发喇叭
		{
			return OnRequestUserSpeakerSend(wRequestID,dwContextID,pData,wDataSize);
		}
	case DBR_GP_USER_QUERY_CONFIG:		//查询配置项
		{
			return OnRequestUserQueryConfig(wRequestID,dwContextID,pData,wDataSize);
		}
	}

	return false;
}

//登录请求处理
bool CDataBaseSink::OnRequestLogon(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//变量定义
		LONG lReturnValue=-1;
		LPCTSTR pszPassword=NULL;

		BOOL bIsRegisterAnony = FALSE;

		//登录处理
		switch (wRequestID)
		{
		case DBR_GP_LOGON_BY_ACCOUNTS:		//帐号登录
			{
				//效验参数
				ASSERT(wDataSize==sizeof(DBR_GP_LogonByAccounts));
				if (wDataSize!=sizeof(DBR_GP_LogonByAccounts)) return false;

				//执行查询
				DBR_GP_LogonByAccounts * pLogonByAccounts=(DBR_GP_LogonByAccounts *)pData;
				lReturnValue=SPLogonByAccounts(pLogonByAccounts->szAccounts,pLogonByAccounts->szPassWord,pLogonByAccounts->dwClientIP,pLogonByAccounts->szComputerID);
				pszPassword=pLogonByAccounts->szPassWord;

				break;
			}
		case DBR_GP_LOGON_BY_USERID:		//I D 登录
			{
				//效验参数
				ASSERT(wDataSize==sizeof(DBR_GP_LogonByUserID));
				if (wDataSize!=sizeof(DBR_GP_LogonByUserID)) return false;

				//执行查询
				DBR_GP_LogonByUserID * pLogonByUserID=(DBR_GP_LogonByUserID *)pData;
				lReturnValue=SPLogonByUserID(pLogonByUserID->dwUserID,pLogonByUserID->szPassWord,pLogonByUserID->dwClientIP,pLogonByUserID->szComputerID);
				pszPassword=pLogonByUserID->szPassWord;

				break;
			}
		case DBR_GP_REGISTER_ACCOUNTS:		//注册帐号
			{
				//效验参数
				ASSERT(wDataSize==sizeof(DBR_GP_RegisterAccounts));
				if (wDataSize!=sizeof(DBR_GP_RegisterAccounts)) return false;

				//执行查询
				DBR_GP_RegisterAccounts * pRegisterAccounts=(DBR_GP_RegisterAccounts *)pData;
				lReturnValue=SPRegisterAccounts(pRegisterAccounts->szAccounts,pRegisterAccounts->szPassWord,pRegisterAccounts->szSpreader,
					pRegisterAccounts->wFaceID,pRegisterAccounts->cbGender,pRegisterAccounts->dwClientIP,pRegisterAccounts->szComputerID);
				pszPassword=pRegisterAccounts->szPassWord;

				if (std::string(pRegisterAccounts->szAccounts).size() == 0)
				{
					bIsRegisterAnony = TRUE;
				}

				break;
			}
		case DBR_GP_REGISTER_ACCOUNTS_WX:		//注册帐号
			{
				//效验参数
				ASSERT(wDataSize==sizeof(DBR_GP_RegisterAccountsWX));
				if (wDataSize!=sizeof(DBR_GP_RegisterAccountsWX)) return false;

				//执行查询
				DBR_GP_RegisterAccountsWX * pRegisterAccounts=(DBR_GP_RegisterAccountsWX *)pData;
				lReturnValue=SPRegisterAccountsWX(pRegisterAccounts->szAccounts,pRegisterAccounts->szPassWord,pRegisterAccounts->szSpreader,
					pRegisterAccounts->szFaceUrl,pRegisterAccounts->cbGender,pRegisterAccounts->dwClientIP,pRegisterAccounts->szWXComputerID,pRegisterAccounts->szComputerID,
					pRegisterAccounts->szNickName, pRegisterAccounts->szOpenID, pRegisterAccounts->szUnionID);
				pszPassword=pRegisterAccounts->szPassWord;

				if (std::string(pRegisterAccounts->szAccounts).size() == 0)
				{
					bIsRegisterAnony = TRUE;
				}

				break;
			}
		case DBR_GP_WXUPDATE:		//升级成WX帐号
			{
				//效验参数
				ASSERT(wDataSize==sizeof(DBR_GP_WXUpdate));
				if (wDataSize!=sizeof(DBR_GP_WXUpdate)) return false;

				//执行查询
				DBR_GP_WXUpdate * pR=(DBR_GP_WXUpdate *)pData;
				lReturnValue=SPWXUpdate(pR->dwUserID,pR->szFaceUrl,pR->cbGender,pR->dwClientIP,pR->szWXComputerID,pR->szComputerID,pR->szNickName, pR->szOpenID, pR->szUnionID);
				bIsRegisterAnony = TRUE;

				break;
			}
		default :	//默认处理
			{
				ASSERT(FALSE); 
				return false;
			}
		}

		//结果判断
		if (lReturnValue==0L)
		{
			//登录成功
			DBR_GP_LogonSuccess LogonSuccess;
			ZeroMemory(&LogonSuccess,sizeof(LogonSuccess));

			//读取变量
			LogonSuccess.wFaceID=m_AccountsDBAide.GetValue_WORD(TEXT("FaceID"));
			LogonSuccess.dwUserID=m_AccountsDBAide.GetValue_DWORD(TEXT("UserID"));
			LogonSuccess.dwGameID=m_AccountsDBAide.GetValue_DWORD(TEXT("GameID"));
			LogonSuccess.cbGender=m_AccountsDBAide.GetValue_INT(TEXT("Gender"));
			LogonSuccess.cbMember=m_AccountsDBAide.GetValue_BYTE(TEXT("MemberOrder"));
			LogonSuccess.dwExperience=m_AccountsDBAide.GetValue_DWORD(TEXT("Experience"));
			try{ 
				//只有账号登陆时有这个字段
				LogonSuccess.lGoldScore=m_AccountsDBAide.GetValue_LONG(TEXT("GoldScore"));
				LogonSuccess.lWelfare=m_AccountsDBAide.GetValue_LONG(TEXT("Welfare"));
				LogonSuccess.wWelfareLost=m_AccountsDBAide.GetValue_WORD(TEXT("WelfareLost"));
			}
			catch(IDataBaseException * pIException){
				//输出错误--忽略此错误
	//			LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
	//			CTraceService::TraceString(pszDescribe,TraceLevel_Exception);
			}

			LogonSuccess.dwCustomFaceVer=m_AccountsDBAide.GetValue_DWORD(TEXT("CustomFaceVer"));
			LogonSuccess.cbIsGuest=m_AccountsDBAide.GetValue_INT(TEXT("IsGuest"));
			LogonSuccess.cbHasGuest=m_AccountsDBAide.GetValue_INT(TEXT("HasGuest"));
			m_AccountsDBAide.GetValue_String(TEXT("Accounts"),LogonSuccess.szAccounts,CountArray(LogonSuccess.szAccounts));
			m_AccountsDBAide.GetValue_String(TEXT("NickName"),LogonSuccess.szNickName,CountArray(LogonSuccess.szNickName));
			m_AccountsDBAide.GetValue_String(TEXT("UnderWrite"),LogonSuccess.szUnderWrite,CountArray(LogonSuccess.szUnderWrite));
			m_AccountsDBAide.GetValue_String(TEXT("ErrorDescribe"),LogonSuccess.szDescribeString,CountArray(LogonSuccess.szDescribeString));
			m_AccountsDBAide.GetValue_String(TEXT("LobbyRule"),LogonSuccess.szRule,CountArray(LogonSuccess.szRule));

			//设置密码
			if(pszPassword)CopyMemory(LogonSuccess.szPassword,pszPassword,CountArray(LogonSuccess.szPassword));		
			///游客
			if (bIsRegisterAnony)
			{
				try{ 
					//只有游客账号注册时有这个字段
					m_AccountsDBAide.GetValue_String(TEXT("GuestPassword"),LogonSuccess.szPassword,CountArray(LogonSuccess.szPassword));
				}
				catch(IDataBaseException * pIException){
					//输出错误
					LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
					CTraceService::TraceString(pszDescribe,TraceLevel_Exception);
				}
			}

			//投递结果
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_GP_LOGON_SUCCESS,dwContextID,&LogonSuccess,sizeof(LogonSuccess));
		}
		else
		{
			//登录失败
			DBR_GP_LogonError LogonError;
			ZeroMemory(&LogonError,sizeof(LogonError));

			//设置变量
			LogonError.lErrorCode=lReturnValue;
			m_AccountsDBAide.GetValue_String(TEXT("ErrorDescribe"),LogonError.szErrorDescribe,CountArray(LogonError.szErrorDescribe));

			//投递结果
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_GP_LOGON_ERROR,dwContextID,&LogonError,sizeof(LogonError));
		}
	}
	catch (IDataBaseException * pIException)
	{
		//登录失败
		DBR_GP_LogonError LogonError;
		ZeroMemory(&LogonError,sizeof(LogonError));

		//输出错误
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);

		//操作失败
		LogonError.lErrorCode=-1;
		lstrcpyn(LogonError.szErrorDescribe,TEXT("由于数据库操作异常，请您稍后重试或选择另一服务器登录！"),CountArray(LogonError.szErrorDescribe));

		//投递结果
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_GP_LOGON_ERROR,dwContextID,&LogonError,sizeof(LogonError));
	}

	//关闭记录集
	m_AccountsDBModule->CloseRecordset();

	return true;
}

//签到
bool CDataBaseSink::OnRequestUserSignIn(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//数据验证
	ASSERT(wDataSize==sizeof(DBR_GP_UserSignIn));
	if (wDataSize!=sizeof(DBR_GP_UserSignIn)) return false;

	//数据转换
	DBR_GP_UserSignIn *pSignIn = (DBR_GP_UserSignIn*)pData;

	//执行存储过程
	LONG lReturnValue = -1;
	try
	{
		lReturnValue = SPUserSignIn(pSignIn->dwUserID);
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);
	}

	//成功判断
	if ( lReturnValue == 0 )
	{
		DBR_GP_UserSignInResult SignInResult;
		ZeroMemory(&SignInResult, sizeof(SignInResult));
		lstrcpyn(SignInResult.szDescribeMsg, TEXT("签到成功！"), sizeof(SignInResult.szDescribeMsg));
		SignInResult.bOperateSuccess = true;
		SignInResult.cbTodaySign = 1;
		SignInResult.wTimes = m_AccountsDBAide.GetValue_WORD(TEXT("Times"));
		SignInResult.dwGoldScore = m_AccountsDBAide.GetValue_DWORD(TEXT("GoldScore"));

		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_GP_SIGNIN_RESULT,dwContextID,
			&SignInResult,sizeof(SignInResult));
	}
	else
	{
		DBR_GP_UserSignInResult SignInResult;
		ZeroMemory(&SignInResult, sizeof(SignInResult));
		lstrcpyn(SignInResult.szDescribeMsg, TEXT("由于数据库操作异常，签到失败！"), sizeof(SignInResult.szDescribeMsg));
		SignInResult.bOperateSuccess = false;

		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_GP_SIGNIN_RESULT,dwContextID,
			&SignInResult,sizeof(SignInResult));
	}

	return true;
}

//领奖
bool CDataBaseSink::OnRequestUserGetAward(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//数据验证
	ASSERT(wDataSize==sizeof(DBR_GP_UserGetAward));
	if (wDataSize!=sizeof(DBR_GP_UserGetAward)) return false;

	//数据转换
	DBR_GP_UserGetAward *pAward = (DBR_GP_UserGetAward*)pData;

	//执行存储过程
	LONG lReturnValue = -1;
	try
	{
		lReturnValue = SPUserGetAward(pAward->dwUserID, pAward->dwTaskID);
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);
	}

	//成功判断
	if ( lReturnValue == 0 )
	{
		DBR_GP_UserGetAwardResult GetAwardResult;
		ZeroMemory(&GetAwardResult, sizeof(GetAwardResult));
		lstrcpyn(GetAwardResult.szDescribeMsg, TEXT("领奖成功！"), sizeof(GetAwardResult.szDescribeMsg));
		GetAwardResult.cbStatus = m_AccountsDBAide.GetValue_WORD(TEXT("Status"));
		GetAwardResult.dwTaskID = m_AccountsDBAide.GetValue_WORD(TEXT("TaskID"));
		//GetAwardResult.dwScore = m_AccountsDBAide.GetValue_WORD(TEXT("Score"));
		GetAwardResult.dwScore = m_AccountsDBAide.GetValue_DWORD(TEXT("Score"));

		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_GP_GETAWARD_RESULT,dwContextID,
			&GetAwardResult,sizeof(GetAwardResult));
	}
	else
	{
		DBR_GP_UserGetAwardResult GetAwardResult;
		ZeroMemory(&GetAwardResult, sizeof(GetAwardResult));
		lstrcpyn(GetAwardResult.szDescribeMsg, TEXT("由于数据库操作异常，领奖励失败！"), sizeof(GetAwardResult.szDescribeMsg));
		GetAwardResult.cbStatus = lReturnValue;

		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_GP_GETAWARD_RESULT,dwContextID,
			&GetAwardResult,sizeof(GetAwardResult));
	}

	return true;
}

//兑换的个人信息
bool CDataBaseSink::OnRequestUserExchangeInfo(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//数据验证
	ASSERT(wDataSize==sizeof(DBR_GP_UserExchangeInfo));
	if (wDataSize!=sizeof(DBR_GP_UserExchangeInfo)) return false;

	//数据转换
	DBR_GP_UserExchangeInfo *pExchangeInfo = (DBR_GP_UserExchangeInfo*)pData;

	//执行存储过程
	LONG lReturnValue = -1;
	try
	{
		lReturnValue = SPUserExchangeInfo(pExchangeInfo->dwUserID, pExchangeInfo->szPhone);
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);
	}

	//成功判断
	if ( lReturnValue == 0 )
	{
		DBR_GP_UserExchangeInfoResult ExchangeInfoResult;
		ZeroMemory(&ExchangeInfoResult, sizeof(ExchangeInfoResult));
		ExchangeInfoResult.bOperateSuccess = true;
		ExchangeInfoResult.dwGold = m_AccountsDBAide.GetValue_DWORD(TEXT("Gold"));
		m_AccountsDBAide.GetValue_String(TEXT("Phone"),ExchangeInfoResult.szPhone,CountArray(ExchangeInfoResult.szPhone));
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_GP_EXCHANGE_INFO_RESULT,dwContextID,
			&ExchangeInfoResult,sizeof(ExchangeInfoResult));
	}
	else
	{
		DBR_GP_UserExchangeInfoResult ExchangeInfoResult;
		ZeroMemory(&ExchangeInfoResult, sizeof(ExchangeInfoResult));
		ExchangeInfoResult.bOperateSuccess = false;

		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_GP_EXCHANGE_INFO_RESULT,dwContextID,
			&ExchangeInfoResult,sizeof(ExchangeInfoResult));
	}

	return true;
}

//兑换商品
bool CDataBaseSink::OnRequestUserExchangeProduct(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//数据验证
	ASSERT(wDataSize==sizeof(DBR_GP_UserExchangeProduct));
	if (wDataSize!=sizeof(DBR_GP_UserExchangeProduct)) return false;

	//数据转换
	DBR_GP_UserExchangeProduct *pExchangeProduct = (DBR_GP_UserExchangeProduct*)pData;

	//执行存储过程
	LONG lReturnValue = -1;
	try
	{
		lReturnValue = SPUserExchangeProduct(pExchangeProduct->dwUserID);
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);
	}

	//成功判断
	if ( lReturnValue == 0 )
	{
		DBR_GP_UserExchangeProductResult ExchangeProductResult;
		ZeroMemory(&ExchangeProductResult, sizeof(ExchangeProductResult));
		ExchangeProductResult.bOperateSuccess = true;

		//获取数据
		IDataBase * pIDataBase=(IDataBase *)m_AccountsDBAide.GetDataBase(IID_IDataBase,VER_IDataBase);

		//读取信息
		while (pIDataBase->IsRecordsetEnd()==false)
		{
			DBR_GP_ExchangeProduct product;
			//读取信息
			product.dwAwardID=m_AccountsDBAide.GetValue_DWORD(TEXT("AwardID"));
			product.dwPrice=m_AccountsDBAide.GetValue_DWORD(TEXT("Price"));
			product.dwLeft=m_AccountsDBAide.GetValue_DWORD(TEXT("Lefts"));
			m_AccountsDBAide.GetValue_String(TEXT("AwardImg"),product.szAwardImg,CountArray(product.szAwardImg));
			m_AccountsDBAide.GetValue_String(TEXT("AwardName"),product.szAwardname,CountArray(product.szAwardname));

			//移动记录
			pIDataBase->MoveToNext();

			//插入信息
			ExchangeProductResult.vecProducts.push_back(product);
		}

		//关闭记录
		pIDataBase->CloseRecordset();

		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_GP_EXCHANGE_PRODUCT_RESULT,dwContextID,
			&ExchangeProductResult,sizeof(ExchangeProductResult));
	}
	else
	{
		DBR_GP_UserExchangeProductResult ExchangeProductResult;
		ZeroMemory(&ExchangeProductResult, sizeof(ExchangeProductResult));
		ExchangeProductResult.bOperateSuccess = false;

		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_GP_EXCHANGE_PRODUCT_RESULT,dwContextID,
			&ExchangeProductResult,sizeof(ExchangeProductResult));
	}

	return true;
}


//兑换商品
bool CDataBaseSink::OnRequestUserExchangeProduct_V2(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//数据验证
	ASSERT(wDataSize==sizeof(DBR_GP_UserExchangeProduct));
	if (wDataSize!=sizeof(DBR_GP_UserExchangeProduct)) return false;

	//数据转换
	DBR_GP_UserExchangeProduct *pExchangeProduct = (DBR_GP_UserExchangeProduct*)pData;

	//执行存储过程
	LONG lReturnValue = -1;
	try
	{
		lReturnValue = SPUserExchangeProduct(pExchangeProduct->dwUserID);
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);
	}

	//成功判断
	if ( lReturnValue == 0 )
	{
		DBR_GP_UserExchangeProductResult ExchangeProductResult;
		ZeroMemory(&ExchangeProductResult, sizeof(ExchangeProductResult));
		ExchangeProductResult.bOperateSuccess = true;

		//获取数据
		IDataBase * pIDataBase=(IDataBase *)m_AccountsDBAide.GetDataBase(IID_IDataBase,VER_IDataBase);

		//读取信息
		while (pIDataBase->IsRecordsetEnd()==false)
		{
			DBR_GP_ExchangeProduct product;
			//读取信息
			product.dwAwardID=m_AccountsDBAide.GetValue_DWORD(TEXT("AwardID"));
			product.dwPrice=m_AccountsDBAide.GetValue_DWORD(TEXT("Price"));
			product.dwLeft=m_AccountsDBAide.GetValue_DWORD(TEXT("Lefts"));
			product.dwType = m_AccountsDBAide.GetValue_INT(TEXT("Type"));
			m_AccountsDBAide.GetValue_String(TEXT("AwardImg"),product.szAwardImg,CountArray(product.szAwardImg));
			m_AccountsDBAide.GetValue_String(TEXT("AwardName"),product.szAwardname,CountArray(product.szAwardname));

			//移动记录
			pIDataBase->MoveToNext();

			//插入信息
			ExchangeProductResult.vecProducts.push_back(product);
		}

		//关闭记录
		pIDataBase->CloseRecordset();

		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_GP_EXCHANGE_PRODUCT_RESULT_V2,dwContextID,
			&ExchangeProductResult,sizeof(ExchangeProductResult));
	}
	else
	{
		DBR_GP_UserExchangeProductResult ExchangeProductResult;
		ZeroMemory(&ExchangeProductResult, sizeof(ExchangeProductResult));
		ExchangeProductResult.bOperateSuccess = false;

		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_GP_EXCHANGE_PRODUCT_RESULT_V2,dwContextID,
			&ExchangeProductResult,sizeof(ExchangeProductResult));
	}

	return true;
}

//兑换
bool CDataBaseSink::OnRequestUserExchange(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//数据验证
	ASSERT(wDataSize==sizeof(DBR_GP_UserExchange));
	if (wDataSize!=sizeof(DBR_GP_UserExchange)) return false;

	//数据转换
	DBR_GP_UserExchange *pExchange = (DBR_GP_UserExchange*)pData;

	//执行存储过程
	LONG lReturnValue = -1;
	try
	{
		lReturnValue = SPUserExchange(pExchange->dwUserID, pExchange->dwAwardID);
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);
	}

	//成功判断
	if ( lReturnValue == 0 )
	{
		DBR_GP_UserExchangeResult ExchangeResult;
		ZeroMemory(&ExchangeResult, sizeof(ExchangeResult));
		ExchangeResult.bOperateSuccess = true;
		ExchangeResult.dwGold = m_AccountsDBAide.GetValue_DWORD(TEXT("Gold"));
		ExchangeResult.dwAwardID = m_AccountsDBAide.GetValue_DWORD(TEXT("AwardID"));

		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_GP_EXCHANGE_RESULT,dwContextID,
			&ExchangeResult,sizeof(ExchangeResult));
	}
	else
	{
		DBR_GP_UserExchangeResult ExchangeResult;
		ZeroMemory(&ExchangeResult, sizeof(ExchangeResult));
		ExchangeResult.bOperateSuccess = false;

		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_GP_EXCHANGE_RESULT,dwContextID,
			&ExchangeResult,sizeof(ExchangeResult));
	}

	return true;
}

//兑换
bool CDataBaseSink::OnRequestUserExchange_V2(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//数据验证
	ASSERT(wDataSize==sizeof(DBR_GP_UserExchange));
	if (wDataSize!=sizeof(DBR_GP_UserExchange)) return false;

	//数据转换
	DBR_GP_UserExchange *pExchange = (DBR_GP_UserExchange*)pData;

	//执行存储过程
	LONG lReturnValue = -1;
	try
	{
		lReturnValue = SPUserExchange_V2(pExchange->dwUserID, pExchange->dwAwardID);
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);
	}

	//成功判断
	if ( lReturnValue == 0 )
	{
		DBR_GP_UserExchangeResult ExchangeResult;
		ZeroMemory(&ExchangeResult, sizeof(ExchangeResult));
		ExchangeResult.bOperateSuccess = true;
		ExchangeResult.dwGold = m_AccountsDBAide.GetValue_DWORD(TEXT("Gold"));
		ExchangeResult.dwAwardID = m_AccountsDBAide.GetValue_DWORD(TEXT("AwardID"));
		ExchangeResult.dwScore = m_AccountsDBAide.GetValue_DWORD(TEXT("TotalScore"));

		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_GP_EXCHANGE_RESULT_V2,dwContextID,
			&ExchangeResult,sizeof(ExchangeResult));
	}
	else
	{
		DBR_GP_UserExchangeResult ExchangeResult;
		ZeroMemory(&ExchangeResult, sizeof(ExchangeResult));
		ExchangeResult.bOperateSuccess = false;

		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_GP_EXCHANGE_RESULT_V2,dwContextID,
			&ExchangeResult,sizeof(ExchangeResult));
	}

	return true;
}

//兑换记录
bool CDataBaseSink::OnRequestUserExchangeRecord(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//数据验证
	ASSERT(wDataSize==sizeof(DBR_GP_UserExchangeRecord));
	if (wDataSize!=sizeof(DBR_GP_UserExchangeRecord)) return false;

	//数据转换
	DBR_GP_UserExchangeRecord *pExchangeRecord = (DBR_GP_UserExchangeRecord*)pData;

	//执行存储过程
	LONG lReturnValue = -1;
	try
	{
		if (pExchangeRecord->nPage < 1)pExchangeRecord->nPage = 1;
		lReturnValue = SPUserExchangeRecord(pExchangeRecord->dwUserID, pExchangeRecord->nPage - 1);
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);
	}

	//成功判断
	if ( lReturnValue == 0 )
	{
		DBR_GP_UserExchangeRecordResult ExchangeRecordResult;
		ExchangeRecordResult.bOperateSuccess = true;

		//获取数据
		IDataBase * pIDataBase=(IDataBase *)m_AccountsDBAide.GetDataBase(IID_IDataBase,VER_IDataBase);

		//读取信息
		while (pIDataBase->IsRecordsetEnd()==false)
		{
			DBR_GP_ExchangeRecord record;
			ZeroMemory(&record, sizeof(record));

			//读取信息
			record.dwTotal=m_AccountsDBAide.GetValue_DWORD(TEXT("T"));
			record.dwID=m_AccountsDBAide.GetValue_DWORD(TEXT("ID"));
			record.dwAwardID=m_AccountsDBAide.GetValue_DWORD(TEXT("AwardID"));
	//		record.dwDate=m_AccountsDBAide.GetValue_DWORD(TEXT("ExchangeDate"));
			m_AccountsDBAide.GetValue_String(TEXT("D"),record.szDate,CountArray(record.szDate));
			m_AccountsDBAide.GetValue_String(TEXT("AwardName"),record.szAwardName,CountArray(record.szAwardName));
			m_AccountsDBAide.GetValue_String(TEXT("Status"),record.szStatus,CountArray(record.szStatus));

			//移动记录
			pIDataBase->MoveToNext();

			//插入信息
			ExchangeRecordResult.vecRecords.push_back(record);
		}

		//关闭记录
		pIDataBase->CloseRecordset();

		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_GP_EXCHANGE_RECORD_RESULT,dwContextID,
			&ExchangeRecordResult,sizeof(ExchangeRecordResult));
	}
	else
	{
		DBR_GP_UserExchangeRecordResult ExchangeRecordResult;
		ZeroMemory(&ExchangeRecordResult, sizeof(ExchangeRecordResult));
		ExchangeRecordResult.bOperateSuccess = false;

		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_GP_EXCHANGE_RECORD_RESULT,dwContextID,
			&ExchangeRecordResult,sizeof(ExchangeRecordResult));
	}

	return true;
}


//上传头像
bool CDataBaseSink::OnRequestUploadFace(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//数据验证
	ASSERT(wDataSize==sizeof(DBR_GP_UploadCustomFace));
	if (wDataSize!=sizeof(DBR_GP_UploadCustomFace)) return false;

	//数据转换
	DBR_GP_UploadCustomFace *pUploadCustomFace = (DBR_GP_UploadCustomFace*)pData;

	//文件定义
	CFile fileCustomFace;
	TCHAR szFileName[128];
	_snprintf(szFileName, CountArray(szFileName), TEXT("%s\\UploadFile_%ld.zip"), theApp.m_szDirWork, pUploadCustomFace->dwUserID);

	try
	{
		if ( fileCustomFace.Open( szFileName, CFile::modeRead|CFile::typeBinary) )
		{
			BYTE cbCustomFaceData[1024*30];

			//读取文件
			UINT uReadSize = fileCustomFace.Read(cbCustomFaceData, CountArray(cbCustomFaceData));

			//关闭文件
			fileCustomFace.Close();

			LONG lRet=-1;
			try
			{
				lRet = SPUploadFace(pUploadCustomFace->dwUserID, cbCustomFaceData, uReadSize);
			}
			catch (IDataBaseException * pIException)
			{
				//错误信息
				LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
				CTraceService::TraceString(pszDescribe,TraceLevel_Exception);
			}

			//成功判断
			if ( lRet != -1 )
			{
				DBR_GP_UploadFaceResult UploadFaceResult;
				ZeroMemory(&UploadFaceResult, sizeof(UploadFaceResult));
				lstrcpyn(UploadFaceResult.szDescribeMsg, TEXT("头像上传成功！"), sizeof(UploadFaceResult.szDescribeMsg));
				UploadFaceResult.dwFaceVer=lRet;
				UploadFaceResult.bOperateSuccess=true;

				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_GP_UPLOAD_FACE_RESULT,dwContextID,
					&UploadFaceResult,sizeof(UploadFaceResult));
			}
			else
			{
				//操作失败
				DBR_GP_UploadFaceResult UploadFaceResult;
				ZeroMemory(&UploadFaceResult, sizeof(UploadFaceResult));
				lstrcpyn(UploadFaceResult.szDescribeMsg, TEXT("由于数据库操作异常，请重新上传！"), sizeof(UploadFaceResult.szDescribeMsg));
				UploadFaceResult.bOperateSuccess=false;

				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_GP_UPLOAD_FACE_RESULT,dwContextID,
					&UploadFaceResult,sizeof(UploadFaceResult));
			}

			//删除文件
			CFile::Remove(szFileName);
		}
	}catch(...){}	

	return true;
}

//下载头像
bool CDataBaseSink::OnRequestDownloadFace(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//校验参数
	ASSERT(wDataSize == sizeof(DBR_GP_DownloadCustomFace));
	if ( wDataSize != sizeof(DBR_GP_DownloadCustomFace)) return false;

	//类型转换
	DBR_GP_DownloadCustomFace *pDownloadCustomFace = (DBR_GP_DownloadCustomFace*)pData;

	LONG lReturnValue=-1;
	long lSize=0;
	bool bRet = false;
	try
	{
		lReturnValue=SPDownloadFace(pDownloadCustomFace->dwUserID);

		//读取数据
		if ( lReturnValue==0 )
		{	
			DWORD dwImageSize=0;
			dwImageSize=m_AccountsDBAide.GetValue_DWORD(TEXT("ImgSize"));
			
			//读取数据
			IDataBase * pIDataBase=m_AccountsDBModule.GetInterface();
			_variant_t vtFld;
			pIDataBase->GetRecordsetValue(TEXT("CustomFaceImage"), vtFld);
			BYTE * pcbBuf=NULL;
			SafeArrayAccessData(vtFld.parray,(LPVOID *)&pcbBuf);

			//投递请求
			DBR_GP_DownloadFaceSuccess DownloadFaceSuccess;
			ZeroMemory(&DownloadFaceSuccess, sizeof(DownloadFaceSuccess));
			DownloadFaceSuccess.dwToltalSize = dwImageSize;
			DownloadFaceSuccess.dwUserID = pDownloadCustomFace->dwUserID;
			DWORD dwSendSize = 0;
			while( dwSendSize <dwImageSize )
			{
				//设置数据
				if ( 1024 < dwImageSize - dwSendSize ) DownloadFaceSuccess.dwCurrentSize = 1024;
				else DownloadFaceSuccess.dwCurrentSize = dwImageSize - dwSendSize ;
				CopyMemory(DownloadFaceSuccess.bFaceData, pcbBuf+dwSendSize, DownloadFaceSuccess.dwCurrentSize);

				//投递消息
				WORD wPostSize = WORD(sizeof(DownloadFaceSuccess)-sizeof(DownloadFaceSuccess.bFaceData)+DownloadFaceSuccess.dwCurrentSize);
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_GP_DOWNLOADFACE_SUCCESS, dwContextID, &DownloadFaceSuccess, wPostSize);

				//递增变量
				dwSendSize += DownloadFaceSuccess.dwCurrentSize;
			}

			//释放资源
			SafeArrayUnaccessData(vtFld.parray);
		}
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);
	}


	return true;
}

//删除消息
bool CDataBaseSink::OnRequestCustomFaceDelete(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//参数验证
	ASSERT(wDataSize == sizeof(DBR_GP_CustomFaceDelete));
	if ( wDataSize != sizeof(DBR_GP_CustomFaceDelete)) return false;

	//类型转换
	DBR_GP_CustomFaceDelete *pCustomFaceDelete = (DBR_GP_CustomFaceDelete*)pData;

	//执行存储过程
	LONG lReturnValue = -1;
	try
	{
		lReturnValue = SPDeleteCustomFace(pCustomFaceDelete->dwUserID);
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);
	}

	//成功判断
	if ( lReturnValue == 0 )
	{
		DBR_GP_DeleteFaceResult DeleteFaceResult;
		ZeroMemory(&DeleteFaceResult, sizeof(DeleteFaceResult));
		lstrcpyn(DeleteFaceResult.szDescribeMsg, TEXT("头像删除成功！"), sizeof(DeleteFaceResult.szDescribeMsg));
		DeleteFaceResult.dwFaceVer=pCustomFaceDelete->dwFaceVer;
		DeleteFaceResult.bOperateSuccess = true;

		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_GP_DELETE_FACE_RESULT,dwContextID,
			&DeleteFaceResult,sizeof(DeleteFaceResult));
	}
	else
	{
		DBR_GP_DeleteFaceResult DeleteFaceResult;
		ZeroMemory(&DeleteFaceResult, sizeof(DeleteFaceResult));
		lstrcpyn(DeleteFaceResult.szDescribeMsg, TEXT("由于数据库操作异常，删除失败！"), sizeof(DeleteFaceResult.szDescribeMsg));
		DeleteFaceResult.dwFaceVer=pCustomFaceDelete->dwFaceVer;
		DeleteFaceResult.bOperateSuccess = false;

		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_GP_DELETE_FACE_RESULT,dwContextID,
			&DeleteFaceResult,sizeof(DeleteFaceResult));
	}

	return true;
}

//修改资料
bool CDataBaseSink::OnRequestModifyIndividual(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//参数验证
	ASSERT(wDataSize == sizeof(DBR_GP_ModifyIndividual));
	if ( wDataSize != sizeof(DBR_GP_ModifyIndividual)) return false;

	//类型转换
	DBR_GP_ModifyIndividual *pModifyIndividual = (DBR_GP_ModifyIndividual*)pData;

	//执行存储过程
	LONG lReturnValue = -1;
	try
	{
		lReturnValue = SPModifyIndividual(pModifyIndividual->szAccount, pModifyIndividual->szNickname, pModifyIndividual->szPhone, pModifyIndividual->nGender);
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);
	}

	//成功判断
	if ( lReturnValue == 0 )
	{
		DBR_GP_ModifyIndividualResult ModifyIndividualResult;
		ZeroMemory(&ModifyIndividualResult, sizeof(ModifyIndividualResult));
	//	lstrcpyn(ModifyIndividualResult.szDescribeMsg, TEXT("资料修改成功！"), sizeof(ModifyIndividualResult.szDescribeMsg));
		m_AccountsDBAide.GetValue_String(TEXT("Accounts"),ModifyIndividualResult.szAccount,CountArray(ModifyIndividualResult.szAccount));
		m_AccountsDBAide.GetValue_String(TEXT("ErrorDescribe"),ModifyIndividualResult.szDescribeMsg,CountArray(ModifyIndividualResult.szDescribeMsg));
		ModifyIndividualResult.iCode = 0;

		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_GP_MODIFY_RESULT,dwContextID,
			&ModifyIndividualResult,sizeof(ModifyIndividualResult));
	}
	else
	{
		DBR_GP_ModifyIndividualResult ModifyIndividualResult;
		ZeroMemory(&ModifyIndividualResult, sizeof(ModifyIndividualResult));
	//	lstrcpyn(ModifyIndividualResult.szDescribeMsg, TEXT("由于数据库操作异常，修改失败！"), sizeof(ModifyIndividualResult.szDescribeMsg));
		m_AccountsDBAide.GetValue_String(TEXT("Accounts"),ModifyIndividualResult.szAccount,CountArray(ModifyIndividualResult.szAccount));
		m_AccountsDBAide.GetValue_String(TEXT("ErrorDescribe"),ModifyIndividualResult.szDescribeMsg,CountArray(ModifyIndividualResult.szDescribeMsg));
		ModifyIndividualResult.iCode = lReturnValue;

		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_GP_MODIFY_RESULT,dwContextID,
			&ModifyIndividualResult,sizeof(ModifyIndividualResult));
	}
	return true;
}

//修改个性签名
bool CDataBaseSink::OnRequestModifyUnderWrite(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//参数验证
	ASSERT(wDataSize == sizeof(DBR_GP_ModifyUnderWrite));
	if ( wDataSize != sizeof(DBR_GP_ModifyUnderWrite)) return false;

	//类型转换
	DBR_GP_ModifyUnderWrite *pModifyUnderWrite = (DBR_GP_ModifyUnderWrite*)pData;

	//执行存储过程
	LONG lReturnValue = -1;
	try
	{
		lReturnValue = SPModifyUnderWrite(pModifyUnderWrite->dwUserID, pModifyUnderWrite->szDescribe);
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);
	}

	//成功判断
	if ( lReturnValue == 0 )
	{
		DBR_GP_ModifyUnderWriteResult ModifyUnderWriteResult;
		ZeroMemory(&ModifyUnderWriteResult, sizeof(ModifyUnderWriteResult));
	//	lstrcpyn(ModifyUnderWriteResult.szDescribeMsg, TEXT("个性签名修改成功！"), sizeof(ModifyUnderWriteResult.szDescribeMsg));
		m_AccountsDBAide.GetValue_String(TEXT("Accounts"),ModifyUnderWriteResult.szAccount,CountArray(ModifyUnderWriteResult.szAccount));
		m_AccountsDBAide.GetValue_String(TEXT("ErrorDescribe"),ModifyUnderWriteResult.szDescribeMsg,CountArray(ModifyUnderWriteResult.szDescribeMsg));
		ModifyUnderWriteResult.iCode = 0;

		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_GP_MODIFY_UNDERWRITE_RESULT,dwContextID,
			&ModifyUnderWriteResult,sizeof(ModifyUnderWriteResult));
	}
	else
	{
		DBR_GP_ModifyUnderWriteResult ModifyUnderWriteResult;
		ZeroMemory(&ModifyUnderWriteResult, sizeof(ModifyUnderWriteResult));
	//	lstrcpyn(ModifyUnderWriteResult.szDescribeMsg, TEXT("由于数据库操作异常，修改失败！"), sizeof(ModifyUnderWriteResult.szDescribeMsg));
		//m_AccountsDBAide.GetValue_String(TEXT("Accounts"),ModifyUnderWriteResult.szAccount,CountArray(ModifyUnderWriteResult.szAccount));
		m_AccountsDBAide.GetValue_String(TEXT("ErrorDescribe"),ModifyUnderWriteResult.szDescribeMsg,CountArray(ModifyUnderWriteResult.szDescribeMsg));
		ModifyUnderWriteResult.iCode = lReturnValue;

		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_GP_MODIFY_UNDERWRITE_RESULT,dwContextID,
			&ModifyUnderWriteResult,sizeof(ModifyUnderWriteResult));
	}
	return true;
}

//修改手机号
bool CDataBaseSink::OnRequestModifyPhone(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//参数验证
	ASSERT(wDataSize == sizeof(DBR_GP_ModifyPhone));
	if ( wDataSize != sizeof(DBR_GP_ModifyPhone)) return false;

	//类型转换
	DBR_GP_ModifyPhone *pModifyPhone = (DBR_GP_ModifyPhone*)pData;

	//执行存储过程
	LONG lReturnValue = -1;
	try
	{
		lReturnValue = SPModifyPhone(pModifyPhone->dwUserID, pModifyPhone->szPhone);
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);
	}

	//成功判断
	if ( lReturnValue == 0 )
	{
		DBR_GP_ModifyPhoneResult ModifyPhoneResult;
		ZeroMemory(&ModifyPhoneResult, sizeof(ModifyPhoneResult));
	//	lstrcpyn(ModifyPhoneResult.szDescribeMsg, TEXT("手机号修改成功！"), sizeof(ModifyPhoneResult.szDescribeMsg));
		m_AccountsDBAide.GetValue_String(TEXT("Accounts"),ModifyPhoneResult.szAccount,CountArray(ModifyPhoneResult.szAccount));
		m_AccountsDBAide.GetValue_String(TEXT("Phone"),ModifyPhoneResult.szPhone,CountArray(ModifyPhoneResult.szPhone));
		m_AccountsDBAide.GetValue_String(TEXT("ErrorDescribe"),ModifyPhoneResult.szDescribeMsg,CountArray(ModifyPhoneResult.szDescribeMsg));
		ModifyPhoneResult.iCode = 0;

		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_GP_MODIFY_PHONE_RESULT,dwContextID,
			&ModifyPhoneResult,sizeof(ModifyPhoneResult));
	}
	else
	{
		DBR_GP_ModifyPhoneResult ModifyPhoneResult;
		ZeroMemory(&ModifyPhoneResult, sizeof(ModifyPhoneResult));
	//	lstrcpyn(ModifyPhoneResult.szDescribeMsg, TEXT("由于数据库操作异常，修改失败！"), sizeof(ModifyPhoneResult.szDescribeMsg));
		m_AccountsDBAide.GetValue_String(TEXT("Accounts"),ModifyPhoneResult.szAccount,CountArray(ModifyPhoneResult.szAccount));
		m_AccountsDBAide.GetValue_String(TEXT("Phone"),ModifyPhoneResult.szPhone,CountArray(ModifyPhoneResult.szPhone));
		m_AccountsDBAide.GetValue_String(TEXT("ErrorDescribe"),ModifyPhoneResult.szDescribeMsg,CountArray(ModifyPhoneResult.szDescribeMsg));
		ModifyPhoneResult.iCode = lReturnValue;

		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_GP_MODIFY_PHONE_RESULT,dwContextID,
			&ModifyPhoneResult,sizeof(ModifyPhoneResult));
	}
	return true;
}

//校验名称
bool CDataBaseSink::OnRequestCheckID(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//参数验证
	ASSERT(wDataSize == sizeof(DBR_GP_CheckID));
	if ( wDataSize != sizeof(DBR_GP_CheckID)) return false;

	//类型转换
	DBR_GP_CheckID *pReq = (DBR_GP_CheckID*)pData;

	//执行存储过程
	LONG lReturnValue = -1;
	try
	{
		lReturnValue = SPCheckID(pReq->szAccount);
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);
	}

	//成功判断
	if ( lReturnValue == 0 )
	{
		DBR_GP_CommonResult result;
		ZeroMemory(&result, sizeof(result));
		//lstrcpyn(result.szDescribeMsg, TEXT("校验名称成功！"), sizeof(result.szDescribeMsg));
		m_AccountsDBAide.GetValue_String(TEXT("Accounts"),result.szAccount,CountArray(result.szAccount));
		m_AccountsDBAide.GetValue_String(TEXT("ErrorDescribe"),result.szDescribeMsg,CountArray(result.szDescribeMsg));
		result.iCode = 0;

		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_GP_CHECKID_RESULT,dwContextID,
			&result,sizeof(DBR_GP_CommonResult));
	}
	else
	{
		DBR_GP_CommonResult result;
		ZeroMemory(&result, sizeof(result));
		//lstrcpyn(result.szDescribeMsg, TEXT("由于数据库操作异常，校验失败！"), sizeof(result.szDescribeMsg));
		
		m_AccountsDBAide.GetValue_String(TEXT("Accounts"),result.szAccount,CountArray(result.szAccount));
		m_AccountsDBAide.GetValue_String(TEXT("ErrorDescribe"),result.szDescribeMsg,CountArray(result.szDescribeMsg));
		result.iCode = lReturnValue;

		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_GP_CHECKID_RESULT,dwContextID,
			&result,sizeof(DBR_GP_CommonResult));
	}
	return true;
}

//校验昵称	
bool CDataBaseSink::OnRequestCheckNickName(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//参数验证
	ASSERT(wDataSize == sizeof(DBR_GP_CheckNickName));
	if ( wDataSize != sizeof(DBR_GP_CheckNickName)) return false;

	//类型转换
	DBR_GP_CheckNickName *pReq = (DBR_GP_CheckNickName*)pData;

	//执行存储过程
	LONG lReturnValue = -1;
	try
	{
		lReturnValue = SPCheckNickName(pReq->szNickName);
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);
	}

	//成功判断
	if ( lReturnValue == 0 )
	{
		DBR_GP_CommonResult result;
		ZeroMemory(&result, sizeof(result));
	//	lstrcpyn(result.szDescribeMsg, TEXT("校验昵称成功！"), sizeof(result.szDescribeMsg));
		int sz = CountArray(result.szAccount);
		m_AccountsDBAide.GetValue_String(TEXT("Accounts"),result.szAccount,CountArray(result.szAccount));
		m_AccountsDBAide.GetValue_String(TEXT("ErrorDescribe"),result.szDescribeMsg,CountArray(result.szDescribeMsg));
		result.iCode = 0;

		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_GP_CHECK_NICKNAME_RESULT,dwContextID,
			&result,sizeof(DBR_GP_CommonResult));
	}
	else
	{
		DBR_GP_CommonResult result;
		ZeroMemory(&result, sizeof(result));
	//	lstrcpyn(result.szDescribeMsg, TEXT("由于数据库操作异常，校验失败！"), sizeof(result.szDescribeMsg));
		m_AccountsDBAide.GetValue_String(TEXT("Accounts"),result.szAccount,CountArray(result.szAccount));
		m_AccountsDBAide.GetValue_String(TEXT("ErrorDescribe"),result.szDescribeMsg,CountArray(result.szDescribeMsg));
		result.iCode = lReturnValue;

		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_GP_CHECK_NICKNAME_RESULT,dwContextID,
			&result,sizeof(DBR_GP_CommonResult));
	}
	return true;
}

//注册	
bool CDataBaseSink::OnRequestRegister(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//参数验证
	ASSERT(wDataSize == sizeof(DBR_GP_Register));
	if ( wDataSize != sizeof(DBR_GP_Register)) return false;

	//类型转换
	DBR_GP_Register *pReq = (DBR_GP_Register*)pData;

	//执行存储过程
	LONG lReturnValue = -1;
	try
	{
		lReturnValue = SPRegister(pReq->szAccount,pReq->szNickName,pReq->szPhone,pReq->nGender, pReq->szPassword, 
			pReq->dwClientIP,pReq->szComputerID, pReq->szChannel);
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);
	}

	//成功判断
	if ( lReturnValue == 0 )
	{
		DBR_GP_CommonResult result;
		ZeroMemory(&result, sizeof(result));
//		lstrcpyn(result.szDescribeMsg, TEXT("注册成功！"), sizeof(result.szDescribeMsg));
		m_AccountsDBAide.GetValue_String(TEXT("Accounts"),result.szAccount,CountArray(result.szAccount));
		m_AccountsDBAide.GetValue_String(TEXT("ErrorDescribe"),result.szDescribeMsg,CountArray(result.szDescribeMsg));
		result.iCode = 0;

		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_GP_REGISTER_RESULT,dwContextID,
			&result,sizeof(DBR_GP_CommonResult));
	}
	else
	{
		DBR_GP_CommonResult result;
		ZeroMemory(&result, sizeof(result));
	//	lstrcpyn(result.szDescribeMsg, TEXT("由于数据库操作异常，注册失败！"), sizeof(result.szDescribeMsg));
		m_AccountsDBAide.GetValue_String(TEXT("Accounts"),result.szAccount,CountArray(result.szAccount));
		m_AccountsDBAide.GetValue_String(TEXT("ErrorDescribe"),result.szDescribeMsg,CountArray(result.szDescribeMsg));
		result.iCode = lReturnValue;

		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_GP_REGISTER_RESULT,dwContextID,
			&result,sizeof(DBR_GP_CommonResult));
	}
	return true;
}

//帐号升级	
bool CDataBaseSink::OnRequestIDUpdate(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//参数验证
	ASSERT(wDataSize == sizeof(DBR_GP_IDUpdate));
	if ( wDataSize != sizeof(DBR_GP_IDUpdate)) return false;

	//类型转换
	DBR_GP_IDUpdate *pReq = (DBR_GP_IDUpdate*)pData;

	//执行存储过程
	LONG lReturnValue = -1;
	try
	{
		lReturnValue = SPIDUpdate(pReq->szAccountOld,pReq->szAccount, pReq->szNickName,pReq->nGender, pReq->szPassword);
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);
	}

	//成功判断
	if ( lReturnValue == 0 )
	{
		DBR_GP_CommonResult result;
		ZeroMemory(&result, sizeof(result));
	//	lstrcpyn(result.szDescribeMsg, TEXT("升级成功！"), sizeof(result.szDescribeMsg));
		m_AccountsDBAide.GetValue_String(TEXT("Accounts"),result.szAccount,CountArray(result.szAccount));
		m_AccountsDBAide.GetValue_String(TEXT("ErrorDescribe"),result.szDescribeMsg,CountArray(result.szDescribeMsg));
		result.iCode = 0;

		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_GP_ID_UPDATE_RESULT,dwContextID,
			&result,sizeof(DBR_GP_CommonResult));
	}
	else
	{
		DBR_GP_CommonResult result;
		ZeroMemory(&result, sizeof(result));
//		lstrcpyn(result.szDescribeMsg, TEXT("由于数据库操作异常，升级失败！"), sizeof(result.szDescribeMsg));
		m_AccountsDBAide.GetValue_String(TEXT("Accounts"),result.szAccount,CountArray(result.szAccount));
		m_AccountsDBAide.GetValue_String(TEXT("ErrorDescribe"),result.szDescribeMsg,CountArray(result.szDescribeMsg));
		result.iCode = lReturnValue;

		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_GP_ID_UPDATE_RESULT,dwContextID,
			&result,sizeof(DBR_GP_CommonResult));
	}
	return true;
}

//设置密码	
bool CDataBaseSink::OnRequestResetPwd(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//参数验证
	ASSERT(wDataSize == sizeof(DBR_GP_ResetPwd));
	if ( wDataSize != sizeof(DBR_GP_ResetPwd)) return false;

	//类型转换
	DBR_GP_ResetPwd *pReq = (DBR_GP_ResetPwd*)pData;

	//执行存储过程
	LONG lReturnValue = -1;
	try
	{
		lReturnValue = SPResetPwd(pReq->szAccount,pReq->szPassword);
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);
	}

	//成功判断
	if ( lReturnValue == 0 )
	{
		DBR_GP_CommonResult result;
		ZeroMemory(&result, sizeof(result));
	//	lstrcpyn(result.szDescribeMsg, TEXT("设置密码成功！"), sizeof(result.szDescribeMsg));
		m_AccountsDBAide.GetValue_String(TEXT("Accounts"),result.szAccount,CountArray(result.szAccount));
		m_AccountsDBAide.GetValue_String(TEXT("ErrorDescribe"),result.szDescribeMsg,CountArray(result.szDescribeMsg));
		result.iCode = 0;

		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_GP_RESET_PWD_RESULT,dwContextID,
			&result,sizeof(DBR_GP_CommonResult));
	}
	else
	{
		DBR_GP_CommonResult result;
		ZeroMemory(&result, sizeof(result));
//		lstrcpyn(result.szDescribeMsg, TEXT("由于数据库操作异常，设置密码失败！"), sizeof(result.szDescribeMsg));
		m_AccountsDBAide.GetValue_String(TEXT("Accounts"),result.szAccount,CountArray(result.szAccount));
		m_AccountsDBAide.GetValue_String(TEXT("ErrorDescribe"),result.szDescribeMsg,CountArray(result.szDescribeMsg));
		result.iCode = lReturnValue;

		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_GP_RESET_PWD_RESULT,dwContextID,
			&result,sizeof(DBR_GP_CommonResult));
	}
	return true;
}

//用户信息	
bool CDataBaseSink::OnRequestUserInfo(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//参数验证
	ASSERT(wDataSize == sizeof(DBR_GP_UserInfo));
	if ( wDataSize != sizeof(DBR_GP_UserInfo)) return false;

	//类型转换
	DBR_GP_UserInfo *pReq = (DBR_GP_UserInfo*)pData;

	//执行存储过程
	LONG lReturnValue = -1;
	try
	{
		lReturnValue = SPUserInfo(pReq->szAccount);
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);
	}

	//成功判断
	if ( lReturnValue == 0 )
	{
		DBR_GP_UserInfo_Result result;
		ZeroMemory(&result, sizeof(result));
		
	//	m_AccountsDBAide.GetValue_String(TEXT("ErrorDescribe"),result.szDescribeMsg,CountArray(result.szDescribeMsg));
		
		m_AccountsDBAide.GetValue_String(TEXT("Accounts"),result.szAccount,CountArray(result.szAccount));
		m_AccountsDBAide.GetValue_String(TEXT("NickName"),result.szNickName,CountArray(result.szNickName));
		m_AccountsDBAide.GetValue_String(TEXT("Phone"),result.szPhone,CountArray(result.szPhone));
		m_AccountsDBAide.GetValue_String(TEXT("ErrorDescribe"),result.szDescribeMsg,CountArray(result.szDescribeMsg));
		result.nGender = m_AccountsDBAide.GetValue_INT(TEXT("Gender"));

		result.iCode = lReturnValue;

		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_GP_USER_INFO_RESULT,dwContextID,
			&result,sizeof(DBR_GP_UserInfo_Result));
	}
	else
	{
		DBR_GP_UserInfo_Result result;
		ZeroMemory(&result, sizeof(result));
//		lstrcpyn(result.szDescribeMsg, TEXT("由于数据库操作异常，查询失败！"), sizeof(result.szDescribeMsg));	
		m_AccountsDBAide.GetValue_String(TEXT("ErrorDescribe"),result.szDescribeMsg,CountArray(result.szDescribeMsg));
		result.iCode = lReturnValue;

		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_GP_USER_INFO_RESULT,dwContextID,
			&result,sizeof(DBR_GP_UserInfo_Result));
	}
	return true;
}

//用户能否领取推广奖励	
bool CDataBaseSink::OnRequestUserCanSpread(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//参数验证
	ASSERT(wDataSize == sizeof(DBR_GP_UserCanSpread));
	if ( wDataSize != sizeof(DBR_GP_UserCanSpread)) return false;

	//类型转换
	DBR_GP_UserCanSpread *pReq = (DBR_GP_UserCanSpread*)pData;

	//执行存储过程
	LONG lReturnValue = -1;
	try
	{
		lReturnValue = SPUserCanSpread(pReq->dwUserID, pReq->szComputerID);
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);
	}

	//成功判断
	if ( lReturnValue == 0 )
	{
		DBR_GP_CommonResult result;
		ZeroMemory(&result, sizeof(result));
		
		m_AccountsDBAide.GetValue_String(TEXT("ErrorDescribe"),result.szDescribeMsg,CountArray(result.szDescribeMsg));

		result.iCode = lReturnValue;
		result.dwUserID = m_AccountsDBAide.GetValue_DWORD(TEXT("UserID"));

		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_GP_USER_CAN_SPREAD_RESULT,dwContextID,
			&result,sizeof(DBR_GP_CommonResult));
	}
	else
	{
		DBR_GP_CommonResult result;
		ZeroMemory(&result, sizeof(result));
		m_AccountsDBAide.GetValue_String(TEXT("ErrorDescribe"),result.szDescribeMsg,CountArray(result.szDescribeMsg));
		result.iCode = lReturnValue;
		result.dwUserID = m_AccountsDBAide.GetValue_DWORD(TEXT("UserID"));

		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_GP_USER_CAN_SPREAD_RESULT,dwContextID,
			&result,sizeof(DBR_GP_CommonResult));
	}
	return true;
}


//用户领取推广奖励	
bool CDataBaseSink::OnRequestUserGetSpread(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//参数验证
	ASSERT(wDataSize == sizeof(DBR_GP_UserGetSpread));
	if ( wDataSize != sizeof(DBR_GP_UserGetSpread)) return false;

	//类型转换
	DBR_GP_UserGetSpread *pReq = (DBR_GP_UserGetSpread*)pData;

	//执行存储过程
	LONG lReturnValue = -1;
	try
	{
		lReturnValue = SPUserGetSpread(pReq->dwUserID, pReq->dwSpreaderID, pReq->szComputerID);
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);
	}

	//成功判断
	if ( lReturnValue == 0 )
	{
		DBR_GP_UserGetSpreadResult result;
		ZeroMemory(&result, sizeof(result));
		
		m_AccountsDBAide.GetValue_String(TEXT("ErrorDescribe"),result.szDescribeMsg,CountArray(result.szDescribeMsg));

		result.iCode = lReturnValue;
		result.dwUserID = m_AccountsDBAide.GetValue_DWORD(TEXT("UserID"));
		result.dwSpreaderID = m_AccountsDBAide.GetValue_DWORD(TEXT("Spreader"));
		result.dwAmount = m_AccountsDBAide.GetValue_DWORD(TEXT("Amount"));
		result.dwType = m_AccountsDBAide.GetValue_DWORD(TEXT("Type"));

		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_GP_USER_GET_SPREAD_RESULT,dwContextID,
			&result,sizeof(DBR_GP_UserGetSpreadResult));
	}
	else
	{
		DBR_GP_UserGetSpreadResult result;
		ZeroMemory(&result, sizeof(result));
		m_AccountsDBAide.GetValue_String(TEXT("ErrorDescribe"),result.szDescribeMsg,CountArray(result.szDescribeMsg));
		result.iCode = lReturnValue;
		result.dwUserID = m_AccountsDBAide.GetValue_DWORD(TEXT("UserID"));

		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_GP_USER_GET_SPREAD_RESULT,dwContextID,
			&result,sizeof(DBR_GP_UserGetSpreadResult));
	}
	return true;
}

//推广员信息	
bool CDataBaseSink::OnRequestSpreaderInfo(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//参数验证
	ASSERT(wDataSize == sizeof(DBR_GP_UserSpreaderInfo));
	if ( wDataSize != sizeof(DBR_GP_UserSpreaderInfo)) return false;

	//类型转换
	DBR_GP_UserSpreaderInfo *pReq = (DBR_GP_UserSpreaderInfo*)pData;

	//执行存储过程
	LONG lReturnValue = -1;
	try
	{
		lReturnValue = SPUserGetSpreadInfo(pReq->dwSpreaderID);
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);
	}

	//成功判断
	if ( lReturnValue == 0 )
	{
		DBR_GP_UserSpreaderInfoResult result;
		ZeroMemory(&result, sizeof(result));

		//获取数据
		IDataBase * pIDataBase=(IDataBase *)m_AccountsDBAide.GetDataBase(IID_IDataBase,VER_IDataBase);

		//读取信息
		while (pIDataBase->IsRecordsetEnd()==false)
		{
			result.dwSpreaderID = m_AccountsDBAide.GetValue_DWORD(TEXT("S")); //推广员
			result.dwAmount = m_AccountsDBAide.GetValue_DWORD(TEXT("C"));  //总奖励数
			result.dwUsers = m_AccountsDBAide.GetValue_DWORD(TEXT("T"));   //总人数
			
			if (result.dwNum < 10)  //最多10条
			{
				m_AccountsDBAide.GetValue_String(TEXT("N"),result.vecNickNames[result.dwNum],CountArray(result.vecNickNames[result.dwNum]));
				result.iAward[result.dwNum] = m_AccountsDBAide.GetValue_DWORD(TEXT("M"));   //奖励数
				result.dwNum++;  //计数
			}
			else
			{
				break;
			}

			//移动记录
			pIDataBase->MoveToNext();
		}

		//关闭记录
		pIDataBase->CloseRecordset();

		result.iCode = lReturnValue;

		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_GP_USER_SPREADER_INFO_RESULT,dwContextID,
			&result,sizeof(DBR_GP_UserSpreaderInfoResult));
	}
	else
	{
		DBR_GP_UserSpreaderInfoResult result;
		ZeroMemory(&result, sizeof(result));
		m_AccountsDBAide.GetValue_String(TEXT("ErrorDescribe"),result.szDescribeMsg,CountArray(result.szDescribeMsg));
		result.iCode = lReturnValue;
		result.dwSpreaderID = m_AccountsDBAide.GetValue_DWORD(TEXT("S"));

		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_GP_USER_SPREADER_INFO_RESULT,dwContextID,
			&result,sizeof(DBR_GP_UserSpreaderInfoResult));
	}
	return true;
}

//弹出公告信息	
bool CDataBaseSink::OnRequestGetBbsPop(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//参数验证
	ASSERT(wDataSize == sizeof(DBR_GP_UserGetBbsPop));
	if ( wDataSize != sizeof(DBR_GP_UserGetBbsPop)) return false;
	
	//类型转换
	DBR_GP_UserGetBbsPop *pReq = (DBR_GP_UserGetBbsPop*)pData;

	//执行存储过程
	LONG lReturnValue = -1;
	try
	{
		lReturnValue = SPUserGetBbsPop(pReq->szChannel);
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);
	}

	//成功判断
	if ( lReturnValue == 0 )
	{
		DBR_GP_UserGetBbsPopResult result;
		ZeroMemory(&result, sizeof(result));

		//获取数据
		IDataBase * pIDataBase=(IDataBase *)m_AccountsDBAide.GetDataBase(IID_IDataBase,VER_IDataBase);

		//读取信息
		m_AccountsDBAide.GetValue_String(TEXT("Details"),result.szDetails,CountArray(result.szDetails));
		result.iAction = m_AccountsDBAide.GetValue_WORD(TEXT("Action")); 
		result.iScrollCount = m_AccountsDBAide.GetValue_DWORD(TEXT("ScrollCount"));  //滚动公告条数
		result.iListCount = m_AccountsDBAide.GetValue_DWORD(TEXT("ListCount"));		 //列表公告条数
		
		//最多3条
		if (result.iListCount > 3)result.iListCount=3;

		for (int i=0; i<result.iListCount; i++)
		{
			char buf[32];
			memset(buf, 0, 32);
			sprintf(buf, "List%d", i+1);
			result.dwListID[i] = m_AccountsDBAide.GetValue_DWORD(buf);		 //列表公告ID
		}

		result.iCode = lReturnValue;

		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_GP_USER_GET_BBS_POP_RESULT,dwContextID,
			&result,sizeof(DBR_GP_UserGetBbsPopResult));
	}
	else
	{
		DBR_GP_UserGetBbsPopResult result;
		ZeroMemory(&result, sizeof(result));
		result.iCode = lReturnValue;
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_GP_USER_GET_BBS_POP_RESULT,dwContextID,
			&result,sizeof(DBR_GP_UserGetBbsPopResult));
	}
	return true;
}

//滚动公告信息	
bool CDataBaseSink::OnRequestGetBbsScroll(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//参数验证
	ASSERT(wDataSize == sizeof(DBR_GP_UserGetBbsScroll));
	if ( wDataSize != sizeof(DBR_GP_UserGetBbsScroll)) return false;
	
	//类型转换
	DBR_GP_UserGetBbsPop *pReq = (DBR_GP_UserGetBbsPop*)pData;

	//执行存储过程
	LONG lReturnValue = -1;
	try
	{
		lReturnValue = SPUserGetBbsScroll(pReq->szChannel);
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);
	}

	//成功判断
	if ( lReturnValue == 0 )
	{
		DBR_GP_UserGetBbsScrollResult result;
		ZeroMemory(&result, sizeof(result));

		//获取数据
		IDataBase * pIDataBase=(IDataBase *)m_AccountsDBAide.GetDataBase(IID_IDataBase,VER_IDataBase);

		//读取信息
		int iCnt = 0;
		while (pIDataBase->IsRecordsetEnd()==false)
		{
			m_AccountsDBAide.GetValue_String(TEXT("Title"),result.szDetails[iCnt],CountArray(result.szDetails[iCnt]));

			iCnt++;

			//移动记录
			pIDataBase->MoveToNext();
		}

		//关闭记录
		pIDataBase->CloseRecordset();

		result.iCount = iCnt;
		result.iCode = lReturnValue;

		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_GP_USER_GET_BBS_SCROLL_RESULT,dwContextID,
			&result,sizeof(DBR_GP_UserGetBbsScrollResult));
	}
	else
	{
		DBR_GP_UserGetBbsScrollResult result;
		ZeroMemory(&result, sizeof(result));
		result.iCode = lReturnValue;
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_GP_USER_GET_BBS_SCROLL_RESULT,dwContextID,
			&result,sizeof(DBR_GP_UserGetBbsScrollResult));
	}
	return true;
}

//列表公告信息	
bool CDataBaseSink::OnRequestGetBbsList(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//参数验证
	ASSERT(wDataSize == sizeof(DBR_GP_UserGetBbsList));
	if ( wDataSize != sizeof(DBR_GP_UserGetBbsList)) return false;
	
	//类型转换
	DBR_GP_UserGetBbsPop *pReq = (DBR_GP_UserGetBbsPop*)pData;

	//执行存储过程
	LONG lReturnValue = -1;
	try
	{
		lReturnValue = SPUserGetBbsList(pReq->szChannel);
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);
	}

	//成功判断
	if ( lReturnValue == 0 )
	{
		DBR_GP_UserGetBbsListResult result;
		ZeroMemory(&result, sizeof(result));

		//获取数据
		IDataBase * pIDataBase=(IDataBase *)m_AccountsDBAide.GetDataBase(IID_IDataBase,VER_IDataBase);

		//读取信息
		int iCnt = 0;
		while (pIDataBase->IsRecordsetEnd()==false)
		{
			m_AccountsDBAide.GetValue_String(TEXT("Title"),result.szTitle[iCnt],CountArray(result.szTitle[iCnt]));
			m_AccountsDBAide.GetValue_String(TEXT("Details"),result.szDetails[iCnt],CountArray(result.szDetails[iCnt]));
			m_AccountsDBAide.GetValue_String(TEXT("D"),result.szDate[iCnt], CountArray(result.szDate[iCnt]));  //日期
			
			result.iType[iCnt] = m_AccountsDBAide.GetValue_WORD(TEXT("Action")); //公告种类:由GM与界面协调
			result.iID[iCnt] = m_AccountsDBAide.GetValue_DWORD(TEXT("ID"));  
			iCnt++;

			//移动记录
			pIDataBase->MoveToNext();
		}

		//关闭记录
		pIDataBase->CloseRecordset();

		result.iCount = iCnt;
		result.iCode = lReturnValue;

		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_GP_USER_GET_BBS_LIST_RESULT,dwContextID,
			&result,sizeof(DBR_GP_UserGetBbsListResult));
	}
	else
	{
		DBR_GP_UserGetBbsListResult result;
		ZeroMemory(&result, sizeof(result));
		result.iCode = lReturnValue;
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_GP_USER_GET_BBS_LIST_RESULT,dwContextID,
			&result,sizeof(DBR_GP_UserGetBbsListResult));
	}
	return true;
}

//玩家任务信息	
bool CDataBaseSink::OnRequestGetTaskInfo(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//参数验证
	ASSERT(wDataSize == sizeof(DBR_GP_UserGetTaskInfo));
	if ( wDataSize != sizeof(DBR_GP_UserGetTaskInfo)) return false;
	
	//类型转换
	DBR_GP_UserGetTaskInfo *pReq = (DBR_GP_UserGetTaskInfo*)pData;

	//执行存储过程
	LONG lReturnValue = -1;
	try
	{
		lReturnValue = SPUserGetTaskInfo(pReq->dwUserID);
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);
	}

	//成功判断
	if ( lReturnValue == 0 )
	{
		DBR_GP_UserGetTaskInfoResult result;
		ZeroMemory(&result, sizeof(result));

		//获取数据
		IDataBase * pIDataBase=(IDataBase *)m_AccountsDBAide.GetDataBase(IID_IDataBase,VER_IDataBase);

		//读取信息
		result.dwUserID = pReq->dwUserID;
		result.iWinToday3 = m_AccountsDBAide.GetValue_DWORD(TEXT("wtoday3"));
		result.iLostToday3 = m_AccountsDBAide.GetValue_DWORD(TEXT("ltoday3"));
		result.iWinTotal3 = m_AccountsDBAide.GetValue_DWORD(TEXT("wtotal3"));
		result.iLostTotal3 = m_AccountsDBAide.GetValue_DWORD(TEXT("ltotal3"));
		result.iAwardToday3 = m_AccountsDBAide.GetValue_DWORD(TEXT("atoday3"));
		result.iAwardTotal3 = m_AccountsDBAide.GetValue_DWORD(TEXT("atotal3"));
		result.iWinToday4 = m_AccountsDBAide.GetValue_DWORD(TEXT("wtoday4"));
		result.iLostToday4 = m_AccountsDBAide.GetValue_DWORD(TEXT("ltoday4"));
		result.iWinTotal4 = m_AccountsDBAide.GetValue_DWORD(TEXT("wtotal4"));
		result.iLostTotal4 = m_AccountsDBAide.GetValue_DWORD(TEXT("ltotal4"));
		result.iAwardToday4 = m_AccountsDBAide.GetValue_DWORD(TEXT("atoday4"));
		result.iAwardTotal4 = m_AccountsDBAide.GetValue_DWORD(TEXT("atotal4"));

		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_GP_USER_GET_TASK_INFO_RESULT,dwContextID,
			&result,sizeof(DBR_GP_UserGetTaskInfoResult));
	}
	else
	{
		DBR_GP_UserGetTaskInfoResult result;
		ZeroMemory(&result, sizeof(result));
		result.iCode = lReturnValue;
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_GP_USER_GET_TASK_INFO_RESULT,dwContextID,
			&result,sizeof(DBR_GP_UserGetTaskInfoResult));
	}
	return true;
}

//玩家排行榜
bool CDataBaseSink::OnRequestGetRanking(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//数据验证
	ASSERT(wDataSize==sizeof(DBR_GP_UserGetRanking));
	if (wDataSize!=sizeof(DBR_GP_UserGetRanking)) return false;

	//数据转换
	DBR_GP_UserGetRanking *pReq = (DBR_GP_UserGetRanking*)pData;

		//执行存储过程
	LONG lReturnValue2 = -1;
	try
	{
		lReturnValue2 = SPUserRankingStatus(pReq->dwUserID);
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);
	}

	//成功判断
	if ( lReturnValue2 == 0 )
	{
		DBR_GP_UserGetRankingStatusResult RankingStatusResult;
		ZeroMemory(&RankingStatusResult, sizeof(RankingStatusResult));

		//获取数据
		IDataBase * pIDataBase=(IDataBase *)m_AccountsDBAide.GetDataBase(IID_IDataBase,VER_IDataBase);

		int iRankingStatus = -1;
		
		//读取信息
		iRankingStatus = m_AccountsDBAide.GetValue_INT(TEXT("RS"));
			
		//关闭记录
		pIDataBase->CloseRecordset();

		RankingStatusResult.dwUserID = pReq->dwUserID;
		RankingStatusResult.isInRanking = iRankingStatus;

		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_GP_USER_GET_RANKINGSTATUS,dwContextID,
			&RankingStatusResult,sizeof(DBR_GP_UserGetRankingStatusResult));
	}
	else
	{
		DBR_GP_UserGetRankingStatusResult RankingStatusResult;
		ZeroMemory(&RankingStatusResult, sizeof(RankingStatusResult));
		RankingStatusResult.dwUserID = pReq->dwUserID;
		RankingStatusResult.isInRanking = -1;

		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_GP_USER_GET_RANKINGSTATUS,dwContextID,
			&RankingStatusResult,sizeof(RankingStatusResult));
	}

	//执行存储过程
	LONG lReturnValue = -1;
	try
	{
		if (pReq->nPage < 1)pReq->nPage = 1;
		lReturnValue = SPUserRanking(pReq->dwUserID, pReq->nPage - 1);
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);
	}

	//成功判断
	if ( lReturnValue == 0 )
	{
		DBR_GP_UserGetRankingResult result;
		ZeroMemory(&result, sizeof(result));

		

		//获取数据
		IDataBase * pIDataBase=(IDataBase *)m_AccountsDBAide.GetDataBase(IID_IDataBase,VER_IDataBase);

		//读取信息
		int iTotal = 0;
		int iMine = 0;
		
		while (pIDataBase->IsRecordsetEnd()==false)
		{
			DBR_GP_UserRankingRecord record;
			ZeroMemory(&record, sizeof(record));

			//读取信息
			iTotal = m_AccountsDBAide.GetValue_DWORD(TEXT("T"));
			iMine = m_AccountsDBAide.GetValue_INT(TEXT("Mine"));
			record.nIndex=m_AccountsDBAide.GetValue_DWORD(TEXT("ID"));
			record.lGoldScore=m_AccountsDBAide.GetValue_LONG(TEXT("Score"));
			record.dwUserID=m_AccountsDBAide.GetValue_DWORD(TEXT("UserID"));
			record.cbGender=m_AccountsDBAide.GetValue_INT(TEXT("Gender"));
			m_AccountsDBAide.GetValue_String(TEXT("NickName"),record.szNickName,CountArray(record.szNickName));
			m_AccountsDBAide.GetValue_String(TEXT("D"),result.szDate,CountArray(result.szDate));

			//移动记录
			pIDataBase->MoveToNext();

			//插入信息
			result.vecRecords.push_back(record);
		}

		//关闭记录
		pIDataBase->CloseRecordset();

		result.dwUserID = pReq->dwUserID;
		result.dwTotal = iTotal;
		result.nMyIndex = iMine;
		
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_GP_USER_GET_RANKING_RESULT,dwContextID,
				&result,sizeof(DBR_GP_UserGetRankingResult));
	}
	else
	{
		DBR_GP_UserGetRankingResult result;
		ZeroMemory(&result, sizeof(result));
		result.iCode = lReturnValue;

		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_GP_USER_GET_RANKING_RESULT,dwContextID,
			&result,sizeof(result));
	}



	return true;
}

//玩家排行榜
bool CDataBaseSink::OnRequestGetRanking_V2(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//数据验证
	ASSERT(wDataSize==sizeof(DBR_GP_UserGetRanking));
	if (wDataSize!=sizeof(DBR_GP_UserGetRanking)) return false;

	//数据转换
	DBR_GP_UserGetRanking *pReq = (DBR_GP_UserGetRanking*)pData;

		//执行存储过程
	LONG lReturnValue2 = -1;
	try
	{
		lReturnValue2 = SPUserRankingStatus(pReq->dwUserID);
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);
	}

	//成功判断
	if ( lReturnValue2 == 0 )
	{
		DBR_GP_UserGetRankingStatusResult RankingStatusResult;
		ZeroMemory(&RankingStatusResult, sizeof(RankingStatusResult));

		//获取数据
		IDataBase * pIDataBase=(IDataBase *)m_AccountsDBAide.GetDataBase(IID_IDataBase,VER_IDataBase);

		int iRankingStatus = -1;

		//读取信息
		iRankingStatus = m_AccountsDBAide.GetValue_INT(TEXT("RS"));

		//关闭记录
		pIDataBase->CloseRecordset();

		RankingStatusResult.dwUserID = pReq->dwUserID;
		RankingStatusResult.isInRanking = iRankingStatus;

		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_GP_USER_GET_RANKINGSTATUS,dwContextID,
			&RankingStatusResult,sizeof(DBR_GP_UserGetRankingStatusResult));
	}
	else
	{
		DBR_GP_UserGetRankingStatusResult RankingStatusResult;
		ZeroMemory(&RankingStatusResult, sizeof(RankingStatusResult));
		RankingStatusResult.dwUserID = pReq->dwUserID;
		RankingStatusResult.isInRanking = -1;

		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_GP_USER_GET_RANKINGSTATUS,dwContextID,
			&RankingStatusResult,sizeof(RankingStatusResult));
	}

	//执行存储过程
	LONG lReturnValue = -1;
	try
	{
		if (pReq->nPage < 1)pReq->nPage = 1;
		lReturnValue = SPUserRanking_V2(pReq->dwUserID, pReq->nPage - 1);
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);
	}

	//成功判断
	if ( lReturnValue == 0 )
	{
		DBR_GP_UserGetRankingResult_V2 result;
		ZeroMemory(&result, sizeof(result));



		//获取数据
		IDataBase * pIDataBase=(IDataBase *)m_AccountsDBAide.GetDataBase(IID_IDataBase,VER_IDataBase);

		//读取信息
		int iTotal = 0;
		int iMine = 0;

		while (pIDataBase->IsRecordsetEnd()==false)
		{
			DBR_GP_UserRankingRecord_V2 record;
			ZeroMemory(&record, sizeof(record));

			//读取信息
			iTotal = m_AccountsDBAide.GetValue_DWORD(TEXT("T"));
			iMine = m_AccountsDBAide.GetValue_INT(TEXT("Mine"));
			record.nIndex=m_AccountsDBAide.GetValue_DWORD(TEXT("ID"));
			record.lGoldScore=m_AccountsDBAide.GetValue_LONG(TEXT("Score"));
			record.dwUserID=m_AccountsDBAide.GetValue_DWORD(TEXT("UserID"));
			record.cbGender=m_AccountsDBAide.GetValue_INT(TEXT("Gender"));
			m_AccountsDBAide.GetValue_String(TEXT("NickName"),record.szNickName,CountArray(record.szNickName));
			m_AccountsDBAide.GetValue_String(TEXT("DescribeInfo"),record.szDescribe,CountArray(record.szDescribe));
			m_AccountsDBAide.GetValue_String(TEXT("D"),result.szDate,CountArray(result.szDate));

			//移动记录
			pIDataBase->MoveToNext();

			//插入信息
			result.vecRecords.push_back(record);
		}

		//关闭记录
		pIDataBase->CloseRecordset();

		result.dwUserID = pReq->dwUserID;
		result.dwTotal = iTotal;
		result.nMyIndex = iMine;

		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_GP_USER_GET_RANKING_RESULT_V2,dwContextID,
			&result,sizeof(DBR_GP_UserGetRankingResult_V2));
	}
	else
	{
		DBR_GP_UserGetRankingResult result;
		ZeroMemory(&result, sizeof(result));
		result.iCode = lReturnValue;

		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_GP_USER_GET_RANKING_RESULT_V2,dwContextID,
			&result,sizeof(result));
	}

	return true;
}

//修改玩家是否入排行榜
bool CDataBaseSink::OnRequestModifyRankingStatus(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//数据验证
	ASSERT(wDataSize==sizeof(DBR_GP_UserModifyRankingStatus));
	if (wDataSize!=sizeof(DBR_GP_UserModifyRankingStatus)) return false;

	//数据转换
	DBR_GP_UserModifyRankingStatus *pReq = (DBR_GP_UserModifyRankingStatus*)pData;

	DBR_GP_UserModifyRankingStatusResult result;
	ZeroMemory(&result, sizeof(result));
	//执行存储过程
	LONG lReturnValue = -1;
	try
	{
		lReturnValue = SPUserModifyRankingStatus(pReq->dwUserID, pReq->isInRanking);
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);
	}

	if(0 == lReturnValue)
	{
		result.iCode = pReq->isInRanking;
	}
	else
	{
		result.iCode = lReturnValue;
	}
	
	result.dwUserID = pReq->dwUserID;
	m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_GP_USER_GET_MODIFY_RANKINGSTATUS,dwContextID,
		&result,sizeof(result));

	return true;
}

//玩家反馈意见
bool CDataBaseSink::OnRequestFeedBack(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//数据验证
	ASSERT(wDataSize==sizeof(DBR_GP_UserFeedBack));
	if (wDataSize!=sizeof(DBR_GP_UserFeedBack)) return false;

	//数据转换
	DBR_GP_UserFeedBack *pReq = (DBR_GP_UserFeedBack*)pData;
	
	DBR_GP_UserFeedBackResult result;
	ZeroMemory(&result, sizeof(result));

	//执行存储过程
	LONG lReturnValue = -1;
	try
	{
		lReturnValue = SPUserFeedBack(pReq->dwUserID, pReq->szContent, pReq->szPicture);
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);

		lstrcpyn(result.szDetails,TEXT("数据库操作异常，请您稍后重试！"),CountArray(result.szDetails));
	}
	result.iCode = lReturnValue;

	m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_GP_USER_FEEDBACK_RESULT,dwContextID,
		&result,sizeof(result));

	return true;
}

//查询玩家反馈的意见
bool CDataBaseSink::OnRequestGetFeedBack(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//数据验证
	ASSERT(wDataSize==sizeof(DBR_GP_UserGetFeedBack));
	if (wDataSize!=sizeof(DBR_GP_UserGetFeedBack)) return false;

	//数据转换
	DBR_GP_UserGetFeedBack *pReq = (DBR_GP_UserGetFeedBack*)pData;

	//执行存储过程
	LONG lReturnValue = -1;
	try
	{
		if (pReq->nPage < 1)pReq->nPage = 1;
		lReturnValue = SPUserGetFeedBack(pReq->dwUserID, pReq->nPage - 1);
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);
	}

	//成功判断
	if ( lReturnValue == 0 )
	{
		DBR_GP_UserGetFeedBackResult result;
		ZeroMemory(&result, sizeof(result));

		//获取数据
		IDataBase * pIDataBase=(IDataBase *)m_AccountsDBAide.GetDataBase(IID_IDataBase,VER_IDataBase);

		//读取信息
		int iTotal = 0;
		while (pIDataBase->IsRecordsetEnd()==false)
		{
			DBR_GP_UserFeedBackRecord record;
			ZeroMemory(&record, sizeof(record));

			//读取信息
			iTotal = m_AccountsDBAide.GetValue_DWORD(TEXT("T"));
			m_AccountsDBAide.GetValue_String(TEXT("Question"),record.szQuestion,CountArray(record.szQuestion));
			m_AccountsDBAide.GetValue_String(TEXT("Reply"),record.szAnswer,CountArray(record.szAnswer));
			m_AccountsDBAide.GetValue_String(TEXT("AttachPath"),record.szPicture,CountArray(record.szPicture));
			m_AccountsDBAide.GetValue_String(TEXT("D"),record.szDate,CountArray(record.szDate));

			if (std::strlen(record.szPicture)>0)record.cbHavePic = 1;

			//移动记录
			pIDataBase->MoveToNext();

			//插入信息
			result.vecRecords.push_back(record);
		}

		//关闭记录
		pIDataBase->CloseRecordset();

		result.dwUserID = pReq->dwUserID;
		result.dwTotal = iTotal;

		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_GP_USER_GET_FEEDBACK_RESULT,dwContextID,
			&result,sizeof(DBR_GP_UserGetFeedBackResult));
	}
	else
	{
		DBR_GP_UserGetFeedBackResult result;
		ZeroMemory(&result, sizeof(result));
		result.iCode = lReturnValue;

		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_GP_USER_GET_FEEDBACK_RESULT,dwContextID,
			&result,sizeof(result));
	}

	return true;
}

//查询用户身份证账号信息
bool CDataBaseSink::OnRequestUserInfoIDCard(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//数据验证
	ASSERT(wDataSize==sizeof(DBR_GP_UserInfoIDCard));
	if (wDataSize!=sizeof(DBR_GP_UserInfoIDCard)) return false;

	//数据转换
	DBR_GP_UserInfoIDCard *pReq = (DBR_GP_UserInfoIDCard*)pData;

	//执行存储过程
	LONG lReturnValue = -1;
	try
	{
		lReturnValue = SPUserInfoIDCard(pReq->dwUserID);
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);
	}

	//成功判断
	if ( lReturnValue == 0 )
	{
		DBR_GP_CommonResult result;
		ZeroMemory(&result, sizeof(result));

		//获取数据
		IDataBase * pIDataBase=(IDataBase *)m_AccountsDBAide.GetDataBase(IID_IDataBase,VER_IDataBase);
		
		//读取信息
		result.dwUserID = pReq->dwUserID;
		result.iCode = lReturnValue;
		m_AccountsDBAide.GetValue_String(TEXT("IDCard"),result.szDescribeMsg,CountArray(result.szDescribeMsg));

		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_GP_USER_INFO_ID_CARD_RESULT,dwContextID,
			&result,sizeof(DBR_GP_CommonResult));
	}
	else
	{
		DBR_GP_CommonResult result;
		ZeroMemory(&result, sizeof(result));
		
		//读取信息
		result.dwUserID = pReq->dwUserID;
		result.iCode = lReturnValue;

		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_GP_USER_INFO_ID_CARD_RESULT,dwContextID,
			&result,sizeof(result));
	}

	return true;
}

//请求提交身份证信息
bool CDataBaseSink::OnRequestUserAuthentication(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//数据验证
	ASSERT(wDataSize==sizeof(DBR_GP_UserAuthentication));
	if (wDataSize!=sizeof(DBR_GP_UserAuthentication)) return false;

	//数据转换
	DBR_GP_UserAuthentication *pReq = (DBR_GP_UserAuthentication*)pData;

	//执行存储过程
	LONG lReturnValue = -1;
	try
	{
		lReturnValue = SPUserAuthentication(pReq->dwUserID, pReq->szName, pReq->szIdCardNum);
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);
	}

	//结果判断
	DBR_GP_CommonResult result;
	ZeroMemory(&result, sizeof(result));

	//获取数据
	IDataBase * pIDataBase=(IDataBase *)m_AccountsDBAide.GetDataBase(IID_IDataBase,VER_IDataBase);
		
	//读取信息
	result.dwUserID = pReq->dwUserID;
	result.iCode = lReturnValue;

	m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_GP_USER_AUTHENTICATION_RESULT,dwContextID,
		&result,sizeof(DBR_GP_CommonResult));	

	return true;
}

//请求商城商品信息
bool CDataBaseSink::OnRequestMallProduct(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize, bool isIOS)
{
	//数据验证
	ASSERT(wDataSize==sizeof(DBR_GP_UserMallProduct));
	if (wDataSize!=sizeof(DBR_GP_UserMallProduct)) return false;

	//数据转换
	DBR_GP_UserMallProduct *pMallProduct = (DBR_GP_UserMallProduct*)pData;

	//执行存储过程
	LONG lReturnValue = -1;
	try
	{
		if (isIOS == true)
			lReturnValue = SPUserMallProductIOS(pMallProduct->dwUserID);
		else
			lReturnValue = SPUserMallProduct(pMallProduct->dwUserID);
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);
	}

	//成功判断
	if ( lReturnValue == 0 )
	{
		DBR_GP_UserMallProductResultFinish mallProductFinish;
		ZeroMemory(&mallProductFinish, sizeof(mallProductFinish));
		mallProductFinish.bOperateSuccess = true;

		//获取数据
		IDataBase * pIDataBase=(IDataBase *)m_AccountsDBAide.GetDataBase(IID_IDataBase,VER_IDataBase);

		//读取信息
		while (pIDataBase->IsRecordsetEnd()==false)
		{
			DBR_GP_UserMallProductResult product;
			//读取信息
			product.productID=m_AccountsDBAide.GetValue_DWORD(TEXT("productID"));
			product.productPrice=m_AccountsDBAide.GetValue_DWORD(TEXT("productPrice"));
			m_AccountsDBAide.GetValue_String(TEXT("productImg"),product.szProductImage,CountArray(product.szProductImage));
			m_AccountsDBAide.GetValue_String(TEXT("productName"),product.szProductName,CountArray(product.szProductName));

			product.version = pMallProduct->dwVersion;
			if (pMallProduct->dwVersion  == 1 || pMallProduct->dwVersion  == 2)
			{
				//增加了2个字段
				product.cbHotFlag = m_AccountsDBAide.GetValue_INT(TEXT("HotFlag")); 
				m_AccountsDBAide.GetValue_String(TEXT("ProductNameAdd"),product.szProductNameAdd,CountArray(product.szProductNameAdd));
			}

			//移动记录
			pIDataBase->MoveToNext();

			//插入信息
			mallProductFinish.vecProducts.push_back(product);
		}

		//关闭记录
		pIDataBase->CloseRecordset();

		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_GP_USER_MALL_PRODUCT_RESULT,dwContextID,
			&mallProductFinish,sizeof(mallProductFinish));
	}
	else
	{
		DBR_GP_UserMallProductResultFinish mallProductResult;
		ZeroMemory(&mallProductResult, sizeof(mallProductResult));
		mallProductResult.bOperateSuccess = false;

		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_GP_USER_MALL_PRODUCT_RESULT,dwContextID,
			&mallProductResult,sizeof(mallProductResult));
	}

	return true;
}

//请求商城订单
bool CDataBaseSink::OnRequestMallPlaceOrder(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//数据验证
	ASSERT(wDataSize==sizeof(DBR_GP_UserMallPlaceOrder));
	if (wDataSize!=sizeof(DBR_GP_UserMallPlaceOrder)) return false;

	//数据转换
	DBR_GP_UserMallPlaceOrder *pOrder = (DBR_GP_UserMallPlaceOrder*)pData;

	//执行存储过程
	LONG lReturnValue = -1;
	try
	{
		lReturnValue = SPUserMallPlaceOrder(pOrder->dwUserID, pOrder->dwProductID,pOrder->szPayWay);
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);
	}

	//成功判断
	if ( lReturnValue == 0 )
	{
		DBR_GP_UserMallPlaceOrderResult orderResult;
		ZeroMemory(&orderResult, sizeof(orderResult));
		orderResult.nResult = true;
		m_AccountsDBAide.GetValue_String(TEXT("dwOrderID"),orderResult.dwOrderID,CountArray(orderResult.dwOrderID));
		m_AccountsDBAide.GetValue_String(TEXT("productName"),orderResult.productName,CountArray(orderResult.productName));
		m_AccountsDBAide.GetValue_String(TEXT("token"),orderResult.token,CountArray(orderResult.token));
		orderResult.productPrice = m_AccountsDBAide.GetValue_DWORD(TEXT("productPrice"));
		m_AccountsDBAide.GetValue_String(TEXT("billingIndex"),orderResult.billingCode,CountArray(orderResult.billingCode));

		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_GP_USER_MALL_PLACE_ORDER_RESULT,dwContextID,
			&orderResult,sizeof(orderResult));
	}
	else
	{
		DBR_GP_UserMallPlaceOrderResult orderResult;
		ZeroMemory(&orderResult, sizeof(orderResult));
		orderResult.nResult = false;

		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_GP_USER_MALL_PLACE_ORDER_RESULT,dwContextID,
			&orderResult,sizeof(orderResult));
	}

	return true;
}

bool CDataBaseSink::OnRequestUserCancelOrder(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//数据验证
	ASSERT(wDataSize==sizeof(DBR_Cancel_Order_Num));
	if (wDataSize!=sizeof(DBR_Cancel_Order_Num)) return false;

	//数据转换
	DBR_Cancel_Order_Num *pOrder = (DBR_Cancel_Order_Num*)pData;

	//执行存储过程
	LONG lReturnValue = -1;
	try
	{
		lReturnValue = SPUserCancelOrder(pOrder->orderNum);
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);
	}

	//成功判断
	if ( lReturnValue == 0 )
	{
		DBR_Cancel_Order_Resut orderResult;
		ZeroMemory(&orderResult, sizeof(orderResult));

		orderResult.iCode = 0;
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_GP_USER_CANCEL_ORDER_NUM,dwContextID,
			&orderResult,sizeof(orderResult));
	}
	else
	{
		DBR_Cancel_Order_Resut orderResult;
		ZeroMemory(&orderResult, sizeof(orderResult));

		orderResult.iCode = -1;
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_GP_USER_CANCEL_ORDER_NUM,dwContextID,
			&orderResult,sizeof(orderResult));
	}

	return true;
}

//请求商城商品购买结果
bool CDataBaseSink::OnRequestMallBuy(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//数据验证
	ASSERT(wDataSize==sizeof(DBR_GP_UserMallBuy));
	if (wDataSize!=sizeof(DBR_GP_UserMallBuy)) return false;

	//数据转换
	DBR_GP_UserMallBuy *pReq = (DBR_GP_UserMallBuy*)pData;

	//执行存储过程
	LONG lReturnValue = -1;
	try
	{
		lReturnValue = SPUserMallBuy(pReq->dwOrderID);
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);
	}

	//成功判断
	DBR_GP_UserMallBuyResult result;
	ZeroMemory(&result, sizeof(result));

	result.iCode = lReturnValue;
	result.dwUserID = m_AccountsDBAide.GetValue_UINT(TEXT("UserID"));
	result.productID = m_AccountsDBAide.GetValue_INT(TEXT("productID"));
	result.dwGoldScore = m_AccountsDBAide.GetValue_UINT(TEXT("dwGoldScore"));
	m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_GP_USER_MALL_BUY_RESULT, dwContextID,
		&result,sizeof(result));

	return true;
}

//请求商城查询订单号更新结果
bool CDataBaseSink::OnRequestMallUpdateBuy(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//数据验证
	ASSERT(wDataSize==sizeof(DBR_GP_UserMallUpdate));
	if (wDataSize!=sizeof(DBR_GP_UserMallUpdate)) return false;

	//数据转换
	DBR_GP_UserMallUpdate *pReq = (DBR_GP_UserMallUpdate*)pData;

	//执行存储过程
	LONG lReturnValue = -1;
	try
	{
		lReturnValue = SPUserMallUpdateBuy(pReq->dwOrderID);
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);
	}

	//成功判断
	DBR_GP_UserMallUpdateResult result;
	ZeroMemory(&result, sizeof(result));

	result.iCode = lReturnValue;
	result.dwUserID = m_AccountsDBAide.GetValue_UINT(TEXT("UserID"));
	result.productID = m_AccountsDBAide.GetValue_INT(TEXT("productID"));
	result.dwGoldScore = m_AccountsDBAide.GetValue_UINT(TEXT("dwGoldScore"));
	m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_GP_USER_MALL_UPDATE_RESULT, dwContextID,
		&result,sizeof(result));

	return true;
}

//请求查询用户银行信息
bool CDataBaseSink::OnRequestUserBankInfo(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//数据验证
	ASSERT(wDataSize==sizeof(DBR_GP_UserBankInfo));
	if (wDataSize!=sizeof(DBR_GP_UserBankInfo)) return false;

	//数据转换
	DBR_GP_UserBankInfo *pReq = (DBR_GP_UserBankInfo*)pData;

	//执行存储过程
	LONG lReturnValue = -1;
	try
	{
		lReturnValue = SPUserBankInfo(pReq->dwUserID);
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);
	}

	//成功判断
	if ( lReturnValue == 0 )
	{
		DBR_GP_UserBankInfoResult result;
		ZeroMemory(&result, sizeof(result));
		result.nResult = lReturnValue;
		result.dwUserID = pReq->dwUserID;
		result.dwCurScore = m_AccountsDBAide.GetValue_DWORD(TEXT("curScore"));
		result.dwBankScore = m_AccountsDBAide.GetValue_DWORD(TEXT("bankScore"));

		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_GP_USER_BANK_INFO_RESULT,dwContextID,
			&result,sizeof(result));
	}
	else
	{
		DBR_GP_UserBankInfoResult result;
		ZeroMemory(&result, sizeof(result));
		result.nResult = lReturnValue;
		result.dwUserID = pReq->dwUserID;

		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_GP_USER_BANK_INFO_RESULT,dwContextID,
			&result,sizeof(result));
	}

	return true;
}

//请求操作银行
bool CDataBaseSink::OnRequestUserBankCharge(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//数据验证
	ASSERT(wDataSize==sizeof(DBR_GP_UserBankCharge));
	if (wDataSize!=sizeof(DBR_GP_UserBankCharge)) return false;

	//数据转换
	DBR_GP_UserBankCharge *pReq = (DBR_GP_UserBankCharge*)pData;

	//执行存储过程
	LONG lReturnValue = -1;
	try
	{
		lReturnValue = SPUserBankCharge(pReq->nOpCode, pReq->dwUserID, pReq->dwCurScore, pReq->dwBankScore);
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);
	}

	//成功判断
	if ( lReturnValue == 0 )
	{
		DBR_GP_UserBankChargeResult result;
		ZeroMemory(&result, sizeof(result));
		result.nResult = lReturnValue;
		result.dwUserID = pReq->dwUserID;
		result.dwCurScore = pReq->dwCurScore;
		result.dwBankScore = pReq->dwBankScore;

		//result.dwCurScore = m_AccountsDBAide.GetValue_DWORD(TEXT("curScore"));
		//result.dwBankScore = m_AccountsDBAide.GetValue_DWORD(TEXT("bankScore"));

		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_GP_USER_BANK_CHARGE_RESULT,dwContextID,
			&result,sizeof(result));
	}
	else
	{
		DBR_GP_UserBankInfoResult result;
		ZeroMemory(&result, sizeof(result));
		result.nResult = lReturnValue;
		result.dwUserID = pReq->dwUserID;

		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_GP_USER_BANK_CHARGE_RESULT,dwContextID,
			&result,sizeof(result));
	}

	return true;
}

//查询活动列表
bool CDataBaseSink::OnRequestActivityList(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//执行存储过程
	LONG lReturnValue = -1;
	try
	{
		lReturnValue = SPUserActivityList();
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);
	}
	
	DBR_GP_UserActivityListResult result;
	ZeroMemory(&result, sizeof(result));
	result.iCode = lReturnValue;

	//成功判断
	if ( lReturnValue == 0 )
	{
		//获取数据
		IDataBase * pIDataBase=(IDataBase *)m_AccountsDBAide.GetDataBase(IID_IDataBase,VER_IDataBase);

		//读取信息
		int iCnt = 0;
		while (pIDataBase->IsRecordsetEnd()==false)
		{
			m_AccountsDBAide.GetValue_String(TEXT("Title"),result.szTitle[iCnt],CountArray(result.szTitle[iCnt]));
			m_AccountsDBAide.GetValue_String(TEXT("Text"),result.szDetails[iCnt],CountArray(result.szDetails[iCnt]));
			m_AccountsDBAide.GetValue_String(TEXT("IconUrl"),result.szIconUrl[iCnt],CountArray(result.szIconUrl[iCnt]));
			m_AccountsDBAide.GetValue_String(TEXT("LinkUrl"),result.szLinkUrl[iCnt],CountArray(result.szLinkUrl[iCnt]));
			
			result.iType[iCnt] = m_AccountsDBAide.GetValue_WORD(TEXT("Type")); 
			result.iStatus[iCnt] = m_AccountsDBAide.GetValue_DWORD(TEXT("Status"));  
			//result.iStatus[iCnt] = m_AccountsDBAide.GetValue_DWORD(TEXT("Status")); 

			TCHAR szStatus[16]=TEXT("");
			m_AccountsDBAide.GetValue_String(TEXT("Status"),szStatus,sizeof(szStatus));
			result.iStatus[iCnt] = atoi(szStatus);
			result.iID[iCnt] = m_AccountsDBAide.GetValue_DWORD(TEXT("ID"));  

			iCnt++;

			//移动记录
			pIDataBase->MoveToNext();
		}

		//关闭记录
		pIDataBase->CloseRecordset();

		result.iCount = iCnt;

		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_GP_USER_ACTIVITY_LIST_RESULT,dwContextID,
			&result,sizeof(result));
	}
	else
	{
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_GP_USER_ACTIVITY_LIST_RESULT,dwContextID,
			&result,sizeof(result));
	}

	return true;
}

//查询活动
bool CDataBaseSink::OnRequestActivity(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//数据验证
	ASSERT(wDataSize==sizeof(DBR_GP_UserActivity));
	if (wDataSize!=sizeof(DBR_GP_UserActivity)) return false;

	//数据转换
	DBR_GP_UserActivity *pReq = (DBR_GP_UserActivity*)pData;

	//执行存储过程
	LONG lReturnValue = -1;
	try
	{
		lReturnValue = SPUserActivity(pReq->dwUserID, pReq->dwActivityID);
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);
	}
	
	DBR_GP_UserActivityResult result;
	ZeroMemory(&result, sizeof(result));
	result.iCode = lReturnValue;

	result.dwActivityID =  pReq->dwActivityID;
	result.dwUserID =  pReq->dwUserID;

	//成功判断
	if ( lReturnValue == 0 )
	{
		//获取数据
		IDataBase * pIDataBase=(IDataBase *)m_AccountsDBAide.GetDataBase(IID_IDataBase,VER_IDataBase);

		//读取信息
		int iCnt = 0;
		while (pIDataBase->IsRecordsetEnd()==false)
		{
			//最多5条
			if (iCnt == 5)break;

			//重复信息，只读一次
			if (iCnt == 0)
			{
				//最多抽奖次数
				DWORD dwMax = m_AccountsDBAide.GetValue_DWORD(TEXT("M"));
				//已使用的抽奖次数
				DWORD dwUsed = m_AccountsDBAide.GetValue_DWORD(TEXT("N"));
				//总盘数
				DWORD dwTotal = m_AccountsDBAide.GetValue_DWORD(TEXT("T"));
				//多少盘抽一次
				DWORD dwUnit = m_AccountsDBAide.GetValue_DWORD(TEXT("U"));
				char buf[128];
				memset(buf, 0, 128);
				sprintf(buf, "m:%d|n:%d|t:%d|u:%d", dwMax, dwUsed, dwTotal, dwUnit);
				memcpy(result.szParam, buf, CountArray(result.szParam));
		
				m_AccountsDBAide.GetValue_String(TEXT("ImageUrl"), result.szResourceUrl, CountArray(result.szResourceUrl));
			}

			//走马灯信息
			TCHAR szUserName[32];
			TCHAR szPrizeName[32];

			m_AccountsDBAide.GetValue_String(TEXT("UserName"),szUserName,CountArray(szUserName));
			m_AccountsDBAide.GetValue_String(TEXT("PrizeName"),szPrizeName,CountArray(szPrizeName));

			if (strlen(szUserName) == 0||strlen(szPrizeName) == 0)
			{
				break;
			}
			sprintf(result.szText[iCnt], "恭喜玩家 %s 抽中%s", szUserName, szPrizeName);

			iCnt++;

			//移动记录
			pIDataBase->MoveToNext();
		}

		//关闭记录
		pIDataBase->CloseRecordset();

		result.wCount = iCnt;

		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_GP_USER_ACTIVITY_RESULT,dwContextID,
			&result,sizeof(result));
	}
	else
	{
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_GP_USER_ACTIVITY_RESULT,dwContextID,
			&result,sizeof(result));
	}

	return true;
}
//抽奖
bool CDataBaseSink::OnRequestActivityLucky(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//数据验证
	ASSERT(wDataSize==sizeof(DBR_GP_UserActivityLucky));
	if (wDataSize!=sizeof(DBR_GP_UserActivityLucky)) return false;

	//数据转换
	DBR_GP_UserActivityLucky *pReq = (DBR_GP_UserActivityLucky*)pData;

	//执行存储过程
	LONG lReturnValue = -1;
	try
	{
		lReturnValue = SPUserActivityLuckyList(pReq->dwUserID, pReq->dwActivityID);
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);
	}

	DBR_GP_UserActivityLuckyResult result;
	ZeroMemory(&result, sizeof(DBR_GP_UserActivityLuckyResult));
	result.dwActivityID =  pReq->dwActivityID;
	result.dwUserID =  pReq->dwUserID;
	result.iCode = lReturnValue;
	//成功判断
	if ( lReturnValue == 0 )
	{
		//读取列表信息
		IDataBase * pIDataBase=(IDataBase *)m_AccountsDBAide.GetDataBase(IID_IDataBase,VER_IDataBase);

		//读取信息
		vector<unsigned int> vecPrizeID;
		vector<unsigned int> vecPossibility;
		while (pIDataBase->IsRecordsetEnd()==false)
		{

			DWORD stock = m_AccountsDBAide.GetValue_DWORD(TEXT("Stock"));
			DWORD maxCount = m_AccountsDBAide.GetValue_DWORD(TEXT("MaxCount"));
			DWORD todayCount = m_AccountsDBAide.GetValue_DWORD(TEXT("TodayCount"));

			if (stock > 0 && todayCount < maxCount)
			{
				vecPrizeID.push_back(m_AccountsDBAide.GetValue_DWORD(TEXT("PID")));
				vecPossibility.push_back(m_AccountsDBAide.GetValue_DWORD(TEXT("Possibility")));
			}

			//移动记录
			pIDataBase->MoveToNext();
		}

		//关闭记录
		pIDataBase->CloseRecordset();

		//失败
		if (vecPrizeID.size() <= 0)
		{
			//目前没有奖品可以抽
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_GP_USER_ACTIVITY_LUCKY_RESULT,dwContextID,
				&result,sizeof(result));
			return true;
		}

		int iTotal = 0;
		for (int i=0; i<vecPrizeID.size(); i++)
		{
			iTotal += vecPossibility[i];
		}
		int wIndex = rand()%iTotal;  //生成随机数
		int iPrizeID = -1;
		iTotal = 0;
		for (int i=0; i<vecPrizeID.size(); i++)
		{
			if (wIndex < iTotal + vecPossibility[i])
			{
				//找到了
				iPrizeID = i;
				break;
			}
			iTotal += vecPossibility[i];
		}

		//失败
		if (iPrizeID == -1)
		{
			//算法出现问题，没有抽中
			result.iCode = -3;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_GP_USER_ACTIVITY_LUCKY_RESULT,dwContextID,
				&result,sizeof(result));
			return true;
		}

		//去领奖
		//执行存储过程
		lReturnValue = -1;
		try
		{
			lReturnValue = SPUserActivityLucky(pReq->dwUserID, pReq->dwActivityID, vecPrizeID[iPrizeID]);
		}
		catch (IDataBaseException * pIException)
		{
			//错误信息
			LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
			CTraceService::TraceString(pszDescribe,TraceLevel_Exception);
		}

		result.iCode = lReturnValue;
		//成功判断
		if ( lReturnValue == 0 )
		{
			//读取信息
			result.dwPrizeID = m_AccountsDBAide.GetValue_DWORD(TEXT("PID"));
			result.dwPrice = m_AccountsDBAide.GetValue_DWORD(TEXT("Price"));
		
			m_AccountsDBAide.GetValue_String(TEXT("PrizeUrl"), result.szImageUrl, CountArray(result.szImageUrl));
			m_AccountsDBAide.GetValue_String(TEXT("PrizeName"), result.szPrizeName, CountArray(result.szPrizeName));

			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_GP_USER_ACTIVITY_LUCKY_RESULT,dwContextID,
				&result,sizeof(result));
		}
		else
		{
			//失败
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_GP_USER_ACTIVITY_LUCKY_RESULT,dwContextID,
				&result,sizeof(result));
			return true;
		}
	}
	else
	{
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_GP_USER_ACTIVITY_LUCKY_RESULT,dwContextID,
			&result,sizeof(result));
	}

	return true;
}
//抽奖记录
bool CDataBaseSink::OnRequestUserActivityRecord(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//数据验证
	ASSERT(wDataSize==sizeof(DBR_GP_UserActivityRecord));
	if (wDataSize!=sizeof(DBR_GP_UserActivityRecord)) return false;

	//数据转换
	DBR_GP_UserActivityRecord *pActivityRecord = (DBR_GP_UserActivityRecord*)pData;

	//执行存储过程
	LONG lReturnValue = -1;
	try
	{
		if (pActivityRecord->nPage < 1)pActivityRecord->nPage = 1;
		lReturnValue = SPUserActivityRecord(pActivityRecord->dwUserID, pActivityRecord->dwActivityID, pActivityRecord->nPage - 1);
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);
	}
	
	DBR_GP_UserActivityRecordResult ActivityRecordResult;
	ZeroMemory(&ActivityRecordResult, sizeof(ActivityRecordResult));
	ActivityRecordResult.dwActivityID = pActivityRecord->dwActivityID;
	ActivityRecordResult.dwUserID = pActivityRecord->dwUserID;

	//成功判断
	if ( lReturnValue == 0 )
	{
		ActivityRecordResult.bOperateSuccess = true;

		//获取数据
		IDataBase * pIDataBase=(IDataBase *)m_AccountsDBAide.GetDataBase(IID_IDataBase,VER_IDataBase);

		//读取信息
		while (pIDataBase->IsRecordsetEnd()==false)
		{
			DBR_GP_ActivityRecord record;
			ZeroMemory(&record, sizeof(record));

			//读取信息
			record.dwTotal=m_AccountsDBAide.GetValue_DWORD(TEXT("T"));
			record.dwID=m_AccountsDBAide.GetValue_DWORD(TEXT("ID"));
			record.dwAwardID=m_AccountsDBAide.GetValue_DWORD(TEXT("PrizeID"));
	//		record.dwDate=m_AccountsDBAide.GetValue_DWORD(TEXT("ExchangeDate"));
			m_AccountsDBAide.GetValue_String(TEXT("D"),record.szDate,CountArray(record.szDate));
			m_AccountsDBAide.GetValue_String(TEXT("PrizeName"),record.szAwardName,CountArray(record.szAwardName));
			m_AccountsDBAide.GetValue_String(TEXT("Status"),record.szStatus,CountArray(record.szStatus));

			//移动记录
			pIDataBase->MoveToNext();

			//插入信息
			ActivityRecordResult.vecRecords.push_back(record);
		}

		//关闭记录
		pIDataBase->CloseRecordset();

		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_GP_USER_ACTIVITY_RECORD_RESULT,dwContextID,
			&ActivityRecordResult,sizeof(ActivityRecordResult));
	}
	else
	{
		ActivityRecordResult.bOperateSuccess = false;

		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_GP_USER_ACTIVITY_RECORD_RESULT,dwContextID,
			&ActivityRecordResult,sizeof(ActivityRecordResult));
	}

	return true;
}

//发送喇叭
bool CDataBaseSink::OnRequestUserSpeakerSend(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//数据验证
	ASSERT(wDataSize==sizeof(DBR_GP_UserSpeakerSend));
	if (wDataSize!=sizeof(DBR_GP_UserSpeakerSend)) return false;

	//数据转换
	DBR_GP_UserSpeakerSend *pReq = (DBR_GP_UserSpeakerSend*)pData;

	//执行存储过程
	LONG lReturnValue = -1;
	try
	{
		lReturnValue = SPUserSpeakerSend(pReq->dwUserID, pReq->iType, pReq->szDate);
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);
	}
	
	DBR_GP_UserSpeakerSendResult dbResult;
	ZeroMemory(&dbResult, sizeof(dbResult));
	dbResult.iType = pReq->iType;
	dbResult.dwUserID = pReq->dwUserID;
	dbResult.nLen = pReq->nLen;
	memcpy(dbResult.szDate, pReq->szDate, pReq->nLen);
	
	dbResult.iCode = lReturnValue;

	//成功判断
	if ( lReturnValue == 0 )
	{
		//读取信息
		dbResult.dwScore=m_AccountsDBAide.GetValue_DWORD(TEXT("Score"));
	}
	
	m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_GP_USER_SPEAKER_SEND_RESULT,dwContextID,
			&dbResult,sizeof(dbResult));

	return true;
}

//查询配置项
bool CDataBaseSink::OnRequestUserQueryConfig(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//数据验证
	ASSERT(wDataSize==sizeof(DBR_GP_UserQueryConfig));
	if (wDataSize!=sizeof(DBR_GP_UserQueryConfig)) return false;

	//数据转换
	DBR_GP_UserQueryConfig *pReq = (DBR_GP_UserQueryConfig*)pData;

	//执行存储过程
	LONG lReturnValue = -1;
	try
	{
		lReturnValue = SPUserQueryConfig(pReq->dwUserID, pReq->dwVersion, pReq->dwConfigID, pReq->szChannel);
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);
	}
	
	DBR_GP_UserQueryConfigResult dbResult;
	ZeroMemory(&dbResult, sizeof(dbResult));
	dbResult.dwUserID = pReq->dwUserID;
	dbResult.dwVersion = pReq->dwVersion;
	
	dbResult.iCode = lReturnValue;

	//成功判断
	if ( lReturnValue == 0 )
	{
		//获取数据
		IDataBase * pIDataBase=(IDataBase *)m_AccountsDBAide.GetDataBase(IID_IDataBase,VER_IDataBase);

		//读取信息
		while (pIDataBase->IsRecordsetEnd()==false)
		{
			//取每一行的值
			CHAR key[64];
			CHAR value[300];
			memset(key, 0, sizeof(key));
			memset(value, 0, sizeof(value));
			memset(dbResult.szDate, 0, sizeof(dbResult.szDate));
			dbResult.dwLen = 0;
			
			m_AccountsDBAide.GetValue_String(TEXT("K"), key,CountArray(key));
			m_AccountsDBAide.GetValue_String(TEXT("V"), value,CountArray(value));

			sprintf(dbResult.szDate, "%s|%s", key, value);

			size_t iLenTmp = strlen(dbResult.szDate);
			dbResult.dwLen = iLenTmp>sizeof(dbResult.szDate)?sizeof(dbResult.szDate):iLenTmp;

			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_GP_USER_QUERY_CONFIG_RESULT,dwContextID,
							&dbResult,sizeof(dbResult));

			//移动记录
			pIDataBase->MoveToNext();
		}

		//关闭记录
		pIDataBase->CloseRecordset();

		//结束消息
		memset(dbResult.szDate, 0, sizeof(dbResult.szDate));
		dbResult.dwLen = 0;
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_GP_USER_QUERY_CONFIG_RESULT,dwContextID,
							&dbResult,sizeof(dbResult));
	}
	else
	{
		//结束消息
		memset(dbResult.szDate, 0, sizeof(dbResult.szDate));
		dbResult.dwLen = 0;
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_GP_USER_QUERY_CONFIG_RESULT,dwContextID,
							&dbResult,sizeof(dbResult));
	}

	return true;
}

//帐号存储过程
LONG CDataBaseSink::SPLogonByAccounts(LPCTSTR pszAccounts, LPCTSTR pszPassword, DWORD dwClientIP, LPCTSTR pszComputerID)
{
	//效验参数
	ASSERT(pszAccounts!=NULL);
	ASSERT(pszPassword!=NULL);

	//转化地址
	TCHAR szClientIP[16]=TEXT("");
	BYTE * pClientIP=(BYTE *)&dwClientIP;
	_snprintf(szClientIP,sizeof(szClientIP),TEXT("%d.%d.%d.%d"),pClientIP[0],pClientIP[1],pClientIP[2],pClientIP[3]);

	//执行存储过程
	m_AccountsDBAide.ResetParameter();
	m_AccountsDBAide.AddParameter(TEXT("@strAccounts"),pszAccounts);
	m_AccountsDBAide.AddParameter(TEXT("@strPassword"),pszPassword);
	m_AccountsDBAide.AddParameter(TEXT("@strClientIP"),szClientIP);
	m_AccountsDBAide.AddParameter(TEXT("@strMachineSerial"),pszComputerID);
	if (m_pInitParamter)
	{
		m_AccountsDBAide.AddParameter(TEXT("@strLoginServer"),m_pInitParamter->m_szMainPage);
	}

	return m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_EfficacyAccounts_v2"),true);
}

//更新存储过程
LONG CDataBaseSink::SPRegisterAccounts(LPCTSTR pszAccounts, LPCTSTR pszPassword, LPCTSTR pszSpreader, WORD wFaceID, BYTE cbGender, DWORD dwClientIP, LPCTSTR pszComputerID)
{
	//效验参数
	ASSERT(pszAccounts!=NULL);
	ASSERT(pszPassword!=NULL);

	//转化地址
	TCHAR szClientIP[16]=TEXT("");
	BYTE * pClientIP=(BYTE *)&dwClientIP;
	_snprintf(szClientIP,sizeof(szClientIP),TEXT("%d.%d.%d.%d"),pClientIP[0],pClientIP[1],pClientIP[2],pClientIP[3]);

	//执行存储过程
	m_AccountsDBAide.ResetParameter();
	m_AccountsDBAide.AddParameter(TEXT("@strAccounts"),pszAccounts);
	m_AccountsDBAide.AddParameter(TEXT("@strPassword"),pszPassword);
	m_AccountsDBAide.AddParameter(TEXT("@strSpreader"),pszSpreader);
	m_AccountsDBAide.AddParameter(TEXT("@nFaceID"),wFaceID);
	m_AccountsDBAide.AddParameter(TEXT("@cbGender"),cbGender);
	m_AccountsDBAide.AddParameter(TEXT("@strClientIP"),szClientIP);
	m_AccountsDBAide.AddParameter(TEXT("@strMachineSerial"),pszComputerID);

	return m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_RegisterAccounts"),true);
}

//注册存储过程WX
LONG CDataBaseSink::SPRegisterAccountsWX(LPCTSTR pszAccounts, LPCTSTR pszPassword, LPCTSTR pszSpreader, 
	LPCTSTR wFaceURL, BYTE cbGender, DWORD dwClientIP, LPCTSTR pszWXComputerID,LPCTSTR pszComputerID,
	LPCTSTR pszNickname, LPCTSTR pszOpenid, LPCTSTR pszUnionid)
{
	//转化地址
	TCHAR szClientIP[16]=TEXT("");
	BYTE * pClientIP=(BYTE *)&dwClientIP;
	_snprintf(szClientIP,sizeof(szClientIP),TEXT("%d.%d.%d.%d"),pClientIP[0],pClientIP[1],pClientIP[2],pClientIP[3]);

	//执行存储过程
	m_AccountsDBAide.ResetParameter();
	m_AccountsDBAide.AddParameter(TEXT("@strAccounts"),pszAccounts);
	m_AccountsDBAide.AddParameter(TEXT("@strPassword"),pszPassword);
	m_AccountsDBAide.AddParameter(TEXT("@strSpreader"),pszSpreader);
	m_AccountsDBAide.AddParameter(TEXT("@strNickname"),pszNickname);
	m_AccountsDBAide.AddParameter(TEXT("@strOpenid"),pszOpenid);
	m_AccountsDBAide.AddParameter(TEXT("@strUnionid"),pszUnionid);
	m_AccountsDBAide.AddParameter(TEXT("@strFaceUrl"),wFaceURL);
	m_AccountsDBAide.AddParameter(TEXT("@cbGender"),cbGender);
	m_AccountsDBAide.AddParameter(TEXT("@strClientIP"),szClientIP);
	m_AccountsDBAide.AddParameter(TEXT("@strWXMachineSerial"),pszWXComputerID);
	m_AccountsDBAide.AddParameter(TEXT("@strMachineSerial"),pszComputerID);

	return m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_RegisterAccountsWX_v2"),true);
}

//升级成WX账号存储过程
LONG CDataBaseSink::SPWXUpdate(DWORD dwUserID,LPCTSTR wFaceURL, BYTE cbGender, DWORD dwClientIP, LPCTSTR pszWXComputerID,LPCTSTR pszComputerID,LPCTSTR pszNickname, LPCTSTR pszOpenid, LPCTSTR pszUnionid)
{
	//转化地址
	TCHAR szClientIP[16]=TEXT("");
	BYTE * pClientIP=(BYTE *)&dwClientIP;
	_snprintf(szClientIP,sizeof(szClientIP),TEXT("%d.%d.%d.%d"),pClientIP[0],pClientIP[1],pClientIP[2],pClientIP[3]);

	//执行存储过程
	m_AccountsDBAide.ResetParameter();
	m_AccountsDBAide.AddParameter(TEXT("@iUserId"),dwUserID);
	m_AccountsDBAide.AddParameter(TEXT("@strNickname"),pszNickname);
	m_AccountsDBAide.AddParameter(TEXT("@strOpenid"),pszOpenid);
	m_AccountsDBAide.AddParameter(TEXT("@strUnionid"),pszUnionid);
	m_AccountsDBAide.AddParameter(TEXT("@strFaceUrl"),wFaceURL);
	m_AccountsDBAide.AddParameter(TEXT("@cbGender"),cbGender);
	m_AccountsDBAide.AddParameter(TEXT("@strClientIP"),szClientIP);
	m_AccountsDBAide.AddParameter(TEXT("@strWXMachineSerial"),pszWXComputerID);
	m_AccountsDBAide.AddParameter(TEXT("@strMachineSerial"),pszComputerID);

	return m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_WXUpdate_v2"),true);
}

//I D 存储过程
LONG CDataBaseSink::SPLogonByUserID(DWORD dwGameID, LPCTSTR pszPassword, DWORD dwClientIP, LPCTSTR pszComputerID)
{
	//效验参数
	ASSERT(dwGameID!=0L);
	ASSERT(pszPassword!=NULL);

	//转化地址
	TCHAR szClientIP[16]=TEXT("");
	BYTE * pClientIP=(BYTE *)&dwClientIP;
	_snprintf(szClientIP,sizeof(szClientIP),TEXT("%d.%d.%d.%d"),pClientIP[0],pClientIP[1],pClientIP[2],pClientIP[3]);

	//执行存储过程
	m_AccountsDBAide.ResetParameter();
	m_AccountsDBAide.AddParameter(TEXT("@dwGameID"),dwGameID);
	m_AccountsDBAide.AddParameter(TEXT("@strPassword"),pszPassword);
	m_AccountsDBAide.AddParameter(TEXT("@strClientIP"),szClientIP);
	m_AccountsDBAide.AddParameter(TEXT("@strMachineSerial"),pszComputerID);

	return m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_EfficacyGameID"),true);
}

//签到存储过程
LONG CDataBaseSink::SPUserSignIn(DWORD dwUserID)
{
	//效验参数
	ASSERT(dwUserID!=0L);

	//执行存储过程
	m_AccountsDBAide.ResetParameter();
	m_AccountsDBAide.AddParameter(TEXT("@dwUserID"),dwUserID);

	return m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_UserSignIn"),true);
}

//领奖存储过程
LONG CDataBaseSink::SPUserGetAward(DWORD dwUserID, DWORD dwTaskID)
{
	//效验参数
	ASSERT(dwUserID!=0L);

	//执行存储过程
	m_AccountsDBAide.ResetParameter();
	m_AccountsDBAide.AddParameter(TEXT("@dwUserID"),dwUserID);
	m_AccountsDBAide.AddParameter(TEXT("@dwTaskID"),dwTaskID);

	return m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_UserGetAward"),true);
}

//兑换的个人信息存储过程
LONG CDataBaseSink::SPUserExchangeInfo(DWORD dwUserID, LPCTSTR pszPhone)
{
	//效验参数
	ASSERT(dwUserID!=0L);

	//执行存储过程
	m_AccountsDBAide.ResetParameter();
	m_AccountsDBAide.AddParameter(TEXT("@dwUserID"),dwUserID);
	m_AccountsDBAide.AddParameter(TEXT("@strPhone"),pszPhone);

	return m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_UserExchangeInfo"),true);
}

//兑换商品
LONG CDataBaseSink::SPUserExchangeProduct(DWORD dwUserID)
{
	//效验参数
	ASSERT(dwUserID!=0L);

	//执行存储过程
	m_AccountsDBAide.ResetParameter();
	m_AccountsDBAide.AddParameter(TEXT("@dwUserID"),dwUserID);

	return m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_UserExchangeProduct"),true);
}

//兑换
LONG CDataBaseSink::SPUserExchange(DWORD dwUserID,DWORD dwAwardID)
{
	//效验参数
	ASSERT(dwUserID!=0L);

	//执行存储过程
	m_AccountsDBAide.ResetParameter();
	m_AccountsDBAide.AddParameter(TEXT("@dwUserID"),dwUserID);
	m_AccountsDBAide.AddParameter(TEXT("@dwAwardID"),dwAwardID);

	return m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_UserExchange"),true);
}

//兑换记录
LONG CDataBaseSink::SPUserExchangeRecord(DWORD dwUserID, int nPage)
{
	//效验参数
	ASSERT(dwUserID!=0L);

	//执行存储过程
	m_AccountsDBAide.ResetParameter();
	m_AccountsDBAide.AddParameter(TEXT("@dwUserID"),dwUserID);
	m_AccountsDBAide.AddParameter(TEXT("@nPage"),nPage);

	return m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_UserExchangeRecord"),true);
}

//兑换
LONG CDataBaseSink::SPUserExchange_V2(DWORD dwUserID,DWORD dwAwardID)
{
	//效验参数
	ASSERT(dwUserID!=0L);

	//执行存储过程
	m_AccountsDBAide.ResetParameter();
	m_AccountsDBAide.AddParameter(TEXT("@dwUserID"),dwUserID);
	m_AccountsDBAide.AddParameter(TEXT("@dwAwardID"),dwAwardID);

	return m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_UserExchange_V2"),true);
}

//上传存储过程
LONG CDataBaseSink::SPUploadFace(DWORD dwUserID, BYTE *bFaceData, DWORD dwFaceDataSize)
{
	//效验参数
	ASSERT(dwUserID!=0L);
	ASSERT(dwFaceDataSize!=0L);
	ASSERT(bFaceData!=NULL);

	//效验参数
	ASSERT(dwUserID!=0L);
	ASSERT(dwFaceDataSize!=0L);
	ASSERT(bFaceData!=NULL);

	//执行存储过程
	m_AccountsDBAide.ResetParameter();
	m_AccountsDBAide.AddParameter(TEXT("@dwUserID"),dwUserID);

	VARIANT varChunk;
	SAFEARRAY *psa=NULL;
	SAFEARRAYBOUND rgsabound[1];
	rgsabound[0].cElements = dwFaceDataSize;
    rgsabound[0].lLbound = 0;
    psa = SafeArrayCreate(VT_UI1,1,rgsabound);
    for (long index =0; index < long(dwFaceDataSize); index++ )
	{
		SafeArrayPutElement(psa,&index,&bFaceData[index]);
	}
    varChunk.vt = VT_ARRAY|VT_UI1;
    varChunk.parray = psa;

	IDataBase * pIDataBase=m_AccountsDBModule.GetInterface();
	pIDataBase->AddParameter(TEXT("@imgCustomFaceImage"),adLongVarBinary,adParamInput,dwFaceDataSize,_variant_t(varChunk));

	LONG lReturnValue=m_AccountsDBAide.ExecuteProcess(TEXT("GSP_CF_InsertCustomFace"),false);

	//释放资源
	SafeArrayDestroy(psa);

	return lReturnValue;
}

//加载存储过程
LONG CDataBaseSink::SPDownloadFace(DWORD dwUserID)
{
	//效验参数
	ASSERT(dwUserID!=0L);

	//执行存储过程
	m_AccountsDBAide.ResetParameter();
	m_AccountsDBAide.AddParameter(TEXT("@dwUserID"),dwUserID);


	return m_AccountsDBAide.ExecuteProcess(TEXT("GSP_CF_LoadCustomFace"),true);
}

//删除存储过程
LONG CDataBaseSink::SPDeleteCustomFace(DWORD dwUserID)
{
	//效验参数
	ASSERT(dwUserID!=0L);

	//执行存储过程
	m_AccountsDBAide.ResetParameter();
	m_AccountsDBAide.AddParameter(TEXT("@dwUserID"),dwUserID);

	return m_AccountsDBAide.ExecuteProcess(TEXT("GSP_CF_DeleteCustomFace"),true);
}

//修改存储过程
LONG CDataBaseSink::SPModifyIndividual(TCHAR szAccount[NAME_LEN],TCHAR szNickname[NAME_LEN], TCHAR szPhone[NAME_LEN],int nGender)
{
	//执行存储过程
	m_AccountsDBAide.ResetParameter();
	m_AccountsDBAide.AddParameter(TEXT("@strAccounts"),szAccount);
	m_AccountsDBAide.AddParameter(TEXT("@strNickName"),szNickname);
	m_AccountsDBAide.AddParameter(TEXT("@strPhone"),szPhone);
	m_AccountsDBAide.AddParameter(TEXT("@nGender"),nGender);

	return m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_ModifyIndividual"),true);
}

//修改个性签名存储过程
LONG CDataBaseSink::SPModifyUnderWrite(unsigned int dwUserID,TCHAR szDescribe[UNDER_WRITE_LEN])
{
	//执行存储过程
	m_AccountsDBAide.ResetParameter();
	m_AccountsDBAide.AddParameter(TEXT("@UserId"),dwUserID);
	m_AccountsDBAide.AddParameter(TEXT("@strUnderWrite"),szDescribe);

	return m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_ModifyUnderWrite"),true);
}

//修改手机号存储过程
LONG CDataBaseSink::SPModifyPhone(unsigned int dwUserID,TCHAR szPhone[31])
{
	//执行存储过程
	m_AccountsDBAide.ResetParameter();
	m_AccountsDBAide.AddParameter(TEXT("@UserId"),dwUserID);
	m_AccountsDBAide.AddParameter(TEXT("@strPhone"),szPhone);

	return m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_ModifyPhone"),true);
}

//校验名称存储过程
LONG CDataBaseSink::SPCheckID(TCHAR szAccount[NAME_LEN])
{
	//执行存储过程
	m_AccountsDBAide.ResetParameter();
	m_AccountsDBAide.AddParameter(TEXT("@strAccount"),szAccount);

	return m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_CheckID"),true);
}

//校验昵称存储过程
LONG CDataBaseSink::SPCheckNickName(TCHAR szNickName[NAME_LEN])
{
	//执行存储过程
	m_AccountsDBAide.ResetParameter();
	m_AccountsDBAide.AddParameter(TEXT("@strNickName"),szNickName);

	return m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_CheckNickName"),true);
}

//注册存储过程
LONG CDataBaseSink::SPRegister(TCHAR szAccount[NAME_LEN],TCHAR szNickname[NAME_LEN],TCHAR szPhone[NAME_LEN], int nGender, 
							   TCHAR szPassword[PASS_LEN], DWORD dwClientIP, LPCTSTR pszComputerID, LPCTSTR pszChannel)
{
	
	//转化地址
	TCHAR szClientIP[16]=TEXT("");
	BYTE * pClientIP=(BYTE *)&dwClientIP;
	_snprintf(szClientIP,sizeof(szClientIP),TEXT("%d.%d.%d.%d"),pClientIP[0],pClientIP[1],pClientIP[2],pClientIP[3]);

	//执行存储过程
	m_AccountsDBAide.ResetParameter();
	m_AccountsDBAide.AddParameter(TEXT("@szAccount"),szAccount);
	m_AccountsDBAide.AddParameter(TEXT("@strNickName"),szNickname);
	m_AccountsDBAide.AddParameter(TEXT("@strPhone"),szPhone);
	m_AccountsDBAide.AddParameter(TEXT("@szPassword"),szPassword);
	m_AccountsDBAide.AddParameter(TEXT("@cbGender"),nGender);
	m_AccountsDBAide.AddParameter(TEXT("@strClientIP"),szClientIP);
	m_AccountsDBAide.AddParameter(TEXT("@strMachineSerial"),pszComputerID);
	m_AccountsDBAide.AddParameter(TEXT("@strChannel"),pszChannel);


	return m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_Register"),true);
}


//帐号升级存储过程
LONG CDataBaseSink::SPIDUpdate(TCHAR szAccount[NAME_LEN],TCHAR szAccountNew[NAME_LEN],TCHAR szNickname[NAME_LEN], int nGender, TCHAR szPassword[PASS_LEN])
{
	//执行存储过程
	m_AccountsDBAide.ResetParameter();
	m_AccountsDBAide.AddParameter(TEXT("@strAccounts"),szAccount);
	m_AccountsDBAide.AddParameter(TEXT("@strAccountsNew"),szAccountNew);
	m_AccountsDBAide.AddParameter(TEXT("@strNickName"),szNickname);
	m_AccountsDBAide.AddParameter(TEXT("@strPassword"),szPassword);
	m_AccountsDBAide.AddParameter(TEXT("@cbGender"),nGender);

	return m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_IDUpdate"),true);
}

//重设密码存储过程
LONG CDataBaseSink::SPResetPwd(TCHAR szAccount[NAME_LEN], TCHAR szPassword[PASS_LEN])
{
	//执行存储过程
	m_AccountsDBAide.ResetParameter();
	m_AccountsDBAide.AddParameter(TEXT("@szAccount"),szAccount);
	m_AccountsDBAide.AddParameter(TEXT("@szPassword"),szPassword);

	return m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_ResetPwd"),true);
}

//查询信息存储过程
LONG CDataBaseSink::SPUserInfo(TCHAR szAccount[NAME_LEN])
{
	//执行存储过程
	m_AccountsDBAide.ResetParameter();
	m_AccountsDBAide.AddParameter(TEXT("@szAccount"),szAccount);

	return m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_UserInfo"),true);
}

//查询用户是否可以领取推广奖励
LONG CDataBaseSink::SPUserCanSpread(DWORD dwUser, LPCTSTR pszComputerID)
{
	//执行存储过程
	m_AccountsDBAide.ResetParameter();
	m_AccountsDBAide.AddParameter(TEXT("@dwUserID"),dwUser);
	m_AccountsDBAide.AddParameter(TEXT("@strMachineSerial"),pszComputerID);

	return m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_UserCanGetSpreadAward"),true);
}

//查询用户是否可以领取推广奖励
LONG CDataBaseSink::SPUserGetSpread(DWORD dwUser, DWORD dwSpreader, LPCTSTR pszComputerID)
{
	//执行存储过程
	m_AccountsDBAide.ResetParameter();
	m_AccountsDBAide.AddParameter(TEXT("@dwUserID"),dwUser);
	m_AccountsDBAide.AddParameter(TEXT("@dwSpreaderID"),dwSpreader);
	m_AccountsDBAide.AddParameter(TEXT("@strMachineSerial"),pszComputerID);

	return m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_UserGetSpreadAward"),true);
}

//查询推广员信息
LONG CDataBaseSink::SPUserGetSpreadInfo(DWORD dwSpreader)
{
	//执行存储过程
	m_AccountsDBAide.ResetParameter();
	m_AccountsDBAide.AddParameter(TEXT("@dwSpreaderID"),dwSpreader);

	return m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_UserGetSpreadInfo"),true);
}

//弹出公告信息
LONG CDataBaseSink::SPUserGetBbsPop(LPCTSTR pszChannel)
{
	//执行存储过程
	m_AccountsDBAide.ResetParameter();
	m_AccountsDBAide.AddParameter(TEXT("@szChannelName"),pszChannel);

	return m_AccountsDBAide.ExecuteProcess(TEXT("WHServerInfoDB.dbo.GSP_GR_GetBbsPop_V2"),true);
}

//滚动公告信息
LONG CDataBaseSink::SPUserGetBbsScroll(LPCTSTR pszChannel)
{
	//执行存储过程
	m_AccountsDBAide.ResetParameter();
	m_AccountsDBAide.AddParameter(TEXT("@szChannelName"),pszChannel);

	return m_AccountsDBAide.ExecuteProcess(TEXT("WHServerInfoDB.dbo.GSP_GR_GetBbsScroll_V2"),true);
}

//残表公告信息
LONG CDataBaseSink::SPUserGetBbsList(LPCTSTR pszChannel)
{
	//执行存储过程
	m_AccountsDBAide.ResetParameter();
	m_AccountsDBAide.AddParameter(TEXT("@szChannelName"),pszChannel);

	return m_AccountsDBAide.ExecuteProcess(TEXT("WHServerInfoDB.dbo.GSP_GR_GetBbsList_V2"),true);
}

//玩家任务信息
LONG CDataBaseSink::SPUserGetTaskInfo(DWORD dwUser)
{
	//执行存储过程
	m_AccountsDBAide.ResetParameter();
	m_AccountsDBAide.AddParameter(TEXT("@dwUserID"),dwUser);
	if (m_pInitParamter)
	{
		m_AccountsDBAide.AddParameter(TEXT("@strLoginServer"),m_pInitParamter->m_szMainPage);
	}

	return m_AccountsDBAide.ExecuteProcess(TEXT("WHTreasureDB.dbo.GSP_GR_GetTaskStat_v2"),true);
}

//排行榜记录
LONG CDataBaseSink::SPUserRanking(DWORD dwUserID, int nPage)
{
	//效验参数
	ASSERT(dwUserID!=0L);

	//执行存储过程
	m_AccountsDBAide.ResetParameter();
	m_AccountsDBAide.AddParameter(TEXT("@dwUserID"),dwUserID);
	m_AccountsDBAide.AddParameter(TEXT("@nPage"),nPage);

	return m_AccountsDBAide.ExecuteProcess(TEXT("WHTreasureDB.dbo.GSP_GR_GetRanking"),true);
}

//排行榜记录
LONG CDataBaseSink::SPUserRanking_V2(DWORD dwUserID, int nPage)
{
	//效验参数
	ASSERT(dwUserID!=0L);

	//执行存储过程
	m_AccountsDBAide.ResetParameter();
	m_AccountsDBAide.AddParameter(TEXT("@dwUserID"),dwUserID);
	m_AccountsDBAide.AddParameter(TEXT("@nPage"),nPage);

	return m_AccountsDBAide.ExecuteProcess(TEXT("WHTreasureDB.dbo.GSP_GR_GetRanking_V2"),true);
}

//玩家排行榜是否入榜
LONG CDataBaseSink::SPUserRankingStatus(DWORD dwUserID)
{
	//效验参数
	ASSERT(dwUserID!=0L);

	//执行存储过程
	m_AccountsDBAide.ResetParameter();
	m_AccountsDBAide.AddParameter(TEXT("@dwUserID"),dwUserID);

	return m_AccountsDBAide.ExecuteProcess(TEXT("WHTreasureDB.dbo.GSP_GR_GetUserRankingStatus"),true);
}

//修改是否入排行榜
LONG CDataBaseSink::SPUserModifyRankingStatus(DWORD dwUserID, int nRankingStatus)
{
	//效验参数
	ASSERT(dwUserID!=0L);

	//执行存储过程
	m_AccountsDBAide.ResetParameter();
	m_AccountsDBAide.AddParameter(TEXT("@dwUserID"),dwUserID);
	m_AccountsDBAide.AddParameter(TEXT("@RankingStatus"),nRankingStatus);

	return m_AccountsDBAide.ExecuteProcess(TEXT("WHTreasureDB.dbo.GSP_GR_ModifyRankingStatus"),true);
}

//反馈问题
LONG CDataBaseSink::SPUserFeedBack(DWORD dwUser,  LPCTSTR pszContent, LPCTSTR pszPicture)
{
	//执行存储过程
	m_AccountsDBAide.ResetParameter();
	m_AccountsDBAide.AddParameter(TEXT("@dwUserID"),dwUser);
	m_AccountsDBAide.AddParameter(TEXT("@Question"),pszContent);
	m_AccountsDBAide.AddParameter(TEXT("@AttachPath"),pszPicture);

	return m_AccountsDBAide.ExecuteProcess(TEXT("WHServerInfoDB.dbo.GSP_GR_CommitKeFu"),true);
}

//查询玩家反馈意见
LONG CDataBaseSink::SPUserGetFeedBack(DWORD dwUserID, int nPage)
{
	//效验参数
	ASSERT(dwUserID!=0L);

	//执行存储过程
	m_AccountsDBAide.ResetParameter();
	m_AccountsDBAide.AddParameter(TEXT("@dwUserID"),dwUserID);
	m_AccountsDBAide.AddParameter(TEXT("@nPage"),nPage);

	return m_AccountsDBAide.ExecuteProcess(TEXT("WHServerInfoDB.dbo.GSP_GR_GetKeFu"),true);
}

//查询用户身份证账号信息
LONG CDataBaseSink::SPUserInfoIDCard(DWORD dwUserID)
{
	//效验参数
	ASSERT(dwUserID!=0L);

	//执行存储过程
	m_AccountsDBAide.ResetParameter();
	m_AccountsDBAide.AddParameter(TEXT("@dwUserID"),dwUserID);

	return m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_GetIDCard"),true);
}

//请求提交身份证信息
LONG CDataBaseSink::SPUserAuthentication(DWORD dwUserID, LPCTSTR pszName, LPCTSTR pszIDCard)
{
	//效验参数
	ASSERT(dwUserID!=0L);

	//执行存储过程
	m_AccountsDBAide.ResetParameter();
	m_AccountsDBAide.AddParameter(TEXT("@dwUserID"),dwUserID);
	m_AccountsDBAide.AddParameter(TEXT("@IDName"),pszName);
	m_AccountsDBAide.AddParameter(TEXT("@IDCard"),pszIDCard);

	return m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_CommitIDCard"),true);
}

//商城商品信息
LONG CDataBaseSink::SPUserMallProduct(DWORD dwUserID)
{
	//效验参数
	ASSERT(dwUserID!=0L);

	//执行存储过程
	m_AccountsDBAide.ResetParameter();
	m_AccountsDBAide.AddParameter(TEXT("@dwUserID"),dwUserID);

	return m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_UserMallProduct"),true);
}

//商城商品信息(ios)
LONG CDataBaseSink::SPUserMallProductIOS(DWORD dwUserID)
{
	//效验参数
	ASSERT(dwUserID!=0L);

	//执行存储过程
	m_AccountsDBAide.ResetParameter();
	m_AccountsDBAide.AddParameter(TEXT("@dwUserID"),dwUserID);

	return m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_UserMallProduct_IOS"),true);
}

//下订单
LONG CDataBaseSink::SPUserMallPlaceOrder(DWORD dwUserID, DWORD dwProductID,TCHAR szPayWay[10])
{
	//效验参数
	ASSERT(dwUserID!=0L);
	//执行存储过程
	m_AccountsDBAide.ResetParameter();
	m_AccountsDBAide.AddParameter(TEXT("@dwUserID"),dwUserID);
	m_AccountsDBAide.AddParameter(TEXT("@dwProductID"),dwProductID);
	m_AccountsDBAide.AddParameter(TEXT("@RechargeWay"),szPayWay);
	
	return m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_UserMallPlaceOrder"),true);
}

//查询用户银行信息
LONG CDataBaseSink::SPUserBankInfo(DWORD dwUserID)
{
	//效验参数
	ASSERT(dwUserID!=0L);
	//执行存储过程
	m_AccountsDBAide.ResetParameter();
	m_AccountsDBAide.AddParameter(TEXT("@dwUserID"),dwUserID);
	
	return m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_UserBankInfo"),true);
}

//用户操作银行
LONG CDataBaseSink::SPUserBankCharge(int nOpCode, DWORD dwUserID, DWORD dwCurScore, DWORD dwBankScore)
{
	//效验参数
	ASSERT(dwUserID!=0L);
	//执行存储过程
	m_AccountsDBAide.ResetParameter();
	m_AccountsDBAide.AddParameter(TEXT("@wOpCode"),nOpCode);
	m_AccountsDBAide.AddParameter(TEXT("@dwUserID"),dwUserID);
	m_AccountsDBAide.AddParameter(TEXT("@dwCurScore"),dwCurScore);
	m_AccountsDBAide.AddParameter(TEXT("@dwBankScore"),dwBankScore);
	
	return m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_UserBankCharge"),true);
}

LONG CDataBaseSink::SPUserCancelOrder(TCHAR szAccount[20])
{
	//执行存储过程
	m_AccountsDBAide.ResetParameter();
	m_AccountsDBAide.AddParameter(TEXT("@dwOrderID"),szAccount);
	return m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_UserCancelOrder"),true);
}
// 商城购买商品结果
LONG CDataBaseSink::SPUserMallBuy(TCHAR orderNum[20])
{
	//执行存储过程
	m_AccountsDBAide.ResetParameter();
	m_AccountsDBAide.AddParameter(TEXT("@dwOrderNum"), orderNum);
	return m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_UserMallBuyResult"),true);
}
// 商城更新购买结果
LONG CDataBaseSink::SPUserMallUpdateBuy(TCHAR orderNum[20])
{
	//执行存储过程
	m_AccountsDBAide.ResetParameter();
	m_AccountsDBAide.AddParameter(TEXT("@dwOrderNum"), orderNum);
	return m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_UserMallUpdateResult"),true);
}

// 查询活动列表
LONG CDataBaseSink::SPUserActivityList()
{
	//执行存储过程
	m_AccountsDBAide.ResetParameter();

	return m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_GetActivityList"),true);
}

//查询活动
LONG CDataBaseSink::SPUserActivity(DWORD dwUserID, DWORD dwActivityID)
{
	//执行存储过程
	m_AccountsDBAide.ResetParameter();

	m_AccountsDBAide.AddParameter(TEXT("@dwUserID"), dwUserID);
	m_AccountsDBAide.AddParameter(TEXT("@dwActivityID"), dwActivityID);

	return m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_GetActivity"),true);
}

//抽奖列表
LONG CDataBaseSink::SPUserActivityLuckyList(DWORD dwUserID, DWORD dwActivityID)
{
	//执行存储过程
	m_AccountsDBAide.ResetParameter();

	m_AccountsDBAide.AddParameter(TEXT("@dwUserID"), dwUserID);
	m_AccountsDBAide.AddParameter(TEXT("@dwActivityID"), dwActivityID);

	return m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_GetActivityLuckyList"),true);
}

//抽奖
LONG CDataBaseSink::SPUserActivityLucky(DWORD dwUserID, DWORD dwActivityID, DWORD dwPrizeID)
{
	//执行存储过程
	m_AccountsDBAide.ResetParameter();

	m_AccountsDBAide.AddParameter(TEXT("@dwUserID"), dwUserID);
	m_AccountsDBAide.AddParameter(TEXT("@dwActivityID"), dwActivityID);
	m_AccountsDBAide.AddParameter(TEXT("@dwPrizeID"), dwPrizeID);

	return m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_GetActivityLucky"),true);
}

//抽奖记录
LONG CDataBaseSink::SPUserActivityRecord(DWORD dwUserID, DWORD dwActivityID, int nPage)
{
	//效验参数
	ASSERT(dwUserID!=0L);

	//执行存储过程
	m_AccountsDBAide.ResetParameter();
	m_AccountsDBAide.AddParameter(TEXT("@dwUserID"),dwUserID);
	m_AccountsDBAide.AddParameter(TEXT("@dwActivityID"),dwActivityID);
	m_AccountsDBAide.AddParameter(TEXT("@nPage"),nPage);

	return m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_GetActivityRecord"),true);
}


//发喇叭
LONG CDataBaseSink::SPUserSpeakerSend(DWORD dwUserID, WORD wType, LPCTSTR pszContent)
{
	//执行存储过程
	m_AccountsDBAide.ResetParameter();
	m_AccountsDBAide.AddParameter(TEXT("@dwUserID"),dwUserID);
	m_AccountsDBAide.AddParameter(TEXT("@wType"),wType);
	m_AccountsDBAide.AddParameter(TEXT("@szMsg"),pszContent);

	return m_AccountsDBAide.ExecuteProcess(TEXT("WHServerInfoDB.dbo.GSP_GP_UserSpeakerSend"),true);
}
//查询配置项
LONG CDataBaseSink::SPUserQueryConfig(DWORD dwUserID, DWORD dwVersion, DWORD dwConfigID, LPCTSTR pszChannel)
{
	//执行存储过程
	m_AccountsDBAide.ResetParameter();
	m_AccountsDBAide.AddParameter(TEXT("@dwUserID"),dwUserID);
	m_AccountsDBAide.AddParameter(TEXT("@dwVersion"),dwVersion);
	m_AccountsDBAide.AddParameter(TEXT("@dwConfigID"),dwConfigID);
	m_AccountsDBAide.AddParameter(TEXT("@szChannel"),pszChannel);

	return m_AccountsDBAide.ExecuteProcess(TEXT("WHServerInfoDB.dbo.GSP_UserQueryConfig"),true);
}

//////////////////////////////////////////////////////////////////////////
