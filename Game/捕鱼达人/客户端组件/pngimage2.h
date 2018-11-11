#pragma once

#include "gdiplus.h"

using namespace Gdiplus;


class CGdiPlusGameBase
{
public:
	CGdiPlusGameBase(void);
	~CGdiPlusGameBase(void);
	GdiplusStartupInput  m_gdiplusStartupInput;
	ULONG_PTR m_gdiplusToken;

	void CGdiPlusGameBase_Init()
	{
	    VERIFY(GdiplusStartup(&m_gdiplusToken, &m_gdiplusStartupInput, NULL)==Ok);
	};
	void CGdiPluseGameBase_Destroy()
	{
		GdiplusShutdown(m_gdiplusToken);
	};
};

class CPngImage2
{
public:
	CPngImage2(void);
	~CPngImage2(void);
	
	Graphics* m_DrawImage;

	Image*   m_Image;
	HGLOBAL m_hMem;


	void Init(
				HINSTANCE param_hinstance,
				LPCTSTR param_bitmapname,
				LPCTSTR param_bitmaptype
				)
	{
		try{
		   this->Clear();
		   ImageFromIDResource(param_hinstance,param_bitmapname,param_bitmaptype);
		}
		catch(...)
		{
		}	
	
	};
	void Init( HDC param_parenthdc,
				HINSTANCE param_hinstance,
				LPCTSTR param_bitmapname,
				LPCTSTR param_bitmaptype
				);
	BOOL ImageFromIDResource(HINSTANCE param_hinstance,
							 LPCTSTR param_bitmapname,
							 LPCTSTR param_bitmaptype//,
							 );

	void Clear();



	int GetWidth()
	 {
		 return m_Image->GetWidth();
	 };

	int GetHeight()
	{
		return m_Image->GetHeight();
	};

	void DrawImage(int top,int left);

	void DrawImage(CDC *pDC ,int top,int left);

	void   TransparentDrawImage(int top,int left,int width,int height,int imagetop,int imageleft,int imagewidth,
		            int imageheight,Color imagecolorkey = Color(255,255,255))
	{
			ImageAttributes imAtt;
				imAtt.SetColorKey(imagecolorkey,imagecolorkey,ColorAdjustTypeBitmap);
    	   m_DrawImage->DrawImage( m_Image,
								Rect(top,left,width,height),   
								imagetop,
								imageleft,
								imagewidth,
								imageheight,
								UnitPixel, 
								&imAtt);
	};

	void  DrawImage(int top,int left,int width,int height,int imagetop,int imageleft,int imagewidth,int imageheight)
	{
		  m_DrawImage->DrawImage( m_Image, 
								Rect(top,left,width,height),   
								imagetop,
								imageleft,
								imagewidth,
								imageheight,
								UnitPixel, 
								NULL);	
	};

	void  DrawImage(CDC *pDC,int top,int left,int width,int height,int imagetop,int imageleft,int imagewidth,int imageheight)
	{

			if(m_DrawImage)
				{
					delete m_DrawImage;
					m_DrawImage = NULL;
				}
		     m_DrawImage = Graphics::FromHDC(pDC->GetSafeHdc()); 
		  m_DrawImage->DrawImage( m_Image, 
								Rect(top,left,width,height),   
								imagetop,
								imageleft,
								imagewidth,
								imageheight,
								UnitPixel, 
								NULL);	
	};
};
