
#include "StdAfx.h"
#include "DxSkin.h"

//////////////////////////////////////////////////////////////////////////////////////

//常量定义
#define LESS_MOVE_COUNT				10									//移动次数


//静态变量
ID3DXSprite*				CDxSkin::m_Sprite=NULL;							//精灵
WORD						CDxSkin::m_wSpriteRefCount=0;					//引用数目
//////////////////////////////////////////////////////////////////////////////////////

#include "GameClientDlg.h"

//CGameClientDlg	* g_GameClientDlg=NULL;

//构造函数
CDxSkin::CDxSkin()
{
	//运行变量
	m_Sprite=NULL;
	for(WORD i=0;i<MAX_TEXTURE_COUNT;i++)
	{
		m_pTexture[i]=NULL;
		m_cbAlphaValue[i]=0xff;
		m_csImageSize[i].SetSize(0,0);
		m_ptTarger[i].SetPoint(0,0);
	}
	ZeroMemory(&m_ImageInfo,sizeof(m_ImageInfo));

	//设置变量
	m_bFistLoad=true;
}

//析构函数
CDxSkin::~CDxSkin()
{
	Release();
}

//释放函数
void CDxSkin::Release()
{
	//释放精灵
	if (m_wSpriteRefCount>0) --m_wSpriteRefCount;
	if (0==m_wSpriteRefCount) SAFE_RELEASE(m_Sprite);

	//释放纹理
	for(WORD i=0;i<MAX_TEXTURE_COUNT;i++)
		SAFE_RELEASE(m_pTexture[i]);

	//设置变量
	m_bFistLoad=true;
}

//设置透明
void CDxSkin::SetAlphaBlend(BYTE bAlphaValue,WORD wImageIndex)
{
	ASSERT(wImageIndex<MAX_TEXTURE_COUNT);
	if(wImageIndex>=MAX_TEXTURE_COUNT)return;

	m_cbAlphaValue[wImageIndex]=bAlphaValue;

	return;
}

//设置位置
void CDxSkin::SetDrawPos(INT nXpost,INT nYpost,WORD wImageIndex)
{	
	ASSERT(wImageIndex<MAX_TEXTURE_COUNT);
	if(wImageIndex>=MAX_TEXTURE_COUNT)return;

	m_ptTarger[wImageIndex].x=nXpost;
	m_ptTarger[wImageIndex].y=nYpost;

	return;
}

//设置大小
void CDxSkin::SetImageSize(INT nCx,INT nCy,WORD wImageIndex)
{
	ASSERT(wImageIndex<MAX_TEXTURE_COUNT);
	if(wImageIndex>=MAX_TEXTURE_COUNT)return;

	m_csImageSize[wImageIndex].SetSize(nCx,nCy);

	return;
}

//图片信息
void CDxSkin::SetImageInfo(UINT uResourceID,WORD wImageIndex)
{
	ASSERT(wImageIndex<MAX_TEXTURE_COUNT);
	if(wImageIndex>=MAX_TEXTURE_COUNT)return;

	//获取信息
	D3DXGetImageInfoFromResource(GetModuleHandle(NULL),MAKEINTRESOURCE(uResourceID),&m_ImageInfo[wImageIndex]);

	//常规大小
	SetImageSize(m_ImageInfo[wImageIndex].Width,m_ImageInfo[wImageIndex].Height);

	return;
}

//加载函数(按文件名)
void CDxSkin::SetLoadInfo(LPDIRECT3DDEVICE9	pD3DDevice,LPCTSTR szFileName,WORD wImageIndex,D3DPOOL Pool, D3DCOLOR dwKeyColor)
{
	ASSERT(wImageIndex<MAX_TEXTURE_COUNT);
	if(wImageIndex>=MAX_TEXTURE_COUNT)return;

//#ifdef _DEBUG
	//if (NULL!=g_GameClientDlg) g_GameClientDlg->InsertSystemString(szFileName);
//#endif

	//创建精灵
	if (NULL==m_Sprite) D3DXCreateSprite( pD3DDevice, &m_Sprite);

	//首次判断
	if (true==m_bFistLoad)
	{
		m_bFistLoad=false;
		++m_wSpriteRefCount;
	}

	//加载纹理
	HRESULT hr=D3DXCreateTextureFromFileEx(pD3DDevice,	szFileName, 0, 0, 0, 0,
		D3DFMT_UNKNOWN, Pool, D3DX_FILTER_NONE,D3DX_DEFAULT,	dwKeyColor, NULL, NULL,
 		&m_pTexture[wImageIndex]);

	//成功判断
	if (D3D_OK!=hr && D3DPOOL_DEFAULT==Pool)
	{
		D3DXCreateTextureFromFileEx(pD3DDevice,	szFileName, 0, 0, 0, 0,
			D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_FILTER_NONE,D3DX_DEFAULT,	dwKeyColor, NULL, NULL,
		&m_pTexture[wImageIndex]);
	}

	if(m_Sprite==NULL||m_pTexture[wImageIndex]==NULL) throw 0 ;

	//获取信息
	D3DXGetImageInfoFromFile(szFileName, &m_ImageInfo[wImageIndex]);

	//常规大小
	SetImageSize(m_ImageInfo[wImageIndex].Width,m_ImageInfo[wImageIndex].Height,wImageIndex);

	return;
}

