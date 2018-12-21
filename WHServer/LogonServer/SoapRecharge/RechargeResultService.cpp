#include <afxwin.h>
#include "../Resource.h"
#include "RechargeResult.nsmap"
#include "../Commonality.h"
#include "../LogonService.h"


//充值日志
void TraceRechargeMessage(LPCTSTR pszMessage)
{
	if(0 == lstrlen(pszMessage))
	{
		return ;
	}

	SYSTEMTIME sys; 
	GetLocalTime( &sys ); 
	char buf[128];
	memset(buf, 0, 128);
	sprintf(buf, "%4d/%02d/%02d %02d:%02d:%02d.%03d ",sys.wYear,sys.wMonth,sys.wDay,sys.wHour,sys.wMinute, sys.wSecond,sys.wMilliseconds);

	//按天输出日志
	char filename[128];
	memset(filename, 0, 128);
	sprintf(filename, "Recharge_TraceData_%4d%02d%02d.txt",sys.wYear,sys.wMonth,sys.wDay);

	CFile File;
	if ((File.Open(filename,CFile::modeWrite)==FALSE)&&
		(File.Open(filename,CFile::modeWrite|CFile::modeCreate)==FALSE))
	{
		ASSERT(FALSE);
		return;
	}

	File.SeekToEnd();
	File.Write(buf,lstrlen(buf));
	File.Write(pszMessage,lstrlen(pszMessage));
	File.Write(TEXT("\r\n"),lstrlen(TEXT("\r\n")));

	File.Flush();
	File.Close();

	return;
}

//3个参数分别为支付结果、app_key（与php一样，用来识别是我们的app）、订单号
int ns__RechargeResult(struct soap * rSoap, std::string param0, std::string param1, std::string param2, std::string &param3)
{
	std::cout<<param0<<std::endl;
	std::cout<<param1<<std::endl;
	std::cout<<param2<<std::endl;
	std::cout<<param3<<std::endl;
	

	//返回的支付结果
	if (param0 == "charge.succeeded" && g_InitParamter.m_szRechargeKey == param1) //比较app_key，进行校验
	{
		//支付成功
		//购买成功，发消息，更新数据库
		CMD_GP_UserMallBuyReq * req = new CMD_GP_UserMallBuyReq;
		strcpy(req->dwOrderID, param2.c_str());
		::PostMessage(::AfxGetMainWnd()->m_hWnd, WM_SOAP_MESSAGE, WPARAM(req->S_ID), (LPARAM)req);

		param3 = "Recharge ok!";
		//支付成功日志
		char buf[200];
		memset(buf, 0, 200);
		sprintf(buf, "收到php soap请求，充值校验正确，RechargeResult充值结果=%s，APPKEY=%s，订单号=%s", param0.c_str(), param1.c_str(), param2.c_str());
		TraceRechargeMessage(buf);
	}
	else
	{
		//支付失败
		param3 = "Recharge failed!";
		//支付失败日志
		char buf[200];
		memset(buf, 0, 200);
		sprintf(buf, "收到php soap请求，充值校验失败，RechargeResult充值结果=%s，APPKEY=%s，订单号=%s", param0.c_str(), param1.c_str(), param2.c_str());
		TraceRechargeMessage(buf);
	}

	return 0;
}

int ns__UpdateResult(struct soap * uSoap, std::string param0, std::string param1, std::string param2, std::string &param3)
{
	std::cout<<param0<<std::endl;
	std::cout<<param1<<std::endl;
	std::cout<<param2<<std::endl;
	std::cout<<param3<<std::endl;

	//返回的支付结果
	if (param0 == "charge.succeeded" && g_InitParamter.m_szRechargeKey == param1) //比较app_key，进行校验
	{
		//支付成功
		CMD_GP_UserMallUpdateReq * req = new CMD_GP_UserMallUpdateReq;
		strcpy(req->dwOrderID, param2.c_str());
		::PostMessage(::AfxGetMainWnd()->m_hWnd, WM_SOAP_MESSAGE, WPARAM(req->S_ID), (LPARAM)req);

		param3 = "Update ok!";
		//支付成功日志
		char buf[200];
		memset(buf, 0, 200);
		sprintf(buf, "收到php soap请求，更新校验正确，UpdateResult更新结果=%s，APPKEY=%s，订单号=%s", param0.c_str(), param1.c_str(), param2.c_str());
		TraceRechargeMessage(buf);
	}
	else
	{
		//支付失败
		param3 = "Update failed!";
		//支付失败日志
		char buf[200];
		memset(buf, 0, 200);
		sprintf(buf, "收到php soap请求，更新校验失败，UpdateResult更新结果=%s，APPKEY=%s，订单号=%s", param0.c_str(), param1.c_str(), param2.c_str());
		TraceRechargeMessage(buf);
	}

	return 0;
}

