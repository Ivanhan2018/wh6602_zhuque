// UpdateThread.cpp : implementation file
//

#include "stdafx.h"
#include "GameUpdate.h"
#include "UpdateThread.h"
#include "GameUpdateDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//const int BUFFER_SIZE = 512;
/////////////////////////////////////////////////////////////////////////////
// CUpdateThread

IMPLEMENT_DYNCREATE(CUpdateThread, CWinThread)

CUpdateThread::CUpdateThread()
{
	m_bSilenceMode = FALSE;
	m_bUserBreak = FALSE;
	m_hProgressWindow = NULL;
	m_setWantUpdateFile.clear();
	m_setDowndloadFile.clear();
	m_CompleteDownByte = 0;
}

CUpdateThread::~CUpdateThread()
{
}

BOOL CUpdateThread::InitInstance()
{
	// TODO:  perform and per-thread initialization here
	return TRUE;
}

int CUpdateThread::ExitInstance()
{
	// TODO:  perform any per-thread cleanup here
	m_bUserBreak = TRUE;

	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CUpdateThread, CWinThread)
	//{{AFX_MSG_MAP(CUpdateThread)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CUpdateThread message handlers

int CUpdateThread::Run() 
{
	if (GENERATE == m_StartOperator)
	{
		DoGenerateMD5Hash();
		return 1;
	}
	else if (GENERATE_RUN == m_StartOperator)
	{
		DoGenerateMD5Hash(TRUE);
	}
	else if (ONLY_RUN != m_StartOperator)
	{
		return 1;
	}

	CheckWantUpdateFile();
	if (DoUpdateEx())
	{
		if (m_hProgressWindow)
		{
			SendMessage(m_hProgressWindow, WM_USER, (WPARAM)NOTIFY_FINISH_UPDATE, (LPARAM)1);
		}
		return 1;
	}
	else
	{
		if (m_hProgressWindow)
		{
			SendMessage(m_hProgressWindow, WM_USER, (WPARAM)NOTIFY_FINISH_UPDATE, (LPARAM)0);
		}
		return 0;
	}

//	return CWinThread::Run();
}

