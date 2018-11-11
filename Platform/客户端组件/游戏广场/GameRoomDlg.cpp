// GameRoomDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "GameRoomDlg.h"
#include ".\gameroomdlg.h"
#include "webpath.h"
#include "PlazaDlg.h"
#include "InitData.h"

// CGameRoomDlg 对话框



//游戏按钮宽与高
const int	I_G_GAMEBT_W	=	45;
const int	I_G_GAMEBT_H	=	45;
const int	I_G_RULEBT_W	=	61;
const int	I_G_RULEBT_H	=	23;

//房间
const int	I_R_GAMEBT_W	=	45;
const int	I_R_GAMEBT_H	=	45;
const int	I_R_RULEBT_W	=	74;
const int	I_R_RULEBT_H	=	23;



IMPLEMENT_DYNAMIC(CGameRoomDlg, CDialog)
CGameRoomDlg::CGameRoomDlg(CWnd* pParent /*=NULL*/)
: CDialog(CGameRoomDlg::IDD, pParent)
{
	m_byGameBTState = 0;
	m_byRuleBTState = 0;

	m_bHovering = false;
	m_bIsDrawFocus = false;
}

CGameRoomDlg::~CGameRoomDlg()
{
	m_memdc.DeleteDC();
	m_ImgBK.DestroyImage();
	m_ImgFT.DestroyImage();
	m_ImgFocus.DestroyImage();
	m_ImgGameRoomFont.DestroyImage();
	m_ImgRoomName.DestroyImage();
	m_ImgLight.DestroyImage();
	m_ImageBTRule.DestroyImage();
	m_ImageBTGame.DestroyImage();
}

BEGIN_MESSAGE_MAP(CGameRoomDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_ERASEBKGND()
	ON_WM_NCMOUSEMOVE()
	ON_WM_MOUSEMOVE()
	ON_MESSAGE(WM_MOUSEHOVER, OnMouseHover) 
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave) 
	ON_WM_DESTROY()
END_MESSAGE_MAP()

void CGameRoomDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BOOL CGameRoomDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	//加载图片
	TCHAR tcTemp[16]={""};
	CString strPath("");
	GetImagePlazaPath(strPath);

	m_ImgLight.LoadImage(TEXT(strPath+"ImageLight.png"));//人数状态
	m_ImgRoomName.LoadImage(TEXT(strPath+"ImageRoomName.png"));//房间名字

	strPath="";
	GetImageDataPath(strPath);
	//数子图片
	m_ImageNumsHuang.SetImageResource(TEXT(strPath+"Number_Huang.png"));
	m_ImageNumsLv.SetImageResource(TEXT(strPath+"Number_Lv.png"));

	//初始DC
	CPaintDC dc(this);
	CBitmap	bmp;
	m_memdc.DeleteDC();
	bmp.CreateCompatibleBitmap(&dc,m_rcClientRect.Width(),m_rcClientRect.Height());
	m_memdc.CreateCompatibleDC(&dc);
	m_memdc.SelectObject(bmp);
	InvalidateRect(NULL);
	bmp.DeleteObject();
	return TRUE;  // return TRUE unless you set the focus to a control
}

