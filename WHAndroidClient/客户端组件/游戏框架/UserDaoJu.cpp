// UserDaoJu.cpp : 实现文件
//

#include "StdAfx.h"
#include "resource.h"
#include "GlobalOption.h"
#include "UserInfoView.h"
#include "GameFrameControl.h"
#include "UserDaoJu.h"
#include ".\userdaoju.h"
#include "GameClientDJInfo.h"
#include "ShowDJInfo.h"
#include "GameFrameNetDefine.h"


#define		IDI_TIME_TASK_BT_DEF		1023
#define		ID_TIME_TASK_INTERVAL		1000

//绘制道具的信息
#define		DJ_ICO_BACK_WIDTH			24
#define		DJ_ICO_BACK_HEIGHT			24

#define		DJ_ICO_WIDTH				20
#define		DJ_ICO_HEIGHT				20

#define		DRAW_DJ_X					14
#define		DRAW_DJ_Y					1

#define		DRAW_DJ_X_ABS				2
#define		DRAW_DJ_Y_ABS				2

#define		DRAW_DJ_INTERVAL			3

#define		DRAW_DJ_NUM_X				1
#define		DRAW_DJ_NUM_Y				8

#define		DJ_SHOW_PAGE_NUMS			7


// CUserDaoJu

IMPLEMENT_DYNAMIC(CUserDaoJu, CWnd)
CUserDaoJu::CUserDaoJu(CGameFrameDlg * pGameFrameDlg):m_pGameFrameDlg(pGameFrameDlg)
{
	m_bIsTimer = false;
	m_bHovering=false;
	m_iTotalPage = 0;
	m_iCurrentPage = 0;
	m_pShowDJInfo = NULL;
}

CUserDaoJu::~CUserDaoJu()
{
}


BEGIN_MESSAGE_MAP(CUserDaoJu, CWnd)
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_BN_CLICKED(IDC_BT_USERDJ_LEFT, OnBnLeftClickedEvent)
	ON_BN_CLICKED(IDC_BT_USERDJ_RIGHT, OnBnRightClickedEvent)
	ON_MESSAGE(WM_MOUSELEAVE,OnMouseLeave)

	ON_WM_TIMER()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
//	ON_WM_NCMOUSEMOVE()
ON_WM_LBUTTONDOWN()
ON_WM_RBUTTONDOWN()
END_MESSAGE_MAP()



