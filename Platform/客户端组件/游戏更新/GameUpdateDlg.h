// GameUpdateDlg.h : header file
//

#if !defined(_GAME_UPDATE_DLG_H_20111214_175900)
#define _GAME_UPDATE_DLG_H_20111214_175900

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CGameUpdateDlg dialog

#include "RDialog.h"
#include "afxcmn.h"
#include "afxwin.h"
#include "InetSession.h"
#include "InternetGetFile.h"
#include "SkinButton.h"
//#include "../../公共文件/ISegmentDownload.h"
//
//#if _DEBUG	
//#pragma comment(lib, "../../链接库/SegmentDownloadD.lib") 
//#else
//#pragma comment(lib, "../../链接库/SegmentDownload.lib") 
//#endif

// 线程参数结构
typedef struct tagThread
{
public:
	LPVOID wndPointer;								// 窗口指针
	BYTE byEventIndex;								// 停止事件索引
	CInternet Internet;								// 同步下载类
	//ISegmentDownload asynchronous;		// 异步下载类 
	bool bAsynchronousStart;						// 异步启动成功

	tagThread()
	{
		bAsynchronousStart = false;
		ZeroMemory(this, sizeof(*this));
	}

	tagThread(LPVOID lPointer, BYTE bIndex)
	{
		wndPointer = lPointer;
		byEventIndex = bIndex;
	}

	//bool InitDowndloadStatus(IDownloadEvent * pDownloadEvent)
	//{
	//	//// 下载配置
	//	//asynchronous.SetEverySegmentLen(400*1024);			// 设置每段的长度
	//	//asynchronous.SetOverTimeEverySegment(15*1000);		// 设置每一段的超时值
	//	//asynchronous.SetMaxDownCount(20);							// 设置每一段的最大的下载次数
	//	//asynchronous.SetTaskThreadNum(4);							// 设置工作线程的数量
	//	//// 注册，非常重要，一定要写
	//	//asynchronous.RegistEventModule(pDownloadEvent);	
	//	//// 启动
	//	//return bAsynchronousStart = asynchronous.Start();
	//}
}ThreadParameter;

typedef enum tagDownloadMode
{
	IE_MODE = 0,								// IE下载	
	BreakPoints_MODE = 1,				// IE支持断点续传
	UM_DLL_MODE =2						// UM库下载方式
}DMODE;
//class CUpdateThread;

#define  THREAD_MAX_SIZE						10					// 最大更新线程数目

class CGameUpdateDlg : public CDialog/*, public IDownloadEvent*/	// 下载的事件接口
{
// Construction
public:
	/// standard constructor
	CGameUpdateDlg(CWnd* pParent = NULL);
	virtual ~CGameUpdateDlg();
	/// 设置配置文件名
	BOOL SetConfigFile(CString& sFilename);
	// 用户消息
	LRESULT OnUserMessage(WPARAM wParam, LPARAM lParam);
	/// 调用外部程序 
	/// [param bMyself in] 调度自己
	BOOL TransferExternalProgram(BOOL bMyself=FALSE, string strUpdateName="");
	/// 更新下载提示控件
	void OnUpdateControlContent(LPCTSTR lpContent=NULL);
	void OnNotifyUpdateFinish(BOOL bSuccess = TRUE);
		
	// 更新函数
public:
	/// 检查文件Update.ini文件
	void CheckUpdateFile();
	/// 检查需要更新的单个文件
	void CheckSingleFileUpdate(void);
	/// 检查需要更新的目录
	void CheckDirectoryUpdate(void);
	/// 检查需要更新的所有文件及目录
	void CheckWantUpdateFile(void);
	/// 开始升级
	void StartUpdate();
	/// 更新所有文件
	void StartCopyFile();
	/// 检查进程
	void CheckProcess(LPCTSTR lpProcessName);
	/// 更新文件
	BOOL CopyFileUpdate(const string &strFile, const string &strTmpFile, const string &strPath) const;
	/// 安全复制
	BOOL OnSuccessCopyFile(const string &strFile, const string &strTmpFile, const string &strPath);
	/// 初始化下载数组
	void InitDowndloadArray();
	/// 线程函数
	static unsigned __stdcall ThreadWorkFun(LPVOID lParam);
	/// 下载文件
	/// [@param bSynchronous in] 为真同步
	/// [@param llFileIndex in] 下载文件时的索引，小于0时不统计，（另：异步时该参数无效）
	static BOOL DowndloadFile(ThreadParameter *pWnd, const TCHAR szDowndLoadPath[MAX_PATH], 
		const TCHAR szDowndLoadSavePath[MAX_PATH], long llFileIndex=-1, DMODE bSynchronous=IE_MODE);
	/// 校验文件
	static BOOL VerifyFile(ThreadParameter *pWnd, const TCHAR szDowndLoadSavePath[MAX_PATH], string strMd5Val);
	/// 线程设置文件下载状态
	static void ThreadSetFileStatus(ThreadParameter *pWnd, TCHAR tchFirstParam[MAX_PATH], BOOL bComplete, BOOL bDownloading);
	/// 下载回调
	void OnDownloadResult (int iCookie, char* pszLocalPathFile, int iRes, int iSuccessDownload, int iTotalDownload);
	/// 优先更新检查
	BOOL IsHavePriorityUpdate(string strFileName);
	/// 下载结果
	void DownResult(char* p, bool bIsSuc);
	enum { IDD = IDD_GameUpdate };

public:
	HBRUSH						m_HBrush;										///< 静态控件背景画刷句柄
	CSkinButton					m_btnCancel;									///< 取消
	CSkinButton					m_btnDown;									///< 下载整包按钮
	CRButton						m_btGenerateHash;						///< 生成MD5 HASH
	CString							m_sAppName;								///< 应用程序名
	CString							m_sConfigFilename;						///< 升级配置文件名
	BOOL							m_bSilenceMode;							///< 静默方式执行升级，不显示升级程序界面，只在升级完毕后提醒用户
	BOOL							m_bUserBreak;								///< 用户终止升级
	BOOL							m_bGenerateIniFile;						///< 生成升级配置文件
	BOOL							m_bIsUpdateMySelf;						///< 更新自己
	//set<string>				   m_setLeachName;							///< 过滤文件名及扩展名
	string						   m_strExternalProgramFileName;		///< 外部程序名
	string						   m_strAccount;									///< 外部账户名
	string						   m_strPW;										///< 外部密码