// 执行升级
BOOL CUpdateThread::DoUpdateEx()
{
	if (m_setWantUpdateFile.empty())
	{
		LOG(0, 0, "Error: 需要更新的文件列表为空 程序相应变量:m_setWantUpdateFile,函数名:CUpdateThread::DoUpdateEx()");
		return FALSE;
	}

	char acBuffer[BUFFER_SIZE];
	DOWNLOAD_INFO_STRU DownloadInfo;

	for (map<string, FileHashTable>::iterator beg=m_setWantUpdateFile.begin(); beg!=m_setWantUpdateFile.end(); beg++)
	{
		DownloadInfo.iFileCount++;
		DownloadInfo.iFileSize += beg->second.size;
	}

	// 将文件总数和文件大小总和上报到界面
	if (!m_bSilenceMode && m_hProgressWindow != NULL)
		SendMessage(m_hProgressWindow, WM_USER, (WPARAM)NOTIFY_DOWNLOAD_INFO, (LPARAM)&DownloadInfo);


	// 下载文件
	UINT uFileCount=m_setDowndloadFile.size();
	char cFileCount[MAX_PATH]={0};
	memset(&DownloadInfo, 0, sizeof(DOWNLOAD_INFO_STRU));
	CString szPath(GetAppDirectory());
	TCHAR chBuffer[MAX_PATH]={0}, chKeyName[MAX_PATH]={0}, chFileSzie[MAX_PATH]={0};
	
	for (map<string, FileHashTable>::iterator beg=m_setWantUpdateFile.begin(); beg!=m_setWantUpdateFile.end(); beg++)
	{
		if (m_bUserBreak)
			return FALSE;

		//检查该文件是否已经下载， 如已下载无需再下
		map<string, FileHashTable>::iterator mLocal = m_setDowndloadFile.begin(); 
		for (; mLocal != m_setDowndloadFile.end(); mLocal++)
		{
			if (0 == beg->second.path.compare(mLocal->second.path))
			{
				break;
			}
		}

		// 文件存在无需要再下载
		if (mLocal != m_setDowndloadFile.end())
		{
			//临时扩展名仍然用之前下载时取的
			beg->second.tmpFileName = mLocal->second.tmpFileName;
			continue;
		}


		_snprintf(cFileCount, sizeof(cFileCount), TEXT(".tmp%d"), ++uFileCount);
		beg->second.tmpFileName = (char *)cFileCount;

		if (!DownloadFile(beg->second.filename, beg->second.tmpFileName, beg->second.path))
		{
			// 文件下载失败，升级失败
			if (!m_bSilenceMode && m_hProgressWindow != NULL)
			{
				memset(acBuffer, 0, BUFFER_SIZE);
				_snprintf(acBuffer, sizeof(acBuffer), "%s", beg->first.c_str());
				SendMessage(m_hProgressWindow, WM_USER, (WPARAM)NOTIFY_DOWNLOAD_FILE_FAIL, (LPARAM)acBuffer);
			}

			LOG(0, 0, "Error: 下载失败 %s, 函数名:CUpdateThread::DoUpdateEx()", beg->first.c_str());
			//// 某个文件下载失败，提示玩家下载完整包，或退出更新
			//CString strBuffer;
			//strBuffer.Format("[%s]文件下载失败!\r\n您可以尝试点击“确定”下载完整版，或者“取消”这次更新", beg->first.c_str());
			//int iResult = RMessageBox(strBuffer, MB_OKCANCEL);
			//if (IDOK == iResult)
			//{
			//	TCHAR tchDowndPath[MAX_PATH];
			//	_snprintf(tchDowndPath, sizeof(tchDowndPath), _T("%s/download.php"), g_pCGameUpdateDlg->m_stTGS.szWeb);
			//	ShellExecute(NULL, "open", tchDowndPath, NULL, NULL, SW_SHOWNORMAL);
			//}

			//g_pCGameUpdateDlg->PostMessage(WM_CLOSE);
			return FALSE;
		}

		// 将升级进度上报到界面
		DownloadInfo.iFileCount++;
		DownloadInfo.iFileSize += beg->second.size;
		if (!m_bSilenceMode && m_hProgressWindow != NULL)
		{
			SendMessage(m_hProgressWindow, WM_USER, (WPARAM)NOTIFY_DOWNLOADED_INFO, (LPARAM)&DownloadInfo);
		}

		//记录下载完成的文件，为续传准备
		_snprintf(chBuffer, sizeof(chBuffer), TEXT("%s\\%s\\DownloadComplete.ini"), szPath, DOWNDLOAD_TMP_PATH);
		_snprintf(chKeyName, sizeof(chKeyName), TEXT("DownloadFile%d"), uFileCount);
		WritePrivateProfileString(chKeyName, TEXT("filename"), beg->second.filename.c_str(), chBuffer);
		WritePrivateProfileString(chKeyName, TEXT("tmpfilename"), beg->second.tmpFileName.c_str(), chBuffer);
		WritePrivateProfileString(chKeyName, TEXT("datname"), beg->second.DATName.c_str(), chBuffer);
		WritePrivateProfileString(chKeyName, TEXT("path"), beg->second.path.c_str(), chBuffer);
		WritePrivateProfileString(chKeyName, TEXT("hash"), beg->second.Hash.c_str(), chBuffer);
		_snprintf(chFileSzie, sizeof(chFileSzie), "%I64u", beg->second.size);
		WritePrivateProfileString(chKeyName, TEXT("size"), chFileSzie, chBuffer);
	}


	// 下载完毕后校验文件
	for (map<string, FileHashTable>::iterator beg=m_setWantUpdateFile.begin(); beg!=m_setWantUpdateFile.end(); beg++)
	{
		if (m_bUserBreak)
			return FALSE;

		
		// 校验失败，默认重新下次三次
		int iCount=0;
		bool bVerifySuccess = false;
		do
		{
			if (VerifyFile(beg->second.filename, beg->second.tmpFileName, beg->second.Hash))
			{
				bVerifySuccess = true;
				break;
			}

			iCount++;
			DownloadFile(beg->second.filename, beg->second.tmpFileName, beg->second.path);
		}while (3 >= iCount);


		if (!bVerifySuccess && !VerifyFile(beg->second.filename, beg->second.tmpFileName, beg->second.Hash))
		{
			// 文件校验不通过，升级失败
			if (!m_bSilenceMode && m_hProgressWindow != NULL)
			{
				memset(acBuffer, 0, BUFFER_SIZE);
				_snprintf(acBuffer, sizeof(acBuffer), "%s", beg->first.c_str());
				SendMessage(m_hProgressWindow, WM_USER, (WPARAM)NOTIFY_VERIFY_FILE_FAIL, (LPARAM)acBuffer);
			}

			//// 某个文件校验失败，提示玩家下载完整包，或退出更新
			//CString strBuffer;
			//strBuffer.Format("[%s]文件下载后校验失败!\r\n您可以尝试点击“确定”下载完整版，或者“取消”这次更新", beg->first.c_str());
			//int iResult = RMessageBox(strBuffer, MB_OKCANCEL);
			//if (IDOK == iResult)
			//{
			//	TCHAR tchDowndPath[MAX_PATH];
			//	_snprintf(tchDowndPath, sizeof(tchDowndPath), _T("%s/download.php"), g_pCGameUpdateDlg->m_stTGS.szWeb);
			//	ShellExecute(NULL, "open", tchDowndPath, NULL, NULL, SW_SHOWNORMAL);
			//}

			//g_pCGameUpdateDlg->PostMessage(WM_CLOSE);
			return FALSE;
		}
	}

	
	//检查大厅是否已经关闭
	CWnd *pWnd = NULL;
	while (true)
	{
		HWND hMain = ::FindWindow(TEXT("QPGamePlaza"), NULL);
		if (hMain != NULL)
		{
			int iResult = RMessageBox(_T("请先关闭游戏大厅，再重试！"), 0x00000007L);
			if (IDCANCEL == iResult)
			{
				g_pCGameUpdateDlg->PostMessage(WM_CLOSE);
				return FALSE;
				
			}
			else if (IDYES == iResult)
			{
				PostMessage(hMain, WM_CLOSE, 0, 0);
				Sleep(100);
				continue;
			}
			else
			{
				continue;
			}
		}
		else
			break;
	}


	// 循环更新所有文件
	for (map<string, FileHashTable>::iterator beg=m_setWantUpdateFile.begin(); beg!=m_setWantUpdateFile.end(); beg++)
	{
		if (m_bUserBreak)
			return FALSE;

		if (!UpdateFile(beg->second.filename, beg->second.tmpFileName, beg->second.path))
		{
			CString strName;
			TCHAR szPath[MAX_PATH]=TEXT("");
			GetModuleFileName(AfxGetInstanceHandle(), szPath, sizeof(szPath));
			strName = GetFilename(szPath);

			// 被占用的文件是更新程序时跳过，更新程序被占用无法更新已经在大厅处理过
			if (0 != strName.CompareNoCase(beg->second.filename.c_str()))
			{
				// 文件更新不成功，升级失败
				if (!m_bSilenceMode && m_hProgressWindow != NULL)
				{
					memset(acBuffer, 0, BUFFER_SIZE);
					_snprintf(acBuffer, sizeof(acBuffer), "%s", beg->first.c_str());
					SendMessage(m_hProgressWindow, WM_USER, (WPARAM)NOTIFY_UPDATE_FILE_FAIL, (LPARAM)acBuffer);
				}

				// 可以让玩家一直点重试，直到不愿意再尝试为止
				while (1)
				{
					CString strBuffer;
					strBuffer.Format(_T("[%s]更新失败 文件正在被另一个人或程序使用，请关闭相应程序，重新试一次\r\n“当无法结束时建议重启电脑”"), beg->second.filename.c_str());
					int iReturn = RMessageBox(strBuffer, MB_RETRYCANCEL);
					if (IDRETRY == iReturn)
					{
						if (UpdateFile(beg->second.filename, beg->second.tmpFileName, beg->second.path))
							break;
					}
					else
					{
						g_pCGameUpdateDlg->PostMessage(WM_CLOSE);
						return FALSE;
					}
				}
			}
		}
	}


	CString strAppDirectory = GetAppDirectory();
	strAppDirectory.Append(DOWNDLOAD_TMP_PATH);
	strAppDirectory.Replace("\\\\", "\\");
	CString strIniFilePath = GetAppDirectory() + CONFIG_PATH + "\\" + UPDATE_CONFIG_FILENAME;
	strIniFilePath.Replace("\\\\", "\\");
	
	// 更新最新MD5 Hash 配置文件
	if (FALSE == m_bUserBreak)
	{
		SetFileAttributes(strIniFilePath, FILE_ATTRIBUTE_NORMAL);
		DeleteFile(strIniFilePath);
		MoveFile(strAppDirectory+"\\"+UPDATE_CONFIG_TMP_FILENAME, strIniFilePath);
	}

	// 删除下载中使用的缓冲目录
	if (FALSE == m_bUserBreak)
	{
		CDirOperate::DeleteAllDirectory(strAppDirectory);
	}


	//更新完成，重启游戏大厅
	g_pCGameUpdateDlg->ShowWindow(SW_HIDE);
	g_pCGameUpdateDlg->TransferExternalProgram();
	g_pCGameUpdateDlg->PostMessage(WM_CLOSE);
	return TRUE;
}

BOOL CUpdateThread::DownFile(const char* sURL, \
					const char* sSaveFilename, \
					const char* sHeader, \
					HWND hProgressWindow)
{
	CInternet vInternet;
	if (CInternet::INTERNET_SUCCESS != vInternet.InternetGetURL(sURL, sSaveFilename, sHeader, hProgressWindow))
	{
		g_pCGameUpdateDlg->m_pCInetSession = new CInetSession(sURL, sSaveFilename);
		delete g_pCGameUpdateDlg->m_pCInetSession;
		g_pCGameUpdateDlg->m_pCInetSession = NULL;

		CFile file;
		if ( file.Open(sSaveFilename, CFile::modeRead))
		{
			file.Close();
			LOG(0, 0, "成功下载 \"%s\".", sSaveFilename);
			return TRUE;
		}

		LOG(0, 0, "Error: 下载失败 \"%s\".", sSaveFilename);
		return FALSE;
	}


	return TRUE;
}

