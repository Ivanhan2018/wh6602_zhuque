
//
// 2009
// DhLj
//

#include "KddzAI.h"

//
// *
//
KddzAI::KddzAI( Roles* rIn ) : m_RoleAI(NULL), m_iHands(15)
{
	m_RoleAI = rIn;	
}


//
// *
//
KddzAI::~KddzAI( void )
{
}


//
// 转换电脑的所有牌存入m_rcCards
//
void KddzAI::ChangeCard( void )
{
	CARDWORTH cwTemp;
	int iTemp = 0;

	// 出错处理|已经没牌了
	if ( m_RoleAI->GetCards().empty() )
	{
		return;
	}

	// 清空m_cwCards
	if ( !m_cwCards.empty() )
	{
		m_cwCards.erase( m_cwCards.begin(), m_cwCards.end() );
	}

	// 所有牌
	for ( int i = 0; i < m_RoleAI->GetCards().size(); ++i )
	{
		if ( m_RoleAI->GetCards()[i].iCard > 0 &&
			 m_RoleAI->GetCards()[i].iCard <= 52 )			// 排除大小王
		{
			iTemp = ( m_RoleAI->GetCards()[i].iCard - 1 ) / 4 + 3;
		}
		else
		{
			iTemp = m_RoleAI->GetCards()[i].iCard;
		}
		
		switch ( iTemp )
		{
		case 3:
			cwTemp.rcard.rc = R3;
			break;
		case 4:
			cwTemp.rcard.rc = R4;
			break;
		case 5:
			cwTemp.rcard.rc = R5;
			break;
		case 6:
			cwTemp.rcard.rc = R6;
			break;
		case 7:
			cwTemp.rcard.rc = R7;
			break;
		case 8:
			cwTemp.rcard.rc = R8;
			break;
		case 9:
			cwTemp.rcard.rc = R9;
			break;
		case 10:
			cwTemp.rcard.rc = RO;
			break;
		case 11:
			cwTemp.rcard.rc = RJ;
			break;
		case 12:
			cwTemp.rcard.rc = RQ;
			break;
		case 13:
			cwTemp.rcard.rc = RK;
			break;
		case 14:
			cwTemp.rcard.rc = RA;
			break;
		case 15:
			cwTemp.rcard.rc = RT;
			break;
		case 53:
			cwTemp.rcard.rc = RWS;
			break;
		case 54:
			cwTemp.rcard.rc = RWB;
			break;
		default:
			break;
		}
		
		// 这样要保证VECTOR的有序排列(大->小)
		if ( !m_cwCards.empty() && m_cwCards[m_cwCards.size() - 1].rcard.rc == cwTemp.rcard.rc )
		{
			++m_cwCards[m_cwCards.size() - 1].rcard.iNum;
		}
		else
		{
			cwTemp.rcard.iNum = 1;
			cwTemp.csif = sde;
			m_cwCards.push_back( cwTemp );
		}
	}
}


//
// 根据(转换)牌面取m_RoleAI->GetCards()的索引值
//
int KddzAI::GetCard( const CARDCHANGE& rIn )
{
	int ivTemp;

	switch ( rIn )
	{
	case R3:
	case R4:
	case R5:
	case R6:
	case R7:
	case R8:
	case R9:
	case RO:
	case RJ:
	case RQ:
	case RK:
	case RA:
	case RT:
		{
			ivTemp = 4 * rIn;
		}
		break;	
	case RWS:
		{
			ivTemp = 53;			// 牌面
		}
		break;
	case RWB:
		{
			ivTemp = 54;
		}
	default:
		break;
	}

	// 是大小王
	if ( ivTemp == 53 || ivTemp == 54 )
	{
		// 寻找大小王的索引
		for ( int i = 0; i < m_RoleAI->GetCards().size(); ++i )
		{		
			if ( m_RoleAI->GetCards()[i].iCard == ivTemp )
			{
				// 返回索引
				return i;
			}
		}
	}

	//	不是大小王		
	else  
	{
		// 寻找符合牌面的最小索引
		for ( int j = m_RoleAI->GetCards().size() - 1; j >= 0; --j )
		{
			if ( m_RoleAI->GetCards()[j].iCard > ivTemp &&
				m_RoleAI->GetCards()[j].iCard <= ivTemp + 4 )
			{
				return j;
			}
		}
	}

	// 返回出错
	return -1;
}

//
// 计算手数
//
int KddzAI::MathHands( void )
{
	int iHandTemp = 0;
	for ( int m = 0; m < m_cwOrderCards.size(); ++m )
	{					
		if ( m_cwOrderCards[m].csif == sde || m_cwOrderCards[m].csif == end )
		{
			++iHandTemp;
		}	
		if ( m_cwOrderCards[m].rcard.iNum == 3 )
		{
			--iHandTemp;
		}
		if ( m_cwOrderCards[m].rcard.iNum == 4 )
		{
			iHandTemp -= 2;
		}
	}

	return iHandTemp;
}



//
// 获取区间随机数
//
int KddzAI::GetNum( const int iBegin, const int iEnd )
{
	int iTemp = 0;

	while ( 1 )
	{
		if ( ( iTemp = rand() % iEnd ) > iBegin	)		// 随机数字
		{
			break;
		}
	}

	return iTemp;
}


//
// 判断是否叫地主
//
bool KddzAI::GetLard( void )
{
	ChangeCard();
	CutCards();	

	AIRUN aiRun;
	MakeCardsTable( aiRun );
	MathInOrOut();

	// 计算手数
	int iHandTemp = MathHands();
	int iHand = 0;

	if ( m_cwOrderCards.size()>=1 &&
		 m_cwOrderCards[0].rcard.rc == RWB && 
		 m_cwOrderCards.size()>=2 &&
		 m_cwOrderCards[1].rcard.rc == RWS )
	{
		iHandTemp -= 1;
	}

	// 此处可以调节电脑叫地主(难度)
	// 8-10手可以出完
	if ( iHandTemp <= GetNum( 8, 10 ) )
	{
		for ( int i = 0; i < m_cwIncome.size(); ++i )
		{
			if ( m_cwIncome[i].csif == sde ||
				 m_cwIncome[i].csif == beg )
			{
				++iHand;
			}
		}

		// 至少2手大牌才叫
		if ( iHand > 1 )
		{
			return true;
		}
	}

	return false;
}


//
// 判断这组牌是否在当前牌面里
//
bool KddzAI::GetIsIn( const CWVector& cwCut, const CWVector& cwCards )
{
	if ( cwCards.empty() || cwCut.empty() )
	{
		return false;
	}

	for ( int i = 0; i < cwCut.size(); ++i )
	{
		int j = 0;
		for (j = 0; j < cwCards.size(); ++j )
		{
			if ( cwCut[i].rcard.rc == cwCards[j].rcard.rc &&
				 cwCut[i].rcard.iNum <= cwCards[j].rcard.iNum )
			{
				// 跳出内层循环
				break;
			}
		}
		// 循环所有但是没找到匹配的牌
		if ( j >= cwCards.size() )
		{
			return false;
		}
	}

	return true;
}


//
// 删除牌面中有的组合
//
void KddzAI::DeleteCards( const CWVector& cwCut, CWVector& cwCards )
{
	if ( cwCards.empty() || cwCut.empty() )
	{
		return;
	}

	for ( int i = 0; i < cwCut.size(); ++i )
	{
		for ( int j = 0; j < cwCards.size(); ++j )
		{
			if ( cwCut[i].rcard.rc == cwCards[j].rcard.rc )
			{
				cwCards[j].rcard.iNum -=  cwCut[i].rcard.iNum;
				if ( cwCards[j].rcard.iNum <= 0 )
				{
					cwCards.erase( cwCards.begin() + j );
				}
				
				// 跳出内层循环
				break;
			}
		}
	}
}



//
// 递归
//
bool KddzAI::WeaveCards( const CWVector& cwIn, const CWVector& cwOut, const CWVector& cwKate )
{			
	// cwKate:一段内的所有减手牌的集合
	int i = 0;
	CWVector cwCut;
	while ( i < cwKate.size() )
	{
		// cwIn:当前的牌面
		CWVector cwTemp = cwIn;

		// cwOut:组合后的牌
		CWVector cwOutTemp = cwOut;

		// 取出组合牌面的片段		
		while ( i < cwKate.size() )
		{
			cwCut.push_back( cwKate[i] );
			if ( cwKate[i].csif == sde || cwKate[i].csif == end )
			{
				++i;
				break;
			}
			++i;
		}

		// 这组组合牌是否在当前牌面里
		if ( GetIsIn( cwCut, cwTemp ) )
		{
			// 在当前牌面中删除这组牌
			DeleteCards( cwCut, cwTemp );

			// 填加这组牌到cwOutTemp
			for ( int k = 0; k < cwCut.size(); ++k )
			{
				cwOutTemp.push_back( cwCut[k] );
			}

			// 递归
			if ( WeaveCards( cwTemp, cwOutTemp, cwKate ) )
			{
				// 填加剩余牌
				for ( int j = 0; j < cwTemp.size(); ++j )
				{
					cwOutTemp.push_back( cwTemp[j] );
				}

				// 计算手数
				int iHandTemp = 0;
				for ( int m = 0; m < cwOutTemp.size(); ++m )
				{					
					if ( cwOutTemp[m].csif == sde || cwOutTemp[m].csif == end )
					{
						++iHandTemp;
					}	
					if ( cwOutTemp[m].rcard.iNum == 3 )
					{
						--iHandTemp;
					}
					if ( cwOutTemp[m].rcard.iNum == 4 )
					{
						iHandTemp -= 2;
					}
				}
				if ( iHandTemp < m_iHands )
				{
					m_iHands = iHandTemp;	
					m_cwCutCards = cwOutTemp;
				}							
			}
		}

		if ( !cwCut.empty() )
		{
			cwCut.erase( cwCut.begin(), cwCut.end() );
		}
	}

	return true;
}



//
// 拆牌面,调用WeaveCards()选择合理的牌型
//
void KddzAI::CutCards( void )
{
	CWVector cwTemp;
	CWVector cwLinkTemp;
	CWVector cwKate;
	CARDWORTH cwKTemp;
	CWVector cwOut;

	// 无牌返回
	if ( m_cwCards.empty() )
	{
		return;
	}

	// 清除
	if ( !m_cwOrderCards.empty() )
	{
		m_cwOrderCards.erase( m_cwOrderCards.begin(), m_cwOrderCards.end() );
	}

	cwTemp = m_cwCards;

	// 去掉王和2,方便连接
	if ( cwTemp.size() >= 2 &&
		 cwTemp[0].rcard.rc == RWB &&
		 cwTemp[1].rcard.rc == RWS )
	{
		cwTemp[0].csif = beg;
		cwTemp[1].csif = end;
		m_cwOrderCards.push_back( cwTemp[0] );
		m_cwOrderCards.push_back( cwTemp[1] );
		cwTemp.erase( cwTemp.begin(), cwTemp.begin() + 2 );
	}

	for ( int i = 0; i < cwTemp.size(); ++i )
	{	
		if ( cwTemp[i].rcard.rc > RA )
		{
			// 加入出牌表
			m_cwOrderCards.push_back( cwTemp[i] );
			cwTemp.erase( cwTemp.begin() + i-- );
		}
		else
		{
			break;
		}
	}

	// 拆,以断为段
	while ( !cwTemp.empty() )
	{
		// 第一位肯定要加
		cwLinkTemp.push_back( cwTemp[0] );

		// 找断点
		for ( int j = 1; j < cwTemp.size(); ++j )
		{
			// 连续
			if ( cwTemp[0].rcard.rc - cwTemp[j].rcard.rc == j )
			{
				cwLinkTemp.push_back( cwTemp[j] );			
			}
			else
			{
				break;
			}
		}

		// 删除选段
		cwTemp.erase( cwTemp.begin(), cwTemp.begin() + cwLinkTemp.size() );

		//
		// loop1:寻找所有可能单顺,双顺,三顺,三顺带,三带,四,四带
		//

		// 单顺
		for ( int ka = 0; ka + 5 <= cwLinkTemp.size(); ++ka )
		{
			for ( int ma = 5; ma + ka <= cwLinkTemp.size(); ++ma )
			{
				int kaTemp = ka;
				while ( kaTemp < ma + ka )
				{		
					cwKTemp.rcard.rc = cwLinkTemp[kaTemp].rcard.rc;
					cwKTemp.rcard.iNum = 1;								// 单
					if ( kaTemp == ka )	
					{
						cwKTemp.csif = beg;
					}
					else if ( kaTemp == ma + ka - 1 )
					{
						cwKTemp.csif = end;
					}
					else
					{
						cwKTemp.csif = mid;
					}
					cwKate.push_back( cwKTemp );
					++kaTemp;
				}				
			}
		}	

		// 双顺
		for ( int kb = 0; kb + 3 <= cwLinkTemp.size(); ++kb )
		{
			for ( int mb = 3; mb + kb <= cwLinkTemp.size(); ++mb )
			{
				int kbTemp = kb;
				while ( kbTemp < mb + kb )
				{
					// 段内满足条件
					if ( cwLinkTemp[kbTemp].rcard.iNum < 2 )
					{
						break;
					}
					++kbTemp;
				}	
				if ( kbTemp >= mb + kb )
				{
					// 加入段内双顺
					kbTemp = kb;
					while ( kbTemp < mb + kb )
					{
						cwKTemp.rcard.rc = cwLinkTemp[kbTemp].rcard.rc;
						cwKTemp.rcard.iNum = 2;								// 双
						if ( kbTemp == kb )	
						{
							cwKTemp.csif = beg;
						}
						else if ( kbTemp == mb + kb - 1 )
						{
							cwKTemp.csif = end;
						}
						else
						{
							cwKTemp.csif = mid;
						}
						cwKate.push_back( cwKTemp );
						++kbTemp;
					}	
				}
				else
				{
					// 跳出循环,cwLinkTemp向下位移动
					break;
				}
			}
		}

		// 三顺带(问题啊)
		for ( int kc = 0; kc + 2 <= cwLinkTemp.size(); ++kc )
		{
			for ( int mc = 2; mc + kc <= cwLinkTemp.size(); ++mc )
			{
				int kcTemp = kc;
				while ( kcTemp < mc + kc )
				{
					if ( cwLinkTemp[kcTemp].rcard.iNum < 3 )
					{
						break;
					}
					++kcTemp;
				}	
				if ( kcTemp >= mc + kc )
				{
					kcTemp = kc;
					while ( kcTemp < mc + kc )
					{
						cwKTemp.rcard.rc = cwLinkTemp[kcTemp].rcard.rc;
						cwKTemp.rcard.iNum = 3;								// 三
						if ( kcTemp == kc )	
						{
							cwKTemp.csif = beg;
						}
						else if ( kcTemp == mc + kc - 1 )
						{
							cwKTemp.csif = end;
						}
						else
						{
							cwKTemp.csif = mid;
						}
						cwKate.push_back( cwKTemp );
						++kcTemp;
					}	
				}
				else
				{
					// 跳出循环,cwLinkTemp向下位移动
					break;
				}
			}
		}

		// 三带,四带
		for ( int kd = 0; kd < cwLinkTemp.size(); ++kd )
		{
			if ( cwLinkTemp[kd].rcard.iNum >= 3 )
			{			
				cwKTemp.rcard.rc = cwLinkTemp[kd].rcard.rc;
				cwKTemp.rcard.iNum = cwLinkTemp[kd].rcard.iNum;
				cwKTemp.csif = sde;
				cwKate.push_back( cwKTemp );
			}		
		}	

		//
		// end loop1
		//

		// 有合理牌面,m_cwCutCards肯定非空
		if ( !cwKate.empty() )
		{
			WeaveCards( cwLinkTemp, cwOut, cwKate );
			
			for ( int nb = 0; nb < m_cwCutCards.size(); ++nb )
			{
				m_cwOrderCards.push_back( m_cwCutCards[nb] );
			}

			m_cwCutCards.erase( m_cwCutCards.begin(), m_cwCutCards.end() );
		}
		
		//没合理牌面则直接组合当前牌面到m_cwOrderCards
		else		
		{
			for ( int na = 0; na < cwLinkTemp.size(); ++na )
			{
				m_cwOrderCards.push_back( cwLinkTemp[na] );
			}
		}

		// 还原清除
		m_iHands = 15;
		if ( !cwLinkTemp.empty() )				// 异常处理，基本不存在
		{
			cwLinkTemp.erase( cwLinkTemp.begin(), cwLinkTemp.end() );
		}
		if ( !cwKate.empty() )
		{
			cwKate.erase( cwKate.begin(), cwKate.end() );
		}
		if ( !cwOut.empty() )
		{
			cwOut.erase( cwOut.begin(), cwOut.end() );
		}
	}
}


