#include "GameClientEx.h"
#include "GameClient.h"

extern "C" IGameClientFrame* NewGameClient(CWnd* pParent, DWORD dwWndID)
{
	if (NULL==pParent || 0==dwWndID)
	{
		return NULL;
	}

	CGameClient *pGameClient = new CGameClient();
	if (NULL != pGameClient)
	{
		CRect rcGameView(0,0,0,0);
		BOOL bSuc = pGameClient->Create(NULL,NULL,WS_CHILD|WS_CLIPSIBLINGS|WS_CLIPCHILDREN,rcGameView,pParent,dwWndID);
		if (bSuc)
		{//创建成功
			pGameClient->SetMeSocketID(dwWndID);
			pGameClient->SetWindowPos(NULL, 0, 0, 1, 1, 0);
			pGameClient->ShowWindow(SW_HIDE);
			return pGameClient;
		}
	}
	
	return NULL;
}

extern "C" ANDROID_GAME_FRAME_EX void FreeGameClient(IGameClientFrame* pClient)
{
	if (NULL == pClient)
	{
		return;
	}
	CGameClient *pGameClient = (CGameClient*)pClient;
	if (NULL != pGameClient)
	{
		if (pGameClient->IsEnableWnd())
		{
			pGameClient->CloseWnd();
		}
		delete pGameClient;
		pGameClient = NULL;
	}
}