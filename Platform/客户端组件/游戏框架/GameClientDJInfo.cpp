#include "StdAfx.h"
#include "gameclientdjinfo.h"
#include "UserDaoJu.h"
#include "ShowTimeDJList.h"


//道具图标
#define				GAME_DJ_ICO_PROP_FSK					TEXT("IMAGE_DJ_ICO_FSK")			//飞升卡
#define				GAME_DJ_ICO_PROP_JBK_S					TEXT("IMAGE_DJ_ICO_JBK_S")			//进宝卡(小)
#define				GAME_DJ_ICO_PROP_MLSS					TEXT("IMAGE_DJ_ICO_MLSS")			//名利双收卡
#define				GAME_DJ_ICO_PROP_JBK_M					TEXT("IMAGE_DJ_ICO_JBK_M")			//进宝卡(中)
#define				GAME_DJ_ICO_PROP_JBK_L					TEXT("IMAGE_DJ_ICO_JBK_L")			//进宝卡(大)
#define				GAME_DJ_ICO_PROP_HSZFK					TEXT("IMAGE_DJ_ICO_HSZFK")			//海神祝福卡
#define				GAME_DJ_ICO_PROP_GOODLUCK				TEXT("IMAGE_DJ_ICO_GOODLUCK")			//幸运女神卡
#define				GAME_DJ_ICO_PROP_GAME_RACE				TEXT("IMAGE_DJ_ICO_GAME_RACE")			//比赛资格卡




CGameClientDJInfo* CGameClientDJInfo::m_pDJInfo = NULL;


CGameClientDJInfo* CGameClientDJInfo::GetDJInfoInstance(void)
{
	if (NULL == m_pDJInfo)
	{
		m_pDJInfo = new CGameClientDJInfo();
	}
	return m_pDJInfo;
}

CGameClientDJInfo::CGameClientDJInfo(void)
{
	m_iCountCurrentDJTotalNums = 0;
	::memset(m_GameDJPropertyList, 0, sizeof(m_GameDJPropertyList));

	//加载资源
	LoadDJICOImage();
	//清空玩家
	m_vecUserHaveDJList.clear();
	m_vecUserUseDJList.clear();

	//
	m_pUserDaoJuInfo = NULL;
	m_pUserCurUseTimeDJ = NULL;
}


//设置用户道具指针
void CGameClientDJInfo::SetUserDaoJuInfoPtr(CUserDaoJu* pUserDaoJuInfoPtr)
{
	if (NULL != pUserDaoJuInfoPtr)
	{
		m_pUserDaoJuInfo = pUserDaoJuInfoPtr;
	}
}

//设置限时道具指针
void CGameClientDJInfo::SetUserTimeDJPtr(CShowTimeDJList* pUserTimeDJPtr)
{
	if (NULL != pUserTimeDJPtr)
	{
		m_pUserCurUseTimeDJ = pUserTimeDJPtr;
	}
}

CGameClientDJInfo::~CGameClientDJInfo(void)
{
	m_pUserDaoJuInfo = NULL;
	m_pUserCurUseTimeDJ = NULL;
	m_pDJInfo = NULL;
}



//添加道具列表
bool CGameClientDJInfo::AddGameDJInfo(const tagPropertyInfo *djInfo)
{
	if (NULL == djInfo)
	{
		return false;
	}
	
	if (m_iCountCurrentDJTotalNums < PROPERTY_COUNT)
	{
		m_GameDJPropertyList[m_iCountCurrentDJTotalNums].idjID = djInfo->nPropertyID;				//道具ID
		m_GameDJPropertyList[m_iCountCurrentDJTotalNums].dwLastingTime = djInfo->dwLastingTime;		//道具持续时间
		++m_iCountCurrentDJTotalNums;
		
		return true;
	}
	
	return false;
}


