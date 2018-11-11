// LanguageList.cpp : 实现文件
//

#include "stdafx.h"
#include "LanguageList.h"
#include ".\languagelist.h"

#include "MemDC.h"

//一页总显示记录数
const int UI_SHOW_NUMS	=	10;
//一条记录高度
const int UI_INFO_HEIGHT =	20;


//ID
const int ID_BT_LEFT	=	1000;
const int ID_BT_RIGHT	=	1001;


// CLanguageList 对话框

IMPLEMENT_DYNAMIC(CLanguageList, CDialog)
CLanguageList::CLanguageList(CWnd* pParent /*=NULL*/)
	: CDialog(CLanguageList::IDD, pParent)
{
	//列表信息
	m_vecListInfo.clear();
	//总页数
	m_dwTotalPageNums  = 0;
	//当前页数
	m_dwCurPageIndex = 0;
	//鼠标当前Index
	m_dwMouseCurIndex = 0;
	//起点
	m_dwIndexStart	=	0;
	//终点
	m_dwIndexEnd	=	0;

	m_pClientKernel = NULL;
}

CLanguageList::~CLanguageList()
{
	m_vecListInfo.clear();	//列表信息
	::DeleteObject(m_TextFont);	//删除字体

	m_pClientKernel = NULL;
}

void CLanguageList::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CLanguageList, CDialog)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_MESSAGE(WM_MOUSELEAVE,OnMouseLeave)
	ON_WM_PAINT()
	ON_BN_CLICKED(ID_BT_LEFT, OnLeft)
	ON_BN_CLICKED(ID_BT_RIGHT, OnRight)
	ON_WM_ERASEBKGND()
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CLanguageList 消息处理程序

BOOL CLanguageList::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	//列表信息
	m_vecListInfo.clear();
	//总页数
	m_dwTotalPageNums  = 0;
	//当前页数
	m_dwCurPageIndex = 0;
	//鼠标当前Index
	m_dwMouseCurIndex = -1;
	//起点
	m_dwIndexStart	=	0;
	//终点
	m_dwIndexEnd	=	0;


	//设置显示区域
	m_rcShowRect.left = 9;
	m_rcShowRect.top = 31;
	m_rcShowRect.right = m_rcShowRect.left + 164;
	m_rcShowRect.bottom = m_rcShowRect.top + 200;
	//页显示区域
	m_rcPageShorRect.left = 80;
	m_rcPageShorRect.top = 236;
	m_rcPageShorRect.right = m_rcPageShorRect.left + 109;
	m_rcPageShorRect.bottom = m_rcPageShorRect.top + 20;

	//加载图片资源
	TCHAR szResourcePath[MAX_PATH];	GetCurrentDirectory(MAX_PATH,szResourcePath);
	CString strPath(szResourcePath);
	strPath.Append("\\data\\gameframe\\plaza\\");
	
	m_BackImage.LoadImage(TEXT(strPath + "llback.png"));
	m_MoveImage.LoadImage(TEXT(strPath + "llmovesel.png"));

	//创建按钮
	CRect rcCreate(0, 0, 0, 0);
	m_btLeft.Create(TEXT(""),WS_CHILD|WS_VISIBLE|WS_CLIPSIBLINGS,rcCreate,this,ID_BT_LEFT);
	m_btLeft.SetButtonImage(TEXT(strPath + "llleft.png"), false, 4);
	m_btLeft.SetWindowPos(NULL, 50, 234, 19, 15, SWP_SHOWWINDOW);
	m_btLeft.EnableWindow(FALSE);
	
	m_btRight.Create(TEXT(""),WS_CHILD|WS_VISIBLE|WS_CLIPSIBLINGS,rcCreate,this,ID_BT_RIGHT);
	m_btRight.SetButtonImage(TEXT(strPath + "llright.png"), false, 4);
	m_btRight.SetWindowPos(NULL, 110, 234, 19, 15, SWP_SHOWWINDOW);
	m_btRight.EnableWindow(FALSE);

	CRect rect;
	GetClientRect(&rect);
	//创建字体
	m_TextFont.CreateFont(-12,0,0,0,400,0,0,0,134,3,2,1,2,TEXT("宋体"));

	SetImageRgn(m_hWnd, &m_BackImage, SIRT_DEBURRING_DOT_LESS);

	m_bIsCanSend = true;
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CLanguageList::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	CDialog::OnLButtonDown(nFlags, point);
}

