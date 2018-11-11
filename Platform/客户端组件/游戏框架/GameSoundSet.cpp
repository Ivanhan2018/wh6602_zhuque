// GameSoundSet.cpp : 实现文件
//

#include "stdafx.h"
#include "GameSoundSet.h"
#include "GameFrameDlg.h"
#include ".\gamesoundset.h"

#include <fstream>

//ID
//关闭游戏, Y:是,N:否
const int	I_CLOSE_GAME_Y_ID		=	1000;
const int	I_CLOSE_GAME_N_ID		=	1001;
//开启游戏, Y:是,N:否
const int	I_OPEN_GAME_Y_ID		=	1002;
const int	I_OPEN_GAME_N_ID		=	1003;
//关闭音乐, Y:是,N:否
const int	I_CLOSE_MUSIC_Y_ID		=	1004;
const int	I_CLOSE_MUSIC_N_ID		=	1005;
//开启音乐, Y:是,N:否
const int	I_OPEN_MUSIC_Y_ID		=	1006;
const int	I_OPEN_MUSIC_N_ID		=	1007;
//OK
const int	I_GAME_OK_BT			=	1010;


// CGameSoundSet 对话框

IMPLEMENT_DYNAMIC(CGameSoundSet, CDialog)

CGameSoundSet::CGameSoundSet(CWnd* pParent)
				: CDialog(CGameSoundSet::IDD, pParent), m_pGameFrameDlg(NULL)
{
}


CGameSoundSet::~CGameSoundSet()
{
	m_pGameFrameDlg = NULL;
}

void CGameSoundSet::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_GAME, m_GamePicCtl);
	DDX_Control(pDX, IDC_STATIC_MUSIC, m_MusicPicCtl);
}


BEGIN_MESSAGE_MAP(CGameSoundSet, CDialog)
	ON_MESSAGE(WM_BITMAPSLIDER_MOVED, OnBitmapSliderMoved)
	ON_MESSAGE(WM_BITMAPSLIDER_MOVING, OnBitmapSliderMoving)

	ON_BN_CLICKED(I_CLOSE_GAME_N_ID, OnCloseGameN)
	ON_BN_CLICKED(I_OPEN_GAME_N_ID, OnOpenGameN)
	ON_BN_CLICKED(I_CLOSE_MUSIC_N_ID, OnCloseMusicN)
	ON_BN_CLICKED(I_OPEN_MUSIC_N_ID, OnOpenMusicN)
	ON_BN_CLICKED(I_GAME_OK_BT, OnCloseWnd)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()


// CGameSoundSet 消息处理程序

