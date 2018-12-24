#include "StdAfx.h"
#include "Direct.h"
#include "DownLoadService.h"
#include ".\downloadservice.h"
//#include <fstream>
//
//
//std::ofstream out("download-info.txt", std::ios::trunc); 
//
//static void WriteInfo( char* p)
//{
//	out << p << std::endl;
//}


void FunDownLoadFinish(char* p, bool isfinish, void* pObj)
{
	CDownLoadMission* pDownSuc = (CDownLoadMission*)pObj;
	if (NULL != pDownSuc)
	{
		pDownSuc->DownResult(isfinish);
	}
}


//////////////////////////////////////////////////////////////////////////
//宏定义

//定时器标识
#define IDI_UPDATE_TIMER					10							//更新定时器

//获取错误 HTML
const TCHAR szErrorHtml1[]=TEXT("<html>");
const TCHAR szErrorHtml2[]=TEXT("<!DOCTYPE HTML PUBLIC");

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CDownLoadMission, CDialog)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BT_HIT_HERE, OnBnClickedHere)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_DESTROY()
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CDownLoadThread::CDownLoadThread()
{
	//设置变量
	m_bPreparative=false;
	m_szTempFile[0]=0;
	m_szLocalFile[0]=0;
	m_dwDownLoadSize=0;
	m_dwTotalFileSize=0;
	m_pHttpFile=NULL;
	m_pHttpConnection=NULL;
	m_pDownLoadRequest=NULL;
	m_DownLoadResult=enDownLoadResult_Noknow;
	m_DownLoadStatus=enDownLoadStatus_Unknow;

	return;
}

//析构函数
CDownLoadThread::~CDownLoadThread()
{
	DownLoadCleanUp();
	return;
}

//初始化线程
bool CDownLoadThread::InitThread(tagDownLoadRequest * pDownLoadRequest)
{
	ASSERT(pDownLoadRequest!=NULL);
	m_pDownLoadRequest=pDownLoadRequest;
	return true;
}

//下载状态
void CDownLoadThread::GetDownLoadStatus(tagDownLoadStatus & DownLoadStatus)
{
	//锁定资源
	CThreadLock ThreadLockHandle(m_CriticalSection);

	//设置变量
	DownLoadStatus.DownLoadStatus=m_DownLoadStatus;
	switch (m_DownLoadStatus)
	{
	case enDownLoadStatus_Unknow:
	case enDownLoadStatus_Ready:
		{
			DownLoadStatus.wProgress=0;
			lstrcpyn(DownLoadStatus.szStatus,TEXT("正在获取文件..."),CountArray(DownLoadStatus.szStatus));
			break;
		}
	case enDownLoadStatus_DownLoadIng:
		{
			DownLoadStatus.wProgress=(WORD)(m_dwDownLoadSize*100L/m_dwTotalFileSize);
			_snprintf(DownLoadStatus.szStatus,sizeof(DownLoadStatus.szStatus),
				TEXT("已复制：%ld KB （共 %ld KB）"),m_dwDownLoadSize/1024L,m_dwTotalFileSize/1024L);
			break;
		}
	case enDownLoadStatus_Finish:
		{
			DownLoadStatus.wProgress=100;
			lstrcpyn(DownLoadStatus.szStatus,TEXT("下载成功"),CountArray(DownLoadStatus.szStatus));
			break;
		}
	case enDownLoadStatus_Fails:
		{
			DownLoadStatus.wProgress=0;
			_snprintf(DownLoadStatus.szStatus,sizeof(DownLoadStatus.szStatus),TEXT("下载失败，错误号：%ld"),m_DownLoadResult);
			break;
		}
	default: 
		{
			ASSERT(FALSE);
			memset(&DownLoadStatus,0,sizeof(DownLoadStatus));
			break;
		}
	}

	return;
}

//目标文件
LPCTSTR CDownLoadThread::GetDownLoadFileName()
{
	ASSERT(m_DownLoadStatus==enDownLoadStatus_Finish);
	return m_szLocalFile;
}