void CLanguageList::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (m_bIsCanSend)
	{
		int iIndex = CheckMousePos(point);
		if (iIndex>=0 && iIndex<UI_SHOW_NUMS && m_dwCurPageIndex>=0)
		{
			iIndex = iIndex+(m_dwCurPageIndex-1)*UI_SHOW_NUMS;
			if (iIndex<m_vecListInfo.size())
			{
				((CGameFrameDlg*)GetParent())->SendLanguageListMsg(m_vecListInfo[iIndex].uiIndex);
				//发送数据
				//if (NULL != m_pClientKernel)
				//{
				//	m_pClientKernel->SendLanguageListMsg(m_vecListInfo[iIndex].uiIndex);
				//}
				ShowWindow(SW_HIDE);

				m_bIsCanSend = false;
				KillTimer(1);
				SetTimer(1, 30000, NULL);
			}
		}
	}
	else
	{
		ShowWindow(SW_HIDE);
		((CGameFrameDlg*)GetParent())->InsertShtickString("发送过于频繁，请稍后再发！", RGB(0, 255, 0), true);
	}
	
	CDialog::OnLButtonUp(nFlags, point);
}

void CLanguageList::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	int iIndex = CheckMousePos(point);
	if (iIndex>=0 && iIndex<UI_SHOW_NUMS)
	{
		m_dwMouseCurIndex = iIndex;
		InvalidateRect(&m_rcShowRect);
	}
	CDialog::OnMouseMove(nFlags, point);
}

//鼠标离开消息
LRESULT CLanguageList::OnMouseLeave(WPARAM wparam, LPARAM lparam)
{
	m_dwMouseCurIndex = -1;
	InvalidateRect(&m_rcShowRect);
	return 0;
}

void CLanguageList::OnLeft()
{
	m_btLeft.EnableWindow(FALSE);
	m_btRight.EnableWindow(FALSE);
	m_dwMouseCurIndex = -1;
	if (m_dwCurPageIndex>1)
	{
		--m_dwCurPageIndex;
		//计算起终点
		CountSEPoint();
		if (m_dwCurPageIndex>1)
		{
			m_btLeft.EnableWindow(TRUE);
		}
		if (m_dwCurPageIndex<m_dwTotalPageNums)
		{
			m_btRight.EnableWindow(TRUE);
		}
		InvalidateRect(&m_rcShowRect);
		InvalidateRect(&m_rcPageShorRect);
	}
	else if (m_dwTotalPageNums>1)
	{
		m_btRight.EnableWindow(TRUE);
	}
}
void CLanguageList::OnRight()
{
	m_btLeft.EnableWindow(FALSE);
	m_btRight.EnableWindow(FALSE);
	m_dwMouseCurIndex = -1;
	if (m_dwCurPageIndex<m_dwTotalPageNums)
	{
		++m_dwCurPageIndex;
		//计算起终点
		CountSEPoint();
		if (m_dwCurPageIndex>1)
		{
			m_btLeft.EnableWindow(TRUE);
		}
		if (m_dwCurPageIndex<m_dwTotalPageNums)
		{
			m_btRight.EnableWindow(TRUE);
		}
		InvalidateRect(&m_rcShowRect);
		InvalidateRect(&m_rcPageShorRect);
	}
	else if (m_dwTotalPageNums>1)
	{
		m_btLeft.EnableWindow(TRUE);
	}
}

