#include "stdafx.h"
#include "DxDialog.h"

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CDxDialog, CWnd)
	ON_MESSAGE(WM_DX_RENDER, OnFrameRender)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//static bool		m_bD3DSucess;						//DX初始化成功标志

//////////////////////////////////////////////////////////////////////////

//构造函数
CDxDialog::CDxDialog()
{
	m_bD3DSucess=false;
	g_Time = 0;
}

//析构函数
CDxDialog::~CDxDialog()
{
	SAFE_RELEASE(g_pD3DDevice);
	SAFE_RELEASE(g_pD3D);
}

//渲染线程
void DxThreadRender(LPVOID pThreadData)
{
	//效验参数
	ASSERT(pThreadData!=NULL);
	if(pThreadData==NULL) return;

	//类型转换
	CDxDialog *pDxDialog=(CDxDialog *)pThreadData;

	//降低cpu使用率
	while(NULL!=pDxDialog && pDxDialog->GetSafeHwnd()!=NULL && pDxDialog->m_bD3DSucess)
	{
		SendMessage(pDxDialog->GetSafeHwnd(), WM_DX_RENDER, 0, 0) ;
		Sleep(10);
	}

	return;
}

//渲染消息
LRESULT CDxDialog::OnFrameRender(WPARAM wParam, LPARAM lParam)
{
	if(g_pD3DDevice==NULL) return 0;
	HRESULT hResult=g_pD3DDevice->TestCooperativeLevel();
	if (hResult==D3DERR_DEVICELOST) 
	{
		OnDeviceLost();
		return 0;
	}
	if (hResult==D3DERR_DEVICENOTRESET)
	{
		OnDeviceLost();
		UpdateDxWindowSize();
		InitScene(g_d3dpp.BackBufferWidth,g_d3dpp.BackBufferHeight);
		return 0;
	}

	OnRender() ;

	return 0 ;
}

//dx窗口初始化
bool CDxDialog::InitDxWindow()
{
	////加载时间
	//DWORD dwStart = GetTickCount();

	//创建判断
	if(m_bD3DSucess==true) return true;

	//获得实例
	g_hWnd=this->GetSafeHwnd();
	if(g_hWnd==NULL) return false;

	//初始化d3d
	try{
		//返回结果
		HRESULT hr;

		//取得DX版本,显示模式
		g_pD3D=Direct3DCreate9(D3D_SDK_VERSION);
		hr=g_pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &g_d3ddm);
		if(FAILED(hr)) 
		{
			MessageBox(TEXT("获得显示适配器模式失败!"),TEXT("错误提示"),MB_OK);
			return false;
		}

		//设置显示参数
		ZeroMemory(&g_d3dpp, sizeof(g_d3dpp));
		g_d3dpp.Windowed=true;										//是否窗口模式显示
		g_d3dpp.BackBufferFormat=g_d3ddm.Format;					//后缓冲区模式
		g_d3dpp.SwapEffect=D3DSWAPEFFECT_DISCARD;					//交换缓冲类型(直接翻屏)
		//g_d3dpp.BackBufferWidth=100;								//后缓冲宽
		//g_d3dpp.BackBufferHeight=20;								//后缓冲高
		g_d3dpp.hDeviceWindow=g_hWnd;								//窗口句柄
		g_d3dpp.FullScreen_RefreshRateInHz=D3DPRESENT_RATE_DEFAULT;	//显示器刷新率
		g_d3dpp.PresentationInterval=D3DPRESENT_INTERVAL_IMMEDIATE;	//图像最大刷新速度
		g_d3dpp.AutoDepthStencilFormat=D3DFMT_D16;					//深度缓冲格式
		g_d3dpp.EnableAutoDepthStencil=TRUE;						//激活深度缓冲

		//兼容检测
		D3DCAPS9 d3dCaps;
		if (FAILED(g_pD3D->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &d3dCaps))) return false;
		DWORD dwBehaviorFlags=0;
		if (d3dCaps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT) dwBehaviorFlags=D3DCREATE_HARDWARE_VERTEXPROCESSING;
		else dwBehaviorFlags=D3DCREATE_SOFTWARE_VERTEXPROCESSING;

		//创建设备
		hr=g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, g_hWnd,dwBehaviorFlags, &g_d3dpp, &g_pD3DDevice);
		if (FAILED(hr))
		{
			if (FAILED(g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_REF, g_hWnd, dwBehaviorFlags,&g_d3dpp, &g_pD3DDevice))) return false;
		}

		//g_pD3DDevice->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);

		//D3DCAPS9 d3dCaps;
		//if (FAILED(g_pD3D->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &d3dCaps))) return false;

		//DWORD dwBehaviorFlags = 0;
		//if (d3dCaps.VertexProcessingCaps!=0)
		//	dwBehaviorFlags|=D3DCREATE_HARDWARE_VERTEXPROCESSING;
		//else
		//	dwBehaviorFlags|=D3DCREATE_SOFTWARE_VERTEXPROCESSING;

		//ZeroMemory(&g_d3dpp, sizeof(g_d3dpp));
		//g_d3dpp.BackBufferFormat=g_d3ddm.Format;
		//g_d3dpp.SwapEffect=D3DSWAPEFFECT_DISCARD;
		//g_d3dpp.Windowed=TRUE; 
		//g_d3dpp.EnableAutoDepthStencil=TRUE;
		//g_d3dpp.AutoDepthStencilFormat=D3DFMT_D16; 
		//g_d3dpp.PresentationInterval=D3DPRESENT_INTERVAL_IMMEDIATE;
		//if (FAILED( g_pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, g_hWnd,dwBehaviorFlags, &g_d3dpp, &g_pD3DDevice)))
		//{
		//	return false;
		//}

		//开启渲染线程
		UINT uThreadID=0;
		m_bD3DSucess = true;
		m_hDxThread=(HANDLE)::CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)DxThreadRender,this,0,0);		
	}catch(...){}

 	//初始化输入
	//g_UserControl.InitDXInput(AfxGetInstanceHandle(),g_hWnd);

	/*DWORD dwStop = GetTickCount();
	TCHAR tch[32]=TEXT("");
	_sntprintf(tch,sizeof(tch),TEXT("%d"),dwStop-dwStart);*/

	return (g_hWnd!=NULL);
}

