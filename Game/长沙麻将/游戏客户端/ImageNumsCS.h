#ifndef ___IMAGE_NUMS_H_CS_DEF___
#define ___IMAGE_NUMS_H_CS_DEF___

#include "SkinControls.h"
#include <vector>
//Author: OSC
//Time: 2011-3-23
//图片数字功能.
//1:绘制所有数字
//2:绘制符号.
//3:绘制分隔符.
//4:具有左中右对齐方式

using namespace std;

//enum NumAlign
//{
//	IA_LEFT	=	0,
//	IA_CENTER,
//	IA_RIGHT
//};


//数字图片类
class CImageNumsCS
{
public:
	CImageNumsCS(void);
	virtual ~CImageNumsCS(void);

public:

	//设置数字
	void	SetNums(long lNums);
	long	GetNums(void)	{	return m_lNums;	}
	int		GetBitNums(void)	{	return (int)m_vecNumArray.size();	}

	//设置有无符号
	void	SetSign(bool bIsSign=false);
	bool	GetSign(void)	{	return m_bIsSign;}

	//设置分隔符号
	void	SetSeparator(bool bIsSeparator=false);
	bool	GetSeparator(void){	return	m_bIsSeparator;	}

	//设置对齐试
	void	SetNumAlign(NumAlign align=IA_LEFT);
	NumAlign	GetNumAlign(void)	{	return m_enNumAlign;	}

	//设置分隔符的宽与高
	void	SetSeparatorWH(int iWidth, int iHeight);

	//设置图片
	void	SetImageResource(char* pResource);

	//设置数字显示区域
	void	SetShowRect(CRect rect);

	//绘制数字
	void	DrawImageNums(CDC* pDC);

	
private:
	long		m_lNums;		//数字
	bool		m_bIsSign;		//有无符号
	bool		m_bIsSeparator;	//有无分隔符
	NumAlign	m_enNumAlign;	//对齐方式
	CRect		m_rcNumsArea;	//绘制数字区域
	CPngImage	m_ImageNums;	//数字图片
	long		m_lNumWidth;	//数字宽度
	long		m_lNumHeight;	//数字高度
	long		m_lSepatatorW;	//分隔符宽
	long		m_lSepatatorH;	//分隔符高


private:
	std::vector<int>	m_vecNumArray;	//保存数字
};



#endif
