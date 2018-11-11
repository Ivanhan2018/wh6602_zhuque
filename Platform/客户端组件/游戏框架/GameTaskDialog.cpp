// GameTaskDialog.cpp : 实现文件
//

#include "stdafx.h"
#include "GameTaskDialog.h"
#include ".\gametaskdialog.h"

#include "MemDC.h"

#define		IDI_BT_TASK_DAY_DEF		3917
#define		IDI_BT_TASK_TJ_DEF		3918
#define		IDI_BT_TASK_FIND_DEF	3919


#define		ID_TIME_SHOW_TASK_ACCEPT_UI		1
#define		IDI_TIME_TASK_INTERVAL			100

#define WND_REMOTE_X	6					// 窗口X偏移
#define WND_REMOTE_Y	6					// 窗口Y偏移

// CGameTaskDialog 对话框

IMPLEMENT_DYNAMIC(CGameTaskDialog, CDialog)
CGameTaskDialog::CGameTaskDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CGameTaskDialog::IDD, pParent)
{
	m_bIsPlayFlash = false;
	m_byFlashTag = 0;
	m_wSortID = 1;
	m_bCurrentTaskType = 0;
	m_TaskImageBuffer.Destroy();
	m_TaskImageBuffer.Create(214, 90, 24);
	m_ptWnd.SetPoint(0,0);

	//初始化新任务
	::memset(&m_tagTaskAcceptInfo, 0, sizeof(m_tagTaskAcceptInfo));
	::memset(&m_tagTaskFinishInfo, 0, sizeof(m_tagTaskFinishInfo));
}

CGameTaskDialog::~CGameTaskDialog()
{
}

void CGameTaskDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CGameTaskDialog, CDialog)
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_WM_LBUTTONDOWN()
	ON_BN_CLICKED(IDI_BT_TASK_DAY_DEF, OnToDayTaskMsg)
	ON_BN_CLICKED(IDI_BT_TASK_TJ_DEF, OnTJTaskMsg)
	ON_BN_CLICKED(IDI_BT_TASK_FIND_DEF, OnFindTaskMsg)
	ON_WM_WINDOWPOSCHANGING()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


// CGameTaskDialog 消息处理程序

void CGameTaskDialog::OnPaint()
{
	//CPaintDC dc(this); // device context for painting
	// TODO: 在此处添加消息处理程序代码
	// 不为绘图消息调用 CDialog::OnPaint()

	CPaintDC dc(this);

	CRect rect;
	GetClientRect(&rect);
	CMemDC dcMem(&dc, rect, true);	// 用单色填充背景
	dcMem.SetBkMode(TRANSPARENT);

	if (!m_ImageTaskWndBack.IsNull())
	{
		m_ImageTaskWndBack.DrawImage(&dcMem, 0, 0, 
			m_ImageTaskWndBack.GetWidth()-WND_REMOTE_X*2, m_ImageTaskWndBack.GetHeight()-WND_REMOTE_Y*2, 
			WND_REMOTE_X, WND_REMOTE_Y, m_ImageTaskWndBack.GetWidth()-WND_REMOTE_X*2, m_ImageTaskWndBack.GetHeight()-WND_REMOTE_Y*2);
	}

	if (!m_ImageFlash.IsNull())
	{
		if (m_bIsPlayFlash)
		{
			if (0==m_byFlashTag)
			{
				m_ImageFlash.DrawImage(&dcMem, 0, 0, 189, 88, 0, 0, 189, 88);
			}
			else
			{
				m_ImageFlash.DrawImage(&dcMem, 0, 0, 189, 88, 189, 0, 189, 88);
			}
		}
	}

	CFont TaskNameInfoFont;
	TaskNameInfoFont.CreateFont(14,0,0,0,700,0,0,0,134,3,2,1,2,TEXT("楷体"));
	dcMem.SetTextColor(RGB(243,152,0));
	CFont * pOldFont=dcMem.SelectObject(&TaskNameInfoFont);


	CString strGems = "";
	CRect crTaskName(0, 40, 193, 70);
	if (1 == m_bCurrentTaskType)
	{
		//今日任务
		//dcMem.FillSolidRect(&crTaskName, RGB(255,255,0));//Test
		dcMem.DrawText(m_tagTaskAcceptInfo.strContext, &crTaskName, DT_CENTER|DT_VCENTER|DT_WORDBREAK|DT_END_ELLIPSIS|DT_SINGLELINE);
	}
	else if (2 == m_bCurrentTaskType)
	{
		//挑战任务
		dcMem.DrawText(m_tagTaskAcceptInfo.strContext, &crTaskName, DT_CENTER|DT_VCENTER|DT_WORDBREAK|DT_SINGLELINE);
	}
	else if (3 == m_bCurrentTaskType)
	{
		//寻宝任务
		dcMem.DrawText(m_tagTaskAcceptInfo.strContext, &crTaskName, DT_CENTER|DT_VCENTER|DT_WORDBREAK|DT_END_ELLIPSIS|DT_SINGLELINE);
	}
	strGems = "";
	int iLargess = 0;
	if (1 == m_wSortID)
	{
		iLargess = m_tagTaskAcceptInfo.iLargessCount1;
	}
	else if (2 == m_wSortID)
	{
		iLargess = m_tagTaskAcceptInfo.iLargessCount2;
	}
	else if (3 == m_wSortID)
	{
		iLargess = m_tagTaskAcceptInfo.iLargessCount3;
	}
	else if (4 == m_wSortID)
	{
		iLargess = m_tagTaskAcceptInfo.iLargessCount4;
	}

	if (iLargess>0)
	{
		crTaskName.OffsetRect(0, 25);
		strGems.Format("获得宝石%d枚。", iLargess);
		//dcMem.FillSolidRect(&crTaskName, RGB(0,255,0));//Test
		dcMem.DrawText(strGems, &crTaskName, DT_SINGLELINE|DT_CENTER|DT_VCENTER|DT_END_ELLIPSIS);
	}

	dcMem.draw();

	//清理资源
	dcMem.SelectObject(pOldFont);
	TaskNameInfoFont.DeleteObject();
}

