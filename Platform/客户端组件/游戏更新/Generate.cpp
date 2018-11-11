// Generate.cpp : 实现文件
//

#include "stdafx.h"
#include "resource.h"
#include <shlobj.h>
#include "UpdateThread.h"
#include "UISkinManager.h"
#include "GameUpdateDlg.h"
#include ".\GameUpdatedlg.h"
#include "Generate.h"
#include ".\generate.h"

// CGenerate 对话框

IMPLEMENT_DYNAMIC(CGenerate, CRDialog)
CGenerate::CGenerate(CWnd* pParent /*=NULL*/)
	: CRDialog(CGenerate::IDD, pParent)
{
	m_hThread = NULL;
	m_bSilenceMode = FALSE;
	m_bStopGenerate = FALSE;
	m_bComplete = FALSE;
	m_bDowndloadServerSetFile = FALSE;
}

CGenerate::~CGenerate()
{
	m_setLeachName.clear();
	m_strLeachFileName.clear();
}

void CGenerate::DoDataExchange(CDataExchange* pDX)
{
	CRDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDOK, m_btGenerateHash);
	DDX_Control(pDX, IDC_RICHEDIT21, m_ctrLeachfile);
	DDX_Text(pDX, IDC_GENERATE_MESSAGE, m_sGenerateProgressMsg);
}