//绘画函数
VOID CUserDaoJu::OnPaint()
{
	if(m_ImageWndBack.IsNull())
	{
		return;
	}
	if (m_DaoJuImageBuffer.IsNull())
	{
		return;
	}

	//变量定义
	CPaintDC dc(this);
	CDC * pDC=CDC::FromHandle(m_DaoJuImageBuffer.GetDC());

	int iWndWidth = m_ImageWndBack.GetWidth();
	int iWndHeight = m_ImageWndBack.GetHeight();

	//创建字体
	CFont DrawFont;
	DrawFont.CreateFont(-13,0,0,0,400,0,0,0,134,3,2,1,2,TEXT("宋体"));
	CFont * pOldFont=pDC->SelectObject(&DrawFont);

	pDC->SetBkMode(TRANSPARENT);
	pDC->FillSolidRect(0,0,iWndWidth,iWndHeight,RGB(0,0,0));

	//绘画背景图片
	m_ImageWndBack.DrawImage(pDC, 0, 0);

	int iCountIndex = 0;
	if ((m_iTotalPage > 0) && (NULL != m_pUserDJInfo))
	{
		//有道具
		//绘制道具信息
		for (int i=m_iCurrentPage*DJ_SHOW_PAGE_NUMS,k=0; i<m_pUserDJInfo->m_vecUserHaveDJList.size()&&k<DJ_SHOW_PAGE_NUMS;i++,k++)
		{
			{
				//绘制道具
				//绘制道具图片
				CPngImage* pDJImage = m_pUserDJInfo->GetImageFromeDJID(m_pUserDJInfo->m_vecUserHaveDJList[i].idjID);
				if (NULL != pDJImage && !pDJImage->IsNull())
				{
					pDJImage->DrawImage(pDC, DRAW_DJ_X+DRAW_DJ_X_ABS+iCountIndex*(DJ_ICO_BACK_WIDTH+DRAW_DJ_INTERVAL), DRAW_DJ_Y+DRAW_DJ_Y_ABS);

					//绘制鼠标响应
					if (m_iCurrentMousePoint == k)
					{
						pDJImage = NULL;
						pDJImage = m_pUserDJInfo->GetLightImageFromeDJID(m_pUserDJInfo->m_vecUserHaveDJList[i].idjID);
						if (NULL != pDJImage && !pDJImage->IsNull())
						{
							pDJImage->DrawImage(pDC, DRAW_DJ_X+DRAW_DJ_X_ABS+iCountIndex*(DJ_ICO_BACK_WIDTH+DRAW_DJ_INTERVAL), DRAW_DJ_Y+DRAW_DJ_Y_ABS);
						}
					}
				}
				if (m_pUserDJInfo->m_vecUserHaveDJList[i].idjNums>0)
				{
					//绘制数字
					pDC->SetTextColor(RGB(255, 255, 255));
					char chNumData[10]={0};
					sprintf(chNumData, "%d", m_pUserDJInfo->m_vecUserHaveDJList[i].idjNums);

					RECT textrect;
					textrect.left = DRAW_DJ_X+DRAW_DJ_X_ABS+iCountIndex*(DJ_ICO_BACK_WIDTH+DRAW_DJ_INTERVAL)+DRAW_DJ_NUM_X;
					textrect.top = DRAW_DJ_Y+DRAW_DJ_Y_ABS+DRAW_DJ_NUM_Y;
					textrect.right = textrect.left + 8;
					textrect.bottom = textrect.top + 20;
					DrawTextString(pDC, chNumData, RGB(255, 255, 255), RGB(0, 0, 0), textrect.left, textrect.top);
				}
				++iCountIndex;
			}
		}
	}
	
	//绘画界面
	m_DaoJuImageBuffer.BitBlt(dc,0,0,iWndWidth,iWndHeight,0,0);

	//清理资源
	pDC->SelectObject(pOldFont);

	m_DaoJuImageBuffer.ReleaseDC();
	DrawFont.DeleteObject();

	ReleaseDC(pDC);			// add by HouGuoJiang 2011-11-25
	CDC::DeleteTempMap();	// add by HouGuoJiang 2011-11-25
}


//创建消息
INT CUserDaoJu::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (__super::OnCreate(lpCreateStruct)==-1) return -1;

	TCHAR szResourcePath[MAX_PATH];	GetCurrentDirectory(MAX_PATH,szResourcePath);
	CString strPath(szResourcePath);
	strPath.Append("\\data\\gameframe\\plaza\\");


	//加载资源
	m_DaoJuImageBuffer.Destroy();
	m_DaoJuImageBuffer.Create(214, 26, 24);

	m_ImageWndBack.LoadImage(TEXT(strPath + "DaoJuBack.png"));

	m_ToolTipCtrl.Create(this);
	m_ToolTipCtrl.Activate(TRUE);

	//创建控件
	CRect CreateRect(0,0,0,0);

	//创建按钮
	m_BTLeft.Create(TEXT(""),WS_CHILD|WS_VISIBLE,CreateRect,this,IDC_BT_USERDJ_LEFT);
	m_BTRight.Create(TEXT(""),WS_CHILD|WS_VISIBLE,CreateRect,this,IDC_BT_USERDJ_RIGHT);

	//设置按钮
	m_BTLeft.SetButtonImage(TEXT(strPath + "UserDJL.png"), false, 4);
	m_BTRight.SetButtonImage(TEXT(strPath + "UserDJR.png"), false, 4);
	const UINT uFlags=SWP_NOACTIVATE|SWP_NOZORDER|SWP_NOCOPYBITS|SWP_NOSIZE;

	m_BTLeft.SetWindowPos(NULL, 1, 0, 12, 26, 0);//左
	m_BTRight.SetWindowPos(NULL, 201, 0, 12, 26, 0);//右

	if(GetDlgItem(IDC_BT_USERDJ_ACTIVE))
		m_ToolTipCtrl.AddTool(GetDlgItem(IDC_BT_USERDJ_ACTIVE),TEXT("动作"));
	
	m_BTLeft.ShowWindow(SW_SHOW);
	m_BTRight.ShowWindow(SW_SHOW);

	//默认没有道具
	m_iTotalPage = 0;
	m_iCurrentPage = 0;
	m_BTLeft.EnableWindow(FALSE);
	m_BTRight.EnableWindow(FALSE);

	//获得道具信息
	m_pUserDJInfo = CGameClientDJInfo::GetDJInfoInstance();

	SetImageRgn(m_hWnd, &m_ImageWndBack, SIRT_FULL_DOT_LESS);
	return 0;
}

