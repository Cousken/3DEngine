#ifndef _MACROS_H_
#define _MACROS_H_
#include <iostream>
#include <sstream>
#include <string>
#include "debugHeader.h"

#define MIN( obj1, obj2 ) (( obj1 < obj2 ) ? obj1: obj2)
#define MAX( obj1, obj2 ) (( obj1 < obj2 ) ? obj2: obj1)
#define SAFE_DELETE(aPointer) { if(aPointer) { delete(aPointer); aPointer = NULL; } }
#define SAFE_ARRAY_DELETE(p)       { if(p) { delete[] (p);     (p)=NULL; } }

#define HW_COLOR(aColor) ((unsigned long(aColor.a)<<24) + (unsigned long(aColor.r)<<16) + (unsigned long(aColor.g)<<8) + unsigned long(aColor.b))


#define GETA(col)      ((col)>>24)
#define GETR(col)      (((col)>>16) & 0xFF)
#define GETG(col)      (((col)>>8) & 0xFF)
#define GETB(col)      ((col) & 0xFF)


template <typename T> 
inline T Clamp(const T& value, const T& low, const T& high) 
{
	return value < low ? low : (value > high ? high : value); 
}

template <typename T> 
inline T Random(T aMin, T aMax) 
{

	const int min = static_cast<int>(aMin*1000);
	const int max = static_cast<int>(aMax*1000);

	if (min == max)
	{
		return aMin;
	}

	MTRand rand;
	const int random = rand.randInt(max - min) + min;
	return static_cast<T>(random) * 0.001f;
}

template<class T>
inline T ConvertFromString(const std::string& aString)
{
	std::istringstream stream(aString);
	T value;
	stream >> value;
	return value;
}

inline float StringToFloat(const std::string& aString)
{
	return ConvertFromString<float>(aString);
}

inline int StringToInt(const std::string& aString)
{
	return ConvertFromString<int>(aString);
}

#endif // _MACROS_H_