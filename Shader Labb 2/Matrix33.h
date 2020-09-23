#ifndef MATRIX33_H_
#define MATRIX33_H_

#include "Math.h"
#include "debugHeader.h"

template<typename Type>
class Matrix44;

template<typename Type>
class Matrix33
{	
public:
	Matrix33<Type>()
	{
		myMatrix[0] = 1;
		myMatrix[1] = 0;
		myMatrix[2] = 0;
		myMatrix[3] = 0;
		myMatrix[4] = 1;
		myMatrix[5] = 0;
		myMatrix[6] = 0;
		myMatrix[7] = 0;
		myMatrix[8] = 1;		
	}

	Matrix33<Type>(Matrix44<Type> aMatrix)
	{
		myMatrix[0] = aMatrix.myData[0];
		myMatrix[1] = aMatrix.myData[1];
		myMatrix[2] = aMatrix.myData[2];
		myMatrix[3] = aMatrix.myData[4];
		myMatrix[4] = aMatrix.myData[5];
		myMatrix[5] = aMatrix.myData[6];
		myMatrix[6] = aMatrix.myData[8];
		myMatrix[7] = aMatrix.myData[9];
		myMatrix[8] = aMatrix.myData[10];
	}

	Matrix33<Type>(const Type anArray[9])
	{
		myMatrix = anArray;
	}
	const Matrix33<Type> operator+(const Matrix33<Type>& aMatrix33) const
	{
		Matrix33<Type> tempMatrix;
		tempMatrix.myMatrix[0] = myMatrix[0] + aMatrix33.myMatrix[0];
		tempMatrix.myMatrix[1] = myMatrix[1] + aMatrix33.myMatrix[1];
		tempMatrix.myMatrix[2] = myMatrix[2] + aMatrix33.myMatrix[2];
		tempMatrix.myMatrix[3] = myMatrix[3] + aMatrix33.myMatrix[3];
		tempMatrix.myMatrix[4] = myMatrix[4] + aMatrix33.myMatrix[4];
		tempMatrix.myMatrix[5] = myMatrix[5] + aMatrix33.myMatrix[5];
		tempMatrix.myMatrix[6] = myMatrix[6] + aMatrix33.myMatrix[6];
		tempMatrix.myMatrix[7] = myMatrix[7] + aMatrix33.myMatrix[7];
		tempMatrix.myMatrix[8] = myMatrix[8] + aMatrix33.myMatrix[8];
		return tempMatrix;
	}

	const Matrix33<Type> operator-(const Matrix33<Type>& aMatrix33) const
	{
		Matrix33<Type> tempMatrix;
		tempMatrix.myMatrix[0] = myMatrix[0] - aMatrix33.myMatrix[0];
		tempMatrix.myMatrix[1] = myMatrix[1] - aMatrix33.myMatrix[1];
		tempMatrix.myMatrix[2] = myMatrix[2] - aMatrix33.myMatrix[2];
		tempMatrix.myMatrix[3] = myMatrix[3] - aMatrix33.myMatrix[3];
		tempMatrix.myMatrix[4] = myMatrix[4] - aMatrix33.myMatrix[4];
		tempMatrix.myMatrix[5] = myMatrix[5] - aMatrix33.myMatrix[5];
		tempMatrix.myMatrix[6] = myMatrix[6] - aMatrix33.myMatrix[6];
		tempMatrix.myMatrix[7] = myMatrix[7] - aMatrix33.myMatrix[7];
		tempMatrix.myMatrix[8] = myMatrix[8] - aMatrix33.myMatrix[8];
		return tempMatrix;
	}

