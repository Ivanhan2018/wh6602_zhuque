#include "Stdafx.h"
#include "Mmsystem.h"
#include "Resource.h"
#include "GameFrameDlg.h"
#include "GlobalOption.h"
#include "GameFrameControl.h"
#include "ClientKernelSink.h"
#include "UserDaoJu.h"
#include "SendMsgDlg.h"
#include "ScoreView.h"
#include "GameTaskDialog.h"
#include "ShowTimeDJList.h"
#include "ShowDJInfo.h"
#include "GameClientDJInfo.h"
#include "GameGiveDJInfo.h"
#include "UserAction.h"
#include "ShowUserActionInfo.h"
#include "DirectSound.h"
#include "MIDI.h"
#include "GameSoundSet.h"
#include "GameFrameNetDefine.h"
#include "UserInfoDlg.h"
#include "GameAffiche.h"
#include "LanguageList.h"
#include "TaskFinishFlash.h"
#include "GameFrameBorder.h"
#include "NewGiftFlashDlg.h"

//打印游戏自定义消息
//#define		PRINT_GAME_CURS_MSG			


//游戏公告
#define		GAME_MSG_TIME_ID			1099			//定时器ID

#define		GAME_MSG_TIME_INTERVAL		30				//定时器时间隔

//分隔线
#define		GAMEINFO_L					120				//左宽
#define		GAMEINFO_R					120				//右宽


#define			GAME_FRAME_DLG_WIDTH		960
#define			GAME_FRAME_DLG_HEIGHT		29


//////////////////////////////////////////////////////////////////////////
CDirectSound                        g_dirSound;                             // --普通游戏声音
CDirectSound                      * g_pdirBGMusic;                          // --WAVE 背景音乐
CMIDI                             * g_pmdBGMusic;                           // --MIDI 背景音乐
CMciSound                           g_mciPlyr;                              // --
//////////////////////////////////////////////////////////////////////////

//方言
#define REG_USER_INFO			TEXT("Software\\WHQPGame\\GamePlaza\\OptionLogon")
//////////////////////////////////////////////////////////////////////////

CGameFrameDlg * g_GameFrameDlg = NULL;

BEGIN_MESSAGE_MAP(CGameFrameDlg, CDialog)
	ON_WM_TIMER()
	ON_WM_SIZE()
	ON_WM_GETMINMAXINFO()
	ON_WM_WINDOWPOSCHANGING()
	ON_BN_CLICKED(IDC_CONTROL, OnBnClickedControl)
	ON_BN_CLICKED(IDC_BT_TITLE_MIN, OnBnMinClickedEvent)
	ON_BN_CLICKED(IDC_BT_TITLE_SOUND, OnBnSoundClickedEvent)
	ON_BN_CLICKED(IDC_BT_TITLE_CLOSE, OnBnCloseClieckedEvent)
	ON_BN_CLICKED(IDC_BT_TITLE_GAMEPHOTO, OnBnGamePhotoClieckedEvent)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_NCHITTEST()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOVING()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_WINDOWPOSCHANGED()
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CGameFrameDlg, CDialog)
//构造函数
CGameFrameDlg::CGameFrameDlg(CGameFrameView * pGameFrameView, CScoreView* pScoreView)  : CDialog(IDD_GAME_FRAME), m_VorSplitter(VorSpliter)
{
	g_GameFrameDlg = this;
	//界面状态
	m_bHideList=false;
	m_rcNormalSize.SetRect(0,0,0,0);

	//设置变量
	m_bReset=false;
	m_bGameSceen=false;
	m_bInitDialog=false;
	m_bIsMouseDown = false;

	//组件接口
	m_pKernelSink=NULL;
	m_pGameFrameControl=NULL;
	//m_pSendMsgDlg = NULL;
	//用户道具栏
	m_pGameTaskDlg = NULL;
	//m_pShowTimeDJList = NULL;
	m_pShowDJInfo = NULL;
	//m_pGameBizCity = NULL;
	//m_pGameGiveDJInfo = NULL;
	//m_pGameUserAction = NULL;
	//m_pShowUserActionInfo = NULL;

	//2011-9-27
	m_pUserInfoDlg = NULL;
	m_pGameAffiche = NULL;
	m_pLanguageList = NULL;
	m_pTaskFinishFlash = NULL;
	m_pGameFrameBorder = NULL;
	m_pNewGiftFlash = NULL;	//新手礼包	Author<cxf>
	//配置变量
	m_bAllowSound = m_bEnableBGMusic = true;
	m_cbSoundVol = m_cbBGMusicVol = 100;

	m_pGameFrameView = NULL;
	if (NULL != pGameFrameView)
	{
		m_pGameFrameView=pGameFrameView;
	}

	m_pGameScoreView = NULL;
	if (NULL != pScoreView)
	{
		m_pGameScoreView = pScoreView;
	}

	//配置变量
	m_bAllowSound=true;

	//创建钩子
	m_pKernelSink=new CClientKernelSink(*this);


	//m_bIsDisableGameMsg = false;
	//m_rcGameMsgInfo.SetRect(0, 0, 0, 0);
	//m_vecGameMsg.clear();
	//m_iCountVecIndex = 0;

	m_strLanguage.clear();
	return;
}

//析构函数
CGameFrameDlg::~CGameFrameDlg()
{
	SafeDelete(m_pKernelSink);
	SafeDelete(m_pGameFrameControl);
	SafeDelete(m_pGameUserDaoJu);
	//SafeDelete(m_pSendMsgDlg);
	SafeDelete(m_pGameScoreView);
	SafeDelete(m_pGameTaskDlg);
	SafeDelete(m_pGameGiveDJInfo);
	SafeDelete(m_pGameUserAction);
	SafeDelete(m_pShowTimeDJList);
	//SafeDelete(m_pShowUserActionInfo);
	SafeDelete(m_pTaskFinishFlash);
	SafeDelete(m_pGameFrameBorder);
	SafeDelete(m_pShowDJInfo);
	SafeDelete(m_pNewGiftFlash);//新手礼包 Author<cxf>
	return;
}

//按钮消息
bool CGameFrameDlg::OnSplitterButton(CSkinSplitter * pSkinSplitter, CSkinButton * pSplitterButton)
{
	return true;
}

//拆分条消息
bool CGameFrameDlg::OnSplitterEvent(CSkinSplitter * pSkinSplitter, UINT uSplitID, int nXPos, int nYPos)
{
	return true;

	//调整界面
	CRect rcClient;
	GetClientRect(&rcClient);
	RectifyControl(rcClient.Width(),rcClient.Height());

	return true;
}


//标题栏资源加载
void CGameFrameDlg::LoadTitleResource(void)
{
	TCHAR szResourcePath[MAX_PATH];
	GetCurrentDirectory(MAX_PATH,szResourcePath);
	CString strPath(szResourcePath);
	strPath.Append("\\data\\gameframe\\plaza\\");

	//加载资源
	m_FrameImageBuffer.Destroy();
	m_FrameImageBuffer.Create(960, 29, 24);

	HINSTANCE hInstance=GetModuleHandle(GAME_FRAME_DLL_NAME);
	m_ImageTitleBack.LoadImage(TEXT(strPath + "titleback.png"));
	m_ImageTitleText.LoadImage(TEXT(strPath + "ImageGameTitle.png"));
	m_ImageTitleNum.LoadImage(TEXT(strPath + "TitleTextNum.png"));

	//创建控件
	CRect CreateRect(0,0,0,0);

	//创建按钮
	m_btTitleMinCtBt.Create(TEXT(""),WS_CHILD|WS_VISIBLE,CreateRect,this,IDC_BT_TITLE_MIN);
	m_btTitleSoundBt.Create(TEXT(""),WS_CHILD|WS_VISIBLE,CreateRect,this,IDC_BT_TITLE_SOUND);
	m_btTitleWndCloseBt.Create(TEXT(""), WS_CHILD|WS_VISIBLE, CreateRect, this, IDC_BT_TITLE_CLOSE);
	m_btTitlePhoto.Create(TEXT(""), WS_CHILD|WS_VISIBLE, CreateRect, this, IDC_BT_TITLE_GAMEPHOTO);
	m_btTitleHome.Create(TEXT(""),WS_CHILD|WS_VISIBLE,CreateRect,this,IDC_BT_TITLE_GAMEHOME);
	m_btTitleShop.Create(TEXT(""), WS_CHILD|WS_VISIBLE, CreateRect, this, IDC_BT_TITLE_GAMESHOP);
	m_btTitleServer.Create(TEXT(""), WS_CHILD|WS_VISIBLE, CreateRect, this, IDC_BT_TITLE_GAMESERVER);

	//设置按钮
	m_btTitleMinCtBt.SetButtonImage(TEXT(strPath + "min.png"),false, 4);
	m_btTitleWndCloseBt.SetButtonImage( TEXT(strPath + "close.png"),false, 4);
	m_btTitlePhoto.SetButtonImage(TEXT(strPath + "jtimage.png"), false, 4);
	m_btTitleSoundBt.SetButtonImage(TEXT(strPath + "sound.png"), false, 4);
	m_btTitleHome.SetButtonImage( TEXT(strPath + "Home.png"),false, 4);
	m_btTitleShop.SetButtonImage(TEXT(strPath + "shop.png"), false, 4);
	m_btTitleServer.SetButtonImage(TEXT(strPath + "server.png"), false, 4);

	m_btTitleSoundBt.EnableWindow(FALSE);
}

//按钮单击事件
void CGameFrameDlg::OnBnMinClickedEvent()
{//最小化
	this->ShowWindow(SW_MINIMIZE);
}

void CGameFrameDlg::OnBnSoundClickedEvent()
{//开启声音
	CGameSoundSet soundset;
	soundset.SetGameFrameDlgPtr(this);
	soundset.DoModal();

	//CGameSoundSet * pDlg = new CGameSoundSet(_TEXT("F:\\game\\server\\运行\\release\\data\\gameframe\\plaza\\framdase.png"), this);
	//pDlg->Create( IDD_DIALOG_GAMESET, this);
	//pDlg->ShowWindow(SW_SHOW);
	
}

void CGameFrameDlg::OnBnCloseClieckedEvent()
{//关闭
	OnCancel();
}

void CGameFrameDlg::OnBnGamePhotoClieckedEvent()
{
	//游戏截图
	SaveScreenToBmp();
}



//重画消息
void CGameFrameDlg::OnPaint()
{
	CPaintDC dc(this);

	CRect rect;
	GetClientRect(&rect);
	CMemDC dcMem(&dc, rect, true);	// 用单色填充背景

	dcMem.SetBkMode(TRANSPARENT);
	dcMem.FillSolidRect(0,0,GAME_FRAME_DLG_WIDTH, GAME_FRAME_DLG_HEIGHT,RGB(0,0,0));

	//创建字体
	CFont DrawFont;
	DrawFont.CreateFont(-13,0,0,0,400,0,0,0,134,3,2,1,2,TEXT("黑体"));
	CFont * pOldFont=dcMem.SelectObject(&DrawFont);

	//绘画背景图片
	if (!m_ImageTitleBack.IsNull())
	{
		m_ImageTitleBack.DrawImage(&dcMem, 0, 0, m_ImageTitleBack.GetWidth(), m_ImageTitleBack.GetHeight(), 0, 0);
	}

	CPoint ptText;
	ptText.x = 20;
	ptText.y = 5;
	if (!m_ImageTitleText.IsNull())
	{
		//绘制标题
		const tagServerAttribute * pServerAttribute=m_ClientKernelHelper->GetServerAttribute();
		if (NULL != pServerAttribute)
		{
			if (10 == pServerAttribute->wKindID)
			{
				//斗地主
				m_ImageTitleText.DrawImage(&dcMem, ptText.x, ptText.y, 144, 20, 11, 0, 144, 20);
				ptText.x += 144;
			}
			else if (310 == pServerAttribute->wKindID)
			{
				//益阳麻将
				m_ImageTitleText.DrawImage(&dcMem, ptText.x, ptText.y, 156, 20, 168, 0, 156, 20);
				ptText.x += 156;
			}
			else if (303 == pServerAttribute->wKindID)
			{
				//长沙麻将
				m_ImageTitleText.DrawImage(&dcMem, ptText.x, ptText.y, 156, 20, 331, 0, 156, 20);
				ptText.x += 156;
			}
			
			if (1 == pServerAttribute->wSortID)
			{
				//新手房
				m_ImageTitleText.DrawImage(&dcMem, ptText.x, ptText.y, 50, 20, 546, 0, 50, 20);
			}
			else if (2 == pServerAttribute->wSortID)
			{
				//初级房
				m_ImageTitleText.DrawImage(&dcMem, ptText.x, ptText.y, 50, 20, 708, 0, 50, 20);
			}
			else if (3 == pServerAttribute->wSortID)
			{
				//中级房
				m_ImageTitleText.DrawImage(&dcMem, ptText.x, ptText.y, 50, 20, 872, 0, 50, 20);
			}
			else if (4 == pServerAttribute->wSortID)
			{
				//高级房
				m_ImageTitleText.DrawImage(&dcMem, ptText.x, ptText.y, 50, 20, 1035, 0, 50, 20);
			}

			ptText.x += 50;
			if (!m_ImageTitleNum.IsNull() && pServerAttribute->lCellScore>0)
			{
				int iNums[16] = {-1};
				int iCount = 0;
				LONG lMoney = pServerAttribute->lCellScore; 
				if (0==lMoney)
				{
					iCount = 1;
					iNums[0] = 0;
				}
				else
				{
					while(lMoney>0&&iCount<16)
					{
						iNums[iCount++] = lMoney%10;
						lMoney = lMoney/10;
					}
				}
				for (int i=iCount-1; i>=0; --i)
				{
					if (iNums[i]>=0)
					{
						m_ImageTitleNum.DrawImage(&dcMem, ptText.x, ptText.y+2, 14, 16, iNums[i]*14, 0, 14, 16);
						ptText.x += 10;
					}
				}
				m_ImageTitleText.DrawImage(&dcMem, ptText.x+2, ptText.y, 20, 20, 1215, 0, 20, 20);
			}
		}
		
		
		CImageNums		m_ClassNum;			//等级
	}

	DrawFont.DeleteObject();
}