//绘画
void CGameRoomDlg::OnPaint()
{
	if (NULL != m_memdc.m_hDC)
	{
		CPaintDC dc(this); // device context for painting
		if (!m_ImgBK.IsNull())
		{//背景
			m_ImgBK.DrawImage(&m_memdc,0,0);
		}

		if (m_bIsDrawFocus)
		{//绘制焦点
			if (!m_ImgFocus.IsNull())
			{
				m_ImgFocus.DrawImage(&m_memdc, 0, 0);
			}
		}

		if (1==m_wMode) 
		{//游戏列表
			if (!m_ImgFT.IsNull())
			{//前景
				if (ACTIVE_ID == m_wKindID)
				{
					m_ImgFT.DrawImage(&m_memdc, 30, 32, m_ImgFT.GetWidth(), m_ImgFT.GetHeight(), 0, 0);
				}
				else
				{
					m_ImgFT.DrawImage(&m_memdc, 36, 34);
				}			
			}

			if (!m_ImageBTGame.IsNull())
			{//游戏按钮
				m_ImageBTGame.DrawImage(&m_memdc, m_rcGameBT.left, m_rcGameBT.top, I_G_GAMEBT_W, I_G_GAMEBT_H, 
					m_byGameBTState*I_G_GAMEBT_W, 0, I_G_GAMEBT_W, I_G_GAMEBT_H);
			}

			if (!m_ImageBTRule.IsNull())
			{//游戏规则
				m_ImageBTRule.DrawImage(&m_memdc, m_rcRuleBT.left, m_rcRuleBT.top, I_G_RULEBT_W, I_G_RULEBT_H, 
					m_byRuleBTState*I_G_RULEBT_W, 0, I_G_RULEBT_W, I_G_RULEBT_H);
			}
		}
		else if(2==m_wMode)
		{//房间列表
			if (ACTIVE_ID != m_wKindID)
			{
				WORD wSort = m_wSortID-1;
				if (!m_ImgFT.IsNull())
				{//前景
					long lWidth=m_ImgFT.GetWidth()/4;
					m_ImgFT.DrawImage(&m_memdc,118,11,lWidth,m_ImgFT.GetHeight(),(wSort>4?0:wSort)*lWidth,0);
				}

				if (!m_ImgRoomName.IsNull())
				{//房间名字
					long lWidth=m_ImgRoomName.GetWidth()/7;
					m_ImgRoomName.DrawImage(&m_memdc,127,14,lWidth,m_ImgRoomName.GetHeight(),wSort*lWidth,0);
				}
				if (!m_ImgLight.IsNull())
				{//人数状态
					long lWidth=m_ImgLight.GetWidth()/3;
					m_ImgLight.DrawImage(&m_memdc,92,125,lWidth,m_ImgLight.GetHeight(),m_wLight*lWidth,0);
				}
				if (!m_ImgGameRoomFont.IsNull())
				{//游戏房间分数文字
					m_ImgGameRoomFont.DrawImage(&m_memdc,18,35);
				}

				CRect rcRect;
				rcRect.left = 55+(m_wKindID != 10 ? 12 : 0);
				rcRect.top = 38;
				rcRect.right = rcRect.left + 33;
				rcRect.bottom = rcRect.top + 17;

				//单元分数
				m_ImageNumsHuang.SetNumAlign(IA_CENTER);
				m_ImageNumsHuang.SetNums(m_lCellScore);
				m_ImageNumsHuang.SetShowRect(rcRect);
				m_ImageNumsHuang.DrawImageNums(&m_memdc);
				//底分
				rcRect.left = 80;
				rcRect.top = 58;
				rcRect.right = rcRect.left + 100;
				rcRect.bottom = rcRect.top + 17;

				m_ImageNumsHuang.SetNumAlign(IA_LEFT);
				m_ImageNumsLv.SetNums(m_lCellScore);
				m_ImageNumsLv.SetShowRect(rcRect);
				m_ImageNumsLv.DrawImageNums(&m_memdc);
				//封顶
				rcRect.top = 78;
				rcRect.bottom = rcRect.top + 17;

				m_ImageNumsLv.SetNums(m_lRestrictScore);
				m_ImageNumsLv.SetShowRect(rcRect);
				m_ImageNumsLv.DrawImageNums(&m_memdc);
				//最高限制
				rcRect.top = 96;
				rcRect.bottom = rcRect.top + 17;

				m_ImageNumsLv.SetNums(m_lEnterMoney);
				m_ImageNumsLv.SetShowRect(rcRect);
				m_ImageNumsLv.DrawImageNums(&m_memdc);


				if (!m_ImageBTRule.IsNull())
				{//游戏规则
					m_ImageBTRule.DrawImage(&m_memdc, m_rcRuleBT.left, m_rcRuleBT.top, I_R_RULEBT_W, I_R_RULEBT_H, 
						m_byRuleBTState*I_R_RULEBT_W, 0, I_R_RULEBT_W, I_R_RULEBT_H);
				}
			}

			if (!m_ImageBTGame.IsNull())
			{//游戏按钮
				m_ImageBTGame.DrawImage(&m_memdc, m_rcGameBT.left, m_rcGameBT.top, I_R_GAMEBT_W, I_R_GAMEBT_H, 
					m_byGameBTState*I_R_GAMEBT_W, 0, I_R_GAMEBT_W, I_R_GAMEBT_H);
			}
		}

		dc.BitBlt(0,0,m_rcClientRect.Width(),m_rcClientRect.Height(),&m_memdc,0,0,SRCCOPY);
	}
}
//设置窗口位置
void CGameRoomDlg::SetWndPos(WORD x, WORD y, WORD w, WORD h)
{
	SetWindowPos(NULL, x, y, w, h, 0);
	m_rcClientRect.left=x;
	m_rcClientRect.top=y;
	m_rcClientRect.right=m_rcClientRect.left+w;
	m_rcClientRect.bottom=m_rcClientRect.top+h;

	//初始DC
	CDC* pDC = GetDC();
	if (NULL != pDC)
	{
		CBitmap	bmp;
		m_memdc.DeleteDC();
		m_memdc.m_hDC = NULL;
		bmp.CreateCompatibleBitmap(pDC,m_rcClientRect.Width(),m_rcClientRect.Height());
		m_memdc.CreateCompatibleDC(pDC);
		m_memdc.SelectObject(bmp);
		bmp.DeleteObject();
	}
	ReleaseDC(pDC);
	pDC = NULL;

	InvalidateRect(NULL);
}
//设置窗口模型
void CGameRoomDlg::SetMode(WORD wMode)
{
	m_wMode=wMode;
}
//设置资源
void CGameRoomDlg::SetResourceInfo(GameRoomRec stGameRoomRec)
{
	CString strPath("");
	GetImagePlazaPath(strPath);//当前大厅资源路径
	m_ImgBK.DestroyImage();
	m_ImgBK.LoadImage(TEXT(strPath+stGameRoomRec.strImgNameBK));//背景图片
	m_ImgFocus.DestroyImage();
	m_ImgFocus.LoadImage(TEXT(strPath+stGameRoomRec.strImgNameFocus));//获取光标图片
	m_ImgFT.DestroyImage();
	m_ImgFT.LoadImage(TEXT(strPath+stGameRoomRec.strImgNameFT));//前景
	m_wKindID = stGameRoomRec.wKindID;//游戏ID
	m_iDifference = stGameRoomRec.iDifference;//按钮坐标位置差异
	CString strEnterGamePath(""),strGameRule("");

	//状态初始化为默认状态
	m_byGameBTState = 0;
	m_byRuleBTState = 0;

	if (1==m_wMode)
	{//游戏大厅
		strEnterGamePath="BTEnterGame.png";
		strGameRule="BTGameRule.png";

		//进入按钮区域
		m_rcGameBT.left = 126/*+m_iDifference*/;
		m_rcGameBT.top = 70;
		m_rcGameBT.right = m_rcGameBT.left + I_G_GAMEBT_W;
		m_rcGameBT.bottom = m_rcGameBT.top + I_G_GAMEBT_H;

		//规则按钮区域
		m_rcRuleBT.left = 34/*+m_iDifference*/;
		m_rcRuleBT.top = 87;
		m_rcRuleBT.right = m_rcRuleBT.left + I_G_RULEBT_W;
		m_rcRuleBT.bottom = m_rcRuleBT.top + I_G_RULEBT_H;
	}
	else if(2==m_wMode)
	{//游戏房间
		strEnterGamePath="BTStartGame.png";
		strGameRule="BTRoomRule.png";

		m_wLight=stGameRoomRec.wLight; //房间人数状态
		m_wSortID=stGameRoomRec.wSortID;//游戏排序编号
		m_lCellScore=stGameRoomRec.lCellScore;//游戏单元积分
		m_lRestrictScore=stGameRoomRec.lRestrictScore;//最大输分限制
		m_lEnterMoney = stGameRoomRec.lEnterMoney;//最低进入金币
		//进入按钮区域
		m_rcGameBT.left = 127+m_iDifference;
		m_rcGameBT.top = 104;
		m_rcGameBT.right = m_rcGameBT.left + I_R_GAMEBT_W;
		m_rcGameBT.bottom = m_rcGameBT.top + I_R_GAMEBT_H;

		//规则按钮区域
		m_rcRuleBT.left = 11;
		m_rcRuleBT.top = 125;
		m_rcRuleBT.right = m_rcRuleBT.left + I_R_RULEBT_W;
		m_rcRuleBT.bottom = m_rcRuleBT.top + I_R_RULEBT_H;
	}

	m_ImageBTGame.DestroyImage();
	m_ImageBTGame.LoadImage(TEXT(strPath+strEnterGamePath));
	m_ImageBTRule.DestroyImage();
	m_ImageBTRule.LoadImage(TEXT(strPath+strGameRule));

	if (ACTIVE_ID == stGameRoomRec.wKindID)
	{
		m_ImageBTRule.DestroyImage();
		m_rcRuleBT.SetRectEmpty();
	}

	//游戏房间文字
	CString strGameRoomtFont("");
	TCHAR tcTemp[16]={""};
	_snprintf(tcTemp,sizeof(tcTemp),"%d",m_wKindID);
	strGameRoomtFont.Append("ImageGameRoomFont_");
	strGameRoomtFont.Append(tcTemp);
	strGameRoomtFont.Append(".png");
	m_ImgGameRoomFont.DestroyImage();
	m_ImgGameRoomFont.LoadImage(TEXT(strPath+strGameRoomtFont));

	InvalidateRect(NULL, FALSE);
}
//显示窗口
void CGameRoomDlg::SetWndShow()
{
	this->ShowWindow(SW_SHOW);
}
//隐藏窗口
void CGameRoomDlg::SetWndHide()
{
	this->ShowWindow(SW_HIDE);
}
//当前路径
void CGameRoomDlg::GetCurrentDirectoryPath(CString &szPath)
{
	szPath="";
	TCHAR szPathTemp[MAX_PATH]={0};
	GetCurrentDirectory(MAX_PATH, szPathTemp);
	szPath.Append(szPathTemp);
}
//大厅资源路径
void CGameRoomDlg::GetImagePlazaPath(CString& strRetValue)
{
	CString strModuleFileName("");
	GetCurrentDirectoryPath(strModuleFileName);
	strModuleFileName.Append("\\data\\plaza\\");
	strRetValue = strModuleFileName;
}
//资源路径
void CGameRoomDlg::GetImageDataPath(CString& strRetValue)
{
	CString strModuleFileName("");
	GetCurrentDirectoryPath(strModuleFileName);
	strModuleFileName.Append("\\data\\");
	strRetValue = strModuleFileName;
}

