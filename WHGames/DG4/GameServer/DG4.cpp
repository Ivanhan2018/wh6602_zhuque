#include "DG4.h"
#include "PokerDefine.h"
#include <string>
using namespace PokerDef;
CDG4::CDG4(void)
{
}

CDG4::~CDG4(void)
{
}
bool CDG4::IsKing(unsigned char card_id)
{
	if(card_id == BIG_KING || card_id == SMA_KING)
		return true;
	else
		return false;
}
bool CDG4::IsAllKing(unsigned char card_list[MAXPAI],int cardlen /* = MAXPAI */)
{
	for(int i = 0; i < cardlen; i ++)
	{
		if(!IsKing(card_list[i]))
			return false;
	}
	return true;
}
int CDG4::GetKingNum(unsigned char card_list[MAXPAI],int cardlen /* = MAXPAI */,TFakePoker* fakepoker/*= NULL*/)
{
	int num =0;
	for(int i = 0; i < cardlen; i++)
	{
		if(IsKing(card_list[i]))
		{
			if(fakepoker != NULL)
				fakepoker[num].index = card_list[i];
			num++;
		}
		if(num >= 4)
			break;
	}
	return num;
}
bool CDG4::IsAllEqualExceptKing(unsigned char& card,/*int& num,*/unsigned char card_list[MAXPAI],int cardlen /* = MAXPAI */)
{
	card = 0;
	//num = 0;
	if(cardlen > MAXPAI)
		return false;
	unsigned char k = 0;
//	SortByIndex(card_list,cardlen);
	for(int i = cardlen-1; i >=0; i--)
	{
		if(IsKing(card_list[i]))
			continue;
		k = card_list[i];
		break;
	}
	if(k == 0)
		return false;//全是王
	int n = 0;
	for(int i = cardlen-1; i >=0; i--)
	{
		if(IsKing(card_list[i]))
			continue;
		if(GetCardLevel(k) != GetCardLevel(card_list[i]))
			return false;
		//num++;
	}
	card = k;
	return true;
}
bool CDG4::IsNormalBomb(unsigned char& card,/*int& num,*/unsigned char card_list[MAXPAI],int cardlen /* = MAXPAI */)
{
	card = 0;
//	num = 0;
	if(cardlen > 12 || cardlen < 4)//4个王+8张同样的牌组成的炸弹
		return false;
	unsigned char p =0;
	//if(IsAllKing(card_list,cardlen))
	//{//如果都是王
	//	if(cardlen < 4)

	//}
	if(IsAllEqualExceptKing(p,card_list,cardlen))
	{
		card = p;
//		num = cardlen;
		return true;
	}
	else
	{

	}
	return false;
}
bool CDG4::Is510KBomb(unsigned char& color,unsigned char card_list[MAXPAI],int cardlen /* = MAXPAI */)
{
	color = 0;
	if(cardlen != 3)
		return false;
	int has_king = 0;
	int has_5	 = 0;
	int has_10	 = 0;
	int has_k	 = 0;

	for(int i = 0; i < cardlen; i++)
	{
		if(IsKing(card_list[i]))//王
			has_king++;
		else if(GetCardLevel(card_list[i]) == GetCardLevel(5))//5
			has_5++;
		else if(GetCardLevel(card_list[i]) == GetCardLevel(10))//10
			has_10++;
		else if(GetCardLevel(card_list[i]) == GetCardLevel(13))//k
			has_k++;
	}
	if(has_king >= 2)
		return false;//两张以上的王，不算
	if(has_5 >= 2)//两张5？不算
		return false;
	if(has_10 >= 2)//两张10？不算
		return false;
	if(has_k >= 2)//两张k？不算
		return false;

	if(has_king == 1)//有王的 
	{
		if(has_king + has_5 + has_10 + has_k == 3)
		{
			TPoker poker[3];
			memset(poker,0,sizeof(poker));
			poker[0].Set(card_list[0]);
			poker[1].Set(card_list[1]);
			poker[2].Set(card_list[2]);
			color = poker[0].color != 0 ? poker[0].color : (poker[1].color != 0 ? poker[1].color :poker[2].color);
			for(int i =0; i < 3;i++)
			{
				if(poker[i].color == 0)
					continue;
				if(poker[i].color != color)
				{//有一个颜色不一样，那么就是杂的
					color = 0;
					break;
				}
			}
			return true;
		}
		else
		{
			return false;
		}
	}
	else if(has_king == 0)
	{//没有王的
		if(has_5 + has_10 + has_k == 3)
		{
			TPoker poker[3];
			memset(poker,0,sizeof(poker));
			poker[0].Set(card_list[0]);
			poker[1].Set(card_list[1]);
			poker[2].Set(card_list[2]);
			if(poker[0].color == poker[1].color && poker[0].color == poker[2].color)
				color = poker[0].color;
			else
				color = 0;
			return true;
		}
		else
		{
			return false;
		}
	}
	return false;
}
bool CDG4::IsKingBomb(unsigned char card_list[MAXPAI],int cardlen /* = MAXPAI */)
{
	if(cardlen != 4)
		return false;
	return IsAllKing(card_list,cardlen);

	//if(IsKing(card_list[0]) && IsKing(card_list[1])
	//	&& IsKing(card_list[2]) && IsKing(card_list[3]) )
	//	return true;
	//return false;
}
bool CDG4::Is8XiBomb(unsigned char card_list[MAXPAI],int cardlen /* = MAXPAI */)
{
	if(cardlen != 8)
		return false;
	for(int i = 1; i < cardlen; i++)
	{
		if(GetCardLevel(card_list[i]) != GetCardLevel(card_list[0]))
			return false;
	}
	return true;
}
bool CDG4::Is7XiBomb(unsigned char card_list[MAXPAI],int cardlen /* = MAXPAI */)
{
	if(cardlen != 7)
		return false;
	for(int i = 1; i < cardlen; i++)
	{
		if(GetCardLevel(card_list[i]) != GetCardLevel(card_list[0]))
			return false;
	}
	return true;
}
bool CDG4::IsBombStr(unsigned char& card,unsigned char& num,unsigned char card_list[MAXPAI],int cardlen /* = MAXPAI */)
{
	if(cardlen < 8 || cardlen > MAXPAI)
		return false;
	card = 0;
	num = 0;
	//先找出牌里面是否都是炸弹
	unsigned char CardIndex[12] = {3,4,5,6,7,8,9,10,11,12,13,1};
	vector<TPokerGroup> vList;
	vList.clear();
	for(int i =0; i < 12; i++)
	{
		int nCount = 0 ;
		int indexM[14] = {0};

		for(int j = 0; j < cardlen; j++)
		{
			if(GetCardLevel(card_list[j]) == GetCardLevel(CardIndex[i]))
			{
				indexM[nCount++] = card_list[j];
			}
		}
		if(nCount >= 4)
		{//是炸弹
			TPokerGroup group;
			group.index.clear();
			for(int k = 0; k < nCount; k++)
			{
				//if(indexM[k] == 0)
				//	continue;
				group.index.push_back(indexM[k]);
			}
			group.count = nCount;
			group.point = indexM[0];
			vList.push_back(group);
		}
	}
	//检查合法性
	int all = 0;
	for(int i = 0; i < (int)vList.size(); i++)
	{
		all += (int)vList[i].count;
	}
	if( all != cardlen)
		return false;
	//检查是不是连炸。。
	//最多连炸数是27/4 = 6
	unsigned char index[6];
	memset(index,0,sizeof(index)); 
	int n = 0;
	for(int i = 0; i < (int)vList.size(); i++)
	{
		index[n++] = vList[i].point;
		if(n >= 6)
			break;
	}
	SortByIndex(index,6);
	if(GetCardLevel(index[0]) + n -1 == GetCardLevel(index[n-1]) )
	{//是连炸，那么找出最多炸弹牌数最多的，且最大的
		int nMaxLong = (int)vList[0].index.size();
		//找出最多的张数是多少？
		for(int i = 0; i < n; i++)
		{
			if((int)vList[i].index.size() >= nMaxLong)
			{
				nMaxLong = (int)vList[i].index.size();
			}
		}
		num = nMaxLong;

		unsigned char MaxIndex[6];
		memset(MaxIndex,0,sizeof(MaxIndex));
		int m = 0;
		for(int i =0; i < n; i++)
		{
			if((int)vList[i].index.size() == nMaxLong)
			{
				MaxIndex[m++] = vList[i].point;
			}
		}
		unsigned char MaxPoint = MaxIndex[0];

		//找出最大的值
		for(int i = 0; i < m; i++)
		{
			if(GetCardLevel(MaxIndex[i]) > GetCardLevel(MaxPoint) )
				MaxPoint = MaxIndex[i];
		}
		card = MaxPoint;
		return true;
	}
	return false;
}
bool CDG4::IsTwoStr(unsigned char card_list[MAXPAI],int cardlen/* = MAXPAI*/)
{
	if(cardlen < 4 || cardlen%2 != 0 || cardlen > 24)
		return false;//最多24张
	//SortByIndex(card_list,cardlen);

	//先找出牌里面是否都是对子
	unsigned char CardIndex[12] = {3,4,5,6,7,8,9,10,11,12,13,1};
	vector<TPokerGroup> vList;
	vList.clear();
	for(int i =0; i < 12; i++)
	{
		int nCount = 0 ;
		int indexM[14] = {0};

		for(int j = 0; j < cardlen; j++)
		{
			if(GetCardLevel(card_list[j]) == GetCardLevel(CardIndex[i]))
			{
				indexM[nCount++] = card_list[j];
			}
		}
		if(nCount == 2)
		{//是对子
			TPokerGroup group;
			group.index.clear();
			for(int k = 0; k < nCount; k++)
			{
				//if(indexM[k] == 0)
				//	continue;
				group.index.push_back(indexM[k]);
			}
			group.count = nCount;
			group.point = indexM[0];
			vList.push_back(group);
		}
	}
	//检查合法性
	int all = 0;
	for(int i = 0; i < (int)vList.size(); i++)
	{
		all += (int)vList[i].count;
	}
	if( all != cardlen)
		return false;

	//检查是不是连对。。
	//3344.....AA,一共是12对
	unsigned char index[12];
	memset(index,0,sizeof(index));
	int n = 0;
	for(int i = 0; i < (int)vList.size(); i++)
	{
		index[n++] = vList[i].point;
		if(n >= 12)
			break;
	}
	SortByIndex(index,12);
	if(GetCardLevel(index[0]) + n -1 == GetCardLevel(index[n-1]) )
	{//是连对，那么返回最大的值
		return true;
	}

	return false;
}
bool CDG4::IsThreeStr(unsigned char card_list[MAXPAI],int cardlen/* = MAXPAI*/)
{
	if(cardlen < 6 || cardlen%3 != 0 || cardlen > 27)
		return false;//最多27张
	//SortByIndex(card_list,cardlen);

	//先找出牌里面是否都是对子
	unsigned char CardIndex[12] = {3,4,5,6,7,8,9,10,11,12,13,1};
	vector<TPokerGroup> vList;
	vList.clear();
	for(int i =0; i < 12; i++)
	{
		int nCount = 0 ;
		int indexM[14] = {0};

		for(int j = 0; j < cardlen; j++)
		{
			if(GetCardLevel(card_list[j]) == GetCardLevel(CardIndex[i]))
			{
				indexM[nCount++] = card_list[j];
			}
		}
		if(nCount == 3)
		{//是三张
			TPokerGroup group;
			group.index.clear();
			for(int k = 0; k < nCount; k++)
			{
				//if(indexM[k] == 0)
				//	continue;
				group.index.push_back(indexM[k]);
			}
			group.count = nCount;
			group.point = indexM[0];
			vList.push_back(group);
		}
	}
	//检查合法性
	int all = 0;
	for(int i = 0; i < (int)vList.size(); i++)
	{
		all += (int)vList[i].count;
	}
	if( all != cardlen)
		return false;

	//检查是不是连三对。。
	//最多可以有27张,9对
	unsigned char index[9];
	memset(index,0,sizeof(index));
	int n = 0;
	for(int i = 0; i < (int)vList.size(); i++)
	{
		index[n++] = vList[i].point;
		if(n >= 9)
			break;
	}
	SortByIndex(index,9);
	if(GetCardLevel(index[0]) + n -1 == GetCardLevel(index[n-1]) )
	{//是连对，那么返回最大的值
		return true;
	}

	return false;
}
int CDG4::GetCardLevel(unsigned char card_id)
{
	int level = 0;
	int tmpid = card_id - 1;
	level = tmpid%13+1 + 100;
	if(card_id%13==1) //Ａ
		level = 114;
	if(card_id%13==2) //2
		level = 116;
	if(card_id==SMA_KING)  //小王
		level = 118;
	if(card_id==BIG_KING)  //大王
		level = 120;
	if(card_id > 55)   //未知牌
		level = 200;
	if(card_id==0)   //无牌
		level = 0;

	return level;
}
void CDG4::SortByIndex(unsigned char card_list[MAXPAI],int cardlen /* = MAX_CARD_NUM */,bool smalltobig/*=true*/)
{
	if(smalltobig)
	{
		for(int i = 0; i < cardlen-1; i++)
		{
			for(int j = i+1;j < cardlen; j++)
			{
				if(card_list[j] > 0)
				{
					if(GetCardLevel(card_list[i]) > GetCardLevel(card_list[j]))
					{
						unsigned char temp = card_list[i];
						card_list[i] = card_list[j];
						card_list[j] = temp;
					}
					else if(GetCardLevel(card_list[i]) == GetCardLevel(card_list[j]))
					{
						if(card_list[i] > card_list[j])
						{
							unsigned char temp = card_list[i];
							card_list[i] = card_list[j];
							card_list[j] = temp;
						}
					}
				}
			}
		}
	}
	else
	{
		for(int i = 0; i < cardlen-1; i++)
		{
			for(int j = i+1;j < cardlen; j++)
			{
				if(card_list[j] > 0)
				{
					if(GetCardLevel(card_list[i]) < GetCardLevel(card_list[j]))
					{
						unsigned char temp = card_list[i];
						card_list[i] = card_list[j];
						card_list[j] = temp;
					}
					else if(GetCardLevel(card_list[i]) == GetCardLevel(card_list[j]))
					{
						if(card_list[i] < card_list[j])
						{
							unsigned char temp = card_list[i];
							card_list[i] = card_list[j];
							card_list[j] = temp;
						}
					}
				}
			}
		}
	}
}
void CDG4::SortByCount(unsigned char card_list[MAXPAI],int cardlen /* = MAXPAI */)
{
	if(cardlen <= 1 || cardlen > MAXPAI)
		return;

	vector<TPokerGroup> vClassList[9];//0放王，1-8放1-8张牌的
	for(int i = 0; i < 9;i ++)
	{
		vClassList[i].clear();
	}

	TPokerGroup group;
	group.point = 0;
	group.index.clear();
	group.count = 0;

	unsigned char pai[MAXPAI];
	memset(pai,0,sizeof(pai));

	SortByIndex(card_list,cardlen);
	unsigned char PointIndex[15] = {CP_3_S,CP_4_S,CP_5_S,CP_6_S,CP_7_S,CP_8_S,CP_9_S,CP_10_S,CP_J_S,CP_Q_S,CP_K_S,CP_A_S,CP_2_S,CARDINDEX_SMALL,CARDINDEX_BIG};
	
	for(int i = 0; i < 15; i++)
	{
		int num = 0 ;
		int indexM[8];
		memset(indexM,0,sizeof(indexM));
		
		if(i == 13)
			int a = 0;
		for(int j = 0; j < MAXPAI; j++)
		{
			if(GetCardPoint(card_list[j]) == GetCardPoint(PointIndex[i]) )
			{
				if(num >= 8)
					break;
				indexM[num] = card_list[j];
				num++;
			}
		}
		if(num > 0)
		{
			group.point = indexM[0];
			group.count = num;
			group.index.clear();
			for(int k = 0; k < num; k++)
			{
				if(indexM[k] == 0)
					continue;
				group.index.push_back(indexM[k]);
			}
			if(PointIndex[i] != CARDINDEX_SMALL && PointIndex[i] != CARDINDEX_BIG )
				vClassList[num].push_back(group);
			else
			{
				vClassList[0].push_back(group);//放王的
			}
		}
	}
	int count = 0;
	//先放王的
	for(int i = (int)vClassList[0].size()-1; i >= 0; i--)
	{
		TPokerGroup Temp = vClassList[0][i];
		for(int j = 0; j < (int)Temp.index.size(); j++)
			pai[count++] = Temp.index[j];
	}
	//然后依次从多到少放
	for(int k = 8; k >= 1; k--)
	{
		for(int i = (int)vClassList[k].size()-1; i >=0 ; i--)
		{
			TPokerGroup Temp = vClassList[k][i];
			for(int j =(int)Temp.index.size()-1; j >= 0 ;j--)
			{
				pai[count++] = Temp.index[j];
			}
		}
	}
	for(int i = 0; i < MAXPAI; i++)
	{
		card_list[i] = pai[i];
	}
}
unsigned char CDG4::GetCardNum(unsigned char card_list[MAXPAI],int cardlen /* = MAXPAI */)
{
	int iNum = 0;
	for(int i=0;i< cardlen && i< MAXPAI; i++)
	{
		if(card_list[i] > 0 && card_list[i] <= 55)
			iNum ++;
	}
	return iNum;
}
int CDG4::Get7XibombNum(unsigned char card_list[MAXPAI])
{
	int nCount = 0;
	for(int i = 1; i <= 13; i++)
	{
		int num = 0;
		for(int j = 0; j < MAXPAI; j++)
		{
			if(GetCardLevel(card_list[j]) == GetCardLevel(i))
			{
				num++;
				//if(num == 7)//这里如果break的话，会把8喜也算成7喜
				//	break;
			}
		}
		if(num == 7)
		{	
			nCount++;
		}
	}
	return nCount;
}
//得到8喜炸弹个数
int CDG4::Get8XibombNum(unsigned char card_list[MAXPAI])
{
	int nCount = 0;
	for(int i = 1; i <= 13; i++)
	{
		int num = 0;
		for(int j = 0; j < MAXPAI; j++)
		{
			if(GetCardLevel(card_list[j]) == GetCardLevel(i))
			{
				num++;
				if(num == 8)
					break;
			}
		}
		if(num == 8)
		{	
			nCount++;
		}
	}
	return nCount;
}
//是否存在天炸？
bool CDG4::HasKingBomb(unsigned char card_list[MAXPAI])
{
	int num = 0;
	for(int i = 0; i < MAXPAI; i++)
	{
		if(IsKing(card_list[i]))
		{
			num++;
			if(num == 4)
				break;
		}
	}
	return num == 4 ? true:false;
}
FakeType CDG4::GetTypeByKing(unsigned char card_list[MAXPAI],int cardlen,int kingnum,TFakePoker fakepoker[4],int outtype/*=TYPE_NULL*/)
{
	FakeType reType;
	memset(reType.fakeking,0,sizeof(reType.fakeking));
	if(kingnum > 0)
	{//有1-4个王
		unsigned char templist[MAXPAI],templist2[MAXPAI];
		memset(templist,0,sizeof(templist));
		memset(templist2,0,sizeof(templist2));
		for(int i = 0; i < cardlen; i++)
		{//去除王
			if(!IsKing(card_list[i]))
				templist[i] = card_list[i];
		}
		SortByIndex(templist);
		vector<FakeType> vTypeList;

		if(kingnum == 1)
		{//1 king，把这个牌当1-13任何牌做进去，
			vTypeList.clear();
			for(int i = 1; i <= 13; i++)
			{
				memcpy(templist2,templist,sizeof(templist2));
				templist2[cardlen-kingnum] = i;
				SortByIndex(templist2);
				FakeType temptype;

				memset(&temptype.fakeking,0,sizeof(temptype.fakeking));
				temptype.cardtype.card = templist2[cardlen-1];
				if(IsTwoStr(templist2,cardlen))
				{
					reType.cardtype.card = temptype.cardtype.card;
					reType.fakeking[0].fakeindex = i;
					reType.fakeking[0].index = fakepoker[0].index;
					reType.cardtype.type = TYPE_TWOSTR;
					vTypeList.push_back(reType);
				}
				else if(IsThreeStr(templist2,cardlen))
				{
					reType.cardtype.card = temptype.cardtype.card;
					reType.fakeking[0].fakeindex = i;
					reType.fakeking[0].index = fakepoker[0].index;
					reType.cardtype.type = TYPE_THREESTR;
					vTypeList.push_back(reType);
				}
			}
		}
		else if(kingnum == 2)
		{//2 kings
			vTypeList.clear();
			for(int i = 1; i <= 13; i++)
			{
				for(int j = 1; j <= 13; j++)
				{
					memcpy(templist2,templist,sizeof(templist2));
					templist2[cardlen-kingnum] = i;
					templist2[cardlen-kingnum+1] = j;
					SortByIndex(templist2);
					FakeType temptype;
					memset(&temptype.fakeking,0,sizeof(temptype.fakeking));
					temptype.cardtype.card = templist2[cardlen-1];
					if(IsTwoStr(templist2,cardlen))
					{
						reType.cardtype.card = temptype.cardtype.card;
						reType.fakeking[0].fakeindex = i;
						reType.fakeking[0].index = fakepoker[0].index;
						reType.fakeking[1].fakeindex = j;
						reType.fakeking[1].index = fakepoker[1].index;
						reType.cardtype.type = TYPE_TWOSTR;
						vTypeList.push_back(reType);
					}
					else if(IsThreeStr(templist2,cardlen))
					{
						reType.cardtype.card = temptype.cardtype.card;
						reType.fakeking[0].fakeindex = i;
						reType.fakeking[0].index = fakepoker[0].index;
						reType.fakeking[1].fakeindex = j;
						reType.fakeking[1].index = fakepoker[1].index;
						reType.cardtype.type = TYPE_THREESTR;
						vTypeList.push_back(reType);
					}
				}
			}
		}
		else if(kingnum == 3)
		{
			vTypeList.clear();
			for(int i = 1; i <= 13; i++)
			{
				for(int j = 1; j <= 13; j++)
				{
					for(int k = 1; k <= 13; k++)
					{
						memcpy(templist2,templist,sizeof(templist2));
						templist2[cardlen-kingnum] = i;
						templist2[cardlen-kingnum+1] = j;
						templist2[cardlen-kingnum+2] = k;
						SortByIndex(templist2);
						FakeType temptype;
						memset(&temptype.fakeking,0,sizeof(temptype.fakeking));
						temptype.cardtype.card = templist2[cardlen-1];
						if(IsTwoStr(templist2,cardlen))
						{
							reType.cardtype.card = temptype.cardtype.card;
							reType.fakeking[0].fakeindex = i;
							reType.fakeking[0].index = fakepoker[0].index;
							reType.fakeking[1].fakeindex = j;
							reType.fakeking[1].index = fakepoker[1].index;
							reType.fakeking[2].fakeindex = k;
							reType.fakeking[2].index = fakepoker[2].index;
							reType.cardtype.type = TYPE_TWOSTR;
							vTypeList.push_back(reType);
						}
						else if(IsThreeStr(templist2,cardlen))
						{
							reType.cardtype.card = temptype.cardtype.card;
							reType.fakeking[0].fakeindex = i;
							reType.fakeking[0].index = fakepoker[0].index;
							reType.fakeking[1].fakeindex = j;
							reType.fakeking[1].index = fakepoker[1].index;
							reType.fakeking[2].fakeindex = k;
							reType.fakeking[2].index = fakepoker[2].index;
							reType.cardtype.type = TYPE_THREESTR;
							vTypeList.push_back(reType);
						}
					}
				}
			}
		}
		else if(kingnum == 4)
		{
			vTypeList.clear();
			for(int i = 1; i <= 13; i++)
			{
				for(int j = 1; j <= 13; j++)
				{
					for(int k = 1; k <= 13; k++)
					{
						for(int v = 1; v <= 13; v++)
						{
							memcpy(templist2,templist,sizeof(templist2));
							templist2[cardlen-kingnum] = i;
							templist2[cardlen-kingnum+1] = j;
							templist2[cardlen-kingnum+2] = k;
							templist2[cardlen-kingnum+3] = v;
							SortByIndex(templist2);
							FakeType temptype;
							memset(&temptype.fakeking,0,sizeof(temptype.fakeking));
							temptype.cardtype.card = templist2[cardlen-1];
							if(IsTwoStr(templist2,cardlen))
							{
								reType.cardtype.card = temptype.cardtype.card;
								reType.fakeking[0].fakeindex = i;
								reType.fakeking[0].index = fakepoker[0].index;
								reType.fakeking[1].fakeindex = j;
								reType.fakeking[1].index = fakepoker[1].index;
								reType.fakeking[2].fakeindex = k;
								reType.fakeking[2].index = fakepoker[2].index;
								reType.fakeking[3].fakeindex = v;
								reType.fakeking[3].index = fakepoker[3].index;
								reType.cardtype.type = TYPE_TWOSTR;
								vTypeList.push_back(reType);
							}
							else if(IsThreeStr(templist2,cardlen))
							{
								reType.cardtype.card = temptype.cardtype.card;
								reType.fakeking[0].fakeindex = i;
								reType.fakeking[0].index = fakepoker[0].index;
								reType.fakeking[1].fakeindex = j;
								reType.fakeking[1].index = fakepoker[1].index;
								reType.fakeking[2].fakeindex = k;
								reType.fakeking[2].index = fakepoker[2].index;
								reType.fakeking[3].fakeindex = v;
								reType.fakeking[3].index = fakepoker[3].index;
								reType.cardtype.type = TYPE_THREESTR;
								vTypeList.push_back(reType);
							}
						}
					}
				}
			}
		}
		else
		{
			vTypeList.clear();
		}
		if((int)vTypeList.size() > 0)
		{
			FakeType max_re;
			memset(&max_re.fakeking,0,sizeof(max_re.fakeking));
			if(outtype == TYPE_THREESTR || outtype == TYPE_TWOSTR)
			{//说明手先出的人已经指定了出牌类型，那么跟的就是出牌类型
				int first = 0;
				for(int i = 0; i < (int)vTypeList.size(); i++)
				{
					if(vTypeList[i].cardtype.type == outtype)
					{
						max_re = vTypeList[i];
						first = i;
						break;
					}
				}
				for(int i = first; i < (int)vTypeList.size(); i++)
				{
					if(vTypeList[i].cardtype.type == max_re.cardtype.type)
					{
						if(Compare(max_re.cardtype,vTypeList[i].cardtype))
						{
							max_re = vTypeList[i];
						}
					}
				}
			}
			else
			{
				max_re = vTypeList[0];
				for(int i = 0; i < (int)vTypeList.size(); i++)
				{
					if(vTypeList[i].cardtype.type > max_re.cardtype.type)
					{
						max_re = vTypeList[i];
					}
					else if(vTypeList[i].cardtype.type == max_re.cardtype.type)
					{
						if(Compare(max_re.cardtype,vTypeList[i].cardtype))
						{
							max_re = vTypeList[i];
						}
					}
				}
			}
			
			reType = max_re;
		}
		else
		{
			reType.cardtype.type = TYPE_ERROR;
		}
	}
	else
	{
		reType.cardtype.type = TYPE_ERROR;
	}
	return reType;
}
TCardType CDG4::GetType(unsigned char card_list[MAXPAI],int cardlen/* =MAXPAI */,int outtype/* = TYPE_NULL*/,TFakePoker* refakepoker/* = NULL*/)
{
	TCardType re;	
	re.len	= 0;
	re.card	= 0;
	re.color= 0;
	re.type	= TYPE_NULL;
	re.count= 0;
	SortByIndex(card_list,cardlen);
	re.len	= GetCardNum(card_list,cardlen);
	re.card = card_list[re.len-1];
	unsigned char color = 0;

	switch(re.len)
	{
	case 0:
		re.type = TYPE_NULL;
		return re;
	case 1:
		re.type = TYPE_ONE;
		if(IsKing(re.card))
			re.card = 3;//王单出算3
		return re;
	case 2:
		if( !IsKing(card_list[0]) && !IsKing(card_list[1]) )
		{//没有大小王
			if(GetCardLevel(card_list[0]) == GetCardLevel(card_list[1]))
				re.type = TYPE_TWO;
			else
				re.type = TYPE_ERROR;
		}
		else if(IsKing(card_list[0]) && IsKing(card_list[1]))
		{//全是王
			re.card = 3;//大小王按3来算
			re.type = TYPE_TWO;
		}
		else
		{//有一张王
			re.card = IsKing(card_list[0]) ? card_list[1] : card_list[0];
			re.type = TYPE_TWO;
		}
		return re;
	case 3:
		if(Is510KBomb(color,card_list,3))
		{
			re.color = color;
			re.type = TYPE_BOMB_510K;
		}
		else if( !IsKing(card_list[0]) && !IsKing(card_list[1]) && !IsKing(card_list[2]))
		{//没有大小王
			if (GetCardLevel(card_list[0])==GetCardLevel(card_list[1]) 
				&& GetCardLevel(card_list[0]) ==GetCardLevel(card_list[2])) 
				re.type = TYPE_THREE;
			else
				re.type = TYPE_ERROR;
		}
		else  if( IsKing(card_list[0]) && IsKing(card_list[1]) && IsKing(card_list[2]))
		{//全是王
			re.card = 3;//大小王按3来算
			re.type = TYPE_THREE;
		}
		else
		{//有小于三张的王
			if(IsAllEqualExceptKing(re.card,card_list,3))
			{
				re.type = TYPE_THREE;
			}
			else
			{
				re.type = TYPE_ERROR;
			}
		}
		return re;
	default:// >= 4张
		if(re.len == 4)
		{
			unsigned char card = 0;
			if(IsKingBomb(card_list,cardlen))
			{//天炸
				re.type = TYPE_BOMB_KING;
			}
			else if(IsNormalBomb(card,card_list,cardlen))
			{//普通4炸弹
				re.card = card;
				re.type = TYPE_BOMB_NOMORL;
			}
			else
			{
				if(IsTwoStr(card_list,cardlen))
				{
					re.type = TYPE_TWOSTR;
				}
				else
				{
					TFakePoker fakepoker[4];
					memset(fakepoker,0,sizeof(fakepoker));

					int kingnum = GetKingNum(card_list,cardlen,fakepoker);
					if(kingnum > 0)
					{//有1-4个王
						FakeType ty = GetTypeByKing(card_list,cardlen,kingnum,fakepoker,outtype);
						re.type = ty.cardtype.type;
						re.card = ty.cardtype.card;
						if(refakepoker != NULL)
						{
							for(int i = 0; i < 4; i++)
							{
								refakepoker[i] = ty.fakeking[i];
							}
						}
					}
					else
					{
						re.type = TYPE_ERROR;
					}
				}
			}
			return re;
		}
		else if(re.len > 4 && re.len < 8)
		{//只能是普通炸弹或者7喜炸弹
			unsigned char card = 0;
			if(Is7XiBomb(card_list,cardlen))
			{
				re.type = TYPE_BOMB_7XI;
			}
			else if(IsNormalBomb(card,card_list,cardlen))
			{
				re.card = card;
				re.type = TYPE_BOMB_NOMORL;
			}
			else 
			{//在这里，就要判断是否有王了
				TFakePoker fakepoker[4];
				memset(fakepoker,0,sizeof(fakepoker));
				int kingnum = GetKingNum(card_list,cardlen,fakepoker);
				if(kingnum > 0)
				{//有1-4个王
					FakeType ty = GetTypeByKing(card_list,cardlen,kingnum,fakepoker,outtype);
					re.type = ty.cardtype.type;
					re.card = ty.cardtype.card;
					if(refakepoker != NULL)
					{
						for(int i = 0; i < 4; i++)
						{
							refakepoker[i] = ty.fakeking[i];
						}
					}
				}
				else
				{//没有王
					if(IsTwoStr(card_list,cardlen))
					{
						re.type = TYPE_TWOSTR;
					}
					else if(IsThreeStr(card_list,cardlen))
					{
						re.type = TYPE_THREESTR;
					}
					else
					{//错误
						re.type = TYPE_ERROR;
					}
				}
			}
			return re;
		}
		else if(re.len >= 8)
		{//8喜或者普通炸弹，或者连炸
			unsigned char card = 0;
			unsigned char maxLong = 0;
			if(Is8XiBomb(card_list,cardlen))
			{
				re.type = TYPE_BOMB_8XI;
			}
			else if(IsNormalBomb(card,card_list,cardlen))
			{
				re.card = card;
				re.type = TYPE_BOMB_NOMORL;
			}
			else if(IsBombStr(card,maxLong,card_list,cardlen))
			{
				re.card = card;
				re.count = maxLong;
				re.type = TYPE_BOMB_STR;
			}
			else
			{//考虑有王的情况下的2连对或者3连对
				TFakePoker fakepoker[4];
				memset(fakepoker,0,sizeof(fakepoker));
				int kingnum = GetKingNum(card_list,cardlen,fakepoker);
				if(kingnum > 0)
				{//有1-4个王
					FakeType ty = GetTypeByKing(card_list,cardlen,kingnum,fakepoker,outtype);
					re.type = ty.cardtype.type;
					re.card = ty.cardtype.card;
					if(refakepoker != NULL)
					{
						for(int i = 0; i < 4; i++)
						{
							refakepoker[i] = ty.fakeking[i];
						}
					}
				}
				else
				{//没有王
					if(IsTwoStr(card_list,cardlen))
					{
						re.type = TYPE_TWOSTR;
					}
					else if(IsThreeStr(card_list,cardlen))
					{
						re.type = TYPE_THREESTR;
					}
					else
					{//错误
						re.type = TYPE_ERROR;
					}
				}
			}
			return re;
		}
	}
	return re;
}
int CDG4::GetScore(unsigned char card_list[MAXPAI],int cardlen /* = MAXPAI */)
{
	int score = 0;
	for(int i=0;i<cardlen;i++)
	{
		if(GetCardLevel(card_list[i]) == GetCardLevel(5))
			score += 5;
		else if(GetCardLevel(card_list[i]) == GetCardLevel(10))
			score += 10;
		else if(GetCardLevel(card_list[i]) == GetCardLevel(13))
			score += 10;
	}
	return score;
}
bool CDG4::CanOut(unsigned char outCard[MAXPAI],unsigned char followCard[MAXPAI],int outtype,int& followtype,bool isfirst/* = false*/,TFakePoker* fakepoker/* = NULL*/)
{
	int len1 = GetCardNum(outCard);
	TCardType cardtype1 = GetType(outCard,len1);
	int len2 = GetCardNum(followCard);
	TCardType cardtype2 = GetType(followCard,len2,outtype/*cardtype1.type*/,fakepoker);
	
	followtype = 0;
//	type = followtype.type;//得到出的牌的type;
	if(isfirst)
	{
		if(cardtype2.type != TYPE_ERROR && cardtype2.type != TYPE_NULL)
		{
			followtype = cardtype2.type;
			return true;
		}
		else
			return false;
	}
	cardtype1.type = outtype;//cardtype1.type得出来的不一定正确
	if(Compare(cardtype1,cardtype2))
	{
		followtype = cardtype2.type;
		return true;
	}
	return false;
}
//比较大小， typeFollow > typeFirst 返回真
bool CDG4::Compare( TCardType typeFirst,TCardType typeFollow )
{
	if(typeFirst.type == TYPE_BOMB_KING) return false;//天炸直接返回
	if(typeFollow.type == TYPE_BOMB_KING) return true;//天炸直接返回

	if(typeFirst.type == TYPE_ERROR) return false;

	if(typeFollow.type == TYPE_ERROR || typeFollow.type == TYPE_NULL) return false;

	if(typeFirst.type == TYPE_NULL)
	{
		if(typeFollow.type == TYPE_ONE
			|| typeFollow.type == TYPE_TWO
			|| typeFollow.type == TYPE_THREE
			|| typeFollow.type == TYPE_TWOSTR
			|| typeFollow.type == TYPE_THREESTR
			|| typeFollow.type == TYPE_BOMB_510K
			|| typeFollow.type == TYPE_BOMB_NOMORL
			|| typeFollow.type == TYPE_BOMB_STR
			|| typeFollow.type == TYPE_BOMB_7XI
			|| typeFollow.type == TYPE_BOMB_8XI
			)
			return true;
		else
			return false;
	}
	else if(typeFirst.type == TYPE_BOMB_NOMORL)
	{//普通的炸弹
		if(typeFollow.type == TYPE_BOMB_7XI
			|| typeFollow.type == TYPE_BOMB_8XI
			|| typeFollow.type == TYPE_BOMB_STR )
		{//8喜，7喜，连炸，都大于普通的炸弹
			return true;
		}
		else if(typeFollow.type == typeFirst.type)
		{//普通的炸弹，那么比张数和点数
			if(typeFollow.len > typeFirst.len)
				return true;//张数多
			else if(typeFollow.len == typeFirst.len)
			{//张数一样，比点数
				if(GetCardLevel(typeFollow.card) > GetCardLevel(typeFirst.card))
					return true;
				else
					return false;
			}
			else
			{//张数少
				return false;
			}
		}
		else
			return false;
	}
	else if(typeFirst.type == TYPE_BOMB_510K)
	{// 510k的炸弹
		if(typeFollow.type == TYPE_BOMB_7XI
			|| typeFollow.type == TYPE_BOMB_8XI
			|| typeFollow.type == TYPE_BOMB_STR 
			|| typeFollow.type == TYPE_BOMB_NOMORL)
		{//8喜，7喜，连炸，普通炸弹都大于510k炸弹
			return true;
		}
		else if(typeFollow.type == typeFirst.type)
		{//跟的牌也是510k?那么，比花色
			if(typeFollow.color > typeFirst.color)
				return true;
			else
				return false;
		}
		else
			return false;
	}
	else if(typeFirst.type == TYPE_BOMB_STR)
	{//连炸
		if(/*typeFollow.type == TYPE_BOMB_7XI
			|| */typeFollow.type == TYPE_BOMB_8XI)
		{//8喜大于连炸
			return true;
		}
		else if(typeFollow.type == typeFirst.type)
		{//同样是连炸，那么先比较张数
			if(typeFollow.len > typeFirst.len)
			{
				return true;
			}
			else if(typeFollow.len == typeFirst.len)
			{//比较连炸中，最大的炸弹张数
				if(typeFollow.count > typeFirst.count)
					return true;
				else if(typeFollow.count == typeFirst.count)
				{
					if(GetCardLevel(typeFollow.card) > GetCardLevel(typeFirst.card))
						return true;
					else
						return false;
				}
				else
					return false;
			}
			else
				return false;
		}
	}
	else if(typeFirst.type == TYPE_BOMB_7XI)
	{//7喜
		if(typeFollow.type == TYPE_BOMB_8XI || typeFollow.type == TYPE_BOMB_STR)
		{//8喜,连炸>7喜
			return true;
		}
		else if(typeFollow.type == typeFirst.type)
		{//比大小
			if(GetCardLevel(typeFollow.card) > GetCardLevel(typeFirst.card))
				return true;
			else
				return false;
		}
		else 
			return false;
	}
	else if(typeFirst.type == TYPE_BOMB_8XI)
	{//8喜
		if(typeFollow.type == typeFirst.type)
		{//比大小
			if(GetCardLevel(typeFollow.card) > GetCardLevel(typeFirst.card))
				return true;
			else
				return false;
		}
		else 
			return false;
	}
	else
	{//其他牌的比较，非炸弹
		if(typeFollow.type == TYPE_BOMB_510K
			|| typeFollow.type == TYPE_BOMB_NOMORL
			|| typeFollow.type == TYPE_BOMB_STR
			|| typeFollow.type == TYPE_BOMB_7XI
			|| typeFollow.type == TYPE_BOMB_8XI
			)
			return true;
		else if(typeFollow.type == typeFirst.type)
		{
			if(typeFollow.len == typeFirst.len && GetCardLevel(typeFollow.card) > GetCardLevel(typeFirst.card))
				return true;
			else
				return false;
		}
		else
			return false;
	}
	return false;
}

