#pragma once
#include "Resource.h"
#include <string>
#include <vector>
#include "ThreadGuard.h"

// CGameAffiche 对话框

using namespace std;

class CGameAffiche : public CDialog
{
	DECLARE_DYNAMIC(CGameAffiche)

public:
	CGameAffiche(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CGameAffiche();

// 对话框数据
	enum { IDD = IDD_DLG_GAMEAFFICHE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()


public:
	//设置为扩展窗口
	void SetWndExp(DWORD wAlpha=30);
	//设置游戏公告
	void SetGameAfficheInfo(const char* pInfo);
	//解析公告
	void ParseChar(const char* pData, WORD wDataSize);
	//设置背景图
	void SetBackImage(const char* pImage, bool bIsRgnWnd=false);
	//获得是否为扩展窗口
	bool GetIsExp();
	//设置显示区域
	void SetShowRect(CRect rect);
	//设置字体颜色
	void SetTextColor(COLORREF textcol=RGB(255, 255, 255));
	//设置X轴开始位置
	void SetXAbs(DWORD dwXAbs);
	//设置步长
	void SetMoveLeep(DWORD dwLeep=2);
	//设置时间频率
	void SetTimerInterval(DWORD dwTimeInterval=30);
	//开始播放
	void SetPlay(bool bIsPlay=true, int iLoopNums=0);

private:
	//BOOL SetImageRgn(CPngImage * pImage);

private:
	CPngImage						m_ImageBack;						//背景
	CFont							m_fontDrawGameInfo;					//窗口字体
	CDC								m_memdc;							//窗口句柄
	bool							m_bIsDisableGameMsg;				//是否激活游戏公告
	CRect							m_rcGameMsgInfo;					//字体绘制区域
	CRect							m_rcCurMsgInfo;						//当前区域
	std::vector<string>				m_vecGameMsg;						//游戏公告
	int								m_iCountVecIndex;					//当前公告序号
	std::string						m_strSaveGameAffiche;				//保存游戏公告
	bool							m_bIsExp;							//是否扩展窗口
	COLORREF						m_bTextColor;						//字体颜色
	DWORD							m_wStartXAbs;						//X轴开始偏移
	DWORD							m_dwMoveLeep;						//移动步长
	DWORD							m_dwTimeInterval;					//时间间隔
	int								m_iLoopNums;						//0:无限循环
	int								m_iCountLoop;


	//互斥量
	CCriticalSpection	m_CriticalSpection;

public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT nIDEvent);
	virtual BOOL OnInitDialog();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};
