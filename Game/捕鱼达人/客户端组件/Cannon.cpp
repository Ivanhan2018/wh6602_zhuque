#include "StdAfx.h"
#include "cannon.h"

//构造函数
CCannon::CCannon(void)
{
	//内核变量
	m_hNotifyWnd=NULL;

	//位置变量
	m_fScalingSpace=0.f;
	m_bCountScaling=false;
	m_wMeChairID=0;
	m_fCountScaling=1.f;
	m_dwScalingTimes=0L;
	ZeroMemory(m_ptBenchmarkPos,sizeof(m_ptBenchmarkPos));
	ZeroMemory(m_ptNumberPos,sizeof(m_ptNumberPos));
	for (int i=0; i<CountArray(m_fAngleCan); ++i) 
	{
		m_fAngleCan[i]=D3DX_PI/2;
		m_fAngleBk[i]=0.f;
		m_wBulletCount[i]=0;
		m_enCannonStatus[i]=enCannonStatus_Normal;
		m_dwChangeTimes[i]=0;
	}

	//状态变量
	ZeroMemory(m_nFrameIndex,sizeof(m_nFrameIndex));
	ZeroMemory(m_nFrameIndexBack,sizeof(m_nFrameIndexBack));
}

//析构函数
CCannon::~CCannon(void)
{
}

//绘画函数
void CCannon::DrawCannon()
{
	//合法判断
	if (m_Sprite==NULL||m_pTexture==NULL) return ;	

	//绘画位置
	CRect drawRect;

	//绘画大炮
	for (WORD wChairID=0; wChairID<GAME_PLAYER; ++wChairID)
	{	
		//数目判断
		if (0==m_wBulletCount[wChairID]) continue;

		//背景位置
		drawRect.left=3 * GetWidth(TEXTURE_INDEX_NUM);
		drawRect.top=0;
		drawRect.right=GetImageWidth();
		drawRect.bottom=GetImageHeight();

		//缩放倍数
		float fCountScaling=1.f;
		if (wChairID==m_wMeChairID) fCountScaling=m_fCountScaling;

		//缩放偏移
		int nExcursionX=int((fCountScaling-1.f)*(drawRect.Width()/2+45));
		int nExcursionY=int((fCountScaling-1.f)*(drawRect.Height()/2+10));

		//绘画背景
		DrawRotateAndZoom(m_ptNumberPos[wChairID].x+nExcursionX,m_ptNumberPos[wChairID].y+nExcursionY,drawRect,0.f,fCountScaling,fCountScaling,TEXTURE_INDEX_NUM);

		//绘画数目
		drawRect.left=(m_wBulletCount[wChairID]-1)%3 * GetWidth(TEXTURE_INDEX_NUM);
		drawRect.top=(m_wBulletCount[wChairID]-1)/3 * GetHeight(TEXTURE_INDEX_NUM);
		drawRect.right=drawRect.left+GetWidth(TEXTURE_INDEX_NUM);
		drawRect.bottom=drawRect.top+GetHeight(TEXTURE_INDEX_NUM);
		DrawRotateAndZoom(m_ptNumberPos[wChairID].x-53,m_ptNumberPos[wChairID].y-20,drawRect,0.f,fCountScaling,fCountScaling,TEXTURE_INDEX_NUM);

		//绘画位置
		drawRect.left=m_nFrameIndexBack[wChairID] * GetWidth(TEXTURE_INDEX_BACK);
		drawRect.top=0;
		drawRect.right=drawRect.left+GetWidth(TEXTURE_INDEX_BACK);
		drawRect.bottom=drawRect.top+GetHeight(TEXTURE_INDEX_BACK);

		//绘画背景
		DrawRotateAndZoom(m_ptBenchmarkPos[wChairID].x,m_ptBenchmarkPos[wChairID].y,drawRect,m_fAngleBk[wChairID],1.f,1.f,TEXTURE_INDEX_BACK);

		//绘画大炮
		drawRect.left=m_nFrameIndex[wChairID] * GetWidth(TEXTURE_INDEX_CANNON);
		drawRect.top=(m_wBulletCount[wChairID]-1) * GetHeight(TEXTURE_INDEX_CANNON);
		drawRect.right=drawRect.left+GetWidth(TEXTURE_INDEX_CANNON);
		drawRect.bottom=drawRect.top+GetHeight(TEXTURE_INDEX_CANNON);

		//绘画图片
		DrawRotateAndZoom(m_ptBenchmarkPos[wChairID].x,m_ptBenchmarkPos[wChairID].y-4,drawRect,m_fAngleCan[wChairID],(drawRect.right-drawRect.left)/2,41,1.f,1.f,TEXTURE_INDEX_CANNON);

		//变更索引
		int const static nSpaceTime=20;
		if (GetTickCount()>=(m_dwChangeTimes[wChairID]+nSpaceTime) && enCannonStatus_Shoot==m_enCannonStatus[wChairID])
		{
			//结束判断
			if (m_nFrameIndex[wChairID]>=CANNON_FRAME_COUNT-1)
			{
				m_enCannonStatus[wChairID]=enCannonStatus_Normal;
				m_nFrameIndex[wChairID]=0;
				m_nFrameIndexBack[wChairID]=0;
			}
			else
			{
				++m_nFrameIndex[wChairID];
			}

			//重置时间
			m_dwChangeTimes[wChairID]=GetTickCount()+nSpaceTime;
		}

		//修改倍数
		if (true==m_bCountScaling && GetTickCount()>=(m_dwScalingTimes+nSpaceTime))
		{
			//重置时间
			m_dwScalingTimes=GetTickCount()+50;

			//修改倍数
			m_fCountScaling+=m_fScalingSpace;

			//修改方向
			if (m_fCountScaling>=1.6f) m_fScalingSpace=-0.1f;

			//结束判断
			if (m_fCountScaling<=1.0f) m_bCountScaling=false;
		}
	}
}

