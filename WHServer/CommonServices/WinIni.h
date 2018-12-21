#ifndef WIN_INI_HEAD_FILE
#define WIN_INI_HEAD_FILE

#pragma once

#include <vector>
#include <string>

#define MAX_VALUE	256
//////////////////////////////////////////////////////////////////////////
#define LOAD_INT(FILE, SECTION, KEY)	\
			::GetPrivateProfileInt(SECTION, KEY, 0, FILE);

#define SAVE_INT(FILE, SECTION, KEY, VALUE) do{	\
			CString szKeyValue;    \
			szKeyValue.Format("%d", nKeyValue);    \
			::WritePrivateProfileString(SECTION, KEY, VALUE, FILE);}while(0)

#define LOAD_STRING(FILE, SECTION, KEY, VALUE)	do{	\
			::GetPrivateProfileString(SECTION, KEY, "", VALUE.GetBuffer(MAX_VALUE), MAX_VALUE, FILE);	\
			VALUE.ReleaseBuffer();}while(0)

#define SAVE_STRING(FILE, SECTION, KEY, VALUE) \
			::WritePrivateProfileString(SECTION, KEY, VALUE, FILE); 

//////////////////////////////////////////////////////////////////////////

#endif