// ShowUserActionInfo.cpp : 实现文件
//

#include "stdafx.h"
#include "ShowUserActionInfo.h"
#include ".\showuseractioninfo.h"
#include "Resource.h"
#include "GameFrameDlg.h"

#include "MemDC.h"

// CShowUserActionInfo

#define		IDI_TIMER_SHOW_USER_ACTION_INFO				5000
#define		TIMER_SHOW_USER_ACTION_INFO_INTERVAL		4000

#define		IDI_TIMER_PLAY_FLASH						5001
#define		TIMER_PLAY_FLASH_INTERVAL					100


IMPLEMENT_DYNAMIC(CShowUserActionInfo, CWnd)
CShowUserActionInfo::CShowUserActionInfo()
{
	EnableAutomation();
	::memset(m_chActionInfo, 0, sizeof(m_chActionInfo));
	m_wCurPlayFlashIndex = 0;
	m_wCountFlashImageNums = 0;
}

CShowUserActionInfo::~CShowUserActionInfo()
{
}

void CShowUserActionInfo::OnFinalRelease()
{
	// 释放了对自动化对象的最后一个引用后，将调用
	// OnFinalRelease。基类将自动
	// 删除该对象。在调用该基类之前，请添加您的
	// 对象所需的附加清除代码。

	CWnd::OnFinalRelease();
}


BEGIN_MESSAGE_MAP(CShowUserActionInfo, CWnd)
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_WM_CREATE()
	ON_WM_SIZE()
END_MESSAGE_MAP()


BEGIN_DISPATCH_MAP(CShowUserActionInfo, CWnd)
END_DISPATCH_MAP()

// 注意: 我们添加 IID_IShowUserActionInfo 支持
//以支持来自 VBA 的类型安全绑定。此 IID 必须同附加到 .IDL 文件中的
//调度接口的 GUID 匹配。

// {1049F0F4-BC96-486A-A1F4-9B7D86E4127D}
static const IID IID_IShowUserActionInfo =
{ 0x1049F0F4, 0xBC96, 0x486A, { 0xA1, 0xF4, 0x9B, 0x7D, 0x86, 0xE4, 0x12, 0x7D } };

BEGIN_INTERFACE_MAP(CShowUserActionInfo, CWnd)
	INTERFACE_PART(CShowUserActionInfo, IID_IShowUserActionInfo, Dispatch)
END_INTERFACE_MAP()


// CShowUserActionInfo 消息处理程序


void CShowUserActionInfo::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 在此处添加消息处理程序代码
	// 不为绘图消息调用 CWnd::OnPaint()
	
	CRect rect;
	GetClientRect(&rect);
	CMemDC dcMem(&dc, rect, true);	// 用单色填充背景

	if (DrawWindow(&dcMem))
	{
		//绘制动画
		if (m_wCurPlayFlashIndex>=0 && m_wCurPlayFlashIndex<MAX_FRAME_NUMS)
		{
			//把绘制的图片居中对齐
			WORD wImageW = m_vecFlashImageArray[m_wCurPlayFlashIndex].GetWidth();
			WORD wImageH = m_vecFlashImageArray[m_wCurPlayFlashIndex].GetHeight();
			WORD wDrawX = m_rcImageRect.left + (m_rcImageRect.Width()-wImageW)/2;
			WORD wDrawY = m_rcImageRect.top + (m_rcImageRect.Height()-wImageH)/2;
			m_vecFlashImageArray[m_wCurPlayFlashIndex].DrawImage(&dcMem, wDrawX, wDrawY, wImageW, wImageH, 0, 0);
		}
		
		//绘制文本
		CFont DrawFont;
		DrawFont.CreateFont(-14,0,0,0,400,0,0,0,134,3,2,1,2,TEXT("微软雅黑"));
		CFont * pOldFont=dcMem.SelectObject(&DrawFont);
		dcMem.DrawText(m_strTextList.c_str(), &m_rcTextRect, DT_LEFT|DT_TOP|DT_WORDBREAK);
		dcMem.SelectObject(pOldFont);
		DrawFont.DeleteObject();
		
		//GetClientRect(&rect);	// 为何不放在循环的上面？ 而要每一次循环都赋值，是不是浪费CPU资源呢？
	}
	
}

