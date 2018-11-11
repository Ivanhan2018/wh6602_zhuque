#pragma once

#ifndef GAME_FRAME_H
#define GAME_FRAME_H

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN		// 从 Windows 头中排除极少使用的资料
#endif
#define _WIN32_WINNT 0x0500	// 这句一定要放在 #include <windows.h> 前面，否则编译不过去

// 如果您必须使用下列所指定的平台之前的平台，则修改下面的定义。
// 有关不同平台的相应值的最新信息，请参考 MSDN。
#ifndef WINVER				// 允许使用特定于 Windows 95 和 Windows NT 4 或更高版本的功能。
#define WINVER 0x0501		// 将此更改为针对于 Windows 98 和 Windows 2000 或更高版本的合适的值。
#endif

#ifndef _WIN32_WINNT		// 允许使用特定于 Windows NT 4 或更高版本的功能。
#define _WIN32_WINNT 0x0501	// 将此更改为针对于 Windows 2000 或更高版本的合适的值。
#endif						

#ifndef _WIN32_WINDOWS		// 允许使用特定于 Windows 98 或更高版本的功能。
#define _WIN32_WINDOWS 0x0501 // 将此更改为针对于 Windows Me 或更高版本的合适的值。
#endif

#ifndef _WIN32_IE			// 允许使用特定于 IE 4.0 或更高版本的功能。
#define _WIN32_IE 0x0501	// 将此更改为针对于 IE 5.0 或更高版本的合适的值。
#endif

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// 某些 CString 构造函数将为显式的

#include <afxwin.h>         // MFC 核心组件和标准组件
#include <afxext.h>         // MFC 扩展

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxole.h>         // MFC OLE 类
#include <afxodlgs.h>       // MFC OLE 对话框类
#include <afxdisp.h>        // MFC 自动化类
#endif // _AFX_NO_OLE_SUPPORT

#ifndef _AFX_NO_DB_SUPPORT
#include <afxdb.h>			// MFC ODBC 数据库类
#endif // _AFX_NO_DB_SUPPORT

#ifndef _AFX_NO_DAO_SUPPORT
#include <afxdao.h>			// MFC DAO 数据库类
#endif // _AFX_NO_DAO_SUPPORT

#include <afxdtctl.h>		// MFC 对 Internet Explorer 4 公共控件的支持
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC 对 Windows 公共控件的支持
#endif // _AFX_NO_AFXCMN_SUPPORT

////道具头文件
//#include "..\..\客户端组件\鲜花道具\PropertyBar.h"
//#include "..\..\客户端组件\鲜花道具\PropertyModule.h"
//#include "..\..\客户端组件\鲜花道具\FlowerEffectcontrol.h"

#include "..\..\共享组件\界面控件\SkinControls.h"

//////////////////////////////////////////////////////////////////////////

enum SET_IMAGE_RGN_TYPE
{
	SIRT_TRANS_DOT_LESS = 0,	// 针对一幅图里面空心（透明）的点比较少
	SIRT_SOLID_DOT_LESS = 1,	// 针对一幅图里面实心（非透明）的点比较少
	SIRT_FULL_DOT_LESS = 2,		// 针对整幅图 //osc
	SIRT_DEBURRING_DOT_LESS = 3,// 针对修剪角图			Author:	<cxf>
};

BOOL SetImageRgn(HWND hWnd, CPngImage * pImage, int iType=SIRT_FULL_DOT_LESS);

//struct CSONY
//{
//	CSONY()
//	{
//		iA = 0;
//		iB = 0;
//	}
//	int	iA; 
//	int    iB;
//};

//CSONY stSony;	// 只是为了检验"游戏广场"是否再次包含了本文件；

#endif