BOOL CUpdateThread::DownloadFile(const string &strFile, const string &strTmpFile, const string &strPath) const
{
	TCHAR szResultPath[MAX_PATH]=TEXT("");
	CString strSavePath;
	_snprintf(szResultPath, sizeof(szResultPath), "%sconfig\\%s", GetAppDirectory(), UPDATE_CONFIG_FILENAME);
	strSavePath.Format("%s%s\\%s%s", GetAppDirectory(), DOWNDLOAD_TMP_PATH, strFile.c_str(), strTmpFile.c_str());

	GetPrivateProfileString(_T("UpdateAllFile"), _T("URL"), _T("http://192.168.1.5/update/hallfile/"), szResultPath, MAX_PATH, szResultPath);
	CString strURL(szResultPath);

	if ('/' != strURL.GetAt(strURL.GetLength()-1))
		strURL.Append("/");
	strURL.Append(strPath.c_str());

	strURL.Replace(_T("\\"), _T("/"));
	strURL.Replace(_T("\\\\"), _T("/"));
	strURL.Replace(_T("//"), _T("/"));
	strURL.Replace(_T(":/"), _T("://"));

	if (!m_bSilenceMode && m_hProgressWindow != NULL)
	{
		SendMessage(m_hProgressWindow, WM_USER, (WPARAM)NOTIFY_DOWNLOADING_FILENAME, (LPARAM)strFile.c_str());
	}

	CInternet vInternet;
	int iResult = vInternet.InternetGetURL(strURL.GetBuffer(0), strSavePath.GetBuffer(0), NULL, m_hProgressWindow);
	strURL.ReleaseBuffer();
	strSavePath.ReleaseBuffer();

	if (iResult != CInternet::INTERNET_SUCCESS)
	{
		CGameUpdateDlg * pWnd = static_cast<CGameUpdateDlg * >(CGameUpdateDlg::FromHandle(m_hProgressWindow));
		pWnd->m_pCInetSession = new CInetSession(strURL.GetBuffer(0), strSavePath.GetBuffer(0));
		strURL.ReleaseBuffer();
		strSavePath.ReleaseBuffer();
		delete pWnd->m_pCInetSession;
		pWnd->m_pCInetSession = NULL;

		CFile file;
		if ( file.Open(strSavePath, CFile::modeRead))
		{
			file.Close();
			LOG(0, 0, "下载成功 \"%s\".", strSavePath);
			return TRUE;
		}

		// 记录下载文件失败日志
		char acBuffer[BUFFER_SIZE];
		LOG(0, 0, "Error: 下载失败 \"%s\".", strSavePath);
		memset(acBuffer, 0, BUFFER_SIZE);
		_snprintf(acBuffer, sizeof(acBuffer), "%s", strSavePath);
		SendMessage(m_hProgressWindow, WM_USER, (WPARAM)NOTIFY_DOWNLOAD_FILE_FAIL, (LPARAM)acBuffer);
		return FALSE;
	}

	LOG(0, 0, "下载成功 \"%s\" .", strSavePath);
	return TRUE;
}

// 校验文件
BOOL CUpdateThread::VerifyFile(const string &strFile, const string &strTmpFile, const string &strHash) const
{
	TCHAR szSavePath[MAX_PATH]=TEXT("");
	CString strAppDirectory = GetAppDirectory();
	strAppDirectory.Append("\\");
	strAppDirectory.Append(DOWNDLOAD_TMP_PATH);
	strAppDirectory.Append("\\");
	strAppDirectory.Append(strFile.c_str());
	strAppDirectory.Append(strTmpFile.c_str());
	strAppDirectory.Replace("\\\\", "\\");
	
	// 计算文件的Hash码以进行比较
	unsigned char acMD5Digest[16];
	CalculateMD5(strAppDirectory.GetBuffer(0), acMD5Digest);
	strAppDirectory.ReleaseBuffer();

	if (0 == strHash.compare(MD5toString(acMD5Digest)))
	{
		LOG(0, 0, "效验成功 \"%s\" .", strAppDirectory.GetBuffer(0));
		return TRUE;
	}
	else
	{
		LOG(0, 0, "Error: 检验失败 函数模块名：\"%s\".", strAppDirectory.GetBuffer(0));
		strAppDirectory.ReleaseBuffer();
		char acBuffer[BUFFER_SIZE];
		memset(acBuffer, 0, BUFFER_SIZE);
		_snprintf(acBuffer, sizeof(acBuffer), "%s", strAppDirectory.GetBuffer(0));
		SendMessage(m_hProgressWindow, WM_USER, (WPARAM)NOTIFY_DOWNLOAD_FILE_FAIL, (LPARAM)acBuffer);
		strAppDirectory.ReleaseBuffer();
	}

	return FALSE;
}

BOOL CUpdateThread::UpdateFile(const string &strFile, const string &strTmpFile, const string &strPath) const
{
	// 得到文件路径
	string::size_type Index = strPath.rfind(strFile);

	CString strAppDirectory = GetAppDirectory();
	strAppDirectory.Append(strPath.substr(0, Index-1).c_str());
	strAppDirectory.Replace("\\\\", "\\");
		
	//判断目录
	ResolvePath(strAppDirectory);
	CDirOperate::CreateAllDirectory(strAppDirectory);
	strAppDirectory.Append("\\");
	strAppDirectory.Append(strFile.c_str());
	strAppDirectory.Replace("\\\\", "\\");

	// 取得本模块文件名与要复制的文件名
	char acBuffer[MAX_PATH] = {0};	
	GetModuleFileName(AfxGetInstanceHandle(), acBuffer, sizeof(acBuffer));
	string strModuleNameOne = GetFilename(acBuffer);
	string strModuleNameTwo = strModuleNameOne;
	strModuleNameTwo.insert(0, "\\");
	string strTmpPath(strPath);
	transform(strModuleNameOne.begin(), strModuleNameOne.end(), strModuleNameOne.begin(), towlower);
	transform(strModuleNameTwo.begin(), strModuleNameTwo.end(), strModuleNameTwo.begin(), towlower);
	transform(strTmpPath.begin(), strTmpPath.end(), strTmpPath.begin(), towlower);
	
	//计算下载缓冲中的文件，文件目标地址
	CString strExistingFile = GetAppDirectory()+DOWNDLOAD_TMP_PATH;
	strExistingFile.Append("\\");
	strExistingFile.Append(strFile.c_str());
	strExistingFile.Append(strTmpFile.c_str());	

	// 升级文件与本模块相同，则先取一个别名，待运行程序运行后，再删除改名
	if (strPath==strModuleNameOne || strPath==strModuleNameTwo)
		strAppDirectory.Append(".new");

	SetFileAttributes(strAppDirectory, FILE_ATTRIBUTE_NORMAL);
	DeleteFile(strAppDirectory);
	return MoveFile(strExistingFile, strAppDirectory);
}

