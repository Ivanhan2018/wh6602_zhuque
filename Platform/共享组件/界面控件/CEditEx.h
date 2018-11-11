//#include "StdAfx.h"
#pragma once
#include "SkinControls.h"
#ifndef __H__CEDIT
#define __H__CEDIT


/***************************************************/
/*2007-04-07			廖明祯			QQ:82358112*/
/***************************************************/

//==========================================================================
// CEditEx（编辑控件）
//@:注意:如果在Res资源里使用了本控件必须的设置它的风格包含有WS_BORDER属性.
//@:注意:如果是动态创建的控件不管你是否包含了WS_BORDER属性控件都会自动加上这个属性
class SKIN_CONTROL_CLASS CEditEx : public CEdit
{	
	DECLARE_DYNAMIC(CEditEx)

public:
	CEditEx();
	virtual ~CEditEx();
	virtual void PreSubclassWindow();
	virtual BOOL OnChildNotify(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pLResult);

	//设置使用默认配置
	void SetDefault();

	//设置背景颜色
	void SetBKColor(COLORREF bkcolor);

	//设置字体颜色
	void SetFontColor(COLORREF fontcolor);

protected:
	afx_msg void OnNcPaint();	
	afx_msg void OnPaint();
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	DECLARE_MESSAGE_MAP()	

protected:
	virtual void DrawBorder();
	
private:
	class CSkinEditRes*	pData;
};


#endif// __H__CEDIT