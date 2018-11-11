#ifndef CELL_SCORE_OPTION_DLG_HEAD_FILE
#define CELL_SCORE_OPTION_DLG_HEAD_FILE
#include "Stdafx.h"
#include "BulletHelpDlg.h"

#pragma once

class CDlgCellScoreOption : public CSkinDialogEx
{
	//变量定义
public:
	LONG							m_lCellScore[CELL_SCORE_COUNT];			//单元积分
	LONG							m_lCellScoreSelect;						//单元积分
	CWnd							* m_pCenterView;						//中心窗体

	//控件变量
private:
	CSkinButton						m_btOK;									//确定按钮
	CComboBox						m_ComBoxCellScore;						//单元积分

	//函数定义
public:
	//构造函数
	CDlgCellScoreOption(CWnd* pParent = NULL);
	//析构函数
	virtual ~CDlgCellScoreOption();
	
	//继承函数
protected:
	//控件绑定
	virtual void DoDataExchange(CDataExchange* pDX);
	//初始函数
	virtual BOOL OnInitDialog();

	//消息函数
private:
	//确定函数
	virtual void OnOK();

	//消息映射
	DECLARE_MESSAGE_MAP()
	DECLARE_DYNAMIC(CDlgCellScoreOption)	
};

#endif