int CDG4::CheckBeep( unsigned char card_list[MAXPAI],unsigned char outCard[MAXPAI],int outlen/*=MAXPAI*/ )
{
	//if(GetCardNum(card_list) < 4)
	//	return 0;
	if(outlen <= 0)
		return 0;
	int num = 0;
	int index[15],outindex[15];
	memset(index,0,sizeof(index));
	memset(outindex,0,sizeof(outindex));

	for(int i = 1; i <= 15; i++)
	{
		for(int j = 0; j < MAXPAI; j++)
		{
			if(card_list[j] >0 && card_list[j] < 55)
			{
				TPoker poker;
				poker.Set(card_list[j]);
				if(poker.point == i)
				{
					index[i-1]++;
				}
			}
		}
	}
	for(int i = 1; i <= 15; i++)
	{
		for(int j = 0; j < outlen; j++)
		{
			if(outCard[j] >0 && outCard[j] < 55)
			{
				TPoker poker;
				poker.Set(outCard[j]);
				if(poker.point == i)
				{
					outindex[i-1]++;
				}
			}
		}
	}
	bool isKingBeep = false;
	bool is7XiBeep  = false;
	bool is8XiBeep  = false;

	if(outlen < 7)
	{//出的牌小于7张
		for(int i = 0; i < 13; i++)
		{
			if(outindex[i] + index[i] == 7 && outindex[i] > 0 && !is7XiBeep)
			{//7喜
				is7XiBeep = true;
			}
			else if(outindex[i] + index[i] == 8 && outindex[i] > 0 && !is8XiBeep)
			{
				is8XiBeep = true;
			}
		}
		if(outlen < 4)
		{
			if(outindex[13] + outindex[14] + index[13] + index[14] == 4
				&& outindex[13] + outindex[14] > 0 && !isKingBeep)
			{
				isKingBeep = true;
			}
		}
		else if(outlen == 4)
		{
			if(outindex[13] + outindex[14] + index[13] + index[14] == 4
				&& outindex[13] + outindex[14] != 4 && outindex[13] + outindex[14] > 0 && !isKingBeep)
			{
				isKingBeep = true;
			}
		}
		else
		{
			if(outindex[13] + outindex[14] + index[13] + index[14] == 4
				&& outindex[13] + outindex[14] > 0 && !isKingBeep)
			{
				isKingBeep = true;
			}
		}
	}
	else if(outlen == 7)
	{
		for(int i = 0; i < 13; i++)
		{
			if(outindex[i] + index[i] == 7 && outindex[i] != 7 && outindex[i] > 0 && !is7XiBeep)
			{//7喜
				is7XiBeep = true;
			}
			else if(outindex[i] + index[i] == 8 && outindex[i] > 0 && !is8XiBeep)
			{
				is8XiBeep = true;
			}
		}
	}
	else if(outlen == 8)
	{
		for(int i = 0; i < 13; i++)
		{
			if(outindex[i] + index[i] == 7 && outindex[i] != 7 && outindex[i] > 0 && !is7XiBeep)
			{//7喜
				is7XiBeep = true;
			}
			else if(outindex[i] + index[i] == 8 && outindex[i] != 8 && outindex[i] > 0 && !is8XiBeep)
			{
				is8XiBeep = true;
			}
		}
	}
	else if(outlen > 8)
	{
		for(int i = 0; i < 13; i++)
		{
			if(outindex[i] + index[i] == 7 && outindex[i] >0 && !is7XiBeep)
			{//7喜
				is7XiBeep = true;
			}
			else if(outindex[i] + index[i] == 8 && outindex[i] > 0 && !is8XiBeep)
			{
				is8XiBeep = true;
			}
		}
	}
	if(isKingBeep)
		return 3;
	if(is8XiBeep)
		return 2;
	if(is7XiBeep)
		return 1;
	return 0;
}
///////////////////////////////////////////////////////
void CDG4::GetGroupType( TCARDS mycard )
{
	m_vOne.clear();
	m_vTwo.clear();
	m_vThree.clear();
	m_vFour.clear();
	m_vFive.clear();
	m_vSix.clear();
	m_vSeven.clear();
	m_vEight.clear();
	m_vKing.clear();
	m_vK105.clear();
	TPoker tempPoker[MAXPAI];
	memset(tempPoker,0,sizeof(tempPoker));
	for(int i = 0; i < mycard.size(); i++)
	{
		tempPoker[i].Set(mycard[i]);
	}
	TPokerGroup2 group;
	group.point = 0;
	group.index.clear();
	group.count = 0;
	group.color = 0;
	//首先找王
	for(int i = 14;i<= 15;i++)
	{
		for(int j = 0; j < MAXPAI; j++)
		{
			if(tempPoker[j].point == i)
			{
				group.point = tempPoker[j].point;
				group.index.clear();
				group.index.push_back(tempPoker[j].index);
				group.count = 1;
				group.color = 0;
				m_vKing.push_back(group);
			}
		}
	}
	//找其他的
	int pokerIndex[13]={3,4,5,6,7,8,9,10,11,12,13,1,2};
	
	for(int i = 0; i < 13; i++)
	{
		int num = 0 ;
		int indexM[10] = {0,0,0,0,0,0,0,0,0,0};
		
		for(int j = 0; j < MAXPAI; j++)
		{
			if(tempPoker[j].point == pokerIndex[i])
			{
				indexM[num] = tempPoker[j].index;
				num++;
				if(pokerIndex[i] == 5 || pokerIndex[i] == 10 || pokerIndex[i] == 13)
				{
					group.point = pokerIndex[i];
					group.color = tempPoker[j].color;
					group.count = 1;
					group.index.clear();
					group.index.push_back(tempPoker[j].index);
					m_vK105.push_back(group);
				}
			}
		}
		group.point = pokerIndex[i];
		group.color = 0;//只针对k105
		group.count = num;
		group.index.clear();
		for(int k = 0; k < 8; k++)
		{
			if(indexM[k] == 0)
				continue;
			group.index.push_back(indexM[k]);
		}
		group.point = pokerIndex[i];
		if(num == 8) m_vEight.push_back(group);
		else if(num == 7) m_vSeven.push_back(group);
		else if(num == 6) m_vSix.push_back(group);
		else if(num == 5) m_vFive.push_back(group);
		else if(num == 4) m_vFour.push_back(group);
		else if(num == 3) m_vThree.push_back(group);
		else if(num == 2) m_vTwo.push_back(group);
		else if(num == 1) m_vOne.push_back(group);
	}
}

