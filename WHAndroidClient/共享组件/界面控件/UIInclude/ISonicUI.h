#pragma once


#ifdef SONICUI_EXPORTS
#define SONICUI_API _declspec(dllexport)
#else
#define SONICUI_API
#endif


#define ARGB(a, r, g, b) ((a) << 24 | (r) << 16 | (g) << 8 | (b))
#define CONVERT_RGB(rgb) ((rgb) >> 16 & 0xff | (rgb) << 16 & 0xff0000 | (rgb) & 0xff00)
#define DEFAULT_COLOR_KEY	RGB(128, 128, 128)
#define SONICUI_DESTROY(pBase) if(pBase){ GetSonicUI()->DestroyObject(pBase); pBase = NULL; }

//////////////////////////////////////////////////////////////////////////
//					SonicUI GUI engine
//
//	Author:			Sonic Guan
//	Date:			2010-05-17
//	Version:		v1.1
//	Introduction:	This is a GUI engine based on the nake gdi APIs.It offers several UI components to
//					accomplish kinds of complex effects, such as irregular windows, URLs on windows, self-draw
//					buttons, animaiton and image operation methods, etc. The main purpose is to use the least
//					code to achieve the best effect and efficiency so you can release yourself from repeated 
//					and flavourless UI work, and concentrate on the more meaning part.
//	Statement:		You can use this code freely, and modify or add components at your pleasure, but please don't 
//					erase this statement. Sharing your modified copy with me would be highly appreciated.
//					It's not allowed to use for merchant purpose without contact with author.
//////////////////////////////////////////////////////////////////////////

namespace sonic_ui
{
	// Delegable events
	enum enDelegateEvent
	{
		DELEGATE_EVENT_NONE = 0,	
		DELEGATE_EVENT_CLICK,		// lbutton click
		DELEGATE_EVENT_RCLICK,		// rbutton click
		DELEGATE_EVENT_DBLCLICK,	// lbutton double click
		DELEGATE_EVENT_DBLRCLICK,	// rbutton double click
		DELEGATE_EVENT_MOUSEWHEEL,	// mouse wheel
		DELEGATE_EVENT_MOUSEENTER,	// mouse enter
		DELEGATE_EVENT_MOUSEHOVER,	// mouse hover
		DELEGATE_EVENT_MOUSELEAVE,	// mouse leave
		DELEGATE_EVENT_GIF_OVER,	// just after a gif complete playing all its frames
		DELEGATE_EVENT_SPARKLE_OVER,// after sparkle
		DELEGATE_EVENT_SHOWGENTLY_OVER,	// after fadeout
		DELEGATE_EVENT_MOVEGENTLY_OVER,	// after move gently
		DELEGATE_EVENT_TRANSFORM_OVER,	// after transform
		DELEGATE_EVENT_ANI_NUM_OVER,	// after a number animation 
		DELEGATE_EVENT_PAINT,			// delegate a procedure to this event of ISonicPaint, which will be called while the Paint is drawing
		DELEGATE_EVENT_SLIDE_OVER,		// after slide
		DELEGATE_EVENT_SHUTTER_OVER,	// after shutter animation
	};

	// Optional Draw Param Mask used by ISonicImage::Draw
	enum enDrawParam
	{
		DP_NONE			= 0x0,	// none
		DP_ALPHA		= 0x1,	// transparent
		DP_SRC_CLIP		= 0x2,	// source clip
		DP_SCALE		= 0x4,	// scale
		DP_DEST_LIMIT	= 0x8,	// auto scale to fit the destination region
		DP_COLOR_KEY	= 0x10,	// color key, cannot be combined with DP_ALPHA
		DP_TILE			= 0x20,	// tile draw. image will be seperated into 3 partes(left, mid, right), use nTileDivider in DRAW_PARAM to decide the width of edge(imageWidth / nTileDivider)
		DP_VER_TILE		= 0x40,	// vertically tile, cannot be combined with DP_TILE. image will be seperated into 3 partes(top, mid, bottom), use nTileDivider in DRAW_PARAM to decide the width of edge(imageWidth / nTileDivider)
	};

	// Components type
	enum enBaseType
	{
		BASE_TYPE_NONE = 0x1457,
		BASE_TYPE_IMAGE,			// ISonicImage
		BASE_TYPE_WND_EFFECT,		// ISonicWndEffect
		BASE_TYPE_STRING,			// ISonicString
		BASE_TYPE_TEXT_SCROLL_BAR,	// ISonicTextScrollBar
		BASE_TYPE_PAINT,			// ISonicPaint
		BASE_TYPE_ANIMATION,		// ISonicAnimation
		BASE_TYPE_SKIN,				// ISonicSkin
		BASE_TYPE_END,
	};

	// attributes used by ISonicUI::HandleRawString 
	enum enRawStringChangeType
	{
		RSCT_DOUBLE = 0x1,		// convert all "/" to "//" in a raw string to avoid keyword confusion
		RSCT_REVERSE = 0x2,		// convert all "/" to "\\" in a raw string to avoid keyword confusion
		RSCT_URL = 0x4,			// auto identify and add URL mark in a raw string
	};

