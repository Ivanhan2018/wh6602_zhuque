#include "StdAfx.h"
#include "AttemperEngineSink.h"
#include "Zip.h"
#include "LogonServer.h"


#include "biostream.h"
#include <fstream>

//声明变量
extern CLogonServerApp theApp;
//////////////////////////////////////////////////////////////////////////

//宏定义
#define TIME_RECONNECT						15000L						//重连时间
#define TIME_UPDATE_LIST					30000L						//更新时间
#define TIME_DOWNLOAD_CUSTOM_FACE			200L						//下载头像

//定时器 ID
#define IDI_CONNECT_CENTER_SERVER			1							//连接服务器
#define IDI_UPDATE_SERVER_LIST				2							//更新列表
#define IDI_DOWNLOAD_CUSTOM_FACE			3							//下载头像

//////////////////////////////////////////////////////////////////////////

#define SEND_USER_DATA(msg)		do{char cbBuffer[SOCKET_PACKET];\
	bostream bos;\
	bos.attach(cbBuffer, SOCKET_PACKET);\
	bos<<msg;\
	if(m_pITCPNetworkEngine)m_pITCPNetworkEngine->SendData(dwContextID, msg.M_ID, msg.S_ID, cbBuffer, bos.length());}while(0)

//输出订单信息
void TraceMallOrderMessage(LPCTSTR pszMessage)
{
	if(0 == lstrlen(pszMessage))
	{
		return ;
	}

	CFile File;
	if ((File.Open(TEXT("TraceData_MallOrderInfo.txt"),CFile::modeWrite)==FALSE)&&
		(File.Open(TEXT("TraceData_MallOrderInfo.txt"),CFile::modeWrite|CFile::modeCreate)==FALSE))
	{
		ASSERT(FALSE);
		return;
	}

	SYSTEMTIME sys; 
	GetLocalTime( &sys ); 
	char buf[128];
	memset(buf, 0, 128);
	sprintf(buf, "%4d/%02d/%02d %02d:%02d:%02d.%03d ",sys.wYear,sys.wMonth,sys.wDay,sys.wHour,sys.wMinute, sys.wSecond,sys.wMilliseconds);

	File.SeekToEnd();
	File.Write(buf,lstrlen(buf));
	File.Write(pszMessage,lstrlen(pszMessage));
	File.Write(TEXT("\r\n"),lstrlen(TEXT("\r\n")));

	File.Flush();
	File.Close();

	return;
}

//构造函数
CAttemperEngineSink::CAttemperEngineSink()
{
	//设置变量
	m_pInitParamter=NULL;
	m_pBindParameter=NULL;

	//设置变量
	m_pITimerEngine=NULL;
	m_pIDataBaseEngine=NULL;
	m_pITCPNetworkEngine=NULL;
	m_pITCPSocketCorrespond=NULL;

	m_mapRechargeUserSocketID.clear();
	m_mapRechargeSocketIDUser.clear();

	return;
}

//析构函数
CAttemperEngineSink::~CAttemperEngineSink()
{
}

//接口查询
void * __cdecl CAttemperEngineSink::QueryInterface(const IID & Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IAttemperEngineSink,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IAttemperEngineSink,Guid,dwQueryVer);
	return NULL;
}

//调度模块启动
bool __cdecl CAttemperEngineSink::OnAttemperEngineStart(IUnknownEx * pIUnknownEx)
{
	//设置组件
	m_ServerList.SetSocketEngine(m_pITCPNetworkEngine);

	//绑定参数
	m_pBindParameter=new tagBindParameter[m_pInitParamter->m_wMaxConnect];
	ZeroMemory(m_pBindParameter,sizeof(tagBindParameter)*m_pInitParamter->m_wMaxConnect);

	//中心连接
	DWORD dwServerIP=inet_addr(m_pInitParamter->m_szCenterServerAddr);
	m_pITCPSocketCorrespond->Connect(htonl(dwServerIP),m_pInitParamter->m_wCenterServerPort);

	//初始数据
	//m_DownloadFaceInfoMap.InitHashTable(503);
	//m_DownloadFaceInfoMap.RemoveAll();

	//设置定时器
	//m_pITimerEngine->SetTimer(IDI_DOWNLOAD_CUSTOM_FACE,TIME_DOWNLOAD_CUSTOM_FACE,TIMES_INFINITY,NULL);

	//加载敏感词文件,从文件读取
	//读取配置
	m_vecInValidWord.clear();
	std::ifstream readfile;
	readfile.open("mingan.txt",std::ios::in);
	while(readfile.is_open() && !readfile.eof())
	{
		char ch[128];
		memset(ch, 0, 128);
		readfile.getline(ch,128,'\n');

		std::string tmp(ch);
		if (tmp.size() > 0)
		{
			m_vecInValidWord.push_back(ch);
		}
	}
	readfile.close();

	//对应替换成的文本
	m_vecWordReplaced.clear();
	readfile.open("mingan_r.txt",std::ios::in);
	while(readfile.is_open() && !readfile.eof())
	{
		char ch[128];
		memset(ch, 0, 128);
		readfile.getline(ch,128,'\n');

		std::string tmp(ch);
		if (tmp.size() > 0)
		{
			m_vecWordReplaced.push_back(ch);
		}
	}
	readfile.close();

	//不能超长
	if (m_vecWordReplaced.size() < m_vecInValidWord.size())
	{
		m_vecInValidWord.resize(m_vecWordReplaced.size());
	}

	return true;
}

//调度模块关闭
bool __cdecl CAttemperEngineSink::OnAttemperEngineStop(IUnknownEx * pIUnknownEx)
{
	//设置变量
	m_pITimerEngine=NULL;
	m_pIDataBaseEngine=NULL;
	m_pITCPNetworkEngine=NULL;
	m_pITCPSocketCorrespond=NULL;

	//删除数据
	SafeDeleteArray(m_pBindParameter);

	return true;
}

//定时器事件
bool __cdecl CAttemperEngineSink::OnEventTimer(DWORD dwTimerID, WPARAM wBindParam)
{
	switch (dwTimerID)
	{
	case IDI_CONNECT_CENTER_SERVER:		//连接中心服务器
		{
			//发起连接
			DWORD dwServerIP=inet_addr(m_pInitParamter->m_szCenterServerAddr);
			m_pITCPSocketCorrespond->Connect(htonl(dwServerIP),m_pInitParamter->m_wCenterServerPort);

			//错误提示
			CTraceService::TraceString(TEXT("正在尝试重新连接中心服务器...."),TraceLevel_Normal);

			return true;
		}
	case IDI_UPDATE_SERVER_LIST:		//更新服务器列表
		{
			m_pITCPSocketCorrespond->SendData(MDM_CS_SERVER_LIST,SUB_CS_GET_SERVER_LIST,NULL,0);

			return true;
		}
	}

	return false;
}

//数据库事件
bool __cdecl CAttemperEngineSink::OnEventDataBase(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	switch (wRequestID)
	{
	case DBR_GP_LOGON_ERROR:			//登录失败
		{
			return OnDBLogonError(dwContextID,pData,wDataSize);
		}
	case DBR_GP_LOGON_SUCCESS:			//登录成功
		{
			return OnDBLogonSuccess(dwContextID,pData,wDataSize);
		}
	case DBR_GP_SIGNIN_RESULT:		//签到结果
		{
			return OnDBUserSignInResult(dwContextID,pData,wDataSize);
		}
	case DBR_GP_GETAWARD_RESULT:		//领奖结果
		{
			return OnDBUserGetAwardResult(dwContextID,pData,wDataSize);
		}
	case DBR_GP_EXCHANGE_INFO_RESULT:		//兑换的个人信息
		{
			return OnDBUserExchangeInfoResult(dwContextID,pData,wDataSize);
		}
	case DBR_GP_EXCHANGE_PRODUCT_RESULT:		//兑换的商品
		{
			return OnDBUserExchangeProductResult(dwContextID,pData,wDataSize);
		}
	case DBR_GP_EXCHANGE_PRODUCT_RESULT_V2:		//兑换的商品
		{
			return OnDBUserExchangeProductResult_V2(dwContextID,pData,wDataSize);
		}
	case DBR_GP_EXCHANGE_RESULT:		//兑换
		{
			return OnDBUserExchangeResult(dwContextID,pData,wDataSize);
		}
	case DBR_GP_EXCHANGE_RESULT_V2:		//兑换
		{
			return OnDBUserExchangeResult_V2(dwContextID,pData,wDataSize);
		}
	case DBR_GP_EXCHANGE_RECORD_RESULT:		//兑换记录
		{
			return OnDBUserExchangeRecordResult(dwContextID,pData,wDataSize);
		}
	case DBR_GP_DOWNLOADFACE_SUCCESS:	//下载成功
		{
			return OnDBDownloadFaceSuccess(dwContextID,pData,wDataSize);
		}
	case DBR_GP_UPLOAD_FACE_RESULT:		//上传结果
		{
			return OnDBRCustomFaceUploadResult(dwContextID,pData,wDataSize);
		}
	case DBR_GP_DELETE_FACE_RESULT:		//删除结果
		{
			return OnDBRCustomFaceDeleteResult(dwContextID,pData,wDataSize);
		}
	case DBR_GP_MODIFY_RESULT:			//修改结果
		{
			return OnDBRModifyIndividual(dwContextID,pData,wDataSize);
		}
	case DBR_GP_MODIFY_UNDERWRITE_RESULT:	//修改个性签名结果
		{
			return OnDBRModifyUnderWrite(dwContextID,pData,wDataSize);
		}
	case DBR_GP_MODIFY_PHONE_RESULT:	//修改手机号结果
		{
			return OnDBRModifyPhone(dwContextID,pData,wDataSize);
		}
	case DBR_GP_CHECKID_RESULT:			//校验名称结果
		{
			return OnDBRCheckID(dwContextID,pData,wDataSize);
		}
	case DBR_GP_CHECK_NICKNAME_RESULT:			//校验昵称
		{
			return OnDBRCheckNickName(dwContextID,pData,wDataSize);
		}
	case DBR_GP_REGISTER_RESULT:			//注册
		{
			return OnDBRRegister(dwContextID,pData,wDataSize);
		}
	case DBR_GP_ID_UPDATE_RESULT:			//帐号升级
		{
			return OnDBRIDUpdate(dwContextID,pData,wDataSize);
		}
	case DBR_GP_RESET_PWD_RESULT:			//重设密码
		{
			return OnDBRResetPwd(dwContextID,pData,wDataSize);
		}
	case DBR_GP_USER_INFO_RESULT:			//查询用户信息
		{
			return OnDBRUserInfo(dwContextID,pData,wDataSize);
		}
	case DBR_GP_USER_CAN_SPREAD_RESULT:			//查询用户是否可以领取推广奖励
		{
			return OnDBRUserCanSpread(dwContextID,pData,wDataSize);
		}
	case DBR_GP_USER_GET_SPREAD_RESULT:			//用户领取推广奖励
		{
			return OnDBRUserGetSpread(dwContextID,pData,wDataSize);
		}
	case DBR_GP_USER_SPREADER_INFO_RESULT:		//查询推广员信息
		{
			return OnDBRUserSpreaderInfo(dwContextID,pData,wDataSize);
		}
	case DBR_GP_USER_GET_BBS_POP_RESULT:		//弹窗公告信息
		{
			return OnDBRUserBbsPop(dwContextID,pData,wDataSize);
		}
	case DBR_GP_USER_GET_BBS_SCROLL_RESULT:		//滚动公告信息
		{
			return OnDBRUserBbsScroll(dwContextID,pData,wDataSize);
		}
	case DBR_GP_USER_GET_BBS_LIST_RESULT:		//列表公告信息
		{
			return OnDBRUserBbsList(dwContextID,pData,wDataSize);
		}
	case DBR_GP_USER_GET_TASK_INFO_RESULT:		//玩家任务信息
		{
			return OnDBRUserTaskInfo(dwContextID,pData,wDataSize);
		}
	case DBR_GP_USER_GET_RANKING_RESULT:		//玩家排行榜
		{
			return OnDBRUserGetRanking(dwContextID,pData,wDataSize);
		}
	case DBR_GP_USER_GET_RANKING_RESULT_V2:		//玩家排行榜
		{
			return OnDBRUserGetRanking_V2(dwContextID,pData,wDataSize);
		}
	case DBR_GP_USER_GET_RANKINGSTATUS:			//查询玩家是否入排行榜
		{
			return OnDBRUserGetRankingStatus(dwContextID,pData,wDataSize);
		}
	case DBR_GP_USER_GET_MODIFY_RANKINGSTATUS:	//修改玩家是否入排行榜返回信息
		{
			return OnDBRUserGetRankingStatusResult(dwContextID,pData,wDataSize);
		}
	case DBR_GP_USER_FEEDBACK_RESULT:			//意见反馈的结果
		{
			return OnDBRUserFeedBack(dwContextID,pData,wDataSize);
		}
	case DBR_GP_USER_GET_FEEDBACK_RESULT:			//查询意见反馈的结果
		{
			return OnDBRUserGetFeedBack(dwContextID,pData,wDataSize);
		}
	case DBR_GP_USER_INFO_ID_CARD_RESULT:			//查询用户身份证账号信息
		{
			return OnDBRUserInfoIDCard(dwContextID,pData,wDataSize);
		}
	case DBR_GP_USER_AUTHENTICATION_RESULT:			//请求提交身份证信息
		{
			return OnDBRUserAuthentication(dwContextID,pData,wDataSize);
		}
	case DBR_GP_USER_MALL_PRODUCT_RESULT:			//请求商城商品列表
		{
			return OnDBRUserMallProductResult(dwContextID, pData, wDataSize);
		}
	case DBR_GP_USER_MALL_PLACE_ORDER_RESULT:			//请求购买下订单
		{
			return OnDBRUserMallPlaceOrderResult(dwContextID, pData, wDataSize);
		}
	case DBR_GP_USER_BANK_INFO_RESULT:			//查询用户银行信息结果
		{
			return OnDBRUserBankInfoResult(dwContextID, pData, wDataSize);
		}
	case DBR_GP_USER_BANK_CHARGE_RESULT:		//用户操作银行结果
		{
			return OnDBRUserBankChargeResult(dwContextID, pData, wDataSize);
		}
	case DBR_GP_USER_CANCEL_ORDER_NUM:
		{
			return OnDBRUserCancelOrderResult(dwContextID, pData, wDataSize);
		}
	case DBR_GP_USER_MALL_BUY_RESULT:
		{
			return OnDBRUserMallBuyResult(dwContextID, pData, wDataSize);
		}
	case DBR_GP_USER_MALL_UPDATE_RESULT:
		{
			return OnDBRUserMallUpdateResult(dwContextID, pData, wDataSize);
		}
	case DBR_GP_USER_ACTIVITY_LIST_RESULT:
		{
			return OnDBRUserActivityListResult(dwContextID, pData, wDataSize);
		}
	case DBR_GP_USER_ACTIVITY_RESULT:
		{
			return OnDBRUserActivityResult(dwContextID, pData, wDataSize);
		}
	case DBR_GP_USER_ACTIVITY_LUCKY_RESULT:
		{
			return OnDBRUserActivityLuckyResult(dwContextID, pData, wDataSize);
		}
	case DBR_GP_USER_ACTIVITY_RECORD_RESULT:	
		{
			return OnDBRUserActivityRecordResult(dwContextID,pData,wDataSize);
		}
	case DBR_GP_USER_SPEAKER_SEND_RESULT:	
		{
			return OnDBRUserSpeakerSendResult(dwContextID,pData,wDataSize);
		}
	case DBR_GP_USER_QUERY_CONFIG_RESULT:	
		{
			return OnDBRUserQueryConfigResult(dwContextID,pData,wDataSize);
		}
	}

	return false;
}

//兑换的个人信息
bool CAttemperEngineSink::OnDBUserExchangeInfoResult(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//效验参数
	ASSERT(wDataSize==sizeof(DBR_GP_UserExchangeInfoResult));
	if (wDataSize!=sizeof(DBR_GP_UserExchangeInfoResult)) return false;

	//判断在线
	ASSERT(LOWORD(dwContextID)<m_pInitParamter->m_wMaxConnect);
	if ((m_pBindParameter+LOWORD(dwContextID))->dwSocketID!=dwContextID) return true;

	//变量定义
	char cbBuffer[SOCKET_PACKET];
	DBR_GP_UserExchangeInfoResult * pExchangeInfo=(DBR_GP_UserExchangeInfoResult *)pData;
	CMD_GP_UserExchangeInfo exchangeInfo;

	//构造数据
	exchangeInfo.dwGold = pExchangeInfo->dwGold;
	exchangeInfo.nCode = pExchangeInfo->bOperateSuccess;
	memcpy(exchangeInfo.szPhone, pExchangeInfo->szPhone, sizeof(pExchangeInfo->szPhone));

	memset(cbBuffer, 0, SOCKET_PACKET);
	bostream bos;
	bos.attach(cbBuffer, SOCKET_PACKET);
	bos << exchangeInfo;

	m_pITCPNetworkEngine->SendData(dwContextID,MDM_GP_USER,SUB_GP_USER_EXCHANGE_INFO,cbBuffer,bos.length());

	return true;
}

//兑换的商品
bool CAttemperEngineSink::OnDBUserExchangeProductResult(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//效验参数
	ASSERT(wDataSize==sizeof(DBR_GP_UserExchangeProductResult));
	if (wDataSize!=sizeof(DBR_GP_UserExchangeProductResult)) return false;

	//判断在线
	ASSERT(LOWORD(dwContextID)<m_pInitParamter->m_wMaxConnect);
	if ((m_pBindParameter+LOWORD(dwContextID))->dwSocketID!=dwContextID) return true;

	//变量定义
	char cbBuffer[SOCKET_PACKET];
	DBR_GP_UserExchangeProductResult * pExchangeProduct=(DBR_GP_UserExchangeProductResult *)pData;

	for (int i=0; i<pExchangeProduct->vecProducts.size(); i++)
	{
		CMD_GP_UserExchangeProduct exchangeProduct;

		//构造数据
		exchangeProduct.dwAwardID = pExchangeProduct->vecProducts[i].dwAwardID;
		exchangeProduct.dwLeft = pExchangeProduct->vecProducts[i].dwLeft;
		exchangeProduct.dwPrice = pExchangeProduct->vecProducts[i].dwPrice;
		memcpy(exchangeProduct.szAwardname, pExchangeProduct->vecProducts[i].szAwardname, sizeof(pExchangeProduct->vecProducts[i].szAwardname));
		memcpy(exchangeProduct.szAwardImage, pExchangeProduct->vecProducts[i].szAwardImg, sizeof(pExchangeProduct->vecProducts[i].szAwardImg));

		memset(cbBuffer, 0, SOCKET_PACKET);
		bostream bos;
		bos.attach(cbBuffer, SOCKET_PACKET);
		bos << exchangeProduct;

		m_pITCPNetworkEngine->SendData(dwContextID,MDM_GP_USER,SUB_GP_USER_EXCHANGE_PRODUCT,cbBuffer,bos.length());
	}

	//结束
	CMD_GP_UserExchangeProductFinish exchangeFinish;
	exchangeFinish.dwTotal = pExchangeProduct->vecProducts.size();

	memset(cbBuffer, 0, SOCKET_PACKET);
	bostream bos;
	bos.attach(cbBuffer, SOCKET_PACKET);
	bos << exchangeFinish;

	m_pITCPNetworkEngine->SendData(dwContextID,MDM_GP_USER,SUB_GP_USER_EXCHANGE_PRODUCT_FINISH,cbBuffer,bos.length());

	return true;
}

//兑换的商品
bool CAttemperEngineSink::OnDBUserExchangeProductResult_V2(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//效验参数
	ASSERT(wDataSize==sizeof(DBR_GP_UserExchangeProductResult));
	if (wDataSize!=sizeof(DBR_GP_UserExchangeProductResult)) return false;

	//判断在线
	ASSERT(LOWORD(dwContextID)<m_pInitParamter->m_wMaxConnect);
	if ((m_pBindParameter+LOWORD(dwContextID))->dwSocketID!=dwContextID) return true;

	//变量定义
	char cbBuffer[SOCKET_PACKET];
	DBR_GP_UserExchangeProductResult * pExchangeProduct=(DBR_GP_UserExchangeProductResult *)pData;

	for (int i=0; i<pExchangeProduct->vecProducts.size(); i++)
	{
		CMD_GP_UserExchangeProduct_V2 exchangeProduct;

		//构造数据
		exchangeProduct.dwAwardID = pExchangeProduct->vecProducts[i].dwAwardID;
		exchangeProduct.dwLeft = pExchangeProduct->vecProducts[i].dwLeft;
		exchangeProduct.dwPrice = pExchangeProduct->vecProducts[i].dwPrice;
		exchangeProduct.dwType = pExchangeProduct->vecProducts[i].dwType;
		memcpy(exchangeProduct.szAwardname, pExchangeProduct->vecProducts[i].szAwardname, sizeof(pExchangeProduct->vecProducts[i].szAwardname));
		memcpy(exchangeProduct.szAwardImage, pExchangeProduct->vecProducts[i].szAwardImg, sizeof(pExchangeProduct->vecProducts[i].szAwardImg));

		memset(cbBuffer, 0, SOCKET_PACKET);
		bostream bos;
		bos.attach(cbBuffer, SOCKET_PACKET);
		bos << exchangeProduct;

		m_pITCPNetworkEngine->SendData(dwContextID,MDM_GP_USER,SUB_GP_USER_EXCHANGE_PRODUCT_V2,cbBuffer,bos.length());
	}

	//结束
	CMD_GP_UserExchangeProductFinish exchangeFinish;
	exchangeFinish.dwTotal = pExchangeProduct->vecProducts.size();

	memset(cbBuffer, 0, SOCKET_PACKET);
	bostream bos;
	bos.attach(cbBuffer, SOCKET_PACKET);
	bos << exchangeFinish;

	m_pITCPNetworkEngine->SendData(dwContextID,MDM_GP_USER,SUB_GP_USER_EXCHANGE_PRODUCT_FINISH,cbBuffer,bos.length());

	return true;
}

//兑换
bool CAttemperEngineSink::OnDBUserExchangeResult(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//效验参数
	ASSERT(wDataSize==sizeof(DBR_GP_UserExchangeResult));
	if (wDataSize!=sizeof(DBR_GP_UserExchangeResult)) return false;

	//判断在线
	ASSERT(LOWORD(dwContextID)<m_pInitParamter->m_wMaxConnect);
	if ((m_pBindParameter+LOWORD(dwContextID))->dwSocketID!=dwContextID) return true;

	//变量定义
	char cbBuffer[SOCKET_PACKET];
	DBR_GP_UserExchangeResult * pExchange=(DBR_GP_UserExchangeResult *)pData;
	CMD_GP_UserExchange exchange;

	//构造数据
	exchange.dwGold = pExchange->dwGold;
	exchange.dwAwardID = pExchange->dwAwardID;
	exchange.nCode = pExchange->bOperateSuccess;

	memset(cbBuffer, 0, SOCKET_PACKET);
	bostream bos;
	bos.attach(cbBuffer, SOCKET_PACKET);
	bos << exchange;

	m_pITCPNetworkEngine->SendData(dwContextID,MDM_GP_USER,SUB_GP_USER_EXCHANGE,cbBuffer,bos.length());

	return true;
}



