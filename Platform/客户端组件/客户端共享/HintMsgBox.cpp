#include "StdAfx.h"
#include "Resource.h"
#include "HintMsgBox.h"

//////////////////////////////////////////////////////////////////////////

//时间标识
#define IDI_MESSAGE					100									//消息标识

//窗口位置
#define SMB_WINDOW_WIDTH			230									//最小宽度

//图标偏移
#define SMB_ICON_LEFT				20									//图标偏移
#define SMB_ICON_WIDTH				32									//图标宽度
#define SMB_ICON_HEIGHT				32									//图标高度

//字符偏移
#define SMB_STRING_TOP				30									//字符偏移
#define SMB_STRING_LEFT				20									//字符偏移
#define SMB_STRING_RIGHT			40									//字符偏移
#define SMB_STRING_WIDTH			600									//最大宽度
#define SMB_STRING_HEIGHT			600									//最大宽度

//按钮偏移
#define SMB_BUTTON_TOP				20									//按钮偏移
#define SMB_BUTTON_LEFT				12									//按钮偏移
#define SMB_BUTTON_RIGHT			12									//按钮偏移
#define SMB_BUTTON_BUTTOM			13									//按钮偏移
#define SMB_BUTTON_SPACE			10									//按钮间隔
#define SMB_BUTTON_WIDTH			81									//按钮偏移
#define SMB_BUTTON_HEIGHT			34									//按钮偏移

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CHintMsgBox, CSkinDialogEx)
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDYES,&OnBnClickedYes) 
	ON_BN_CLICKED(IDNO,&OnBnClickedNo) 
	ON_BN_CLICKED(IDABORT,&OnBnClickedAbort) 
	ON_BN_CLICKED(IDRETRY,&OnBnClickedRetry) 
	ON_BN_CLICKED(IDIGNORE,&OnBnClickedIgnore) 
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_WM_CREATE()
END_MESSAGE_MAP()
//////////////////////////////////////////////////////////////////////////

//构造函数
CHintMsgBox::CHintMsgBox(CWnd * pParentWnd) : CSkinDialogEx(CHintMsgBox::IDD,pParentWnd)
{
	//设置变量
	m_uType=MB_OK;

	//辅助变量
	m_nElapse=0;
	m_nBtCount=0;
	m_hResInstance=NULL;

	return;
}

