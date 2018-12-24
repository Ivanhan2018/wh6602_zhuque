#pragma once

#include "resource.h"
#include "GameRoomDlg.h"

const int IGAME_COUNT = 9;

class CGameListCenterDlg : public CDialog
{
	DECLARE_DYNAMIC(CGameListCenterDlg)

public:
	CGameListCenterDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CGameListCenterDlg();

// 对话框数据
	enum { IDD = IDD_GAME_LIST_CENTER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

private:
	CPngImage							m_ImageBK;						//背景图片

private:
	CDC									m_memdc;						
	CGameRoomDlg						m_GameRoomDlg[IGAME_COUNT];				//游戏
private:
	//获取当前路径
	void GetCurrentDirectoryPath(CString &szPath);
	//大厅路径
	void GetImagePlazaPath(CString& strRetValue);
	//创建游戏单元
	void CreateGameUnit();

public:
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();

public:
	//设置游戏列表
	void SetGameList();
	//隐藏所以游戏
	void SetWndHide();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};
