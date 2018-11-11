#include "StdAfx.h"
#include "resource.h"
#include "GlobalOption.h"
#include "UserInfoView.h"
#include "GameFrameControl.h"

//#include "../../头文件/CharmValueExchange.h"

#include "MemDC.h"

///////////////////////////////////////////////////////////////////////////

//宏定义
#define COLOR_VIEW_BACKGROUND						RGB(173,227,255)								//背景颜色
#define COLOR_PHOTO_FRAM							RGB(3,148,227)									//边框颜色

#define PHOTO_FRAME_WIDTH							103												//图片大小
#define PHOTO_FRAME_HEIGHT							103												//图片大小
#define PHOTO_FRAME_EXCURSION_X						7												//图片偏移
#define PHOTO_FRAME_EXCURSION_Y						9												//图片偏移

#define IDC_EXCHANGE								101												//兑换按钮


//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CUserInfoView, CWnd)
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_EXCHANGE, OnBtnClickedExchange)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////
//构造函数
CUserInfoView::CUserInfoView()
{
	//设置变量
	m_pIUserFaceRes=NULL;
	m_pIClientKernel=NULL;
	m_pCurrentUserData=NULL;

	return;
}

//析构函数
CUserInfoView::~CUserInfoView()
{
}

//设置接口
bool CUserInfoView::SetUserFaceRes(IUnknownEx * pIUnknownEx)
{
	ASSERT(pIUnknownEx!=NULL);
	m_pIUserFaceRes=(IUserFaceRes *)pIUnknownEx->QueryInterface(IID_IUserFaceRes,VER_IUserFaceRes);
	return (m_pIUserFaceRes!=NULL);
}

//更新资源
bool CUserInfoView::UpdateSkinResource()
{
	//获取资源
	HINSTANCE hInstance=g_GlobalOption.m_PlatformResourceModule->GetResInstance();
	tagGameFrameImage &GameFrameImage=g_GlobalOption.m_GameFrameImage;

	m_ImageMaleFace.SetLoadInfo(GameFrameImage.uMaleFace, hInstance, false);
	m_ImageFemaleFace.SetLoadInfo(GameFrameImage.uFemaleFace, hInstance, false);
	m_btExchange.SetButtonImage(GameFrameImage.uBtExchangeCharm, hInstance, false);
	m_ImageUserListTL.SetLoadInfo(IDB_LIST_TL, GetModuleHandle(GAME_FRAME_DLL_NAME), false);
	m_ImageUserListTM.SetLoadInfo(IDB_LIST_TM, GetModuleHandle(GAME_FRAME_DLL_NAME), false);
	m_ImageUserListTR.SetLoadInfo(IDB_LIST_TR, GetModuleHandle(GAME_FRAME_DLL_NAME), false);

	return true;
}

//设置用户
VOID CUserInfoView::SetUserViewInfo(tagUserData const* pCurrentUserData)
{
	//设置用户
	if (m_pCurrentUserData!=pCurrentUserData)
	{
		//设置变量
		m_pCurrentUserData=pCurrentUserData;

		//更新界面
		if (m_hWnd!=NULL) Invalidate(FALSE);
	}

	//更新控件
	UpdateControls();

	return;
}