//析构函数
CHintMsgBox::~CHintMsgBox()
{
}
//建立消息
int CHintMsgBox::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (__super::OnCreate(lpCreateStruct)==-1)
	{
		return -1;
	}
	SetWindowPos(NULL,0,0,0,0,SWP_FRAMECHANGED|SWP_NOOWNERZORDER| 
		SWP_NOMOVE|SWP_NOSIZE);

	return 0;
}
//初始化消息
BOOL CHintMsgBox::OnInitDialog()
{
	__super::OnInitDialog();

	//设置标题
	SetWindowText(m_strCaption);
	
	HINSTANCE hinstance = GetModuleHandle(CLIENT_SHARE_DLL_NAME);
	m_imgMsgBox_LeftTop.SetLoadInfo(IDB_MsgBox_LeftTop, hinstance, false);
	m_imgMsgBox_Left.SetLoadInfo(IDB_MsgBox_Left, hinstance, false);
	m_imgMsgBox_LeftBottom.SetLoadInfo(IDB_MsgBox_LeftBottom, hinstance, false);
	m_imgMsgBox_Top.SetLoadInfo(IDB_MsgBox_Top, hinstance, false);
	m_imgMsgBox_RightTop.SetLoadInfo(IDB_MsgBox_RightTop, hinstance, false);
	m_imgMsgBox_Right.SetLoadInfo(IDB_MsgBox_Right, hinstance, false);
	m_imgMsgBox_RightBottom.SetLoadInfo(IDB_MsgBox_RightBottom, hinstance, false);
	m_imgMsgBox_Bottom.SetLoadInfo(IDB_MsgBox_Bottom, hinstance, false);
	m_imgMsgBox_Center.SetLoadInfo(IDB_MsgBox_Center, hinstance, false);
	
	CImageHandle hImage(&m_imgMsgBox_LeftTop);
	CImageHandle hImage1(&m_imgMsgBox_Left);
	CImageHandle hImage2(&m_imgMsgBox_LeftBottom);
	CImageHandle hImage3(&m_imgMsgBox_Top);
	CImageHandle hImage4(&m_imgMsgBox_RightTop);
	CImageHandle hImage5(&m_imgMsgBox_Right);
	CImageHandle hImage6(&m_imgMsgBox_RightBottom);
	CImageHandle hImage7(&m_imgMsgBox_Bottom);
	CImageHandle hImage8(&m_imgMsgBox_Center);

	m_btnClose.Create(TEXT(""), WS_CHILD|WS_VISIBLE, CRect(0, 0, 14, 14), this, IDCANCEL);
	m_btnClose.SetButtonImage(IDB_Close, hinstance, false, 5);

	//SetForegroundWindow();
	//关闭按钮
	if((m_uType&MB_DEFBUTTON2)!=0)
	{
		m_cbButtonState[BST_CLOSE]&=(~BUT_EXIST);
	}
	else
	{
		m_cbButtonState[BST_CLOSE]|=BUT_EXIST;
	}

	//按钮区域
	CreateBoxButton();
	INT nButtonAreaHeight=SMB_BUTTON_HEIGHT+SMB_BUTTON_TOP+SMB_BUTTON_BUTTOM;
	INT nButtonAreaWidth=m_nBtCount*SMB_BUTTON_WIDTH+(m_nBtCount+1)*SMB_BUTTON_SPACE+SMB_BUTTON_LEFT+SMB_BUTTON_RIGHT;

	//字符空间
	CClientDC ClientDC(this);
	CRect rcString(0,0,SMB_STRING_WIDTH,SMB_STRING_HEIGHT);
	ClientDC.SelectObject(CSkinResourceManager::GetDefaultFont());
	ClientDC.DrawText(m_strString,
						rcString,
						DT_CALCRECT|DT_EXPANDTABS|DT_NOCLIP|DT_WORD_ELLIPSIS);

	//字符区域
	INT nStringAreaHeight=rcString.Height()+SMB_STRING_TOP;
	INT nStringAreaWidth=rcString.Width()+SMB_ICON_LEFT+SMB_ICON_WIDTH+SMB_STRING_LEFT+SMB_STRING_RIGHT;

	//窗口区域
	INT nWindowAreaHeight=nStringAreaHeight+nButtonAreaHeight;
	INT nWindowAreaWidth=__max( SMB_WINDOW_WIDTH,
							   __max( nStringAreaWidth,
									nButtonAreaWidth ) );

	//输出位置
	if (nWindowAreaWidth>nStringAreaWidth)
	{
		m_rcString.top=m_SkinAttribute.m_nCaptionHeigth+SMB_STRING_TOP;
		m_rcString.left=GetXBorder()+SMB_ICON_LEFT+SMB_ICON_WIDTH+SMB_STRING_LEFT+(nWindowAreaWidth-nStringAreaWidth)/2;
		m_rcString.right=m_rcString.left+rcString.Width();
		m_rcString.bottom=m_rcString.top+rcString.Height();
	}
	else
	{
		m_rcString.top=m_SkinAttribute.m_nCaptionHeigth+SMB_STRING_TOP;
		m_rcString.left=GetXBorder()+SMB_ICON_LEFT+SMB_ICON_WIDTH+SMB_STRING_LEFT;
		m_rcString.right=m_rcString.left+rcString.Width();
		m_rcString.bottom=m_rcString.top+rcString.Height();
	}

	//调整窗口
	CRect rcWindow(0,0,0,0);
	rcWindow.right=nWindowAreaWidth+GetXBorder()*2;
	rcWindow.bottom=nWindowAreaHeight+m_SkinAttribute.m_nCaptionHeigth+GetYBorder();
	SetWindowPos(&wndTopMost,0,0,rcWindow.Width(),rcWindow.Height(),SWP_NOMOVE|SWP_NOZORDER);


	//调整按钮
	INT nYButton=m_SkinAttribute.m_nCaptionHeigth+nWindowAreaHeight-SMB_BUTTON_BUTTOM-SMB_BUTTON_HEIGHT;
	INT nXButton=GetXBorder()+(nWindowAreaWidth-m_nBtCount*SMB_BUTTON_WIDTH-(m_nBtCount+1)*SMB_BUTTON_SPACE)/2+SMB_BUTTON_SPACE;
	for (UINT i=0;i<m_nBtCount;i++)
	{
		m_btButton[i].SetWindowPos(NULL,nXButton,nYButton-5,0,0,SWP_NOSIZE|SWP_NOZORDER|SWP_SHOWWINDOW);
		nXButton+=SMB_BUTTON_SPACE+SMB_BUTTON_WIDTH;
	}

	//默认按钮
	UINT uDefaultIndex=(m_uType&MB_DEFMASK)>>8;
	if (uDefaultIndex>=m_nBtCount)
	{
		uDefaultIndex=0;
	}

//	m_btButton[uDefaultIndex].SetFocus();
	m_btButton[uDefaultIndex].SetButtonStyle(m_btButton[uDefaultIndex].GetButtonStyle()|BS_DEFPUSHBUTTON);

	//居中窗口
	CenterWindow(GetParent());

	//设置时间
	if (m_nElapse!=0) 
	{
		SetTimer(IDI_MESSAGE,1000,NULL);
	}
	SetNCRect(rcWindow.Height());
	return FALSE;
}
void CHintMsgBox::AddRectRgn(CDC* pDC,CRgn* rgn,CRect* rect,COLORREF col)
{
	for(int y=rect->top;y<rect->bottom;y++)
		for(int x=rect->left;x<rect->right;x++)
			if (pDC->GetPixel(x,y)==col)
			{
				CRgn rgnAdd;
				rgnAdd.CreateRectRgn (x, y, x+1, y+1);
				rgn->CombineRgn(rgn, &rgnAdd, RGN_DIFF);
				rgnAdd.DeleteObject();
			}
}
void CHintMsgBox::UpdateRgn(CDC * pDC)
{
	CRect rcClient;
	GetClientRect(&rcClient);
	CRgn rgn;

	COLORREF col = RGB(255,0,255);  // mask color
	UINT width,height;
	CRect rc(0,0,0,0);

	rgn.CreateRectRgn (0, 0, rcClient.Width(), rcClient.Height());

	//左上
	width = m_imgMsgBox_LeftTop.GetWidth();
	height = m_imgMsgBox_LeftTop.GetHeight();
	rc.bottom = height;
	rc.right = width;
	AddRectRgn(pDC,&rgn,&rc,col);

// 	//上
// 	rc.left = width;
// 	height = m_imgMsgBox_Top.GetHeight();
// 	rc.bottom = height;
// 	rc.right = rcClient.right - m_imgMsgBox_RightTop.GetWidth();
// 	AddRectRgn(pDC,&rgn,&rc,col);

	//右上
	width = m_imgMsgBox_RightTop.GetWidth();
	rc.left = rc.right;
	height = m_imgMsgBox_RightTop.GetHeight();
	rc.bottom = height;
	rc.right = rcClient.right;
	AddRectRgn(pDC,&rgn,&rc,col);

// 	//左
// 	rc.left = 0;
// 	rc.top = m_imgMsgBox_LeftTop.GetHeight();
// 	rc.right = m_imgMsgBox_Left.GetWidth();
// 	rc.bottom = rcClient.bottom - m_imgMsgBox_LeftBottom.GetHeight();
// 	AddRectRgn(pDC,&rgn,&rc,col);
// 
// 	//右
// 	rc.left = rcClient.right - m_imgMsgBox_Right.GetWidth();
// 	rc.top = m_imgMsgBox_RightTop.GetHeight();
// 	rc.right = rcClient.right;
// 	rc.bottom = rcClient.bottom - m_imgMsgBox_RightBottom.GetHeight();
// 	AddRectRgn(pDC,&rgn,&rc,col);

	//左下
	rc.left = 0;
	rc.top = rcClient.bottom - m_imgMsgBox_LeftBottom.GetHeight();
	rc.bottom = rcClient.bottom;
	rc.right = m_imgMsgBox_RightBottom.GetWidth();
	AddRectRgn(pDC,&rgn,&rc,col);

// 	//下
// 	rc.left = rc.right;
// 	rc.top = rcClient.bottom - m_imgMsgBox_Bottom.GetHeight();
// 	rc.right = rcClient.right - m_imgMsgBox_Bottom.GetWidth();
// 	AddRectRgn(pDC,&rgn,&rc,col);

	//右下
	rc.left = rcClient.right - m_imgMsgBox_RightBottom.GetWidth();
	rc.top = rcClient.bottom - m_imgMsgBox_RightBottom.GetHeight();
	rc.right = rcClient.right;
	AddRectRgn(pDC,&rgn,&rc,col);


	SetWindowRgn (rgn,TRUE);
	//SetForegroundWindow();
}