void CUpdateThread::CheckSingleFileUpdate(void)
{
	TCHAR szNamePath[MAX_PATH]=TEXT(""); //本地配置文件路径
	TCHAR szKeyword[MAX_PATH]=TEXT("");//更新文件字段
	
	TCHAR szResultfile[MAX_PATH]=TEXT("");//返回文件名结果
	TCHAR szResultPath[MAX_PATH]=TEXT("");//返回路径结果
	TCHAR szResultHash[MAX_PATH]=TEXT("");//返回HASH结果
	TCHAR szResultSize[MAX_PATH]=TEXT("");//返回文件大小

	//读取本地HASH列表
	CString szPath(GetAppDirectory());
	_snprintf(szNamePath, sizeof(szNamePath), "%s%s\\%s", szPath, CONFIG_PATH, UPDATE_CONFIG_FILENAME);
	CString strFile;
	map<string, FileHashTable> localFileList,ServerFileList; //保存单个文件更新
	SendMessage(m_hProgressWindow, WM_USER, (WPARAM)NOTIFY_MESSAGE, (LPARAM)"正在读取本地文件列表...");
	int iIndex=0;

	while (1)
	{
		_snprintf(szKeyword, sizeof(szKeyword), "_SignleFileUpdate__%d", iIndex++);
		GetPrivateProfileString(szKeyword, _T("name"), _T(""), szResultfile, MAX_PATH, szNamePath);
		GetPrivateProfileString(szKeyword, _T("size"), _T(""), szResultSize, MAX_PATH, szNamePath);
		GetPrivateProfileString(szKeyword, _T("path"), _T(""), szResultPath, MAX_PATH, szNamePath);
		GetPrivateProfileString(szKeyword, _T("hash"), _T(""), szResultHash, MAX_PATH, szNamePath);

		if (0==strcmp(szResultfile, "") || 0==strcmp(szResultPath, ""))
			break;

		FileHashTable filehashtable;
		filehashtable.filename = szResultfile;
		filehashtable.size = atol(szResultSize);
		filehashtable.path = szResultPath;
		filehashtable.Hash = szResultHash;
		string::size_type sfileNameIndex = filehashtable.filename.find(".");
		if (0<=sfileNameIndex && filehashtable.filename.size()>sfileNameIndex)
			filehashtable.DATName = filehashtable.filename.substr(sfileNameIndex, filehashtable.filename.size()-sfileNameIndex);
		localFileList.insert(make_pair(string(szResultPath), filehashtable));
	}


	//读取已下载的文件准备续传
	int iFileCount = 0;
	FileHashTable filehashtable;
	TCHAR chBuffer[MAX_PATH], chKeyName[MAX_PATH], chVal[MAX_PATH];
	_snprintf(chBuffer, sizeof(chBuffer), TEXT("%s%s\\DownloadComplete.ini"), szPath, DOWNDLOAD_TMP_PATH);
	m_CompleteDownByte = 0;

	while (1)
	{
		_snprintf(chKeyName, sizeof(chKeyName), TEXT("DownloadFile%d"), ++iFileCount);
		GetPrivateProfileString(chKeyName, TEXT("filename"), TEXT(""), chVal, sizeof(chVal), chBuffer);
		filehashtable.filename = chVal;
		memset(chVal, 0, sizeof(chVal));
		GetPrivateProfileString(chKeyName, TEXT("tmpfilename"), TEXT(""), chVal, sizeof(chVal), chBuffer);
		filehashtable.tmpFileName = chVal;
		memset(chVal, 0, sizeof(chVal));
		GetPrivateProfileString(chKeyName, TEXT("datname"), TEXT(""), chVal, sizeof(chVal), chBuffer);
		filehashtable.DATName = chVal;
		memset(chVal, 0, sizeof(chVal));
		GetPrivateProfileString(chKeyName, TEXT("path"), TEXT(""), chVal, sizeof(chVal), chBuffer);
		filehashtable.path = chVal;
		memset(chVal, 0, sizeof(chVal));
		GetPrivateProfileString(chKeyName, TEXT("hash"), TEXT(""), chVal, sizeof(chVal), chBuffer);
		filehashtable.Hash = chVal;
		memset(chVal, 0, sizeof(chVal));
		GetPrivateProfileString(chKeyName, TEXT("size"), TEXT(""), chVal, sizeof(chVal), chBuffer);
		filehashtable.size = atol(chVal);

		if (0 < filehashtable.filename.size()  &&  0 < filehashtable.path.size())
		{
			m_CompleteDownByte += filehashtable.size;
			m_setDowndloadFile.insert(make_pair(filehashtable.path, filehashtable));
		}
		else
		{
			break;
		}
	}



	// 创建相应目录
	CString cstrDirectory(GetAppDirectory());
	cstrDirectory.Append(DOWNDLOAD_TMP_PATH);
	//不能续传或者续传文件损坏，清空该文件夹
	if ( m_setDowndloadFile.empty() )
	{
		CDirOperate::DeleteAllDirectory(cstrDirectory);
		CDirOperate::CreateAllDirectory(cstrDirectory);
	}


	//读取服务器HASH列表
	SendMessage(m_hProgressWindow, WM_USER, (WPARAM)NOTIFY_MESSAGE, (LPARAM)"正在获取服务器文件列表...");
	TCHAR szServerPath[MAX_PATH]=TEXT(""); //服务器配置文件保存路径
	_snprintf(szNamePath, sizeof(szNamePath), "%s%s\\%s", szPath, CONFIG_PATH, UPDATE_CONFIG_FILENAME);
	_snprintf(szServerPath, sizeof(szServerPath), "%s%s",szPath,DOWNDLOAD_TMP_PATH);
	CDirOperate::CreateAllDirectory(szServerPath);//创建配置文件保存目录
	_snprintf(szServerPath, sizeof(szServerPath), "%s%s\\%s", szPath, DOWNDLOAD_TMP_PATH, UPDATE_CONFIG_TMP_FILENAME);
	GetPrivateProfileString(_T("UpdateAllFile"), _T("URL"), _T("http://192.168.1.5/update/hallfile/"), szNamePath, MAX_PATH, szNamePath);
	_snprintf(szNamePath, sizeof(szNamePath), "%s%s/%s", szNamePath, CONFIG_PATH, UPDATE_CONFIG_FILENAME);

	if (FALSE == DownFile(szNamePath, szServerPath, NULL, g_pCGameUpdateDlg->m_hWnd))
	{
		DWORD dwError = GetLastError();
		LOG(0, 0, "Error: 下载服务器配置文件失败 错误号:%ld 文件路径: %s", dwError, szNamePath);
		return;
	}

	//读取服务器文件列表
	iIndex=0;
	while (1)
	{
		_snprintf(szKeyword, sizeof(szKeyword), "_SignleFileUpdate__%d", iIndex++);
		GetPrivateProfileString(szKeyword, _T("name"), _T(""), szResultfile, MAX_PATH, szServerPath);
		GetPrivateProfileString(szKeyword, _T("size"), _T(""), szResultSize, MAX_PATH, szServerPath);
		GetPrivateProfileString(szKeyword, _T("path"), _T(""), szResultPath, MAX_PATH, szServerPath);
		GetPrivateProfileString(szKeyword, _T("hash"), _T(""), szResultHash, MAX_PATH, szServerPath);

		if (0==strcmp(szResultfile, "") || 0==strcmp(szResultPath, ""))
			break;

		FileHashTable filehashtable;
		filehashtable.filename = szResultfile;
		filehashtable.size = atol(szResultSize);
		filehashtable.path = szResultPath;
		filehashtable.Hash = szResultHash;
		string::size_type sfileNameIndex = filehashtable.filename.find(".");
		if (0<=sfileNameIndex && filehashtable.filename.size()>sfileNameIndex)
			filehashtable.DATName = filehashtable.filename.substr(sfileNameIndex, filehashtable.filename.size()-sfileNameIndex);
		ServerFileList.insert(make_pair(string(szResultPath), filehashtable));
	}

	//对比本地与服务器文件 HASH 确定哪些文件需要更新
	SendMessage(m_hProgressWindow, WM_USER, (WPARAM)NOTIFY_MESSAGE, (LPARAM)"正在对比服务器与客户端文件列表...");
	for (map<string, FileHashTable>::iterator begServer=ServerFileList.begin(); begServer!=ServerFileList.end(); begServer++)
	{
		map<string, FileHashTable>::iterator begLocal=localFileList.begin();
		for (; begLocal!=localFileList.end(); begLocal++)
		{
			if (string::npos!=begServer->first.find(begLocal->first))
			{
				if (begLocal->second.Hash!=begServer->second.Hash)
				{
					m_setWantUpdateFile.insert(*begServer);
				}
				break;
			}
		}

		if (begLocal == localFileList.end())
			m_setWantUpdateFile.insert(*begServer);
	}
}

