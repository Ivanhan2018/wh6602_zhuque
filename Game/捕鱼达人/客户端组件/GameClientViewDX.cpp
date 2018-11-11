#include "StdAfx.h"
#include "GameClientViewDX.h"
#include "Resource.h"
#include <mmsystem.h>
#include "Mathaide.h"
#include "Gameclientviewdx.h"
#include "GameClientDlg.h"
#include "GameHelpDlg.h"
#include "BulletHelpDlg.h"
#include ".\gameclientviewdx.h"

////////////////////////////////////////////////////////////////////////////////////
//时间定义
#define	IDI_IDLE_TIME					4									//空闲时间
#define	IDI_CHANGE_SCENE				5									//切换场景
#define	IDI_CHANGE_SCENE_EFFECT			6									//切换动画
#define	IDI_NO_BULLET_WARN				7									//子弹警告
#define	IDI_SHOOT_BULLET				8									//发射子弹

//时间定义
#define	TIME_DX_CREATE_SOUNT			400									//加载声音
#define	TIME_DX_LOAD_IMAGE				200									//加载图片
#define	TIME_CHANGE_SCENE_EFFECT		20									//加载图片
#define	TIME_CHANGE_SCENE				5000								//切换场景
#define	TIME_SHOOT_BULLET				500									//发射子弹

#define	TIME_SHOOT_SPEED_F				400									//发射速度
#define	TIME_SHOOT_SPEED_S				600									//发射速度

//#define WM_FISH_CALC_TRACE				WM_USER+300							//轨迹消息

#ifndef _DEBUG

#define	IDLE_TIME						120									//空闲时间

#else

#define	IDLE_TIME						120									//空闲时间

#endif

//按钮标识
enum enViewButtonsID
{
	enViewButtonsID_Begin=200,

	IDC_SHOW_BULLET_SHOP,								//购买子弹
	IDC_QUIT_GAME,										//退出游戏
	IDC_ACCOUNTS_WND,									//账号窗体
	IDC_ACCOUNTS_WND_1,									//账号窗体
	IDC_ACCOUNTS_WND_2,									//账号窗体
	IDC_ACCOUNTS_WND_3,									//账号窗体
	IDC_ACCOUNTS_WND_4,									//账号窗体
	IDC_SHOW_GAME_HELP,									//游戏帮助

	enViewButtonsID_End
};

////////////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CGameClientViewDX, CDxDialog)

	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_TIMER()

	ON_MESSAGE(WM_FISH_DISAPPEAR, OnFishDisappear)
	ON_MESSAGE(WM_FISH_CALC_TRACE, OnFinishCalcTrace)
	ON_MESSAGE(WM_NET_OPEN, OnNetOpen)

	ON_WM_RBUTTONDOWN()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_WM_SETCURSOR()
END_MESSAGE_MAP()


//计算线程
void ThreadCalcTrace(LPVOID pThreadData)
{
	//效验参数
	ASSERT(pThreadData!=NULL);
	if(pThreadData==NULL) return;

	//类型转换
	CGameClientViewDX *pGameClientViewDX=(CGameClientViewDX *)pThreadData;

	//计算轨迹
	while (true)
	{
		//等待信号
		WaitForSingleObject(pGameClientViewDX->m_hEventFinish,INFINITE);

		//退出判断
		if (true==pGameClientViewDX->m_bExist) break;

		//开始计算
		while (pGameClientViewDX->m_FishArrayBuffer.GetCount()>0)
		{
			//获取鱼类
			tagFishTraceInfo *pFishTraceInfo=&pGameClientViewDX->m_FishArrayBuffer[0];
			tagFishInfo *pFishInfo=pFishTraceInfo->pFishInfo;
			if (NULL==pFishInfo)
			{
				//	ASSERT(FALSE);
				TraceString(TEXT("if (NULL==pFishInfo)"));
				pGameClientViewDX->m_FishArrayBuffer.RemoveAt(0);
				continue;
			}
			if (true==pGameClientViewDX->m_bExist) break;

			//初始变量
			pFishInfo->wTracePtIndex=0;
			pFishInfo->byFrameIndex=0;
			pFishInfo->FishStatus=enFishStatus_Alive;
			pFishInfo->dwMoveTimes=GetTickCount();

			//计算轨迹
			CMathAide::BuildLinear(pFishTraceInfo->nInitX, pFishTraceInfo->nInitY, pFishTraceInfo->nInitCount, pFishInfo->ptTrace, CountArray(pFishInfo->ptTrace), pFishInfo->wTracePtCount, pFishTraceInfo->byMoveSpace);

			//合法判断
			if (pFishInfo->wTracePtCount<2) 
			{
				continue;
			}

			//首个角度
			LONG lDistance=CMathAide::CalcDistance(pFishInfo->ptTrace[pFishInfo->wTracePtCount-1].x,pFishInfo->ptTrace[pFishInfo->wTracePtCount-1].y,pFishInfo->ptTrace[0].x,pFishInfo->ptTrace[0].y);
			if (lDistance<=0) continue;
			float fSinValue=(pFishInfo->ptTrace[pFishInfo->wTracePtCount-1].x-pFishInfo->ptTrace[0].x)/(lDistance*1.0f);
			pFishInfo->fAngle=acos(fSinValue);

			//调整角度
			if (pFishInfo->ptTrace[pFishInfo->wTracePtCount/2-1].y<pFishInfo->ptTrace[0].y) 
			{
				pFishInfo->fAngle=2*D3DX_PI-pFishInfo->fAngle;
			}
			pFishInfo->fAngleStep=0.f;

			//退出判断
			if (true==pGameClientViewDX->m_bExist) break;
			if (true==pGameClientViewDX->m_bResetArray) break;
			if (pGameClientViewDX->m_FishArrayBuffer.GetCount()==0) break;			

			//移除缓存
			pGameClientViewDX->m_FishArrayActive.Add(pFishInfo);
			pGameClientViewDX->m_FishArrayBuffer.RemoveAt(0);

			//设置变量
			PostMessage(pGameClientViewDX->GetSafeHwnd(),WM_FISH_CALC_TRACE,0,0);
		}

		//退出判断
		if (true==pGameClientViewDX->m_bExist) break;
		else ResetEvent(pGameClientViewDX->m_hEventFinish);
	}

	//关闭句柄
	CloseHandle(pGameClientViewDX->m_hEventFinish);
	pGameClientViewDX->m_hEventFinish=NULL;

	return;
}

//////////////////////////////////////////////////////////////////////////////////////

//构造函数
CGameClientViewDX::CGameClientViewDX(VOID)
{
	//用户信息
	ZeroMemory(m_lBulletCount,sizeof(m_lBulletCount));
	ZeroMemory(m_lCoinCount,sizeof(m_lCoinCount));
	ZeroMemory(m_lBulletCharge,sizeof(m_lBulletCharge));
	ZeroMemory(m_wCptrFishCount,sizeof(m_wCptrFishCount));
	m_bLookonMode=false;

	//游戏变量
	m_bReleaseKey=true;
	m_bWaitScoreInfo=false;
	m_bInitControlTag=false;
	m_dwDelayTimes=8;
	m_dwCurTimes=GetTickCount();
	m_wMeChairID=INVALID_CHAIR;
	m_dwLastShootTime=GetTickCount();
	m_wIdleTime=IDLE_TIME;
	m_CurSeaScene=enSeaSceneKind_No;
	m_bNoBulletWarn=false;
	m_dwNoBulletWarnTime=0L;
	for (int i=0; i<GAME_PLAYER; ++i) m_lCellScore[i]=1;
	m_dwLastCalcTime=GetTickCount();
	m_dwLastSwitchTime=GetTickCount();

	//彩金变量
	m_lBonusCount=0L;

	//涨潮变量
	m_nSceneMovePosX=0;
	m_NewSeaScene=enSeaSceneKind_1;
	m_bWaveLToR=true;
	m_bWaveEffect=false;
	m_bWaveComing=false;

	//内核变量
	m_hEventFinish=NULL;
	m_hCalcThread=NULL;
	m_bExist=false;
	m_bResetArray=false;
	m_bDeviceReset=false;

	//资源变量
	m_pFontUserInfo=NULL;
	m_pFontGameInfo=NULL;

	//组件变量
	m_pGameClientDlg=NULL;

	//线程变量
	m_bLoadResFinish=false;

	///////////////////////////////////////////////
	/*m_CurSeaScene=enSeaSceneKind_3;
	m_wMeChairID=1;*/
	///////////////////////////////////////////////
}

//析构函数
CGameClientViewDX::~CGameClientViewDX(VOID)
{
	ReleaseScene();
}

//释放资源
VOID CGameClientViewDX::ReleaseScene()
{
	//设置标识
	m_bExist=true;
	if (NULL!=m_hEventFinish) SetEvent(m_hEventFinish);

	//结束线程
	WaitForSingleObject(m_hCalcThread,2000);
	CloseHandle(m_hCalcThread);

	//释放图片
	m_ToolBarBk.Release();
	m_Cannon.Release();
	m_Bullet.Release();
	m_Coin.Release();
	m_Bonus.Release();
	m_UserInfo.Release();
	m_Number.Release();
	if (m_Wave.IsLoaded()==true) m_Wave.Release();
	m_NoBulletWarn.Release();
	m_WaveEffect.Release();
	m_GameTip.Release();
	for (enSeaSceneKind SeaSceneKind=enSeaSceneKind_1; SeaSceneKind<enSeaSceneKind_No; SeaSceneKind=enSeaSceneKind(SeaSceneKind+1))
	{
		if (m_SeaScene[SeaSceneKind].IsLoaded()==true) m_SeaScene[SeaSceneKind].Release();
	}

	//释放资源
	SAFE_RELEASE(m_pFontUserInfo);
	SAFE_RELEASE(m_pFontGameInfo);

	//释放鱼群
	for (enFishKind FishKind=enFishKind_1; FishKind<enFishKind_No; FishKind=enFishKind(FishKind+1))
	{
		g_FishGroup[FishKind].Release();
		g_FishGroup[FishKind].ResetKenel();
	}

	return ;
}

//载入资源
VOID CGameClientViewDX::InitScene(INT nWidth,INT nHeight)
{
	//自己座位
	m_Cannon.SetMeChairID(m_wMeChairID);

	//设置位置
	InitPosition();

	//初始数组
	m_FishArrayBuffer.RemoveAll();
	m_FishArrayActive.RemoveAll();

	//设置标识
	m_bResetArray=false;

	//创建字体
	SAFE_RELEASE(m_pFontUserInfo);
	SAFE_RELEASE(m_pFontGameInfo);	
	D3DXCreateFont(g_pD3DDevice,14,0,0,1,0,ANSI_CHARSET,0,0,0,TEXT("新宋体"),&m_pFontUserInfo);
	D3DXCreateFont(g_pD3DDevice,18,0,0,0,0,DEFAULT_CHARSET,0,0,0,TEXT("新宋体"),&m_pFontGameInfo);

	//初始鱼群
	for (enFishKind FishKind=enFishKind_1; FishKind<enFishKind_No; FishKind=enFishKind(FishKind+1))
	{
		g_FishGroup[FishKind].SetGroupInfo(this->GetSafeHwnd(),FishKind);
	}

	//加载资源
	DWORD dwStart = GetTickCount();

	//加载鱼群
	LoadFish();

	//加载图片	
	if (!m_bInitControlTag) 
	{
		LoadDXImage();
		LoadSeaScene(m_CurSeaScene);
	}
	else 
	{
		OnResetDevice();
	}

	//初始控件
	if (!m_bInitControlTag)
	{
		//设置变量
		m_bInitControlTag = true;
	}

	//资源位置
	m_ToolBarBk.SetDrawPos(nWidth/2-m_ToolBarBk.GetImageWidth()/2,nHeight-m_ToolBarBk.GetImageHeight());
	m_Cannon.SetNotifyWnd(GetSafeHwnd());

	//子弹配置
	CRect rcBound;
	GetClientRect(rcBound);
	rcBound.DeflateRect(20,20,20,20);
	m_Bullet.SetNotifyWnd(GetSafeHwnd());
	m_Bullet.SetBound(rcBound);

	//启动线程
	if (NULL==m_hCalcThread)
	{
		UINT uThreadID=0;
		m_hEventFinish=CreateEvent(NULL,FALSE,FALSE,NULL);
		m_hCalcThread=(HANDLE)::CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)ThreadCalcTrace,LPVOID(this),0,0);		
	}

	return ;
}

//播放标志
VOID CGameClientViewDX::EnablePlaySound(bool bEnable)
{
	return ;
}

//艺术字体
VOID CGameClientViewDX::DrawTextString(LPD3DXFONT pFont, LPCWSTR pszString, D3DCOLOR crText, int nXPos, int nYPos)
{
	//绘画字体
	RECT Rect;
	Rect.left = nXPos;	
	Rect.top = nYPos;
	Rect.right = 0;	Rect.bottom = 0;
	pFont->DrawTextW(NULL, pszString, (int)wcslen(pszString), &Rect,DT_SINGLELINE|DT_NOCLIP|DT_LEFT|DT_TOP, crText|0xFF000000);

	return;
}

//游戏信息
void CGameClientViewDX::DrawGameInfo()
{
	//获取大小
	CRect rcClient;
	GetClientRect(&rcClient);
	rcClient.top=160;

	//变量定义
	static WCHAR szBuffer[128]={0};
	static CRect ImageRect;

	//退出提示
	if (m_wIdleTime>0 && m_wIdleTime<=60)
	{
		//输出信息
		_snwprintf(szBuffer,CountArray(szBuffer),L"超过60秒没发炮，程序即将退出。(%d)",m_wIdleTime);
		m_pFontGameInfo->DrawTextW(NULL, szBuffer, (int)wcslen(szBuffer), &rcClient,DT_SINGLELINE|DT_NOCLIP|DT_CENTER|DT_TOP|DT_CALCRECT, RGB(0,255,255)|0xFF000000);

		//输出背景
		ImageRect.left=0;
		ImageRect.top=0;
		ImageRect.right=m_GameTip.GetImageWidth(0);
		ImageRect.bottom=m_GameTip.GetImageHeight(0);
		rcClient.left-=10;
		rcClient.right+=10;
		rcClient.bottom+=2*rcClient.Height();
		m_GameTip.DrawRotateAndZoom(rcClient.left+10,rcClient.top-rcClient.Height()/3,ImageRect,0.f,rcClient.Width()/float(ImageRect.Width()),rcClient.Height()/float(ImageRect.Height()),0,enXLeft,enYTop);

		//输出信息
		m_pFontGameInfo->DrawTextW(NULL, szBuffer, (int)wcslen(szBuffer), &rcClient,DT_SINGLELINE|DT_NOCLIP|DT_CENTER|DT_TOP, RGB(0,255,255)|0xFF000000);
	}

	//涨潮通知
	if (true==m_bWaveComing)
	{

		//输出信息
		_snwprintf(szBuffer,CountArray(szBuffer),L"潮水即将来临，鱼群即将出现，各位渔民请上好子弹……");
		m_pFontGameInfo->DrawTextW(NULL, szBuffer, (int)wcslen(szBuffer), &rcClient,DT_SINGLELINE|DT_NOCLIP|DT_CENTER|DT_TOP|DT_CALCRECT, RGB(0,255,255)|0xFF000000);

		//输出背景
		ImageRect.left=0;
		ImageRect.top=0;
		ImageRect.right=m_GameTip.GetImageWidth(0);
		ImageRect.bottom=m_GameTip.GetImageHeight(0);
		rcClient.left-=10;
		rcClient.right+=10;
		rcClient.bottom+=2*rcClient.Height();
		m_GameTip.DrawRotateAndZoom(rcClient.left+85,rcClient.top-rcClient.Height()/3,ImageRect,0.f,rcClient.Width()/float(ImageRect.Width()),rcClient.Height()/float(ImageRect.Height()),0,enXLeft,enYTop);

		//输出信息
		m_pFontGameInfo->DrawTextW(NULL, szBuffer, (int)wcslen(szBuffer), &rcClient,DT_SINGLELINE|DT_NOCLIP|DT_CENTER|DT_TOP, RGB(0,255,255)|0xFF000000);
	}

	//获取分数
	if (true==m_bWaitScoreInfo)
	{
		//输出信息
		_snwprintf(szBuffer,CountArray(szBuffer),L"正在获取信息，请稍后……");
		m_pFontGameInfo->DrawTextW(NULL, szBuffer, (int)wcslen(szBuffer), &rcClient,DT_SINGLELINE|DT_NOCLIP|DT_CENTER|DT_TOP|DT_CALCRECT, RGB(0,255,255)|0xFF000000);

		//输出背景
		ImageRect.left=0;
		ImageRect.top=0;
		ImageRect.right=m_GameTip.GetImageWidth(0);
		ImageRect.bottom=m_GameTip.GetImageHeight(0);
		rcClient.left-=10;
		rcClient.right+=10;
		rcClient.bottom+=2*rcClient.Height();
		m_GameTip.DrawRotateAndZoom(rcClient.left-30,rcClient.top-rcClient.Height()/3,ImageRect,0.f,rcClient.Width()/float(ImageRect.Width()),rcClient.Height()/float(ImageRect.Height()),0,enXLeft,enYTop);

		//输出信息
		m_pFontGameInfo->DrawTextW(NULL, szBuffer, (int)wcslen(szBuffer), &rcClient,DT_SINGLELINE|DT_NOCLIP|DT_CENTER|DT_TOP, RGB(0,255,255)|0xFF000000);
	}
}

