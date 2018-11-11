// InetSession.cpp: implementation of the CInetSession class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "InetSession.h"

#ifdef _AFXDLL
#pragma comment(lib, "wininet.lib")
#endif
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


HANDLE hRequestCompleteEvent = NULL;
HANDLE hConnectedEvent = NULL;
HANDLE hRequestOpenedEvent = NULL;

HINTERNET hConnect = NULL;
HINTERNET hRequest = NULL;
HINTERNET hInternet = NULL;
HINTERNET hHttpFile = NULL;

DWORD dwLength = 0;
DWORD dwError = 0;
BOOL bAllDone = FALSE;

void AFXAPI OnStatusCallback(HINTERNET hInternet, DWORD dwContext, DWORD dwInternetStatus, LPVOID lpvStatusInformation, DWORD dwStatusInformationLength)
{
    AFX_MANAGE_STATE( AfxGetAppModuleState( ) );
    switch (dwContext)
    {
    case 1:// Connection handle
        if (dwInternetStatus == INTERNET_STATUS_HANDLE_CREATED)
        {
            INTERNET_ASYNC_RESULT *pRes = (INTERNET_ASYNC_RESULT *)lpvStatusInformation;
            hConnect = (HINTERNET)pRes->dwResult;
            SetEvent(hConnectedEvent);
        }
        break;
    case 2:// Request handle
        switch(dwInternetStatus)
        {
        case INTERNET_STATUS_RESPONSE_RECEIVED:
            //Successfully received a response from the server. The lpvStatusInformation parameter is NULL.
            {
                DWORD *dwBytesReceived = (DWORD*)lpvStatusInformation;
                if (*dwBytesReceived == 0) 
                {
                    bAllDone = TRUE;
                }
            }
            break;
        case INTERNET_STATUS_CLOSING_CONNECTION:
            break;
        case INTERNET_STATUS_CONNECTION_CLOSED:
            //Successfully closed the connection to the server. The lpvStatusInformation parameter is NULL. 
            break;
        case INTERNET_STATUS_HANDLE_CREATED:
            //Used by the Win32 API function InternetConnect to indicate that it has created the new handle. This lets the application call the Win32 function InternetCloseHandle from another thread if the connect is taking too long. See the ActiveX SDK for more information about these functions. 
            {
                INTERNET_ASYNC_RESULT *pRes = (INTERNET_ASYNC_RESULT *)lpvStatusInformation;
                hRequest = (HINTERNET)pRes->dwResult;
                SetEvent(hRequestOpenedEvent);
            }
            break;
        case INTERNET_STATUS_REQUEST_COMPLETE :
            {
                //Used by the Win32 API function InternetConnect to indicate that 
                //it has created the new handle. 
                //This lets the application call the Win32 function InternetCloseHandle from another thread if the connect is taking too long. See the ActiveX SDK for more information about these functions. 
                INTERNET_ASYNC_RESULT* res = (INTERNET_ASYNC_RESULT*)lpvStatusInformation;
                HINTERNET hInet = (HINTERNET)(res->dwResult);
                DWORD dwError = res->dwError;
                if(dwError != 0)
                {
                    TRACE1("网络错误码：%d\r\n", dwError);
                    //CString str;
                    //str.Format(L"网络错误码：%d", dwError);
                    //MessageBox(NULL,str,L"",MB_ICONERROR | MB_OK);
                }    
                SetEvent(hRequestCompleteEvent);
            }
            break;
        default:
            break;
        }
        break;
    default:
        break;
    }
}

void CInetSession::Init()
{
    m_bResult = FALSE;
    m_str = "";

    memset(m_buf,0,sizeof(m_buf));
    memset(&ib,0,sizeof(INTERNET_BUFFERSA));
    ib.dwStructSize=sizeof(INTERNET_BUFFERSA);
    ib.lpvBuffer=m_buf;
    ib.dwBufferLength=sizeof(m_buf)-1;
}

CInetSession::CInetSession()
{
    Init();
}

