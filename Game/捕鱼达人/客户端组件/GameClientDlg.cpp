#include "StdAfx.h"
#include "Resource.h"
#include "GameClient.h"
#include "GameClientDlg.h"
#include "DlgBulletShop.h"
#include "DlgCellScoreOption.h"
//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CGameClientDlg, CGameFrameDlg)
	ON_WM_NCHITTEST()

	//ON_MESSAGE(WM_SHOW_BULLET_SHOP,OnShowBulletShop)
	ON_MESSAGE(WM_SHOOT,OnShoot)
	//ON_MESSAGE(WM_CAPTURE_FISH,OnCaptureFish)
	ON_MESSAGE(WM_QUIT_GAME,OnQuitGame)
	ON_MESSAGE((WM_USER+888),OnEnableSound)
	ON_MESSAGE(WM_HIT_FISH,OnHitFish)

	//ON_MESSAGE(WM_USER+888,OnCapture)

END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////

//构造函数
CGameClientDlg::CGameClientDlg(): CGameFrameDlg(&m_GameClientView)
{
	//初始变量
	ZeroMemory(m_lBulletCharge,sizeof(m_lBulletCharge));
	m_BulletCountKindSelected=enBulletCountKind_600;
	m_lCellScore=0L;

	return;
}

//析构函数
CGameClientDlg::~CGameClientDlg()
{
}

//初始函数
bool  CGameClientDlg::InitGameFrame()
{
	//设置图标
	HICON hIcon=LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME));
	SetIcon(hIcon,TRUE);
	SetIcon(hIcon,FALSE);

	//修改风格
	ModifyStyle(WS_MAXIMIZEBOX|WS_MINIMIZEBOX|WS_SIZEBOX,0);

	return true;
}

//游戏设置
void CGameClientDlg::OnGameOptionSet()
{
	return;
}
//旁观状态
void CGameClientDlg::OnLookonChanged(bool bLookonUser, const void * pBuffer, WORD wDataSize)
{
	//扑克控制

	return;
}

//重置引擎
void  CGameClientDlg::ResetGameFrame()
{
	//内核重置

	return ;
}

//时间消息
bool CGameClientDlg::OnTimerMessage(WORD wChairID, UINT nElapse, UINT nTimerID)
{
	return false;
}

//网络消息
bool CGameClientDlg::OnGameMessage(WORD wSubCmdID, const void * pData, WORD wDataSize)
{
	//消息处理
	switch (wSubCmdID)
	{
	case SUB_S_TRACE_POINT:				//轨迹坐标
		{
			return OnSubFishTrace(pData, wDataSize);
		}
	case SUB_S_USER_SHOOT:				//发射炮弹
		{
			return OnSubUserShoot(pData, wDataSize);
		}
	case SUB_S_CAPTURE_FISH:			//捕获鱼群
		{
			return OnSubCaptureFish(pData, wDataSize);
		}
	case SUB_S_BULLET_COUNT:			//子弹数目
		{
			return OnSubBulletCount(pData, wDataSize);
		}
	case SUB_S_COIN_COUNT:				//金币数目
		{
			return OnSubCoinCount(pData, wDataSize);
		}
	case SUB_S_FISH_COUNT:				//鱼群数目
		{
			return OnSubCptrFishCount(pData, wDataSize);
		}
	case SUB_S_CHANGE_SCENE:			//切换场景
		{
			return OnSubChangeScene(pData, wDataSize);
		}
	case SUB_S_SCORE_INFO:				//分数信息
		{
			return OnSubScoreInfo(pData, wDataSize);
		}
	case SUB_S_SET_CELL_SCORE:			//单元积分
		{
			return OnSubSetCellScore(pData, wDataSize);
		}
	case SUB_S_BONUS_INFO:				//彩金信息
		{
			return OnSubBonusInfo(pData, wDataSize);
		}
	case SUB_S_GAME_MESSAGE:			//游戏消息
		{
			return OnSubGameMessage(pData, wDataSize);
		}
	}
	return false;
}

