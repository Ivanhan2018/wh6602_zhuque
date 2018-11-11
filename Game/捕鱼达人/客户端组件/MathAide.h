#ifndef MATH_AIDE_HEAD_FILE
#define MATH_AIDE_HEAD_FILE
#pragma once

//算术辅助
class CMathAide
{
	//函数定义
public:
	//构造函数
	CMathAide(void);
	//析构函数
	~CMathAide(void);

	//辅助计算
public:
	//贝塞尔点
	static void BuildBezier(int nInitX[], int nInitY[], int nInitCount, POINT ptBezier[], int nBuffertCount, WORD &nBezierCount, int nDistance);
	//建立直线
	static void BuildLinear(int nInitX[], int nInitY[], int nInitCount, POINT ptLinear[], int nBuffertCount, WORD &nLinearCount, int nDistance);
	//建立直线
	static void BuildLinear(int nInitX[], int nInitY[], int nInitCount, POINT ptLinear[], int nBuffertCount, WORD &nLinearCount, int nDistance, CRect &rcBound);
	//计算距离
	static LONG CalcDistance(int nX1, int nY1, int nX2, int nY2);
	//计算组合
	static LONG Combination(int nCount,int nR);
	//计算阶乘
	static LONG Factorial(int nNumber);
};

#endif