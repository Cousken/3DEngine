#ifndef COMMONUTILITIES_VECTORMATH_HEADER
#define COMMONUTILITIES_VECTORMATH_HEADER

#include "CU_vector.h"
#include "CU_Matrix.h"

namespace CommonUtilities
{
	template <class TYPE>
	Vector3<TYPE> operator*(const Vector3<TYPE>& aVector,const Matrix33<TYPE>& aMatrix)
	{
		Vector3<TYPE> returnValue;

		returnValue.x=aVector.x*aMatrix.myMatrix[0][0]+aVector.y*aMatrix.myMatrix[1][0]+aVector.z*aMatrix.myMatrix[2][0];
		returnValue.y=aVector.x*aMatrix.myMatrix[0][1]+aVector.y*aMatrix.myMatrix[1][1]+aVector.z*aMatrix.myMatrix[2][1];
		returnValue.z=aVector.x*aMatrix.myMatrix[0][2]+aVector.y*aMatrix.myMatrix[1][2]+aVector.z*aMatrix.myMatrix[2][2];

		return(returnValue);
	}

	template <class TYPE>
	Vector3<TYPE> operator*(const Vector3<TYPE>& aVector,const Matrix44<TYPE>& aMatrix)
	{
		Vector3<TYPE> returnValue;

		returnValue.x=aVector.x*aMatrix.myMatrix[0][0]+aVector.y*aMatrix.myMatrix[1][0]+aVector.z*aMatrix.myMatrix[2][0];
		returnValue.y=aVector.x*aMatrix.myMatrix[0][1]+aVector.y*aMatrix.myMatrix[1][1]+aVector.z*aMatrix.myMatrix[2][1];
		returnValue.z=aVector.x*aMatrix.myMatrix[0][2]+aVector.y*aMatrix.myMatrix[1][2]+aVector.z*aMatrix.myMatrix[2][2];

		return(returnValue);
	}

}

#endif