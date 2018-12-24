#include "StdAfx.h"
#include "StreamReader.h"

// ###########################  MISC  #################################

void cMisc::RegWriteString(HKEY h_Class, CString s_Path, CString s_Key, CString s_Value)
{
	CRegKey i_Reg;
	i_Reg.Create  (h_Class, s_Path);   // closed in destructor of CRegKey
	i_Reg.SetValue(s_Value, s_Key);
}

void cMisc::RegWriteDword(HKEY h_Class, CString s_Path, CString s_Key, DWORD u32_Value)
{
	CRegKey i_Reg;
	i_Reg.Create  (h_Class,   s_Path);  // closed in destructor of CRegKey
	i_Reg.SetValue(u32_Value, s_Key);
}

// Replaces MIME encoded strings
// "%20" --> " "
// "%FC" --> "?
// "%25" --> "%"     etc....
CString cMisc::DecodeMime(CString s_In)
{
	CString s_Out;
	TCHAR *pt_Out = (TCHAR*) s_Out.GetBuffer(s_In.GetLength());
	TCHAR *pt_In  = (TCHAR*) s_In. GetBuffer(0);

	int O=0;
	for (int I=0; I<s_In.GetLength(); I++)
	{
		TCHAR t_Chr = pt_In[I];

		if (t_Chr == '%') 
		{
			UINT u32_Hex = 0;

			// convert hexadecimal string into UINT ("FA" --> 0xFA)
			for (UINT D=1; D<=2; D++)
			{
				UINT u32_Digit = (UINT) pt_In[I+D];

				if (u32_Digit >= (UINT)'0' && u32_Digit <= (UINT)'9') u32_Digit -=  (UINT) '0';
				else if (u32_Digit >= (UINT)'A' && u32_Digit <= (UINT)'F') u32_Digit -= ((UINT) 'A' -10);
				else if (u32_Digit >= (UINT)'a' && u32_Digit <= (UINT)'f') u32_Digit -= ((UINT) 'a' -10);
				else break;

				u32_Hex = u32_Hex * 16 + u32_Digit;

				if (D == 2) // second hex digit
				{
					t_Chr = (TCHAR) u32_Hex;
					I += 2; // skip two hex digits
				}
			}
		}

		pt_Out[O++] = t_Chr;
	}

	s_Out.ReleaseBuffer(O);
	return s_Out;
}


// converts Unicode to ANSI using HTML Hex character encoding --> "&#xABCD;"
// If compiled with _UNICODE compiler setting, no encoding is required -> just copy string
// bs_Html is freed here at the end
CString cMisc::EncodeHtml(BSTR bs_Html)
{
	CString s_Out;
	UINT u32_Len = SysStringLen(bs_Html);
	if (!u32_Len)
		return _T("");

	UINT u32_BufSize = u32_Len * 8 +1;
	LPTSTR t_Buf     = s_Out.GetBuffer(u32_BufSize); // for worst case

#ifdef _UNICODE
	wcscpy(t_Buf, bs_Html);
#else // _MBCS
	// EncodeHtml() modifies s32_Len AND u32_BufSize !!
	EncodeHtml(bs_Html, &u32_Len, t_Buf, &u32_BufSize);
	u32_Len = u32_BufSize; // count of chars copied to t_Buf
#endif

	SysFreeString(bs_Html);
	s_Out.ReleaseBuffer(u32_Len);
	return s_Out;
}

// converts pu32_WcharToCopy WCHAR's in the Unicode string u16_In which are 
// outside the ASCII code range to ANSI s8_Out using HTML Hex character encoding --> "&#xABCD;"
// s8_Out must have the 8 fold size of u32_WcharToCopy to cover the worst case !!
// As s8_Out will be zero terminated, the minimum buffer size is 9 characters
// If the buffer is too small no conversion takes place
// e.g. The Greek letter Unicode 960 is converted to "&#x03C0;\0"
void cMisc::EncodeHtml(const WCHAR *u16_In,     // IN=buffer with UNICODE Html code
									UINT  *pu32_WcharToCopy, // IN=Wchars to copy, OUT=Converted Wchars
									char  *s8_Out,           // IN=buffer to receive ANSI Html code
									UINT  *pu32_OutSize)     // IN=sizeof(s8_Out), OUT=Bytes copied to s8_Out
{
	// Minimum required buffer size is 9 characters !! (for "&#x03C0;\0")
	// In the worst case 9 MBCS chars are required to convert ONE Unicode character !!
	ASSERT(*pu32_OutSize > 8); 

	UINT Pos = 0;
	UINT i;
	for (i=0; i<*pu32_WcharToCopy && Pos+8<*pu32_OutSize; i++)
	{
		WCHAR Chr = u16_In[i];
		if (Chr == 0) // End of Wstring
			break;

		if (Chr <= '~') // no conversion required
		{
			s8_Out[Pos] = (char) Chr;
			Pos ++;
		}
		else // encode HTML
		{
			sprintf(s8_Out +Pos, "&#x%04X;", Chr);
			Pos += 8;
		}
	}

	s8_Out[Pos] = 0; // terminate string

	*pu32_WcharToCopy = i;
	*pu32_OutSize     = Pos;
}

