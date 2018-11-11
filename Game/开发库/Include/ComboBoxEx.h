#ifndef __H__COMBOBOX
#define __H__COMBOBOX

#pragma once
#include "SkinControls.h"

/***************************************************/
/*2007-04-07			廖明祯			QQ:82358112*/
/***************************************************/

//==========================================================================
// ComboBoxEx(下拉链表)
//@:注意	如果重载了OnPaint()函数请调用DrawBorDer(CDC* pDC)函数
class SKIN_CONTROL_CLASS ComboBoxEx : public CComboBox
{
	DECLARE_DYNAMIC(ComboBoxEx)

public:
	ComboBoxEx();
	virtual ~ComboBoxEx();	
	virtual void PreSubclassWindow();	

public:
	//绘制边框与背景色
	void DrawBorder(CDC* pDC);

	//设置使用默认配置
	void SetDefault();

	//设置背景颜色
	void SetBKColor(COLORREF bkcolor);

	//设置编辑框背景颜色
	void SetEditBKColor(COLORREF bkcolor);

	//设置编辑框中的按钮偏移
	void SetEditBTX(int iabsX=0);
	void SetEditBTY(int iabxY=0);

	//设置字体颜色
	void SetFontColor(COLORREF fontcolor);

	//设置下拉按钮背景图
	void SetArrowBKImage(PTCHAR Path);
	void SetArrowBKImage(class CSkinImage* pTmpImg);

protected:	
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg LRESULT OnMouseLeave(WPARAM wparam, LPARAM lparam);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	afx_msg void OnPaint();	
	DECLARE_MESSAGE_MAP()

private:
	void DrawArrow();

private:
	class CSkinComBoboxRes*	pData;	
public:
    afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
};

#endif//__H__COMBOBOX