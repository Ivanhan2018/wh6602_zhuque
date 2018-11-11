#pragma once

#include "Resource.h"
#include <vector>
#include "ThreadGuard.h"

using namespace std;
// CTaskFinishFlash 对话框

class CTaskFinishFlash : public CDialog
{
	DECLARE_DYNAMIC(CTaskFinishFlash)

public:
	CTaskFinishFlash(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CTaskFinishFlash();

// 对话框数据
	enum { IDD = IDD_DLG_TASK_FINISH };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnTimer(UINT nIDEvent);
	virtual BOOL OnInitDialog();

	BOOL UpdateClockDisplay(int Transparent);

	//播放动画
	void Play(bool bIsPlay, UINT uiNums);

private:
	CPngImage			m_pImageBack;
	CPngImage			m_pImageText;
	CPngImage			m_pImageNums;

	BLENDFUNCTION m_Blend;
	CDC m_hdcMemory;
	int m_BakWidth , m_BakHeight ;
	HINSTANCE hFuncInst ;
	typedef BOOL (WINAPI *MYFUNC)(HWND,HDC,POINT*,SIZE*,HDC,POINT*,COLORREF,BLENDFUNCTION*,DWORD);          
	MYFUNC UpdateLayeredWindow;
	int	m_iTransparent;

	//绘制文本
	void DrawText(CDC* pDC);
private:
	CPoint		m_pClientPos;

	//帧动画
	int			m_iCurFrame;
	

	//数字
	std::vector<int>	m_vecNumList;

	//是否显示文本
	bool		m_bIsShowText;

	//互斥量
	CCriticalSpection	m_CriticalSpection;
};