void CHintMsgBox::OnSize(UINT nType, int cx, int cy)
{
	//CSkinDialogEx::OnSize(nType, cx, cy);
	CSkinDialogEx::OnSize(nType, cx, cy);
	// TODO: 在此处添加消息处理程序代码

	SetWindowPos(NULL,0,0,0,0,SWP_FRAMECHANGED|SWP_NOOWNERZORDER| 
		SWP_NOMOVE|SWP_NOSIZE);

	if (::IsWindow(m_btnClose.GetSafeHwnd()))
	{
		CRect rcBnClose;
		m_btnClose.GetClientRect(&rcBnClose);
		rcBnClose.MoveToXY(cx-rcBnClose.Width()-8, 5);
		m_btnClose.SetWindowPos(0, rcBnClose.left, rcBnClose.top, rcBnClose.Width(), rcBnClose.Height(), SWP_SHOWWINDOW);
	}

	if (Draw(&CWindowDC(this), &m_memdc))
	{
		UpdateRgn(&m_memdc);

		CRect rect,rc,rc2;
		GetClientRect(&rect);
		CString sss;
		if( GetUpdateRect(&rc) )
		{
			rc2 = rc;
			if( rc.left < rc.right )
			{
				rc2.top = rect.top;
				rc2.bottom = rect.bottom;
				InvalidateRect(&rc2,0);
			}
			rc2 = rc;
			if( rc.top < rc.bottom )
			{
				rc2.left = rect.left;
				rc2.right = rect.right;
				InvalidateRect(&rc2,0);
			}
		}
		else
			InvalidateRect(0);
	}
	

}

