#ifndef COIN_HEAD_FILE
#define COIN_HEAD_FILE
#pragma once

//////////////////////////////////////////////////////////////////////////////////

#include "Stdafx.h"
#include "DXSKin.h"

//金币信息
struct tagCoinInfo
{
	//金币信息
	LONG							lCoinCount;								//金币数目
	BYTE							byTextureIndex;							//纹理索引
	BYTE							byAlphaBlend;							//透明度

	//轨迹变量
	POINT							ptNumber;								//数字坐标
	BYTE							byFrameIndex;							//帧数索引
	DWORD							dwMoveTimes;							//移动时间
	BYTE							bySpaceTimes;							//间隔时间
	POINT							ptTrace[500];							//轨迹坐标
	WORD							wTracePtCount;							//轨迹数目
	WORD							wTracePtIndex;							//轨迹索引
	float							fScaling;								//缩放大小
};

#define TEXTURE_INDEX_COIN_SHOW			0										//纹理索引
#define TEXTURE_INDEX_COIN_MOVE_1		1										//纹理索引
#define TEXTURE_INDEX_COIN_MOVE_2		2										//纹理索引
#define TEXTURE_INDEX_COIN_MOVE_3		3										//纹理索引
#define TEXTURE_INDEX_COIN_BK			4										//纹理索引

#define COIN_FRAME_COUNT				11										//帧数目

//捕获金币
class CCoin :public CDxSkin
{
	//变量定义
private:
	CFactoryTemplate<tagCoinInfo>	m_CoinInfoFactory;						//金币工厂

	//函数定义
public:
	//构造函数
	CCoin(void);
	//析构函数
	~CCoin(void);

	//绘画函数
public:
	//绘画金币
	void DrawCoin();

	//接口函数
public:
	//增加金币
	void AddCoin(int nSourcePosX, int nSourcePosY, int nTargetPosX, int nTargetPosY, int nNumberX,int nNumberY,LONG lCoinCount, BYTE byTextureIndex);

	//内部函数
private:
	//绘画数字
	void DrawNumber(int nXPos, int nYPos, int nNumber, BYTE byAlphaBlend);
};

#endif