/******************************************************************
Copyright (C),  2010-2025  
File name:		CCDownloadManager.h
Version:		1.0
Description:    下载接口

Author:			pengwen
Create Date:	2010-5-12

------------------------------------
2010-9-23       pengwen 创建
******************************************************************/



#pragma once
#ifdef  API_EXPORTS
#define API_ABC   __declspec(dllexport)   
#else
#define API_ABC   __declspec(dllimport)  
#endif

typedef  void  (*CallBackFinsh)(char* pUrl, bool isFinish, void* p);
class API_ABC CCDownloadManager
{
public:
	CCDownloadManager(void);
	~CCDownloadManager(void);
private:
	static CCDownloadManager s_dlm;

public:
	static CCDownloadManager& getInstance();
	static CCDownloadManager* getInstancePtr();
private:
	char*  m_pUrl;
	char*  m_pOutFolder;
	void*  m_pUserNeed; 
	CallBackFinsh m_pCallBackFinsh;

private:
	bool ReadDNP();
	int _CheckDownloadAlrExists();
public:	
	//下载完成，模块内部调用
	void  Finish(char* pUrl);

public:
	bool Init(const char* pUrl, const char* pOutFolder, CallBackFinsh pFt, void* pUserNeed = NULL);
	void Clear();
	//注意可能返回 0,一般实在文件开始下载的时候
	unsigned __int64  GetFileSize();
	//得到已经下载的文件大小, 如果下载的过程网络太慢或者出错，
	//可能导致返回的结果大于文件大小，注意处理这种情况
	unsigned __int64  GetDownLoadSize();
	//得到速度
	unsigned int GetSpeed();
	//下载
	void Start();
	//取消下载
	void Stop();
	//继续下载
	void GoOn();

};
 