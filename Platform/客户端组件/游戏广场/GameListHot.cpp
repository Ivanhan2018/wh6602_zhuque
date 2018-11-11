// GameListHot.cpp : 实现文件
//

#include "stdafx.h"
#include "GameListHot.h"
#include ".\gamelisthot.h"
#include "PlazaDlg.h"


// CGameListHot 对话框

IMPLEMENT_DYNAMIC(CGameListHot, CDialog)
CGameListHot::CGameListHot(CWnd* pParent /*=NULL*/)
: CDialog(CGameListHot::IDD, pParent)
{
	//m_bFlag=false;
	//m_memdc.m_hDC=NULL;
	//m_wLandCrazyPos=0;
	//m_wSparrowZZPos=0;
	//m_wSparrowYYPos=0;
}

CGameListHot::~CGameListHot()
{
	//m_memdc.DeleteDC();
}

void CGameListHot::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LANDCRAZY, m_btnLandCrazy);
	DDX_Control(pDX, IDC_SPARROWYY, m_btnSparrowYY);
	DDX_Control(pDX, IDC_SPARROWZZ, m_btnSparrowZZ);
	DDX_Control(pDX, IDC_SISEQIU, m_btnSiSeQiu);
}


BEGIN_MESSAGE_MAP(CGameListHot, CDialog)

	ON_BN_CLICKED(IDC_LANDCRAZY, OnBnClickedLandcrazy)
	ON_BN_CLICKED(IDC_SPARROWYY, OnBnClickedSparrowYy)
	ON_BN_CLICKED(IDC_SPARROWZZ, OnBnClickedSparrowZz)
	ON_BN_CLICKED(IDC_SISEQIU, OnBnClickedSiSeQiu)

	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_MBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()


// CGameListHot 消息处理程序

