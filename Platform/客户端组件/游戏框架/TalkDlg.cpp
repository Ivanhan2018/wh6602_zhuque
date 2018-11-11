// TalkDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "TalkDlg.h"
#include ".\talkdlg.h"
#include "GameFrameDlg.h"

#include "MemDC.h"

#define TALK_SHOW_TIME		2001
#define TALK_SET_POINT		2002
// CTalkDlg 对话框

IMPLEMENT_DYNAMIC(CTalkDlg, CDialog)
CTalkDlg::CTalkDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTalkDlg::IDD, pParent)
{
	m_strTextInfo = "";
	m_btDirectionTag = 3;
	m_btTalkSkin = 1;
}

CTalkDlg::~CTalkDlg()
{
}

void CTalkDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CTalkDlg, CDialog)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_MOUSEMOVE()
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CTalkDlg 消息处理程序

BOOL CTalkDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	// TODO:  在此添加额外的初始化

	SetWindowLong(GetSafeHwnd(), GWL_EXSTYLE, GetWindowLong(GetSafeHwnd(), GWL_EXSTYLE)|0x80000);
	typedef BOOL (WINAPI * FSetLayeredWindowAttributes)(HWND, COLORREF, BYTE, DWORD);
	FSetLayeredWindowAttributes	SetLayeredWindowAttributes;
	HINSTANCE hInst = LoadLibrary("User32.dll");
	SetLayeredWindowAttributes = (FSetLayeredWindowAttributes)GetProcAddress(hInst, "SetLayeredWindowAttributes");
	// -- 128 透明度  最后一个参数 1:透明一种颜色, 2: 窗体透明
	if (SetLayeredWindowAttributes) SetLayeredWindowAttributes(GetSafeHwnd(), RGB(255, 0, 255), 200, 2);		
	FreeLibrary(hInst); 
	
	//创建缓冲图
	if (m_ImagePic.IsNull())
	{
		m_ImagePic.Create(149, 103, 32);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

BOOL CTalkDlg::SetImageRgn(CImage * pImage)
{
    if (pImage->IsNull()) return FALSE;
    WORD wWidth = pImage->GetWidth();
    WORD wHeight = pImage->GetHeight();
    CBitmap * pbmp;
    CImage imgBuf;
    imgBuf.Create(wWidth, wHeight, 32);

    CDC *pDCTmp = CDC::FromHandle(imgBuf.GetDC());

	pImage->Draw(pDCTmp->m_hDC, 0, 0, wWidth, wHeight, 0, 0, wWidth, wHeight);
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
    
	SafeDeleteArray(pData);

	BOOL bResult = TRUE;
    if (rgnImage.m_hObject)
	{
		SetWindowRgn((HRGN)rgnImage.m_hObject, TRUE);
		DeleteObject(rgnImage);
	}
	else
	{
		bResult = FALSE;
	}
	
	CDC::DeleteTempMap();	// add by HouGuoJiang 2011-11-25

    return bResult;
} 

BOOL CTalkDlg::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	return true;
	return CDialog::OnEraseBkgnd(pDC);
}