//游戏场景
bool CGameClientDlg::OnGameSceneMessage(BYTE cbGameStatus, bool bLookonOther, const void * pData, WORD wDataSize)
{
	//场景处理
	switch (cbGameStatus)
	{
	case GS_FREE:
	case GS_PLAYING:
		{
			//合法判断
			ASSERT(wDataSize==sizeof(CMD_S_GameScene));
			if (!(wDataSize==sizeof(CMD_S_GameScene))) return false;

			//设置座位
			m_GameClientView.m_GameClientViewDX.SetMeChairID(GetMeChairID());

			//设置模式
			m_GameClientView.m_GameClientViewDX.SetLokonMode(IsLookonMode());

			//类型转换
			CMD_S_GameScene *pGameScene=(CMD_S_GameScene *)pData;

			//合法判断
			ASSERT(pGameScene->CureSeaSceneKind<enSeaSceneKind_No);
			if (!(pGameScene->CureSeaSceneKind<enSeaSceneKind_No)) return false;

			//保存信息
			CopyMemory(m_lBulletCharge,pGameScene->lBulletCharge,sizeof(m_lBulletCharge));
			m_GameClientView.m_GameClientViewDX.SetBulletCharge(m_lBulletCharge);

			//设置场景
			m_GameClientView.m_GameClientViewDX.SetChangeScene(true, false, pGameScene->CureSeaSceneKind);

			//单元积分
			for (int i=0; i<GAME_PLAYER; ++i) 
			{
				m_GameClientView.m_GameClientViewDX.SetCellScore(i,pGameScene->lUserCellScore[i]);
				if (i==GetMeChairID()) m_lCellScore=pGameScene->lUserCellScore[i];
			}

			////购买子弹
			//if (false==IsLookonMode())
			//{
			//	//椅子号码
			//	WORD wMeChairID=GetMeChairID();

			//	//单元积分
			//	if (0==pGameScene->lUserCellScore[wMeChairID])
			//	{
			//		//弹出对话框
			//		CDlgCellScoreOption DlgCellScoreOption;
			//		DlgCellScoreOption.m_pCenterView=&m_GameClientView.m_GameClientViewDX;
			//		CopyMemory(DlgCellScoreOption.m_lCellScore,pGameScene->lCellScore,sizeof(DlgCellScoreOption.m_lCellScore));
			//		//DlgCellScoreOption.DoModal();

			//		//合法判断
			//		LONG lCellScore=DlgCellScoreOption.m_lCellScoreSelect;
			//		bool bReset=false;
			//		if (lCellScore!=pGameScene->lCellScore[0])
			//		{
			//			tagUserData const *pMeUserData=GetUserData(wMeChairID);
			//			if (NULL!=pMeUserData)
			//			{
			//				LONGLONG lUserScore=pMeUserData->lScore;
			//				if (lUserScore<(lCellScore*m_lBulletCharge[0]))
			//				{
			//					lCellScore=pGameScene->lCellScore[0];
			//					for (int i=0; i<CountArray(pGameScene->lCellScore); ++i)
			//					{
			//						if (lUserScore>=(pGameScene->lCellScore[i]*m_lBulletCharge[0]))
			//						{
			//							lCellScore=pGameScene->lCellScore[i];
			//							bReset=true;
			//						}
			//					}
			//				}
			//			}
			//		}
			//		if (true==bReset)
			//		{
			//			TCHAR szBuffer[256];
			//			_sntprintf(szBuffer,CountArray(szBuffer),TEXT("对不起，您的金币不够，系统为您重新设置了倍数，当前倍数为 %ld"),lCellScore);
			//			MessageBox(szBuffer,TEXT("温馨提示"));
			//		}

			//		//发送消息
			//		CMD_C_SetCellScore SetCellScore={0};
			//		SetCellScore.lCellScore=lCellScore;
			//		SendData(SUB_C_SET_CELL_SCORE,&SetCellScore,sizeof(SetCellScore));

			//		//保存信息
			//		m_lCellScore=DlgCellScoreOption.m_lCellScoreSelect;
			//		m_GameClientView.m_GameClientViewDX.SetCellScore(wMeChairID,DlgCellScoreOption.m_lCellScoreSelect);
			//	}

			//	//购买子弹
			//	ShowBulletShop();
			//}

			return true;
		}
	}
	return false;
}

//测试消息
UINT CGameClientDlg::OnNcHitTest(CPoint point)
{
	UINT uResult=CGameFrameDlg::OnNcHitTest(point);
	if (HTBOTTOM==uResult || HTRIGHT==uResult || HTLEFT==uResult || HTTOP==uResult) uResult=HTCLIENT;

	return uResult;
}