	const Matrix33<Type> operator*(const Matrix33<Type>& aMatrix33) const
	{
		Matrix33<Type> tempMatrix;
		tempMatrix.myMatrix[0] = myMatrix[0] * aMatrix33.myMatrix[0] + myMatrix[1] * aMatrix33.myMatrix[3] + myMatrix[2] * aMatrix33.myMatrix[6];
		tempMatrix.myMatrix[1] = myMatrix[0] * aMatrix33.myMatrix[1] + myMatrix[1] * aMatrix33.myMatrix[4] + myMatrix[2] * aMatrix33.myMatrix[7];
		tempMatrix.myMatrix[2] = myMatrix[0] * aMatrix33.myMatrix[2] + myMatrix[1] * aMatrix33.myMatrix[5] + myMatrix[2] * aMatrix33.myMatrix[8];
		tempMatrix.myMatrix[3] = myMatrix[3] * aMatrix33.myMatrix[0] + myMatrix[4] * aMatrix33.myMatrix[3] + myMatrix[5] * aMatrix33.myMatrix[6];
		tempMatrix.myMatrix[4] = myMatrix[3] * aMatrix33.myMatrix[1] + myMatrix[4] * aMatrix33.myMatrix[4] + myMatrix[5] * aMatrix33.myMatrix[7];
		tempMatrix.myMatrix[5] = myMatrix[3] * aMatrix33.myMatrix[2] + myMatrix[4] * aMatrix33.myMatrix[5] + myMatrix[5] * aMatrix33.myMatrix[8];
		tempMatrix.myMatrix[6] = myMatrix[6] * aMatrix33.myMatrix[0] + myMatrix[7] * aMatrix33.myMatrix[3] + myMatrix[8] * aMatrix33.myMatrix[6];
		tempMatrix.myMatrix[7] = myMatrix[6] * aMatrix33.myMatrix[1] + myMatrix[7] * aMatrix33.myMatrix[4] + myMatrix[8] * aMatrix33.myMatrix[7];
		tempMatrix.myMatrix[8] = myMatrix[6] * aMatrix33.myMatrix[2] + myMatrix[7] * aMatrix33.myMatrix[5] + myMatrix[8] * aMatrix33.myMatrix[8];
		return tempMatrix;
	}

	const Matrix33<Type> operator*(const Matrix44<Type>& aMatrix44) const
	{
		Matrix33<Type> tempMatrix;
		tempMatrix.myMatrix[0] = myMatrix[0] * aMatrix44.myData[0] + myMatrix[1] * aMatrix44.myData[4] + myMatrix[2] * aMatrix44.myData[8];
		tempMatrix.myMatrix[1] = myMatrix[0] * aMatrix44.myData[1] + myMatrix[1] * aMatrix44.myData[5] + myMatrix[2] * aMatrix44.myData[9];
		tempMatrix.myMatrix[2] = myMatrix[0] * aMatrix44.myData[2] + myMatrix[1] * aMatrix44.myData[6] + myMatrix[2] * aMatrix44.myData[10];
		tempMatrix.myMatrix[3] = myMatrix[3] * aMatrix44.myData[0] + myMatrix[4] * aMatrix44.myData[4] + myMatrix[5] * aMatrix44.myData[8];		
		tempMatrix.myMatrix[4] = myMatrix[3] * aMatrix44.myData[1] + myMatrix[4] * aMatrix44.myData[5] + myMatrix[5] * aMatrix44.myData[9];		
		tempMatrix.myMatrix[5] = myMatrix[3] * aMatrix44.myData[2] + myMatrix[4] * aMatrix44.myData[6] + myMatrix[5] * aMatrix44.myData[10];		
		tempMatrix.myMatrix[6] = myMatrix[6] * aMatrix44.myData[0] + myMatrix[7] * aMatrix44.myData[4] + myMatrix[8] * aMatrix44.myData[8];		
		tempMatrix.myMatrix[7] = myMatrix[6] * aMatrix44.myData[1] + myMatrix[7] * aMatrix44.myData[5] + myMatrix[8] * aMatrix44.myData[9];		
		tempMatrix.myMatrix[8] = myMatrix[6] * aMatrix44.myData[2] + myMatrix[7] * aMatrix44.myData[6] + myMatrix[8] * aMatrix44.myData[10];
		return tempMatrix;
	}

	const Matrix33<Type> Inverse() const
	{
		Matrix33<Type> tempMatrix;
		tempMatrix.myMatrix[0] = myMatrix[0];
		tempMatrix.myMatrix[1] = myMatrix[3];
		tempMatrix.myMatrix[2] = myMatrix[6];
		tempMatrix.myMatrix[3] = myMatrix[1];
		tempMatrix.myMatrix[4] = myMatrix[4];
		tempMatrix.myMatrix[5] = myMatrix[7];
		tempMatrix.myMatrix[6] = myMatrix[2];
		tempMatrix.myMatrix[7] = myMatrix[5];
		tempMatrix.myMatrix[8] = myMatrix[8];
		return tempMatrix;
	}


