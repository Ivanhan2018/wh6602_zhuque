#include "StdAfx.h"
#include "TableFrameSink.h"
#include "TraceService.h"
#include "GameLogic.h"
#include ".\readfixcard.h"//add by Ivan_han 20111213
#include<assert.h>

#define SHOW_BRAINPOWER  0  // 0表示在日志中不显示知能发牌，1表示在日志中显示知能发牌

//静态变量
const WORD			CTableFrameSink::m_wPlayerCount=GAME_PLAYER;			//游戏人数
const enStartMode	CTableFrameSink::m_GameStartMode=enStartMode_FullReady;	//开始模式

//宏定义
#define	GOOD_CARD_PERCENT				4									//好牌概率

//构造函数
CTableFrameSink::CTableFrameSink()
{
	m_pRoomLog = NULL;

	//游戏变量
	m_wBombTime=1;
	m_wBombNums = 0;	//炸弹个数
	m_bLandScore=0;
	m_wFirstUser=rand()%GAME_PLAYER; 
	m_wBankerUser=INVALID_CHAIR;
	m_wCurrentUser=INVALID_CHAIR;
	memset(m_bCircuit,0,sizeof(m_bCircuit));
	memset(m_bScoreInfo,0,sizeof(m_bScoreInfo));
	memset(m_bUserBeiShu,0,sizeof(m_bUserBeiShu));
	memset(m_OneBombNums,0,sizeof(m_OneBombNums));
	m_bSnatchCount=0;

	//结束框信息
	m_IsMissile=false;

	//疯狂变量
	m_bBrightCard=false;
	m_bCallScorePhase=CSD_NORMAL;
	ZeroMemory( m_bGiveUpBanker, sizeof( m_bGiveUpBanker ) );
	ZeroMemory( m_bBrightStart, sizeof( m_bBrightStart ) );
	m_cbBrightCardTime = 1;
	m_cbDoubleCount=0;
	ZeroMemory( m_bDoubleUser, sizeof( m_bDoubleUser ) );
	m_bAllowDoubleScore = false;
	ZeroMemory(m_bFinishDoubleUser, sizeof(m_bFinishDoubleUser) );
	ZeroMemory(m_bUserBrightCard, sizeof(m_bUserBrightCard));
	m_cbBrightCardCount=0;

	//运行信息
	m_bTurnCardCount=0;
	m_wTurnWiner=INVALID_CHAIR;
	memset(m_bTurnCardData,0,sizeof(m_bTurnCardData));
	memset(m_bUserTrustee, 0, sizeof( m_bUserTrustee ) );

	//扑克信息
	memset(m_bBackCard,0,sizeof(m_bBackCard));
	memset(m_bCardCount,0,sizeof(m_bCardCount));
	memset(m_bOutCardCount,0,sizeof(m_bOutCardCount));
	memset(m_bHandCardData,0,sizeof(m_bHandCardData));

	//初始化任务
	//memset(m_UserTaskInfo, 0, sizeof(m_UserTaskInfo));

	//组件变量
	m_pITableFrame=NULL;
	m_pGameServiceOption=NULL;

	m_wOperateUser = INVALID_CHAIR;									//操作用户		Author<cxf>
	ZeroMemory( m_bGiveUpSnatch, sizeof( m_bGiveUpSnatch ) );		//抢地主标识	Author<cxf>
	//奖励宝石

	//服务器打印信息
	//CTraceService::TraceString(TEXT("asdfasdfsadfsdf"), TraceLevel_Normal);
	return;
}

//析构函数
CTableFrameSink::~CTableFrameSink(void)
{
}

//接口查询
void * __cdecl CTableFrameSink::QueryInterface(const IID & Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(ITableFrameSink,Guid,dwQueryVer);
	QUERYINTERFACE(ITableUserAction,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(ITableFrameSink,Guid,dwQueryVer);
	return NULL;
}

// GameSerivce.dll 通向游戏服务端的通道，可以传递任何简易数据或成员指针
bool CTableFrameSink::channel_to_game_service(int iMainCode, int iAssistantCode, void* pData, int iDataLen)
{
	switch(iMainCode)
	{
	case CTGS_MAIN_LOG:	// 日志
		{
			if(CTGS_ASS_LOG_POINT==iAssistantCode)	// 日志组件的指针
			{
				if(pData)
				{
					m_pRoomLog = (cnd_ILogMgr*)pData;

					// 由于每张桌子都会到此，但只能写一次
					static bool bArrow = true;
					if(bArrow && m_pRoomLog)
					{
						bArrow = !bArrow;

						m_pRoomLog->AddMsg(0, 0, TRUE, "***** 这是【斗地主】 游戏服务端日志");
						m_pRoomLog->AddMsg(0, 0, TRUE, "***** 此文件在启动服务时开始记录");
						m_pRoomLog->AddMsg(0, 0, TRUE, "***** 在停止服务或者挂掉时终止");
						m_pRoomLog->AddMsg(0, 0, TRUE, "***** 每条记录前面是记录时间");
						m_pRoomLog->AddMsg(0, 0, TRUE, "***** 下面开始真正有意义的记录");
					}
				}
			}

			break;
		}
	default:
		{
			break;
		}
	}

	return true;
}

//初始化
bool __cdecl CTableFrameSink::InitTableFrameSink(IUnknownEx * pIUnknownEx)
{
	//查询接口
	ASSERT(pIUnknownEx!=NULL);
	m_pITableFrame=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,ITableFrame);
	if (m_pITableFrame==NULL) return false;

	//获取参数
	m_pGameServiceOption=m_pITableFrame->GetGameServiceOption();
	ASSERT(m_pGameServiceOption!=NULL);

	return true;
}
//复位桌子
void __cdecl CTableFrameSink::RepositTableFrameSink()
{
	//游戏变量
	m_wBombTime=1;
	m_wBombNums = 0;
	m_bLandScore=0;
	m_wBankerUser=INVALID_CHAIR;
	m_wCurrentUser=INVALID_CHAIR;
	memset(m_bCircuit,0,sizeof(m_bCircuit));
	memset(m_bScoreInfo,0,sizeof(m_bScoreInfo));
	memset(m_bUserBeiShu,0,sizeof(m_bUserBeiShu));
	m_bSnatchCount=0;

	//疯狂变量
	m_bBrightCard=false;
	m_bCallScorePhase=CSD_NORMAL;
	ZeroMemory( m_bGiveUpBanker, sizeof( m_bGiveUpBanker ) );
	ZeroMemory( m_bBrightStart, sizeof( m_bBrightStart ) );
	m_cbBrightCardTime = 1;
	m_cbDoubleCount=0;
	ZeroMemory( m_bDoubleUser, sizeof( m_bDoubleUser ) );
	ZeroMemory(m_bUserBrightCard, sizeof(m_bUserBrightCard));
	m_cbBrightCardCount=0;
	ZeroMemory(m_bFinishDoubleUser, sizeof(m_bFinishDoubleUser) );

	//运行信息
	m_bTurnCardCount=0;
	m_wTurnWiner=INVALID_CHAIR;
	memset(m_bTurnCardData,0,sizeof(m_bTurnCardData));
	memset(m_bUserTrustee, 0, sizeof( m_bUserTrustee ) );

	//扑克信息
	memset(m_bBackCard,0,sizeof(m_bBackCard));
	memset(m_bCardCount,0,sizeof(m_bCardCount));
	memset(m_bOutCardCount,0,sizeof(m_bOutCardCount));
	memset(m_bHandCardData,0,sizeof(m_bHandCardData));

	m_wOperateUser = INVALID_CHAIR;									//操作用户		Author<cxf>
	ZeroMemory( m_bGiveUpSnatch, sizeof( m_bGiveUpSnatch ) );		//抢地主标识	Author<cxf>
	return;
}

//开始模式
enStartMode __cdecl CTableFrameSink::GetGameStartMode()
{
	return m_GameStartMode;
}

//游戏状态
bool __cdecl CTableFrameSink::IsUserPlaying(WORD wChairID)
{
	return true;
}

//设置固定牌或随机牌或好牌
int CTableFrameSink::SetFixOrRandCardOrGoodCard()
{
	//////////////////////////////////////////////////////////
	//add by Ivan_han 20111213
	CReadFixCard	readFixCard(54, 3);
	BYTE			byTxtFileHead[3];	// 测试文件的头的值，如果 byTxtFileHead[1]==1 时， 发完牌后跳过亮牌至踹牌，直接出牌，测试时用
	memset(byTxtFileHead, 0, 3);
	bool IsFixCard=false;
	int nType=0;
	CString strLclPth, strFlPth;
	GetModuleFileName(NULL, strLclPth.GetBufferSetLength(256), 256);
	strFlPth = strLclPth.Left(strLclPth.ReverseFind('\\')+1)+ TEXT("LandCard.ini");
	if (readFixCard.fnReadFile((LPCTSTR)strFlPth)) 
	{
		readFixCard.fnGetHead(byTxtFileHead);
		nType=byTxtFileHead[1];
		if (1 == byTxtFileHead[1])//取固定牌
		{
			BYTE bCard[54];
			int AllCardNumber=readFixCard.fnGetCards(bCard);  // 从LandCard.ini文件中提取固定的牌
			ASSERT(AllCardNumber==54);
			IsFixCard=true;
			//printf("发固定牌\n");
			//分发扑克
			for ( WORD i = 0; i < m_wPlayerCount; i++ )
			{
				m_bCardCount[ i ] = 17;
				CopyMemory( &m_bHandCardData[ i ], &bCard[17*i], sizeof( BYTE ) * 17 );
			}
			CopyMemory( m_bBackCard, &bCard[ 51 ], sizeof( m_bBackCard ) );
			m_GameLogic.SortCardList( m_bBackCard, 3, ST_ORDER );
		}
		//add by Ivan_han 20120203
		else if (2 == byTxtFileHead[1]||3 == byTxtFileHead[1])//好牌只发给一个人
		{
			this->SetGoodCard1();
		}
		//else if (3 == byTxtFileHead[1])//好牌只发给二个人
		//{
		//     this->SetGoodCard2();

		//}
		else if(4 == byTxtFileHead[1])//综合0,2,3三种发牌策略的发牌策略
		{
			int nret=0; 
			//for(;;)
			//{
			//nret=rand()%4;
			nret=CGameLogic::GetRand(0,5);//1/3的几率发智能牌
			//	if(nret!=1)
			//		break;
			//}
			//if(nret==0)this->SetRandCard();
			if(nret==2||nret==3)
				this->SetGoodCard1();
			else
				this->SetRandCard();
			//if(nret==3)this->SetGoodCard2();
		}
		else
		{
			nType=0;
			//printf("发随机牌\n");
		}
	}
	else
	{
		//printf("读取ini文件失败，发随机牌\n");
	}
	if(nType==0)
	{
		this->SetRandCard();
	}
	return nType;
}

//随机牌
void CTableFrameSink::SetRandCard()
{
	////混乱扑克 
	BYTE bRandCard[ 54 ];
	memset(bRandCard, 0, sizeof(bRandCard));
	m_GameLogic.RandCardList( bRandCard, sizeof( bRandCard ) / sizeof( bRandCard[ 0 ] ) );

	//分发扑克
	for ( WORD i = 0; i < m_wPlayerCount; i++ )
	{
		m_bCardCount[ i ] = 17;
		CopyMemory( &m_bHandCardData[ i ], &bRandCard[ i * m_bCardCount[ i ] ], sizeof( BYTE ) * m_bCardCount[ i ] );
	}
	CopyMemory( m_bBackCard, &bRandCard[ 51 ], sizeof( m_bBackCard ) );
	m_GameLogic.SortCardList( m_bBackCard, 3, ST_ORDER );

#if SHOW_BRAINPOWER
	if(m_pRoomLog)
		m_pRoomLog->AddMsg(0, 0, TRUE, "发随机牌");
#endif
}

//好牌只发给一个人
void CTableFrameSink::SetGoodCard1()
{
	//发送好牌
	vector<BYTE>::iterator   Iter;
	vector<BYTE> the_vector;
	the_vector.clear();

	for( int i = 0; i < 54; i++ )
	{
		the_vector.push_back(CGameLogic::m_cbCardData[i]);
	}


	srand( (unsigned)time( NULL ) );
	vector <BYTE> V[3];
	vector<BYTE> Vback;

	//长顺子花色要打乱
	vector<BYTE> vec;
	int nSize=the_vector.size();
	for(int i=0;i<nSize;i++)
	{
		int Index=CGameLogic::GetRand(0,the_vector.size()-1);
		vec.push_back(the_vector[Index]);
		the_vector.erase(the_vector.begin()+Index);
	}
	assert(nSize==vec.size() && the_vector.size()==0 && "断言失败提示");
	the_vector.swap(vec);
	assert(nSize==the_vector.size() && "断言失败提示");

	vec=the_vector;
	std::sort(vec.begin(),vec.end());

	int nRCount=0;
	for(;;nRCount++)
	{
		int PeekFunIndex=CGameLogic::GetRand(0,CountArray(CGameLogic::PeekFun)-1);
		//int PeekFunIndex=CGameLogic::GetRand(0,13);
		vector <BYTE> v0;
		bool bret=CGameLogic::PeekFun[PeekFunIndex](the_vector,V[0]);
		if(bret)
		{
			break;
		}
	}
	for(int i=0;i<17;i++)
	{
		int Index=CGameLogic::GetRand(0,the_vector.size()-1);
		V[1].push_back(the_vector[Index]);
		the_vector.erase(the_vector.begin()+Index);
	}

	nRCount=0;
	bool bsucback=CGameLogic::PeekRandBackCard(the_vector,Vback);
	V[2].insert(V[2].end(),the_vector.begin(),the_vector.end());
	//V[2].swap(the_vector);

	//assert(V[0].size()==V[1].size()==V[2].size()==17);
	assert(V[0].size()==17);
	assert(V[1].size()==17);
	assert(V[2].size()==17);

	//将3个容器中的扑克分给3个椅子
	for(int i=0;i<3;i++)
		m_bCardCount[i]=17;
	int FirstChairId=CGameLogic::GetRand(0,2);
	int SecondChairId=0;
	for(;;nRCount++)
	{
		SecondChairId=CGameLogic::GetRand(0,2);
		if(SecondChairId!=FirstChairId)
		{
			break;
		}
	}
	int LastChairId=3-FirstChairId-SecondChairId;

	memcpy(&m_bHandCardData[FirstChairId][0],&V[0][0],17);
	memcpy(&m_bHandCardData[SecondChairId][0],&V[1][0],17);
	memcpy(&m_bHandCardData[LastChairId][0],&V[2][0],17);
	memcpy(&m_bBackCard[0],&Vback[0],3);
	m_GameLogic.SortCardList( m_bBackCard, 3, ST_ORDER );

	#if SHOW_BRAINPOWER
		if(m_pRoomLog)
			m_pRoomLog->AddMsg(0, 0, TRUE, "发智能牌");
	#endif

	//按照牌的顺序检查一下时否有多出的牌、缺少的牌
	vector<BYTE> vec1;
	vec1.insert(vec1.end(),V[0].begin(),V[0].end());
	vec1.insert(vec1.end(),V[1].begin(),V[1].end());
	vec1.insert(vec1.end(),V[2].begin(),V[2].end());
	vec1.insert(vec1.end(),Vback.begin(),Vback.end());
	std::sort(vec1.begin(),vec1.end());
	bool b=equal(vec.begin(),vec.end(), vec1.begin());
	char sz[100]={0};
	if(b)
	{
		sprintf(sz,"vec==vec1,FirstChairId=%d",FirstChairId);
	}
	else
	{
		sprintf(sz,"vec!=vec1,FirstChairId=%d",FirstChairId);
	}
	#if SHOW_BRAINPOWER
		if(m_pRoomLog)
			m_pRoomLog->AddMsg(0, 0, TRUE, sz);
	#endif

	for(int j=0;j<3;j++)
		for( int i = 0; i < 17; i++ )
		{
			sprintf(sz,"V[%d][%d]=%d(%s)",j,i,V[j][i],CGameLogic::GetCardName(V[j][i]));

			#if SHOW_BRAINPOWER
				if(m_pRoomLog)
					m_pRoomLog->AddMsg(0, 0, TRUE, sz);
			#endif
		}
		for( int i = 0; i < 3; i++ )
		{
			sprintf(sz,"Vback[%d]=%d(%s)",i,Vback[i],CGameLogic::GetCardName(Vback[i]));

			#if SHOW_BRAINPOWER
				if(m_pRoomLog)
					m_pRoomLog->AddMsg(0, 0, TRUE, sz);
			#endif
		}
}

