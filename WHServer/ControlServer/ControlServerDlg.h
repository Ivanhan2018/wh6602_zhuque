#pragma once

#include "Stdafx.h"
#include "Resource.h"
#include "ControlService.h"

//////////////////////////////////////////////////////////////////////////

//配置对话框
class CSystemOptionDlg : public CDialog
{
	//构造函数
public:
	//构造程序
	CSystemOptionDlg();
	//析构函数
	virtual ~CSystemOptionDlg();

	//重载函数
protected:
	//初始化函数
	virtual BOOL OnInitDialog();
	//控件子类化
	virtual void DoDataExchange(CDataExchange * pDX);
	//确定函数
	virtual void OnOK();

	//消息映射
	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

//主对话框
class CControlServerDlg : public CDialog
{
	//组件变量
public:
	CControlService					m_ControlService;						//登录服务
	CRichEditTrace					m_RichEditTrace;					//事件记录

	//构造函数
public:
	//构造程序
	CControlServerDlg();
	//析构函数
	virtual ~CControlServerDlg();

	//重载函数
protected:
	//控件子类化
	virtual void DoDataExchange(CDataExchange * pDX);
	//初始化函数
	virtual BOOL OnInitDialog();
	//确定消息
	virtual void OnOK();
	//取消函数
	virtual void OnCancel();
	//消息解释
	virtual BOOL PreTranslateMessage(MSG * pMsg);

	//消息函数
protected:
	//启动服务
	afx_msg void OnBnClickedStartService();
	//停止服务
	afx_msg void OnBnClickedStopService();
	//配置服务
	afx_msg void OnBnClickedInitService();
	//发送命令
	afx_msg void OnBnClickedSendCommand();

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////
