#pragma once
#include "Resource.h"
#include <vector>

//#include " E:/ZhuQue/Platform/客户端组件/游戏框架/GameFrame.h"
#include "ClientKernel.h"
#include "GameFrameDlg.h"

using namespace std;
// CLanguageList 对话框

//列表信息
typedef struct
{
	UINT	uiIndex;		//序号
	char	chName[128];	//键名
	char	chValue[512];	//键值
	CRect	rcArea;			//显示区域
}LListInfo;

class CLanguageList : public CDialog
{
	DECLARE_DYNAMIC(CLanguageList)

public:
	CLanguageList(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CLanguageList();

// 对话框数据
	enum { IDD = IDD_DLG_GAME_LAUGUAGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg LRESULT OnMouseLeave(WPARAM wparam, LPARAM lparam);
	afx_msg void OnPaint();

public:
	//获得语音
	void GetLanguage(DWORD dwIndex, CString& strName, CString& strValue);
	//设置客户端内核
	void SetClientKernel(IClientKernel * pIClientKernel);

public:
	afx_msg void OnLeft();
	afx_msg void OnRight();
public:
	//初始化数据
	void InitData();

private:
	//检测鼠标位置
	int CheckMousePos(const CPoint& point);
	//计算起点与终点
	void CountSEPoint();
	//设置窗口
	//BOOL SetImageRgn(CPngImage * pImage);

private:
	//总页数
	DWORD		m_dwTotalPageNums;
	//当前页数
	DWORD		m_dwCurPageIndex;
	//起点
	DWORD		m_dwIndexStart;
	//终点
	DWORD		m_dwIndexEnd;
	//鼠标当前Index
	int			m_dwMouseCurIndex;

private:
	//列表信息
	std::vector<LListInfo>	m_vecListInfo;
	//显示区域
	CRect			m_rcShowRect;
	//文本显示区域
	CRect			m_rcPageShorRect;

private:
	//字体
	CFont		m_TextFont;

	//背景
	CPngImage	m_BackImage;
	//移动光条
	CPngImage	m_MoveImage;
	//左
	CSkinButton	m_btLeft;
	//右
	CSkinButton	m_btRight;

private:
	IClientKernel*	m_pClientKernel;


private:
	bool		m_bIsCanSend;

public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnTimer(UINT nIDEvent);
};
