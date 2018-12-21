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
	friend class CLogonService;

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
	//登录请求处理
	bool OnRequestLogon(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize);
	//签到
	bool OnRequestUserSignIn(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize);
	//领奖
	bool OnRequestUserGetAward(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize);
	//兑换的个人信息
	bool OnRequestUserExchangeInfo(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize);
	//兑换商品
	bool OnRequestUserExchangeProduct(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize);
	//兑换商品
	bool OnRequestUserExchangeProduct_V2(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize);
	//兑换
	bool OnRequestUserExchange(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize);
	//兑换
	bool OnRequestUserExchange_V2(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize);
	//兑换记录
	bool OnRequestUserExchangeRecord(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize);
	//上传头像
	bool OnRequestUploadFace(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize);
	//下载头像
	bool OnRequestDownloadFace(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize);
	//删除消息
	bool OnRequestCustomFaceDelete(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize);
	//修改资料
	bool OnRequestModifyIndividual(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize);
	//修改个性签名
	bool OnRequestModifyUnderWrite(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize);
	//修改手机号
	bool OnRequestModifyPhone(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize);
	//校验名称
	bool OnRequestCheckID(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize);
	//校验昵称
	bool OnRequestCheckNickName(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize);
	//注册
	bool OnRequestRegister(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize);
	//帐号升级
	bool OnRequestIDUpdate(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize);
	//设置密码
	bool OnRequestResetPwd(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize);
	//用户信息
	bool OnRequestUserInfo(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize);
	//用户能否领取推广奖励
	bool OnRequestUserCanSpread(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize);
	//用户领取推广奖励
	bool OnRequestUserGetSpread(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize);
	//推广员信息
	bool OnRequestSpreaderInfo(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize);
	//弹出公告信息
	bool OnRequestGetBbsPop(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize);
	//滚动公告信息
	bool OnRequestGetBbsScroll(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize);
	//列表公告信息
	bool OnRequestGetBbsList(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize);
	//玩家任务信息
	bool OnRequestGetTaskInfo(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize);
	//玩家排行榜
	bool OnRequestGetRanking(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize);
	//玩家排行榜
	bool OnRequestGetRanking_V2(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize);
	//修改玩家是否入排行榜
	bool OnRequestModifyRankingStatus(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize);
	//玩家反馈意见
	bool OnRequestFeedBack(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize);
	//查询玩家反馈意见
	bool OnRequestGetFeedBack(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize);
	//查询用户身份证账号信息
	bool OnRequestUserInfoIDCard(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize);
	//请求提交身份证信息(姓名及身份证号)
	bool OnRequestUserAuthentication(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize);
	//请求商城商品信息
	bool OnRequestMallProduct(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize, bool isIOS=false);
	//请求商城订单
	bool OnRequestMallPlaceOrder(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize);
	//请求取消订单号
	bool OnRequestUserCancelOrder(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize);
	//请求商城商品购买结果
	bool OnRequestMallBuy(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize);
	//请求商城查询订单号更新结果
	bool OnRequestMallUpdateBuy(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize);
	//请求查询银行信息
	bool OnRequestUserBankInfo(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize);
	//请求操作银行
	bool OnRequestUserBankCharge(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize);

	//查询活动列表
	bool OnRequestActivityList(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize);
	//查询活动
	bool OnRequestActivity(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize);
	//抽奖
	bool OnRequestActivityLucky(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize);
	//抽奖记录
	bool OnRequestUserActivityRecord(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize);
	//发送喇叭
	bool OnRequestUserSpeakerSend(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize);
	//查询配置项
	bool OnRequestUserQueryConfig(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize);

	//存储过程
