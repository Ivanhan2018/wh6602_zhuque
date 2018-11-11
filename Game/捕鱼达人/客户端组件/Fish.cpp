#include "StdAfx.h"
#include "Bullet.h"
#include "Fish.h"
#include "Mathaide.h"
//////////////////////////////////////////////////////////////////////////////////////
//鱼群数组
CFishGroup						g_FishGroup[enFishKind_No];

//静态变量
bool CFishGroup::m_bWaveComing=false;
//////////////////////////////////////////////////////////////////////////////////////

//构造函数
CFishGroup::CFishGroup()
{
	//内核变量
	m_hNotifyWnd=NULL;

	//鱼群信息
	m_enFishKind=enFishKind_No;
}

//析构函数
CFishGroup::~CFishGroup()
{
	//释放内存
	m_FishFactory.FreeExtra();
}

//D X 绘画
void CFishGroup::DrawFish()
{
////////////////////////////////////////////////////
//#ifdef _DEBUG
//	//当前位置
//	CRect drawRect2;
//	drawRect2.left=0;
//	drawRect2.top=0;
//	drawRect2.right=GetWidth();
//	drawRect2.bottom=GetHeight();
//	static DWORD dwMoveTimes=GetTickCount();
//	static float fAngle=0.f;
//	//if (GetTickCount()>=dwMoveTimes+10)
//	//{
//	//	dwMoveTimes=GetTickCount();
//	//	fAngle+=0.01f;
//	//}
//
//	//DrawRotateAndZoom(300,250,drawRect2,fAngle,drawRect2.Width()/2+g_nDrawCenterExcsX[m_enFishKind],drawRect2.Height()/2+g_nDrawCenterExcsY[m_enFishKind]);
//
//	drawRect2.left=0;
//	drawRect2.top=0;
//	drawRect2.right=GetWidth(9);
//	drawRect2.bottom=GetHeight(9);
//	DrawRotateAndZoom(33,33,drawRect2,0.f,1.f,1.f,9,enXRight,enYBottom);
//#endif
//	/////////////////////////////////////////////////////
	//合法判断
	if (m_Sprite==NULL||m_pTexture==NULL) return ;
	if (enFishKind_No==m_enFishKind) return;
	if (0==m_FishFactory.GetActiveCount()) return;

	//绘画鱼群
	INT_PTR nEnumIndex=0;
	tagFishInfo *pFish=NULL;
	do
	{
		//枚举鱼群
		pFish=m_FishFactory.EnumActiveObject(nEnumIndex);
		if (NULL==pFish) break;

		//激活判断
		if (false==pFish->bActive)
		{
			++nEnumIndex;

			continue;
		}

		//当前位置
		CRect drawRect;
		drawRect.left=(pFish->byFrameIndex % g_nColumnCount[m_enFishKind]) * GetWidth();
		drawRect.top=(pFish->byFrameIndex / g_nColumnCount[m_enFishKind]) * GetHeight();
		drawRect.right=drawRect.left+GetWidth();
		drawRect.bottom=drawRect.top+GetHeight();

		//绘画图片
		if (enFishStatus_Die==pFish->FishStatus && enFishKind_12==m_enFishKind)
            DrawRotateAndZoom(pFish->ptTrace[pFish->wTracePtIndex].x,pFish->ptTrace[pFish->wTracePtIndex].y,drawRect,pFish->fAngle,1.f,1.f,1);
		else
			DrawRotateAndZoom(pFish->ptTrace[pFish->wTracePtIndex].x,pFish->ptTrace[pFish->wTracePtIndex].y,drawRect,pFish->fAngle);

		//完成判断
		bool bMoveNext=true;
		if (pFish->wTracePtIndex>=pFish->wTracePtCount-1) 
		{
			//设置状态
			pFish->FishStatus=enFishStatus_No;

			//回收资源
			m_FishFactory.FreeItem(pFish);

			//设置标识
			bMoveNext=false;
		}

		//移动帧
		if(GetTickCount()>=pFish->dwMoveTimes)
		{
			//移动帧数
			int nFrameCount=g_nAliveFrameCount[m_enFishKind];
			if (enFishStatus_Die==pFish->FishStatus) nFrameCount=g_nTotalFrameCount[m_enFishKind];

			//重复次数
			BYTE byRepeatCount=3;
			if (g_nTotalFrameCount[m_enFishKind]-g_nAliveFrameCount[m_enFishKind]>=20) byRepeatCount=1;
			else if (g_nTotalFrameCount[m_enFishKind]-g_nAliveFrameCount[m_enFishKind]<=8) byRepeatCount=4;

			//死亡判断
			if (enFishStatus_Die!=pFish->FishStatus) 
			{
				//递增索引
				++pFish->wTracePtIndex;

				//改变角度
				if (pFish->fAngle>=pFish->fAngleStep) pFish->fAngle-=pFish->fAngleStep;
			}
			else if (pFish->byFrameIndex==(nFrameCount-1) && enFishStatus_Die==pFish->FishStatus && pFish->byDieCount<byRepeatCount)
			{
				pFish->byFrameIndex=g_nAliveFrameCount[m_enFishKind];
				++pFish->byDieCount;
			}
			else if (pFish->byFrameIndex==(nFrameCount-1) && pFish->byDieCount>=byRepeatCount)
			{
				////通知消息
				//if (pFish->wTracePtIndex<pFish->wTracePtCount)
				//	PostMessage(m_hNotifyWnd, WM_FISH_DISAPPEAR, MAKEWPARAM(pFish->byShootUser,m_enFishKind),MAKELPARAM(pFish->ptTrace[pFish->wTracePtIndex].x,pFish->ptTrace[pFish->wTracePtIndex].y));

				//设置状态
				pFish->FishStatus=enFishStatus_No;

				//回收资源
				m_FishFactory.FreeItem(pFish);

				//设置标识
				bMoveNext=false;
			}

			//重设时间
			pFish->dwMoveTimes=m_bWaveComing?(GetTickCount()):(GetTickCount()+pFish->bySpaceTimes);

			//移动帧
			pFish->byFrameIndex=(pFish->byFrameIndex+1)%nFrameCount;			
		}

		//移动判断
		if (true==bMoveNext) ++nEnumIndex;

	} while (true);

	return ;
}

