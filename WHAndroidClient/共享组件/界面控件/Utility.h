#pragma once
#include "SkinControls.h"

class SKIN_CONTROL_CLASS CUtility
{
public:
	static CString GetModulePath(HMODULE hModule = NULL);

	static BOOL IsFileExist(LPCTSTR lpszFilePath);

	static BOOL ExtractResourceToFile( LPCTSTR lpszType
									, UINT nResID
									, LPCTSTR lpszFilename
									, HMODULE hModule
									);

	static Image * LoadImage( UINT nID, LPCTSTR lpszType, HINSTANCE hInstance = NULL);
};
