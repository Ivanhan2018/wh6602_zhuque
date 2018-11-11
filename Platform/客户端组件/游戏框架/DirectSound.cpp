/////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1998 by J鰎g K鰊ig
// All rights reserved
//
// This file is part of the completely free tetris clone "CGTetris".
//
// This is free software.
// You may redistribute it by any means providing it is not sold for profit
// without the authors written consent.
//
// No warrantee of any kind, expressed or implied, is included with this
// software; use at your own risk, responsibility for damages (if any) to
// anyone resulting from the use of this software rests entirely with the
// user.
//
// Send bug reports, bug fixes, enhancements, requests, flames, etc., and
// I'll try to keep a version up to date.  I can be reached as follows:
//    J.Koenig@adg.de                 (company site)
//    Joerg.Koenig@rhein-neckar.de    (private site)
/////////////////////////////////////////////////////////////////////////////

// DirectSound.cpp: implementation of the CDirectSound class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DirectSound.h"
#include "GameFrame.h"
// The following macro is defined since DirectX 5, but will work with
// older versions too.
#ifndef DSBLOCK_ENTIREBUFFER
	#define DSBLOCK_ENTIREBUFFER        0x00000002
#endif

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

static void DSError( HRESULT hRes ) {
	switch(hRes) {
		case DS_OK: TRACE0("NO ERROR\n"); break;
		case DSERR_ALLOCATED: TRACE0("ALLOCATED\n"); break;
		case DSERR_INVALIDPARAM: TRACE0("INVALIDPARAM\n"); break;
		case DSERR_OUTOFMEMORY: TRACE0("OUTOFMEMORY\n"); break;
		case DSERR_UNSUPPORTED: TRACE0("UNSUPPORTED\n"); break;
		case DSERR_NOAGGREGATION: TRACE0("NOAGGREGATION\n"); break;
		case DSERR_UNINITIALIZED: TRACE0("UNINITIALIZED\n"); break;
		case DSERR_BADFORMAT: TRACE0("BADFORMAT\n"); break;
		case DSERR_ALREADYINITIALIZED: TRACE0("ALREADYINITIALIZED\n"); break;
		case DSERR_BUFFERLOST: TRACE0("BUFFERLOST\n"); break;
		case DSERR_CONTROLUNAVAIL: TRACE0("CONTROLUNAVAIL\n"); break;
		case DSERR_GENERIC: TRACE0("GENERIC\n"); break;
		case DSERR_INVALIDCALL: TRACE0("INVALIDCALL\n"); break;
		case DSERR_OTHERAPPHASPRIO: TRACE0("OTHERAPPHASPRIO\n"); break;
		case DSERR_PRIOLEVELNEEDED: TRACE0("PRIOLEVELNEEDED\n"); break;
		default: TRACE1("%lu\n",hRes);break;
	}
}
#define BG_MUSIC                        TEXT("BG_Music")
#ifdef _DEBUG
void static CPYMSG_(CString str,HWND hWnd = NULL){ if(NULL==hWnd) hWnd=AfxGetMainWnd()->GetSafeHwnd();COPYDATASTRUCT cpyData={0};cpyData.dwData=((WORD)hWnd);
            cpyData.cbData=str.GetLength();cpyData.lpData=str.GetBuffer();SendMessage(hWnd,WM_COPYDATA,((WPARAM)hWnd),(LPARAM)&cpyData);return; }
