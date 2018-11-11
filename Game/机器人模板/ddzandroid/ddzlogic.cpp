
//
// 2011
//

#include "ddzlogic.h"

const BYTE GAME_USER_NUMS = 3;

const BYTE g_byCardData[55]={0, 0x33, 0x23, 0x13, 0x03, 0x34, 0x24, 0x14, 0x04, 
								0x35, 0x25, 0x15, 0x05, 0x36, 0x26, 0x16, 0x06, 
								0x37, 0x27, 0x17, 0x07, 0x38, 0x28, 0x18, 0x08, 
								0x39, 0x29, 0x19, 0x09, 0x3A, 0x2A, 0x1A, 0x0A, 
								0x3B, 0x2B, 0x1B, 0x0B, 0x3C, 0x2C, 0x1C, 0x0C, 
								0x3D, 0x2D, 0x1D, 0x0D, 0x31, 0x21, 0x11, 0x01, 
								0x32, 0x22, 0x12, 0x02, 0x4E, 0x4F};


//数组维数
#define CountArray(Array) (sizeof(Array)/sizeof(Array[0]))

//
// *
//
CDDZLogic::CDDZLogic( void ) 
{
	m_aiAndroid = new KddzAI( &m_rAndroid ) ;

	m_aiRun.cc = ctrue;

	m_byMeChairID = -1;
	m_byLandChairID = -1;
}

//
// *
//
CDDZLogic::~CDDZLogic( void )
{
}

int CDDZLogic::SwitchCardDataToInt(BYTE byCardData)
{
	if (0==byCardData || byCardData>0x4F)
	{
		return 0;
	}
	
	BYTE byColor = GetCardColor(byCardData)>>4;
	BYTE byValue = GetCardValue(byCardData);
	if (0 == byValue || byColor>4)
	{
		char chData[64]={0};
		::sprintf(chData, "SwitchCardDataToInt转换出错！-->%x", byCardData);
		AfxMessageBox(chData);
		return 0;
	}

	if (1 == byValue)
	{//A
		return 48-byColor;
	}
	else if (2 == byValue)
	{//2
		return 52-byColor;
	}
	else if (4 == byColor)
	{//王
		if (0x4E == byCardData)
		{
			return 53;
		}
		else if (0x4F == byCardData)
		{
			return 54;
		}
	}
	int iValue = (byValue-3)*4+1+3-byColor;
	return iValue;
}

BYTE CDDZLogic::SwitchCardDataToByte(int iCardData)
{
	if (0==iCardData || iCardData>54)
	{
		return 0;
	}

	//方法1
	return g_byCardData[iCardData];

	//方法2
	//if (iCardData>=53&&iCardData<=54)
	//{//王
	//	return 0x4E+(iCardData-53);
	//}
	//else if (iCardData>=45&&iCardData<=48)
	//{//A
	//	return ((48-iCardData)<<4)+1;
	//}
	//else if (iCardData>=49&&iCardData<=52)
	//{//2
	//	return ((52-iCardData)<<4)+2;
	//}
	//
	//BYTE byColor = 3-(((iCardData%4)+4-1)%4);
	//BYTE byValue = 3+(iCardData/4);
	//
	//return (byColor<<4)+byValue;
}

//转换牌数据.
bool CDDZLogic::SwitchBYTEtoINT(const BYTE* pCard, BYTE byNums, int* pRetCard, int& iRetNums)
{
	if (NULL==pCard || 0==byNums || NULL==pRetCard)
	{
		return false;
	}

	iRetNums = 0;
	for (BYTE i=0; i<byNums; i++)
	{
		if (0 != pCard[i])
		{
			int iValue = SwitchCardDataToInt(pCard[i]);
			if (iValue>=1 && iValue<=54)
			{
				pRetCard[iRetNums] = iValue;
				++iRetNums;
			}
		}
	}
	return (iRetNums==byNums)?true:false;
}

bool CDDZLogic::SwitchINTtoBYTE(const int* pCard, int iNums, BYTE* pRetCard, BYTE& byRetNums)
{
	if (NULL==pCard || 0==iNums || NULL==pRetCard)
	{
		return false;
	}
	byRetNums = 0;
	for (int i=0; i<iNums; i++)
	{
		if (pCard[i]>=1 && pCard[i]<=54)
		{
			BYTE byValue = SwitchCardDataToByte(pCard[i]);
			if (0 != byValue)
			{
				pRetCard[byRetNums] = byValue;
				++byRetNums;
			}
		}
	}
	return (byRetNums==iNums)?true:false;
}