////好牌只发给二个人
//void CTableFrameSink::SetGoodCard2()
//{
//	//发送好牌
//	vector<BYTE>::iterator   Iter;
//	vector<BYTE> the_vector;
//	the_vector.clear();
//
//	for( int i = 0; i < 54; i++ )
//	{
//		the_vector.push_back(CGameLogic::m_cbCardData[i]);
//	}
//
//
//	srand( (unsigned)time( NULL ) );
//	vector <BYTE> V[3];
//	vector<BYTE> Vback;
//
//	int nRCount=0;
//	//int nMaxRcount=10000;
//	int nMaxRcount=10;
//	for(int i=0;i<2;i++)
//	{
//		nRCount=0;
//		for(;;nRCount++)
//		{
//			int PeekFunIndex=CGameLogic::GetRand(0,1);
//			//vector <BYTE> v0;
//			bool bret=CGameLogic::PeekFun[PeekFunIndex](the_vector,V[i]);
//			if(bret)
//			{
//				break;
//			}
//			if(nRCount>=nMaxRcount)
//			{
//				V[0].clear();
//				V[1].clear();
//				the_vector.clear();
//				for( int j = 0; j < 54; j++ )
//				{
//					the_vector.push_back(CGameLogic::m_cbCardData[j]);
//				}
//				i=0;
//				nRCount=0;
//				break;//break;
//			}
//		}
//	}
//
//	nRCount=0;
//	bool bsucback=CGameLogic::PeekRandBackCard(the_vector,Vback);
//	V[2].insert(V[2].end(),the_vector.begin(),the_vector.end());
//	//V[2].swap(the_vector);
//
//	//assert(V[0].size()==V[1].size()==V[2].size()==17);
//	assert(V[0].size()==17);
//	assert(V[1].size()==17);
//	assert(V[2].size()==17);
//
//	//将3个容器中的扑克分给3个椅子
//	for(int i=0;i<3;i++)
//		m_bCardCount[i]=17;
//	int FirstChairId=CGameLogic::GetRand(0,2);
//	int SecondChairId=0;
//	for(;;nRCount++)
//	{
//		SecondChairId=CGameLogic::GetRand(0,2);
//		if(SecondChairId!=FirstChairId)
//		{
//			break;
//		}
//	}
//	int LastChairId=3-FirstChairId-SecondChairId;
//
//	memcpy(&m_bHandCardData[FirstChairId][0],&V[0][0],17);
//	memcpy(&m_bHandCardData[SecondChairId][0],&V[1][0],17);
//	memcpy(&m_bHandCardData[LastChairId][0],&V[2][0],17);
//	memcpy(&m_bBackCard[0],&Vback[0],3);
//	m_GameLogic.SortCardList( m_bBackCard, 3, ST_ORDER );
//}

////设置固定牌或随机牌
//bool CTableFrameSink::SetFixOrRandCard()
//{
//	//////////////////////////////////////////////////////////
//	//add by Ivan_han 20111213
//	CReadFixCard	readFixCard(54, 3);
//	BYTE			byTxtFileHead[3];	// 测试文件的头的值，如果 byTxtFileHead[1]==1 时， 发完牌后跳过亮牌至踹牌，直接出牌，测试时用
//	memset(byTxtFileHead, 0, 3);
//	bool IsFixCard=false;
//	CString strLclPth, strFlPth;
//	GetModuleFileName(NULL, strLclPth.GetBufferSetLength(256), 256);
//	strFlPth = strLclPth.Left(strLclPth.ReverseFind('\\')+1)+ TEXT("LandCard.ini");
//	if (readFixCard.fnReadFile((LPCTSTR)strFlPth)) 
//	{
//		readFixCard.fnGetHead(byTxtFileHead);
//		if (1 == byTxtFileHead[1])
//		{
//			BYTE bCard[54];
//			int AllCardNumber=readFixCard.fnGetCards(bCard);  // 从LandCard.ini文件中提取固定的牌
//			ASSERT(AllCardNumber==54);
//			IsFixCard=true;
//			//printf("发固定牌\n");
//			//分发扑克
//			for ( WORD i = 0; i < m_wPlayerCount; i++ )
//			{
//				m_bCardCount[ i ] = 17;
//				CopyMemory( &m_bHandCardData[ i ], &bCard[17*i], sizeof( BYTE ) * 17 );
//			}
//			CopyMemory( m_bBackCard, &bCard[ 51 ], sizeof( m_bBackCard ) );
//			m_GameLogic.SortCardList( m_bBackCard, 3, ST_ORDER );
//		}
//		else
//		{
//			//printf("发随机牌\n");
//		}
//	}
//	else
//	{
//		//printf("读取ini文件失败，发随机牌\n");
//	}
//	if(!IsFixCard)
//	{
//		////混乱扑克 
//		BYTE bRandCard[ 54 ];
//		memset(bRandCard, 0, sizeof(bRandCard));
//		m_GameLogic.RandCardList( bRandCard, sizeof( bRandCard ) / sizeof( bRandCard[ 0 ] ) );
//
//		//分发扑克
//		for ( WORD i = 0; i < m_wPlayerCount; i++ )
//		{
//			m_bCardCount[ i ] = 17;
//			CopyMemory( &m_bHandCardData[ i ], &bRandCard[ i * m_bCardCount[ i ] ], sizeof( BYTE ) * m_bCardCount[ i ] );
//		}
//		CopyMemory( m_bBackCard, &bRandCard[ 51 ], sizeof( m_bBackCard ) );
//		m_GameLogic.SortCardList( m_bBackCard, 3, ST_ORDER );
//	}
//	return IsFixCard;
//}

//游戏开始
bool __cdecl CTableFrameSink::OnEventGameStart()
{
	//osc add
	memset(m_tagTask, 0, sizeof(m_tagTask));

	//在一局开始之前，初始化任务信息
	//for (int i=0; i<GAME_PLAYER; i++)
	//{
	//	m_UserTaskInfo[i].bIsOutBombWin = false;	//是否打出炸弹赢
	//	m_UserTaskInfo[i].bIsOutMissile = false;	//是否打出火箭赢
	//	m_UserTaskInfo[i].bRegBombNums = 0;			//一局当中打出的炸弹个数
	//}
	//
	//疯狂变量
	ZeroMemory( m_bGiveUpBanker, sizeof( m_bGiveUpBanker ) );
	ZeroMemory( m_bGiveUpSnatch, sizeof( m_bGiveUpSnatch ) );		//抢地主标识	Author<cxf>
	memset(m_bUserTrustee, 0, sizeof( m_bUserTrustee ) );

	//设置状态
	m_pITableFrame->SetGameStatus(GS_WK_SCORE);//设置叫分状态

	/* Del:<cxf>; description:<删除废弃代码>
	BYTE cbSendGoodCard = rand () % GOOD_CARD_PERCENT ;
	BYTE cbAndroidUserCount = 0 ;
	WORD wAndroidUser[ GAME_PLAYER ] = {0};*/

	//游戏开始，叫分为零
	m_bLandScore = 0;
	memset(m_bCircuit,0,sizeof(m_bCircuit));
	m_bSnatchCount=0;

	//add by Ivan_han
	m_vecChairIDOffLine.clear();
	this->SetFixOrRandCardOrGoodCard();

	//设置用户
	if ( m_wCurrentUser == INVALID_CHAIR ) 
	{
		m_wCurrentUser = m_wFirstUser;
	}
	//操作用户
	m_wOperateUser = m_wCurrentUser;					//Author<cxf>
	//发送扑克
	CMD_S_SendAllCard SendAllCard ;
	SendAllCard.wCurrentUser = m_wCurrentUser ;

	//亮牌信息
	CopyMemory(SendAllCard.bUserBrightCard, m_bUserBrightCard, sizeof(SendAllCard.bUserBrightCard));

	//手上扑克
	CopyMemory( SendAllCard.bCardData, m_bHandCardData, sizeof( m_bHandCardData ) ) ;
	//底牌扑克
	CopyMemory( SendAllCard.bBackCardData, m_bBackCard, 3 ) ;
	for ( WORD i = 0; i < m_wPlayerCount; i++ )
	{
		m_pITableFrame->SendTableData( i, SUB_S_SEND_CARD, &SendAllCard, sizeof( SendAllCard ) );
		m_pITableFrame->SendLookonData( i, SUB_S_SEND_CARD, &SendAllCard, sizeof( SendAllCard ) );
	}

	//排列扑克
	for ( WORD i = 0; i < m_wPlayerCount; i++ )
	{
		m_GameLogic.SortCardList( m_bHandCardData[ i ], m_bCardCount[ i ], ST_ORDER );
	}

	return true;
}

//add by Ivan_han
//用户断线
bool __cdecl CTableFrameSink::OnActionUserOffLine(WORD wChairID, IServerUserItem * pIServerUserItem)
{ 
	if(pIServerUserItem==NULL)
		return false;
	m_vecChairIDOffLine.push_back(wChairID);
	//如果有3个椅子断线，则游戏结束
	if(m_vecChairIDOffLine.size()==3)
	{
		m_pITableFrame->DismissGame();
		return true;
	}
	if(m_wCurrentUser==wChairID)
	{
		int a=0;
		this->OnAutoPassOrOutCard();
	}

	//if (m_pITableFrame->GetGameStatus()==GS_WK_SCORE)
	//{
	//	if (m_wCurrentUser==wChairID)
	//	{
	//		this->OnUserLandScore(wChairID,0);
	//	}
	//}
	//else if (m_pITableFrame->GetGameStatus()==GS_WK_SNATCH)
	//{
	//	if (m_wCurrentUser==wChairID)
	//	{
	//		this->OnUserLandScore(wChairID,1);
	//	}
	//}
	//else if (m_pITableFrame->GetGameStatus()==GS_WK_PLAYING)
	//{
	//	if(m_wCurrentUser==wChairID)
	//		this->OnAutoPassOrOutCard();
	//}

	////判断其他两个椅子是否出了牌，如果都没出牌，则断线玩家必须出一张牌
	//if(m_wCurrentUser==wChairID && m_wCurrentUser!=INVALID_CHAIR)
	//{
	//	if(m_wCurrentUser!=m_wTurnWiner)
	//	{	
	//	    bool bFlag=this->OnUserPassCard(wChairID);
	//	}
	//	else
	//	{
	//		
	//		BYTE bCardData[1]={0};
	//		BYTE bCardCount=1;
	//           m_GameLogic.SortCardList(m_bHandCardData[wChairID],m_bCardCount[wChairID],ST_ORDER);
	//           bCardData[0]=m_bHandCardData[wChairID][0];
	//		m_GameLogic.RemoveCard(bCardData,bCardCount,m_bHandCardData[wChairID],m_bCardCount[wChairID]);
	//		this->OnUserOutCard(wChairID,bCardData,bCardCount);
	//	}
	//}


	/*
	//游戏玩家
	for ( WORD wUserIdx = 0; wUserIdx < GAME_PLAYER; ++wUserIdx )
	{
	IServerUserItem *pIServerUserItem= m_pITableFrame->GetServerUserItem( wUserIdx );
	tagServerUserData * pUserData=pIServerUserItem->GetUserData();
	//if (pUserData->cbUserStatus!=US_PLAY)
	if(pUserData!=NULL)
	{
	//用户状态定义
	static char *szUserStatus[7]={"US_NULL没有状态","US_FREE站立状态","US_SIT坐下状态","US_READY同意状态","US_LOOKON旁观状态","US_PLAY游戏状态","US_OFFLINE断线状态"};
	//服务器打印数据
	char chData[128]={0};
	sprintf(chData, "wUserIdx=%d,%s的用户状态=%d(%s)", wUserIdx,pUserData->szNickName,pUserData->cbUserStatus,szUserStatus[pUserData->cbUserStatus]);
	}
	}
	*/
	return true; 

}

//add by Ivan_han
//用户重入
bool __cdecl CTableFrameSink::OnActionUserReConnect(WORD wChairID, IServerUserItem * pIServerUserItem)
{
	if(pIServerUserItem==NULL)
		return false;
	m_vecChairIDOffLine.erase(remove(m_vecChairIDOffLine.begin(),m_vecChairIDOffLine.end(), wChairID),m_vecChairIDOffLine.end());

	//CTraceService::TraceString("用户重入OnActionUserReConnect", TraceLevel_Normal);
	////游戏玩家
	//for ( WORD wUserIdx = 0; wUserIdx < GAME_PLAYER; ++wUserIdx )
	//{
	//	IServerUserItem *pIServerUserItem= m_pITableFrame->GetServerUserItem( wUserIdx );
	//	tagServerUserData * pUserData=pIServerUserItem->GetUserData();
	//	//if (pUserData->cbUserStatus!=US_PLAY)
	//	if(pUserData!=NULL)
	//	{
	//		//用户状态定义
	//		static char *szUserStatus[7]={"US_NULL没有状态","US_FREE站立状态","US_SIT坐下状态","US_READY同意状态","US_LOOKON旁观状态","US_PLAY游戏状态","US_OFFLINE断线状态"};
	//		//服务器打印数据
	//		char chData[128]={0};
	//		sprintf(chData, "wUserIdx=%d,%s的用户状态=%d(%s)", wUserIdx,pUserData->szNickName,pUserData->cbUserStatus,szUserStatus[pUserData->cbUserStatus]);
	//		CTraceService::TraceString(chData, TraceLevel_Normal);
	//	}
	//}
	return true; 
}

//游戏常规结束
bool __cdecl CTableFrameSink::OnEventGameNormalEnd(WORD wChairID,bool bLandWin/*, IServerUserItem * pIServerUserItem, BYTE cbReason*/)
{
	//定义变量
	CMD_S_GameEnd GameEnd;
	memset(&GameEnd,0,sizeof(GameEnd));

	//剩余扑克，设置bHandCardData字段和bCardData字段的值
	BYTE bCardPos=0;
	for (WORD i=0;i<m_wPlayerCount;i++)
	{
		CopyMemory(&GameEnd.bHandCardData[i],m_bHandCardData[i],m_bCardCount[i]*sizeof(BYTE));
		GameEnd.bCardCount[i]=m_bCardCount[i];
		CopyMemory(&GameEnd.bCardData[bCardPos],m_bHandCardData[i],m_bCardCount[i]*sizeof(BYTE));
		bCardPos+=m_bCardCount[i];
	}

	////地主没牌了表示是地主赢了
	//bool bLandWin=(m_bCardCount[m_wBankerUser]==0)?true:false;



	//春天判断，设置IsChunTian字段和IsFanChun字段的值，计算玩家倍数m_bUserBeiShu
	bool bchuntian=false;
	bool bfanchun=false;
	if ( wChairID == m_wBankerUser )//只是地主出了牌（春天）
	{
		WORD wUser1 = ( m_wBankerUser + 1 ) % GAME_PLAYER;
		WORD wUser2 = ( m_wBankerUser + 2 ) % GAME_PLAYER;
		if ( ( m_bOutCardCount[ wUser1 ] == 0 ) && ( m_bOutCardCount[ wUser2 ] == 0 ) ) 
		{
			//cxf del 12-01-04 wDrawTimes *= 2;
			bchuntian=true;
			//GameEnd.nSpringTime*=2;
			for (int i=0;i<GAME_PLAYER;++i)
			{
				m_bUserBeiShu[i]*=2;
			}

		}
		GameEnd.IsChunTian=bchuntian;
	}
	else//地主只出了一次牌（反春）
	{
		if ( m_bOutCardCount[ m_wBankerUser ] == 1 ) 
		{
			//cxf del 12-01-04 wDrawTimes *= 2;
			bfanchun=true;
			//GameEnd.nSpringTime*=2;
			for (int i=0;i<GAME_PLAYER;++i)
			{
				m_bUserBeiShu[i]*=2;
			}
		}
		GameEnd.IsFanChun=bfanchun;
	}

	//基本积分=地主倍数*单位积分
	LONGLONG lBaseScore= m_bUserBeiShu[m_wBankerUser]*(WORD)m_pGameServiceOption->lCellScore;

	//计算积分，设置lGameScore字段的值
	for (int i=0; i<GAME_PLAYER; ++i)
	{
		if (i==m_wBankerUser)//地主赢输基本积分的双倍
		{
			GameEnd.lGameScore[i]+=(lBaseScore*((bLandWin==true)?2:-2));
		}
		else//农民赢输基本积分
		{
			GameEnd.lGameScore[i]+=(lBaseScore*((bLandWin==true)?-1:1));
		}
	}

	LONG lRestrictScore = m_pGameServiceOption->lRestrictScore; //最高限制分数
	LONG lBankerScore = GameEnd.lGameScore[m_wBankerUser]; //地主分数

	//输分限制，修正lGameScore字段的值
	if (lBankerScore > lRestrictScore*2)//地主赢的上限，农民输的下限
	{
		GameEnd.lGameScore[m_wBankerUser] = lRestrictScore*2;
		GameEnd.lGameScore[(m_wBankerUser+1)%m_wPlayerCount] = -lRestrictScore;
		GameEnd.lGameScore[(m_wBankerUser+2)%m_wPlayerCount] = -lRestrictScore;
	}
	else if (lBankerScore < -lRestrictScore)//地主输的下限，农民赢的上限
	{
		GameEnd.lGameScore[m_wBankerUser] = -lRestrictScore;
		GameEnd.lGameScore[(m_wBankerUser+1)%m_wPlayerCount] = lRestrictScore/2;
		GameEnd.lGameScore[(m_wBankerUser+2)%m_wPlayerCount] = lRestrictScore/2;
	}



	//统计积分
	LONGLONG lScore=0;
	LONGLONG lRevenue=0;
	enScoreKind ScoreKind;
	for (WORD i=0;i<m_wPlayerCount;i++)
	{
		//计算税收（服务费），设置lGameTax字段的值，修正lGameScore字段的值
		lRevenue=0;
		if ( GameEnd.lGameScore[i]>0L)//赢家扣除服务费
		{
			lRevenue= LONGLONG((GameEnd.lGameScore[i]*(m_pGameServiceOption->wRevenue/100.0)));
			//GameEnd.lGameTax+=lRevenue;
			GameEnd.lGameTax[i]=lRevenue;
			GameEnd.lGameScore[i]=GameEnd.lGameScore[i]-lRevenue;
		}
		else
		{
			GameEnd.lGameTax[i]=0;
		}
		lScore = GameEnd.lGameScore[i];
		ScoreKind = lScore > 0 ? enScoreKind_Win : enScoreKind_Lost;

		//任务判断，设置任务标识位
		if(GameEnd.lGameScore[i]>0)
		{//获胜
			m_tagTask[i] |=CT_TASK_WIN;
		}

		//打出12以上倍并出完牌；
		if (m_bUserBeiShu[i]>=12)
		{
			m_tagTask[i] |=CT_TASK_BEI_12;
		}

		//打出24倍以上并出完牌；
		if (m_bUserBeiShu[i]>=24)
		{
			m_tagTask[i] |=CT_TASK_BEI_24;
		}

		//打出48倍以上并出完牌；
		if (m_bUserBeiShu[i]>=48)
		{
			m_tagTask[i] |=CT_TASK_BEI_48;
		}

		//打出96倍以上并出完牌；
		if (m_bUserBeiShu[i]>=96)
		{
			m_tagTask[i] |=CT_TASK_BEI_96;
		}
		//手牌为零；
		if (0==m_bCardCount[i])
		{
			m_tagTask[i] |=CT_TASK_CARD_ZERO;
		}
		//设置奖励宝石iLargessCount字段
		GameEnd.iLargessCount[i] = OnUserTaskJudgement(i,m_wBankerUser);
		//将最终计算的游戏积分lGameScore字段的值，服务费，分数类型（输赢情况）写到数据库
		m_pITableFrame->WriteUserScore(i, lScore, lRevenue, ScoreKind );	
	}
	//结算框信息，设置游戏倍数GameBeiShu字段和房间积分RoomScore字段的值
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		GameEnd.GameBeiShu[i]=m_bUserBeiShu[i];
		GameEnd.RoomScore[i]=(m_pGameServiceOption->lCellScore)*m_bLandScore;
	}
	//发送信息
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));

	//切换用户
	m_wFirstUser=wChairID;

	//结束游戏
	m_pITableFrame->ConcludeGame();

	memset(m_bUserTrustee, 0, sizeof( m_bUserTrustee ) );

	return true;
}

