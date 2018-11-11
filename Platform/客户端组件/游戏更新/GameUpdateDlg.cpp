// GameUpdateDlg.cpp : implementation file
//

#include "stdafx.h"
#include <shlobj.h>
#include "GameUpdate.h"
#include "UpdateThread.h"
#include "UISkinManager.h"
#include "GameUpdateDlg.h"
#include ".\GameUpdatedlg.h"
#include "Generate.h"
#include <tlhelp32.h>
#include "CDownloadManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define  IDB_BTN_DOWN	100					// 下载整包按钮

class CGameUpdateDlg * g_pCGameUpdateDlg = NULL;

void FunDownLoadFinish(char* p, bool isfinish, void* pObj)
{
	CGameUpdateDlg* pDownSuc = (CGameUpdateDlg*)pObj;
	if (NULL != pDownSuc)
	{
		pDownSuc->DownResult(p, isfinish);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

	// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
	// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGameUpdateDlg dialog

CGameUpdateDlg::CGameUpdateDlg(CWnd* pParent /*=NULL*/)
: CDialog(CGameUpdateDlg::IDD, pParent)
{
	m_btNums = 0;
	g_pCGameUpdateDlg = this;
	//{{AFX_DATA_INIT(CGameUpdateDlg)
	m_TotalFileInfo.iFileCount = 0;
	m_TotalFileInfo.iFileSize = 0;
	m_FinishedFileInfo.iFileCount = 0;
	m_FinishedFileInfo.iFileSize = 0;
	m_bSilenceMode = FALSE;
	m_bUserBreak = FALSE;
	m_bGenerateIniFile = FALSE;
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_HBrush = (HBRUSH)CreateSolidBrush(NULL_BRUSH);
	m_strContent.Empty();
	m_pInternet = NULL;
	m_lDownBaiBi = 0;
	m_bDownError = false;
	m_lTmpFileCount = 0;
	m_bIsUpdateMySelf = FALSE;
	m_bIsCopyfile = FALSE;
	m_eResumeMode = IE_MODE;
	m_uThreadCount = 0;
	m_setWantUpdateFile.clear();
	m_setDowndloadFile.clear();
	m_setPriorityDowndload.clear();

	for (BYTE i=0; i<THREAD_MAX_SIZE; i++)
	{
		m_hUpdateThread[i] = NULL;
		m_hStopEvent[i] = NULL;
		m_hDownloadEvent[i][0] = NULL;
		m_hDownloadEvent[i][0] = NULL;
		m_hCompleteEvent[i] = NULL;
	}
}

CGameUpdateDlg::~CGameUpdateDlg()
{
	LOG(0, 0, "****************** 结束 %s 工程升级 ******************", PROJECT_NAME);
}

void CGameUpdateDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGameUpdateDlg)
	DDX_Control(pDX, IDCANCEL, m_btnCancel);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CGameUpdateDlg, CDialog)
	//{{AFX_MSG_MAP(CGameUpdateDlg)
	RDIALOG_UI_MESSAGE_MAP
	ON_WM_SYSCOMMAND()
	ON_WM_QUERYDRAGICON()
	ON_MESSAGE(WM_USER_MSG, OnUserMessage)
	//}}AFX_MSG_MAP
	ON_WM_TIMER()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_BN_CLICKED(IDB_BTN_DOWN, OnBtnDown)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGameUpdateDlg message handlers