//调整界面
void CGameFrameDlg::RectifyControl(int nWidth, int nHeight)
{
	if (nWidth<960 || nHeight<720) return;

	//状态判断
	if (m_bInitDialog==false) return;

	//变量定义
	//CRect rcVorSplitter;
	//const int nSplitterWidth=6;
	//const int iXExc=GetXExcursionPos();
	//const int iYExc=GetYExcursionPos();
	//const UINT uFlags=SWP_NOACTIVATE|SWP_NOZORDER;

	////获取位置
	//m_VorSplitter.GetWindowRect(&rcVorSplitter);
	//ScreenToClient(&rcVorSplitter);

	////调整拆分条
	//if (m_bRectify==false)
	//{
	//	//设置变量
	//	m_bRectify=true;

	//	//设置位置
	//	if (m_bHideList==false)
	//	{
	//		rcVorSplitter.left=nWidth*74/100;
	//		rcVorSplitter.right=rcVorSplitter.left+nSplitterWidth;
	//	}
	//	else
	//	{
	//		rcVorSplitter.right=nWidth;
	//		rcVorSplitter.left=nWidth-nSplitterWidth;
	//	}
	//}

	////设置拆分范围
	//if (m_bHideList==false)
	//{
	//	int nLessPos=nWidth*3/5,nMaxPos=nWidth*74/100-2;
	//	m_VorSplitter.SetSplitterRange(nLessPos,nMaxPos);
	//	if ((nWidth!=0)&&(nHeight!=0))
	//	{
	//		if (rcVorSplitter.left<nLessPos) 
	//		{
	//			rcVorSplitter.left=nLessPos;
	//			rcVorSplitter.right=rcVorSplitter.left+nSplitterWidth;
	//		}
	//		if (rcVorSplitter.right>nMaxPos)
	//		{
	//			rcVorSplitter.right=nMaxPos;
	//			rcVorSplitter.left=rcVorSplitter.right-nSplitterWidth;
	//		}
	//	}
	//}

	//if (m_VorSplitter.GetSplitterType()==HorSpliter) rcVorSplitter.SetRect(nWidth,0,nWidth,nHeight);

	const UINT uFlags=SWP_NOACTIVATE|SWP_NOZORDER|SWP_NOCOPYBITS|SWP_NOSIZE;

	RECT rect;
	GetClientRect(&rect);
	RECT wndrect;
	GetWindowRect(&wndrect);

	//锁定屏幕
	LockWindowUpdate();

	if (NULL != m_pGameFrameView)
	{//游戏视图
		m_pGameFrameView->SetWindowPos(NULL,0,29,960,720,0);
	}


	if (NULL != m_pGameScoreView)
	{//结算视图
		CRect scorerect;
		scorerect.left = wndrect.left+((rect.right-rect.left)-m_pGameScoreView->m_szWndSize.cx)/2;
		//scorerect.right = scorerect.left + m_pGameScoreView->m_szWndSize.cx;
		scorerect.top = wndrect.top + ((rect.bottom-rect.top)-m_pGameScoreView->m_szWndSize.cy)/2-50;
		//scorerect.bottom = scorerect.top + m_pGameScoreView->m_szWndSize.cy;
		m_pGameScoreView->SetWindowPos(NULL,scorerect.left, scorerect.top, m_pGameScoreView->m_szWndSize.cx, m_pGameScoreView->m_szWndSize.cy, 0);
	}

	if (NULL != m_pGameFrameControl)
	{
		CRect rc;
		m_pGameFrameControl->GetClientRect(&rc);
		m_pGameFrameControl->SetWindowPos(&wndTop, nWidth-rc.Width()-18, nHeight-rc.Height()-22, 0, 0, SWP_NOSIZE);
	}

	//if (NULL != m_pGameFrameControl)
	//{//聊天框控件
	//	CRect rcFrameControl(rect.right-213,rect.bottom-133,rect.right,rect.bottom);
	//	bool bIsUp = m_pGameFrameControl->GetWndIsUp();
	//	if (true == bIsUp)
	//	{//向上(显示状态)
	//		//DeferWindowPos(hDwp,m_pGameFrameControl->GetSafeHwnd(),NULL,rect.right-249,rect.bottom-137,rect.right,rect.bottom,0);
	//	}
	//	else
	//	{//向下(隐藏状态)
	//		rcFrameControl.top += 89;
	//		rcFrameControl.bottom += 89;
	//	}
	//	m_pGameFrameControl->SetWindowPos(NULL,rcFrameControl.left,rcFrameControl.top,rcFrameControl.right,rcFrameControl.bottom,0);
	//}
	if (NULL != m_pGameTaskDlg)
	{//任务框
		m_pGameTaskDlg->SetWindowPos(&wndTop,rect.right-220+10,rect.bottom-204-10, 0, 0, SWP_NOSIZE);
	}
	//if (NULL != m_pSendMsgDlg)
	//{//聊天输入窗口
	//	//m_pSendMsgDlg->SetWindowPos(NULL,wndrect.left+rect.right-213,wndrect.top+rect.bottom-34,213,34,0);
	//	m_pSendMsgDlg->SetWindowPos(NULL,rect.right-213,rect.bottom-34,213,34,0);
	//	m_pSendMsgDlg->ShowWindow(SW_SHOW);
	//}
	//if (NULL != m_pShowTimeDJList)
	//{//时间道具列表窗口
	//	CSize wndsize = m_pShowTimeDJList->GetDlgWH();
	//	m_pShowTimeDJList->SetWindowPos(NULL, 380, rect.bottom-40-wndsize.cy, wndsize.cx, wndsize.cy, 0);
	//}
	if(NULL != m_pShowDJInfo)
	{//显示道具信息窗口
		m_pShowDJInfo->SetWindowPos(NULL, 260, rect.bottom-128, 178, 98, 0);
	}
	//if (NULL != m_pGameGiveDJInfo)
	//{//道具赠送信息窗口
	//	m_pGameGiveDJInfo->SetWindowPos(NULL, 350, rect.bottom-93, 178, 63, 0);
	//}
	//if (NULL != m_pGameBizCity)
	//{//商城窗口大小
	//	m_pGameBizCity->SetWindowPos(NULL, wndrect.left, wndrect.top, nWidth, nHeight, 0);
	//}
	//if (NULL != m_pGameUserAction)
	//{//用户动作窗口
	//	m_pGameUserAction->SetWindowPos(NULL, wndrect.left+rect.right-342, wndrect.top+rect.bottom-150-30, 342, 150, 0);
	//}
	//if (NULL != m_pShowUserActionInfo)
	//{//创建用户动作信息显示窗口
	//	m_pShowUserActionInfo->SetWindowPos(NULL, (nWidth-245)/2, (nHeight-94)/2-30, 245, 94, 0);
	//}
	if (NULL != m_pUserInfoDlg)
	{//创建用户信息显示窗口
		m_pUserInfoDlg->SetWindowPos(&wndTop, -1, nHeight-111, 0, 0, SWP_NOSIZE);
		SafeShowOrHideWnd(m_pUserInfoDlg, SW_SHOW);
	}
	if (NULL != m_pGameAffiche)
	{//创建喜报系统
		bool bIsExp = m_pGameAffiche->GetIsExp();
		if (bIsExp)
		{
			m_pGameAffiche->SetWindowPos(NULL, wndrect.left+6,wndrect.top+29,948,20, 0);
		}
		else
		{
			m_pGameAffiche->SetWindowPos(NULL, 6,29, 948,20, 0);
		}
	}
	//if (NULL != m_pLanguageList)
	//{//语音列表
	//	m_pLanguageList->SetWindowPos(NULL, wndrect.left+566, wndrect.top+410, 180, 258, 0);
	//}
	if (NULL != m_pTaskFinishFlash)
	{
		m_pTaskFinishFlash->SetWindowPos(NULL, wndrect.left+(nWidth-256)/2, wndrect.top, 0, 0, SWP_NOSIZE);
	}

	if (NULL != m_pGameFrameBorder)
	{
		m_pGameFrameBorder->SetWindowPos(NULL, 0, 0, nWidth, nHeight, SWP_SHOWWINDOW);
	}

	int iLeft = 430;
	int iLeftValue = 2;
	int iTop = 6;
	m_btTitleHome.SetWindowPos(NULL,iLeft,iTop,0,0,uFlags);
	m_btTitleShop.SetWindowPos(NULL,iLeft+75+iLeftValue,iTop,0,0,uFlags);
	m_btTitleServer.SetWindowPos(NULL,iLeft+2*75+2*iLeftValue,iTop,0,0,uFlags);
	m_btTitlePhoto.SetWindowPos(NULL,iLeft+3*75+3*iLeftValue,iTop,0,0,uFlags);

	m_btTitleSoundBt.SetWindowPos(NULL,rect.right-172,iTop-1,0,0,uFlags);
	m_btTitleMinCtBt.SetWindowPos(NULL,rect.right-119,iTop-1,0,0,uFlags);
	m_btTitleWndCloseBt.SetWindowPos(NULL,rect.right-67,iTop-1,0,0,uFlags);

	
	//重画界面
	Invalidate(FALSE);

	//解除锁定
	UnlockWindowUpdate();


	/*
*/
	//DeferWindowPos(hDwp,m_VorSplitter,NULL,rcVorSplitter.left,iYExc,rcVorSplitter.Width(),nHeight-iYExc,uFlags);
	//DeferWindowPos(hDwp,m_pGameFrameControl->GetSafeHwnd(),NULL,rect.right-249,rect.bottom-137,rect.right,rect.bottom,0);

	//
	//DeferWindowPos(hDwp,m_pSendMsgDlg->GetSafeHwnd(),NULL,rect.right-240,rect.bottom-30,240,30,0);

	//DeferWindowPos(hDwp,m_btTitleMinCtBt,NULL,rect.right-2*24-iRightTopValue,5,0,0,uFlags);
	//DeferWindowPos(hDwp,m_btTitleSoundBt,NULL,rect.right-24-iRightTopValue,5,0,0,uFlags);
	//DeferWindowPos(hDwp,m_btTitleWndCloseBt,NULL,rect.right-iRightTopValue,5,0,0,uFlags);

	
	//DeferWindowPos(hDwp, m_pGameUserDaoJu->GetSafeHwnd(), NULL, 0, rect.bottom-30, 721, rect.bottom, 0);

	

	//控制按钮
	//CRect rcControl;
	//m_btControl.GetClientRect(&rcControl);
	//m_btControl.SetWindowPos(NULL,rcVorSplitter.left+(rcVorSplitter.Width()-rcControl.Width())/2,(nHeight-rcControl.Height())/2,0,0,uFlags|SWP_NOSIZE);
	
	//m_pGameFrameControl->ShowWindow(SW_SHOW);
	//m_pGameUserDaoJu->ShowWindow(SW_SHOW);
	//m_pSendMsgDlg->ShowWindow(SW_SHOW);

	m_VorSplitter.ShowWindow(SW_HIDE);
	m_btControl.ShowWindow(SW_HIDE);

	m_btTitleMinCtBt.ShowWindow(SW_SHOW);
	m_btTitleSoundBt.ShowWindow(SW_SHOW);
	m_btTitleWndCloseBt.ShowWindow(SW_SHOW);


	//m_btTitleMinCtBt.ShowWindow(SW_SHOW);
	//m_btTitleWndCloseBt.ShowWindow(SW_SHOW);
}


//网络消息
bool CGameFrameDlg::OnFrameMessage(WORD wSubCmdID, const void * pBuffer, WORD wDataSize)
{
	return false;
}

//用户进入
void __cdecl CGameFrameDlg::OnEventUserEnter(tagUserData * pUserData, WORD wChairID, bool bLookonUser)
{
	if (NULL != m_pUserInfoDlg)
	{
		BYTE byChairID = GetMeChairID();
		if (INVALID_CHAIR!=byChairID && wChairID==byChairID)
		{
			if (IsLookonMode())
			{
				if (bLookonUser)
				{
					m_pUserInfoDlg->SetUserData(pUserData);
				}
			}
			else if (!bLookonUser)
			{
				m_pUserInfoDlg->SetUserData(pUserData);
			}

			
			SafeEnableWnd(&m_btTitleSoundBt, TRUE);
			if (!IsLookonMode())
			{//不是旁观玩家
				m_pUserInfoDlg->EnableBTWindow();
			}

			if (NULL != m_pGameFrameView)
				m_pGameFrameView->ResetWinLost(pUserData);//Author<cxf>;DateTime<2012-02-20>;Description<绘画赢取局数>;
		}

		//设置游戏用户数
		if (NULL != m_pGameFrameView)
		{
			m_pGameFrameView->SetGameUserNums(GetGameDeskChairNums());
		}
	}
	return;
}

//用户离开
void __cdecl CGameFrameDlg::OnEventUserLeave(tagUserData * pUserData, WORD wChairID, bool bLookonUser)
{
	return;
}

//用户积分
void __cdecl CGameFrameDlg::OnEventUserScore(tagUserData * pUserData, WORD wChairID, bool bLookonUser)
{
	if (NULL != m_pUserInfoDlg)
	{
		BYTE byChairID = GetMeChairID();
		if (INVALID_CHAIR!=byChairID && wChairID==byChairID)
		{
			m_pUserInfoDlg->SetUserData(pUserData);
			m_pGameFrameView->ResetWinLost(pUserData);//Author<cxf>;DateTime<2012-08-20>;Description<绘画赢取局数>;
		}
	}
	return;
}

//用户状态
void __cdecl CGameFrameDlg::OnEventUserStatus(tagUserData * pUserData, WORD wChairID, bool bLookonUser)
{
	return;
}

//初始道具
void __cdecl CGameFrameDlg::OnEventInitProperty(tagPropertyInfo *pPropertyInfo, int nInfoCount)
{
	//获取对象
	//CPropertyBar *pPropertyBar = CPropertyBar::GetInstance();
	//ASSERT( pPropertyBar != NULL );
	//if ( pPropertyBar == NULL ) return;

	////初始结构
	//for ( int nIndex = 0; nIndex < nInfoCount; ++nIndex )
	//{
	//	pPropertyBar->SetPropertyInfo((pPropertyInfo+nIndex)->nPropertyID, *(pPropertyInfo+nIndex));
	//}

	//osc update
	if (NULL != m_pUserDJInfo)
	{
		for (int i=0; i<nInfoCount; i++)
		{
			m_pUserDJInfo->AddGameDJInfo(&pPropertyInfo[i]);
		}
	}

#ifdef PRINT_GAME_CURS_MSG
	char chdata[100]={0};
	sprintf(chdata, "道具地址:%d,类型个数:%d", pPropertyInfo, nInfoCount);
	InsertShtickString(chdata, RGB(0, 255, 0), true);
#endif

	return;
}


//玩家剩余道具
void __cdecl CGameFrameDlg::OnEventUserHaveDJInfo(CMD_GF_ResidualProperty *pUserDJInfo)
{
	if (NULL == pUserDJInfo)
	{
		return;
	}

#ifdef PRINT_GAME_CURS_MSG
	int iCountNums = 0;
	for (int i=0; i<PROPERTY_COUNT; i++)
	{
		char chData[100]={0};
		sprintf(chData, "%i-%i-%i-%i", pUserDJInfo->dwResidual[i].dwID, pUserDJInfo->dwResidual[i].bUse, pUserDJInfo->dwResidual[i].dwResidualTime, pUserDJInfo->dwResidual[i].dwResidualTimeCount);
		InsertShtickString(chData, RGB(0, 255, 0), true);
		if (!pUserDJInfo->dwResidual[i].bUse&&0==pUserDJInfo->dwResidual[i].dwResidualTime)
		{
			continue;
		}
		++iCountNums;
	}
	char chNumsData[10]={0};
	sprintf(chNumsData, "共%d个道具", iCountNums);
	InsertShtickString(chNumsData, RGB(0, 255, 0), true);
#endif

	if (NULL != m_pUserDJInfo)
	{
		//添加玩家道具
		m_pUserDJInfo->AddUserDJList(pUserDJInfo);
	}


#ifdef PRINT_GAME_CURS_MSG
	char chSizeData[255]={0};
	sprintf(chSizeData, "初始化信息:道具框拥有道具数共:%d个,正在使用道具数共:%d个", m_pUserDJInfo->m_vecUserHaveDJList.size(), m_pUserDJInfo->m_vecUserUseDJList.size());
	InsertShtickString(chSizeData, RGB(255, 255, 0), true);
#endif

}


