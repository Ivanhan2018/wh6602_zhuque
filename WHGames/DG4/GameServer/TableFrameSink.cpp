

#include "TableFrameSink.h"

//////////////////////////////////////////////////////////////////////////

//静态变量
const WORD			CTableFrameSink::m_wPlayerCount=GAME_PLAYER;			//游戏人数
const enStartMode	CTableFrameSink::m_GameStartMode=enStartMode_FullReady;	//开始模式

//////////////////////////////////////////////////////////////////////////

void TraceMessage(LPCTSTR pszMessage);

//构造函数
CTableFrameSink::CTableFrameSink()
{
	//组件变量
	m_pITableFrame=NULL;
	m_pGameServiceOption=NULL;

	

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

IServerUserItem *  CTableFrameSink::GetServerUserItem(WORD wChairID)
{
	if (m_pITableFrame) return m_pITableFrame->GetServerUserItem(wChairID);

	return NULL;
}

bool CTableFrameSink::SendTableData(WORD wChairID, WORD wSubCmdID, void * pData, WORD wDataSize)
{
	if (m_pITableFrame) return m_pITableFrame->SendTableData(wChairID, wSubCmdID, pData, wDataSize);

	return false;
}

bool CTableFrameSink::SendTableDataToAll(WORD wSubCmdID, void * pData, WORD wDataSize)
{
	if (m_pITableFrame) return m_pITableFrame->SendTableData(INVALID_CHAIR, wSubCmdID, pData, wDataSize);

	return false;
}

void CTableFrameSink::SendTableDataToOther(WORD wChairID, WORD wSubCmdID, void * pData, WORD wDataSize)
{
	for (WORD i=0;i<m_wPlayerCount;i++)
	{
		if(i==wChairID)continue;

		IServerUserItem * pIServerUserItem = m_pITableFrame->GetServerUserItem(i);
		if (pIServerUserItem != NULL)
		{
			m_pITableFrame->SendUserData(pIServerUserItem, wSubCmdID, pData, wDataSize);
		}
	}
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
	ASSERT(wChairID<m_wPlayerCount);
	return (m_cbPlayStatus[wChairID]==TRUE)?true:false;
}

//用户坐下 
bool __cdecl CTableFrameSink::OnActionUserSitDown(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	return true;
}

//用户起来
bool __cdecl CTableFrameSink::OnActionUserStandUp(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)
{	
	if(bLookonUser || m_wDUser==INVALID_CHAIR)return true;
	for (WORD i=0;i<m_wPlayerCount;i++)
	{
		if(i==wChairID)continue;
		if (m_pITableFrame->GetServerUserItem(i)!=NULL)
		{
			break;
		}
	}


	return true;
}

//游戏开始
bool __cdecl CTableFrameSink::OnEventGameStart()
{
	//设置状态
	m_pITableFrame->SetGameStatus(GS_PLAYING);

	//游戏变量
	WORD wUserCount=0;
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		//获取用户
		IServerUserItem * pIServerUserItem=m_pITableFrame->GetServerUserItem(i);

		//无效用户
		if (pIServerUserItem==NULL) continue;

		//获取积分
		const tagUserScore * pUserScore=pIServerUserItem->GetUserScore();
		ASSERT(pUserScore->lScore>=m_pGameServiceOption->lCellScore);

		//设置状态
		m_cbPlayStatus[i]=TRUE;
		wUserCount++;
	}

//	CMD_S_GameStart_CB GameStart;
//	m_pITableFrame->SendTableData(0,SUB_S_GAME_START,cbBuffer,bos.length());

//	m_GameLogic.OnStartGame(GameStart);

	m_pITableFrame->SetGameTimer(GAME_TIMER, 1000, -1, 0);

	OnStartGame();
/*
	//变量定义
	char cbBuffer[SOCKET_PACKET];
	memset(cbBuffer, 0, SOCKET_PACKET);
	bostream bos;
	bos.attach(cbBuffer, SOCKET_PACKET);
	bos << GameStart;

	m_pITableFrame->SendTableData(0,SUB_S_GAME_START,cbBuffer,bos.length());
*/
	return true;
}