//
// 排序
//
void KddzAI::SetOrder( void )
{
	CWVector cw;
	CWVector cwTemp = m_cwOrderCards;

	// 比对最小的
	while ( !cwTemp.empty() )
	{
		// 找到第一个
		int iTemp = 0;
		while ( iTemp < cwTemp.size() )
		{
			if ( cwTemp[iTemp].csif == sde ||
				 cwTemp[iTemp].csif == end )
			{
				break;
			}
			++iTemp;
		}

		// 比对
		for ( int i = iTemp + 1; i < cwTemp.size(); ++i )
		{
			if ( cwTemp[i].csif == sde ||
				 cwTemp[i].csif == end )
			{
				// 选大
				if ( cwTemp[iTemp].rcard.rc < cwTemp[i].rcard.rc )
				{
					iTemp = i;
				}
			}
		}

		// 找BEG
		while ( iTemp >= 0 )
		{
			if ( cwTemp[iTemp].csif == sde ||
				 cwTemp[iTemp].csif == beg )
			{
				break;
			}
			--iTemp;
		}

		// 加入新组合

		while ( iTemp < cwTemp.size() )
		{
			CARDWORTH cwcw = cwTemp[iTemp];

			cw.push_back( cwTemp[iTemp] );
			cwTemp.erase( cwTemp.begin() + iTemp );

			if ( cwcw.csif == sde ||
				 cwcw.csif == end )
			{
				break;
			}
		}
		
	}

	m_cwOrderCards = cw;
}

//
// 分类收入和支出牌
//
void KddzAI::MathInOrOut( void )
{
	CWVector m_cwTemp = m_cwOrderCards;
	
	// 无牌返回
	if ( m_cwTemp.empty() )
	{
		return;
	}

	if ( !m_cwIncome.empty() )
	{
		m_cwIncome.erase( m_cwIncome.begin(), m_cwIncome.end() );
	}

	if ( !m_cwOutgo.empty() )
	{
		m_cwOutgo.erase( m_cwOutgo.begin(), m_cwOutgo.end() );
	}

	int iCut = 0;
	while ( iCut < m_cwTemp.size() )
	{
		CWVector m_cwCut;

		// 找牌
		int iTempWhile = iCut;
		while ( iTempWhile < m_cwTemp.size() )
		{
			if ( m_cwTemp[iTempWhile].csif == sde || 
				 m_cwTemp[iTempWhile].csif == end )
			{
				break;
			}
			++iTempWhile;
		}

		// 选牌
		while ( iCut <= iTempWhile )
		{
			m_cwCut.push_back( m_cwTemp[iCut++] );
		}

		// 是收入
		if ( GetBig( m_cwCut ) )
		{
			for ( int i = 0; i < m_cwCut.size(); ++i )
			{
				m_cwIncome.push_back( m_cwCut[i] );
			}
		}

		// 是支出
		else
		{
			for ( int j = 0; j < m_cwCut.size(); ++j )
			{
				m_cwOutgo.push_back( m_cwCut[j] );
			}
		}
	}
}


//
// 判断是否为收入牌
//
bool KddzAI::GetBig( const CWVector& cwIn )
{	
	switch ( cwIn[0].rcard.iNum )
	{
	case 1:							
		{
			if ( cwIn.size() == 1 )				// 单
			{
				for ( int i = 0; i < m_rcAll.size(); ++i )
				{
					if ( m_rcAll[i].rc > cwIn[0].rcard.rc &&
						 m_rcAll[i].iNum >= 1 )
					{
						return false;
					}
				}
				return true;
			}
			else if ( cwIn.size() == 2 )		// 对王
			{
				return true;
			}
			else								// 单顺
			{
				for ( int i = 0; i < m_rcAll.size(); ++i )
				{
					if ( m_rcAll[i].rc <= RA &&
						 m_rcAll[i].rc > cwIn[0].rcard.rc &&
						 m_rcAll[i].iNum >= 1 )
					{
						int iAdd = 1;
						for ( int j = i + 1; j < m_rcAll.size(); ++j )
						{
							if ( m_rcAll[j].iNum >= 1 &&
								 m_rcAll[j - 1].rc - m_rcAll[j].rc == 1 )
							{
								++iAdd;
							}
							else
							{
								break;
							}
						}
						if ( iAdd >= cwIn.size() )
						{
							return false;
						}
					}
				}
				return true;
			}
		}
		break;
	case 2:							
		{
			if ( cwIn.size() == 1 )				// 对
			{
				for ( int i = 0; i < m_rcAll.size(); ++i )
				{
					if ( m_rcAll[i].rc > cwIn[0].rcard.rc &&
						 m_rcAll[i].iNum >= 2 )
					{
						return false;
					}
				}
				return true;
			}
			else								// 双顺
			{
				for ( int i = 0; i < m_rcAll.size(); ++i )
				{
					if ( m_rcAll[i].rc <= RA &&
						 m_rcAll[i].rc > cwIn[0].rcard.rc &&
						 m_rcAll[i].iNum >= 2 )
					{
						int iAdd = 1;
						for ( int j = i + 1; j < m_rcAll.size(); ++j )
						{
							if ( m_rcAll[j].iNum >= 2 &&
								 m_rcAll[j - 1].rc - m_rcAll[j].rc == 1 )
							{
								++iAdd;
							}
							else
							{
								break;
							}
						}
						if ( iAdd >= cwIn.size() )
						{
							return false;
						}
					}
				}
				return true;
			}
		}
		break;
	case 3:							
		{
			if ( cwIn.size() == 1 )				// 三
			{
				for ( int i = 0; i < m_rcAll.size(); ++i )
				{
					if ( m_rcAll[i].rc > cwIn[0].rcard.rc &&
						 m_rcAll[i].iNum >= 3 )
					{
						return false;
					}
				}
				return true;
			}
			else								// 三顺
			{
				for ( int i = 0; i < m_rcAll.size(); ++i )
				{
					if ( m_rcAll[i].rc <= RA &&
						 m_rcAll[i].rc > cwIn[0].rcard.rc &&
						 m_rcAll[i].iNum >= 3 )
					{
						int iAdd = 1;
						for ( int j = i + 1; j < m_rcAll.size(); ++j )
						{
							if ( m_rcAll[j].iNum >= 3 &&
								 m_rcAll[j - 1].rc - m_rcAll[j].rc == 1 )
							{
								++iAdd;
							}
							else
							{
								break;
							}
						}
						if ( iAdd >= cwIn.size() )
						{
							return false;
						}
					}
				}
				return true;
			}
		}
		break;
	case 4:	
		{
			// 炸弹都归类到收入牌
			return true;
		}
	default:
		break;
	}

	return false;
}



//
// 判断同伴出的牌是不是最大
//
bool KddzAI::GetBigOther( const AIRUN& aiRun )
{
	CWVector cwTemp;
	CARDWORTH cw;

	cw.csif = sde;
	cw.rcard.iNum = aiRun.rcIn[0].iNum;
	cw.rcard.rc = aiRun.rcIn[0].rc;
	cwTemp.push_back( cw );

	if ( GetBig( cwTemp ) )
	{
		return true;
	}

	return false;
}


//
// 混合出牌表,取反表
//
void KddzAI::MakeCardsTable( const AIRUN& aiRun )
{
	RCARD rTemp;
	RCVector rcTemp;
	RCVector rcAll;

	if ( !aiRun.rcAll.empty() )
	{
		rcAll = aiRun.rcAll;
	}

	// 制造54张牌表
	for ( int iEnum = 14; iEnum >= 0; --iEnum )
	{
		if ( iEnum == 14 || iEnum == 13 )
		{
			rTemp.iNum = 1;
		}
		else
		{
			rTemp.iNum = 4;
		}
		rTemp.rc = (CARDCHANGE)iEnum;
		rcTemp.push_back( rTemp );
	}

	// 混合当前出牌和自己手中的牌
	for ( int i = 0; i < m_cwCards.size(); ++i )
	{
		if ( rcAll.empty() )
		{
			rcAll.push_back( m_cwCards[i].rcard );
		}
		else
		{
			int j = 0;
			while ( j < rcAll.size() )
			{
				if ( rcAll[j].rc == m_cwCards[i].rcard.rc )
				{
					rcAll[j].iNum += m_cwCards[i].rcard.iNum;
					break;
				}
				++j;
			}

			// 表中无这张牌
			if ( j >= rcAll.size() )
			{
				int k = 0;
				while ( k < rcAll.size() )
				{
					if ( m_cwCards[i].rcard.rc > rcAll[k].rc )
					{
						rcAll.insert( rcAll.begin() + k, m_cwCards[i].rcard );
						break;
					}
					++k;
				}

				// 最小
				if ( k >= rcAll.size() )
				{
					rcAll.push_back( m_cwCards[i].rcard );
				}
			}
		}
	}

	// 取反表
	for ( int m = 0; m < rcTemp.size(); ++m )
	{
		for ( int n = 0; n < rcAll.size(); ++n )
		{
			if ( rcTemp[m].rc == rcAll[n].rc )
			{
				rcTemp[m].iNum -= rcAll[n].iNum;
				if ( rcTemp[m].iNum <= 0 )
				{
					rcTemp.erase( rcTemp.begin() + m-- );
				}
				break;
			}
		}
	}

	m_rcAll = rcTemp;
}



//
// 选牌,大到小
//
void KddzAI::ChooseCards( const CARDCHANGE rcIn, const int iNum )
{
	// 无牌返回
	if ( m_cwCards.empty() )
	{
		return;
	}

	for ( int i = 0; i < m_cwCards.size(); ++i )
	{
		if ( m_cwCards[i].rcard.rc == rcIn )
		{		
			// 从大到小排列
			for ( int j = 0; j < iNum; ++j )
			{
				m_RoleAI->ChooseCards( GetCard( m_cwCards[i].rcard.rc ) - j );
			}
			break;
		}
	}
}



//
// 出牌
//
bool KddzAI::PushCards( const AIRUN& aiRun )
{
	bool bTemp = false;
	
	ChangeCard();
	CutCards();	

	if ( m_cwOrderCards.empty() )
	{
		return false;
	}

	MakeCardsTable( aiRun );
	MathInOrOut();
	SetOrder();

	switch ( m_RoleAI->m_pos )
	{
	case p_mid:
		{
			bTemp = PushCardsMid( aiRun );
		}
		break;
	case p_up:
		{
			bTemp = PushCardsUp( aiRun );
		}
		break;
	case p_down:
		{
			bTemp = PushCardsDown( aiRun );
		}
		break;
	default:
		break;
	}

	return bTemp;
}



//
// 找牌
//
int KddzAI::FindOrderIncome( const CARDIF ci, const unsigned int iIn )
{
	CARDIF ciTemp = ( ci == sde ) ? sde : beg;

	for ( int i = m_cwIncome.size() - 1; i >= 0; --i )
	{
		// 找牌(ci:sde|end)
		if ( m_cwIncome[i].csif == ci &&
			 ( ( iIn == 0 && m_cwIncome[i].rcard.iNum <= 3 ) || 
			 ( iIn != 0 && ( m_cwIncome[i].rcard.iNum == iIn || iIn == 5 ) ) ) )
		{
			// 两王，不让当顺出
			if ( ci == end && 
				 iIn == 0 &&
				 m_cwIncome[i].rcard.rc == RWS )
			{
				continue;
			}

			int iTempWhile = i;
			while ( iTempWhile >= 0 )
			{
				if ( m_cwIncome[iTempWhile].csif == ciTemp )
				{
					break;
				}
				--iTempWhile;
			}

			int iRe = i - iTempWhile + 1;
			// 选牌
			while ( i >= iTempWhile )
			{
				ChooseCards( m_cwIncome[i].rcard.rc, 
							 m_cwIncome[i].rcard.iNum );
				--i;
			}

			if ( m_cwIncome[iTempWhile].rcard.iNum == 3 )
			{
				// 返回3顺的长度
				return iRe;
			}

			// 其他返回1表示成功选牌(注:单3张也是返回1的)
			return 1;
		}
	}

	return 0;
}


//
// 找牌
//
int KddzAI::FindOrderOutgo( const CARDIF ci, const unsigned int iIn )
{
	CARDIF ciTemp = ( ci == sde ) ? sde : beg;

	for ( int i = m_cwOutgo.size() - 1; i >= 0; --i )
	{
		// 找牌(ci:sde|end)
		if ( m_cwOutgo[i].csif == ci &&
			 ( ( iIn == 0 && m_cwOutgo[i].rcard.iNum <= 3 ) || 
			 ( iIn != 0 && m_cwOutgo[i].rcard.iNum == iIn ) ) )
		{
			int iTempWhile = i;
			while ( iTempWhile >= 0 )
			{
				if ( m_cwOutgo[iTempWhile].csif == ciTemp )
				{
					break;
				}
				--iTempWhile;
			}

			int iRe = i - iTempWhile + 1;
			// 选牌
			while ( i >= iTempWhile )
			{
				ChooseCards( m_cwOutgo[i].rcard.rc, 
							 m_cwOutgo[i].rcard.iNum );
				--i;
			}

			if ( m_cwOutgo[iTempWhile].rcard.iNum == 3 )
			{
				// 返回3顺的长度
				return iRe;
			}

			// 其他返回1表示成功选牌(注:单3张也是返回1的)
			return 1;
		}
	}

	return 0;
}




//
// 找大单
//
bool KddzAI::FindBigOne( const CWVector& cwIn )
{
	for ( int i = 0; i < cwIn.size(); ++i )
	{
		if ( cwIn[i].csif == sde &&
			 cwIn[i].rcard.iNum == 1 )
		{
			ChooseCards( cwIn[i].rcard.rc, 
						 cwIn[i].rcard.iNum );
			return true;
		}
	}

	return false;
}



//
// 找最小单对(拆牌出)
//
bool KddzAI::FindSmall( const unsigned int iIn )
{
	for ( int i = m_cwCards.size() - 1; i >= 0 ; --i )
	{
		if ( m_cwCards[i].rcard.iNum >= iIn )
		{
			ChooseCards( m_cwCards[i].rcard.rc, iIn );
			return true;
		}
	}

	return false;
}



