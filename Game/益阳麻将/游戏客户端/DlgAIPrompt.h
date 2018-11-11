#pragma once

enum ST_SHOWTAG
{
	ST_LEFT = 0,
	ST_CENTER,
	ST_RIGHT,
};

// CDlgAIPrompt 对话框

class CDlgAIPrompt : public CDialog
{
	DECLARE_DYNAMIC(CDlgAIPrompt)

	//配置变量
protected:
	CPoint							m_PointBenchmark;					//基准位置

public:
	CDlgAIPrompt(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgAIPrompt();

// 对话框数据
	enum { IDD = IDD_AI_PROMPT };


public:
	//加载图片
	bool LoadImage(const char* pImagePath);
	//设置牌数据
	void SetCardData(const BYTE* pCardData, int iCardNums);
	//显示窗口
	void ShowWin(POINT ptPoint, ST_SHOWTAG showtag=ST_CENTER);
	//设置牌透明度
	void SetCardAlpha(float fAlpha=1.0f);
	//设置牌绘制起点
	void SetCardDrawXY(POINT ptPoint);

public:
	void AddRectRgn(CDC* pDC,CRgn* rgn,CRect* rect,COLORREF col);
	void UpdateRgn(CDC * pDC);

private:
	CGDIPlus_Image		m_ImageCard;//牌
	CGDIPlus_Image		m_ImageCenter;//中
	CGDIPlus_Image		m_ImageLeft;//左
	CGDIPlus_Image		m_ImageRight;//右
	CGDIPlus_Image      m_ImageCardBack; //牌底

	float				m_fCardAlpha;//透明度
	POINT				m_fDrawXY;	//绘制牌起点

private:
	int					m_iAbs_Left;
	int					m_iAbs_Right;
	int					m_iAbs_Center_X;
	int					m_iAbs_Center_Y;

	int					m_iCurWidth;
	int					m_iCurHeight;

private:
	std::vector<BYTE>	m_vecCardList;	//牌列表


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};