//游戏结束
bool __cdecl CTableFrameSink::OnEventGameEnd(WORD wChairID, IServerUserItem * pIServerUserItem, unsigned char cbReason)
{
	switch (cbReason)
	{
	case GER_NORMAL:		//常规结束
		{
			return true;
		}
		break;
	case GER_USER_LEFT:
		{
			//效验参数
			ASSERT(pIServerUserItem!=NULL);
			ASSERT(wChairID<m_wPlayerCount);

			//强退处理
//			OnUserGiveUp(wChairID);
			return true;
		}
		break;
	}

	return false;
}

//发送场景
bool __cdecl CTableFrameSink::SendGameScene(WORD wChiarID, IServerUserItem * pIServerUserItem, unsigned char cbGameStatus, bool bSendSecret)
{
	switch (cbGameStatus)
	{
	case GS_FREE:		//空闲状态
		{
			//构造数据
			DG4::CMD_S_StatusFree StatusFree;
			ZeroMemory(&StatusFree,sizeof(StatusFree));

			//游戏变量
			StatusFree.cbStatus=cbGameStatus;
			StatusFree.lCellMinScore=m_pGameServiceOption->lCellScore;
			StatusFree.lCellMaxScore=m_pGameServiceOption->lRestrictScore;

			//发送场景
			return m_pITableFrame->SendGameScene(pIServerUserItem,&StatusFree,sizeof(StatusFree));	
		}
	case GS_PLAYING:	//游戏状态
		{
			/* 
			这种做法不全面，暂时放弃

			//构造数据
			CMD_S_StatusPlay StatusPlay;
			ZeroMemory(&StatusPlay,sizeof(StatusPlay));

			StatusPlay.cbStatus=cbGameStatus;
			StatusPlay.lCellMinScore=m_pGameServiceOption->lCellScore;
			StatusPlay.lCellMaxScore=m_pGameServiceOption->lRestrictScore;
			
			//发送场景
			return m_pITableFrame->SendGameScene(pIServerUserItem,&StatusPlay,sizeof(StatusPlay));
			*/

			WORD chairID = wChiarID;
			BOOL isReLink = TRUE;//这个现在暂时先写死（后续在考虑看牌的情况）
			
			unsigned char * buff = new unsigned char[sizeof(BOOL)+sizeof(WORD)];
			memcpy(buff,0,sizeof(BOOL)+sizeof(WORD));
			
			bostream bos;
			bos.attach((char*)buff, sizeof(BOOL)+sizeof(WORD));
			bos << chairID;
			bos << isReLink;

			OnGameAction(SUB_S_RELINK,buff,bos.length());//视为游戏逻辑消息，交给sendGameStatus去处理
			delete []buff;
			return true;

		}

	default :
		return true;

	}
	//效验结果
	ASSERT(FALSE);
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
	switch(wSubCmdID)
	{
	case SUB_CS_OUT:		//出牌
		{
			//用户效验
			tagServerUserData * pUserData=pIServerUserItem->GetUserData();
			if (pUserData->cbUserStatus!=US_PLAY) return true;

			//状态判断
			ASSERT(m_cbPlayStatus[pUserData->wChairID]==TRUE);
			if (m_cbPlayStatus[pUserData->wChairID]==FALSE) return false;

/*			//变量定义
			CMD_OutCard_CB outCards;
			bistream bis;
			bis.attach((char*)pDataBuffer, wDataSize);
			bis >> outCards;
*/
			//消息处理
			OnGameAction(SUB_CS_OUT, pDataBuffer, wDataSize);
/*
			//成功后发给其他玩家
			//变量定义
			char cbBuffer[SOCKET_PACKET];
			memset(cbBuffer, 0, SOCKET_PACKET);
			bostream bos;
			bos.attach(cbBuffer, SOCKET_PACKET);
			bos << outCards;

			m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_CS_OUT, cbBuffer, bos.length());
			m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_CS_OUT, cbBuffer, bos.length());

			//for test
			//结束游戏
			m_pITableFrame->ConcludeGame();
*/

			//消息处理
			return true;
		}
	case SUB_CS_ROAR:	//用户吼牌
		{
			//用户效验
			tagServerUserData * pUserData=pIServerUserItem->GetUserData();
			if (pUserData->cbUserStatus!=US_PLAY) return true;
			//状态判断
			ASSERT(m_cbPlayStatus[pUserData->wChairID]==TRUE);
			if (m_cbPlayStatus[pUserData->wChairID]==FALSE) return false;

			//消息处理
			OnGameAction(SUB_CS_ROAR, pDataBuffer, wDataSize);

			return true;
		}
	case SUB_S_TUOGUAN:
		{
			//用户效验
			tagServerUserData * pUserData=pIServerUserItem->GetUserData();
			if (pUserData->cbUserStatus!=US_PLAY) return true;
			
			//状态判断
			ASSERT(m_cbPlayStatus[pUserData->wChairID]==TRUE);
			if (m_cbPlayStatus[pUserData->wChairID]==FALSE) return false;

			//消息处理
			OnGameAction(SUB_S_TUOGUAN, pDataBuffer, wDataSize);
			return true;
		}

	default:
		return true;
	}
	return false;
}