//鱼群轨迹
bool CGameClientDlg::OnSubFishTrace(const void * pData, WORD wDataSize)
{
	//合法判断
	ASSERT((wDataSize%sizeof(CMD_S_FishTrace))==0);
	if (!((wDataSize%sizeof(CMD_S_FishTrace))==0)) 
	{
		TraceString(TEXT("(!((wDataSize%sizeof(CMD_S_FishTrace))==0))"));
		return false;
	}

	//类型转换
	WORD wTraceCount=wDataSize/sizeof(CMD_S_FishTrace);
	CMD_S_FishTrace *pFishTrace=(CMD_S_FishTrace *)pData;

	//设置轨迹
	for (int i=0; i<wTraceCount; ++i)
	{
		m_GameClientView.m_GameClientViewDX.AddTrace(pFishTrace->nInitX,pFishTrace->nInitY,pFishTrace->nInitCount,pFishTrace->FishKind,pFishTrace->dwFishID,pFishTrace->bRegular);
		pFishTrace++;
	}	

	return true;
}

//发射炮弹
bool CGameClientDlg::OnSubUserShoot(const void * pData, WORD wDataSize)
{
	//合法判断
	ASSERT(wDataSize==sizeof(CMD_S_UserShoot));
	if (!(wDataSize==sizeof(CMD_S_UserShoot))) return false;

	//类型转换
	CMD_S_UserShoot *pUserShoot=(CMD_S_UserShoot *)pData;

	//发射炮弹
	m_GameClientView.m_GameClientViewDX.UserShoot(pUserShoot->wChairID, pUserShoot->fAngle, pUserShoot->BulletCountKind);

	//子弹数目
	m_GameClientView.m_GameClientViewDX.SetShootBulletCount(pUserShoot->wChairID, pUserShoot->byShootCount);

	return true;
}

//捕获鱼群
bool CGameClientDlg::OnSubCaptureFish(const void * pData, WORD wDataSize)
{
	//合法判断
	ASSERT(wDataSize==sizeof(CMD_S_CaptureFish));
	if (!(wDataSize==sizeof(CMD_S_CaptureFish))) return false;

	//类型转换
	CMD_S_CaptureFish *pCaptureFish=(CMD_S_CaptureFish *)pData;

	//合法判断
	ASSERT(pCaptureFish->FishKind<enFishKind_No);
	if (!(pCaptureFish->FishKind<enFishKind_No)) 
	{
		TraceString(TEXT("(!(pCaptureFish->FishKind<enFishKind_No))"));
		return false;
	}

	//发射炮弹
	m_GameClientView.m_GameClientViewDX.SetFishStatus(pCaptureFish->FishKind, pCaptureFish->dwFishID, enFishStatus_Die);

	//增加金币
	enFishKind FishKind=m_GameClientView.m_GameClientViewDX.GetFishKind(pCaptureFish->dwFishID);
	if (FishKind<enFishKind_No)
	{
		tagFishInfo *pFishInfo=g_FishGroup[FishKind].GetFish(pCaptureFish->dwFishID);
		LONG lScore=pCaptureFish->lFishScore;//g_nFishScore[FishKind];
		if (NULL!=pFishInfo &&pFishInfo->wTracePtIndex<pFishInfo->wTracePtCount)
		{
			m_GameClientView.m_GameClientViewDX.UserAddCoin(pCaptureFish->wChairID, pFishInfo->ptTrace[pFishInfo->wTracePtIndex].x,pFishInfo->ptTrace[pFishInfo->wTracePtIndex].y,lScore);
			if (enFishKind_12==FishKind) m_GameClientView.m_GameClientViewDX.AddBonus(pCaptureFish->wChairID,lScore);
		}
	}

	//击中声音
	if (pCaptureFish->FishKind>=enFishKind_8)
	{
		TCHAR *pszHitFishSound[]={TEXT("Effect\\HitFish01.mp3"),TEXT("Effect\\HitFish02.mp3"),TEXT("Effect\\HitFish03.mp3"),TEXT("Effect\\HitFish04.mp3"),TEXT("Effect\\HitFish05.mp3"),
			TEXT("Effect\\HitFish06.mp3"),TEXT("Effect\\HitFish07.mp3"),TEXT("Effect\\HitFish08.mp3"),TEXT("Effect\\HitFish09.mp3"),TEXT("Effect\\HitFish10.mp3"),TEXT("Effect\\HitFish11.mp3"),TEXT("Effect\\HitFish12.mp3")};
		m_GameClientView.m_GameClientViewDX.PlayMp3Sound(pszHitFishSound[pCaptureFish->FishKind],false);
	}

	//死亡声音
	TCHAR *pszFishDyingSound[]={TEXT("Effect\\FishDying01.mp3"),TEXT("Effect\\FishDying02.mp3"),TEXT("Effect\\FishDying03.mp3"),TEXT("Effect\\FishDying04.mp3"),TEXT("Effect\\FishDying05.mp3"),
		TEXT("Effect\\FishDying06.mp3"),TEXT("Effect\\FishDying07.mp3"),TEXT("Effect\\FishDying08.mp3"),TEXT("Effect\\FishDying09.mp3"),TEXT("Effect\\FishDying10.mp3"),TEXT("Effect\\FishDying11.mp3")};
	m_GameClientView.m_GameClientViewDX.PlayMp3Sound(pszFishDyingSound[pCaptureFish->FishKind],false);

	return true;
}

