#pragma once
#include "resource.h"

class CSkinDlg : public CDialogImpl<CSkinDlg>
{
public:
	enum { IDD = IDD_DIALOG1};
	CSkinDlg(void);
	~CSkinDlg(void);

	BEGIN_MSG_MAP(CSkinDlg)
		MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLButtonDown)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_ID_HANDLER(IDCANCEL, OnClose)
		COMMAND_ID_HANDLER(IDOK, OnOK)
	END_MSG_MAP()

	LRESULT OnLButtonDown(UINT uMessage, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
    LRESULT OnInitDialog(UINT uMessage, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
	LRESULT OnClose(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnOK(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
};
