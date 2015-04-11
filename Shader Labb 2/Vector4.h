#ifndef _VECTOR4_H_
#define _VECTOR4_H_

#include "Math.h"

template<typename Type>
class Matrix44;

template<typename Type>
class Vector4
{
	
public:

	Vector4<Type> ()
	{
		myX = 0;
		myY = 0;
		myZ = 0;
		myW = 0;
	}
	Vector4<Type> (Type anX, Type aY, Type aZ, Type aW)
	{
		myX = anX;
		myY = aY;
		myZ = aZ;
		myW = aW;
	}

	union
	{
		Type myX;
		Type x;
		Type r;
		Type myR;
	};
	union
	{
		Type myY;
		Type y;
		Type g;
		Type myG;
	};
	union
	{
		Type myZ;
		Type z;
		Type b;
		Type myB;
	};
	union
	{
		Type myW;
		Type w;
		Type a;
		Type myA;
	};
	
	const Vector4<Type> operator+(const Vector4<Type>& aVector4) const
	{
		Vector4<Type> tempVector;
		tempVector.myX = myX + aVector4.myX;
		tempVector.myY = myY + aVector4.myY;
		tempVector.myZ = myZ + aVector4.myZ;
		tempVector.myW = myW + aVector4.myW;
		return tempVector;
	}

	Vector4<Type> operator*(const Matrix44<Type> aMatrix44) const
	{
		Vector4<Type> tempVector;
		tempVector.myX = (myX * aMatrix44.myData[0] + myY * aMatrix44.myData[4] + myZ * aMatrix44.myData[8] + myW * aMatrix44.myData[12]);
		tempVector.myY = (myX * aMatrix44.myData[1] + myY * aMatrix44.myData[5] + myZ * aMatrix44.myData[9] + myW * aMatrix44.myData[13]);
		tempVector.myZ = (myX * aMatrix44.myData[2] + myY * aMatrix44.myData[6] + myZ * aMatrix44.myData[10] + myW * aMatrix44.myData[14]);
		tempVector.myW = (myX * aMatrix44.myData[3] + myY * aMatrix44.myData[7] + myZ * aMatrix44.myData[11] + myW * aMatrix44.myData[15]);
		return tempVector;
	}	

	const Vector4<Type> operator-(const Vector4<Type>& aVector4) const
	{
		Vector4<Type> tempVector;
		tempVector.myX = myX - aVector4.myX;
		tempVector.myY = myY - aVector4.myY;
		tempVector.myZ = myZ - aVector4.myZ;
		tempVector.myW = myW - aVector4.myW;
		return tempVector;
	}

	Vector4<Type> operator*(const Type aScalar) const
	{
		Vector4<Type> tempVector;
		tempVector.myX = myX * aScalar;
		tempVector.myY = myY * aScalar;
		tempVector.myZ = myZ * aScalar;
		tempVector.myW = myW * aScalar;
		return tempVector;
	}

	const Vector4<Type> operator/(const Type aScalar) const
	{
		Vector4<Type> tempVector;
		tempVector.myX = myX / aScalar;
		tempVector.myY = myY / aScalar;
		tempVector.myZ = myZ / aScalar;
		tempVector.myW = myW / aScalar;
		return tempVector;
	}
	const Vector4<Type> operator/(const Vector4<Type>& aVector4) const
	{
		Vector4<Type> tempVector;
		tempVector.myX = myX / aVector4.myX;
		tempVector.myY = myY / aVector4.myY;
		tempVector.myZ = myZ / aVector4.myZ;
		tempVector.myW = myW / aVector4.myW;
		return tempVector;
	}

	const Vector4<Type> operator*(const Vector4<Type>& aVector4) const
	{
		Vector4<Type> tempVector;
		tempVector.myX = myX * aVector4.myX;
		tempVector.myY = myY * aVector4.myY;
		tempVector.myZ = myZ * aVector4.myZ;
		tempVector.myW = myW * aVector4.myW;
		return tempVector;
	}

	Vector4<Type> operator +=(const Vector4<Type>& aVector4)
	{		
		myX += aVector4.myX;
		myY += aVector4.myY;
		myZ += aVector4.myZ;
		myW += aVector4.myW;
		return *this;
	}
	Vector4<Type> operator -=(const Vector4<Type>& aVector4)
	{		
		myX -= aVector4.myX;
		myY -= aVector4.myY;
		myZ -= aVector4.myZ;
		myW -= aVector4.myW;
		return *this;
	}
	Vector4<Type> operator *=(const Vector4<Type>& aVector4)
	{		
		myX *= aVector4.myX;
		myY *= aVector4.myY;
		myZ *= aVector4.myZ;
		myW *= aVector4.myW;
		return *this;
	}
	const Type operator *=(const Type& aScalar)
	{	
		Type tempScalar; 
		tempScalar += myX * aScalar;
		tempScalar += myY * aScalar;
		tempScalar += myZ * aScalar;
		tempScalar += myW * aScalar;
		return tempScalar;
	}
	Vector4<Type> operator /=(const Type& aScalar)
	{		
		myX /= aScalar;
		myY /= aScalar;
		myZ /= aScalar;
		myW /= aScalar;
		return *this;
	}
	Vector4<Type> operator /=(const Vector4<Type>& aVector4)
	{		
		myX /= aVector4.myX;
		myY /= aVector4.myY;
		myZ /= aVector4.myZ;
		myW /= aVector4.myW;
		return *this;
	}	

	float Length()
	{
		return sqrt(static_cast<float>(x*x+y*y+z*z+w*w));
	}

	Vector4<Type> Normalize()
	{
		float length = Length();
		return (Vector4<Type>(x/length, y/length, z/length, w/length));
	}

	const Type Dot(Vector4<Type>& aVector4) const
	{
		return (myX * aVector4.myX + myY * aVector4.myY + myZ * aVector4.myZ + myW * aVector4.myW);
	}	
};

#endif