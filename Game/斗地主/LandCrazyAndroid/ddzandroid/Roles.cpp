
//
// 2009
// DhLj
//

#include "Roles.h"

//
// *
//
Roles::Roles( void ) 
{
}

//
// *
//
Roles::~Roles( void )
{
}

//
// 排序
//
void Roles::SetCards( IVector& ivIn )
{
	int iTemp;

	// 清空m_Cards
	if ( !m_Cards.empty() )
	{
		m_Cards.erase( m_Cards.begin(), m_Cards.end() );
	}

	// 选择排序
	for ( int j = 0; j < ivIn.size(); ++j )
	{
		for ( int i = 0; i < ivIn.size(); ++i )
		{
			if ( ivIn[j] > ivIn[i] )
			{
				iTemp = ivIn[j];
				ivIn[j] = ivIn[i];
				ivIn[i] = iTemp;
			}
		}
	}

	CARD cTemp;
	for ( int k = 0; k < ivIn.size(); ++k )
	{	
		cTemp.iCard = ivIn[k];
		cTemp.cs = no;

		m_Cards.push_back( cTemp );
	}
}


//
// 转换牌,保存到m_RCards
//
void Roles::ChangeCard( void )
{
	RCARD rcTemp;
	int iTemp = 0;

	// 出错处理
	if ( m_ivTemp.empty() )
	{
		return;
	}

	// 清空m_RCards
	if ( !m_RCards.empty() )
	{
		m_RCards.erase( m_RCards.begin(), m_RCards.end() );
	}

	for ( int j = 0; j < m_ivTemp.size(); ++j )
	{
		if ( m_ivTemp[j] > 0 && m_ivTemp[j] <= 52 )			// 排除大小王
		{
			iTemp = ( m_ivTemp[j] - 1 ) / 4 + 3;
		}
		else
		{
			iTemp = m_ivTemp[j];
		}
		
		switch ( iTemp )
		{
		case 3:
			rcTemp.rc = R3;
			break;
		case 4:
			rcTemp.rc = R4;
			break;
		case 5:
			rcTemp.rc = R5;
			break;
		case 6:
			rcTemp.rc = R6;
			break;
		case 7:
			rcTemp.rc = R7;
			break;
		case 8:
			rcTemp.rc = R8;
			break;
		case 9:
			rcTemp.rc = R9;
			break;
		case 10:
			rcTemp.rc = RO;
			break;
		case 11:
			rcTemp.rc = RJ;
			break;
		case 12:
			rcTemp.rc = RQ;
			break;
		case 13:
			rcTemp.rc = RK;
			break;
		case 14:
			rcTemp.rc = RA;
			break;
		case 15:
			rcTemp.rc = RT;
			break;
		case 53:
			rcTemp.rc = RWS;
			break;
		case 54:
			rcTemp.rc = RWB;
			break;
		default:
			break;
		}

		// 这样要保证VECTOR的有序排列
		if ( !m_RCards.empty() && m_RCards[m_RCards.size() - 1].rc == rcTemp.rc )
		{
			++m_RCards[m_RCards.size() - 1].iNum;
		}
		else
		{
			rcTemp.iNum = 1;
			m_RCards.push_back( rcTemp );
		}

	}
}

//附加
bool Roles::ChangeCardToIvVertor(const IVector& ivIn, RCVector& rcVec)
{
	RCARD rcTemp;
	int iTemp = 0;

	// 出错处理
	if ( ivIn.empty() )
	{
		return false;
	}

	// 清空m_RCards
	if ( !rcVec.empty() )
	{
		rcVec.erase( rcVec.begin(), rcVec.end() );
	}

	for ( int j = 0; j < ivIn.size(); ++j )
	{
		if ( ivIn[j] > 0 && ivIn[j] <= 52 )			// 排除大小王
		{
			iTemp = ( ivIn[j] - 1 ) / 4 + 3;
		}
		else
		{
			iTemp = ivIn[j];
		}

		switch ( iTemp )
		{
		case 3:
			rcTemp.rc = R3;
			break;
		case 4:
			rcTemp.rc = R4;
			break;
		case 5:
			rcTemp.rc = R5;
			break;
		case 6:
			rcTemp.rc = R6;
			break;
		case 7:
			rcTemp.rc = R7;
			break;
		case 8:
			rcTemp.rc = R8;
			break;
		case 9:
			rcTemp.rc = R9;
			break;
		case 10:
			rcTemp.rc = RO;
			break;
		case 11:
			rcTemp.rc = RJ;
			break;
		case 12:
			rcTemp.rc = RQ;
			break;
		case 13:
			rcTemp.rc = RK;
			break;
		case 14:
			rcTemp.rc = RA;
			break;
		case 15:
			rcTemp.rc = RT;
			break;
		case 53:
			rcTemp.rc = RWS;
			break;
		case 54:
			rcTemp.rc = RWB;
			break;
		default:
			break;
		}

		// 这样要保证VECTOR的有序排列
		if ( !rcVec.empty() && rcVec[rcVec.size() - 1].rc == rcTemp.rc )
		{
			++rcVec[rcVec.size() - 1].iNum;
		}
		else
		{
			rcTemp.iNum = 1;
			rcVec.push_back( rcTemp );
		}

	}

	return true;
}


