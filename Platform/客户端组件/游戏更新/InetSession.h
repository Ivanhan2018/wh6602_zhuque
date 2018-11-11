// InetSession.h: interface for the CInetSession class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INETSESSION_H__5BAA22A7_DBED_45DF_BB65_1B46CB1A38AF__INCLUDED_)
#define AFX_INETSESSION_H__5BAA22A7_DBED_45DF_BB65_1B46CB1A38AF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <afxinet.h>
#define WM_REQUEST_COMPLETE	WM_USER+1001

#define MAXBUFFERSIZE		65535

class CInetSession 
{
public:
    char* GetBuffer();
    DWORD GetLength();

    CInetSession();
    //pstrFilePath 要保存的文件全路径
    CInetSession(LPCTSTR pstrURL,LPCTSTR pstrFilePath = NULL,DWORD dwFlag=INTERNET_FLAG_ASYNC);
    virtual ~CInetSession();
private:
    void Init();
    void Connect(LPCTSTR pstrURL,DWORD dwFlag);
    void ReadBuffer(LPCTSTR pstrFilePath);
public:
    BOOL m_bResult;
private:
    char m_buf[MAXBUFFERSIZE];
    INTERNET_BUFFERSA ib ;    
	CString m_str;
};

#endif // !defined(AFX_INETSESSION_H__5BAA22A7_DBED_45DF_BB65_1B46CB1A38AF__INCLUDED_)
