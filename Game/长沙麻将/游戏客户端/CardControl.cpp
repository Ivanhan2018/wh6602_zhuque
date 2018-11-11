#include "StdAfx.h"
#include "Resource.h"
#include "CardControl.h"
#include "GameClientDlg.h"

//////////////////////////////////////////////////////////////////////////
//宏定义

//公共定义
#define POS_SHOOT					5									//弹起象素
#define POS_SPACE					8									//分隔间隔
#define ITEM_COUNT					29									//子项数目
#define INVALID_ITEM				0xFFFF								//无效索引

//扑克大小
#define CARD_WIDTH					48									//扑克宽度
#define CARD_HEIGHT					75									//扑克高度

//////////////////////////////////////////////////////////////////////////

//构造函数
CCardListImage::CCardListImage()
{
	//位置变量
	m_nItemWidth=0;
	m_nItemHeight=0;
	m_nViewWidth=0;
	m_nViewHeight=0;

	return;
}

//析构函数
CCardListImage::~CCardListImage()
{
}

//加载资源
/*
bool CCardListImage::LoadResource(char* pFileName, int nViewWidth, int nViewHeight)
{
	if (NULL != pFileName)
	{
		char chFileName[20]={0};
		sprintf(chFileName, TEXT("%s"), pFileName);

		TCHAR szResourcePath[MAX_PATH]="";
		GetCurrentDirectory(MAX_PATH,szResourcePath);
		CString strPath(szResourcePath);
		strPath.Append("\\SparrowCS\\Picture\\Card\\");
		strPath.Append(pFileName);
	    m_CardListImage.LoadImage((LPCTSTR)strPath);
		if (!m_CardListImage.IsNull())
		{
			//设置变量
			m_nViewWidth=nViewWidth;
			m_nViewHeight=nViewHeight;
			m_nItemHeight=m_CardListImage.GetHeight();
			m_nItemWidth=m_CardListImage.GetWidth()/ITEM_COUNT;
		}
	}
	return true;
}


//释放资源
bool CCardListImage::DestroyResource()
{
	//设置变量
	m_nItemWidth=0;
	m_nItemHeight=0;

	//释放资源
	//m_CardListImage.DestroyImage();

	return true;
}

//获取位置
int CCardListImage::GetImageIndex(BYTE cbCardData)
{
	//背景判断
	if (cbCardData==0) 
		return 0;

	//计算位置
	BYTE cbValue=cbCardData&MASK_VALUE;
	BYTE cbColor=(cbCardData&MASK_COLOR)>>4;
	return (cbColor>=0x03)?(cbValue+27):(cbColor*9+cbValue);
}

//绘画扑克
bool CCardListImage::DrawCardItem(CDC * pDestDC, BYTE cbCardData, int xDest, int yDest)
{
	//效验状态
	ASSERT(m_CardListImage.IsNull()==false);
	ASSERT((m_nItemWidth!=0)&&(m_nItemHeight!=0));

	//绘画子项
	if(cbCardData<=0x37)
	{
		int nImageXPos=GetImageIndex(cbCardData)*m_nItemWidth;
       m_CardListImage.DrawImage(pDestDC,xDest,yDest,m_nItemWidth,m_nItemHeight,nImageXPos,0);
		 //m_CardListImage.AlphaDrawImage(pDestDC,xDest,yDest,m_nItemWidth,m_nItemHeight,nImageXPos,0,RGB(255,0,255));
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////

//构造函数
CCardResource::CCardResource()
{
}

//析构函数
CCardResource::~CCardResource()
{
}

//加载资源
bool CCardResource::LoadResource()
{
	//变量定义
	TCHAR szResourcePath[MAX_PATH];
	GetCurrentDirectory(MAX_PATH,szResourcePath);
	CString strPath(szResourcePath);
	strPath.Append("\\SparrowCS\\Picture\\Card\\");

	//用户扑克
	//3DView
	//m_ImageMyselfCard.LoadResource("myselfcard.png",63.5,72);
	//m_ImageMyselfCardNum.LoadResource("cardnum.png",CARD_WIDTH,CARD_HEIGHT);

	//m_ImageUserTop.LoadImage(TEXT(strPath+"CARD_USER_TOP.png"));
	//m_ImageUserLeft.LoadImage(TEXT(strPath+"CARD_USER_LEFT.png"));
	//m_ImageUserRight.LoadImage(TEXT(strPath+"CARD_USER_RIGHT.png"));
	//m_ImageUserBottom.LoadResource("CARD_USER_BOTTOM.png",CARD_WIDTH,CARD_HEIGHT);
	//m_ImageKingBottom.LoadResource("KING_USER_BOTTOM.png",CARD_WIDTH,CARD_HEIGHT);

	//桌子扑克
	//m_ImageTableTop.LoadResource("CARD_TABLE_TOP.png",30,45);
	//m_ImageTableLeft.LoadResource("CARD_TABLE_LEFT.png",38,34);
	//m_ImageTableRight.LoadResource("CARD_TABLE_RIGHT.png",38,34);
	//m_ImageTableBottom.LoadResource("CARD_TABLE_BOTTOM.png",30,45);

	//牌堆扑克
	//m_ImageBackH.LoadImage(TEXT(strPath+"CARD_BACK_H.png"));
	//m_ImageBackV.LoadImage(TEXT(strPath+"CARD_BACK_V.png"));
	//m_ImageHeapSingleV.LoadImage(TEXT(strPath+"CARD_HEAP_SINGLE_V.png"));
	//m_ImageHeapSingleH.LoadImage(TEXT(strPath+"CARD_HEAP_SINGLE_H.png"));
	//m_ImageHeapDoubleV.LoadImage(TEXT(strPath+"CARD_HEAP_DOUBLE_V.png"));
	//m_ImageHeapDoubleH.LoadImage(TEXT(strPath+"CARD_HEAP_DOUBLE_H.png"));
	//m_ImageHeapTripleV.LoadImage(TEXT(strPath+"CARD_HEAP_TRIPLE_V.png"));
	//m_ImageHeapTripleH.LoadImage(TEXT(strPath+"CARD_HEAP_TRIPLE_H.png"));

	

	return true;
}

//消耗资源
bool CCardResource::DestroyResource()
{
	//用户扑克
	//m_ImageUserTop.DestroyImage();
	//m_ImageUserLeft.DestroyImage();
	//m_ImageUserRight.DestroyImage();
	//m_ImageUserBottom.DestroyResource();
    //m_ImageKingBottom.DestroyResource();

	//桌子扑克
	//m_ImageTableTop.DestroyResource();
	//m_ImageTableLeft.DestroyResource();
	//m_ImageTableRight.DestroyResource();
	//m_ImageTableBottom.DestroyResource();

	//牌堆扑克
	//m_ImageBackH.DestroyImage();
	//m_ImageBackV.DestroyImage();
	//m_ImageHeapSingleV.DestroyImage();
	//m_ImageHeapSingleH.DestroyImage();
	//m_ImageHeapDoubleV.DestroyImage();
	//m_ImageHeapDoubleH.DestroyImage();
	//m_ImageHeapTripleV.DestroyImage();
	//m_ImageHeapTripleH.DestroyImage();

	return true;
}
*/
//////////////////////////////////////////////////////////////////////////

//构造函数
CHeapCard::CHeapCard()
{
	//控制变量
	m_ControlPoint.SetPoint(0,0);
	m_CardDirection=Direction_East;

	//扑克变量
	m_wFullCount=0;
	m_wMinusHeadCount=0;
	m_wMinusLastCount=0;

	//变量定义
	TCHAR szResourcePath[MAX_PATH];
	GetCurrentDirectory(MAX_PATH,szResourcePath);
	CString strPath(szResourcePath);
	strPath.Append("\\SparrowCS\\Picture\\Card\\");
	m_ImageHeapDoubleVL.LoadImage(TEXT(strPath+"CARD_HEAP_DOUBLE_VL.png"));
	m_ImageHeapDoubleVR.LoadImage(TEXT(strPath+"CARD_HEAP_DOUBLE_VR.png"));
	m_ImageHeapDoubleHT.LoadImage(TEXT(strPath+"CARD_HEAP_DOUBLE_HT.png"));
	m_ImageHeapDoubleHB.LoadImage(TEXT(strPath+"CARD_HEAP_DOUBLE_HB.png"));

	m_ImageHeapSingleVL.LoadImage(TEXT(strPath+"CARD_HEAP_SINGLE_VL.png"));
	m_ImageHeapSingleVR.LoadImage(TEXT(strPath+"CARD_HEAP_SINGLE_VR.png"));
	m_ImageHeapSingleHT.LoadImage(TEXT(strPath+"CARD_HEAP_SINGLE_HT.png"));
	m_ImageHeapSingleHB.LoadImage(TEXT(strPath+"CARD_HEAP_SINGLE_HB.png"));

	return;
}

//析构函数
CHeapCard::~CHeapCard()
{
}