CARDCLASS Roles::GetCardRCVector(const RCVector& rcVec)
{
	CARDCLASS ccTemp = cerror;

	switch ( rcVec.size() )
	{
	case 1:												// 一种牌
		{
			switch ( rcVec[0].iNum )
			{
			case 1:										// 单张
				{
					ccTemp = cone;
				}
				break;
			case 2:										// 对子
				{
					ccTemp = ctwo;
				}
				break;
			case 3:										// 三张
				{
					ccTemp = cthree;
				}
				break;
			case 4:										// 炸弹
				{
					ccTemp = cfour;
				}
				break;
			default:
				break;
			}
		}
		break;
	case 2:
		{
			// 三顺
			if ( rcVec[0].rc <= RA && 
				FindLinkTemp(rcVec,  3, 0, 1 ) )
			{
				ccTemp = ctth;
			}

			// 三带一单
			else if ( rcVec[0].iNum == 3 && rcVec[1].iNum == 1 ||
				rcVec[1].iNum == 3 && rcVec[0].iNum == 1 )
			{
				ccTemp = cthone;
			}

			// 三带一对
			else if ( rcVec[0].iNum == 3 && rcVec[1].iNum == 2 ||
				rcVec[1].iNum == 3 && rcVec[0].iNum == 2 )
			{
				ccTemp = cthtwo;
			}

			//			// 四带一单
			//			else if ( rcVec[0].iNum == 4 && rcVec[1].iNum == 1 ||
			//					  rcVec[1].iNum == 4 && rcVec[0].iNum == 1 )
			//			{
			//				ccTemp = cfone;
			//			}
			//
			//			// 四带一对
			//			else if ( rcVec[0].iNum == 4 && rcVec[1].iNum == 2 ||
			//					  rcVec[1].iNum == 4 && rcVec[0].iNum == 2 )
			//			{
			//				ccTemp = cftwo;
			//			}

			// 双王
			else if ( ( rcVec[0].rc == RWS && rcVec[1].rc == RWB ) ||
				( rcVec[1].rc == RWS && rcVec[0].rc == RWB ) )
			{
				ccTemp = chigh;
			}
		}
		break;
	case 3:
		{
			// 双顺(3)	
			if ( rcVec[0].rc <= RA && FindLinkTemp(rcVec, 2, 0, 2 ) )
			{
				ccTemp = cttth;
			}

			// 三顺(3)	
			else if ( rcVec[0].rc <= RA && FindLinkTemp(rcVec, 3, 0, 2 ) )
			{
				ccTemp = cthth;
			}

			// 四带两单
			else if ( rcVec[0].iNum == 4 && rcVec[1].iNum == 1 &&
				rcVec[2].iNum == 1 ||
				rcVec[1].iNum == 4 && rcVec[0].iNum == 1 &&
				rcVec[2].iNum == 1 ||
				rcVec[2].iNum == 4 && rcVec[0].iNum == 1 &&
				rcVec[1].iNum == 1 )
			{
				ccTemp = cftone;
			}

			// 四带两对
			else if ( rcVec[0].iNum == 4 && rcVec[1].iNum == 2 &&
				rcVec[2].iNum == 2 ||
				rcVec[1].iNum == 4 && rcVec[0].iNum == 2 &&
				rcVec[2].iNum == 2 ||
				rcVec[2].iNum == 4 && rcVec[0].iNum == 2 &&
				rcVec[1].iNum == 2 )
			{
				ccTemp = cfttwo;
			}
		}
		break;
	case 4:
		{
			// 双顺(4)
			if ( rcVec[0].rc <= RA && FindLinkTemp(rcVec, 2, 0, 3 ) )
			{
				ccTemp = ctfour;
			}

			// 三顺(4)
			else if ( rcVec[0].rc <= RA && FindLinkTemp(rcVec, 3, 0, 3 ) )
			{
				ccTemp = cthfour;
			}

			// 三顺带两单
			else if ( rcVec[0].rc <= RA &&
				( FindLinkTemp(rcVec, 3, 0, 1 ) && 
				rcVec[2].iNum == 1 && rcVec[3].iNum == 1 ) ||
				( FindLinkTemp(rcVec, 3, 1, 2 ) && 
				rcVec[0].iNum == 1 && rcVec[3].iNum == 1 ) ||
				( FindLinkTemp(rcVec, 3, 2, 3 ) && 
				rcVec[0].iNum == 1 && rcVec[1].iNum == 1 ) )
			{
				ccTemp = ctthone;
			}

			// 三顺带两对
			else if ( rcVec[0].rc <= RA &&
				( FindLinkTemp(rcVec, 3, 0, 1 ) && 
				rcVec[2].iNum == 2 && rcVec[3].iNum == 2 ) ||
				( FindLinkTemp(rcVec, 3, 1, 2 ) && 
				rcVec[0].iNum == 2 && rcVec[3].iNum == 2 ) ||
				( FindLinkTemp(rcVec, 3, 2, 3 ) && 
				rcVec[0].iNum == 2 && rcVec[1].iNum == 2 ) )
			{
				ccTemp = ctthtwo;
			}

		}
		break;
	case 5:
		{
			// 单顺(5)
			if ( rcVec[0].rc <= RA &&
				FindLinkTemp(rcVec, 1, 0, 4 ) )
			{
				ccTemp = clink5;
			}

			// 双顺(5)
			else if ( rcVec[0].rc <= RA && FindLinkTemp(rcVec, 2, 0, 4 ) )
			{
				ccTemp = ctfive;
			}

			// 三顺(5)
			else if ( rcVec[0].rc <= RA && FindLinkTemp(rcVec, 3, 0, 4 ) )
			{
				ccTemp = cthfive;
			}
		}
		break;
	case 6:
		{
			// 单顺(6)
			if ( rcVec[0].rc <= RA && FindLinkTemp(rcVec, 1, 0, 5 ) )
			{
				ccTemp = clink6;
			}

			// 双顺(6)
			else if ( rcVec[0].rc <= RA && FindLinkTemp(rcVec, 2, 0, 5 ) )
			{
				ccTemp = ctsix;
			}

			// 三顺(6)
			else if ( rcVec[0].rc <= RA && FindLinkTemp(rcVec, 3, 0, 5 ) )
			{
				ccTemp = cthsix;
			}

			// 三顺带三单
			else if ( rcVec[0].rc <= RA &&
				( FindLinkTemp(rcVec, 3, 0, 2 ) && 
				rcVec[3].iNum == 1 && rcVec[4].iNum == 1 &&
				rcVec[5].iNum == 1 ) ||
				( FindLinkTemp(rcVec, 3, 1, 3 ) && 
				rcVec[0].iNum == 1 && rcVec[4].iNum == 1 &&
				rcVec[5].iNum == 1 ) ||
				( FindLinkTemp(rcVec, 3, 2, 4 ) && 
				rcVec[0].iNum == 1 && rcVec[1].iNum == 1 &&
				rcVec[5].iNum == 1 ) ||
				( FindLinkTemp(rcVec, 3, 3, 5 ) && 
				rcVec[0].iNum == 1 && rcVec[1].iNum == 1 &&
				rcVec[2].iNum == 1 ) )
			{
				ccTemp = cththone;
			}

			// 三顺带三对
			else if ( rcVec[0].rc <= RA &&
				( FindLinkTemp(rcVec, 3, 0, 2 ) && 
				rcVec[3].iNum == 2 && rcVec[4].iNum == 2 &&
				rcVec[5].iNum == 2 ) ||
				( FindLinkTemp(rcVec, 3, 1, 3 ) && 
				rcVec[0].iNum == 2 && rcVec[4].iNum == 2 &&
				rcVec[5].iNum == 2 ) ||
				( FindLinkTemp(rcVec, 3, 2, 4 ) && 
				rcVec[0].iNum == 2 && rcVec[1].iNum == 2 &&
				rcVec[5].iNum == 2 ) ||
				( FindLinkTemp(rcVec, 3, 3, 5 ) && 
				rcVec[0].iNum == 2 && rcVec[1].iNum == 2 &&
				rcVec[2].iNum == 2 ) )
			{
				ccTemp = cththtwo;
			}
		}
		break;
	case 7:
		{
			// 单顺(7)
			if ( rcVec[0].rc <= RA && FindLinkTemp(rcVec, 1, 0, 6 ) )
			{
				ccTemp = clink7;
			}

			// 双顺(7)
			else if ( rcVec[0].rc <= RA && FindLinkTemp(rcVec, 2, 0, 6 ) )
			{
				ccTemp = ctseven;
			}
		}
		break;
	case 8:
		{
			// 单顺(8)
			if ( rcVec[0].rc <= RA && FindLinkTemp(rcVec, 1, 0, 7 ) )
			{
				ccTemp = clink8;
			}

			// 双顺(8)
			else if ( rcVec[0].rc <= RA && FindLinkTemp(rcVec, 2, 0, 7 ) )
			{
				ccTemp = ctevent;
			}

			// 三顺带四单
			else if ( rcVec[0].rc <= RA &&
				( FindLinkTemp(rcVec, 3, 0, 3 ) && 
				rcVec[4].iNum == 1 && rcVec[5].iNum == 1 &&
				rcVec[6].iNum == 1 && rcVec[7].iNum == 1 ) ||
				( FindLinkTemp(rcVec, 3, 1, 4 ) && 
				rcVec[0].iNum == 1 && rcVec[5].iNum == 1 &&
				rcVec[6].iNum == 1 && rcVec[7].iNum == 1 ) ||
				( FindLinkTemp(rcVec, 3, 2, 5 ) && 
				rcVec[0].iNum == 1 && rcVec[1].iNum == 1 &&
				rcVec[6].iNum == 1 && rcVec[7].iNum == 1 ) ||
				( FindLinkTemp(rcVec, 3, 3, 6 ) && 
				rcVec[0].iNum == 1 && rcVec[1].iNum == 1 &&
				rcVec[2].iNum == 1 && rcVec[7].iNum == 1 ) ||
				( FindLinkTemp(rcVec, 3, 4, 7 ) && 
				rcVec[0].iNum == 1 && rcVec[1].iNum == 1 &&
				rcVec[2].iNum == 1 && rcVec[3].iNum == 1 ) )
			{
				ccTemp = cthfone;
			}

			// 三顺带四对
			else if ( rcVec[0].rc <= RA &&
				( FindLinkTemp(rcVec, 3, 0, 3 ) && 
				rcVec[4].iNum == 2 && rcVec[5].iNum == 2 &&
				rcVec[6].iNum == 2 && rcVec[7].iNum == 2 ) ||
				( FindLinkTemp(rcVec, 3, 1, 4 ) && 
				rcVec[0].iNum == 2 && rcVec[5].iNum == 2 &&
				rcVec[6].iNum == 2 && rcVec[7].iNum == 2 ) ||
				( FindLinkTemp(rcVec, 3, 2, 5 ) && 
				rcVec[0].iNum == 2 && rcVec[1].iNum == 2 &&
				rcVec[6].iNum == 2 && rcVec[7].iNum == 2 ) ||
				( FindLinkTemp(rcVec, 3, 3, 6 ) && 
				rcVec[0].iNum == 2 && rcVec[1].iNum == 2 &&
				rcVec[2].iNum == 2 && rcVec[7].iNum == 2 ) ||
				( FindLinkTemp(rcVec, 3, 4, 7 ) && 
				rcVec[0].iNum == 2 && rcVec[1].iNum == 2 &&
				rcVec[2].iNum == 2 && rcVec[3].iNum == 2 ) )
			{
				ccTemp = cthftwo;
			}
		}
		break;
	case 9:
		{
			// 单顺(9)
			if ( rcVec[0].rc <= RA && FindLinkTemp(rcVec, 1, 0, 8 ) )
			{
				ccTemp = clink9;
			}

			// 双顺(8)
			else if ( rcVec[0].rc <= RA && FindLinkTemp(rcVec, 2, 0, 8 ) )
			{
				ccTemp = ctnine;
			}
		}
		break;
	case 10:
		{
			// 单顺(10)
			if ( rcVec[0].rc <= RA && FindLinkTemp(rcVec, 1, 0, 9 ) )
			{
				ccTemp = clink10;
			}

			// 双顺(10)
			else if ( rcVec[0].rc <= RA && FindLinkTemp(rcVec, 2, 0, 9 ) )
			{
				ccTemp = ctten;
			}
		}
		break;
	case 11:
		{
			// 单顺(11)
			if ( rcVec[0].rc <= RA && FindLinkTemp(rcVec, 1, 0, 10 ) )
			{
				ccTemp = clink11;
			}
		}
		break;
	case 12:
		{
			// 单顺(12)
			if ( rcVec[0].rc <= RA && FindLinkTemp(rcVec, 1, 0, 11 ) )
			{
				ccTemp = clink12;
			}
		}
		break;
	default:
		break;
	}

	return ccTemp;
}