BOOL CGameListHot::OnInitDialog()
{
	CDialog::OnInitDialog();

	//加载图片
	CString strCurrentPathPlaza("");
	GetImagePlazaPath(strCurrentPathPlaza);
	m_ImageBK.LoadImage(TEXT(strCurrentPathPlaza+"BKGameHot.png"));
	//m_imgLandCrazy.LoadImage(TEXT(strCurrentPathPlaza+"BTLandCrazy.png"));
	//m_imgSparrowZZ.LoadImage(TEXT(strCurrentPathPlaza+"BTSparrowZZ.png"));
	//m_imgSparrowYY.LoadImage(TEXT(strCurrentPathPlaza+"BTSparrowYY.png"));
	m_imgFrame.LoadImage(TEXT(strCurrentPathPlaza+"imageNull.png"));
	m_ImageFlag.LoadImage(TEXT(strCurrentPathPlaza+"BTFlag.png"));

	m_btnLandCrazy.SetButtonImage(TEXT(strCurrentPathPlaza+"BTLandCrazy.png"), false);
	m_btnSparrowYY.SetButtonImage(TEXT(strCurrentPathPlaza+"BTSparrowYY.png"), false);
	m_btnSparrowZZ.SetButtonImage(TEXT(strCurrentPathPlaza+"BTSparrowZZ.png"), false);
	m_btnSiSeQiu.SetButtonImage(TEXT(strCurrentPathPlaza+"BTSiSeQiu.png"), false);

	//锁定屏幕
	LockWindowUpdate();
	HDWP hDwp=BeginDeferWindowPos(5);
	const UINT uFlags=SWP_NOACTIVATE|SWP_NOZORDER|SWP_NOCOPYBITS|SWP_NOSIZE;
	//移动位置
	DeferWindowPos(hDwp,m_btnLandCrazy,NULL,5,7,0,0,uFlags);
	DeferWindowPos(hDwp,m_btnSiSeQiu,NULL,5,100,0,0,uFlags);
	DeferWindowPos(hDwp,m_btnSparrowZZ,NULL,5,193,0,0,uFlags);
	DeferWindowPos(hDwp,m_btnSparrowYY,NULL,5,286,0,0,uFlags);
	/*DeferWindowPos(hDwp,m_btnNull,NULL,5,286,0,0,uFlags);*/

	EndDeferWindowPos(hDwp);
	//解除锁定
	UnlockWindowUpdate();

	m_btnLandCrazy.EnableWindow(FALSE);
	m_btnSparrowZZ.EnableWindow(FALSE);
	m_btnSparrowYY.EnableWindow(FALSE);
	m_btnSiSeQiu.EnableWindow(FALSE);

	//m_bIsDown = false;
	////设置坐标
	//m_wLandCrazyPos=0;
	//m_wSparrowZZPos=0;
	//m_wSparrowYYPos=0;
	//标志位置
	m_rectFlagArea.left=170;
	m_rectFlagArea.top=20;
	m_rectFlagArea.right=188;
	m_rectFlagArea.bottom=350;
	////斗地主位置
	//m_rectLandCrazyArea.left=5;
	//m_rectLandCrazyArea.top=7;
	//m_rectLandCrazyArea.right=m_rectLandCrazyArea.left+m_imgLandCrazy.GetWidth()/5;
	//m_rectLandCrazyArea.bottom=m_rectLandCrazyArea.top+m_imgLandCrazy.GetHeight();
	////长沙麻将位置
	//m_rectSparrowZZArea.left=5;
	//m_rectSparrowZZArea.top=100;
	//m_rectSparrowZZArea.right=m_rectSparrowZZArea.left+m_imgSparrowZZ.GetWidth()/5;
	//m_rectSparrowZZArea.bottom=m_rectSparrowZZArea.top+m_imgSparrowZZ.GetHeight();
	////益阳麻将位置
	//m_rectSparrowYYArea.left=5;
	//m_rectSparrowYYArea.top=193;
	//m_rectSparrowYYArea.right=m_rectSparrowYYArea.left+m_imgSparrowYY.GetWidth()/5;
	//m_rectSparrowYYArea.bottom=m_rectSparrowYYArea.top+m_imgSparrowYY.GetHeight();

	//显示

	//创建DC
	//CPaintDC dc(this);
	//m_memdc.DeleteDC();
	//CBitmap bmp;
	//bmp.CreateCompatibleBitmap(&dc,198,382);
	//m_memdc.CreateCompatibleDC(&dc);
	//m_memdc.SelectObject(bmp);
	//InvalidateRect(NULL);
	//bmp.DeleteObject();
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CGameListHot::OnBnClickedLandcrazy()
{
	SetFlag(true);
	SetFlagPos(172,43);
	SetInvalidateRect();
	//绘制斗地主标题
	CPlazaDlg * plazaDlg =((CPlazaDlg*) GetParent());
	plazaDlg->m_RoomListCenterDlg.m_wKindID=10;//斗地主
	plazaDlg->PostMessage(WM_COMMAND,IDM_CALL_ROOM_LIST,0);
	plazaDlg->SwitchTitle(7);//切换标题
}

void CGameListHot::OnBnClickedSparrowZz()
{
	SetFlag(true);
	SetFlagPos(172,230);
	SetInvalidateRect();
	//绘制益阳麻将标题
	CPlazaDlg * plazaDlg =((CPlazaDlg*) GetParent());
	plazaDlg->m_RoomListCenterDlg.m_wKindID=303;//转转麻将
	plazaDlg->PostMessage(WM_COMMAND,IDM_CALL_ROOM_LIST,0);
	plazaDlg->SwitchTitle(8);//切换标题
}

void CGameListHot::OnBnClickedSparrowYy()
{
	//设置标志位置
	SetFlag(true);
	SetFlagPos(172,320);
	SetInvalidateRect();
	//绘制转转麻将标题
	CPlazaDlg * plazaDlg =((CPlazaDlg*) GetParent());
	plazaDlg->m_RoomListCenterDlg.m_wKindID=310;//益阳麻将
	plazaDlg->PostMessage(WM_COMMAND,IDM_CALL_ROOM_LIST,0);
	plazaDlg->SwitchTitle(9);//切换标题
}

void CGameListHot::OnBnClickedSiSeQiu()
{
	//设置标志位置
	SetFlag(true);
	SetFlagPos(172,135);
	SetInvalidateRect();
	//绘制转转麻将标题
	CPlazaDlg * plazaDlg =((CPlazaDlg*) GetParent());
	plazaDlg->m_RoomListCenterDlg.m_wKindID=SISEQIU_ID;//四色球
	plazaDlg->PostMessage(WM_COMMAND,IDM_CALL_ROOM_LIST,0);
	plazaDlg->SwitchTitle(14);//切换标题
}

//设置有效游戏
void CGameListHot::SetGameID(WORD wKinds)
{
	if (10==wKinds)
	{
		m_btnLandCrazy.EnableWindow(TRUE);
	}
	else if(303==wKinds)
	{
		m_btnSparrowZZ.EnableWindow(TRUE);
	}else if(310==wKinds)
	{
		m_btnSparrowYY.EnableWindow(TRUE);
	}
	else if (SISEQIU_ID == wKinds)
	{
		m_btnSiSeQiu.EnableWindow(TRUE);
	}
}

//当前路径
void CGameListHot::GetCurrentDirectoryPath(CString &szPath)
{
	szPath="";
	TCHAR szPathTemp[MAX_PATH]={0};
	GetCurrentDirectory(MAX_PATH, szPathTemp);
	szPath.Append(szPathTemp);
}

//大厅资源路径
void CGameListHot::GetImagePlazaPath(CString& strRetValue)
{
	CString strModuleFileName("");
	GetCurrentDirectoryPath(strModuleFileName);
	strModuleFileName.Append("\\data\\plaza\\");
	strRetValue = strModuleFileName;
}
void CGameListHot::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 在此处添加消息处理程序代码
	// 不为绘图消息调用 CDialog::OnPaint()
	CRect rect;
	GetClientRect(&rect);
	int iWidth = rect.Width();
	int iHeight = rect.Height();
	CDC memdc;
	memdc.CreateCompatibleDC(&dc);
	CBitmap bmp;
	bmp.CreateCompatibleBitmap(&dc, iWidth, iHeight);
	::SelectObject(memdc.GetSafeHdc(), bmp);
	memdc.SetBkMode(TRANSPARENT);
	if (!m_ImageBK.IsNull())
	{
		m_ImageBK.DrawImage(&memdc,0,0);
	}
	//绘画标志
	if (m_bFlag)
	{
		m_ImageFlag.DrawImage(&memdc,m_rectFlag.left,m_rectFlag.top);
	}
	//if (!m_imgLandCrazy.IsNull())
	//{//斗地主
	//	WORD  width=m_imgLandCrazy.GetWidth()/5;
	//	m_imgLandCrazy.DrawImage(&memdc, m_rectLandCrazyArea.left, m_rectLandCrazyArea.top, width, m_imgLandCrazy.GetHeight(),
	//		m_wLandCrazyPos*width, 0);
	//}
	//if (!m_imgSparrowYY.IsNull())
	//{//益阳麻将
	//	WORD  width=m_imgSparrowYY.GetWidth()/5;
	//	m_imgSparrowYY.DrawImage(&memdc, m_rectSparrowYYArea.left, m_rectSparrowYYArea.top, width, m_imgSparrowYY.GetHeight(),
	//		m_wSparrowYYPos*width, 0);
	//}
	//if (!m_imgSparrowZZ.IsNull())
	//{//转转麻将
	//	WORD  width=m_imgSparrowZZ.GetWidth()/5;
	//	m_imgSparrowZZ.DrawImage(&memdc, m_rectSparrowZZArea.left, m_rectSparrowZZArea.top, width, m_imgSparrowZZ.GetHeight(),
	//		m_wSparrowZZPos*width, 0);
	//}
	if (!m_imgFrame.IsNull())
	{//框
		WORD width=m_imgFrame.GetWidth()/5;
		m_imgFrame.DrawImage(&memdc, 5, 286, width, m_imgFrame.GetHeight(), 
			4*width, 0);
	}
	dc.BitBlt(0, 0, iWidth, iHeight, &memdc, 0, 0, SRCCOPY);

	::DeleteObject(bmp);

	::DeleteDC(memdc.GetSafeHdc());
}
void CGameListHot::SetInvalidateRect()
{
	InvalidateRect(&m_rectFlagArea,FALSE);
}
//设置标志
void CGameListHot::SetFlag(bool bFlag)
{
	m_bFlag=bFlag;
}
//设置标志位置
void CGameListHot::SetFlagPos(int x, int y)
{
	m_rectFlag.left=x;
	m_rectFlag.top=y;
}