//兑换
bool CAttemperEngineSink::OnDBUserExchangeResult_V2(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//效验参数
	ASSERT(wDataSize==sizeof(DBR_GP_UserExchangeResult));
	if (wDataSize!=sizeof(DBR_GP_UserExchangeResult)) return false;

	//判断在线
	ASSERT(LOWORD(dwContextID)<m_pInitParamter->m_wMaxConnect);
	if ((m_pBindParameter+LOWORD(dwContextID))->dwSocketID!=dwContextID) return true;

	//变量定义
	char cbBuffer[SOCKET_PACKET];
	DBR_GP_UserExchangeResult * pExchange=(DBR_GP_UserExchangeResult *)pData;
	CMD_GP_UserExchange_V2 exchange;

	//构造数据
	exchange.dwGold = pExchange->dwGold;
	exchange.dwAwardID = pExchange->dwAwardID;
	exchange.nCode = pExchange->bOperateSuccess;
	exchange.dwScore = pExchange->dwScore;

	memset(cbBuffer, 0, SOCKET_PACKET);
	bostream bos;
	bos.attach(cbBuffer, SOCKET_PACKET);
	bos << exchange;

	m_pITCPNetworkEngine->SendData(dwContextID,MDM_GP_USER,SUB_GP_USER_EXCHANGE_V2,cbBuffer,bos.length());

	return true;
}

//兑换记录
bool CAttemperEngineSink::OnDBUserExchangeRecordResult(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//效验参数
	ASSERT(wDataSize==sizeof(DBR_GP_UserExchangeRecordResult));
	if (wDataSize!=sizeof(DBR_GP_UserExchangeRecordResult)) return false;

	//判断在线
	ASSERT(LOWORD(dwContextID)<m_pInitParamter->m_wMaxConnect);
	if ((m_pBindParameter+LOWORD(dwContextID))->dwSocketID!=dwContextID) return true;

	//变量定义
	char cbBuffer[SOCKET_PACKET];
	DBR_GP_UserExchangeRecordResult * pExchangeRecord=(DBR_GP_UserExchangeRecordResult *)pData;

	int iTotal = 0;

	for (int i=0; i<pExchangeRecord->vecRecords.size(); i++)
	{
		CMD_GP_UserExchangeRecord exchangeRecord;

		//构造数据
		exchangeRecord.dwAwardID = pExchangeRecord->vecRecords[i].dwAwardID;
		exchangeRecord.dwID = pExchangeRecord->vecRecords[i].dwID;
		exchangeRecord.dwTotal = pExchangeRecord->vecRecords[i].dwTotal;
//		exchangeRecord.dwExchangeDate = pExchangeRecord->vecRecords[i].dwDate;
		memcpy(exchangeRecord.szAwardname, pExchangeRecord->vecRecords[i].szAwardName, sizeof(pExchangeRecord->vecRecords[i].szAwardName));
		memcpy(exchangeRecord.szStatus, pExchangeRecord->vecRecords[i].szStatus, sizeof(pExchangeRecord->vecRecords[i].szStatus));
		memcpy(exchangeRecord.szExchangeDate, pExchangeRecord->vecRecords[i].szDate, sizeof(pExchangeRecord->vecRecords[i].szDate));

		memset(cbBuffer, 0, SOCKET_PACKET);
		bostream bos;
		bos.attach(cbBuffer, SOCKET_PACKET);
		bos << exchangeRecord;

		iTotal = pExchangeRecord->vecRecords[i].dwTotal;

		m_pITCPNetworkEngine->SendData(dwContextID,MDM_GP_USER,SUB_GP_USER_EXCHANGE_RECORD,cbBuffer,bos.length());
	}

	//结束
	CMD_GP_UserExchangeRecordFinish exchangeFinish;
	exchangeFinish.dwTotal = pExchangeRecord->vecRecords.size();

	memset(cbBuffer, 0, SOCKET_PACKET);
	bostream bos;
	bos.attach(cbBuffer, SOCKET_PACKET);
	bos << exchangeFinish;

	m_pITCPNetworkEngine->SendData(dwContextID,MDM_GP_USER,SUB_GP_USER_EXCHANGE_RECORD_FINISH,cbBuffer,bos.length());

	return true;
}

//用户签到
bool CAttemperEngineSink::OnDBUserSignInResult(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//效验参数
	ASSERT(wDataSize==sizeof(DBR_GP_UserSignInResult));
	if (wDataSize!=sizeof(DBR_GP_UserSignInResult)) return false;

	//判断在线
	ASSERT(LOWORD(dwContextID)<m_pInitParamter->m_wMaxConnect);
	if ((m_pBindParameter+LOWORD(dwContextID))->dwSocketID!=dwContextID) return true;

	//变量定义
	char cbBuffer[SOCKET_PACKET];
	DBR_GP_UserSignInResult * pSignIn=(DBR_GP_UserSignInResult *)pData;
	CMD_GP_UserSignIn signIn;

	//构造数据
	signIn.cbTodaySign = pSignIn->cbTodaySign;
	signIn.dwGoldScore = pSignIn->dwGoldScore;
	signIn.wTimes = pSignIn->wTimes;

	memset(cbBuffer, 0, SOCKET_PACKET);
	bostream bos;
	bos.attach(cbBuffer, SOCKET_PACKET);
	bos << signIn;

	m_pITCPNetworkEngine->SendData(dwContextID,MDM_GP_USER,SUB_GP_USER_SIGNIN,cbBuffer,bos.length());

	return true;
}

//用户领奖
bool CAttemperEngineSink::OnDBUserGetAwardResult(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//效验参数
	ASSERT(wDataSize==sizeof(DBR_GP_UserGetAwardResult));
	if (wDataSize!=sizeof(DBR_GP_UserGetAwardResult)) return false;

	//判断在线
	ASSERT(LOWORD(dwContextID)<m_pInitParamter->m_wMaxConnect);
	if ((m_pBindParameter+LOWORD(dwContextID))->dwSocketID!=dwContextID) return true;

	//变量定义
	char cbBuffer[SOCKET_PACKET];
	DBR_GP_UserGetAwardResult * pAward=(DBR_GP_UserGetAwardResult *)pData;
	CMD_GP_UserGetAward getAward;

	//构造数据
	getAward.cbStatus = pAward->cbStatus;
	getAward.dwScore = pAward->dwScore;
	getAward.dwTaskID = pAward->dwTaskID;

	memset(cbBuffer, 0, SOCKET_PACKET);
	bostream bos;
	bos.attach(cbBuffer, SOCKET_PACKET);
	bos << getAward;

	m_pITCPNetworkEngine->SendData(dwContextID,MDM_GP_USER,SUB_GP_USER_GETAWARD,cbBuffer,bos.length());

	return true;
}

//用户登录成功
bool CAttemperEngineSink::OnDBLogonSuccess(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//效验参数
	ASSERT(wDataSize==sizeof(DBR_GP_LogonSuccess));
	if (wDataSize!=sizeof(DBR_GP_LogonSuccess)) return false;

	//判断在线
	ASSERT(LOWORD(dwContextID)<m_pInitParamter->m_wMaxConnect);
	if ((m_pBindParameter+LOWORD(dwContextID))->dwSocketID!=dwContextID) return true;

	//变量定义
	char cbBuffer[SOCKET_PACKET];
	DBR_GP_LogonSuccess * pDBRLogonSuccess=(DBR_GP_LogonSuccess *)pData;
//	CMD_GP_LogonSuccess * pCMDLogonSuccess=(CMD_GP_LogonSuccess *)cbBuffer;
	CMD_GP_LogonSuccess_CB pCMDLogonSuccess;


	//保存玩家id和socketID
	std::map<unsigned int, DWORD>::iterator iter = m_mapRechargeUserSocketID.find(pDBRLogonSuccess->dwUserID);
	if (iter != m_mapRechargeUserSocketID.end())
	{
		iter->second = dwContextID;
	}
	else
	{
		m_mapRechargeUserSocketID.insert(std::pair<unsigned int , DWORD>(pDBRLogonSuccess->dwUserID, dwContextID));
		m_mapRechargeSocketIDUser.insert(std::pair<unsigned int , DWORD>(dwContextID, pDBRLogonSuccess->dwUserID));
	}
	

	//构造数据
	pCMDLogonSuccess.wFaceID=pDBRLogonSuccess->wFaceID;
	pCMDLogonSuccess.cbGender=pDBRLogonSuccess->cbGender;
	pCMDLogonSuccess.cbIsGuest=pDBRLogonSuccess->cbIsGuest;
	pCMDLogonSuccess.cbMember=pDBRLogonSuccess->cbMember;
	pCMDLogonSuccess.dwUserID=pDBRLogonSuccess->dwUserID;
	pCMDLogonSuccess.dwGameID=pDBRLogonSuccess->dwGameID;
	pCMDLogonSuccess.dwExperience=pDBRLogonSuccess->dwExperience;
	pCMDLogonSuccess.dwGoldScore=pDBRLogonSuccess->lGoldScore;
	pCMDLogonSuccess.lWelfare=pDBRLogonSuccess->lWelfare;
	pCMDLogonSuccess.wWelfareLost=pDBRLogonSuccess->wWelfareLost;
	pCMDLogonSuccess.dwCustomFaceVer=pDBRLogonSuccess->dwCustomFaceVer;
	memcpy(pCMDLogonSuccess.szAccounts, pDBRLogonSuccess->szAccounts, sizeof(pDBRLogonSuccess->szAccounts));
	memcpy(pCMDLogonSuccess.szNickName, pDBRLogonSuccess->szNickName, sizeof(pDBRLogonSuccess->szNickName));
	memcpy(pCMDLogonSuccess.szPassWord, pDBRLogonSuccess->szPassword, sizeof(pDBRLogonSuccess->szPassword));
#ifdef USE_UNDERWRITE
	memcpy(pCMDLogonSuccess.szUnderWrite, pDBRLogonSuccess->szUnderWrite, sizeof(pDBRLogonSuccess->szUnderWrite));
#endif

	//叠加数据
/*	CSendPacketHelper SendPacket(cbBuffer+sizeof(CMD_GP_LogonSuccess),sizeof(cbBuffer)-sizeof(CMD_GP_LogonSuccess));
	SendPacket.AddPacket(pDBRLogonSuccess->szPassword,CountStringBuffer(pDBRLogonSuccess->szPassword),DTP_USER_PASS);
	SendPacket.AddPacket(pDBRLogonSuccess->szAccounts,CountStringBuffer(pDBRLogonSuccess->szAccounts),DTP_USER_ACCOUNTS);
	SendPacket.AddPacket(pDBRLogonSuccess->szUnderWrite,CountStringBuffer(pDBRLogonSuccess->szUnderWrite),DTP_UNDER_WRITE);

	//社团信息
	if (pDBRLogonSuccess->szGroupName[0]!=0)
	{
		SendPacket.AddPacket(pDBRLogonSuccess->szGroupName,CountStringBuffer(pDBRLogonSuccess->szGroupName),DTP_USER_GROUP_NAME);
	}
	
	//站点主页
	if (m_pInitParamter->m_szMainPage[0]!=0)
	{
		SendPacket.AddPacket(m_pInitParamter->m_szMainPage,CountStringBuffer(m_pInitParamter->m_szMainPage),DTP_STATION_PAGE);
	}

	//发送登录结果
	WORD wSendSize=sizeof(CMD_GP_LogonSuccess)+SendPacket.GetDataSize();
*/

	memset(cbBuffer, 0, SOCKET_PACKET);
	bostream bos;
	bos.attach(cbBuffer, SOCKET_PACKET);
	bos << pCMDLogonSuccess;

	m_pITCPNetworkEngine->SendData(dwContextID,MDM_GP_LOGON,SUB_GP_LOGON_SUCCESS,cbBuffer,bos.length());
//	m_pITCPNetworkEngine->SendData(dwContextID,MDM_GP_LOGON,SUB_GP_LOGON_SUCCESS,cbBuffer,wSendSize);

    // 个性签名协议的兼容版本实现
#ifndef USE_UNDERWRITE
	CMD_GP_UnderWrite UnderWrite;
	ZeroMemory(&UnderWrite, sizeof(UnderWrite));
	UnderWrite.dwUserID = pDBRLogonSuccess->dwUserID;
	lstrcpyn(UnderWrite.szUnderWrite, pDBRLogonSuccess->szUnderWrite, sizeof(pDBRLogonSuccess->szUnderWrite));

	//发送数据
	m_pITCPNetworkEngine->SendData(dwContextID, MDM_GP_USER, SUB_GP_UNDERWRITE, &UnderWrite, sizeof(UnderWrite));
#endif

	// 是否有游客账号
	CMD_GP_HasGuest HasGuest;
	ZeroMemory(&HasGuest, sizeof(HasGuest));
	HasGuest.dwUserID = pDBRLogonSuccess->dwUserID;
	HasGuest.cbHasGuest = pDBRLogonSuccess->cbHasGuest;

	//发送数据
	m_pITCPNetworkEngine->SendData(dwContextID, MDM_GP_USER, SUB_GP_HASGUEST, &HasGuest, sizeof(HasGuest));

	//列表配置
	CMD_GP_ListConfig_CB ListConfig;
	ListConfig.bShowOnLineCount=TRUE;
	memcpy(ListConfig.szRule, pDBRLogonSuccess->szRule, sizeof(pDBRLogonSuccess->szRule));

	char cbBuffer2[SOCKET_PACKET];
	memset(cbBuffer2, 0, SOCKET_PACKET);
	bostream bos2;
	bos2.attach(cbBuffer2, SOCKET_PACKET);
	bos2 << ListConfig;

	m_pITCPNetworkEngine->SendData(dwContextID,MDM_GP_SERVER_LIST,SUB_GP_LIST_CONFIG,cbBuffer2,bos2.length());

	//发送游戏列表
	m_ServerList.SendGameTypeList(dwContextID);
	m_ServerList.SendGameKindList(dwContextID);
	m_ServerList.SendGameStationList(dwContextID);
	m_ServerList.SendGameServerList(dwContextID);

	//发送登录完成
	m_pITCPNetworkEngine->SendData(dwContextID,MDM_GP_LOGON,SUB_GP_LOGON_FINISH);

	//关闭连接
//	if ( pDBRLogonSuccess->dwCustomFaceVer == 0 )
//		m_pITCPNetworkEngine->ShutDownSocket(dwContextID);

	return true;
}

//用户登录失败
bool CAttemperEngineSink::OnDBLogonError(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//效验参数
	ASSERT(wDataSize==sizeof(DBR_GP_LogonError));
	if (wDataSize!=sizeof(DBR_GP_LogonError)) return false;

	//判断在线
	ASSERT(LOWORD(dwContextID)<m_pInitParamter->m_wMaxConnect);
	if ((m_pBindParameter+LOWORD(dwContextID))->dwSocketID!=dwContextID) return true;

	//变量定义
	CMD_GP_LogonError_CB LogonError;
	DBR_GP_LogonError * pLogonError=(DBR_GP_LogonError *)pData;
	pLogonError->szErrorDescribe[CountStringBuffer(pLogonError->szErrorDescribe)-1]=0;

	//构造数据
	LogonError.lErrorCode=pLogonError->lErrorCode;
	lstrcpyn(LogonError.szErrorDescribe,pLogonError->szErrorDescribe,sizeof(LogonError.szErrorDescribe));
//	WORD wDescribeSize=CountStringBuffer(LogonError.szErrorDescribe);

	char cbBuffer[SOCKET_PACKET];

	bostream bos;
	bos.attach(cbBuffer, SOCKET_PACKET);
	bos << LogonError;

	//发送数据
	m_pITCPNetworkEngine->SendData(dwContextID,MDM_GP_LOGON,SUB_GP_LOGON_ERROR,cbBuffer,bos.length());
//	WORD wSendSize=sizeof(LogonError)-sizeof(LogonError.szErrorDescribe)+wDescribeSize;
//	m_pITCPNetworkEngine->SendData(dwContextID,MDM_GP_LOGON,SUB_GP_LOGON_ERROR,&LogonError,wSendSize);

	//关闭连接
	m_pITCPNetworkEngine->ShutDownSocket(dwContextID);

	return true;
}

//下载成功
bool CAttemperEngineSink::OnDBDownloadFaceSuccess(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//数据转换
	DBR_GP_DownloadFaceSuccess *pDownloadFaceSuccess = (DBR_GP_DownloadFaceSuccess*)pData;

	//数据验证
	WORD wPostSize = WORD(sizeof(DBR_GP_DownloadFaceSuccess)-sizeof(pDownloadFaceSuccess->bFaceData)+pDownloadFaceSuccess->dwCurrentSize);
	ASSERT(wDataSize == wPostSize);
	if ( wPostSize != wDataSize ) return false;

	//发送数据
	CMD_GP_DownloadFaceSuccess DownloadFaceSuccess;
	DownloadFaceSuccess.dwUserID = pDownloadFaceSuccess->dwUserID;
	DownloadFaceSuccess.dwToltalSize = pDownloadFaceSuccess->dwToltalSize;
	DownloadFaceSuccess.dwCurrentSize = pDownloadFaceSuccess->dwCurrentSize;
	CopyMemory(DownloadFaceSuccess.bFaceData, pDownloadFaceSuccess->bFaceData, pDownloadFaceSuccess->dwCurrentSize);
	m_pITCPNetworkEngine->SendData(dwContextID, MDM_GP_USER, SUB_GP_USER_DOWNLOAD_FACE, &DownloadFaceSuccess, wPostSize);

	return true;
}

//成功消息
bool CAttemperEngineSink::OnDBRCustomFaceUploadResult(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	ASSERT(wDataSize == sizeof(DBR_GP_UploadFaceResult));
	if ( wDataSize != sizeof(DBR_GP_UploadFaceResult)) return false;

	//类型转换
	DBR_GP_UploadFaceResult *pUploadFaceResult = (DBR_GP_UploadFaceResult*)pData;
	pUploadFaceResult->szDescribeMsg[sizeof(pUploadFaceResult->szDescribeMsg)-1] = 0;

	//设置数据
	CMD_GP_UploadFaceResult UploadFaceResult;
	ZeroMemory(&UploadFaceResult, sizeof(UploadFaceResult));
	lstrcpy(UploadFaceResult.szDescribeMsg, pUploadFaceResult->szDescribeMsg);
	UploadFaceResult.dwFaceVer = pUploadFaceResult->dwFaceVer;
	UploadFaceResult.bOperateSuccess=pUploadFaceResult->bOperateSuccess;

	//发送数据
	m_pITCPNetworkEngine->SendData(dwContextID, MDM_GP_USER, SUB_GP_UPLOAD_FACE_RESULT, &UploadFaceResult, sizeof(UploadFaceResult));

	//关闭链接
	m_pITCPNetworkEngine->ShutDownSocket(dwContextID);

	return true;
}

//删除结果
bool CAttemperEngineSink::OnDBRCustomFaceDeleteResult(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//数据验证
	ASSERT(wDataSize == sizeof(DBR_GP_DeleteFaceResult));
	if (wDataSize != sizeof(DBR_GP_DeleteFaceResult)) return false;

	//类型转换
	DBR_GP_DeleteFaceResult *pDeleteFaceResult = (DBR_GP_DeleteFaceResult*)pData;
	pDeleteFaceResult->szDescribeMsg[sizeof(pDeleteFaceResult->szDescribeMsg)-1] = 0;

	//设置数据
	CMD_GP_DeleteFaceResult DeleteFaceResult;
	ZeroMemory(&DeleteFaceResult, sizeof(DeleteFaceResult));
	lstrcpy(DeleteFaceResult.szDescribeMsg, pDeleteFaceResult->szDescribeMsg);
	DeleteFaceResult.dwFaceVer=pDeleteFaceResult->dwFaceVer;
	DeleteFaceResult.bOperateSuccess = pDeleteFaceResult->bOperateSuccess;

	//发送数据
	m_pITCPNetworkEngine->SendData(dwContextID, MDM_GP_USER, SUB_GP_DELETE_FACE_RESULT, &DeleteFaceResult, sizeof(DeleteFaceResult));

	//关闭链接
	m_pITCPNetworkEngine->ShutDownSocket(dwContextID);
	
	return true;
}

//修改结果
bool CAttemperEngineSink::OnDBRModifyIndividual(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//数据验证
	ASSERT(wDataSize == sizeof(DBR_GP_ModifyIndividualResult));
	if (wDataSize != sizeof(DBR_GP_ModifyIndividualResult)) return false;

	//类型转换
	DBR_GP_ModifyIndividualResult *pModifyIndividualResul = (DBR_GP_ModifyIndividualResult*)pData;
	pModifyIndividualResul->szDescribeMsg[sizeof(pModifyIndividualResul->szDescribeMsg)-1] = 0;
	pModifyIndividualResul->szAccount[sizeof(pModifyIndividualResul->szAccount)-1] = 0;

	CMD_GP_ModifyIndividualResult ModifyIndividualResult;
	ZeroMemory(&ModifyIndividualResult, sizeof(ModifyIndividualResult));
	ModifyIndividualResult.nCode = pModifyIndividualResul->iCode;
	lstrcpyn(ModifyIndividualResult.szDescribeMsg, pModifyIndividualResul->szDescribeMsg, sizeof(ModifyIndividualResult.szDescribeMsg));
	lstrcpyn(ModifyIndividualResult.szAccount, pModifyIndividualResul->szAccount, sizeof(ModifyIndividualResult.szAccount));

	//发送数据
	m_pITCPNetworkEngine->SendData(dwContextID, MDM_GP_USER, SUB_GP_MODIFY_INDIVIDUAL_RESULT, &ModifyIndividualResult, sizeof(ModifyIndividualResult));

	//关闭链接
//	m_pITCPNetworkEngine->ShutDownSocket(dwContextID);

	return true;
}

