#ifndef SEA_SCENE_HEAD_FILE
#define SEA_SCENE_HEAD_FILE

#pragma once
#include "stdafx.h"
#include "dxskin.h"

//生物信息
struct tagBiologyInfo
{
	int								nPosX;									//生物位置
	int								nPosY;									//生物位置
	bool							bAsc;									//升序标识
	float							fReAngle;								//翻转角度
	float							fScaling;								//缩放比例
	BYTE							byFrameIndex;							//动画索引
	BYTE							byFrameCount;							//动画帧数
	BYTE							byTextureIndex;							//纹理索引
	BYTE							byColumnCount;							//纹理列数
	DWORD							dwMoveTimes;							//移动时间
	BYTE							bySpaceTimes;							//间隔时间
};

//气泡信息
struct tagBubble
{
	int								nPosX;									//气泡位置
	int								nPosY;									//气泡位置
	int								nMoveSpaceX;							//气泡位置
	int								nMoveSpaceY;							//气泡位置
	float							fScaling;								//缩放比例
	BYTE							byAlphaBlend;							//透明度
	DWORD							dwMoveTimes;							//移动时间
};

//海底场景
class CSeaScene :public CDxSkin
{
	//内核变量
private:
	bool								m_bTride;							//涨潮标识
	bool								m_bReverse;							//翻转标识
	enFishKind							m_enFishKind;						//场景种类
	CFactoryTemplate<tagBiologyInfo>	m_BiologyFactory;					//生物工厂
	tagBubble							m_Bubble[8];						//气泡信息

	//函数定义
public:
	//构造函数
	CSeaScene();
	//析构函数
	virtual ~CSeaScene();

	//接口函数
public:
	//涨潮标识
	void SetTride(bool bTride) {m_bTride=bTride;}
	//绘画函数
	void DrawScene();
	//设置翻转
	void SetReverse(bool bReverse) {m_bReverse=bReverse;}
	//获取翻转
	bool IsReverse() {return m_bReverse;}
	//增加生物
	bool AddBiology(int nPosX, int nPosY, BYTE byFrameCount, BYTE byTextureIndex, BYTE byColumnCount, float fReAngle=0.f, BYTE bySpaceTimes=50, float fScaling=1.f);
	//重置场景
	void ResetScene() {m_BiologyFactory.FreeAllItem();}

	//内部函数
private:
	//产生气泡
	void MakeBubble(tagBubble &Bubble);
};					

#endif