void CShowUserActionInfo::OnTimer(UINT nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	
	if (IDI_TIMER_PLAY_FLASH == nIDEvent)
	{
		++m_wCurPlayFlashIndex;
		if ((m_wCurPlayFlashIndex>=m_wCountFlashImageNums) || (m_wCurPlayFlashIndex>=MAX_FRAME_NUMS))
		{
			m_wCurPlayFlashIndex = 0;
		}
		InvalidateRect(&m_rcImageRect);
	}
	else if (IDI_TIMER_SHOW_USER_ACTION_INFO == nIDEvent)
	{
		KillTimer(IDI_TIMER_SHOW_USER_ACTION_INFO);
		ShowWindow(SW_HIDE);
	}

	CWnd::OnTimer(nIDEvent);
}

//初始化数据
void CShowUserActionInfo::InitShowData()
{
	::memset(m_chActionInfo, 0, sizeof(m_chActionInfo));
	m_wCurPlayFlashIndex = -1;
	KillTimer(IDI_TIMER_PLAY_FLASH);
	InvalidateRect(NULL);
}

//设置动画
void CShowUserActionInfo::SetPlayFlash(BYTE byFlashIndex, const char* pSendName, const char* pRecvName)
{
	if (byFlashIndex>=0 && byFlashIndex<TOTAL_FLASH_NUMS)
	{
		if (m_PlayFlash[byFlashIndex].wTotalFrame>0)
		{
			ShowWindow(SW_SHOW);
			SetForegroundWindow();

			KillTimer(IDI_TIMER_PLAY_FLASH);
			KillTimer(IDI_TIMER_SHOW_USER_ACTION_INFO);
			
			//配置动画
			m_wCountFlashImageNums = 0;
			m_wCurPlayFlashIndex = 0;
			m_PlayFlash[byFlashIndex].bIsPlay = true;
			m_PlayFlash[byFlashIndex].wTimeInterval = TIMER_PLAY_FLASH_INTERVAL;
			HINSTANCE hInstance=GetModuleHandle(GAME_FRAME_DLL_NAME);
			CPngImage image;
			char chData[32]={0};
			for (int i=1; i<=m_PlayFlash[byFlashIndex].wTotalFrame; ++i)
			{
				m_vecFlashImageArray[m_wCountFlashImageNums].DestroyImage();
				::memset(chData, 0, sizeof(chData));
				::sprintf(chData, "IMAGE_UA_%d_%d_F", byFlashIndex, i);
				m_vecFlashImageArray[m_wCountFlashImageNums].LoadImage(hInstance, chData);
				++m_wCountFlashImageNums;
				if (m_wCountFlashImageNums>=MAX_FRAME_NUMS)
				{
					break;
				}
			}

			//配置文本
			SetFlashText(byFlashIndex, pSendName, pRecvName);
			InvalidateRect(&m_rcImageRect);

			//动画定时器
			SetTimer(IDI_TIMER_PLAY_FLASH, m_PlayFlash[byFlashIndex].wTimeInterval, NULL);

			//自动关闭窗口
			SetTimer(IDI_TIMER_SHOW_USER_ACTION_INFO, TIMER_SHOW_USER_ACTION_INFO_INTERVAL, NULL);
		}
	}
}
//设置文本
void CShowUserActionInfo::SetFlashText(BYTE byFlashIndex, const char* pSendName, const char* pRecvName)
{
	char chInfo[100] = {0};
	m_strTextList = "";
	switch(byFlashIndex)
	{
	case 0:	//送玫瑰--XX悄悄的走到了XX身边，拿出了一枝玫瑰。
		{
			m_strTextList.append(pSendName);
			::memset(chInfo, 0, sizeof(chInfo));
			::sprintf(chInfo, " 悄悄的走到了 ");
			m_strTextList.append(chInfo);
			m_strTextList.append(pRecvName);
			::memset(chInfo, 0, sizeof(chInfo));
			::sprintf(chInfo, " 身边,拿出了一枝玫瑰.");
			m_strTextList.append(chInfo);
		}
	break;
	case 1:	//做鬼脸--XX捏着自己的脸向所有人伸了伸舌头。		
		{
			m_strTextList.append(pSendName);
			::memset(chInfo, 0, sizeof(chInfo));
			::sprintf(chInfo, " 捏着自己的脸向所有人伸了伸舌头.");
			m_strTextList.append(chInfo);
		}
		break;
	case 2:	//熊抱--XX狠狠的抱了XX一下，粗鲁中略带温柔。	
		{
			m_strTextList.append(pSendName);
			::memset(chInfo, 0, sizeof(chInfo));
			::sprintf(chInfo, " 狠狠的抱了 ");
			m_strTextList.append(chInfo);
			m_strTextList.append(pRecvName);
			::memset(chInfo, 0, sizeof(chInfo));
			::sprintf(chInfo, " 一下,粗鲁中略带温柔.");
			m_strTextList.append(chInfo);
		}
		break;
	case 3:	//扭屁股--XX撅起自己的屁股，在大家面前晃悠。	
		{
			m_strTextList.append(pSendName);
			::memset(chInfo, 0, sizeof(chInfo));
			::sprintf(chInfo, " 撅起自己的屁股,在大家面前晃悠.");
			m_strTextList.append(chInfo);
		}
		break;
	case 4:	//挠头--XX无奈的挠了挠头
		{
			m_strTextList.append(pSendName);
			::memset(chInfo, 0, sizeof(chInfo));
			::sprintf(chInfo, " 无奈的挠了挠头.");
			m_strTextList.append(chInfo);
		}
		break;
	case 5:	//飞吻--XX向XX献了一个飞吻.			
		{
			m_strTextList.append(pSendName);
			::memset(chInfo, 0, sizeof(chInfo));
			::sprintf(chInfo, " 向 ");
			m_strTextList.append(chInfo);
			m_strTextList.append(pRecvName);
			::memset(chInfo, 0, sizeof(chInfo));
			::sprintf(chInfo, " 献了一个飞吻.");
			m_strTextList.append(chInfo);
		}
		break;
	case 6:	//献花圈--XX郑重的向XX献上了一个美丽的花圈	
		{
			m_strTextList.append(pSendName);
			::memset(chInfo, 0, sizeof(chInfo));
			::sprintf(chInfo, " 郑重的向 ");
			m_strTextList.append(chInfo);
			m_strTextList.append(pRecvName);
			::memset(chInfo, 0, sizeof(chInfo));
			::sprintf(chInfo, " 献上了一个美丽的花圈.");
			m_strTextList.append(chInfo);
		}
		break;
	case 7:	//抽泣--XX委屈的哭了	
		{
			m_strTextList.append(pSendName);
			::memset(chInfo, 0, sizeof(chInfo));
			::sprintf(chInfo, " 委屈的哭了.");
			m_strTextList.append(chInfo);
		}
		break;
	case 8:	//来一杯--XX向所有人敬了一杯酒。	
		{
			m_strTextList.append(pSendName);
			::memset(chInfo, 0, sizeof(chInfo));
			::sprintf(chInfo, " 向所有人敬了一杯酒.");
			m_strTextList.append(chInfo);
		}
		break;
	case 9:	//嘲笑--XX指着XX轻蔑的笑了一下。
		{
			m_strTextList.append(pSendName);
			::memset(chInfo, 0, sizeof(chInfo));
			::sprintf(chInfo, " 指着 ");
			m_strTextList.append(chInfo);
			m_strTextList.append(pRecvName);
			::memset(chInfo, 0, sizeof(chInfo));
			::sprintf(chInfo, " 轻蔑的笑了一下.");
			m_strTextList.append(chInfo);
		}
		break;
	case 10:	//拔汗毛--XX把手伸向了胳肢窝，五秒钟之后只见他的手里多出了一根毛…
		{
			m_strTextList.append(pSendName);
			::memset(chInfo, 0, sizeof(chInfo));
			::sprintf(chInfo, " 把手伸向了胳肢窝,只见他手里多出了一根毛.");
			m_strTextList.append(chInfo);
		}
		break;
	case 11:	//调情--XX指着XX说,来给大爷笑一个.
		{
			m_strTextList.append(pSendName);
			::memset(chInfo, 0, sizeof(chInfo));
			::sprintf(chInfo, " 指着 ");
			m_strTextList.append(chInfo);
			m_strTextList.append(pRecvName);
			::memset(chInfo, 0, sizeof(chInfo));
			::sprintf(chInfo, " 说,来给大爷笑一个.");
			m_strTextList.append(chInfo);
		}
		break;
	case 12:	//臭美--XX把小拇指插进了鼻孔里,对着镜子邪恶的笑了.			
		{
			m_strTextList.append(pSendName);
			::memset(chInfo, 0, sizeof(chInfo));
			::sprintf(chInfo, " 把小拇指插进了鼻孔里,对着镜子邪恶的笑了.");
			m_strTextList.append(chInfo);
		}
		break;
	case 13:	//大笑--XX放声大笑.			
		{
			m_strTextList.append(pSendName);
			::memset(chInfo, 0, sizeof(chInfo));
			::sprintf(chInfo, " 放声大笑.");
			m_strTextList.append(chInfo);
		}
		break;
	case 14:	//内急--XX捂着肚子,憋得满脸通红.
		{
			m_strTextList.append(pSendName);
			::memset(chInfo, 0, sizeof(chInfo));
			::sprintf(chInfo, " 捂着肚子,憋得满脸通红.");
			m_strTextList.append(chInfo);
		}
		break;
	case 15:	//扔鸡蛋--XX使出吃奶的力气向XX扔了一个臭鸡蛋。			
		{
			m_strTextList.append(pSendName);
			::memset(chInfo, 0, sizeof(chInfo));
			::sprintf(chInfo, " 使出吃奶的力气向 ");
			m_strTextList.append(chInfo);
			m_strTextList.append(pRecvName);
			::memset(chInfo, 0, sizeof(chInfo));
			::sprintf(chInfo, " 扔了一个臭鸡蛋.");
			m_strTextList.append(chInfo);
		}
		break;
	case 16:	//放屁--咚的一声巨响，之后传来了一股化学气体的味道，XX邪恶的笑了。
		{
			::memset(chInfo, 0, sizeof(chInfo));
			::sprintf(chInfo, "咚的一声,传来了一股化学气体味道,");
			m_strTextList.append(chInfo);
			m_strTextList.append(pSendName);
			::memset(chInfo, 0, sizeof(chInfo));
			::sprintf(chInfo, " 邪恶的笑了.");
			m_strTextList.append(chInfo);
		}
		break;
	case 17:	//感动--XX感动的哭了			
		{
			m_strTextList.append(pSendName);
			::memset(chInfo, 0, sizeof(chInfo));
			::sprintf(chInfo, " 感动的哭了.");
			m_strTextList.append(chInfo);
		}
		break;
	case 18:	//坑爹--XX歇斯底里的喊道,坑爹啊!			
		{
			m_strTextList.append(pSendName);
			::memset(chInfo, 0, sizeof(chInfo));
			::sprintf(chInfo, " 歇斯底里的喊道,坑爹啊!");
			m_strTextList.append(chInfo);
		}
		break;
	case 19:	//招手--XX面带微笑，向XX招了招手。			
		{
			m_strTextList.append(pSendName);
			::memset(chInfo, 0, sizeof(chInfo));
			::sprintf(chInfo, " 面带微笑,向 ");
			m_strTextList.append(chInfo);
			m_strTextList.append(pRecvName);
			::memset(chInfo, 0, sizeof(chInfo));
			::sprintf(chInfo, " 招了招手.");
			m_strTextList.append(chInfo);
		}
		break;
	default:
		break;
	}
	CWnd* pMainWnd = AfxGetMainWnd();
	if (NULL != pMainWnd)
	{
		((CGameFrameDlg*)pMainWnd)->InsertSystemString(m_strTextList.c_str());
	}
	InvalidateRect(&m_rcTextRect);
}

