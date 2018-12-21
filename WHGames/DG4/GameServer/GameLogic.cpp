
#include "TableFrameSink.h"
#include "GameLogic.h"

#include <iostream>
#include <fstream>

#include "PokerDefine.h"
using namespace PokerDef;

extern void TraceMessage(LPCTSTR pszMessage);
extern void TraceMessageDetails(LPCTSTR pszMessage, LPCTSTR pszFile);
namespace DG4
{


#ifdef _DEBUG
#define TRUST_TIME				3
#define ROAR_TIME				15
#define PLAY_TIME				20
#define FIRST_START_PLAY_TIME	20
#else
#define TRUST_TIME				3
#define ROAR_TIME				15
#define PLAY_TIME				20
#define FIRST_START_PLAY_TIME	20
#endif


//任务ID定义
#define TASK_HEI_510K		1
#define TASK_HONG_510K		2
#define TASK_MEI_510K		3
#define TASK_FANG_510K		4
#define TASK_4_2			5
#define TASK_4_5			6
#define TASK_4_10			7
#define TASK_4_K			8
#define TASK_7_ZHA			9
#define TASK_FIRST_PLANE	10
#define TASK_FIRST_4_STR	11
#define TASK_FIRST_STR_678	12
#define TASK_FIRST_STR_789	13
#define TASK_FIRST_STR_910	14
#define TASK_FIRST_STR_10J	15
#define TASK_FIRST_STR_QK	16
#define TASK_FIRST_STR_KA	17
#define TASK_LAST_HEI_510K	18
#define TASK_LAST_HONG_510K	19
#define TASK_LAST_MEI_510K	20
#define TASK_LAST_THREE_5	21
#define TASK_LAST_THREE_10	22
#define TASK_LAST_THREE_K	23
#define TASK_LAST_PLANE		24
#define TASK_LAST_4_STR		25
#define TASK_LAST_STR_678	26
#define TASK_LAST_STR_789	27
#define TASK_LAST_STR_910	28
#define TASK_LAST_STR_10J	29
#define TASK_LAST_STR_QK	30
#define TASK_LAST_STR_KA	31
#define TASK_LAST_3			32
#define TASK_YAO_BAI		33

#define TASK_ZHUA_TIAN		34
#define TASK_ZHUA_8_XI		35
#define TASK_ZHUA_7_XI		36

extern string g_strRoomRule;  //房间规则

#define PAI_NUM		2

#define SEND_TABLE_DATA(chair,ID,msg) do{char cbBuffer[SOCKET_PACKET];\
	if (g_iDebug > 200)Sleep(g_iDebug);	\
	memset(cbBuffer, 0, SOCKET_PACKET);\
	bostream bos;\
	bos.attach(cbBuffer, SOCKET_PACKET);\
	bos<<msg;SendTableData(chair,ID,cbBuffer,bos.length());}while(0)

#define SEND_TABLE_DATA_ALL(ID,msg) do{char cbBuffer[SOCKET_PACKET];\
	if (g_iDebug > 200)Sleep(g_iDebug);	\
	memset(cbBuffer, 0, SOCKET_PACKET);\
	bostream bos;\
	bos.attach(cbBuffer, SOCKET_PACKET);\
	bos<<msg;SendTableDataToAll(ID,cbBuffer,bos.length());}while(0)

#define SEND_TABLE_DATA_OTHER(chair,ID,msg) do{char cbBuffer[SOCKET_PACKET];\
	if (g_iDebug > 200)Sleep(g_iDebug);	\
	memset(cbBuffer, 0, SOCKET_PACKET);\
	bostream bos;\
	bos.attach(cbBuffer, SOCKET_PACKET);\
	bos<<msg;SendTableDataToOther(chair,ID,cbBuffer,bos.length());}while(0)


static int g_iDebug = 0;
static int g_iLog = 0;

#define LOGS(chair, msg)	do{\
	if (g_iLog > 0)	\
	{	\
		IServerUserItem* item = GetServerUserItem(chair);	\
		if (item)	\
		{	\
			char fn[64];	\
			memset(fn, 0 ,64);	\
			sprintf(fn, "%d_%d.txt",m_pGameServiceOption->wServerPort,item->GetTableID());	\
			char id[64];	\
			memset(id, 0 ,64);	\
			sprintf(id, "userid=%u", item->GetUserID());	\
			TraceMessageDetails(id, fn);	\
			TraceMessageDetails(msg, fn);	\
		}	\
	}	\
}while(0)

CGameLogic::CGameLogic(void)
{
	//初始化下一个庄家位置
	m_nextbanker = 0;

	//初始化所有牌点数
	for (int i=0;i< MAX_POKER_COUNTS;i++)
	{
		for(int j = 0;j< PAI_NUM;j++)
			m_dbp[i+j*MAX_POKER_COUNTS] = i+1;
	}

	//初始化所有牌点数，供做牌的时候使用
	for (unsigned char i = 0 ; i < ALLCARD ; i++)
	{
		m_tempdbp[i] = m_dbp[i];
	}
	m_specialAward = 0;
	Reset();

	//初始化谁断线了，牌数，财富数据
	for(int i = 0; i < MAX_PLAYER;i++)
	{
		m_whoBreak[i]  = 0; 
		m_thePaiCount[i] = MAXCARD;
		m_playerrich[i] = 0;
	}
	memset(m_lastScore,0,sizeof(m_lastScore));
	memset(m_total,0,sizeof(m_total));

	m_StartMakePai = FALSE;

	m_nRoarTime = ROAR_TIME;
	m_nPlayTime = PLAY_TIME;
	m_nAutoOutTime = TRUST_TIME;


}


CGameLogic::~CGameLogic(void)
{
	
}

//定时器事件
bool __cdecl CGameLogic::OnTimerMessage(WORD wTimerID, WPARAM wBindParam)
{
	
	if (wTimerID == GAME_TIMER)
	{
		if (m_tDownTime > 0 &&
			m_theActionStep > AS_NULL && 
			m_theActionStep < AS_COUNT)
		{
			if (GetCPUTickCount() > m_tDownTime)
			{
				SetActionStep(m_theActionStep);
			}
		}
	}

	return false;
}

//游戏结束
bool __cdecl CGameLogic::OnEventGameEnd(WORD wChairID, IServerUserItem * pIServerUserItem, unsigned char cbReason)
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
			GameOver(wChairID, GOT_ESCAPE);
			return true;
		}
		break;
	}

	return false;
}
//用户断线
bool CGameLogic::OnActionUserOffLine(WORD wChairID, IServerUserItem * pIServerUserItem)
{
	if (wChairID >= 0 && wChairID < MAX_PLAYER)
	{
		m_whoBreak[wChairID] = TRUE;
	}

	return true;
}

//用户重入
bool CGameLogic::OnActionUserReConnect(WORD wChairID, IServerUserItem * pIServerUserItem)
{
	if (wChairID >= 0 && wChairID < MAX_PLAYER)
	{
		m_whoBreak[wChairID] = FALSE;
	}

	return true;
}

bool __cdecl CGameLogic::SendGameScene(WORD wChiarID, IServerUserItem * pIServerUserItem, unsigned char cbGameStatus, bool bSendSecret)
{
	switch (cbGameStatus)
	{
	case GS_FREE:		//空闲状态
		{
			//构造数据
			CMD_S_StatusFree StatusFree;
			ZeroMemory(&StatusFree,sizeof(StatusFree));

			//游戏变量
			StatusFree.cbStatus=cbGameStatus;
			StatusFree.lCellMinScore=m_pGameServiceOption->lCellScore;
			StatusFree.lCellMaxScore=m_pGameServiceOption->lRestrictScore;

			CMD_CS_GameRule msgRule;
			msgRule.theBase = m_pGameServiceOption->lCellScore;
			msgRule.theGiveGift = m_specialAward;
			msgRule.theMinRich = m_iFaOfTao;
			
			SEND_TABLE_DATA(wChiarID,SUB_CS_GAMERULE,msgRule);

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
			

			char buff[16];
			
			bostream bos;
			bos.attach((char*)buff, 16);
			bos << chairID;
			bos << isReLink;

			OnGameAction(SUB_S_RELINK,buff,bos.length());//视为游戏逻辑消息，交给sendGameStatus去处理
			
			return true;

		}

		
	default :
		return true;
	}
	//效验结果
	ASSERT(FALSE);
	return false;
}

void CGameLogic::Reset()
{
	InitGame();
	time(&nowTime);
}

void CGameLogic::InitGame()
{
	for(int i =0;i < MAX_PLAYER;i++)
	{
		//逃跑
		if(m_isPlayerEscape[i])
		{
			m_lastScore[i] = 0;			
			m_total[i] = 0;				
		}
		m_isPlayerEscape[i]  = FALSE;	
		GameWinLostSR[i]	= 0;		
		GameWinLostJF[i]	= 0;

		//牌数据
		for(int j = 0;j < MAXCARD; j++)
		{
			if (m_StartMakePai == FALSE)
			{
				m_dbplay[i][j]	= 0;
			}
			m_allPaiOut[i][j]	= 0;
			m_lastPaiOut[i][j]	= 0;
		}
		m_thePaiCount[i] = MAXCARD;
		//托管
		m_TrustCounts[i]    = 0;
		m_TuoGuanPlayer[i]	= 0;
		//断线
		m_WireBreakCounts[i] = 0;

		/////////////
		m_whoReady[i]		= 0;
		m_playerWin[i]		= FALSE;
		m_bTeamOut[i]		= 0;

		m_playerscore[i]	= 0;
		m_whoAllOutted[i]	= 0;
		m_playerTurn[i]		= 0xff;
		m_whoBreak[i]		= 0;
		m_whoPass[i]		= 0;

		m_who7Xi[i]			= 0;
		m_who8Xi[i]			= 0;

	}
	m_tDownTime     = 0;
	m_RoarPai		= 0;
	m_WhoRoar		= 0;
	m_whoplay		= 0;
	m_whoplayout	= 0;
	m_banker		= 0;
	m_parter		= 0;
	m_GameType		= GT_NULL;
	m_nowActionStep = AS_NULL;
	m_theActionStep = AS_NULL;
	m_lastOutType   = TYPE_NULL;
	m_GameState     = gsNull;
	m_bMingJi		= false;

	m_score			= 0;
	m_turn			= 0;
	m_whoHasKingBomb = 0;//谁有天炸
	srand(time(0));

	m_iJiangOfTao = 3;		//默认值3
	m_iFaOfTao = 9;			//默认值9


	//初始化随机任务
	m_iRandomTask = -1;
	m_bHaveOutCard = false;
	m_mapAllTask.clear();
	m_vecAllTaskID.clear();
	for (int j=0 ; j<MAX_PLAYER ; j++)
	{
		m_vecFinishedTask[j].clear();
	}


	//解析游戏规则，格式：n1;v1|n2:v2
	vector<string> vecParam;
	stovs(g_strRoomRule, "|", vecParam);
	for (int i=0; i<vecParam.size(); i++)
	{
		vector<string> nv;
		stovs(vecParam[i], ":", nv);
		if (nv.size() == 2)
		{
			if (nv[0] == "Fa")
			{
				m_iFaOfTao = atoi(nv[1].c_str());
			}
			else if (nv[0] == "Jiang")
			{
				m_iJiangOfTao = atoi(nv[1].c_str());
			}
			else if (nv[0] == "TaskID")
			{
				ParseTaskConfig(nv[1].c_str());
			}
			else if (nv[0] == "D")
			{
				g_iDebug = atoi(nv[1].c_str());
			}
			else if (nv[0] == "G")
			{
				g_iLog = atoi(nv[1].c_str());
			}
			else if (nv[0] == "K")
			{
				m_specialAward = atoi(nv[1].c_str());
			}
		}
	}

}