//绘画扑克
void CHeapCard::DrawCardControl(CDC * pDC)
{
	switch (m_CardDirection)
	{
	case Direction_East:	//东向
		{
			//绘画扑克
			if ((m_wFullCount-m_wMinusHeadCount-m_wMinusLastCount)>0)
			{
				//变量定义
				int nXPos=0,nYPos=0;
				WORD wHeapIndex=m_wMinusHeadCount/2;
				WORD wDoubleHeap=(m_wMinusHeadCount+1)/2;
				WORD wDoubleLast=(m_wFullCount-m_wMinusLastCount)/2;
				WORD wFinallyIndex=(m_wFullCount-m_wMinusLastCount)/2;

				//头部扑克
				if (m_wMinusHeadCount%2!=0)
				{
					int nWidth=m_ImageHeapSingleVR.GetWidth()/13;
					int nHeight=m_ImageHeapSingleVR.GetHeight();
					if (0==wHeapIndex)
					{
						nXPos=m_ControlPoint.x+1;
						nYPos=m_ControlPoint.y+12;
					}
					else if (1==wHeapIndex)
					{
						nXPos=m_ControlPoint.x+6;
						nYPos=m_ControlPoint.y+22;
					}
					else if (2==wHeapIndex)
					{
						nXPos=m_ControlPoint.x+11;
						nYPos=m_ControlPoint.y+32;
					}
					else if (3==wHeapIndex)
					{
						nXPos=m_ControlPoint.x+17;
						nYPos=m_ControlPoint.y+42;
					}
					else if (4==wHeapIndex)
					{
						nXPos=m_ControlPoint.x+23;
						nYPos=m_ControlPoint.y+54;
					}
					else if (5==wHeapIndex)
					{
						nXPos=m_ControlPoint.x+28;
						nYPos=m_ControlPoint.y+66;
					}
					else if (6==wHeapIndex)
					{
						nXPos=m_ControlPoint.x+35;
						nYPos=m_ControlPoint.y+79;
					}
					else if (7==wHeapIndex)
					{
						nXPos=m_ControlPoint.x+43;
						nYPos=m_ControlPoint.y+93;
					}
					else if (8==wHeapIndex)
					{
						nXPos=m_ControlPoint.x+52;
						nYPos=m_ControlPoint.y+108;
					}
					else if (9==wHeapIndex)
					{
						nXPos=m_ControlPoint.x+59;
						nYPos=m_ControlPoint.y+125;
					}
					else if (10==wHeapIndex)
					{
						nXPos=m_ControlPoint.x+70;
						nYPos=m_ControlPoint.y+143;
					}
					else if (11==wHeapIndex)
					{
						nXPos=m_ControlPoint.x+80;
						nYPos=m_ControlPoint.y+163;
					}
					else if (12==wHeapIndex)
					{
						nXPos=m_ControlPoint.x+92;
						nYPos=m_ControlPoint.y+184;
					}
					m_ImageHeapSingleVR.DrawImage(pDC,nXPos,nYPos,nWidth,nHeight,nWidth*(12-wHeapIndex),0);
				}

				//中间扑克
				for (WORD i=wDoubleHeap;i<wFinallyIndex;i++)
				{
					int nWidth=m_ImageHeapDoubleVR.GetWidth()/13;
					int nHeight=m_ImageHeapDoubleVR.GetHeight();
					if (0==i)
					{
						nXPos=m_ControlPoint.x+1;
						nYPos=m_ControlPoint.y+12;
					}
					else if (1==i)
					{
						nXPos=m_ControlPoint.x+6;
						nYPos=m_ControlPoint.y+22;
					}
					else if (2==i)
					{
						nXPos=m_ControlPoint.x+11;
						nYPos=m_ControlPoint.y+32;
					}
					else if (3==i)
					{
						nXPos=m_ControlPoint.x+17;
						nYPos=m_ControlPoint.y+42;
					}
					else if (4==i)
					{
						nXPos=m_ControlPoint.x+23;
						nYPos=m_ControlPoint.y+54;
					}
					else if (5==i)
					{
						nXPos=m_ControlPoint.x+28;
						nYPos=m_ControlPoint.y+66;
					}
					else if (6==i)
					{
						nXPos=m_ControlPoint.x+35;
						nYPos=m_ControlPoint.y+79;
					}
					else if (7==i)
					{
						nXPos=m_ControlPoint.x+43;
						nYPos=m_ControlPoint.y+93;
					}
					else if (8==i)
					{
						nXPos=m_ControlPoint.x+52;
						nYPos=m_ControlPoint.y+108;
					}
					else if (9==i)
					{
						nXPos=m_ControlPoint.x+59;
						nYPos=m_ControlPoint.y+125;
					}
					else if (10==i)
					{
						nXPos=m_ControlPoint.x+70;
						nYPos=m_ControlPoint.y+143;
					}
					else if (11==i)
					{
						nXPos=m_ControlPoint.x+80;
						nYPos=m_ControlPoint.y+163;
					}
					else if (12==i)
					{
						nXPos=m_ControlPoint.x+92;
						nYPos=m_ControlPoint.y+184;
					}
					m_ImageHeapDoubleVR.DrawImage(pDC,nXPos,nYPos,nWidth,nHeight,nWidth*(12-i),0);
				}

				//尾部扑克
				if (m_wMinusLastCount%2!=0)
				{
					nXPos=m_ControlPoint.x;
					nYPos=m_ControlPoint.y+wFinallyIndex*23+9;
					if (0==wFinallyIndex)
					{
						nXPos=m_ControlPoint.x+1;
						nYPos=m_ControlPoint.y+12;
					}
					else if (1==wFinallyIndex)
					{
						nXPos=m_ControlPoint.x+6;
						nYPos=m_ControlPoint.y+22;
					}
					else if (2==wFinallyIndex)
					{
						nXPos=m_ControlPoint.x+11;
						nYPos=m_ControlPoint.y+32;
					}
					else if (3==wFinallyIndex)
					{
						nXPos=m_ControlPoint.x+17;
						nYPos=m_ControlPoint.y+42;
					}
					else if (4==wFinallyIndex)
					{
						nXPos=m_ControlPoint.x+23;
						nYPos=m_ControlPoint.y+54;
					}
					else if (5==wFinallyIndex)
					{
						nXPos=m_ControlPoint.x+28;
						nYPos=m_ControlPoint.y+66;
					}
					else if (6==wFinallyIndex)
					{
						nXPos=m_ControlPoint.x+35;
						nYPos=m_ControlPoint.y+79;
					}
					else if (7==wFinallyIndex)
					{
						nXPos=m_ControlPoint.x+43;
						nYPos=m_ControlPoint.y+93;
					}
					else if (8==wFinallyIndex)
					{
						nXPos=m_ControlPoint.x+52;
						nYPos=m_ControlPoint.y+108;
					}
					else if (9==wFinallyIndex)
					{
						nXPos=m_ControlPoint.x+59;
						nYPos=m_ControlPoint.y+125;
					}
					else if (10==wFinallyIndex)
					{
						nXPos=m_ControlPoint.x+70;
						nYPos=m_ControlPoint.y+143;
					}
					else if (11==wFinallyIndex)
					{
						nXPos=m_ControlPoint.x+80;
						nYPos=m_ControlPoint.y+163;
					}
					else if (12==wFinallyIndex)
					{
						nXPos=m_ControlPoint.x+92;
						nYPos=m_ControlPoint.y+184;
					}
					int nWidth=m_ImageHeapSingleVR.GetWidth()/13;
					int nHeight=m_ImageHeapSingleVR.GetHeight();
					m_ImageHeapSingleVR.DrawImage(pDC,nXPos,nYPos,nWidth,nHeight,nWidth*(12-wFinallyIndex),0);
				}
			}
			
			break;
		}
	case Direction_South:	//南向
		{
			//绘画扑克
			if ((m_wFullCount-m_wMinusHeadCount-m_wMinusLastCount)>0)
			{
				//变量定义
				int nXPos=0,nYPos=0;
				WORD wHeapIndex=m_wMinusHeadCount/2;
				WORD wDoubleHeap=(m_wMinusHeadCount+1)/2;
				WORD wDoubleLast=(m_wFullCount-m_wMinusLastCount)/2;
				WORD wFinallyIndex=(m_wFullCount-m_wMinusLastCount)/2;

				//头部扑克
				if (m_wMinusHeadCount%2!=0)
				{
					int nWidth=m_ImageHeapSingleHB.GetWidth()/14;
					int nHeight=m_ImageHeapSingleHB.GetHeight();
					if (wHeapIndex < 7)
					{
						nXPos=m_ControlPoint.x+35*(14-wHeapIndex)+4;
						nYPos=m_ControlPoint.y;
						m_ImageHeapSingleHB.DrawImage(pDC,nXPos,nYPos,nWidth,nHeight,nWidth*(13-wHeapIndex),0);
					}
				}

				//中间扑克
				for (WORD i=wHeapIndex;i<wFinallyIndex && i <7; ++i)
				{
					int nWidth=m_ImageHeapDoubleHB.GetWidth()/14;
					int nHeight=m_ImageHeapDoubleHB.GetHeight();
					nXPos=m_ControlPoint.x+35*(13-i)+4;
					nYPos=m_ControlPoint.y;

					m_ImageHeapDoubleHB.DrawImage(pDC,nXPos,nYPos,nWidth,nHeight,nWidth*(13-i),0);
				}

				//尾部扑克
				if (m_wMinusLastCount%2!=0)
				{
					int nWidth=m_ImageHeapSingleHB.GetWidth()/14;
					int nHeight=m_ImageHeapSingleHB.GetHeight();
					if (wFinallyIndex > 6)
					{
						nXPos=m_ControlPoint.x+35*(12-wFinallyIndex);
						nYPos=m_ControlPoint.y;
						m_ImageHeapSingleHB.DrawImage(pDC,nXPos,nYPos,nWidth,nHeight,nWidth*(12-wFinallyIndex),0);
					}
					else if (wFinallyIndex < 7)
					{
						nXPos=m_ControlPoint.x+35*wFinallyIndex+4;
						nYPos=m_ControlPoint.y;
						m_ImageHeapSingleHB.DrawImage(pDC,nXPos,nYPos,nWidth,nHeight,nWidth*wFinallyIndex,0);
					}
					
				}

				
				//中间扑克
				for (WORD i=wFinallyIndex;i>6 && i>wHeapIndex; --i)
				{
					int nWidth=m_ImageHeapDoubleHB.GetWidth()/14;
					int nHeight=m_ImageHeapDoubleHB.GetHeight();
					nXPos=m_ControlPoint.x+35*(13-i)+3;
					nYPos=m_ControlPoint.y;
					m_ImageHeapDoubleHB.DrawImage(pDC,nXPos,nYPos,nWidth,nHeight,nWidth*(13-i),0);
				}

				//头部扑克
				if (m_wMinusHeadCount%2!=0)
				{
					int nWidth=m_ImageHeapSingleHB.GetWidth()/14;
					int nHeight=m_ImageHeapSingleHB.GetHeight();
					if (wHeapIndex > 6 )
					{
						nXPos=m_ControlPoint.x+35*(13-wHeapIndex);
						nYPos=m_ControlPoint.y;
						m_ImageHeapSingleHB.DrawImage(pDC,nXPos,nYPos,nWidth,nHeight,nWidth*(13-wHeapIndex),0);
					}
				}

			}

			break;
		}
	case Direction_West:	//西向
		{
			//绘画扑克
			if ((m_wFullCount-m_wMinusHeadCount-m_wMinusLastCount)>0)
			{
				//变量定义
				int nXPos=0,nYPos=0;
				WORD wHeapIndex=m_wMinusLastCount/2;
				WORD wDoubleHeap=(m_wMinusLastCount+1)/2;
				WORD wDoubleLast=(m_wFullCount-m_wMinusHeadCount)/2;
				WORD wFinallyIndex=(m_wFullCount-m_wMinusHeadCount)/2;

				//尾部扑克
				if (m_wMinusLastCount%2!=0)
				{
					if (0==wHeapIndex)
					{
						nXPos=m_ControlPoint.x-3;
						nYPos=m_ControlPoint.y-1;
					}
					else if (1==wHeapIndex)
					{
						nXPos=m_ControlPoint.x-8;
						nYPos=m_ControlPoint.y+9;
					}
					else if (2==wHeapIndex)
					{
						nXPos=m_ControlPoint.x-13;
						nYPos=m_ControlPoint.y+19;
					}
					else if (3==wHeapIndex)
					{
						nXPos=m_ControlPoint.x-17;
						nYPos=m_ControlPoint.y+29;
					}
					else if (4==wHeapIndex)
					{
						nXPos=m_ControlPoint.x-23;
						nYPos=m_ControlPoint.y+41;
					}
					else if (5==wHeapIndex)
					{
						nXPos=m_ControlPoint.x-30;
						nYPos=m_ControlPoint.y+54;
					}
					else if (6==wHeapIndex)
					{
						nXPos=m_ControlPoint.x-37;
						nYPos=m_ControlPoint.y+67;
					}
					else if (7==wHeapIndex)
					{
						nXPos=m_ControlPoint.x-45;
						nYPos=m_ControlPoint.y+81;
					}
					else if (8==wHeapIndex)
					{
						nXPos=m_ControlPoint.x-52;
						nYPos=m_ControlPoint.y+96;
					}
					else if (9==wHeapIndex)
					{
						nXPos=m_ControlPoint.x-61;
						nYPos=m_ControlPoint.y+113;
					}
					else if (10==wHeapIndex)
					{
						nXPos=m_ControlPoint.x-70;
						nYPos=m_ControlPoint.y+131;
					}
					else if (11==wHeapIndex)
					{
						nXPos=m_ControlPoint.x-80;
						nYPos=m_ControlPoint.y+151;
					}
					else if (12==wHeapIndex)
					{
						nXPos=m_ControlPoint.x-92;
						nYPos=m_ControlPoint.y+172;
					}
					int nWidth=m_ImageHeapSingleVL.GetWidth()/13;
					int nHeight=m_ImageHeapSingleVL.GetHeight();
					m_ImageHeapSingleVL.DrawImage(pDC,nXPos,nYPos,nWidth,nHeight,nWidth*(12-wHeapIndex),0);
				}

				//中间扑克
				for (WORD i=0;i<wFinallyIndex-wDoubleHeap;i++)
				{
					int nWidth=m_ImageHeapDoubleVL.GetWidth()/13;
					int nHeight=m_ImageHeapDoubleVL.GetHeight();
					if (0==i)
					{
						nXPos=m_ControlPoint.x-3;
						nYPos=m_ControlPoint.y-1;
					}
					else if (1==i)
					{
						nXPos=m_ControlPoint.x-8;
						nYPos=m_ControlPoint.y+9;
					}
					else if (2==i)
					{
						nXPos=m_ControlPoint.x-13;
						nYPos=m_ControlPoint.y+19;
					}
					else if (3==i)
					{
						nXPos=m_ControlPoint.x-17;
						nYPos=m_ControlPoint.y+29;
					}
					else if (4==i)
					{
						nXPos=m_ControlPoint.x-23;
						nYPos=m_ControlPoint.y+41;
					}
					else if (5==i)
					{
						nXPos=m_ControlPoint.x-30;
						nYPos=m_ControlPoint.y+54;
					}
					else if (6==i)
					{
						nXPos=m_ControlPoint.x-37;
						nYPos=m_ControlPoint.y+67;
					}
					else if (7==i)
					{
						nXPos=m_ControlPoint.x-45;
						nYPos=m_ControlPoint.y+81;
					}
					else if (8==i)
					{
						nXPos=m_ControlPoint.x-52;
						nYPos=m_ControlPoint.y+96;
					}
					else if (9==i)
					{
						nXPos=m_ControlPoint.x-61;
						nYPos=m_ControlPoint.y+113;
					}
					else if (10==i)
					{
						nXPos=m_ControlPoint.x-70;
						nYPos=m_ControlPoint.y+131;
					}
					else if (11==i)
					{
						nXPos=m_ControlPoint.x-80;
						nYPos=m_ControlPoint.y+151;
					}
					else if (12==i)
					{
						nXPos=m_ControlPoint.x-92;
						nYPos=m_ControlPoint.y+172;
					}
					m_ImageHeapDoubleVL.DrawImage(pDC,nXPos,nYPos,nWidth,nHeight,nWidth*(12-i),0);
				}

				//头部扑克
				if (m_wMinusHeadCount%2!=0)
				{
					if (0==wFinallyIndex)
					{
						nXPos=m_ControlPoint.x-3;
						nYPos=m_ControlPoint.y-1;
					}
					else if (1==wFinallyIndex)
					{
						nXPos=m_ControlPoint.x-8;
						nYPos=m_ControlPoint.y+9;
					}
					else if (2==wFinallyIndex)
					{
						nXPos=m_ControlPoint.x-13;
						nYPos=m_ControlPoint.y+19;
					}
					else if (3==wFinallyIndex)
					{
						nXPos=m_ControlPoint.x-17;
						nYPos=m_ControlPoint.y+29;
					}
					else if (4==wFinallyIndex)
					{
						nXPos=m_ControlPoint.x-23;
						nYPos=m_ControlPoint.y+41;
					}
					else if (5==wFinallyIndex)
					{
						nXPos=m_ControlPoint.x-30;
						nYPos=m_ControlPoint.y+54;
					}
					else if (6==wFinallyIndex)
					{
						nXPos=m_ControlPoint.x-37;
						nYPos=m_ControlPoint.y+67;
					}
					else if (7==wFinallyIndex)
					{
						nXPos=m_ControlPoint.x-45;
						nYPos=m_ControlPoint.y+81;
					}
					else if (8==wFinallyIndex)
					{
						nXPos=m_ControlPoint.x-52;
						nYPos=m_ControlPoint.y+96;
					}
					else if (9==wFinallyIndex)
					{
						nXPos=m_ControlPoint.x-61;
						nYPos=m_ControlPoint.y+113;
					}
					else if (10==wFinallyIndex)
					{
						nXPos=m_ControlPoint.x-70;
						nYPos=m_ControlPoint.y+131;
					}
					else if (11==wFinallyIndex)
					{
						nXPos=m_ControlPoint.x-80;
						nYPos=m_ControlPoint.y+151;
					}
					else if (12==wFinallyIndex)
					{
						nXPos=m_ControlPoint.x-92;
						nYPos=m_ControlPoint.y+172;
					}
					int nWidth=m_ImageHeapSingleVL.GetWidth()/13;
					int nHeight=m_ImageHeapSingleVL.GetHeight();
					m_ImageHeapSingleVL.DrawImage(pDC,nXPos,nYPos,nWidth,nHeight,nWidth*(12-wFinallyIndex),0);
				}
			}
			
			break;
		}
	case Direction_North:	//北向
		{
			//绘画扑克
			if ((m_wFullCount-m_wMinusHeadCount-m_wMinusLastCount)>0)
			{
				//变量定义
				int nXPos=0,nYPos=0;
				WORD wHeapIndex=m_wMinusHeadCount/2;
				WORD wDoubleHeap=(m_wMinusHeadCount+1)/2;
				WORD wDoubleLast=(m_wFullCount-m_wMinusLastCount)/2;
				WORD wFinallyIndex=(m_wFullCount-m_wMinusLastCount)/2;

				//头部扑克
				if (m_wMinusHeadCount%2!=0)
				{
					int nWidth=m_ImageHeapSingleHT.GetWidth()/14;
					int nHeight=m_ImageHeapSingleHT.GetHeight();
					if (wHeapIndex < 7)
					{
						nXPos=m_ControlPoint.x+24*wHeapIndex;
						nYPos=m_ControlPoint.y;
					    m_ImageHeapSingleHT.DrawImage(pDC,nXPos,nYPos,nWidth,nHeight,nWidth*wHeapIndex,0);
					}

				}

				//中间扑克
				for (WORD i=wDoubleHeap;i<7 && i<wFinallyIndex; i++)
				{
				   int nWidth=m_ImageHeapDoubleHT.GetWidth()/14;
				   int nHeight=m_ImageHeapDoubleHT.GetHeight();
				   nXPos=m_ControlPoint.x+24*i;
				   nYPos=m_ControlPoint.y;
				   m_ImageHeapDoubleHT.DrawImage(pDC,nXPos,nYPos,nWidth,nHeight,nWidth*i,0);
			    }

				//尾部扑克
				if (m_wMinusLastCount%2!=0)
				{
					int nWidth=m_ImageHeapSingleHT.GetWidth()/14;
					int nHeight=m_ImageHeapSingleHT.GetHeight();
					if (wFinallyIndex < 7)
					{
						nXPos=m_ControlPoint.x+24*wFinallyIndex;
						nYPos=m_ControlPoint.y;
					}
					else if (wFinallyIndex > 6)
					{
						nXPos=m_ControlPoint.x+23*wFinallyIndex+7;
						nYPos=m_ControlPoint.y;
					}
					m_ImageHeapSingleHT.DrawImage(pDC,nXPos,nYPos,nWidth,nHeight,nWidth*wFinallyIndex,0);
				}

			   for (WORD i=wFinallyIndex - 1;i > 6 && i > wHeapIndex ; --i)
			   {
				   int nWidth=m_ImageHeapDoubleHT.GetWidth()/14;
				   int nHeight=m_ImageHeapDoubleHT.GetHeight();
				   nXPos=m_ControlPoint.x+23*i+7;
				   nYPos=m_ControlPoint.y;
			
				   m_ImageHeapDoubleHT.DrawImage(pDC,nXPos,nYPos,nWidth,nHeight,nWidth*i,0);
			   }

			   //头部扑克
			  if (m_wMinusHeadCount%2!=0)
			   {
				   int nWidth=m_ImageHeapSingleHT.GetWidth()/14;
				   int nHeight=m_ImageHeapSingleHT.GetHeight();
				   if (wHeapIndex > 6)
				    {
					   nXPos=m_ControlPoint.x+23*wHeapIndex+7;
					   nYPos=m_ControlPoint.y;
					   m_ImageHeapSingleHT.DrawImage(pDC,nXPos,nYPos,nWidth,nHeight,nWidth*wHeapIndex,0);
				    }
			   }

			}

			break;
		}
	}

	return;
}

//设置扑克
bool CHeapCard::SetCardData(WORD wMinusHeadCount, WORD wMinusLastCount, WORD wFullCount)
{
	//设置变量
	m_wFullCount=wFullCount;
	m_wMinusHeadCount=wMinusHeadCount;
	m_wMinusLastCount=wMinusLastCount;

	return true;
}

//////////////////////////////////////////////////////////////////////////

//构造函数
CWeaveCard::CWeaveCard()
{
	//状态变量
	m_bDisplayItem=false;
	m_ControlPoint.SetPoint(0,0);
	m_CardDirection=Direction_South;
	m_cbDirectionCardPos = 1;

	//扑克数据
	m_wCardCount=0;
	ZeroMemory(&m_cbCardData,sizeof(m_cbCardData));

	//变量定义
	TCHAR szResourcePath[MAX_PATH];
	GetCurrentDirectory(MAX_PATH,szResourcePath);
	CString strPath(szResourcePath);
	strPath.Append("\\SparrowCS\\Picture\\Card\\");
	//牌面
	m_ImageUserWeaveCardVL.LoadImage(TEXT(strPath+"USER_TABLE_CARD_VL.png"));
	m_ImageUserWeaveCardVR.LoadImage(TEXT(strPath+"USER_TABLE_CARD_VR.png"));
	m_ImageUserWeaveCardHT.LoadImage(TEXT(strPath+"USER_TABLE_CARD_HT.png"));
	m_ImageUserWeaveCardHB.LoadImage(TEXT(strPath+"USER_TABLE_CARD_HB.png"));
	//牌数据
	m_ImageWeaveCardNumVL.LoadImage(TEXT(strPath+"USER_TABCARDNUM_VL.png"));
	m_ImageWeaveCardNumVR.LoadImage(TEXT(strPath+"USER_TABCARDNUM_VR.png"));
	m_ImageWeaveCardNumHT.LoadImage(TEXT(strPath+"USER_TABCARDNUM_HT.png"));
	m_ImageWeaveCardNumHB.LoadImage(TEXT(strPath+"USER_TABCARDNUM_HB.png"));
	//暗杠
	m_ImageWeaveCardBackVL.LoadImage(TEXT(strPath+"USER_WEAVE_CARD_BACKVL.png"));
	m_ImageWeaveCardBackVR.LoadImage(TEXT(strPath+"USER_WEAVE_CARD_BACKVR.png"));
	m_ImageWeaveCardBackHT.LoadImage(TEXT(strPath+"USER_WEAVE_CARD_BACKHT.png"));
	m_ImageWeaveCardBackHB.LoadImage(TEXT(strPath+"USER_WEAVE_CARD_BACKHB.png"));

	return;
}

