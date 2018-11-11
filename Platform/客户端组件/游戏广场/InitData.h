#pragma once
#include "Stdafx.h"

class CInitData
{
public:
	CInitData(void);
	~CInitData(void);

public:
	static void InitData();									//初始化配置
	static void FreeData();									//释放资源

	

public:
	static char				s_cLoginServer[128];						//登录服务器地址
	static char				s_cLoadServer[128];							//登录服务器地址
	static char				s_cHomePage[128];							//官方网站
	static char				s_cGameHelp[128];							//游戏帮助
	static char				s_cClientCenter[128];						//客服中心
	static char				s_cGetPassword[128];						//找回密码
	static char				s_cErrorTickling[128];						//问题反馈
	static char				s_cAccountsManage[128];						//账号管理
	static char				s_cLogonAdverTisement[128];					//登录广告
	static char				s_cLogonNotice[128];						//登录公告
	static char				s_cContactService[256];						//在线客服
	static char				s_cConfirmGetGold[128];						//获取金币
	static char				s_cLogonRegister[128];						//注册帐号
	static char				s_cLoginHomePage[128];						//官方网站(登录)
	static char				s_cLoginAccountsManage[128];				//账号管理(登录)
	static char				s_cLoginPayment[128];						//帐号充值
	static char				s_cLoginExchange[128];						//奖品兑换
	static char				s_cLoginPropShop[128];						//道具商城
	static char				s_cLoginPropShopOpen[128];					//道具商城(弹出式)
	static char				s_cLoginTaskNew[128];						//新手任务
	static char				s_cLoginUserInfo[128];						//玩家信息
	static char				s_cLoginSysMessage[128];					//系统邮件
	static char				s_cLoginBank[128];							//银行
	static char				s_cLoginBackpack[128];						//背包
	static char				s_cGameHelpSingle[128];						//单个游戏帮助
	static char				s_cXBMessage[128];							//喜报消息(游戏列表下面网页)
	static char				s_cNotice[128];								//活动公告
	static char				s_cAdverTisement[128];						//底部广告
	static char				s_cTitleNotice[128];						//头部广告
};
