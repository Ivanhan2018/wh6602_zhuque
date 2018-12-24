#pragma once

#include "SonicPaint.h"

class CSonicAnimation : public ISonicAnimation, public ISonicBaseData
{
public:
	CSonicAnimation(void);
	~CSonicAnimation(void);
	virtual BOOL IsValid();
	virtual void Clear();
	virtual BOOL Create(HWND hWnd, int x, int y, int cx, int cy);
	virtual BOOL MoveGently(int x, int y, int nFrame, BOOL bEvenSpeed = FALSE);
	virtual BOOL ShowGently(BOOL bShow, int nFrame);
	virtual BOOL Transform(BOOL b, int nAngle = 0, RECT * pRect = NULL, int nFrame = 0);
	virtual const RECT * GetRect();
	virtual BOOL Redraw();
	virtual DWORD GetAnimationStatus();
	virtual void OnInternalTimer(DWORD dwTimerId);
	virtual BOOL AddObject(DWORD dwObjectId, int x = 0, int y = 0);
	virtual BOOL DelObject(DWORD dwObjectId, BOOL bByIndex = FALSE, BOOL bDestroyObject = FALSE);
	virtual BOOL SetObject(DWORD dwObjectId, int x = 0, int y = 0, BOOL bVisible = TRUE, BOOL bByIndex = FALSE);
	virtual BOOL DelAllObject(BOOL bDestroyObject = FALSE);
	virtual int GetCount();
	virtual PAINT_ITEM * GetPaintItem(DWORD dwObjectId, BOOL bByIndex = FALSE);
	virtual BOOL ShowObject(BOOL b, BOOL bRedraw, DWORD dwObjectId, BOOL bByIndex = FALSE);
	virtual BOOL OnWndMsg(UINT msg, WPARAM wParam, LPARAM lParam);
	virtual BOOL Render(HDC hdc);

protected:
	ISonicImage * SnapShot();
	void InitValue();
	void InternalMove();
	void InternalTransform();
	void OnTimerMove();
	void OnTimerShow();
	void OnTimerTransform();
	void OnDrawTransform(ISonicPaint * pPaint, LPVOID);
	void OnDrawShowGently(ISonicPaint * pPaint, LPVOID);
	void AdjustTransform();
	BOOL OnLButtonDown(WPARAM wParam, LPARAM lParam);
	BOOL OnRButtonDown(WPARAM wParam, LPARAM lParam);
	BOOL OnMouseMove(WPARAM wParam, LPARAM lParam);
	BOOL OnMouseLeave(WPARAM wParam, LPARAM lParam);

	BOOL PtInAnimation(POINT & pt);
	
protected:
	typedef struct tagMoveParam
	{
		int x;
		int y;
		BOOL bEven;
		int nFrame;
		int nCurrFrame;
		CRect rtOrigin;
	}MOVE_PARAM;
	typedef struct tagShowParam
	{
		int nAlphaStep;
		int nCurrAlpha;
		ISonicImage * pImgOrigin;
		void Clear();
	}SHOW_PARAM;
	typedef struct tagTransformParam
	{
		int nFrame;
		int nCurrFrame;
		int nAngle;
		BOOL bRestore;
		CRect rtOrigin;
		CRect rtTrans;
		ISonicImage * pImgOrigin;
		ISonicImage * pImg;
		void Clear();
        void InitValue();			
	}TRANSFORM_PARAM;
protected:
	ISonicPaint * m_pPaint;
	DWORD m_EffectMask;
	CRect m_rtAni;
	MOVE_PARAM m_Move;
	SHOW_PARAM	m_Show;
	TRANSFORM_PARAM	m_Transform;
	int m_nStatus;
};
