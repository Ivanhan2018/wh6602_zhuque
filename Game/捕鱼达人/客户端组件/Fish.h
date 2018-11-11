//////////////////////////////////////////////////////////////////////////////////

#ifndef FISH_HEAD_FILE
#define FISH_HEAD_FILE
#pragma once

//////////////////////////////////////////////////////////////////////////////////

#include "Stdafx.h"
#include "DXSKin.h"

//鱼群状态
enum enFishStatus
{
	enFishStatus_No,				//错误状态
	enFishStatus_Alive,				//活跃状态
	enFishStatus_Die,				//死亡状态
};

#define TEXTURE_INDEX_FISH			0										//纹理索引

//移动速度
int const g_nMoveSpeed[enFishKind_No]={40,40,40,35,35,32,32,50,33,32,60,60};
int const g_nDieSpeed[enFishKind_No]={10,10,10,10,6,10,10,10,10,10,10,10};

//图片信息
int const g_nColumnCount[enFishKind_No]=	{8,8,8,8,8,8,8,6,8,8,4,4};
int const g_nRowCount[enFishKind_No]=		{3,5,4,14,6,3,5,5,5,5,5,3};
int const g_nAliveFrameCount[enFishKind_No]={14,24,20,96,17,15,21,18,15,13,12,12};
int const g_nTotalFrameCount[enFishKind_No]={22,34,32,106,45,22,33,30,37,36,20,12};
int const g_nShootRoundW[enFishKind_No]={40,56,68,56,68,88,72,80,166,180,354,368};
int const g_nShootRoundH[enFishKind_No]={10,18,24,49,30,20,26,44,60,50,60,60};

//int const g_nDrawCenterExcsX[enFishKind_No]={0,5,5,0,5,0,5,0,0,0,10};
//int const g_nDrawCenterExcsY[enFishKind_No]={0,0,5,0,0,0,0,0,0,0,-5};

//鱼类信息
struct tagFishInfo
{
	//状态信息
	bool							bActive;								//激活判断

	//鱼群信息
	enFishStatus					FishStatus;								//鱼类状态
	DWORD							dwFishID;								//鱼类标识
	BYTE							byCptrPro;								//捕获概率

	//轨迹变量
	BYTE							byFrameIndex;							//帧数索引
	DWORD							dwMoveTimes;							//移动时间
	BYTE							bySpaceTimes;							//间隔时间
	float							fAngle;									//轨迹角度
	float							fAngleStep;								//轨迹步长
	POINT							ptTrace[500];							//轨迹坐标
	WORD							wTracePtCount;							//轨迹数目
	WORD							wTracePtIndex;							//轨迹索引
	BYTE							byDieCount;								//死亡数目
	BYTE							byShootUser;							//设计玩家

	//捕获信息
	BYTE							byBulletCount;							//子弹数目	
};

//#define WM_CAPTURE_FISH				WM_USER+200								//捕获鱼群
//#define WM_HIT_FISH					WM_USER+201								//击中鱼群

////////////////////////////////////////////////////////////////////////////////
//类型声明
enum enBulletKind;

//dx图象类
class CFishGroup :public CDxSkin
{
	//内核变量
private:
	HWND							m_hNotifyWnd;							//通知窗体
	enFishKind						m_enFishKind;							//鱼群种类
	CFactoryTemplate<tagFishInfo>	m_FishFactory;							//鱼群工厂

	//静态变量
private:
	static bool						m_bWaveComing;							//涨潮来临

	//函数定义
public:
	//构造函数
	CFishGroup();
	//析构函数
	virtual ~CFishGroup();

	//静态函数
public:
	//涨潮来临
	static void SetWaveComing(bool bComing) {m_bWaveComing=bComing;}

	//功能函数
public:
	//D X 绘画
	void DrawFish();
	//击中判断
	bool ShootTest(WORD wChairID, int nPosX, int nPosY, enBulletKind BulletKind, bool bCptrMode, DWORD dwBulletID, BYTE byBulletCount, bool bNetStatus);
	//击中判断
	bool PtInFish(tagFishInfo *pFishInfo, int nCurIndex, int nPosShootX, int nPosShootY);
	//设置状态
	bool SetStatus(DWORD dwFishID, enFishStatus FishStatus);
	//获取鱼类
	tagFishInfo *GetFish(DWORD dwFishID);
	//激活鱼类
	tagFishInfo *ActiveNewFish();
	//枚举鱼类
	tagFishInfo *EnumFish(WORD wIndex);
	//获取数目
	WORD GetFishCount();
	//释放鱼群
	void FreeFish(tagFishInfo *pFishInfo);
		
	//配置函数
public:
	//初始鱼类
	bool InitTrace(int nInitX[], int nInitY[], int nInitCount, DWORD dwFishID);
	//初始鱼类
	bool InitTraceLinear(int nInitX[], int nInitY[], int nInitCount, DWORD dwFishID);
	//鱼群信息
	void SetGroupInfo(HWND hNotifyWnd, enFishKind FishKind) {m_hNotifyWnd=hNotifyWnd; m_enFishKind=FishKind;}
	//重置鱼群
	void ResetKenel();	
};

//鱼群数组
extern CFishGroup						g_FishGroup[enFishKind_No];

////////////////////////////////////////////////////////////////////////////////
#endif