//
//BOOL CUserDaoJu::SetImageRgn(CPngImage * pImage)
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


//按钮事件
void CUserDaoJu::OnBnLeftClickedEvent()
{//向左
	if (m_iCurrentPage>0)
	{
		--m_iCurrentPage;
		CRect DrawRect(DRAW_DJ_X, DRAW_DJ_Y, DRAW_DJ_X+DJ_SHOW_PAGE_NUMS*(DJ_ICO_BACK_WIDTH+DRAW_DJ_INTERVAL), DRAW_DJ_Y+DJ_ICO_BACK_HEIGHT);
		InvalidateRect(DrawRect);
		if (m_iCurrentPage <= 0)
		{
			m_BTLeft.EnableWindow(FALSE);
		}
		if (m_iTotalPage > 0)
		{
			m_BTRight.EnableWindow(TRUE);
		}
	}
}

void CUserDaoJu::OnBnRightClickedEvent()
{//向右
	if (m_iCurrentPage<m_iTotalPage-1)
	{
		++m_iCurrentPage;
		CRect DrawRect(DRAW_DJ_X, DRAW_DJ_Y, DRAW_DJ_X+DJ_SHOW_PAGE_NUMS*(DJ_ICO_BACK_WIDTH+DRAW_DJ_INTERVAL), DRAW_DJ_Y+DJ_ICO_BACK_HEIGHT);
		InvalidateRect(DrawRect);

		if (m_iCurrentPage==m_iTotalPage-1)
		{
			m_BTRight.EnableWindow(FALSE);
		}
		if (m_iTotalPage > 0)
		{
			m_BTLeft.EnableWindow(TRUE);
		}
	}
}

//设置显示道具信息指针
void CUserDaoJu::SetShowDJInfo(CShowDJInfo* pShowDJInfoPtr)
{
	if (NULL != pShowDJInfoPtr)
	{
		m_pShowDJInfo = pShowDJInfoPtr;
	}
}

// CUserDaoJu 消息处理程序}
void CUserDaoJu::OnTimer(UINT nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CWnd::OnTimer(nIDEvent);
}


//艺术字体
void CUserDaoJu::DrawTextString(CDC * pDC, LPCTSTR pszString, COLORREF crText, COLORREF crFrame, int nXPos, int nYPos)
{
	//变量定义
	int nStringLength=lstrlen(pszString);
	int nXExcursion[8]={1,1,1,0,-1,-1,-1,0};
	int nYExcursion[8]={-1,0,1,1,1,0,-1,-1};

	//绘画边框
	pDC->SetTextColor(crFrame);
	for (int i=0;i<CountArray(nXExcursion);i++)
	{
		pDC->TextOut(nXPos+nXExcursion[i],nYPos+nYExcursion[i],pszString,nStringLength);
	}

	//绘画字体
	pDC->SetTextColor(crText);
	pDC->TextOut(nXPos,nYPos,pszString,nStringLength);

	return;
}

