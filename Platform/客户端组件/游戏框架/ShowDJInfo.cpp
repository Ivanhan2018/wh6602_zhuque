// ShowDJInfo.cpp : 实现文件
//

#include "stdafx.h"
#include "ShowDJInfo.h"
#include ".\showdjinfo.h"
#include "GameClientDJInfo.h"

#include "MemDC.h"

// CShowDJInfo 对话框
#define			SHOW_DJ_INFO_X					30
#define			SHOW_DJ_INFO_MOVE_INTERVAL		29
#define			SHOW_DJ_INFO_WIDTH				178
#define			SHOW_DJ_INFO_HEIGHT				98


#define			IDI_TIME_SHOW_DJ_INFO_MOVE_ID		1024
#define			ID_TIME_MOVE_INTERVAL				10



IMPLEMENT_DYNAMIC(CShowDJInfo, CDialog)
CShowDJInfo::CShowDJInfo(CWnd* pParent /*=NULL*/)
	: CDialog(CShowDJInfo::IDD, pParent)
{
	m_iSaveMoveIndex = -1;
	m_iSaveCurDJID = -1;
	m_pGameClientDJInfo = NULL;
}

CShowDJInfo::~CShowDJInfo()
{
}

void CShowDJInfo::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CShowDJInfo, CDialog)
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CShowDJInfo 消息处理程序


//显示道具信息窗口
void CShowDJInfo::SetWndShow(bool bIsShow, int iIndex)
{
	if (false == bIsShow)
	{
		KillTimer(IDI_TIME_SHOW_DJ_INFO_MOVE_ID);
		m_iSaveMoveIndex = -1;
		m_iSaveCurDJID = -1;
		ShowWindow(SW_HIDE);
	}
	else
	{
		CWnd* pParam = GetParent();
		if (NULL != pParam)
		{
			CRect rect;
			pParam->GetClientRect(&rect);

			if (-1 == m_iSaveMoveIndex)
			{
				//保存旧窗口位置
				m_rcSaveOldWnd.x = SHOW_DJ_INFO_X+iIndex*SHOW_DJ_INFO_MOVE_INTERVAL;
				m_rcSaveOldWnd.y = rect.bottom-30-SHOW_DJ_INFO_HEIGHT;
				SetWindowPos(NULL, m_rcSaveOldWnd.x,rect.bottom-30-SHOW_DJ_INFO_HEIGHT, SHOW_DJ_INFO_WIDTH, SHOW_DJ_INFO_HEIGHT, SWP_NOSIZE | SWP_SHOWWINDOW);
			}
			else
			{//移动窗口
				KillTimer(IDI_TIME_SHOW_DJ_INFO_MOVE_ID);
				if (m_rcSaveOldWnd.x==SHOW_DJ_INFO_X+iIndex*SHOW_DJ_INFO_MOVE_INTERVAL)
				{
					ShowWindow(SW_SHOW);
					return;
				}
				m_MoveInfo.Init(m_rcSaveOldWnd.x,SHOW_DJ_INFO_X+iIndex*SHOW_DJ_INFO_MOVE_INTERVAL);
				m_iCountMoveIndex = 0;
				m_iCountMoveTotal = 5;
				SetTimer(IDI_TIME_SHOW_DJ_INFO_MOVE_ID, ID_TIME_MOVE_INTERVAL, NULL);
			}
			//保存旧的序号
			m_iSaveMoveIndex = iIndex;
		}
	}
}

