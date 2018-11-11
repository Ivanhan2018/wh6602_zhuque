/*
*  文件名字： CAutoQueue.h
*  创建日期： 2011.3.8
*  创建人：   彭文
*  功能：     实现了排队算法
*  描述：
*/

#include "ThreadGuard.h"

#ifndef CCAUTOQUEUE_H_
#define CCAUTOQUEUE_H_
#include <list>
#include <set>
#include "GameServiceExport.h"
class CCAutoQueue
{
private:
	typedef std::list<IServerUserItem*>				 ItemQueue; 
	typedef std::list<IServerUserItem*>::iterator    ItemQueueIt;
	ItemQueue	m_listAutoQueue;						//排队用户队列

	CCriticalSpection	m_lockTwo;		// add by HouGuoJiang 2012-03-27
														// 锁，针对 m_listAutoQueue 与 m_setAuto     

	typedef std::set<IServerUserItem*>				  ItemSet; 
	typedef std::set<IServerUserItem*>::iterator      ItemSetIt; 
	ItemSet     m_setAuto;                          //排队用户查找树  

private:
	CCAutoQueue(const CCAutoQueue&);
	CCAutoQueue& operator = (const CCAutoQueue&);
public:
	CCAutoQueue(void);
	virtual ~CCAutoQueue(void);

public:
	//清理
	void Clear(); 
	//删除一个用户
	void Erase(IServerUserItem* pUserItem);
	//查找用户是否存在
	bool IsExist(IServerUserItem* pUserItem);
	//向队列尾部增加一个用户
    void PushBack(IServerUserItem* pUserItem);
	//得到队列长度
	size_t Size();
	//从队列头部得到一个用户并且
	IServerUserItem* GetFront();
	//删除头结点
	void  PoPFront();
};



#endif