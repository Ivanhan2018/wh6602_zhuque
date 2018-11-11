//------------------------------------------------------------------------------
// 文件名称：UpdateThread.h
// 文件版本：v1.0
// 创建日期：2006-05-04 14:25
// 作    者：Richard Ree
// 文件描述：自动升级线程类
//------------------------------------------------------------------------------

#if !defined(AFX_UPDATETHREAD_H__194A514F_A0D7_4ADD_8D2A_9E7081810D82__INCLUDED_)
#define AFX_UPDATETHREAD_H__194A514F_A0D7_4ADD_8D2A_9E7081810D82__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// UpdateThread.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CUpdateThread thread

class CUpdateThread : public CWinThread
{
	DECLARE_DYNCREATE(CUpdateThread)
protected:

// Attributes
public:
	CString											m_sConfigFilename;						// 升级配置文件名
	BOOL											m_bSilenceMode;							// 静默方式执行升级，不显示升级程序界面，只在升级完毕后提醒用户
	BOOL											m_bUserBreak;								// 用户终止升级
	HWND											m_hProgressWindow;						// 显示升级进度的窗口句柄
	map<string, FileHashTable>			m_setWantUpdateFile;					// 需要更新的文件
	map<string, FileHashTable>			m_setDowndloadFile;						// 已下载的文件
	ULONGLONG									m_CompleteDownByte;					// 完成下载的字节
	ThreadRunStartOperator				m_StartOperator;							// 线程开始运行时的操作
	
// Operations
public:
	/// protected constructor used by dynamic creation
	CUpdateThread();
	/// 执行升级
	BOOL DoUpdateEx();								
	/// 直接下载
	static BOOL DownFile(const char* sURL, const char* sSaveFilename, const char* sHeader, HWND hProgressWindow);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CUpdateThread)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	virtual int Run();
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CUpdateThread();

	// Generated message map functions
	//{{AFX_MSG(CUpdateThread)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

private:
	//下载文件
	// [@param strFile in] 文件名
	// [@param strTmpFile in] 临时补充的文件名
	// [@param strPath in] 文件相对路径
	BOOL DownloadFile(const string &strFile, const string &strTmpFile, const string &strPath)  const; //下载文件
	// 校验文件
	// [@param strFile in] 文件名
	// [@param strTmpFile in] 临时补充的文件名
	// [@param strHash in] 文件HASH值
	BOOL VerifyFile(const string &strFile, const string &strTmpFile, const string &strHash) const; 
	// 更新文件
	// [@param strFile in] 文件名
	// [@param strTmpFile in] 临时补充的文件名
	// [@param strPath in] 相对路径
	BOOL UpdateFile(const string &strFile, const string &strTmpFile, const string &strPath) const;
	// 检查需要更新的单个文件
	void CheckSingleFileUpdate(void);
	// 检查需要更新的目录
	void CheckDirectoryUpdate(void);
	// 检查需要更新的文件
	void CheckWantUpdateFile(void);
	// 逐目录把所有文件生成MD5Hash
	// [@param bShush in] 是否安静模式，即不在游戏界面显示生成相关信息
	BOOL DoGenerateMD5Hash(BOOL bShush=FALSE);
	// 设置界面进度条
	// [@param bShush TRUE 生成更新 FALSE 只生成
	// [@param iPos in] 设置进度条位置
	void SetProgressPos(BOOL bShush, int iPos);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_UPDATETHREAD_H__194A514F_A0D7_4ADD_8D2A_9E7081810D82__INCLUDED_)
