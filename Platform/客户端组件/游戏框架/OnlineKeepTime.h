#pragma once


// COnlineKeepTime 在线送礼，计时类

class COnlineKeepTime : public CWnd
{
	DECLARE_DYNAMIC(COnlineKeepTime)

public:
	COnlineKeepTime();
	virtual ~COnlineKeepTime();
	/// 设置时间
	void SetKeepTime(int iTime);
	/// 播放状态
	/// [@param bPlay in] 为真播放，为假暂停
	void PlayStatus(BOOL bPlay);

private:
	CPngImage				m_imgBackground;		///< 背景
	CPngImage				m_imgMinNumber;			///< 小数字
	CPngImage				m_imgMaxNumber;			///< 大数字
	CPngImage				m_imgMaohao;			///< 冒号

	int						m_iTotalSecond;			///< 总秒数
	int						m_iHour;				///< 时
	int						m_iMinuter;				///< 分
	int						m_iSecond;				///< 秒


protected:
	DECLARE_MESSAGE_MAP()
	// 绘制时间数字
	void DrawTimeNum(CDC *pDC, CRect rc, CPngImage *pImg, int iFrameCount, int iNum);
	// 重新统计时间
	void ReFigureNum(int iTime);
	// 区域统计
	void UpdateRgn(CDC * pDC);
	void AddRectRgn(CDC* pDC,CRgn* rgn,CRect* rect,COLORREF col);

public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
};