//
// 找顺
//
bool Roles::FindLinkTemp(const RCVector& rcVec, const int iOr, const int iF, const int iL )
{
	bool bTemp = true;

	for ( int i = iF; bTemp && i < iL; ++i )
	{
		if ( rcVec[i].rc - rcVec[i + 1].rc != 1 ||
			rcVec[i].iNum != iOr || 
			rcVec[i + 1].iNum != iOr)
		{
			bTemp = false;
		}
	}

	return bTemp;
}



//
// 牌分类
//
CARDCLASS Roles::SelectClass( void )
{
	CARDCLASS ccTemp = cerror;

	switch ( m_RCards.size() )
	{
	case 1:												// 一种牌
		{
			switch ( m_RCards[0].iNum )
			{
			case 1:										// 单张
				{
					ccTemp = cone;
				}
				break;
			case 2:										// 对子
				{
					ccTemp = ctwo;
				}
				break;
			case 3:										// 三张
				{
					ccTemp = cthree;
				}
				break;
			case 4:										// 炸弹
				{
					ccTemp = cfour;
				}
				break;
			default:
				break;
			}
		}
		break;
	case 2:
		{
			// 三顺
			if ( m_RCards[0].rc <= RA && 
				 FindLink( 3, 0, 1 ) )
			{
				ccTemp = ctth;
			}

			// 三带一单
			else if ( m_RCards[0].iNum == 3 && m_RCards[1].iNum == 1 ||
				 m_RCards[1].iNum == 3 && m_RCards[0].iNum == 1 )
			{
				ccTemp = cthone;
			}

			// 三带一对
			else if ( m_RCards[0].iNum == 3 && m_RCards[1].iNum == 2 ||
					  m_RCards[1].iNum == 3 && m_RCards[0].iNum == 2 )
			{
				ccTemp = cthtwo;
			}

//			// 四带一单
//			else if ( m_RCards[0].iNum == 4 && m_RCards[1].iNum == 1 ||
//					  m_RCards[1].iNum == 4 && m_RCards[0].iNum == 1 )
//			{
//				ccTemp = cfone;
//			}
//
//			// 四带一对
//			else if ( m_RCards[0].iNum == 4 && m_RCards[1].iNum == 2 ||
//					  m_RCards[1].iNum == 4 && m_RCards[0].iNum == 2 )
//			{
//				ccTemp = cftwo;
//			}
			
			// 双王
			else if ( ( m_RCards[0].rc == RWS && m_RCards[1].rc == RWB ) ||
					  ( m_RCards[1].rc == RWS && m_RCards[0].rc == RWB ) )
			{
				ccTemp = chigh;
			}
		}
		break;
	case 3:
		{
			// 双顺(3)	
			if ( m_RCards[0].rc <= RA && FindLink( 2, 0, 2 ) )
			{
				ccTemp = cttth;
			}

			// 三顺(3)	
			else if ( m_RCards[0].rc <= RA && FindLink( 3, 0, 2 ) )
			{
				ccTemp = cthth;
			}

			// 四带两单
			else if ( m_RCards[0].iNum == 4 && m_RCards[1].iNum == 1 &&
					  m_RCards[2].iNum == 1 ||
				      m_RCards[1].iNum == 4 && m_RCards[0].iNum == 1 &&
					  m_RCards[2].iNum == 1 ||
					  m_RCards[2].iNum == 4 && m_RCards[0].iNum == 1 &&
					  m_RCards[1].iNum == 1 )
			{
				ccTemp = cftone;
			}

			// 四带两对
			else if ( m_RCards[0].iNum == 4 && m_RCards[1].iNum == 2 &&
					  m_RCards[2].iNum == 2 ||
					  m_RCards[1].iNum == 4 && m_RCards[0].iNum == 2 &&
					  m_RCards[2].iNum == 2 ||
					  m_RCards[2].iNum == 4 && m_RCards[0].iNum == 2 &&
					  m_RCards[1].iNum == 2 )
			{
				ccTemp = cfttwo;
			}
		}
		break;
	case 4:
		{
			// 双顺(4)
			if ( m_RCards[0].rc <= RA && FindLink( 2, 0, 3 ) )
			{
				ccTemp = ctfour;
			}

			// 三顺(4)
			else if ( m_RCards[0].rc <= RA && FindLink( 3, 0, 3 ) )
			{
				ccTemp = cthfour;
			}

			// 三顺带两单
			else if ( m_RCards[0].rc <= RA &&
				    ( FindLink( 3, 0, 1 ) && 
					  m_RCards[2].iNum == 1 && m_RCards[3].iNum == 1 ) ||
					( FindLink( 3, 1, 2 ) && 
					  m_RCards[0].iNum == 1 && m_RCards[3].iNum == 1 ) ||
					( FindLink( 3, 2, 3 ) && 
					  m_RCards[0].iNum == 1 && m_RCards[1].iNum == 1 ) )
			{
				ccTemp = ctthone;
			}

			// 三顺带两对
			else if ( m_RCards[0].rc <= RA &&
				    ( FindLink( 3, 0, 1 ) && 
					  m_RCards[2].iNum == 2 && m_RCards[3].iNum == 2 ) ||
					( FindLink( 3, 1, 2 ) && 
					  m_RCards[0].iNum == 2 && m_RCards[3].iNum == 2 ) ||
					( FindLink( 3, 2, 3 ) && 
					  m_RCards[0].iNum == 2 && m_RCards[1].iNum == 2 ) )
			{
				ccTemp = ctthtwo;
			}

		}
		break;
	case 5:
		{
			// 单顺(5)
			if ( m_RCards[0].rc <= RA &&
				 FindLink( 1, 0, 4 ) )
			{
				ccTemp = clink5;
			}

			// 双顺(5)
			else if ( m_RCards[0].rc <= RA && FindLink( 2, 0, 4 ) )
			{
				ccTemp = ctfive;
			}

			// 三顺(5)
			else if ( m_RCards[0].rc <= RA && FindLink( 3, 0, 4 ) )
			{
				ccTemp = cthfive;
			}
		}
		break;
	case 6:
		{
			// 单顺(6)
			if ( m_RCards[0].rc <= RA && FindLink( 1, 0, 5 ) )
			{
				ccTemp = clink6;
			}

			// 双顺(6)
			else if ( m_RCards[0].rc <= RA && FindLink( 2, 0, 5 ) )
			{
				ccTemp = ctsix;
			}

			// 三顺(6)
			else if ( m_RCards[0].rc <= RA && FindLink( 3, 0, 5 ) )
			{
				ccTemp = cthsix;
			}

			// 三顺带三单
			else if ( m_RCards[0].rc <= RA &&
				    ( FindLink( 3, 0, 2 ) && 
					  m_RCards[3].iNum == 1 && m_RCards[4].iNum == 1 &&
					  m_RCards[5].iNum == 1 ) ||
					( FindLink( 3, 1, 3 ) && 
					  m_RCards[0].iNum == 1 && m_RCards[4].iNum == 1 &&
					  m_RCards[5].iNum == 1 ) ||
					( FindLink( 3, 2, 4 ) && 
					  m_RCards[0].iNum == 1 && m_RCards[1].iNum == 1 &&
					  m_RCards[5].iNum == 1 ) ||
					( FindLink( 3, 3, 5 ) && 
					  m_RCards[0].iNum == 1 && m_RCards[1].iNum == 1 &&
					  m_RCards[2].iNum == 1 ) )
			{
				ccTemp = cththone;
			}

			// 三顺带三对
			else if ( m_RCards[0].rc <= RA &&
				    ( FindLink( 3, 0, 2 ) && 
					  m_RCards[3].iNum == 2 && m_RCards[4].iNum == 2 &&
					  m_RCards[5].iNum == 2 ) ||
					( FindLink( 3, 1, 3 ) && 
					  m_RCards[0].iNum == 2 && m_RCards[4].iNum == 2 &&
					  m_RCards[5].iNum == 2 ) ||
					( FindLink( 3, 2, 4 ) && 
					  m_RCards[0].iNum == 2 && m_RCards[1].iNum == 2 &&
					  m_RCards[5].iNum == 2 ) ||
					( FindLink( 3, 3, 5 ) && 
					  m_RCards[0].iNum == 2 && m_RCards[1].iNum == 2 &&
					  m_RCards[2].iNum == 2 ) )
			{
				ccTemp = cththtwo;
			}
		}
		break;
	case 7:
		{
			// 单顺(7)
			if ( m_RCards[0].rc <= RA && FindLink( 1, 0, 6 ) )
			{
				ccTemp = clink7;
			}

			// 双顺(7)
			else if ( m_RCards[0].rc <= RA && FindLink( 2, 0, 6 ) )
			{
				ccTemp = ctseven;
			}
		}
		break;
	case 8:
		{
			// 单顺(8)
			if ( m_RCards[0].rc <= RA && FindLink( 1, 0, 7 ) )
			{
				ccTemp = clink8;
			}

			// 双顺(8)
			else if ( m_RCards[0].rc <= RA && FindLink( 2, 0, 7 ) )
			{
				ccTemp = ctevent;
			}

			// 三顺带四单
			else if ( m_RCards[0].rc <= RA &&
				    ( FindLink( 3, 0, 3 ) && 
					  m_RCards[4].iNum == 1 && m_RCards[5].iNum == 1 &&
					  m_RCards[6].iNum == 1 && m_RCards[7].iNum == 1 ) ||
					( FindLink( 3, 1, 4 ) && 
					  m_RCards[0].iNum == 1 && m_RCards[5].iNum == 1 &&
					  m_RCards[6].iNum == 1 && m_RCards[7].iNum == 1 ) ||
					( FindLink( 3, 2, 5 ) && 
					  m_RCards[0].iNum == 1 && m_RCards[1].iNum == 1 &&
					  m_RCards[6].iNum == 1 && m_RCards[7].iNum == 1 ) ||
					( FindLink( 3, 3, 6 ) && 
					  m_RCards[0].iNum == 1 && m_RCards[1].iNum == 1 &&
					  m_RCards[2].iNum == 1 && m_RCards[7].iNum == 1 ) ||
				    ( FindLink( 3, 4, 7 ) && 
					  m_RCards[0].iNum == 1 && m_RCards[1].iNum == 1 &&
					  m_RCards[2].iNum == 1 && m_RCards[3].iNum == 1 ) )
			{
				ccTemp = cthfone;
			}

			// 三顺带四对
			else if ( m_RCards[0].rc <= RA &&
				    ( FindLink( 3, 0, 3 ) && 
					  m_RCards[4].iNum == 2 && m_RCards[5].iNum == 2 &&
					  m_RCards[6].iNum == 2 && m_RCards[7].iNum == 2 ) ||
					( FindLink( 3, 1, 4 ) && 
					  m_RCards[0].iNum == 2 && m_RCards[5].iNum == 2 &&
					  m_RCards[6].iNum == 2 && m_RCards[7].iNum == 2 ) ||
					( FindLink( 3, 2, 5 ) && 
					  m_RCards[0].iNum == 2 && m_RCards[1].iNum == 2 &&
					  m_RCards[6].iNum == 2 && m_RCards[7].iNum == 2 ) ||
					( FindLink( 3, 3, 6 ) && 
					  m_RCards[0].iNum == 2 && m_RCards[1].iNum == 2 &&
					  m_RCards[2].iNum == 2 && m_RCards[7].iNum == 2 ) ||
				    ( FindLink( 3, 4, 7 ) && 
					  m_RCards[0].iNum == 2 && m_RCards[1].iNum == 2 &&
					  m_RCards[2].iNum == 2 && m_RCards[3].iNum == 2 ) )
			{
				ccTemp = cthftwo;
			}
		}
		break;
	case 9:
		{
			// 单顺(9)
			if ( m_RCards[0].rc <= RA && FindLink( 1, 0, 8 ) )
			{
				ccTemp = clink9;
			}

			// 双顺(8)
			else if ( m_RCards[0].rc <= RA && FindLink( 2, 0, 8 ) )
			{
				ccTemp = ctnine;
			}
		}
		break;
	case 10:
		{
			// 单顺(10)
			if ( m_RCards[0].rc <= RA && FindLink( 1, 0, 9 ) )
			{
				ccTemp = clink10;
			}

			// 双顺(10)
			else if ( m_RCards[0].rc <= RA && FindLink( 2, 0, 9 ) )
			{
				ccTemp = ctten;
			}
		}
		break;
	case 11:
		{
			// 单顺(11)
			if ( m_RCards[0].rc <= RA && FindLink( 1, 0, 10 ) )
			{
				ccTemp = clink11;
			}
		}
		break;
	case 12:
		{
			// 单顺(12)
			if ( m_RCards[0].rc <= RA && FindLink( 1, 0, 11 ) )
			{
				ccTemp = clink12;
			}
		}
		break;
	default:
		break;
	}

	return ccTemp;
}