//绘画函数
VOID CUserInfoView::OnPaint()
{
	CPaintDC dc(this);

	CRect rect;
	GetClientRect(&rect);
	CMemDC dcMem(&dc, rect, true);	// 用单色填充背景

	//设置 DC
	dcMem.SetBkMode(TRANSPARENT);
	dcMem.SetTextColor(RGB(153,204,204));

	//填充背景
	CImageHandle ImageHandleUserListTL(&m_ImageUserListTL);
	CImageHandle ImageHandleUserListTM(&m_ImageUserListTM);
	CImageHandle ImageHandleUserListTR(&m_ImageUserListTR);
	m_ImageUserListTL.BitBlt(dcMem.GetSafeHdc(),0,0);
	for (int nXPos=m_ImageUserListTL.GetWidth(); nXPos<rect.Width(); nXPos+=m_ImageUserListTM.GetWidth())
	{
		m_ImageUserListTM.BitBlt(dcMem.GetSafeHdc(),nXPos,0);
	}
	
	//绘画边框
	CRect rcPhotoFram(PHOTO_FRAME_EXCURSION_X, PHOTO_FRAME_EXCURSION_Y, PHOTO_FRAME_EXCURSION_X+PHOTO_FRAME_WIDTH, PHOTO_FRAME_EXCURSION_Y+PHOTO_FRAME_HEIGHT);
	CPen BorderPen(PS_SOLID,2,COLOR_PHOTO_FRAM);
	CPen * pOldPen=dcMem.SelectObject(&BorderPen);
	dcMem.RoundRect(&rcPhotoFram,CPoint(0,00));
	dcMem.SelectObject(pOldPen);

	//创建字体
	CFont Font;
	VERIFY(Font.CreateFont(12,0,0,0,FW_NORMAL,FALSE,FALSE,0,ANSI_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,DEFAULT_PITCH | FF_SWISS,TEXT("宋体")));             

	CFont *pOldFont = dcMem.SelectObject(&Font);

	//绘画用户
	if (m_pCurrentUserData!=NULL)
	{
		//变量定义
		TCHAR szString[256]=TEXT("");
		tagServerAttribute const* pGameAttribute=m_pIClientKernel->GetServerAttribute();

		//绘画标识
		if ( m_pCurrentUserData->dwCustomFaceVer == 0 )
		{
			//设置大小
			rcPhotoFram.DeflateRect(1, 1, 2, 2);

			if ( m_pCurrentUserData->cbGender != 2 )
			{
				CImageHandle ImageHandleMaleFace(&m_ImageMaleFace);
				m_ImageMaleFace.BitBlt(dcMem.GetSafeHdc(), rcPhotoFram.left, rcPhotoFram.top);
			}
			else
			{
				CImageHandle ImageHandleFemaleFace(&m_ImageFemaleFace);
				m_ImageFemaleFace.BitBlt(dcMem.GetSafeHdc(), rcPhotoFram.left, rcPhotoFram.top);
			}
		}
		else	//用户头像
		{
			ASSERT(m_pIUserFaceRes!=NULL);
			m_pIUserFaceRes->DrawCustomBigFace(&dcMem, rcPhotoFram.left+1, rcPhotoFram.top+1,m_pCurrentUserData->dwUserID,m_pCurrentUserData->dwCustomFaceVer);
		}

		//用户属性
		INT nUserInfoCuont = 6;
		const tagUserData *pMeUserData = m_pIClientKernel->GetMeUserInfo();
		if(pMeUserData!=NULL && m_pCurrentUserData->dwUserID==pMeUserData->dwUserID)
			nUserInfoCuont++;
		for (INT i=0;i<nUserInfoCuont;i++)
		{
			//位置定义
			CRect rcString;
			rcString.SetRect(PHOTO_FRAME_EXCURSION_X+PHOTO_FRAME_WIDTH+10,6+i*20,rect.Width()-5,6+i*20+15);

			TCHAR szString[128];

			//绘画信息
			switch (i)
			{
			case 0:		//用户帐号
				{
					//用户帐号
					_sntprintf(szString,CountArray(szString),TEXT("用户名：%s"),m_pCurrentUserData->szName);
					dcMem.DrawText(szString,lstrlen(szString),&rcString,DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);

					break;
				}
			case 1:
				{
					//输出信息
					if ( m_pCurrentUserData->cbGender == 0 ) 
						_sntprintf(szString,CountArray(szString),TEXT("性别：保密"));
					else if ( m_pCurrentUserData->cbGender == 1 ) 
						_sntprintf(szString,CountArray(szString),TEXT("性别：男"));
					else _sntprintf(szString,CountArray(szString),TEXT("性别：女"));
					dcMem.DrawText(szString,lstrlen(szString),&rcString,DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);

					break;
				}
			case 5:
				{
					dcMem.SetTextColor(RGB(255,0,0));
					//输出信息
					_sntprintf(szString,CountArray(szString),TEXT("魅力：%ld"),m_pCurrentUserData->lLoveliness);
					dcMem.DrawText(szString,lstrlen(szString),&rcString,DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);

					dcMem.SetTextColor(RGB(153,204,204));

					break;
				}
			case 3:
				{
#if 0
					//输出信息
					if ( pGameAttribute->wGameGenre == GAME_GENRE_GOLD )
						_sntprintf(szString,CountArray(szString),TEXT("金币：%I64d"),m_pCurrentUserData->lScore);
					else if (pGameAttribute->wGameGenre == GAME_GENRE_EDUCATE)
						_sntprintf(szString,CountArray(szString),TEXT("练习币：%I64d"),m_pCurrentUserData->lScore);
					else if (pGameAttribute->wGameGenre == GAME_GENRE_MATCH)
						_sntprintf(szString,CountArray(szString),TEXT("赛币：%I64d"),m_pCurrentUserData->lScore);
					else
						_sntprintf(szString,CountArray(szString),TEXT("积分：%I64d"),m_pCurrentUserData->lScore);
					dcMem.DrawText(szString,lstrlen(szString),&rcString,DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);
#endif
					break;
				}
			case 4:
				{
					CGameFrameControl *pGameFrameControl= (CGameFrameControl *)GetParent();
					CUserListView &UserListView = pGameFrameControl->m_UserListView;

					//输出信息
					_sntprintf(szString,CountArray(szString),TEXT("游戏级别：%s"),
						UserListView.m_GameRankManagerHelper->GetLevelDescribe(m_pCurrentUserData->lScore));
					dcMem.DrawText(szString,lstrlen(szString),&rcString,DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);

					break;
				}
			case 2:
				{
					CGameFrameControl *pGameFrameControl= (CGameFrameControl *)GetParent();
					CUserListView &UserListView = pGameFrameControl->m_UserListView;

					//会员信息
					LPCTSTR pszMemberOrder[]={TEXT("非会员"),TEXT("红钻会员"),TEXT("蓝钻会员"),TEXT("黄钻会员"),TEXT("紫钻会员")};
					if (m_pCurrentUserData->cbMemberOrder>=CountArray(pszMemberOrder))break;

					//输出信息
					_sntprintf(szString,CountArray(szString),TEXT("会员级别：%s"),pszMemberOrder[m_pCurrentUserData->cbMemberOrder]);
					dcMem.DrawText(szString,lstrlen(szString),&rcString,DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);

					break;
				}
			case 6:
				{
#if 0
					//输出信息
					_sntprintf(szString,CountArray(szString),TEXT("银行存款：%I64d"),m_pCurrentUserData->lInsureScore);
					dcMem.DrawText(szString,lstrlen(szString),&rcString,DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);
#endif
					break;
				}
			}
		}

		//魅力数目
		dcMem.SetTextColor(RGB(255,0,0));
		CRect rcExchange;
		m_btExchange.GetWindowRect(rcExchange);
		ScreenToClient(rcExchange);
		CString strFlowerCount;
		strFlowerCount.Format(TEXT("%ld"), m_pCurrentUserData->lLoveliness);
		dcMem.TextOut(rcExchange.right+4, rcExchange.top+4, strFlowerCount);
	}

	//绘画界面
	dcMem.draw();

	//清理资源	
	Font.DeleteObject();

	return;
}

