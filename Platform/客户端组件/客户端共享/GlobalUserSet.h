#ifndef __GLOBAL_USER_SET_H_E06D16BA_34A9_48c1_A486_9509DD8D4385
#define __GLOBAL_USER_SET_H_E06D16BA_34A9_48c1_A486_9509DD8D4385
#pragma once

#include "ClientShare.h"
#include "Resource.h"


class CLIENT_SHARE_CLASS CGlobalUserSet{
public:
	int m_nBackMusic;
	int m_nGameMusic;
	int m_nManMusic;
	int m_nCartoonMusic;
	int m_nLanguage;

public:
	CGlobalUserSet();
	~CGlobalUserSet();
	void InitGlobal();
	void UnitGlobal();
};

extern	CLIENT_SHARE_CLASS CGlobalUserSet	g_GolbalUserSet;				//全局的用户设置

#endif