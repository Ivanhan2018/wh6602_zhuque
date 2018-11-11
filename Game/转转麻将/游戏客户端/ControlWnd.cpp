#include "StdAfx.h"
#include "GameLogic.h"
#include "ControlWnd.h"
#include "CardControl.h"
#include ".\controlwnd.h"

//////////////////////////////////////////////////////////////////////////

//按钮标识
#define IDC_BT_PENG					100									//碰
#define IDC_BT_BU						101									//补
#define IDC_BT_HU					102									//胡
#define IDC_BT_GIVE_UP				103									//放弃




//#define IDC_CHIHU					100									//吃胡按钮
//#define IDC_GIVEUP					102									//放弃按钮
//
////位置标识
//#define ITEM_WIDTH					90									//子项宽度
//#define ITEM_HEIGHT					55									//子项高度
//#define CONTROL_WIDTH				179									//控制宽度
//#define CONTROL_HEIGHT				41									//控制高度
//
////////////////////////////////////////////////////////////////////////////
//
//BEGIN_MESSAGE_MAP(CControlWnd, CWnd)
//	ON_WM_PAINT()
//	ON_WM_CREATE()
//	ON_WM_SETCURSOR()
//	ON_WM_LBUTTONDOWN()
//	ON_BN_CLICKED(IDC_CHIHU, OnChiHu)
//	ON_BN_CLICKED(IDC_GIVEUP, OnGiveUp)
//END_MESSAGE_MAP()
//
////////////////////////////////////////////////////////////////////////////
//
////构造函数
//CControlWnd::CControlWnd()
//{
//	//配置变量
//	m_cbActionMask=0;
//	m_cbCenterCard=0;
//	m_PointBenchmark.SetPoint(0,0);
//	ZeroMemory(m_cbGangCard,sizeof(m_cbGangCard));
//
//	//状态变量
//	m_cbItemCount=0;
//	m_cbCurrentItem=0xFF;
//
//	//加载资源
//	HINSTANCE hInstance=AfxGetInstanceHandle();
//
//	//设置变量
//	TCHAR szResourcePath[MAX_PATH]="";
//	GetCurrentDirectory(MAX_PATH,szResourcePath);
//	CString strPath(szResourcePath);
//	strPath.Append("\\SparrowZZ\\Picture\\PNG\\");
//	m_ImageControlTop.LoadImage(TEXT(strPath+"CONTROL_TOP.png"));               //弹出框上
//	//m_ImageControlMid.LoadImage(TEXT(strPath+"CONTROL_MID.png"));               //弹出框中
//	m_ImageControlBottom.LoadImage(TEXT(strPath+"CONTROL_BOTTOM.png"));         //弹出框下
//	m_ImageActionExplain.LoadImage(TEXT(strPath+"ACTION_EXPLAIN.png"));         //动作名称
//	m_ImageActionBlack.LoadImage(TEXT(strPath+"ACTIONBLACK.png"));              //牌背面阴影
//
//	m_sgldCardNums = 0;
//	::memset(&m_sgldCardData, 0, sizeof(ShowGangListData));
//
//	//m_ImageControlMid.LoadImage(TEXT(strPath+"CONTROL_MID.png"));               //弹出框中
//	WCHAR* pPath = NULL;
//	CString strMid="";
//	strMid.Append(strPath);
//	strMid.Append("CONTROL_MID.png");
//	pPath = strMid.AllocSysString();
//	m_ImageControlMid.LoadFromFile(pPath);
//
//	strPath.Empty();
//	strPath.Format("%s", szResourcePath);
//	strPath.Append("\\SparrowZZ\\Picture\\Card\\");
//	m_ImageActionCardBack.LoadImage(TEXT(strPath+"ACTIONCARDBACK.png"));        //牌面
//	m_ImageActionCardNum.LoadImage(TEXT(strPath+"ACTIONCARDNUM.png"));          //牌面数据
//	return;
//}
//
////析构函数
//CControlWnd::~CControlWnd()
//{
//}
//
////基准位置
//void CControlWnd::SetBenchmarkPos(int nXPos, int nYPos)
//{
//	//位置变量
//	m_PointBenchmark.SetPoint(nXPos,nYPos);
//
//	//tagGangCardResult card;
//	//card.cbCardCount = 0;
//	//card.cbCardData[0] = 0x04;
//	//card.cbCardData[1] = 0x04;
//	//card.cbCardData[2] = 0x04;
//	//SetControlInfo(0x01, WIK_LEFT, card);
//	//调整控件
//	RectifyControl();
//
//	return;
//}
//
////设置扑克
//void CControlWnd::SetControlInfo(BYTE cbCenterCard, BYTE cbActionMask, tagGangCardResult & GangCardResult)
//{
//	//设置变量
//	m_sgldCardNums = 0;
//	m_cbItemCount=0;
//	m_cbCurrentItem=0xFF;
//	m_cbActionMask=cbActionMask;
//	m_cbCenterCard=cbCenterCard;
//
//	//杠牌信息
//	ZeroMemory(m_cbGangCard,sizeof(m_cbGangCard));
//	for (BYTE i=0;i<GangCardResult.cbCardCount;i++) 
//	{
//		m_cbItemCount++;
//		m_cbGangCard[i]=GangCardResult.cbCardData[i];
//		m_sgldCardData[m_sgldCardNums].byCardData = GangCardResult.cbCardData[i];
//		m_sgldCardData[m_sgldCardNums++].byTag = 0;
//	}
//
//	//计算数目
//	BYTE cbItemKind[4]={WIK_LEFT,WIK_CENTER,WIK_RIGHT,WIK_PENG};
//	for (BYTE i=0;i<CountArray(cbItemKind);i++) 
//		if ((m_cbActionMask&cbItemKind[i])!=0) 
//			m_cbItemCount++;
//
//	if (cbActionMask&WIK_CHI_HU)
//	{
//		m_cbItemCount++;
//	}
//	//按钮控制
//	m_btChiHu.ShowWindow(SW_HIDE);
//	m_btChiHu.EnableWindow(false);
//	m_btGiveUp.ShowWindow(TRUE);
//	//m_btChiHu.EnableWindow(cbActionMask&WIK_CHI_HU);
//
//	//调整控件
//	RectifyControl();
//
//	//显示窗口
//	ShowWindow(SW_SHOW);
//
//	return;
//}
//
////杠补信息
//void CControlWnd::AddGangCardData(const BYTE* pGangCardData, BYTE byCardNums )
//{
//	if (NULL==pGangCardData || 0==byCardNums)
//	{
//		return;
//	}
//
//	for (BYTE i=0; i<byCardNums; i++)
//	{
//		if (0 != pGangCardData[i])
//		{
//			m_sgldCardData[m_sgldCardNums].byCardData = pGangCardData[i];
//			m_sgldCardData[m_sgldCardNums++].byTag = 1;	//杠
//			m_cbItemCount++;
//		}
//	}
//
//	//调整控件
//	RectifyControl();
//
//	//显示窗口
//	ShowWindow(SW_SHOW);
//}
//
////调整控件
//void CControlWnd::RectifyControl()
//{
//	//设置位置
//	CRect rcRect;
//	rcRect.right=m_PointBenchmark.x;
//	rcRect.bottom=m_PointBenchmark.y;
//	rcRect.left=m_PointBenchmark.x-CONTROL_WIDTH;
//	rcRect.top=m_PointBenchmark.y-ITEM_HEIGHT*m_cbItemCount-CONTROL_HEIGHT;
//	if (m_cbItemCount>0)
//		rcRect.top-=5;
//
//	//移动窗口
//	MoveWindow(&rcRect);
//
//	//调整按钮
//	CRect rcButton;
//	m_btGiveUp.GetWindowRect(&rcButton);
//	int nYPos=rcRect.Height()-rcButton.Height()-10;
//	m_btChiHu.SetWindowPos(NULL,rcRect.Width()-rcButton.Width()-18,nYPos,0,0,SWP_NOZORDER|SWP_NOSIZE);
//	m_btGiveUp.SetWindowPos(NULL,rcRect.Width()-rcButton.Width()-60,nYPos,0,0,SWP_NOZORDER|SWP_NOSIZE);
//
//	//CRect rcClient;
//	//GetClientRect(&rcClient);
//	//CRgn CardRegion;
//	//CardRegion.CreateRoundRectRgn(0,0,rcClient.Width(),rcClient.Height(),12,12);
//	//SetWindowRgn(CardRegion,TRUE);
//	//CardRegion.DeleteObject();
//
//	return;
//}
//
//
//void CControlWnd::AddRectRgn(CDC* pDC,CRgn* rgn,CRect* rect,COLORREF col)
//{
//	for(int y=rect->top;y<rect->bottom;y++)
//		for(int x=rect->left;x<rect->right;x++)
//			if (pDC->GetPixel(x,y)==col)
//			{
//				CRgn rgnAdd;
//				rgnAdd.CreateRectRgn (x, y, x+1, y+1);
//				rgn->CombineRgn(rgn, &rgnAdd, RGN_DIFF);
//				rgnAdd.DeleteObject();
//			}
//}
//void CControlWnd::UpdateRgn(CDC * pDC)
//{
//	CRect rcClient;
//	GetClientRect(&rcClient);
//	CRgn rgn;
//	COLORREF col = RGB(0,0,0);  // mask color
//	rgn.CreateRectRgn (0, 0, rcClient.Width(), rcClient.Height());
//
//	CRect rc(0,0,0,0);
//	rc.right = rcClient.Width();
//	rc.bottom = rcClient.Height();
//	AddRectRgn(pDC,&rgn,&rc,col);
//	SetWindowRgn (rgn,TRUE);
//	//SetForegroundWindow();
//}
////
////bool CControlWnd::DrawWindow(CDC * pDC)
////{
////	if (NULL != pDC && NULL != pDC->m_hDC)
////	{
////		CImageHandle ImageHandle(&m_ImageBack);
////		pDC->SetBkMode(TRANSPARENT);
////		pDC->FillSolidRect(0,0,m_ImageBack.GetWidth(),m_ImageBack.GetHeight(),RGB(255,0,255));
////
////		//绘画背景图片
////		CImageHandle HandleCenter(&m_ImageBack);
////		m_ImageBack.AlphaDrawImage(pDC, 0, 0, RGB(255, 0, 255));
////		return true;
////	}
////	return false;
////}
//
////吃胡按钮
//void CControlWnd::OnChiHu()
//{
//	AfxGetMainWnd()->PostMessage(IDM_CARD_OPERATE,WIK_CHI_HU,0);
//	return;
//}
//
////放弃按钮
//void CControlWnd::OnGiveUp()
//{
//	AfxGetMainWnd()->PostMessage(IDM_CARD_OPERATE,WIK_NULL,0);
//	return;
//}
//
////获取位置
//int CControlWnd::GetImageIndex(BYTE cbCardData)
//{
//	//背景判断
//	if (cbCardData==0) 
//		return 0;
//
//	//计算位置
//	BYTE cbValue=cbCardData&MASK_VALUE;
//	BYTE cbColor=(cbCardData&MASK_COLOR)>>4;
//	return (cbColor>=0x03)?(cbValue+27):(cbColor*9+cbValue);
//}
//
//
////重画函数
//void CControlWnd::OnPaint()
//{
//	CPaintDC dc(this);
//
//	//获取位置
//	CRect rcClient;
//	GetClientRect(&rcClient);
//
//	//创建缓冲
//	CDC BufferDC;
//	CBitmap BufferImage;
//	BufferDC.CreateCompatibleDC(&dc);
//	BufferImage.CreateCompatibleBitmap(&dc,rcClient.Width(),rcClient.Height());
//	BufferDC.SelectObject(&BufferImage);
//	BufferDC.SetBkMode(TRANSPARENT);
//	BufferDC.FillSolidRect(0,0,rcClient.Width(),rcClient.Height(),RGB(0,0,0));
//
//	//加载资源
//	int nWidth=m_ImageActionCardNum.GetWidth()/27;
//	int nHeight=m_ImageActionCardNum.GetHeight();
//	//绘画背景
//	ULONG ulHeight = 0;
//	m_ImageControlTop.DrawImage(&BufferDC,0,0);
//	int iCenterW = m_ImageControlMid.GetWidth();
//	int iCenterH = m_ImageControlMid.GetHeight();
//	int iTempS = m_ImageControlTop.GetHeight();
//	int iTempE = rcClient.Height()-m_ImageControlBottom.GetHeight();
//	do 
//	{
//		int iTempHeight = 0;
//		if (iTempE-iTempS >= iCenterH)
//		{
//			iTempHeight = iCenterH;
//			m_ImageControlMid.DrawImage(&BufferDC,0,iTempS);
//		}
//		else
//		{
//			iTempHeight = iTempE-iTempS;
//			m_ImageControlMid.DrawImage(&BufferDC,0,iTempS,iCenterW, iTempHeight, 0, 0,iCenterW, iTempHeight);
//			break;
//		}
//		iTempS += iTempHeight;
//	} while (iTempS<iTempE);
//	m_ImageControlBottom.DrawImage(&BufferDC, 0, rcClient.Height()-m_ImageControlBottom.GetHeight());
//
//	//if (!m_ImageControlMid.IsNull())
//	//{
//	//	int iCenterW = m_ImageControlMid.GetWidth();
//	//	int iCenterH = m_ImageControlMid.GetHeight();
//	//	int iTempS = 0;
//	//	int iTempE = rcClient.Height();
//	//	do 
//	//	{
//	//		m_ImageControlMid.DrawImage(&BufferDC,0,iTempS);
//	//		iTempS += iCenterH;
//	//	} while (iTempS<iTempE);
//	//}
//	
//	//m_ImageControlTop.DrawImage(&BufferDC,0,0);
//	//m_ImageControlBottom.DrawImage(&BufferDC, 0, rcClient.Height()-m_ImageControlBottom.GetHeight());
//
//
//	//变量定义
//	int nYPos=5;
//	BYTE cbCurrentItem=0;
//	BYTE cbExcursion[3]={0,1,2};
//	BYTE cbItemKind[4]={WIK_LEFT,WIK_CENTER,WIK_RIGHT,WIK_PENG};
//
//	//绘画扑克
//	for (BYTE i=0;i<CountArray(cbItemKind);i++)
//	{
//		if ((m_cbActionMask&cbItemKind[i])!=0)
//		{
//			//绘画扑克阴影
//			///if (!m_ImageActionBlack.IsNull())
//			{
//				m_ImageActionBlack.DrawImage(&BufferDC,5,nYPos);
//			}
//
//
//			//绘画扑克
//			for (BYTE j=0;j<3;j++)
//			{
//				BYTE cbCardData=m_cbCenterCard;
//				if (i<CountArray(cbExcursion))
//					cbCardData=m_cbCenterCard+j-cbExcursion[i];
//				m_ImageActionCardBack.DrawImage(&BufferDC,j*28+12+16,nYPos+2);
//				m_ImageActionCardNum.DrawImage(&BufferDC,j*28+10+16,nYPos+7,nWidth,nHeight,(GetImageIndex(cbCardData)-1)*nWidth,0);
//			}
//
//			//计算位置
//			int nXImagePos=0;
//			int nItemWidth=m_ImageActionExplain.GetWidth()/5;
//			if ((m_cbActionMask&cbItemKind[i])&WIK_PENG)
//				nXImagePos=nItemWidth;
//
//			//绘画标志
//			int nItemHeight=m_ImageActionExplain.GetHeight();
//			m_ImageActionExplain.DrawImage(&BufferDC,136,nYPos+5,nItemWidth,nItemHeight,nXImagePos,0);
//
//			//绘画边框
//			if (cbCurrentItem==m_cbCurrentItem)
//			{
//				BufferDC.Draw3dRect(3,nYPos-2,172,ITEM_HEIGHT-4,RGB(255,255,0),RGB(255,255,0));
//			}
//
//			//设置变量
//			++cbCurrentItem;
//			nYPos+=ITEM_HEIGHT;
//		}
//	}
//
//	//补或杠牌
//	for (BYTE i=0; i<m_sgldCardNums; i++)
//	{
//		if (0 != m_sgldCardData[i].byCardData)
//		{
//			//绘画扑克阴影
//			m_ImageActionBlack.DrawImage(&BufferDC,5,nYPos);
//
//			//绘画扑克
//			for (BYTE j=0;j<4;j++)
//			{
//				m_ImageActionCardBack.DrawImage(&BufferDC,j*28+12,nYPos+2);
//				m_ImageActionCardNum.DrawImage(&BufferDC,j*28+10,nYPos+7,nWidth,nHeight,(GetImageIndex(m_sgldCardData[i].byCardData)-1)*nWidth,0);
//			}
//
//			//绘画边框
//			if (cbCurrentItem==m_cbCurrentItem)
//			{
//				BufferDC.Draw3dRect(3,nYPos-2,172,ITEM_HEIGHT-4,RGB(255,255,0),RGB(255,255,0));
//			}
//
//			if (0 == m_sgldCardData[i].byTag)
//			{
//				//绘画补标志
//				int nItemWidth=m_ImageActionExplain.GetWidth()/5;
//				int nItemHeight=m_ImageActionExplain.GetHeight();
//				m_ImageActionExplain.DrawImage(&BufferDC,136,nYPos+5,nItemWidth,nItemHeight,nItemWidth*3,0);
//			}
//			else if (1 == m_sgldCardData[i].byTag)
//			{
//				//绘画杠标志
//				int nItemWidth=m_ImageActionExplain.GetWidth()/5;
//				int nItemHeight=m_ImageActionExplain.GetHeight();
//				m_ImageActionExplain.DrawImage(&BufferDC,132,nYPos+5,nItemWidth,nItemHeight,nItemWidth*2,0);
//			}
//
//			//设置变量
//			cbCurrentItem++;
//			nYPos+=ITEM_HEIGHT;
//		}
//	}
//
//	//胡牌
//	if ((m_cbActionMask&WIK_CHI_HU)!=0)
//	{
//		//绘画扑克阴影
//		m_ImageActionBlack.DrawImage(&BufferDC,5,nYPos);
//
//		//绘画扑克
//		BYTE cbCardData=m_cbCenterCard;
//		BYTE byCardIndex = (GetImageIndex(cbCardData)-1);
//		m_ImageActionCardBack.DrawImage(&BufferDC,12+48,nYPos+2);
//		m_ImageActionCardNum.DrawImage(&BufferDC,10+48,nYPos+7,nWidth,nHeight,byCardIndex*nWidth,0);
//
//		//计算位置
//		int nXImagePos=0;
//		int nItemWidth=m_ImageActionExplain.GetWidth()/5;
//		if (m_cbActionMask&WIK_CHI_HU)
//		{
//			nXImagePos=4*nItemWidth;
//		}
//
//
//		//绘画标志
//		int nItemHeight=m_ImageActionExplain.GetHeight();
//		m_ImageActionExplain.DrawImage(&BufferDC,136,nYPos+5,nItemWidth,nItemHeight,nXImagePos,0);
//
//		//绘画边框
//		if (cbCurrentItem==m_cbCurrentItem)
//		{
//			BufferDC.Draw3dRect(3,nYPos-2,172,ITEM_HEIGHT-4,RGB(255,255,0),RGB(255,255,0));
//		}
//
//		//设置变量
//		nYPos+=ITEM_HEIGHT;
//	}
//
//	UpdateRgn(&BufferDC);
//
//	//绘画界面
//	dc.BitBlt(0,0,rcClient.Width(),rcClient.Height(),&BufferDC,0,0,SRCCOPY);
//
//	//清理资源
//	BufferDC.DeleteDC();
//	BufferImage.DeleteObject();
//
//	return;
//}
//
////建立消息
//int CControlWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
//{
//	if (__super::OnCreate(lpCreateStruct)==-1) return -1;
//
//	//创建按钮
//	CRect rcCreate(0,0,0,0);
//	m_btGiveUp.Create(NULL,WS_CHILD|WS_VISIBLE,rcCreate,this,IDC_GIVEUP);
//	m_btChiHu.Create(NULL,WS_CHILD|WS_VISIBLE|WS_DISABLED,rcCreate,this,IDC_CHIHU);
//
//
//	//设置变量
//	TCHAR szResourcePath[MAX_PATH]="";
//	GetCurrentDirectory(MAX_PATH,szResourcePath);
//	CString strPath(szResourcePath);
//	strPath.Append("\\SparrowZZ\\Picture\\PNG\\");
//	//m_btChiHu.SetButtonImage(TEXT(strPath+"CHIHU.png"),false,5);
//	m_btGiveUp.SetButtonImage(TEXT(strPath+"GIVEUP.png"),false,5);
//
//	return 0;
//}
//
////鼠标消息
//void CControlWnd::OnLButtonDown(UINT nFlags, CPoint Point)
//{
//	__super::OnLButtonDown(nFlags, Point);
//
//	//索引判断
//	if (m_cbCurrentItem!=0xFF)
//	{
//		//变量定义
//		BYTE cbIndex=0;
//		BYTE cbItemKind[4]={WIK_LEFT,WIK_CENTER,WIK_RIGHT,WIK_PENG};
//
//		//类型子项
//		for (BYTE i=0;i<CountArray(cbItemKind);i++)
//		{
//			if (((m_cbActionMask&cbItemKind[i])!=0)&&(m_cbCurrentItem==cbIndex++))
//			{
//				AfxGetMainWnd()->PostMessage(IDM_CARD_OPERATE,cbItemKind[i],m_cbCenterCard);
//				return;
//			}
//		}
//
//		//补或杠牌
//		for (BYTE i=0; i<m_sgldCardNums; i++)
//		{
//			if ((0 != m_sgldCardData[i].byCardData) &&(m_cbCurrentItem==cbIndex++))
//			{
//				BYTE byOperaCode = 9;
//				//if (1 == m_sgldCardData[i].byTag)
//				//{
//				//	byOperaCode = 10;
//				//}
//				AfxGetMainWnd()->PostMessage(IDM_CARD_OPERATE,byOperaCode,m_sgldCardData[i].byCardData);
//			}
//		}
//
//		if (m_cbActionMask&WIK_CHI_HU && (m_cbCurrentItem==cbIndex++))
//		{
//			OnChiHu();
//		}
//
//		////杠牌子项
//		//for (BYTE i=0;i<CountArray(m_cbGangCard);i++)
//		//{
//		//	if ((m_cbGangCard[i]!=0)&&(m_cbCurrentItem==cbIndex++))
//		//	{
//		//		AfxGetMainWnd()->PostMessage(IDM_CARD_OPERATE,WIK_GANG,m_cbGangCard[i]);
//		//		return;
//		//	}
//		//}
//	}
//
//	return;
//}
//
////光标消息
//BOOL CControlWnd::OnSetCursor(CWnd * pWnd, UINT nHitTest, UINT uMessage)
//{
//	//位置测试
//	if (m_cbItemCount!=0)
//	{
//		//获取位置
//		CRect rcClient;
//		CPoint MousePoint;
//		GetClientRect(&rcClient);
//		GetCursorPos(&MousePoint);
//		ScreenToClient(&MousePoint);
//
//		//计算索引
//		BYTE bCurrentItem=0xFF;
//		CRect rcItem(5,5,rcClient.Width()-5,ITEM_HEIGHT*m_cbItemCount+5);
//
//		if (rcItem.PtInRect(MousePoint))
//			bCurrentItem=(BYTE)((MousePoint.y-7)/ITEM_HEIGHT);
//
//		//设置索引
//		if (m_cbCurrentItem!=bCurrentItem)
//		{
//			Invalidate(FALSE);
//			m_cbCurrentItem=bCurrentItem;
//		}
//
//		//设置光标
//		if (m_cbCurrentItem!=0xFF)
//		{
//			SetCursor(LoadCursor(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDC_CARD_CUR)));
//			return TRUE;
//		}
//	}
//
//	return __super::OnSetCursor(pWnd,nHitTest,uMessage);
//}