#else
#define CPYMSG_
#endif
CMciSound::CMciSound()
{
    m_bMusicPlaying = false;
    m_wPlyCnt = 0;
    m_cbSoundVol = 100;
    m_cbMusicVol = 100;
}
CMciSound::~CMciSound()
{
}
bool CMciSound::InitVol(BYTE cbVol, BYTE cbBGVol)
{
    m_cbSoundVol =((100 >= cbVol)? cbVol : 100);
    m_cbMusicVol =((100 >= cbBGVol)? cbBGVol : 100);
    return true;
}
bool CMciSound::PlaySounds(LPCTSTR lpstrPath)
{
    MCIERROR mciError;
    TCHAR szSndstr[256] = {0};
    WORD wCnt =(WORD)m_arrAlias.GetCount();
    if (0 < m_arrAlias.GetCount())
    {   // --关闭已播放完的设备释放资源
        for (WORD i = 0;i < m_arrAlias.GetCount();)
        {
            TCHAR tchBuffer[256];
			::memset(tchBuffer, 0, sizeof(tchBuffer));
            _snprintf(szSndstr, sizeof(szSndstr), TEXT("status \"%04x\" mode"), m_arrAlias.GetAt(0));
            mciError = mciSendString(szSndstr, tchBuffer, sizeof(tchBuffer), NULL);
            if (mciError)
            {
                TCHAR szErrMsg[128];
                mciGetErrorString(mciError,szErrMsg,128);
                CPYMSG_(szSndstr, AfxGetMainWnd()->GetSafeHwnd());
                CPYMSG_(szErrMsg, AfxGetMainWnd()->GetSafeHwnd());
                return false;
            }
            if (0 == lstrcmp(tchBuffer, TEXT("stopped")))
            {
                _snprintf(szSndstr, sizeof(szSndstr), TEXT("stop \"%04x\""), m_arrAlias.GetAt(0));
                mciError = mciSendString(szSndstr, NULL, 0, NULL);
                if (mciError)
                {
                    TCHAR szErrMsg[128];
                    mciGetErrorString(mciError,szErrMsg,128);
                    CPYMSG_(szSndstr, AfxGetMainWnd()->GetSafeHwnd());
                    CPYMSG_(szErrMsg, AfxGetMainWnd()->GetSafeHwnd());
                    return false;
                }
                _snprintf(szSndstr, sizeof(szSndstr), TEXT("close \"%04x\""), m_arrAlias.GetAt(0));
                mciError = mciSendString(szSndstr, NULL, 0, NULL);
                if (mciError)
                {
                    TCHAR szErrMsg[128];
                    mciGetErrorString(mciError,szErrMsg,128);
                    CPYMSG_(szSndstr, AfxGetMainWnd()->GetSafeHwnd());
                    CPYMSG_(szErrMsg, AfxGetMainWnd()->GetSafeHwnd());
                    return false;
                }
                m_arrAlias.RemoveAt(0);
                continue;
            }
            i++;
        }
    }
    if (256 <= lstrlen(lpstrPath))
    {
        ASSERT(0);
        return false;
    }
    CString strMP3 = lpstrPath;
    strMP3 = strMP3.Mid(strMP3.ReverseFind('.')+1);
    strMP3.MakeLower();
    if (TEXT("mp3")!= strMP3)
    {
        ASSERT(0);
        return false;
    }
    // --OPEN
    _snprintf(szSndstr, sizeof(szSndstr), TEXT("open \"%s\" alias %04x"), lpstrPath, m_wPlyCnt);
	mciError = mciSendString(szSndstr, NULL, 0, NULL);
    if(mciError)
    {
        TCHAR szErrMsg[128];
        mciGetErrorString(mciError,szErrMsg,128);
        CPYMSG_(szSndstr, AfxGetMainWnd()->GetSafeHwnd());
        CPYMSG_(szErrMsg, AfxGetMainWnd()->GetSafeHwnd());
        return false;
    }
    m_arrAlias.Add(m_wPlyCnt++);
    // --设置音量
    _snprintf(szSndstr, sizeof(szSndstr), TEXT("setaudio \"%04x\" volume to %d0"), m_wPlyCnt-1, m_cbSoundVol);
	mciError = mciSendString(szSndstr, NULL, 0, NULL);
    if (mciError)
    {
        TCHAR szErrMsg[128];
        mciGetErrorString(mciError,szErrMsg,128);
        CPYMSG_(szSndstr, AfxGetMainWnd()->GetSafeHwnd());
        CPYMSG_(szErrMsg, AfxGetMainWnd()->GetSafeHwnd());
        return false;
    }
    // --播放声音文件
    _snprintf(szSndstr, sizeof(szSndstr), TEXT("play \"%04x\" from 0"), m_wPlyCnt-1);
	mciError = mciSendString(szSndstr, NULL, 0, NULL);
    if (mciError)
    {
        TCHAR szErrMsg[128];
        mciGetErrorString(mciError,szErrMsg,128);
        CPYMSG_(szSndstr, AfxGetMainWnd()->GetSafeHwnd());
        CPYMSG_(szErrMsg, AfxGetMainWnd()->GetSafeHwnd());
        return false;
    }
    
    return true;
}
bool CMciSound::StopAllSounds()
{
    TCHAR szSndstr[256] = {0};
    TCHAR szAlias[8] = {0};
    while (m_arrAlias.GetCount())
    {
        _snprintf(szAlias, sizeof(szAlias), TEXT("%04x"), m_arrAlias.GetAt(0));
        if (mciGetDeviceID(szAlias))
        {
            _snprintf(szSndstr, sizeof(szSndstr), TEXT("stop %04x"), m_arrAlias.GetAt(0));
            MCIERROR mciError = mciSendString(szSndstr, NULL, 0, NULL);
            if (mciError)
            {
                TCHAR szErrMsg[128];
                mciGetErrorString(mciError,szErrMsg,128);
                CPYMSG_(szSndstr, AfxGetMainWnd()->GetSafeHwnd());
                CPYMSG_(szErrMsg, AfxGetMainWnd()->GetSafeHwnd());
                return false;
            }
            _snprintf(szSndstr, sizeof(szSndstr), TEXT("close %04x"), m_arrAlias.GetAt(0));
            mciError = mciSendString(szSndstr, NULL, 0, NULL);
            if (mciError)
            {
                TCHAR szErrMsg[128];
                mciGetErrorString(mciError,szErrMsg,128);
                CPYMSG_(szSndstr, AfxGetMainWnd()->GetSafeHwnd());
                CPYMSG_(szErrMsg, AfxGetMainWnd()->GetSafeHwnd());
                return false;
            }
        }
        m_arrAlias.RemoveAt(0);
    }
    return true;
}
bool CMciSound::IsSoundPlaying()
{
    if (m_arrAlias.GetCount())
    {
        TCHAR szSndstr[256] = {0};
        for (WORD i = 0;i < m_arrAlias.GetCount();)
        {
            TCHAR tchBuffer[256];
			::memset(tchBuffer, 0, sizeof(tchBuffer));
            _snprintf(szSndstr, sizeof(szSndstr), TEXT("status \"%04x\" mode"), m_arrAlias.GetAt(0));
            MCIERROR mciError = mciSendString(szSndstr, tchBuffer, sizeof(tchBuffer), NULL);
            if (mciError)
            {
                TCHAR szErrMsg[128];
                mciGetErrorString(mciError,szErrMsg,128);
                CPYMSG_(szSndstr, AfxGetMainWnd()->GetSafeHwnd());
                CPYMSG_(szErrMsg, AfxGetMainWnd()->GetSafeHwnd());
                return false;
            }
            if (0 == lstrcmp(tchBuffer, TEXT("stopped")))
            {
                _snprintf(szSndstr, sizeof(szSndstr), TEXT("stop \"%04x\""), m_arrAlias.GetAt(0));
                mciError = mciSendString(szSndstr, NULL, 0, NULL);
                if (mciError)
                {
                    TCHAR szErrMsg[128];
                    mciGetErrorString(mciError,szErrMsg,128);
                    CPYMSG_(szSndstr, AfxGetMainWnd()->GetSafeHwnd());
                    CPYMSG_(szErrMsg, AfxGetMainWnd()->GetSafeHwnd());
                    return false;
                }
                _snprintf(szSndstr, sizeof(szSndstr), TEXT("close \"%04x\""), m_arrAlias.GetAt(0));
                mciError = mciSendString(szSndstr, NULL, 0, NULL);
                if (mciError)
                {
                    TCHAR szErrMsg[128];
                    mciGetErrorString(mciError,szErrMsg,128);
                    CPYMSG_(szSndstr, AfxGetMainWnd()->GetSafeHwnd());
                    CPYMSG_(szErrMsg, AfxGetMainWnd()->GetSafeHwnd());
                    return false;
                }
                m_arrAlias.RemoveAt(0);
                continue;
            }
            i++;
        }
        if (m_arrAlias.GetCount()) return true;
    }
    return false;
}
bool CMciSound::LoadMusic(LPCTSTR lpstrPath, bool bPlay)
{
    if (256 <= lstrlen(lpstrPath))
    {
        ASSERT(0);
        return false;
    }
	MCIERROR mciError;
    TCHAR szSndstr[256] = {0};
    if (mciGetDeviceID(BG_MUSIC))
    {
        _snprintf(szSndstr, sizeof(szSndstr), TEXT("stop \"%s\""), BG_MUSIC);
        mciError = mciSendString(szSndstr, NULL, 0, NULL);
        if (mciError)
        {
            TCHAR szErrMsg[128];
            mciGetErrorString(mciError,szErrMsg,128);
            CPYMSG_(szSndstr, AfxGetMainWnd()->GetSafeHwnd());
            CPYMSG_(szErrMsg, AfxGetMainWnd()->GetSafeHwnd());
            return false;
        }
        _snprintf(szSndstr, sizeof(szSndstr), TEXT("close \"%s\""), BG_MUSIC);
        mciError = mciSendString(szSndstr, NULL, 0, NULL);
        if (mciError)
        {
            TCHAR szErrMsg[128];
            mciGetErrorString(mciError,szErrMsg,128);
            CPYMSG_(szSndstr, AfxGetMainWnd()->GetSafeHwnd());
            CPYMSG_(szErrMsg, AfxGetMainWnd()->GetSafeHwnd());
            return false;
        }
    }
    CString strMP3 = lpstrPath;
    strMP3 = strMP3.Mid(strMP3.ReverseFind('.')+1);
    strMP3.MakeLower();
    if (TEXT("mp3")!= strMP3)
    {
        ASSERT(0);
        return false;
    }
    // --OPEN
    _snprintf(szSndstr, sizeof(szSndstr), TEXT("open \"%s\" alias %s"), lpstrPath, BG_MUSIC);
	mciError = mciSendString(szSndstr, NULL, 0, NULL);
    if(mciError)
    {
        TCHAR szErrMsg[128];
        mciGetErrorString(mciError,szErrMsg,128);
        CPYMSG_(szSndstr, AfxGetMainWnd()->GetSafeHwnd());
        CPYMSG_(szErrMsg, AfxGetMainWnd()->GetSafeHwnd());
        return false;
    }
    // --设置音量
    _snprintf(szSndstr, sizeof(szSndstr), TEXT("setaudio \"%s\" volume to %d0"), BG_MUSIC, m_cbSoundVol);
	mciError = mciSendString(szSndstr, NULL, 0, NULL);
    if (mciError)
    {
        TCHAR szErrMsg[128];
        mciGetErrorString(mciError,szErrMsg,128);
        CPYMSG_(szSndstr, AfxGetMainWnd()->GetSafeHwnd());
        CPYMSG_(szErrMsg, AfxGetMainWnd()->GetSafeHwnd());
        return false;
    }
    if (bPlay)
    {
        // --播放声音文件
        _snprintf(szSndstr, sizeof(szSndstr), TEXT("play \"%s\" from 0 repeat"), BG_MUSIC);
        mciError = mciSendString(szSndstr, NULL, 0, NULL);
        if (mciError)
        {
            TCHAR szErrMsg[128];
            mciGetErrorString(mciError,szErrMsg,128);
            CPYMSG_(szSndstr, AfxGetMainWnd()->GetSafeHwnd());
            CPYMSG_(szErrMsg, AfxGetMainWnd()->GetSafeHwnd());
            return false;
        }
        m_bMusicPlaying = true;
    }
    return true;
}
bool CMciSound::StopMusic(bool bStopOrReplay)
{
    TCHAR szSndstr[256] = {0};
    if (mciGetDeviceID(BG_MUSIC))
    {
        if (!bStopOrReplay)
        {
            _snprintf(szSndstr, sizeof(szSndstr), TEXT("stop \"%s\""), BG_MUSIC);
            MCIERROR mciError = mciSendString(szSndstr, NULL, 0, NULL);
            if (mciError)
            {
                TCHAR szErrMsg[128];
                mciGetErrorString(mciError,szErrMsg,128);
                CPYMSG_(szSndstr, AfxGetMainWnd()->GetSafeHwnd());
                CPYMSG_(szErrMsg, AfxGetMainWnd()->GetSafeHwnd());
                return false;
            }
        }
        else
        {
            _snprintf(szSndstr, sizeof(szSndstr), TEXT("play \"%s\" from 0 repeat"), BG_MUSIC);
            MCIERROR mciError = mciSendString(szSndstr, NULL, 0, NULL);
            if (mciError)
            {
                TCHAR szErrMsg[128];
                mciGetErrorString(mciError,szErrMsg,128);
                CPYMSG_(szSndstr, AfxGetMainWnd()->GetSafeHwnd());
                CPYMSG_(szErrMsg, AfxGetMainWnd()->GetSafeHwnd());
                return false;
            }
			m_bMusicPlaying = true;
        }
    }
    return true;
}
// --设置音量
bool CMciSound::SetSoundVol(BYTE cbVol)
{
    ASSERT(100 >= cbVol);
    m_cbSoundVol =((100 >= cbVol)? cbVol : 100);
    WORD wSoundCnt =(WORD)m_arrAlias.GetCount();
    if (wSoundCnt)
    {
        for (BYTE i = 0;i < wSoundCnt;i++)
        {
            TCHAR szSndstr[256] = {0};
            _snprintf(szSndstr, sizeof(szSndstr), TEXT("setaudio \"%04x\" volume to %d0"), m_arrAlias.GetAt(i), m_cbSoundVol);
            MCIERROR mciError = mciSendString(szSndstr, NULL, 0, NULL);
            if (mciError)
            {
                TCHAR szErrMsg[128];
                mciGetErrorString(mciError,szErrMsg,128);
                AfxMessageBox(szErrMsg);
                return false;
            }
        }
    }
    return true;
}
bool CMciSound::SetMusicVol(BYTE cbVol)
{
    ASSERT(100 >= cbVol);
    m_cbMusicVol =((100 >= cbVol)? cbVol : 100);
    TCHAR szSndstr[256] = {0};
    if (mciGetDeviceID(BG_MUSIC))
    {
        _snprintf(szSndstr, sizeof(szSndstr), TEXT("setaudio \"%s\" volume to %d0"), BG_MUSIC, m_cbMusicVol);
        MCIERROR mciError = mciSendString(szSndstr, NULL, 0, NULL);
        if (mciError)
        {
            TCHAR szErrMsg[128];
            mciGetErrorString(mciError,szErrMsg,128);
            CPYMSG_(szSndstr, AfxGetMainWnd()->GetSafeHwnd());
            CPYMSG_(szErrMsg, AfxGetMainWnd()->GetSafeHwnd());
            return false;
        }
    }
    return true;
}
void CMciSound::CloseAll()
{
    m_arrAlias.RemoveAll();
    try
    {
        MCIERROR mciError = mciSendString("stop all", NULL, 0, NULL);
        if (mciError)
        {
            TCHAR szErrMsg[128];
            mciGetErrorString(mciError,szErrMsg,128);
            CPYMSG_(TEXT("stop all"), AfxGetMainWnd()->GetSafeHwnd());
            CPYMSG_(szErrMsg, AfxGetMainWnd()->GetSafeHwnd());
            return;
        }
        mciError = mciSendString("close all", NULL, 0, NULL);
        if (mciError)
        {
            TCHAR szErrMsg[128];
            mciGetErrorString(mciError,szErrMsg,128);
            CPYMSG_(TEXT("close all"), AfxGetMainWnd()->GetSafeHwnd());
            CPYMSG_(szErrMsg, AfxGetMainWnd()->GetSafeHwnd());
            return;
        }
    }
    catch (...)
    {
        ASSERT(0);
    }
    m_bMusicPlaying = false;
    m_wPlyCnt = 0;
}



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
LPDIRECTSOUND CDirectSound::m_lpDirectSound;
DWORD CDirectSound::m_dwInstances;

