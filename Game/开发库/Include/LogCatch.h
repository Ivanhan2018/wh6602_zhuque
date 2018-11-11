/********************************************************************
	created:	2012/03/27
	created:	27:3:2012   15:27
	filename: 	e:\ZhuQue\Platform\服务器组件\游戏服务\LogCatch.h
	file path:	e:\ZhuQue\Platform\服务器组件\游戏服务
	file base:	LogCatch
	file ext:	h
	author:		侯国江
	purpose:	异步日志绑定了异常，可以定位到异常的函数
*********************************************************************/

#ifndef LOG_CATCH_H
#define LOG_CATCH_H

#include "cnd_ILogMgr.h"

#define __ENTER_FUNCTION_LOG_CATCH try{
#define __LEAVE_FUNCTION_LOG_CATCH }catch(...){		\
	if(m_pRoomLog)	\
{							\
	CString strErrorLine;		\
	strErrorLine.Format("函数名：%s  所在行：%d 文件名：%s",  __FUNCTION__ ,  __LINE__  , 	__FILE__ );	\
	CString strPrompt;	\
	strPrompt.Format("%s ,出错原因： 待查。", strErrorLine.GetBuffer());	\
	CTraceService::TraceString(strPrompt,TraceLevel_Exception);	\
	m_pRoomLog->AddMsg(0, 0, TRUE, strPrompt.GetBuffer());		\
} }	



#endif