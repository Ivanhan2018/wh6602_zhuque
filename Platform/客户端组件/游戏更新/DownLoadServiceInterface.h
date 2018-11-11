#ifndef		___H_DEF_GAME_DOWN_LOAD_SERVICE_INTERFACE_H_DEF___
#define		___H_DEF_GAME_DOWN_LOAD_SERVICE_INTERFACE_H_DEF___

#include "DownLoad.h"
//类说明
class CDownLoadMission;
class CDownLoadService;
typedef CArrayTemplate<CDownLoadMission *> CDownLoadMissionArray;

//下载状态
enum enDownLoadStatus
{
	enDownLoadStatus_Unknow,			//未知状态
	enDownLoadStatus_Ready,				//准备状态
	enDownLoadStatus_DownLoadIng,		//下载状态
	enDownLoadStatus_Finish,			//完成状态
	enDownLoadStatus_Fails,				//下载失败
};


//任务回调接口
interface IDownLoadMissionSink
{
	//下载通知
	virtual void OnMissionFinish(enDownLoadStatus DownLoadStatus, CDownLoadMission * pDownLoadMission)=NULL;
};

//////////////////////////////////////////////////////////////////////////

//下载服务
class IDOWN_LOAD_CLASS CDownLoadService : public IDownLoadMissionSink
{
	//变量定义
protected:
	DWORD								m_dwMissionID;					//任务标识
	IDownLoadServiceSink				* m_pIDownLoadServiceSink;		//回调接口
	CDownLoadMissionArray				m_DownLoadMissionActive;		//下载任务
	CDownLoadMissionArray				m_DownLoadMissionRelease;		//下载任务

	//函数定义
public:
	//构造函数
	CDownLoadService();
	//析构函数
	virtual ~CDownLoadService();

	//基础接口
public:
	//释放对象
	virtual void __cdecl Release() { delete this; }
	//接口查询
	virtual void * __cdecl QueryInterface(const IID & Guid, DWORD dwQueryVer);

	//配置接口
public:
	//获取数目
	virtual INT_PTR __cdecl GetDownLoadMissionCount();
	//设置接口
	virtual bool __cdecl SetDownLoadServiceSink(IUnknownEx * pIUnknownEx);
	//下载请求
	virtual DWORD __cdecl AddDownLoadRequest(DWORD dwDownLoadType, tagDownLoadRequest * pDownLoadRequest);

	//事件接口
public:
	//下载通知
	virtual void OnMissionFinish(enDownLoadStatus DownLoadStatus, CDownLoadMission * pDownLoadMission);

	//内部函数
private:
	//查找下载
	CDownLoadMission * SearchMission(tagDownLoadRequest * pDownLoadRequest);
};

//////////////////////////////////////////////////////////////////////////








#endif