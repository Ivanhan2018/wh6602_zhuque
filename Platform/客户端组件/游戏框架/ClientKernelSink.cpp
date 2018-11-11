#include "StdAfx.h"
#include "GameFrameDlg.h"
#include "GlobalOption.h"
#include "UserInfoDlg.h"
#include "GameFrameControl.h"
#include "ClientKernelSink.h"

//////////////////////////////////////////////////////////////////////////

//构造函数
CClientKernelSink::CClientKernelSink(CGameFrameDlg & GameFrameDlg) : m_GameFrameDlg(GameFrameDlg)
{
}

//析构函数
CClientKernelSink::~CClientKernelSink()
{
}

//接口查询
void * __cdecl CClientKernelSink::QueryInterface(const IID & Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IClientKernelSink,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IClientKernelSink,Guid,dwQueryVer);
	return NULL;
}

//获取窗口
HWND __cdecl CClientKernelSink::GetFrameWnd()
{
	return m_GameFrameDlg.m_hWnd;
}

//重置状态
void __cdecl CClientKernelSink::ResetGameFrame()
{
	//设置变量
	m_GameFrameDlg.m_bGameSceen=false;

	//重置界面
	m_GameFrameDlg.m_pGameFrameView->ResetData();
	m_GameFrameDlg.m_pGameFrameView->ResetGameView();
	m_GameFrameDlg.ResetGameFrame();
	m_GameFrameDlg.m_pGameFrameControl->ResetGameFrameControl();

#if REMOVE_VIDEO // add by HouGuoJiang 2011-11-25
	//重置视频
	CVideoServiceManager * pVideoServiceManager=CVideoServiceManager::GetInstance();
	if (pVideoServiceManager!=NULL) pVideoServiceManager->OnEventGameReset();
#endif

	return;
}

//关闭窗口
void __cdecl CClientKernelSink::CloseGameFrame(bool bForceClose)
{
	if (m_GameFrameDlg.m_hWnd!=NULL) m_GameFrameDlg.PostMessage(WM_CLOSE,0,0);
	return;
}

//时间消息
bool __cdecl CClientKernelSink::OnEventTimer(WORD wChairID, UINT nElapse, UINT nTimerID)
{
	//变量界面
	WORD wViewChairID=m_GameFrameDlg.SwitchViewChairID(wChairID);
	m_GameFrameDlg.m_pGameFrameView->SetUserTimer(wViewChairID,nElapse);

	//事件通知
	return m_GameFrameDlg.OnTimerMessage(wChairID,nElapse,nTimerID);
}

