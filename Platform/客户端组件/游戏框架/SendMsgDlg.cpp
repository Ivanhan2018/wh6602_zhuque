// SendMsgDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "SendMsgDlg.h"
#include ".\sendmsgdlg.h"
#include "GameFrameControl.h"
#include "GlobalOption.h"

//按钮过滤颜色
#define		DEF_BT_COLOR		RGB(255, 0, 255)

//消息响应定时器
#define		IDI_TIME_CHAT_MSG				1010
#define		TIME_CHAT_MSG_INTERVAL			1000


// CSendMsgDlg 对话框

CSendMsgDlg::CSendMsgDlg(): CRgnDialog(IDD_INPUT_MSG_DLG)
{
	m_pGameFrameControl = NULL;
	m_dwLimitChatTimes = 0;
	m_bIsShowGameFrame = true;
	m_bIsShowWndTag = true;
	m_iCountMsgNums = 0;
	m_bIsStartTime=false;
}

CSendMsgDlg::~CSendMsgDlg()
{
}

void CSendMsgDlg::DoDataExchange(CDataExchange* pDX)
{
	__super::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_UP_TAG, m_btWndTag);
	DDX_Control(pDX, IDC_INPUT_EDIT_MSG, m_ChatInput);
	DDX_Control(pDX, IDC_SEND_MSG, m_btSendChat);
	DDX_Control(pDX, IDC_EXP_BT, m_btExpression);
	DDX_Control(pDX, IDC_UP_TAG2, m_btWndTagLight);
}


BEGIN_MESSAGE_MAP(CSendMsgDlg, CRgnDialog)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_MESSAGE(WM_HIT_EXPMESSTION,OnHitExpression)
//	ON_WM_ERASEBKGND()
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CSendMsgDlg 消息处理程序

BOOL CSendMsgDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类

	if (pMsg->message==WM_KEYDOWN)
	{
		if (VK_RETURN == pMsg->wParam)
		{//回车键
			SetGameChatMsg();
			if (!m_bIsShowGameFrame)
			{
				m_iCountMsgNums = 0;
				SetGameChatShow();
			}
			
			return TRUE;
		}
		else if (VK_ESCAPE == pMsg->wParam)
		{
			return TRUE;
		}
	}
	return __super::PreTranslateMessage(pMsg);

	//return CRgnDialog::PreTranslateMessage(pMsg);
}

void CSendMsgDlg::OnPaint()
{
	CRgnDialog::OnPaint();

	if (!m_ImageBuffer.IsNull())
	{
		//变量定义
		CPaintDC dc(this);
		CDC * pDC=CDC::FromHandle(m_ImageBuffer.GetDC());

		int iWndWidth = m_ImageBuffer.GetWidth();
		int iWndHeight = m_ImageBuffer.GetHeight();

		pDC->SetBkMode(TRANSPARENT);
		pDC->FillSolidRect(0,0,iWndWidth,iWndHeight,RGB(0,0,0));

		//绘画背景图片
		CImageHandle HandleCenter(&m_ImageWndBack);
		m_ImageWndBack.AlphaDrawImage(pDC, 0, 0, RGB(255, 0, 255));

		//绘画界面
		m_ImageBuffer.BitBlt(dc,0,0,iWndWidth,iWndHeight,0,0);

		//清理资源
		m_ImageBuffer.ReleaseDC();
		ReleaseDC(pDC);
		pDC = NULL;
	}
	
}

//设置显示
void CSendMsgDlg::SetGameChatShow()
{
	if (NULL != m_pGameFrameControl)
	{
		m_bIsStartTime = false;
		m_bIsShowGameFrame = !m_bIsShowGameFrame;
		m_pGameFrameControl->SetWndShow(m_bIsShowGameFrame);
		HINSTANCE hInstance=GetModuleHandle(GAME_FRAME_DLL_NAME);
		if (true == m_bIsShowGameFrame)
		{//向上
			m_btWndTag.LoadRgnImage(hInstance, IDB_BT_DOWN, DEF_BT_COLOR, 4);
		}
		else
		{//向下
			m_btWndTag.LoadRgnImage(hInstance, IDB_BT_UP, DEF_BT_COLOR, 4);
		}
		m_btWndTagLight.ShowWindow(SW_HIDE);
		m_btWndTag.ShowWindow(SW_SHOW);
		m_btWndTag.InvalidateRect(NULL);
	}
}

