#ifndef TABLE_FRAME_SINK_HEAD_FILE
#define TABLE_FRAME_SINK_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "GameLogic.h"
#include <afxtempl.h>
#include "cnd_ILogMgr.h"
//////////////////////////////////////////////////////////////////////////

//轨迹信息
struct tagServerFishTrace
{
	//关键坐标
	int								nInitX[5];								//关键坐标
	int								nInitY[5];								//关键坐标
	int								nInitCount;								//坐标数目

	//鱼群属性
	enFishKind						FishKind;								//鱼群种类
	DWORD							dwFishID;								//鱼类标识

	//轨迹变量
	//POINT							ptTrace[500];							//轨迹坐标
	//int							nTracePtCount;							//轨迹数目
	DWORD							dwBuildTime;							//建立时间
};

//子弹信息
struct tagBulletInfo
{
	BYTE							byBulletCount;							//子弹数目
	DWORD							dwShootTime;							//发射时间
	BYTE							byCptrFishCount;						//捕获数目
	enFishKind						FishKind;								//鱼种类
};


//游戏桌子
class CTableFrameSink : public ITableFrameSink, public ITableUserAction
{
private:
	cnd_ILogMgr*	m_pRoomLog;	// 日志组件的指针

public:
	// GameSerivce.dll 通向游戏服务端的通道，可以传递任何简易数据或成员指针
	virtual bool channel_to_game_service(int iMainCode, int iAssistantCode, void* pData, int iDataLen);

	//配置变量
private:
	LONG							m_lBulletCharge[enBulletCountKind_End];	//子弹费用
	BYTE							m_byCptrProbability[5][enFishKind_No];	//捕获概率
	LONG							m_lUserCellScore[GAME_PLAYER];			//单元积分
	LONG							m_lCellScore[CELL_SCORE_COUNT];			//单元积分

	//游戏变量
protected:
	LONG							m_lBulletCount[GAME_PLAYER][enBulletCountKind_End];	//子弹数目
	LONG							m_lCoinCount[GAME_PLAYER];				//金币数目
	LONG							m_lWinCoinCount[GAME_PLAYER];			//金币数目
	LONG							m_lUserShootCount[GAME_PLAYER];			//射击数目
	DWORD							m_dwFishID;								//鱼群标识
	WORD							m_wCptrFishCount[GAME_PLAYER][enFishKind_No];//鱼群数目
	enSeaSceneKind					m_CureSeaSceneKind;						//当前场景
	bool							m_bWaveLToR;							//改变方向
	DWORD							m_dwSharkAppearTime;					//鲨鱼时间
	DWORD							m_dwGoldSharkAppearTime;				//鲨鱼时间
	DWORD							m_dwPlayTime[GAME_PLAYER];				//游戏时间
	CMap<DWORD,DWORD,tagBulletInfo *,tagBulletInfo *>	m_ShootBullet[GAME_PLAYER];//子弹信息
	CFactoryTemplate<tagServerFishTrace>	m_ServerFishTrace;				//轨迹工厂
	CFactoryTemplate<tagBulletInfo>			m_ShootBulletFactory;			//子弹工厂

	//彩金变量
protected:
	LONG							m_lBonusOrigin;							//初始数目
	LONG							m_lBonusMultipleMax;					//最大倍数
	LONG							m_lBonusMultipleUser[GAME_PLAYER];		//当前倍数
	LONG							m_lBonusMultiple;						//单元数目
	LONG							m_lBulletLimit;							//子弹限制
	LONG							m_lBonusUnitTime;						//单元时间
	LONG							m_lBonusAccumulate[GAME_PLAYER];		//递增分数
	LONG							m_lShootBulletCount[GAME_PLAYER];		//射击数目

	//控制参数
protected:
	bool							m_bControlScore;						//控制积分
	BYTE							m_byWindMulLimitBonus;					//得分限制
	BYTE							m_byWindMulLimitMax;					//得分限制
	BYTE							m_byWindMulLimitMin;					//得分限制
	BYTE							m_byCellScoreRectify[CELL_SCORE_COUNT];	//单元调整
	bool							m_bTraceGameInfo;						//输出信息
	LONG							m_lTraceScoreLevel;						//输出级别

	//规则鱼群
protected:
	BYTE							m_byRegFishColCount;					//规则列数
	int								m_nRegularSign;							//规则方向
	BYTE							m_byRegularForm;						//规则形式

	//系统消息
protected:
	TCHAR							m_szGameRoomName[SERVER_LEN];			//房间名称
	TCHAR							m_szIniFileName[MAX_PATH];				//配置文件
	BYTE							m_bySysIndex;							//消息索引

	//组件变量
protected:
	CGameLogic						m_GameLogic;							//游戏逻辑

	//组件接口
protected:
	ITableFrame	*					m_pITableFrame;							//框架接口
	ITableFrameControl				* m_pITableFrameControl;				//框架接口
	const tagGameServiceOption		* m_pGameServiceOption;					//配置参数

	//属性变量
protected:
	static const WORD				m_wPlayerCount;							//游戏人数
	static const enStartMode		m_GameStartMode;						//开始模式

