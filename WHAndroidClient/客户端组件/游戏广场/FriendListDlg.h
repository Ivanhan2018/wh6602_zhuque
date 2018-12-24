#pragma once

#include "StdAfx.h"
#include "Resource.h"
#include "Afxcmn.h"
#include "afxwin.h"
#include "UserListView.h"
// CFriendListDlg 对话框



class CFriendListDlg : public CSkinDialog
{
	DECLARE_DYNAMIC(CFriendListDlg)

public:
	CFriendListDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CFriendListDlg();

// 对话框数据
	enum { IDD = IDD_FRIEND_LIST };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	// 好友
	CButton m_btnFriend;
	// 陌生人
	CButton m_btnStranger;
	// 黑名单
	CButton m_btnBlackList;
	// 队伍
	CButton m_btnTeames;
	// 好友列表
	CUserListView m_listFriend;
	// 陌生人列表
	CUserListView m_listStranger;
	// 黑名单列表
	CUserListView m_listBlackList;
	// 队伍列表
	CUserListView m_listTeames;
	// 输入
	CEdit m_editInput;
	// 添加
	CButton m_btnAdd;
	// 删除
	CButton m_btnDel;
	// 允许其他玩家加我为好友
	CButton m_btnAllowAddMy;
	// 被好友自动添加
	CButton m_btnAutoMatismAddMy;
	// 关闭
	CButton m_btnCancel;

	CUserListView * m_list;


	CSkinImage m_imgback;

	enum LIST_TYPE{
		LT_MY_FRIEND=0,
		LT_STRANGER,
		LT_BLACK_LIST,
		LT_TEAM
	};
	LIST_TYPE m_type;
protected:
	//更新列表页面
	void UpdateListTab();
public:
	virtual BOOL OnInitDialog();
	//好友
	afx_msg void OnBnClickedBtnFriend();
	//陌生人
	afx_msg void OnBnClickedBtnStranger();
	//黑名单
	afx_msg void OnBnClickedBtnTeames();
	//队伍
	afx_msg void OnBnClickedBtnBlacklist();
	afx_msg void OnPaint();
protected:
	virtual void OnCancel();
public:
	afx_msg void OnEnSetfocusName();
protected:
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual void OnOK();
};