//运行函数
bool CDownLoadThread::OnEventThreadRun()
{
	try
	{
		if (m_bPreparative==true)
		{//初始化已完成

			//读取文件
			TCHAR szBuffer[4096]={0};
			UINT uReadCount=m_pHttpFile->Read(szBuffer,sizeof(szBuffer));

			if (uReadCount>0)
			{//正在下载
				//写入文件
				m_LocalFile.Write(szBuffer,uReadCount);

				//锁定资源
				CThreadLock ThreadLockHandle(m_CriticalSection);

				//设置变量
				m_dwDownLoadSize+=uReadCount;
				m_DownLoadStatus=enDownLoadStatus_DownLoadIng;

				return true;
			}
			else 
			{//下载完成
				//关闭文件
				m_LocalFile.Close();

				//移动文件
				_snprintf(m_szLocalFile,sizeof(m_szLocalFile),TEXT("%s\\%s"),m_pDownLoadRequest->szLocalPath,m_pDownLoadRequest->szFileName);
				DeleteFile(m_szLocalFile);
				if (MoveFileWithProgress(m_szTempFile,m_szLocalFile,NULL, NULL, MOVEFILE_WRITE_THROUGH|MOVEFILE_REPLACE_EXISTING)==FALSE)
					throw enDownLoadResult_CreateFileFails;

				//锁定资源
				CThreadLock ThreadLockHandle(m_CriticalSection);

				//设置变量
				m_DownLoadStatus=enDownLoadStatus_Finish;

				//下载清理
				DownLoadCleanUp();

				return false;
			}
		}
		else 
		{
			//锁定资源
			CThreadLock ThreadLockHandle(m_CriticalSection);

			//设置变量
			m_DownLoadStatus=enDownLoadStatus_Ready;

			//下载准备
			DownLoadPreparative();

			return true;
		}
	}
	catch (enDownLoadResult DownLoadResult)
	{
		//锁定资源
		CThreadLock ThreadLockHandle(m_CriticalSection);

		//设置变量
		m_DownLoadStatus=enDownLoadStatus_Fails;
		m_DownLoadResult=DownLoadResult;
	}
	catch (CInternetException * pInternetException)
	{
		//清理异常
		pInternetException->Delete();

		//锁定资源
		CThreadLock ThreadLockHandle(m_CriticalSection);

		//设置变量
		m_DownLoadStatus=enDownLoadStatus_Fails;
		m_DownLoadResult=enDownLoadResult_Exception;
	}
	catch (...) 
	{
		//锁定资源
		CThreadLock ThreadLockHandle(m_CriticalSection);

		//设置变量
		m_DownLoadStatus=enDownLoadStatus_Fails;
		m_DownLoadResult=enDownLoadResult_Exception;
	}

	//清理处理
	DownLoadCleanUp();

	return false;
}

//关闭事件
bool CDownLoadThread::OnEventThreadConclude()
{
	DownLoadCleanUp();
	return true;
}