//道具操作信息
void __cdecl CGameFrameDlg::OnEventUserOperationDJInfo(BYTE byTag, void * pBuffer)
{
	if (0==byTag || NULL==pBuffer || NULL==m_pUserDJInfo)
	{
		return;
	}
	
	CMD_GF_UserProperty* pUserProperty = (CMD_GF_UserProperty*)pBuffer;
	if (NULL != pUserProperty)
	{

#ifdef PRINT_GAME_CURS_MSG
		char chDataTip[255]={0};
		sprintf(chDataTip, "%d-%d-%d-%d", pUserProperty->dwID, pUserProperty->bUse, pUserProperty->dwResidualTime, pUserProperty->dwResidualTimeCount);
		InsertShtickString(chDataTip, RGB(255, 255, 0), true);
#endif

		switch(byTag)
		{
		case 1:		//道具激活成功
			{
				char chTipInfo[255]={0};
				if (pUserProperty->dwID>=PROP_FSK && pUserProperty->dwID<=PROP_GOODLUCK)
				{
					if (pUserProperty->dwResidualTime>=0)
					{//拥有剩余道具数
						m_pUserDJInfo->SetDJHaveProperty(pUserProperty);
					}
					if (pUserProperty->bUse)
					{//更新正在使用的道具
						m_pUserDJInfo->SetDJUseProperty(pUserProperty);

						//系统提示
						sprintf(chTipInfo, "恭喜你使用了%s(%d)道具，还剩余%d个道具！", g_PropertyDescribe[pUserProperty->dwID].szName, pUserProperty->dwID, pUserProperty->dwResidualTime);
						InsertSystemString(chTipInfo);
					}
				}
				else
				{
					sprintf(chTipInfo, "错误：激活道具ID(%d)有误！", pUserProperty->dwID);
					InsertSystemString(chTipInfo);
				}
			}
			break;
		case 2:		//没有道具,激活失败
			{
				//系统提示
				char chTipInfo[100]={0};
				sprintf(chTipInfo, "对不起！您没有此道具(%d)，建议从商城购买！", pUserProperty->dwID);
				InsertSystemString(chTipInfo);

				m_pUserDJInfo->DelUserAllDJID(pUserProperty->dwID);
			}
			break;
		case 3:		//道具还在使用，激活失败
			{
				char chTipInfo[255]={0};
				if (pUserProperty->dwID>=PROP_FSK && pUserProperty->dwID<=PROP_GOODLUCK)
				{
					if (pUserProperty->dwResidualTime>=0)
					{//拥有剩余道具数
						m_pUserDJInfo->SetDJHaveProperty(pUserProperty);
					}
					if (pUserProperty->bUse)
					{//更新正在使用的道具
						m_pUserDJInfo->SetDJUseProperty(pUserProperty);
					}
					//系统提示
					sprintf(chTipInfo, "你已经正在使用%s(%d)道具，还剩余%d个道具，请稍后再试！", g_PropertyDescribe[pUserProperty->dwID].szName, pUserProperty->dwID, pUserProperty->dwResidualTime);
					InsertSystemString(chTipInfo);
				}
				else
				{
					sprintf(chTipInfo, "错误：激活道具ID(%d)有误！", pUserProperty->dwID);
					InsertSystemString(chTipInfo);
				}
			}
			break;
		case 4:		//目前的道具已经过期了,use为false,代表正在使用的道具无效.num表示剩余道具数.
			{
				char chTipInfo[255]={0};
				if (pUserProperty->dwID>=PROP_FSK && pUserProperty->dwID<=PROP_GOODLUCK)
				{
					if (pUserProperty->dwResidualTime>=0)
					{//拥有剩余道具数
						m_pUserDJInfo->SetDJHaveProperty(pUserProperty);
					}
					m_pUserDJInfo->SetDJUseProperty(pUserProperty);
					
					//系统提示
					sprintf(chTipInfo, "你使用%s(%d)道具已过期！还剩余%d个道具！", g_PropertyDescribe[pUserProperty->dwID].szName, pUserProperty->dwID, pUserProperty->dwResidualTime);
					InsertSystemString(chTipInfo);
				}
				else
				{
					sprintf(chTipInfo, "错误：道具ID(%d)有误！", pUserProperty->dwID);
					InsertSystemString(chTipInfo);
				}
			}
			break;
		case 5:		//道具信息更新,道具奖励.
			{
				char chTipInfo[255]={0};
				if (pUserProperty->dwID>=PROP_FSK && pUserProperty->dwID<=PROP_GOODLUCK)
				{
					if (pUserProperty->dwResidualTime>=0)
					{//拥有剩余道具数
						m_pUserDJInfo->SetDJHaveProperty(pUserProperty);
					}
					if (pUserProperty->bUse)
					{//更新正在使用的道具
						m_pUserDJInfo->SetDJUseProperty(pUserProperty);
					}
					//系统提示
					sprintf(chTipInfo, "你的%s(%d)道具已更新,还剩余%d个道具！", g_PropertyDescribe[pUserProperty->dwID].szName, pUserProperty->dwID, pUserProperty->dwResidualTime);
					InsertSystemString(chTipInfo);
				}
				else
				{
					sprintf(chTipInfo, "错误：激活道具ID(%d)有误！", pUserProperty->dwID);
					InsertSystemString(chTipInfo);
				}
			}
			break;
		default:
			break;
		}
	}

#ifdef PRINT_GAME_CURS_MSG
	char chSizeData[255]={0};
	sprintf(chSizeData, "当前信息:道具框拥有道具数共:%d个,正在使用道具数共:%d个", m_pUserDJInfo->m_vecUserHaveDJList.size(), m_pUserDJInfo->m_vecUserUseDJList.size());
	InsertShtickString(chSizeData, RGB(255, 255, 0), true);
#endif

}


//道具赠送消息
void __cdecl CGameFrameDlg::OnEventGameDJInfo(const char* pInfo)
{
	return;
	//if (NULL == pInfo)
	//{
	//	return;
	//}
	//if (NULL != m_pGameGiveDJInfo && !IsLookonMode())
	//{
	//	InsertSystemString(pInfo);
	//	m_pGameGiveDJInfo->SetGiveDJInfo(pInfo);
	//}
}


//更新道具信息
void CGameFrameDlg::UpdateDJInfo(int iDJID)
{
	IPC_UserUpdateDJInfo djinfo;
	djinfo.iDJID = iDJID;
	m_ClientKernelHelper->SendProcessData(IPC_MAIN_CONCTROL, IPC_SUB_USER_UPDATE_DJINFO, &djinfo, sizeof(IPC_UserUpdateDJInfo));
}


//用户信息
void __cdecl CGameFrameDlg::OnEventUserMD5PWMsg(char* chUserData)
{
	return;
	//if ((NULL != m_pGameBizCity) && (NULL != chUserData))
	//{
	//	WORD wChairNums = GetGameDeskChairNums();
	//	WORD wChairID = GetMeChairID();
	//	if ((0 != wChairNums) && (wChairID>=0 && wChairID<wChairNums))
	//	{
	//		const tagUserData* pUserData = GetUserData(wChairID);
	//		if (NULL != pUserData)
	//		{
	//			m_pGameBizCity->SetGameBizCity(pUserData->dwUserID, chUserData);
	//		}
	//	}
	//}
}

//公告信息
void __cdecl CGameFrameDlg::OnEventGameAfficheInfo(const char* pData, WORD wDataSize)
{
	if (NULL == pData || 0==wDataSize)
	{
		return;
	}

	if (NULL != m_pGameAffiche)
	{
		m_pGameAffiche->SetPlay(false);
		SafeShowOrHideWnd(m_pGameAffiche, SW_SHOW);

		bool bIsExp = m_pGameAffiche->GetIsExp();
		if (bIsExp)
		{
			RECT wndrect;
			GetWindowRect(&wndrect);
			m_pGameAffiche->SetWindowPos(NULL, wndrect.left+6,wndrect.top+29,0,0, SWP_NOSIZE);
		}
		else
		{
			m_pGameAffiche->SetWindowPos(NULL, 6,29, 948,20, 0);
		}

		//m_pGameAffiche->ShowWindow(SW_SHOW);
		m_pGameAffiche->SetGameAfficheInfo(pData);
		m_pGameAffiche->SetPlay(true, 3);
	}
	return;


	//KillTimer(GAME_MSG_TIME_ID);
	//InsertSystemString(pData);
	//
	//m_iCountVecIndex = 0;
	////解析公告
	//ParseChar(pData, wDataSize);
	////SetTimer(GAME_MSG_TIME_ID,GAME_MSG_TIME_INTERVAL,NULL);

	//DWORD wDataWidth = 0;
	//CDC *pDC = GetDC();
	//if (NULL != pDC)
	//{
	//	wDataWidth = pDC->GetTextExtent(m_vecGameMsg[0].c_str()).cx;
	//	m_rcGameMsgInfo.left = 960-GAMEINFO_L;
	//	m_rcGameMsgInfo.top = 0;
	//	m_rcGameMsgInfo.right = m_rcGameMsgInfo.left+wDataWidth;
	//	m_rcGameMsgInfo.bottom = 22;
	//}
	//ReleaseDC(pDC);

	////激活走马灯
	//if (m_vecGameMsg.size() > 0)
	//{
	//	m_bIsDisableGameMsg = true;
	//	if ((m_vecGameMsg.size() > 1) || (m_rcGameMsgInfo.Width()>960-GAMEINFO_L-GAMEINFO_R))
	//	{
	//		m_rcGameMsgInfo.left -= 400;
	//		m_rcGameMsgInfo.right -= 400;
	//		SetTimer(GAME_MSG_TIME_ID,GAME_MSG_TIME_INTERVAL,NULL);
	//	}
	//	else
	//	{
	//		//暂时不用走马灯
	//		m_rcGameMsgInfo.left = GAMEINFO_L+(960-GAMEINFO_L-GAMEINFO_R-wDataWidth)/2;
	//		m_rcGameMsgInfo.right = m_rcGameMsgInfo.left+wDataWidth;
	//	}
	//}
	//InvalidateRect(m_rcGameMsgInfo);
}


//解析公告
void CGameFrameDlg::ParseChar(const char* pData, WORD wDataSize)
{
	//if (NULL == pData || 0==wDataSize)
	//{
	//	return;
	//}
	//m_vecGameMsg.clear();

	//WORD iCountIndex = 0;
	//char* pNewData = new char[wDataSize];
	//if (NULL == pNewData)
	//{
	//	return;
	//}
	//::memset(pNewData, 0, wDataSize);
	//const char* pOldData = pData;
	//while ((*pData != '\0') && (iCountIndex<wDataSize))
	//{
	//	if ('|'== *pData)
	//	{
	//		if (('*' == *(pData+1))
	//			&& ('*' == *(pData+2))
	//			&& ('|' == *(pData+3)))
	//		{
	//			::memset(pNewData, 0, wDataSize);
	//			::memcpy(pNewData, pOldData, pData-pOldData);
	//			m_vecGameMsg.push_back(pNewData);
	//			pOldData = pData+4;
	//			pData = pData+3;
	//		}
	//	}
	//	pData++;
	//	++iCountIndex;
	//}
	//delete pNewData;
	//pNewData = NULL;
}


//用户动作
void __cdecl CGameFrameDlg::OnEventUserActionInfo(BYTE btSender, BYTE btRecver, BYTE btTag)
{
	//if (NULL != m_pShowUserActionInfo)
	//{
	//	BYTE byChairNums = GetGameDeskChairNums();
	//	if (255!=btSender && btSender>=byChairNums)
	//	{
	//		return;
	//	}

	//	char chSendName[64] = {0};
	//	char chRecvName[64] = {0};
	//	::sprintf(chSendName, "玩家");
	//	::sprintf(chRecvName, "玩家");
	//	const tagUserData* pSendUserData = GetUserData(btSender);
	//	if (NULL != pSendUserData)
	//	{
	//		::memcpy(chSendName, pSendUserData->szName, strlen(pSendUserData->szName));
	//	}
	//	
	//	if (255!=btRecver && btRecver<byChairNums)
	//	{
	//		const tagUserData* pRecvUserData = GetUserData(btRecver);
	//		if (NULL != pRecvUserData)
	//		{
	//			::memcpy(chRecvName, pRecvUserData->szName, strlen(pRecvUserData->szName));
	//		}
	//	}
	//	m_pShowUserActionInfo->SetPlayFlash(btTag, chSendName, chRecvName);
	//}
}

//发送动作
void CGameFrameDlg::SendUserActionMsg(BYTE btRecvChairID)
{
	//BYTE btMeChairID = GetMeChairID();
	//if (!IsLookonMode() \
	//	&& (NULL!=m_pGameUserAction) \
	//	&&(btRecvChairID!=btMeChairID))
	//{//非旁观玩家
	//	bool bIsSend = m_pGameUserAction->GetCurSendStatus();
	//	if (bIsSend)
	//	{//可以发送
	//		IPC_UserActionInfo actioninfo;
	//		actioninfo.dwSendUserID = btMeChairID;
	//		actioninfo.dwRecvUserID = btRecvChairID;
	//		actioninfo.dwActionTag = m_pGameUserAction->GetCurUserActionTag();
	//		if (255 != btRecvChairID)
	//		{
	//			const tagUserData* pUserData = GetUserData(btRecvChairID);
	//			if (NULL  == pUserData)
	//			{
	//				return;
	//			}
	//		}
	//		//发送消息
	//		m_ClientKernelHelper->SendProcessData(IPC_MAIN_CONCTROL,IPC_SUB_SENDACTIONINFO,&actioninfo,sizeof(IPC_UserActionInfo));
	//		m_pGameUserAction->SetResetData();
	//	}
	//}
}

