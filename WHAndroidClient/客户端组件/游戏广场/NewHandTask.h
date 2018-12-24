#ifndef _NEW_HAND_TASK_H_2012_2_16_
#define  _NEW_HAND_TASK_H_2012_2_16_

#pragma once

// 新手任务

class CNewHandTask : public CRgnDialog
{
	DECLARE_DYNAMIC(CNewHandTask)

public:
	CNewHandTask(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CNewHandTask();

// 对话框数据
	enum { IDD = IDD_NEWHANDTASK };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();
	void GetImagePlazaPath(CString& strRetValue);
	afx_msg void OnPaint();
	afx_msg void OnbtnDailyTask();
	afx_msg void OnbtnChallengeTask();
	afx_msg void OnbtnNewHandTask();
	afx_msg void OnbtnClose();

private:
	CSkinButton								m_btDailyTask;								///< 每日任务
	CSkinButton								m_btChallengeTask;						///< 挑战任务
	CSkinButton								m_btNewHandTask;						///< 新手任务
	CSkinButton								m_btClose;									///< 关闭
	CWebBrowser							m_webBrower;								///< 浏览器

public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};

#endif