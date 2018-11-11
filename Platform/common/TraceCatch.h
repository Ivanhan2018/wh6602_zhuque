/********************************************************************
	created:	2011/07/21
	created:	21:7:2011   18:56
	filename: 	e:\Net\Leak\Leak\TraceCatch.h
	file path:	e:\Net\Leak\Leak
	file base:	TraceCatch
	file ext:	h
	author:		侯国江
	
	purpose:	追踪代码中的异常
*********************************************************************/

#ifndef _TRACE_CATCH_H
#define _TRACE_CATCH_H

/*
编译器里常用的宏
__FILE__
__LINE__
__DATE__
__TIME__
__FUNCTION__
*/

#define __ENTER_FUNCTION {try{
#define __LEAVE_FUNCTION }catch(...){__assertspecial__(__FILE__, __LINE__, __FUNCTION__, "我的信息");}}

#define __LEAVE_FUNCTION_EX(MyPromptMsg) }catch(...){__assertspecial__(__FILE__, __LINE__, __FUNCTION__, ##MyPromptMsg);}}

int __assertspecial__ (const TCHAR* file, int line, const TCHAR* func, const TCHAR* msg) ;

class MyLock
{
	CRITICAL_SECTION m_Lock ;
public :
	MyLock( ){ InitializeCriticalSection(&m_Lock); } ;
	~MyLock( ){ DeleteCriticalSection(&m_Lock); } ;
	VOID	Lock( ){ EnterCriticalSection(&m_Lock); } ;
	VOID	Unlock( ){ LeaveCriticalSection(&m_Lock); } ;
};

#endif