BOOL CGameListHot::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	return TRUE;
	//return CDialog::OnEraseBkgnd(pDC);
}

void CGameListHot::OnMButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CDialog::OnMButtonUp(nFlags, point);
}

void CGameListHot::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	//if (m_rectLandCrazyArea.PtInRect(point))
	//{//按下斗地主
	//	if (2 != m_wLandCrazyPos)
	//	{
	//		m_wLandCrazyPos=2;
	//		m_wSparrowZZPos=0;
	//		m_wSparrowYYPos=0;
	//	}
	//}else if (m_rectSparrowZZArea.PtInRect(point))
	//{//按下长沙麻将
	//	if (2 != m_wSparrowZZPos)
	//	{
	//		m_wLandCrazyPos=0;
	//		m_wSparrowZZPos=2;
	//		m_wSparrowYYPos=0;
	//	}
	//}else if (m_rectSparrowYYArea.PtInRect(point))
	//{//按下益阳麻将
	//	if (2 != m_wSparrowYYPos)
	//	{
	//		m_wLandCrazyPos=0;
	//		m_wSparrowZZPos=0;
	//		m_wSparrowYYPos=2;
	//	}
	//}else
	//{//绘画还原
	//	m_wLandCrazyPos=0;
	//	m_wSparrowZZPos=0;
	//	m_wSparrowYYPos=0;
	//}
	//if (!m_bIsDown)
	//{
	//	InvalidateRect(m_rectLandCrazyArea);
	//	InvalidateRect(m_rectSparrowZZArea);
	//	InvalidateRect(m_rectSparrowYYArea);
	//}

	CDialog::OnMouseMove(nFlags, point);
}