//更新输入
//VOID CGameClientViewDX::UpdateControl()
//{
//	if(&g_UserControl==NULL) return;
//	if (INVALID_CHAIR==m_wMeChairID) return;
//
//	//旁观判断
//	if (true==m_bLookonMode) return;
//
//	//激活判断
//	CWnd *pActiveWnd=GetActiveWindow();
//	CWnd *pMainFrame=AfxGetMainWnd();
//	if (NULL==pActiveWnd) return;
//	if (NULL==pMainFrame) return;
//	if (pActiveWnd->GetSafeHwnd()!=pMainFrame->GetSafeHwnd()) return;
//
//	//更新输入
//	g_UserControl.UpdateInput();
//
//	//数字信息
//	WORD wKeyNumberIndex[]={DIK_1,DIK_2,DIK_3,DIK_4,DIK_5,DIK_SPACE,DIK_LEFT,DIK_RIGHT,DIK_UP,DIK_DOWN};
//	for(WORD i=0;i<CountArray(wKeyNumberIndex);i++)
//	{
//		int nIndex=wKeyNumberIndex[i];
//		if(nIndex<CountArray(g_UserControl.m_cbKeyState) && g_UserControl.m_cbKeyState[nIndex]&0x80)
//		{
//			//数字判断
//			if (i<=4)
//			{
//				LONG nMinCount=min((i+1),GetCurBulletCount(m_wMeChairID));
//				m_Cannon.SetBulletCount(m_wMeChairID,nMinCount,true);
//				
//				//播放声音
//				PlayMp3Sound(TEXT("Effect\\SwitchSilo.mp3"),false);
//			}
//			else if (DIK_SPACE==wKeyNumberIndex[i] || DIK_UP==wKeyNumberIndex[i])
//			{
//				//空格判断
//				if (DIK_SPACE==wKeyNumberIndex[i])
//				{
//					//激活判断
//					CWnd *pFocusWnd=GetFocus();
//					if (NULL==pFocusWnd) return;
//					if (pFocusWnd->GetSafeHwnd()!=GetSafeHwnd()) return;
//				}
//
//				//发射判断
//				UserShoot(m_wMeChairID);
//			}
//			else if (DIK_LEFT==wKeyNumberIndex[i])
//			{
//				//设置焦点
//				SetFocus();
//
//				//设置角度
//				float fNowAngle=m_Cannon.GetAngleCan(m_wMeChairID);
//				fNowAngle-=0.02f;
//				if (fNowAngle<ME_MIN_CAN_ANGLE) fNowAngle=ME_MIN_CAN_ANGLE;
//				m_Cannon.SetAngleCan(m_wMeChairID,fNowAngle);
//			}
//			else if (DIK_RIGHT==wKeyNumberIndex[i])
//			{
//				//设置焦点
//				SetFocus();
//
//				//设置角度
//				float fNowAngle=m_Cannon.GetAngleCan(m_wMeChairID);
//				fNowAngle+=0.02f;
//				if (fNowAngle>ME_MAX_CAN_ANGLE) fNowAngle=ME_MAX_CAN_ANGLE;
//				m_Cannon.SetAngleCan(m_wMeChairID,fNowAngle);
//			}
//			else if (DIK_DOWN==wKeyNumberIndex[i])
//			{
//				//切换炮弹
//				if (m_dwLastSwitchTime+300<=GetTickCount())
//				{
//					SwitchCannon();	
//					m_dwLastSwitchTime=GetTickCount();
//				}
//			}			
//		}
//	}
//
//	return;
//}

DWORD g_dwCurTimes=0;;

//绘画函数
VOID CGameClientViewDX::DrawGameView2D()
{
	//合法判断
	if (false==m_bLoadResFinish) return;
	if (true==m_bDeviceReset) return;
	if (m_wMeChairID>=GAME_PLAYER) return;

	//时间判断
	if((m_dwCurTimes+m_dwDelayTimes)>= GetTickCount()) return;
	m_dwCurTimes=GetTickCount();

	//DWORD dwBeginTime= timeGetTime();

	//开始绘画
	if (CDxSkin::m_Sprite!=NULL) CDxSkin::m_Sprite->Begin(D3DXSPRITE_ALPHABLEND|D3DXSPRITE_SORT_TEXTURE);

	//绘画背景
	if (m_CurSeaScene<enSeaSceneKind_No) m_SeaScene[m_CurSeaScene].DrawScene();

	//动画判断
	if (true==m_bWaveEffect)
	{
		//获取大小
		CRect rcClient;
		GetClientRect(&rcClient);

		//偏移位置
		int nExcursionX=(rcClient.Width()-m_SeaScene[m_NewSeaScene].GetImageWidth())/2;
		int nExcursionY=(rcClient.Height()-m_ToolBarBk.GetImageHeight()+24-m_SeaScene[m_NewSeaScene].GetImageHeight())/2;

		//方向判断
		if (true==m_bWaveLToR)
		{
			//绘画场景
			CRect rcDraw;
			if (true==m_SeaScene[m_NewSeaScene].IsReverse())
			{
				rcDraw.SetRect(m_SeaScene[m_NewSeaScene].GetImageWidth()-m_nSceneMovePosX,0,m_SeaScene[m_NewSeaScene].GetImageWidth(),m_SeaScene[m_NewSeaScene].GetImageHeight());
				m_SeaScene[m_NewSeaScene].DrawRotateAndZoom(nExcursionX+rcDraw.Width(),nExcursionY+rcDraw.Height(),rcDraw,D3DX_PI,1.f,1.f,0,enXLeft,enYTop);
			}
			else
			{
				rcDraw.SetRect(0,0,m_nSceneMovePosX,m_SeaScene[m_NewSeaScene].GetImageHeight());
				m_SeaScene[m_NewSeaScene].DrawSprite(nExcursionX,nExcursionY,rcDraw);
			}

			//绘画海浪
			rcDraw.SetRect(0,0,m_Wave.GetImageWidth(),m_Wave.GetImageHeight());
			m_Wave.DrawRotateAndZoom(m_nSceneMovePosX+nExcursionX+30,nExcursionY+m_Wave.GetImageHeight(),rcDraw,D3DX_PI,1.f,1.f,0,enXLeft,enYTop);
		}
		else
		{
			//绘画场景
			CRect rcClient;
			GetClientRect(&rcClient);
			CRect rcDraw;
			if (true==m_SeaScene[m_NewSeaScene].IsReverse())
			{
				rcDraw.SetRect(0,0,m_nSceneMovePosX,m_SeaScene[m_NewSeaScene].GetImageHeight());
				m_SeaScene[m_NewSeaScene].DrawRotateAndZoom(rcClient.right-m_nSceneMovePosX-nExcursionX+rcDraw.Width(),nExcursionY+rcDraw.Height(),rcDraw,D3DX_PI,1.f,1.f,0,enXLeft,enYTop);
			}
			else
			{
				rcDraw.SetRect(rcClient.right-m_nSceneMovePosX,0,m_SeaScene[m_NewSeaScene].GetImageWidth(),m_SeaScene[m_NewSeaScene].GetImageHeight());
				m_SeaScene[m_NewSeaScene].DrawRotateAndZoom(rcClient.right-m_nSceneMovePosX-nExcursionX,nExcursionY,rcDraw,0.f,1.f,1.f,0,enXLeft,enYTop);
			}

			//绘画海浪
			rcDraw.SetRect(0,0,m_Wave.GetImageWidth(),m_Wave.GetImageHeight());
			m_Wave.DrawRotateAndZoom(rcClient.right-m_nSceneMovePosX-nExcursionX-30,nExcursionY,rcDraw,0.f,1.f,1.f,0,enXLeft,enYTop);
		}		
	}

	//绘画鱼群
	for (enFishKind FishKind=enFishKind_1; FishKind<enFishKind_No; FishKind=enFishKind(FishKind+1))
	{
		g_FishGroup[FishKind].DrawFish();
	}

	//绘制子弹
	m_Bullet.DrawBullet();

	//绘画大炮
	m_Cannon.DrawCannon();

	//绘画金币
	m_Coin.DrawCoin();

	//绘画彩金
	m_Bonus.DrawBonus();

	//水纹效果
	bool static bFlag=true;
	static DWORD dwTime=GetTickCount();
	static float fScaling=4.0f;
	if (dwTime+200<=GetTickCount())
	{
		dwTime=GetTickCount();

		if (false==bFlag)
		{
			if (fScaling<4.2f) fScaling+=0.01f;
			if (fScaling>=4.2f) bFlag=true;
		}
		else
		{
			if (fScaling>4.0f) fScaling-=0.01f;
			if (fScaling<=4.0f) bFlag=false;
		}	
	}

	BYTE byChannel=190;
	CRect drawRect;
	drawRect.left=0;
	drawRect.top=0;
	drawRect.right=m_WaveEffect.GetImageWidth(0);
	drawRect.bottom=m_WaveEffect.GetImageHeight(0);
	m_WaveEffect.DrawRotateAndZoom(205,205,drawRect,0.f,fScaling,fScaling,0,enXLeft,enYTop,RGB(byChannel,byChannel,byChannel));

	//等待分数
	if (true==m_bWaitScoreInfo)
	{
		drawRect.left=0;
		drawRect.top=0;
		drawRect.right=m_WaveEffect.GetImageWidth(0);
		drawRect.bottom=m_WaveEffect.GetImageHeight(0);
		m_WaveEffect.DrawRotateAndZoom(205,205,drawRect,0.f,fScaling,fScaling,0,enXLeft,enYTop,RGB(byChannel,byChannel,byChannel));
	}	

	//个人信息
	DrawUserInfo();

	//工具栏
	m_ToolBarBk.OnDrawImage();

	//绘画彩金
	if (m_lBonusCount>0L)
	{
		//输出信息
		static WCHAR szBuffer[128]={0};
		CRect rcClient(398,569,642,590);
		_snwprintf(szBuffer,CountArray(szBuffer),L"黄金鲨鱼：%ld 彩金",m_lBonusCount);
		m_pFontGameInfo->DrawTextW(NULL, szBuffer, (int)wcslen(szBuffer), &rcClient,DT_SINGLELINE|DT_NOCLIP|DT_CENTER|DT_VCENTER, RGB(0,255,255)|0xFF000000);
	}

	//警告图片
	if (true==m_bNoBulletWarn)
	{
		static bool bShow=true;
		if (true==bShow) m_NoBulletWarn.OnDrawImage();
		if (m_dwNoBulletWarnTime+800<GetTickCount())
		{
			bShow=!bShow;
			m_dwNoBulletWarnTime=GetTickCount();
		}
	}

	//游戏信息
	DrawGameInfo();

	//结束绘画
	if (CDxSkin::m_Sprite!=NULL) CDxSkin::m_Sprite->End();

	/*DWORD dwEndTime= timeGetTime();

	if ((dwEndTime-dwBeginTime)!=0L)
	{
	TCHAR szBuffer[256];
	m_pGameClientDlg->InsertSystemString(_itoa((dwEndTime-dwBeginTime),szBuffer,10));
	}*/

	return;
}

//右键消息
void CGameClientViewDX::OnRButtonDown(UINT nFlags, CPoint point)
{
	//切换炮弹
	SwitchCannon();	

	__super::OnRButtonDown(nFlags, point);
}

//鼠标左击
VOID CGameClientViewDX::OnLButtonDown(UINT nFlags, CPoint Point)
{
	//调用父类
	__super::OnLButtonDown(nFlags, Point);

	//合法判断
	if (INVALID_CHAIR==m_wMeChairID) return;

	//旁观判断
	if (true==m_bLookonMode) return;

	//设置焦点
	SetFocus();

	//玩家射击
	if (true == _UserShoot(m_wMeChairID))
	{
		SetTimer(IDI_SHOOT_BULLET,TIME_SHOOT_BULLET,NULL);
	}
	else
	{
		char sz[100]={0};
		OutputDebugString("false == _UserShoot");
	}

	//设置捕获
	SetCapture();

	return ;
}

//鼠标左击
VOID CGameClientViewDX::OnLButtonUp(UINT nFlags, CPoint point)
{
	__super::OnLButtonUp(nFlags, point);

	//关闭定时
	KillTimer(IDI_SHOOT_BULLET);

	//释放捕获
	ReleaseCapture();

	return ;
}

//鼠标移动
VOID CGameClientViewDX::OnMouseMove(UINT nFlags, CPoint Point)
{
	__super::OnMouseMove(nFlags, Point);

	//合法判断
	if (INVALID_CHAIR==m_wMeChairID) return;

	//旁观判断
	if (true==m_bLookonMode) return;

	//获取鼠标
	CPoint ptCusor;
	::GetCursorPos(&ptCusor);
	ScreenToClient(&ptCusor);

	//基准位置
	POINT ptBenchmarkPos={0};
	ptBenchmarkPos=m_Cannon.GetBenchmarkPos(m_wMeChairID);

	//计算角度
	LONG lDistance=CMathAide::CalcDistance(ptCusor.x,ptCusor.y,ptBenchmarkPos.x,ptBenchmarkPos.y);
	if (lDistance<=0) return;
	float fSinValue=(ptCusor.x-ptBenchmarkPos.x)/(lDistance*1.0f);
	float fAngle=acos(fSinValue);

	//调整角度
	if (ptCusor.y<ptBenchmarkPos.y)
	{
		fAngle=2*D3DX_PI-fAngle;
	}
	fAngle+=D3DX_PI/2;

	//设置角度
	m_Cannon.SetAngleCan(m_wMeChairID,fAngle);
	//for (WORD wChairID=0; wChairID<GAME_PLAYER; ++wChairID)
	//{
	//	if (IsSameSide(wChairID,m_wMeChairID)) 
	//		m_Cannon.SetAngleCan(wChairID,fAngle);
	//	else
	//		m_Cannon.SetAngleCan(wChairID,SwitchAngle(fAngle));
	//}

	return ;
}

//定时消息
void CGameClientViewDX::OnTimer(UINT nIDEvent)
{
	switch(nIDEvent)
	{
	case IDI_SHOOT_BULLET:				//发射子弹
		{
			if (false==_UserShoot(m_wMeChairID,false) && (m_dwLastShootTime+TIME_SHOOT_SPEED_F)>GetTickCount())
			{
				KillTimer(IDI_SHOOT_BULLET);
			}

			return;
		}
	case IDI_IDLE_TIME:					//空闲时间
		{
			//合法判断
			if (INVALID_CHAIR==m_wMeChairID && m_wIdleTime<60)
			{
				KillTimer(IDI_IDLE_TIME);
				return;
			}

			//关闭判断
			if (m_wIdleTime>0) m_wIdleTime-=1;
			else AfxGetMainWnd()->PostMessage(WM_QUIT_GAME,0,0);

			break;
		}
	case IDI_CHANGE_SCENE_EFFECT:			//涨潮过程
		{
			//修改位置
			m_nSceneMovePosX+=10;

			//结束判断
			CRect rcClient;
			GetClientRect(&rcClient);
			if (m_nSceneMovePosX>=rcClient.Width())
			{
				//关闭定时
				KillTimer(IDI_CHANGE_SCENE_EFFECT);

				//释放资源
				if (m_CurSeaScene<enSeaSceneKind_No) ReleaseSeaScene(m_CurSeaScene);
				m_Wave.Release();

				//设置变量
				m_CurSeaScene=m_NewSeaScene;
				m_SeaScene[m_NewSeaScene].SetTride(false);
				m_NewSeaScene=enSeaSceneKind_No;	

				//设置标识
				m_bWaveEffect=false;
				m_bWaveComing=false;
				CFishGroup::SetWaveComing(false);				
			}

			break;
		}
	case IDI_CHANGE_SCENE:				//涨潮准备
		{
			//关闭时间
			KillTimer(IDI_CHANGE_SCENE);

			//加载资源
			LoadSeaScene(m_NewSeaScene);

			//加载海浪
			TCHAR szFileName[MAX_PATH];
			_sntprintf(szFileName,CountArray(szFileName),TEXT("%sFish\\SeaScene\\WAVE.dds"),g_szWorkDir);
			m_Wave.SetLoadInfo(g_pD3DDevice,szFileName,0,D3DPOOL_MANAGED);

			//设置标识
			m_bWaveEffect=true;

			//设置时间
			SetTimer(IDI_CHANGE_SCENE_EFFECT,TIME_CHANGE_SCENE_EFFECT,NULL);

			break;
		}
	case IDI_NO_BULLET_WARN:			//子弹警告
		{
			//关闭定时
			KillTimer(IDI_NO_BULLET_WARN);

			//设置标识
			m_bNoBulletWarn=false;

			break;
		}
	}
	return ;
}