//时间消息
void CGameTaskDialog::OnTimer(UINT nIDEvent)
{
	if (ID_TIME_SHOW_TASK_ACCEPT_UI == nIDEvent)
	{
		m_byFlashTag = !m_byFlashTag;

		m_bIsPlayFlash = true;
		m_iCountTimer += IDI_TIME_TASK_INTERVAL;
		if (m_iCountTimer >= 3000)
		{
			KillTimer(ID_TIME_SHOW_TASK_ACCEPT_UI);
			m_iCountTimer = 0;
			m_byFlashTag = 0;
			m_bIsPlayFlash = false;

			m_BTTaskToDay.ShowWindow(SW_SHOW);
			m_BTTaskTJ.ShowWindow(SW_SHOW);
			m_BTTaskFind.ShowWindow(SW_SHOW);
		}
		InvalidateRect(NULL, FALSE);
	}
	CDialog::OnTimer(nIDEvent);
}

BOOL CGameTaskDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	char pPath[64] = "data\\gameframe\\plaza";
	TCHAR szResourcePath[MAX_PATH];	GetCurrentDirectory(MAX_PATH,szResourcePath);
	WCHAR * pWC = NULL;
	CString strTalks = "";
	strTalks.Format("%s\\%s\\gametaskback.png", szResourcePath, pPath);
	pWC = strTalks.AllocSysString();
	m_ImageTaskWndBack.LoadFromFile(pWC);

	strTalks = "";
	strTalks.Format("%s\\%s\\GameTaskFlash.png", szResourcePath, pPath);
	pWC = strTalks.AllocSysString();
	m_ImageFlash.LoadFromFile(pWC);

	//创建按钮
	CRect rect(0, 0, 0, 0);
	m_BTTaskToDay.Create(TEXT(""), WS_CHILD|WS_VISIBLE, rect, this, IDI_BT_TASK_DAY_DEF);
	m_BTTaskToDay.SetWindowPos(NULL, 64, 3, 94, 42, 0);
	strTalks = "";
	strTalks.Format("%s\\%s\\BTToDayTask.png", szResourcePath, pPath);
	m_BTTaskToDay.SetButtonImage(strTalks, false, 4);
	m_BTTaskToDay.EnableWindow(FALSE);

	m_BTTaskTJ.Create(TEXT(""), WS_CHILD|WS_VISIBLE, rect, this, IDI_BT_TASK_TJ_DEF);
	m_BTTaskTJ.SetWindowPos(NULL, 126, 3, 94, 42, 0);
	strTalks = "";
	strTalks.Format("%s\\%s\\BTTJTask.png", szResourcePath, pPath);
	m_BTTaskTJ.SetButtonImage(strTalks, false, 4);
	m_BTTaskTJ.EnableWindow(FALSE);

	m_BTTaskFind.Create(TEXT(""), WS_CHILD|WS_VISIBLE, rect, this, IDI_BT_TASK_FIND_DEF);
	m_BTTaskFind.SetWindowPos(NULL, 3, 3, 94, 42, 0);
	strTalks = "";
	strTalks.Format("%s\\%s\\BTFindTask.png", szResourcePath, pPath);
	m_BTTaskFind.SetButtonImage(strTalks, false, 4);
	m_BTTaskFind.EnableWindow(FALSE);
	
	m_rcInvalidateRect.left = 6;
	m_rcInvalidateRect.top = 40;
	m_rcInvalidateRect.right = m_rcInvalidateRect.left + 200;
	m_rcInvalidateRect.bottom = m_rcInvalidateRect.top + 65;

	//默认没任务
	m_bCurrentTaskType = 0;

	SetWindowPos(&wndTop, 0, 0, m_ImageTaskWndBack.GetWidth(), m_ImageTaskWndBack.GetHeight(), SWP_NOMOVE);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CGameTaskDialog::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	CDialog::OnLButtonDown(nFlags, point);
}