//
// 给3带配对
//
bool KddzAI::FindTowOne( const CWVector& cwIn, const unsigned int iIn )
{
	unsigned int iAddOne = 0;
	unsigned int iAddTwo = 0;
	int iRe = 0;

	for ( int i = cwIn.size() - 1; i >= 0; --i )
	{
		if ( cwIn[i].csif == sde )
		{
			// 先到先得(AA4433,KQJ这样应该带前者，可是它会带后者BUG待处理)
			if ( cwIn[i].rcard.iNum == 1 )
			{
				++iAddOne;
				if ( iAddOne >= iIn )
				{
					// 单牌满足数量
					iRe = 1;
					break;
				}
			}
			else if ( cwIn[i].rcard.iNum == 2 )
			{
				++iAddTwo;
				if ( iAddTwo >= iIn )
				{
					// 对牌满足数量
					iRe = 2;
					break;
				}
			}
		}
	}

	unsigned int iPlas = iIn;
	if ( iRe != 0 )
	{
		for ( int j = cwIn.size() - 1; j >= 0 && iPlas > 0; --j )
		{
			if ( cwIn[j].csif == sde &&
				 cwIn[j].rcard.iNum == iRe )
			{
				ChooseCards( cwIn[j].rcard.rc, 
							 cwIn[j].rcard.iNum );
				--iPlas;
			}
		}

		return true;
	}

	return false;
}


//
// 给3顺配对
//
bool KddzAI::FindTowOne( const CWVector& cwIn, const unsigned int iNum, const unsigned int iIn )
{
	unsigned int iAdd = 0;
	bool bRe = false;

	for ( int i = cwIn.size() - 1; i >= 0; --i )
	{
		if ( cwIn[i].csif == sde )
		{
			// 先到先得(AA4433,KQJ这样应该带前者，可是它会带后者BUG待处理)
			if ( cwIn[i].rcard.iNum == iNum )
			{
				++iAdd;
				if ( iAdd >= iIn )
				{
					bRe = true;
					break;
				}
			}
		}
	}

	unsigned int iPlas = iIn;
	if ( bRe )
	{
		for ( int j = cwIn.size() - 1; j >= 0 && iPlas > 0; --j )
		{
			if ( cwIn[j].csif == sde &&
				 cwIn[j].rcard.iNum == iNum )
			{
				ChooseCards( cwIn[j].rcard.rc, 
							 cwIn[j].rcard.iNum );
				--iPlas;
			}
		}

		return true;
	}

	return false;
}




//
// 找单(报单对)
//
bool KddzAI::FindOne( const AIRUN& aiRun, const unsigned int iIn )
{
	int iFour = ( iIn == 4 ) ? 4 : 3;
	CARDCHANGE rcTemp = R3;				// 最小

	// 找出最大
	for ( int rc = aiRun.rcIn.size() - 1; rc >= 0; --rc )
	{	
		if ( aiRun.rcIn[rc].iNum == iIn &&
			 aiRun.rcIn[rc].rc > rcTemp )
		{
			rcTemp = aiRun.rcIn[rc].rc;
		}
	}

	if ( !m_cwOrderCards.empty() )
	{
		// 找单张
		for ( int i = m_cwOrderCards.size() - 1; i >= 0; --i )
		{
			if ( m_cwOrderCards[i].csif == sde &&
				 m_cwOrderCards[i].rcard.iNum == iIn &&
				 m_cwOrderCards[i].rcard.rc > rcTemp )
			{
				CWVector cwTemp;
				CARDWORTH cw;

				cw.csif = sde;
				cw.rcard.iNum = iIn;
				cw.rcard.rc = m_cwOrderCards[i].rcard.rc;
				cwTemp.push_back( cw );

				if ( GetBig( cwTemp ) )
				{
					ChooseCards( m_cwOrderCards[i].rcard.rc, iIn );
					return true;
				}
			}
		}

		// 拆SDE	
		for ( int j = m_cwOrderCards.size() - 1; j >= 0; --j )
		{
			// 拆2222
			if ( m_cwOrderCards[j].csif == sde &&
				 m_cwOrderCards[j].rcard.iNum == 4 &&
				 m_cwOrderCards[j].rcard.rc == RT &&
				 m_cwOrderCards[j].rcard.rc > rcTemp )
			{
				ChooseCards( m_cwOrderCards[j].rcard.rc, iIn );
				return true;
			}
			// 拆王
			else if ( m_cwOrderCards[j].csif == end &&
				 m_cwOrderCards[j].rcard.rc == RWS &&
				 iIn == 1 )
			{
				ChooseCards( m_cwOrderCards[j].rcard.rc, iIn );
				return true;
			}
			else if ( m_cwOrderCards[j].csif == sde &&
					  m_cwOrderCards[j].rcard.iNum >= iIn &&
					  m_cwOrderCards[j].rcard.iNum <= iFour &&		// 炸弹不拆
					  m_cwOrderCards[j].rcard.rc > rcTemp )
			{
				CWVector cwTemp;
				CARDWORTH cw;

				cw.csif = sde;
				cw.rcard.iNum = iIn;
				cw.rcard.rc = m_cwOrderCards[j].rcard.rc;
				cwTemp.push_back( cw );

				if ( m_cwOrderCards[j].rcard.rc == RT || GetBig( cwTemp ) )
				{
					ChooseCards( m_cwOrderCards[j].rcard.rc, iIn );
					return true;
				}
			}				
		}

		// 拆BEG-END	
		for ( int k = m_cwOrderCards.size() - 1; k >= 0; --k )
		{
			// 顺中最大的
			if ( m_cwOrderCards[k].csif == beg &&
				 m_cwOrderCards[k].rcard.iNum >= iIn &&
				 m_cwOrderCards[k].rcard.iNum <= iFour &&
				 m_cwOrderCards[k].rcard.rc > rcTemp )
			{
				CWVector cwTemp;
				CARDWORTH cw;

				cw.csif = sde;
				cw.rcard.iNum = iIn;
				cw.rcard.rc = m_cwOrderCards[k].rcard.rc;
				cwTemp.push_back( cw );

				if ( GetBig( cwTemp ) )
				{
					ChooseCards( m_cwOrderCards[k].rcard.rc, iIn );
					return true;
				}
			}				
		}


		// 最大单或对
		for ( int m = 0; m < m_cwCards.size(); ++m )
		{
			if ( m_cwCards[m].rcard.iNum >= iIn &&
				 m_cwCards[m].rcard.iNum <= iFour &&
				 m_cwCards[m].rcard.rc > rcTemp )
			{
				ChooseCards( m_cwCards[m].rcard.rc, iIn );
				return true;
			}
		}

	}

	return false;
}



//
// 打支出单
//
bool KddzAI::FindOneOutgo( const AIRUN& aiRun, const unsigned int iIn, const bool bOneSide )
{
	CARDCHANGE rcTemp = R3;				// 最小

	// 找出最大
	for ( int rc = aiRun.rcIn.size() - 1; rc >= 0; --rc )
	{	
		if ( aiRun.rcIn[rc].iNum == iIn &&
			 aiRun.rcIn[rc].rc > rcTemp )
		{
			rcTemp = aiRun.rcIn[rc].rc;
		}
	}

	if ( !m_cwOutgo.empty() )
	{
		// 找单张
		for ( int i = m_cwOutgo.size() - 1; i >= 0; --i )
		{
			if ( m_cwOutgo[i].csif == sde &&
				 m_cwOutgo[i].rcard.iNum == iIn &&
				 m_cwOutgo[i].rcard.rc > rcTemp )
			{
				//
				// patch:2和小王用来抗牌的BUG
				//
				// 是同伴出的牌,2,小王不出
				if ( bOneSide &&
					 m_cwOutgo[i].rcard.rc >= RT )
				{
					continue;
				}

				ChooseCards( m_cwOutgo[i].rcard.rc, iIn );
				return true;
			}
		}
	}

	return false;
}


//
// 打收入单
//
bool KddzAI::FindOneIncome( const AIRUN& aiRun, const unsigned int iIn )
{
	int iFour = ( iIn == 4 ) ? 4 : 3;
	CARDCHANGE rcTemp = R3;				// 最小

	// 找出最大
	for ( int rc = aiRun.rcIn.size() - 1; rc >= 0; --rc )
	{	
		if ( aiRun.rcIn[rc].iNum == iIn &&
			 aiRun.rcIn[rc].rc > rcTemp )
		{
			rcTemp = aiRun.rcIn[rc].rc;
		}
	}

	if ( !m_cwIncome.empty() )
	{
		// 找单张
		for ( int i = m_cwIncome.size() - 1; i >= 0; --i )
		{
			if ( m_cwIncome[i].csif == sde &&
				 m_cwIncome[i].rcard.iNum == iIn &&
				 m_cwIncome[i].rcard.rc > rcTemp )
			{
				CWVector cwTemp;
				CARDWORTH cw;

				cw.csif = sde;
				cw.rcard.iNum = iIn;
				cw.rcard.rc = m_cwIncome[i].rcard.rc;
				cwTemp.push_back( cw );

				if ( GetBig( cwTemp ) )
				{
					ChooseCards( m_cwIncome[i].rcard.rc, iIn );
					return true;
				}
			}
		}

		// 拆SDE	
		for ( int j = m_cwIncome.size() - 1; j >= 0; --j )
		{
			// 拆2222
			if ( m_cwIncome[j].csif == sde &&
				 m_cwIncome[j].rcard.iNum == 4 &&
				 m_cwIncome[j].rcard.rc == RT &&
				 m_cwIncome[j].rcard.rc > rcTemp )
			{
				ChooseCards( m_cwIncome[j].rcard.rc, iIn );
				return true;
			}
			// 拆王
			else if ( m_cwIncome[j].csif == end &&
				 m_cwIncome[j].rcard.rc == RWS &&
				 iIn == 1 )
			{
				ChooseCards( m_cwIncome[j].rcard.rc, iIn );
				return true;
			}
			else if ( m_cwIncome[j].csif == sde &&
					  m_cwIncome[j].rcard.iNum >= iIn &&
					  m_cwIncome[j].rcard.iNum <= iFour &&
					  m_cwIncome[j].rcard.rc > rcTemp )
			{
				CWVector cwTemp;
				CARDWORTH cw;

				cw.csif = sde;
				cw.rcard.iNum = iIn;
				cw.rcard.rc = m_cwIncome[j].rcard.rc;
				cwTemp.push_back( cw );

				// 拆2
				if ( m_cwIncome[j].rcard.rc == RT || GetBig( cwTemp ) )
				{
					ChooseCards( m_cwIncome[j].rcard.rc, iIn );
					return true;
				}
			}				
		}

		// 没警报不拆顺
	}

	return false;
}



//
// 找顺(报单对)
//
bool KddzAI::FindLink( const AIRUN& aiRun, const unsigned int iNum, const unsigned int iWeidth )
{
	CARDCHANGE rcTemp = RWB;				// 最大

	// 找出最小
	for ( int rc = aiRun.rcIn.size() - 1; rc >= 0; --rc )
	{	
		if ( aiRun.rcIn[rc].iNum == iNum &&
			 aiRun.rcIn[rc].rc < rcTemp )
		{
			rcTemp = aiRun.rcIn[rc].rc;
		}
	}

	if ( !m_cwOrderCards.empty() )
	{
		// 找配对的
		for ( int i = m_cwOrderCards.size() - 1; i >= 0; --i )
		{
			if ( m_cwOrderCards[i].csif == end &&
				 m_cwOrderCards[i].rcard.iNum == iNum &&
				 m_cwOrderCards[i].rcard.rc > rcTemp )
			{
				int iTempWhile = i;
				while ( iTempWhile >= 0 )
				{
					if ( m_cwOrderCards[iTempWhile].csif == beg )
					{
						break;
					}
					--iTempWhile;
				}

				if ( iTempWhile - i + 1 == iWeidth )
				{
					while ( i >= iTempWhile )
					{
						ChooseCards( m_cwOrderCards[i].rcard.rc, 
									 m_cwOrderCards[i].rcard.iNum );
						--i;
					}

					return true;
				}
			}
		}

		// 拆顺
		for ( int j = m_cwOrderCards.size() - 1; j >= 0; --j )
		{
			if ( m_cwOrderCards[j].csif == end &&
				 m_cwOrderCards[j].rcard.iNum >= iNum &&
				 m_cwOrderCards[j].rcard.rc > rcTemp )
			{
				int iTempWhile = j;
				while ( iTempWhile >= 0 )
				{
					if ( m_cwOrderCards[iTempWhile].csif == beg )
					{
						break;
					}
					--iTempWhile;
				}

				if ( iTempWhile - j + 1 >= iWeidth )
				{
					while ( j >= iTempWhile )
					{
						ChooseCards( m_cwOrderCards[j].rcard.rc, 
									 m_cwOrderCards[j].rcard.iNum );
						--j;
					}

					return true;
				}
			}
		}
	}

	return false;
}



//
//
//
bool KddzAI::FindLinkOutgo( const AIRUN& aiRun, const unsigned int iNum, const unsigned int iWeidth )
{
	CARDCHANGE rcTemp = RWB;				// 最大

	// 找出最小
	for ( int rc = aiRun.rcIn.size() - 1; rc >= 0; --rc )
	{	
		if ( aiRun.rcIn[rc].iNum == iNum &&
			 aiRun.rcIn[rc].rc < rcTemp )
		{
			rcTemp = aiRun.rcIn[rc].rc;
		}
	}

	if ( !m_cwOutgo.empty() )
	{
		// 找配对的
		for ( int i = m_cwOutgo.size() - 1; i >= 0; --i )
		{
			if ( m_cwOutgo[i].csif == end &&
				 m_cwOutgo[i].rcard.iNum == iNum &&
				 m_cwOutgo[i].rcard.rc > rcTemp )
			{
				int iTempWhile = i;
				while ( iTempWhile >= 0 )
				{
					if ( m_cwOrderCards[iTempWhile].csif == beg )
					{
						break;
					}
					--iTempWhile;
				}

				if ( iTempWhile - i + 1 == iWeidth )
				{
					while ( i >= iTempWhile )
					{
						ChooseCards( m_cwOutgo[i].rcard.rc, 
									 m_cwOutgo[i].rcard.iNum );
						--i;
					}

					return true;
				}
			}
		}

		// 不拆
	}

	return false;
}


