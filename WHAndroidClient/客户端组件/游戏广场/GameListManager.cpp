#include "StdAfx.h"
#include "GlobalUnits.h"
#include ".\gamelistmanager.h"


#define GAME_LIST_HTML  (TEXT("<html xmlns=\"http://www.w3.org/1999/xhtml\"><head><meta http-equiv=\"Content-Type\" content=\"text/html; charset=GB2312\" /></head>\
								<body background=\"res://SkinResource.dll/ICON_BLANK2.GIF\" scroll=\"no\" topmargin=\"0\" leftmargin=\"0\" rightmargin=\"0\" bottommargin=\"0\" onselectstart=\"return false;\">\
								<table width=\"645\" height=\"191\" border=\"0\" cellpadding=\"0\" cellspacing=\"0\" frame=\"void\" rules=\"none\">\
								<tr>\
								<td><input type=\"image\" src=\"<$ICON_NORMAL_1$>\" onclick=\"window.external.CallRoomList('<$NAME_1$>');\" /></td>\
								<td><input type=\"image\" src=\"<$ICON_NORMAL_2$>\" onclick=\"window.external.CallRoomList('<$NAME_2$>');\" /></td>\
								<td><input type=\"image\" src=\"<$ICON_NORMAL_3$>\" onclick=\"window.external.CallRoomList('<$NAME_3$>');\" /></td>\
								<td><input type=\"image\" src=\"<$ICON_NORMAL_4$>\" onclick=\"window.external.CallRoomList('<$NAME_4$>');\" /></td>\
								</tr>\
								<tr>\
								<td><input type=\"image\" src=\"<$ICON_NORMAL_5$>\" onclick=\"window.external.CallRoomList('<$NAME_5$>');\" /></td>\
								<td><input type=\"image\" src=\"<$ICON_NORMAL_6$>\" onclick=\"window.external.CallRoomList('<$NAME_6$>');\" /></td>\
								<td><input type=\"image\" src=\"<$ICON_NORMAL_7$>\" onclick=\"window.external.CallRoomList('<$NAME_7$>');\" /></td>\
								<td><input type=\"image\" src=\"<$ICON_NORMAL_8$>\" onclick=\"window.external.CallRoomList('<$NAME_8$>');\" /></td>\
								</tr>\
								<tr>\
								<td><input type=\"image\" src=\"<$ICON_NORMAL_9$>\" onclick=\"window.external.CallRoomList('<$NAME_9$>');\" /></td>\
								<td><input type=\"image\" src=\"<$ICON_NORMAL_10$>\" onclick=\"window.external.CallRoomList('<$NAME_10$>');\" /></td>\
								<td><input type=\"image\" src=\"<$ICON_NORMAL_11$>\" onclick=\"window.external.CallRoomList('<$NAME_11$>');\" /></td>\
								<td><input type=\"image\" src=\"<$ICON_NORMAL_12$>\" onclick=\"window.external.CallRoomList('<$NAME_12$>');\" /></td>\
								</tr>\
								</table>\
								</body></html>"))

CGameListManager::CGameListManager(void):m_strHtml(GAME_LIST_HTML)//GB2312 utf-8
{
}

CGameListManager::~CGameListManager(void)
{
}

bool CGameListManager::Init()
{
	TCHAR *path =new TCHAR[MAX_PATH];
	ZeroMemory(path,MAX_PATH);
	GetModuleFileName(NULL,path,MAX_PATH);
	CString strPath;
	strPath.Append(path);
	int pos = strPath.ReverseFind(L'\\');
	strPath = strPath.Left(pos);
	strPath.Append("\\Games\\");
	
	CFileFind ff;
	BOOL bFound;
	bFound = ff.FindFile(strPath+L"*.dll");

	while (bFound)//读取所有dll的信息
	{
		bFound   =   ff.FindNextFile();   
		CString   sFilePath   =   ff.GetFilePath();
		CGameInfo gameInfo;
		gameInfo.GetFromFile(sFilePath); 
		m_arrayGameInfoCollection.Add(gameInfo);  
	}
	ff.Close();
	delete path;
	return TRUE;
}
 