	// Image types that ISonicImage can support
	enum enImageType
	{
		IMAGE_TYPE_BMP = 0,
		IMAGE_TYPE_JPEG,
		IMAGE_TYPE_PNG,
		IMAGE_TYPE_TGA,
		IMAGE_TYPE_ICO,
	};

	// Optional attributes used by ISonicTextScrollBar
	enum enTextScrollBarOpt
	{
		TSBO_SINGLE_NO_SCROLL = 0x1,		// the scroll bar will not scroll text if there is only one item in it
		TSBO_LEFT_ALIGN	= 0x2,				// scroll alignment
		TSBO_FIRST_NO_SCROLL = 0x4,			// don't scroll the first item
	};

	// windows effects used by ISonicWndEffect
	enum enWndEffectMask
	{
		WND_EFFECT_NORMAL = 0,
		WND_EFFECT_GRAY = 0x1,			// grayness
		WND_EFFECT_FRAME = 0x2,			// frame
		WND_EFFECT_WHOLE_DRAG = 0x4,	// full window drag
		WND_EFFECT_HSL = 0x8,			// full window HSL adjustment
		WND_EFFECT_PREDRAG = 0x10,		// window predrag effect
		WND_EFFECT_TRANSFORM = 0x20,	// transform:rotate and scale
		WND_EFFECT_BKCOLOR = 0x40,		// set background color
		WND_EFFECT_TRACK_MOUSE = 0x80,	// use a timer to track mouse
	};

	// animations status
	enum enAnimationStatus
	{
		ANI_STATUS_NONE = 0,
		ANI_STATUS_MOVE_GENTLY = 0x1,	// moving
		ANI_STATUS_SHOW_GENTYLE = 0x2,	// fading out
		ANI_STATUS_TRANSFROM = 0x4,		// rotating or scaling
	};
	
	// Optional Draw Param Mask used by ISonicImage::Draw
	typedef struct tagDrawParam
	{
		DWORD dwMask;			// maskstruct 
		float fScaleX;			// width scale rate
		float fScaleY;			// height scale rate	
		int cx;					// destination region width
		int cy;					// destination region height
		int nTileLength;		// tile width or height
		int nTileDivider;		// horizontally divide the orignal width into n blocks and make the width of a block as the width of left and right edges
		unsigned char cAlpha;	// transparent rate
		RECT rtSrc;				// source region clip
		DWORD dwColorKey;		// color key
	}DRAW_PARAM;

	// mainly used in proc delegated to DELEGATE_EVENT_PAINT of ISonicPaint
	typedef struct tagCurrentPaint
	{
		HDC hdc;				// current hdc of paint, a real memdc or a virtual one
		int x;					// x coordinate of hdc in the window
		int y;					// y coordinate of hdc in the window
		HWND hWnd;				// window contain current hdc
		RECT rtMem;				// dc size
	}CURRENT_PAINT;

	class ISonicBase;
	// Paint Items
	typedef struct tagPaintItem
	{
		ISonicBase * pBase;		// a Item
		int x;					// x coordinate of the item
		int y;					// y coordinate of the item
		BOOL bVisible;			// item visibility
	}PAINT_ITEM;

	//////////////////////////////////////////////////////////////////////////
	// interface:	ISonicBase
	// function:	base class type contain common attributes and methods
	//////////////////////////////////////////////////////////////////////////
	class ISonicBase
	{
	public:
		ISonicBase();
		virtual ~ISonicBase();

		// get the base object id
		virtual DWORD GetObjectId();

		// get the validity of current object
		virtual BOOL IsValid() = 0;

		// clear data only, but not destroy the buffer
		virtual void Clear() = 0;

		// get base type
		virtual DWORD GetType();

		// delegate various proc to events
		// 'message' stands for enDelegateEvent
		// 'pReserve' stands for a delegated data used by callback function
		// 'pClass' is used only when the delegated proc is a member function of a class
		// the last param is the delegated proc. here use the volatile parameter trick to avoid C++ compliling check when the proc is a member function
		//////////////////////////////////////////////////////////////////////////
		// notice: delegated proc can be only declared as following style:
		// void WINAPI Func(ISonicBase *, LPVOID pReserve);
		//////////////////////////////////////////////////////////////////////////
		// the ISonicBase * is a pointer to the object which was delegated previously
		virtual void Delegate(UINT message, LPVOID pParam, LPVOID pClass, ...);

		// get handle of window which was attached by this object
		virtual HWND GetSafeHwnd();

		friend class ISonicUI;
	protected:
		DWORD m_dwType;
	};

	//////////////////////////////////////////////////////////////////////////
	// interface:	ISonicBaseContainer
	// function:	common container interfaces. a container can hold plenty of object as items
	//////////////////////////////////////////////////////////////////////////
	class ISonicBaseContainer
	{
	public:
		// add objects
		virtual BOOL AddObject(DWORD dwObjectId, int x = 0, int y = 0) = 0;

