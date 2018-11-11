#ifndef		___CREATE_DEF_GAME_CLIENT_1005_DEF__
#define		___CREATE_DEF_GAME_CLIENT_1005_DEF__


#include "gameclientframe.h"
#include "CMD_Sparrow.h"
#include "GameLogic.h"
#include "..\..\开发库\Include\cnd_ILogMgr.h"

//游戏客户端
class CGameClient : public CGameClientFrame
{
public:
	CGameClient(void);
	virtual ~CGameClient(void);

private:
	//游戏逻辑
	CGameLogic				m_GameLogic;

private:
	WORD					m_wCurrentUser;								//当前玩家
	WORD					m_wBankerUser;								//庄家
	BYTE					m_cbActionMask;								//动作标识
	BYTE					m_cbActionCard;								//操作牌
	bool					m_bHearStatus;								//听牌状态
	bool					m_bIsMeCanOutCard;							//是否自己可以出牌

private:
	BYTE					m_byGangCard[4];							//杠牌
	BYTE					m_byGangCardNums;							//个数
	BYTE					m_byBuCard[4];								//补牌
	BYTE					m_byBuCardNums;								//个数

	//扑克变量
protected:
	BYTE					m_cbCardIndex[MAX_INDEX];					//手中扑克
	//BYTE                    m_TempKing;                                 //临时变量
	// add by Ivan_han 2011-12-28
	cnd_ILogMgr*	m_pRoomLog;	// 房间日志
public:
	// 初始化一些东西
	void InitSome(DWORD dwWndID);

	// 释放一些东西
	void UninitSome();

	//初始函数
	virtual bool InitGameFrame();
	//重置框架
	virtual void ResetGameFrame();

	//系统事件
public:
	//时间消息
	virtual bool OnTimerMessage(UINT nElapse, UINT nTimerID);
	//网络消息
	virtual bool OnGameMessage(WORD wSubCmdID, const void * pBuffer, WORD wDataSize);
	//游戏场景
	virtual bool OnGameSceneMessage(BYTE cbGameStatus, const void * pBuffer, WORD wDataSize);

public:
	//初始化
	void InitData();
	//发送之前初始化
	void InitSendCard();
	//取消所有定时器
	void CancelAllTimer();

	//消息处理
protected:
	//是否开始骰子动画
	bool OnSubStartSZFlash();
	//游戏开始
	bool OnSubGameStart(const void * pBuffer, WORD wDataSize);
	//游戏结束
	bool OnSubGameEnd(const void * pBuffer, WORD wDataSize);
	////海底操作
	//bool OnSubOperateHaiDi(const void * pBuffer, WORD wDataSize);
	//游戏开始出牌
	bool OnSubGameStartOutCard(const void * pBuffer, WORD wDataSize);
	//用户出牌
	bool OnSubOutCard(const void * pBuffer, WORD wDataSize);
	//操作提示
	bool OnSubOperateNotify(const void * pBuffer, WORD wDataSize);
	//发牌消息
	bool OnSubSendCard(const void * pBuffer, WORD wDataSize);
	//操作结果
	bool OnSubOperateResult(const void * pBuffer, WORD wDataSize);
	//听牌
	bool OnSubTingCard(const void * pBuffer, WORD wDataSize);
	//听牌结果
	bool OnSubTingCardResult(const void * pBuffer, WORD wDataSize);

	//处理函数
protected:
	//出牌
	void OnOutCard();
	//用户操作牌
	void OnUserOperateCard();
	//扑克操作
	void OnCardOperate(BYTE byOperaType, BYTE byCardData);
	//是否听牌操作了
	bool bIsTingCardOpera();
	//判断是否手中有这张牌
	bool bIsHaveCard(BYTE byCard, BYTE& byLastCard);

	//辅助函数
protected:
	//搜索出牌
	void SearchOutCard(BYTE& byOutCard);

public:
	DECLARE_MESSAGE_MAP()
};
#endif