bool CDG4::BeepFirstCardOut(vector<TPokerGroup2>& mybeepout)
{
	TPokerGroup2 group;
	mybeepout.clear();
	
	vector<TPokerGroup2> templist;
	templist.clear();

	for(vector<TPokerGroup2>::iterator it = m_vOne.begin(); it != m_vOne.end(); it++)
	{//1
		group.index.clear();
		group.count = 1;
		group.point = it->point;
		group.index = it->index;
		templist.push_back(group);
	}
	for(vector<TPokerGroup2>::iterator it = m_vTwo.begin(); it != m_vTwo.end(); it++)
	{//2
		group.index.clear();
		group.count = 2;
		group.point = it->point;
		group.index = it->index;
		templist.push_back(group);
	}
	for(vector<TPokerGroup2>::iterator it = m_vThree.begin(); it != m_vThree.end(); it++)
	{//3
		group.index.clear();
		group.count = 3;
		group.point = it->point;
		group.index = it->index;
		templist.push_back(group);
	}
	//3个以内的，都用最小的提示
	
	int size = (int)templist.size();
	for(int i =0; i < size-1; i++)
	{
		for(int j = i+1; j < size; j++)
		{
			if(GetCardLevel(templist[i].point) > GetCardLevel(templist[j].point) && templist[j].point > 0)
			{
				TPokerGroup2 temp = templist[i];
				templist[i] = templist[j];
				templist[j] = temp;
			}
		}
	}
	if(size > 0)
	{
		TPokerGroup2 temp = templist[0];
		mybeepout.push_back(temp);
		return true;
	}
	mybeepout.clear();
	for(vector<TPokerGroup2>::iterator it = m_vFour.begin(); it != m_vFour.end(); it++)
	{//4
		group.index.clear();
		group.count = 4;
		group.point = it->point;
		group.index = it->index;
		mybeepout.push_back(group);
		return true;
	}
	for(vector<TPokerGroup2>::iterator it = m_vFive.begin(); it != m_vFive.end(); it++)
	{//5
		group.index.clear();
		group.count = 5;
		group.point = it->point;
		group.index = it->index;
		mybeepout.push_back(group);
		return true;
	}
	for(vector<TPokerGroup2>::iterator it = m_vSix.begin(); it != m_vSix.end(); it++)
	{
		group.index.clear();
		group.count = 6;
		group.point = it->point;
		group.index = it->index;
		mybeepout.push_back(group);
		return true;
	}
	for(vector<TPokerGroup2>::iterator it = m_vSeven.begin(); it != m_vSeven.end(); it++)
	{
		group.index.clear();
		group.count = 7;
		group.point = it->point;
		group.index = it->index;
		mybeepout.push_back(group);
		return true;
	}
	for(vector<TPokerGroup2>::iterator it = m_vEight.begin(); it != m_vEight.end(); it++)
	{
		group.index.clear();
		group.count = 8;
		group.point = it->point;
		group.index = it->index;
		mybeepout.push_back(group);
		return true;
	}

	for(vector<TPokerGroup2>::iterator it = m_vKing.begin(); it != m_vKing.end(); it++)
	{
		group.index.clear();
		group.count = 1;
		group.point = it->point;
		group.index = it->index;
		mybeepout.push_back(group);
		return true;
	}		
	return false;
}
bool CDG4::BeepCardOut( unsigned char otherout[MAXPAI],int outtype,vector<TPokerGroup2>& mybeepout )
{
	mybeepout.clear();
	TPokerGroup2 group;
	group.color = 0;
	int len1 = GetCardNum(otherout);
	TCardType cardtype1 = GetType(otherout,len1);
	
	cardtype1.type = outtype;//重新设置下

	int outlevel = GetCardLevel(cardtype1.card);
	if(outtype == TYPE_NULL || outtype == TYPE_ERROR)
		return false;//不需要提示
	if(outtype == TYPE_BOMB_KING)
	{//天炸最大
		return false;
	}
	else if(outtype == TYPE_BOMB_8XI)
	{//天炸，或者点数大的8喜
		for(vector<TPokerGroup2>::iterator it = m_vEight.begin(); it != m_vEight.end(); it++)
		{
			if(GetCardLevel(it->point) > outlevel)
			{//找到一个大的
				group.index = it->index;
				group.point = it->point;
				group.count = 8;
				mybeepout.push_back(group);
			}
		}
		if(m_vKing.size() == 4)//有天炸
		{
			group.index.clear();
			group.count = 4;
			group.point = 15;//王
			group.index.push_back(m_vKing[0].index[0]);
			group.index.push_back(m_vKing[1].index[0]);
			group.index.push_back(m_vKing[2].index[0]);
			group.index.push_back(m_vKing[3].index[0]);
			mybeepout.push_back(group);
		}
	}
	else if(outtype == TYPE_BOMB_7XI)
	{
		for(vector<TPokerGroup2>::iterator it = m_vSeven.begin(); it != m_vSeven.end(); it++)
		{
			if(GetCardLevel(it->point) > outlevel)
			{//找到一个大的
				group.index = it->index;
				group.point = it->point;
				group.count = 7;
				mybeepout.push_back(group);
			}
		}

		//摇摆炸弹 hxz
		int nstrcount = 0;
		int nmaxcount = 0;
		unsigned char npoint = 0;

		GetFromBombStr(otherout,nstrcount,nmaxcount,npoint);
		vector<TBombStr> mybombstr;
		GetAllBombStr(mybombstr);
		for(int i =0; i < (int)mybombstr.size(); i++)
		{
			group.index.clear();
			group.count = mybombstr[i].nCount;
			group.point = mybombstr[i].nPoint;
			group.index = mybombstr[i].index;
			mybeepout.push_back(group);
		}//摇摆炸弹hxz

		for(vector<TPokerGroup2>::iterator it = m_vEight.begin(); it != m_vEight.end(); it++)
		{//8喜
			group.index = it->index;
			group.point = it->point;
			group.count = 8;
			mybeepout.push_back(group);
		}
		if(m_vKing.size() == 4)//有天炸
		{
			group.index.clear();
			group.count = 4;
			group.point = 15;//王
			group.index.push_back(m_vKing[0].index[0]);
			group.index.push_back(m_vKing[1].index[0]);
			group.index.push_back(m_vKing[2].index[0]);
			group.index.push_back(m_vKing[3].index[0]);
			mybeepout.push_back(group);
		}
	}
	else if(outtype == TYPE_BOMB_STR)
	{//连炸，摇摆
		int nstrcount = 0;
		int nmaxcount = 0;
		unsigned char npoint = 0;

		GetFromBombStr(otherout,nstrcount,nmaxcount,npoint);
		vector<TBombStr> mybombstr;
		GetAllBombStr(mybombstr);
		for(int i =0; i < (int)mybombstr.size(); i++)
		{
			if(mybombstr[i].nCount == len1)
			{//长度相等
				if(mybombstr[i].nMaxCount > nmaxcount)
				{//最大的炸弹的张数
					group.index.clear();
					group.count = mybombstr[i].nCount;
					group.point = mybombstr[i].nPoint;
					group.index = mybombstr[i].index;
					mybeepout.push_back(group);
				}
				else if(mybombstr[i].nMaxCount == nmaxcount && mybombstr[i].nPoint > npoint)
				{
					group.index.clear();
					group.count = mybombstr[i].nCount;
					group.point = mybombstr[i].nPoint;
					group.index = mybombstr[i].index;
					mybeepout.push_back(group);
				}
			}
			else if(mybombstr[i].nCount > len1)
			{
				group.index.clear();
				group.count = mybombstr[i].nCount;
				group.point = mybombstr[i].nPoint;
				group.index = mybombstr[i].index;
				mybeepout.push_back(group);
			}
		}
		for(vector<TPokerGroup2>::iterator it = m_vSeven.begin(); it != m_vSeven.end(); it++)
		{//7喜
			group.index = it->index;
			group.point = it->point;
			group.count = 7;
			mybeepout.push_back(group);
		}
		for(vector<TPokerGroup2>::iterator it = m_vEight.begin(); it != m_vEight.end(); it++)
		{//8喜
			group.index = it->index;
			group.point = it->point;
			group.count = 8;
			mybeepout.push_back(group);
		}
		if(m_vKing.size() == 4)//有天炸
		{
			group.index.clear();
			group.count = 4;
			group.point = 15;//王
			group.index.push_back(m_vKing[0].index[0]);
			group.index.push_back(m_vKing[1].index[0]);
			group.index.push_back(m_vKing[2].index[0]);
			group.index.push_back(m_vKing[3].index[0]);
			mybeepout.push_back(group);
		}
	}
	else if(outtype == TYPE_BOMB_NOMORL)
	{//普通炸弹，这里面就有王可以作为任何牌来搭配了
		//4个王
		//先比较张数，再比较大小
		for(vector<TPokerGroup2>::iterator it = m_vFour.begin(); it != m_vFour.end(); it++)
		{//4
			if(len1 > 4)
				break;
			//下面都是==4的,<4的不是炸弹了
			else if(len1 == 4 && GetCardLevel(it->point) > outlevel)
			{
				group.index.clear();
				group.count = 4;
				group.point = it->point;
				group.index = it->index;
				mybeepout.push_back(group);
			}
			else if(len1 < 4)
			{//不可能
				group.index.clear();
				group.count = 4;
				group.point = it->point;
				group.index = it->index;
				mybeepout.push_back(group);
			}
		}
		for(vector<TPokerGroup2>::iterator it = m_vFive.begin(); it != m_vFive.end(); it++)
		{//5
			if(len1 > 5)
				break;

			else if(len1 == 5 && GetCardLevel(it->point) > outlevel)
			{
				group.index.clear();
				group.count = 5;
				group.point = it->point;
				group.index = it->index;
				mybeepout.push_back(group);
			}
			else if(len1 < 5)
			{
				group.index.clear();
				group.count = 5;
				group.point = it->point;
				group.index = it->index;
				mybeepout.push_back(group);
			}
		}
		for(vector<TPokerGroup2>::iterator it = m_vSix.begin(); it != m_vSix.end(); it++)
		{
			if(len1 > 6)
				break;

			else if(len1 == 6 && GetCardLevel(it->point) > outlevel)
			{
				group.index.clear();
				group.count = 6;
				group.point = it->point;
				group.index = it->index;
				mybeepout.push_back(group);
			}
			else if(len1 < 6)
			{
				group.index.clear();
				group.count = 6;
				group.point = it->point;
				group.index = it->index;
				mybeepout.push_back(group);
			}
		}
		//数目大于6的炸弹，都有王组合了。。4个王+8个牌，最多是12炸弹
		//这里来找组合
		//找出能和王组合的所有炸弹
		vector<TPokerGroup2> templist;
		GetCombineBomb(cardtype1.card,len1,templist);
		for(vector<TPokerGroup2>::iterator it = templist.begin(); it != templist.end(); it++)
		{
			mybeepout.push_back(*it);
		}
		////7喜，8喜了

		for(vector<TPokerGroup2>::iterator it = m_vSeven.begin(); it != m_vSeven.end(); it++)
		{//7喜
			group.index = it->index;
			group.point = it->point;
			group.count = 7;
			mybeepout.push_back(group);
		}

		//摇摆炸弹 hxz
		int nstrcount = 0;
		int nmaxcount = 0;
		unsigned char npoint = 0;

		GetFromBombStr(otherout,nstrcount,nmaxcount,npoint);
		vector<TBombStr> mybombstr;
		GetAllBombStr(mybombstr);
		for(int i =0; i < (int)mybombstr.size(); i++)
		{
			group.index.clear();
			group.count = mybombstr[i].nCount;
			group.point = mybombstr[i].nPoint;
			group.index = mybombstr[i].index;
			mybeepout.push_back(group);
		}//摇摆炸弹hxz

		for(vector<TPokerGroup2>::iterator it = m_vEight.begin(); it != m_vEight.end(); it++)
		{//8喜
			group.index = it->index;
			group.point = it->point;
			group.count = 8;
			mybeepout.push_back(group);
		}
		if(m_vKing.size() == 4)//有天炸
		{
			group.index.clear();
			group.count = 4;
			group.point = 15;//王
			group.index.push_back(m_vKing[0].index[0]);
			group.index.push_back(m_vKing[1].index[0]);
			group.index.push_back(m_vKing[2].index[0]);
			group.index.push_back(m_vKing[3].index[0]);
			mybeepout.push_back(group);
		}
	}
	else if(outtype == TYPE_BOMB_510K)
	{//k-10-5
		//在这里要判断花色，这里要考虑组合的k105
		vector<TPokerGroup2> templist;
		GetAllCombineK105(cardtype1.color,templist);
		for(vector<TPokerGroup2>::iterator it = templist.begin(); it != templist.end(); it++)
		{
			mybeepout.push_back(*it);
		}

		for(vector<TPokerGroup2>::iterator it = m_vFour.begin(); it != m_vFour.end(); it++)
		{//4
			group.index.clear();
			group.count = 4;
			group.point = it->point;
			group.index = it->index;
			mybeepout.push_back(group);
		}
		for(vector<TPokerGroup2>::iterator it = m_vFive.begin(); it != m_vFive.end(); it++)
		{//5
			group.index.clear();
			group.count = 5;
			group.point = it->point;
			group.index = it->index;
			mybeepout.push_back(group);
		}
		for(vector<TPokerGroup2>::iterator it = m_vSix.begin(); it != m_vSix.end(); it++)
		{
			group.index.clear();
			group.count = 6;
			group.point = it->point;
			group.index = it->index;
			mybeepout.push_back(group);
		}
		//数目大于6的炸弹，都有王组合了。。4个王+8个牌，最多是12炸弹
		//这里来找组合
		//找出能和王组合的所有炸弹
		templist.clear();
		GetCombineBomb(0,4,templist);
		for(vector<TPokerGroup2>::iterator it = templist.begin(); it != templist.end(); it++)
		{
			mybeepout.push_back(*it);
		}

		////7喜，8喜了

		for(vector<TPokerGroup2>::iterator it = m_vSeven.begin(); it != m_vSeven.end(); it++)
		{//7喜
			group.index = it->index;
			group.point = it->point;
			group.count = 7;
			mybeepout.push_back(group);
		}

		//摇摆炸弹 hxz
		int nstrcount = 0;
		int nmaxcount = 0;
		unsigned char npoint = 0;

		GetFromBombStr(otherout,nstrcount,nmaxcount,npoint);
		vector<TBombStr> mybombstr;
		GetAllBombStr(mybombstr);
		for(int i =0; i < (int)mybombstr.size(); i++)
		{
			group.index.clear();
			group.count = mybombstr[i].nCount;
			group.point = mybombstr[i].nPoint;
			group.index = mybombstr[i].index;
			mybeepout.push_back(group);
		}//摇摆炸弹hxz

		for(vector<TPokerGroup2>::iterator it = m_vEight.begin(); it != m_vEight.end(); it++)
		{//8喜
			group.index = it->index;
			group.point = it->point;
			group.count = 8;
			mybeepout.push_back(group);
		}
		if(m_vKing.size() == 4)//有天炸
		{
			group.index.clear();
			group.count = 4;
			group.point = 15;//王
			group.index.push_back(m_vKing[0].index[0]);
			group.index.push_back(m_vKing[1].index[0]);
			group.index.push_back(m_vKing[2].index[0]);
			group.index.push_back(m_vKing[3].index[0]);
			mybeepout.push_back(group);
		}
	}
	else if(outtype == TYPE_THREESTR)
	{
		vector<TPokerGroup2> templist;
		templist.clear();
		GetCombineThreeStr(cardtype1.card,cardtype1.len,templist);
		for(vector<TPokerGroup2>::iterator it = templist.begin(); it != templist.end(); it++)
		{
			mybeepout.push_back(*it);
		}
		templist.clear();
		GetAllCombineK105(-1,templist);
		//这里只需要一个k105就够了。。。否则将出现很多提示
		for(vector<TPokerGroup2>::iterator it = templist.begin(); it != templist.end(); it++)
		{
			mybeepout.push_back(*it);
			break;//1个够了
		}
		//炸弹

		for(vector<TPokerGroup2>::iterator it = m_vFour.begin(); it != m_vFour.end(); it++)
		{//4
			group.index.clear();
			group.count = 4;
			group.point = it->point;
			group.index = it->index;
			mybeepout.push_back(group);
		}
		for(vector<TPokerGroup2>::iterator it = m_vFive.begin(); it != m_vFive.end(); it++)
		{//5
			group.index.clear();
			group.count = 5;
			group.point = it->point;
			group.index = it->index;
			mybeepout.push_back(group);
		}
		for(vector<TPokerGroup2>::iterator it = m_vSix.begin(); it != m_vSix.end(); it++)
		{
			group.index.clear();
			group.count = 6;
			group.point = it->point;
			group.index = it->index;
			mybeepout.push_back(group);
		}
		templist.clear();
		GetCombineBomb(0,4,templist);
		for(vector<TPokerGroup2>::iterator it = templist.begin(); it != templist.end(); it++)
		{
			mybeepout.push_back(*it);
		}
		////7喜，8喜了

		for(vector<TPokerGroup2>::iterator it = m_vSeven.begin(); it != m_vSeven.end(); it++)
		{//7喜
			group.index = it->index;
			group.point = it->point;
			group.count = 7;
			mybeepout.push_back(group);
		}

		//摇摆炸弹 hxz
		int nstrcount = 0;
		int nmaxcount = 0;
		unsigned char npoint = 0;

		GetFromBombStr(otherout,nstrcount,nmaxcount,npoint);
		vector<TBombStr> mybombstr;
		GetAllBombStr(mybombstr);
		for(int i =0; i < (int)mybombstr.size(); i++)
		{
			group.index.clear();
			group.count = mybombstr[i].nCount;
			group.point = mybombstr[i].nPoint;
			group.index = mybombstr[i].index;
			mybeepout.push_back(group);
		}//摇摆炸弹hxz

		for(vector<TPokerGroup2>::iterator it = m_vEight.begin(); it != m_vEight.end(); it++)
		{//8喜
			group.index = it->index;
			group.point = it->point;
			group.count = 8;
			mybeepout.push_back(group);
		}
		if(m_vKing.size() == 4)//有天炸
		{
			group.index.clear();
			group.count = 4;
			group.point = 15;//王
			group.index.push_back(m_vKing[0].index[0]);
			group.index.push_back(m_vKing[1].index[0]);
			group.index.push_back(m_vKing[2].index[0]);
			group.index.push_back(m_vKing[3].index[0]);
			mybeepout.push_back(group);
		}
	}
	else if(outtype == TYPE_TWOSTR)
	{
		vector<TPokerGroup2> templist;
		templist.clear();
		GetCombineTwoStr(cardtype1.card,cardtype1.len,templist);
		for(vector<TPokerGroup2>::iterator it = templist.begin(); it != templist.end(); it++)
		{
			mybeepout.push_back(*it);
		}
		templist.clear();
		GetAllCombineK105(-1,templist);
		for(vector<TPokerGroup2>::iterator it = templist.begin(); it != templist.end(); it++)
		{
			mybeepout.push_back(*it);
			break;//一个就够了
		}
		//炸弹

		for(vector<TPokerGroup2>::iterator it = m_vFour.begin(); it != m_vFour.end(); it++)
		{//4
			group.index.clear();
			group.count = 4;
			group.point = it->point;
			group.index = it->index;
			mybeepout.push_back(group);
		}
		for(vector<TPokerGroup2>::iterator it = m_vFive.begin(); it != m_vFive.end(); it++)
		{//5
			group.index.clear();
			group.count = 5;
			group.point = it->point;
			group.index = it->index;
			mybeepout.push_back(group);
		}
		for(vector<TPokerGroup2>::iterator it = m_vSix.begin(); it != m_vSix.end(); it++)
		{
			group.index.clear();
			group.count = 6;
			group.point = it->point;
			group.index = it->index;
			mybeepout.push_back(group);
		}
		templist.clear();
		GetCombineBomb(0,4,templist);
		for(vector<TPokerGroup2>::iterator it = templist.begin(); it != templist.end(); it++)
		{
			mybeepout.push_back(*it);
		}
		////7喜，8喜了

		for(vector<TPokerGroup2>::iterator it = m_vSeven.begin(); it != m_vSeven.end(); it++)
		{//7喜
			group.index = it->index;
			group.point = it->point;
			group.count = 7;
			mybeepout.push_back(group);
		}

		//摇摆炸弹 hxz
		int nstrcount = 0;
		int nmaxcount = 0;
		unsigned char npoint = 0;

		GetFromBombStr(otherout,nstrcount,nmaxcount,npoint);
		vector<TBombStr> mybombstr;
		GetAllBombStr(mybombstr);
		for(int i =0; i < (int)mybombstr.size(); i++)
		{
			group.index.clear();
			group.count = mybombstr[i].nCount;
			group.point = mybombstr[i].nPoint;
			group.index = mybombstr[i].index;
			mybeepout.push_back(group);
		}//摇摆炸弹hxz

		for(vector<TPokerGroup2>::iterator it = m_vEight.begin(); it != m_vEight.end(); it++)
		{//8喜
			group.index = it->index;
			group.point = it->point;
			group.count = 8;
			mybeepout.push_back(group);
		}
		if(m_vKing.size() == 4)//有天炸
		{
			group.index.clear();
			group.count = 4;
			group.point = 15;//王
			group.index.push_back(m_vKing[0].index[0]);
			group.index.push_back(m_vKing[1].index[0]);
			group.index.push_back(m_vKing[2].index[0]);
			group.index.push_back(m_vKing[3].index[0]);
			mybeepout.push_back(group);
		}
	}
	else if(outtype == TYPE_THREE)
	{
		vector<TPokerGroup2> templist;
		templist.clear();
		GetCombineThree(cardtype1.card,templist);
		for(vector<TPokerGroup2>::iterator it = templist.begin(); it != templist.end(); it++)
		{
			mybeepout.push_back(*it);
		}
		templist.clear();
		GetAllCombineK105(-1,templist);
		for(vector<TPokerGroup2>::iterator it = templist.begin(); it != templist.end(); it++)
		{
			mybeepout.push_back(*it);
			break;//一个够了
		}
		//炸弹

		for(vector<TPokerGroup2>::iterator it = m_vFour.begin(); it != m_vFour.end(); it++)
		{//4
			group.index.clear();
			group.count = 4;
			group.point = it->point;
			group.index = it->index;
			mybeepout.push_back(group);
		}
		for(vector<TPokerGroup2>::iterator it = m_vFive.begin(); it != m_vFive.end(); it++)
		{//5
			group.index.clear();
			group.count = 5;
			group.point = it->point;
			group.index = it->index;
			mybeepout.push_back(group);
		}
		for(vector<TPokerGroup2>::iterator it = m_vSix.begin(); it != m_vSix.end(); it++)
		{
			group.index.clear();
			group.count = 6;
			group.point = it->point;
			group.index = it->index;
			mybeepout.push_back(group);
		}
		templist.clear();
		GetCombineBomb(0,4,templist);
		for(vector<TPokerGroup2>::iterator it = templist.begin(); it != templist.end(); it++)
		{
			mybeepout.push_back(*it);
		}
		////7喜，8喜了

		for(vector<TPokerGroup2>::iterator it = m_vSeven.begin(); it != m_vSeven.end(); it++)
		{//7喜
			group.index = it->index;
			group.point = it->point;
			group.count = 7;
			mybeepout.push_back(group);
		}

		//摇摆炸弹 hxz
		int nstrcount = 0;
		int nmaxcount = 0;
		unsigned char npoint = 0;

		GetFromBombStr(otherout,nstrcount,nmaxcount,npoint);
		vector<TBombStr> mybombstr;
		GetAllBombStr(mybombstr);
		for(int i =0; i < (int)mybombstr.size(); i++)
		{
			group.index.clear();
			group.count = mybombstr[i].nCount;
			group.point = mybombstr[i].nPoint;
			group.index = mybombstr[i].index;
			mybeepout.push_back(group);
		}//摇摆炸弹hxz

		for(vector<TPokerGroup2>::iterator it = m_vEight.begin(); it != m_vEight.end(); it++)
		{//8喜
			group.index = it->index;
			group.point = it->point;
			group.count = 8;
			mybeepout.push_back(group);
		}
		if(m_vKing.size() == 4)//有天炸
		{
			group.index.clear();
			group.count = 4;
			group.point = 15;//王
			group.index.push_back(m_vKing[0].index[0]);
			group.index.push_back(m_vKing[1].index[0]);
			group.index.push_back(m_vKing[2].index[0]);
			group.index.push_back(m_vKing[3].index[0]);
			mybeepout.push_back(group);
		}
	}
	else if(outtype == TYPE_TWO)
	{
		vector<TPokerGroup2> templist;
		templist.clear();
		GetCombineTwo(cardtype1.card,templist);
		for(vector<TPokerGroup2>::iterator it = templist.begin(); it != templist.end(); it++)
		{
			mybeepout.push_back(*it);
		}
		templist.clear();
		GetAllCombineK105(-1,templist);
		for(vector<TPokerGroup2>::iterator it = templist.begin(); it != templist.end(); it++)
		{
			mybeepout.push_back(*it);
			break;//一个够了
		}
		//炸弹

		for(vector<TPokerGroup2>::iterator it = m_vFour.begin(); it != m_vFour.end(); it++)
		{//4
			group.index.clear();
			group.count = 4;
			group.point = it->point;
			group.index = it->index;
			mybeepout.push_back(group);
		}
		for(vector<TPokerGroup2>::iterator it = m_vFive.begin(); it != m_vFive.end(); it++)
		{//5
			group.index.clear();
			group.count = 5;
			group.point = it->point;
			group.index = it->index;
			mybeepout.push_back(group);
		}
		for(vector<TPokerGroup2>::iterator it = m_vSix.begin(); it != m_vSix.end(); it++)
		{
			group.index.clear();
			group.count = 6;
			group.point = it->point;
			group.index = it->index;
			mybeepout.push_back(group);
		}
		templist.clear();
		GetCombineBomb(0,4,templist);
		for(vector<TPokerGroup2>::iterator it = templist.begin(); it != templist.end(); it++)
		{
			mybeepout.push_back(*it);
		}
		////7喜，8喜了

		for(vector<TPokerGroup2>::iterator it = m_vSeven.begin(); it != m_vSeven.end(); it++)
		{//7喜
			group.index = it->index;
			group.point = it->point;
			group.count = 7;
			mybeepout.push_back(group);
		}

		//摇摆炸弹 hxz
		int nstrcount = 0;
		int nmaxcount = 0;
		unsigned char npoint = 0;

		GetFromBombStr(otherout,nstrcount,nmaxcount,npoint);
		vector<TBombStr> mybombstr;
		GetAllBombStr(mybombstr);
		for(int i =0; i < (int)mybombstr.size(); i++)
		{
			group.index.clear();
			group.count = mybombstr[i].nCount;
			group.point = mybombstr[i].nPoint;
			group.index = mybombstr[i].index;
			mybeepout.push_back(group);
		}//摇摆炸弹hxz

		for(vector<TPokerGroup2>::iterator it = m_vEight.begin(); it != m_vEight.end(); it++)
		{//8喜
			group.index = it->index;
			group.point = it->point;
			group.count = 8;
			mybeepout.push_back(group);
		}
		if(m_vKing.size() == 4)//有天炸
		{
			group.index.clear();
			group.count = 4;
			group.point = 15;//王
			group.index.push_back(m_vKing[0].index[0]);
			group.index.push_back(m_vKing[1].index[0]);
			group.index.push_back(m_vKing[2].index[0]);
			group.index.push_back(m_vKing[3].index[0]);
			mybeepout.push_back(group);
		}
	}
	else if(outtype == TYPE_ONE)
	{
		vector<TPokerGroup2> templist;
		templist.clear();
		GetAllOne(cardtype1.card,templist);
		for(vector<TPokerGroup2>::iterator it = templist.begin(); it != templist.end(); it++)
		{
			mybeepout.push_back(*it);
		}

		//for(vector<TPokerGroup2>::iterator it = m_vOne.begin(); it != m_vOne.end(); it++)
		//{
		//	//if(IsKing(it->point))
		//	//	continue;
		//	if(GetCardLevel(it->point) > GetCardLevel(cardtype1.card))
		//	{
		//		group.index.clear();
		//		group.count = 1;
		//		group.point = it->point;
		//		group.index = it->index;
		//		mybeepout.push_back(group);
		//	}

		//}
		templist.clear();
		GetAllCombineK105(-1,templist);
		for(vector<TPokerGroup2>::iterator it = templist.begin(); it != templist.end(); it++)
		{
			mybeepout.push_back(*it);
			break;//一个够了
		}
		//炸弹
		for(vector<TPokerGroup2>::iterator it = m_vFour.begin(); it != m_vFour.end(); it++)
		{//4
			group.index.clear();
			group.count = 4;
			group.point = it->point;
			group.index = it->index;
			mybeepout.push_back(group);
		}
		for(vector<TPokerGroup2>::iterator it = m_vFive.begin(); it != m_vFive.end(); it++)
		{//5
			group.index.clear();
			group.count = 5;
			group.point = it->point;
			group.index = it->index;
			mybeepout.push_back(group);
		}
		for(vector<TPokerGroup2>::iterator it = m_vSix.begin(); it != m_vSix.end(); it++)
		{
			group.index.clear();
			group.count = 6;
			group.point = it->point;
			group.index = it->index;
			mybeepout.push_back(group);
		}
		
		GetCombineBomb(0,4,templist);
		for(vector<TPokerGroup2>::iterator it = templist.begin(); it != templist.end(); it++)
		{
			mybeepout.push_back(*it);
		}
		////7喜，8喜了

		for(vector<TPokerGroup2>::iterator it = m_vSeven.begin(); it != m_vSeven.end(); it++)
		{//7喜
			group.index = it->index;
			group.point = it->point;
			group.count = 7;
			mybeepout.push_back(group);
		}

		//摇摆炸弹 hxz
		int nstrcount = 0;
		int nmaxcount = 0;
		unsigned char npoint = 0;

		GetFromBombStr(otherout,nstrcount,nmaxcount,npoint);
		vector<TBombStr> mybombstr;
		GetAllBombStr(mybombstr);
		for(int i =0; i < (int)mybombstr.size(); i++)
		{
			group.index.clear();
			group.count = mybombstr[i].nCount;
			group.point = mybombstr[i].nPoint;
			group.index = mybombstr[i].index;
			mybeepout.push_back(group);
		}//摇摆炸弹hxz

		for(vector<TPokerGroup2>::iterator it = m_vEight.begin(); it != m_vEight.end(); it++)
		{//8喜
			group.index = it->index;
			group.point = it->point;
			group.count = 8;
			mybeepout.push_back(group);
		}
		if(m_vKing.size() == 4)//有天炸
		{
			group.index.clear();
			group.count = 4;
			group.point = 15;//王
			group.index.push_back(m_vKing[0].index[0]);
			group.index.push_back(m_vKing[1].index[0]);
			group.index.push_back(m_vKing[2].index[0]);
			group.index.push_back(m_vKing[3].index[0]);
			mybeepout.push_back(group);
		}
	}
	if(mybeepout.size() > 0)
		return true;
	else
		return false;
}

