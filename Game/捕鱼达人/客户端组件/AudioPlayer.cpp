#include "stdafx.h"
#include "AudioPlayer.h"
#include "Digitalv.h"

////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CAudioPlayer, CWnd)
	ON_MESSAGE(MM_MCINOTIFY, OnMCINotify) 
END_MESSAGE_MAP()

//¹¹Ôìº¯Êý
CAudioPlayer::CAudioPlayer()
{
	//ÉèÖÃ±äÁ¿
	m_nDeviceType=0;
	m_szAudioName[0]=0;
}

//Îö¹¹º¯Êý
CAudioPlayer::~CAudioPlayer()
{
	//¹Ø±ÕÓïÒô
	try 
	{
	if(0!=m_nDeviceType) Close();
	}
	catch (...){}
}

//²¥·ÅÓïÒô
bool CAudioPlayer::Play(LPCTSTR pszFileName, bool bLoop)
{
	//¹Ø±ÕÓïÒô
	if(0!=m_nDeviceType) Close();

	//ÉèÖÃ±äÁ¿
	MCI_OPEN_PARMS pmOpen={0};
	pmOpen.dwCallback=0;
	pmOpen.lpstrAlias=NULL;
	pmOpen.lpstrDeviceType=TEXT("MPEGAudio");
	pmOpen.lpstrElementName=pszFileName;
	pmOpen.wDeviceID=NULL;

	//´ò¿ªÓïÒô
	//TCHAR szError[256];
	MCIERROR mciResult=0;
	try
	{
	mciResult=mciSendCommand(NULL,MCI_OPEN,MCI_OPEN_ELEMENT,(DWORD_PTR)&pmOpen);
	if(0==mciResult)
	{
		m_nDeviceType=pmOpen.wDeviceID;
	}
	else
	{
//		mciGetErrorString(mciResult,szError,CountArray(szError));
//#ifdef _DEBUG
//
//		m_pGameFrameDlg->InsertSystemString(szError+CString(TEXT("open")));
//#endif
	}
	}
	catch (...)
	{
//#ifdef _DEBUG
//		m_pGameFrameDlg->InsertSystemString(TEXT("2"));
//#endif
		return false;
	}

	//ºÏ·¨ÅÐ¶Ï
	if (0==m_nDeviceType) 
	{
//#ifdef _DEBUG
//		m_pGameFrameDlg->InsertSystemString(TEXT("1"));
//#endif
		return false;
	}
	
	//ÉèÖÃ±äÁ¿
	MCI_PLAY_PARMS pmPlay={0};
	pmPlay.dwCallback=MAKEWPARAM(GetSafeHwnd(),0);
	pmPlay.dwFrom=0;
	pmPlay.dwTo=0;

	//²¥·ÅÓïÒô
	try
	{
		//Ñ­»·ÅÐ¶Ï
		if (true==bLoop) mciResult=mciSendCommand(m_nDeviceType,MCI_PLAY,MCI_NOTIFY|MCI_DGV_PLAY_REPEAT,(DWORD_PTR)&pmPlay);
		else mciResult=mciSendCommand(m_nDeviceType,MCI_PLAY,MCI_NOTIFY,(DWORD_PTR)&pmPlay);
	}
	catch (...)
	{
//#ifdef _DEBUG
//		m_pGameFrameDlg->InsertSystemString(TEXT("0"));
//#endif
		return false;
	}

//#ifdef _DEBUG
//	if (0!=mciResult)
//	{
//		mciGetErrorString(mciResult,szError,CountArray(szError));
//		m_pGameFrameDlg->InsertSystemString(szError+CString(TEXT("play")));
//	}
//#endif

	//±£ÁôÃû³Æ
	lstrcpyn(m_szAudioName,pszFileName,CountArray(m_szAudioName));

	return mciResult==0?true:false;
}

//ÖØ²¥ÓïÒô
bool CAudioPlayer::RePlay()
{
	//ÉèÖÃ±äÁ¿
	MCI_PLAY_PARMS pmPlay={0};
	pmPlay.dwCallback=MAKEWPARAM(GetSafeHwnd(),0);
	pmPlay.dwFrom=0;
	pmPlay.dwTo=0;

	//²¥·ÅÓïÒô
	try
	{
		mciSendCommand(m_nDeviceType,MCI_PLAY,MCI_NOTIFY,(DWORD_PTR)&pmPlay);
	}
	catch (...)
	{
//#ifdef _DEBUG
//		m_pGameFrameDlg->InsertSystemString(TEXT("RePlay"));
//#endif
		return false;
	}

	return true;
}

//¹Ø±ÕÓïÒô
bool CAudioPlayer::Close()
{
	try
	{
	if(0!=m_nDeviceType)
	{
		MCI_GENERIC_PARMS gp={0};
		gp.dwCallback=NULL;
		mciSendCommand(m_nDeviceType,MCI_CLOSE,0,(DWORD_PTR)&gp);
		m_nDeviceType=0;
	}
	}
	catch (...)
	{
		return false;
	}

	return true;
}

//ÔÝÍ£ÓïÒô
bool CAudioPlayer::Pause()
{
	try
	{
	if(0!=m_nDeviceType)
	{
		MCI_GENERIC_PARMS gp={0};
		gp.dwCallback=NULL;
		mciSendCommand(m_nDeviceType,MCI_PAUSE,0,(DWORD_PTR)&gp);
	}
	}
	catch (...)
	{
		return false;
	}

	return true;
}

//ÖØÖÃÓïÒô
bool CAudioPlayer::Resume()
{
	try
	{
	if(0!=m_nDeviceType)
	{
		MCI_GENERIC_PARMS gp={0};
		gp.dwCallback=NULL;
		mciSendCommand(m_nDeviceType,MCI_RESUME,0,(DWORD_PTR)&gp);
	}
	}
	catch (...)
	{
		return false;
	}

	return true;
}

//Í£Ö¹ÓïÒô
bool CAudioPlayer::Stop()
{
	try
	{
	if(0!=m_nDeviceType)
	{
		MCI_GENERIC_PARMS gp={0};
		gp.dwCallback=NULL;
		mciSendCommand(m_nDeviceType,MCI_STOP,0,(DWORD_PTR)&gp);
	}
	}
	catch (...)
	{
		return false;
	}

	return true;
}

//ÓïÒô»Øµ÷
LRESULT CAudioPlayer::OnMCINotify(WPARAM wParam, LPARAM lParam)
{
	//¹Ø±ÕÅÐ¶Ï
	try
	{
		//¹Ø±ÕÓïÒô
		if(0!=m_nDeviceType) Close();
				
	/*	if (MCI_NOTIFY_SUCCESSFUL==wParam)
		{*/
			
//#ifdef _DEBUG
//			m_pGameFrameDlg->InsertSystemString(TEXT("MCI_NOTIFY_SUCCESSFUL"));
//#endif
		//}
//#ifdef _DEBUG
//		else if (MCI_NOTIFY_ABORTED==wParam)
//		{
//			m_pGameFrameDlg->InsertSystemString(TEXT("MCI_NOTIFY_ABORTED"));
//		}
//		else if (MCI_NOTIFY_FAILURE==wParam)
//		{
//			m_pGameFrameDlg->InsertSystemString(TEXT("MCI_NOTIFY_FAILURE"));
//		}
//		else if (MCI_NOTIFY_SUPERSEDED==wParam)
//		{
//			m_pGameFrameDlg->InsertSystemString(TEXT("MCI_NOTIFY_SUPERSEDED"));
//		}
//#endif

	}
	catch (...)
	{

	}

	return 0;
}