void CGameTaskDialog::OnToDayTaskMsg()
{//今日任务
	m_bCurrentTaskType = 1;
	InvalidateRect(&m_rcInvalidateRect, FALSE);
}
void CGameTaskDialog::OnTJTaskMsg()
{//挑战任务
	m_bCurrentTaskType = 2;
	InvalidateRect(&m_rcInvalidateRect, FALSE);
}
void CGameTaskDialog::OnFindTaskMsg()
{//寻宝任务
	m_bCurrentTaskType = 3;
	InvalidateRect(&m_rcInvalidateRect, FALSE);
}

void CGameTaskDialog::DrawBackground(CDC *pDC, int nWidth, int nHeight, int iExcursionY/*=0*/)
{
	if (NULL == pDC  || NULL == GetSafeHwnd()  ||  FALSE == IsWindowVisible())
	{
		return;
	}

	if (!m_ImageTaskWndBack.IsNull())
	{
		m_ImageTaskWndBack.DrawImage(pDC, m_ptWnd.x, m_ptWnd.y-iExcursionY);
	}
}

//已接新任务信息
void CGameTaskDialog::SetTaskAcceptInfo(const CMD_GF_TASK_tagTask* pTaskInfo)
{
	if (NULL != pTaskInfo)
	{
		//保存新的任务信息
		memcpy(&m_tagTaskAcceptInfo, pTaskInfo, sizeof(CMD_GF_TASK_tagTask));

		m_BTTaskFind.EnableWindow(TRUE);

		m_BTTaskFind.SetFocus();
		m_bCurrentTaskType = 3;
		InvalidateRect(&m_rcInvalidateRect, FALSE);


		m_BTTaskToDay.ShowWindow(SW_HIDE);
		m_BTTaskTJ.ShowWindow(SW_HIDE);
		m_BTTaskFind.ShowWindow(SW_HIDE);
		KillTimer(ID_TIME_SHOW_TASK_ACCEPT_UI);
		SetTimer(ID_TIME_SHOW_TASK_ACCEPT_UI, IDI_TIME_TASK_INTERVAL, NULL);
		m_iCountTimer = 0;
		m_byFlashTag = 0;
		m_bIsPlayFlash = true;
	}
}

//单个任务完成
void CGameTaskDialog::SetTaskOnceFinish(const CMD_GF_TASK_tagTaskFinish* pTaskFinish)
{
	if (NULL != pTaskFinish)
	{
		//保存新的任务信息
		memcpy(&m_tagTaskFinishInfo, pTaskFinish, sizeof(CMD_GF_TASK_tagTaskFinish));
		

		KillTimer(ID_TIME_SHOW_TASK_ACCEPT_UI);
		m_iCountTimer = 0;
		m_byFlashTag = 0;
		m_bIsPlayFlash = false;
		m_BTTaskToDay.ShowWindow(SW_SHOW);
		m_BTTaskTJ.ShowWindow(SW_SHOW);
		m_BTTaskFind.ShowWindow(SW_SHOW);
	}
}

//设置房间标识
void CGameTaskDialog::SetSortID(WORD wSortID)
{
	m_wSortID = wSortID;
	InvalidateRect(&m_rcInvalidateRect, FALSE);
}


void CGameTaskDialog::OnWindowPosChanging(WINDOWPOS* lpwndpos)
{
	//CRect parrect;
	//CWnd * pWnd= AfxGetMainWnd();
	//if (NULL == pWnd)
	//{
	//	return;
	//}
	//pWnd->GetWindowRect(&parrect);

	//if (lpwndpos->y+476 > parrect.top+parrect.Height()-30)
	//{//下边
	//	lpwndpos->y = parrect.top+parrect.Height()-30-476;
	//}
	//if (lpwndpos->x+321 > parrect.left+parrect.Width())
	//{//右边
	//	lpwndpos->x = parrect.left+parrect.Width()-321;
	//}
	//if (lpwndpos->y < parrect.top+29)
	//{//上
	//	lpwndpos->y = parrect.top+29;
	//}
	//if (lpwndpos->x < parrect.left)
	//{//下
	//	lpwndpos->x = parrect.left;
	//}
	CDialog::OnWindowPosChanging(lpwndpos);

	// TODO: 在此处添加消息处理程序代码
}

BOOL CGameTaskDialog::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	return TRUE;
	//return CDialog::OnEraseBkgnd(pDC);
}

BOOL CGameTaskDialog::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if ((pMsg->message==WM_KEYDOWN)&&(pMsg->wParam==VK_RETURN || VK_ESCAPE==pMsg->wParam))
	{
		return TRUE;
	}
	return CDialog::PreTranslateMessage(pMsg);
}

BOOL CGameTaskDialog::SetWindowPos(const CWnd* pWndInsertAfter, int x, int y,
				  int cx, int cy, UINT nFlags)
{
	m_ptWnd.SetPoint(x, y);
	return CDialog::SetWindowPos(pWndInsertAfter, x+WND_REMOTE_X, y+WND_REMOTE_Y, cx-WND_REMOTE_X*2, cy-WND_REMOTE_X*2, nFlags);
}