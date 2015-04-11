#ifndef _VECTOR_2_H_
#define _VECTOR_2_H_

#include <cmath>
#include <cassert>


template <typename T>
class Vector2
{
public:
	T x;
	T y;


	Vector2()
		:x(0)
		,y(0)
	{

	}

	Vector2(const T &anX, const T &anY)
		: x(anX)
		, y(anY)
	{

	}

	inline Vector2<T> &operator+=(const Vector2<T> &aVector2)
	{
		x += aVector2.x;
		y += aVector2.y;

		return *this;
	}

	inline Vector2<T> &operator-=(const Vector2<T> &aVector2)
	{
		x -= aVector2.x;
		y -= aVector2.y;

		return *this;
	}

	Vector2<T> &operator*=(const T &aValue)
	{
		x *= aValue;
		y *= aValue;

		return *this;
	}

	Vector2<T> &operator*=(const Vector2<T> &aVector2)
	{
		x *= aVector2.x;
		y *= aVector2.y;

		return *this;
	}

	Vector2<T> &operator/=(const T &aValue)
	{
		assert(aValue != 0 && "DIVISION BY ZERO (aValue == 0)");

		x /= aValue;
		y /= aValue;

		return *this;
	}

	inline Vector2<T> operator+(const Vector2<T> &aVector2) const
	{
		return Vector2<T>(x + aVector2.x, y + aVector2.y);
	}

	inline Vector2<T> operator-(const Vector2<T> &aVector2) const
	{
		return Vector2<T>(x - aVector2.x, y - aVector2.y);
	}

	inline Vector2<T> operator*(const T &aValue) const
	{
		return Vector2<T>(x * aValue, y * aValue);
	}

	inline Vector2<T> operator*(const Vector2<T> &aVector2) const
	{
		return Vector2<T>(x * aVector2.x, y * aVector2.y);
	}

	inline Vector2<T> operator/(const T &aValue) const
	{
		assert(aValue != 0 && "DIVISION BY ZERO (aValue == 0)");
		return Vector2<T>(x / aValue, y / aValue);
	}

	inline bool operator==(const Vector2<T> &aVector2) const
	{
		if(x != aVector2.x)
		{
			return false;
		}
		if(y != aVector2.y)
		{
			return false;
		}

		return true;
	}

	bool operator!=(const Vector2<T> &aVector2) const
	{
		if((*this==aVector2) == true)
		{
			return false;
		}
		return true;
	}

	void Set( const T aValueForX, const T aValueForY)
	{
		x = aValueForX;
		y = aValueForY;
	}

	inline T Dot(const Vector2<T> &aVector2) const
	{
		return x*aVector2.x + y*aVector2.y;
	}
	
	inline float Length() const
	{
		return static_cast<float>(sqrt((x * x) + (y * y)));
	}

	inline Vector2<T> Normalize()
	{
		const float length = Length();
		return Vector2<T>(x/length, y/length);
	}

	static inline Vector2<T> Zero()
	{
		return Vector2<T>(static_cast<T>(0), static_cast<T>(0));
	}

	~Vector2(void)
	{

	}
};


#endif // _VECTOR_2_H_