//修改个性签名结果
bool CAttemperEngineSink::OnDBRModifyUnderWrite(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//数据验证
	ASSERT(wDataSize == sizeof(DBR_GP_ModifyUnderWriteResult));
	if (wDataSize != sizeof(DBR_GP_ModifyUnderWriteResult)) return false;

	//类型转换
	DBR_GP_ModifyUnderWriteResult *pModifyUnderWriteResul = (DBR_GP_ModifyUnderWriteResult*)pData;
	pModifyUnderWriteResul->szDescribeMsg[sizeof(pModifyUnderWriteResul->szDescribeMsg)-1] = 0;
	pModifyUnderWriteResul->szAccount[sizeof(pModifyUnderWriteResul->szAccount)-1] = 0;

	CMD_GP_ModifyUnderWriteResult ModifyUnderWriteResult;
	ZeroMemory(&ModifyUnderWriteResult, sizeof(ModifyUnderWriteResult));
	ModifyUnderWriteResult.nCode = pModifyUnderWriteResul->iCode;
	lstrcpyn(ModifyUnderWriteResult.szDescribeMsg, pModifyUnderWriteResul->szDescribeMsg, sizeof(ModifyUnderWriteResult.szDescribeMsg));
	lstrcpyn(ModifyUnderWriteResult.szAccount, pModifyUnderWriteResul->szAccount, sizeof(ModifyUnderWriteResult.szAccount));

	//发送数据
	m_pITCPNetworkEngine->SendData(dwContextID, MDM_GP_USER, SUB_GP_MODIFY_UNDERWRITE_RESULT, &ModifyUnderWriteResult, sizeof(ModifyUnderWriteResult));

	//关闭链接
//	m_pITCPNetworkEngine->ShutDownSocket(dwContextID);

	return true;
}

//修改手机号结果
bool CAttemperEngineSink::OnDBRModifyPhone(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//数据验证
	ASSERT(wDataSize == sizeof(DBR_GP_ModifyPhoneResult));
	if (wDataSize != sizeof(DBR_GP_ModifyPhoneResult)) return false;

	//类型转换
	DBR_GP_ModifyPhoneResult *pModifyPhoneResul = (DBR_GP_ModifyPhoneResult*)pData;
	pModifyPhoneResul->szDescribeMsg[sizeof(pModifyPhoneResul->szDescribeMsg)-1] = 0;
	pModifyPhoneResul->szAccount[sizeof(pModifyPhoneResul->szAccount)-1] = 0;
	pModifyPhoneResul->szPhone[sizeof(pModifyPhoneResul->szPhone)-1] = 0;

	CMD_GP_ModifyPhoneResult ModifyPhoneResult;
	ZeroMemory(&ModifyPhoneResult, sizeof(ModifyPhoneResult));
	ModifyPhoneResult.nCode = pModifyPhoneResul->iCode;
	lstrcpyn(ModifyPhoneResult.szDescribeMsg, pModifyPhoneResul->szDescribeMsg, sizeof(ModifyPhoneResult.szDescribeMsg));
	lstrcpyn(ModifyPhoneResult.szAccount, pModifyPhoneResul->szAccount, sizeof(ModifyPhoneResult.szAccount));
	lstrcpyn(ModifyPhoneResult.szPhone, pModifyPhoneResul->szPhone, sizeof(ModifyPhoneResult.szPhone));

	//发送数据
	m_pITCPNetworkEngine->SendData(dwContextID, MDM_GP_USER, SUB_GP_MODIFY_PHONE_RESULT, &ModifyPhoneResult, sizeof(ModifyPhoneResult));

	//关闭链接
//	m_pITCPNetworkEngine->ShutDownSocket(dwContextID);

	return true;
}

//校验名称
bool CAttemperEngineSink::OnDBRCheckID(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//数据验证
	ASSERT(wDataSize == sizeof(DBR_GP_CommonResult));
	if (wDataSize != sizeof(DBR_GP_CommonResult)) return false;

	//类型转换
	DBR_GP_CommonResult *pResul = (DBR_GP_CommonResult*)pData;
	pResul->szDescribeMsg[sizeof(pResul->szDescribeMsg)-1] = 0;
	pResul->szAccount[sizeof(pResul->szAccount)-1] = 0;

	CMD_GP_CheckIDRep rep;
	ZeroMemory(&rep, sizeof(rep));
	rep.nCode = pResul->iCode;
	lstrcpyn(rep.szDescribeMsg, pResul->szDescribeMsg, sizeof(rep.szDescribeMsg));
	lstrcpyn(rep.szAccount, pResul->szAccount, sizeof(rep.szAccount));

	//发送数据
	m_pITCPNetworkEngine->SendData(dwContextID, MDM_GP_USER, SUB_GP_CHECK_ID_REP, &rep, sizeof(CMD_GP_CheckIDRep));

	return true;
}

//校验昵称
bool CAttemperEngineSink::OnDBRCheckNickName(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//数据验证
	ASSERT(wDataSize == sizeof(DBR_GP_CommonResult));
	if (wDataSize != sizeof(DBR_GP_CommonResult)) return false;

	//类型转换
	DBR_GP_CommonResult *pResul = (DBR_GP_CommonResult*)pData;
	pResul->szDescribeMsg[sizeof(pResul->szDescribeMsg)-1] = 0;
	pResul->szAccount[sizeof(pResul->szAccount)-1] = 0;

	CMD_GP_CheckNickNameRep rep;
	ZeroMemory(&rep, sizeof(rep));
	rep.nCode = pResul->iCode;
	lstrcpyn(rep.szDescribeMsg, pResul->szDescribeMsg, sizeof(rep.szDescribeMsg));
	lstrcpyn(rep.szNickName, pResul->szAccount, sizeof(rep.szNickName));

	//发送数据
	m_pITCPNetworkEngine->SendData(dwContextID, MDM_GP_USER, SUB_GP_CHECK_NICKNAME_REP, &rep, sizeof(CMD_GP_CheckNickNameRep));
	
	return true;
}

//注册
bool CAttemperEngineSink::OnDBRRegister(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//数据验证
	ASSERT(wDataSize == sizeof(DBR_GP_CommonResult));
	if (wDataSize != sizeof(DBR_GP_CommonResult)) return false;

	//类型转换
	DBR_GP_CommonResult *pResul = (DBR_GP_CommonResult*)pData;
	pResul->szDescribeMsg[sizeof(pResul->szDescribeMsg)-1] = 0;
	pResul->szAccount[sizeof(pResul->szAccount)-1] = 0;

	CMD_GP_RegisterRep rep;
	ZeroMemory(&rep, sizeof(rep));
	rep.nCode = pResul->iCode;
	lstrcpyn(rep.szDescribeMsg, pResul->szDescribeMsg, sizeof(rep.szDescribeMsg));
	lstrcpyn(rep.szAccount, pResul->szAccount, sizeof(rep.szAccount));

	//发送数据
	m_pITCPNetworkEngine->SendData(dwContextID, MDM_GP_USER, SUB_GP_REGISTER_REP, &rep, sizeof(CMD_GP_RegisterRep));
	
	return true;
}

//升级
bool CAttemperEngineSink::OnDBRIDUpdate(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//数据验证
	ASSERT(wDataSize == sizeof(DBR_GP_CommonResult));
	if (wDataSize != sizeof(DBR_GP_CommonResult)) return false;

	//类型转换
	DBR_GP_CommonResult *pResul = (DBR_GP_CommonResult*)pData;
	pResul->szDescribeMsg[sizeof(pResul->szDescribeMsg)-1] = 0;
	pResul->szAccount[sizeof(pResul->szAccount)-1] = 0;

	CMD_GP_IDUpdateRep rep;
	ZeroMemory(&rep, sizeof(rep));
	rep.nCode = pResul->iCode;
	lstrcpyn(rep.szDescribeMsg, pResul->szDescribeMsg, sizeof(rep.szDescribeMsg));
	lstrcpyn(rep.szAccount, pResul->szAccount, sizeof(rep.szAccount));

	//发送数据
	//m_pITCPNetworkEngine->SendData(dwContextID, MDM_GP_USER, SUB_GP_ID_UPDATE_REP, &rep, sizeof(CMD_GP_IDUpdateRep));
	SEND_USER_DATA(rep);
	
	return true;
}

//重置密码
bool CAttemperEngineSink::OnDBRResetPwd(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//数据验证
	ASSERT(wDataSize == sizeof(DBR_GP_CommonResult));
	if (wDataSize != sizeof(DBR_GP_CommonResult)) return false;

	//类型转换
	DBR_GP_CommonResult *pResul = (DBR_GP_CommonResult*)pData;
	pResul->szDescribeMsg[sizeof(pResul->szDescribeMsg)-1] = 0;
	pResul->szAccount[sizeof(pResul->szAccount)-1] = 0;

	CMD_GP_ResetPwdRep rep;
	ZeroMemory(&rep, sizeof(rep));
	rep.nCode = pResul->iCode;
	lstrcpyn(rep.szDescribeMsg, pResul->szDescribeMsg, sizeof(rep.szDescribeMsg));
	lstrcpyn(rep.szAccount, pResul->szAccount, sizeof(rep.szAccount));

	//发送数据
	m_pITCPNetworkEngine->SendData(dwContextID, MDM_GP_USER, SUB_GP_RESET_PWD_REP, &rep, sizeof(CMD_GP_ResetPwdRep));
	
	return true;
}

//查询用户信息
bool CAttemperEngineSink::OnDBRUserInfo(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//数据验证
	ASSERT(wDataSize == sizeof(DBR_GP_UserInfo_Result));
	if (wDataSize != sizeof(DBR_GP_UserInfo_Result)) return false;

	//类型转换
	DBR_GP_UserInfo_Result *pResul = (DBR_GP_UserInfo_Result*)pData;
	pResul->szDescribeMsg[sizeof(pResul->szDescribeMsg)-1] = 0;
	pResul->szAccount[sizeof(pResul->szAccount)-1] = 0;

	CMD_GP_UserInfoRep rep;
	ZeroMemory(&rep, sizeof(rep));
	rep.nCode = pResul->iCode;
	rep.nGender = pResul->nGender;
	lstrcpyn(rep.szNickname, pResul->szNickName, sizeof(rep.szNickname));
	lstrcpyn(rep.szAccount, pResul->szAccount, sizeof(rep.szAccount));
	lstrcpyn(rep.szPhone, pResul->szPhone, sizeof(rep.szPhone));

	//发送数据
	m_pITCPNetworkEngine->SendData(dwContextID, MDM_GP_USER, SUB_GP_USER_INFO_REP, &rep, sizeof(CMD_GP_UserInfoRep));
	
	return true;
}

//查询用户是否可以领取推广奖励
bool CAttemperEngineSink::OnDBRUserCanSpread(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//数据验证
	ASSERT(wDataSize == sizeof(DBR_GP_CommonResult));
	if (wDataSize != sizeof(DBR_GP_CommonResult)) return false;

	//类型转换
	DBR_GP_CommonResult *pResul = (DBR_GP_CommonResult*)pData;
	pResul->szDescribeMsg[sizeof(pResul->szDescribeMsg)-1] = 0;

	CMD_GP_UserCanSpreadResult rep;
	ZeroMemory(&rep, sizeof(rep));
	rep.nCode = pResul->iCode;
	rep.dwUserID = pResul->dwUserID;
	lstrcpyn(rep.szDescribeMsg, pResul->szDescribeMsg, sizeof(rep.szDescribeMsg));

	//发送数据
	m_pITCPNetworkEngine->SendData(dwContextID, MDM_GP_USER, SUB_GP_USER_CAN_SPREAD_REP, &rep, sizeof(CMD_GP_UserCanSpreadResult));
	
	return true;
}

//用户领取推广奖励
bool CAttemperEngineSink::OnDBRUserGetSpread(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//数据验证
	ASSERT(wDataSize == sizeof(DBR_GP_UserGetSpreadResult));
	if (wDataSize != sizeof(DBR_GP_UserGetSpreadResult)) return false;

	//类型转换
	DBR_GP_UserGetSpreadResult *pResul = (DBR_GP_UserGetSpreadResult*)pData;
	pResul->szDescribeMsg[sizeof(pResul->szDescribeMsg)-1] = 0;

	CMD_GP_UserGetSpreadResult rep;
	ZeroMemory(&rep, sizeof(rep));
	rep.nCode = pResul->iCode;
	lstrcpyn(rep.szDescribeMsg, pResul->szDescribeMsg, sizeof(rep.szDescribeMsg));
	rep.dwAmount = pResul->dwAmount;
	rep.dwSpreaderID = pResul->dwSpreaderID;
	rep.dwType = pResul->dwType;
	rep.dwUserID = pResul->dwUserID;

	//发送数据
	m_pITCPNetworkEngine->SendData(dwContextID, MDM_GP_USER, SUB_GP_USER_GET_SPREAD_REP, &rep, sizeof(CMD_GP_UserInfoRep));
	
	return true;
}

//查询推广员信息
bool CAttemperEngineSink::OnDBRUserSpreaderInfo(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//数据验证
	ASSERT(wDataSize == sizeof(DBR_GP_UserSpreaderInfoResult));
	if (wDataSize != sizeof(DBR_GP_UserSpreaderInfoResult)) return false;

	//类型转换
	DBR_GP_UserSpreaderInfoResult *pResul = (DBR_GP_UserSpreaderInfoResult*)pData;
	pResul->szDescribeMsg[sizeof(pResul->szDescribeMsg)-1] = 0;

	CMD_GP_UserGetSpreaderInfoResult rep;
	ZeroMemory(&rep, sizeof(rep));
	rep.nCode = pResul->iCode;
	lstrcpyn(rep.szDescribeMsg, pResul->szDescribeMsg, sizeof(rep.szDescribeMsg));
	rep.dwSpreaderID = pResul->dwSpreaderID;
	rep.dwAmount = pResul->dwAmount;
	rep.dwUsers = pResul->dwUsers;
	rep.nCount = pResul->dwNum;
	for (int i=0; i<rep.nCount; i++)
	{
		lstrcpyn(rep.szNickName[i], pResul->vecNickNames[i], sizeof(rep.szNickName[i]));
		rep.nAward[i] = pResul->iAward[i];
	}

	//发送数据
	m_pITCPNetworkEngine->SendData(dwContextID, MDM_GP_USER, SUB_GP_USER_SPREADER_INFO_REP, &rep, sizeof(CMD_GP_UserGetSpreaderInfoResult));
	
	return true;
}

//弹窗公告信息
bool CAttemperEngineSink::OnDBRUserBbsPop(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//数据验证
	ASSERT(wDataSize == sizeof(DBR_GP_UserGetBbsPopResult));
	if (wDataSize != sizeof(DBR_GP_UserGetBbsPopResult)) return false;

	//类型转换
	DBR_GP_UserGetBbsPopResult *pResul = (DBR_GP_UserGetBbsPopResult*)pData;
	pResul->szDetails[sizeof(pResul->szDetails)-1] = 0;

	CMD_GP_UserBbsPopResult rep;
	ZeroMemory(&rep, sizeof(rep));
	if (pResul->iCode == 0)
	{
		lstrcpyn(rep.szBbsPop, pResul->szDetails, sizeof(rep.szBbsPop));
		rep.nAction = pResul->iAction;
		rep.nBbsScroll = pResul->iScrollCount;
		std::string strID;
		for (int i=0; i<pResul->iListCount; i++)
		{
			char buf[16];
			memset(buf, 0, 16);
			sprintf(buf, "%d|", pResul->dwListID[i]);

			strID += buf;
		}
		lstrcpyn(rep.szBbsListIDs, strID.c_str(), sizeof(rep.szBbsListIDs));
	}

	//发送数据
	m_pITCPNetworkEngine->SendData(dwContextID, MDM_GP_USER, SUB_GP_USER_BBS_POP_REP, &rep, sizeof(CMD_GP_UserBbsPopResult));
	
	return true;
}

//滚动公告信息
bool CAttemperEngineSink::OnDBRUserBbsScroll(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//数据验证
	ASSERT(wDataSize == sizeof(DBR_GP_UserGetBbsScrollResult));
	if (wDataSize != sizeof(DBR_GP_UserGetBbsScrollResult)) return false;

	//类型转换
	DBR_GP_UserGetBbsScrollResult *pResul = (DBR_GP_UserGetBbsScrollResult*)pData;
	for (int i=0; i<sizeof(pResul->szDetails)/sizeof(pResul->szDetails[0]); i++)
	{
		pResul->szDetails[i][sizeof(pResul->szDetails[i])-1] = 0;
	}

	CMD_GP_UserBbsScrollResult rep;
	ZeroMemory(&rep, sizeof(rep));
	if (pResul->iCode == 0)
	{
		for (int i=0; i<pResul->iCount; i++)
		{
			lstrcpyn(rep.szBbsScroll[i], pResul->szDetails[i], sizeof(rep.szBbsScroll[i]));
		}
		rep.nBbsScroll = pResul->iCount;
	}

	//发送数据
	m_pITCPNetworkEngine->SendData(dwContextID, MDM_GP_USER, SUB_GP_USER_BBS_SCROLL_REP, &rep, sizeof(CMD_GP_UserBbsScrollResult));
	
	return true;
}

//列表公告信息
bool CAttemperEngineSink::OnDBRUserBbsList(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//数据验证
	ASSERT(wDataSize == sizeof(DBR_GP_UserGetBbsListResult));
	if (wDataSize != sizeof(DBR_GP_UserGetBbsListResult)) return false;

	//类型转换
	DBR_GP_UserGetBbsListResult *pResul = (DBR_GP_UserGetBbsListResult*)pData;
	for (int i=0; i<sizeof(pResul->szDetails)/sizeof(pResul->szDetails[0]); i++)
	{
		pResul->szDetails[i][sizeof(pResul->szDetails[i])-1] = 0;
		pResul->szTitle[i][sizeof(pResul->szTitle[i])-1] = 0;
		pResul->szDate[i][sizeof(pResul->szDate[i])-1] = 0;
	}

	CMD_GP_UserBbsListResult rep;
	ZeroMemory(&rep, sizeof(rep));
	if (pResul->iCode == 0)
	{
		for (int i=0; i<pResul->iCount; i++)
		{
			lstrcpyn(rep.szBbsList[i], pResul->szDetails[i], sizeof(rep.szBbsList[i]));
			lstrcpyn(rep.szTitle[i], pResul->szTitle[i], sizeof(rep.szTitle[i]));
			lstrcpyn(rep.szDate[i], pResul->szDate[i], sizeof(rep.szDate[i]));

			rep.iID[i] = pResul->iID[i];
			rep.iType[i] = pResul->iType[i];
		}

		rep.nBbsList = pResul->iCount;
	}

	//发送数据
	m_pITCPNetworkEngine->SendData(dwContextID, MDM_GP_USER, SUB_GP_USER_BBS_LIST_REP, &rep, sizeof(CMD_GP_UserBbsListResult));
	
	return true;
}

//玩家任务信息
bool CAttemperEngineSink::OnDBRUserTaskInfo(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//数据验证
	ASSERT(wDataSize == sizeof(DBR_GP_UserGetTaskInfoResult));
	if (wDataSize != sizeof(DBR_GP_UserGetTaskInfoResult)) return false;

	//类型转换
	DBR_GP_UserGetTaskInfoResult *pResul = (DBR_GP_UserGetTaskInfoResult*)pData;

	CMD_GP_UserTaskInfoResult rep;
	ZeroMemory(&rep, sizeof(rep));
	
	rep.dwUserID = pResul->dwUserID;

	if (pResul->iCode == 0)
	{
		//拼接字符串
		//任务描述。考虑到任务的多样性，所以此处定义为字符串，具体内容根据需要自定义。
		//目前定义的有：t3:1|a3:5|t4:0|a4:10|st3:1|st4:0|sa3:1|sa4:2
		//具体含义：t3表示三人拱的今天盘数
		//			t4表示四人拱的今天盘数
		//			a3表示三人拱的总盘数
		//			a4表示四人拱的总盘数
		//			st3表示三人拱的每日任务的已领奖励阶段，依次为0,1,2,3
		//			st4表示四人拱的每日任务的已领奖励阶段，依次为0,1,2,3
		//			sa3表示三人拱的系统任务的已领奖励阶段，依次为0,1,2,3....13
		//			sa4表示四人拱的系统任务的已领奖励阶段，依次为0,1,2,3....13
		sprintf(rep.szRule, "t3:%d|a3:%d|t4:%d|a4:%d|st3:%d|st4:%d|sa3:%d|sa4:%d",
			pResul->iWinToday3+pResul->iLostToday3, pResul->iWinTotal3,
			pResul->iWinToday4+pResul->iLostToday4, pResul->iWinTotal4,
			pResul->iAwardToday3, pResul->iAwardToday4,
			pResul->iAwardTotal3, pResul->iAwardTotal4);
	}

	//发送数据
	m_pITCPNetworkEngine->SendData(dwContextID, MDM_GP_USER, SUB_GP_USER_TASK_INFO_REP, &rep, sizeof(CMD_GP_UserTaskInfoResult));
	
	return true;
}

//玩家排行榜
bool CAttemperEngineSink::OnDBRUserGetRanking(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//效验参数
	ASSERT(wDataSize==sizeof(DBR_GP_UserGetRankingResult));
	if (wDataSize!=sizeof(DBR_GP_UserGetRankingResult)) return false;

	//判断在线
	ASSERT(LOWORD(dwContextID)<m_pInitParamter->m_wMaxConnect);
	if ((m_pBindParameter+LOWORD(dwContextID))->dwSocketID!=dwContextID) return true;

	//变量定义
	char cbBuffer[SOCKET_PACKET];
	DBR_GP_UserGetRankingResult * pResult=(DBR_GP_UserGetRankingResult *)pData;

	int iTotal = 0;

	for (int i=0; i<pResult->vecRecords.size(); i++)
	{
		CMD_GP_UserRankingRecord record;

		//构造数据
		record.dwUserID = pResult->vecRecords[i].dwUserID;
		record.cbGender = pResult->vecRecords[i].cbGender;
		record.dwGoldScore = pResult->vecRecords[i].lGoldScore;
		record.wIndex = pResult->vecRecords[i].nIndex;
		memcpy(record.szNickName, pResult->vecRecords[i].szNickName, sizeof(pResult->vecRecords[i].szNickName));

		memset(cbBuffer, 0, SOCKET_PACKET);
		bostream bos;
		bos.attach(cbBuffer, SOCKET_PACKET);
		bos << record;

		m_pITCPNetworkEngine->SendData(dwContextID,MDM_GP_USER,SUB_GP_USER_RANKING_RECORD,cbBuffer,bos.length());
	}

	//结束
	CMD_GP_UserRankingRecordFinish recordFinish;
	recordFinish.dwTotalofCurPage = pResult->vecRecords.size();
	recordFinish.dwMyRanking = pResult->nMyIndex;
	recordFinish.dwTotal = pResult->dwTotal;
	recordFinish.dwUserID = pResult->dwUserID;
	recordFinish.nCode = pResult->iCode;
	memcpy(recordFinish.szDate, pResult->szDate, sizeof(pResult->szDate));

	memset(cbBuffer, 0, SOCKET_PACKET);
	bostream bos;
	bos.attach(cbBuffer, SOCKET_PACKET);
	bos << recordFinish;

	m_pITCPNetworkEngine->SendData(dwContextID,MDM_GP_USER,SUB_GP_USER_RANKING_RECORD_FINISH,cbBuffer,bos.length());

	return true;
}