void CDG4::GetAllBombStr(vector<TBombStr>& mybombstr)
{
	mybombstr.clear();
	if(m_vFour.size() + m_vFive.size() + m_vSix.size() + m_vSeven.size() + m_vEight.size() == 0)
	{
		return;
	}

	int count1 = 0,count2=0;
	struct TempStr{
		unsigned char point;
		unsigned char count;
	};
	//27张牌，即便全部是4个的炸弹，也最多27/4 = 6个
	TempStr TempIndexList[MAX_BOMBSTR_NUM];
	memset(TempIndexList,0,sizeof(TempIndexList));

	int n =0;
	//2是不能做连炸的
	for(int i = 0; i < (int)m_vFour.size(); i++)
	{
		if(m_vFour[i].point == 2)
			continue;
		TempIndexList[n].point = m_vFour[i].point;
		TempIndexList[n].count = 4;
		n++;
	}

	for(int i = 0; i < (int)m_vFive.size(); i++)
	{
		if(m_vFive[i].point == 2)
			continue;
		TempIndexList[n].point = m_vFive[i].point;
		TempIndexList[n].count = 5;
		n++;
	}

	for(int i = 0; i < (int)m_vSix.size(); i++)
	{
		if(m_vSix[i].point == 2)
			continue;
		TempIndexList[n].point = m_vSix[i].point;
		TempIndexList[n].count = 6;
		n++;
	}

	for(int i = 0; i < (int)m_vSeven.size(); i++)
	{
		if(m_vSeven[i].point == 2)
			continue;
		TempIndexList[n].point = m_vSeven[i].point;
		TempIndexList[n].count = 7;
		n++;
	}

	for(int i = 0; i < (int)m_vEight.size(); i++)
	{
		if(m_vEight[i].point == 2)
			continue;
		TempIndexList[n].point = m_vEight[i].point;
		TempIndexList[n].count = 8;
		n++;
	}
	if(n < 2) return;
	//排序
	for(int i = 0; i < n-1; i++)
	{
		for(int j = i+1; j < n; j++)
		{
			if(GetCardLevel(TempIndexList[i].point) > GetCardLevel(TempIndexList[j].point) && TempIndexList[j].point > 0)
			{
				TempStr temp = TempIndexList[i];
				TempIndexList[i] = TempIndexList[j];
				TempIndexList[j] = temp;
			}
		}
	}
	//这里，找出所有的最长的连子，，
	bool start = false;
	
	TempStr onestr[3][MAX_BOMBSTR_NUM];//最多3个连子，每个连子都是2两对34,67,910
	memset(onestr,0,sizeof(onestr));
	int count = 0;
	int index=0;
	for(int i = 0; i < n-1; i ++)
	{
		if(GetCardLevel(TempIndexList[i].point) +1 == GetCardLevel(TempIndexList[i+1].point))
		{
			if(!start)
			{
				index = 0;
				start = true;
				onestr[count][index++] = TempIndexList[i];
			}
			else
			{
				onestr[count][index++] = TempIndexList[i];
			}
			continue;
		}
		else
		{
			if(start)
			{
				onestr[count][index++] = TempIndexList[i];
				start = false;
				count++;
			}
			
		}
	}
	if(start)
	{//最后这个放入队列
		onestr[count][index] = TempIndexList[n-1];
	}
	
	for(int i = 0; i < count+1; i++)
	{
		for(int j=0;j < MAX_BOMBSTR_NUM-1; j++)
		{
			for(int n = 1; n < MAX_BOMBSTR_NUM; n++)
			{
				if(GetCardLevel(onestr[i][j].point) +n == GetCardLevel(onestr[i][j+n].point))
				{//这里是个连子
				}
				else
				{//3456
					int m = n;
					while(m > 1)
					{
						vector<unsigned char> indexlist;
						int maxcount = 0;
						int maxpoint = 0;
						int allcount = 0;
						int bombcount= 0;
						indexlist.clear();
						for(int ii = 0; ii < m; ii++)
						{
							int ncount = onestr[i][j+ii].count;
							int npoint = onestr[i][j+ii].point;
							bombcount++;
							vector<TPokerGroup2> tempGroup;
							switch(ncount)
							{
							case 4:
								tempGroup = m_vFour;
								break;
							case 5:
								tempGroup = m_vFive;
								break;
							case 6:
								tempGroup = m_vSix;
								break;
							case 7:
								tempGroup = m_vSeven;
								break;
							case 8:
								tempGroup = m_vEight;
								break;
							}
							for(int size = 0; size < (int)tempGroup.size(); size++)
							{
								if(GetCardLevel(npoint) == GetCardLevel(tempGroup[size].point))
								{
									if( ncount > maxcount)
									{
										maxpoint = npoint;
										maxcount = ncount;
									}
									else if(ncount == maxcount)
									{
										if(npoint > maxpoint) maxpoint = npoint;
									}
									allcount += ncount;
									for(int cn = 0; cn < ncount; cn++)
									{
										indexlist.push_back(tempGroup[size].index[cn]);
									}
								}
							}
						}
						TBombStr bombstr;
						bombstr.nBombCount = bombcount;
						bombstr.nCount     = allcount;
						bombstr.nMaxCount  = maxcount;
						bombstr.nPoint     = maxpoint;
						bombstr.index	   = indexlist;
						mybombstr.push_back(bombstr);
						m--;
					}
					break;
				}
			}
		}
	}
}