//加载函数(按资源ID)
void CDxSkin::SetLoadInfo(LPDIRECT3DDEVICE9	pD3DDevice,UINT uResourceID,WORD wImageIndex,D3DPOOL Pool,D3DCOLOR dwKeyColor)
{
	ASSERT(wImageIndex<MAX_TEXTURE_COUNT);
	if(wImageIndex>=MAX_TEXTURE_COUNT)return;

	//创建精灵
	if (NULL==m_Sprite) D3DXCreateSprite( pD3DDevice, &m_Sprite);

	//首次判断
	if (true==m_bFistLoad)
	{
		m_bFistLoad=false;
		++m_wSpriteRefCount;
	}

	//加载纹理
	HRESULT hr=D3DXCreateTextureFromResourceEx(pD3DDevice,	NULL,MAKEINTRESOURCE(uResourceID), 0, 0, 0, 0,
		D3DFMT_UNKNOWN, Pool, D3DX_FILTER_NONE,D3DX_DEFAULT,	dwKeyColor, NULL, NULL,
		&m_pTexture[wImageIndex]);
	
	//成功判断
	if (D3D_OK!=hr && D3DPOOL_DEFAULT==Pool)
	{
		D3DXCreateTextureFromResourceEx(pD3DDevice,	NULL,MAKEINTRESOURCE(uResourceID), 0, 0, 0, 0,
		D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_FILTER_NONE,D3DX_DEFAULT,	dwKeyColor, NULL, NULL,
		&m_pTexture[wImageIndex]);
	}

	if(m_Sprite==NULL||m_pTexture[wImageIndex]==NULL) throw 0 ;

	//常规大小
	SetImageSize(m_ImageInfo[wImageIndex].Width,m_ImageInfo[wImageIndex].Height);
}

//移动动画
void CDxSkin::OnDrawImage(WORD wImageIndex)
{
	ASSERT(wImageIndex<MAX_TEXTURE_COUNT);
	if(wImageIndex>=MAX_TEXTURE_COUNT)return;

	if(m_Sprite==NULL||m_pTexture[wImageIndex]==NULL) return;

	//位置信息
	INT x=m_ptTarger[wImageIndex].x;
	INT y=m_ptTarger[wImageIndex].y;
	CRect drawRect(0,0,m_csImageSize[wImageIndex].cx,m_csImageSize[wImageIndex].cy);

	//设置参数
	D3DXMATRIX matRot;D3DXMatrixIdentity(&matRot);//矩阵

	//最后的参数是图片的绘制位置
	float fCenterX=float(drawRect.right-drawRect.left)/2;
	float fCenterY=float(drawRect.bottom-drawRect.top)/2;
	D3DXMatrixTransformation2D(	&matRot,0,0,0,&D3DXVECTOR2(fCenterX,fCenterY),0.0f,&D3DXVECTOR2((float)x,(float)y));

	//绘画图片
	DrawSprite(0,0,D3DXVECTOR3(0,0,0),drawRect,wImageIndex,&matRot);

	return ;
}

