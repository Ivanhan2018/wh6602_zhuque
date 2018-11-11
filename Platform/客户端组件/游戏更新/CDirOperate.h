//////////////////////////////////////////////////////////
// CDirOperate.h文件

#ifndef __CDIROPERATE_H_
#define __CDIROPERATE_H_
#pragma once

#include <afxwin.h>

class CDirOperate
{
public:
	CDirOperate();
	virtual ~CDirOperate();

	//创建由字符串“lpszDir”指定的目录
	static void CreateAllDirectory(LPCTSTR lpszDir);
	//删除“lpszDir”整个目录
	static void DeleteAllDirectory(LPCTSTR lpszDir);

protected:
	//判断一个目录是否存在
	static BOOL IsDirExist(LPCTSTR lpszDir);
	//递归循环删除指定目录下的所有文件及其子目录
	static void RecursiveDelete(LPCTSTR lpszPath); 
};

#endif