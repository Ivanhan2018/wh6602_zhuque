#ifndef _CB_DAGUN_H
#define _CB_DAGUN_H
#pragma once
#include <vector>
using namespace std;
#include "PokerDefine.h"

typedef unsigned char TCARD;
typedef vector<TCARD> TCARDS;

#define	MAXPAI					27		//每个人最多27张
#define TYPE_ERROR				-1
#define	TYPE_NULL				0
#define TYPE_ONE				1		//单张
#define TYPE_TWO				2		//对子
#define TYPE_THREE				3       //三只
#define TYPE_TWOSTR				4		//连对，比如3344,445566... 2除外
#define TYPE_THREESTR			5       //三连对，比如333444,333444555... 2除外
#define TYPE_BOMB_510K			6		//510k炸弹
#define TYPE_BOMB_NOMORL		7		//普通炸弹
#define TYPE_BOMB_STR			8		//连炸，摇摆
#define TYPE_BOMB_7XI			9		//7喜炸弹，7张相同的牌，不带大小王
#define TYPE_BOMB_8XI			10		//8喜炸弹，8张相同的牌，不带大小王
#define TYPE_BOMB_KING			11      //四张大小王
#define TYPE_KING               12      //王

#define SMA_KING				53		//小王
#define BIG_KING				54		//大王

#define MAX_BOMBSTR_NUM         6       // 27张牌，最多6连炸
//一张牌
typedef struct tagPOKER
{
	unsigned char		point;	// 1-15;
	unsigned char		index;	// 1-54
	unsigned char		color;  // 0-4
	void		Set(int n)
	{
		if(n > 0 && n < 55)
			index = n;
		if( n <= 0) {point = 0;color = 0;return;}
		if( n <= 13){point = n;color = 1;return;}
		if( n <= 26){point = n-13;color = 2;return;}
		if( n <= 39){point = n-26;color = 3;return;}
		if( n <= 52){point = n-39;color = 4;return;}
		if( n == 53){point = 14;color = 0;return;}
		if( n == 54){point = 15;color = 0;return;}
		if( n > 54) {point = 0;color = 0;return;}
	}
}TPoker;
typedef struct tagCARDTYPE
{
	int		type;		//牌类型
	int     len;		//牌长度
	unsigned char	card;
	unsigned char	color;		//花色，这个只针对510k炸弹
	unsigned char    count;		//这个只针对连炸中，最大炸弹的张数
}TCardType;

//一幅牌型
typedef struct tagPOKERGROUP
{
	unsigned char			point;		//牌型值1-54
	unsigned char            count;
	vector<unsigned char>	index;		//1-54
}TPokerGroup;
//用来替换王的
typedef struct tagFakePoker
{
	unsigned char        index;		//原来的值
	unsigned char		fakeindex;	//替代值	
}TFakePoker;
typedef struct tagFakeType
{
	TFakePoker   fakeking[4];
	TCardType	 cardtype;
}FakeType;

//做提示用
typedef struct tagPOKERGROUP2
{
	unsigned char			point;		//牌型值1-15,k105无效
	unsigned char            color;      //花色，这个只针对k105
	unsigned char            count;		//张数
	vector<unsigned char>	index;		//牌列表
}TPokerGroup2;

typedef struct tagBOMBSTR
{//连炸结构
	unsigned char nBombCount;//炸弹数
	unsigned char nMaxCount; //最大炸弹的张数
	unsigned char nCount;	//所有的张数
	unsigned char nPoint;	//最大炸弹的值
	vector<unsigned char> index;//牌列表
}TBombStr;
enum tagPATCOUNID
{
	pai_null = 0,
	pai_1,
	pai_2,
	pai_3,
	pai_4,
	pai_5,
	pai_6,
	pai_7,
	pai_8,
	pai_king,
	pai_max
};