//玩家排行榜
bool CAttemperEngineSink::OnDBRUserGetRanking_V2(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//效验参数
	ASSERT(wDataSize==sizeof(DBR_GP_UserGetRankingResult_V2));
	if (wDataSize!=sizeof(DBR_GP_UserGetRankingResult_V2)) return false;

	//判断在线
	ASSERT(LOWORD(dwContextID)<m_pInitParamter->m_wMaxConnect);
	if ((m_pBindParameter+LOWORD(dwContextID))->dwSocketID!=dwContextID) return true;

	//变量定义
	char cbBuffer[SOCKET_PACKET];
	DBR_GP_UserGetRankingResult_V2 * pResult=(DBR_GP_UserGetRankingResult_V2 *)pData;

	int iTotal = 0;

	for (int i=0; i<pResult->vecRecords.size(); i++)
	{
		CMD_GP_UserRankingRecord_V2 record;

		//构造数据
		record.dwUserID = pResult->vecRecords[i].dwUserID;
		record.cbGender = pResult->vecRecords[i].cbGender;
		record.dwGoldScore = pResult->vecRecords[i].lGoldScore;
		record.wIndex = pResult->vecRecords[i].nIndex;
		memcpy(record.szNickName, pResult->vecRecords[i].szNickName, sizeof(pResult->vecRecords[i].szNickName));
		memcpy(record.szDescribeInfo, pResult->vecRecords[i].szDescribe, sizeof(pResult->vecRecords[i].szDescribe));

		memset(cbBuffer, 0, SOCKET_PACKET);
		bostream bos;
		bos.attach(cbBuffer, SOCKET_PACKET);
		bos << record;

		m_pITCPNetworkEngine->SendData(dwContextID,MDM_GP_USER,SUB_GP_USER_RANKING_RECORD_V2,cbBuffer,bos.length());
	}

	//结束
	CMD_GP_UserRankingRecordFinish recordFinish;
	recordFinish.dwTotalofCurPage = pResult->vecRecords.size();
	recordFinish.dwMyRanking = pResult->nMyIndex;
	recordFinish.dwTotal = pResult->dwTotal;
	recordFinish.dwUserID = pResult->dwUserID;
	recordFinish.nCode = pResult->iCode;
	memcpy(recordFinish.szDate, pResult->szDate, sizeof(pResult->szDate));

	memset(cbBuffer, 0, SOCKET_PACKET);
	bostream bos;
	bos.attach(cbBuffer, SOCKET_PACKET);
	bos << recordFinish;

	m_pITCPNetworkEngine->SendData(dwContextID,MDM_GP_USER,SUB_GP_USER_RANKING_RECORD_FINISH,cbBuffer,bos.length());

	return true;
}

//玩家是否入排行榜信息
bool CAttemperEngineSink::OnDBRUserGetRankingStatus(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//效验参数
	ASSERT(wDataSize==sizeof(DBR_GP_UserGetRankingStatusResult));
	if (wDataSize!=sizeof(DBR_GP_UserGetRankingStatusResult)) return false;

	//判断在线
	ASSERT(LOWORD(dwContextID)<m_pInitParamter->m_wMaxConnect);
	if ((m_pBindParameter+LOWORD(dwContextID))->dwSocketID!=dwContextID) return true;

	//变量定义
	char cbBuffer[SOCKET_PACKET];
	DBR_GP_UserGetRankingStatusResult * pResult=(DBR_GP_UserGetRankingStatusResult *)pData;

	CMD_GP_UserRepRankingStatus recordFinish;
	recordFinish.dwUserID = pResult->dwUserID;
	recordFinish.isInRanking = pResult->isInRanking;

	memset(cbBuffer, 0, SOCKET_PACKET);
	bostream bos;
	bos.attach(cbBuffer, SOCKET_PACKET);
	bos << recordFinish;

	m_pITCPNetworkEngine->SendData(dwContextID,MDM_GP_USER,SUB_GP_USER_REP_RANKINGSTATUS,cbBuffer,bos.length());

	return true;
}

//修改玩家是否入排行榜返回信息
bool CAttemperEngineSink::OnDBRUserGetRankingStatusResult(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//效验参数
	ASSERT(wDataSize==sizeof(DBR_GP_UserModifyRankingStatusResult));
	if (wDataSize!=sizeof(DBR_GP_UserModifyRankingStatusResult)) return false;

	//判断在线
	ASSERT(LOWORD(dwContextID)<m_pInitParamter->m_wMaxConnect);
	if ((m_pBindParameter+LOWORD(dwContextID))->dwSocketID!=dwContextID) return true;

	//变量定义
	char cbBuffer[SOCKET_PACKET];
	DBR_GP_UserModifyRankingStatusResult * pResult=(DBR_GP_UserModifyRankingStatusResult *)pData;

	CMD_GP_UserRepModifyRankingStatus recordFinish;
	recordFinish.dwUserID = pResult->dwUserID;
	recordFinish.nCode = pResult->iCode;

	memset(cbBuffer, 0, SOCKET_PACKET);
	bostream bos;
	bos.attach(cbBuffer, SOCKET_PACKET);
	bos << recordFinish;

	m_pITCPNetworkEngine->SendData(dwContextID,MDM_GP_USER,SUB_GP_USER_REP_MODIFY_RANKINGSTATUS,cbBuffer,bos.length());

	return true;
}

//意见反馈结果
bool CAttemperEngineSink::OnDBRUserFeedBack(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//数据验证
	ASSERT(wDataSize == sizeof(DBR_GP_UserFeedBackResult));
	if (wDataSize != sizeof(DBR_GP_UserFeedBackResult)) return false;

	//类型转换
	DBR_GP_UserFeedBackResult *pResul = (DBR_GP_UserFeedBackResult*)pData;
	pResul->szDetails[sizeof(pResul->szDetails)-1] = 0;

	CMD_GP_UserFeedBackResult rep;
	ZeroMemory(&rep, sizeof(rep));
	
	rep.nCode = pResul->iCode;

	if (pResul->iCode != 0)
	{
		memcpy(rep.szDetails, pResul->szDetails, sizeof(rep.szDetails));
	}

	//发送数据
	m_pITCPNetworkEngine->SendData(dwContextID, MDM_GP_USER, SUB_GP_USER_FEEDBACK_REP, &rep, sizeof(CMD_GP_UserFeedBackResult));
	
	return true;
}

//查询意见反馈结果
bool CAttemperEngineSink::OnDBRUserGetFeedBack(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//效验参数
	ASSERT(wDataSize==sizeof(DBR_GP_UserGetFeedBackResult));
	if (wDataSize!=sizeof(DBR_GP_UserGetFeedBackResult)) return false;

	//判断在线
	ASSERT(LOWORD(dwContextID)<m_pInitParamter->m_wMaxConnect);
	if ((m_pBindParameter+LOWORD(dwContextID))->dwSocketID!=dwContextID) return true;

	//变量定义
	char cbBuffer[SOCKET_PACKET];
	DBR_GP_UserGetFeedBackResult * pResult=(DBR_GP_UserGetFeedBackResult *)pData;

	int iTotal = 0;

	for (int i=0; i<pResult->vecRecords.size(); i++)
	{
		CMD_GP_UserFeedBackRecord record;

		//构造数据
		record.cbHavePic = pResult->vecRecords[i].cbHavePic;
		memcpy(record.szQuestion, pResult->vecRecords[i].szQuestion, sizeof(pResult->vecRecords[i].szQuestion));
		memcpy(record.szAnswer, pResult->vecRecords[i].szAnswer, sizeof(pResult->vecRecords[i].szAnswer));
		memcpy(record.szPicture, pResult->vecRecords[i].szPicture, sizeof(pResult->vecRecords[i].szPicture));
		memcpy(record.szDate, pResult->vecRecords[i].szDate, sizeof(pResult->vecRecords[i].szDate));

		memset(cbBuffer, 0, SOCKET_PACKET);
		bostream bos;
		bos.attach(cbBuffer, SOCKET_PACKET);
		bos << record;

		m_pITCPNetworkEngine->SendData(dwContextID,MDM_GP_USER,SUB_GP_USER_FEEDBACK_RECORD,cbBuffer,bos.length());
	}

	//结束
	CMD_GP_UserFeedBackRecordFinish recordFinish;
	recordFinish.dwTotalofCurPage = pResult->vecRecords.size();
	recordFinish.dwTotal = pResult->dwTotal;
	recordFinish.dwUserID = pResult->dwUserID;
	recordFinish.nCode = pResult->iCode;

	memset(cbBuffer, 0, SOCKET_PACKET);
	bostream bos;
	bos.attach(cbBuffer, SOCKET_PACKET);
	bos << recordFinish;

	m_pITCPNetworkEngine->SendData(dwContextID,MDM_GP_USER,SUB_GP_USER_FEEDBACK_RECORD_FINISH,cbBuffer,bos.length());

	return true;
}

//查询用户身份证账号信息的结果
bool CAttemperEngineSink::OnDBRUserInfoIDCard(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//效验参数
	ASSERT(wDataSize==sizeof(DBR_GP_CommonResult));
	if (wDataSize!=sizeof(DBR_GP_CommonResult)) return false;

	//判断在线
	ASSERT(LOWORD(dwContextID)<m_pInitParamter->m_wMaxConnect);
	if ((m_pBindParameter+LOWORD(dwContextID))->dwSocketID!=dwContextID) return true;

	//变量定义
	char cbBuffer[SOCKET_PACKET];
	DBR_GP_CommonResult * pResult=(DBR_GP_CommonResult *)pData;

	//结束
	CMD_GP_UserInfoIDCARDRep ret;
	ret.dwUserID = pResult->dwUserID;
	if (pResult->iCode == 0)
	{
		memcpy(ret.szIdNum, pResult->szDescribeMsg, sizeof(pResult->szDescribeMsg));
	}

	memset(cbBuffer, 0, SOCKET_PACKET);
	bostream bos;
	bos.attach(cbBuffer, SOCKET_PACKET);
	bos << ret;

	m_pITCPNetworkEngine->SendData(dwContextID,MDM_GP_USER,SUB_GP_USER_INFO_ID_CARD_REP,cbBuffer,bos.length());

	return true;
}

//请求提交身份证信息的结果
bool CAttemperEngineSink::OnDBRUserAuthentication(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//效验参数
	ASSERT(wDataSize==sizeof(DBR_GP_CommonResult));
	if (wDataSize!=sizeof(DBR_GP_CommonResult)) return false;

	//判断在线
	ASSERT(LOWORD(dwContextID)<m_pInitParamter->m_wMaxConnect);
	if ((m_pBindParameter+LOWORD(dwContextID))->dwSocketID!=dwContextID) return true;

	//变量定义
	char cbBuffer[SOCKET_PACKET];
	DBR_GP_CommonResult * pResult=(DBR_GP_CommonResult *)pData;

	//结束
	CMD_GP_UserAuthenticationRep ret;
	ret.dwUserID = pResult->dwUserID;
	ret.resultCode = pResult->iCode;

	memset(cbBuffer, 0, SOCKET_PACKET);
	bostream bos;
	bos.attach(cbBuffer, SOCKET_PACKET);
	bos << ret;

	m_pITCPNetworkEngine->SendData(dwContextID,MDM_GP_USER,SUB_GP_USER_AUTHENTICA_REP,cbBuffer,bos.length());

	return true;
}

//请求商城商品列表信息的结果
bool CAttemperEngineSink::OnDBRUserMallProductResult(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//效验参数
	ASSERT(wDataSize==sizeof(DBR_GP_UserMallProductResultFinish));
	if (wDataSize!=sizeof(DBR_GP_UserMallProductResultFinish)) return false;

	//判断在线
	ASSERT(LOWORD(dwContextID)<m_pInitParamter->m_wMaxConnect);
	if ((m_pBindParameter+LOWORD(dwContextID))->dwSocketID!=dwContextID) return true;

	//变量定义
	char cbBuffer[SOCKET_PACKET];
	DBR_GP_UserMallProductResultFinish * pExchangeProduct=(DBR_GP_UserMallProductResultFinish *)pData;

	for (int i=0; i<pExchangeProduct->vecProducts.size(); i++)
	{
		if (   pExchangeProduct->vecProducts[i].version == 1 
			|| pExchangeProduct->vecProducts[i].version == 2)
		{
			CMD_GP_UserMallProductResp_V2 exchangeProduct;
			memset(&exchangeProduct,0, sizeof(exchangeProduct));

			//构造数据
			exchangeProduct.dwVersion = pExchangeProduct->vecProducts[i].version;
			exchangeProduct.cbHotFlag = pExchangeProduct->vecProducts[i].cbHotFlag;
			exchangeProduct.productID = pExchangeProduct->vecProducts[i].productID;
			exchangeProduct.productPrice = pExchangeProduct->vecProducts[i].productPrice;
			memcpy(exchangeProduct.szProductName, pExchangeProduct->vecProducts[i].szProductName, sizeof(pExchangeProduct->vecProducts[i].szProductName));
			memcpy(exchangeProduct.szProductNameAdd, pExchangeProduct->vecProducts[i].szProductNameAdd, sizeof(pExchangeProduct->vecProducts[i].szProductNameAdd));
			memcpy(exchangeProduct.szProductImage, pExchangeProduct->vecProducts[i].szProductImage, sizeof(pExchangeProduct->vecProducts[i].szProductImage));

			memset(cbBuffer, 0, SOCKET_PACKET);
			bostream bos;
			bos.attach(cbBuffer, SOCKET_PACKET);
			bos << exchangeProduct;

			m_pITCPNetworkEngine->SendData(dwContextID,MDM_GP_USER,SUB_GP_USER_MALL_PRODUCT_RESP_V2,cbBuffer,bos.length());
		}
		else
		{
			CMD_GP_UserMallProductResp exchangeProduct;
			memset(&exchangeProduct,0, sizeof(exchangeProduct));

			//构造数据
			exchangeProduct.productID = pExchangeProduct->vecProducts[i].productID;
			exchangeProduct.productPrice = pExchangeProduct->vecProducts[i].productPrice;
			memcpy(exchangeProduct.szProductName, pExchangeProduct->vecProducts[i].szProductName, sizeof(pExchangeProduct->vecProducts[i].szProductName));
			memcpy(exchangeProduct.szProductImage, pExchangeProduct->vecProducts[i].szProductImage, sizeof(pExchangeProduct->vecProducts[i].szProductImage));

			memset(cbBuffer, 0, SOCKET_PACKET);
			bostream bos;
			bos.attach(cbBuffer, SOCKET_PACKET);
			bos << exchangeProduct;

			m_pITCPNetworkEngine->SendData(dwContextID,MDM_GP_USER,SUB_GP_USER_MALL_PRODUCT_RESP,cbBuffer,bos.length());
		}
	}

	//结束
	CMD_GP_UserMallProductRespFinish exchangeFinish;
	exchangeFinish.dwTotal = pExchangeProduct->vecProducts.size();

	memset(cbBuffer, 0, SOCKET_PACKET);
	bostream bos;
	bos.attach(cbBuffer, SOCKET_PACKET);
	bos << exchangeFinish;

	m_pITCPNetworkEngine->SendData(dwContextID,MDM_GP_USER,SUB_GP_USER_MALL_PRODUCT_RESP_FINISH,cbBuffer,bos.length());

	return true;
}

// 用户请求删除订单
bool CAttemperEngineSink::OnDBRUserCancelOrderResult(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//效验参数
	ASSERT(wDataSize==sizeof(DBR_Cancel_Order_Resut));
	if (wDataSize!=sizeof(DBR_Cancel_Order_Resut)) return false;

	//判断在线
	ASSERT(LOWORD(dwContextID)<m_pInitParamter->m_wMaxConnect);
	if ((m_pBindParameter+LOWORD(dwContextID))->dwSocketID!=dwContextID) return true;

	//变量定义
	char cbBuffer[SOCKET_PACKET];
	DBR_Cancel_Order_Resut * pRest=(DBR_Cancel_Order_Resut *)pData;

	CMD_GP_UserCancelOrderResp result;
	result.nResult = pRest->iCode;

	bostream bos;
	bos.attach(cbBuffer, SOCKET_PACKET);
	bos << result;

	m_pITCPNetworkEngine->SendData(dwContextID,MDM_GP_USER,SUB_GP_USER_CANCEL_ORDER_REP,cbBuffer,bos.length());

	return true;
}

//用户购买结果
bool CAttemperEngineSink::OnDBRUserMallBuyResult(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//效验参数
	ASSERT(wDataSize==sizeof(DBR_GP_UserMallBuyResult));
	if (wDataSize!=sizeof(DBR_GP_UserMallBuyResult)) return false;

	DBR_GP_UserMallBuyResult * pResp = (DBR_GP_UserMallBuyResult*)pData;

	//根据用户id获取用户socketID
	DWORD wSocketID;
	std::map<unsigned int, DWORD>::iterator iter = m_mapRechargeUserSocketID.find(pResp->dwUserID);
	if (iter != m_mapRechargeUserSocketID.end())
		wSocketID = iter->second;
	else
		return false;

	//判断在线
	ASSERT(LOWORD(wSocketID)<m_pInitParamter->m_wMaxConnect);
	if ((m_pBindParameter+LOWORD(wSocketID))->dwSocketID!=wSocketID) return true;

	char cbBuffer[SOCKET_PACKET];
	CMD_GP_UserMallBuyResp result;
	result.nResult = pResp->iCode;
	result.productID = pResp->productID;
	result.dwGoldScore = pResp->dwGoldScore;

	bostream bos;
	bos.attach(cbBuffer, SOCKET_PACKET);
	bos << result;

	m_pITCPNetworkEngine->SendData(wSocketID, MDM_GP_USER, SUB_GP_USER_MALL_BUY_RESP, cbBuffer, bos.length());

	return true;
}

//用户更新结果
bool CAttemperEngineSink::OnDBRUserMallUpdateResult(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//不用通知客户端
	return true;
}

//活动列表结果
bool CAttemperEngineSink::OnDBRUserActivityListResult(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//数据验证
	ASSERT(wDataSize == sizeof(DBR_GP_UserActivityListResult));
	if (wDataSize != sizeof(DBR_GP_UserActivityListResult)) return false;

	//类型转换
	DBR_GP_UserActivityListResult *pResul = (DBR_GP_UserActivityListResult*)pData;
	for (int i=0; i<sizeof(pResul->szTitle)/sizeof(pResul->szTitle[0]); i++)
	{
		pResul->szDetails[i][sizeof(pResul->szDetails[i])-1] = 0;
		pResul->szTitle[i][sizeof(pResul->szTitle[i])-1] = 0;
		pResul->szLinkUrl[i][sizeof(pResul->szLinkUrl[i])-1] = 0;
		pResul->szIconUrl[i][sizeof(pResul->szIconUrl[i])-1] = 0;
	}

	CMD_GP_UserActivityList rep;
	ZeroMemory(&rep, sizeof(rep));
	rep.wCode = pResul->iCode;
	if (pResul->iCode == 0)
	{
		for (int i=0; i<pResul->iCount; i++)
		{
			lstrcpyn(rep.activity[i].szText, pResul->szDetails[i], sizeof(rep.activity[i].szText));
			lstrcpyn(rep.activity[i].szTitle, pResul->szTitle[i], sizeof(rep.activity[i].szTitle));
			lstrcpyn(rep.activity[i].szIconUrl, pResul->szIconUrl[i], sizeof(rep.activity[i].szIconUrl));
			lstrcpyn(rep.activity[i].szLinkUrl, pResul->szLinkUrl[i], sizeof(rep.activity[i].szLinkUrl));

			rep.activity[i].dwActivityID = pResul->iID[i];
			rep.activity[i].wType = pResul->iType[i];
			rep.activity[i].wStatus = pResul->iStatus[i];
		}

		rep.wCount = pResul->iCount;
	}
	
	char cbBuffer[SOCKET_PACKET];
	memset(cbBuffer, 0, SOCKET_PACKET);
	bostream bos;
	bos.attach(cbBuffer, SOCKET_PACKET);
	bos << rep;

	//发送数据
	m_pITCPNetworkEngine->SendData(dwContextID, MDM_GP_USER, SUB_GP_USER_ACTIVITY_LIST_REP, cbBuffer,bos.length());
	
	return true;
}
//活动结果
bool CAttemperEngineSink::OnDBRUserActivityResult(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//数据验证
	ASSERT(wDataSize == sizeof(DBR_GP_UserActivityResult));
	if (wDataSize != sizeof(DBR_GP_UserActivityResult)) return false;

	//类型转换
	DBR_GP_UserActivityResult *pResul = (DBR_GP_UserActivityResult*)pData;
	for (int i=0; i<sizeof(pResul->szText)/sizeof(pResul->szText[0]); i++)
	{
		pResul->szText[i][sizeof(pResul->szText[i])-1] = 0;
	}
	pResul->szParam[sizeof(pResul->szParam)-1] = 0;
	pResul->szResourceUrl[sizeof(pResul->szResourceUrl)-1] = 0;

	CMD_GP_UserActivity rep;
	ZeroMemory(&rep, sizeof(rep));
	rep.dwActivityID = pResul->dwActivityID;
	rep.dwUserID = pResul->dwUserID;
	rep.wCode = pResul->iCode;
	if (pResul->iCode == 0)
	{
		lstrcpyn(rep.szResourceUrl, pResul->szResourceUrl, sizeof(rep.szResourceUrl));
		lstrcpyn(rep.szParam, pResul->szParam, sizeof(rep.szParam));

		rep.wCount = pResul->wCount;
		for (int i=0; i<pResul->wCount; i++)
		{
			lstrcpyn(rep.szText[i], pResul->szText[i], sizeof(rep.szText[i]));
		}
	}
	
	char cbBuffer[SOCKET_PACKET];
	memset(cbBuffer, 0, SOCKET_PACKET);
	bostream bos;
	bos.attach(cbBuffer, SOCKET_PACKET);
	bos << rep;

	//发送数据
	m_pITCPNetworkEngine->SendData(dwContextID, MDM_GP_USER, SUB_GP_USER_ACTIVITY_REP, cbBuffer,bos.length());
	
	return true;
}
//抽奖结果
bool CAttemperEngineSink::OnDBRUserActivityLuckyResult(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//数据验证
	ASSERT(wDataSize == sizeof(DBR_GP_UserActivityLuckyResult));
	if (wDataSize != sizeof(DBR_GP_UserActivityLuckyResult)) return false;

	//类型转换
	DBR_GP_UserActivityLuckyResult *pResul = (DBR_GP_UserActivityLuckyResult*)pData;
	pResul->szImageUrl[sizeof(pResul->szImageUrl)-1] = 0;

	CMD_GP_UserActivityLucky rep;
	ZeroMemory(&rep, sizeof(rep));
	rep.dwActivityID = pResul->dwActivityID;
	rep.dwUserID = pResul->dwUserID;
	rep.wCode = pResul->iCode;
	if (pResul->iCode == 0)
	{
		lstrcpyn(rep.szImageUrl, pResul->szImageUrl, sizeof(rep.szImageUrl));
		lstrcpyn(rep.szPrizeName, pResul->szPrizeName, sizeof(rep.szPrizeName));

		rep.dwPrice = pResul->dwPrice;
		rep.dwPrizeID = pResul->dwPrizeID;
	}
	
	char cbBuffer[SOCKET_PACKET];
	memset(cbBuffer, 0, SOCKET_PACKET);
	bostream bos;
	bos.attach(cbBuffer, SOCKET_PACKET);
	bos << rep;

	//发送数据
	m_pITCPNetworkEngine->SendData(dwContextID, MDM_GP_USER, SUB_GP_USER_ACTIVITY_LUCKY_REP, cbBuffer,bos.length());
	
	return true;
}
//抽奖记录
bool CAttemperEngineSink::OnDBRUserActivityRecordResult(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//效验参数
	ASSERT(wDataSize==sizeof(DBR_GP_UserActivityRecordResult));
	if (wDataSize!=sizeof(DBR_GP_UserActivityRecordResult)) return false;

	//判断在线
	ASSERT(LOWORD(dwContextID)<m_pInitParamter->m_wMaxConnect);
	if ((m_pBindParameter+LOWORD(dwContextID))->dwSocketID!=dwContextID) return true;

	//变量定义
	char cbBuffer[SOCKET_PACKET];
	DBR_GP_UserActivityRecordResult * pActivityRecord=(DBR_GP_UserActivityRecordResult *)pData;

	int iTotal = 0;

	for (int i=0; i<pActivityRecord->vecRecords.size(); i++)
	{
		CMD_GP_UserActivityRecord activityRecord;

		activityRecord.dwActivityID = pActivityRecord->dwActivityID;
		activityRecord.dwUserID = pActivityRecord->dwUserID;

		//构造数据
		activityRecord.dwPrizeID = pActivityRecord->vecRecords[i].dwAwardID;
		activityRecord.dwID = pActivityRecord->vecRecords[i].dwID;
		activityRecord.dwTotal = pActivityRecord->vecRecords[i].dwTotal;
//		exchangeRecord.dwExchangeDate = pExchangeRecord->vecRecords[i].dwDate;
		memcpy(activityRecord.szPrizename, pActivityRecord->vecRecords[i].szAwardName, sizeof(pActivityRecord->vecRecords[i].szAwardName));
		memcpy(activityRecord.szStatus, pActivityRecord->vecRecords[i].szStatus, sizeof(pActivityRecord->vecRecords[i].szStatus));
		memcpy(activityRecord.szExchangeDate, pActivityRecord->vecRecords[i].szDate, sizeof(pActivityRecord->vecRecords[i].szDate));

		memset(cbBuffer, 0, SOCKET_PACKET);
		bostream bos;
		bos.attach(cbBuffer, SOCKET_PACKET);
		bos << activityRecord;

		iTotal = pActivityRecord->vecRecords[i].dwTotal;

		m_pITCPNetworkEngine->SendData(dwContextID,MDM_GP_USER,SUB_GP_USER_ACTIVITY_RECORD,cbBuffer,bos.length());
	}

	//结束
	CMD_GP_UserActivityRecordFinish activityFinish;
	activityFinish.dwTotal = pActivityRecord->vecRecords.size();
	activityFinish.dwActivityID = pActivityRecord->dwActivityID;
	activityFinish.dwUserID = pActivityRecord->dwUserID;

	memset(cbBuffer, 0, SOCKET_PACKET);
	bostream bos;
	bos.attach(cbBuffer, SOCKET_PACKET);
	bos << activityFinish;

	m_pITCPNetworkEngine->SendData(dwContextID,MDM_GP_USER,SUB_GP_USER_ACTIVITY_RECORD_FINISH,cbBuffer,bos.length());

	return true;
}

