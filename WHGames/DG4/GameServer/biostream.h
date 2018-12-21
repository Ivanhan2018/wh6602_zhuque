/*///////////////////////////////////////////////////////////////////////////////////
 //提供二进制数据流
 //bios			基类
 //	bistream	输入流
 //	bostream	输出流
 //biosexception	异常类
 //
 //提供对所有 C 固有类型的 << 和 >> 操作符重载
 //其中对char* 的写入方式与MFC的CString兼容。在流中当第一个字节的无符号值小于255时,该值
 //表示其后字符串的字节长度；当流中第一个字节的无符号值等于255时,则其后的unsigned short
 //值为字符串长度，在该unsigned short之后为字符串内容。当unsigned short==0xffff,则其后的
 //四字节值(unsigned long)为字符串长度，在unsigned long之后是字符串内容.
 //
 //bios 提供:
 //  attach方法 将流绑定到内存缓冲区上
 //  length方法 获得流当前已使用的长度
 //  avail方法  获得缓冲区当前可用的长度
 //
 //bostream 提供:
 //  write方法    将数据写入缓冲区
 //  seekp方法	   在流中定位
 //  
 //bistream 提供:
 //  read方法	   从流中读数据
 //  seekg方法    在流中定位
 // peek         从流中预读一个字节（不影响流的当前位置）
 //
 //biosexception
 // 提供两个枚举常量
 //     end;	   从流里读数据时读到流的缓冲区末尾 由bistream.read 抛出	
 //	 overflow: 往流里写数据时超出流的缓冲区末尾 由bostream.write 抛出
 ////////////////////////////////////////////////////////////////////////////////////*/

#ifndef BIOSTREAM_H
#define	BIOSTREAM_H


#ifdef OS_WIN

#if defined (_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <cstring>
#else
#include <string.h>
#include <algorithm>
#endif

inline
bool IS_BIGENDIAN( void )
{
	unsigned short data = 0x1122;
	unsigned char* pdata= (unsigned char*)&data; 
	
	return (*pdata == 0x11);
}

inline
bool IS_SAME_ENDIAN( bool bigendian )
{
	return (bigendian&&IS_BIGENDIAN())||(!bigendian&&!IS_BIGENDIAN());
}

class agproexception
{
public:
	agproexception():m_cause(none) {}
	agproexception(int cause):m_cause(cause) {}
	enum ERRORTYPE {
		none = 0,		//no error;
		invalidcmd,		//不合法的协议类型
		rangeerror		//超出范围
	};
	int m_cause;
	
};

class biosexception 
{
public:
	enum BIOSEXCEPTION_CODE {
		none = 0,
		end = 1,		//从流里读数据时读到流的缓冲区末尾
		overflow = 2,	//往流里写数据时超出流的缓冲区末尾
		stringtoolong	//读标准C字符串时超长
	};
	
	BIOSEXCEPTION_CODE	m_cause;
	
	biosexception(BIOSEXCEPTION_CODE cause = none):m_cause(cause){}
};

class bios 
{
public:
	enum mode {	
		in = 1,		//输入流
		out = 2		//输出流
	};
	enum seek_dir {
		beg = 0,	
		cur ,
		end} ;
protected:
	char*	pbase;	//缓冲区的头指针
	char*	pcurr;	//缓冲区中的当前指针
	char*	pend;	//缓冲区的尾指针
	int		m_mode; //该流的模式 in or out
	bool	m_bigendian;// 
public:
	
	bios(bool bigendian):pbase(0),pcurr(0),pend(0),m_mode(0),m_bigendian(bigendian)
	{
	}
	
	bios(char* pch,long nLength,int mode,bool bigendian):pbase(pch),pcurr(pch),m_mode(mode),m_bigendian(bigendian)
	{
		pend = pch+nLength;
	}
	~bios() {}
	
	//将流关联到一个缓冲区
	//pch 缓冲区的开始地址
	//nLength: 缓冲区的大小
	void attach(char* pch,long nLength) {
		pbase = pcurr = pch;
		pend = pbase+nLength;
	}
	
	//将流与缓冲区脱离
	void detach() { pbase = pcurr = pend = 0 ; }
	
	//得到缓冲区剩余可用的长度
	//seek到不同的位置会影响该函数结果
	long avail() const { return (long)(pend - pcurr); }
	
