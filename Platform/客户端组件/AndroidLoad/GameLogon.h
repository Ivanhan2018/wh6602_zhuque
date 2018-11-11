#pragma once

#include "stdafx.h"
#include "afxcmn.h"
#include "IGamePlaza.h"
#include "GameUserInfo.h"


class CFlashEnterDlg;

//登录服务器
class CGameLogon : public CWnd, public IViewItem, public ITCPSocketSink
{
	DECLARE_DYNAMIC(CGameLogon)

public:
	CGameLogon();
	virtual ~CGameLogon();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

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
	bool OnSocketMainLogon(WORD wSocketID, CMD_Command Command, void * pData, WORD wDataSize);
	//列表消息
	bool OnSocketMainServerList(WORD wSocketID, CMD_Command Command, void * pBuffer, WORD wDataSize);

public:
	//设置主窗口指针
	void SetMainWnd(CFlashEnterDlg* pMainWnd);
	//连接登录服务器
	bool OnEventConnectServer();
	//退出登录服务器
	bool OnEventExitServer();
	//返回相应值
	void GetLogonInfo(DWORD &dwLogonSucNums, DWORD &dwLogonFailNums, DWORD &dwTCPCoonectNums, DWORD &dwUserConnectNums);
	//登录至登录服务器
	void UserLogonToLogonServer(strUserGameInfo* pUserInfo);

private:
	CFlashEnterDlg*		m_pMainWnd;
	CGameUserInfo*		m_pGameUserInfo;
	strLogonInfo		m_LogonInfo;
	
public:
	afx_msg void OnTimer(UINT nIDEvent);
};


