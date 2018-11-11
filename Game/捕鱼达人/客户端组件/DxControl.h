//////////////////////////////////////////////////////////////////////
//用户控制类，用于直接获取键盘消息
//////////////////////////////////////////////////////////////////////
#ifndef DX_CONTROL_HEAD_FILE
#define DX_CONTROL_HEAD_FILE
#pragma once

#define DIRECTINPUT_VERSION 0x0800

#include <dinput.h>
#include <dinputd.h>
#include <d3dx9math.h>
#pragma comment (lib,"dinput8.lib")
#pragma comment (lib,"dxguid.lib")

//////////////////////////////////////////////////////////////////////

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=NULL; } }
#endif

//////////////////////////////////////////////////////////////////////

//输入控制类
class CDxControl
{
	//设备变量
private:
	LPDIRECTINPUT8			m_pDXInput;				//输入设备
	LPDIRECTINPUTDEVICE8	m_pDXInputKB;			//键盘设备

	//状态变量
public:
	BYTE					m_cbKeyState[256];		//键盘状态

	//基础函数
public:
	//构造函数
	CDxControl();
	//析构函数
	~CDxControl();

	//功能函数
public:
	//初始化输入
	LRESULT InitDXInput(HINSTANCE hInst,HWND hWnd);
	//更新状态
	LRESULT UpdateInput();
	//释放资源
	void Release();
};
//////////////////////////////////////////////////////////////////////

#endif