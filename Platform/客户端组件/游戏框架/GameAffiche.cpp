// GameAffiche.cpp : 实现文件
//

#include "stdafx.h"
#include "GameAffiche.h"
#include ".\gameaffiche.h"
#include "TraceCatch.h"

//公告信息
#define	IDI_ROOM_AFFICHE_INFO			20000							//房间公告信息定时器
#define TIMER_ROOM_AFFICHE_INFO			10								//时间间隔

// CGameAffiche 对话框

IMPLEMENT_DYNAMIC(CGameAffiche, CDialog)
CGameAffiche::CGameAffiche(CWnd* pParent /*=NULL*/)
	: CDialog(CGameAffiche::IDD, pParent), m_bIsExp(false)
{
	m_memdc.m_hDC = NULL;
}

CGameAffiche::~CGameAffiche()
{
	__ENTER_FUNCTION

	CThreadGuard Guard(&m_CriticalSpection);

	if (!m_ImageBack.IsNull())
	{
		m_ImageBack.DestroyImage();
	}

	::DeleteObject(m_fontDrawGameInfo);
	//m_fontDrawGameInfo.DeleteObject();
	::DeleteDC(m_memdc.GetSafeHdc());
	//m_memdc.DeleteDC();
	m_memdc.m_hDC = NULL;

	__LEAVE_FUNCTION
}

void CGameAffiche::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CGameAffiche, CDialog)
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


// CGameAffiche 消息处理程序

int CGameAffiche::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码
	//游戏公告字体
	m_fontDrawGameInfo.CreateFont(18,0,0,0,400,0,0,0,134,3,2,1,2,TEXT("宋体"));
	m_bIsExp = false;
	m_iLoopNums = 0;
	m_iCountLoop = 0;
	m_vecGameMsg.clear();
	return 0;
}

void CGameAffiche::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 在此处添加消息处理程序代码
	// 不为绘图消息调用 CDialog::OnPaint()

	__ENTER_FUNCTION

	CThreadGuard Guard(&m_CriticalSpection);

	int iNums = m_vecGameMsg.size();
	if (iNums > 0)
	{
		if (NULL != m_memdc.m_hDC)
		{
			CRect rect;
			GetClientRect(&rect);
			if (!m_ImageBack.IsNull())
			{
				m_ImageBack.DrawImage(&m_memdc, 0, 0);
			}

			m_memdc.SetTextColor(m_bTextColor);

			if (m_bIsDisableGameMsg)
			{//绘制房间公告信息
				CFont * pOldFont=m_memdc.SelectObject(&m_fontDrawGameInfo);
				if (m_iCountVecIndex < m_vecGameMsg.size())
				{
					m_memdc.DrawText(m_vecGameMsg[m_iCountVecIndex].c_str(), m_rcCurMsgInfo, DT_RIGHT|DT_VCENTER|DT_SINGLELINE);
				}		
				m_memdc.SelectObject(pOldFont);
				pOldFont = NULL;
			}

			dc.BitBlt(0, 0, rect.Width(), rect.Height(), &m_memdc, 0, 0, SRCCOPY);
		}
	}

	__LEAVE_FUNCTION
	
}

//
//BOOL CGameAffiche::SetImageRgn(CPngImage * pImage)
//{
//	if (NULL==pImage) return FALSE;
//	if (pImage->IsNull()) return FALSE;
//	long wWidth = pImage->GetWidth();
//	long wHeight = pImage->GetHeight();
//
//	CRgn rgnImage, tempRgn;
//	rgnImage.CreateRectRgn(0, 0, 0, 0);
//
//	int leftX = 0;
//	int rightX = 0;
//	for (int i=wHeight-1; i>=0; --i)
//	{
//		for (int j = 0; j<wWidth; ++j)
//		{
//			leftX = j;
//			rightX = j;
//			for (int k=j; k<wWidth; ++k)  //记住左边的x
//			{
//				BYTE byValue = pImage->GetAlphaValue(k, i);
//				if (byValue != 0)
//				{
//					leftX = k;
//					break;
//				}
//			}
//			for (int k=leftX+1; k<wWidth; ++k)
//			{
//				BYTE byValue = pImage->GetAlphaValue(k, i);
//				if (byValue == 0 || (k == wWidth -1 && byValue!=0))
//				{
//					rightX = k;
//					break;
//				}
//			}
//
//			j = leftX > rightX ? leftX : rightX;
//
//			if (rightX > leftX)
//			{
//				tempRgn.CreateRectRgn(leftX, (wHeight - i-1) , rightX, (wHeight -i));
//				rgnImage.CombineRgn(&rgnImage, &tempRgn, RGN_OR);
//				tempRgn.DeleteObject();
//			}
//		}
//	}
//
//	if (NULL == rgnImage.m_hObject)
//		return FALSE;
//
//	SetWindowRgn((HRGN)rgnImage.m_hObject, TRUE);
//	rgnImage.DeleteObject();
//	return TRUE;
//}

