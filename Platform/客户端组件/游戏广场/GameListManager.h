#pragma once
 
#include "GameInfo.h"
#include <afxtempl.h>
#include "SystemOption.h"

class CGameListManager
{
public:
	CGameListManager(void);
	~CGameListManager(void);
public:  
	CArray<CGameInfo,CGameInfo>		m_arrayGameInfoCollection;
	CArray<CGameRoomInfo,CGameRoomInfo>		m_arrayGameRoomInfo;
	CString							m_strHtml;
public:
	//初始化，读取所有的游戏信息
	bool Init();
	//游戏列表
	CString GetGameListHtml();
	//房间列表
	CString GetGameRoomHtml();
	//设置房间
	void SetHtmlRoom();
	//设置大厅
	void SetHtmlPlazz();
};