		// delete objects
		virtual BOOL DelObject(DWORD dwObjectId, BOOL bByIndex = FALSE, BOOL bDestroyObject = FALSE) = 0;

		// set objects region
		virtual BOOL SetObject(DWORD dwObjectId, int x = 0, int y = 0, BOOL bVisible = TRUE, BOOL bByIndex = FALSE) = 0;

		// delete all objects
		virtual BOOL DelAllObject(BOOL bDestroyObject = FALSE) = 0;

		// get the count of current objects
		virtual int GetCount() = 0;

		// get specified PAINT_ITEM of a object by object id or index
		virtual PAINT_ITEM * GetPaintItem(DWORD dwObjectId, BOOL bByIndex = FALSE) = 0;

		// show or hide a specified object by object id or index
		virtual BOOL ShowObject(BOOL b, BOOL bRedraw, DWORD dwObjectId, BOOL bByIndex = FALSE) = 0;
	};

	//////////////////////////////////////////////////////////////////////////
	// interface:	ISonicBaseCtrl
	// function:	common ctrl methods.
	//////////////////////////////////////////////////////////////////////////
	class ISonicBaseCtrl
	{
	public:
		// render a control, can be called within a WM_PAINT, if not, will be auto called at the end of paintng
		virtual BOOL Render(HDC hdc) = 0;

		// get current region
		virtual const RECT * GetRect() = 0;

		// don't use it
		virtual void SwitchRender(BOOL b);

		// don't use it
		virtual BOOL GetRenderSwitch();
	private:
		BOOL m_bRender;
	};

	//////////////////////////////////////////////////////////////////////////
	// interface:	ISonicImage
	// function:	image operation interface.Loading and saving procedure depends on CxImage lib internally. Supports bmp, jpeg, png, tga, gif, ico.
	//				Supports rotation, scale and hsl adjustment, etc.
	// remarks:		ISonicImage does not response to windows message so it cannot be delegated.
	//////////////////////////////////////////////////////////////////////////
	class ISonicImage : public ISonicBase
	{
	public:
		// load from file
		virtual BOOL Load(LPCTSTR lpszFileName) = 0;

		// load from resource
		// nResId:			resource id
		// hResModule:		module contain resource
		// szResourceType:	resource type name, default as "IMAGE"
		virtual BOOL Load(UINT nResId, HMODULE hResModule = NULL, LPCTSTR szResourceType = NULL) = 0;

		// load from hdc
		// bAlphaChannel:	image will add alpha channel if this param is set 
		virtual BOOL Load(HDC hdc, int x, int y, int cx, int cy, BOOL bAlphaChannel = FALSE) = 0;

		// create a image with specified attributes
		virtual BOOL Create(int cx, int cy, DWORD dwColor = 0, BOOL bWithAlpha = FALSE) = 0;

		// fill the image with specified color 
		virtual BOOL Flush(DWORD dwColor) = 0;

		// fill the alpha channel with 0
		virtual BOOL DestroyAlphaChannel() = 0;

		// set alpha channel with specified value
		virtual BOOL SetAlphaChannel(BYTE bAlpha) = 0;
		
		// draw to hdc with optional params
		virtual BOOL Draw(HDC hdc, int x = 0, int y = 0, DRAW_PARAM * pParam = NULL) = 0;

		// draw without any effects, even the alpha channel
		virtual BOOL SetBitsToDevice(HDC hdc, int x = 0, int y = 0) = 0;

		// get the frame count of this image
		virtual UINT GetFrameCount() = 0;

		// switch to specified frame, nIndex = -1 stands for next frame.
		virtual void SetCurrentFrame(int nIndex = -1) = 0;

		// get current frame index
		virtual UINT GetCurrentFrame() = 0;

		// get delay of specified frame, -1 stands for current
		virtual UINT GetFrameDelay(int nIndex = -1) = 0;

		// get width
		virtual int GetWidth() = 0;

		// get height
		virtual int GetHeight() = 0;

		// copy from
		virtual BOOL CloneFrom(const ISonicImage *) = 0;

		// rotate
		// pScout is a scout point, set a scout point in and the out value is the converted coordinates after rotation
		virtual BOOL Rotate(int nAngle, POINT * pScout = NULL) = 0;

		// grayness
		virtual BOOL Gray() = 0;

		// save as file, imgType is between 0 and 100, only available when imgType is IMAGE_TYPE_JPEG
		virtual BOOL SaveAsFile(LPCTSTR lpszFileName, enImageType imgType, int nQuality = 0) = 0;

		// make a rgn with specified color key. remember to destroy the rgn object when not used
		// x, y presents the destination left and top coordinates in CreateRectRgn
		// pRtSrc: uses the source clip to make rgn
		// bReverse: if set to TRUE, rgn equals to color key region, otherwise color key as tranparent region
		virtual HRGN CreateRgn(DWORD dwColorKey = DEFAULT_COLOR_KEY, int x = 0, int y = 0, RECT * pRtSrc = NULL, BOOL bReverse = FALSE) = 0;