//析构函数
CWeaveCard::~CWeaveCard()
{
}

//获取位置
int CWeaveCard::GetImageIndex(BYTE cbCardData)
{
	//背景判断
	if (cbCardData==0) 
		return 0;

	//计算位置
	BYTE cbValue=cbCardData&MASK_VALUE;
	BYTE cbColor=(cbCardData&MASK_COLOR)>>4;
	return (cbColor>=0x03)?(cbValue+27):(cbColor*9+cbValue);
}


//绘画扑克
void CWeaveCard::DrawCardControl(CDC * pDC)
{
	//显示判断
	if (m_wCardCount==0) 
		return;

	//变量定义
	int nXScreenPos=0,nYScreenPos=0;
	int nItemWidth=0,nItemHeight=0,nItemWidthEx=0,nItemHeightEx=0;

	//绘画扑克
	switch (m_CardDirection)
	{
	case Direction_East:	//东向
		{
			//变量定义
			nItemWidth=m_ImageUserWeaveCardVR.GetWidth()/14;
			nItemHeight=m_ImageUserWeaveCardVR.GetHeight();

			int nHeight =  m_ImageWeaveCardNumVR.GetHeight()/27;
			int nImageYPos=0;

			if (0 == m_iIndex)
			{
				//第一张
				nXScreenPos = m_ControlPoint.x;
				nYScreenPos = m_ControlPoint.y +m_iIndex*23;
				nImageYPos=(GetImageIndex(m_cbCardData[0])-1)*nHeight;
				if (m_bDisplayItem)
				{
					m_ImageUserWeaveCardVR.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth*2,0);
					m_ImageWeaveCardNumVR.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth*2,nImageYPos);
				}
				else
				{
					m_ImageWeaveCardBackVR.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth*2,0);
				}
				

				//第二张
				nXScreenPos = m_ControlPoint.x+9;
				nYScreenPos = m_ControlPoint.y+(m_iIndex+1)*23-6;
				nImageYPos=(GetImageIndex(m_cbCardData[1])-1)*nHeight;
				if (m_bDisplayItem)
				{
					m_ImageUserWeaveCardVR.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth,0);
					m_ImageWeaveCardNumVR.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth,nImageYPos);
				}
				else
				{
					m_ImageWeaveCardBackVR.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth,0);
				}
				

				//第三张
				nXScreenPos = m_ControlPoint.x+21;
				nYScreenPos = m_ControlPoint.y+(m_iIndex+2)*23-11;
				nImageYPos=(GetImageIndex(m_cbCardData[2])-1)*nHeight;
				if (m_bDisplayItem)
				{
					m_ImageUserWeaveCardVR.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,0,0);
					m_ImageWeaveCardNumVR.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,0,nImageYPos);
				}
				else
				{
					m_ImageWeaveCardBackVR.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,0,0);
				}
				

				if (m_wCardCount==4)
				{
					//第四张
					nXScreenPos = m_ControlPoint.x+10;
					nYScreenPos = m_ControlPoint.y+(m_iIndex+1)*23-16;
					nImageYPos=(GetImageIndex(m_cbCardData[3])-1)*nHeight;
					if (m_bDisplayItem)
					{
						m_ImageUserWeaveCardVR.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth,0);
						m_ImageWeaveCardNumVR.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth,nImageYPos);
					}
					else
					{
						m_ImageWeaveCardBackVR.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth,0);
					}
						
				}				
			}

			if (1 == m_iIndex)
			{
				//第一张
				nXScreenPos = m_ControlPoint.x;
				nYScreenPos = m_ControlPoint.y +m_iIndex*23;
				nImageYPos=(GetImageIndex(m_cbCardData[0])-1)*nHeight;
				if (m_bDisplayItem)
				{
					m_ImageUserWeaveCardVR.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth*5,0);
					m_ImageWeaveCardNumVR.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth*5,nImageYPos);
				}
				else
				{
					m_ImageWeaveCardBackVR.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth*5,0);
				}
				

				//第二张
				nXScreenPos = m_ControlPoint.x+8;
				nYScreenPos = m_ControlPoint.y +(m_iIndex+1)*23-9;
				nImageYPos=(GetImageIndex(m_cbCardData[1])-1)*nHeight;
				if (m_bDisplayItem)
				{
					m_ImageUserWeaveCardVR.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth*4,0);
					m_ImageWeaveCardNumVR.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth*4,nImageYPos);
				}
				else
				{
					m_ImageWeaveCardBackVR.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth*4,0);
				}
				

				//第三张
				nXScreenPos = m_ControlPoint.x+17;
				nYScreenPos = m_ControlPoint.y +(m_iIndex+2)*23-17;
				nImageYPos=(GetImageIndex(m_cbCardData[2])-1)*nHeight;
				if (m_bDisplayItem)
				{
					m_ImageUserWeaveCardVR.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth*3,0);
					m_ImageWeaveCardNumVR.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth*3,nImageYPos);
				}
				else
				{
					m_ImageWeaveCardBackVR.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth*3,0);
				}
				

				if (m_wCardCount==4)
				{
					//第四张
					nXScreenPos = m_ControlPoint.x+9;
					nYScreenPos = m_ControlPoint.y +(m_iIndex+1)*23-21;
					nImageYPos=(GetImageIndex(m_cbCardData[3])-1)*nHeight;
					if (m_bDisplayItem)
					{
						m_ImageUserWeaveCardVR.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth*4,0);
						m_ImageWeaveCardNumVR.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth*4,nImageYPos);
					}
					else
					{
						m_ImageWeaveCardBackVR.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth*4,0);
					}			
				}				
			}

			if (2 == m_iIndex)
			{
				//第三张
				nXScreenPos = m_ControlPoint.x;
				nYScreenPos = m_ControlPoint.y +m_iIndex*23;
				nImageYPos=(GetImageIndex(m_cbCardData[0])-1)*nHeight;
				if (m_bDisplayItem)
				{
					m_ImageUserWeaveCardVR.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth*9,0);
					m_ImageWeaveCardNumVR.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth*9,nImageYPos);
				}
				else
				{
					m_ImageWeaveCardBackVR.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth*9,0);
				}
				

				//第二张
				nXScreenPos = m_ControlPoint.x+7;
				nYScreenPos = m_ControlPoint.y +(m_iIndex+1)*23-13;
				nImageYPos=(GetImageIndex(m_cbCardData[1])-1)*nHeight;
				if (m_bDisplayItem)
				{
					m_ImageUserWeaveCardVR.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth*8,0);
					m_ImageWeaveCardNumVR.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth*8,nImageYPos);
				}
				else
				{
					m_ImageWeaveCardBackVR.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth*8,0);
				}
				

				//第一张
				nXScreenPos = m_ControlPoint.x+14;
				nYScreenPos = m_ControlPoint.y +(m_iIndex+2)*23-26;
				nImageYPos=(GetImageIndex(m_cbCardData[2])-1)*nHeight;
				if (m_bDisplayItem)
				{
					m_ImageUserWeaveCardVR.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth*7,0);
					m_ImageWeaveCardNumVR.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth*7,nImageYPos);
				}
				else
				{
					m_ImageWeaveCardBackVR.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth*7,0);
				}
				

				if (m_wCardCount==4)
				{
					//第四张
					nXScreenPos = m_ControlPoint.x+9;
					nYScreenPos = m_ControlPoint.y +(m_iIndex+1)*23-23;
					nImageYPos=(GetImageIndex(m_cbCardData[3])-1)*nHeight;
					if (m_bDisplayItem)
					{
						m_ImageUserWeaveCardVR.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth*8,0);
						m_ImageWeaveCardNumVR.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth*8,nImageYPos);
					}
					else
					{
						m_ImageWeaveCardBackVR.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth*8,0);
					}
						
				}				
			}

			if (3 == m_iIndex)
			{
				//第三张
				nXScreenPos = m_ControlPoint.x;
				nYScreenPos = m_ControlPoint.y +m_iIndex*23;
				nImageYPos=(GetImageIndex(m_cbCardData[0])-1)*nHeight;
				if (m_bDisplayItem)
				{
					m_ImageUserWeaveCardVR.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth*12,0);
					m_ImageWeaveCardNumVR.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth*12,nImageYPos);
				}
				else
				{
					m_ImageWeaveCardBackVR.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth*12,0);
				}
				

				//第二张
				nXScreenPos = m_ControlPoint.x+6;
				nYScreenPos = m_ControlPoint.y +(m_iIndex+1)*23-15;
				nImageYPos=(GetImageIndex(m_cbCardData[1])-1)*nHeight;
				if (m_bDisplayItem)
				{
					m_ImageUserWeaveCardVR.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth*11,0);
					m_ImageWeaveCardNumVR.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth*11,nImageYPos);
				}
				else
				{
					m_ImageWeaveCardBackVR.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth*11,0);
				}
				

				//第一张
				nXScreenPos = m_ControlPoint.x+11;
				nYScreenPos = m_ControlPoint.y +(m_iIndex+2)*23-30;
				nImageYPos=(GetImageIndex(m_cbCardData[2])-1)*nHeight;
				if (m_bDisplayItem)
				{
					m_ImageUserWeaveCardVR.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth*10,0);
					m_ImageWeaveCardNumVR.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth*10,nImageYPos);
				}
				else
				{
					m_ImageWeaveCardBackVR.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth*10,0);
				}
				

				if (m_wCardCount==4)
				{
					//第四张
					nXScreenPos = m_ControlPoint.x+6;
					nYScreenPos = m_ControlPoint.y +(m_iIndex+1)*23-28;
					nImageYPos=(GetImageIndex(m_cbCardData[3])-1)*nHeight;
					if (m_bDisplayItem)
					{
						m_ImageUserWeaveCardVR.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth*11,0);
						m_ImageWeaveCardNumVR.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth*11,nImageYPos);	
					}
					else
					{
						m_ImageWeaveCardBackVR.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth*11,0);
					}
					
				}				
			}

			break;
		}
	case Direction_South:	//南向
		{
			//变量定义
			nItemWidth=m_ImageUserWeaveCardHB.GetWidth()/14;
			nItemHeight=m_ImageUserWeaveCardHB.GetHeight();

			//int nWidth = nItemWidth;
			int nHeight =  m_ImageWeaveCardNumHB.GetHeight()/27;
			int nImageYPos=0;

			if (0 == m_iIndex)
			{
				//第一张
				nXScreenPos = m_ControlPoint.x +m_iIndex*38;
				nYScreenPos = m_ControlPoint.y;
				nImageYPos=(GetImageIndex(m_cbCardData[0])-1)*nHeight;
				if (m_bDisplayItem)
				{
					m_ImageUserWeaveCardHB.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,0,0);
					m_ImageWeaveCardNumHB.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,0,nImageYPos);
				}
				else
				{
					m_ImageWeaveCardBackHB.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,0,0);
				}
				

				//第二张
				nXScreenPos = m_ControlPoint.x +(m_iIndex+1)*38;
				nYScreenPos = m_ControlPoint.y;
				nImageYPos=(GetImageIndex(m_cbCardData[1])-1)*nHeight;
				if (m_bDisplayItem)
				{
					m_ImageUserWeaveCardHB.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth,0);
					m_ImageWeaveCardNumHB.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth,nImageYPos);
				}
				else
				{
					m_ImageWeaveCardBackHB.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth,0);
				}
				

				//第三张
				nXScreenPos = m_ControlPoint.x +(m_iIndex+2)*38;
				nYScreenPos = m_ControlPoint.y;
				nImageYPos=(GetImageIndex(m_cbCardData[2])-1)*nHeight;
				if (m_bDisplayItem)
				{
					m_ImageUserWeaveCardHB.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth*2,0);
					m_ImageWeaveCardNumHB.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth*2,nImageYPos);
				}
				else
				{
					m_ImageWeaveCardBackHB.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth*2,0);
				}
				

				if (m_wCardCount==4)
				{
					//第四张
					nXScreenPos = m_ControlPoint.x +(m_iIndex+1)*38-3;
					nYScreenPos = m_ControlPoint.y-20+3;
					nImageYPos=(GetImageIndex(m_cbCardData[3])-1)*nHeight;
					if (m_bDisplayItem)
					{
						m_ImageUserWeaveCardHB.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth,0);
						m_ImageWeaveCardNumHB.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth,nImageYPos);
					}
					else
					{
						m_ImageWeaveCardBackHB.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth,0);
					}
					
				}				
			}

			if (1 == m_iIndex)
			{
				//第一张
				nXScreenPos = m_ControlPoint.x +m_iIndex*38;
				nYScreenPos = m_ControlPoint.y;
				int nImageYPos=(GetImageIndex(m_cbCardData[0])-1)*nHeight;
				if (m_bDisplayItem)
				{
					m_ImageUserWeaveCardHB.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth*3,0);
					m_ImageWeaveCardNumHB.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth*3,nImageYPos);
				}
				else
				{
					m_ImageWeaveCardBackHB.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth*3,0);
				}
				

				//第二张
				nXScreenPos = m_ControlPoint.x +(m_iIndex+1)*38;
				nYScreenPos = m_ControlPoint.y;
				nImageYPos=(GetImageIndex(m_cbCardData[1])-1)*nHeight;
				if (m_bDisplayItem)
				{
					m_ImageUserWeaveCardHB.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth*4,0);
					m_ImageWeaveCardNumHB.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth*4,nImageYPos);
				}
				else
				{
					m_ImageWeaveCardBackHB.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth*4,0);
				}
				

				//第三张
				nXScreenPos = m_ControlPoint.x +(m_iIndex+2)*38;
				nYScreenPos = m_ControlPoint.y;
				nImageYPos=(GetImageIndex(m_cbCardData[2])-1)*nHeight;
				if (m_bDisplayItem)
				{
					m_ImageUserWeaveCardHB.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth*5,0);
					m_ImageWeaveCardNumHB.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth*5,nImageYPos);
				}
				else
				{
					m_ImageWeaveCardBackHB.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth*5,0);
				}
				

				if (m_wCardCount==4)
				{
					//第四张
					nXScreenPos = m_ControlPoint.x +(m_iIndex+1)*38-2;
					nYScreenPos = m_ControlPoint.y-20+3;
					nImageYPos=(GetImageIndex(m_cbCardData[3])-1)*nHeight;
					if (m_bDisplayItem)
					{
						m_ImageUserWeaveCardHB.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth*4,0);
						m_ImageWeaveCardNumHB.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth*4,nImageYPos);
					}
					else
					{
						m_ImageWeaveCardBackHB.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth*4,0);
					}
					
				}				
			}

			if (2 == m_iIndex)
			{
				//第三张
				nXScreenPos = m_ControlPoint.x +(m_iIndex+2)*38;
				nYScreenPos = m_ControlPoint.y;
				nImageYPos=(GetImageIndex(m_cbCardData[0])-1)*nHeight;
				if (m_bDisplayItem)
				{
					m_ImageUserWeaveCardHB.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth*9,0);
					m_ImageWeaveCardNumHB.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth*9,nImageYPos);
				}
				else
				{
					m_ImageWeaveCardBackHB.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth*9,0);
				}
				

				//第二张
				nXScreenPos = m_ControlPoint.x +(m_iIndex+1)*38;
				nYScreenPos = m_ControlPoint.y;
				nImageYPos=(GetImageIndex(m_cbCardData[1])-1)*nHeight;
				if (m_bDisplayItem)
				{
					m_ImageUserWeaveCardHB.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth*8,0);
					m_ImageWeaveCardNumHB.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth*8,nImageYPos);
				}
				else
				{
					m_ImageWeaveCardBackHB.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth*8,0);
				}
				

				//第一张
				nXScreenPos = m_ControlPoint.x +m_iIndex*38;
				nYScreenPos = m_ControlPoint.y;
				nImageYPos=(GetImageIndex(m_cbCardData[2])-1)*nHeight;
				if (m_bDisplayItem)
				{
					m_ImageUserWeaveCardHB.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth*7,0);
					m_ImageWeaveCardNumHB.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth*7,nImageYPos);
				}
				else
				{
					m_ImageWeaveCardBackHB.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth*7,0);
				}
				

				if (m_wCardCount==4)
				{
					//第四张
					nXScreenPos = m_ControlPoint.x +(m_iIndex+1)*38+2;
					nYScreenPos = m_ControlPoint.y-20+3;
					nImageYPos=(GetImageIndex(m_cbCardData[3])-1)*nHeight;
					if (m_bDisplayItem)
					{
						m_ImageUserWeaveCardHB.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth*8,0);
						m_ImageWeaveCardNumHB.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth*8,nImageYPos);
					}
					else
					{
						m_ImageWeaveCardBackHB.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth*8,0);
					}
					
				}				
			}

			if (3 == m_iIndex)
			{
				//第三张
				nXScreenPos = m_ControlPoint.x +(m_iIndex+2)*38;
				nYScreenPos = m_ControlPoint.y;
				nImageYPos=(GetImageIndex(m_cbCardData[0])-1)*nHeight;
				if (m_bDisplayItem)
				{
					m_ImageUserWeaveCardHB.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth*12,0);
					m_ImageWeaveCardNumHB.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth*12,nImageYPos);
				}
				else
				{
					m_ImageWeaveCardBackHB.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth*12,0);
				}
				

				//第二张
				nXScreenPos = m_ControlPoint.x +(m_iIndex+1)*38;
				nYScreenPos = m_ControlPoint.y;
				nImageYPos=(GetImageIndex(m_cbCardData[1])-1)*nHeight;
				if (m_bDisplayItem)
				{
					m_ImageUserWeaveCardHB.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth*11,0);
					m_ImageWeaveCardNumHB.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth*11,nImageYPos);
				}
				else
				{
					m_ImageWeaveCardBackHB.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth*11,0);
				}
				

				//第一张
				nXScreenPos = m_ControlPoint.x +m_iIndex*38;
				nYScreenPos = m_ControlPoint.y;
				nImageYPos=(GetImageIndex(m_cbCardData[2])-1)*nHeight;
				if (m_bDisplayItem)
				{
					m_ImageUserWeaveCardHB.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth*10,0);
					m_ImageWeaveCardNumHB.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth*10,nImageYPos);
				}
				else
				{
					m_ImageWeaveCardBackHB.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth*10,0);
				}
				

				if (m_wCardCount==4)
				{
					//第四张
					nXScreenPos = m_ControlPoint.x +(m_iIndex+1)*38+3;
					nYScreenPos = m_ControlPoint.y-20+3;
					nImageYPos=(GetImageIndex(m_cbCardData[3])-1)*nHeight;
					if (m_bDisplayItem)
					{
						m_ImageUserWeaveCardHB.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth*11,0);
						m_ImageWeaveCardNumHB.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth*11,nImageYPos);
					}
					else
					{
						m_ImageWeaveCardBackHB.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth*11,0);
					}
					
				}				
			}

			break;
		}
	case Direction_West:	//西向
		{
			//变量定义
			nItemWidth=m_ImageUserWeaveCardVL.GetWidth()/14;
			nItemHeight=m_ImageUserWeaveCardVL.GetHeight();

			int nHeight =  m_ImageWeaveCardNumVL.GetHeight()/27;
			int nImageYPos=0;

			if (0 == m_iIndex)
			{
				//第一张
				nXScreenPos = m_ControlPoint.x;
				nYScreenPos = m_ControlPoint.y -(m_iIndex+2)*23;
				nImageYPos=(GetImageIndex(m_cbCardData[0])-1)*nHeight;
				if (m_bDisplayItem)
				{
					m_ImageUserWeaveCardVL.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth*13,0);
					m_ImageWeaveCardNumVL.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth*13,nImageYPos);
				}
				else
				{
					m_ImageWeaveCardBackVL.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth*13,0);
				}
				

				//第二张
				nXScreenPos = m_ControlPoint.x-6;
				nYScreenPos = m_ControlPoint.y -(m_iIndex+1)*23-14;
				nImageYPos=(GetImageIndex(m_cbCardData[1])-1)*nHeight;
				if (m_bDisplayItem)
				{
					m_ImageUserWeaveCardVL.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth*12,0);
					m_ImageWeaveCardNumVL.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth*12,nImageYPos);
				}
				else
				{
					m_ImageWeaveCardBackVL.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth*12,0);
				}
				

				//第三张
				nXScreenPos = m_ControlPoint.x-12;
				nYScreenPos = m_ControlPoint.y -m_iIndex*23-28;
				nImageYPos=(GetImageIndex(m_cbCardData[2])-1)*nHeight;
				if (m_bDisplayItem)
				{
					m_ImageUserWeaveCardVL.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth*11,0);
					m_ImageWeaveCardNumVL.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth*11,nImageYPos);
				}
				else
				{
					m_ImageWeaveCardBackVL.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth*11,0);
				}
				

				if (m_wCardCount==4)
				{
					//第四张
					nXScreenPos = m_ControlPoint.x-6;
					nYScreenPos = m_ControlPoint.y -(m_iIndex+1)*23-23;
					nImageYPos=(GetImageIndex(m_cbCardData[3])-1)*nHeight;
					if (m_bDisplayItem)
					{
						m_ImageUserWeaveCardVL.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth*12,0);
						m_ImageWeaveCardNumVL.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth*12,nImageYPos);
					}
					else
					{
						m_ImageWeaveCardBackVL.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth*12,0);
					}
						
				}				
			}

			if (1 == m_iIndex)
			{
				//第一张
				nXScreenPos = m_ControlPoint.x;
				nYScreenPos = m_ControlPoint.y -(m_iIndex+2)*23;
				nImageYPos=(GetImageIndex(m_cbCardData[0])-1)*nHeight;
				if (m_bDisplayItem)
				{
					m_ImageUserWeaveCardVL.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth*10,0);
					m_ImageWeaveCardNumVL.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth*10,nImageYPos);
				}
				else
				{
					m_ImageWeaveCardBackVL.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth*10,0);
				}
				

				//第二张
				nXScreenPos = m_ControlPoint.x-6;
				nYScreenPos = m_ControlPoint.y -(m_iIndex+1)*23-13;
				nImageYPos=(GetImageIndex(m_cbCardData[1])-1)*nHeight;
				if (m_bDisplayItem)
				{
					m_ImageUserWeaveCardVL.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth*9,0);
					m_ImageWeaveCardNumVL.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth*9,nImageYPos);
				}
				else
				{
					m_ImageWeaveCardBackVL.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth*9,0);
				}
				

				//第三张
				nXScreenPos = m_ControlPoint.x-13;
				nYScreenPos = m_ControlPoint.y -m_iIndex*23-26;
				nImageYPos=(GetImageIndex(m_cbCardData[2])-1)*nHeight;
				if (m_bDisplayItem)
				{
					m_ImageUserWeaveCardVL.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth*8,0);
					m_ImageWeaveCardNumVL.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth*8,nImageYPos);
				}
				else
				{
					m_ImageWeaveCardBackVL.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth*8,0);
				}
				
				if (m_wCardCount==4)
				{
					//第四张
					nXScreenPos = m_ControlPoint.x-8;
					nYScreenPos = m_ControlPoint.y -(m_iIndex+1)*23-24;
					nImageYPos=(GetImageIndex(m_cbCardData[3])-1)*nHeight;
					if (m_bDisplayItem)
					{
						m_ImageUserWeaveCardVL.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth*9,0);
						m_ImageWeaveCardNumVL.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth*9,nImageYPos);
					}
					else
					{
						m_ImageWeaveCardBackVL.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth*9,0);
					}
						
				}				
			}

			if (2 == m_iIndex)
			{
				//第三张
				nXScreenPos = m_ControlPoint.x;
				nYScreenPos = m_ControlPoint.y -(m_iIndex+2)*23;
				nImageYPos=(GetImageIndex(m_cbCardData[0])-1)*nHeight;
				if (m_bDisplayItem)
				{
					m_ImageUserWeaveCardVL.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth*6,0);
					m_ImageWeaveCardNumVL.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth*6,nImageYPos);
				}
				else
				{
					m_ImageWeaveCardBackVL.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth*6,0);
				}
				

				//第二张
				nXScreenPos = m_ControlPoint.x-8;
				nYScreenPos = m_ControlPoint.y -(m_iIndex+1)*23-10;
				nImageYPos=(GetImageIndex(m_cbCardData[1])-1)*nHeight;
				if (m_bDisplayItem)
				{
					m_ImageUserWeaveCardVL.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth*5,0);
					m_ImageWeaveCardNumVL.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth*5,nImageYPos);
				}
				else
				{
					m_ImageWeaveCardBackVL.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth*5,0);
				}
				

				//第一张
				nXScreenPos = m_ControlPoint.x-17;
				nYScreenPos = m_ControlPoint.y -m_iIndex*23-19;
				nImageYPos=(GetImageIndex(m_cbCardData[2])-1)*nHeight;
				if (m_bDisplayItem)
				{
					m_ImageUserWeaveCardVL.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth*4,0);
					m_ImageWeaveCardNumVL.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth*4,nImageYPos);
				}
				else
				{
					m_ImageWeaveCardBackVL.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth*4,0);
				}
				
	
				if (m_wCardCount==4)
				{
					//第四张
					nXScreenPos = m_ControlPoint.x-11;
					nYScreenPos = m_ControlPoint.y -(m_iIndex+1)*23-21;
					nImageYPos=(GetImageIndex(m_cbCardData[3])-1)*nHeight;
					if (m_bDisplayItem)
					{
						m_ImageUserWeaveCardVL.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth*5,0);
						m_ImageWeaveCardNumVL.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth*5,nImageYPos);
					}
					else
					{
						m_ImageWeaveCardBackVL.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth*5,0);
					}
							
				}				
			}

			if (3 == m_iIndex)
			{
				//第三张
				nXScreenPos = m_ControlPoint.x;
				nYScreenPos = m_ControlPoint.y -(m_iIndex+2)*23;
				nImageYPos=(GetImageIndex(m_cbCardData[0])-1)*nHeight;
				if (m_bDisplayItem)
				{
					m_ImageUserWeaveCardVL.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth*3,0);
					m_ImageWeaveCardNumVL.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth*3,nImageYPos);
				}
				else
				{
					m_ImageWeaveCardBackVL.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth*3,0);
				}
				
	
				//第二张
				nXScreenPos = m_ControlPoint.x-9;
				nYScreenPos = m_ControlPoint.y -(m_iIndex+1)*23-8;
				nImageYPos=(GetImageIndex(m_cbCardData[1])-1)*nHeight;
				if (m_bDisplayItem)
				{
					m_ImageUserWeaveCardVL.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth*2,0);
					m_ImageWeaveCardNumVL.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth*2,nImageYPos);
				}
				else
				{
					m_ImageWeaveCardBackVL.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth*2,0);
				}
				

				//第一张
				nXScreenPos = m_ControlPoint.x-19;
				nYScreenPos = m_ControlPoint.y -m_iIndex*23-16;
				nImageYPos=(GetImageIndex(m_cbCardData[2])-1)*nHeight;
				if (m_bDisplayItem)
				{
					m_ImageUserWeaveCardVL.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth*1,0);
					m_ImageWeaveCardNumVL.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth*1,nImageYPos);
				}
				else
				{
					m_ImageWeaveCardBackVL.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth*1,0);
				}
				

				if (m_wCardCount==4)
				{
					//第四张
					nXScreenPos = m_ControlPoint.x-11;
					nYScreenPos = m_ControlPoint.y -(m_iIndex+1)*23-22;
					nImageYPos=(GetImageIndex(m_cbCardData[3])-1)*nHeight;
					if (m_bDisplayItem)
					{
						m_ImageUserWeaveCardVL.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth*2,0);
						m_ImageWeaveCardNumVL.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth*2,nImageYPos);
					}
					else
					{
						m_ImageWeaveCardBackVL.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth*2,0);
					}
										
				}				
			}

			break;
		}
	case Direction_North:	//北向
		{
			//变量定义
			nItemWidth=m_ImageUserWeaveCardHT.GetWidth()/14;
			nItemHeight=m_ImageUserWeaveCardHT.GetHeight();

			int nHeight =  m_ImageWeaveCardNumHT.GetHeight()/27;
			int nImageYPos=0;

			if (0 == m_iIndex)
			{
				//第一张
				nXScreenPos = m_ControlPoint.x -m_iIndex*23;
				nYScreenPos = m_ControlPoint.y;
				nImageYPos=(GetImageIndex(m_cbCardData[0])-1)*nHeight;
				if (m_bDisplayItem)
				{
					m_ImageUserWeaveCardHT.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth*13,0);
					m_ImageWeaveCardNumHT.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth*13,nImageYPos);
				}
				else
				{
					m_ImageWeaveCardBackHT.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth*13,0);
				}
				
	
				//第二张
				nXScreenPos = m_ControlPoint.x -(m_iIndex+1)*23;
				nYScreenPos = m_ControlPoint.y;
				nImageYPos=(GetImageIndex(m_cbCardData[1])-1)*nHeight;
				if (m_bDisplayItem)
				{
					m_ImageUserWeaveCardHT.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth*12,0);
					m_ImageWeaveCardNumHT.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth*12,nImageYPos);
				}
				else
				{
					m_ImageWeaveCardBackHT.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth*12,0);
				}
				
	
				//第三张
				nXScreenPos = m_ControlPoint.x -(m_iIndex+2)*23;
				nYScreenPos = m_ControlPoint.y;
				nImageYPos=(GetImageIndex(m_cbCardData[2])-1)*nHeight;
				if (m_bDisplayItem)
				{
					m_ImageUserWeaveCardHT.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth*11,0);
					m_ImageWeaveCardNumHT.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth*11,nImageYPos);
				}
				else
				{
					m_ImageWeaveCardBackHT.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth*11,0);
				}
				
	
				if (m_wCardCount==4)
				{
					//第四张
					nXScreenPos = m_ControlPoint.x -(m_iIndex+1)*23+1;
					nYScreenPos = m_ControlPoint.y-20+8;
					nImageYPos=(GetImageIndex(m_cbCardData[3])-1)*nHeight;
					if (m_bDisplayItem)
					{
						m_ImageUserWeaveCardHT.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth*12,0);
						m_ImageWeaveCardNumHT.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth*12,nImageYPos);
					}
					else
					{
						m_ImageWeaveCardBackHT.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth*12,0);
					}
							
				}				
			}

			if (1 == m_iIndex)
			{
				//第一张
				nXScreenPos = m_ControlPoint.x -m_iIndex*23;
				nYScreenPos = m_ControlPoint.y;
				nImageYPos=(GetImageIndex(m_cbCardData[0])-1)*nHeight;
				if (m_bDisplayItem)
				{
					m_ImageUserWeaveCardHT.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth*10,0);
					m_ImageWeaveCardNumHT.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth*10,nImageYPos);
				}
				else
				{
					m_ImageWeaveCardBackHT.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth*10,0);
				}
				

				//第二张
				nXScreenPos = m_ControlPoint.x -(m_iIndex+1)*23;
				nYScreenPos = m_ControlPoint.y;
				nImageYPos=(GetImageIndex(m_cbCardData[1])-1)*nHeight;
				if (m_bDisplayItem)
				{
					m_ImageUserWeaveCardHT.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth*9,0);
					m_ImageWeaveCardNumHT.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth*9,nImageYPos);
				}
				else
				{
					m_ImageWeaveCardBackHT.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth*9,0);
				}
				
	
				//第三张
				nXScreenPos = m_ControlPoint.x -(m_iIndex+2)*23;
				nYScreenPos = m_ControlPoint.y;
				nImageYPos=(GetImageIndex(m_cbCardData[2])-1)*nHeight;
				if (m_bDisplayItem)
				{
					m_ImageUserWeaveCardHT.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth*8,0);
					m_ImageWeaveCardNumHT.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth*8,nImageYPos);
				}
				else
				{
					m_ImageWeaveCardBackHT.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth*8,0);
				}
				
	
				if (m_wCardCount==4)
				{
					//第四张
					nXScreenPos = m_ControlPoint.x -(m_iIndex+1)*23+1;
					nYScreenPos = m_ControlPoint.y-20+9;
					nImageYPos=(GetImageIndex(m_cbCardData[3])-1)*nHeight;
					if (m_bDisplayItem)
					{
						m_ImageUserWeaveCardHT.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth*9,0);
						m_ImageWeaveCardNumHT.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth*9,nImageYPos);
					}
					else
					{
						m_ImageWeaveCardBackHT.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth*9,0);
					}
							
				}				
			}

			if (2 == m_iIndex)
			{
				//第三张
				nXScreenPos = m_ControlPoint.x -(m_iIndex+2)*23;
				nYScreenPos = m_ControlPoint.y;
				nImageYPos=(GetImageIndex(m_cbCardData[0])-1)*nHeight;
				if (m_bDisplayItem)
				{
					m_ImageUserWeaveCardHT.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth*4,0);
					m_ImageWeaveCardNumHT.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth*4,nImageYPos);
				}
				else
				{
					m_ImageWeaveCardBackHT.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth*4,0);
				}
				

				//第二张
				nXScreenPos = m_ControlPoint.x -(m_iIndex+1)*23;
				nYScreenPos = m_ControlPoint.y;
				nImageYPos=(GetImageIndex(m_cbCardData[1])-1)*nHeight;
				if (m_bDisplayItem)
				{
					m_ImageUserWeaveCardHT.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth*5,0);
					m_ImageWeaveCardNumHT.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth*5,nImageYPos);
				}
				else
				{
					m_ImageWeaveCardBackHT.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth*5,0);
				}
				
	
				//第一张
				nXScreenPos = m_ControlPoint.x -m_iIndex*23;
				nYScreenPos = m_ControlPoint.y;
				nImageYPos=(GetImageIndex(m_cbCardData[2])-1)*nHeight;
				if (m_bDisplayItem)
				{
					m_ImageUserWeaveCardHT.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth*6,0);
					m_ImageWeaveCardNumHT.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth*6,nImageYPos);
				}
				else
				{
					m_ImageWeaveCardBackHT.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth*6,0);
				}
				
	
				if (m_wCardCount==4)
				{
					//第四张
					nXScreenPos = m_ControlPoint.x -(m_iIndex+1)*23-1;
					nYScreenPos = m_ControlPoint.y-20+9;
					nImageYPos=(GetImageIndex(m_cbCardData[3])-1)*nHeight;
					if (m_bDisplayItem)
					{
						m_ImageUserWeaveCardHT.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth*5,0);
						m_ImageWeaveCardNumHT.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth*5,nImageYPos);
					}
					else
					{
						m_ImageWeaveCardBackHT.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth*5,0);
					}
							
				}				
			}

			if (3 == m_iIndex)
			{
				//第三张
				nXScreenPos = m_ControlPoint.x -(m_iIndex+2)*23;
				nYScreenPos = m_ControlPoint.y;
				nImageYPos=(GetImageIndex(m_cbCardData[0])-1)*nHeight;
				if (m_bDisplayItem)
				{
					m_ImageUserWeaveCardHT.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth*1,0);
					m_ImageWeaveCardNumHT.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth*1,nImageYPos);
				}
				else
				{
					m_ImageWeaveCardBackHT.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth*1,0);
				}
				
	
				//第二张
				nXScreenPos = m_ControlPoint.x -(m_iIndex+1)*23;
				nYScreenPos = m_ControlPoint.y;
				nImageYPos=(GetImageIndex(m_cbCardData[1])-1)*nHeight;
				if (m_bDisplayItem)
				{
					m_ImageUserWeaveCardHT.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth*2,0);
					m_ImageWeaveCardNumHT.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth*2,nImageYPos);
				}
				else
				{
					m_ImageWeaveCardBackHT.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth*2,0);
				}
				

				//第一张
				nXScreenPos = m_ControlPoint.x -m_iIndex*23;
				nYScreenPos = m_ControlPoint.y;
				nImageYPos=(GetImageIndex(m_cbCardData[2])-1)*nHeight;
				if (m_bDisplayItem)
				{
					m_ImageUserWeaveCardHT.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth*3,0);
					m_ImageWeaveCardNumHT.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth*3,nImageYPos);
				}
				else
				{
					m_ImageWeaveCardBackHT.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth*3,0);
				}
				
	
				if (m_wCardCount==4)
				{
					//第四张
					nXScreenPos = m_ControlPoint.x -(m_iIndex+1)*23-1;
					nYScreenPos = m_ControlPoint.y-20+9;
					nImageYPos=(GetImageIndex(m_cbCardData[3])-1)*nHeight;
					if (m_bDisplayItem)
					{
						m_ImageUserWeaveCardHT.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth*2,0);
						m_ImageWeaveCardNumHT.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth*2,nImageYPos);
					}
					else
					{
						m_ImageWeaveCardBackHT.DrawImage(pDC,nXScreenPos,nYScreenPos,nItemWidth,nItemHeight,nItemWidth*2,0);
					}
						
				}				
			}

			break;
		}
	}

	return;
}

