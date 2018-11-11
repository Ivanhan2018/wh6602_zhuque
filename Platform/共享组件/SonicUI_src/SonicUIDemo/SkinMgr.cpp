#include "StdAfx.h"
#include "resource.h"
#include ".\skinmgr.h"

CSkinMgr _SkinMgr;

CSkinMgr::CSkinMgr(void)
{
}

CSkinMgr::~CSkinMgr(void)
{
}

void CSkinMgr::Init()
{
	pImgNormal = GetSonicUI()->CreateImage();
	pImgNormal->Load(BMP_NORMAL);
	pImgNormal->SetColorKey(RGB(255, 0, 255));

	pImgHover = GetSonicUI()->CreateImage();
	pImgHover->Load(BMP_HOVER);
	pImgHover->SetColorKey(RGB(255, 0, 255));

	pImgClick = GetSonicUI()->CreateImage();
	pImgClick->Load(BMP_CLICK);
	pImgClick->SetColorKey(RGB(255, 0, 255));

	pImgGif = GetSonicUI()->CreateImage();
	pImgGif->Load(GIF_TEST1);

	pImgCard = GetSonicUI()->CreateImage();
	pImgCard->Load(BMP_CLICK);

	pImgButton = GetSonicUI()->CreateImage();
	pImgButton->Load(PNG_BUTTON);

	pImgDlg = GetSonicUI()->CreateImage();
	pImgDlg->Load(PNG_DLG_BG);
	pImgDlg->DestroyAlphaChannel();

	pImgClose = GetSonicUI()->CreateImage();
	pImgClose->Load(PNG_CLOSE);
}