BOOL CGameUpdateDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	TCHAR szNamePath[MAX_PATH]="", szResultfile[MAX_PATH]="";
	_snprintf(szNamePath, sizeof(szNamePath), "%s\\%s\\%s", GetAppDirectory(FALSE), CONFIG_PATH, UPDATE_CONFIG_FILENAME);
	GetPrivateProfileString(_T("UpdateAllFile"), _T("WriteLog"), _T("5"), szResultfile, MAX_PATH, szNamePath);
	g_IsWriteLog = (0!=atoi(szResultfile)) ? TRUE : FALSE;
		// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	CString	sFileName("");
	CString sFilePath("");
	GetModuleFileName(NULL, sFileName.GetBufferSetLength(128), 128);
	sFilePath=sFileName.Left(sFileName.ReverseFind('\\') + 1) + TEXT("config\\config.ini");
	GetPrivateProfileString(TEXT("ServersAddr"), TEXT("HallDowndloadURL"), TEXT(""), m_chURL, 128, sFilePath);
	
	// 域名修正
	string strHallDowndloadURL(m_chURL);
	if (0 != strHallDowndloadURL.find(TEXT("http://")))
		strHallDowndloadURL.insert(0, TEXT("http://"));
	_snprintf(m_chURL, sizeof(m_chURL), TEXT("%s"), strHallDowndloadURL.c_str());

	// TODO: Add extra initialization here
	m_btNums = 0;
	SetWindowText(STRING(STR_AUTO_UPDATE, "自动升级"));
	//m_btnCancel.SetWindowText(STRING(STR_BUTTON_CANCEL_UPGRADE, "取   消"));
	OnUpdateControlContent("正在准备升级中...");
	LOG(0, 0, "");
	LOG(0, 0, "");
	LOG(0, 0, "****************** 开始 %s 工程升级 ******************", PROJECT_NAME);
	SetWindowPos(&wndTopMost, 0, 0, 536, 150, SWP_NOMOVE);
	m_bitmapBG.LoadBitmap(IDB_UPDATEGAME_BMP);
	m_bitmapErrorBG.LoadBitmap(IDB_BITMAP_ERROR_BG);
	m_bTransfer.LoadBitmap(IDB_BITMAP_TRANSFER);
	m_ImageProgressBG.LoadBitmap(IDB_BITMAP_PROGRESS_BG);
	m_ImageProgressFrontGree.LoadBitmap(IDB_BITMAP_PROGRESS_GREEN);
	m_imageProgressFrontRed.LoadBitmap(IDB_BITMAP_PROGRESS_RED);
	m_ImageDownLoadNum.LoadBitmap(IDB_BITMAP_NUM);
	m_bitmapPercentSign.LoadBitmap(IDB_BITMAP_PERCENT_SIGN);
	m_bDownErrorWaring.LoadBitmap(IDB_BITMAP_WARNING);
	//m_btnClose.ShowWindow(SW_HIDE);
	m_btnCancel.SetWindowPos(NULL, 500, 10, 120/5, 19, SWP_NOZORDER|SWP_SHOWWINDOW);
	m_btnCancel.SetButtonImage(IDB_BUTTON_CLOSE, AfxGetInstanceHandle(), false);
	//m_btnCancel.SetTextColorEx(RGB(255,255,255));
	m_rcDownDrawRect.SetRect(110, 20, 110+410, 20+70);

	m_btnDown.Create(NULL, WS_VISIBLE|WS_CHILD, CRect(350,95,0,0), this, IDB_BTN_DOWN);
	m_btnDown.SetButtonImage(IDB_BTN_HERE, AfxGetInstanceHandle(), false);

	if (m_bSilenceMode)
	{
		ShowWindow(SW_HIDE);
		SetTimer(WM_USER+1, 2000, NULL);
		return TRUE;
	}

	SetTimer(WM_USER+1, 500, NULL);
	SetTimer(WM_USER+2, 100, NULL);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CGameUpdateDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CGameUpdateDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		PostMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CPaintDC dc(this);
		CDC BufferDC, ImgDC;
		BITMAP bitmapInfo;
		CRect  rect;
		GetClientRect(&rect);
		dc.SetBkMode(TRANSPARENT);

		BufferDC.CreateCompatibleDC(&dc);
		HBITMAP BufferBitmap = CreateCompatibleBitmap(dc.GetSafeHdc(), rect.Width(), rect.Height());
		BufferDC.SelectObject(BufferBitmap);
		ImgDC.CreateCompatibleDC(&dc);
		ImgDC.SelectObject(m_bDownError?m_bitmapErrorBG:m_bitmapBG);

		// 背景
		BitBlt(BufferDC.GetSafeHdc(), 0, 0, rect.Width(), rect.Height(), ImgDC.GetSafeHdc(), 0, 0, SRCCOPY);
		//TransparentBlt(BufferDC.GetSafeHdc(), 0, 0, rect.Width(), rect.Height(), ImgDC.GetSafeHdc(), 0, 0, 536, 150, RGB(255,255,0));
		//BufferDC.Draw3dRect(0, 0, 497, 56 , RGB(21, 80, 135), RGB(0, 0, 0));			

		//// 下载的文件
		//CFont UserFont;
		//UserFont.CreateFont(12,0,0, 0, FW_REGULAR, FALSE, FALSE, FALSE, 
		//	DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		//	PROOF_QUALITY,DEFAULT_QUALITY,TEXT("宋体"));

		//BufferDC.SetBkMode(TRANSPARENT);
		//BufferDC.SetTextColor(RGB(255,255,255));
		//CFont * pOldFont = BufferDC.SelectObject(&UserFont);
		//BufferDC.DrawText(m_strContent, &CRect(70, 15, 383, 30), 
		//	DT_VCENTER|DT_LEFT|DT_SINGLELINE|DT_NOPREFIX|DT_NOCLIP|DT_WORD_ELLIPSIS);
		//BufferDC.SelectObject(pOldFont);

		//绘制数字
		if (0 < m_lDownBaiBi  &&  !m_bDownError)
		{
			CPoint ptNum;
			ptNum.x = 265;
			ptNum.y = 25;
			int iNums[3] = {-1};
			int iCount = 0;
			int iValue = m_lDownBaiBi; 
			if (0==iValue)
			{
				iNums[0] = 0;
				iCount = 1;
			}
			else
			{
				while(iValue>0&&iCount<3)
				{
					iNums[iCount++] = iValue%10;
					iValue = iValue/10;
				}
			}
			m_ImageDownLoadNum.GetBitmap(&bitmapInfo);
			for (int i=iCount-1; i>=0; --i)
			{
				if (iNums[i]>=0)
				{

					ImgDC.SelectObject(m_ImageDownLoadNum);
					BitBlt(BufferDC.GetSafeHdc(), ptNum.x, ptNum.y+2, 20, 23, ImgDC.GetSafeHdc(), iNums[i]*20, 0, SRCCOPY);
					ptNum.x += 20;
				}
			}

			//百分号
			m_bitmapPercentSign.GetBitmap(&bitmapInfo);
			ImgDC.SelectObject(m_bitmapPercentSign);
			BitBlt(BufferDC.GetSafeHdc(), ptNum.x, ptNum.y+2, bitmapInfo.bmWidth, bitmapInfo.bmHeight, ImgDC.GetSafeHdc(), 0, 0, SRCCOPY);
		}

		//绘制进度条
		m_ImageProgressBG.GetBitmap(&bitmapInfo);
		ImgDC.SelectObject(m_ImageProgressBG);
		BitBlt(BufferDC.GetSafeHdc(), 120, 65, bitmapInfo.bmWidth, bitmapInfo.bmHeight, ImgDC.GetSafeHdc(), 0, 0, SRCCOPY);
		if (m_bDownError)
		{
			m_imageProgressFrontRed.GetBitmap(&bitmapInfo);
			ImgDC.SelectObject(m_imageProgressFrontRed);
		}
		else
		{
			m_ImageProgressFrontGree.GetBitmap(&bitmapInfo);
			ImgDC.SelectObject(m_ImageProgressFrontGree);
		}
		BitBlt(BufferDC.GetSafeHdc(), 120, 65, m_lDownBaiBi*3.94, bitmapInfo.bmHeight, ImgDC.GetSafeHdc(), 0, 0, SRCCOPY);


		// 转盘-警告
		if (m_bDownError)
		{
			ImgDC.SelectObject(m_bDownErrorWaring);
			m_bDownErrorWaring.GetBitmap(&bitmapInfo);
			BitBlt(BufferDC.GetSafeHdc(), 16, 30, bitmapInfo.bmWidth/2, bitmapInfo.bmHeight, ImgDC.GetSafeHdc(), 
				bitmapInfo.bmWidth/2*(m_btNums%2), 0, SRCCOPY);
		}
		else
		{
			ImgDC.SelectObject(m_bTransfer);
			TransparentBlt(BufferDC.GetSafeHdc(), 16, 30, 1710/19, 86, ImgDC, 1710/19*m_btNums, 0, 1710/19, 86, RGB(255,255,0));
		}

		dc.BitBlt(0, 0, rect.Width(), rect.Height(), &BufferDC, 0, 0, SRCCOPY);

		BufferDC.DeleteDC();
		ImgDC.DeleteDC();
		DeleteObject(BufferBitmap);

		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CGameUpdateDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CGameUpdateDlg::OnCancel() 
{
	KillTimer(WM_USER+1);
	KillTimer(WM_USER+2);	
	m_bUserBreak = TRUE;

	if (NULL != m_pInternet)
	{
		delete m_pInternet;
		m_pInternet = NULL;
	}

	CDialog::OnCancel();
}

// 设置升级配置文件名
BOOL CGameUpdateDlg::SetConfigFile(CString &sFilename)
{
	CFileFind FileFinder;

	if (FileFinder.FindFile(sFilename))
	{
		FileFinder.FindNextFile();
		if (!FileFinder.IsDirectory())
		{
			m_sConfigFilename = sFilename;
			FileFinder.Close();
			return TRUE;
		}
		FileFinder.Close();
	}

	return FALSE;
}

// 处理用户消息
LRESULT CGameUpdateDlg::OnUserMessage(WPARAM wParam, LPARAM lParam)
{
	switch (wParam)
	{
	case NOTIFY_DOWNLOAD_PROGRESS:		// 通知下载单个文件进度
		{
			DOWNLOAD_PROGRESS_STRU* pInfo = (DOWNLOAD_PROGRESS_STRU*)lParam;

			// 非统计类下载（如配置文件下载，进度条不处理）
			if (0 > pInfo->uIndex)
			{
				break;
			}

			// 已下载字节数
			ULONGLONG ullTmpSize = 0l;
			m_mDowndloadCount.erase(pInfo->uIndex);
			m_mDowndloadCount.insert(make_pair(pInfo->uIndex, pInfo->iCurrentFileFinishedSize));

			map<long, ULONGLONG>::const_iterator cMap=m_mDowndloadCount.begin();
			for (; cMap != m_mDowndloadCount.end(); ++cMap)
			{
				ullTmpSize += cMap->second;
			}
			m_lDownBaiBi = 1.0*((DOUBLE)(ullTmpSize + m_FinishedFileInfo.iFileSize) / (DOUBLE)m_TotalFileInfo.iFileSize)*100;

			CString str;
			str.Format("num: 索引%ld, 已下载 %u,  总大小 %u", pInfo->uIndex, pInfo->iCurrentFileFinishedSize, pInfo->iCurrentFileSize);
			OutputDebugString(str);
			str.Format("num: 完成数组 %I64u,  所有文件数组 %I64u,  总大小 %I64u", ullTmpSize, m_FinishedFileInfo.iFileSize, m_TotalFileInfo.iFileSize);
			OutputDebugString(str);
		}
		break;
		//case NOTIFY_DOWNLOADING_FILENAME:	// 通知正在下载的文件名
		//	{
		//		//m_strContent.Format("下载文件 [%d/%d]: %s", m_FinishedFileInfo.iFileCount+m_pUpdateThread->m_setDowndloadFile.size(), 
		//		//	m_TotalFileInfo.iFileCount, (0 != lParam) ? (char*)lParam : _T("正在下载文件 :"));
		//		//InvalidateRect(&m_rcDownDrawRect);
		//	}
		//	break;
		//case NOTIFY_DOWNLOAD_FILE_FAIL:		// 通知下载文件失败
		//	{
		//		m_strContent.Format("Error: 下载失败 %s ！", (char*)lParam);
		//		//InvalidateRect(&m_rcDownDrawRect);
		//	}
		//	break;
		//case NOTIFY_VERIFY_FILE_FAIL:		// 通知校验文件失败
		//	{
		//		m_strContent.Format("Error: 校验失败 %s ！", (char*)lParam);
		//		//InvalidateRect(&m_rcDownDrawRect);
		//	}
		//	break;
		//case NOTIFY_UPDATE_FILE_FAIL:		// 通知更新文件失败
		//	{
		//		m_strContent.Format("Error: 更新失败 %s ！", (char*)lParam);
		//		//InvalidateRect(&m_rcDownDrawRect);
		//	}
		//	break;
	case NOTIFY_FINISH_UPDATE:			// 通知升级完毕消息
		{
			OnNotifyUpdateFinish((BOOL)lParam);
		}
		break;
		//case NOTIFY_MESSAGE:				// 通知界面提示消息
		//	{
		//		OnUpdateControlContent((LPCTSTR)lParam);
		//	}
		//	break;
		//case NOTIFY_GENERATE_MESSAGE:		// 通知生成提示消息
		//	{
		//		LPCTSTR ptchData = (LPCTSTR)lParam;
		//		//OnUpdateGenerateInIFile(ptchData);
		//	}
		//	break;
		//case NOTIFY_GENERATE_FILE_NAME:	// 通知生成文件列表
		//	{
		//		LPCTSTR ptchData = (LPCTSTR)lParam;
		//	}
		//break;
		//case NOTIFY_LEACH_FILE_NAME:		// 通知过滤的文件名
		//	{
		//		;OnSetLeachFileName((LPCTSTR)lParam);
		//	}
		//	break;
	default:
		break;
	}

	return TRUE;
}

