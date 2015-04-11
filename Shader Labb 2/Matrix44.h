#ifndef MATRIX44_H_
#define MATRIX44_H_

#include "Matrix33.h"

#include "Vector3.h"

template<typename Type>
class Matrix44
{
public:
	Matrix44<Type>()
	{
		myData[0] = 1;
		myData[1] = 0;
		myData[2] = 0;
		myData[3] = 0;

		myData[4] = 0;
		myData[5] = 1;
		myData[6] = 0;
		myData[7] = 0;

		myData[8] = 0;
		myData[9] = 0;
		myData[10] = 1;
		myData[11] = 0;

		myData[12] = 0;
		myData[13] = 0;
		myData[14] = 0;
		myData[15] = 1;		
	}

	const Matrix44<Type> operator+(const Matrix44<Type>& aMatrix44) const
	{
		Matrix44<Type> tempMatrix;
		tempMatrix.myData[0] = myData[0] + aMatrix44.myData[0];
		tempMatrix.myData[1] = myData[1] + aMatrix44.myData[1];
		tempMatrix.myData[2] = myData[2] + aMatrix44.myData[2];
		tempMatrix.myData[3] = myData[3] + aMatrix44.myData[3];
		tempMatrix.myData[4] = myData[4] + aMatrix44.myData[4];
		tempMatrix.myData[5] = myData[5] + aMatrix44.myData[5];
		tempMatrix.myData[6] = myData[6] + aMatrix44.myData[6];
		tempMatrix.myData[7] = myData[7] + aMatrix44.myData[7];
		tempMatrix.myData[8] = myData[8] + aMatrix44.myData[8];
		tempMatrix.myData[9] = myData[9] + aMatrix44.myData[9];
		tempMatrix.myData[10] = myData[10] + aMatrix44.myData[10];
		tempMatrix.myData[11] = myData[11] + aMatrix44.myData[11];
		tempMatrix.myData[12] = myData[12] + aMatrix44.myData[12];
		tempMatrix.myData[13] = myData[13] + aMatrix44.myData[13];
		tempMatrix.myData[14] = myData[14] + aMatrix44.myData[14];
		tempMatrix.myData[15] = myData[15] + aMatrix44.myData[15];
		return tempMatrix;
	}

	const Matrix44<Type> operator+=(const Vector3<Type>& aVector3)
	{		
		myData[12] = myData[12] + aVector3.myX;
		myData[13] = myData[13] + aVector3.myY;
		myData[14] = myData[14] + aVector3.myZ;		
		return (*this);
	}

	const Matrix44<Type> operator-=(const Vector3<Type>& aVector3)
	{		
		myData[12] = myData[12] - aVector3.myX;
		myData[13] = myData[13] - aVector3.myY;
		myData[14] = myData[14] - aVector3.myZ;		
		return (*this);
	}


	const Matrix44<Type> operator-(const Matrix44<Type>& aMatrix44) const
	{
		Matrix44<Type> tempMatrix;
		tempMatrix.myData[0] = myData[0] - aMatrix44.myData[0];
		tempMatrix.myData[1] = myData[1] - aMatrix44.myData[1];
		tempMatrix.myData[2] = myData[2] - aMatrix44.myData[2];
		tempMatrix.myData[3] = myData[3] - aMatrix44.myData[3];
		tempMatrix.myData[4] = myData[4] - aMatrix44.myData[4];
		tempMatrix.myData[5] = myData[5] - aMatrix44.myData[5];
		tempMatrix.myData[6] = myData[6] - aMatrix44.myData[6];
		tempMatrix.myData[7] = myData[7] - aMatrix44.myData[7];
		tempMatrix.myData[8] = myData[8] - aMatrix44.myData[8];
		tempMatrix.myData[9] = myData[9] - aMatrix44.myData[9];
		tempMatrix.myData[10] = myData[10] - aMatrix44.myData[10];
		tempMatrix.myData[11] = myData[11] - aMatrix44.myData[11];
		tempMatrix.myData[12] = myData[12] - aMatrix44.myData[12];
		tempMatrix.myData[13] = myData[13] - aMatrix44.myData[13];
		tempMatrix.myData[14] = myData[14] - aMatrix44.myData[14];
		tempMatrix.myData[15] = myData[15] - aMatrix44.myData[15];
		return tempMatrix;
	}

