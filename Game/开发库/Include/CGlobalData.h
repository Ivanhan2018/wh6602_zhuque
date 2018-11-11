#pragma once
#include "GameServiceExport.h"
#include <set> 

class CCGlobalData
{
public:
	CCGlobalData(void);
	~CCGlobalData(void);
private:
	static CMD_GP_UserGradeInfo*           m_pGradeInfo;                      //等级信息
	static int                             m_iGradeNum;                        //等级数目
public:
	//分配等级内存
	static void AllocGradeInfoMemory(int num);
	//释放等级信息内存
	static void UnAllocGradeInfoMemory();
	//得到等级信息
	/*static CMD_GP_UserGradeInfo*  GetGradeInfo(int index);  */
	//设置等级数据
	static void SetGradeInfoData(int id, TCHAR* szName, int iMaxExp);
	//根据经验计算新的等级
	static int  CaluGrade(LONG exp); 
};	



class  CCTask
{
	typedef std::set<DWORD>            TaskMarkSet;
	typedef std::set<DWORD>::iterator  TaskMarkSetIt;

	TaskMarkSet                               m_taskMarkSet;  //标志查询集合
public:
	void SetMark(DWORD taskMark);
	void Init();
	void Clear();
	bool FindMark(DWORD taskMark);
	//兼容之前位操作模式
	bool operator &(DWORD taskMark);
	bool operator |(DWORD taskMark);

};