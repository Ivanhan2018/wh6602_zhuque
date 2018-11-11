#pragma once

#include "ISonicUI.h"
using namespace sonic_ui;

#include "DibMgr.h"
#include "ISonicBaseData.h"

#define DEFAULT_ALPHA_STEP		35
#define DEFAULT_MOVE_FRAME		28

#define TIMER_SHOWING_GENTLY	0x1
#define TIMER_MOVE_GENTLY		0x2
#define TIMER_FRAME				0x8	
#define TIMER_TRANSFORM			0x10
#define TIMER_SLIDE				0x20
#define TIMER_TRACK_MOUSE		0x40
#define TIMER_SHUTTER			0x80

enum enEffectHelpType
{
	EHT_NONE = 0,
	EHT_FRAME,
	EHT_TRANSFORM,
	EHT_PREDRAG,
	EHT_SLIDE,
	EHT_SHUTTER,
};

#define DRAW_LAYER_ERASE		2
#define DRAW_LAYER_NOERASE		1

typedef list<HWND> WND_LIST;

class CSonicWndEffect : public ISonicWndEffect, public ISonicBaseData
{
public:

	typedef struct tagFrameParam
	{
		BYTE bSideMask;
		BYTE bType;
		int nAlphaSpeed;
		int nCurrAlpha;
		int nSpace;
		ISonicImage * pImg[8];
		int nSideWidth;
		int nCornerWidth;
		int nCornerHeight;
		void Clear();
	}FRAME_PARAM;

	typedef struct tagTransformParam
	{
		int nFrame;
		int nMaxFrame;
		int nAngle;
		CRect rtDest;
		CRect rtOrigin;
		HWND hHelp;
		ISonicImage * pImgOrigin;
		ISonicImage * pImgTransform;
		BOOL bRestore;
		BOOL bDirect;
		void Transform(int &x, int &y);
		void Clear();
	}TRANSFORM_PARAM;

	typedef struct tagSlideParam
	{
		int nFrame;
		int nMaxFrame;
		POINT ptFrom;
		POINT ptTo;
		int nShow;
		HWND hHelp;
		void Clear();
	}SLIDE_PARAM;

	typedef struct tagPreDragParam
	{
		HWND hHelp;
		ISonicImage * pImg[4];
		void Clear();
	}PRE_DRAG_PARAM;

	typedef struct tagMoveParam
	{
		CRect rtMove[DEFAULT_MOVE_FRAME + 1];
		int nFrame;
		int nMoveTimerId;
	}MOVE_PARAM;

	typedef struct tagFadeoutParam
	{
		int nAlphaStep;
		int nCurrAlpha;
		int nShowType;
	}FADEOUT_PARAM;

	typedef struct tagShutterParam
	{
		int nFrame;
		BOOL bCross;
		HWND hHelp;
		ISonicImage * pMask;
		ISonicImage * pOrigin;
		CRect rtOld;
		void Clear();
	}SHUTTER_PARAM;

