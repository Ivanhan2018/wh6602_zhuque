
#ifndef IP_FINDER_H
#define IP_FINDER_H

#include <string>
#include "atlstr.h"

// 需要下载 【QQ IP数据库 20120320 纯真版】，安装后提取 qqwry.dat 文件，并改名成 qqwry__勿删.dat


/*		已知的IP，用于测试代码
电信
	222.240.205.10
	222.240.205.11
	222.240.205.12
	222.240.205.13
	222.240.205.14
	222.240.205.150
	222.240.205.246
联通或网通
	58.20.54.237
	58.20.54.238
	58.20.54.239
	58.20.54.240
	58.20.54.241
铁通
	59.191.45.0
	59.191.47.255
*/

enum
{
	IP_TYPE_Unknow = 0,				// 未知
	IP_TYPE_China_Telecom = 1,		// 中国电信
	IP_TYPE_China_Unicom = 2,		// 中国联通，联通、网通已经合并了，是一家了
	IP_TYPE_China_Railcom = 3,		// 中国铁通
};

const int INDEX_LENGTH = 7;        // 一个索引包含4字节的起始IP和3字节的IP记录偏移，共7字节
const int IP_LENGTH = 4;
const int OFFSET_LENGTH = 3;

enum 
{
	REDIRECT_MODE_1 = 0x01,    // 重定向模式1 偏移量后无地区名
	REDIRECT_MODE_2 = 0x02,    // 重定向模式2 偏移量后有地区名
};

class CIpFinder
{
public:
	CIpFinder();
	CIpFinder(const char* pszFileName);
	~CIpFinder();

	// 获取ip国家名、地区名
	void GetAddressByIp(unsigned long ipValue, std::string& strCountry, std::string& strLocation) const;
	void GetAddressByIp(const char* pszIp, std::string& strCountry, std::string& strLocation) const;
	void GetAddressByOffset(unsigned long ulOffset, std::string& strCountry, std::string& strLocation) const;

	unsigned long GetString(std::string& str, unsigned long indexStart) const;
	unsigned long GetValue3(unsigned long indexStart) const;
	unsigned long GetValue4(unsigned long indexStart) const;

	// 转换
	unsigned long IpString2IpValue(const char *pszIp) const;
	void IpValue2IpString(unsigned long ipValue, char* pszIpAddress, int nMaxCount) const;
	bool IsRightIpString(const char* pszIp) const;

	// 输出数据
	unsigned long OutputData(const char* pszFileName, unsigned long ulIndexStart = 0, unsigned long ulIndexEnd = 0) const;
	unsigned long OutputDataByIp(const char* pszFileName, unsigned long ipValueStart, unsigned long ipValueEnd) const;
	unsigned long OutputDataByIp(const char* pszFileName, const char* pszStartIp, const char* pszEndIp) const;

	unsigned long SearchIp(unsigned long ipValue, unsigned long indexStart = 0, unsigned long indexEnd = 0) const;
	unsigned long SearchIp(const char* pszIp, unsigned long indexStart = 0, unsigned long indexEnd = 0) const;

	bool Open(const char* pszFileName);

	// 获得IP类型，即获得电信、联通、铁通
	int GetIpType(const char* szInfo);

private:

	FILE *m_fpIpDataFile;				// IP数据库文件
	unsigned long m_indexStart;    // 起始索引偏移
	unsigned long m_indexEnd;      // 结束索引偏移
};

#endif