//下载准备
void CDownLoadThread::DownLoadPreparative()
{
	//效验状态
	ASSERT(m_pHttpFile==NULL);
	ASSERT(m_bPreparative==false);

	//设置变量
	m_szTempFile[0]=0;
	m_szLocalFile[0]=0;
	m_dwDownLoadSize=0;
	m_dwTotalFileSize=0;

	//建立目录
	int nExcursion=0;
	TCHAR szDirectory[MAX_PATH]=TEXT("");
	lstrcpyn(szDirectory,m_pDownLoadRequest->szLocalPath,CountArray(szDirectory));
	do
	{
		if (szDirectory[nExcursion]==0) 
		{
			::CreateDirectory(szDirectory,NULL);
			break;
		}
		if (szDirectory[nExcursion]==TEXT('\\'))
		{
			szDirectory[nExcursion]=0;
			::CreateDirectory(szDirectory,NULL);
			szDirectory[nExcursion]=TEXT('\\');
		}
		nExcursion++;
	} while (true);

	//判断下载内容
	int nStepCount=0;
	DWORD dwStatusCode =0;
	TCHAR szDownBuffer[4096]={0};
	try
	{
		//读取内容
		UINT nReadCount=0;
		DWORD dwType;
		CString strServer, strObject;
		INTERNET_PORT wPort;
		DWORD dwStateCode;
		

		m_InternetSession.SetOption(INTERNET_OPTION_CONNECT_TIMEOUT, 5000);      // 5秒的连接超时
		m_InternetSession.SetOption(INTERNET_OPTION_SEND_TIMEOUT, 2000);           // 1秒的发送超时    
		m_InternetSession.SetOption(INTERNET_OPTION_RECEIVE_TIMEOUT, 15000);        // 7秒的接收超时
		m_InternetSession.SetOption(INTERNET_OPTION_DATA_SEND_TIMEOUT, 2000);     // 1秒的发送超时
		m_InternetSession.SetOption(INTERNET_OPTION_DATA_RECEIVE_TIMEOUT, 15000);       // 7秒的接收超时
		m_InternetSession.SetOption(INTERNET_OPTION_CONNECT_RETRIES, 5);          // 1次重试
		
		AfxParseURL(m_pDownLoadRequest->szDownLoadUrl, dwType, strServer, strObject, wPort);

		m_pHttpConnection = m_InternetSession.GetHttpConnection(strServer, wPort, NULL, NULL);

		m_pHttpFile = m_pHttpConnection->OpenRequest(CHttpConnection::HTTP_VERB_GET, strObject);


		m_pHttpFile=(CHttpFile *)m_InternetSession.OpenURL(m_pDownLoadRequest->szDownLoadUrl,
															1,
															INTERNET_FLAG_TRANSFER_ASCII|INTERNET_FLAG_DONT_CACHE);
		//m_pHttpFile->SendRequest();
		//m_pHttpFile->QueryInfoStatusCode(dwStateCode);
		//if(dwStateCode != HTTP_STATUS_OK)
		//{
		//	return;
		//}

		DWORD dwFileSize = 0;   //文件长度
		DWORD dwInfoBufferSize = sizeof(szDownBuffer);
		BOOL bResult = FALSE;
		bResult = m_pHttpFile->QueryInfo(HTTP_QUERY_CONTENT_LENGTH,
			(void*)szDownBuffer,&dwInfoBufferSize,NULL);
		if (FALSE==bResult)
		{
			return;
		}

		m_dwTotalFileSize = atoi(szDownBuffer);
		memset(szDownBuffer, 0, sizeof(szDownBuffer));
		nReadCount = m_pHttpFile->Read(szDownBuffer, sizeof(szDownBuffer)); //读取服务器上数据

		//nStepCount++;
		//m_pHttpFile->QueryInfoStatusCode(dwStatusCode);
		//nReadCount=m_pHttpFile->Read(szDownBuffer,sizeof(szDownBuffer));
		//nStepCount++;
		//if ( nReadCount < sizeof( szErrorHtml1 ) ) 
		//{
		//	throw 0;
		//}
		//nStepCount++;
		//if ( nReadCount < sizeof( szErrorHtml2 ) ) 
		//{
		//	throw 0;
		//}
		//nStepCount++;
		//if (memcmp(szErrorHtml1,szDownBuffer,lstrlen(szErrorHtml1))==0) 
		//{
		//	throw 0;
		//}
		//nStepCount++;
		//if (memcmp(szErrorHtml2,szDownBuffer,lstrlen(szErrorHtml2))==0)
		//{
		//	throw 0;
		//}
		//nStepCount++;

		////读取大小
		//ASSERT(m_pHttpFile!=NULL);
		//BOOL bSuccess=m_pHttpFile->QueryInfo(HTTP_QUERY_CONTENT_LENGTH|HTTP_QUERY_FLAG_NUMBER,m_dwTotalFileSize);
		//nStepCount++;
		//if ( FALSE == bSuccess )
		//{
		//	//CString strError;
		//	//DWORD dwError=GetLastError();
		//	//strError.Format(TEXT("错误号：%ld"),dwError);
		//	//AfxMessageBox(strError);
		//	throw enDownLoadResult_InternetReadError;
		//}

		nStepCount++;

		//本地文件
		_snprintf(m_szTempFile,
				  sizeof(m_szTempFile),
				  TEXT("%s\\%s.DTM"),
				  m_pDownLoadRequest->szLocalPath,
				  m_pDownLoadRequest->szFileName);

		if (FALSE == m_LocalFile.Open(m_szTempFile,
							CFile::modeReadWrite|CFile::modeCreate))
		{
			throw enDownLoadResult_CreateFileFails;
		}

		nStepCount++;

		//写入文件
		m_LocalFile.Write(szDownBuffer,nReadCount);
		nStepCount++;

		//锁定资源
		CThreadLock ThreadLockHandle(m_CriticalSection);
		nStepCount++;

		//设置变量
		m_dwDownLoadSize+=nReadCount;
		m_DownLoadStatus=enDownLoadStatus_DownLoadIng;

		//设置变量
		m_bPreparative=true;

		return;
	}
	catch (...)
	{ 
		//CString strError;
		//DWORD dwError=GetLastError();
		//strError.Format(TEXT("执行步骤：%ld,状态码：%ld"),nStepCount,dwStatusCode);
		//AfxMessageBox(szDownBuffer);
		//SafeDelete(m_pHttpFile);
		throw enDownLoadResult_InternetReadError;
	}

	return;
}

//下载完成
void CDownLoadThread::DownLoadCleanUp()
{
	//设置变量
	m_dwDownLoadSize=0;
	m_dwTotalFileSize=0;
	m_bPreparative=false;

	//清理资源
	if (m_pHttpFile!=NULL) 
	{
		m_pHttpFile->Close();
		SafeDelete(m_pHttpFile);
	}
	if (m_pHttpConnection!=NULL)
	{
		m_pHttpConnection->Close();
		SafeDelete(m_pHttpConnection);
	}

	//m_InternetSession.Close();
	if (m_LocalFile!=INVALID_HANDLE_VALUE) m_LocalFile.Close();

	//删除文件
	if (m_szTempFile[0]!=0) DeleteFile(m_szTempFile);

	return;
}

//////////////////////////////////////////////////////////////////////////


//模块定义
#ifdef _DEBUG
#define GAME_DOWNLOAD_DLL_NAME	TEXT("DownLoadD.dll")					//组件 DLL 名字
#else
#define GAME_DOWNLOAD_DLL_NAME	TEXT("DownLoad.dll")					//组件 DLL 名字
#endif


const int IWND_WIDTH		=	536;
const int IWND_HEIGHT		=	150;


const int I_DOWNPROCESS_NUMS	=	19;
const int I_DOWNFAIL_NUMS		=	2;