//网络消息
bool __cdecl CClientKernelSink::OnEventSocket(const CMD_Command & Command, const void * pBuffer, WORD wDataSize)
{
	switch (Command.wMainCmdID)
	{
	case MDM_GF_FRAME:	//框架消息
		{
			return m_GameFrameDlg.OnFrameMessage(Command.wSubCmdID,pBuffer,wDataSize);
		}
	case MDM_GF_GAME:	//游戏消息
		{
			return m_GameFrameDlg.OnGameMessage(Command.wSubCmdID,pBuffer,wDataSize);
		}
	case MDM_GF_VIDEO:	//视频消息
		{
			switch (Command.wSubCmdID)
			{
			case SUB_GF_S_VIDEO_CONFIG:		//视频配置
				{
					//效验参数
					ASSERT(wDataSize==sizeof(CMD_S_VideoConfig));
					if (wDataSize!=sizeof(CMD_S_VideoConfig)) return false;

					//消息处理
					CMD_S_VideoConfig * pVideoConfig=(CMD_S_VideoConfig *)pBuffer;

					//查找用户
					tagUserData * pRomterUserData=m_GameFrameDlg.m_ClientKernelHelper->SearchUserItem(pVideoConfig->dwLocalUserID);
					if (pRomterUserData==NULL) return true;

					//用户效验
					ASSERT((m_GameFrameDlg.IsLookonMode()==false)&&(pRomterUserData->cbUserStatus!=US_LOOKON));
					if ((m_GameFrameDlg.IsLookonMode()==true)||(pRomterUserData->cbUserStatus==US_LOOKON)) return true;

				#if REMOVE_VIDEO // add by HouGuoJiang 2011-11-25
					//获取视频
					WORD wViewChairID=m_GameFrameDlg.SwitchViewChairID(pRomterUserData->wChairID);
					CVideoServiceManager * pVideoServiceManager=CVideoServiceManager::GetInstance();

					//视频处理
					if (pVideoServiceManager!=NULL)
					{
						//变量定义
						tagVideoNetwordInfo VideoNetwordInfo;
						ZeroMemory(&VideoNetwordInfo,sizeof(VideoNetwordInfo));

						//设置变量
						VideoNetwordInfo.wNatPort=pVideoConfig->wNatPort;
						VideoNetwordInfo.wLocalPort=pVideoConfig->wLocalPort;
						VideoNetwordInfo.dwNatAddr=pVideoConfig->dwNatAddr;
						VideoNetwordInfo.dwLocalAddr=pVideoConfig->dwLocalAddr;

						//视频配置
						pVideoServiceManager->OnEventUserConfig(wViewChairID,pVideoConfig->dwLocalUserID,&VideoNetwordInfo);
					}
				#endif
					return true;
				}
			case SUB_GF_S_VIDEO_CLOSE:		//关闭视频
				{
					//效验参数
					ASSERT(wDataSize==sizeof(CMD_S_VideoClose));
					if (wDataSize!=sizeof(CMD_S_VideoClose)) return false;

					//消息处理
					CMD_S_VideoClose * pVideoClose=(CMD_S_VideoClose *)pBuffer;

					//查找用户
					tagUserData const * pRomterUserData=m_GameFrameDlg.GetUserData(pVideoClose->wCloseUser);
					if (pRomterUserData==NULL) return true;

					//用户效验
					ASSERT((m_GameFrameDlg.IsLookonMode()==false)&&(pRomterUserData->cbUserStatus!=US_LOOKON));
					if ((m_GameFrameDlg.IsLookonMode()==true)||(pRomterUserData->cbUserStatus==US_LOOKON)) return true;
				#if REMOVE_VIDEO // add by HouGuoJiang 2011-11-25
					//获取视频
					WORD wViewChairID=m_GameFrameDlg.SwitchViewChairID(pRomterUserData->wChairID);
					CVideoServiceManager * pVideoServiceManager=CVideoServiceManager::GetInstance();

					//视频处理
					if (pVideoServiceManager!=NULL)
					{
						pVideoServiceManager->OnEventCloseVideo( wViewChairID );
					}
				#endif
					return true;
				}
			case SUB_GF_S_VIDEO_OPEN:		//打开视频
				{
					//效验参数
					ASSERT(wDataSize==sizeof(CMD_S_VideoOpen));
					if (wDataSize!=sizeof(CMD_S_VideoOpen)) return false;

					//消息处理
					CMD_S_VideoOpen * pVideoOpen=(CMD_S_VideoOpen *)pBuffer;

					//查找用户
					tagUserData const * pRomterUserData=m_GameFrameDlg.GetUserData(pVideoOpen->wOpenUser);
					if (pRomterUserData==NULL) return true;

					//用户效验
					ASSERT((m_GameFrameDlg.IsLookonMode()==false)&&(pRomterUserData->cbUserStatus!=US_LOOKON));
					if ((m_GameFrameDlg.IsLookonMode()==true)||(pRomterUserData->cbUserStatus==US_LOOKON)) return true;

				#if 0 // add by HouGuoJiang 2011-11-25
					//获取视频
					WORD wViewChairID=m_GameFrameDlg.SwitchViewChairID(pRomterUserData->wChairID);
					CVideoServiceManager * pVideoServiceManager=CVideoServiceManager::GetInstance();

					//视频处理
					if (pVideoServiceManager!=NULL)
					{
						pVideoServiceManager->OnEventOpenVideo( wViewChairID );
					}
				#endif

					return true;
				}
			}
		}
	}

	return false;
}