//绘画函数
void CDxSkin::DrawSprite(INT x,INT y,CRect &drawRect,WORD wImageIndex,D3DCOLOR channelColor)
{
	//合法判断
	if (m_Sprite==NULL||m_pTexture==NULL) return ;
	ASSERT(wImageIndex<MAX_TEXTURE_COUNT);
	if(wImageIndex>=MAX_TEXTURE_COUNT)return;

	////状态检测
	//LPDIRECT3DDEVICE9 pD3DDevice=NULL;
	//m_Sprite->GetDevice(&pD3DDevice);
	//if (NULL!=pD3DDevice)
	//{
	//	HRESULT hResult=pD3DDevice->TestCooperativeLevel();
	//	if (hResult==D3DERR_DEVICELOST || hResult==D3DERR_DEVICENOTRESET) return ;
	//}

	//设置透明
	D3DCOLOR color = (m_cbAlphaValue[wImageIndex]&255)<<24|channelColor;

	//设置参数
	D3DXMATRIX matRot;D3DXMatrixIdentity(&matRot);//矩阵

	//绘画当前帧
	D3DXVECTOR3 drawAtPoint((float)x,(float)y,0);
 	//m_Sprite->Begin(D3DXSPRITE_ALPHABLEND|D3DXSPRITE_SORT_TEXTURE);
	m_Sprite->SetTransform(&matRot);
	m_Sprite->Draw(m_pTexture[wImageIndex], drawRect, NULL, &drawAtPoint, color);
	m_Sprite->Flush();
	//m_Sprite->End();

	return ;
}

//绘画函数
void CDxSkin::DrawSprite(INT x,INT y,D3DXVECTOR3 &vCenterPoint,CRect &drawRect,WORD wImageIndex,D3DXMATRIX *matRot,D3DCOLOR channelColor)
{
	//合法判断
	if (m_Sprite==NULL||m_pTexture==NULL) return ;
	ASSERT(wImageIndex<MAX_TEXTURE_COUNT);
	if(wImageIndex>=MAX_TEXTURE_COUNT)return;

	////状态检测
	//LPDIRECT3DDEVICE9 pD3DDevice=NULL;
	//m_Sprite->GetDevice(&pD3DDevice);
	//if (NULL!=pD3DDevice)
	//{
	//	HRESULT hResult=pD3DDevice->TestCooperativeLevel();
	//	if (hResult==D3DERR_DEVICELOST || hResult==D3DERR_DEVICENOTRESET) return ;
	//}

	//设置透明
	D3DCOLOR color = (m_cbAlphaValue[wImageIndex]&255)<<24|channelColor;

	//绘画位置
	D3DXVECTOR3 drawAtPoint((float)x,(float)y,0);
	//m_Sprite->Begin(D3DXSPRITE_ALPHABLEND|D3DXSPRITE_SORT_TEXTURE);
	m_Sprite->SetTransform(matRot);
	m_Sprite->Draw(m_pTexture[wImageIndex], drawRect, &vCenterPoint, &drawAtPoint, color);
	m_Sprite->Flush();
	//m_Sprite->End();

	return ;	
}

//旋转缩放
bool CDxSkin::DrawRotateAndZoom(INT x,INT y,CRect &drawRect,float fCurrentAngle,float fXScaling,float fYScaling,WORD wImageIndex,enXCollocateMode XCollocateMode, enYCollocateMode YCollocateMode)
{
	//设置参数
	D3DXMATRIX matRot;D3DXMatrixIdentity(&matRot);//矩阵

	//最后的参数是图片的绘制位置
	float fCenterX=float(drawRect.right-drawRect.left)/2;
	float fCenterY=float(drawRect.bottom-drawRect.top)/2;
	D3DXMatrixTransformation2D(&matRot,&D3DXVECTOR2(fCenterX,fCenterY),0,&D3DXVECTOR2(fXScaling,fYScaling),&D3DXVECTOR2(0,0),fCurrentAngle,&D3DXVECTOR2((float)x,(float)y));

	//绘画图片
	float fXCollocate=0;
	float fYCollocate=0;
	if (XCollocateMode==enXLeft) fXCollocate=0.f;
	else if (XCollocateMode==enXRight) fXCollocate=float(drawRect.Width());
	else fXCollocate=fCenterX;

	if (YCollocateMode==enYTop) fYCollocate=0.f;
	else if (YCollocateMode==enYBottom) fYCollocate=float(drawRect.Height());
	else fYCollocate=fCenterY;

	DrawSprite(0,0,D3DXVECTOR3(fXCollocate,fYCollocate,0),drawRect,wImageIndex,&matRot);

	return true;
}

//旋转缩放
bool CDxSkin::DrawRotateAndZoom(INT x,INT y,CRect &drawRect,float fCurrentAngle,int nRoteCenterX,int nRoteCenterY,float fXScaling,float fYScaling,WORD wImageIndex)
{
	//设置参数
	D3DXMATRIX matRot;D3DXMatrixIdentity(&matRot);//矩阵

	//最后的参数是图片的绘制位置
	float fCenterX=float(drawRect.right-drawRect.left)/2;
	float fCenterY=float(drawRect.bottom-drawRect.top)/2;
	D3DXMatrixTransformation2D(&matRot,&D3DXVECTOR2(fCenterX,fCenterY),0,&D3DXVECTOR2(fXScaling,fYScaling),&D3DXVECTOR2(0,0),fCurrentAngle,&D3DXVECTOR2((float)x,(float)y));

	//绘画图片
	DrawSprite(0,0,D3DXVECTOR3(float(nRoteCenterX),float(nRoteCenterY),0),drawRect,wImageIndex,&matRot);

	return true;
}

