#ifndef READ_FIXCARD_HEAD_FILE
#define READ_FIXCARD_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include <string>
#include <algorithm>
#include <map>
#include <vector>
using namespace std ;  // 类前一定要加

#define MAX_READ_BUFFER		4096
#define F_TAB				0x09	// Tab 
#define F_DOT				0x2C	// 符英文逗号
#define F_SPACE				0x20	// 空格
#define F_RETURN_CAR		0x0D	// 回车
#define F_EXCHANGE_LINE		0x0A	// 换行

class CReadFixCard
{
public:
	CReadFixCard();
	CReadFixCard(BYTE byAllCardNumber,BYTE byHeadSize) ;
	~CReadFixCard(void);
	
	// 初始化
	void fnInit(BYTE byAllCardNumber,BYTE byHeadSize);

	// 两位十六进制的字串转成数字
	int fnStringToByte(string strTwoBit);

	// 设置十六进制字符的 Map 表 
	void fnSetHexStringMap();

	//读文件，将值写入扑克数组
	BOOL fnReadFile(const char* pFileName);
	
	// 返回文件的头
	int fnGetHead(BYTE* byHeads);
	
	// 返回牌
	int fnGetCards(BYTE* byCards);

	vector<string> vec;
	map<char,BYTE> hexStringMap;

public:
	BYTE m_byAllCardNumber;	// 发牌的张数
	BYTE m_byHeadSize;		// 头的长度

	BYTE m_byHeads[10];
};

#endif
