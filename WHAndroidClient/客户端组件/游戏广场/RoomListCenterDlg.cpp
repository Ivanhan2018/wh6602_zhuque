// RoomListCenterDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "RoomListCenterDlg.h"
#include ".\roomlistcenterdlg.h"


// CRoomListCenterDlg 对话框

IMPLEMENT_DYNAMIC(CRoomListCenterDlg, CDialog)
CRoomListCenterDlg::CRoomListCenterDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CRoomListCenterDlg::IDD, pParent)
{
	m_wSortID=-1;
}

CRoomListCenterDlg::~CRoomListCenterDlg()
{
	m_memdc.DeleteDC();
}

void CRoomListCenterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CRoomListCenterDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// CRoomListCenterDlg 消息处理程序

BOOL CRoomListCenterDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CString strPath("");
	GetImagePlazaPath(strPath);
	m_ImageBK.LoadImage(TEXT(strPath+"BKGameList.png"));
	
	//创建DC
	CPaintDC dc(this);
	m_memdc.DeleteDC();
	CBitmap bmp;
	bmp.CreateCompatibleBitmap(&dc,536,360);
	m_memdc.CreateCompatibleDC(&dc);
	m_memdc.SelectObject(bmp);
	InvalidateRect(NULL);
	bmp.DeleteObject();

	//创建房间列表
	CreateRoomUnit();

	return TRUE;  // return TRUE unless you set the focus to a control
}
void CRoomListCenterDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	if (!m_ImageBK.IsNull())
	{
		m_ImageBK.DrawImage(&m_memdc,0,0);
	}

	CRect rect;
	GetClientRect(&rect);
	dc.BitBlt(0, 0, rect.Width(), rect.Height(), &m_memdc, 0, 0, SRCCOPY);
}

void CRoomListCenterDlg::EnterRoom()
{//进入房间列表
	int nCount = m_arrayGameRoomInfo.GetCount();
	nCount=min(nCount,IGAME_ROOM_UNIT_COUNT);
	CString strGamename;
	WORD wKindID=0;

	TCHAR chTemp[16]=TEXT("");
	WORD x=1,y=20,w=178,h=159;	//窗口位置
	GameRoomRec stGameRoomRec;
	for(BYTE i=0;i<nCount;i++)
	{
		CString strGameNameImg("");
		WORD wLight = 0;
		float f = (float)m_arrayGameRoomInfo[i].m_pListServer->m_GameServer.dwOnLineCount / (float)m_arrayGameRoomInfo[i].m_pListServer->m_GameServer.dwMaxCount;
		if (f >= 0.7f) wLight = 2;	//红灯
		else if (f >= 0.5f)wLight = 1;	//黄灯
		else wLight = 0;	//绿灯

		m_GameRoomDlg[i].SetMode(2);//
		wKindID=m_arrayGameRoomInfo[i].GetKindID();
				
		if (ACTIVE_ID == m_arrayGameRoomInfo[i].m_pListServer->m_GameServer.wKindID)
		{
			strGameNameImg.Append(_T("BKActive_"));
			_snprintf(chTemp,sizeof(chTemp),TEXT("%d"), m_arrayGameRoomInfo[i].m_pListServer->m_GameServer.wSortID);
		}
		else
		{
			strGameNameImg.Append(_T("BKRoom_"));
			_snprintf(chTemp,sizeof(chTemp),TEXT("%d"),i+1);
		}
		strGameNameImg.Append(chTemp);
		strGameNameImg.Append(_T(".png"));
		stGameRoomRec.strImgNameBK=strGameNameImg;
		stGameRoomRec.strImgNameFT=TEXT("FTRoom.png");
		stGameRoomRec.strImgNameFocus=TEXT("ImageSelectRoom.png");
		stGameRoomRec.wKindID=wKindID;
		stGameRoomRec.wLight=wLight;
		stGameRoomRec.wSortID=m_arrayGameRoomInfo[i].GetSortId();
		stGameRoomRec.lCellScore=m_arrayGameRoomInfo[i].m_pListServer->m_GameServer.lCellScore;
		stGameRoomRec.lRestrictScore=m_arrayGameRoomInfo[i].m_pListServer->m_GameServer.lRestrictScore;
		stGameRoomRec.lEnterMoney=m_arrayGameRoomInfo[i].m_pListServer->m_GameServer.lEnterMoney;
		if (3==i)
		{
			stGameRoomRec.iDifference=0;//按钮差异位置
		}
		else
		{
			stGameRoomRec.iDifference=-i;//按钮差异位置
		}
		m_GameRoomDlg[i].SetResourceInfo(stGameRoomRec);//初始化变量
		m_GameRoomDlg[i].SetWndPos(x, y, w, h);//设置窗口位置

		x += w;
		if ((i % 3)==2)
		{
			x=0;
			y += h;
			y=y+5;
		}
	}
	for (int j=0; j<IGAME_ROOM_UNIT_COUNT; ++j)
	{//隐藏所以窗口
		m_GameRoomDlg[j].SetWndHide();
	}
	for (int j=0; j<nCount; ++j)
	{//显示创建好的窗口
		m_GameRoomDlg[j].SetWndShow();
	}

	InvalidateRect(NULL);
}
//创建游戏单元
void CRoomListCenterDlg::CreateRoomUnit()
{
	for (int i=0; i<IGAME_ROOM_UNIT_COUNT; ++i)
	{
		m_GameRoomDlg[i].Create(CGameRoomDlg::IDD, this);
	}
}
//当前路径
void CRoomListCenterDlg::GetCurrentDirectoryPath(CString &szPath)
{
	szPath="";
	TCHAR szPathTemp[MAX_PATH]={0};
	GetCurrentDirectory(MAX_PATH, szPathTemp);
	szPath.Append(szPathTemp);
}

//大厅资源路径
void CRoomListCenterDlg::GetImagePlazaPath(CString& strRetValue)
{
	CString strModuleFileName("");	GetCurrentDirectoryPath(strModuleFileName);
	strModuleFileName.Append("\\data\\plaza\\");
	strRetValue = strModuleFileName;
}


BOOL CRoomListCenterDlg::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	return TRUE;
	//return CDialog::OnEraseBkgnd(pDC);
}

void CRoomListCenterDlg::OnDestroy()
{
	CDialog::OnDestroy();

	m_memdc.DeleteDC();
	m_ImageBK.DestroyImage();
	// TODO: 在此处添加消息处理程序代码
}
