// KeyDialog.cpp : 实现文件
//

#include "stdafx.h"
#include "KeyDialog.h"
#include ".\keydialog.h"
#include "DlgLogon.h"

// CKeyDialog 对话框

#define  KEY_BTN_ID   100
#define  KEY_RES_ID	  347	
//键值对照表
TCHAR g_czKey[KEY_NUM][12]={
	TEXT("1"),		TEXT("2"), TEXT("3"), TEXT("4"),  TEXT("5"),  TEXT("6"),  TEXT("7"), TEXT("8"),  TEXT("9"),  TEXT("0"), TEXT("`"),  TEXT("BackSpace"),
	TEXT("Shift"),  TEXT("/"), TEXT("-"), TEXT("="),  TEXT("\\"), TEXT("["),  TEXT("]"), TEXT(";"),  TEXT("\'"), TEXT(","), TEXT("."),  TEXT("Caps Lock"),
	TEXT("q"),      TEXT("w"), TEXT("e"), TEXT("r"),  TEXT("t"),  TEXT("y"),  TEXT("u"), TEXT("i"),  TEXT("o"),  TEXT("p"), TEXT("a"),  TEXT("s"), TEXT("d"),
	TEXT("f"),      TEXT("g"), TEXT("h"), TEXT("j"),  TEXT("k"),  TEXT("l"),  TEXT("z"), TEXT("x"),  TEXT("c"),  TEXT("v"), TEXT("b"),  TEXT("n"), TEXT("m")
};

//键值对照表
TCHAR	   g_czKeyShift[KEY_NUM][12]={
	TEXT("!"),		TEXT("@"),  TEXT("#"), TEXT("$"),  TEXT("%"), TEXT("^"),  TEXT("&"), TEXT("*"),  TEXT("("),  TEXT(")"), TEXT("~"), TEXT("BackSpace"),
	TEXT("Shift"),  TEXT("?"),  TEXT("_"), TEXT("+"),  TEXT("|"), TEXT("{"),  TEXT("}"), TEXT(":"),  TEXT("\""), TEXT("<"), TEXT(">"), TEXT("Caps Lock"),
	TEXT("Q"),		TEXT("W"),  TEXT("E"), TEXT("R"),  TEXT("T"), TEXT("Y"),  TEXT("U"), TEXT("I"),  TEXT("O"),  TEXT("P"), TEXT("A"), TEXT("S"), TEXT("D"),
	TEXT("F"),		TEXT("G"),  TEXT("H"), TEXT("J"),  TEXT("K"), TEXT("L"),  TEXT("Z"), TEXT("X"),  TEXT("C"),  TEXT("V"), TEXT("B"), TEXT("N"), TEXT("M")
};


TCHAR g_czKeyASC[KEY_NUM][1]={
	'1', '2', '3', '4',  '5',  '6',  '7', '8',  '9',  '0', '`',  VK_BACK,
		VK_SHIFT,  '/', '-', '=',  '\\', '[',  ']', ';',  '\'', ',', '.',  VK_CAPITAL,
		'q',  'w', 'e', 'r',  't',  'y',  'u', 'i',  'o',  'p', 'a',  's', 'd',
		'f',  'g', 'h', 'j',  'k',  'l',  'z', 'x',  'c',  'v', 'b',  'n', 'm'
};
TCHAR g_czKeyShiftASC[KEY_NUM][1]={
	'!', '@',  '#', '$',  '%', '^',  '&', '*',  '(',  ')', '~', VK_BACK,
	VK_SHIFT,  '?',  '_', '+',  '|', '{',  '}', ':',  '\"', '<', '>', VK_CAPITAL,
	'Q', 'W',  'E', 'R',  'T', 'Y',  'U', 'I',  'O',  'P', 'A', 'S', 'D',
	'F', 'G',  'H', 'J',  'K', 'L',  'Z', 'X',  'C',  'V', 'B', 'N', 'M'
};


