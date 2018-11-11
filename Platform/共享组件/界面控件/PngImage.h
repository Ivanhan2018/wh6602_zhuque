#ifndef PNG_IMAGE_HEAD_FILE
#define PNG_IMAGE_HEAD_FILE

#pragma once

#include "SkinControls.h"

//////////////////////////////////////////////////////////////////////////

//图片对象
class SKIN_CONTROL_CLASS CPngImage
{
	//变量定义
protected:
	void*       m_pSrcImage; 
	//函数定义
public:
	//构造函数
	CPngImage();
	//析构函数
	virtual ~CPngImage();

	//状态函数
public:
	//是否加载
	bool IsNull();
	//获取宽度
	INT GetWidth();
	//获取高度
	INT GetHeight();

	//管理函数
public:
	//销毁图片
	bool DestroyImage();
	//加载图片
	bool LoadImage(LPCTSTR pszFileName);
	//加载图片
	bool LoadImage(HINSTANCE hInstance, LPCTSTR pszResourceName);

public:
	//设置透明度(0-100)
	void SetImageAlpha(int iAlpha=100);
	//获得透明值get the ARGB of specified coordinates
	DWORD GetAlphaValue(const int x,const int y);

	//绘画函数
public:
	//绘画图像
	bool DrawImage(CDC * pDC, INT nXPos, INT nYPos);
	//绘画图像
	bool DrawImage(CDC * pDC, INT nXDest, INT nYDest, INT nDestWidth, INT nDestHeight, INT nXScr, INT YSrc);
	//绘画图像
	bool DrawImage(CDC * pDC, INT nXDest, INT nYDest, INT nDestWidth, INT nDestHeight, INT nXScr, INT YSrc, INT nSrcWidth, INT nSrcHeight);
};

//////////////////////////////////////////////////////////////////////////

#endif