//游戏结束 
bool __cdecl CTableFrameSink::OnEventGameEnd( WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason )
{
	switch ( cbReason )
	{  
	case GER_DISMISS:		//游戏解散
		{
			//效验参数
			//ASSERT( pIServerUserItem != NULL );//by Ivan_han
			//ASSERT( wChairID < m_wPlayerCount );//by Ivan_han

			//构造数据
			CMD_S_GameEnd GameEnd;
			memset( &GameEnd, 0, sizeof( GameEnd ) );

			//剩余扑克
			BYTE bCardPos = 0;
			for ( WORD i = 0; i < m_wPlayerCount; i++ )
			{
				CopyMemory(&GameEnd.bHandCardData[i],m_bHandCardData[i],m_bCardCount[i]*sizeof(BYTE));
				GameEnd.bCardCount[ i ] = m_bCardCount[ i ];
				CopyMemory( &GameEnd.bCardData[ bCardPos ], m_bHandCardData[ i ], m_bCardCount[ i ] * sizeof( BYTE ) );
				bCardPos += m_bCardCount[ i ];
			}                     

			//初始化任务
			//memset(m_UserTaskInfo, 0, sizeof(m_UserTaskInfo));
			memset(m_bUserTrustee, 0, sizeof( m_bUserTrustee ) );

			//发送信息
			m_pITableFrame->SendTableData( INVALID_CHAIR, SUB_S_GAME_END, &GameEnd, sizeof( GameEnd ) );
			m_pITableFrame->SendLookonData( INVALID_CHAIR, SUB_S_GAME_END, &GameEnd, sizeof( GameEnd ) );

			//结束游戏
			m_pITableFrame->ConcludeGame();

			return true;
		}
	case GER_NORMAL:		//常规结束
		{
			//if(m_vecChairIDOffLine.size()==0)
			//	printf("m_vecChairIDOffLine.size()==0\n");
			//else if(m_vecChairIDOffLine.size()==1||m_vecChairIDOffLine.size()==2)
			//	//printf("%d\n",m_vecChairIDOffLine.back());
			//	printf("%d\n",m_vecChairIDOffLine[0]);
			//else if(m_vecChairIDOffLine.size()>2)
			//	printf("m_vecChairIDOffLine.size()>2\n");
			//char sz[100]={0};
			//sprintf(sz,"m_vecChairIDOffLine.size()=%d",m_vecChairIDOffLine.size());
			//CTraceService::TraceString(sz, TraceLevel_Normal);

			//add by Ivan_han
			//地主没牌了表示是地主赢了
			bool bLandWin=(m_bCardCount[m_wBankerUser]==0)?true:false;
			if(m_vecChairIDOffLine.size()==0)
				return this->OnEventGameNormalEnd(wChairID,bLandWin);
			if(m_vecChairIDOffLine.size()==1||m_vecChairIDOffLine.size()==2)
			{
				//第一个断线的玩家     
				WORD wChairIDOffLine=m_vecChairIDOffLine[0];
				//地主断线，判农民胜利，照正常结算
				if(wChairIDOffLine==m_wBankerUser)
					return this->OnEventGameNormalEnd(wChairID,false);

				//未断线农民
				WORD wChairIDOnLine=3-wChairIDOffLine-m_wBankerUser;
				CMD_S_GameEnd GameEnd;
				memset(&GameEnd,0,sizeof(GameEnd));

				//剩余扑克，设置bHandCardData字段和bCardData字段的值
				BYTE bCardPos=0;
				for (WORD i=0;i<m_wPlayerCount;i++)
				{
					CopyMemory(&GameEnd.bHandCardData[i],m_bHandCardData[i],m_bCardCount[i]*sizeof(BYTE));
					GameEnd.bCardCount[i]=m_bCardCount[i];
					CopyMemory(&GameEnd.bCardData[bCardPos],m_bHandCardData[i],m_bCardCount[i]*sizeof(BYTE));
					bCardPos+=m_bCardCount[i];
				}

				//春天判断，设置IsChunTian字段和IsFanChun字段的值，计算玩家倍数m_bUserBeiShu
				bool bchuntian=false;
				bool bfanchun=false;
				if ( wChairID == m_wBankerUser )//只是地主出了牌（春天）
				{
					WORD wUser1 = ( m_wBankerUser + 1 ) % GAME_PLAYER;
					WORD wUser2 = ( m_wBankerUser + 2 ) % GAME_PLAYER;
					if ( ( m_bOutCardCount[ wUser1 ] == 0 ) && ( m_bOutCardCount[ wUser2 ] == 0 ) ) 
					{
						//cxf del 12-01-04 wDrawTimes *= 2;
						bchuntian=true;
						//GameEnd.nSpringTime*=2;
						for (int i=0;i<GAME_PLAYER;++i)
						{
							m_bUserBeiShu[i]*=2;
						}

					}
					GameEnd.IsChunTian=bchuntian;
				}
				else//地主只出了一次牌（反春）
				{
					if ( m_bOutCardCount[ m_wBankerUser ] == 1 ) 
					{
						//cxf del 12-01-04 wDrawTimes *= 2;
						bfanchun=true;
						//GameEnd.nSpringTime*=2;
						for (int i=0;i<GAME_PLAYER;++i)
						{
							m_bUserBeiShu[i]*=2;
						}
					}
					GameEnd.IsFanChun=bfanchun;
				}

				//基本积分=地主倍数*单位积分
				LONGLONG lBaseScore= m_bUserBeiShu[m_wBankerUser]*(WORD)m_pGameServiceOption->lCellScore;
				//计算积分，设置lGameScore字段的值
				//农民断线，地主赢
				if(wChairIDOffLine!=m_wBankerUser && bLandWin)
				{
					GameEnd.lGameScore[m_wBankerUser]+=(lBaseScore*2);
					GameEnd.lGameScore[wChairIDOffLine]-=(lBaseScore*2);
				}
				//农民断线，农民赢
				if(wChairIDOffLine!=m_wBankerUser && !bLandWin)
				{
					GameEnd.lGameScore[wChairIDOnLine]+=(lBaseScore);
					GameEnd.lGameScore[wChairIDOffLine]-=(lBaseScore);
				}
				//if(GameEnd.lGameScore[wChairIDOffLine]<0)
				//	GameEnd.lGameScore[wChairIDOffLine]=0;
				//暂时不考虑输分限制，暂时不修正lGameScore字段的值

				//统计积分
				LONGLONG lScore=0;
				LONGLONG lRevenue=0;
				enScoreKind ScoreKind;
				for (WORD i=0;i<m_wPlayerCount;i++)
				{
					//计算税收（服务费），设置lGameTax字段的值，修正lGameScore字段的值
					lRevenue=0;
					if ( GameEnd.lGameScore[i]>0L)//赢家扣除服务费
					{
						lRevenue= LONGLONG((GameEnd.lGameScore[i]*(m_pGameServiceOption->wRevenue/100.0)));
						//GameEnd.lGameTax+=lRevenue;
						GameEnd.lGameTax[i]=lRevenue;
						GameEnd.lGameScore[i]=GameEnd.lGameScore[i]-lRevenue;
					}
					else
					{
						GameEnd.lGameTax[i]=0;
					}
					lScore = GameEnd.lGameScore[i];
					ScoreKind = lScore > 0 ? enScoreKind_Win : enScoreKind_Lost;

					//任务判断，设置任务标识位
					if(GameEnd.lGameScore[i]>0)
					{//获胜
						m_tagTask[i] |=CT_TASK_WIN;
					}

					//打出12以上倍并出完牌；
					if (m_bUserBeiShu[i]>=12)
					{
						m_tagTask[i] |=CT_TASK_BEI_12;
					}

					//打出24倍以上并出完牌；
					if (m_bUserBeiShu[i]>=24)
					{
						m_tagTask[i] |=CT_TASK_BEI_24;
					}

					//打出48倍以上并出完牌；
					if (m_bUserBeiShu[i]>=48)
					{
						m_tagTask[i] |=CT_TASK_BEI_48;
					}

					//打出96倍以上并出完牌；
					if (m_bUserBeiShu[i]>=96)
					{
						m_tagTask[i] |=CT_TASK_BEI_96;
					}
					//手牌为零；
					if (0==m_bCardCount[i])
					{
						m_tagTask[i] |=CT_TASK_CARD_ZERO;
					}
					//设置奖励宝石iLargessCount字段
					GameEnd.iLargessCount[i] = OnUserTaskJudgement(i,m_wBankerUser);
					//将最终计算的游戏积分lGameScore字段的值，服务费，分数类型（输赢情况）写到数据库
					m_pITableFrame->WriteUserScore(i, lScore, lRevenue, ScoreKind );	
				}
				//结算框信息，设置游戏倍数GameBeiShu字段和房间积分RoomScore字段的值
				for (WORD i=0;i<GAME_PLAYER;i++)
				{
					GameEnd.GameBeiShu[i]=m_bUserBeiShu[i];
					GameEnd.RoomScore[i]=(m_pGameServiceOption->lCellScore)*m_bLandScore;
				}
				//发送信息
				m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
				m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));

				//切换用户
				m_wFirstUser=wChairID;

				//结束游戏
				m_pITableFrame->ConcludeGame();

				memset(m_bUserTrustee, 0, sizeof( m_bUserTrustee ) );

				return true;
			}
			ASSERT(FALSE);
			return true;
		}
	case GER_USER_LEFT:		//用户强退
		{
			//效验参数
			ASSERT(pIServerUserItem!=NULL);
			ASSERT(wChairID<m_wPlayerCount);

			//构造数据
			CMD_S_GameEnd GameEnd;
			memset(&GameEnd,0,sizeof(GameEnd));

			//cxf 计算最高限制
			LONG lRestrictScore = m_pGameServiceOption->lRestrictScore; //最高限制分数
			//强退用户输分以最高限制
			GameEnd.lGameScore[wChairID] = -lRestrictScore;//强退用户
			GameEnd.lGameScore[(wChairID+1)%GAME_PLAYER] = abs(lRestrictScore/2);//玩家赢得强退玩家分数/2
			GameEnd.lGameScore[(wChairID+2)%GAME_PLAYER] = abs(lRestrictScore/2);//玩家赢得强退玩家分数/2
			GameEnd.IsUserLeft[wChairID]=true;
			GameEnd.IsUserLeft[(wChairID+1)%GAME_PLAYER]=false;
			GameEnd.IsUserLeft[(wChairID+2)%GAME_PLAYER]=false;
			//cxf end

			//统计积分
			LONGLONG lScore=0;
			LONGLONG lRevenue=0;
			enScoreKind ScoreKind;
			for (WORD i=0;i<m_wPlayerCount;i++)
			{
				//计算税收
				lRevenue=0;
				if ( GameEnd.lGameScore[i]>0L)
				{
					lRevenue=LONGLONG((GameEnd.lGameScore[i]*(m_pGameServiceOption->wRevenue/100.0)));
					GameEnd.lGameTax[i]=lRevenue;
					GameEnd.lGameScore[i]=GameEnd.lGameScore[i]-lRevenue;
				}
				else
				{
					GameEnd.lGameTax[i]=0;
				}
				lScore = GameEnd.lGameScore[i];
				ScoreKind = lScore > 0 ? enScoreKind_Win : enScoreKind_Flee;
				if (i!=wChairID) 
				{
					m_pITableFrame->WriteUserScore(i, lScore, lRevenue, ScoreKind );
				}

			}

			//剩余扑克
			BYTE bCardPos=0;
			for (WORD i=0;i<m_wPlayerCount;i++)
			{
				CopyMemory(&GameEnd.bHandCardData[i],m_bHandCardData[i],m_bCardCount[i]*sizeof(BYTE));
				GameEnd.bCardCount[i]=m_bCardCount[i];
				CopyMemory(&GameEnd.bCardData[bCardPos],m_bHandCardData[i],m_bCardCount[i]*sizeof(BYTE));
				bCardPos+=m_bCardCount[i];
			}
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				GameEnd.GameBeiShu[i]=m_bUserBeiShu[i];
				GameEnd.RoomScore[i]=(m_pGameServiceOption->lCellScore)*m_bLandScore;
			}


			//发送信息
			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));

			//修改积分
			m_pITableFrame->WriteUserScore(wChairID,GameEnd.lGameScore[wChairID],0,enScoreKind_Draw);

			//结束游戏
			m_pITableFrame->ConcludeGame();
			memset(m_bUserTrustee, 0, sizeof( m_bUserTrustee ) );
			return true;
		}
	}

	ASSERT(FALSE);

	return false;
}

