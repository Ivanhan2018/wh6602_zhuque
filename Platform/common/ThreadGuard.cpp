#include "StdAfx.h"
#include ".\ThreadGuard.h"

// 关键代码段
CCriticalSpection::CCriticalSpection(DWORD dwSpinCount)
{
	m_bInitState = false;
	m_dwSpinCount = dwSpinCount;

	for(int i=0; i<10; i++)
	{
		if(!m_bInitState)	// 处于 DeleteCriticalSection 状态
		{
			#if 1
				// 在多线程、release 状态下，出池 1200次/ms，速度快多了
				if (InitializeCriticalSectionAndSpinCount(&stCriticalSection, m_dwSpinCount))
				{
					m_bInitState = true;
					return;	// 成功就可以返回了
				}
			#else
				// 在多线程、release 状态下，出池 650次/ms ，速度慢
				InitializeCriticalSection(&stCriticalSection);
				m_bInitState = true;
				return;
			#endif
		}

		// 初始化关键代码段_循环锁 失败! 在内存不足的情况下会发生这种错误 
		Sleep(200);
	}
};

CCriticalSpection::~CCriticalSpection()
{
	if(m_bInitState)	// 处于 InitializeCriticalSectionAndSpinCount 状态
	{
		DeleteCriticalSection(&stCriticalSection);	// 清除关键代码段

		m_bInitState = false;
	}
};

// 返回关键代码段
CRITICAL_SECTION* CCriticalSpection::get_critical_section()
{
	return &stCriticalSection;
}

// 有效的
bool CCriticalSpection::is_validate()
{
	return m_bInitState;
}

///////////////////////////////////////////////////////////////////////////////////

// 线程级别的守卫
CThreadGuard::CThreadGuard(CCriticalSpection* pCriticalSection)
{
	m_bEnterState = false;

	m_pCriticalSection = pCriticalSection;

	if(m_pCriticalSection->is_validate())
	{
		enter();
	}
}

CThreadGuard::~CThreadGuard(void)
{
	leave();
}

// 进入关键代码段
void CThreadGuard::enter()
{
	if(m_pCriticalSection)
	{
		::EnterCriticalSection(m_pCriticalSection->get_critical_section());

		m_bEnterState = true;
	}
}

// 离开关键代码段
void CThreadGuard::leave()
{
	if(m_bEnterState && m_pCriticalSection)
	{
		::LeaveCriticalSection(m_pCriticalSection->get_critical_section());

		m_bEnterState = false;
	}
}