void CDG4::GetFromBombStr( unsigned char otherpai[MAXPAI],int& nStrCount,int& nMaxCount,unsigned char& nPoint )
{
	nStrCount = 0;
	nMaxCount = 0;
	nPoint    = 0;
	
	int len = GetCardNum(otherpai);
	if(len < 8) return;

	TCardType cardtype = GetType(otherpai,len);
	if(cardtype.type != TYPE_BOMB_STR) 
		return;
	unsigned char index[MAX_BOMBSTR_NUM]={0,0,0,0,0,0};//最多6连炸...一共才27张牌呢
	unsigned char pointlist[12] = {3,4,5,6,7,8,9,10,11,12,13,1};//从3到A
	for(int i = 0; i < 12; i++)
	{
		int num = 0;
		for(int j = 0; j < len; j++)
		{
			if(otherpai[j] > 0 && otherpai[j] < 55)
			{
				if(GetCardLevel(otherpai[j]) == GetCardLevel(pointlist[i]))
				{
					num++;
				}
			}
		}
		if(num >= 4)
		{
			nStrCount++;
			if(num > nMaxCount)
			{
				nMaxCount = num;
				nPoint = pointlist[i];
			}
			else if(num == nMaxCount)
			{
				if(pointlist[i] > nPoint)
					nPoint = pointlist[i];
			}
		}
	}
}

void CDG4::GetCombineBomb(unsigned char outpoint,int outbomblen,vector<TPokerGroup2>& combinelist)
{
	combinelist.clear();
	int size = (int)m_vKing.size();
	if(size == 0)
		return;
	if(outbomblen < 4)
		return;
	TPokerGroup2 group;
	//最多4个王。。
	for(vector<TPokerGroup2>::iterator it = m_vOne.begin(); it != m_vOne.end(); it++)
	{//1
		if(outbomblen > 1+size)
			break;
		else if(outbomblen == 1+size && GetCardLevel(it->point) > GetCardLevel(outpoint))
		{
			group.index.clear();
			group.count = 1+size;
			group.point = it->point;
			group.index = it->index;
			for(vector<TPokerGroup2>::iterator it2 = m_vKing.begin(); it2 != m_vKing.end(); it2++)
				group.index.push_back(it2->index[0]);
			combinelist.push_back(group);
		}
		else if(outbomblen < 1+size)
		{
			group.index.clear();
			group.count = 1+size;
			group.point = it->point;
			group.index = it->index;
			for(vector<TPokerGroup2>::iterator it2 = m_vKing.begin(); it2 != m_vKing.end(); it2++)
				group.index.push_back(it2->index[0]);
			combinelist.push_back(group);
		}
	}
	for(vector<TPokerGroup2>::iterator it = m_vTwo.begin(); it != m_vTwo.end(); it++)
	{//2
		if(outbomblen > 2+size)
			break;
		else if(outbomblen == 2+size && GetCardLevel(it->point) > GetCardLevel(outpoint))
		{
			group.index.clear();
			group.count = 2+size;
			group.point = it->point;
			group.index = it->index;
			for(vector<TPokerGroup2>::iterator it2 = m_vKing.begin(); it2 != m_vKing.end(); it2++)
				group.index.push_back(it2->index[0]);
			combinelist.push_back(group);
		}
		else if(outbomblen < 2+size)
		{
			group.index.clear();
			group.count = 2+size;
			group.point = it->point;
			group.index = it->index;
			for(vector<TPokerGroup2>::iterator it2 = m_vKing.begin(); it2 != m_vKing.end(); it2++)
				group.index.push_back(it2->index[0]);
			combinelist.push_back(group);
		}
	}
	for(vector<TPokerGroup2>::iterator it = m_vThree.begin(); it != m_vThree.end(); it++)
	{//3
		if(outbomblen > 3+size)
			break;
		else if(outbomblen == 3+size && GetCardLevel(it->point) > GetCardLevel(outpoint))
		{
			group.index.clear();
			group.count = 3+size;
			group.point = it->point;
			group.index = it->index;
			for(vector<TPokerGroup2>::iterator it2 = m_vKing.begin(); it2 != m_vKing.end(); it2++)
				group.index.push_back(it2->index[0]);
			combinelist.push_back(group);
		}
		else if(outbomblen < 3+size)//不可能
		{
			group.index.clear();
			group.count = 3+size;
			group.point = it->point;
			group.index = it->index;
			for(vector<TPokerGroup2>::iterator it2 = m_vKing.begin(); it2 != m_vKing.end(); it2++)
				group.index.push_back(it2->index[0]);
			combinelist.push_back(group);
		}
	}
	for(vector<TPokerGroup2>::iterator it = m_vFour.begin(); it != m_vFour.end(); it++)
	{//4
		if(outbomblen > 4+size)
			break;
		else if(outbomblen == 4+size && GetCardLevel(it->point) > GetCardLevel(outpoint))
		{
			group.index.clear();
			group.count = 4+size;
			group.point = it->point;
			group.index = it->index;
			for(vector<TPokerGroup2>::iterator it2 = m_vKing.begin(); it2 != m_vKing.end(); it2++)
				group.index.push_back(it2->index[0]);
			combinelist.push_back(group);
		}
		else if(outbomblen < 4+size)
		{
			group.index.clear();
			group.count = 4+size;
			group.point = it->point;
			group.index = it->index;
			for(vector<TPokerGroup2>::iterator it2 = m_vKing.begin(); it2 != m_vKing.end(); it2++)
				group.index.push_back(it2->index[0]);
			combinelist.push_back(group);
		}
	}
	for(vector<TPokerGroup2>::iterator it = m_vFive.begin(); it != m_vFive.end(); it++)
	{//5
		if(outbomblen > 5+size)
			break;
		else if(outbomblen == 6 && GetCardLevel(it->point) > GetCardLevel(outpoint))
		{
			group.index.clear();
			group.count = 5+size;
			group.point = it->point;
			group.index = it->index;
			for(vector<TPokerGroup2>::iterator it2 = m_vKing.begin(); it2 != m_vKing.end(); it2++)
				group.index.push_back(it2->index[0]);
			combinelist.push_back(group);
		}
		else if(outbomblen < 5+size)
		{
			group.index.clear();
			group.count = 5+size;
			group.point = it->point;
			group.index = it->index;
			for(vector<TPokerGroup2>::iterator it2 = m_vKing.begin(); it2 != m_vKing.end(); it2++)
				group.index.push_back(it2->index[0]);
			combinelist.push_back(group);
		}
	}
	for(vector<TPokerGroup2>::iterator it = m_vSix.begin(); it != m_vSix.end(); it++)
	{//6
		if(outbomblen > 6+size)
			break;
		else if(outbomblen == 6+size && GetCardLevel(it->point) > GetCardLevel(outpoint))
		{
			group.index.clear();
			group.count = 6+size;
			group.point = it->point;
			group.index = it->index;
			for(vector<TPokerGroup2>::iterator it2 = m_vKing.begin(); it2 != m_vKing.end(); it2++)
				group.index.push_back(it2->index[0]);
			combinelist.push_back(group);
		}
		else if(outbomblen < 6+size)
		{
			group.index.clear();
			group.count = 6+size;
			group.point = it->point;
			group.index = it->index;
			for(vector<TPokerGroup2>::iterator it2 = m_vKing.begin(); it2 != m_vKing.end(); it2++)
				group.index.push_back(it2->index[0]);
			combinelist.push_back(group);
		}
	}
	for(vector<TPokerGroup2>::iterator it = m_vSeven.begin(); it != m_vSeven.end(); it++)
	{//7
		if(outbomblen > 7+size)
			break;
		else if(outbomblen == 7+size && GetCardLevel(it->point) > GetCardLevel(outpoint))
		{
			group.index.clear();
			group.count = 7+size;
			group.point = it->point;
			group.index = it->index;
			for(vector<TPokerGroup2>::iterator it2 = m_vKing.begin(); it2 != m_vKing.end(); it2++)
				group.index.push_back(it2->index[0]);
			combinelist.push_back(group);
		}
		else if(outbomblen < 7+size)
		{
			group.index.clear();
			group.count = 7+size;
			group.point = it->point;
			group.index = it->index;
			for(vector<TPokerGroup2>::iterator it2 = m_vKing.begin(); it2 != m_vKing.end(); it2++)
				group.index.push_back(it2->index[0]);
			combinelist.push_back(group);
		}
	}
	for(vector<TPokerGroup2>::iterator it = m_vEight.begin(); it != m_vEight.end(); it++)
	{//8
		if(outbomblen > 8+size)
			break;
		else if(outbomblen == 8+size && GetCardLevel(it->point) > GetCardLevel(outpoint))
		{
			group.index.clear();
			group.count = 8+size;
			group.point = it->point;
			group.index = it->index;
			for(vector<TPokerGroup2>::iterator it2 = m_vKing.begin(); it2 != m_vKing.end(); it2++)
				group.index.push_back(it2->index[0]);
			combinelist.push_back(group);
		}
		else if(outbomblen < 8+size)
		{
			group.index.clear();
			group.count = 8+size;
			group.point = it->point;
			group.index = it->index;
			for(vector<TPokerGroup2>::iterator it2 = m_vKing.begin(); it2 != m_vKing.end(); it2++)
				group.index.push_back(it2->index[0]);
			combinelist.push_back(group);
		}
	}
}

void CDG4::GetAllCombineK105(int color,vector<TPokerGroup2>& combinelist)
{
	combinelist.clear();
	if(m_vK105.size() == 0)
		return;
	
	vector<TPokerGroup2> templist;

	templist.clear();

	vector<TPoker> tempK,temp10,temp5;
	for(vector<TPokerGroup2>::iterator it = m_vK105.begin(); it != m_vK105.end(); it++)
	{
		TPoker temp;
		if(GetCardLevel(it->point) == GetCardLevel(13))
		{
			temp.index = it->index[0];
			temp.color = it->color;
			tempK.push_back(temp);
		}
		else if(GetCardLevel(it->point) == GetCardLevel(10))
		{
			temp.index = it->index[0];
			temp.color = it->color;
			temp10.push_back(temp);
		}
		else if(GetCardLevel(it->point) == GetCardLevel(5))
		{
			temp.index = it->index[0];
			temp.color = it->color;
			temp5.push_back(temp);
		}
	}
	//这个是没有王的
	TPokerGroup2 group;
	for(vector<TPoker>::iterator itK = tempK.begin(); itK != tempK.end();itK++)
	{
		for(vector<TPoker>::iterator it10 = temp10.begin(); it10 != temp10.end();it10++)
		{
			for(vector<TPoker>::iterator it5 = temp5.begin(); it5 != temp5.end();it5++)
			{
				if(itK->color == it10->color && itK->color == it5->color)
					group.color = itK->color;
				else
					group.color = 0;
				group.point = 0;//这个是无效的
				group.count = 3;
				group.index.clear();
				group.index.push_back(itK->index);
				group.index.push_back(it10->index);
				group.index.push_back(it5->index);
				templist.push_back(group);
			}
		}
	}
	
	combinelist.clear();

	for(vector<TPokerGroup2>::iterator it = templist.begin();it != templist.end(); it++)
	{
		if(it->color > color)
		{
			combinelist.push_back(*it);
		}
	}

	if(m_vKing.size() == 0)
		return;
	//k-10-王
	for(vector<TPoker>::iterator itK = tempK.begin(); itK != tempK.end();itK++)
	{
		for(vector<TPoker>::iterator it10 = temp10.begin(); it10 != temp10.end();it10++)
		{
			if(itK->color == it10->color)
				group.color = itK->color;
			else
				group.color = 0;
			group.point = 0;//这个是无效的
			group.count = 3;
			group.index.clear();
			group.index.push_back(itK->index);
			group.index.push_back(it10->index);
			group.index.push_back(m_vKing[0].index[0]);
			templist.push_back(group);
		}
	}
	
	//k-王-5
	for(vector<TPoker>::iterator itK = tempK.begin(); itK != tempK.end();itK++)
	{
		for(vector<TPoker>::iterator it5 = temp5.begin(); it5 != temp5.end();it5++)
		{
			if(itK->color == it5->color)
				group.color = itK->color;
			else
				group.color = 0;
			group.point = 0;//这个是无效的
			group.count = 3;
			group.index.clear();
			group.index.push_back(itK->index);
			group.index.push_back(m_vKing[0].index[0]);
			group.index.push_back(it5->index);
			templist.push_back(group);
		}
	}

	//王-10-5
	for(vector<TPoker>::iterator it10 = temp10.begin(); it10 != temp10.end();it10++)
	{
		for(vector<TPoker>::iterator it5 = temp5.begin(); it5 != temp5.end();it5++)
		{
			if(it5->color == it10->color)
				group.color = it5->color;
			else
				group.color = 0;
			group.point = 0;//这个是无效的
			group.count = 3;
			group.index.clear();
			group.index.push_back(m_vKing[0].index[0]);
			group.index.push_back(it10->index);
			group.index.push_back(it5->index);
			templist.push_back(group);
		}
	}
	
	for(vector<TPokerGroup2>::iterator it = templist.begin();it != templist.end(); it++)
	{
		if(it->color > color)
		{
			combinelist.push_back(*it);
		}
	}
}

