#pragma once

#include "ocidl.h"
#include "olectl.h"
#include "math.h"
#include <vector>
#include "DibMgr.h"

#pragma pack(1)   // turn byte alignment on
enum GIFBlockTypes
{
	BLOCK_UNKNOWN,
	BLOCK_APPEXT,
	BLOCK_COMMEXT,
	BLOCK_CONTROLEXT,
	BLOCK_PLAINTEXT,
	BLOCK_IMAGE,
	BLOCK_TRAILER
};

enum ControlExtValues // graphic control extension packed field values
{
	GCX_PACKED_DISPOSAL,  // disposal method
	GCX_PACKED_USERINPUT,
	GCX_PACKED_TRANSPCOLOR
};

enum LSDPackedValues  // logical screen descriptor packed field values
{
	LSD_PACKED_GLOBALCT,
	LSD_PACKED_CRESOLUTION,
	LSD_PACKED_SORT,
	LSD_PACKED_GLOBALCTSIZE
};

enum IDPackedValues   // image descriptor packed field values
{
	ID_PACKED_LOCALCT,
	ID_PACKED_INTERLACE,
	ID_PACKED_SORT,
	ID_PACKED_LOCALCTSIZE
};

struct TGIFHeader       // GIF header  
{
	char cSignature[3]; // Signature - Identifies the GIF Data Stream
	// This field contains the fixed value 'GIF'
	char cVersion[3]; // Version number. May be one of the following:
	// "87a" or "89a"
};

struct TGIFLSDescriptor // Logical Screen Descriptor
{
	WORD wWidth; // 2 bytes. Logical screen width
	WORD wHeight; // 2 bytes. Logical screen height

	unsigned char cPacked;      // packed field 

	unsigned char cBkIndex;     // 1 byte. Background color index
	unsigned char cPixelAspect; // 1 byte. Pixel aspect ratio
	inline int GetPackedValue(enum LSDPackedValues Value);
};

struct TGIFAppExtension // application extension block
{
	unsigned char cExtIntroducer; // extension introducer (0x21)
	unsigned char cExtLabel; // app. extension label (0xFF)
	unsigned char cBlockSize; // fixed value of 11
	char cAppIdentifier[8];   // application identifier
	char cAppAuth[3];  // application authentication code
};

struct TGIFControlExt // graphic control extension block
{
	unsigned char cExtIntroducer; // extension introducer (0x21)
	unsigned char cControlLabel;  // control extension label (0xF9)
	unsigned char cBlockSize; // fixed value of 4
	unsigned char cPacked;    // packed field
	WORD wDelayTime; // delay time
	unsigned char cTColorIndex; // transparent color index
	unsigned char cBlockTerm;   // block terminator (0x00)
	inline int GetPackedValue(enum ControlExtValues Value);
};

struct TGIFCommentExt  // comment extension block
{
	unsigned char cExtIntroducer; // extension introducer (0x21)
	unsigned char cCommentLabel;  // comment extension label (0xFE)
};

struct TGIFPlainTextExt // plain text extension block
{
	unsigned char cExtIntroducer;  // extension introducer (0x21)
	unsigned char cPlainTextLabel; // text extension label (0x01)
	unsigned char cBlockSize; // fixed value of 12
	WORD wLeftPos;    // text grid left position
	WORD wTopPos;     // text grid top position
	WORD wGridWidth;  // text grid width
	WORD wGridHeight; // text grid height
	unsigned char cCellWidth;  // character cell width
	unsigned char cCellHeight; // character cell height
	unsigned char cFgColor; // text foreground color index
	unsigned char cBkColor; // text background color index
};

struct TGIFImageDescriptor // image descriptor block
{
	unsigned char cImageSeparator; // image separator byte (0x2C)
	WORD wLeftPos; // image left position
	WORD wTopPos;  // image top position
	WORD wWidth;   // image width
	WORD wHeight;  // image height
	unsigned char cPacked; // packed field
	inline int GetPackedValue(enum IDPackedValues Value);
};
#pragma pack() // turn byte alignment off

typedef struct TFrame
{
	IPicture * pImg;
	SIZE    frameSize;
	SIZE    frameOffset;
	UINT    nDelay;     // delay (in 1/100s of a second)
	UINT    nDisposal;  // disposal method
	TFrame()
	{
		memset(this, 0, sizeof(TFrame));
	}
}TFRAME;

typedef std::vector<TFRAME> VEC_TFRAME;

class CGifHandler
{
public:
	CGifHandler(void);
	~CGifHandler(void);
	UINT GetFrameCount();
	// ret:1 success, 0 not a gif, -1 load error
	int LoadGif(HGLOBAL hGlobal, DWORD dwSize);
	void SetFrame(int nIndex = -1);
	int GetFrameDelay(int nIndex);
	int GetCurrentFrame(){return nCurrFrame;}
	void Draw(HDC hdc);
	void Clear();
	int GetWidth(){return nWidth;}
	int GetHeight(){return nHeight;}
private:
	VEC_TFRAME vecFrame;
	TGIFLSDescriptor * pGIFLSDescriptor;
	UINT nDataSize;
	UINT nCurrOffset;
	unsigned char * pRawData;
	UINT nGlobalCTSize;
	int nWidth;
	int nHeight;
	COLORREF clrBackground;
	IPicture * pRawImg;
	int nCurrFrame;
protected:
	CDibMgr m_Dib;
	void ResetDataPointer();
	UINT GetSubBlocksLen(UINT nStartingOffset) const;
	enum GIFBlockTypes GetNextBlock() const;
	BOOL SkipNextBlock();
	int GetNextBlockLen() const;
	BOOL SkipNextGraphicBlock();
	HGLOBAL GetNextGraphicBlock(UINT *pBlockLen, UINT *pDelay, SIZE *pBlockSize, SIZE *pBlockOffset, UINT *pDisposal);
};