	const Matrix44<Type> operator=(const Matrix33<Type>& aMatrix33)
	{		
		myData[0] = aMatrix33.myMatrix[0];
		myData[1] = aMatrix33.myMatrix[1];
		myData[2] = aMatrix33.myMatrix[2];
		myData[4] = aMatrix33.myMatrix[3];
		myData[5] = aMatrix33.myMatrix[4];
		myData[6] = aMatrix33.myMatrix[5];
		myData[8] = aMatrix33.myMatrix[6];
		myData[9] = aMatrix33.myMatrix[7];
		myData[10] = aMatrix33.myMatrix[8];		
		return (*this);
	}

	const Matrix44<Type> operator=(const Matrix44<Type>& aMatrix44)
	{		
		myData[0] = aMatrix44.myData[0];
		myData[1] = aMatrix44.myData[1];
		myData[2] = aMatrix44.myData[2];
		myData[3] = aMatrix44.myData[3];
		myData[4] = aMatrix44.myData[4];
		myData[5] = aMatrix44.myData[5];
		myData[6] = aMatrix44.myData[6];
		myData[7] = aMatrix44.myData[7];
		myData[8] = aMatrix44.myData[8];
		myData[9] = aMatrix44.myData[9];
		myData[10] = aMatrix44.myData[10];
		myData[11] = aMatrix44.myData[11];
		myData[12] = aMatrix44.myData[12];
		myData[13] = aMatrix44.myData[13];
		myData[14] = aMatrix44.myData[14];
		myData[15] = aMatrix44.myData[15];
		return (*this);
	}

	Matrix44<Type> operator*(const Matrix44<Type>& aMatrix44) const
	{
		Matrix44<Type> tempMatrix;
		tempMatrix.myData[0] = myData[0] * aMatrix44.myData[0] + myData[1] * aMatrix44.myData[4] + myData[2] * aMatrix44.myData[8] + myData[3] * aMatrix44.myData[12];
		tempMatrix.myData[1] = myData[0] * aMatrix44.myData[1] + myData[1] * aMatrix44.myData[5] + myData[2] * aMatrix44.myData[9] + myData[3] * aMatrix44.myData[13];
		tempMatrix.myData[2] = myData[0] * aMatrix44.myData[2] + myData[1] * aMatrix44.myData[6] + myData[2] * aMatrix44.myData[10] + myData[3] * aMatrix44.myData[14];
		tempMatrix.myData[3] = myData[0] * aMatrix44.myData[3] + myData[1] * aMatrix44.myData[7] + myData[2] * aMatrix44.myData[11] + myData[3] * aMatrix44.myData[15];
		tempMatrix.myData[4] = myData[4] * aMatrix44.myData[0] + myData[5] * aMatrix44.myData[4] + myData[6] * aMatrix44.myData[8] + myData[7] * aMatrix44.myData[12];
		tempMatrix.myData[5] = myData[4] * aMatrix44.myData[1] + myData[5] * aMatrix44.myData[5] + myData[6] * aMatrix44.myData[9] + myData[7] * aMatrix44.myData[13];
		tempMatrix.myData[6] = myData[4] * aMatrix44.myData[2] + myData[5] * aMatrix44.myData[6] + myData[6] * aMatrix44.myData[10] + myData[7] * aMatrix44.myData[14];
		tempMatrix.myData[7] = myData[4] * aMatrix44.myData[3] + myData[5] * aMatrix44.myData[7] + myData[6] * aMatrix44.myData[11] + myData[7] * aMatrix44.myData[15];
		tempMatrix.myData[8] = myData[8] * aMatrix44.myData[0] + myData[9] * aMatrix44.myData[4] + myData[10] * aMatrix44.myData[8] + myData[11] * aMatrix44.myData[12];
		tempMatrix.myData[9] = myData[8] * aMatrix44.myData[1] + myData[9] * aMatrix44.myData[5] + myData[10] * aMatrix44.myData[9] + myData[11] * aMatrix44.myData[13];
		tempMatrix.myData[10] = myData[8] * aMatrix44.myData[2] + myData[9] * aMatrix44.myData[6] + myData[10] * aMatrix44.myData[10] + myData[11] * aMatrix44.myData[14];
		tempMatrix.myData[11] = myData[8] * aMatrix44.myData[3] + myData[9] * aMatrix44.myData[7] + myData[10] * aMatrix44.myData[11] + myData[11] * aMatrix44.myData[15];
		tempMatrix.myData[12] = myData[12] * aMatrix44.myData[0] + myData[13] * aMatrix44.myData[4] + myData[14] * aMatrix44.myData[8] + myData[15] * aMatrix44.myData[12];
		tempMatrix.myData[13] = myData[12] * aMatrix44.myData[1] + myData[13] * aMatrix44.myData[5] + myData[14] * aMatrix44.myData[9] + myData[15] * aMatrix44.myData[13];
		tempMatrix.myData[14] = myData[12] * aMatrix44.myData[2] + myData[13] * aMatrix44.myData[6] + myData[14] * aMatrix44.myData[10] + myData[15] * aMatrix44.myData[14];
		tempMatrix.myData[15] = myData[12] * aMatrix44.myData[3] + myData[13] * aMatrix44.myData[7] + myData[14] * aMatrix44.myData[11] + myData[15] * aMatrix44.myData[15];
		return (tempMatrix);
	}

