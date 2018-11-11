// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__6E171DE5_929C_4558_880B_14D7DDC8781D__INCLUDED_)
#define AFX_STDAFX_H__6E171DE5_929C_4558_880B_14D7DDC8781D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <string>
#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include "afxwin.h"
#include "CDirOperate.h"
#include "ThreadGuard.h"

using namespace std;
// 取得程序运行的目录
CString GetAppDirectory(BOOL bEndWithBackSlash = TRUE);

// 从文件全路径取得文件名
CString GetFilename(const char* sFilePath);
CString GetFilename(const CString& sFilePath);

// 取得文件扩展名
CString GetFileExtendName(const char* sFilename);
CString GetFileExtendName(const CString& sFilename);

// 取得文件大小
size_t GetFileSize(const char* sFilename);
size_t GetFileSize(const CString& sFilename);

// 写日志
static BOOL g_IsWriteLog = TRUE;
void LOG(int iModuleID, int iLevel, const char* sFormat, ...);

// 计算文件的MD5摘要码
extern BOOL CalculateMD5(const char* sFilename, unsigned char acMD5Digest[]);
// 计算字符串的MD5摘要码
extern BOOL CalculateStringMD5(const char* sString, unsigned int nLen, unsigned char acMD5Digest[]);
extern CString MD5toString(unsigned char acMD5Digest[]);

// 显示消息框函数，用于替代AfxMessageBox函数
int RMessageBox(CString& sText, UINT nType = MB_OK, UINT nIDHelp = 0);
int RMessageBox(LPCTSTR lpszText, UINT nType = MB_OK, UINT nIDHelp = 0);

// 替换路径中的特殊字符串为实际路径
CString ResolvePath(const char* sPath);

// string 字体串全部替换
string& replace_all(string& str, const string& old_value, const string& new_value);

// 下载文件结构体
typedef struct tagFileHashTable
{
	string filename;	// 文件名
	string tmpFileName; // 临时文件名
	string DATName;		// 扩展名
	string path;		// 当前的相对路径
	string Hash;		// 对应的Hash值
	BOOL bDowndloadComplete; // 下载完成
	BOOL bDowndLoading;		// 下载中
	UINT uDowndloadCount;	// 下载次数
	ULONGLONG size;		// 文件大小
	
	tagFileHashTable()
	{
		ZeroMemory(this, sizeof(*this));
	}
}FileHashTable;

typedef enum tagThreadRunStartOperator
{
	ONLY_RUN,				// 默认情况运行
	GENERATE,				// 生成INI
	GENERATE_RUN,			// 先生成INI，再运行
}ThreadRunStartOperator;

#define DOWNDLOAD_TMP_PATH					(_T("tmpupdate"))					// 下载保存临时路径
#define CONFIG_PATH										(_T("config"))							// 配置文件保存路径
#define UPDATE_CONFIG_FILENAME				(_T("update.ini"))					// 本地保存的升级配置文件名
#define UPDATE_CONFIG_TMP_FILENAME		(_T("updateserver.ini"))			// 服务器的升级配置文件名
#define UPDATE_SET_INI_FILE							(_T("updateset.ini"))				// 更新机器设置文件
#define UPDATE_SET_INI_TMP_FILE					(_T("updatesetserver.ini"))		// 更新机器设置文件
#define DOWNDLOAD_DEFAULT_PATH				(_T("http://dls.cbwgame.com/update/hallfile/"))	// 默认下载路径
#define PROJECT_NAME									(_T("cbwGameUpdate.exe"))	// 项目名称
#define HELP_UPDATE_NAME							(_T("cbwUpdateHelp.exe"))	// 辅助更新线程
#define	FILE_DOWNLOAD_COUNT					3											// 文件下载失败重试的次数

const int BUFFER_SIZE = 512;


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__6E171DE5_929C_4558_880B_14D7DDC8781D__INCLUDED_)