	typedef struct tagWndEffect
	{
		DWORD	dwEffectMask;
		ISonicImage * pImg;	// »­²¼
		int h;
		int s;
		int l;
		BOOL bHover;
		FRAME_PARAM frame;
		MOVE_PARAM move;
		FADEOUT_PARAM fadeout;
		TRANSFORM_PARAM transform;
		PRE_DRAG_PARAM predrag;
		SLIDE_PARAM slide;
		SHUTTER_PARAM shutter;
	}WND_EFFECT;
public:
	CSonicWndEffect(void);
	~CSonicWndEffect(void);
	virtual BOOL Attach(HWND hWnd, BOOL bAlphaPerPixel = FALSE);
	virtual void Clear();
	virtual BOOL ShowGently(BOOL bShow, BOOL bActivted = FALSE, int nStep = 0);
	virtual BOOL SetShapeByImage(ISonicImage * pImg, DWORD dwColorKey = -1);
	virtual BOOL MoveGently(int x, int y, BOOL bEvenSpeed = FALSE);
	virtual BOOL SetLayeredAttributes(COLORREF crKey, BYTE bAlpha, DWORD dwFlag);
	virtual BOOL OnWndMsg(UINT msg, WPARAM wParam, LPARAM lParam);
	virtual BOOL IsValid();
	virtual BOOL EnableWholeDrag(BOOL b);
	virtual BOOL EnableGray(BOOL b);
	virtual BOOL EnableHSLAdjust(BOOL b, int h = -1, int s = -1, int l = -1);
	virtual HWND CreateChildCtrl(LPCTSTR lpszClassName, LPCTSTR lpszText, DWORD dwStyle, int x, int y, int cx, int cy, int nAttach = -1);
	virtual BOOL EnableFrame(BOOL b, BYTE bSideMask = 0xf, BYTE bType = 0, int nSpace = 0, int nAlphaSpeed = 10, 
		ISonicImage * pSide = NULL, ISonicImage * pCorner = NULL);
	virtual BOOL SetBkColor(DWORD dwColor);
	virtual BOOL EnableTransform(BOOL b, int nAngle = 0, RECT * pDest = NULL, int nFrame = 0);
	virtual BOOL EnablePreDrag(BOOL b);
	virtual BOOL SlideShow(BOOL b, POINT ptFrom, POINT ptTo, int nFrame = 15, BOOL bActivted = FALSE);
	virtual ISonicImage * Snapshot();
	virtual BOOL EnableTrackMouse(BOOL b);
	virtual BOOL ConvertChildCtrl(HWND hChild, int nAttach = -1);
	virtual DWORD GetWndEffectType();
	virtual BOOL DirectTransform(int nAngle, RECT * pDest, int nFrame);
	virtual BOOL Shutter(BOOL bCross = TRUE);
	virtual BOOL IsAnimating();
	
	friend struct tagTransformParam;
	static BOOL Init();
public:
	void FinalEffect(HDC hdc);
	void Redraw();
	void UpdateLayer();
	void UpdateDibLayer(int x, int y, int cx, int cy, int xSrc = 0, int ySrc = 0, BOOL bRedraw = DRAW_LAYER_ERASE);
	void UpdateDibLayer(BOOL bRedraw = DRAW_LAYER_ERASE);
	BOOL IsAlphaPerPixel() {return m_Dib.IsValid();}

protected:
	WND_EFFECT m_Effect;
	DWORD m_dwBkColor;

	DWORD m_dwOldExStyle;	
	int m_nKeyColor;
	BYTE m_bAlpha;
	BYTE m_bDelayShowSwitch;
	ISonicImage * m_pBg;
	CDibMgr m_Dib;	
	SIZE m_szSrc;	
	WND_LIST m_ChildList;
	
	BOOL OnEraseBackground(HDC hdc);
	void InitValue();
	void OnTimerShowingGently();
	void OnTimerMoveGently();
	void OnTimerSlide();
	void OnTimerFrame();
	void OnTimerTransform();
	void OnTimerTrackMouse();
	void OnTimerShutter();
	void OnMouseEnter();
	void OnMouseLeave();
	void EndShowTimer();
	void DrawLayer(BOOL bErase = TRUE);
	void PrepareLayer(int cx, int cy);
	BOOL OnPosChanging(LPWINDOWPOS pPos);
	BOOL OnPosChanged(LPWINDOWPOS pPos);
	void OnInternalTimer(DWORD dwTimerId);
	void ModifyChild(HWND hWnd);
	void RestoreChild();
	void AnimateFrame(BOOL b);
	BOOL OnFrameMsg(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	BOOL OnTransformMsg(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	BOOL OnPreDragMsg(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	BOOL OnShutterMsg(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	HWND CreateEffectHelpWnd(enEffectHelpType eht, DWORD dwStyle);
	void RestoreTransform();	
	void Transform();
protected:
	static void CALLBACK TimerProc(UINT uTimerID, UINT uMsg, DWORD_PTR dwUser, DWORD_PTR dw1, DWORD_PTR dw2);
	static LRESULT CALLBACK ChildProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK EffectHelpProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
};
