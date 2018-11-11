#pragma once

#include "Stdafx.h"
#include "ControlWnd.h"
#include "CardControl.h"
#include "GameScoreWnd.h"
#include "DlgAIPrompt.h"
#include "..\..\开发库\Include\GameFlash.h"

//////////////////////////////////////////////////////////////////////////
//消息定义

#define IDM_START					(WM_USER+100)						//开始消息
#define IDM_OUT_CARD				(WM_USER+101)						//出牌消息
#define IDM_OPERATE_HAI_DI			(WM_USER+102)						//出牌消息
#define IDM_TRUSTEE_CONTROL			(WM_USER+103)						//托管控制
#define IDM_SHOW_SZ                 (WM_USER+104)                       //骰子消息
#define IDM_TING_CARD               (WM_USER+105)                       //听牌消息
#define IDM_MOPAI                   (WM_USER+106)                       //摸牌消息
#define IDM_SZSTART                 (WM_USER+107)                       //启动筛子按钮消息


#define IDM_SZ_BUTTON				(WM_USER+110)						//色子按钮消息





//方位动画总帧数
#define I_DIRECT_FLASH_TOTOL_NUMS		16
#define I_DIRECT_FLASH_WIDTH			630
#define I_DIRECT_FLASH_HEIGHT			390

//色子动画总帧数
#define I_SZ_FLASH_TOTOL_NUMS		4
#define I_SZ_FLASH_WIDTH			197
#define I_SZ_FLASH_HEIGHT			264




 class CGameClientDlg;

//////////////////////////////////////////////////////////////////////////

//游戏视图
class CGameClientView : public CGameFrameView
{
	//标志变量
public:
	bool							m_bTrustee[GAME_PLAYER];			//是否托管
protected:
	//bool							m_bOutCard;							//出牌标志
	//bool							m_bWaitOther;						//等待标志
	BYTE							m_cbHaiDiCard;						//海底扑克	
	bool                            m_bIsInitBT;                        //按钮初始化
	
	//游戏属性
protected:
	LONGLONG						m_lCellScore;						//基础积分
	WORD							m_wBankerUser;						//庄家用户

	//用户状态
protected:
	BYTE							m_cbCardData;						//出牌扑克
	WORD							m_wOutCardUser;						//出牌用户
	BYTE							m_cbUserAction[4];					//用户动作

public:
	WORD                            m_wChiHuKind[GAME_PLAYER];          //吃胡类型
	WORD                            m_wChiHuRight[GAME_PLAYER];         //胡牌权位
	WORD                            m_wChiHuType;                       //游戏结束类型
	int                             m_iChiHuNums;                       //胡牌玩家个数

	//位置变量
protected:
	CPoint							m_UserBankerFlagPos[4];				//标志位置 庄
	CPoint							m_PointTrustee[GAME_PLAYER];		//托管位置
	CPoint							m_ptLastMahjong[GAME_PLAYER];		//打出麻将位置
	CPoint                          m_UserDuanXian[GAME_PLAYER];        //玩家断线
	CPoint                          m_Info[3];                          //信息框位置(0:对家; 1:右家; 2: 左家)

	POINT								m_ptActionDice;					//骰子动画起始位
	POINT								m_ptActionDice2;				//骰子动画
	//位置变量
	CSize								m_csActionDice1;				//骰子动画大小
	CSize								m_csActionDice2;				//骰子大小


public:
	//数据变量
	//int                                 m_nKingCardIndex;               //王牌索引
	int									m_nDiceCount1;					//骰子数1
	int									m_nDiceCount2;					//骰子数2

	//位图变量
protected:
	CSkinImage						m_ImageBack;						//背景图案
	CPngImage                       m_ImageDirectionBack;                //方位标志背景DIRECTIONBACK.png
	CPngImage						m_ImageUserFlag;					//方位标志
	CPngImage						m_ImageUserBankerFlag;				//庄家标志
	//CPngImage						m_ImageUserAction;					//用户动作
	//CPngImage						m_ImageActionBack;					//动作背景
	CPngImage						m_ImageOutCardBack;					//出牌背景

	CPngImage						m_ImageHuangZhuang;					//荒庄标志
	CPngImage						m_ImageTrustee;						//托管标志	
	CPngImage						m_ImageOutCard;						//出牌提示
	//CPngImage						m_ImageWait;						//等待提示
	
	//CPngImage                       m_ImageMPEffect;                    //筛子动画
	//CPngImage                       m_ImageMPBACK;                      //摸牌动画
	//CPngImage                       m_ImageMPCARD;                      //摸牌数据

	CPngImage                       m_ImageUserInfo[GAME_PLAYER];       //其余玩家信息框
	CPngImage                       m_ImageUserGradeNum;                //等级数字图片 

