#include "StdAfx.h"
#include ".\gamecard.h"
#include "GameClientView.h"

#include "TraceCatch.h"

CGameCard::CGameCard() : m_pViewWnd(NULL), m_pImageBack(NULL)
{
	__ENTER_FUNCTION 

	m_vCardList.clear();
	m_bIsPositively = true;
	m_bIsShow = true;

	//牌宽与高
	m_iCardWidth = 0;
	m_iCardHeight = 0;

	//牌选择宽与高
	m_iSelCardWidth = 0;
	m_iSelCardHeight = 0;

	//牌间隔
	m_iCardSpace = 23;
	//牌弹起高度
	m_iCardShootHeight = 18;

	//牌起点
	m_ptDrawStartXY.x = 0;
	m_ptDrawStartXY.y = 0;

	m_iMouseCurIndex = -1;
	m_iMouseDownIndex = -1;
	m_bIsMouseDown = false;

	m_CardXCollocateMode = enGCXCenter;

	m_memdc.m_hDC = NULL;
	__LEAVE_FUNCTION
}

CGameCard::~CGameCard(void)
{
	__ENTER_FUNCTION 

	m_vCardList.clear();
	if (!m_ImageCard.IsNull())
	{
		m_ImageCard.DestroyImage();
	}
	if (!m_ImageSelCard.IsNull())
	{
		m_ImageSelCard.DestroyImage();
	}

	if (NULL!=m_memdc.m_hDC)
	{
		m_memdc.DeleteDC();
	}
	//m_memdc.m_hDC = NULL;
	m_pViewWnd = NULL;
	__LEAVE_FUNCTION
}

//设置视图窗口指针
void CGameCard::SetViewWndPtr(CWnd* pViewWnd)
{
	__ENTER_FUNCTION 

	m_pViewWnd = pViewWnd;
	__LEAVE_FUNCTION
}

//添加牌数据
int CGameCard::SetCardData(const BYTE* pCardData, int iSize)
{
	int iCount = 0;
	__ENTER_FUNCTION 
	m_vCardList.clear();
	if(NULL!=pCardData && iSize>0)
	{
		tagGameCardItem item;
		for (int i=0; i<iSize; ++i)
		{
			item.bCardData = pCardData[i];	//牌数据
			item.bShoot = false;			//不弹起
			m_vCardList.push_back(item);
			++iCount;
		}
	}
	UpdateGameCard();
	__LEAVE_FUNCTION
	return iCount;
}

//获得牌数据
int CGameCard::GetCardData(BYTE* pCardData, int iSize)
{
	int iCount = 0;
	__ENTER_FUNCTION 
	int iCardNums = m_vCardList.size();
	if(NULL!=pCardData && iSize>0 && iCardNums>0)
	{
		for (int i=0; i<iCardNums&&i<iSize; ++i)
		{
			pCardData[iCount++] = m_vCardList[i].bCardData;
		}
	}
	__LEAVE_FUNCTION
	return iCount;
}

//获得弹起的牌
int CGameCard::GetShootCard(BYTE* pCardShootCard, int iSize)
{
	int iCount = 0;
	__ENTER_FUNCTION 
	int iCardNums = m_vCardList.size();
	if(NULL!=pCardShootCard && iSize>0 && iCardNums>0)
	{
		for (int i=0; i<iCardNums&&i<iSize; ++i)
		{
			if(m_vCardList[i].bShoot==true)
			{
				pCardShootCard[iCount++] = m_vCardList[i].bCardData;
			}
		}
	}
	__LEAVE_FUNCTION
	return iCount;	
}

//设置弹起的牌
int CGameCard::SetShootCard(BYTE* pCardShootCard, int iSize)
{
	int iCount = 0;
	__ENTER_FUNCTION 
	int iCardNums = m_vCardList.size();
	if (NULL!=pCardShootCard && iSize>0 && iCardNums)
	{
		//设置所有牌都不弹起.
		SetAllShoot(false);

		for (int i=0; i<iSize; ++i)
		{
			for (int j=0; j<iCardNums; ++j)
			{
				if (false==m_vCardList[j].bShoot && m_vCardList[j].bCardData==pCardShootCard[i])
				{
					m_vCardList[j].bShoot = true;
					++iCount;
					break;
				}
			}
		}
	}
	UpdateGameCard();
	__LEAVE_FUNCTION
	return iCount;
}

