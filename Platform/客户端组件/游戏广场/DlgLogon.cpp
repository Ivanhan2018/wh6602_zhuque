#include "Stdafx.h"
#include "Resource.h"
#include "DlgLogon.h"
#include "GlobalUnits.h"
#include "GamePlaza.h"
#include ".\dlglogon.h"
#include "webpath.h"
#include <fstream>
#include <string>
#include "InitData.h"

//登录方式
#define LOGON_BY_ACCOUNTS				0						//帐号登录
#define LOGON_BY_USERID					1						//ID 登录
#define	LOGONADVERTISEMENT				119211855				//登录界面网页广告
#define LOGONNOTICE						119211918				//登录公告

//颜色定义
#define SELECT_COLOR		RGB(37,56,220)						//选择颜色

//屏幕限制
#define LESS_SCREEN_W					1024					//最小宽度
#define LESS_SCREEN_H					768						//最小高度
#define TIMER_WEB_LOADING				1111031344				//加载网页

#define TIME_ID_UPDATEPROGRAM_LOGON		100						// 更新程序触发自动登陆

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CTabCtrlLogon, CTabCtrl)
	ON_WM_PAINT()	
	//	ON_WM_WINDOWPOSCHANGING()
END_MESSAGE_MAP()

BEGIN_MESSAGE_MAP(CDlgRegister, CSkinDialogEx)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_REGISTER_USER_SEX_MAN, OnEnMakeUserSexMan)
	ON_BN_CLICKED(IDC_REGISTER_USER_SEX_WOMAN, OnEnMakeUserSexWoman)
	ON_BN_CLICKED(IDC_REGISTER_USER_DOCU, OnBnClickedUserDocu)
	ON_EN_SETFOCUS(IDC_ACCOUNTS, OnEnSetfocusAccouts)
	ON_EN_SETFOCUS(IDC_PASSWORD, OnEnSetfocusPassword)
	ON_EN_SETFOCUS(IDC_PASSWORD2, OnEnSetfocusPassword2)
	ON_EN_SETFOCUS(IDC_MAIL, OnEnSetfocusMail)
	ON_EN_SETFOCUS(IDC_MAIL2, OnEnSetfocusMail2)
	ON_EN_SETFOCUS(IDC_SPREADER, OnEnSetfocusSpreader)
END_MESSAGE_MAP()

BEGIN_MESSAGE_MAP(CDlgLogon, CSkinDialogEx)
	ON_BN_CLICKED(IDC_BT_LOGON_EXIT,OnBnClickedOnExit)
	ON_EN_CHANGE(IDC_PASSWORD, OnEnChangePassword)
	ON_BN_CLICKED(IDC_REGISTER, OnRegisterAccounts)
	ON_BN_CLICKED(IDC_DELETE_RECORD, OnDeleteAccounts)
	ON_BN_CLICKED(IDC_NET_OPTION, OnBnClickedNetOption)
	ON_BN_CLICKED(IDC_PROXY_TEST, OnBnClickedProxyTest)
	ON_BN_CLICKED(IDC_BUTTON_GETBACK_PASSWORD,OnBnClickedGetBackPassword)
	ON_BN_CLICKED(IDC_BUTTON_VISIT_WEB,OnBnClickedVisitWeb)
	ON_BN_CLICKED(IDC_GAME_WEB,OnBnClickedGameWeb)
	ON_BN_CLICKED(IDC_ACCOUNTS_MANAGE,OnBnClickedAccountsManage)
	ON_BN_CLICKED(IDC_CHECK_PASSWORD,OnBnClickCheckPassword)
	ON_BN_CLICKED(IDC_BUTTON_GAME_HELP,OnBnClickGameHelp)
	ON_BN_CLICKED(IDC_BUTTON_GAME_SHOP,OnBnClickGameShop)
	ON_BN_CLICKED(IDC_BUTTON_GAME_CLIENT,OnBnClickGameClient)
	ON_BN_CLICKED(IDC_BUTTON_KEY,OnBnClickKey)
	ON_BN_CLICKED(IDC_BT_TELECOM,OnEnTelecomMsg)
	ON_BN_CLICKED(IDC_BT_MESHWORK,OnEnMeshWorkMsg)
	ON_BN_CLICKED(IDC_BT_AUTO, OnEnAutoMsg)
	ON_WM_PAINT()
	ON_WM_NCPAINT() 
	ON_WM_LBUTTONDOWN()
	ON_WM_DESTROY()
	ON_MESSAGE(KEY_DOWN, OnKeyDownDlg)
	ON_WM_TIMER()
	//	ON_WM_MOVING()
	//	ON_WM_MOVE()
	ON_WM_MOVE()
	ON_WM_SIZE()
	ON_WM_WINDOWPOSCHANGING()
	ON_WM_SYSCOMMAND()
	ON_WM_WINDOWPOSCHANGED()
	ON_MESSAGE(1124,OnExitMessage)
	ON_MESSAGE(1125,OnCallInnoIssMsgSelectGame)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////



//构造函数
CTabCtrlLogon::CTabCtrlLogon()
{
}

//析够函数
CTabCtrlLogon::~CTabCtrlLogon()
{
}

