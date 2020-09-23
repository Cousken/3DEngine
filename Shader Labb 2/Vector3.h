#ifndef _VECTOR_3_H_
#define _VECTOR_3_H_


#include <cmath>
template<typename T>
class Matrix33;
#include <cassert>
#include "debugHeader.h"

template <typename T>
class Vector3
{
public:
	union
	{
		T x, r, myX;
	};

	union
	{
		T y, g, myY;
	};

	union
	{
		T z, b, myZ;
	};	
		

	Vector3()
		:x(0)
		,y(0)
		,z(0)
	{

	}

	Vector3(const T &anX, const T &anY, const T &aZ)
		: x(anX)
		, y(anY)
		, z(aZ)
	{

	}

	Vector3(const T &aValue)
		: x(aValue)
		, y(aValue)
		, z(aValue)
	{

	}

	Vector3<T> &operator+=(const Vector3<T> &aVector3)
	{
		x += aVector3.x;
		y += aVector3.y;
		z += aVector3.z;

		return *this;
	}

	Vector3<T> &operator-=(const Vector3<T> &aVector3)
	{
		x -= aVector3.x;
		y -= aVector3.y;
		z -= aVector3.z;

		return *this;
	}

	Vector3<T> &operator*=(const T &aValue)
	{
		x *= aValue;
		y *= aValue;
		z *= aValue;

		return *this;
	}

	Vector3<T> &operator*=(const Vector3<T> &aVector3)
	{
		x *= aVector3.x;
		y *= aVector3.y;
		z *= aVector3.z;

		return *this;
	}

	Vector3<T> &operator/=(const T &aValue)
	{
		x /= aValue;
		y /= aValue;
		z /= aValue;

		return *this;
	}

	inline Vector3<T> operator+(const Vector3<T> &aVector3) const
	{
		return Vector3<T>(x + aVector3.x, y + aVector3.y, z + aVector3.z);
	}

	inline Vector3<T> operator-(const Vector3<T> &aVector3) const
	{
		return Vector3<T>(x - aVector3.x, y - aVector3.y, z - aVector3.z);
	}

	inline Vector3<T> operator*(const T &aValue) const
	{
		return Vector3<T>(x * aValue, y * aValue, z * aValue);
	}

	inline Vector3<T> operator*(const Vector3<T> &aVector3) const
	{
		return Vector3<T>(x * aVector3.x, y * aVector3.y, z * aVector3.z);
	}

	Vector3<T> operator/(const T &aValue) const
	{
		return Vector3<T>(x / aValue, y / aValue, z / aValue);
	}

	inline bool operator==(const Vector3<T> &aVector3) const
	{
		return (x == aVector3.x && y == aVector3.y && z == aVector3.z);
	}

	bool operator!=(const Vector3<T> &aVector3) const
	{
		if((*this==aVector3) == true)
		{
			return false;
		}
		return true;
	}

	Vector3<T> Cross(const Vector3<T> &aVector3) const
	{
		return Vector3<T>(y*aVector3.z - z*aVector3.y, 
			z*aVector3.x - x*aVector3.z, 
			x*aVector3.y - y*aVector3.x);
	}

	const Vector3<T> operator*(const Matrix33<T> aMatrix33) const
	{
		Vector3<T> tempVector;
		tempVector.myX = (myX * aMatrix33.myMatrix[0] + myY * aMatrix33.myMatrix[3] + myZ * aMatrix33.myMatrix[6]);
		tempVector.myY = (myX * aMatrix33.myMatrix[1] + myY * aMatrix33.myMatrix[4] + myZ * aMatrix33.myMatrix[7]);
		tempVector.myZ = (myX * aMatrix33.myMatrix[2] + myY * aMatrix33.myMatrix[5] + myZ * aMatrix33.myMatrix[8]);
		return tempVector;
	}

	inline float Length() const
	{
		return sqrt(static_cast<float>((x * x) + (y * y) + (z * z)));	
	}

	inline float Length2() const
	{
		const float length = this->Length();
		return pow(Length(), 2);
	}

	inline T Dot(const Vector3<T> &aVector3) const
	{
		return x*aVector3.x + y*aVector3.y + z*aVector3.z;
	}

	void Set( const T aValueForX, const T aValueForY, const T aValueForZ)
	{
		x = aValueForX;
		y = aValueForY;
		z = aValueForZ;
	}

	Vector3<T> Normalize()
	{
		float length = Length();
		return (Vector3<T>(x/length, y/length, z/length));
	}

	~Vector3(void)
	{

	}
};


#endif // _VECTOR_3_H_