//发喇叭
bool CAttemperEngineSink::OnDBRUserSpeakerSendResult(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//效验参数
	ASSERT(wDataSize==sizeof(DBR_GP_UserSpeakerSendResult));
	if (wDataSize!=sizeof(DBR_GP_UserSpeakerSendResult)) return false;
	
	DBR_GP_UserSpeakerSendResult * pDBResult=(DBR_GP_UserSpeakerSendResult *)pData;

	//如果是用户喇叭,要给用户反馈信息
	if (pDBResult->iType == 1)
	{
		//判断在线
		ASSERT(LOWORD(dwContextID)<m_pInitParamter->m_wMaxConnect);
		if ((m_pBindParameter+LOWORD(dwContextID))->dwSocketID == dwContextID)
		{
			//回复消息给客户端
			CMD_GP_UserSendLabaRep rep;
			rep.nCode = pDBResult->iCode;
			rep.dwUserID = pDBResult->dwUserID;
			rep.dwCurScore = pDBResult->dwScore;
			
			char cbBuffer[SOCKET_PACKET];
			memset(cbBuffer, 0, SOCKET_PACKET);
			bostream bos;
			bos.attach(cbBuffer, SOCKET_PACKET);
			bos << rep;

			m_pITCPNetworkEngine->SendData(dwContextID,MDM_GP_USER,SUB_GP_USER_SEND_LABA_REP,cbBuffer,bos.length());
		}
	}

	//将消息转给中心服务器，由中心服务器广播消息
	CMD_CS_SpeakerSendMsgReq csReq;
	csReq.nType = pDBResult->iType;
	csReq.nLen = pDBResult->nLen;
	memcpy(csReq.szMsg, pDBResult->szDate, pDBResult->nLen);

	//转发
	if (m_pITCPSocketCorrespond)m_pITCPSocketCorrespond->SendData(MDM_CS_SERVER_MANAGER, SUB_CS_SPEAKER_SEND_MSG_REQ, &csReq, sizeof(csReq));

	return true;
}

//查询配置结果
bool CAttemperEngineSink::OnDBRUserQueryConfigResult(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//效验参数
	ASSERT(wDataSize==sizeof(DBR_GP_UserQueryConfigResult));
	if (wDataSize!=sizeof(DBR_GP_UserQueryConfigResult)) return false;
	
	//判断在线
	ASSERT(LOWORD(dwContextID)<m_pInitParamter->m_wMaxConnect);
	if ((m_pBindParameter+LOWORD(dwContextID))->dwSocketID!=dwContextID) return true;
	
	DBR_GP_UserQueryConfigResult * pDBResult=(DBR_GP_UserQueryConfigResult *)pData;

	//回复消息给客户端
	CMD_GP_UserQueryConfigRep rep;
	rep.dwUserID = pDBResult->dwUserID;
	rep.dwVersion = pDBResult->dwVersion;
	rep.nLen = pDBResult->dwLen;
	memcpy(rep.szConfig, pDBResult->szDate, pDBResult->dwLen);

	//变量定义
	char cbBuffer[SOCKET_PACKET];
	memset(cbBuffer, 0, SOCKET_PACKET);
	bostream bos;
	bos.attach(cbBuffer, SOCKET_PACKET);
	bos << rep;
	
	m_pITCPNetworkEngine->SendData(dwContextID,MDM_GP_USER,SUB_GP_USER_QUERY_CONFIG_REP,cbBuffer,bos.length());

	return true;
}

//请求商城购买下订单的结果
bool CAttemperEngineSink::OnDBRUserMallPlaceOrderResult(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//效验参数
	ASSERT(wDataSize==sizeof(DBR_GP_UserMallPlaceOrderResult));
	if (wDataSize!=sizeof(DBR_GP_UserMallPlaceOrderResult)) return false;

	//判断在线
	ASSERT(LOWORD(dwContextID)<m_pInitParamter->m_wMaxConnect);
	if ((m_pBindParameter+LOWORD(dwContextID))->dwSocketID!=dwContextID) return true;

	//变量定义
	char cbBuffer[SOCKET_PACKET];
	DBR_GP_UserMallPlaceOrderResult * pExchange=(DBR_GP_UserMallPlaceOrderResult *)pData;
	CMD_GP_UserMallPlaceOrderResp exchange;

	//写日志
	char buf[200];
	memset(buf, 0, 200);
	sprintf(buf, "订单号=%s， 商品名=%s， 商品价格=%d", pExchange->dwOrderID, pExchange->productName, pExchange->productPrice);
	TraceMallOrderMessage(buf);

	//构造数据
	exchange.nResult = pExchange->nResult;
	exchange.productPrice = pExchange->productPrice;
	memcpy(exchange.dwOrderID, pExchange->dwOrderID, sizeof(pExchange->dwOrderID));
	memcpy(exchange.token, pExchange->token, sizeof(pExchange->token));
	memcpy(exchange.productName, pExchange->productName, sizeof(pExchange->productName));
	memcpy(exchange.billingCode, pExchange->billingCode, sizeof(pExchange->billingCode));

	memset(cbBuffer, 0, SOCKET_PACKET);
	bostream bos;
	bos.attach(cbBuffer, SOCKET_PACKET);
	bos << exchange;

	m_pITCPNetworkEngine->SendData(dwContextID,MDM_GP_USER,SUB_GP_USER_MALL_PLACE_ORDER_RESP,cbBuffer,bos.length());

	return true;
}

//请求查询用户银行信息的结果
bool CAttemperEngineSink::OnDBRUserBankInfoResult(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//效验参数
	ASSERT(wDataSize==sizeof(DBR_GP_UserBankInfoResult));
	if (wDataSize!=sizeof(DBR_GP_UserBankInfoResult)) return false;

	//判断在线
	ASSERT(LOWORD(dwContextID)<m_pInitParamter->m_wMaxConnect);
	if ((m_pBindParameter+LOWORD(dwContextID))->dwSocketID!=dwContextID) return true;

	//变量定义
	char cbBuffer[SOCKET_PACKET];
	DBR_GP_UserBankInfoResult * pResult=(DBR_GP_UserBankInfoResult *)pData;
	CMD_GP_UserBankInfoRep rep;

	//构造数据
	rep.nCode = pResult->nResult;
	rep.dwUserID = pResult->dwUserID;
	rep.dwCurScore = pResult->dwCurScore;
	rep.dwBankScore = pResult->dwBankScore;

	memset(cbBuffer, 0, SOCKET_PACKET);
	bostream bos;
	bos.attach(cbBuffer, SOCKET_PACKET);
	bos << rep;

	m_pITCPNetworkEngine->SendData(dwContextID,MDM_GP_USER,SUB_GP_USER_BANK_INFO_REP,cbBuffer,bos.length());

	return true;
}

//用户操作银行结果
bool CAttemperEngineSink::OnDBRUserBankChargeResult(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//效验参数
	ASSERT(wDataSize==sizeof(DBR_GP_UserBankChargeResult));
	if (wDataSize!=sizeof(DBR_GP_UserBankChargeResult)) return false;

	//判断在线
	ASSERT(LOWORD(dwContextID)<m_pInitParamter->m_wMaxConnect);
	if ((m_pBindParameter+LOWORD(dwContextID))->dwSocketID!=dwContextID) return true;

	//变量定义
	char cbBuffer[SOCKET_PACKET];
	DBR_GP_UserBankChargeResult * pResult=(DBR_GP_UserBankChargeResult *)pData;
	CMD_GP_UserBankChargeRep rep;

	//构造数据
	rep.nCode = pResult->nResult;
	rep.dwUserID = pResult->dwUserID;
	rep.dwCurScore = pResult->dwCurScore;
	rep.dwBankScore = pResult->dwBankScore;

	memset(cbBuffer, 0, SOCKET_PACKET);
	bostream bos;
	bos.attach(cbBuffer, SOCKET_PACKET);
	bos << rep;

	m_pITCPNetworkEngine->SendData(dwContextID,MDM_GP_USER,SUB_GP_USER_BANK_CHARGE_REP,cbBuffer,bos.length());

	return true;
}

//连接事件
bool __cdecl CAttemperEngineSink::OnEventTCPSocketLink(WORD wServiceID, INT nErrorCode)
{
	//错误判断
	if (nErrorCode!=0)
	{
		//设置时间
		m_pITimerEngine->SetTimer(IDI_CONNECT_CENTER_SERVER,TIME_RECONNECT,1,NULL);

		//错误提示
		CTraceService::TraceString(TEXT("中心服务器连接失败，稍后将会再次尝试...."),TraceLevel_Warning);

		return true;
	}
	else
	{
		//错误提示
		CTraceService::TraceString(TEXT("中心服务器连接成功"),TraceLevel_Normal);
	}

	//获取列表
	m_pITCPSocketCorrespond->SendData(MDM_CS_SERVER_LIST,SUB_CS_GET_SERVER_LIST,NULL,0);

	//设置定时器
	m_pITimerEngine->SetTimer(IDI_UPDATE_SERVER_LIST,TIME_UPDATE_LIST,TIMES_INFINITY,NULL);

	return true;
}

//关闭事件
bool __cdecl CAttemperEngineSink::OnEventTCPSocketShut(WORD wServiceID, BYTE cbShutReason)
{
	//重新连接
	m_pITimerEngine->KillTimer(IDI_UPDATE_SERVER_LIST);
	m_pITimerEngine->SetTimer(IDI_CONNECT_CENTER_SERVER,TIME_RECONNECT,1,NULL);

	//错误提示
	CTraceService::TraceString(TEXT("中心服务器连接中断，稍后将会再次尝试...."),TraceLevel_Warning);

	return true;
}

//读取事件
bool __cdecl CAttemperEngineSink::OnEventTCPSocketRead(WORD wServiceID, CMD_Command Command, VOID * pData, WORD wDataSize)
{
	switch (Command.wMainCmdID)
	{
	case MDM_CS_SERVER_LIST:	//列表消息
		{
			return OnCenterMainServerList(Command.wSubCmdID,pData,wDataSize);
		}
	case MDM_CS_SERVER_MANAGER:	//服务器管理
		{
			return OnCenterMainServerManager(Command.wSubCmdID,pData,wDataSize);
		}
	}

	return true;
}

//网络应答事件
bool __cdecl CAttemperEngineSink::OnEventTCPNetworkBind(DWORD dwClientIP, DWORD dwSocketID)
{
	//获取索引
	ASSERT(LOWORD(dwSocketID)<m_pInitParamter->m_wMaxConnect);
	if (LOWORD(dwSocketID)>=m_pInitParamter->m_wMaxConnect) return false;

	//变量定义
	WORD wBindIndex=LOWORD(dwSocketID);
	tagBindParameter * pBindParameter=(m_pBindParameter+wBindIndex);

	//设置变量
	pBindParameter->dwSocketID=dwSocketID;
	pBindParameter->dwClientIP=dwClientIP;
	pBindParameter->dwActiveTime=(DWORD)time(NULL);

	return true;
}



//网络读取事件
bool __cdecl CAttemperEngineSink::OnEventTCPNetworkRead(CMD_Command Command, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch (Command.wMainCmdID)
	{
	case MDM_GP_LOGON:				//登录消息
		{
			return OnSocketMainLogon(Command.wSubCmdID,pData,wDataSize,dwSocketID);
		} 
	case MDM_GP_USER:				//用户信息
		{
			return OnSocketMainUser(Command.wSubCmdID,pData,wDataSize,dwSocketID);
		}
	case MDM_GP_SERVER_LIST:		//服务器列表
		{
			return OnSocketServerList(Command.wSubCmdID,pData,wDataSize,dwSocketID);
		}
	}

	return false;
}

//网络关闭事件
bool __cdecl CAttemperEngineSink::OnEventTCPNetworkShut(DWORD dwClientIP, DWORD dwActiveTime, DWORD dwSocketID)
{
	//清除信息
	WORD wBindIndex=LOWORD(dwSocketID);
	ZeroMemory((m_pBindParameter+wBindIndex),sizeof(tagBindParameter));

	std::map<DWORD, unsigned int>::const_iterator cit = m_mapRechargeSocketIDUser.find(dwSocketID);
	if (m_mapRechargeSocketIDUser.end() != cit)
	{
		m_mapRechargeUserSocketID.erase(cit->second);
		m_mapRechargeSocketIDUser.erase(cit);
	}

	return true;
}

