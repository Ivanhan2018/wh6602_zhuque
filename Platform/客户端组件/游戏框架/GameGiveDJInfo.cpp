// GameGiveDJInfo.cpp : 实现文件
//

#include "stdafx.h"
#include "GameGiveDJInfo.h"
#include ".\gamegivedjinfo.h"

#include "MemDC.h"

#define		IDI_TIMER_DJ_GIVE_INFO				3210
#define		TIMER_GIVE_DJINFO_INTERVAL			3000


// CGameGiveDJInfo 对话框

IMPLEMENT_DYNAMIC(CGameGiveDJInfo, CDialog)
CGameGiveDJInfo::CGameGiveDJInfo(CWnd* pParent /*=NULL*/)
	: CDialog(CGameGiveDJInfo::IDD, pParent)
{
	::memset(m_chGiveInfo, 0, sizeof(m_chGiveInfo));
}

CGameGiveDJInfo::~CGameGiveDJInfo()
{
}

void CGameGiveDJInfo::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CGameGiveDJInfo, CDialog)
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_WM_PAINT()
END_MESSAGE_MAP()


// CGameGiveDJInfo 消息处理程序



void CGameGiveDJInfo::AddRectRgn(CDC* pDC,CRgn* rgn,CRect* rect,COLORREF col)
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
void CGameGiveDJInfo::UpdateRgn(CDC * pDC)
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

bool CGameGiveDJInfo::DrawWindow(CDC * pDC)
{
	if (NULL != pDC && NULL != pDC->m_hDC)
	{
		CImageHandle ImageHandle(&m_ImageBack);
		pDC->SetBkMode(TRANSPARENT);
		pDC->FillSolidRect(0,0,m_ImageBack.GetWidth(),m_ImageBack.GetHeight(),RGB(255,0,255));

		//绘画背景图片
		CImageHandle HandleCenter(&m_ImageBack);
		m_ImageBack.AlphaDrawImage(pDC, 0, 0, RGB(255, 0, 255));
		return true;
	}
	return false;
}
void CGameGiveDJInfo::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

}

void CGameGiveDJInfo::OnTimer(UINT nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (IDI_TIMER_DJ_GIVE_INFO == nIDEvent)
	{
		KillTimer(IDI_TIMER_DJ_GIVE_INFO);
		ShowWindow(SW_HIDE);
	}
	CDialog::OnTimer(nIDEvent);
}

void CGameGiveDJInfo::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 在此处添加消息处理程序代码
	// 不为绘图消息调用 CDialog::OnPaint()

	CRect rect;
	GetClientRect(&rect);
	CMemDC dcMem(&dc, rect, true);	// 用单色填充背景

	//创建字体
	CFont DrawFont;
	DrawFont.CreateFont(14,0,0,0,400,0,0,0,134,3,2,1,2,TEXT("宋体"));
	CFont * pOldFont=dcMem.SelectObject(&DrawFont);
	CRect rctext;
	rctext.left = rect.left+10;
	rctext.top = rect.top + 13;
	rctext.right = rctext.left + 166;
	rctext.bottom = rctext.top + 40;
	dcMem.DrawText(m_chGiveInfo, &rctext, DT_LEFT|DT_TOP|DT_WORDBREAK);

	dcMem.SelectObject(pOldFont);
	::DeleteObject(DrawFont);
	pOldFont = NULL;
}

BOOL CGameGiveDJInfo::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	HINSTANCE hInstance=GetModuleHandle(GAME_FRAME_DLL_NAME);
	m_ImageBack.SetLoadInfo(IDB_GAME_DJ_GIVE_INFO, hInstance);

	::memset(m_chGiveInfo, 0, sizeof(m_chGiveInfo));

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

//设置数值
void CGameGiveDJInfo::SetGiveDJInfo(const char* chGiveInfo)
{
	if (NULL == chGiveInfo)
	{
		return;
	}
	::memcpy(m_chGiveInfo, chGiveInfo, 1024);
	InvalidateRect(NULL);
	ShowWindow(SW_SHOW);
	KillTimer(IDI_TIMER_DJ_GIVE_INFO);
	SetTimer(IDI_TIMER_DJ_GIVE_INFO, TIMER_GIVE_DJINFO_INTERVAL, NULL);
}