//初始化逻辑数据
void CDDZLogic::InitLogicData(BYTE byMeChairID)
{
	m_aiRun.cc = ctrue;

	//各个玩家牌张数
	m_aiRun.iSizeMid = 20;
	m_aiRun.iSizeUp = 17;
	m_aiRun.iSizeDown = 17;
	m_byLandChairID = -1;

	m_byMeChairID = byMeChairID;
}

//设置手牌数据
void CDDZLogic::SetHandCard(const BYTE* pCard, BYTE byNums, bool bIsLand)
{
	if (NULL == pCard || 0 == byNums)
	{
		return;
	}

	IVector ivIn;
	ivIn.clear();
	int iCardData[MAXCARDNUMS] = {0};
	int iCardNums = 0;

	bool bIsSuc = SwitchBYTEtoINT(pCard, byNums, iCardData, iCardNums);
	if (bIsSuc)
	{
		for (BYTE i=0; i<byNums; i++)
		{
			ivIn.push_back(iCardData[i]);
		}
	}
	else
	{
		return;
	}
	
	if (ivIn.size()>0)
	{
		m_rAndroid.SetCards( ivIn );
	}

	//各个玩家牌张数
	m_aiRun.iSizeMid = bIsLand?20:17;
	m_aiRun.iSizeUp = 17;
	m_aiRun.iSizeDown = 17;
	
	m_aiRun.rcIn.clear();
	m_aiRun.rcAll.clear();
	m_rcAll.clear();

	//首出牌
	m_aiRun.cc = ctrue;

	//地主
	//出牌
	m_rAndroid.m_Lard = bIsLand?yes:no;
	m_rAndroid.m_Push = bIsLand?yes:no;
	if (bIsLand)
	{
		m_rAndroid.m_pos = p_mid;
	}
}


//叫不叫地主
bool CDDZLogic::GetLandTag()
{
	if (NULL!=m_aiAndroid)
	{
		return m_aiAndroid->GetLard();
	}
	return false;
}


//设置地主
void CDDZLogic::SetLandTag(BYTE bBankerUserID)
{
	//各个玩家牌张数
	m_aiRun.iSizeMid = 17;
	m_aiRun.iSizeUp = 17;
	m_aiRun.iSizeDown = 17;
	m_rAndroid.m_Lard = no;
	m_rAndroid.m_Push = no;
	m_byLandChairID = bBankerUserID;
	if (bBankerUserID == (m_byMeChairID+2)%GAME_USER_NUMS)
	{//上家
		m_aiRun.iSizeUp = 20;

		m_rAndroid.m_pos = p_down;
	}
	else if (m_byMeChairID == bBankerUserID)
	{//本人
		m_aiRun.iSizeMid = 20;

		m_rAndroid.m_Lard = yes;
		m_rAndroid.m_Push = yes;

		m_rAndroid.m_pos = p_mid;
	}
	else if (bBankerUserID == (m_byMeChairID+1)%GAME_USER_NUMS)
	{//下家
		m_aiRun.iSizeDown = 20;

		m_rAndroid.m_pos = p_up;
	}
	
	//首出牌
	m_aiRun.cc = ctrue;
}


//出牌
void CDDZLogic::OutCard(BYTE* pCard, BYTE& byNums)
{
	byNums = 0;
	if (NULL==pCard)
	{
		return;
	}
	
	if ( m_aiAndroid->PushCards( m_aiRun ) )
	{		
		IVector icl;
		icl.clear();
		icl = m_rAndroid.GetChCards();		//获得当前要出的牌
		int iCardData[MAXCARDNUMS]={0};
		int iCardNums = 0; 
		for (int i=0; i<icl.size(); i++)
		{
			if (0 != icl[i])
			{
				iCardData[iCardNums] = icl[i]; 
				++iCardNums;
			}
			else return;
		}
		bool bIsSuc = SwitchINTtoBYTE(iCardData, iCardNums, pCard, byNums);
		if (!bIsSuc)
		{
			byNums = 0;
		}
	}
}