//登录消息处理
bool CAttemperEngineSink::OnSocketMainLogon(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch (wSubCmdID)
	{
	case SUB_GP_HEART_BEAT:		//心跳
		{
			return true;
		}
	case SUB_GP_LOGON_ACCOUNTS:		//帐号登录
		{
			
			bistream bis;
			bis.attach((char*)pData,wDataSize);

			CMD_GP_LogonByAccounts_CB obj;
			bis >> obj;


			//效验参数
	//		ASSERT(wDataSize>=sizeof(CMD_GP_LogonByAccounts));
	//		if (wDataSize<sizeof(CMD_GP_LogonByAccounts)) return false;

			//处理消息
			CMD_GP_LogonByAccounts_CB * pLogonByAccounts = &obj;
	//		CMD_GP_LogonByAccounts * pLogonByAccounts=(CMD_GP_LogonByAccounts *)pData;
	//		pLogonByAccounts->szAccounts[CountArray(pLogonByAccounts->szAccounts)-1]=0;
	//		pLogonByAccounts->szPassWord[CountArray(pLogonByAccounts->szPassWord)-1]=0;

			//连接信息
			ASSERT(LOWORD(dwSocketID)<m_pInitParamter->m_wMaxConnect);
			DWORD dwClientAddr=(m_pBindParameter+LOWORD(dwSocketID))->dwClientIP;

			//效验版本
			if (pLogonByAccounts->dwPlazaVersion!=VER_PLAZA_FRAME)
			{
				//获取版本
				WORD wLow=LOWORD(pLogonByAccounts->dwPlazaVersion);
				WORD wHigh=HIWORD(pLogonByAccounts->dwPlazaVersion);

				//构造数据
				CMD_GP_Version Version;
				Version.bNewVersion=TRUE;
				Version.bAllowConnect=(wHigh==VER_PLAZA_HIGH)?TRUE:FALSE;

				//发送数据
				m_pITCPNetworkEngine->SendData(dwSocketID,MDM_GP_SYSTEM,SUB_GP_VERSION,&Version,sizeof(Version));

				//判断关闭
				if (Version.bAllowConnect==FALSE)
				{
					return true;
				}
			}

			//构造数据
			DBR_GP_LogonByAccounts LogonByAccounts;
			ZeroMemory(&LogonByAccounts,sizeof(LogonByAccounts));

			//设置变量
			LogonByAccounts.dwClientIP=dwClientAddr;
			lstrcpyn(LogonByAccounts.szAccounts,pLogonByAccounts->szAccounts,sizeof(LogonByAccounts.szAccounts));
			lstrcpyn(LogonByAccounts.szPassWord,pLogonByAccounts->szPassWord,sizeof(LogonByAccounts.szPassWord));
			lstrcpyn(LogonByAccounts.szComputerID,pLogonByAccounts->szComputerID,sizeof(LogonByAccounts.szComputerID));

			//变量定义
			VOID * pData=NULL;
			tagDataDescribe DataDescribe;
			DataDescribe.wDataSize = 0;
			DataDescribe.wDataDescribe = 0;
			CRecvPacketHelper RecvPacket(pLogonByAccounts+1,wDataSize-sizeof(CMD_GP_LogonByAccounts_CB));

			//扩展信息
			while (false)
			{
				//提取数据
				pData=RecvPacket.GetData(DataDescribe);
				if (DataDescribe.wDataDescribe==DTP_NULL) break;

				//数据分析
				switch (DataDescribe.wDataDescribe)
				{
				case DTP_COMPUTER_ID:		//机器标识
					{
						ASSERT(pData!=NULL);
						ASSERT(DataDescribe.wDataSize==sizeof(tagClientSerial));
						if (DataDescribe.wDataSize==sizeof(tagClientSerial))
						{
							BYTE * pcbByte=(BYTE *)pData;
							for (INT i=0;i<sizeof(tagClientSerial)/sizeof(BYTE);i++)
							{
								ASSERT(CountArray(LogonByAccounts.szComputerID)>((i*2)+1));
								_stprintf(&LogonByAccounts.szComputerID[i*2],TEXT("%02X"),*(pcbByte+i));
							}
						}

						break;
					}
				}
			}

			//投递数据库
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_LOGON_BY_ACCOUNTS,dwSocketID,&LogonByAccounts,sizeof(LogonByAccounts));

			return true;
		}
	case SUB_GP_LOGON_USERID:		//ID 登录
		{
			//效验参数
			ASSERT(wDataSize>=sizeof(CMD_GP_LogonByUserID_CB));
			if (wDataSize<sizeof(CMD_GP_LogonByUserID_CB)) return false;

			bistream bis;
			bis.attach((char*)pData,wDataSize);

			CMD_GP_LogonByUserID_CB obj;
			bis >> obj;

			//处理消息
			//CMD_GP_LogonByUserID * pLogonByUserID=(CMD_GP_LogonByUserID *)pData;
			CMD_GP_LogonByUserID_CB * pLogonByUserID=&obj;
			pLogonByUserID->szPassWord[CountArray(pLogonByUserID->szPassWord)-1]=0;

			//连接信息
			ASSERT(LOWORD(dwSocketID)<m_pInitParamter->m_wMaxConnect);
			DWORD dwClientAddr=(m_pBindParameter+LOWORD(dwSocketID))->dwClientIP;

			//效验版本
			if (pLogonByUserID->dwPlazaVersion!=VER_PLAZA_FRAME)
			{
				//获取版本
				WORD wLow=LOWORD(pLogonByUserID->dwPlazaVersion);
				WORD wHigh=HIWORD(pLogonByUserID->dwPlazaVersion);

				//构造数据
				CMD_GP_Version Version;
				Version.bNewVersion=TRUE;
				Version.bAllowConnect=(wHigh==VER_PLAZA_HIGH)?TRUE:FALSE;

				//发送数据
				m_pITCPNetworkEngine->SendData(dwSocketID,MDM_GP_SYSTEM,SUB_GP_VERSION,&Version,sizeof(Version));

				//判断关闭
				if (Version.bAllowConnect==FALSE)
				{
					return true;
				}
			}

			//构造数据
			DBR_GP_LogonByUserID LogonByUserID;
			memset(&LogonByUserID,0,sizeof(LogonByUserID));
			LogonByUserID.dwClientIP=dwClientAddr;
			LogonByUserID.dwUserID=pLogonByUserID->dwUserID;
			lstrcpyn(LogonByUserID.szPassWord,pLogonByUserID->szPassWord,sizeof(LogonByUserID.szPassWord));

			//变量定义
			VOID * pData=NULL;
			tagDataDescribe DataDescribe;
			CRecvPacketHelper RecvPacket(pLogonByUserID+1,wDataSize-sizeof(CMD_GP_LogonByUserID_CB));

			//扩展信息
			while (true)
			{
				//提取数据
				pData=RecvPacket.GetData(DataDescribe);
				if (DataDescribe.wDataDescribe==DTP_NULL) break;

				//数据分析
				switch (DataDescribe.wDataDescribe)
				{
				case DTP_COMPUTER_ID:		//机器标识
					{
						ASSERT(pData!=NULL);
						ASSERT(DataDescribe.wDataSize==sizeof(tagClientSerial));
						if (DataDescribe.wDataSize==sizeof(tagClientSerial))
						{
							BYTE * pcbByte=(BYTE *)pData;
							for (INT i=0;i<sizeof(tagClientSerial)/sizeof(BYTE);i++)
							{
								ASSERT(CountArray(LogonByUserID.szComputerID)>((i*2)+1));
								_stprintf(&LogonByUserID.szComputerID[i*2],TEXT("%02X"),*(pcbByte+i));
							}
						}

						break;
					}
				}
			}

			//投递数据库
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_LOGON_BY_USERID,dwSocketID,&LogonByUserID,sizeof(LogonByUserID));

			return true;
		}
	case SUB_GP_REGISTER_ACCOUNTS:		//注册帐号
		{
			//效验参数
//			ASSERT(wDataSize>=sizeof(CMD_GP_RegisterAccounts_CB));
//			if (wDataSize<sizeof(CMD_GP_RegisterAccounts_CB)) return false;

			
			bistream bis;
			bis.attach((char*)pData,wDataSize);

			CMD_GP_RegisterAccounts_CB obj;
			bis >> obj;

			//处理消息
			//CMD_GP_RegisterAccounts * pRegisterAccounts=(CMD_GP_RegisterAccounts *)pData;
			CMD_GP_RegisterAccounts_CB * pRegisterAccounts=&obj;
			pRegisterAccounts->szAccounts[CountArray(pRegisterAccounts->szAccounts)-1]=0;
			pRegisterAccounts->szPassWord[CountArray(pRegisterAccounts->szPassWord)-1]=0;

			//连接信息
			ASSERT(LOWORD(dwSocketID)<m_pInitParamter->m_wMaxConnect);
			DWORD dwClientAddr=(m_pBindParameter+LOWORD(dwSocketID))->dwClientIP;

			//效验版本
			if (pRegisterAccounts->dwPlazaVersion!=VER_PLAZA_FRAME)
			{
				//获取版本
				WORD wLow=LOWORD(pRegisterAccounts->dwPlazaVersion);
				WORD wHigh=HIWORD(pRegisterAccounts->dwPlazaVersion);

				//构造数据
				CMD_GP_Version Version;
				Version.bNewVersion=TRUE;
				Version.bAllowConnect=(wHigh==VER_PLAZA_HIGH)?TRUE:FALSE;

				//发送数据
				m_pITCPNetworkEngine->SendData(dwSocketID,MDM_GP_SYSTEM,SUB_GP_VERSION,&Version,sizeof(Version));

				//判断关闭
				if (Version.bAllowConnect==FALSE)
				{
					return true;
				}
			}

			//构造数据
			DBR_GP_RegisterAccounts RegisterAccounts;
			memset(&RegisterAccounts,0,sizeof(RegisterAccounts));
			RegisterAccounts.dwClientIP=dwClientAddr;
			RegisterAccounts.wFaceID=pRegisterAccounts->wFaceID;
			RegisterAccounts.cbGender=pRegisterAccounts->cbGender;
			lstrcpyn(RegisterAccounts.szSpreader,pRegisterAccounts->szSpreader,sizeof(RegisterAccounts.szSpreader));
			lstrcpyn(RegisterAccounts.szAccounts,pRegisterAccounts->szAccounts,sizeof(RegisterAccounts.szAccounts));
			lstrcpyn(RegisterAccounts.szPassWord,pRegisterAccounts->szPassWord,sizeof(RegisterAccounts.szPassWord));
			lstrcpyn(RegisterAccounts.szComputerID,pRegisterAccounts->szComputerID,sizeof(RegisterAccounts.szComputerID));

			//变量定义
			VOID * pData=NULL;
			tagDataDescribe DataDescribe;
			CRecvPacketHelper RecvPacket(pRegisterAccounts+1,wDataSize-sizeof(CMD_GP_RegisterAccounts_CB));
/*
			//扩展信息
			while (true)
			{
				//提取数据
				pData=RecvPacket.GetData(DataDescribe);
				if (DataDescribe.wDataDescribe==DTP_NULL) break;

				//数据分析
				switch (DataDescribe.wDataDescribe)
				{
				case DTP_COMPUTER_ID:		//机器标识
					{
						ASSERT(pData!=NULL);
						ASSERT(DataDescribe.wDataSize==sizeof(tagClientSerial));
						if (DataDescribe.wDataSize==sizeof(tagClientSerial))
						{
							BYTE * pcbByte=(BYTE *)pData;
							for (INT i=0;i<sizeof(tagClientSerial)/sizeof(BYTE);i++)
							{
								ASSERT(CountArray(RegisterAccounts.szComputerID)>((i*2)+1));
								_stprintf(&RegisterAccounts.szComputerID[i*2],TEXT("%02X"),*(pcbByte+i));
							}
						}

						break;
					}
				}
			}
*/
			//投递数据库
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_REGISTER_ACCOUNTS,dwSocketID,&RegisterAccounts,sizeof(RegisterAccounts));

			return true;
		}
	case SUB_GP_LOGON_WX:		//微信登录
		{
			bistream bis;
			bis.attach((char*)pData,wDataSize);

			CMD_GP_LogonByWX obj;
			bis >> obj;

			//处理消息
			CMD_GP_LogonByWX * pRegisterAccounts=&obj;

			//连接信息
			ASSERT(LOWORD(dwSocketID)<m_pInitParamter->m_wMaxConnect);
			DWORD dwClientAddr=(m_pBindParameter+LOWORD(dwSocketID))->dwClientIP;

			//效验版本
			if (pRegisterAccounts->dwPlazaVersion!=VER_PLAZA_FRAME)
			{
				//获取版本
				WORD wLow=LOWORD(pRegisterAccounts->dwPlazaVersion);
				WORD wHigh=HIWORD(pRegisterAccounts->dwPlazaVersion);

				//构造数据
				CMD_GP_Version Version;
				Version.bNewVersion=TRUE;
				Version.bAllowConnect=(wHigh==VER_PLAZA_HIGH)?TRUE:FALSE;

				//发送数据
				m_pITCPNetworkEngine->SendData(dwSocketID,MDM_GP_SYSTEM,SUB_GP_VERSION,&Version,sizeof(Version));

				//判断关闭
				if (Version.bAllowConnect==FALSE)
				{
					return true;
				}
			}

			//构造数据
			DBR_GP_RegisterAccountsWX RegisterAccounts;
			memset(&RegisterAccounts,0,sizeof(RegisterAccounts));
			RegisterAccounts.dwClientIP=dwClientAddr;
			RegisterAccounts.cbGender=pRegisterAccounts->cSex;
			lstrcpyn(RegisterAccounts.szSpreader,pRegisterAccounts->szSpreader,sizeof(RegisterAccounts.szSpreader));
			lstrcpyn(RegisterAccounts.szAccounts,pRegisterAccounts->szAccounts,sizeof(RegisterAccounts.szAccounts));
			lstrcpyn(RegisterAccounts.szPassWord,pRegisterAccounts->szPassWord,sizeof(RegisterAccounts.szPassWord));
			lstrcpyn(RegisterAccounts.szWXComputerID,pRegisterAccounts->szWXComputerID,sizeof(RegisterAccounts.szWXComputerID));
			lstrcpyn(RegisterAccounts.szComputerID,pRegisterAccounts->szComputerID,sizeof(RegisterAccounts.szComputerID));
			lstrcpyn(RegisterAccounts.szOpenID,pRegisterAccounts->szOpenID,sizeof(RegisterAccounts.szOpenID));
			lstrcpyn(RegisterAccounts.szUnionID,pRegisterAccounts->szUnionID,sizeof(RegisterAccounts.szUnionID));
			lstrcpyn(RegisterAccounts.szNickName,pRegisterAccounts->szNickName,sizeof(RegisterAccounts.szNickName));
			lstrcpyn(RegisterAccounts.szFaceUrl,pRegisterAccounts->szFaceUrl,sizeof(RegisterAccounts.szFaceUrl));


			//投递数据库
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_REGISTER_ACCOUNTS_WX,dwSocketID,&RegisterAccounts,sizeof(RegisterAccounts));

			return true;
		}
	case SUB_GP_WXUPDATE:		//升级成WX帐号
		{
			bistream bis;
			bis.attach((char*)pData,wDataSize);

			CMD_GP_WXUpdate obj;
			bis >> obj;

			//处理消息
			CMD_GP_WXUpdate * pRegisterAccounts=&obj;

			//连接信息
			ASSERT(LOWORD(dwSocketID)<m_pInitParamter->m_wMaxConnect);
			DWORD dwClientAddr=(m_pBindParameter+LOWORD(dwSocketID))->dwClientIP;

			//构造数据
			DBR_GP_WXUpdate RegisterAccounts;
			memset(&RegisterAccounts,0,sizeof(RegisterAccounts));
			RegisterAccounts.dwClientIP=dwClientAddr;
			RegisterAccounts.cbGender=pRegisterAccounts->cSex;
			RegisterAccounts.dwUserID=pRegisterAccounts->dwUserID;
			lstrcpyn(RegisterAccounts.szWXComputerID,pRegisterAccounts->szWXComputerID,sizeof(RegisterAccounts.szWXComputerID));
			lstrcpyn(RegisterAccounts.szComputerID,pRegisterAccounts->szComputerID,sizeof(RegisterAccounts.szComputerID));
			lstrcpyn(RegisterAccounts.szOpenID,pRegisterAccounts->szOpenID,sizeof(RegisterAccounts.szOpenID));
			lstrcpyn(RegisterAccounts.szUnionID,pRegisterAccounts->szUnionID,sizeof(RegisterAccounts.szUnionID));
			lstrcpyn(RegisterAccounts.szNickName,pRegisterAccounts->szNickName,sizeof(RegisterAccounts.szNickName));
			lstrcpyn(RegisterAccounts.szFaceUrl,pRegisterAccounts->szFaceUrl,sizeof(RegisterAccounts.szFaceUrl));


			//投递数据库
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_WXUPDATE,dwSocketID,&RegisterAccounts,sizeof(RegisterAccounts));

			return true;
		}
	}
	return false;
}