		// if alpha channel is valid
		virtual BOOL IsAlphaChannelValid() = 0;

		// get the ARGB of specified coordinates
		virtual DWORD GetPixel(int x, int y) = 0;

		// set the ARGB of specified coordinates
		virtual BOOL SetPixel(int x, int y, DWORD dwColor) = 0;

		// get data buffer
		virtual BYTE * GetBits() = 0;

		// calculate the render rect by draw param
		// pSrc is a optional param to get the new source region
		virtual RECT CalculateRectByDrawParam(int x, int y, const DRAW_PARAM * pDp, RECT * pSrc = NULL) = 0;

		// set color key. the whole image will own alpha channel after this operation
		virtual BOOL SetColorKey(DWORD dwColor) = 0;

		// set hue, saturation, and luminance. -1 stands for unchanged.
		// h [0, 360], s [0, 255], l [0, 255]
		virtual BOOL SetHsl(int h = -1, int s = -1, int l = -1) = 0;

		// mirror by x
		virtual BOOL MirrorX() = 0;

		// mirror by y
		virtual BOOL MirrorY() = 0;

		// stretch
		virtual BOOL Stretch(int cx, int cy) = 0;

		// just extend without stretch
		virtual BOOL Extend(int cx = 0, int cy = 0) = 0;

		// get internal dc of image directly
		virtual HDC GetDC() = 0;

		// blur
		// nType:0 for gauss blur, 1 for even blur
		virtual BOOL Blur(int nType = 0) = 0;
	};

	//////////////////////////////////////////////////////////////////////////
	// interface:	ISonicPaint
	// function		paint can hold some objects. can accomplish dirty update mechanism easily
	//////////////////////////////////////////////////////////////////////////
	class ISonicPaint : public ISonicBase, public ISonicBaseContainer
	{
	public:
		// create
		// hMemDib:	if the paint obtain its own mem dc
		// cx, cy:	set to 0 to make a auto adjustment by objects added
		// hWnd:	window attached, can be specified later when drawing
		virtual BOOL Create(BOOL bMemDib, int cx = 0, int cy = 0, HWND hWnd = NULL) = 0;

		// draw
		virtual BOOL Draw(HDC hdc, int x = 0, int y = 0, HWND hWnd = NULL, RECT * pMemDCRect = NULL, int nAlpha = -1) = 0;

		// resize
		// cx, cy:	set to 0 to make a auto adjustment by objects added
		virtual BOOL Resize(int cx = 0, int cy = 0) = 0;

		// backup current data buffer, only available when this paint owns memdc
		virtual BOOL Backup() = 0;

		// restore data buffer from backup, only available when this paint owns memdc
		virtual BOOL Restore() = 0;

		// redraw
		virtual BOOL Redraw(BOOL bEraseBackground = TRUE) = 0;

		// get width
		virtual int GetWidth() = 0;

		// get heigth
		virtual int GetHeight() = 0;

		// get mem dc
		virtual HDC GetDC() = 0;

		// get mem data buffer
		virtual BYTE * GetBits() = 0;

		// get size of data buffer, width * height * 4
		virtual int GetSize() = 0;

		// get HBitmap of memdc
		virtual HBITMAP GetHBitmap() = 0;

		// fill memdc with specified color
		virtual BOOL Flush(DWORD dwColor) = 0;

		// return current paint rect
		virtual const RECT * GetPaintRect() = 0;

		// return current paint params
		virtual CURRENT_PAINT * GetCurrentPaint() = 0;

		// show or hide the paint
		virtual BOOL ShowPaint(BOOL b, BOOL bRedraw = TRUE) = 0;

		// visible
		virtual BOOL IsVisible() = 0;

		// return rect in window which responses to message
		virtual const RECT * GetMsgRect() = 0;

		// clean up current backup
		virtual BOOL ClearBackup() = 0;

		// if backup is valid
		virtual BOOL IsBackup() = 0;

		// copy from
		virtual BOOL CloneFrom(const ISonicPaint *) = 0;

		// turn on/off auto background backup mechanism when drawing
		virtual BOOL EnableDrawingBackup(BOOL b) = 0;
	};

	//////////////////////////////////////////////////////////////////////////
	// interface:	ISonicAnimation
	// function:	animation operation methods, such as rotation, stretch, tec
	//////////////////////////////////////////////////////////////////////////
	class ISonicAnimation : public ISonicBase, public ISonicBaseContainer, public ISonicBaseCtrl
	{
	public:
		// create an animation with specified region
		virtual BOOL Create(HWND hWnd, int x, int y, int cx, int cy) = 0;

		// move animation
		// nFrame: animation frames
		// bEvenSpeed: movement will be at an even speed if this is set to TRUE, otherwise will slow down smoothly
		virtual BOOL MoveGently(int x, int y, int nFrame, BOOL bEvenSpeed = FALSE) = 0;

