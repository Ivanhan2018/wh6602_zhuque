#pragma once
#include "SkinControls.h"
#include "resource.h"
#include "afxwin.h"
#include "ImageNums.h"
#include "GameRuleDlg.h"

// CGameRoomDlg 对话框
struct GameRoomRec
{
	CString						strImgNameBK;						//背景图片
	CString						strImgNameFT;						//前景图片
	CString						strImgNameFocus;					//获取光标图片
	WORD						wKindID;							//游戏ID
	WORD						wLight;								//房间人数状态标记
	WORD						wSortID;							//游戏排序(1:新手;2:初级.....)
	LONG						lCellScore;							//单元积分
	LONG						lRestrictScore;						//限制最大输分
	int							iDifference;						//按钮坐标位置差异
	LONG						lEnterMoney;						//最低进入金币
	GameRoomRec()
	{
		strImgNameBK="";
		strImgNameFT="";
		strImgNameFocus="";
		wKindID=0;
		wLight=0;
		wSortID=0;
		lCellScore=0;
		lRestrictScore=0;
		iDifference=0;
	}
};

class CGameRoomDlg : public CDialog
{
	DECLARE_DYNAMIC(CGameRoomDlg)

public:
	CGameRoomDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CGameRoomDlg();

// 对话框数据
	enum { IDD = IDD_GAME_ROOM_DLG };

public:
	WORD					m_wMode;						//窗口模型(1:游戏列表;2:房间列表)
	CPngImage				m_ImgBK;						//背景图片
	CPngImage				m_ImgFT;						//前景图片
	CPngImage				m_ImgFocus;						//获取焦点图片
	CPngImage				m_ImgGameRoomFont;				//游戏房间分数
	WORD					m_wKindID;						//游戏ID
	WORD					m_wLight;						//房间人数状态标记
	WORD					m_wSortID;						//游戏排序(1:新手;2:初级.....)
	LONG					m_lCellScore;					//单元积分
	LONG					m_lRestrictScore;				//限制最大输分
	int						m_iDifference;					//按钮坐标位置差异
	LONG					m_lEnterMoney;					//最低进入金币
private:
	CPngImage				m_ImgRoomName;					//房间名字图片
	CPngImage				m_ImgLight;						//房间人数状态标志
	CImageNums				m_ImageNumsHuang;				//数字黄色
	CImageNums				m_ImageNumsLv;					//数字绿色

	//按钮
	CPngImage				m_ImageBTRule;					//游戏规则按钮
	CPngImage				m_ImageBTGame;					//进入游戏按钮

private:
	CDC						m_memdc;						//
	CRect					m_rcClientRect;					//当前窗口大小


private:
	CRect					m_rcGameBT;						//游戏按钮区域
	CRect					m_rcRuleBT;						//规则按钮区域

	//状态: 0:默认状态; 2:鼠标移上去状态; 1:鼠标按下去状态.
	BYTE					m_byGameBTState;				//游戏按钮状态
	BYTE					m_byRuleBTState;				//规则按钮状态

	bool					m_bHovering;
	bool					m_bIsDrawFocus;					//是否绘制焦点

private:
	CGameRuleDlg			m_GameRuleDlg;				//游戏规则

public:
	//设置窗口位置
	void SetWndPos(WORD x, WORD y, WORD w, WORD h);
	//设置窗口模式(1:游戏列表;2:房间列表)
	void SetMode(WORD wMode);
	//设置资源
	void SetResourceInfo(GameRoomRec stGameRoomRec);
	//显示窗口
	void SetWndShow();
	//隐藏窗口
	void SetWndHide();

private:
	//当前路径
	void GetCurrentDirectoryPath(CString &szPath);
	//大厅资源路径
	void GetImagePlazaPath(CString& strRetValue);
	//资源路径
	void GetImageDataPath(CString& strRetValue);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持


public:
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();

	afx_msg void OnBnClickedGameRule();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnBnClickedEnterGame();

	afx_msg LRESULT OnMouseLeave(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMouseHover(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnDestroy();
};
