#include "StdAfx.h"
#include ".\gifhandler.h"


inline int TGIFControlExt::GetPackedValue(enum ControlExtValues Value)
{
	int nRet = (int)cPacked;
	switch (Value)
	{
	case GCX_PACKED_DISPOSAL:
		nRet = (nRet & 28) >> 2;
		break;

	case GCX_PACKED_USERINPUT:
		nRet = (nRet & 2) >> 1;
		break;

	case GCX_PACKED_TRANSPCOLOR:
		nRet &= 1;
		break;
	};

	return nRet;
}

inline int TGIFLSDescriptor::GetPackedValue(enum LSDPackedValues Value)
{
	int nRet = (int)cPacked;

	switch (Value)
	{
	case LSD_PACKED_GLOBALCT:
		nRet = nRet >> 7;
		break;

	case LSD_PACKED_CRESOLUTION:
		nRet = ((nRet & 0x70) >> 4) + 1;
		break;

	case LSD_PACKED_SORT:
		nRet = (nRet & 8) >> 3;
		break;

	case LSD_PACKED_GLOBALCTSIZE:
		nRet &= 7;
		break;
	};

	return nRet;
}

inline int TGIFImageDescriptor::GetPackedValue(enum IDPackedValues Value)
{
	int nRet = (int)cPacked;

	switch (Value)
	{
	case ID_PACKED_LOCALCT:
		nRet >>= 7;
		break;

	case ID_PACKED_INTERLACE:
		nRet = ((nRet & 0x40) >> 6);
		break;

	case ID_PACKED_SORT:
		nRet = (nRet & 0x20) >> 5;
		break;

	case ID_PACKED_LOCALCTSIZE:
		nRet &= 7;
		break;
	};

	return nRet;
}

CGifHandler::CGifHandler(void)
{
	pGIFLSDescriptor = NULL;
	nDataSize = 0;
	nCurrOffset = 0;
	nGlobalCTSize = 0;
	nWidth = 0;
	nHeight = 0;
	pRawImg = NULL;
	pRawData = NULL;
	nCurrFrame = 0;
}

CGifHandler::~CGifHandler(void)
{
	Clear();
}

void CGifHandler::Clear()
{
	m_Dib.Clear();
	if(pRawImg)
	{
		pRawImg->Release();
		pRawImg = NULL;
	}
	if(vecFrame.size() > 1)
	{
		for(VEC_TFRAME::iterator it = vecFrame.begin(); it != vecFrame.end(); it++)
		{
			if(it->pImg)
			{
				it->pImg->Release();
			}
		}
	}
	vecFrame.clear();
}

int CGifHandler::LoadGif(HGLOBAL hGlobal, DWORD dwSize)
{
	IStream *pStream = NULL;
	BOOL bRet = -1;
	__try
	{
		pRawData = (unsigned char *)GlobalLock(hGlobal);
		if(pRawData == NULL)
		{
			__leave;
		};
		TGIFHeader * pGifHeader = (TGIFHeader *)(pRawData);
		if ((memcmp(&pGifHeader->cSignature,"GIF",3) != 0) &&
			((memcmp(&pGifHeader->cVersion,"87a",3) != 0) ||
			(memcmp(&pGifHeader->cVersion,"89a",3) != 0)) )
		{
			bRet = 0;
			GlobalUnlock(hGlobal);
			__leave;
		}
		nDataSize = dwSize;
		// it's a GIF
		pGIFLSDescriptor = (TGIFLSDescriptor *)(pRawData + sizeof(TGIFHeader));
		if (pGIFLSDescriptor->GetPackedValue(LSD_PACKED_GLOBALCT) == 1)
		{
			// calculate the global color table size
			nGlobalCTSize = (int)(3* (1 << (pGIFLSDescriptor->GetPackedValue(LSD_PACKED_GLOBALCTSIZE)+1)));
			// get the background color if GCT is present
			unsigned char *pBkClr = pRawData + sizeof(TGIFHeader) + 
				sizeof(TGIFLSDescriptor) + 3*pGIFLSDescriptor->cBkIndex;
			clrBackground = RGB(pBkClr[0],pBkClr[1],pBkClr[2]);
		};

		nWidth = pGIFLSDescriptor->wWidth;
		nHeight = pGIFLSDescriptor->wHeight;

		// determine frame count for this picture
		UINT nFrameCount=0;
		ResetDataPointer();
		while(SkipNextGraphicBlock())
		{
			nFrameCount++;
		}

		if (nFrameCount == 0) // it's an empty GIF!
		{
			__leave;
		};

		// now check the frame count
		// if there's only one frame, no need to animate this GIF
		// therefore, treat it like any other pic

		// clear GIF variables

		GlobalUnlock(hGlobal);
		// don't delete memory on object's release
		if (CreateStreamOnHGlobal(hGlobal,FALSE,&pStream) != S_OK)
		{
			__leave;
		}
		if (OleLoadPicture(pStream,dwSize,FALSE,IID_IPicture,(LPVOID *)&pRawImg) != S_OK)
		{
			if(pRawImg)
			{
				pRawImg->Release();
			}
			__leave;
		};
		if (nFrameCount == 1)
		{
			pRawData = NULL;
			TFRAME frame;
			frame.pImg = pRawImg;
			vecFrame.push_back(frame);
		}
		else
		{
			// if, on the contrary, there are several frames
			// then store separate frames in an array
			TFrame frame;
			UINT nBlockLen;
			HGLOBAL hFrameData;
			UINT nCurFrame = 0;

			ResetDataPointer();
			while (hFrameData = GetNextGraphicBlock(&nBlockLen,
				&frame.nDelay, &frame.frameSize,
				&frame.frameOffset, &frame.nDisposal) )
			{

				IStream *pStream = NULL;

				// delete memory on object's release
				if (CreateStreamOnHGlobal(hFrameData,TRUE,&pStream) != S_OK)
				{
					GlobalFree(hFrameData);
					continue;
				};

				if (OleLoadPicture(pStream, nBlockLen, FALSE,
					IID_IPicture, reinterpret_cast<LPVOID *>(&frame.pImg)) != S_OK)
				{
					pStream->Release();
					continue;
				};
				pStream->Release();

				if(frame.nDelay < 10)
				{
					frame.nDelay = 10;
				}
				// everything went well, add this frame
				vecFrame.push_back(frame);
			};

			// clean after ourselves
			pRawData = NULL;
			GlobalUnlock(hGlobal);
			if (vecFrame.empty()) // couldn't load any frames
			{
				__leave;
			}
		};
		bRet = 1;
	}
	__finally
	{
		if(pStream)
		{
			pStream->Release();
			pStream = NULL;
		}
	}
	return bRet;
}