//设置所有牌不弹起
void CGameCard::SetAllShoot(bool bIsShott)
{
	__ENTER_FUNCTION 

	int iCardNums = m_vCardList.size();
	for (int i=0; i<iCardNums; ++i)
	{
		m_vCardList[i].bShoot = bIsShott;
	}
	UpdateGameCard();
	__LEAVE_FUNCTION
}

//设置响应
void CGameCard::SetCardPositively(bool bIsPositively)
{
	__ENTER_FUNCTION 

	m_bIsPositively = bIsPositively;
	__LEAVE_FUNCTION
}
//设置显示
void CGameCard::SetCardShow(bool bIsShow)
{
	__ENTER_FUNCTION 

	m_bIsShow = bIsShow;
	InvalidateCard();
	__LEAVE_FUNCTION
}

//设置牌背景
void CGameCard::SetCardBackImage(CPngImage* pBackImage)
{
	__ENTER_FUNCTION 

	m_pImageBack = pBackImage;
	__LEAVE_FUNCTION
}

//设置牌资源
bool CGameCard::SetCardImage(CString strImagePath)
{
	__ENTER_FUNCTION 

	if (!strImagePath.IsEmpty())
	{
		//m_ImageCard.DestroyImage();
		m_iCardWidth = 0;
		m_iCardHeight = 0;
		bool bIsSuc = m_ImageCard.LoadImage(strImagePath);
		if(bIsSuc/* && !m_ImageCard.IsNull()*/)
		{
			m_iCardWidth = m_ImageCard.GetWidth()/13;
			m_iCardHeight = m_ImageCard.GetHeight()/5;
		}
		DrawCardData();
		InvalidateCard();
		return bIsSuc;
	}
	__LEAVE_FUNCTION
	return false;	
}

//设置牌选择
bool CGameCard::SetCardSelImage(CString strSelImagePath)
{
	__ENTER_FUNCTION 

	if (!strSelImagePath.IsEmpty())
	{
		m_ImageSelCard.DestroyImage();
		m_iSelCardWidth = 0;
		m_iSelCardHeight = 0;
		bool bIsSuc = m_ImageSelCard.LoadImage(strSelImagePath);
		if(bIsSuc && !m_ImageSelCard.IsNull())
		{
			m_iSelCardWidth = m_ImageSelCard.GetWidth();
			m_iSelCardHeight = m_ImageSelCard.GetHeight();
		}
		DrawCardData();
		InvalidateCard();
		return bIsSuc;
	}
	__LEAVE_FUNCTION
	return false;	
}

//设置牌间隔
void CGameCard::SetCardSpace(int iCardSpace)
{
	__ENTER_FUNCTION 

	m_iCardSpace = iCardSpace;
	__LEAVE_FUNCTION
	UpdateGameCard();
	
}

//设置牌位置
void CGameCard::SetCardPosition(CPoint ptPosition, enGameCardXCollocateMode CardMode)
{
	__ENTER_FUNCTION 

	m_ptOldPosition = ptPosition;
	m_CardXCollocateMode = CardMode;

	UpdateCardPosition();
	__LEAVE_FUNCTION
}

//获得牌宽与高
int CGameCard::GetCardControlWidth()
{
	int iCardNums = m_vCardList.size();
	return (m_iCardWidth+(iCardNums-1)*m_iCardSpace);
	
}
int CGameCard::GetCardControlHeight()
{
	return m_iCardHeight+m_iCardShootHeight;
}

void CGameCard::UpdateCardPosition()
{
	__ENTER_FUNCTION 

	int iCardNums = m_vCardList.size();
	if (enGCXLeft==m_CardXCollocateMode)
	{//左对齐
		m_ptDrawStartXY = m_ptOldPosition;
	}
	else if (enGCXRight==m_CardXCollocateMode)
	{//右对齐
		m_ptDrawStartXY.x = m_ptOldPosition.x-(m_iCardWidth+iCardNums*(m_iCardSpace-1));
	}
	else
	{//居中对齐
		m_ptDrawStartXY.x = m_ptOldPosition.x-(m_iCardWidth+iCardNums*(m_iCardSpace-1))/2;
	}
	m_ptDrawStartXY.y = m_ptOldPosition.y;

	m_rcGameCard.left = m_ptDrawStartXY.x;
	m_rcGameCard.top = m_ptDrawStartXY.y;
	m_rcGameCard.right = m_rcGameCard.left + (m_iCardWidth+(iCardNums-1)*m_iCardSpace);
	m_rcGameCard.bottom = m_rcGameCard.top + m_iCardHeight+m_iCardShootHeight;

	DrawCardData();
	InvalidateCard();
	__LEAVE_FUNCTION
}

