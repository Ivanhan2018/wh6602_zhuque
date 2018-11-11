// Chat.cpp : 实现文件
//

#include "stdafx.h"
//#include "疯狂斗地主游戏.h"
#include "Chat.h"
#include ".\chat.h"


// CChat 对话框

IMPLEMENT_DYNAMIC(CChat, CDialog)
CChat::CChat(CWnd* pParent /*=NULL*/)
	: CDialog(CChat::IDD, pParent)
{
}

CChat::~CChat()
{
}

void CChat::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO1, m_ChatInput);
}


BEGIN_MESSAGE_MAP(CChat, CDialog)
END_MESSAGE_MAP()


// CChat 消息处理程序

BOOL CChat::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	m_ChatInput.LimitText(MAX_CHAT_LEN-1);

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

	return TRUE;
}

void CChat::SetPos(int cx,int cy)
{
	CRect rect;
	GetWindowRect(rect);
	MoveWindow(cx,cy,rect.Width(),rect.Height(),true);
}

BOOL CChat::PreTranslateMessage(MSG* pMsg)
{
	if ((pMsg->message==WM_KEYDOWN)&&(pMsg->wParam==VK_RETURN))
	{
		//if(m_GameClientView.m_GameChat.Is)
		//m_GameClientView.m_GameChat.ShowWindow(SW_SHOW);
		//OnChat(1, 1);
		//ShowWindow(SW_SHOW);
	}
	return CDialog::PreTranslateMessage(pMsg);
}

void CChat::OnOK()
{
	// TODO: 在此添加专用代码和/或调用基类

	//CDialog::OnOK();
}
