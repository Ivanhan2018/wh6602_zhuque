// InternetGetFile.h: interface for the InternetGetFile namespace.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#if !defined(INTERNETGETFILE_H)
#define INTERNETGETFILE_H

#define WM_USER_MSG							WM_USER+2000

// 上报到界面的升级进度消息
enum
{
	NOTIFY_DOWNLOAD_INFO = 1001,		// 通知要下载的文件状况
	NOTIFY_DOWNLOADED_INFO,				// 通知已下载的文件状况
	NOTIFY_DOWNLOAD_PROGRESS,			// 通知下载单个文件进度
	NOTIFY_DOWNLOADING_FILENAME,		// 通知正在下载的文件名
	NOTIFY_DOWNLOAD_FILE_FAIL,			// 通知下载文件失败
	NOTIFY_VERIFY_FILE_FAIL,			// 通知校验文件失败
	NOTIFY_UPDATE_FILE_FAIL,			// 通知更新文件失败
    NOTIFY_FINISH_UPDATE,				// 通知升级完毕消息
	NOTIFY_MESSAGE,						// 通知界面提示消息
	NOTIFY_GENERATE_MESSAGE,			// 通知生成提示消息
	NOTIFY_GENERATE_FILE_NAME,			// 通知生成文件列表
	NOTIFY_LEACH_FILE_NAME,				// 通知过滤的文件名
};

// 升级下载文件状态消息结构
struct DOWNLOAD_INFO_STRU
{
	UINT iFileCount;					// 合计文件数
	ULONGLONG iFileSize;				// 合计字节数

	DOWNLOAD_INFO_STRU()
	{
		ZeroMemory(this, sizeof(*this));
	}
};

// 下载单个文件进度消息结构
struct DOWNLOAD_PROGRESS_STRU
{
	long uIndex;									// 文件索引，方便多线程统计
	UINT iCurrentFileSize;					// 当前正在下载的文件的字节数
	UINT iCurrentFileFinishedSize;		// 当前文件已下载字节数
	DWORD dwDownloadUseTime;		// 当前文件下载用时

	DOWNLOAD_PROGRESS_STRU()
	{
		ZeroMemory(this, sizeof(*this));
	}
};

class CInternet
{
public:
	enum
	{
		INTERNET_SUCCESS = 0,
		INTERNET_ERROR_OPENURL,
		INTERNET_ERROR_FILEOPEN,
		INTERNET_ERROR_READFILE,
		INTERNET_ERROR_OPEN
	};

	BOOL bInternetGetURLUserBreak;	// 中断下载标记，置为真则中断下载文件


	CInternet();
	virtual ~CInternet();

	/// 通过HTTP协议从指定网址下载文件并保存到磁盘
	/// sURL：网址
	/// sSaveFilename：要保存的本地文件全路径
	/// sHeader：HTTP请求头信息
	/// hProgressWindow：接收下载文件进度消息并显示的窗口句柄
	/// llFileIndex 文件索引(用于多线程统计下载字节数)
	int InternetGetURL(const char* sURL, const char* sSaveFilename, const char* sHeader = NULL, HWND hProgressWindow = NULL, long llFileIndex=-1);
	/// 通过HTTP协议从指定网址获取网页内容
	/// sURL：网址
	/// [@return] 返回获取的网页内容
	static CString InternetGetInfo(CString szUrl);
};
#endif // !defined(INTERNETGETFILE_H)