	CPngImage						m_ActionDice1;					    //骰子动画
	CPngImage						m_ActionDice2;					    //骰子动画定点
	

	
	//骰子和摸牌动画
protected:
	bool							m_bDiceEffect;						//骰子效果
	bool							m_bStartEffect;						//开始效果
	BYTE							m_cbFrameIndex;					    //帧数索引
	bool							m_bMPEffect;						//摸牌效果
   

	//扑克控件
public:
	CHeapCard						m_HeapCard[4];						//堆立扑克
	CUserCard						m_UserCard[3];						//用户扑克
	CTableCard						m_TableCard[4];						//桌面扑克
	CWeaveCard						m_WeaveCard[4][4];					//组合扑克
	CDiscardCard					m_DiscardCard[4];					//丢弃扑克
	CCardControl					m_HandCardControl;					//手上扑克

	int								m_tDice;						    //骰子动画时间
	int                             m_tStart;                           //开始动画时间
	bool							m_bTingFlag[GAME_PLAYER];			//听牌标志
	bool                            m_bIsHuangZhuang;                   //是否荒庄


	//控件变量
public:
	CSkinButton					    m_btStart;							//开始按钮
	CSkinButton						m_btStusteeControl;					//拖管控制
	CSkinButton						m_btAutoPlayOn;
	CSkinButton						m_btAutoPlayOff;                    //取消托管
	CSkinButton                     m_btSortCard;                       //排序按钮
	//CSkinButton						m_ControlWnd;						//控制窗口
	//CSkinButton						m_btStusteeControl;					//拖管控制
	//CSkinButton					    m_btTingCard;					    //听牌按钮
	//CSkinButton                     m_btBuTing;                         //不听按钮
	//CSkinButton						m_btHaiDi;							//海底按钮
	//CSkinButton						m_btNoHaiDi;						//放弃按钮
	CControlWnd						m_ControlWnd;						//控制窗口


private:
	CGameClientDlg*					m_pGameClientDlg;
    CDlgAIPrompt                    m_CardHuPaiList;                    //智能提示框
	CCardControl                    m_CardControl;                      //牌控件


private:
	CGameFlash						m_GameOperationFlash;				//操作动画
	CGameFlash                      m_OutCardFlash;                     //出牌消失动画
	//CGameFlash      				m_TingCardFlash;                    //听牌动画

	//CGameFlash                      m_ChiHuKindFlashZiMo;                //自摸动画
	//CGameFlash                      m_ChiHuKindFlashDianPao;             //点炮动画
	//CGameFlash                      m_ChiHuKindFlashFangPao;             //放炮动画
	//CGameFlash                      m_ChiHuKindFlashTongPao;             //通炮动画
	//CGameFlash                      m_ChiHuKindFlashHaidi;               //海底
	//CGameFlash                      m_ChiHuKindFlashGang;                //杠上开花

	// 筋动画
	CThreadAnimation				* m_pTAFlashMuscle;								///< 筋
	CPngImage						m_ImageGoldChar;								///< 金币字符
	LONGLONG						m_llGoldMuscle[GAME_PLAYER];				///< 筋金币
	BYTE									m_byShowMuscle;										///< 筋显示
	CImageNums						m_INMuscleNumGray;							///< 筋灰色数字控件
	CImageNums						m_INMuscleNumGreen;							///< 筋灰色数字控件
	CRect								m_rcMuscleNum[GAME_PLAYER];				///< 筋动画位置

	//CGameFlash                      m_ChiHuKindFlashPengPeng;            //碰碰胡
	//CGameFlash                      m_ChiHuKindFlashQingYiSe;            //清一色
	//CGameFlash                      m_ChiHuKindFlashQiXiaoDui;           //七小对
	//CGameFlash                      m_ChiHuKindFlashJiangJiang;          //将将胡
	//CGameFlash                      m_HuangZhuangFlash;                  //荒庄

	int								m_iSaveCheckPaiIndex;

	CImageNums						m_TestNums;
	CFont							m_fontDrawGameInfo;						//窗口字体
private:
	//桌面动画控制
	bool							m_bIsShowOutCardFlash;					//是否显示牌有背景
	CRect							m_rcShowDrawArea;



public:
	bool							m_bIsOpenSZFlash;						//是否打开色子动画
	int								m_iCountSZFCurIndex;					//计算当前色子序号
	CRect							m_rcSZFlashDraw;						//色子动画区域
	CRect							m_rcSZBTRect;							//色子按钮有效区域
	CPngImage                       m_ActionSZFlash;       

	bool							m_bIsPlayDirect;						//是否打开定庄动画
	int								m_iCountDirectIndex;					//计算当前定庄序号
	CRect							m_rcDirectFlashDraw;					//定庄动画区域
	CPngImage                       m_ActionDirectFlash;  



private:
	CPoint							m_ptOperationFlash[GAME_PLAYER];     //操作动画
	//CPoint                          m_ptChiHuFlash[GAME_PLAYER];       //胡牌动画位置
	//函数定义
public:
	void TestClient(int iType);
	//构造函数
	CGameClientView();
	//析构函数
	virtual ~CGameClientView();