private:
	//I D 存储过程
	LONG SPLogonByUserID(DWORD dwGameID, LPCTSTR pszPassword, DWORD dwClientIP, LPCTSTR pszComputerID);
	//帐号存储过程
	LONG SPLogonByAccounts(LPCTSTR pszAccounts, LPCTSTR pszPassword, DWORD dwClientIP, LPCTSTR pszComputerID);
	//注册存储过程
	LONG SPRegisterAccounts(LPCTSTR pszAccounts, LPCTSTR pszPassword, LPCTSTR pszSpreader, WORD wFaceID, BYTE cbGender, DWORD dwClientIP, LPCTSTR pszComputerID);
	//注册存储过程WX
	LONG SPRegisterAccountsWX(LPCTSTR pszAccounts, LPCTSTR pszPassword, LPCTSTR pszSpreader, 
							  LPCTSTR wFaceURL, BYTE cbGender, DWORD dwClientIP, LPCTSTR pszWXComputerID,LPCTSTR pszComputerID,
							  LPCTSTR pszNickname, LPCTSTR pszOpenid, LPCTSTR unionid);
	//升级成WX账号存储过程
	LONG SPWXUpdate(DWORD dwUserID,LPCTSTR wFaceURL, BYTE cbGender, DWORD dwClientIP, LPCTSTR pszWXComputerID,LPCTSTR pszComputerID,LPCTSTR pszNickname, LPCTSTR pszOpenid, LPCTSTR unionid);
	//上传存储过程
	LONG SPUploadFace(DWORD dwUserID, BYTE *bFaceData, DWORD dwFaceDataSize);
	//签到存储过程
	LONG SPUserSignIn(DWORD dwUserID);
	//领奖存储过程
	LONG SPUserGetAward(DWORD dwUserID, DWORD dwTaskID);
	//兑换的个人信息存储过程
	LONG SPUserExchangeInfo(DWORD dwUserID, LPCTSTR pszPhone);
	//兑换商品
	LONG SPUserExchangeProduct(DWORD dwUserID);
	//兑换
	LONG SPUserExchange(DWORD dwUserID,DWORD dwAwardID);
	//兑换
	LONG SPUserExchange_V2(DWORD dwUserID,DWORD dwAwardID);
	//兑换记录
	LONG SPUserExchangeRecord(DWORD dwUserID, int nPage);

	//加载存储过程
	LONG SPDownloadFace(DWORD dwUserID);
	//删除存储过程
	LONG SPDeleteCustomFace(DWORD dwUserID);
	//修改存储过程
	LONG SPModifyIndividual(TCHAR szAccount[NAME_LEN],TCHAR szNickname[NAME_LEN], TCHAR szPhone[NAME_LEN],int nGender);
	//修改个性签名存储过程
	LONG SPModifyUnderWrite(unsigned int dwUserID,TCHAR szDescribe[UNDER_WRITE_LEN]);
	//修改手机号存储过程
	LONG SPModifyPhone(unsigned int dwUserID,TCHAR szPhone[31]);

	//校验名称存储过程
	LONG SPCheckID(TCHAR szAccount[NAME_LEN]);
	//校验昵称存储过程
	LONG SPCheckNickName(TCHAR szNickName[NAME_LEN]);
	//注册存储过程
	LONG SPRegister(TCHAR szAccount[NAME_LEN],TCHAR szNickname[NAME_LEN], TCHAR szPhone[NAME_LEN], int nGender, TCHAR szPassword[PASS_LEN], 
					DWORD dwClientIP, LPCTSTR pszComputerID, LPCTSTR pszChannel);
	//帐号升级存储过程
	LONG SPIDUpdate(TCHAR szAccount[NAME_LEN],TCHAR szAccountNew[NAME_LEN],TCHAR szNickname[NAME_LEN], int nGender, TCHAR szPassword[PASS_LEN]);
	//重设密码存储过程
	LONG SPResetPwd(TCHAR szAccount[NAME_LEN], TCHAR szPassword[PASS_LEN]);
	//查询信息存储过程
	LONG SPUserInfo(TCHAR szAccount[NAME_LEN]);
	//查询用户是否可以领取推广奖励
	LONG SPUserCanSpread(DWORD dwUser, LPCTSTR pszComputerID);
	//用户领取推广奖励
	LONG SPUserGetSpread(DWORD dwUser, DWORD dwSpreader, LPCTSTR pszComputerID);
	//查询推广员信息
	LONG SPUserGetSpreadInfo(DWORD dwSpreader);
	//弹出公告信息
	LONG SPUserGetBbsPop(LPCTSTR pszChannel);
	//滚动公告信息
	LONG SPUserGetBbsScroll(LPCTSTR pszChannel);
	//列表公告信息
	LONG SPUserGetBbsList(LPCTSTR pszChannel);
	//玩家任务信息
	LONG SPUserGetTaskInfo(DWORD dwUser);
	//排行榜记录
	LONG SPUserRanking(DWORD dwUserID, int nPage);
	//排行榜记录
	LONG SPUserRanking_V2(DWORD dwUserID, int nPage);
	//玩家排行榜是否入榜
	LONG SPUserRankingStatus(DWORD dwUserID);
	//修改是否入排行榜
	LONG SPUserModifyRankingStatus(DWORD dwUserID, int nRankingStatus);
	//反馈问题
	LONG SPUserFeedBack(DWORD dwUserID, LPCTSTR pszContent, LPCTSTR pszPicture);
	//查询反馈问题
	LONG SPUserGetFeedBack(DWORD dwUserID, int nPage);
	//查询用户身份证账号信息
	LONG SPUserInfoIDCard(DWORD dwUserID);
	//请求提交身份证信息
	LONG SPUserAuthentication(DWORD dwUserID, LPCTSTR pszName, LPCTSTR pszIDCard);

	//商城商品信息
	LONG SPUserMallProduct(DWORD dwUserID);
	//商城商品信息(ios)
	LONG SPUserMallProductIOS(DWORD dwUserID);
	//下订单
	LONG SPUserMallPlaceOrder(DWORD dwUserID, DWORD dwProductID,TCHAR szPayWay[10]);
	// 取消订单号
	LONG SPUserCancelOrder(TCHAR szAccount[20]);
	// 商城购买商品结果
	LONG SPUserMallBuy(TCHAR orderNum[20]);
	// 商城更新购买结果
	LONG SPUserMallUpdateBuy(TCHAR orderNum[20]);
	//查询用户银行信息
	LONG SPUserBankInfo(DWORD dwUserID);
	//用户操作银行
	LONG SPUserBankCharge(int nOpCode, DWORD dwUserID, DWORD dwCurScore, DWORD dwBankScore);
	
	//查询活动列表
	LONG SPUserActivityList();
	//查询活动
	LONG SPUserActivity(DWORD dwUserID, DWORD dwActivityID);
	//抽奖
	LONG SPUserActivityLuckyList(DWORD dwUserID, DWORD dwActivityID);
	LONG SPUserActivityLucky(DWORD dwUserID, DWORD dwActivityID, DWORD dwPrizeID);
	//抽奖记录
	LONG SPUserActivityRecord(DWORD dwUserID, DWORD dwActivityID, int nPage);
	//发喇叭
	LONG SPUserSpeakerSend(DWORD dwUserID, WORD wType, LPCTSTR pszContent);
	//查询配置项
	LONG SPUserQueryConfig(DWORD dwUserID, DWORD dwVersion, DWORD dwConfigID, LPCTSTR pszChannel);
};

//////////////////////////////////////////////////////////////////////////

#endif