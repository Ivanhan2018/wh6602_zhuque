#include "stdafx.h"
#include ".\skindlg.h"
#include "SkinMgr.h"

CSkinDlg::CSkinDlg(void)
{
}

CSkinDlg::~CSkinDlg(void)
{
}


LRESULT CSkinDlg::OnInitDialog(UINT uMessage, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	CenterWindow(GetParent());
	return TRUE;
}

LRESULT CSkinDlg::OnLButtonDown(UINT uMessage, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	return 0;
}

LRESULT CSkinDlg::OnClose(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	DestroyWindow();
	return 0;
}

LRESULT CSkinDlg::OnOK(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	//::SetWindowPos(GetDlgItem(IDOK), NULL, 10, 200, 150, 30, SWP_NOZORDER);
	if(GetSonicUI()->SkinFromHwnd(m_hWnd) == NULL)
	{
		ISonicSkin * pSkin = GetSonicUI()->CreateSkin();
		pSkin->SetSkin(_T("bg"), _T("image:%d; title_text:'/font, font_height=14, font_bold=1/²âÊÔ¶Ô»°¿ò'; close:%d; btn_top_margin:6; color_key:%d"), 
			_SkinMgr.pImgDlg->GetObjectId(), _SkinMgr.pImgClose->GetObjectId(), RGB(128, 0, 128));
		pSkin->SetSkin(_T("button"), _T("image:%d; btn_ctrl:'animation=30, linkc=0';"), _SkinMgr.pImgButton->GetObjectId());
		pSkin->Attach(m_hWnd);
/*
		ISonicWndEffect * pEffect = GetSonicUI()->CreateWndEffect();
		pEffect->Attach(m_hWnd);
		pEffect->SetLayeredAttributes(RGB(128, 0, 128), 0, 1);
*/
	}
	return 0;
}