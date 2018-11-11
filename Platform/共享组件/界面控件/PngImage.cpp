#include "StdAfx.h"
#include "PngImage.h"

#include "ISonicUI.h"
using namespace sonic_ui;

//////////////////////////////////////////////////////////////////////////
#define  STATIC_COVERT(p)           (static_cast<ISonicImage*> (p))


//构造函数
CPngImage::CPngImage():m_pSrcImage(NULL)
{
}

//析构函数
CPngImage::~CPngImage()
{
	//销毁图片
	DestroyImage(); 

	return;
}

//是否加载
bool CPngImage::IsNull()
{
	//加载判断
	if (NULL!=m_pSrcImage && STATIC_COVERT(m_pSrcImage)->IsValid())
	{
		return false;
	}
	return true;
}

//获取宽度
INT CPngImage::GetWidth()
{
	//加载判断
	if (IsNull()==true) return 0;

	//获取宽度
	return STATIC_COVERT(m_pSrcImage)->GetWidth();
}

//获取高度
INT CPngImage::GetHeight()
{
	//加载判断
	if (IsNull()==true) return 0;

	//获取高度
	return STATIC_COVERT(m_pSrcImage)->GetHeight();
}

//销毁图片
bool CPngImage::DestroyImage()
{
	//删除对象
	if (NULL != m_pSrcImage)
	{
		GetSonicUI()->DestroyObject((ISonicImage*)(m_pSrcImage));
	}
	m_pSrcImage = NULL;
	return true;
}

//加载图片
bool CPngImage::LoadImage(LPCTSTR pszFileName)
{
	//加载判断
	DestroyImage();

	m_pSrcImage = (ISonicImage*)GetSonicUI()->CreateImage();
	STATIC_COVERT(m_pSrcImage)->Load(pszFileName);
	if (NULL != m_pSrcImage)
	{
		return true;
	}
	return false;
}

//加载图片
bool CPngImage::LoadImage(HINSTANCE hInstance, LPCTSTR pszResourceName)
{

	////加载判断
	//DestroyImage();

	//m_pSrcImage = GetSonicUI()->CreateImage();
	//bool bRet = STATIC_COVERT(m_pSrcImage)->Load(MAKEINTRESOURCE(pszResourceName), hInstance, "PNG");
	//if (NULL != m_pSrcImage && bRet)
	//{
	//	return true;
	//}
	return false;
}

//绘画图像
bool CPngImage::DrawImage(CDC * pDC, INT nXPos, INT nYPos)
{
	//加载判断
	if (IsNull()==true) return false;

	STATIC_COVERT(m_pSrcImage)->Draw(pDC->m_hDC, nXPos, nYPos);

	INT nDestWidth=0, nDestHeight=0;
	nDestWidth = STATIC_COVERT(m_pSrcImage)->GetWidth();
	nDestHeight = STATIC_COVERT(m_pSrcImage)->GetHeight();

	DRAW_PARAM param;
	param.dwMask = DP_TILE|DP_SRC_CLIP|DP_DEST_LIMIT|DP_ALPHA;
	param.nTileLength = nDestWidth;
	param.nTileDivider = 1;
	param.cAlpha = 255;
	param.fScaleX = 1.0f;
	param.fScaleY = 1.0f;
	param.cx = nDestWidth;
	param.cy = nDestHeight;

	param.rtSrc.left = 0;
	param.rtSrc.top = 0;
	param.rtSrc.right = param.rtSrc.left + nDestWidth;
	param.rtSrc.bottom = param.rtSrc.top + nDestHeight;

	STATIC_COVERT(m_pSrcImage)->Draw(pDC->m_hDC, nXPos, nYPos, &param);

	return true;
}

//绘画图像
bool CPngImage::DrawImage(CDC * pDC, INT nXDest, INT nYDest, INT nDestWidth, INT nDestHeight, INT nXScr, INT YSrc)
{
	//加载判断
	if (IsNull()==true) return false;

	DRAW_PARAM param;
	param.dwMask = DP_TILE|DP_SRC_CLIP|DP_DEST_LIMIT|DP_ALPHA;
	param.nTileLength = nDestWidth;
	param.nTileDivider = 1;
	param.cAlpha = 255;
	param.fScaleX = 1.0f;
	param.fScaleY = 1.0f;
	param.cx = nDestWidth;
	param.cy = nDestHeight;

	param.rtSrc.left = nXScr;
	param.rtSrc.top = YSrc;
	param.rtSrc.right = param.rtSrc.left + nDestWidth;
	param.rtSrc.bottom = param.rtSrc.top + nDestHeight;

	STATIC_COVERT(m_pSrcImage)->Draw(pDC->m_hDC, nXDest, nYDest, &param);
	return true;
}

//绘画图像
bool CPngImage::DrawImage(CDC * pDC, INT nXDest, INT nYDest, INT nDestWidth, INT nDestHeight, INT nXScr, INT YSrc, INT nSrcWidth, INT nSrcHeight)
{
	//加载判断
	if (IsNull()==true) return false;

	DRAW_PARAM param;
	param.dwMask = DP_TILE|DP_SRC_CLIP|DP_DEST_LIMIT|DP_ALPHA;
	param.nTileLength = nDestWidth;
	param.nTileDivider = 1;
	param.cAlpha = 255;
	param.fScaleX = 1.0f;
	param.fScaleY = 1.0f;
	param.cx = nDestWidth;
	param.cy = nDestHeight;

	param.rtSrc.left = nXScr;
	param.rtSrc.top = YSrc;
	param.rtSrc.right = param.rtSrc.left + nSrcWidth;
	param.rtSrc.bottom = param.rtSrc.top + nSrcHeight;

	STATIC_COVERT(m_pSrcImage)->Draw(pDC->m_hDC, nXDest, nYDest, &param);
	return true;
}

//设置透明度(0-100)
void CPngImage::SetImageAlpha(int iAlpha)
{
	//加载判断
	if (IsNull()==true) return;

	if (iAlpha<0 || iAlpha>255) return;

	STATIC_COVERT(m_pSrcImage)->SetAlphaChannel(iAlpha);
}


//获得透明值get the ARGB of specified coordinates
DWORD CPngImage::GetAlphaValue(const int x,const int y)
{
	//加载判断
	if (IsNull()==true) return 0;

	return STATIC_COVERT(m_pSrcImage)->GetPixel(x, y);
}

//////////////////////////////////////////////////////////////////////////
