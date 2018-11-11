#include "StdAfx.h"
#include "Seascene.h"

//构造函数
CSeaScene::CSeaScene(void)
{
	//设置变量
	m_bReverse=false;
	m_bTride=false;

	for (int i=0; i<CountArray(m_Bubble); ++i)
		MakeBubble(m_Bubble[i]);
}

//析构函数
CSeaScene::~CSeaScene(void)
{
	//释放内存
	m_BiologyFactory.FreeExtra();
}

//绘画函数
void CSeaScene::DrawScene()
{
	//合法判断
	if (m_Sprite==NULL||m_pTexture==NULL) return ;

	//绘制海底
	CRect drawRect;
	if (false==m_bTride)
	{
		drawRect.left=0;
		drawRect.top=0;
		drawRect.right=GetImageWidth(0);
		drawRect.bottom=GetImageHeight(0);

		//绘画图片	
		DrawRotateAndZoom(m_ptTarger[0].x,m_ptTarger[0].y,drawRect,m_bReverse?D3DX_PI:0.f,1.f,1.f,0,enXLeft,enYTop);
	}

	//绘画场景
	INT_PTR nEnumIndex=0;
	tagBiologyInfo *pBiologyInfo=NULL;
	do
	{
		//枚举金币
		pBiologyInfo=m_BiologyFactory.EnumActiveObject(nEnumIndex++);
		if (NULL==pBiologyInfo) break;

		//当前位置
		drawRect.left=(pBiologyInfo->byFrameIndex % pBiologyInfo->byColumnCount) * GetWidth(pBiologyInfo->byTextureIndex);
		drawRect.top=(pBiologyInfo->byFrameIndex / pBiologyInfo->byColumnCount) * GetHeight(pBiologyInfo->byTextureIndex);
		drawRect.right=drawRect.left+GetWidth(pBiologyInfo->byTextureIndex);
		drawRect.bottom=drawRect.top+GetHeight(pBiologyInfo->byTextureIndex);

		//绘画图片
		DrawRotateAndZoom(pBiologyInfo->nPosX,pBiologyInfo->nPosY,drawRect,pBiologyInfo->fReAngle,pBiologyInfo->fScaling,pBiologyInfo->fScaling,pBiologyInfo->byTextureIndex,enXLeft,enYTop);

		//移动帧
		if(GetTickCount()>=(pBiologyInfo->dwMoveTimes+pBiologyInfo->bySpaceTimes))
		{
			//重设时间
			pBiologyInfo->dwMoveTimes=GetTickCount();

			//移动帧
			if (true==pBiologyInfo->bAsc)
			{
				if (pBiologyInfo->byFrameIndex>=pBiologyInfo->byFrameCount-1) 
				{
					pBiologyInfo->bAsc=false;
					pBiologyInfo->byFrameIndex-=1;
				}
				else
				{
					pBiologyInfo->byFrameIndex+=1;					
				}
			}
			else
			{
				if (pBiologyInfo->byFrameIndex==0)
				{
					pBiologyInfo->bAsc=true;
					pBiologyInfo->byFrameIndex+=1;
				}
				else
				{
					pBiologyInfo->byFrameIndex-=1;
				}
			}
		}
	} while (true);

	//绘画气泡
	drawRect.left=0;
	drawRect.top=0;
	drawRect.right=drawRect.left+GetWidth(MAX_TEXTURE_COUNT-1);
	drawRect.bottom=drawRect.top+GetHeight(MAX_TEXTURE_COUNT-1);
	
	for (int i=0; i<CountArray(m_Bubble); ++i)
	{		
		SetAlphaBlend(m_Bubble[i].byAlphaBlend,MAX_TEXTURE_COUNT-1);
		DrawRotateAndZoom(m_Bubble[i].nPosX,m_Bubble[i].nPosY,drawRect,0.f,m_Bubble[i].fScaling,m_Bubble[i].fScaling,MAX_TEXTURE_COUNT-1,enXLeft,enYTop);

		if (m_Bubble[i].dwMoveTimes+200<GetTickCount())
		{
			m_Bubble[i].dwMoveTimes=GetTickCount();
			m_Bubble[i].byAlphaBlend-=(min(5,m_Bubble[i].byAlphaBlend));
			m_Bubble[i].fScaling+=0.01f;
			m_Bubble[i].nPosX+=m_Bubble[i].nMoveSpaceX;
			m_Bubble[i].nPosY+=m_Bubble[i].nMoveSpaceY;
			if (0==m_Bubble[i].byAlphaBlend) MakeBubble(m_Bubble[i]);
		}
	}
}

//增加生物
bool CSeaScene::AddBiology(int nPosX, int nPosY, BYTE byFrameCount, BYTE byTextureIndex, BYTE byColumnCount, float fReAngle, BYTE bySpaceTimes, float fScaling)
{
	//获取子项
	tagBiologyInfo *pBiologyInfo=m_BiologyFactory.ActiveItem();
	ASSERT(NULL!=pBiologyInfo);
	if (NULL==pBiologyInfo) return false;

	//设置信息
	pBiologyInfo->byFrameCount=byFrameCount;
	pBiologyInfo->byFrameIndex=0;
	pBiologyInfo->byTextureIndex=byTextureIndex;
	pBiologyInfo->dwMoveTimes=GetTickCount();
	pBiologyInfo->nPosX=nPosX;
	pBiologyInfo->nPosY=nPosY;
	pBiologyInfo->byColumnCount=byColumnCount;
	pBiologyInfo->bySpaceTimes=bySpaceTimes;
	pBiologyInfo->fReAngle=fReAngle;
	pBiologyInfo->bAsc=true;
	pBiologyInfo->fScaling=fScaling;
		
	return true;
}

//产生气泡
void CSeaScene::MakeBubble(tagBubble &Bubble)
{
	ZeroMemory(&Bubble,sizeof(Bubble));
	Bubble.nPosX=300+rand()%350;
	Bubble.nPosY=130+rand()%250;
	Bubble.nMoveSpaceX=(rand()%2)?(1):(-1);
	Bubble.nMoveSpaceY=(rand()%2)?(1):(-1);
	Bubble.fScaling=(rand()%3+1)/10.f;
	Bubble.byAlphaBlend=100+rand()%100;
	Bubble.dwMoveTimes=GetTickCount();
}