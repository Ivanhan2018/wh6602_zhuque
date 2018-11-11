// stdafx.cpp : 只包括标准包含文件的源文件
// GameFrame.pch 将成为预编译头
// stdafx.obj 将包含预编译类型信息

#include "stdafx.h"

#define USE_HOU 0	// 

/*
两种方式的比较
SIRT_TRANS_DOT_MORE模式  SIRT_SOLID_DOT_MORE模式
iCount=8                   iCount=258 
iCount=0                   iCount=17 
iCount=0                   iCount=26 
iCount=48                  iCount=154 
iCount=14                  iCount=325
iCount=0                   iCount=20 
iCount=710                 iCount=1430
*/

BOOL SetImageRgn_TransDotLess(HWND hWnd, CPngImage * pImage);
BOOL SetImageRgn_SolidDotLess(HWND hWnd, CPngImage * pImage);
BOOL SetImageRgn_FullDotLess(HWND hWnd, CPngImage* pImage);
BOOL SetImageRgn_DeburringDotLess(HWND hWnd, CPngImage* pImage);//Author:<cxf>; CreateDate:<2011-12-27>; Description:<针对修剪透明四角>

BOOL SetImageRgn(HWND hWnd, CPngImage * pImage, int iType)
{
	switch(iType)
	{
	case SIRT_TRANS_DOT_LESS:
		{
			return SetImageRgn_TransDotLess(hWnd, pImage);
		}
	case SIRT_SOLID_DOT_LESS:
		{
			return SetImageRgn_SolidDotLess(hWnd, pImage);
		}
	case SIRT_FULL_DOT_LESS:
		{
			return SetImageRgn_FullDotLess(hWnd, pImage);
		}
	case SIRT_DEBURRING_DOT_LESS://Author:<cxf>; CreateDate:<2011-12-27>; Description:<针对修剪透明四角>
		{
			return SetImageRgn_DeburringDotLess(hWnd, pImage);
		}
	default:
		break;
	}
	
	return false;
}

