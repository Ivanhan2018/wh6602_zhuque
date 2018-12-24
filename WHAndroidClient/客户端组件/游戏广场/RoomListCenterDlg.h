#pragma once

#include "resource.h"
#include "GameRoomDlg.h"
#include "GameInfo.h"
#include <afxtempl.h>

// CRoomListCenterDlg 对话框
const			int						IGAME_ROOM_UNIT_COUNT = 4;
class CRoomListCenterDlg : public CDialog
{
	DECLARE_DYNAMIC(CRoomListCenterDlg)

public:
	CRoomListCenterDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CRoomListCenterDlg();

// 对话框数据
	enum { IDD = IDD_ROOM_LIST_CENTER };

private:
	CPngImage							m_ImageBK;									//背景图片

private:
	CDC									m_memdc;	
	CGameRoomDlg						m_GameRoomDlg[IGAME_ROOM_UNIT_COUNT];		//游戏房间单元

public:
	WORD								m_wKindID;									//游戏ID(过度所用-点击游戏单元所传进来的)
	WORD								m_wSortID;									//房间排序号码(过度所用-点击房间单元所传进来的)
	CArray<CGameRoomInfo,CGameRoomInfo>	m_arrayGameRoomInfo;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

public:
	void EnterRoom();//进入游戏列表

private:
	//获取当前路径
	void GetCurrentDirectoryPath(CString &szPath);
	//大厅路径
	void GetImagePlazaPath(CString& strRetValue);
	//创建游戏列表
	void CreateRoomUnit();

public:
	afx_msg void OnPaint();
	virtual BOOL OnInitDialog();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnDestroy();
};
