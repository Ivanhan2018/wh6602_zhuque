#ifndef DOWN_LOAD_SERVICE_HEAD_FILE
#define DOWN_LOAD_SERVICE_HEAD_FILE

#pragma once

#include "AfxInet.h"
#include "Resource.h"
#include "DownLoad.h"
#include <string>
#include "CDownloadManager.h"
#include "DownLoadServiceInterface.h"


using namespace std;

//////////////////////////////////////////////////////////////////////////
//枚举定义


//错误枚举
enum enDownLoadResult
{
	enDownLoadResult_Noknow,			//没有错误
	enDownLoadResult_Exception,			//异常错误
	enDownLoadResult_CreateFileFails,	//创建失败
	enDownLoadResult_InternetReadError,	//读取错误
};

//下载状态
struct tagDownLoadStatus
{
	WORD								wProgress;						//下载进度
	TCHAR								szStatus[128];					//状态描述
	enDownLoadStatus					DownLoadStatus;					//下载状态
};

//////////////////////////////////////////////////////////////////////////

//下载线程
class CDownLoadThread : public CServiceThread
{
	//变量定义
protected:
	bool							m_bPreparative;						//初始标志
	tagDownLoadRequest				* m_pDownLoadRequest;				//下载请求

	//信息变量
protected:
	TCHAR							m_szTempFile[MAX_PATH];				//临时文件
	TCHAR							m_szLocalFile[MAX_PATH];			//下载文件

	//状态变量
protected:
	CCriticalSection				m_CriticalSection;					//线程锁定
	enDownLoadStatus				m_DownLoadStatus;					//下载状态
	enDownLoadResult				m_DownLoadResult;					//结果状态
	DWORD							m_dwDownLoadSize;					//下载大小
	DWORD							m_dwTotalFileSize;					//文件大小

	//线程变量
protected:
	CFile							m_LocalFile;						//本地文件
	CHttpFile						* m_pHttpFile;						//下载文件
	CHttpConnection					* m_pHttpConnection;				//网络连接
	CInternetSession				m_InternetSession;					//网络会话

	//函数定义
public:
	//构造函数
	CDownLoadThread();
	//析构函数
	virtual ~CDownLoadThread();

	//功能函数
public:
	//初始化线程
	bool InitThread(tagDownLoadRequest * pDownLoadRequest);
	//下载状态
	void GetDownLoadStatus(tagDownLoadStatus & DownLoadStatus);
	//目标文件
	LPCTSTR GetDownLoadFileName();

	//重载函数
private:
	//运行函数
	virtual bool OnEventThreadRun();
	//关闭事件
	virtual bool OnEventThreadConclude();

	//线程函数
private:
	//下载准备
	void DownLoadPreparative();
	//下载清理
	void DownLoadCleanUp();
};

//////////////////////////////////////////////////////////////////////////


//下载任务
class CDownLoadMission : public CDialog
{
	//变量定义
protected:
	DWORD								m_dwMissionID;					//任务标识
	DWORD								m_dwDownLoadType;				//下载类型
	tagDownLoadRequest					m_DownLoadRequest;				//下载请求
	IDownLoadMissionSink				* m_pIDownLoadMissionSink;		//回调接口

	//控件变量
protected:
	CSkinButton							m_btCloseBT;						//关闭按钮
	CSkinButton							m_btHitHereBT;						//点击这里按钮

	CPngImage							m_ImageDownLoadFlash;				//下载动画
	CPngImage							m_ImageDownLoadNum;					//下载数字
	CPngImage							m_ImageFailFlash;					//下载失败动画
	CPngImage							m_ImageImageBack;					//进度条背景
	CPngImage							m_ImageImageFrontGree;				//进度条前景(绿)
	CPngImage							m_ImageImageFrontRed;				//进度条前景(红)
	CPngImage							m_ImageImageBai;					//%分号
	CPngImage							m_ImageDownBack;					//下载背景
	CPngImage							m_ImageUpdateBack;					//更新背景
	CPngImage							m_ImageDownFail;					//下载失败背景

private:
	//下载状态
	int									m_iDownTag;						//0:没有下载;1:正在下载;2:下载完成;3:下载失败
	BYTE								m_byDownLoadTag;				//下载标识.1:下载;2:更新;3:下载失败

	//URL
	std::string							m_strNetURL;					//外网URL
	//本地下载路径
	std::string							m_strLocalFilePath;				//本地文件路径


private:
	int									m_iDownBaiBi;					//下载百分比值
	int									m_iSaveDownBaiBi;				//保存

	//动画
	int									m_iCountDownSuc;				//计数下载过程
	int									m_iCountDownFail;				//计数下载失败

private:
	CDC									m_memdc;						//内存DC

	CRect								m_rcFlash;						//动画
	CRect								m_rcNums;						//数字
	CRect								m_rcProcess;					//进度条

	//函数定义
public:
	//构造函数
	CDownLoadMission(IDownLoadMissionSink * pIDownLoadMissionSink);
	//析构函数
	virtual ~CDownLoadMission();

	//重载函数
protected:
	//控件绑定
	virtual void DoDataExchange(CDataExchange * pDX);
	//初始化函数
	virtual BOOL OnInitDialog();
	//消息过虑
	virtual BOOL PreTranslateMessage(MSG * pMsg);
	//确定函数
	virtual void OnOK() { return; }
	//取消消息
	virtual void OnCancel();

	//功能函数
public:
	//任务标识
	DWORD GetMissionID() { return m_dwMissionID; }
	//下载类型
	DWORD GetDownLoadType() { return m_dwDownLoadType; }
	//开始下载
	bool StartDownLoad(DWORD dwMissionID, DWORD dwDownLoadType, tagDownLoadRequest * pDownLoadRequest);
	//停止下载
	bool StopDownLoad();
	//对比请求
	bool CompareRequest(tagDownLoadRequest * pDownLoadRequest);
	//显示界面
	bool DisplayDownLoadFace(bool bDisplay);
	//下载结果
	void DownResult(bool bIsSuc);

	//消息函数
protected:
	//重试按钮
	afx_msg void OnBnClickedReTry();
	//点击这里
	afx_msg void OnBnClickedHere();
	//定时器消息
	afx_msg void OnTimer(UINT_PTR nIDEvent);

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnDestroy();
};


#endif