BOOL CGifHandler::SkipNextGraphicBlock()
{
	if (!pRawData) return FALSE;

	// GIF header + LSDescriptor [+ GCT] [+ Control block] + Data

	enum GIFBlockTypes nBlock;

	nBlock = GetNextBlock();

	while ((nBlock != BLOCK_CONTROLEXT) &&
		(nBlock != BLOCK_IMAGE) &&
		(nBlock != BLOCK_PLAINTEXT) &&
		(nBlock != BLOCK_UNKNOWN) &&
		(nBlock != BLOCK_TRAILER) )
	{
		if (!SkipNextBlock()) return NULL;
		nBlock = GetNextBlock();
	};

	if ((nBlock == BLOCK_UNKNOWN) ||
		(nBlock == BLOCK_TRAILER))
		return FALSE;

	// it's either a control ext.block, an image or a plain text

	if (GetNextBlockLen() <= 0) return FALSE;

	if (nBlock == BLOCK_CONTROLEXT)
	{
		if (!SkipNextBlock()) return FALSE;
		nBlock = GetNextBlock();

		// skip everything until we meet an image block or a plain-text block
		while ((nBlock != BLOCK_IMAGE) &&
			(nBlock != BLOCK_PLAINTEXT) &&
			(nBlock != BLOCK_UNKNOWN) &&
			(nBlock != BLOCK_TRAILER) )
		{
			if (!SkipNextBlock()) return NULL;
			nBlock = GetNextBlock();
		};

		if ((nBlock == BLOCK_UNKNOWN) ||
			(nBlock == BLOCK_TRAILER))
			return FALSE;
	};

	// skip the found data block (image or plain-text)
	if (!SkipNextBlock()) return FALSE;

	return TRUE;
}