		// fading out animation
		// nFrame: animation frames
		virtual BOOL ShowGently(BOOL bShow, int nFrame) = 0;

		// transform animation
		virtual BOOL Transform(BOOL b, int nAngle = 0, RECT * pRect = NULL, int nFrame = 0) = 0;

		// redraw
		virtual BOOL Redraw() = 0;

		// get current animation Status with enum enAnimationStatus
		virtual DWORD GetAnimationStatus() = 0;
	};

	//////////////////////////////////////////////////////////////////////////
	// interface:	ISonicWndEffect
	// function:	a effect engine for windows. you can make irregular windows, move or show or close the window with animation, or gray windows, 
	//				or make a snapshot of a window with this interface.
	//////////////////////////////////////////////////////////////////////////
	class ISonicWndEffect : public ISonicBase
	{
	public:
		// attach to a specified window, if already attached, will release the previous attaching relation.
		virtual BOOL Attach(HWND hWnd, BOOL bAlphaPerPixel = FALSE) = 0;

		// shading animation
		virtual BOOL ShowGently(BOOL bShow, BOOL bActivted = FALSE, int nStep = 0) = 0;

		// make a irregular window with a ISonicImage and color key
		virtual BOOL SetShapeByImage(ISonicImage * pImg, DWORD dwColorKey = -1) = 0;

		// moving animation
		virtual BOOL MoveGently(int x, int y, BOOL bEvenSpeed = FALSE) = 0;

		// set color key or transparency
		virtual BOOL SetLayeredAttributes(COLORREF crKey, BYTE bAlpha, DWORD dwFlag) = 0;

		// enable whole window dragment
		virtual BOOL EnableWholeDrag(BOOL b) = 0;

		// enable pre-drag shadow
		virtual BOOL EnablePreDrag(BOOL b) = 0;

		// enable whole window grayness
		virtual BOOL EnableGray(BOOL b) = 0;

		// create a fake child window
		virtual HWND CreateChildCtrl(LPCTSTR lpszClassName, LPCTSTR lpszText, DWORD dwStyle, int x, int y, int cx, int cy, int nAttach = -1) = 0;

		// convert a normal child window into a fake one.
		virtual BOOL ConvertChildCtrl(HWND hChild, int nAttach = -1) = 0;

		// enable window frame
		// bType:0 show and hide when mouse moves in and out; 2 show and hide by mouse movement and focus changes; 2 always show
		// bSideMask:0xf every side; 0x1 left; 0x2 top; 0x4 right; 0x8 bottom; can be combined
		// nSpace:currently not implemented
		virtual BOOL EnableFrame(BOOL b, BYTE bSideMask = 0xf, BYTE bType = 0, int nSpace = 0, int nAlphaSpeed = 10, 
			ISonicImage * pSide = NULL, ISonicImage * pCorner = NULL) = 0;

		// enable hsl effect
		// h [0, 360], s [0, 255], l [0, 255]
		virtual BOOL EnableHSLAdjust(BOOL b, int h = -1, int s = -1, int l = -1) = 0;

		// set background color
		virtual BOOL SetBkColor(DWORD dwColor) = 0;

		// enable transformation
		virtual BOOL EnableTransform(BOOL b, int nAngle = 0, RECT * pDest = NULL, int nFrame = 0) = 0;

		// direct transformation
		virtual BOOL DirectTransform(int nAngle, RECT * pDest, int nFrame) = 0;

		// sliding the window while showing or hiding
		virtual BOOL SlideShow(BOOL b, POINT ptFrom, POINT ptTo, int nFrame = 15, BOOL bActivted = FALSE) = 0;

		// make a snapshot of a window
		virtual ISonicImage * Snapshot() = 0;

		// enable mouse track with a timer
		virtual BOOL EnableTrackMouse(BOOL b) = 0;

		// get current effect type with enum enWndEffectMask
		virtual DWORD GetWndEffectType() = 0;

		// shutter alike
		virtual BOOL Shutter(BOOL bCross = TRUE) = 0;

		// is doing animation, including shutter, transformation, slide, fadeout
		virtual BOOL IsAnimating() = 0;
	};