//加载场景
void CGameClientViewDX::LoadSeaScene(enSeaSceneKind SeaSceneKind)
{
	//获取大小
	CRect rcClient;
	GetClientRect(&rcClient);

	//变量定义
	TCHAR szFileName[MAX_PATH];
	BYTE byTextureIndex=0;
	BYTE wLestTimes=50, wRandTimes=40, wMultiple=4;
	BYTE wSpaceTime=wLestTimes+rand()%wMultiple*wRandTimes;
	D3DPOOL Pool=D3DPOOL_MANAGED;

	//海浪效果
	if (m_WaveEffect.IsLoaded()==false)
	{
		_sntprintf(szFileName,CountArray(szFileName),TEXT("%sFish\\SeaScene\\MASK.dds"),g_szWorkDir);
		m_WaveEffect.SetLoadInfo(g_pD3DDevice,szFileName,0,Pool);
	}

	if (enSeaSceneKind_1==SeaSceneKind)
	{
		//重置场景
		m_SeaScene[enSeaSceneKind_1].ResetScene();

		//加载海底
		_sntprintf(szFileName,CountArray(szFileName),TEXT("%sFish\\SeaScene\\SCENE_1.dds"),g_szWorkDir);
		m_SeaScene[enSeaSceneKind_1].SetLoadInfo(g_pD3DDevice,szFileName,0,Pool);
		m_SeaScene[enSeaSceneKind_1].SetReverse(true);

		_sntprintf(szFileName,CountArray(szFileName),TEXT("%sFish\\SeaScene\\BUBBLE.dds"),g_szWorkDir);
		m_SeaScene[enSeaSceneKind_1].SetLoadInfo(g_pD3DDevice,szFileName,MAX_TEXTURE_COUNT-1,Pool);

		//海底生物
		_sntprintf(szFileName,CountArray(szFileName),TEXT("%sFish\\SeaScene\\BIOLOGY_1.dds"),g_szWorkDir);
		m_SeaScene[enSeaSceneKind_1].SetLoadInfo(g_pD3DDevice,szFileName,1,Pool);

		//设置位置
		m_SeaScene[enSeaSceneKind_1].SetDrawPos(rcClient.Width()/2-m_SeaScene[enSeaSceneKind_1].GetImageWidth(byTextureIndex)/2+m_SeaScene[enSeaSceneKind_1].GetImageWidth(byTextureIndex),
			(rcClient.Height()-m_ToolBarBk.GetImageHeight()+24)/2-m_SeaScene[enSeaSceneKind_1].GetImageHeight(byTextureIndex)/2+m_SeaScene[enSeaSceneKind_1].GetImageHeight(byTextureIndex),byTextureIndex);

		byTextureIndex=1;
		m_SeaScene[enSeaSceneKind_1].SetImageSize(m_SeaScene[enSeaSceneKind_1].GetImageWidth(byTextureIndex)/5,m_SeaScene[enSeaSceneKind_1].GetImageHeight(byTextureIndex)/4,byTextureIndex);
		m_SeaScene[enSeaSceneKind_1].AddBiology(rcClient.Width()/2-70-126,rcClient.Height()/2+268-61,20,byTextureIndex,5,0.f,wSpaceTime);

		byTextureIndex=1;
		wSpaceTime=wLestTimes+rand()%wMultiple*wRandTimes;
		m_SeaScene[enSeaSceneKind_1].AddBiology(rcClient.Width()/2-135-162,rcClient.Height()/2-225-153,20,byTextureIndex,5,0.f,wSpaceTime);

		byTextureIndex=1;
		wSpaceTime=wLestTimes+rand()%wMultiple*wRandTimes;
		m_SeaScene[enSeaSceneKind_1].AddBiology(rcClient.Width()/2-280-143,rcClient.Height()/2-110-139,20,byTextureIndex,5,0.f,wSpaceTime);

		byTextureIndex=1;
		wSpaceTime=wLestTimes+rand()%wMultiple*wRandTimes;
		m_SeaScene[enSeaSceneKind_1].AddBiology(rcClient.Width()/2+60+171,rcClient.Height()/2+170+149,20,byTextureIndex,5,D3DX_PI,wSpaceTime);

		byTextureIndex=1;
		wSpaceTime=wLestTimes+rand()%wMultiple*wRandTimes;
		m_SeaScene[enSeaSceneKind_1].AddBiology(rcClient.Width()/2-412,rcClient.Height()/2+180,20,byTextureIndex,5,D3DX_PI,wSpaceTime);

		byTextureIndex=1;
		wSpaceTime=wLestTimes+rand()%wMultiple*wRandTimes;
		m_SeaScene[enSeaSceneKind_1].AddBiology(rcClient.Width()/2+10+175,rcClient.Height()/2-305+65,20,byTextureIndex,5,D3DX_PI,wSpaceTime);

		byTextureIndex=1;
		wSpaceTime=wLestTimes+rand()%wMultiple*wRandTimes;
		m_SeaScene[enSeaSceneKind_1].AddBiology(rcClient.Width()/2+252+180,rcClient.Height()/2+137,20,byTextureIndex,5,D3DX_PI/2,wSpaceTime,1.2f);

		byTextureIndex=1;
		wSpaceTime=wLestTimes+rand()%wMultiple*wRandTimes;
		m_SeaScene[enSeaSceneKind_1].AddBiology(rcClient.Width()/2+350,rcClient.Height()/2-50,20,byTextureIndex,5,D3DX_PI/3,wSpaceTime,0.8f);

		byTextureIndex=1;
		wSpaceTime=wLestTimes+rand()%wMultiple*wRandTimes;
		m_SeaScene[enSeaSceneKind_1].AddBiology(rcClient.Width()/2+470,rcClient.Height()/2,20,byTextureIndex,5,D3DX_PI/4,wSpaceTime,1.1f);

		byTextureIndex=1;
		wSpaceTime=wLestTimes+rand()%wMultiple*wRandTimes;
		m_SeaScene[enSeaSceneKind_1].AddBiology(rcClient.Width()/2+460,rcClient.Height()/2-100,20,byTextureIndex,5,D3DX_PI/5,wSpaceTime,0.7f);

		byTextureIndex=1;
		wSpaceTime=wLestTimes+rand()%wMultiple*wRandTimes;
		m_SeaScene[enSeaSceneKind_1].AddBiology(rcClient.Width()/2+440,rcClient.Height()/2-220,20,byTextureIndex,5,D3DX_PI/6,wSpaceTime,0.9f);
	}
	else if (enSeaSceneKind_2==SeaSceneKind)
	{
		//重置场景
		m_SeaScene[enSeaSceneKind_2].ResetScene();

		//加载海底
		_sntprintf(szFileName,CountArray(szFileName),TEXT("%sFish\\SeaScene\\SCENE_2.dds"),g_szWorkDir);
		m_SeaScene[enSeaSceneKind_2].SetLoadInfo(g_pD3DDevice,szFileName,0,Pool);

		_sntprintf(szFileName,CountArray(szFileName),TEXT("%sFish\\SeaScene\\BUBBLE.dds"),g_szWorkDir);
		m_SeaScene[enSeaSceneKind_2].SetLoadInfo(g_pD3DDevice,szFileName,MAX_TEXTURE_COUNT-1,Pool);

		//海底生物
		_sntprintf(szFileName,CountArray(szFileName),TEXT("%sFish\\SeaScene\\BIOLOGY_12.dds"),g_szWorkDir);
		m_SeaScene[enSeaSceneKind_2].SetLoadInfo(g_pD3DDevice,szFileName,1,Pool);
		_sntprintf(szFileName,CountArray(szFileName),TEXT("%sFish\\SeaScene\\BIOLOGY_14.dds"),g_szWorkDir);
		m_SeaScene[enSeaSceneKind_2].SetLoadInfo(g_pD3DDevice,szFileName,2,Pool);
		_sntprintf(szFileName,CountArray(szFileName),TEXT("%sFish\\SeaScene\\BIOLOGY_16.dds"),g_szWorkDir);
		m_SeaScene[enSeaSceneKind_2].SetLoadInfo(g_pD3DDevice,szFileName,3,Pool);

		//设置位置
		byTextureIndex=0;
		m_SeaScene[enSeaSceneKind_2].SetDrawPos(rcClient.Width()/2-m_SeaScene[enSeaSceneKind_2].GetImageWidth(byTextureIndex)/2,(rcClient.Height()-m_ToolBarBk.GetImageHeight()+24)/2-m_SeaScene[enSeaSceneKind_2].GetImageHeight(byTextureIndex)/2,byTextureIndex);

		byTextureIndex=1;
		wLestTimes=60, wRandTimes=40, wMultiple=4;
		wSpaceTime=wLestTimes+rand()%wMultiple*wRandTimes;
		m_SeaScene[enSeaSceneKind_2].SetImageSize(m_SeaScene[enSeaSceneKind_2].GetImageWidth(byTextureIndex)/8,m_SeaScene[enSeaSceneKind_2].GetImageHeight(byTextureIndex)/6,byTextureIndex);
		m_SeaScene[enSeaSceneKind_2].AddBiology(rcClient.Width()/2-355-109,rcClient.Height()/2+30-110,43,byTextureIndex,8,0.f,wSpaceTime);

		byTextureIndex=1;
		wLestTimes=60, wRandTimes=40, wMultiple=4;
		wSpaceTime=wLestTimes+rand()%wMultiple*wRandTimes;
		m_SeaScene[enSeaSceneKind_2].SetImageSize(m_SeaScene[enSeaSceneKind_2].GetImageWidth(byTextureIndex)/8,m_SeaScene[enSeaSceneKind_2].GetImageHeight(byTextureIndex)/6,byTextureIndex);
		m_SeaScene[enSeaSceneKind_2].AddBiology(rcClient.Width()/2+370+109,rcClient.Height()/2+190+110,43,byTextureIndex,8,D3DX_PI,wSpaceTime);

		byTextureIndex=1;
		wLestTimes=60, wRandTimes=40, wMultiple=4;
		wSpaceTime=wLestTimes+rand()%wMultiple*wRandTimes;
		m_SeaScene[enSeaSceneKind_2].SetImageSize(m_SeaScene[enSeaSceneKind_2].GetImageWidth(byTextureIndex)/8,m_SeaScene[enSeaSceneKind_2].GetImageHeight(byTextureIndex)/6,byTextureIndex);
		m_SeaScene[enSeaSceneKind_2].AddBiology(rcClient.Width()/2+470+80,rcClient.Height()/2-100,43,byTextureIndex,8,D3DX_PI/2,wSpaceTime,0.7f);

		byTextureIndex=1;
		wLestTimes=60, wRandTimes=40, wMultiple=4;
		wSpaceTime=wLestTimes+rand()%wMultiple*wRandTimes;
		m_SeaScene[enSeaSceneKind_2].SetImageSize(m_SeaScene[enSeaSceneKind_2].GetImageWidth(byTextureIndex)/8,m_SeaScene[enSeaSceneKind_2].GetImageHeight(byTextureIndex)/6,byTextureIndex);
		m_SeaScene[enSeaSceneKind_2].AddBiology(rcClient.Width()/2-360+30,rcClient.Height()/2+290-120,43,byTextureIndex,8,D3DX_PI/3,wSpaceTime,0.8f);

		byTextureIndex=2;
		wSpaceTime=wLestTimes+rand()%wMultiple*wRandTimes;
		m_SeaScene[enSeaSceneKind_2].SetImageSize(m_SeaScene[enSeaSceneKind_2].GetImageWidth(byTextureIndex)/7,m_SeaScene[enSeaSceneKind_2].GetImageHeight(byTextureIndex)/4,byTextureIndex);
		m_SeaScene[enSeaSceneKind_2].AddBiology(rcClient.Width()/2+210-245,rcClient.Height()/2-110,28,byTextureIndex,7,5*D3DX_PI/3,wSpaceTime,1.3f);

		byTextureIndex=2;
		wSpaceTime=wLestTimes+rand()%wMultiple*wRandTimes;
		m_SeaScene[enSeaSceneKind_2].SetImageSize(m_SeaScene[enSeaSceneKind_2].GetImageWidth(byTextureIndex)/7,m_SeaScene[enSeaSceneKind_2].GetImageHeight(byTextureIndex)/4,byTextureIndex);
		m_SeaScene[enSeaSceneKind_2].AddBiology(rcClient.Width()/2+320-160,rcClient.Height()/2+185-201,28,byTextureIndex,7,0.f,wSpaceTime);

		byTextureIndex=2;
		wSpaceTime=wLestTimes+rand()%wMultiple*wRandTimes;
		m_SeaScene[enSeaSceneKind_2].SetImageSize(m_SeaScene[enSeaSceneKind_2].GetImageWidth(byTextureIndex)/7,m_SeaScene[enSeaSceneKind_2].GetImageHeight(byTextureIndex)/4,byTextureIndex);
		m_SeaScene[enSeaSceneKind_2].AddBiology(rcClient.Width()/2+50,rcClient.Height()/2+230-153,28,byTextureIndex,7,D3DX_PI/2,wSpaceTime,1.2f);

		byTextureIndex=3;
		wSpaceTime=wLestTimes+rand()%wMultiple*wRandTimes;
		m_SeaScene[enSeaSceneKind_2].SetImageSize(m_SeaScene[enSeaSceneKind_2].GetImageWidth(byTextureIndex)/5,m_SeaScene[enSeaSceneKind_2].GetImageHeight(byTextureIndex)/3,byTextureIndex);
		m_SeaScene[enSeaSceneKind_2].AddBiology(rcClient.Width()/2-120-92,rcClient.Height()/2+265-91,15,byTextureIndex,5,0.f,wSpaceTime);

		byTextureIndex=5;
		wSpaceTime=wLestTimes+rand()%wMultiple*wRandTimes;
		m_SeaScene[enSeaSceneKind_2].SetImageSize(m_SeaScene[enSeaSceneKind_2].GetImageWidth(byTextureIndex)/5,m_SeaScene[enSeaSceneKind_2].GetImageHeight(byTextureIndex)/3,byTextureIndex);
		m_SeaScene[enSeaSceneKind_2].AddBiology(rcClient.Width()/2+120-92,rcClient.Height()/2+275-91,15,byTextureIndex,5,0.f,wSpaceTime);

		byTextureIndex=5;
		wSpaceTime=wLestTimes+rand()%wMultiple*wRandTimes;
		m_SeaScene[enSeaSceneKind_2].SetImageSize(m_SeaScene[enSeaSceneKind_2].GetImageWidth(byTextureIndex)/5,m_SeaScene[enSeaSceneKind_2].GetImageHeight(byTextureIndex)/3,byTextureIndex);
		m_SeaScene[enSeaSceneKind_2].AddBiology(rcClient.Width()/2+300-92,rcClient.Height()/2+40-91,15,byTextureIndex,5,0.f,wSpaceTime);

		byTextureIndex=5;
		wSpaceTime=wLestTimes+rand()%wMultiple*wRandTimes;
		m_SeaScene[enSeaSceneKind_2].SetImageSize(m_SeaScene[enSeaSceneKind_2].GetImageWidth(byTextureIndex)/5,m_SeaScene[enSeaSceneKind_2].GetImageHeight(byTextureIndex)/3,byTextureIndex);
		m_SeaScene[enSeaSceneKind_2].AddBiology(rcClient.Width()/2+455-int(92*0.8f),rcClient.Height()/2-80-int(91*0.8f),15,byTextureIndex,5,0.f,wSpaceTime,0.8f);

		byTextureIndex=5;
		wSpaceTime=wLestTimes+rand()%wMultiple*wRandTimes;
		m_SeaScene[enSeaSceneKind_2].SetImageSize(m_SeaScene[enSeaSceneKind_2].GetImageWidth(byTextureIndex)/5,m_SeaScene[enSeaSceneKind_2].GetImageHeight(byTextureIndex)/3,byTextureIndex);
		m_SeaScene[enSeaSceneKind_2].AddBiology(rcClient.Width()/2+495-int(92*0.7f),rcClient.Height()/2-140-int(91*0.7f),15,byTextureIndex,5,0.f,wSpaceTime,0.7f);
	}
	else if (enSeaSceneKind_3==SeaSceneKind)
	{
		//重置场景
		m_SeaScene[enSeaSceneKind_3].ResetScene();

		//加载海底
		_sntprintf(szFileName,CountArray(szFileName),TEXT("%sFish\\SeaScene\\SCENE_3.dds"),g_szWorkDir);
		m_SeaScene[enSeaSceneKind_3].SetLoadInfo(g_pD3DDevice,szFileName,0,Pool);
		m_SeaScene[enSeaSceneKind_3].SetReverse(true);

		_sntprintf(szFileName,CountArray(szFileName),TEXT("%sFish\\SeaScene\\BUBBLE.dds"),g_szWorkDir);
		m_SeaScene[enSeaSceneKind_3].SetLoadInfo(g_pD3DDevice,szFileName,MAX_TEXTURE_COUNT-1,Pool);

		//海底生物
		_sntprintf(szFileName,CountArray(szFileName),TEXT("%sFish\\SeaScene\\BIOLOGY_8.dds"),g_szWorkDir);
		m_SeaScene[enSeaSceneKind_3].SetLoadInfo(g_pD3DDevice,szFileName,1,Pool);
		_sntprintf(szFileName,CountArray(szFileName),TEXT("%sFish\\SeaScene\\BIOLOGY_10.dds"),g_szWorkDir);
		m_SeaScene[enSeaSceneKind_3].SetLoadInfo(g_pD3DDevice,szFileName,2,Pool);
		_sntprintf(szFileName,CountArray(szFileName),TEXT("%sFish\\SeaScene\\BIOLOGY_12.dds"),g_szWorkDir);
		m_SeaScene[enSeaSceneKind_3].SetLoadInfo(g_pD3DDevice,szFileName,3,Pool);

		//设置位置
		byTextureIndex=0;
		m_SeaScene[enSeaSceneKind_3].SetDrawPos(rcClient.Width()/2-m_SeaScene[enSeaSceneKind_3].GetImageWidth(byTextureIndex)/2+m_SeaScene[enSeaSceneKind_3].GetImageWidth(byTextureIndex),
			(rcClient.Height()-m_ToolBarBk.GetImageHeight()+24)/2-m_SeaScene[enSeaSceneKind_3].GetImageHeight(byTextureIndex)/2+m_SeaScene[enSeaSceneKind_3].GetImageHeight(byTextureIndex),byTextureIndex);

		byTextureIndex=5;
		wLestTimes=60, wRandTimes=40, wMultiple=4;
		wSpaceTime=wLestTimes+rand()%wMultiple*wRandTimes;
		m_SeaScene[enSeaSceneKind_3].SetImageSize(m_SeaScene[enSeaSceneKind_3].GetImageWidth(byTextureIndex)/8,m_SeaScene[enSeaSceneKind_3].GetImageHeight(byTextureIndex)/6,byTextureIndex);
		m_SeaScene[enSeaSceneKind_3].AddBiology(rcClient.Width()/2-80-109,rcClient.Height()/2+15-110,43,byTextureIndex,8,0.f,wSpaceTime);

		byTextureIndex=3;
		wLestTimes=60, wRandTimes=40, wMultiple=4;
		wSpaceTime=wLestTimes+rand()%wMultiple*wRandTimes;
		m_SeaScene[enSeaSceneKind_3].SetImageSize(m_SeaScene[enSeaSceneKind_3].GetImageWidth(byTextureIndex)/8,m_SeaScene[enSeaSceneKind_3].GetImageHeight(byTextureIndex)/6,byTextureIndex);
		m_SeaScene[enSeaSceneKind_3].AddBiology(rcClient.Width()/2+280+76,rcClient.Height()/2-100+77,43,byTextureIndex,8,D3DX_PI,wSpaceTime,0.7f);

		byTextureIndex=1;
		wLestTimes=60, wRandTimes=40, wMultiple=4;
		wSpaceTime=wLestTimes+rand()%wMultiple*wRandTimes;
		m_SeaScene[enSeaSceneKind_3].SetImageSize(m_SeaScene[enSeaSceneKind_3].GetImageWidth(byTextureIndex)/8,m_SeaScene[enSeaSceneKind_3].GetImageHeight(byTextureIndex)/4,byTextureIndex);
		m_SeaScene[enSeaSceneKind_3].AddBiology(rcClient.Width()/2-190+134,rcClient.Height()/2-200+126,32,byTextureIndex,8,D3DX_PI,wSpaceTime);

		byTextureIndex=1;
		wLestTimes=60, wRandTimes=40, wMultiple=4;
		wSpaceTime=wLestTimes+rand()%wMultiple*wRandTimes;
		m_SeaScene[enSeaSceneKind_3].SetImageSize(m_SeaScene[enSeaSceneKind_3].GetImageWidth(byTextureIndex)/8,m_SeaScene[enSeaSceneKind_3].GetImageHeight(byTextureIndex)/4,byTextureIndex);
		m_SeaScene[enSeaSceneKind_3].AddBiology(rcClient.Width()/2+250+120,rcClient.Height()/2-190+113,32,byTextureIndex,8,D3DX_PI,wSpaceTime,0.9f);

		byTextureIndex=1;
		wLestTimes=60, wRandTimes=40, wMultiple=4;
		wSpaceTime=wLestTimes+rand()%wMultiple*wRandTimes;
		m_SeaScene[enSeaSceneKind_3].SetImageSize(m_SeaScene[enSeaSceneKind_3].GetImageWidth(byTextureIndex)/8,m_SeaScene[enSeaSceneKind_3].GetImageHeight(byTextureIndex)/4,byTextureIndex);
		m_SeaScene[enSeaSceneKind_3].AddBiology(rcClient.Width()/2-110-175,rcClient.Height()/2+155-180,32,byTextureIndex,8,0.f,wSpaceTime,1.2f);

		byTextureIndex=2;
		wLestTimes=60, wRandTimes=40, wMultiple=4;
		wSpaceTime=wLestTimes+rand()%wMultiple*wRandTimes;
		m_SeaScene[enSeaSceneKind_3].SetImageSize(m_SeaScene[enSeaSceneKind_3].GetImageWidth(byTextureIndex)/7,m_SeaScene[enSeaSceneKind_3].GetImageHeight(byTextureIndex)/3,byTextureIndex);
		m_SeaScene[enSeaSceneKind_3].AddBiology(rcClient.Width()/2+120-225,rcClient.Height()/2+50-209,21,byTextureIndex,7,0.f,140);

		byTextureIndex=2;
		wLestTimes=60, wRandTimes=40, wMultiple=4;
		wSpaceTime=wLestTimes+rand()%wMultiple*wRandTimes;
		m_SeaScene[enSeaSceneKind_3].SetImageSize(m_SeaScene[enSeaSceneKind_3].GetImageWidth(byTextureIndex)/7,m_SeaScene[enSeaSceneKind_3].GetImageHeight(byTextureIndex)/3,byTextureIndex);
		m_SeaScene[enSeaSceneKind_3].AddBiology(rcClient.Width()/2-400-180,rcClient.Height()/2-190-167,21,byTextureIndex,7,0.f,160,0.8f);

		byTextureIndex=2;
		wLestTimes=60, wRandTimes=40, wMultiple=4;
		wSpaceTime=wLestTimes+rand()%wMultiple*wRandTimes;
		m_SeaScene[enSeaSceneKind_3].SetImageSize(m_SeaScene[enSeaSceneKind_3].GetImageWidth(byTextureIndex)/7,m_SeaScene[enSeaSceneKind_3].GetImageHeight(byTextureIndex)/3,byTextureIndex);
		m_SeaScene[enSeaSceneKind_3].AddBiology(rcClient.Width()/2+310-157,rcClient.Height()/2+220-146,21,byTextureIndex,7,0.f,180,0.7f);

		byTextureIndex=2;
		wLestTimes=60, wRandTimes=40, wMultiple=4;
		wSpaceTime=wLestTimes+rand()%wMultiple*wRandTimes;
		m_SeaScene[enSeaSceneKind_3].SetImageSize(m_SeaScene[enSeaSceneKind_3].GetImageWidth(byTextureIndex)/7,m_SeaScene[enSeaSceneKind_3].GetImageHeight(byTextureIndex)/3,byTextureIndex);
		m_SeaScene[enSeaSceneKind_3].AddBiology(rcClient.Width()/2+530,rcClient.Height()/2-175-160,21,byTextureIndex,7,D3DX_PI/2,180,0.6f);

		byTextureIndex=2;
		wLestTimes=60, wRandTimes=40, wMultiple=4;
		wSpaceTime=wLestTimes+rand()%wMultiple*wRandTimes;
		m_SeaScene[enSeaSceneKind_3].SetImageSize(m_SeaScene[enSeaSceneKind_3].GetImageWidth(byTextureIndex)/7,m_SeaScene[enSeaSceneKind_3].GetImageHeight(byTextureIndex)/3,byTextureIndex);
		m_SeaScene[enSeaSceneKind_3].AddBiology(rcClient.Width()/2-295+153,rcClient.Height()/2+80,21,byTextureIndex,7,D3DX_PI/3,200,0.7f);	
	}
}