const int IDI_DOWN_ING_TIMER	=	2001;
const int IDI_DOWN_FAIL_TIMER	=	2002;


const int TIMER_DOWNING_INTERVAL	=	100;
const int TIMER_DOWNFAIL_INTERVAL	=	500;


const int I_FLASH_WIDTH	=	90;
const int I_FLASH_HEIGHT =	86;


//构造函数
CDownLoadMission::CDownLoadMission(IDownLoadMissionSink * pIDownLoadMissionSink) : CDialog(IDD_DOWN_LOAD)
{
	//设置变量
	m_dwMissionID=0;
	m_dwDownLoadType=0;
	m_pIDownLoadMissionSink=pIDownLoadMissionSink;
	//m_DownLoadThread.InitThread(&m_DownLoadRequest);
	memset(&m_DownLoadRequest,0,sizeof(m_DownLoadRequest));

	m_iDownTag = 0;
	m_byDownLoadTag = 1;
	m_iDownBaiBi = 0;
	m_iSaveDownBaiBi = 0;

	m_iCountDownSuc = 0;
	m_iCountDownFail = 0;

	m_memdc.m_hDC = NULL;
	return;
}

//析构函数
CDownLoadMission::~CDownLoadMission()
{
	ReleaseDC(&m_memdc);
	m_memdc.m_hDC = NULL;
}

//控件绑定
void CDownLoadMission::DoDataExchange(CDataExchange* pDX)
{
	__super::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BT_HIT_HERE, m_btHitHereBT);
	DDX_Control(pDX, IDCANCEL, m_btCloseBT);
}

//初始化函数
BOOL CDownLoadMission::OnInitDialog()
{
	__super::OnInitDialog();

	HINSTANCE hInstance=GetModuleHandle(IDOWN_LOAD_DLL_NAME);
	TCHAR szCurrentPath[MAX_PATH];
	::memset(szCurrentPath, 0, sizeof(szCurrentPath));
	GetCurrentDirectory(MAX_PATH,szCurrentPath);

	CString strResourcePath2="";
	strResourcePath2.Format("%s\\data\\ClientShare\\download\\",szCurrentPath);
	
	m_ImageDownLoadFlash.DestroyImage();
	m_ImageDownLoadFlash.LoadImage(TEXT(strResourcePath2 + "DownLoadFlash.png"));
	m_ImageDownLoadNum.DestroyImage();
	m_ImageDownLoadNum.LoadImage(TEXT(strResourcePath2 + "DownLoadNum.png"));
	m_ImageFailFlash.DestroyImage();
	m_ImageFailFlash.LoadImage(TEXT(strResourcePath2 + "FailFlash.png"));
	m_ImageImageBack.DestroyImage();
	m_ImageImageBack.LoadImage(TEXT(strResourcePath2 + "ImageBack.png"));
	m_ImageImageFrontGree.DestroyImage();
	m_ImageImageFrontGree.LoadImage(TEXT(strResourcePath2 + "ImageFrontGreen.png"));
	m_ImageImageFrontRed.DestroyImage();
	m_ImageImageFrontRed.LoadImage(TEXT(strResourcePath2 + "ImageFrontRed.png"));
	m_ImageImageBai.DestroyImage();
	m_ImageImageBai.LoadImage(TEXT(strResourcePath2 + "ImageBai.png"));
	m_ImageDownBack.DestroyImage();
	m_ImageDownBack.LoadImage(TEXT(strResourcePath2 + "ImageDown.png"));
	m_ImageUpdateBack.DestroyImage();
	m_ImageUpdateBack.LoadImage(TEXT(strResourcePath2 + "ImageUpdate.png"));
	m_ImageDownFail.DestroyImage();
	m_ImageDownFail.LoadImage(TEXT(strResourcePath2 + "ImageDownFail.png"));
	
	
	m_btHitHereBT.SetButtonImage(TEXT(strResourcePath2 + "ImageHere.png"), false);
	m_btCloseBT.SetButtonImage(TEXT(strResourcePath2 + "Close.png"), false);

	SetWindowPos(NULL, 0, 0, IWND_WIDTH, IWND_HEIGHT, SWP_NOMOVE);

	m_btHitHereBT.SetWindowPos(NULL, 349, 94, 0, 0, SWP_NOSIZE);
	m_btCloseBT.SetWindowPos(NULL, IWND_WIDTH-36, 10, 0, 0, SWP_NOSIZE);

	m_iDownTag = 0;
	m_iDownBaiBi = 0;
	m_iSaveDownBaiBi = 0;

	m_iCountDownSuc = 0;
	m_iCountDownFail = 0;

	CDC *pDC = GetDC();
	m_memdc.m_hDC = NULL;
	m_memdc.CreateCompatibleDC(pDC);
	CBitmap hbmp;
	hbmp.CreateCompatibleBitmap(pDC, IWND_WIDTH, IWND_HEIGHT);
	m_memdc.SelectObject(&hbmp);
	m_memdc.SetBkMode(TRANSPARENT);
	hbmp.DeleteObject();
	ReleaseDC(pDC);
	pDC = NULL;


	m_rcFlash.left = 16;
	m_rcFlash.top = 30;
	m_rcFlash.right = m_rcFlash.left + I_FLASH_WIDTH+4;
	m_rcFlash.bottom = m_rcFlash.top + I_FLASH_HEIGHT+4;

	m_rcNums.left = 260;
	m_rcNums.top = 25;
	m_rcNums.right = m_rcNums.left + 100;
	m_rcNums.bottom = m_rcNums.top + 30;

	m_rcProcess.left = 120;
	m_rcProcess.top = 65;
	m_rcProcess.right = m_rcProcess.left + 394;
	m_rcProcess.bottom = m_rcProcess.top + 20;

	//设置定时器
	SetTimer(IDI_DOWN_ING_TIMER,TIMER_DOWNING_INTERVAL,NULL);

	return TRUE;
}