//更新牌
void CGameCard::UpdateGameCard()
{
	__ENTER_FUNCTION 

	UpdateCardMem();
	UpdateCardPosition();
	DrawCardData();
	InvalidateCard();
	__LEAVE_FUNCTION
}

//获取牌区域
void CGameCard::GetCardDrawRect(CRect& rect)
{
	__ENTER_FUNCTION 

	rect.left = m_ptDrawStartXY.x;
	rect.top = m_ptDrawStartXY.y;
	rect.right = rect.left + GetCardControlWidth();
	rect.bottom = rect.top + GetCardControlHeight();
	__LEAVE_FUNCTION
}

//刷新牌区域
void CGameCard::InvalidateCard()
{
	__ENTER_FUNCTION 

	if (NULL != m_pViewWnd)
	{
		CRect rect;
		rect.left = m_ptDrawStartXY.x;
		rect.top = m_ptDrawStartXY.y;
		rect.right = rect.left + GetCardControlWidth();
		rect.bottom = rect.top + GetCardControlHeight();
		((CGameClientView*)m_pViewWnd)->UpdateGameView(&rect);
	}
	__LEAVE_FUNCTION
}

//更新缓冲
void CGameCard::UpdateCardMem()
{
	__ENTER_FUNCTION 

	CThreadGuard guard(&m_lockMemDC);

	int iWidth = GetCardControlWidth();
	int iHeight = GetCardControlHeight();
	if (NULL!=m_memdc.m_hDC)
	{
		m_memdc.DeleteDC();
	}
	CDC* pDC = m_pViewWnd->GetDC();
	//m_memdc.m_hDC = NULL;
	m_memdc.CreateCompatibleDC(pDC);
	CBitmap hBmp;
	hBmp.CreateCompatibleBitmap(pDC, iWidth, iHeight);
	m_memdc.SelectObject(&hBmp);
	m_memdc.SetBkMode(TRANSPARENT);
	hBmp.DeleteObject();
	::ReleaseDC(m_pViewWnd->GetSafeHwnd(), pDC->GetSafeHdc());
	pDC = NULL;
	__LEAVE_FUNCTION
}

//绘制牌
void CGameCard::DrawCardData()
{
	__ENTER_FUNCTION 

	CThreadGuard guard(&m_lockMemDC);

	if (NULL != m_memdc.m_hDC)
	{
		int iWidth = GetCardControlWidth();
		int iHeight = GetCardControlHeight();
		if (NULL != m_pImageBack)
		{
			m_pImageBack->DrawImage(&m_memdc, 0, 0, iWidth, iHeight, \
				m_ptDrawStartXY.x, m_ptDrawStartXY.y, iWidth, iHeight);
		}

		CPoint ptStart;
		ptStart.x = 0;
		ptStart.y = 0;

		int iCardNums = m_vCardList.size();
		int iMin = -1, iMax = -1;
		if (m_bIsMouseDown && m_iMouseDownIndex>=0 && m_iMouseDownIndex<iCardNums)
		{
			if (m_iMouseDownIndex>m_iMouseCurIndex)
			{
				iMin = m_iMouseCurIndex;
				iMax = m_iMouseDownIndex;
			}
			else
			{
				iMin = m_iMouseDownIndex;
				iMax = m_iMouseCurIndex;
			}
		}
		
		for (int i=0; i<iCardNums; ++i)
		{
			BYTE byCardData = m_vCardList[i].bCardData;
			int iXImagePos=0, iYImagePos=0, iShootY=0; 
			if ((byCardData==0x4E)||(byCardData==0x4F))
			{
				iXImagePos=((byCardData&CARD_MASK_VALUE)%14)*m_iCardWidth;
				iYImagePos=((byCardData&CARD_MASK_COLOR)>>4)*m_iCardHeight;
			}
			else
			{
				iXImagePos=((byCardData&CARD_MASK_VALUE)-1)*m_iCardWidth;
				iYImagePos=((byCardData&CARD_MASK_COLOR)>>4)*m_iCardHeight;
			}
			iShootY = m_vCardList[i].bShoot?(0):(m_iCardShootHeight);
			m_ImageCard.DrawImage(&m_memdc,ptStart.x, iShootY, m_iCardWidth, m_iCardHeight, \
				iXImagePos, iYImagePos, m_iCardWidth, m_iCardHeight);
			if (m_bIsMouseDown && i>=iMin && i<=iMax)
			{
				m_ImageSelCard.DrawImage(&m_memdc,ptStart.x, iShootY, m_iCardWidth, m_iCardHeight, \
					0, 0, m_iCardWidth, m_iCardHeight);
			}
			//else if (i==m_iMouseCurIndex)
			//{
			//	m_ImageSelCard.DrawImage(&m_memdc,ptStart.x, iShootY, m_iCardWidth, m_iCardHeight, \
			//		0, 0, m_iCardWidth, m_iCardHeight);
			//}
			ptStart.x += m_iCardSpace;
		}
	}
	__LEAVE_FUNCTION
}

