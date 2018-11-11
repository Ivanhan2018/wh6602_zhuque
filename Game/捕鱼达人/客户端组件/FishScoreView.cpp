#include "StdAfx.h"
#include "FishScoreView.h"

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CFishScoreView, CDialog)
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_LBUTTONDOWN()
	ON_WM_SIZE()
	ON_WM_TIMER()
END_MESSAGE_MAP()

#define IDC_EDIT_ID 1009
////按钮标识
//enum enViewButtonsID
//{
//	enViewButtonsID_Begin=100,
//
//	IDC_QUIT_GAME,											//退出游戏
//	IDC_CALCSCORE,											//结算并退出
//	IDC_CONTINUE_GAME,										//继续游戏
//
//	enViewButtonsID_End
//};

//定时标识
enum enTimerID
{
	enTimerID_Begin=100,

	IDI_ALPHA_DEPTH,										//透明定时

	enTimerID_End
};
//////////////////////////////////////////////////////////////////////////

//构造函数
CFishScoreView::CFishScoreView(CWnd	*pViewWnd, LONG lCellScore):CDialog(IDD_SCORE)
{
	//设置变量
	m_hRGN=NULL;
	m_pViewWnd=pViewWnd;
	ZeroMemory(m_wCptrFishCount,sizeof(m_wCptrFishCount));
	ZeroMemory(m_byAlphaDepth,sizeof(m_byAlphaDepth));
	m_lBulletCount = 0L;
    m_lCoinCount=0L;	
	m_AlphaIndex=enFishKind_1;
	m_bShowNumber=false;
	m_lCellScore=lCellScore;

	//加载图片
	HINSTANCE hInstance=AfxGetInstanceHandle();
	m_PngBackUp.LoadImage(hInstance,TEXT("GAME_END_UP"));
	m_PngBackDown.LoadImage(hInstance,TEXT("GAME_END_DOWN"));
	m_PngNumber.LoadImage(hInstance,TEXT("GAME_END_NUMBER"));
	m_PngNumberScore.LoadImage(hInstance,TEXT("GAME_END_SCORE_NUMBER"));
	m_PngFish.LoadImage(hInstance,TEXT("GAME_END_FISH"));

	return;
}

//析构函数
CFishScoreView::~CFishScoreView()
{
}

//建立消息
int CFishScoreView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (__super::OnCreate(lpCreateStruct)==-1) return -1;

	return 0;
}


//鼠标消息
void CFishScoreView::OnLButtonDown(UINT nFlags, CPoint Point)
{
	__super::OnLButtonDown(nFlags,Point);

	//消息模拟
	PostMessage(WM_NCLBUTTONDOWN,HTCAPTION,MAKELPARAM(Point.x,Point.y));	

	return;
}

