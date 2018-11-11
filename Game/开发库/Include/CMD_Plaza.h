#ifndef CMD_PLAZA_HEAD_FILE
#define CMD_PLAZA_HEAD_FILE

//////////////////////////////////////////////////////////////////////////

//广场版本
#define VER_PLAZA_LOW					0								//广场版本
#define VER_PLAZA_HIGH					1								//广场版本
#define VER_PLAZA_FRAME					MAKELONG(VER_PLAZA_LOW,VER_PLAZA_HIGH)

//////////////////////////////////////////////////////////////////////////
//登录错误标识

#define ERC_GP_LOGON_SUCCESS			0								//登陆成功
#define ERC_GP_ACCOUNTS_NOT_EXIST		1								//帐号不存在
#define ERC_GP_LONG_NULLITY				2								//禁止登录
#define ERC_GP_PASSWORD_ERCOR			3								//密码错误

//////////////////////////////////////////////////////////////////////////
//登录命令码

#define MDM_GP_LOGON					1								//广场登录

#define SUB_GP_LOGON_ACCOUNTS			1								//帐号登录
#define SUB_GP_LOGON_USERID				2								//I D 登录
#define SUB_GP_REGISTER_ACCOUNTS		3								//注册帐号
#define SUB_GP_UPLOAD_CUSTOM_FACE		4								//定义头像
#define SUB_GP_LOGON_USER_GRADE_INFO    6                               //等级信息     彭文添加

#define SUB_GP_LOGON_SUCCESS			100								//登陆成功
#define SUB_GP_LOGON_ERROR				101								//登陆失败
#define SUB_GP_LOGON_FINISH				102								//登陆完成

//帐号登录
struct CMD_GP_LogonByAccounts
{
	CMD_GP_LogonByAccounts()
	{
			memset(this, 0, sizeof(*this));
	}
	DWORD								dwPlazaVersion;					//广场版本
	TCHAR								szAccounts[NAME_LEN];			//登录帐号
	TCHAR								szPassWord[PASS_LEN];			//登录密码
};

//I D 登录
struct CMD_GP_LogonByUserID
{
	CMD_GP_LogonByUserID()
	{
			memset(this, 0, sizeof(*this));
	}
	DWORD								dwPlazaVersion;					//广场版本
	DWORD								dwUserID;						//用户 I D
	TCHAR								szPassWord[PASS_LEN];			//登录密码
};

//用户信息查询结构                                                      //彭文添加
struct CMD_GP_UpdateUserInfo
{
	CMD_GP_UpdateUserInfo()
	{
			memset(this, 0, sizeof(*this));
	}
	DWORD								dwUserID;						//用户 I D
	TCHAR								szPassWord[PASS_LEN];			//登录密码
};

//免费申请
struct CMD_GP_GiftGold
{
    CMD_GP_GiftGold()
	{
			memset(this, 0, sizeof(*this));
	}
	DWORD								dwUserID;						//用户 I D
	TCHAR								szPassWord[PASS_LEN];			//登录密码
	DWORD                               dwClientIP;                 //用户ip
};

// 检查线路
struct CMD_GP_CheckLine
{
	CMD_GP_CheckLine()
	{
		memset(this, 0, sizeof(*this));
	}

	DWORD								dwUserID;						//用户 I D
	DWORD								dwClientIP;						//用户ip
};

// 删除机器码
struct CMD_GP_DeleteMachineCode
{
	CMD_GP_DeleteMachineCode()
	{
		memset(this, 0, sizeof(*this));
	}

	tagClientSerial						clientSerial;
	TCHAR								szAccounts[NAME_LEN];			//登录帐号
};

//免费申请
struct CMD_GP_GiftGoldResult
{
    CMD_GP_GiftGoldResult()
	{
			memset(this, 0, sizeof(*this));
	}
	LONG                                 lRet;            //是否申请成功
	DWORD                                dwGold;          //赠送的金币数量
	TCHAR								 szDescribe[64];  //提示信息
};



//用户信息更新成功结构
struct CMD_GP_UserInfoSucc                                              //彭文添加
{
	CMD_GP_UserInfoSucc()
	{
			memset(this, 0, sizeof(*this));
	}
	WORD								wFaceID;						//头像索引
	BYTE								cbGender;						//用户性
	TCHAR								szNickName[NAME_LEN];	        //昵称
	LONGLONG							lMoney;							//游戏金币
	LONGLONG                            lGold;                          //用户的金币 
	LONGLONG                            lGem;                           //用户的宝石 
};

//注册帐号
struct CMD_GP_RegisterAccounts
{
	CMD_GP_RegisterAccounts()
	{
			memset(this, 0, sizeof(*this));
	}
	WORD								wFaceID;						//头像标识
	BYTE								cbGender;						//用户性别
	DWORD								dwPlazaVersion;					//广场版本
	TCHAR								szSpreader[NAME_LEN];			//推广人名
	TCHAR								szAccounts[NAME_LEN];			//登录帐号
	TCHAR								szPassWord[PASS_LEN];			//登录密码
	TCHAR                               szEmail[EMAIL_LEN];             //email长度  彭文添加
};