//
// 找顺
//
bool Roles::FindLink( const int iOr, const int iF, const int iL )
{
	bool bTemp = true;

	for ( int i = iF; bTemp && i < iL; ++i )
	{
		if ( m_RCards[i].rc - m_RCards[i + 1].rc != 1 ||
			m_RCards[i].iNum != iOr || 
			m_RCards[i + 1].iNum != iOr)
		{
			bTemp = false;
		}
	}
	
	return bTemp;
}


//
// 取出选中牌,保存到m_ivTemp
//
bool Roles::GetChoose( void )
{
	// 清空m_ivTemp
	if ( !m_ivTemp.empty() )
	{
		m_ivTemp.erase( m_ivTemp.begin(), m_ivTemp.end() );
	}

	for ( int i = 0; i < m_Cards.size(); ++i )
	{
		if ( m_Cards[i].cs == yes )						// 选中则被取出
		{
			m_ivTemp.push_back( m_Cards[i].iCard );		// 保存原始数据
		}
	}
	
	if ( m_ivTemp.empty() )
	{
		return false;
	}

	return true;
}

//
// 清空选中的牌
//
void Roles::DeleteChoose( void )
{
	// 清空选中状态
	for ( int i = 0; i < m_Cards.size(); ++i )
	{
		m_Cards[i].cs = no;
	}
}



