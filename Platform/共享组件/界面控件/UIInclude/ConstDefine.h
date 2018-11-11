#pragma once


#define IMAGE_NAME		_T("IMAGE")
#define MY_WND			_T("SonicWnd")
#define CHILD_PROC		_T("ChildProc")
#define SKIN_CHILD_PROC	_T("SkinChildProc")
#define EFFECT_HELP_WND	_T("EffectHelpWnd")
#define EHT_PROP		_T("EHTProp")
#define WND_EFFECT_PROP_NAME	_T("SonicWndEffect")
#define DEFAULT_JPEG_QULITY		75

#define PAI		3.14159265
#define ROUND(x) (int)((x) > 0 ? (x) + 0.5 : (x) - 0.5)
#define Tsizeof(n) sizeof(n) / sizeof(TCHAR)
#define EQUAL_24COLOR(c1, c2) ((0xffffff & (c1)) == (0xffffff & (c2)))


// internal timer defined must be times of base interval
#define BASE_INTERVAL			10
#define ANIMATION_INTERVAL		20
#define GIF_INTERVAL			10
#define FADEOUT_INTERVAL		50
//////////////////////////////////////////////////////////////////////////
// error info
class CErrorContext
{
public:
	int nErrorId;
	CErrorContext(int _nErrorId) : nErrorId(_nErrorId){};	
};

enum enErrorId
{
	ERROR_ID_NONE = 0,
	ERROR_ID_UNKNOWN,
	ERROR_ID_STRING_FORMAT,
	ERROR_ID_INVALID_OBJECT_ID,
};

extern const TCHAR g_ErrorInfo[][256];

extern DWORD GetMainThreadId();
extern HWND GetTopParent(HWND hWnd);
extern DWORD GetColorDifference(DWORD dwColor1, DWORD dwColor2);