//发送场景
bool __cdecl CTableFrameSink::SendGameScene(WORD wChiarID, IServerUserItem * pIServerUserItem, BYTE cbGameStatus, bool bSendSecret)
{
	switch (cbGameStatus)
	{
	case GS_WK_FREE:		//空闲状态
		{
			//在空闲状态下，初始化任务信息.
			//初始化任务
			//memset(m_UserTaskInfo, 0, sizeof(m_UserTaskInfo));

			//构造数据
			CMD_S_StatusFree StatusFree;
			memset(&StatusFree,0,sizeof(StatusFree));

			//设置变量
			StatusFree.lBaseScore=m_pGameServiceOption->lCellScore;
			CopyMemory( StatusFree.bBrightStart, m_bBrightStart, sizeof( StatusFree.bBrightStart ) );

			//发送场景
			return m_pITableFrame->SendGameScene(pIServerUserItem,&StatusFree,sizeof(StatusFree));
		}
	case GS_WK_SCORE:		//叫分状态
		{
			//构造数据
			CMD_S_StatusScore StatusScore;
			memset(&StatusScore,0,sizeof(StatusScore));

			//设置变量
			StatusScore.bLandScore=m_bLandScore;
			StatusScore.wCurrentUser=m_wCurrentUser;
			StatusScore.lBaseScore=m_pGameServiceOption->lCellScore;
			StatusScore.bCallScorePhase=m_bCallScorePhase;
			CopyMemory(StatusScore.bScoreInfo,m_bScoreInfo,sizeof(m_bScoreInfo));
			//		CopyMemory(StatusScore.bCardData,m_bHandCardData[wChiarID],m_bCardCount[wChiarID]);
			CopyMemory(StatusScore.bCardData,m_bHandCardData, sizeof(m_bHandCardData)*sizeof(BYTE));
			CopyMemory( StatusScore.bUserTrustee, m_bUserTrustee, sizeof( m_bUserTrustee ) );
			StatusScore.bBrightTime = m_cbBrightCardTime;
			CopyMemory( StatusScore.bUserBrightCard, m_bUserBrightCard, sizeof( StatusScore.bUserBrightCard ) );

			//明牌判断
			if ( m_bCallScorePhase == CSD_BRIGHTCARD && m_wCurrentUser!=wChiarID && m_bUserBrightCard[wChiarID]==false ) m_cbBrightCardCount++;

			//发送场景
			return m_pITableFrame->SendGameScene(pIServerUserItem,&StatusScore,sizeof(StatusScore));
		}
	case GS_WK_DOUBLE_SCORE:	//加倍状态
		{
			//构造消息
			CMD_S_StatusDoubleScore StatusDoubleScore;
			ZeroMemory( &StatusDoubleScore, sizeof( StatusDoubleScore ) );
			StatusDoubleScore.bAllowDouble = m_bAllowDoubleScore;
			CopyMemory(StatusDoubleScore.bBackCard, m_bBackCard, sizeof(m_bBackCard ) );
			CopyMemory(StatusDoubleScore.bCardCount, m_bCardCount, sizeof(m_bCardCount ) );
			CopyMemory(StatusDoubleScore.bCardData, m_bHandCardData, sizeof(StatusDoubleScore.bCardData ) );
			StatusDoubleScore.bLandScore = m_bLandScore;
			CopyMemory(StatusDoubleScore.bUserTrustee, m_bUserTrustee, sizeof(StatusDoubleScore.bUserTrustee ) );	
			StatusDoubleScore.lBaseScore = m_pGameServiceOption->lCellScore;
			StatusDoubleScore.wLandUser = m_wBankerUser;
			CopyMemory(StatusDoubleScore.bDoubleUser, m_bDoubleUser, sizeof(StatusDoubleScore.bDoubleUser ) );	
			CopyMemory( StatusDoubleScore.bUserBrightCard, m_bUserBrightCard, sizeof( StatusDoubleScore.bUserBrightCard ) );


			//发送场景
			return m_pITableFrame->SendGameScene(pIServerUserItem,&StatusDoubleScore,sizeof(StatusDoubleScore));
		}
	case GS_WK_PLAYING:		//游戏状态
		{
			//构造数据
			CMD_S_StatusPlay StatusPlay;
			memset(&StatusPlay,0,sizeof(StatusPlay));

			//设置变量
			StatusPlay.wBombTime=m_wBombTime;
			StatusPlay.wBombNums = m_wBombNums;
			StatusPlay.wLandUser=m_wBankerUser;
			StatusPlay.wCurrentUser=m_wCurrentUser;
			StatusPlay.wLastOutUser=m_wTurnWiner;
			StatusPlay.bLandScore=m_bLandScore;
			StatusPlay.bTurnCardCount=m_bTurnCardCount;
			StatusPlay.lBaseScore=m_pGameServiceOption->lCellScore;
			CopyMemory(StatusPlay.bBackCard,m_bBackCard,sizeof(m_bBackCard));
			CopyMemory(StatusPlay.bCardCount,m_bCardCount,sizeof(m_bCardCount));
			CopyMemory(StatusPlay.bTurnCardData,m_bTurnCardData,sizeof(BYTE)*m_bTurnCardCount);
			//		CopyMemory(StatusPlay.bCardData,m_bHandCardData[wChiarID],sizeof(BYTE)*m_bCardCount[wChiarID]);
			CopyMemory(StatusPlay.bCardData,m_bHandCardData,sizeof(BYTE)*sizeof(m_bHandCardData));
			CopyMemory( StatusPlay.bUserTrustee, m_bUserTrustee, sizeof( m_bUserTrustee ) );
			StatusPlay.bBrightTime = m_cbBrightCardTime;
			CopyMemory( StatusPlay.bUserBrightCard, m_bUserBrightCard, sizeof( StatusPlay.bUserBrightCard ) );
			::memcpy(StatusPlay.bDoubleUser, m_bDoubleUser, sizeof(StatusPlay.bDoubleUser));

			//发送场景
			return m_pITableFrame->SendGameScene(pIServerUserItem,&StatusPlay,sizeof(StatusPlay));
		}
	}

	return false;
}

//定时器事件
bool __cdecl CTableFrameSink::OnTimerMessage(WORD wTimerID, WPARAM wBindParam)
{
	return false;
}

//游戏消息处理
bool __cdecl CTableFrameSink::OnGameMessage(WORD wSubCmdID, const void * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	bool bFlag=false; //返回状态
	switch (wSubCmdID)
	{
	case SUB_C_LAND_SCORE:	//用户叫地主
		{
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_C_LandScore));

			if (wDataSize!=sizeof(CMD_C_LandScore)) 
			{
				bFlag = false;
				break;
			}

			//用户效验
			tagServerUserData * pUserData=pIServerUserItem->GetUserData();
			if (pUserData->cbUserStatus!=US_PLAY)
			{
				bFlag=false;
				break;
			}

			//消息处理
			CMD_C_LandScore * pLandScore=(CMD_C_LandScore *)pDataBuffer;
			bFlag = OnUserLandScore(pUserData->wChairID,pLandScore->bLandScore);

			//add by Ivan_han
			this->OnAutoPassOrOutCard();

			break;
		}
	case SUB_C_SNATCH_SCORE:	//抢地主
		{
			//校验数据
			ASSERT(wDataSize==sizeof(CMD_C_SnatchLand));
			if(wDataSize != sizeof(CMD_C_SnatchLand))
			{
				bFlag = false;
				break;
			}
			//用户校验
			tagServerUserData * pUserData = pIServerUserItem->GetUserData();
			if (pUserData->cbUserStatus != US_PLAY)
			{
				bFlag = false;
				break;
			}
			//消息处理
			CMD_C_SnatchLand * pSnatchLand=(CMD_C_SnatchLand *)pDataBuffer;
			if (NULL != pSnatchLand)
			{
				bFlag = OnUserSnatchLand(pUserData->wChairID,pSnatchLand->bSnatchLand);
			}

			//add by Ivan_han
			this->OnAutoPassOrOutCard();

			break;
		}
	case SUB_C_OUT_CART:		//用户出牌
		{
			if (sizeof(CMD_C_OutCard)!=wDataSize)
			{
				bFlag=false;
				break;
			}
			//变量定义
			CMD_C_OutCard * pOutCard=(CMD_C_OutCard *)pDataBuffer;

			if (NULL != pOutCard)
			{
				//用户效验
				tagServerUserData * pUserData=pIServerUserItem->GetUserData();
				if (pUserData->cbUserStatus!=US_PLAY)
				{
					bFlag=false;
					break;
				}

				//消息处理
				bFlag = OnUserOutCard(pUserData->wChairID,pOutCard->bCardData,pOutCard->bCardCount);

				//add by Ivan_han
				//char sz[100]={0};
				//sprintf(sz,"响应SUB_C_OUT_CART消息时的额外处理:m_wCurrentUser=%d",m_wCurrentUser);
				//CTraceService::TraceString(sz, TraceLevel_Normal);
				this->OnAutoPassOrOutCard();
				////游戏还没有结束
				//if (m_wCurrentUser!=INVALID_CHAIR)
				//{
				//	WORD wCurrentUser=m_wCurrentUser;
				//	vector<WORD>::iterator p;
				//	p=find(m_vecChairIDOffLine.begin(),m_vecChairIDOffLine.end(),wCurrentUser);
				//	if(p!=m_vecChairIDOffLine.end())
				//	{
				//		char sz[100]={0};
				//		sprintf(sz,"响应SUB_C_OUT_CART消息时的额外处理:m_wCurrentUser=%d,wCurrentUser=%d",m_wCurrentUser,wCurrentUser);
				//		CTraceService::TraceString(sz, TraceLevel_Normal);
				//		OnUserPassCard(wCurrentUser);
				//	}
				//}

			}
			break;
		}
	case SUB_C_PASS_CARD:
		{
			//用户效验
			tagServerUserData * pUserData=pIServerUserItem->GetUserData();
			if (pUserData->cbUserStatus!=US_PLAY)
			{
				bFlag=false;
				break;
			}

			//消息处理
			bFlag = OnUserPassCard(pUserData->wChairID);

			//add by Ivan_han
			//char sz[100]={0};
			//sprintf(sz,"响应SUB_C_PASS_CARD消息时的额外处理:m_wCurrentUser=%d",m_wCurrentUser);
			//CTraceService::TraceString(sz, TraceLevel_Normal);
			this->OnAutoPassOrOutCard();

			break;
		}
	case SUB_C_TRUSTEE:			//玩家托管
		{
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_C_UserTrustee));
			if (wDataSize != sizeof(CMD_C_UserTrustee))
			{
				bFlag=false;
				break;
			}

			//变量定义
			CMD_C_UserTrustee *pUserTrustee = ( CMD_C_UserTrustee * )pDataBuffer;
			if (NULL != pUserTrustee)
			{
				//消息处理
				bFlag = OnUserTrustee(pUserTrustee->wUserChairID, pUserTrustee->bTrustee);	
			}
			break;
		}
	case SUB_C_BRIGHT_START:	//明牌开始
		{
			//用户效验
			tagServerUserData * pUserData=pIServerUserItem->GetUserData();

			//消息处理
			if (NULL != pUserData)
			{
				bFlag = OnBrightStart( pUserData->wChairID, true );
			}
			break;
		}
	case SUB_C_BRIGHT:		//玩家明牌
		{
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_C_BrightCard));
			if (wDataSize != sizeof(CMD_C_BrightCard))
			{
				bFlag=false;
				break;
			}

			//用户效验
			tagServerUserData * pUserData=pIServerUserItem->GetUserData();
			if (pUserData->cbUserStatus!=US_PLAY)
			{
				bFlag=false;
				break;
			}

			//变量定义
			CMD_C_BrightCard *pBrightCard = ( CMD_C_BrightCard * )pDataBuffer;

			if (NULL != pBrightCard)
			{
				//消息处理
				bFlag = OnBrightCard( pUserData->wChairID, pBrightCard->cbBrightCardTime);
			}
			break;
		}
	case SUB_C_DOUBLE_SCORE:	//玩家加倍
		{
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_C_DoubleScore));
			if (wDataSize != sizeof(CMD_C_DoubleScore))
			{
				bFlag=false;
				break;
			}

			//用户效验
			tagServerUserData * pUserData=pIServerUserItem->GetUserData();
			if (pUserData->cbUserStatus!=US_PLAY)
			{
				bFlag=false;
				break;
			}

			if (m_bDoubleUser[pUserData->wChairID])
			{//玩家已经加倍
				bFlag=true;
				break;
			}

			//变量定义
			CMD_C_DoubleScore *pDoubleScore = ( CMD_C_DoubleScore * )pDataBuffer;

			if (NULL != pDoubleScore)
			{
				//消息处理
				bFlag = OnUserDoubleScore( pUserData->wChairID, pDoubleScore->bDoubleScore, pDoubleScore->bDoubleType);
			}
			break;
		}
	}
	if (!bFlag)
	{//测试
		return true;
	}
	return true;
}

//框架消息处理
bool __cdecl CTableFrameSink::OnFrameMessage(WORD wSubCmdID, const void * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	return false;
}

//Author:<cxf>; 
//CreateDate:<2011-12-29>;
//Description:<叫地主>;
bool CTableFrameSink::OnUserLandScore(WORD wChairID, BYTE bLandScore)
{
	//效验状态<叫地主状态>
	if (m_pITableFrame->GetGameStatus()!=GS_WK_SCORE) return false;
	//效验操作用户<是不是服务器指定操作用户>
	if (m_wOperateUser != wChairID) return false;

	//叫地主状态
	if (m_bCallScorePhase==CSD_NORMAL)
	{
		//验证<1:叫地主;0:不叫;>
		if(bLandScore != 1 && bLandScore != 0) 
		{
			return false;
		}

		//切换用户<为叫地主用户>
		m_wCurrentUser =(wChairID+1)%GAME_PLAYER;
		//操作用户
		m_wOperateUser = m_wCurrentUser;
		//是否叫地主
		if (1==bLandScore)
		{//叫地主
			m_wBankerUser = wChairID;										//庄家
			//是否有放弃叫地主用户
			bool bStart=true;
			for (int i=0;i<m_wPlayerCount;++i)
			{
				if (wChairID == i) continue;
				if (!m_bGiveUpBanker[i])
				{//其中还有  没放弃的
					bStart=false;
					break;
				}
			}
			m_pITableFrame->SetGameStatus(GS_WK_SNATCH);					//设置为抢地主阶段
			m_bCallScorePhase=CSD_SNATCHLAND;								//设置为抢地主阶段
			m_wFirstUser = wChairID;										//首叫用户

			m_bGiveUpBanker[wChairID]=false;
			for (int i=0;i<GAME_PLAYER;++i)
			{
				m_bUserBeiShu[i]=3;
			}
			//发送叫地主消息
			CMD_S_LandScore LandScore;
			ZeroMemory( &LandScore, sizeof( LandScore ) );
			LandScore.bLandUser=wChairID;									//叫地主用户
			LandScore.bLandScore=bLandScore;								//叫地主		(解释<0:不叫; 1:叫地主>此时=1)
			LandScore.wCurrentUser= bStart?INVALID_CHAIR:m_wCurrentUser;	//下个操作用户
			LandScore.bCallScorePhase=CSD_SNATCHLAND;						//现在状态<抢地主状态>
			LandScore.bPreCallScorePhase = CSD_NORMAL;						//之前状态<叫地主状态>
			CopyMemory(LandScore.bUserBeiShu,m_bUserBeiShu,sizeof(m_bUserBeiShu));  //设置玩家的倍数
			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_LAND_SCORE,&LandScore,sizeof(LandScore));		//发送玩家:叫分消息
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_LAND_SCORE,&LandScore,sizeof(LandScore));	//发送旁观玩家:叫分消息

			//游戏开始
			if (bStart)
			{
				//发送底牌
				m_bCardCount[m_wBankerUser]=20;
				CopyMemory(&m_bHandCardData[m_wBankerUser][17],m_bBackCard,sizeof(m_bBackCard));
				m_GameLogic.SortCardList(m_bHandCardData[m_wBankerUser],m_bCardCount[m_wBankerUser],ST_ORDER);

				//设置开始游戏状态
				m_pITableFrame->SetGameStatus(GS_WK_PLAYING);	
				m_bCallScorePhase=CSD_GAMESTART;
				//出牌信息
				m_bTurnCardCount=0;											//初始化出牌数目
				m_wTurnWiner=m_wBankerUser;									//每轮胜利玩家
				m_wCurrentUser=m_wBankerUser;								//当前玩家

				//发送消息
				CMD_S_GameStart GameStart;
				GameStart.wLandUser=m_wBankerUser;							//庄家
				GameStart.bLandScore=bLandScore;							//
				GameStart.wCurrentUser=m_wBankerUser;						//当前操作玩家
				CopyMemory(GameStart.bBackCardData, m_bBackCard, sizeof(GameStart.bBackCardData));
				CopyMemory(GameStart.bUserBrightCard, m_bUserBrightCard, sizeof(GameStart.bUserBrightCard));
				//游戏开始消息
				for (int i=0; i<GAME_PLAYER; ++i)
				{
					::memset(GameStart.bCardData, 0, sizeof(GameStart.bCardData));
					::memcpy(GameStart.bCardData, m_bHandCardData[i], sizeof(GameStart.bCardData));
					GameStart.bCardNum = m_bCardCount[i];
					m_pITableFrame->SendTableData(i,SUB_S_GAME_START,&GameStart,sizeof(CMD_S_GameStart));
				}
				m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_START,&GameStart,sizeof(GameStart));
				m_bLandScore = 1;
			}
		}
		else
		{//不叫
			m_bGiveUpBanker[wChairID]=true;									//放弃叫地主标识

			if (m_bGiveUpBanker[m_wCurrentUser])
			{//<一圈没人叫地主>
				//add by Ivan_han 
				if(m_vecChairIDOffLine.size()>0)
				{//如果有断线的玩家,解散游戏.
				     m_pITableFrame->DismissGame();
				}
				else
				{//重新发牌
					m_bCallScorePhase = CSD_NORMAL;								//叫地主阶段
					ZeroMemory( m_bGiveUpBanker, sizeof( m_bGiveUpBanker ) );	//放弃标识
					m_wCurrentUser=m_wFirstUser;								//设置用户
					//发牌
					this->SetFixOrRandCardOrGoodCard();
					//发送扑克
					CMD_S_SendAllCard SendAllCard ;
					ZeroMemory(&SendAllCard, sizeof(SendAllCard));
					SendAllCard.wCurrentUser = m_wCurrentUser ;					//当前用户
					CopyMemory(SendAllCard.bCardData, m_bHandCardData, sizeof(m_bHandCardData)) ;//手上扑克
					CopyMemory(SendAllCard.bBackCardData, m_bBackCard, 3);		//底牌扑克
					for (WORD i=0;i<m_wPlayerCount;i++)
					{//发送牌
						m_pITableFrame->SendTableData(i,SUB_S_SEND_CARD,&SendAllCard,sizeof(SendAllCard));
						m_pITableFrame->SendLookonData(i,SUB_S_SEND_CARD,&SendAllCard,sizeof(SendAllCard));
					}
					//排列扑克
					for (WORD i=0;i<m_wPlayerCount;i++)
					{
						m_GameLogic.SortCardList(m_bHandCardData[i],m_bCardCount[i],ST_ORDER);
					}
				}
			}
			else
			{//下一位叫地主
				//发送叫地主消息
				CMD_S_LandScore LandScore;
				ZeroMemory( &LandScore, sizeof(LandScore));
				LandScore.bLandUser=wChairID;								//叫地主用户
				LandScore.bLandScore=bLandScore;							//叫地主		(解释<0:不叫; 1:叫地主>此时=0)
				LandScore.wCurrentUser=m_wCurrentUser;						//下个操作用户
				LandScore.bCallScorePhase = CSD_NORMAL;						//现在状态<叫地主状态>
				LandScore.bPreCallScorePhase = CSD_NORMAL;					//之前状态<叫地主状态>
				m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_LAND_SCORE,&LandScore,sizeof(LandScore));		//发送玩家:叫分消息
				m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_LAND_SCORE,&LandScore,sizeof(LandScore));	//发送旁观玩家:叫分消息
			}
		}
		return true;
	}

	return false;//错误阶段
}