//设置游戏公告
void CGameAffiche::SetGameAfficheInfo(const char* pInfo)
{
	__ENTER_FUNCTION

	CThreadGuard Guard(&m_CriticalSpection);

	if (NULL != pInfo)
	{
		KillTimer(IDI_ROOM_AFFICHE_INFO);
		m_strSaveGameAffiche.clear();
		m_strSaveGameAffiche.append(pInfo);
		//初始化公告信息
		m_vecGameMsg.clear();
		m_iCountVecIndex = 0;
		//解析公告
		ParseChar(pInfo, ::strlen(pInfo)+1);
		WORD wInfoCount = m_vecGameMsg.size();
		if (wInfoCount>0)
		{
			CFont * pOldFont=m_memdc.SelectObject(&m_fontDrawGameInfo);
			m_rcCurMsgInfo.left = m_rcGameMsgInfo.right;
			m_rcCurMsgInfo.top = m_rcGameMsgInfo.top;
			m_rcCurMsgInfo.right = m_rcCurMsgInfo.left+m_memdc.GetTextExtent(m_vecGameMsg[0].c_str()).cx;
			m_rcCurMsgInfo.bottom = m_rcGameMsgInfo.bottom;
			m_memdc.SelectObject(pOldFont);
			pOldFont = NULL;
		}
		//激活走马灯
		if (wInfoCount > 0)
		{
			int iCurMsgW = m_rcCurMsgInfo.Width();
			int iGameMsgW = m_rcGameMsgInfo.Width();
			if ((wInfoCount > 1) || (iCurMsgW>iGameMsgW))
			{
				m_rcCurMsgInfo.left -= m_wStartXAbs;
				m_rcCurMsgInfo.right -= m_wStartXAbs;
			}
			else
			{
				//暂时不用走马灯
				m_rcCurMsgInfo.left = (iGameMsgW-iCurMsgW)/2;
				m_rcCurMsgInfo.right = m_rcCurMsgInfo.left+iCurMsgW;
			}
		}
	}

	__LEAVE_FUNCTION
}
//解析公告
void CGameAffiche::ParseChar(const char* pData, WORD wDataSize)
{
	__ENTER_FUNCTION

	if (NULL != pData && wDataSize>0)
	{
		m_vecGameMsg.clear();

		WORD iCountIndex = 0;
		char* pNewData = new char[wDataSize];
		if (NULL == pNewData)
		{
			return;
		}
		::memset(pNewData, 0, wDataSize);
		const char* pOldData = pData;
		while ((*pData != '\0') && (iCountIndex<wDataSize))
		{
			if ('|'== *pData)
			{
				if (('*' == *(pData+1))
					&& ('*' == *(pData+2))
					&& ('|' == *(pData+3)))
				{
					::memset(pNewData, 0, wDataSize);
					::memcpy(pNewData, pOldData, pData-pOldData);
					m_vecGameMsg.push_back(pNewData);
					pOldData = pData+4;
					pData = pData+3;
				}
			}
			pData++;
			++iCountIndex;
		}
		delete pNewData;
		pNewData = NULL;
	}

	__LEAVE_FUNCTION
}

//设置显示区域
void CGameAffiche::SetShowRect(CRect rect)
{
	m_rcGameMsgInfo = rect;
}

//设置背景图
void CGameAffiche::SetBackImage(const char* pImage, bool bIsRgnWnd)
{
	__ENTER_FUNCTION

	CThreadGuard Guard(&m_CriticalSpection);

	if (NULL != pImage)
	{
		m_ImageBack.LoadImage(TEXT(pImage));

		if (!m_ImageBack.IsNull() && bIsRgnWnd)
		{
			SetImageRgn(m_hWnd, &m_ImageBack);
		}

		CRect rect;
		GetClientRect(&rect);
		CPaintDC dc(this);
		DeleteDC(m_memdc.GetSafeHdc());
		m_memdc.m_hDC = NULL;
		CBitmap bmp;
		m_memdc.CreateCompatibleDC(&dc);
		bmp.CreateCompatibleBitmap(&dc, m_ImageBack.GetWidth(), m_ImageBack.GetHeight());
		m_memdc.SelectObject(bmp);	
		m_memdc.SetBkMode(TRANSPARENT);
		UpdateWindow();
		DeleteObject(bmp);

		InvalidateRect(NULL, TRUE);
	}

	__LEAVE_FUNCTION
}

//设置字体颜色
void CGameAffiche::SetTextColor(COLORREF textcol)
{
	m_bTextColor = textcol;
}

//设置X轴开始位置
void CGameAffiche::SetXAbs(DWORD dwXAbs)
{
	m_wStartXAbs = dwXAbs;
}

