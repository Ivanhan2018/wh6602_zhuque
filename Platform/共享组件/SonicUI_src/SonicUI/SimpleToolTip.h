#pragma once

// 封装了标准windows的tooltip32
class CSimpleToolTip
{
public:
	CSimpleToolTip(void);
	~CSimpleToolTip(void);
	BOOL Create(HWND hParent);
	void Destory();
	void SetTipText(LPCTSTR lpszText, HWND hWnd, RECT& rt, DWORD dwCurrentHolder);

	// 设置tip的宽度，如果为0，则没有强制宽度，tip都单行显示，否则按所给宽度换行显示
	void SetTipWidth(int nWidth);
	void ClearTip();
	DWORD GetCurrentHolder(){return m_dwCurrentHolder;}
protected:
	HWND m_hTipWnd;
	HWND m_hFakeParent;			// 当前为哪个窗体服务
	DWORD m_dwCurrentHolder;	// 当前为哪个对象服务
};
