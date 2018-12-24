#pragma once

#include "resource.h"
#include "afxwin.h"
// CGameListHot 对话框

class CGameListHot : public CDialog
{
	DECLARE_DYNAMIC(CGameListHot)

public:
	CGameListHot(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CGameListHot();

// 对话框数据
	enum { IDD = IDD_GAMELIST_HOT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

private:
	//CPngImage							m_imgLandCrazy;					// 斗地主
	//CPngImage							m_imgSparrowZZ;					// 长沙麻将
	//CPngImage							m_imgSparrowYY;					// 益阳麻将
	CPngImage							m_imgFrame;						// 空框
	CPngImage							m_ImageBK;						//背景图片
	CPngImage							m_ImageFlag;					//标志图片
	CRect								m_rectFlag;						//标志位置
	//CRect								m_rectLandCrazyArea;			//斗地主位置区域
	//CRect								m_rectSparrowZZArea;			//长沙麻将位置区域
	//CRect								m_rectSparrowYYArea;			//益阳麻将位置区域
	CRect								m_rectFlagArea;					//标志位置区域

	//bool								m_bIsDown;						
	
	CSkinButton							m_btnLandCrazy;					//斗地主
	CSkinButton							m_btnSparrowZZ;					//转转麻将
	CSkinButton							m_btnSparrowYY;					//益阳麻将
	CSkinButton							m_btnSiSeQiu;					///< 四色球

private:
	//CDC									m_memdc;						
	//CString								m_strCurrentPathPlaza;			//大厅资源路径

public:
	//WORD								m_wLandCrazyPos;				//绘画位置
	//WORD								m_wSparrowZZPos;				//绘画位置
	//WORD								m_wSparrowYYPos;				//绘画位置
	bool								m_bFlag;						//是否绘画标志
	
public:
	virtual BOOL OnInitDialog();
	// 斗地主
	afx_msg void OnBnClickedLandcrazy();
	// 长沙麻将
	afx_msg void OnBnClickedSparrowZz();
	// 益阳麻将
	afx_msg void OnBnClickedSparrowYy();
	/// 四色球
	afx_msg void OnBnClickedSiSeQiu();

private:
	//获取当前路径
	void GetCurrentDirectoryPath(CString &szPath);
	//大厅路径
	void GetImagePlazaPath(CString& strRetValue);
public:
	afx_msg void OnPaint();

public:
	//设置标志
	void SetFlag(bool bFlag);
	//设置标志位置
	void SetFlagPos(int x, int y);
	//设置无效区域
	void SetInvalidateRect();
	//设置游戏ID
	void SetGameID(WORD wKinds);

	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnMButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