//用户语音
void __cdecl CGameFrameDlg::OnEventUserLanguageInfo(BYTE btSender, DWORD dwIndex)
{
	const tagUserData* pSendUserData = GetUserData(btSender);
	if (NULL != pSendUserData)
	{
		BYTE byViewID = SwitchViewChairID(btSender);
		if (NULL != m_pLanguageList)
		{
			CString strName = "";
			CString strValue = "";
			m_pLanguageList->GetLanguage(dwIndex, strName, strValue);

			if (!strValue.IsEmpty())
			{
				//显示语音
				m_pGameFrameView->ShowUserSayInfo(byViewID, strValue);
			}

			if (!strName.IsEmpty())
			{
				//播放声音
				TCHAR szResourcePath[MAX_PATH]={0};
				GetCurrentDirectory(MAX_PATH,szResourcePath);
				CString strPath(szResourcePath);
				strPath.Append("\\data\\gameframe\\sound\\");
				if (1==pSendUserData->cbGender)
				{//女性
					strName.Append("_1");
				}
				else
				{//男性
					strName.Append("_0");
				}
				strName.Append(".mp3");
				strPath.Append(strName);
				PlayDirSound(strPath, true);	//播放游戏音效
			}
		}
	}
}

//初始鲜花
void __cdecl CGameFrameDlg::OnEventInitFlower(tagFlowerInfo *pFlowerInfo, int nInfoCount)
{
#if 0	// 屏蔽鲜花
	//获取对象
	CPropertyBar *pPropertyBar = CPropertyBar::GetInstance();
	ASSERT( pPropertyBar != NULL );
	if ( pPropertyBar == NULL ) return;

	//初始结构
	for ( int nIndex = 0; nIndex < nInfoCount; ++nIndex )
	{
		pPropertyBar->SetFlowerInfo((pFlowerInfo+nIndex)->nFlowerID, *(pFlowerInfo+nIndex));
	}
#endif

	return;
}

//鲜花消息
void __cdecl CGameFrameDlg::OnEventFlower(const tagUserData * pSendUserData, const tagUserData * pRecvUserData, UINT uFlowerID, UINT uFlowerEffectID)
{
	//界面显示
	m_pGameFrameView->ShowFlower(SwitchViewChairID(pSendUserData->wChairID), SwitchViewChairID(pRecvUserData->wChairID), uFlowerID, 
		uFlowerEffectID, m_ClientKernelHelper->GetMeChairID()==pRecvUserData->wChairID);

	return;
}

//控制列表
bool CGameFrameDlg::ShowViewControl(bool bShowControl)
{
	return true;

	if (bShowControl!=m_bHideList)
	{
		//设置变量
		m_bHideList=bShowControl;

		//获取位置
		CRect rcClient;
		GetClientRect(&rcClient);

		//调整界面
		RectifyControl(rcClient.Width(),rcClient.Height());
	}

	return true;
}

//显示任务信息
void CGameFrameDlg::SetTaskWndShow(bool bIsShow)
{
	if (NULL != m_pGameTaskDlg)
	{
		SafeShowOrHideWnd(m_pGameTaskDlg, bIsShow?SW_SHOW:SW_HIDE);
		//m_pGameTaskDlg->ShowWindow(bIsShow?SW_SHOW:SW_HIDE);
		m_pGameFrameView->UpdateGameView(NULL);
	}
}

//更新资源
bool CGameFrameDlg::UpdateSkinResource()
{
	return true;

	//获取资源
	HINSTANCE hInstance=g_GlobalOption.m_PlatformResourceModule->GetResInstance();
	tagGameFrameImage &GameFrameImage=g_GlobalOption.m_GameFrameImage;
	m_btControl.SetButtonImage(m_bHideList?GameFrameImage.uBtShowUserList:GameFrameImage.uBtHideUserList,hInstance,false);

	//控制按钮
	UINT uImageID=m_bAllowSound?GameFrameImage.uBtGameSound:GameFrameImage.uBtGameUnSound;
	m_pGameFrameControl->m_btSound.SetButtonImage(uImageID,hInstance,false);

	//控制按钮
	if (m_ClientKernelHelper.GetInterface()!=NULL) 
	{
		/*uImageID=m_ClientKernelHelper->IsAllowUserLookon()?GameFrameImage.uBtGameLookOn:GameFrameImage.uBtGameUnLookOn;*/
		uImageID=GameFrameImage.uBtGameLookOn;
		m_pGameFrameControl->m_btLookOn.SetButtonImage(uImageID,hInstance,false);
	}

	return true;
}

//发送函数
bool CGameFrameDlg::SendData(WORD wSubCmdID)
{
	ASSERT(m_ClientKernelHelper.GetInterface()!=NULL);
	return m_ClientKernelHelper->SendSocketData(MDM_GF_GAME,wSubCmdID);
}

//发送函数
bool CGameFrameDlg::SendData(WORD wSubCmdID, void * pData, WORD wDataSize)
{
	ASSERT(m_ClientKernelHelper.GetInterface()!=NULL);
	return m_ClientKernelHelper->SendSocketData(MDM_GF_GAME,wSubCmdID,pData,wDataSize);
}

//发送准备
bool CGameFrameDlg::SendUserReady(void * pBuffer, WORD wDataSize)
{
	ASSERT(m_ClientKernelHelper.GetInterface()!=NULL);
	return m_ClientKernelHelper->SendUserReady(pBuffer,wDataSize);
}

//设置定时器
bool CGameFrameDlg::SetGameTimer(WORD wChairID, UINT nTimerID, UINT nElapse)
{
	ASSERT(m_ClientKernelHelper.GetInterface()!=NULL);
	return m_ClientKernelHelper->SetGameTimer(wChairID,nTimerID,nElapse);
}

//删除定时器
bool CGameFrameDlg::KillGameTimer(UINT nTimerID)
{
	ASSERT(m_ClientKernelHelper.GetInterface()!=NULL);
	return m_ClientKernelHelper->KillGameTimer(nTimerID);
}

//播放声音
bool CGameFrameDlg::PlayGameSound(LPCTSTR pszSoundName)
{
	if (m_bAllowSound && GetSafeHwnd() && IsWindowVisible())
	{
		HINSTANCE hInstance=GetModuleHandle(GAME_FRAME_DLL_NAME);
		::PlaySound(pszSoundName,hInstance,SND_ASYNC|SND_FILENAME);
		return true;
	}
	
	return false;
}

//播放声音
bool CGameFrameDlg::PlayGameSound(HINSTANCE hInstance, LPCTSTR pszSoundName)
{
	if (m_bAllowSound && GetSafeHwnd() && IsWindowVisible())
	{
		//加载资源
		HRSRC hResour=FindResource(hInstance,pszSoundName,TEXT("WAVE"));
		if (hResour==NULL) return false;
		HGLOBAL hGlobal=LoadResource(hInstance,hResour);
		if (hGlobal==NULL) return false;

		//播放声音
		LPCTSTR pszMemory=(LPCTSTR)LockResource(hGlobal);
		::PlaySound(pszMemory,hInstance,SND_ASYNC|SND_MEMORY);

		//清理资源
		UnlockResource(hGlobal);
		FreeResource(hGlobal);
	}

	return true;
}


bool CGameFrameDlg::PlayGameSound(HINSTANCE hInstance, LPCTSTR pszSoundName, bool bWait)
{
	if (m_bAllowSound && GetSafeHwnd() && IsWindowVisible())
	{   //加载资源
		HRSRC hResour=FindResource(hInstance,pszSoundName,TEXT("WAVE"));
		if (hResour==NULL) return false;
		HGLOBAL hGlobal=LoadResource(hInstance,hResour);
		if (hGlobal==NULL) return false;
		//播放声音
		LPCTSTR pszMemory=(LPCTSTR)LockResource(hGlobal);
		if (bWait) ::PlaySound(pszMemory,hInstance,SND_SYNC|SND_MEMORY);
		else ::PlaySound(pszMemory,hInstance,SND_ASYNC|SND_MEMORY);
		//清理资源
		UnlockResource(hGlobal);
		FreeResource(hGlobal);
	}
	return true;
}

bool CGameFrameDlg::PlayDirSound(LPCTSTR szRes, bool bFromFile, bool bLoop)
{
	if (!m_bAllowSound)
	{
		//__ASSERT("CGameFrameDlg::PlayDirSound   m_bAllowSound == false");
		return true;
	}
	///////////////////////////
	if (bFromFile)
	{
		CString strTp = szRes;
		strTp = strTp.Mid(strTp.ReverseFind('.')+1);
		strTp.MakeLower();
		if (TEXT("mp3")== strTp)
		{
			if (!g_mciPlyr.PlaySounds(szRes))
			{
				//__ASSERT(szRes);
			}
			return true;
		}
	}
	///////////////////////////
	if (bFromFile) { if (!g_dirSound.CreateInFile((char *)szRes)) { /*MSGBOX("加载WAVE失败, (是否为PCM格式)");*/ return false; } }
	else if (!g_dirSound.Create(szRes)) { /*MSGBOX("加载WAVE失败, (资源名是否正确或是否为PCM格式)");*/ return false; }
	g_dirSound.SetWavVolume((m_cbSoundVol-100)*-(m_cbSoundVol-100));
	g_dirSound.Play(0, bLoop);
	return true;
}
bool CGameFrameDlg::PlayBGMusic(LPCTSTR szRes, BYTE cbType, bool bFromFile)
{
	switch (cbType)
	{
	case 0:
		{
			if (NULL != g_pdirBGMusic)
			{
				delete g_pdirBGMusic;
				g_pdirBGMusic = NULL;
			}
			if (bFromFile)            ///////////////////////////
			{
				CString strTp = szRes;
				strTp = strTp.Mid(strTp.ReverseFind('.')+1);
				strTp.MakeLower();
				if (TEXT("mp3")== strTp)
				{
					if (!g_mciPlyr.LoadMusic(szRes, m_bEnableBGMusic))
					{
						InsertGeneralString(szRes, 0, true);
					}
					return true;
				}
			}            ///////////////////////////
			g_pdirBGMusic = new CDirectSound;
			if (NULL == g_pdirBGMusic) return false;
			if (bFromFile) { if (!g_pdirBGMusic->CreateInFile((char *)szRes)) { /*MSGBOX("加载WAVE失败, (是否为PCM格式)");*/ return false; } }
			else if (!g_pdirBGMusic->Create(szRes)) {/* MSGBOX("加载WAVE失败, (资源名是否正确或是否为PCM格式)");*/ return false; }
			if (m_bEnableBGMusic)
			{
				g_pdirBGMusic->SetWavVolume((m_cbBGMusicVol-100)*-(m_cbBGMusicVol-100));
				g_pdirBGMusic->Play(0, TRUE);
			}
			break;
		}
	case 1:
		{
			if (NULL != g_pmdBGMusic) delete g_pmdBGMusic;
			g_pmdBGMusic = new CMIDI;
			g_pmdBGMusic->Create(szRes);
			if (m_bEnableBGMusic)
			{
				g_pmdBGMusic->Play(TRUE);
				g_pmdBGMusic->SetVolume(m_cbBGMusicVol);
			}
			break;
		}
	default:
		ASSERT(FALSE);
	}
	return true;
}
bool CGameFrameDlg::IsPlayingSound()
{
	if (g_mciPlyr.IsSoundPlaying()) return true;
	DWORD dwState = 0;
	g_dirSound.GetStatus(&dwState);
	if ((dwState & DSBSTATUS_PLAYING) == DSBSTATUS_PLAYING)
		return true;
	return false;
}

//声音状态
void CGameFrameDlg::SetSoundState(bool bIsEnable)
{
	if (bIsEnable)
	{//声音开启
		EnableSound(false);
		EnableBGMusic(false, 0);
		EnableBGMusic(false, 1);
	}
	else
	{//声音关闭
		EnableSound(true);
		EnableBGMusic(true, 0);
		EnableBGMusic(true, 1);
	}
}


//是否启用
bool CGameFrameDlg::IsEnableSound()
{
	return m_bAllowSound;
}

//启用声音
void CGameFrameDlg::EnableSound(bool bEnable)
{
	m_bAllowSound = bEnable;
	if (!m_bAllowSound) g_dirSound.Stop();

	CString strPath;
	GetModuleFileName(NULL, strPath.GetBufferSetLength(256), 256);
	strPath = strPath.Left(strPath.ReverseFind('\\')+1);
	strPath = strPath + TEXT("config\\Common.ini");
	WritePrivateProfileString(TEXT("Sound"), TEXT("EnableSound"),((true == bEnable)? TEXT("1"): TEXT("0")), strPath);

	g_mciPlyr.StopAllSounds();
}


bool CGameFrameDlg::StopBGMusic()
{
	if (!m_bEnableBGMusic) return true;
	if (g_mciPlyr.m_bMusicPlaying) g_mciPlyr.StopMusic(false);
	if (NULL != g_pdirBGMusic) g_pdirBGMusic->Stop();
	if (NULL != g_pmdBGMusic) g_pmdBGMusic->Stop(FALSE);
	return true;
}

void CGameFrameDlg::EnableBGMusic(bool bEnable, BYTE cbType)
{
	m_bEnableBGMusic = bEnable;

	CString strPath;
	GetModuleFileName(NULL, strPath.GetBufferSetLength(256), 256);
	strPath = strPath.Left(strPath.ReverseFind('\\')+1);
	strPath = strPath + TEXT("config\\Common.ini");
	WritePrivateProfileString(TEXT("Sound"), TEXT("EnableBGMusic"),((true == bEnable)? TEXT("1"): TEXT("0")), strPath);

	switch (cbType)
	{
	case 0:
		{
			g_mciPlyr.StopMusic(bEnable);
			if (NULL != g_pdirBGMusic)
			{
				if (m_bEnableBGMusic)
					g_pdirBGMusic->Play(NULL, TRUE);
				else
					g_pdirBGMusic->Stop();
			}
			break;
		}
	case 1:
		{
			if (NULL != g_pmdBGMusic)
			{
				if (m_bEnableBGMusic)
				{
					g_pmdBGMusic->Rewind();
					g_pmdBGMusic->Play(TRUE);
					g_pmdBGMusic->SetVolume(m_cbBGMusicVol);
				}
				else
					g_pmdBGMusic->Pause();
			}
			break;
		}
	default:
		ASSERT(FALSE);
	}
}

bool CGameFrameDlg::SetSoundVol(BYTE cbVol)
{
	ASSERT(100 >= cbVol);
	if (100 < cbVol) return false;
	m_cbSoundVol = cbVol;

	CString strPath, strTmp;
	GetModuleFileName(NULL, strPath.GetBufferSetLength(256), 256);
	strPath = strPath.Left(strPath.ReverseFind('\\')+1);
	strPath = strPath + TEXT("config\\Common.ini");
	strTmp.Format("%ld", cbVol);
	WritePrivateProfileString(TEXT("Sound"), TEXT("SoundVol"), strTmp, strPath);

	g_mciPlyr.SetSoundVol(cbVol);
	if (!m_bAllowSound) return true;
	if (g_dirSound.IsValid()) g_dirSound.SetWavVolume((cbVol-100)*-(cbVol-100));
	return true;
}
bool CGameFrameDlg::SetBGMusicVol(BYTE cbVol, BYTE cbType)
{
	ASSERT(100 >= cbVol);
	if (100 < cbVol) return false;
	m_cbBGMusicVol = cbVol;

	CString strPath, strTmp;
	GetModuleFileName(NULL, strPath.GetBufferSetLength(256), 256);
	strPath = strPath.Left(strPath.ReverseFind('\\')+1);
	strPath = strPath + TEXT("config\\Common.ini");
	strTmp.Format("%ld", cbVol);
	WritePrivateProfileString(TEXT("Sound"), TEXT("BGMusicVol"), strTmp, strPath);

	if (!m_bEnableBGMusic) return true;
	g_mciPlyr.SetMusicVol(cbVol);
	if (NULL != g_pdirBGMusic) g_pdirBGMusic->SetWavVolume((cbVol-100)*-(cbVol-100));
	if (NULL != g_pmdBGMusic) g_pmdBGMusic->SetVolume(cbVol);
	return true;
}