//Author:<cxf>; 
//CreateDate:<2011-12-29>;
//Description:<抢地主>;
//抢地主事件
bool CTableFrameSink::OnUserSnatchLand(WORD wChairID, BYTE bLandScore)
{
	//效验状态<抢地主状态>
	if (m_pITableFrame->GetGameStatus()!=GS_WK_SNATCH) return false;
	//效验操作用户
	if (m_wOperateUser != wChairID) return false;

	//抢地主
	if (CSD_SNATCHLAND == m_bCallScorePhase)
	{
		bool bContinue=false;											//是否继续抢地主<false:开始; true:继续抢地主>
		//是否抢地主
		if (1==bLandScore)
		{//抢地主
			++m_bSnatchCount;
			for (int i=0;i<GAME_PLAYER;++i)
			{
				m_bUserBeiShu[i]*=2;                                    //设置玩家倍数
			}
			m_wFirstUser = wChairID;									//抢地主用户
			m_bGiveUpSnatch[wChairID] = 2;							    //抢地主标识
			if (m_wBankerUser==wChairID)
			{//叫地主用户再次抢地主										
				m_bSnatchCount=3;										//抢地主次数
				m_wCurrentUser = wChairID;								//当前用户
				bContinue = false;
			}
			else
			{
				m_wCurrentUser = INVALID_CHAIR;							//当前用户
				//过滤放弃叫地主用户
				WORD wUser = (wChairID+1)%GAME_PLAYER;
				while (wUser != wChairID)
				{
					//没有放弃过叫地主才有机会抢地主 和 还没抢过地主的
					if (!m_bGiveUpBanker[wUser] && m_bGiveUpSnatch[wUser] == 0)
					{
						m_wCurrentUser=wUser;
						break;
					}
					if(wUser==wChairID) break;
					wUser = (wUser+1)%GAME_PLAYER;
				}

				//是否还可以抢地主
				if (m_wCurrentUser == INVALID_CHAIR)
				{//没人再可以抢地主
					m_wCurrentUser = wChairID;
					bContinue = false;
				}
				else
				{//下一玩家抢地主
					bContinue = true;
				}
			}
		}
		else
		{//不抢
			m_bGiveUpSnatch[wChairID] = 1;								//抢地主标识<0:没有操作; 1:不抢; 2:抢地主>
			m_wCurrentUser = INVALID_CHAIR;								//当前用户

			//检查是否完成抢地主
			bool bSnatch=false;
			WORD wAccessUser = (m_wFirstUser+1)%GAME_PLAYER;
			while (wAccessUser != m_wFirstUser)
			{
				if (m_bGiveUpSnatch[wAccessUser] == 0 && !m_bGiveUpBanker[wAccessUser])
				{//继续可以抢地主
					bSnatch =true;
					break;
				}
				wAccessUser = (wAccessUser+1)%GAME_PLAYER;
			}

			//过滤放弃叫地主用户
			if (bSnatch)
			{
				WORD wUser = (wChairID+1)%GAME_PLAYER;
				while (wUser != wChairID)
				{
					//没有放弃过叫地主才有机会抢地主 和 还没抢过地主的
					if (!m_bGiveUpBanker[wUser] && m_bGiveUpSnatch[wUser] == 0)
					{
						m_wCurrentUser=wUser;
						break;
					}
					if(wUser==wChairID) break;
					wUser = (wUser+1)%GAME_PLAYER;
				}
			}

			//是否还可以抢地主
			if (m_wCurrentUser == INVALID_CHAIR)
			{//没人再可以抢地主
				m_wCurrentUser = m_wFirstUser;							//上次抢地主用户<>
				bContinue = false;
			}
			else
			{//下一玩家抢地主
				bContinue = true;
			}
		}

		//指定操作玩家
		m_wOperateUser = m_wCurrentUser;

		//是否继续抢地主<false:开始; true:继续抢地主>
		if (!bContinue)
		{//抢地主完成
			m_wBankerUser = m_wCurrentUser;								//庄家
			//抢地主消息
			CMD_S_SnatchLand SnatchLand;
			ZeroMemory(&SnatchLand, sizeof(CMD_S_SnatchLand));
			SnatchLand.wCurrentUser=INVALID_CHAIR;						//当前用户
			SnatchLand.wSnatchUser=wChairID;							//抢地主用户
			SnatchLand.bSnatchLand=bLandScore;							//抢地主		(解释<0:不抢; 1:抢地主>此时=1)
			SnatchLand.bPreCallScorePhase = CSD_SNATCHLAND;				//之前状态<抢地主状态>
			SnatchLand.bCallScorePhase = CSD_GAMESTART;					//现在状态<抢地主状态>
			CopyMemory(SnatchLand.bUserBeiShu,m_bUserBeiShu,sizeof(m_bUserBeiShu));   //设置抢地主倍数
			SnatchLand.bSnatchLandCount=m_bSnatchCount;                 //抢地主次数
			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_USER_SNATCH_SCORE,&SnatchLand,sizeof(SnatchLand));		//发送玩家:抢地主消息
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_USER_SNATCH_SCORE,&SnatchLand,sizeof(SnatchLand));		//发送旁观玩家:抢地主消息

			//发送底牌
			m_bCardCount[m_wBankerUser]=20;
			CopyMemory(&m_bHandCardData[m_wBankerUser][17],m_bBackCard,sizeof(m_bBackCard));
			m_GameLogic.SortCardList(m_bHandCardData[m_wBankerUser],m_bCardCount[m_wBankerUser],ST_ORDER);

			//设置开始游戏状态
			m_pITableFrame->SetGameStatus(GS_WK_PLAYING);	
			m_bCallScorePhase=CSD_GAMESTART;

			//出牌信息
			m_bTurnCardCount=0;											//初始化出牌数目
			m_wTurnWiner=m_wBankerUser;									//每轮胜利玩家

			//发送消息
			CMD_S_GameStart GameStart;
			GameStart.wLandUser=m_wBankerUser;							//庄家
			GameStart.bLandScore=bLandScore;							//
			GameStart.wCurrentUser=m_wBankerUser;						//当前操作玩家
			CopyMemory(GameStart.bBackCardData, m_bBackCard, sizeof(GameStart.bBackCardData));
			//游戏开始消息
			for (int i=0; i<GAME_PLAYER; ++i)
			{
				::memset(GameStart.bCardData, 0, sizeof(GameStart.bCardData));
				::memcpy(GameStart.bCardData, m_bHandCardData[i], sizeof(GameStart.bCardData));
				GameStart.bCardNum = m_bCardCount[i];
				m_pITableFrame->SendTableData(i,SUB_S_GAME_START,&GameStart,sizeof(CMD_S_GameStart));
			}
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_START,&GameStart,sizeof(GameStart));
			m_bLandScore =1;
		}
		else
		{//下一玩家抢地主
			//抢地主消息
			CMD_S_SnatchLand SnatchLand;
			ZeroMemory(&SnatchLand, sizeof(CMD_S_SnatchLand));
			SnatchLand.wCurrentUser=m_wCurrentUser;						//当前用户
			SnatchLand.wSnatchUser=wChairID;							//抢地主用户
			SnatchLand.bSnatchLand=bLandScore;							//抢地主		(解释<0:不抢; 1:抢地主>此时=1)
			SnatchLand.bPreCallScorePhase = CSD_SNATCHLAND;				//现在状态<抢地主状态>
			SnatchLand.bCallScorePhase = CSD_SNATCHLAND;				//之前状态<抢地主状态>
			CopyMemory(SnatchLand.bUserBeiShu,m_bUserBeiShu,sizeof(m_bUserBeiShu));  //设置抢地主倍数
			SnatchLand.bSnatchLandCount=m_bSnatchCount;                 //抢地主次数
			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_USER_SNATCH_SCORE,&SnatchLand,sizeof(SnatchLand));
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_USER_SNATCH_SCORE,&SnatchLand,sizeof(SnatchLand));
		}
		return true;
	}
	return false;//错误阶段
}