void CUserDaoJu::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	
	m_iCurrentMousePoint = -1;
	CRect DrawRect(DRAW_DJ_X, DRAW_DJ_Y, DRAW_DJ_X+DJ_SHOW_PAGE_NUMS*(DJ_ICO_BACK_WIDTH+DRAW_DJ_INTERVAL), DRAW_DJ_Y+DJ_ICO_BACK_HEIGHT);
	if (TRUE == DrawRect.PtInRect(point))
	{//在区域之内
		m_iCurrentMousePoint = (point.x-DRAW_DJ_X)/(DRAW_DJ_INTERVAL+DJ_ICO_BACK_WIDTH);
		if (m_iCurrentMousePoint>=0 && m_iCurrentMousePoint<DJ_SHOW_PAGE_NUMS)
		{
			if (NULL == m_pUserDJInfo) return;
			if (m_iCurrentPage*DJ_SHOW_PAGE_NUMS+m_iCurrentMousePoint < m_pUserDJInfo->m_vecUserHaveDJList.size())
			{
				if (NULL != m_pShowDJInfo)
				{
					m_pShowDJInfo->SetShowDJDiscription(m_iCurrentPage*DJ_SHOW_PAGE_NUMS+m_iCurrentMousePoint);
					m_pShowDJInfo->SetWndShow(true, m_iCurrentMousePoint);
				}
			}
			else
			{
				m_iCurrentMousePoint - 1;
				if (NULL != m_pShowDJInfo)
				{
					m_pShowDJInfo->SetWndShow(false);
				}
			}
			InvalidateRect(&DrawRect);
		}
	}
	else
	{
		if (NULL != m_pShowDJInfo)
		{
			m_pShowDJInfo->SetWndShow(false);
		}
	}
	
	if (m_bHovering==false)
	{
		//注册消息
		m_bHovering=true;
		Invalidate(FALSE);
		TRACKMOUSEEVENT TrackMouseEvent;
		TrackMouseEvent.cbSize=sizeof(TrackMouseEvent);
		TrackMouseEvent.dwFlags=TME_LEAVE;
		TrackMouseEvent.hwndTrack=GetSafeHwnd();
		TrackMouseEvent.dwHoverTime=HOVER_DEFAULT;
		_TrackMouseEvent(&TrackMouseEvent);
	}
	
	CWnd::OnMouseMove(nFlags, point);
}


void CUserDaoJu::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (NULL != m_pGameFrameDlg)
	{
		if (m_pGameFrameDlg->IsLookonMode())
		{
			return;
		}
	}
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	//CRect DrawRect(DRAW_DJ_X, DRAW_DJ_Y, DRAW_DJ_X+DJ_SHOW_PAGE_NUMS*(DJ_ICO_BACK_WIDTH+DRAW_DJ_INTERVAL), DRAW_DJ_Y+DJ_ICO_BACK_HEIGHT);
	//if (TRUE == DrawRect.PtInRect(point))
	//{//在区域之内
	//	if (m_iCurrentMousePoint>=0 && m_iCurrentMousePoint<DJ_SHOW_PAGE_NUMS)
	//	{//使用此道具
	//		if (NULL == m_pUserDJInfo) return;
	//		if (m_iCurrentPage*DJ_SHOW_PAGE_NUMS+m_iCurrentMousePoint < m_pUserDJInfo->m_vecUserHaveDJList.size())
	//		{/*
	//			char chIndexInfo[100]={0};
	//			sprintf(chIndexInfo, "你确定要使用道具:%d(Index:%d)吗？", m_iCurrentPage*DJ_SHOW_PAGE_NUMS+m_iCurrentMousePoint, m_iCurrentMousePoint);
	//			MessageBox(chIndexInfo);*/
	//			if (NULL != m_pGameFrameDlg)
	//			{
	//				int iSaveDJID = m_pUserDJInfo->GetDJIDFromDJIndex(m_iCurrentPage*DJ_SHOW_PAGE_NUMS+m_iCurrentMousePoint);
	//				if (iSaveDJID>=0)
	//				{
	//					CMD_GF_Active_Prop prop;
	//					::memset(&prop, 0, sizeof(CMD_GF_Active_Prop));
	//					prop.nPropertyID = iSaveDJID;
	//					prop.cbSendLocation = LOCATION_GAME_ROOM;
	//					m_pGameFrameDlg->m_ClientKernelHelper->SendSocketData(MDM_GF_PRESENT,SUB_GF_ACTIVE_PROP,&prop,sizeof(CMD_GF_Active_Prop));
	//					char chInfo[255]={0};
	//					sprintf(chInfo, "%d道具已尝试使用，使用消息已发送至服务器！", iSaveDJID);
	//					m_pGameFrameDlg->InsertGeneralString(chInfo, RGB(0, 255, 255), true);
	//				}
	//				
	//			}
	//			//DelUserUseDJFromeIndex(m_iCurrentPage*DJ_SHOW_PAGE_NUMS+m_iCurrentMousePoint);
	//		}
	//		else
	//		{
	//			m_iCurrentMousePoint - 1;
	//		}
	//	}
	//}
	CWnd::OnLButtonUp(nFlags, point);
}

