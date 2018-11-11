#include "StdAfx.h"
#include ".\isonicbasedata.h"
#include "ConstDefine.h"
#include "SonicUI.h"

LIST_BASE_DATA ISonicBaseData::m_TimerList;

ISonicBaseData::ISonicBaseData(void)
{
	m_dwTimer = 0;
	m_dwTimerOnce = 0;
	m_hWnd = NULL;
	m_dwTimerId = 0;
}

ISonicBaseData::~ISonicBaseData(void)
{
}

void ISonicBaseData::ClearBaseData()
{
	ClearInternalTimer();
	ClearDelegate();
	m_hWnd = NULL;
}

void ISonicBaseData::ClearDelegate()
{
	m_mapDelegate.clear();
}

void ISonicBaseData::OnInternalTimer()
{
	m_dwTimer += BASE_INTERVAL;
	if(m_mapIntervalToTimer.size() == 0)
	{
		return;
	}
	for(INTERVAL_TO_TIMER::iterator it = m_mapIntervalToTimer.begin(); it != m_mapIntervalToTimer.end(); )
	{
		if(m_dwTimer && m_dwTimer % it->first == 0)
		{
			DWORD dwTimerId = it->second;
			DWORD dwOnce = m_dwTimerOnce;
			OnInternalTimer(dwTimerId);
			if(dwTimerId & dwOnce)
			{
				KillInternalTimer(dwOnce);
			}
		}
		if(it->second == 0)
		{
			it = m_mapIntervalToTimer.erase(it);
		}
		else
		{
			it++;
		}
	}
	if(m_dwTimer >= 10000000)
	{
		m_dwTimer = 0;
	}
}

void ISonicBaseData::OnInternalTimer(DWORD dwTimerId)
{
}

void ISonicBaseData::SetInternalTimer(DWORD dwTimerId, int nInterval, BOOL bOnceTimer /* = FALSE */)
{
	if(nInterval <= 0)
	{
		return;
	}
	KillInternalTimer(dwTimerId);
	if(m_dwTimerId == 0)
	{
		m_TimerList.push_back(this);
	}
	m_mapIntervalToTimer[nInterval] |= dwTimerId;
	if(bOnceTimer)
	{
		m_dwTimerOnce |= dwTimerId;
	}
	m_dwTimer = 0;
	m_dwTimerId |= dwTimerId;
}

void ISonicBaseData::KillInternalTimer(DWORD dwTimerId)
{
	if(m_mapIntervalToTimer.empty())
	{
		return;
	}
	for(INTERVAL_TO_TIMER::iterator it = m_mapIntervalToTimer.begin(); it != m_mapIntervalToTimer.end(); it++)
	{
		if(it->second & dwTimerId)
		{
			it->second &= ~dwTimerId;
			m_dwTimerOnce &= ~dwTimerId;
			break;
		}
	}
	m_dwTimerId &= ~dwTimerId;
	if(m_dwTimerId == 0)
	{
		for(LIST_BASE_DATA::iterator it = m_TimerList.begin(); it != m_TimerList.end(); it++)
		{
			if(*it == this)
			{
				m_TimerList.erase(it);
				break;
			}
		}
	}
}

BOOL ISonicBaseData::QueryInternalTimer(DWORD dwTimerId)
{
	if(m_mapIntervalToTimer.empty())
	{
		return FALSE;
	}
	return (m_dwTimerId & dwTimerId);
}

void ISonicBaseData::ClearInternalTimer()
{
	m_mapIntervalToTimer.clear();
	m_dwTimer = 0;
	m_dwTimerOnce = 0;
	m_dwTimerId = 0;
	for(LIST_BASE_DATA::iterator it = m_TimerList.begin(); it != m_TimerList.end(); it++)
	{
		if(*it == this)
		{
			m_TimerList.erase(it);
			break;
		}
	}
}

void ISonicBaseData::FireDelegate(UINT message, BOOL bImmediately /* = FALSE */)
{
	if(bImmediately == FALSE)
	{
		PostMessage(CSonicUI::m_hWnd, WM_SONIC_INTERNAL_EVENT, (WPARAM)dynamic_cast<ISonicBase *>(this), message);
	}
	else
	{
		SendMessage(CSonicUI::m_hWnd, WM_SONIC_INTERNAL_EVENT, (WPARAM)dynamic_cast<ISonicBase *>(this), message);
	}
}

void ISonicBaseData::OnDelegate(UINT message)
{
	MSG_TO_DELEGATE_PARAM::iterator it = m_mapDelegate.find(message);
	if(it == m_mapDelegate.end())
	{
		return;
	}
	DELEGATE_PARAM &pm = it->second;
	if(pm.pFunc == NULL || IsBadCodePtr((FARPROC)pm.pFunc))
	{
		return;
	}
	ISonicBase * pBase = dynamic_cast<ISonicBase *>(this);
	if(pBase == NULL)
	{
		return;
	}
	LPVOID pParam = pm.pParam;
	LPVOID pClass = pm.pClass;
	LPVOID pFunc = pm.pFunc;
	__asm
	{
		push ecx
		push [pParam]
		push [pBase]
		mov ecx, [pClass]
		call [pFunc]
		pop ecx
	}
}