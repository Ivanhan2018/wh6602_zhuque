// RoomListDlg.cpp : 实现文件
//

#include "stdafx.h"
 
#include "RoomListDlg.h"
#include "StreamReader.h"   
#include "HtmlResource.h"
#include "GlobalUnits.h"
#include "PlazaDlg.h"
#include ".\roomlistdlg.h"
#include "webpath.h"
#define IDC_ROOM_UNIT_BTN_OK     (1568)

CPngImage CRoomUnit::m_pngBack;
CPngImage CRoomUnit::m_pngBeilu;
CPngImage CRoomUnit::m_pngLimit;
CPngImage CRoomUnit::m_pngMoney;
CPngImage CRoomUnit::m_pngBtn;
CPngImage CRoomUnit::m_pngNumber_Max;
CPngImage CRoomUnit::m_pngNumber_Min;
CPngImage CRoomUnit::m_pngManIdentifier;
CPngImage CRoomUnit::m_pngTitle;

const CRect g_rcUnit[MAX_ROOM_UNIT_COUNT] = { CRect(15, 223, 15+175, 223+107), CRect(214, 223, 214+175, 223+107), 
									CRect(15, 365, 15+175, 365+107), CRect(214, 365, 214+175, 365+107), 
									CRect(15, 502, 15+175, 502+107), CRect(214, 502, 214+175, 502+107)};