void CDownLoadMission::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	//__super::OnPaint();
	// TODO: 在此处添加消息处理程序代码
	// 不为绘图消息调用 CSkinDialogEx::OnPaint()

	if (NULL != m_memdc.m_hDC)
	{
		if (1==m_byDownLoadTag || 2==m_byDownLoadTag)
		{
			if (1==m_byDownLoadTag && !m_ImageDownBack.IsNull())
			{//下载界面
				m_ImageDownBack.DrawImage(&m_memdc, 0, 0);
			}
			else if (2==m_byDownLoadTag && !m_ImageUpdateBack.IsNull())
			{//更新界面
				m_ImageUpdateBack.DrawImage(&m_memdc, 0, 0);
			}

			if (!m_ImageDownLoadNum.IsNull())
			{//绘制数字
				CPoint ptNum;
				ptNum.x = m_rcNums.left;
				ptNum.y = m_rcNums.top;
				int iNums[3] = {-1};
				int iCount = 0;
				int iValue = m_iDownBaiBi; 
				m_iSaveDownBaiBi = m_iDownBaiBi;
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
				for (int i=iCount-1; i>=0; --i)
				{
					if (iNums[i]>=0)
					{
						m_ImageDownLoadNum.DrawImage(&m_memdc, ptNum.x, ptNum.y+2, 20, 23, iNums[i]*20, 0, 20, 23);
						ptNum.x += 20;
					}
				}
				if (m_iDownBaiBi>=0 && !m_ImageImageBai.IsNull())
				{//百分号
					m_ImageImageBai.DrawImage(&m_memdc, ptNum.x, ptNum.y+2);
				}
			}

			//绘制进度条
			if (!m_ImageImageBack.IsNull())
			{
				m_ImageImageBack.DrawImage(&m_memdc, m_rcProcess.left, m_rcProcess.top);
			}
			if (!m_ImageImageFrontGree.IsNull())
			{
				int iValue = m_iDownBaiBi*3.94;
				m_ImageImageFrontGree.DrawImage(&m_memdc, m_rcProcess.left, m_rcProcess.top, iValue, 17, 0, 0, iValue, 17);
			}

			//绘制动画
			if (!m_ImageDownLoadFlash.IsNull())
			{
				m_ImageDownLoadFlash.DrawImage(&m_memdc, m_rcFlash.left, m_rcFlash.top, I_FLASH_WIDTH, I_FLASH_HEIGHT,	\
					m_iCountDownSuc*I_FLASH_WIDTH, 0, I_FLASH_WIDTH, I_FLASH_HEIGHT);
			}
		}
		else
		{//下载失败界面
			if (!m_ImageDownFail.IsNull())
			{
				m_ImageDownFail.DrawImage(&m_memdc, 0, 0);
			}

			//绘制进度条
			if (!m_ImageImageBack.IsNull())
			{
				m_ImageImageBack.DrawImage(&m_memdc, m_rcProcess.left, m_rcProcess.top);
			}
			if (!m_ImageImageFrontRed.IsNull())
			{
				int iValue = m_iDownBaiBi*3.94;
				m_ImageImageFrontRed.DrawImage(&m_memdc, m_rcProcess.left, m_rcProcess.top, iValue, 17, 0, 0, iValue, 17);
			}

			//绘制动画
			if (!m_ImageFailFlash.IsNull())
			{
				m_ImageFailFlash.DrawImage(&m_memdc, m_rcFlash.left, m_rcFlash.top, I_FLASH_WIDTH, I_FLASH_HEIGHT,	\
					m_iCountDownFail*I_FLASH_WIDTH, 0, I_FLASH_WIDTH, I_FLASH_HEIGHT);
			}
		}

		dc.BitBlt(0,0, IWND_WIDTH, IWND_HEIGHT, &m_memdc, 0,0, SRCCOPY);
	}
}



//////////////////////////////////////////////////////////////////////////