	//得到缓冲区被使用的长度
	//seek到不同的位置会影响该函数结果
	long length() const { return (long)(pcurr - pbase); }
};



class bostream : public bios
{
private:
	bostream& write_order(const char* p,size_t len)
	{
		if(avail() < (long)len)
			throw biosexception(biosexception::overflow);
		
		memcpy(pcurr,p,len);
		pcurr+=len;
		
		return *this;
	}
	
	bostream& write_opposite(const char* p,size_t len)
	{
		if(avail() < (long)len)
			throw biosexception(biosexception::overflow);
		
		size_t	times	= len/sizeof(char);
		char*	pos		= (char*)p+times-1;
		
		do 
		{
			*pcurr++ = *pos--;
			times--;
		} while ( times );
		
		return *this;
	}
	
public:
	bostream(bool bigendian=false):bios(bigendian)													{ m_mode = bios::out; }
	bostream(char* pch,long nLength, bool bigendian=false):bios(pch,nLength,bios::out, bigendian)	{ }
	
	//向输出流写入内容
	//p 要写入的内容
	//len p 的长度
	bostream& write(const char* p,size_t len)
	{
		if(avail() < (long)len)
			throw biosexception(biosexception::overflow);
		
		memcpy(pcurr,p,len);
		pcurr+=len;
		
		return *this;
	}
	
	bostream& write_care_endian( const char* p,size_t len )
	{
		if ( IS_SAME_ENDIAN( m_bigendian ) )
		{
			return write_order( p, len );
		}
		else
		{
			return write_opposite( p, len );
		}
	}
	
	bostream& writecstring( const char* _s )
	{
		return operator<<(_s);
	}
	
	~bostream() {}
	
	//移动输出流的当前写入位置
	//offset: 移动的字节数
	//seekmode: 从哪个位置开始移动
	bostream& seekp(long offset,bios::seek_dir seekmode = bios::cur) {
		switch(seekmode)
		{
			case bios::beg:
				pcurr = pbase+offset;
				break;
			case bios::cur:
				pcurr += offset;
				break;
			case bios::end:
				pcurr = pend+offset;
				break;
		}
		if(avail() < 0 || avail() > (pend-pbase))
			throw biosexception(biosexception::overflow);
		return *this;
	}
	
	inline bostream& operator<<(char);
	inline bostream& operator<<(unsigned char);
	inline bostream& operator<<(signed char);
	inline bostream& operator<<(short);
	inline bostream& operator<<(unsigned short);
	inline bostream& operator<<(int);
	inline bostream& operator<<(unsigned int);
	inline bostream& operator<<(long);
	inline bostream& operator<<(unsigned long);
	inline bostream& operator<<(long long);
	inline bostream& operator<<(unsigned long long);
	inline bostream& operator<<(float);
	inline bostream& operator<<(double);
	inline bostream& operator<<(long double);
	inline bostream& operator<<(bool);
	inline bostream& operator<<(const char*);
	inline bostream& operator<<(char*);
	inline bostream& operator<<(unsigned char*);
	inline bostream& operator<<(const unsigned char*);
};

class bistream : public bios
{
private:
	bistream& read_order(char* p,size_t len)
	{
		if(avail() < (long)len)
			throw biosexception(biosexception::overflow);
		
		memcpy(p, pcurr, len);
		pcurr+=len;
		
		return *this;
	}
	
	bistream& read_opposite(char* p,size_t len)
	{
		if(avail() < (long)len)
			throw biosexception(biosexception::overflow);
		
		size_t	times	= len/sizeof(char);
		char*	pos		= (char*)p+times-1;
		
		do 
		{
			*pos-- = *pcurr++;
			times--;
		} while ( times );
		
		return *this;
	}
	
public:
	bistream(bool bigendian=false):bios(bigendian)													{ m_mode = bios::in; }
	bistream(char* pch,long nLength, bool bigendian=false):bios(pch,nLength,bios::in,bigendian)		{ }
	~bistream() {}
	
	//从输入流中读len长度的字节放入p中
    bistream& read(char* p,size_t len) {
		if(avail() < (long)len)
			throw biosexception(biosexception::end);
		
		memcpy(p,pcurr,len);
		pcurr += len;
		
		return *this;
	}
	