// add by HouGuoJiang 2011-11-25
// 针对一幅图里面空心（透明）的点比较少
BOOL SetImageRgn_TransDotLess(HWND hWnd, CPngImage * pImage)
{
//	if (!pImage) 
//		return FALSE;
//	if (pImage->IsNull()) 
//		return FALSE;
//
//	CBitmap* pBmp;
//	CImage imgBuf;
//
//	int iWidth = pImage->GetWidth();
//	int iHeight = pImage->GetHeight();
//	int iCount = 0;
//
//	imgBuf.Create(iWidth, iHeight, 32);
//	CDC *pDCTmp = CDC::FromHandle(imgBuf.GetDC());	// 缺少成对出现的 CDC::DeleteTempMap  ???
//	pImage->DrawImage(pDCTmp, 0, 0, iWidth, iHeight, 0, 0, iWidth, iHeight);
//	pBmp = ((CBitmap *)(&imgBuf));
//	DWORD * pData = new DWORD[iWidth * iHeight];
//	pBmp->GetBitmapBits((sizeof (DWORD)* iWidth * iHeight),  pData);
//
//	CRgn rgnImage;			// pImage 的 镜像
//	CRgn rgnDecrease;		// 要剪掉的区域
//
//	rgnImage.CreateRectRgn(0,  0,  iWidth,  iHeight);
//	rgnDecrease.CreateRectRgn(0,  0,  iWidth,  iHeight);
//
//	DWORD *pDataTmp = pData;
//	
//	bool		bStart = true;	// 可以做起点
//	CPoint	ptStartTrans;		// 透明的起点
//	CPoint	ptEndTrans;			// 透明的终点
//	int		iPix;		
//	int		iPhalanx = 0;		// 方阵的点
//	
//	for (int h=0; h<iHeight; ++h)	// 高度
//	{
//		iPhalanx = iWidth * h;
//		bStart = true;				// 可以做起点
//		ptStartTrans.SetPoint(0, h);
//		ptEndTrans.SetPoint(0, h);
//		for(int w=0;  w<iWidth;  w++, pDataTmp++)
//		{
//			iPix = (*pDataTmp) & 0xff000000;
//			if(0 == iPix)	// 完全透明
//			{
//				if(bStart)
//				{
//					ptStartTrans.SetPoint(w, h);
//					bStart = false;	// 不能做起点
//
//					// 最后一个起点透明 ,直接裁剪
//					if(w==iWidth)
//					{
//						// 裁剪，透明会比非透明的少，所以裁剪透明比叠加非透明会更有效率 ...... 
//						rgnDecrease.SetRectRgn(ptStartTrans.x, ptStartTrans.y,  ptEndTrans.x,  ptEndTrans.y+1);	// 要裁剪的区域
//						rgnImage.CombineRgn(&rgnImage, &rgnDecrease, RGN_XOR);	// 两个剪裁区域的全部除去公共部分		
//						iCount++;
//					}
//				}
//			}
//			else	// 不完全透明
//			{
//				ptEndTrans.SetPoint(w-1, h);
//				
//				// 安全距离并且
//				if(ptStartTrans.x <= ptEndTrans.x)
//				{
//					iPix = pData[iPhalanx + ptEndTrans.x ] & 0xff000000;
//					if (0==iPix)	// 该点透明
//					{
//						// 裁剪，透明会比非透明的少，所以裁剪透明比叠加非透明会更有效率 ...... 
//						rgnDecrease.SetRectRgn(ptStartTrans.x, ptStartTrans.y,  ptEndTrans.x,  ptEndTrans.y+1);	// 要裁剪的区域
//						rgnImage.CombineRgn(&rgnImage, &rgnDecrease, RGN_XOR);	// 	两个剪裁区域的全部除去公共部分	
//						iCount++;
//					}
//				}
//				bStart = true;
//			}
//		}
//	}
//
//#if _DEBUG
//	CString strTmp;
//	strTmp.Format("iCount=%d \r\n", iCount);
//	TRACE(strTmp.GetBuffer());
//#endif
//
//	SafeDeleteArray(pData);
//	imgBuf.ReleaseDC();
//	imgBuf.Destroy();
//
//	BOOL bResult = TRUE;
//
//	if (rgnImage.m_hObject)
//	{
//		SetWindowRgn(hWnd, (HRGN)rgnImage.m_hObject, TRUE);
//		DeleteObject(rgnImage);
//	}
//	else
//	{
//		bResult = FALSE;
//	}
//
//	DeleteObject(rgnDecrease);
//	
//	CDC::DeleteTempMap();	// add by HouGuoJiang 2011-11-25
//
//	return bResult;

	if (!pImage) 
		return FALSE;
	if (pImage->IsNull()) 
		return FALSE;

	CBitmap* pBmp;
	CImage imgBuf;

	int iWidth = pImage->GetWidth();
	int iHeight = pImage->GetHeight();
	int iCount = 0;

	imgBuf.Create(iWidth, iHeight, 32);
	CDC *pDCTmp = CDC::FromHandle(imgBuf.GetDC());	// 缺少成对出现的 CDC::DeleteTempMap  ???
	pImage->DrawImage(pDCTmp, 0, 0, iWidth, iHeight, 0, 0, iWidth, iHeight);
	pBmp = ((CBitmap *)(&imgBuf));
	DWORD * pData = new DWORD[iWidth * iHeight];
	pBmp->GetBitmapBits((sizeof (DWORD)* iWidth * iHeight),  pData);

	CRgn rgnImage;			// pImage 的 镜像
	CRgn rgnDecrease;		// 要剪掉的区域

	rgnImage.CreateRectRgn(0,  0,  iWidth,  iHeight);
	rgnDecrease.CreateRectRgn(0,  0,  iWidth,  iHeight);

	DWORD *pDataTmp = pData;

	bool		bStart = true;	// 可以做起点
	CPoint	ptStartTrans;		// 透明的起点
	CPoint	ptEndTrans;			// 透明的终点
	int		iPix;		
	int		iPhalanx = 0;		// 方阵的点

	for (int h=0; h<iHeight; ++h)	// 高度
	{
		iPhalanx = iWidth * h;
		bStart = true;				// 可以做起点
		ptStartTrans.SetPoint(0, h);
		ptEndTrans.SetPoint(0, h);
		for(int w=0;  w<iWidth;  w++, pDataTmp++)
		{
			iPix = (*pDataTmp) & 0xff000000;
			if(0 == iPix)	// 完全透明
			{
				if(bStart)
				{
					ptStartTrans.SetPoint(w, h);
					bStart = false;	// 不能做起点
				}
				// 最后一个起点透明 ,直接裁剪
				if(w==(iWidth-1))
				{
					// 裁剪，透明会比非透明的少，所以裁剪透明比叠加非透明会更有效率 ...... 
					rgnDecrease.SetRectRgn(ptEndTrans.x+2, ptEndTrans.y,  iWidth,  ptStartTrans.y+1);	// 要裁剪的区域
					rgnImage.CombineRgn(&rgnImage, &rgnDecrease, RGN_XOR);	// 两个剪裁区域的全部除去公共部分		
					iCount++;
				}
			}
			else	// 不完全透明
			{
				ptEndTrans.SetPoint(w-1, h);

				// 安全距离并且
				if(ptStartTrans.x <= ptEndTrans.x)
				{
					iPix = pData[iPhalanx + ptEndTrans.x ] & 0xff000000;
					if (0==iPix)	// 该点透明
					{
						// 裁剪，透明会比非透明的少，所以裁剪透明比叠加非透明会更有效率 ...... 
						rgnDecrease.SetRectRgn(ptStartTrans.x, ptStartTrans.y,  ptEndTrans.x,  ptEndTrans.y+1);	// 要裁剪的区域
						rgnImage.CombineRgn(&rgnImage, &rgnDecrease, RGN_XOR);	// 	两个剪裁区域的全部除去公共部分	
						iCount++;
					}
				}
				bStart = true;
			}
		}
	}

#if _DEBUG
	CString strTmp;
	strTmp.Format("iCount=%d \r\n", iCount);
	TRACE(strTmp.GetBuffer());
#endif

	SafeDeleteArray(pData);
	imgBuf.ReleaseDC();
	imgBuf.Destroy();

	BOOL bResult = TRUE;

	if (rgnImage.m_hObject)
	{
		SetWindowRgn(hWnd, (HRGN)rgnImage.m_hObject, TRUE);
		DeleteObject(rgnImage);
	}
	else
	{
		bResult = FALSE;
	}

	DeleteObject(rgnDecrease);

	CDC::DeleteTempMap();	// add by HouGuoJiang 2011-11-25

	return bResult;
}

