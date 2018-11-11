#include "StdAfx.h"
#include ".\cbusydeskstat.h"

CCBusyDeskStat::CCBusyDeskStat(void)
{
}

CCBusyDeskStat::~CCBusyDeskStat(void)
{
}

//将目前繁忙桌子的数量写入
void CCBusyDeskStat::Insert(unsigned int num)
{
	if (m_busyDeskList.size() < MAX_SAMPLE)
	{
		m_busyDeskList.push_back(num);
	}else{
		m_busyDeskList.pop_front();
		m_busyDeskList.push_back(num);
	}
}
//得到平均数量
unsigned int  CCBusyDeskStat::GetAverage()
{
	unsigned int count = 0;
	BusyDeskListIt it = m_busyDeskList.begin();
	BusyDeskListIt itE = m_busyDeskList.end();
	for (; it != itE; ++it)
	{
		count += (*it);
	}
	return (count / m_busyDeskList.size());
}