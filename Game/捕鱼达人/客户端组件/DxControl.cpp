#include "StdAfx.h"
#include "DxControl.h"

//////////////////////////////////////////////////////////////////

//构造函数
CDxControl::CDxControl()
{
	m_pDXInputKB=NULL;
	m_pDXInput=NULL;
}

//析构函数
CDxControl::~CDxControl()
{

}

//初始化输入
LRESULT CDxControl::InitDXInput(HINSTANCE hInst,HWND hWnd)
{
	//创建DXInput对象
	HRESULT hr;
	hr=DirectInput8Create(hInst,DIRECTINPUT_VERSION,IID_IDirectInput8,(LPVOID*)&m_pDXInput,NULL);
	if FAILED(hr)
	{
		MessageBox(hWnd,TEXT("创建DXInput对象失败"),TEXT("ERROR"),MB_OK);
		return FALSE;
	}

	//创建键盘设备
	hr=m_pDXInput->CreateDevice(GUID_SysKeyboard,&m_pDXInputKB,NULL);
	if FAILED(hr)
	{
		MessageBox(hWnd,TEXT("创建键盘设备失败"),TEXT("ERROR"),MB_OK);
		return FALSE;
	}

	// 设定为通过一个 256 字节的数组返回查询状态值
	hr=m_pDXInputKB->SetDataFormat(&c_dfDIKeyboard);
	if FAILED(hr)
	{
		MessageBox(hWnd,TEXT("设置键盘数据格式失败"),TEXT("ERROR"),MB_OK);
		return FALSE;
	}

	// 设定协作模式
	//hr=m_pDXInputKB->SetCooperativeLevel(hWnd,DISCL_EXCLUSIVE|DISCL_NOWINKEY);
	//if FAILED(hr)
	//{
	//	MessageBox(hWnd,TEXT("设定协作模式失败"),TEXT("ERROR"),MB_OK);
	//	return FALSE;
	//}

	m_pDXInputKB->Acquire();

	return TRUE;
}

//更新输入
LRESULT CDxControl::UpdateInput()
{
	if(m_pDXInputKB != NULL)      // 如果 m_pDXInputKB 对象界面存在
	{
		HRESULT hr;

		hr = DIERR_INPUTLOST;   // 为循环检测做准备

		// if input is lost then acquire and keep trying
		while(hr == DIERR_INPUTLOST)
		{
			// 读取输入设备状态值到状态数据缓冲区
			hr = m_pDXInputKB->GetDeviceState(sizeof(m_cbKeyState), &m_cbKeyState);

			// DirectInput 报告输入流被中断
			if(hr == DIERR_INPUTLOST)
			{
				// 必须先重新调用 Acquire 方法，然后再试一次
				hr = m_pDXInputKB->Acquire();
				if(FAILED(hr))
					return hr;
			}
		}

		if(FAILED(hr))
			return hr;
	}

	//if(m_pDXInputKB==NULL) return DI_NOTATTACHED;

	////获取键盘
	//if(DI_OK!=m_pDXInputKB->GetDeviceState(sizeof(m_cbKeyState),m_cbKeyState))
	//{	
	//	memset(&m_cbKeyState,0,sizeof(m_cbKeyState));	
	//	m_pDXInputKB->Acquire();
	//}

	return DI_OK;
}

//释放设备
void CDxControl::Release()
{
	SAFE_RELEASE(m_pDXInputKB);
	SAFE_RELEASE(m_pDXInput);

	return;
}
//////////////////////////////////////////////////////////////////
