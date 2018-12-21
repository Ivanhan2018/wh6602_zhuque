#pragma once

namespace PokerDef
{
const unsigned char MAX_POKER_COUNTS		= 54;							// 整副牌54张(含小王,大王) 
const unsigned char CARDINDEX_SMALL		= 53;							// 小王牌索引 
const unsigned char CARDINDEX_BIG		= 54;							// 大王牌索引 
const unsigned char CARDINDEX_BACK		= 55;							// 背面牌索引 
const unsigned char CARDINDEX_NULL		= 0;							// 无效牌索引 
//const unsigned char CARDINDEX_FIRST	= 1;							// 有效牌索引第一张 
//const unsigned char CARDINDEX_LAST		= CARDINDEX_BIG;				// 有效牌索引最后一张 


	///>==============================================
	///> 扑克牌花色
	///>==============================================
	typedef enum e_PokerColor
	{
		CC_NULL_S = 0,		/* 无色					*/
	    
		CC_DIAMOND_S,		/* 方块 => Diamond		*/
		CC_CLUB_S,			/* 梅花 => Club			*/
		CC_HEART_S,			/* 红心 => Heart		*/
		CC_SPADE_S,			/* 黑桃 => Spade		*/
	    
		CC_TOTAL_S,			/* TOTAL OF COLORS 		*/
	}TPOKERCOLOR;

	///>==============================================
	///> 扑克牌点数
	///>==============================================
	typedef enum e_CardPoint
	{
		CP_NULL_S = 0, 		/* 牌背					*/
	    
		CP_A_S,			 	/* A					*/
		CP_2_S,	 			/* 2					*/
		CP_3_S,		 		/* 3					*/
		CP_4_S,		 		/* 4					*/
		CP_5_S,		 		/* 5					*/
		CP_6_S,		 		/* 6					*/
		CP_7_S,		 		/* 7					*/
		CP_8_S,		 		/* 8					*/
		CP_9_S,		 		/* 9					*/
		CP_10_S,		 	/* 10					*/
		CP_J_S,		 		/* J	=> Jack			*/
		CP_Q_S,		 		/* Q	=> Queen		*/
		CP_K_S,		 		/* K	=> King			*/
	    
		CP_BJ_S,		 	/* 小王	=> Black Joker	*/
		CP_RJ_S,		 	/* 大王	=> Red Joker	*/
	    
		CP_TOTAL_S,	 		/* TOTAL OF POINTS		*/
	}TCARDPOINT;

	// 获取牌索引
	inline unsigned char GetCard(unsigned char nColor, unsigned char nPoint)
	{
		//获取小王索引
		if (CP_BJ_S == nPoint)
		{
			return (CC_SPADE_S - 1) * 13 + CP_BJ_S;
		}
		//获取大王索引
		else if (CP_RJ_S == nPoint)
		{
			return (CC_SPADE_S - 1) * 13 + CP_RJ_S;
		}
		//获取普通牌索引
		else if (CP_A_S <= nPoint && nPoint <= CP_K_S)
		{
			if (CC_DIAMOND_S <= nColor && nColor <= CC_SPADE_S)
				return (nColor - 1) * 13 + nPoint;
		}
		return 0;
	}

	// 获取牌花色
	inline unsigned char GetCardColor(unsigned char nCard)
	{
		//处理获取普通牌花色
		if (CARDINDEX_NULL < nCard && nCard < CARDINDEX_SMALL)
			return (((nCard - 1) / 13) % 4) + 1;
		//处理王牌的花色
		else if (nCard == CARDINDEX_SMALL)
			return CC_NULL_S;//小王无花色
		else if (nCard == CARDINDEX_BIG)
			return CC_NULL_S;//大王无花色
		return 0;
	}

	// 获取牌点数
	inline unsigned char GetCardPoint(unsigned char nCard)
	{
		//处理获取普通牌的点数
		if (CARDINDEX_NULL < nCard && nCard < CARDINDEX_SMALL)
			return (nCard - 1) % 13 + 1;
		//处理获取王牌的点数
		else if (nCard == CARDINDEX_SMALL)
			return CP_BJ_S;
		else if (nCard == CARDINDEX_BIG)
			return CP_RJ_S;
		return 0;
	}

};