//
//
//
bool KddzAI::FindLinkIncome( const AIRUN& aiRun, const unsigned int iNum, const unsigned int iWeidth )
{
	CARDCHANGE rcTemp = RWB;				// 最大

	// 找出最小
	for ( int rc = aiRun.rcIn.size() - 1; rc >= 0; --rc )
	{	
		if ( aiRun.rcIn[rc].iNum == iNum &&
			 aiRun.rcIn[rc].rc < rcTemp )
		{
			rcTemp = aiRun.rcIn[rc].rc;
		}
	}

	if ( !m_cwIncome.empty() )
	{
		// 找配对的
		for ( int i = m_cwIncome.size() - 1; i >= 0; --i )
		{
			if ( m_cwIncome[i].csif == end &&
				 m_cwIncome[i].rcard.iNum == iNum &&
				 m_cwIncome[i].rcard.rc > rcTemp )
			{
				int iTempWhile = i;
				while ( iTempWhile >= 0 )
				{
					if ( m_cwIncome[iTempWhile].csif == beg )
					{
						break;
					}
					--iTempWhile;
				}

				if ( iTempWhile - i + 1 == iWeidth )
				{
					while ( i >= iTempWhile )
					{
						ChooseCards( m_cwIncome[i].rcard.rc, 
									 m_cwIncome[i].rcard.iNum );
						--i;
					}

					return true;
				}
			}
		}

		// 拆顺
		for ( int j = m_cwIncome.size() - 1; j >= 0; --j )
		{
			if ( m_cwIncome[j].csif == end &&
				 m_cwIncome[j].rcard.iNum >= iNum &&
				 m_cwIncome[j].rcard.rc > rcTemp )
			{
				int iTempWhile = j;
				while ( iTempWhile >= 0 )
				{
					if ( m_cwIncome[iTempWhile].csif == beg )
					{
						break;
					}
					--iTempWhile;
				}

				if ( iTempWhile - j + 1 >= iWeidth )
				{
					while ( j >= iTempWhile )
					{
						ChooseCards( m_cwIncome[j].rcard.rc, 
									 m_cwIncome[j].rcard.iNum );
						--j;
					}

					return true;
				}
			}
		}
	}

	return false;
}



//
// 地主出牌
//
bool KddzAI::PushCardsMid( const AIRUN& aiRun )
{
	bool bTemp = false;
	bool bOne = false;
	bool bTwo = false;
	bool bReOne = false;
	bool bReTwo = false;

	// 农民有单牌警告
	if ( aiRun.iSizeUp == 1 || 
		 aiRun.iSizeDown == 1 )
	{
		bOne = true;
	}

	// 农民有对牌警告
	if ( aiRun.iSizeUp == 2 || 
		 aiRun.iSizeDown == 2 )
	{
		bTwo = true;
	}

	// 模拟选牌
	switch ( aiRun.cc )
	{		
		case ctrue:									// 首先出牌
			{				
				int iRe = 0;
				bool bBreak = false;
				bool bEnough = true;
				int iReHandNum = GetHandNum( true );
				
				// 支出牌型
				// 不够步，或者没有收入牌
				if ( iReHandNum > 1 || m_cwIncome.empty() )
				{
					// 预够步,且没报单对
					if ( iReHandNum == 2 && !bOne && !bTwo )
					{
						iRe = UpEnough();

						switch ( iRe )
						{
						case 0:					// 失败
							break;
						case 1:					// 单张
						case 2:					// 对
							{
								bEnough = false;
							}
							break;
						case 3:					// 3张
							{
								// 带牌
								FindTowOne( m_cwOrderCards, 1 );
								bEnough = false;
							}
							break;
						default:
							break;
						}
					}

					if ( bEnough )
					{
						// 单顺，双顺，三顺
						iRe = FindOrderOutgo( end );

						switch ( iRe )
						{
						case 0:					// 失败
							{
								// 3张
								iRe = FindOrderOutgo( sde, 3 );				

								// 失败
								if ( iRe == 0 )
								{
									if ( iReHandNum > 1 )
									{
										// 单对
										if ( bOne )
										{
											// 找对出
											iRe = FindOrderOutgo( sde, 2 );
											
											// 够步就直接出
											if ( iRe == 0 )
											{
												bReOne = true;
//												FindBigOne( m_cwOutgo );
											}
										}

										else if ( bTwo )
										{
											// 找单出
											iRe = FindOrderOutgo( sde, 1 );
											
											if ( iRe == 0 )
											{
												// 拆对
												bReTwo = true;
//												FindSmall( 1 );
											}
										}

										else
										{
											FindOrderOutgo( sde );
										}
									}

									else if ( m_cwIncome.empty() )
									{
										FindOrderOutgo( sde );
									}
								}

								// 成功
								else
								{
									// 全部中找
									FindTowOne( m_cwOrderCards, 1 );
								}
							}
							break;
						case 1:					// 成功,但不是3顺
							break;
						default:				// 3顺
							{
								// 找对应数量的单或者对
								FindTowOne( m_cwOrderCards, iRe );
							}
							break;
						}
					}
				}

				// 收入牌型
				if ( iReHandNum <= 1 || bReOne || bReTwo )
				{	
					// 单顺，双顺，三顺
					iRe = FindOrderIncome( end );

					switch ( iRe )
					{
					case 0:					// 失败
						{
							// 3张
							iRe = FindOrderIncome( sde, 3 );				

							// 失败
							if ( iRe == 0 )
							{
								if ( iReHandNum <= 1 )
								{
									// 4张
									if ( FindOrderIncome( sde, 4 ) != 0 )
									{										
										// 找2个单
										if ( !FindTowOne( m_cwOrderCards, 1, 2 ) )
										{
											// 找2个对
											FindTowOne( m_cwOrderCards, 2, 2 );
										}

										bBreak = true;
									}
								}

								if ( !bBreak )
								{
									if ( bReOne )
									{
										if ( FindBigOne( m_cwOutgo ) )
										{
											break;
										}
									}
									else if ( bReTwo )
									{
										if ( FindSmall( 1 ) )
										{
											break;
										}
									}

									// 大王+333
									if ( !Patch3And() )
									{
										// 单对
										iRe = FindOrderIncome( sde );
										
										if ( iRe == 0 )
										{											
											// 拆王
											if ( !Patch2w( bOne || bTwo ) )
											{		
												// 出双王
												FindOrderIncome( end, 5 );												
											}
										}										
									}
								}
							}

							// 成功
							else
							{
								// 全部中找
								FindTowOne( m_cwOrderCards, 1 );
							}
						}
						break;
					case 1:					// 成功,但不是3顺
						break;
					default:				// 3顺
						{
							// 找对应数量的单或者对(感觉这个不太可能)[AAAKKKQQQ]
							FindTowOne( m_cwOrderCards, iRe );
						}
						break;
					}
				}

				// 出牌
				bTemp = m_RoleAI->PushCard( aiRun.rcIn, aiRun.cc );
			}
			break;
		case cone:									// 单牌
			{
				ChoseOneMid( aiRun, 1, bOne || bTwo );

				bTemp = m_RoleAI->PushCard( aiRun.rcIn, aiRun.cc );
			}
			break;
		case ctwo:									// 对子
			{
				ChoseOneMid( aiRun, 2, bOne || bTwo );

				bTemp = m_RoleAI->PushCard( aiRun.rcIn, aiRun.cc );
			}
			break;
		case cthree:									// 3张不带
			{
				ChoseOneMid( aiRun, 3, bOne || bTwo );

				bTemp = m_RoleAI->PushCard( aiRun.rcIn, aiRun.cc );
			}
			break;
		case cfour:									// 4张不带
			{
				ChoseOneMid( aiRun, 4, bOne || bTwo );

				bTemp = m_RoleAI->PushCard( aiRun.rcIn, aiRun.cc );
			}
			break;
		case ctth:										// 三顺(2)
			{
				ChoseLinkMid( aiRun, 3, 2, bOne || bTwo );

				bTemp = m_RoleAI->PushCard( aiRun.rcIn, aiRun.cc );
			}
			break;
		case cthone:										// 3带1
			{
				ChoseOneMid( aiRun, 3, bOne || bTwo );
				FindTowOne( m_cwOrderCards, 1, 1 );

				bTemp = m_RoleAI->PushCard( aiRun.rcIn, aiRun.cc );
			}
			break;
		case cthtwo:										// 3带2
			{
				ChoseOneMid( aiRun, 3, bOne || bTwo );
				FindTowOne( m_cwOrderCards, 2, 1 );

				bTemp = m_RoleAI->PushCard( aiRun.rcIn, aiRun.cc );
			}
			break;
//		case cfone:											// 4带1							
//			{
//				ChoseOneMid( aiRun, 4, bOne || bTwo );
//				FindTowOne( m_cwOrderCards, 1, 1 );
//
//				bTemp = m_RoleAI->PushCard( aiRun.rcIn, aiRun.cc );
//			}
//			break;
//		case cftwo:											// 4带2
//			{
//				ChoseOneMid( aiRun, 4, bOne || bTwo );
//				FindTowOne( m_cwOrderCards, 2, 1 );
//
//				bTemp = m_RoleAI->PushCard( aiRun.rcIn, aiRun.cc );
//			}
//			break;
		case cttth:									// 双顺(3)
			{
				ChoseLinkMid( aiRun, 2, 3, bOne || bTwo );

				bTemp = m_RoleAI->PushCard( aiRun.rcIn, aiRun.cc );
			}
			break;
		case cftone:								// 四带两单
			{
				ChoseOneMid( aiRun, 4, bOne || bTwo );
				FindTowOne( m_cwOrderCards, 1, 2 );

				bTemp = m_RoleAI->PushCard( aiRun.rcIn, aiRun.cc );
			}
			break;
		case cfttwo:										// 四带两对
			{
				ChoseOneMid( aiRun, 4, bOne || bTwo );
				FindTowOne( m_cwOrderCards, 2, 2 );

				bTemp = m_RoleAI->PushCard( aiRun.rcIn, aiRun.cc );
			}
			break;
		case ctfour:									// 双顺(4)
			{
				ChoseLinkMid( aiRun, 2, 4, bOne || bTwo );

				bTemp = m_RoleAI->PushCard( aiRun.rcIn, aiRun.cc );
			}
			break;
		case cthfour:													// 三顺(4)
			{
				ChoseLinkMid( aiRun, 3, 4, bOne || bTwo );

				bTemp = m_RoleAI->PushCard( aiRun.rcIn, aiRun.cc );
			}
			break;
		case ctthone:											// 三顺带两单
			{
				ChoseLinkMid( aiRun, 3, 2, bOne || bTwo );
				FindTowOne( m_cwOrderCards, 1, 2 );

				bTemp = m_RoleAI->PushCard( aiRun.rcIn, aiRun.cc );
			}
			break;
		case ctthtwo:										// 三顺带两对
			{
				ChoseLinkMid( aiRun, 3, 2, bOne || bTwo );
				FindTowOne( m_cwOrderCards, 2, 2 );

				bTemp = m_RoleAI->PushCard( aiRun.rcIn, aiRun.cc );
			}
			break;
		case clink5:									// 单顺(5)
			{
				ChoseLinkMid( aiRun, 1, 5, bOne || bTwo );

				bTemp = m_RoleAI->PushCard( aiRun.rcIn, aiRun.cc );
			}
			break;
		case clink6:								// 单顺(6)
			{
				ChoseLinkMid( aiRun, 1, 6, bOne || bTwo );

				bTemp = m_RoleAI->PushCard( aiRun.rcIn, aiRun.cc );
			}
			break;
		case clink7:								// 单顺(7)
			{
				ChoseLinkMid( aiRun, 1, 7, bOne || bTwo );

				bTemp = m_RoleAI->PushCard( aiRun.rcIn, aiRun.cc );
			}
			break;
		case clink8:									// 单顺(8)
			{
				ChoseLinkMid( aiRun, 1, 8, bOne || bTwo );

				bTemp = m_RoleAI->PushCard( aiRun.rcIn, aiRun.cc );
			}
			break;
		case clink9:									// 单顺(9)
			{
				ChoseLinkMid( aiRun, 1, 9, bOne || bTwo );

				bTemp = m_RoleAI->PushCard( aiRun.rcIn, aiRun.cc );
			}
			break;
		case clink10:							// 单顺(10)
			{
				ChoseLinkMid( aiRun, 1, 10, bOne || bTwo );

				bTemp = m_RoleAI->PushCard( aiRun.rcIn, aiRun.cc );
			}
			break;
		case clink11:								// 单顺(11)
			{
				ChoseLinkMid( aiRun, 1, 11, bOne || bTwo );

				bTemp = m_RoleAI->PushCard( aiRun.rcIn, aiRun.cc );
			}
			break;
		case ctfive:									// 双顺(5)
			{
				ChoseLinkMid( aiRun, 2, 5, bOne || bTwo );

				bTemp = m_RoleAI->PushCard( aiRun.rcIn, aiRun.cc );
			}
			break;
		case cthfive:								// 三顺(5)
			{
				ChoseLinkMid( aiRun, 3, 5, bOne || bTwo );

				bTemp = m_RoleAI->PushCard( aiRun.rcIn, aiRun.cc );
			}
			break;
		case ctsix:								// 双顺(6)
			{
				ChoseLinkMid( aiRun, 2, 6, bOne || bTwo );

				bTemp = m_RoleAI->PushCard( aiRun.rcIn, aiRun.cc );
			}
			break;
		case cththone:							// 三顺带三单
			{
				ChoseLinkMid( aiRun, 3, 3, bOne || bTwo );
				FindTowOne( m_cwOrderCards, 1, 3 );

				bTemp = m_RoleAI->PushCard( aiRun.rcIn, aiRun.cc );
			}
			break;
		case cththtwo:							// 三顺带三对
			{
				ChoseLinkMid( aiRun, 3, 3, bOne || bTwo );
				FindTowOne( m_cwOrderCards, 2, 3 );

				bTemp = m_RoleAI->PushCard( aiRun.rcIn, aiRun.cc );
			}
			break;
		case ctseven:							// 双顺(7)
			{
				ChoseLinkMid( aiRun, 2, 7, bOne || bTwo );

				bTemp = m_RoleAI->PushCard( aiRun.rcIn, aiRun.cc );
			}
			break;
		case ctevent:				     	// 双顺(8)
			{
				ChoseLinkMid( aiRun, 2, 8, bOne || bTwo );

				bTemp = m_RoleAI->PushCard( aiRun.rcIn, aiRun.cc );
			}
			break;
		case cthfone:					// 三顺带四单
			{
				ChoseLinkMid( aiRun, 3, 4, bOne || bTwo );
				FindTowOne( m_cwOrderCards, 1, 4 );

				bTemp = m_RoleAI->PushCard( aiRun.rcIn, aiRun.cc );
			}
			break;
		default:
			break;
	} 

	return bTemp;
}



