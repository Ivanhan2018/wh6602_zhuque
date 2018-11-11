#ifndef HINT_MSG_HEAD_FILE
#define HINT_MSG_HEAD_FILE

#pragma once

#include "ClientShare.h"
#include "Resource.h"

#define MB_CUSTOMER		0x00000007L
//////////////////////////////////////////////////////////////////////////

//消息窗口
class CLIENT_SHARE_CLASS CHintMsgBox : public CSkinDialogEx
{
	//属性变量
protected:
	UINT							m_uType;							//消息类型
	CString							m_strString;						//提示消息
	CString							m_strCaption;						//信息标题

	//辅助变量
protected:
	UINT							m_nElapse;							//剩余时间
	UINT							m_nBtCount;							//按钮数目
	CRect							m_rcString;							//字符位置
	HINSTANCE						m_hResInstance;						//资源句柄

	CSkinImage m_imgMsgBox_LeftTop;
	CSkinImage m_imgMsgBox_Left;
	CSkinImage m_imgMsgBox_LeftBottom;
	CSkinImage m_imgMsgBox_Top;
	CSkinImage m_imgMsgBox_RightTop;
	CSkinImage m_imgMsgBox_Right;
	CSkinImage m_imgMsgBox_RightBottom;
	CSkinImage m_imgMsgBox_Bottom;
	CSkinImage m_imgMsgBox_Center;

	CDC m_memdc;
	//控件变量
protected:
	CSkinButton						m_btButton[3];						//按钮控件
	CSkinButton						m_btnClose;
	//函数定义
public:
	//构造函数
	CHintMsgBox(CWnd * pParentWnd=NULL);
	//析构函数
	virtual ~CHintMsgBox();

	// 对话框数据
	enum { IDD = IDD_HINT_MSG };

	//重载函数
protected:
	//初始化消息
	virtual BOOL OnInitDialog();

	//功能函数
public:
	//显示消息
	INT ShowMessageBox(LPCTSTR pszString, UINT uType=MB_OK, UINT nElapse=0);
	//显示消息
	INT ShowMessageBox(LPCTSTR pszCaption, LPCTSTR pszString, UINT uType=MB_OK, UINT nElapse=0);
	//边框宽度
	INT GetXBorder(){return 2;}
	//边框高度
	INT GetYBorder(){return 2;}

	//绘图消息
public:
	bool Draw(CDC * pDC, CDC * pMemdc);
	bool DrawBack(CDC * pDC);
	void FillRectImage(CDC * pDC, CImage & img, const CRect & rect);

private:
	//获取当前路径
	void GetCurrentDirectoryPath(CString &szPath);
	//大厅路径
	void GetImageClientSharePath(CString& strRetValue);

	//内部函数
private:
	//创建按钮
	VOID CreateBoxButton();

	void AddRectRgn(CDC* pDC,CRgn* rgn,CRect* rect,COLORREF col);
	void UpdateRgn(CDC * pDC);
	//消息映射
protected:
	//绘画消息
	VOID OnPaint();
	//时间消息
	VOID OnTimer(UINT_PTR nIDEvent);
	DECLARE_MESSAGE_MAP()

	afx_msg void OnBnClickedYes() { EndDialog(IDYES);}
	afx_msg void OnBnClickedNo() { EndDialog(IDNO);}
	afx_msg void OnBnClickedAbort() { EndDialog(IDABORT);}
	afx_msg void OnBnClickedRetry() { EndDialog(IDRETRY);}
	afx_msg void OnBnClickedIgnore() { EndDialog(IDIGNORE);}
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

};


//////////////////////////////////////////////////////////////////////////

//显示消息
extern "C" CLIENT_SHARE_CLASS INT __cdecl ShowInformation(LPCTSTR pszString, UINT nElapse, UINT uType);
extern "C" CLIENT_SHARE_CLASS INT __cdecl ShowInformationEx(LPCTSTR pszString, UINT nElapse, UINT uType ,LPCTSTR pszCaption);


//////////////////////////////////////////////////////////////////////////

#endif