	const bool operator==(const Matrix33<Type>& aMatrix33) const
	{
		if (myMatrix[0] == aMatrix33.myMatrix[0] == true)
		{
			if (myMatrix[1] == aMatrix33.myMatrix[1] == true)
			{
				if (myMatrix[2] == aMatrix33.myMatrix[2] == true)
				{
					if (myMatrix[3] == aMatrix33.myMatrix[3] == true)
					{
						if (myMatrix[4] == aMatrix33.myMatrix[4] == true)
						{
							if (myMatrix[5] == aMatrix33.myMatrix[5] == true)
							{
								if (myMatrix[6] == aMatrix33.myMatrix[6] == true)
								{
									if (myMatrix[7] == aMatrix33.myMatrix[7] == true)
									{
										if (myMatrix[8] == aMatrix33.myMatrix[8] == true)
										{
											return (true);
										}

									}

								}

							}

						}

					}

				}

			}
		}
		return (false);		
	}

	Matrix33<float> operator=(const Matrix33<Type>& aMatrix33)
	{		
		myMatrix[0] = aMatrix33.myMatrix[0];
		myMatrix[1] = aMatrix33.myMatrix[1];
		myMatrix[2] = aMatrix33.myMatrix[2];
		myMatrix[3] = aMatrix33.myMatrix[3];
		myMatrix[4] = aMatrix33.myMatrix[4];
		myMatrix[5] = aMatrix33.myMatrix[5];
		myMatrix[6] = aMatrix33.myMatrix[6];
		myMatrix[7] = aMatrix33.myMatrix[7];
		myMatrix[8] = aMatrix33.myMatrix[8];
		return (*this);
	}

	Matrix33<Type> operator*=(const Matrix33<Type>& aMatrix33)
	{		
		Matrix33<Type> tempMatrix;
		tempMatrix.myMatrix[0] = myMatrix[0] * aMatrix33.myMatrix[0] + myMatrix[1] * aMatrix33.myMatrix[3] + myMatrix[2] * aMatrix33.myMatrix[6];
		tempMatrix.myMatrix[1] = myMatrix[0] * aMatrix33.myMatrix[1] + myMatrix[1] * aMatrix33.myMatrix[4] + myMatrix[2] * aMatrix33.myMatrix[7];
		tempMatrix.myMatrix[2] = myMatrix[0] * aMatrix33.myMatrix[2] + myMatrix[1] * aMatrix33.myMatrix[5] + myMatrix[2] * aMatrix33.myMatrix[8];
		tempMatrix.myMatrix[3] = myMatrix[3] * aMatrix33.myMatrix[0] + myMatrix[4] * aMatrix33.myMatrix[3] + myMatrix[5] * aMatrix33.myMatrix[6];
		tempMatrix.myMatrix[4] = myMatrix[3] * aMatrix33.myMatrix[1] + myMatrix[4] * aMatrix33.myMatrix[4] + myMatrix[5] * aMatrix33.myMatrix[7];
		tempMatrix.myMatrix[5] = myMatrix[3] * aMatrix33.myMatrix[2] + myMatrix[4] * aMatrix33.myMatrix[5] + myMatrix[5] * aMatrix33.myMatrix[8];
		tempMatrix.myMatrix[6] = myMatrix[6] * aMatrix33.myMatrix[0] + myMatrix[7] * aMatrix33.myMatrix[3] + myMatrix[8] * aMatrix33.myMatrix[6];
		tempMatrix.myMatrix[7] = myMatrix[6] * aMatrix33.myMatrix[1] + myMatrix[7] * aMatrix33.myMatrix[4] + myMatrix[8] * aMatrix33.myMatrix[7];
		tempMatrix.myMatrix[8] = myMatrix[6] * aMatrix33.myMatrix[2] + myMatrix[7] * aMatrix33.myMatrix[5] + myMatrix[8] * aMatrix33.myMatrix[8];

		for (int index = 0; index < 9; index++)
		{
			myMatrix[index] = tempMatrix.myMatrix[index];
		}
		return (*this);
	}