//
// 地主上家出牌
//
bool KddzAI::PushCardsUp( const AIRUN& aiRun )
{
	bool bTemp = false;
	bool bOneMid = false;
	bool bTwoMid = false;
	bool bReOneMid = false;
	bool bReTwoMid = false;
	bool bOneDown = false;
	bool bTwoDown = false;
	static bool bTwoToOne = false;			// 打对改打单

	// 无警告状态
	if ( aiRun.iSizeDown > 2 )
	{
		bTwoToOne = false;
	}

	// 同伴有单牌警告
	if ( aiRun.iSizeDown == 1 ||
		 bTwoToOne )
	{
		bOneDown = true;
	}

	// 同伴有对牌警告
	if ( aiRun.iSizeDown == 2 &&
		 !bTwoToOne )
	{
		bTwoDown = true;
	}

	// 地主有单牌警告
	if ( aiRun.iSizeMid == 1 )
	{
		bOneMid = true;
	}

	// 地主有对牌警告
	if ( aiRun.iSizeMid == 2 )
	{
		bTwoMid = true;
	}

	// 模拟选牌
	switch ( aiRun.cc )
	{		
		case ctrue:									// 首先出牌
			{
				int iRe = 0;
				bool bRe = false;
				bool bBreak = false;
				bool bEnough = true;
				int iReHandNum = GetHandNum( true );

				// 自己不够步
				if ( iReHandNum > 1  )
				{
					if ( bOneDown && !bOneMid )
					{
						// 出小单
						bRe = FindSmall( 1 );
					}

					else if ( bTwoDown && !bTwoMid )
					{
						// 出小对
						bRe = FindSmall( 2 );
					}

					if ( bRe )
					{
						bTemp = m_RoleAI->PushCard( aiRun.rcIn, aiRun.cc );

						if ( bTemp && bTwoDown )
						{
							bTwoToOne = true;
						}

						break;
					}
				}

				// 支出牌型
				if ( iReHandNum > 1 || m_cwIncome.empty() )
				{			
					// 预够步
					if ( iReHandNum == 2 && !bOneMid && !bTwoMid )
					{
						iRe = UpEnough();

						switch ( iRe )
						{
						case 0:					// 失败
							break;
						case 1:					// 单张
						case 2:					// 对
							{
								bEnough = false;
							}
							break;
						case 3:					// 3张
							{
								// 带牌
								FindTowOne( m_cwOrderCards, 1 );
								bEnough = false;
							}
							break;
						default:
							break;
						}
					}

					if ( bEnough )
					{
						// 单顺，双顺，三顺
						iRe = FindOrderOutgo( end );

						switch ( iRe )
						{
						case 0:					// 失败
							{
								// 3张
								iRe = FindOrderOutgo( sde, 3 );				

								// 失败
								if ( iRe == 0 )
								{
									if ( iReHandNum > 1 )
									{
										// 单对
										if ( bOneMid )
										{
											// 找对出
											iRe = FindOrderOutgo( sde, 2 );
											
											if ( iRe == 0 )
											{
												bReOneMid = true;
//												FindBigOne( m_cwOutgo );
											}
										}

										else if ( bTwoMid )
										{
											// 找单出
											iRe = FindOrderOutgo( sde, 1 );
											
											if ( iRe == 0 )
											{
												bReTwoMid = true;
//												FindSmall( 1 );
											}
										}

										else
										{
											FindOrderOutgo( sde );
										}
									}

									else if ( m_cwIncome.empty() )
									{
										FindOrderOutgo( sde );
									}
								}

								// 成功
								else
								{
									FindTowOne( m_cwOrderCards, 1 );
								}
							}
							break;
						case 1:					// 成功,但不是3顺
							break;
						default:				// 3顺
							{
								// 找对应数量的单或者对
								FindTowOne( m_cwOrderCards, iRe );
							}
							break;
						}
					}
				}

				// 收入牌型
				if ( iReHandNum <= 1 || bReOneMid || bReTwoMid )
				{	
					// 单顺，双顺，三顺
					iRe = FindOrderIncome( end );

					switch ( iRe )
					{
					case 0:					// 失败
						{
							// 3张
							iRe = FindOrderIncome( sde, 3 );				

							// 失败
							if ( iRe == 0 )
							{
								if ( iReHandNum <= 1 )
								{
									// 4张
									if ( FindOrderIncome( sde, 4 ) != 0 )
									{
										// 找2个单
										if ( !FindTowOne( m_cwOrderCards, 1, 2 ) )
										{
											// 找2个对
											FindTowOne( m_cwOrderCards, 2, 2 );
										}

										bBreak = true;
									}
								}

								if ( !bBreak )
								{
									if ( bReOneMid )
									{
										if ( FindBigOne( m_cwOutgo ) )
										{
											break;
										}
									}
									else if ( bReTwoMid )
									{
										if ( FindSmall( 1 ) )
										{
											break;
										}
									}

									if ( !Patch3And() )
									{
										// 单对
										iRe = FindOrderIncome( sde );
										
										if ( iRe == 0 )
										{
											if ( !Patch2w( bOneMid || bTwoMid ) )
											{
												// 2王
												FindOrderIncome( end, 5 );
											}
										}
									}
								}
							}

							// 成功
							else
							{
								FindTowOne( m_cwOrderCards, 1 );
							}
						}
						break;
					case 1:					// 成功,但不是3顺
						break;
					default:				// 3顺
						{
							// 找对应数量的单或者对(感觉这个不太可能)[AAAKKKQQQ]
							FindTowOne( m_cwOrderCards, iRe );
						}
						break;
					}
				}

				// 出牌
				bTemp = m_RoleAI->PushCard( aiRun.rcIn, aiRun.cc );
			}
			break;
		case cone:									// 单牌
			{
				bool bOneInCome = false;

				// 够步
				if ( ChoseOneOk( aiRun, 1 ) )
				{
					bTemp = m_RoleAI->PushCard( aiRun.rcIn, aiRun.cc );	
					break;
				}

				// 地主报单
				if ( bOneMid )
				{
					// 地主报对，同伴放单就不抗了
					// 同伴出的牌，且不是最大
					if ( aiRun.aipos == p_down &&
						 !GetBigOther( aiRun ) )						
					{
						// 管上
						FindOne( aiRun, 1 );
					}
					else if ( aiRun.aipos != p_down )
					{
						// 管上
						FindOne( aiRun, 1 );
					}
				}

				else if ( bTwoMid && aiRun.aipos != p_down )
				{
					// 管上
					FindOne( aiRun, 1 );
				}

				// 同伴报单,且地主出的牌
				else if ( bOneDown && 
						  aiRun.aipos != p_down )
				{
					// 管上
					if ( !FindOne( aiRun, 1 ) )
					{
						PushZha( aiRun );
					}
				}

				// 同伴报对
				else if ( bTwoDown && 
						  aiRun.aipos != p_down )
				{
					// 管上
					if ( !FindOne( aiRun, 1 ) )
					{
						PushZha( aiRun );
					}
				}
						
				else
				{					
					// 顺牌
					if ( !m_cwOutgo.empty() )
					{
						// 顺单
						if ( !FindOneOutgo( aiRun, 1, aiRun.aipos == p_down ) &&
							 aiRun.aipos != p_down )
						{
							bOneInCome = true;			
						}
					}

					if ( m_cwOutgo.empty() || bOneInCome )
					{
						// 管上(不是同伴才打)
						FindOneIncome( aiRun, 1 );
					}
				}
				
				bTemp = m_RoleAI->PushCard( aiRun.rcIn, aiRun.cc );	
			}
			break;
		case ctwo:									// 对子
			{
				bool bOneInCome = false;

				// 够步
				if ( ChoseOneOk( aiRun, 2 ) )
				{
					bTemp = m_RoleAI->PushCard( aiRun.rcIn, aiRun.cc );	
					break;
				}

				// 地主报单或对
				if ( bTwoMid )
				{
					// 地主报单，同伴放对就不抗了
					// 同伴出的牌，且不是最大
					if ( aiRun.aipos == p_down &&
						 !GetBigOther( aiRun ) )						
					{
						// 管上
						FindOne( aiRun, 2 );
					}
					else if ( aiRun.aipos != p_down )
					{
						// 管上
						FindOne( aiRun, 2 );
					}
				}

				else if ( bOneMid && aiRun.aipos != p_down )
				{
					// 管上
					FindOne( aiRun, 2 );
				}

				//
				// 注:
				// 如果地主上家根本管不上的话就出现一个BUG
				// 地主一直放单和对的时候没人管
				//
				// 同伴报单,且地主出的牌
				else if ( bOneDown && 
						  aiRun.aipos != p_down )
				{
					// 管上
					if ( !FindOne( aiRun, 2 ) )
					{
						PushZha( aiRun );
					}
				}

				// 同伴报对
				else if ( bTwoDown && 
						  aiRun.aipos != p_down )
				{
					// 管上
					if ( !FindOne( aiRun, 2 ) )
					{
						PushZha( aiRun );
					}
				}
						
				else
				{
					// 顺牌
					if ( !m_cwOutgo.empty() )
					{
						// 顺单
						if ( !FindOneOutgo( aiRun, 2 ) &&
							 aiRun.aipos != p_down )
						{
							bOneInCome = true;			
						}
					}

					if ( m_cwOutgo.empty() || bOneInCome )
					{
						// 管上(不是同伴才打)
						FindOneIncome( aiRun, 2 );
					}
				}
				
				bTemp = m_RoleAI->PushCard( aiRun.rcIn, aiRun.cc );	
			}
			break;
		case cthree:									// 3张不带
			{
				ChoseOneUp( aiRun, 3, bOneMid || bTwoMid, bOneDown, bTwoDown );
				
				bTemp = m_RoleAI->PushCard( aiRun.rcIn, aiRun.cc );		
			}
			break;
		case cfour:									// 4张不带
			{
				ChoseOneUp( aiRun, 4, bOneMid || bTwoMid, bOneDown, bTwoDown );
				
				bTemp = m_RoleAI->PushCard( aiRun.rcIn, aiRun.cc );	
			}
			break;
		case ctth:										// 三顺(2)
			{
				ChoseLinkUp( aiRun, 3, 2, bOneMid || bTwoMid, bOneDown, bTwoDown );

				bTemp = m_RoleAI->PushCard( aiRun.rcIn, aiRun.cc );	
			}
			break;
		case cthone:										// 3带1
			{
				ChoseOneUp( aiRun, 3, bOneMid || bTwoMid, bOneDown, bTwoDown );
				FindTowOne( m_cwOrderCards, 1, 1 );

				bTemp = m_RoleAI->PushCard( aiRun.rcIn, aiRun.cc );	
			}
			break;
		case cthtwo:										// 3带2
			{
				ChoseOneUp( aiRun, 3, bOneMid || bTwoMid, bOneDown, bTwoDown );
				FindTowOne( m_cwOrderCards, 2, 1 );

				bTemp = m_RoleAI->PushCard( aiRun.rcIn, aiRun.cc );	
			}
			break;
//		case cfone:											// 4带1							
//			{
//				ChoseOneUp( aiRun, 4, bOneMid || bTwoMid, bOneDown, bTwoDown );
//				FindTowOne( m_cwOrderCards, 1, 1 );
//
//				bTemp = m_RoleAI->PushCard( aiRun.rcIn, aiRun.cc );	
//			}
//			break;
//		case cftwo:											// 4带2
//			{
//				ChoseOneUp( aiRun, 3, bOneMid || bTwoMid, bOneDown, bTwoDown );
//				FindTowOne( m_cwOrderCards, 2, 1 );
//
//				bTemp = m_RoleAI->PushCard( aiRun.rcIn, aiRun.cc );	
//			}
//			break;
		case cttth:									// 双顺(3)
			{
				ChoseLinkUp( aiRun, 2, 3, bOneMid || bTwoMid, bOneDown, bTwoDown );

				bTemp = m_RoleAI->PushCard( aiRun.rcIn, aiRun.cc );
			}
			break;
		case cftone:								// 四带两单
			{
				ChoseOneUp( aiRun, 4, bOneMid || bTwoMid, bOneDown, bTwoDown );
				FindTowOne( m_cwOrderCards, 1, 2 );

				bTemp = m_RoleAI->PushCard( aiRun.rcIn, aiRun.cc );	
			}
			break;
		case cfttwo:										// 四带两对
			{
				ChoseOneUp( aiRun, 4, bOneMid || bTwoMid, bOneDown, bTwoDown );
				FindTowOne( m_cwOrderCards, 2, 2 );

				bTemp = m_RoleAI->PushCard( aiRun.rcIn, aiRun.cc );	
			}
			break;
		case ctfour:									// 双顺(4)
			{
				ChoseLinkUp( aiRun, 2, 4, bOneMid || bTwoMid, bOneDown, bTwoDown );

				bTemp = m_RoleAI->PushCard( aiRun.rcIn, aiRun.cc );
			}
			break;
		case cthfour:													// 三顺(4)
			{
				ChoseLinkUp( aiRun, 3, 4, bOneMid || bTwoMid, bOneDown, bTwoDown );

				bTemp = m_RoleAI->PushCard( aiRun.rcIn, aiRun.cc );
			}
			break;
		case ctthone:											// 三顺带两单
			{
				ChoseLinkUp( aiRun, 3, 2, bOneMid || bTwoMid, bOneDown, bTwoDown );
				FindTowOne( m_cwOrderCards, 1, 2 );

				bTemp = m_RoleAI->PushCard( aiRun.rcIn, aiRun.cc );	
			}
			break;
		case ctthtwo:										// 三顺带两对
			{
				ChoseLinkUp( aiRun, 3, 2, bOneMid || bTwoMid, bOneDown, bTwoDown );
				FindTowOne( m_cwOrderCards, 2, 2 );

				bTemp = m_RoleAI->PushCard( aiRun.rcIn, aiRun.cc );	
			}
			break;
		case clink5:									// 单顺(5)
			{
				ChoseLinkUp( aiRun, 1, 5, bOneMid || bTwoMid, bOneDown, bTwoDown );

				bTemp = m_RoleAI->PushCard( aiRun.rcIn, aiRun.cc );	
			}
			break;
		case clink6:								// 单顺(6)
			{
				ChoseLinkUp( aiRun, 1, 6, bOneMid || bTwoMid, bOneDown, bTwoDown );

				bTemp = m_RoleAI->PushCard( aiRun.rcIn, aiRun.cc );	
			}
			break;
		case clink7:								// 单顺(7)
			{
				ChoseLinkUp( aiRun, 1, 7, bOneMid || bTwoMid, bOneDown, bTwoDown );

				bTemp = m_RoleAI->PushCard( aiRun.rcIn, aiRun.cc );	
			}
			break;
		case clink8:									// 单顺(8)
			{
				ChoseLinkUp( aiRun, 1, 8, bOneMid || bTwoMid, bOneDown, bTwoDown );

				bTemp = m_RoleAI->PushCard( aiRun.rcIn, aiRun.cc );	
			}
			break;
		case clink9:									// 单顺(9)
			{
				ChoseLinkUp( aiRun, 1, 9, bOneMid || bTwoMid, bOneDown, bTwoDown );

				bTemp = m_RoleAI->PushCard( aiRun.rcIn, aiRun.cc );	
			}
			break;
		case clink10:							// 单顺(10)
			{
				ChoseLinkUp( aiRun, 1, 10, bOneMid || bTwoMid, bOneDown, bTwoDown );

				bTemp = m_RoleAI->PushCard( aiRun.rcIn, aiRun.cc );	
			}
			break;
		case clink11:								// 单顺(11)
			{
				ChoseLinkUp( aiRun, 1, 11, bOneMid || bTwoMid, bOneDown, bTwoDown );

				bTemp = m_RoleAI->PushCard( aiRun.rcIn, aiRun.cc );	
			}
			break;
		case ctfive:									// 双顺(5)
			{
				ChoseLinkUp( aiRun, 2, 5, bOneMid || bTwoMid, bOneDown, bTwoDown );

				bTemp = m_RoleAI->PushCard( aiRun.rcIn, aiRun.cc );
			}
			break;
		case cthfive:								// 三顺(5)
			{
				ChoseLinkUp( aiRun, 3, 5, bOneMid || bTwoMid, bOneDown, bTwoDown );

				bTemp = m_RoleAI->PushCard( aiRun.rcIn, aiRun.cc );
			}
			break;
		case ctsix:								// 双顺(6)
			{
				ChoseLinkUp( aiRun, 2, 6, bOneMid || bTwoMid, bOneDown, bTwoDown );

				bTemp = m_RoleAI->PushCard( aiRun.rcIn, aiRun.cc );
			}
			break;
		case cththone:							// 三顺带三单
			{
				ChoseLinkUp( aiRun, 3, 3, bOneMid || bTwoMid, bOneDown, bTwoDown );
				FindTowOne( m_cwOrderCards, 1, 3 );

				bTemp = m_RoleAI->PushCard( aiRun.rcIn, aiRun.cc );	
			}
			break;
		case cththtwo:							// 三顺带三对
			{
				ChoseLinkUp( aiRun, 3, 3, bOneMid || bTwoMid, bOneDown, bTwoDown );
				FindTowOne( m_cwOrderCards, 2, 3 );

				bTemp = m_RoleAI->PushCard( aiRun.rcIn, aiRun.cc );	
			}
			break;
		case ctseven:							// 双顺(7)
			{
				ChoseLinkUp( aiRun, 2, 7, bOneMid || bTwoMid, bOneDown, bTwoDown );

				bTemp = m_RoleAI->PushCard( aiRun.rcIn, aiRun.cc );
			}
			break;
		case ctevent:				     	// 双顺(8)
			{
				ChoseLinkUp( aiRun, 2, 8, bOneMid || bTwoMid, bOneDown, bTwoDown );

				bTemp = m_RoleAI->PushCard( aiRun.rcIn, aiRun.cc );
			}
			break;
		case cthfone:					// 三顺带四单
			{
				ChoseLinkUp( aiRun, 3, 4, bOneMid || bTwoMid, bOneDown, bTwoDown );
				FindTowOne( m_cwOrderCards, 1, 4 );

				bTemp = m_RoleAI->PushCard( aiRun.rcIn, aiRun.cc );	
			}
			break;
		default:
			break;
	} 

	return bTemp;
}