//插入图片
bool CGameFrameDlg::InsertImage(CBitmap * pBitmap)
{
	if (m_MessageProxyHelper.GetInterface()==NULL) return false;
	return m_MessageProxyHelper->InsertImage(pBitmap);
}

//插入名字
bool CGameFrameDlg::InsertUserName(LPCTSTR pszUserName)
{
	if (m_MessageProxyHelper.GetInterface()==NULL) return false;
	return m_MessageProxyHelper->InsertUserName(pszUserName);
}

//系统消息
bool CGameFrameDlg::InsertSystemString(LPCTSTR pszString)
{
	if (m_MessageProxyHelper.GetInterface()==NULL) return false;
	return m_MessageProxyHelper->InsertSystemString(pszString,MS_NORMAL);
}

//特色消息
bool CGameFrameDlg::InsertShtickString(LPCTSTR pszString, COLORREF crColor, bool bNewLine)
{
	if (m_MessageProxyHelper.GetInterface()==NULL) return false;
	return m_MessageProxyHelper->InsertShtickString(pszString,crColor,MS_NORMAL,bNewLine);
}

//普通消息
bool CGameFrameDlg::InsertGeneralString(LPCTSTR pszString, COLORREF crColor, bool bNewLine)
{
	if (m_MessageProxyHelper.GetInterface()==NULL) return false;
	return m_MessageProxyHelper->InsertGeneralString(pszString,crColor,MS_NORMAL,bNewLine);
}

//激活界面
void CGameFrameDlg::ActiveGameFrame()
{
	//获取窗口
	CWnd * pFocusWnd=GetFocus();
	CWnd * pMainWnd=AfxGetMainWnd();

	//激活判断
	if ((pFocusWnd==NULL)||(pMainWnd->IsChild(pFocusWnd)==FALSE)) pMainWnd->FlashWindow(TRUE);

	return;
}

//切换椅子
WORD CGameFrameDlg::SwitchViewChairID(WORD wChairID)
{
	//效验状态
	if (m_ClientKernelHelper.GetInterface()==NULL) return INVALID_CHAIR;

	//变量定义
	const tagUserData * pMeUserData=m_ClientKernelHelper->GetMeUserInfo();
	if (NULL == pMeUserData)
	{
		return INVALID_CHAIR;
	}
	const tagServerAttribute * pServerAttribute=m_ClientKernelHelper->GetServerAttribute();
	if (NULL == pServerAttribute)
	{
		return INVALID_CHAIR;
	}

	//转换椅子
	WORD wViewChairID=(wChairID+pServerAttribute->wChairCount-pMeUserData->wChairID);
	switch (pServerAttribute->wChairCount)
	{
	case 2: { wViewChairID+=1; break; }
	case 3: { wViewChairID+=1; break; }
	case 4: { wViewChairID+=2; break; }
	case 5: { wViewChairID+=2; break; }
	case 6: { wViewChairID+=3; break; }
	case 7: { wViewChairID+=3; break; }
	case 8: { wViewChairID+=4; break; }
	}
	return wViewChairID%pServerAttribute->wChairCount;
}

//获得桌子椅子数
WORD CGameFrameDlg::GetGameDeskChairNums()
{
	const tagServerAttribute * pServerAttribute=m_ClientKernelHelper->GetServerAttribute();
	if (NULL == pServerAttribute)
	{
		return INVALID_CHAIR;
	}
	return pServerAttribute->wChairCount;
}

//获取游戏ID
WORD CGameFrameDlg::GetGameKindID()
{
	const tagServerAttribute * pServerAttribute = m_ClientKernelHelper->GetServerAttribute();
	if (NULL != pServerAttribute)
	{
		return pServerAttribute->wKindID;
	}
	return INVALID_CHAIR;
}
//游戏状态
BYTE CGameFrameDlg::GetGameStatus()
{
	if (m_ClientKernelHelper.GetInterface()==NULL) return GS_FREE;
	return m_ClientKernelHelper->GetGameStatus();
}

//旁观模式
bool CGameFrameDlg::IsLookonMode()
{
	if (m_ClientKernelHelper.GetInterface()==NULL) return true;
	return m_ClientKernelHelper->IsLookonMode();
}

//允许旁观
bool CGameFrameDlg::IsAllowLookon()
{
	if (m_ClientKernelHelper.GetInterface()==NULL) return false;
	return m_ClientKernelHelper->IsAllowLookon();
}

//允许旁观
bool CGameFrameDlg::IsAllowUserLookon()
{
	if (m_ClientKernelHelper.GetInterface()==NULL) return false;
	return m_ClientKernelHelper->IsAllowUserLookon();
}

//旁观控制
bool CGameFrameDlg::AllowUserLookon(DWORD dwUserID, bool bAllowLookon)
{
	return false;

	//状态效验
	if (m_ClientKernelHelper.GetInterface()==NULL) return false;

	//设置按钮
	if (dwUserID==0L)
	{
		//获取资源
		HINSTANCE hInstance=g_GlobalOption.m_PlatformResourceModule->GetResInstance();
		tagGameFrameImage &GameFrameImage=g_GlobalOption.m_GameFrameImage;

		//控制按钮
		//UINT uImageID=m_ClientKernelHelper->IsAllowUserLookon()?GameFrameImage.uBtGameLookOn:GameFrameImage.uBtGameUnLookOn;
		UINT uImageID=GameFrameImage.uBtGameLookOn;
		m_pGameFrameControl->m_btLookOn.SetButtonImage(uImageID,hInstance,false);
	}

	//发送消息
	return m_ClientKernelHelper->AllowUserLookon(dwUserID,bAllowLookon);
}

//游戏状态
void CGameFrameDlg::SetGameStatus(BYTE bGameStatus)
{
	if (m_ClientKernelHelper.GetInterface()==NULL) return;
	m_ClientKernelHelper->SetGameStatus(bGameStatus);
	return;
}

//自己位置
WORD CGameFrameDlg::GetMeChairID()
{
	if (m_ClientKernelHelper.GetInterface()==NULL) return INVALID_CHAIR;
	return m_ClientKernelHelper->GetMeChairID();
}

//时间位置
WORD CGameFrameDlg::GetTimeChairID()
{
	if (m_ClientKernelHelper.GetInterface()==NULL) return INVALID_CHAIR;
	return m_ClientKernelHelper->GetTimeChairID();
}

//加入游戏
VOID CGameFrameDlg::JoinInGameTable(WORD wChairID)
{
	//状态效验
	ASSERT(m_ClientKernelHelper.GetInterface()!=NULL);
	if (m_ClientKernelHelper.GetInterface()==NULL) return;

	//获取信息
	const tagUserData * pUserData=m_ClientKernelHelper->GetMeUserInfo();
	if (pUserData==NULL) return;

	//变量定义
	IPC_JoinInGame JoinInGame;
	ZeroMemory(&JoinInGame,sizeof(JoinInGame));

	//设置变量
	JoinInGame.wChairID=wChairID;
	JoinInGame.wTableID=pUserData->wTableID;

	//发送消息
	m_ClientKernelHelper->SendProcessData(IPC_MAIN_CONCTROL,IPC_SUB_JOIN_IN_GAME,&JoinInGame,sizeof(JoinInGame));

	return;
}

//获取用户
const tagUserData * CGameFrameDlg::GetUserData(WORD wChairID)
{
	if (m_ClientKernelHelper.GetInterface()==NULL) return NULL;
	return m_ClientKernelHelper->GetUserInfo(wChairID);
}

//内核接口
void * CGameFrameDlg::GetClientKernel(const IID & Guid, DWORD dwQueryVer)
{
	if (m_ClientKernelHelper.GetInterface()==NULL) return NULL;
	return m_ClientKernelHelper->QueryInterface(Guid,dwQueryVer);
}

//控件绑定
void CGameFrameDlg::DoDataExchange(CDataExchange * pDX)
{
	__super::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CONTROL, m_btControl);
// 	DDX_Control(pDX, IDC_BT_TITLE_MIN, m_btTitleMinCtBt);
// 	DDX_Control(pDX, IDC_BT_TITLE_SOUND, m_btTitleSoundBt);
// 	DDX_Control(pDX, IDC_BT_TITLE_MUTESOUND, m_btTitleMuteSoundBt);
// 	DDX_Control(pDX, IDC_BT_TITLE_CLOSE, m_btTitleWndCloseBt);
	DDX_Control(pDX, IDC_VOR_SPLITER, m_VorSplitter);
}