/* Del:<cxf>; DeleteDate:<2011-12-29>
//叫分事件
bool CTableFrameSink::OnUserLandScore(WORD wChairID, BYTE bLandScore)
{
//效验状态
if (m_pITableFrame->GetGameStatus()!=GS_WK_SCORE) return true;

//变量定义
bool bContinue=false;
BYTE cbPreCallScorePhase = m_bCallScorePhase;

//叫分处理
if (m_bCallScorePhase==CSD_NORMAL)
{
//效验参数
ASSERT(((bLandScore<=3)||(bLandScore==255))&&(bLandScore>m_bLandScore));
if (((bLandScore>3)&&(bLandScore!=255))||(bLandScore<=m_bLandScore)) return false;

//设置变量
if (bLandScore!=255)
{
m_bLandScore=bLandScore;
//m_bLandScore=1;
m_wBankerUser=m_wCurrentUser;
}
else
{
bContinue = true;
m_bGiveUpBanker[wChairID] = true;

//切换玩家
m_wCurrentUser=(wChairID+1)%m_wPlayerCount;

//重新发牌
if ( m_bGiveUpBanker[m_wCurrentUser] )
{
//重新发牌
if ( m_wBankerUser == INVALID_CHAIR )
{
//疯狂变量
m_bBrightCard=false;
m_bCallScorePhase=CSD_NORMAL;
ZeroMemory( m_bGiveUpBanker, sizeof( m_bGiveUpBanker ) );

BYTE cbSendGoodCard = rand() % GOOD_CARD_PERCENT ;
BYTE cbAndroidUserCount=0 ;
WORD wAndroidUser[GAME_PLAYER]={0};

//重置变量
ZeroMemory(m_bScoreInfo, sizeof(m_bScoreInfo)) ;

//机器人个数
for(WORD wChairID=0; wChairID<GAME_PLAYER; ++wChairID)
{
IServerUserItem *pServerUserItem = m_pITableFrame->GetServerUserItem(wChairID) ;
if(pServerUserItem->IsAndroidUser())
{
wAndroidUser[cbAndroidUserCount] = (wChairID) ;
++cbAndroidUserCount ;
}
}

WORD wHaveGoodCardAndroidUser = INVALID_CHAIR ;
if(cbAndroidUserCount>0) wHaveGoodCardAndroidUser = wAndroidUser[rand()%cbAndroidUserCount] ;

//发送好牌
if( 0 == cbSendGoodCard && wHaveGoodCardAndroidUser!=INVALID_CHAIR)
{
//混乱扑克
BYTE bRandCard[54];
m_GameLogic.RandCardList(bRandCard,sizeof(bRandCard)/sizeof(bRandCard[0]));

BYTE cbGoodCard[NORMAL_COUNT] ;
m_GameLogic.GetGoodCardData(cbGoodCard) ;

//抽取好牌
m_GameLogic.RemoveGoodCardData(cbGoodCard, NORMAL_COUNT, bRandCard, FULL_COUNT ) ;

//分发扑克
CopyMemory(m_bHandCardData[wHaveGoodCardAndroidUser], cbGoodCard, NORMAL_COUNT) ;
m_bCardCount[wHaveGoodCardAndroidUser]=17;
for (WORD i=0, j=0;i<m_wPlayerCount;i++)
{
if(i!=wHaveGoodCardAndroidUser)
{
m_bCardCount[i]=17;
CopyMemory(&m_bHandCardData[i],&bRandCard[j*17],sizeof(BYTE)*17);
++j ;
}
}
CopyMemory(m_bBackCard,&bRandCard[34],sizeof(m_bBackCard));
m_GameLogic.SortCardList(m_bBackCard,3,ST_ORDER);
}
else
{
////混乱扑克
//BYTE bRandCard[54];
//m_GameLogic.RandCardList(bRandCard,sizeof(bRandCard)/sizeof(bRandCard[0]));

////分发扑克
//for (WORD i=0;i<m_wPlayerCount;i++)
//{
//	m_bCardCount[i]=17;
//	CopyMemory(&m_bHandCardData[i],&bRandCard[i*m_bCardCount[i]],sizeof(BYTE)*m_bCardCount[i]);
//}
//CopyMemory(m_bBackCard,&bRandCard[51],sizeof(m_bBackCard));
//m_GameLogic.SortCardList(m_bBackCard,3,ST_ORDER);
this->SetFixOrRandCard();
}

//设置用户
m_wCurrentUser=m_wFirstUser;

//发送扑克
CMD_S_SendAllCard SendAllCard ;
ZeroMemory(&SendAllCard, sizeof(SendAllCard));
SendAllCard.wCurrentUser = m_wCurrentUser ;

//手上扑克
CopyMemory(SendAllCard.bCardData, m_bHandCardData, sizeof(m_bHandCardData)) ;
//底牌扑克
CopyMemory(SendAllCard.bBackCardData, m_bBackCard, 3) ;
for (WORD i=0;i<m_wPlayerCount;i++)
{
m_pITableFrame->SendTableData(i,SUB_S_SEND_CARD,&SendAllCard,sizeof(SendAllCard));
m_pITableFrame->SendLookonData(i,SUB_S_SEND_CARD,&SendAllCard,sizeof(SendAllCard));
}

//排列扑克
for (WORD i=0;i<m_wPlayerCount;i++)
{
m_GameLogic.SortCardList(m_bHandCardData[i],m_bCardCount[i],ST_ORDER);
}

return true;
}
else
{
//设置玩家
m_wCurrentUser = m_wBankerUser;

//切换状态
m_bCallScorePhase=CSD_GAMESTART;

//设置变量
bContinue = false;
}
}
}

m_bScoreInfo[wChairID]=bLandScore;



//状态切换
if (m_bLandScore > 0 && m_bCallScorePhase!=CSD_GAMESTART)
{
bContinue=true;
//m_wFirstUser=wChairID;
//m_bCallScorePhase=CSD_SNATCHLAND;

//切换玩家
m_wCurrentUser = INVALID_CHAIR;
WORD wUser = (wChairID+1)%GAME_PLAYER;
if(m_bLandScore != 3)
{
while (  wUser != wChairID ) 
{
if ( m_bGiveUpBanker[wUser] == false )
{
m_wCurrentUser = wUser;
break;
}
wUser = (wUser+1)%GAME_PLAYER ;
}
}

if ( m_wCurrentUser == INVALID_CHAIR )
{
m_wCurrentUser = wChairID;
bContinue = false;
}
}
//开始判断
if(m_wFirstUser == m_wCurrentUser)
{
//设置玩家
m_wCurrentUser = m_wBankerUser;

//切换状态
m_bCallScorePhase=CSD_GAMESTART;

//设置变量
bContinue = false;
}
}
else if (m_bCallScorePhase==CSD_SNATCHLAND)
{
//效验参数
ASSERT((bLandScore==4)||(bLandScore==255));
if ((bLandScore!=4)&&(bLandScore!=255)) return false;

//设置变量
if (bLandScore==4)
{
m_bLandScore++;
m_wBankerUser=wChairID;
}

//状态切换
if (m_wFirstUser==wChairID)
{
bContinue=true;
m_wFirstUser=m_wBankerUser;
m_wCurrentUser = m_wBankerUser;

//明牌判断
if ( m_cbBrightCardTime > 2 && m_bUserBrightCard[m_wCurrentUser] )
{
m_bCallScorePhase = CSD_GAMESTART;
bContinue = false;
}
else
m_bCallScorePhase=CSD_BRIGHTCARD;
}
else bContinue=true;

//切换玩家
if ( m_bCallScorePhase == CSD_SNATCHLAND )
{
m_wCurrentUser = INVALID_CHAIR;
WORD wUser = (wChairID+1)%GAME_PLAYER;
while (  wUser != wChairID && wUser != (m_wFirstUser+1)%GAME_PLAYER ) 
{
if ( m_bGiveUpBanker[wUser] == false )
{
m_wCurrentUser = wUser;
break;
}
wUser = (wUser+1)%GAME_PLAYER ;
}

if ( m_wCurrentUser == INVALID_CHAIR || ( m_wFirstUser == m_wBankerUser && m_wFirstUser == m_wCurrentUser ))
{
m_wCurrentUser = m_wBankerUser;
bContinue=true;
m_wFirstUser=m_wBankerUser;

//明牌判断
bool bAllUserBright = true;
for ( WORD wBrightIdx = 0; wBrightIdx < GAME_PLAYER; ++wBrightIdx )
if ( ! m_bUserBrightCard[wBrightIdx] ) bAllUserBright = false;

//明牌判断
if ( m_cbBrightCardTime > 2 && bAllUserBright )
{
m_bCallScorePhase = CSD_GAMESTART;
bContinue = false;
}
else
m_bCallScorePhase=CSD_BRIGHTCARD;
}
}
}
else if (m_bCallScorePhase==CSD_BRIGHTCARD)
{
//效验参数
ASSERT((bLandScore==5)||(bLandScore==255));
if ((bLandScore!=5)&&(bLandScore!=255)) return false;

//增加次数
m_cbBrightCardCount++;
if ( m_cbBrightCardCount == GAME_PLAYER )
{
bContinue = false;
m_bCallScorePhase = CSD_BRIGHTCARD;
}
else 
{
bContinue = true;
m_bCallScorePhase=CSD_GAMESTART;
}

//设置变量
m_bBrightCard=(bLandScore==5)?true:false;
if ( bLandScore == 5 )
{
m_bLandScore++;
m_bUserBrightCard[wChairID] = true;
}
bContinue = true;
}

//开始判断
//if ((bContinue==false)&&(m_bLandScore>=1 || m_wBankerUser != INVALID_CHAIR))
//{
//	//切换状态
//	//m_bCallScorePhase = CSD_BRIGHTCARD;
//}


////明牌状态
//if ( m_bCallScorePhase == CSD_BRIGHTCARD )
//{
//	//明牌个数
//	for ( WORD wBrightIdx = 0; wBrightIdx < GAME_PLAYER; ++wBrightIdx ) 
//		if ( m_bUserBrightCard[wBrightIdx] ) ++m_cbBrightCardCount;
//}

//开始判断
//if ( m_cbBrightCardCount == GAME_PLAYER  && CSD_BRIGHTCARD ==m_bCallScorePhase )
if ((bContinue==false)&&(m_bLandScore>=1 || m_wBankerUser != INVALID_CHAIR))
{
//cxf 
//设置状态
//m_pITableFrame->SetGameStatus(GS_WK_DOUBLE_SCORE);//加倍状态
// end

//游戏开始之前记录斗地主m_wBankerUser
//for (int i=0; i<GAME_PLAYER; i++)
//{
//	if (i == m_wBankerUser)
//	{//是地主，加1。
//		++m_UserTaskInfo[i].iRegJoineLandNums;
//	}
//	else
//	{//不是地主，清零。
//		m_UserTaskInfo[i].iRegJoineLandNums = 0;
//	}
//}
// 
//设置开始状态 lq ADD JiaoFen
m_bCallScorePhase=CSD_GAMESTART;

//当前玩家倍数
for (int i=0; i<GAME_PLAYER; i++)
{
if (i == m_wBankerUser)
{
m_bUserBeiShu[i]=2*1;
}
else
{
m_bUserBeiShu[i]=1;
}	
}
BYTE bBackCard[3]={0};
bBackCard[0]=m_GameLogic.GetCardValue(m_bBackCard[0]);
bBackCard[1]=m_GameLogic.GetCardValue(m_bBackCard[1]);
bBackCard[2]=m_GameLogic.GetCardValue(m_bBackCard[2]);
if ((2==bBackCard[0]&&bBackCard[1]!=2&&bBackCard[2]!=2) || (2==bBackCard[1]&&bBackCard[0]!=2&&bBackCard[2]!=2) ||
(2==bBackCard[2]&&bBackCard[0]!=2&&bBackCard[1]!=2))  //lq 底牌任务
{
m_tagTask[m_wBankerUser] |= CT_TASK_DP_TWO;
}
if (15==bBackCard[0] || 15==bBackCard[1] || 15==bBackCard[2])
{
m_tagTask[m_wBankerUser] |= CT_TASK_DP_KING;
}
if (bBackCard[0]==bBackCard[1] || bBackCard[1]==bBackCard[2])
{
m_tagTask[m_wBankerUser] |= CT_TASK_DP_SAME;
}

//发送消息
CMD_S_LandScore LandScore;
ZeroMemory( &LandScore, sizeof( LandScore ) );
LandScore.bLandUser=wChairID;
LandScore.bLandScore=bLandScore;
LandScore.wCurrentUser=INVALID_CHAIR;
LandScore.bCurrentScore=m_bLandScore;
LandScore.bCallScorePhase=m_bCallScorePhase;
LandScore.bPreCallScorePhase = cbPreCallScorePhase;
CopyMemory(LandScore.bBrightCardUser, m_bUserBrightCard, sizeof( LandScore.bBrightCardUser ) );
CopyMemory(LandScore.bUserBeiShu, m_bUserBeiShu, sizeof( LandScore.bUserBeiShu ) );
m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_LAND_SCORE,&LandScore,sizeof(LandScore));
m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_LAND_SCORE,&LandScore,sizeof(LandScore));

//获取路径
CHAR szDirectory[128];
GetCurrentDirectory(sizeof( szDirectory ), szDirectory );

//文件名称
CHAR szProfileName[128];
sprintf(szProfileName, TEXT("%s\\LandCrazy.ini"), szDirectory);

//读取配置
LONGLONG lDoubleLimit = GetPrivateProfileInt(TEXT("DoubleScore"), TEXT("Score"), 100, szProfileName );

bool bAllowDouble = true;
for ( WORD wUserID = 0; wUserID < GAME_PLAYER; ++wUserID )
{
//获取玩家
IServerUserItem *pIServerUserItem = m_pITableFrame->GetServerUserItem( wUserID );
if ( pIServerUserItem == NULL ) continue;

//条件判断
LONGLONG lUserScore = pIServerUserItem->GetUserScore()->lGold;
//(可能出错)当游戏为宝石计算应该lScore 为宝石，当游戏为积分的应该lScore飞积分.  由服务器来决定
if ( lUserScore < lDoubleLimit )
{
bAllowDouble = false;
break;
}
}
m_bAllowDoubleScore = bAllowDouble;

//发送底牌
m_bCardCount[m_wBankerUser]=20;
CopyMemory(&m_bHandCardData[m_wBankerUser][17],m_bBackCard,sizeof(m_bBackCard));
m_GameLogic.SortCardList(m_bHandCardData[m_wBankerUser],m_bCardCount[m_wBankerUser],ST_ORDER);

//设置消息
CMD_S_DoubleScore DoubleScore;
ZeroMemory( &DoubleScore, sizeof( DoubleScore ) );
DoubleScore.bAllowDouble = bAllowDouble;
DoubleScore.wLandUser = m_wBankerUser;
DoubleScore.bCurrentScore = m_bLandScore;
CopyMemory(DoubleScore.bBackCard,m_bBackCard,sizeof(m_bBackCard));

//发送消息
m_pITableFrame->SendTableData( INVALID_CHAIR, SUB_S_DOUBLE_SCORE, &DoubleScore, sizeof( DoubleScore ) );

//----cxf 叫分完成
//设置状态
m_pITableFrame->SetGameStatus(GS_WK_PLAYING);	

//出牌信息
m_bTurnCardCount=0;
m_wTurnWiner=m_wBankerUser;
m_wCurrentUser=m_wBankerUser;

//发送消息
CMD_S_GameStart GameStart;
GameStart.wLandUser=m_wBankerUser;
GameStart.bLandScore=m_bLandScore;
GameStart.bBrightCard=m_bBrightCard;
GameStart.wCurrentUser=m_wCurrentUser;
CopyMemory(GameStart.bBackCardData, m_bBackCard, sizeof(GameStart.bBackCardData));
CopyMemory(GameStart.bUserBrightCard, m_bUserBrightCard, sizeof(GameStart.bUserBrightCard));

for (int i=0; i<GAME_PLAYER; ++i)
{
::memset(GameStart.bCardData, 0, sizeof(GameStart.bCardData));
::memcpy(GameStart.bCardData, m_bHandCardData[i], sizeof(GameStart.bCardData));
GameStart.bCardNum = m_bCardCount[i];
m_pITableFrame->SendTableData(i,SUB_S_GAME_START,&GameStart,sizeof(CMD_S_GameStart));
}

m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_START,&GameStart,sizeof(GameStart));
//------cxf  end
//设置变量
if (m_bLandScore==0) m_bLandScore=1;
if (m_wBankerUser==INVALID_CHAIR) m_wBankerUser=m_wFirstUser;

return true;
}

//发送消息
CMD_S_LandScore LandScore;
ZeroMemory( &LandScore, sizeof( LandScore ) );
LandScore.bLandUser=wChairID;
LandScore.bLandScore=bLandScore;
LandScore.wCurrentUser=m_wCurrentUser;
LandScore.bCurrentScore=m_bLandScore;
LandScore.bCallScorePhase=m_bCallScorePhase;
LandScore.bPreCallScorePhase = cbPreCallScorePhase;
CopyMemory(LandScore.bBrightCardUser, m_bUserBrightCard, sizeof( LandScore.bBrightCardUser ) );
m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_LAND_SCORE,&LandScore,sizeof(LandScore));
m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_LAND_SCORE,&LandScore,sizeof(LandScore));

return true;
}
*/
//用户出牌
bool CTableFrameSink::OnUserOutCard(WORD wChairID, BYTE bCardData[], BYTE bCardCount)
{
	//效验状态
	if (m_pITableFrame->GetGameStatus()!=GS_WK_PLAYING) return true;
	if (wChairID!=m_wCurrentUser) 
		return false;

	//排序扑克
	m_GameLogic.SortCardList(bCardData,bCardCount,ST_ORDER);

	//类型判断
	BYTE bCardType=m_GameLogic.GetCardType(bCardData,bCardCount);
	if (bCardType==CT_ERROR) 
		return false;
	//任务
	if(CT_SINGLE_LINE==bCardType)
	{
		m_tagTask[wChairID] |=CT_TASK_SHUN;
		if ( 3==m_GameLogic.GetCardValue(bCardData[bCardCount-1])|| 3==m_GameLogic.GetCardValue(bCardData[bCardCount-2]))
		{
			m_tagTask[wChairID] |=CT_TASK_SHUN_3;
		}    
		if (1==m_GameLogic.GetCardValue(bCardData[bCardCount-1]))
		{
			m_tagTask[wChairID] |=CT_TASK_SHUN_A;
		}
		if (8==bCardCount)
		{
			m_tagTask[wChairID] |=CT_TASK_SHUN_8;
		}
		if (12==bCardCount)
		{
			m_tagTask[wChairID] |=CT_TASK_SHUN_12;
		}
	}
	if (CT_DOUBLE==bCardType)
	{
		m_tagTask[wChairID] |=CT_TASK_DOUBLE;
		if (4==m_GameLogic.GetCardValue(bCardData[0]))
		{
			m_tagTask[wChairID] |=CT_TASK_DOUBLE_4;
		}
		if (11==m_GameLogic.GetCardValue(bCardData[0]))
		{
			m_tagTask[wChairID] |=CT_TASK_DOUBLE_J;
		}
		if (12==m_GameLogic.GetCardValue(bCardData[0]))
		{
			m_tagTask[wChairID] |=CT_TASK_DOUBLE_Q;
		}
	}
	if (CT_DOUBLE_LINE==bCardType)
	{
		m_tagTask[wChairID] |=CT_TASK_LIANDUI;
		for (int i=0;i<=bCardCount; i+=2)
		{
			BYTE bLastCard=0;
			bLastCard=m_GameLogic.GetCardValue(bCardData[i]);
			if (8==bLastCard)
			{
				m_tagTask[wChairID] |=CT_TASK_LIANDUI_8;
				break;
			}
		}
	}
	if ((CT_THREE_LINE_TAKE_ONE==bCardType || CT_THREE_LINE_TAKE_TWO==bCardType) )
	{
		m_tagTask[wChairID] |=CT_TASK_THREE_N;
	}
	if ((CT_THREE_LINE_TAKE_ONE==bCardType || CT_THREE_LINE_TAKE_TWO==bCardType || 
		CT_THREE_LINE==bCardType) && bCardCount>5)
	{
		m_tagTask[wChairID] |=CT_TASK_FEIJI;
	}
	if (CT_MISSILE_CARD==bCardType)
	{
		m_tagTask[wChairID] |=CT_TASK_HUOJIAN;
	}
	if (CT_BOMB_CARD==bCardType)
	{
		m_tagTask[wChairID] |=CT_TASK_ZHADAN;
		if (12==m_GameLogic.GetCardValue(bCardData[0]))
		{
			m_tagTask[wChairID] |=CT_TASK_ZHADAN_Q;
		}
	}
	if (CT_THREE==bCardType && 3==m_GameLogic.GetCardValue(bCardData[0]))
	{
		m_tagTask[wChairID] |=CT_TASK_THREE_3;
	}
	if (CT_SINGLE==bCardType)
	{
		if (3==m_GameLogic.GetCardValue(bCardData[0]))
		{
			m_tagTask[wChairID] |=CT_TASK_SINGLE_3;
		}
		if (9==m_GameLogic.GetCardValue(bCardData[0]))
		{
			m_tagTask[wChairID] |=CT_TASK_SINGLE_9;
		}
		if (1==m_GameLogic.GetCardValue(bCardData[0]))
		{
			m_tagTask[wChairID] |=CT_TASK_SINGLE_A;
		}
		if (13==m_GameLogic.GetCardValue(bCardData[0]))
		{
			m_tagTask[wChairID] |=CT_TASK_SINGLE_K;
		}
		if (14==m_GameLogic.GetCardValue(bCardData[0]) || 15==m_GameLogic.GetCardValue(bCardData[0]))
		{
			m_tagTask[wChairID] |=CT_TASK_SINGLE_KING;
		}
	}

	//更随出牌
	if (m_bTurnCardCount==0)
	{
		m_bTurnCardCount=bCardCount;
	}
	else if (m_GameLogic.CompareCard(m_bTurnCardData,bCardData,m_bTurnCardCount,bCardCount)==false) 
	{
		return false;
	}

	//删除扑克
	if (m_GameLogic.RemoveCard(bCardData,bCardCount,m_bHandCardData[wChairID],m_bCardCount[wChairID])==false) 
	{
		return false;
	}
	m_bCardCount[wChairID]-=bCardCount;

	//出牌记录
	m_bTurnCardCount=bCardCount;
	m_bOutCardCount[wChairID]++;
	CopyMemory(m_bTurnCardData,bCardData,sizeof(BYTE)*bCardCount);

	//炸弹判断
	if ((bCardType==CT_BOMB_CARD)||(bCardType==CT_MISSILE_CARD))
	{
		m_wBombTime*=2;
		m_wBombNums++;
		//if (CT_BOMB_CARD == bCardType)
		//{//打出炸弹赢了
		//	++m_UserTaskInfo[wChairID].bRegBombNums;
		//	m_UserTaskInfo[wChairID].bIsOutBombWin = true;
		//}
		//else if (CT_MISSILE_CARD == bCardType)
		//{//打出火箭赢了
		//	m_UserTaskInfo[wChairID].bIsOutMissile = true;
		//}
		//玩家倍数
		for (int i=0;i<GAME_PLAYER;++i)
		{
			m_bUserBeiShu[i]*=2;
			//m_OneBombNums[i]++;
		}
	}
	if (bCardType==CT_MISSILE_CARD)
	{
		m_IsMissile=true;
	}
	//切换用户
	m_wTurnWiner=wChairID;
	if (m_bCardCount[wChairID]!=0)
	{
		if (bCardType!=CT_MISSILE_CARD)
		{//不是火箭
			m_wCurrentUser=(m_wCurrentUser+1)%m_wPlayerCount;
		}
	}
	else
	{//牌出完，游戏结束.
		m_wCurrentUser=INVALID_CHAIR;
	}

	//构造数据
	CMD_S_OutCard OutCard;
	OutCard.bCardCount=bCardCount;
	OutCard.wOutCardUser=wChairID;
	OutCard.wCurrentUser=m_wCurrentUser;
	OutCard.wBombNums = m_wBombNums;//炸弹个数
	OutCard.bHandCardCount=m_bCardCount[wChairID];//手牌个数；
	CopyMemory(OutCard.bCardData,m_bTurnCardData,m_bTurnCardCount*sizeof(BYTE));
	CopyMemory(OutCard.bUserBeiShu,m_bUserBeiShu,sizeof(OutCard.bUserBeiShu));
	//---cxf
	//出牌次数
	++m_bCircuit[wChairID];
	OutCard.wBankerUser = m_wBankerUser;//庄家
	ZeroMemory(OutCard.bIsDouble,sizeof(OutCard.bIsDouble));
	CopyMemory(OutCard.bIsDouble,m_bDoubleUser,sizeof(OutCard.bIsDouble));
	CopyMemory(OutCard.bCircuit,m_bCircuit,sizeof(OutCard.bCircuit));
	//---end

	//发送数据
	//WORD wSendSize=sizeof(CMD_S_OutCard)-sizeof(OutCard.bCardData)+OutCard.bCardCount*sizeof(BYTE);
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_OUT_CARD,&OutCard,sizeof(CMD_S_OutCard));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_OUT_CARD,&OutCard,sizeof(CMD_S_OutCard));

	//出牌最大
	if (bCardType==CT_MISSILE_CARD)
	{//出的是火箭
		m_bTurnCardCount=0;
	}

	//结束判断
	if (m_wCurrentUser==INVALID_CHAIR)
	{
		OnEventGameEnd(wChairID,NULL,GER_NORMAL);
	}
	//服务器打印数据
	//char chData[128]={0};
	//sprintf(chData, "%x,%x,%x,%x,%x", m_bTurnCardData[0], m_bTurnCardData[1],m_bTurnCardData[2], m_bTurnCardData[3], m_bTurnCardData[4]);
	//CTraceService::TraceString(chData, TraceLevel_Normal);

	return true;
}