void CGameLogic::OnGameAction(WORD wSubCmdID, const void * pDataBuffer, WORD wDataSize)
{
	switch(wSubCmdID)
	{
		case SUB_CS_ROAR:	//用户吼牌
			{
				//变量定义
				CMD_Roar_CB roar;
				bistream bis;
				bis.attach((char*)pDataBuffer, wDataSize);
				bis >> roar;

//				SEND_TABLE_DATA_ALL( SUB_CS_ROAR, roar);
				if (g_iLog > 0)
				{
					char buf[64];
					memset(buf, 0 ,64);
					sprintf(buf, "CMD_Roar_CB.theFlag=%d", roar.theFlag);
						
					LOGS(roar.theSeat, buf);
				}

				//逻辑处理
				OnRoarAction(roar.theSeat, roar.theFlag);
			}
			break;
		case SUB_CS_OUT:	//用户出牌
			{
				//变量定义
				CMD_OutCard_CB outCards;
				bistream bis;
				bis.attach((char*)pDataBuffer, wDataSize);
				bis >> outCards;

				if (g_iLog > 0)
				{
					char buf[128];
					memset(buf, 0 ,128);
					sprintf(buf, "CMD_OutCard_CB m_GameState=%d outCards.theCount=%d", m_GameState, outCards.theCount);
					string tmp(buf);
					for (size_t i = 0; i<outCards.theCount; i++)
					{
						memset(buf, 0 ,64);
						sprintf(buf, "card[%d]=%d", i, outCards.thePai[i]);
						tmp += buf;
					}
						
					LOGS(outCards.theSeat, tmp.c_str());
				}

				//校验规则
				if(m_GameState != gsPlay)
				{
					char buf[125] = {0};
					sprintf(buf, "%d: 游戏状态不正确", outCards.theSeat);
					TraceMessage(buf);
					return;
				}

				if(outCards.theCount < 0 || outCards.theCount > MAXCARD)
				{//错误
					char buf[125] = {0};
					sprintf(buf, "%d: 出牌数据错误,出牌的长度非法", outCards.theSeat);
					TraceMessage(buf);
					return;
				}

				AutoOutCard(outCards.theSeat, outCards);

	//			m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_CS_OUT, cbBuffer, bos.length());
	//			m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_CS_OUT, cbBuffer, bos.length());

				//for test
				//结束游戏
	//			m_pITableFrame->ConcludeGame();
			}
			break;
		case SUB_S_TUOGUAN: //托管
			{
				//变量定义
				CMD_S_Tuoguan_CB tuoguan;
				bistream bis;
				bis.attach((char*)pDataBuffer, wDataSize);
				bis >> tuoguan;

				//详细日志
				if (g_iLog > 0)
				{
					char buf[128];
					memset(buf, 0 ,128);
					sprintf(buf, "CMD_S_Tuoguan_CB m_whoplay=%d tuoguan.theSeat=%d tuoguan.theFlag=%d", m_whoplay, tuoguan.theSeat,tuoguan.theFlag);
						
					LOGS(tuoguan.theSeat, buf);
				}

				//校验规则
				if((m_GameState == gsPlay) || (m_GameState == gsRoarPai))
				{
					if(tuoguan.theFlag == CMD_S_Tuoguan_CB::T_TUOGUAN)
					{	
						m_TuoGuanPlayer[tuoguan.theSeat] = TRUE;
						m_TrustCounts[tuoguan.theSeat]++;
						if(tuoguan.theSeat + 1  == m_whoplay){
							m_tDownTime = GetCPUTickCount()+m_nAutoOutTime;
						}
						SEND_TABLE_DATA_ALL(SUB_S_TUOGUAN,tuoguan);
					}
					else if(tuoguan.theFlag == CMD_S_Tuoguan_CB::T_NULL){
						time_t now;
						time(&now);
						m_TuoGuanPlayer[tuoguan.theSeat] = FALSE;

						//如果是当前的玩家，那么重新设置一下开始时间
						if(tuoguan.theSeat + 1 == m_whoplay)
						{
							m_tDownTime = m_nPlayTime-(now-nowTime)+GetCPUTickCount();
						}

						tuoguan.theTime = m_nPlayTime-(now-nowTime);
						SEND_TABLE_DATA_ALL(SUB_S_TUOGUAN,tuoguan);
					}
					else 
						return ;
				}
				else
				{
					char buf[125] = {0};
					sprintf(buf, "%d: 游戏状态不正确", tuoguan.theSeat);
					TraceMessage(buf);
					return;
				}
			}
			break;
		case SUB_S_RELINK:
			{			
			    WORD chairID;
				BOOL isReLink;
				bistream bis;
				bis.attach((char*)pDataBuffer, wDataSize);
				bis >> chairID;
				bis >> isReLink;

				//详细日志
				if (g_iLog > 0)
				{
					char buf[128];
					memset(buf, 0 ,128);
					sprintf(buf, "SUB_S_RELINK chairID=%d isReLink=%d", chairID,isReLink);
						
					LOGS(chairID, buf);
				}

				sendGameStatus(chairID,isReLink);
			}
		default:
			break;
	}
}

void CGameLogic::OnStartGame()
{
	InitGame();
	m_GameState = gsNull;
	SetActionStep(AS_NULL);

	m_nBase = m_pGameServiceOption->lCellScore;
	m_nSerPay = m_pGameServiceOption->wRevenue;


	int m,n;
	//洗牌
	for(int i=0; i<1000; i++)
	{
		m = div(rand(),MAX_POKER_COUNTS*PAI_NUM).rem;
		n = div(rand(),MAX_POKER_COUNTS*PAI_NUM).rem;
		unsigned char zz = m_dbp[m];
		m_dbp[m] = m_dbp[n];
		m_dbp[n] = zz;
	}

	if (m_StartMakePai == FALSE)
	{
		for(int i =0;i<MAX_PLAYER;i++)
		{
			for(int j = 0; j < MAXCARD; j++)
			{
				m_dbplay[i][j] = 0;
			}
		}
		
		for(int i=0; i < MAXCARD;i++)
		{
			for(int k=0; k < MAX_PLAYER; k++)  //发牌
			{
				m_dbplay[k][i] = m_dbp[MAX_PLAYER*i+k];					
			}
		}

if (g_iDebug > 0)
{
		//从文件中读，循环赋值
		vector<string> vecCase;
		//读取配置
		ifstream readfile;
		readfile.open("cbdagun4_test.txt",ios::in);
		while(!readfile.eof())
		{
			char ch[512];
			memset(ch, 0, 512);
			readfile.getline(ch,512,'\n');
			vecCase.push_back(ch);
		}
		readfile.close();

		int iTC = g_iDebug;
		//造牌,200时，进行用例循环。否则，使用指定用例
		if (g_iDebug == 200)
		{
			static int iCaseStart = 0;
			++iCaseStart;
			if (iCaseStart > vecCase.size())iCaseStart = 1;
			iTC = iCaseStart;
		}

		if (iTC > vecCase.size() || iTC < 1)iTC = 1;

		if (vecCase.size() >= iTC)
		{
			//解析牌，格式：n1,n2|m1,m2
			vector<string> vecParam;
			stovs(vecCase[iTC-1], "|", vecParam);
			for (int j=0; j<vecParam.size() && j<MAX_PLAYER; j++)
			{
				//二次解析
				vector<string> vecCards;
				stovs(vecParam[j], ",", vecCards);
				for (int k=0; k<MAXCARD && k<vecCards.size(); k++)
				{
					m_dbplay[j][k] = atoi(vecCards[k].c_str());
				}
			}
		}
}

	}
	SendPaiCount();

	if(m_nextbanker > 0 && m_nextbanker <= MAX_PLAYER )
	{
		m_whoplay = m_nextbanker;
	}
	else
	{
		m_whoplay = rand()%MAX_PLAYER + 1;
	}

	m_banker = m_whoplay;

	for(int i = 0; i < MAX_PLAYER; i++)
	{
	/*
		if(m_pTable->getPlayer(i)->getVipID() > 0)
		{
			m_spay[i] = m_nBase*m_nVipSerPay/1000;
		}
		else
		*/
		{
			m_spay[i] = m_nBase*m_nSerPay/100;
		}
	}

	CMD_S_GameStart_CB startmsg;

	startmsg.theIsGameStart = TRUE;
	startmsg.theBanker = m_whoplay;
	startmsg.theType = CMD_S_GameStart_CB::GAME_START;

	//这里检查天炸，7，8，喜
	for(int i = 0; i < MAX_PLAYER; i++)
	{
		m_who8Xi[i] = m_dagun.Get8XibombNum(m_dbplay[i]);
		m_who7Xi[i] = m_dagun.Get7XibombNum(m_dbplay[i]);
		if(m_whoHasKingBomb == 0 && m_dagun.HasKingBomb(m_dbplay[i]))
			m_whoHasKingBomb = i+1;
	}

	for(int seat = 0; seat < MAX_PLAYER; seat++)
	{
//		m_playerrich[seat] = player->getPlayTypeScore();
		startmsg.theSeat = seat;
		startmsg.theCount = MAXCARD;
		
		for(int i = 0;i < MAXCARD; i++)
		{
			startmsg.theCards[i] = m_dbplay[startmsg.theSeat][i];
		}

		//详细日志
		if (g_iLog > 0)
		{
			char buf[32];
			string tmp;
			for(int i = 0;i < MAXCARD; i++)
			{
				memset(buf, 0 ,32);
				sprintf(buf, "card[%d]=%d ", i, m_dbplay[startmsg.theSeat][i]);
				tmp += buf;
			}

			LOGS(startmsg.theSeat, tmp.c_str());
		}


		SEND_TABLE_DATA(seat,SUB_S_GAME_START,startmsg);
	}

	StartRoar(m_whoplay-1);

	//发送随机任务
	SendTaskID(true);
}

void CGameLogic::StartRoar(unsigned char theSeat)
{
	// 开始进入吼牌状态
	m_GameState = gsRoarPai;
	DWORD dwPower = CMD_S_Power_CB::p_Roar;
	SendPower(theSeat + 1, dwPower, m_nRoarTime);
}

void CGameLogic::EndRoar(bool bRoar)
{
	CMD_S_EndRoar_CB endroarmsg;

	//有人吼牌
	if(bRoar)
	{
		m_WhoRoar = m_whoplay;
		endroarmsg.theWhoRoar = m_WhoRoar;
		m_GameType = GT_ROAR;
	}
	//没人吼牌
	else
	{
		m_GameType = GT_NORMAL;
		m_WhoRoar = 0;
		endroarmsg.theWhoRoar = m_WhoRoar;
		GetJiaoPai();//得到叫牌
	}
	//吼牌的为庄家了
	if(bRoar)
	{
		m_banker = m_WhoRoar;
		m_parter = 0;
	}
	m_whoplay = m_banker;
	endroarmsg.theBanker = m_banker;
	endroarmsg.thePai = m_RoarPai;

	SEND_TABLE_DATA_ALL(SUB_S_ENDROAR, endroarmsg);

	StartPlay(m_whoplay);
}