void CInetSession::Connect(LPCTSTR pstrURL,DWORD dwFlag)
{
    hRequestCompleteEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    hConnectedEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    hRequestOpenedEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

    hInternet=InternetOpen("",INTERNET_OPEN_TYPE_PRECONFIG,NULL,NULL,dwFlag);
    if(!hInternet) //有些手机用上面的参数访问不了Internet
        hInternet=InternetOpen("",INTERNET_OPEN_TYPE_DIRECT,NULL,NULL,dwFlag);
    InternetSetStatusCallback(hInternet, (INTERNET_STATUS_CALLBACK)&OnStatusCallback);          

    if(!pstrURL)
        return;
    CString strUrl(pstrURL);
    if(strUrl.GetLength() <=7)
        return;
    strUrl = strUrl.Mid(7,strUrl.GetLength()-7); //只去除掉第一个http://

    CString lpszServer = "";
    CString lpszUrl = "";
    int nPos = strUrl.Find("/");
    if(nPos == -1)
        return;
    lpszServer = strUrl.Left(nPos);
    lpszUrl = strUrl.Mid(nPos+1, strUrl.GetLength()-nPos-1);
    hConnect = InternetConnect(hInternet, lpszServer,INTERNET_DEFAULT_HTTP_PORT,NULL,NULL,INTERNET_SERVICE_HTTP,0,1); // Connection handle's Context
    if (hConnect == NULL)                   
    {
        dwError = GetLastError();
        if (dwError != ERROR_IO_PENDING)
        {
            TRACE1("HttpSendRequest GetLastError:%d\r\n",dwError);
            return;
        }

        DWORD retval = WaitForSingleObject(hConnectedEvent, 20000);
        if(WAIT_TIMEOUT == retval)
        {
            dwError = GetLastError();
            TRACE1("WaitForSingleObject GetLastError:%d\r\n",dwError);
            //MessageBox(NULL,L"Erro",L"",MB_ICONERROR | MB_OK);
            return;
        }
    }

    // Open the request
    hRequest = HttpOpenRequest(hConnect, "GET", lpszUrl,NULL,NULL,NULL,INTERNET_FLAG_RELOAD | INTERNET_FLAG_NO_CACHE_WRITE,2);  // Request handle's context 
    if (hRequest == NULL)
    {
        if (GetLastError() != ERROR_IO_PENDING)
        {
            dwError = GetLastError();
            TRACE1("HttpOpenRequest GetLastError:%d\r\n",dwError);
            return;
        }
        DWORD retval = WaitForSingleObject(hRequestOpenedEvent, 20000);
        if(WAIT_TIMEOUT == retval)
        {
            dwError = GetLastError();
            TRACE1("WaitForSingleObject GetLastError:%d\r\n",dwError);
            //MessageBox(NULL,L"Erro",L"",MB_ICONERROR | MB_OK);
            return;
        }
    }

    if (!HttpSendRequest(hRequest, NULL, 0, NULL,0))
    {
        dwError = GetLastError();
        if (dwError != ERROR_IO_PENDING)
        {
            TRACE1("HttpSendRequest GetLastError:%d\r\n",dwError);
        }
    }
    DWORD retval = WaitForSingleObject(hRequestCompleteEvent, 20000);
    if(WAIT_TIMEOUT == retval)
    {
        dwError = GetLastError();
        TRACE1("WaitForSingleObject GetLastError:%d\r\n",dwError);
        //MessageBox(NULL,L"Erro",L"",MB_ICONERROR | MB_OK);
        return;
    }
}