//初始化函数
BOOL CGameFrameDlg::OnInitDialog()
{
	__super::OnInitDialog();

	//CHtmlEditCtrl2						  * m_pBrowser;						///< 浏览器

	//移动窗口
	INT nXScreen=GetSystemMetrics(SM_CXSCREEN);
	INT nYScreen=GetSystemMetrics(SM_CYSCREEN);
	SetWindowPos(NULL,0,0,__min(nXScreen,960),__min(nYScreen,720),SWP_NOZORDER|SWP_NOMOVE);

	// --读取声音配置信息
	CString strLclPth, strFlPth;
	GetModuleFileName(NULL, strLclPth.GetBufferSetLength(256), 256);
	strFlPth = strLclPth.Left(strLclPth.ReverseFind('\\')+1)+ TEXT("config\\Common.ini");
	UINT uiReturn = GetPrivateProfileInt(TEXT("Sound"), TEXT("EnableSound"), 1, strFlPth);
	m_bAllowSound =((0 == uiReturn)? false : true);     // --允许游戏音效
	m_cbSoundVol = GetPrivateProfileInt(TEXT("Sound"), TEXT("SoundVol"), 100, strFlPth);
	if (100 < m_cbSoundVol) m_cbSoundVol = 100;         // --读取音量
	else if (0 > m_cbSoundVol) m_cbSoundVol = 0;
	uiReturn = GetPrivateProfileInt(TEXT("Sound"), TEXT("EnableBGMusic"), 1, strFlPth);
	m_bEnableBGMusic =((0 == uiReturn)? false : true);// --允许背景音乐
	m_cbBGMusicVol = GetPrivateProfileInt(TEXT("Sound"), TEXT("BGMusicVol"), 100, strFlPth);
	if (100 < m_cbBGMusicVol) m_cbBGMusicVol = 100;     // --读取背景音乐音量
	else if (0 > m_cbBGMusicVol) m_cbBGMusicVol = 0;
	g_mciPlyr.SetSoundVol(m_cbSoundVol);
	g_mciPlyr.SetMusicVol(m_cbBGMusicVol);


	//设置属性
	//ModifyStyle(WS_CAPTION|WS_BORDER,0,0);
	//删除窗口标题和获取属性
	//ModifyStyle(WS_CAPTION|WS_BORDER,SWP_DRAWFRAME,0);

	//创建控制
	//m_pGameFrameControl=new CGameFrameControl(this);
	//if (m_pGameFrameControl==NULL) throw TEXT("游戏框架创建失败");

	//创建组件
	if (m_MessageProxyHelper.CreateInstance()==false) throw TEXT("信息组件创建失败");
	//m_MessageProxyHelper->SetRichEditHwnd(&m_pGameFrameControl->m_ChatMessage);

	//设置界面
	//m_VorSplitter.SetSplitterSink(this);
	//m_VorSplitter.ShowWindow(SW_HIDE);

	//创建组件
	if (m_UserFaceResHelper.CreateInstance()==false) throw TEXT("头像组件加载失败");
	if (m_ClientKernelHelper.CreateInstance()==false) throw TEXT("游戏框架内核模块加载失败");


	//创建游戏视图
	CRect rcGameView(0,0,0,0);
	m_pGameFrameView->Create(NULL,NULL,WS_VISIBLE|WS_CHILD|WS_CLIPSIBLINGS|WS_CLIPCHILDREN,rcGameView,this,10);
	if (m_pGameFrameView->SetUserFaceRes(m_UserFaceResHelper.GetInterface())==false) throw TEXT("头像资源接口设置失败");

#if REMOVE_VIDEO // add by HouGuoJiang 2011-11-25
	CVideoServiceManager * pVideoServiceManager=CVideoServiceManager::GetInstance();
	if (pVideoServiceManager!=NULL && pVideoServiceManager->SetUserFaceRes(m_UserFaceResHelper.GetInterface())==false) throw TEXT("头像资源接口设置失败");
#endif
	//m_pGameFrameControl->Create(IDD_FRAME_CONTROL,this);
	//if (m_pGameFrameControl->SetUserFaceRes(m_UserFaceResHelper.GetInterface())==false) throw TEXT("头像资源接口设置失败");

	//创建聊天框
	m_pGameFrameControl=new CGameFrameControl(this);
	if (m_pGameFrameControl==NULL) throw TEXT("游戏框架创建失败");
	m_pGameFrameControl->Create(IDD_FRAME_CONTROL,this);
	m_pGameFrameControl->ShowWindow(SW_SHOW);

	//初始化内核
	LPCTSTR lpszCmdLine=AfxGetApp()->m_lpCmdLine;
	bool bSuccess=m_ClientKernelHelper->InitClientKernel(lpszCmdLine,m_pKernelSink);
	if (bSuccess==false) throw TEXT("游戏框架逻辑模块初始化失败");
	bSuccess=m_ClientKernelHelper->SetMessageProxy(m_MessageProxyHelper.GetInterface());
	if (bSuccess==false) throw TEXT("获取消息输出模块失败");



	//创建玩家道具列表
	m_pUserDJInfo = NULL;
	m_pUserDJInfo = CGameClientDJInfo::GetDJInfoInstance();
	if (NULL == m_pUserDJInfo)
	{
		throw TEXT("玩家道具列表创建失败！");
	}

	////创建时间道具列表
	//m_pShowTimeDJList = new CShowTimeDJList(this);
	//if (NULL == m_pShowTimeDJList)
	//{
	//	throw TEXT("时间道具列表窗口创建失败！");
	//}
	//m_pShowTimeDJList->Create(IDD_SHOWTIMEDJLIST, this);
	//m_pShowTimeDJList->ShowWindow(SW_HIDE);
	//m_pUserDJInfo->SetUserTimeDJPtr(m_pShowTimeDJList);

	//创建道具信息显示窗口
	m_pShowDJInfo = new CShowDJInfo(this);
	if (NULL == m_pShowDJInfo)
	{
		throw TEXT("道具信息显示窗口创建失败！");
	}
	m_pShowDJInfo->Create(IDD_SHOWDJINFO, this);
	SafeShowOrHideWnd(m_pShowDJInfo, SW_HIDE);
	//m_pShowDJInfo->ShowWindow(SW_HIDE);

	////创建道具赠送信息窗口
	//m_pGameGiveDJInfo = new CGameGiveDJInfo(this);
	//if (NULL == m_pGameGiveDJInfo)
	//{
	//	throw TEXT("道具赠送信息窗口创建失败！");
	//}
	//m_pGameGiveDJInfo->Create(IDD_DLG_DJ_GIVE_INFO, this);
	//m_pGameGiveDJInfo->ShowWindow(SW_HIDE);

	////创建商城窗口
	//m_pGameBizCity = new CGameBizCity(this);
	//if (NULL != m_pGameBizCity)
	//{
	//	m_pGameBizCity->Create(IDD_GAME_BIZ_CITY_DLG, this);
	//	m_pGameBizCity->ShowWindow(SW_HIDE);
	//}

	//语音列表
	m_pLanguageList = NULL;
	m_pLanguageList = new CLanguageList();
	if (NULL == m_pLanguageList)
	{
		throw TEXT("语音列表窗口创建失败！");
	}
	m_pLanguageList->Create(CLanguageList::IDD, this);
	//m_pLanguageList->ShowWindow(SW_HIDE);
	SafeShowOrHideWnd(m_pLanguageList, SW_HIDE);
	m_pLanguageList->SetClientKernel(m_ClientKernelHelper.GetInterface());
	m_pLanguageList->InitData();


	//用户信息显示对话框
	m_pUserInfoDlg = new CUserInfoDlg();
	if (NULL == m_pUserInfoDlg)
	{
		throw TEXT("用户信息显示对话框创建失败！");
	}
	m_pUserInfoDlg->SetGameFrameDlgPtr(this);
	m_pUserInfoDlg->Create(IDD_USERINFODLG, this);
	//m_pUserInfoDlg->ShowWindow(SW_SHOW);
	SafeShowOrHideWnd(m_pUserInfoDlg, SW_SHOW);
	m_pUserInfoDlg->SetShowDJInfo(m_pShowDJInfo);
	m_pUserInfoDlg->SetLLInfoPtr(m_pLanguageList);
	m_MessageProxyHelper->SetRichEditHwnd(&m_pGameFrameControl->m_ChatMessage);
	CUserDaoJu* pUserDaoJuPtr= m_pUserInfoDlg->GetUserDaoJuListPtr();
	if (NULL != pUserDaoJuPtr)
	{
		m_pUserDJInfo->SetUserDaoJuInfoPtr(pUserDaoJuPtr);
	}


	//创建游戏结算视图
	if (NULL != m_pGameScoreView && !::IsWindow(m_pGameScoreView->GetSafeHwnd()))
	{
		m_pGameScoreView->Create(IDD_SCORE_VIEW_DLG, this);
		m_pGameScoreView->SetGameFramePtr(this);
		m_pGameScoreView->ShowWindow(SW_HIDE);
	}


	//创建任务视图
	m_pGameTaskDlg = new CGameTaskDialog();
	if (NULL == m_pGameTaskDlg) 
	{
		throw TEXT("任务对话框创建失败！");
	}
	m_pGameTaskDlg->Create(IDD_GAME_TASK_DLG, this);
	m_pGameTaskDlg->ShowWindow(SW_HIDE);



	////创建发送聊天消息
	//m_pSendMsgDlg = new CSendMsgDlg();
	//if (NULL == m_pSendMsgDlg)
	//{
	//	throw TEXT("输入聊天消息窗口创建失败！");
	//}
	//m_pSendMsgDlg->Create(IDD_INPUT_MSG_DLG,this);
	//m_pSendMsgDlg->SetGameFrameDlgPtr(this);
	//m_pSendMsgDlg->SetClientKernel(m_ClientKernelHelper.GetInterface());

	//创建用户动作
	//m_pGameUserAction = new CUserAction();
	//if (NULL == m_pGameUserAction)
	//{
	//	throw TEXT("用户动作窗口创建失败！");
	//}
	//m_pGameUserAction->Create(IDD_DLG_USERACTION,this);
	//m_pGameUserAction->ShowWindow(SW_HIDE);


	//喜报系统
	m_pGameAffiche = NULL;
	m_pGameAffiche = new CGameAffiche();
	if (NULL == m_pGameAffiche)
	{
		throw TEXT("喜报系统窗口创建失败！");
	}
	m_pGameAffiche->Create(IDD_DLG_GAMEAFFICHE, this);
	TCHAR szResourcePath[MAX_PATH];
	GetCurrentDirectory(MAX_PATH,szResourcePath);
	std::string strPath(szResourcePath);
	strPath.append("\\data\\gameframe\\plaza\\AfficheBack.png");
	m_pGameAffiche->SetBackImage(strPath.c_str(), true);
	m_pGameAffiche->SetWndExp(150);
	m_pGameAffiche->SetTextColor(RGB(0, 255, 0));
	m_pGameAffiche->SetShowRect(CRect(0, 0, 948, 20));
	m_pGameAffiche->SetGameAfficheInfo("");
	m_pGameAffiche->SetPlay(false);
	SafeShowOrHideWnd(m_pGameAffiche, SW_HIDE);



	//设置任务完成动画
	m_pTaskFinishFlash = NULL;
	m_pTaskFinishFlash = new CTaskFinishFlash();
	if (NULL == m_pTaskFinishFlash)
	{
		throw TEXT("任务完成动画窗口创建失败！");
	}
	m_pTaskFinishFlash->Create(IDD_DLG_TASK_FINISH, this);
	m_pTaskFinishFlash->ShowWindow(SW_HIDE);


	////创建用户动作信息显示窗口
	//m_pShowUserActionInfo = new CShowUserActionInfo();
	//if (NULL == m_pShowUserActionInfo)
	//{
	//	throw TEXT("用户动作显示窗口创建失败！");
	//}
	//m_pShowUserActionInfo->Create(NULL,NULL,WS_VISIBLE|WS_CHILD|WS_CLIPSIBLINGS|WS_CLIPCHILDREN,rcGameView,this,13);
	//m_pShowUserActionInfo->ShowWindow(SW_HIDE);
	//m_pShowUserActionInfo->InitShowData();


	//游戏框架边框
	m_pGameFrameBorder = NULL;
	m_pGameFrameBorder = new CGameFrameBorder();
	if (NULL == m_pGameFrameBorder)
	{
		throw TEXT("游戏边框窗口创建失败！");
	}
	m_pGameFrameBorder->Create(CGameFrameBorder::IDD, this);
	m_pGameFrameBorder->ShowWindow(SW_SHOW);

	//新手礼包	Author:<cxf>	DateTime<2012-01-17>
	m_pNewGiftFlash = NULL;
	m_pNewGiftFlash = new CNewGiftFlashDlg();
	if (NULL ==m_pNewGiftFlash)
	{
		throw TEXT("新手礼包窗口创建失败！");
	}
	m_pNewGiftFlash->Create(IDD_NEWGIFT, this);


	//加载资源
	//UpdateSkinResource();


	//显示窗口
	m_bInitDialog=true;
	m_pGameFrameView->ShowWindow(SW_SHOW);

	//OSC 
	//m_btControl.ShowWindow(SW_HIDE);

	//
	//设置标题栏高度
	//SetNCRect(29);

	//开始加载资源
	LoadTitleResource();
	//显示窗口
	CenterWindow();
	//MaxSizeWindow();
	ShowWindow(SW_SHOW);
	RECT rect;
	GetClientRect(&rect);
	CRect rcFrameControl(rect.right-240,rect.bottom-127,rect.right,rect.bottom);

	//m_pGameFrameControl->SetWndPoint(rcFrameControl);
	//设置游戏框架控件指针
	//m_pSendMsgDlg->SetGameFrameControlPtr(m_pGameFrameControl);
	RectifyControl(960, 720);

	//m_pGameFrameControl->SetWindowPos(NULL, rcFrameControl.left,rcFrameControl.top,rcFrameControl.right,rcFrameControl.bottom,0);
	//Z顺序
	//::SetWindowPos(m_pGameFrameControl->GetSafeHwnd(), m_pSendMsgDlg->GetSafeHwnd(), rect.right-240,rect.bottom-127,rect.right,rect.bottom,0);
	//::SetWindowPos(, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_SHOWWINDOW);
	//::SetWindowPos(, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_HIDEWINDOW);
	//清空游戏公告信息
	//m_vecGameMsg.clear();
	//m_iCountVecIndex = 0;
	//m_bIsDisableGameMsg = false;

	//默认显示
	//SetChatShowTag(true);


	//获得指定方言
	//TCHAR chLanguage[][7] = {	TEXT("GY"),   //普通话
	//							TEXT("CS"),	  //长沙话
	//							TEXT("YY") };  //益阳话
	//m_strLanguage.clear();
	//CRegKey RegKey;
	//RegKey.Open(HKEY_CURRENT_USER,REG_USER_INFO,KEY_READ);
	//DWORD iLanguageValue=0;
	//if (RegKey != NULL)
	//{
	//	RegKey.QueryDWORDValue(TEXT("Language"), iLanguageValue);
	//	RegKey.Close();
	//}
	//if (iLanguageValue>=0 && iLanguageValue<=2)
	//{
	//	m_strLanguage.append(chLanguage[iLanguageValue]);
	//}
	//else
	//{
	//	m_strLanguage.append(chLanguage[0]);
	//}
	
	//初始化游戏
	m_pGameFrameControl->SetClientKernel(m_ClientKernelHelper.GetInterface());
	if (InitGameFrame()==false) throw TEXT("游戏框架初始化失败");

	//PlayDirSound("F:\\game\\server\\运行\\release\\data\\music\\abc.mp3", true);	//播放游戏音效
	//PlayBGMusic("F:\\game\\server\\运行\\release\\data\\music\\Land.mp3", 0, true);	//播放背景音乐

	// 针对的问题：提示窗口隐藏太快了，游戏加载还未完成，进程如同突然消失了，客户不知所措
	m_hEventGameLoadOver = NULL;
	m_strGameLoadOver = TEXT("MY_GAME_LOAD_OVER");
	m_hEventGameLoadOver = CreateEvent(NULL, FALSE, FALSE, m_strGameLoadOver);

	if(m_hEventGameLoadOver)
	{
		SetEvent(m_hEventGameLoadOver);

		CloseHandle(m_hEventGameLoadOver);
		m_hEventGameLoadOver = NULL;
	}

	//m_pGameTaskDlg->ShowWindow(SW_SHOW);//Test
	//m_pGameFrameView->UpdateGameView(NULL);//Test
	return TRUE;
}


//设置显示状态
void CGameFrameDlg::SetChatShowTag(bool bIsShow, bool bIsUp)
{
	return;
	//if (true == bIsShow)
	//{//显示
	//	m_pGameFrameControl->ShowWindow(SW_SHOW);
	//	m_pSendMsgDlg->SetGameFrameShowTag(bIsShow);
	//}
	//else
	//{//隐藏
	//	m_pGameFrameControl->ShowWindow(SW_HIDE);
	//	m_pSendMsgDlg->SetGameFrameShowTag(bIsShow);
	//}

	//m_pGameFrameControl->SetWndShow(bIsUp);
}


//时间消息
void CGameFrameDlg::OnTimer(UINT nIDEvent)
{
	////时间处理
	//if (GAME_MSG_TIME_ID==nIDEvent)
	//{
	//	m_rcGameMsgInfo.left -= 2;
	//	m_rcGameMsgInfo.right -= 2;
	//	if (m_rcGameMsgInfo.right < GAMEINFO_L)
	//	{
	//		++m_iCountVecIndex;
	//		if (m_iCountVecIndex>=m_vecGameMsg.size())
	//		{
	//			m_iCountVecIndex = 0;
	//		}
	//		CDC *pDC = GetDC();
	//		if ((NULL != pDC) && (m_vecGameMsg.size()>0) && (m_iCountVecIndex<m_vecGameMsg.size()))
	//		{
	//			m_rcGameMsgInfo.left = 960-GAMEINFO_L;
	//			m_rcGameMsgInfo.top = 0;
	//			m_rcGameMsgInfo.right = m_rcGameMsgInfo.left+pDC->GetTextExtent(m_vecGameMsg[m_iCountVecIndex].c_str()).cx;
	//			m_rcGameMsgInfo.bottom = 22;
	//		}
	//		else
	//		{
	//			KillTimer(GAME_MSG_TIME_ID);
	//		}
	//		ReleaseDC(pDC);
	//	}
	//	CRect drawrect(GAMEINFO_L, 0, 960-GAMEINFO_R, 22);
	//    InvalidateRect(&drawrect, TRUE);
	//}

	CDialog::OnTimer(nIDEvent);
}


//消息解释
BOOL CGameFrameDlg::PreTranslateMessage(MSG * pMsg)
{
	//过滤消息
	if ((pMsg->message==WM_KEYDOWN)&&(pMsg->wParam==VK_ESCAPE)) return TRUE;
	return __super::PreTranslateMessage(pMsg);
}

//确定消息
void CGameFrameDlg::OnOK()
{
	return;
}

//取消消息
void CGameFrameDlg::OnCancel()
{

	//退出提示
	if ((m_ClientKernelHelper.GetInterface()!=NULL)&&(m_ClientKernelHelper->GetExitFlags()==false))
	{
		//退出捕鱼不用提示信息；Author<cxf>;DateTime<2012-03-31>
		if(601!=m_ClientKernelHelper->GetServerAttribute()->wKindID)
		{
			WORD wChairID=GetMeChairID();
			const tagUserData * pUserData=GetUserData(wChairID);
			if (pUserData!=NULL)
			{
				int iRet=ShowInformation(TEXT("您正在游戏中，强行退出将被扣分，确实要强退吗？"),0,MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2);
				if (iRet!=IDYES) return;
			}
		}
	}

	m_ClientKernelHelper->SendProcessData(IPC_MAIN_CONCTROL,IPC_SUB_GAME_FRAME_CLOSED);

	//保存配置
	g_GlobalOption.SaveOptionParameter();

	//注销接口
	m_ClientKernelHelper->DestroyClientKernel();

	//销毁组件
	SafeDelete(m_pKernelSink);
	m_UserFaceResHelper.CloseInstance();
	m_ClientKernelHelper.CloseInstance();

	//关闭窗口
	DestroyWindow();

	return;
}

