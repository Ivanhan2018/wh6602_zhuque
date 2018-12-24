#pragma once
#ifndef __THREAD_ANIMATION_H_HEAD_
#define __THREAD_ANIMATION_H_HEAD_

#include "SkinControls.h"
#include <process.h>
#include <vector>
using namespace std;
#include <gdiplus.h>
using namespace Gdiplus;
#pragma comment(lib, "gdiplus.lib")

class CThreadAnimationManager;

/// 消息类
class CMsgSonToParent
{
public:
	virtual int TranslateMsg(int iMainCode, int iAssCode, void* pData, int iDataLen) = 0;
};

/// 回调参数结构
struct tagCallBack
{
	UINT			uID;			// 消息ID
	WPARAM			wParam;			// 消息参数
	LPARAM			lParam;			// 消息参数

	// 结构有构造函数，定义的变量无需再初始化
	tagCallBack()
	{
		uID = 0;
		wParam = 0;
		lParam = 0;
	}

	tagCallBack(UINT uID, WPARAM wParam, LPARAM lParam)
	{
		this->uID = uID;
		this->wParam = wParam;
		this->lParam = lParam;
	}
};

/// 动画线程类
class SKIN_CONTROL_CLASS CThreadAnimation : public CDialog
{
	friend class CThreadAnimationManager;
	DECLARE_DYNAMIC(CThreadAnimation)

public:
	CThreadAnimation();
	virtual ~CThreadAnimation();
	/// [@param filename in] 动画资源路径
	BOOL Create(CWnd* pParentWnd, UINT nID, const WCHAR* filename);

	afx_msg void OnDestroy();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnEraseBkgnd(CDC *pDC);

public:
	enum	// modi by HouGuoJiang 2011-6-17
	{
		STATE_RUN=0,				// 允许动画
		STATE_STOP_NO_CLOSE_WND=1,	// 停止动画，不关闭窗口
		STATE_STOP_CLOSE_WND=2,		// 停止动画，关闭窗口，用于结束进程时候
		
	};

protected:

	DECLARE_MESSAGE_MAP()

public:
	/// 设置回调
	void SetMsgSonToParent(CMsgSonToParent* pMsgSonToParent, tagCallBack &callback);

	/// 线程函数
	static unsigned __stdcall ThreadWorkFuc(LPVOID lParam);

	/// 绘画线程
	//static unsigned __stdcall OnDraw(CThreadAnimation *p);
	unsigned OnDraw();	// 不能用 static , modi by HouGuoJiang 2011-6-17

	/// 装入动画资源
	HRESULT OnLoad(const WCHAR* filename);	
	/// 清理资源
	void OnClear();
	/// 开始动画线程
	unsigned int OnStart();

public:
	/// 播放动画
	/// [@param iAllFramesPos in] 所有帧位置
	/// [@param iAllFramesTime in] 所有帧停留时间
	/// [@param iFramesCount in] 帧总数目
	/// [@param bStartFadeinout in] 开始渐隐渐显
	/// [@param bEndFadeinout in] 结束渐隐渐显
	/// [@param iLoop in] 循环 0 表示无限循环，小于0不循环，大于0循环的次数
	/// [@param iLoopStartIndex in] 循环开始的图片索引
	void Play(POINT iAllFramesPos, int iAllFramesTime=30, int iFramesCount=1, 
		BOOL bStartFadeinout=FALSE, BOOL bEndFadeinout=FALSE, int iLoop=-1, int iLoopStartIndex=0);
	void Play(POINT iFramesPos[], int iFramesTime[], int iFramesCount=1
		, BOOL bStartFadeinout=FALSE, BOOL bEndFadeinout=FALSE, int iLoop=-1, int iLoopStartIndex=0);

	/// 停止动画（删除相关资源）
	/// [@param bMsgNoCallBack in] 为false 动画结束不进行消息回调
	void Stop(bool bMsgCallBack=false, int iRunState=STATE_STOP_NO_CLOSE_WND);

	/// 移动窗口位置(解决父窗口移动，动画没有跟随的问题)
	void MoveWindow(int x, int y, int nWidth, int nHeight, BOOL bRepaint);
	void MoveWindow(LPCRECT lpRect, BOOL bRepaint);


private:
	CWnd					  * m_pParent;								///< 父窗口指针
	CMsgSonToParent*			m_pMsgSonToParent;						///< 父窗口的专用回调指针
	UINT_PTR					m_uwaParam;								///< 消息参数
	tagCallBack					m_callback;								///< 回调参数

	unsigned int				m_dwThreadID;							///< 线程ID
	Image					  * m_pImage;								///< 动画背景
	BOOL						m_bStatFadeinout;						///< 开始渐隐渐显
	BOOL						m_bEndFadeinout;						///< 结束渐隐渐显
	int							m_iLoop;								///< 循环
	int							m_iLoopStartIndex;					///< 循环图片索引
	vector<int>					m_vFramesTime;							///< 帧停留时间
	vector<POINT>				m_vFramesPos;							///< 帧位置
	int							m_iFramesCount;							///< 帧数目
	int							m_iWidth, m_iHeight;					///< 宽，高
	int							m_uIndex;								///< 动画索引
	ULONG_PTR					m_gdiplus_Token;						///< GDI+资源	

	HANDLE						m_hThreadHandle;						///< 线程句柄
	HANDLE						m_hThreadEvent;							///< 结束绘画的事件

	BLENDFUNCTION				m_Blend;
	typedef BOOL (WINAPI *MYFUNC)(HWND,HDC,POINT*,SIZE*,HDC,POINT*,COLORREF,BLENDFUNCTION*,DWORD);          
	MYFUNC						UpdateLayeredWindow;

	volatile long				m_iRunState;	// 参考 STATE_STOP_NO_CLOSE_WND
	CString						m_strFileName;
protected:
	virtual void PostNcDestroy();
};
typedef vector<CThreadAnimation *> PThreadAnimation;

// 动画管理
class SKIN_CONTROL_CLASS CThreadAnimationManager//: public CMsgSonToParent
{
public:
	~CThreadAnimationManager();
	/// 初始化
	static void Init(CWnd * pParent);
	/// 动画结算回调
	//virtual int TranslateMsg(int iMainCode, int iAssCode, void* pData, int iDataLen);
	/// 删除所有动画资源
	static void FreeAll();
	/// 播放动画
	/// [filename in] 动画资源
	/// [iAllFramesPos in] 所有帧位置
	/// [iAllFramesTime in] 所有帧停留时间
	/// [iFramesCount in] 帧总数目
	/// [bStartFadeinout in] 开始渐隐渐显
	/// [bEndFadeinout in] 结束渐隐渐显
	/// [iLoop in] 循环 0 表示无限循环，小于0不循环，大于0循环的次数
	/// [iLoopStartIndex in] 循环开始的图片索引
	static bool Play(const WCHAR* filename, POINT iAllFramesPos, int iAllFramesTime=30, int iFramesCount=1, 
		BOOL bStartFadeinout=FALSE, BOOL bEndFadeinout=FALSE, int iLoop=-1, int iLoopStartIndex=0);
	static bool Play(const WCHAR* filename, POINT iFramesPos[], int iFramesTime[], int iFramesCount=1
		, BOOL bStartFadeinout=FALSE, BOOL bEndFadeinout=FALSE, int iLoop=-1, int iLoopStartIndex=0);

private:
	PThreadAnimation							  m_AllAnimation;
	CWnd										* m_pParent;
	static CThreadAnimationManager				* m_staAnimationManager;

private:
	CThreadAnimationManager();
	/// 获得管理类实例
	static CThreadAnimationManager * GetInstance();
};

#endif