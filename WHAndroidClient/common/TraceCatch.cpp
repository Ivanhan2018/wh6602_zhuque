#include "stdafx.h"

#include "TraceCatch.h"

int __show__( const TCHAR* szTemp )
{
	//保存日志
	FILE* f = fopen( "./Trace_Catch.log", "a" ) ;
	fwrite( szTemp, 1, strlen(szTemp), f ) ;
	fwrite( "\r\n", 1, 2, f ) ;
	fclose(f) ;

	static MyLock lock ;

	lock.Lock() ;
	//int iRet = AfxMessageBox(szTemp);//::MessageBoxA( NULL, szTemp, "异常", MB_OK ) ;
	#if _DEBUG
		DebugBreak();
	#endif
	lock.Unlock() ;

	return 1;
}

int __messagebox__(const TCHAR*msg )
{
	AfxMessageBox(msg) ;

	return 1;
}

int __assertspecial__ ( const TCHAR * file , int line , const TCHAR * func , const TCHAR* msg)
{
	TCHAR szTemp[1024] = {0};

	sprintf( szTemp, "%s %s [%s][%d][%s][%s]\n", __DATE__,__TIME__, file, line, func ,msg ) ;

	return __show__(szTemp) ;
}
