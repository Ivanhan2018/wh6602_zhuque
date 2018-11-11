#include "StdAfx.h"
#include "Coin.h"
#include "Mathaide.h"

//构造函数
CCoin::CCoin(void)
{
}

//析构函数
CCoin::~CCoin(void)
{
}

//绘画金币
void CCoin::DrawCoin()
{
	//合法判断
	if (m_Sprite==NULL||m_pTexture==NULL) return ;
	if (0==m_CoinInfoFactory.GetActiveCount()) return;

	//绘画金币
	INT_PTR nEnumIndex=0;
	tagCoinInfo *pCoinInfo=NULL;
	do
	{
		//枚举金币
		pCoinInfo=m_CoinInfoFactory.EnumActiveObject(nEnumIndex);
		if (NULL==pCoinInfo) break;

		//绘画倍数
		DrawNumber(pCoinInfo->ptNumber.x,pCoinInfo->ptNumber.y,pCoinInfo->lCoinCount,pCoinInfo->byAlphaBlend);

		//状态判断
		bool bMoveNext=true;

		//当前位置
		CRect drawRect;

		////绘画背景
		//drawRect.left=0 * GetWidth(TEXTURE_INDEX_COIN_BK);
		//drawRect.top=2*GetHeight(TEXTURE_INDEX_COIN_BK);
		//drawRect.right=drawRect.left+GetWidth(TEXTURE_INDEX_COIN_BK);
		//drawRect.bottom=drawRect.top+GetHeight(TEXTURE_INDEX_COIN_BK);
		//DrawRotateAndZoom(100,100,drawRect,0.f,3,3,TEXTURE_INDEX_COIN_BK);

		//绘画金币
		drawRect.left=pCoinInfo->byFrameIndex * GetWidth(pCoinInfo->byTextureIndex);
		drawRect.top=0;
		drawRect.right=drawRect.left+GetWidth(pCoinInfo->byTextureIndex);
		drawRect.bottom=drawRect.top+GetHeight(pCoinInfo->byTextureIndex);

		//绘画图片
		DrawRotateAndZoom(pCoinInfo->ptTrace[pCoinInfo->wTracePtIndex].x,pCoinInfo->ptTrace[pCoinInfo->wTracePtIndex].y,drawRect,0.f,pCoinInfo->fScaling,pCoinInfo->fScaling,pCoinInfo->byTextureIndex);

		//完成判断
		if (pCoinInfo->wTracePtIndex>=pCoinInfo->wTracePtCount-1) 
		{
			//回收资源
			m_CoinInfoFactory.FreeItem(pCoinInfo);

			//设置标识
			bMoveNext=false;
		}

		//移动帧
		if(GetTickCount()>=(pCoinInfo->dwMoveTimes+pCoinInfo->bySpaceTimes))
		{
			//重设时间
			pCoinInfo->dwMoveTimes=GetTickCount();

			//移动帧
			pCoinInfo->byFrameIndex=(pCoinInfo->byFrameIndex+1)%COIN_FRAME_COUNT;

			//移动位置
			pCoinInfo->wTracePtIndex+=2;
			if (pCoinInfo->wTracePtIndex>=pCoinInfo->wTracePtCount-1) pCoinInfo->wTracePtIndex=pCoinInfo->wTracePtCount-1;

			//修改透明
			if (pCoinInfo->byAlphaBlend>=5) pCoinInfo->byAlphaBlend-=5;

			//修改倍数
			if (pCoinInfo->fScaling<1.2) pCoinInfo->fScaling+=0.01f;
		}

		//移动判断
		if (true==bMoveNext) ++nEnumIndex;

	} while (true);

}

//绘画数字
void CCoin::DrawNumber(int nXPos, int nYPos, int nNumber, BYTE byAlphaBlend)
{
	if (nNumber>0)
	{
		//获取属性
		const INT nNumberHeight=GetHeight(TEXTURE_INDEX_COIN_SHOW);
		const INT nNumberWidth=GetWidth(TEXTURE_INDEX_COIN_SHOW);

		//计算数目
		LONG lNumberCount=0;
		WORD wNumberTemp=nNumber;
		do
		{
			lNumberCount++;
			wNumberTemp/=10;
		} while (wNumberTemp>0L);

		//位置定义
		int const nSpace=0;
		INT nYDrawPos=nYPos;
		INT nXDrawPos=nXPos;

		//当前位置
		CRect drawRect;
		drawRect.left=0;
		drawRect.top=0;
		drawRect.right=drawRect.left+GetWidth();
		drawRect.bottom=drawRect.top+GetHeight();

		//绘画乘号
		SetAlphaBlend(byAlphaBlend,TEXTURE_INDEX_COIN_SHOW);
		DrawSprite(nXDrawPos,nYDrawPos,drawRect,TEXTURE_INDEX_COIN_SHOW);

		//绘画号码
		nXDrawPos+=lNumberCount*(nNumberWidth+nSpace);
		for (LONG i=0;i<lNumberCount;i++)
		{
			//绘画号码
			WORD wCellNumber=nNumber%10;
			drawRect.left=(wCellNumber+1)*nNumberWidth;
			drawRect.top=0;
			drawRect.right=drawRect.left+GetWidth(TEXTURE_INDEX_COIN_SHOW);
			drawRect.bottom=drawRect.top+GetHeight(TEXTURE_INDEX_COIN_SHOW);
			DrawSprite(nXDrawPos,nYDrawPos,drawRect,TEXTURE_INDEX_COIN_SHOW);

			//设置变量
			nNumber/=10;
			nXDrawPos-=(nNumberWidth+nSpace);
		}
	}
}

//增加金币
void CCoin::AddCoin(int nSourcePosX, int nSourcePosY, int nTargetPosX, int nTargetPosY, int nNumberX,int nNumberY,LONG lCoinCount, BYTE byTextureIndex)
{
	//获取子项
	tagCoinInfo *pCoinInfo=m_CoinInfoFactory.ActiveItem();
	ASSERT(NULL!=pCoinInfo);
	if (NULL==pCoinInfo) return;

	//初始变量
	ZeroMemory(pCoinInfo,sizeof(tagCoinInfo));

	//建立坐标
	int nPosX[2]={nSourcePosX,nTargetPosX};
	int nPosY[2]={nSourcePosY,nTargetPosY};
	pCoinInfo->ptNumber.x=nNumberX-40;
	pCoinInfo->ptNumber.y=nNumberY-((nTargetPosY>nSourcePosY)?50:-50);

	//计算轨迹
	CMathAide::BuildLinear(nPosX, nPosY, 2, pCoinInfo->ptTrace, CountArray(pCoinInfo->ptTrace), pCoinInfo->wTracePtCount, TRACE_POINT_SPACE_NORMAL);

	//合法判断
	if (pCoinInfo->wTracePtCount<2) return;

	//设置信息
	pCoinInfo->byTextureIndex=byTextureIndex;
	pCoinInfo->lCoinCount=lCoinCount;
	pCoinInfo->bySpaceTimes=20;
	pCoinInfo->dwMoveTimes=GetTickCount();
	pCoinInfo->byAlphaBlend=255;
	pCoinInfo->fScaling=1.f;
}