void CInetSession::ReadBuffer(LPCTSTR pstrFilePath)
{
    if(pstrFilePath == NULL)  //没有指定将buffer保存到文件
    {
        do
        {
            INTERNET_BUFFERSA InetBuff;
            FillMemory(&InetBuff, sizeof(InetBuff), 0);
            FillMemory(&m_buf, sizeof(m_buf), 0);
            InetBuff.dwStructSize = sizeof(InetBuff);
            InetBuff.lpvBuffer = m_buf;
            InetBuff.dwBufferLength = sizeof(m_buf) - 1;

            if (!InternetReadFileExA(hRequest,&InetBuff,IRF_ASYNC,2))
            {
                int n = GetLastError();
                if (GetLastError() == ERROR_IO_PENDING)  
                {
                    DWORD retval = WaitForSingleObject(hRequestCompleteEvent, 20000);
                    if(WAIT_TIMEOUT == retval)
                    {
                        dwError = GetLastError();
                        //MessageBox(NULL,L"Erro",L"",MB_ICONERROR | MB_OK);
                        return;
                    }
                }
                else
                {
                    break;
                }
            }
            dwLength += InetBuff.dwBufferLength;
            m_str += m_buf; 
            if (InetBuff.dwBufferLength == 0) 
                bAllDone = TRUE;

        } while (bAllDone == FALSE);
    }
    else
    {
        CFile file;
        if( (file.Open(pstrFilePath, CFile::modeCreate |CFile::modeWrite)) != NULL )
        {
            do
            {
                INTERNET_BUFFERSA InetBuff;
                FillMemory(&InetBuff, sizeof(InetBuff), 0);
                FillMemory(&m_buf, sizeof(m_buf), 0);
                InetBuff.dwStructSize = sizeof(InetBuff);
                InetBuff.lpvBuffer = m_buf;
                InetBuff.dwBufferLength = sizeof(m_buf) - 1;

                if (!InternetReadFileExA(hRequest,&InetBuff,IRF_ASYNC,2))
                {
                    int n = GetLastError();
                    if (GetLastError() == ERROR_IO_PENDING)  
                    {
                        DWORD retval = WaitForSingleObject(hRequestCompleteEvent, 20000);
                        if(WAIT_TIMEOUT == retval)
                        {
                            dwError = GetLastError();
                            //MessageBox(NULL,L"Error",L"",MB_ICONERROR | MB_OK);
                            return;
                        }
                    }
                    else
                    {
                        break;
                    }
                }
                file.Write(m_buf, InetBuff.dwBufferLength);
                file.Flush();

                dwLength += InetBuff.dwBufferLength;
                m_str += m_buf; 
                if (InetBuff.dwBufferLength == 0) 
                    bAllDone = TRUE;

            } while (bAllDone == FALSE);
            file.Close();
        }
    }
    if(m_str.GetAllocLength() == 0)
        m_bResult = FALSE;
   else
        m_bResult = TRUE;
}

CInetSession::CInetSession(LPCTSTR pstrURL,LPCTSTR pstrFilePath,DWORD dwFlag)
{
	CString strBuffer(pstrURL);
	int iFindIndex = strBuffer.FindOneOf(_T("http://"));
	if (0 != iFindIndex)
	{
		strBuffer.Insert(0, _T("http://"));
	}

    Init();

    Connect(strBuffer,dwFlag);

    bAllDone = FALSE;

    ReadBuffer(pstrFilePath);
}

CInetSession::~CInetSession()
{
    if(hHttpFile)
    {
        InternetCloseHandle(hHttpFile);
        hHttpFile=NULL;
    }
    if(hInternet)
    {
        InternetSetStatusCallback(hInternet,NULL);
        InternetCloseHandle(hInternet);
        hInternet=NULL;
    }
    if(hRequestCompleteEvent)
    {
        CloseHandle(hRequestCompleteEvent);
        hRequestCompleteEvent = NULL;
    }
    if(hConnectedEvent)
    {
        CloseHandle(hConnectedEvent);
        hConnectedEvent = NULL;
    }
    if(hRequestOpenedEvent)
    {
        CloseHandle(hRequestOpenedEvent);
        hRequestOpenedEvent = NULL;
    }
    if(hConnect)
    {
        InternetCloseHandle(hConnect);
        hConnect=NULL;
    }
    if(hRequest)
    {
        InternetCloseHandle(hRequest);
        hRequest=NULL;
    }
    dwLength = 0;
    bAllDone = FALSE;
}

char* CInetSession::GetBuffer()
{
	if(m_str.GetAllocLength() ==0)
        return NULL;
    else
		return m_str.GetBufferSetLength(MAXBUFFERSIZE);
}

DWORD CInetSession::GetLength()
{
    return dwLength; 
}