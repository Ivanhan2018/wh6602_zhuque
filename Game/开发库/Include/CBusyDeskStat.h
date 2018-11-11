#pragma once
#ifndef  CCBUSYDESKSTAT_H_
#define  CCBUSYDESKSTAT_H_
#include <list>
class CCBusyDeskStat
{
	enum { MAX_SAMPLE=1 };
	typedef std::list<unsigned int>                           BusyDeskList;
	typedef std::list<unsigned int>::iterator                 BusyDeskListIt;
	BusyDeskList  m_busyDeskList;                      //繁忙座子的数量链表
public:
	//将目前繁忙桌子的数量写入
	void Insert(unsigned int num);
	//得到平均数量
    unsigned int  GetAverage(); 


public:
	CCBusyDeskStat(void);
	~CCBusyDeskStat(void);

};
#endif