//////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CControlWnd, CWnd)
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_BT_PENG, OnBtClickPeng)
	ON_BN_CLICKED(IDC_BT_BU, OnBtClickBu)
	ON_BN_CLICKED(IDC_BT_HU, OnBtClickHu)
	ON_BN_CLICKED(IDC_BT_GIVE_UP, OnBtClickGiveup)
END_MESSAGE_MAP()

CControlWnd::CControlWnd()
{
	m_bAction = 0;
	m_bCardData = 0;
}

CControlWnd::~CControlWnd()
{
}

void CControlWnd::SetAction(BYTE byCardData, BYTE byAction)
{
	m_btPeng.EnableWindow(FALSE);
	m_btBu.EnableWindow(FALSE);
	m_bthu.EnableWindow(FALSE);

	m_bAction = byAction;
	m_bCardData = byCardData;

	m_btPeng.EnableWindow((WIK_PENG&m_bAction)?TRUE:FALSE);
	m_btBu.EnableWindow(((WIK_GANG|WIK_FILL)&m_bAction)?TRUE:FALSE);
	m_bthu.EnableWindow((WIK_CHI_HU&m_bAction)?TRUE:FALSE);
	m_btGiveUp.EnableWindow(TRUE);

	//ShowWindow(SW_SHOW);
	SafeShowOrHideWnd(this,SW_SHOW);
}