//子弹数目
bool CGameClientDlg::OnSubBulletCount(const void * pData, WORD wDataSize)
{
	//合法判断
	ASSERT(wDataSize==sizeof(CMD_S_BulletCount));
	if (!(wDataSize==sizeof(CMD_S_BulletCount))) return false;

	//类型转换
	CMD_S_BulletCount *pBulletCount=(CMD_S_BulletCount *)pData;

	//合法判断
	ASSERT(pBulletCount->BulletCountKind<enBulletCountKind_End);
	if (!(pBulletCount->BulletCountKind<enBulletCountKind_End)) 
	{
		TraceString(TEXT("(!(pBulletCount->BulletCountKind<enBulletCountKind_End))"));
		return false;
	}

	//设置数目
	m_GameClientView.m_GameClientViewDX.SetUserBulletCount(pBulletCount->wChairID, pBulletCount->BulletCountKind, pBulletCount->lBulletCount);

	return true;
}

//金币数目
bool CGameClientDlg::OnSubCoinCount(const void * pData, WORD wDataSize)
{
	//合法判断
	ASSERT(wDataSize==sizeof(CMD_S_CoinCount));
	if (!(wDataSize==sizeof(CMD_S_CoinCount))) return false;

	//类型转换
	CMD_S_CoinCount *pCoinCount=(CMD_S_CoinCount *)pData;

	//设置数目
	m_GameClientView.m_GameClientViewDX.SetUserCoinCount(pCoinCount->wChairID, pCoinCount->lCoinCount);

	return true;
}

//鱼群数目
bool CGameClientDlg::OnSubCptrFishCount(const void * pData, WORD wDataSize)
{
	//合法判断
	ASSERT(wDataSize==sizeof(CMD_S_CptrFishCount));
	if (!(wDataSize==sizeof(CMD_S_CptrFishCount))) return false;

	//类型转换
	CMD_S_CptrFishCount *pCptrFishCount=(CMD_S_CptrFishCount *)pData;

	//合法判断
	ASSERT(pCptrFishCount->FishKind<enFishKind_No);
	if (!(pCptrFishCount->FishKind<enFishKind_No)) 
	{
		TraceString(TEXT("(!(pCptrFishCount->FishKind<enFishKind_No))"));
		return false;
	}

	//设置数目
	m_GameClientView.m_GameClientViewDX.SetCptrFishCount(pCptrFishCount->FishKind, pCptrFishCount->wFishCount);

	return true;
}

//分数信息
bool CGameClientDlg::OnSubScoreInfo(const void * pData, WORD wDataSize)
{
	//设置标志
	m_GameClientView.m_GameClientViewDX.SetWaitScoreInfo(false);

	//显示对话框
	m_GameClientView.m_GameClientViewDX.ShowScoreView();

	return true;
}

//彩金信息
bool CGameClientDlg::OnSubBonusInfo(const void * pData, WORD wDataSize)
{
	//合法判断
	ASSERT(wDataSize==sizeof(CMD_S_BonusInfo));
	if (!(wDataSize==sizeof(CMD_S_BonusInfo))) return false;

	//类型转换
	CMD_S_BonusInfo *pBonusInfo=(CMD_S_BonusInfo *)pData;

	//保存信息
	m_GameClientView.m_GameClientViewDX.SetBonusInfo(pBonusInfo->lBonus);

	return true;
}