//控制按钮
VOID CGameFrameDlg::OnBnClickedControl()
{
	return;

	//设置变量
	m_bHideList=!m_bHideList;

	//设置界面
	HINSTANCE hInstance=g_GlobalOption.m_PlatformResourceModule->GetResInstance();
	tagGameFrameImage &GameFrameImage=g_GlobalOption.m_GameFrameImage;
	m_btControl.SetButtonImage(m_bHideList?GameFrameImage.uBtShowUserList:GameFrameImage.uBtHideUserList,hInstance,false);

	//设置界面
	m_VorSplitter.EnableSplitter(m_bHideList==false);

	//调整界面
	CRect rcClient;
	GetClientRect(&rcClient);
	RectifyControl(rcClient.Width(),rcClient.Height());

	return;
}

//位置信息
void CGameFrameDlg::OnGetMinMaxInfo(MINMAXINFO * lpMMI)
{
	__super::OnGetMinMaxInfo(lpMMI);

	//设置位置
	lpMMI->ptMinTrackSize.x=960;
	lpMMI->ptMinTrackSize.y=720;

	return;
}

//位置改变
void CGameFrameDlg::OnWindowPosChanging(WINDOWPOS * lpwndpos)
{
	//if (m_bInitDialog==false) lpwndpos->flags&=~SWP_SHOWWINDOW;
	__super::OnWindowPosChanging(lpwndpos);
}

//位置改变
void CGameFrameDlg::OnSize(UINT nType, int cx, int cy)
{
	__super::OnSize(nType, cx, cy);

	//调整控件
	RectifyControl(960,720);

	if (m_bEnableBGMusic)
	{
		if ((0 >= cx)||(0 >= cy))
		{
			if (g_mciPlyr.m_bMusicPlaying) g_mciPlyr.SetMusicVol(0);
			if (NULL != g_pdirBGMusic) g_pdirBGMusic->Pause();
			if (NULL != g_pmdBGMusic) g_pmdBGMusic->Pause();
		}
		else
		{
			if (g_mciPlyr.m_bMusicPlaying) g_mciPlyr.SetMusicVol(m_cbBGMusicVol);
			if (NULL != g_pdirBGMusic) g_pdirBGMusic->Continue();
			if (NULL != g_pmdBGMusic) g_pmdBGMusic->Continue();
		}
	}

	return;

}

//响应消息
void CGameFrameDlg::SetChatMessageBeep()
{
	//if (NULL != m_pSendMsgDlg)
	//{
	//	m_pSendMsgDlg->SetMsgBeep();
	//}
}

//退出游戏方法
void CGameFrameDlg::SetGameExitOperation(BYTE bTag)
{
	if (1 == bTag)
	{//返回大厅(游戏列表)
		//发送消息
		m_ClientKernelHelper->SendProcessData(IPC_MAIN_CONCTROL,IPC_SUB_GO_PLAZA);

		//退出游戏
		OnCancel();
	}
	else if (2 == bTag)
	{//继续游戏
		OnEventContinueGame();

		//隐藏结算框
		m_pGameScoreView->ShowWindow(SW_HIDE);
	}
	else if (3 == bTag)
	{//返回房间列表
		//发送消息
		m_ClientKernelHelper->SendProcessData(IPC_MAIN_CONCTROL,IPC_SUB_GO_ROOM);

		//退出游戏
		OnCancel();
	}
	else
	{
		//退出游戏
		OnCancel();
	}
}



//接收新任务
void __cdecl CGameFrameDlg::OnEventTaskAccept(const CMD_GF_TASK_tagTask* pTaskAccept)
{
	if (IsLookonMode()) return;

#ifdef PRINT_GAME_CURS_MSG
	char chTaskID[64]={0};
	sprintf(chTaskID, "CGameFrameDlg::OnEventTaskAccept: 已接受的任务ID:%d", pTaskAccept->iTaskID);
	InsertGeneralString(chTaskID, RGB(255, 0, 0),true);
#endif

	if ((NULL != m_pGameTaskDlg) && (NULL != pTaskAccept) && (0!=pTaskAccept->iTaskID))
	{
		WORD wChairID=GetMeChairID();
		const tagUserData * pUserData=GetUserData(wChairID);
		if (pUserData!=NULL)
		{
			if (pUserData->dwUserID == pTaskAccept->dwUserID)
			{//如果是本人的任务
				const tagServerAttribute * pServerAttribute=m_ClientKernelHelper->GetServerAttribute();
				m_pGameTaskDlg->SetSortID(pServerAttribute->wSortID);
				m_pGameTaskDlg->SetTaskAcceptInfo(pTaskAccept);
			}
		}

#ifdef PRINT_GAME_CURS_MSG
		char chTaskID[128]={0};
		sprintf(chTaskID, "已接受的任务ID:%d, %d,%d", pTaskAccept->iTaskID, pUserData->dwUserID, pTaskAccept->dwUserID);
		InsertGeneralString(chTaskID, RGB(255, 0, 0),true);
#endif
	}
}
//单个任务完成
void __cdecl CGameFrameDlg::OnEventTaskOnceFinish(const CMD_GF_TASK_tagTaskFinish* pTaskFinish)
{
	if (IsLookonMode()) return;

	if ((NULL != m_pGameTaskDlg) && (NULL != pTaskFinish) && (0!=pTaskFinish->iTaskID))
	{
		WORD wChairID=GetMeChairID();
		const tagUserData * pUserData=GetUserData(wChairID);
		if (pUserData!=NULL)
		{
			char chTaskID[256]={0};
			if (pUserData->dwUserID == pTaskFinish->dwUserID)
			{//如果是本人的任务
				const tagServerAttribute * pServerAttribute=m_ClientKernelHelper->GetServerAttribute();
				m_pGameTaskDlg->SetSortID(pServerAttribute->wSortID);
				m_pGameTaskDlg->SetTaskOnceFinish(pTaskFinish);
				
				sprintf(chTaskID, "%s完成寻宝任务并获取宝石%d枚！", pUserData->szName, pTaskFinish->dwGems);
				InsertSystemString(chTaskID);
			}
			else
			{
				sprintf(chTaskID, "玩家完成寻宝任务并获取宝石%d枚！", pTaskFinish->dwGems);
				InsertSystemString(chTaskID);
			}
		}
	}

#ifdef PRINT_GAME_CURS_MSG
	char chTaskID[64]={0};
	sprintf(chTaskID, "任务已完成ID:%d, %d", pTaskFinish->iTaskID, pTaskFinish->dwUserID);
	InsertGeneralString(chTaskID, RGB(255, 0, 0),true);
#endif
}

//////////////////////////////////////////////////////////////////////////

BOOL CGameFrameDlg::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	return TRUE;
	return __super::OnEraseBkgnd(pDC);
}

LRESULT CGameFrameDlg::OnNcHitTest(CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	
	//if (point.y <= GAME_FRAME_DLG_HEIGHT)
	//{
	//	UINT iResult = CDialog::OnNcHitTest(point);
	//	if (HTCLIENT == iResult)
	//	{
	//		iResult = HTCAPTION;
	//	}
	//	return iResult;
	//}
	return __super::OnNcHitTest(point);
}

void CGameFrameDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	ViewHitOper();
	if (point.y < GAME_FRAME_DLG_HEIGHT)
	{
		PostMessage(WM_NCLBUTTONDOWN,HTCAPTION,MAKELPARAM(point.x,point.y));
	}



	//CMD_GF_NEW_GIFT newGift;
	//newGift.iGem = 4321;
	//newGift.iGold = 1234;
	//newGift.iGrade = 3;
	//newGift.iOnlineTime = 500;
	//newGift.wGiftType = 1;
	//OnEventNewGift(&newGift);//Test

	//if (NULL != m_pGameFrameView)
	//{
	//	m_pGameFrameView->OnlineKeepTime(30);
	//}//Test

	__super::OnLButtonDown(nFlags, point);
}

void CGameFrameDlg::OnMoving(UINT fwSide, LPRECT pRect)
{
	__super::OnMoving(fwSide, pRect);
	
	//RECT rect;
	//GetClientRect(&rect);
	
	//if (NULL != m_pGameScoreView)
	//{//结算视图
	//	CRect scorerect;
	//	scorerect.left = wndrect.left+((rect.right-rect.left)-m_pGameScoreView->m_szWndSize.cx)/2;
	//	//scorerect.right = scorerect.left + m_pGameScoreView->m_szWndSize.cx;
	//	scorerect.top = wndrect.top + ((rect.bottom-rect.top)-m_pGameScoreView->m_szWndSize.cy)/2-50;
	//	//scorerect.bottom = scorerect.top + m_pGameScoreView->m_szWndSize.cy;
	//	m_pGameScoreView->SetWindowPos(NULL,scorerect.left, scorerect.top, m_pGameScoreView->m_szWndSize.cx, m_pGameScoreView->m_szWndSize.cy, 0);
	//}
	//if (NULL != m_pGameTaskDlg)
	//{//任务框
	//	m_pGameTaskDlg->SetWindowPos(NULL,wndrect.left+((rect.right-rect.left)-321)/2,wndrect.top+((rect.bottom-rect.top)-476)/2+100,321,476,0);
	//}
	
	// TODO: 在此处添加消息处理程序代码
}

void CGameFrameDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	//__super::OnMouseMove(nFlags, point);
}

void CGameFrameDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	__super::OnLButtonUp(nFlags, point);
}

//显示时间道具列表窗口
void CGameFrameDlg::SetShowTimeDJListWnd(bool bShow)
{
	return ;
	//if (IsLookonMode())
	//{
	//	return;
	//}
	//if (NULL != m_pShowTimeDJList)
	//{
	//	m_pShowTimeDJList->SetShowWnd(bShow);
	//}
}

//显示道具信息窗口
void CGameFrameDlg::SetShowDJInfoWnd(bool bShow, int iIndex)
{
	//if (NULL != m_pShowDJInfo)
	//{
	//	m_pShowDJInfo->SetWndShow(bShow, iIndex);
	//}
}

//显示商城窗口
void CGameFrameDlg::SetShowBizCityWnd(bool bShow)
{
	return;
	//if (IsLookonMode())
	//{
	//	return;
	//}
	//if (NULL != m_pGameBizCity)
	//{
	//	RECT wndrect;
	//	GetWindowRect(&wndrect);
	//	if (NULL != m_pGameBizCity)
	//	{//商城窗口大小
	//		m_pGameBizCity->SetWindowPos(NULL, wndrect.left, wndrect.top, wndrect.right-wndrect.left, wndrect.bottom-wndrect.top, 0);
	//		m_pGameBizCity->ShowWindow(bShow?SW_SHOW:SW_HIDE);
	//	}
	//}
}

void CGameFrameDlg::CloseBizCityWindow(int iID)
{
	return;
	//if ((1 == iID) && (NULL!=m_pGameBizCity))
	//{
	//	m_pGameBizCity->ShowWindow(SW_HIDE);
	//}
}


void CGameFrameDlg::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	ViewHitOper();
	__super::OnRButtonDown(nFlags, point);
}

//隐藏用户动作窗口
void CGameFrameDlg::SetShowUserActionWnd(bool bIsShow)
{
	//if (IsLookonMode())
	//{
	//	return;
	//}
	//if (NULL != m_pGameUserAction)
	//{//用户动作窗口
	//	if ((m_ClientKernelHelper.GetInterface()!=NULL)&&(m_ClientKernelHelper->GetExitFlags()==false))
	//	{
	//		if (!bIsShow)
	//		{
	//			m_pGameUserAction->ShowWindow(SW_HIDE);
	//		}
	//		else
	//		{
	//			bool bIsCanSend = m_pGameUserAction->GetTimeIntervalSendStatus();
	//			if (bIsCanSend)
	//			{//可以发送
	//				m_pGameUserAction->InvalidateRect(NULL);
	//				m_pGameUserAction->ShowWindow(SW_SHOW);
	//			}
	//			else
	//			{
	//				char chTaskID[50]={0};
	//				sprintf(chTaskID, "动作发送过于频繁，请稍候再发！");
	//				InsertSystemString(chTaskID);
	//			}
	//		}
	//	}
	//}
}

void CGameFrameDlg::SaveScreenToBmp()
{
	HBITMAP hBitmap;
	HWND hWnd = AfxGetMainWnd()->m_hWnd;//获得屏幕的HWND.
	HDC hScreenDC = ::GetDC(hWnd);   //获得屏幕的HDC.
	HDC MemDC = ::CreateCompatibleDC(hScreenDC);
	CRect rect; 
	::GetClientRect(hWnd,&rect);
	SIZE screensize;
	screensize.cx=rect.Width();
	screensize.cy=rect.Height();
	hBitmap = ::CreateCompatibleBitmap(hScreenDC,screensize.cx,screensize.cy);
	HGDIOBJ hOldBMP = ::SelectObject(MemDC,hBitmap);
	::BitBlt(MemDC,0,0,screensize.cx,screensize.cy,hScreenDC,0,0,SRCCOPY);
	::SelectObject(MemDC,hOldBMP);
	::DeleteObject(MemDC);
	::ReleaseDC(hWnd,hScreenDC);

	//CDC *pDC=GetDC();
	//HDC dDlgDC=pDC->m_hDC;
	//HDC MemDC1 = CreateCompatibleDC(dDlgDC);
	//HGDIOBJ hOldBMP1 = ::SelectObject(MemDC1,hBitmap);
	//::StretchBlt(dDlgDC,0,0,600,400,MemDC1,0,0,screensize.cx,screensize.cy,SRCCOPY);
	//::SelectObject(MemDC1,hOldBMP1);
	//::DeleteObject(MemDC1);
	//::ReleaseDC(m_hWnd,dDlgDC);
	
	TCHAR szResourcePath[MAX_PATH];
	GetCurrentDirectory(MAX_PATH,szResourcePath);
	CString strPath(szResourcePath);

	int iA1 = 1+rand()%1000;
	int iA2 = 1+rand()%1000;
	int iA3 = 1+rand()%1000;

	SYSTEMTIME sys; 
	GetLocalTime( &sys ); 
	CString strFileName="";
	strFileName.Format("\\photo\\%d-%d-%d-%d-%d-%d-%d-%d%d%d.bmp", sys.wYear, sys.wMonth, sys.wDay, sys.wHour, sys.wMinute, sys.wSecond, sys.wMilliseconds, iA1, iA2, iA3);
	strPath.Append(strFileName);
	WriteBmpToFile(hBitmap, strPath);

	InsertSystemString(TEXT("您的截图照片已保存到：") + strPath);
}

