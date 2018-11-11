// stdafx.h : 标准系统包含文件的包含文件，
// 或是常用但不常更改的项目特定的包含文件
//

#pragma once

#define _WIN32_WINNT 0x0500	// 这句一定要放在 #include <windows.h> 前面，否则编译不过去

#define WIN32_LEAN_AND_MEAN		// 从 Windows 头中排除极少使用的资料
// Windows 头文件:
#include <atlstr.h>
#include <windows.h>


// C 运行时头文件
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <string>
#include <algorithm>
#include <shlobj.h>

#include <atltime.h>
#include <atlfile.h>

#include <fstream>
#include <ostream>

#pragma comment(lib, "shell32.lib")
using namespace std;
// TODO: 在此处引用程序要求的附加头文件
