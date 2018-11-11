#pragma once

#include "resource.h"
#define			CMD_LINE_COUNT				4						//命令行数目
#define			PARAMTER_MAX_COUNT		MAX_PATH		//参数读取最大值
HINSTANCE g_hInstance = NULL;

// 取得程序运行的目录
// bEndWithBackSlash：是否以反斜线结尾
CString GetAppDirectory(BOOL bEndWithBackSlash)
{
	char acBuffer[MAX_PATH];

	memset((void *)acBuffer, 0, MAX_PATH);
	GetModuleFileName(g_hInstance, acBuffer, sizeof(acBuffer));

	char *p = strrchr(acBuffer, '\\');
	if (p != NULL)
	{
		if (bEndWithBackSlash)
		{
			*(p + 1) = '\0';
		}
		else
		{
			*p = '\0';
		}
	}
	else
	{
		p = strrchr(acBuffer, ':');
		if (p != NULL)
		{
			if (bEndWithBackSlash)
			{
				*(p + 1) = '\\';
				*(p + 2) = '\0';
			}
			else
			{
				*p = '\\';
				*(p + 1) = '\0';
			}
		}
	}

	return CString(acBuffer);
}

// 分析命令行
void GetCmdLinePara(TCHAR tchArray[CMD_LINE_COUNT][MAX_PATH], LPTSTR lpCmdLine)
{
	BYTE byArrayIndex = 0;
	CString strLine=lpCmdLine;
	if(strLine.IsEmpty())
		return;

	int nLength=strLine.GetLength();
	char *buf=new char[nLength+1];
	lstrcpyn(buf, strLine, sizeof(char)*(nLength+1));
	char *p=buf;

	for(int i=0;i<PARAMTER_MAX_COUNT;i++)
	{
		if (CMD_LINE_COUNT<=byArrayIndex)
			break;

		if(buf[i]==0x20)//空格
		{
			buf[i]=0x00;
			_snprintf(tchArray[byArrayIndex++], sizeof(tchArray[0]), TEXT("%s"), p);
			i++;
			p=buf+i;
		}
		if(buf[i]==0x00)
		{
			_snprintf(tchArray[byArrayIndex++], sizeof(tchArray[0]), TEXT("%s"), p);
			break;
		}
	}

	delete []buf;
}

// 替换路径中的特殊字符串为实际路径
CString ResolvePath(const char* sPath)
{
	if (sPath == NULL)
	{
		DebugBreak();
		return CString("");
	}

	char acBuffer[MAX_PATH];
	CString sDestFilename = sPath;

	sDestFilename.Replace("<INSTDIR>", GetAppDirectory(TRUE).GetBuffer(0));

	memset(acBuffer, 0, MAX_PATH);
	GetWindowsDirectory(acBuffer, MAX_PATH);
	sDestFilename.Replace("<WINDIR>", acBuffer);

	memset(acBuffer, 0, MAX_PATH);
	GetSystemDirectory(acBuffer, MAX_PATH);
	sDestFilename.Replace("<SYSDIR>", acBuffer);

	memset(acBuffer, 0, MAX_PATH);
	SHGetSpecialFolderPath(NULL, acBuffer, 0x0014, FALSE); // CSIDL_FONTS (0x0014)
	sDestFilename.Replace("<FONTS>", acBuffer);

	memset(acBuffer, 0, MAX_PATH);
	SHGetSpecialFolderPath(NULL, acBuffer, 0x0026, FALSE); // CSIDL_PROGRAM_FILES (0x0026)
	sDestFilename.Replace("<PROGRAMFILES>", acBuffer);

	memset(acBuffer, 0, MAX_PATH);
	SHGetSpecialFolderPath(NULL, acBuffer, 0x002b, FALSE); // CSIDL_PROGRAM_FILES_COMMON (0x002b)
	sDestFilename.Replace("<COMMONFILES>", acBuffer);

	memset(acBuffer, 0, MAX_PATH);
	SHGetSpecialFolderPath(NULL, acBuffer, 0x0028, FALSE); // CSIDL_PROFILE (0x0028)
	sDestFilename.Replace("<PROFILE>", acBuffer);

	memset(acBuffer, 0, MAX_PATH);
	SHGetSpecialFolderPath(NULL, acBuffer, CSIDL_PERSONAL, FALSE);
	sDestFilename.Replace("<DOCUMENTS>", acBuffer);

	memset(acBuffer, 0, MAX_PATH);
	SHGetSpecialFolderPath(NULL, acBuffer, CSIDL_DESKTOPDIRECTORY, FALSE);
	sDestFilename.Replace("<DESKTOP>", acBuffer);

	memset(acBuffer, 0, MAX_PATH);
	SHGetSpecialFolderPath(NULL, acBuffer, CSIDL_STARTMENU, FALSE);
	sDestFilename.Replace("<STARTMENU>", acBuffer);

	memset(acBuffer, 0, MAX_PATH);
	SHGetSpecialFolderPath(NULL, acBuffer, CSIDL_PROGRAMS, FALSE);
	sDestFilename.Replace("<SMPROGRAMS>", acBuffer);

	memset(acBuffer, 0, MAX_PATH);
	SHGetSpecialFolderPath(NULL, acBuffer, CSIDL_STARTUP, FALSE);
	sDestFilename.Replace("<SMSTARTUP>", acBuffer);

	//	sDestFilename.Replace("<QUICKLAUNCH>", acBuffer);

	memset(acBuffer, 0, MAX_PATH);
	SHGetSpecialFolderPath(NULL, acBuffer, CSIDL_SENDTO, FALSE);
	sDestFilename.Replace("<SENDTO>", acBuffer);

	memset(acBuffer, 0, MAX_PATH);
	SHGetSpecialFolderPath(NULL, acBuffer, 0x0006, FALSE); // CSIDL_FAVORITES (0x0006)
	sDestFilename.Replace("<FAVORITES>", acBuffer);

	memset(acBuffer, 0, MAX_PATH);
	SHGetSpecialFolderPath(NULL, acBuffer, CSIDL_APPDATA, FALSE);
	sDestFilename.Replace("<APPDATA>", acBuffer);

	memset(acBuffer, 0, MAX_PATH);
	SHGetSpecialFolderPath(NULL, acBuffer, 0x001c, FALSE); // CSIDL_LOCAL_APPDATA (0x001c)
	sDestFilename.Replace("<LOCAL_APPDATA>", acBuffer);

	//	sDestFilename.Replace("<TEMP>", acBuffer);

	return sDestFilename;
}