//添加玩家道具
void CGameClientDJInfo::AddUserDJList(const CMD_GF_ResidualProperty *pUserDJInfo)
{
	if (NULL == pUserDJInfo)
	{
		return;
	}

	/*
	use         nums
	false		0			:没有这种道具
	false		>0			:拥有这种道具，并没有使用。
	true		0			:正在使用，不过已经没有这种道具了。
	true		>0			:正在使用，并且还拥有这种道具。
	*/

	//清空数据
	m_vecUserHaveDJList.clear();	//拥有道具
	m_vecUserUseDJList.clear();		//使用道具
	tag_user_dj_info djinfo;
	for (int i=0; i<PROPERTY_COUNT; i++)
	{
		if (pUserDJInfo->dwResidual[i].dwID>=PROP_FSK && pUserDJInfo->dwResidual[i].dwID<=PROP_GOODLUCK)
		{//过滤部分道具
			if (!pUserDJInfo->dwResidual[i].bUse&&0==pUserDJInfo->dwResidual[i].dwResidualTime)
			{//玩家没有这种道具
				continue;
			}
			memset(&djinfo, 0, sizeof(tag_user_dj_info));
			djinfo.idjID = pUserDJInfo->dwResidual[i].dwID;	//道具ID
			djinfo.bUse = pUserDJInfo->dwResidual[i].bUse;	//是否有同类型的道具正在使用
			djinfo.idjNums = pUserDJInfo->dwResidual[i].dwResidualTime;	//还有剩余道具个数
			djinfo.dwResidualTimeCount = pUserDJInfo->dwResidual[i].dwResidualTimeCount;	//道具还剩余多少时间生效
			if (pUserDJInfo->dwResidual[i].dwResidualTime>0)
			{//拥有道具
				//添加玩家道具
				m_vecUserHaveDJList.push_back(djinfo);
			}
			if (pUserDJInfo->dwResidual[i].bUse && pUserDJInfo->dwResidual[i].dwResidualTimeCount>0)
			{//正在使用道具
				//添加玩家道具
				m_vecUserUseDJList.push_back(djinfo);
			}
		}
	}

	if (NULL != m_pUserDaoJuInfo)
	{//更新道具列表
		m_pUserDaoJuInfo->UpdateUserDJPage();
	}
	if (NULL != m_pUserCurUseTimeDJ)
	{//更新玩家限时道具信息
		m_pUserCurUseTimeDJ->SetDisableAndUpdateWnd(true);
	}
}

//设置某种道具数量及剩余时间
void CGameClientDJInfo::SetDJHaveProperty(CMD_GF_UserProperty* pUserProperty)
{
	if (NULL == pUserProperty)
	{
		return;
	}

	if (pUserProperty->dwID>=PROP_FSK && pUserProperty->dwID<=PROP_GOODLUCK)
	{
		bool bIsHave = false;
		for (int i=0; i<m_vecUserHaveDJList.size(); i++)
		{
			if (m_vecUserHaveDJList[i].idjID == pUserProperty->dwID)
			{
				bIsHave = true;
				m_vecUserHaveDJList[i].bUse = pUserProperty->bUse;
				m_vecUserHaveDJList[i].idjNums = pUserProperty->dwResidualTime;
				m_vecUserHaveDJList[i].dwResidualTimeCount = pUserProperty->dwResidualTimeCount;
				if (m_vecUserHaveDJList[i].idjNums <= 0)
				{//没有这种道具了。
					DelUserDJFromIndex(i);
				}
				break;
			}
		}
		tag_user_dj_info djinfo;
		memset(&djinfo, 0, sizeof(tag_user_dj_info));
		djinfo.idjID = pUserProperty->dwID;	//道具ID
		djinfo.bUse = pUserProperty->bUse;	//是否有同类型的道具正在使用
		djinfo.idjNums = pUserProperty->dwResidualTime;	//还有剩余道具个数
		djinfo.dwResidualTimeCount = pUserProperty->dwResidualTimeCount;	//道具还剩余多少时间生效
		if (!bIsHave)
		{//如果没有这种道具，则添加这种道具。
			if (pUserProperty->dwResidualTime>0)
			{//拥有道具
				//添加玩家道具
				m_vecUserHaveDJList.push_back(djinfo);
			}
		}
		bool IsHave = GetUserUseDJNums(pUserProperty->dwID);
		if (!IsHave)
		{//没有使用过这种道具.
			if (pUserProperty->bUse && pUserProperty->dwResidualTimeCount>0)
			{//正在使用道具
				//添加玩家道具
				m_vecUserUseDJList.push_back(djinfo);
			}
		}
		if (NULL != m_pUserDaoJuInfo)
		{//更新道具列表
			m_pUserDaoJuInfo->UpdateUserDJPage();
		}
		if (NULL != m_pUserCurUseTimeDJ)
		{//更新玩家限时道具信息
			m_pUserCurUseTimeDJ->SetDisableAndUpdateWnd(true);
		}
	}
}

