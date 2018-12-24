#include "StdAfx.h"
#include "initdata.h"

char  CInitData::s_cLoginServer[128];						//登录服务器地址
char  CInitData::s_cLoadServer[128];						//登录服务器地址
char  CInitData::s_cHomePage[128];							//官方网站
char  CInitData::s_cGameHelp[128];							//游戏帮助
char  CInitData::s_cClientCenter[128];						//客服中心
char  CInitData::s_cGetPassword[128];						//找回密码
char  CInitData::s_cErrorTickling[128];						//问题反馈
char  CInitData::s_cAccountsManage[128];					//账号管理
char  CInitData::s_cLogonAdverTisement[128];				//登录广告
char  CInitData::s_cLogonNotice[128];						//登录公告
char  CInitData::s_cContactService[256];					//在线客服
char  CInitData::s_cConfirmGetGold[128];					//获取金币
char  CInitData::s_cLogonRegister[128];						//注册帐号
char  CInitData::s_cLoginHomePage[128];						//官方网站(登录)
char  CInitData::s_cLoginAccountsManage[128];				//账号管理(登录)
char  CInitData::s_cLoginPayment[128];						//帐号充值
char  CInitData::s_cLoginExchange[128];						//奖品兑换
char  CInitData::s_cLoginPropShop[128];						//道具商城
char  CInitData::s_cLoginPropShopOpen[128];					//道具商城(弹出式)
char  CInitData::s_cLoginTaskNew[128];						//新手任务
char  CInitData::s_cLoginUserInfo[128];						//玩家信息
char  CInitData::s_cLoginSysMessage[128];					//系统邮件
char  CInitData::s_cLoginBank[128];							//银行
char  CInitData::s_cLoginBackpack[128];						//背包
char  CInitData::s_cGameHelpSingle[128];					//单个游戏帮助
char  CInitData::s_cXBMessage[128];							//喜报消息(游戏列表下面网页)
char  CInitData::s_cNotice[128];							//活动公告
char  CInitData::s_cAdverTisement[128];						//底部广告
char  CInitData::s_cTitleNotice[128];						//头部广告

CInitData::CInitData(void)
{
}

CInitData::~CInitData(void)
{
}

//初始化
void CInitData::InitData()
{
	CString	sFileName("");
	CString sFilePath("");
	GetModuleFileName(NULL, sFileName.GetBufferSetLength(128), 128);
	sFilePath=sFileName.Left(sFileName.ReverseFind('\\') + 1) + TEXT("config\\config.ini");
	
	GetPrivateProfileString(TEXT("ServersAddr"), TEXT("LoginServer"), TEXT(""), s_cLoginServer, 128, sFilePath);
	GetPrivateProfileString(TEXT("ServersAddr"), TEXT("LoadServer"), TEXT(""), s_cLoadServer, 128, sFilePath);
	GetPrivateProfileString(TEXT("WebBrowser"), TEXT("HomePage"), TEXT(""), s_cHomePage, 128, sFilePath);
	GetPrivateProfileString(TEXT("WebBrowser"), TEXT("GameHelp"), TEXT(""), s_cGameHelp, 128, sFilePath);
	GetPrivateProfileString(TEXT("WebBrowser"), TEXT("ClientCenter"), TEXT(""), s_cClientCenter, 128, sFilePath);
	GetPrivateProfileString(TEXT("WebBrowser"), TEXT("GetPassword"), TEXT(""), s_cGetPassword, 128, sFilePath);
	GetPrivateProfileString(TEXT("WebBrowser"), TEXT("ErrorTickling"), TEXT(""), s_cErrorTickling, 128, sFilePath);
	GetPrivateProfileString(TEXT("WebBrowser"), TEXT("AccountsManage"), TEXT(""), s_cAccountsManage, 128, sFilePath);
	GetPrivateProfileString(TEXT("WebBrowser"), TEXT("LogonAdverTisement"), TEXT(""), s_cLogonAdverTisement, 128, sFilePath);
	GetPrivateProfileString(TEXT("WebBrowser"), TEXT("LogonNotice"), TEXT(""), s_cLogonNotice, 128, sFilePath);
	GetPrivateProfileString(TEXT("WebBrowser"), TEXT("ContactService"), TEXT(""), s_cContactService, 256, sFilePath);
	GetPrivateProfileString(TEXT("WebBrowser"), TEXT("ConfirmGetGold"), TEXT(""), s_cConfirmGetGold, 128, sFilePath);
	GetPrivateProfileString(TEXT("WebBrowser"), TEXT("LogonRegister"), TEXT(""), s_cLogonRegister, 128, sFilePath);
	GetPrivateProfileString(TEXT("WebBrowser"), TEXT("LoginHomePage"), TEXT(""), s_cLoginHomePage, 128, sFilePath);
	GetPrivateProfileString(TEXT("WebBrowser"), TEXT("LoginAccountsManage"), TEXT(""), s_cLoginAccountsManage, 128, sFilePath);
	GetPrivateProfileString(TEXT("WebBrowser"), TEXT("LoginPayment"), TEXT(""), s_cLoginPayment, 128, sFilePath);
	GetPrivateProfileString(TEXT("WebBrowser"), TEXT("LoginExchange"), TEXT(""), s_cLoginExchange, 128, sFilePath);
	GetPrivateProfileString(TEXT("WebBrowser"), TEXT("LoginPropShop"), TEXT(""), s_cLoginPropShop, 128, sFilePath);
	GetPrivateProfileString(TEXT("WebBrowser"), TEXT("LoginPropShopOpen"), TEXT(""), s_cLoginPropShopOpen, 128, sFilePath);
	GetPrivateProfileString(TEXT("WebBrowser"), TEXT("LoginTaskNew"), TEXT(""), s_cLoginTaskNew, 128, sFilePath);
	GetPrivateProfileString(TEXT("WebBrowser"), TEXT("LoginUserInfo"), TEXT(""), s_cLoginUserInfo, 128, sFilePath);
	GetPrivateProfileString(TEXT("WebBrowser"), TEXT("LoginSysMessage"), TEXT(""), s_cLoginSysMessage, 128, sFilePath);
	GetPrivateProfileString(TEXT("WebBrowser"), TEXT("LoginBank"), TEXT(""), s_cLoginBank, 128, sFilePath);
	GetPrivateProfileString(TEXT("WebBrowser"), TEXT("LoginBackpack"), TEXT(""), s_cLoginBackpack, 128, sFilePath);
	GetPrivateProfileString(TEXT("WebBrowser"), TEXT("GameHelpSingle"), TEXT(""), s_cGameHelpSingle, 128, sFilePath);
	GetPrivateProfileString(TEXT("WebBrowser"), TEXT("XBMessage"), TEXT(""), s_cXBMessage, 128, sFilePath);
	GetPrivateProfileString(TEXT("WebBrowser"), TEXT("Notice"), TEXT(""), s_cNotice, 128, sFilePath);
	GetPrivateProfileString(TEXT("WebBrowser"), TEXT("AdverTisement"), TEXT(""), s_cAdverTisement, 128, sFilePath);
	GetPrivateProfileString(TEXT("WebBrowser"), TEXT("TitleNotice"), TEXT(""), s_cTitleNotice, 128, sFilePath);
}
//释放资源
void CInitData::FreeData()
{
}