//设置步长
void CGameAffiche::SetMoveLeep(DWORD dwLeep)
{
	m_dwMoveLeep = dwLeep;
}
//设置时间频率
void CGameAffiche::SetTimerInterval(DWORD dwTimeInterval)
{
	m_dwTimeInterval = dwTimeInterval;
}
//开始播放
void CGameAffiche::SetPlay(bool bIsPlay, int iLoopNums)
{
	__ENTER_FUNCTION

	WORD wInfoCount = m_vecGameMsg.size();
	//激活走马灯
	if (wInfoCount > 0)
	{
		m_iLoopNums = 0;
		m_iCountLoop = 0;
		if (!bIsPlay)
		{
			m_bIsDisableGameMsg = false;
			KillTimer(IDI_ROOM_AFFICHE_INFO);
		}
		else
		{
			m_iLoopNums = iLoopNums;
			m_bIsDisableGameMsg = true;
			int iCurMsgW = m_rcCurMsgInfo.Width();
			int iGameMsgW = m_rcGameMsgInfo.Width();
			if ((wInfoCount > 1) || (iCurMsgW>iGameMsgW))
			{
				m_rcCurMsgInfo.left -= m_wStartXAbs;
				m_rcCurMsgInfo.right -= m_wStartXAbs;
				SetTimer(IDI_ROOM_AFFICHE_INFO, m_dwTimeInterval, NULL);
			}
			else
			{
				//暂时不用走马灯
				m_rcCurMsgInfo.left = (iGameMsgW-iCurMsgW)/2;
				m_rcCurMsgInfo.right = m_rcCurMsgInfo.left+iCurMsgW;
			}
		}

		InvalidateRect(NULL, TRUE);
	}

	__LEAVE_FUNCTION
}

//设置为扩展窗口
void CGameAffiche::SetWndExp(DWORD wAlpha)
{
	__ENTER_FUNCTION

	SetWindowLong(this->GetSafeHwnd(), GWL_EXSTYLE, GetWindowLong(this->GetSafeHwnd(), GWL_EXSTYLE)^0x80000);
	// 添加库
	HINSTANCE hInst = LoadLibrary("User32.DLL");
	if(hInst)
	{ 
		typedef BOOL (WINAPI *ShowLayer)(HWND,COLORREF,BYTE,DWORD); 
		ShowLayer fun = NULL;

		// 读取函数指针
		fun = (ShowLayer)GetProcAddress(hInst, "SetLayeredWindowAttributes");
		if (fun)
		{
			//// -- 128 透明度  最后一个参数 1:透明一种颜色, 2: 窗体透明
			fun(this->GetSafeHwnd(), 0, wAlpha, 2);
			m_bIsExp = true;
		}
		FreeLibrary(hInst); 
	}

	__LEAVE_FUNCTION
}
//获得是否为扩展窗口
bool CGameAffiche::GetIsExp()
{
	return m_bIsExp;
}

void CGameAffiche::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
}

void CGameAffiche::OnTimer(UINT nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	__ENTER_FUNCTION

	CThreadGuard Guard(&m_CriticalSpection);

	switch(nIDEvent)
	{
	case IDI_ROOM_AFFICHE_INFO://房间公告信息定时器
		{
			if (m_bIsDisableGameMsg)
			{
				m_rcCurMsgInfo.left -= m_dwMoveLeep;
				m_rcCurMsgInfo.right -= m_dwMoveLeep;
				if (m_rcCurMsgInfo.left <= m_rcGameMsgInfo.left)
				{
					m_rcCurMsgInfo.left = m_rcGameMsgInfo.left;
				}
				if (m_rcCurMsgInfo.right < m_rcGameMsgInfo.left)
				{
					++m_iCountVecIndex;
					if (m_iCountVecIndex>=m_vecGameMsg.size())
					{
						if (m_iLoopNums>0)
						{
							++m_iCountLoop;
							if (m_iCountLoop>=m_iLoopNums)
							{
								KillTimer(IDI_ROOM_AFFICHE_INFO);
								ShowWindow(SW_HIDE);
							}
						}
						m_iCountVecIndex = 0;
					}
					if ((m_vecGameMsg.size()>0) && (m_iCountVecIndex<m_vecGameMsg.size()))
					{
						CFont * pOldFont=m_memdc.SelectObject(&m_fontDrawGameInfo);
						m_rcCurMsgInfo.left = m_rcGameMsgInfo.right;
						m_rcCurMsgInfo.top = m_rcGameMsgInfo.top;
						m_rcCurMsgInfo.right = m_rcCurMsgInfo.left+m_memdc.GetTextExtent(m_vecGameMsg[m_iCountVecIndex].c_str()).cx;
						m_rcCurMsgInfo.bottom = m_rcGameMsgInfo.bottom;
						m_memdc.SelectObject(pOldFont);
						pOldFont = NULL;
					}
					else
					{
						KillTimer(IDI_ROOM_AFFICHE_INFO);
					}
				}
				Invalidate(FALSE);
			}
			break;
		}
	}

	__LEAVE_FUNCTION

	CDialog::OnTimer(nIDEvent);
}

BOOL CGameAffiche::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_bIsExp = false;
	m_bTextColor = RGB(255, 255, 255);
	m_wStartXAbs = 0;
	m_dwMoveLeep = 1;
	m_dwTimeInterval = TIMER_ROOM_AFFICHE_INFO;
	// TODO:  在此添加额外的初始化
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

BOOL CGameAffiche::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	return TRUE;
	//return CDialog::OnEraseBkgnd(pDC);
}