void CGameFrameDlg::WriteBmpToFile(HBITMAP hBitmap,CString strFileName)
{
	HDC hDC =::CreateDC("DISPLAY",NULL,NULL,NULL); 
	int iBits = ::GetDeviceCaps(hDC, BITSPIXEL) * ::GetDeviceCaps(hDC, PLANES);//当前分辨率下每个像素所占字节数  
	::DeleteDC(hDC);

	WORD   wBitCount;   //位图中每个像素所占字节数    
	if (iBits <= 1)
		wBitCount = 1;
	else if (iBits <= 4)
		wBitCount = 4;
	else if (iBits <= 8)
		wBitCount = 8;
	else if (iBits <= 24)
		wBitCount = 24;
	else
		wBitCount = iBits;

	DWORD   dwPaletteSize=0;	//调色板大小， 位图中像素字节大小 
	if (wBitCount <= 8)		
		dwPaletteSize = (1 << wBitCount) *	sizeof(RGBQUAD);	


	BITMAP  bm;        //位图属性结构
	::GetObject(hBitmap, sizeof(bm), (LPSTR)&bm);  


	BITMAPINFOHEADER   bi,bi1;       //位图信息头结构     
	bi.biSize            = sizeof(BITMAPINFOHEADER);  
	bi.biWidth           = bm.bmWidth;
	bi.biHeight          = bm.bmHeight;
	bi.biPlanes          = 1;
	bi.biBitCount        = wBitCount;
	bi.biCompression     = BI_RGB; //BI_RGB表示位图没有压缩
	bi.biSizeImage       = 0;
	bi.biXPelsPerMeter   = 0;
	bi.biYPelsPerMeter   = 0;
	bi.biClrUsed         = 0;
	bi.biClrImportant    = 0;
	bi1=bi;
	bi1.biBitCount=24;

	DWORD dwBmBitsSize = ((bm.bmWidth * wBitCount+31)/32) * 4 * bm.bmHeight;    
	HANDLE hDib  = ::GlobalAlloc(GHND,dwBmBitsSize + dwPaletteSize + sizeof(BITMAPINFOHEADER));  //为位图内容分配内存
	LPBITMAPINFOHEADER lpbi = (LPBITMAPINFOHEADER)GlobalLock(hDib);
	*lpbi = bi;

	HANDLE hPal = ::GetStockObject(DEFAULT_PALETTE);  // 处理调色板 
	HANDLE  hOldPal=NULL; 
	if (hPal)
	{
		hDC = ::GetDC(NULL);
		hOldPal = SelectPalette(hDC,(HPALETTE)hPal, FALSE);
		RealizePalette(hDC);
	}
	int nOutputBytes = 0;
	unsigned char *pJpp,*m_pDibBits;
	pJpp = new unsigned char [dwBmBitsSize];
	m_pDibBits = new unsigned char [dwBmBitsSize];
	//::GetDIBits(hDC, hBitmap, 0, (UINT) bm.bmHeight,m_pDibBits,(BITMAPINFO*)lpbi,DIB_RGB_COLORS);
	::GetDIBits(hDC, hBitmap, 0, (UINT) bm.bmHeight,(LPSTR)lpbi + sizeof(BITMAPINFOHEADER)+dwPaletteSize,(BITMAPINFO*)lpbi,DIB_RGB_COLORS);// 获取该调色板下新的像素值
	if (hOldPal)//恢复调色板
	{
		SelectPalette(hDC, (HPALETTE)hOldPal, TRUE);
		RealizePalette(hDC);
		::ReleaseDC(NULL, hDC);
	}
	/*	memcpy(m_pDibBits,(LPSTR)lpbi+sizeof(BITMAPINFOHEADER)+dwPaletteSize,dwBmBitsSize);
	int i=0,j=0;

	for(j=0;j<dwBmBitsSize;j++)
	{
	if(j%4==3)
	j++;
	m_pDibBits[i]=m_pDibBits[j];		
	i++;
	}*/


	BITMAPFILEHEADER   bmfHdr; //位图文件头结构     
	bmfHdr.bfType = 0x4D42;  // "BM"  	// 设置位图文件头
	DWORD dwDIBSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + dwPaletteSize + dwBmBitsSize;  
	bmfHdr.bfSize = dwDIBSize;
	bmfHdr.bfReserved1 = 0;
	bmfHdr.bfReserved2 = 0;
	bmfHdr.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + (DWORD)sizeof(BITMAPINFOHEADER) + dwPaletteSize;

	HANDLE hFile = CreateFile(strFileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);//创建位图文件   
	DWORD dwWritten;
	WriteFile(hFile, (LPSTR)&bmfHdr, sizeof(BITMAPFILEHEADER), &dwWritten, NULL);	// 写入位图文件头
	WriteFile(hFile, (LPSTR)lpbi, dwDIBSize, &dwWritten, NULL);// 写入位图文件其余内容


	GlobalUnlock(hDib);   //清除   
	GlobalFree(hDib);
	CloseHandle(hFile); 
	delete []pJpp;
	delete []m_pDibBits;
}
BOOL CGameFrameDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
	// TODO: 在此添加专用代码和/或调用基类
	int iCmdID =  LOWORD(wParam);
	switch (iCmdID)
	{
	case IDC_BT_TITLE_GAMEHOME:	//官方首页
		{
			//char szRuleUrl[256]={0};
			//sprintf(szRuleUrl, "%s", WEB_HOME_PAGE);
			//ShellExecute(NULL,TEXT("open"),szRuleUrl,NULL,NULL,SW_SHOWDEFAULT);
			IPC_ConnWeb web;
			web.byConWebTag = 1;
			m_ClientKernelHelper->SendProcessData(IPC_MAIN_CONCTROL, IPC_SUB_CMD_CONNECTWEB, &web, sizeof(IPC_ConnWeb));
		}
		break;
	case IDC_BT_TITLE_GAMESHOP:	//道具商城
		{
			//char szRuleUrl[256]={0};
			//sprintf(szRuleUrl, "%s", WEB_MY_SHOP);
			//ShellExecute(NULL,TEXT("open"),szRuleUrl,NULL,NULL,SW_SHOWDEFAULT);
			IPC_ConnWeb web;
			web.byConWebTag = 2;
			m_ClientKernelHelper->SendProcessData(IPC_MAIN_CONCTROL, IPC_SUB_CMD_CONNECTWEB, &web, sizeof(IPC_ConnWeb));
		}
		break;
	case IDC_BT_TITLE_GAMESERVER:	//客户中心
		{
			//char szRuleUrl[256]={0};
			//sprintf(szRuleUrl, "%s", WEB_CLINET_CENTER);
			//ShellExecute(NULL,TEXT("open"),szRuleUrl,NULL,NULL,SW_SHOWDEFAULT);
			IPC_ConnWeb web;
			web.byConWebTag = 3;
			m_ClientKernelHelper->SendProcessData(IPC_MAIN_CONCTROL, IPC_SUB_CMD_CONNECTWEB, &web, sizeof(IPC_ConnWeb));
		}
		break;
	default:
		break;
	}
	
	return __super::OnCommand(wParam, lParam);
}

//视图点击操作
void CGameFrameDlg::ViewHitOper()
{
	//if (NULL != m_pGameUserAction)
	//{//用户动作窗口
	//	m_pGameUserAction->ShowWindow(SW_HIDE);
	//}
	if (NULL != m_pLanguageList)
	{//语音列表
		m_pLanguageList->ShowWindow(SW_HIDE);
	}
	if (NULL != m_pShowDJInfo)
	{//道具信息显示窗口
		m_pShowDJInfo->ShowWindow(SW_HIDE);
	}
}

void CGameFrameDlg::GetGameName(CString& strName)
{
	strName = "";

	char* pText = GetCommandLine();
	std::string strProcessName(pText);
	int iIndex = strProcessName.rfind(".");
	strProcessName = strProcessName.substr(0, iIndex);
	if (!strProcessName.empty())
	{
		strName.Format("%s", strProcessName.c_str());
	}
}
void CGameFrameDlg::OnWindowPosChanged(WINDOWPOS* lpwndpos)
{
	__super::OnWindowPosChanged(lpwndpos);

	if (m_pGameAffiche && m_pGameAffiche->GetSafeHwnd() && m_pGameAffiche->IsWindowVisible())
	{//喜报系统
		bool bIsExp = m_pGameAffiche->GetIsExp();
		if (bIsExp)
		{
			RECT wndrect;
			GetWindowRect(&wndrect);
			m_pGameAffiche->SetWindowPos(NULL, wndrect.left+6,wndrect.top+29,0,0, SWP_NOSIZE);
		}
	}
	
	// add by HouGuoJiang 2011-11-25
	// 所有的 IsWindowVisible 都要加上 GetSafeHwnd 来保护
	//结算视图
	if (NULL!=m_pGameScoreView && m_pGameScoreView->GetSafeHwnd() && m_pGameScoreView->IsWindowVisible())
	{
		RECT wndrect;
		GetWindowRect(&wndrect);
		CRect scorerect;
		scorerect.left = wndrect.left+(wndrect.right-wndrect.left-m_pGameScoreView->m_szWndSize.cx)/2;
		scorerect.top = wndrect.top + ((wndrect.bottom-wndrect.top)-m_pGameScoreView->m_szWndSize.cy)/2-50;
		m_pGameScoreView->SetWindowPos(NULL,scorerect.left, scorerect.top, m_pGameScoreView->m_szWndSize.cx, m_pGameScoreView->m_szWndSize.cy, SWP_NOSIZE);
	}

	if (NULL!=m_pGameFrameView && m_pGameFrameView->GetSafeHwnd()  && m_pGameFrameView->IsWindowVisible())
	{
		//隐藏冒泡聊天显示框
		m_pGameFrameView->HideUserSayInfo();
	}

	if (0 < lpwndpos->cx  || 0 < lpwndpos->cy)
	{
		BringWindowToTop();
		SetActiveWindow();
		SetForegroundWindow();
	}
}


//播放任务完成动画
void CGameFrameDlg::PlayTaskFinishFlash(bool bIsPlay, UINT uiNums)
{
	if (NULL!=m_pTaskFinishFlash)
	{
		m_pTaskFinishFlash->Play(bIsPlay, uiNums);
	}
}


//发送语音消息
void CGameFrameDlg::SendLanguageListMsg(UINT uiIndex)
{
	const tagUserData* pMeInfo = GetUserData(GetMeChairID());
	if (NULL != pMeInfo)
	{
		IPC_UserLanguageInfo info;
		::memset(&info, 0, sizeof(IPC_UserLanguageInfo));
		info.dwIndex = uiIndex;
		info.dwSendUserID = GetMeChairID();
		m_ClientKernelHelper->SendSocketData(MDM_GF_FRAME,SUB_SEND_SOUND_MESS,&info,sizeof(IPC_UserLanguageInfo));
	}
}

//显示结算框
void CGameFrameDlg::ShowScoreView(bool bIsView)
{
	if (NULL != m_pGameScoreView)
	{//结算视图
		if (!bIsView)
		{
			m_pGameScoreView->ShowWindow(SW_HIDE);
		}
		else
		{
			RECT rect;
			GetClientRect(&rect);
			RECT wndrect;
			GetWindowRect(&wndrect);
			CRect scorerect;
			scorerect.left = wndrect.left+((rect.right-rect.left)-m_pGameScoreView->m_szWndSize.cx)/2;
			//scorerect.right = scorerect.left + m_pGameScoreView->m_szWndSize.cx;
			scorerect.top = wndrect.top + ((rect.bottom-rect.top)-m_pGameScoreView->m_szWndSize.cy)/2-50;
			//scorerect.bottom = scorerect.top + m_pGameScoreView->m_szWndSize.cy;
			m_pGameScoreView->SetWindowPos(NULL,scorerect.left, scorerect.top, m_pGameScoreView->m_szWndSize.cx, m_pGameScoreView->m_szWndSize.cy, 0);
			m_pGameScoreView->ShowWindow(SW_SHOW);
		}
	}
}

//播放等待动画
void CGameFrameDlg::PlayWaitFlash(bool bIsPlay)
{
	m_pGameFrameView->PlayGameWaitFlash(bIsPlay);
}

// 安全显示或隐藏窗口
void CGameFrameDlg::SafeShowOrHideWnd(CWnd* pWnd, int nCmdShow)
{
	if(pWnd && pWnd->GetSafeHwnd())
	{
		pWnd->ShowWindow(nCmdShow);
	}
}

// 安全打亮或打灰窗口
void CGameFrameDlg::SafeEnableWnd(CWnd* pWnd, BOOL bEnable)
{
	if(pWnd && pWnd->GetSafeHwnd())
	{
		pWnd->EnableWindow(bEnable);
	}
}

void CGameFrameDlg::DrawFrameView(CDC * pDC, int nWidth, int nHeight)
{
	if (NULL != m_pGameTaskDlg)
	{
		m_pGameTaskDlg->DrawBackground(pDC, nWidth, nHeight, GAME_FRAME_DLG_HEIGHT);
	}

	if (NULL != m_pUserInfoDlg)
	{
		m_pUserInfoDlg->DrawBackground(pDC, nWidth, nHeight, GAME_FRAME_DLG_HEIGHT);
	}

	if (NULL != m_pGameFrameControl)
	{
		m_pGameFrameControl->DrawBackground(pDC, nWidth, nHeight, GAME_FRAME_DLG_HEIGHT);
	}
}

//新手礼包	Author<cxf>; DateTime<2012-01-16>
void CGameFrameDlg::OnEventNewGift(CMD_GF_NEW_GIFT * pNewGift)
{
	if (IsLookonMode()) return;

	if (NULL != pNewGift)
	{
		TCHAR chTipInfo[MAX_PATH]={0};
		switch(pNewGift->wGiftType)
		{
		case 3:	//升级送宝石
			{
				m_pNewGiftFlash->InitDataRes(pNewGift);
				m_pNewGiftFlash->SetNum(pNewGift->iGem);//赠送宝石
				sprintf(chTipInfo, "恭喜您完成升级送宝石任务！系统将赠送%d宝石。", pNewGift->iGem);
				InsertSystemString(chTipInfo);
				break;
			}
		case 2:	// 在线有礼
			{
				m_pNewGiftFlash->InitDataRes(pNewGift);
				m_pNewGiftFlash->SetNum(pNewGift->iGold);//赠送金币
				sprintf(chTipInfo, "恭喜您完成%d分钟在线有礼任务！系统将赠送%d金币。", (pNewGift->iGiftTime)/60, pNewGift->iGold);
				InsertSystemString(chTipInfo);
				break;
			}
		case 1: // 开门红
			{
				m_pNewGiftFlash->InitDataRes(pNewGift);
				sprintf(chTipInfo, "恭喜您完成开门红任务！系统将赠送%d宝石。", pNewGift->iGem);
				InsertSystemString(chTipInfo);
				break;
			}
		default: break;
		}
		//更新用户数据
		if (NULL != m_pUserInfoDlg)
		{
			m_pUserInfoDlg->UpdateUserData(pNewGift);
		}
	}
}

void CGameFrameDlg::OnEventOnlineKeepTime(CMD_GF_ONLINE * pOnline)
{
	if (IsLookonMode()) return;
	if (NULL != pOnline && NULL != m_pGameFrameView)
	{
		if (GetMeChairID() == pOnline->dwChairID)
		{
			m_pGameFrameView->OnlineKeepTime(pOnline->iToDayOnlineTime, pOnline->bStart);
		}
	}
}