	//////////////////////////////////////////////////////////////////////////
	// interface:	ISonicString
	// function:	format a string with key words, make the string act as a URL, a button, an image with message response ability, just like what the HTML does
	// keywords
	//			a		usage:	make a url
	//					exp:	/a=www.xxx.com/hello
	//
	//			c		usage:	set font color
	//					exp:	/c=0xffffff/hello
	//
	//			linkl	usage:	only available with 'a' keyword. 0 no underline under a url; 1 uderline when hover; 2 underline always
	//					exp:	/a=www.xxx.com, linkl=1/hello
	//
	//			linkc	usage:	only available with 'a' keyword. 0 cursor will not change when hover; 1 cursor will change to "HAND" style when hover
	//					exp:	/a=www.xxx.com, linkc=0/hello
	//
	//			linkh	usage:	only available with 'a' keyword. set color when hover on a url
	//					exp:	/a=www.xxx.com, linkh=0xff00ff/hello
	//
	//			linkt	usage:	show a tip when hover
	//					exp:	/linkt='hello tip'/hello world!
	//
	//		btn_type	usage:	only available with 'a' keyword. 0 normal button, 1 check button, 2 quick button
	//
	//		pass_msg	usage:	mouse message will pass through the object to the window beneath
	//					exp:	/a, pass_msg/你好
	//
	//			p		usage:	display a image, auto switch frame if a gif is set. set a ISonicImage object id to it 
	//					exp:	Format("/p=%d//c=0xff/你好啊", pImg->GetObjectId());
	//
	//			p%d		usage:	only available with 'a' keywords. make a %d-state button, %d can be 2, 3, 4, you can assign %d different pictures or one picture as 
	//							param to p%d, and if only one picture is assigned, the picture would be split into %d pieces internally
	//					exp:	/a, p3=%d|%d|%d/, pNormal->GetObjectId(), pHover->GetObjectId(), pClick->GetObjectId()
	//							/a, p3=%d/pButton->GetObjectId()
	//
	//		btn_text	usage:	only available with 'a', 'p', and 'p%d' keywords. output text on the button, if the length of text exceeds the suitable length of the button,
	//							auto button length extention would be made internally
	//					exp:	/a, p3=%d, btn_text='确定'/pButton->GetObjectId()
	//
	//		btn_width	usage:	only available with 'a', 'p', and 'p%d' keywords. force button width to the specified value.
	//					exp:	/a, p3=%d, btn_width=40/pButton->GetObjectId()
	//		btn_height	usage:	only available with 'a', 'p', and 'p%d' keywords. force button height to the specified value.
	//					exp:	/a, p3=%d, btn_height=30/pButton->GetObjectId()
	//
	//		animation	usage:	only available with 'a', 'p', 'ph' and 'pc' keywords. make a animation button with specified speed
	//					exp:	Format("/a, p=%d, ph=%d, pc=%d, animation=30/", pNormal->GetObjectId(), pHover->GetObjectId(), pClick->GetObjectId());
	//
	//			key		usage:	only available with 'p' keyword. set color key of a image
	//					exp:	Format("/key=0x808080, p=%d/", pImg->GetObjectId());
	//
	//			pclip	usage:	only available with 'p' keyword. source image clip
	//					exp:	Format("/p=%d, pclip=0|0|15|15/", pImg->GetObjectId());
	//
	//			def		usage:	use a default font color and style
	//					exp:	/def/你好
	//
	//		line_width	usage:	set line width, text will make a newline while exceeding the width
	//					exp:	/line_width=50//a=www.xxx.com/你好吗，你好吗
	//
	//		line_space	usage:	space between lines
	//					exp:	/line_space=5/你好吗
	//
	//		single_line	usage:	only available with 'line_width' keyword. force a single line format. 0 disable single line. 
	//							1.clip when exceeding; 2 clip when exceeding, and add "..." to make an end ellipsis, and display whole info when hover
	//					exp:	/line_width=30, single_line=2//a=www.xxx.com/我们都有一个家，名字叫中国
	//
	//			align	usage:	line aligning mode. 0 for top alignment, 1 for middle ,2 for bottom
	//					exp:	Format("/align=2/你好吗/p=%d/", pImg->GetObjectId());
	//
	//		br_offset	usage:	offset of a newline beginning
	//					exp:	/br_offset=30/你好\n你好
	//
	//			font	usage:	set font style
	//		font_height	usage:	only available with 'font' keyword. change font size
	//		font_bold	usage:	only available with 'font' keyword. set bold
	//		font_italic	usage:	only available with 'font' keyword. set italic
	//	font_strikeout	usage:	only available with 'font' keyword. set strikeout
	//		font_face	usage:	only available with 'font' keyword. set font face.
	//					exp:	/font, font_height=16, font_italic=1, font_face=隶书/你好/def/你好/a=www.xxx.com/你好
	//
	//			sparkle	usage:	sparkle effect
	//	sparkle_color	usage:	only available with 'sparkle' keyword. sparkle color. three kinds.
	// sparkle_interval	usage:	only available with 'sparkle' keyword. sparkle color. sparkle interval, default as 500 ms
	//	sparkle_timeout	usage:	only available with 'sparkle' keyword. set sparkle timeout.
	//					exp:	/sparkle, sparkle_color=0xff|0xff00|0xff0000, sparkle_timeout=40/你好吗
	//
	//			fadeout	usage:	fadeout effect
	//		fadeout_max	usage:	only available with 'fadeout' keyword. fadeout max value
	//		fadeout_min	usage:	only available with 'fadeout' keyword. fadeout min value
	//	fadeout_speed	usage:	only available with 'fadeout' keyword. fadeout speed
	//					exp:	/fadeout, fadeout_speed=10/你好吗
	//
	//			global	usage:	global attributes. when '/def/' is used, global attributes will be adopted
	//					exp:	/global, c=0xff, font, font_height=15/你好
	//
	//			ani_num	usage:	make a animation number effect
	//	ani_num_from	usage:	only available with 'ani_num' keyword. number from
	//		ani_num_to	usage:	only available with 'ani_num' keyword. number to
	//	ani_num_frame	usage:	only available with 'ani_num' keyword. animation frames
	//					exp:	/ani_num, ani_num_from=0, ani_num_to=3000, ani_num_frame=30/
	//
	//		init_normal	usage:	don't act special effect except call AnimateXXX() manually
	//					exp:	/fadeout, init_normal/hello world!
	//////////////////////////////////////////////////////////////////////////
	class ISonicString : public ISonicBase
	{
	public:		
		// format like the CString
		virtual BOOL Format(LPCTSTR lpszArg, ...) = 0;