//框架消息处理
bool __cdecl CTableFrameSink::OnFrameMessage(WORD wSubCmdID, const void * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	return false;
}

void CTableFrameSink::stovs( const string& s, const string& sp, vector<string>& sa )
{
	sa.clear();
	if (sp.length() == 0) return;
	if (s.length() == 0)
	{
		/*sa.push_back("");*/
		return;
	}

	size_t szsp = sp.length();
	string tmps = s + sp;
	size_t start = 0;
	while( true )
	{
		if( tmps.length() < sp.length() )
		{
			break;
		}
		if( tmps.length() < start + sp.length() )
		{
			break;
		}

		size_t idx = tmps.find_first_of( sp[0], start );
		string sub = tmps.substr( idx, szsp );
		if( sub == sp )
		{
			string item = tmps.substr( 0, idx );
			sa.push_back( item );
			start = 0;
			tmps = tmps.substr( idx + szsp, tmps.length() - ( idx + szsp ) );
		}
		else
		{
			start += idx;
		}
	}
}


//输出信息
void TraceMessage(LPCTSTR pszMessage)
{
	if(0 == lstrlen(pszMessage))
	{
		return;
	}

	CFile File;
	if ((File.Open(TEXT("TraceData_DG4.txt"),CFile::modeWrite)==FALSE)&&
		(File.Open(TEXT("TraceData_DG4.txt"),CFile::modeWrite|CFile::modeCreate)==FALSE))
	{
		ASSERT(FALSE);
		return;
	}

	SYSTEMTIME sys; 
	GetLocalTime( &sys ); 
	char buf[128];
	memset(buf, 0, 128);
	sprintf(buf, "%4d/%02d/%02d %02d:%02d:%02d.%03d ",sys.wYear,sys.wMonth,sys.wDay,sys.wHour,sys.wMinute, sys.wSecond,sys.wMilliseconds);

	File.SeekToEnd();
	File.Write(buf,lstrlen(buf));
	File.Write(pszMessage,lstrlen(pszMessage));
	File.Write(TEXT("\r\n"),lstrlen(TEXT("\r\n")));

	File.Flush();
	File.Close();

	return;
}


//输出信息
void TraceMessageDetails(LPCTSTR pszMessage, LPCTSTR pszFile)
{
	if(0 == lstrlen(pszMessage) || 0 == lstrlen(pszFile) )
	{
		return;
	}

	SYSTEMTIME sys; 
	GetLocalTime( &sys ); 
	char buf[128];
	memset(buf, 0, 128);
	sprintf(buf, "%4d/%02d/%02d %02d:%02d:%02d.%03d ",sys.wYear,sys.wMonth,sys.wDay,sys.wHour,sys.wMinute, sys.wSecond,sys.wMilliseconds);

	//文件名
	char fnBuf[128];
	memset(fnBuf, 0, 128);
	sprintf(fnBuf, "Log//%4d%02d%02d_%s",sys.wYear,sys.wMonth,sys.wDay, pszFile);
	
	CFile File;
	if ((File.Open(fnBuf,CFile::modeWrite)==FALSE)&&
		(File.Open(fnBuf,CFile::modeWrite|CFile::modeCreate)==FALSE))
	{
		ASSERT(FALSE);
		return;
	}

	File.SeekToEnd();
	File.Write(buf,lstrlen(buf));
	File.Write(pszMessage,lstrlen(pszMessage));
	File.Write(TEXT("\r\n"),lstrlen(TEXT("\r\n")));

	File.Flush();
	File.Close();

	return;
}
//////////////////////////////////////////////////////////////////////////