HGLOBAL CGifHandler::GetNextGraphicBlock(UINT *pBlockLen, 
											 UINT *pDelay, SIZE *pBlockSize, SIZE *pBlockOffset, 
											 UINT *pDisposal)
{
	if (!pRawData) return NULL;

	// GIF header + LSDescriptor [+ GCT] [+ Control block] + Data

	*pDisposal = 0;
	enum GIFBlockTypes nBlock;
	nBlock = GetNextBlock();

	while (
		(nBlock != BLOCK_CONTROLEXT) &&
		(nBlock != BLOCK_IMAGE) &&
		(nBlock != BLOCK_PLAINTEXT) &&
		(nBlock != BLOCK_UNKNOWN) &&
		(nBlock != BLOCK_TRAILER)
		)
	{
		if (!SkipNextBlock()) return NULL;
		nBlock = GetNextBlock();
	};

	if ((nBlock == BLOCK_UNKNOWN) ||
		(nBlock == BLOCK_TRAILER))
		return NULL;

	// it's either a control ext.block, an image or a plain text

	int nStart = nCurrOffset;
	int nBlockLen = GetNextBlockLen();

	if (nBlockLen <= 0) return NULL;

	if (nBlock == BLOCK_CONTROLEXT)
	{
		// get the following data
		TGIFControlExt *pControl = 
			reinterpret_cast<TGIFControlExt *> (&pRawData[nCurrOffset]);
		// store delay time
		*pDelay = pControl->wDelayTime;
		// store disposal method
		*pDisposal = pControl->GetPackedValue(GCX_PACKED_DISPOSAL);

		if (!SkipNextBlock()) return NULL;
		nBlock = GetNextBlock();

		// skip everything until we find data to display 
		// (image block or plain-text block)

		while (
			(nBlock != BLOCK_IMAGE) &&
			(nBlock != BLOCK_PLAINTEXT) &&
			(nBlock != BLOCK_UNKNOWN) &&
			(nBlock != BLOCK_TRAILER)
			)
		{
			if (!SkipNextBlock()) return NULL;
			nBlock = GetNextBlock();
			nBlockLen += GetNextBlockLen();
		};

		if ((nBlock == BLOCK_UNKNOWN) || (nBlock == BLOCK_TRAILER))
			return NULL;
		nBlockLen += GetNextBlockLen();
	}
	else
		*pDelay = -1; // to indicate that there was no delay value

	if (nBlock == BLOCK_IMAGE)
	{
		// store size and offsets
		TGIFImageDescriptor *pImage = 
			reinterpret_cast<TGIFImageDescriptor *> (&pRawData[nCurrOffset]);
		pBlockSize->cx = pImage->wWidth;
		pBlockSize->cy = pImage->wHeight;
		pBlockOffset->cx = pImage->wLeftPos;
		pBlockOffset->cy = pImage->wTopPos;
	};

	if (!SkipNextBlock()) return NULL;

	HGLOBAL hGlobal = GlobalAlloc(GMEM_FIXED,
		sizeof(TGIFHeader) +
		sizeof(TGIFLSDescriptor) +
		nGlobalCTSize +
		nBlockLen + 
		1);  // for the trailer

	if (!hGlobal) return NULL;

	int nOffset = 0; 

	// GMEM_FIXED means we get a pointer
	unsigned char *pGlobal = reinterpret_cast<unsigned char *> (hGlobal);

	CopyMemory(pGlobal,pRawData, 
		sizeof(TGIFHeader)+sizeof(TGIFLSDescriptor)+nGlobalCTSize);
	nOffset += sizeof(TGIFHeader)+sizeof(TGIFLSDescriptor)+nGlobalCTSize;

	CopyMemory(pGlobal + nOffset,&pRawData[nStart], nBlockLen);
	nOffset += nBlockLen;

	pGlobal[nOffset] = 0x3B; // trailer
	nOffset++;

	*pBlockLen = nOffset;

	return hGlobal;
}

void CGifHandler::ResetDataPointer()
{
	// skip header and logical screen descriptor
	nCurrOffset = sizeof(TGIFHeader)+sizeof(TGIFLSDescriptor)+nGlobalCTSize;
}

UINT CGifHandler::GetSubBlocksLen(UINT nStartingOffset) const
{
	UINT nRet = 0;
	UINT nCurOffset = nStartingOffset;

	while (pRawData[nCurOffset] != 0)
	{
		nRet += pRawData[nCurOffset]+1;
		nCurOffset += pRawData[nCurOffset]+1;
	};

	return nRet+1;
}

enum GIFBlockTypes CGifHandler::GetNextBlock() const
{
	switch(pRawData[nCurrOffset])
	{
	case 0x21:
		// extension block
		switch(pRawData[nCurrOffset+1])
		{
		case 0x01:
			// plain text extension
			return BLOCK_PLAINTEXT;
			break;

		case 0xF9:
			// graphic control extension
			return BLOCK_CONTROLEXT;
			break;

		case 0xFE:
			// comment extension
			return BLOCK_COMMEXT;
			break;

		case 0xFF:
			// application extension
			return BLOCK_APPEXT;
			break;
		};
		break;

	case 0x3B:
		// trailer
		return BLOCK_TRAILER;
		break;

	case 0x2C:
		// image data
		return BLOCK_IMAGE;
		break;
	};

	return BLOCK_UNKNOWN;
}

BOOL CGifHandler::SkipNextBlock()
{
	if (!pRawData) return FALSE;

	int nLen = GetNextBlockLen();
	if ((nLen <= 0) || ((nCurrOffset+nLen) > nDataSize))
		return FALSE;

	nCurrOffset += nLen;
	return TRUE;
}

