//
//  Bezier.h
//  AS2_Framework
//
//  Created by Licky on 2019/1/9.
//  Copyright © 2019 NTHU. All rights reserved.
//

#ifndef Bezier_h
#define Bezier_h

#include <cstdio>

// Use this data structure to make a bezier curve
struct point
{
    float x;
    float y;
};

// simple linear interpolation between two points
void lerp(point& dest, const point& a, const point& b, const float t)
{
    dest.x = a.x + (b.x-a.x)*t;
    dest.y = a.y + (b.y-a.y)*t;
}

// evaluate a point on a bezier-curve. t goes from 0 to 1.0
void bezier(point &dest, const point& a, const point& b, const point& c, const point& d, const float t)
{
    point ab,bc,cd,abbc,bccd;
    lerp(ab, a,b,t);           // point between a and b (green)
    lerp(bc, b,c,t);           // point between b and c (green)
    lerp(cd, c,d,t);           // point between c and d (green)
    lerp(abbc, ab,bc,t);       // point between ab and bc (blue)
    lerp(bccd, bc,cd,t);       // point between bc and cd (blue)
    lerp(dest, abbc,bccd,t);   // point on the bezier-curve (black)
}

#endif /* Bezier_h */