//释放场景
void CGameClientViewDX::ReleaseSeaScene(enSeaSceneKind SeaSceneKind)
{
	if (enSeaSceneKind_1==SeaSceneKind)
	{
		//释放海底
		if (m_SeaScene[enSeaSceneKind_1].IsLoaded()==true) m_SeaScene[enSeaSceneKind_1].Release();
	}
	else if (enSeaSceneKind_2==SeaSceneKind)
	{
		//释放海底
		if (m_SeaScene[enSeaSceneKind_2].IsLoaded(0)==true) m_SeaScene[enSeaSceneKind_2].Release();
	}
	else if (enSeaSceneKind_3==SeaSceneKind)
	{
		//释放海底
		if (m_SeaScene[enSeaSceneKind_3].IsLoaded(0)==true) m_SeaScene[enSeaSceneKind_3].Release();
	}
}

//加载鱼群
void CGameClientViewDX::LoadFish()
{
	//变量定义
	TCHAR szFileName[MAX_PATH];

	//鱼群图片
	for (enFishKind FishKind=enFishKind_11; FishKind<enFishKind_No; FishKind=enFishKind(FishKind+1))
	{
		//加载图片
		_sntprintf(szFileName,CountArray(szFileName),TEXT("%sFish\\Fish\\FISH_%d.dds"),g_szWorkDir,FishKind+1);
		g_FishGroup[FishKind].SetLoadInfo(g_pD3DDevice,szFileName,TEXTURE_INDEX_FISH,D3DPOOL_DEFAULT);
		g_FishGroup[FishKind].SetImageSize(g_FishGroup[FishKind].GetImageWidth(TEXTURE_INDEX_FISH)/g_nColumnCount[FishKind],
			g_FishGroup[FishKind].GetImageHeight(TEXTURE_INDEX_FISH)/g_nRowCount[FishKind],TEXTURE_INDEX_FISH);
	}
}

//加载图片
VOID CGameClientViewDX::LoadDXImage()
{
	//变量定义
	TCHAR szFileName[MAX_PATH];
	D3DPOOL Pool=D3DPOOL_MANAGED;

	//加载图片
	if (m_ToolBarBk.IsLoaded(0)==false)
	{
		_sntprintf(szFileName,CountArray(szFileName),TEXT("%sFish\\Background\\TOOL_BAR.dds"),g_szWorkDir);
		m_ToolBarBk.SetLoadInfo(g_pD3DDevice,szFileName,0,Pool);
	}

	//提示背景
	_sntprintf(szFileName,CountArray(szFileName),TEXT("%sFish\\SeaScene\\GAME_TIP.png"),g_szWorkDir);
	m_GameTip.SetLoadInfo(g_pD3DDevice,szFileName,0,Pool);	

	//警告图片
	_sntprintf(szFileName,CountArray(szFileName),TEXT("%sFish\\Tool\\Warn.png"),g_szWorkDir);
	m_NoBulletWarn.SetLoadInfo(g_pD3DDevice,szFileName,0,Pool);

	//鱼群图片
	for (enFishKind FishKind=enFishKind_1; FishKind<enFishKind_11; FishKind=enFishKind(FishKind+1))
	{
		//加载图片
		_sntprintf(szFileName,CountArray(szFileName),TEXT("%sFish\\Fish\\FISH_%d.dds"),g_szWorkDir,FishKind+1);
		g_FishGroup[FishKind].SetLoadInfo(g_pD3DDevice,szFileName,TEXTURE_INDEX_FISH,Pool);
		g_FishGroup[FishKind].SetImageSize(g_FishGroup[FishKind].GetImageWidth(TEXTURE_INDEX_FISH)/g_nColumnCount[FishKind],
			g_FishGroup[FishKind].GetImageHeight(TEXTURE_INDEX_FISH)/g_nRowCount[FishKind],TEXTURE_INDEX_FISH);
	}

	//数字图片
	_sntprintf(szFileName,CountArray(szFileName),TEXT("%sFish\\Background\\BULLET_COUNT.dds"),g_szWorkDir);
	m_Number.SetLoadInfo(g_pD3DDevice,szFileName,TEXTURE_INDEX_BULLET_COUNT,Pool);
	m_Number.SetImageSize(m_Number.GetImageWidth(TEXTURE_INDEX_BULLET_COUNT)/11,m_Number.GetImageHeight(TEXTURE_INDEX_BULLET_COUNT),TEXTURE_INDEX_BULLET_COUNT);

	_sntprintf(szFileName,CountArray(szFileName),TEXT("%sFish\\Background\\COIN_COUNT.dds"),g_szWorkDir);
	m_Number.SetLoadInfo(g_pD3DDevice,szFileName,TEXTURE_INDEX_COIN_COUNT,Pool);
	m_Number.SetImageSize(m_Number.GetImageWidth(TEXTURE_INDEX_COIN_COUNT)/10,m_Number.GetImageHeight(TEXTURE_INDEX_COIN_COUNT),TEXTURE_INDEX_COIN_COUNT);

	_sntprintf(szFileName,CountArray(szFileName),TEXT("%sFish\\Background\\NONE.dds"),g_szWorkDir);
	m_Number.SetLoadInfo(g_pD3DDevice,szFileName,TEXTURE_INDEX_NONE,Pool);
	m_Number.SetImageSize(m_Number.GetImageWidth(TEXTURE_INDEX_NONE),m_Number.GetImageHeight(TEXTURE_INDEX_NONE),TEXTURE_INDEX_NONE);

	//加载图片
	_sntprintf(szFileName,CountArray(szFileName),TEXT("%sFish\\Fish\\FISH_12_2.dds"),g_szWorkDir);
	g_FishGroup[enFishKind_12].SetLoadInfo(g_pD3DDevice,szFileName,TEXTURE_INDEX_FISH+1,Pool);
	g_FishGroup[enFishKind_12].SetImageSize(g_FishGroup[enFishKind_12].GetImageWidth(TEXTURE_INDEX_FISH)/g_nColumnCount[enFishKind_12],
		g_FishGroup[enFishKind_12].GetImageHeight(TEXTURE_INDEX_FISH)/g_nRowCount[enFishKind_12],TEXTURE_INDEX_FISH+1);

	//加载大炮
	_sntprintf(szFileName,CountArray(szFileName),TEXT("%sFish\\Tool\\Cannon.dds"),g_szWorkDir);
	m_Cannon.SetLoadInfo(g_pD3DDevice,szFileName,TEXTURE_INDEX_CANNON,Pool);
	m_Cannon.SetImageSize(m_Cannon.GetImageWidth(TEXTURE_INDEX_CANNON)/5,m_Cannon.GetImageHeight(TEXTURE_INDEX_CANNON)/5,TEXTURE_INDEX_CANNON);

	_sntprintf(szFileName,CountArray(szFileName),TEXT("%sFish\\Tool\\Cannon_Back.dds"),g_szWorkDir);
	m_Cannon.SetLoadInfo(g_pD3DDevice,szFileName,TEXTURE_INDEX_BACK,Pool);
	m_Cannon.SetImageSize(m_Cannon.GetImageWidth(TEXTURE_INDEX_BACK)/2,m_Cannon.GetImageHeight(TEXTURE_INDEX_BACK));

	_sntprintf(szFileName,CountArray(szFileName),TEXT("%sFish\\Tool\\BulletCount.dds"),g_szWorkDir);
	m_Cannon.SetLoadInfo(g_pD3DDevice,szFileName,TEXTURE_INDEX_NUM,Pool);
	m_Cannon.SetImageSize(BULLET_NUMBER_WIDTH,m_Cannon.GetImageHeight(TEXTURE_INDEX_NUM)/2,TEXTURE_INDEX_NUM);

	//加载子弹
	_sntprintf(szFileName,CountArray(szFileName),TEXT("%sFish\\Tool\\Bullet_1.png"),g_szWorkDir);
	m_Bullet.SetLoadInfo(g_pD3DDevice,szFileName,TEXTURE_INDEX_SHOOT_1,Pool);
	m_Bullet.SetImageSize(m_Bullet.GetImageWidth(TEXTURE_INDEX_SHOOT_1)/BULLET_FRAME_COUNT,m_Bullet.GetImageHeight(TEXTURE_INDEX_SHOOT_1),TEXTURE_INDEX_SHOOT_1);

	_sntprintf(szFileName,CountArray(szFileName),TEXT("%sFish\\Tool\\Bullet_2.png"),g_szWorkDir);
	m_Bullet.SetLoadInfo(g_pD3DDevice,szFileName,TEXTURE_INDEX_SHOOT_2,Pool);
	m_Bullet.SetImageSize(m_Bullet.GetImageWidth(TEXTURE_INDEX_SHOOT_2)/BULLET_FRAME_COUNT,m_Bullet.GetImageHeight(TEXTURE_INDEX_SHOOT_2),TEXTURE_INDEX_SHOOT_2);

	_sntprintf(szFileName,CountArray(szFileName),TEXT("%sFish\\Tool\\Bullet_3.png"),g_szWorkDir);
	m_Bullet.SetLoadInfo(g_pD3DDevice,szFileName,TEXTURE_INDEX_SHOOT_3,Pool);
	m_Bullet.SetImageSize(m_Bullet.GetImageWidth(TEXTURE_INDEX_SHOOT_3)/BULLET_FRAME_COUNT,m_Bullet.GetImageHeight(TEXTURE_INDEX_SHOOT_3),TEXTURE_INDEX_SHOOT_3);

	_sntprintf(szFileName,CountArray(szFileName),TEXT("%sFish\\Tool\\Net.dds"),g_szWorkDir);
	m_Bullet.SetLoadInfo(g_pD3DDevice,szFileName,TEXTURE_INDEX_NET,Pool);
	m_Bullet.SetImageSize(m_Bullet.GetImageWidth(TEXTURE_INDEX_NET)/g_nNetColumnCount[enBulletKind_1],m_Bullet.GetImageHeight(TEXTURE_INDEX_NET)/g_nNetRowCount[enBulletKind_1],TEXTURE_INDEX_NET);

	//彩金图片
	_sntprintf(szFileName,CountArray(szFileName),TEXT("%sFish\\Score\\NUMBER.dds"),g_szWorkDir);
	m_Bonus.SetLoadInfo(g_pD3DDevice,szFileName,TEXTURE_INDEX_BONUS_NUMBER,Pool);
	m_Bonus.SetImageSize(m_Bonus.GetImageWidth(TEXTURE_INDEX_BONUS_NUMBER)/10,m_Bonus.GetImageHeight(TEXTURE_INDEX_BONUS_NUMBER),TEXTURE_INDEX_BONUS_NUMBER);

	_sntprintf(szFileName,CountArray(szFileName),TEXT("%sFish\\Score\\BACK_LIGHT.dds"),g_szWorkDir);
	m_Bonus.SetLoadInfo(g_pD3DDevice,szFileName,TEXTURE_INDEX_BONUS_LIGHT,Pool);
	m_Bonus.SetImageSize(m_Bonus.GetImageWidth(TEXTURE_INDEX_BONUS_LIGHT),m_Bonus.GetImageHeight(TEXTURE_INDEX_BONUS_LIGHT),TEXTURE_INDEX_BONUS_LIGHT);

	_sntprintf(szFileName,CountArray(szFileName),TEXT("%sFish\\Score\\BONUS_BACK.dds"),g_szWorkDir);
	m_Bonus.SetLoadInfo(g_pD3DDevice,szFileName,TEXTURE_INDEX_BONUS_BACK,Pool);
	m_Bonus.SetImageSize(m_Bonus.GetImageWidth(TEXTURE_INDEX_BONUS_BACK),m_Bonus.GetImageHeight(TEXTURE_INDEX_BONUS_BACK),TEXTURE_INDEX_BONUS_BACK);

	_sntprintf(szFileName,CountArray(szFileName),TEXT("%sFish\\Score\\BONUS_FLASH.dds"),g_szWorkDir);
	m_Bonus.SetLoadInfo(g_pD3DDevice,szFileName,TEXTURE_INDEX_BONUS_STAR,Pool);
	m_Bonus.SetImageSize(m_Bonus.GetImageWidth(TEXTURE_INDEX_BONUS_STAR)/2,m_Bonus.GetImageHeight(TEXTURE_INDEX_BONUS_STAR)/2,TEXTURE_INDEX_BONUS_STAR);

	_sntprintf(szFileName,CountArray(szFileName),TEXT("%sFish\\Score\\BONUS_DETAIL.dds"),g_szWorkDir);
	m_Bonus.SetLoadInfo(g_pD3DDevice,szFileName,TEXTURE_INDEX_BONUS_DETAIL,Pool);
	m_Bonus.SetImageSize(m_Bonus.GetImageWidth(TEXTURE_INDEX_BONUS_DETAIL),m_Bonus.GetImageHeight(TEXTURE_INDEX_BONUS_DETAIL),TEXTURE_INDEX_BONUS_DETAIL);

	//加载金币
	_sntprintf(szFileName,CountArray(szFileName),TEXT("%sFish\\Fish\\VALUE_COUNT.dds"),g_szWorkDir);
	m_Coin.SetLoadInfo(g_pD3DDevice,szFileName,TEXTURE_INDEX_COIN_SHOW,Pool);
	m_Coin.SetImageSize(m_Coin.GetImageWidth(TEXTURE_INDEX_COIN_SHOW)/11,m_Coin.GetImageHeight(TEXTURE_INDEX_COIN_SHOW),TEXTURE_INDEX_COIN_SHOW);

	_sntprintf(szFileName,CountArray(szFileName),TEXT("%sFish\\Fish\\COIN_1.dds"),g_szWorkDir);
	m_Coin.SetLoadInfo(g_pD3DDevice,szFileName,TEXTURE_INDEX_COIN_MOVE_1,Pool);
	m_Coin.SetImageSize(m_Coin.GetImageWidth(TEXTURE_INDEX_COIN_MOVE_1)/11,m_Coin.GetImageHeight(TEXTURE_INDEX_COIN_MOVE_1),TEXTURE_INDEX_COIN_MOVE_1);

	_sntprintf(szFileName,CountArray(szFileName),TEXT("%sFish\\Fish\\COIN_2.dds"),g_szWorkDir);
	m_Coin.SetLoadInfo(g_pD3DDevice,szFileName,TEXTURE_INDEX_COIN_MOVE_2,Pool);
	m_Coin.SetImageSize(m_Coin.GetImageWidth(TEXTURE_INDEX_COIN_MOVE_2)/11,m_Coin.GetImageHeight(TEXTURE_INDEX_COIN_MOVE_2),TEXTURE_INDEX_COIN_MOVE_2);

	_sntprintf(szFileName,CountArray(szFileName),TEXT("%sFish\\Fish\\COIN_3.dds"),g_szWorkDir);
	m_Coin.SetLoadInfo(g_pD3DDevice,szFileName,TEXTURE_INDEX_COIN_MOVE_3,Pool);
	m_Coin.SetImageSize(m_Coin.GetImageWidth(TEXTURE_INDEX_COIN_MOVE_3)/11,m_Coin.GetImageHeight(TEXTURE_INDEX_COIN_MOVE_3),TEXTURE_INDEX_COIN_MOVE_3);

	//玩家信息
	_sntprintf(szFileName,CountArray(szFileName),TEXT("%sFish\\Background\\USER_INFO.dds"),g_szWorkDir);
	m_UserInfo.SetLoadInfo(g_pD3DDevice,szFileName,0,Pool);
	m_UserInfo.SetImageSize(m_UserInfo.GetImageWidth()/2,m_UserInfo.GetImageHeight());

	//设置标识
	m_bLoadResFinish=true;

	return ;
}