BOOL CGameSoundSet::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	
	//游戏音效
	m_GamePicCtl.SetBitmapChannel( IDB_MP_CHANNEL, IDB_MP_CHANNEL_ACTIVE );
	m_GamePicCtl.SetBitmapThumb( IDB_MP_THUMB, IDB_MP_THUMB_ACTIVE, TRUE );
	m_GamePicCtl.DrawFocusRect( FALSE );
	m_GamePicCtl.SetRange( 0, 100 );
	m_GamePicCtl.SetPos( 0 );
	m_GamePicCtl.SetMarginLeft( 8 );
	m_GamePicCtl.SetMarginRight( 8 );
	m_GamePicCtl.SetPageSize( 10 );
	m_GamePicCtl.SetMargin( 0, 0, 0, 0 );

	//背景音乐
	m_MusicPicCtl.SetBitmapChannel( IDB_MP_CHANNEL, IDB_MP_CHANNEL_ACTIVE );
	m_MusicPicCtl.SetBitmapThumb( IDB_MP_THUMB, IDB_MP_THUMB_ACTIVE, TRUE );
	m_MusicPicCtl.DrawFocusRect( FALSE );
	m_MusicPicCtl.SetRange( 0, 100 );
	m_MusicPicCtl.SetPos( 0 );
	m_MusicPicCtl.SetMarginLeft( 8 );
	m_MusicPicCtl.SetMarginRight( 8 );
	m_MusicPicCtl.SetPageSize( 10 );
	m_MusicPicCtl.SetMargin( 0, 0, 0, 0 );

	TCHAR szResourcePath[MAX_PATH];	GetCurrentDirectory(MAX_PATH,szResourcePath);
	CString strPath(szResourcePath);
	strPath.Append("\\data\\gameframe\\plaza\\");


	//添加图片资源
	CRect rcCreate(0, 0, 0, 0);
	//关闭游戏
	m_CloseGY.Create(TEXT(""),WS_CHILD|WS_VISIBLE|WS_CLIPSIBLINGS,rcCreate,this,I_CLOSE_GAME_Y_ID);
	m_CloseGY.SetButtonImage(TEXT(strPath + "openS.png"), false, 4);
	m_CloseGY.SetWindowPos(NULL, 194, 38, 18, 18, SWP_HIDEWINDOW);
	m_CloseGN.Create(TEXT(""),WS_CHILD|WS_VISIBLE|WS_CLIPSIBLINGS,rcCreate,this,I_CLOSE_GAME_N_ID);
	m_CloseGN.SetButtonImage(TEXT(strPath + "closeS.png"), false, 4);
	m_CloseGN.SetWindowPos(NULL, 194, 38, 18, 18, SWP_HIDEWINDOW);

	//开启游戏
	m_OpenGY.Create(TEXT(""),WS_CHILD|WS_VISIBLE|WS_CLIPSIBLINGS,rcCreate,this,I_OPEN_GAME_Y_ID);
	m_OpenGY.SetButtonImage(TEXT(strPath + "openS.png"), false, 4);
	m_OpenGY.SetWindowPos(NULL, 120, 38, 18, 18, SWP_HIDEWINDOW);
	m_OpenGN.Create(TEXT(""),WS_CHILD|WS_VISIBLE|WS_CLIPSIBLINGS,rcCreate,this,I_OPEN_GAME_N_ID);
	m_OpenGN.SetButtonImage(TEXT(strPath + "closeS.png"), false, 4);
	m_OpenGN.SetWindowPos(NULL, 120, 38, 18, 18, SWP_HIDEWINDOW);

	//关闭音乐
	m_CloseMY.Create(TEXT(""),WS_CHILD|WS_VISIBLE|WS_CLIPSIBLINGS,rcCreate,this,I_CLOSE_MUSIC_Y_ID);
	m_CloseMY.SetButtonImage(TEXT(strPath + "openS.png"), false, 4);
	m_CloseMY.SetWindowPos(NULL, 194, 102, 18, 18, SWP_HIDEWINDOW);
	m_CloseMN.Create(TEXT(""),WS_CHILD|WS_VISIBLE|WS_CLIPSIBLINGS,rcCreate,this,I_CLOSE_MUSIC_N_ID);
	m_CloseMN.SetButtonImage(TEXT(strPath + "closeS.png"), false, 4);
	m_CloseMN.SetWindowPos(NULL, 194, 102, 18, 18, SWP_HIDEWINDOW);

	//开启音乐
	m_OpenMY.Create(TEXT(""),WS_CHILD|WS_VISIBLE|WS_CLIPSIBLINGS,rcCreate,this,I_OPEN_MUSIC_Y_ID);
	m_OpenMY.SetButtonImage(TEXT(strPath + "openS.png"), false, 4);
	m_OpenMY.SetWindowPos(NULL, 120, 102, 18, 18, SWP_HIDEWINDOW);
	m_OpenMN.Create(TEXT(""),WS_CHILD|WS_VISIBLE|WS_CLIPSIBLINGS,rcCreate,this,I_OPEN_MUSIC_N_ID);
	m_OpenMN.SetButtonImage(TEXT(strPath + "closeS.png"), false, 4);
	m_OpenMN.SetWindowPos(NULL, 120, 102, 18, 18, SWP_HIDEWINDOW);

	CString strLclPth, strFlPth;
	GetModuleFileName(NULL, strLclPth.GetBufferSetLength(256), 256);
	strFlPth = strLclPth.Left(strLclPth.ReverseFind('\\')+1)+ TEXT("config\\Common.ini");
	UINT uiReturn = GetPrivateProfileInt(TEXT("Sound"), TEXT("EnableSound"), 1, strFlPth);
	if (uiReturn>0)
	{//开启声音
		OnOpenGame();
	}
	else
	{//关闭声音
		OnCloseGame();
	}
	uiReturn = GetPrivateProfileInt(TEXT("Sound"), TEXT("EnableBGMusic"), 1, strFlPth);
	if (uiReturn>0)
	{//开启音乐
		OnOpenMusic();
	}
	else
	{//关闭音乐
		OnCloseMusic();
	}

	//声音音量值
	uiReturn = GetPrivateProfileInt(TEXT("Sound"), TEXT("SoundVol"), 0, strFlPth);
	if (uiReturn>=100)
	{
		uiReturn = 100;
	}
	m_GamePicCtl.SetPos(uiReturn);
	//背景音量值
	uiReturn = GetPrivateProfileInt(TEXT("Sound"), TEXT("BGMusicVol"), 0, strFlPth);
	if (uiReturn>=100)
	{
		uiReturn = 100;
	}
	m_MusicPicCtl.SetPos(uiReturn);


	//OK按钮
	m_btOK.Create(TEXT(""),WS_CHILD|WS_VISIBLE|WS_CLIPSIBLINGS,rcCreate,this,I_GAME_OK_BT);
	m_btOK.SetButtonImage(TEXT(strPath + "btsetok.png"), false, 4);
	m_btOK.SetWindowPos(NULL, 114, 160, 74, 33, SWP_SHOWWINDOW);

	//背景图
	m_BackImage.LoadImage(TEXT(strPath + "gamesetback.png"));
	if (!m_BackImage.IsNull())
	{
		SetImageRgn(m_hWnd, &m_BackImage, SIRT_FULL_DOT_LESS);
		SetWindowPos(NULL, 0, 0, m_BackImage.GetWidth(), m_BackImage.GetHeight(), SWP_NOMOVE); 
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CGameSoundSet::OnCloseGame()
{
	//关闭游戏否
	m_CloseGN.ShowWindow(SW_HIDE);
	m_OpenGY.ShowWindow(SW_HIDE);

	m_CloseGY.ShowWindow(SW_SHOW);
	m_OpenGN.ShowWindow(SW_SHOW);
}
void CGameSoundSet::OnOpenGame()
{
	//开启游戏否
	m_OpenGN.ShowWindow(SW_HIDE);
	m_CloseGY.ShowWindow(SW_HIDE);

	m_OpenGY.ShowWindow(SW_SHOW);
	m_CloseGN.ShowWindow(SW_SHOW);
}
void CGameSoundSet::OnCloseMusic()
{
	//关闭音乐否
	m_CloseMN.ShowWindow(SW_HIDE);
	m_OpenMY.ShowWindow(SW_HIDE);

	m_CloseMY.ShowWindow(SW_SHOW);
	m_OpenMN.ShowWindow(SW_SHOW);
}
void CGameSoundSet::OnOpenMusic()
{
	//开启音乐否
	m_OpenMN.ShowWindow(SW_HIDE);
	m_CloseMY.ShowWindow(SW_HIDE);

	m_OpenMY.ShowWindow(SW_SHOW);
	m_CloseMN.ShowWindow(SW_SHOW);
}

//设置窗口指针
void CGameSoundSet::SetGameFrameDlgPtr(CGameFrameDlg* pGameFrameDlg)
{
	if (NULL != pGameFrameDlg)
	{
		m_pGameFrameDlg = pGameFrameDlg;
	}
}


LRESULT CGameSoundSet::OnBitmapSliderMoved(WPARAM wParam, LPARAM lParam)
{
	switch( wParam ) 
	{
	case IDC_STATIC_GAME:
		{//游戏音效

		}
		break;
	case IDC_STATIC_MUSIC:
		{//背景音乐

		}
		break;
	default:break;
	}
	return 0;
}

LRESULT CGameSoundSet::OnBitmapSliderMoving(WPARAM wParam, LPARAM lParam)
{
	switch( wParam ) 
	{
	case IDC_STATIC_GAME:
		{//游戏音效
			if (NULL != m_pGameFrameDlg)
			{
				int iGameVolue = m_GamePicCtl.GetPos();
				m_pGameFrameDlg->SetSoundVol(iGameVolue);
			}
		}
		break;
	case IDC_STATIC_MUSIC:
		{//背景音乐
			if (NULL != m_pGameFrameDlg)
			{
				int iGameVolue = m_MusicPicCtl.GetPos();
				m_pGameFrameDlg->SetBGMusicVol(iGameVolue, 0);
			}
		}
		break;
	default:break;
	}
	return 0;
}
void CGameSoundSet::OnCloseGameN()
{
	
	OnCloseGame();
	if (NULL != m_pGameFrameDlg)
	{
		m_pGameFrameDlg->EnableSound(false);
	}
}

void CGameSoundSet::OnOpenGameN()
{
	
	OnOpenGame();
	if (NULL != m_pGameFrameDlg)
	{
		m_pGameFrameDlg->EnableSound(true);
	}
}

void CGameSoundSet::OnCloseMusicN()
{
	
	OnCloseMusic();
	if (NULL != m_pGameFrameDlg)
	{
		m_pGameFrameDlg->EnableBGMusic(false, 0);
		m_pGameFrameDlg->EnableBGMusic(false, 1);
	}
}

void CGameSoundSet::OnOpenMusicN()
{
	
	OnOpenMusic();
	if (NULL != m_pGameFrameDlg)
	{
		m_pGameFrameDlg->EnableBGMusic(true, 0);
		m_pGameFrameDlg->EnableBGMusic(true, 1);
	}
}

//关闭窗口
void CGameSoundSet::OnCloseWnd()
{
	OnOK();
}

//
//
//BOOL CGameSoundSet::SetImageRgn(CPngImage * pImage)
//{
//	if (NULL==pImage) return FALSE;
//	if (pImage->IsNull()) return FALSE;
//	long wWidth = pImage->GetWidth();
//	long wHeight = pImage->GetHeight();
//
//	CRgn rgnImage, tempRgn;
//	rgnImage.CreateRectRgn(0, 0, 0, 0);
//
//	//std::ofstream of("a.txt");//	//std::string  str;
//
//	int leftX = 0;
//	int rightX = 0;
//	for (int i=wHeight-1; i>=0; --i)
//	{
//		for (int j = 0; j<wWidth; ++j)
//		{
//			//of << "...................\n.";
//			leftX = j;
//			rightX = j;
//			for (int k=j; k<wWidth; ++k)  //记住左边的x
//			{
//				BYTE byValue = pImage->GetAlphaValue(k, i);
//				if (byValue != 0 ||(byValue == 0 && k == wWidth -1))
//				{
//					leftX = k;
//					break;
//				}
//			}
//			for (int k=leftX+1; k<wWidth; ++k)
//			{
//				BYTE byValue = pImage->GetAlphaValue(k, i);
//				if (byValue == 0 || (k == wWidth -1 && byValue!=0))
//				{
//					rightX = k;
//					break;
//				}
//			}
//
//			j = (leftX > rightX ? leftX : rightX) + 1;
//
//
//			//char buf[64]={0};
//			//sprintf(buf, "1-->lx=%i, rx=%i, j=%i, i=%i", leftX, rightX, j, i);
//			//of<< buf<<std::endl;
//			if (rightX > leftX)
//			{
//				//::memset(buf, 0, sizeof(buf));
//				//sprintf(buf, "2-->lx=%i, rx=%i, j=%i, i=%i", leftX, rightX, j, i);
//				//of<< buf<<std::endl;
//				tempRgn.CreateRectRgn(leftX, (wHeight - i-1) , rightX, (wHeight -i));
//				rgnImage.CombineRgn(&rgnImage, &tempRgn, RGN_OR);
//				tempRgn.DeleteObject();
//			}
//			//of<< buf<<std::endl;
//			//of << "----------------------------------------------\n";
//		}
//	}
//
//	if (NULL == rgnImage.m_hObject)
//		return FALSE;
//
//	SetWindowRgn((HRGN)rgnImage.m_hObject, TRUE);
//	rgnImage.DeleteObject();
//	return TRUE;
//}
void CGameSoundSet::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 在此处添加消息处理程序代码
	// 不为绘图消息调用 CImgDialogBase::OnPaint()
	if (!m_BackImage.IsNull())
	{
		m_BackImage.DrawImage(&dc, 0, 0);
	}
}

void CGameSoundSet::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	PostMessage(WM_NCLBUTTONDOWN,HTCAPTION,MAKELPARAM(point.x,point.y));
	CDialog::OnLButtonDown(nFlags, point);
}