//获得指定道具个数
bool CGameClientDJInfo::GetUserUseDJNums(int iDJID)
{
	if (iDJID>=PROP_FSK && iDJID<=PROP_GOODLUCK)
	{
		for (int i=0; i<m_vecUserUseDJList.size(); i++)
		{
			if (m_vecUserUseDJList[i].idjID == iDJID)
			{
				return true;
			}
		}
	}
	return false;
}

//设置使用道具数
void CGameClientDJInfo::SetDJUseProperty(CMD_GF_UserProperty* pUserProperty)
{
	if (NULL == pUserProperty)
	{
		return;
	}

	bool bIsFind = false;
	if (pUserProperty->dwID>=PROP_FSK && pUserProperty->dwID<=PROP_GOODLUCK)
	{
		for (int i=0; i<m_vecUserUseDJList.size(); i++)
		{
			if (m_vecUserUseDJList[i].idjID == pUserProperty->dwID)
			{
				m_vecUserUseDJList[i].bUse = pUserProperty->bUse;
				m_vecUserUseDJList[i].idjNums = pUserProperty->dwResidualTime;
				m_vecUserUseDJList[i].dwResidualTimeCount = pUserProperty->dwResidualTimeCount;
				
				if (!pUserProperty->bUse || m_vecUserUseDJList[i].dwResidualTimeCount <= 0)
				{//此道具时间已到，失效了。
					DelUserUseDJ(pUserProperty->dwID);
				}
				bIsFind = true;
				break;
			}
		}
	}
	if (!bIsFind && pUserProperty->dwResidualTimeCount>0)
	{//没有此道具信息，则添加道具信息.
		tag_user_dj_info info;
		::memset(&info, 0, sizeof(tag_user_dj_info));
		info.idjID = pUserProperty->dwID;
		info.bUse = pUserProperty->bUse;
		info.idjNums = pUserProperty->dwResidualTime;
		info.dwResidualTimeCount = pUserProperty->dwResidualTimeCount;
		m_vecUserUseDJList.push_back(info);
	}
	if (NULL != m_pUserCurUseTimeDJ)
	{//更新玩家限时道具信息
		m_pUserCurUseTimeDJ->SetResetWndSize();
	}
}


//删除玩家正在使用的道具
bool CGameClientDJInfo::DelUserUseDJ(int iDJID)
{
	if (iDJID>=PROP_FSK && iDJID<=PROP_GOODLUCK)
	{
		//删除当前道具
		std::vector<tag_user_dj_info>::iterator itemiter = m_vecUserUseDJList.begin();
		for (int i=0; (i<m_vecUserUseDJList.size()) && (itemiter!=m_vecUserUseDJList.end()); itemiter++, i++)
		{
			if (i==iDJID)
			{
				m_vecUserUseDJList.erase(itemiter);
				return true;
			}
		}
	}
	return false;
}



//删除拥有的道具
int CGameClientDJInfo::DelUserDJFromIndex(int iDJIndex)
{
	if (iDJIndex<0 || iDJIndex>=m_vecUserHaveDJList.size())
	{
		return 0;
	}

	bool bIsHave = false;
	//删除当前道具
	std::vector<tag_user_dj_info>::iterator itemiter = m_vecUserHaveDJList.begin();
	for (int i=0; (i<m_vecUserHaveDJList.size()) && (itemiter!=m_vecUserHaveDJList.end()); itemiter++, i++)
	{
		if (i==iDJIndex)
		{
			m_vecUserHaveDJList.erase(itemiter);
			bIsHave = true;
			break;
		}
	}
	if (false == bIsHave)
	{//没有找到
		return 0;
	}

	return iDJIndex;
}