void CGameLogic::StartPlay( unsigned char whoplay )
{
	m_score = 0;
	// 开始
	m_GameState = gsPlay;

	DWORD dwPower = CMD_S_Power_CB::p_Out;
	SendPower(whoplay,dwPower,FIRST_START_PLAY_TIME);
}

unsigned char CGameLogic::GetJiaoPai()
{
	unsigned char temp[MAXCARD];
	for(int i =0;i<MAXCARD;i++)
	{
		temp[i] = 0;
	}
	int count = 0;
	for(int i = 1; i < CARDINDEX_BACK; i++)
	{
		int num = 0;
		for(int j = 0; j < MAXCARD; j++)
		{
			if(m_dbplay[m_banker-1][j] == i)
			{
				num++;
			}
		}
		if(num == 1)
		{
			temp[count++] = i;
		}
	}

	int seed = rand()%count;
	m_RoarPai = temp[seed];

	for(int i =0; i < MAX_PLAYER; i++)
	{
		if(i == m_banker-1)
			continue;
		for(int j = 0;j < MAXCARD; j++)
		{
			if(m_dbplay[i][j] == m_RoarPai)
			{
				m_parter = i+1;//明鸡
				break;
			}
		}
		if(m_parter > 0 && m_parter <= MAX_PLAYER)
			break;
	}
	return m_RoarPai;
}
//发送声音
void CGameLogic::SendPlaySoundMsg(unsigned char seat,unsigned char type)
{
    CMD_CS_msgPlaySound_CB soundmsg;

    soundmsg.theSeat = seat;
    soundmsg.thePaiType = type;

    SEND_TABLE_DATA_ALL(SUB_CS_PLAYSOUND,soundmsg);
}