CString CGameListManager::GetGameListHtml()
{
	CString result = m_strHtml;
	int nCount = m_arrayGameInfoCollection.GetCount();
	INT_PTR nIndex=0;
	CListServer * pListServer=NULL;
	int iCountIndex = 0;
	//先设置激活的
	for (int i=0; i<12; i++)
	{
		pListServer=NULL;
		CString tag(""),data(""),linktag(""),linkdata("");
		tag.Format("<$ICON_NORMAL_%d$>",iCountIndex+1);
		linktag.Format("<$NAME_%d$>",iCountIndex+1);
		if (i<nCount)
		{	
			int iHaveNums = 0;
			pListServer=NULL;
			nIndex = 0;
			do
			{ 
				pListServer=g_GlobalUnits.m_ServerListManager.EnumServerItem(nIndex++);
				if (NULL == pListServer)
				{
					break;
				}
				else if((NULL!=pListServer)&&(pListServer->m_pListKind->m_GameKind.wKindID==m_arrayGameInfoCollection[i].m_wKindID) )
				{
					++iHaveNums;
					break;
				}
			}while(true);
			if (iHaveNums > 0)
			{//可激活
				data = "res://Games\\"+m_arrayGameInfoCollection[i].m_strName+".dll/ICON_NORMAL.GIF";
				linkdata.Format("%d",m_arrayGameInfoCollection[i].m_wKindID);
				result.Replace(tag,data);
				result.Replace(linktag,linkdata);
				++iCountIndex;
			}
		}
	}

	//再设置不激活的
	for (int i=0; i<12; i++)
	{
		pListServer=NULL;
		CString tag(""),data(""),linktag(""),linkdata("");
		tag.Format("<$ICON_NORMAL_%d$>",iCountIndex+1);
		linktag.Format("<$NAME_%d$>",iCountIndex+1);
		if (i<nCount)
		{	
			int iHaveNums = 0;
			pListServer=NULL;
			nIndex = 0;
			do
			{ 
				pListServer=g_GlobalUnits.m_ServerListManager.EnumServerItem(nIndex++);
				if (NULL == pListServer)
				{
					break;
				}
				else if((NULL!=pListServer)&&(pListServer->m_pListKind->m_GameKind.wKindID==m_arrayGameInfoCollection[i].m_wKindID) )
				{
					++iHaveNums;
					break;
				}
			}while(true);
			if (0==iHaveNums)
			{//可激活
				data = "res://Games\\"+m_arrayGameInfoCollection[i].m_strName+".dll/ICON_NORMAL_N.GIF";
				linkdata.Format("%d",m_arrayGameInfoCollection[i].m_wKindID);
				result.Replace(tag,data);
				result.Replace(linktag,linkdata);
				++iCountIndex;
			}
		}
	}

	//设置后面的背景图
	for (int k=iCountIndex; k<12; k++)
	{
		CString tag(""),data(""),linktag(""),linkdata("");
		tag.Format("<$ICON_NORMAL_%d$>",k+1);
		linktag.Format("<$NAME_%d$>",k+1);

#ifdef _DEBUG	
		data ="res://SkinResourceD.dll/ICON_BLANK.GIF";
#else
		data ="res://SkinResource.dll/ICON_BLANK.GIF";
#endif
		linkdata="";
		result.Replace(tag,data);
		result.Replace(linktag,linkdata);
	}



//
//
//	//原版
//	for (int i=0;i<=12;i++)//最多能容纳12纳游戏.
//	{
//		CString tag ,data,linktag,linkdata;
//		tag.Format("<$ICON_NORMAL_%d$>",i+1);
//		linktag.Format("<$NAME_%d$>",i+1);
//
//		if (i<nCount)
//		{	
//			int iHaveNums = 0;
//			pListServer=NULL;
//			nIndex = 0;
//			do
//			{ 
//				pListServer=g_GlobalUnits.m_ServerListManager.EnumServerItem(nIndex++);
//				if (NULL == pListServer)
//				{
//					break;
//				}
//				else if((NULL!=pListServer)&&(pListServer->m_pListKind->m_GameKind.wKindID==m_arrayGameInfoCollection[i].m_wKindID) )
//				{
//					++iHaveNums;
//					break;
//				}
//			}while(true);
//			if (iHaveNums > 0)
//			{//可激活
//				data = "res://Games\\"+m_arrayGameInfoCollection[i].m_strName+".dll/ICON_NORMAL.GIF";
//			}
//			else
//			{//不可激活
//				data = "res://Games\\"+m_arrayGameInfoCollection[i].m_strName+".dll/ICON_NORMAL_N.GIF";
//			}
//			
//			linkdata.Format("%d",m_arrayGameInfoCollection[i].m_wKindID);
//		}
//		else
//		{
//#ifdef _DEBUG	
//			data ="res://SkinResourceD.dll/ICON_BLANK.GIF";
//#else
//			data ="res://SkinResource.dll/ICON_BLANK.GIF";
//#endif
//			linkdata="";
//		}
//		result.Replace(tag,data);
//		result.Replace(linktag,linkdata);
//		
//	}	 
	
	return result;	 
}

