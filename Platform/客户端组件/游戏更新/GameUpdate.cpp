// GameUpdate.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "GameUpdate.h"
#include "UISkinManager.h"
#include "RMessageBox.h"
#include "GameUpdateDlg.h"
#include "Generate.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// 全局字符串常量表数组（数组初始化在InitStringTable()函数中进行）
struct StringStru g_String[STRING_BOTTOM];

// 全局语言代码（不可随意修改其值，否则会导致程序异常！）
enum enLANGUAGE g_LanguageID;

/////////////////////////////////////////////////////////////////////////////
// CGameUpdateApp

BEGIN_MESSAGE_MAP(CGameUpdateApp, CWinApp)
	//{{AFX_MSG_MAP(CGameUpdateApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGameUpdateApp construction

CGameUpdateApp::CGameUpdateApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
	m_bSilenceMode = FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CGameUpdateApp object

CGameUpdateApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CGameUpdateApp initialization

BOOL CGameUpdateApp::InitInstance()
{
	// 初始化全局字符串常量表数组
	InitStringTable();

	// 检查程序是否已经在运行，如是则直接退出
	if (IsAppRunning())
	{
		return FALSE;
	}
	
	AfxInitRichEdit2();
	AfxEnableControlContainer();	

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	// 初始化自绘界面
	bool bGenerate=false;
	
	//获取目录
	CString szPath(GetAppDirectory());
	szPath.Append("config\\");
	szPath.Append(UPDATE_SET_INI_FILE);
	CFile file;

	//AfxDebugBreak();//Test

	CStringArray arr;
	GetCmdLinePara(arr);//获得命令行	

	if(3 != arr.GetSize())
	{
		bGenerate = true;
	}

	//bGenerate = FALSE;//Test

	// 界面初始化
	CUISkinManager::Init(bGenerate);

	if (bGenerate)
	{
		if (file.Open(szPath, CFile::modeRead))
		{
			file.Close();		
		}
		else
		{
			return FALSE;
		}

		CGenerate GenerateDlg;
		m_pMainWnd = &GenerateDlg;
		GenerateDlg.DoModal();
		return TRUE;
	}

	// 显示主窗口
	CGameUpdateDlg dlg;
	m_pMainWnd = &dlg;
	dlg.m_sAppName = m_sAppName;
	dlg.m_bSilenceMode = m_bSilenceMode;
	dlg.m_bGenerateIniFile = bGenerate;
	
	if (3 == arr.GetSize())
	{
		string strTmp;
		// 得到外部程序名
		strTmp = arr.GetAt(0);
		string::size_type sIndex = strTmp.find(TEXT("/modulefilename:"));
		if (0 != sIndex)
			return FALSE;
		dlg.m_strExternalProgramFileName = strTmp.erase(0, strlen(TEXT("/modulefilename:")));

		// 得到登陆名
		strTmp = arr.GetAt(1);
		sIndex = strTmp.find("/account:");
		if (0 != sIndex)
			return FALSE;
		dlg.m_strAccount = strTmp.erase(0, strlen("/account:"));

		// 得到密码
		strTmp = arr.GetAt(2);
		sIndex = strTmp.find("/pw:");
		if (0 != sIndex)
			return FALSE;
		dlg.m_strPW = strTmp.erase(0, strlen("/pw:"));
	}

	//if (m_bSilenceMode)
	//{
	//	// 静默方式执行升级
	//	m_pMainWnd = NULL;
	//	dlg.SetConfigFile(GetAppDirectory() + UPDATE_CONFIG_FILENAME);
	//	dlg.CheckWantUpdateFile();
	//}
	//else
	{
		int nResponse = dlg.DoModal();
	}

	// 清理自绘界面
	CUISkinManager::Uninit();

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

// 处理命令行
// 返回值为 TRUE 表示要继续运行后面的程序，为 FALSE 表示终止程序运行
//BOOL CGameUpdateApp::ParseCommandLine()
//{
//	CString sParameter;
//
//	// 检查命令行参数
//
//	for (int i = 1; i < __argc; i++)
//	{
//		sParameter = __argv[i];
//		if (sParameter.Left(strlen(PARA_KEY_APP_NAME)).CompareNoCase(PARA_KEY_APP_NAME) == 0)
//		{
//			m_sAppName = sParameter.Mid(strlen(PARA_KEY_APP_NAME));
//		}
//		else if (sParameter.Left(strlen(PARA_KEY_CURRENTVERSION)).CompareNoCase(PARA_KEY_CURRENTVERSION) == 0)
//		{
//			m_sVersion = sParameter.Mid(strlen(PARA_KEY_CURRENTVERSION));
//		}
//		else if (sParameter.Left(strlen(PARA_KEY_CHECKURL)).CompareNoCase(PARA_KEY_CHECKURL) == 0)
//		{
//			m_sURL = sParameter.Mid(strlen(PARA_KEY_CHECKURL));
//		}
//		else if (sParameter.Left(strlen(PARA_KEY_NOTIFYWINDOW)).CompareNoCase(PARA_KEY_NOTIFYWINDOW) == 0)
//		{
//			m_iNotifyWindow = (DWORD)atoi(sParameter.Mid(strlen(PARA_KEY_NOTIFYWINDOW)));
//		}
//		else if (sParameter.Left(strlen(PARA_KEY_NOTIFYWINDOWTITLE)).CompareNoCase(PARA_KEY_NOTIFYWINDOWTITLE) == 0)
//		{
//			m_sNotifyWindowTitle = sParameter.Mid(strlen(PARA_KEY_NOTIFYWINDOWTITLE));
//		}
//		else if (sParameter.Left(strlen(PARA_KEY_NOTIFYFINISH)).CompareNoCase(PARA_KEY_NOTIFYFINISH) == 0)
//		{
//			m_iNotifyFinish = (DWORD)atoi(sParameter.Mid(strlen(PARA_KEY_NOTIFYFINISH)));
//		}
//		else if (sParameter.Left(strlen(PARA_KEY_SILENCE)).CompareNoCase(PARA_KEY_SILENCE) == 0)
//		{
//			m_bSilenceMode = (sParameter.Mid(strlen(PARA_KEY_SILENCE)) == "1");
//		}
//		else
//		{
//			LOG(0, 0, "Invalid parameter : %s", sParameter.GetBuffer(0));
//			return FALSE;
//		}
//	}
//
//	m_sAppName.TrimLeft();
//	m_sAppName.TrimRight();
//	if (m_sAppName.IsEmpty())
//	{
//		LOG(0, 0, "Invalid parameters.");
//		return FALSE;
//	}
//
//	m_sVersion.TrimLeft();
//	m_sVersion.TrimRight();
//	if (m_sVersion.IsEmpty())
//	{
//		LOG(0, 0, "Invalid parameters.");
//		return FALSE;
//	}
//
//	// 检查升级配置文件，判断是否有新版本的软件可用
//	if (CheckUpdate())
//	{
//		if (!m_bSilenceMode)
//		{
//			CRMessageBox MsgBox;
//			MsgBox.m_sTitle = m_sAppName + " " + STRING(STR_AUTO_UPDATE, "Auto Update");
//			MsgBox.m_sPromptMessage = STRING(STR_PROMPT_NEWER_VERSION_AVAILABLE, "当前有更新版本的软件可用，是否立即升级？");
//			MsgBox.m_bOption1 = FALSE;
//			MsgBox.m_sOptionPromptMessage1 = STRING(STR_OPTION_UPGRADE_IN_BACKGROUND , "继续运行程序，从后台执行升级。");
//			MsgBox.m_iType = MB_YESNO + MB_ICONQUESTION;
//			if (IDOK == MsgBox.DoModal())
//			{
//				m_bSilenceMode = MsgBox.m_bOption1;
//
//				if (MsgBox.m_iID == IDYES)
//				{
//					return TRUE;
//				}
//				else
//				{
//					return FALSE;
//				}
//			}
//			else
//			{
//				return FALSE;
//			}
//		}
//	}
//
//	return TRUE;
//}

// 从软件网站下载升级配置文件，检查是否有新版本的软件可用
// 返回值为 TRUE 表示有新版本的软件可用
//BOOL CGameUpdateApp::CheckUpdate()
//{
//	CString sConfigFilename = GetAppDirectory() + UPDATE_CONFIG_FILENAME;
//
//	// 从指定的URL下载升级配置文件
//	if (!m_sURL.IsEmpty())
//	{
//		if (Internet::InternetGetURL(m_sURL.GetBuffer(0), sConfigFilename.GetBuffer(0))
//			!= Internet::INTERNET_SUCCESS)
//		{
//			LOG(0, 0, "Fail to download file %s", m_sURL.GetBuffer(0));
//			return FALSE;
//		}
//	}
//
//	// 从升级配置文件取得最新软件版本号
//	CString sKey = "Version";
//	const int BUFFER_SIZE = 512;
//	char acBuffer[BUFFER_SIZE] = {0};
//	GetPrivateProfileString(SECTION_UPDATE, sKey.GetBuffer(0), ""
//		, acBuffer, BUFFER_SIZE, sConfigFilename.GetBuffer(0));
//	CString sVersion = (char*)acBuffer;
//
//	// 与当前软件版本号比较以确定是否需要升级
//	if (sVersion > m_sVersion)
//	{
//		return TRUE;
//	}
//	else
//	{
//		return FALSE;
//	}
//}

// 初始化全局字符串常量表
void CGameUpdateApp::InitStringTable(enum enLANGUAGE Language)
{
	if (Language < LANGUAGE_BOTTOM)
	{
		g_LanguageID = Language;
	}
	else
	{
		// 根据操作系统语言代码确定程序界面的语言代码
		switch (GetSystemDefaultLangID()) 
		{
		case 0x0804: // Chinese (PRC)
		case 0x1004: // Chinese (Singapore)
			g_LanguageID = LANGUAGE_GB;
			break;
		case 0x0404: // Chinese (Taiwan)
		case 0x0c04: // Chinese (Hong Kong SAR, PRC)
		case 0x1404: // Chinese (Macao)
			g_LanguageID = LANGUAGE_BIG5;
			break;
		default:
			g_LanguageID = LANGUAGE_ENGLISH;
			break;
		}
	}

// for test
//g_LanguageID=LANGUAGE_ENGLISH;

	// 初始化全局字符串常量表

	g_String[STR_NULL].Set("", "", "");
	g_String[STR_AUTO_UPDATE].Set("自动升级", "笆ど", "Auto Update");
	g_String[STR_APP_ALREADY_RUNNING].Set("升级程序已经在运行中！", "ど祘竒笲︽い", "Auto update program is already running."); //  退出应用程序；提示保存文档\n退出

	g_String[STR_PROMPT_NEWER_VERSION_AVAILABLE].Set("当前有更新版本的软件可用，是否立即升级？", "讽玡Τ穝セ硁砰ノ琌ミど", "There is a newer version available. Do you want to update right now?");
	g_String[STR_OPTION_UPGRADE_IN_BACKGROUND].Set("后台执行升级", "磅︽ど", "Run updating in background mode");
	g_String[STR_PROMPT_UPGRADE_READY].Set("升级准备就绪。", "ど非称碞狐", "Update is ready. Please press \"Start update\" button to update.");
	g_String[STR_PROMPT_FAIL_TO_OPEN_UPDATE_CONFIG_FILE].Set("打开升级配置文件失败，无法执行升级！", "ゴ秨ど皌竚郎ア毖礚猭磅︽ど", "Fail to open update config file. Update is canceled.");
	g_String[STR_PROMPT_DOWNLOADING_FILE].Set("正在下载文件 %s", "タ更郎 %s", "Downloading file %s");
	g_String[STR_TOTAL_UPGRADE_PROGRESS].Set("升级总进度 [%d \/ %d]  速度: %.2fKB\/S", "ど羆秈 %d / %d", "Total progress %d / %d");
	g_String[STR_PROMPT_FAIL_IN_DOWNLOADING_FILES].Set("下载文件 %s 失败！", "更郎 %s ア毖", "Fail in downloading file %s!");
	g_String[STR_PROMPT_FAIL_IN_VERIFYING_FILES].Set("校验文件 %s 失败！", "喷郎 %s ア毖", "Fail in verifying file %s!");
	g_String[STR_PROMPT_FAIL_IN_UPDATING_FILES].Set("更新文件 %s 失败！", "穝郎 %s ア毖", "Fail in updating file %s!");
	g_String[STR_PROMPT_FAIL_IN_UPDATING].Set("升级失败！", "どア毖", "Fail in updating!");
	g_String[STR_PROMPT_UPGRADE_FINISHED].Set("升级完毕！", "どЧ拨", "Update finished!");

	g_String[STR_BUTTON_START_UPGRADE].Set("开始升级", "秨﹍ど", "Start update");
	g_String[STR_BUTTON_CANCEL_UPGRADE].Set("取   消", "ど", "Cancel");

	g_String[STR_BUTTON_OK].Set("确定(&O)", "絋﹚(&O)", "&OK");
	g_String[STR_BUTTON_CANCEL].Set("取消(&C)", "(&C)", "&Cancel");
	g_String[STR_BUTTON_ABORT].Set("跳出(&A)", "铬(&A)", "&Abort");
	g_String[STR_BUTTON_IGANORE].Set("忽略(&I)", "┛菠(&I)", "&Ignore");
	g_String[STR_BUTTON_RETRY].Set("重试(&R)", "刚(&R)", "&Retry");
	g_String[STR_BUTTON_CONTINUE].Set("继续(&C)", "膥尿(&C)", "&Continue");
	g_String[STR_BUTTON_YES].Set("是(&Y)", "琌(&Y)", "&Yes");
	g_String[STR_BUTTON_NO].Set("否(&N)", "(&N)", "&No");
	g_String[STR_BUTTON_CLOSE].Set("关闭", "闽超", "Close");
	g_String[STR_BUTTON_APPLY].Set("应用(&A)", "甅ノ(&A)", "&Apply");

	g_String[STR_OTHER].Set("其他", "ㄤ", "Other");

	g_String[STR_ERROR].Set("错误", "岿粇", "Error");
	g_String[STR_ERROR_MESSAGE].Set("错误：%s", "岿粇%s", "Error: %s");
}

BOOL CGameUpdateApp::IsAppRunning()
{
	// 创建互斥量，防止同时启动多个程序实例
	CString sMutex = m_sAppName + "GameUpdateMutex";
	if (::OpenMutex(MUTEX_ALL_ACCESS, FALSE, sMutex.GetBuffer(0)))
	{
		return TRUE;
	}
	else
	{
		::CreateMutex(0, FALSE, sMutex.GetBuffer(0));
		sMutex.ReleaseBuffer();
		return FALSE;
	}
}

//字符串分解
void CGameUpdateApp::GetCmdLinePara(CStringArray &paraArr)
{
	paraArr.RemoveAll();
	//--"/modulefilename:GamePlaza.exe /account:a11111 /pw:dc483e80a7a0bd9ef71d8cf973673924"
	CString strLine=::AfxGetApp()->m_lpCmdLine;
	if(strLine.IsEmpty())
		return;

	int nLength=strLine.GetLength();
	char *buf=new char[nLength+1];
	lstrcpyn(buf, strLine, sizeof(char)*(nLength+1));
	char *p=buf;
	for(int i=0;i<128;i++)
	{
		if(buf[i]==0x20)//空格
		{
			buf[i]=0x00;
			paraArr.Add(p);
			i++;
			p=buf+i;
		}
		if(buf[i]==0x00)
		{
			paraArr.Add(p);
			break;
		}
	}

	delete []buf;
}