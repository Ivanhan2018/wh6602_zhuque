#pragma once
#include "afxwin.h"
#include "BitmapSlider.h"
#include "Resource.h"
// CGameSoundSet 对话框


class CGameFrameDlg;

class CGameSoundSet : public CImgDialogBase
{
	DECLARE_DYNAMIC(CGameSoundSet)

public:
	CGameSoundSet(LPCTSTR lpszFile, CWnd* pParent = NULL);	// standard constructor
	virtual ~CGameSoundSet();

//// 对话框数据
//	enum { IDD = IDD_DIALOG_GAMESET };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

private:

	CGameFrameDlg*	m_pGameFrameDlg;

private:
	CPngImage		m_BackImage;


public:
	virtual BOOL OnInitDialog();
	virtual void OnDraw(Graphics & graphics);

public:
	BOOL SetImageRgn(CPngImage * pImage);
	afx_msg void OnPaint();
};
