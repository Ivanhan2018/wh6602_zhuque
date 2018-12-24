#pragma once
#include <atlbase.h>     
#include <afxhtml.h>    
class cMisc
{
public:
	static CString DecodeMime(CString s_Text);
	static CString EncodeHtml(BSTR bs_Html);
	static void    EncodeHtml(const WCHAR *u16_In, UINT *pu32_WcharToCopy, char *s8_Out, UINT *pu32_OutSize);
	static CString VarToStr(CComVariant &v_Variant);
	static CString RemoveTag(CString s_Html, CString s_Tag);
	static CString CutString(CString s_In, CString s_Cut, BOOL b_End);
	static void    RegWriteString(HKEY h_Class, CString s_Path, CString s_Key, CString s_Value);
	static void    RegWriteDword (HKEY h_Class, CString s_Path, CString s_Key, DWORD u32_Value);
};



 

class CStreamReader : public IStream, public cMisc
{
public:
	CStreamReader();
	virtual ~CStreamReader();
	void SetData(LPCTSTR szData);
	ULONG STDMETHODCALLTYPE AddRef();
	ULONG STDMETHODCALLTYPE Release(); 
	STDMETHOD(QueryInterface)(REFIID iid, void **ppUnk);
	STDMETHOD(Read)(void *pv_Buffer, ULONG u32_BufSize, ULONG *pu32_ReadBytes);
	STDMETHOD(Write)(const void *pv_Buffer, ULONG u32_BufSize, ULONG *pcbWritten);
	STDMETHOD(Seek)(LARGE_INTEGER, DWORD, ULARGE_INTEGER*);
	STDMETHOD(SetSize)(ULARGE_INTEGER);
	STDMETHOD(CopyTo)(IStream *, ULARGE_INTEGER, ULARGE_INTEGER*, ULARGE_INTEGER*);
	STDMETHOD(Commit)(DWORD);
	STDMETHOD(Revert)();
	STDMETHOD(LockRegion)(ULARGE_INTEGER, ULARGE_INTEGER, DWORD);
	STDMETHOD(UnlockRegion)(ULARGE_INTEGER, ULARGE_INTEGER, DWORD);
	STDMETHOD(Stat)(STATSTG*, DWORD);
	STDMETHOD(Clone)(IStream**);

protected:
	char *mps8_Data;
	UINT  mu32_DataLen;
	UINT  mu32_Index;
};