CPoint g_ptKey[KEY_NUM]={
	CPoint(11, 4), CPoint(37, 4), CPoint(63, 4), CPoint(89, 4), CPoint(115, 4), CPoint(141, 4),	CPoint(167, 4), CPoint(193, 4), CPoint(219, 4), CPoint(245, 4), CPoint(271, 4), CPoint(297, 4),
	CPoint(3, 30), CPoint(43, 30), CPoint(68, 30), CPoint(93, 30), CPoint(118, 30), CPoint(143, 30),	CPoint(168, 30), CPoint(193, 30), CPoint(218, 30), CPoint(243, 30), CPoint(268, 30), CPoint(293, 30),
	CPoint(11, 56), CPoint(37, 56), CPoint(63, 56), CPoint(89, 56), CPoint(115, 56), CPoint(141, 56),	CPoint(167, 56), CPoint(193, 56), CPoint(219, 56), CPoint(245, 56), CPoint(271, 56), CPoint(297, 56),CPoint(323, 56),
	CPoint(11, 82), CPoint(37, 82), CPoint(63, 82), CPoint(89, 82), CPoint(115, 82), CPoint(141, 82),	CPoint(167, 82), CPoint(193, 82), CPoint(219, 82), CPoint(245, 82), CPoint(271, 82), CPoint(297, 82),CPoint(323, 82),
}
;
UINT g_uKeyResId[KEY_NUM]={
	IDB_KEY_0, IDB_KEY_1,	IDB_KEY_2,	IDB_KEY_3,	IDB_KEY_4,	IDB_KEY_5,	IDB_KEY_6,	IDB_KEY_7,	IDB_KEY_8,	IDB_KEY_9,	IDB_KEY_10,	IDB_KEY_11,
	IDB_KEY_12,IDB_KEY_13,	IDB_KEY_14,	IDB_KEY_15,	IDB_KEY_16,	IDB_KEY_17,	IDB_KEY_18,	IDB_KEY_19,	IDB_KEY_20,	IDB_KEY_21,	IDB_KEY_22,	IDB_KEY_23,
	IDB_KEY_24,IDB_KEY_25,	IDB_KEY_26,	IDB_KEY_27,	IDB_KEY_28,	IDB_KEY_29,	IDB_KEY_30,	IDB_KEY_31,	IDB_KEY_32,	IDB_KEY_33,	IDB_KEY_34,	IDB_KEY_35,	IDB_KEY_36,
	IDB_KEY_37,IDB_KEY_38,	IDB_KEY_39,	IDB_KEY_40,	IDB_KEY_41,	IDB_KEY_42,	IDB_KEY_43,	IDB_KEY_44,	IDB_KEY_45,	IDB_KEY_46,	IDB_KEY_47,	IDB_KEY_48,	IDB_KEY_49
};
UINT g_uKeyShiftResId[KEY_NUM]={
	IDB_KEY_50,IDB_KEY_51,	IDB_KEY_52,	IDB_KEY_53,	IDB_KEY_54,	IDB_KEY_55,	IDB_KEY_56,	IDB_KEY_57,	IDB_KEY_58,	IDB_KEY_59,	IDB_KEY_60,	IDB_KEY_11,
	IDB_KEY_12,IDB_KEY_61,  IDB_KEY_62,	IDB_KEY_63,	IDB_KEY_64,	IDB_KEY_65,	IDB_KEY_66,	IDB_KEY_67,	IDB_KEY_68,	IDB_KEY_69,	IDB_KEY_70,	IDB_KEY_23,
	IDB_KEY_71,IDB_KEY_72,	IDB_KEY_73,	IDB_KEY_74,	IDB_KEY_75,	IDB_KEY_76,	IDB_KEY_77,	IDB_KEY_78,	IDB_KEY_79,	IDB_KEY_80,	IDB_KEY_81,	IDB_KEY_82,	IDB_KEY_83,
	IDB_KEY_84,IDB_KEY_85,	IDB_KEY_86,	IDB_KEY_87,	IDB_KEY_88,	IDB_KEY_89,	IDB_KEY_90,	IDB_KEY_91,	IDB_KEY_92,	IDB_KEY_93,	IDB_KEY_94,	IDB_KEY_95,	IDB_KEY_96,
};