//初始鱼类
bool CFishGroup::InitTrace(int nInitX[], int nInitY[], int nInitCount, DWORD dwFishID)
{
	//获取鱼类
	tagFishInfo *pFishInfo=m_FishFactory.ActiveItem();
	if (NULL==pFishInfo) return false;

	//初始变量
	pFishInfo->wTracePtIndex=0;
	pFishInfo->byFrameIndex=0;
	pFishInfo->FishStatus=enFishStatus_Alive;
	pFishInfo->dwFishID=dwFishID;
	pFishInfo->dwMoveTimes=GetTickCount();

	//设置变量
	pFishInfo->bySpaceTimes=g_nMoveSpeed[m_enFishKind];

	//计算轨迹
	CMathAide::BuildBezier(nInitX, nInitY, nInitCount, pFishInfo->ptTrace, CountArray(pFishInfo->ptTrace), pFishInfo->wTracePtCount, TRACE_POINT_SPACE_NORMAL);

	//合法判断
	if (pFishInfo->wTracePtCount<2) 
	{
		//释放资源
		m_FishFactory.FreeItem(pFishInfo);

		return false;
	}

	//首个角度
	LONG lDistance=CMathAide::CalcDistance(pFishInfo->ptTrace[1].x,pFishInfo->ptTrace[1].y,pFishInfo->ptTrace[0].x,pFishInfo->ptTrace[0].y);
	float fSinValue=(pFishInfo->ptTrace[1].x-pFishInfo->ptTrace[0].x)/(lDistance*1.0f);
	pFishInfo->fAngle=acos(fSinValue);

	//中间角度
	lDistance=CMathAide::CalcDistance(pFishInfo->ptTrace[pFishInfo->wTracePtCount/2].x,pFishInfo->ptTrace[pFishInfo->wTracePtCount/2].y,pFishInfo->ptTrace[pFishInfo->wTracePtCount/2-1].x,pFishInfo->ptTrace[pFishInfo->wTracePtCount/2-1].y);
	fSinValue=(pFishInfo->ptTrace[pFishInfo->wTracePtCount/2].x-pFishInfo->ptTrace[pFishInfo->wTracePtCount/2-1].x)/(lDistance*1.0f);
	float fAngleMiddle=acos(fSinValue);

	//间隔角度
	pFishInfo->fAngleStep=(pFishInfo->fAngle-fAngleMiddle)/(pFishInfo->wTracePtCount/2);

	//调整角度
	if (pFishInfo->ptTrace[pFishInfo->wTracePtCount/2-1].y<pFishInfo->ptTrace[0].y) 
	{
		pFishInfo->fAngle=2*D3DX_PI-pFishInfo->fAngle;
		fAngleMiddle=2*D3DX_PI-fAngleMiddle;
		pFishInfo->fAngleStep=(pFishInfo->fAngle-fAngleMiddle)/(pFishInfo->wTracePtCount/2);
	}

	//调整角度(为了在到了中间角度的时候不需要重新判断切换角度)
	pFishInfo->fAngle+=(2*D3DX_PI);

	return true;
}