//游戏消息
bool CGameClientDlg::OnSubGameMessage(const void * pData, WORD wDataSize)
{
	//效验参数
	CMD_S_GameMessage * pMessage=(CMD_S_GameMessage *)pData;
	ASSERT(wDataSize>(sizeof(CMD_S_GameMessage)-sizeof(pMessage->szContent)));
	if (wDataSize<=(sizeof(CMD_S_GameMessage)-sizeof(pMessage->szContent))) return false;

	//消息处理
	WORD wHeadSize=sizeof(CMD_S_GameMessage)-sizeof(pMessage->szContent);
	ASSERT(wDataSize==(wHeadSize+pMessage->wMessageLength*sizeof(TCHAR)));
	if (wDataSize!=(wHeadSize+pMessage->wMessageLength*sizeof(TCHAR))) return false;
	pMessage->szContent[pMessage->wMessageLength-1]=0;

	//输出信息
	InsertGeneralString(TEXT("温馨提示："),RGB(255,0,0),false);
	InsertGeneralString(pMessage->szContent,RGB(255,0,255),true);

	return true;
}

//单元积分
bool CGameClientDlg::OnSubSetCellScore(const void * pData, WORD wDataSize)
{
	//合法判断
	ASSERT(wDataSize==sizeof(CMD_S_SetCellScore));
	if (!(wDataSize==sizeof(CMD_S_SetCellScore))) return false;

	//类型转换
	CMD_S_SetCellScore *pSetCellScore=(CMD_S_SetCellScore *)pData;

	//保存信息
	if (GetMeChairID()==pSetCellScore->wChairID) m_lCellScore=pSetCellScore->lCellScore;
	m_GameClientView.m_GameClientViewDX.SetCellScore(pSetCellScore->wChairID,pSetCellScore->lCellScore);

	return true;
}

//切换场景
bool CGameClientDlg::OnSubChangeScene(const void * pData, WORD wDataSize)
{
	//合法判断
	ASSERT(wDataSize==sizeof(CMD_S_ChangeScene));
	if (!(wDataSize==sizeof(CMD_S_ChangeScene))) return false;

	//类型转换
	CMD_S_ChangeScene *pChangeScene=(CMD_S_ChangeScene *)pData;

	//合法判断
	ASSERT(pChangeScene->CureSeaSceneKind<enSeaSceneKind_No);
	if (!(pChangeScene->CureSeaSceneKind<enSeaSceneKind_No)) 
	{
		TraceString(TEXT("(!(pChangeScene->CureSeaSceneKind<enSeaSceneKind_No))"));
		return false;
	}

	//设置数目
	m_GameClientView.m_GameClientViewDX.SetChangeScene(pChangeScene->bLToR,pChangeScene->bChange, pChangeScene->CureSeaSceneKind);

	return true;
}

//发炮消息
LRESULT CGameClientDlg::OnShoot(WPARAM wParam, LPARAM lParam)
{
	//旁观判断
	if (true==IsLookonMode()) return 0L;

	/////////////////////////////////////////////////
#ifdef _DEBUG
	//获取接口
	IClientKernel *pIClientKernel=(IClientKernel *)GetClientKernel(IID_IClientKernel,VER_IClientKernel);

	//设置标志
	//if (NULL!=pIClientKernel) pIClientKernel->SetExitFlags(false);
	const tagUserData *pUserData=pIClientKernel->GetMeUserInfo();

#endif
	/////////////////////////////////////////////////

	//发送消息
	CMD_C_UserShoot UserShoot;
	UserShoot.fAngle=m_GameClientView.m_GameClientViewDX.GetMeShootAngle();
	UserShoot.BulletCountKind=enBulletCountKind(LOWORD(lParam));
	UserShoot.byShootCount=BYTE(HIWORD(lParam));
	UserShoot.dwBulletID=DWORD(wParam);
	SendData(SUB_C_USER_SHOOT,&UserShoot,sizeof(UserShoot));

	return 0L;
}

