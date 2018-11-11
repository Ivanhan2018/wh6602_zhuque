// ShowTimeDJList.cpp : 实现文件
//

#include "stdafx.h"
#include "ShowTimeDJList.h"
#include ".\showtimedjlist.h"

#include "MemDC.h"

#define			DJ_TIME_CENTER_INTERVAL			34
#define			DJ_DRAW_TIME_X					10
#define			DJ_DRAW_TIME_Y					35
#define			DJ_DRAW_TIME_INTERVAL			33

#define			IDI_DJ_COUNT_TIME_ID			1023
#define			ID_DJ_TIME_INTERVAL				1000


// CShowTimeDJList 对话框

IMPLEMENT_DYNAMIC(CShowTimeDJList, CDialog)
CShowTimeDJList::CShowTimeDJList(CWnd* pParent /*=NULL*/)
	: CDialog(CShowTimeDJList::IDD, pParent)
{
	m_pUserDJInfo = NULL;
	m_bIsDisableTime = false;
}

CShowTimeDJList::~CShowTimeDJList()
{
	m_pUserDJInfo = NULL;
}

void CShowTimeDJList::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CShowTimeDJList, CDialog)
//	ON_WM_NCMOUSEMOVE()
ON_WM_PAINT()
ON_WM_ERASEBKGND()
ON_WM_TIMER()
END_MESSAGE_MAP()


//动态获得窗口宽与高
CSize CShowTimeDJList::GetDlgWH(void)
{
	CSize wndsize;
	wndsize.SetSize(0, 0);
	if (NULL!=m_pUserDJInfo)
	{
		int iNums = m_pUserDJInfo->GetUserUseDJAndTimeDJNums();
		if (iNums>0)
		{
			wndsize.cx = m_ImageWndTop.GetWidth();
			wndsize.cy = m_ImageWndTop.GetHeight() + m_ImageWndDown.GetHeight() + (iNums-1)*DJ_TIME_CENTER_INTERVAL;
		}
	}
	return wndsize;
}

//重置窗口大小
void CShowTimeDJList::SetResetWndSize(void)
{
	if (NULL==m_pUserDJInfo)
	{
		return;
	}
	int iNums = m_pUserDJInfo->GetUserUseDJAndTimeDJNums();
	if (iNums > 0)
	{
		CSize wndsize;
		wndsize.cx = m_ImageWndTop.GetWidth();
		wndsize.cy = m_ImageWndTop.GetHeight() + m_ImageWndDown.GetHeight() + (iNums-1)*DJ_TIME_CENTER_INTERVAL;
		
		CWnd* pParamWnd = GetParent();
		if (NULL!=pParamWnd)
		{
			CRect rect;
			pParamWnd->GetClientRect(&rect);
			SetWindowPos(NULL, 380, rect.bottom-40-wndsize.cy, wndsize.cx, wndsize.cy, 0);
			InvalidateRect(NULL);
		}
	}
	else
	{
		ShowWindow(SW_HIDE);
	}
}

// CShowTimeDJList 消息处理程序