BOOL CControlWnd::SetImageRgn(HWND hWnd, CPngImage * pImage)
{

#if 0
	if (NULL==pImage) return FALSE;
	if (pImage->IsNull()) return FALSE;
	long wWidth = pImage->GetWidth();
	long wHeight = pImage->GetHeight();

	CRgn rgnImage, tempRgn;
	rgnImage.CreateRectRgn(0, 0, 0, 0);

	//std::ofstream of("a.txt");	//std::string  str;

	int leftX = 0;
	int rightX = 0;
	for (int i=wHeight-1; i>=0; --i)
	{
		for (int j = 0; j<wWidth; ++j)
		{
			//of << "...................\n.";
			leftX = j;
			rightX = j;
			for (int k=j; k<wWidth; ++k)  //记住左边的x
			{
				BYTE byValue = pImage->GetAlphaValue(k, i);
				if (byValue != 0 ||(byValue == 0 && k == wWidth -1))
				{
					leftX = k;
					break;
				}
			}
			for (int k=leftX+1; k<wWidth; ++k)//记住右边的y
			{
				BYTE byValue = pImage->GetAlphaValue(k, i);
				if (byValue == 0 || (k == wWidth -1 && byValue!=0))
				{
					rightX = k;
					break;
				}
			}

			j = (leftX > rightX ? leftX : rightX) + 1;


			//char buf[64]={0};
			//sprintf(buf, "1-->lx=%i, rx=%i, j=%i, i=%i", leftX, rightX, j, i);
			//of<< buf<<std::endl;
			if (rightX > leftX)
			{
				//::memset(buf, 0, sizeof(buf));
				//sprintf(buf, "2-->lx=%i, rx=%i, j=%i, i=%i", leftX, rightX, j, i);
				//of<< buf<<std::endl;
				tempRgn.CreateRectRgn(leftX, (wHeight - i-1) , rightX, (wHeight -i));
				rgnImage.CombineRgn(&rgnImage, &tempRgn, RGN_OR);
				DeleteObject(tempRgn);
			}
			//of<< buf<<std::endl;
			//of << "----------------------------------------------\n";
		}
	}

	if (NULL == rgnImage.m_hObject)
		return FALSE;

	SetWindowRgn(hWnd, (HRGN)rgnImage.m_hObject, TRUE);
	DeleteObject(rgnImage);
#endif

#if 0
	if (NULL==pImage) return FALSE;
	if (pImage->IsNull()) return FALSE;
	INT wWidth = pImage->GetWidth();
	INT wHeight = pImage->GetHeight();
	CBitmap * pbmp;
	CImage imgBuf;
	imgBuf.Create(wWidth, wHeight, 32);
	CDC *pDCTmp = CDC::FromHandle(imgBuf.GetDC());
	pImage->DrawImage(pDCTmp, 0, 0, wWidth, wHeight, 0, 0, wWidth, wHeight);
	pbmp = ((CBitmap *)(&imgBuf));
	DWORD * pData = new DWORD[wWidth * wHeight];
	pbmp->GetBitmapBits((sizeof (DWORD)* wWidth * wHeight), pData);
	imgBuf.ReleaseDC();
	CRgn rgnImage, tempRgn;
	rgnImage.CreateRectRgn(0, 0, 0, 0);
	DWORD *pDataTmp = pData;
	for (UINT h = 0;h < wHeight;++h)
	{
		UINT w = 0, leftX = 0;
		while(w < wWidth)
		{
			while (w++ < wWidth &&(0 == (*pDataTmp++ & 0xff000000)));
			leftX =(w - 1);
			while (w++ < wWidth &&(0 != (*pDataTmp++ & 0xff000000))); 
			tempRgn.CreateRectRgn(leftX, h,(w - 1),(h + 1));
			rgnImage.CombineRgn(&rgnImage, &tempRgn, RGN_OR);
			DeleteObject(tempRgn);
		}
	}

	SafeDeleteArray(pData);
	imgBuf.ReleaseDC();
	imgBuf.Destroy();

	if (NULL == rgnImage.m_hObject)
		return FALSE;

	SetWindowRgn((HRGN)rgnImage.m_hObject, TRUE);
	DeleteObject(rgnImage);

	return TRUE;
#endif

	if (!pImage) 
		return FALSE;
	if (pImage->IsNull()) 
		return FALSE;

	CBitmap* pBmp;
	CImage imgBuf;

	int iWidth = pImage->GetWidth();
	int iHeight = pImage->GetHeight();
	int iCount = 0;

	imgBuf.Create(iWidth, iHeight, 32);
	CDC *pDCTmp = CDC::FromHandle(imgBuf.GetDC());	// 缺少成对出现的 CDC::DeleteTempMap  ???
	pImage->DrawImage(pDCTmp, 0, 0, iWidth, iHeight, 0, 0, iWidth, iHeight);
	pBmp = ((CBitmap *)(&imgBuf));
	DWORD * pData = new DWORD[iWidth * iHeight];
	pBmp->GetBitmapBits((sizeof (DWORD)* iWidth * iHeight),  pData);

	CRgn rgnImage;			// pImage 的 镜像
	CRgn rgnDecrease;		// 要剪掉的区域

	rgnImage.CreateRectRgn(0,  0,  iWidth,  iHeight);
	rgnDecrease.CreateRectRgn(0,  0,  iWidth,  iHeight);

	DWORD *pDataTmp = pData;

	bool		bStart = true;	// 可以做起点
	CPoint	ptStartTrans;		// 透明的起点
	CPoint	ptEndTrans;			// 透明的终点
	int		iPix;		
	int		iPhalanx = 0;		// 方阵的点

	for (int h=0; h<iHeight; ++h)	// 高度
	{
		iPhalanx = iWidth * h;
		bStart = true;				// 可以做起点
		ptStartTrans.SetPoint(0, h);
		ptEndTrans.SetPoint(0, h);
		for(int w=0;  w<iWidth;  w++, pDataTmp++)
		{
			iPix = (*pDataTmp) & 0xff000000;
			if(0 == iPix)	// 完全透明
			{
				if(bStart)
				{
					ptStartTrans.SetPoint(w, h);
					bStart = false;	// 不能做起点

				}
				// 最后一个起点透明 ,直接裁剪
				if(w==(iWidth-1))
				{
					// 裁剪，透明会比非透明的少，所以裁剪透明比叠加非透明会更有效率 ...... 
					rgnDecrease.SetRectRgn(ptEndTrans.x+2, ptEndTrans.y,  iWidth,  ptStartTrans.y+1);	// 要裁剪的区域
					rgnImage.CombineRgn(&rgnImage, &rgnDecrease, RGN_XOR);	// 两个剪裁区域的全部除去公共部分		
					iCount++;
				}
			}
			else	// 不完全透明
			{
				ptEndTrans.SetPoint(w-1, h);

				// 安全距离并且
				if(ptStartTrans.x <= ptEndTrans.x)
				{
					iPix = pData[iPhalanx + ptEndTrans.x ] & 0xff000000;
					if (0==iPix)	// 该点透明
					{
						// 裁剪，透明会比非透明的少，所以裁剪透明比叠加非透明会更有效率 ...... 
						rgnDecrease.SetRectRgn(ptStartTrans.x, ptStartTrans.y,  ptEndTrans.x,  ptEndTrans.y+1);	// 要裁剪的区域
						rgnImage.CombineRgn(&rgnImage, &rgnDecrease, RGN_XOR);	// 	两个剪裁区域的全部除去公共部分	
						iCount++;
					}
				}
				bStart = true;
			}
		}
	}

#if _DEBUG
	CString strTmp;
	strTmp.Format("iCount=%d \r\n", iCount);
	TRACE(strTmp.GetBuffer());
#endif

	SafeDeleteArray(pData);
	imgBuf.ReleaseDC();
	imgBuf.Destroy();

	BOOL bResult = TRUE;

	if (rgnImage.m_hObject)
	{
		::SetWindowRgn(hWnd, (HRGN)rgnImage.m_hObject, TRUE);
		DeleteObject(rgnImage);
	}
	else
	{
		bResult = FALSE;
	}

	DeleteObject(rgnDecrease);

	CDC::DeleteTempMap();	// add by HouGuoJiang 2011-11-25

	return bResult;
}

int CControlWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码
	CString strPicPath;
	strPicPath.Empty();
	TCHAR szResourcePath[MAX_PATH]={0};
	GetCurrentDirectory(MAX_PATH,szResourcePath);
	strPicPath.Append(szResourcePath);
	strPicPath.Append(TEXT("\\SparrowZZ\\Picture\\PNG\\CtrolPanel\\"));

	m_btPeng.Create(NULL, WS_CHILD|WS_VISIBLE, CRect(5,10,0,0), this, IDC_BT_PENG);
	m_btBu.Create(NULL, WS_CHILD|WS_VISIBLE, CRect(5+(1*58),10,0,0), this, IDC_BT_BU);
	m_bthu.Create(NULL, WS_CHILD|WS_VISIBLE, CRect(5+(2*58),10,0,0), this, IDC_BT_HU);
	m_btGiveUp.Create(NULL, WS_CHILD|WS_VISIBLE, CRect(5+(3*58),10,0,0), this, IDC_BT_GIVE_UP);
	m_btPeng.SetButtonImage(strPicPath + TEXT("btPeng.png"), false);
	m_btBu.SetButtonImage(strPicPath + TEXT("btBu.png"), false);
	m_bthu.SetButtonImage(strPicPath + TEXT("btHu.png"), false);
	m_btGiveUp.SetButtonImage(strPicPath + TEXT("btGiveup.png"), false);


	m_pngBackground.LoadImage(strPicPath + TEXT("background.png"));

	if ( !m_pngBackground.IsNull() )
	{
		SetWindowPos(&wndTop, 0, 0, m_pngBackground.GetWidth(), m_pngBackground.GetHeight(), SWP_NOMOVE);
	}
	else
	{
		ASSERT(0);
	}

	SetImageRgn(m_hWnd, &m_pngBackground);
	return 0;
}

