#pragma once
#include "stdafx.h"
#include "afxcmn.h"
#include "AndroidGameFrameEx.h"
#include "IGameClientFrame.h"

// CGameClient
//游戏客户端框架
class ANDROID_GAME_FRAME_EX CGameClientFrame : public CWnd,  public IGameClientFrame
{
	DECLARE_DYNAMIC(CGameClientFrame)

public:
	CGameClientFrame();
	virtual ~CGameClientFrame();

protected:
	DECLARE_MESSAGE_MAP()

public:
	//初始函数
	virtual bool InitGameFrame()=NULL;
	//重置框架
	virtual void ResetGameFrame()=NULL;

public:
	//设置SocketID
	virtual void __cdecl SetMeSocketID(WORD wSocketID);
	//是否窗口
	virtual bool __cdecl IsEnableWnd();
	//关闭窗口
	virtual void __cdecl CloseWnd();
	//获得自己信息
	virtual const tagUserData* __cdecl GetMeData();

public:
	//游戏框架数据
	virtual bool __cdecl OnSocketMainGameFrame(CMD_Command Command, void * pBuffer, WORD wDataSize);

	//系统事件
public:
	//时间消息
	virtual bool OnTimerMessage(UINT nElapse, UINT nTimerID)=NULL;
	//网络消息
	virtual bool OnGameMessage(WORD wSubCmdID, const void * pBuffer, WORD wDataSize)=NULL;
	//游戏场景
	virtual bool OnGameSceneMessage(BYTE cbGameStatus, const void * pBuffer, WORD wDataSize)=NULL;

public:
	//事件Socket
	virtual bool OnEventSocket(CMD_Command Command, void * pBuffer, WORD wDataSize);
	//框架消息
	virtual bool OnFrameMessage(WORD wSubCmdID, const void * pBuffer, WORD wDataSize);

	//用户事件
public:
	//用户进入
	virtual void __cdecl OnEventUserEnter(const tagUserData* pUserData);
	//用户离开
	virtual void __cdecl OnEventUserLeave();
	//用户积分
	virtual void __cdecl OnEventUserScore(const IPC_UserScore* pUserScore);
	//用户状态
	virtual void __cdecl OnEventUserStatus(WORD wTableID, WORD wChairID, BYTE cbUserStatus);

public:
	//自己位置
	WORD GetMeChairID();
	//发送函数
	bool SendData(WORD wSubCmdID);
	//发送函数
	bool SendData(WORD wSubCmdID, void * pData, WORD wDataSize);
	//发送函数
	bool SendData(WORD wMainCmdID, WORD wSubCmdID);
	//发送函数
	bool SendData(WORD wMainCmdID, WORD wSubCmdID, void * pData, WORD wDataSize);
	//发送准备
	bool SendUserReady(void * pBuffer, WORD wDataSize);
	//设置定时器
	bool SetGameTimer(UINT nTimerID, UINT nElapse);
	//删除定时器
	bool KillGameTimer(UINT nTimerID);
	
	//游戏状态
public:
	//游戏状态
	BYTE GetGameStatus();
	//游戏状态
	void SetGameStatus(BYTE bGameStatus);

protected:
	//自己信息
	tagUserData			m_MeUserInfo;

private:
	//保存自己的Socket标识
	WORD				m_wSocketID;

	//时间检测
	DWORD				m_wTimeID;
	DWORD				m_nElapseCount;

	//游戏状态
	BYTE				m_bGameStatus;

public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnTimer(UINT nIDEvent);
};