BOOL CShowTimeDJList::OnInitDialog()
{
	CDialog::OnInitDialog();

	////加入WS_EX_LAYERED扩展属性
	//SetWindowLong(this->GetSafeHwnd(),GWL_EXSTYLE,
	//	GetWindowLong(this->GetSafeHwnd(),GWL_EXSTYLE)^0x80000);
	//HINSTANCE hInst = LoadLibrary("User32.DLL");
	//if(hInst)
	//{
	//	typedef BOOL (WINAPI *MYFUNC)(HWND,COLORREF,BYTE,DWORD);
	//	MYFUNC fun = NULL;
	//	//取得SetLayeredWindowAttributes函数指针
	//	fun=(MYFUNC)GetProcAddress(hInst, "SetLayeredWindowAttributes");
	//	if(fun)fun(this->GetSafeHwnd(),200,128,2);
	//	FreeLibrary(hInst);
	//}

	// TODO:  在此添加额外的初始化
	m_pUserDJInfo = CGameClientDJInfo::GetDJInfoInstance();

	HINSTANCE hInstance=GetModuleHandle(GAME_FRAME_DLL_NAME);
	
	//上
	m_ImageWndTop.LoadImage(hInstance, TEXT("IMAGE_DJTIME_BACK_UP"));
	//中
	m_ImageWndCenter.LoadImage(hInstance, TEXT("IMAGE_DJTIME_BACK_CENTER"));
	//下
	m_ImageWndDown.LoadImage(hInstance, TEXT("IMAGE_DJTIME_BACK_DOWN"));
	//道具背景框
	m_ImageDJBackFrame.LoadImage(hInstance, TEXT("IMAGE_DJ_TIME_DJBACK"));
	//道具时间数字
	m_ImageDJTimeNums.LoadImage(hInstance, TEXT("IMAGE_DJ_TIME_NUMS"));
	//不足一分钟
	m_ImageNoMinute.LoadImage(hInstance, TEXT("IMAGE_DJ_NO_MINUTE"));
	m_bIsDisableTime = false;
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

//开始计时
void CShowTimeDJList::SetDisableAndUpdateWnd(bool bIsDisable)
{
	KillTimer(IDI_DJ_COUNT_TIME_ID);
	if (true == bIsDisable)
	{
		SetResetWndSize();
		//启动定时器
		SetTimer(IDI_DJ_COUNT_TIME_ID, ID_DJ_TIME_INTERVAL, NULL);
	}
	m_bIsDisableTime = bIsDisable;
	InvalidateRect(NULL);
}

void CShowTimeDJList::OnPaint()
{
	CPaintDC dc(this);
	CRect rect;
	GetClientRect(&rect);
	CMemDC dcMem(&dc, rect, true);	// 用单色填充背景

	dcMem.SetBkMode(TRANSPARENT);
	dcMem.FillSolidRect(0,0,rect.Width(), rect.Height(),RGB(0,0,0));
	
	//绘制背景
	DrawWndBack(&dcMem);

	//绘制道具信息
	if (NULL!=m_pUserDJInfo)
	{
		for (int i=0; i<m_pUserDJInfo->m_vecUserUseDJList.size(); i++)
		{
			DrawDJTimeItem(&dcMem, i, DJ_DRAW_TIME_X, DJ_DRAW_TIME_Y+i*DJ_DRAW_TIME_INTERVAL);
		}
	}
}


//显示窗口
void CShowTimeDJList::SetShowWnd(bool bShow)
{
	if (false == bShow)
	{
		ShowWindow(SW_HIDE);
	}
	else
	{
		if (NULL!=m_pUserDJInfo&&m_pUserDJInfo->m_vecUserUseDJList.size() > 0)
		{
			SetResetWndSize();
			ShowWindow(SW_SHOW);
		}
	}
}
BOOL CShowTimeDJList::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	return TRUE;
	return CDialog::OnEraseBkgnd(pDC);
}

void CShowTimeDJList::OnTimer(UINT nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (IDI_DJ_COUNT_TIME_ID == nIDEvent)
	{
		if (NULL!=m_pUserDJInfo)
		{
			//使所有限时道具倒计时.并清理所有限时道具.
			bool bIsChanged = m_pUserDJInfo->SetTimeDJAndClearDJ();
			if (true == bIsChanged)
			{
				SetResetWndSize();
			}
			InvalidateRect(NULL);
		}
	}
	CDialog::OnTimer(nIDEvent);
}

//绘制背景
void CShowTimeDJList::DrawWndBack(CDC *pDC)
{
	if (NULL == pDC)
	{
		return;
	}

	CRect wndrect;
	GetClientRect(&wndrect);

	int iCountHeight = 0;
	int iImageTopHeight = 0;
	if (!m_ImageWndTop.IsNull())
	{//上
		m_ImageWndTop.DrawImage(pDC, 0, 0);
		iImageTopHeight = m_ImageWndTop.GetHeight();
		iCountHeight += iImageTopHeight;
	}
	if (!m_ImageWndDown.IsNull())
	{//下
		int iDownHeight = m_ImageWndDown.GetHeight();
		m_ImageWndDown.DrawImage(pDC, 0, wndrect.bottom-iDownHeight);
		iCountHeight += iDownHeight;
	}
	if (!m_ImageWndCenter.IsNull())
	{//中
		int iDrawCenterHeight = wndrect.Height()-iCountHeight;//中间高度
		int iImageCenterHeight = m_ImageWndCenter.GetHeight();
		int iImageCenterWidth = m_ImageWndCenter.GetWidth();
		iCountHeight = 0;
		while (iDrawCenterHeight>0)
		{//中
			if (iImageCenterHeight > iDrawCenterHeight)
			{
				iCountHeight = iDrawCenterHeight;
			}
			else
			{
				iCountHeight = iImageCenterHeight;
			}
			m_ImageWndCenter.DrawImage(pDC, 0, iImageTopHeight, iImageCenterWidth, iCountHeight, 0, 0, iImageCenterWidth, iCountHeight);
			iDrawCenterHeight -= iCountHeight;
			iImageTopHeight += iCountHeight;
		}
	}
}

