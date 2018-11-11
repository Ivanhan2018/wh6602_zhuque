#pragma once

#include "Stdafx.h"
#include "GameFrame.h"
#include "Resource.h"

// CGameTaskDialog 对话框

class CGameTaskDialog : public CDialog
{
	DECLARE_DYNAMIC(CGameTaskDialog)

public:
	CGameTaskDialog(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CGameTaskDialog();

// 对话框数据
	enum { IDD = IDD_GAME_TASK_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
	virtual BOOL OnInitDialog();


private:
	CImage					m_TaskImageBuffer;					//缓冲位图
	CGDIPlus_Image			m_ImageTaskWndBack;					//窗口背景
	BYTE					m_bCurrentTaskType;					//当前任务类型.1:今日任务; 2:挑战任务; 3:寻宝任务, 0:没任务

	CSkinButton				m_BTTaskToDay;						//今日任务
	CSkinButton				m_BTTaskTJ;							//挑战任务
	CSkinButton				m_BTTaskFind;						//寻宝任务

	CRect					m_rcInvalidateRect;					//刷新区域
	WORD					m_wSortID;							//排序号码
	CPoint					m_ptWnd;							//窗口坐标

	//动画
	BYTE					m_byFlashTag;						//动画标识
	int						m_iCountTimer;						//计算时间
	bool					m_bIsPlayFlash;						//是否播放动画
	CGDIPlus_Image			m_ImageFlash;

	//任务数据
private:
	CMD_GF_TASK_tagTask				m_tagTaskAcceptInfo;				//已接受的任务
	CMD_GF_TASK_tagTaskFinish		m_tagTaskFinishInfo;				//单个任务完成



public:
	//获得任务类型
	BYTE GetCurrentTaskType(void) { return m_bCurrentTaskType; }
	afx_msg void OnToDayTaskMsg();
	afx_msg void OnTJTaskMsg();
	afx_msg void OnFindTaskMsg();
	//绘制已接任务界面
	void DrawTaskAcceptUI(CDC *pDC);
	//绘制任务完成界面
	void DrawTaskEndUI(CDC *pDC);
	// 绘制背景
	void DrawBackground(CDC *pDC, int nWidth, int nHeight,int iExcursionY=0);

	//任务信息
public:
	//已接新任务信息
	void SetTaskAcceptInfo(const CMD_GF_TASK_tagTask* pTaskInfo);
	//单个任务完成
	void SetTaskOnceFinish(const CMD_GF_TASK_tagTaskFinish* pTaskFinish);
	//设置房间标识
	void SetSortID(WORD wSortID);

	//记录任务的进度
	void SetTaskCount(DWORD wTaskID, DWORD iTaskCount, DWORD iTaskTotal);
	//任务完成
	void SetTaskEnd(DWORD wTaskID);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnWindowPosChanging(WINDOWPOS* lpwndpos);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	BOOL SetWindowPos(const CWnd* pWndInsertAfter, int x, int y,
		int cx, int cy, UINT nFlags);
};