void CLanguageList::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 在此处添加消息处理程序代码
	
	CRect	rect;
	GetClientRect(&rect);
	CMemDC dcMem(&dc, rect, true);	// 用单色填充背景

	//绘制背景
	if (!m_BackImage.IsNull())
	{
		m_BackImage.DrawImage(&dcMem, 0, 0);
	}

	DWORD iTotalNums = m_vecListInfo.size();
	if (iTotalNums > 0)
	{
		CFont* pOldFont = dcMem.SelectObject(&m_TextFont);
		dcMem.SetTextColor(RGB(255, 255, 255));
		dcMem.SetBkMode(TRANSPARENT);

		int iCount = 0;
		for (DWORD i=m_dwIndexStart; i<iTotalNums&&i<m_dwIndexEnd; i++)
		{
			CRect rect(&m_vecListInfo[i].rcArea);
			if (iCount == m_dwMouseCurIndex)
			{
				if (!m_MoveImage.IsNull())
				{
					//绘制移动光标
					m_MoveImage.DrawImage(&dcMem, rect.left, rect.top);
				}
			}

			//绘制文本
			rect.left += 2;
			rect.top += 2;
			dcMem.DrawText(m_vecListInfo[i].chValue, &rect, DT_LEFT|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);

			++iCount;
		}

		dcMem.SetTextColor(RGB(200, 0, 0));
		char chData[10]={0};
		sprintf(chData, "%d/%d", m_dwCurPageIndex, m_dwTotalPageNums);
		dcMem.TextOut(m_rcPageShorRect.left, m_rcPageShorRect.top, chData);

		dcMem.SelectObject(pOldFont);
		pOldFont = NULL;
	}
}

//初始化数据
void CLanguageList::InitData()
{
	//列表信息
	m_vecListInfo.clear();
	//总页数
	m_dwTotalPageNums  = 0;
	//当前页数
	m_dwCurPageIndex = 0;
	//鼠标当前Index
	m_dwMouseCurIndex = -1;
	//起点
	m_dwIndexStart	=	0;
	//终点
	m_dwIndexEnd	=	0;
	
	//开始读取配置文件
	CString strLclPth="", strFlPth="";
	GetModuleFileName(NULL, strLclPth.GetBufferSetLength(256), 256);
	strFlPth = strLclPth.Left(strLclPth.ReverseFind('\\')+1)+ TEXT("config\\LanguageList.ini");
	
	char chData[512]={0};
	DWORD uiCountNums = 0;
	int	iCountIndex = 0;
	CString strValue="";
	
	while (true)
	{
		::memset(chData, 0, sizeof(chData));
		++uiCountNums;
		strValue = "";
		
		LListInfo info;
		::memset(&info, 0, sizeof(LListInfo));
		::sprintf(chData, "LL_%d", uiCountNums);
		strValue.Append(chData);
		::memset(chData, 0, sizeof(chData));
		UINT uiReturn = GetPrivateProfileString(TEXT("LanguageList"), strValue, TEXT(""), chData, sizeof(chData), strFlPth);
		if (uiReturn>0 && '0'!=chData[0] && uiReturn<512)
		{//有值

			//序号
			info.uiIndex = uiCountNums;
			//键名
			::sprintf(info.chName, "LL_%d", uiCountNums);
			//键值
			::memcpy(info.chValue, chData, ::strlen(chData));
			//设置显示区域
			info.rcArea.left = m_rcShowRect.left;
			info.rcArea.right = m_rcShowRect.right;
			info.rcArea.top = m_rcShowRect.top+iCountIndex*UI_INFO_HEIGHT;
			info.rcArea.bottom = info.rcArea.top + UI_INFO_HEIGHT;

			//插入到列表
			m_vecListInfo.push_back(info);
		}
		else
		{
			--uiCountNums;
			break;
		}
		++iCountIndex;
		if (iCountIndex>=UI_SHOW_NUMS)
		{//换页了
			iCountIndex = 0;
		}
	};
	
	if (uiCountNums<=0 || m_vecListInfo.size()<=0 || uiCountNums!=m_vecListInfo.size())
	{//没有记录
		return;
	}
	
	//总页数
	int iPageValue = (uiCountNums/UI_SHOW_NUMS);
	m_dwTotalPageNums  = (0==(uiCountNums%UI_SHOW_NUMS))?(iPageValue):(iPageValue+1);
	//当前页数
	m_dwCurPageIndex = 1;
	//鼠标当前Index
	m_dwMouseCurIndex = -1;
	//计算起终点
	CountSEPoint();
	m_btLeft.EnableWindow(FALSE);
	m_btRight.EnableWindow(FALSE);
	if (m_dwTotalPageNums>1)
	{
		m_btRight.EnableWindow(TRUE);
	}

	InvalidateRect(NULL);
}

