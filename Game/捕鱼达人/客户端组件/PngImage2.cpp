#include "StdAfx.h"
#include "pngimage2.h"



CGdiPlusGameBase::CGdiPlusGameBase(void)
{
}

CGdiPlusGameBase::~CGdiPlusGameBase(void)
{

}


CPngImage2::CPngImage2(void)
{
	m_hMem = NULL;
	m_Image =NULL;
	this->m_DrawImage = NULL; 
}


CPngImage2::~CPngImage2(void)
{

}

void CPngImage2::Clear()
{

	if(m_Image)
	{
	   delete m_Image;
	   m_Image = NULL;
	}
	if(m_DrawImage)
	{
		delete m_DrawImage;
		m_DrawImage = NULL;
	}
}


void  CPngImage2::Init( HDC param_parenthdc,
								HINSTANCE param_hinstance,
								LPCTSTR param_bitmapname,
								LPCTSTR param_bitmaptype
								)
	{
		try{
		this->Clear();
		m_DrawImage = Graphics::FromHDC(param_parenthdc); 
		ImageFromIDResource(param_hinstance,param_bitmapname,param_bitmaptype);
		}
		catch(...)
		{
		}
		
	}

BOOL CPngImage2::ImageFromIDResource(HINSTANCE param_hinstance,
							 LPCTSTR param_bitmapname,
							 LPCTSTR param_bitmaptype
							 )

{
		try{
			if(m_hMem)
				GlobalFree(m_hMem);
			HRSRC hRsrc = ::FindResource(param_hinstance,
										param_bitmapname,
										param_bitmaptype);

			if (!hRsrc)
					return FALSE;

			DWORD len = SizeofResource(param_hinstance,hRsrc);
			
			BYTE* lpRsrc = (BYTE*)LoadResource(param_hinstance,hRsrc);
			
			if (!lpRsrc)
				return FALSE;

			m_hMem = GlobalAlloc(GMEM_FIXED, len);

			BYTE* pmem = (BYTE*)GlobalLock(m_hMem);

			memcpy(pmem,lpRsrc,len);

			IStream* pstm = NULL;

			CreateStreamOnHGlobal(m_hMem,FALSE,&pstm);

			m_Image  = Image::FromStream(pstm);
   

			GlobalUnlock(m_hMem);

			pstm->Release();

		FreeResource(lpRsrc);

		}
		catch(...)
		{
			TRACE0("ERROR: ImageFromIdResource");				
		}
		return true;
}

void   CPngImage2::DrawImage(int top,int left)
{
		try{
		   m_DrawImage->DrawImage( m_Image,
			   top,left,m_Image->GetWidth(),m_Image->GetHeight());
		}
		catch(...)
		{
			TRACE0("error");
		}
}

void CPngImage2::DrawImage(CDC *pDC ,int top,int left)
{
	if(m_DrawImage)
	{
		delete m_DrawImage;
        m_DrawImage = NULL;
	}
    m_DrawImage = Graphics::FromHDC(pDC->GetSafeHdc()); 
	m_DrawImage->DrawImage( m_Image,
		                    top,left,m_Image->GetWidth(),m_Image->GetHeight());

}