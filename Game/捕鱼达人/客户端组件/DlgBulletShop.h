#ifndef BULLET_SHOP_DLG_HEAD_FILE
#define BULLET_SHOP_DLG_HEAD_FILE
#include "Stdafx.h"
#include "DxSkin.h"
#include "DxDialog.h"
#include "BulletHelpDlg.h"

#pragma once

//按钮索引
enum enButtonsID
{
	enButtonsID_Begin=100,
	enButtonsID_Invalidate,

	enButtonsID_Bullet100,
	enButtonsID_Bullet600,
	enButtonsID_Bullet800,
	enButtonsID_Return,
	enButtonsID_Buy,
	enButtonsID_Help,

	enButtonsID_End
};

//类声明
class CGameClientViewDX;

//子弹购买
class CDlgBulletShop : public CDialog
{
	//状态变量
private:
	enButtonsID							m_CurButtonsID;						//当前按钮
	enButtonsID							m_CurBulletID;						//当前按钮
	bool								m_bLeftButtonDown;					//鼠标按下
	enBulletCountKind					m_BulletCountKindSelected;			//选择类型

	//游戏变量
protected:
	LONG								m_lBulletCharge[enBulletCountKind_End];	//子弹费用
	LONG								m_lCellScore;							//单元积分

	//位置变量
private:
	CRect								m_rcBullet100;						//位置变量
	CRect								m_rcBullet600;						//位置变量
	CRect								m_rcBullet800;						//位置变量
	CRect								m_rcReturn;							//位置变量
	CRect								m_rcBuy;							//位置变量
	CRect								m_rcHelp;							//位置变量

	//界面变量
protected:
	CPngImage							m_PngBack;							//桌面背景
	CPngImage							m_PngBullet100;						//子弹图片
	CPngImage							m_PngBullet600;						//子弹图片
	CPngImage							m_PngBullet800;						//子弹图片
	CPngImage							m_PngReturn;						//返回图片
	CPngImage							m_PngBuy;							//购买图片
	CPngImage							m_PngHelp;							//帮助图片

	//组件变量
protected:
	CGameClientViewDX					* m_pGameClientViewDX;				//视图窗体

	//函数定义
public:
	//构造函数
	CDlgBulletShop(CGameClientViewDX * pGameClientViewDX, enBulletCountKind	BulletCountKindSelected, LONG lCellScore);
	//析构函数
	virtual ~CDlgBulletShop();

	//继承函数
protected:
	//控件绑定
	virtual void DoDataExchange(CDataExchange* pDX);
	//初始函数
	virtual BOOL OnInitDialog();

	//接口函数
public:
	//子弹类型
	enBulletCountKind GetBulletKind();
	//设置费用
	void SetBulletCharge(LONG lBulletCharge[enBulletCountKind_End]);

	//继承接口
protected:
	//绘画函数
	afx_msg void OnPaint();    	

	//消息相应
public:
	//鼠标按下
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//移动消息
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//命令消息
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//弹起消息
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);

	//辅助函数
private:
	//返回索引
	enButtonsID GetButtonID(CPoint &ptPos);

	//消息映射
	DECLARE_DYNAMIC(CDlgBulletShop)
	DECLARE_MESSAGE_MAP()	
};

#endif