//绘制牌
void CGameCard::DrawGameCard(CDC* pDC)
{
	__ENTER_FUNCTION 

	CThreadGuard guard(&m_lockMemDC);

	if (m_bIsShow)
	{
		int iCardNums = m_vCardList.size();
		if (iCardNums>0 && NULL!=m_memdc.m_hDC)
		{
			int iWidth = GetCardControlWidth();
			int iHeight = GetCardControlHeight();
			pDC->BitBlt(m_ptDrawStartXY.x, m_ptDrawStartXY.y, iWidth, iHeight, &m_memdc, 0, 0, SRCCOPY);
		}
	}
	__LEAVE_FUNCTION
} 
//索引切换
int CGameCard::SwitchCardPoint(const CPoint &Point)
{
	int iCardNums = m_vCardList.size();
	int iX = Point.x - m_rcGameCard.left;
	int iIndex = iX/m_iCardSpace;
	__ENTER_FUNCTION 
	if (iIndex<iCardNums)
	{
		iIndex = iX/m_iCardSpace;
	}
	else if (Point.x>=m_rcGameCard.left+(iCardNums-1)*m_iCardSpace 
			&& Point.x<=m_rcGameCard.left+iCardNums*m_iCardWidth)
	{
		iIndex = iCardNums-1;
	}
	__LEAVE_FUNCTION
	return iIndex;
	
}
//鼠标消息(左键按下)
void CGameCard::OnLButtonDown(CPoint Point)
{
	__ENTER_FUNCTION 

	if (m_rcGameCard.PtInRect(Point) && m_bIsPositively)
	{
		m_bIsMouseDown = true;
		m_iMouseDownIndex = SwitchCardPoint(Point);
	}
	__LEAVE_FUNCTION
}
//鼠标消息(左键弹起)
void CGameCard::OnLButtonUp(CPoint Point)
{
	__ENTER_FUNCTION 

	if (m_rcGameCard.PtInRect(Point) && m_bIsPositively)
	{
		int iIndex = SwitchCardPoint(Point);
		if (-1 != iIndex)
		{
			int iCardNums = m_vCardList.size();
			if (iIndex>=0&&iIndex<iCardNums)
			{
				if (m_iMouseDownIndex==m_iMouseCurIndex)
				{
					m_vCardList[iIndex].bShoot = !m_vCardList[iIndex].bShoot;
				}
				else if (m_bIsMouseDown && m_iMouseDownIndex>=0 
					&& m_iMouseDownIndex<iCardNums 
					&& m_iMouseDownIndex!=m_iMouseCurIndex)
				{
					int iMin = -1, iMax = -1;
					if (m_iMouseDownIndex>m_iMouseCurIndex)
					{
						iMin = m_iMouseCurIndex;
						iMax = m_iMouseDownIndex;
					}
					else
					{
						iMin = m_iMouseDownIndex;
						iMax = m_iMouseCurIndex;
					}
					for (int i=iMin; i<iCardNums&&i<=iMax; ++i)
					{
						m_vCardList[i].bShoot = !m_vCardList[i].bShoot;
					}
				}

				DrawCardData();
				InvalidateCard();
				m_bIsMouseDown = false;
			}
		}
	}
	__LEAVE_FUNCTION
}
//鼠标移动
void CGameCard::OnMouseMove(CPoint Point)
{
	__ENTER_FUNCTION 

	if (m_rcGameCard.PtInRect(Point) && m_bIsPositively)
	{
		int iIndex = SwitchCardPoint(Point);
		if (-1!=iIndex && iIndex!=m_iMouseCurIndex)
		{
			m_iMouseCurIndex = iIndex;
			if (!m_bIsMouseDown)
			{
				m_iMouseDownIndex=-1;
			}
			DrawCardData();
			InvalidateCard();
		}
	}
	else
	{
		if (-1 != m_iMouseCurIndex)
		{
			m_iMouseCurIndex = -1;
			m_iMouseDownIndex = -1;
			DrawCardData();
			InvalidateCard();
		}
	}
	__LEAVE_FUNCTION
}