//旋转缩放
bool CDxSkin::DrawRotateAndZoom(INT x,INT y,CRect &drawRect,float fCurrentAngle,float fXScaling,float fYScaling,WORD wImageIndex,enXCollocateMode XCollocateMode, enYCollocateMode YCollocateMode,D3DCOLOR channelColor)
{
	//设置参数
	D3DXMATRIX matRot;D3DXMatrixIdentity(&matRot);//矩阵

	//最后的参数是图片的绘制位置
	float fCenterX=float(drawRect.right-drawRect.left)/2;
	float fCenterY=float(drawRect.bottom-drawRect.top)/2;
	D3DXMatrixTransformation2D(&matRot,&D3DXVECTOR2(fCenterX,fCenterY),0,&D3DXVECTOR2(fXScaling,fYScaling),&D3DXVECTOR2(0,0),fCurrentAngle,&D3DXVECTOR2((float)x,(float)y));

	//绘画图片
	float fXCollocate=0;
	float fYCollocate=0;
	if (XCollocateMode==enXLeft) fXCollocate=0.f;
	else if (XCollocateMode==enXRight) fXCollocate=float(drawRect.Width());
	else fXCollocate=fCenterX;

	if (YCollocateMode==enYTop) fYCollocate=0.f;
	else if (YCollocateMode==enYBottom) fYCollocate=float(drawRect.Height());
	else fYCollocate=fCenterY;

	DrawSprite(0,0,D3DXVECTOR3(fXCollocate,fYCollocate,0),drawRect,wImageIndex,&matRot,channelColor);

	return true;
}
//HRESULT Draw(
//　　LPDIRECT3DTEXTURE9 pTexture,
//　　CONST RECT *pSrcRect,
//　　CONST D3DXVECTOR3 *pCenter,
//　　CONST D3DXVECTOR3 *pPosition,
//　　D3DCOLOR Color );
//
//pTexture 是需要绘制的贴图
//pSrcRect 是需要绘制的贴图上的一个矩形区域, 绘制整过贴图可以指定为NULL.
//pCenter 是绘制的中心座标(旋转时会以此点为中心), 指定NULL表示中心座标为(0,0,0)
//pPosition 是绘制的位置座标, 也可以指定NULL表示(0,0,0)
//Color 是绘制的颜色, 一般情况下指定为 0xffffffff. 最高位到底位的各8字节为Alpha, 红, 绿, 蓝, 如果指定 0x80ffffff就是半透明贴图. 
//如果0xffff0000就只保留贴图里的红色分量, 具体功能自己体会. 当然贴图本身可以包含Alpha信息.
//需要旋转等功能可以使用 pSprite->SetTransform(), 函数原型为:
//HRESULT SetTransform( CONST D3DXMATRIX *pTransform );

//////////////////////////////////////////////////////////////////////////////////////

//以下为使用示例  平面轮盘的旋转
////平面轮盘
//D3DXMATRIX matRot;
//D3DXMatrixIdentity(&matRot);
//float   iAngle=0;
//if((INT)m_Ball_MoveRadius!=WIN_RADIUS&&m_bWinNumber==255) //旋转轮盘
//{
//	iAngle=1.5f*timeGetTime()/500.0f;   
//}
//else if(m_bWinNumber!=255)	//转到胜利数字
//{
//	for(INT i=0;i<37;i++) if(m_bWinNumber==numberArrayIndex[i]) iAngle=i*(2*D3DX_PI/37);;
//}
//D3DXMatrixTransformation2D(	&matRot,0,0,0,&D3DXVECTOR2((float)m_PlatePlane.GetWidth()/2,(float)m_PlatePlane.GetHeight()/2),iAngle,&D3DXVECTOR2((float)m_nScreenWidth/2+309-m_PlatePlane.GetWidth()/2,(float)m_nScreenHeight/2-68-m_PlatePlane.GetHeight()/2));
//m_PlatePlane.DrawSprite(0,0,D3DXVECTOR3(0,0,0), CRect(0,0,584,584),&matRot);
//