//重画函数
void CFishScoreView::OnPaint() 
{
	//绘画DC
	CPaintDC dc(this);

	//创建判断
	if (m_ImageFish.IsNull())
	{
		//创建图片
		m_ImageFish.Create(m_PngFish.GetWidth(),m_PngFish.GetHeight(),24);

		//拷贝图片
		HDC hDCBuffer=m_ImageFish.GetDC();
		CDC *pDCBuffer=CDC::FromHandle(hDCBuffer);
		pDCBuffer->FillSolidRect(0,0,m_PngFish.GetWidth(),m_PngFish.GetHeight(),RGB(12,34,60));
		m_PngFish.DrawImage(pDCBuffer,0,0);
		m_ImageFish.ReleaseDC();
	}

	//缓冲资源
	CDC dcMemory;
	CBitmap bmpMemory;

	//获取大小
	CRect rcClient;
	GetClientRect(rcClient);

	//创建资源
	dcMemory.CreateCompatibleDC(&dc);
	bmpMemory.CreateCompatibleBitmap(&dc,rcClient.Width(),rcClient.Height());
	CBitmap *pOldBmp=dcMemory.SelectObject(&bmpMemory);

	//绘画背景
	dcMemory.FillSolidRect(rcClient,RGB(0,23,35));
	m_PngBackUp.DrawImage(&dcMemory,0,0);
	m_PngBackDown.DrawImage(&dcMemory,0,rcClient.Height()-m_PngBackDown.GetHeight());

    //鱼群变量
	CImageHandle ImageHandleFish(&m_ImageFish);
	int nDrawPosX=55,nDrawPosY=48;
	int nFishPosX=0,nFishPosY=0;
	int nFishWidth=m_ImageFish.GetWidth()/13,nFishHeight=m_ImageFish.GetHeight()/2;
	int nDrawSpaceX=(rcClient.Width()-110)/3;
	int nDrawSpaceY=152/4;

	//绘画鱼群
	for (enFishKind FishKind=enFishKind_1; FishKind<enFishKind_No; FishKind=enFishKind(FishKind+1))
	{
		//设置索引
		int nFishIndex=FishKind;
		if (enFishKind_12==FishKind) nFishIndex+=1;

		//捕获判断
		if (m_wCptrFishCount[FishKind]>0 && m_byAlphaDepth[FishKind]>0)
		{
			//显示鱼群
			m_ImageFish.AlphaBlend(dcMemory.GetSafeHdc(),nDrawPosX+(FishKind%3)*nDrawSpaceX,nDrawPosY+(FishKind/3)*nDrawSpaceY,
				nFishWidth,nFishHeight,nFishIndex*nFishWidth,0,nFishWidth,nFishHeight,m_byAlphaDepth[FishKind]);

			//显示数字
			if (m_byAlphaDepth[FishKind]>=255) 
			{
				DrawNumber(&dcMemory,nDrawPosX+(FishKind%3)*nDrawSpaceX+50,nDrawPosY+(FishKind/3)*nDrawSpaceY+13,m_wCptrFishCount[FishKind],true);
			}
		}
		else
		{
			m_ImageFish.BitBlt(dcMemory.GetSafeHdc(),nDrawPosX+(FishKind%3)*nDrawSpaceX,nDrawPosY+(FishKind/3)*nDrawSpaceY,
				nFishWidth,nFishHeight,nFishIndex*nFishWidth,1*nFishHeight);
		}
	}

	//金币数目
	if (true==m_bShowNumber)
	{
		long _lAllMoney = m_lCoinCount+m_lBulletCount*m_lCellScore;
		//总计金币
		DrawNumber(&dcMemory, 145, 228, _lAllMoney);
		//宝箱个数 1宝箱 = 100金币
		DrawNumber(&dcMemory, 305, 228, _lAllMoney / 100);
		//送多少个宝箱 这里是个输入框吗？
		DrawNumber(&dcMemory, 145, 288, _lAllMoney / 100);
		//送给谁 这里是个输入框

		//剩余多少个
		DrawNumber(&dcMemory,145,328, _lAllMoney % 100);
	}
	
	////设置字体
	//CFont DrawFont;
	//DrawFont.CreateFont(-12,0,0,0,FW_BOLD,0,0,0,134,3,2,ANTIALIASED_QUALITY,2,TEXT("宋体"));
	//CFont * pOldFont=dcMemory.SelectObject(&DrawFont);
	//dcMemory.SetTextColor(RGB(225,227,161));
	//dcMemory.SetBkMode(TRANSPARENT);

	//绘画界面
	CRect rcClip;
	dc.GetClipBox(&rcClip);
	dc.BitBlt(rcClip.left,rcClip.top,rcClip.Width(),rcClip.Height(),&dcMemory,rcClip.left,rcClip.top,SRCCOPY);

	//清除资源
	/*dcMemory.SelectObject(pOldFont);
	DrawFont.DeleteObject();*/
	dcMemory.SelectObject(pOldBmp);
	bmpMemory.DeleteObject();
	dcMemory.DeleteDC();

	return;
}

