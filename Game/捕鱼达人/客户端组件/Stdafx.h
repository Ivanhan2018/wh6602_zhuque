#pragma once

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN		// 从 Windows 标头中排除不常使用的资料
#endif

// 如果您必须使用下列所指定的平台之前的平台，则修改下面的定义。
// 有关不同平台的相应值的最新信息，请参考 MSDN。
#ifndef WINVER				// 允许使用 Windows 95 和 Windows NT 4 或更高版本的特定功能。
#define WINVER 0x0500		//为 Windows98 和 Windows 2000 及更新版本改变为适当的值。
#endif

#ifndef _WIN32_WINNT		// 允许使用 Windows NT 4 或更高版本的特定功能。
#define _WIN32_WINNT 0x0400		//为 Windows98 和 Windows 2000 及更新版本改变为适当的值。
#endif						

#ifndef _WIN32_WINDOWS		// 允许使用 Windows 98 或更高版本的特定功能。
#define _WIN32_WINDOWS 0x0410 //为 Windows Me 及更新版本改变为适当的值。
#endif

#ifndef _WIN32_IE			// 允许使用 IE 4.0 或更高版本的特定功能。
#define _WIN32_IE 0x0400	//为 IE 5.0 及更新版本改变为适当的值。
#endif

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// 某些 CString 构造函数将是显式的

// 关闭 MFC 对某些常见但经常被安全忽略的警告消息的隐藏
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC 核心和标准组件
#include <afxext.h>         // MFC 扩展
#include <afxdisp.h>        // MFC 自动化类

#include <afxdtctl.h>		// Internet Explorer 4 公共控件的 MFC 支持
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// Windows 公共控件的 MFC 支持
#endif // _AFX_NO_AFXCMN_SUPPORT

//////////////////////////////////////////////////////////////////////////

#include "..\消息定义\CMD_Game.h"
#include "..\..\..\开发库\Include\GameFrame.h"

#pragma comment (lib,"Winmm.lib")

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=NULL; } }
#endif


//全局变量
extern TCHAR							g_szWorkDir[MAX_PATH];

////////////////////////////////////////////////////////////////////////////
//消息定义
enum enMessage
{
	enMessage_Begin=WM_USER+100,

	//按键消息
	WM_KEY_NUMBER,
	WM_KEY_SWITCH,
	WM_KEY_DELETE,

	//鱼群消息
	WM_FISH_DISAPPEAR,

	//子弹消息
	WM_BULLET_DISAPPEAR,

	enMessage_End
};

//add by Ivan_han
#define WM_SHOW_COIN_SHOP		WM_USER+101								//购买小金币 weni23
#define WM_SHOOT				WM_USER+100								//发射炮弹
#define WM_QUIT_GAME		WM_USER+301								//退出游戏
#define WM_CAPTURE_FISH				WM_USER+200								//捕获鱼群
#define WM_HIT_FISH					WM_USER+201								//击中鱼群
#define WM_HIT_Laser				WM_USER+202								//激光打鱼

#define WM_FISH_CALC_TRACE				WM_USER+300							//轨迹消息
#define WM_NET_OPEN					WM_USER+310								//打开网

//输出信息
void TraceString(LPCTSTR pszMessage);
////////////////////////////////////////////////////////////////////////////
