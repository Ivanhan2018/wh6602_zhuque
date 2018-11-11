// UpdateHelp.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "resource.h"
#include <tlhelp32.h>
#include "UpdateHelp.h"
#include <vector>

#define PROJECT_NAME							(_T("cbwUpdateHelp.exe"))	// 辅助更新线程

// 优先更新文件结构
typedef struct tagPriorityUpdate
{
	string filename;
	string tmpname;
	string path;

	tagPriorityUpdate()
	{
		filename.clear();
		tmpname.clear();
		path.clear();
	}

	tagPriorityUpdate(string filename, string tmpname, string path)
	{
		this->filename = filename;
		this->tmpname = tmpname;
		this->path = path;
	}
}PRIORITYUPDATE;

int APIENTRY _tWinMain(HINSTANCE hInstance,
					   HINSTANCE hPrevInstance,
					   LPTSTR    lpCmdLine,
					   int       nCmdShow)
{
	LOG(0, 0, "");
	LOG(0, 0, "");
	LOG(0, 0, "****************** 开始 %s 工程升级 ******************", PROJECT_NAME);
	g_hInstance = hInstance;

	TCHAR tchCmd[CMD_LINE_COUNT][MAX_PATH]={0};
	// 参数与更新程序对应，三个参数
	//"/modulefilename:GamePlaza.exe /account:a22222 /pw:dc483e80a7a0bd9ef71d8cf973673924 /updatename:cbwGameUpdate.exe"
	LOG(0, 0, "命令行：[%s]", lpCmdLine);
	GetCmdLinePara(tchCmd, lpCmdLine);

	for (BYTE i=0; i<CMD_LINE_COUNT; i++)
	{
		if (0 >= lstrlen(tchCmd[i]))
			return 0;
	}

	//DebugBreak();//Test

	TCHAR tchRunName[MAX_PATH]={0};
	_snprintf(tchRunName, sizeof(tchRunName), _T("%s"), tchCmd[3]+sizeof("/updatename:")-1);
	
	// 检查进程存在，并杀死它
	CheckProcess(tchRunName);

	TCHAR szKeyword[MAX_PATH]=TEXT("");
	TCHAR szServerPath[MAX_PATH]=TEXT("");
	TCHAR tchName[MAX_PATH]=TEXT("");
	TCHAR tchTmpName[MAX_PATH]=TEXT("");
	TCHAR tchPathName[MAX_PATH]=TEXT("");
	CString strAppDirectory = GetAppDirectory(TRUE);
	strAppDirectory.Append(_T("tmpupdate\\DownloadComplete.ini"));

	// 获取优先更新文件列表
	std::vector<PRIORITYUPDATE> vPriorityUpdateFile;
	int iFlag = 1;
	do
	{
		_snprintf(szKeyword, sizeof(szKeyword), _T("DownloadFile%d"), iFlag++);
		GetPrivateProfileString(szKeyword, _T("filename"), "", tchName, MAX_PATH, strAppDirectory);
		GetPrivateProfileString(szKeyword, _T("tmpfilename"), "", tchTmpName, MAX_PATH, strAppDirectory);
		GetPrivateProfileString(szKeyword, _T("path"), "", tchPathName, MAX_PATH, strAppDirectory);

		if (0 >= lstrlen(tchName)  ||  0 >= lstrlen(tchTmpName)  ||  0 >= lstrlen(tchPathName))
		{
			break;
		}

		vPriorityUpdateFile.push_back(PRIORITYUPDATE(tchName, tchTmpName, tchPathName));
	}while (1);

	// 可以让玩家一直点重试，直到不愿意再尝试为止
	Sleep(1000);
	std::vector<PRIORITYUPDATE>::iterator vBeg = vPriorityUpdateFile.begin();
	for (; vBeg != vPriorityUpdateFile.end(); ++vBeg)
	{
		// 如果自己优先更新，则跳过，因为在更新进程已经更新了
		if (0 == (*vBeg).filename.compare(PROJECT_NAME))
		{
			continue;
		}

		while (1)
		{
			if (CopyFileUpdate((*vBeg).filename, (*vBeg).tmpname, (*vBeg).path))
			{
				LOG(0, 0, "复制文件：%s", (*vBeg).path.c_str());
				break;
			}

			CString strBuffer;
			strBuffer.Format(_T("[%s]更新失败 文件正在被另一个人或程序使用，请关闭相应程序，重新试一次\r\n“当无法结束时建议重启电脑”"), 
				(*vBeg).filename.c_str());
			int iReturn = MessageBox(NULL, strBuffer, TEXT("来自UpdateHelp.exe提示"), MB_RETRYCANCEL);
			if (IDRETRY == iReturn)
			{
				continue;
			}
			else
			{
				return 0; // 更新失败，更新文件被占用，玩家没有重试了。
			}
		}
	}

	//构造命令行
	CString strCommonLine;
	strCommonLine.Format(TEXT("%s %s %s %s"), tchRunName, tchCmd[0], tchCmd[1], tchCmd[2]);
	//启动游戏客户端
	STARTUPINFO StartInfo;
	memset(&StartInfo,0,sizeof(StartInfo));
	StartInfo.cb=sizeof(StartInfo);
	StartInfo.wShowWindow=SW_SHOWMAXIMIZED;
	BOOL bSuccess = FALSE;
	PROCESS_INFORMATION ProcessInfo;
	memset(&ProcessInfo,0,sizeof(ProcessInfo));

	try
	{
		LOG(0, 0, "创建进程：%s", strCommonLine.GetBuffer());
		strCommonLine.ReleaseBuffer();
		bSuccess = CreateProcess(NULL,strCommonLine.GetBuffer(),NULL,NULL,FALSE,CREATE_DEFAULT_ERROR_MODE,NULL,NULL,
			&StartInfo,&ProcessInfo);
		DWORD dwError = GetLastError();
		CString strBuffer;
		strBuffer.Format("UpdateHelp: %d, 错误ID %ld", bSuccess, dwError);
		OutputDebugString(strBuffer);
	}
	catch (...) 
	{ 
		DWORD dwError = GetLastError();
		DebugBreak();
	}

	strCommonLine.ReleaseBuffer();
	LOG(0, 0, "****************** 结束 %s 工程升级 ******************", PROJECT_NAME);
	return 0;
}