// removes a tag at the begin and end of a Html code
// RemoveTag("<DIV align=right><b>Text</b></DIV>", "Div")   will return "<b>Text</b>"
// See also cHtmlDomNode::Strip() and cHtmlDomNode::Remove()
CString cMisc::RemoveTag(CString s_Html, CString s_Tag)
{
	s_Html.TrimLeft (_T(" \t\r\n"));
	s_Html.TrimRight(_T(" \t\r\n"));

	s_Tag.MakeUpper();
	CString s_Start = _T("<")  + s_Tag;
	CString s_End   = _T("</") + s_Tag + _T(">");

	if (s_Html.Left (s_Start.GetLength()).CompareNoCase(s_Start) != 0 ||
		s_Html.Right(s_End.  GetLength()).CompareNoCase(s_End)   != 0)
		return s_Html;

	UINT s32_Begin = s_Html.Find(_T(">"), s_Start.GetLength()) +1;
	UINT s32_Count = s_Html.GetLength() - s_End.GetLength() - s32_Begin;

	return s_Html.Mid(s32_Begin, s32_Count);
}


// Removes the string s_Cut once from the beginning (b_End=FALSE) or end (b_End=TRUE) of s_In
// E.g. s_In="Test&nbsp;&nbsp;", s_Cut="&nbsp;", b_End=TRUE --> returns "Test&nbsp;"
CString cMisc::CutString(CString s_In, CString s_Cut, BOOL b_End)
{
	int s32_InLen  = s_In. GetLength();
	int s32_CutLen = s_Cut.GetLength();

	if (s32_InLen < s32_CutLen || s32_InLen == 0 || s32_CutLen == 0)
		return s_In;

	if (b_End)
	{
		if (s_Cut.CompareNoCase(s_In.Right(s32_CutLen)) == 0)
			return s_In.Mid(0, s32_InLen - s32_CutLen);
	}
	else
	{
		if (s_Cut.CompareNoCase(s_In.Left(s32_CutLen)) == 0)
			return s_In.Mid(s32_CutLen);
	}

	return s_In;
}

// required for Visual Studio 6 and bugfix for Visual Studio 7 not converting VT_NULL correctly
// This function is ONLY used to convert strings like "TABLE" or "#FF5500" from BSTR to CString
// ATTENTION !!!
// For country dependant conversions of special characters (like öäüß or Greek, Russian) 
// use EncodeHtml() instead !!!!
CString cMisc::VarToStr(CComVariant &v_Variant)
{
	CString s_Result;

	if (v_Variant.vt == VT_I4)
	{
		s_Result.Format(_T("%d"), v_Variant.intVal);
	}
	else if (v_Variant.vt == VT_BOOL)
	{
		s_Result.Format(_T("%d"), v_Variant.boolVal);
	}
	else if (v_Variant.vt == VT_BSTR) 
	{
		if (!v_Variant.bstrVal)
			return _T("");

		UINT u32_Len = SysStringLen(v_Variant.bstrVal);
		if (!u32_Len)
			return _T("");

		LPTSTR t_Buf = s_Result.GetBuffer(u32_Len);

#ifdef _UNICODE // Unicode --> Unicode
		wcscpy(t_Buf, v_Variant.bstrVal);
#else // _MBCS     Unicode --> Ansi
		WideCharToMultiByte(CP_ACP, 0, v_Variant.bstrVal, u32_Len, t_Buf, u32_Len, 0, 0);
#endif

		s_Result.ReleaseBuffer(u32_Len);
	}

	return s_Result;
}


// ##########################  STREAMREADER  ###############################

// cStreamReader is required for cHtmlDocument::SetHtml()
// The other methods except Read are not required so they are not implemented here
// (MFC7 uses CStreamOnCString whose implementation is BUGGY !)

