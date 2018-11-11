// External.cpp : implementation file
//

#include "stdafx.h" 
#include "GameFrameExternal.h"
#include "GameFrame.h"//by hxh 

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGameFrameExternal

IMPLEMENT_DYNCREATE(CGameFrameExternal, CCmdTarget)

CGameFrameExternal::CGameFrameExternal()
{
	EnableAutomation();
 
}

CGameFrameExternal::~CGameFrameExternal()
{
}


void CGameFrameExternal::OnFinalRelease()
{
	// When the last reference for an automation object is released
	// OnFinalRelease is called.  The base class will automatically
	// deletes the object.  Add additional cleanup required for your
	// object before calling the base class.

	CCmdTarget::OnFinalRelease();
}


BEGIN_MESSAGE_MAP(CGameFrameExternal, CCmdTarget)
	//{{AFX_MSG_MAP(CGameFrameExternal)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CGameFrameExternal, CCmdTarget)
	//{{AFX_DISPATCH_MAP(CGameFrameExternal)
	//DISP_FUNCTION(CGameFrameExternal, "ShowDlg", ShowDlg, VT_EMPTY, VTS_NONE)
	/*//////////////////////////////////MoveApp///////////////////////////////////////////////
	DISP_FUNCTION(CGameFrameExternal, "Move", Move, VT_EMPTY, VTS_BSTR)
	//////////////////////////////////ProductApp///////////////////////////////////////////	 
	DISP_FUNCTION(CGameFrameExternal, "ProductApp_FillData", ProductApp_FillData, VT_EMPTY, VTS_BSTR)
	//////////////////////////////////AccountManagementApp///////////////////////////////////////////
	DISP_FUNCTION(CGameFrameExternal, "AccountManagementApp_Add", AccountManagementApp_Add, VT_EMPTY, VTS_NONE)		
	DISP_FUNCTION(CGameFrameExternal, "AccountManagementApp_Modify", AccountManagementApp_Modify, VT_EMPTY, VTS_BSTR)		
	DISP_FUNCTION(CGameFrameExternal, "AccountManagementApp_Delete", AccountManagementApp_Delete, VT_EMPTY, VTS_BSTR)	
	//////////////////////////////////MaskApp///////////////////////////////////////////////////////
	DISP_FUNCTION(CGameFrameExternal, "MaskApp_ManualLogin", MaskApp_ManualLogin, VT_EMPTY, VTS_NONE)	*/
	
	DISP_FUNCTION(CGameFrameExternal,"CallRoomList",CallRoomList,VT_EMPTY,VTS_WBSTR)
	DISP_FUNCTION(CGameFrameExternal,"CallTableList",CallTableList,VT_EMPTY,VTS_WBSTR)
	DISP_FUNCTION(CGameFrameExternal,"CloseWindow",CloseWindow,VT_EMPTY,VTS_I4)
	DISP_FUNCTION(CGameFrameExternal,"UpdateClientDJInfo",UpdateClientDJInfo,VT_EMPTY,VTS_I4)
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

// Note: we add support for IID_IExternal to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the 
//  dispinterface in the .ODL file.

// {016AF104-4062-495C-9083-99D906D3A65D}
static const IID IID_IExternal =
{ 0x16af104, 0x4062, 0x495c, { 0x90, 0x83, 0x99, 0xd9, 0x6, 0xd3, 0xa6, 0x5d } };

BEGIN_INTERFACE_MAP(CGameFrameExternal, CCmdTarget)
	INTERFACE_PART(CGameFrameExternal, IID_IExternal, Dispatch)
END_INTERFACE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGameFrameExternal message handlers

 void CGameFrameExternal::CallRoomList( BSTR gamename )
{
	 
}

void CGameFrameExternal::CallTableList( BSTR gamename )
{	
}
void CGameFrameExternal::CloseWindow(int nIDD)
{
	m_palazaDlg->CloseBizCityWindow(nIDD);
}
BOOL CGameFrameExternal::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	// TODO: 在此添加专用代码和/或调用基类
	return CCmdTarget::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

void CGameFrameExternal::UpdateClientDJInfo(int iDJID)
{
	if (NULL != m_palazaDlg)
	{
		m_palazaDlg->UpdateDJInfo(iDJID);
	}
}