BOOL CGameUpdateDlg::TransferExternalProgram(BOOL bMyself/*=FALSE*/, string strUpdateName/*=""*/)
{
	//构造命令行
	CString strCommonLine;

	if (TRUE == bMyself)
	{
		strCommonLine.Format(TEXT("%s /modulefilename:%s /account:%s /pw:%s /updatename:%s"), 
			HELP_UPDATE_NAME, m_strExternalProgramFileName.c_str(), m_strAccount.c_str(), m_strPW.c_str(), 
			strUpdateName.c_str());
	}
	else
	{
		strCommonLine.Format(TEXT("%s /account:%s /pw:%s"),m_strExternalProgramFileName.c_str(), m_strAccount.c_str(), m_strPW.c_str());
	}

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
		LOG(0, 0, "创建进程：CGameUpdateDlg::TransferExternalProgram 命令行：{%s} \"%s\".", strCommonLine, (TRUE==bMyself?"创建辅助进程":"创建调用者进程"));
		bSuccess = CreateProcess(NULL,strCommonLine.GetBuffer(),NULL,NULL,FALSE,CREATE_DEFAULT_ERROR_MODE,NULL,NULL,&StartInfo,&ProcessInfo);
	}
	catch (...) 
	{ 
		DWORD dwError = GetLastError();
		ASSERT(0); 
	}
	strCommonLine.ReleaseBuffer();

	if (FALSE == bSuccess)
	{
		return FALSE;
	}

	return TRUE;
}

void CGameUpdateDlg::OnUpdateControlContent(LPCTSTR lpContent)
{
	if (NULL != lpContent)
	{
		m_strContent.Format("%s", lpContent);
		//InvalidateRect(&m_rcDownDrawRect);
	}
}

// 升级程序执行完毕
void CGameUpdateDlg::OnNotifyUpdateFinish(BOOL bSuccess)
{
	m_bDownError = (TRUE==bSuccess) ? false : true;

	for (BYTE i=0; i<m_uThreadCount; i++)
	{
		SetEvent(m_hStopEvent[i]);
	}
	WaitForMultipleObjects(m_uThreadCount, m_hUpdateThread, TRUE, 5000);

	if (m_bSilenceMode || TRUE==bSuccess)
	{
		__super::OnOK();
	}
}

void CGameUpdateDlg::CheckUpdateFile()
{
	//ThreadRunStartOperator startOperator;
	//startOperator = ONLY_RUN;
	// 获取配置文件的HASH，来检验文件是否合法
	CString szPath(GetAppDirectory());
	TCHAR szHaseList[MAX_PATH]=TEXT(""); //本地配置文件路径
	TCHAR tchResult[MAX_PATH]=TEXT("");
	_snprintf(szHaseList, sizeof(szHaseList), "%s%s\\%s", szPath, CONFIG_PATH, UPDATE_CONFIG_FILENAME);
	GetPrivateProfileString(_T("UpdateAllFile"), _T("GameHaseList"), _T(""), tchResult, MAX_PATH, szHaseList);

	// 点击开始升级按钮，执行升级
	if (!SetConfigFile(GetAppDirectory() + "config\\" + UPDATE_CONFIG_FILENAME)  ||  (0==strcmp("", tchResult)))
	{
		int iResult = RMessageBox(_T("更新配置文件损坏或已被删除，更新机制将重新生成配置文件，与服务器校验！"), MB_YESNO);
		if (IDNO == iResult)
		{
			ShowWindow(SW_HIDE);
			PostMessage(WM_CLOSE);
			return;
		}
		else
		{
			m_strContent.Format("正在重新生成游戏更新配置文件");
			//InvalidateRect(&m_rcDownDrawRect);

			CGenerate GenerateDlg;
			GenerateDlg.m_bDowndloadServerSetFile = TRUE;
			GenerateDlg.SetQuietMode(TRUE);
			GenerateDlg.DoModal();
			
			if (FALSE == GenerateDlg.m_bComplete)
			{
				// 取消生成，更新也被取消
				PostMessage(WM_CLOSE);
				return;
			}
		}
	}

	CheckWantUpdateFile();
}

void CGameUpdateDlg::CheckWantUpdateFile(void)
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
		CopyFile(szServerPath, szNamePath, FALSE);
		CDirOperate::DeleteAllDirectory(cstrDirectory);

		RMessageBox("您已经是最新版本！");
		g_pCGameUpdateDlg->ShowWindow(SW_HIDE);
		g_pCGameUpdateDlg->TransferExternalProgram();
		g_pCGameUpdateDlg->PostMessage(WM_CLOSE);
		return;
	}

	PostMessage(WM_USER_MSG, (WPARAM)NOTIFY_MESSAGE, (LPARAM)"开始升级...");

	StartUpdate();
}

void CGameUpdateDlg::CheckSingleFileUpdate(void)
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
	PostMessage(WM_USER_MSG, (WPARAM)NOTIFY_MESSAGE, (LPARAM)"正在读取本地文件列表...");
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
			m_FinishedFileInfo.iFileSize += filehashtable.size;
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
	PostMessage(WM_USER_MSG, (WPARAM)NOTIFY_MESSAGE, (LPARAM)"正在获取服务器文件列表...");
	TCHAR szServerPath[MAX_PATH]=TEXT(""); //服务器配置文件保存路径
	_snprintf(szNamePath, sizeof(szNamePath), "%s%s\\%s", szPath, CONFIG_PATH, UPDATE_CONFIG_FILENAME);
	_snprintf(szServerPath, sizeof(szServerPath), "%s%s",szPath,DOWNDLOAD_TMP_PATH);
	CDirOperate::CreateAllDirectory(szServerPath);//创建配置文件保存目录
	_snprintf(szServerPath, sizeof(szServerPath), "%s%s\\%s", szPath, DOWNDLOAD_TMP_PATH, UPDATE_CONFIG_TMP_FILENAME);
	GetPrivateProfileString(_T("UpdateAllFile"), _T("URL"), _T("http://192.168.1.5/update/hallfile/"), szNamePath, MAX_PATH, szNamePath);
	_snprintf(szNamePath, sizeof(szNamePath), "%s%s/%s", szNamePath, CONFIG_PATH, UPDATE_CONFIG_FILENAME);

	if (FALSE == DowndloadFile(&ThreadParameter(this, 255), szNamePath, szServerPath, -1, IE_MODE))
	{
		DWORD dwError = GetLastError();
		LOG(0, 0, "Error: 下载服务器配置文件失败 错误号:%ld 文件路径: %s", dwError, szNamePath);
		return;
	}

	ZeroMemory(szResultfile, sizeof(szResultfile));
	GetPrivateProfileString(TEXT("UpdateAllFile"), _T("URL"), _T(""), szResultfile, MAX_PATH, szServerPath);
	if (0 >= lstrlen(szResultfile))
	{
		LOG(0, 0, "Error: 校验下载服务器配置文件失败，文件路径: %s--%s", szNamePath, szResultfile);
		return;
	}

	// 获取优先更新文件列表
	int iFlag = 1;
	do
	{
		_snprintf(szKeyword, sizeof(szKeyword), _T("file%d"), iFlag++);
		GetPrivateProfileString(_T("priority"), szKeyword, "", szResultfile, MAX_PATH, szServerPath);
		
		if (0 >= lstrlen(szResultfile))
		{
			break;
		}

		m_setPriorityDowndload.push_back(string(szResultfile));
	}while (1);

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
	PostMessage(WM_USER_MSG, (WPARAM)NOTIFY_MESSAGE, (LPARAM)"正在对比服务器与客户端文件列表...");
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