void CUpdateThread::CheckDirectoryUpdate(void)
{
	TCHAR szNamePath[MAX_PATH]=TEXT(""); //本地配置文件路径
	TCHAR szKeyword[MAX_PATH]=TEXT("");//更新文件字段
	
	TCHAR szResultfile[MAX_PATH]=TEXT("");//返回文件名结果
	TCHAR szResultPath[MAX_PATH]=TEXT("");//返回路径结果
	TCHAR szResultHash[MAX_PATH]=TEXT("");//返回HASH结果
	TCHAR szResultSize[MAX_PATH]=TEXT("");//返回文件大小

	CString szPath(GetAppDirectory());
	_snprintf(szNamePath, sizeof(szNamePath), "%s%s\\%s", szPath, CONFIG_PATH, UPDATE_CONFIG_FILENAME);
	CString strFile;
	map<string, string> localDirectoryList,serverDirectoryList; //保存目录更新

	SendMessage(m_hProgressWindow, WM_USER, (WPARAM)NOTIFY_MESSAGE, (LPARAM)"正在获取服务器目录列表...");
	TCHAR szServerPath[MAX_PATH]=TEXT(""); //服务器配置文件保存路径
	_snprintf(szNamePath, sizeof(szNamePath), "%s%s\\%s", szPath, CONFIG_PATH, UPDATE_CONFIG_FILENAME);
	_snprintf(szServerPath, sizeof(szServerPath), "%s%s\\%s", szPath, DOWNDLOAD_TMP_PATH, UPDATE_CONFIG_TMP_FILENAME);
	
	//读取本地更新目录列表
	int iIndex=0;
	SendMessage(m_hProgressWindow, WM_USER, (WPARAM)NOTIFY_MESSAGE, (LPARAM)"正在读取本地更新目录列表...");
	while (1)
	{
		_snprintf(szKeyword, sizeof(szKeyword), "_SignleDirectoryUpdate__%d", iIndex++);
		GetPrivateProfileString(szKeyword, _T("name"), _T(""), szResultfile, MAX_PATH, szNamePath);
		GetPrivateProfileString(szKeyword, _T("hash"), _T(""), szResultHash, MAX_PATH, szNamePath);
		if (0==strcmp(szResultHash, "") || 0==strcmp(szResultfile, ""))
			break;

		localDirectoryList.insert(make_pair(szResultfile, szResultHash));
	}

	//读取服务器更新目录列表
	iIndex=0;
	while (1)
	{
		_snprintf(szKeyword, sizeof(szKeyword), "_SignleDirectoryUpdate__%d", iIndex++);
		GetPrivateProfileString(szKeyword, _T("name"), _T(""), szResultfile, MAX_PATH, szServerPath);
		GetPrivateProfileString(szKeyword, _T("hash"), _T(""), szResultHash, MAX_PATH, szServerPath);
		if (0==strcmp(szResultHash, "") || 0==strcmp(szResultfile, ""))
			break;

		serverDirectoryList.insert(make_pair(szResultfile, szResultHash));
	}

	//对比本地与服务器目录 HASH 确定哪些文件需要更新
	map<string, string> mResultDirectory;
	SendMessage(m_hProgressWindow, WM_USER, (WPARAM)NOTIFY_MESSAGE, (LPARAM)"正在对比服务器与客户端目录列表...");
	for (map<string, string>::iterator begServer=serverDirectoryList.begin(); begServer!=serverDirectoryList.end(); begServer++)
	{
		map<string, string>::iterator begLocal=localDirectoryList.begin();
		for (; begLocal!=localDirectoryList.end(); begLocal++)
		{
			if (string::npos!=begServer->first.find(begLocal->first))
			{
				if (begLocal->second!=begServer->second)
				{
					mResultDirectory.insert(*begServer);
				}
				break;
			}
		}

		if (begLocal == localDirectoryList.end())
			mResultDirectory.insert(*begServer);
	}

	//检查目录为单位的更新单元是否有变动，有变动则再检查旗下的文件
	int iServerIndex=0;
	map<string, FileHashTable> mLocal, mServer;
	for (map<string, string>::iterator mbeg=mResultDirectory.begin(); mbeg!=mResultDirectory.end(); mbeg++)
	{
		iIndex=0;
		while (1)
		{
			_snprintf(szKeyword, sizeof(szKeyword), "%s%d", mbeg->first.c_str(), iIndex++);
			GetPrivateProfileString(szKeyword, _T("name"), _T(""), szResultfile, MAX_PATH, szNamePath);
			GetPrivateProfileString(szKeyword, _T("size"), _T(""), szResultSize, MAX_PATH, szNamePath);
			GetPrivateProfileString(szKeyword, _T("path"), _T(""), szResultPath, MAX_PATH, szNamePath);
			GetPrivateProfileString(szKeyword, _T("hash"), _T(""), szResultHash, MAX_PATH, szNamePath);

			if (0==strcmp(szResultfile, "") || 0==strcmp(szResultPath, ""))
				break;

			FileHashTable filehashtable;
			filehashtable.filename = szResultfile;
			filehashtable.size = atol(szResultSize);
			filehashtable.path = szResultPath;
			filehashtable.Hash = szResultHash;
			string::size_type sfileNameIndex = filehashtable.filename.find(".");
			if (0<=sfileNameIndex && filehashtable.filename.size()>sfileNameIndex)
				filehashtable.DATName = filehashtable.filename.substr(sfileNameIndex, filehashtable.filename.size()-sfileNameIndex);
			mLocal.insert(make_pair(string(szResultPath), filehashtable));
		}

		iServerIndex=0;
		while (1)
		{
			_snprintf(szKeyword, sizeof(szKeyword), "%s%d", mbeg->first.c_str(), iServerIndex++);
			GetPrivateProfileString(szKeyword, _T("name"), _T(""), szResultfile, MAX_PATH, szServerPath);
			GetPrivateProfileString(szKeyword, _T("size"), _T(""), szResultSize, MAX_PATH, szServerPath);
			GetPrivateProfileString(szKeyword, _T("path"), _T(""), szResultPath, MAX_PATH, szServerPath);
			GetPrivateProfileString(szKeyword, _T("hash"), _T(""), szResultHash, MAX_PATH, szServerPath);

			if (0==strcmp(szResultfile, "") || 0==strcmp(szResultPath, ""))
				break;

			FileHashTable filehashtable;
			filehashtable.filename = szResultfile;
			filehashtable.size = atol(szResultSize);
			filehashtable.path = szResultPath;
			filehashtable.Hash = szResultHash;
			string::size_type sfileNameIndex = filehashtable.filename.find(".");
			if (0<=sfileNameIndex && filehashtable.filename.size()>sfileNameIndex)
				filehashtable.DATName = filehashtable.filename.substr(sfileNameIndex, filehashtable.filename.size()-sfileNameIndex);
			mServer.insert(make_pair(string(szResultPath), filehashtable));
		}
	}


	//对比本地与服务器目录下的文件 HASH 确定哪些文件需要更新
	SendMessage(m_hProgressWindow, WM_USER, (WPARAM)NOTIFY_MESSAGE, (LPARAM)"正在对比服务器与客户端目录下的文件列表...");
	for (map<string, FileHashTable>::iterator begServer=mServer.begin(); begServer!=mServer.end(); begServer++)
	{
		map<string, FileHashTable>::iterator begLocal=mLocal.begin();
		for (; begLocal!=mLocal.end(); begLocal++)
		{
			if (string::npos!=begServer->first.find(begLocal->first))
			{
				if (begLocal->second.Hash!=begServer->second.Hash)
				{
					m_setWantUpdateFile.insert(*begServer);
				}
				break;
			}
		}

		if (begLocal == mLocal.end())
			m_setWantUpdateFile.insert(*begServer);
	}
}