		// render the text, please remember to specify a hWnd if the hdc is a memory one
		virtual BOOL TextOut(HDC hdc, int x, int y, HWND hWnd = NULL, RECT * pMemDCRect = NULL) = 0;

		// get height
		virtual int GetHeight() = 0;

		// get width
		virtual int GetWidth() = 0;

		// turn on/off gif animation
		virtual BOOL AnimateGif(BOOL bBegin = TRUE) = 0;

		// turn on/off sparkle
		virtual BOOL AnimateSparkle(BOOL b = TRUE) = 0;

		// detach from current window
		virtual BOOL Detach() = 0;

		// get raw string
		virtual LPCTSTR GetStr() = 0;

		// get text without control charactors
		virtual LPCTSTR GetTextWithoutCtrl() = 0;

		// show or hide string
		virtual void ShowString(BOOL bShow = TRUE, BOOL bRedraw = TRUE) = 0;

		// is visible
		virtual BOOL IsVisible() = 0;

		// redraw
		virtual void Redraw(BOOL bEraseBackground = TRUE) = 0;

		// turn on/off fadeout animation
		virtual BOOL AnimateFadeout(BOOL b = TRUE) = 0;

		// turn on/off number animation
		virtual BOOL AnimateAniNum(BOOL b = TRUE) = 0;

		// get current rendering region
		virtual const RECT * GetRect() = 0;

		// if disabled, all mouse message will be skiped. if asigned with 4-state picture and act as a button, set disabled attribute to show disabled button state
		virtual void Enable(BOOL b = TRUE) = 0;

		// is enabled
		virtual BOOL IsEnabled() = 0;
	};

	//////////////////////////////////////////////////////////////////////////
	// interface:	ISonicTextScrollBar
	// function:	Text strcoll bar control, used for scroll text or images
	// remarks:	text strcoll bar is not a real window
	//////////////////////////////////////////////////////////////////////////
	class ISonicTextScrollBar : public ISonicBase, public ISonicBaseCtrl
	{
	public:
		// create a text scroll bar
		// nSpeed:		scroll speed
		// nMidPause:	pause a specified time if the text is in center of the bar
		virtual BOOL Create(HWND hWnd, int x, int y, int cx, int cy, BOOL bVerScroll = TRUE, int nSpeed = 50, int nMidPause = 0) = 0;

		// add a ISonicImage or ISonicString or ISonicPaint object
		virtual BOOL AddText(DWORD dwObjectId) = 0;

		// delete a ISonicImage or ISonicString or ISonicPaint object
		// bDestroy:	if true the object will be destroyed, otherwise just delete the relation between the object and scroll bar
		virtual BOOL DelText(int nIndex, BOOL bDestoryObject = FALSE) = 0;

		// turn on/off scroll 
        virtual BOOL DoScroll(BOOL bScroll = TRUE) = 0;

		// set a new position
		virtual BOOL Move(int x, int y, int cx, int cy) = 0;

		// enable extra option
		virtual BOOL EnableOpt(DWORD dwOpt) = 0;

		// get current count
		virtual int GetCount() = 0;

		// show/hide scroll bar
		virtual BOOL ShowScroll(BOOL b, BOOL bRedraw = TRUE) = 0;

		//set color of scroll bar
		virtual BOOL SetBGColor(DWORD dwColor) = 0;
	};

