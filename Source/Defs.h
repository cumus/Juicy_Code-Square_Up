#ifndef __DEFS_H__
#define __DEFS_H__

#include <stdio.h>

// Deletes a buffer
#define DEL( x ) \
    {                        \
    if( x != nullptr )        \
	    {                      \
      delete x;            \
	  x = nullptr;              \
	    }                      \
    }

// Deletes an array of buffers
#define DEL_ARRAY( x ) \
    {                              \
    if( x != nullptr )              \
	    {                            \
      delete[] x;                \
	  x = nullptr;                    \
	    }                            \
                              \
    }

#define IN_RANGE( value, min, max ) ( ((value) >= (min) && (value) <= (max)) ? 1 : 0 )
#define MIN( a, b ) ( ((a) < (b)) ? (a) : (b) )
#define MAX( a, b ) ( ((a) > (b)) ? (a) : (b) )
#define TO_BOOL( a )  ( (a != 0) ? true : false )

template <class VALUE_TYPE> void SWAP(VALUE_TYPE& a, VALUE_TYPE& b)
{
	VALUE_TYPE tmp = a;
	a = b;
	b = tmp;
}

// Standard string size
#define SHORT_STR	32
#define MID_STR		255
#define HUGE_STR	8192

// Trigonometry
#define DEGTORAD 0.0174532925199432957f
#define RADTODEG 57.295779513082320876f
#define PI 3.14159265358979323846f
#define TWO_PI 6.28318530717958647692f
#define HALF_PI 1.57079632679489661923f
#define QUARTER_PI 0.78539816339744830961f
#define INV_PI 0.31830988618379067154f
#define INV_TWO_PI 0.15915494309189533576f

// Memory Units
#define KILOBYTE 1024
#define MEGABYTE 1048576
#define GIGABYTE 1073741824
#define KILOBYTE_F 1024.0
#define MEGABYTE_F 1048576.0
#define GIGABYTE_F 1073741824.0

#endif // __DEFS_H__