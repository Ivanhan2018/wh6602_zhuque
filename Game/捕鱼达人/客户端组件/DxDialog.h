/////////////////////////////////////////
//这个是dx窗口管理类
////////////////////////////////////////

#ifndef DX_DIALOG_HEAD_FILE
#define DX_DIALOG_HEAD_FILE
#pragma once

////////////////////////////////////////////////////////////////////////////////

#include "Stdafx.h"
#include <d3d9.h>
#include <d3dx9.h>
#include "DxControl.h"
#include "resource.h"

////////////////////////////////////////////////////////////////////////////////

#define WM_DX_RENDER					WM_APP+100			//渲染消息

struct CUSTOMVERTEX
{
	float x,y,z;
};
////////////////////////////////////////////////////////////////////////////////

//dx窗口管理类
class CDxDialog : public CWnd
{
	//状态变量
public:
	bool					m_bD3DSucess;			//成功标志

	//变量定义
private:
	HANDLE 					m_hDxThread;			//线程句柄
	
	//应用变量
private:
	D3DDISPLAYMODE			g_d3ddm;				//显示模式
	D3DPRESENT_PARAMETERS	g_d3dpp;				//显示参数

	//设备变量
public:
	HWND					g_hWnd;					//d3d窗口句柄
	LPDIRECT3D9				g_pD3D;					//d3d对象
	LPDIRECT3DDEVICE9		g_pD3DDevice;			//d3d设备对象

	//控件变量
public:
	DWORD					g_Time;					//全局时间
	//CDxControl				g_UserControl;			//用户控制

	//函数定义
public:
	//构造函数
	CDxDialog();
	//析构函数
	virtual ~CDxDialog();

public:
	//创建窗口
	virtual bool InitDxWindow();
	//更新窗口
	bool UpdateDxWindowSize();

	//继承函数
protected:
	//更新输入
	virtual void UpdateControl();
	//渲染函数
	virtual void OnRender();
	//绘画2d
	virtual void DrawGameView2D()=NULL;
	//D X丢失
	virtual void OnDeviceLost()=NULL;
	//初始化场景
	virtual void InitScene(INT nWidth,INT nHeight)=NULL;
	//设备重置
    virtual void ReleaseScene()=NULL;
	//停止渲染
	virtual void StopRender();
	//开始渲染
	virtual void StartRender();

	//内部函数
private:
	//渲染消息
	LRESULT OnFrameRender(WPARAM wParam, LPARAM lParam) ;

	DECLARE_MESSAGE_MAP()
};
////////////////////////////////////////////////////////////////////////////////
#endif