//动画数组初始化
void CShowUserActionInfo::InitFlashData()
{
	m_wCurPlayFlashIndex = 0;
	for (int i=0; i<TOTAL_FLASH_NUMS; i++)
	{
		m_PlayFlash[i].bIsPlay = false;
		m_PlayFlash[i].wTotalFrame = 0;
		m_PlayFlash[i].wTimeInterval = TIMER_PLAY_FLASH_INTERVAL;
	}
	m_PlayFlash[0].wTotalFrame = 15;
	m_PlayFlash[1].wTotalFrame = 11;
	m_PlayFlash[2].wTotalFrame = 6;
	m_PlayFlash[3].wTotalFrame = 11;
	m_PlayFlash[4].wTotalFrame = 2;
	m_PlayFlash[5].wTotalFrame = 13;
	m_PlayFlash[6].wTotalFrame = 4;
	m_PlayFlash[7].wTotalFrame = 10;
	m_PlayFlash[8].wTotalFrame = 8;
	m_PlayFlash[9].wTotalFrame = 10;
	m_PlayFlash[10].wTotalFrame = 12;
	m_PlayFlash[11].wTotalFrame = 12;
	m_PlayFlash[12].wTotalFrame = 10;
	m_PlayFlash[13].wTotalFrame = 8;
	m_PlayFlash[14].wTotalFrame = 3;
	m_PlayFlash[15].wTotalFrame = 18;
	m_PlayFlash[16].wTotalFrame = 11;
	m_PlayFlash[17].wTotalFrame = 5;
	m_PlayFlash[18].wTotalFrame = 9;
	m_PlayFlash[19].wTotalFrame = 12;
}

