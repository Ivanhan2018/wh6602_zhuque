#ifndef _WEBPATH_H__A4775701_F4E9_4d22_8B4D_856AB1BDC87C
#define _WEBPATH_H__A4775701_F4E9_4d22_8B4D_856AB1BDC87C

/*
弹出式网页：
			官方网站
			游戏帮助
			商城充值
			找回密码
			客服中心
			注册用户——用户使用协议
			账号管理
			充值中心
			客服中心

内嵌网页：
			我的宝箱
			游戏商城
			任务
			藏宝阁
			用户形象
			对应游戏房间列表的广告
			大厅广告
*/

/*

#define		ONLINENET			1


#ifdef ONLINENET
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//外网
//用户使用协议
#define WEB_USER_DOUCOMET  (TEXT("http://www.cbwgame.com"))
//---------------------------

//官方网站
#define WEB_HOME_PAGE  (TEXT("http://www.cbwgame.com/index.php"))
//游戏帮助
#define WEB_GAME_HELP  (TEXT("http://www.cbwgame.com/customer.php#game"))
//客服中心
#define WEB_CLINET_CENTER  (TEXT("http://www.cbwgame.com/customer.php"))
//找回密码
#define WEB_GET_PASSWORD  (TEXT("http://www.cbwgame.com/find_password.php"))
//问题反馈
#define WEB_ERROR_TICKLING  (TEXT("http://www.cbwgame.com/suggest.php?userid=%d&key=%s"))
//账号管理
#define WEB_USER_ACCOUNTS_MANAGE (TEXT("http://www.cbwgame.com/user_center.php"))
//登录广告
#define WEB_LOGON_ADVERTISEMENT (TEXT("http://www.cbwgame.com/client/inner.php?mod=2"))
//登录公告
#define WEB_LOGON_NOTICE (TEXT("http://www.cbwgame.com/client/inner.php?mod=1"))
//在线客服
#define WEB_CONTACT_SERVICE (TEXT("http://chat56.live800.com/live800/chatClient/chatbox.jsp?companyID=170069&jid=8073229581&enterurl=http%3A%2F%2Fwww%2Ecbwgame%2Ecom%2F&pagereferrer=&tm=1321516662827"))
//获取金币
#define WEB_CONFIRM_GETGOLD (TEXT("http://cbwgame.com/payment.php"))
//注册帐号
#define WEB_LOGON_REGISTER (TEXT("http://www.cbwgame.com/register.php"))
//官方网站(登录)
#define WEB_LOGIN_HOME_PAGE  (TEXT("http://www.cbwgame.com/index.php?userid=%d&key=%s"))
//账号管理(登录)
#define WEB_LOGIN_ACCOUNTS_MANAGE (TEXT("http://www.cbwgame.com/user_center.php?userid=%d&key=%s"))
//帐号充值
#define WEB_LOGIN_PAYMENT (TEXT("http://www.cbwgame.com/client/payment.php?userid=%d&key=%s"))
//奖品兑换
#define WEB_LOGIN_EXCHANGE (TEXT("http://www.cbwgame.com/shop.php?userid=%d&key=%s"))
//道具商城
#define WEB_LOGIN_PROPSHOP (TEXT("http://www.cbwgame.com/client/propshop.php?userid=%d&key=%s"))
//道具商城(弹出式)
#define WEB_LOGIN_PROPSHOP_OPEN (TEXT("http://www.cbwgame.com/propshop.php?userid=%d&key=%s"))
//新手任务
#define WEB_LOGIN_TASK_NEW (TEXT("http://www.cbwgame.com/client/tasknewer.php?userid=%d&key=%s"))
//玩家信息
#define WEB_LOGIN_USERINFO (TEXT("http://www.cbwgame.com/client/inner.php?mod=5&userid=%d&key=%s"))
//系统邮件
#define WEB_LOGIN_SYS_MESSAGE (TEXT("http://www.cbwgame.com/client/inner.php?mod=6&userid=%d&key=%s"))
//银行
#define WEB_LOGIN_BANK (TEXT("http://www.cbwgame.com/client/inner.php?mod=7&userid=%d&key=%s"))
//背包
#define WEB_LOGIN_BACKPACK (TEXT("http://www.cbwgame.com/client/inner.php?mod=8&userid=%d&key=%s"))
//单个游戏帮助
#define WEB_GAME_HELP_SINGLE (TEXT("http://www.cbwgame.com/client/game_rule.php?kindID=%d"))
//喜报消息(游戏列表下面网页)
#define WEB_XB_MESSAGE (TEXT("http://www.cbwgame.com/client/inner.php?mod=9"))
//活动公告
#define WEB_NOTICE (TEXT("http://www.cbwgame.com/client/inner.php?mod=4"))
//底部广告
#define WEB_ADVERTISEMENT (TEXT("http://www.cbwgame.com/client/inner.php?mod=3"))
//头部广告
#define WEB_TITLE_NOTICE (TEXT("http://www.cbwgame.com/client/inner.php?mod=10"))

//-----------------------------------以下好像没用到了。-----------------------------------
//查看更多个人消息
#define WEB_LOOK_MY_MSG	(TEXT("http://www.cbw66.com/lookmoreusermsg.aspx?userid=%d&password=%s"))
//游戏帮助网址
#define WEB_MY_GAMEROOM_HELP (TEXT("http://www.cbw66.com/gamehelpstaill.aspx?helpstaill=%ld"))
//我的商城
#define WEB_MY_SHOP (TEXT("http://www.cbw66.com/gameshops.aspx?hashell=1&userid=%d&password=%s"))
//我的任务
#define WEB_MY_TASK (TEXT("http://www.cbw66.com/task/Task.aspx??hashell=1&userid=%d&password=%s"))
//我的宝箱
#define WEB_MY_DOTEY_BOX (TEXT("http://www.cbw66.com/UserBuyGrouy.aspx?userid=%d&password=%s"))
//房间公告
#define WEB_ROOM_AD  (TEXT("http://www.cbw66.com/line.aspx"))
//大厅公告
#define WEB_HALL_AD  (TEXT("http://www.cbw66.com/pub.aspx"))
//游戏商场
#define WEB_GAME_SHOP  (TEXT("http://www.cbw66.com/gameshop.aspx"))
//用户形象
#define WEB_USER_INFO  (TEXT("http://www.cbw66.com/publicimage.aspx?userid=%d&password=%s"))
//商城充值
#define WEB_DEPOSIT_LIST  (TEXT("http://www.cbw66.com/depositlist.aspx"))
//-----------------------------------------------------------------------------------------

//服务器地址
#define SERVER_IP			(TEXT("loginserver.cbwgame.com"))				//服务器IP
//#define SERVER_IP			(TEXT("222.240.205.246"))			//服务器IP

//游戏下载地址
#define GAME_DOWNLOAD_SERVER_IP_ADDRESS (TEXT("http://www.cbwgame.com/download/%s"))

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#else
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//内网测试
//用户使用协议
#define WEB_USER_DOUCOMET  (TEXT("http://192.168.1.66/zhuque/index.php"))
//-----------------------------

//官方网站
#define WEB_HOME_PAGE  (TEXT("http://192.168.1.66/zhuque/index.php"))
//游戏帮助
#define WEB_GAME_HELP  (TEXT("http://192.168.1.66/zhuque/customer.php#game"))
//客服中心
#define WEB_CLINET_CENTER  (TEXT("http://192.168.1.66/zhuque/customer.php"))
//找回密码
#define WEB_GET_PASSWORD  (TEXT("http://192.168.1.66/zhuque/find_password.php"))
//问题反馈
#define WEB_ERROR_TICKLING  (TEXT("http://192.168.1.66/zhuque/suggest.php?userid=%d&key=%s"))
//账号管理
#define WEB_USER_ACCOUNTS_MANAGE (TEXT("http://192.168.1.66/zhuque/user_center.php"))
//登录广告
#define WEB_LOGON_ADVERTISEMENT (TEXT("http://192.168.1.66/zhuque/client/inner.php?mod=2"))
//登录公告
#define WEB_LOGON_NOTICE (TEXT("http://192.168.1.66/zhuque/client/inner.php?mod=1"))
//在线客服
#define WEB_CONTACT_SERVICE (TEXT("http://chat56.live800.com/live800/chatClient/chatbox.jsp?companyID=170069&jid=8073229581&enterurl=http%3A%2F%2Fwww%2Ecbwgame%2Ecom%2F&pagereferrer=&tm=1321516662827"))
//获取金币
#define WEB_CONFIRM_GETGOLD (TEXT("http://192.168.1.66/zhuque/payment.php"))
//注册帐号
#define WEB_LOGON_REGISTER (TEXT("http://192.168.1.66/zhuque/register.php"))
//官方网站(登录)
#define WEB_LOGIN_HOME_PAGE  (TEXT("http://192.168.1.66/zhuque/index.php?userid=%d&key=%s"))
//账号管理(登录)
#define WEB_LOGIN_ACCOUNTS_MANAGE (TEXT("http://192.168.1.66/zhuque/user_center.php?userid=%d&key=%s"))
//帐号充值
#define WEB_LOGIN_PAYMENT (TEXT("http://192.168.1.66/zhuque/client/payment.php?userid=%d&key=%s"))
//奖品兑换
#define WEB_LOGIN_EXCHANGE (TEXT("http://192.168.1.66/zhuque/shop.php?userid=%d&key=%s"))
//道具商城
#define WEB_LOGIN_PROPSHOP (TEXT("http://192.168.1.66/zhuque/client/propshop.php?userid=%d&key=%s"))
//道具商城(弹出式)
#define WEB_LOGIN_PROPSHOP_OPEN (TEXT("http://192.168.1.66/zhuque/propshop.php?userid=%d&key=%s"))
//新手任务
#define WEB_LOGIN_TASK_NEW (TEXT("http://192.168.1.66/zhuque/client/tasknewer.php?userid=%d&key=%s"))
//玩家信息
#define WEB_LOGIN_USERINFO (TEXT("http://192.168.1.66/zhuque/client/inner.php?mod=5&userid=%d&key=%s"))
//系统邮件
#define WEB_LOGIN_SYS_MESSAGE (TEXT("http://192.168.1.66/zhuque/client/inner.php?mod=6&userid=%d&key=%s"))
//银行
#define WEB_LOGIN_BANK (TEXT("http://192.168.1.66/zhuque/client/inner.php?mod=7&userid=%d&key=%s"))
//背包
#define WEB_LOGIN_BACKPACK (TEXT("http://192.168.1.66/zhuque/client/inner.php?mod=8&userid=%d&key=%s"))
//单个游戏帮助
#define WEB_GAME_HELP_SINGLE (TEXT("http://192.168.1.66/zhuque/client/game_rule.php?kindID=%d"))
//喜报消息(游戏列表下面网页)
#define WEB_XB_MESSAGE (TEXT("http://192.168.1.66/zhuque/client/inner.php?mod=9"))
//活动公告
#define WEB_NOTICE (TEXT("http://192.168.1.66/zhuque/client/inner.php?mod=4"))
//底部广告
#define WEB_ADVERTISEMENT (TEXT("http://192.168.1.66/zhuque/client/inner.php?mod=3"))
//头部广告
#define WEB_TITLE_NOTICE (TEXT("http://192.168.1.66/zhuque/client/inner.php?mod=10"))


//-----------------------------------以下好像没用到了。-----------------------------------
//查看更多个人消息
#define WEB_LOOK_MY_MSG	(TEXT("http://192.168.1.108/qpgame/lookmoreusermsg.aspx?userid=%d&password=%s"))
//游戏帮助网址
#define WEB_MY_GAMEROOM_HELP (TEXT("http://192.168.1.108/qpgame/gamehelpstaill.aspx?helpstaill=%ld"))
//我的商城
#define WEB_MY_SHOP (TEXT("http://192.168.1.108/qpgame/gameshops.aspx?hashell=1&userid=%d&password=%s"))
//我的任务
#define WEB_MY_TASK (TEXT("http://192.168.1.108/qpgame/task/Task.aspx??hashell=1&userid=%d&password=%s"))
//我的宝箱
#define WEB_MY_DOTEY_BOX (TEXT("http://192.168.1.108/qpgame/UserBuyGrouy.aspx?userid=%d&password=%s"))
//房间公告
#define WEB_ROOM_AD  (TEXT("http://192.168.1.66/zhuque"))
//大厅公告
#define WEB_HALL_AD  (TEXT("http://192.168.1.66/zhuque"))
//游戏商场
#define WEB_GAME_SHOP  (TEXT("http://192.168.1.66/zhuque"))
//用户形象
#define WEB_USER_INFO  (TEXT("http://192.168.1.108/qpgame/publicimage.aspx?userid=%d&password=%s"))
//商城充值
#define WEB_DEPOSIT_LIST  (TEXT("http://192.168.1.66/zhuque"))
//-----------------------------------------------------------------------------------------

//服务器地址
#define SERVER_IP			(TEXT("192.168.1.88"))				//服务器IP
//#define SERVER_IP			(TEXT("192.168.1.108"))				//服务器IP
//#define SERVER_IP			(TEXT("222.240.205.246"))			//服务器IP

//游戏下载地址
#define GAME_DOWNLOAD_SERVER_IP_ADDRESS (TEXT("http://192.168.1.66/zhuque/download/%s"))

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif
*/

#endif