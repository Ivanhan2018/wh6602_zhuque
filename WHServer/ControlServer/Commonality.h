#ifndef COMMONALITY_HEAD_FILE
#define COMMONALITY_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include <vector>
using std::vector;

//////////////////////////////////////////////////////////////////////////
//数据库对象定义

//数据库请求标识
#define DBR_GP_CHECK_GM_TASK			100								//检查GM任务
#define DBR_GP_CHECK_GM_TASK_RESULT		101								//检查GM任务的结果
#define DBR_GP_CREATE_RANKING			102								//生成排行榜
#define DBR_GP_WRITE_ONLINE				103								//写最高在线
#define DBR_GP_CHECK_SPEAKER_TASK_RESULT		104						//检查系统喇叭任务的结果
#define DBR_GP_SPEAKER_SEND				105								//系统喇叭入库

struct DBR_GR_Online
{
	DWORD							dwTypeID;							//类型
	DWORD							dwValue;							//值
	TCHAR							szCollectDate[32];					//时间点
};

struct DBR_GP_Speaker_Task_Result
{
	DWORD							dwUserID;							//用户ID
	short							nLen;								//长度
	TCHAR							szMsg[256];							//内容
};

//发喇叭请求
struct DBR_GP_SpeakerSend
{
	short							iType;					//喇叭类型：1用户消息，2系统消息
	DWORD							dwUserID;				//玩家 ID
	DWORD							nLen;					
	TCHAR							szDate[256];					//喇叭内容
};

//////////////////////////////////////////////////////////////////////////

#endif

