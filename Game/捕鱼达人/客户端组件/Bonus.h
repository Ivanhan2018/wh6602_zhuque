#ifndef BONUS_HEAD_FILE
#define BONUS_HEAD_FILE
#pragma once

//////////////////////////////////////////////////////////////////////////////////

#include "Stdafx.h"
#include "DXSKin.h"

//奖品信息
struct tagBonusInfo
{
	//奖品信息
	LONG							lBonusCount;							//金币数目
	INT								nBenchmarkPosX;							//基准位置
	INT								nBenchmarkPosY;							//基准位置
	BYTE							byBackAlpha;							//背景透明
	BYTE							byScalingIndex;							//金币倍数
	BYTE							byStarIndex;							//星星索引
	BYTE							byShowTimeCount;						//显示时间
};

#define TEXTURE_INDEX_BONUS_BACK		0										//纹理索引
#define TEXTURE_INDEX_BONUS_STAR		1										//纹理索引
#define TEXTURE_INDEX_BONUS_DETAIL		2										//纹理索引
#define TEXTURE_INDEX_BONUS_NUMBER		3										//纹理索引
#define TEXTURE_INDEX_BONUS_LIGHT		4										//纹理索引

//捕获奖品
class CBonus :public CDxSkin
{
	//变量定义
private:
	DWORD							m_dwDrawTime;
	DWORD							m_dwStartTime;
	CFactoryTemplate<tagBonusInfo>	m_BonusInfoFactory;						//奖品工厂

	//函数定义
public:
	//构造函数
	CBonus(void);
	//析构函数
	~CBonus(void);

	//绘画函数
public:
	//绘画金币
	void DrawBonus();

	//接口函数
public:
	//增加金币
	void AddBonus(LONG lBonusCount, INT nBenchmarkPosX, INT nBenchmarkPosY);

	//内部函数
private:
	//绘画数字
	void DrawNumber(int nXPos, int nYPos, int nNumber, BYTE byAlphaBlend);
};

#endif