//
// 地主下家出牌
//
bool KddzAI::PushCardsDown( const AIRUN& aiRun )
{
	bool bTemp = false;
	bool bOneMid = false;
	bool bTwoMid = false;
	bool bReOneMid = false;
	bool bReTwoMid = false;
	bool bOneUp = false;
	bool bTwoUp = false;
	static bool bTwoToOne = false;				

	// 无警告状态
	if ( aiRun.iSizeUp > 2 )
	{
		bTwoToOne = false;
	}

	// 同伴有单牌警告
	if ( aiRun.iSizeUp == 1 ||
		 bTwoToOne )
	{
		bOneUp = true;
	}

	// 同伴有对牌警告
	if ( aiRun.iSizeUp == 2 &&
		 !bTwoToOne )				
	{
		bTwoUp = true;
	}

	// 地主有单牌警告
	if ( aiRun.iSizeMid == 1 )
	{
		bOneMid = true;
	}

	// 地主有对牌警告
	if ( aiRun.iSizeMid == 2 )
	{
		bTwoMid = true;
	}

	// 模拟选牌
	switch ( aiRun.cc )
	{		
		case ctrue:									// 首先出牌
			{
				int iRe = 0;
				bool bRe = false;
				bool bBreak = false;
				bool bEnough = true;
				int iReHandNum = GetHandNum( true );

				// 单对
				if ( iReHandNum > 1 )
				{
					if ( bOneUp )
					{
						bRe = FindSmall( 1 );
					}

					else if ( bTwoUp )
					{
						bRe = FindSmall( 2 );
					}

					if ( bRe )
					{
						bTemp = m_RoleAI->PushCard( aiRun.rcIn, aiRun.cc );

						if ( bTemp && bTwoUp )
						{
							bTwoToOne = true;
						}

						break;
					}
				}
				
				// 支出牌型
				if ( iReHandNum > 1 || m_cwIncome.empty() )
				{
					// 预够步
					if ( iReHandNum == 2 && !bOneMid && !bTwoMid )
					{
						iRe = UpEnough();

						switch ( iRe )
						{
						case 0:					// 失败
							break;
						case 1:					// 单张
						case 2:					// 对
							{
								bEnough = false;
							}
							break;
						case 3:					// 3张
							{
								// 带牌
								FindTowOne( m_cwOrderCards, 1 );
								bEnough = false;
							}
							break;
						default:
							break;
						}
					}

					if ( bEnough )
					{
						// 单顺，双顺，三顺
						iRe = FindOrderOutgo( end );

						switch ( iRe )
						{
						case 0:					// 失败
							{
								// 3张
								iRe = FindOrderOutgo( sde, 3 );				

								// 失败
								if ( iRe == 0 )
								{				
									// 不够步的时候
									if ( iReHandNum > 1 )
									{
										if ( bOneMid )
										{
											// 找对出
											iRe = FindOrderOutgo( sde, 2 );
											
											if ( iRe == 0 )
											{
												bReOneMid = true;
//												FindBigOne( m_cwOutgo );
											}
										}

										else if ( bTwoMid )
										{
											// 找单出
											iRe = FindOrderOutgo( sde, 1 );
											
											if ( iRe == 0 )
											{
												bReTwoMid = true;
//												FindSmall( 1 );
											}
										}

										// 无警告照常出
										else 
										{
											FindOrderOutgo( sde );
										}
									}

									// 最后一手牌
									else if ( m_cwIncome.empty() )
									{
										FindOrderOutgo( sde );
									}
								}

								// 成功
								else
								{
									FindTowOne( m_cwOrderCards, 1 );
								}
							}
							break;
						case 1:					// 成功,但不是3顺
							break;
						default:				// 3顺
							{
								// 找对应数量的单或者对
								FindTowOne( m_cwOrderCards, iRe );
							}
							break;
						}
					}
				}

				// 收入牌型
				if ( iReHandNum <= 1 || bReOneMid || bReTwoMid )
				{	
					// 单顺，双顺，三顺
					iRe = FindOrderIncome( end );

					switch ( iRe )
					{
					case 0:					// 失败
						{
							// 3张
							iRe = FindOrderIncome( sde, 3 );				

							// 失败
							if ( iRe == 0 )
							{
								if ( iReHandNum <= 1 )
								{
									// 4张
									if ( FindOrderIncome( sde, 4 ) != 0 )
									{
										// 找2个单
										if ( !FindTowOne( m_cwOrderCards, 1, 2 ) )
										{
											// 找2个对
											FindTowOne( m_cwOrderCards, 2, 2 );
										}

										bBreak = true;
									}
								}

								if ( !bBreak )
								{
									if ( bReOneMid )
									{
										if ( FindBigOne( m_cwOutgo ) )
										{
											break;
										}
									}
									else if ( bReTwoMid )
									{
										if ( FindSmall( 1 ) )
										{
											break;
										}
									}

									if ( !Patch3And() )
									{									
										// 单对
										iRe = FindOrderIncome( sde );
										
										if ( iRe == 0 )
										{
											if ( !Patch2w( bOneMid || bTwoMid ) )
											{
												// 2王
												FindOrderIncome( end, 5 );
											}
										}
									}
								}
							}

							// 成功
							else
							{
								FindTowOne( m_cwOrderCards, 1 );
							}
						}
						break;
					case 1:					// 成功,但不是3顺
						break;
					default:				// 3顺
						{
							// 找对应数量的单或者对(感觉这个不太可能)[AAAKKKQQQ]
							FindTowOne( m_cwOrderCards, iRe );
						}
						break;
					}
				}

				// 出牌
				bTemp = m_RoleAI->PushCard( aiRun.rcIn, aiRun.cc );	
			}
			break;
		case cone:									// 单牌
			{
				bool bOneInCome = false;

				// 够步
				if ( ChoseOneOk( aiRun, 1 ) )
				{
					bTemp = m_RoleAI->PushCard( aiRun.rcIn, aiRun.cc );	
					break;
				}

				// 同伴出的牌, 地主不管，咱也不管
				if ( aiRun.aipos == p_up )
				{
					break;
				}
				else
				{
					// 地主报单或对
					if ( bOneMid || bTwoMid )
					{
						// 管上
						if ( !FindOne( aiRun, 1 ) && ( bOneUp || bTwoUp ) )
						{
							PushZha( aiRun );
						}
					}

					// 同伴报单
					else if ( bOneUp )
					{
						// 比自己最小的牌还小不管	
						if ( aiRun.rcIn[0].rc < m_cwCards[m_cwCards.size() - 1].rcard.rc )
						{
							break;
						}
						else
						{
							// 管上
							if ( !FindOne( aiRun, 1 ) )
							{
								PushZha( aiRun );
							}
						}
					}

					// 同伴报对
					else if ( bTwoUp && 
							  aiRun.aipos != p_up )
					{
						// 管上
						if ( !FindOne( aiRun, 1 ) )
						{
							PushZha( aiRun );
						}
					}

					else
					{
						// 顺牌
						if ( !m_cwOutgo.empty() )
						{
							// 顺单
							if ( !FindOneOutgo( aiRun, 1, aiRun.aipos == p_up ) )
							{
								bOneInCome = true;							
							}
						}

						if ( m_cwOutgo.empty() || bOneInCome )
						{
							// 管上(肯定是地主出的)
							FindOneIncome( aiRun, 1 );
						}
					}
					bTemp = m_RoleAI->PushCard( aiRun.rcIn, aiRun.cc );	
				}								
			}
			break;
		case ctwo:									// 对子
			{
				bool bTwoInCome = false;

				// 够步
				if ( ChoseOneOk( aiRun, 1 ) )
				{
					bTemp = m_RoleAI->PushCard( aiRun.rcIn, aiRun.cc );	
					break;
				}

				// 同伴出的牌, 地主不管，咱也不管
				if ( aiRun.aipos == p_up )
				{
					break;
				}
				else
				{
					// 地主报单或对
					if ( bOneMid || bTwoMid )
					{
						// 管上
						if ( !FindOne( aiRun, 2 ) && ( bOneUp || bTwoUp ) )
						{
							PushZha( aiRun );
						}
					}

					// 同伴报单
					else if ( bOneUp && 
							  aiRun.aipos != p_up )
					{
						// 管上
						if ( !FindOne( aiRun, 2 ) )
						{
							PushZha( aiRun );
						}
					}

					// 同伴报对
					else if ( bTwoUp )
					{
						// 比自己最小的牌还小不管
						CARDCHANGE rc;
						for ( int ic = m_cwCards.size() - 1; ic >= 0; --ic )
						{
							if ( m_cwCards[ic].rcard.iNum >= 2 )
							{
								rc = m_cwCards[ic].rcard.rc;
								break;
							}
						}
						if ( aiRun.rcIn[0].rc < rc )
						{
							break;
						}
						else
						{
							// 管上
							if ( !FindOne( aiRun, 2 ) )
							{
								PushZha( aiRun );
							}
						}
					}

					else
					{
						// 顺牌
						if ( !m_cwOutgo.empty() )
						{
							// 顺单
							if ( !FindOneOutgo( aiRun, 2 ) )
							{
								bTwoInCome = true;							
							}
						}

						if ( m_cwOutgo.empty() || bTwoInCome )
						{
							// 管上(肯定是地主出的)
							FindOneIncome( aiRun, 2 );
						}
					}

					bTemp = m_RoleAI->PushCard( aiRun.rcIn, aiRun.cc );	
				}
			}
			break;
		case cthree:									// 3张不带
			{
				ChoseOneDown( aiRun, 3, bOneMid || bTwoMid, bOneUp, bTwoUp );

				bTemp = m_RoleAI->PushCard( aiRun.rcIn, aiRun.cc );	
			}
			break;
		case cfour:									// 4张不带
			{
				ChoseOneDown( aiRun, 4, bOneMid || bTwoMid, bOneUp, bTwoUp );

				bTemp = m_RoleAI->PushCard( aiRun.rcIn, aiRun.cc );	
			}
			break;
		case ctth:										// 三顺(2)
			{
				ChoseLinkDown( aiRun, 3, 2, bOneMid || bTwoMid, bOneUp, bTwoUp );
				
				bTemp = m_RoleAI->PushCard( aiRun.rcIn, aiRun.cc );	
			}
			break;
		case cthone:										// 3带1
			{
				ChoseOneDown( aiRun, 3, bOneMid || bTwoMid, bOneUp, bTwoUp );
				FindTowOne( m_cwOrderCards, 1, 1 );

				bTemp = m_RoleAI->PushCard( aiRun.rcIn, aiRun.cc );	
			}
			break;
		case cthtwo:										// 3带2
			{
				ChoseOneDown( aiRun, 3, bOneMid || bTwoMid, bOneUp, bTwoUp );
				FindTowOne( m_cwOrderCards, 2, 1 );

				bTemp = m_RoleAI->PushCard( aiRun.rcIn, aiRun.cc );	
			}
			break;
//		case cfone:											// 4带1							
//			{
//				ChoseOneDown( aiRun, 4, bOneMid || bTwoMid, bOneUp, bTwoUp );
//				FindTowOne( m_cwOrderCards, 1, 1 );

//				bTemp = m_RoleAI->PushCard( aiRun.rcIn, aiRun.cc );	
//			}
//			break;
//		case cftwo:											// 4带2
//			{
//				ChoseOneDown( aiRun, 4, bOneMid || bTwoMid, bOneUp, bTwoUp );
//				FindTowOne( m_cwOrderCards, 2, 1 );
//
//				bTemp = m_RoleAI->PushCard( aiRun.rcIn, aiRun.cc );	
//			}
//			break;
		case cttth:									// 双顺(3)
			{
				ChoseLinkDown( aiRun, 2, 3, bOneMid || bTwoMid, bOneUp, bTwoUp );
				
				bTemp = m_RoleAI->PushCard( aiRun.rcIn, aiRun.cc );	
			}
			break;
		case cftone:								// 四带两单
			{
				ChoseOneDown( aiRun, 4, bOneMid || bTwoMid, bOneUp, bTwoUp );
				FindTowOne( m_cwOrderCards, 1, 2 );

				bTemp = m_RoleAI->PushCard( aiRun.rcIn, aiRun.cc );	
			}
			break;
		case cfttwo:										// 四带两对
			{
				ChoseOneDown( aiRun, 4, bOneMid || bTwoMid, bOneUp, bTwoUp );
				FindTowOne( m_cwOrderCards, 2, 2 );

				bTemp = m_RoleAI->PushCard( aiRun.rcIn, aiRun.cc );	
			}
			break;
		case ctfour:									// 双顺(4)
			{
				ChoseLinkDown( aiRun, 2, 4, bOneMid || bTwoMid, bOneUp, bTwoUp );
				
				bTemp = m_RoleAI->PushCard( aiRun.rcIn, aiRun.cc );	
			}
			break;
		case cthfour:													// 三顺(4)
			{
				ChoseLinkDown( aiRun, 3, 4, bOneMid || bTwoMid, bOneUp, bTwoUp );
				
				bTemp = m_RoleAI->PushCard( aiRun.rcIn, aiRun.cc );	
			}
			break;
		case ctthone:											// 三顺带两单
			{
				ChoseLinkDown( aiRun, 3, 2, bOneMid || bTwoMid, bOneUp, bTwoUp );
				FindTowOne( m_cwOrderCards, 1, 2 );

				bTemp = m_RoleAI->PushCard( aiRun.rcIn, aiRun.cc );	
			}
			break;
		case ctthtwo:										// 三顺带两对
			{
				ChoseLinkDown( aiRun, 3, 2, bOneMid || bTwoMid, bOneUp, bTwoUp );
				FindTowOne( m_cwOrderCards, 2, 2 );

				bTemp = m_RoleAI->PushCard( aiRun.rcIn, aiRun.cc );	
			}
			break;
		case clink5:									// 单顺(5)
			{
				ChoseLinkDown( aiRun, 1, 5, bOneMid || bTwoMid, bOneUp, bTwoUp );
				
				bTemp = m_RoleAI->PushCard( aiRun.rcIn, aiRun.cc );	
			}
			break;
		case clink6:								// 单顺(6)
			{
				ChoseLinkDown( aiRun, 1, 6, bOneMid || bTwoMid, bOneUp, bTwoUp );
				
				bTemp = m_RoleAI->PushCard( aiRun.rcIn, aiRun.cc );	
			}
			break;
		case clink7:								// 单顺(7)
			{
				ChoseLinkDown( aiRun, 1, 7, bOneMid || bTwoMid, bOneUp, bTwoUp );
				
				bTemp = m_RoleAI->PushCard( aiRun.rcIn, aiRun.cc );	
			}
			break;
		case clink8:									// 单顺(8)
			{
				ChoseLinkDown( aiRun, 1, 8, bOneMid || bTwoMid, bOneUp, bTwoUp );
				
				bTemp = m_RoleAI->PushCard( aiRun.rcIn, aiRun.cc );	
			}
			break;
		case clink9:									// 单顺(9)
			{
				ChoseLinkDown( aiRun, 1, 9, bOneMid || bTwoMid, bOneUp, bTwoUp );
				
				bTemp = m_RoleAI->PushCard( aiRun.rcIn, aiRun.cc );	
			}
			break;
		case clink10:							// 单顺(10)
			{
				ChoseLinkDown( aiRun, 1, 10, bOneMid || bTwoMid, bOneUp, bTwoUp );
				
				bTemp = m_RoleAI->PushCard( aiRun.rcIn, aiRun.cc );	
			}
			break;
		case clink11:								// 单顺(11)
			{
				ChoseLinkDown( aiRun, 1, 11, bOneMid || bTwoMid, bOneUp, bTwoUp );
				
				bTemp = m_RoleAI->PushCard( aiRun.rcIn, aiRun.cc );	
			}
			break;
		case ctfive:									// 双顺(5)
			{
				ChoseLinkDown( aiRun, 2, 5, bOneMid || bTwoMid, bOneUp, bTwoUp );
				
				bTemp = m_RoleAI->PushCard( aiRun.rcIn, aiRun.cc );	
			}
			break;
		case cthfive:								// 三顺(5)
			{
				ChoseLinkDown( aiRun, 3, 5, bOneMid || bTwoMid, bOneUp, bTwoUp );
				
				bTemp = m_RoleAI->PushCard( aiRun.rcIn, aiRun.cc );	
			}
			break;
		case ctsix:								// 双顺(6)
			{
				ChoseLinkDown( aiRun, 2, 6, bOneMid || bTwoMid, bOneUp, bTwoUp );
				
				bTemp = m_RoleAI->PushCard( aiRun.rcIn, aiRun.cc );	
			}
			break;
		case cththone:							// 三顺带三单
			{
				ChoseLinkDown( aiRun, 3, 3, bOneMid || bTwoMid, bOneUp, bTwoUp );
				FindTowOne( m_cwOrderCards, 1, 3 );

				bTemp = m_RoleAI->PushCard( aiRun.rcIn, aiRun.cc );	
			}
			break;
		case cththtwo:							// 三顺带三对
			{
				ChoseLinkDown( aiRun, 3, 3, bOneMid || bTwoMid, bOneUp, bTwoUp );
				FindTowOne( m_cwOrderCards, 2, 3 );

				bTemp = m_RoleAI->PushCard( aiRun.rcIn, aiRun.cc );	
			}
			break;
		case ctseven:							// 双顺(7)
			{
				ChoseLinkDown( aiRun, 2, 7, bOneMid || bTwoMid, bOneUp, bTwoUp );
				
				bTemp = m_RoleAI->PushCard( aiRun.rcIn, aiRun.cc );	
			}
			break;
		case ctevent:				     	// 双顺(8)
			{
				ChoseLinkDown( aiRun, 2, 8, bOneMid || bTwoMid, bOneUp, bTwoUp );
				
				bTemp = m_RoleAI->PushCard( aiRun.rcIn, aiRun.cc );	
			}
			break;
		case cthfone:					// 三顺带四单
			{
				ChoseLinkDown( aiRun, 3, 4, bOneMid || bTwoMid, bOneUp, bTwoUp );
				FindTowOne( m_cwOrderCards, 1, 4 );

				bTemp = m_RoleAI->PushCard( aiRun.rcIn, aiRun.cc );	
			}
			break;
		default:
			break;
	} 

	return bTemp;
}