//重画消息
void CTabCtrlLogon::OnPaint()
{
	//变量定义
	CPaintDC dc(this);
	CSkinWndAttribute * pSkinAttribute=CSkinDialog::GetSkinAttribute();

	//绘画背景
	CRect ClipRect,ClientRect;
	dc.GetClipBox(&ClipRect);
	dc.FillSolidRect(&ClipRect,pSkinAttribute->m_crBackGround);

	//设置 DC
	dc.SetBkMode(TRANSPARENT);
	dc.SelectObject(pSkinAttribute->GetDefaultFont());

	//绘画边框
	GetClientRect(&ClientRect);
	ClientRect.top+=18;
	dc.Draw3dRect(&ClientRect,pSkinAttribute->m_crInsideBorder,pSkinAttribute->m_crInsideBorder);

	//获取信息
	TCITEM ItemInfo;
	TCHAR szBuffer[100];
	memset(&ItemInfo,0,sizeof(ItemInfo));
	ItemInfo.mask=TCIF_TEXT|TCIF_IMAGE;
	ItemInfo.pszText=szBuffer;
	ItemInfo.cchTextMax=sizeof(szBuffer);

	//绘画按钮
	CRect rcItem;
	int iCursel=GetCurSel();
	CPen LinePen(PS_SOLID,1,pSkinAttribute->m_crInsideBorder);
	CPen * pOldPen=dc.SelectObject(&LinePen);
	for (int i=0;i<GetItemCount();i++)
	{
		GetItem(i,&ItemInfo);
		GetItemRect(i,&rcItem);
		dc.FillSolidRect(&rcItem,pSkinAttribute->m_crBackGround);
		dc.MoveTo(rcItem.left,rcItem.bottom-1);
		dc.LineTo(rcItem.left,rcItem.top+2);
		dc.LineTo(rcItem.left+2,rcItem.top);
		dc.LineTo(rcItem.right-2,rcItem.top);
		dc.LineTo(rcItem.right,rcItem.top+2);
		dc.LineTo(rcItem.right,rcItem.bottom-1);

		if (iCursel!=i)
		{
			rcItem.top+=2;
			dc.MoveTo(rcItem.left,rcItem.bottom-1);
			dc.LineTo(rcItem.right,rcItem.bottom-1);
			dc.SetTextColor(pSkinAttribute->m_crControlTXColor);
			DrawText(dc,szBuffer,lstrlen(szBuffer),&rcItem,DT_VCENTER|DT_SINGLELINE|DT_CENTER);
		}
		else 
		{
			rcItem.top+=3;
			dc.SetTextColor(SELECT_COLOR);
			DrawText(dc,szBuffer,lstrlen(szBuffer),&rcItem,DT_VCENTER|DT_SINGLELINE|DT_CENTER);
		}
	}

	//清理资源
	dc.SelectObject(pOldPen);
	LinePen.DeleteObject();

	return;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

//构造函数
CDlgRegister::CDlgRegister() : CSkinDialogEx(IDD_REGISTER)
{
	//登录信息
	m_wFaceID=0;
	m_cbGender=0;
	m_szSpreader[0]=0;
	m_szAccounts[0]=0;
	m_szPassword[0]=0;
	m_szMail[0]=0;
	m_szMailSuffix[0]=0;
	return;
}

//析构函数
CDlgRegister::~CDlgRegister()
{
}

//控件绑定
void CDlgRegister::DoDataExchange(CDataExchange * pDX)
{
	__super::DoDataExchange(pDX);
	DDX_Control(pDX, IDOK, m_btLogon);
	DDX_Control(pDX, IDCANCEL, m_btCancel);
	DDX_Control(pDX, IDC_USER_FACE, m_FaceSelect);
	DDX_Control(pDX, IDC_REG_WEB, m_LineRegWeb);
	DDX_Control(pDX, IDC_MAIN_PAGE, m_LineMainPage);
	DDX_Control(pDX, IDC_PASSWORD_PROTECT, m_LinePassWord);
	DDX_Control(pDX, IDC_REGISTER_USER_DOCU, m_btUserDocu);
	DDX_Control(pDX, IDC_REGISTER_USER_SEX_MAN, m_btUserSexMan);
	DDX_Control(pDX, IDC_REGISTER_USER_SEX_WOMAN, m_btUserSexWoman);



}

//初始化函数
BOOL CDlgRegister::OnInitDialog()
{
	__super::OnInitDialog();

	//设置标题
	SetWindowText(TEXT("帐号注册"));

	//限制输入
	((CEdit *)(GetDlgItem(IDC_PASSWORD)))->LimitText(PASS_LEN-1);
	((CEdit *)(GetDlgItem(IDC_PASSWORD2)))->LimitText(PASS_LEN-1);
	m_LineRegWeb.SetHyperLinkUrl(TEXT("http://qp.foxuc.net"));
	m_LineMainPage.SetHyperLinkUrl(TEXT("http://qp.foxuc.net"));
	m_LinePassWord.SetHyperLinkUrl(TEXT("http://qp.foxuc.net"));

	//加载头像
	g_GlobalUnits.m_UserFaceRes->FillImageList(m_ImageList);
	m_FaceSelect.SetImageList(&m_ImageList);

	//插入头像
	COMBOBOXEXITEM Item;
	ZeroMemory(&Item,sizeof(Item));
	Item.mask=CBEIF_IMAGE|CBEIF_SELECTEDIMAGE;
	for (int i=g_GlobalUnits.m_UserFaceRes->GetFaceCount()-1;i>=0;i--) 
	{
		Item.lParam=i;
		Item.iImage=i;
		Item.iSelectedImage=i;
		m_FaceSelect.InsertItem(&Item);
	}

	/*
	//用户性别
	BYTE cbGendexr[]={/ *GENDER_NULL,* /GENDER_BOY,GENDER_GIRL};
	LPCTSTR pszGender[]={/ *TEXT("保密"),* /TEXT("男性"),TEXT("女性")};
	CComboBox * pComboBox=(CComboBox *)GetDlgItem(IDC_GENDER);

	//用户性别
	for (int i=0;i<CountArray(cbGender);i++)
	{
	int nItem=pComboBox->InsertString(i,pszGender[i]);
	pComboBox->SetItemData(nItem,cbGender[i]);
	}
	pComboBox->SetCurSel(0);
	*/


	//设置头像
	m_wFaceID=rand()%g_GlobalUnits.m_UserFaceRes->GetFaceCount();
	m_FaceSelect.SetCurSel(m_wFaceID);

	//获取目录
	TCHAR szPath[MAX_PATH]=TEXT("");
	GetCurrentDirectory(sizeof(szPath),szPath);

	//读取配置
	TCHAR szFileName[MAX_PATH]=TEXT("");
	_snprintf(szFileName,sizeof(szFileName),TEXT("%s\\Spreader.ini"),szPath);

	//读取推荐人
	TCHAR szSpreader[NAME_LEN]=TEXT("");
	GetPrivateProfileString(TEXT("SpreaderInfo"),TEXT("SpreaderName"),TEXT(""),szSpreader,CountArray(szSpreader),szFileName);

	//设置名字
	if (szSpreader[0]!=0)
	{
		SetDlgItemText(IDC_SPREADER,szSpreader);
		((CEdit *)GetDlgItem(IDC_SPREADER))->SetReadOnly();
	} 
	MoveWindow(0,0,320,450);

	m_strOutput = (TEXT("请输入您的完整信息!"));

	((CButton *)GetDlgItem(IDC_CHECK_USER_DOCU))->SetCheck(1);

	//按钮绘制：
	HINSTANCE hInstance=AfxGetInstanceHandle();

	m_btLogon.LoadRgnImage(hInstance,IDB_REGIST_OK,DEF_TRANS_COLOR, 4);
	m_btCancel.LoadRgnImage(hInstance,IDB_REGIST_CANCEL,DEF_TRANS_COLOR, 4);
	m_btUserDocu.LoadRgnImage(hInstance,IDB_REGIST_USER_DOCU,DEF_TRANS_COLOR, 5);


	UpdateUserSex();


	((CEdit*)GetDlgItem(IDC_MAIL))->SetLimitText(32);
	((CEdit*)GetDlgItem(IDC_MAIL2))->SetLimitText(16);


	HDWP hDwp=BeginDeferWindowPos(32);
	const UINT uFlags=SWP_NOACTIVATE|SWP_NOZORDER|SWP_NOCOPYBITS|SWP_NOSIZE;
	DeferWindowPos(hDwp,m_btLogon,NULL,22,390,0,0,uFlags);
	DeferWindowPos(hDwp,m_btCancel,NULL,180,390,0,0,uFlags);
	DeferWindowPos(hDwp,m_btUserDocu,NULL,77,360,0,0,uFlags);
	m_rcOutput.SetRect(0,165,320,165+16);
	DeferWindowPos(hDwp,GetDlgItem(IDC_ACCOUNTS)->GetSafeHwnd(),NULL,93-5,187+5,127,16,SWP_NOACTIVATE|SWP_NOZORDER|SWP_NOCOPYBITS);
	DeferWindowPos(hDwp,GetDlgItem(IDC_PASSWORD)->GetSafeHwnd(),NULL,93-5,217+5,127,16,SWP_NOACTIVATE|SWP_NOZORDER|SWP_NOCOPYBITS);
	DeferWindowPos(hDwp,GetDlgItem(IDC_PASSWORD2)->GetSafeHwnd(),NULL,93-5,246+5,127,16,SWP_NOACTIVATE|SWP_NOZORDER|SWP_NOCOPYBITS);
	DeferWindowPos(hDwp,GetDlgItem(IDC_MAIL)->GetSafeHwnd(),NULL,93-5,276+5,87,16,SWP_NOACTIVATE|SWP_NOZORDER|SWP_NOCOPYBITS);
	DeferWindowPos(hDwp,GetDlgItem(IDC_MAIL2)->GetSafeHwnd(),NULL,198-5,276+5,87,16,SWP_NOACTIVATE|SWP_NOZORDER|SWP_NOCOPYBITS);
	DeferWindowPos(hDwp,GetDlgItem(IDC_SPREADER)->GetSafeHwnd(),NULL,93-5,306+5,127,16,SWP_NOACTIVATE|SWP_NOZORDER|SWP_NOCOPYBITS);
	DeferWindowPos(hDwp,GetDlgItem(IDC_CHECK_USER_DOCU)->GetSafeHwnd(),NULL,75-15,364,0,0,uFlags);
	DeferWindowPos(hDwp,m_btUserSexMan,NULL,104,334+5,0,0,uFlags);
	DeferWindowPos(hDwp,m_btUserSexWoman,NULL,184,334+5,0,0,uFlags);
	EndDeferWindowPos(hDwp);
	CenterWindow();
	GetDlgItem(IDC_ACCOUNTS)->SetFocus();
	SetNCRect(450);

	return TRUE;
}

//确定函数
void CDlgRegister::OnOK()
{
	//AfxMessageBox(TEXT("CDlgIndividualInfo::OnOK"));

	if (!IsOK())
		return;
	__super::OnOK();

}

//取消消息
void CDlgRegister::OnCancel()
{
	//AfxMessageBox(TEXT("CUserInfoView::OnCancel"));
	__super::OnCancel();
}

void CDlgRegister::OnPaint()
{
	CPaintDC dc(this); 
	CRect ClientRect;
	GetClientRect(&ClientRect);	
	DrawPic(&dc,IDB_REGIST_BG,&ClientRect);

	int oldBkMode=dc.SetBkMode(TRANSPARENT);

	CFont font;
	LOGFONT lf;
	memset(&lf, 0, sizeof(LOGFONT));       // Zero out the structure.
	lf.lfHeight = 12;                      // Request a 12-pixel-height font.
	lstrcpy(lf.lfFaceName, TEXT("宋体"));   // Request a face name "Arial".
	VERIFY(font.CreateFontIndirect(&lf));  // Create the font.
	// Use the font you just created.
	CFont* def_font = dc.SelectObject(&font);
	COLORREF rgbOldText= dc.SetTextColor(RGB(255,202,126));
	//dc.TextOut(m_rcOutput.left, m_rcOutput.top, m_strOutput);
	dc.DrawText(m_strOutput, m_rcOutput, DT_CENTER);
	dc.SetBkMode(oldBkMode);
	dc.SelectObject(def_font);
	dc.SetTextColor(rgbOldText);
	font.DeleteObject();


}

//修改用户性别——男
void CDlgRegister::OnEnMakeUserSexMan()
{
	m_cbGender = 0;
	UpdateUserSex();
}
//修改用户性别——女
void CDlgRegister::OnEnMakeUserSexWoman()
{
	m_cbGender = 1;
	UpdateUserSex();
}
//查看用户协议
void CDlgRegister::OnBnClickedUserDocu()
{
	//cxf 1202 ShellExecute(NULL,   "open",   WEB_USER_DOUCOMET,     ""   ,NULL,   SW_SHOW);
}

//激活帐号
void CDlgRegister::OnEnSetfocusAccouts()
{	
	CString strBuffer;
	GetDlgItemText(IDC_ACCOUNTS,strBuffer);
	strBuffer.TrimLeft();
	strBuffer.TrimRight();
	if (strBuffer.IsEmpty())
	{
		m_strOutput=TEXT("请输入帐号!");
	}
	else if (strBuffer.GetLength()<4)
	{
		m_strOutput=TEXT("帐号名字的长度最短为4位字符，请重新输入注册帐号!");
	}
	InvalidateRect(m_rcOutput, FALSE);
}
//激活密码
void CDlgRegister::OnEnSetfocusPassword()
{
	CString strBuffer;
	GetDlgItemText(IDC_PASSWORD,strBuffer);
	if (strBuffer.IsEmpty())
	{
		m_strOutput=TEXT("请输入密码!");
	}else if (strBuffer.GetLength()<6)
	{
		m_strOutput=TEXT("游戏密码长度最短为6位字符，请重新输入游戏密码!");
	}
	InvalidateRect(m_rcOutput, FALSE);
}
//激活确认密码
void CDlgRegister::OnEnSetfocusPassword2()
{
	CString strBuffer, strBuffer2;
	GetDlgItemText(IDC_PASSWORD,strBuffer);
	GetDlgItemText(IDC_PASSWORD2,strBuffer2);
	if (strBuffer2.IsEmpty())
	{
		m_strOutput=TEXT("请再次输入密码!");
	}
	else if (lstrcmp(strBuffer,strBuffer2)!=0)
	{
		m_strOutput=TEXT("游戏密码与确认密码不一致,请重新输入确认密码!");
	}
	InvalidateRect(m_rcOutput, FALSE);
}
//激活邮箱
void CDlgRegister::OnEnSetfocusMail()
{
	return;

	CString strBuffer;
	GetDlgItemText(IDC_MAIL,strBuffer);
	if (strBuffer.IsEmpty())
	{
		m_strOutput=TEXT("请输入邮箱!");
	}
	else
	{
		GetDlgItemText(IDC_MAIL2, strBuffer);
		CString strSuffix(strBuffer);
		strSuffix = strSuffix.MakeUpper();
		if (strSuffix.Find(TEXT(".COM")) <= 0
			&& strSuffix.Find(TEXT(".CN")) <= 0
			&& strSuffix.Find(TEXT(".NET")) <= 0
			&& strSuffix.Find(TEXT(".ASP")) <= 0
			&& strSuffix.Find(TEXT(".HTML")) <= 0
			&& strSuffix.Find(TEXT(".ORG")) <= 0
			&& strSuffix.Find(TEXT(".UK")) <= 0
			&& strSuffix.Find(TEXT(".EDU")) <= 0)
		{
			m_strOutput=TEXT("请输入正确的邮箱!");
		}
	}
	InvalidateRect(m_rcOutput, FALSE);
}
//激活邮箱后缀
void CDlgRegister::OnEnSetfocusMail2()
{
	return;
	OnEnSetfocusMail();
}
//激活推荐人
void CDlgRegister::OnEnSetfocusSpreader()
{
	m_strOutput=TEXT("请输入推荐人帐号!");
	InvalidateRect(m_rcOutput, FALSE);
}


void CDlgRegister::UpdateUserSex()
{
	HINSTANCE hInstance=g_GlobalUnits.m_PlatformResourceModule->GetResInstance();
	if (m_cbGender == 0)
	{
		m_btUserSexMan.LoadRgnImage(hInstance,g_GlobalUnits.m_PlazaViewImage.uBtRadioNo);
		m_btUserSexWoman.LoadRgnImage(hInstance,g_GlobalUnits.m_PlazaViewImage.uBtRadioOff);
	}else
	{
		m_btUserSexMan.LoadRgnImage(hInstance,g_GlobalUnits.m_PlazaViewImage.uBtRadioOff);
		m_btUserSexWoman.LoadRgnImage(hInstance,g_GlobalUnits.m_PlazaViewImage.uBtRadioNo);
	}
	m_btUserSexMan.Invalidate(FALSE);
	m_btUserSexWoman.Invalidate(FALSE);
}
BOOL CDlgRegister::IsOK()
{
	CString strBuffer;
	//用户帐号
	GetDlgItemText(IDC_ACCOUNTS,strBuffer);
	strBuffer.TrimLeft();
	strBuffer.TrimRight();
	if (strBuffer.GetLength()<4)
	{
		//ShowInformation(TEXT("帐号名字的长度最短为 4 位字符，请重新输入注册帐号！"),0,MB_ICONQUESTION);
		m_strOutput=TEXT("帐号名字的长度最短为4位字符,请重新输入注册帐号!");
		InvalidateRect(m_rcOutput, FALSE);
		GetDlgItem(IDC_ACCOUNTS)->SetFocus();
		return FALSE;
	}
	lstrcpyn(m_szAccounts,strBuffer,CountArray(m_szAccounts));

	//用户密码
	GetDlgItemText(IDC_PASSWORD,strBuffer);
	if (strBuffer.GetLength()<6)
	{
		//ShowInformation(TEXT("游戏密码长度最短为 6 位字符，请重新输入游戏密码！"),0,MB_ICONQUESTION);
		m_strOutput=TEXT("游戏密码长度最短为6位字符,请重新输入游戏密码!");
		InvalidateRect(m_rcOutput, FALSE);
		GetDlgItem(IDC_PASSWORD)->SetFocus();
		return FALSE;
	}

	//确认密码
	GetDlgItemText(IDC_PASSWORD2,m_szPassword,CountArray(m_szPassword));
	if (lstrcmp(m_szPassword,strBuffer)!=0)
	{
		//ShowInformation(TEXT("游戏密码与确认密码不一致，请重新输入确认密码！"),0,MB_ICONQUESTION);
		m_strOutput=TEXT("游戏密码与确认密码不一致，请重新输入确认密码!");
		InvalidateRect(m_rcOutput, FALSE);
		GetDlgItem(IDC_PASSWORD2)->SetFocus();
		return FALSE;
	}
	////邮箱
	//GetDlgItemText(IDC_MAIL,m_szMail,CountArray(m_szMail));
	//GetDlgItemText(IDC_MAIL2,m_szMailSuffix,CountArray(m_szMailSuffix));
	//if (strlen(m_szMail)<1)
	//{
	//	m_strOutput=TEXT("请输入正确的邮箱!");
	//	InvalidateRect(m_rcOutput, FALSE);
	//	GetDlgItem(IDC_MAIL)->SetFocus();
	//	return FALSE;
	//}
	//CString strSuffix(m_szMailSuffix);
	//strSuffix = strSuffix.MakeUpper();
	//if (strSuffix.Find(TEXT(".COM")) <= 0
	//	&& strSuffix.Find(TEXT(".CN")) <= 0
	//	&& strSuffix.Find(TEXT(".NET")) <= 0
	//	&& strSuffix.Find(TEXT(".ASP")) <= 0
	//	&& strSuffix.Find(TEXT(".ORG")) <= 0
	//	&& strSuffix.Find(TEXT(".UK")) <= 0
	//	&& strSuffix.Find(TEXT(".EDU")) <= 0
	//	&& strSuffix.Find(TEXT(".HTML")) <= 0)
	//{
	//	m_strOutput=TEXT("请输入正确的邮箱后缀后!");
	//	InvalidateRect(m_rcOutput, FALSE);
	//	GetDlgItem(IDC_MAIL2)->SetFocus();
	//	return FALSE;
	//}

	if (((CButton *)GetDlgItem(IDC_CHECK_USER_DOCU))->GetCheck()!=BST_CHECKED)
	{
		m_strOutput=TEXT("请仔细阅读用户协议!并同意接受用户协议!");
		InvalidateRect(m_rcOutput, FALSE);
		return FALSE;
	}
	//推广员名
	GetDlgItemText(IDC_SPREADER,strBuffer);
	strBuffer.TrimLeft();
	strBuffer.TrimRight();
	lstrcpyn(m_szSpreader,strBuffer,CountArray(m_szSpreader));

	//用户性别
	// 	int nCurSel=((CComboBox *)GetDlgItem(IDC_GENDER))->GetCurSel();
	// 	m_cbGender=(BYTE)((CComboBox *)GetDlgItem(IDC_GENDER))->GetItemData(nCurSel);

	//用户头像
	m_wFaceID=0/*m_FaceSelect.GetCurSel()*/;
	return TRUE;
}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////



//构造函数
CDlgLogon::CDlgLogon() : CSkinDialogEx(IDD_LOGON), m_bIsLogoCreate(false), m_bIsKeyDlgShow(false)
{
	//登录信息
	m_wFaceID=0;
	m_cbGender=0;
	m_dwUserID=0;
	m_szSpreader[0]=0;
	m_szAccounts[0]=0;
	m_szPassword[0]=0;
	m_szMail[0]=0;
	m_LogonMode=LogonMode_Accounts;

	//辅助信息
	m_bRegister=false;
	m_bChangePassWord=false;

	//位置信息
	m_nFullWidth=0;
	m_nFullHeight=0;
	m_bNetOption=false;

	m_bRemPassword = false;

	m_NCRect.Caption = 30;
	return;
}

//析构函数
CDlgLogon::~CDlgLogon()
{
}

//控件绑定
void CDlgLogon::DoDataExchange(CDataExchange * pDX)
{
	__super::DoDataExchange(pDX);
	DDX_Control(pDX, IDOK, m_btLogon);
	DDX_Control(pDX, IDCANCEL, m_btCancel);
	DDX_Control(pDX, IDC_REGISTER, m_btRegister);
	DDX_Control(pDX, IDC_DELETE_RECORD, m_btDelete);
	DDX_Control(pDX, IDC_LOGON_TYPE, m_TabLogonMode);
	DDX_Control(pDX, IDC_MAIN_PAGE, m_LineMainPage);
	DDX_Control(pDX, IDC_PASSWORD_PROTECT, m_LinePassWord);
	DDX_Control(pDX, IDC_GET_PASSWORD, m_LineGetPassWord);
	DDX_Control(pDX, IDC_BUTTON_GETBACK_PASSWORD,m_btGetBackPassword);
	DDX_Control(pDX, IDC_BUTTON_VISIT_WEB,m_btVisitWeb);
	DDX_Control(pDX, IDC_ACCOUNTS_MANAGE,m_btAccountsManage);
	DDX_Control(pDX, IDC_NET_OPTION, m_btNetOption);
	DDX_Control(pDX, IDC_PROXY_TEST, m_btProxyTest);	
	DDX_Control(pDX, IDC_PROXY_TYPE, m_cmProxyServerType);
	DDX_Control(pDX, IDC_PROXY_PORT, m_edProxyPort);
	DDX_Control(pDX, IDC_PROXY_SERVER, m_edProxyServer);
	DDX_Control(pDX, IDC_PROXY_USER, m_edProxyUserName);
	DDX_Control(pDX, IDC_PROXY_PASS, m_edProxyPassword);
	DDX_Control(pDX, IDC_CHECK_PASSWORD, m_checkPassword);

	DDX_Control(pDX, IDC_BUTTON_GAME_HELP, m_btnGameHelp);
	DDX_Control(pDX, IDC_BUTTON_GAME_SHOP, m_btnGameShop);
	DDX_Control(pDX, IDC_BUTTON_GAME_CLIENT, m_btnGameClinet);
	DDX_Control(pDX, IDC_BUTTON_KEY, m_btnKey);
	DDX_Control(pDX, IDC_EDIT_ACCOUNTS, m_edUserAccounts);
	DDX_Control(pDX, IDC_PASSWORD, m_edUserPassword);

	DDX_Control(pDX, IDC_BT_TELECOM, m_btTelecom);
	DDX_Control(pDX, IDC_BT_MESHWORK, m_btMeshWork);
	DDX_Control(pDX, IDC_BT_AUTO, m_btAuto);

	DDX_Control(pDX, IDC_GAME_WEB,m_btGameWeb);

	DDX_Control(pDX, IDC_BT_LOGON_EXIT,m_btExit);
}

//初始化函数
BOOL CDlgLogon::OnInitDialog()
{
	__super::OnInitDialog();
	//cxf
	int width   =   GetSystemMetrics(SM_CXSCREEN); 
	int height   =   GetSystemMetrics(SM_CYSCREEN);
	SetWindowPos(NULL, (width-640)/2, (height-420)/2, 640, 420, SWP_HIDEWINDOW);

	m_bIsLogoCreate = false;
	m_bIsKeyDlgShow = false;
	//设置标题
	SetWindowText("藏宝湾"); 

	CInitData::InitData();
	//广告控件
	//m_BrowerAD.Create(NULL,NULL,WS_VISIBLE|WS_CHILD,CRect(0,0,0,0),this,100,NULL);
	//m_BrowerAD.Navigate(TEXT("http://www.acetechcn.com"),NULL,NULL,NULL,NULL);
	m_bRegister=false;
	m_ToolTipCtrl.Create(this);
	m_ToolTipCtrl.Activate(TRUE);
	//广告位置
	int nXExcursion=100;//GetXExcursionPos();
	int nYExcursion=30;//GetYExcursionPos();
	//m_BrowerAD.MoveWindow(nXExcursion+4,nYExcursion+5,507,255);

	//加入标签
	//m_TabLogonMode.InsertItem(LOGON_BY_ACCOUNTS,TEXT("帐号登录"));
	//m_TabLogonMode.InsertItem(LOGON_BY_USERID,TEXT("ＩＤ登录"));

	//设置控件
	//m_LineMainPage.SetHyperLinkUrl(TEXT("http://qp.foxuc.net"));
	//m_LinePassWord.SetHyperLinkUrl(TEXT("http://qp.foxuc.net/Passwordprotection.asp"));
	//m_LineGetPassWord.SetHyperLinkUrl(TEXT("http://qp.foxuc.net/FindPassWord.asp"));
	CString strPath("");
	GetResPath("\\Url\\login\\", strPath);
	m_LogonAdvertisement.Create(NULL,NULL,WS_VISIBLE | WS_CHILD,CRect(0,0,0,0),this,LOGONADVERTISEMENT,NULL);
	m_LogonNotice.Create(NULL,NULL,WS_VISIBLE | WS_CHILD,CRect(0,0,0,0),this,LOGONNOTICE,NULL);
	m_LogonAdvertisement.Navigate(TEXT(strPath+"LogonAdvertisement.html"),NULL,NULL,NULL,NULL);
	m_LogonNotice.Navigate(TEXT(strPath+"LogonNotice.html"),NULL,NULL,NULL,NULL);

	m_LineMainPage.BringWindowToTop();
	m_LinePassWord.BringWindowToTop();
	m_LineGetPassWord.BringWindowToTop();

	m_edUserPassword.m_SkinAttribute.m_crFocusBK=0xFFFFFF;//背景颜色
	m_edUserPassword.m_SkinAttribute.m_crNoFocusBK=0xFFFFFF;
	m_edUserPassword.m_SkinAttribute.m_crDisFocusBK=0xE5E5E5;
	m_edUserPassword.m_SkinAttribute.m_crFocusTX=0x000000;//字体颜色
	m_edUserPassword.m_SkinAttribute.m_crNoFocusTX=0x000000;					
	m_edUserPassword.m_SkinAttribute.m_crDisFocusTX=0x000000;		

	//按钮绘制：
	/*cxf HINSTANCE hInstance=g_GlobalUnits.m_PlatformResourceModule->GetResInstance();	
	m_btLogon.SetButtonImage(g_GlobalUnits.m_PlazaViewImage.uBtLogon, hInstance, false);
	m_btCancel.SetButtonImage(g_GlobalUnits.m_PlazaViewImage.uBtQuit, hInstance, false);
	m_btRegister.LoadRgnImage(hInstance,g_GlobalUnits.m_PlazaViewImage.uBtAccount,4);
	m_btGetBackPassword.LoadRgnImage(hInstance,g_GlobalUnits.m_PlazaViewImage.uBtGetPassword,4);
	m_btVisitWeb.LoadRgnImage(hInstance,g_GlobalUnits.m_PlazaViewImage.uBtVisitWeb,4);
	m_btDelete.LoadRgnImage(hInstance,g_GlobalUnits.m_PlazaViewImage.uBtClear,4);
	m_btnKey.LoadRgnImage(hInstance,g_GlobalUnits.m_PlazaViewImage.uBtKey,4);
	m_btnGameHelp.LoadRgnImage(hInstance,g_GlobalUnits.m_PlazaViewImage.uBtGameHelp,4);
	m_btnGameShop.LoadRgnImage(hInstance,g_GlobalUnits.m_PlazaViewImage.uBtGameShop,4);
	m_btnGameClinet.LoadRgnImage(hInstance,g_GlobalUnits.m_PlazaViewImage.uBtGameClinet,4);
	m_checkPassword.LoadRgnImage(hInstance,g_GlobalUnits.m_PlazaViewImage.uBtCheckOff,4);
	*/

	//加载图片资源
	CString strValue("");
	GetImagePath(strValue);
	m_pngLogon.LoadImage(TEXT(strValue+"BKLogonback.png"));

	//cxf
	m_btExit.LoadRgnImage(TEXT(strValue+"BTCancel.png"), false,4);
	m_btLogon.SetButtonImage(TEXT(strValue+"BTLogon.png"), false, 4);
	m_btCancel.SetButtonImage(TEXT(strValue+"BTCancel.png"), false, 4);
	m_btRegister.LoadRgnImage(TEXT(strValue+"BTRegister.png"), false, 4);
	m_btGetBackPassword.LoadRgnImage(TEXT(strValue+"BTGetBackPassword.png"), false, 4);
	m_btVisitWeb.LoadRgnImage(TEXT(strValue+"BTVisitWeb.png"), false, 4);
	m_btDelete.LoadRgnImage(TEXT(strValue+"BTDelete.png"), false, 4);
	m_btnKey.LoadRgnImage(TEXT(strValue+"BTKey.png"), false,4);
	m_btnGameClinet.LoadRgnImage(TEXT(strValue+"BTGameClinet.png"), false, 4);
	m_checkPassword.LoadRgnImage(TEXT(strValue+"BTCheckNo.png"), false);
	m_btTelecom.LoadRgnImage(TEXT(strValue+"BTCT.png"), false);
	m_btMeshWork.LoadRgnImage(TEXT(strValue+"BTCU.png"), false);
	m_btAuto.LoadRgnImage(TEXT(strValue+"BTAuto.png"), false);
	m_btAccountsManage.LoadRgnImage(TEXT(strValue+"BTAccountsManage.png"), false,4);
	m_btGameWeb.LoadRgnImage(TEXT(strValue+"BTGameWeb.png"), false,4);


	m_ToolTipCtrl.AddTool(&m_btExit,TEXT("退出"));
	m_ToolTipCtrl.AddTool(&m_btLogon,TEXT("登陆游戏"));
	m_ToolTipCtrl.AddTool(&m_btRegister,TEXT("注册帐号"));
	m_ToolTipCtrl.AddTool(&m_btCancel,TEXT("退出"));
	m_ToolTipCtrl.AddTool(&m_btGetBackPassword,TEXT("找回密码"));
	m_ToolTipCtrl.AddTool(&m_btVisitWeb,TEXT("官方网站"));
	m_ToolTipCtrl.AddTool(&m_btDelete,TEXT("清空帐号和密码"));
	m_ToolTipCtrl.AddTool(&m_btnKey,TEXT("打开\\关闭软件盘"));
	m_ToolTipCtrl.AddTool(&m_btnGameHelp,TEXT("游戏帮助"));
	m_ToolTipCtrl.AddTool(&m_btnGameShop,TEXT("游戏商场"));
	m_ToolTipCtrl.AddTool(&m_btnGameClinet,TEXT("在线客服"));
	m_ToolTipCtrl.AddTool(&m_checkPassword,TEXT("记住密码"));
	m_ToolTipCtrl.AddTool(&m_btAccountsManage,TEXT("帐号管理"));
	
	m_ToolTipCtrl.AddTool(&m_btTelecom,TEXT("连接电信线路"));
	m_ToolTipCtrl.AddTool(&m_btMeshWork,TEXT("连接网通线路"));
	m_ToolTipCtrl.AddTool(&m_btAuto,TEXT("系统根据IP自动选择线路"));

	//锁定屏幕
	//LockWindowUpdate();

	HDWP hDwp=BeginDeferWindowPos(32);
	const UINT uFlags=SWP_NOZORDER|SWP_NOCOPYBITS|SWP_NOSIZE;
	DeferWindowPos(hDwp,m_btExit,NULL,597,20,0,0,uFlags);
	DeferWindowPos(hDwp,m_btLogon,NULL,52,307,0,0,uFlags);
	DeferWindowPos(hDwp,m_btCancel,NULL,0,0,0,0,uFlags);
	DeferWindowPos(hDwp,m_btDelete,NULL,247,149,0,0,uFlags);
	DeferWindowPos(hDwp,m_btnKey,NULL,209,195,0,0,uFlags);
	DeferWindowPos(hDwp, m_btRegister, NULL, 272,64,0,0,uFlags);
	DeferWindowPos(hDwp, m_btVisitWeb, NULL, 359,64,0,0,uFlags);
	DeferWindowPos(hDwp, m_btAccountsManage, NULL, 446,64,0,0,uFlags);
	DeferWindowPos(hDwp, m_btnGameClinet, NULL, 535,64,0,0,uFlags);
	DeferWindowPos(hDwp, m_checkPassword, NULL, 48,235,0,0,uFlags);
	DeferWindowPos(hDwp, m_btGetBackPassword, NULL, 182,237,0,0,uFlags);

	DeferWindowPos(hDwp, m_edUserAccounts, NULL, 93,149,142,150, SWP_SHOWWINDOW);

	DeferWindowPos(hDwp, m_edUserPassword, NULL, 95,198,100,12,SWP_NOZORDER);
	DeferWindowPos(hDwp, m_btTelecom, NULL, 114,273,0,0,uFlags);
	DeferWindowPos(hDwp, m_btMeshWork, NULL, 188,273,0,0,uFlags);
	DeferWindowPos(hDwp, m_btAuto, NULL, 50,273,0,0,uFlags);
	DeferWindowPos(hDwp, m_btGameWeb, NULL, 340,390,0,0,uFlags);
	
	m_LogonAdvertisement.MoveWindow(278,112,330,260);
	m_LogonNotice.MoveWindow(29,85,232,41);

	/*cxf
	DeferWindowPos(hDwp, m_btnGameHelp, NULL, 305,52,0,0,uFlags);
	DeferWindowPos(hDwp, m_btnGameShop, NULL, 554,52,0,0,uFlags);
	*/
	m_btCancel.ShowWindow(SW_HIDE);
	m_btnGameHelp.ShowWindow(SW_HIDE);
	m_btnGameShop.ShowWindow(SW_HIDE);
	//end

	m_DlgLogonLogo = new CDlgLogonLogo();
	if (NULL != m_DlgLogonLogo)
	{
		m_DlgLogonLogo->Create(IDD_LOGONLOGO,this);
		CRect crDlgLogon;
		GetWindowRect(&crDlgLogon);
		m_DlgLogonLogo->SetWindowPos(NULL,crDlgLogon.left-24,crDlgLogon.top-126,350,260,SWP_SHOWWINDOW);
	}

	EndDeferWindowPos(hDwp);

	//解除锁定
	//UnlockWindowUpdate();

	//记录密码
	// 	if (g_GlobalOption.m_enAcountsRule==enAcountsRule_AccountsAndPass)
	// 	{
	// 		m_bRemPassword = true; 
	// 		m_checkPassword.LoadRgnImage(hInstance,g_GlobalUnits.m_PlazaViewImage.uBtCheckNo);
	// 	}
	m_enRule = g_GlobalOption.m_enAcountsRule;

	//加载信息
	LoadLogonServer();
	LoadAccountsInfo();
	LoadProxyServerInfo();

	//限制输入
	m_edUserAccounts.LimitText(NAME_LEN-1);
	//设置焦点
	if (m_edUserAccounts.GetWindowTextLength()==0)
	{
		m_edUserAccounts.SetFocus();
		m_edUserAccounts.SetEditSel(0, -1);
	}
	else 
	{
		GetDlgItem(IDC_PASSWORD)->SetFocus();
		((CEdit *)GetDlgItem(IDC_PASSWORD))->SetSel(0,-1);
	}

	//框架位置
	CRect rcFrame;
	//GetWindowRect(&m_rcNormalFrame);

	AfxGetMainWnd()->GetWindowRect(&m_rcNormalFrame);
	//m_rcNormalFrame.SetRect(0,0,960,720);
	GetDlgItem(IDC_BORDER)->GetWindowRect(&rcFrame);

	//记录位置
	m_nFullWidth=m_rcNormalFrame.Width();
	m_nFullHeight=m_rcNormalFrame.Height();

	//调整位置
	//m_rcNormalFrame.bottom=rcFrame.top;
	//::SetWindowPos(m_hWnd, 0, m_rcNormalFrame.left, m_rcNormalFrame.top, m_rcNormalFrame.Width(), m_rcNormalFrame.Height(), 0);
	MoveWindow(&m_rcNormalFrame,FALSE);

	SetImageRgn(&m_pngLogon);

	SetTimer(TIMER_WEB_LOADING,20,NULL);

	m_bIsLogoCreate = true;
	//CenterWindow(this);

	if (2 == g_GlobalUnits.GetApplicationCmdLine().GetSize())
	{
		EnableControlWindow(FALSE);
		SetTimer(TIME_ID_UPDATEPROGRAM_LOGON, 1000, NULL);
	}

	if (2==g_GlobalOption.m_byNetWorkTag)
	{
		GetParent()->PostMessage(IDM_CHECK_LINE, (WPARAM)1, (LPARAM)1);
		m_btLogon.EnableWindow(FALSE);
	}
	
	return FALSE;
}

void CDlgLogon::OnBnClickedGetBackPassword()
{
	ShellExecute(NULL,   "open",   CInitData::s_cGetPassword,     ""   ,NULL,   SW_SHOW);
}
void CDlgLogon::OnBnClickedVisitWeb()
{
	ShellExecute(NULL,   "open",   CInitData::s_cHomePage,     ""   ,NULL,   SW_SHOW);
}

void CDlgLogon::OnBnClickedAccountsManage()
{
	ShellExecute(NULL,   "open",   CInitData::s_cAccountsManage,     ""   ,NULL,   SW_SHOW);
}
void CDlgLogon::OnBnClickedGameWeb()
{
	ShellExecute(NULL,   "open",   CInitData::s_cHomePage,     ""   ,NULL,   SW_SHOW);
}
//消息解释
BOOL CDlgLogon::PreTranslateMessage(MSG * pMsg)
{
	if (::IsWindow(m_ToolTipCtrl.GetSafeHwnd()))
		m_ToolTipCtrl.RelayEvent(pMsg);

	if(pMsg->message==WM_KEYDOWN)
	{
		if (VK_SPACE == pMsg->wParam)
		{
			return TRUE;
		}
		else if (VK_RETURN == pMsg->wParam)
		{
			if (IsWindowVisible())
			{
				OnOK();
			}
			return TRUE;
		}
	}
	return __super::PreTranslateMessage(pMsg);
}

//电信
void CDlgLogon::OnEnTelecomMsg()
{
	m_btTelecom.SetSelect(true);
	m_btTelecom.Invalidate(FALSE);
	m_btMeshWork.SetSelect(false);
	m_btMeshWork.Invalidate(FALSE);
	m_btAuto.SetSelect(false);
	m_btAuto.Invalidate(FALSE);

	g_GlobalOption.m_byNetWorkTag = 0;
}
//网通
void CDlgLogon::OnEnMeshWorkMsg()
{
	m_btTelecom.SetSelect(false);
	m_btTelecom.Invalidate(FALSE);
	m_btMeshWork.SetSelect(true);
	m_btMeshWork.Invalidate(FALSE);
	m_btAuto.SetSelect(false);
	m_btAuto.Invalidate(FALSE);

	g_GlobalOption.m_byNetWorkTag = 1;
}

void CDlgLogon::OnEnAutoMsg()
{
	m_btTelecom.SetSelect(false);
	m_btTelecom.Invalidate(FALSE);
	m_btMeshWork.SetSelect(false);
	m_btMeshWork.Invalidate(FALSE);
	m_btAuto.SetSelect(true);
	m_btAuto.Invalidate(FALSE);

	g_GlobalOption.m_byNetWorkTag = 2;
}

void CDlgLogon::SetCheckLineResult(BYTE bNetWorkTag)
{
	//IP_TYPE_Unknow = 0,				// 未知
	//IP_TYPE_China_Telecom = 1,		// 中国电信
	//IP_TYPE_China_Unicom = 2,		// 中国联通，联通、网通已经合并了，是一家了
	//IP_TYPE_China_Railcom = 3,		// 中国铁通
	/// 请参照IpFinder.h文件

	// 数据调整
	// m_byNetWorkTag: 0电信；1网通
	if (2 == bNetWorkTag)
		g_GlobalOption.m_byCheckLineResult = 1;
	else
		g_GlobalOption.m_byCheckLineResult = 0;

	// g_GlobalOption.m_byNetWorkTag不为2，说明玩家选择了线路
	if (2 == g_GlobalOption.m_byNetWorkTag)
	{
		g_GlobalOption.m_byNetWorkTag = g_GlobalOption.m_byCheckLineResult;
	}

	m_btLogon.EnableWindow(TRUE);
}

//确定函数
void CDlgLogon::OnOK()
{
	//AfxMessageBox(TEXT("CDlgLogon::OnOK"));
	//效验输入
	if (CheckLogonInput(true)==false) return;

	//设置变量
	m_bRegister=false;

	// 
	if (0 > g_GlobalOption.m_byNetWorkTag  ||  1 < g_GlobalOption.m_byNetWorkTag)
		g_GlobalOption.m_byNetWorkTag = 0;

	AfxGetMainWnd()->PostMessage(WM_COMMAND,IDM_CONNECT_SERVER,0);
	//IPlazaViewItem * pIPlazaViewItem=((CGameFrame*)AfxGetMainWnd())->GetPlazaViewItem();
	//ASSERT(pIPlazaViewItem!=NULL);

	//pIPlazaViewItem->SendConnectMessage();

	//显示主窗口
	/*CWnd * pMainWnd=((CGamePlazaApp*)AfxGetApp())->m_pMainWnd;
	pMainWnd->ShowWindow(SW_SHOW);*/

	return;
}

//取消消息
void CDlgLogon::OnCancel()
{
	((CGamePlazaApp*)AfxGetApp())->ExitGame();
}
//退出登录
void CDlgLogon::OnBnClickedOnExit()
{
	((CGamePlazaApp*)AfxGetApp())->ExitGame();
}

void CDlgLogon::EnableControlWindow(BOOL bEnable)
{
	m_edUserAccounts.EnableWindow(bEnable);
	m_edUserPassword.EnableWindow(bEnable);
	m_btLogon.EnableWindow(bEnable);
}

//退出消息
LRESULT CDlgLogon::OnExitMessage(WPARAM wParam, LPARAM lParam)
{
	OnBnClickedOnExit();
	return 0;
}
void CDlgLogon::ExitLogon()
{
	DestroyWindow();
	AfxGetMainWnd()->PostMessage(WM_CLOSE,0,0);
}

//发送登录包
bool CDlgLogon::SendLogonPacket(ITCPSocket * pIClientSocke)
{
	//变量定义
	TCHAR szPassword[33];
	BYTE cbBuffer[SOCKET_PACKET];

	if (2 == g_GlobalUnits.GetApplicationCmdLine().GetSize())
	{
		_snprintf(szPassword, sizeof(szPassword), "%s", g_GlobalUnits.GetApplicationCmdLine().GetAt(1));
	}
	else 
	{
		CMD5Encrypt::EncryptData(m_szPassword,szPassword);
	}

	//发送数据
	switch (m_LogonMode)
	{
	case LogonMode_Accounts:		//帐号登录
		{
			if (m_bRegister==false)
			{
				//构造数据
				CMD_GP_LogonByAccounts * pLogonByAccounts=(CMD_GP_LogonByAccounts *)cbBuffer;
				memset(pLogonByAccounts,0,sizeof(CMD_GP_LogonByAccounts));
				pLogonByAccounts->dwPlazaVersion=g_GlobalUnits.GetPlazaVersion();
				CopyMemory(pLogonByAccounts->szPassWord,szPassword,sizeof(pLogonByAccounts->szPassWord));
				lstrcpyn(pLogonByAccounts->szAccounts,m_szAccounts,CountArray(pLogonByAccounts->szAccounts));

				//机器序列号
				tagClientSerial ClientSerial;
				g_GlobalUnits.GetClientSerial(ClientSerial);

				//发送数据包
				CSendPacketHelper Packet(cbBuffer+sizeof(CMD_GP_LogonByAccounts),sizeof(cbBuffer)-sizeof(CMD_GP_LogonByAccounts));
				Packet.AddPacket(&ClientSerial,sizeof(ClientSerial),DTP_COMPUTER_ID);
				pIClientSocke->SendData(MDM_GP_LOGON,SUB_GP_LOGON_ACCOUNTS,cbBuffer,sizeof(CMD_GP_LogonByAccounts)+Packet.GetDataSize());
			}
			else
			{
				//构造数据
				CMD_GP_RegisterAccounts * pRegisterAccounts=(CMD_GP_RegisterAccounts *)cbBuffer;
				memset(pRegisterAccounts,0,sizeof(CMD_GP_RegisterAccounts));
				pRegisterAccounts->wFaceID=m_wFaceID;
				pRegisterAccounts->cbGender=m_cbGender;
				pRegisterAccounts->dwPlazaVersion=g_GlobalUnits.GetPlazaVersion();
				lstrcpyn(pRegisterAccounts->szPassWord,szPassword,CountArray(pRegisterAccounts->szPassWord));
				lstrcpyn(pRegisterAccounts->szSpreader,m_szSpreader,CountArray(pRegisterAccounts->szSpreader));
				lstrcpyn(pRegisterAccounts->szAccounts,m_szAccounts,CountArray(pRegisterAccounts->szAccounts));
				lstrcpyn(pRegisterAccounts->szEmail,m_szMail,CountArray(pRegisterAccounts->szEmail));


				//机器序列号
				tagClientSerial ClientSerial;
				g_GlobalUnits.GetClientSerial(ClientSerial);

				//发送数据包
				CSendPacketHelper Packet(cbBuffer+sizeof(CMD_GP_RegisterAccounts),sizeof(cbBuffer)-sizeof(CMD_GP_RegisterAccounts));
				Packet.AddPacket(&ClientSerial,sizeof(ClientSerial),DTP_COMPUTER_ID);
				pIClientSocke->SendData(MDM_GP_LOGON,SUB_GP_REGISTER_ACCOUNTS,cbBuffer,sizeof(CMD_GP_RegisterAccounts)+Packet.GetDataSize());
			}

			break;
		}
	case LogonMode_UserID:			//I D 登录
		{
			if (m_bRegister==false)
			{
				//构造数据
				CMD_GP_LogonByUserID * pLogonByUserID=(CMD_GP_LogonByUserID *)cbBuffer;
				memset(pLogonByUserID,0,sizeof(CMD_GP_LogonByUserID));
				pLogonByUserID->dwUserID=m_dwUserID;
				pLogonByUserID->dwPlazaVersion=g_GlobalUnits.GetPlazaVersion();
				CopyMemory(pLogonByUserID->szPassWord,szPassword,sizeof(pLogonByUserID->szPassWord));

				//机器序列号
				tagClientSerial ClientSerial;
				g_GlobalUnits.GetClientSerial(ClientSerial);

				//发送数据包
				CSendPacketHelper Packet(cbBuffer+sizeof(CMD_GP_LogonByUserID),sizeof(cbBuffer)-sizeof(CMD_GP_LogonByUserID));
				Packet.AddPacket(&ClientSerial,sizeof(ClientSerial),DTP_COMPUTER_ID);
				pIClientSocke->SendData(MDM_GP_LOGON,SUB_GP_LOGON_USERID,cbBuffer,sizeof(CMD_GP_LogonByUserID)+Packet.GetDataSize());
			}
			else
			{
				//构造数据
				CMD_GP_RegisterAccounts * pRegisterAccounts=(CMD_GP_RegisterAccounts *)cbBuffer;
				memset(pRegisterAccounts,0,sizeof(CMD_GP_RegisterAccounts));
				pRegisterAccounts->wFaceID=m_wFaceID;
				pRegisterAccounts->cbGender=m_cbGender;
				pRegisterAccounts->dwPlazaVersion=g_GlobalUnits.GetPlazaVersion();
				lstrcpyn(pRegisterAccounts->szPassWord,szPassword,CountArray(pRegisterAccounts->szPassWord));
				lstrcpyn(pRegisterAccounts->szSpreader,m_szSpreader,CountArray(pRegisterAccounts->szSpreader));
				lstrcpyn(pRegisterAccounts->szAccounts,m_szAccounts,CountArray(pRegisterAccounts->szAccounts));

				//机器序列号
				tagClientSerial ClientSerial;
				g_GlobalUnits.GetClientSerial(ClientSerial);

				//发送数据包
				CSendPacketHelper Packet(cbBuffer+sizeof(CMD_GP_RegisterAccounts),sizeof(cbBuffer)-sizeof(CMD_GP_RegisterAccounts));
				Packet.AddPacket(&ClientSerial,sizeof(ClientSerial),DTP_COMPUTER_ID);
				pIClientSocke->SendData(MDM_GP_LOGON,SUB_GP_REGISTER_ACCOUNTS,cbBuffer,sizeof(CMD_GP_RegisterAccounts)+Packet.GetDataSize());
			}

			break;
		}
	}

	return true;
}

//登录成功处理
bool CDlgLogon::OnLogonSuccess()
{
	//登录配置
	AfxGetApp()->WriteProfileInt(REG_OPTION_LOGON,TEXT("LogonMode"),m_LogonMode);
	AfxGetApp()->WriteProfileString(REG_OPTION_LOGON,TEXT("LogonServer"),m_strLogonServer);

	//配置信息
	if (!m_bRemPassword)
	{
		if (g_GlobalOption.m_enAcountsRule==enAcountsRule_AccountsAndPass)
		{
			g_GlobalOption.m_enAcountsRule=enAcountsRule_Accounts;
		}
	}
	else
	{
		g_GlobalOption.m_enAcountsRule=enAcountsRule_AccountsAndPass;
	}
	m_enRule = g_GlobalOption.m_enAcountsRule;

	//用户信息
	tagGlobalUserData & UserData=g_GlobalUnits.GetGolbalUserData();
	AfxGetApp()->WriteProfileInt(REG_OPTION_LOGON,TEXT("LastUserID"),UserData.dwUserID);

	//写入用户记录
	if (g_GlobalOption.m_enAcountsRule!=enAcountsRule_None)
	{
		//构造信息
		TCHAR szBuffer[256];
		_snprintf(szBuffer,sizeof(szBuffer),TEXT("%s\\%ld"),REG_USER_INFO,UserData.dwUserID);

		//写入信息
		CRegKey RegUserInfo;
		if (RegUserInfo.Create(HKEY_CURRENT_USER,szBuffer)==ERROR_SUCCESS)
		{
			TCHAR szPassBuffer[256]=TEXT("");
			if (m_bRemPassword) 
			{
				CXOREncrypt::EncryptData(m_szPassword,szPassBuffer,CountArray(szPassBuffer));
			}
			RegUserInfo.SetKeyValue(TEXT(""),UserData.szAccounts,TEXT("UserAccount"));
			RegUserInfo.SetKeyValue(TEXT(""),UserData.szPassWord,TEXT("UserPassToken"));
			RegUserInfo.SetKeyValue(TEXT(""),szPassBuffer,TEXT("UserPassword"));
			RegUserInfo.SetDWORDValue(TEXT("AcountsRule"),m_enRule);
			if (UserData.dwGameID!=0L) 
			{
				RegUserInfo.SetDWORDValue(TEXT("GameID"),UserData.dwGameID);
			}
			RegUserInfo.SetDWORDValue(TEXT("UserNetWork"),g_GlobalOption.m_byNetWorkTag);
			RegUserInfo.SetDWORDValue(TEXT("BackMusicTag"),g_GlobalOption.m_bEnableBackMusic?1:0);
		}
	}
	else
	{
		//构造信息
		TCHAR szBuffer[256];
		_snprintf(szBuffer,sizeof(szBuffer),TEXT("%ld"),UserData.dwUserID);

		//写入信息
		CRegKey RegUserID;
		if (RegUserID.Open(HKEY_CURRENT_USER,REG_USER_INFO)==ERROR_SUCCESS)	RegUserID.RecurseDeleteKey(szBuffer);
	}

	m_bRegister=false;
	//关闭窗口
	//DestroyWindow();
	ShowWindow(SW_HIDE);
	m_DlgLogonLogo->ShowWindow(SW_HIDE);
	return true;
}

//读取帐号
void CDlgLogon::LoadAccountsInfo()
{
	if (2 == g_GlobalUnits.GetApplicationCmdLine().GetSize())
		return;

	//变量定义
	CRegKey RegUserInfo;
	RegUserInfo.Open(HKEY_CURRENT_USER,REG_USER_INFO,KEY_READ);
	//最近用户
	DWORD dwLastUserID=AfxGetApp()->GetProfileInt(REG_OPTION_LOGON,TEXT("LastUserID"),0L);
	if (dwLastUserID!=0L)
	{
		//读取信息
		if (RegUserInfo!=NULL)
		{
			int iInsertItem=CB_ERR;
			LONG lErrorCode=ERROR_SUCCESS;
			DWORD dwType=REG_SZ,dwIndex=0,dwUserDBID=0;
			TCHAR szUserIDBuffer[32]=TEXT(""),szTempBuffer[256]=TEXT("");
			do
			{
				//设置变量
				dwUserDBID=0;
				szTempBuffer[0]=0;
				szUserIDBuffer[0]=0;
				//读取游戏 ID
				DWORD dwBufferSize=sizeof(szUserIDBuffer);
				if (RegUserInfo.EnumKey(dwIndex++,szUserIDBuffer,&dwBufferSize,NULL)!=ERROR_SUCCESS)
				{
					break;
				}

				dwUserDBID=atol(szUserIDBuffer);
				if (dwUserDBID==0)
				{
					continue;
				}
				//加载其他信息
				CRegKey RegUserAccount;
				_snprintf(szTempBuffer,sizeof(szTempBuffer),TEXT("%s\\%ld"),REG_USER_INFO,dwUserDBID);
				RegUserAccount.Open(HKEY_CURRENT_USER,szTempBuffer,KEY_READ);
				if (RegUserAccount!=NULL)
				{
					//游戏帐号
					dwBufferSize=sizeof(szTempBuffer);
					lErrorCode=RegUserAccount.QueryValue(TEXT("UserAccount"),&dwType,szTempBuffer,&dwBufferSize);
					if ((lErrorCode==ERROR_SUCCESS)&&(szTempBuffer[0]!=0))
					{
						m_edUserAccounts.AddString(szTempBuffer);
						if (dwLastUserID == dwUserDBID)
						{
							m_edUserAccounts.SetWindowText(szTempBuffer);
							lErrorCode=RegUserAccount.QueryValue(TEXT("UserAccount"),&dwType,szTempBuffer,&dwBufferSize);
							RegUserAccount.QueryDWORDValue(TEXT("AcountsRule"),m_enRule);
							if ((lErrorCode==ERROR_SUCCESS)&&(szTempBuffer[0]!=0))
							{
								if (m_enRule == enAcountsRule_AccountsAndPass)
								{
									m_bRemPassword = true;
								}
								UpdateNetWorkTag(dwUserDBID);
								UpdateBackMusicTag(dwUserDBID);
								CString strValue ("");
								//HINSTANCE hInstance=g_GlobalUnits.m_PlatformResourceModule->GetResInstance();
								if (m_bRemPassword)
								{
									UpdateUserPassWord(dwUserDBID);
									GetImagePath(strValue);
									m_checkPassword.LoadRgnImage(TEXT(strValue+"BTCheckNo.png"),false);
									m_ToolTipCtrl.UpdateTipText(TEXT("取消记住密码"),&m_checkPassword);
								}
								else
								{
									UpdateUserPassWord(0L);
									GetImagePath(strValue);
									m_checkPassword.LoadRgnImage(TEXT(strValue+"BTCheckOff.png"), false);
									m_ToolTipCtrl.UpdateTipText(TEXT("记住密码"),&m_checkPassword);
								}
								m_checkPassword.Invalidate(FALSE);
							}
						}
					}
				}
			} while (true);
		}	
	}
	return;
}
void CDlgLogon::GetCurrentDirectoryPath(CString &szPath)
{
	TCHAR szResourcePath[MAX_PATH]={0};
	GetCurrentDirectory(MAX_PATH,szResourcePath);
	szPath.Append(szResourcePath);
}
void CDlgLogon::GetImagePath(CString& strRetValue)
{
	CString strModuleFileName("");
	GetCurrentDirectoryPath(strModuleFileName);
	strModuleFileName.Append("\\data\\logon\\");
	strRetValue = strModuleFileName;
}

void CDlgLogon::GetResPath(CString strClipName, CString &strResPath)
{
	CString strModuleFileName("");
	GetCurrentDirectoryPath(strModuleFileName);
	strModuleFileName.Append(strClipName);
	strResPath = strModuleFileName;
}
//加载服务器
void CDlgLogon::LoadLogonServer()
{
	//读取最近登录服务器
	CComboBox * pComBoxServer=(CComboBox *)GetDlgItem(IDC_SERVER);
	//m_strLogonServer=AfxGetApp()->GetProfileString(REG_OPTION_LOGON,TEXT("LogonServer"),NULL);
	//m_strLogonServer=L"192.168.1.108";//服务器ip
	m_strLogonServer=CInitData::s_cLoginServer;//服务器ip

	if (m_strLogonServer.IsEmpty()==FALSE)
	{
		pComBoxServer->AddString(m_strLogonServer);
		pComBoxServer->SetWindowText(m_strLogonServer);
	}

	//读取服务器列表
	CRegKey RegLogonServer;
	RegLogonServer.Open(HKEY_CURRENT_USER,REG_LOGON_SERVER,KEY_READ);
	if (RegLogonServer!=NULL)
	{
		TCHAR szLogonServer[128]=TEXT("");
		DWORD dwIndex=0,dwBufferSize=sizeof(szLogonServer);
		do
		{
			dwBufferSize=sizeof(szLogonServer);
			if (RegLogonServer.EnumKey(dwIndex++,szLogonServer,&dwBufferSize,NULL)!=ERROR_SUCCESS) break;
			if (szLogonServer[0]!=0)
			{
				if (m_strLogonServer.IsEmpty()) m_strLogonServer=szLogonServer;
				if (ComboBoxFindString(pComBoxServer,szLogonServer)==LB_ERR) pComBoxServer->AddString(szLogonServer);
			}
		} while (true);
	}

	//设置选择
	//if ((pComBoxServer->GetWindowTextLength()==0)&&(pComBoxServer->GetCount()>0)) pComBoxServer->SetCurSel(0);
	//if (pComBoxServer->GetCount()==0)
	//{
	//	pComBoxServer->AddString(TEXT("220.169.30.242"));
	//	pComBoxServer->SetCurSel(0);
	//} 
	return;
}

//代理信息
void CDlgLogon::LoadProxyServerInfo()
{
	//变量定义
	LPCTSTR szProxyType[]={TEXT("不使用代理"),TEXT("HTTP 代理"),TEXT("SOCKS 4"),TEXT("SOCKS 5")};
	enProxyServerType ProxyServerType[]={ProxyType_None,ProxyType_Http,ProxyType_Socks4,ProxyType_Socks5};

	//获取信息
	enProxyServerType CurrentProxyType=g_GlobalOption.m_ProxyServerType;
	const tagProxyServerInfo & ProxyServerInfo=g_GlobalOption.m_ProxyServerInfo;

	//加载类型
	for (INT i=0;i<CountArray(szProxyType);i++)
	{
		ASSERT(i<CountArray(ProxyServerType));
		INT nItem=m_cmProxyServerType.AddString(szProxyType[i]);
		m_cmProxyServerType.SetItemData(nItem,ProxyServerType[i]);
		if (CurrentProxyType==ProxyServerType[i]) m_cmProxyServerType.SetCurSel(nItem);
	}

	//设置信息
	SetDlgItemText(IDC_PROXY_USER,ProxyServerInfo.szUserName);
	SetDlgItemText(IDC_PROXY_PASS,ProxyServerInfo.szPassword);
	SetDlgItemText(IDC_PROXY_SERVER,ProxyServerInfo.szProxyServer);
	if (ProxyServerInfo.wProxyPort!=0) SetDlgItemInt(IDC_PROXY_PORT,ProxyServerInfo.wProxyPort);

	//设置选择
	if (m_cmProxyServerType.GetCurSel()==CB_ERR) m_cmProxyServerType.SetCurSel(0);

	return;
}

//查找数据
int CDlgLogon::ComboBoxFindString(CComboBox * pComboBox, LPCTSTR pszFindString)
{
	CString strBuffer;
	int iItemHandle=LB_ERR,iItemFind=LB_ERR;
	int nStringLen=lstrlen(pszFindString);
	do 
	{
		iItemFind=pComboBox->FindString((iItemHandle==LB_ERR)?0:(iItemHandle+1),pszFindString);
		if ((iItemFind==LB_ERR)||(iItemHandle==iItemFind)||(iItemFind<iItemHandle)) return LB_ERR;
		iItemHandle=iItemFind;
		pComboBox->GetLBText(iItemHandle,strBuffer);
		if (pComboBox->GetLBTextLen(iItemHandle)==nStringLen) return iItemHandle;
	} while (true);
	return LB_ERR;
}

//效验输入
bool CDlgLogon::CheckLogonInput(bool bShowError)
{
	//定义变量
	UINT uControlID=0;
	CString strBuffer;

	try
	{
		//屏幕像素
		RECT reWindowSize;
		GetDesktopWindow()->GetWindowRect(&reWindowSize);
		INT nHeight =reWindowSize.bottom-reWindowSize.top;
		INT nWidth =reWindowSize.right-reWindowSize.left;

		//判断像素
		if(nWidth<LESS_SCREEN_W || nHeight<LESS_SCREEN_H)
		{
			TCHAR szInfo[255]=TEXT("");
			_sntprintf(szInfo,CountArray(szInfo),TEXT("屏幕像素需要在%d*%d或以上才可以正常游戏!"),LESS_SCREEN_W,LESS_SCREEN_H);
			throw szInfo;
		}

		//登录服务器
		GetDlgItemText(IDC_SERVER,m_strLogonServer);
		m_strLogonServer.TrimLeft();
		m_strLogonServer.TrimRight();
		if (m_strLogonServer.IsEmpty())
		{
			uControlID=IDC_SERVER;
			throw TEXT("登录服务器不能为空，请重新选择或者输入登录服务器！");
		}

		//登录帐号
		m_edUserAccounts.GetWindowText(strBuffer);
		strBuffer.TrimLeft();
		strBuffer.TrimRight();
		if (strBuffer.IsEmpty())
		{
			uControlID=IDC_EDIT_ACCOUNTS;
			throw TEXT("用户登录帐号不能为空，请重新输入登录帐号！");
		}
		lstrcpyn(m_szAccounts,strBuffer,CountArray(m_szAccounts));

		//用户密码
		if (m_bChangePassWord==true)
		{
			m_bChangePassWord=false;
			GetDlgItemText(IDC_PASSWORD,m_szPassword,CountArray(m_szPassword));
		}
		if (m_szPassword[0]==0)
		{
			uControlID=IDC_PASSWORD;
			throw TEXT("登录密码不能为空，请重新输入登录密码！");
		}

		//代理类型
		CComboBox * pComProxyType=(CComboBox *)GetDlgItem(IDC_PROXY_TYPE);
		enProxyServerType ProxyServerType=(enProxyServerType)pComProxyType->GetItemData(pComProxyType->GetCurSel());
		//代理信息
		tagProxyServerInfo ProxyServerInfo;
		ZeroMemory(&ProxyServerInfo,sizeof(ProxyServerInfo));
		ProxyServerInfo.wProxyPort=GetDlgItemInt(IDC_PROXY_PORT);
		GetDlgItemText(IDC_PROXY_USER,ProxyServerInfo.szUserName,CountArray(ProxyServerInfo.szUserName));
		GetDlgItemText(IDC_PROXY_PASS,ProxyServerInfo.szPassword,CountArray(ProxyServerInfo.szPassword));
		GetDlgItemText(IDC_PROXY_SERVER,ProxyServerInfo.szProxyServer,CountArray(ProxyServerInfo.szProxyServer));
		//效验代理
		if (ProxyServerType!=ProxyType_None)
		{
			//代理地址
			if (ProxyServerInfo.szProxyServer[0]==0)
			{
				ShowInformation(TEXT("代理服务器地址不能为空，请重新输入！"),0,MB_ICONINFORMATION);
				if (m_bNetOption==false) SwitchNetOption(true);
				m_edProxyServer.SetFocus();
				return false;
			}

			//代理端口
			if (ProxyServerInfo.wProxyPort==0)
			{
				ShowInformation(TEXT("请输入代理服务器端口号，例如：1080！"),0,MB_ICONINFORMATION);
				if (m_bNetOption==false) SwitchNetOption(true);
				m_edProxyPort.SetFocus();
				return false;
			}
		}
		//保存配置
		g_GlobalOption.m_ProxyServerType=ProxyServerType;
		g_GlobalOption.m_ProxyServerInfo=ProxyServerInfo;
		return true;
	}
	catch (LPCTSTR pszError)
	{
		if (bShowError)
		{
			if (2 == g_GlobalUnits.GetApplicationCmdLine().GetSize())
			{
				EnableControlWindow(TRUE);
			}

			ShowWindow(SW_SHOW);
			BringWindowToTop();
			ShowInformation(pszError,0,MB_ICONQUESTION);
			if (uControlID!=0) GetDlgItem(uControlID)->SetFocus();
		}

	}
	return false;
}

//更新密码
void CDlgLogon::UpdateUserPassWord(DWORD dwUserDBID)
{
	if (dwUserDBID!=0L)
	{
		//注册表字符
		CString strRegKey;
		strRegKey.Format(TEXT("%s\\%ld"),REG_USER_INFO,dwUserDBID);

		//打开注册表
		CRegKey RegPassWord;
		RegPassWord.Open(HKEY_CURRENT_USER,strRegKey,KEY_READ);
		if (RegPassWord!=NULL)
		{
			DWORD dwType=REG_SZ;
			TCHAR szPassBuffer[256]=TEXT("");
			DWORD dwBufferLength=sizeof(szPassBuffer);
			if (RegPassWord.QueryValue(TEXT("UserPassword"),&dwType,szPassBuffer,&dwBufferLength)==ERROR_SUCCESS)
			{
				m_szPassword[0]=0;
				CXOREncrypt::CrevasseData(szPassBuffer,m_szPassword,CountArray(m_szPassword));
				if (m_szPassword[0]==0) SetDlgItemText(IDC_PASSWORD,TEXT(""));
				else SetDlgItemText(IDC_PASSWORD,TEXT("**********"));
				m_bChangePassWord=false;
				return;
			}
		}
	}

	//设置信息
	m_szPassword[0]=0;
	SetDlgItemText(IDC_PASSWORD,TEXT(""));
	m_bChangePassWord=false;

	return;
}

//更新网络标志
void CDlgLogon::UpdateNetWorkTag(DWORD dwUserDBID)
{
	if (dwUserDBID!=0L)
	{
		//注册表字符
		CString strRegKey;
		strRegKey.Format(TEXT("%s\\%ld"),REG_USER_INFO,dwUserDBID);

		//打开注册表
		CRegKey RegPassWord;
		RegPassWord.Open(HKEY_CURRENT_USER,strRegKey,KEY_READ);
		if (RegPassWord!=NULL)
		{
			DWORD dwNetTag = 0;
			if (RegPassWord.QueryValue(dwNetTag, TEXT("UserNetWork"))==ERROR_SUCCESS)
			{
				if (0 == dwNetTag)
				{
					OnEnTelecomMsg();
				}
				else if (1 == dwNetTag)
				{
					OnEnMeshWorkMsg();
				}
				else
				{
					OnEnAutoMsg();
				}
				return;
			}
		}
	}

	//设置信息
	OnEnAutoMsg();
}

//更新播放背景音乐标识
void CDlgLogon::UpdateBackMusicTag(DWORD dwUserDBID)
{
	if (dwUserDBID!=0L)
	{
		//注册表字符
		CString strRegKey;
		strRegKey.Format(TEXT("%s\\%ld"),REG_USER_INFO,dwUserDBID);

		//打开注册表
		CRegKey RegPassWord;
		RegPassWord.Open(HKEY_CURRENT_USER,strRegKey,KEY_READ);
		if (RegPassWord!=NULL)
		{
			DWORD dwNetTag = 0;
			if (RegPassWord.QueryValue(dwNetTag, TEXT("BackMusicTag"))==ERROR_SUCCESS)
			{
				if (0 == dwNetTag)
				{
					//设置信息
					g_GlobalOption.m_bEnableBackMusic = false;
				}
				else
				{
					//设置信息
					g_GlobalOption.m_bEnableBackMusic = true;
				}
				return;
			}
		}
	}

	//设置信息
	g_GlobalOption.m_bEnableBackMusic = true;
}

//代理模式
void CDlgLogon::SwitchNetOption(bool bNetOption)
{
	//设置变量
	m_bNetOption=bNetOption;

	//控件设置
	GetDlgItem(IDC_PROXY_TYPE)->EnableWindow(m_bNetOption);
	GetDlgItem(IDC_PROXY_PORT)->EnableWindow(m_bNetOption);
	GetDlgItem(IDC_PROXY_USER)->EnableWindow(m_bNetOption);
	GetDlgItem(IDC_PROXY_PASS)->EnableWindow(m_bNetOption);
	GetDlgItem(IDC_PROXY_TEST)->EnableWindow(m_bNetOption);
	GetDlgItem(IDC_PROXY_SERVER)->EnableWindow(m_bNetOption);

	//移动位置
	INT nHeight=m_bNetOption?m_nFullHeight:m_rcNormalFrame.Height();
	//SetWindowPos(NULL,0,0,m_nFullWidth,nHeight,SWP_NOMOVE|SWP_NOZORDER|SWP_NOCOPYBITS);

	return;
}

//密码改变
void CDlgLogon::OnEnChangePassword()
{
	m_bChangePassWord=true;
}

//记住密码
void CDlgLogon::OnBnClickCheckPassword()
{
	m_bRemPassword = m_bRemPassword?false:true;
	//HINSTANCE hInstance=g_GlobalUnits.m_PlatformResourceModule->GetResInstance();
	CString strValue("");
	GetImagePath(strValue);
	if (m_bRemPassword)
	{
		m_checkPassword.LoadRgnImage(TEXT(strValue+"BTCheckNo.png"), false);
		//m_checkPassword.LoadRgnImage(hInstance,g_GlobalUnits.m_PlazaViewImage.uBtCheckNo);
		m_ToolTipCtrl.UpdateTipText(TEXT("取消记住密码"),&m_checkPassword);
	}
	else
	{
		m_checkPassword.LoadRgnImage(TEXT(strValue+"BTCheckOff.png"), false);
		//m_checkPassword.LoadRgnImage(hInstance,g_GlobalUnits.m_PlazaViewImage.uBtCheckOff);
		m_ToolTipCtrl.UpdateTipText(TEXT("记住密码"),&m_checkPassword);
	}
	m_checkPassword.Invalidate(FALSE);

}
//游戏帮助
void CDlgLogon::OnBnClickGameHelp()
{
	ShellExecute(NULL,   "open",   CInitData::s_cGameHelp,     ""   ,NULL,   SW_SHOW);
}
//游戏商场
void CDlgLogon::OnBnClickGameShop()
{
	//ShellExecute(NULL,   "open",   WEB_DEPOSIT_LIST,     ""   ,NULL,   SW_SHOW);
}
//客户中心
void CDlgLogon::OnBnClickGameClient()
{
	ShellExecute(NULL,   "open",   CInitData::s_cContactService,     ""   ,NULL,   SW_SHOW);

}

//软键盘
void CDlgLogon::OnBnClickKey()
{
	m_bIsKeyDlgShow = false;
	if (!::IsWindow(m_keydlg.GetSafeHwnd()))
	{
		m_keydlg.Create(CKeyDialog::IDD, this);
		m_keydlg.m_hParent = GetSafeHwnd();
		UpdateKeyPos();
		m_btnKey.SetSelect(TRUE);
		m_btnKey.Invalidate(FALSE);
		m_bIsKeyDlgShow = true;
	}
	else
	{
		m_keydlg.OnOK();
		m_btnKey.SetSelect(FALSE);
		m_btnKey.Invalidate(FALSE);
	}
}

//隐藏键盘
void CDlgLogon::SetKeyHide()
{
	m_bIsKeyDlgShow = false;
}

void CDlgLogon::UpdateKeyPos()
{
	if (!::IsWindow(m_keydlg.GetSafeHwnd()))
	{
		return;
	}
	CRect rect, rcTemp;
	::GetWindowRect(GetDlgItem(IDC_PASSWORD)->GetSafeHwnd(), rect);
	//m_btnKey.GetWindowRect(rect);
	m_keydlg.GetWindowRect(rcTemp);
	rcTemp.MoveToXY(rect.left - 120, rect.bottom + 10);
	::SetWindowPos(m_keydlg.GetSafeHwnd(), HWND_TOP, rcTemp.left, rcTemp.top, 0, 0, SWP_NOSIZE | SWP_SHOWWINDOW);
}

//检测弹出的窗口
void CDlgLogon::OnPopRegisterDlg()
{
	if (m_bRegister)
	{
		OnRegisterAccounts();
	}
}

//注册帐号
void CDlgLogon::OnRegisterAccounts()
{
	//#ifndef _DEBUG
	//	ShellExecute(NULL,TEXT("open"),TEXT("http://qp.foxuc.net/service/viewrule.jsp"),NULL,NULL,SW_SHOWDEFAULT);
	//	return;
	//#endif
	//cxf 打开注册网站
	ShellExecute(NULL,   "open",   CInitData::s_cLogonRegister,     ""   ,NULL,   SW_SHOW);
	/* cxf 
	CDlgRegister DlgRegister;
	//ShowWindow(SW_HIDE);
	if (DlgRegister.DoModal()!=IDOK) 
	{
	//ShowWindow(SW_SHOW);
	return;
	}

	//设置变量
	m_bRegister=true;
	m_wFaceID=DlgRegister.m_wFaceID;
	m_cbGender=DlgRegister.m_cbGender;
	lstrcpy(m_szSpreader,DlgRegister.m_szSpreader);
	lstrcpy(m_szAccounts,DlgRegister.m_szAccounts);
	lstrcpy(m_szPassword,DlgRegister.m_szPassword);
	lstrcpy(m_szMail,DlgRegister.m_szMail);
	lstrcat(m_szMail, TEXT("@"));
	lstrcat(m_szMail, DlgRegister.m_szMailSuffix);

	//服务器
	GetDlgItemText(IDC_SERVER,m_strLogonServer);
	if (m_strLogonServer.IsEmpty()) m_strLogonServer=TEXT("qp.foxuc.net");

	//登录广场
	//ShowWindow(SW_HIDE);
	//d登录登录等-------------------------------------------------------------------------------------
	IPlazaViewItem * pIPlazaViewItem=(CPlazaDlg*)AfxGetMainWnd();
	ASSERT(pIPlazaViewItem!=NULL);
	pIPlazaViewItem->SendConnectMessage();
	*/
	return;
}

//删除用户
void CDlgLogon::OnDeleteAccounts() 
{
	/*cxf 1024//删除信息
	CString strBuffer;
	m_edUserAccounts.GetWindowText(strBuffer);
	m_edUserAccounts.SetWindowText(TEXT(""));
	if (strBuffer.IsEmpty()) return;

	//最近用户
	DWORD dwLastUserID=AfxGetApp()->GetProfileInt(REG_OPTION_LOGON,TEXT("LastUserID"),0L);
	CRegKey RegUserID;
	strBuffer.Format(TEXT("%d"), dwLastUserID);
	RegUserID.Open(HKEY_CURRENT_USER,REG_USER_INFO);
	if (RegUserID!=NULL) 
	{
	RegUserID.RecurseDeleteKey(strBuffer);
	}
	m_edUserAccounts.SetFocus();
	UpdateUserPassWord(dwLastUserID);
	AfxGetApp()->WriteProfileInt(REG_OPTION_LOGON,TEXT("LastUserID"),0L);
	return;
	*/

	CRegKey RegUserID;
	RegUserID.Open(HKEY_CURRENT_USER,REG_USER_INFO);
	if (RegUserID!=NULL) 
	{
		do 
		{
			TCHAR szUserIDBuffer[16]={""};
			DWORD dwBufferSize=sizeof(szUserIDBuffer);
			if (RegUserID.EnumKey(0,szUserIDBuffer,&dwBufferSize,NULL)!=ERROR_SUCCESS)
			{
				break;
			}
			RegUserID.RecurseDeleteKey(szUserIDBuffer);

		} while (true);
	}
	m_edUserAccounts.SetFocus();
	m_edUserAccounts.SetWindowText(TEXT(""));
	m_edUserPassword.SetWindowText(TEXT(""));
	return;
}

//网络设置
void CDlgLogon::OnBnClickedNetOption()
{
	//设置模式
	SwitchNetOption(!m_bNetOption);	 
	return;
}

//代理测试
void CDlgLogon::OnBnClickedProxyTest()
{
	//代理类型
	CComboBox * pComProxyType=(CComboBox *)GetDlgItem(IDC_PROXY_TYPE);
	enProxyServerType ProxyServerType=(enProxyServerType)pComProxyType->GetItemData(pComProxyType->GetCurSel());

	//代理信息
	tagProxyServerInfo ProxyServerInfo;
	ZeroMemory(&ProxyServerInfo,sizeof(ProxyServerInfo));
	ProxyServerInfo.wProxyPort=GetDlgItemInt(IDC_PROXY_PORT);
	GetDlgItemText(IDC_PROXY_USER,ProxyServerInfo.szUserName,CountArray(ProxyServerInfo.szUserName));
	GetDlgItemText(IDC_PROXY_PASS,ProxyServerInfo.szPassword,CountArray(ProxyServerInfo.szPassword));
	GetDlgItemText(IDC_PROXY_SERVER,ProxyServerInfo.szProxyServer,CountArray(ProxyServerInfo.szProxyServer));

	//效验代理
	if (ProxyServerType!=ProxyType_None)
	{
		//代理地址
		if (ProxyServerInfo.szProxyServer[0]==0)
		{
			ShowInformation(TEXT("代理服务器地址不能为空，请重新输入！"),0,MB_ICONINFORMATION);
			if (m_bNetOption==false) SwitchNetOption(true);
			m_edProxyServer.SetFocus();
			return;
		}

		//代理端口
		if (ProxyServerInfo.wProxyPort==0)
		{
			ShowInformation(TEXT("请输入代理服务器端口号，例如：1080！"),0,MB_ICONINFORMATION);
			if (m_bNetOption==false) SwitchNetOption(true);
			m_edProxyPort.SetFocus();
			return;
		}
	}
	else 
	{
		//错误提示
		ShowInformation(TEXT("请先配置代理服务器连接信息！"),0,MB_ICONINFORMATION);
		pComProxyType->SetFocus();
		return;
	}

	//创建组件
	CNetworkManagerHelper NetworkManagerModule;
	if (NetworkManagerModule.CreateInstance()==false)
	{
		ShowInformation(TEXT("网络服务管理组件创建失败，测试失败！"),0,MB_ICONINFORMATION);
		return;
	}

	try
	{
		//代理测试
		DWORD dwSuccess=NetworkManagerModule->ProxyServerTesting(ProxyServerType,ProxyServerInfo);

		//结果提示
		if (dwSuccess==CONNECT_SUCCESS)
		{
			ShowInformation(TEXT("代理服务器工作正常！"),0,MB_ICONINFORMATION);
			return;
		}
		else throw TEXT("无法连接代理服务器！");
	}
	catch (LPCTSTR pszDescribe)
	{
		ShowInformation(pszDescribe,0,MB_ICONINFORMATION);
		return;
	}

	return;
}

//边框
void CDlgLogon::OnNcPaint()
{
	// CPaintDC dc(this); `

}
//重画消息
void CDlgLogon::OnPaint()
{
	/* cxf CPaintDC dc(this); 
	CRect ClientRect;
	GetClientRect(&ClientRect);

	//dc.FillSolidRect(ClientRect,RGB(200,200,10)); 
	//DrawPic(&dc,IDB_LOGON,&ClientRect);

	{
	CBitmap  m_bmp;
	if( m_bmp.m_hObject != NULL )
	m_bmp.DeleteObject(); 

	HBITMAP hbmp = (HBITMAP)::LoadImage(AfxGetInstanceHandle(), 
	MAKEINTRESOURCE(IDB_LOGON), IMAGE_BITMAP, 0, 0, LR_LOADMAP3DCOLORS);         
	m_bmp.Attach( hbmp );
	CBrush brush;
	brush.CreatePatternBrush(&m_bmp);		 
	dc.FillRect(ClientRect,&brush);	  
	m_bmp.DeleteObject();
	brush.DeleteObject();
	}*/
	//cxf
	CPaintDC dc(this); 
	CDC	memdc;
	CRect rect;
	CBitmap bmp;

	GetClientRect(&rect);
	bmp.CreateCompatibleBitmap(&dc, 640, 420);
	memdc.CreateCompatibleDC(&dc);
	memdc.SelectObject(bmp);
	memdc.SetBkMode(TRANSPARENT);
	memdc.FillSolidRect(0, 0, 640, 420, RGB(0, 0, 0));
	memdc.SetTextColor(RGB(0, 0, 0));
	if (!m_pngLogon.IsNull())
	{
		m_pngLogon.DrawImage(&memdc, 0, 0);
	}
	dc.BitBlt(0, 0, rect.Width(), rect.Height(), &memdc, 0, 0, SRCCOPY);
	bmp.DeleteObject();
	memdc.DeleteDC();
	//end

	//获取位置
	//	CRect ClientRect;
	//	GetClientRect(&ClientRect);
	//
	//	//绘画标题
	//	//DrawSkinView(&dc);
	//
	////	绘画背景
	//	int nXExcursion=GetXExcursionPos();
	//	int nYExcursion=GetYExcursionPos();
	//	dc.FillSolidRect(nXExcursion,nYExcursion,ClientRect.Width()-2*nXExcursion,4,m_SkinAttribute.m_crBackFrame);
	//	dc.FillSolidRect(nXExcursion,ClientRect.Height()-8,ClientRect.Width()-2*nXExcursion,7,m_SkinAttribute.m_crBackFrame);
	//	dc.FillSolidRect(nXExcursion,nYExcursion+4,12,ClientRect.Height()-nYExcursion-9,m_SkinAttribute.m_crBackFrame);
	//	dc.FillSolidRect(ClientRect.Width()-nXExcursion-12,nYExcursion+4,12,ClientRect.Height()-nYExcursion-9,m_SkinAttribute.m_crBackFrame);
	//	dc.FillSolidRect(nXExcursion,119,ClientRect.Width()-2*nXExcursion,5,m_SkinAttribute.m_crBackFrame);
	//
	////	创建画笔
	//	CPen BorderPen(PS_SOLID,1,m_SkinAttribute.m_crInsideBorder);
	//	CPen * pOldPen=dc.SelectObject(&BorderPen);
	//	dc.SelectObject(m_SkinAttribute.m_brBackGround);
	//
	////	绘画内框
	//	CRect rcDrawRect(nXExcursion,nYExcursion,ClientRect.Width()-2*nXExcursion,ClientRect.Height());
	//	if (m_bMaxSize==true) rcDrawRect.DeflateRect(3,3,3,2);
	//	else rcDrawRect.DeflateRect(3,4,2,4);
	//	rcDrawRect.top=123;
	//	if(m_bNetOption)rcDrawRect.bottom=368;
	//	else rcDrawRect.bottom=284;
	//	dc.RoundRect(&rcDrawRect,CPoint(8,8));
	//
	////	释放资源
	//	dc.SelectObject(pOldPen);
	//	BorderPen.DeleteObject();

	return;
}

//获取代理
void CDlgLogon::GetProxyInfo(enProxyServerType &ProxyServerType, tagProxyServerInfo &ProxyServerInfo)
{
	//代理类型
	CComboBox * pComProxyType=(CComboBox *)GetDlgItem(IDC_PROXY_TYPE);
	ProxyServerType=(enProxyServerType)pComProxyType->GetItemData(pComProxyType->GetCurSel());

	//代理信息
	ZeroMemory(&ProxyServerInfo,sizeof(ProxyServerInfo));
	ProxyServerInfo.wProxyPort=GetDlgItemInt(IDC_PROXY_PORT);
	GetDlgItemText(IDC_PROXY_USER,ProxyServerInfo.szUserName,CountArray(ProxyServerInfo.szUserName));
	GetDlgItemText(IDC_PROXY_PASS,ProxyServerInfo.szPassword,CountArray(ProxyServerInfo.szPassword));
	GetDlgItemText(IDC_PROXY_SERVER,ProxyServerInfo.szProxyServer,CountArray(ProxyServerInfo.szProxyServer));

	//效验代理
	if (ProxyServerType!=ProxyType_None)
	{
		//代理地址
		if (ProxyServerInfo.szProxyServer[0]==0)
		{
			if (m_bNetOption==false) SwitchNetOption(true);
			m_edProxyServer.SetFocus();
			throw TEXT("代理服务器地址不能为空，请重新输入！");
		}

		//代理端口
		if (ProxyServerInfo.wProxyPort==0)
		{

			if (m_bNetOption==false) SwitchNetOption(true);
			m_edProxyPort.SetFocus();
			throw TEXT("请输入代理服务器端口号，例如：1080！");
		}
	}
}


void CDlgLogon::OnLButtonDown( UINT nFlags, CPoint point )
{
	if (!m_bIsKeyDlgShow)
	{
		CSkinDialogEx::OnLButtonDown(nFlags, point);
	}
}


void CDlgLogon::OnMove(int x, int y)
{
	CSkinDialogEx::OnMove(x, y);

	// TODO: 在此处添加消息处理程序代码
	if (!m_bIsKeyDlgShow && m_bIsLogoCreate)
	{
		if (NULL!=m_DlgLogonLogo && ::IsWindow(m_DlgLogonLogo->GetSafeHwnd()))
		{
			CRect crDlgLogon;
			GetWindowRect(&crDlgLogon);
			m_DlgLogonLogo->SetWindowPos(NULL,crDlgLogon.left-24,crDlgLogon.top-126,350,260,0);
		}
	}
}

void CDlgLogon::OnWindowPosChanging(WINDOWPOS* lpwndpos)
{
	CSkinDialogEx::OnWindowPosChanging(lpwndpos);
}

//////////////////////////////////////////////////////////////////////////


void CDlgLogon::OnDestroy()
{
	CSkinDialogEx::OnDestroy();

	if (NULL != m_DlgLogonLogo)
	{
		delete m_DlgLogonLogo;
	}
	m_DlgLogonLogo = NULL;
	// TODO: 在此处添加消息处理程序代码
}

LRESULT CDlgLogon::OnKeyDownDlg(WPARAM wParam, LPARAM lParam)
{
	/*
	CString str;
	TCHAR ch = (TCHAR)wParam;
	GetDlgItemText(IDC_PASSWORD, str);
	if (ch == VK_CAPITAL)
	{
	//删除
	SetDlgItemText(IDC_PASSWORD, str);
	}else
	{
	TCHAR ch2[2]={'\0'};
	ch2[0]=ch;
	str+=ch2;
	SetDlgItemText(IDC_PASSWORD, str);
	}*/
	return GetDlgItem(IDC_PASSWORD)->SendMessage(WM_CHAR, wParam, lParam);
}
//void CDlgLogon::OnMoving(UINT fwSide, LPRECT pRect)
//{
//	CSkinDialogEx::OnMoving(fwSide, pRect);
//
//	// TODO: 在此处添加消息处理程序代码
//	UpdateKeyPos();
//}



BOOL CDlgLogon::SetImageRgn(CPngImage * pImage)
{

	/*if (NULL==pImage) return FALSE;
	if (pImage->IsNull()) return FALSE;
	INT wWidth = pImage->GetWidth();
	INT wHeight = pImage->GetHeight();
	CBitmap * pbmp;
	CImage imgBuf;
	imgBuf.Create(wWidth, wHeight, 32);
	CDC *pDCTmp = CDC::FromHandle(imgBuf.GetDC());
	pImage->DrawImage(pDCTmp, 0, 0, wWidth, wHeight, 0, 0, wWidth, wHeight);
	pbmp = ((CBitmap *)(&imgBuf));
	DWORD * pData = new DWORD[wWidth * wHeight];
	pbmp->GetBitmapBits((sizeof (DWORD)* wWidth * wHeight), pData);
	imgBuf.ReleaseDC();
	CRgn rgnImage, tempRgn;
	rgnImage.CreateRectRgn(0, 0, 0, 0);
	DWORD *pDataTmp = pData;
	for (UINT h = 0;h < wHeight;++h)
	{
		UINT w = 0, leftX = 0;
		while(w < wWidth)
		{
			while (w++ < wWidth &&(0 == (*pDataTmp++ & 0xff000000)));
			leftX =(w - 1);
			while (w++ < wWidth &&(0 != (*pDataTmp++ & 0xff000000))); 
			tempRgn.CreateRectRgn(leftX, h,(w - 1),(h + 1));
			rgnImage.CombineRgn(&rgnImage, &tempRgn, RGN_OR);
			tempRgn.DeleteObject();
		}
	}

	SafeDeleteArray(pData);
	imgBuf.Destroy();

	if (NULL == rgnImage.m_hObject)
		return FALSE;

	::SetWindowRgn(m_hWnd, (HRGN)rgnImage.m_hObject, TRUE);

	rgnImage.DeleteObject();

	return TRUE;*/


//	if (!pImage) 
//		return FALSE;
//	if (pImage->IsNull()) 
//		return FALSE;
//
//	CBitmap* pBmp;
//	CImage imgBuf;
//
//	int iWidth = pImage->GetWidth();
//	int iHeight = pImage->GetHeight();
//	int iCount = 0;
//
//	imgBuf.Create(iWidth, iHeight, 32);
//	CDC *pDCTmp = CDC::FromHandle(imgBuf.GetDC());	// 缺少成对出现的 CDC::DeleteTempMap  ???
//	pImage->DrawImage(pDCTmp, 0, 0, iWidth, iHeight, 0, 0, iWidth, iHeight);
//	pBmp = ((CBitmap *)(&imgBuf));
//	DWORD * pData = new DWORD[iWidth * iHeight];
//	pBmp->GetBitmapBits((sizeof (DWORD)* iWidth * iHeight),  pData);
//
//	CRgn rgnImage;			// pImage 的 镜像
//	CRgn rgnDecrease;		// 要剪掉的区域
//
//	rgnImage.CreateRectRgn(0,  0,  iWidth,  iHeight);
//	rgnDecrease.CreateRectRgn(0,  0,  iWidth,  iHeight);
//
//	DWORD *pDataTmp = pData;
//
//	bool		bStart = true;	// 可以做起点
//	CPoint	ptStartTrans;		// 透明的起点
//	CPoint	ptEndTrans;			// 透明的终点
//	int		iPix;		
//	int		iPhalanx = 0;		// 方阵的点
//
//	for (int h=0; h<iHeight; ++h)	// 高度
//	{
//		iPhalanx = iWidth * h;
//		bStart = true;				// 可以做起点
//		ptStartTrans.SetPoint(0, h);
//		ptEndTrans.SetPoint(0, h);
//		for(int w=0;  w<iWidth;  w++, pDataTmp++)
//		{
//			iPix = (*pDataTmp) & 0xff000000;
//			if(0 == iPix)	// 完全透明
//			{
//				if(bStart)
//				{
//					ptStartTrans.SetPoint(w, h);
//					bStart = false;	// 不能做起点
//				}
//				// 最后一个起点透明 ,直接裁剪
//				if(w==(iWidth-1))
//				{
//					// 裁剪，透明会比非透明的少，所以裁剪透明比叠加非透明会更有效率 ...... 
//					rgnDecrease.SetRectRgn(ptEndTrans.x+2, ptEndTrans.y,  iWidth,  ptStartTrans.y+1);	// 要裁剪的区域
//					rgnImage.CombineRgn(&rgnImage, &rgnDecrease, RGN_XOR);	// 两个剪裁区域的全部除去公共部分		
//					iCount++;
//				}
//			}
//			else	// 不完全透明
//			{
//				ptEndTrans.SetPoint(w-1, h);
//
//				// 安全距离并且
//				if(ptStartTrans.x <= ptEndTrans.x)
//				{
//					iPix = pData[iPhalanx + ptEndTrans.x ] & 0xff000000;
//					if (0==iPix)	// 该点透明
//					{
//						// 裁剪，透明会比非透明的少，所以裁剪透明比叠加非透明会更有效率 ...... 
//						rgnDecrease.SetRectRgn(ptStartTrans.x, ptStartTrans.y,  ptEndTrans.x,  ptEndTrans.y+1);	// 要裁剪的区域
//						rgnImage.CombineRgn(&rgnImage, &rgnDecrease, RGN_XOR);	// 	两个剪裁区域的全部除去公共部分	
//						iCount++;
//					}
//				}
//				bStart = true;
//			}
//		}
//	}
//
//#if _DEBUG
//	CString strTmp;
//	strTmp.Format("iCount=%d \r\n", iCount);
//	TRACE(strTmp.GetBuffer());
//#endif
//
//	SafeDeleteArray(pData);
//	imgBuf.ReleaseDC();
//	imgBuf.Destroy();
//
//	BOOL bResult = TRUE;
//
//	if (rgnImage.m_hObject)
//	{
//		::SetWindowRgn(m_hWnd, (HRGN)rgnImage.m_hObject, TRUE);
//		DeleteObject(rgnImage);
//	}
//	else
//	{
//		bResult = FALSE;
//	}
//
//	DeleteObject(rgnDecrease);
//
//	CDC::DeleteTempMap();	// add by HouGuoJiang 2011-11-25
//
//	return bResult;


	if (NULL==pImage) return FALSE;
	if (pImage->IsNull()) return FALSE;
	INT wWidth = pImage->GetWidth();
	INT wHeight = pImage->GetHeight();
	CBitmap * pbmp;
	CImage imgBuf;
	imgBuf.Create(wWidth, wHeight, 32);
	CDC *pDCTmp = CDC::FromHandle(imgBuf.GetDC());
	pImage->DrawImage(pDCTmp, 0, 0, wWidth, wHeight, 0, 0, wWidth, wHeight);
	pbmp = ((CBitmap *)(&imgBuf));
	DWORD * pData = new DWORD[wWidth * wHeight];
	pbmp->GetBitmapBits((sizeof (DWORD)* wWidth * wHeight), pData);
	imgBuf.ReleaseDC();
	imgBuf.Destroy();

	CRgn rgnImage, tempRgn;
	rgnImage.CreateRectRgn(0, 0, 0, 0);
	tempRgn.CreateRectRgn(0, 0, wWidth, wHeight);
	DWORD *pDataTmp = pData;
	WORD wAreaW=30,wAreaH=30;
	if (wAreaW > wWidth || wAreaH > wHeight)return false;

	//透明角
	for (UINT i=0; i<wHeight; ++i)
	{
		UINT j=0,k=wWidth;
		while (j++ < wAreaW && (0 == (pData[(i*wWidth)+j] & 0xff000000)));

		while (k-- > wWidth-wAreaW && (0 == (pData[(i*wWidth)+k] & 0xff000000)));

		tempRgn.SetRectRgn(j, i, k, (i + 1));
		rgnImage.CombineRgn(&rgnImage, &tempRgn, RGN_OR);
	}

	DeleteObject(tempRgn);

	SafeDeleteArray(pData);

	if (NULL == rgnImage.m_hObject) return FALSE;

	::SetWindowRgn(m_hWnd, (HRGN)rgnImage.m_hObject, TRUE);
	DeleteObject(rgnImage);
	CDC::DeleteTempMap();

	return TRUE;


}


BOOL CDlgLogon::OnCommand(WPARAM wParam, LPARAM lParam)
{
	// TODO: 在此添加专用代码和/或调用基类
	return CSkinDialogEx::OnCommand(wParam, lParam);
}

void CDlgRegister::PreSubclassWindow()
{
	// TODO: 在此添加专用代码和/或调用基类

	CSkinDialogEx::PreSubclassWindow();
}

BOOL CDlgRegister::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if((pMsg->message==WM_KEYDOWN)
		&& (pMsg->wParam==VK_RETURN))
	{
		OnOK();
		return TRUE;
	}
	return CSkinDialogEx::PreTranslateMessage(pMsg);
}

