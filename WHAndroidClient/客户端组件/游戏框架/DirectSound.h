/////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1998 by Jˆrg Kˆnig
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

// DirectSound.h: interface for the CDirectSound class.
//
//////////////////////////////////////////////////////////////////////
#if !defined(AFX_DIRECTSOUND_H__A20FE86F_118F_11D2_9AB3_0060B0CDC13E__INCLUDED_)
#define AFX_DIRECTSOUND_H__A20FE86F_118F_11D2_9AB3_0060B0CDC13E__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <mmsystem.h>
#include <dsound.h>
using namespace std;
#pragma comment(lib,"dxguid.lib")
#pragma comment(lib, "dsound.lib")
#pragma comment(lib,"winmm.lib")
#define WINCLASSNAME "winclass1"

/////////////////////////////////////////////////////////////////////////////////////
//#include "..\..\TemplateLibrary\Array.h"
class CMciSound
{
public:
    CMciSound();
    virtual ~CMciSound();
public:
    bool PlaySounds(LPCTSTR lpstrPath);
    bool StopAllSounds();
    bool IsSoundPlaying();
    bool LoadMusic(LPCTSTR lpstrPath, bool bPlay = true);
    bool StopMusic(bool bStopOrReplay = true);
    bool SetSoundVol(BYTE cbVol);
    bool SetMusicVol(BYTE cbVol);
    void CloseAll();
    bool InitVol(BYTE cbVol = 100, BYTE cbBGVol = 100);
public:
    bool                            m_bMusicPlaying;                // -- «∑Ò’˝‘⁄≤•∑≈±≥æ∞“Ù¿÷
    BYTE                            m_cbSoundVol;                   // --∆’Õ®“Ù¡ø
    BYTE                            m_cbMusicVol;                   // --±≥æ∞“Ù¿÷“Ù¡ø
    WORD                            m_wPlyCnt;
protected:
    TCHAR                           m_szMusic[32];                  // --
    CArrayTemplate<WORD>            m_arrAlias;
};
/////////////////////////////////////////////////////////////////////////////////////
class CDirectSound  
{
public:		// construction/destruction
	CDirectSound();
	virtual ~CDirectSound();
	// If the "pWnd" paramter is NULL, then AfxGetApp()->GetMainWnd() will be used.
	BOOL	Create(LPCTSTR pszResource, CWnd * pWnd = 0);
	BOOL	Create(UINT uResourceID, CWnd * pWnd = 0)
    {
        return Create(MAKEINTRESOURCE(uResourceID), pWnd);
    }
	// Alternativly you can specify the sound by yourself
	// Note that the class does not copy the entire data ! Instead
	// a pointer to the given data will be stored !
	// You can load an entire WAV file into memory and then call this
	// Create() method.
	BOOL	Create(LPVOID pSoundData, CWnd * pWnd = 0);
    BOOL    CreateInFile(LPSTR strFilePath, CWnd * pWnd = 0);
public:		// operations
    BOOL			IsValid() const;
    void			Play(DWORD dwStartPosition = 0, BOOL bLoop = FALSE);
    HRESULT         GetStatus(DWORD * pdwStatus);
    void			Stop();
    void			Pause();
    void			Continue();
    CDirectSound &	EnableSound(BOOL bEnable = TRUE)
    {
        m_bEnabled = bEnable;
        if(!bEnable) Stop();
        return * this;
    }
    BOOL			IsEnabled() const { return m_bEnabled; }
    HRESULT SetWavVolume(long lVol = 0/* 0 >= lVol >= -10000 */);
    HRESULT GetWavVolume(long * plVol);
protected:	// implementation
	BOOL SetSoundData(LPVOID pSoundData, DWORD dwSoundSize);
	BOOL CreateSoundBuffer(WAVEFORMATEX * pcmwf);
	BOOL GetWaveData(void * pRes, WAVEFORMATEX * & pWaveHeader, void * & pbWaveData, DWORD & cbWaveSize);
private:	// data member
	LPVOID m_pTheSound;
	DWORD m_dwTheSound;
	LPDIRECTSOUNDBUFFER m_pDsb;
	BOOL m_bEnabled;
	static LPDIRECTSOUND m_lpDirectSound;
	static DWORD m_dwInstances;
};
#endif // !defined(AFX_DIRECTSOUND_H__A20FE86F_118F_11D2_9AB3_0060B0CDC13E__INCLUDED_)