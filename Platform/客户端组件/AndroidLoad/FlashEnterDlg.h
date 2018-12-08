// FlashEnterDlg.h : 头文件
//

#pragma once
#include "resource.h"
#include "GameLogon.h"
#include "GameServer.h"
#include "afxwin.h"
#include "afxcmn.h"
//#include "xmlfile.h"
#include "xmlparse.h"

// CFlashEnterDlg 对话框
class CFlashEnterDlg : public CDialog
{
// 构造
public:
	CFlashEnterDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_FLASHENTER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

public:
	//更新登录信息
	void UpdateLogonInfo();
	void UpdateGameInfo();
	void UpdateQueueInfo();
	void UpdateLogonConnInfo();
	void UpdateGameConnInfo();
	//设置游戏配置信息
	bool SetGameOptionInfo();
	//插入列表数据
	void InsertListData(WORD dwSocketID, int dwErrorID, char* pErrorData);
	//清空所有数据
	void ClearAllListData();
	//关闭工具
	void OnCloseTool();

private:
	//登录服务器窗口
	CGameLogon		*m_pGameLogon;
	//游戏服务器窗口
	CGameServer		*m_pGameServer;


public:
	afx_msg void OnBnClickedBtLogonConnect();
	afx_msg void OnBnClickedBtLogonExitServer();
	afx_msg void OnBnClickedBtGameConnect();
	afx_msg void OnBnClickedBtExitGameServer();
	afx_msg void OnBnClickedBtAutoUserQueue();
	afx_msg void OnBnClickedBtAllUserExitGame();


private:
	CEdit m_UserID;
	CEdit m_UserPassWord;
	CIPAddressCtrl m_ServerIP;
	CEdit m_GameID;
	CEdit m_RoomID;
	CEdit m_TotalUserNums;
	CEdit m_ServerPort;
	CEdit m_UserIndex;
	CEdit m_GameDLL;
	CListCtrl m_ErrorList;
	DWORD	m_dwListCountItem;
	CButton m_ConnectLogonServer;
	CButton m_ResetUserData;
	CButton m_ConnectGameServer;
	CButton m_ExitGameServer;
	CButton m_AutoQueue;
	CButton m_AllExitGame;
	CIPAddressCtrl m_GameServerIP;//Author<cxf>
	CEdit m_GameServerPort;//Author<cxf>


private:
	//CXmlFile*	m_XmlFile;
	CXMLParse xmlParse;
};