//出牌结果
void CDDZLogic::OutCardResult(BYTE byChairID, bool bIsOutCard, const BYTE* pCard, BYTE byNums)
{
	if (byChairID >= GAME_USER_NUMS)
	{
		return;
	}
	if (bIsOutCard && NULL!=pCard && byNums>0)
	{//有出牌
		int iCardData[MAXCARDNUMS]={0};
		int iCardNums = 0; 
		bool bIsSuc = SwitchBYTEtoINT(pCard, byNums, iCardData, iCardNums);
		if (bIsSuc)
		{
			IVector ivCardData;
			ivCardData.clear();
			for (int i=0; i<iCardNums; i++)
			{
				ivCardData.push_back(iCardData[i]);
			}
			RCVector rcCardVector;
			rcCardVector.clear();
			bIsSuc = m_rAndroid.ChangeCardToIvVertor(ivCardData, rcCardVector);
			if (bIsSuc)
			{
				m_aiRun.rcIn = rcCardVector;
				m_aiRun.cc = m_rAndroid.GetCardRCVector(rcCardVector);

				if (byChairID == (m_byLandChairID+2)%GAME_USER_NUMS)
				{//地主上家
					m_aiRun.aipos = p_up;		//出牌位置

					m_aiRun.iSizeUp -= byNums;	//减去相应牌张数
				}
				else if (m_byLandChairID == byChairID)
				{//地主
					m_aiRun.aipos = p_mid;		//出牌位置

					m_aiRun.iSizeMid -= byNums;	//减去相应牌张数
				}
				else if (byChairID == (m_byLandChairID+1)%GAME_USER_NUMS)
				{//地主下家
					m_aiRun.aipos = p_down;		//出牌位置

					m_aiRun.iSizeDown -= byNums;//减去相应牌张数
				}
				MakeCardsTable( m_aiRun.rcIn );		//统计所有出的牌张数功能
				m_aiRun.rcAll = m_rcAll;			//统计所有玩家出的牌的张数
			}
		}
	}
	else if (byChairID == m_byMeChairID)
	{//不出
		if ( m_aiRun.cc != ctrue )
		{
			m_rAndroid.DeleteChoose();
		}
	}
}

//设置谁出牌
void CDDZLogic::SetWhoOutCard(BYTE byChairID, bool bIsReset)
{
	if (m_byMeChairID == byChairID)
	{
		m_rAndroid.m_Push = yes;
	}
	else
	{
		m_rAndroid.m_Push = no;
	}

	if (bIsReset)
	{
		//首出牌
		m_aiRun.cc = ctrue;
	}
}

//
// 出牌表
//
void CDDZLogic::MakeCardsTable( const RCVector& rcIn )
{
	for ( int i = 0; i < rcIn.size(); ++i )
	{
		if ( m_rcAll.empty() )
		{
			m_rcAll.push_back( rcIn[i] );
		}
		else
		{
			int j = 0;
			while ( j < m_rcAll.size() )
			{
				if ( rcIn[i].rc == m_rcAll[j].rc )
				{
					m_rcAll[j].iNum += rcIn[i].iNum;
					break;
				}
				++j;
			}

			// 表中无这张牌
			if ( j >= m_rcAll.size() )
			{//插入到表中.
				int k = 0;
				while ( k < m_rcAll.size() )
				{
					if ( rcIn[i].rc > m_rcAll[k].rc )
					{
						m_rcAll.insert( m_rcAll.begin() + k, rcIn[i] );
						break;
					}
					++k;
				}

				// 最小
				if ( k >= m_rcAll.size() )
				{
					m_rcAll.push_back( rcIn[i] );
				}
			}
		}
	}
}


//删除扑克
bool CDDZLogic::RemoveCard(const BYTE cbRemoveCard[], BYTE cbRemoveCount, BYTE cbCardData[], BYTE cbCardCount)
{
	//检验数据
	ASSERT(cbRemoveCount<=cbCardCount);

	//定义变量
	BYTE cbDeleteCount=0,cbTempCardData[MAXCARDNUMS];
	if (cbCardCount>CountArray(cbTempCardData)) return false;
	CopyMemory(cbTempCardData,cbCardData,cbCardCount*sizeof(cbCardData[0]));

	//置零扑克
	for (BYTE i=0;i<cbRemoveCount;i++)
	{
		for (BYTE j=0;j<cbCardCount;j++)
		{
			if (cbRemoveCard[i]==cbTempCardData[j])
			{
				cbDeleteCount++;
				cbTempCardData[j]=0;
				break;
			}
		}
	}
	if (cbDeleteCount!=cbRemoveCount) return false;

	//清理扑克
	BYTE cbCardPos=0;
	for (BYTE i=0;i<cbCardCount;i++)
	{
		if (cbTempCardData[i]!=0) cbCardData[cbCardPos++]=cbTempCardData[i];
	}

	return true;
}