void CRoomUnit::Init()
{
	HINSTANCE hInstance = AfxGetInstanceHandle();
	m_pngBack.LoadImage(hInstance, TEXT("ROOM_UNIT_BACK"));			//背景图
	m_pngBeilu.LoadImage(hInstance, TEXT("ROOM_UNIT_BEILU"));		//倍率
	m_pngLimit.LoadImage(hInstance, TEXT("ROOM_UNIT_LIMIT"));		//限
	m_pngMoney.LoadImage(hInstance, TEXT("ROOM_UNIT_MONEY"));		//宝石
	m_pngBtn.LoadImage(hInstance, TEXT("ROOM_UNIT_BTN"));			//进入按钮
	m_pngNumber_Max.LoadImage(hInstance, TEXT("ROOM_UNIT_NUMBER_MAXSIZE"));			//大数字
	m_pngNumber_Min.LoadImage(hInstance, TEXT("ROOM_UNIT_NUMBER_MINSIZE"));			//小数字
	m_pngManIdentifier.LoadImage(hInstance, TEXT("ROOM_UNIT_MAN_IDENTIFIER"));			//进入按钮
	m_pngTitle.LoadImage(hInstance, TEXT("ROOM_UNIT_TITLE"));
}
void CRoomUnit::Uinit()
{
	m_pngBack.DestroyImage();
	m_pngBeilu.DestroyImage();
	m_pngLimit.DestroyImage();
	m_pngMoney.DestroyImage();
	m_pngBtn.DestroyImage();
	m_pngManIdentifier.DestroyImage();
	m_pngNumber_Max.DestroyImage();
	m_pngNumber_Min.DestroyImage();
	m_pngTitle.DestroyImage();
}
void CRoomUnit::SetInfo(CWnd * pWnd, WORD index,const CRect & rect)
{
	m_pParent = pWnd;
	m_Index = index;
	CopyMemory(&m_rect, &rect, sizeof(m_rect));
}
void CRoomUnit::EableUnit(bool bEable)
{
	m_bEable = bEable;
	if (!m_bEable)
	{
		m_wLight = 3;
	}
}
void CRoomUnit::SetData(const HINSTANCE & hInstance, WORD wKindID, WORD wLight, long lCellScore, long lLessScore)
{
	m_wKindID = wKindID;
	m_wLight = wLight;
	m_lCellScore = lCellScore;
	m_lLessScore = lLessScore;

	m_pngGameIcon.DestroyImage();
	if (hInstance)
	{
		if (!m_pngGameIcon.IsNull())
			m_pngGameIcon.DestroyImage();
		m_pngGameIcon.LoadImage(hInstance, TEXT("ROOM_UNIT_ICON"));		//游戏Icon
	}
}
void CRoomUnit::OnPaint(CDC * pDC)
{
	int x, y, nWidth, nHeight;
	if ( !m_pngBack.IsNull() )
	{//背景
		if (m_bEable)
            m_pngBack.DrawImage(pDC, m_rect.left, m_rect.top, m_pngBack.GetWidth(), m_pngBack.GetHeight()/2, 0, 0);
		else
			m_pngBack.DrawImage(pDC, m_rect.left, m_rect.top, m_pngBack.GetWidth(), m_pngBack.GetHeight()/2, 0, m_pngBack.GetHeight()/2);
	}

	if ( !m_pngGameIcon.IsNull() )
	{
		x = m_rect.left+93;
		y = m_rect.top+6;

		nWidth = m_pngGameIcon.GetWidth();
		nHeight = m_pngGameIcon.GetHeight()/2;
		if (m_bEable)
		{
			m_pngGameIcon.DrawImage(pDC, x, y, nWidth, nHeight, 0, 0);
		}
		else
		{
			m_pngGameIcon.DrawImage(pDC, x, y, nWidth, nHeight, 0, nHeight, nWidth, nHeight);
		}
	}

	if ( !m_pngTitle.IsNull() )
	{
		x = m_rect.left+11;
		y = m_rect.top+79;
		long imgHegiht = m_pngTitle.GetHeight();
		imgHegiht = imgHegiht/6;
		if (m_bEable)
		{
			m_pngTitle.DrawImage(pDC, x, y, m_pngTitle.GetWidth()/2, imgHegiht, 0, m_Index*imgHegiht);
		}
		else
		{
			m_pngTitle.DrawImage(pDC, x, y, m_pngTitle.GetWidth()/2, imgHegiht, m_pngTitle.GetWidth()/2, m_Index*imgHegiht);
		}
	}

	if ( !m_pngBeilu.IsNull() )
	{//房间倍率
		x = m_rect.left+10;
		y = m_rect.top+7;
		if (m_bEable)
		{
			m_pngBeilu.DrawImage(pDC, x, y, m_pngBeilu.GetWidth(), m_pngBeilu.GetHeight()/2, 0, 0);
			if (!m_pngNumber_Max.IsNull())
			{
				y = m_rect.top+11;
				x += m_pngBeilu.GetWidth();
				DarwNumber(pDC, m_pngNumber_Max, m_lCellScore, CPoint(x, y));
			}
		}else
		{
			m_pngBeilu.DrawImage(pDC, x, y, m_pngBeilu.GetWidth(), m_pngBeilu.GetHeight()/2, 0, m_pngBeilu.GetHeight()/2);
		}
	}

	if ( !m_pngBtn.IsNull() )
	{
		x = m_rect.left+120;
		y = m_rect.top+52;
		if (m_bEable)
		{
			m_pngBtn.DrawImage(pDC, x, y, m_pngBtn.GetWidth(), m_pngBtn.GetHeight()/2, 0 , 0);
		}else
		{
			m_pngBtn.DrawImage(pDC, x, y, m_pngBtn.GetWidth(), m_pngBtn.GetHeight()/2, 0 , m_pngBtn.GetHeight()/2);
		}
	}


	if ( !m_pngLimit.IsNull() && !m_pngMoney.IsNull())
	{//限制宝石
		x = m_rect.left+10;
		y = m_rect.top+52;
		//限
		m_pngLimit.DrawImage(pDC, x, y, m_pngLimit.GetWidth(), m_pngLimit.GetHeight()/2, 0, m_bEable?0:18);
		x += m_pngLimit.GetWidth();
		if (!m_pngNumber_Min.IsNull())
		{
			//数字
			x += DarwNumber(pDC, m_pngNumber_Min, m_lLessScore, CPoint(x, y)).cx;
		}
		y = m_rect.top+50;
		//宝石
		m_pngMoney.DrawImage(pDC, x, y, m_pngMoney.GetWidth(), m_pngMoney.GetHeight()/2, 0, m_bEable?0:19);
	}

	if ( !m_pngManIdentifier.IsNull() )
	{
		long imgWidth = m_pngManIdentifier.GetWidth() / 4;
		x = m_rect.left+71;
		y = m_rect.top+82;
		m_pngManIdentifier.DrawImage(pDC,x,y, imgWidth, m_pngManIdentifier.GetHeight(), m_wLight*imgWidth, 0);
		x += imgWidth;
		m_pngManIdentifier.DrawImage(pDC,x,y, imgWidth, m_pngManIdentifier.GetHeight(), m_wLight*imgWidth, 0);
		x += imgWidth;
		m_pngManIdentifier.DrawImage(pDC,x,y, imgWidth, m_pngManIdentifier.GetHeight(), m_wLight*imgWidth, 0);

	}
}