BOOL CDownLoadMission::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	return TRUE;
	//return CDialog::OnEraseBkgnd(pDC);
}

void CDownLoadMission::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CDialog::OnLButtonDown(nFlags, point);
}


//消息过虑
BOOL CDownLoadMission::PreTranslateMessage(MSG * pMsg)
{
	if ((pMsg->message==WM_KEYDOWN)&&(pMsg->wParam==VK_ESCAPE||VK_RETURN==pMsg->wParam ||VK_SPACE==pMsg->wParam)) return TRUE;
	return __super::PreTranslateMessage(pMsg);
}

//取消消息
void CDownLoadMission::OnCancel()
{
	KillTimer(IDI_DOWN_ING_TIMER);
	KillTimer(IDI_DOWN_FAIL_TIMER);

	//停止下载
	//m_DownLoadThread.ConcludeThread(INFINITE);

	//事件通知
	m_pIDownLoadMissionSink->OnMissionFinish(enDownLoadStatus_Fails,this);

	//关闭界面
	DestroyWindow();

	return;
}

//重试按钮
void CDownLoadMission::OnBnClickedReTry()
{
	KillTimer(IDI_DOWN_ING_TIMER);
	KillTimer(IDI_DOWN_FAIL_TIMER);


	//重新下载
	//m_DownLoadThread.StartThread();

	CCDownloadManager::getInstance().GoOn();

	//设置时间
	SetTimer(IDI_DOWN_ING_TIMER,TIMER_DOWNING_INTERVAL,NULL);

	return;
}

//点击这里
void CDownLoadMission::OnBnClickedHere()
{
	if (!m_strNetURL.empty())
	{
		ShellExecute(NULL,TEXT("open"),m_strNetURL.c_str(),NULL,NULL,SW_SHOWDEFAULT);
	}
}


//下载结果
void CDownLoadMission::DownResult(bool bIsSuc)
{
	if (bIsSuc)
	{//下载成功
		m_iDownTag = 2;	
	}
	else
	{//下载失败
		m_iDownTag = 3;	
	}
}


//定时器消息
void CDownLoadMission::OnTimer(UINT_PTR nIDEvent)
{
	__super::OnTimer(nIDEvent);

	//更新状态
	if (nIDEvent==IDI_DOWN_ING_TIMER)
	{
		if (1 == m_iDownTag)
		{
			unsigned __int64  fileSize   =  CCDownloadManager::getInstance().GetFileSize();
			unsigned __int64  downSize   =  CCDownloadManager::getInstance().GetDownLoadSize();
			//int speed = CCDownloadManager::getInstance().GetSpeed();
			if (fileSize > 0)
			{
				float rate = ((DOUBLE)downSize / (DOUBLE)fileSize)*100;
				m_iDownBaiBi = (int)rate;

				//sprintf(buf, "%0.2fKB/%0.2fKB(%0.2fKB/s)", downSize / 1024.0f, fileSize / 1024.0f, (float)speed / 1024.0f);//已下载大小/总下载大小
				//SetDlgItemText(IDC_STATUS,buf);
			}

			++m_iCountDownSuc;
			if (m_iCountDownSuc >= I_DOWNPROCESS_NUMS)
			{
				m_iCountDownSuc = 0;
			}
		}
		else if (2 == m_iDownTag)
		{//下载完成
			KillTimer(IDI_DOWN_ING_TIMER);
			KillTimer(IDI_DOWN_ING_TIMER);

			CCDownloadManager::getInstance().Clear();
			
			DestroyWindow();

			//事件通知
			m_pIDownLoadMissionSink->OnMissionFinish(enDownLoadStatus_Finish,this);

			std::string strFilePath="";
			strFilePath.append(m_DownLoadRequest.szLocalPath);
			strFilePath.append("\\");
			strFilePath.append(m_DownLoadRequest.szFileName);
			//运行安装文件
			ShellExecute(NULL,TEXT("open"),strFilePath.c_str(),NULL,NULL,SW_SHOWDEFAULT);
		}
		else if (3 == m_iDownTag)
		{//下载失败
			//设置界面
			KillTimer(IDI_DOWN_ING_TIMER);
			KillTimer(IDI_DOWN_ING_TIMER);
			//设置界面
			m_byDownLoadTag = 3;
			m_iCountDownFail = 0;
			
			SetTimer(IDI_DOWN_FAIL_TIMER, TIMER_DOWNFAIL_INTERVAL, NULL);
			InvalidateRect(&CRect(100, 10, 500, 50), FALSE);
		}
	}
	else if (IDI_DOWN_FAIL_TIMER == nIDEvent)
	{//下载失败动画
		m_iCountDownFail = !m_iCountDownFail;
	}

	if(GetSafeHwnd())
	{
		InvalidateRect(&m_rcFlash, FALSE);
		InvalidateRect(&m_rcNums, FALSE);
		InvalidateRect(&m_rcProcess, FALSE);
	}


	////设置控件
	//tagDownLoadStatus DownLoadStatus;
	//m_DownLoadThread.GetDownLoadStatus(DownLoadStatus);
	//m_ProgressCtrl.SetPos(DownLoadStatus.wProgress);
	//SetDlgItemText(IDC_STATUS,DownLoadStatus.szStatus);

	////状态切换
	//switch (DownLoadStatus.DownLoadStatus)
	//{
	//case enDownLoadStatus_Fails:
	//	{
	//		//设置界面
	//		KillTimer(IDI_UPDATE_TIMER);
	//		m_btReTry.EnableWindow(TRUE);
	//		
	//		//事件通知
	//		if (m_DownLoadRequest.bDisplay==false)
	//		{
	//			m_pIDownLoadMissionSink->OnMissionFinish(enDownLoadStatus_Fails,this);
	//			DestroyWindow();
	//		}

	//		break;
	//	}
	//case enDownLoadStatus_Finish:
	//	{//下载完成
	//		DestroyWindow();
	//		LPCTSTR pszLocalFile=m_DownLoadThread.GetDownLoadFileName();
	//		ShellExecute(NULL,TEXT("open"),pszLocalFile,NULL,NULL,SW_SHOWDEFAULT);

	//		//事件通知
	//		m_pIDownLoadMissionSink->OnMissionFinish(enDownLoadStatus_Finish,this);

	//		break;
	//	}
	//}
	return;
}