//设置扑克
bool CWeaveCard::SetCardData(const BYTE cbCardData[], WORD wCardCount)
{
	//效验大小
	ASSERT(wCardCount<=CountArray(m_cbCardData));
	if (wCardCount>CountArray(m_cbCardData)) return false;

	//设置扑克
	m_wCardCount=wCardCount;
	CopyMemory(m_cbCardData,cbCardData,sizeof(BYTE)*wCardCount);

	return true;
}

//获取扑克
BYTE CWeaveCard::GetCardData(WORD wIndex)
{
	ASSERT(wIndex<CountArray(m_cbCardData));
	return ((m_bDisplayItem==true)||(wIndex==1))?m_cbCardData[wIndex]:0;
}
//////////////////////////////////////////////////////////////////////////////////////////////

//构造函数
CUserCard::CUserCard()
{
	//扑克数据
	m_wCardCount=0;
	m_bCurrentCard=false;

	//控制变量
	m_ControlPoint.SetPoint(0,0);
	m_CardDirection=Direction_East;

	//变量定义
	TCHAR szResourcePath[MAX_PATH];
	GetCurrentDirectory(MAX_PATH,szResourcePath);
	CString strPath(szResourcePath);
	strPath.Append("\\SparrowCS\\Picture\\Card\\");
	m_ImageUserHandVL.LoadImage(TEXT(strPath+"CARD_USER_HANDL.png"));
	m_ImageUserHandVR.LoadImage(TEXT(strPath+"CARD_USER_HANDR.png"));
	m_ImageUserHandHT.LoadImage(TEXT(strPath+"CARD_USER_HANDT.png"));

	return;
}

