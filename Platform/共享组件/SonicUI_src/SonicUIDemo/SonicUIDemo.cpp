// SonicUIDemo.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "time.h"
#include "SonicUIDemo.h"
#include "ISonicUI.h"
#define MAX_LOADSTRING 100
//#include "tool.h" 

#include "SkinDlg.h"
#include "SkinMgr.h"

// 全局变量:
HINSTANCE hInst;								// 当前实例
TCHAR szTitle[MAX_LOADSTRING];					// 标题栏文本
TCHAR szWindowClass[MAX_LOADSTRING];			// 主窗口类名

#define TEST 15
ISonicString * g_pTest[TEST] = {0};
ISonicWndEffect * g_pEffect = NULL;
ISonicAnimation * g_pAni = NULL;
ISonicAnimation * g_pMoveImg = NULL;
ISonicAnimation * g_pTransPaint = NULL;

void WINAPI OnClose(ISonicString * pStr, LPVOID)
{
	DestroyWindow(g_pEffect->GetSafeHwnd());
}

void WINAPI OnShutter(ISonicBase *, LPVOID)
{
	g_pEffect->Shutter();
}

void WINAPI OnMove(ISonicString * pStr, LPVOID)
{
	g_pEffect->MoveGently(0, 0);
}

void WINAPI OnAniClose(ISonicString * pStr, LPVOID)
{
	RECT rt = {0, 0, 1, 1};
	g_pEffect->EnableTransform(TRUE, 0, &rt, 20);
}

void WINAPI OnAni(ISonicString * pStr, LPVOID)
{
	g_pAni->Transform(TRUE, 360, NULL, 30);
}

void WINAPI OnTransOver(ISonicAnimation * pAni, LPVOID)
{
	g_pAni->Transform(FALSE);
}

void WINAPI OnPopupDlg(ISonicString * pStr, LPVOID pParam)
{
	HWND hWnd = (HWND)pParam;
	static CSkinDlg dlg;
	//dlg.DoModal(hWnd);
	dlg.Create(hWnd);
	dlg.ShowWindow(SW_SHOW);
}

void WINAPI OnRotate(ISonicAnimation * pAni, LPVOID)
{
	if(pAni->GetAnimationStatus() & ANI_STATUS_TRANSFROM)
	{
		pAni->Transform(FALSE, 0, 0, 15);
	}
	else
	{
		pAni->Transform(TRUE, 90, NULL, 15);
	}

}

void WINAPI OnMouseEnter(ISonicAnimation * pAni, LPVOID)
{
	int x = 150 + rand() % 600;
	int y = 100 + rand() % 350;
	pAni->MoveGently(x, y, 30);
}

// 此代码模块中包含的函数的前向声明:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
 	// TODO: 在此放置代码。
	MSG msg;
	HACCEL hAccelTable;

	// 初始化全局字符串
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_SONICUIDEMO, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// 执行应用程序初始化:
	if (!InitInstance (hInstance, nCmdShow)) 
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_SONICUIDEMO);

	// 主消息循环:
	while (GetMessage(&msg, NULL, 0, 0)) 
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) 
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	GetSonicUI()->DestroyAllObject();

	return (int) msg.wParam;
}



//
//  函数: MyRegisterClass()
//
//  目的: 注册窗口类。
//
//  注释: 
//
//    仅当希望在已添加到 Windows 95 的
//    “RegisterClassEx”函数之前此代码与 Win32 系统兼容时，
//    才需要此函数及其用法。调用此函数
//    十分重要，这样应用程序就可以获得关联的
//   “格式正确的”小图标。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX); 

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, (LPCTSTR)IDI_SONICUIDEMO);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= (LPCTSTR)IDC_SONICUIDEMO;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);

	return RegisterClassEx(&wcex);
}

HWND hSuck = NULL;

