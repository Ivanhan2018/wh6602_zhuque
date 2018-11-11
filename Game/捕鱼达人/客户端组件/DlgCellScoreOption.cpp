#include "stdafx.h"
#include "resource.h"
#include "DlgCellScoreOption.h"
#include ".\dlgcellscoreoption.h"

IMPLEMENT_DYNAMIC(CDlgCellScoreOption, CSkinDialogEx)
BEGIN_MESSAGE_MAP(CDlgCellScoreOption, CSkinDialogEx)
END_MESSAGE_MAP()

//构造函数
CDlgCellScoreOption::CDlgCellScoreOption(CWnd* pParent): CSkinDialogEx(IDD_CELL_SCORE_OPTION, pParent)
{
	m_lCellScoreSelect=0L;
	m_pCenterView=NULL;
}

//析构函数
CDlgCellScoreOption::~CDlgCellScoreOption()
{
}

//控件绑定
void CDlgCellScoreOption::DoDataExchange(CDataExchange* pDX)
{
	__super::DoDataExchange(pDX);
	DDX_Control(pDX, IDOK, m_btOK);	
	DDX_Control(pDX, IDC_CELL_SCORE, m_ComBoxCellScore);
}

//确定函数
void CDlgCellScoreOption::OnOK()
{
	//更新变量
	UpdateData(TRUE);

	//获取选项
	int nItem=m_ComBoxCellScore.GetCurSel();
	m_lCellScoreSelect=LONG(m_ComBoxCellScore.GetItemData(nItem));
	
	//调用父类
	__super::OnOK();
}

//初始函数
BOOL CDlgCellScoreOption::OnInitDialog()
{
	//调用父类
	__super::OnInitDialog();

	//设置标题
	SetWindowText(TEXT("设置倍数"));

	//单元积分
	TCHAR szCellScore[128];
	for (int i=0; i<CountArray(m_lCellScore); ++i)
	{
		int nItem=m_ComBoxCellScore.AddString(_itoa(m_lCellScore[i],szCellScore,10));
		if (CB_ERR!=nItem) m_ComBoxCellScore.SetItemData(nItem,m_lCellScore[i]);
	}
	m_ComBoxCellScore.SetCurSel(0);

	//设置变量
	m_lCellScoreSelect=m_lCellScore[0];

	//居中窗体
	if (NULL!=m_pCenterView) CenterWindow(m_pCenterView);

	return TRUE;
}
