#include "StdAfx.h"
#include "Bullet.h"
#include "Mathaide.h"

//构造函数
CBullet::CBullet(void)
{
	//设置变量
	m_hNotifyWnd=NULL;
	m_enBulletKind=enBulletKind_1;
	m_BulletFactory.FreeAllItem();
	m_dwBulletID=0L;
}

//析构函数
CBullet::~CBullet(void)
{
}

//激活子弹
tagBulletInfo *CBullet::ActiveNewBullet()
{
	//获取变量
	tagBulletInfo *pBulletInfo=m_BulletFactory.ActiveItem();
	ZeroMemory(pBulletInfo,sizeof(tagBulletInfo));

	return pBulletInfo;
}

//发射子弹
DWORD CBullet::Shoot(WORD wChairID, bool bCptrMode, DWORD dwCptrFishID[], WORD wCptrFishCount, int nSourceX, int nSourceXY, float fAngle, enBulletCountKind BulletCountKind, BYTE byBulletCount)
{
	//数目判断
	if (false==bCptrMode && m_BulletFactory.GetActiveCount()>=50) return 0L;

	//合法判断
	ASSERT(byBulletCount<=5);
	if (!(byBulletCount<=5)) return 0L;

	//激活子弹
	tagBulletInfo *pBulletInfo=ActiveNewBullet();
	ASSERT(NULL!=pBulletInfo);
	if (NULL==pBulletInfo) return 0L;

	//变量定义
	int nPosX[2], nPosY[2];
	nPosX[0]=nSourceX;
	nPosY[0]=nSourceXY;

	//计算结束点
	int const nMaxLen=1200;
	float fTmpValue=0.f;
	fTmpValue=nPosX[0]+sin(fAngle)*nMaxLen;
	nPosX[1]=LONG(fTmpValue);
	fTmpValue=nPosY[0]-cos(fAngle)*nMaxLen;
	nPosY[1]=LONG(fTmpValue);

	//生成直线
	bool bAccelerate=enBulletCountKind_800==BulletCountKind?true:false;
	CMathAide::BuildLinear(nPosX, nPosY, 2, pBulletInfo->ptTrace, CountArray(pBulletInfo->ptTrace), pBulletInfo->wTracePtCount, bAccelerate?8:7, m_rcBound);

	//合法判断
	int const static nExcursion=6;
	if (pBulletInfo->wTracePtCount<(nExcursion+2)) 
	{
		//释放资源
		m_BulletFactory.FreeItem(pBulletInfo);

		return 0L;
	}

	//剔除起始点到炮口的点
	pBulletInfo->wTracePtCount-=nExcursion;
	CopyMemory(pBulletInfo->ptTrace,pBulletInfo->ptTrace+nExcursion,pBulletInfo->wTracePtCount*sizeof(pBulletInfo->ptTrace[0]));

	//首个角度
	LONG lDistance=CMathAide::CalcDistance(pBulletInfo->ptTrace[pBulletInfo->wTracePtCount-1].x,pBulletInfo->ptTrace[pBulletInfo->wTracePtCount-1].y,pBulletInfo->ptTrace[0].x,pBulletInfo->ptTrace[0].y);
	float fSinValue=(pBulletInfo->ptTrace[pBulletInfo->wTracePtCount-1].x-pBulletInfo->ptTrace[0].x)/(lDistance*1.0f);
	pBulletInfo->fAngle=acos(fSinValue);

	//调整角度
	if (nPosY[1]<=nPosY[0]) 
	{
		pBulletInfo->fAngle=2*D3DX_PI-pBulletInfo->fAngle;
	}

	//设置变量
	pBulletInfo->bCptrMode=bCptrMode;
	pBulletInfo->BulletStatus=enBulletStatus_Shoot;
	CopyMemory(pBulletInfo->dwCptrFishID,dwCptrFishID,min(wCptrFishCount,CountArray(pBulletInfo->dwCptrFishID))*sizeof(DWORD));
	pBulletInfo->dwMoveTimes=GetTickCount();
	pBulletInfo->bySpaceTimes=enBulletCountKind_800==BulletCountKind?BULLET_SPEED_ACCELERATE:BULLET_SPEED_NORMAL;
	pBulletInfo->byFrameIndex=0;
	pBulletInfo->wTracePtIndex=0;
	pBulletInfo->dwBulletID=bCptrMode?(++m_dwBulletID):0L;
	pBulletInfo->byBulletCount=byBulletCount;
	//pBulletInfo->byAddedFrame=(byBulletCount==5)?0:5;
	pBulletInfo->byShootUser=BYTE(wChairID);
	pBulletInfo->byTextureIndex=BulletCountKind-enBulletCountKind_100;

	//修正标识
	if (true==bCptrMode && 0L==pBulletInfo->dwBulletID)
	{
		m_dwBulletID=1;
		pBulletInfo->dwBulletID=1;
	}

	return bCptrMode?m_dwBulletID:0L;
}