//用户放弃
bool CTableFrameSink::OnUserPassCard(WORD wChairID)
{
	//效验状态
	if (m_pITableFrame->GetGameStatus()!=GS_WK_PLAYING) return true;
	if ((wChairID!=m_wCurrentUser)||(m_bTurnCardCount==0)) return false;

	//设置变量
	m_wCurrentUser=(m_wCurrentUser+1)%m_wPlayerCount;
	if (m_wCurrentUser==m_wTurnWiner) m_bTurnCardCount=0;


	//发送数据
	CMD_S_PassCard PassCard;
	PassCard.wPassUser=wChairID;
	PassCard.wCurrentUser=m_wCurrentUser;
	PassCard.bNewTurn=(m_bTurnCardCount==0)?TRUE:FALSE;
	//---cxf
	//出牌次数.
	++m_bCircuit[wChairID];
	ZeroMemory(PassCard.bIsDouble,sizeof(PassCard.bIsDouble));
	CopyMemory(PassCard.bIsDouble,m_bDoubleUser,sizeof(PassCard.bIsDouble));
	CopyMemory(PassCard.bCircuit,m_bCircuit,sizeof(PassCard.bCircuit));
	PassCard.wBankerUser = m_wBankerUser;//庄家
	//---end
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_PASS_CARD,&PassCard,sizeof(PassCard));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_PASS_CARD,&PassCard,sizeof(PassCard));

	return true;
}

//add by Ivan_han
//服务器端替断线玩家出牌或不出或叫地主或抢地主
bool CTableFrameSink::OnAutoPassOrOutCard()
{
	int a=0;
	//游戏还没有结束
	if (m_wCurrentUser!=INVALID_CHAIR)
	{
		vector<WORD>::iterator p;
		p=find(m_vecChairIDOffLine.begin(),m_vecChairIDOffLine.end(),m_wCurrentUser);
		if(p!=m_vecChairIDOffLine.end())
		{
			BYTE b=m_pITableFrame->GetGameStatus();
			if(b==GS_WK_SCORE)//100
				this->OnUserLandScore(m_wCurrentUser,0);
			else if(b==GS_WK_SNATCH)//103
				this->OnUserSnatchLand(m_wCurrentUser,1);
			else if(b==GS_WK_PLAYING)//102
			{
				//判断其他两个椅子是否出了牌，如果都没出牌，则断线玩家必须出一张牌
				if(m_wCurrentUser!=m_wTurnWiner)
				{	
					bool bFlag=this->OnUserPassCard(m_wCurrentUser);
				}
				else
				{

					BYTE bCardData[1]={0};
					BYTE bCardCount=1;
					m_GameLogic.SortCardList(m_bHandCardData[m_wCurrentUser],m_bCardCount[m_wCurrentUser],ST_ORDER);
					bCardData[0]=m_bHandCardData[m_wCurrentUser][0];
					//m_GameLogic.RemoveCard(bCardData,bCardCount,m_bHandCardData[m_wCurrentUser],m_bCardCount[m_wCurrentUser]);
					this->OnUserOutCard(m_wCurrentUser,bCardData,bCardCount);
				}
			}

		}
	}
	return true;
}

//用户起来
bool __cdecl CTableFrameSink::OnActionUserStandUp(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	if ( ! bLookonUser )
	{
		//设置变量
		m_bBrightStart[wChairID] = false;
		m_bUserBrightCard[wChairID] = false;
		m_cbBrightCardTime=1;
		for ( WORD wBrightUser = 0; wBrightUser < GAME_PLAYER; ++wBrightUser )
			if ( m_bBrightStart[wBrightUser] ) m_cbBrightCardTime = BRIGHT_START_TIME;

		//发送消息
		OnBrightStart( wChairID, false );
	}

	return true;
}

//玩家托管
bool CTableFrameSink::OnUserTrustee(WORD wChairID, bool bTrustee)
{
	m_bUserTrustee[ wChairID ] = bTrustee;

	//变量定义
	CMD_C_UserTrustee UserTrustee;
	ZeroMemory( &UserTrustee, sizeof( UserTrustee ) );
	UserTrustee.wUserChairID = wChairID;
	UserTrustee.bTrustee = bTrustee;

	//发送消息
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_C_TRUSTEE, &UserTrustee,sizeof(UserTrustee));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_C_TRUSTEE, &UserTrustee,sizeof(UserTrustee));

	return true;
}

//明牌开始
bool CTableFrameSink::OnBrightStart( WORD wChairID , bool bBright )
{
	//设置变量
	m_bBrightStart[wChairID] = bBright;
	m_bUserBrightCard[wChairID] = bBright;

	if ( bBright ) m_cbBrightCardTime=BRIGHT_START_TIME;

	//设置玩家
	if ( bBright && m_wCurrentUser == INVALID_CHAIR )
	{
		m_wCurrentUser = wChairID;
		m_wBankerUser = wChairID;
	}

	//设置消息
	CMD_S_BrightStart BrightStart;
	ZeroMemory( &BrightStart, sizeof( BrightStart ) );
	BrightStart.wBrightUser = wChairID;
	BrightStart.bBright = bBright;

	//发送消息
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_BRIGHT_START,&BrightStart,sizeof(BrightStart));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_BRIGHT_START,&BrightStart,sizeof(BrightStart));
	return true;
}


//玩家明牌
bool  CTableFrameSink::OnBrightCard( WORD wChairID, BYTE cbBrightCardTime)
{
	//发送消息
	if ( 0 < cbBrightCardTime && cbBrightCardTime != 255 ) 
	{
		CHAR szMsg[128] ;
		if (m_pGameServiceOption->cbHideUserInfo)
			sprintf(szMsg, TEXT("玩家选择了明牌，积分翻%d倍"), cbBrightCardTime==5?1:cbBrightCardTime );
		else
			sprintf(szMsg, TEXT("[ %s ] 玩家选择了明牌，积分翻%d倍"), m_pITableFrame->GetServerUserItem( wChairID )->GetAccounts(), cbBrightCardTime==5?1:cbBrightCardTime );

		SendGameMessage( INVALID_CHAIR, szMsg );
	}

	//发牌明牌
	if ( m_bCallScorePhase != CSD_BRIGHTCARD )
	{
		ASSERT( cbBrightCardTime == BRIGHT_CARD_TIME );
		if ( cbBrightCardTime != BRIGHT_CARD_TIME ) return true;

		//倍数判断
		m_cbBrightCardTime = m_cbBrightCardTime > cbBrightCardTime ? m_cbBrightCardTime : cbBrightCardTime;

		//设置变量
		m_bUserBrightCard[wChairID] = cbBrightCardTime > 0 ? true:false;

		//设置消息
		CMD_S_BrightCard BrightCard;
		ZeroMemory( &BrightCard, sizeof( BrightCard ) );
		BrightCard.wBrightUser = wChairID;
		BrightCard.cbBrightTime = cbBrightCardTime;
		BrightCard.cbCurrenBrightTime = m_cbBrightCardTime;
		BrightCard.cbCallScorePhase = CSD_NORMAL;

		//发送消息
		m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_BRIGHT_CARD,&BrightCard,sizeof(BrightCard));
		m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_BRIGHT_CARD,&BrightCard,sizeof(BrightCard));
	}
	else
	{
		//效验参数
		ASSERT((cbBrightCardTime==5)||(cbBrightCardTime==255));
		if ((cbBrightCardTime!=5)&&(cbBrightCardTime!=255)) return false;

		//明牌判断
		if ( m_bUserBrightCard[wChairID] ) return true;

		//增加次数
		m_cbBrightCardCount++;
		if ( m_cbBrightCardCount == GAME_PLAYER )
		{
			m_bCallScorePhase = CSD_GAMESTART;
		}
		else 
		{
			m_bCallScorePhase = CSD_BRIGHTCARD;
		}

		//设置变量
		if ( cbBrightCardTime == 5 )
		{
			m_bLandScore++;
			m_bUserBrightCard[wChairID] = true;
		}

		//设置消息
		CMD_S_BrightCard BrightCard;
		ZeroMemory( &BrightCard, sizeof( BrightCard ) );
		BrightCard.wBrightUser = wChairID;
		BrightCard.cbBrightTime = cbBrightCardTime;
		BrightCard.cbCurrenBrightTime = m_cbBrightCardTime;
		BrightCard.cbCallScorePhase = CSD_BRIGHTCARD;

		//发送消息
		m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_BRIGHT_CARD,&BrightCard,sizeof(BrightCard));
		m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_BRIGHT_CARD,&BrightCard,sizeof(BrightCard));

		//结束判断
		if ( m_bCallScorePhase==CSD_GAMESTART )
		{
			//设置状态
			m_pITableFrame->SetGameStatus(GS_WK_DOUBLE_SCORE);

			//获取路径
			CHAR szDirectory[128];
			GetCurrentDirectory(sizeof( szDirectory ), szDirectory );

			//文件名称
			CHAR szProfileName[128];
			sprintf(szProfileName, TEXT("%s\\LandCrazy.ini"), szDirectory);

			//读取配置
			LONGLONG lDoubleLimit = GetPrivateProfileInt(TEXT("DoubleScore"), TEXT("Score"), 10000, szProfileName );

			bool bAllowDouble = true;
			for ( WORD wUserID = 0; wUserID < GAME_PLAYER; ++wUserID )
			{
				//获取玩家
				IServerUserItem *pIServerUserItem = m_pITableFrame->GetServerUserItem( wUserID );
				if ( pIServerUserItem == NULL ) continue;

				//条件判断
				LONGLONG lUserScore = pIServerUserItem->GetUserScore()->lGold;
				if ( lUserScore < lDoubleLimit )
				{
					bAllowDouble = false;
					break;
				}
			}
			m_bAllowDoubleScore = bAllowDouble;

			//发送底牌
			m_bCardCount[m_wBankerUser]=20;
			CopyMemory(&m_bHandCardData[m_wBankerUser][17],m_bBackCard,sizeof(m_bBackCard));
			m_GameLogic.SortCardList(m_bHandCardData[m_wBankerUser],m_bCardCount[m_wBankerUser],ST_ORDER);

			//设置消息
			CMD_S_DoubleScore DoubleScore;
			ZeroMemory( &DoubleScore, sizeof( DoubleScore ) );
			DoubleScore.bAllowDouble = bAllowDouble;
			DoubleScore.wLandUser = m_wBankerUser;
			DoubleScore.bCurrentScore = m_bLandScore;
			CopyMemory(DoubleScore.bBackCard,m_bBackCard,sizeof(m_bBackCard));

			//发送消息
			m_pITableFrame->SendTableData( INVALID_CHAIR, SUB_S_DOUBLE_SCORE, &DoubleScore, sizeof( DoubleScore ) );

			//设置变量
			if (m_bLandScore==0) m_bLandScore=1;
			if (m_wBankerUser==INVALID_CHAIR) m_wBankerUser=m_wFirstUser;

			return true;
		}

	}

	return true;
}

//玩家加倍
bool CTableFrameSink::OnUserDoubleScore( WORD wChairID, bool bDoubleScore, WORD bDoubleType)
{
	//加倍判断
	//if ( bDoubleScore ) m_bLandScore++;

	////发送消息
	//if ( bDoubleScore ) 
	//{
	//	//CHAR szMsg[128] ;
	//	//if (m_pGameServiceOption->cbHideUserInfo)
	//	//	//sprintf(szMsg, TEXT("玩家选择了翻倍，积分翻1倍"));
	//	//else
	//	//{
	//	//	IServerUserItem *pUserItem = m_pITableFrame->GetServerUserItem( wChairID );
	//	//	if (NULL == pUserItem)
	//	//	{
	//	//		return true;
	//	//	}
	//	//	tagServerUserData *pUserData = pUserItem->GetUserData();
	//	//	if (NULL != pUserData)
	//	//	{
	//	//		//sprintf(szMsg, TEXT("[ %s ] 玩家选择了翻倍，积分翻1倍"), pUserData->szNickName );
	//	//	}
	//	//}

	//	//SendGameMessage( INVALID_CHAIR, szMsg );
	//}
	//if (bDoubleScore)
	//{
	//	if (m_wBankerUser==wChairID)
	//	{
	//		for (int i=0; i<GAME_PLAYER;++i)
	//		{
	//			if (i!=m_wBankerUser)
	//			{
	//				if (m_bDoubleUser[i])
	//				{
	//					m_bUserBeiShu[i] *= 2;
	//				}
	//			}
	//		}
	//	}
	//	else
	//	{
	//		m_bDoubleUser[wChairID]=bDoubleScore;
	//		m_bUserBeiShu[wChairID] *= 2;
	//	}
	//}

	//m_bUserBeiShu[m_wBankerUser] = 0;// 地主倍数等于农民之和
	//for (int i=0; i<GAME_PLAYER;++i)
	//{
	//	if (i!=m_wBankerUser)
	//	{
	//		m_bUserBeiShu[m_wBankerUser] += m_bUserBeiShu[i];
	//	}
	//}
	//设置变量
	m_bFinishDoubleUser[wChairID] = true;
	m_bDoubleUser[wChairID] = false;
	if (bDoubleScore) 
	{
		m_bDoubleUser[wChairID] = true;
	}

	//构造消息 
	CMD_S_UserDouble UserDouble;
	ZeroMemory( &UserDouble, sizeof( UserDouble ) );
	UserDouble.bCurrentScore = m_bLandScore;
	UserDouble.bDoubleScore = bDoubleScore;
	UserDouble.wDoubleUser = wChairID;
	UserDouble.bDoubleType=bDoubleType;
	/*::memcpy(UserDouble.bBeiShu, m_bUserBeiShu, sizeof(m_bUserBeiShu));*/
	BYTE bNextUser = (wChairID+1)%GAME_PLAYER;
	UserDouble.bNextUser = bNextUser;
	if (wChairID==m_wBankerUser)
	{
		UserDouble.bState = 0; //结束加倍
	}
	else
	{
		if (bNextUser == m_wBankerUser)
		{
			UserDouble.bState = 3; //反
		}
		else
		{
			UserDouble.bState = 2; //跟
		}
	}

	//发送消息
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_USER_DOUBLE,&UserDouble,sizeof(UserDouble));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_USER_DOUBLE,&UserDouble,sizeof(UserDouble));

	/*cxf 倒跟反 替代加倍
	//统计次数
	m_cbDoubleCount++;

	//结束判断
	if ( m_cbDoubleCount == GAME_PLAYER )
	{
	//设置状态
	m_pITableFrame->SetGameStatus(GS_WK_PLAYING);	

	//出牌信息
	m_bTurnCardCount=0;
	m_wTurnWiner=m_wBankerUser;
	m_wCurrentUser=m_wBankerUser;

	//发送消息
	CMD_S_GameStart GameStart;
	GameStart.wLandUser=m_wBankerUser;
	GameStart.bLandScore=m_bLandScore;
	GameStart.bBrightCard=m_bBrightCard;
	GameStart.wCurrentUser=m_wCurrentUser;
	CopyMemory(GameStart.bBackCardData, m_bBackCard, 3);
	CopyMemory(GameStart.bUserBrightCard, m_bUserBrightCard, sizeof(GameStart.bUserBrightCard));
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_START,&GameStart,sizeof(GameStart));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_START,&GameStart,sizeof(GameStart));
	}
	*/
	return true;
}

//发送消息
void CTableFrameSink::SendGameMessage( WORD wChairID, CHAR szMsg[] )
{
	if ( wChairID == INVALID_CHAIR )
	{
		//游戏玩家
		for ( WORD wUserIdx = 0; wUserIdx < GAME_PLAYER; ++wUserIdx )
		{
			IServerUserItem *pUserItem = m_pITableFrame->GetServerUserItem( wUserIdx );
			m_pITableFrame->SendGameMessage( pUserItem, szMsg, SMT_INFO );
		}

		//旁观玩家
		WORD wLookonIdx = 0;
		IServerUserItem *pLookonUserItem = m_pITableFrame->EnumLookonUserItem(wLookonIdx);
		while( pLookonUserItem != NULL )
		{
			wLookonIdx++;
			m_pITableFrame->SendGameMessage( pLookonUserItem, szMsg, SMT_INFO );
			pLookonUserItem = m_pITableFrame->EnumLookonUserItem(wLookonIdx);
		}

	}
	else
	{
		IServerUserItem *pUserItem = m_pITableFrame->GetServerUserItem( wChairID );
		m_pITableFrame->SendGameMessage( pUserItem, szMsg, SMT_INFO );
	}
}