void CDG4::GetCombineThreeStr( unsigned char outpoint,int outlen,vector<TPokerGroup2>& combinelist )
{
	int kingnum = (int)m_vKing.size();
	//先找出所有三对

	TThree threelist[MAXPAI];
	memset(threelist,0,sizeof(threelist));
	//把所有的三牌列出来
	int count1 = 0;
	for(int i =0; i < (int)m_vThree.size(); i++)
	{
		if(GetCardLevel(m_vThree[i].index[0]) == GetCardLevel(2))
			continue;
		threelist[count1].kingcount  = 0;
		threelist[count1].point = m_vThree[i].index[0];
		threelist[count1].index1 = m_vThree[i].index[0];
		threelist[count1].index2 = m_vThree[i].index[1];
		threelist[count1].index3 = m_vThree[i].index[2];
		count1++;
	}
	for(int i =0; i < (int)m_vFour.size(); i++)
	{
		if(GetCardLevel(m_vFour[i].index[0]) == GetCardLevel(2))
			continue;
		threelist[count1].kingcount  = 0;
		threelist[count1].point = m_vFour[i].index[0];
		threelist[count1].index1 = m_vFour[i].index[0];
		threelist[count1].index2 = m_vFour[i].index[1];
		threelist[count1].index3 = m_vFour[i].index[2];
		count1++;
	}
	for(int i =0; i < (int)m_vFive.size(); i++)
	{
		if(GetCardLevel(m_vFive[i].index[0]) == GetCardLevel(2))
			continue;
		threelist[count1].kingcount  = 0;
		threelist[count1].point = m_vFive[i].index[0];
		threelist[count1].index1 = m_vFive[i].index[0];
		threelist[count1].index2 = m_vFive[i].index[1];
		threelist[count1].index3 = m_vFive[i].index[2];
		count1++;
	}
	for(int i =0; i < (int)m_vSix.size(); i++)
	{
		if(GetCardLevel(m_vSix[i].index[0]) == GetCardLevel(2))
			continue;
		threelist[count1].kingcount  = 0;
		threelist[count1].point = m_vSix[i].index[0];
		threelist[count1].index1 = m_vSix[i].index[0];
		threelist[count1].index2 = m_vSix[i].index[1];
		threelist[count1].index3 = m_vSix[i].index[2];
		count1++;
	}
	for(int i =0; i < (int)m_vSeven.size(); i++)
	{
		if(GetCardLevel(m_vSeven[i].index[0]) == GetCardLevel(2))
			continue;
		threelist[count1].kingcount  = 0;
		threelist[count1].point = m_vSeven[i].index[0];
		threelist[count1].index1 = m_vSeven[i].index[0];
		threelist[count1].index2 = m_vSeven[i].index[1];
		threelist[count1].index3 = m_vSeven[i].index[2];
		count1++;
	}
	for(int i =0; i < (int)m_vEight.size(); i++)
	{
		if(GetCardLevel(m_vEight[i].index[0]) == GetCardLevel(2))
			continue;
		threelist[count1].kingcount  = 0;
		threelist[count1].point = m_vEight[i].index[0];
		threelist[count1].index1 = m_vEight[i].index[0];
		threelist[count1].index2 = m_vEight[i].index[1];
		threelist[count1].index3 = m_vEight[i].index[2];
		count1++;
	}

	int num = outlen/3;//连对数
	TPokerGroup2 pai[MAXPAI];
	if(kingnum == 0)
	{//没有王
		//排序
		for(int i = 0; i < count1-1; i++)
		{
			for(int j = i+1; j < count1; j++)
			{
				if(GetCardLevel(threelist[i].point) > GetCardLevel(threelist[j].point) && threelist[j].point > 0)
				{
					TThree temp = threelist[i];
					threelist[i] = threelist[j];
					threelist[j] = temp;
				}
			}
		}
		if(count1 >= num)
		{
			//找出所有的连牌
			int count = 0;
			for(int i =0; i < count1; i++)
			{
				if(GetCardLevel(threelist[i].point) + num -1 == GetCardLevel(threelist[i+num-1].point) )
				{//找到一个
					for(int j = i; j < i+num; j++)
					{
						pai[count].index.push_back(threelist[j].index1);
						pai[count].index.push_back(threelist[j].index2);
						pai[count].index.push_back(threelist[j].index3);
					}
					pai[count].point = threelist[i+num-1].point;
					pai[count].count = outlen;
					count++;
				}
			}
			for(int i = 0; i < count; i++)
			{
				if(GetCardLevel(pai[i].point) > GetCardLevel(outpoint))
				{
					combinelist.push_back(pai[i]);
				}
			}
		}
	}
	else if(kingnum == 1)
	{//有一个王，那么对子也可以做为三对出
		for(int i =0; i < (int)m_vTwo.size(); i++)
		{
			if(GetCardLevel(m_vTwo[i].index[0]) == GetCardLevel(2))
				continue;
			threelist[count1].kingcount  = 1;
			threelist[count1].point = m_vTwo[i].index[0];
			threelist[count1].index1 = m_vTwo[i].index[0];
			threelist[count1].index2 = m_vTwo[i].index[1];
			threelist[count1].index3 = m_vKing[0].index[0];
			count1++;
		}
		//排序
		for(int i = 0; i < count1-1; i++)
		{
			for(int j = i+1; j < count1; j++)
			{
				if(GetCardLevel(threelist[i].point) > GetCardLevel(threelist[j].point) && threelist[j].point > 0)
				{
					TThree temp = threelist[i];
					threelist[i] = threelist[j];
					threelist[j] = temp;
				}
			}
		}
		
		if(count1 >= num)
		{
			//找出所有的连牌
			int count = 0;
			for(int i =0; i < count1; i++)
			{
				if(GetCardLevel(threelist[i].point) + num -1 == GetCardLevel(threelist[i+num-1].point) )
				{//找到一个
					int tempnum = 0;
					bool bFlag = false;
					for(int j = i; j < i+num; j++)
					{
						if(threelist[j].kingcount == 1)
						{//如果是由王组成的三对
							tempnum++;
						}
						if(tempnum >= 2)
						{
							bFlag = true;
							break;
						}
					}
					if(bFlag)
						continue;//跳过两个对子的
					for(int j = i; j < i+num; j++)
					{
						pai[count].index.push_back(threelist[j].index1);
						pai[count].index.push_back(threelist[j].index2);
						pai[count].index.push_back(threelist[j].index3);
					}
					pai[count].point = threelist[i+num-1].point;
					pai[count].count = outlen;
					count++;
				}
			}
			for(int i = 0; i < count; i++)
			{
				if(GetCardLevel(pai[i].point) > GetCardLevel(outpoint))
				{
					combinelist.push_back(pai[i]);
				}
			}
		}

	}//end 1 king
	else if(kingnum == 2)
	{//有两个个王，那么对子和单牌也可以做为三对出
		for(int i =0; i < (int)m_vTwo.size(); i++)
		{
			if(GetCardLevel(m_vTwo[i].index[0]) == GetCardLevel(2))
				continue;
			threelist[count1].kingcount = 1;
			threelist[count1].point = m_vTwo[i].index[0];
			threelist[count1].index1 = m_vTwo[i].index[0];
			threelist[count1].index2 = m_vTwo[i].index[1];
			threelist[count1].index3 = m_vKing[0].index[0];
			count1++;
		}
		for(int i =0; i < (int)m_vOne.size(); i++)
		{
			if(GetCardLevel(m_vOne[i].index[0]) == GetCardLevel(2))
				continue;
			threelist[count1].kingcount = 2;
			threelist[count1].point = m_vOne[i].index[0];
			threelist[count1].index1 = m_vOne[i].index[0];
			threelist[count1].index2 = m_vKing[0].index[0];
			threelist[count1].index3 = m_vKing[1].index[0];
			count1++;
		}

		//排序
		for(int i = 0; i < count1-1; i++)
		{
			for(int j = i+1; j < count1; j++)
			{
				if(GetCardLevel(threelist[i].point) > GetCardLevel(threelist[j].point) && threelist[j].point > 0)
				{
					TThree temp = threelist[i];
					threelist[i] = threelist[j];
					threelist[j] = temp;
				}
			}
		}
		if(count1 >= num)
		{
			//找出所有的连牌
			int count = 0;
			for(int i =0; i < count1; i++)
			{
				if(GetCardLevel(threelist[i].point) + num -1 == GetCardLevel(threelist[i+num-1].point) )
				{//找到一个
					int duizi_num = 0;
					int danpai_num =0;
					bool bFlag = false;
					for(int j = i; j < i+num; j++)
					{
						if(threelist[j].kingcount == 1)
						{//如果是由王和对子组成的三对
							duizi_num++;
						}
						if(threelist[j].kingcount == 2)
						{//如果是由王和单牌组成的三对
							danpai_num++;
						}
						if(duizi_num >= 3)
						{
							bFlag = true;
							break;
						}
						if(danpai_num >= 2)
						{
							bFlag = true;
							break;
						}
						if(danpai_num == 1 && duizi_num > 0)
						{
							bFlag = true;
							break;
						}
					}
					if(bFlag)
						continue;
					if(duizi_num == 1)//只有一个对子+王
					{//那么不用管
					}
					else if(duizi_num == 2)//两个对子+王
					{
						for(int j = i; j < i+num; j++)
						{
							if(threelist[j].kingcount == 1)
							{
								threelist[j].index3 = m_vKing[1].index[0];
								break;
							}
						}
					}
					for(int j = i; j < i+num; j++)
					{
						pai[count].index.push_back(threelist[j].index1);
						pai[count].index.push_back(threelist[j].index2);
						pai[count].index.push_back(threelist[j].index3);
					}
					pai[count].point = threelist[i+num-1].point;
					pai[count].count = outlen;
					count++;
				}
			}//end 
			for(int i = 0; i < count; i++)
			{
				if(GetCardLevel(pai[i].point) > GetCardLevel(outpoint))
				{
					combinelist.push_back(pai[i]);
				}
			}
		}

	}//end 2 king
	else if(kingnum == 3)
	{
		//有三个王，那么不但单双都可以作为三对出牌，而且王本身也可以做为三双
		for(int i =0; i < (int)m_vTwo.size(); i++)
		{
			if(GetCardLevel(m_vTwo[i].index[0]) == GetCardLevel(2))
				continue;
			threelist[count1].kingcount = 1;
			threelist[count1].point = m_vTwo[i].index[0];
			threelist[count1].index1 = m_vTwo[i].index[0];
			threelist[count1].index2 = m_vTwo[i].index[1];
			threelist[count1].index3 = m_vKing[0].index[0];
			count1++;
		}
		for(int i =0; i < (int)m_vOne.size(); i++)
		{
			if(GetCardLevel(m_vOne[i].index[0]) == GetCardLevel(2))
				continue;
			threelist[count1].kingcount = 2;
			threelist[count1].point = m_vOne[i].index[0];
			threelist[count1].index1 = m_vOne[i].index[0];
			threelist[count1].index2 = m_vKing[0].index[0];
			threelist[count1].index3 = m_vKing[1].index[0];
			count1++;
		}
		
		//这里检查下，还缺少什么？因为有三王，所以，可以作为任何三对
		//0,1,2,3,4,5,6,7,8,9,....13 
		unsigned char indexlist[14];
		memset(indexlist,0,sizeof(indexlist));

		for(int i = 1; i <= 13; i++)
		{
			for(int j = 0; j < count1; j++)
			{
				if(GetCardLevel(threelist[j].index1) == GetCardLevel(i) )
				{
					indexlist[i]++;
				}
			}
		}
		
		for(int i = 1; i <= 13; i++)
		{
			if(i == 2)
				continue;
			if(indexlist[i] == 0)
			{//说明没有一张这样的牌，那么就用王来凑
				threelist[count1].kingcount = 3;
				threelist[count1].point = i;//这个才是具体的值
				threelist[count1].index1 = m_vKing[0].index[0];
				threelist[count1].index2 = m_vKing[1].index[0];
				threelist[count1].index3 = m_vKing[2].index[0];
				count1++;
			}
		}

		//排序
		for(int i = 0; i < count1-1; i++)
		{
			for(int j = i+1; j < count1; j++)
			{
				if(GetCardLevel(threelist[i].point) > GetCardLevel(threelist[j].point) && threelist[j].point > 0)
				{
					TThree temp = threelist[i];
					threelist[i] = threelist[j];
					threelist[j] = temp;
				}
			}
		}
		if(count1 >= num)
		{
			//找出所有的连牌
			int count = 0;
			for(int i =0; i < count1; i++)
			{
				if(GetCardLevel(threelist[i].point) + num -1 == GetCardLevel(threelist[i+num-1].point) )
				{//找到一个
					int duizi_num = 0;
					int danpai_num =0;
					int allking_num =0;
					bool bFlag = false;
					for(int j = i; j < i+num; j++)
					{
						if(threelist[j].kingcount == 1)
						{//如果是由王和对子组成的三对
							duizi_num++;
						}
						if(threelist[j].kingcount == 2)
						{//如果是由王和单牌组成的三对
							danpai_num++;
						}
						if(threelist[j].kingcount == 3)
						{//如果是全由王组成的三对
							allking_num++;
						}
						if(duizi_num >= 4)//最多只能有三对对子和三个王组成
						{
							bFlag = true;
							break;
						}
						if(danpai_num >= 2)//最多只能有一个单牌和一对王组成
						{
							bFlag = true;
							break;
						}
						if(danpai_num == 1 && duizi_num > 1)//单+双王，对+单王
						{
							bFlag = true;
							break;
						}
						if(allking_num > 1)//全王的，只能有一个
						{
							bFlag = true;
							break;
						}
						if(allking_num > 0 && (duizi_num >0 || danpai_num > 0))//有了一个全王的，就不能有其他的了
						{
							bFlag = true;
							break;
						}
					}
					if(bFlag)
						continue;
					if(duizi_num == 1)//只有一个对子+王
					{
						if(danpai_num == 1)
						{//这里有一对王+一个单牌
							for(int j = i; j < i+num; j++)
							{
								if(threelist[j].kingcount == 2)
								{
									threelist[j].index2 = m_vKing[1].index[0];
									threelist[j].index3 = m_vKing[2].index[0];
									break;
								}
							}
						}
					}
					else if(duizi_num == 2)//两个对子+王
					{
						for(int j = i; j < i+num; j++)
						{
							if(threelist[j].kingcount == 1)
							{
								threelist[j].index3 = m_vKing[1].index[0];
								break;
							}
						}
					}
					else if(duizi_num == 3)//三个对子+三个王
					{
						int testnum = 0;
						for(int j = i; j < i+num; j++)
						{
							if(threelist[j].kingcount == 1)
							{
								if(testnum == 0)
									threelist[j].index3 = m_vKing[1].index[0];
								else if(testnum == 1)
									threelist[j].index3 = m_vKing[2].index[0];
								else if(testnum > 1)
									break;
								testnum++;
							}
						}
					}
					if(danpai_num == 1)
					{//这里有一对王+一个单牌 不管，因为没有对子，所以不需要替换回来
					}
					if(allking_num == 1)
					{//这里也不需要再替换回来了

					}
					for(int j = i; j < i+num; j++)
					{
						pai[count].index.push_back(threelist[j].index1);
						pai[count].index.push_back(threelist[j].index2);
						pai[count].index.push_back(threelist[j].index3);
					}
					pai[count].point = threelist[i+num-1].point;
					pai[count].count = outlen;
					count++;
				}
			}
			for(int i = 0; i < count; i++)
			{
				if(GetCardLevel(pai[i].point) > GetCardLevel(outpoint))
				{
					combinelist.push_back(pai[i]);
				}
			}
		}
	}//end 3 king
	else if(kingnum == 4)
	{//4个王
		//有4个王，那么不但单双都可以作为三对出牌，而且王本身也可以做为三双
		for(int i =0; i < (int)m_vTwo.size(); i++)
		{
			if(GetCardLevel(m_vTwo[i].index[0]) == GetCardLevel(2))
				continue;
			threelist[count1].kingcount = 1;
			threelist[count1].point = m_vTwo[i].index[0];
			threelist[count1].index1 = m_vTwo[i].index[0];
			threelist[count1].index2 = m_vTwo[i].index[1];
			threelist[count1].index3 = m_vKing[0].index[0];
			count1++;
		}
		for(int i =0; i < (int)m_vOne.size(); i++)
		{
			if(GetCardLevel(m_vOne[i].index[0]) == GetCardLevel(2))
				continue;
			threelist[count1].kingcount = 2;
			threelist[count1].point = m_vOne[i].index[0];
			threelist[count1].index1 = m_vOne[i].index[0];
			threelist[count1].index2 = m_vKing[0].index[0];
			threelist[count1].index3 = m_vKing[1].index[0];
			count1++;
		}

		//这里检查下，还缺少什么？因为有三王，所以，可以作为任何三对
		//0,1,2,3,4,5,6,7,8,9,....13 
		unsigned char indexlist[14];
		memset(indexlist,0,sizeof(indexlist));

		for(int i = 1; i <= 13; i++)
		{
			for(int j = 0; j < count1; j++)
			{
				if(GetCardLevel(threelist[j].index1) == GetCardLevel(i) )
				{
					indexlist[i]++;
				}
			}
		}

		for(int i = 1; i <= 13; i++)
		{
			if(i == 2)
				continue;
			if(indexlist[i] == 0)
			{//说明没有一张这样的牌，那么就用王来凑
				threelist[count1].kingcount = 3;
				threelist[count1].point = i;
				threelist[count1].index1 = m_vKing[0].index[0];
				threelist[count1].index2 = m_vKing[1].index[0];
				threelist[count1].index3 = m_vKing[2].index[0];
				count1++;
			}
		}

		//排序
		for(int i = 0; i < count1-1; i++)
		{
			for(int j = i+1; j < count1; j++)
			{
				if(GetCardLevel(threelist[i].point) > GetCardLevel(threelist[j].point) && threelist[j].point > 0)
				{
					TThree temp = threelist[i];
					threelist[i] = threelist[j];
					threelist[j] = temp;
				}
			}
		}
		////////////////////
		if(count1 >= num)
		{
			//找出所有的连牌
			int count = 0;
			for(int i =0; i < count1; i++)
			{
				if(GetCardLevel(threelist[i].point) + num -1 == GetCardLevel(threelist[i+num-1].point) )
				{//找到一个
					int duizi_num = 0;
					int danpai_num =0;
					int allking_num =0;
					bool bFlag = false;
					for(int j = i; j < i+num; j++)
					{
						if(threelist[j].kingcount == 1)
						{//如果是由王和对子组成的三对
							duizi_num++;
						}
						if(threelist[j].kingcount == 2)
						{//如果是由王和单牌组成的三对
							danpai_num++;
						}
						if(threelist[j].kingcount == 3)
						{//如果是全由王组成的三对
							allking_num++;
						}
						if(duizi_num >= 5)//最多只能有4对对子和4个王组成
						{
							bFlag = true;
							break;
						}
						if(danpai_num >= 3)//最多只能有2个单牌和2对王组成
						{
							bFlag = true;
							break;
						}
						if(danpai_num == 2 && duizi_num > 0)//2单+2双王
						{
							bFlag = true;
							break;
						}
						if(danpai_num == 1 && duizi_num > 2)//1单+1对王，2王+2对子
						{
							bFlag = true;
							break;
						}
						if(allking_num > 1 )
						{//全王的，只能有一个,同时，只能有一个对子+王，无单牌
							bFlag = true;
							break;
						}
						if(allking_num == 1 && (duizi_num > 1 || danpai_num > 0))
						{
							bFlag = true;
							break;
						}
					}
					if(bFlag)
						continue;
					if(duizi_num == 1)//只有一个对子+王
					{
						if(danpai_num == 1)
						{//这里有一对王+一个单牌
							for(int j = i; j < i+num; j++)
							{
								if(threelist[j].kingcount == 2)
								{
									threelist[j].index2 = m_vKing[1].index[0];
									threelist[j].index3 = m_vKing[2].index[0];
									break;
								}
							}
						}
						if(allking_num == 1)
						{
							for(int j = i; j < i+num; j++)
							{
								if(threelist[j].kingcount == 3)
								{
									threelist[j].index1 = m_vKing[1].index[0];
									threelist[j].index2 = m_vKing[2].index[0];
									threelist[j].index3 = m_vKing[3].index[0];
									break;
								}
							}
						}
					}
					else if(duizi_num == 2)//两个对子+王
					{
						for(int j = i; j < i+num; j++)
						{
							if(threelist[j].kingcount == 1)
							{
								threelist[j].index3 = m_vKing[1].index[0];
								break;
							}
						}
						if(danpai_num == 1)
						{//这里有一对王+一个单牌
							for(int j = i; j < i+num; j++)
							{
								if(threelist[j].kingcount == 2)
								{
									threelist[j].index2 = m_vKing[2].index[0];
									threelist[j].index3 = m_vKing[3].index[0];
									break;
								}
							}
						}
					}
					else if(duizi_num == 3)//三个对子+三个王
					{
						int testnum = 0;
						for(int j = i; j < i+num; j++)
						{
							if(threelist[j].kingcount == 1)
							{
								if(testnum == 0)
									threelist[j].index3 = m_vKing[1].index[0];
								else if(testnum == 1)
									threelist[j].index3 = m_vKing[2].index[0];
								else if(testnum > 1)
									break;
								testnum++;
							}
						}
					}
					else if(duizi_num == 4)//4个对子+4个王
					{
						int testnum = 0;
						for(int j = i; j < i+num; j++)
						{
							if(threelist[j].kingcount == 1)
							{
								if(testnum == 0)
									threelist[j].index3 = m_vKing[1].index[0];
								else if(testnum == 1)
									threelist[j].index3 = m_vKing[2].index[0];
								else if(testnum == 2)
									threelist[j].index3 = m_vKing[3].index[0];
								else if(testnum > 2)
									break;
								testnum++;
							}
						}
					}
					if(danpai_num == 1)
					{//这里有一对王+一个单牌 上面已经替换掉了
					}
					else if(danpai_num == 2)
					{//4王+2单牌
						for(int j = i; j < i+num; j++)
						{
							if(threelist[j].kingcount == 2)
							{
								threelist[j].index2 = m_vKing[2].index[0];
								threelist[j].index3 = m_vKing[3].index[0];
								break;
							}
						}
					}
					
					for(int j = i; j < i+num; j++)
					{
						pai[count].index.push_back(threelist[j].index1);
						pai[count].index.push_back(threelist[j].index2);
						pai[count].index.push_back(threelist[j].index3);
					}
					pai[count].point = threelist[i+num-1].point;
					pai[count].count = outlen;
					count++;
				}
			}
			for(int i = 0; i < count; i++)
			{
				if(GetCardLevel(pai[i].point) > GetCardLevel(outpoint))
				{
					combinelist.push_back(pai[i]);
				}
			}
		}//end if (count1 > num)
	}//end 4 king

	//最后可以做一次优化，让没有王的先提示，有王的后提示
	if(kingnum >0)SortBeepCardList(combinelist);
}//end GetCombineThreeStr

