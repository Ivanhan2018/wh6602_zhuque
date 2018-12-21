#ifndef CMD_CENTER_HEAD_FILE
#define CMD_CENTER_HEAD_FILE

//
//游戏列表命令码
//

#define MDM_CS_SERVER_LIST				1								//列表信息

#define SUB_CS_GET_SERVER_LIST			1								//请求列表

#define SUB_CS_LIST_INFO				100								//列表信息
#define SUB_CS_LIST_TYPE				101								//类型列表
#define SUB_CS_LIST_KIND				102								//种类列表
#define SUB_CS_LIST_STATION				103								//站点列表
#define SUB_CS_LIST_SERVER				104								//房间列表
#define SUB_CS_LIST_FINISH				105								//列表完成

//列表信息
struct CMD_CS_ListInfo
{
	DWORD								dwTypeCount;					//类型数目
	DWORD								dwKindCount;					//种类数目
	DWORD								dwServerCount;					//房间数目
	DWORD								dwStationCount;					//站点数目
};

//
//房间管理命令码
//

#define MDM_CS_SERVER_MANAGER			2								//房间管理

#define SUB_CS_REG_GAME_SERVER			1								//注册房间
#define SUB_CS_UNREG_GAME_SERVER		2								//注销房间
#define SUB_CS_SERVER_ONLINE_COUNT		3								//房间人数
#define SUB_CS_CREATE_SELF_REQ			4								//请求创建自建场
#define SUB_CS_CREATE_SELF_REP			5								//创建自建场结果
#define SUB_CS_QUERY_SELF_REQ			6								//请求查询自建场
#define SUB_CS_QUERY_SELF_REP			7								//查询自建场结果
#define SUB_CS_CHECK_PWD_SELF_REQ		8								//请求自建场校验密码
#define SUB_CS_CHECK_PWD_SELF_REP		9								//自建场校验密码结果
#define SUB_CS_SPEAKER_SEND_MSG_REQ		10								//发送喇叭消息
#define SUB_CS_CREATE_FRIEND_REQ		11								//请求创建好友房
#define SUB_CS_CREATE_FRIEND_REP		12								//创建好友房结果
#define SUB_CS_ENTER_FRIEND_REQ			13								//请求进入好友房
#define SUB_CS_ENTER_FRIEND_REP			14								//进入好友房结果
#define SUB_CS_QUERY_FRIEND_ROOMID_REQ			15						//查询好友房
#define SUB_CS_QUERY_FRIEND_ROOMID_REP			16						//查询好友房结果


//注册房间
struct CMD_CS_RegGameServer
{
	tagGameServer						GameServer;						//房间信息
};

//注销房间
struct CMD_CS_UnRegGameServer
{
	WORD								wKindID;						//名称号码
	WORD								wServerID;						//房间号码
};

//房间人数
struct CMD_CS_ServerOnLineCount
{
	WORD								wKindID;						//名称号码
	WORD								wServerID;						//房间号码
	DWORD								dwOnLineCount;					//在线人数
};
//请求查询好友房
struct CMD_CS_QueryFriend
{
	DWORD								dwSocketID;					//LoginServer用来标识客户端
	DWORD								dwSocketIDLogin;			//CenterServer用来标识LoginServer
	char								cType;						//查询发起的原因。1：因为自动登录引发
	WORD								wKindID;					//kindid
	DWORD								dwUserID;					//用户
	DWORD								dwRoomID;					//房间ID
	char								szRule[512];				//配置参数:名值对，不建议太长
};

//查询好友房结果
struct CMD_CS_QueryFriendRep
{
	DWORD								dwSocketID;					//LoginServer用来标识客户端
	DWORD								dwSocketIDLogin;			//CenterServer用来标识LoginServer
	char								cType;						//查询发起的原因。1：因为自动登录引发
	WORD								wKindID;					//kindid
	char								nCode;						//结果
	DWORD								dwUserID;					//用户
	DWORD								dwUserIDFangZhu;			//房主用户
	DWORD								dwRoomID;					//房间ID
	WORD								wServerID;					//游戏ID
	WORD								wTableID;					//桌子ID
	char								szRule[512];				//配置参数:名值对，不建议太长
};

//请求创建好友房
struct CMD_CS_CreateFriend
{
	DWORD								dwSocketID;					//LoginServer用来标识客户端
	DWORD								dwSocketIDLogin;			//CenterServer用来标识LoginServer
	DWORD								dwUserID;					//用户
	WORD								wKindID;					//游戏类型
	DWORD								dwRoomID;					//由中心服务器分配
	char								szRule[512];				//配置参数:名值对，不建议太长
};

//创建好友房结果
struct CMD_CS_CreateFriendRep
{
	DWORD								dwSocketID;					//LoginServer用来标识客户端
	DWORD								dwSocketIDLogin;			//CenterServer用来标识LoginServer
	unsigned short						nCode;						//结果
	DWORD								dwUserID;					//用户
	DWORD								nRoomID;					//RoomID
	unsigned short						nServerID;					//房间ID
	unsigned short						nTableID;					//桌子ID
	char								szName[NAME_LEN];			//房间名称
	char								szRoomID[NAME_LEN];			//房间唯一ID
	char								szRule[512];				//配置参数:名值对，不建议太长
};