//
//   函数: InitInstance(HANDLE, int)
//
//   目的: 保存实例句柄并创建主窗口
//
//   注释: 
//
//        在此函数中，我们在全局变量中保存实例句柄并
//        创建和显示主程序窗口。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // 将实例句柄存储在全局变量中

   hWnd = CreateWindowEx(0, szWindowClass, szTitle, WS_OVERLAPPEDWINDOW, 
      100, 100, 800, 550, NULL, NULL, hInstance, NULL);

//    hSuck = CreateWindow(szWindowClass, szTitle, WS_POPUP | WS_VISIBLE,
// 	   50, 150, 200, 200, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }
   srand((unsigned int)time(NULL));
   _SkinMgr.Init();

   CSkinDlg dlg;
//    dlg.DoModal();
//    return FALSE;

   for(int i = 0; i < TEST; i++)
   {
	   g_pTest[i] = GetSonicUI()->CreateString();
   }
   g_pTest[0]->Format(_T("/c=%x/测试字样1"), RGB(255, 0, 0));

   g_pTest[1]->Format(_T("/c=%x, a='http://hi.csdn.net/zskof'/点我打开链接"), RGB(0, 0, 255));

   g_pTest[2]->Format(_T("/c=%x, a='http://hi.csdn.net/zskof', linkl=1, linkc=0, linkt='点击'/点我打开链接1"), RGB(255, 0, 255));

   g_pTest[3]->Format(_T("/c=%x, a='http://hi.csdn.net/zskof', linkl=2, font, font_height=14, font_bold=1, font_italic=1/点我打开链接2"), RGB(255, 0, 255));

   g_pTest[4]->Format(_T("/sparkle, sparkle_color=%x|%x|%x/闪烁特殊"), RGB(255, 0, 0), RGB(255, 255, 0), RGB(0, 255, 0));

   g_pTest[5]->Format(_T("/fadeout, fadeout_min=0/渐隐渐现/p=%d/"), _SkinMgr.pImgClick->GetObjectId());

   g_pTest[6]->Format(_T("/c=%x, line_width=100/我们都有一个家，名字叫中国，兄弟姐妹都很多，景色也不错"), RGB(0, 255, 0));

   g_pTest[7]->Format(_T("/c=%x, line_width=100, single_line=2/我们都有一个家，名字叫中国，兄弟姐妹都很多，景色也不错"), RGB(0, 0, 0));

   g_pTest[8]->Format(_T("/a, c=%x, linkl=2/点我关闭程序"), RGB(255, 0, 255));
   g_pTest[8]->Delegate(DELEGATE_EVENT_CLICK, NULL, NULL, OnShutter);

   g_pTest[9]->Format(_T("/a, p3=%d|%d|%d, linkt='点我移动'/"), _SkinMgr.pImgNormal->GetObjectId(), _SkinMgr.pImgHover->GetObjectId(), _SkinMgr.pImgClick->GetObjectId());
   g_pTest[9]->Delegate(DELEGATE_EVENT_CLICK, NULL, NULL, OnMove);

   g_pTest[10]->Format(_T("/a, p3=%d|%d|%d, linkt='点我动画关闭', animation=40/"), _SkinMgr.pImgNormal->GetObjectId(), _SkinMgr.pImgHover->GetObjectId(), _SkinMgr.pImgClick->GetObjectId());
   g_pTest[10]->Delegate(DELEGATE_EVENT_CLICK, NULL, NULL, OnAniClose);

   g_pTest[11]->Format(_T("/a, p4=%d, linkt='大家好', btn_text='点击弹出对话框', btn_width=156, btn_height=76/"), _SkinMgr.pImgButton->GetObjectId());
   g_pTest[11]->Delegate(DELEGATE_EVENT_CLICK, hWnd, NULL, OnPopupDlg);
