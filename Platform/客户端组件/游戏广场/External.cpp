// External.cpp : implementation file
//

#include "stdafx.h" 
#include "TableFrame.h"//by hxh 
#include "External.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CExternal

IMPLEMENT_DYNCREATE(CExternal, CCmdTarget)

CExternal::CExternal()
{
	EnableAutomation();
 
}

CExternal::~CExternal()
{
}


void CExternal::OnFinalRelease()
{
	// When the last reference for an automation object is released
	// OnFinalRelease is called.  The base class will automatically
	// deletes the object.  Add additional cleanup required for your
	// object before calling the base class.

	CCmdTarget::OnFinalRelease();
}


BEGIN_MESSAGE_MAP(CExternal, CCmdTarget)
	//{{AFX_MSG_MAP(CExternal)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CExternal, CCmdTarget)
	//{{AFX_DISPATCH_MAP(CExternal)
	//DISP_FUNCTION(CExternal, "ShowDlg", ShowDlg, VT_EMPTY, VTS_NONE)
	/*//////////////////////////////////MoveApp///////////////////////////////////////////////
	DISP_FUNCTION(CExternal, "Move", Move, VT_EMPTY, VTS_BSTR)
	//////////////////////////////////ProductApp///////////////////////////////////////////	 
	DISP_FUNCTION(CExternal, "ProductApp_FillData", ProductApp_FillData, VT_EMPTY, VTS_BSTR)
	//////////////////////////////////AccountManagementApp///////////////////////////////////////////
	DISP_FUNCTION(CExternal, "AccountManagementApp_Add", AccountManagementApp_Add, VT_EMPTY, VTS_NONE)		
	DISP_FUNCTION(CExternal, "AccountManagementApp_Modify", AccountManagementApp_Modify, VT_EMPTY, VTS_BSTR)		
	DISP_FUNCTION(CExternal, "AccountManagementApp_Delete", AccountManagementApp_Delete, VT_EMPTY, VTS_BSTR)	
	//////////////////////////////////MaskApp///////////////////////////////////////////////////////
	DISP_FUNCTION(CExternal, "MaskApp_ManualLogin", MaskApp_ManualLogin, VT_EMPTY, VTS_NONE)	*/
	
	DISP_FUNCTION(CExternal,"CallRoomList",CallRoomList,VT_EMPTY,VTS_WBSTR)
	DISP_FUNCTION(CExternal,"CallTableList",CallTableList,VT_EMPTY,VTS_WBSTR)
	DISP_FUNCTION(CExternal,"CloseWindow",CloseWindow,VT_EMPTY,VTS_I4)
	DISP_FUNCTION(CExternal,"UpdateClientDJInfo",UpdateClientDJInfo,VT_EMPTY,VTS_I4)
	DISP_FUNCTION(CExternal,"UpdateUserGems",UpdateUserGold,VT_EMPTY,VTS_UI4)
	DISP_FUNCTION(CExternal,"UpdateUserGameGold",UpdateUserMoney,VT_EMPTY,VTS_UI4)
	DISP_FUNCTION(CExternal,"Refresh",Refresh,VT_EMPTY,VTS_NONE)
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

// Note: we add support for IID_IExternal to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the 
//  dispinterface in the .ODL file.

// {016AF104-4062-495C-9083-99D906D3A65D}
static const IID IID_IExternal =
{ 0x16af104, 0x4062, 0x495c, { 0x90, 0x83, 0x99, 0xd9, 0x6, 0xd3, 0xa6, 0x5d } };

BEGIN_INTERFACE_MAP(CExternal, CCmdTarget)
	INTERFACE_PART(CExternal, IID_IExternal, Dispatch)
END_INTERFACE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CExternal message handlers
void CExternal::Refresh()
{//刷新大厅的头像和数值
	//AfxMessageBox("test");
	m_palazaDlg->UpdateUserInfo();
}
 void CExternal::CallRoomList( BSTR gamename )
{//点击单个游戏，网页返回的游戏ID
	CString gn(gamename);
	//cxf1022 m_palazaDlg->m_roomListDlg.m_wKindID= _tcstoul(gn, NULL, 10);
	m_palazaDlg->m_RoomListCenterDlg.m_wKindID= _tcstoul(gn, NULL, 10);
	m_palazaDlg->PostMessage(WM_COMMAND,IDM_CALL_ROOM_LIST,0);
	 
}

void CExternal::CallTableList( BSTR gamename )
{	
	CString gn(gamename);
	BYTE i=0;
	
	//房间搜索
	if(atoi(gn) <= m_palazaDlg->m_gameListManager.m_arrayGameRoomInfo.GetCount())
	{
		for(i=0;i<m_palazaDlg->m_gameListManager.m_arrayGameRoomInfo.GetCount();i++)
		{
			if(atoi(gn) == m_palazaDlg->m_gameListManager.m_arrayGameRoomInfo[i].GetSortId())
			{
				//cxf1022 m_palazaDlg->m_roomListDlg.m_nRoomNum= atoi(gn);
				m_palazaDlg->m_RoomListCenterDlg.m_wSortID= atoi(gn);
				m_palazaDlg->PostMessage(WM_COMMAND,IDM_CALL_TABLE_LIST,0);
				return;
			}
		}
	}
	
	//无效房间
	//cxf1022 m_palazaDlg->m_roomListDlg.m_nRoomNum= -1;
	m_palazaDlg->m_RoomListCenterDlg.m_wSortID= -1;
	m_palazaDlg->PostMessage(WM_COMMAND,IDM_CALL_TABLE_LIST,0);
}
void CExternal::CloseWindow(int nIDD)
{
	//m_palazaDlg->CloseWebWindow(nIDD);
}

//更新道具信息
void CExternal::UpdateClientDJInfo(int iDJID)
{
	if (NULL!=m_palazaDlg)
	{
		m_palazaDlg->UpdateUserDJInfo(iDJID);
	}
}
//更新用户金币
void CExternal::UpdateUserGold(DWORD dwGold)
{
	if (NULL!=m_palazaDlg)
	{
		m_palazaDlg->UpdateUserGold(dwGold);
	}
}
//更新用户藏宝币
void CExternal::UpdateUserMoney(DWORD dwMoney)
{
	if (NULL!=m_palazaDlg)
	{
		m_palazaDlg->UpdateUserMoney(dwMoney);
	}
}


BOOL CExternal::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	// TODO: 在此添加专用代码和/或调用基类
	if (nID == IDOK)
	{
		//AfxMessageBox(TEXT("CExternal::OnCmdMsg nID==IDOK"));
	}
	return CCmdTarget::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}