	static Matrix33<Type> CreateRotateAroundX(const Type aAngleInRadians)
	{
		Matrix33<Type> tempMatrix;
		tempMatrix.myMatrix[0] = 1;
		tempMatrix.myMatrix[1] = 0;
		tempMatrix.myMatrix[2] = 0;
		tempMatrix.myMatrix[3] = 0;
		tempMatrix.myMatrix[4] = static_cast<Type>(cos(aAngleInRadians));
		tempMatrix.myMatrix[5] = static_cast<Type>((-sin(aAngleInRadians)));
		tempMatrix.myMatrix[6] = 0;
		tempMatrix.myMatrix[7] = static_cast<Type>(sin(aAngleInRadians));
		tempMatrix.myMatrix[8] = static_cast<Type>(cos(aAngleInRadians));

		return (tempMatrix);
	}

	static Matrix33<Type> CreateRotateAroundY(const Type aAngleInRadians)
	{
		Matrix33<Type> tempMatrix;
		tempMatrix.myMatrix[0] = static_cast<Type>(cos(aAngleInRadians));
		tempMatrix.myMatrix[1] = 0;
		tempMatrix.myMatrix[2] = static_cast<Type>(sin(aAngleInRadians));
		tempMatrix.myMatrix[3] = 0;
		tempMatrix.myMatrix[4] = 1;
		tempMatrix.myMatrix[5] = 0;
		tempMatrix.myMatrix[6] = static_cast<Type>((-sin(aAngleInRadians)));
		tempMatrix.myMatrix[7] = 0;
		tempMatrix.myMatrix[8] = static_cast<Type>(cos(aAngleInRadians));

		return (tempMatrix);
	}

	static Matrix33<Type> CreateRotateAroundZ(const Type aAngleInRadians)
	{
		Matrix33<Type> tempMatrix;
		tempMatrix.myMatrix[0] = static_cast<Type>(cos(aAngleInRadians));
		tempMatrix.myMatrix[1] = static_cast<Type>((-sin(aAngleInRadians)));
		tempMatrix.myMatrix[2] = 0;
		tempMatrix.myMatrix[3] = static_cast<Type>(sin(aAngleInRadians));
		tempMatrix.myMatrix[4] = static_cast<Type>(cos(aAngleInRadians));
		tempMatrix.myMatrix[5] = 0;
		tempMatrix.myMatrix[6] = 0;
		tempMatrix.myMatrix[7] = 0;
		tempMatrix.myMatrix[8] = 1;

		return (tempMatrix);
	}

	static Matrix33 <Type> Identity()
	{
		Matrix33<Type> tempMatrix;
		tempMatrix.myMatrix[0] = 1;
		tempMatrix.myMatrix[1] = 0;
		tempMatrix.myMatrix[2] = 0;
		tempMatrix.myMatrix[3] = 0;
		tempMatrix.myMatrix[4] = 1;
		tempMatrix.myMatrix[5] = 0;
		tempMatrix.myMatrix[6] = 0;
		tempMatrix.myMatrix[7] = 0;
		tempMatrix.myMatrix[8] = 1;

		return (tempMatrix);
	}

	static Matrix33 <Type> RotationAroundVector(const Type aX, const Type aY, const Type aZ, const Type anAngle)
	{
		Type c, s, t;
		c = cos(anAngle);
		s = sin(anAngle);
		t = 1-cos(anAngle);

		Type result[9];

		// http://www.cprogramming.com/tutorial/3d/rotation.html

		result[(0 * 3) + 0] = t * pow(aX, 2) + c;
		result[(0 * 3) + 1] = t * aX * aY - s * aZ;
		result[(0 * 3) + 2] = t * aX * aZ + s * aY;

		result[(1 * 3) + 0] = t * aX * aY + s * aZ;
		result[(1 * 3) + 1] = t * pow(aY, 2) + c;
		result[(1 * 3) + 2] = t * aY * aZ - s * aX;

		result[(2 * 3) + 0] = t * aX * aZ - s * aY;
		result[(2 * 3) + 1] = t * aY * aZ + s * aX;
		result[(2 * 3) + 2] = t * pow(aZ, 2) + c;
	
		Matrix33<Type> resultMatrix;
		memcpy(&resultMatrix.myMatrix[0], &result[0], sizeof(Type) * 9);
			
		return resultMatrix;
	}

	Type myMatrix [9];
};

#endif