//
//
//
void KddzAI::ChoseLinkMid( const AIRUN& aiRun, const unsigned int iNum, const unsigned int iWeidth, const bool bIn  )
{
	bool bLinkInCome = false;

	// 够步
	if ( ChoseLinkOk( aiRun, iNum, iWeidth ) )
	{
		return;
	}

	// 农民报单或对
	if ( bIn )
	{
		// 管上,若管不上, 则抗牌
		FindLink( aiRun, iNum, iWeidth );
	}
	else
	{
		// 顺牌
		if ( !m_cwOutgo.empty() )
		{
			if ( !FindLinkOutgo( aiRun, iNum, iWeidth ) )
			{
				bLinkInCome = true;			
			}
		}

		// 管牌
		if ( m_cwOutgo.empty() || bLinkInCome )
		{
			// 管上
			if ( !FindLinkIncome( aiRun, iNum, iWeidth ) )
			{
				// 管牌表
				if ( aiRun.cc == ctth ||
					 aiRun.cc == cttth ||
					 aiRun.cc == cthth ||
					 aiRun.cc == ctfour ||
					 aiRun.cc == cthfour ||
					 aiRun.cc == ctthone ||
					 aiRun.cc == ctthtwo ||
					 aiRun.cc == clink8 ||
					 aiRun.cc == clink9 ||
					 aiRun.cc == clink10 ||
					 aiRun.cc == clink11 ||
					 aiRun.cc == clink12 ||
					 aiRun.cc == ctfive ||
					 aiRun.cc == cthfive ||
					 aiRun.cc == ctsix ||
					 aiRun.cc == cthsix ||
					 aiRun.cc == cththone ||
					 aiRun.cc == cththtwo ||
					 aiRun.cc == ctseven ||
					 aiRun.cc == ctevent ||
					 aiRun.cc == ctnine ||
					 aiRun.cc == cthfone ||
				  	 aiRun.cc == cfttwo ||
					 aiRun.cc == cftone )
				{
					PushZha( aiRun );
				}
			}
		}
	}
}


//
//
//
void KddzAI::ChoseLinkUp( const AIRUN& aiRun, const unsigned int iNum, 
						 const unsigned int iWeidth, const bool bIn,
						 const bool bOneDown, const bool bTwoDown )
{
	bool bLinkInCome = false;

	// 够步
	if ( ChoseLinkOk( aiRun, iNum, iWeidth ) )
	{
		return;
	}

	// 地主报单或对
	if ( bIn )
	{
		// 不是同伴出的牌
		if ( aiRun.aipos != p_down )						
		{
			// 管上
			FindLink( aiRun, iNum, iWeidth );
		}
	}

	// 同伴报对,且地主出的牌
	else if ( bOneDown && 
			  aiRun.aipos != p_down )
	{
		// 管上
		if ( !FindLink( aiRun, iNum, iWeidth ) )
		{
			PushZha( aiRun );
		}
	}

	// 同伴报对
	else if ( bTwoDown && 
			  aiRun.aipos != p_down )
	{
		// 管上
		if ( !FindLink( aiRun, iNum, iWeidth ) )
		{
			PushZha( aiRun );
		}
	}
				
	else
	{
		// 顺牌
		if ( !m_cwOutgo.empty() )
		{
			// 顺单
			if ( !FindLinkOutgo( aiRun, iNum, iWeidth ) &&
				 aiRun.aipos != p_down )
			{
				bLinkInCome = true;						
			}
		}

		if ( m_cwOutgo.empty() || bLinkInCome )
		{
			// 管上(不是同伴才打)
			if ( !FindLinkIncome( aiRun, iNum, iWeidth ) )
			{
				// 管牌表
				if ( aiRun.cc == ctth ||
					 aiRun.cc == cttth ||
					 aiRun.cc == cthth ||
					 aiRun.cc == ctfour ||
					 aiRun.cc == cthfour ||
					 aiRun.cc == ctthone ||
					 aiRun.cc == ctthtwo ||
					 aiRun.cc == clink8 ||
					 aiRun.cc == clink9 ||
					 aiRun.cc == clink10 ||
					 aiRun.cc == clink11 ||
					 aiRun.cc == clink12 ||
					 aiRun.cc == ctfive ||
					 aiRun.cc == cthfive ||
					 aiRun.cc == ctsix ||
					 aiRun.cc == cthsix ||
					 aiRun.cc == cththone ||
					 aiRun.cc == cththtwo ||
					 aiRun.cc == ctseven ||
					 aiRun.cc == ctevent ||
					 aiRun.cc == ctnine ||
					 aiRun.cc == cthfone ||
					 aiRun.cc == cfttwo ||
					 aiRun.cc == cftone )
				{
					PushZha( aiRun );
				}
			}
		}
	}
}


//
//
//
void KddzAI::ChoseLinkDown( const AIRUN& aiRun, const unsigned int iNum, 
						 const unsigned int iWeidth, const bool bIn,
						 const bool bOneUp, const bool bTwoUp )
{
	bool bLinkInCome = false;
	
	// 够步
	if ( ChoseLinkOk( aiRun, iNum, iWeidth ) )
	{
		return;
	}

	// 同伴出的牌, 地主不管，咱也不管
	if ( aiRun.aipos == p_up )
	{
		return;
	}
	else
	{
		// 地主报单或对
		if ( bIn )
		{
			// 管上
			if ( !FindLink( aiRun, iNum, iWeidth ) && ( bOneUp || bTwoUp ) )
			{
				PushZha( aiRun );
			}
		}

		// 同伴报单
		else if ( bOneUp && 
				  aiRun.aipos != p_up )
		{
			// 管上
			if ( !FindLink( aiRun, iNum, iWeidth ) )
			{
				PushZha( aiRun );
			}
		}

		// 同伴报对
		else if ( bTwoUp && 
				 aiRun.aipos != p_up )
		{
			// 管上
			if ( !FindLink( aiRun, 1, 5 ) )
			{
				PushZha( aiRun );
			}
		}

		else
		{
			// 顺牌
			if ( !m_cwOutgo.empty() )
			{
				// 顺单
				if ( !FindLinkOutgo( aiRun, iNum, iWeidth ) )
				{
					bLinkInCome = true;							
				}
			}

			if ( m_cwOutgo.empty() || bLinkInCome )
			{
				// 管上(肯定是地主出的)
				if ( !FindLinkIncome( aiRun, iNum, iWeidth ) )
				{
					// 管牌表
					if ( aiRun.cc == ctth ||
						 aiRun.cc == cttth ||
						 aiRun.cc == cthth ||
						 aiRun.cc == ctfour ||
						 aiRun.cc == cthfour ||
						 aiRun.cc == ctthone ||
						 aiRun.cc == ctthtwo ||
						 aiRun.cc == clink8 ||
						 aiRun.cc == clink9 ||
						 aiRun.cc == clink10 ||
						 aiRun.cc == clink11 ||
						 aiRun.cc == clink12 ||
						 aiRun.cc == ctfive ||
						 aiRun.cc == cthfive ||
						 aiRun.cc == ctsix ||
						 aiRun.cc == cthsix ||
						 aiRun.cc == cththone ||
						 aiRun.cc == cththtwo ||
						 aiRun.cc == ctseven ||
						 aiRun.cc == ctevent ||
						 aiRun.cc == ctnine ||
						 aiRun.cc == cthfone ||
						 aiRun.cc == cfttwo ||
						 aiRun.cc == cftone )
					{
						PushZha( aiRun );
					}
				}
			}
		}
	}
}


//
//
//
void KddzAI::ChoseOneMid( const AIRUN& aiRun, const unsigned int iNum, const bool bIn )
{
	bool bOneInCome = false;

	// 够步
	if ( ChoseOneOk( aiRun, iNum ) )
	{
		return;
	}

	// 农民报单或对
	if ( bIn )
	{
		// 管上,若管不上, 则抗牌
		FindOne( aiRun, iNum );
	}
	else
	{
		// 顺牌
		if ( !m_cwOutgo.empty() )
		{
			// 顺单
			if ( !FindOneOutgo( aiRun, iNum ) )
			{
				bOneInCome = true;			
			}
		}

		// 管牌
		if ( m_cwOutgo.empty() || bOneInCome )
		{
			// 管上
			if ( !FindOneIncome( aiRun, iNum ) )
			{
				// 管牌表
				if ( aiRun.cc == ctth ||
					 aiRun.cc == cttth ||
					 aiRun.cc == cthth ||
					 aiRun.cc == ctfour ||
					 aiRun.cc == cthfour ||
					 aiRun.cc == ctthone ||
					 aiRun.cc == ctthtwo ||
					 aiRun.cc == clink8 ||
					 aiRun.cc == clink9 ||
					 aiRun.cc == clink10 ||
					 aiRun.cc == clink11 ||
					 aiRun.cc == clink12 ||
					 aiRun.cc == ctfive ||
					 aiRun.cc == cthfive ||
					 aiRun.cc == ctsix ||
					 aiRun.cc == cthsix ||
					 aiRun.cc == cththone ||
					 aiRun.cc == cththtwo ||
					 aiRun.cc == ctseven ||
					 aiRun.cc == ctevent ||
					 aiRun.cc == ctnine ||
					 aiRun.cc == cthfone ||
					 aiRun.cc == cfttwo ||
					 aiRun.cc == cftone )
				{
					PushZha( aiRun );
				}
			}
		}
	}
}


//
// 3张以上
//
void KddzAI::ChoseOneUp( const AIRUN& aiRun, const unsigned int iNum, 
					  const bool bIn, const bool bOneDown, const bool bTwoDown )
{
	bool bOneInCome = false;

	// 够步
	if ( ChoseOneOk( aiRun, iNum ) )
	{
		return;
	}

	// 地主报单或对
	if ( bIn &&
		 aiRun.aipos != p_down &&
		 !bOneDown && !bTwoDown )
	{
		// 管上
		FindOne( aiRun, iNum );
	}

	// 同伴报单,且地主出的牌
	else if ( bOneDown && 
			  aiRun.aipos != p_down )
	{
		// 管上
		if ( !FindOne( aiRun, iNum ) )
		{
			PushZha( aiRun );
		}
	}

	// 同伴报对
	else if ( bTwoDown && 
			  aiRun.aipos != p_down )
	{
		// 管上
		if ( !FindOne( aiRun, iNum ) )
		{
			PushZha( aiRun );
		}
	}
			
	else
	{
		// 顺牌
		if ( !m_cwOutgo.empty() )
		{
			// 顺单
			if ( !FindOneOutgo( aiRun, iNum, aiRun.aipos == p_down ) &&
				 aiRun.aipos != p_down )
			{
				bOneInCome = true;			
			}
		}

		if ( m_cwOutgo.empty() || bOneInCome )
		{
			// 管上(不是同伴才打)
			if ( !FindOneIncome( aiRun, iNum ) )
			{
				// 管牌表
				if ( aiRun.cc == ctth ||
					 aiRun.cc == cttth ||
					 aiRun.cc == cthth ||
					 aiRun.cc == ctfour ||
					 aiRun.cc == cthfour ||
					 aiRun.cc == ctthone ||
					 aiRun.cc == ctthtwo ||
					 aiRun.cc == clink8 ||
					 aiRun.cc == clink9 ||
					 aiRun.cc == clink10 ||
					 aiRun.cc == clink11 ||
					 aiRun.cc == clink12 ||
					 aiRun.cc == ctfive ||
					 aiRun.cc == cthfive ||
					 aiRun.cc == ctsix ||
					 aiRun.cc == cthsix ||
					 aiRun.cc == cththone ||
					 aiRun.cc == cththtwo ||
					 aiRun.cc == ctseven ||
					 aiRun.cc == ctevent ||
					 aiRun.cc == ctnine ||
					 aiRun.cc == cthfone ||
					 aiRun.cc == cfttwo ||
					 aiRun.cc == cftone
					 )
				{
					PushZha( aiRun );
				}
			}
		}
	}
}


