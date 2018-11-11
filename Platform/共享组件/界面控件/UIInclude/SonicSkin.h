#pragma once

#include "ISonicUI.h"
using namespace sonic_ui;
#include "ISonicBaseData.h"

class CSonicSkin : public ISonicSkin, public ISonicBaseData
{
public:
	typedef void (CSonicSkin::* FUNC_SKIN_NAME_CTRL)(LPCTSTR);
	typedef void (CSonicSkin::* FUNC_SKIN_VALUE_CTRL)(LPCTSTR, LPCTSTR);

	typedef map<CString, FUNC_SKIN_NAME_CTRL> MAP_NAME_TO_CTRL;
	typedef map<CString, FUNC_SKIN_VALUE_CTRL> MAP_VALUE_TO_CTRL;

	typedef struct tagBgParam
	{
		ISonicImage * pImg;
		ISonicImage * pIcon;
		ISonicString * pClose;
		ISonicString * pMin;
		ISonicString * pMax;
		ISonicString * pRestore;
		
		HRGN hRgn[4];
		HRGN hMainRgn;
		HBRUSH hBrush;
		WORD wTitleHeight;
		WORD wTopHeight;
		BYTE bEnableDrag;
		int nColorKey;
		WORD wBtnSpace;
		WORD wBtnTopMargin;
		WORD wBtnRightMargin;
		ISonicString * pText;

		void InitValue()
		{
			memset(this, 0, (char *)&(tagBgParam::pText) - (char *)this);
			nColorKey = -1;
			wBtnSpace = 2;
			wBtnTopMargin = 2;
			wBtnRightMargin = 5;
			bEnableDrag = TRUE;
		}
		tagBgParam()
		{
			InitValue();
			pText = GetSonicUI()->CreateString();

		}
		void Clear()
		{
			if(hBrush)
			{
				DeleteObject(hBrush);
				hBrush = NULL;
			}
			SONICUI_DESTROY(pText);
			SONICUI_DESTROY(pMin);
			SONICUI_DESTROY(pMax);
			SONICUI_DESTROY(pRestore);
			SONICUI_DESTROY(pClose);
			for(int i = 0; i < 4; i++)
			{
				if(hRgn[i])
				{
					DeleteObject(hRgn[i]);
					hRgn[i] = NULL;
				}
			}
			if(hMainRgn)
			{
				DeleteObject(hMainRgn);
				hMainRgn = NULL;
			}
			InitValue();
			
		}
	}BG_PARAM;

	typedef struct tagStytleParam
	{
		BYTE bReserve;
		tagStytleParam()
		{
		}
		void Clear()
		{
			memset(this, 0, sizeof(tagStytleParam));
		}
	}STYLE_PARAM;

	typedef struct tagButtonParam
	{
		ISonicImage * pImg;
		typedef map<HWND, ISonicString *> MAP_HWND_TO_STRING;
		MAP_HWND_TO_STRING mapString;
		CString strTextCtrl;
		CString strBtnCtrl;


		tagButtonParam()
		{
			Clear();
		}
		void Clear()
		{
			strTextCtrl.Empty();
			strBtnCtrl.Empty();
			pImg = NULL;
			if(mapString.size())
			{
				for (MAP_HWND_TO_STRING::iterator it=  mapString.begin(); it != mapString.end(); it++)
				{
					GetSonicUI()->DestroyObject(it->second);
				}
				mapString.clear();
			}
		}
	}BUTTON_PARAM;
public:
	CSonicSkin(void);
	~CSonicSkin(void);

	void SetRgn(RECT * pRect = NULL, BOOL bDraw = TRUE);
	virtual BOOL IsValid();
	virtual void Clear();

	virtual BOOL SetSkin(LPCTSTR lpszSkinName, LPCTSTR lpszValue, ...);
	virtual BOOL Attach(HWND hWnd);
	virtual ISonicPaint * GetPaint();

	virtual BOOL OnWndMsg(UINT msg, WPARAM wParam, LPARAM lParam);

public:
	static BOOL Init();
	void OnBtnClose(ISonicSkin *, LPVOID);
	void OnBtnMinimize(ISonicSkin *, LPVOID);
	void OnBtnMaximize(ISonicSkin *, LPVOID);
	void OnBtnRestore(ISonicSkin *, LPVOID);

protected:
	void InitValue();
	int AutoAnylizeTitleHeight();
	BOOL OnGetMinMaxInfo(MINMAXINFO * pMMI);
	BOOL OnEraseBackground(HDC hdc);
	BOOL OnNCHittest(int x, int y);
	BOOL OnPosChanged(LPWINDOWPOS pPos);
	BOOL OnPosChanging(LPWINDOWPOS pPos);
	BOOL OnDrawItem(LPDRAWITEMSTRUCT pDI);
	BOOL OnCtlColorBtn(HDC hdc, HWND hCtrl);
	BOOL OnCtlColorStatic(HDC hdc, HWND hCtrl);

private:
	static MAP_NAME_TO_CTRL m_mapName;
	static MAP_VALUE_TO_CTRL m_mapValue;

	void Name_button(LPCTSTR lpszSkinName);
	void Name_bg(LPCTSTR lpszSkinName);
	void Name_style(LPCTSTR lpszSkinName);
	
	void Value_button_image(LPCTSTR lpszValueName, LPCTSTR lpszValue);
	void Value_button_text_ctrl(LPCTSTR lpszValueName, LPCTSTR lpszValue);
	void Value_button_btn_ctrl(LPCTSTR lpszValueName, LPCTSTR lpszValue);
	void Value_bg_image(LPCTSTR lpszValueName, LPCTSTR lpszValue);
	void Value_bg_close(LPCTSTR lpszValueName, LPCTSTR lpszValue);
	void Value_bg_min(LPCTSTR lpszValueName, LPCTSTR lpszValue);
	void Value_bg_max(LPCTSTR lpszValueName, LPCTSTR lpszValue);
	void Value_bg_restore(LPCTSTR lpszValueName, LPCTSTR lpszValue);
	void Value_bg_icon(LPCTSTR lpszValueName, LPCTSTR lpszValue);
	void Value_bg_title_height(LPCTSTR lpszValueName, LPCTSTR lpszValue);
	void Value_bg_top_height(LPCTSTR lpszValueName, LPCTSTR lpszValue);
	void Value_bg_title_text(LPCTSTR lpszValueName, LPCTSTR lpszValue);
	void Value_bg_color_key(LPCTSTR lpszValueName, LPCTSTR lpszValue);
	void Value_bg_btn_space(LPCTSTR lpszValueName, LPCTSTR lpszValue);
	void Value_bg_btn_top_margin(LPCTSTR lpszValueName, LPCTSTR lpszValue);
	void Value_bg_btn_right_margin(LPCTSTR lpszValueName, LPCTSTR lpszValue);
	void Value_bg_enable_drag(LPCTSTR lpszValueName, LPCTSTR lpszValue);

	static LRESULT CALLBACK ChildProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	void InstallChildProc(HWND hChild);

private:
	BG_PARAM m_bg;
	STYLE_PARAM m_style;
	BUTTON_PARAM m_button;
	ISonicPaint * m_pPaint;
};