//发送消息
void CSendMsgDlg::SetGameChatMsg()
{
	m_iCountMsgNums = 0; 

	//聊天信息
	CString strChatInfo="";
	m_ChatInput.GetWindowText(strChatInfo);
	if (!strChatInfo.IsEmpty())
	{
		////设置过滤
		//g_GlobalOption.m_FilterString.SetFilterString(strChatInfo);
		//if (strChatInfo.IsEmpty()) return;

		//限时聊天
		DWORD dwCurTimes = GetTickCount();
		if(m_dwLimitChatTimes==0 || dwCurTimes-m_dwLimitChatTimes>=LIMIT_CHAT_TIMES)
		{
			m_dwLimitChatTimes = dwCurTimes;

			//OSC,发送至所有人.
			m_ChatInput.SetWindowText(TEXT(""));

			if (NULL != m_pGameFrameControl)
			{
				m_pGameFrameControl->InsertToGameFrameMsg(strChatInfo.GetBuffer());
			}
		}
	}
	//设置界面
	m_ChatInput.SetFocus();
}

BOOL CSendMsgDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
	// TODO: 在此添加专用代码和/或调用基类

	switch (LOWORD(wParam))
	{
		case IDC_UP_TAG:		//标志按钮
		case IDC_UP_TAG2:
			{
				if (!m_bIsShowGameFrame && m_iCountMsgNums>0)
				{
					KillTimer(IDI_TIME_CHAT_MSG);
				}
				SetGameChatShow();
			}
			break;
		case IDC_SEND_MSG:		//发送按钮
			{
				//显示聊天窗口
				if (!m_bIsShowGameFrame)
				{
					m_iCountMsgNums = 0;
					SetGameChatShow();
				}
				//发送聊天语言
				SetGameChatMsg();
			}
			break;
		case IDC_EXP_BT:		//表情按钮
			{
				//建立表情窗口
				if (m_Expression.GetSafeHwnd()==NULL) m_Expression.CreateExpression(this);

				//移动窗口
				CRect rcButton;
				CSize ExpressSize;
				m_Expression.GetFixSize(ExpressSize);
				m_btExpression.GetWindowRect(&rcButton);
				m_Expression.MoveWindow(rcButton.right-ExpressSize.cx+2,rcButton.top-ExpressSize.cy,ExpressSize.cx,ExpressSize.cy);
				m_Expression.ShowWindow(SW_SHOW);
				m_Expression.SetFocus();
			}
			break;
		default:
			break;
	}

	return CRgnDialog::OnCommand(wParam, lParam);
}

//表情消息
LRESULT CSendMsgDlg::OnHitExpression(WPARAM wParam, LPARAM lParam)
{
	CExpressionItem * pExpressionItem=m_Expression.GetExpressItem((WORD)wParam);
	if (pExpressionItem!=NULL)
	{
		CString strBuffer;
		m_ChatInput.GetWindowText(strBuffer);
		strBuffer+=pExpressionItem->m_szTrancelate;
		m_ChatInput.SetWindowText(strBuffer);
		m_ChatInput.SetFocus();
		m_ChatInput.SetEditSel(strBuffer.GetLength(),strBuffer.GetLength());
	}

	return 0;
}


//设置窗口指针
void CSendMsgDlg::SetGameFrameControlPtr(CGameFrameControl* pGameFrameControl)
{
	if (NULL != pGameFrameControl)
	{
		m_pGameFrameControl = pGameFrameControl;
	}
}