//重置图片
VOID CGameClientViewDX::OnResetDevice()
{
	//重置图片
	if (m_ToolBarBk.IsLoaded()==true) m_ToolBarBk.OnResetDevice();
	if (m_Cannon.IsLoaded()==true) m_Cannon.OnResetDevice();
	if (m_Bullet.IsLoaded()==true) m_Bullet.OnResetDevice();
	if (m_Coin.IsLoaded()==true) m_Coin.OnResetDevice();
	if (m_UserInfo.IsLoaded()==true) m_UserInfo.OnResetDevice();
	if (m_Number.IsLoaded()==true) m_Number.OnResetDevice();
	if (m_Wave.IsLoaded()==true) m_Wave.OnResetDevice();
	if (m_NoBulletWarn.IsLoaded()==true) m_NoBulletWarn.OnResetDevice();
	if (m_WaveEffect.IsLoaded()==true) m_WaveEffect.OnResetDevice();
	if (m_GameTip.IsLoaded()==true) m_GameTip.OnResetDevice();
	if (m_Bonus.IsLoaded()==true) m_Bonus.OnResetDevice();

	for (enSeaSceneKind SeaSceneKind=enSeaSceneKind_1; SeaSceneKind<enSeaSceneKind_No; SeaSceneKind=enSeaSceneKind(SeaSceneKind+1))
	{	
		if (m_SeaScene[SeaSceneKind].IsLoaded()==true) m_SeaScene[SeaSceneKind].OnResetDevice();
	}

	//重置资源
	m_pFontUserInfo->OnResetDevice();
	m_pFontGameInfo->OnResetDevice();

	//重置鱼群
	for (enFishKind FishKind=enFishKind_1; FishKind<enFishKind_No; FishKind=enFishKind(FishKind+1))
	{
		if (g_FishGroup[FishKind].IsLoaded()==true)
		{
			g_FishGroup[FishKind].OnResetDevice();
			g_FishGroup[FishKind].ResetKenel();
		}
	}

	return ;
}

//释放资源
VOID CGameClientViewDX::OnDeviceLost()
{
	//加载图片
	m_ToolBarBk.OnLostDevice();
	m_Cannon.OnLostDevice();
	m_Bullet.OnLostDevice();
	m_Coin.OnLostDevice();
	m_Bonus.OnLostDevice();
	m_UserInfo.OnLostDevice();
	m_Number.OnLostDevice();
	if (m_Wave.IsLoaded()==true) m_Wave.OnLostDevice();
	m_NoBulletWarn.OnLostDevice();

	//释放资源
	m_pFontUserInfo->OnLostDevice();
	m_pFontGameInfo->OnLostDevice();

	////释放鱼群
	//for (enFishKind FishKind=enFishKind_1; FishKind<enFishKind_No; FishKind=enFishKind(FishKind+1))
	//{
	//	g_FishGroup[FishKind].OnLostDevice();
	//	g_FishGroup[FishKind].ResetKenel();
	//}

	//释放海景
	m_WaveEffect.OnLostDevice();
	for (enSeaSceneKind SeaSceneKind=enSeaSceneKind_1; SeaSceneKind<enSeaSceneKind_No; SeaSceneKind=enSeaSceneKind(SeaSceneKind+1))
	{
		if (m_SeaScene[SeaSceneKind].IsLoaded()==true) m_SeaScene[SeaSceneKind].OnLostDevice();
	}

	//释放鱼群
	for (enFishKind FishKind=enFishKind_11; FishKind<enFishKind_No; FishKind=enFishKind(FishKind+1))
	{
		g_FishGroup[FishKind].Release();
		g_FishGroup[FishKind].ResetKenel();
	}
	for (enFishKind FishKind=enFishKind_1; FishKind<enFishKind_11; FishKind=enFishKind(FishKind+1))
	{
		g_FishGroup[FishKind].OnLostDevice();
		g_FishGroup[FishKind].ResetKenel();
	}

	return ;
}

//鱼群消失
LRESULT CGameClientViewDX::OnFishDisappear(WPARAM wParam, LPARAM lParam)
{
	////获取参数
	//BYTE byShootUser=BYTE(LOWORD(wParam));
	//enFishKind FishKind=enFishKind(HIWORD(wParam));
	//int nPosX=LOWORD(lParam);
	//int nPosY=HIWORD(lParam);

	////合法判断
	//ASSERT(byShootUser<GAME_PLAYER);
	//ASSERT(FishKind<enFishKind_No);
	//if (!(byShootUser<GAME_PLAYER)) return 0;
	//if (!(FishKind<enFishKind_No)) return 0;

	////增加金币
	//if (FishKind<enFishKind_No)
	//{
	//	LONG lScore=g_nFishScore[FishKind];
	//	UserAddCoin(byShootUser, nPosX,nPosY, lScore);
	//}

	return 0;
}

//轨迹消息
LRESULT CGameClientViewDX::OnFinishCalcTrace(WPARAM wParam, LPARAM lParam)
{
	//通知鱼群
	while (m_FishArrayActive.GetCount()>0)
	{
		//设置标识
		ASSERT(NULL!=m_FishArrayActive[0]);
		if (NULL!=m_FishArrayActive[0]) m_FishArrayActive[0]->bActive=true;

		//鱼群数目
		WORD wFishCount=0;
		for (enFishKind FishKindIndex=enFishKind_1; FishKindIndex<enFishKind_No; FishKindIndex=enFishKind(FishKindIndex+1))
			wFishCount+=g_FishGroup[FishKindIndex].GetFishCount();

		//数目判断
		if (wFishCount>=70)
		{
			for (enFishKind FishKindIndex=enFishKind_1; FishKindIndex<enFishKind_No; FishKindIndex=enFishKind(FishKindIndex+1))
				g_FishGroup[FishKindIndex].FreeFish(m_FishArrayActive[0]);
		}

		//移除信息
		m_FishArrayActive.RemoveAt(0);
	}

	//设置时间
	m_dwLastCalcTime=GetTickCount();

	return 0;
}

//更新窗口
bool CGameClientViewDX::UpdateDxWindowSize()
{
	//设置标识
	m_bResetArray=true;

	return __super::UpdateDxWindowSize();
}

//设置位置
void CGameClientViewDX::InitPosition()
{
	//常量定义
	int const static nCanPosX[2]={260,760};
	int const static nCanPosY[2]={37,527};
	int const static nNumPosX[2]={387,740};
	int const static nNumPosY[2]={47,560};

	//设置位置
	for (WORD wChairID=0; wChairID<GAME_PLAYER; ++wChairID)
	{
		//自己判断
		int nIndex=wChairID%2;
		if (true==IsSameSide(m_wMeChairID,wChairID))
		{
			//大炮坐标
			m_Cannon.SetBenchmarkPos(wChairID,nCanPosX[nIndex],nCanPosY[1]);

			//数字位置
			m_Cannon.SetNumberPos(wChairID,nNumPosX[nIndex],nNumPosY[1]);

			//设置角度
			m_Cannon.SetAngleBk(wChairID,0.f);
		}
		else
		{
			//大炮坐标
			m_Cannon.SetBenchmarkPos(wChairID,nCanPosX[1-nIndex],nCanPosY[0]);

			//数字位置
			m_Cannon.SetNumberPos(wChairID,nNumPosX[1-nIndex],nNumPosY[0]);

			//设置角度
			m_Cannon.SetAngleBk(wChairID,D3DX_PI);
		}
	}
}

//增加轨迹
void CGameClientViewDX::AddTrace(int nInitX[], int nInitY[], int nInitCount, enFishKind FishKind, DWORD dwFishID, bool bRegular)
{
	//设置数组
	tagFishTraceInfo FishTraceInfo={0};
	FishTraceInfo.pFishInfo=g_FishGroup[FishKind].ActiveNewFish();

	//合法判断
	ASSERT(NULL!=FishTraceInfo.pFishInfo);
	if (NULL==FishTraceInfo.pFishInfo)
	{
		ASSERT(FALSE);
		TraceString(TEXT("if (NULL==FishTraceInfo.pFishInfo)"));
		return;
	}	

	//鱼群信息
	FishTraceInfo.nInitCount=nInitCount;
	CopyMemory(FishTraceInfo.nInitX,nInitX,sizeof(FishTraceInfo.nInitX));
	CopyMemory(FishTraceInfo.nInitY,nInitY,sizeof(FishTraceInfo.nInitY));
	FishTraceInfo.pFishInfo->dwFishID=dwFishID;
	FishTraceInfo.pFishInfo->bySpaceTimes=bRegular?g_nMoveSpeed[enFishKind_1]:g_nMoveSpeed[FishKind];
	FishTraceInfo.byMoveSpace=(false==bRegular && FishKind>=enFishKind_11)?TRACE_POINT_SPACE_BIG:TRACE_POINT_SPACE_NORMAL;

	//坐标转换
	if (0==m_wMeChairID || 1==m_wMeChairID)
	{
		for (int i=0; i<nInitCount; ++i)
		{
			FishTraceInfo.nInitX[i]=SwitchCoorX(FishTraceInfo.nInitX[i]);
			FishTraceInfo.nInitY[i]=SwitchCoorY(FishTraceInfo.nInitY[i]);
		}
	}

	//加入数组
	m_FishArrayBuffer.Add(FishTraceInfo);

	//设置信号
	bool bRestart=false;
	if (NULL!=m_hEventFinish) 
	{
		SetEvent(m_hEventFinish);

		////时间判断
		//if ((m_dwLastCalcTime+15000)<GetTickCount())
		//{
		//	bRestart=true;
		//	CloseHandle(m_hEventFinish);
		//}
	}
	else
	{
		bRestart=true;
	}

	if (true==bRestart)
	{
		//结束线程
		WaitForSingleObject(m_hCalcThread,2000);
		CloseHandle(m_hCalcThread);

		//重启线程
		UINT uThreadID=0;
		m_hEventFinish=CreateEvent(NULL,FALSE,FALSE,NULL);
		m_hCalcThread=(HANDLE)::CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)ThreadCalcTrace,LPVOID(this),0,0);		

#ifdef _DEBUG
		if (NULL!=m_pGameClientDlg) m_pGameClientDlg->InsertSystemString(TEXT("重启线程"));
#endif
	}
}

//设置座位
void CGameClientViewDX::SetMeChairID(WORD wMeChairID) 
{
	//自己座位
	m_wMeChairID=wMeChairID;
	m_Cannon.SetMeChairID(m_wMeChairID);

	//子弹数目
	m_Cannon.SetBulletCount(m_wMeChairID,5,false);

	//设置位置
	InitPosition();
}






	////输出信息
	//for (WORD wChairID=0; wChairID<GAME_PLAYER; ++wChairID)
	//{
	//	if(m_lBulletCount[wChairID] > 0 || wChairID == m_wMeChairID)
	//	{
	//		//金币背景
	//		GetDrawPos(wChairID,enDrawPos_MeGoldBack,nDrawPosX,nDrawPosY);
	//		int _w = m_CoinBack.GetWidth();
	//		int _h = m_CoinBack.GetHeight();
	//		rcDraw.SetRect(0,0,_w,_h);
	//		m_CoinBack.DrawSprite(nDrawPosX,nDrawPosY,rcDraw);

	//		//金币数目(炮台右边)
	//		GetDrawPos(wChairID,enDrawPos_MeGold,nDrawPosX,nDrawPosY);
	//		LONGLONG lScore= m_lCoinCount[wChairID] + m_lBulletCount[wChairID]*m_lCellScore[wChairID];
	//		//LONGLONG lScore=123456789;//测试用
	//		DrawNumber(nDrawPosX, nDrawPosY, lScore, TEXTURE_INDEX_COIN_COUNT);

	//		//写昵称(炮台右边)
	//		rcClient.SetRect(nDrawPosX+20,nDrawPosY-20,nDrawPosX + 120, nDrawPosY + 35);
	//		if(m_pGameClientDlg->GetUserData(wChairID) != NULL)
	//		{
	//			sprintf(m_szNiceName[wChairID], m_pGameClientDlg->GetUserData(wChairID)->szNickName);
	//			//m_pFontUserInfo->DrawTextA(NULL, "昵称ABC", (int)strlen("昵称ABC"), &rcClient,DT_SINGLELINE|DT_NOCLIP|DT_CENTER|DT_TOP, RGB(255,0,255)|0xFF000000);
	//			m_pFontUserInfo->DrawTextA(NULL, m_szNiceName[wChairID], (int)strlen(m_szNiceName[wChairID]), &rcClient,DT_SINGLELINE|DT_NOCLIP|DT_CENTER|DT_TOP, RGB(255,0,255)|0xFF000000);
	//		}

	//	}
	//}