CDirectSound::CDirectSound()
{
	m_lpDirectSound = 0;
	m_pDsb          = 0;
	m_pTheSound     = 0;
	m_dwTheSound    = 0;
	m_bEnabled      = TRUE;
	m_dwInstances++;
}
CDirectSound::~CDirectSound()
{
	if( m_pDsb ) m_pDsb->Release();

	if( !--m_dwInstances && m_lpDirectSound )
    {
		m_lpDirectSound->Release();
		m_lpDirectSound = 0;
	}
}
BOOL CDirectSound::Create(LPCTSTR pszResource, CWnd * pWnd)
{
	//////////////////////////////////////////////////////////////////
	// load resource
	HINSTANCE hApp = ::GetModuleHandle(0);
	ASSERT(hApp);
    HRSRC hResInfo = ::FindResource(hApp, pszResource, TEXT("WAVE"));
	if(hResInfo == 0) return FALSE;
	HGLOBAL hRes = ::LoadResource(hApp, hResInfo);
	if(hRes == 0) return FALSE;
	LPVOID pTheSound = ::LockResource(hRes);
	if(pTheSound == 0) return FALSE;
	return Create(pTheSound, pWnd);
}
BOOL CDirectSound :: Create(LPVOID pSoundData, CWnd * pWnd)
{
	if(pWnd == 0) pWnd = AfxGetApp()->GetMainWnd();
	ASSERT(pWnd != 0);
	ASSERT(::IsWindow(pWnd->GetSafeHwnd()));
	ASSERT(pSoundData != 0);
	//////////////////////////////////////////////////////////////////
	// create direct sound object
	if( m_lpDirectSound == 0 )
    {
		// Someone might use sounds for starting apps. This may cause
		// DirectSoundCreate() to fail because the driver is used by
		// anyone else. So wait a little before starting with the work ...
		HRESULT hRes = DS_OK;
		short nRes = 0;
		do
        {
			if( nRes ) ::Sleep(500);
			hRes = ::DirectSoundCreate(0, &m_lpDirectSound, 0);
			++nRes;
		} while( nRes < 10 && (hRes == DSERR_ALLOCATED || hRes == DSERR_NODRIVER) );
		if( hRes != DS_OK ) return FALSE;
		m_lpDirectSound->SetCooperativeLevel(pWnd->GetSafeHwnd(), DSSCL_NORMAL);
	}
	ASSERT(m_lpDirectSound != 0);
	WAVEFORMATEX * pcmwf;
	if( ! GetWaveData(pSoundData, pcmwf, m_pTheSound, m_dwTheSound) ||
		! CreateSoundBuffer(pcmwf) ||
		! SetSoundData(m_pTheSound, m_dwTheSound) ) return FALSE;
	return TRUE;
}
// --
BOOL CDirectSound::CreateInFile(LPSTR strFilePath, CWnd * pWnd)
{
	if(NULL == pWnd) pWnd = AfxGetApp()->GetMainWnd();
	ASSERT(NULL != pWnd);
	ASSERT(::IsWindow(pWnd->GetSafeHwnd()));
    DSBUFFERDESC dsbd;
    WAVEFORMATEX wfmx;
    UCHAR * sndBuffer = NULL;
    ::memset(&dsbd, 0, sizeof (dsbd));
    ::memset(&wfmx, 0, sizeof (wfmx));
	if(0 == m_lpDirectSound)
    {
		HRESULT hRes = DS_OK;
		short nRes = 0;
		do
        {
			if(nRes) ::Sleep(500);
			hRes = DirectSoundCreate(0, &m_lpDirectSound, 0);
			nRes++;
		} while((10 > nRes)&&((DSERR_ALLOCATED)== hRes)||(DSERR_NODRIVER == hRes));
		if (DS_OK != hRes) return FALSE;
		if (DS_OK != m_lpDirectSound->SetCooperativeLevel(pWnd->GetSafeHwnd(), DSSCL_NORMAL)) return FALSE;
        ASSERT(0 != m_lpDirectSound);
	}
    HMMIO hmmio;
    MMCKINFO mmckriff, mmckIn;
    PCMWAVEFORMAT pwfm;
    ::memset(&mmckriff, 0, sizeof (MMCKINFO));
    if(NULL ==(hmmio = mmioOpen(strFilePath, NULL, MMIO_READ | MMIO_ALLOCBUF))) return FALSE;
    //进入块(chunk),查找 riff和wave的标记
    if(0 != mmioDescend(hmmio, &mmckriff, NULL, 0))
    {
        mmioClose(hmmio, 0);
        return FALSE;
    }
    if((FOURCC_RIFF != mmckriff.ckid)||(mmioFOURCC('W', 'A', 'V', 'E')!= mmckriff.fccType))
    {
        mmioClose(hmmio, 0);
        return FALSE;
    }
    //查找 fmt 块
    mmckIn.ckid = mmioFOURCC('f', 'm', 't', ' ');
    if(0 != mmioDescend(hmmio, &mmckIn, &mmckriff, MMIO_FINDCHUNK))
    {
        mmioClose(hmmio, 0);
        return FALSE;
    }
    //fmt块的格式与PCMWAVEFORMAT 的格式定义相同,所以读入临时变量，最后写入 wmtf中
    if(mmioRead(hmmio,(HPSTR)&pwfm, sizeof (PCMWAVEFORMAT))!= sizeof (PCMWAVEFORMAT))
    {
        mmioClose(hmmio, 0);
        return FALSE;
    }
    //检测是不是 pc的wave标准格式
    if(WAVE_FORMAT_PCM != pwfm.wf.wFormatTag)
    {
        mmioClose(hmmio,0);
        return FALSE;
    }
    //给wmfx赋值
	::memcpy(&wfmx, &pwfm, sizeof (pwfm));

    wfmx.cbSize =0;
    if(0 != mmioAscend(hmmio,&mmckIn,0))
    {
        mmioClose(hmmio,0);
        return FALSE;
    }
    //查找 data chunk
    mmckIn.ckid = mmioFOURCC('d', 'a', 't', 'a');
    if(0 !=mmioDescend(hmmio,&mmckIn,&mmckriff,MMIO_FINDCHUNK))
    {
        mmioClose(hmmio,0);
        return FALSE;
    }
    m_dwTheSound = mmckIn.cksize;
    sndBuffer = new UCHAR[mmckIn.cksize];
    //数据写入sndBuffer
    mmioRead(hmmio,(HPSTR)sndBuffer, mmckIn.cksize);
    mmioClose(hmmio,0);
    //建立directsound的辅助缓存
    dsbd.dwSize = sizeof(DSBUFFERDESC);
    dsbd.dwBufferBytes = mmckIn.cksize;
    dsbd.dwFlags = DSBCAPS_CTRLFREQUENCY | DSBCAPS_CTRLPAN | DSBCAPS_CTRLVOLUME | DSBCAPS_STATIC | DSBCAPS_GLOBALFOCUS;
    dsbd.lpwfxFormat = &wfmx;
    if(FAILED(m_lpDirectSound->CreateSoundBuffer(&dsbd, &m_pDsb, NULL)))
    {
        delete [] sndBuffer;
        return FALSE;
    }
    VOID * pDSLockedBuffer = NULL;
    DWORD dwDSLockedBufferSize = 0;
    //lock 住内存，现在他是可操作的，拷贝进去即可
    if(m_pDsb->Lock(0, mmckIn.cksize, &pDSLockedBuffer, &dwDSLockedBufferSize, NULL, NULL, 0L)) return FALSE;
    ::memcpy(pDSLockedBuffer, sndBuffer, mmckIn.cksize);
    m_pTheSound = pDSLockedBuffer;
    //Unlock 缓冲区内存
    if(FAILED(m_pDsb->Unlock(pDSLockedBuffer, dwDSLockedBufferSize, NULL, 0)))
    {
        delete [] sndBuffer;
        return FALSE;
    }
    return TRUE;
}