//击中消息
LRESULT CGameClientDlg::OnHitFish(WPARAM wParam, LPARAM lParam)
{
	//合法判断
	ASSERT(0L!=wParam && 0L!=lParam);
	if (!(0L!=wParam && 0L!=lParam)) return 0L;

	//旁观判断
	if (true==IsLookonMode()) return 0L;

	//变量定义
	CMD_C_HitFish HitFish={0};

	//设置信息
	HitFish.dwFishID=DWORD(wParam);
	HitFish.dwBulletID=DWORD(lParam);

	//发送消息
	SendData(SUB_C_HIT_FISH,&HitFish,sizeof(HitFish));

	return 0L;
}

//
////捕获消息
//LRESULT CGameClientDlg::OnCaptureFish(WPARAM wParam, LPARAM lParam)
//{
//	//合法判断
//	ASSERT(0L!=wParam && 0L!=lParam);
//	if (!(0L!=wParam && 0L!=lParam)) return 0L;
//
//	//旁观判断
//	if (true==IsLookonMode()) return 0L;
//
//	//增加金币
//	enFishKind FishKind=m_GameClientView.m_GameClientViewDX.GetFishKind(DWORD(wParam));
//	if (FishKind<enFishKind_No)
//	{
//		//获取鱼群
//		tagFishInfo *pFishInfo=g_FishGroup[FishKind].GetFish(DWORD(wParam));
//		ASSERT(NULL!=pFishInfo);
//		if (NULL==pFishInfo) return 0L;
//		ASSERT(pFishInfo->byBulletCount<=5);
//		if (!(pFishInfo->byBulletCount<=5)) return 0L;
//
//		//增加金币
//		if (pFishInfo->wTracePtIndex<pFishInfo->wTracePtCount)
//		{
//			LONG lScore=(enFishKind_12==FishKind)?(m_GameClientView.m_GameClientViewDX.GetBonusCount()):g_nFishScore[FishKind];
//			POINT ptPos=pFishInfo->ptTrace[pFishInfo->wTracePtIndex];
//			m_GameClientView.m_GameClientViewDX.UserAddCoin(GetMeChairID(), ptPos.x, ptPos.y, lScore);
//			if (enFishKind_12==FishKind) m_GameClientView.m_GameClientViewDX.AddBonus(GetMeChairID(),lScore);
//		}
//
//		//变量定义
//		CMD_C_CaptureFish CaptureFish={0};
//
//		//设置信息
//		CaptureFish.dwFishID=DWORD(wParam);
//		CaptureFish.dwBulletID=DWORD(lParam);
//		CaptureFish.byCptrProbability=m_byCptrProbability[pFishInfo->byBulletCount-1][FishKind];
//		CaptureFish.byBulletCount=pFishInfo->byBulletCount;
//
//		//发送消息
//		SendData(SUB_C_CAPTURE_FISH,&CaptureFish,sizeof(CaptureFish));
//
//		//击中声音
//		if (FishKind>=enFishKind_8)
//		{
//			TCHAR *pszHitFishSound[]={TEXT("Effect\\HitFish01.mp3"),TEXT("Effect\\HitFish02.mp3"),TEXT("Effect\\HitFish03.mp3"),TEXT("Effect\\HitFish04.mp3"),TEXT("Effect\\HitFish05.mp3"),
//				TEXT("Effect\\HitFish06.mp3"),TEXT("Effect\\HitFish07.mp3"),TEXT("Effect\\HitFish08.mp3"),TEXT("Effect\\HitFish09.mp3"),TEXT("Effect\\HitFish10.mp3"),TEXT("Effect\\HitFish11.mp3"),TEXT("Effect\\HitFish12.mp3")};
//			m_GameClientView.m_GameClientViewDX.PlayMp3Sound(pszHitFishSound[FishKind],false);
//		}
//
//		//死亡声音
//		TCHAR *pszFishDyingSound[]={TEXT("Effect\\FishDying01.mp3"),TEXT("Effect\\FishDying02.mp3"),TEXT("Effect\\FishDying03.mp3"),TEXT("Effect\\FishDying04.mp3"),TEXT("Effect\\FishDying05.mp3"),
//				TEXT("Effect\\FishDying06.mp3"),TEXT("Effect\\FishDying07.mp3"),TEXT("Effect\\FishDying08.mp3"),TEXT("Effect\\FishDying09.mp3"),TEXT("Effect\\FishDying10.mp3"),TEXT("Effect\\FishDying11.mp3")};
//			m_GameClientView.m_GameClientViewDX.PlayMp3Sound(pszFishDyingSound[FishKind],false);
//	}
//
//	//TCHAR szMessage[64];
//	//_sntprintf(szMessage,CountArray(szMessage),TEXT("%ld"),CaptureFish.dwFishID);
//	//InsertShtickString(szMessage,RGB(255,0,0),true);
//
//	return 0L;
//}