enum USER_TYPE {PERSON=0, ANDROID=1 };
//登陆成功
struct CMD_GP_LogonSuccess
{
	CMD_GP_LogonSuccess()
	{
			memset(this, 0, sizeof(*this));
	}

	WORD								wFaceID;						//头像索引
	BYTE								cbGender;						//用户性别
	BYTE								cbMember;						//会员等级
	DWORD								dwUserID;						//用户 I D
	DWORD								dwGameID;						//游戏 I D
	DWORD								dwExperience;					//用户经验
	
	//扩展信息
	DWORD								dwCustomFaceVer;				//头像版本
	BYTE								cbMoorMachine;					//锁定机器
	DWORD                               dwLockServerID;                 //被锁的服务器id,不为0就是被锁了         彭文添加




	//彭文添加
	TCHAR								szNickName[NAME_LEN];	            //昵称
	LONGLONG							lMoney;								//游戏金币
	LONGLONG                            lGold;                              //用户的金币 2011-9-30前为宝石 lGems
	LONGLONG                            lGem;                              //用户的宝石 2011-9-30前为荣誉 lHonor
	DWORD                               dwGrade;                           //用户的等级（不超过256） 2011-9-30前为dwClass

	LONG								lWinCount;							//胜利盘数
	LONG								lLostCount;							//失败盘数
	LONG								lDrawCount;							//和局盘数
	LONG								lFleeCount;							//断线数目

	USER_TYPE                           utIsAndroid;						//是否是机器人
	TCHAR                               szKey[33];							//网站用的密钥
	LONG                                lGiftNum;							//领取赠送的次数（目前是一天一次）


	//添加结束
	// add by HouGuoJiang 2011-11-25
	TCHAR				szHashID[33];		// 碎片升级用的版本号
	TCHAR				szReserve[48];		// 保留，日后扩展使用，就不需要每个玩家更新大厅了，若用扩充大容量的内容，就单独发消息好了

	int									nMasterRight;						//权限管理
	int									nMasterOrder;						//权限等级
};

//彭文添加
//用户级公告
//struct CMD_GP_UserPlacard
//{
//	DWORD                       dwUserId; //用户id
//	DWORD                       dwIndex; //序号
//	TCHAR                       szTitle[USER_PLACARD_TITLE_LEN]; //标题
//	TCHAR                       szDate[USER_PLACARD_TITLE_LEN];   //消息时间
//	TCHAR                       szContex[USER_PLACARD_CONTENT_LEN] ; //内容
//};

//用户等级信息
struct CMD_GP_UserGradeInfo 
{
	CMD_GP_UserGradeInfo()
	{
			memset(this, 0, sizeof(*this));
	}
	TCHAR strName[24];
	int   iMaxExp;
};


//添加结束


//登陆失败
struct CMD_GP_LogonError
{
	CMD_GP_LogonError()
	{
		memset(this, 0, sizeof(*this));
	}
	LONG								lErrorCode;						//错误代码
	TCHAR								szErrorDescribe[128];			//错误消息
};

//////////////////////////////////////////////////////////////////////////
//游戏列表命令码

#define MDM_GP_SERVER_LIST				2								//列表信息

#define SUB_GP_LIST_TYPE				100								//类型列表
#define SUB_GP_LIST_KIND				101								//种类列表
#define SUB_GP_LIST_STATION				102								//站点列表
#define SUB_GP_LIST_SERVER				103								//房间列表
#define SUB_GP_LIST_FINISH				104								//发送完成
#define SUB_GP_LIST_CONFIG				105								//列表配置

//列表配置
struct CMD_GP_ListConfig
{
	CMD_GP_ListConfig()
	{
		memset(this, 0, sizeof(*this));
	}
	BYTE								bShowOnLineCount;				//显示人数
};

//////////////////////////////////////////////////////////////////////////
//系统命令码

#define MDM_GP_SYSTEM					3								//系统命令

#define SUB_GP_VERSION					100								//版本通知
#define SUB_SP_SYSTEM_MSG				101								//系统消息

//版本通知
struct CMD_GP_Version
{
	CMD_GP_Version()
	{
		memset(this, 0, sizeof(*this));
	}
	BYTE								bNewVersion;					//更新版本
	BYTE								bAllowConnect;					//允许连接
};

//////////////////////////////////////////////////////////////////////////

#define MDM_GP_USER						4								//用户信息