	Matrix44<Type> operator*(const Matrix33<Type> &aMatrix33) const
	{	
		Matrix44<float> temp44Matrix;
		/*tempMatrix.myMatrix[0] = myMatrix[0] * aMatrix33.myMatrix[0] + myMatrix[4] * aMatrix33.myMatrix[1] + myMatrix[8] * aMatrix33.myMatrix[2];
		tempMatrix.myMatrix[1] = myMatrix[1] * aMatrix33.myMatrix[0] + myMatrix[5] * aMatrix33.myMatrix[1] + myMatrix[9] * aMatrix33.myMatrix[2];
		tempMatrix.myMatrix[2] = myMatrix[2] * aMatrix33.myMatrix[0] + myMatrix[6] * aMatrix33.myMatrix[1] + myMatrix[10] * aMatrix33.myMatrix[2];
		tempMatrix.myMatrix[3] = myMatrix[3];
		tempMatrix.myMatrix[4] = myMatrix[0] * aMatrix33.myMatrix[3] + myMatrix[4] * aMatrix33.myMatrix[4] + myMatrix[8] * aMatrix33.myMatrix[5];
		tempMatrix.myMatrix[5] = myMatrix[1] * aMatrix33.myMatrix[3] + myMatrix[5] * aMatrix33.myMatrix[4] + myMatrix[9] * aMatrix33.myMatrix[5];
		tempMatrix.myMatrix[6] = myMatrix[2] * aMatrix33.myMatrix[3] + myMatrix[6] * aMatrix33.myMatrix[4] + myMatrix[10] * aMatrix33.myMatrix[5];
		tempMatrix.myMatrix[7] = myMatrix[7];
		tempMatrix.myMatrix[8] = myMatrix[0] * aMatrix33.myMatrix[6] + myMatrix[4] * aMatrix33.myMatrix[7] + myMatrix[8] * aMatrix33.myMatrix[8];
		tempMatrix.myMatrix[9] = myMatrix[1] * aMatrix33.myMatrix[6] + myMatrix[5] * aMatrix33.myMatrix[7] + myMatrix[9] * aMatrix33.myMatrix[8];
		tempMatrix.myMatrix[10] = myMatrix[2] * aMatrix33.myMatrix[6] + myMatrix[6] * aMatrix33.myMatrix[7] + myMatrix[10] * aMatrix33.myMatrix[8];
		tempMatrix.myMatrix[11] = myMatrix[11];
		tempMatrix.myMatrix[12] = myMatrix[12];
		tempMatrix.myMatrix[13] = myMatrix[13];
		tempMatrix.myMatrix[14] = myMatrix[14];
		tempMatrix.myMatrix[15] = myMatrix[15];*/

		Matrix33<Type> tempMatrix = aMatrix33 * Get33();
		temp44Matrix.myData[0] = tempMatrix.myMatrix[0];
		temp44Matrix.myData[1] = tempMatrix.myMatrix[1];
		temp44Matrix.myData[2] = tempMatrix.myMatrix[2];
		temp44Matrix.myData[3] = myData[3];
		temp44Matrix.myData[4] = tempMatrix.myMatrix[3];
		temp44Matrix.myData[5] = tempMatrix.myMatrix[4];
		temp44Matrix.myData[6] = tempMatrix.myMatrix[5];
		temp44Matrix.myData[7] = myData[7];
		temp44Matrix.myData[8] = tempMatrix.myMatrix[6];
		temp44Matrix.myData[9] = tempMatrix.myMatrix[7];
		temp44Matrix.myData[10] = tempMatrix.myMatrix[8];
		temp44Matrix.myData[11] = myData[11];
		temp44Matrix.myData[12] = myData[12];
		temp44Matrix.myData[13] = myData[13];
		temp44Matrix.myData[14] = myData[14];
		temp44Matrix.myData[15] = myData[15];


		/*tempMatrix.myMatrix[0] = myMatrix[0] * aMatrix33.myMatrix[0] + myMatrix[1] * aMatrix33.myMatrix[3] + myMatrix[2] * aMatrix33.myMatrix[6];
		tempMatrix.myMatrix[1] = myMatrix[0] * aMatrix33.myMatrix[1] + myMatrix[1] * aMatrix33.myMatrix[4] + myMatrix[2] * aMatrix33.myMatrix[7];
		tempMatrix.myMatrix[2] = myMatrix[0] * aMatrix33.myMatrix[2] + myMatrix[1] * aMatrix33.myMatrix[5] + myMatrix[2] * aMatrix33.myMatrix[8];
		tempMatrix.myMatrix[3] = myMatrix[3];
		tempMatrix.myMatrix[4] = myMatrix[4] * aMatrix33.myMatrix[0] + myMatrix[5] * aMatrix33.myMatrix[3] + myMatrix[6] * aMatrix33.myMatrix[6];
		tempMatrix.myMatrix[5] = myMatrix[4] * aMatrix33.myMatrix[1] + myMatrix[5] * aMatrix33.myMatrix[4] + myMatrix[6] * aMatrix33.myMatrix[7];
		tempMatrix.myMatrix[6] = myMatrix[4] * aMatrix33.myMatrix[2] + myMatrix[5] * aMatrix33.myMatrix[5] + myMatrix[6] * aMatrix33.myMatrix[8];
		tempMatrix.myMatrix[7] = myMatrix[7];
		tempMatrix.myMatrix[8] = myMatrix[8] * aMatrix33.myMatrix[0] + myMatrix[9] * aMatrix33.myMatrix[3] + myMatrix[10] * aMatrix33.myMatrix[6];
		tempMatrix.myMatrix[9] = myMatrix[8] * aMatrix33.myMatrix[1] + myMatrix[9] * aMatrix33.myMatrix[4] + myMatrix[10] * aMatrix33.myMatrix[7];
		tempMatrix.myMatrix[10] = myMatrix[8] * aMatrix33.myMatrix[2] + myMatrix[9] * aMatrix33.myMatrix[5] + myMatrix[10] * aMatrix33.myMatrix[8];
		tempMatrix.myMatrix[11] = myMatrix[11];
		tempMatrix.myMatrix[12] = myMatrix[12];
		tempMatrix.myMatrix[13] = myMatrix[13];
		tempMatrix.myMatrix[14] = myMatrix[14];
		tempMatrix.myMatrix[15] = myMatrix[15];*/
		return (temp44Matrix);
	}