//鼠标离开消息
LRESULT CUserDaoJu::OnMouseLeave(WPARAM wparam, LPARAM lparam)
{
	if (NULL != m_pGameFrameDlg)
	{
		m_pGameFrameDlg->SetShowTimeDJListWnd(false);
		if (NULL != m_pShowDJInfo)
		{
			m_pShowDJInfo->SetWndShow(false);
		}
		m_bHovering=false;
		InvalidateRect(NULL);
	}
	return 0;
}


//删除道具
bool CUserDaoJu::DelUserUseDJFromeIndex(int iIndex)
{
	if (NULL == m_pUserDJInfo || iIndex >= m_pUserDJInfo->m_vecUserHaveDJList.size())
	{
		return false;
	}

	int bRet = m_pUserDJInfo->DelUserDJFromIndex(iIndex);
	ASSERT(bRet==iIndex);
	if (iIndex != bRet)
	{//删除不成功
		return false;
	}
	else
	{
		UpdateUserDJInfo(bRet);
	}
	return true;
}


//重置道具页面
void CUserDaoJu::UpdateUserDJInfo(int iIndex)
{
	if (NULL == m_pUserDJInfo)
	{
		return;
	}
	int iTotalDJNums = m_pUserDJInfo->m_vecUserHaveDJList.size();
	if (iTotalDJNums>0)
	{//还有道具
		int iCountPage = iTotalDJNums/DJ_SHOW_PAGE_NUMS;//删除后的总页数
		if (0 != iTotalDJNums%DJ_SHOW_PAGE_NUMS)
		{
			++iCountPage;
		}
		if (iCountPage != m_iTotalPage)
		{//有变化
			m_iTotalPage = iCountPage;
			if (iIndex<DJ_SHOW_PAGE_NUMS)
			{//在第一页
				m_iCurrentPage = 0;
				//左边按钮不激活
				m_BTLeft.EnableWindow(FALSE);
			}
			else
			{//至前面一页
				--iIndex;
				m_iCurrentPage = iIndex/DJ_SHOW_PAGE_NUMS;//当前页
			}
			if (0 == iTotalDJNums%DJ_SHOW_PAGE_NUMS)
			{
				//右边按钮不激活
				m_BTRight.EnableWindow(FALSE);
			}
			if (iTotalDJNums<=DJ_SHOW_PAGE_NUMS)
			{
				//按钮不激活
				m_BTLeft.EnableWindow(FALSE);
				m_BTRight.EnableWindow(FALSE);
			}
		}
	}
	else
	{
		//按钮不激活
		m_BTLeft.EnableWindow(FALSE);
		m_BTRight.EnableWindow(FALSE);
	}
	InvalidateRect(NULL);
}


//更新道具信息
void CUserDaoJu::UpdateUserDJPage()
{
	//默认没有道具
	m_iTotalPage = 0;
	m_iCurrentPage = 0;

	if (NULL != m_pUserDJInfo)
	{
		int iTotalDJNums = m_pUserDJInfo->m_vecUserHaveDJList.size();
		if (iTotalDJNums > 0)
		{
			int iCountPage = iTotalDJNums/DJ_SHOW_PAGE_NUMS;//页数
			if (0 != iTotalDJNums%DJ_SHOW_PAGE_NUMS)
			{
				++iCountPage;
			}
			m_iTotalPage = iCountPage;//总页数
		}
	}
	m_BTLeft.EnableWindow(FALSE);
	if (m_iTotalPage>1)
	{
		m_BTRight.EnableWindow(TRUE);
	}
	else
	{
		m_BTRight.EnableWindow(FALSE);
	}
	//更新界面
	InvalidateRect(NULL);
}

void CUserDaoJu::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (NULL != m_pGameFrameDlg)
	{
		m_pGameFrameDlg->SetShowUserActionWnd(false);
	}
	CWnd::OnLButtonDown(nFlags, point);
}

void CUserDaoJu::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (NULL != m_pGameFrameDlg)
	{
		m_pGameFrameDlg->SetShowUserActionWnd(false);
	}
	CWnd::OnRButtonDown(nFlags, point);
}

BOOL CUserDaoJu::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	m_ToolTipCtrl.RelayEvent(pMsg);
	return CWnd::PreTranslateMessage(pMsg);
}