//显示界面
bool CDownLoadMission::DisplayDownLoadFace(bool bDisplay)
{
	//设置变量
	m_DownLoadRequest.bDisplay=bDisplay;

	//显示界面
	if (m_hWnd!=NULL)
	{
		ShowWindow(bDisplay?SW_SHOW:SW_HIDE);
	}

	return true;
}

//对比请求
bool CDownLoadMission::CompareRequest(tagDownLoadRequest * pDownLoadRequest)
{
	if (lstrcmp(m_DownLoadRequest.szDownLoadUrl,pDownLoadRequest->szDownLoadUrl)!=0) return false;
	return true;
}

//开始下载
bool CDownLoadMission::StartDownLoad(DWORD dwMissionID, DWORD dwDownLoadType, tagDownLoadRequest * pDownLoadRequest)
{
	if (NULL==pDownLoadRequest)
	{
		return false;
	}
	
	//设置变量
	m_dwMissionID=dwMissionID;
	m_dwDownLoadType=dwDownLoadType;
	CopyMemory(&m_DownLoadRequest,pDownLoadRequest,sizeof(tagDownLoadRequest));
	if (m_DownLoadRequest.szLocalPath[0]==0)
	{
		GetCurrentDirectory(sizeof(m_DownLoadRequest.szLocalPath),
							m_DownLoadRequest.szLocalPath);
	}

	//启动下载
	//m_DownLoadThread.StartThread();

	std::string strURL="", strFilePath="";
	strURL.append(pDownLoadRequest->szDownLoadUrl);
	strFilePath.append(pDownLoadRequest->szLocalPath);
	strFilePath.append("\\");
	strFilePath.append(pDownLoadRequest->szFileName);

	m_strNetURL = "";
	m_strNetURL.append(strURL);

	m_strLocalFilePath = "";
	m_strLocalFilePath.append(strFilePath);

	DeleteFile(strFilePath.c_str());

	//开始下载
	CCDownloadManager::getInstance().Init(strURL.c_str() ,  strFilePath.c_str(), FunDownLoadFinish, this);
	CCDownloadManager::getInstance().Start();

	//创建界面
	AfxSetResourceHandle(GetModuleHandle(IDOWN_LOAD_DLL_NAME));
	Create(IDD_DOWN_LOAD,AfxGetMainWnd());
	AfxSetResourceHandle(GetModuleHandle(NULL));
	if (m_DownLoadRequest.bDisplay==true)
	{
		m_iDownTag = 1;
		m_byDownLoadTag = pDownLoadRequest->byDownLoadTag;
		ShowWindow(SW_SHOW);
	}

	return true;
}

//停止下载
bool CDownLoadMission::StopDownLoad()
{
	KillTimer(IDI_DOWN_ING_TIMER);

	//停止线程
	//m_DownLoadThread.ConcludeThread(INFINITE);
	CCDownloadManager::getInstance().Clear();

	//关闭界面
	DestroyWindow();

	return true;
}

//////////////////////////////////////////////////////////////////////////


//构造函数
CDownLoadService::CDownLoadService()
{
	m_dwMissionID=0;
	m_pIDownLoadServiceSink=NULL;
}

//析构函数
CDownLoadService::~CDownLoadService()
{
	//清理任务
	CDownLoadMission * pDownLoadMission=NULL;
	for (INT_PTR i=0;i<m_DownLoadMissionActive.GetCount();i++)
	{
		pDownLoadMission=m_DownLoadMissionActive[i];
		pDownLoadMission->StopDownLoad();
		SafeDelete(pDownLoadMission);
	}
	for (INT_PTR i=0;i<m_DownLoadMissionRelease.GetCount();i++)
	{
		pDownLoadMission=m_DownLoadMissionRelease[i];
		SafeDelete(pDownLoadMission);
	}
	m_DownLoadMissionActive.RemoveAll();
	m_DownLoadMissionRelease.RemoveAll();
	
	return;
}