//析构函数
CUserCard::~CUserCard()
{
}

//绘画扑克
void CUserCard::DrawCardControl(CDC * pDC)
{
	switch (m_CardDirection)
	{

			case Direction_East:	//东向
				{
					int nWidth=m_ImageUserHandVR.GetWidth()/14;
					int nHeight=m_ImageUserHandVR.GetHeight();

					//当前扑克
					if (m_bCurrentCard==true)
					{
						int nXPos=m_ControlPoint.x-2;
						int nYPos=m_ControlPoint.y+8;
						m_ImageUserHandVR.DrawImage(pDC,nXPos,nYPos,nWidth,nHeight,nWidth*13,0);
					}

					//正常扑克
					if (m_wCardCount>0)
					{
						int nXPos=0,nYPos=0;
						for (WORD i=0;i<m_wCardCount;i++)
						{
							if (0==i)
							{
								nXPos=m_ControlPoint.x+5+5;
								nYPos=m_ControlPoint.y+27+10;
							}
							else if (1==i)
							{
								nXPos=m_ControlPoint.x+11 +5;
								nYPos=m_ControlPoint.y+35 +10;
							}
							else if (2==i)
							{
								nXPos=m_ControlPoint.x+16 + 2+5;
								nYPos=m_ControlPoint.y+46+10;
							}
							else if (3==i)
							{
								nXPos=m_ControlPoint.x+23 + 2+5;
								nYPos=m_ControlPoint.y+56+10;
							}
							else if (4==i)
							{
								nXPos=m_ControlPoint.x+30+3+5;
								nYPos=m_ControlPoint.y+68+10;
							}
							else if (5==i)
							{
								nXPos=m_ControlPoint.x+37+3+5;
								nYPos=m_ControlPoint.y+81+10;
							}
							else if (6==i)
							{
								nXPos=m_ControlPoint.x+45+4+5;
								nYPos=m_ControlPoint.y+94+10;
							}
							else if (7==i)
							{
								nXPos=m_ControlPoint.x+53+5+5;
								nYPos=m_ControlPoint.y+107+10;
							}
							else if (8==i)
							{
								nXPos=m_ControlPoint.x+61+6+5;
								nYPos=m_ControlPoint.y+120+10;
							}
							else if (9==i)
							{
								nXPos=m_ControlPoint.x+70+6+5;
								nYPos=m_ControlPoint.y+155-21+10;
							}
							else if (10==i)
							{
								nXPos=m_ControlPoint.x+80+6+5;
								nYPos=m_ControlPoint.y+169-20+10;
							}
							else if (11==i)
							{
								nXPos=m_ControlPoint.x+91+5+5;
								nYPos=m_ControlPoint.y+193-28+10;
							}
							if (12==i)
							{
								nXPos=m_ControlPoint.x+102+5+5;
								nYPos=m_ControlPoint.y+213-32+10;
							}
							m_ImageUserHandVR.DrawImage(pDC,nXPos,nYPos,nWidth,nHeight,nWidth*(12-i),0);
						}
					}

					


					break;
				}

			case Direction_West:	//西向
				{
					int nWidth=m_ImageUserHandVL.GetWidth()/14;
					int nHeight=m_ImageUserHandVL.GetHeight();
					//正常扑克
					if (m_wCardCount>0)
					{
						int nXPos=0,nYPos=0;
						int num=13-m_wCardCount;
						for (WORD i=num;i<13;++i)
						{
							if (0==i)
							{
								nXPos=m_ControlPoint.x+80;
								nYPos=m_ControlPoint.y;
							}
							else if (1==i)
							{
								nXPos=m_ControlPoint.x-7+80+2;
								nYPos=m_ControlPoint.y+12-2;
							}
							else if (2==i)
							{
								nXPos=m_ControlPoint.x-14+80+3;
								nYPos=m_ControlPoint.y+24-4;
							}
							else if (3==i)
							{
								nXPos=m_ControlPoint.x-20+80+4;
								nYPos=m_ControlPoint.y+37-8+1;
							}
							else if (4==i)
							{
								nXPos=m_ControlPoint.x-27+80+4;
								nYPos=m_ControlPoint.y+50-12+1+1;
							}
							else if (5==i)
							{
								nXPos=m_ControlPoint.x-34+80+5;
								nYPos=m_ControlPoint.y+65-16+1+1;
							}
							else if (6==i)
							{
								nXPos=m_ControlPoint.x-43+80+7;
								nYPos=m_ControlPoint.y+80-20+3;
							}
						    if (7==i)
							{
								nXPos=m_ControlPoint.x-51+80+7;
								nYPos=m_ControlPoint.y+97-24+3;
							}
							else if (8==i)
							{
								nXPos=m_ControlPoint.x-60+80+8;
								nYPos=m_ControlPoint.y+115-28+1+1;
							}
							else if (9==i)
							{
								nXPos=m_ControlPoint.x-70+90;
								nYPos=m_ControlPoint.y+133-33+1+1;
							}
							else if (10==i)
							{
								nXPos=m_ControlPoint.x-80+91;
								nYPos=m_ControlPoint.y+152-37+1+1;
							}
							else if (11==i)
							{
								nXPos=m_ControlPoint.x-90+91;
								nYPos=m_ControlPoint.y+172-42+1+1;
							}
							else if (12==i)
							{
								nXPos=m_ControlPoint.x-9;
								nYPos=m_ControlPoint.y+148;
							}
							m_ImageUserHandVL.DrawImage(pDC,nXPos,nYPos,nWidth,nHeight,nWidth*(13-i),0);
						}
					}

					//当前扑克
					if (m_bCurrentCard==true)
					{
						int nXPos=m_ControlPoint.x-25;
						int nYPos=m_ControlPoint.y+180;
						m_ImageUserHandVL.DrawImage(pDC,nXPos,nYPos,nWidth,nHeight,0,0);
					}
					break;
				}

			case Direction_North:	//北向
				{
					int nWidth=m_ImageUserHandHT.GetWidth()/14;
					int nHeight=m_ImageUserHandHT.GetHeight();

					//正常扑克
					int count = m_wCardCount;
					if (count>0)
					{
						int nXPos=0,nYPos=0;
						if (count>6)
						{
							for (int i=count;i > 6;--i)
							{
								nYPos=m_ControlPoint.y;
								nXPos=m_ControlPoint.x+i*22+34;
								m_ImageUserHandHT.DrawImage(pDC,nXPos,nYPos,nWidth,nHeight,nWidth*i,0);
							}
							count -= (count-6);
						}
						
						if (count <7)
						{
							for (int i=0;i < count;++i)
							{
								nYPos=m_ControlPoint.y;
								nXPos=m_ControlPoint.x+i*23+40+10;
								m_ImageUserHandHT.DrawImage(pDC,nXPos,nYPos,nWidth,nHeight,nWidth*i,0);
							}
						}
					}

					//当前扑克
					if (m_bCurrentCard==true)
					{
						int nXPos=m_ControlPoint.x+10;
						int nYPos=m_ControlPoint.y;
						m_ImageUserHandHT.DrawImage(pDC,nXPos,nYPos,nWidth,nHeight,nWidth,0);
					}

					break;
				}

			return;
	}
}

//设置扑克
bool CUserCard::SetCurrentCard(bool bCurrentCard)
{
	//设置变量
	m_bCurrentCard=bCurrentCard;

	return true;
}

//设置扑克
bool CUserCard::SetCardData(WORD wCardCount, bool bCurrentCard)
{
	//设置变量
	m_wCardCount=wCardCount;
	m_bCurrentCard=bCurrentCard;

	return true;
}

//////////////////////////////////////////////////////////////////////////

//构造函数
CDiscardCard::CDiscardCard()
{
	//扑克数据
	m_wCardCount=0;
	ZeroMemory(m_cbCardData,sizeof(m_cbCardData));

	//控制变量
	m_ControlPoint.SetPoint(0,0);
	m_CardDirection=Direction_East;

	//变量定义
	TCHAR szResourcePath[MAX_PATH];
	GetCurrentDirectory(MAX_PATH,szResourcePath);
	CString strPath(szResourcePath);
	strPath.Append("\\SparrowCS\\Picture\\Card\\");
	m_ImageUserDisCardHB.LoadImage(TEXT(strPath+"USER_DIS_CARD_HB.png"));
	m_ImageUserDisCardHT.LoadImage(TEXT(strPath+"USER_DIS_CARD_HT.png"));
	m_ImageUserDisCardVL.LoadImage(TEXT(strPath+"USER_DIS_CARD_VL.png"));
	m_ImageUserDisCardVR.LoadImage(TEXT(strPath+"USER_DIS_CARD_VR.png"));
	//m_ImageUserDisCardNumHB.LoadImage(strPath+TEXT("DISCARDNUM_HB.png"));
	//m_ImageTestDisCardVL.LoadImage(TEXT(strPath+"USER_DISCARD_VL.png"));
	//m_ImageTestDisCardVR.LoadImage(TEXT(strPath+"TEST_DISCARD_VR.png"));
	//m_ImageTestDisCardHT.LoadImage(TEXT(strPath+"TEST_DISCARD_HT.png"));
    m_ImageDisCardNumVL.LoadImage(TEXT(strPath+"USER_DISCARDNUM_VL.png"));
	m_ImageDisCardNumVR.LoadImage(TEXT(strPath+"USER_DISCARDNUM_VR.png"));
	m_ImageDisCardNumHT.LoadImage(TEXT(strPath+"USER_DISCARDNUM_HT.png"));
	m_ImageDisCardNumHB.LoadImage(TEXT(strPath+"USER_DISCARDNUM_HB.png"));


	return;
}

//析构函数
CDiscardCard::~CDiscardCard()
{
}