void CGameListHot::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	// TODO: 在此添加消息处理程序代码和/或调用默认值
	//if (m_rectLandCrazyArea.PtInRect(point))
	//{//按下斗地主
	//	m_wLandCrazyPos=1;
	//	m_wSparrowZZPos=0;
	//	m_wSparrowYYPos=0;
	//}
	//else if (m_rectSparrowZZArea.PtInRect(point))
	//{//按下长沙麻将
	//	m_wLandCrazyPos=0;
	//	m_wSparrowZZPos=1;
	//	m_wSparrowYYPos=0;
	//}
	//else if (m_rectSparrowYYArea.PtInRect(point))
	//{//按下益阳麻将
	//	m_wLandCrazyPos=0;
	//	m_wSparrowZZPos=0;
	//	m_wSparrowYYPos=1;
	//}
	//InvalidateRect(m_rectLandCrazyArea);
	//InvalidateRect(m_rectSparrowZZArea);
	//InvalidateRect(m_rectSparrowYYArea);
	//m_bIsDown = true;
	CDialog::OnLButtonDown(nFlags, point);
}

void CGameListHot::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	//if (m_rectLandCrazyArea.PtInRect(point))
	//{//按下斗地主
	//	OnBnClickedLandcrazy();
	//}
	//else if (m_rectSparrowZZArea.PtInRect(point))
	//{//按下长沙麻将
	//	OnBnClickedSparrowZz();
	//}
	//else if (m_rectSparrowYYArea.PtInRect(point))
	//{//按下益阳麻将
	//	OnBnClickedSparrowYy();
	//}
	//m_wLandCrazyPos=0;
	//m_wSparrowZZPos=0;
	//m_wSparrowYYPos=0;
	//m_bIsDown = false;
	//InvalidateRect(m_rectLandCrazyArea);
	//InvalidateRect(m_rectSparrowZZArea);
	//InvalidateRect(m_rectSparrowYYArea);
	CDialog::OnLButtonUp(nFlags, point);
}

BOOL CGameListHot::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if(pMsg->message==WM_KEYDOWN)
	{
		if (VK_ESCAPE == pMsg->wParam)
		{
			return TRUE;
		}
	}

	return CDialog::PreTranslateMessage(pMsg);
}