	const Matrix33<Type> Matrix44<Type>::Get33() const
	{
		Matrix33<Type> tempMatrix;
		tempMatrix.myMatrix[0] = myData[0];
		tempMatrix.myMatrix[1] = myData[1];
		tempMatrix.myMatrix[2] = myData[2];
		tempMatrix.myMatrix[3] = myData[4];
		tempMatrix.myMatrix[4] = myData[5];
		tempMatrix.myMatrix[5] = myData[6];
		tempMatrix.myMatrix[6] = myData[8];
		tempMatrix.myMatrix[7] = myData[9];
		tempMatrix.myMatrix[8] = myData[10];
		return (tempMatrix);
	}	

	Matrix44<Type> operator*=(const Matrix44<Type>& aMatrix44)
	{	
		Matrix44<Type> tempMatrix;
		tempMatrix.myData[0] = myData[0] * aMatrix44.myData[0] + myData[1] * aMatrix44.myData[4] + myData[2] * aMatrix44.myData[8] + myData[3] * aMatrix44.myData[12];
		tempMatrix.myData[1] = myData[0] * aMatrix44.myData[1] + myData[1] * aMatrix44.myData[5] + myData[2] * aMatrix44.myData[9] + myData[3] * aMatrix44.myData[13];
		tempMatrix.myData[2] = myData[0] * aMatrix44.myData[2] + myData[1] * aMatrix44.myData[6] + myData[2] * aMatrix44.myData[10] + myData[3] * aMatrix44.myData[14];
		tempMatrix.myData[3] = myData[0] * aMatrix44.myData[3] + myData[1] * aMatrix44.myData[7] + myData[2] * aMatrix44.myData[11] + myData[3] * aMatrix44.myData[15];
		tempMatrix.myData[4] = myData[4] * aMatrix44.myData[0] + myData[5] * aMatrix44.myData[4] + myData[6] * aMatrix44.myData[8] + myData[7] * aMatrix44.myData[12];
		tempMatrix.myData[5] = myData[4] * aMatrix44.myData[1] + myData[5] * aMatrix44.myData[5] + myData[6] * aMatrix44.myData[9] + myData[7] * aMatrix44.myData[13];
		tempMatrix.myData[6] = myData[4] * aMatrix44.myData[2] + myData[5] * aMatrix44.myData[6] + myData[6] * aMatrix44.myData[10] + myData[7] * aMatrix44.myData[14];
		tempMatrix.myData[7] = myData[4] * aMatrix44.myData[3] + myData[5] * aMatrix44.myData[7] + myData[6] * aMatrix44.myData[11] + myData[7] * aMatrix44.myData[15];
		tempMatrix.myData[8] = myData[8] * aMatrix44.myData[0] + myData[9] * aMatrix44.myData[4] + myData[10] * aMatrix44.myData[8] + myData[11] * aMatrix44.myData[12];
		tempMatrix.myData[9] = myData[8] * aMatrix44.myData[1] + myData[9] * aMatrix44.myData[5] + myData[10] * aMatrix44.myData[9] + myData[11] * aMatrix44.myData[13];
		tempMatrix.myData[10] = myData[8] * aMatrix44.myData[2] + myData[9] * aMatrix44.myData[6] + myData[10] * aMatrix44.myData[10] + myData[11] * aMatrix44.myData[14];
		tempMatrix.myData[11] = myData[8] * aMatrix44.myData[3] + myData[9] * aMatrix44.myData[7] + myData[10] * aMatrix44.myData[11] + myData[11] * aMatrix44.myData[15];
		tempMatrix.myData[12] = myData[12] * aMatrix44.myData[0] + myData[13] * aMatrix44.myData[4] + myData[14] * aMatrix44.myData[8] + myData[15] * aMatrix44.myData[12];
		tempMatrix.myData[13] = myData[12] * aMatrix44.myData[1] + myData[13] * aMatrix44.myData[5] + myData[14] * aMatrix44.myData[9] + myData[15] * aMatrix44.myData[13];
		tempMatrix.myData[14] = myData[12] * aMatrix44.myData[2] + myData[13] * aMatrix44.myData[6] + myData[14] * aMatrix44.myData[10] + myData[15] * aMatrix44.myData[14];
		tempMatrix.myData[15] = myData[12] * aMatrix44.myData[3] + myData[13] * aMatrix44.myData[7] + myData[14] * aMatrix44.myData[11] + myData[15] * aMatrix44.myData[15];
			
		for (int index = 0; index < 16; index++)
		{
			myData[index] = tempMatrix.myData[index];
		}

		return (*this);
	}