//获取位置
int CDiscardCard::GetImageIndex(BYTE cbCardData)
{
	//背景判断
	if (cbCardData==0) 
		return 0;

	//计算位置
	BYTE cbValue=cbCardData&MASK_VALUE;
	BYTE cbColor=(cbCardData&MASK_COLOR)>>4;
	return (cbColor>=0x03)?(cbValue+27):(cbColor*9+cbValue);
}



//绘画扑克
void CDiscardCard::DrawCardControl(CDC * pDC)
{
	//绘画控制
	switch (m_CardDirection)
	{
	case Direction_East:	//东向
		{
			int nWidth=m_ImageUserDisCardVR.GetWidth()/18;
			int nHeight=m_ImageUserDisCardVR.GetHeight();
			int nItemWidth = nWidth;
			int nItemHeight =  m_ImageDisCardNumVR.GetHeight()/27;

			int nXPos=0,nYPos=0;
			//绘画扑克
			int count1 = 0,count2=0,count3=0;
			if(m_wCardCount>12)
			{
				count1=6;
				count2=12;
				count3=m_wCardCount;
			}
			if (m_wCardCount > 6 && m_wCardCount<13)
			{
				count1 = 6;
				count2 = m_wCardCount;
			}
			else if (m_wCardCount < 7)
			{
				count1 = m_wCardCount ;
			}

			//第一列
			if (count1<7)
			{
				for (int i=count1-1;i>=0;--i)
				{
					if (0 == i)
					{
						nXPos=m_ControlPoint.x-((17-i)/6)*29+42+5;
						nYPos=m_ControlPoint.y+((17-i)%6)*24-50;
					}
					else if (1 == i)
					{
						nXPos=m_ControlPoint.x-((17-i)/6)*29+33+5;
						nYPos=m_ControlPoint.y+((17-i)%6)*24-50-2+5;
					}
					else if (2 == i)
					{
						nXPos=m_ControlPoint.x-((17-i)/6)*29+24+5;
						nYPos=m_ControlPoint.y+((17-i)%6)*24-50-3+10;
					}
					else if (3 == i)
					{
						nXPos=m_ControlPoint.x-((17-i)/6)*29+16+5;
						nYPos=m_ControlPoint.y+((17-i)%6)*24-50-3+15;
					}
					else if (4 == i)
					{
						nXPos=m_ControlPoint.x-((17-i)/6)*29+8+6;
						nYPos=m_ControlPoint.y+((17-i)%6)*24-50-1+20;
					}
					else if (5 == i)
					{
						nXPos=m_ControlPoint.x-((17-i)/6)*29+7;
						nYPos=m_ControlPoint.y+((17-i)%6)*24-49+26;
					}
					//绘画背景
					int nImageXPos=i*nItemWidth;
					int nImageYPos=(GetImageIndex(m_cbCardData[i])-1)*nItemHeight;
					m_ImageUserDisCardVR.DrawImage(pDC,nXPos,nYPos,nWidth,nHeight,nWidth*i,0);
					m_ImageDisCardNumVR.DrawImage(pDC,nXPos,nYPos,nWidth,nHeight,nImageXPos,nImageYPos);					
				}
			}
			//第二列
			if(6<count2 && count2<13)
			{
				for (int i=count2-1;i>=6;--i)
				{
					if (6 == i)
					{
						nXPos=m_ControlPoint.x-((17-i)/6)*29+42+5-60+4;
						nYPos=m_ControlPoint.y+((17-i)%6)*24-50;
					}
					else if (7 == i)
					{
						nXPos=m_ControlPoint.x-((17-i)/6)*29+33+4-59+6;
						nYPos=m_ControlPoint.y+((17-i)%6)*24-50-2+5;
					}
					else if (8 == i)
					{
						nXPos=m_ControlPoint.x-((17-i)/6)*29+24+3-58+7;
						nYPos=m_ControlPoint.y+((17-i)%6)*24-50-3+10;
					}
					else if (9 == i)
					{
						nXPos=m_ControlPoint.x-((17-i)/6)*29+16+2-57+8;
						nYPos=m_ControlPoint.y+((17-i)%6)*24-50-3+15;
					}
					else if (10 == i)
					{
						nXPos=m_ControlPoint.x-((17-i)/6)*29+8+1-56+10;
						nYPos=m_ControlPoint.y+((17-i)%6)*24-50-1+20;
					}
					else if (11 == i)
					{
						nXPos=m_ControlPoint.x-((17-i)/6)*29-55+11;
						nYPos=m_ControlPoint.y+((17-i)%6)*24-49+26;
					}
					//绘画背景
					int nImageXPos=i*nItemWidth;
					int nImageYPos=(GetImageIndex(m_cbCardData[i])-1)*nItemHeight;
					m_ImageUserDisCardVR.DrawImage(pDC,nXPos,nYPos,nWidth,nHeight,nWidth*i,0);
					m_ImageDisCardNumVR.DrawImage(pDC,nXPos,nYPos,nWidth,nHeight,nImageXPos,nImageYPos);					
				}
			}
			////第三列
			if (count3 > 12)
			{
				for (int i=count3-1;i>=12;--i)
				{
					if (12 == i)
					{
						nXPos=m_ControlPoint.x-((17-i)/6)*29+42+5-61-59+10;
						nYPos=m_ControlPoint.y+((17-i)%6)*24-50;
					}
					else if (13 == i)
					{
						nXPos=m_ControlPoint.x-((17-i)/6)*29+33+4-60-58+12;
						nYPos=m_ControlPoint.y+((17-i)%6)*24-50-2+5;
					}
					else if (14 == i)
					{
						nXPos=m_ControlPoint.x-((17-i)/6)*29+24+3-59-57+13;
						nYPos=m_ControlPoint.y+((17-i)%6)*24-50-3+10;
					}
					else if (15 == i)
					{
						nXPos=m_ControlPoint.x-((17-i)/6)*29+16+2-58-56+13;
						nYPos=m_ControlPoint.y+((17-i)%6)*24-50-3+15;
					}
					else if (16 == i)
					{
						nXPos=m_ControlPoint.x-((17-i)/6)*29+8+1-57-55+15;
						nYPos=m_ControlPoint.y+((17-i)%6)*24-50-1+20;
					}
					else if (17 == i)
					{
						nXPos=m_ControlPoint.x-((17-i)/6)*29-55-54+16;
						nYPos=m_ControlPoint.y+((17-i)%6)*24-49+26;
					}
					//绘画背景
					int nImageXPos=i*nItemWidth;
					int nImageYPos=(GetImageIndex(m_cbCardData[i])-1)*nItemHeight;
					m_ImageUserDisCardVR.DrawImage(pDC,nXPos,nYPos,nWidth,nHeight,nWidth*i,0);
					m_ImageDisCardNumVR.DrawImage(pDC,nXPos,nYPos,nWidth,nHeight,nImageXPos,nImageYPos);					
				}
			}



			break;
		}
	case Direction_South:	//南向
		{//绘画扑克
			int nWidth=m_ImageUserDisCardHB.GetWidth()/20;
			int nHeight=m_ImageUserDisCardHB.GetHeight();

			int nItemWidth = nWidth;
			int nItemHeight =  m_ImageDisCardNumHB.GetHeight()/27;
			//绘画背景
			int count = m_wCardCount;
			if (count > 15) //第二排最后五张
			{
				int num = count - 15;
				count -= num;
				for(int i = num; i>0; --i)
				{
					int nXPos=m_ControlPoint.x+(i%10)*28+90;
					int nYPos=m_ControlPoint.y-24;
					int nImageXPos=(count + i - 1)*nItemWidth;
					int nImageYPos=(GetImageIndex(m_cbCardData[i+14])-1)*nItemHeight;
					m_ImageUserDisCardHB.DrawImage(pDC,nXPos,nYPos,nWidth,nHeight,nImageXPos,0);
					m_ImageDisCardNumHB.DrawImage(pDC,nXPos,nYPos,nWidth,nHeight,nImageXPos,nImageYPos);

					//m_ImageUserDisCardHB.DrawImage(pDC,nXPos,nYPos,nWidth,nHeight,nWidth*(count + i - 1),0);
					//m_ImageDisCardNumHB.DrawImage(pDC,nXPos,nYPos,nWidth,nHeight,nWidth*(count + i - 1),0);
				}
			
			}

			if (count > 10)//第二排前面五张
			{
				int num = count %10;
				count -= num;
				for (int i=0; i<num; ++i)
				{
					int nXPos=m_ControlPoint.x+(i%10)*28-24;
					int nYPos=m_ControlPoint.y-24;
					int nImageXPos=(count + i)*nItemWidth;
					int nImageYPos=(GetImageIndex(m_cbCardData[i+10])-1)*nItemHeight;
					m_ImageUserDisCardHB.DrawImage(pDC,nXPos,nYPos,nWidth,nHeight,nImageXPos,0);
					m_ImageDisCardNumHB.DrawImage(pDC,nXPos,nYPos,nWidth,nHeight,nImageXPos,nImageYPos);

					//m_ImageUserDisCardHB.DrawImage(pDC,nXPos,nYPos,nWidth,nHeight,nWidth*(count + i),0);
					//m_ImageDisCardNumHB.DrawImage(pDC,nXPos,nYPos,nWidth,nHeight,nWidth*(count + i),0);
					//DrawCardItem(pDC,m_cbCardData[count + i],nXPos,nYPos);
				}
			}

			if (count > 5) //第一排最后五张
			{
				int num = count - 5;
				count -= num;
				for(int i = num; i>0; --i)
				{
					int nXPos=m_ControlPoint.x+(i%10)*29+90;
					int nYPos=m_ControlPoint.y+3;
					int nImageXPos=(count + i -1)*nItemWidth;
					int nImageYPos=(GetImageIndex(m_cbCardData[i+4])-1)*nItemHeight;
					m_ImageUserDisCardHB.DrawImage(pDC,nXPos,nYPos,nWidth,nHeight,nImageXPos,0);
					m_ImageDisCardNumHB.DrawImage(pDC,nXPos,nYPos,nWidth,nHeight,nImageXPos,nImageYPos);

					//m_ImageUserDisCardHB.DrawImage(pDC,nXPos,nYPos,nWidth,nHeight,nWidth*(count + i - 1),0);
					//m_ImageDisCardNumHB.DrawImage(pDC,nXPos,nYPos,nWidth,nHeight,nWidth*(count + i - 1),0);
					//DrawCardItem(pDC,m_cbCardData[count + i-1],nXPos,nYPos);
				}
			}

			if (count > 0)//第一排前面五张
			{
				int num = count %10;
				count -= num;
				for (int i=0; i<num; ++i)
				{
					int nXPos=m_ControlPoint.x+(i%10)*30-32;
					int nYPos=m_ControlPoint.y+3;
					int nImageXPos=(count + i)*nItemWidth;
					int nImageYPos=(GetImageIndex(m_cbCardData[i])-1)*nItemHeight;
					m_ImageUserDisCardHB.DrawImage(pDC,nXPos,nYPos,nWidth,nHeight,nImageXPos,0);
					m_ImageDisCardNumHB.DrawImage(pDC,nXPos,nYPos,nWidth,nHeight,nImageXPos,nImageYPos);

					//m_ImageUserDisCardHB.DrawImage(pDC,nXPos,nYPos,nWidth,nHeight,nWidth*(count + i),0);
					//m_ImageDisCardNumHB.DrawImage(pDC,nXPos,nYPos,nWidth,nHeight,nWidth*(count + i),0);
					//DrawCardItem(pDC,m_cbCardData[count + i],nXPos,nYPos);
				}
			}

			break;
		}
	case Direction_West:	//西向
		{
			int nWidth=m_ImageUserDisCardVL.GetWidth()/18;
			int nHeight=m_ImageUserDisCardVL.GetHeight();

			int nItemWidth = nWidth;
			int nItemHeight = m_ImageDisCardNumVL.GetHeight()/27;

			//绘画扑克
			for (WORD i=0;i<m_wCardCount;i++)
			{
				int nXPos=0,nYPos=0;
				if (0 == i)
				{
					nXPos=m_ControlPoint.x-((17-i)/6)*29-(i%6)*11;
					nYPos=m_ControlPoint.y-((17-i)%6)*24+(i%6)*11;
				}
				else if (1 == i)
				{
					nXPos=m_ControlPoint.x-((17-i)/6)*29-(i%6)*11+2-1+2;
					nYPos=m_ControlPoint.y-((17-i)%6)*24+(i%6)*11-8-6-5;
				}
				else if (2 == i)
				{
					nXPos=m_ControlPoint.x-((17-i)/6)*29-(i%6)*11+4-2+5;
					nYPos=m_ControlPoint.y-((17-i)%6)*24+(i%6)*11-15-12-10;
				}
				else if (3 == i)
				{
					nXPos=m_ControlPoint.x-((17-i)/6)*29-(i%6)*11+4-3+8;
					nYPos=m_ControlPoint.y-((17-i)%6)*24+(i%6)*11-21-15-18;
				}
				else if (4 == i)
				{
					nXPos=m_ControlPoint.x-((17-i)/6)*29-(i%6)*11+4-4+11;
					nYPos=m_ControlPoint.y-((17-i)%6)*24+(i%6)*11-26-19-25;
				}
				else if (5 == i)
				{
					nXPos=m_ControlPoint.x-((17-i)/6)*29-(i%6)*11+4-6+14;
					nYPos=m_ControlPoint.y-((17-i)%6)*24+(i%6)*11-30-20-33;
				}


				else if (6 == i)
				{
					nXPos=m_ControlPoint.x-((17-i)/6)*29-3-4;
					nYPos=m_ControlPoint.y-((17-i)%6)*24;
				}
				else if (7 == i)
				{
					nXPos=m_ControlPoint.x-((17-i)/6)*29-13;
					nYPos=m_ControlPoint.y-((17-i)%6)*24-3-6;
				}
				else if (8 == i)
				{
					nXPos=m_ControlPoint.x-((17-i)/6)*29-20;
					nYPos=m_ControlPoint.y-((17-i)%6)*24-5-11;
				}
				else if (9 == i)
				{
					nXPos=m_ControlPoint.x-((17-i)/6)*29-29;
					nYPos=m_ControlPoint.y-((17-i)%6)*24-3-19;
				}
				else if (10 == i)
				{
					nXPos=m_ControlPoint.x-((17-i)/6)*29-36;
					nYPos=m_ControlPoint.y-((17-i)%6)*24-26;
				}
				else if (11 == i)
				{
					nXPos=m_ControlPoint.x-((17-i)/6)*29-46;
					nYPos=m_ControlPoint.y-((17-i)%6)*24+5-33;
				}



				else if (12 == i)
				{
					nXPos=m_ControlPoint.x-((17-i)/6)*29-15;
					nYPos=m_ControlPoint.y-((17-i)%6)*24;
				}
				else if (13 == i)
				{
					nXPos=m_ControlPoint.x-((17-i)/6)*29-20;
					nYPos=m_ControlPoint.y-((17-i)%6)*24-9;
				}
				else if (14 == i)
				{
					nXPos=m_ControlPoint.x-((17-i)/6)*29-26;
					nYPos=m_ControlPoint.y-((17-i)%6)*24-16;
				}
				else if (15 == i)
				{
					nXPos=m_ControlPoint.x-((17-i)/6)*29-33;
					nYPos=m_ControlPoint.y-((17-i)%6)*24-22;
				}
				else if (16 == i)
				{
					nXPos=m_ControlPoint.x-((17-i)/6)*29-40;
					nYPos=m_ControlPoint.y-((17-i)%6)*24-25;
				}
				else if (17 == i)
				{
					nXPos=m_ControlPoint.x-((17-i)/6)*29-48;
					nYPos=m_ControlPoint.y-((17-i)%6)*24-28;
				}
				//绘画背景
				int nImageXPos=i*nItemWidth;
				int nImageYPos=(GetImageIndex(m_cbCardData[i])-1)*nItemHeight;
				m_ImageUserDisCardVL.DrawImage(pDC,nXPos,nYPos,nWidth,nHeight,nWidth*i,0);
				m_ImageDisCardNumVL.DrawImage(pDC,nXPos,nYPos,nWidth,nHeight,nImageXPos,nImageYPos);
			}

			break;
		}
	case Direction_North:	//北向
		{//绘画扑克
			int nWidth=m_ImageUserDisCardHT.GetWidth()/20;
			int nHeight=m_ImageUserDisCardHT.GetHeight();

			int nItemWidth = nWidth;
			int nItemHeight =  m_ImageDisCardNumHT.GetHeight()/27;

			//绘画背景
			int count = 0;
			if (m_wCardCount > 10)
			{
				count = 10;
			}else
			{
			    count = m_wCardCount ;
			}

			if (count < 11 && count >5)//第一排前面五张
			{
				for (int i=count; i > 5; --i)
				{
					int nXPos=m_ControlPoint.x-i*23+205;
					int nYPos=m_ControlPoint.y;
					int nImageXPos=(10-i)*nItemWidth;
					int nImageYPos=(GetImageIndex(m_cbCardData[i-1])-1)*nItemHeight;
					m_ImageUserDisCardHT.DrawImage(pDC,nXPos,nYPos,nWidth,nHeight,nWidth*(10-i),0);
					m_ImageDisCardNumHT.DrawImage(pDC,nXPos,nYPos,nWidth,nHeight,nImageXPos,nImageYPos);
				}
                count-=(count - 5);
			}

			if (count < 6) //第一排最后五张
			{
				for(int i = 0; i < count; ++i)
				{
					int nXPos=m_ControlPoint.x-i*23+180;
					int nYPos=m_ControlPoint.y;
					int nImageXPos=(9-i)*nItemWidth;
					int nImageYPos=(GetImageIndex(m_cbCardData[i])-1)*nItemHeight;
					m_ImageUserDisCardHT.DrawImage(pDC,nXPos,nYPos,nWidth,nHeight,nWidth*(9-i),0);
					m_ImageDisCardNumHT.DrawImage(pDC,nXPos,nYPos,nWidth,nHeight,nImageXPos,nImageYPos);
				}
			}

		
			if (m_wCardCount>10)
			{
				count = m_wCardCount;

				if (count < 21 && count>15)//第二排前面五张
				{
					for (int i=count; i>15; --i)
					{
						int nXPos=m_ControlPoint.x-i*23+433;
						int nYPos=m_ControlPoint.y+23;
						int nImageXPos=(29-i)*nItemWidth;
						int nImageYPos=(GetImageIndex(m_cbCardData[i-1])-1)*nItemHeight;
						m_ImageUserDisCardHT.DrawImage(pDC,nXPos,nYPos,nWidth,nHeight,nWidth*(29-i),0);
						m_ImageDisCardNumHT.DrawImage(pDC,nXPos,nYPos,nWidth,nHeight,nImageXPos,nImageYPos);
					}
					count-=(count-15);
				}

				if (count < 16) //第二排最后五张
				{
					for(int i = 10; i<count; ++i)
					{
						int nXPos=m_ControlPoint.x-i*24+421;
						int nYPos=m_ControlPoint.y+23;
						int nImageXPos=(29-i)*nItemWidth;
						int nImageYPos=(GetImageIndex(m_cbCardData[i])-1)*nItemHeight;
						m_ImageUserDisCardHT.DrawImage(pDC,nXPos,nYPos,nWidth,nHeight,nWidth*(29-i),0);
						m_ImageDisCardNumHT.DrawImage(pDC,nXPos,nYPos,nWidth,nHeight,nImageXPos,nImageYPos);
					}
				}
			}

			break;
		}
	}

	return;
}