//房间列表//暂无添资源
CString CGameListManager::GetGameRoomHtml()
{
/* 2011-4-1 yy 注销，已改成程序实现
	CString result = m_strHtml;

	int nCount = m_arrayGameRoomInfo.GetCount();
	WORD wSortID=0;

	for(BYTE j=0;j<m_arrayGameInfoCollection.GetCount();j++)
	{
		if(m_arrayGameRoomInfo.GetCount() == 0 || m_arrayGameInfoCollection.GetCount() == 0)break;

		//房间列表
		if(m_arrayGameInfoCollection[j].m_wKindID == m_arrayGameRoomInfo[0].GetKindID())
		{
			for(BYTE k=0;k<nCount;k++)
			{
				//载入资源
				WORD str=m_arrayGameRoomInfo[k].GetSortId();
				if(wSortID == m_arrayGameRoomInfo[k].GetSortId())
				{
					continue;
				}

				CString tag ,data,linktag,linkdata;

				//房间资源
				tag . Format("<$ICON_NORMAL_%d$>",m_arrayGameRoomInfo[k].GetSortId());
				linktag.Format("<$NAME_%d$>",m_arrayGameRoomInfo[k].GetSortId());

				wSortID=m_arrayGameRoomInfo[0].GetSortId();

				data.Format("res://Games\\"+m_arrayGameInfoCollection[j].m_strName+".dll/ICON_ROOM%d.GIF",m_arrayGameRoomInfo[k].GetSortId()-1);
				linkdata.Format("%d",m_arrayGameInfoCollection[j].m_wKindID);

				result.Replace(tag,data);
				result.Replace(linktag,linkdata);
			}
			CString tag ,data,linktag,linkdata;
			tag . Format("<$ICON_HALL$>");
			linktag.Format("<$NAME_%d$>");

			wSortID=m_arrayGameRoomInfo[0].GetSortId();

			data = "res://Games\\"+m_arrayGameInfoCollection[j].m_strName+".dll/ICON_HALL.GIF";
			linkdata.Format("%d",m_arrayGameInfoCollection[j].m_wKindID);

			result.Replace(tag,data);
			result.Replace(linktag,linkdata);
		}
	}
	return result;	 
*/

	return CString("");
}

void CGameListManager::SetHtmlRoom()
{
	/*
			  
			  	m_strHtml=TEXT("<html xmlns=\"http://www.w3.org/1999/xhtml\">\
			  				<head><meta http-equiv=\"Content-Type\" content=\"text/html; charset=GB2312\" /></head>\
			  				<body background=\"res://SkinResource.dll/ICON_BLANK2.GIF\" scroll=\"no\" topmargin=\"0\" leftmargin=\"5\" rightmargin=\"0\" bottommargin=\"0\" onselectstart=\"return false;\">\
			  				<table width=\"404\" height=\"191\" border=\"0\" cellpadding=\"0\" cellspacing=\"0\">\
			  					<tr>\
			  						<td align=center><img type=\"image\" src=\"<$ICON_HALL$>\" /></td>\
			  						<tr>\
			  						<td width=\"404\" height=\"191\">\
			  						<table width=\"100%\" border=\"0\" cellpadding=\"10\" cellspacing=\"0\">\
			  							<tr>\
			  								<td><input type=\"image\" src=\"<$ICON_NORMAL_1$>\" onclick=\"window.external.CallTableList('1');\"/></td>\
			  								<td><input type=\"image\" src=\"<$ICON_NORMAL_2$>\" onclick=\"window.external.CallTableList('2');\" /></td>\
			  								</tr>\
			  								<td><input type=\"image\" src=\"<$ICON_NORMAL_3$>\" onclick=\"window.external.CallTableList('3');\" /></td>\
			  								<td><input type=\"image\" src=\"<$ICON_NORMAL_4$>\" onclick=\"window.external.CallTableList('4');\" /></td>\
			  								</tr>\
			  								<td><input type=\"image\" src=\"<$ICON_NORMAL_5$>\" onclick=\"window.external.CallTableList('5');\" /></td>\
			  								<td><input type=\"image\" src=\"<$ICON_NORMAL_6$>\" onclick=\"window.external.CallTableList('6');\" /></td>\
			  							</tr>\
			  						</table>\
			  						</td>\
			  					</tr>\
			  				</table>\
			  			  </body>\
			  			  </html>");*/
			  

}
void CGameListManager::SetHtmlPlazz()
{
	m_strHtml=GAME_LIST_HTML;
}