CSize CRoomUnit::DarwNumber(CDC * pDC, CPngImage & pngImg,LONG number, const CPoint & pt)
{
	if (NULL == pDC || pngImg.IsNull())
		return CSize(0, 0);

	TCHAR tc[NAME_LEN]=TEXT("");
	_sntprintf(tc,sizeof(tc),TEXT("%d"),number);
	LPCTSTR pszString = tc;
	int nImgWidth = pngImg.GetWidth()/10;
	int nImgHeight =  pngImg.GetHeight()/2;
	int nOffsetX = pt.x, nOffsetY = pt.y;
	int nLen = strlen(pszString);
	CRect rc(0, 0, 0, 0);
	rc.bottom = rc.top + nImgHeight;
	rc.right = rc.left + (nLen * nImgWidth);

	int n = 0;
	while (*pszString != '\0')
	{
		CString str;
		str =  *pszString;
		n = atoi((LPCTSTR)str);
		//庄家标志
		if (m_bEable)
		{
			pngImg.DrawImage(pDC, nOffsetX, nOffsetY,nImgWidth,nImgHeight,n*nImgWidth,0);
		}
		else
		{
			pngImg.DrawImage(pDC, nOffsetX, nOffsetY,nImgWidth,nImgHeight,n*nImgWidth,nImgHeight);
		}
		nOffsetX += nImgWidth;
		pszString++;
	}
	return CSize(rc.Width(), rc.Height());
}
bool CRoomUnit::IsFouse()
{
	return true;
}

int CRoomUnit::OnLButtonDown(const UINT & nFlags, const CPoint & pt)
{
	if (m_bEable && m_rect.PtInRect(pt))
	{
		return m_Index;
	}
	return -1;
}
int CRoomUnit::OnMouseMove(const UINT & nFlags, const CPoint & point)
{
	if (m_bEable && m_rect.PtInRect( point))
	{
		return m_Index;
	}
	return -1;
}
// CRoomListDlg 对话框

IMPLEMENT_DYNAMIC(CRoomListDlg, CDialog)
CRoomListDlg::CRoomListDlg(/*CWnd* pParent =NULL*/):CSkinDialogEx(IDD_GAME_LIST)
	/*: CDialog(CRoomListDlg::IDD, pParent)*/
{
	m_wKindID=-1;
	m_nRoomNum=-1;
	m_dwLimitChatTimes=0;
	m_exMain = NULL;
}

CRoomListDlg::~CRoomListDlg()
{
	if (NULL != m_exMain)
	{
		delete m_exMain;
	}
}

void CRoomListDlg::DoDataExchange(CDataExchange* pDX)
{
	CSkinDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CRoomListDlg, CSkinDialogEx)
	ON_WM_LBUTTONDOWN()
	ON_WM_PAINT()
	ON_WM_DESTROY()
	ON_WM_MOUSEMOVE()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

void CRoomListDlg::OnLButtonDown( UINT nFlags, CPoint point )
{
	//设置默认值
	for (WORD n=0; n<CountArray(m_Unit); n++)
	{
		if (-1 != m_Unit[n].OnLButtonDown(nFlags, point))
		{
			BYTE i=0;
			//房间搜索
			for(i=0;i<m_gameListManager->m_arrayGameRoomInfo.GetCount();i++)
			{
				if(m_Unit[n].GetIndex()+1 == m_gameListManager->m_arrayGameRoomInfo[i].GetSortId())
				{//切换到桌子列表
					m_nRoomNum= n+1;
					AfxGetApp()->GetMainWnd()->PostMessage(WM_COMMAND,IDM_CALL_TABLE_LIST,0);
					return;
				}
			}
			//无效房间
			m_nRoomNum= -1;
			AfxGetApp()->GetMainWnd()->PostMessage(WM_COMMAND,IDM_CALL_TABLE_LIST,0);
			break;
		}
	}
}