//初始鱼类
bool CFishGroup::InitTraceLinear(int nInitX[], int nInitY[], int nInitCount, DWORD dwFishID)
{
	//获取鱼类
	tagFishInfo *pFishInfo=m_FishFactory.ActiveItem();
	if (NULL==pFishInfo) return false;

	//初始变量
	pFishInfo->wTracePtIndex=0;
	pFishInfo->byFrameIndex=0;
	pFishInfo->FishStatus=enFishStatus_Alive;
	pFishInfo->dwFishID=dwFishID;
	pFishInfo->dwMoveTimes=GetTickCount();

	//设置变量
	pFishInfo->bySpaceTimes=g_nMoveSpeed[m_enFishKind];

	///////////////////////////////////////////////////////
	////寻找鱼类
	//tagFishInfo *pFishInfoTmp=m_FishFactory.EnumActiveObject(0);
	//if (NULL!=pFishInfoTmp && m_FishFactory.GetActiveCount()>1) 
	//{
	//	CopyMemory(pFishInfo->ptTrace,pFishInfoTmp->ptTrace,sizeof(pFishInfoTmp->ptTrace));
	//	pFishInfo->wTracePtCount=pFishInfoTmp->wTracePtCount;
	//}
	//else
	//////////////////////////////////////////////////////

	//计算轨迹
	CMathAide::BuildBezier(nInitX, nInitY, nInitCount, pFishInfo->ptTrace, CountArray(pFishInfo->ptTrace), pFishInfo->wTracePtCount, TRACE_POINT_SPACE_NORMAL);

	//合法判断
	if (pFishInfo->wTracePtCount<2) 
	{
		//释放资源
		m_FishFactory.FreeItem(pFishInfo);

		return false;
	}

	//首个角度
	LONG lDistance=CMathAide::CalcDistance(pFishInfo->ptTrace[pFishInfo->wTracePtCount-1].x,pFishInfo->ptTrace[pFishInfo->wTracePtCount-1].y,pFishInfo->ptTrace[0].x,pFishInfo->ptTrace[0].y);
	float fSinValue=(pFishInfo->ptTrace[pFishInfo->wTracePtCount-1].x-pFishInfo->ptTrace[0].x)/(lDistance*1.0f);
	pFishInfo->fAngle=acos(fSinValue);

	//调整角度
	if (pFishInfo->ptTrace[pFishInfo->wTracePtCount/2-1].y<pFishInfo->ptTrace[0].y) 
	{
		pFishInfo->fAngle=2*D3DX_PI-pFishInfo->fAngle;		
	}
	pFishInfo->fAngleStep=0.f;

	return true;
}

//设置状态
bool CFishGroup::SetStatus(DWORD dwFishID, enFishStatus FishStatus)
{
	//获取鱼类
	tagFishInfo *pFishInfo=GetFish(dwFishID);
	if (NULL==pFishInfo) return false;

	//设置变量
	pFishInfo->FishStatus=FishStatus;
	pFishInfo->bySpaceTimes=20;

	//设置索引
	if (enFishKind_12!=m_enFishKind)
        pFishInfo->byFrameIndex=g_nAliveFrameCount[m_enFishKind];
	else
		pFishInfo->byFrameIndex=0;

	return true;
}

//重置鱼群
void CFishGroup::ResetKenel()
{
	//重置内存
	m_FishFactory.FreeAllItem();
}

//获取鱼类
tagFishInfo *CFishGroup::GetFish(DWORD dwFishID)
{
	//寻找鱼类
	INT_PTR nEnumIndex=0;
	tagFishInfo *pFishInfo=NULL;
	do
	{
		//枚举鱼群
		pFishInfo=m_FishFactory.EnumActiveObject(nEnumIndex++);
		if (NULL==pFishInfo) break;
		if (pFishInfo->dwFishID==dwFishID) return pFishInfo;

	} while (true);

	return NULL;
}

//激活鱼类
tagFishInfo *CFishGroup::ActiveNewFish()
{
	tagFishInfo *pFishInfo=m_FishFactory.ActiveItem();
	ZeroMemory(pFishInfo,sizeof(*pFishInfo));
	pFishInfo->bActive=false;

	return pFishInfo;
}