////购买子弹
//LRESULT CGameClientDlg::OnShowBulletShop(WPARAM wParam, LPARAM lParam)
//{
//	//购买子弹
//	if (false==IsLookonMode())
//	{
//		//购买子弹
//		ShowBulletShop();		
//	}
//
//	return 0L;
//}

//设置语音
LRESULT CGameClientDlg::OnEnableSound(WPARAM wParam, LPARAM lParam)
{
	//禁止判断
	if (true==IsEnableSound())
	{
		//背景音乐
		TCHAR *pszBKSound[]={TEXT("BGSound0.mp3"),TEXT("BGSound1.mp3"),TEXT("BGSound2.mp3")};
		int nIndex=rand()%CountArray(pszBKSound);
		m_GameClientView.m_GameClientViewDX.PlayMp3SoundBG(pszBKSound[nIndex]);
	}
	else
	{
		m_GameClientView.m_GameClientViewDX.StopMp3SoundBG();
	}

	return 0L;
}

//退出游戏
LRESULT CGameClientDlg::OnQuitGame(WPARAM wParam, LPARAM lParam)
{
	//获取接口
	IClientKernel *pIClientKernel=(IClientKernel *)GetClientKernel(IID_IClientKernel,VER_IClientKernel);

	//设置标志
	//if (NULL!=pIClientKernel) pIClientKernel->SetExitFlags(false);//Author<cxf>;DateTime<2012-03-27>;Description<屏蔽>

	//调用父类
	__super::OnCancel();

	return 0L;
}

//用户进入
void __cdecl CGameClientDlg::OnEventUserEnter(tagUserData * pUserData, WORD wChairID, bool bLookonUser)
{
	//子弹数目
	if (false==bLookonUser)
	{
		m_GameClientView.m_GameClientViewDX.SetShootBulletCount(wChairID, 5);
	}

	return ;
}

//用户离开
void __cdecl CGameClientDlg::OnEventUserLeave(tagUserData * pUserData, WORD wChairID, bool bLookonUser)
{
	//子弹数目
	if (false==bLookonUser)
	{
		m_GameClientView.m_GameClientViewDX.SetShootBulletCount(wChairID, 0);
		m_GameClientView.m_GameClientViewDX.SetUserCoinCount(wChairID, 0);

		//子弹数目
		m_GameClientView.m_GameClientViewDX.SetUserBulletCount(wChairID, enBulletCountKind_100, 0);
		m_GameClientView.m_GameClientViewDX.SetUserBulletCount(wChairID, enBulletCountKind_600, 0);
		m_GameClientView.m_GameClientViewDX.SetUserBulletCount(wChairID, enBulletCountKind_800, 0);
	}

	return ;
}