BOOL CRoomListDlg::OnInitDialog()
{
	__super::OnInitDialog();

	//SetWindowLong(this->GetSafeHwnd(), GWL_EXSTYLE, GetWindowLong(this->GetSafeHwnd(), GWL_EXSTYLE)^0x80000);

	CRoomUnit::Init();
	for (WORD i=0; i<CountArray(m_Unit); i++)
		m_Unit[i].SetInfo(this, i, g_rcUnit[i]);
	m_exMain=new CExmain;	//今日公告
	m_exMain->Create(NULL,NULL,WS_CHILD,CRect(0,0,254,623),this,20,NULL);	 
	m_exMain->Navigate2(&CComVariant("about:blank"),NULL,NULL,NULL,NULL);
	HRESULT hr = ::CoMarshalInterThreadInterfaceInStream(IID_IWebBrowser2,m_exMain->GetControlUnknown(),&m_iStream);	
	if ( FAILED(hr) )return FALSE;
	m_pBrowser = NULL;	 
	hr = ::CoGetInterfaceAndReleaseStream(m_iStream,IID_IWebBrowser2,(void**)&m_pBrowser); //在这里获得接口指针
	CComPtr < IDispatch > spDispDoc;
	hr = m_pBrowser->get_Document( &spDispDoc );
	if (FALSE(hr))return FALSE; 
	CComQIPtr< IHTMLDocument2 > pIDoc = spDispDoc;
	if (pIDoc == NULL)return FALSE;	
	m_webPage.SetDocument(pIDoc);
	::ShowWindow(m_exMain->GetSafeHwnd(), SW_HIDE);
	return TRUE;
}

void CRoomListDlg::OnPaint()
{
	CPaintDC pDC(this);
	CRect rect;
	GetClientRect(&rect);
	DrawPic(&pDC,IDB_ROOM_LIST_BG,&rect);
	if (!m_pngGameTitle.IsNull())
	{//游戏标题
		int x = 406/2 - m_pngGameTitle.GetWidth()/2;
		int y = 35 + 192/2 - m_pngGameTitle.GetHeight()/2;
		m_pngGameTitle.DrawImage(&pDC, x, 20, m_pngGameTitle.GetWidth(), m_pngGameTitle.GetHeight(), 0, 0);
	}
	for (WORD i=0; i<CountArray(m_Unit); i++)
	{//绘制房间列表
		m_Unit[i].OnPaint(&pDC);
	}
}

void CRoomListDlg::OnOK()
{
}

void CRoomListDlg::OnBnClickedFriend()
{
	//((CPlazaDlg *)AfxGetMainWnd())->GetFriendListInterface()->ShowWindow(SW_SHOW);
}

void CRoomListDlg::ShowHtml( CString content )
{
	CStreamReader coc;
	coc.SetData(content);

	HRESULT hr;

	CComPtr< IDispatch > spDoc(m_exMain->get_Document());
	ASSERT( spDoc );

	CComQIPtr< IPersistStreamInit, &IID_IPersistStreamInit > spPSI( spDoc );
	ASSERT(spPSI);


	hr = spPSI->InitNew();
	ASSERT( SUCCEEDED(hr) );

	hr = spPSI->Load( static_cast<IStream*>(&coc) );	
	ASSERT( SUCCEEDED(hr) );

}

