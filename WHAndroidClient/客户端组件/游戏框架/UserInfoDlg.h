#pragma once

#include "Resource.h"
#include "GameAffiche.h"
#include "ShowDJInfo.h"
#include "UserDaoJu.h"
#include "LanguageList.h"
#include <string>
#include "ImageNums.h"


class CGameFrameDlg;
class CGameFrameControl;


using namespace std;

// CUserInfoDlg 对话框

class CUserInfoDlg : public CDialog
{
	DECLARE_DYNAMIC(CUserInfoDlg)

public:
	CUserInfoDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CUserInfoDlg();

// 对话框数据
	enum { IDD = IDD_USERINFODLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

private:
	CPngImage		m_ImageBack;		//背景
	CPngImage		m_InfoBack;			//信息背景
	CPngImage		m_GoldBack;			//金币背景
	CPngImage		m_GemsBack;			//宝石背景
	CPngImage		m_NumImage;			//数字图片
	CPngImage		m_ClassNumImage;	//等级数字

	//CPngImage		m_GameInfoBack;		//游戏公告背景

	CSkinButton		m_btSound;			//声音
	CSkinButton		m_btBB;				//背包

private:
	CImageNums		m_ClassNum;			//等级
	CImageNums		m_GoldNum;			//金币
	CImageNums		m_GemsNum;			//宝石

	CToolTipCtrl	m_ToolTipCtrl;		//提示控件
	CPoint			m_ptWnd;			//窗口坐标

private:
	//用户数据
	tagUserData*	m_pUserData;		//用户数据


	//走马灯子窗口
	CGameAffiche				m_Affiche;						//
	//游戏用户道具
	CUserDaoJu					* m_pGameUserDaoJu;		
	//框架指针
	CGameFrameDlg				* m_pGameFrameDlg;
	//语音列表
	CLanguageList				* m_pLanguageListPtr;


private:
	std::string					m_strGradeName;


public:
	CGameFrameControl			* m_pGameFrameControl;				//控制视图

public:
	afx_msg void OnPaint();
	virtual BOOL OnInitDialog();

public:
	//BOOL SetImageRgn(CPngImage * pImage);
	//设置用户信息
	void SetUserData(tagUserData* pUserData);
	//设置道具信息
	void SetShowDJInfo(CShowDJInfo* pShowDJInfoPtr);
	//设置语音列表
	void SetLLInfoPtr(CLanguageList* pLLPtr);
	//获得道具列表信息
	CUserDaoJu* GetUserDaoJuListPtr();
	//设置框架指针
	void SetGameFrameDlgPtr(CGameFrameDlg* pFrameDlg);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//激活按钮
	void EnableBTWindow();
	//更新用户数据
	void UpdateUserData(CMD_GF_NEW_GIFT* pNewData);


	//艺术字体
	void DrawTextString(CDC * pDC, LPCTSTR pszString, COLORREF crText, COLORREF crFrame, int nXPos, int nYPos);
public:
	//语音
	afx_msg void OnLanguageList();
	//背包
	afx_msg void OnOpenBB();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	BOOL SetWindowPos(const CWnd* pWndInsertAfter, int x, int y,
		int cx, int cy, UINT nFlags);
	// 绘画背景 -处理非矩形边框
	void DrawBackground(CDC *pDC, int nWidth, int nHeight, int iExcursionY=0);
};