	//函数定义
public:
	//构造函数
	CTableFrameSink();
	//析构函数
	virtual ~CTableFrameSink();

	//基础接口
public:
	//释放对象
	virtual void __cdecl Release() { if (IsValid()) delete this; return ; }
	//是否有效
	virtual bool __cdecl IsValid() { return AfxIsValidAddress(this,sizeof(CTableFrameSink))?true:false; }
	//接口查询
	virtual void * __cdecl QueryInterface(const IID & Guid, DWORD dwQueryVer);

	//管理接口
public:
	//配置桌子
	virtual bool __cdecl InitTableFrameSink(IUnknownEx * pIUnknownEx);
	//复位桌子
	virtual VOID __cdecl RepositTableFrameSink();

	//游戏事件
public:
	//游戏开始
	virtual bool __cdecl OnEventGameStart();
	//游戏结束
	virtual bool __cdecl OnEventGameEnd(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason);
	//发送场景
	virtual bool __cdecl SendGameScene(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbGameStatus, bool bSendSecret);

	//信息接口
public:
	//开始模式
	virtual enStartMode __cdecl GetGameStartMode();
	//游戏状态
	virtual bool __cdecl IsUserPlaying(WORD wChairID) { return true; }

	//事件接口
public:
	//定时器事件
	virtual bool __cdecl OnTimerMessage(WORD wTimerID, WPARAM wBindParam);
	//游戏消息处理
	virtual bool __cdecl OnGameMessage(WORD wSubCmdID, const void * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem);
	//框架消息处理
	virtual bool __cdecl OnFrameMessage(WORD wSubCmdID, const void * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem);

	//用户事件
public:
	//用户断线
	virtual bool __cdecl OnActionUserOffLine(WORD wChairID, IServerUserItem * pIServerUserItem);
	//用户重入
	virtual bool __cdecl OnActionUserReConnect(WORD wChairID,IServerUserItem * pIServerUserItem) { return true; }
	//用户坐下
	virtual bool __cdecl OnActionUserSitDown(WORD wChairID,IServerUserItem * pIServerUserItem, bool bLookonUser);
	//用户起立
	virtual bool __cdecl OnActionUserStandUp(WORD wChairID,IServerUserItem * pIServerUserItem, bool bLookonUser);
	//用户同意
	virtual bool __cdecl OnActionUserReady(WORD wChairID,IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize) { return true; }

	//游戏事件
private:
	//购买子弹
	bool OnSubBuyBullet(IServerUserItem * pIServerUserItem, enBulletCountKind BulletCountKind);
	//发射炮弹
	bool OnSubUserShoot(IServerUserItem * pIServerUserItem, DWORD dwBulletID, float fAngle, enBulletCountKind BulletCountKind, BYTE byShootCount);
	////捕捉鱼群
	//bool OnSubCaptureFish(IServerUserItem * pIServerUserItem, DWORD dwFishID, DWORD dwBulletID, BYTE byBulletCount, BYTE byCptrProbability);
	//捕捉鱼群
	bool OnSubHitFish(IServerUserItem * pIServerUserItem, DWORD dwFishID, DWORD dwBulletID);
	//计算分数
	bool OnSubCalcScore(IServerUserItem * pIServerUserItem);
	//获取分数
	bool OnSubGetScoreInfo(IServerUserItem * pIServerUserItem);
	//设置单元
	bool OnSubSetCellScore(IServerUserItem * pIServerUserItem, LONG lCellScore);	
	
	//辅助函数
private:
	//鱼群标识
	DWORD GetNewFishID();
	//产生轨迹
	bool BuildTracePointLinear(tagServerFishTrace *pServerFishTraceNew, tagServerFishTrace *pServerFishTraceLast);
	//产生轨迹
	bool BuildTracePointLinearBonus(tagServerFishTrace *pServerFishTraceNew);
	//发送消息
	void SendDataExcludeSourceUser(IServerUserItem * pIServerUserItemSource, WORD wSubCmdID, void * pData, WORD wDataSize);
	//输出信息
	void TraceString(LPCTSTR pszMessage);
	//发送子弹
	void SendBulletCount(WORD wChairID, enBulletCountKind BulletCountKind, bool bBroadcast=true);
	//发送金币
	void SendCoinCount(WORD wChairID);
	//发送鱼群
	void SendFishCount(WORD wChairID, enFishKind FishKind);
	//产生群鱼
	void BuildRegularFish(bool bLastCol);
	//计算分数
	void CalcScore(IServerUserItem * pIServerUserItem, WORD wChairID);
	//调整位置
	bool RectifyTracePosX(int &nPosX);
	//读取配置
	void ReadConfig();
	//发送消息
	bool SendGameMessage(IServerUserItem * pIServerUserItem, LPCTSTR lpszMessage);
	//存在判断
	tagServerFishTrace *GetFishTraceInfo(DWORD dwFishID);
	//调试信息
	void TraceGameInfo(IServerUserItem * pIServerUserItem, LONGLONG lScore, LONGLONG lLeftBullet, WORD wChairID);
};

//////////////////////////////////////////////////////////////////////////

#endif