//更新窗口
bool CDxDialog::UpdateDxWindowSize()
{
	//释放设备
	//ReleaseScene();
	//g_UserControl.Release();
	StopRender();
	//SAFE_RELEASE(g_pD3DDevice);
	//SAFE_RELEASE(g_pD3D);

	//窗口大小改变需要重新设置d3d设备，与初始化相似
	try{
		//g_pD3D=Direct3DCreate9(D3D_SDK_VERSION);
		g_pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &g_d3ddm);

		//设置显示参数
		ZeroMemory(&g_d3dpp, sizeof(g_d3dpp));
		g_d3dpp.Windowed=true;										//是否窗口模式显示
		g_d3dpp.BackBufferFormat=g_d3ddm.Format;					//后缓冲区模式
		g_d3dpp.SwapEffect=D3DSWAPEFFECT_DISCARD;					//交换缓冲类型(直接翻屏)
		//g_d3dpp.BackBufferWidth=nWidth;							//后缓冲宽
		//g_d3dpp.BackBufferHeight=nHeight;							//后缓冲高
		g_d3dpp.hDeviceWindow=g_hWnd;								//窗口句柄
		g_d3dpp.FullScreen_RefreshRateInHz=D3DPRESENT_RATE_DEFAULT;	//显示器刷新率
		g_d3dpp.PresentationInterval=D3DPRESENT_INTERVAL_IMMEDIATE;	//图像最大刷新速度
		g_d3dpp.AutoDepthStencilFormat=D3DFMT_D16;					//深度缓冲格式
		g_d3dpp.EnableAutoDepthStencil=TRUE;						//激活深度缓冲

		//还原设备
		g_pD3DDevice->Reset(&g_d3dpp);

		//创建设备
		//g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, g_hWnd,D3DCREATE_SOFTWARE_VERTEXPROCESSING, &g_d3dpp, &g_pD3DDevice);
		if(g_pD3DDevice==NULL) return false;
	}catch(...){}

	//初始化输入
	//g_UserControl.InitDXInput(AfxGetInstanceHandle(),g_hWnd);

	//开始渲染
	StartRender();

	return true;
}

//用户控制
VOID CDxDialog::UpdateControl()
{
	return;
}

//渲染函数 客户端可以重载onrender();必须重写DrawGameViewEx()
void CDxDialog::OnRender()
{	
	//效验状态
	if(m_bD3DSucess==false) return;
	if(g_pD3DDevice==NULL) return;

	//贞数限制
	if((GetTickCount()-g_Time)<10) return;
	g_Time=GetTickCount();

	//更新控制
	//UpdateControl();

	//清除后备缓冲区
	g_pD3DDevice->Clear(0,NULL,D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER,D3DCOLOR_XRGB(100,100,100),1.0f,0);

	//开始绘制场景
	g_pD3DDevice->BeginScene();

	//g_Camera.Update2DMatrix();
	DrawGameView2D();

	//结束绘制
	g_pD3DDevice->EndScene();

	//翻页
	g_pD3DDevice->Present(NULL,NULL,NULL,NULL);
}

//设置停止标志
void CDxDialog::StopRender()
{
	m_bD3DSucess=false;
}

//开始渲染
void CDxDialog::StartRender()
{
	m_bD3DSucess=true;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////