void CShowDJInfo::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
	// TODO: 在此处添加消息处理程序代码

	CClientDC dc(this);
	CRect rect;
	GetClientRect(&rect);
	CMemDC dcMem(&dc, rect, true);	// 用单色填充背景


	if(!GetSafeHwnd())
		return;

	CBitmap bmp;

	if (DrawWindow(&dcMem))
	{
		UpdateRgn(&dcMem);
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

void CShowDJInfo::AddRectRgn(CDC* pDC,CRgn* rgn,CRect* rect,COLORREF col)
{
	for(int y=rect->top;y<rect->bottom;y++)
		for(int x=rect->left;x<rect->right;x++)
			if (pDC->GetPixel(x,y)==col)
			{
				CRgn rgnAdd;
				rgnAdd.CreateRectRgn (x, y, x+1, y+1);
				rgn->CombineRgn(rgn, &rgnAdd, RGN_DIFF);
				DeleteObject(rgnAdd);
			}
}
void CShowDJInfo::UpdateRgn(CDC * pDC)
{
	CImageHandle ImageHandle(&m_ImageBack);
	CRect rcClient;
	GetClientRect(&rcClient);
	CRgn rgn;
	COLORREF col = RGB(255,0,255);  // mask color
	CRect rc(0,0,0,0);
	rgn.CreateRectRgn (0, 0, rcClient.Width(), rcClient.Height());

	//左上
	rc.bottom =  m_ImageBack.GetHeight();
	rc.right = m_ImageBack.GetWidth();
	AddRectRgn(pDC,&rgn,&rc,col);
	SetWindowRgn (rgn,TRUE);
	//SetForegroundWindow();
}

bool CShowDJInfo::DrawWindow(CDC * pDC)
{
	if (NULL != pDC && NULL != pDC->m_hDC)
	{
		if(m_ImageBack.IsSetLoadInfo())	// add by HouGuoJiang 2011-11-25
		{
			CImageHandle ImageHandle(&m_ImageBack);
			pDC->SetBkMode(TRANSPARENT);
			pDC->FillSolidRect(0,0,m_ImageBack.GetWidth(),m_ImageBack.GetHeight(),RGB(255,0,255));

			//绘画背景图片
			CImageHandle HandleCenter(&m_ImageBack);
			m_ImageBack.AlphaDrawImage(pDC, 0, 0, RGB(255, 0, 255));
			return true;
		}
	}
	return false;
}
BOOL CShowDJInfo::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	HINSTANCE hInstance=GetModuleHandle(GAME_FRAME_DLL_NAME);
	m_ImageBack.SetLoadInfo(IDB_DJINFOSHOW_BACK, hInstance, false);

	m_iSaveMoveIndex = -1;
	m_iSaveCurDJID = -1;
	m_pGameClientDJInfo = NULL;

	m_MoveInfo.Init(0.0f, 0.0f);

	m_pGameClientDJInfo = CGameClientDJInfo::GetDJInfoInstance();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


//显示道具信息
void CShowDJInfo::SetShowDJDiscription(int idjindex)
{
	if (NULL != m_pGameClientDJInfo)
	{
		m_iSaveCurDJID = -1;

		m_iSaveCurDJID = m_pGameClientDJInfo->GetDJIDFromDJIndex(idjindex);
		if (-1 != m_iSaveCurDJID)
		{
			InvalidateRect(NULL);
		}
	}
	
}

void CShowDJInfo::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 在此处添加消息处理程序代码
	// 不为绘图消息调用 CDialog::OnPaint()

	CRect rect;
	GetClientRect(&rect);
	CMemDC dcMem(&dc, rect, true);	// 用单色填充背景

	if (DrawWindow(&dcMem))
	{
		if ((NULL != m_pGameClientDJInfo) && (-1 != m_iSaveCurDJID) && (m_iSaveCurDJID>=0) && (m_iSaveCurDJID<PROPERTY_COUNT))
		{
			if (m_iSaveCurDJID>=PROP_FSK && m_iSaveCurDJID<=PROP_GOODLUCK)
			{
				//创建字体
				CFont DrawFont;
				DrawFont.CreateFont(-13,0,0,0,400,0,0,0,134,3,2,1,2,TEXT("宋体"));
				CFont * pOldFont=dcMem.SelectObject(&DrawFont);

				char chDescription[319]={0};
				memset(chDescription, 0, sizeof(chDescription));
				//绘制道具名称
				sprintf(chDescription, "道具名称:%s", g_PropertyDescribe[m_iSaveCurDJID].szName);
				dcMem.TextOut(10, 10, chDescription);
				//绘制道具描述
				memset(chDescription, 0, sizeof(chDescription));
				sprintf(chDescription, "道具描述:%s", g_PropertyDescribe[m_iSaveCurDJID].szDescribe);
				CRect rect;
				rect.left = 10;
				rect.top = 25;
				rect.right = rect.left + 160;
				rect.bottom = rect.top + 38;
				dcMem.DrawText(chDescription, &rect, DT_LEFT|DT_WORDBREAK);
				//所属游戏
				dcMem.TextOut(10, 65, TEXT("所属游戏: 所有游戏"));

				dcMem.SelectObject(pOldFont);
				::DeleteObject(DrawFont);
				pOldFont = NULL;
			}
		}

		//GetClientRect(&rect);	// 为何不放在循环的上面？ 而要每一次循环都赋值，是不是浪费CPU资源呢？
	}
}

BOOL CShowDJInfo::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	return TRUE;
	return CDialog::OnEraseBkgnd(pDC);
}

void CShowDJInfo::OnTimer(UINT nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (IDI_TIME_SHOW_DJ_INFO_MOVE_ID == nIDEvent)
	{
		if (m_iCountMoveIndex < m_iCountMoveTotal)
		{
			int iNewX = m_MoveInfo.lerp(m_iCountMoveIndex, m_iCountMoveTotal);
			CWnd* pParam = GetParent();
			if (NULL != pParam)
			{
				CRect rect;
				pParam->GetClientRect(&rect);
				m_rcSaveOldWnd.x = iNewX;
				SetWindowPos(NULL, m_rcSaveOldWnd.x,rect.bottom-30-SHOW_DJ_INFO_HEIGHT, SHOW_DJ_INFO_WIDTH, SHOW_DJ_INFO_HEIGHT, SWP_NOSIZE | SWP_SHOWWINDOW);
			}
			++m_iCountMoveIndex;
		}
		else
		{
			//移到最终点.
			KillTimer(IDI_TIME_SHOW_DJ_INFO_MOVE_ID);
			//恢复
			m_iCountMoveIndex = 0;
			m_MoveInfo.Init(0.0f,0.0f);
			CWnd* pParam = GetParent();
			if (NULL != pParam)
			{
				CRect rect;
				pParam->GetClientRect(&rect);
				m_rcSaveOldWnd.x = SHOW_DJ_INFO_X+m_iSaveMoveIndex*SHOW_DJ_INFO_MOVE_INTERVAL;
				m_rcSaveOldWnd.y = rect.bottom-30-SHOW_DJ_INFO_HEIGHT;
				SetWindowPos(NULL, m_rcSaveOldWnd.x,rect.bottom-30-SHOW_DJ_INFO_HEIGHT, SHOW_DJ_INFO_WIDTH, SHOW_DJ_INFO_HEIGHT, SWP_NOSIZE | SWP_SHOWWINDOW);
			}
		}
	}
	CDialog::OnTimer(nIDEvent);
}