//创建消息
INT CUserInfoView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (__super::OnCreate(lpCreateStruct)==-1) return -1;

	CRect rcCreate(0,0,0,0);
	m_btExchange.Create(NULL, WS_CHILD|WS_VISIBLE, rcCreate, this, IDC_EXCHANGE);
	m_btExchange.EnableWindow(FALSE);

	//加载资源
	UpdateSkinResource();

	return 0;
}

//设置内核
void CUserInfoView::SetClientKernel(IClientKernel *pIClientKernel)
{
	m_pIClientKernel = pIClientKernel;
}

void CUserInfoView::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	RectifyControl(cx, cy);
}

//调整函数
void CUserInfoView::RectifyControl(int nWidth, int nHeight)
{
	//调整位置
	HDWP hDwp = BeginDeferWindowPos(20);

	UINT uFlags = SWP_NOCOPYBITS | SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOZORDER;

	CRect rcButton;
	m_btExchange.GetWindowRect(rcButton);

	int nBtPhotoPosX = PHOTO_FRAME_EXCURSION_X + (PHOTO_FRAME_WIDTH-rcButton.Width()) / 2;
	DeferWindowPos(hDwp, m_btExchange, NULL,  10, nHeight- rcButton.Height() - 5, 0, 0, uFlags);

	EndDeferWindowPos(hDwp);

	return;
}

//兑换消息
void CUserInfoView::OnBtnClickedExchange()
{
	//兑换窗口
	const tagUserData *pMeUserData = m_pIClientKernel->GetMeUserInfo();
	//ShowCharmValueExchangeDlg(m_pIClientKernel,NULL,(tagUserData *)pMeUserData);

	return;
}

//更新函数
void CUserInfoView::UpdateUserInfo()
{
	UpdateControls();
	InvalidateRect(NULL);
}

//更新控件
void CUserInfoView::UpdateControls()
{
	if ( m_pCurrentUserData == NULL ) return ;

	//自己判断
	tagUserData const *pMeUserData = m_pIClientKernel->GetMeUserInfo();

	if ( pMeUserData->dwUserID == m_pCurrentUserData->dwUserID && 1 <= pMeUserData->lLoveliness )
		m_btExchange.EnableWindow(TRUE);
	else
		m_btExchange.EnableWindow(FALSE);
}

//获取函数
tagUserData const *CUserInfoView::GetCurrentUser()
{
	return m_pCurrentUserData;
}

///////////////////////////////////////////////////////////////////////////