BOOL CDirectSound :: GetWaveData(void * pRes, WAVEFORMATEX * & pWaveHeader, void * & pbWaveData, DWORD & cbWaveSize)
{
	pWaveHeader = 0;
	pbWaveData = 0;
	cbWaveSize = 0;
	DWORD * pdw = (DWORD *)pRes;
	DWORD dwRiff = *pdw++;
	DWORD dwLength = *pdw++;
	DWORD dwType = *pdw++;
	if(dwRiff != FOURCC_RIFF) return FALSE;      // not even RIFF
	if(mmioFOURCC('W', 'A', 'V', 'E')!= dwType) return FALSE;      // not a WAV
	DWORD * pdwEnd = (DWORD *)((BYTE *)pdw + dwLength-4);
    while( pdw < pdwEnd )
    {
        dwType = *pdw++;
        dwLength = *pdw++;
        switch( dwType )
        {
        case mmioFOURCC('f', 'm', 't', ' '):
            if( !pWaveHeader )
            {
                if( dwLength < sizeof(WAVEFORMAT) ) return FALSE;      // not a WAV
                pWaveHeader = (WAVEFORMATEX *)pdw;
                if( pbWaveData && cbWaveSize ) return TRUE;
            }
            break;
        case mmioFOURCC('d', 'a', 't', 'a'):
            pbWaveData = LPVOID(pdw);
            cbWaveSize = dwLength;
            if( pWaveHeader ) return TRUE;
            break;
        }
        pdw = (DWORD *)((BYTE *)pdw + ((dwLength+1)&~1));
    }
	return FALSE;
}