void CGameUpdateDlg::CheckDirectoryUpdate(void)
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

	PostMessage(WM_USER_MSG, (WPARAM)NOTIFY_MESSAGE, (LPARAM)"正在获取服务器目录列表...");
	TCHAR szServerPath[MAX_PATH]=TEXT(""); //服务器配置文件保存路径
	_snprintf(szNamePath, sizeof(szNamePath), "%s%s\\%s", szPath, CONFIG_PATH, UPDATE_CONFIG_FILENAME);
	_snprintf(szServerPath, sizeof(szServerPath), "%s%s\\%s", szPath, DOWNDLOAD_TMP_PATH, UPDATE_CONFIG_TMP_FILENAME);

	//读取本地更新目录列表
	int iIndex=0;
	PostMessage(WM_USER_MSG, (WPARAM)NOTIFY_MESSAGE, (LPARAM)"正在读取本地更新目录列表...");
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
	PostMessage(WM_USER_MSG, (WPARAM)NOTIFY_MESSAGE, (LPARAM)"正在对比服务器与客户端目录列表...");
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
	PostMessage(WM_USER_MSG, (WPARAM)NOTIFY_MESSAGE, (LPARAM)"正在对比服务器与客户端目录下的文件列表...");
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

void CGameUpdateDlg::StartUpdate()
{
	if (m_setWantUpdateFile.empty())
	{
		LOG(0, 0, "Error: 需要更新的文件列表为空 程序相应变量:m_setWantUpdateFile,函数名:CUpdateThread::DoUpdateEx()");
		return;
	}

	// 下载文件
	char cFileCount[MAX_PATH]={0};
	CString szPath(GetAppDirectory());
	TCHAR chBuffer[MAX_PATH]={0}, chKeyName[MAX_PATH]={0}, chFileSzie[MAX_PATH]={0};
	m_bIsUpdateMySelf = FALSE;

	for (map<string, FileHashTable>::iterator beg=m_setWantUpdateFile.begin(); beg!=m_setWantUpdateFile.end(); beg++)
	{
		if (m_bUserBreak)
			return;

		//检查该文件是否已经下载， 已经下载过滤掉
		map<string, FileHashTable>::iterator mLocal = m_setDowndloadFile.begin(); 
		for (; mLocal != m_setDowndloadFile.end(); mLocal++)
		{
			if (0 == beg->second.path.compare(mLocal->second.path))
			{
				beg->second.bDowndloadComplete = TRUE;
				beg->second.tmpFileName = mLocal->second.tmpFileName;
				break;
			}
		}

		// 更新程序自己需要更新，且还没有更新过（考虑更新了一部分终止了。）
		// 这次运行只更新自己，其他先假如已经更新
		if (TRUE == IsHavePriorityUpdate(beg->second.filename)  &&  FALSE==beg->second.bDowndloadComplete)
		{
			m_bIsUpdateMySelf = TRUE;

			// 创建相应目录
			CString cstrDirectory(GetAppDirectory());
			cstrDirectory.Append(DOWNDLOAD_TMP_PATH);
			//不能续传或者续传文件损坏，清空该文件夹(因为是更新自己时的情况)
			if ( m_setDowndloadFile.empty() )
			{
				CDirOperate::DeleteAllDirectory(cstrDirectory);
				CDirOperate::CreateAllDirectory(cstrDirectory);
			}

			// 设置所有文件已经下载
			map<string, FileHashTable>::iterator bMySef=m_setWantUpdateFile.begin();
			for (; bMySef!=m_setWantUpdateFile.end(); bMySef++)
			{
				if (FALSE == IsHavePriorityUpdate(bMySef->second.filename))
				{
					bMySef->second.bDowndloadComplete = TRUE;
				}
			}

			// 更新程序设置 没有 下载
			beg->second.bDowndloadComplete = FALSE;
		}
	}

	// 获取线程下载数目
	TCHAR szNamePath[MAX_PATH]="", szResultfile[MAX_PATH]="";
	_snprintf(szNamePath, sizeof(szNamePath), "%s%s\\%s", szPath, CONFIG_PATH, UPDATE_CONFIG_FILENAME);
	GetPrivateProfileString(_T("UpdateAllFile"), _T("ThreadCount"), _T("1"), szResultfile, MAX_PATH, szNamePath);
	m_uThreadCount = atol(szResultfile);
	if (0>=m_uThreadCount  ||  10<m_uThreadCount)
	{
		m_uThreadCount = 1;
	}

	// 获取下载模式
	UINT uDownloadMode = GetPrivateProfileInt(_T("UpdateAllFile"), _T("DownloadMode"), 0, szNamePath);
	switch (uDownloadMode)
	{
	case 0: m_eResumeMode = IE_MODE; break;
	case 1: m_eResumeMode = BreakPoints_MODE;break;
	case 2: m_eResumeMode = UM_DLL_MODE;break;
	default: m_eResumeMode = IE_MODE;break;
	}

	m_lTmpFileCount = m_setDowndloadFile.size();
	unsigned int unThreadID[5] = {0};


	for (long lIndex=0; lIndex<m_uThreadCount; lIndex++)
	{
		m_structThreadParameter[lIndex].wndPointer = this;
		m_structThreadParameter[lIndex].byEventIndex= lIndex;
		//bool bResult = m_structThreadParameter[lIndex].InitDowndloadStatus((IDownloadEvent*)this);

		if (BreakPoints_MODE == m_eResumeMode)
		{
			//m_stDownload[lIndex].RegistEventModule((IDownloadEvent*)this);
			//bool bResult = m_stDownload[lIndex].Start();
			LOG(0, 0, "断点续传模式，组件注册成功！");
		}
		
		m_hDownloadEvent[lIndex][0] = CreateEvent(NULL, FALSE, FALSE, NULL);
		m_hDownloadEvent[lIndex][1] = CreateEvent(NULL, FALSE, FALSE, NULL);
		m_hStopEvent[lIndex] = m_hDownloadEvent[lIndex][0];
		m_hCompleteEvent[lIndex] = m_hDownloadEvent[lIndex][1];

		m_hUpdateThread[lIndex] = (HANDLE)_beginthreadex(NULL, 0, &ThreadWorkFun, 
			&m_structThreadParameter[lIndex], CREATE_SUSPENDED, &unThreadID[lIndex]);
	}

	for (long lIndex=0; lIndex<m_uThreadCount; lIndex++)
	{
		ResumeThread(m_hUpdateThread[lIndex]);
		Sleep(1);
	}
	return;
}

