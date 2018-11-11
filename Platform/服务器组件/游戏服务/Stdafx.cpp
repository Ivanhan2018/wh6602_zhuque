#include "Stdafx.h"
#include <Afxdllx.h>

// {3669456D-645C-4565-8A6B-2F4CE4EC52BD}
extern "C" const GUID CLSID_DefLogMgr =
{ 0x3669456d, 0x645c, 0x4565, { 0x8a, 0x6b, 0x2f, 0x4c, 0xe4, 0xec, 0x52, 0xbd }};

// {D11EB638-4A61-4706-B40E-A0827D27BE63}
extern "C" const GUID IID_ILogMgr = 
{ 0xd11eb638, 0x4a61, 0x4706, { 0xb4, 0xe, 0xa0, 0x82, 0x7d, 0x27, 0xbe, 0x63 }};

//组件头文件
#include "GameServiceExport.h"

//////////////////////////////////////////////////////////////////////////

static AFX_EXTENSION_MODULE GameServiceDLL={NULL,NULL};

//DLL 导出主函数
extern "C" int APIENTRY DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	UNREFERENCED_PARAMETER(lpReserved);
	if (dwReason==DLL_PROCESS_ATTACH)
	{
		if (!AfxInitExtensionModule(GameServiceDLL,hInstance)) return 0;
		new CDynLinkLibrary(GameServiceDLL);
	}
	else if (dwReason==DLL_PROCESS_DETACH)
	{
		AfxTermExtensionModule(GameServiceDLL);
	}
	return 1;
}

//////////////////////////////////////////////////////////////////////////