void CDlgLogon::OnSize(UINT nType, int cx, int cy)
{
	CSkinDialogEx::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
	SetNCRect(cy);
}

void CDlgLogon::OnSysCommand(UINT nID, LPARAM lParam)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CSkinDialogEx::OnSysCommand(nID, lParam);
}



void CDlgLogon::OnWindowPosChanged(WINDOWPOS* lpwndpos)
{
	CSkinDialogEx::OnWindowPosChanged(lpwndpos);

	// TODO: 在此处添加消息处理程序代码
}

//刷新控件
void CDlgLogon::InvalidateRectControl()
{
	InvalidateRect(NULL);
	m_btRegister.InvalidateRect(NULL);
	m_btVisitWeb.InvalidateRect(NULL);
}

void CDlgLogon::OnTimer(UINT nIDEvent)
{
	switch (nIDEvent)
	{
	case TIMER_WEB_LOADING:					//加载网页
		{
			KillTimer(TIMER_WEB_LOADING);
			m_LogonAdvertisement.Navigate(CInitData::s_cLogonAdverTisement, NULL, NULL, NULL, NULL);
			m_LogonNotice.Navigate(CInitData::s_cLogonNotice,NULL,NULL,NULL,NULL);
		}
		break;
	case TIME_ID_UPDATEPROGRAM_LOGON:
		{
			if (IsWindowVisible())
			{
				KillTimer(TIME_ID_UPDATEPROGRAM_LOGON);

				CString strNewAccount = g_GlobalUnits.GetApplicationCmdLine().GetAt(0);
				m_edUserAccounts.InsertString(0, strNewAccount);
				m_edUserAccounts.SetCurSel(0);
				m_edUserPassword.SetWindowText(TEXT("**********"));

				OnOK();
			}
			break;
		}
	}
}
LRESULT CDlgLogon::OnCallInnoIssMsgSelectGame(WPARAM wParam, LPARAM lParam)
{
	((CGamePlazaApp*)AfxGetApp())->CallInnoIssMsgSelectGame(wParam,lParam);
	return 1;
}