BOOL CGenerate::OnInitDialog()
{
	CRDialog::OnInitDialog();

	m_sGenerateProgressMsg.Empty();

	CString	sFileName("");
	CString sFilePath("");
	GetModuleFileName(NULL, sFileName.GetBufferSetLength(128), 128);
	sFilePath=sFileName.Left(sFileName.ReverseFind('\\') + 1) + TEXT("config\\config.ini");
	GetPrivateProfileString(TEXT("ServersAddr"), TEXT("HallDowndloadURL"), TEXT(""), m_chURL, 128, sFilePath);

	//获取目录
	CString szPath(GetAppDirectory());
	szPath.Append("config\\");
	szPath.Append(UPDATE_SET_INI_FILE);
	TCHAR szResultVal[MAX_PATH]=TEXT("");
	GetPrivateProfileString(_T("set"), _T("filter"), _T("updateset.ini;"), szResultVal, MAX_PATH, szPath);

	// 生成MD5 Hash 相应文件
	SetWindowText(_T("生成更新配置文件"));
	m_btGenerateHash.SetWindowText(_T("生成(&C)"));
	//m_ctrLeachfile.SetWindowPos(NULL, 10, 60, 351, 122, SWP_NOACTIVATE|SWP_NOSENDCHANGING);
	m_ctrLeachfile.ShowWindow(SW_SHOW);
	m_ctrLeachfile.SetWindowText(szResultVal);
	m_btGenerateHash.ShowWindow(SW_SHOW);

	//GetDlgItem(IDC_STATIC_LEACH)->ShowWindow(SW_SHOW);
//	GetDlgItem(IDC_GENERATE_MESSAGE)->ShowWindow(SW_HIDE);

	m_setLeachName.clear();
	//m_strLeachFileName.clear();

	if (m_bSilenceMode)
	{
		OnBnGenerateIniFile();
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

BOOL CGenerate::GenerateMD5Hash()
{
	//获取目录
	CString szPath(GetAppDirectory());
	string sCurPath(szPath.GetBuffer(0));
	szPath.ReleaseBuffer();
	// 得到所有的文件集
	multimap<string, FileHashTable> AllFileHashStruct; //保存计算后的所有文件名集与对应的HASH、路径
	SearchCatalogueAllFile(AllFileHashStruct, sCurPath, sCurPath);
	//过滤的文件名
	vector<string> leachFileName;

	UINT uIndex=1;
	TCHAR tchUpdateFile[MAX_PATH]=_T("");
	TCHAR tchUpdatePath[MAX_PATH]=_T("");
	TCHAR tchUpdateKey[MAX_PATH]=_T("");
	_snprintf(tchUpdatePath, sizeof(tchUpdatePath), "%sconfig\\%s", szPath, UPDATE_SET_INI_FILE);
	vector<string> updatefile;
	TCHAR szServerPath[MAX_PATH]=TEXT(""); //服务器配置文件保存路径
	_snprintf(szServerPath, sizeof(szServerPath), "%s%s\\%s",szPath, DOWNDLOAD_TMP_PATH, UPDATE_SET_INI_TMP_FILE);

	// 下载生成配置文件
	DowndloadGenerateDeployFile(szPath.GetBufferSetLength(MAX_PATH), tchUpdatePath);
	szPath.ReleaseBufferSetLength(MAX_PATH);

	while (1)
	{
		if (m_bStopGenerate)
			return FALSE;

		_snprintf(tchUpdateKey, sizeof(tchUpdateKey), "directory%d", uIndex++);
		// 修复使用tmpupdate\updatesetserver.ini文件，生成使用config\updateset.ini
		GetPrivateProfileString(_T("enforce"), tchUpdateKey, _T(""), tchUpdateFile, MAX_PATH, 
			(TRUE==m_bDowndloadServerSetFile?szServerPath:tchUpdatePath));

		if (0 == strcmp("", tchUpdateFile))
			break;

		string strFile(tchUpdateFile);
		updatefile.push_back(strFile);
	}

	//在这指定需要更新的文件及目录
	unsigned char acMD5Digest[16];
	int iCount = 1;
	int iTotalCount = AllFileHashStruct.size();
	CString strContent;
	multimap<string, FileHashTable> tmpAllFileHashStruct;
	for (multimap<string, FileHashTable>::iterator beg=AllFileHashStruct.begin(); beg!=AllFileHashStruct.end(); beg++)
	{
		if (m_bStopGenerate)
			return FALSE;

		vector<string>::iterator vbeg=updatefile.begin();
		for (; vbeg!=updatefile.end(); vbeg++)
		{
			if (0==beg->second.path.find(*vbeg))
			{
				tmpAllFileHashStruct.insert(tmpAllFileHashStruct.begin(), *beg);
				break;
			}
		}

		if (vbeg==updatefile.end())
		{
			string strSubStr = beg->second.path.substr(beg->second.path.size()-4, 4);

			if (beg->first.size()+1==beg->second.path.size())
			{
				if (0 == strSubStr.compare(".exe"))
				{
					PostMessage(WM_USER_MSG, (WPARAM)NOTIFY_LEACH_FILE_NAME, (LPARAM)beg->second.path.c_str());
					continue;				
				}
				tmpAllFileHashStruct.insert(tmpAllFileHashStruct.begin(), *beg);
			}
		}
	}

	// 生成相应MD5 HASH值
	for (multimap<string, FileHashTable>::iterator beg=tmpAllFileHashStruct.begin(); beg!=tmpAllFileHashStruct.end(); beg++,iCount++)
	{
		if (m_bStopGenerate)
			return FALSE;

		string strTmpPath(szPath.GetBuffer(0));
		szPath.ReleaseBuffer();
		strTmpPath.append(beg->second.path);
		CalculateMD5(strTmpPath.c_str(), acMD5Digest);

		CString strHash = MD5toString(acMD5Digest);
		beg->second.Hash = strHash.GetBuffer();
		strHash.ReleaseBuffer();

		strContent.Format("%s单个文件生成: [%d/%d]", ((TRUE==m_bSilenceMode)?"修复 ":""), iCount, iTotalCount); 
		PostMessage(WM_USER_MSG, (WPARAM)((TRUE==m_bSilenceMode)?NOTIFY_MESSAGE:NOTIFY_GENERATE_MESSAGE), (LPARAM)strContent.GetBuffer(0));
		PostMessage(WM_USER_MSG, (WPARAM)NOTIFY_GENERATE_FILE_NAME, (LPARAM)strTmpPath.c_str());
		strContent.ReleaseBuffer();
	}


	// 读取以目录为单位检查MD5值，并一起更新的所有路径
	uIndex=1;
	memset(tchUpdateFile, 0, sizeof(tchUpdateFile));
	memset(tchUpdateKey, 0, sizeof(tchUpdateKey));
	vector<string> vDirectoryUntix;
	while (1)
	{
		if (m_bStopGenerate)
			return FALSE;

		_snprintf(tchUpdateKey, sizeof(tchUpdateKey), "unit%d", uIndex++);
		GetPrivateProfileString(_T("directory"), tchUpdateKey, _T(""), tchUpdateFile, MAX_PATH, 
			(TRUE==m_bDowndloadServerSetFile?szServerPath:tchUpdatePath));
		if (0 == strcmp("", tchUpdateFile))
			break;

		string strFile(tchUpdateFile);
		vDirectoryUntix.push_back(strFile);
	}

	// 保存MD5 Hash到ini文件中
	int iIndex=0;
	TCHAR szIniPath[MAX_PATH]=TEXT(""),szFileSize[MAX_PATH]=TEXT("");
	_snprintf(szIniPath, sizeof(szIniPath), "%s%s", szPath, CONFIG_PATH);
	CDirOperate::CreateAllDirectory(szIniPath);
	_snprintf(szIniPath, sizeof(szIniPath), "%s%s\\%s", szPath, CONFIG_PATH, UPDATE_CONFIG_FILENAME);
	SetFileAttributes(szIniPath, FILE_ATTRIBUTE_NORMAL);
	::DeleteFile(szIniPath);

	string strLower;
	CString strFile,strAllHash;
	//之所以这里 GameHaseList 写个初值，因为我不希望统计数据GameHaseList 放到ini 文件的后面
	//当然真实准确的数据是在strAllHash中计算得到并写入的
	WritePrivateProfileString(_T("UpdateAllFile"), _T("GameHaseList"), _T(""), szIniPath);
	WritePrivateProfileString(_T("priority"), _T("file1"), _T(""), szIniPath);

	// 单独生成配置文件的过滤字符串已经在点击生成时设置好
	// 这里不需要再设置，重复设置也会出错，（因为设置过滤的编辑框与显示已经过滤的编辑框是同一个IDC_RICHEDIT21控件）
	if (TRUE == m_bDowndloadServerSetFile)
	{
		SetLeachFileName(m_bDowndloadServerSetFile);
	}

	set< string > setTmpLeachName = m_setLeachName;

	iIndex=0;
	iCount = 1;
	iTotalCount = tmpAllFileHashStruct.size();
	multimap<string, FileHashTable> mDirectoryUnit;

	for (multimap<string, FileHashTable>::iterator beg=tmpAllFileHashStruct.begin(); beg!=tmpAllFileHashStruct.end(); beg++,iCount++)
	{
		if (m_bStopGenerate)
			return FALSE;

		// 部分以目录为整体更新的包先不写入，待会归入相应目录字段中，而不是_SignleFileUpdate__字段
		vector<string>::iterator vbeg=vDirectoryUntix.begin();
		for (; vbeg!=vDirectoryUntix.end(); vbeg++)
		{
			if (0 == beg->second.path.find(*vbeg))
				break;
		}
		if (vDirectoryUntix.end() != vbeg)
		{
			beg->second.filename = beg->first;
			mDirectoryUnit.insert(mDirectoryUnit.begin(), make_pair(*vbeg, beg->second));
			continue;
		}

		// 部分 文件名 设置不需要升级，在这里过滤
		strLower = beg->first.c_str();
		transform(strLower.begin(), strLower.end(), strLower.begin(), towlower);
		if (setTmpLeachName.end()!=setTmpLeachName.find(strLower))
		{
			PostMessage(WM_USER_MSG, (WPARAM)NOTIFY_LEACH_FILE_NAME, (LPARAM)beg->second.path.c_str());
			continue;
		}

		// 部分 扩展名 设置不需要升级，在这里过滤
		strLower = beg->second.DATName.c_str();
		transform(strLower.begin(), strLower.end(), strLower.begin(), towlower);
		if (setTmpLeachName.end()!=setTmpLeachName.find(strLower))
		{
			PostMessage(WM_USER_MSG, (WPARAM)NOTIFY_LEACH_FILE_NAME, (LPARAM)beg->second.path.c_str());
			continue;
		}

		strFile.Format("_SignleFileUpdate__%d", iIndex++);
		_snprintf(szFileSize, sizeof(szFileSize), "%I64u", beg->second.size);
		WritePrivateProfileString(strFile, _T("name"), beg->first.c_str(), szIniPath);
		WritePrivateProfileString(strFile, _T("size"), szFileSize, szIniPath);
		WritePrivateProfileString(strFile, _T("path"), beg->second.path.c_str(), szIniPath);
		WritePrivateProfileString(strFile, _T("hash"), beg->second.Hash.c_str(), szIniPath);
		strAllHash.Append(beg->second.Hash.c_str());

		strContent.Format("%s单个文件保存: [%d/%d]", ((TRUE==m_bDowndloadServerSetFile)?"修复 ":""), iCount, iTotalCount);
		PostMessage(WM_USER_MSG, (WPARAM)((TRUE==m_bDowndloadServerSetFile)?NOTIFY_MESSAGE:NOTIFY_GENERATE_MESSAGE), (LPARAM)strContent.GetBuffer(0));

		strContent.ReleaseBuffer();
		string strTmpPath(szPath.GetBuffer(0));
		szPath.ReleaseBuffer();
		strTmpPath.append(beg->second.path);
		PostMessage(WM_USER_MSG, (WPARAM)NOTIFY_GENERATE_FILE_NAME, (LPARAM)strTmpPath.c_str());
	}

	// 分类目录下的文件写入INI
	iIndex=0;
	iCount = 1;
	map<string, string> mDirectory;
	iTotalCount = mDirectoryUnit.size();
	string sLastDirectory("");
	for (multimap<string, FileHashTable>::iterator mbeg=mDirectoryUnit.begin(); mbeg!=mDirectoryUnit.end(); mbeg++,iCount++)
	{
		if (m_bStopGenerate)
			return FALSE;

		// 部分 文件名 设置不需要升级，在这里过滤
		strLower = mbeg->second.filename.c_str();
		transform(strLower.begin(), strLower.end(), strLower.begin(), towlower);
		if (setTmpLeachName.end()!=setTmpLeachName.find(strLower))
		{
			PostMessage(WM_USER_MSG, (WPARAM)NOTIFY_LEACH_FILE_NAME, (LPARAM)mbeg->second.path.c_str());
			continue;
		}

		// 部分 扩展名 设置不需要升级，在这里过滤
		strLower = mbeg->second.DATName.c_str();
		transform(strLower.begin(), strLower.end(), strLower.begin(), towlower);
		if (setTmpLeachName.end()!=setTmpLeachName.find(strLower))
		{
			PostMessage(WM_USER_MSG, (WPARAM)NOTIFY_LEACH_FILE_NAME, (LPARAM)mbeg->second.path.c_str());
			continue;
		}

		pair<map<string, string>::iterator, bool> pairResult = mDirectory.insert(make_pair(mbeg->first, mbeg->second.Hash));
		if ( !pairResult.second )
		{
			pairResult.first->second.append(mbeg->second.Hash);
		}

		if (0 != sLastDirectory.compare(mbeg->first))
		{
			iIndex = 0;
		}

		strFile.Format("%s%d", mbeg->first.c_str(), iIndex++);
		strFile.Replace(" ", "");
		strFile.Replace("\\\\", "_");
		strFile.Replace("\\", "_");
		strFile.Replace("/", "_");
		_snprintf(szFileSize, sizeof(szFileSize), "%I64u", mbeg->second.size);
		WritePrivateProfileString(strFile, _T("name"), mbeg->second.filename.c_str(), szIniPath);
		WritePrivateProfileString(strFile, _T("size"), szFileSize, szIniPath);
		WritePrivateProfileString(strFile, _T("path"), mbeg->second.path.c_str(), szIniPath);
		WritePrivateProfileString(strFile, _T("hash"), mbeg->second.Hash.c_str(), szIniPath);
		strAllHash.Append(mbeg->second.Hash.c_str());

		strContent.Format("%s指定目录包文件保存: [%d/%d]", ((TRUE==m_bDowndloadServerSetFile)?"修复 ":""), iCount, iTotalCount);
		PostMessage(WM_USER_MSG, (WPARAM)((TRUE==m_bDowndloadServerSetFile)?NOTIFY_MESSAGE:NOTIFY_GENERATE_MESSAGE), (LPARAM)strContent.GetBuffer(0));
		strContent.ReleaseBuffer();

		string strTmpPath(szPath.GetBuffer(0));
		szPath.ReleaseBuffer();
		strTmpPath.append(mbeg->second.path);
		PostMessage(WM_USER_MSG, (WPARAM)NOTIFY_GENERATE_FILE_NAME, (LPARAM)strTmpPath.c_str());
		sLastDirectory = mbeg->first;
	}


	// 分类目录写入INI文件
	iIndex=0;
	iCount = 1;
	iTotalCount = mDirectory.size();
	for (map<string, string>::iterator mbeg=mDirectory.begin(); mbeg!=mDirectory.end(); mbeg++,iCount++,iIndex++)
	{
		if (m_bStopGenerate)
			return FALSE;

		CalculateStringMD5(mbeg->second.c_str(), mbeg->second.size(), acMD5Digest);
		CString strResult(MD5toString(acMD5Digest));
		CString strDirectory(mbeg->first.c_str());
		strDirectory.Replace(" ", "");
		strDirectory.Replace("\\\\", "_");
		strDirectory.Replace("\\", "_");
		strDirectory.Replace("/", "_");

		strFile.Format("_SignleDirectoryUpdate__%d", iIndex);
		WritePrivateProfileString(strFile, _T("name"), strDirectory, szIniPath);
		WritePrivateProfileString(strFile, _T("hash"), strResult, szIniPath);
		strAllHash.Append(mbeg->second.c_str());

		strContent.Format("%s指定目录包目录保存: [%d/%d]", ((TRUE==m_bDowndloadServerSetFile)?"修复 ":""), iCount, iTotalCount);
		PostMessage(WM_USER_MSG, (WPARAM)((TRUE==m_bDowndloadServerSetFile)?NOTIFY_MESSAGE:NOTIFY_GENERATE_MESSAGE), (LPARAM)strContent.GetBuffer(0));
		strContent.ReleaseBuffer();

		string strTmpPath(szPath.GetBuffer(0));
		szPath.ReleaseBuffer();
		strTmpPath.append(mbeg->first);
		PostMessage(WM_USER_MSG, (WPARAM)NOTIFY_GENERATE_FILE_NAME, (LPARAM)strTmpPath.c_str());		
	}


	//生成游戏HASH值用来判断是否可以升级
	CalculateStringMD5((LPCTSTR)strAllHash, strAllHash.GetLength(), acMD5Digest);
	CString strResult(MD5toString(acMD5Digest));
	WritePrivateProfileString(_T("UpdateAllFile"), _T("GameHaseList"), (LPCTSTR)strResult, szIniPath);
	//设置更新地址
	TCHAR szResultVal[MAX_PATH]=TEXT(""), szFieldPath[MAX_PATH]=TEXT(""), szDefaultPath[MAX_PATH];
	_snprintf(szDefaultPath, sizeof(szDefaultPath), _T("http://%s/update/hallfile/"), m_chURL);
	GetPrivateProfileString(_T("set"), _T("URL"), szDefaultPath, szResultVal, MAX_PATH, (TRUE==m_bDowndloadServerSetFile?szServerPath:tchUpdatePath));
	WritePrivateProfileString(_T("UpdateAllFile"), _T("URL"), szResultVal, szIniPath);
	GetPrivateProfileString(_T("set"), _T("WriteLog"), "1", szResultVal, MAX_PATH, (TRUE==m_bDowndloadServerSetFile?szServerPath:tchUpdatePath));
	WritePrivateProfileString(_T("UpdateAllFile"), _T("WriteLog"), szResultVal, szIniPath);
	GetPrivateProfileString(_T("set"), _T("ThreadCount"), "1", szResultVal, MAX_PATH, (TRUE==m_bDowndloadServerSetFile?szServerPath:tchUpdatePath));
	WritePrivateProfileString(_T("UpdateAllFile"), _T("ThreadCount"), szResultVal, szIniPath);
	GetPrivateProfileString(_T("set"), _T("DownloadMode"), "1", szResultVal, MAX_PATH, (TRUE==m_bDowndloadServerSetFile?szServerPath:tchUpdatePath));
	WritePrivateProfileString(_T("UpdateAllFile"), _T("DownloadMode"), szResultVal, szIniPath);

	//_snprintf(szDefaultPath, sizeof(szDefaultPath), _T("http://%s/update.php?action=hall"), m_chURL);
	//GetPrivateProfileString(_T("set"), _T("UpdateURL"), szDefaultPath, szResultVal, MAX_PATH, (TRUE==m_bDowndloadServerSetFile?szServerPath:tchUpdatePath));
	//WritePrivateProfileString(_T("UpdateAllFile"), _T("UpdateURL"), szResultVal, szIniPath);


	int iFlag = 1;
	do
	{
		_snprintf(szFieldPath, sizeof(szFieldPath), _T("file%d"), iFlag++);
		GetPrivateProfileString(_T("priority"), szFieldPath, "", szResultVal, MAX_PATH, (TRUE==m_bDowndloadServerSetFile?szServerPath:tchUpdatePath));
		WritePrivateProfileString(_T("priority"), szFieldPath, szResultVal, szIniPath);
	}while (0 < lstrlen(szResultVal));


	if (FALSE == m_bDowndloadServerSetFile)
	{
		m_btGenerateHash.EnableWindow();
		m_btGenerateHash.SetWindowText(_T("完成(&C)"));
	}

	_snprintf(szServerPath, sizeof(szServerPath), "%s%s",szPath,DOWNDLOAD_TMP_PATH);
	CDirOperate::DeleteAllDirectory(szServerPath);

	szPath.Append("config\\");
	CFile fileServerSet;
	if (fileServerSet.Open(szPath+UPDATE_SET_INI_FILE, CFile::modeRead))
	{
		fileServerSet.Close();
		CFile::Rename(szPath+UPDATE_SET_INI_FILE, szPath+TEXT("updateset.dat"));
	}

	m_bComplete = TRUE;
	
 	if (m_bSilenceMode)
	{
		PostMessage(WM_CLOSE);
	}

	PostMessage(WM_USER_MSG, (WPARAM)NOTIFY_GENERATE_MESSAGE, (LPARAM)"生成成功  ");
	return TRUE;
}

BOOL CGenerate::DowndloadGenerateDeployFile(const string strPath, const string strUpdatePath)
{
	if (TRUE == m_bDowndloadServerSetFile)
	{
		// 得到更新目录与文件名
		CFile file;
		TCHAR szHaseList[MAX_PATH]=TEXT(""); //本地配置文件路径
		_snprintf(szHaseList, sizeof(szHaseList), "%s%s\\%s", strPath.c_str(), CONFIG_PATH, UPDATE_CONFIG_FILENAME);
		GetPrivateProfileString(_T("UpdateAllFile"), _T("GameHaseList"), _T(""), szHaseList, MAX_PATH, szHaseList);

		//update.ini文件读取失败，需要重新生成，生成又需要updateset.ini
		//updateset.ini （强调）每次都需要从服务器下载，因为他可能会有变动
		if ( (0==lstrcmp("", szHaseList)) || FALSE==file.Open(strUpdatePath.c_str(), CFile::modeRead) )
		{
			PostMessage(WM_USER_MSG, (WPARAM)NOTIFY_MESSAGE, (LPARAM)"下载服务器相关生成配置文件！");
			TCHAR szNamePath[MAX_PATH]=TEXT(""); //本地配置文件路径
			TCHAR szServerPath[MAX_PATH]=TEXT(""); //保存到本地的服务器配置文件
			_snprintf(szServerPath, sizeof(szServerPath), "%s%s", strPath.c_str(), DOWNDLOAD_TMP_PATH);
			CDirOperate::CreateAllDirectory(szServerPath);//创建配置文件保存目录
			_snprintf(szServerPath, sizeof(szServerPath), "%s%s\\%s", strPath.c_str(), DOWNDLOAD_TMP_PATH, UPDATE_SET_INI_TMP_FILE);
			_snprintf(szNamePath, sizeof(szNamePath), "%sconfig/updateset.dat", m_chURL);
			OutputDebugString(szNamePath);

			DowndloadFile(this, szNamePath, szServerPath);
			//DownFile(szNamePath, szServerPath, NULL, m_hWnd);
			GetPrivateProfileString(_T("set"), _T("URL"), _T(""), szHaseList, MAX_PATH, szServerPath);

			if (0 == lstrlen(szHaseList))
			{
				LOG(0, 0, "游戏修复需要的相关配置文件下载失败 \"%s\".");
				PostMessage(WM_CLOSE);
				return FALSE;
			}
		}
		else
		{
			file.Close(); //不关闭，下面无法读取
		}
	}
}

void CGenerate::SearchCatalogueAllFile(multimap<string, FileHashTable> & vResult, string strFindPath, string strDelPath)
{
	CFileFind cPath;
	string strCmp(strFindPath);
	strCmp.append("\\*.*");	
	BOOL bFinded = cPath.FindFile(strCmp.c_str());

	while (bFinded)
	{
		bFinded = cPath.FindNextFile();

		//跳过“.”和“..”。
		if (cPath.IsDots())
			continue;

		//递归搜索子目录。
		if (TRUE == cPath.IsDirectory()) 
		{
			CString FilePath(cPath.GetFilePath());
			SearchCatalogueAllFile(vResult, FilePath.GetBuffer(0), strDelPath);
			FilePath.ReleaseBuffer();
		}

		if (FALSE == cPath.IsDirectory())
		{
			//是文件
			CString strPath = cPath.GetFilePath();
			CString strName = cPath.GetFileName();
			CString strDATName(strName);
			strDATName.Delete(0, strDATName.ReverseFind('.'));

			FileHashTable tmpFileHash;		
			strPath.Replace(strDelPath.c_str(), "");
			strPath.Insert(0, "\\");
			tmpFileHash.size = cPath.GetLength();
			tmpFileHash.DATName = strDATName.GetBuffer(0);
			tmpFileHash.path = strPath.GetBuffer(0);
			tmpFileHash.Hash = "";
			vResult.insert(make_pair(strName.GetBuffer(0), tmpFileHash));
			strDATName.ReleaseBuffer();
			strName.ReleaseBuffer();
			strPath.ReleaseBuffer();
		}
	}

	cPath.Close(); 
}

void CGenerate::SetLeachFileName(BOOL bLoadIni)
{
	CString strBuffer;
	m_ctrLeachfile.GetWindowText(strBuffer);

	if (bLoadIni)
	{
		//获取目录
		TCHAR tchVal[MAX_PATH];
		CString szPath(GetAppDirectory());
		szPath.Append(DOWNDLOAD_TMP_PATH);
		szPath.Append("\\");
		szPath.Append(UPDATE_SET_INI_TMP_FILE);
		TCHAR szResultVal[MAX_PATH]=TEXT("");
		GetPrivateProfileString(_T("set"), _T("filter"), _T("updateset.ini;"), tchVal, MAX_PATH, szPath);

		strBuffer.Empty();
		strBuffer.Format("%s", tchVal);
	}

	strBuffer.MakeLower();
	strBuffer.Replace(" ", "");
	strBuffer.Replace("；", ";");
	strBuffer.Replace("；；", ";");
	strBuffer.Replace(";;", ";");
	strBuffer.Replace("*", "");	

	if (0 != strBuffer.Find(';'))
		strBuffer.Insert(0, ';');
	if (strBuffer.GetLength()-1 != strBuffer.ReverseFind(';'))
		strBuffer.Insert(strBuffer.GetLength(), ';');

	int iStrStar = 0;
	int iStrEnd = 0;
	CString strTmp;
	iStrStar=strBuffer.Find(';', iStrStar);
	iStrEnd=strBuffer.Find(';', iStrStar+1);

	while (strBuffer.GetLength()>iStrEnd && iStrStar!=iStrEnd && 0<iStrEnd)
	{
		strTmp = strBuffer.Mid(iStrStar+1, iStrEnd-iStrStar-1);
		m_setLeachName.insert(strTmp.GetBuffer(0));
		strTmp.ReleaseBuffer();
		iStrStar=strBuffer.Find(';', iStrEnd);
		iStrEnd=strBuffer.Find(';', iStrStar+1);
	}

	if (m_setLeachName.end() != m_setLeachName.find("updateset.ini"))
		m_setLeachName.insert("updateset.ini");
	if (m_setLeachName.end() != m_setLeachName.find("update.ini"))
		m_setLeachName.insert("update.ini");
}

void CGenerate::OnSetLeachFileName(string str)
{
	if (0 == str.find("\\"))
	{
		str.erase(str.begin());
	}

	string strTmp;
	strTmp.clear();
	m_strLeachFileName.push_back(str);

	for (vector<string>::iterator beg=m_strLeachFileName.begin(); beg!=m_strLeachFileName.end(); beg++)
	{
		if (beg->empty())
			continue;

		strTmp.append(*beg);
		strTmp.append(TEXT("\n"));
	}

	m_ctrLeachfile.SetWindowText(strTmp.c_str());
	m_ctrLeachfile.PostMessage(WM_VSCROLL, SB_BOTTOM);

	CString strBuffer;
	strBuffer.Format("Result: %s",strTmp.c_str());
	OutputDebugString(strBuffer);
}

unsigned __stdcall CGenerate::ThreadGenerateWorkFun(LPVOID lParam)
{
	CGenerate * pWnd = (CGenerate *)lParam;
	pWnd->GenerateMD5Hash();
	return 0;
}

BOOL CGenerate::DowndloadFile(CGenerate *pWnd, const TCHAR szDowndLoadPath[MAX_PATH], 
								   const TCHAR szDowndLoadSavePath[MAX_PATH], long llFileIndex/*=-1*/)
{
	if (NULL == pWnd || 0==lstrlen(szDowndLoadPath) || 0==lstrlen(szDowndLoadSavePath))
	{
		return FALSE;
	}

	if (!pWnd->m_bSilenceMode)
	{
		pWnd->PostMessage(WM_USER_MSG, (WPARAM)NOTIFY_DOWNLOADING_FILENAME, (LPARAM)szDowndLoadPath);
	}

	// 开始下载文件
	CInternet vInternet;
	int iResult = vInternet.InternetGetURL(szDowndLoadPath, szDowndLoadSavePath, NULL, pWnd->GetSafeHwnd(), llFileIndex);
	if (CInternet::INTERNET_SUCCESS != iResult)
	{
		return FALSE;
	}

	CFile file;
	if ( file.Open(szDowndLoadSavePath, CFile::modeRead))
	{
		file.Close();
		LOG(0, 0, "下载成功 \"%s\".", szDowndLoadSavePath);
		return TRUE;
	}

	// 记录下载文件失败日志
	if (!pWnd->m_bSilenceMode)
	{
		TCHAR tchBuffer[MAX_PATH]=TEXT("");
		LOG(0, 0, _T("Error: 下载失败 \"%s\"."), szDowndLoadSavePath);
		_snprintf(tchBuffer, sizeof(tchBuffer), "%s", szDowndLoadSavePath);
		pWnd->PostMessage(WM_USER_MSG, (WPARAM)NOTIFY_DOWNLOAD_FILE_FAIL, (LPARAM)tchBuffer);
	}
	return FALSE;
}

BEGIN_MESSAGE_MAP(CGenerate, CRDialog)
	ON_BN_CLICKED(IDOK, OnBnGenerateIniFile)
	ON_WM_PAINT()
	ON_MESSAGE(WM_USER_MSG, OnUserMessage)
	ON_WM_LBUTTONDOWN()
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// CGenerate 消息处理程序

void CGenerate::OnBnGenerateIniFile()
{
	UpdateData(TRUE);
	CString strBuffer,strButton;
	m_ctrLeachfile.SetFocus();
	m_ctrLeachfile.GetWindowText(strBuffer);
	m_btGenerateHash.GetWindowText(strButton);

	if (-1!=strButton.Find(_T("完成")))
	{
		PostMessage(WM_CLOSE);
		return;
	}

	if (0>=strBuffer.GetLength())
	{
		int iResult = RMessageBox(_T("您确定不过滤文件吗？"), MB_YESNO);
		if (IDNO == iResult)
			return;

		Invalidate(FALSE);
		UpdateWindow();
	}

	//单独生成在这设置过滤文件名
	SetLeachFileName(FALSE);
	m_ctrLeachfile.SetReadOnly();
	m_ctrLeachfile.SetWindowText("");
	m_ctrLeachfile.SetBackgroundColor(FALSE, RGB(238,247,255));
	m_btGenerateHash.EnableWindow(FALSE);
	m_btGenerateHash.SetWindowText(_T("生成中..."));
	GetDlgItem(IDC_STATIC_PROMPT)->SetWindowText(_T("过滤列表:"));
	GetDlgItem(IDC_GENERATE_MESSAGE)->ShowWindow(SW_SHOW);
	//GetDlgItem(IDC_STATIC_LEACH)->SetWindowText(_T("以下文件被过滤："));

	unsigned int unThreadID = 0;
	m_hThread = (HANDLE)_beginthreadex(NULL, 0, &ThreadGenerateWorkFun, this, 0, &unThreadID);
}

void CGenerate::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 在此处添加消息处理程序代码
	// 不为绘图消息调用 CRDialog::OnPaint()
	CUISkinManager::Paint(this, &dc);
	CUISkinManager::PaintDialogBorder(this, &dc);
}

// 处理用户消息
LRESULT CGenerate::OnUserMessage(WPARAM wParam, LPARAM lParam)
{
	CString strBuffer;
	strBuffer.Format("Result: CGameUpdateDlg::OnUserMessage %d", wParam);
	TRACE(strBuffer);
	OutputDebugString(strBuffer);

	switch (wParam)
	{
	case NOTIFY_MESSAGE:				// (安静生成MD5，再升级时触发)通知界面提示消息
		{
			//m_strContent.Format("%s", (LPCTSTR)lParam);
		}
		break;
	case NOTIFY_GENERATE_MESSAGE:		// 通知生成提示消息
		{
			LPCTSTR ptchData = (LPCTSTR)lParam;
			if (NULL != ptchData)
			{
				m_sGenerateProgressMsg.Empty();
				m_sGenerateProgressMsg.Format(ptchData);
				UpdateData(FALSE);
			}
		}
		break;
	case NOTIFY_LEACH_FILE_NAME:		// 通知过滤的文件名
		{
			OnSetLeachFileName((LPCTSTR)lParam);
		}
		break;
	default: break;
	}
	return TRUE;
}
void CGenerate::OnCancel()
{
	m_bStopGenerate = TRUE;
	CRDialog::OnCancel();
}

BOOL CGenerate::DestroyWindow()
{
	// TODO: 在此添加专用代码和/或调用基类
	m_bStopGenerate = TRUE;
	// TODO: 在此添加专用代码和/或调用基类
	WaitForSingleObject(m_hThread, 5000);
	CloseHandle(m_hThread);
	return CRDialog::DestroyWindow();
}

void CGenerate::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	SendMessage(WM_NCLBUTTONDOWN, HTCAPTION, 0);
	CRDialog::OnLButtonDown(nFlags, point);
}

HBRUSH CGenerate::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CRDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	if (nCtlColor == CTLCOLOR_STATIC)
	{
		return (HBRUSH)CreateSolidBrush(RGB(238,247,255));
	}

	// TODO:  如果默认的不是所需画笔，则返回另一个画笔
	return hbr;
}