//个人信息
void CGameClientViewDX::DrawUserInfo()
{
	//合法判断
	ASSERT(NULL!=m_pGameClientDlg);
	if (NULL==m_pGameClientDlg) return;
	if (m_wMeChairID>=GAME_PLAYER) return;

	//获取大小
	CRect rcClient, rcDraw;
	GetClientRect(&rcClient);

	//绘画变量
	int nDrawPosX,nDrawPosY;
	WCHAR szBuffer[256];

	//输出信息
	for (WORD wChairID=0; wChairID<GAME_PLAYER; ++wChairID)
	{
		//获取用户
		tagUserData const *pUserData=m_pGameClientDlg->GetUserData(wChairID);
		if (NULL==pUserData) continue;

		/////////////////////////////////////////////////////////////////////////////////
		/*tagUserData UserData={0};
		tagUserData *pUserData=&UserData;
		lstrcpyn(UserData.szName,TEXT("KJDKFJDFKJDFKJF"),CountArray(UserData.szName));
		UserData.lScore=100;
		m_lBulletCount[wChairID][enBulletCountKind_800]=wChairID+1;
		m_Cannon.SetBulletCount(wChairID,5,false);*/
		/////////////////////////////////////////////////////////////////////////////////

		//自己判断
		if (m_wMeChairID==wChairID)
		{
			//金币背景
			GetDrawPos(wChairID,enDrawPos_MeGoldBack,nDrawPosX,nDrawPosY);
			rcDraw.SetRect(0,0,100,24);
			m_UserInfo.DrawSprite(nDrawPosX,nDrawPosY,rcDraw);

			//金币数目
			GetDrawPos(wChairID,enDrawPos_MeGold,nDrawPosX,nDrawPosY);
			//LONGLONG lScore=LONGLONG(pUserData->lScore);
			LONGLONG lScore=123456789;//测试用
			DrawTextString(m_pFontUserInfo,_ui64tow(lScore,szBuffer,10),RGB(0,255,255),nDrawPosX,nDrawPosY);

			////金币数目(炮台右边)
			//GetDrawPos(wChairID,enDrawPos_MeGold,nDrawPosX,nDrawPosY);
			//LONGLONG lScore= m_lCoinCount[wChairID] + m_lBulletCount[wChairID]*m_lCellScore[wChairID];
			////LONGLONG lScore=123456789;//测试用
			//DrawNumber(nDrawPosX, nDrawPosY, lScore, TEXTURE_INDEX_COIN_COUNT);

			//rcClient.SetRect(220, 665, 360, 680);
			//TCHAR _szNumber[32];
			//sprintf(_szNumber, "%d", m_lCoinCount[m_wMeChairID] + m_lBulletCount[m_wMeChairID]*m_lCellScore[m_wMeChairID]);
			//m_pFontUserInfo->DrawTextA(NULL, _szNumber, (int)strlen(_szNumber), &rcClient,DT_SINGLELINE|DT_NOCLIP|DT_CENTER|DT_TOP, RGB(255,0,255)|0xFF000000);
			////m_pFontUserInfo->DrawTextA(NULL, "54321", (int)strlen("54321"), &rcClient,DT_SINGLELINE|DT_NOCLIP|DT_CENTER|DT_TOP, RGB(255,0,255)|0xFF000000);

			//左下角
			CRect rcCellScore1;
			rcCellScore1.SetRect(0, 600, 75, 620);
			//ok:m_pFontGameInfo->DrawTextA(NULL,"昵称DEF", (int)strlen("昵称DEF"), &rcCellScore1,DT_SINGLELINE|DT_NOCLIP|DT_CENTER|DT_TOP, RGB(255,0,255)|0xFF000000);
			/*m_pFontUserInfo*/m_pFontGameInfo->DrawTextA(NULL, pUserData->szName/*m_szNiceName[m_wMeChairID]*/, (int)strlen(pUserData->szName/*m_szNiceName[m_wMeChairID]*/), &rcCellScore1,DT_SINGLELINE|DT_NOCLIP|DT_CENTER|DT_TOP, RGB(255,0,255)|0xFF000000);
			//ok:m_pFontGameInfo->DrawTextW(NULL,L"昵称DEF", (int)wcslen(L"昵称DEF"), &rcCellScore1,DT_SINGLELINE|DT_NOCLIP|DT_LEFT|DT_TOP, RGB(0,0,255)|0xFF000000);

			//绘画子弹
			int nBulletCount=0;
			int nIndex=0;
			if (0!=m_lBulletCount[wChairID][enBulletCountKind_800])
			{
				rcDraw.SetRect(72,60,110,81);
				nBulletCount=m_lBulletCount[wChairID][enBulletCountKind_800];

				GetDrawPos(wChairID,enDrawPos_BulletBack,nDrawPosX,nDrawPosY,nIndex);
				m_UserInfo.DrawSprite(nDrawPosX,nDrawPosY,rcDraw);

				GetDrawPos(wChairID,enDrawPos_Bullet,nDrawPosX,nDrawPosY,nIndex);
				rcDraw.SetRect(0,0,57,20);
				DrawNumber(nDrawPosX,nDrawPosY,nBulletCount,TEXTURE_INDEX_BULLET_COUNT);

				++nIndex;
			}
			if (0!=m_lBulletCount[wChairID][enBulletCountKind_600])
			{
				rcDraw.SetRect(42,60,72,81);
				nBulletCount=m_lBulletCount[wChairID][enBulletCountKind_600];

				GetDrawPos(wChairID,enDrawPos_BulletBack,nDrawPosX,nDrawPosY,nIndex);
				m_UserInfo.DrawSprite(nDrawPosX+5,nDrawPosY,rcDraw);

				GetDrawPos(wChairID,enDrawPos_Bullet,nDrawPosX,nDrawPosY,nIndex);
				rcDraw.SetRect(0,0,57,20);
				DrawNumber(nDrawPosX,nDrawPosY,nBulletCount,TEXTURE_INDEX_BULLET_COUNT);

				++nIndex;
			}
			if (0!=m_lBulletCount[wChairID][enBulletCountKind_100])
			{
				rcDraw.SetRect(0,60,42,81);
				nBulletCount=m_lBulletCount[wChairID][enBulletCountKind_100];

				GetDrawPos(wChairID,enDrawPos_BulletBack,nDrawPosX,nDrawPosY,nIndex);
				m_UserInfo.DrawSprite(nDrawPosX,nDrawPosY,rcDraw);

				GetDrawPos(wChairID,enDrawPos_Bullet,nDrawPosX,nDrawPosY,nIndex);
				rcDraw.SetRect(0,0,57,20);
				DrawNumber(nDrawPosX,nDrawPosY,nBulletCount,TEXTURE_INDEX_BULLET_COUNT);

				++nIndex;
			}
			else
			{
				rcDraw.SetRect(0,60,42,81);
			}

			//没有子弹
			if (0==nBulletCount)
			{
				GetDrawPos(wChairID,enDrawPos_BulletBack,nDrawPosX,nDrawPosY,nIndex);
				m_UserInfo.DrawSprite(nDrawPosX,nDrawPosY,rcDraw);

				GetDrawPos(wChairID,enDrawPos_Bullet,nDrawPosX,nDrawPosY,nIndex);
				rcDraw.SetRect(0,0,57,20);
				m_Number.DrawSprite(nDrawPosX+15,nDrawPosY,rcDraw,TEXTURE_INDEX_NONE);
			}
		}
		else
		{
			//昵称背景
			GetDrawPos(wChairID,enDrawPos_AccountBack,nDrawPosX,nDrawPosY);
			rcDraw.SetRect(0,26,146,60);
			m_UserInfo.DrawSprite(nDrawPosX,nDrawPosY,rcDraw);

			//玩家昵称
			GetDrawPos(wChairID,enDrawPos_Account,nDrawPosX,nDrawPosY);
			DrawTextString(m_pFontUserInfo,CA2W(pUserData->szName),RGB(0,255,255),nDrawPosX,nDrawPosY);

			//绘画子弹
			int nBulletCount=0;
			if (0!=m_lBulletCount[wChairID][enBulletCountKind_800])
			{
				rcDraw.SetRect(72,60,110,81);
				nBulletCount=m_lBulletCount[wChairID][enBulletCountKind_800];
			}
			else if (0!=m_lBulletCount[wChairID][enBulletCountKind_600])
			{
				rcDraw.SetRect(42,60,72,81);
				nBulletCount=m_lBulletCount[wChairID][enBulletCountKind_600];
			}
			else if (0!=m_lBulletCount[wChairID][enBulletCountKind_100])
			{
				rcDraw.SetRect(0,60,42,81);
				nBulletCount=m_lBulletCount[wChairID][enBulletCountKind_100];
			}
			else
			{
				rcDraw.SetRect(0,60,42,81);
			}
			GetDrawPos(wChairID,enDrawPos_BulletBack,nDrawPosX,nDrawPosY);
			m_UserInfo.DrawSprite(nDrawPosX,nDrawPosY,rcDraw);

			GetDrawPos(wChairID,enDrawPos_Bullet,nDrawPosX,nDrawPosY);
			rcDraw.SetRect(0,0,57,20);
			if (nBulletCount>0) DrawNumber(nDrawPosX,nDrawPosY,nBulletCount,TEXTURE_INDEX_BULLET_COUNT);
			else m_Number.DrawSprite(nDrawPosX+15,nDrawPosY,rcDraw,TEXTURE_INDEX_NONE);
		}

		//绘画金币
		rcDraw.SetRect(148,24,299,5020);
		GetDrawPos(wChairID,enDrawPos_CoinBack,nDrawPosX,nDrawPosY);
		m_UserInfo.DrawSprite(nDrawPosX,nDrawPosY,rcDraw);

		//单元积分
		static WCHAR szBuffer[128]={0};
		_snwprintf(szBuffer,CountArray(szBuffer),L"(%ld倍)",m_lCellScore[wChairID]);
		CRect rcCellScore;
		rcCellScore.left=0;	
		rcCellScore.top=nDrawPosY+5;
		rcCellScore.right=0;
		rcCellScore.bottom=0;
		m_pFontGameInfo->DrawTextW(NULL, szBuffer, (int)wcslen(szBuffer), &rcCellScore,DT_SINGLELINE|DT_NOCLIP|DT_LEFT|DT_TOP|DT_CALCRECT, RGB(0,0,255)|0xFF000000);
		rcCellScore.left=nDrawPosX-rcCellScore.Width();
		m_pFontGameInfo->DrawTextW(NULL, szBuffer, (int)wcslen(szBuffer), &rcCellScore,DT_SINGLELINE|DT_NOCLIP|DT_LEFT|DT_TOP, RGB(0,0,255)|0xFF000000);

		GetDrawPos(wChairID,enDrawPos_Coin,nDrawPosX,nDrawPosY);
		DrawNumber(nDrawPosX,nDrawPosY,987654/*m_lCoinCount[wChairID]*/,TEXTURE_INDEX_COIN_COUNT);
		//金币数目
		nDrawPosX=100;
		nDrawPosY=600;
		DrawNumber(nDrawPosX,nDrawPosY,123456789/*m_lCoinCount[wChairID]*/,TEXTURE_INDEX_COIN_COUNT);
	}
}

//获取角度
float CGameClientViewDX::GetMeShootAngle()
{
	//获取角度
	float fAngle=m_Cannon.GetAngleCan(m_wMeChairID);

	//角度转换
	if (0==m_wMeChairID || 1==m_wMeChairID) fAngle=SwitchAngle(fAngle);

	return fAngle;
}

//玩家发炮
void CGameClientViewDX::UserShoot(WORD wChairID, float fAngle, enBulletCountKind BulletCountKind)
{
	//调整角度
	if (0==m_wMeChairID || 1==m_wMeChairID) fAngle=SwitchAngle(fAngle);

	//发射炮弹
	DWORD dwCptrFishID[5]={0};
	m_Cannon.SetAngleCan(wChairID, fAngle);

	//设置范围
	CRect rcBound;
	GetClientRect(rcBound);
	BYTE byBulletCount=m_Cannon.GetBulletCount(wChairID);
	if (byBulletCount>5) return;
	if (IsSameSide(m_wMeChairID,wChairID))
	{
		if (wChairID%2==0) rcBound.right-=(SHOOT_ROUND_SHRINK_X*(5-byBulletCount));
		else rcBound.left+=(SHOOT_ROUND_SHRINK_X*(5-byBulletCount));
		rcBound.top+=(SHOOT_ROUND_SHRINK_Y*(5-byBulletCount));
	}
	else
	{
		if (wChairID%2!=0) rcBound.right-=(SHOOT_ROUND_SHRINK_X*(5-byBulletCount));
		else rcBound.left+=(SHOOT_ROUND_SHRINK_X*(5-byBulletCount));
		rcBound.bottom-=(SHOOT_ROUND_SHRINK_Y*(5-byBulletCount));
	}
	m_Bullet.SetBound(rcBound);

	//发射炮弹
	m_Bullet.Shoot(wChairID,false, dwCptrFishID, 5, m_Cannon.GetBenchmarkPos(wChairID).x,m_Cannon.GetBenchmarkPos(wChairID).y,
		m_Cannon.GetAngleCan(wChairID), BulletCountKind,byBulletCount);
	m_Cannon.Shoot(wChairID);

	//播放声音
	TCHAR *pSiloSound[]={TEXT("Effect\\Silo0.mp3"),TEXT("Effect\\Silo1.mp3"),TEXT("Effect\\Silo2.mp3"),TEXT("Effect\\Silo3.mp3"),TEXT("Effect\\Silo4.mp3")};
	PlayMp3Sound(pSiloSound[byBulletCount-1],false);
}

//设置状态
bool CGameClientViewDX::SetFishStatus(enFishKind FishKind, DWORD dwFishID, enFishStatus FishStatus)
{
	//合法判断
	ASSERT(FishKind<enFishKind_No);
	if (!(FishKind<enFishKind_No)) return false;

	//设置状态
	g_FishGroup[FishKind].SetStatus(dwFishID, FishStatus);

	return true;
}

//增加金币
void CGameClientViewDX::UserAddCoin(WORD wChairID, int nSourcePosX, int nSourcePosY, LONG lCoinCount)
{
	//调整位置
	int nRectifyPosX[]={30,-30,40,-40,60,-60};
	int nCoinKind[]={20,10,1};
	int nTextureIndex[]={TEXTURE_INDEX_COIN_MOVE_3,TEXTURE_INDEX_COIN_MOVE_2,TEXTURE_INDEX_COIN_MOVE_1};

	//增加金币
	int nCoinPosX,nCoinPosY;
	GetDrawPos(wChairID,enDrawPos_Coin,nCoinPosX,nCoinPosY);
	for (int nKIndex=0; nKIndex<CountArray(nCoinKind); ++nKIndex)
	{
		if (0==lCoinCount%nCoinKind[nKIndex])
		{
			int nCount=lCoinCount/nCoinKind[nKIndex];
			for (int nCTIndex=0; nCTIndex<nCount; ++nCTIndex)
			{
				m_Coin.AddCoin(nSourcePosX+nRectifyPosX[rand()%CountArray(nRectifyPosX)], nSourcePosY, nCoinPosX, nCoinPosY, nSourcePosX, nSourcePosY, lCoinCount, nTextureIndex[nKIndex]);
			}

			break;
		}
	}

	//播放声音
	PlayMp3Sound(TEXT("Effect\\CoinFly.mp3"),false);
}

//增加彩金
void CGameClientViewDX::AddBonus(WORD wChairID, LONG lBonusCount)
{
	int const static nBonusPosX[2]={140,630};
	int const static nBonusPosY[2]={37+70,527-220};
	int nIndex=wChairID%2;
	if (IsSameSide(m_wMeChairID,wChairID))
		m_Bonus.AddBonus(lBonusCount,nBonusPosX[nIndex],nBonusPosY[1]);
	else
		m_Bonus.AddBonus(lBonusCount,nBonusPosX[1-nIndex],nBonusPosY[0]);
}

//鱼群种类
enFishKind CGameClientViewDX::GetFishKind(DWORD dwFish)
{
	for (enFishKind FishKind=enFishKind_1; FishKind<enFishKind_No; FishKind=enFishKind(FishKind+1))
	{
		tagFishInfo *pFishInfo=g_FishGroup[FishKind].GetFish(dwFish);
		if (NULL!=pFishInfo) return FishKind;
	}

	return enFishKind_No;
}

//创建窗口
bool CGameClientViewDX::InitDxWindow()
{
	//调用父类
	bool bResult=__super::InitDxWindow();

	//创建按钮
	m_btShowBulletShop.Create(NULL,WS_CHILD|WS_VISIBLE,CRect(10,10,20,30),this,IDC_SHOW_BULLET_SHOP);
	m_btQuitGame.Create(NULL,WS_CHILD|WS_VISIBLE,CRect(10,10,20,30),this,IDC_QUIT_GAME);
	m_btGameHelp.Create(NULL,WS_CHILD|WS_VISIBLE,CRect(10,10,20,30),this,IDC_SHOW_GAME_HELP);

	//按钮图片
	HINSTANCE hInstance=AfxGetInstanceHandle();
	m_btShowBulletShop.SetButtonImage(IDB_BT_SHOW_SHOP,hInstance,4,false);
	m_btQuitGame.SetButtonImage(IDB_BT_QUIT_GAME,hInstance,3,false);
	m_btGameHelp.SetButtonImage(IDB_BT_HELP,hInstance,4,false);

	//设置焦点（否则键盘消息没有响应）
	SetFocus();

	//设置时间
	SetTimer(IDI_IDLE_TIME,1000,NULL);

	//随机因子
	srand((unsigned)time(NULL));

	//创建窗体
	//km_TranslateWnd.Create(NULL,NULL,WS_CHILD|WS_VISIBLE|WS_CLIPCHILDREN|WS_CLIPSIBLINGS,CRect(10,10,20,30),this,IDC_WND);

	//SetTimer(IDI_CHANGE_SCENE_EFFECT,TIME_CHANGE_SCENE_EFFECT,NULL);
	//m_SeaScene[m_NewSeaScene].SetTride(true);

	//背景音乐
	TCHAR *pszBKSound[]={TEXT("BGSound0.mp3"),TEXT("BGSound1.mp3"),TEXT("BGSound2.mp3")};
	int nIndex=rand()%CountArray(pszBKSound);
	PlayMp3SoundBG(pszBKSound[nIndex]);
	PlayMp3Sound(nIndex?TEXT("Effect\\ShellOpen1.mp3"):TEXT("Effect\\ShellOpen2.mp3"),false);

	return bResult;
}