void CControlWnd::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 在此处添加消息处理程序代码
	// 不为绘图消息调用 CWnd::OnPaint()
	m_pngBackground.DrawImage(&dc, 0, 0);
}

void CControlWnd::OnBtClickPeng()
{
	if (WIK_PENG&m_bAction)
	{
		AfxGetMainWnd()->PostMessage(IDM_CARD_OPERATE, WIK_PENG, m_bCardData);
	}
	//ShowWindow(SW_HIDE);
	SafeShowOrHideWnd(this,SW_HIDE);
}

void CControlWnd::OnBtClickBu()
{
	if ((WIK_FILL|WIK_GANG)&m_bAction)
	{
		CString strBuffer;
		strBuffer.Format("client:CGameClientDlg::OnBtClickBu() 单击了补 m_bAction %d", m_bAction);
		OutputDebugString(strBuffer);
		AfxGetMainWnd()->PostMessage(IDM_CARD_OPERATE, WIK_FILL, m_bCardData);
	}
	//ShowWindow(SW_HIDE);
	SafeShowOrHideWnd(this,SW_HIDE);
}

void CControlWnd::OnBtClickHu()
{
	if (WIK_CHI_HU&m_bAction)
	{
		AfxGetMainWnd()->PostMessage(IDM_CARD_OPERATE,WIK_CHI_HU,0);
	}
	//ShowWindow(SW_HIDE);
	SafeShowOrHideWnd(this,SW_HIDE);
}

void CControlWnd::OnBtClickGiveup()
{
	AfxGetMainWnd()->PostMessage(IDM_CARD_OPERATE,WIK_NULL,0);
	//ShowWindow(SW_HIDE);
	SafeShowOrHideWnd(this,SW_HIDE);
}

// 安全显示或隐藏窗口
void CControlWnd::SafeShowOrHideWnd(CWnd* pWnd, int nCmdShow)
{
	if(pWnd && pWnd->GetSafeHwnd())
	{
		pWnd->ShowWindow(nCmdShow);
	}
}

//////////////////////////////////////