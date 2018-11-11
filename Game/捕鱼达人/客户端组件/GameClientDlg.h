#ifndef GAME_CLIENT_ENGINE_HEAD_FILE
#define GAME_CLIENT_ENGINE_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "GameClientView.h"

//////////////////////////////////////////////////////////////////////////

//游戏引擎
class CGameClientDlg : public CGameFrameDlg
{
	//游戏变量
protected:
	LONG							m_lBulletCharge[enBulletCountKind_End];	//子弹费用
	enBulletCountKind				m_BulletCountKindSelected;				//选择类型
	LONG							m_lCellScore;							//单元积分

	//控件变量
public:
	CGameClientView					m_GameClientView;					//游戏视图

	//函数定义
public:
	//构造函数
	CGameClientDlg();
	//析构函数
	virtual ~CGameClientDlg();

	//常规继承
private:
	//初始函数
	virtual bool InitGameFrame();
	//重置框架
	virtual void ResetGameFrame();
	//游戏设置
	virtual void OnGameOptionSet();
	//时间消息
	virtual bool OnTimerMessage(WORD wChairID, UINT nElapse, UINT nTimerID);
	//旁观状态
	virtual void OnLookonChanged(bool bLookonUser, const void * pBuffer, WORD wDataSize);
	//网络消息
	virtual bool OnGameMessage(WORD wSubCmdID, const void * pBuffer, WORD wDataSize);
	//游戏场景
	virtual bool OnGameSceneMessage(BYTE cbGameStatus, bool bLookonOther, const void * pBuffer, WORD wDataSize);

	//事件处理
private:
	//鱼群轨迹
	bool OnSubFishTrace(const void * pData, WORD wDataSize);
	//发射炮弹
	bool OnSubUserShoot(const void * pData, WORD wDataSize);
	//捕获鱼群
	bool OnSubCaptureFish(const void * pData, WORD wDataSize);
	//子弹数目
	bool OnSubBulletCount(const void * pData, WORD wDataSize);
	//金币数目
	bool OnSubCoinCount(const void * pData, WORD wDataSize);
	//鱼群数目
	bool OnSubCptrFishCount(const void * pData, WORD wDataSize);
	//切换场景
	bool OnSubChangeScene(const void * pData, WORD wDataSize);
	//分数信息
	bool OnSubScoreInfo(const void * pData, WORD wDataSize);
	//单元积分
	bool OnSubSetCellScore(const void * pData, WORD wDataSize);
	//彩金信息
	bool OnSubBonusInfo(const void * pData, WORD wDataSize);
	//游戏消息
	bool OnSubGameMessage(const void * pData, WORD wDataSize);
		
	//消息映射
protected:
	//测试消息
	afx_msg UINT OnNcHitTest(CPoint point);
	//发炮消息
	LRESULT OnShoot(WPARAM wParam, LPARAM lParam);
	////捕获消息
	//LRESULT OnCaptureFish(WPARAM wParam, LPARAM lParam);
	//击中消息
	LRESULT OnHitFish(WPARAM wParam, LPARAM lParam);
	////购买子弹
	//LRESULT OnShowBulletShop(WPARAM wParam, LPARAM lParam);
	//退出游戏
	LRESULT OnQuitGame(WPARAM wParam, LPARAM lParam);
	//设置语音
	LRESULT OnEnableSound(WPARAM wParam, LPARAM lParam);
	
	//LRESULT OnCapture(WPARAM wParam, LPARAM lParam);
	
	//关闭函数
	virtual void OnCancel();
	
	//用户事件
private:
	//用户进入
	virtual void __cdecl OnEventUserEnter(tagUserData * pUserData, WORD wChairID, bool bLookonUser);
	//用户离开
	virtual void __cdecl OnEventUserLeave(tagUserData * pUserData, WORD wChairID, bool bLookonUser);

public:
	//点击继续游戏
	virtual void OnEventContinueGame();//cxf

	////add by Ivan_han
	//BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);
;
	//辅助函数
private:
	//购买子弹
	void ShowBulletShop();

		
	DECLARE_MESSAGE_MAP()	
};

//////////////////////////////////////////////////////////////////////////

#endif