/*
   ISonicSkin * pSkin = GetSonicUI()->CreateSkin();
   pSkin->SetSkin(_T("style"), _T("sizable:1"));
   pSkin->SetSkin(_T("bg"), _T("image:%d; color_key:%d"), _SkinMgr.pImgDlg->GetObjectId(), RGB(128, 0, 128));
   pSkin->Attach(hWnd);
*/
   g_pEffect = GetSonicUI()->CreateWndEffect();
   g_pEffect->Attach(hWnd);
   g_pEffect->Delegate(DELEGATE_EVENT_TRANSFORM_OVER, NULL, NULL, OnClose);
   g_pEffect->Delegate(DELEGATE_EVENT_SHUTTER_OVER, NULL, NULL, OnClose);


   

   ISonicString * pDemo1 = GetSonicUI()->CreateString();
   pDemo1->Format(_T("/a/大家好我是滚动字幕/p=%d/"), _SkinMgr.pImgCard->GetObjectId());
   ISonicTextScrollBar * pScroll  = GetSonicUI()->CreateTextScrollBar();
   pScroll->Create(hWnd, 300, 10, 150, 60, TRUE, 40);
   pScroll->AddText(pDemo1->GetObjectId());
   pScroll->AddText(_SkinMgr.pImgNormal->GetObjectId());
   pScroll->DoScroll();

   ISonicString * pDemo2 = GetSonicUI()->CreateString();
   pDemo2->Format(_T("/a, p3=%d|%d|%d, linkt='点我试试动画效果'/"), _SkinMgr.pImgNormal->GetObjectId(), _SkinMgr.pImgHover->GetObjectId(), _SkinMgr.pImgClick->GetObjectId());
   pDemo2->Delegate(DELEGATE_EVENT_CLICK, NULL, NULL, OnAni);
   g_pAni = GetSonicUI()->CreateAnimation();
   g_pAni->Create(hWnd, 300, 200, pDemo2->GetWidth(), pDemo2->GetHeight());
   g_pAni->Delegate(DELEGATE_EVENT_TRANSFORM_OVER, NULL, NULL, OnTransOver);
   g_pAni->AddObject(pDemo2->GetObjectId());

   ISonicString * pGif = GetSonicUI()->CreateString();
   pGif->Format(_T("/p=%d/"), _SkinMgr.pImgGif->GetObjectId());
   g_pMoveImg = GetSonicUI()->CreateAnimation();
   g_pMoveImg->Create(hWnd, 100, 200, pGif->GetWidth(), pGif->GetHeight());
   g_pMoveImg->Delegate(DELEGATE_EVENT_MOUSEENTER, NULL, NULL, OnMouseEnter);
   g_pMoveImg->AddObject(pGif->GetObjectId());

   ISonicString * pStrTrans = GetSonicUI()->CreateString();
   pStrTrans->Format(_T("/c=0xff0000, fadeout, init_normal/Do me a favor"));
   ISonicPaint * pPaint = GetSonicUI()->CreatePaint();
   pPaint->Create(FALSE);
   pPaint->AddObject(pStrTrans->GetObjectId());
   pPaint->AddObject(_SkinMgr.pImgNormal->GetObjectId(), 0, pStrTrans->GetHeight() + 10);
   g_pTransPaint = GetSonicUI()->CreateAnimation();
   g_pTransPaint->Create(hWnd, 400, 200, pPaint->GetWidth(), pPaint->GetHeight());
   g_pTransPaint->Delegate(DELEGATE_EVENT_CLICK, NULL, NULL, OnRotate);
   g_pTransPaint->AddObject(pPaint->GetObjectId());

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  函数: WndProc(HWND, unsigned, WORD, LONG)
//
//  目的: 处理主窗口的消息。
//
//  WM_COMMAND	- 处理应用程序菜单
//  WM_PAINT	- 绘制主窗口
//  WM_DESTROY	- 发送退出消息并返回
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message) 
	{
// 	case WM_NCPAINT:
// 		{
// 			HDC hdc;
// 			hdc = GetWindowDC(hWnd);
// 			// Paint into this DC
// 			RECT rtWnd;
// 			GetWindowRect(hWnd, &rtWnd);
// 			OffsetRect(&rtWnd, -rtWnd.left, -rtWnd.top);
// 			FillRect(hdc, &rtWnd, (HBRUSH)GetStockObject(GRAY_BRUSH));
// 			ReleaseDC(hWnd, hdc);
// 		}
// 		break;
	case WM_LBUTTONUP:
		{
			//SendMessage(hWnd, WM_NCLBUTTONDBLCLK, HTCAPTION, MAKELPARAM(100, 10));
			//SendMessage(hWnd, WM_RBUTTONUP, 0, MAKELPARAM(99, -20));
		}
		break;
	case WM_COMMAND:
		wmId    = LOWORD(wParam); 
		wmEvent = HIWORD(wParam); 
		// 分析菜单选择:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, (LPCTSTR)IDD_ABOUTBOX, hWnd, (DLGPROC)About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_ERASEBKGND:
		{
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		{
			hdc = BeginPaint(hWnd, &ps);
			// TODO: 在此添加任意绘图代码...
			int y = 10;
			for(int i = 0; i < TEST; i++)
			{
				g_pTest[i]->TextOut(hdc, 10, y, hWnd);
				y += g_pTest[i]->GetHeight() + 20;
			}
			EndPaint(hWnd, &ps);
		}
		break;
// 	case WM_WINDOWPOSCHANGING:
// 		{
// 			WINDOWPOS * pPos = (WINDOWPOS *)lParam;
// 			if(hSuck != hWnd)
// 			{
// 				if(!(pPos->flags & SWP_NOMOVE))
// 				{
// 					SetWindowPos(hSuck, NULL, pPos->x - 50, pPos->y - 50, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
// 				}
// 			}
// 		}
// 		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

class CAboutUI
{
public:
	ISonicString * pContactMe;
	ISonicString * pClose;
	void OnClose(ISonicString * pStr, LPVOID);
}about;

void CAboutUI::OnClose(ISonicString * pStr, LPVOID)
{
	EndDialog(pStr->GetSafeHwnd(), 0);
}

// “关于”框的消息处理程序。
LRESULT CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
		{
			about.pContactMe = GetSonicUI()->CreateString();
			about.pContactMe->Format(_T("/global, c=0xffffff, line_space=3/CopyRight(C) 2008 Sonic Guan\nQQ:20346020\n/a='mailto:zs_kof@163.com', linkl=2, c=0xffffff/mailto:zs_kof@163.com"));

			about.pClose = GetSonicUI()->CreateString();
			about.pClose->Format(_T("/a, linkl=2, c=%x, linkh=%x/Close"), RGB(255, 255, 255), RGB(128, 128, 128));
			about.pClose->Delegate(DELEGATE_EVENT_CLICK, NULL, &about, &CAboutUI::OnClose);

			ISonicImage * pImg = GetSonicUI()->CreateImage();
			pImg->Load(PNG_BG);
			ISonicWndEffect * pEffect = GetSonicUI()->CreateWndEffect();
			pEffect->Attach(hDlg, TRUE);
			pEffect->SetShapeByImage(pImg);
			pEffect->EnableWholeDrag(TRUE);
			GetSonicUI()->DestroyObject(pImg);
		}
		return TRUE;
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hDlg, &ps);
			about.pContactMe->TextOut(hdc, 50, 150, hDlg);
			about.pClose->TextOut(hdc, 50, 150 + about.pContactMe->GetHeight() + 5, hDlg);
			EndPaint(hDlg, &ps);
		}
		return TRUE;
	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) 
		{
			EndDialog(hDlg, LOWORD(wParam));
			return TRUE;
		}
		break;
	case WM_NCCALCSIZE:
		{
			return TRUE;
		}
		break;
	}
	return FALSE;
}
