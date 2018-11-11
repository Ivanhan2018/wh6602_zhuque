#pragma once
#include "afxwin.h"
#include "BitmapSlider.h"
#include "Resource.h"
// CGameSoundSet 对话框

class CGameFrameDlg;

class CGameSoundSet : public CDialog
{
	DECLARE_DYNAMIC(CGameSoundSet)

public:
	CGameSoundSet(CWnd* pParent = NULL);	// standard constructor
	virtual ~CGameSoundSet();

// 对话框数据
	enum { IDD = IDD_DIALOG_GAMESET };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

private:
	CBitmapSlider	m_GamePicCtl;
	CBitmapSlider	m_MusicPicCtl;

	CSkinButton		m_CloseGY;
	CSkinButton		m_CloseGN;

	CSkinButton		m_CloseMY;
	CSkinButton		m_CloseMN;

	CSkinButton		m_OpenGY;
	CSkinButton		m_OpenGN;

	CSkinButton		m_OpenMY;
	CSkinButton		m_OpenMN;

	CGameFrameDlg*	m_pGameFrameDlg;

private:
	CPngImage		m_BackImage;
	CSkinButton		m_btOK;


public:
	virtual BOOL OnInitDialog();
	afx_msg LRESULT OnBitmapSliderMoved(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnBitmapSliderMoving(WPARAM wParam, LPARAM lParam);

	afx_msg void OnCloseGameN();
	afx_msg void OnOpenGameN();
	afx_msg void OnCloseMusicN();
	afx_msg void OnOpenMusicN();
	afx_msg void OnCloseWnd();

	afx_msg void OnCloseGame();
	afx_msg void OnOpenGame();
	afx_msg void OnCloseMusic();
	afx_msg void OnOpenMusic();

public:
	//设置窗口指针
	void SetGameFrameDlgPtr(CGameFrameDlg* pGameFrameDlg);
	//BOOL SetImageRgn(CPngImage * pImage);
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
};