//
//
//
void KddzAI::ChoseOneDown( const AIRUN& aiRun, const unsigned int iNum, 
					  const bool bIn, const bool bOneUp, const bool bTwoUp )
{
	bool bThreeInCome = false;
	
	// 够步
	if ( ChoseOneOk( aiRun, iNum ) )
	{
		return;
	}

	// 同伴出的牌, 地主不管，咱也不管
	if ( aiRun.aipos == p_up )
	{
		return;
	}
	else
	{
		// 地主报单或对
		if ( bIn && 
			 aiRun.aipos != p_up )
		{
			// 管上(如果自己伙计也报单了，那砸被)
			if ( !FindOne( aiRun, iNum ) && ( bOneUp || bTwoUp ) )
			{
				PushZha( aiRun );
			}
		}

		// 同伴报单
		else if ( bOneUp && 
				  aiRun.aipos != p_up )
		{
			// 管上
			if ( !FindOne( aiRun, iNum ) )
			{
				PushZha( aiRun );
			}
		}

		// 同伴报对
		else if ( bTwoUp && 
				  aiRun.aipos != p_up )
		{
			// 管上
			if ( !FindOne( aiRun, iNum ) )
			{
				PushZha( aiRun );
			}
		}

		else
		{
			// 顺牌
			if ( !m_cwOutgo.empty() )
			{
				// 顺单
				if ( !FindOneOutgo( aiRun, iNum, aiRun.aipos == p_up ) )
				{
					bThreeInCome = true;							
				}
			}

			if ( m_cwOutgo.empty() || bThreeInCome )
			{
				// 管上(肯定是地主出的)
				if ( !FindOneIncome( aiRun, iNum ) )
				{
					// 管牌表
					if ( aiRun.cc == ctth ||
						 aiRun.cc == cttth ||
						 aiRun.cc == cthth ||
						 aiRun.cc == ctfour ||
						 aiRun.cc == cthfour ||
						 aiRun.cc == ctthone ||
						 aiRun.cc == ctthtwo ||
						 aiRun.cc == clink8 ||
						 aiRun.cc == clink9 ||
						 aiRun.cc == clink10 ||
						 aiRun.cc == clink11 ||
						 aiRun.cc == clink12 ||
						 aiRun.cc == ctfive ||
						 aiRun.cc == cthfive ||
						 aiRun.cc == ctsix ||
						 aiRun.cc == cthsix ||
						 aiRun.cc == cththone ||
						 aiRun.cc == cththtwo ||
						 aiRun.cc == ctseven ||
						 aiRun.cc == ctevent ||
						 aiRun.cc == ctnine ||
						 aiRun.cc == cthfone ||
						 aiRun.cc == cfttwo ||
						 aiRun.cc == cftone
						 )
					{
						PushZha( aiRun );
					}
				}
			}
		}
	}
}



//
// 取得牌型的手数(计算够步):bIn用来处理出炸弹对够步的影响
//
int KddzAI::GetHandNum( const bool bIn )
{
	int iSdeOne = 0;
	int iSdeTwo = 0;
	int iSde = 0;
	int ibeg = 0;
	bool bTemp = bIn;

	if ( !m_cwOutgo.empty() )
	{
		for ( int i = 0; i < m_cwOutgo.size(); ++i )
		{
			if ( m_cwOutgo[i].csif == sde )
			{
				++iSde;
				if ( m_cwOutgo[i].rcard.iNum == 1 )
				{
					++iSdeOne;
				}
				else if ( m_cwOutgo[i].rcard.iNum == 2 )
				{
					++iSdeTwo;
				}
			}
			else if ( m_cwOutgo[i].csif == beg )
			{
				++ibeg;
			}

		}
	}

	if ( !m_cwIncome.empty() )
	{
		for ( int j = 0; j < m_cwIncome.size(); ++j )
		{
			if ( m_cwIncome[j].csif == sde )
			{
				// 有3张情况，但是不连不能带
				if ( m_cwIncome[j].rcard.iNum == 3 &&
					 ( iSdeOne >= 1 || iSdeTwo >= 1 ) )
				{
					--iSde;
				}
				else if ( m_cwIncome[j].rcard.iNum == 4 )
				{	
					if ( bTemp )
					{						
						if ( iSdeOne >= 2 || iSdeTwo >= 2 )
						{
							iSde -= 2;
						}
					}
					else
					{
						// 一个以上的炸弹,BUG处理
						bTemp = true;
					}
				}
			}
			
			// 3顺
			else if ( m_cwIncome[j].csif == beg &&
				      m_cwIncome[j].rcard.iNum == 3 )
			{
				int jTemp = j;
				while ( jTemp < m_cwIncome.size() )
				{
					if ( m_cwIncome[jTemp].csif == end )
					{
						break;
					}
					++jTemp;
				}

				int iTemp = jTemp - j + 1;

				if ( iSdeOne >= iTemp || iSdeTwo >= iTemp )
				{
					iSde -= iTemp;
				}
			}
		}
	}

	iSde = ( iSde <= 0 ) ? 0 : iSde;

	return ibeg + iSde;
}


//
// 砸
//
bool KddzAI::ChoseOneOk( const AIRUN& aiRun, const unsigned int iIn )
{
	int iFour = ( iIn == 4 ) ? 4 : 3;
	CARDCHANGE rcTemp = R3;				// 最小

	// 够步
	if ( GetHandNum( true ) <= 1 )	
	{
		// 找出最大
		for ( int rc = aiRun.rcIn.size() - 1; rc >= 0; --rc )
		{	
			if ( aiRun.rcIn[rc].iNum == iIn &&
				 aiRun.rcIn[rc].rc > rcTemp )
			{
				rcTemp = aiRun.rcIn[rc].rc;
			}
		}

		if ( !m_cwIncome.empty() )
		{
			// 找SDE(匹配的)
			for ( int i = m_cwIncome.size() - 1; i >= 0; --i )
			{
				if ( m_cwIncome[i].csif == sde &&
					 m_cwIncome[i].rcard.iNum == iIn &&
					 m_cwIncome[i].rcard.rc > rcTemp )
				{
					CWVector cwTemp;
					CARDWORTH cw;

					cw.csif = sde;
					cw.rcard.iNum = iIn;
					cw.rcard.rc = m_cwIncome[i].rcard.rc;
					cwTemp.push_back( cw );

					if ( GetBig( cwTemp ) )
					{
						ChooseCards( m_cwIncome[i].rcard.rc, iIn );
						return true;
					}
				}
			}
		

			if ( GetHandNum( false ) <= 1 )
			{
				// 找炸弹
				for ( int n = m_cwIncome.size() - 1; n >= 0 ; --n )
				{
					if ( m_cwIncome[n].rcard.iNum == 4 )
					{
						ChooseCards( m_cwIncome[n].rcard.rc, 4 );
						return true;
					}
				}
			}

			// 找双王
			if ( m_cwIncome.size() >= 2 &&
				 m_cwIncome[0].rcard.rc == RWB &&
				 m_cwIncome[1].rcard.rc == RWS )
			{
				ChooseCards( m_cwIncome[0].rcard.rc, 1 );
				ChooseCards( m_cwIncome[1].rcard.rc, 1 );
				return true;
			}
			
			// 都是大牌了,拆
			if ( m_cwOutgo.empty() )
			{
				for ( int j = m_cwIncome.size() - 1; j >= 0; --j )
				{
					if ( m_cwIncome[j].csif == sde &&
						 m_cwIncome[j].rcard.iNum >= iIn &&
						 m_cwIncome[j].rcard.rc > rcTemp )
					{
						CWVector cwTemp;
						CARDWORTH cw;

						cw.csif = sde;
						cw.rcard.iNum = iIn;
						cw.rcard.rc = m_cwIncome[j].rcard.rc;
						cwTemp.push_back( cw );

						if ( m_cwIncome[j].rcard.rc == RT || GetBig( cwTemp ) )
						{
							ChooseCards( m_cwIncome[j].rcard.rc, iIn );
							return true;
						}
					}
				}
			}
		}

		

		// 没大牌了
		if ( !m_cwOutgo.empty() )
		{
			for ( int j = 0; j < m_cwOutgo.size(); ++j )
			{
				if ( m_cwOutgo[j].csif == sde &&
					 m_cwOutgo[j].rcard.iNum == iIn &&
					 m_cwOutgo[j].rcard.rc > rcTemp )
				{
					ChooseCards( m_cwOutgo[j].rcard.rc, 
								 m_cwOutgo[j].rcard.iNum );
					return true;
				}
			}
		}
	}

	return false;
}



//
// 砸
//
bool KddzAI::ChoseLinkOk( const AIRUN& aiRun, const unsigned int iNum, const unsigned int iWeidth )
{
	CARDCHANGE rcTemp = RWB;				// 最大

	// 够步
	if ( GetHandNum( true ) <= 1 )	
	{
		// 找出最小
		for ( int rc = aiRun.rcIn.size() - 1; rc >= 0; --rc )
		{	
			if ( aiRun.rcIn[rc].iNum == iNum &&
				 aiRun.rcIn[rc].rc < rcTemp )
			{
				rcTemp = aiRun.rcIn[rc].rc;
			}
		}

		if ( !m_cwIncome.empty() )
		{
			// 找配对的
			for ( int i = m_cwIncome.size() - 1; i >= 0; --i )
			{
				if ( m_cwIncome[i].csif == end &&
					 m_cwIncome[i].rcard.iNum == iNum &&
					 m_cwIncome[i].rcard.rc > rcTemp )
				{
					int iTempWhile = i;
					while ( iTempWhile >= 0 )
					{
						if ( m_cwIncome[iTempWhile].csif == beg )
						{
							break;
						}
						--iTempWhile;
					}

					if ( iTempWhile - i + 1 == iWeidth )
					{
						while ( i >= iTempWhile )
						{
							ChooseCards( m_cwIncome[i].rcard.rc, 
										 m_cwIncome[i].rcard.iNum );
							--i;
						}

						return true;
					}
				}
			}

			if ( GetHandNum( false ) <= 1 )
			{
				// 找炸弹
				for ( int n = m_cwIncome.size() - 1; n >= 0 ; --n )
				{
					if ( m_cwIncome[n].rcard.iNum == 4 )
					{
						ChooseCards( m_cwIncome[n].rcard.rc, 4 );
						return true;
					}
				}
			}

			// 找双王
			if ( m_cwIncome.size() >= 2 &&
				 m_cwIncome[0].rcard.rc == RWB &&
				 m_cwIncome[1].rcard.rc == RWS )
			{
				ChooseCards( m_cwIncome[0].rcard.rc, 1 );
				ChooseCards( m_cwIncome[1].rcard.rc, 1 );
				return true;
			}
		}

		// 没大牌了
		if ( !m_cwOutgo.empty() )
		{
			for ( int j = 0; j < m_cwOutgo.size(); ++j )
			{
				if ( m_cwOutgo[j].csif == end &&
					 m_cwOutgo[j].rcard.iNum == iNum &&
					 m_cwOutgo[j].rcard.rc > rcTemp )
				{
					int iTempWhile = j;
					while ( iTempWhile >= 0 )
					{
						if ( m_cwOutgo[iTempWhile].csif == beg )
						{
							break;
						}
						--iTempWhile;
					}

					if ( iTempWhile - j + 1 == iWeidth )
					{
						while ( j >= iTempWhile )
						{
							ChooseCards( m_cwOutgo[j].rcard.rc, 
										 m_cwOutgo[j].rcard.iNum );
							--j;
						}
						return true;
					}
				}
			}
		}
	}

	return false;
}


//
// 出炸弹
//
void KddzAI::PushZha( const AIRUN& aiRun )
{
	if ( !m_cwIncome.empty() )
	{
		
		// 找炸弹
		for ( int n = m_cwIncome.size() - 1; n >= 0 ; --n )
		{
			if ( m_cwIncome[n].rcard.iNum == 4 )
			{
				ChooseCards( m_cwIncome[n].rcard.rc, 4 );
				return;
			}
		}

		// 找双王
		if ( m_cwIncome.size() >= 2 &&
			 m_cwIncome[0].rcard.rc == RWB &&
			 m_cwIncome[1].rcard.rc == RWS )
		{
			ChooseCards( m_cwIncome[0].rcard.rc, 1 );
			ChooseCards( m_cwIncome[1].rcard.rc, 1 );
		}
	}
}


//
// 拆王
//
bool KddzAI::Patch2w( const bool bIn )
{
	// 对家警报,并且自己不够步
	if ( bIn && GetHandNum( true ) > 1 )
	{
		// 拆王
		if ( m_cwIncome.size() >= 2 &&
			 m_cwIncome[0].rcard.rc == RWB &&
			 m_cwIncome[1].rcard.rc == RWS )
		{
			// 选小王
			ChooseCards( m_cwIncome[1].rcard.rc, 1 );
			return true;
		}
	}

	return false;
}



//
// 3带一的补丁
//
bool KddzAI::Patch3And( void )
{
	if ( !m_cwIncome.empty() && !m_cwOutgo.empty() )
	{
		if ( m_cwIncome.size() == 1 && 
			 m_cwIncome[0].csif == sde &&
			 ( m_cwIncome[0].rcard.iNum <= 2 ) )
		{
			if ( m_cwOutgo.size() == 1 && 
				 m_cwOutgo[0].csif == sde &&
				 m_cwOutgo[0].rcard.iNum == 3 )
			{
				ChooseCards( m_cwIncome[0].rcard.rc, m_cwIncome[0].rcard.iNum );
				ChooseCards( m_cwOutgo[0].rcard.rc, m_cwOutgo[0].rcard.iNum );
				return true;
			}	
		}
	}

	return false;
}


//
// 预够步, Outgo handnum == 2
//
int KddzAI::UpEnough( void )
{
	// iNum 不会有4的可能
	for ( int i = m_cwOutgo.size() - 1; i >= 0; --i )
	{
		// 单
		if ( m_cwOutgo[i].csif == sde )
		{
			for ( int j = m_cwIncome.size() - 1; j >= 0; --j )
			{
				// 个数相等
				if ( m_cwOutgo[i].rcard.iNum == m_cwIncome[j].rcard.iNum )
				{
					ChooseCards( m_cwOutgo[i].rcard.rc, m_cwOutgo[i].rcard.iNum );
					return m_cwOutgo[i].rcard.iNum;
				}
			}
		}

		// 顺
		else if ( m_cwOutgo[i].csif == end )
		{
			// 不处理,因为单找不到能管上的牌，还是会优先出顺的
		}
	}
	
	return 0;
}