void CDG4::GetCombineTwoStr( unsigned char outpoint,int outlen,vector<TPokerGroup2>& combinelist )
{
	int kingnum = (int)m_vKing.size();
	//先找出所有两对

	TTwo twolist[MAXPAI];
	memset(twolist,0,sizeof(twolist));
	//把所有的两对列出来
	int count1 = 0;
	for(int i =0; i < (int)m_vTwo.size(); i++)
	{
		if(GetCardLevel(m_vTwo[i].index[0]) == GetCardLevel(2))
			continue;
		twolist[count1].kingcount  = 0;
		twolist[count1].point = m_vTwo[i].index[0];
		twolist[count1].index1 = m_vTwo[i].index[0];
		twolist[count1].index2 = m_vTwo[i].index[1];
		count1++;
	}
	for(unsigned int i =0; i < m_vThree.size(); i++)
	{
		if(GetCardLevel(m_vThree[i].index[0]) == GetCardLevel(2))
			continue;
		twolist[count1].kingcount  = 0;
		twolist[count1].point = m_vThree[i].index[0];
		twolist[count1].index1 = m_vThree[i].index[0];
		twolist[count1].index2 = m_vThree[i].index[1];
		count1++;
	}
	for(int i =0; i < m_vFour.size(); i++)
	{
		if(GetCardLevel(m_vFour[i].index[0]) == GetCardLevel(2))
			continue;
		twolist[count1].kingcount  = 0;
		twolist[count1].point = m_vFour[i].index[0];
		twolist[count1].index1 = m_vFour[i].index[0];
		twolist[count1].index2 = m_vFour[i].index[1];
		count1++;
	}
	for(int i =0; i < m_vFive.size(); i++)
	{
		if(GetCardLevel(m_vFive[i].index[0]) == GetCardLevel(2))
			continue;
		twolist[count1].kingcount  = 0;
		twolist[count1].point = m_vFive[i].index[0];
		twolist[count1].index1 = m_vFive[i].index[0];
		twolist[count1].index2 = m_vFive[i].index[1];
		count1++;
	}
	for(int i =0; i < m_vSix.size(); i++)
	{
		if(GetCardLevel(m_vSix[i].index[0]) == GetCardLevel(2))
			continue;
		twolist[count1].kingcount  = 0;
		twolist[count1].point = m_vSix[i].index[0];
		twolist[count1].index1 = m_vSix[i].index[0];
		twolist[count1].index2 = m_vSix[i].index[1];
		count1++;
	}
	for(int i =0; i < m_vSeven.size(); i++)
	{
		if(GetCardLevel(m_vSeven[i].index[0]) == GetCardLevel(2))
			continue;
		twolist[count1].kingcount  = 0;
		twolist[count1].point = m_vSeven[i].index[0];
		twolist[count1].index1 = m_vSeven[i].index[0];
		twolist[count1].index2 = m_vSeven[i].index[1];
		count1++;
	}
	for(int i =0; i < m_vEight.size(); i++)
	{
		if(GetCardLevel(m_vEight[i].index[0]) == GetCardLevel(2))
			continue;
		twolist[count1].kingcount  = 0;
		twolist[count1].point = m_vEight[i].index[0];
		twolist[count1].index1 = m_vEight[i].index[0];
		twolist[count1].index2 = m_vEight[i].index[1];
		count1++;
	}

	int num = outlen/2;//连对数
	TPokerGroup2 pai[MAXPAI];
	if(kingnum == 0)
	{
		//排序
		for(int i = 0; i < count1-1; i++)
		{
			for(int j = i+1; j < count1; j++)
			{
				if(GetCardLevel(twolist[i].point) > GetCardLevel(twolist[j].index1) && twolist[j].point > 0)
				{
					TTwo temp = twolist[i];
					twolist[i] = twolist[j];
					twolist[j] = temp;
				}
			}
		}

		if(count1 >= num)
		{
			//找出所有的连牌
			int count = 0;
			for(int i =0; i < count1; i++)
			{
				if(GetCardLevel(twolist[i].point) + num -1 == GetCardLevel(twolist[i+num-1].point) )
				{//找到一个
					for(int j = i; j < i+num; j++)
					{
						pai[count].index.push_back(twolist[j].index1);
						pai[count].index.push_back(twolist[j].index2);
					}
					pai[count].point = twolist[i+num-1].point;
					pai[count].count = outlen;
					count++;
				}
			}
			for(int i = 0; i < count; i++)
			{
				if(GetCardLevel(pai[i].point) > GetCardLevel(outpoint))
				{
					combinelist.push_back(pai[i]);
				}
			}
		}
	}
	else if(kingnum == 1)
	{//有一个王，那么单牌子也可以做为2对出
		for(int i =0; i < m_vOne.size(); i++)
		{
			if(GetCardLevel(m_vOne[i].index[0]) == GetCardLevel(2))
				continue;
			twolist[count1].kingcount  = 1;
			twolist[count1].point = m_vOne[i].index[0];
			twolist[count1].index1 = m_vOne[i].index[0];
			twolist[count1].index2 = m_vKing[0].index[0];
			count1++;
		}
		//排序
		for(int i = 0; i < count1-1; i++)
		{
			for(int j = i+1; j < count1; j++)
			{
				if(GetCardLevel(twolist[i].point) > GetCardLevel(twolist[j].index1) && twolist[j].point > 0)
				{
					TTwo temp = twolist[i];
					twolist[i] = twolist[j];
					twolist[j] = temp;
				}
			}
		}

		if(count1 >= num)
		{
			//找出所有的连牌
			int count = 0;
			for(int i =0; i < count1; i++)
			{
				if(GetCardLevel(twolist[i].point) + num -1 == GetCardLevel(twolist[i+num-1].point) )
				{//找到一个
					int tempnum = 0;
					bool bFlag = false;
					for(int j = i; j < i+num; j++)
					{
						if(twolist[j].kingcount == 1)
						{//如果是由王组成的2对
							tempnum++;
						}
						if(tempnum >= 2)
						{
							bFlag = true;
							break;
						}
					}
					if(bFlag)
						continue;//如果连对里有两个王。。当然是不允许的，
					for(int j = i; j < i+num; j++)
					{
						pai[count].index.push_back(twolist[j].index1);
						pai[count].index.push_back(twolist[j].index2);
					}
					pai[count].point = twolist[i+num-1].point;
					pai[count].count = outlen;
					count++;
				}
			}
			for(int i = 0; i < count; i++)
			{
				if(GetCardLevel(pai[i].point) > GetCardLevel(outpoint))
				{
					combinelist.push_back(pai[i]);
				}
			}
		}

	}//end 1 king
	else if(kingnum == 2)
	{//有两个个王
		for(int i =0; i < m_vOne.size(); i++)
		{
			if(GetCardLevel(m_vOne[i].index[0]) == GetCardLevel(2))
				continue;
			twolist[count1].kingcount  = 1;
			twolist[count1].point = m_vOne[i].index[0];
			twolist[count1].index1 = m_vOne[i].index[0];
			twolist[count1].index2 = m_vKing[0].index[0];
			count1++;
		}

		//这里检查下，还缺少什么？因为有两王，所以，可以作为任何两对
		//0,1,2,3,4,5,6,7,8,9,....13 
		unsigned char indexlist[14];
		memset(indexlist,0,sizeof(indexlist));

		for(int i = 1; i <= 13; i++)
		{
			for(int j = 0; j < count1; j++)
			{
				if(GetCardLevel(twolist[j].index1) == GetCardLevel(i) )
				{
					indexlist[i]++;
				}
			}
		}

		for(int i = 1; i <= 13; i++)
		{
			if(i == 2)
				continue;
			if(indexlist[i] == 0)
			{//说明没有一张这样的牌，那么就用王来凑
				twolist[count1].kingcount = 2;
				twolist[count1].point = i;
				twolist[count1].index1 = m_vKing[0].index[0];
				twolist[count1].index2 = m_vKing[1].index[0];
				count1++;
			}
		}

		//排序
		for(int i = 0; i < count1-1; i++)
		{
			for(int j = i+1; j < count1; j++)
			{
				if(GetCardLevel(twolist[i].point) > GetCardLevel(twolist[j].point) && twolist[j].point > 0)
				{
					TTwo temp = twolist[i];
					twolist[i] = twolist[j];
					twolist[j] = temp;
				}
			}
		}

		if(count1 >= num)
		{
			//找出所有的连牌
			int count = 0;
			for(int i =0; i < count1; i++)
			{
				if(GetCardLevel(twolist[i].point) + num -1 == GetCardLevel(twolist[i+num-1].point) )
				{//找到一个
					int danpai_num =0;
					int allking_num =0;
					bool bFlag = false;
					for(int j = i; j < i+num; j++)
					{
						if(twolist[j].kingcount == 1)
						{//如果是由王和单牌组成的2对
							danpai_num++;
						}
						if(twolist[j].kingcount == 2)
						{//如果是全由王组成的2对
							allking_num++;
						}
						if(danpai_num > 2)//最多只能有2个单牌和一对王组成
						{
							bFlag = true;
							break;
						}
						if(allking_num > 1)//全王的，只能有一个
						{
							bFlag = true;
							break;
						}
						if(allking_num > 0 && danpai_num > 0)//有了一个全王的，就不能有其他的了
						{
							bFlag = true;
							break;
						}
					}
					if(bFlag)
						continue;
					if(danpai_num == 1)//只有一个单+王
					{
					}
					else if(danpai_num == 2)//2单牌+2王
					{
						for(int j = i; j < i+num; j++)
						{
							if(twolist[j].kingcount == 1)
							{
								twolist[j].index2 = m_vKing[1].index[0];
								break;
							}
						}
					}
					if(allking_num == 1)
					{//这里也不需要再替换回来了

					}
					for(int j = i; j < i+num; j++)
					{
						pai[count].index.push_back(twolist[j].index1);
						pai[count].index.push_back(twolist[j].index2);
					}
					pai[count].point = twolist[i+num-1].point;
					pai[count].count = outlen;
					count++;
				}
			}
			for(int i = 0; i < count; i++)
			{
				if(GetCardLevel(pai[i].point) > GetCardLevel(outpoint))
				{
					combinelist.push_back(pai[i]);
				}
			}
		}

	}//end 2 king
	else if(kingnum == 3)
	{
		for(int i =0; i < m_vOne.size(); i++)
		{
			if(GetCardLevel(m_vOne[i].index[0]) == GetCardLevel(2))
				continue;
			twolist[count1].kingcount = 1;
			twolist[count1].point = m_vOne[i].index[0];
			twolist[count1].index1 = m_vOne[i].index[0];
			twolist[count1].index2 = m_vKing[0].index[0];
			count1++;
		}

		//这里检查下，还缺少什么？因为有三王，所以，可以作为任何三对
		//0,1,2,3,4,5,6,7,8,9,....13 
		unsigned char indexlist[14];
		memset(indexlist,0,sizeof(indexlist));

		for(int i = 1; i <= 13; i++)
		{
			for(int j = 0; j < count1; j++)
			{
				if(GetCardLevel(twolist[j].index1) == GetCardLevel(i) )
				{
					indexlist[i]++;
				}
			}
		}

		for(int i = 1; i <= 13; i++)
		{
			if(i == 2)
				continue;
			if(indexlist[i] == 0)
			{//说明没有一张这样的牌，那么就用王来凑
				twolist[count1].kingcount = 2;
				twolist[count1].point = i;
				twolist[count1].index1 = m_vKing[0].index[0];
				twolist[count1].index2 = m_vKing[1].index[0];
				count1++;
			}
		}

		//排序
		for(int i = 0; i < count1-1; i++)
		{
			for(int j = i+1; j < count1; j++)
			{
				if(GetCardLevel(twolist[i].point) > GetCardLevel(twolist[j].point) && twolist[j].point > 0)
				{
					TTwo temp = twolist[i];
					twolist[i] = twolist[j];
					twolist[j] = temp;
				}
			}
		}
		if(count1 >= num)
		{
			//找出所有的连牌
			int count = 0;
			for(int i =0; i < count1; i++)
			{
				if(GetCardLevel(twolist[i].point) + num -1 == GetCardLevel(twolist[i+num-1].point) )
				{//找到一个
					int danpai_num =0;
					int allking_num =0;
					bool bFlag = false;
					for(int j = i; j < i+num; j++)
					{
						if(twolist[j].kingcount == 1)
						{//如果是由王和单牌组成的2对
							danpai_num++;
						}
						if(twolist[j].kingcount == 2)
						{//如果是全由王组成的2对
							allking_num++;
						}
						if(danpai_num > 3)//最多只能有3个单牌和3个王组成
						{
							bFlag = true;
							break;
						}
						if(allking_num > 1)//全王的，只能有一个
						{
							bFlag = true;
							break;
						}
						if(allking_num ==1 &&  danpai_num > 1)//有了一个全王的，最多只能有1个单+王
						{
							bFlag = true;
							break;
						}
					}
					if(bFlag)
						continue;
					if(allking_num == 1)
					{
						if(danpai_num == 1)
						{//这里有一对王+一个单牌
							for(int j = i; j < i+num; j++)
							{
								if(twolist[j].kingcount == 1)
								{
									twolist[j].index2 = m_vKing[2].index[0];
									break;
								}
							}
						}
					}
					else if(allking_num == 0)
					{
						if(danpai_num == 1)
						{

						}
						else if(danpai_num == 2)
						{
							for(int j = i; j < i+num; j++)
							{
								if(twolist[j].kingcount == 1)
								{
									twolist[j].index2 = m_vKing[1].index[0];
									break;
								}
							}
						}
						else if(danpai_num == 3)
						{
							int testnum = 0;
							for(int j = i; j < i+num; j++)
							{
								if(twolist[j].kingcount == 1)
								{
									if(testnum == 0)
										twolist[j].index2 = m_vKing[1].index[0];
									else if(testnum == 1)
										twolist[j].index2 = m_vKing[2].index[0];
									else if(testnum > 1)
										break;
									testnum++;
								}
							}
						}
					}
					for(int j = i; j < i+num; j++)
					{
						pai[count].index.push_back(twolist[j].index1);
						pai[count].index.push_back(twolist[j].index2);
					}
					pai[count].point = twolist[i+num-1].point;
					pai[count].count = outlen;
					count++;
				}
			}
			for(int i = 0; i < count; i++)
			{
				if(GetCardLevel(pai[i].point) > GetCardLevel(outpoint))
				{
					combinelist.push_back(pai[i]);
				}
			}
		}
	}//end 3 king
	else if(kingnum == 4)
	{//4个王
		for(int i =0; i < m_vOne.size(); i++)
		{
			if(GetCardLevel(m_vOne[i].index[0]) == GetCardLevel(2))
				continue;
			twolist[count1].kingcount = 1;
			twolist[count1].point = m_vOne[i].index[0];
			twolist[count1].index1 = m_vOne[i].index[0];
			twolist[count1].index2 = m_vKing[0].index[0];
			count1++;
		}

		//这里检查下，还缺少什么？因为有三王，所以，可以作为任何三对
		//0,1,2,3,4,5,6,7,8,9,....13 
		unsigned char indexlist[14];
		memset(indexlist,0,sizeof(indexlist));

		for(int i = 1; i <= 13; i++)
		{
			for(int j = 0; j < count1; j++)
			{
				if(GetCardLevel(twolist[count1].index1) == GetCardLevel(i) )
				{
					indexlist[i]++;
				}
			}
		}

		for(int i = 1; i <= 13; i++)
		{
			if(i == 2)
				continue;
			if(indexlist[i] == 0)
			{//说明没有一张这样的牌，那么就用王来凑
				twolist[count1].kingcount = 2;
				twolist[count1].point = i;
				twolist[count1].index1 = m_vKing[0].index[0];
				twolist[count1].index2 = m_vKing[1].index[0];
				count1++;
			}
		}

		//排序
		for(int i = 0; i < count1-1; i++)
		{
			for(int j = i+1; j < count1; j++)
			{
				if(GetCardLevel(twolist[i].point) > GetCardLevel(twolist[j].point) && twolist[j].point > 0)
				{
					TTwo temp = twolist[i];
					twolist[i] = twolist[j];
					twolist[j] = temp;
				}
			}
		}
		////////////////////
		if(count1 >= num)
		{
			//找出所有的连牌
			int count = 0;
			for(int i =0; i < count1; i++)
			{
				if(GetCardLevel(twolist[i].point) + num -1 == GetCardLevel(twolist[i+num-1].point) )
				{//找到一个
					int danpai_num =0;
					int allking_num =0;
					bool bFlag = false;
					for(int j = i; j < i+num; j++)
					{
						if(twolist[j].kingcount == 1)
						{
							danpai_num++;
						}
						if(twolist[j].kingcount == 2)
						{//如果是全由王组成的2对
							allking_num++;
						}
						if(danpai_num > 4)//最多只能有4个单牌和4王组成
						{
							bFlag = true;
							break;
						}
						if(allking_num > 2 )
						{//全王的，只能有两个
							bFlag = true;
							break;
						}
						if(allking_num == 1 && danpai_num > 2)//1全王，2单牌+2王
						{
							bFlag = true;
							break;
						}
						if(allking_num == 2 && danpai_num > 0)//2全王，无其他
						{
							bFlag = true;
							break;
						}
					}
					if(bFlag)
						continue;
					
					if(allking_num == 1)
					{
						if(danpai_num == 1)
						{//这里有一对王+一个单牌
							for(int j = i; j < i+num; j++)
							{
								if(twolist[j].kingcount == 1)
								{
									twolist[j].index2 = m_vKing[2].index[0];
									break;
								}
							}
						}
						else if(danpai_num == 2)
						{
							int testnum = 0;
							for(int j = i; j < i+num; j++)
							{
								if(twolist[j].kingcount == 1)
								{
									if(testnum == 0)
										twolist[j].index2 = m_vKing[2].index[0];
									else if(testnum == 1)
										twolist[j].index2 = m_vKing[3].index[0];
									else
										break;
									testnum++;
								}
							}
						}
					}
					else if(allking_num == 2)
					{
						for(int j = i; j < i+num; j++)
						{
							if(twolist[j].kingcount == 2)
							{
								twolist[j].index1 = m_vKing[2].index[0];
								twolist[j].index2 = m_vKing[3].index[0];
								break;
							}
						}
					}
					else if(allking_num == 0)
					{
						if(danpai_num == 1)
						{

						}
						else if(danpai_num == 2)
						{
							for(int j = i; j < i+num; j++)
							{
								if(twolist[j].kingcount == 1)
								{
									twolist[j].index2 = m_vKing[1].index[0];
									break;
								}
							}
						}
						else if(danpai_num == 3)
						{
							int testnum = 0;
							for(int j = i; j < i+num; j++)
							{
								if(twolist[j].kingcount == 1)
								{
									if(testnum == 0)
										twolist[j].index2 = m_vKing[1].index[0];
									else if(testnum == 1)
										twolist[j].index2 = m_vKing[2].index[0];
									else
										break;
									testnum++;
								}
							}
						}
						else if(danpai_num == 4)
						{
							int testnum = 0;
							for(int j = i; j < i+num; j++)
							{
								if(twolist[j].kingcount == 1)
								{
									if(testnum == 0)
										twolist[j].index2 = m_vKing[1].index[0];
									else if(testnum == 1)
										twolist[j].index2 = m_vKing[2].index[0];
									else if(testnum == 2)
										twolist[j].index2 = m_vKing[3].index[0];
									else
										break;
									testnum++;
								}
							}
						}
					}
					for(int j = i; j < i+num; j++)
					{
						pai[count].index.push_back(twolist[j].index1);
						pai[count].index.push_back(twolist[j].index2);
					}
					pai[count].point = twolist[i+num-1].point;
					pai[count].count = outlen;
					count++;
				}
			}
			for(int i = 0; i < count; i++)
			{
				if(GetCardLevel(pai[i].point) > GetCardLevel(outpoint))
				{
					combinelist.push_back(pai[i]);
				}
			}
		}//end if (count1 > num)
	}//end 4 king
	if(kingnum >0) SortBeepCardList(combinelist);
}//end GetCombineTwoStr