void CGameRoomDlg::OnBnClickedGameRule()
{
	//CPlazaDlg * plazaDlg=(CPlazaDlg *)GetParent()->GetParent();
	CString strUrl("");
	if (10==m_wKindID)
	{
		//plazaDlg->LoadPlazaGameWeb(9, 7);
		strUrl.Format(CInitData::s_cGameHelpSingle, 10);
	}
	else if (303==m_wKindID)
	{
		//plazaDlg->LoadPlazaGameWeb(10, 8);
		strUrl.Format(CInitData::s_cGameHelpSingle, 303);
	}
	else if (310==m_wKindID)
	{
		//plazaDlg->LoadPlazaGameWeb(11, 9);
		strUrl.Format(CInitData::s_cGameHelpSingle, 310);
	}
	else if (SISEQIU_ID==m_wKindID)
	{
		//plazaDlg->LoadPlazaGameWeb(11, 9);
		strUrl.Format(CInitData::s_cGameHelpSingle, SISEQIU_ID);
	}
	m_GameRuleDlg.SetRuleUrl(strUrl);
	m_GameRuleDlg.DoModal();
	//ShellExecute(NULL,"open",strUrl,"",NULL,SW_SHOW);
}



BOOL CGameRoomDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if ((pMsg->message==WM_KEYDOWN)&&(pMsg->wParam==VK_ESCAPE))
	{
		return TRUE;
	}
	return CDialog::PreTranslateMessage(pMsg);
}