//增加扑克
bool CDiscardCard::AddCardItem(BYTE cbCardData)
{
	//清理扑克
	if (m_wCardCount>=CountArray(m_cbCardData))
	{
		m_wCardCount--;
		MoveMemory(m_cbCardData,m_cbCardData+1,CountArray(m_cbCardData)-1);
	}

	//设置扑克
	m_cbCardData[m_wCardCount++]=cbCardData;

	return true;
}

//设置扑克
bool CDiscardCard::SetCardData(const BYTE cbCardData[], WORD wCardCount)
{
	//效验大小
	ASSERT(wCardCount<=CountArray(m_cbCardData));
	if (wCardCount>CountArray(m_cbCardData))
		return false;

	//设置扑克
	m_wCardCount=wCardCount;
	CopyMemory(m_cbCardData,cbCardData,sizeof(m_cbCardData[0])*wCardCount);

	return true;
}


//////////////////////////////////////////////////////////////////////////

//构造函数
CTableCard::CTableCard()
{
	//扑克数据
	m_wCardCount=0;
	ZeroMemory(m_cbCardData,sizeof(m_cbCardData));

	//控制变量
	m_ControlPoint.SetPoint(0,0);
	m_CardDirection=Direction_East;

	//图片资源
	TCHAR szResourcePath[MAX_PATH];
	GetCurrentDirectory(MAX_PATH,szResourcePath);
	CString strPath(szResourcePath);
	strPath.Append("\\SparrowCS\\Picture\\Card\\");
	m_ImageUserTableCardHB.LoadImage(TEXT(strPath+"USER_TABLE_CARD_HB.png"));
	m_ImageUserTableCardHT.LoadImage(TEXT(strPath+"USER_TABLE_CARD_HT.png"));
	m_ImageUserTableCardVL.LoadImage(TEXT(strPath+"USER_TABLE_CARD_VL.png"));
	m_ImageUserTableCardVR.LoadImage(TEXT(strPath+"USER_TABLE_CARD_VR.png"));

	m_ImageTabCardNumVL.LoadImage(TEXT(strPath+"USER_TABCARDNUM_VL.png"));
	m_ImageTabCardNumVR.LoadImage(TEXT(strPath+"USER_TABCARDNUM_VR.png"));
	m_ImageTabCardNumHT.LoadImage(TEXT(strPath+"USER_TABCARDNUM_HT.png"));
	m_ImageTabCardNumHB.LoadImage(TEXT(strPath+"USER_TABCARDNUM_HB.png"));


	return;
}

//析构函数
CTableCard::~CTableCard()
{
}



//获取位置
int CTableCard::GetImageIndex(BYTE cbCardData)
{
	//背景判断
	if (cbCardData==0) 
		return 0;

	//计算位置
	BYTE cbValue=cbCardData&MASK_VALUE;
	BYTE cbColor=(cbCardData&MASK_COLOR)>>4;
	return (cbColor>=0x03)?(cbValue+27):(cbColor*9+cbValue);
}

//绘画扑克
void CTableCard::DrawCardControl(CDC * pDC)
{
	//绘画控制
	switch (m_CardDirection)
	{
	case Direction_East:	//东向
		{
			//绘画扑克
			int nWidth=m_ImageUserTableCardVR.GetWidth()/14;
			int nHeight=m_ImageUserTableCardVR.GetHeight();
			int nItemWidth = nWidth;
			int nItemHeight =  m_ImageTabCardNumVR.GetHeight()/27;

			int num=0;
			if (2 == m_wCardCount%3)  //胡牌
			{
				num=m_wCardCount-1;
			}
			else if(1 == m_wCardCount%3) //不胡
			{
				num=m_wCardCount;
			}
			for (WORD i=0;i<=num;i++)
			{
				int nXPos=0,nYPos=0;				
				if (i ==0 &&  2 == (m_wCardCount%3))
				{//胡牌
					nXPos=m_ControlPoint.x-32-7*(13-i);
					nYPos=m_ControlPoint.y+i*18;
					int nImageXPos=13*nItemWidth;
					int nImageYPos=(GetImageIndex(m_cbCardData[m_wCardCount-1])-1)*nItemHeight;
					m_ImageUserTableCardVR.DrawImage(pDC,nXPos,nYPos,nWidth,nHeight,nImageXPos,0);
					m_ImageTabCardNumVR.DrawImage(pDC,nXPos,nYPos,nWidth,nHeight,nImageXPos,nImageYPos);
				}
				if(0!=i)
				{
					if (1 == i)
					{
						nXPos=m_ControlPoint.x-33-7*(13-i)-1;
						nYPos=m_ControlPoint.y+i*18-9;
					}
					else if (2 == i)
					{
						nXPos=m_ControlPoint.x-33-7*(13-i)-1;
						nYPos=m_ControlPoint.y+i*18-17;
					}
					else if (3 == i)
					{
						nXPos=m_ControlPoint.x-33-7*(13-i)-2;
						nYPos=m_ControlPoint.y+i*18-25;
					}
					else if (4 == i)
					{
						nXPos=m_ControlPoint.x-33-7*(13-i)-2;
						nYPos=m_ControlPoint.y+i*18-33;
					}
					else if (5 == i)
					{
						nXPos=m_ControlPoint.x-33-7*(13-i)-2;
						nYPos=m_ControlPoint.y+i*18-40;
					}
					else if (6 == i)
					{
						nXPos=m_ControlPoint.x-33-7*(13-i)-1;
						nYPos=m_ControlPoint.y+i*18-47;
					}
					else if (7 == i)
					{
						nXPos=m_ControlPoint.x-33-7*(13-i)-2;
						nYPos=m_ControlPoint.y+i*18-53;
					}
					else if (8 == i)
					{
						nXPos=m_ControlPoint.x-33-7*(13-i);
						nYPos=m_ControlPoint.y+i*18-58;
					}
					else if (9 == i)
					{
						nXPos=m_ControlPoint.x-33-7*(13-i)+2;
						nYPos=m_ControlPoint.y+i*18-62;
					}
					else if (10 == i)
					{
						nXPos=m_ControlPoint.x-33-7*(13-i)+5;
						nYPos=m_ControlPoint.y+i*18-65;
					}
					else if (11 == i)
					{
						nXPos=m_ControlPoint.x-33-7*(13-i)+9;
						nYPos=m_ControlPoint.y+i*18-67;
					}
					else if (12 == i)
					{
						nXPos=m_ControlPoint.x-33-7*(13-i)+11;
						nYPos=m_ControlPoint.y+i*18-69;
					}
					else if (13 == i)
					{
						nXPos=m_ControlPoint.x-33-7*(13-i)+17;
						nYPos=m_ControlPoint.y+i*18-69;
					}

					int nImageXPos=(13 - i)*nItemWidth;
					int nImageYPos=(GetImageIndex(m_cbCardData[num-i])-1)*nItemHeight;
					m_ImageUserTableCardVR.DrawImage(pDC,nXPos,nYPos,nWidth,nHeight,nImageXPos,0);
					m_ImageTabCardNumVR.DrawImage(pDC,nXPos,nYPos,nWidth,nHeight,nImageXPos,nImageYPos);
				}
			
			}

			break;
		}
	case Direction_South:	//南向
		{//绘画扑克
			int nWidth=m_ImageUserTableCardHB.GetWidth()/14;
			int nHeight=m_ImageUserTableCardHB.GetHeight();

			int nItemWidth = nWidth;
			int nItemHeight =  m_ImageTabCardNumHB.GetHeight()/27;
			//绘画背景
			int count = m_wCardCount;
			if (2 == count%3)                  //胡牌14
			{
				if (count > 7) //前七张
				{
					for(int i = count; i>7; --i)
					{
						int nXPos=m_ControlPoint.x-i*34+204;
						int nYPos=m_ControlPoint.y;
						int nImageXPos=(14 - i)*nItemWidth;
						int nImageYPos=(GetImageIndex(m_cbCardData[count-i])-1)*nItemHeight;
						m_ImageUserTableCardHB.DrawImage(pDC,nXPos,nYPos,nWidth,nHeight,nImageXPos,0);
						m_ImageTabCardNumHB.DrawImage(pDC,nXPos,nYPos,nWidth,nHeight,nImageXPos,nImageYPos);
					}
					count -= (count -7);
				}

				if (count > 0)//后七张
				{
					for (int i=0; i<count; ++i)
					{
						int nXPos=m_ControlPoint.x-i*34+170;
						int nYPos=m_ControlPoint.y;
						int nImageXPos=(13 - i)*nItemWidth;
						int nImageYPos=(GetImageIndex(m_cbCardData[m_wCardCount-i-1])-1)*nItemHeight;
						m_ImageUserTableCardHB.DrawImage(pDC,nXPos,nYPos,nWidth,nHeight,nImageXPos,0);
						m_ImageTabCardNumHB.DrawImage(pDC,nXPos,nYPos,nWidth,nHeight,nImageXPos,nImageYPos);
					}
				}
			}
			else  if (1 == count%3)             //没胡13
			{
				if (count > 6) //前七张
				{
					for(int i = count; i>6; --i)
					{
						int nXPos=m_ControlPoint.x-(i+1)*34+204;
						int nYPos=m_ControlPoint.y;
						int nImageXPos=(13 - i)*nItemWidth;
						int nImageYPos=(GetImageIndex(m_cbCardData[count-i])-1)*nItemHeight;
						m_ImageUserTableCardHB.DrawImage(pDC,nXPos,nYPos,nWidth,nHeight,nImageXPos,0);
						m_ImageTabCardNumHB.DrawImage(pDC,nXPos,nYPos,nWidth,nHeight,nImageXPos,nImageYPos);
					}
					count -= (count -6);
				}

				if (count > 0)//后六张
				{
					for (int i=0; i<count; ++i)
					{
						int nXPos=m_ControlPoint.x-(i+1)*34+170;
						int nYPos=m_ControlPoint.y;
						int nImageXPos=(12 - i)*nItemWidth;
						int nImageYPos=(GetImageIndex(m_cbCardData[m_wCardCount-i-1])-1)*nItemHeight;
						m_ImageUserTableCardHB.DrawImage(pDC,nXPos,nYPos,nWidth,nHeight,nImageXPos,0);
						m_ImageTabCardNumHB.DrawImage(pDC,nXPos,nYPos,nWidth,nHeight,nImageXPos,nImageYPos);
					}
				}
			}
			
			break;
		}
	case Direction_West:	//西向
		{
			//绘画扑克
			int nWidth=m_ImageUserTableCardVL.GetWidth()/14;
			int nHeight=m_ImageUserTableCardVL.GetHeight();

			int nItemWidth = nWidth;
			int nItemHeight =  m_ImageTabCardNumVL.GetHeight()/27;

			int num=0;
			if (2 == m_wCardCount%3)  //胡牌
			{
				num=m_wCardCount-1;
			}
			else if(1 == m_wCardCount%3) //不胡
			{
				num=m_wCardCount;
			}
			for (WORD i=13-num;i<=13;i++)
			{
				int nXPos=0,nYPos=0;
				if (13!=i)
				{
					if (0 == i)
					{
						nXPos=m_ControlPoint.x+9*(13-i);
						nYPos=m_ControlPoint.y+i*21-1;
					}
					else if (1 == i)
					{
						nXPos=m_ControlPoint.x+9*(13-i)+3;
						nYPos=m_ControlPoint.y+i*21-9-3;
					}
					else if (2 == i)
					{
						nXPos=m_ControlPoint.x+9*(13-i)+6;
						nYPos=m_ControlPoint.y+i*21-16-7;
					}
					else if (3 == i)
					{
						nXPos=m_ControlPoint.x+9*(13-i)+8;
						nYPos=m_ControlPoint.y+i*21-23-11;
					}
					else if (4 == i)
					{
						nXPos=m_ControlPoint.x+9*(13-i)+11;
						nYPos=m_ControlPoint.y+i*21-30-15;
					}
					else if (5 == i)
					{
						nXPos=m_ControlPoint.x+9*(13-i)+13;
						nYPos=m_ControlPoint.y+i*21-35-20;
					}
					else if (6 == i)
					{
						nXPos=m_ControlPoint.x+9*(13-i)+15;
						nYPos=m_ControlPoint.y+i*21-40-25;
					}
					else if (7 == i)
					{
						nXPos=m_ControlPoint.x+9*(13-i)+17;
						nYPos=m_ControlPoint.y+i*21-45-30;
					}
					else if (8 == i)
					{
						nXPos=m_ControlPoint.x+9*(13-i)+17;
						nYPos=m_ControlPoint.y+i*21-48-35;
					}
					else if (9 == i)
					{
						nXPos=m_ControlPoint.x+9*(13-i)+17;
						nYPos=m_ControlPoint.y+i*21-50-41;
					}
					else if (10 == i)
					{
						nXPos=m_ControlPoint.x+9*(13-i)+18;
						nYPos=m_ControlPoint.y+i*21-51-49;
					}
					else if (11 == i)
					{
						nXPos=m_ControlPoint.x+9*(13-i)+18;
						nYPos=m_ControlPoint.y+i*21-51-56;
					}
					else if (12 == i)
					{
						nXPos=m_ControlPoint.x+9*(13-i)+16;
						nYPos=m_ControlPoint.y+i*21-51-62;
					}

					int nImageXPos=(13 - i)*nItemWidth;
					int nImageYPos=(GetImageIndex(m_cbCardData[i+num-13])-1)*nItemHeight;
					m_ImageUserTableCardVL.DrawImage(pDC,nXPos,nYPos,nWidth,nHeight,nWidth*(13 - i),0);
					m_ImageTabCardNumVL.DrawImage(pDC,nXPos,nYPos,nWidth,nHeight,nImageXPos,nImageYPos);
				}


				if (i ==13 && 2 == (m_wCardCount%3))
				{
					nXPos=m_ControlPoint.x+9*(13-i)+15;
					nYPos=m_ControlPoint.y+i*21-49-68;
					int nImageXPos=0;
					int nImageYPos=(GetImageIndex(m_cbCardData[m_wCardCount-1])-1)*nItemHeight;
					m_ImageUserTableCardVL.DrawImage(pDC,nXPos,nYPos,nWidth,nHeight,0,0);
					m_ImageTabCardNumVL.DrawImage(pDC,nXPos,nYPos,nWidth,nHeight,nImageXPos,nImageYPos);
				}

			}


			break;
		}
	case Direction_North:	//北向
		{//绘画扑克
			int nWidth=m_ImageUserTableCardHT.GetWidth()/14;
			int nHeight=m_ImageUserTableCardHT.GetHeight();

			int nItemWidth = nWidth;
			int nItemHeight =  m_ImageTabCardNumHT.GetHeight()/27;

			//绘画背景
			int count = m_wCardCount;
			if (2 == count%3)                  //胡牌14
			{
				if (count > 7) //后七张
				{
					for(int i = count-1; i>6; --i)
					{
						int nXPos=m_ControlPoint.x+(i%7)*23+30;
						int nYPos=m_ControlPoint.y;
						int nImageXPos=i*nItemWidth;
						int nImageYPos=(GetImageIndex(m_cbCardData[i])-1)*nItemHeight;
						m_ImageUserTableCardHT.DrawImage(pDC,nXPos,nYPos,nWidth,nHeight,nWidth*i,0);
						m_ImageTabCardNumHT.DrawImage(pDC,nXPos,nYPos,nWidth,nHeight,nImageXPos,nImageYPos);
					}
					count -= (count -7);
				}

				if (count > 0)//前七张
				{
					for (int i=0; i<count; ++i)
					{
						int nXPos=m_ControlPoint.x+(i%7)*23-130;
						int nYPos=m_ControlPoint.y;
						int nImageXPos=i*nItemWidth;
						int nImageYPos=(GetImageIndex(m_cbCardData[i])-1)*nItemHeight;
						m_ImageUserTableCardHT.DrawImage(pDC,nXPos,nYPos,nWidth,nHeight,nWidth*i,0);
						m_ImageTabCardNumHT.DrawImage(pDC,nXPos,nYPos,nWidth,nHeight,nImageXPos,nImageYPos);
					}
				}
			}
			else  if (1 == count%3)             //没胡13
			{
				if (count > 7) //后七张
				{
					for(int i = count; i>6; --i)
					{
						int nXPos=m_ControlPoint.x+(i%7)*23+30;
						int nYPos=m_ControlPoint.y;
						int nImageXPos=i*nItemWidth;
						int nImageYPos=(GetImageIndex(m_cbCardData[i-1])-1)*nItemHeight;
						m_ImageUserTableCardHT.DrawImage(pDC,nXPos,nYPos,nWidth,nHeight,nWidth*i,0);
						m_ImageTabCardNumHT.DrawImage(pDC,nXPos,nYPos,nWidth,nHeight,nImageXPos,nImageYPos);
					}
					count -= (count -6);
				}

				if (count > 0)//前六张
				{
					for (int i=0; i<count; ++i)
					{
						int nXPos=m_ControlPoint.x+(i%6)*23-130+23;
						int nYPos=m_ControlPoint.y;
						int nImageXPos=i*nItemWidth;
						int nImageYPos=(GetImageIndex(m_cbCardData[i])-1)*nItemHeight;
						m_ImageUserTableCardHT.DrawImage(pDC,nXPos,nYPos,nWidth,nHeight,nWidth*i,0);
						m_ImageTabCardNumHT.DrawImage(pDC,nXPos,nYPos,nWidth,nHeight,nImageXPos,nImageYPos);
					}
				}
			}

			break;
		}
	}

	return;
}