//请求进入好友房
struct CMD_CS_EnterFriend
{
	DWORD								dwSocketID;					//LoginServer用来标识客户端
	DWORD								dwSocketIDLogin;			//CenterServer用来标识LoginServer
	char								cType;						//查询发起的原因。1：因为自动登录引发
	DWORD								dwUserID;					//用户
	unsigned short						nServerID;					//房间ID
	char								szRoomID[NAME_LEN];			//房间唯一ID
	char								szPassword[PASS_LEN];		//密码
};

//进入好友房结果
struct CMD_CS_EnterFriendRep
{
	DWORD								dwSocketID;					//LoginServer用来标识客户端
	DWORD								dwSocketIDLogin;			//CenterServer用来标识LoginServer
	char								cType;						//查询发起的原因。1：因为自动登录引发
	unsigned short						nCode;						//结果
	DWORD								dwUserID;					//用户
	unsigned short						nServerID;					//房间ID
	unsigned short						nTableID;					//桌号
	char								szRoomID[NAME_LEN];			//房间唯一ID
	char								szRule[512];				//配置参数:名值对，不建议太长
};

//请求创建自建场
struct CMD_CS_CreateSelf
{
	DWORD								dwSocketID;					//LoginServer用来标识客户端
	DWORD								dwSocketIDLogin;			//CenterServer用来标识LoginServer
	unsigned short						nServerID;					//房间ID
	char								szName[NAME_LEN];			//房间名称
	char								szPassword[PASS_LEN];		//密码
	char								szRule[128];				//配置参数:名值对，不建议太长
};

//创建自建场结果
struct CMD_CS_CreateSelfRep
{
	DWORD								dwSocketID;					//LoginServer用来标识客户端
	DWORD								dwSocketIDLogin;			//CenterServer用来标识LoginServer
	unsigned short						nCode;						//结果
	unsigned short						nServerID;					//房间ID
	char								szName[NAME_LEN];			//房间名称
	char								szRoomID[NAME_LEN];			//房间唯一ID
	char								szRule[128];				//配置参数:名值对，不建议太长
};

//请求校验房间密码
struct CMD_CS_CheckPwdSelf
{
	DWORD								dwSocketID;					//LoginServer用来标识客户端
	DWORD								dwSocketIDLogin;			//CenterServer用来标识LoginServer
	unsigned short						nServerID;					//房间ID
	char								szRoomID[NAME_LEN];			//房间唯一ID
	char								szPassword[PASS_LEN];		//密码
};

//请求校验房间密码结果
struct CMD_CS_CheckPwdSelfRep
{
	DWORD								dwSocketID;					//LoginServer用来标识客户端
	DWORD								dwSocketIDLogin;			//CenterServer用来标识LoginServer
	unsigned short						nCode;						//结果
	unsigned short						nServerID;					//房间ID
	char								szRoomID[NAME_LEN];			//房间唯一ID
};

//请求查询自建场
struct CMD_CS_QuerySelf
{
	DWORD								dwSocketID;					//LoginServer用来标识客户端
	DWORD								dwSocketIDLogin;			//CenterServer用来标识LoginServer
	unsigned short						nServerID;					//游戏ID
	unsigned short						nPage;						//分页数，从0开始，每页的条数，由服务端控制
	char								szName[NAME_LEN];			//房间名称：模糊查询条件
	char								szRoomID[NAME_LEN];			//房间编号：模糊查询条件
	char								szFilter[128];				//扩展的查询参数:名值对，不建议太长
};

//查询自建场结果
struct CMD_CS_QuerySelfRep
{
	DWORD								dwSocketID;					//LoginServer用来标识客户端
	DWORD								dwSocketIDLogin;			//CenterServer用来标识LoginServer
	unsigned short						nCode;						//结果
	unsigned short						nServerID;					//房间ID
	unsigned short						nPage;						//页码
	unsigned short						nTotal;						//总的记录条数
	unsigned short						nCount;						//返回的实际记录条数，最多不能超过10个
	char								szName[10][NAME_LEN];		//房间名称。独立出来，因为可能有保留字符
	char								szExtend[640];				//附加参数，格式：id:1,c:100,m:3000,x:20000,k:1|id:2,m:100,x:20000|id:5,m:200,x:20000,k:1
																	//id表示桌子标识，c为单元积分，m为最小财富，x为最大财富，k为是否有密码
};

//请求发送喇叭消息
struct CMD_CS_SpeakerSendMsgReq
{
	unsigned short						nType;					//消息类型
	unsigned short						nLen;					//消息长度
	char								szMsg[256];				//消息内容
};

//
//控制命令码
//

#define MDM_CS_CONTROL_COMMAND			3								//控制命令
#define SUB_CS_CONTROL_COMMAND_REQ		1								//命令请求
#define SUB_CS_CONTROL_COMMAND_REP		2								//命令响应

struct CMD_CS_ControlCommandReq
{
	WORD								wCommandID;						//命令字
	DWORD								dwServerID;						//房间ID
	DWORD								dwUserID;						//用户
	TCHAR								szParam[128];					//参数
};

struct CMD_CS_ControlCommandRep
{
	WORD								wCommandID;						//命令字
	WORD								wCode;							//返回状态
	DWORD								dwServerID;						//房间ID
	DWORD								dwUserID;						//用户
	TCHAR								szResult[128];					//描述
};

#endif


//-----------------------------------------------
//					the end
//-----------------------------------------------
