#ifndef CANNON_HEAD_FILE
#define CANNON_HEAD_FILE
#pragma once
#include "stdafx.h"
#include "dxskin.h"

#define TEXTURE_INDEX_BACK			0										//纹理索引
#define TEXTURE_INDEX_CANNON		1										//纹理索引
#define TEXTURE_INDEX_NUM			2										//纹理索引

#define CANNON_FRAME_COUNT			5										//炮台帧数

#define MAX_BULLET_COUNT			5										//子弹数目

#define BULLET_NUMBER_WIDTH			20										//数目宽度
#define BULLET_NUMBER_HEIGHT		21										//数目高度

//炮台状态
enum enCannonStatus
{
	enCannonStatus_No,				//错误状态
	enCannonStatus_Normal,			//正常状态
	enCannonStatus_Shoot			//发射状态
};

//捕鱼炮弹
class CCannon :public CDxSkin
{
	//内核变量
private:
	HWND							m_hNotifyWnd;							//通知窗体

	//位置变量
private:
	POINT							m_ptBenchmarkPos[GAME_PLAYER];			//基准位置
	POINT							m_ptNumberPos[GAME_PLAYER];				//数字位置
	float							m_fAngleCan[GAME_PLAYER];				//大炮角度
	float							m_fAngleBk[GAME_PLAYER];				//背景角度
	WORD							m_wMeChairID;							//自己座位
	float							m_fCountScaling;						//数目倍数

	//状态变量
private:
	DWORD							m_dwChangeTimes[GAME_PLAYER];			//改变时间
	DWORD							m_dwScalingTimes;						//改变时间
	bool							m_bCountScaling;						//缩放标识
	float							m_fScalingSpace;						//缩放增量
	int								m_wBulletCount[GAME_PLAYER];			//子弹数目
	int								m_nFrameIndex[GAME_PLAYER];				//绘画索引
	int								m_nFrameIndexBack[GAME_PLAYER];			//绘画索引
	enCannonStatus					m_enCannonStatus[GAME_PLAYER];			//炮台状态

	//函数定义
public:
	//构造函数
	CCannon(void);
	//析构函数
	~CCannon(void);

	//接口函数
public:
	//绘画函数
	void DrawCannon();
	//设置窗体
	void SetNotifyWnd(HWND hNotifyWnd) {m_hNotifyWnd=hNotifyWnd; }
	//发射炮弹
	void Shoot(WORD wChairID);
	//子弹数目
	void SetBulletCount(WORD wChairID, int nBulletCount, bool bScale);
	//子弹数目
	int GetBulletCount(WORD wChairID) { return m_wBulletCount[wChairID];}
	//设置座位
	void SetMeChairID(WORD wChairID) {m_wMeChairID=wChairID;}

	//位置函数
public:
	//设置位置
	void SetNumberPos(WORD wChairID, int nPosX, int nPosY);
	//设置位置
	void SetBenchmarkPos(WORD wChairID, int nPosX, int nPosY);
	//获取位置
	POINT &GetBenchmarkPos(WORD wChairID) {return m_ptBenchmarkPos[wChairID];}
	//设置角度
	void SetAngleCan(WORD wChairID, float fAngle);
	//获取角度
	float GetAngleCan(WORD wChairID) {return m_fAngleCan[wChairID];}
	//设置角度
	void SetAngleBk(WORD wChairID, float fAngle) {m_fAngleBk[wChairID]=fAngle;}
	//获取角度
	float GetAngleBk(WORD wChairID) {return m_fAngleBk[wChairID];}
};
#endif