	bistream& read_care_endian( char* p,size_t len )
	{
		if ( IS_SAME_ENDIAN( m_bigendian ) )
		{
			return read_order( p, len );
		}
		else
		{
			return read_opposite( p, len );
		}
	}
	
	bistream& readcstring( char* _s, unsigned long _ssize )
	{
		if( _ssize<=0 ) throw agproexception(agproexception::rangeerror);
		_s[_ssize-1] = 0;
		
		unsigned char blen;
		operator>>(blen);
		if( blen < 0xff )
		{
			if ( (unsigned long)blen>=_ssize )
			{
				seekg( 0-sizeof(blen) );
				throw agproexception(agproexception::rangeerror);
			}
			return read(_s,blen);
		}
		
		unsigned short wlen;
		operator>>(wlen);
		if(wlen < 0xfffe)
		{
			if ( (unsigned long)wlen>=_ssize )
			{
				seekg( 0-sizeof(wlen) );
				throw agproexception(agproexception::rangeerror);
			}
			return read(_s,wlen);
		}
		
		unsigned long dwlen;
		operator>>(dwlen);
		if ( dwlen>=_ssize )
		{
			seekg( 0-sizeof(dwlen) );
			throw agproexception(agproexception::rangeerror);
		}
		
		return read(_s,dwlen);
	}
	
	//预读下一个字节
	int peek() {
		if(pcurr>=pend)
		{
			throw biosexception(biosexception::end);
		}
		else
			return static_cast<int>(*(pcurr));
	}
	
	//移动输入流的当前读入指针
	bistream& seekg(long offset,bios::seek_dir dir=bios::cur) {
		switch(dir)
		{
			case bios::beg:
				pcurr = pbase+offset;
				break;
			case bios::cur:
				pcurr += offset;
				break;
			case bios::end:
				pcurr = pend+offset;
				break;
		}
		if(avail() < 0 || avail() > (pend-pbase))
			throw biosexception(biosexception::end);
		return *this;
	}
	
	inline bistream& operator>>(char&);
	inline bistream& operator>>(unsigned char&);
	inline bistream& operator>>(signed char&);
	inline bistream& operator>>(short&);
	inline bistream& operator>>(unsigned short&);
	inline bistream& operator>>(int&);
	inline bistream& operator>>(unsigned int&);
	inline bistream& operator>>(long&);
	inline bistream& operator>>(unsigned long&);
	inline bistream& operator>>(long long&);
	inline bistream& operator>>(unsigned long long&);
	inline bistream& operator>>(float&);
	inline bistream& operator>>(double&);
	inline bistream& operator>>(long double&);
	inline bistream& operator>>(bool&);
	inline bistream& operator>>(char*);
	inline bistream& operator>>(unsigned char*);
	inline bistream& operator>>(const unsigned char*);
};


inline bostream& bostream::operator<<(char _c) {
	write((const char*)&_c,sizeof(char));
	return *this;
}
inline bostream& bostream::operator<<(unsigned char _c) {
	write((const char*)&_c,sizeof(unsigned char));
	return *this;
}
inline bostream& bostream::operator<<(signed char _c) {
	write((const char*)&_c,sizeof(signed char));
	return *this;
}
inline bostream& bostream::operator<<(short _s) 
{
	write_care_endian( (const char*)&_s, sizeof(short) );
	return *this;
}