void CRoomListDlg::EnterRoom()
{//进入房间列表
	//获取html
// 	m_gameListManager->SetHtmlRoom();
// 	CString strHtml=m_gameListManager->GetGameRoomHtml(); 
// 	ShowHtml(strHtml);
	
	CArray<CGameRoomInfo,CGameRoomInfo> & m_arrayGameRoomInfo = m_gameListManager->m_arrayGameRoomInfo;
	CArray<CGameInfo,CGameInfo> & m_arrayGameInfoCollection = m_gameListManager->m_arrayGameInfoCollection;
	int nCount = m_arrayGameRoomInfo.GetCount();
	CString strGamename;
	WORD wKindID=0;

	for(BYTE j=0;j<m_arrayGameInfoCollection.GetCount();j++)
	{
		//查找 游戏相关资源信息
		if(m_arrayGameInfoCollection[j].m_wKindID == m_arrayGameRoomInfo[0].GetKindID())
		{
			strGamename = TEXT("Games/") + m_arrayGameInfoCollection[j].m_strName+TEXT(".dll");
			break;
		}
	}
	HINSTANCE  hInstance = LoadLibrary(strGamename);

	if (hInstance)
	{
		if (!m_pngGameTitle.IsNull())
			m_pngGameTitle.DestroyImage();
		m_pngGameTitle.LoadImage(hInstance, TEXT("ROOM_UNIT_TITLE"));
	}
	if( m_arrayGameRoomInfo.GetCount() != 0 && m_arrayGameInfoCollection.GetCount() != 0)
	{
		//设置默认值
		for (WORD i=0; i<CountArray(m_Unit); i++)
		{
			m_Unit[i].SetData(hInstance,0,0,0,0);
			m_Unit[i].EableUnit(false);			
		}

		for(BYTE j=0;j<m_arrayGameInfoCollection.GetCount();j++)
		{
			//查找 游戏相关资源信息
			if(m_arrayGameInfoCollection[j].m_wKindID == m_arrayGameRoomInfo[0].GetKindID())
			{
				for (BYTE k=0; k<nCount; k++)
				{
					WORD n = m_arrayGameRoomInfo[k].GetSortId()-1 + CountArray(m_Unit);
					n = n % CountArray(m_Unit);

					WORD wLight = 0;
					float f = (float)m_arrayGameRoomInfo[k].m_pListServer->m_GameServer.dwOnLineCount / (float)m_arrayGameRoomInfo[k].m_pListServer->m_GameServer.dwMaxCount;
					if (f >= 0.7f) wLight = 2;	//红灯
					else if (f >= 0.5f)wLight = 1;	//黄灯
					else wLight = 0;	//绿灯
					//long lValue = m_arrayGameRoomInfo[k].m_pListServer->m_GameServer.lCellScore;
					//lValue = m_arrayGameRoomInfo[k].m_pListServer->m_GameServer.lLessScore;

					WORD wKindID = m_arrayGameInfoCollection[j].m_wKindID;
					LONG lCellScore = m_arrayGameRoomInfo[k].m_pListServer->m_GameServer.lCellScore;
					LONG lRestrictScore = m_arrayGameRoomInfo[k].m_pListServer->m_GameServer.lRestrictScore;
					m_Unit[n].EableUnit(true);
					m_Unit[n].SetData(hInstance, wKindID, wLight, lCellScore, lRestrictScore);

					wKindID = m_arrayGameInfoCollection[j].m_wKindID;
				}
				break;
			}
		}
	}

	WORD wCount = 0;
	WORD i=0;
	for (i=0; i<CountArray(m_Unit); i++)
	{
		if (m_Unit[i].isEableUnit())
			m_Unit[i].SetRect(g_rcUnit[wCount++]);
	}
	for (i=0; i<CountArray(m_Unit); i++)
	{
		if (!m_Unit[i].isEableUnit())
			m_Unit[i].SetRect(g_rcUnit[wCount++]);
	}

	FreeLibrary(hInstance);
	InvalidateRect(NULL);
}

void CRoomListDlg::SetResetHtml()
{
	CString strUrl;
	//strUrl.Format(WEB_ROOM_AD);
	//m_exMain->Navigate2(&CComVariant(strUrl),NULL,NULL,NULL,NULL);
}

void CRoomListDlg::OnDestroy()
{
	CRoomUnit::Uinit();
	CSkinDialogEx::OnDestroy();
	// TODO: 在此处添加消息处理程序代码
}

void CRoomListDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CSkinDialogEx::OnMouseMove(nFlags, point);
	BOOL b= FALSE;
	for (WORD n=0; n<CountArray(m_Unit); n++)
	{
		if (-1 != m_Unit[n].OnMouseMove(nFlags, point))
		{
			b = TRUE;
			break;
		}
	}
 	if (b)
 	{
		HCURSOR hCursor = ::LoadCursor(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDC_HAND_CUR));
		::SetCursor(hCursor);
 	}
}
void CRoomListDlg::OnSize(UINT nType, int cx, int cy)
{
	CSkinDialogEx::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
	if (NULL != m_exMain)
	{
		CRect rect;
		::GetClientRect(m_exMain->GetSafeHwnd(), &rect);
		::SetWindowPos(m_exMain->GetSafeHwnd(), 0, cx-rect.Width(), 0, 0, 0, SWP_NOSIZE);
	//	CString strUrl;
	//	strUrl.Format(WEB_ROOM_AD);
	//	m_exMain->Navigate2(&CComVariant(strUrl),NULL,NULL,NULL,NULL);
	}
}

BOOL CRoomListDlg::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	return TRUE;
	//return CSkinDialogEx::OnEraseBkgnd(pDC);
}