void CUpdateThread::CheckWantUpdateFile(void)
{
	TCHAR szNamePath[MAX_PATH]=TEXT(""); //本地配置文件路径
	CString szPath(GetAppDirectory());
	_snprintf(szNamePath, sizeof(szNamePath), "%s%s\\%s", szPath, CONFIG_PATH, UPDATE_CONFIG_FILENAME);

	TCHAR szServerPath[MAX_PATH]=TEXT(""); //服务器配置文件保存路径
	_snprintf(szServerPath, sizeof(szServerPath), "%s%s\\%s", szPath, DOWNDLOAD_TMP_PATH, UPDATE_CONFIG_TMP_FILENAME);

	CString cstrDirectory(GetAppDirectory());
	cstrDirectory.Append(DOWNDLOAD_TMP_PATH);

	// 单个文件检查
	CheckSingleFileUpdate();
	// 目录检查（有些指定的目录先目录检查，再检查旗下的文件）
	CheckDirectoryUpdate();

	if (m_setWantUpdateFile.empty())
	{
		// 更新最新MD5 Hash 配置文件
		_snprintf(szNamePath, sizeof(szNamePath), "%s%s\\%s", szPath, CONFIG_PATH, UPDATE_CONFIG_FILENAME);
		SetFileAttributes(szNamePath, FILE_ATTRIBUTE_NORMAL);
		DeleteFile(szNamePath);
		MoveFile(szServerPath, szNamePath);
		CDirOperate::DeleteAllDirectory(cstrDirectory);

		RMessageBox("您已经是最新版本！");
		g_pCGameUpdateDlg->ShowWindow(SW_HIDE);
		g_pCGameUpdateDlg->TransferExternalProgram();
		g_pCGameUpdateDlg->PostMessage(WM_CLOSE);
		return;
	}

	SendMessage(m_hProgressWindow, WM_USER, (WPARAM)NOTIFY_MESSAGE, (LPARAM)"开始升级...");
}