int CShowUserActionInfo::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码
	HINSTANCE hInstance=GetModuleHandle(GAME_FRAME_DLL_NAME);
	m_ImageBack.SetLoadInfo(IDB_USER_ACTION_FLASH_BK, hInstance, false);

	//动画数组初始化
	InitFlashData();

	//文本区域
	m_rcTextRect.left = 100;
	m_rcTextRect.top = 20;
	m_rcTextRect.right = m_rcTextRect.left + 130;
	m_rcTextRect.bottom = m_rcTextRect.top + 82;

	//动画区域
	m_rcImageRect.left = 0;
	m_rcImageRect.top = 8;
	m_rcImageRect.right = m_rcImageRect.left + 100;
	m_rcImageRect.bottom = m_rcImageRect.top + 82;
	return 0;
}




void CShowUserActionInfo::AddRectRgn(CDC* pDC,CRgn* rgn,CRect* rect,COLORREF col)
{
	for(int y=rect->top;y<rect->bottom;y++)
		for(int x=rect->left;x<rect->right;x++)
			if (pDC->GetPixel(x,y)==col)
			{
				CRgn rgnAdd;
				rgnAdd.CreateRectRgn (x, y, x+1, y+1);
				rgn->CombineRgn(rgn, &rgnAdd, RGN_DIFF);
				DeleteObject(rgnAdd);
			}
}
void CShowUserActionInfo::UpdateRgn(CDC * pDC)
{
	CImageHandle ImageHandle(&m_ImageBack);
	CRect rcClient;
	GetClientRect(&rcClient);
	CRgn rgn;
	COLORREF col = RGB(255,0,255);  // mask color
	CRect rc(0,0,0,0);
	rgn.CreateRectRgn (0, 0, rcClient.Width(), rcClient.Height());

	//左上
	rc.bottom =  m_ImageBack.GetHeight();
	rc.right = m_ImageBack.GetWidth();
	AddRectRgn(pDC,&rgn,&rc,col);
	SetWindowRgn (rgn,TRUE);
	//SetForegroundWindow();
}