	const bool operator==(const Matrix44<Type>& aMatrix44) const
	{
		for (int index = 0; index < 16; index++)
		{
			if (myData[index] != aMatrix44.myData[index])
			{
				return (false);
			}
		}
		return (true);		
	}

	const Matrix44<Type> operator+(const Matrix44<Type>& aMatrix44)
	{		
		myData[0] = aMatrix44.myData[0];
		myData[1] = aMatrix44.myData[1];
		myData[2] = aMatrix44.myData[2];
		myData[3] = aMatrix44.myData[3];
		myData[4] = aMatrix44.myData[4];
		myData[5] = aMatrix44.myData[5];
		myData[6] = aMatrix44.myData[6];
		myData[7] = aMatrix44.myData[7];
		myData[8] = aMatrix44.myData[8];
		myData[9] = aMatrix44.myData[9];
		myData[10] = aMatrix44.myData[10];
		myData[11] = aMatrix44.myData[11];
		myData[12] = aMatrix44.myData[12];
		myData[13] = aMatrix44.myData[13];
		myData[14] = aMatrix44.myData[14];
		myData[15] = aMatrix44.myData[15];
		return (*this);
	}

	const Matrix44<Type> operator*(const Vector3<Type>& aVector3) const
	{		
		Matrix44<Type> tempMatrix;
		tempMatrix.myData[0] = myData[0];
		tempMatrix.myData[1] = myData[1];
		tempMatrix.myData[2] = myData[2];
		tempMatrix.myData[3] = myData[3];
		tempMatrix.myData[4] = myData[4];
		tempMatrix.myData[5] = myData[5];
		tempMatrix.myData[6] = myData[6];
		tempMatrix.myData[7] = myData[7];
		tempMatrix.myData[8] = myData[8];
		tempMatrix.myData[9] = myData[9];
		tempMatrix.myData[10] = myData[10];
		tempMatrix.myData[11] = myData[11];
		tempMatrix.myData[12] = aVector3.myX * myData[12];
		tempMatrix.myData[13] = aVector3.myY * myData[13];
		tempMatrix.myData[14] = aVector3.myZ * myData[14];
		tempMatrix.myData[15] = myData[15];
		return (tempMatrix);
	}