//设置位置
void CCannon::SetBenchmarkPos(WORD wChairID, int nPosX, int nPosY)
{
	//合法判断
	ASSERT(wChairID<GAME_PLAYER);
	if (!(wChairID<GAME_PLAYER)) return;

	//设置坐标
	m_ptBenchmarkPos[wChairID].x=nPosX;
	m_ptBenchmarkPos[wChairID].y=nPosY;
}

//设置角度
void CCannon::SetAngleCan(WORD wChairID, float fAngle)
{
	//合法判断
	ASSERT(wChairID<GAME_PLAYER);
	if (!(wChairID<GAME_PLAYER)) return;

	//设置变量
	m_fAngleCan[wChairID]=fAngle;

	//限制范围
	if (IsSameSide(m_wMeChairID,wChairID))
	{
		if (m_fAngleCan[wChairID]>=D3DX_PI/2 && m_fAngleCan[wChairID]<=D3DX_PI) m_fAngleCan[wChairID]=ME_MAX_CAN_ANGLE;
		if (m_fAngleCan[wChairID]>=D3DX_PI && m_fAngleCan[wChairID]<3*D3DX_PI/2) m_fAngleCan[wChairID]=ME_MIN_CAN_ANGLE;
	}
	else
	{
		if (m_fAngleCan[wChairID]>=4*D3DX_PI/2 && m_fAngleCan[wChairID]<=5*D3DX_PI/2) m_fAngleCan[wChairID]=UP_MIN_CAN_ANGLE;
		if (m_fAngleCan[wChairID]>3*D3DX_PI/2 && m_fAngleCan[wChairID]<4*D3DX_PI/2) m_fAngleCan[wChairID]=UP_MAX_CAN_ANGLE;
	}
}

//发射炮弹
void CCannon::Shoot(WORD wChairID)
{
	//合法判断
	ASSERT(wChairID<GAME_PLAYER);
	if (!(wChairID<GAME_PLAYER)) return;

	//设置状态
	if (enCannonStatus_Normal==m_enCannonStatus[wChairID])
	{
		m_enCannonStatus[wChairID]=enCannonStatus_Shoot;
		m_nFrameIndex[wChairID]=0;
	}

	//背景索引
	m_nFrameIndexBack[wChairID]=1;

	//设置时间
	m_dwChangeTimes[wChairID]=GetTickCount();
}

//设置位置
void CCannon::SetNumberPos(WORD wChairID, int nPosX, int nPosY)
{
	//合法判断
	ASSERT(wChairID<GAME_PLAYER);
	if (!(wChairID<GAME_PLAYER)) return;

	//设置变量
	m_ptNumberPos[wChairID].x=nPosX;
	m_ptNumberPos[wChairID].y=nPosY;
}

//子弹数目
void CCannon::SetBulletCount(WORD wChairID, int nBulletCount, bool bScale) 
{
	//设置变量
	m_wBulletCount[wChairID]=nBulletCount;
	m_dwScalingTimes=GetTickCount();

	if (true==bScale)
	{
		m_bCountScaling=true;
		m_fScalingSpace=0.1f;
		m_fCountScaling=1.0f;
	}
}