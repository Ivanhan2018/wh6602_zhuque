///////////////////////////////////////////////////////////////////////////////
// file name: cnd_ILogMgr.h
// author: condor
// date created: 2007-09-05
// date last modified: 2007-09-05
// person web of author: www.staresky.com
///////////////////////////////////////////////////////////////////////////////


#ifndef CND_ILOGMGR_H_20070905
#define CND_ILOGMGR_H_20070905


#include "unknwn.h"

// {3669456D-645C-4565-8A6B-2F4CE4EC52BD}
DEFINE_GUID(CLSID_DefLogMgr, 0x3669456d, 0x645c, 0x4565, 0x8a, 0x6b, 0x2f, 0x4c, 0xe4, 0xec, 0x52, 0xbd);

// {02AEAE3B-988F-4102-B075-5D11B0D2536F}
DEFINE_GUID(CLSID_GuiLogMgr, 0x2aeae3b, 0x988f, 0x4102, 0xb0, 0x75, 0x5d, 0x11, 0xb0, 0xd2, 0x53, 0x6f);

// {D11EB638-4A61-4706-B40E-A0827D27BE63}
DEFINE_GUID(IID_ILogMgr, 0xd11eb638, 0x4a61, 0x4706, 0xb4, 0xe, 0xa0, 0x82, 0x7d, 0x27, 0xbe, 0x63);

// {A2E06FB9-2CAE-49f7-B3BF-6F609A124B9F}
DEFINE_GUID(IID_ILogMsgQueue, 0xa2e06fb9, 0x2cae, 0x49f7, 0xb3, 0xbf, 0x6f, 0x60, 0x9a, 0x12, 0x4b, 0x9f);


// {9D4C77B3-1747-4e18-84ED-25E6F9A7902F}
DEFINE_GUID(IID_ILogMgrEvent, 0x9d4c77b3, 0x1747, 0x4e18, 0x84, 0xed, 0x25, 0xe6, 0xf9, 0xa7, 0x90, 0x2f);






///////////////////////////////////////////////////////////////////////////////
//							CLogManager statement							 //
// 使用说明:该组件用于记录日志，使用时必须先调用Register()注册一个（其内容包 //
// 括：pszFileNameBase日志文件名路径，LogManager会为该						 //
// 文件名加上SN以产生多个同类日志； nViewLevel显示级别； nLogLevel日志记录级 //
// 别，当添加的日志消息的级别〉=该级别时才显示或记录日志。					 //
//																			 //
// 注意：MsgTransmitServer需要该组件记录运行日志，而且使用的Type为0(定为系统 //
// 日志，所以在使用MsgTransmitServer时必须先Register a system log type(type  //
// is zero)																	 //
///////////////////////////////////////////////////////////////////////////////
#define BUGREPORT(a) BugReport(a, __FILE__, __LINE__)


struct cnd_LogMsg
{
	enum { MSG_BUF_SIZE = 512 };

	int			nMsgSize;
	char		szMsgBuf[MSG_BUF_SIZE];
};


DECLARE_INTERFACE_(cnd_ILogMgrEvent, IUnknown)
{
// Methods
public:
	STDMETHOD(OnLogMsg)() PURE;
};


DECLARE_INTERFACE_(cnd_ILogMgr, IUnknown)
{
// Attributes
public:
	STDMETHOD(GetMaxLine)(int* pLine) PURE;
	STDMETHOD(SetMaxLine)(int nMaxLine) PURE;
	
	// File name expand mode
	STDMETHOD(GetExpandMode)(int* pExpandMode) PURE;
	STDMETHOD(SetExpandMode)(int nExpandMode = 0) PURE;
	
	// msg was printed while Log's View level more than nViewLever and was writed 
	// to file while Log's loglever more than nLogLevel
	STDMETHOD(GetRecordLevel)(int* pViewLevel, int* pLogLevel) PURE;
	STDMETHOD(SetRecordLevel)(int nViewLevel, int nLogLevel) PURE; 
	
	
	// Expand file name base on the base name
	STDMETHOD(GetFileNameBase)(TCHAR* pszResultBuf, int nBufSize) PURE;
	STDMETHOD(SetFileNameBase)(const TCHAR* pszFileNameBase) PURE;
	
// Methods
	STDMETHOD(Init)() PURE;
	STDMETHOD(Uninit)() PURE;

	STDMETHOD(Start)() PURE;
	STDMETHOD(Stop)() PURE;

	STDMETHOD(EnableViewMsg)(BOOL bEnable = TRUE) PURE;
	STDMETHOD(EnableWriteLog)(BOOL bEnable = TRUE) PURE;

	STDMETHOD(Register)(const TCHAR* pszFileNameBase, int nMaxLine = 50000, int nViewLevel = 0, int nLogLevel = 0) PURE;

	STDMETHOD(QueryBaseDir)(TCHAR* pszBaseDir, int nSizeOfBytes) PURE;
									
	// Report state of running
	STDMETHOD(ReportRunState)() PURE;
	STDMETHOD(QueryRunState)(int nQueryType, DWORD* pdwState) PURE;

	STDMETHOD(AddMsg)(int nLogLevel, int nViewLevel, BOOL bIsNeedDate, const TCHAR* format, ...) PURE;
	STDMETHOD(AddMsg)(const TCHAR* pszMsg, int nLogLevel = 0, int nViewLevel = 0, BOOL bIsNeedDate = FALSE) PURE;

	STDMETHOD(BugReport)(const TCHAR* pszMsg, const TCHAR* pszFileName, int nLine) PURE;
};



DECLARE_INTERFACE_(cnd_ILogMsgQueue, IUnknown)
{
// Methods
public:
	STDMETHOD(PugMsg)(cnd_LogMsg* pLogMsg) PURE;

	STDMETHOD(GetMsg)(cnd_LogMsg** ppLogMsg) PURE;
	STDMETHOD(PeekMsg)(cnd_LogMsg** ppLogMsg) PURE;

	STDMETHOD(RecycleMsg)(cnd_LogMsg* pLogMsg) PURE;

	
};





#endif