	void SetPosition(Vector3<Type> aVector3)  
	{
		myData[12] = aVector3.myX;
		myData[13] = aVector3.myY;
		myData[14] = aVector3.myZ;
	}	

	Vector3<Type> GetPosition()
	{
		Vector3<Type> tempVector;
		tempVector.myX = myData[12];
		tempVector.myY = myData[13];
		tempVector.myZ = myData[14];
		return (tempVector);
	}

	void Init(Type* anArray)
	{
		myData[0] = anArray[0];
		myData[1] = anArray[1];
		myData[2] = anArray[2];
		myData[3] = anArray[3];
		myData[4] = anArray[4];
		myData[5] = anArray[5];
		myData[6] = anArray[6];
		myData[7] = anArray[7];
		myData[8] = anArray[8];
		myData[9] = anArray[9];
		myData[10] = anArray[10];
		myData[11] = anArray[11];
		myData[12] = anArray[12];
		myData[13] = anArray[13];
		myData[14] = anArray[14];
		myData[15] = anArray[15];
	}

	static Matrix44<Type> CreateRotateAroundX(Type aAngleInRadians)
	{
		Matrix44<Type> tempMatrix;		
		tempMatrix.myData[5] = static_cast<Type>(cos(aAngleInRadians));
		tempMatrix.myData[6] = static_cast<Type>((-sin(aAngleInRadians)));		
		tempMatrix.myData[9] = static_cast<Type>(sin(aAngleInRadians));
		tempMatrix.myData[10] = static_cast<Type>(cos(aAngleInRadians));		

		return (tempMatrix);
	}