//大小消息
void CGameClientViewDX::OnSize(UINT nType, int cx, int cy)
{
	__super::OnSize(nType, cx, cy);

	//调整控件
	RectifyGameView(cx, cy);
}

//调整控件
VOID CGameClientViewDX::RectifyGameView(INT nWidth, INT nHeight)
{
	//合法判断
	if (NULL==m_btShowBulletShop.GetSafeHwnd()) return;

	//开始设置
	const UINT uFlags=SWP_NOACTIVATE|SWP_NOZORDER|SWP_NOCOPYBITS|SWP_NOSIZE;
	HDWP hDWP=BeginDeferWindowPos(10);

	//设置位置
	CRect rcButtonShop,rcButtonQuit;
	m_btShowBulletShop.GetWindowRect(&rcButtonShop);
	m_btQuitGame.GetWindowRect(&rcButtonQuit);
	DeferWindowPos(hDWP,m_btShowBulletShop,NULL,700,nHeight-rcButtonShop.Height(),0,0,uFlags);
	DeferWindowPos(hDWP,m_btQuitGame,NULL,700+rcButtonShop.Width()+5,nHeight-rcButtonShop.Height(),0,0,uFlags);
	DeferWindowPos(hDWP,m_btGameHelp,NULL,700+rcButtonShop.Width()+rcButtonQuit.Width()+10,nHeight-rcButtonShop.Height()+4,0,0,uFlags);

	//结束移动
	EndDeferWindowPos(hDWP);
}

//命令函数
BOOL CGameClientViewDX::OnCommand(WPARAM wParam, LPARAM lParam)
{
	//变量定义
	UINT nCommandID=LOWORD(wParam);
	UINT nCommandID2=HIWORD(wParam);

	//消息处理
	switch(nCommandID)
	{
	case IDC_SHOW_BULLET_SHOP:
		{
			////发送消息
			//if (NULL!=m_pGameClientDlg) m_pGameClientDlg->PostMessage(WM_SHOW_BULLET_SHOP,0,0);

			////设置焦点
			//SetFocus();

			return TRUE;
		}
	case IDC_QUIT_GAME:
		{
			//合法判断
			if (m_wMeChairID>=GAME_PLAYER) return false;
			if (true==m_bLookonMode) return false;

			//分数信息
			SetWaitScoreInfo(true);

			//发送消息
			if (NULL!=m_pGameClientDlg) m_pGameClientDlg->SendData(SUB_C_GET_SCORE_INFO);

			return TRUE;
		}
	case IDC_SHOW_GAME_HELP:
		{
			//游戏帮助
			CGameHelpDlg GameHelpDlg(this);
			GameHelpDlg.DoModal();

			return TRUE;
		}	
	//case IDC_SHOW_BALANCE://“结算”按钮
	//	{
	//		//合法判断
	//		if (m_wMeChairID>=GAME_PLAYER) return false;
	//		if (true==m_bLookonMode) return false;

	//		//分数信息
	//		SetWaitScoreInfo(true);

	//		//发送消息
	//		if (NULL!=m_pGameClientDlg) m_pGameClientDlg->SendData(SUB_C_GET_SCORE_INFO);

	//		return TRUE;
	//	}
	//case IDC_SHOW_GAME_HELP://“鱼价”按钮
	//	{
	//		//游戏帮助（“鱼的价值”对话框）
	//		CGameHelpDlg GameHelpDlg(this);
	//		GameHelpDlg.DoModal();

	//		return TRUE;
	//	}
	}
	return __super::OnCommand(wParam,lParam);
}

//设置子弹
void CGameClientViewDX::SetUserBulletCount(WORD wChairID, enBulletCountKind BulletCountKind, LONG lCount)
{
	//合法判断
	ASSERT(wChairID<GAME_PLAYER);
	ASSERT(BulletCountKind<enBulletCountKind_End);
	if (!(wChairID<GAME_PLAYER)) return;
	if (!(BulletCountKind<enBulletCountKind_End)) return;

	//设置变量
	m_lBulletCount[wChairID][BulletCountKind]=lCount;

	//警告判断
	if (m_wMeChairID==wChairID && true==m_bNoBulletWarn && lCount>40)
	{
		m_bNoBulletWarn=false;
	}
}

//设置金币
void CGameClientViewDX::SetUserCoinCount(WORD wChairID, int nCount)
{
	//合法判断
	ASSERT(wChairID<GAME_PLAYER);
	if (!(wChairID<GAME_PLAYER)) return;

	//设置变量
	m_lCoinCount[wChairID]=nCount;
}

//绘画数字
void CGameClientViewDX::DrawNumber(int nXPos, int nYPos, LONGLONG lNumber, int nTextIndex)
{
	//获取属性
	const INT nNumberHeight=m_Number.GetHeight(nTextIndex);
	const INT nNumberWidth=m_Number.GetWidth(nTextIndex);

	//计算数目
	LONG lNumberCount=0;
	LONGLONG lNumberTemp=lNumber;
	do
	{
		lNumberCount++;
		lNumberTemp/=10;
	} while (lNumberTemp>0L);

	//位置定义
	int const nSpace=0;
	INT nYDrawPos=nYPos;
	INT nXDrawPos=nXPos+lNumberCount*(nNumberWidth+nSpace);

	//当前位置
	CRect drawRect;

	//绘画号码
	for (LONG i=0;i<lNumberCount;i++)
	{
		//绘画号码
		WORD wCellNumber=WORD(lNumber%10);
		drawRect.left=wCellNumber*nNumberWidth;
		drawRect.top=0;
		drawRect.right=drawRect.left+m_Number.GetWidth(nTextIndex);
		drawRect.bottom=drawRect.top+m_Number.GetHeight(nTextIndex);
		m_Number.DrawSprite(nXDrawPos,nYDrawPos,drawRect,nTextIndex);

		//设置变量
		lNumber/=10;
		nXDrawPos-=(nNumberWidth+nSpace);
	}
}

//获取位置
void CGameClientViewDX::GetDrawPos(WORD wChairID,enDrawPos DrawPos, int &nPosX, int &nPosY, int nIndex)
{
	//设置变量
	nPosX=0;
	nPosY=0;

	//合法判断
	ASSERT(wChairID<GAME_PLAYER);
	if (!(wChairID<GAME_PLAYER)) return;

	//获取大小
	CRect rcClient;
	GetClientRect(&rcClient);

	//自己判断
	if (IsSameSide(m_wMeChairID,wChairID))
	{
		//左边判断
		if (wChairID%2==0)
		{
			//自己判断
			if (m_wMeChairID==wChairID)
			{
				if (enDrawPos_MeGoldBack==DrawPos)
				{
					nPosX=105;
					nPosY=rcClient.Height()-m_ToolBarBk.GetHeight()-12;
				}
				else if (enDrawPos_MeGold==DrawPos)
				{
					nPosX=105+35;
					nPosY=rcClient.Height()-m_ToolBarBk.GetHeight()-8;
				}
				else if (enDrawPos_BulletBack==DrawPos)
				{
					if (0==nIndex)
						nPosY=rcClient.Height()/2+195;
					else if (1==nIndex)
						nPosY=rcClient.Height()/2+173;
					else if (2==nIndex)
						nPosY=rcClient.Height()/2+151;
					nPosX=95;

				}
				else if (enDrawPos_Bullet==DrawPos)
				{
					if (0==nIndex)
						nPosY=rcClient.Height()/2+198;
					else if (1==nIndex)
						nPosY=rcClient.Height()/2+198-22;
					else if (2==nIndex)
						nPosY=rcClient.Height()/2+198-22-22;

					nPosX=116;					
				}
				else if (enDrawPos_CoinBack==DrawPos)
				{
					nPosX=95;
					nPosY=rcClient.Height()/2+218;
				}
				else if (enDrawPos_Coin==DrawPos)
				{
					nPosX=122;
					nPosY=rcClient.Height()/2+219;
				}				
			}
			else
			{
				if (enDrawPos_AccountBack==DrawPos)
				{
					nPosX=55;
					nPosY=rcClient.Height()-m_ToolBarBk.GetHeight()-21;
				}
				else if (enDrawPos_Account==DrawPos)
				{
					nPosX=102;
					nPosY=539;
				}
				else if (enDrawPos_BulletBack==DrawPos)
				{
					nPosX=51;
					nPosY=rcClient.Height()/2+188;
				}
				else if (enDrawPos_Bullet==DrawPos)
				{
					nPosX=77;
					nPosY=rcClient.Height()/2+191;
				}
				else if (enDrawPos_CoinBack==DrawPos)
				{
					nPosX=51;
					nPosY=rcClient.Height()/2+208;
				}
				else if (enDrawPos_Coin==DrawPos)
				{
					nPosX=77;
					nPosY=rcClient.Height()/2+208;
				}				
			}
		}
		else
		{
			//自己判断
			if (m_wMeChairID==wChairID)
			{
				if (enDrawPos_MeGoldBack==DrawPos)
				{
					nPosX=820+70;
					nPosY=rcClient.Height()-m_ToolBarBk.GetHeight()-12;
				}
				else if (enDrawPos_MeGold==DrawPos)
				{
					nPosX=854+70;
					nPosY=rcClient.Height()-m_ToolBarBk.GetHeight()-8;
				}
				else if (enDrawPos_BulletBack==DrawPos)
				{
					if (0==nIndex)
						nPosY=492;
					else if (1==nIndex)
						nPosY=492-22;
					else if (2==nIndex)
						nPosY=492-22-22;

					nPosX=805+70;
				}
				else if (enDrawPos_Bullet==DrawPos)
				{
					if (0==nIndex)
						nPosY=495;
					else if (1==nIndex)
						nPosY=495-22;
					else if (2==nIndex)
						nPosY=495-22-22;

					nPosX=829+70;					
				}
				else if (enDrawPos_CoinBack==DrawPos)
				{
					nPosX=805+70;
					nPosY=515;
				}
				else if (enDrawPos_Coin==DrawPos)
				{
					nPosX=833+70;
					nPosY=516;
				}				
			}
			else
			{
				if (enDrawPos_AccountBack==DrawPos)
				{
					nPosX=869;
					nPosY=rcClient.Height()-m_ToolBarBk.GetHeight()-21;
				}
				else if (enDrawPos_Account==DrawPos)
				{
					nPosX=915;
					nPosY=539;
				}
				else if (enDrawPos_BulletBack==DrawPos)
				{
					nPosX=865;
					nPosY=485;
				}
				else if (enDrawPos_Bullet==DrawPos)
				{
					nPosX=889;
					nPosY=488;
				}
				else if (enDrawPos_CoinBack==DrawPos)
				{
					nPosX=865;
					nPosY=505;
				}
				else if (enDrawPos_Coin==DrawPos)
				{
					nPosX=891;
					nPosY=507;
				}				
			}
		}
	}
	else
	{
		//左边判断
		if (wChairID%2!=0)
		{
			if (enDrawPos_AccountBack==DrawPos)
			{
				nPosX=55;
				nPosY=0;
			}
			else if (enDrawPos_Account==DrawPos)
			{
				nPosX=102;
				nPosY=8;
			}
			else if (enDrawPos_BulletBack==DrawPos)
			{
				nPosX=51;
				nPosY=60;
			}
			else if (enDrawPos_Bullet==DrawPos)
			{
				nPosX=75;
				nPosY=63;
			}
			else if (enDrawPos_CoinBack==DrawPos)
			{
				nPosX=51;
				nPosY=33;
			}
			else if (enDrawPos_Coin==DrawPos)
			{
				nPosX=77;
				nPosY=35;
			}
		}
		else
		{
			if (enDrawPos_AccountBack==DrawPos)
			{
				nPosX=869;
				nPosY=0;
			}
			else if (enDrawPos_Account==DrawPos)
			{
				nPosX=915;
				nPosY=8;
			}
			else if (enDrawPos_BulletBack==DrawPos)
			{
				nPosX=865;
				nPosY=60;
			}
			else if (enDrawPos_Bullet==DrawPos)
			{
				nPosX=889;
				nPosY=63;
			}
			else if (enDrawPos_CoinBack==DrawPos)
			{
				nPosX=865;
				nPosY=33;
			}
			else if (enDrawPos_Coin==DrawPos)
			{
				nPosX=891;
				nPosY=35;
			}
		}
	}
}

//子弹类型
enBulletCountKind CGameClientViewDX::GetCurUseBulletKind(WORD wChairID)
{
	//合法判断
	ASSERT(wChairID<GAME_PLAYER);
	if (!(wChairID<GAME_PLAYER)) return enBulletCountKind_100;

	if (m_lBulletCount[wChairID][enBulletCountKind_800]>0) return enBulletCountKind_800;
	else if (m_lBulletCount[wChairID][enBulletCountKind_600]>0) return enBulletCountKind_600;
	else if (m_lBulletCount[wChairID][enBulletCountKind_100]>0) return enBulletCountKind_100;

	return enBulletCountKind_100;
}

//玩家射击
bool CGameClientViewDX::_UserShoot(WORD wChairID, bool bReleaseKey)
{
	//旁观判断
	if (true==m_bLookonMode) return false;

	//激活判断
	CWnd *pActiveWnd=GetActiveWindow();
	CWnd *pMainFrame=AfxGetMainWnd();
	if (NULL==pActiveWnd) return false;
	if (NULL==pMainFrame) return false;
	if (pActiveWnd->GetSafeHwnd()!=pMainFrame->GetSafeHwnd()) return false;

	//时间判断
	bool bContinuous=(m_lBulletCount[wChairID][enBulletCountKind_600]+m_lBulletCount[wChairID][enBulletCountKind_800]>0)?true:false;
	if (true==bContinuous && (m_dwLastShootTime+TIME_SHOOT_SPEED_F)>GetTickCount()) return false;
	else if (false==bContinuous && ((m_dwLastShootTime+TIME_SHOOT_SPEED_S)>GetTickCount() || false==bReleaseKey)) return false;

	//设置时间
	m_dwLastShootTime=GetTickCount();

	//合法判断
	WORD wBulletCount=m_Cannon.GetBulletCount(wChairID);
	LONG lShootCount=0;
	enBulletCountKind BulletCountKind=enBulletCountKind_End;
	if (m_lBulletCount[wChairID][enBulletCountKind_800]>0) BulletCountKind=enBulletCountKind_800;
	else if (m_lBulletCount[wChairID][enBulletCountKind_600]>0) BulletCountKind=enBulletCountKind_600;
	else if (m_lBulletCount[wChairID][enBulletCountKind_100]>0) BulletCountKind=enBulletCountKind_100;

	//备份数目
	LONG lPreBulletCount=m_lBulletCount[wChairID][enBulletCountKind_800]+m_lBulletCount[wChairID][enBulletCountKind_600]+m_lBulletCount[wChairID][enBulletCountKind_100];

	//合法判断
	if (BulletCountKind!=enBulletCountKind_End)
	{
		if (m_lBulletCount[wChairID][BulletCountKind]>=wBulletCount)
		{
			lShootCount=wBulletCount;
			m_lBulletCount[wChairID][BulletCountKind]-=wBulletCount;			
		}
		else
		{
			lShootCount=m_lBulletCount[wChairID][BulletCountKind];
			m_lBulletCount[wChairID][BulletCountKind]=0;
		}

		//切换子弹
		if (m_lBulletCount[wChairID][BulletCountKind]<wBulletCount && m_lBulletCount[wChairID][BulletCountKind]>0) m_Cannon.SetBulletCount(wChairID, m_lBulletCount[wChairID][BulletCountKind],false);
		else if (m_lBulletCount[wChairID][enBulletCountKind_600]>0) m_Cannon.SetBulletCount(wChairID, min(m_lBulletCount[wChairID][enBulletCountKind_600],wBulletCount),false);
		else if (m_lBulletCount[wChairID][enBulletCountKind_100]>0) m_Cannon.SetBulletCount(wChairID, min(m_lBulletCount[wChairID][enBulletCountKind_100],wBulletCount),false);

		//设置范围
		CRect rcBound;
		GetClientRect(rcBound);
		BYTE byBulletCount=m_Cannon.GetBulletCount(wChairID);
		if (IsSameSide(m_wMeChairID,wChairID))
		{
			if (wChairID%2==0) rcBound.right-=(SHOOT_ROUND_SHRINK_X*(5-byBulletCount));
			else rcBound.left+=(SHOOT_ROUND_SHRINK_X*(5-byBulletCount));
			rcBound.top+=(SHOOT_ROUND_SHRINK_Y*(5-byBulletCount));
		}
		else
		{
			if (wChairID%2!=0) rcBound.right-=(SHOOT_ROUND_SHRINK_X*(5-byBulletCount));
			else rcBound.left+=(SHOOT_ROUND_SHRINK_X*(5-byBulletCount));
			rcBound.bottom-=(SHOOT_ROUND_SHRINK_Y*(5-byBulletCount));
		}
		m_Bullet.SetBound(rcBound);

		//发射炮弹
		DWORD dwCptrFishID[5]={0};
		DWORD dwBulletID=m_Bullet.Shoot(wChairID,true, dwCptrFishID, 5, m_Cannon.GetBenchmarkPos(m_wMeChairID).x,m_Cannon.GetBenchmarkPos(m_wMeChairID).y,
			m_Cannon.GetAngleCan(m_wMeChairID), BulletCountKind,byBulletCount);
		m_Cannon.Shoot(m_wMeChairID);

		//合法判断
		ASSERT(0L!=dwBulletID);
		if (0L==dwBulletID) return false;

		//发送消息
		if (NULL!=m_pGameClientDlg) m_pGameClientDlg->PostMessage(WM_SHOOT,WPARAM(dwBulletID),MAKELPARAM(BulletCountKind,lShootCount));

		//播放声音
		TCHAR *pSiloSound[]={TEXT("Effect\\Silo0.mp3"),TEXT("Effect\\Silo1.mp3"),TEXT("Effect\\Silo2.mp3"),TEXT("Effect\\Silo3.mp3"),TEXT("Effect\\Silo4.mp3")};
		PlayMp3Sound(pSiloSound[wBulletCount-1],false);

		//警告判断
		LONG lLeftBulletCount=m_lBulletCount[wChairID][enBulletCountKind_800]+m_lBulletCount[wChairID][enBulletCountKind_600]+m_lBulletCount[wChairID][enBulletCountKind_100];
		if (lPreBulletCount>=40 && lLeftBulletCount>0 && lLeftBulletCount<40) 
		{
			//设置标识
			m_bNoBulletWarn=true;
			m_dwNoBulletWarnTime=GetTickCount();

			//设置位置
			int nDrawPosX,nDrawPosY;
			GetDrawPos(m_wMeChairID,enDrawPos_BulletBack,nDrawPosX,nDrawPosY);
			m_NoBulletWarn.SetDrawPos(nDrawPosX-70,nDrawPosY-50);

			//设置时间
			SetTimer(IDI_NO_BULLET_WARN,6000,NULL);
		}
		else if (lLeftBulletCount>40)
		{
			m_bNoBulletWarn=false;
		}

		//播放声音
		if (true==m_bNoBulletWarn) PlayMp3Sound(TEXT("Effect\\NoBullet.mp3"),false);

		//设置时间
		SetTimer(IDI_IDLE_TIME,1000,NULL);
		m_wIdleTime=IDLE_TIME;
	}
	else
	{
		//发送消息
		if (NULL!=m_pGameClientDlg) m_pGameClientDlg->PostMessage(WM_SHOW_COIN_SHOP/*WM_SHOW_BULLET_SHOP*/,0,0);
	}

	return true;
}