void CTalkDlg::SetTalkDlgPos(int iwx, int iwy, int ix, int iy)
{
	m_ptWndPoint.x = iwx;
	m_ptWndPoint.y = iwy;
	m_ptSavePoint.x = ix;
	m_ptSavePoint.y = iy;
	//设置聊天框朝向
	if (m_ptSavePoint.x<0 || m_ptSavePoint.y<0)
	{//默认为下左
		m_btDirectionTag = 5;
	}
	else
	{
		CRect rect;
		AfxGetMainWnd()->GetWindowRect(&rect);
		if (m_ptSavePoint.y<=rect.top+rect.Height()*0.3)
		{
			if (m_ptSavePoint.x<rect.left+rect.Width()*0.25)
			{
				m_btDirectionTag = 1;//上左
			}
			else m_btDirectionTag = 2;//上右
		}
		else if (m_ptSavePoint.y<=rect.top+rect.Height()*0.6)
		{
			if (m_ptSavePoint.x<=rect.left+rect.Width()*0.25)
			{
				m_btDirectionTag = 3;//中左
			}
			else m_btDirectionTag = 4;//中右
		}
		else
		{
			if (m_ptSavePoint.x<=rect.left+rect.Width()*0.25)
			{
				m_btDirectionTag = 5;//下左
			}
			else m_btDirectionTag = 6;//下右
		}
	}
	switch(m_btDirectionTag)
	{
	case 1://上左
		SetWindowPos(NULL, ix+10, iy+45, 149, 103, NULL);
		break;
	case 2://上右
		SetWindowPos(NULL, ix-135, iy+45, 149, 103, NULL);
		break;
	case 3://中左
		SetWindowPos(NULL, ix+10, iy-90, 149, 103, NULL);
		break;
	case 4://中右
		SetWindowPos(NULL, ix-135, iy-90, 149, 103, NULL);
		break;
	case 5://下左
		SetWindowPos(NULL, ix+10, iy-90, 149, 103, NULL);
		break;
	case 6://下右
		SetWindowPos(NULL, ix-135, iy-90, 149, 103, NULL);
		break;
	default:
		SetWindowPos(NULL, ix, iy, 149, 103, NULL);
		break;
	}
	
	CDC *pBufferDC = ::CDC::FromHandle(m_ImagePic.GetDC());
	pBufferDC->FillSolidRect(0, 0, 149, 103, RGB(255, 0, 0));
	if (1==m_btDirectionTag || 2==m_btDirectionTag)
	{
		//上
		if (!m_TalkS.IsNull())
		{
			if (1 == m_btDirectionTag)
			{
				m_TalkS.DrawImage(pBufferDC, 0, 0);//正常
			}
			else
			{
				m_TalkS.DrawImage(pBufferDC, 0, 0, m_TalkS.GetWidth(), m_TalkS.GetHeight(), 
					m_TalkS.GetWidth(), 0, -m_TalkS.GetWidth(), m_TalkS.GetHeight());
			}
		}
	}
	else
	{
		//下
		if (!m_TalkX.IsNull())
		{
			if (3 == m_btDirectionTag  ||  5 == m_btDirectionTag)
			{
				m_TalkX.DrawImage(pBufferDC, 0, 0);//正常
			}
			else
			{
				m_TalkX.DrawImage(pBufferDC, 0, 0, m_TalkX.GetWidth(), m_TalkX.GetHeight(), 
					m_TalkX.GetWidth(), 0, -m_TalkX.GetWidth(), m_TalkX.GetHeight());
			}
		}
	}

	m_ImagePic.ReleaseDC();
	SetImageRgn(&m_ImagePic);

	ReleaseDC(pBufferDC);	// add by HouGuoJiang 2011-11-25
	CDC::DeleteTempMap();	// add by HouGuoJiang 2011-11-25
}

void CTalkDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 在此处添加消息处理程序代码
	// 不为绘图消息调用 CDialog::OnPaint()

	if (!m_TalkS.IsNull() && !m_TalkX.IsNull())
	{
		CRect rect;
		GetClientRect(&rect);
		CMemDC dcMem(&dc, rect, true);	// 用单色填充背景

		if (1==m_btDirectionTag || 2==m_btDirectionTag)
		{
			//上
			if (!m_TalkS.IsNull())
			{
				if (1 == m_btDirectionTag)
				{
					m_TalkS.DrawImage(&dcMem, 0, 0);//正常
				}
				else
				{
					m_TalkS.DrawImage(&dcMem, 0, 0, m_TalkS.GetWidth(), m_TalkS.GetHeight(), 
						m_TalkS.GetWidth(), 0, -m_TalkS.GetWidth(), m_TalkS.GetHeight());
				}
			}
		}
		else
		{
			//下
			if (!m_TalkX.IsNull())
			{
				if (3 == m_btDirectionTag  ||  5 == m_btDirectionTag)
				{
					m_TalkX.DrawImage(&dcMem, 0, 0);//正常
				}
				else
				{
					m_TalkX.DrawImage(&dcMem, 0, 0, m_TalkX.GetWidth(), m_TalkX.GetHeight(), 
						m_TalkX.GetWidth(), 0, -m_TalkX.GetWidth(), m_TalkX.GetHeight());
				}
			}
		}

		dcMem.SetBkMode(TRANSPARENT);
		dcMem.SetTextColor(RGB(255, 255, 255));
		CFont ViewFont, *pOldFont = NULL;
		ViewFont.CreateFont(12,0,0,0,400,0,0,0,134,3,2,1,2,TEXT("宋体"));
		pOldFont = dcMem.SelectObject(&ViewFont);

		CString strSaveText;
		strSaveText.Format("%s", m_strTextInfo);
		BYTE btHavePic=0, btCowNums = 0;
		int iHeight = 10;
		if (1==m_btDirectionTag || 2==m_btDirectionTag)
		{
			iHeight = 26;
		}

		while (""!=strSaveText)
		{
			CString strTempText="";
			strTempText=InterceptString(strSaveText, btHavePic);
			if (""==strTempText) break;
			btCowNums ++;
			int iStartX = 10;
			if (1==m_btDirectionTag || 2==m_btDirectionTag)
			{
				if ((iHeight+((1==btHavePic)?22:14))>=91)
				{
					char chPoint[4];
					::memset(chPoint, 0, sizeof(chPoint));
					_snprintf(chPoint, sizeof(chPoint), "...");
					dcMem.TextOut(iStartX, iHeight-4, chPoint, lstrlen(chPoint));
					break;
				}
			}
			else
			{
				if ((iHeight+((1==btHavePic)?22:14))>=70)
				{
					char chPoint[4];
					::memset(chPoint, 0, sizeof(chPoint));
					_snprintf(chPoint, sizeof(chPoint), "...");
					dcMem.TextOut(iStartX, iHeight-2, chPoint, lstrlen(chPoint));
					break;
				}
			}
			int iTempTextLength=strTempText.GetLength();
			if (1==btHavePic)
			{
				int iPicTop=iHeight, iTextTop=iHeight+8;
				for (int k=0; k<iTempTextLength; k++)
				{
					BYTE btWide = 1, btTag = 0, btSaveNums = 0;
					if ((k<iTempTextLength-3)&&('/'==strTempText[k])&&(':'==strTempText[k+1])
						&&(strTempText[k+2]>='0' && strTempText[k+2]<='9')
						&&(strTempText[k+3]>='0' && strTempText[k+3]<='9'))
					{
						char szNums[2];
						::memset(szNums, 0, sizeof(szNums));
						szNums[0] = strTempText[k+2];
						szNums[1] = strTempText[k+3];
						btSaveNums = atoi(szNums);
						if (btSaveNums>=1 && btSaveNums<=70)
						{//有图片,四个字符
							btTag = 1;
							btWide += 3;
						}
					}
					if (0==btTag)
					{
						//没有图片
						if(strTempText[k]<0 || strTempText[k]>255)
						{
							//二个字符
							btWide += 1;
						}
						dcMem.TextOut(iStartX, iTextTop, strTempText.Mid(k, btWide), btWide);
						iStartX += (btWide*6);
					}
					else
					{
						//画图
						ASSERT(btSaveNums>=1 && btSaveNums<=70);
						btSaveNums -= 1;
						if(!m_ImageExpression.IsNull())
						{
							m_ImageExpression.DrawImage(&dcMem, iStartX, iPicTop, 20, 20, (btSaveNums%10)*20, (btSaveNums/10)*20, 20, 20); 
						}
						iStartX += 20;
					}
					k += (btWide-1);
				}
				iHeight+=22;
			}
			else
			{
				//没有图片
				CRect rect;
				rect.left = iStartX;
				rect.top = iHeight;
				rect.right = rect.left + 136;
				rect.bottom = rect.top + 12;
				dcMem.DrawText(strTempText, strTempText.GetLength(), &rect, DT_LEFT|DT_BOTTOM|DT_SINGLELINE);
				iHeight+=14;
			}
			strSaveText=strSaveText.Mid(iTempTextLength); 
		}

		dcMem.draw();
		ViewFont.DeleteObject();
	}
}


CString CTalkDlg::InterceptString(CString strSource, BYTE &btHavePic)
{
	btHavePic = 0;//没有图片
	if (""==strSource) return "";

	int len=0,i=0,y=0;
	CString sTemp="",sreturn="",ceshi="";
	len=strSource.GetLength();
	sTemp.Format("%s", strSource);
	
	for(i=0;i<len;i++)
	{
		BYTE btWide = 1;
		BYTE btTag = 0;
		if ((i<len-3)&&('/'==sTemp[y])&&(':'==sTemp[y+1])
			&&(sTemp[y+2]>='0' && sTemp[y+2]<='9')
			&&(sTemp[y+3]>='0' && sTemp[y+3]<='9'))
		{
			char szNums[2];
			::memset(szNums, 0, sizeof(szNums));
			szNums[0] = sTemp[y+2];
			szNums[1] = sTemp[y+3];
			BYTE btNums = atoi(szNums);
			if (btNums>=1 && btNums<=70)
			{//有图片
				btTag = 1;
				btWide += 3;
				y=y+4;
			}
		}
		if (0==btTag)
		{
			if(sTemp[y]<0 || sTemp[y]>255)
			{
				y=y+2;
				btWide += 1;
			}
			else
			{
				y=y+1;
			}
		}
		if(y>22)
		{
			y-=btWide;
			break;
		}
		if (1==btTag)
		{//有图片
			btHavePic = btTag;
		}
		i+=(btWide-1);
	}
	sreturn=sTemp.Left(y); 
	return sreturn;
}

void CTalkDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CDialog::OnMouseMove(nFlags, point);
}

BOOL CTalkDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if (pMsg->message == WM_KEYDOWN || pMsg->message == WM_SYSKEYDOWN)
	{
		return TRUE;
	}
	return CDialog::PreTranslateMessage(pMsg);
}
//设置聊天内容
void CTalkDlg::SetTalkText(CString strTalk)
{
	m_strTextInfo = "";
	m_strTextInfo.Format("%s", strTalk);
}

//设置聊天框显示
void CTalkDlg::SetTalkDlgShow()
{
	ShowWindow(SW_SHOW);
	KillTimer(TALK_SHOW_TIME);
	KillTimer(TALK_SET_POINT);
	SetTimer(TALK_SHOW_TIME, 5000, NULL);
	SetTimer(TALK_SET_POINT, 100, NULL);
}
void CTalkDlg::OnTimer(UINT nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	switch(nIDEvent)
	{
	case TALK_SHOW_TIME:
		{
			KillTimer(TALK_SHOW_TIME);
			KillTimer(TALK_SET_POINT);
			SendMessage(WM_CLOSE, 0, 0);
			break;
		}
	case TALK_SET_POINT:
		{
			CRect rect;
			AfxGetMainWnd()->GetWindowRect(&rect);
			if ((m_ptWndPoint.x != rect.left) || (m_ptWndPoint.y != rect.top))
			{
				m_ptWndPoint.x = rect.left;
				m_ptWndPoint.y = rect.top;
				SetWindowPos(NULL, rect.left+m_ptSavePoint.x, rect.top+m_ptSavePoint.y, 149, 103, NULL);
			}
			break;
		}
	}
	CDialog::OnTimer(nIDEvent);
}

//加载聊天框皮肤
void CTalkDlg::LoadTalkSkin(BYTE btSkinID)
{
	//加载资源
	HINSTANCE hInstance=GetModuleHandle(GAME_FRAME_DLL_NAME);
	TCHAR szCurrentPath[MAX_PATH];
	::memset(szCurrentPath, 0, sizeof(szCurrentPath));
	CString strResourcePath2="", strFileName="";
    GetCurrentDirectory(MAX_PATH,szCurrentPath);

	CString strGameName="";
	CWnd* pMainWnd = AfxGetMainWnd();
	if (NULL != pMainWnd)
	{
		((CGameFrameDlg*)pMainWnd)->GetGameName(strGameName);
	}
	WCHAR * pWC = NULL;
	CString strTalks = "";
	strTalks.Format("%s\\%s\\talkframeup.png", szCurrentPath, strGameName);
	pWC = strTalks.AllocSysString();
	m_TalkS.LoadFromFile(pWC);
	if (m_TalkS.IsNull())
	{
		pWC = NULL;
		strResourcePath2 = "";
		strFileName = "";
		strFileName.Format("TalkFrame%dUp.png", btSkinID);
		strResourcePath2.Format("%s\\data\\gameframe\\plaza\\%s",szCurrentPath, strFileName);
		//加载资源
		pWC = strResourcePath2.AllocSysString();
		m_TalkS.LoadFromFile(pWC);
	}

	pWC = NULL;
	strTalks = "";
	strTalks.Format("%s\\%s\\talkframebottom.png", szCurrentPath, strGameName);
	pWC = strTalks.AllocSysString();
	m_TalkX.LoadFromFile(pWC);
	if (m_TalkX.IsNull())
	{
		pWC = NULL;
		strResourcePath2 = "";
		strFileName = "";
		strFileName.Format("TalkFrame%dBottom.png", btSkinID);
		strResourcePath2.Format("%s\\data\\gameframe\\plaza\\%s",szCurrentPath, strFileName);
		//加载资源
		pWC = strResourcePath2.AllocSysString();
		m_TalkX.LoadFromFile(pWC);
	}

	pWC = NULL;
	strResourcePath2.Format("%s\\data\\gameframe\\plaza\\expression.png",szCurrentPath);
	//加载资源
	pWC = strResourcePath2.AllocSysString();
	if (m_ImageExpression.IsNull())
	{
		m_ImageExpression.LoadFromFile(pWC);
	}
}

void CTalkDlg::OnKillAllTimer()
{
	KillTimer(TALK_SHOW_TIME);
	KillTimer(TALK_SET_POINT);
}