//购买子弹
void CGameClientDlg::ShowBulletShop()
{
	////旁观判断
	//if (true==IsLookonMode()) return ;

	////重复判断
	//if (NULL!=FindWindow(NULL,TEXT("购买子弹"))) return ;

	//CDlgBulletShop DlgBulletShop(&m_GameClientView.m_GameClientViewDX,m_BulletCountKindSelected, m_lCellScore);
	//DlgBulletShop.SetBulletCharge(m_lBulletCharge);
	//if (IDOK==DlgBulletShop.DoModal())
	//{
	//	//变量定义
	//	CMD_C_BuyBullet BuyBullet;

	//	//获取类型
	//	BuyBullet.BulletCountKind=DlgBulletShop.GetBulletKind();

	//	//合法判断
	//	tagUserData const *pUserData=GetUserData(GetMeChairID());
	//	if (NULL==pUserData) return;
	//	if (pUserData->lScore<m_lBulletCharge[BuyBullet.BulletCountKind]*m_lCellScore)
	//	{
	//		//设置文件名
	//		TCHAR szPath[MAX_PATH]=TEXT(""),szIniFileName[MAX_PATH]=TEXT("");
	//		GetCurrentDirectory(CountArray(szPath),szPath);	
	//		_sntprintf(szIniFileName,CountArray(szIniFileName),TEXT("%s\\fish\\Config.ini"),szPath);

	//		TCHAR szBuffer[1024];
	//		GetPrivateProfileString(TEXT("Config"),TEXT("URL"),TEXT("www.baidu.com"),szBuffer,CountArray(szBuffer),szIniFileName);
	//		if (IDYES==MessageBox(TEXT("您的金币不足，是否现在购买？"),TEXT("温馨提示"),MB_ICONQUESTION|MB_YESNO))
	//			ShellExecute(NULL,TEXT("open"),szBuffer,NULL,NULL,SW_SHOWDEFAULT);
	//	}
	//	else
	//	{
	//		//发送消息
	//		SendData(SUB_C_BUY_BULLET,&BuyBullet,sizeof(BuyBullet));

	//		//设置数目
	//		LONG lBulletCount=m_GameClientView.m_GameClientViewDX.GetBulletCount(GetMeChairID(),BuyBullet.BulletCountKind);
	//		if (enBulletCountKind_100==BuyBullet.BulletCountKind) lBulletCount+=100;
	//		else if (enBulletCountKind_600==BuyBullet.BulletCountKind) lBulletCount+=600;
	//		else if (enBulletCountKind_800==BuyBullet.BulletCountKind) lBulletCount+=800;
	//		m_GameClientView.m_GameClientViewDX.SetUserBulletCount(GetMeChairID(), BuyBullet.BulletCountKind, lBulletCount);

	//		//设置变量
	//		m_BulletCountKindSelected=BuyBullet.BulletCountKind;
	//	}
	//}
	//重复判断
	if (NULL!=FindWindow(NULL,TEXT("充值"))) return ;
	//合法判断
	tagUserData const *pUserData=GetUserData(GetMeChairID());
	if (NULL==pUserData) return;

	//设置文件名
	TCHAR szPath[MAX_PATH]=TEXT(""),szIniFileName[MAX_PATH]=TEXT("");
	GetCurrentDirectory(CountArray(szPath),szPath);	
	_sntprintf(szIniFileName,CountArray(szIniFileName),TEXT("%s\\Config.ini"),szPath);

	TCHAR szBuffer[1024];
	GetPrivateProfileString(TEXT("QianCheng"),TEXT("Recharge"),TEXT("http://game.baihuogu.com"),szBuffer,CountArray(szBuffer),szIniFileName);
	if (IDYES==MessageBox(TEXT("您的金币不足，是否现在购买？"),TEXT("温馨提示"),MB_ICONQUESTION|MB_YESNO))
	{
		ShellExecute(NULL,TEXT("open"),szBuffer,NULL,NULL,SW_SHOWDEFAULT);
		PostMessage(WM_QUIT_GAME,0,0);
	}
}

//关闭函数
void CGameClientDlg::OnCancel()
{
	PostMessage(WM_QUIT_GAME,0,0);
}

//点击继续游戏
void CGameClientDlg::OnEventContinueGame()
{
}

//BOOL CGameClientDlg::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
//{
//	//////WM_NCHITTEST消息
//	////if(message==WM_NCHITTEST)
//	////{
//	////	int a=0;
//	////	//OnNcHitTest(wParam,lParam);
//	////	//return TRUE;
//	////}
//	//购买子弹(筹码)
//	if(message==WM_SHOW_COIN_SHOP)
//	{
//		ShowBulletShop();
//		return TRUE;
//	}
//	////发炮消息
//	//if(message==WM_SHOOT)
//	//{
//	//	OnShoot(wParam,lParam);
//	//	return TRUE;
//	//}
//	////退出游戏,
//	//if(message==WM_QUIT_GAME)
//	//{
//	//	OnQuitGame(wParam,lParam);
//	//	return TRUE;
//	//}
//	////击中鱼群
//	//if(message==WM_HIT_FISH)
//	//{
//	//	OnHitFish(wParam,lParam);
//	//	return TRUE;
//	//}
//	////激光打鱼(使用能量炮)
//	//if(message==WM_HIT_Laser)
//	//{
//	//	OnHitLaser(wParam,lParam);
//	//	return TRUE;
//	//}
//	return FALSE;
//	//return __super::OnWndMsg(message,wParam,lParam,pResult);
//}
//////////////////////////////////////////////////////////////////////////
