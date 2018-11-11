/*
**	def.h
**	created by weni23
**	2011-10-14
**
**	一些有用的宏定义
*/

#ifndef __PD_DEF_H__
#define __PD_DEF_H__

#ifndef NULL
#define NULL 0
#endif

//#define SAFE_RELEASE(p){if((p)) (p)->Release();}
#define SAFE_UNLOAD(p) {if((p)) (p)->UnLoad();}
#define SAFE_DELETE(p) {if((p)) delete (p); (p) = NULL;}

struct qcRect 
{
	float x, y, w, h;
	qcRect() : x(0), y(0), w(0), h(0) {}
	qcRect(float _x, float _y, float _w, float _h) : x(_x), y(_y), w(_w), h(_h) {}
};

struct qcPoint
{
	float x, y;
	qcPoint() : x(0), y(0) {}
	qcPoint(float _x, float _y) : x(_x), y(_y) {}
	qcPoint& operator= (const qcPoint& point) {x = point.x; y = point.y; return *this;}
};

struct qcLine
{
	qcPoint point[2];
	qcLine() {memset(point, 0, sizeof(qcPoint)*2);}
	qcLine(const qcPoint& p1, const qcPoint& p2) { point[0] = p1; point[1] = p2;}
	qcLine& operator= (const qcLine& line) { point[0] = line.point[0]; point[1] = line.point[1]; return *this;}
};

struct qcQuad
{
	qcPoint point[4];
	qcQuad() {memset(point, 0, sizeof(qcPoint)*4);}
	qcQuad(const qcPoint& p1, const qcPoint& p2, const qcPoint& p3, const qcPoint& p4) 
	{
		point[0] = p1; point[1] = p2; point[2] = p3; point[3] = p4;
	}
	qcQuad& operator= (const qcQuad& quad) { for(int i=0; i<4; ++i) point[i]	= quad.point[i]; return *this;}
};

#endif