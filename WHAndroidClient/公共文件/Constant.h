#ifndef CONSTANT_HEAD_FILE
#define CONSTANT_HEAD_FILE

#ifndef _DEBUG

//////////////////////////////////////////////////////////////////////////
//项目信息

const TCHAR	szProductName[]=TEXT("藏宝湾");										//产品名字
const TCHAR szSystemRegKey[]=TEXT("WHQPGame");								//注册主键
const TCHAR szStationPage[]=TEXT("192.168.1.108");							//游戏主页
const TCHAR szCookieUrl[]=TEXT("http://192.168.1.108");							//记录地址

#define MAIN_DLG_CLASSNAME				TEXT("WHQPGamePlaza")				//窗口类名

//////////////////////////////////////////////////////////////////////////

#else

//////////////////////////////////////////////////////////////////////////
//网狐信息

const TCHAR	szProductName[]=TEXT("藏宝湾");								//产品名字
const TCHAR szSystemRegKey[]=TEXT("WHQPGame");								//注册主键
const TCHAR szStationPage[]=TEXT("192.168.1.108");							//游戏主页
const TCHAR szCookieUrl[]=TEXT("http://192.168.1.108");							//记录地址

#define MAIN_DLG_CLASSNAME				TEXT("WHQPGamePlaza")				//窗口类名

#endif

//////////////////////////////////////////////////////////////////////////

#endif