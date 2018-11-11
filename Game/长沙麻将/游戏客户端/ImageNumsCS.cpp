#include "StdAfx.h"
#include "ImageNumsCS.h"

CImageNumsCS::CImageNumsCS(void)
{
	m_lNums			=	0;		//数字
	m_bIsSign		=	false;	//有无符号
	m_bIsSeparator	=	false;	//有无分隔符
	m_enNumAlign	=	IA_LEFT;//对齐方式
	m_rcNumsArea.SetRect(0, 0, 100, 100);	//绘制数字区域

	//有资源
	m_lSepatatorW	=	0;//分隔符宽17
	m_lSepatatorH	=	0;//分隔符高25
	m_lNumWidth		=	0;//数字宽度
	m_lNumHeight	=	0;//数字高度
	
	//清空数字
	m_vecNumArray.clear();
}

CImageNumsCS::~CImageNumsCS(void)
{
	if (!m_ImageNums.IsNull())
	{
		m_ImageNums.DestroyImage();
	}
}


//设置数字
void CImageNumsCS::SetNums(long lNums)
{
	m_lNums = lNums;
	lNums = abs(lNums);
	m_vecNumArray.clear();
	do 
	{
		m_vecNumArray.push_back(lNums%10);
		lNums = lNums/10;
	} while (0!=lNums);
}

//设置有无符号
void CImageNumsCS::SetSign(bool bIsSign)
{
	m_bIsSign = bIsSign;
}

//设置分隔符号
void CImageNumsCS::SetSeparator(bool bIsSeparator)
{
	m_bIsSeparator = bIsSeparator;
}

//设置对齐试
void CImageNumsCS::SetNumAlign(NumAlign align)
{
	m_enNumAlign = align;
}

//设置数字显示区域
void CImageNumsCS::SetShowRect(CRect rect)
{
	m_rcNumsArea = rect;
}

//设置分隔符的宽与高
void CImageNumsCS::SetSeparatorWH(int iWidth, int iHeight)
{
	m_lSepatatorW	=	iWidth;//分隔符宽
	m_lSepatatorH	=	iHeight;//分隔符高
}

//设置图片
void CImageNumsCS::SetImageResource(char* pResource)
{
	if (NULL != pResource)
	{
		char chResource[50]={0};
		memset(chResource, 0, sizeof(chResource));
		sprintf(chResource, TEXT("%s"), pResource);
		
		//装载图片资源
		HINSTANCE hInstance = AfxGetInstanceHandle();
		TCHAR szResourcePath[MAX_PATH]="";
		GetCurrentDirectory(MAX_PATH,szResourcePath);
		CString strPath(szResourcePath);
		strPath.Append("\\SparrowCS\\Picture\\GameEnd\\");
		strPath.Append(chResource);
		m_ImageNums.LoadImage((LPCTSTR)strPath);//数字图片

		if (!m_ImageNums.IsNull())
		{//有资源
			m_lNumWidth		=	(m_ImageNums.GetWidth()-m_lSepatatorW)/12;//数字宽度
			m_lNumHeight	=	m_ImageNums.GetHeight()/4;//数字高度
		}
	}
}

//绘制数字
void CImageNumsCS::DrawImageNums(CDC* pDC)
{
	if (!m_ImageNums.IsNull())
	{
		long lBitNums = m_vecNumArray.size();
		if (lBitNums>0)
		{
			//总长度
			long lTotalImageNumsWidth = 0;
			//总数字宽度
			lTotalImageNumsWidth = lBitNums*m_lNumWidth;
			
			if (true == m_bIsSign)
			{//有符号
				lTotalImageNumsWidth += m_lNumWidth;
			}
			
			int iCountSeparatorNums = lBitNums/3;
			if (true == m_bIsSeparator)
			{//有分隔符号,并且还得判断一下要多少分隔符号
				if (0==lBitNums%3)
				{//如果是3的倍数,则少一个分隔符号
					iCountSeparatorNums -= 1;
				}
				lTotalImageNumsWidth += iCountSeparatorNums*m_lSepatatorW;
			}
			int iX = 0;
			if (IA_LEFT == m_enNumAlign)
			{//左对齐
				iX = m_rcNumsArea.left+lTotalImageNumsWidth;
			}
			else if (IA_CENTER == m_enNumAlign)
			{//居中对齐
				iX = m_rcNumsArea.left+(m_rcNumsArea.Width()-lTotalImageNumsWidth)/2+lTotalImageNumsWidth;//居中起点
			}
			else if (IA_RIGHT == m_enNumAlign)
			{//右对齐
				iX = m_rcNumsArea.right;
			}
			//减去一个数字的宽度为末点
			iX-=m_lNumWidth;

			int nYPos=0;
			if (m_lNums>0)
			{
				nYPos=2*m_lNumHeight;
			}
			else if (m_lNums<0)
			{
				nYPos=m_lNumHeight;
			}
			else
			{
				nYPos=3*m_lNumHeight;
			}

			int iCount3Point = 0;
			for (int i=0; i<lBitNums; i++)
			{//绘制数字
				if (m_vecNumArray[i]>=0 && m_vecNumArray[i]<=9)
				{
					if (true == m_bIsSign)
					{
						m_ImageNums.DrawImage(pDC, iX, m_rcNumsArea.top, m_lNumWidth, m_lNumHeight, m_vecNumArray[i]*m_lNumWidth, nYPos, m_lNumWidth, m_lNumHeight);
					}
					else
					{
						m_ImageNums.DrawImage(pDC, iX, m_rcNumsArea.top, m_lNumWidth, m_lNumHeight, m_vecNumArray[i]*m_lNumWidth, 0, m_lNumWidth, m_lNumHeight);
					}
					++iCount3Point;

					if ((true==m_bIsSeparator)&&(iCountSeparatorNums>0)&&(3==iCount3Point))
					{//绘制分隔符号
						iX-=m_lSepatatorW;
						m_ImageNums.DrawImage(pDC, iX, m_rcNumsArea.top, m_lSepatatorW, m_lSepatatorH, 12*m_lNumWidth, nYPos, m_lSepatatorW, m_lSepatatorH);
						iCount3Point = 0;
						--iCountSeparatorNums;
					}
					iX-=m_lNumWidth;
				}
			}

			if (true == m_bIsSign)
			{//绘制符号
				if (0 != m_lNums)
				{
					int iDrawX = (m_lNums>0)?10:11;
					m_ImageNums.DrawImage(pDC, iX, m_rcNumsArea.top, m_lNumWidth, m_lNumHeight, iDrawX*m_lNumWidth, nYPos, m_lNumWidth, m_lNumHeight);
				}				
			}
		}
	}
}