//进程消息
bool __cdecl CClientKernelSink::OnEventProcess(const IPC_Head & Head, const void * pBuffer, WORD wDataSize, HWND hWndSend)
{
	return false;
}

//游戏状态
bool __cdecl CClientKernelSink::OnEventGameScene(BYTE cbGameStation, bool bLookonOther, void * pBuffer, WORD wDataSize)
{
	//设置变量
	m_GameFrameDlg.m_bGameSceen=true;

	return m_GameFrameDlg.OnGameSceneMessage(cbGameStation,bLookonOther,pBuffer,wDataSize);
}

//聊天信息
bool __cdecl CClientKernelSink::OnUserChatMessage(tagUserData * pSendUserData, tagUserData * pRecvUserData, LPCTSTR pszChatMessage, COLORREF crColor)
{
	CString strChatMessage = "";
	strChatMessage.Append(pszChatMessage);
	//设置过滤
	g_GlobalOption.m_FilterString.SetFilterString(strChatMessage);
	if (strChatMessage.IsEmpty())
	{
		return true;
	}
	
	//显示消息
	if (pRecvUserData==NULL) 
	{
		const tagUserData * pMeInfo = m_GameFrameDlg.m_ClientKernelHelper->GetMeUserInfo();
		if (NULL != pMeInfo)
		{
			if (pMeInfo->dwUserID==pSendUserData->dwUserID)
			{
				m_GameFrameDlg.m_MessageProxyHelper->InsertUserChat(pSendUserData->szName,strChatMessage.GetBuffer(),crColor,MS_NORMAL);
			}
			else
			{
				m_GameFrameDlg.m_MessageProxyHelper->InsertUserChat("玩家",strChatMessage.GetBuffer(),crColor,MS_NORMAL);
			}
		}
	}
	else 
	{//现在游戏中不会有目标
		m_GameFrameDlg.m_MessageProxyHelper->InsertUserChat(pSendUserData->szName,pRecvUserData->szName,strChatMessage.GetBuffer(),crColor,MS_NORMAL);
	}
	
	//响应消息
	//OnEventHaveChatMessage();

	return true;
}

//有消息
void __cdecl CClientKernelSink::OnEventHaveChatMessage()
{
	m_GameFrameDlg.SetChatMessageBeep();
}

//公告信息
void __cdecl CClientKernelSink::OnEventGameAfficheInfo(const char* pDataInfo, WORD wDataSize)
{
	m_GameFrameDlg.OnEventGameAfficheInfo(pDataInfo, wDataSize);
}

//用户md5密码
void __cdecl CClientKernelSink::OnEventUserMD5PWMessage(void *pBuffer)
{
	if (NULL != pBuffer)
	{
		IPC_UserPassWord * pUserMD5PW=(IPC_UserPassWord *)pBuffer;
		if (NULL != pUserMD5PW)
		{
			m_GameFrameDlg.OnEventUserMD5PWMsg(pUserMD5PW->chUserPassWord);
		}
	}
}

//用户动作消息
void __cdecl CClientKernelSink::OnEventUserActionInfo(BYTE btSender, BYTE btRecver, BYTE btTag)
{
	m_GameFrameDlg.OnEventUserActionInfo(btSender, btRecver, btTag);
}

//用户语音消息
void __cdecl CClientKernelSink::OnEventUserLanguageInfo(BYTE bySender, DWORD dwIndex)
{
	m_GameFrameDlg.OnEventUserLanguageInfo(bySender, dwIndex);
}

//时间更新
void __cdecl CClientKernelSink::OnEventTimerKilled(WORD wChairID, UINT nTimerID)
{
	//设置界面
	WORD wViewChairID=m_GameFrameDlg.SwitchViewChairID(wChairID);
	m_GameFrameDlg.m_pGameFrameView->SetUserTimer(wViewChairID,0);

	return;
}

