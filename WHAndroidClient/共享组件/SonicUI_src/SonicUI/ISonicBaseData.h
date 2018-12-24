#pragma once

typedef struct tagDelegateParam
{
	LPVOID pClass;
	LPVOID pFunc;
	LPVOID pParam;
}DELEGATE_PARAM;
typedef map<UINT, DELEGATE_PARAM> MSG_TO_DELEGATE_PARAM;

class ISonicBaseData;
typedef list<ISonicBaseData *> LIST_BASE_DATA;

#define TIMER_BASE_DATA		0x1213

class ISonicBaseData
{
public:
	ISonicBaseData(void);
	virtual ~ISonicBaseData(void);

	typedef map<int, DWORD> INTERVAL_TO_TIMER;
	void OnInternalTimer();
	virtual void SetInternalTimer(DWORD dwTimerId, int nInterval, BOOL bOnceTimer = FALSE);
	virtual void KillInternalTimer(DWORD dwTimerId);
	virtual BOOL QueryInternalTimer(DWORD dwTimerId);
	virtual void ClearInternalTimer();
	virtual void FireDelegate(UINT message, BOOL bImmediately = FALSE);
	virtual void OnDelegate(UINT message);
	virtual void OnInternalTimer(DWORD dwTimerId);
	virtual void ClearBaseData();
	virtual void ClearDelegate();
	DWORD m_dwTimer;
	DWORD m_dwTimerOnce;
	DWORD m_dwTimerId;
	INTERVAL_TO_TIMER m_mapIntervalToTimer;
	MSG_TO_DELEGATE_PARAM m_mapDelegate;
	HWND m_hWnd;	
	static LIST_BASE_DATA m_TimerList;
};