BOOL CUpdateThread::DoGenerateMD5Hash(BOOL bShush)
{
//	//获取目录
//	CString szPath(GetAppDirectory());
//	string sCurPath(szPath.GetBuffer(0));
//	szPath.ReleaseBuffer();
//	// 得到所有的文件集
//	multimap<string, FileHashTable> AllFileHashStruct; //保存计算后的所有文件名集与对应的HASH、路径
//	//g_pCGameUpdateDlg->SearchCatalogueAllFile(AllFileHashStruct, sCurPath, sCurPath);
//	//过滤的文件名
//	vector<string> leachFileName;
//
//	UINT uIndex=1;
//	TCHAR tchUpdateFile[MAX_PATH]=_T("");
//	TCHAR tchUpdatePath[MAX_PATH]=_T("");
//	TCHAR tchUpdateKey[MAX_PATH]=_T("");
//	_snprintf(tchUpdatePath, sizeof(tchUpdatePath), "%sconfig\\%s", szPath, UPDATE_SET_INI_FILE);
//	vector<string> updatefile;
//	TCHAR szServerPath[MAX_PATH]=TEXT(""); //服务器配置文件保存路径
//	_snprintf(szServerPath, sizeof(szServerPath), "%s%s",szPath,DOWNDLOAD_TMP_PATH);
//
//	// 得到更新目录与文件名
//	CFile file;
//	TCHAR szHaseList[MAX_PATH]=TEXT(""); //本地配置文件路径
//	_snprintf(szHaseList, sizeof(szHaseList), "%s%s\\%s", szPath, CONFIG_PATH, UPDATE_CONFIG_FILENAME);
//	GetPrivateProfileString(_T("UpdateAllFile"), _T("GameHaseList"), _T(""), szHaseList, MAX_PATH, szHaseList);
//	
//	if (TRUE == bShush)
//	{
//		//update.ini文件读取失败，需要重新生成，生成又需要updateset.ini
//		//updateset.ini （强调）每次都需要从服务器下载，因为他可能会有变动
//		SetProgressPos(bShush, 0);
//		if ( (0==lstrcmp("", szHaseList)) || FALSE==file.Open(tchUpdatePath, CFile::modeRead) )
//		{
//			SendMessage(m_hProgressWindow, WM_USER, (WPARAM)NOTIFY_MESSAGE, (LPARAM)"下载服务器相关生成配置文件！");
//			TCHAR szNamePath[MAX_PATH]=TEXT(""); //本地配置文件路径
//			CDirOperate::CreateAllDirectory(szServerPath);//创建配置文件保存目录
//			_snprintf(szServerPath, sizeof(szServerPath), "%s%s\\%s", szPath, DOWNDLOAD_TMP_PATH, UPDATE_SET_INI_TMP_FILE);
//			_snprintf(szNamePath, sizeof(szNamePath), "http://%s/update/hallfile/config/updateset.dat", g_pCGameUpdateDlg->m_chURL);
//			OutputDebugString(szNamePath);
//
//			int iReturn;
//			DownFile(szNamePath, szServerPath, NULL, g_pCGameUpdateDlg->m_hWnd);
//			GetPrivateProfileString(_T("set"), _T("URL"), _T(""), szHaseList, MAX_PATH, szServerPath);
//
//			if (0 == lstrlen(szHaseList))
//			{
//				iReturn = RMessageBox(_T("游戏修复需要的相关配置文件下载失败！"), MB_OK);
//				if (IDOK == iReturn)
//				{
//					g_pCGameUpdateDlg->PostMessage(WM_CLOSE);
//					return FALSE;
//				}
//				//else
//				//{
//				//	while (IDRETRY == iReturn)
//				//	{
//				//		// 尝试再次下载
//				//		DownFile(szNamePath, szServerPath, NULL, g_pCGameUpdateDlg->m_hWnd);
//				//		// 检查下载的文件合法性,通过获取文件中的UpdateURL字段判断文件合法性
//				//		GetPrivateProfileString(_T("set"), _T("UpdateURL"), _T(""), szHaseList, MAX_PATH, szServerPath);
//				//		if (0 != lstrlen(szHaseList))
//				//			break;
//
//				//		iReturn = RMessageBox(_T("游戏修复需要的相关配置文件下载失败！"), MB_RETRYCANCEL);
//				//	}
//				//}
//			}
//		}
//		else
//			file.Close(); //不关闭，下面无法读取
//	}
//
//
//	while (1)
//	{
//		_snprintf(tchUpdateKey, sizeof(tchUpdateKey), "directory%d", uIndex++);
//		// 修复使用tmpupdate\updatesetserver.ini文件，生成使用config\updateset.ini
//		GetPrivateProfileString(_T("enforce"), tchUpdateKey, _T(""), tchUpdateFile, MAX_PATH, (TRUE==bShush?szServerPath:tchUpdatePath));
//
//		if (0 == strcmp("", tchUpdateFile))
//			break;
//
//		string strFile(tchUpdateFile);
//		updatefile.push_back(strFile);
//	}
//
//	SetProgressPos(bShush, 10);
//	//在这指定需要更新的文件及目录
//	unsigned char acMD5Digest[16];
//	int iCount = 1;
//	int iTotalCount = AllFileHashStruct.size();
//	CString strContent;
//	multimap<string, FileHashTable> tmpAllFileHashStruct;
//	for (multimap<string, FileHashTable>::iterator beg=AllFileHashStruct.begin(); beg!=AllFileHashStruct.end(); beg++)
//	{
//		vector<string>::iterator vbeg=updatefile.begin();
//		for (; vbeg!=updatefile.end(); vbeg++)
//		{
//			if (0==beg->second.path.find(*vbeg))
//			{
//				tmpAllFileHashStruct.insert(tmpAllFileHashStruct.begin(), *beg);
//				break;
//			}
//		}
//
//		if (vbeg==updatefile.end())
//		{
//			string strSubStr = beg->second.path.substr(beg->second.path.size()-4, 4);
//
//			if (beg->first.size()+1==beg->second.path.size())
//			{
//				if (0 == strSubStr.compare(".exe"))
//				{
//					SendMessage(m_hProgressWindow, WM_USER, (WPARAM)NOTIFY_LEACH_FILE_NAME, (LPARAM)beg->second.path.c_str());
//					continue;				
//				}
//				tmpAllFileHashStruct.insert(tmpAllFileHashStruct.begin(), *beg);
//			}
//		}
//	}
//
//	SetProgressPos(bShush, 15);
//	// 生成相应MD5 HASH值
//	for (multimap<string, FileHashTable>::iterator beg=tmpAllFileHashStruct.begin(); beg!=tmpAllFileHashStruct.end(); beg++,iCount++)
//	{
//		string strTmpPath(szPath.GetBuffer(0));
//		szPath.ReleaseBuffer();
//		strTmpPath.append(beg->second.path);
//		CalculateMD5(strTmpPath.c_str(), acMD5Digest);
//		
//		CString strHash = MD5toString(acMD5Digest);
//		beg->second.Hash = strHash.GetBuffer();
//		strHash.ReleaseBuffer();
//		
//		strContent.Format("%s单个文件生成: [%d/%d]", ((TRUE==bShush)?"修复 ":""), iCount, iTotalCount); 
//		SendMessage(m_hProgressWindow, WM_USER, (WPARAM)((TRUE==bShush)?NOTIFY_MESSAGE:NOTIFY_GENERATE_MESSAGE), (LPARAM)strContent.GetBuffer(0));
//		SendMessage(m_hProgressWindow, WM_USER, (WPARAM)NOTIFY_GENERATE_FILE_NAME, (LPARAM)strTmpPath.c_str());
//		strContent.ReleaseBuffer();
//	}
//
//
//	// 读取以目录为单位检查MD5值，并一起更新的所有路径
//	uIndex=1;
//	memset(tchUpdateFile, 0, sizeof(tchUpdateFile));
//	memset(tchUpdateKey, 0, sizeof(tchUpdateKey));
//	vector<string> vDirectoryUntix;
//	while (1)
//	{
//		_snprintf(tchUpdateKey, sizeof(tchUpdateKey), "unit%d", uIndex++);
//		GetPrivateProfileString(_T("directory"), tchUpdateKey, _T(""), tchUpdateFile, MAX_PATH, (TRUE==bShush?szServerPath:tchUpdatePath));
//		if (0 == strcmp("", tchUpdateFile))
//			break;
//
//		string strFile(tchUpdateFile);
//		vDirectoryUntix.push_back(strFile);
//	}
//
//
//	// 保存MD5 Hash到ini文件中
//	int iIndex=0;
//	TCHAR szIniPath[MAX_PATH]=TEXT(""),szFileSize[MAX_PATH]=TEXT("");
//	_snprintf(szIniPath, sizeof(szIniPath), "%s%s", szPath, CONFIG_PATH);
//	CDirOperate::CreateAllDirectory(szIniPath);
//	_snprintf(szIniPath, sizeof(szIniPath), "%s%s\\%s", szPath, CONFIG_PATH, UPDATE_CONFIG_FILENAME);
//	SetFileAttributes(szIniPath, FILE_ATTRIBUTE_NORMAL);
//	::DeleteFile(szIniPath);
//	
//	string strLower;
//	CString strFile,strAllHash;
//	//之所以这里 GameHaseList 写个初值，因为我不希望统计数据GameHaseList 放到ini 文件的后面
//	//当然真实准确的数据是在strAllHash中计算得到并写入的
//	WritePrivateProfileString(_T("UpdateAllFile"), _T("GameHaseList"), _T(""), szIniPath);
//
//	// 单独生成配置文件的过滤字符串已经在点击生成时设置好
//	// 这里不需要再设置，重复设置也会出错，（因为设置过滤的编辑框与显示已经过滤的编辑框是同一个IDC_RICHEDIT21控件）
//	if (TRUE == bShush)
//	{
//		;//g_pCGameUpdateDlg->SetLeachFileName(bShush);
//	}
//	//set< string > setTmpLeachName = g_pCGameUpdateDlg->GetLeachName();
//
//	iIndex=0;
//	iCount = 1;
//	iTotalCount = tmpAllFileHashStruct.size();
//	multimap<string, FileHashTable> mDirectoryUnit;
//	
//	for (multimap<string, FileHashTable>::iterator beg=tmpAllFileHashStruct.begin(); beg!=tmpAllFileHashStruct.end(); beg++,iCount++)
//	{
//		// 部分以目录为整体更新的包先不写入，待会归入相应目录字段中，而不是_SignleFileUpdate__字段
//		vector<string>::iterator vbeg=vDirectoryUntix.begin();
//		for (; vbeg!=vDirectoryUntix.end(); vbeg++)
//		{
//			if (0 == beg->second.path.find(*vbeg))
//				break;
//		}
//		if (vDirectoryUntix.end() != vbeg)
//		{
//			beg->second.filename = beg->first;
//			mDirectoryUnit.insert(mDirectoryUnit.begin(), make_pair(*vbeg, beg->second));
//			continue;
//		}
//
//
//		// 部分 文件名 设置不需要升级，在这里过滤
//		strLower = beg->first.c_str();
//		transform(strLower.begin(), strLower.end(), strLower.begin(), towlower);
//		if (setTmpLeachName.end()!=setTmpLeachName.find(strLower))
//		{
//			SendMessage(m_hProgressWindow, WM_USER, (WPARAM)NOTIFY_LEACH_FILE_NAME, (LPARAM)beg->second.path.c_str());
//			continue;
//		}
//
//		// 部分 扩展名 设置不需要升级，在这里过滤
//		strLower = beg->second.DATName.c_str();
//		transform(strLower.begin(), strLower.end(), strLower.begin(), towlower);
//		if (setTmpLeachName.end()!=setTmpLeachName.find(strLower))
//		{
//			SendMessage(m_hProgressWindow, WM_USER, (WPARAM)NOTIFY_LEACH_FILE_NAME, (LPARAM)beg->second.path.c_str());
//			continue;
//		}
//
//		strFile.Format("_SignleFileUpdate__%d", iIndex++);
//		_snprintf(szFileSize, sizeof(szFileSize), "%I64u", beg->second.size);
//		WritePrivateProfileString(strFile, _T("name"), beg->first.c_str(), szIniPath);
//		WritePrivateProfileString(strFile, _T("size"), szFileSize, szIniPath);
//		WritePrivateProfileString(strFile, _T("path"), beg->second.path.c_str(), szIniPath);
//		WritePrivateProfileString(strFile, _T("hash"), beg->second.Hash.c_str(), szIniPath);
//		strAllHash.Append(beg->second.Hash.c_str());
//
//		strContent.Format("%s单个文件保存: [%d/%d]", ((TRUE==bShush)?"修复 ":""), iCount, iTotalCount);
//		SendMessage(m_hProgressWindow, WM_USER, (WPARAM)((TRUE==bShush)?NOTIFY_MESSAGE:NOTIFY_GENERATE_MESSAGE), (LPARAM)strContent.GetBuffer(0));
//
//		strContent.ReleaseBuffer();
//		string strTmpPath(szPath.GetBuffer(0));
//		szPath.ReleaseBuffer();
//		strTmpPath.append(beg->second.path);
//		SendMessage(m_hProgressWindow, WM_USER, (WPARAM)NOTIFY_GENERATE_FILE_NAME, (LPARAM)strTmpPath.c_str());
//	}
//
//
//	SetProgressPos(bShush, 45);
//	// 分类目录下的文件写入INI
//	iIndex=0;
//	iCount = 1;
//	map<string, string> mDirectory;
//	iTotalCount = mDirectoryUnit.size();
//	string sLastDirectory("");
//	for (multimap<string, FileHashTable>::iterator mbeg=mDirectoryUnit.begin(); mbeg!=mDirectoryUnit.end(); mbeg++,iCount++)
//	{
//		// 部分 文件名 设置不需要升级，在这里过滤
//		strLower = mbeg->second.filename.c_str();
//		transform(strLower.begin(), strLower.end(), strLower.begin(), towlower);
//		if (setTmpLeachName.end()!=setTmpLeachName.find(strLower))
//		{
//			SendMessage(m_hProgressWindow, WM_USER, (WPARAM)NOTIFY_LEACH_FILE_NAME, (LPARAM)mbeg->second.path.c_str());
//			continue;
//		}
//
//		// 部分 扩展名 设置不需要升级，在这里过滤
//		strLower = mbeg->second.DATName.c_str();
//		transform(strLower.begin(), strLower.end(), strLower.begin(), towlower);
//		if (setTmpLeachName.end()!=setTmpLeachName.find(strLower))
//		{
//			SendMessage(m_hProgressWindow, WM_USER, (WPARAM)NOTIFY_LEACH_FILE_NAME, (LPARAM)mbeg->second.path.c_str());
//			continue;
//		}
//
//		pair<map<string, string>::iterator, bool> pairResult = mDirectory.insert(make_pair(mbeg->first, mbeg->second.Hash));
//		if ( !pairResult.second )
//		{
//			pairResult.first->second.append(mbeg->second.Hash);
//		}
//
//		if (0 != sLastDirectory.compare(mbeg->first))
//		{
//			iIndex = 0;
//		}
//
//		strFile.Format("%s%d", mbeg->first.c_str(), iIndex++);
//		strFile.Replace(" ", "");
//		strFile.Replace("\\\\", "_");
//		strFile.Replace("\\", "_");
//		strFile.Replace("/", "_");
//		_snprintf(szFileSize, sizeof(szFileSize), "%I64u", mbeg->second.size);
//		WritePrivateProfileString(strFile, _T("name"), mbeg->second.filename.c_str(), szIniPath);
//		WritePrivateProfileString(strFile, _T("size"), szFileSize, szIniPath);
//		WritePrivateProfileString(strFile, _T("path"), mbeg->second.path.c_str(), szIniPath);
//		WritePrivateProfileString(strFile, _T("hash"), mbeg->second.Hash.c_str(), szIniPath);
//		strAllHash.Append(mbeg->second.Hash.c_str());
//
//		strContent.Format("%s指定目录包文件保存: [%d/%d]", ((TRUE==bShush)?"修复 ":""), iCount, iTotalCount);
//		SendMessage(m_hProgressWindow, WM_USER, (WPARAM)((TRUE==bShush)?NOTIFY_MESSAGE:NOTIFY_GENERATE_MESSAGE), (LPARAM)strContent.GetBuffer(0));
//		strContent.ReleaseBuffer();
//
//		string strTmpPath(szPath.GetBuffer(0));
//		szPath.ReleaseBuffer();
//		strTmpPath.append(mbeg->second.path);
//		SendMessage(m_hProgressWindow, WM_USER, (WPARAM)NOTIFY_GENERATE_FILE_NAME, (LPARAM)strTmpPath.c_str());
//		sLastDirectory = mbeg->first;
//	}
//
//
//	SetProgressPos(bShush, 90);
//	// 分类目录写入INI文件
//	iIndex=0;
//	iCount = 1;
//	iTotalCount = mDirectory.size();
//	for (map<string, string>::iterator mbeg=mDirectory.begin(); mbeg!=mDirectory.end(); mbeg++,iCount++,iIndex++)
//	{
//		CalculateStringMD5(mbeg->second.c_str(), mbeg->second.size(), acMD5Digest);
//		CString strResult(MD5toString(acMD5Digest));
//		CString strDirectory(mbeg->first.c_str());
//		strDirectory.Replace(" ", "");
//		strDirectory.Replace("\\\\", "_");
//		strDirectory.Replace("\\", "_");
//		strDirectory.Replace("/", "_");
//
//		strFile.Format("_SignleDirectoryUpdate__%d", iIndex);
//		WritePrivateProfileString(strFile, _T("name"), strDirectory, szIniPath);
//		WritePrivateProfileString(strFile, _T("hash"), strResult, szIniPath);
//		strAllHash.Append(mbeg->second.c_str());
//
//		strContent.Format("%s指定目录包目录保存: [%d/%d]", ((TRUE==bShush)?"修复 ":""), iCount, iTotalCount);
//		SendMessage(m_hProgressWindow, WM_USER, (WPARAM)((TRUE==bShush)?NOTIFY_MESSAGE:NOTIFY_GENERATE_MESSAGE), (LPARAM)strContent.GetBuffer(0));
//		strContent.ReleaseBuffer();
//
//		string strTmpPath(szPath.GetBuffer(0));
//		szPath.ReleaseBuffer();
//		strTmpPath.append(mbeg->first);
//		SendMessage(m_hProgressWindow, WM_USER, (WPARAM)NOTIFY_GENERATE_FILE_NAME, (LPARAM)strTmpPath.c_str());		
//	}
//
//
//	SetProgressPos(bShush, 98);
//	//生成游戏HASH值用来判断是否可以升级
//	CalculateStringMD5((LPCTSTR)strAllHash, strAllHash.GetLength(), acMD5Digest);
//	CString strResult(MD5toString(acMD5Digest));
//	WritePrivateProfileString(_T("UpdateAllFile"), _T("GameHaseList"), (LPCTSTR)strResult, szIniPath);
//	//设置更新地址
//	TCHAR szResultVal[MAX_PATH]=TEXT(""), szDefaultPath[MAX_PATH];
//	_snprintf(szDefaultPath, sizeof(szDefaultPath), _T("http://%s/update/hallfile/"), g_pCGameUpdateDlg->m_chURL);
//	GetPrivateProfileString(_T("set"), _T("URL"), szDefaultPath, szResultVal, MAX_PATH, (TRUE==bShush?szServerPath:tchUpdatePath));
//	WritePrivateProfileString(_T("UpdateAllFile"), _T("URL"), szResultVal, szIniPath);
//	_snprintf(szDefaultPath, sizeof(szDefaultPath), _T("http://%s/update.php?action=hall"), g_pCGameUpdateDlg->m_chURL);
//	//GetPrivateProfileString(_T("set"), _T("UpdateURL"), szDefaultPath, szResultVal, MAX_PATH, (TRUE==bShush?szServerPath:tchUpdatePath));
//	//WritePrivateProfileString(_T("UpdateAllFile"), _T("UpdateURL"), szResultVal, szIniPath);
//
//	   
//	if (FALSE == bShush)
//	{
//		g_pCGameUpdateDlg->m_btGenerateHash.EnableWindow();
//		g_pCGameUpdateDlg->m_btGenerateHash.SetWindowText(_T("完成(&C)"));
//	}
//
//	_snprintf(szServerPath, sizeof(szServerPath), "%s%s",szPath,DOWNDLOAD_TMP_PATH);
//	CDirOperate::DeleteAllDirectory(szServerPath);
//	
//	szPath.Append("config\\");
//	CFile fileServerSet;
//	if (fileServerSet.Open(szPath+UPDATE_SET_INI_FILE, CFile::modeRead))
//	{
//		fileServerSet.Close();
//		CFile::Rename(szPath+UPDATE_SET_INI_FILE, szPath+TEXT("updateset .dat"));
//	}
//
//	SetProgressPos(bShush, 100);
//	SendMessage(m_hProgressWindow, WM_USER, (WPARAM)NOTIFY_GENERATE_MESSAGE, (LPARAM)"生成成功  ");
	return TRUE;
}

void CUpdateThread::SetProgressPos(BOOL bShush, int iPos)
{
	//if ( !bShush || NULL==g_pCGameUpdateDlg || FALSE==g_pCGameUpdateDlg->m_ProgressUpdate.IsWindowVisible())
		return;

	//g_pCGameUpdateDlg->m_ProgressUpdate.SetPos(iPos);
}
