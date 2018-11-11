#pragma once

// CGameBizCity 对话框
#include "Stdafx.h"
#include "Resource.h"
#include "GameFrame.h"
#include <afxcmn.h>
#include "GameFrameExmain.h"

//游戏商城
class CGameBizCity : public CDialog
{
	DECLARE_DYNAMIC(CGameBizCity)

public:
	CGameBizCity(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CGameBizCity();

// 对话框数据
	enum { IDD = IDD_GAME_BIZ_CITY_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

	//浏览器窗口
	CGameFrameExmain		*				m_exMain;
	//用户MD5密码
	char						m_chUserMD5PassWord[PASS_LEN];	
public:
	virtual BOOL OnInitDialog();

	DECLARE_DISPATCH_MAP()//声明dispatch map表
	DECLARE_EVENTSINK_MAP()
	BOOL onHtmlContextMenu(IHTMLEventObj *pEvtObj);
	BOOL onHtmlMouseDown(IHTMLEventObj *pEvtObj);
	void OnDocumentCompleteExplorer1(LPDISPATCH pDisp, VARIANT FAR* URL);
	//事件处理函数,原型可以参考MSDN中关于HTMLDocumentEvents2的说明
	DWORD m_dwCookie;
	//用于标记一个连接点
	IHTMLDocument2* pDoc2;
	//想要处理事件的COM对象的指针

public:
	//设置网页数据
	void SetGameBizCity(int iUserID, char* chUserData);
};