//绘制道具时间
void CShowTimeDJList::DrawDJTimeItem(CDC *pDC, int iIndex, int iX, int iY)
{
	if (NULL==pDC || NULL==m_pUserDJInfo || iIndex>=m_pUserDJInfo->m_vecUserUseDJList.size())
	{
		return;
	}

	//绘制道具背景框
	if (!m_ImageDJBackFrame.IsNull())
	{
		m_ImageDJBackFrame.DrawImage(pDC, iX, iY);
	}
	CPngImage* pImage = m_pUserDJInfo->GetImageFromeDJID(m_pUserDJInfo->m_vecUserUseDJList[iIndex].idjID);
	if (NULL!=pImage&&!pImage->IsNull())
	{
		pImage->DrawImage(pDC, iX+4, iY+3);
	}

	if (!m_ImageDJTimeNums.IsNull())
	{//时间图片
		DWORD iTime = m_pUserDJInfo->m_vecUserUseDJList[iIndex].dwResidualTimeCount;
		//绘制时间
		if (iTime>0)
		{
			int iTimeX = iX+34;
			int iTimeY = iY+4;
			int iTimeWidth = m_ImageDJTimeNums.GetWidth()/11;
			int iTimeHeight = m_ImageDJTimeNums.GetHeight();
			//绘制时
			int iHour = iTime/3600;
			int iMinute = (iTime%3600)/60;
			if (iHour>0 || iMinute>0)
			{//有时钟显示
				if (iHour<10)
				{
					m_ImageDJTimeNums.DrawImage(pDC, iTimeX, iTimeY, iTimeWidth, iTimeHeight, 0, 0, iTimeWidth, iTimeHeight);
					m_ImageDJTimeNums.DrawImage(pDC, iTimeX+iTimeWidth, iTimeY, iTimeWidth, iTimeHeight, iHour*iTimeWidth, 0, iTimeWidth, iTimeHeight);
				}
				else
				{
					m_ImageDJTimeNums.DrawImage(pDC, iTimeX, iTimeY, iTimeWidth, iTimeHeight, (iHour/10)*iTimeWidth, 0, iTimeWidth, iTimeHeight);
					m_ImageDJTimeNums.DrawImage(pDC, iTimeX+iTimeWidth, iTimeY, iTimeWidth, iTimeHeight, (iHour%10)*iTimeWidth, 0, iTimeWidth, iTimeHeight);
				}
				iTimeX += 2*iTimeWidth;

				//绘制分隔符
				m_ImageDJTimeNums.DrawImage(pDC, iTimeX, iTimeY, iTimeWidth, iTimeHeight, 10*iTimeWidth, 0, iTimeWidth, iTimeHeight);
				iTimeX += iTimeWidth;

				//绘制分
				if (iMinute<10)
				{
					m_ImageDJTimeNums.DrawImage(pDC, iTimeX, iTimeY, iTimeWidth, iTimeHeight, 0, 0, iTimeWidth, iTimeHeight);
					m_ImageDJTimeNums.DrawImage(pDC, iTimeX+iTimeWidth, iTimeY, iTimeWidth, iTimeHeight, iMinute*iTimeWidth, 0, iTimeWidth, iTimeHeight);
				}
				else
				{
					m_ImageDJTimeNums.DrawImage(pDC, iTimeX, iTimeY, iTimeWidth, iTimeHeight, (iMinute/10)*iTimeWidth, 0, iTimeWidth, iTimeHeight);
					m_ImageDJTimeNums.DrawImage(pDC, iTimeX+iTimeWidth, iTimeY, iTimeWidth, iTimeHeight, (iMinute%10)*iTimeWidth, 0, iTimeWidth, iTimeHeight);
				}
			}
			else
			{//不足一分钟
				m_ImageNoMinute.DrawImage(pDC, iTimeX, iTimeY+5);
			}
		}
	}
	
}