//初始函数
BOOL CFishScoreView::OnInitDialog()
{
	__super::OnInitDialog();
		//创建按钮
	m_btQuitGame.Create(NULL,WS_CHILD|WS_VISIBLE,CRect(10,10,20,30),this,IDC_QUIT_GAME);
	m_btCalcScore.Create(NULL,WS_CHILD|WS_VISIBLE,CRect(10,10,20,30),this,IDC_CALCSCORE);
	m_btContinueGame.Create(NULL,WS_CHILD|WS_VISIBLE,CRect(10,10,20,30),this,IDC_CONTINUE_GAME);

	//输入框
	m_font.CreateFont(32,16,0,0,0,FALSE,FALSE,0,0,0,0,0,0,_TEXT("宋体")); 
	m_etSendID.Create(WS_CHILD|WS_VISIBLE, CRect(0, 0, 150, 35), this, IDC_EDIT_ID);
	m_etSendID.SetFont(&m_font,TRUE);
	m_font.Detach();
	//m_etSendID.SetWindowText("");

	//按钮图片
	HINSTANCE hInstance=AfxGetInstanceHandle();
	m_btQuitGame.SetButtonImage(IDB_BT_QUIT_GAME_EX,hInstance,5,false);
	m_btCalcScore.SetButtonImage(IDB_BT_CALC_SCORE,hInstance,5,false);
	m_btContinueGame.SetButtonImage(IDB_BT_CONTINUE_GAME,hInstance,5,false);

	//设置大小
	MoveWindow(0,0,486,427);

	//设置区域
	if (NULL!=m_hRGN) DeleteObject(m_hRGN);
	m_hRGN=CreateRoundRectRgn(0,0,486,427,25,25);
	if (NULL!=m_hRGN) SetWindowRgn(m_hRGN,TRUE);

	//居中窗体
	CenterWindow(m_pViewWnd);

	//设置定时
	SetTimer(IDI_ALPHA_DEPTH,10,NULL);

	return TRUE;
}

//命令函数
BOOL CFishScoreView::OnCommand(WPARAM wParam, LPARAM lParam)
{
	//变量定义
	UINT nCommandID=LOWORD(wParam);

	//消息处理
	switch(nCommandID)
	{
	case IDC_QUIT_GAME:
		{
			AfxGetMainWnd()->PostMessage(WM_QUIT_GAME,0,0);

			__super::OnCancel();
			
			return TRUE;
		}
	case IDC_CONTINUE_GAME:
		{
			__super::OnCancel();

			return TRUE;
		}
	case IDC_CALCSCORE:	//结算并退出
		{
			long _lAllMoney = m_lCoinCount+m_lBulletCount*m_lCellScore;	//暂时存在

			char szTemp[30]={0};
			m_etSendID.GetWindowText(szTemp,sizeof(szTemp));	//拿到输入的ID

			m_Info.nBOSS = _lAllMoney / 100;	//暂时的，实际应该从输入框取出来

			m_Info.ID	=atoi(szTemp);

			__super::OnOK();

			AfxGetMainWnd()->PostMessage(WM_QUIT_GAME,0,0);

			return TRUE;
		}		
	}
	return __super::OnCommand(wParam,lParam);
}

//大小消息
void CFishScoreView::OnSize(UINT nType, int cx, int cy)
{
	__super::OnSize(nType, cx, cy);

	//合法判断
	if (NULL==m_btQuitGame.GetSafeHwnd()) return;

	//开始设置
	const UINT uFlags=SWP_NOACTIVATE|SWP_NOZORDER|SWP_NOCOPYBITS|SWP_NOSIZE;
	HDWP hDWP=BeginDeferWindowPos(10);

	//设置位置
	CRect rcButton;
	m_btCalcScore.GetWindowRect(&rcButton);
	int nSpace=(cx-70-rcButton.Width()*3)/2;
	DeferWindowPos(hDWP,m_btQuitGame,NULL,35,cy-45,0,0,uFlags);
	DeferWindowPos(hDWP,m_btCalcScore,NULL,35+1*(rcButton.Width()+nSpace),cy-45,0,0,uFlags);
	DeferWindowPos(hDWP,m_btContinueGame,NULL,35+2*(rcButton.Width()+nSpace),cy-45,0,0,uFlags);
	
	//
	DeferWindowPos(hDWP, m_etSendID, NULL, 290, 282, 0, 0, uFlags);
	
	//结束移动
	EndDeferWindowPos(hDWP);
}