//击中判断
bool CFishGroup::ShootTest(WORD wChairID, int nPosX, int nPosY, enBulletKind BulletKind, bool bCptrMode, DWORD dwBulletID, BYTE byBulletCount, bool bNetStatus)
{
	//合法判断
	ASSERT(byBulletCount<=5);
	if (!(byBulletCount<=5)) return false;

	//寻找鱼类
	INT_PTR nEnumIndex=0;
	tagFishInfo *pFishInfo=NULL;
	CRect rcBound;
	POINT ptShoot;
	ptShoot.x=nPosX;
	ptShoot.y=nPosY;
	bool bShoot=false;
	CRect rcAnd;
	do
	{
		//枚举鱼群
		pFishInfo=m_FishFactory.EnumActiveObject(nEnumIndex++);

		//合法判断
		if (NULL==pFishInfo) break;
		if (enFishStatus_Alive!=pFishInfo->FishStatus && true==bCptrMode) continue;
		if (pFishInfo->wTracePtIndex>=pFishInfo->wTracePtCount) continue;

		//设置范围
		int const nBound=g_nShootRoundH[m_enFishKind]/2;
		int nMoveSpace=m_enFishKind>=enFishKind_11?TRACE_POINT_SPACE_BIG:TRACE_POINT_SPACE_NORMAL;
		int nStepCount=g_nShootRoundW[m_enFishKind]/2/nMoveSpace;
		int nBeginIndex=max(0,(int(pFishInfo->wTracePtIndex)-nStepCount));
		int nEndIndex=min(pFishInfo->wTracePtCount,(pFishInfo->wTracePtIndex+nStepCount));

		//击中判断
		int nCenterX=0;
		int nCenterY=0;
		for (int nIndex=nBeginIndex; nIndex<nEndIndex; ++nIndex)
		{
			nCenterX=pFishInfo->ptTrace[nIndex].x;
			nCenterY=pFishInfo->ptTrace[nIndex].y;
			rcBound.SetRect(nCenterX-nBound,nCenterY-nBound,nCenterX+nBound,nCenterY+nBound);
			if (rcBound.PtInRect(ptShoot)) 
			{
				//设置标识
				bShoot=true;

				//中断判断
				break;
			}
		}

		//中断判断
		if (true==bShoot) break;

	} while (true);

	if (true==bCptrMode && (true==bShoot || true==bNetStatus))
	{
		//设置变量
		nEnumIndex=0;
		CRect rcShoot(nPosX-g_nNetRadiusX[BulletKind],nPosY-g_nNetRadiusY[BulletKind],nPosX+g_nNetRadiusX[BulletKind],nPosY+g_nNetRadiusY[BulletKind]);

		do
		{
			//枚举鱼群
			pFishInfo=m_FishFactory.EnumActiveObject(nEnumIndex++);

			//合法判断
			if (NULL==pFishInfo) break;
			if (enFishStatus_Alive!=pFishInfo->FishStatus) continue;
			if (pFishInfo->wTracePtIndex>=pFishInfo->wTracePtCount) continue;

			//击中判断
			ptShoot.x=pFishInfo->ptTrace[pFishInfo->wTracePtIndex].x;
			ptShoot.y=pFishInfo->ptTrace[pFishInfo->wTracePtIndex].y;
			if (rcShoot.PtInRect(ptShoot)) 
			{
				////概率捕获
				//BYTE byRandPrecent=rand()%200+1;
				//if (byRandPrecent<=m_byCptrProbability[byBulletCount-1][m_enFishKind])
				//{
				//	//设置状态
				//	SetStatus(pFishInfo->dwFishID,enFishStatus_Die);

				//	//子弹数目
				//	pFishInfo->byBulletCount=byBulletCount;

				//	//设计玩家
				//	pFishInfo->byShootUser=BYTE(wChairID);

				//	//发送消息
				//	AfxGetMainWnd()->PostMessage(WM_CAPTURE_FISH,pFishInfo->dwFishID,dwBulletID);
				//}

				//发送消息
				AfxGetMainWnd()->PostMessage(WM_HIT_FISH,pFishInfo->dwFishID,dwBulletID);
			}

		} while (true);
	}

	return bShoot;
}

//枚举鱼类
tagFishInfo *CFishGroup::EnumFish(WORD wIndex)
{
	return m_FishFactory.EnumActiveObject(wIndex);
}

//释放鱼群
void CFishGroup::FreeFish(tagFishInfo *pFishInfo)
{
	if (NULL!=GetFish(pFishInfo->dwFishID)) m_FishFactory.FreeItem(pFishInfo);
}

//获取数目
WORD CFishGroup::GetFishCount()
{
	//统计数目
	WORD wFishActiveCount=0;
	INT_PTR nEnumIndex=0;
	tagFishInfo *pFishInfo=NULL;
	do
	{ 
		//枚举鱼群
		pFishInfo=m_FishFactory.EnumActiveObject(nEnumIndex++);
		if (NULL==pFishInfo) break;
		if (false==pFishInfo->bActive) continue;

		//递增数目
		++wFishActiveCount;

	} while (true);

	return wFishActiveCount;
}

//击中判断
bool CFishGroup::PtInFish(tagFishInfo *pFishInfo, int nCurIndex, int nPosShootX, int nPosShootY)
{
	////合法判断
	//ASSERT(NULL!=pFishInfo && nCurIndex<pFishInfo->wTracePtCount);
	//if (!(NULL!=pFishInfo && nCurIndex<pFishInfo->wTracePtCount)) return false;

	////长度判断
	//WORD wLen

	return true;
}