//旁观状态
void __cdecl CClientKernelSink::OnEventLookonChanged(bool bLookonUser, void * pBuffer, WORD wDataSize)
{
	//设置界面
	m_GameFrameDlg.OnLookonChanged(bLookonUser,pBuffer,wDataSize);

	return;
}

//用户进入
void __cdecl CClientKernelSink::OnEventUserEnter(tagUserData * pUserData, WORD wChairID, bool bLookonUser)
{
	//设置列表
	//m_GameFrameDlg.m_pGameFrameControl->m_UserListView.m_wKindID=m_GameFrameDlg.m_ClientKernelHelper->GetServerAttribute()->wKindID;
	//m_GameFrameDlg.m_pGameFrameControl->m_UserListView.m_wGameGenre=m_GameFrameDlg.m_ClientKernelHelper->GetServerAttribute()->wGameGenre;

//#ifdef _DEBUG
//		TCHAR szUserCount[64];
//		_sntprintf(szUserCount,CountArray(szUserCount),TEXT("[ %s ]加入"),pUserData->szName);
//		m_GameFrameDlg.InsertSystemString(szUserCount);	
//#endif

	//插入列表
	m_GameFrameDlg.m_pGameFrameControl->InsertUserItem(pUserData);

	//设置界面
	if (bLookonUser==false)
	{
		WORD wViewChairID=m_GameFrameDlg.SwitchViewChairID(wChairID);
		m_GameFrameDlg.m_pGameFrameView->SetUserInfo(wViewChairID,pUserData);
	}

	//插入用户
	if (pUserData!=m_GameFrameDlg.m_ClientKernelHelper->GetMeUserInfo())
	{
		//插入用户
		//int nItem=m_GameFrameDlg.m_pUserInfoDlg->m_pGameFrameControl->m_ChatObject.AddString(pUserData->szName);
		//m_GameFrameDlg.m_pUserInfoDlg->m_pGameFrameControl->m_ChatObject.SetItemData(nItem,pUserData->dwUserID);

		//提示消息
		if (m_GameFrameDlg.m_bGameSceen==true)
		{
			if (US_LOOKON!=pUserData->cbUserStatus)
			{
				m_GameFrameDlg.m_MessageProxyHelper->InsertUserName("玩家"/*pUserData->szName*/);
				m_GameFrameDlg.m_MessageProxyHelper->InsertGeneralString(TEXT("进来了"),g_GlobalOption.m_crMsgNormal,0,true);
			}

			//响应聊天消息
			OnEventHaveChatMessage();
		}
	}

	//事件通知
	m_GameFrameDlg.OnEventUserEnter(pUserData,wChairID,bLookonUser);

	//视频通知
	if (bLookonUser==false)
	{
	#if REMOVE_VIDEO // add by HouGuoJiang 2011-11-25
		//获取视频
		CVideoServiceManager * pVideoServiceManager=CVideoServiceManager::GetInstance();

		//视频通知
		if (pVideoServiceManager!=NULL)
		{
			WORD wViewChairID=m_GameFrameDlg.SwitchViewChairID(wChairID);
			pVideoServiceManager->OnEventUserEnter(wViewChairID,pUserData->dwUserID);
		}
	#endif
	}



	//信息打印
	//char chData[255]={0};
	//sprintf(chData, "%d玩家进来的宝石:%d,等级:%d", wChairID, pUserData->lGems, pUserData->dwClass);
	//m_GameFrameDlg.m_MessageProxyHelper->InsertGeneralString(TEXT(chData),g_GlobalOption.m_crMsgNormal,0,true);

	return;
}

