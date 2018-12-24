#pragma once

#include "ServerItemView.h"

class CGameInfo
{
public:
	CGameInfo(void);
	~CGameInfo(void);
	bool GetFromFile(CString path);

	//属性
public:
	CString					m_strPath;//文件路径
	CString					m_strName;
	WORD					m_wKindID;//游戏ID
	//int						m_nSort;
	
public:
	

};

//房间信息
class CGameRoomInfo
{
public:
	CListServer * m_pListServer;

public:
	CGameRoomInfo();
	~CGameRoomInfo();

	WORD GetKindID(){return m_pListServer->m_pListKind->m_GameKind.wKindID;};
	WORD GetSortId(){return m_pListServer->m_GameServer.wSortID;};
};