IMPLEMENT_DYNAMIC(CKeyDialog, CSkinDialogEx)
CKeyDialog::CKeyDialog(CWnd* pParent /*=NULL*/)
	: CSkinDialogEx(CKeyDialog::IDD, pParent)
	
{	
	m_bShift=FALSE;					//Shift 
	m_bCapsLoc=FALSE;				//Caps Lock
	m_hParent=NULL;
}

CKeyDialog::~CKeyDialog()
{
}

void CKeyDialog::DoDataExchange(CDataExchange* pDX)
{
	CSkinDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDCANCEL, m_btnClose);
}


BEGIN_MESSAGE_MAP(CKeyDialog, CSkinDialogEx)
	ON_BN_CLICKED(KEY_BTN_ID,OnBnClickKey)

	ON_WM_DESTROY()
	ON_WM_PAINT()
END_MESSAGE_MAP()


// CKeyDialog 消息处理程序

BOOL CKeyDialog::OnInitDialog()
{
	CSkinDialogEx::OnInitDialog();
	// TODO:  在此添加额外的初始化

	//按钮绘制：
	HINSTANCE hInstance=AfxGetInstanceHandle();

	m_backImage.SetLoadInfo(IDB_KEY_BACK, hInstance, true);
	int i=0;
	for (i=0; i<KEY_NUM; i++)
	{
		//m_KeyBtn[i].Create(g_czKey[i], WS_CHILD|WS_VISIBLE, CRect(g_ptKey[i].x,g_ptKey[i].y,0,0), this, KEY_BTN_ID);
		m_KeyBtn[i].Create(TEXT(""), WS_CHILD|WS_VISIBLE, CRect(g_ptKey[i].x,g_ptKey[i].y,0,0), this, KEY_BTN_ID);
		m_KeyBtn[i].LoadRgnImage(hInstance, g_uKeyResId[i],DEF_TRANS_COLOR, 4);
	}

	int count=26;
	int j=0;
	RandKey(m_nRandKey, count);
	for (i=24; i<KEY_NUM && j<count; i++, j++)
	{
		::SetWindowPos(m_KeyBtn[i].GetSafeHwnd(), 0, g_ptKey[24+m_nRandKey[j]].x, g_ptKey[24+m_nRandKey[j]].y, 0, 0, SWP_NOSIZE);
	}

	count=11;
	j=0;
	RandKey(m_nRandKey, count);
	for (i=0; i<KEY_NUM && j<count; i++, j++)
	{
		::SetWindowPos(m_KeyBtn[i].GetSafeHwnd(), 0, g_ptKey[m_nRandKey[j]].x, g_ptKey[m_nRandKey[j]].y, 0, 0, SWP_NOSIZE);
	}

	count=10;
	j=0;
	RandKey(m_nRandKey, count);
	for (i=13; i<KEY_NUM && j<count; i++, j++)
	{
		::SetWindowPos(m_KeyBtn[i].GetSafeHwnd(), 0, g_ptKey[13+m_nRandKey[j]].x, g_ptKey[13+m_nRandKey[j]].y, 0, 0, SWP_NOSIZE);
	}

	m_btnClose.LoadRgnImage(hInstance, IDB_KEY_CLOSE);
	::SetWindowPos(m_btnClose.GetSafeHwnd(), HWND_TOP, 347, 6, 0, 0, SWP_NOSIZE);
	//SetWindowPos(0, 0, 0, 360, 110, SWP_NOMOVE);

	
	//合并区域
	CRgn SignedRegion;
	SignedRegion.CreateRoundRectRgn(0, 0, 360, 110,4,4);
	//设置区域
	SetWindowRgn(SignedRegion,TRUE);
	SignedRegion.DeleteObject();


	//UpdateKeySkin();
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

//随机排列
void CKeyDialog::RandKey(int key[], int count)
{
	if (count <= 0 )
	{
		return;
	}
	int i=0;
	for (; i<count; i++)
	   key[i]=-1;

	i=0;
	srand( (unsigned)time( NULL ) );
	for (; i<count; i++)
	{
		int nRand = rand() % count;
		for (int j=0; j<count; j++)
		{
			if (key[j] == nRand)
			{
				nRand = 0;
				for (int k=0; k<count; k++)
				{
					int n=0;
					BOOL b=TRUE;
					for (; n<count; n++)
					{
						if (key[n] == nRand)
						{
							b=FALSE;
							break;
						}
					}
					if (b)
					{
						break;
					}else
					{
						nRand++;
					}
				}
			}
		}
		key[i] = nRand;
	}
}

void CKeyDialog::OnBnClickKey()
{
	TCHAR ** pKey = (TCHAR **)g_czKey;
	if (m_bShift)
	{
		pKey = (TCHAR **)g_czKeyShift;
	}
	HWND hFocus = ::GetFocus();
	CString str;
	TCHAR ch = -1;
	for (int i=0; i<KEY_NUM; i++)
	{
		if (hFocus == m_KeyBtn[i].GetSafeHwnd())
		{
			str.Empty();

			if ( 0 == strcmp(TEXT("Shift"), g_czKey[i]) )
			{
				m_bShift = !m_bShift;
				UpdateKeySkin();
			}
			else if ( 0 == strcmp(TEXT("Caps Lock"), g_czKey[i]) )
			{
				m_bCapsLoc = !m_bCapsLoc;
				UpdateKeySkin();
			}else
			{
				if (m_bShift)
				{
					if (m_bCapsLoc && i >= 24)
					{
						str = g_czKey[i];
						ch = g_czKeyASC[i][0];
					}else
					{
						str = g_czKeyShift[i];
						ch = g_czKeyShiftASC[i][0];
					}
				}else
				{
					if (m_bCapsLoc && i >= 24)
					{
						str = g_czKeyShift[i];
						ch = g_czKeyShiftASC[i][0];
					}else
					{
						str = g_czKey[i];
						ch = g_czKeyASC[i][0];
					}
				}
			}
			if (-1 != ch && NULL != m_hParent)
			{
				//AfxMessageBox(str);
				if (::SendMessage(m_hParent, KEY_DOWN, ch, 0))
				{
					//AfxMessageBox(str);
				}
			}
		}
		//m_KeyBtn[i].LoadRgnImage(hInstance, uResId,DEF_TRANS_COLOR, 4);
	}
}
//更新按钮皮肤
void CKeyDialog::UpdateKeySkin()
{
	int k = 0;
	int j = 0;
	CString str;
	UINT uResId = 0;
	HINSTANCE hInstance=AfxGetInstanceHandle();
	for (int i=0; i<KEY_NUM; i++)
	{
		if (m_bShift)
		{
			if (m_bCapsLoc && i >= 24)
			{
				str = g_czKey[i];
				uResId = g_uKeyResId[i];
			}else
			{
				str = g_czKeyShift[i];
				uResId = g_uKeyShiftResId[i];
			}
		}else
		{
			if (m_bCapsLoc && i >= 24)
			{
				str = g_czKeyShift[i];
				uResId = g_uKeyShiftResId[i];
			}else
			{
				str = g_czKey[i];
				uResId = g_uKeyResId[i];
			}
		}
		//m_KeyBtn[i].SetWindowText(str);
		m_KeyBtn[i].LoadRgnImage(hInstance, uResId,DEF_TRANS_COLOR, 4);
		m_KeyBtn[i].Invalidate(FALSE);
	}
}
void CKeyDialog::OnOK()
{
	// TODO: 在此添加专用代码和/或调用基类
	CSkinDialogEx::OnOK();

	((CDlgLogon*)GetParent())->SetKeyHide();

	DestroyWindow();
}

void CKeyDialog::OnCancel()
{
	// TODO: 在此添加专用代码和/或调用基类
	CSkinDialogEx::OnCancel();

	((CDlgLogon*)GetParent())->SetKeyHide();

	DestroyWindow();
}

void CKeyDialog::OnDestroy()
{
	CSkinDialogEx::OnDestroy();

	// TODO: 在此处添加消息处理程序代码

}

void CKeyDialog::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 在此处添加消息处理程序代码
	// 不为绘图消息调用 CSkinDialogEx::OnPaint()
	m_backImage.Draw(dc.m_hDC,0,0);
}
