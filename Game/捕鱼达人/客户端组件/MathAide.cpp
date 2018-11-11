#include "StdAfx.h"
#include "Mathaide.h"
#include "math.h"

//构造函数
CMathAide::CMathAide(void)
{
}

//析构函数
CMathAide::~CMathAide(void)
{
}


//计算阶乘
LONG CMathAide::Factorial(int nNumber)
{
	int nFactorial=1;
	int nTemp=nNumber;
	for(int nIndex=0; nIndex<nNumber; nIndex++)
	{
		nFactorial*=nTemp;
		nTemp--;
	}
	return nFactorial;
}

//计算组合
LONG CMathAide::Combination(int nCount,int nR)
{
	LONG lResult;
	lResult=Factorial(nCount)/((Factorial(nR))*Factorial(nCount-nR));

	return lResult;
}

//计算距离
LONG CMathAide::CalcDistance(int nX1, int nY1, int nX2, int nY2)
{
	LONG lSqure=(nX1-nX2)*(nX1-nX2)+(nY1-nY2)*(nY1-nY2);
	float fTmpValue=sqrt(float(lSqure));

	return LONG(fTmpValue);
}

//建立直线
void CMathAide::BuildLinear(int nInitX[], int nInitY[], int nInitCount, POINT ptLinear[], int nBuffertCount, WORD &nLinearCount, int nDistance, CRect &rcBound)
{
	//设置变量
	nLinearCount=0;

	//合法判断
	if (nInitCount<2) return ;
	if (nDistance<=0) return ;

	//计算角度
	LONG lDisTotal=CalcDistance(nInitX[nInitCount-1],nInitY[nInitCount-1],nInitX[0],nInitY[0]);
	if (lDisTotal<=0) return;
	float fCos=(labs(nInitY[nInitCount-1]-nInitY[0])*1.f)/lDisTotal;
	float fAngle=float(acos(double(fCos)));

	//计算坐标
	nLinearCount=1;
	ptLinear[0].x=nInitX[0];
	ptLinear[0].y=nInitY[0];
	LONG lDis=0;
	float fTmpValue=0.f;
	while (lDis<lDisTotal && nLinearCount<nBuffertCount)
	{
		if (nInitX[nInitCount-1]<nInitX[0])
			fTmpValue=ptLinear[0].x-sin(fAngle)*(nDistance*nLinearCount);
		else
			fTmpValue=ptLinear[0].x+sin(fAngle)*(nDistance*nLinearCount);

		if (LONG(fTmpValue)>=rcBound.right || LONG(fTmpValue)<=rcBound.left) break;
		ptLinear[nLinearCount].x=LONG(fTmpValue);

		if (nInitY[nInitCount-1]<nInitY[0])
			fTmpValue=ptLinear[0].y-cos(fAngle)*(nDistance*nLinearCount);
		else
			fTmpValue=ptLinear[0].y+cos(fAngle)*(nDistance*nLinearCount);

		if (LONG(fTmpValue)>=rcBound.bottom || LONG(fTmpValue)<=rcBound.top) break;
		ptLinear[nLinearCount].y=LONG(fTmpValue);

		++nLinearCount;
		lDis=CalcDistance(ptLinear[nLinearCount-1].x,ptLinear[nLinearCount-1].y,ptLinear[0].x,ptLinear[0].y);
	}
}

//建立直线
void CMathAide::BuildLinear(int nInitX[], int nInitY[], int nInitCount, POINT ptLinear[], int nBuffertCount, WORD &nLinearCount, int nDistance)
{
	//设置变量
	nLinearCount=0;

	//合法判断
	if (nInitCount<2) return ;
	if (nDistance<=0) return ;

	//计算角度
	LONG lDisTotal=CalcDistance(nInitX[nInitCount-1],nInitY[nInitCount-1],nInitX[0],nInitY[0]);
	if (lDisTotal<=0) return;
	float fCos=(labs(nInitY[nInitCount-1]-nInitY[0])*1.f)/lDisTotal;
	float fAngle=float(acos(double(fCos)));

	//计算坐标
	nLinearCount=1;
	ptLinear[0].x=nInitX[0];
	ptLinear[0].y=nInitY[0];
	LONG lDis=0;
	float fTmpValue=0.f;
	while (lDis<lDisTotal && nLinearCount<nBuffertCount)
	{
		if (nInitX[nInitCount-1]<nInitX[0])
			fTmpValue=ptLinear[0].x-sin(fAngle)*(nDistance*nLinearCount);
		else
			fTmpValue=ptLinear[0].x+sin(fAngle)*(nDistance*nLinearCount);
		ptLinear[nLinearCount].x=LONG(fTmpValue);

		if (nInitY[nInitCount-1]<nInitY[0])
			fTmpValue=ptLinear[0].y-cos(fAngle)*(nDistance*nLinearCount);
		else
			fTmpValue=ptLinear[0].y+cos(fAngle)*(nDistance*nLinearCount);
		ptLinear[nLinearCount].y=LONG(fTmpValue);

		++nLinearCount;
		lDis=CalcDistance(ptLinear[nLinearCount-1].x,ptLinear[nLinearCount-1].y,ptLinear[0].x,ptLinear[0].y);
	}

	/*while (lDis<lDisTotal && nLinearCount<nBuffertCount)
	{
		if (nInitX[nInitCount-1]<nInitX[0])
			fTmpValue=ptLinear[nLinearCount-1].x-sin(fAngle)*nDistance;            
		else
			fTmpValue=ptLinear[nLinearCount-1].x+sin(fAngle)*nDistance;
		ptLinear[nLinearCount].x=LONG(fTmpValue);

		if (nInitY[nInitCount-1]<nInitY[0])
			fTmpValue=ptLinear[nLinearCount-1].y-cos(fAngle)*nDistance;
		else
			fTmpValue=ptLinear[nLinearCount-1].y+cos(fAngle)*nDistance;
		ptLinear[nLinearCount].y=LONG(fTmpValue);
		++nLinearCount;
		lDis=CalcDistance(ptLinear[nLinearCount-1].x,ptLinear[nLinearCount-1].y,ptLinear[0].x,ptLinear[0].y);
	}*/
}

//贝塞尔点
void CMathAide::BuildBezier(int nInitX[], int nInitY[], int nInitCount, POINT ptBezier[], int nBuffertCount, WORD &nBezierCount, int nDistance)
{
	//初始变量
	nBezierCount=0;

	//变量定义
	int i=0;
	double dTempX=0.0;
	double dTempY=0.0;
	double t=0.0;
	int n=nInitCount-1;

	//生成贝塞尔
	while(t<=1)
	{
		dTempX=0.0;
		dTempY=0.0;
		i=0;
		while(i<=n)
		{
			dTempX+=(nInitX[i]*pow(t,i)*pow((1-t),(n-i))*Combination(n,i));
			dTempY+=(nInitY[i]*pow(t,i)*pow((1-t),(n-i))*Combination(n,i));
			i++;
		}

		//合法判断
		if (nBezierCount>=nBuffertCount) return;

		//判断距离
		int nPtSpace=0;
		if (nBezierCount>0) nPtSpace=CalcDistance(ptBezier[nBezierCount-1].x, ptBezier[nBezierCount-1].y, int(dTempX), int(dTempY));
		if (nPtSpace>=nDistance || 0==nBezierCount)
		{
			ptBezier[nBezierCount].x=(long)dTempX;
			ptBezier[nBezierCount++].y=(long)dTempY;
		}
		t+=0.00001;
	}
	return ;
}