//设置扑克
bool CTableCard::SetCardData(const BYTE cbCardData[], WORD wCardCount)
{
	//效验大小
	ASSERT(wCardCount<=CountArray(m_cbCardData));
	if (wCardCount>CountArray(m_cbCardData))
		return false;

	//设置扑克
	m_wCardCount=wCardCount;
	CopyMemory(m_cbCardData,cbCardData,sizeof(m_cbCardData[0])*wCardCount);

	return true;
}

//////////////////////////////////////////////////////////////////////////

//构造函数
CCardControl::CCardControl()
{
	//状态变量
	m_bPositively=false;
	m_bDisplayItem=false;

	//位置变量
	m_XCollocateMode=enXCenter;
	m_YCollocateMode=enYCenter;
	m_BenchmarkPos.SetPoint(0,0);

	//扑克数据
	m_wCardCount=0;
	m_wHoverItem=INVALID_ITEM;
	ZeroMemory(&m_CurrentCard,sizeof(m_CurrentCard));
	ZeroMemory(&m_CardItemArray,sizeof(m_CardItemArray));

	//加载设置
	m_ControlPoint.SetPoint(0,0);
	m_ControlSize.cy=CARD_HEIGHT+POS_SHOOT;
	m_ControlSize.cx=(CountArray(m_CardItemArray)+1)*CARD_WIDTH+POS_SPACE;


	//变量定义
	TCHAR szResourcePath[MAX_PATH];
	GetCurrentDirectory(MAX_PATH,szResourcePath);
	CString strPath(szResourcePath);
	strPath.Append("\\SparrowCS\\Picture\\Card\\");

	//用户扑克
	//3DView
	//m_ImageMyselfCard.LoadImage(TEXT(strPath+"myselfcard.png"));

	m_ImageUserBottom.LoadImage(TEXT(strPath+"CARD_USER_BOTTOM.png"));
	m_ImageOutCardNum.LoadImage(TEXT(strPath+"OUTCARDNUM.png"));

	m_iCardIndex = INVALID_ITEM;
	m_btCurCardData = 0;
	m_iCurCardX = 0;
	return;
}

//析构函数
CCardControl::~CCardControl()
{
	
}

//基准位置
void CCardControl::SetBenchmarkPos(int nXPos, int nYPos, enXCollocateMode XCollocateMode, enYCollocateMode YCollocateMode)
{
	//设置变量
	m_BenchmarkPos.x=nXPos;
	m_BenchmarkPos.y=nYPos;
	m_XCollocateMode=XCollocateMode;
	m_YCollocateMode=YCollocateMode;

	//横向位置
	switch (m_XCollocateMode)
	{
	case enXLeft:	{ m_ControlPoint.x=m_BenchmarkPos.x; break; }
	case enXCenter: { m_ControlPoint.x=m_BenchmarkPos.x-m_ControlSize.cx/2; break; }
	case enXRight:	{ m_ControlPoint.x=m_BenchmarkPos.x-m_ControlSize.cx; break; }
	}

	//竖向位置
	switch (m_YCollocateMode)
	{
	case enYTop:	{ m_ControlPoint.y=m_BenchmarkPos.y; break; }
	case enYCenter: { m_ControlPoint.y=m_BenchmarkPos.y-m_ControlSize.cy/2; break; }
	case enYBottom: { m_ControlPoint.y=m_BenchmarkPos.y-m_ControlSize.cy; break; }
	}

	return;
}

//基准位置
void CCardControl::SetBenchmarkPos(const CPoint & BenchmarkPos, enXCollocateMode XCollocateMode, enYCollocateMode YCollocateMode)
{
	//设置变量
	m_BenchmarkPos=BenchmarkPos;
	m_XCollocateMode=XCollocateMode;
	m_YCollocateMode=YCollocateMode;

	//横向位置
	switch (m_XCollocateMode)
	{
	case enXLeft:	{ m_ControlPoint.x=m_BenchmarkPos.x; break; }
	case enXCenter: { m_ControlPoint.x=m_BenchmarkPos.x-m_ControlSize.cx/2; break; }
	case enXRight:	{ m_ControlPoint.x=m_BenchmarkPos.x-m_ControlSize.cx; break; }
	}

	//竖向位置
	switch (m_YCollocateMode)
	{
	case enYTop:	{ m_ControlPoint.y=m_BenchmarkPos.y; break; }
	case enYCenter: { m_ControlPoint.y=m_BenchmarkPos.y-m_ControlSize.cy/2; break; }
	case enYBottom: { m_ControlPoint.y=m_BenchmarkPos.y-m_ControlSize.cy; break; }
	}

	return;
}

//获取扑克
BYTE CCardControl::GetHoverCard()
{
	//获取扑克
	if (m_wHoverItem!=INVALID_ITEM)
	{
		if (m_wHoverItem==CountArray(m_CardItemArray))
			return m_CurrentCard.cbCardData;
		return m_CardItemArray[m_wHoverItem].cbCardData;
	}

	return 0;
}

//设置扑克
bool CCardControl::SetCurrentCard(BYTE cbCardData)
{
	//设置变量
	m_CurrentCard.bShoot=false;
	m_CurrentCard.cbCardData=cbCardData;

	return true;
}

//设置扑克
bool CCardControl::SetCurrentCard(tagCardItem CardItem)
{
	//设置变量
	m_CurrentCard.bShoot=CardItem.bShoot;
	m_CurrentCard.cbCardData=CardItem.cbCardData;

	return true;
}

//设置扑克
bool CCardControl::SetCardData(const BYTE cbCardData[], WORD wCardCount, BYTE cbCurrentCard)
{
	//效验大小
	ASSERT(wCardCount<=CountArray(m_CardItemArray));
	if (wCardCount>CountArray(m_CardItemArray)) 
		return false;

	//当前扑克
	m_CurrentCard.bShoot=false;
	m_CurrentCard.cbCardData=cbCurrentCard;

	//设置扑克
	m_wCardCount=wCardCount;
	for (WORD i=0;i<m_wCardCount;i++)
	{
		m_CardItemArray[i].bShoot=false;
		m_CardItemArray[i].cbCardData=cbCardData[i];
	}

	return true;
}

//设置扑克
bool CCardControl::SetCardItem(const tagCardItem CardItemArray[], WORD wCardCount)
{
	//效验大小
	ASSERT(wCardCount<=CountArray(m_CardItemArray));
	if (wCardCount>CountArray(m_CardItemArray))
		return false;

	//设置扑克
	m_wCardCount=wCardCount;
	for (WORD i=0;i<m_wCardCount;i++)
	{
		m_CardItemArray[i].bShoot=CardItemArray[i].bShoot;
		m_CardItemArray[i].cbCardData=CardItemArray[i].cbCardData;
	}

	return true;
}


//获取位置
int CCardControl::GetImageIndex(BYTE cbCardData)
{
	//背景判断
	if (cbCardData==0) 
		return 0;

	//计算位置
	BYTE cbValue=cbCardData&MASK_VALUE;
	BYTE cbColor=(cbCardData&MASK_COLOR)>>4;
	return (cbColor>=0x03)?(cbValue+27):(cbColor*9+cbValue);
}


//绘画扑克
void CCardControl::DrawCardControl(CDC * pDC)
{
	//绘画准备
	int nXExcursion=m_ControlPoint.x+(CountArray(m_CardItemArray)-m_wCardCount)*CARD_WIDTH;

	//绘画扑克
	for (WORD i=0;i<m_wCardCount;i++)
	{
		//计算位置
		int nXScreenPos=nXExcursion+CARD_WIDTH*i;
		int nYScreenPos=m_ControlPoint.y+(((m_CardItemArray[i].bShoot==false)&&(m_wHoverItem!=i))?POS_SHOOT:0);

		//绘画扑克
		BYTE cbCardData=(m_bDisplayItem==true)?m_CardItemArray[i].cbCardData:0;
		BYTE byIndex= GetImageIndex(cbCardData);
			m_ImageUserBottom.DrawImage(pDC,nXScreenPos,nYScreenPos,CARD_WIDTH,CARD_HEIGHT,byIndex*CARD_WIDTH,0,CARD_WIDTH,CARD_HEIGHT);
	}

	//当前扑克
	if (m_CurrentCard.cbCardData!=0)
	{
		//计算位置
		int nXScreenPos=m_ControlPoint.x+m_ControlSize.cx-CARD_WIDTH;
		int nYScreenPos=m_ControlPoint.y+(((m_CurrentCard.bShoot==false)&&(m_wHoverItem!=CountArray(m_CardItemArray)))?POS_SHOOT:0);

		//绘画扑克
		BYTE cbCardData=(m_bDisplayItem==true)?m_CurrentCard.cbCardData:0;
		BYTE byIndex= GetImageIndex(cbCardData);
		m_ImageUserBottom.DrawImage(pDC,nXScreenPos+8.5,nYScreenPos,CARD_WIDTH,CARD_HEIGHT,byIndex*CARD_WIDTH,0,CARD_WIDTH,CARD_HEIGHT);
	}

	return;
}


//索引切换
WORD CCardControl::SwitchCardPoint(CPoint & MousePoint)
{
	//基准位置
	int nXPos=MousePoint.x-m_ControlPoint.x;
	int nYPos=MousePoint.y-m_ControlPoint.y;

	//范围判断
	if ((nXPos<0)||(nXPos>m_ControlSize.cx)) 
		return INVALID_ITEM;
	if ((nYPos<POS_SHOOT)||(nYPos>m_ControlSize.cy)) 
		return INVALID_ITEM;

	//牌列子项
	if (nXPos<CARD_WIDTH*CountArray(m_CardItemArray))
	{
		WORD wViewIndex=(WORD)(nXPos/CARD_WIDTH)+m_wCardCount;
		if (wViewIndex>=CountArray(m_CardItemArray))
			return wViewIndex-CountArray(m_CardItemArray);
		return INVALID_ITEM;
	}

	//当前子项
	if ((m_CurrentCard.cbCardData!=0)&&(nXPos>=(m_ControlSize.cx-CARD_WIDTH))) 
		return CountArray(m_CardItemArray);

	return INVALID_ITEM;
}

//光标消息
bool CCardControl::OnEventSetCursor(CPoint Point, bool & bRePaint)
{
	//获取索引
	WORD wHoverItem=SwitchCardPoint(Point);

	//响应判断
	if ((m_bPositively==false)&&(m_wHoverItem!=INVALID_ITEM))
	{
		bRePaint=true;
		m_wHoverItem=INVALID_ITEM;
	}

	//更新判断
	if ((wHoverItem!=m_wHoverItem)&&(m_bPositively==true))
	{
		bRePaint=true;
		m_wHoverItem=wHoverItem;
		SetCursor(LoadCursor(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDC_CARD_CUR)));
	}

	return (wHoverItem!=INVALID_ITEM);
}

int CCardControl::CheckIndex(CPoint Point)
{
	//获取索引
	WORD wHoverItem=SwitchCardPoint(Point);

	m_iCardIndex = wHoverItem;
	m_iCurCardX = m_ControlPoint.x+(CountArray(m_CardItemArray)-m_wCardCount)*CARD_WIDTH;
	if (wHoverItem>=0 && wHoverItem<CountArray(m_CardItemArray))
	{

		m_iCurCardX = m_iCurCardX+wHoverItem*CARD_WIDTH+CARD_WIDTH/2;
		m_btCurCardData = m_CardItemArray[wHoverItem].cbCardData;

		return wHoverItem;
	}
	else if (13==wHoverItem)
	{
		m_iCurCardX = m_ControlPoint.x+m_ControlSize.cx-CARD_WIDTH/2+8;
		m_btCurCardData = m_CurrentCard.cbCardData;
		return wHoverItem;
	}
	return -1;
}


//get
void CCardControl::GetCurCardDataAndX(BYTE& btCardData, int& iX)
{
	btCardData = m_btCurCardData;
	iX = m_iCurCardX;
}
//set
void CCardControl::InitCurCardDataX()
{
	m_btCurCardData = 0;
	m_iCardIndex = INVALID_ITEM;
}
//////////////////////////////////////////////////////////////////////////

//变量声明
//CCardResource						g_CardResource;						//扑克资源

//////////////////////////////////////////////////////////////////////////