//用户信息处理
bool CAttemperEngineSink::OnSocketMainUser(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch (wSubCmdID)
	{
	case SUB_GP_USER_SIGNIN_REQ:		//签到
		{
			//数据验证
//			ASSERT(sizeof(CMD_GP_UserSignInReq)==wDataSize);
//			if ( sizeof(CMD_GP_UserSignInReq) != wDataSize) return false;

			bistream bis;
			bis.attach((char*)pData,wDataSize);

			CMD_GP_UserSignInReq obj;
			bis >> obj;

			CMD_GP_UserSignInReq *pSignin = &obj;

			//投递请求
			DBR_GP_UserSignIn UserSignIn;
			ZeroMemory(&UserSignIn, sizeof(UserSignIn));
			UserSignIn.dwUserID = pSignin->dwUserID;
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_USER_SIGNIN, dwSocketID, &UserSignIn, sizeof(UserSignIn));

			return true;
		}
	case SUB_GP_USER_GETAWARD_REQ:		//领奖
		{
			//数据验证
//			ASSERT(sizeof(CMD_GP_UserGetAwardReq)==wDataSize);
//			if ( sizeof(CMD_GP_UserGetAwardReq) != wDataSize) return false;
			
			bistream bis;
			bis.attach((char*)pData,wDataSize);

			CMD_GP_UserGetAwardReq obj;
			bis >> obj;

			CMD_GP_UserGetAwardReq *pGetAward = &obj;

			//投递请求
			DBR_GP_UserGetAward UserGetAward;
			ZeroMemory(&UserGetAward, sizeof(UserGetAward));
			UserGetAward.dwUserID = pGetAward->dwUserID;
			UserGetAward.dwTaskID = pGetAward->dwTaskID;
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_USER_GETAWARD, dwSocketID, &UserGetAward, sizeof(UserGetAward));

			return true;
		}
	case SUB_GP_USER_EXCHANGE_INFO_REQ:		//兑换个人信息请求（查询金豆、手机号、更新手机号）
		{
			//数据验证
//			ASSERT(sizeof(CMD_GP_UserExchangeInfoReq)==wDataSize);
//			if ( sizeof(CMD_GP_UserExchangeInfoReq) != wDataSize) return false;
			
			bistream bis;
			bis.attach((char*)pData,wDataSize);

			CMD_GP_UserExchangeInfoReq obj;
			bis >> obj;

			CMD_GP_UserExchangeInfoReq *pExchangeInfo = &obj;
			pExchangeInfo->szPhone[sizeof(pExchangeInfo->szPhone)-1] = 0;

			//投递请求
			DBR_GP_UserExchangeInfo UserExchangeInfo;
			ZeroMemory(&UserExchangeInfo, sizeof(UserExchangeInfo));
			UserExchangeInfo.dwUserID = pExchangeInfo->dwUserID;
			lstrcpyn(UserExchangeInfo.szPhone, pExchangeInfo->szPhone, sizeof(UserExchangeInfo.szPhone));
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_USER_EXCHANGE_INFO, dwSocketID, &UserExchangeInfo, sizeof(UserExchangeInfo));

			return true;
		}
	case SUB_GP_USER_EXCHANGE_PRODUCT_REQ:		//兑换产品请求
		{
			//数据验证
//			ASSERT(sizeof(CMD_GP_UserExchangeProductReq)==wDataSize);
//			if ( sizeof(CMD_GP_UserExchangeProductReq) != wDataSize) return false;
			bistream bis;
			bis.attach((char*)pData,wDataSize);

			CMD_GP_UserExchangeProductReq obj;
			bis >> obj;

			CMD_GP_UserExchangeProductReq *pExchangeProduct = &obj;
			//投递请求
			DBR_GP_UserExchangeProduct UserExchangeProduct;
			ZeroMemory(&UserExchangeProduct, sizeof(UserExchangeProduct));
			UserExchangeProduct.dwUserID = pExchangeProduct->dwUserID;
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_USER_EXCHANGE_PRODUCT, dwSocketID, &UserExchangeProduct, sizeof(UserExchangeProduct));

			return true;
		}
	case SUB_GP_USER_EXCHANGE_PRODUCT_REQ_V2:		//兑换产品请求
		{
			//数据验证
			//			ASSERT(sizeof(CMD_GP_UserExchangeProductReq)==wDataSize);
			//			if ( sizeof(CMD_GP_UserExchangeProductReq) != wDataSize) return false;
			bistream bis;
			bis.attach((char*)pData,wDataSize);

			CMD_GP_UserExchangeProductReq_V2 obj;
			bis >> obj;

			CMD_GP_UserExchangeProductReq_V2 *pExchangeProduct = &obj;
			//投递请求
			CMD_GP_UserExchangeProductReq_V2 UserExchangeProduct;
			ZeroMemory(&UserExchangeProduct, sizeof(UserExchangeProduct));
			UserExchangeProduct.dwUserID = pExchangeProduct->dwUserID;
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_USER_EXCHANGE_PRODUCT_V2, dwSocketID, &UserExchangeProduct, sizeof(UserExchangeProduct));

			return true;
		}
	case SUB_GP_USER_EXCHANGE_REQ:		//兑换请求
		{
			//数据验证
//			ASSERT(sizeof(CMD_GP_UserExchangeReq)==wDataSize);
//			if ( sizeof(CMD_GP_UserExchangeReq) != wDataSize) return false;
			bistream bis;
			bis.attach((char*)pData,wDataSize);

			CMD_GP_UserExchangeReq obj;
			bis >> obj;

			CMD_GP_UserExchangeReq *pExchange = &obj;
			//投递请求
			DBR_GP_UserExchange UserExchange;
			ZeroMemory(&UserExchange, sizeof(UserExchange));
			UserExchange.dwUserID = pExchange->dwUserID;
			UserExchange.dwAwardID = pExchange->dwAwardID;
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_USER_EXCHANGE, dwSocketID, &UserExchange, sizeof(UserExchange));

			return true;
		}
	case SUB_GP_USER_EXCHANGE_REQ_V2:		//兑换请求
		{
			//数据验证
			//			ASSERT(sizeof(CMD_GP_UserExchangeReq)==wDataSize);
			//			if ( sizeof(CMD_GP_UserExchangeReq) != wDataSize) return false;
			bistream bis;
			bis.attach((char*)pData,wDataSize);

			CMD_GP_UserExchangeReq_V2 obj;
			bis >> obj;

			CMD_GP_UserExchangeReq_V2 *pExchange = &obj;
			//投递请求
			DBR_GP_UserExchange UserExchange;
			ZeroMemory(&UserExchange, sizeof(UserExchange));
			UserExchange.dwUserID = pExchange->dwUserID;
			UserExchange.dwAwardID = pExchange->dwAwardID;
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_USER_EXCHANGE_V2, dwSocketID, &UserExchange, sizeof(UserExchange));

			return true;
		}
	case SUB_GP_USER_EXCHANGE_RECORD_REQ:		//兑换记录请求
		{
			//数据验证
//			int aa = sizeof(CMD_GP_UserExchangeRecordReq);
//			ASSERT(sizeof(CMD_GP_UserExchangeRecordReq)==wDataSize);
//			if ( sizeof(CMD_GP_UserExchangeRecordReq) != wDataSize) return false;
			
			bistream bis;
			bis.attach((char*)pData,wDataSize);

			CMD_GP_UserExchangeRecordReq obj;
			bis >> obj;

			CMD_GP_UserExchangeRecordReq *pExchangeRecord = &obj;
			//投递请求
			DBR_GP_UserExchangeRecord UserExchangeRecord;
			ZeroMemory(&UserExchangeRecord, sizeof(UserExchangeRecord));
			UserExchangeRecord.dwUserID = pExchangeRecord->dwUserID;
			UserExchangeRecord.nPage = pExchangeRecord->nPage;
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_USER_EXCHANGE_RECORD, dwSocketID, &UserExchangeRecord, sizeof(UserExchangeRecord));

			return true;
		}
	case SUB_GP_USER_UPLOAD_FACE:		//上传头像
		{
			//数据转换
			CMD_GP_UploadCustomFace *pUploadCustomFace = (CMD_GP_UploadCustomFace*)pData;

			//数据验证
			ASSERT(wDataSize == sizeof(CMD_GP_UploadCustomFace)-sizeof(pUploadCustomFace->bFaceData)+pUploadCustomFace->wCurrentSize);
			if (wDataSize != sizeof(CMD_GP_UploadCustomFace)-sizeof(pUploadCustomFace->bFaceData)+pUploadCustomFace->wCurrentSize) return false;

			//写入文件
			try
			{
				//文件定义
				CFile fileCustomFace;
				TCHAR szFileName[128];
				_snprintf(szFileName, CountArray(szFileName), TEXT("%s\\UploadFile_%ld.zip"), theApp.m_szDirWork, pUploadCustomFace->dwUserID);
				BOOL bSuccess=FALSE;
				if ( pUploadCustomFace->bFirstPacket == true )
					bSuccess=fileCustomFace.Open( szFileName, CFile::modeCreate|CFile::modeWrite|CFile::typeBinary);
				else
					bSuccess=fileCustomFace.Open( szFileName, CFile::modeCreate|CFile::modeWrite|CFile::typeBinary|CFile::modeNoTruncate);
				if ( bSuccess )
				{
					//移动指针
					fileCustomFace.SeekToEnd();

					//写入文件
					fileCustomFace.Write(pUploadCustomFace->bFaceData, pUploadCustomFace->wCurrentSize);

					//关闭文件
					fileCustomFace.Close();

					//完成判断
					if ( pUploadCustomFace->bLastPacket == true )
					{
						//投递请求
						DBR_GP_UploadCustomFace UploadCustomFace;
						ZeroMemory(&UploadCustomFace, sizeof(UploadCustomFace));
						UploadCustomFace.dwUserID = pUploadCustomFace->dwUserID;
						m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_USER_UPLOAD_FACE, dwSocketID, &UploadCustomFace, sizeof(DBR_GP_UploadCustomFace));
					}
				}
			}
			catch(...){}

			return true;
		}
	case SUB_GP_USER_DOWNLOAD_FACE:		//下载头像
		{
			//数据验证
			ASSERT(sizeof(CMD_GP_DownloadFace)==wDataSize);
			if ( sizeof(CMD_GP_DownloadFace) != wDataSize) return false;

			CMD_GP_DownloadFace *pDownloadFace = (CMD_GP_DownloadFace*)pData;

			ASSERT( pDownloadFace->dwUserID != 0 );

			//投递请求
			DBR_GP_DownloadCustomFace DownloadCustomFace;
			ZeroMemory(&DownloadCustomFace, sizeof(DownloadCustomFace));
			DownloadCustomFace.dwUserID = pDownloadFace->dwUserID ;
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_DOWNLOAD_FACE, dwSocketID, &DownloadCustomFace, sizeof(DownloadCustomFace));

			return true;
		}
	case SUB_GP_CUSTOM_FACE_DELETE:		//删除头像
		{
			//数据验证
			ASSERT(sizeof(CMD_GP_CustomFaceDelete) == wDataSize);
			if ( sizeof(CMD_GP_CustomFaceDelete) != wDataSize) return false;

			//类型转换
			CMD_GP_CustomFaceDelete *pCustomFaceDelete = (CMD_GP_CustomFaceDelete *)pData;

			DBR_GP_CustomFaceDelete CustomFaceDelete;
			ZeroMemory(&CustomFaceDelete, sizeof(CustomFaceDelete));
			CustomFaceDelete.dwUserID = pCustomFaceDelete->dwUserID;
			CustomFaceDelete.dwFaceVer = pCustomFaceDelete->dwFaceVer;

			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_DELETE_FACE, dwSocketID, &CustomFaceDelete, sizeof(CustomFaceDelete));

			return true;
		}
	case SUB_GP_MODIFY_INDIVIDUAL:		//修改资料
		{
			//数据验证
//			int ii = sizeof(CMD_GP_ModifyIndividual);
//			ASSERT(sizeof(CMD_GP_ModifyIndividual) == wDataSize);
//			if ( sizeof(CMD_GP_ModifyIndividual) != wDataSize) return false;
			
			bistream bis;
			bis.attach((char*)pData,wDataSize);

			CMD_GP_ModifyIndividual obj;
			bis >> obj;

			//类型转换
			CMD_GP_ModifyIndividual *pModifyIndividual = &obj;
			pModifyIndividual->szNickname[sizeof(pModifyIndividual->szNickname)-1] = 0;
			pModifyIndividual->szAccount[sizeof(pModifyIndividual->szAccount)-1] = 0;
			pModifyIndividual->szPhone[sizeof(pModifyIndividual->szPhone)-1] = 0;

			DBR_GP_ModifyIndividual ModifyIndividual;

			ZeroMemory(&ModifyIndividual, sizeof(ModifyIndividual));

			ModifyIndividual.nGender = pModifyIndividual->nGender;
			lstrcpyn(ModifyIndividual.szAccount, pModifyIndividual->szAccount, sizeof(ModifyIndividual.szAccount));
			lstrcpyn(ModifyIndividual.szNickname, pModifyIndividual->szNickname, sizeof(ModifyIndividual.szNickname));
			lstrcpyn(ModifyIndividual.szPhone, pModifyIndividual->szPhone, sizeof(ModifyIndividual.szPhone));

			//投递请求
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_MODIFY_INDIVIDUAL, dwSocketID, &ModifyIndividual, sizeof(ModifyIndividual));

			return true;
		}
	case SUB_GP_MODIFY_UNDERWRITE:		//修改资料
		{
			//数据验证
//			int ii = sizeof(CMD_GP_ModifyUnderWrite);
//			ASSERT(sizeof(CMD_GP_ModifyUnderWrite) == wDataSize);
//			if ( sizeof(CMD_GP_ModifyUnderWrite) != wDataSize) return false;
			
			bistream bis;
			bis.attach((char*)pData,wDataSize);

			CMD_GP_ModifyUnderWrite obj;
			bis >> obj;

			//类型转换
			CMD_GP_ModifyUnderWrite *pModifyUnderWrite = &obj;
			pModifyUnderWrite->szDescribe[sizeof(pModifyUnderWrite->szDescribe)-1] = 0;

			DBR_GP_ModifyUnderWrite ModifyUnderWrite;

			ZeroMemory(&ModifyUnderWrite, sizeof(ModifyUnderWrite));

			ModifyUnderWrite.dwUserID = pModifyUnderWrite->dwUserID;
			lstrcpyn(ModifyUnderWrite.szDescribe, pModifyUnderWrite->szDescribe, sizeof(ModifyUnderWrite.szDescribe));

			//投递请求
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_MODIFY_UNDERWRITE, dwSocketID, &ModifyUnderWrite, sizeof(ModifyUnderWrite));

			return true;
		}
	case SUB_GP_MODIFY_PHONE:		//修改手机号
		{
			//数据验证
//			int ii = sizeof(CMD_GP_ModifyPhone);
//			ASSERT(sizeof(CMD_GP_ModifyPhone) == wDataSize);
//			if ( sizeof(CMD_GP_ModifyPhone) != wDataSize) return false;
			
			bistream bis;
			bis.attach((char*)pData,wDataSize);

			CMD_GP_ModifyPhone obj;
			bis >> obj;

			//类型转换
			CMD_GP_ModifyPhone *pModifyPhone = &obj;
			pModifyPhone->szPhone[sizeof(pModifyPhone->szPhone)-1] = 0;

			DBR_GP_ModifyPhone ModifyPhone;

			ZeroMemory(&ModifyPhone, sizeof(ModifyPhone));

			ModifyPhone.dwUserID = pModifyPhone->dwUserID;
			lstrcpyn(ModifyPhone.szPhone, pModifyPhone->szPhone, sizeof(ModifyPhone.szPhone));

			//投递请求
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_MODIFY_PHONE, dwSocketID, &ModifyPhone, sizeof(ModifyPhone));

			return true;
		}
	case SUB_GP_CHECK_ID_REQ:		//校验名称
		{
			//数据验证
//			ASSERT(sizeof(CMD_GP_CheckIDReq)==wDataSize);
//			if ( sizeof(CMD_GP_CheckIDReq) != wDataSize) return false;
			
			bistream bis;
			bis.attach((char*)pData,wDataSize);

			CMD_GP_CheckIDReq obj;
			bis >> obj;

			CMD_GP_CheckIDReq *pReq = &obj;
			//投递请求
			DBR_GP_CheckID checkID;
			ZeroMemory(&checkID, sizeof(checkID));
			pReq->szAccount[sizeof(pReq->szAccount)-1] = 0;
			lstrcpyn(checkID.szAccount, pReq->szAccount, sizeof(checkID.szAccount));

			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_CHECKID, dwSocketID, &checkID, sizeof(checkID));

			return true;
		}
	case SUB_GP_CHECK_NICKNAME_REQ:		//校验昵称
		{
			//数据验证
//			ASSERT(sizeof(CMD_GP_CheckNickNameReq)==wDataSize);
//			if ( sizeof(CMD_GP_CheckNickNameReq) != wDataSize) return false;

			bistream bis;
			bis.attach((char*)pData,wDataSize);

			CMD_GP_CheckNickNameReq obj;
			bis >> obj;

			CMD_GP_CheckNickNameReq *pReq =  &obj;
			//投递请求
			DBR_GP_CheckNickName dbReq;
			ZeroMemory(&dbReq, sizeof(dbReq));
			pReq->szNickName[sizeof(pReq->szNickName)-1] = 0;
			lstrcpyn(dbReq.szNickName, pReq->szNickName, sizeof(dbReq.szNickName));

			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_CHECK_NICKNAME, dwSocketID, &dbReq, sizeof(dbReq));

			return true;
		}
	case SUB_GP_REGISTER_REQ:		//注册
		{
			//数据验证
//			ASSERT(sizeof(CMD_GP_RegisterReq)==wDataSize);
//			if ( sizeof(CMD_GP_RegisterReq) != wDataSize) return false;
			bistream bis;
			bis.attach((char*)pData,wDataSize);

			CMD_GP_RegisterReq obj;
			bis >> obj;

			CMD_GP_RegisterReq *pReq =  &obj;
			//投递请求
			DBR_GP_Register dbReq;
			ZeroMemory(&dbReq, sizeof(dbReq));
			pReq->szAccount[sizeof(pReq->szAccount)-1] = 0;
			pReq->szPassWord[sizeof(pReq->szPassWord)-1] = 0;
			pReq->szNickName[sizeof(pReq->szNickName)-1] = 0;
			pReq->szPhone[sizeof(pReq->szPhone)-1] = 0;

			lstrcpyn(dbReq.szAccount, pReq->szAccount, sizeof(dbReq.szAccount));
			lstrcpyn(dbReq.szPassword, pReq->szPassWord, sizeof(dbReq.szPassword));
			lstrcpyn(dbReq.szNickName, pReq->szNickName, sizeof(dbReq.szNickName));
			lstrcpyn(dbReq.szPhone, pReq->szPhone, sizeof(dbReq.szPhone));
			dbReq.nGender = pReq->nGender;

			DWORD dwClientAddr=(m_pBindParameter+LOWORD(dwSocketID))->dwClientIP;
			dbReq.dwClientIP=dwClientAddr;
			lstrcpyn(dbReq.szComputerID,pReq->szComputerID,sizeof(dbReq.szComputerID));

			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_REGISTER, dwSocketID, &dbReq, sizeof(dbReq));

			return true;
		}
	case SUB_GP_REGISTER_CHANNEL_REQ:		//注册 -- 带渠道号
		{
			//数据验证
//			ASSERT(sizeof(CMD_GP_RegisterReq)==wDataSize);
//			if ( sizeof(CMD_GP_RegisterReq) != wDataSize) return false;
			bistream bis;
			bis.attach((char*)pData,wDataSize);

			CMD_GP_RegisterWithChannelReq obj;
			bis >> obj;

			CMD_GP_RegisterWithChannelReq *pReq =  &obj;
			//投递请求
			DBR_GP_Register dbReq;
			ZeroMemory(&dbReq, sizeof(dbReq));
			pReq->szAccount[sizeof(pReq->szAccount)-1] = 0;
			pReq->szPassWord[sizeof(pReq->szPassWord)-1] = 0;
			pReq->szNickName[sizeof(pReq->szNickName)-1] = 0;
			pReq->szPhone[sizeof(pReq->szPhone)-1] = 0;
			pReq->szChannel[sizeof(pReq->szChannel)-1] = 0;

			lstrcpyn(dbReq.szAccount, pReq->szAccount, sizeof(dbReq.szAccount));
			lstrcpyn(dbReq.szPassword, pReq->szPassWord, sizeof(dbReq.szPassword));
			lstrcpyn(dbReq.szNickName, pReq->szNickName, sizeof(dbReq.szNickName));
			lstrcpyn(dbReq.szPhone, pReq->szPhone, sizeof(dbReq.szPhone));
			lstrcpyn(dbReq.szChannel, pReq->szChannel, sizeof(dbReq.szChannel));
			dbReq.nGender = pReq->nGender;

			DWORD dwClientAddr=(m_pBindParameter+LOWORD(dwSocketID))->dwClientIP;
			dbReq.dwClientIP=dwClientAddr;
			lstrcpyn(dbReq.szComputerID,pReq->szComputerID,sizeof(dbReq.szComputerID));

			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_REGISTER, dwSocketID, &dbReq, sizeof(dbReq));

			return true;
		}
	case SUB_GP_ID_UPDATE_REQ:		//升级
		{
			//数据验证
//			ASSERT(sizeof(CMD_GP_IDUpdateReq)==wDataSize);
//			if ( sizeof(CMD_GP_IDUpdateReq) != wDataSize) return false;
			bistream bis;
			bis.attach((char*)pData,wDataSize);

			CMD_GP_IDUpdateReq obj;
			bis >> obj;

			CMD_GP_IDUpdateReq *pReq = &obj;
			//投递请求
			DBR_GP_IDUpdate dbReq;
			ZeroMemory(&dbReq, sizeof(dbReq));
			pReq->szAccount[sizeof(pReq->szAccount)-1] = 0;
			pReq->szAccountOld[sizeof(pReq->szAccountOld)-1] = 0;
			pReq->szPassWord[sizeof(pReq->szPassWord)-1] = 0;
			pReq->szNickName[sizeof(pReq->szNickName)-1] = 0;

			lstrcpyn(dbReq.szAccount, pReq->szAccount, sizeof(dbReq.szAccount));
			lstrcpyn(dbReq.szAccountOld, pReq->szAccountOld, sizeof(dbReq.szAccountOld));
			lstrcpyn(dbReq.szPassword, pReq->szPassWord, sizeof(dbReq.szPassword));
			lstrcpyn(dbReq.szNickName, pReq->szNickName, sizeof(dbReq.szNickName));
			dbReq.nGender = pReq->nGender;

			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_ID_UPDATE, dwSocketID, &dbReq, sizeof(dbReq));

			return true;
		}
	case SUB_GP_RESET_PWD_REQ:		//设置密码
		{
			//数据验证
//			int sz = sizeof(CMD_GP_ResetPwdReq);
//			ASSERT(sizeof(CMD_GP_ResetPwdReq)==wDataSize);
//			if ( sizeof(CMD_GP_ResetPwdReq) != wDataSize) return false;
			
			bistream bis;
			bis.attach((char*)pData,wDataSize);

			CMD_GP_ResetPwdReq obj;
			bis >> obj;

			CMD_GP_ResetPwdReq *pReq = &obj;
			//投递请求
			DBR_GP_ResetPwd dbReq;
			ZeroMemory(&dbReq, sizeof(dbReq));
			pReq->szAccount[sizeof(pReq->szAccount)-1] = 0;
			pReq->szPassword[sizeof(pReq->szPassword)-1] = 0;

			lstrcpyn(dbReq.szAccount, pReq->szAccount, sizeof(dbReq.szAccount));
			lstrcpyn(dbReq.szPassword, pReq->szPassword, sizeof(dbReq.szPassword));

			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_RESET_PWD, dwSocketID, &dbReq, sizeof(dbReq));

			return true;
		}
	case SUB_GP_USER_INFO_REQ:		//用户信息
		{
			//数据验证
//			ASSERT(sizeof(CMD_GP_UserInfoReq)==wDataSize);
//			if ( sizeof(CMD_GP_UserInfoReq) != wDataSize) return false;
			
			bistream bis;
			bis.attach((char*)pData,wDataSize);

			CMD_GP_UserInfoReq obj;
			bis >> obj;

			CMD_GP_UserInfoReq *pReq = &obj;

			//投递请求
			DBR_GP_UserInfo dbReq;
			ZeroMemory(&dbReq, sizeof(dbReq));
			pReq->szAccount[sizeof(pReq->szAccount)-1] = 0;

			lstrcpyn(dbReq.szAccount, pReq->szAccount, sizeof(dbReq.szAccount));

			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_USER_INFO, dwSocketID, &dbReq, sizeof(dbReq));

			return true;
		}
	case SUB_GP_USER_CAN_SPREAD_REQ:		//用户能否领取推广
		{
			bistream bis;
			bis.attach((char*)pData,wDataSize);

			CMD_GP_UserCanSpread obj;
			bis >> obj;

			CMD_GP_UserCanSpread *pReq = &obj;

			//投递请求
			DBR_GP_UserCanSpread dbReq;
			ZeroMemory(&dbReq, sizeof(dbReq));
			dbReq.dwUserID = pReq->dwUserID;
			lstrcpyn(dbReq.szComputerID,pReq->szComputerID,sizeof(dbReq.szComputerID));

			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_USER_CAN_SPREAD, dwSocketID, &dbReq, sizeof(dbReq));

			return true;
		}
	case SUB_GP_USER_GET_SPREAD_REQ:		//用户领取推广
		{
			bistream bis;
			bis.attach((char*)pData,wDataSize);

			CMD_GP_UserGetSpread obj;
			bis >> obj;

			CMD_GP_UserGetSpread *pReq = &obj;

			//投递请求
			DBR_GP_UserGetSpread dbReq;
			ZeroMemory(&dbReq, sizeof(dbReq));
			dbReq.dwUserID = pReq->dwUserID;
			dbReq.dwSpreaderID = pReq->dwSpreaderID;
			lstrcpyn(dbReq.szComputerID,pReq->szComputerID,sizeof(dbReq.szComputerID));

			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_USER_GET_SPREAD, dwSocketID, &dbReq, sizeof(dbReq));

			return true;
		}
	case SUB_GP_USER_SPREADER_INFO_REQ:		//推广员信息
		{
			bistream bis;
			bis.attach((char*)pData,wDataSize);

			CMD_GP_UserGetSpreaderInfo obj;
			bis >> obj;

			CMD_GP_UserGetSpreaderInfo *pReq = &obj;

			//投递请求
			DBR_GP_UserSpreaderInfo dbReq;
			ZeroMemory(&dbReq, sizeof(dbReq));
			dbReq.dwSpreaderID = pReq->dwSpreaderID;

			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_USER_SPREADER_INFO, dwSocketID, &dbReq, sizeof(dbReq));

			return true;
		}
	case SUB_GP_USER_BBS_POP_REQ:		//查询弹窗公告
		{
			//投递请求
			DBR_GP_UserGetBbsPop dbReq;
			ZeroMemory(&dbReq, sizeof(dbReq));

			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_USER_GET_BBS_POP, dwSocketID, &dbReq, sizeof(dbReq));

			return true;
		}
	case SUB_GP_USER_BBS_POP_REQ_V2:		//查询弹窗公告
		{
			bistream bis;
			bis.attach((char*)pData,wDataSize);

			CMD_GP_UserBbsPop_V2 obj;
			bis >> obj;

			CMD_GP_UserBbsPop_V2 *pReq = &obj;

			//投递请求
			DBR_GP_UserGetBbsPop dbReq;
			ZeroMemory(&dbReq, sizeof(dbReq));
			lstrcpyn(dbReq.szChannel,pReq->szChannel,sizeof(dbReq.szChannel));

			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_USER_GET_BBS_POP, dwSocketID, &dbReq, sizeof(dbReq));

			return true;
		}
	case SUB_GP_USER_BBS_SCROLL_REQ:		//查询滚动公告
		{
			//fix bug:因为协议字写反了，所以这里做兼容处理
			CMD_GP_UserBbsScroll_V2 obj;
			if (wDataSize > 0)
			{
				bistream bis;
				bis.attach((char*)pData,wDataSize);

				bis >> obj;
			}
			CMD_GP_UserBbsScroll_V2 *pReq = &obj;

			//投递请求
			DBR_GP_UserGetBbsScroll dbReq;
			ZeroMemory(&dbReq, sizeof(dbReq));
			lstrcpyn(dbReq.szChannel,pReq->szChannel,sizeof(dbReq.szChannel));

			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_USER_GET_BBS_SCROLL, dwSocketID, &dbReq, sizeof(dbReq));

			return true;
		}
	case SUB_GP_USER_BBS_SCROLL_REQ_V2:		//查询滚动公告
		{
			bistream bis;
			bis.attach((char*)pData,wDataSize);

			CMD_GP_UserBbsScroll_V2 obj;
			bis >> obj;

			CMD_GP_UserBbsScroll_V2 *pReq = &obj;

			//投递请求
			DBR_GP_UserGetBbsScroll dbReq;
			ZeroMemory(&dbReq, sizeof(dbReq));
			lstrcpyn(dbReq.szChannel,pReq->szChannel,sizeof(dbReq.szChannel));

			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_USER_GET_BBS_SCROLL, dwSocketID, &dbReq, sizeof(dbReq));

			return true;
		}
	case SUB_GP_USER_BBS_LIST_REQ:		//查询列表公告
		{
			//fix bug:因为协议字写反了，所以这里做兼容处理
			CMD_GP_UserBbsList_V2 obj;
			if (wDataSize > 0)
			{
				bistream bis;
				bis.attach((char*)pData,wDataSize);

				bis >> obj;
			}
			CMD_GP_UserBbsList_V2 *pReq = &obj;

			//投递请求
			DBR_GP_UserGetBbsList dbReq;
			ZeroMemory(&dbReq, sizeof(dbReq));
			lstrcpyn(dbReq.szChannel,pReq->szChannel,sizeof(dbReq.szChannel));

			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_USER_GET_BBS_LIST, dwSocketID, &dbReq, sizeof(dbReq));

			return true;
		}
	case SUB_GP_USER_BBS_LIST_REQ_V2:		//查询列表公告
		{
			bistream bis;
			bis.attach((char*)pData,wDataSize);

			CMD_GP_UserBbsList_V2 obj;
			bis >> obj;

			CMD_GP_UserBbsList_V2 *pReq = &obj;

			//投递请求
			DBR_GP_UserGetBbsList dbReq;
			ZeroMemory(&dbReq, sizeof(dbReq));
			lstrcpyn(dbReq.szChannel,pReq->szChannel,sizeof(dbReq.szChannel));

			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_USER_GET_BBS_LIST, dwSocketID, &dbReq, sizeof(dbReq));

			return true;
		}
	case SUB_GP_USER_TASK_INFO_REQ:		//查询玩家任务统计信息
		{
			bistream bis;
			bis.attach((char*)pData,wDataSize);

			CMD_GP_UserTaskInfo obj;
			bis >> obj;

			CMD_GP_UserTaskInfo *pReq = &obj;

			//投递请求
			DBR_GP_UserGetTaskInfo dbReq;
			dbReq.dwUserID = pReq->dwUserID;

			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_USER_GET_TASK_INFO, dwSocketID, &dbReq, sizeof(dbReq));

			return true;
		}
	case SUB_GP_USER_RANKING_REQ:		//查询玩家排行榜
		{
			bistream bis;
			bis.attach((char*)pData,wDataSize);

			CMD_GP_UserRankingReq obj;
			bis >> obj;

			CMD_GP_UserRankingReq *pReq = &obj;

			//投递请求
			DBR_GP_UserGetRanking dbReq;
			dbReq.dwUserID = pReq->dwUserID;
			dbReq.nPage = pReq->nPage;

			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_USER_GET_BANKING, dwSocketID, &dbReq, sizeof(dbReq));

			return true;
		}
	case SUB_GP_USER_RANKING_REQ_V2:		//查询玩家排行榜
		{
			bistream bis;
			bis.attach((char*)pData,wDataSize);

			CMD_GP_UserRankingReq_V2 obj;
			bis >> obj;

			CMD_GP_UserRankingReq_V2 *pReq = &obj;

			//投递请求
			DBR_GP_UserGetRanking dbReq;
			dbReq.dwUserID = pReq->dwUserID;
			dbReq.nPage = pReq->nPage;

			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_USER_GET_BANKING_V2, dwSocketID, &dbReq, sizeof(dbReq));

			return true;
		}
	case SUB_GP_USER_MODIFY_RANKINGSTATUS:		//设置玩家是否入排行榜
		{
			bistream bis;
			bis.attach((char*)pData,wDataSize);

			CMD_GP_UserModifyRankingStatus obj;
			bis >> obj;

			CMD_GP_UserModifyRankingStatus *pReq = &obj;

			//设置玩家是否入排行榜请求
			DBR_GP_UserModifyRankingStatus dbReq;
			dbReq.dwUserID = pReq->dwUserID;
			dbReq.isInRanking = pReq->isInRanking;

			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_USER_MODIFY_RANKINGSTATUS, dwSocketID, &dbReq, sizeof(dbReq));

			return true;
		}
	case SUB_GP_USER_FEEDBACK_REQ:		//玩家反馈问题
		{
			bistream bis;
			bis.attach((char*)pData,wDataSize);

			CMD_GP_UserFeedBackReq obj;
			bis >> obj;

			CMD_GP_UserFeedBackReq *pReq = &obj;

			//投递请求
			DBR_GP_UserFeedBack dbReq;
			dbReq.dwUserID = pReq->dwUserID;
			lstrcpyn(dbReq.szPicture,pReq->szPicture,sizeof(dbReq.szPicture));
			lstrcpyn(dbReq.szContent,pReq->szContent,sizeof(dbReq.szContent));

			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_USER_FEEDBACK, dwSocketID, &dbReq, sizeof(dbReq));

			return true;
		}
	case SUB_GP_USER_GET_FEEDBACK_REQ:		//查询玩家问题
		{
			bistream bis;
			bis.attach((char*)pData,wDataSize);

			CMD_GP_UserGetFeedBackReq obj;
			bis >> obj;

			CMD_GP_UserGetFeedBackReq *pReq = &obj;

			//投递请求
			DBR_GP_UserGetFeedBack dbReq;
			dbReq.dwUserID = pReq->dwUserID;
			dbReq.nPage = pReq->nPage;

			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_USER_GET_FEEDBACK, dwSocketID, &dbReq, sizeof(dbReq));

			return true;
		}
	case SUB_GP_USER_INFO_ID_CARD_REQ:		//查询用户身份证账号信息
		{
			bistream bis;
			bis.attach((char*)pData,wDataSize);

			CMD_GP_UserInfoIDCARDReq obj;
			bis >> obj;

			//投递请求
			DBR_GP_UserInfoIDCard dbReq;
			dbReq.dwUserID = obj.dwUserID;

			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_USER_INFO_ID_CARD, dwSocketID, &dbReq, sizeof(dbReq));

			return true;
		}
	case SUB_GP_USER_AUTHENTICA_REQ:		//请求提交身份证信息(姓名及身份证号)
		{
			bistream bis;
			bis.attach((char*)pData,wDataSize);

			CMD_GP_UserAuthenticationReq obj;
			bis >> obj;

			//投递请求
			DBR_GP_UserAuthentication dbReq;
			dbReq.dwUserID = obj.dwUserID;
			lstrcpyn(dbReq.szName, obj.szName, sizeof(dbReq.szName));
			lstrcpyn(dbReq.szIdCardNum, obj.szIdCardNum, sizeof(dbReq.szIdCardNum));

			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_USER_AUTHENTICATION, dwSocketID, &dbReq, sizeof(dbReq));

			return true;
		}
	case SUB_GP_USER_MALL_PRODUCT_REQ:		//请求商城商品
		{
			bistream bis;
			bis.attach((char*)pData, wDataSize);

			CMD_GP_UserMallProductReq obj;
			bis >> obj;
			
			//投递请求
			DBR_GP_UserMallProduct dbReq;
			dbReq.dwUserID = obj.dwUserID;
			dbReq.dwVersion = 0;   //原始版本

			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_USER_MALL_PRODUCT, dwSocketID, &dbReq, sizeof(dbReq));

			return true;
		}
	case SUB_GP_USER_MALL_PRODUCT_REQ_IOS:		//请求商城商品
		{
			bistream bis;
			bis.attach((char*)pData, wDataSize);

			CMD_GP_UserMallProductReq_IOS obj;
			bis >> obj;

			//投递请求
			DBR_GP_UserMallProduct dbReq;
			dbReq.dwUserID = obj.dwUserID;
			dbReq.dwVersion = 0;   //原始版本

			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_USER_MALL_PRODUCT_IOS, dwSocketID, &dbReq, sizeof(dbReq));

			return true;
		}
	case SUB_GP_USER_MALL_PRODUCT_REQ_V2:		//请求商城商品V2
		{
			bistream bis;
			bis.attach((char*)pData, wDataSize);

			CMD_GP_UserMallProductReq_V2 obj;
			bis >> obj;
			
			//检查版本，投递不同请求
			//投递请求
			DBR_GP_UserMallProduct dbReq;
			dbReq.dwUserID = obj.dwUserID;
			dbReq.dwVersion = obj.dwVersion; 
			if (obj.dwVersion == 1)  //android v2
			{
				m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_USER_MALL_PRODUCT, dwSocketID, &dbReq, sizeof(dbReq));
			}
			else if (obj.dwVersion == 2)  // ios  v2
			{
				m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_USER_MALL_PRODUCT_IOS, dwSocketID, &dbReq, sizeof(dbReq));
			}

			return true;
		}
	case SUB_GP_USER_MALL_PLACE_ORDER_REQ:		//商城购买下订单
		{
			bistream bis;
			bis.attach((char*)pData, wDataSize);

			CMD_GP_UserMallPlaceOrderReq obj;
			bis >> obj;

			//投递请求
			DBR_GP_UserMallPlaceOrder dbReq;
			dbReq.dwUserID = obj.dwUserID;
			dbReq.dwProductID = obj.dwProductID;
			strcpy(dbReq.szPayWay,obj.szPayWay);

			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_USER_MALL_PLACE_ORDER, dwSocketID, &dbReq, sizeof(dbReq));

			return true;
		}
	case SUB_GP_USER_CANCEL_ORDER_REQ:	// 请求取消订单号
		{
			bistream bis;
			bis.attach((char*)pData, wDataSize);

			CMD_GP_UserCancelOrderNumReq obj;
			bis >> obj;

			DBR_Cancel_Order_Num dbReq;
			strcpy(dbReq.orderNum,obj.dwOrderID);
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_USER_CANCEL_ORDER,dwSocketID,&dbReq,sizeof(dbReq));
			return true;
		}
	case SUB_GP_USER_BANK_INFO_REQ:		//查询玩家银行信息
		{
			bistream bis;
			bis.attach((char*)pData, wDataSize);

			CMD_GP_UserBankInfoReq obj;
			bis >> obj;

			//投递请求
			DBR_GP_UserBankInfo dbReq;
			dbReq.dwUserID = obj.dwUserID;

			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_USER_BANK_INFO, dwSocketID, &dbReq, sizeof(dbReq));

			return true;
		}
	case SUB_GP_USER_BANK_CHARGE_REQ:		//玩家操作银行
		{
			bistream bis;
			bis.attach((char*)pData, wDataSize);

			CMD_GP_UserBankChargeReq obj;
			bis >> obj;

			//投递请求
			DBR_GP_UserBankCharge dbReq;
			dbReq.nOpCode = obj.nOpCode;
			dbReq.dwUserID = obj.dwUserID;
			dbReq.dwCurScore = obj.dwCurScore;
			dbReq.dwBankScore = obj.dwBankScore;

			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_USER_BANK_CHARGE, dwSocketID, &dbReq, sizeof(dbReq));

			return true;
		}
	case SUB_GP_USER_ACTIVITY_LIST_REQ:		//查询活动列表信息
		{
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_USER_ACTIVITY_LIST, dwSocketID, NULL, 0);

			return true;
		}
	case SUB_GP_USER_ACTIVITY_REQ:		//查询指定活动信息
		{
			bistream bis;
			bis.attach((char*)pData, wDataSize);

			CMD_GP_UserActivityReq obj;
			bis >> obj;

			//投递请求
			DBR_GP_UserActivity dbReq;
			dbReq.dwUserID = obj.dwUserID;
			dbReq.dwActivityID = obj.dwActivityID;

			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_USER_ACTIVITY, dwSocketID, &dbReq, sizeof(dbReq));

			return true;
		}
	case SUB_GP_USER_ACTIVITY_LUCKY_REQ:		//活动抽奖
		{
			bistream bis;
			bis.attach((char*)pData, wDataSize);

			CMD_GP_UserActivityLuckyReq obj;
			bis >> obj;

			//投递请求
			DBR_GP_UserActivityLucky dbReq;
			dbReq.dwUserID = obj.dwUserID;
			dbReq.dwActivityID = obj.dwActivityID;

			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_USER_ACTIVITY_LUCKY, dwSocketID, &dbReq, sizeof(dbReq));

			return true;
		}
	case SUB_GP_USER_ACTIVITY_RECORD_REQ:		//中奖记录请求
		{
			bistream bis;
			bis.attach((char*)pData,wDataSize);

			CMD_GP_UserActivityRecordReq obj;
			bis >> obj;

			CMD_GP_UserActivityRecordReq *pActivityRecord = &obj;
			//投递请求
			DBR_GP_UserActivityRecord UserActivityRecord;
			ZeroMemory(&UserActivityRecord, sizeof(UserActivityRecord));
			UserActivityRecord.dwUserID = pActivityRecord->dwUserID;
			UserActivityRecord.dwActivityID = pActivityRecord->dwActivityID;
			UserActivityRecord.nPage = pActivityRecord->nPage;
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_USER_ACTIVITY_RECORD, dwSocketID, &UserActivityRecord, sizeof(UserActivityRecord));

			return true;
		}
	case SUB_GP_USER_CHECK_LABA_REQ:		//请求校验发送喇叭内容
		{
			bistream bis;
			bis.attach((char*)pData,wDataSize);

			CMD_GP_UserCheckLabaReq obj;
			bis >> obj;

			//校验
			std::string ret = CheckWord(obj.strLabaInfo, obj.iLen);

			CMD_GP_UserCheckLabaRep result;

			result.nCode = 0;
			result.nLen = ret.size()>sizeof(result.strLabaInfo)?sizeof(result.strLabaInfo):ret.size();
			memcpy(result.strLabaInfo, ret.c_str(), result.nLen);

			//变量定义
			char cbBuffer[SOCKET_PACKET];
			memset(cbBuffer, 0, SOCKET_PACKET);
			bostream bos;
			bos.attach(cbBuffer, SOCKET_PACKET);
			bos << result;

			//直接回消息给客户端
			m_pITCPNetworkEngine->SendData(dwSocketID,MDM_GP_USER,SUB_GP_USER_CHECK_LABA_REP,cbBuffer,bos.length());

			return true;
		}
	case SUB_GP_USER_SEND_LABA_REQ:		//请求发送喇叭
		{
			bistream bis;
			bis.attach((char*)pData,wDataSize);

			CMD_GP_UserSendLabaReq obj;
			bis >> obj;

			//校验
			std::string ret = CheckWord(obj.strLabaInfo, obj.nLen);

			//校验通过后，提交数据库处理
			DBR_GP_UserSpeakerSend dbReq;
			ZeroMemory(&dbReq, sizeof(dbReq));
			dbReq.iType = 1;   //用户消息
			dbReq.dwUserID = obj.dwUserID;
			dbReq.nLen = ret.size()>sizeof(obj.strLabaInfo)?sizeof(obj.strLabaInfo):ret.size();
			memcpy(dbReq.szDate, obj.strLabaInfo, dbReq.nLen);
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_USER_SPEAKER_SEND, dwSocketID, &dbReq, sizeof(dbReq));

			return true;
		}
	case SUB_GP_USER_QUERY_CONFIG_REQ:		//查询配置项
		{
			bistream bis;
			bis.attach((char*)pData,wDataSize);

			CMD_GP_UserQueryConfigReq obj;
			bis >> obj;

			//提交数据库处理
			DBR_GP_UserQueryConfig dbReq;
			ZeroMemory(&dbReq, sizeof(dbReq));
			dbReq.dwUserID = obj.dwUserID;
			dbReq.dwVersion = obj.dwVersion;
			dbReq.dwConfigID = obj.dwConfigID;
			memcpy(dbReq.szChannel, obj.szChannel, sizeof(dbReq.szChannel));
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_USER_QUERY_CONFIG, dwSocketID, &dbReq, sizeof(dbReq));

			return true;
		}
	default:
		{
			ASSERT(FALSE);
			return false;
		}
	}

	return true;
}

