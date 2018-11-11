#pragma once


#include "Stdafx.h"
#include <vector>

#include "ThreadGuard.h"

using namespace std;

//数值掩码
#define	CARD_MASK_COLOR				0xF0								//花色掩码
#define	CARD_MASK_VALUE				0x0F								//数值掩码


//X 排列方式
enum enGameCardXCollocateMode 
{ 
	enGCXLeft,						//左对齐
	enGCXCenter,					//中对齐
	enGCXRight,						//右对齐
};

//扑克结构
struct tagGameCardItem
{
	bool							bShoot;								//弹起标志
	BYTE							bCardData;							//扑克数据
};

//游戏牌控件
class CGameCard
{
public:
	CGameCard();
	~CGameCard(void);

public:
	//设置视图窗口指针
	void SetViewWndPtr(CWnd* pViewWnd);
	
public:
	//添加牌数据
	int SetCardData(const BYTE* pCardData=NULL, int iSize=0);
	//获得牌数据
	int GetCardData(BYTE* pCardData, int iSize);
	//获得弹起的牌
	int GetShootCard(BYTE* pCardShootCard, int iSize);
	//设置弹起的牌
	int SetShootCard(BYTE* pCardShootCard, int iSize);
	//设置所有牌不弹起
	void SetAllShoot(bool bIsShott=false);

public:
	//设置响应
	void SetCardPositively(bool bIsPositively=true);
	//设置显示
	void SetCardShow(bool bIsShow=true);


public:
	//索引切换
	int SwitchCardPoint(const CPoint &Point);
	//鼠标消息
	void OnLButtonDown(CPoint Point);
	//鼠标消息
	void OnLButtonUp(CPoint Point);
	//鼠标移动
	void OnMouseMove(CPoint point);

public:
	//设置牌背景
	void SetCardBackImage(CPngImage* pBackImage);
	//设置牌资源
	bool SetCardImage(CString strImagePath);
	//设置牌选择
	bool SetCardSelImage(CString strSelImagePath);
	//设置牌间隔
	void SetCardSpace(int iCardSpace);
	//设置牌位置
	void SetCardPosition(CPoint ptPosition, enGameCardXCollocateMode CardMode=enGCXCenter);
	//获得牌宽与高
	int	 GetCardControlWidth();
	int	 GetCardControlHeight();
	//获取牌区域
	void GetCardDrawRect(CRect& rect);
	//刷新牌区域
	void UpdateGameCard();
	void InvalidateCard();
	void UpdateCardPosition();
	//更新缓冲
	void UpdateCardMem();
	//绘制牌
	void DrawCardData();
	void DrawGameCard(CDC* pDC);

private:
	//牌数据
	std::vector<tagGameCardItem>	m_vCardList;

	//是否响应
	bool			m_bIsPositively;
	//是否显示
	bool			m_bIsShow;


private:
	//背景
	CPngImage*		m_pImageBack;
	//牌资源
	CPngImage		m_ImageCard;
	//牌选择
	CPngImage		m_ImageSelCard;

	//牌宽与高
	int				m_iCardWidth;
	int				m_iCardHeight;

	//牌选择宽与高
	int				m_iSelCardWidth;
	int				m_iSelCardHeight;

	//牌间隔
	int				m_iCardSpace;
	//牌弹起高度
	int				m_iCardShootHeight;

	//牌起点
	CPoint			m_ptOldPosition;
	CPoint			m_ptDrawStartXY;

	//牌区域
	CRect			m_rcGameCard;

	//对齐模式
	enGameCardXCollocateMode	m_CardXCollocateMode;
private:
	CWnd*			m_pViewWnd;

private:
	//牌缓冲
	CDC					m_memdc;
	CCriticalSpection	m_lockMemDC;	


private:
	//操作
	int				m_iMouseCurIndex;
	int				m_iMouseDownIndex;
	bool			m_bIsMouseDown;
};