BOOL CSendMsgDlg::OnInitDialog()
{
	CRgnDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	
	//聊天短语
	m_ChatInput.AddString(TEXT("快点吧我等的花儿也谢了"));
	m_ChatInput.AddString(TEXT("大家好，很高兴见到各位！"));
	m_ChatInput.AddString(TEXT("怎么又断线了，网络太差了..."));
	m_ChatInput.AddString(TEXT("不要走，决战到天亮。"));
	m_ChatInput.AddString(TEXT("和您合作真是太愉快了..."));
	m_ChatInput.AddString(TEXT("我们交个朋友，能告诉我您的联系方式吗？"));
	m_ChatInput.AddString(TEXT("您是MM,还是GG?"));
	m_ChatInput.AddString(TEXT("快点，花儿都凋谢了！"));
	m_ChatInput.AddString(TEXT("痛快，痛快，来十斤牛肉，八碗好酒！"));
	m_ChatInput.AddString(TEXT("不要吵了，有什么好吵的..."));
	m_ChatInput.AddString(TEXT("再见了，我会想念大家的！BYE"));
	m_ChatInput.LimitText(MAX_CHAT_LEN-8);
	m_ChatInput.SetFocus();

	//加载资源
	HINSTANCE hInstance=GetModuleHandle(GAME_FRAME_DLL_NAME);
	m_ImageWndBack.SetLoadInfo(IDB_IMAGE_DOWN_BACK, hInstance);
	m_btWndTag.LoadRgnImage(hInstance, IDB_BT_UP, DEF_BT_COLOR, 4);
	m_btSendChat.LoadRgnImage(hInstance, IDB_BT_SENDCHAT, DEF_BT_COLOR);
	m_btExpression.LoadRgnImage(hInstance, IDB_BT_EXPRESSION, DEF_BT_COLOR);
	m_btWndTagLight.LoadRgnImage(hInstance, IDB_BT_LIGHT_UP, DEF_BT_COLOR, 4);

	m_ImageBuffer.Destroy();
	CImageHandle HandleCenter(&m_ImageWndBack);
	m_ImageBuffer.Create(m_ImageWndBack.GetWidth(), m_ImageWndBack.GetHeight(), 24);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}



//鼠标右键按下消息
void CSendMsgDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
}

//BOOL CSendMsgDlg::OnEraseBkgnd(CDC* pDC)
//{
//	// TODO: 在此添加消息处理程序代码和/或调用默认值
//
//	return TRUE;
//	//return CRgnDialog::OnEraseBkgnd(pDC);
//}


//设置显示标志
void CSendMsgDlg::SetGameFrameShowTag(bool bIsShow)
{
	m_bIsShowGameFrame = bIsShow;
	HINSTANCE hInstance=GetModuleHandle(GAME_FRAME_DLL_NAME);
	if (true == m_bIsShowGameFrame)
	{//向上
		m_btWndTag.LoadRgnImage(hInstance, IDB_BT_DOWN, DEF_BT_COLOR, 4);
	}
	else
	{//向下
		m_btWndTag.LoadRgnImage(hInstance, IDB_BT_UP, DEF_BT_COLOR, 4);
	}
	m_btWndTagLight.ShowWindow(SW_HIDE);
	m_btWndTag.ShowWindow(SW_SHOW);
	m_btWndTag.InvalidateRect(NULL);
}


//响应消息
void CSendMsgDlg::SetMsgBeep()
{
	++m_iCountMsgNums;
	if (true == m_bIsShowGameFrame)
	{//显示就不响应了.
		m_iCountMsgNums = 0;
	}
	else
	{//没有显示窗口
		if ((false==m_bIsStartTime)&&(1 == m_iCountMsgNums))
		{
			m_bIsStartTime = true;
			++m_iCountMsgNums;
			//设置响应提示
			SetTimer(IDI_TIME_CHAT_MSG, TIME_CHAT_MSG_INTERVAL, NULL);
		}
	}
}

void CSendMsgDlg::OnTimer(UINT nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (IDI_TIME_CHAT_MSG == nIDEvent)
	{
		m_bIsStartTime=true;
		if (!m_bIsShowGameFrame && m_iCountMsgNums>0)
		{//响应按钮
			m_bIsShowWndTag = !m_bIsShowWndTag;
			HINSTANCE hInstance=GetModuleHandle(GAME_FRAME_DLL_NAME);
			if (true == m_bIsShowWndTag)
			{//加亮
				m_btWndTag.ShowWindow(SW_HIDE);
				m_btWndTagLight.ShowWindow(SW_SHOW);
			}
			else
			{
				m_btWndTagLight.ShowWindow(SW_HIDE);
				m_btWndTag.ShowWindow(SW_SHOW);
			}
		}
	}

	CRgnDialog::OnTimer(nIDEvent);
}