//定时消息
void CFishScoreView::OnTimer(UINT nIDEvent)
{
	//处理定时
	switch (nIDEvent)
	{
	case IDI_ALPHA_DEPTH:
		{
			//修改透明
			if (m_AlphaIndex<enFishKind_No && m_byAlphaDepth[m_AlphaIndex]<255 && m_wCptrFishCount[m_AlphaIndex]>0) 
			{
				m_byAlphaDepth[m_AlphaIndex]+=51;
			}
			else if (m_AlphaIndex>=enFishKind_No-1)
			{
				m_bShowNumber=true;
				KillTimer(IDI_ALPHA_DEPTH);
			}
			else
			{
				while (true)
				{
					//递增索引
					m_AlphaIndex=enFishKind(m_AlphaIndex+1);

					//中断判断
					if (m_AlphaIndex>=enFishKind_No)
					{
						m_bShowNumber=true;
						KillTimer(IDI_ALPHA_DEPTH);

						//更新界面
						InvalidateRect(NULL,FALSE);
						return;
					}
					if (m_wCptrFishCount[m_AlphaIndex]>0) break;					
				}

				//修改透明
				m_byAlphaDepth[m_AlphaIndex]+=51;
			}

			//更新界面
			InvalidateRect(NULL,FALSE);

			return;
		}
	}
	__super::OnTimer(nIDEvent);
}

//绘画数字
void CFishScoreView::DrawNumber(CDC *pDC, int nXPos, int nYPos, LONGLONG lNumber, bool bShowQuantifier)
{
	//获取属性
	const INT nNumberHeight=m_PngNumber.GetHeight();
	const INT nNumberWidth=m_PngNumber.GetWidth()/11;

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

	//绘画号码
	for (LONG i=0;i<lNumberCount;i++)
	{
		//绘画号码
		WORD wCellNumber=WORD(lNumber%10);
		m_PngNumber.DrawImage(pDC,nXDrawPos,nYDrawPos,nNumberWidth,nNumberHeight,wCellNumber*nNumberWidth,0);

		//设置变量
		lNumber/=10;
		nXDrawPos-=(nNumberWidth+nSpace);
	}

	//输出量词
	if (true==bShowQuantifier)
	{
		nXDrawPos=nXPos+(lNumberCount+1)*(nNumberWidth+nSpace);
		m_PngNumber.DrawImage(pDC,nXDrawPos,nYDrawPos,nNumberWidth,nNumberWidth,10*nNumberWidth,0);
	}
}

//绘画数字
void CFishScoreView::DrawNumber(CDC *pDC, int nXPos, int nYPos, LONGLONG lNumber)
{
	//获取属性
	const INT nNumberHeight=m_PngNumberScore.GetHeight();
	const INT nNumberWidth=m_PngNumberScore.GetWidth()/10;

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
	INT nXDrawPos=nXPos+lNumberCount*(nNumberWidth+nSpace)/2;

	//绘画号码
	for (LONG i=0;i<lNumberCount;i++)
	{
		//绘画号码
		WORD wCellNumber=WORD(lNumber%10);
		m_PngNumberScore.DrawImage(pDC,nXDrawPos,nYDrawPos,nNumberWidth,nNumberHeight,wCellNumber*nNumberWidth,0);

		//设置变量
		lNumber/=10;
		nXDrawPos-=(nNumberWidth+nSpace);
	}
}

//设置信息
void CFishScoreView::SetScoreInfo(LONG lBulletCount,WORD wCptrFishCount[enFishKind_No],LONGLONG lCoinCount)//, long lLeaveScore)
{
	//设置信息
	m_lBulletCount = lBulletCount;											//子弹数目
	CopyMemory(m_wCptrFishCount,wCptrFishCount,sizeof(m_wCptrFishCount));	//捕获情况
	m_lCoinCount=lCoinCount;												//剩余的金币数量
}

//weni23 待验证
const CMD_C_BalanceInfo CFishScoreView::GetBalanceInfo()
{
	return m_Info;
}
//////////////////////////////////////////////////////////////////////////