	CBitmap						m_bitmapBG;
	CBitmap						m_bitmapErrorBG;
	CBitmap						m_bitmapPercentSign;					///< 百分号
	CBitmap						m_ImageDownLoadNum;				///< 下载数字
	CBitmap						m_ImageProgressBG;						///< 进度条背景
	CBitmap						m_ImageProgressFrontGree;			///< 绿色进度条值
	CBitmap						m_imageProgressFrontRed;			///< 红色进度条值
	CBitmap						m_bTransfer;									///< 转盘
	CBitmap						m_bDownErrorWaring;					///< 下载错误

	int									m_btNums;									///< 转盘图片索引
	CString							m_strContent;									///< 下载进度内容
	long								m_lDownBaiBi;								///< 下载百分比
	CRect							m_rcDownDrawRect;						///< 下载绘制区域
	bool								m_bDownError;								///< 下载错误

	CInternet					  * m_pInternet;									///< 下载类
	TCHAR							m_chURL[128];								///< 备份的URL地址
	HICON							m_hIcon;
	
	// 需要更新的
public:
	map<string, FileHashTable>			m_setWantUpdateFile;											///< 需要更新的文件
	map<string, FileHashTable>			m_setDowndloadFile;											///< 已下载的文件
	vector<string>								m_setPriorityDowndload;										///< 优先下载文件
	long												m_lTmpFileCount;													///< 临时文件计数
	BOOL											m_bIsCopyfile;														///< 已复制文件

	HANDLE										m_hUpdateThread[THREAD_MAX_SIZE];				///< 更新线程
	HANDLE										m_hDownloadEvent[THREAD_MAX_SIZE][2];			///< 下载事件
	HANDLE										m_hStopEvent[THREAD_MAX_SIZE];						///< 停止事件
	HANDLE										m_hCompleteEvent[THREAD_MAX_SIZE];				///< 完成事件
	ThreadParameter							m_structThreadParameter[THREAD_MAX_SIZE];		///< 线程参数
	long												m_uThreadCount;													///< 线程数目
	DMODE										m_eResumeMode;												///< 下载方式(IE, 续传)

private:
	map<long, ULONGLONG>			m_mDowndloadCount;											///< 异步下载字节统计
	map<string, ULONGLONG>			m_strDowndloadCount;											///< 下载字节统计
	DOWNLOAD_INFO_STRU				m_TotalFileInfo;													///< 要下载的文件总数和大小总和
	DOWNLOAD_INFO_STRU				m_FinishedFileInfo;												///< 已下载的文件总数和大小总和
	float												m_fDownloadSpeed;												///< 下载速度

	CCriticalSpection							criticalWantUpdateFile;											///< 保护 m_setWantUpdateFile
	CCriticalSpection							criticalDowndloadInfo;											///< 保护 m_FinishedFileInfo

	//ISegmentDownload						m_stDownload[THREAD_MAX_SIZE];						///< 异步下载


	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGameUpdateDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	//}}AFX_VIRTUAL

public:
	// Generated message map functions
	//{{AFX_MSG(CGameUpdateDlg)	
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnBtnDown();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	virtual BOOL DestroyWindow();
};

extern class CGameUpdateDlg * g_pCGameUpdateDlg;
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GameUpdateDLG_H__4CA1BB40_75FF_4A00_AF96_F141507CE885__INCLUDED_)
