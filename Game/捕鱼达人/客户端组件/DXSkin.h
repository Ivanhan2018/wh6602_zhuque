//////////////////////////////////////
//图象类
/////////////////////////////////////////////////////////////////////////////

#ifndef SKIN_SPRITE_HEAD_FILE
#define SKIN_SPRITE_HEAD_FILE
#pragma once

#include "Stdafx.h"
#include <d3d9.h> 
#include <d3dx9.h>
#include <mmsystem.h>
#pragma comment (lib,"winmm.lib")
#pragma comment (lib,"d3d9.lib")
#pragma comment (lib,"d3dx9.lib")

#define MAX_TEXTURE_COUNT			10							//纹理数量
////////////////////////////////////////////////////////////////////////////////
//X 排列方式
enum enXCollocateMode 
{ 
	enXLeft,						//左对齐
	enXCenter,						//中对齐
	enXRight,						//右对齐
};

//Y 排列方式
enum enYCollocateMode 
{ 
	enYTop,							//上对齐
	enYCenter,						//中对齐
	enYBottom,						//下对齐
};

class CGameClientDlg;

//extern CGameClientDlg	* g_GameClientDlg;

//dx图象类
class CDxSkin
{
	//运行变量
public:	
	IDirect3DTexture9*				m_pTexture[MAX_TEXTURE_COUNT];		//纹理
	D3DXIMAGE_INFO					m_ImageInfo[MAX_TEXTURE_COUNT];		//图片信息
	BYTE							m_cbAlphaValue[MAX_TEXTURE_COUNT];	//透明程度
	CSize							m_csImageSize[MAX_TEXTURE_COUNT];	//图片大小
	CPoint							m_ptTarger[MAX_TEXTURE_COUNT];		//目的位置
	bool							m_bFistLoad;						//首次加载

	//静态变量
public:
	static WORD						m_wSpriteRefCount;					//引用数目
	static ID3DXSprite*				m_Sprite;							//DX精灵

	//函数定义
public:
	//构造函数
	CDxSkin();
	//析构函数
	virtual ~CDxSkin();

	//功能函数
public:
	//图片信息
	void SetImageInfo(UINT uResourceID,WORD wImageIndex=0);
	//加载函数
	void SetLoadInfo(LPDIRECT3DDEVICE9	pD3DDevice,LPCTSTR szFileName,WORD wImageIndex=0,D3DPOOL Pool=D3DPOOL_DEFAULT,D3DCOLOR dwKeyColor=D3DCOLOR_XRGB(255, 0, 255));
	//加载函数
	void SetLoadInfo(LPDIRECT3DDEVICE9	pD3DDevice,UINT uResourceID,WORD wImageIndex=0,D3DPOOL Pool=D3DPOOL_DEFAULT,D3DCOLOR dwKeyColor=D3DCOLOR_XRGB(255, 0, 255));
	//设置透明
	void SetAlphaBlend(BYTE bAlphaValue,WORD wImageIndex=0);
	//设置位置
	void SetDrawPos(INT nXpost,INT nYpost,WORD wImageIndex=0);
	//设置大小
	void SetImageSize(INT nCx,INT nCy,WORD wImageIndex=0);
	//绘画函数
	void DrawSprite(INT x,INT y,CRect &drawRect,WORD wImageIndex=0,D3DCOLOR channelColor=0x00ffffff);
	//绘画函数
	void DrawSprite(INT x,INT y,D3DXVECTOR3 &vCenterPoint, CRect &drawRect,WORD wImageIndex=0,D3DXMATRIX *matRot=NULL,D3DCOLOR channelColor=0x00ffffff);
	//旋转缩放
	bool DrawRotateAndZoom(INT x,INT y,CRect &drawRect,float fCurrentAngle,float fXScaling=1.0f,float fYScaling=1.0f,WORD wImageIndex=0,enXCollocateMode XCollocateMode=enXCenter, enYCollocateMode YCollocateMode=enYCenter);
	//旋转缩放
	bool DrawRotateAndZoom(INT x,INT y,CRect &drawRect,float fCurrentAngle,int nRoteCenterX,int nRoteCenterY,float fXScaling=1.0f,float fYScaling=1.0f,WORD wImageIndex=0);
	//旋转缩放
	bool DrawRotateAndZoom(INT x,INT y,CRect &drawRect,float fCurrentAngle,float fXScaling,float fYScaling,WORD wImageIndex,enXCollocateMode XCollocateMode, enYCollocateMode YCollocateMode,D3DCOLOR channelColor);
	//释放函数
	void Release();

	//绘画函数
public:
	//绘画函数
	void OnDrawImage(WORD wImageIndex=0);

	//返回参数
public:
	//数字宽度
	INT GetWidth(WORD wImageIndex=0){return m_csImageSize[wImageIndex].cx;}
	//数字高度
	INT GetHeight(WORD wImageIndex=0){return m_csImageSize[wImageIndex].cy;}
	//图片宽度
	INT GetImageWidth(WORD wImageIndex=0){return m_ImageInfo[wImageIndex].Width;}
	//图片高度
	INT GetImageHeight(WORD wImageIndex=0){return m_ImageInfo[wImageIndex].Height;}
	//加载判断
	bool IsLoaded(WORD wImageIndex=0){return (m_Sprite!=NULL&&m_pTexture[wImageIndex]!=NULL);}
	//旋转标志
	CPoint GetDrawPoint(WORD wImageIndex=0){return m_ptTarger[wImageIndex];}
	//设备丢失
	HRESULT OnLostDevice() {return (NULL!=m_Sprite)?m_Sprite->OnLostDevice():D3DERR_NOTAVAILABLE;}
	//重置设备
	HRESULT OnResetDevice(){return (NULL!=m_Sprite)?m_Sprite->OnResetDevice():D3DERR_NOTAVAILABLE;}
};

////////////////////////////////////////////////////////////////////////////////
#endif