//判断指定的目录是否存在
BOOL IsDirExist(LPCTSTR lpszDir)
{
	BOOL bExist;
	int n;
	CString szTemp = lpszDir;
	CString szDir;
	n = szTemp.ReverseFind('\\');
	if(n<=0)
		return FALSE;
	szDir = szTemp.Left(n);

	WIN32_FIND_DATA wfd;
	HANDLE hFind = FindFirstFile(szDir,&wfd);
	if(hFind == INVALID_HANDLE_VALUE)
		bExist = FALSE;
	else
	{
		if(wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			bExist = TRUE;//目录存在
		else
			bExist = FALSE;//目录不存在
		FindClose(hFind);
	}
	return bExist;
}

// 创建路径目录
void CreateAllDirectory(LPCTSTR lpszDir)
{
	int nPos = 0;
	CString strTemp = lpszDir;
	if(strTemp.Right(1)!="\\")
		strTemp += "\\";

	//逐层创建目录
	while((nPos = strTemp.Find('\\',nPos+1)) != -1)
	{
		if(nPos != 2) //排除第一个'\\',(如：C:\\)
		{
			if(!IsDirExist(strTemp.Left(nPos+1))) //判断是否存在
				::CreateDirectory(strTemp.Left(nPos+1),NULL);
		}
	}
}

// 从文件全路径取得文件名
CString GetFilename(const char* sFilePath)
{
	if (sFilePath == NULL)
	{
		return CString("");
	}

	const char *pBackSlash = strrchr(sFilePath, '\\');
	if (pBackSlash == NULL)
	{
		return CString(sFilePath);
	}
	else
	{
		return CString(++pBackSlash);
	}
}

CString GetFilename(const CString &sFilePath)
{
	int iPos = sFilePath.ReverseFind('\\');
	if (iPos == -1)
	{
		return sFilePath;
	}
	else
	{
		return sFilePath.Mid(iPos + 1);
	}
}

// 拷贝文件
BOOL CopyFileUpdate(const string &strFile, const string &strTmpFile, const string &strPath)
{
	// 得到文件路径
	string::size_type Index = strPath.rfind(strFile);

	CString strAppDirectory = GetAppDirectory(TRUE);
	strAppDirectory.Append(strPath.substr(0, Index-1).c_str());
	strAppDirectory.Replace("\\\\", "\\");

	//判断目录
	ResolvePath(strAppDirectory);
	CreateAllDirectory(strAppDirectory);
	strAppDirectory.Append("\\");
	strAppDirectory.Append(strFile.c_str());
	strAppDirectory.Replace("\\\\", "\\");

	// 取得本模块文件名与要复制的文件名
	char acBuffer[MAX_PATH] = {0};	
	GetModuleFileName(g_hInstance, acBuffer, sizeof(acBuffer));
	string strModuleNameOne = GetFilename(acBuffer);
	string strModuleNameTwo = strModuleNameOne;
	strModuleNameTwo.insert(0, "\\");
	string strTmpPath(strPath);
	transform(strModuleNameOne.begin(), strModuleNameOne.end(), strModuleNameOne.begin(), towlower);
	transform(strModuleNameTwo.begin(), strModuleNameTwo.end(), strModuleNameTwo.begin(), towlower);
	transform(strTmpPath.begin(), strTmpPath.end(), strTmpPath.begin(), towlower);

	//计算下载缓冲中的文件，文件目标地址
	CString strExistingFile = GetAppDirectory(TRUE)+_T("tmpupdate");
	strExistingFile.Append("\\");
	strExistingFile.Append(strFile.c_str());
	strExistingFile.Append(strTmpFile.c_str());	

	// 升级文件与本模块相同，则先取一个别名，待运行程序运行后，再删除改名
	if (strPath==strModuleNameOne || strPath==strModuleNameTwo)
	{
		DebugBreak();
	}

	ofstream filestream;
	filestream.open(strExistingFile, ios::in);
	if (filestream)
	{
		SetFileAttributes(strAppDirectory, FILE_ATTRIBUTE_NORMAL);
		DeleteFile(strAppDirectory);
		CopyFile(strExistingFile, strAppDirectory, FALSE);
		filestream.close();
	}
	
	return true;
}


// 记录日志级别
int g_iLogLevel;

// 显示日志的文本框
//extern CRichEditCtrl *g_pLogViewer;

// 日志格式[时间][模块ID][日志级别][日志内容]
#define LOG_FORMAT "[%s][%d][%d][%s]\r\n"
// 日志缓冲区大小
#define LOG_BUFFER_SIZE 1024
// 写日志
// iModuleID：模块ID
// iLevel：日志级别
// szFormat：日志格式
// ...：日志内容
void LOG(int iModuleID, int iLevel, const char* sFormat, ...)
{
	//#ifdef _DEBUG
	// 只记录级别在指定日志级别以上的日志
	if (iLevel < g_iLogLevel)
	{
		return;
	}

	char *pBuffer1 = new char[LOG_BUFFER_SIZE];
	char *pBuffer2 = new char[LOG_BUFFER_SIZE];
	memset(pBuffer1, 0, LOG_BUFFER_SIZE);
	memset(pBuffer2, 0, LOG_BUFFER_SIZE);

	va_list args;
	va_start(args, sFormat);
	_vsnprintf(pBuffer1, LOG_BUFFER_SIZE, sFormat, args);
	va_end(args);

	CString sNow = CTime::GetCurrentTime().Format("%Y-%m-%d %H:%M:%S");
	if (0 < strlen(pBuffer1))
	{
		_snprintf(pBuffer2, LOG_BUFFER_SIZE, LOG_FORMAT, sNow.GetBuffer(0), iModuleID, iLevel, pBuffer1);
	}
	else
	{
		memset(pBuffer2, 0, LOG_BUFFER_SIZE);
		_snprintf(pBuffer2, LOG_BUFFER_SIZE, TEXT("\r\n"));
	}

	// 写入文件
	ofstream appendFile;
	CreateDirectory(GetAppDirectory(TRUE) + "LOG\\GameUpdate.log", NULL);
	appendFile.open(GetAppDirectory(TRUE) + "LOG\\GameUpdate.log", ios::binary | ios::app | ios::out );	

	if(appendFile)
	{
		appendFile.write(pBuffer2, strlen(pBuffer2));
		appendFile.close();
	}


	// 释放临时申请的内存
	delete []pBuffer1;
	delete []pBuffer2;
	pBuffer1 = NULL;
	pBuffer2 = NULL;

	OutputDebugString("GameUpdateDlg.cpp,离开关键代码段, LOG,Guard.leave();");//Test
	//#endif
}

void CheckProcess(LPCTSTR lpProcessName)
{
	HANDLE hProcessSnap = NULL; //初始化 
	PROCESSENTRY32 pe32 = {0}; 
	int count = 0; 
	pe32.dwSize = sizeof(PROCESSENTRY32); 
	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);  //获得句柄 

	if (hProcessSnap == (HANDLE)-1)
		return ; 

	//列举所有进程 
	if (Process32First(hProcessSnap, &pe32)) 
	{  
		do  
		{
			//将进程名和 GamePlace.exe 比较 有 则退出 没有继续查找 
			if (0 == lstrcmp(lpProcessName, pe32.szExeFile)) 
			{
				count++;  
				// 可以做相应的操作

				// 下面是强杀进程的操作
				bool bFlag = false;

				HANDLE hProcess = NULL; 
				DWORD dwExitCode = 0 ;
				hProcess = OpenProcess (PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID); // 根据进程ID获得进程句柄

				if(hProcess)
				{
					bFlag = GetExitCodeProcess(hProcess, &dwExitCode);
					if(bFlag)
					{
						bFlag = TerminateProcess(hProcess, dwExitCode);
					}
				}
			}
		} 
		while (Process32Next(hProcessSnap, &pe32));//直到列举完毕 
	}

	CloseHandle(hProcessSnap);  //关闭句柄
}