//退出查询
bool CGameClientViewDX::QuitGameInquire()
{
	//合法判断
	if (INVALID_CHAIR==m_wMeChairID) return false;
	if (true==m_bLookonMode) return false;

	//子弹数目
	for (enBulletCountKind BulletCountKind=enBulletCountKind_100; BulletCountKind<enBulletCountKind_End; BulletCountKind=enBulletCountKind(BulletCountKind+1))
	{
		if (m_lBulletCount[m_wMeChairID][BulletCountKind]>0) return true;
	}

	//金币数目
	if (m_lCoinCount[m_wMeChairID]>0) return true;

	return false;
}

//显示成绩
bool CGameClientViewDX::ShowScoreView()
{
	//变量定义
	CFishScoreView ScoreView(this, m_lCellScore[m_wMeChairID]);

	//设置信息
	ScoreView.SetScoreInfo(m_lBulletCount[m_wMeChairID], m_wCptrFishCount, m_lCoinCount[m_wMeChairID], m_lBulletCharge);

	//显示对话框
	if (IDOK==ScoreView.DoModal())
	{
		//设置焦点
		SetFocus();

		//发送消息
		if (NULL!=m_pGameClientDlg) m_pGameClientDlg->SendData(SUB_C_CALC_SCORE);		
	}

	return true;
}

//旁观模式
void CGameClientViewDX::SetLokonMode(bool bLookonMode)
{
	m_bLookonMode=bLookonMode;
	if (true==m_bLookonMode) KillTimer(IDI_IDLE_TIME);
}

//切换场景
void CGameClientViewDX::SetChangeScene(bool bWaveLToR, bool bChange, enSeaSceneKind SeaSceneKind)
{
	//改变判断
	if (true==bChange)
	{
		//修改时间
		/*WORD nEnumIndex=0;
		tagFishInfo *pFish=NULL;*/
		//for (enFishKind FishKind=enFishKind_1; FishKind<enFishKind_No; FishKind=enFishKind(FishKind+1))
		//{
		//	//nEnumIndex=0;
		//	//do
		//	//{
		//	//	//枚举鱼群
		//	//	pFish=g_FishGroup[FishKind].EnumFish(nEnumIndex++);
		//	//	if (NULL==pFish) break;

		//	//	//重置时间
		//	//	pFish->bySpaceTimes=0;

		//	//} while (true);
		//}

		//设置变量
		m_NewSeaScene=SeaSceneKind;
		m_bWaveEffect=false;
		m_bWaveEffect=false;
		m_bWaveLToR=bWaveLToR;
		m_nSceneMovePosX=0;
		m_bWaveComing=true;		

		//设置时间
		SetTimer(IDI_CHANGE_SCENE,TIME_CHANGE_SCENE,NULL);

		//涨潮通知
		CFishGroup::SetWaveComing(true);

		//播放声音
		PlayMp3Sound(TEXT("Effect\\WaveEnter.mp3"),false);
	}
	else
	{
		//切换判断
		if (m_CurSeaScene!=SeaSceneKind)
		{
			//加载资源
			LoadSeaScene(SeaSceneKind);

			//释放资源
			ReleaseSeaScene(m_CurSeaScene);

			//设置变量
			m_CurSeaScene=SeaSceneKind;
		}	
	}
}

//子弹数目
LONG CGameClientViewDX::GetBulletCount(WORD wChairID, enBulletCountKind BulletCountKind)
{
	//合法判断
	ASSERT(wChairID<GAME_PLAYER);
	ASSERT(BulletCountKind<enBulletCountKind_End);
	if (!(wChairID<GAME_PLAYER)) return 0;
	if (!(BulletCountKind<enBulletCountKind_End)) return 0;

	return m_lBulletCount[wChairID][BulletCountKind];
}

//获取组件
CAudioPlayer *CGameClientViewDX::GetaudioPlayer(LPCTSTR pszAudioName)
{
	//变量定义
	CAudioPlayer *pAudioPlayer=NULL;
	int nIndex=0;

	//存在判断
	/*for (int i=0; i<MAX_SYN_SOUND; ++i)
	{
	if (0==lstrcmpi(pszAudioName,m_AudioPlayerAsyn[i].GetAudioName()) && m_AudioPlayerAsyn[i].GetDeviceType()!=0) 
	{
	pAudioPlayer=&m_AudioPlayerAsyn[i];
	nIndex=i;
	break;
	}
	}*/

	//寻找空位
	if (NULL==pAudioPlayer)
	{
		for (int i=0; i<MAX_SYN_SOUND; ++i)
		{
			if (m_AudioPlayerAsyn[i].GetDeviceType()==0) 
			{
				pAudioPlayer=&m_AudioPlayerAsyn[i];
				nIndex=i;
				break;
			}
		}
	}
	else 
	{
		//关闭文件
		pAudioPlayer->Stop();
		pAudioPlayer->RePlay();
		return NULL;
	}

	//创建判断
	if (NULL!=pAudioPlayer && NULL==pAudioPlayer->GetSafeHwnd()) 
	{
		pAudioPlayer->Create(NULL,NULL,WS_CHILD,CRect(0,0,0,0),this,500+nIndex,NULL);
		//pAudioPlayer->SetGameFrameDlg(this);
	}

	return pAudioPlayer;
}

//播放声音
bool CGameClientViewDX::PlayMp3Sound(LPCTSTR pszSoundName, bool bSyn)
{
	//禁用判断
	if (m_pGameClientDlg->IsEnableSound()==false) return false;

	try
	{
		//构造路径
		TCHAR szFullPath[MAX_PATH]={0};
		_sntprintf(szFullPath,CountArray(szFullPath),TEXT("%sFish\\Sound\\%s"),g_szWorkDir,pszSoundName);

		//异步判断
		if (false==bSyn)
		{
			CAudioPlayer *pAudioPlayer=GetaudioPlayer(szFullPath);
			if (NULL!=pAudioPlayer) pAudioPlayer->Play(szFullPath);
		}
		else
		{
			m_AudioPlayerSyn.Play(szFullPath,false);
		}
	}
	catch (...)
	{
		return false;
	}

	return true;
}

//播放声音
bool CGameClientViewDX::PlayMp3Sound(LPCTSTR pszPath, LPCTSTR pszSoundName, bool bSyn)
{
	//禁用判断
	if (m_pGameClientDlg->IsEnableSound()==false) return false;

	try
	{
		//构造路径
		TCHAR szFullPath[MAX_PATH]={0};
		_sntprintf(szFullPath,CountArray(szFullPath),TEXT("%s%s"),pszPath,pszSoundName);

		//异步判断
		if (false==bSyn)
		{
			CAudioPlayer *pAudioPlayer=GetaudioPlayer(szFullPath);
			if (NULL!=pAudioPlayer) pAudioPlayer->Play(szFullPath);
		}
		else
		{
			m_AudioPlayerSyn.Play(szFullPath,false);
		}
	}
	catch (...)
	{
		return false;
	}

	return true;
}

//播放声音
bool CGameClientViewDX::PlayMp3SoundBG(LPCTSTR pszSoundName, bool bLoop)
{
	//禁用判断
	if (m_pGameClientDlg->IsEnableSound()==false) return false;
	//#ifdef _DEBUG
	//	InsertSystemString(TEXT("PlayMp3SoundBG0"));
	//#endif

	try
	{
		//构造路径
		TCHAR szFullPath[MAX_PATH]={0};
		_sntprintf(szFullPath,CountArray(szFullPath),TEXT("%sFish\\Sound\\%s"),g_szWorkDir,pszSoundName);

		//播放声音
		m_AudioPlayerBG.Play(szFullPath,bLoop);
	}
	catch (...)
	{
		return false;
	}

	return true;
}

//播放声音
bool CGameClientViewDX::PlayMp3SoundBG(LPCTSTR pszPath, LPCTSTR pszSoundName, bool  bLoop)
{
	//禁用判断
	if (m_pGameClientDlg->IsEnableSound()==false) return false;

	//#ifdef _DEBUG
	//	InsertSystemString(TEXT("PlayMp3SoundBG"));
	//#endif

	try
	{
		//构造路径
		TCHAR szFullPath[MAX_PATH]={0};
		_sntprintf(szFullPath,CountArray(szFullPath),TEXT("%s%s"),pszPath,pszSoundName);

		//播放声音
		m_AudioPlayerBG.Play(szFullPath,bLoop);
	}
	catch (...)
	{
		return false;
	}

	return true;
}

//停止声音
bool CGameClientViewDX::StopMp3SoundBG()
{
	//#ifdef _DEBUG
	//	InsertSystemString(TEXT("StopMp3SoundBG"));
	//#endif

	try
	{
		m_AudioPlayerBG.Stop();
	}
	catch (...)
	{
		return false;
	}

	return true;
}
//暂停声音
bool CGameClientViewDX::PauseMp3SoundBG()
{
	try
	{
		m_AudioPlayerBG.Pause();
	}
	catch (...)
	{
		return false;
	}

	return true;
}

//打开网
LRESULT CGameClientViewDX::OnNetOpen(WPARAM wParam, LPARAM lParam)
{
	//播放声音
	PlayMp3Sound(TEXT("Effect\\NetOpen.mp3"),false);

	return 0L;
}

//当前数目
LONG CGameClientViewDX::GetCurBulletCount(WORD wChairID)
{
	//合法判断
	ASSERT(wChairID<GAME_PLAYER);
	if (!(wChairID<GAME_PLAYER)) return 1;

	if (m_lBulletCount[wChairID][enBulletCountKind_800]>0) return m_lBulletCount[wChairID][enBulletCountKind_800];
	else if (m_lBulletCount[wChairID][enBulletCountKind_600]>0) return m_lBulletCount[wChairID][enBulletCountKind_600];
	else if (m_lBulletCount[wChairID][enBulletCountKind_100]>0) return m_lBulletCount[wChairID][enBulletCountKind_100];

	return 1;
}

//切换炮弹
void CGameClientViewDX::SwitchCannon()
{
	//合法判断
	if (INVALID_CHAIR==m_wMeChairID) return;

	//旁观判断
	if (true==m_bLookonMode) return;

	//数目判断
	int nBulletCount=m_Cannon.GetBulletCount(m_wMeChairID);
	LONG lCurBulletCount=GetCurBulletCount(m_wMeChairID);
	if (nBulletCount==lCurBulletCount && lCurBulletCount<5) 
		nBulletCount=1;
	else
		nBulletCount=(nBulletCount!=4) ? ((nBulletCount+1)%MAX_BULLET_COUNT):5;

	//设置子弹
	int nMinCount=min(nBulletCount,lCurBulletCount);
	m_Cannon.SetBulletCount(m_wMeChairID, nMinCount,true);

	//播放声音
	PlayMp3Sound(TEXT("Effect\\SwitchSilo.mp3"),false);
}
////////////////////////////////////////////////////////////////////////////////////

BOOL CGameClientViewDX::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	//设置光标
	SetCursor(LoadCursor(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDC_CARD_CUR)));

	return __super::OnSetCursor(pWnd, nHitTest, message);
}

//消息处理
BOOL CGameClientViewDX::PreTranslateMessage(MSG* pMsg)
{
	//键盘输入
	if (WM_KEYDOWN==pMsg->message)
	{
		//合法判断
		bool bKeyDown=true;
		CWnd *pActiveWnd=GetActiveWindow();
		CWnd *pMainFrame=AfxGetMainWnd();
		if (INVALID_CHAIR==m_wMeChairID) bKeyDown=false;
		if (true==m_bLookonMode) bKeyDown=false;
		if (NULL==pActiveWnd) bKeyDown=false;
		if (NULL==pMainFrame) bKeyDown=false;
		if (pActiveWnd->GetSafeHwnd()!=pMainFrame->GetSafeHwnd()) bKeyDown=false;
		if (false==bKeyDown) return __super::PreTranslateMessage(pMsg);

		//数字判断
		if (pMsg->wParam==49 || pMsg->wParam==50 || pMsg->wParam==51 || pMsg->wParam==52 || pMsg->wParam==53)
		{
			LONG nMinCount=min(INT(pMsg->wParam-49+1),GetCurBulletCount(m_wMeChairID));
			m_Cannon.SetBulletCount(m_wMeChairID,nMinCount,true);

			//播放声音
			PlayMp3Sound(TEXT("Effect\\SwitchSilo.mp3"),false);
		}
		else if (VK_SPACE==pMsg->wParam || VK_UP==pMsg->wParam)
		{
			//空格判断
			if (VK_SPACE==pMsg->wParam)
			{
				//激活判断
				CWnd *pFocusWnd=GetFocus();
				if (NULL==pFocusWnd) return TRUE;
				if (pFocusWnd->GetSafeHwnd()!=GetSafeHwnd()) return TRUE;
			}

			//发射判断
			_UserShoot(m_wMeChairID, m_bReleaseKey);

			//设置标志
			m_bReleaseKey=false;
		}
		else if (VK_LEFT==pMsg->wParam)
		{
			//设置焦点
			SetFocus();

			//设置角度
			float fNowAngle=m_Cannon.GetAngleCan(m_wMeChairID);
			fNowAngle-=0.1f;
			if (fNowAngle<ME_MIN_CAN_ANGLE) fNowAngle=ME_MIN_CAN_ANGLE;
			m_Cannon.SetAngleCan(m_wMeChairID,fNowAngle);
		}
		else if (VK_RIGHT==pMsg->wParam)
		{
			//设置焦点
			SetFocus();

			//设置角度
			float fNowAngle=m_Cannon.GetAngleCan(m_wMeChairID);
			fNowAngle+=0.1f;
			if (fNowAngle>ME_MAX_CAN_ANGLE) fNowAngle=ME_MAX_CAN_ANGLE;
			m_Cannon.SetAngleCan(m_wMeChairID,fNowAngle);
		}
		else if (VK_DOWN==pMsg->wParam)
		{
			//切换炮弹
			if (m_dwLastSwitchTime+300<=GetTickCount())
			{
				SwitchCannon();	
				m_dwLastSwitchTime=GetTickCount();
			}
		}			

		return TRUE;
	}
	else if (WM_KEYUP==pMsg->message)
	{
		m_bReleaseKey=true;
	}

	return __super::PreTranslateMessage(pMsg);
}


BOOL CGameClientViewDX::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	//键盘输入
	if(message==WM_KEYDOWN)
	{
		int a=0;
		//OnNcHitTest(wParam,lParam);
		//return TRUE;
	}
	////WM_NCHITTEST消息
	//if(message==WM_NCHITTEST)
	//{
	//	int a=0;
	//	//OnNcHitTest(wParam,lParam);
	//	//return TRUE;
	//}
	//////轨迹消息
	////if(message==WM_FISH_CALC_TRACE)
	////{
	////	OnFinishCalcTrace(wParam,lParam);
	////	return TRUE;
	////}
	//////打开网 
	////if(message==WM_NET_OPEN)
	////{
	////	OnNetOpen(wParam,lParam);
	////	return TRUE;
	////}
	////退出游戏,
	//if(message==WM_QUIT_GAME)
	//{
	//	OnQuitGame(wParam,lParam);
	//	return TRUE;
	//}
	////击中鱼群
	//if(message==WM_HIT_FISH)
	//{
	//	OnHitFish(wParam,lParam);
	//	return TRUE;
	//}
	////激光打鱼(使用能量炮)
	//if(message==WM_HIT_Laser)
	//{
	//	OnHitLaser(wParam,lParam);
	//	return TRUE;
	//}
	return __super::OnWndMsg(message,wParam,lParam,pResult);
}



