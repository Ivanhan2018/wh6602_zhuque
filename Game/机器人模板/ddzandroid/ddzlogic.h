
//
// 2011
//

#ifndef _ROLES_H_DDZ_LOGIC_DEF_H_
#define _ROLES_H_DDZ_LOGIC_DEF_H_

#include "TagRoles.h"
#include "KddzAI.h"
#include "Roles.h"

//数值掩码
#define	MASK_COLOR					0xF0								//花色掩码
#define	MASK_VALUE					0x0F								//数值掩码


//斗地主AI逻辑类
class CDDZLogic
{
public:
	CDDZLogic( void );
	virtual ~CDDZLogic( void );

public:
	//初始化逻辑数据
	void InitLogicData(BYTE byMeChairID);
	//设置手牌数据
	void SetHandCard(const BYTE* pCard, BYTE byNums, bool bIsLand=false);
	//叫不叫地主
	bool GetLandTag();
	//设置地主
	void SetLandTag(BYTE bBankerUserID);
	//出牌
	void OutCard(BYTE* pCard, BYTE& byNums);
	//出牌结果
	void OutCardResult(BYTE byChairID, bool bIsOutCard, const BYTE* pCard=NULL, BYTE byNums=0);
	//设置谁出牌
	void SetWhoOutCard(BYTE byChairID, bool bIsReset=false);
	//删除扑克
	bool RemoveCard(const BYTE cbRemoveCard[], BYTE cbRemoveCount, BYTE cbCardData[], BYTE cbCardCount);

public:
	//获取数值
	BYTE GetCardValue(BYTE cbCardData) { return cbCardData&MASK_VALUE; }
	//获取花色
	BYTE GetCardColor(BYTE cbCardData) { return cbCardData&MASK_COLOR; }
	//转换牌数据.
	int  SwitchCardDataToInt(BYTE byCardData);
	BYTE SwitchCardDataToByte(int iCardData);
	bool SwitchBYTEtoINT(const BYTE* pCard, BYTE byNums, int* pRetCard, int& iRetNums);
	bool SwitchINTtoBYTE(const int* pCard, int iNums, BYTE* pRetCard, BYTE& byRetNums);
	//出牌表
	void MakeCardsTable( const RCVector& rcIn );

private:
	Roles					m_rAndroid;			// 机器人
	KddzAI*					m_aiAndroid;		// 机器人->AI

	AIRUN					m_aiRun;	 		//当前出牌状态
	RCVector				m_rcAll;			//保存牌张数
	BYTE					m_byMeChairID;		//自己椅子号
	BYTE					m_byLandChairID;	//地主椅子号
};


#endif