BOOL CGameRoomDlg::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	return TRUE;
	//return CDialog::OnEraseBkgnd(pDC);
}

void CGameRoomDlg::OnBnClickedEnterGame()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CPlazaDlg * PlazaDlg = (CPlazaDlg *) GetParent()->GetParent();
	if (1==m_wMode)
	{//游戏列表
		PlazaDlg->m_RoomListCenterDlg.m_wKindID=m_wKindID;
		PlazaDlg->PostMessage(WM_COMMAND,IDM_CALL_ROOM_LIST,0);
		if (10==m_wKindID)
		{
			PlazaDlg->SwitchTitle(7);//切换标题
		}
		else if (303==m_wKindID)
		{
			PlazaDlg->SwitchTitle(8);//切换标题
		}
		else if (310==m_wKindID)
		{
			PlazaDlg->SwitchTitle(9);//切换标题
		}
		else if (SISEQIU_ID==m_wKindID)
		{
			PlazaDlg->SwitchTitle(14);
		}
		else if (ACTIVE_ID == m_wKindID)
		{
			PlazaDlg->SwitchTitle(15);
		}
	}
	else
	{//房间列表
		PlazaDlg->m_RoomListCenterDlg.m_wSortID=m_wSortID;
		PlazaDlg->PostMessage(WM_COMMAND,IDM_CALL_TABLE_LIST,0);
	}
}
void CGameRoomDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	if (!m_bIsDrawFocus)
	{
		m_bIsDrawFocus = true;
		InvalidateRect(NULL, FALSE);
	}

	if (m_rcGameBT.PtInRect(point))
	{
		if (2 != m_byGameBTState)
		{
			m_byGameBTState = 2;
			//更新区域
			InvalidateRect(&m_rcGameBT, FALSE);
		}
	}
	else
	{
		if (0 != m_byGameBTState)
		{
			m_byGameBTState = 0;
			//更新区域
			InvalidateRect(&m_rcGameBT, FALSE);
		}
	}

	if (m_rcRuleBT.PtInRect(point))
	{
		if (2 != m_byRuleBTState)
		{
			m_byRuleBTState = 2;
			//更新区域
			InvalidateRect(&m_rcRuleBT, FALSE);
		}
	}
	else
	{
		if (0 != m_byRuleBTState)
		{
			m_byRuleBTState = 0;
			//更新区域
			InvalidateRect(&m_rcRuleBT, FALSE);
		}
	}

	if (!m_bHovering)
	{
		m_bHovering = true;
		TRACKMOUSEEVENT TrackMouseEvent;
		TrackMouseEvent.cbSize=sizeof(TrackMouseEvent);
		TrackMouseEvent.dwFlags=TME_LEAVE;
		TrackMouseEvent.hwndTrack=GetSafeHwnd();
		TrackMouseEvent.dwHoverTime=HOVER_DEFAULT;
		_TrackMouseEvent(&TrackMouseEvent);

		TrackMouseEvent.cbSize=sizeof(TrackMouseEvent);
		TrackMouseEvent.dwFlags=TME_HOVER;
		TrackMouseEvent.hwndTrack=GetSafeHwnd();
		TrackMouseEvent.dwHoverTime=HOVER_DEFAULT;
		_TrackMouseEvent(&TrackMouseEvent);

	}

	CDialog::OnMouseMove(nFlags, point);
}

void CGameRoomDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	//进入游戏按钮
	if (m_rcGameBT.PtInRect(point))
	{
		if (1 != m_byGameBTState)
		{
			m_byGameBTState = 1;
			//更新区域
			InvalidateRect(&m_rcGameBT, FALSE);
		}
	}
	//游戏规则按钮
	if (m_rcRuleBT.PtInRect(point))
	{
		if (1 != m_byRuleBTState)
		{
			m_byRuleBTState = 1;
			//更新区域
			InvalidateRect(&m_rcRuleBT, FALSE);
		}
	}

	CDialog::OnLButtonDown(nFlags, point);
}

void CGameRoomDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	//进入游戏按钮
	/*if (m_rcGameBT.PtInRect(point))
	{
	OnBnClickedEnterGame();
	return ;
	}*/
	//游戏规则按钮
	if (m_rcRuleBT.PtInRect(point))
	{
		OnBnClickedGameRule();
		return ;
	}
	//进入游戏
	OnBnClickedEnterGame();

	CDialog::OnLButtonUp(nFlags, point);
}

LRESULT CGameRoomDlg::OnMouseHover(WPARAM wParam, LPARAM lParam)
{
	if (!m_bIsDrawFocus)
	{
		m_bIsDrawFocus = true;
		InvalidateRect(NULL, FALSE);
	}
	return 0;
}
LRESULT CGameRoomDlg::OnMouseLeave(WPARAM wParam, LPARAM lParam)
{
	m_bHovering = false;
	m_byGameBTState = 0;
	m_byRuleBTState = 0;
	m_bIsDrawFocus = false;
	InvalidateRect(NULL, FALSE);

	return 0;
}

void CGameRoomDlg::OnDestroy()
{
	CDialog::OnDestroy();

	m_memdc.DeleteDC();
	m_ImgBK.DestroyImage();
	m_ImgFT.DestroyImage();
	m_ImgFocus.DestroyImage();
	m_ImgGameRoomFont.DestroyImage();
	m_ImgRoomName.DestroyImage();
	m_ImgLight.DestroyImage();
	m_ImageBTRule.DestroyImage();
	m_ImageBTGame.DestroyImage();
}