void CGameUpdateDlg::StartCopyFile()
{
	//检查大厅是否已经关闭
	while (true)
	{
		HWND hMain = ::FindWindow(m_strExternalProgramFileName.c_str(), NULL);
		if (hMain != NULL)
		{
			int iResult = RMessageBox(_T("请先关闭游戏大厅，再重试！"), 0x00000007L);
			if (IDCANCEL == iResult)
			{
				g_pCGameUpdateDlg->PostMessage(WM_CLOSE);
				return;

			}
			else if (IDYES == iResult)
			{
				::PostMessage(hMain, WM_CLOSE, 0, 0);
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

	//// 自动调用分包文件
	////auto start
	//CThreadGuard GuardWantUpdateArray(&criticalWantUpdateFile);
	//map<string, FileHashTable>::iterator beg=m_setWantUpdateFile.begin();
	//for (; beg!=m_setWantUpdateFile.end(); ++beg)
	//{
	//	string strPart = beg->second.filename.substr(beg->second.filename.rfind('.'), sizeof(".exe"));
	//	if ((string::npos != beg->second.filename.find(TEXT(".sfx"))  ||  string::npos != beg->second.filename.find(TEXT(".part")))  && 
	//		0== strPart.compare(".exe"))
	//	{
	//		BOOL bSuccess = FALSE;
	//		STARTUPINFO StartInfo;
	//		memset(&StartInfo,0,sizeof(StartInfo));
	//		StartInfo.cb=sizeof(StartInfo);
	//		StartInfo.wShowWindow=SW_SHOWMAXIMIZED;
	//		PROCESS_INFORMATION ProcessInfo;
	//		memset(&ProcessInfo,0,sizeof(ProcessInfo));
	//		CString strCommonLine("GamePlaza.sfx.part1.exe");
	//		bSuccess = CreateProcess(NULL,strCommonLine.GetBuffer(),NULL,NULL,FALSE,CREATE_DEFAULT_ERROR_MODE,
	//			NULL,NULL,&StartInfo,&ProcessInfo);
	//		strCommonLine.ReleaseBuffer();
	//		DWORD dwError = GetLastError();
	//		LOG(0, 0, "解压文件：\"%s\" bSuccess %d, dwError %ld", beg->second.filename.c_str(), bSuccess, dwError);
	//	}
	//}
	//GuardWantUpdateArray.leave();
	//auto end

	
	// 调用辅助进程更新自己，其他文件先不需要更新，因为还没下载，下次运行时才下载
	// 这样能保证每次更新除本程序外，其他程序都是用最新的更新机制更新
	if (TRUE==m_bIsUpdateMySelf)
	{
		// 辅助更新进程优先更新
		map<string, FileHashTable>::iterator vHelp=m_setWantUpdateFile.begin(); 
		for (;vHelp != m_setWantUpdateFile.end(); vHelp++)
		{
			if (0 == vHelp->second.filename.compare(HELP_UPDATE_NAME))
			{
				CheckProcess(HELP_UPDATE_NAME);
				BOOL bResult = OnSuccessCopyFile(vHelp->second.filename, vHelp->second.tmpFileName, vHelp->second.path);
				if (FALSE == bResult)
				{
					return;
				}
				break;
			}
		}

		// 调用辅助进程更新自己
		ShowWindow(SW_HIDE);
		TransferExternalProgram(TRUE, PROJECT_NAME);
		PostMessage(WM_USER_MSG, (WPARAM)NOTIFY_FINISH_UPDATE, (LPARAM)1);
		return;
	}
    
	// 循环更新所有文件
	for (map<string, FileHashTable>::iterator beg=m_setWantUpdateFile.begin(); beg!=m_setWantUpdateFile.end(); beg++)
	{
		if (m_bUserBreak)
			return;

		if (FALSE==m_bIsUpdateMySelf  &&  TRUE == IsHavePriorityUpdate(beg->second.filename))
		{
			// 这种情况是这次不需要更新自己，因此直接跳过
			// 如果需要更新自己（更新机制）时，会执行两次更新，第一次只更新自己，第二次更新其他文件。
			continue;
		}

		BOOL bResult = OnSuccessCopyFile(beg->second.filename, beg->second.tmpFileName, beg->second.path);
		if (FALSE == bResult)
		{
			return;
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
		CopyFile(strAppDirectory+"\\"+UPDATE_CONFIG_TMP_FILENAME, strIniFilePath, FALSE);
	}

	// 删除下载中使用的缓冲目录
	CDirOperate::DeleteAllDirectory(strAppDirectory);

	//更新完成，重启游戏大厅
	ShowWindow(SW_HIDE);
	TransferExternalProgram();
	PostMessage(WM_USER_MSG, (WPARAM)NOTIFY_FINISH_UPDATE, (LPARAM)1);
}

BOOL CGameUpdateDlg::CopyFileUpdate(const string &strFile, const string &strTmpFile, const string &strPath) const
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
	LOG(0, 0, "删除文件：\"%s, 新文件：%s\".", strAppDirectory, strExistingFile);
	return CopyFile(strExistingFile, strAppDirectory, FALSE);
}

BOOL CGameUpdateDlg::OnSuccessCopyFile(const string &strFile, const string &strTmpFile, const string &strPath)
{
	if (!CopyFileUpdate(strFile, strTmpFile, strPath))
	{
		CString strName;
		TCHAR szPath[MAX_PATH]=TEXT("");
		GetModuleFileName(AfxGetInstanceHandle(), szPath, sizeof(szPath));
		strName = GetFilename(szPath);

		// 被占用的文件是更新程序时跳过
		if (0 != strName.CompareNoCase(strFile.c_str()))
		{
			// 文件更新不成功，升级失败
			if (!m_bSilenceMode)
			{
				TCHAR tchBuffer[MAX_PATH]=TEXT("");
				_snprintf(tchBuffer, sizeof(tchBuffer), "%s", strPath.c_str());
				PostMessage(WM_USER_MSG, (WPARAM)NOTIFY_UPDATE_FILE_FAIL, (LPARAM)tchBuffer);
			}

			// 可以让玩家一直点重试，直到不愿意再尝试为止
			while (1)
			{
				CString strBuffer;
				strBuffer.Format(_T("[%s]更新失败 文件正在被另一个人或程序使用，请关闭相应程序，重新试一次\r\n“当无法结束时建议重启电脑”"), strFile.c_str());
				int iReturn = RMessageBox(strBuffer, MB_RETRYCANCEL);
				if (IDRETRY == iReturn)
				{
					if (CopyFileUpdate(strFile, strTmpFile, strPath))
						break;
				}
				else
				{
					PostMessage(WM_USER_MSG, (WPARAM)NOTIFY_FINISH_UPDATE, (LPARAM)0);
					return FALSE;
				}
			}
		}
	}

	LOG(0, 0, "复制文件：\"%s%s\".", strPath.c_str(), strTmpFile.c_str());
	return TRUE;
}

void CGameUpdateDlg::InitDowndloadArray()
{
	DOWNLOAD_INFO_STRU tmpTotalFileInfo;
	DOWNLOAD_INFO_STRU tmpDowndloadInfo;

	Sleep(1);
	OutputDebugString("GameUpdateDlg.cpp,进入关键代码段, InitDowndloadArray,GuardWantUpdate(&criticalWantUpdateFile);");//Test
	CThreadGuard GuardWantUpdate(&criticalWantUpdateFile);
	// 总下载文件数目及大小
	for (map<string, FileHashTable>::iterator beg=m_setWantUpdateFile.begin(); 
		beg!=m_setWantUpdateFile.end(); beg++)
	{
		tmpTotalFileInfo.iFileCount++;
		tmpTotalFileInfo.iFileSize += beg->second.size;
	}
	// 完成的文件数目及大小（包含上次下载到一半的文件）
	for (map<string, FileHashTable>::iterator beg=m_setDowndloadFile.begin(); 
		beg!=m_setDowndloadFile.end(); beg++)
	{
		tmpDowndloadInfo.iFileCount++;
		tmpDowndloadInfo.iFileSize += beg->second.size;
	}
	GuardWantUpdate.leave();
	OutputDebugString("GameUpdateDlg.cpp,离开关键代码段, InitDowndloadArray,GuardWantUpdate.leave();");//Test

	Sleep(1);
	OutputDebugString("GameUpdateDlg.cpp,进入关键代码段, InitDowndloadArray,GuardDowndloadInfo(&criticalDowndloadInfo);");//Test
	CThreadGuard GuardDowndloadInfo(&criticalDowndloadInfo);
	if (0 >= m_TotalFileInfo.iFileCount)
	{		
		m_TotalFileInfo = tmpTotalFileInfo;
		m_FinishedFileInfo.iFileCount = tmpDowndloadInfo.iFileCount;
	}
	OutputDebugString("GameUpdateDlg.cpp,离开关键代码段, InitDowndloadArray,GuardDowndloadInfo.leave();");//Test
}

unsigned __stdcall CGameUpdateDlg::ThreadWorkFun(LPVOID lParam)
{
	ThreadParameter* ParameterContent = (ThreadParameter*)lParam;
	CGameUpdateDlg * pWnd = (CGameUpdateDlg *)ParameterContent->wndPointer;
	if (NULL == pWnd  ||  NULL==pWnd->GetSafeHwnd())
	{
		return 0;
	}

	LOG(0, 0, "线程[%d]，线程启动", ParameterContent->byEventIndex);
	TCHAR tchFileCount[MAX_PATH]=TEXT("");  // 临时文件的路径
	TCHAR szInIPath[MAX_PATH]=TEXT(""); // Update.ini的路径
	TCHAR szDowndLoadSavePath[MAX_PATH]=TEXT(""); // 下载保存的路径
	TCHAR szDowndLoadPath[MAX_PATH]=TEXT(""),szDowndloadTMPPath[MAX_PATH]=TEXT(""); //目标下载路径
	TCHAR szCurString[MAX_PATH]=TEXT("");		// 表示当前m_setWantUpdateFile下载中的元素
	TCHAR tchComplete[MAX_PATH]=TEXT("");	// 完成下载配置文件路径
	TCHAR tchKeyName[MAX_PATH]=TEXT("");	// Key名字
	TCHAR tchFileSize[MAX_PATH]=TEXT("");	// 文件大小
	string szMD5Val;	// MD5值
	long lTmpFileCount = -1;

	// 初始化总下载和已下载数组
	pWnd->InitDowndloadArray();

	while(1)
	{
		ZeroMemory(tchFileCount, sizeof(tchFileCount));
		ZeroMemory(szInIPath, sizeof(szInIPath));
		ZeroMemory(szDowndLoadSavePath, sizeof(szDowndLoadSavePath));
		ZeroMemory(szDowndLoadPath, sizeof(szDowndLoadPath));
		ZeroMemory(szDowndloadTMPPath, sizeof(szDowndloadTMPPath));
		ZeroMemory(szCurString, sizeof(szCurString));

		// 取需要下载的文件路径
		bool bDownloading = FALSE;
		DMODE downloadMode = pWnd->m_eResumeMode;
		////////////////////关键代码段保护 - 开始///////////////////////
		Sleep(1);
		OutputDebugString("进入关键代码段, GameUpdateDlg, ThreadWorkFun GuardWantUpdateArray(&(pWnd->criticalWantUpdateFile));");//Test
		CThreadGuard GuardWantUpdateArray(&(pWnd->criticalWantUpdateFile));
		map<string, FileHashTable>::iterator beg=pWnd->m_setWantUpdateFile.begin();
		for (; beg!=pWnd->m_setWantUpdateFile.end(); ++beg)
		{
			if (FALSE == beg->second.bDowndloadComplete  &&  FALSE == beg->second.bDowndLoading)
			{
				// 下载失败次数超过最大范围
				if (FILE_DOWNLOAD_COUNT < beg->second.uDowndloadCount)
				{
					LOG(0, 0, "线程[%d]，Error: 重试下载失败，%d次 %s", ParameterContent->byEventIndex, 
						beg->second.uDowndloadCount, beg->second.path.c_str());
					pWnd->PostMessage(WM_USER_MSG, (WPARAM)NOTIFY_FINISH_UPDATE, (LPARAM)0);
					return FALSE;
				}
				else if (FILE_DOWNLOAD_COUNT == beg->second.uDowndloadCount)
				{
					downloadMode = UM_DLL_MODE;
				}

				if (-1 == lTmpFileCount)
				{
					pWnd->m_lTmpFileCount++;
					lTmpFileCount = pWnd->m_lTmpFileCount;
				}
				_snprintf(tchFileCount, sizeof(tchFileCount), TEXT(".tmp%d"), lTmpFileCount);
				beg->second.tmpFileName = (TCHAR *)tchFileCount;
				beg->second.bDowndLoading = TRUE;   // 设置正在下载
				beg->second.uDowndloadCount++;

				// INI文件路径，本地保存路径
				_snprintf(szInIPath, sizeof(szInIPath), _T("%sconfig\\%s"), GetAppDirectory(), UPDATE_CONFIG_FILENAME);
				_snprintf(szDowndLoadSavePath, sizeof(szDowndLoadSavePath), _T("%s%s\\%s%s"), GetAppDirectory(), DOWNDLOAD_TMP_PATH, 
					beg->second.filename.c_str(), beg->second.tmpFileName.c_str());
				_snprintf(szCurString, sizeof(szCurString), _T("%s"), beg->first.c_str());

				GetPrivateProfileString(_T("UpdateAllFile"), _T("URL"), DOWNDLOAD_DEFAULT_PATH, szDowndloadTMPPath, MAX_PATH, szInIPath);
				_snprintf(szDowndLoadPath, sizeof(szDowndLoadPath), _T("%s/%s"), szDowndloadTMPPath, beg->second.path.c_str());
				szMD5Val = beg->second.Hash;
				break;
			}
			else if (TRUE==beg->second.bDowndLoading  &&  FALSE==beg->second.bDowndloadComplete)
			{
				bDownloading = true;
			}
		}

		// 已经没有文件需要下载
		if (beg == pWnd->m_setWantUpdateFile.end())
		{
			// 还有文件在下载，UpdateFile由最后那个线程来调用
			if ( !bDownloading  &&  FALSE == pWnd->m_bIsCopyfile)
			{
				pWnd->m_bIsCopyfile = TRUE;
				pWnd->StartCopyFile();
			}

			////////////////////关键代码段保护 - 结束///////////////////////
			GuardWantUpdateArray.leave();
			OutputDebugString("离开关键代码段, GameUpdateDlg, ThreadWorkFun GuardWantUpdateArray.leave()  0 pos");//Test

			LOG(0, 0, "线程[%d]，线程退出", ParameterContent->byEventIndex);
			break; // 跳出 while
		}
		////////////////////关键代码段保护 - 结束///////////////////////
		GuardWantUpdateArray.leave();
		OutputDebugString("离开关键代码段, GameUpdateDlg, ThreadWorkFun GuardWantUpdateArray.leave()  1 pos");//Test

		CString strURL(szDowndLoadPath);
		strURL.Replace(_T("\\"), _T("/"));
		strURL.Replace(_T("\\\\"), _T("/"));
		strURL.Replace(_T("///"), _T("/"));
		strURL.Replace(_T("//"), _T("/"));
		strURL.Replace(_T(":/"), _T("://"));
		_snprintf(szDowndLoadPath, sizeof(szDowndLoadPath), "%s", strURL);		
		LOG(0, 0, "线程[%d]，准备下载 %s", ParameterContent->byEventIndex, szDowndLoadPath);
		BOOL bResult = DowndloadFile(ParameterContent, szDowndLoadPath, szDowndLoadSavePath, lTmpFileCount, downloadMode);
		DWORD dwResult = 0l;

		switch (downloadMode)
		{
		case IE_MODE:
		case BreakPoints_MODE:
			{
				dwResult = WAIT_OBJECT_0+1;
				break;
			}
		case UM_DLL_MODE:
			{
				// 下载事件等待
				dwResult = WaitForMultipleObjects(2, pWnd->m_hDownloadEvent[ParameterContent->byEventIndex], FALSE, INFINITE);
			}
			break;
		}


		switch (dwResult)
		{
		case WAIT_OBJECT_0:	// 停止下载
			{
				//ParameterContent->asynchronous.Stop();
				ParameterContent->Internet.bInternetGetURLUserBreak = TRUE;
				return 0;
			}
		case WAIT_OBJECT_0+1: // 下载完成文件
			{
				if (FALSE == bResult)
				{
					ThreadSetFileStatus(ParameterContent, szCurString, FALSE, FALSE);
					continue;
				}
				else
				{
					bool bResult = VerifyFile(ParameterContent, szDowndLoadSavePath, szMD5Val);

					if (bResult)
					{
						UINT bFileCount = 0;
						////////////////////关键代码段保护 - 开始///////////////////////
						OutputDebugString("GameUpdateDlg.cpp,进入关键代码段, ThreadWorkFun,GuardDowndloadInfo(&(pWnd->criticalDowndloadInfo));");//Test
						CThreadGuard GuardDowndloadInfo(&(pWnd->criticalDowndloadInfo));
						pWnd->m_FinishedFileInfo.iFileCount++;
						bFileCount = pWnd->m_FinishedFileInfo.iFileCount;
						//记录下载完成的文件，为续传准备
						_snprintf(tchComplete, sizeof(tchComplete), TEXT("%s\\%s\\DownloadComplete.ini"), GetAppDirectory(), DOWNDLOAD_TMP_PATH);
						_snprintf(tchKeyName, sizeof(tchKeyName), TEXT("DownloadFile%d"), bFileCount);
						WritePrivateProfileString(tchKeyName, TEXT("filename"), beg->second.filename.c_str(), tchComplete);
						WritePrivateProfileString(tchKeyName, TEXT("tmpfilename"), beg->second.tmpFileName.c_str(), tchComplete);
						WritePrivateProfileString(tchKeyName, TEXT("datname"), beg->second.DATName.c_str(), tchComplete);
						WritePrivateProfileString(tchKeyName, TEXT("path"), beg->second.path.c_str(), tchComplete);
						WritePrivateProfileString(tchKeyName, TEXT("hash"), beg->second.Hash.c_str(), tchComplete);
						_snprintf(tchFileSize, sizeof(tchFileSize), "%I64u", beg->second.size);
						WritePrivateProfileString(tchKeyName, TEXT("size"), tchFileSize, tchComplete);
						////////////////////关键代码段保护 - 结束///////////////////////
						GuardDowndloadInfo.leave();
						OutputDebugString("GameUpdateDlg.cpp,离开关键代码段, ThreadWorkFun,GuardDowndloadInfo.leave();");//Test
						LOG(0, 0, "线程[%d]，写入DownloadComplete.ini，为续传作准备 -当前文件临时索引 %d,%s", 
							ParameterContent->byEventIndex, bFileCount, beg->second.path.c_str());

						// 下载，且校验成功，临时扩展名才允许累加
						lTmpFileCount = -1;
						ThreadSetFileStatus(ParameterContent, szCurString, TRUE, FALSE);
						break;
					}

					ThreadSetFileStatus(ParameterContent, szCurString, FALSE, FALSE);
				}
				break;
			}			
		case WAIT_FAILED: // 下载失败
			{
				LOG(0, 0, "线程[%d]，Error: 下载失败 \"%s\" CGameUpdateDlg::ThreadWorkFun - WaitForSingleObject - WAIT_FAILED .", 
					ParameterContent->byEventIndex, szDowndLoadSavePath);
				ThreadSetFileStatus(ParameterContent, szCurString, FALSE, FALSE);
				break;
			}
		default:break;
		}//end switch
	}

	return 1;
}

BOOL CGameUpdateDlg::DowndloadFile(ThreadParameter *ParameterContent, const TCHAR szDowndLoadPath[MAX_PATH], 
										 const TCHAR szDowndLoadSavePath[MAX_PATH], long llFileIndex/*=-1*/, DMODE bSynchronous/*=TRUE*/)
{
	CGameUpdateDlg *pWnd = (CGameUpdateDlg *)ParameterContent->wndPointer;
	if (NULL == ParameterContent || NULL == pWnd || 
		0==lstrlen(szDowndLoadPath) || 0==lstrlen(szDowndLoadSavePath))
	{
		return FALSE;
	}

	if (!pWnd->m_bSilenceMode)
	{
		pWnd->PostMessage(WM_USER_MSG, (WPARAM)NOTIFY_DOWNLOADING_FILENAME, 
			(LPARAM)szDowndLoadPath);
	}

	// 开始续传模式下载文件
	switch (bSynchronous)
	{
	case IE_MODE:
		{
			int iResult = ParameterContent->Internet.InternetGetURL(szDowndLoadPath, szDowndLoadSavePath, NULL, 
				pWnd->GetSafeHwnd(), llFileIndex);
			if (CInternet::INTERNET_SUCCESS != iResult)
			{
				LOG(0, 0, "线程[%d]，Error: 下载失败 IE_MODE模式 INTERNET_SUCCESS != iResult  \"%s\".", ParameterContent->byEventIndex, szDowndLoadSavePath);
				return FALSE;
			}
		}
	case BreakPoints_MODE:
		{
			if (BreakPoints_MODE == bSynchronous)
			{
				char szError[256] = {0};
				char cRemotePath[MAX_PATH]={0}, cSavePath[MAX_PATH]={0};
				_snprintf(cRemotePath, sizeof(cRemotePath), TEXT("%s"), szDowndLoadPath);
				_snprintf(cSavePath, sizeof(cSavePath), TEXT("%s"), szDowndLoadSavePath);

				bool bResult = true;//pWnd->m_stDownload[ParameterContent->byEventIndex].Synchronization_DownWebFile(
				//ParameterContent->byEventIndex, cRemotePath, cSavePath, szError);
			}


			CFile file;
			if ( file.Open(szDowndLoadSavePath, CFile::modeRead))
			{
				file.Close();
				LOG(0, 0, "线程[%d]，下载成功  %s模式 \"%s\".", ParameterContent->byEventIndex, 
					(BreakPoints_MODE==bSynchronous?_T("BreakPoints_MODE"):_T("IE_MODE")),
					szDowndLoadSavePath);
				return TRUE;
			}
			// 记录下载文件失败日志
			if (!pWnd->m_bSilenceMode)
			{
				LOG(0, 0, _T("线程[%d]，Error: 下载失败  %s模式 \"%s\"."), ParameterContent->byEventIndex, 
					(BreakPoints_MODE==bSynchronous?_T("BreakPoints_MODE"):_T("IE_MODE")),
					szDowndLoadSavePath);
			}
			// 下载失败，暂停1秒，再重新下载
			Sleep(1000);
			return FALSE;
		}
	case UM_DLL_MODE:
		{
			char szError[256] = {0};
			char cRemotePath[MAX_PATH]={0}, cSavePath[MAX_PATH]={0};
			_snprintf(cRemotePath, sizeof(cRemotePath), TEXT("%s"), szDowndLoadPath);
			_snprintf(cSavePath, sizeof(cSavePath), TEXT("%s"), szDowndLoadSavePath);
			BOOL _bResult = CCDownloadManager::getInstance().Init(cRemotePath ,  cSavePath, FunDownLoadFinish, pWnd);
			CCDownloadManager::getInstance().Start();
			break;
		}
	default: ASSERT(0); break;
	}
}

BOOL CGameUpdateDlg::VerifyFile(ThreadParameter *ParameterContent, const TCHAR szDowndLoadSavePath[MAX_PATH], string strMd5Val)
{
	CGameUpdateDlg *pWnd = (CGameUpdateDlg *)ParameterContent->wndPointer;
	if (NULL == ParameterContent || NULL == pWnd || 
		0==lstrlen(szDowndLoadSavePath) || strMd5Val.empty())
	{
		return FALSE;
	}

	// 校验文件 - 计算文件的Hash码以进行比较
	unsigned char acMD5Digest[16];
	CalculateMD5(szDowndLoadSavePath, acMD5Digest);

	if (0 == strMd5Val.compare(MD5toString(acMD5Digest)))
	{
		LOG(0, 0, "线程[%d]，效验成功 \"%s\" .", ParameterContent->byEventIndex, szDowndLoadSavePath);
		return TRUE;
	}
	else
	{
		LOG(0, 0, "线程[%d]，Error: 效验失败 函数模块名：\"%s\".", ParameterContent->byEventIndex, szDowndLoadSavePath);
		if (!pWnd->m_bSilenceMode)
		{
			char acBuffer[BUFFER_SIZE];
			memset(acBuffer, 0, BUFFER_SIZE);
			_snprintf(acBuffer, sizeof(acBuffer), "%s", szDowndLoadSavePath);
			pWnd->PostMessage(WM_USER_MSG, (WPARAM)NOTIFY_VERIFY_FILE_FAIL, (LPARAM)acBuffer);
		}

		// 效验失败，暂停1秒，再重新下载
		Sleep(1000);
		return FALSE;
	}
}
void CGameUpdateDlg::ThreadSetFileStatus(ThreadParameter *ParameterContent, TCHAR tchFirstParam[MAX_PATH], BOOL bComplete, BOOL bDownloading)
{	
	Sleep(1);
	CGameUpdateDlg *pWnd = (CGameUpdateDlg *)ParameterContent->wndPointer;
	OutputDebugString("GameUpdateDlg.cpp,进入关键代码段, ThreadSetFileStatus,Guard(&pWnd->criticalWantUpdateFile);");//Test
	CThreadGuard Guard(&pWnd->criticalWantUpdateFile);
	map<string, FileHashTable>::iterator beg=pWnd->m_setWantUpdateFile.begin();
	for (; beg!=pWnd->m_setWantUpdateFile.end(); ++beg)
	{
		if (0 == beg->second.path.compare(tchFirstParam))
		{
			beg->second.bDowndLoading = bDownloading;
			beg->second.bDowndloadComplete = bComplete;
		}
	}

	OutputDebugString("GameUpdateDlg.cpp,离开关键代码段, ThreadSetFileStatus, Guard.leave();");//Test
}

void CGameUpdateDlg::OnDownloadResult (int iCookie, char* pszLocalPathFile, int iRes, int iSuccessDownload, int iTotalDownload)
{
	//// 已下载字节数
	//ULONGLONG ullTmpSize = 0l;
	//m_strDowndloadCount.erase(pszLocalPathFile);
	//m_strDowndloadCount.insert(make_pair(pszLocalPathFile, iSuccessDownload));

	//map<string, ULONGLONG>::const_iterator cMap=m_strDowndloadCount.begin();
	//for (; cMap != m_strDowndloadCount.end(); ++cMap)
	//{
	//	ullTmpSize += cMap->second;
	//}
	//m_lDownBaiBi = 1.0*((DOUBLE)(ullTmpSize + m_FinishedFileInfo.iFileSize) / (DOUBLE)m_TotalFileInfo.iFileSize)*100;

	//CString str;
	//str.Format("num: 索引%s, 已下载 %u,  总大小 %u", pszLocalPathFile, iSuccessDownload, iTotalDownload);
	//OutputDebugString(str);
	//str.Format("num: 完成数组 %I64u,  所有文件数组 %I64u,  总大小 %I64u", ullTmpSize, m_FinishedFileInfo.iFileSize, m_TotalFileInfo.iFileSize);
	//OutputDebugString(str);

	//////////////////////////////////////////////////////////////////////////
	/// 下载完成后胡操作
	if (20 == iRes) // 成功下载
	{
		LOG(0, 0, "线程[%d]，下载成功 \"%s\".", iCookie, pszLocalPathFile);
		SetEvent(m_hCompleteEvent[iCookie]);
	}
	else if (30 == iRes) // 下载失败
	{		
		LOG(0, 0, _T("线程[%d]，Error: 下载失败 \"%s\"."), iCookie, pszLocalPathFile);
		SetEvent(m_hCompleteEvent[iCookie]);
	}
}

BOOL CGameUpdateDlg::IsHavePriorityUpdate(string strFileName)
{
	// 自己是必须优先更新的，防止配置文件去掉
	if (0 == strFileName.compare(PROJECT_NAME))
	{
		return TRUE;
	}

	// 辅助更新进程有更新，必须优先
	if (0 == strFileName.compare(HELP_UPDATE_NAME))
	{
		return TRUE;
	}

	// 那些文件需要先更新
	vector<string>::iterator vbeg=m_setPriorityDowndload.begin();
	for (; vbeg != m_setPriorityDowndload.end(); vbeg++)
	{
		if (0 == strFileName.compare(*vbeg))
		{
			return TRUE;
		}
	}

	return FALSE;
}
void CGameUpdateDlg::OnTimer(UINT nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (WM_USER+1 == nIDEvent)
	{
		// 安静模式，或者窗口已经显示，才能升级
		if (m_bSilenceMode || TRUE == IsWindowVisible())
		{
			KillTimer(WM_USER+1);
			CheckUpdateFile();
		}
		return;
	}
	else if (WM_USER+2 == nIDEvent)
	{
		//try
		//{
		//	float rate = ((DOUBLE)(m_FinishedFileInfo.iFileSize) / (DOUBLE)m_TotalFileInfo.iFileSize)*100;
		//	m_lDownBaiBi = (int)rate;

		//	CString str;
		//	str.Format("new: 完成数组 (%I64u, %I64u),  ", m_FinishedFileInfo.iFileSize, m_TotalFileInfo.iFileSize);
		//	OutputDebugString(str);
		//}
		//catch(...)
		//{
		//	;
		//}

		if (FALSE==m_bSilenceMode && TRUE == IsWindowVisible())
		{
			m_btNums = (++m_btNums) < 19 ?m_btNums:0;
			InvalidateRect(NULL);
		}
		return;
	}

	CDialog::OnTimer(nIDEvent);
}

BOOL CGameUpdateDlg::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	return FALSE;
	return CDialog::OnEraseBkgnd(pDC);
}

void CGameUpdateDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	PostMessage(WM_NCLBUTTONDOWN, HTCAPTION, MAKELPARAM(point.x,point.y));
	CDialog::OnLButtonDown(nFlags, point);
}

void CGameUpdateDlg::OnBtnDown()
{
	// 域名修正
	//string strHallDowndloadURL(m_chURL);
	//int iStart = sizeof("http://");
	//int iEnd = strHallDowndloadURL.find(TEXT("/"), iStart-1);
	//string strResult = strHallDowndloadURL.substr(0, iEnd);

	//CString strUrl;
	//strUrl.Format("%s/download.php?id=100", strResult.c_str());
	ShellExecute(NULL, TEXT("open"), _T("http://www.cbwgame.com/download.php?id=100"), NULL, NULL, SW_SHOWDEFAULT);
}
BOOL CGameUpdateDlg::DestroyWindow()
{
	// TODO: 在此添加专用代码和/或调用基类
	for (BYTE i=0; i<m_uThreadCount; i++)
	{
		SetEvent(m_hStopEvent[i]);
	}

	CCDownloadManager::getInstance().Clear();
	WaitForMultipleObjects(m_uThreadCount, m_hUpdateThread, TRUE, 5000);
	return CDialog::DestroyWindow();
}

void CGameUpdateDlg::CheckProcess(LPCTSTR lpProcessName)
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

//下载结果
void CGameUpdateDlg::DownResult(char* p, bool bIsSuc)
{
	m_FinishedFileInfo.iFileSize +=  CCDownloadManager::getInstance().GetFileSize();
	if (bIsSuc)
	{//下载成功
		//m_iDownTag = 2;	
		SetEvent(m_hCompleteEvent[0]);
	}
	else
	{
		SetEvent(m_hCompleteEvent[0]);
		//下载失败
		//m_iDownTag = 3;	
	}

	if (bIsSuc)
	{
		CFile file;
		if ( file.Open(p, CFile::modeRead))
		{
			file.Close();
			LOG(0, 0, "线程[0]，下载成功  UM_DLL_MODE模式 \"%s\".", p);
			return;
		}
	}

	// 记录下载文件失败日志
	LOG(0, 0, _T("线程[0]，Error: 下载失败  UM_DLL_MODE模式 \"%s\"."), p);
}