#pragma once


// ISynchronyTask 子类的成员如何投送任务
class   __declspec(dllexport)  IDownloadEvent
{
public:
	/* --------------------------------------------------------------------------
	函数说明：获得下载结果，是一个回调函数
	传入参数：
		iCookie						DownWebFile 输入的是什么值，此处也返回此值，相当于唯一标识
		pszLocalPathFile		要下载到本地的路径及文件名
		iRes							// 下载状态
										DOWNLOAD_NOT_START	 0	// 还未开始
										DOWNLOAD_RUNING		   10	// 正在下载过程中
										DOWNLOAD_SUCCES			20	// 下载成功
										DOWNLOAD_FAIL				30	// 下载失败
										PROGRAM_OVER					40	// 程序结束了
		iSuccessDownload   已经下载成功的字节数量
		iTotalDownload			此文件总共需要下载的字节数量
	--------------------------------------------------------------------------*/
	virtual void OnDownloadResult (int iCookie, char* pszLocalPathFile, int iRes, int iSuccessDownload, int iTotalDownload) = 0;
	
};

class  __declspec(dllexport)  ISegmentDownload
{
public:
	ISegmentDownload();
	~ISegmentDownload();
	
	void RegistEventModule(IDownloadEvent* pDownloadEvent);

	// 启动，注意：最好不要在子线程中启动
	bool Start();	

	// 停止
	bool Stop();

	// 设置每段的长度
	void SetEverySegmentLen(int	iEverySegmentLen);

	// 获得每段的长度
	int GetEverySegmentLen();

	// 设置每一段的超时值
	void SetOverTimeEverySegment(DWORD dwOverTimeEverySegment);

	// 设置每一段的最大的下载次数
	void SetMaxDownCount(int iMaxDownCount);

	// 设置任务线程的数量
	void SetTaskThreadNum(int iTaskThreadNum);

	// 主机名转成点分 IP 地址
	void HostNameToIP(char* szHostName, char* szIP);

	// 测试
	void Test(int iMsgCode, void* pData, int iDataLen); 

	/* --------------------------------------------------------------------------
	函数说明：下载一个远程文件
	传入参数：
		iCookie				此处输入的是什么值，由 OnDownloadResult 反回的也是此值，相当于唯一标识
		szRemoteUrl		Web上的文件
		szLocalFile		要下载到本地的路径及文件名
		pszError			返回错误，一定不能为NULL，长度最好不低于128

	返回值：
		成功则返回 true，否则返回 false 。
	备注：
		成功后需要在 OnDownloadResult 函数中
	--------------------------------------------------------------------------*/
	bool DownWebFile(int iCookie,  char* szRemoteUrl, char* szLocalFile, char* pszError);

private:
	void*					m_pCDownload;
	IDownloadEvent* m_pDownloadEvent;
};