//删除玩家与道具ID有关
bool CGameClientDJInfo::DelUserAllDJID(int iDJID)
{
	if (iDJID>=PROP_FSK && iDJID<=PROP_GOODLUCK)
	{
		int iSaveDJID = DelUserDJFromDJID(iDJID);
		bool bIsSuc = DelUserUseDJ(iDJID);

		if (iSaveDJID==iDJID)
		{
			if (NULL != m_pUserDaoJuInfo)
			{//更新道具列表
				m_pUserDaoJuInfo->UpdateUserDJPage();
			}
		}
		if (bIsSuc)
		{
			if (NULL != m_pUserCurUseTimeDJ)
			{//更新玩家限时道具信息
				m_pUserCurUseTimeDJ->SetResetWndSize();
			}
		}
	}
	return false;
}


int CGameClientDJInfo::DelUserDJFromDJID(int iDJID)
{
	if (iDJID>=PROP_FSK && iDJID<=PROP_GOODLUCK)
	{
		for (int i=0; i<m_vecUserHaveDJList.size(); i++)
		{
			if (m_vecUserHaveDJList[i].idjID == iDJID)
			{
				if (i == DelUserDJFromIndex(i))
				{
					if (NULL != m_pUserDaoJuInfo)
					{
						m_pUserDaoJuInfo->UpdateUserDJInfo(i);
					}
					return iDJID;
				}
				else
				{
					return 0;
				}
			}
		}
	}
	return 0;
}


//加载图片资源
void CGameClientDJInfo::LoadDJICOImage()
{
	HINSTANCE hInstance=GetModuleHandle(GAME_FRAME_DLL_NAME);
	char chImageData[100]={0};

	//飞升卡
	m_ImageDJICO[PROP_FSK].LoadImage(hInstance, GAME_DJ_ICO_PROP_FSK);
	memset(chImageData, 0, sizeof(chImageData));
	sprintf(chImageData, TEXT("%s_L"), GAME_DJ_ICO_PROP_FSK);
	m_ImageDJICOLight[PROP_FSK].LoadImage(hInstance, chImageData);

	//进宝卡(小)
	m_ImageDJICO[PROP_JBK_S].LoadImage(hInstance, GAME_DJ_ICO_PROP_JBK_S);
	memset(chImageData, 0, sizeof(chImageData));
	sprintf(chImageData, TEXT("%s_L"), GAME_DJ_ICO_PROP_JBK_S);
	m_ImageDJICOLight[PROP_JBK_S].LoadImage(hInstance, chImageData);

	//名利双收卡
	m_ImageDJICO[PROP_MLSS].LoadImage(hInstance, GAME_DJ_ICO_PROP_MLSS);
	memset(chImageData, 0, sizeof(chImageData));
	sprintf(chImageData, TEXT("%s_L"), GAME_DJ_ICO_PROP_MLSS);
	m_ImageDJICOLight[PROP_MLSS].LoadImage(hInstance, chImageData);

	//进宝卡(中)
	m_ImageDJICO[PROP_JBK_M].LoadImage(hInstance, GAME_DJ_ICO_PROP_JBK_M);
	memset(chImageData, 0, sizeof(chImageData));
	sprintf(chImageData, TEXT("%s_L"), GAME_DJ_ICO_PROP_JBK_M);
	m_ImageDJICOLight[PROP_JBK_M].LoadImage(hInstance, chImageData);

	//进宝卡(大)
	m_ImageDJICO[PROP_JBK_L].LoadImage(hInstance, GAME_DJ_ICO_PROP_JBK_L);
	memset(chImageData, 0, sizeof(chImageData));
	sprintf(chImageData, TEXT("%s_L"), GAME_DJ_ICO_PROP_JBK_L);
	m_ImageDJICOLight[PROP_JBK_L].LoadImage(hInstance, chImageData);

	//海神祝福卡
	m_ImageDJICO[PROP_HSZFK].LoadImage(hInstance, GAME_DJ_ICO_PROP_HSZFK);
	memset(chImageData, 0, sizeof(chImageData));
	sprintf(chImageData, TEXT("%s_L"), GAME_DJ_ICO_PROP_HSZFK);
	m_ImageDJICOLight[PROP_HSZFK].LoadImage(hInstance, chImageData);

	//幸运女神卡
	m_ImageDJICO[PROP_GOODLUCK].LoadImage(hInstance, GAME_DJ_ICO_PROP_GOODLUCK);
	memset(chImageData, 0, sizeof(chImageData));
	sprintf(chImageData, TEXT("%s_L"), GAME_DJ_ICO_PROP_GOODLUCK);
	m_ImageDJICOLight[PROP_GOODLUCK].LoadImage(hInstance, chImageData);

	//比赛资格卡
	m_ImageDJICO[PROP_GAME_RACE].LoadImage(hInstance, GAME_DJ_ICO_PROP_GAME_RACE);
	memset(chImageData, 0, sizeof(chImageData));
	sprintf(chImageData, TEXT("%s_L"), GAME_DJ_ICO_PROP_GAME_RACE);
	m_ImageDJICOLight[PROP_GAME_RACE].LoadImage(hInstance, chImageData);
}


