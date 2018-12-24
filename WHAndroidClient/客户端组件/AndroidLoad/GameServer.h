#pragma once

#include "stdafx.h"
#include "afxcmn.h"
#include "IGamePlaza.h"
#include "GameUserInfo.h"

class CFlashEnterDlg;


//游戏服务器
class CGameServer : public CWnd, public IViewItem, public ITCPSocketSink
{
	DECLARE_DYNAMIC(CGameServer)

public:
	CGameServer();
	virtual ~CGameServer();

protected:
	DECLARE_MESSAGE_MAP()

public:
	//释放对象
	virtual void __cdecl Release() { delete this; }
	//接口查询
	virtual void * __cdecl QueryInterface(const IID & Guid, DWORD dwQueryVer);
public:
	//获取句柄
	virtual HWND __cdecl GetWindowHandle() { return GetSafeHwnd(); }
	//网络事件
public:
	//连接事件
	virtual bool __cdecl OnEventTCPSocketLink(WORD wSocketID, INT nErrorCode);
	//关闭事件
	virtual bool __cdecl OnEventTCPSocketShut(WORD wSocketID, BYTE cbShutReason);
	//读取事件
	virtual bool __cdecl OnEventTCPSocketRead(WORD wSocketID, CMD_Command Command, VOID * pData, WORD wDataSize);

public:
	//登录消息
	bool OnSocketMainLogon(WORD wSocketID, CMD_Command Command, void * pBuffer, WORD wDataSize);
	//用户消息
	bool OnSocketMainUser(WORD wSocketID, CMD_Command Command, void * pBuffer, WORD wDataSize);
	//配置消息
	bool OnSocketMainInfo(WORD wSocketID, CMD_Command Command, void * pBuffer, WORD wDataSize);
	//状态消息
	bool OnSocketMainStatus(WORD wSocketID, CMD_Command Command, void * pBuffer, WORD wDataSize);
	//系统消息
	bool OnSocketMainSystem(WORD wSocketID, CMD_Command Command, void * pBuffer, WORD wDataSize);
	//房间消息
	bool OnSocketMainServerInfo(WORD wSocketID, CMD_Command Command, void * pBuffer, WORD wDataSize);
	//游戏消息
	bool OnSocketMainGameFrame(WORD wSocketID, CMD_Command Command, void * pBuffer, WORD wDataSize);

public:
	//用户进入
	bool OnSocketSubUserCome(strUserGameInfo* pUserGameInfo,  void * pBuffer, WORD wDataSize);
	//用户状态
	bool OnSocketSubStatus(strUserGameInfo* pUserGameInfo,  void * pBuffer, WORD wDataSize);
	//用户分数
	bool OnSocketSubScore(strUserGameInfo* pUserGameInfo,  void * pBuffer, WORD wDataSize);

public:
	//设置主窗口指针
	void SetMainWnd(CFlashEnterDlg* pMainWnd);
	//连接游戏服务器之前初始化数据
	bool InitGameLocalData();
	//连接到游戏服务器
	void OnConnectGameServer();
	//返回相应值
	void GetLogonInfo(DWORD &dwLogonSucNums, DWORD &dwLogonFailNums, DWORD &dwTCPCoonectNums, DWORD &dwUserConnectNums);
	void GetQueueInfo(DWORD &dwQueueSucNums, DWORD &dwQueueFailNums);
	//退出游戏服务器
	bool OnEventExitServer();
	//登录至游戏服务器
	void UserLogonToGameServer(strUserGameInfo* pUserInfo);
	//所有玩家排队
	void SetAllUserQueue();
	//开始分配服务器IP和端口号
	bool StartAllotServerIPAndPort();
	//玩家宝石不够,不能登录游戏服务器
	bool UserCanLogonGame(LONG lRestrictScore);
	
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnTimer(UINT nIDEvent);

private:
	//主窗口句柄
	CFlashEnterDlg*		m_pMainWnd;
	//所有用户socket信息
	CGameUserInfo*		m_pGameUserInfo;
	//登录游戏信息
	strLogonInfo		m_LogonInfo;
	//自动排队成功人数
	DWORD				m_dwQueueSuccNums;
	//排队失败人数
	DWORD				m_dwQueueFailNums;

};