void CHintMsgBox::OnDestroy()
{
	CSkinDialogEx::OnDestroy();

	// TODO: 在此处添加消息处理程序代码
	if (m_memdc.m_hDC) m_memdc.DeleteDC();
}

//显示消息
INT CHintMsgBox::ShowMessageBox(LPCTSTR pszString, UINT uType, UINT nElapse)
{
	return ShowMessageBox(TEXT("提示消息"),pszString,uType,nElapse);
}

//显示消息
INT CHintMsgBox::ShowMessageBox(LPCTSTR pszCaption, LPCTSTR pszString, UINT uType, UINT nElapse)
{
	//设置变量
	m_uType=uType;
	m_nElapse=nElapse;
	m_strString=pszString;
	m_strCaption=pszCaption;
	//显示窗口
	INT nResult=(INT)DoModal();
	return nResult;
}

//激活按钮
VOID CHintMsgBox::CreateBoxButton()
{
	//变量定义
	UINT uButtonID[3]={0,0,0};
	CString pszString[3]={"","",""};

	//设置按钮
	switch (m_uType&MB_TYPEMASK)
	{
	case MB_OKCANCEL:
		{
			m_nBtCount=2;
			uButtonID[0]=IDOK;
			uButtonID[1]=IDCANCEL;
			//cxf 1101 pszString[0]=TEXT("确定(&O)");
			//cxf 1101 pszString[1]=TEXT("取消(&C)");
			pszString[0]=TEXT("BTConfirm.png");
			pszString[1]=TEXT("BTCancel.png");
			break;
		}
	case MB_YESNO:
		{
			m_nBtCount=2;
			uButtonID[0]=IDYES;
			uButtonID[1]=IDNO;
			//cxf 1101 pszString[0]=TEXT("是(&Y)");
			//cxf 1101 pszString[1]=TEXT("否(&N)");
			pszString[0]=TEXT("BTYes.png");
			pszString[1]=TEXT("BTNo.png");
			break;
		}
	case MB_YESNOCANCEL:
		{
			m_nBtCount=3;
			uButtonID[0]=IDYES;
			uButtonID[1]=IDNO;
			uButtonID[2]=IDCANCEL;
			//cxf 1101 pszString[0]=TEXT("是(&Y)");
			//cxf 1101 pszString[1]=TEXT("否(&N)");
			//cxf 1101 pszString[2]=TEXT("取消(&C)");
			pszString[0]=TEXT("BTYes.png");
			pszString[1]=TEXT("BTNo.png");
			pszString[2]=TEXT("BTCancel.png");
			break;
		}
	case MB_RETRYCANCEL:
		{
			m_nBtCount=2;
			uButtonID[0]=IDRETRY;
			uButtonID[1]=IDCANCEL;
			//cxf 1101 pszString[0]=TEXT("重试(&R)");
			//cxf 1101 pszString[1]=TEXT("取消(&C)");
			pszString[0]=TEXT("BTRetry.png");
			pszString[1]=TEXT("BTCancel.png");
			break;
		}
	case MB_ABORTRETRYIGNORE:
		{
			m_nBtCount=3;
			uButtonID[0]=IDABORT;
			uButtonID[1]=IDRETRY;
			uButtonID[2]=IDIGNORE;
			//cxf 1101 pszString[0]=TEXT("中止(&A)");
			//cxf 1101 pszString[1]=TEXT("重试(&R)");
			//cxf 1101 pszString[2]=TEXT("忽略(&I)");
			pszString[0]=TEXT("BTStop.png");
			pszString[1]=TEXT("BTRetry.png");
			pszString[2]=TEXT("BTOverLook.png");
			break;
		}
	case MB_CUSTOMER:
		{
			m_nBtCount=1;
			uButtonID[0]=IDOK;
			pszString[0]=TEXT("BTCustomer.png");
			break;
		}
	default:
		{
			m_nBtCount=1;
			uButtonID[0]=IDOK;
			//cxf 1101 pszString[0]=TEXT("确定(&O)");
			pszString[0]=TEXT("BTConfirm.png");
			break;
		}
	}

	//获取图片路径
	CString strPath="",strTemp="";
	GetImageClientSharePath(strPath);
	//显示按钮
	CRect rcButton(0,0,SMB_BUTTON_WIDTH,SMB_BUTTON_HEIGHT);
	for (UINT i=0;i<m_nBtCount;i++) 
	{
		strTemp="";
		strTemp.Append(strPath);
		strTemp.Append(pszString[i]);
		m_btButton[i].Create("",WS_CHILD,rcButton,this, uButtonID[i]);
		m_btButton[i].SetButtonImage(TEXT(strTemp),false);
	}

	m_btButton[0].SetFocus();
	return;
}
//当前路径
void CHintMsgBox::GetCurrentDirectoryPath(CString &szPath)
{
	szPath="";
	TCHAR szPathTemp[MAX_PATH]={0};
	GetCurrentDirectory(MAX_PATH, szPathTemp);
	szPath.Append(szPathTemp);
}
//资源路径
void CHintMsgBox::GetImageClientSharePath(CString& strRetValue)
{
	CString strModuleFileName="";	GetCurrentDirectoryPath(strModuleFileName);
	strModuleFileName.Append("\\data\\ClientShare\\");
	strRetValue = strModuleFileName;
}
//绘画消息
VOID CHintMsgBox::OnPaint()
{
	CPaintDC dc(this);
	CRect rcClient;
	GetWindowRect(&rcClient);
	rcClient.OffsetRect(-rcClient.left,-rcClient.top);
	if (m_memdc.m_hDC && Draw(&dc, &m_memdc))
	{
		dc.BitBlt(0, 0, rcClient.Width(), rcClient.Height(), &m_memdc, 0, 0, SRCCOPY);
	}else
	{
		Draw(&dc, 0);
	}
	
	return;
}