// 针对一幅图里面实心（非透明）的点比较少
BOOL SetImageRgn_SolidDotLess(HWND hWnd, CPngImage * pImage)
{
	if (NULL==pImage) 
		return FALSE;

	if (pImage->IsNull()) 
		return FALSE;

	int iCount = 0;
	INT wWidth = pImage->GetWidth();
	INT wHeight = pImage->GetHeight();
	CBitmap * pbmp;
	CImage imgBuf;
	imgBuf.Create(wWidth, wHeight, 32);

	CDC *pDCTmp = CDC::FromHandle(imgBuf.GetDC());	// 一定要成对出现 CDC::DeleteTempMap 
	pImage->DrawImage(pDCTmp, 0, 0, wWidth, wHeight, 0, 0, wWidth, wHeight);
	pbmp = ((CBitmap *)(&imgBuf));
	DWORD * pData = new DWORD[wWidth * wHeight];
	pbmp->GetBitmapBits((sizeof (DWORD)* wWidth * wHeight), pData);

	CRgn rgnImage, tempRgn;
	rgnImage.CreateRectRgn(0, 0, 0, 0);
	tempRgn.CreateRectRgn(0,  0,  wWidth,  wHeight);	// 测试用
	DWORD *pDataTmp = pData;
	for (UINT h = 0;h < wHeight;++h)	// 高度
	{
		UINT w = 0;
		UINT leftX = 0;		// 不透明（包括半透明）象素的最左端
		UINT rightX = 0;	// 不透明（包括半透明）象素的最右端
		while(w < wWidth)	// 宽度
		{
			// 0xABGR 表示为 0xFFFFFFFF 高位的A为 00  表示全透明，FF表示不透明
			// 逐个掠过透明象素，一直走到不透明（包括半透明）的象素
			while (w++ < wWidth && (0 == (*pDataTmp++ & 0xff000000)));	

			leftX =(w - 1);	// 第一个不透明（包括半透明）的象素
			if(leftX >= wWidth-1)
				continue;	

			// 逐个掠过不透明（包括半透明）的象素，一直走到透明的象素
			while (w++ < wWidth &&(0 != (*pDataTmp++ & 0xff000000))); 

			rightX = w-1;
			if(rightX > wWidth-1)	// 约束宽度
				rightX = wWidth - 1;
			
			tempRgn.SetRectRgn(leftX, h,  rightX, h+1);	// modi by HouGuoJiang 2011-11-25 ，
														// 没必要在一个函数里面反复的 CreateRectRgn 与 DeleteObject 裁剪区对象以致于消耗过高

			rgnImage.CombineRgn(&rgnImage, &tempRgn, RGN_OR);
			iCount++;
		}
	}


	BOOL bResult = TRUE;
	CString strTmp;
	strTmp.Format("iCount=%d \r\n", iCount);
	TRACE(strTmp.GetBuffer());
	
	SafeDeleteArray(pData);
	imgBuf.ReleaseDC();
	imgBuf.Destroy();
	
	if (rgnImage.m_hObject)
	{
		SetWindowRgn(hWnd, (HRGN)rgnImage.m_hObject, TRUE);
		DeleteObject(rgnImage);
	}
	else
	{
		bResult = FALSE;
	}

	DeleteObject(tempRgn);
	CDC::DeleteTempMap();	// add by HouGuoJiang 2011-11-25

	return bResult;
}