int CGifHandler::GetNextBlockLen() const
{
	GIFBlockTypes nBlock = GetNextBlock();

	int nTmp;

	switch(nBlock)
	{
	case BLOCK_UNKNOWN:
		return -1;
		break;

	case BLOCK_TRAILER:
		return 1;
		break;

	case BLOCK_APPEXT:
		nTmp = GetSubBlocksLen(nCurrOffset+sizeof(TGIFAppExtension));
		if (nTmp > 0)
			return sizeof(TGIFAppExtension)+nTmp;
		break;

	case BLOCK_COMMEXT:
		nTmp = GetSubBlocksLen(nCurrOffset+sizeof(TGIFCommentExt));
		if (nTmp > 0)
			return sizeof(TGIFCommentExt)+nTmp;
		break;

	case BLOCK_CONTROLEXT:
		return sizeof(TGIFControlExt);
		break;

	case BLOCK_PLAINTEXT:
		nTmp = GetSubBlocksLen(nCurrOffset+sizeof(TGIFPlainTextExt));
		if (nTmp > 0)
			return sizeof(TGIFPlainTextExt)+nTmp;
		break;

	case BLOCK_IMAGE:
		TGIFImageDescriptor *pIDescr = 
			reinterpret_cast<TGIFImageDescriptor *> (&pRawData[nCurrOffset]);
		int nLCTSize = (int)
			(pIDescr->GetPackedValue(ID_PACKED_LOCALCT)*3*
			(1 << (pIDescr->GetPackedValue(ID_PACKED_LOCALCTSIZE)+1)));

		int nTmp = GetSubBlocksLen(nCurrOffset+
			sizeof(TGIFImageDescriptor) + nLCTSize + 1);
		if (nTmp > 0)
			return sizeof(TGIFImageDescriptor) + nLCTSize + 1 + nTmp;
		break;
	};

	return 0;
}

UINT CGifHandler::GetFrameCount()
{
	return (UINT)vecFrame.size();
}

void CGifHandler::SetFrame(int nIndex /* = -1 */)
{
	if(vecFrame.size() == 0)
	{
		return;
	}
	if(nIndex > 0)
	{
		nCurrFrame = nIndex % (int)vecFrame.size();
	}
	else
	{
		nCurrFrame = (nCurrFrame + 1) % (int)vecFrame.size();
	}
}

int CGifHandler::GetFrameDelay(int nIndex)
{
	if(vecFrame.size() == 0 || nIndex >= (int)vecFrame.size())
	{
		return 0;
	}
	if(nIndex == -1)
	{
		nIndex = nCurrFrame;
	}
	return vecFrame[nIndex].nDelay * 10;
}

void CGifHandler::Draw(HDC hdc)
{
	if(vecFrame.size() == 0)
	{
		return;
	}
	if(vecFrame.size() > 1)
	{
		if(vecFrame[nCurrFrame].nDisposal == 2)
		{
			// ÏÈ»Ö¸´±³¾°É«
			RECT rt;
			SetRect(&rt, 0, 0, nWidth, nHeight);
			::SetBkColor(hdc, clrBackground);
			::ExtTextOut(hdc, 0, 0, ETO_OPAQUE, &rt, NULL, 0, NULL);
		}
		if(m_Dib.IsValid())
		{
			BitBlt(hdc, 0, 0, nWidth, nHeight, m_Dib.GetSafeHdc(), 0, 0, SRCCOPY);
			m_Dib.Clear();
		}
		if(vecFrame[nCurrFrame].nDisposal == 3)
		{
			m_Dib.Create(hdc, nWidth, nHeight);
			BitBlt(m_Dib.GetSafeHdc(), 0, 0, nWidth, nHeight, hdc, 0, 0, SRCCOPY);
		}
	}

	long hmWidth;
	long hmHeight;
	vecFrame[nCurrFrame].pImg->get_Width(&hmWidth);
	vecFrame[nCurrFrame].pImg->get_Height(&hmHeight);
	int x = 0;
	int y = 0;
	UINT cx;
	UINT cy;
	if(vecFrame.size() > 1)
	{
		cx = vecFrame[nCurrFrame].frameSize.cx;
		cy = vecFrame[nCurrFrame].frameSize.cy;
		x += vecFrame[nCurrFrame].frameOffset.cx;
		y += vecFrame[nCurrFrame].frameOffset.cy;
	}
	else
	{
		cx = nWidth;
		cy = nHeight;	
	}
	vecFrame[nCurrFrame].pImg->Render(hdc, x, y, cx, cy, 0, hmHeight, hmWidth, -hmHeight, NULL);
}

