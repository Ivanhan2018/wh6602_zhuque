// PlazaGameWeb.cpp : 实现文件
//

#include "stdafx.h"
#include "PlazaGameWeb.h"
#include ".\plazagameweb.h"
#include "webpath.h"
#include "GlobalUnits.h"
#include "InitData.h"

#define IDI_WEB_BROWSER_COMMON				1109291526					//网页浏览

#define TIMER_WEB_LOADING_GAMEWEB			1111031633					//加载网页
// CPlazaGameWeb 对话框

IMPLEMENT_DYNAMIC(CPlazaGameWeb, CDialog)
CPlazaGameWeb::CPlazaGameWeb(CWnd* pParent /*=NULL*/)
: CDialog(CPlazaGameWeb::IDD, pParent)
{
}

CPlazaGameWeb::~CPlazaGameWeb()
{
}

void CPlazaGameWeb::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPlazaGameWeb, CDialog)
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


// CPlazaGameWeb 消息处理程序

BOOL CPlazaGameWeb::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_WebBrowser.Create(NULL,NULL,WS_VISIBLE | WS_CHILD,CRect(0,0,0,0),this,IDI_WEB_BROWSER_COMMON,NULL);
	m_WebBrowser.MoveWindow(0, 0, 536, 360);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CPlazaGameWeb::OnPaint()
{
	CPaintDC dc(this); // device context for painting
}

//当前路径
void CPlazaGameWeb::GetCurrentDirectoryPath(CString &szPath)
{
	szPath="";
	TCHAR szPathTemp[MAX_PATH]={0};
	GetCurrentDirectory(MAX_PATH, szPathTemp);
	szPath.Append(szPathTemp);
}

//大厅资源路径
void CPlazaGameWeb::GetImagePlazaPath(CString& strRetValue)
{
	CString strModuleFileName("");
	GetCurrentDirectoryPath(strModuleFileName);
	strModuleFileName.Append("\\data\\plaza\\");
	strRetValue = strModuleFileName;
}
//资源路径
void CPlazaGameWeb::GetResPath(CString strClipName, CString& strPath)
{
	CString strModuleFileName("");
	GetCurrentDirectoryPath(strModuleFileName);
	strModuleFileName.Append(strClipName);
	strPath = strModuleFileName;
}

//创建大厅游戏网页
bool CPlazaGameWeb::LoadPlazaGameWeb(UINT uiType)
{
	//加载本地页面
	CString strUrlPath("");
	GetResPath("\\Url\\plaza\\",strUrlPath);
	m_WebBrowser.Navigate(TEXT(strUrlPath+"webBrowserCenter.html"),NULL,NULL,NULL,NULL);

	tagGlobalUserData UserData=g_GlobalUnits.GetGolbalUserData();
	CString strUrl("");
	switch (uiType)
	{
	case 1://帐号充值
		strUrl.Format(CInitData::s_cLoginPayment, UserData.dwUserID, UserData.szKey);
		break;
	case 2://奖品兑换
		strUrl.Format(CInitData::s_cLoginExchange, UserData.dwUserID, UserData.szKey);
		break;
	case 3://道具商城
		strUrl.Format(CInitData::s_cLoginPropShop, UserData.dwUserID, UserData.szKey);
		break;
	case 4://新手任务
		strUrl.Format(CInitData::s_cLoginTaskNew, UserData.dwUserID, UserData.szKey);
		break;
	case 5://背包
		strUrl.Format(CInitData::s_cLoginBackpack, UserData.dwUserID, UserData.szKey);
		break;
	case 6://用户信息
		strUrl.Format(CInitData::s_cLoginUserInfo, UserData.dwUserID, UserData.szKey);
		break;
	case 7://银行
		strUrl.Format(CInitData::s_cLoginBank, UserData.dwUserID, UserData.szKey);
		break;
	case 8://邮件
		strUrl.Format(CInitData::s_cLoginSysMessage, UserData.dwUserID, UserData.szKey);
		break;
	case 9://斗地主(游戏规则)
		strUrl.Format(CInitData::s_cGameHelpSingle, 10);
		break;
	case 10://长沙麻将(游戏规则)
		strUrl.Format(CInitData::s_cGameHelpSingle, 303);
		break;
	case 11://益阳麻将(游戏规则)
		strUrl.Format(CInitData::s_cGameHelpSingle, 310);
		break;
	default:
		return true;
	}
	m_WebBrowser.Navigate(strUrl,NULL,NULL,NULL,NULL);

	//m_WebBrowser.Navigate("file://C:\\test.html",NULL,NULL,NULL,NULL);
	return true;
}

BOOL CPlazaGameWeb::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	return TRUE;
	//return CDialog::OnEraseBkgnd(pDC);
}

BOOL CPlazaGameWeb::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if (pMsg->message==WM_KEYDOWN)
	{
		if (pMsg->wParam==VK_ESCAPE || pMsg->wParam==VK_RETURN)
		{
			return true;
		}
	}
	return CDialog::PreTranslateMessage(pMsg);
}