	static Matrix44<Type> CreateRotateAroundY(Type aAngleInRadians)
	{
		Matrix44<Type> tempMatrix;
		tempMatrix.myData[0] = static_cast<Type>(cos(aAngleInRadians));		
		tempMatrix.myData[2] = static_cast<Type>(sin(aAngleInRadians));		
		tempMatrix.myData[8] = static_cast<Type>((-sin(aAngleInRadians)));		
		tempMatrix.myData[10] = static_cast<Type>(cos(aAngleInRadians));	

		return (tempMatrix);
	}

	static Matrix44<Type> CreateRotateAroundZ(Type aAngleInRadians)
	{
		Matrix44<Type> tempMatrix;
		tempMatrix.myData[0] = static_cast<Type>(cos(aAngleInRadians));
		tempMatrix.myData[1] = static_cast<Type>((-sin(aAngleInRadians)));		
		tempMatrix.myData[4] = static_cast<Type>(sin(aAngleInRadians));
		tempMatrix.myData[5] = static_cast<Type>(cos(aAngleInRadians));		

		return (tempMatrix);
	}

	static Matrix44<Type> CreateTranslation(Vector3<float> aVector3)
	{
		Matrix44<Type> tempMatrix;		
		tempMatrix.myData[12] = aVector3.x;
		tempMatrix.myData[13] = aVector3.y;
		tempMatrix.myData[14] = aVector3.z;		

		return (tempMatrix);
	}

	static Matrix44<Type> Identity()
	{
		Matrix44<Type> tempMatrix;		
		return (tempMatrix);
	}

	Matrix44<Type> Inverse()
	{
		Matrix33<Type> invOrientation = Get33().Inverse();
		Vector3<Type> invPosition = (GetPosition() * -1) * invOrientation;	

		Matrix44<Type> matrix;

		matrix.myData[0] = invOrientation.myMatrix[0];
		matrix.myData[1] = invOrientation.myMatrix[1];
		matrix.myData[2] = invOrientation.myMatrix[2];
		matrix.myData[3] = 0;

		matrix.myData[4] = invOrientation.myMatrix[3];
		matrix.myData[5] = invOrientation.myMatrix[4];
		matrix.myData[6] = invOrientation.myMatrix[5];
		matrix.myData[7] = 0;

		matrix.myData[8] = invOrientation.myMatrix[6];
		matrix.myData[9] = invOrientation.myMatrix[7];
		matrix.myData[10] = invOrientation.myMatrix[8];
		matrix.myData[11] = 0;

		matrix.SetPosition(invPosition);
		matrix.myData[15] = 1;

		return matrix;
	}

	void ConvertD3D( float* aMatrix )
	{
		for (int i = 0; i < 16; i++ )
		{
			myData[i] = aMatrix[i];
		}
	}

	Type myData [16];
};

#endif