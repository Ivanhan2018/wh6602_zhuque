#ifndef VIDEO_SERVICE_HEAD_FILE
#define VIDEO_SERVICE_HEAD_FILE

//////////////////////////////////////////////////////////////////////////



#ifndef  GAME_LOAD_DLL
		#include "..\..\模板库\Template.h"
		#include "..\..\公共文件\GlobalDef.h"
		#include "..\..\消息定义\CMD_Video.h"
		#include "..\..\模板库\Module.h"
		#include "..\..\共享组件\界面控件\SkinControls.h"
		#include "..\..\客户端组件\游戏框架\GameFrame.h"
		#include "..\..\客户端组件\鲜花道具\FlowerEffectcontrol.h"
#else
		#include "Template.h"
		#include "GlobalDef.h"
		#include "CMD_Video.h"
		#include "Module.h"
		#include "SkinControls.h"
		#include "GameFrame.h"
		#include "FlowerEffectcontrol.h"
#endif


//////////////////////////////////////////////////////////////////////////
//公共宏定义

//导出定义
#ifndef VIDEO_SERVICE_CLASS
	#ifdef  VIDEO_SERVICE_DLL
		#define VIDEO_SERVICE_CLASS _declspec(dllexport)
	#else
		#define VIDEO_SERVICE_CLASS _declspec(dllimport)
	#endif
#endif

//模块定义
#ifndef _DEBUG
	#define VIDEO_SERVICE_DLL_NAME	TEXT("VideoService.dll")			//组件 DLL 名字
#else
	#define VIDEO_SERVICE_DLL_NAME	TEXT("VideoServiceD.dll")			//组件 DLL 名字
#endif

//////////////////////////////////////////////////////////////////////////
//结构定义

//连接信息
struct tagVideoNetwordInfo
{
	WORD							wNatPort;							//网络端口
	WORD							wLocalPort;							//网络端口
	DWORD							dwNatAddr;							//网络地址
	DWORD							dwLocalAddr;						//本地地址
};

//////////////////////////////////////////////////////////////////////////

//导出类头文件

#include "VideoServiceControl.h"

//////////////////////////////////////////////////////////////////////////

#endif