BOOL SetImageRgn_FullDotLess(HWND hWnd, CPngImage* pImage)
{
	if (NULL==pImage) return FALSE;
	if (pImage->IsNull()) return FALSE;
	INT wWidth = pImage->GetWidth();
	INT wHeight = pImage->GetHeight();
	CBitmap * pbmp;
	CImage imgBuf;
	imgBuf.Create(wWidth, wHeight, 32);
	CDC *pDCTmp = CDC::FromHandle(imgBuf.GetDC());
	pImage->DrawImage(pDCTmp, 0, 0, wWidth, wHeight, 0, 0, wWidth, wHeight);
	pbmp = ((CBitmap *)(&imgBuf));
	DWORD * pData = new DWORD[wWidth * wHeight];
	pbmp->GetBitmapBits((sizeof (DWORD)* wWidth * wHeight), pData);
	imgBuf.ReleaseDC();
	imgBuf.Destroy();

	CRgn rgnImage, tempRgn;
	rgnImage.CreateRectRgn(0, 0, 0, 0);
	tempRgn.CreateRectRgn(0, 0, wWidth, wHeight);
	DWORD *pDataTmp = pData;
	for (UINT h = 0;h < wHeight;++h)
	{
		UINT w = 0, leftX = 0;
		while(w < wWidth)
		{
			while (w++ < wWidth &&(0 == (*pDataTmp++ & 0xff000000)));
			leftX =(w - 1);
			while (w++ < wWidth &&(0 != (*pDataTmp++ & 0xff000000))); 
			tempRgn.SetRectRgn(leftX, h,(w - 1),(h + 1));
			rgnImage.CombineRgn(&rgnImage, &tempRgn, RGN_OR);
		}
	}

	DeleteObject(tempRgn);

	SafeDeleteArray(pData);

	if (NULL == rgnImage.m_hObject)
		return FALSE;

	SetWindowRgn(hWnd, (HRGN)rgnImage.m_hObject, TRUE);
	DeleteObject(rgnImage);
	CDC::DeleteTempMap();

	return TRUE;
}

//Author:<cxf>
//CreateDate:<2011-12-27>
//Description:<针对修剪透明四角>
BOOL SetImageRgn_DeburringDotLess(HWND hWnd, CPngImage* pImage)
{
	if (NULL==pImage) return FALSE;
	if (pImage->IsNull()) return FALSE;
	INT wWidth = pImage->GetWidth();
	INT wHeight = pImage->GetHeight();
	CBitmap * pbmp;
	CImage imgBuf;
	imgBuf.Create(wWidth, wHeight, 32);
	CDC *pDCTmp = CDC::FromHandle(imgBuf.GetDC());
	pImage->DrawImage(pDCTmp, 0, 0, wWidth, wHeight, 0, 0, wWidth, wHeight);
	pbmp = ((CBitmap *)(&imgBuf));
	DWORD * pData = new DWORD[wWidth * wHeight];
	pbmp->GetBitmapBits((sizeof (DWORD)* wWidth * wHeight), pData);
	imgBuf.ReleaseDC();
	imgBuf.Destroy();

	CRgn rgnImage, tempRgn;
	rgnImage.CreateRectRgn(0, 0, 0, 0);
	tempRgn.CreateRectRgn(0, 0, wWidth, wHeight);
	DWORD *pDataTmp = pData;
	WORD wAreaW=30,wAreaH=30;
	if (wAreaW > wWidth || wAreaH > wHeight)return false;

	//透明角
	for (UINT i=0; i<wHeight; ++i)
	{
		UINT j=0,k=wWidth;
		while (j++ < wAreaW && (0 == (pData[(i*wWidth)+j] & 0xff000000)));

		while (k-- > wWidth-wAreaW && (0 == (pData[(i*wWidth)+k] & 0xff000000)));

		tempRgn.SetRectRgn(j, i, k, (i + 1));
		rgnImage.CombineRgn(&rgnImage, &tempRgn, RGN_OR);
	}

	DeleteObject(tempRgn);

	SafeDeleteArray(pData);

	if (NULL == rgnImage.m_hObject) return FALSE;

	SetWindowRgn(hWnd, (HRGN)rgnImage.m_hObject, TRUE);
	DeleteObject(rgnImage);
	CDC::DeleteTempMap();

	return TRUE;
}