void CDG4::GetCombineThree( unsigned char outpoint,vector<TPokerGroup2>& combinelist )
{
	combinelist.clear();
	int kingnum = m_vKing.size();
	//先找出所有三对

	TThree threelist[MAXPAI];
	memset(threelist,0,sizeof(threelist));
	//把所有的三牌列出来
	int count1 = 0;
	for(int i =0; i < m_vThree.size(); i++)
	{
		threelist[count1].kingcount  = 0;
		threelist[count1].point = m_vThree[i].index[0];
		threelist[count1].index1 = m_vThree[i].index[0];
		threelist[count1].index2 = m_vThree[i].index[1];
		threelist[count1].index3 = m_vThree[i].index[2];
		count1++;
	}
	for(int i =0; i < m_vFour.size(); i++)
	{
		threelist[count1].kingcount  = 0;
		threelist[count1].point = m_vFour[i].index[0];
		threelist[count1].index1 = m_vFour[i].index[0];
		threelist[count1].index2 = m_vFour[i].index[1];
		threelist[count1].index3 = m_vFour[i].index[2];
		count1++;
	}
	for(int i =0; i < m_vFive.size(); i++)
	{
		threelist[count1].kingcount  = 0;
		threelist[count1].point = m_vFive[i].index[0];
		threelist[count1].index1 = m_vFive[i].index[0];
		threelist[count1].index2 = m_vFive[i].index[1];
		threelist[count1].index3 = m_vFive[i].index[2];
		count1++;
	}
	for(int i =0; i < m_vSix.size(); i++)
	{
		threelist[count1].kingcount  = 0;
		threelist[count1].point = m_vSix[i].index[0];
		threelist[count1].index1 = m_vSix[i].index[0];
		threelist[count1].index2 = m_vSix[i].index[1];
		threelist[count1].index3 = m_vSix[i].index[2];
		count1++;
	}
	for(int i =0; i < m_vSeven.size(); i++)
	{
		threelist[count1].kingcount  = 0;
		threelist[count1].point = m_vSeven[i].index[0];
		threelist[count1].index1 = m_vSeven[i].index[0];
		threelist[count1].index2 = m_vSeven[i].index[1];
		threelist[count1].index3 = m_vSeven[i].index[2];
		count1++;
	}
	for(int i =0; i < m_vEight.size(); i++)
	{
		threelist[count1].kingcount  = 0;
		threelist[count1].point = m_vEight[i].index[0];
		threelist[count1].index1 = m_vEight[i].index[0];
		threelist[count1].index2 = m_vEight[i].index[1];
		threelist[count1].index3 = m_vEight[i].index[2];
		count1++;
	}
	TPokerGroup2 group;
	if(kingnum == 1)
	{
		for(int i =0; i < m_vTwo.size(); i++)
		{
			threelist[count1].kingcount  = 1;
			threelist[count1].point = m_vTwo[i].index[0];
			threelist[count1].index1 = m_vTwo[i].index[0];
			threelist[count1].index2 = m_vTwo[i].index[1];
			threelist[count1].index3 = m_vKing[0].index[0];
			count1++;
		}
	}
	else if(kingnum >= 2)
	{
		for(int i =0; i < m_vTwo.size(); i++)
		{
			threelist[count1].kingcount  = 1;
			threelist[count1].point = m_vTwo[i].index[0];
			threelist[count1].index1 = m_vTwo[i].index[0];
			threelist[count1].index2 = m_vTwo[i].index[1];
			threelist[count1].index3 = m_vKing[0].index[0];
			count1++;
		}
		for(int i =0; i < m_vOne.size(); i++)
		{
			threelist[count1].kingcount  = 1;
			threelist[count1].point = m_vOne[i].index[0];
			threelist[count1].index1 = m_vOne[i].index[0];
			threelist[count1].index2 = m_vKing[0].index[0];
			threelist[count1].index3 = m_vKing[1].index[0];
			count1++;
		}
	}
	//不可能全部用王来做三对，那就是三个3了
	for(int i = 0; i < count1; i++)
	{
		if(GetCardLevel(threelist[i].point) > GetCardLevel(outpoint))
		{
			group.color = 0;
			group.count = 3;
			group.point = threelist[i].point;
			group.index.clear();
			group.index.push_back(threelist[i].index1);
			group.index.push_back(threelist[i].index2);
			group.index.push_back(threelist[i].index3);
			combinelist.push_back(group);
		}
	}
	if(kingnum >0) SortBeepCardList(combinelist);
}

void CDG4::GetCombineTwo( unsigned char outpoint,vector<TPokerGroup2>& combinelist )
{
	combinelist.clear();
	int kingnum = m_vKing.size();
	//先找出所有三对

	TTwo twolist[MAXPAI];
	memset(twolist,0,sizeof(twolist));
	//把所有的三牌列出来
	int count1 = 0;
	for(int i =0; i < m_vTwo.size(); i++)
	{
		twolist[count1].kingcount  = 0;
		twolist[count1].point = m_vTwo[i].index[0];
		twolist[count1].index1 = m_vTwo[i].index[0];
		twolist[count1].index2 = m_vTwo[i].index[1];
		count1++;
	}
	for(int i =0; i < m_vThree.size(); i++)
	{
		twolist[count1].kingcount  = 0;
		twolist[count1].point = m_vThree[i].index[0];
		twolist[count1].index1 = m_vThree[i].index[0];
		twolist[count1].index2 = m_vThree[i].index[1];
		count1++;
	}
	for(int i =0; i < m_vFour.size(); i++)
	{
		twolist[count1].kingcount  = 0;
		twolist[count1].point = m_vFour[i].index[0];
		twolist[count1].index1 = m_vFour[i].index[0];
		twolist[count1].index2 = m_vFour[i].index[1];
		count1++;
	}
	for(int i =0; i < m_vFive.size(); i++)
	{
		twolist[count1].kingcount  = 0;
		twolist[count1].point = m_vFive[i].index[0];
		twolist[count1].index1 = m_vFive[i].index[0];
		twolist[count1].index2 = m_vFive[i].index[1];
		count1++;
	}
	for(int i =0; i < m_vSix.size(); i++)
	{
		twolist[count1].kingcount  = 0;
		twolist[count1].point = m_vSix[i].index[0];
		twolist[count1].index1 = m_vSix[i].index[0];
		twolist[count1].index2 = m_vSix[i].index[1];
		count1++;
	}
	for(int i =0; i < m_vSeven.size(); i++)
	{
		twolist[count1].kingcount  = 0;
		twolist[count1].point = m_vSeven[i].index[0];
		twolist[count1].index1 = m_vSeven[i].index[0];
		twolist[count1].index2 = m_vSeven[i].index[1];
		count1++;
	}
	for(int i =0; i < m_vEight.size(); i++)
	{
		twolist[count1].kingcount  = 0;
		twolist[count1].point = m_vEight[i].index[0];
		twolist[count1].index1 = m_vEight[i].index[0];
		twolist[count1].index2 = m_vEight[i].index[1];
		count1++;
	}
	TPokerGroup2 group;
	if(kingnum > 0 )
	{
		for(int i =0; i < m_vOne.size(); i++)
		{
			twolist[count1].kingcount  = 1;
			twolist[count1].point = m_vOne[i].index[0];
			twolist[count1].index1 = m_vOne[i].index[0];
			twolist[count1].index2 = m_vKing[0].index[0];
			count1++;
		}
	}
	//不可能全部用王来做2对，那就是2个3了

	for(int i = 0; i < count1; i++)
	{
		if(GetCardLevel(twolist[i].point) > GetCardLevel(outpoint))
		{
			group.color = 0;
			group.count = 2;
			group.point = twolist[i].point;
			group.index.clear();
			group.index.push_back(twolist[i].index1);
			group.index.push_back(twolist[i].index2);
			combinelist.push_back(group);
		}
	}
	if(kingnum >0) SortBeepCardList(combinelist);
}

void CDG4::GetAllOne( unsigned char outpoint,vector<TPokerGroup2>& combinelist )
{
	TPokerGroup2 group;
	combinelist.clear();
	for(vector<TPokerGroup2>::iterator it = m_vOne.begin(); it != m_vOne.end(); it++)
	{
		if(GetCardLevel(it->point) > GetCardLevel(outpoint))
		{
			group.index.clear();
			group.count = 1;
			group.point = it->point;
			group.index.clear();
			group.index.push_back(it->index[0]);
			combinelist.push_back(group);
		}
	}
	for(vector<TPokerGroup2>::iterator it = m_vTwo.begin(); it != m_vTwo.end(); it++)
	{
		if(GetCardLevel(it->point) > GetCardLevel(outpoint))
		{
			group.index.clear();
			group.count = 1;
			group.point = it->point;
			group.index.clear();
			group.index.push_back(it->index[0]);
			combinelist.push_back(group);
		}
	}
	for(vector<TPokerGroup2>::iterator it = m_vThree.begin(); it != m_vThree.end(); it++)
	{
		if(GetCardLevel(it->point) > GetCardLevel(outpoint))
		{
			group.index.clear();
			group.count = 1;
			group.point = it->point;
			group.index.clear();
			group.index.push_back(it->index[0]);
			combinelist.push_back(group);
		}
	}
	for(vector<TPokerGroup2>::iterator it = m_vFour.begin(); it != m_vFour.end(); it++)
	{
		if(GetCardLevel(it->point) > GetCardLevel(outpoint))
		{
			group.index.clear();
			group.count = 1;
			group.point = it->point;
			group.index.clear();
			group.index.push_back(it->index[0]);
			combinelist.push_back(group);
		}
	}
	for(vector<TPokerGroup2>::iterator it = m_vFive.begin(); it != m_vFive.end(); it++)
	{
		if(GetCardLevel(it->point) > GetCardLevel(outpoint))
		{
			group.index.clear();
			group.count = 1;
			group.point = it->point;
			group.index.clear();
			group.index.push_back(it->index[0]);
			combinelist.push_back(group);
		}
	}
	for(vector<TPokerGroup2>::iterator it = m_vSix.begin(); it != m_vSix.end(); it++)
	{
		if(GetCardLevel(it->point) > GetCardLevel(outpoint))
		{
			group.index.clear();
			group.count = 1;
			group.point = it->point;
			group.index.clear();
			group.index.push_back(it->index[0]);
			combinelist.push_back(group);
		}
	}
	for(vector<TPokerGroup2>::iterator it = m_vSeven.begin(); it != m_vSeven.end(); it++)
	{
		if(GetCardLevel(it->point) > GetCardLevel(outpoint))
		{
			group.index.clear();
			group.count = 1;
			group.point = it->point;
			group.index.clear();
			group.index.push_back(it->index[0]);
			combinelist.push_back(group);
		}
	}
	for(vector<TPokerGroup2>::iterator it = m_vEight.begin(); it != m_vEight.end(); it++)
	{
		if(GetCardLevel(it->point) > GetCardLevel(outpoint))
		{
			group.index.clear();
			group.count = 1;
			group.point = it->point;
			group.index.clear();
			group.index.push_back(it->index[0]);
			combinelist.push_back(group);
		}
	}
}

void CDG4::SortBeepCardList( vector<TPokerGroup2>& combinelist )
{
	//for(int i =0; i < combinelist.size()-1; i++)
	//{
	//	for(int j = i+1; j < combinelist.size(); j++)
	//	{
	//		if(GetCardLevel(combinelist[i].point) > GetCardLevel(combinelist[j].point) && combinelist[j].point > 0)
	//		{
	//			TPokerGroup2 temp = combinelist[i];
	//			combinelist[i] = combinelist[j];
	//			combinelist[j] = temp;
	//		}
	//	}
	//}
	vector<TPokerGroup2> templist0;
	vector<TPokerGroup2> templist1;
	vector<TPokerGroup2> templist2;
	vector<TPokerGroup2> templist3;
	vector<TPokerGroup2> templist4;
	templist0.clear();
	templist1.clear();
	templist2.clear();
	templist3.clear();
	templist4.clear();

	for(int i =0; i < combinelist.size(); i++)
	{
		int nNum = 0;
		for(int j = 0; j < combinelist[i].index.size(); j++)
		{
			if(IsKing(combinelist[i].index[j]))
			{
				nNum++;
			}
		}
		if(nNum == 0)
		{
			templist0.push_back(combinelist[i]);
		}
		else if(nNum == 1)
		{
			templist1.push_back(combinelist[i]);
		}
		else if(nNum == 2)
		{
			templist2.push_back(combinelist[i]);
		}
		else if(nNum == 3)
		{
			templist3.push_back(combinelist[i]);
		}
		else if(nNum == 4)
		{
			templist4.push_back(combinelist[i]);
		}
	}
	combinelist.clear();
	for(int i = 0; i < templist0.size(); i++)
		combinelist.push_back(templist0[i]);
	for(int i = 0; i < templist1.size(); i++)
		combinelist.push_back(templist1[i]);
	for(int i = 0; i < templist2.size(); i++)
		combinelist.push_back(templist2[i]);
	for(int i = 0; i < templist3.size(); i++)
		combinelist.push_back(templist3[i]);
	for(int i = 0; i < templist4.size(); i++)
		combinelist.push_back(templist4[i]);
}