////===============================================================================
//任务ID
#define					LANDCRAZY_TASK_ID_1			11101                  //获胜并出完牌
#define					LANDCRAZY_TASK_ID_2			11102                  //打出顺子并出完牌
#define					LANDCRAZY_TASK_ID_3			11103                  //打出对子并出完牌
#define					LANDCRAZY_TASK_ID_4			11104                  //打出24倍以上并出完牌
#define					LANDCRAZY_TASK_ID_5			11105                  //打出单张3并出完牌
#define					LANDCRAZY_TASK_ID_6			11106                  //打出单张9并出完牌
#define					LANDCRAZY_TASK_ID_7			11107                  //打出单张K并出完牌
#define					LANDCRAZY_TASK_ID_8			11108                  //打出单张A并出完牌
#define					LANDCRAZY_TASK_ID_9			11109                  //打出单张王并出完牌
#define					LANDCRAZY_TASK_ID_10		11110                  //打出12倍以上并出完牌
#define					LANDCRAZY_TASK_ID_11		11201                  //打出带A的顺子并出完牌
#define					LANDCRAZY_TASK_ID_12		11202                  //打出8张的顺子并出完牌
#define					LANDCRAZY_TASK_ID_13		11203                  //打出对J并出完牌
#define					LANDCRAZY_TASK_ID_14		11204                  //打出3带N并出完牌
#define					LANDCRAZY_TASK_ID_15		11205                  //打出火箭并出完牌
#define					LANDCRAZY_TASK_ID_16		11206                  //打出48倍以上并出完牌
#define					LANDCRAZY_TASK_ID_17		11207                  //打出对4并出完牌
#define					LANDCRAZY_TASK_ID_18		11208                  //打出对Q并出完牌
#define					LANDCRAZY_TASK_ID_19		11209                  //打出带3的顺子并出完牌
#define					LANDCRAZY_TASK_ID_20		11210                  //打出3条3并出完牌
#define					LANDCRAZY_TASK_ID_21		11301                  //底牌抓两相同
#define					LANDCRAZY_TASK_ID_22		11302                  //底牌抓单张2
#define					LANDCRAZY_TASK_ID_23		11303                  //底牌抓大王
#define					LANDCRAZY_TASK_ID_24		11304                  //打出12张的顺子并出完牌
#define					LANDCRAZY_TASK_ID_25		11305                  //打出连对并出完牌
#define					LANDCRAZY_TASK_ID_26		11306                  //打出带8的连对并出完牌
#define					LANDCRAZY_TASK_ID_27		11307                  //打出飞机并出完牌
#define					LANDCRAZY_TASK_ID_28		11308                  //打出炸弹并出完牌
#define					LANDCRAZY_TASK_ID_29		11309                  //打出带Q的炸弹并出完牌
#define					LANDCRAZY_TASK_ID_30		11310                  //打出96倍以上并出完牌
////===============================================================================
//
//
//
//
//
////玩家任务判断
//void CTableFrameSink::OnUserTaskJudgement(WORD wChairID, WORD wBandID, BYTE btTag)
//{
//	//wChairID:玩家ID; wBandID:当前庄家ID; btTag:1:赢,0:输.
//	if (wChairID>=GAME_PLAYER || wBandID>=GAME_PLAYER || btTag>1)
//	{
//		return;
//	}
//	//首先获取wChairID玩家任务序号.
//	IServerUserItem *pUserItem = m_pITableFrame->GetServerUserItem( wChairID );
//	if (NULL == pUserItem)
//	{
//		return;
//	}
//	tagServerUserData *pUserData = pUserItem->GetUserData();
//	if (NULL == pUserData)
//	{
//		return;
//	}
//	//pUserData->task.iTaskID:任务ID
//	if (1==btTag)
//	{//wChairID赢了
//		bool bIsWin = false;
//		switch(pUserData->task.iTaskID)
//		{
//		case LANDCRAZY_TASK_ID_1:	//任务1: 拿到地主并且在牌局中胜利
//			{
//				if (wChairID==wBandID)
//				{//任务完成.
//					bIsWin = true;
//				}
//			}
//			break;
//		case LANDCRAZY_TASK_ID_2:	//任务2: 在游戏中打出炸弹并且胜利
//			{
//				if (true==m_UserTaskInfo[wChairID].bIsOutBombWin)
//				{//任务完成.
//					bIsWin = true;
//				}
//			}
//			break;
//		case LANDCRAZY_TASK_ID_3:	//任务3: 在游戏中打出火箭并且胜利
//			{
//				if (true==m_UserTaskInfo[wChairID].bIsOutMissile)
//				{//任务完成.
//					bIsWin = true;
//				}
//			}
//			break;
//		case LANDCRAZY_TASK_ID_4:	//任务4: 在同一局游戏中出现两次以上炸弹并且胜利
//			{
//				if (m_UserTaskInfo[wChairID].bRegBombNums>=2)
//				{//任务完成
//					bIsWin = true;
//				}
//			}
//			break;
//		case LANDCRAZY_TASK_ID_5:	//任务5: 在斗地主三次连续获胜
//			{
//				++m_UserTaskInfo[wChairID].iRegJointWinNums;
//				pUserData->task.count = m_UserTaskInfo[wChairID].iRegJointWinNums;
//				m_pITableFrame->PefermTaskCount(wChairID);
//				if (m_UserTaskInfo[wChairID].iRegJointWinNums>=3)
//				{//任务完成
//					bIsWin = true;
//				}
//			}
//			break;
//		case LANDCRAZY_TASK_ID_6:	//任务6: 在斗地主中连续三次拿到地主并且连续获胜
//			{
//				++m_UserTaskInfo[wChairID].iRegJointWinNums;
//				pUserData->task.count = m_UserTaskInfo[wChairID].iRegJointWinNums;
//				m_pITableFrame->PefermTaskCount(wChairID);
//				if (m_UserTaskInfo[wChairID].iRegJoineLandNums>=3 && m_UserTaskInfo[wChairID].iRegJointWinNums>=3)
//				{//任务完成
//					bIsWin = true;
//				}
//			}
//			break;
//		case LANDCRAZY_TASK_ID_7:	//任务7: 在斗地主中连续10次获胜
//			{
//				++m_UserTaskInfo[wChairID].iRegJointWinNums;
//				pUserData->task.count = m_UserTaskInfo[wChairID].iRegJointWinNums;
//				m_pITableFrame->PefermTaskCount(wChairID);
//				if (m_UserTaskInfo[wChairID].iRegJointWinNums>=10)
//				{//任务完成
//					bIsWin = true;
//				}
//			}
//			break;
//		default:
//			break;
//		}
//		if (true == bIsWin)
//		{//任务完成.
//			//提交任务完成
//			m_pITableFrame->PefermTaskFinish(wChairID);
//			//清空任务信息
//			memset(&m_UserTaskInfo[wChairID], 0, sizeof(tag_Server_TaskInfo));
//		}
//	}
//	else
//	{//输了
//		switch(pUserData->task.dwID)
//		{
//		case LANDCRAZY_TASK_ID_5:	//任务5: 在斗地主三次连续获胜
//		case LANDCRAZY_TASK_ID_6:	//任务6: 在斗地主中连续三次拿到地主并且连续获胜
//		case LANDCRAZY_TASK_ID_7:	//任务7: 在斗地主中连续10次获胜
//			{
//				//把连续赢的次数清零。
//				m_UserTaskInfo[wChairID].iRegJointWinNums = 0;
//				pUserData->task.count = 0;
//				m_pITableFrame->PefermTaskCount(wChairID);
//			}
//			break;
//		default:
//			break;
//		}
//	}
//	
//}
////////////////////////////////////////////////////////////////////////////
//玩家任务判断
int CTableFrameSink::OnUserTaskJudgement(WORD wChairID, WORD wBandID)
{
	//wChairID:玩家ID; wBandID:当前庄家ID;
	if (wChairID>=GAME_PLAYER || wBandID>=GAME_PLAYER )
	{
		return 0;
	}

	//首先获取wChairID玩家任务序号.
	IServerUserItem *pUserItem = m_pITableFrame->GetServerUserItem( wChairID );
	if (NULL == pUserItem)
	{
		return 0;
	}
	tagServerUserData *pUserData = pUserItem->GetUserData();
	if (NULL == pUserData)
	{
		return 0;
	}
	bool bIsFinish=false;
	switch (pUserData->task.iTaskID)
	{
	case LANDCRAZY_TASK_ID_22: //底牌抓单张2；
		{
			if (m_tagTask[wChairID] & CT_TASK_DP_TWO)
			{
				bIsFinish=true;
			}
		}
		break;
	case LANDCRAZY_TASK_ID_23: //底牌抓大王；
		{
			if (m_tagTask[wChairID] & CT_TASK_DP_KING)
			{
				bIsFinish=true;
			}
		}
		break;
	case LANDCRAZY_TASK_ID_21: //底牌抓两相同；
		{
			if (m_tagTask[wChairID] & CT_TASK_DP_SAME)
			{
				bIsFinish=true;
			}
		}

	case LANDCRAZY_TASK_ID_1: //获胜并出完牌；
		{
			if (m_tagTask[wChairID] & CT_TASK_WIN && m_tagTask[wChairID] &CT_TASK_CARD_ZERO)
			{
				bIsFinish=true;
			}
		}
		break;
	case LANDCRAZY_TASK_ID_2: //打出顺子并出完牌；
		if (m_tagTask[wChairID] & CT_TASK_SHUN && m_tagTask[wChairID] &CT_TASK_CARD_ZERO)
		{
			bIsFinish=true;
		}
		break;
	case LANDCRAZY_TASK_ID_19: //打出带3的顺子并出完牌；
		if (m_tagTask[wChairID] & CT_TASK_SHUN_3 && m_tagTask[wChairID] &CT_TASK_CARD_ZERO)
		{
			bIsFinish=true;
		}
		break;
	case LANDCRAZY_TASK_ID_11: //打出带A的顺子并出完牌；
		if (m_tagTask[wChairID] & CT_TASK_SHUN_A && m_tagTask[wChairID] &CT_TASK_CARD_ZERO)
		{
			bIsFinish=true;
		}
		break;
	case LANDCRAZY_TASK_ID_12://打出8张的顺子并出完牌；
		if (m_tagTask[wChairID] & CT_TASK_SHUN_8 && m_tagTask[wChairID] &CT_TASK_CARD_ZERO)
		{
			bIsFinish=true;
		}
		break;
	case LANDCRAZY_TASK_ID_24://打出12张的顺子并出完牌；
		if (m_tagTask[wChairID] & CT_TASK_SHUN_12 && m_tagTask[wChairID] &CT_TASK_CARD_ZERO)
		{
			bIsFinish=true;
		}
		break;
	case LANDCRAZY_TASK_ID_3: //打出对子并出完牌；
		if (m_tagTask[wChairID] & CT_TASK_DOUBLE && m_tagTask[wChairID] &CT_TASK_CARD_ZERO)
		{
			bIsFinish=true;
		}
		break;
	case LANDCRAZY_TASK_ID_13: //打出对J并出完牌；
		if (m_tagTask[wChairID] & CT_TASK_DOUBLE_J && m_tagTask[wChairID] &CT_TASK_CARD_ZERO)
		{
			bIsFinish=true;
		}
		break;
	case LANDCRAZY_TASK_ID_17: //打出对4并出完牌；
		if (m_tagTask[wChairID] & CT_TASK_DOUBLE_4 && m_tagTask[wChairID] &CT_TASK_CARD_ZERO)
		{
			bIsFinish=true;
		}
		break;
	case LANDCRAZY_TASK_ID_18: //打出对Q并出完牌；
		if (m_tagTask[wChairID] & CT_TASK_DOUBLE_Q && m_tagTask[wChairID] &CT_TASK_CARD_ZERO)
		{
			bIsFinish=true;
		}
		break;
	case LANDCRAZY_TASK_ID_25: //打出连对并出完牌；
		if (m_tagTask[wChairID] & CT_TASK_LIANDUI && m_tagTask[wChairID] &CT_TASK_CARD_ZERO)
		{
			bIsFinish=true;
		}
		break;
	case LANDCRAZY_TASK_ID_26: //打出带8的连对并出完牌；
		if (m_tagTask[wChairID] & CT_TASK_LIANDUI_8 && m_tagTask[wChairID] &CT_TASK_CARD_ZERO)
		{
			bIsFinish=true;
		}

		break;
	case LANDCRAZY_TASK_ID_14: //打出3带N并出完牌；
		if (m_tagTask[wChairID] & CT_TASK_THREE_N && m_tagTask[wChairID] &CT_TASK_CARD_ZERO)
		{
			bIsFinish=true;
		}
		break;
	case LANDCRAZY_TASK_ID_27: //打出飞机并出完牌；
		if (m_tagTask[wChairID] & CT_TASK_FEIJI && m_tagTask[wChairID] &CT_TASK_CARD_ZERO)
		{
			bIsFinish=true;
		}
		break;
	case LANDCRAZY_TASK_ID_28: //打出炸弹并出完牌；
		if (m_tagTask[wChairID] & CT_TASK_ZHADAN && m_tagTask[wChairID] &CT_TASK_CARD_ZERO)
		{
			bIsFinish=true;
		}
		break;
	case LANDCRAZY_TASK_ID_15: //打出火箭并出完牌；
		if (m_tagTask[wChairID] & CT_TASK_HUOJIAN && m_tagTask[wChairID] &CT_TASK_CARD_ZERO)
		{
			bIsFinish=true;
		}
		break;
	case LANDCRAZY_TASK_ID_29: //打出带Q的炸弹并出完牌；
		if (m_tagTask[wChairID] & CT_TASK_ZHADAN_Q && m_tagTask[wChairID] &CT_TASK_CARD_ZERO)
		{
			bIsFinish=true;
		}
		break;
	case LANDCRAZY_TASK_ID_20: //打出三条3并出完牌；
		if (m_tagTask[wChairID] & CT_TASK_THREE_3 && m_tagTask[wChairID] &CT_TASK_CARD_ZERO)
		{
			bIsFinish=true;
		}
		break;
	case LANDCRAZY_TASK_ID_10: //打出12倍以上并出完牌；
		if (m_tagTask[wChairID] & CT_TASK_BEI_12 && m_tagTask[wChairID] &CT_TASK_CARD_ZERO)
		{
			bIsFinish=true;
		}
		break;
	case LANDCRAZY_TASK_ID_4: //打出24倍以上并出完牌；
		if (m_tagTask[wChairID] & CT_TASK_BEI_24 && m_tagTask[wChairID] &CT_TASK_CARD_ZERO)
		{
			bIsFinish=true;
		}
		break;
	case LANDCRAZY_TASK_ID_16: //打出48倍以上并出完牌；
		if (m_tagTask[wChairID] & CT_TASK_BEI_48 && m_tagTask[wChairID] &CT_TASK_CARD_ZERO)
		{
			bIsFinish=true;
		}
		break;
	case LANDCRAZY_TASK_ID_30: //打出96倍以上并出完牌；
		if (m_tagTask[wChairID] & CT_TASK_BEI_96 && m_tagTask[wChairID] &CT_TASK_CARD_ZERO)
		{
			bIsFinish=true;
		}
		break;
	case LANDCRAZY_TASK_ID_5: //打出单张3并出完牌；
		if (m_tagTask[wChairID] & CT_TASK_SINGLE_3 && m_tagTask[wChairID] &CT_TASK_CARD_ZERO)
		{
			bIsFinish=true;
		}
		break;
	case LANDCRAZY_TASK_ID_6: //打出单张9并出完牌；
		if (m_tagTask[wChairID] & CT_TASK_SINGLE_9 && m_tagTask[wChairID] &CT_TASK_CARD_ZERO)
		{
			bIsFinish=true;
		}
		break;
	case LANDCRAZY_TASK_ID_8: //打出单张A并出完牌；
		if (m_tagTask[wChairID] & CT_TASK_SINGLE_A && m_tagTask[wChairID] &CT_TASK_CARD_ZERO)
		{
			bIsFinish=true;
		}
		break;
	case LANDCRAZY_TASK_ID_7: //打出单张K并出完牌；
		if (m_tagTask[wChairID] & CT_TASK_SINGLE_K && m_tagTask[wChairID] &CT_TASK_CARD_ZERO)
		{
			bIsFinish=true;
		}
		break;
	case LANDCRAZY_TASK_ID_9: //打出单张王并出完牌；
		if (m_tagTask[wChairID] & CT_TASK_SINGLE_KING && m_tagTask[wChairID] &CT_TASK_CARD_ZERO)
		{
			bIsFinish=true;
		}
		break;
	default:
		break;
	}

	int iEncourageGem=0;
	bool iServerFinish=false;
	if (true == bIsFinish)
	{//任务完成.
		if (m_pGameServiceOption->wSortID==1)
		{//一级房间宝石奖励
			iEncourageGem=pUserData->task.iLargessCount1;
		}
		else if (m_pGameServiceOption->wSortID==2)
		{//二级房间宝石奖励
			iEncourageGem=pUserData->task.iLargessCount2;
		}
		else if (m_pGameServiceOption->wSortID==3)
		{//三级房间宝石奖励
			iEncourageGem=pUserData->task.iLargessCount3;
		}
		else //四级房间宝石奖励
		{
			iEncourageGem=pUserData->task.iLargessCount4;
		}
	}
	//提交任务完成
	m_pITableFrame->QuestTaskFinish(wChairID,iEncourageGem);
	return iEncourageGem;	
}
