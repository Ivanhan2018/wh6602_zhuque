#pragma once

#include "resource.h"
#include "exmain.h"
#include "WebPage.h"
#include "SystemOption.h"
#include "GameListManager.h"
// CRoomListDlg 对话框

#define  MAX_ROOM_UNIT_COUNT			(6)
class CRoomUnit			//房间单元
{
	
public:
	CWnd * m_pParent;
	CRect m_rect;
	bool  m_bEable;
	WORD m_wKindID;
	WORD m_Index;
	WORD m_wLight;				//灯 0 绿, 1, 2
	long m_lCellScore;
	long m_lLessScore;
	CPngImage m_pngGameIcon;	//游戏Icon 

	static CPngImage m_pngBack;		//背景图
	static CPngImage m_pngBeilu;		//“倍率”
	static CPngImage m_pngLimit;		//“限”
	static CPngImage m_pngMoney;		//“宝石”
	static CPngImage m_pngBtn;					//进入按钮
	static CPngImage m_pngManIdentifier;	//人数标识
	static CPngImage m_pngNumber_Max;		    //大字数字
	static CPngImage m_pngNumber_Min;		    //小字数字
	static CPngImage m_pngTitle;		//标题 如：新手区
	//CRgnButton m_btnOk;

public:
	CRoomUnit()
	{
		m_bEable = FALSE;
		memset(&m_rect, 0, sizeof(m_rect));
		m_wKindID=0;
		m_lCellScore = 0;
		m_lLessScore = 0;
	}
	~CRoomUnit(){}
public:
	static void Init();
	static void Uinit();
	void SetInfo(CWnd * pWnd, WORD index, const CRect & rect);
	void EableUnit(bool bEable);
	void SetRect(const CRect & rect) { CopyMemory(&m_rect, &rect, sizeof(m_rect));}
	void SetData(const HINSTANCE & hInstance,  WORD wKindID, WORD wLight, long lCellScore, long lLessScore);//初始化
	void OnPaint(CDC * pDC);
	CSize DarwNumber(CDC * pDC, CPngImage & pngImg, LONG number, const CPoint & pt);
	bool IsFouse();
	int OnLButtonDown(const UINT & nFlags, const CPoint & pt);
	int OnMouseMove(const UINT & nFlags, const CPoint & point);
	WORD GetIndex() { return m_Index; }	
	bool isEableUnit() {return m_bEable;}
};
class CRoomListDlg : public CSkinDialogEx
{
	DECLARE_DYNAMIC(CRoomListDlg)

public:
	CRoomListDlg(/*CWnd* pParent = NULL*/);   // 标准构造函数
	virtual ~CRoomListDlg();

// 对话框数据
	enum { IDD = IDD_ROOM_LIST };
protected:
	//列表
	CExmain		*	m_exMain;
	IStream*		m_iStream;
	IWebBrowser2*	m_pBrowser;
	CWebPage		m_webPage;
	
public:
	WORD m_wKindID;
	int m_nRoomNum;
	CGameListManager * m_gameListManager;
	DWORD							m_dwLimitChatTimes;					//限制聊时
	CRoomUnit						m_Unit[MAX_ROOM_UNIT_COUNT];		//最多6个游戏服务器
	CPngImage						m_pngGameTitle;
public:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
	DECLARE_MESSAGE_MAP()
	virtual BOOL OnInitDialog();

public:
	//afx_msg void OnBnClickedExpression();
	afx_msg void OnBnClickedDepository();
	afx_msg void OnBnClickedFriend();
	afx_msg void OnBnClickedShop();
	afx_msg void OnBnClickedTask();
	afx_msg void OnBnClickedTool();
	virtual void OnPaint();
	//LRESULT OnHitExpression(WPARAM wParam, LPARAM lParam);
protected:
	virtual void OnOK();
public:
	void SetResetHtml();
	void ShowHtml(CString content);
	void EnterRoom();
	afx_msg void OnDestroy();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};