typedef struct tagThree
{
	unsigned char kingcount;		//这个用来代表index里面，有几个王
	unsigned char point;			//点数
	unsigned char index1;
	unsigned char index2;
	unsigned char index3;
}TThree;
typedef struct tagTwo
{
	unsigned char kingcount;		//这个用来代表index里面，有几个王
	unsigned char point;			//点数
	unsigned char index1;
	unsigned char index2;
}TTwo;
//算法类
class CDG4
{
public:
	CDG4(void);
	~CDG4(void);
public:
	vector<TPokerGroup2>	m_vOne;
	vector<TPokerGroup2>	m_vTwo;
	vector<TPokerGroup2>	m_vThree;
	vector<TPokerGroup2>	m_vFour;
	vector<TPokerGroup2>	m_vFive;
	vector<TPokerGroup2>	m_vSix;
	vector<TPokerGroup2>	m_vSeven;
	vector<TPokerGroup2>	m_vEight;
	vector<TPokerGroup2>	m_vK105;//k105,比较特殊，
	vector<TPokerGroup2>	m_vKing;
public:
	int			GetCardLevel(unsigned char card_id);	//一张牌大小card_id = 1-54
	TCardType	GetType(unsigned char card_list[MAXPAI],int cardlen=MAXPAI,int outtype = TYPE_NULL,TFakePoker* refakepoker = NULL);		//得到这组牌的类型
	//有王的情况下，得出他的牌型，这个只针对三连对或者2连对
	//fakepoker里面是这个王的替代值
	FakeType   GetTypeByKing(unsigned char card_list[MAXPAI],int cardlen,int kingnum,TFakePoker fakepoker[4],int outtype=TYPE_NULL);
	//牌数量
	unsigned char		GetCardNum(unsigned char card_list[MAXPAI],int cardlen = MAXPAI);  
	//按大小排列,smalltobig=ture,从小到大
	void		SortByIndex(unsigned char card_list[MAXPAI],int cardlen = MAXPAI,bool smalltobig=true); 
	//按照牌的多少来排
	void		SortByCount(unsigned char card_list[MAXPAI],int cardlen = MAXPAI);
	//是否是王
	bool		IsKing(unsigned char card_id);	
	//是否全部是王
	bool		IsAllKing(unsigned char card_list[MAXPAI],int cardlen = MAXPAI);
	//得到王的个数
	int			GetKingNum(unsigned char card_list[MAXPAI],int cardlen = MAXPAI,TFakePoker* fakepoker=NULL);
	//非王的情况下，其他牌是不是都相等？如果相等，返回最大序列值和数
	bool		IsAllEqualExceptKing(unsigned char& card,/*int& num,*/unsigned char card_list[MAXPAI],int cardlen = MAXPAI);
	//判断是不是普通炸弹，如果是，返回值
	bool		IsNormalBomb(unsigned char& card,/*int& num,*/unsigned char card_list[MAXPAI],int cardlen = MAXPAI);
	//判断是不是天炸
	bool		IsKingBomb(unsigned char card_list[MAXPAI],int cardlen = MAXPAI);
	//判断是不是8喜
	bool		Is8XiBomb(unsigned char card_list[MAXPAI],int cardlen = MAXPAI);
	//判断是不是7喜
	bool		Is7XiBomb(unsigned char card_list[MAXPAI],int cardlen = MAXPAI);
	//判断是不是510k，如果是，返回花色1-4代表了方块，梅花，红桃，黑桃,0代表杂的
	bool		Is510KBomb(unsigned char& color,unsigned char card_list[MAXPAI],int cardlen = MAXPAI);
	//是不是连炸,num返回的是最大炸弹的张数
	bool        IsBombStr(unsigned char& card,unsigned char& num,unsigned char card_list[MAXPAI],int cardlen = MAXPAI);
	//是不是2连对
	bool		IsTwoStr(unsigned char card_list[MAXPAI],int cardlen = MAXPAI);
	//是不是3连对
	bool        IsThreeStr(unsigned char card_list[MAXPAI],int cardlen = MAXPAI);
	//判断是否能出牌
	bool        CanOut(unsigned char outCard[MAXPAI],unsigned char followCard[MAXPAI],int outtype,int& followtype,bool isfirst = false,TFakePoker* fakepoker = NULL);
	//比较大小， typeFollow > typeFirst 返回真
	bool		Compare(TCardType typeFirst,TCardType typeFollow);
	//得到牌里的分数
	int         GetScore(unsigned char card_list[MAXPAI],int cardlen = MAXPAI);
	//得到7喜炸弹个数
	int         Get7XibombNum(unsigned char card_list[MAXPAI]);
	//得到8喜炸弹个数
	int         Get8XibombNum(unsigned char card_list[MAXPAI]);
	//是否存在天炸？
	bool        HasKingBomb(unsigned char card_list[MAXPAI]);
	// 检查出的牌里面，是否是拆开7xi,8xi或者天炸来出的
	// 返回1的话是拆开7喜，返回2的话是拆开8喜，返回3的话是拆开天炸，返回0无效
	int         CheckBeep(unsigned char card_list[MAXPAI],unsigned char outCard[MAXPAI],int outlen=MAXPAI);
///////////////////////////////////////////////
//    以下为提示
////////////////////////////
	
	//优化一下提示顺序，让没有王的先提示
	void       SortBeepCardList(vector<TPokerGroup2>& mybeepout);     
	//分类成基本的牌形
	void		GetGroupType(TCARDS mycard);
	//提示出牌
	bool        BeepCardOut(unsigned char otherout[MAXPAI],int outtype,vector<TPokerGroup2>& mybeepout);//提示出牌
	//首出牌提示
	bool		BeepFirstCardOut(vector<TPokerGroup2>& mybeepout);
	//得到所有连炸，在调用这个函数前，需要调用GetGroupType来分类
	void        GetAllBombStr(vector<TBombStr>& mybombstr);
	//得出连炸中，连子数，和最大炸弹的张数和点数
	void        GetFromBombStr(unsigned char otherpai[MAXPAI],int& nStrCount,int& nMaxCount,unsigned char& nPoint);
	//得到用王组合的炸弹,outpoint 为1-54
	void        GetCombineBomb(unsigned char outpoint,int outbomblen,vector<TPokerGroup2>& combinelist);
	//得到所有的k-10-5,包括王，也包括杂色
	void		GetAllCombineK105(int color,vector<TPokerGroup2>& combinelist);
	//得到所有和出牌长度一样的三连对，包括用王组合的,outpoint 为1-54
	void        GetCombineThreeStr(unsigned char outpoint,int outlen,vector<TPokerGroup2>& combinelist);
	//得到所有和出牌长度一样的两连对，包括用王组合的，outpoint为1-54
	void        GetCombineTwoStr(unsigned char outpoint,int outlen,vector<TPokerGroup2>& combinelist);
	//得到所有的三对，包括用王组合的
	void        GetCombineThree(unsigned char outpoint,vector<TPokerGroup2>& combinelist);
	//得到所有的两对，
	void        GetCombineTwo(unsigned char outpoint,vector<TPokerGroup2>& combinelist);
	//得到所有大过outpoint的单牌
	void        GetAllOne(unsigned char outpoint,vector<TPokerGroup2>& combinelist);
};
#endif