//用户离开
void __cdecl CClientKernelSink::OnEventUserLeave(tagUserData * pUserData, WORD wChairID, bool bLookonUser)
{
	//事件通知
	m_GameFrameDlg.OnEventUserLeave(pUserData,wChairID,bLookonUser);

	//视频通知
	if ( ! bLookonUser )
	{
	#if REMOVE_VIDEO // add by HouGuoJiang 2011-11-25
		WORD wViewChairID=m_GameFrameDlg.SwitchViewChairID(wChairID);
		CVideoServiceManager * pVideoServiceManager=CVideoServiceManager::GetInstance();
		if (pVideoServiceManager!=NULL) pVideoServiceManager->OnEventUserLeave(wViewChairID,pUserData->dwUserID);
	#endif
	}

	//设置界面
	if (bLookonUser==false)
	{
		WORD wViewChairID=m_GameFrameDlg.SwitchViewChairID(wChairID);
		m_GameFrameDlg.m_pGameFrameView->SetUserInfo(wViewChairID,NULL);
	}

	//删除列表
	m_GameFrameDlg.m_pGameFrameControl->DeleteUserItem(pUserData);

	//提示消息
	if (m_GameFrameDlg.m_bGameSceen==true)
	{
		if(US_LOOKON!=pUserData->cbUserStatus)
		{
			m_GameFrameDlg.m_MessageProxyHelper->InsertUserName("玩家"/*pUserData->szName*/);
			m_GameFrameDlg.m_MessageProxyHelper->InsertGeneralString(TEXT("离开了"),g_GlobalOption.m_crMsgNormal,0,true);
		}
		
		//响应聊天消息
		OnEventHaveChatMessage();
	}

	//删除用户
	//CComboBox * pChatObject=&m_GameFrameDlg.m_pUserInfoDlg->m_pGameFrameControl->m_ChatObject;
	//int nCount=pChatObject->GetCount();
	//for (int i=1;i<nCount;i++)
	//{
	//	if (pChatObject->GetItemData(i)==pUserData->dwUserID)
	//	{
	//		if (pChatObject->GetCurSel()==i) pChatObject->SetCurSel(0);
	//		pChatObject->DeleteString(i);
	//		break;
	//	}
	//}

	return;
}

//用户积分
void __cdecl CClientKernelSink::OnEventUserScore(tagUserData * pUserData, WORD wChairID, bool bLookonUser)
{
	//设置界面
	m_GameFrameDlg.m_pGameFrameControl->UpdateUserItem(pUserData);
	if (bLookonUser==false)
	{
		WORD wViewChairID=m_GameFrameDlg.SwitchViewChairID(wChairID);
		m_GameFrameDlg.m_pGameFrameView->SetUserInfo(wViewChairID, pUserData);
		m_GameFrameDlg.m_pGameFrameView->UpdateGameView(NULL);
	}

	//事件通知
	m_GameFrameDlg.OnEventUserScore(pUserData,wChairID,bLookonUser);


	//信息打印
	//char chData[255]={0};
	//sprintf(chData, "%d玩家用户信息更新宝石:%d,等级:%d", wChairID, pUserData->lGems, pUserData->dwClass);
	//m_GameFrameDlg.m_MessageProxyHelper->InsertGeneralString(TEXT(chData),g_GlobalOption.m_crMsgNormal,0,true);

	return;
}

//用户状态
void __cdecl CClientKernelSink::OnEventUserStatus(tagUserData * pUserData, WORD wChairID, bool bLookonUser)
{
	//设置界面
	m_GameFrameDlg.m_pGameFrameControl->UpdateUserItem(pUserData);
	if (bLookonUser==false) m_GameFrameDlg.m_pGameFrameView->UpdateGameView(NULL);

	//事件通知
	m_GameFrameDlg.OnEventUserStatus(pUserData,wChairID,bLookonUser);

	//视频通知
	tagUserData const * pMeUserData=m_GameFrameDlg.GetUserData(m_GameFrameDlg.GetMeChairID());
	if ((pMeUserData!=pUserData)&&((pUserData->cbUserStatus==US_OFFLINE)||(pUserData->cbUserStatus==US_PLAY)))
	{
	#if REMOVE_VIDEO // add by HouGuoJiang 2011-11-25
		//椅子转换
		WORD wViewChairID=m_GameFrameDlg.SwitchViewChairID(pUserData->wChairID);

		//事件通知
		CVideoServiceManager * pVideoServiceManager=CVideoServiceManager::GetInstance();
		if (pVideoServiceManager!=NULL) pVideoServiceManager->OnUpdateUserItem(wViewChairID,pUserData->cbUserStatus,pUserData->dwUserID);
	#endif
	}

	return;
}

