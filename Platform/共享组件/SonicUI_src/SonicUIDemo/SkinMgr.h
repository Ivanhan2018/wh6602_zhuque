#pragma once

class CSkinMgr
{
public:
	CSkinMgr(void);
	~CSkinMgr(void);

	void Init();

	ISonicImage * pImgNormal;
	ISonicImage * pImgHover;
	ISonicImage * pImgClick;
	ISonicImage * pImgGif;
	ISonicImage * pImgCard;
	ISonicImage * pImgButton;
	ISonicImage * pImgDlg;
	ISonicImage * pImgClose;
};


extern CSkinMgr _SkinMgr;