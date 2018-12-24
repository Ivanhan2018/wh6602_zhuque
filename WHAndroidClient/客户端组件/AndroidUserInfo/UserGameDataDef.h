#ifndef		___DEF_USER_GAME_DATA_INCL__1342_CREATE_DEF__
#define		___DEF_USER_GAME_DATA_INCL__1342_CREATE_DEF__

#include <vector>
using namespace std;

//总共连接次数
const int I_CONNECT_NUMS = 5;

//连接信息
struct strLogonInfo 
{
	DWORD		dwLogonSucNums;			//登录成功用户数
	DWORD		dwLogonFailNums;		//登录失败用户数
	DWORD		dwTCPConnectNums;		//TCP重连数
	DWORD		dwUserConnectNums;		//User重连数
};


//-------------------------------------------------------
//TCP连接状态
#define		TAG_LINK_FAIL		0		//失败
#define		TAG_LINK_SUC		1		//成功
#define		TAG_LINKING			2		//正在连接...
//-------------------------------------------------------


//--------------------------------------------------------------------------------
//玩家连接状态
#define		USER_LOGON_SERVER_FAIL				0		//失败
#define		USER_LOGON_SERVER_SUC				1		//成功
#define		USER_LOGON_SERVER_LINKING			2		//正在连接...
#define		USER_AUTO_QUEUE_SERVER_LINKING		3		//正在排队...
//--------------------------------------------------------------------------------

//数值
#define		DEF_NUM_VALUE						0.9


//游戏配置信息
struct GameOption
{
	char	chUserID[32];
	char	chUserPassWord[32];			//明码
	char	chUserAddPassWord[32];		//加密后明码
	DWORD	dwGameID;
	DWORD	dwRoomID;
	DWORD	dwUserNums;
	char	chServerIP[32];
	DWORD	dwServerPort;
	DWORD	dwSockStartID;
	DWORD	dwUserIndex;
	char	chGameServerIP[32];
	DWORD	dwGameServerPort;

	GameOption()
	{
		::memset(chUserID, 0, sizeof(chUserID));
		::memset(chUserPassWord, 0, sizeof(chUserPassWord));
		::memset(chUserAddPassWord, 0, sizeof(chUserAddPassWord));
		::memset(chServerIP, 0, sizeof(chServerIP));
		dwGameID = 0;
		dwRoomID = 0;
		dwUserNums = 0;
		dwServerPort = 0;
		dwUserIndex = 0;
		dwSockStartID = 2000;
		::memset(chGameServerIP, 0, sizeof(chGameServerIP));
		dwGameServerPort=0;
	}
};



class IGameClientFrame;

//游戏用户信息
struct strUserGameInfo 
{
	char						chName[32];		//username
	DWORD						dwUserID;		//userid
	DWORD						dwKindID;		//game kindid
	DWORD						dwSocketID;		//socketid	,SOCKET标识
	DWORD						dwGameServerIP;		//gameserverip
	DWORD						dwGameServerport;	//gameserverport
	DWORD						dwLogonLinkState;	//登录服务器连接状态
	DWORD						dwGameLinkState;	//游戏服务器连接状态
	DWORD						dwUserLogonState;	//玩家登录状态
	DWORD						dwUserGameState;	//玩家登录游戏状态
	LONGLONG					llUserGems;			//玩家宝石
	DWORD						dwCountConnect;		//连接次数
	CTCPSocketHelper			TcpSocketHelper;	//sockethelper
	IGameClientFrame*			pIGameClient;		//游戏客户端

	strUserGameInfo()
	{
		::memset(chName, 0, sizeof(chName));
		dwKindID = 0;
		dwUserID = 0;
		dwSocketID = 0;
		dwGameServerIP = 0;
		dwGameServerport = 0;
		llUserGems = 0;
		dwCountConnect = 0;
		dwLogonLinkState = TAG_LINK_FAIL;
		dwLogonLinkState = TAG_LINK_FAIL;
		dwUserLogonState = USER_LOGON_SERVER_FAIL;
		dwUserGameState = USER_LOGON_SERVER_FAIL;
	}
};



//游戏房间信息
struct Local_GR_ServerInfo
{
	//房间属性
	WORD							wKindID;							//类型 I D
	WORD							wTableCount;						//桌子数目
	WORD							wChairCount;						//椅子数目
	//扩展配置
	WORD							wGameGenre;							//游戏类型
	bool							bIsInit;							//是否初始化
};


//游戏房间列表
struct GameServerList
{
	WORD						wSortID;			//房间标识
	WORD						wKindID;			//游戏类型
	DWORD						dwGameServerIP;		//gameserverip
	DWORD						dwGameServerport;	//gameserverport
	DWORD						dwOnLineCount;		//onLineCount
	DWORD						dwMaxCount;			//MaxCount
	LONG                        lRestrictScore;		//限制积分
};


#endif