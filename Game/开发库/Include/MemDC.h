#ifndef _MEMDC_H_
#define _MEMDC_H_

class CMemDC : public CDC 
{
protected:
	CBitmap	m_bitmap;			// 内存中承载临时图象的位图
	CBitmap*	m_oldBitmap;		// 旧的
	CDC*			m_pDC;			// 真实绘图的 CDC
	CRect		m_rect;					// 绘图区域
	bool		m_bMemDC;			// true if CDC really is a Memory DC.
	bool		m_alreadyDraw;		// 已经用 draw 函数画图了，析构函数就不用画了，为了 ReleaseDC
	bool		m_bBg;
    
   void Construct(CDC* pDC)
   {
        ASSERT(pDC != NULL); 

        m_pDC = pDC;
        m_oldBitmap = NULL;
        m_bMemDC = !pDC->IsPrinting();

        if (m_bMemDC) 
		{
            CreateCompatibleDC(pDC);	// 依附窗口DC创建兼容内存DC
            pDC->LPtoDP(&m_rect);
			
            m_bitmap.CreateCompatibleBitmap(pDC, m_rect.Width(), m_rect.Height());
            m_oldBitmap = SelectObject(&m_bitmap);
            
            SetMapMode(pDC->GetMapMode());
            
			//SetWindowExt(pDC->GetWindowExt());			// ？？？完全没有必要
			//SetViewportExt(pDC->GetViewportExt());		//？？？完全没有必要
			
            pDC->DPtoLP(&m_rect);
            SetWindowOrg(m_rect.left, m_rect.top);
        } 
		else 
		{
            m_bPrinting = pDC->m_bPrinting;
            m_hDC       = pDC->m_hDC;
            m_hAttribDC = pDC->m_hAttribDC;
		}
	
        if( m_bBg )
			BitBlt(m_rect.left, m_rect.top, m_rect.Width(), m_rect.Height(),
					 m_pDC, m_rect.left, m_rect.top, SRCCOPY);
		else
			FillSolidRect(m_rect, pDC->GetBkColor());
    }

public:

   /* --------------------------------------------------------------------------
   函数说明：
   传入参数：
		pRect	指定的绘图区域
		bBg		true 表示用原图填充背景，false 表示用单色填充背景
   备注：
		pDC 若是从CDC* pDC= GetDC 中而来，则一定要执行 draw 才能绘图
		pDC 若是从ClientDC dc(this) 中而来，则不需执行 draw ，在析构中自动绘图
   --------------------------------------------------------------------------*/
   CMemDC(CDC* pDC, const RECT* pRect=NULL, bool bBg = false) : CDC() 
   { 
	   m_alreadyDraw = false;
	   m_bBg = bBg;

	   if (pRect == NULL) 
		   pDC->GetClipBox(&m_rect);
		else 
		   m_rect = *pRect;

	   Construct(pDC); 
   }
    
   virtual ~CMemDC()
   {        
		if (!m_alreadyDraw)
		{
			if (m_bMemDC) 
			{
				m_pDC->BitBlt(m_rect.left, m_rect.top, m_rect.Width(), m_rect.Height(),
					this, m_rect.left, m_rect.top, SRCCOPY);

				SelectObject(m_oldBitmap);        
			} 
			else 
			{
				m_hDC = m_hAttribDC = NULL;
			} 
		}

		m_bitmap.DeleteObject();
		m_pDC = NULL;
		m_oldBitmap = NULL;
    }
    
   virtual draw()
   {        
	   if (m_bMemDC) 
	   {
		   m_pDC->BitBlt(m_rect.left, m_rect.top, m_rect.Width(), m_rect.Height(),
			   this, m_rect.left, m_rect.top, SRCCOPY);

		   SelectObject(m_oldBitmap);  
		   m_alreadyDraw = true;
	   } 
	   else 
	   {
		   m_hDC = m_hAttribDC = NULL;
	   }    
   }
};

#endif
