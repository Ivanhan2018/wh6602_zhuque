#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include "RButton.h"

// CGenerate 生成MD5对话框

class CGenerate : public CRDialog
{
	DECLARE_DYNAMIC(CGenerate)

public:
	CGenerate(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CGenerate();
	virtual BOOL OnInitDialog();
	virtual void OnCancel();

// 对话框数据
	enum { IDD = IDD_GENERATE };
	CRichEditCtrl			m_ctrLeachfile;										///< 过滤&忽略的文件及扩展名
	CRButton				m_btGenerateHash;								///< 生成按钮
	TCHAR					m_chURL[128];										///< 备份的URL地址
	set<string>			m_setLeachName;									///< 过滤文件名及扩展名
	vector<string>		m_strLeachFileName;							///< 过滤文件名
	BOOL					m_bSilenceMode;									///< 静默方式执行升级，不显示升级程序界面，只在升级完毕后提醒用户
	BOOL					m_bStopGenerate;								///<  停止生成
	BOOL					m_bComplete;										///< 完成生成
	BOOL					m_bDowndloadServerSetFile;				///< 下载服务器设置文件生成
	HANDLE				m_hThread;											///< 线程句柄
	CString					m_sGenerateProgressMsg;					///< 生成提示进度信息

	// 生成MD5函数
public:
	/// 逐目录把所有文件生成MD5Hash
	BOOL GenerateMD5Hash();	
	/// 下载生成配置文件
	BOOL DowndloadGenerateDeployFile(const string strPath, const string strUpdatePath);
	/// 搜索指定目录下所有文件
	/// [vResult out] 返回所有的文件名
	/// [strFindPath in] 需要查找的目录
	/// [strDelPath in] 返回文件名时需要替换的目录
	void SearchCatalogueAllFile(multimap<string, FileHashTable> & vResult, string strFindPath, string strDelPath);	
	/// 设置不需要生成的文件名或扩展名
	/// [@param bLoadIni in] TRUE 读取INI过滤字符串 / FALSE 读取界面上的过滤字符串
	void SetLeachFileName(BOOL bLoadIni);
	/// 设置生成被过滤的文件
	void OnSetLeachFileName(string str);
	/// 设置安静模式
	void SetQuietMode(BOOL bMode) { m_bSilenceMode = bMode; }
	/// 线程生成函数
	static unsigned __stdcall ThreadGenerateWorkFun(LPVOID lParam);
	/// 下载文件
	static BOOL DowndloadFile(CGenerate *pWnd, const TCHAR szDowndLoadPath[MAX_PATH], 
		const TCHAR szDowndLoadSavePath[MAX_PATH], long llFileIndex=-1);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	// 生成按钮事件
	afx_msg void OnBnGenerateIniFile();
	afx_msg void OnPaint();
	// 用户消息
	LRESULT OnUserMessage(WPARAM wParam, LPARAM lParam);	
	virtual BOOL DestroyWindow();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
};