//根据道具ID获得图片
CPngImage* CGameClientDJInfo::GetImageFromeDJID(int idjID)
{
	if (idjID<0 || idjID>=PROPERTY_COUNT)
	{
		return NULL;
	}
	return &m_ImageDJICO[idjID];
}
//获得加亮的图片
CPngImage* CGameClientDJInfo::GetLightImageFromeDJID(int idjID)
{
	if (idjID<0 || idjID>=PROPERTY_COUNT)
	{
		return NULL;
	}
	return &m_ImageDJICOLight[idjID];
}

//获得道具信息
const GC_DJPropertyInfo* CGameClientDJInfo::GetGameDJInfo(int idjid)
{
	if (idjid<0 || idjid>=PROPERTY_COUNT)
	{
		return NULL;
	}
	return &m_GameDJPropertyList[idjid];
}


//根据序号获得道具ID
int CGameClientDJInfo::GetDJIDFromDJIndex(int iIndex)
{
	if (iIndex<0 || iIndex>=m_vecUserHaveDJList.size())
	{
		return -1;
	}
	
	return m_vecUserHaveDJList[iIndex].idjID;
}


//获得正在使用的并且有限时道具个数
int CGameClientDJInfo::GetUserUseDJAndTimeDJNums()
{
	return m_vecUserUseDJList.size();
}

//获得玩家没有使用的道具
int CGameClientDJInfo::GetUserNoUseDJNums()
{
	return m_vecUserHaveDJList.size();
}

//使所有限时道具倒计时.并清理所有限时道具.
bool CGameClientDJInfo::SetTimeDJAndClearDJ()
{
	bool bIsHaveZero = false;
	for (int i=0; i<m_vecUserUseDJList.size(); i++)
	{
		if (m_vecUserUseDJList[i].idjID>=PROP_FSK && m_vecUserUseDJList[i].idjID<=PROP_GOODLUCK)
		{
			{//玩家正在使用的道具
				--m_vecUserUseDJList[i].dwResidualTimeCount;
				if (m_vecUserUseDJList[i].dwResidualTimeCount <= 0)
				{
					bIsHaveZero = true;
				}
			}
		}
	}
	
	if (true == bIsHaveZero)
	{//重置窗口大小
		std::vector<tag_user_dj_info>::iterator itemiter;
		for (itemiter=m_vecUserUseDJList.begin(); (m_vecUserUseDJList.size()>0) && (itemiter!=m_vecUserUseDJList.end()); itemiter++)
		{
			if (itemiter->dwResidualTimeCount<=0)
			{
				m_vecUserUseDJList.erase(itemiter);
				itemiter=m_vecUserUseDJList.begin();
			}
		}
	}

	return bIsHaveZero;
}