#ifndef			__INTERFACE_GAME_CLIENT_INCL_DEF_1141_CREATE__
#define			__INTERFACE_GAME_CLIENT_INCL_DEF_1141_CREATE__

#include "stdafx.h"


class IGameClientFrame
{
public:
	//是否窗口
	virtual bool __cdecl IsEnableWnd() = 0;
	//关闭窗口
	virtual void __cdecl CloseWnd() = 0;
	//设置SocketID
	virtual void __cdecl SetMeSocketID(WORD wSocketID) = 0;
	//获取自己信息
	virtual const tagUserData* __cdecl GetMeData() = 0;
	//网络Socket游戏主框架消息
	virtual bool __cdecl OnSocketMainGameFrame(CMD_Command Command, void * pBuffer, WORD wDataSize) = 0;
	//用户进入
	virtual void __cdecl OnEventUserEnter(const tagUserData* pUserData) = 0;
	//用户离开
	virtual void __cdecl OnEventUserLeave() = 0;
	//用户积分
	virtual void __cdecl OnEventUserScore(const IPC_UserScore* pUserScore) = 0;
	//用户状态
	virtual void __cdecl OnEventUserStatus(WORD wTableID, WORD wChairID, BYTE cbUserStatus) = 0;
};

#endif