//会员等级
void __cdecl CClientKernelSink::OnEventUserMemberOrder(tagUserData * pUserData, WORD wChairID, bool bLookonUser)
{
	//设置界面
	m_GameFrameDlg.m_pGameFrameControl->UpdateUserItem(pUserData);
	if (bLookonUser==false) m_GameFrameDlg.m_pGameFrameView->UpdateGameView(NULL);

	return;
}

//初始道具
void __cdecl CClientKernelSink::OnEventInitProperty(tagPropertyInfo *pPropertyInfo, int nInfoCount)
{
	m_GameFrameDlg.OnEventInitProperty(pPropertyInfo, nInfoCount);
	return;
}

//初始鲜花
void __cdecl CClientKernelSink::OnEventInitFlower(tagFlowerInfo *pFlowerInfo, int nInfoCount)
{
	m_GameFrameDlg.OnEventInitFlower(pFlowerInfo, nInfoCount);

	return;
}

//鲜花消息
void __cdecl CClientKernelSink::OnEventFlower(const tagUserData * pSendUserData, const tagUserData * pRecvUserData, UINT uFlowerID, UINT uFlowerEffectID)
{
	//播放声音
	m_GameFrameDlg.PlayGameSound(GetModuleHandle(GAME_FRAME_DLL_NAME), TEXT("FLOWER"));

	//对方动画
	m_GameFrameDlg.OnEventFlower(pSendUserData, pRecvUserData, uFlowerID, uFlowerEffectID+1);	

	//本身动画
	if(pSendUserData->dwUserID!=pRecvUserData->dwUserID)
		m_GameFrameDlg.OnEventFlower(pSendUserData, pSendUserData, uFlowerID, uFlowerEffectID);	

	return;
}


//玩家剩余道具
void __cdecl CClientKernelSink::OnEventUserHaveDJInfo(CMD_GF_ResidualProperty *pUserDJInfo)
{
	if (NULL == pUserDJInfo)
	{
		return;
	}
	m_GameFrameDlg.OnEventUserHaveDJInfo(pUserDJInfo);
}

//道具操作信息
void __cdecl CClientKernelSink::OnEventUserOperationDJInfo(BYTE byTag, void * pBuffer)
{
	if (0==byTag || NULL==pBuffer)
	{
		return;
	}
	m_GameFrameDlg.OnEventUserOperationDJInfo(byTag, pBuffer);
}

//道具赠送消息
void __cdecl CClientKernelSink::OnEventGameDJInfo(const char* pInfo)
{
	if (NULL == pInfo)
	{
		return;
	}
	m_GameFrameDlg.OnEventGameDJInfo(pInfo);
}



//OSC update
//接收新任务
void __cdecl CClientKernelSink::OnEventTaskAccept(const CMD_GF_TASK_tagTask* pTaskAccept)
{
	if (NULL != pTaskAccept)
	{
		m_GameFrameDlg.OnEventTaskAccept(pTaskAccept);
	}
}
//单个任务完成
void __cdecl CClientKernelSink::OnEventTaskOnceFinish(const CMD_GF_TASK_tagTaskFinish* pTaskFinish)
{
	if (NULL != pTaskFinish)
	{
		m_GameFrameDlg.OnEventTaskOnceFinish(pTaskFinish);
	}
}
//新手礼包	Author<cxf>; DateTime<2012-01-16>
void __cdecl CClientKernelSink::OnEventNewGift(CMD_GF_NEW_GIFT* pNewGift)
{
	if (NULL != pNewGift)
	{
		m_GameFrameDlg.OnEventNewGift(pNewGift);
	}
}

void __cdecl CClientKernelSink::OnEventOnlineKeepTime(CMD_GF_ONLINE * pOnline)
{
	if (NULL != pOnline)
	{
		m_GameFrameDlg.OnEventOnlineKeepTime(pOnline);
	}
}
//////////////////////////////////////////////////////////////////////////
