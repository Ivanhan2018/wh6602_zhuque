
//
// 2009
// DhLj
// 段注释(>=3),行注释(==1)
//

#ifndef _TAGROLES_H_
#define _TAGROLES_H_

#include <vector>

const int MAXCARDNUMS = 20;

typedef enum _tagCardState
{
	no,		//未选中状态
	yes		//选中状态
}CARDSTATE;

typedef enum _tagCardClass
{
	cerror,								// 无效牌	|0
	cone,								// 单牌		|1!
	ctwo,								// 双牌		|2!
	cthree,								// 三牌		|3!
	cfour,								// 炸弹		|4!
	ctth,								// 三顺(2)	|5!
	cthone,								// 三带一单	|6!
	cthtwo,								// 三带一对	|7!
//	cfone,								// 四带一单	|8!
//	cftwo,								// 四带一对	|9!
	cttth,								// 双顺(3)	|10!
	cthth,								// 三顺(3)	|11!
	cftone,								// 四带两单	|12!
	cfttwo,								// 四带两对	|13!
	ctfour,								// 双顺(4)	|14!
	cthfour,							// 三顺(4)	|15!
	ctthone,							// 三顺带两单|16!
	ctthtwo,							// 三顺带两对|17!
	clink5,								// 单顺(5)	|18!
	clink6,								// 单顺(6)	|19!
	clink7,								// 单顺(7)	|20!
	clink8,								// 单顺(8)	|21!
	clink9,								// 单顺(9)	|22!
	clink10,							// 单顺(10)	|23!
	clink11,							// 单顺(11)	|24!
	clink12,							// 单顺(12)	|25!
	ctfive,								// 双顺(5)	|26!
	cthfive,							// 三顺(5)	|27!
	ctsix,								// 双顺(6)	|28!
	cthsix,								// 三顺(6)	|29!
	cththone,							// 三顺带三单|30!
	cththtwo,							// 三顺带三对|31!
	ctseven,							// 双顺(7)	|32!
	ctevent,							// 双顺(8)	|33!
	ctnine,								// 双顺(9)	|34!
	ctten,								// 双顺(10)	|35!
	cthfone,							// 三顺带四单|36!
	cthftwo,							// 三顺带四对|37!
	chigh,								// 双王
	ctrue								// 首出
	
}CARDCLASS;

typedef enum _tagCardChange
{
	R3,									// 0
	R4,									// 1
	R5,									// 2
	R6,									// 3
	R7,									// 4
	R8,									// 5
	R9,									// 6
	RO,									// 7
	RJ,									// 8
	RQ,									// 9
	RK,									// 10
	RA,									// 11
	RT,									// 12
	RWS,								// 小王
	RWB									// 大王
}CARDCHANGE;


typedef enum _tagCardIf
{
	sde,								// 单牌位
	beg,								// 开始位
	mid,								// 中间位
	end									// 结束位
}CARDIF;


typedef struct _tagCard			// typedef struct _tagCard CARD;
{
	int iCard;
	CARDSTATE cs;
}CARD;

typedef struct _tagCardR
{
	int iNum;							// 这张牌的数量
	CARDCHANGE rc;
}RCARD;


typedef struct _tagCardWorth
{
	RCARD rcard;
	CARDIF csif;						
}CARDWORTH;

typedef struct _tagWebPos
{
	unsigned long ulSizex;
	unsigned long ulSizey;
}WEBPOS;


typedef std::vector<int> IVector;
typedef std::vector<CARD> CVector;
typedef std::vector<RCARD> RCVector;
typedef std::vector<CARDWORTH> CWVector;

typedef enum _tagAIPos
{
	p_error,
	p_mid,
	p_up,
	p_down	
}AIPOS;


typedef struct _tagAIRun
{
	RCVector rcIn;			//当前出的每张牌的个数
	RCVector rcAll;			//所有出的每张牌的个数
	CARDCLASS cc;			//出牌类型
	AIPOS aipos;			//出牌位置(谁家出牌)
	unsigned int iSizeMid;	//自己牌张数
	unsigned int iSizeUp;	//上家牌张数
	unsigned int iSizeDown;	//下家牌张数
}AIRUN;

#endif