CStreamReader::CStreamReader()
{
	mps8_Data    = 0;
	mu32_DataLen = 0;
	mu32_Index   = 0;
}

CStreamReader::~CStreamReader()
{
	if (mps8_Data) delete mps8_Data;
}

// Sets a MBCS or Unicode string
void CStreamReader::SetData(LPCTSTR szData)
{
	UINT u32_CharsToCopy = _tcsclen(szData);

	if (mps8_Data) delete mps8_Data;

	// Make mps8_Data ALWAYS contain an ANSI string even in _UNICODE version !
#ifdef _UNICODE

	mu32_DataLen = u32_CharsToCopy * 8; // for worst case: 1 Unicode char -> "&#xABCD;"
	mps8_Data    = new char[mu32_DataLen];

	// EncodeHtml() modifies mu32_DataLen to the new MBCS length!
	EncodeHtml(szData, &u32_CharsToCopy, mps8_Data, &mu32_DataLen);

#else // _MBCS

	mu32_DataLen = u32_CharsToCopy;
	mps8_Data    = new char[mu32_DataLen];

	memcpy(mps8_Data, szData, mu32_DataLen);

#endif

	mu32_Index = 0;
}

// Read is used for cHtmlDocument::SetHtml()
// Read is called by Mshtml.dll
// returns the ANSI string data which must have been set with cStreamReader::SetData() 
// to the caller which may request any number of bytes from the stream
HRESULT CStreamReader::Read(void    *pv_Buffer,    // IN  the buffer that receives the stream data
										 ULONG   u32_BufSize,   // IN  the number of bytes requested (=buffer size)
										 ULONG *pu32_ReadBytes) // OUT the number of bytes which have been copied to pv_Buffer
{
	if (pu32_ReadBytes) *pu32_ReadBytes = 0;

	if (!pv_Buffer)
		return E_POINTER;

	((char*)pv_Buffer)[0] = 0;

	char *s8_Start = mps8_Data + mu32_Index;
	char *s8_End   = mps8_Data + mu32_DataLen;

	int s32_CharsLeft  = s8_End - s8_Start;
	if (s32_CharsLeft <= 0)
		return S_FALSE; // no more data to read

	int s32_CharsToCopy  = min(s32_CharsLeft, (int)u32_BufSize);
	if (s32_CharsToCopy <= 0)
	{
		mu32_Index = 0;
		return S_FALSE;
	}

	memcpy(pv_Buffer, s8_Start, s32_CharsToCopy);

	if (pu32_ReadBytes) *pu32_ReadBytes = s32_CharsToCopy;

	mu32_Index += s32_CharsToCopy;

	return S_OK;
}

HRESULT CStreamReader::QueryInterface(REFIID iid, void **ppUnk)
{
	return E_NOINTERFACE;
}

ULONG CStreamReader::AddRef() 
{
	return 1;
}

ULONG CStreamReader::Release() 
{
	return 1;
}

HRESULT CStreamReader::Seek(LARGE_INTEGER, DWORD, ULARGE_INTEGER*)
{
	return E_NOTIMPL;
}

HRESULT CStreamReader::SetSize(ULARGE_INTEGER)
{
	return E_NOTIMPL;
}

HRESULT CStreamReader::CopyTo(IStream*, ULARGE_INTEGER, ULARGE_INTEGER*, ULARGE_INTEGER*)
{
	return E_NOTIMPL;
}

HRESULT CStreamReader::Commit(DWORD)
{
	return E_NOTIMPL;
}

HRESULT CStreamReader::Revert()
{
	return E_NOTIMPL;
}

HRESULT CStreamReader::LockRegion(ULARGE_INTEGER, ULARGE_INTEGER, DWORD)
{
	return E_NOTIMPL;
}

HRESULT CStreamReader::UnlockRegion(ULARGE_INTEGER, ULARGE_INTEGER, DWORD)
{
	return E_NOTIMPL;
}

HRESULT CStreamReader::Stat(STATSTG*, DWORD)
{
	return E_NOTIMPL;
}

HRESULT CStreamReader::Clone(IStream**)
{
	return E_NOTIMPL;
}

// Write is not required as this can be done much easyier see cHtmlDocument::GetHtml()
HRESULT CStreamReader::Write(const void *pv_Buffer, ULONG u32_BufSize, ULONG *pcbWritten)
{
	return E_NOTIMPL;
}