inline bostream& bostream::operator<<(unsigned short _s) 
{
	write_care_endian( (const char*)&_s, sizeof(unsigned short) );
	return *this;
}
inline bostream& bostream::operator<<(int _i) {
	write_care_endian((const char*)&_i,sizeof(int));
	return *this;
}
inline bostream& bostream::operator<<(unsigned int _i) {
	write_care_endian((const char*)&_i,sizeof(unsigned int));
	return *this;
}
inline bostream& bostream::operator<<(long _l) {
	write_care_endian((const char*)&_l,sizeof(long));
	return *this;
}
inline bostream& bostream::operator<<(unsigned long _l) {
	write_care_endian((const char*)&_l,sizeof(unsigned long));
	return *this;
}
inline bostream& bostream::operator<<(long long _i64)
{
	write_care_endian((const char*)&_i64,sizeof(long long));
	return *this;
}
inline bostream& bostream::operator<<(unsigned long long _i64)
{
	write_care_endian((const char*)&_i64,sizeof(unsigned long long));
	return *this;
}
inline bostream& bostream::operator<<(float _f) {
	write_care_endian((const char*)&_f,sizeof(float));
	return *this;
}
inline bostream& bostream::operator<<(double _d) {
	write_care_endian((const char*)&_d,sizeof(double));
	return *this;
}
inline bostream& bostream::operator<<(long double _d) {
	write_care_endian((const char*)&_d,sizeof(long double));
	return *this;
}
inline bostream& bostream::operator<<(bool _b) {
	write((const char*)&_b,sizeof(bool));
	return *this;
}
inline bostream& bostream::operator<<(const char* _s) {
	int  slen = (int)strlen(_s);
	if(slen < 255)
	{
		operator<<((unsigned char)slen);
	}
	else if(slen < 0xfffe)
	{
		operator<<((unsigned char)0xff);
		operator<<((unsigned short)slen);
	}
	else
	{
		operator<<((unsigned char)0xff);
		operator<<((unsigned short)0xffff);
		operator<<((unsigned long)slen);
	}
	write((const char*)_s,slen);
	return *this;
}
inline bostream& bostream::operator<<(char* _s) {
	return operator<<((const char*)_s) ;
}
inline bostream& bostream::operator<<(unsigned char* _s) {
	return operator<<((const char*)_s);
}
inline bostream& bostream::operator<<(const unsigned char* _s) {
	return operator<<((const char*)_s);
}

inline bistream& bistream::operator>>(char& _c) {
	read(&_c,sizeof(char));
	return *this;
}
inline bistream& bistream::operator>>(signed char& _c) {
	read((char*)&_c,sizeof(signed char));
	return *this;
}
inline bistream& bistream::operator>>(unsigned char& _c) {
	read((char*)&_c,sizeof(unsigned char));
	return *this;
}
inline bistream& bistream::operator>>(short& _s) {
	read_care_endian((char*)&_s,sizeof(short));
	return *this;
}
inline bistream& bistream::operator>>(unsigned short& _s) {
	read_care_endian((char*)&_s,sizeof(unsigned short));
	return *this;
}
inline bistream& bistream::operator>>(int& _i) {
	read_care_endian((char*)&_i,sizeof(int));
	return *this;
}
inline bistream& bistream::operator>>(unsigned int& _i) {
	read_care_endian((char*)&_i,sizeof(unsigned int));
	return *this;
}
inline bistream& bistream::operator>>(long& _l) {
	read_care_endian((char*)&_l,sizeof(long));
	return *this;
}
inline bistream& bistream::operator>>(unsigned long& _l) {
	read_care_endian((char*)&_l,sizeof(unsigned long));
	return *this;
}
inline bistream& bistream::operator>>(long long& _i64) {
	read_care_endian((char*)&_i64,sizeof(long long));
	return *this;
}
inline bistream& bistream::operator>>(unsigned long long& _i64)
{
	read_care_endian((char*)&_i64,sizeof(unsigned long long));
	return *this;
}
inline bistream& bistream::operator>>(float& _f) {
	read_care_endian((char*)&_f,sizeof(float));
	return *this;
}
inline bistream& bistream::operator>>(double& _d) {
	read_care_endian((char*)&_d,sizeof(double));
	return *this;
}
inline bistream& bistream::operator>>(long double& _d) {
	read_care_endian((char*)&_d,sizeof(long double));
	return *this;
}
inline bistream& bistream::operator>>(bool& _b) {
	read((char*)&_b,sizeof(bool));
	return *this;
}
inline bistream& bistream::operator>>(char* _s) {
	unsigned char blen;
	operator>>(blen);
	if(blen < 0xff)
		return read(_s,blen);
    
	unsigned short wlen;
	operator>>(wlen);
	if(wlen < 0xfffe)
		return read(_s,wlen);
    
	unsigned long dwlen;
	operator>>(dwlen);
	return read(_s,dwlen);
}
inline bistream& bistream::operator>>(unsigned char* _s) {
	return operator>>((char*)_s);
}
inline bistream& bistream::operator>>(const unsigned char* _s) {
	return operator>>((char*)_s);
}

#endif