	//继承函数
private:
	//重置界面
	virtual void ResetGameView();
	//调整控件
	virtual void RectifyGameView(int nWidth, int nHeight);
	//绘画界面
	virtual void DrawGameView(CDC * pDC, int nWidth, int nHeight);
	// 安全显示或隐藏窗口
	void SafeShowOrHideWnd(CWnd* pWnd, int nCmdShow);
public:
	void SetGameClientDlgPtr(CGameClientDlg* pGameClient);

	//功能函数
public:
	//获取麻将路径
	void GetMahjonPath(CString& strPath);
	//获取图片路径
	void GetPicPath(CString& strPath);
	//基础积分
	void SetCellScore(LONGLONG lCellScore);
	// 获取基础积分
	LONGLONG GetCellScore();
	//海底扑克
	void SetHaiDiCard(BYTE cbHaiDiCard);
	//庄家用户
	void SetBankerUser(WORD wBankerUser);
	////听牌状态
	//void SetStatusTingFlag(WORD wViewChairID, bool cbHearStatus);
	//状态标志
	//void SetStatusFlag(bool bOutCard, bool bWaitOther);
	//出牌信息
	void SetOutCardInfo(WORD wViewChairID, BYTE cbCardData);
	//动作信息
	void SetUserAction(WORD wViewChairID, BYTE bUserAction);
	//设置托管
	void SetTrustee(WORD wTrusteeUser,bool bTrustee);

	//
	void CreateGameBT();
	//按钮加载资源
	void  LoadRes(CGdipButton* pBT, CString strValue);
	
	//绘图函数
	//特效动画
	void ActionEffect(CDC *pDC,int nWidth, int nHeight,int nFrame);
	////开始动画
	//void ActionStart( CDC *pDC,int nFrame);
	//骰子动画
	void ActionDice1( CDC *pDC,int nFrame);
	//骰子动画2
	void ActionDice2( CDC *pDC,int pos1, int pos2 );
	//摸牌动画
	void ActionMP( CDC *pDC,int nFrame);
	//骰子效果
	bool SetDiceEffect(bool bDiceEffect);
	//摸牌效果
	bool SetMPEffect(bool bDiceEffect);
    //动作动画
	void PlayOperationFlash(BYTE byViewID,BYTE byUserAction);
	/// 筋动画
	/// [@param byTargetViewID in] 目的 视图ID，（注：一般是赚钱玩家）
	/// [@param bySourceViewID in] 源 视图ID, 255代表不包括目的玩家内的所有玩家，（注：一般是陪钱玩家）
	/// [@param byUserAction in] 筋类别 0 无效值, 1 暗筋，2 明筋，3 放筋
	/// [@param llGold in] 变动的金币值
	void PlayMuscleFlash(BYTE byTargetViewID, BYTE bySourceViewID, BYTE byUserAction, const LONGLONG llGold[]);
	//胡牌动画
	void PlayChiHuFlash(BYTE byViewID);
	//胡牌动画
	void PlayFangPaoFlash(BYTE byViewID);
	//方位动画
	void PlayDirectFlash(bool bIsPlay=true);
	//启动色子动画
	void PlaySZFlashBT(bool bIsPlay=true);
	//启动听牌动画
	//void PlayTingCardFlash(BYTE byViewID);
	void InitViewFrame();
	/// 绘制麻将
	void OnDrawMahjong(CDC *pDC);
	/// 绘制刚打出的那张麻将
	void OnDrawLastMahjong(CDC *pDC);
	/// 绘制筋动画
	void OnDrawMuscle(CDC *pDC);
public:
	void SetCanHuPaiData(const BYTE* pCardData, int iCardNums, int iX);
	//辅助函数
protected:
	//艺术字体
	void DrawTextString(CDC * pDC, LPCTSTR pszString, COLORREF crText, COLORREF crFrame, int nXPos, int nYPos);	
	//消息映射
protected:
	//时间消息
	VOID OnTimer(UINT nIDEvent);
	//开始按钮
	afx_msg void OnStart();
	////海底按钮
	//afx_msg void OnHaiDi();
	////放弃按钮
	//afx_msg void OnNoHaiDi();
	//拖管控制
	afx_msg void OnStusteeControl();
	////听牌按钮
	//afx_msg void OnTingCard();
	////不听按钮
	//afx_msg void OnNoTingCard();
	// 自动托管
	afx_msg void OnAutoPlayerOn();
	// 取消托管
	afx_msg void OnAutoPlayerOff();
	//建立函数
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//鼠标消息
	afx_msg void OnLButtonDown(UINT nFlags, CPoint Point);
	//光标消息
	afx_msg BOOL OnSetCursor(CWnd * pWnd, UINT nHitTest, UINT uMessage);

	DECLARE_MESSAGE_MAP()



public:
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
};

//////////////////////////////////////////////////////////////////////////