//服务器列表相关处理
bool CAttemperEngineSink::OnSocketServerList(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch (wSubCmdID)
	{
	case SUB_GP_CREATE_SELF_REQ:		//玩家创建自建场
		{
			bistream bis;
			bis.attach((char*)pData, wDataSize);

			CMD_GP_CreateSelf obj;
			bis >> obj;

			//转发请求给CenterServer
			CMD_CS_CreateSelf req;
			req.dwSocketID = dwSocketID;
			req.nServerID = obj.nServerID;
			lstrcpyn(req.szName, obj.szName, sizeof(req.szName));
			lstrcpyn(req.szPassword, obj.szPassword, sizeof(req.szPassword));
			lstrcpyn(req.szRule, obj.szRule, sizeof(req.szRule));

			//转发
			if (m_pITCPSocketCorrespond)m_pITCPSocketCorrespond->SendData(MDM_CS_SERVER_MANAGER, SUB_CS_CREATE_SELF_REQ, &req, sizeof(req));

			return true;
		}
	case SUB_GP_CHECK_PWD_SELF_REQ:		//请求自建场校验密码
		{
			bistream bis;
			bis.attach((char*)pData, wDataSize);

			CMD_GP_CheckPwdSelf obj;
			bis >> obj;

			//转发请求给CenterServer
			CMD_CS_CheckPwdSelf req;
			req.dwSocketID = dwSocketID;
			req.nServerID = obj.nServerID;
			lstrcpyn(req.szRoomID, obj.szRoomID, sizeof(req.szRoomID));
			lstrcpyn(req.szPassword, obj.szPassword, sizeof(req.szPassword));

			//转发
			if (m_pITCPSocketCorrespond)m_pITCPSocketCorrespond->SendData(MDM_CS_SERVER_MANAGER, SUB_CS_CHECK_PWD_SELF_REQ, &req, sizeof(req));

			return true;
		}
	case SUB_GP_QUERY_SELF_REQ:		//玩家查询自建场
		{
			bistream bis;
			bis.attach((char*)pData, wDataSize);

			CMD_GP_QuerySelf obj;
			bis >> obj;

			//转发请求给CenterServer
			CMD_CS_QuerySelf req;
			req.dwSocketID = dwSocketID;
			req.nServerID = obj.nServerID;
			req.nPage = obj.nPage;
			lstrcpyn(req.szName, obj.szName, sizeof(req.szName));
			lstrcpyn(req.szRoomID, obj.szRoomID, sizeof(req.szRoomID));
			lstrcpyn(req.szFilter, obj.szFilter, sizeof(req.szFilter));

			//转发
			if (m_pITCPSocketCorrespond)m_pITCPSocketCorrespond->SendData(MDM_CS_SERVER_MANAGER, SUB_CS_QUERY_SELF_REQ, &req, sizeof(req));

			return true;
		}
	default:
		{
			ASSERT(FALSE);
			return false;
		}
	}

	return true;
}

//列表消息处理
bool CAttemperEngineSink::OnCenterMainServerList(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_CS_LIST_INFO:		//列表信息
		{
			//效验参数
			ASSERT(wDataSize==sizeof(CMD_CS_ListInfo));
			if (wDataSize!=sizeof(CMD_CS_ListInfo)) return false;

			//消息处理
			m_ServerList.ResetServerListBuffer();

			return true;
		}
	case SUB_CS_LIST_TYPE:		//种类信息
		{
			//效验参数
			ASSERT(wDataSize>=sizeof(tagGameType));
			ASSERT(wDataSize%sizeof(tagGameType)==0);

			//消息处理
			DWORD dwCount=wDataSize/sizeof(tagGameType);
			tagGameType * pGameType=(tagGameType *)pData;
			m_ServerList.AppendGameTypeBuffer(pGameType,dwCount);

			return true;
		}
	case SUB_CS_LIST_KIND:		//类型信息
		{
			//效验参数
			ASSERT(wDataSize>=sizeof(tagGameKind));
			ASSERT(wDataSize%sizeof(tagGameKind)==0);

			//消息处理
			DWORD dwCount=wDataSize/sizeof(tagGameKind);
			tagGameKind * pGameKind=(tagGameKind *)pData;
			m_ServerList.AppendGameKindBuffer(pGameKind,dwCount);

			return true;
		}
	case SUB_CS_LIST_STATION:	//站点信息
		{
			//效验参数
			ASSERT(wDataSize>=sizeof(tagGameStation));
			ASSERT(wDataSize%sizeof(tagGameStation)==0);

			//消息处理
			DWORD dwCount=wDataSize/sizeof(tagGameStation);
			tagGameStation * pGameStation=(tagGameStation *)pData;
			m_ServerList.AppendGameStationBuffer(pGameStation,dwCount);

			return true;
		}
	case SUB_CS_LIST_SERVER:	//房间信息
		{
			//效验参数
			ASSERT(wDataSize>=sizeof(tagGameServer));
			ASSERT(wDataSize%sizeof(tagGameServer)==0);

			//消息处理
			DWORD dwCount=wDataSize/sizeof(tagGameServer);
			tagGameServer * pGameServer=(tagGameServer *)pData;
			m_ServerList.AppendGameServerBuffer(pGameServer,dwCount);

			return true;
		}
	case SUB_CS_LIST_FINISH:	//更新完成
		{
			//消息处理
			m_ServerList.ActiveServerListBuffer();

			//广播消息,因为人数有更新
			for (int i=0; i<m_pInitParamter->m_wMaxConnect;i++)
			{
				if (m_pBindParameter[i].dwSocketID != 0)
				{
					m_ServerList.SendGameServerList(m_pBindParameter[i].dwSocketID);
					m_pITCPNetworkEngine->SendData(m_pBindParameter[i].dwSocketID,MDM_GP_SERVER_LIST,SUB_GP_LIST_FINISH);
				}
			}

			return true;
		}
	}

	return true;
}

//服务器管理消息处理
bool CAttemperEngineSink::OnCenterMainServerManager(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_CS_CREATE_SELF_REP:		//创建自建场结果
		{
			//效验参数
			ASSERT(wDataSize==sizeof(CMD_CS_CreateSelfRep));
			if (wDataSize!=sizeof(CMD_CS_CreateSelfRep)) return true;
			
			CMD_CS_CreateSelfRep * repCS=(CMD_CS_CreateSelfRep *)pData;
			DWORD dwContextID = repCS->dwSocketID;

			//判断在线
			ASSERT(LOWORD(dwContextID)<m_pInitParamter->m_wMaxConnect);
			if ((m_pBindParameter+LOWORD(dwContextID))->dwSocketID!=dwContextID) return true;

			//变量定义
			CMD_GP_CreateSelfRep repLS;
			repLS.nCode = repCS->nCode;
			repLS.nServerID = repCS->nServerID;
			memcpy(repLS.szName, repCS->szName, sizeof(repCS->szName));
			memcpy(repLS.szRoomID, repCS->szRoomID, sizeof(repCS->szRoomID));
			memcpy(repLS.szRule, repCS->szRule, sizeof(repCS->szRule));
			
			char cbBuffer[SOCKET_PACKET];
			memset(cbBuffer, 0, SOCKET_PACKET);
			bostream bos;
			bos.attach(cbBuffer, SOCKET_PACKET);
			bos << repLS;
			
			//返回给指定客户端
			m_pITCPNetworkEngine->SendData(dwContextID,MDM_GP_SERVER_LIST,SUB_GP_CREATE_SELF_REP,cbBuffer,bos.length());

			return true;
		}
	case SUB_CS_CHECK_PWD_SELF_REP:		//校验密码结果
		{
			//效验参数
			ASSERT(wDataSize==sizeof(CMD_CS_CheckPwdSelfRep));
			if (wDataSize!=sizeof(CMD_CS_CheckPwdSelfRep)) return false;

			CMD_CS_CheckPwdSelfRep * repCS=(CMD_CS_CheckPwdSelfRep *)pData;
			DWORD dwContextID = repCS->dwSocketID;

			//判断在线
			ASSERT(LOWORD(dwContextID)<m_pInitParamter->m_wMaxConnect);
			if ((m_pBindParameter+LOWORD(dwContextID))->dwSocketID!=dwContextID) return true;

			//变量定义
			CMD_GP_CheckPwdSelfRep repLS;
			repLS.nCode = repCS->nCode;
			repLS.nServerID = repCS->nServerID;
			memcpy(repLS.szRoomID, repCS->szRoomID, sizeof(repCS->szRoomID));
			
			char cbBuffer[SOCKET_PACKET];
			memset(cbBuffer, 0, SOCKET_PACKET);
			bostream bos;
			bos.attach(cbBuffer, SOCKET_PACKET);
			bos << repLS;
			
			//返回给指定客户端
			m_pITCPNetworkEngine->SendData(dwContextID,MDM_GP_SERVER_LIST,SUB_GP_CHECK_PWD_SELF_REP,cbBuffer,bos.length());

			return true;
		}
	case SUB_CS_QUERY_SELF_REP:		//查询自建场结果
		{
			//效验参数
			ASSERT(wDataSize==sizeof(CMD_CS_QuerySelfRep));
			if (wDataSize!=sizeof(CMD_CS_QuerySelfRep)) return false;

			CMD_CS_QuerySelfRep * repCS=(CMD_CS_QuerySelfRep *)pData;
			DWORD dwContextID = repCS->dwSocketID;

			//判断在线
			ASSERT(LOWORD(dwContextID)<m_pInitParamter->m_wMaxConnect);
			if ((m_pBindParameter+LOWORD(dwContextID))->dwSocketID!=dwContextID) return true;

			//变量定义
			CMD_GP_QuerySelfRep repLS;
			repLS.nCode = repCS->nCode;
			repLS.nServerID = repCS->nServerID;
			repLS.nPage = repCS->nPage;
			repLS.nCount = repCS->nCount;
			memcpy(repLS.szName, repCS->szName, sizeof(repCS->szName));
			memcpy(repLS.szExtend, repCS->szExtend, sizeof(repCS->szExtend));
			
			char cbBuffer[SOCKET_PACKET];
			memset(cbBuffer, 0, SOCKET_PACKET);
			bostream bos;
			bos.attach(cbBuffer, SOCKET_PACKET);
			bos << repLS;
			
			//返回给指定客户端
			m_pITCPNetworkEngine->SendData(dwContextID,MDM_GP_SERVER_LIST,SUB_GP_QUERY_SELF_REP,cbBuffer,bos.length());

			return true;
		}
	case SUB_CS_SPEAKER_SEND_MSG_REQ:		//从中心服务器转过来的喇叭消息
		{
			//效验参数
			ASSERT(wDataSize==sizeof(CMD_CS_SpeakerSendMsgReq));
			if (wDataSize!=sizeof(CMD_CS_SpeakerSendMsgReq)) return false;

			CMD_CS_SpeakerSendMsgReq * repCS=(CMD_CS_SpeakerSendMsgReq *)pData;

			//转发给所有在线用户

			CMD_GP_SpeakerMsgRep repLS;
			repLS.nLen = repCS->nLen;
			repLS.nType = repCS->nType;
			memcpy(repLS.szMsg, repCS->szMsg, repCS->nLen);

			char cbBuffer[SOCKET_PACKET];
			memset(cbBuffer, 0, SOCKET_PACKET);
			bostream bos;
			bos.attach(cbBuffer, SOCKET_PACKET);
			bos << repLS;

			for (int i=0; i<m_pInitParamter->m_wMaxConnect; i++)
			{
				if (m_pBindParameter[i].dwSocketID != 0)
				{
					//广播给客户端
					m_pITCPNetworkEngine->SendData(m_pBindParameter[i].dwSocketID,MDM_GP_USER,SUB_GP_USER_SPEAKER_MSG_REP,cbBuffer,bos.length());
				}
			}
			
			return true;
		}
	}

	return true;
}

//soap事件
bool CAttemperEngineSink::OnSoapRequest(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_GP_USER_MALL_BUY_REQ:
		{
			bistream bis;
			bis.attach((char*)pData, wDataSize);

			CMD_GP_UserMallBuyReq obj;
			bis >> obj;

			DBR_GP_UserMallBuy dbReq;
			strcpy(dbReq.dwOrderID, obj.dwOrderID);
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_USER_MALL_BUY, -1, &dbReq, sizeof(dbReq));
			return true;
		}
	case SUB_GP_USER_MALL_UPDATE_REQ:
		{
			bistream bis;
			bis.attach((char*)pData, wDataSize);

			CMD_GP_UserMallUpdateReq obj;
			bis >> obj;

			DBR_GP_UserMallUpdate dbReq;
			strcpy(dbReq.dwOrderID, obj.dwOrderID);
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_USER_MALL_UPDATE, -1, &dbReq, sizeof(dbReq));
			return true;
		}
	default:
		break;
	}

	return false;
}

//处理敏感词
std::string CAttemperEngineSink::CheckWord(char* data, short len)
{
	std::string tmp(data);
	for (int i=0; i<m_vecInValidWord.size(); i++)
	{
		std::string::size_type pos = 0;
		std::string::size_type srcLen = m_vecInValidWord[i].size();
		std::string::size_type desLen = m_vecWordReplaced[i].size();
		pos=tmp.find(m_vecInValidWord[i], pos); 
		while ((pos != std::string::npos))
		{
			tmp.replace(pos, srcLen, m_vecWordReplaced[i]);
			pos=tmp.find(m_vecInValidWord[i], (pos+desLen));
		}
	}
	return tmp;
}

//////////////////////////////////////////////////////////////////////////
