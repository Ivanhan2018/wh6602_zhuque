//Create Author: osc
//Create Date:	2011-4-14
//Description:	道具信息列表,包括本人道具信息

#ifndef		__GAME_CLIENT_FRAME_DJ_INFO_DEF___
#define		__GAME_CLIENT_FRAME_DJ_INFO_DEF___

#include "StdAfx.h"
#include "GameFrame.h"
#include <vector>

using namespace std;

//道具信息结构
struct	GC_DJPropertyInfo
{
	int				idjID;				//道具ID
	DWORD			dwLastingTime;		//道具持续时间	
};

//玩家当前道具列表
struct	tag_user_dj_info
{
	int		idjID;		//道具ID
	int		idjNums;	//道具个数
	bool	bUse;       //是否还有道具正在使用
	DWORD	dwResidualTimeCount;  //还剩下多少时间道具失去作用 时间为秒
};

class CUserDaoJu;
class CShowTimeDJList;

//保存道具信息
class CGameClientDJInfo
{
public:
	//获得当前唯一实例
	static CGameClientDJInfo* GetDJInfoInstance(void);
	//设置用户道具指针
	void SetUserDaoJuInfoPtr(CUserDaoJu* pUserDaoJuInfoPtr);
	//设置限时道具指针
	void SetUserTimeDJPtr(CShowTimeDJList* pUserTimeDJPtr);

	//添加道具信息
	bool AddGameDJInfo(const tagPropertyInfo *djInfo);
	//获得道具信息
	const GC_DJPropertyInfo* GetGameDJInfo(int idjid);
	//根据道具ID获得图片
	CPngImage* GetImageFromeDJID(int idjID);
	CPngImage* GetLightImageFromeDJID(int idjID);

	//添加玩家道具
	void AddUserDJList(const CMD_GF_ResidualProperty *pUserDJInfo);
	//删除道具
	int DelUserDJFromIndex(int iDJIndex);
	int DelUserDJFromDJID(int iDJID);
	//根据序号获得道具ID
	int GetDJIDFromDJIndex(int iIndex);

	//获得正在使用的并且有限时道具个数
	int GetUserUseDJAndTimeDJNums();
	//获得玩家没有使用的道具
	int GetUserNoUseDJNums();
	//使所有限时道具倒计时.并清理所有限时道具.
	bool SetTimeDJAndClearDJ();
	//获得指定道具个数
	bool GetUserUseDJNums(int iDJID);

	//设置某种道具数量及人剩余时间
	void SetDJHaveProperty(CMD_GF_UserProperty* pUserProperty);
	//设置使用道具数
	void SetDJUseProperty(CMD_GF_UserProperty* pUserProperty);
	
	//删除玩家正在使用的道具
	bool DelUserUseDJ(int iDJID);
	//删除玩家与道具ID有关
	bool DelUserAllDJID(int iDJID);
	
public:
	//玩家道具列表
	std::vector<tag_user_dj_info>	m_vecUserHaveDJList;	//拥有道具
	std::vector<tag_user_dj_info>	m_vecUserUseDJList;		//使用道具

protected:
	CGameClientDJInfo(void);
public:
	virtual ~CGameClientDJInfo(void);
	
private:
	static CGameClientDJInfo*	m_pDJInfo;
	CUserDaoJu*					m_pUserDaoJuInfo;//玩家道具信息
	CShowTimeDJList*			m_pUserCurUseTimeDJ;	//玩家当前正在使用的限时道具
	
private:
	CPngImage						m_ImageDJICO[PROPERTY_COUNT];				//道具图标
	CPngImage						m_ImageDJICOLight[PROPERTY_COUNT];			//道具加亮图标
	GC_DJPropertyInfo				m_GameDJPropertyList[PROPERTY_COUNT];		//道具列表
	DWORD							m_iCountCurrentDJTotalNums;					//记录当前总道具数

private:
	//加载资源
	void	LoadDJICOImage();

	
};



#endif