//绘画子弹
void CBullet::DrawBullet()
{
	//合法判断
	if (m_Sprite==NULL||m_pTexture==NULL) return ;
	if (enBulletKind_No==m_enBulletKind) return;
	if (0==m_BulletFactory.GetActiveCount()) return;

	//绘画子弹
	INT_PTR nEnumIndex=0;
	tagBulletInfo *pBulletInfo=NULL;
	do
	{
		//枚举子弹
		pBulletInfo=m_BulletFactory.EnumActiveObject(nEnumIndex);
		if (NULL==pBulletInfo) break;

		//状态判断
		if (pBulletInfo->BulletStatus==enBulletStatus_No)
		{
			m_BulletFactory.FreeItem(pBulletInfo);
			++nEnumIndex;
			continue;
		}

		//状态判断
		bool bShowNet=false;
		if (enBulletStatus_Shoot==pBulletInfo->BulletStatus)
		{
			//当前位置
			CRect drawRect;
			drawRect.left=pBulletInfo->byFrameIndex * GetWidth(pBulletInfo->byTextureIndex);
			drawRect.top=0;
			drawRect.right=drawRect.left+GetWidth(pBulletInfo->byTextureIndex);
			drawRect.bottom=drawRect.top+GetHeight(pBulletInfo->byTextureIndex);

			//绘画图片
			DrawRotateAndZoom(pBulletInfo->ptTrace[pBulletInfo->wTracePtIndex].x,pBulletInfo->ptTrace[pBulletInfo->wTracePtIndex].y,drawRect,pBulletInfo->fAngle,
				1.f,1.f,pBulletInfo->byTextureIndex);

			//切换标识
			bool bSwitchStatus=false;

			//结束判断
			if (pBulletInfo->wTracePtIndex<(pBulletInfo->wTracePtCount-1))
			{
				for (enFishKind FishKind=enFishKind_1; FishKind<enFishKind_No; FishKind=enFishKind(FishKind+1))
				{
					if (true==g_FishGroup[FishKind].ShootTest(pBulletInfo->byShootUser, pBulletInfo->ptTrace[pBulletInfo->wTracePtIndex].x,pBulletInfo->ptTrace[pBulletInfo->wTracePtIndex].y, m_enBulletKind, pBulletInfo->bCptrMode, pBulletInfo->dwBulletID, pBulletInfo->byBulletCount, bSwitchStatus))
					{
						bSwitchStatus=true;
						break;
					}					
				}
				if (true==bSwitchStatus && true==pBulletInfo->bCptrMode)
				{
					for (enFishKind FishKind=enFishKind_1; FishKind<enFishKind_No; FishKind=enFishKind(FishKind+1))
						g_FishGroup[FishKind].ShootTest(pBulletInfo->byShootUser, pBulletInfo->ptTrace[pBulletInfo->wTracePtIndex].x,pBulletInfo->ptTrace[pBulletInfo->wTracePtIndex].y, m_enBulletKind, pBulletInfo->bCptrMode, pBulletInfo->dwBulletID, pBulletInfo->byBulletCount, bSwitchStatus);
				}
			}
			else if (pBulletInfo->wTracePtIndex>=(pBulletInfo->wTracePtCount-1))
			{
				bSwitchStatus=true;
			}

			if (true==bSwitchStatus)
			{
				//设置状态
				pBulletInfo->BulletStatus=enBulletStatus_Net;

				//重置变量
				pBulletInfo->byFrameIndex=0;
				pBulletInfo->bySpaceTimes=30;

				//投递消息
				::PostMessage(m_hNotifyWnd,WM_NET_OPEN,0,0);

				//设置变量
				bShowNet=true;
			}

			//移动帧
			if(false==bShowNet && GetTickCount()>=pBulletInfo->dwMoveTimes)
			{
				//移动帧数
				int nSpaceFrame=pBulletInfo->wTracePtCount/BULLET_FRAME_COUNT;
				if (nSpaceFrame>0) pBulletInfo->byFrameIndex=pBulletInfo->wTracePtIndex/nSpaceFrame;
				if (pBulletInfo->byFrameIndex>=BULLET_FRAME_COUNT) pBulletInfo->byFrameIndex=BULLET_FRAME_COUNT-1;

				//重设时间
				pBulletInfo->dwMoveTimes=GetTickCount()+pBulletInfo->bySpaceTimes;

				//移动位置
				++pBulletInfo->wTracePtIndex;
			}
		}
		else
		{
			//设置变量
			bShowNet=true;
		}
		
		if (true==bShowNet)
		{
			//当前位置
			CRect drawRect;
			drawRect.left=(pBulletInfo->byFrameIndex % g_nNetColumnCount[m_enBulletKind]) * GetWidth(TEXTURE_INDEX_NET);
			drawRect.top=(pBulletInfo->byFrameIndex / g_nNetColumnCount[m_enBulletKind]) * GetWidth(TEXTURE_INDEX_NET);
			drawRect.right=drawRect.left+GetWidth(TEXTURE_INDEX_NET);
			drawRect.bottom=drawRect.top+GetHeight(TEXTURE_INDEX_NET);

			//绘画图片
			static const float fScaling[]={0.8f,0.9f,1.0f,1.1f,1.2f};
			static const D3DCOLOR clrBullet[]={D3DCOLOR_XRGB(255, 255, 0),D3DCOLOR_XRGB(0, 255, 0),D3DCOLOR_XRGB(0, 0, 255),D3DCOLOR_XRGB(255, 0, 255),D3DCOLOR_XRGB(255, 0, 0)};
			DrawRotateAndZoom(pBulletInfo->ptTrace[pBulletInfo->wTracePtIndex].x,pBulletInfo->ptTrace[pBulletInfo->wTracePtIndex].y,drawRect,pBulletInfo->fAngle+D3DX_PI/2,
				fScaling[pBulletInfo->byBulletCount-1],fScaling[pBulletInfo->byBulletCount-1],TEXTURE_INDEX_NET,enXCenter, enYCenter,clrBullet[pBulletInfo->byBulletCount-1]);

			//结束判断
			//if (pBulletInfo->byFrameIndex>=(g_nNetFrameCount[m_enBulletKind][pBulletInfo->byBulletCount-1]-1) && 0==pBulletInfo->byAddedFrame)
			if (pBulletInfo->byFrameIndex>=(g_nNetFrameCount[m_enBulletKind][pBulletInfo->byBulletCount-1]-1))
			{
				//设置状态
				pBulletInfo->BulletStatus=enBulletStatus_No;

				//释放资源
				m_BulletFactory.FreeItem(pBulletInfo);

				continue;
			}

			//移动帧
			if(GetTickCount()>=pBulletInfo->dwMoveTimes)
			{
				//移动帧数
				if (pBulletInfo->byFrameIndex<(g_nNetFrameCount[m_enBulletKind][pBulletInfo->byBulletCount-1]-1))
					pBulletInfo->byFrameIndex=(pBulletInfo->byFrameIndex+1)%g_nNetFrameCount[m_enBulletKind][pBulletInfo->byBulletCount-1];
				//else if (pBulletInfo->byAddedFrame>0)
					//pBulletInfo->byAddedFrame-=1;

				//重设时间
				pBulletInfo->dwMoveTimes=GetTickCount()+pBulletInfo->bySpaceTimes;
			}
		}

		//递增判断
		++nEnumIndex;

	} while (true);
}