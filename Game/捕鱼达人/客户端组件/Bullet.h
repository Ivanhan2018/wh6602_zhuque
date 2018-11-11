#ifndef BULLET_HEAD_FILE
#define BULLET_HEAD_FILE
#pragma once
#include "dxskin.h"
#include "Fish.h"

//子弹状态
enum enBulletStatus
{
	enBulletStatus_No,				//错误状态
	enBulletStatus_Shoot,			//设计状态
	enBulletStatus_Net,				//撒网状态
};

//子弹种类
enum enBulletKind
{
	enBulletKind_1,					//子弹种类
	enBulletKind_2,					//子弹种类
	enBulletKind_3,					//子弹种类
	enBulletKind_4,					//子弹种类
	enBulletKind_5,					//子弹种类
	enBulletKind_No					//错误种类
};

//子弹帧数
int const g_nShootColumnCount[enBulletKind_No]={1,1,1,1,1};
int const g_nShootRowCount[enBulletKind_No]={1,1,1,1,1};
int const g_nShootFrameCount[enBulletKind_No]={1,1,1,1,1};
int const g_nNetColumnCount[enBulletKind_No]={8,8,8,8,8};
int const g_nNetRowCount[enBulletKind_No]={2,2,2,2,2};
//int const g_nNetFrameCount[enBulletKind_No][5]={{6,7,8,9,16},{6,7,8,9,16},{6,7,8,9,16},{6,7,8,9,16},{6,7,8,9,16}};
int const g_nNetFrameCount[enBulletKind_No][5]={{16,16,16,16,16},{16,16,16,16,16},{16,16,16,16,16},{16,16,16,16,16},{16,16,16,16,16}};

int const g_nNetRadiusX[enBulletKind_No]={55,60,65,70,75};
int const g_nNetRadiusY[enBulletKind_No]={55,60,65,70,75};

#define MAX_CPTR_FISH_COUNT			15										//捕获数目

#define TEXTURE_INDEX_SHOOT_1		0										//纹理索引
#define TEXTURE_INDEX_SHOOT_2		1										//纹理索引
#define TEXTURE_INDEX_SHOOT_3		2										//纹理索引
#define TEXTURE_INDEX_NET			3										//纹理索引

#define BULLET_SPEED_NORMAL			40										//子弹速度
#define BULLET_SPEED_ACCELERATE		30										//子弹速度

#define SHOOT_ROUND_SHRINK_X		90										//子弹速度
#define SHOOT_ROUND_SHRINK_Y		70										//子弹速度

#define BULLET_FRAME_COUNT			10										//子弹帧数

//#define WM_NET_OPEN					WM_USER+310								//打开网

//子弹信息
struct tagBulletInfo
{
	//子弹信息
	enBulletStatus					BulletStatus;							//鱼类状态
	DWORD							dwCptrFishID[MAX_CPTR_FISH_COUNT];		//捕获鱼类
	bool							bCptrMode;								//捕获模式
	DWORD							dwBulletID;								//子弹标识
	BYTE							byBulletCount;							//子弹数目
	BYTE							byShootUser;							//设计玩家
	BYTE							byTextureIndex;							//纹理索引

	//轨迹变量
	//BYTE							byAddedFrame;							//额外帧数
	BYTE							byFrameIndex;							//帧数索引
	DWORD							dwMoveTimes;							//移动时间
	BYTE							bySpaceTimes;							//间隔时间
	float							fAngle;									//轨迹角度
	POINT							ptTrace[500];							//轨迹坐标
	WORD							wTracePtCount;							//轨迹数目
	WORD							wTracePtIndex;							//轨迹索引
};

//子弹类
class CBullet :public CDxSkin
{
	//内核变量
private:
	HWND							m_hNotifyWnd;							//通知窗体
	enBulletKind					m_enBulletKind;							//子弹种类
	CFactoryTemplate<tagBulletInfo>	m_BulletFactory;						//子弹工厂
	DWORD							m_dwBulletID;							//子弹标识

	//位置变量
private:
	CRect							m_rcBound;								//射击范围

	//静态变量
private:

	//函数定义
public:
	//构造函数
	CBullet(void);
	//析构函数
	~CBullet(void);

	//接口函数
public:
	//设置窗体
	void SetNotifyWnd(HWND hNotifyWnd) {m_hNotifyWnd=hNotifyWnd; }
	//激活子弹
	tagBulletInfo *ActiveNewBullet();
	//发射子弹
	DWORD Shoot(WORD wChairID, bool bCptrMode, DWORD dwCptrFishID[], WORD wCptrFishCount, int nSourceX, int nSourceXY, float fAngle, enBulletCountKind BulletCountKind, BYTE byBulletCount);
	//绘画子弹
	void DrawBullet();
	//设置范围
	void SetBound(CRect &rcBound) {m_rcBound=rcBound;}
};

#endif