#define SUB_GP_USER_UPLOAD_FACE			100								//上传头像
#define SUB_GP_USER_DOWNLOAD_FACE		101								//下载头像
#define SUB_GP_UPLOAD_FACE_RESULT		102								//上传结果
#define SUB_GP_DELETE_FACE_RESULT		103								//删除结果
#define SUB_GP_CUSTOM_FACE_DELETE		104								//删除头像
#define SUB_GP_MODIFY_INDIVIDUAL		105								//个人资料
#define SUB_GP_MODIFY_INDIVIDUAL_RESULT	106								//修改结果

#define SUB_GP_SAFE_BIND				107								//玩家绑定
#define SUB_GP_SAFE_UNBIND				108								//解除绑定

#define SUB_GP_UPDATE_USERINFO			109								//更新用户信息		彭文添加  
#define SUB_GP_UPDATE_USERINFO_SUCC     120                             //更新用户信息成功  彭文添加  


#define SUB_GP_GET_GIFT_GOLD          130                             //免费领取          彭文添加
#define SUB_GP_GET_GIFT_GOLD_RESULT   131                             //免费领取          彭文添加
#define SUB_GP_CHECK_LINE				132								//检查线路
#define SUB_GP_DELETE_MACHINE_CODE		133								//删除机器码
      
//绑定帐号
struct CMD_GP_SafeBind
{
	CMD_GP_SafeBind()
	{
		memset(this, 0, sizeof(*this));
	}
	DWORD								dwUserID;						//玩家ID
	tagClientSerial						ClientSerial;					//机器序列
	TCHAR								szPassWord[PASS_LEN];			//登录密码
};

//解除绑定
struct CMD_GP_SafUnbind
{
	CMD_GP_SafUnbind()
	{
		memset(this, 0, sizeof(*this));
	}
	DWORD								dwUserID;						//玩家ID
	TCHAR								szPassWord[PASS_LEN];			//登录密码
};

//解除绑定
struct CMD_GP_BindResult
{
	CMD_GP_BindResult()
	{
		memset(this, 0, sizeof(*this));
	}
	BYTE								cbSuccess;						//玩家ID
	//TCHAR								szErrorDescribe[128];			//登录密码
};

//个人资料
struct CMD_GP_ModifyIndividual
{
	CMD_GP_ModifyIndividual()
	{
		memset(this, 0, sizeof(*this));
	}
	DWORD							dwUserID;							//玩家 ID
	TCHAR							szNickname[NAME_LEN];				//玩家昵称
	int								nGender;							//玩家性别
	int								nAge;								//玩家年龄
	TCHAR							szAddress[64];						//玩家地址
	TCHAR							szUnderWrite[32];					//个性签名
	TCHAR							szPassword[33];						//玩家密码
};

//定义头像
struct CMD_GP_UploadCustomFace
{
	CMD_GP_UploadCustomFace()
	{
		memset(this, 0, sizeof(*this));
	}
	DWORD								dwUserID;						//玩家 ID
	WORD								wCurrentSize;					//当前大小
	bool								bLastPacket;					//最后标识
	bool								bFirstPacket;					//第一个标识
	BYTE								bFaceData[2048];				//头像数据
};

//下载成功
struct CMD_GP_DownloadFaceSuccess
{
	CMD_GP_DownloadFaceSuccess()
	{
		memset(this, 0, sizeof(*this));
	}
	DWORD							dwToltalSize;						//总共大小
	DWORD							dwCurrentSize;						//当前大小
	DWORD							dwUserID;							//玩家 ID
	BYTE							bFaceData[2048];					//头像数据
};

//下载头像
struct CMD_GP_DownloadFace
{
	CMD_GP_DownloadFace()
	{
		memset(this, 0, sizeof(*this));
	}
	DWORD							dwUserID;							//玩家 ID
};

//上传结果
struct CMD_GP_UploadFaceResult
{
	CMD_GP_UploadFaceResult()
	{
		memset(this, 0, sizeof(*this));
	}
	TCHAR							szDescribeMsg[128];					//描述信息
	DWORD							dwFaceVer;							//头像版本
	bool							bOperateSuccess;					//成功标识
};

//删除结果
struct CMD_GP_DeleteFaceResult
{
	CMD_GP_DeleteFaceResult()
	{
		memset(this, 0, sizeof(*this));
	}
	TCHAR							szDescribeMsg[128];					//描述信息
	DWORD							dwFaceVer;							//头像版本
	bool							bOperateSuccess;					//成功标识
};

//删除消息
struct CMD_GP_CustomFaceDelete
{
	CMD_GP_CustomFaceDelete()
	{
		memset(this, 0, sizeof(*this));
	}
	DWORD							dwUserID;							//玩家 ID
	DWORD							dwFaceVer;							//头像版本
};

//修改结果
struct CMD_GP_ModifyIndividualResult
{
	CMD_GP_ModifyIndividualResult()
	{
		memset(this, 0, sizeof(*this));
	}
	TCHAR							szDescribeMsg[128];					//描述信息
	bool							bOperateSuccess;					//成功标识
};

//////////////////////////////////////////////////////////////////////////

#endif