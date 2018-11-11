#pragma once
#include "StdAfx.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "PublicDef.h"

using namespace std;

class CFilterString
{
public:
	CFilterString(void);
	~CFilterString(void);

public:
	//初始化数据
	void InitFilterData();
	//过滤字符
	void SetFilterString(CString& strData);

private:
	std::vector<CString> m_FilterList;
};