//
// 删牌
//
void Roles::DeleteCard( void )
{
	for ( int i = 0; i < m_Cards.size(); ++i )
	{
		if ( m_Cards[i].cs == yes )							// 选中的牌则删除
		{
			m_Cards.erase( m_Cards.begin() + i-- );
		}
	}
}

//
// 比较
//
bool Roles::CompareCard( RCVector rcIn, CARDCLASS cc ) const 
{
	bool bTemp = false;

	if ( cc == ctrue )					// 首出
	{
		return true;
	}

	if ( m_ccTemp == chigh )
	{
		return true;					// 2KING
	}

	if ( cc == m_ccTemp )				// 所出牌类型与当前牌相符
	{
		// 判断类型以比较牌面大小
		switch ( cc )
		{
		case cone:									// 单牌
		case ctwo:									// 双牌	
		case cthree:								// 三牌	
		case cfour:									// 炸弹	
		case ctth:									// 三顺
		case cttth:									// 双顺(3)
		case cthth:									// 三顺(3)
		case ctfour:								// 双顺(4)
		case cthfour:								// 三顺(4)
		case clink5:								// 单顺(5)	
		case clink6:								// 单顺(6)	
		case clink7:								// 单顺(7)	
		case clink8:								// 单顺(8)	
		case clink9:								// 单顺(9)	
		case clink10:								// 单顺(10)	
		case clink11:								// 单顺(11)	
		case clink12:								// 单顺(12)	
		case ctfive:								// 双顺(5)	
		case cthfive:								// 三顺(5)	
		case ctsix:									// 双顺(6)	
		case cthsix:								// 三顺(6)
		case ctseven:								// 双顺(7)	
		case ctevent:								// 双顺(8)	
		case ctnine:								// 双顺(9)	
		case ctten:									// 双顺(10)	
			{
				if ( rcIn[0].rc < m_RCards[0].rc )	// 比较最大即可
				{
					bTemp = true;
				}
			}
			break;
		case cthone:								// 三带一单
		case cthtwo:								// 三带一对
		case ctthone:								// 三顺带两单
		case ctthtwo:								// 三顺带两对
		case cththone:								// 三顺带三单
		case cththtwo:								// 三顺带三对
		case cthfone:								// 三顺带四单
		case cthftwo:								// 三顺带四对
			{
				int i = 0;
				while ( i < rcIn.size() )
				{
					if ( rcIn[i].iNum == 3 )		// 最大的三张
					{
						break;
					}
					++i;
				}
				int j = 0;
				while ( j < m_RCards.size() )
				{
					if ( m_RCards[j].iNum == 3 )		// 最大的三张
					{
						break;
					}
					++j;
				}
				if ( rcIn[i].rc < m_RCards[j].rc )
				{
					bTemp = true;
				}
			}
			break;
//		case cfone:									// 四带一单	
//		case cftwo:									// 四带一对
		case cftone:								// 四带两单	
		case cfttwo:								// 四带两对
			{
				int i = 0;
				while ( i < rcIn.size() )
				{
					if ( rcIn[i].iNum == 4 )		// 最大的四张
					{
						break;
					}
					++i;
				}
				int j = 0;
				while ( j < m_RCards.size() )
				{
					if ( m_RCards[j].iNum == 4 )		// 最大的四张
					{
						break;
					}
					++j;
				}
				if ( rcIn[i].rc < m_RCards[j].rc )
				{
					bTemp = true;
				}
			}
			break;
		default:
			break;
		}
	}
	else										// 所出牌类型与当前牌不符
	{
		if ( m_ccTemp == cfour )		// 炸弹
		{
			if ( cc != chigh 					// 不是2KING
//				 cc != cfone &&					// 四带一单	
//				 cc != cftwo &&					// 四带一对	
//				 cc != cftone &&				// 四带两单	
//				 cc != cfttwo					// 四带两对
				)
			{
				bTemp = true;
			}
		}
	}

	return bTemp;
}


//
// public
// ---------------------
// 响应点击，改变牌的选中状态
//
void Roles::ChooseCards( const int iNum )
{
	if ( !m_Cards.empty() && iNum >= 0 && iNum < m_Cards.size() )
	{
		if ( m_Cards[iNum].cs == yes )					// 设置状态为选中
		{
			m_Cards[iNum].cs = no;
		}
		else
		{
			m_Cards[iNum].cs = yes;
		}
	}
}

//
// 出牌
//
bool Roles::PushCard( const RCVector rcIn, const CARDCLASS cc )
{
	if ( !GetChoose() )									// 取选中牌数据
	{
		return false;
	}

	ChangeCard();										// 转换成JQK格式

	// 保存选中牌类型到m_ccTemp
	if ( ( m_ccTemp = SelectClass() ) == cerror )		// 非法牌
	{
		DeleteChoose();									// 清楚选中数据以及状态
		return false;									// 不出牌
	}

	if ( !CompareCard( rcIn, cc ) )						// 比较选中牌与上把牌面大小
	{
		DeleteChoose();
		return false;									// 不出牌
	}

	DeleteCard();										// 出牌后，删除已出牌
	return true;										// 响应出牌
}