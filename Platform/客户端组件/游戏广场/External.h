#if !defined(AFX_EXTERNAL_H__0859966D_DDDF_4837_BB7E_695C0B05C639__INCLUDED_)
#define AFX_EXTERNAL_H__0859966D_DDDF_4837_BB7E_695C0B05C639__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// External.h : header file
//
#include "PlazaDlg.h"
//#include "Globle.h"

/////////////////////////////////////////////////////////////////////////////
// CExternal command target

class CExternal : public CCmdTarget
{
	DECLARE_DYNCREATE(CExternal)

	CExternal();           // protected constructor used by dynamic creation
	virtual ~CExternal();

// Attributes 
// Operations
public:
	CPlazaDlg * m_palazaDlg;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CExternal)
	public:
	virtual void OnFinalRelease();
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CExternal)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
	// Generated OLE dispatch map functions
	//{{AFX_DISPATCH(CExternal)
	afx_msg void Refresh();

	/*/////////////////////////////////MoveApp////////////////////////////////
	afx_msg void Move(BSTR urls);
	////////////////////////////////ProductApp//////////////////////////////	 
	afx_msg void ProductApp_FillData(BSTR type);
	///////////////////////////////AccountManagementApp////////////////////
	afx_msg void AccountManagementApp_Add();
	afx_msg void AccountManagementApp_Modify(BSTR uid);
	afx_msg void AccountManagementApp_Delete(BSTR uid);
	///////////////////////////////MaskApp////////////////////////////////
	afx_msg void MaskApp_ManualLogin();*/

	afx_msg void CallRoomList(BSTR gamename);
	afx_msg void CallTableList(BSTR gamename);
	afx_msg void CloseWindow(int nIDD);
	afx_msg void UpdateClientDJInfo(int iDJID);
	afx_msg void UpdateUserGold(DWORD dwGold);
	afx_msg void UpdateUserMoney(DWORD dwMoney);

	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()
	


public:
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EXTERNAL_H__0859966D_DDDF_4837_BB7E_695C0B05C639__INCLUDED_)