// 发送权限
void CGameLogic::SendPower( unsigned char whoplay,DWORD aPower,DWORD dwWaitTime /*= 0*/ )
{
	///////////////////////////////////////////////////////////////////////////////////////////////////////
	CMD_S_Power_CB power;

	DWORD nTime = dwWaitTime ? dwWaitTime : 0;
	power.theWhoPlay	= whoplay;
	power.thePower		= aPower;
	if(m_TuoGuanPlayer[whoplay-1])
	{
		if(nTime > m_nAutoOutTime)
		{
			nTime = m_nAutoOutTime;
		}
	}
	power.theDownTime	= nTime;

	//详细日志
	if (g_iLog > 0)
	{
		char buf[64];
		memset(buf, 0 ,64);
		sprintf(buf, "SUB_S_POWER seat=%d power=%d,time=%u ", power.theWhoPlay, power.thePower, power.theDownTime);

		LOGS(power.theWhoPlay, buf);
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////////
	SEND_TABLE_DATA_ALL(SUB_S_POWER,power);


	///////////////////////////////////////////////////////////////////////////////////////////////////////
	if (m_GameState == gsPlay)
		SetActionStep(AS_PLAY,nTime + 1);//设置等待时间，服务端多等一下
	else if(m_GameState == gsRoarPai)
		SetActionStep(AS_ROAR,nTime + 1);//设置等待时间，服务端多等一下
}


int CGameLogic::AutoTuoGuan(unsigned char theSeat)
{
	CMD_S_Tuoguan_CB msgtg;
	msgtg.theFlag = CMD_S_Tuoguan_CB::T_TUOGUAN;
	msgtg.theSeat = theSeat;

	if(m_GameState == gsNull)
		return 0;

	//详细日志
	if (g_iLog > 0)
	{
		char buf[128];
		memset(buf, 0 ,128);
		sprintf(buf, "CMD_S_Tuoguan_CB AutoTuoGuan msgtg.theFlag=%d msgtg.theSeat=%d ", msgtg.theFlag, msgtg.theSeat);

		LOGS(msgtg.theSeat, buf);
	}

	if (m_GameState == gsPlay || m_GameState == gsRoarPai)
	{
		if (CMD_S_Tuoguan_CB::T_TUOGUAN == msgtg.theFlag)
		{
			m_TuoGuanPlayer[theSeat] = TRUE;
			m_TrustCounts[theSeat]++;
			if( theSeat+1 == m_whoplay)
			{//如果是当前的玩家，那么重新设置一下开始时间
				m_tDownTime = GetCPUTickCount()+m_nAutoOutTime;
			}
			SEND_TABLE_DATA_ALL(SUB_S_TUOGUAN,msgtg);
		}
		else if (CMD_S_Tuoguan_CB::T_CANCEL == msgtg.theFlag)
		{
			m_TuoGuanPlayer[theSeat] = FALSE;
			SEND_TABLE_DATA_ALL(SUB_S_TUOGUAN,msgtg);
		}
		else
			return 0;
	}
	return 1;
}

void CGameLogic::SetActionStep(ActionStep as, unsigned long nSecond)
{
	if (as == AS_NULL || as >= AS_COUNT)
	{
		m_theActionStep = AS_NULL;
		m_nowActionStep = AS_NULL;
		m_tDownTime = 0;
	}
	else
	{
		if (nSecond > 0)
		{
			m_theActionStep = as;
			m_nowActionStep = as;
			m_tPowerStartTime = GetCPUTickCount();
			m_tDownTime = GetCPUTickCount() + (time_t)nSecond;
		}
		else
		{
			m_theActionStep = AS_NULL;
			m_nowActionStep = as;
			m_tPowerStartTime = 0;
			m_tDownTime = 0;
			OnActionStep(as);
		}
	}
}
void CGameLogic::OnActionStep(ActionStep as)
{
	if(as == AS_ROAR)
	{
		if(m_whoplay > 0 && m_whoplay <= MAX_PLAYER
			&& !m_TuoGuanPlayer[m_whoplay-1])
		{
			AutoTuoGuan(m_whoplay-1);
		}

		OnRoarAction(m_whoplay-1,FALSE);
	}
	else  if(as == AS_PLAY)
	{
		if(m_whoplay > 0 && m_whoplay <= MAX_PLAYER
			&& !m_TuoGuanPlayer[m_whoplay-1])
		{
			AutoTuoGuan(m_whoplay-1);
		}

		unsigned char buf[MAXCARD];
		for(int i =0; i < MAXCARD;i++)
			buf[i] = 0;
		CMD_OutCard_CB outmsg;
		outmsg.theSeat = m_whoplay-1;
		if(m_whoplayout == 0)
		{
			for(int i =0; i < MAXCARD;i++)
				buf[i] = 0;
			int n = 0;
			for(int i=0;i<MAXCARD;i++) 
			{
				if(m_dbplay[m_whoplay-1][i] > 0 && m_dbplay[m_whoplay-1][i] < 55)
					buf[n++] = m_dbplay[m_whoplay-1][i];
			}
			m_dagun.SortByIndex(buf);
			outmsg.theCount = 1;
			outmsg.thePai[0] = buf[0];
			outmsg.thePaiType = TYPE_ONE;
		}
		else
		{
			outmsg.theCount = 0;
		}

		AutoOutCard(m_whoplay-1,outmsg);
	}
}

//硬牌动作响应
void CGameLogic::OnRoarAction(unsigned char seat,unsigned char bRoar)
{
	if(seat < 0 || seat >= MAX_PLAYER)
		return;
	if(m_whoReady[seat])
		return;
	m_whoReady[seat] = TRUE;	

	//变量定义
	CMD_Roar_CB roar;
	roar.theFlag = bRoar;
	roar.theSeat = seat;

	SEND_TABLE_DATA_ALL( SUB_CS_ROAR, roar);

	if(bRoar)
	{//如果硬牌，那么结束硬牌动作
		EndRoar(true);
		return;
	}
	else
	{
		int z = 0;
		for(int i =0; i < MAX_PLAYER; i++)
		{
			if(m_whoReady[i])
			{
				z++;
			}
		}
		if(z == MAX_PLAYER)
		{//开始游戏
			EndRoar(false);
		}
		else
		{
			GoNextPlayer();
		}
	}
}


long CGameLogic::GetCPUTickCount( void )
{
	time_t now;
	time(&now);
	return now;
}


void CGameLogic::GoNextPlayer()
{
	///////////////////////////////////////////////////////////////////////////////////////////////////////
	while (TRUE)
	{
		if (m_whoplay==MAX_PLAYER) 
		{
			m_whoplay = 1;
		}
		else 
		{
			m_whoplay++;
		}

		//如果当前玩家出完了
		if(m_whoAllOutted[m_whoplay-1])
		{
			if(m_whoplayout == m_whoplay)
			{//这个玩家是不是上一次出牌玩家
				if(EndTurn())
					return;//结束游戏

				if(m_GameState == gsRoarPai)
				{
					StartRoar(m_whoplay-1);
				}
				else if(m_GameState == gsPlay)
				{
					DWORD dwPower = CMD_S_Power_CB::p_Out;
					SendPower(m_whoplay,dwPower,m_nPlayTime);
					time(&nowTime);
				}

				for(int i =0; i< MAXCARD; i++)
				{
					m_allPaiOut[m_whoplay-1][i] = 0;
				}
				return;
			}
			else
			{
				continue;
			}
		}
		else
		{//没出完？
			break;
		}
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////////
	if(m_whoplayout == m_whoplay)
	{
		if(EndTurn())
			return;

		if(m_GameState == gsRoarPai)
		{
			StartRoar(m_whoplay-1);
		}
		else if(m_GameState == gsPlay)
		{
			DWORD dwPower = CMD_S_Power_CB::p_Out;
			SendPower(m_whoplay,dwPower,m_nPlayTime);
			time(&nowTime);
		}
	}
	else
	{
		if(m_GameState == gsRoarPai)
		{
			StartRoar(m_whoplay-1);
		}
		else if(m_GameState == gsPlay)
		{
			DWORD dwPower = CMD_S_Power_CB::p_Out;
			SendPower(m_whoplay,dwPower,m_nPlayTime);
			time(&nowTime);
		}
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////////
	for(int i =0; i< MAXCARD; i++)
	{
		m_allPaiOut[m_whoplay-1][i] = 0;
	}
}

//出牌动作
int CGameLogic::AutoOutCard(unsigned char theSeat,const CMD_OutCard_CB& msgout)
{
	if( theSeat != m_whoplay -1)
	{
		char buf[256] = {0};
		sprintf(buf, "%d: AutoOutCard error: 出牌玩家不是当前玩家", theSeat);
		TraceMessage(buf);
		return 0;
	}
	///////////////////////////////////////////////////////////////////////////////////////////////////////
	//出牌数目为0，则为放弃的情况
	if(msgout.theCount == 0)
	{
		if(m_whoplayout != 0 && m_whoplay == theSeat + 1)
		{
			CMD_OutCard_CB outmsg;
			outmsg.theCount = 0;
			outmsg.theSeat = msgout.theSeat;
			for(int i = 0; i < MAXCARD; i ++)
			{
				outmsg.thePai[i] = 0;
				m_allPaiOut[m_whoplay-1][i] = 0;
				m_lastPaiOut[m_whoplay-1][i] = 0;
			}

			SEND_TABLE_DATA_ALL(SUB_CS_OUT,outmsg);

			m_whoPass[m_whoplay-1] = TRUE;
			GoNextPlayer();
		}

		return 1;
	}

	unsigned char buf[MAXCARD];

	for(int i = 0 ; i < MAXCARD ; i++)
		buf[i] = m_dbplay[m_whoplay-1][i];
	int z = 0;
	for(int i = 0 ; i < msgout.theCount ; i++)
	{
		for(int j = 0 ; j < MAXCARD ; j++)
		{
			if(msgout.thePai[i] == buf[j])
			{
				buf[j] = 0;
				z++;
				break;
			}
		}
	}
	if(z == msgout.theCount)
	{
		TCardType re1,re2;
		int iNumOfKing = 0;
		for(int i=0;i<MAXCARD;i++)
			m_allPaiOut[m_whoplay-1][i] = msgout.thePai[i];

		//TYPE_NULL代表第一个出
		if(m_whoplayout==0)
		{
			re1.type = TYPE_NULL;
			m_lastOutType = TYPE_NULL;
		}
		else 
		{
			int len = m_dagun.GetCardNum(m_allPaiOut[m_whoplayout-1]);
			re1 = m_dagun.GetType(m_allPaiOut[m_whoplayout-1],len);
			//这里为什么要重新设置下呢？因为很可能出现不同的判断
			//比如A走了334455,B走了44王王王5,那么当C走的时候，GetType很可能把B的牌型判断为444555这样的大牌型
			//所以，在这里，一定要把牌型设置回来，根据客户端传过来的为依据
			re1.type = m_lastOutType;
		}
		re2 = m_dagun.GetType(m_allPaiOut[m_whoplay-1],msgout.theCount,m_lastOutType);
		iNumOfKing = m_dagun.GetKingNum(m_allPaiOut[m_whoplay-1],msgout.theCount);

	
		m_whoPass[m_whoplay-1] = FALSE;
		if(m_dagun.Compare(re1,re2))
		{	
			//管得起才能算分
			if(m_GameType == GT_NORMAL)
			{
				m_score += m_dagun.GetScore(m_allPaiOut[m_whoplay-1],msgout.theCount);
				SendTurnScore();
			}
			else
			{
				m_score = 0;
			}

			m_lastOutType = re2.type;

			//在这里，re2.type 是一定要等于lpMsg->thePaiType的，否则就是错误的
			if(re2.type != msgout.thePaiType && m_whoplayout != 0)
			{
				char buf[125] = {0};
				sprintf(buf, "UserID=%d: error:re2.type != lpMsg->thePaiType", theSeat);
				TraceMessage(buf);
			}
			if(m_GameType == GT_NORMAL && m_whoplay != m_banker)
			{
				for(int i = 0; i < msgout.theCount; i++)
				{
					if(msgout.thePai[i] == m_RoarPai)
					{
						if(m_whoplay-1 >= 0 && m_whoplay-1 < MAX_PLAYER)
						{
							CMD_S_Show_CB showmsg;

							m_bMingJi = true;
							showmsg.theFlag = CMD_S_Show_CB::F_MINGJI;
							showmsg.theSeat = m_whoplay-1;

							SEND_TABLE_DATA_ALL(SUB_S_SHOW, showmsg);
						}
						break;
					}
				}
			}

			m_thePaiCount[m_whoplay-1] -= msgout.theCount;

			m_whoplayout = m_whoplay;

			int z = 0;
			for(int i=0;i<MAXCARD;i++) 
			{
				m_dbplay[m_whoplay-1][i] = buf[i];
				if(buf[i]>0) z++;
			}
			SendPaiCount();
			int type = m_dagun.CheckBeep(m_dbplay[m_whoplay-1], m_allPaiOut[m_whoplay-1],msgout.theCount);
//			msgout.theNoSound = (type > 0 ? 1:0);
//			msgout.theNoSound = (z == 1 ? 1:0);
			SEND_TABLE_DATA_ALL(SUB_CS_OUT, msgout);

			//插底提示
			if(z == 1)
			{
				SendPlaySoundMsg(m_whoplay-1,CMD_CS_msgPlaySound_CB::TY_ChaDi);
			}
			else
			{
				if( type > 0 )
				{
					if(type == 1)
					{
						SendPlaySoundMsg(m_whoplay-1,CMD_CS_msgPlaySound_CB::TY_7Xi_BEEP);
					}
					else if(type ==2)
					{
						SendPlaySoundMsg(m_whoplay-1,CMD_CS_msgPlaySound_CB::TY_8Xi_BEEP);
					}
					else if(type == 3)
					{
						SendPlaySoundMsg(m_whoplay-1,CMD_CS_msgPlaySound_CB::TY_KING_BEEP);
					}
				}
			}

			//判断任务是否完成
			IsTaskFinished(re2, theSeat, iNumOfKing);
			//end 

			//硬牌，有一个没有牌了就结束游戏
			if(m_GameType == GT_ROAR)
			{
				//结束游戏
				if(z==0)
				{
					//判断最后一手的任务是否完成
					IsTaskFinishedOfLastHand(re2, theSeat, iNumOfKing);
					//end

					m_nextbanker = m_whoplay;
					GameOver(m_whoplay-1,GOT_NORMAL);
				}
				else
				{
					GoNextPlayer();
				}
			}
			//不硬牌，找朋友的模式
			else if(m_GameType == GT_NORMAL)
			{
				//m_whoplay的牌没有了，那么检查下我的对家结束没有
				if(z==0)
				{
					//判断最后一手的任务是否完成
					IsTaskFinishedOfLastHand(re2, theSeat, iNumOfKing);
					//end

					int teamer = GetTeamer(m_whoplay);
					m_bTeamOut[teamer-1] = TRUE;

					m_whoAllOutted[m_whoplay-1] = TRUE;
					m_turn++;
					m_playerTurn[m_whoplay-1] = m_turn;

					SendPlayerTurn();

					if(teamer != 0)
					{
						//我的对家走完，那么结束游戏
						if(m_whoAllOutted[teamer-1])
						{
							//接着检测是不是双扣
							m_whoAllOutted[m_whoplay-1] = TRUE;
							int ncount = 0;
							for(int i = 0; i < MAX_PLAYER; i++)
							{
								if(!m_whoAllOutted[i])
									ncount++;
							}

							//双扣
							if(ncount == 2)
							{
								for(int i = 0; i < MAX_PLAYER; i++)
								{
									if(m_playerTurn[i] == 1)
									{
										m_nextbanker = i+1;
										break;
									}
								}
								GameOver(m_whoplay-1,GOT_DOUBLEKILL);
							}

							//3家结束
							else if(ncount == 1)
							{
								for(int i = 0; i < MAX_PLAYER; i++)
								{
									if(m_playerTurn[i] == 1)
									{
										m_nextbanker = i+1;
										break;
									}
								}
								m_playerscore[m_whoplay-1] += m_score;
								GameOver(m_whoplay-1,GOT_NORMAL);
							}
							else
							{//可能吗？如果是其他情况，那么逻辑就错了！！

							}
							//这里检查下，是否结束游戏
							if(JudgeEndGame(m_whoplay))
							{
								return 1;
							}
						}

						//接风
						else
						{
							//这里检查下，是否结束游戏
							if(JudgeEndGame(m_whoplay))
							{
								return 1;
							}
							//SendPlaySoundMsg(m_whoplay-1,msgPlaySound::TY_JieFeng);
							SendPaiToTeamer(teamer-1,m_whoplay-1);

							GoNextPlayer();
						}
					}

				}
				else
				{
					if(m_bTeamOut[m_whoplay-1])
					{
						int teamer = GetTeamer(m_whoplay);
						SendPaiToTeamer(m_whoplay-1,teamer-1);
					}
					GoNextPlayer();
				}
			}

		}

		//压不住就过
		else
		{
			m_whoPass[m_whoplay-1] = TRUE;
		}
	}
	return 1;
}

//记分 结构
struct TALLY
{
	TALLY(void) {memset(this, 0, sizeof(TALLY));}

	int			_brandid;		// 品牌id
	int			_numid;			// 玩家id

	// 不计盘数，赢，输，平局，逃跑
	enum TALLY_RESAULT{endNone=0,endWin,endLose,endDraw,endEscape};
	TALLY_RESAULT	_result;		// 计分结果

	long			_typescore;		// 当前玩的类型的输赢
	long			_typescore_charge;// 当前玩的类型的对局费用
	int				_exp;			// 增加de经验, 游戏服务不根据游戏逻辑提供的经验计算，经验是游戏时间的累积

	char			_luastring[128];// 附加Lua字符串, 给奖励 等使用

	char			_blog;			// 是否写日志,指的是些下面的string，上面的分数是否写日志，有游戏本身决定
	char			_logdesc[256];	// 日志信息，必须是string

	bool			_ret_baward;
	long			_ret_typescore_award;// 当前玩的类型的奖励

	long			_subscore;				// 辅助分数
	char			_subscore_string[128];	// 辅助分数额外说明

	int				_reserve;		// 保留给后台服务使用
};

void CGameLogic::GameOver(unsigned char winner,GameOverType type)
{
	if(winner < 0 || winner >= MAX_PLAYER)
		return;
	if(m_GameType == GT_NORMAL)
	{
		m_score = 0;
		SendTurnScore();
	}
	TALLY tally[MAX_PLAYER];
	memset(tally,0,sizeof(tally));
	long iScore[MAX_PLAYER];
	memset(iScore,0,sizeof(iScore));

	CMD_S_EndGame_CB endgameMsg;
	int ls1=0,ls2=0,ws1=0,ws2=0;

	int fan = 1;

	//有人逃跑，游戏结束
	///////////////////////////////////////////////////////////////////////////////////////////////////////
	if(type == GOT_ESCAPE)
	{
		endgameMsg.theEndType = CMD_S_EndGame_CB::TY_ESCAPE;
		endgameMsg.theWhoKingBomb = m_whoHasKingBomb;
		for(int i = 0; i < MAX_PLAYER; i++)
		{
			endgameMsg.the7Xi[i] = m_who7Xi[i];
			endgameMsg.the8Xi[i] = m_who8Xi[i];
		}
		int nFan = 0;
		if(m_GameType == GT_ROAR)
		{
			nFan = m_iFaOfTao * 2;
		}
		else
		{
			nFan = m_iFaOfTao;
		}
		for(int i = 0; i < MAX_PLAYER; i++)
		{
			if(i == winner)
			{
				iScore[i] = -m_nBase*nFan;
				endgameMsg.theWinLose[i] = 0;
				tally[i]._result = TALLY::endEscape;
			}
			else
			{
				if(m_GameType == GT_ROAR)
				{
					iScore[i] = m_nBase*(m_iJiangOfTao)*2;
				}
				else
				{
					iScore[i] = m_nBase*(m_iJiangOfTao);
				}
				endgameMsg.theWinLose[i] = 1;
				tally[i]._result = TALLY::endWin;
			}
		}
		for(int i = 0; i < MAX_PLAYER; i++)
		{
			AddSpecailScore(iScore,i,m_nBase);
		}
		endgameMsg.theFan = nFan;
	}

	//只有在双杀的情况下，才会可能出现光头:一方得到1游和2游，另外一方没得分，就是光头;游戏结束
	///////////////////////////////////////////////////////////////////////////////////////////////////////
	else if(type == GOT_DOUBLEKILL)
	{
		endgameMsg.theEndType = CMD_S_EndGame_CB::TY_SCORE;
		endgameMsg.theWhoKingBomb = m_whoHasKingBomb;
		for(int i = 0; i < MAX_PLAYER; i++)
		{
			endgameMsg.the7Xi[i] = m_who7Xi[i];
			endgameMsg.the8Xi[i] = m_who8Xi[i];
		}

		int teamer = GetTeamer(winner+1);

		ws1 = winner;
		ws2 = teamer-1;
		for(int i =0; i < MAX_PLAYER; i ++)
		{
			if(i == ws1)
				continue;
			if(i == ws2)
				continue;
			ls1 = i;
			break;
		}
		for(int i =0; i < MAX_PLAYER; i ++)
		{
			if(i == ws1)
				continue;
			if(i == ws2)
				continue;
			if(i == ls1)
				continue;
			ls2 = i;
			break;
		}
		if(m_playerscore[ls1]+m_playerscore[ls2] == 0)
		{//光头
			endgameMsg.theGuang = 1;
			fan *= 4;//光头*4
		}
		else
		{// 没有光头才算绑
			endgameMsg.theDoubleKill = 1;//双杀了
			fan *= 2;//绑*2
		}
		//这里来计算输赢的分数

		iScore[ws1] = fan*m_nBase;
		iScore[ws2] = fan*m_nBase;
		iScore[ls1] = -fan*m_nBase;
		iScore[ls2] = -fan*m_nBase;
		for(int i = 0; i < MAX_PLAYER; i++)
		{
			AddSpecailScore(iScore,i,m_nBase);
		}
		endgameMsg.theFan = fan;
		for(int i = 0; i < MAX_PLAYER; i++)
		{
			if(i == ws1 || i == ws2)
			{
				endgameMsg.theWinLose[i] = 1;
				tally[i]._result = TALLY::endWin;
			}
			else
			{
				endgameMsg.theWinLose[i] = 0;
				tally[i]._result = TALLY::endLose;
			}
		}
	}

	//普通类型，包括硬牌结束以及找朋友结束;游戏结束
	///////////////////////////////////////////////////////////////////////////////////////////////////////
	else if(type == GOT_NORMAL)
	{
		if(m_GameType == GT_NORMAL)
		{
			endgameMsg.theEndType = CMD_S_EndGame_CB::TY_SCORE;
			endgameMsg.theWhoKingBomb = m_whoHasKingBomb;
			for(int i = 0; i < MAX_PLAYER; i++)
			{
				endgameMsg.the7Xi[i] = m_who7Xi[i];
				endgameMsg.the8Xi[i] = m_who8Xi[i];
			}

			int player1 =0,player2=0,player3=0,player4=0;

			//处理没有给最后一个玩家赋值名次号的数据处理
			for(int i =0; i < MAX_PLAYER; i++)
			{
				//若名次号还是INVALID_unsigned char，则表示还是初始化的数据，需要重新赋值
				if(m_playerTurn[i] == 0xff)
					m_playerTurn[i] = 4;
			}

			//
			for(int i =0; i < MAX_PLAYER; i++)
			{
				if(m_playerTurn[i] == 1) player1 = i;
				if(m_playerTurn[i] == 2) player2 = i;
				if(m_playerTurn[i] == 3) player3 = i;
				if(m_playerTurn[i] == 4) player4 = i;
			}
			m_playerscore[player1] += m_playerscore[player4];//末游玩家得分归一游
			m_playerscore[player4] = 0;
			m_playerscore[player3] += m_dagun.GetScore(m_dbplay[player4]);//末游玩家手上分数归三游玩家
			int backerscore = m_playerscore[m_banker-1]+m_playerscore[m_parter-1];

			if(backerscore == 200 || backerscore == 0)
			{//庄家清
				endgameMsg.theQing = 1;
				fan *= 2;//清*2
				if(backerscore == 200)
				{
					ws1 = m_banker-1;
					ws2 = m_parter-1;
					for(int i = 0; i < MAX_PLAYER; i++)
					{
						if(i == ws1) continue;
						if(i == ws2) continue;
						ls1 = i;
						break;
					}
					ls2 = GetTeamer(ls1+1)-1;
				}
				else
				{
					ls1 = m_banker-1;
					ls2 = m_parter-1;
					for(int i = 0; i < MAX_PLAYER; i++)
					{
						if(i == ls1) continue;
						if(i == ls2) continue;
						ws1 = i;
						break;
					}
					ws2 = GetTeamer(ws1+1)-1;
				}
			}
			else if(backerscore > 100 && backerscore < 200)
			{//庄家赢了
				ws1 = m_banker-1;
				ws2 = m_parter-1;
				for(int i = 0; i < MAX_PLAYER; i++)
				{
					if(i == ws1) continue;
					if(i == ws2) continue;
					ls1 = i;
					break;
				}
				ls2 = GetTeamer(ls1+1)-1;
			}
			else if(backerscore > 0 && backerscore < 100)
			{//闲家赢了
				ls1 = m_banker-1;
				ls2 = m_parter-1;
				for(int i = 0; i < MAX_PLAYER; i++)
				{
					if(i == ls1) continue;
					if(i == ls2) continue;
					ws1 = i;
					break;
				}
				ws2 = GetTeamer(ws1+1)-1;
			}
			else if(backerscore == 100)
			{//看谁是第4游
				if(player4+1 == m_banker || player4+1 == m_parter)
				{//闲家赢
					ls1 = m_banker-1;
					ls2 = m_parter-1;
					for(int i = 0; i < MAX_PLAYER; i++)
					{
						if(i == ls1) continue;
						if(i == ls2) continue;
						ws1 = i;
						break;
					}
					ws2 = GetTeamer(ws1+1)-1;
				}
				else
				{//庄家赢
					ws1 = m_banker-1;
					ws2 = m_parter-1;
					for(int i = 0; i < MAX_PLAYER; i++)
					{
						if(i == ws1) continue;
						if(i == ws2) continue;
						ls1 = i;
						break;
					}
					ls2 = GetTeamer(ls1+1)-1;
				}
			}
			//这里来计算输赢的分数

			iScore[ws1] = fan*m_nBase;
			iScore[ws2] = fan*m_nBase;
			iScore[ls1] = -fan*m_nBase;
			iScore[ls2] = -fan*m_nBase;
			for(int i = 0; i < MAX_PLAYER; i++)
			{
				AddSpecailScore(iScore,i,m_nBase);
			}
			endgameMsg.theFan = fan;
			for(int i = 0; i < MAX_PLAYER; i++)
			{
				if(i == ws1 || i == ws2)
				{
					endgameMsg.theWinLose[i] = 1;
					tally[i]._result = TALLY::endWin;
				}
				else
				{
					endgameMsg.theWinLose[i] = 0;
					tally[i]._result = TALLY::endLose;
				}
			}
		}
		else if(m_GameType == GT_ROAR)
		{
			endgameMsg.theEndType = CMD_S_EndGame_CB::TY_ROAR;
			endgameMsg.theWhoKingBomb = m_whoHasKingBomb;
			fan *= 4;//吼*4
			for(int i = 0; i < MAX_PLAYER; i++)
			{
				endgameMsg.the7Xi[i] = m_who7Xi[i];
				endgameMsg.the8Xi[i] = m_who8Xi[i];
			}
			endgameMsg.theFan = fan;
			if(winner == m_banker-1)
			{//庄家赢
				for(int i = 0; i < MAX_PLAYER; i++)
				{
					if(i==winner)
					{
						endgameMsg.theWinLose[i] = 1;
						iScore[i] = fan*m_nBase*3;
						tally[i]._result = TALLY::endWin;
					}
					else
					{
						endgameMsg.theWinLose[i] = 0;
						iScore[i] = -fan*m_nBase;
						tally[i]._result = TALLY::endLose;
					}
				}
			}
			else
			{//庄家输
				for(int i = 0; i < MAX_PLAYER; i++)
				{
					if(i==m_banker-1)
					{
						endgameMsg.theWinLose[i] = 0;
						iScore[i] = -fan*m_nBase*3;
						tally[i]._result = TALLY::endLose;
					}
					else
					{
						endgameMsg.theWinLose[i] = 1;
						iScore[i] = fan*m_nBase;
						tally[i]._result = TALLY::endWin;
					}
				}
			}
			for(int i = 0; i < MAX_PLAYER; i++)
			{
				AddSpecailScore(iScore,i,m_nBase);
			}
		}
	}

	//中途退出;游戏结束
	///////////////////////////////////////////////////////////////////////////////////////////////////////
	else if(type == GOT_ZHONGTU)
	{
		if(m_GameType == GT_NORMAL)
		{
			ws1 = winner;
			ws2 = GetTeamer(winner+1)-1;

			endgameMsg.theEndType = CMD_S_EndGame_CB::TY_SCORE;
			endgameMsg.theWhoKingBomb = m_whoHasKingBomb;
			for(int i = 0; i < MAX_PLAYER; i++)
			{
				endgameMsg.the7Xi[i] = m_who7Xi[i];
				endgameMsg.the8Xi[i] = m_who8Xi[i];
			}
			//判断清的情况
			int player1 =0,player2=0,player3=0,player4=0;
			//
			for(int i =0; i < MAX_PLAYER; i++)
			{
				if(m_playerTurn[i] == 1) player1 = i;
				if(m_playerTurn[i] == 2) player2 = i;
				if(m_playerTurn[i] == 3) player3 = i;
				if(m_playerTurn[i] == 4) player4 = i;
			}
			int score = GetTeamScore(ws1);
			if(score == 200)
			{
				endgameMsg.theQing = 1;
				fan *= 2;//清*2
			}

			endgameMsg.theFan = fan;

			for(int i = 0; i < MAX_PLAYER; i++)
			{
				if(i == ws1) continue;
				if(i == ws2) continue;
				ls1 = i;
				break;
			}
			ls2 = GetTeamer(ls1+1)-1;

			iScore[ws1] = fan*m_nBase;
			iScore[ws2] = fan*m_nBase;
			iScore[ls1] = -fan*m_nBase;
			iScore[ls2] = -fan*m_nBase;
			for(int i = 0; i < MAX_PLAYER; i++)
			{
				AddSpecailScore(iScore,i,m_nBase);
			}
			for(int i = 0; i < MAX_PLAYER; i++)
			{
				if(i == ws1 || i == ws2)
				{
					endgameMsg.theWinLose[i] = 1;
					tally[i]._result = TALLY::endWin;
				}
				else
				{
					endgameMsg.theWinLose[i] = 0;
					tally[i]._result = TALLY::endLose;
				}
			}
		}
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////
/////	if(m_pTable->getPlayType() == PT_PLAY_RICH || m_pTable->getPlayType() == PT_PLAY_RICHSCORE)
//	{//这里要修正
		for (int i=0;i<MAX_PLAYER;i++)
		{
			IServerUserItem * pIServerUserItem = GetServerUserItem(i);
			if (pIServerUserItem != NULL)
			{
				long lMyScore = pIServerUserItem->GetUserScore()->lScore;
				if(lMyScore>0 && lMyScore+iScore[i]-m_spay[i] < 0)
				{
					iScore[i] = -lMyScore+m_spay[i];
				}
			}	
		}
		//这里要检查下，如果不够扣，其他人只能平分
		GetFinalWinLoseScore(iScore);
//	}
	int tallycount=0;
	for(int i = 0 ;i < MAX_PLAYER; i++)
	{
		IServerUserItem * pIServerUserItem = GetServerUserItem(i);
		if(pIServerUserItem)
		{
			tally[tallycount]._brandid = pIServerUserItem->GetUserData()->dwGameID;
			tally[tallycount]._numid = pIServerUserItem->GetUserID();
			tally[tallycount]._typescore_charge = m_spay[i];
			tally[tallycount]._typescore = iScore[i] - m_spay[i];
			//7喜8喜天炸的奖励 2010-10-28
			sprintf(tally[tallycount]._luastring,"xi7=%d;xi8=%d;kingbomb=%d;",m_who7Xi[i],m_who8Xi[i],(m_whoHasKingBomb==i+1 ? 1:0) );

			//奖励数
			tally[tallycount]._ret_typescore_award = GetTaskAward(i);

			//日志信息
			sprintf(tally[tallycount]._logdesc,"%s", GetTaskAwardDetails(i).c_str());
			tally[tallycount]._blog = true;

			//写入积分
			enScoreKind nScoreKind;
			if(tally[tallycount]._result==TALLY::endDraw)nScoreKind=enScoreKind_Draw;
			else if(tally[tallycount]._result==TALLY::endWin)nScoreKind=enScoreKind_Win;
			else if(tally[tallycount]._result==TALLY::endLose)nScoreKind=enScoreKind_Lost;
			else if(tally[tallycount]._result==TALLY::endEscape)nScoreKind=enScoreKind_Flee;

			//发放奖励的详细描述直接写到日志文件中去
			m_pITableFrame->WriteUserScore(i,tally[tallycount]._typescore,tally[tallycount]._typescore_charge,nScoreKind,-1,tally[tallycount]._ret_typescore_award);

			//写日志文件 
			if(0 != lstrlen(tally[tallycount]._logdesc))
			{
				char buf[512] = {0};
				sprintf(buf, "UserID=%d: %s", tally[tallycount]._numid, tally[tallycount]._logdesc);
				TraceMessage(buf);
			}

			tallycount++;
		}
	}

	for (unsigned char i=0;i<MAX_PLAYER;i++)
	{
		endgameMsg.theScore[i] = iScore[i]-m_spay[i];

		if (endgameMsg.theScore[i] >0)
		{
			endgameMsg.theWinLose[i] = 1;
		}
		else if (endgameMsg.theScore[i] <0 )
		{
			endgameMsg.theWinLose[i] = 0;
		}

		int cn = 0;
		for(int j = 0; j < MAXCARD; j++)
		{
			if(m_dbplay[i][j] > CARDINDEX_NULL && m_dbplay[i][j] < CARDINDEX_BACK)
				endgameMsg.theLeftPai[i][cn++] = m_dbplay[i][j];
		}
		m_lastScore[i] = endgameMsg.theScore[i];
		m_total[i] += endgameMsg.theScore[i];
	}
	SEND_TABLE_DATA_ALL(SUB_S_ENDGAME, endgameMsg);

	Reset();
	//结束游戏
	m_pITableFrame->ConcludeGame();
}
unsigned char CGameLogic::GetTeamer(unsigned char who)
{
	int re = 0;
	if(who <= 0 || who > MAX_PLAYER)
		return 0;
	if(who == m_banker)
	{
		re = m_parter;
	}
	else if(who == m_parter)
	{
		re = m_banker;
	}
	else
	{//闲家
		for(int i = 0; i < MAX_PLAYER; i++)
		{
			if(i == who-1)
				continue;
			if(i == m_parter-1)
				continue;
			if(i == m_banker-1)
				continue;
			re = i+1;
			break;
		}
	}
	return re;
}
int CGameLogic::GetTeamScore( unsigned char seat )
{
	if(m_GameType != GT_NORMAL)
		return 0;
	if(seat < 0 || seat >= MAX_PLAYER)
		return 0;
	int teamer = GetTeamer(seat+1)-1;
	if(m_playerTurn[seat] == 1 || m_playerTurn[teamer] == 1)
	{//一游玩家的分
		return m_playerscore[seat]+m_playerscore[teamer];
	}
	else if(m_playerTurn[seat] == 2 || m_playerTurn[teamer] == 2)
	{
		int who2you = (m_playerTurn[seat] == 2 ? seat : teamer) ;
		return m_playerscore[who2you];
	}
	return 0;
}


bool CGameLogic::JudgeEndGame(unsigned char who)
{
	//如果有一队拣了105分，其中有人一游走掉，另外一队拣分且2游走掉，则结束游戏
	if(who > MAX_PLAYER || who <= 0)
		return false;

	int num = 0;
	for(int i =0;i<MAX_PLAYER;i++)
	{
		if(m_whoAllOutted[i])
			num++;
	}
	if(num != 2)
		return false;//只有在其中两个玩家牌出完的情况下，才有可能中途结束游戏
	int seat1 = who-1;
	int seat2 = GetTeamer(who)-1;
	//seat1 和seat2是一队的
	int seat3=0,seat4=0;
	for(int i = 0; i < MAX_PLAYER; i++)
	{
		if(i == seat1 || i == seat2)
			continue;
		seat3 = i;
		break;
	}
	seat4 = GetTeamer(seat3+1)-1;
	int score1 = GetTeamScore(seat1);
	int score2 = GetTeamScore(seat3);

	if(score1 == 200)
	{//结束游戏
		for(int i = 0; i < MAX_PLAYER; i++)
		{
			if(m_playerTurn[i] == 1)
			{
				m_nextbanker = i+1;
				break;
			}
		}
		GameOver(seat1,GOT_ZHONGTU);
		return true;
	}
	else if(score2 == 200)
	{
		for(int i = 0; i < MAX_PLAYER; i++)
		{
			if(m_playerTurn[i] == 1)
			{
				m_nextbanker = i+1;
				break;
			}
		}
		GameOver(seat3,GOT_ZHONGTU);
		return true;
	}
	else
	{
		if(score1 >= 105 && score2 >= 5)
		{//结束游戏
			for(int i = 0; i < MAX_PLAYER; i++)
			{
				if(m_playerTurn[i] == 1)
				{
					m_nextbanker = i+1;
					break;
				}
			}
			GameOver(seat1,GOT_ZHONGTU);
			return true;
		}
		if(score2 >= 105 && score1 >= 5)
		{
			for(int i = 0; i < MAX_PLAYER; i++)
			{
				if(m_playerTurn[i] == 1)
				{
					m_nextbanker = i+1;
					break;
				}
			}
			GameOver(seat3,GOT_ZHONGTU);
			return true;
		}
	}
	return false;
};


// 结束一轮
bool CGameLogic::EndTurn()
{
	for(int i = 0; i < MAXCARD; i++)
	{
		m_lastPaiOut[m_whoplayout-1][i] = m_allPaiOut[m_whoplayout-1][i];
	}
	for(int i =0;i<MAX_PLAYER;i++)
	{
		for(int j = 0; j < MAXCARD; j++)
		{
			m_allPaiOut[i][j] = 0;
		}
		m_whoPass[i] = 0;
	}

	if(m_GameType == GT_NORMAL)//打分的模式
	{
		m_playerscore[m_whoplayout-1] += m_score;
		m_score = 0;
		SendPlayerScore();
		SendTurnScore();
		//这里可以判断下是否结束游戏
		if(JudgeEndGame(m_whoplayout))
		{
			return true;
		}
	}
	m_whoplayout = 0;

	bool isjieFeng = false;
	if(m_whoAllOutted[m_whoplay-1])
	{//这里说明当前玩家出完了
		isjieFeng = true;
		int teamer = GetTeamer(m_whoplay);

		if(m_bMingJi)
		{//如果明鸡确定，那么是该玩家的队友接风
			m_whoplay = teamer;
			SendPlaySoundMsg(m_whoplay-1,CMD_CS_msgPlaySound_CB::TY_JieFeng);
		}
		else
		{//否则就是下家接风

		}	
	}
	while(TRUE)
	{
		if(m_whoAllOutted[m_whoplay-1])
		{
			if (m_whoplay==MAX_PLAYER) m_whoplay = 1;
			else m_whoplay++;
		}
		else
		{
			break;
		}
	}

	EndOut();//结束一轮
	return false;
}
void CGameLogic::EndOut()
{
	m_lastOutType = TYPE_NULL;

	CMD_S_EndOut_CB endout;

	endout.theWhoPlay = m_whoplay;

	SEND_TABLE_DATA_ALL(SUB_S_ENDOUT, endout);
}

void CGameLogic::AddSpecailScore(long Score[MAX_PLAYER],int seat,int base)
{
	if(seat < 0 && seat >= MAX_PLAYER)
		return;
	//这里来计算各自该赢的，特殊的分
	for(int i = 0; i < m_who7Xi[seat]; i++)
	{
		Score[seat] += base*3;//赢家*3
		for(int j = 0; j < MAX_PLAYER; j++)
		{
			if(j == seat)
				continue;
			Score[j] -= base;//其他人
		}
	}
	for(int i = 0; i < m_who8Xi[seat]; i++)
	{
		Score[seat] += base*6;//赢家*6
		for(int j = 0; j < MAX_PLAYER; j++)
		{
			if(j == seat)
				continue;
			Score[j] -= 2*base;//其他人
		}
	}
	if(m_whoHasKingBomb == seat+1)
	{
		Score[seat] += base*3;//赢家*3
		for(int j = 0; j < MAX_PLAYER; j++)
		{
			if(j == seat)
				continue;
			Score[j] -= base;//其他人
		}
	}
}

void CGameLogic::GetFinalWinLoseScore( long score[MAX_PLAYER] )
{
	/*
	if(m_pTable->getPlayType() != PT_PLAY_RICH
		&& m_pTable->getPlayType() != PT_PLAY_RICHSCORE)
	{
		return;
	}
	*/
	int wintotal=0,losetotal=0;
	long total[MAX_PLAYER];
	memset(total, 0, sizeof(total));
	int n = 0;
	for(int i =0; i < MAX_PLAYER; i++)
	{
		total[i] = score[i];
		if(score[i] <= 0)
			losetotal += score[i];
		else
		{
			n++;
			wintotal += score[i];
		}
	}
	if(n <= 0)
		return;
	if(wintotal > -losetotal)
	{//扣的分比加的分少，那么只能平分了
		for(int i =0; i < MAX_PLAYER; i++)
		{
			if(score[i] > 0)
			{
				//total[i] = -losetotal/n;
				total[i] = -losetotal * (1.0f*score[i])/(1.0f*wintotal);
			}
		}
	}
	for(int i =0; i < MAX_PLAYER; i++)
	{
		score[i] = total[i];
	}
}


void CGameLogic::SendTurnScore(int wChairID)
{
	CMD_CS_TurnScore_CB turnScore;

	turnScore.theScore = m_score;

	if(wChairID == INVALID_CHAIR)
		SEND_TABLE_DATA_ALL(SUB_S_TURNSCORE,turnScore);
	else if(wChairID>=0&&wChairID<MAX_PLAYER)
		SEND_TABLE_DATA(wChairID,SUB_S_TURNSCORE,turnScore);
}

void CGameLogic::SendPlayerScore(int wChairID)
{
	CMD_CS_PLAYSCORE_CB playScore;
	
	for(int i = 0;i < MAX_PLAYER;i++)
	{
		playScore.theScore[i] = m_playerscore[i];
	}

	if(wChairID == INVALID_CHAIR)
		SEND_TABLE_DATA_ALL(SUB_S_PLAYERSCORE,playScore);
	else if(wChairID>=0&&wChairID<MAX_PLAYER)
		SEND_TABLE_DATA(wChairID,SUB_S_PLAYERSCORE,playScore);
}

void CGameLogic::SendPaiCount(int wChairID)
{
	CMD_CS_SendCount_CB paiCount;

	for(int i = 0; i < MAX_PLAYER; i++)
		paiCount.thePaiCount[i] = m_thePaiCount[i];

	if(wChairID == INVALID_CHAIR)
		SEND_TABLE_DATA_ALL(SUB_S_SENDCOUNT,paiCount);
	else if(wChairID>=0&&wChairID<MAX_PLAYER)
		SEND_TABLE_DATA(wChairID,SUB_S_SENDCOUNT,paiCount);
}

void CGameLogic::SendPlayerTurn(int wChairID)
{
	CMD_CS_SendTurn_CB turnmsg;

	///////////////////////////////////////////////////////////////////////////////////////////////////////
	for(int i = 0; i < MAX_PLAYER; i++)
		turnmsg.theTurn[i] = m_playerTurn[i];

	///////////////////////////////////////////////////////////////////////////////////////////////////////
	if(wChairID == INVALID_CHAIR)
		SEND_TABLE_DATA_ALL(SUB_S_SENDTURN,turnmsg);
	else if(wChairID>=0&&wChairID<MAX_PLAYER)
		SEND_TABLE_DATA(wChairID,SUB_S_SENDTURN,turnmsg);
}

void CGameLogic::SendPaiToTeamer(unsigned char seat1,unsigned char seat2)
{
	if(seat1 < 0 || seat1 >= MAX_PLAYER) return ;
	if(seat2 < 0 || seat2 >= MAX_PLAYER) return ;

	CMD_CS_TeamerPai teamerPai;
	teamerPai.theSeat = seat1;
	for(int i=0;i<MAXCARD;i++)
	{
		if(m_dbplay[seat1][i] >0 && m_dbplay[seat1][i] < 55 )
		{
			teamerPai.thePai[teamerPai.theCount++] = m_dbplay[seat1][i];
		}
	}
	IServerUserItem * pIServerUserItem = GetServerUserItem(seat2);
	if (pIServerUserItem)
		SEND_TABLE_DATA(seat2,SUB_CS_TEAMERPAI,teamerPai);
}


void CGameLogic::sendGameStatus(WORD wChair ,bool isReLink)
{
	WORD theSeat = wChair;
	if(isReLink)//断线重连
	{
		if(!IsGamePlaying())
			return;
		

		//发送底分
		CMD_CS_GameRule msgRule;
		msgRule.theBase = m_pGameServiceOption->lCellScore;
		msgRule.theGiveGift = m_specialAward;
		msgRule.theMinRich = m_iFaOfTao;

		SEND_TABLE_DATA(wChair,SUB_CS_GAMERULE,msgRule);

		
		/*
		这些应该放到重连消息后面，考虑到重启的情况，要先写缓存
		
		//出牌顺序
		SendPlayerTurn(wChair);
		
		//剩余牌数量
		SendPaiCount(wChair);
		if(m_GameType == GT_NORMAL)
		{
			SendPlayerScore(wChair);//每个人的分,这个找朋友模式才有
		}
		*/
			

		m_whoBreak[theSeat] = FALSE;//重连嘛，所以取消断线

		//以下是考虑到有断线次数限制的情况
		/*if (m_WireBreakCounts[theSeat] > m_nWireBreakTimes)
		{
			msgQuit msgwireclose;
			InitMsg(msgwireclose);
			msgwireclose.theSeat = theSeat;
			SendGamePacket_OnePlayer(player,msgwireclose);
			return;
		}*/

		//提示其他玩家，我又回来了！
		CMD_CS_msgReLinkTip msgTip;
		msgTip.theSeat = theSeat;
		msgTip.theFlag = 0;
		SEND_TABLE_DATA_OTHER(theSeat,msgTip.XY_ID,msgTip);
	


		CMD_CS_msgReLink reLinkMsg;
		reLinkMsg.theChair = theSeat;

		if(m_tDownTime-GetCPUTickCount() > 0)
			reLinkMsg.theLeftTime	= m_tDownTime - GetCPUTickCount();
		else
			reLinkMsg.theLeftTime	= 0;

		reLinkMsg.theLastPaiType  = m_lastOutType;//上一次出牌的类型，可能没用？
		reLinkMsg.theGameState	= m_GameState;//游戏状态
		reLinkMsg.theWhoPlay		= m_whoplay;//当前玩家
		reLinkMsg.theWhoPlayOut	= m_whoplayout;//上一个玩家
		reLinkMsg.theTurnScore	= m_score;//本轮分
		reLinkMsg.theBanker       = m_banker;//庄家
		reLinkMsg.theRoarPai		= m_RoarPai;//叫的什么牌

		for(int i = 0; i < MAX_PLAYER; i++)
		{
			reLinkMsg.theWhoBreak[i]	= m_whoBreak[i];//掉线的有那些人
			reLinkMsg.theTuoGuan[i]	= m_TuoGuanPlayer[i];//托管的有那些人
			reLinkMsg.theLast[i]		= m_lastScore[i];//上一轮输赢
			reLinkMsg.theTotal[i]		= m_total[i];//总输赢
			reLinkMsg.thePass[i]		= m_whoPass[i];//谁放弃
			reLinkMsg.theWhoReady[i]	= m_whoReady[i];//谁已经完成叫牌过程
			reLinkMsg.theWhoRoar      = m_WhoRoar;//谁叫了牌
			reLinkMsg.Score[i] = m_playerscore[i];//每个人的分
			if(m_bMingJi)
				reLinkMsg.theWhoMJ    = m_parter;//谁鸣鸡
			for(int j = 0; j < MAXCARD; j++)
			{
				reLinkMsg.theOutCard[i][j] = m_allPaiOut[i][j];//刚才出的牌
				reLinkMsg.theLastOutCard[i][j] = m_lastPaiOut[i][j];//上一轮出的牌，（这个其实可以不要）
			}
		}

		SEND_TABLE_DATA(theSeat,reLinkMsg.XY_ID,reLinkMsg);


		//出牌顺序
		//SendPlayerTurn(wChair);
		{
			CMD_CS_SendTurn_CB turnmsg;
			turnmsg.isFirst = false;

			for(int i = 0; i < MAX_PLAYER; i++)
				turnmsg.theTurn[i] = m_playerTurn[i];

			SEND_TABLE_DATA(wChair,SUB_S_SENDTURN,turnmsg);
		}
		//剩余牌数量
		SendPaiCount(wChair);
		if(m_GameType == GT_NORMAL)
		{
			SendPlayerScore(wChair);//每个人的分,这个找朋友模式才有
		}

		//自己有什么样的牌
		CMD_S_GameStart_CB sendpaimsg;

	    sendpaimsg.theIsGameStart = FALSE;
	    sendpaimsg.theBanker = m_whoplay;
		sendpaimsg.theSeat = theSeat;
		sendpaimsg.theType = CMD_S_GameStart_CB::GAME_RELINK;
		unsigned char count = 0;
		for(int i = 0;i < MAXCARD; i++)
		{
			if(m_dbplay[theSeat][i] > 0 && m_dbplay[theSeat][i] < 55)
			{
				sendpaimsg.theCards[count] = m_dbplay[theSeat][i];
				count++;
			}
		}
		sendpaimsg.theCount = count;

		SEND_TABLE_DATA(theSeat,SUB_S_GAME_START,sendpaimsg);

		SendTaskID(false, theSeat);
		//如果有任务系统
		//则要把当前局的随机任务也发给玩家
		//以下是参考：
		/*
		if (m_iRandomTask >= 0 && m_iRandomTask < m_vecAllTaskID.size())
		{
			if (m_mapAllTask.find(m_vecAllTaskID[m_iRandomTask]) != m_mapAllTask.end())
			{
				msgTaskID taskID;
				InitMsg(taskID);
				taskID.theID = m_vecAllTaskID[m_iRandomTask];
				taskID.theAward = m_mapAllTask[taskID.theID];
				SendGamePacket_OnePlayer(player, taskID);
			}
		}
		*/

		//同样，如果有任务系统，还要把当前局的任务完成情况发个玩家
		//以下是参考
		/*
		for (int iTaskID=0; iTaskID<m_vecFinishedTask[theSeat].size(); iTaskID++)
		{
			//发送给客户端
			msgFinishedTaskID finishTask;
			InitMsg(finishTask);
			finishTask.theID = m_vecFinish
			edTask[theSeat][iTaskID];
			finishTask.theSeat = theSeat;
			finishTask.theFlag = 1;  //重连

			SendGamePacket_OnePlayer(player, finishTask);
		}
		*/

		//如果断线之前我的牌出完了,把队友的牌发给我
		if(m_whoAllOutted[theSeat])
		{
			int teamer = GetTeamer(theSeat+1);
			SendPaiToTeamer(teamer-1,theSeat);
		}

		//发送权限（什么权限、该谁出牌、倒计时等）
		if(m_GameState == gsRoarPai)
		{
			SendPower(m_whoplay,CMD_S_Power_CB::p_Roar,m_tDownTime - GetCPUTickCount());
		}
		else if(m_GameState ==gsPlay)
		{
			SendPower(m_whoplay,CMD_S_Power_CB::p_Out,m_tDownTime - GetCPUTickCount());
		}



	}
	else
	{

	}

}


BOOL CGameLogic::IsGamePlaying()
{
	return  (m_GameState != gsNull)? TRUE : FALSE;
}


//解析配置的任务,格式： "1@5/2@5/33@10"
void CGameLogic::ParseTaskConfig(const string& strTaskID)
{
	if (strTaskID.size() == 0)
	{
		return;
	}

	vector<string> vecID;
	stovs(strTaskID, "/", vecID);
	for (int i=0; i<vecID.size(); i++)
	{
		vector<string> details;
		stovs(vecID[i], "@", details);
		if (details.size() == 2)
		{
			int id = atoi(details[0].c_str());
			int fans = atoi(details[1].c_str());
			if (id > 0 && fans > 0)
			{
				m_mapAllTask[id] = fans;

				//特殊处理下：有3个任务不能作为随机选择项
				if (id == TASK_ZHUA_TIAN || id == TASK_ZHUA_7_XI || id == TASK_ZHUA_8_XI)
				{
					//nothing
				}
				else
				{
					m_vecAllTaskID.push_back(id);
				}
			}
		}
	}
}

//发送随机任务
void CGameLogic::SendTaskID (bool bFirst, unsigned char nSeat)
{
	int iSize = m_vecAllTaskID.size();
	if (iSize > 0)
	{
		//生成随机数
		if(bFirst)
		{
			if (g_iDebug > 0)
			{
				if (100 == g_iDebug)
				{
					m_iRandomTask = (++m_iRandomTask)%iSize;
				}
				else
				{
					m_iRandomTask = g_iDebug - 1;
				}
			}
			else
			{
				m_iRandomTask = rand()%iSize;
			}
		}

		if (m_iRandomTask < iSize)
		{
			if (m_mapAllTask.find(m_vecAllTaskID[m_iRandomTask]) != m_mapAllTask.end())
			{
				//发送给客户端
				CMD_CS_msgTaskID msgTask;
				//InitMsg(taskID);
				memset(&msgTask, 0, sizeof(msgTask));
				msgTask.theID = m_vecAllTaskID[m_iRandomTask];
				msgTask.theAward = m_mapAllTask[msgTask.theID];
				//m_iRandomTask = taskID.theID;

				if(bFirst)
				{
					SEND_TABLE_DATA_ALL(SUB_CS_RAND_TASK_ID , msgTask);
				}
				else
				{
					SEND_TABLE_DATA(nSeat, SUB_CS_RAND_TASK_ID, msgTask);
				}
			}
			else
			{
				//:-( 应该永远不会执行这句
				m_iRandomTask = -1;
			}
		}
		else
		{
			//:-( 应该永远不会执行这句
			m_iRandomTask = -1;
		}
	}

	//如果是抓牌就奖励的任务，也在这里处理
	//if (m_whoHasKingBomb > 0 && m_whoHasKingBomb <= MAX_PLAYER)
	//{
	//	m_vecFinishedTask[m_whoHasKingBomb-1].push_back(TASK_ZHUA_TIAN);
	//	SendFinishTask(m_whoHasKingBomb-1, TASK_ZHUA_TIAN, 1, false);  //只发给自己
	//}
	//for(int i = 0; i < MAX_PLAYER; i++)
	//{
	//	if (m_who8Xi[i] > 0)
	//	{
	//		m_vecFinishedTask[i].insert(m_vecFinishedTask[i].end(), m_who8Xi[i], TASK_ZHUA_8_XI);
	//		SendFinishTask(i, TASK_ZHUA_8_XI, m_who8Xi[i], false); //只发给自己
	//	}
	//}
	//for(int i = 0; i < MAX_PLAYER; i++)
	//{
	//	if (m_who7Xi[i] > 0)
	//	{
	//		m_vecFinishedTask[i].insert(m_vecFinishedTask[i].end(), m_who7Xi[i], TASK_ZHUA_7_XI);
	//		SendFinishTask(i, TASK_ZHUA_7_XI, m_who7Xi[i], false); //只发给自己
	//	}
	//}
}

//判断任务是否完成
#define TASK_FINISH(seat,id)  do{if (m_iRandomTask < m_vecAllTaskID.size() && m_vecAllTaskID[m_iRandomTask] == id){m_vecFinishedTask[seat].push_back(id);SendFinishTask(seat,id);m_bHaveOutCard = true;}}while(0)
void CGameLogic::IsTaskFinished(const TCardType& re, int iSeat, int iNumOfKing)
{
	if (re.type == TYPE_BOMB_510K && iNumOfKing == 0)
	{
		switch (re.color)
		{
		case CC_SPADE_S:
			{
				TASK_FINISH(iSeat,TASK_HEI_510K);
			}
			break;
		case CC_HEART_S:
			{
				TASK_FINISH(iSeat,TASK_HONG_510K);
			}
			break;
		case CC_CLUB_S:
			{
				TASK_FINISH(iSeat,TASK_MEI_510K);
			}
			break;
		case CC_DIAMOND_S:
			{
				TASK_FINISH(iSeat,TASK_FANG_510K);
			}
			break;
		default:
			break;
		}
	}
	else if (re.type == TYPE_BOMB_NOMORL && re.len == 4 && iNumOfKing == 0)
	{	
		switch (GetCardPoint(re.card))
		{
		case CP_2_S:
			{
				TASK_FINISH(iSeat,TASK_4_2);
			}
			break;
		case CP_5_S:
			{
				TASK_FINISH(iSeat,TASK_4_5);
			}
			break;
		case CP_10_S:
			{
				TASK_FINISH(iSeat,TASK_4_10);
			}
			break;
		case CP_K_S:
			{
				TASK_FINISH(iSeat,TASK_4_K);
			}
			break;
		default:
			break;
		}
	}
	else if (re.type == TYPE_BOMB_NOMORL && re.len == 7)
	{
		TASK_FINISH(iSeat,TASK_7_ZHA);
	}
	else if (re.type == TYPE_BOMB_STR)
	{
		TASK_FINISH(iSeat,TASK_YAO_BAI);
	}
	else if (m_bHaveOutCard == false && re.type == TYPE_TWOSTR && iNumOfKing == 0)
	{
		//首次打出连对
		if (re.len > 7)
		{
			TASK_FINISH(iSeat,TASK_FIRST_4_STR);
		}
		else if (re.len == 6)
		{
			switch (GetCardPoint(re.card))
			{
			case CP_8_S:
				{
					TASK_FINISH(iSeat,TASK_FIRST_STR_678);
				}
				break;
			case CP_9_S:
				{
					TASK_FINISH(iSeat,TASK_FIRST_STR_789);
				}
				break;
			default:
				break;
			}
		}
		else if (re.len == 4)
		{
			switch (GetCardPoint(re.card))
			{
			case CP_A_S:
				{
					TASK_FINISH(iSeat,TASK_FIRST_STR_KA);
				}
				break;
			case CP_K_S:
				{
					TASK_FINISH(iSeat,TASK_FIRST_STR_QK);
				}
				break;
			case CP_J_S:
				{
					TASK_FINISH(iSeat,TASK_FIRST_STR_10J);
				}
				break;
			case CP_10_S:
				{
					TASK_FINISH(iSeat,TASK_FIRST_STR_910);
				}
				break;
			default:
				break;
			}
		}
	}
	else if (m_bHaveOutCard == false && re.type == TYPE_THREESTR && re.len > 5 && iNumOfKing == 0)
	{
		//首次打出飞机
		TASK_FINISH(iSeat,TASK_FIRST_PLANE);
	}
}

void CGameLogic::IsTaskFinishedOfLastHand(const TCardType& re, int iSeat, int iNumOfKing)
{
	if (re.type == TYPE_BOMB_510K  && iNumOfKing == 0)
	{
		switch (re.color)
		{
		case CC_SPADE_S:
			{
				TASK_FINISH(iSeat,TASK_LAST_HEI_510K);
			}
			break;
		case CC_HEART_S:
			{
				TASK_FINISH(iSeat,TASK_LAST_HONG_510K);
			}
			break;
		case CC_CLUB_S:
			{
				TASK_FINISH(iSeat,TASK_LAST_MEI_510K);
			}
			break;
		default:
			break;
		}
	}
	else if (re.type == TYPE_THREE && iNumOfKing == 0)
	{
		switch (GetCardPoint(re.card))
		{
		case CP_5_S:
			{
				TASK_FINISH(iSeat,TASK_LAST_THREE_5);
			}
			break;
		case CP_10_S:
			{
				TASK_FINISH(iSeat,TASK_LAST_THREE_10);
			}
			break;
		case CP_K_S:
			{
				TASK_FINISH(iSeat,TASK_LAST_THREE_K);
			}
			break;
		default:
			break;
		}
	}
	else if (re.type == TYPE_THREESTR && re.len >= 6 && iNumOfKing == 0)
	{
		//打出飞机
		TASK_FINISH(iSeat,TASK_LAST_PLANE);
	}
	else if (re.type == TYPE_TWOSTR && iNumOfKing == 0)
	{
		//打出连对
		if (re.len == 8)
		{
			TASK_FINISH(iSeat,TASK_LAST_4_STR);
		}
		else if (re.len == 6)
		{
			switch (GetCardPoint(re.card))
			{
			case CP_8_S:
				{
					TASK_FINISH(iSeat,TASK_LAST_STR_678);
				}
				break;
			case CP_9_S:
				{
					TASK_FINISH(iSeat,TASK_LAST_STR_789);
				}
				break;
			default:
				break;
			}
		}
		else if (re.len == 4)
		{
			switch (GetCardPoint(re.card))
			{
			case CP_A_S:
				{
					TASK_FINISH(iSeat,TASK_LAST_STR_KA);
				}
				break;
			case CP_K_S:
				{
					TASK_FINISH(iSeat,TASK_LAST_STR_QK);
				}
				break;
			case CP_J_S:
				{
					TASK_FINISH(iSeat,TASK_LAST_STR_10J);
				}
				break;
			case CP_10_S:
				{
					TASK_FINISH(iSeat,TASK_LAST_STR_910);
				}
				break;
			default:
				break;
			}
		}
	}
	else if (re.type == TYPE_ONE && GetCardPoint(re.card) == CP_3_S && iNumOfKing == 0)
	{
		TASK_FINISH(iSeat,TASK_LAST_3);
	}
}

//发送完成了的任务
void CGameLogic::SendFinishTask (int iSeat, int id, int cnt,bool bToAll)
{
	if (m_mapAllTask.find(id) != m_mapAllTask.end())
	{
		//发送给客户端
		CMD_CS_msgFinishedTaskID msgFinishedTask;
		//InitMsg(msgFinishedTask);
		memset(&msgFinishedTask, 0, sizeof(msgFinishedTask));
		msgFinishedTask.theID = id;
		msgFinishedTask.theSeat = iSeat;
		msgFinishedTask.theCnt = cnt;
		msgFinishedTask.theTotal = m_mapAllTask[id] * msgFinishedTask.theCnt;

		if (bToAll)
		{
			SEND_TABLE_DATA_ALL(SUB_CS_RAND_FINISHED_TASK , msgFinishedTask);
		}
		else
		{
			SEND_TABLE_DATA(iSeat , SUB_CS_RAND_FINISHED_TASK , msgFinishedTask);
		}

		//发放奖励的详细描述直接写到日志文件中去
		//m_pITableFrame->WriteUserScore(wChairID, lScore, lRevenue, ScoreKind, lPlayTimeCount=-1, lAward=0);
		//写日志文件 
		//TraceMessage(GetTaskAwardDetails(iSeat).c_str());
	}
}

//获取奖励数
int CGameLogic::GetTaskAward(int iSeat)
{
	int iTotal = 0;
	for (int i=0; i<m_vecFinishedTask[iSeat].size(); i++)
	{
		int id = m_vecFinishedTask[iSeat][i];
		if (m_mapAllTask.find(id) != m_mapAllTask.end())
		{
			iTotal += m_mapAllTask[id];
		}
	}

	return iTotal;
}

//获取奖励详细.必须小于255
string CGameLogic::GetTaskAwardDetails(int iSeat)
{
	string details;
	for (int i=0; i<m_vecFinishedTask[iSeat].size(); i++)
	{
		char id[16];
		memset(id, 0, 16);
		sprintf(id, "AwardCount=%d|", m_vecFinishedTask[iSeat][i]);

		details += id;
	}

	if (details.size() > 255)
	{
		details.resize(255);
	}
	return details;
}

}; //end of namespace
///////////////////////////////////////////////////////////////////////////////////////////////////////