	//////////////////////////////////////////////////////////////////////////
	// interface:	ISonicSkin
	// function:	subclass mechanism to change skin
	// remarks:		while formating lpszValue, use colon to set value and semicolon to seperate different values
	//				it's highly recommended that attach ISonicSkin a window before it's visible for more smooth UI sense.
	//				it's free to attach ISonicSkin to normal windows with kinds of style, but when face to dialog, you should 
	//				modify dialog style in the template editor. style modification after dialogs are created will cause weird
	//				action, especially in modal dialogs. for example, if modify style to WS_MINIMIZE or WS_SIZEBOX within WM_INITDIALOG
	//				process, modification could be confirmed with spy++ but the new style will not function at all.
	//				so, if attach ISonicSkin to a dialog, you should prepare a none-border dialog with the template editor. if more optional
	//				styles wanted, you will have to edit the .rc file manually
	//////////////////////////////////////////////////////////////////////////
	class ISonicSkin : public ISonicBase
	{
	public:
		// make sure pImg passed in is valid while working time
		// skin type description:
		//////////////////////////////////////////////////////////////////////////
		// Skin Name: bg
		// Description: background render
		// Value:	image: specify background image
		//			top_height:	top region will not be tiled vertically whiling rendering background
		//			title_height: specify the title area in the background image, if not specified, auto judged internally
		//			title_text:	specify the title text, if not specified
		//			enable_drag: set to false to disable window drag, default to TRUE
		//			color_key: specify the transparent color to make window rgn	
		//			close: system close button, 4-state tile image
		//			min: system minimize button, 4-state tile image
		//			max: system maximize button, 4-state tile image
		//			restore: system restore buttn, 4-state tile image
		//			icon: system icon, image
		//			btn_space: width between system buttons
		//			btn_top_margin: top margin of system buttons
		//			btn_right_margin: right margin of system buttons
		//////////////////////////////////////////////////////////////////////////
		// Skin Name: button
		// Description: self draw button, change the standard button skin
		// Value:	image: specify button image, 4-state tiled
		//			text_ctrl: specify text color and font, refer to ISonicString syntax, '/' is not needed
		//			btn_ctrl: specify button attributes, such as animation button, cursor while hovering, etc. refer to ISonicString syntax, '/' is not needed
		//////////////////////////////////////////////////////////////////////////
		// Exp:		SetSkin("bg", "image:%d;title_height:1;close:%d", pBG->GetObjectId(), pClose->GetObjectId());
		//////////////////////////////////////////////////////////////////////////
		virtual BOOL SetSkin(LPCTSTR lpszSkinName, LPCTSTR lpszValue, ...) = 0;

		// window skin would be changed after attaching
		// bLayered: if set to TRUE, use layered window to implement shaped window, otherwise use window rgn
		virtual BOOL Attach(HWND hWnd) = 0;

		// get paint of ISonicSkin
		// you can assign a callback function to the paint retreived, which will be called just after the background has been rendered
		// another advantage of this mechanism is that ficker can be avoided because memory dc is used when render the background
		// exp: pSonicSkin->GetPaint()->Delegate(DELEGATE_EVENT_PAINT, NULL, NULL, DrawCallback);
		virtual ISonicPaint * GetPaint() = 0;
	};

	//////////////////////////////////////////////////////////////////////////
	// interface:	ISonicUI
	// function:	object factory and manager, arrange the objects
	//////////////////////////////////////////////////////////////////////////
	class ISonicUI
	{
	public:
		// create ISonicSkin
		virtual ISonicSkin * CreateSkin() = 0;

		// create ISonicImage
		virtual ISonicImage * CreateImage() = 0;

		// create ISonicWndEffect
		virtual ISonicWndEffect * CreateWndEffect() = 0;

		// create ISonicString, one string can be used only in one window
		virtual ISonicString * CreateString() = 0;

		// create a text scroll bar
		virtual ISonicTextScrollBar * CreateTextScrollBar() = 0;

		// destroy object, don't worry about repeated destruction because of internal memory check
		virtual void DestroyObject(ISonicBase *) = 0;

		// destroy all objects attached to a specified window
		virtual void DestroyAttachedObject(HWND hWnd) = 0;

		// judge whether a object is valid
		virtual BOOL IsValidObject(const ISonicBase *) = 0;

		// destroy all objects in current process
		virtual void DestroyAllObject() = 0;

		// get a ISonicWndEffect from a specified window
		virtual ISonicWndEffect * EffectFromHwnd(HWND hWnd) = 0;

		// get a ISonicSkin from a specified window
		virtual ISonicSkin * SkinFromHwnd(HWND hWnd) = 0;

		// handle raw strings to avoid keywords confict
		virtual LPCTSTR HandleRawString(LPCTSTR lpszStr, int nType, LPCTSTR lpszUrlAttr = NULL) = 0;

		// create a ISonicPaint
		virtual ISonicPaint * CreatePaint() = 0;

		// create a ISonicAnimation
		virtual ISonicAnimation * CreateAnimation() = 0;

		// return a default HFONT with following style:font-family:宋体，font-size:12
		// !!!Don't delete the handle returned!!!
		virtual HFONT GetDefaultFont() = 0;

		// simple draw text
		virtual BOOL DrawText(HDC hDC, int x, int y, LPCTSTR lpszString, DWORD dwColor = 0, HFONT hFont = NULL) = 0;

		// draw a snapshot of window to the specified hdc
		virtual BOOL DrawWindow(HDC hdc, HWND hWnd, BOOL bRecursion = TRUE) = 0;

	protected:
		virtual const DWORD * GetObjectTypePtr(const ISonicBase *);
	};
};

extern "C" SONICUI_API sonic_ui::ISonicUI * GetSonicUI();
typedef sonic_ui::ISonicUI * (*FUNC_GET_SONIC_UI)();