//时间消息
VOID CHintMsgBox::OnTimer(UINT_PTR nIDEvent)
{
	//倒数时间
	if (nIDEvent==IDI_MESSAGE)
	{
		//删除判断
		if (m_nElapse==0)
		{
			PostMessage(WM_CLOSE,0,0);
			return;
		}
		//设置界面
		TCHAR szTitle[32]=TEXT("");
		_sntprintf(szTitle,CountArray(szTitle),TEXT("%s （%ld 秒）"),m_strCaption,m_nElapse--);
		SetWindowText(szTitle);
		return;
	}

	__super::OnTimer(nIDEvent);
}
bool CHintMsgBox::Draw(CDC * pDC, CDC * pMemdc)
{
	if (pMemdc)
	{
		pMemdc->DeleteDC();

		CRect rect;
		GetClientRect(&rect);
		if (m_memdc.m_hDC) m_memdc.DeleteDC();
		CWindowDC dc(this);
		CBitmap   bmp;
		m_memdc.CreateCompatibleDC(pDC);
		bmp.CreateCompatibleBitmap(pDC, rect.Width(), rect.Height());
		CBitmap * pOldbmp = (CBitmap *)m_memdc.SelectObject(&bmp);
		if (pOldbmp) pOldbmp->DeleteObject();
	
	}else
	{
		pMemdc = pDC;
	}

	if (NULL == pMemdc)
		return false;
	//绘画界面
	// 	DrawCaption(&dc);
	// 	DrawBackGround(&dc);
	// 	DrawBorder(&dc);
	if (!DrawBack(pMemdc))
		return false;

	//获取位置
	CRect rcClient;
	GetClientRect(&rcClient);

	//加载图标
	HICON hIcon=NULL;
	UINT uIconType=(m_uType&MB_ICONMASK);
	if (uIconType==MB_ICONHAND) 
	{
		hIcon=LoadIcon(NULL,IDI_HAND);
	}
	else if (uIconType==MB_ICONQUESTION)
	{
		hIcon=LoadIcon(NULL,IDI_QUESTION);
	}
	else if (uIconType==MB_ICONEXCLAMATION)
	{
		hIcon=LoadIcon(NULL,IDI_EXCLAMATION);
	}
	else 
	{
		hIcon=LoadIcon(NULL,IDI_ASTERISK);
	}
	//绘画图标
	INT nYPos=(m_rcString.bottom+m_rcString.top)/2-SMB_ICON_HEIGHT/2;
	DrawIconEx(pMemdc->m_hDC,GetXBorder()+SMB_ICON_LEFT,nYPos,hIcon,SMB_ICON_WIDTH,SMB_ICON_HEIGHT,0,NULL,DI_NORMAL);

	//绘画字体
	INT nOldMode = pMemdc->SetBkMode(TRANSPARENT);
	//cxf 1101 COLORREF clrOld = pMemdc->SetTextColor(RGB(10,10,10));
	COLORREF clrOld = pMemdc->SetTextColor(RGB(255,255,0));
	CFont fontXPValue;
	fontXPValue.CreateFont(12,0,0,0,200,0,0,0,134,3,2,1,2,TEXT("宋体"));
	pMemdc->SelectObject(fontXPValue);
	pMemdc->DrawText(m_strString,&m_rcString,DT_EXPANDTABS|DT_NOCLIP|DT_WORD_ELLIPSIS);

	pMemdc->SetBkMode(nOldMode);
	pMemdc->SetTextColor(clrOld);
	return true;
}
bool CHintMsgBox::DrawBack(CDC * pDC)
{
	if (    NULL == pDC 
		|| NULL == pDC->m_hDC 
		|| !m_imgMsgBox_LeftTop.IsSetLoadInfo()
		|| !m_imgMsgBox_Left.IsSetLoadInfo()
		|| !m_imgMsgBox_LeftBottom.IsSetLoadInfo()
		|| !m_imgMsgBox_Top.IsSetLoadInfo()
		|| !m_imgMsgBox_RightTop.IsSetLoadInfo()
		|| !m_imgMsgBox_Right.IsSetLoadInfo()
		|| !m_imgMsgBox_RightBottom.IsSetLoadInfo()
		|| !m_imgMsgBox_Bottom.IsSetLoadInfo()
		|| !m_imgMsgBox_Center.IsSetLoadInfo() ){
			return false;
		}

		CImageHandle hImage(&m_imgMsgBox_LeftTop);
		CImageHandle hImage1(&m_imgMsgBox_Left);
		CImageHandle hImage2(&m_imgMsgBox_LeftBottom);
		CImageHandle hImage3(&m_imgMsgBox_Top);
		CImageHandle hImage4(&m_imgMsgBox_RightTop);
		CImageHandle hImage5(&m_imgMsgBox_Right);
		CImageHandle hImage6(&m_imgMsgBox_RightBottom);
		CImageHandle hImage7(&m_imgMsgBox_Bottom);
		CImageHandle hImage8(&m_imgMsgBox_Center);

		CRect rect, rc;
		
		GetClientRect(&rect);
		m_imgMsgBox_LeftTop.Draw(pDC->m_hDC, 0, 0, m_imgMsgBox_LeftTop.GetWidth(), m_imgMsgBox_LeftTop.GetHeight());
		m_imgMsgBox_LeftBottom.Draw(pDC->m_hDC, 0, rect.bottom-m_imgMsgBox_LeftBottom.GetHeight(), m_imgMsgBox_LeftBottom.GetWidth(), m_imgMsgBox_LeftBottom.GetHeight());
		m_imgMsgBox_RightTop.Draw(pDC->m_hDC, rect.right-m_imgMsgBox_RightTop.GetWidth(), 0, m_imgMsgBox_RightTop.GetWidth(), m_imgMsgBox_RightTop.GetHeight());
		m_imgMsgBox_RightBottom.Draw(pDC->m_hDC, rect.right-m_imgMsgBox_RightBottom.GetWidth(), rect.bottom-m_imgMsgBox_RightBottom.GetHeight(), m_imgMsgBox_RightBottom.GetWidth(), m_imgMsgBox_RightBottom.GetHeight());


		rc.SetRect(0, 
			m_imgMsgBox_LeftTop.GetHeight(),
			m_imgMsgBox_Left.GetWidth(), 
			rect.bottom - m_imgMsgBox_LeftBottom.GetHeight());
		FillRectImage(pDC, m_imgMsgBox_Left, rc);

	
		rc.SetRect(rect.right - m_imgMsgBox_Right.GetWidth(),
			m_imgMsgBox_RightTop.GetHeight(),
			rect.right,
			rect.bottom - m_imgMsgBox_RightBottom.GetHeight());
		FillRectImage(pDC, m_imgMsgBox_Right, rc);


		rc.SetRect(m_imgMsgBox_LeftTop.GetWidth(),
			0,
			rect.right-m_imgMsgBox_RightTop.GetWidth(),
			m_imgMsgBox_Top.GetHeight());
		FillRectImage(pDC, m_imgMsgBox_Top, rc);


		rc.SetRect(m_imgMsgBox_LeftBottom.GetWidth(),
			rect.bottom - m_imgMsgBox_Bottom.GetHeight(), 
			rect.right-m_imgMsgBox_RightBottom.GetWidth(),
			rect.bottom);
		FillRectImage(pDC, m_imgMsgBox_Bottom, rc);

		rc.SetRect(m_imgMsgBox_Left.GetWidth(),
			m_imgMsgBox_Top.GetHeight(),
			rect.right-m_imgMsgBox_Right.GetWidth(),
			rect.bottom-m_imgMsgBox_Bottom.GetHeight());
		FillRectImage(pDC, m_imgMsgBox_Center, rc);

		return true;
}
void CHintMsgBox::FillRectImage(CDC * pDC, CImage & img, const CRect & rect)
{
	CDC memdc;
	CBitmap bmp;
	memdc.CreateCompatibleDC(pDC);
	bmp.CreateCompatibleBitmap(pDC, rect.Width(), rect.Height());
	memdc.SelectObject(bmp);
	
	CBrush brush;
	brush.CreatePatternBrush((CBitmap*)&img);
	CRect rc(0, 0, rect.Width(), rect.Height());
	memdc.FillRect(rc, &brush);
	pDC->BitBlt(rect.left, rect.top, rect.Width(), rect.Height(), &memdc, 0, 0, SRCCOPY);
	brush.DeleteObject();
	bmp.DeleteObject();
	memdc.DeleteDC();

}
//////////////////////////////////////////////////////////////////////////

//显示消息
extern "C" CLIENT_SHARE_CLASS INT __cdecl ShowInformation(LPCTSTR pszString, UINT nElapse, UINT uType)
{
	CHintMsgBox HintMsgBox;

	return HintMsgBox.ShowMessageBox(pszString,uType,nElapse);
}

//显示消息
extern "C" CLIENT_SHARE_CLASS INT __cdecl ShowInformationEx(LPCTSTR pszString, UINT nElapse, UINT uType ,LPCTSTR pszCaption)
{
	CHintMsgBox HintMsgBox;

	return HintMsgBox.ShowMessageBox(pszCaption,pszString,uType,nElapse);
}

//////////////////////////////////////////////////////////////////////////


