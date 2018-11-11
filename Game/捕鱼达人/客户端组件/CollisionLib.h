/*
**	CollisionLib.h
**	created by weni23
**	2011-10-16
**
**	简单碰撞检测库，慢慢加
*/

#ifndef __QC_COLLISIONLIB_H__
#define __QC_COLLISIONLIB_H__

#include "def.h"


//将一个线段转为矩形
qcRect Line_conv2_Rect(const qcLine& line);
qcRect Line_conv2_Rect(const qcPoint& p1, const qcPoint& p2);

//将一个矩形结构体转化为四边形结构体
qcQuad Rect_conv2_Quad(const qcRect& rect);

//////////////////////////////////////////////////////////////////////////

bool Point_hit_Rect(const qcPoint& p, const qcRect& rect);

//检测线段和线段是否相交
bool Line_hit_Line(const qcLine& line1, const qcLine& line2);

bool Rect_hit_Rect(const qcRect& rect1, const qcRect& rect2);

// 这个函数需要人为的保证多边形的点序列能组成凸多边形
bool Quad_hit_Quad(const qcQuad& quad1, const qcQuad& quad2);

#endif//__QC_COLLISIONLIB_H__