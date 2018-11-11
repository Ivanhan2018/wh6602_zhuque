#pragma once

#include "stdafx.h"
#include "ISonicUI.h"
#include "ISonicBaseData.h"
using namespace sonic_ui;

ISonicBase::ISonicBase()
{
	m_dwType = 0;
}

ISonicBase::~ISonicBase()
{
	
}

DWORD ISonicBase::GetObjectId()
{
	return (DWORD)(DWORD_PTR)this;
}

DWORD ISonicBase::GetType()
{
	return m_dwType;
}

void ISonicBase::Delegate(UINT message, LPVOID pParam, LPVOID pClass, ...)
{
	if(IsValid() == FALSE)
	{
		return;
	}
	ISonicBaseData * pData = dynamic_cast<ISonicBaseData *>(this);
	if(pData == NULL)
	{
		return;
	}
	DELEGATE_PARAM pm = {0};
	pm.pClass = pClass;
	pm.pParam = pParam;
	va_list argPtr;
	va_start(argPtr, pClass);
	pm.pFunc = va_arg(argPtr, LPVOID);
	va_end(argPtr);
	pData->m_mapDelegate[message] = pm;
}

HWND ISonicBase::GetSafeHwnd()
{
	ISonicBaseData * pData = dynamic_cast<ISonicBaseData *>(this);
	if(pData == NULL)
	{
		return NULL;
	}
	return pData->m_hWnd;
}

//////////////////////////////////////////////////////////////////////////
//
// ISonicBaseCtrl
void ISonicBaseCtrl::SwitchRender(BOOL b)
{
	m_bRender = b;
}

BOOL ISonicBaseCtrl::GetRenderSwitch()
{
	return m_bRender;
}