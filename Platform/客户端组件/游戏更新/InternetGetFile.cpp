// InternetGetFile.cpp: implementation of the InternetGetFile namespace.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <wininet.h>
#include <stdio.h>
#include <mmsystem.h>
#include "InternetGetFile.h"
#include "Wininet.h"
#include <mmsystem.h>
#pragma comment(lib, "Wininet.lib")
#pragma comment(lib, "Winmm.lib")

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define INTERNET_CONNECTION_OFFLINE 0x20
//#define NOTIFY_DOWNLOAD_PROGRESS 1003

CInternet::CInternet()
{
	bInternetGetURLUserBreak = FALSE;
}

CInternet::~CInternet()
{
}

int CInternet::InternetGetURL(const char* sURL, const char* sSaveFilename, const char* sHeader/* = NULL*/, 
							  HWND hProgressWindow/* = NULL*/, long llFileIndex/*=-1*/)
{
	CString strBuffer(sURL);
	int iFindIndex = strBuffer.FindOneOf(_T("http://"));
	if (0 != iFindIndex)
	{
		strBuffer.Insert(0, _T("http://"));
	}
	
	const int BUFFER_SIZE = 32768;
	const int iInterval = 250;
	DWORD iFlags;
	char sAgent[64];
	HINTERNET hInternet;
	HINTERNET hConnect;
	char acBuffer[BUFFER_SIZE];
	DWORD iReadBytes;
	DWORD iTotalReadBytes = 0;
	FILE *pFile = NULL;
	DWORD iStartTime;
	DWORD iCurrentTime;
	DWORD iDuration = 10;
	DWORD iLastRefreshTime = GetTickCount();
	DOWNLOAD_PROGRESS_STRU DownloadProgress;
	DWORD iBytesToRead = 0;
	DWORD iReadBytesOfRq = 4;
	DWORD iCPUTickStart;
	DWORD iCPUTickEnd;

	// Get connection state
	InternetGetConnectedState(&iFlags, 0);
	if (iFlags & INTERNET_CONNECTION_OFFLINE) // take appropriate steps
	{
		return INTERNET_ERROR_OPEN;
	}

	// Set agent string
	_snprintf(sAgent, 64, "Agent%ld", GetTickCount() / 1000);
	// Open internet session
	if (!(iFlags & INTERNET_CONNECTION_PROXY))
	{
		hInternet = InternetOpenA(sAgent, INTERNET_OPEN_TYPE_PRECONFIG_WITH_NO_AUTOPROXY, NULL, NULL, 0);
	}
	else
	{
		hInternet = InternetOpenA(sAgent, INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
	}
	if (hInternet)
	{
		if (sHeader == NULL)
		{
			sHeader = "Accept: */*\r\n\r\n";
		}

		// Get URL
		if (!(hConnect = InternetOpenUrlA (hInternet, strBuffer, sHeader, lstrlenA(sHeader)
			, INTERNET_FLAG_DONT_CACHE | INTERNET_FLAG_PRAGMA_NOCACHE | INTERNET_FLAG_RELOAD, 0)))
		{
			//CString strBuffer;
			//strBuffer.Format("不能打开URL：\r\n%s\r\n请联系棋牌60客服！", strBuffer);
			//AfxMessageBox(strBuffer);

			InternetCloseHandle(hInternet);
			return INTERNET_ERROR_OPENURL;
		}

		// Open file to write
		if (!(pFile = fopen(sSaveFilename, "wb")))
		{
			InternetCloseHandle(hInternet);
			return INTERNET_ERROR_FILEOPEN;
		}

		// Get content size
		if (!HttpQueryInfo(hConnect, HTTP_QUERY_CONTENT_LENGTH | HTTP_QUERY_FLAG_NUMBER
			, (LPVOID)&iBytesToRead, &iReadBytesOfRq, NULL))
		{
			iBytesToRead = 0;
		}

		DownloadProgress.iCurrentFileSize = iBytesToRead;

		iStartTime = GetTickCount();
		iCPUTickStart = GetTickCount();

		do
		{
			if (bInternetGetURLUserBreak)
			{
				LOG(0, 0, "CInternet::InternetGetURL 下载被中断 下载路径 %s, 保存路径 %s", sURL, sSaveFilename);
				break;
			}

			// Keep copying in 16 KB chunks, while file has any data left.
			// Note: bigger buffer will greatly improve performance.
			if (!InternetReadFile(hConnect, acBuffer, BUFFER_SIZE, &iReadBytes))
			{
				fclose(pFile);
				InternetCloseHandle(hInternet);
				return INTERNET_ERROR_READFILE;
			}
			if (iReadBytes > 0)
			{
				// Write to file
				fwrite(acBuffer, sizeof(char), iReadBytes, pFile);
			}

			iTotalReadBytes += iReadBytes;

			iCurrentTime = GetTickCount();
			iDuration = iCurrentTime - iStartTime;
			if (iDuration < 10)
			{
				iDuration = 10;
			}

			if (iBytesToRead > 0
				&& hProgressWindow != NULL
				&& iCurrentTime - iLastRefreshTime > iInterval
				|| iTotalReadBytes == iBytesToRead)
			{
				// Show progress
				memset(acBuffer, 0, BUFFER_SIZE);
				DownloadProgress.uIndex = llFileIndex;
				DownloadProgress.iCurrentFileFinishedSize = iTotalReadBytes;
				DownloadProgress.dwDownloadUseTime = iCurrentTime-iLastRefreshTime;
				iLastRefreshTime = iCurrentTime;
				PostMessage(hProgressWindow, WM_USER_MSG, (WPARAM)NOTIFY_DOWNLOAD_PROGRESS, (LPARAM)&DownloadProgress);	

#ifdef _DEBUG
				Sleep(100); // Delay some time to see progress clearly
#endif
			}

			if (iReadBytes <= 0)
			{
				break; // Condition of iReadBytes=0 indicate EOF. Stop.
			}

			// 降低资源CPU占用率
			iCPUTickEnd = GetTickCount();
			if (iCPUTickEnd - iCPUTickStart > 50)
			{
				Sleep(iCPUTickEnd - iCPUTickStart);
				iCPUTickStart = GetTickCount();
			}
		} while (TRUE);

		fflush (pFile);
		fclose (pFile);

		InternetCloseHandle(hInternet);
	}
	else
	{
		return INTERNET_ERROR_OPEN;
	}

	return INTERNET_SUCCESS;
}

CString CInternet::InternetGetInfo(CString szUrl)
{
    DWORD dwFlags;
	InternetGetConnectedState(&dwFlags, 0);
	CString reValue,strContent;
	reValue=_T("Error");
	CHAR strAgent[64];
	memset(strAgent,0,sizeof(strAgent));
	_snprintf(strAgent, sizeof(strAgent), "Agent%ld", timeGetTime());
	HINTERNET hOpen;
	if(!(dwFlags & INTERNET_CONNECTION_PROXY))
		hOpen = InternetOpenA(strAgent, INTERNET_OPEN_TYPE_PRECONFIG_WITH_NO_AUTOPROXY, NULL, NULL, 0);
	else
		hOpen = InternetOpenA(strAgent, INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
	if(!hOpen)
	{
		return reValue;
	}

	DWORD dwSize;
	CHAR   szHead[] = "Accept: */*\r\n\r\n";
	VOID* szTemp[16384]={0};
	HINTERNET  hConnect;

	if ( !(hConnect = InternetOpenUrlA ( hOpen, szUrl, szHead,
		 lstrlenA (szHead), INTERNET_FLAG_DONT_CACHE | INTERNET_FLAG_PRAGMA_NOCACHE | INTERNET_FLAG_RELOAD, 0)))
	{
		//CString strBuffer;
		//strBuffer.Format("不能打开URL：\r\n%s\r\n请联系棋牌60客服！", szUrl);
		//AfxMessageBox(strBuffer);

		InternetCloseHandle(hOpen);
	    return reValue;
	}

	DWORD dwByteToRead = 0;
	DWORD dwSizeOfRq = 4;
	DWORD dwBytes = 0;

	CString strInfoTemp;

    if (!HttpQueryInfo(hConnect, HTTP_QUERY_CONTENT_LENGTH | HTTP_QUERY_FLAG_NUMBER, 
                  (LPVOID)&dwByteToRead, &dwSizeOfRq, NULL))
	{
		dwByteToRead = 0;
	}


	do
	{
		if (!InternetReadFile (hConnect, szTemp, 16384,  &dwSize))
		{
			return reValue;
		}
		if (dwSize==0)
		{
			break;
		}
		else
		{
			strInfoTemp.Format("%s",szTemp);
			strContent+=strInfoTemp;
		}
		
	}while (TRUE);

	InternetCloseHandle(hConnect);
	InternetCloseHandle(hOpen);

	return strContent;
}