BOOL CDirectSound::CreateSoundBuffer(WAVEFORMATEX * pcmwf)
{
	DSBUFFERDESC dsbdesc;
	// Set up DSBUFFERDESC structure.
	::memset(&dsbdesc, 0, sizeof(DSBUFFERDESC)); // Zero it out.
	dsbdesc.dwSize = sizeof(DSBUFFERDESC);
	// Need no controls (pan, volume, frequency).
	dsbdesc.dwFlags = DSBCAPS_CTRLFREQUENCY | DSBCAPS_CTRLPAN | DSBCAPS_CTRLVOLUME | DSBCAPS_GLOBALFOCUS | DSBCAPS_STATIC;		// assumes that the sound is played often
	dsbdesc.dwBufferBytes = m_dwTheSound;
	dsbdesc.lpwfxFormat = pcmwf;    // Create buffer.
	HRESULT hRes;
	if( DS_OK != (hRes = m_lpDirectSound->CreateSoundBuffer(&dsbdesc, &m_pDsb, 0)) )
    {
		DSError(hRes);
		m_pDsb = 0;
		return FALSE;
	}
	return TRUE;
}


BOOL CDirectSound::SetSoundData(void * pSoundData, DWORD dwSoundSize)
{
	LPVOID lpvPtr1;
	DWORD dwBytes1;
	// Obtain write pointer.
	HRESULT hr = m_pDsb->Lock(0, 0, &lpvPtr1, &dwBytes1, 0, 0, DSBLOCK_ENTIREBUFFER);    
    // If DSERR_BUFFERLOST is returned, restore and retry lock.
	if(DSERR_BUFFERLOST == hr)
    {
		m_pDsb->Restore();
		hr = m_pDsb->Lock(0, 0, &lpvPtr1, &dwBytes1, 0, 0, DSBLOCK_ENTIREBUFFER);
	}
	if(DS_OK == hr)
    {
		// Write to pointers.
		::CopyMemory(lpvPtr1, pSoundData, dwBytes1);
		// Release the data back to DirectSound.
		hr = m_pDsb->Unlock(lpvPtr1, dwBytes1, 0, 0);
		if(DS_OK == hr) return TRUE;
	}
	// Lock, Unlock, or Restore failed.
	return FALSE;
}

