#ifndef		__DEF_INCLUDE_GAME_USER_INFO_INSTANCE_H___
#define		__DEF_INCLUDE_GAME_USER_INFO_INSTANCE_H___

#include "stdafx.h"
#include "AndroidUserInfoEx.h"
#include "IGameClientFrame.h"
#include "UserGameDataDef.h"
#include <map>

using namespace std;


typedef IGameClientFrame* NewGameClient(CWnd* pParent, DWORD dwWndID);
typedef  void FreeGameClient(IGameClientFrame* pClient);


 class ANDROID_USER_INFO_EX  CGameUserInfo
{

public:
	~CGameUserInfo(void);

public:
	//获得唯一实例
	static CGameUserInfo* GetInstance();

public:
	//加载DLL
	bool LoadGameClientDll(const std::string& strDllName);
	//初始化信息
	WORD InitGameUserInfo();
	//释放初始化信息
	bool FreeGameUserInfo();
	//设置组件回调关联
	bool SetIUnknownEx(BYTE byTag, IUnknownEx * pIUnknownEx);
	//所有玩家退出游戏
	void AllUserExitGame();
	//关闭所有socket
	void CloseAllSocket();
	//清空并退出服务器
	void ClearGameUserInfo();

public:
	//初始化用户共享数据
	void InitGameUserShareData();
	//获得房间信息
	const Local_GR_ServerInfo* GetLocalRoomInfo();
	//设置房间信息
	void SetLocalRoomInfo(WORD wGameGenre, WORD wKindID, WORD wTableCount, WORD wChairCount);

public:
	//玩家信息数组
	std::map<DWORD, strUserGameInfo*>		m_vecUserGameInfo;

protected:
	CGameUserInfo(void);
	
private:
	//游戏客户端DLL实例
	HINSTANCE	m_hGameDllInstance;
	//创建游戏客户端指针
	NewGameClient* m_ptrNewGameClient;
	//释放游戏客户端指针
	FreeGameClient* m_ptrFreeGameClient;
	//实例
	static CGameUserInfo*	m_Instance;
	//房间信息
	Local_GR_ServerInfo		m_GameServerInfo;

};


#endif