bool CShowUserActionInfo::DrawWindow(CDC * pDC)
{
	if (NULL != pDC && NULL != pDC->m_hDC)
	{
		CImageHandle ImageHandle(&m_ImageBack);
		pDC->SetBkMode(TRANSPARENT);
		pDC->FillSolidRect(0,0,m_ImageBack.GetWidth(),m_ImageBack.GetHeight(),RGB(255,0,255));

		//绘画背景图片
		CImageHandle HandleCenter(&m_ImageBack);
		m_ImageBack.AlphaDrawImage(pDC, 0, 0, RGB(255, 0, 255));
		return true;
	}
	return false;
}


void CShowUserActionInfo::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

#if 0  // 删除这一段会有问题吗？
	CClientDC dc(this);
	CRect rect;
	GetClientRect(&rect);
	CMemDC dcMem(&dc, rect, true);	// 用单色填充背景

	if (DrawWindow(&dc))
	{
		UpdateRgn(&dc);
		CRect rect,rc,rc2;
		GetClientRect(&rect);
		CString sss;
		if( GetUpdateRect(&rc) )
		{
			rc2 = rc;
			if( rc.left < rc.right )
			{
				rc2.top = rect.top;
				rc2.bottom = rect.bottom;
				InvalidateRect(&rc2,0);
			}
			rc2 = rc;
			if( rc.top < rc.bottom )
			{
				rc2.left = rect.left;
				rc2.right = rect.right;
				InvalidateRect(&rc2,0);
			}
		}
		else
			InvalidateRect(0);
	}
#endif

}