//接口查询
void * __cdecl CDownLoadService::QueryInterface(const IID & Guid, DWORD dwQueryVer)
{
	//QUERYINTERFACE(IDownLoadService,Guid,dwQueryVer);
	//QUERYINTERFACE_IUNKNOWNEX(IDownLoadService,Guid,dwQueryVer);
	return NULL;
}

//获取数目
INT_PTR __cdecl CDownLoadService::GetDownLoadMissionCount()
{
	return m_DownLoadMissionActive.GetCount();
}

//设置接口
bool __cdecl CDownLoadService::SetDownLoadServiceSink(IUnknownEx * pIUnknownEx)
{
	ASSERT(pIUnknownEx!=NULL);
	m_pIDownLoadServiceSink=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,IDownLoadServiceSink);
	ASSERT(m_pIDownLoadServiceSink!=NULL);
	return (m_pIDownLoadServiceSink!=NULL);
}

//下载请求
DWORD __cdecl CDownLoadService::AddDownLoadRequest(DWORD dwDownLoadType, tagDownLoadRequest * pDownLoadRequest)
{
	//效验参数
	ASSERT(pDownLoadRequest!=NULL);
	if (pDownLoadRequest==NULL) return 0;

	//寻找下载
	CDownLoadMission * pDownLoadMission=SearchMission(pDownLoadRequest);
	if (pDownLoadMission!=NULL)
	{
		if (pDownLoadRequest->bDisplay==true) pDownLoadMission->DisplayDownLoadFace(pDownLoadRequest->bDisplay);
		return pDownLoadMission->GetMissionID();
	}

	//创建下载
	INT_PTR nReleaseCount=m_DownLoadMissionRelease.GetCount();
	if (nReleaseCount>0)
	{
		pDownLoadMission=m_DownLoadMissionRelease[nReleaseCount-1];
		m_DownLoadMissionRelease.RemoveAt(nReleaseCount-1);
	}
	else
	{
		try {
			pDownLoadMission=new CDownLoadMission(this); 
		}
		catch (...) {
			return 0; 
		}
	}
	ASSERT(pDownLoadMission!=NULL);
	if (pDownLoadMission==NULL) 
	{
		return 0;
	}
	m_DownLoadMissionActive.Add(pDownLoadMission);

	//配置下载
	m_dwMissionID++;
	pDownLoadMission->StartDownLoad(m_dwMissionID,dwDownLoadType,pDownLoadRequest);
	return pDownLoadMission->GetMissionID();
}

//下载通知
void CDownLoadService::OnMissionFinish(enDownLoadStatus DownLoadStatus, CDownLoadMission * pDownLoadMission)
{
	//寻找下载
	int i=0;
	for (i=0;i<m_DownLoadMissionActive.GetCount();i++)
	{
		if (pDownLoadMission==m_DownLoadMissionActive[i]) break;
	}
	ASSERT(i!=m_DownLoadMissionActive.GetCount());
	if (i==m_DownLoadMissionActive.GetCount()) return;

	//清理下载
	m_DownLoadMissionActive.RemoveAt(i);
	m_DownLoadMissionRelease.Add(pDownLoadMission);

	return;
}

//查找下载
CDownLoadMission * CDownLoadService::SearchMission(tagDownLoadRequest * pDownLoadRequest)
{
	CDownLoadMission * pDownLoadMission=NULL;
	for (INT_PTR i=0;i<m_DownLoadMissionActive.GetCount();i++)
	{
		pDownLoadMission=m_DownLoadMissionActive[i];
		if (pDownLoadMission->CompareRequest(pDownLoadRequest)==true) return pDownLoadMission;
	}
	return NULL;
}

//////////////////////////////////////////////////////////////////////////

//建立对象函数
extern "C" __declspec(dllexport) void * __cdecl CreateDownLoadService(const GUID & Guid, DWORD dwInterfaceVer)
{
	//建立对象
	CDownLoadService * pDownLoadService=NULL;
	try
	{
		pDownLoadService=new CDownLoadService();
		if (pDownLoadService==NULL) throw TEXT("创建失败");
		void * pObject=pDownLoadService->QueryInterface(Guid,dwInterfaceVer);
		if (pObject==NULL) throw TEXT("接口查询失败");
		return pObject;
	}
	catch (...) {}

	//清理对象
	SafeDelete(pDownLoadService);
	return NULL;
}
void CDownLoadMission::OnDestroy()
{
	m_iDownTag = 0;
	CCDownloadManager::getInstance().Clear();

	CDialog::OnDestroy();
	// TODO: 在此处添加消息处理程序代码
}