void CDirectSound::Play(DWORD dwStartPosition, BOOL bLoop)
{
	if( ! IsValid() || ! IsEnabled() ) return;
	if( dwStartPosition > m_dwTheSound ) dwStartPosition = m_dwTheSound;
	m_pDsb->SetCurrentPosition(dwStartPosition);
	if( DSERR_BUFFERLOST == m_pDsb->Play(0, 0, bLoop ? DSBPLAY_LOOPING : 0) )
    {
		SetSoundData(m_pTheSound, m_dwTheSound);
		// Try playing again
		m_pDsb->Play(0, 0, bLoop ? DSBPLAY_LOOPING : 0);
	}
}
// --返回当前播放状态
HRESULT CDirectSound::GetStatus(DWORD * pdwStatus)
{
    return m_pDsb->GetStatus(pdwStatus);
}
void CDirectSound::Stop()
{
	if( IsValid() ) m_pDsb->Stop();
}

void CDirectSound::Pause()
{
	Stop();
}

void CDirectSound::Continue()
{
	if( IsValid() )
    {
		DWORD dwPlayCursor, dwWriteCursor;
		m_pDsb->GetCurrentPosition(&dwPlayCursor, &dwWriteCursor);
		Play(dwPlayCursor);
	}
}

BOOL CDirectSound::IsValid() const
{
	return (m_lpDirectSound && m_pDsb && m_pTheSound && m_dwTheSound) ? TRUE : FALSE;
}
HRESULT CDirectSound::SetWavVolume(long lVol)
{
    if (!IsValid()) return 1;
    if (0 < lVol) lVol = 0;
    if (-10000 > lVol) lVol = -10000;
    if(IsValid()) return m_pDsb->SetVolume(lVol);
    return 1;
}
HRESULT CDirectSound::GetWavVolume(long * plVol)
{
    if(IsValid()) return m_pDsb->GetVolume(plVol);
    return 1;
}