//检测鼠标位置
int CLanguageList::CheckMousePos(const CPoint& point)
{
	int iPos = -1;
	if (m_rcShowRect.PtInRect(point))
	{
		int iTotalNums = m_vecListInfo.size();
		if (iTotalNums > 0)
		{
			int iCount = 0;
			for (DWORD i=m_dwIndexStart; i<iTotalNums&&i<m_dwIndexEnd; ++i)
			{
				if (m_vecListInfo[i].rcArea.PtInRect(point))
				{
					iPos = iCount;
					break;
				}
				++iCount;
			}
		}
	}
	return iPos;
}


//计算起点与终点
void CLanguageList::CountSEPoint()
{
	int iTotalNums = m_vecListInfo.size();
	if (iTotalNums>0 && m_dwTotalPageNums>0 
		&& m_dwCurPageIndex>0 && m_dwCurPageIndex<=m_dwTotalPageNums)
	{
		m_dwIndexStart = (m_dwCurPageIndex-1)*UI_SHOW_NUMS;
		m_dwIndexEnd = m_dwIndexStart + (((iTotalNums-m_dwIndexStart)>=UI_SHOW_NUMS)?(UI_SHOW_NUMS):(iTotalNums-m_dwIndexStart));
	}
}

//
//
//BOOL CLanguageList::SetImageRgn(CPngImage * pImage)
//{
//	if (NULL==pImage) return FALSE;
//	if (pImage->IsNull()) return FALSE;
//	long wWidth = pImage->GetWidth();
//	long wHeight = pImage->GetHeight();
//
//	CRgn rgnImage, tempRgn;
//	rgnImage.CreateRectRgn(0, 0, 0, 0);
//
//	int leftX = 0;
//	int rightX = 0;
//	for (int i=wHeight-1; i>=0; --i)
//	{
//		for (int j = 0; j<wWidth; ++j)
//		{
//			leftX = j;
//			rightX = j;
//			for (int k=j; k<wWidth; ++k)  //记住左边的x
//			{
//				BYTE byValue = pImage->GetAlphaValue(k, i);
//				if (byValue != 0)
//				{
//					leftX = k;
//					break;
//				}
//			}
//			for (int k=leftX+1; k<wWidth; ++k)
//			{
//				BYTE byValue = pImage->GetAlphaValue(k, i);
//				if (byValue == 0 || (k == wWidth -1 && byValue!=0))
//				{
//					rightX = k;
//					break;
//				}
//			}
//
//			j = leftX > rightX ? leftX : rightX;
//
//			if (rightX > leftX)
//			{
//				tempRgn.CreateRectRgn(leftX, (wHeight - i-1) , rightX, (wHeight -i));
//				rgnImage.CombineRgn(&rgnImage, &tempRgn, RGN_OR);
//				tempRgn.DeleteObject();
//			}
//		}
//	}
//
//	if (NULL == rgnImage.m_hObject)
//		return FALSE;
//
//	SetWindowRgn((HRGN)rgnImage.m_hObject, TRUE);
//	rgnImage.DeleteObject();
//	return TRUE;
//}


BOOL CLanguageList::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	return TRUE;
	//return CDialog::OnEraseBkgnd(pDC);
}


//设置客户端内核
void CLanguageList::SetClientKernel(IClientKernel * pIClientKernel)
{
	m_pClientKernel = pIClientKernel;
}


//获得语音
void CLanguageList::GetLanguage(DWORD dwIndex, CString& strName, CString& strValue)
{
	strName = "";
	strValue = "";
	std::vector<LListInfo>::iterator iter;
	for (iter=m_vecListInfo.begin(); iter!=m_vecListInfo.end(); ++iter)
	{
		if (iter->uiIndex == dwIndex)
		{
			strName.Append(iter->chName);
			strValue.Append(iter->chValue);
			break;
		}
	}
}


void CLanguageList::OnTimer(UINT nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	
	if (1 == nIDEvent)
	{
		m_bIsCanSend = true;
		KillTimer(1);
	}
	CDialog::OnTimer(nIDEvent);
}
