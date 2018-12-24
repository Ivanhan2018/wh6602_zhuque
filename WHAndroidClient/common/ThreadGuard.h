#ifndef THREAD_GUARD_H
#define THREAD_GUARD_H


// 关键代码段
class __declspec(dllexport) CCriticalSpection
{
public:
	CCriticalSpection(DWORD dwSpinCount=4000);
	~CCriticalSpection();
	
	// 返回关键代码段
	CRITICAL_SECTION*	get_critical_section();

	// 有效的
	bool is_validate();

private:
	CRITICAL_SECTION	stCriticalSection;
	DWORD				m_dwSpinCount;		// 自旋的次数
	bool				m_bInitState;		// true 表示 m_pCriticalSection 处于 InitializeCriticalSectionAndSpinCount 状态，false 表示m_pCriticalSection 处于 DeleteCriticalSection 状态
};

// 线程级别的守卫
class __declspec(dllexport) CThreadGuard
{
public:
	CThreadGuard(CCriticalSpection* pCriticalSection);

	~CThreadGuard(void);

	// 进入关键代码段
	void enter();

	// 离开关键代码段
	void leave();

private:

	// 初始化与销毁都已在类中自动进行，优点：万一程序在解锁前return了或异常了，
	// g_SampleCS 会通过析构自我销毁掉关键代码段
	CCriticalSpection*	m_pCriticalSection;

	bool				m_bEnterState;		// true 表示 m_pCriticalSection 处于 EnterCriticalSection 状态，false 表示 m_pCriticalSection 处于 LeaveCriticalSection 状态

	
};

#endif
