#ifndef COMMON_UTILITIES_STATICARRAY_HEADER
#define COMMON_UTILITIES_STATICARRAY_HEADER

#include "LoopMacros.h"

namespace CommonUtilities
{
	template <class Type,int Size>
	class StaticArray
	{
	public:
		StaticArray();
		~StaticArray();

		StaticArray& operator=(StaticArray& aGrowingArray);

		inline Type& operator[](const int& aIndex);
		inline const Type& operator[](const int& aIndex) const;

	// Utility functions 

		inline void Insert(int aIndex,Type& aObject);
		inline void DeleteAll();
		inline int GetSize();

	private:
		Type myArray[Size];
		int myCurNrOfItems;
	};


	template <class Type,int Size>
	StaticArray<Type,Size>::StaticArray()
	{
		myCurNrOfItems = 0;
	}

	template <class Type,int Size>
	StaticArray<Type,Size>::~StaticArray()
	{
	}

	template <class Type,int Size>
	StaticArray<Type,Size>& StaticArray<Type,Size>::operator=( StaticArray<Type,Size>& aGrowingArray)
	{
		For_Count_i(Size)
		{
			myArray[i]=aGrowingArray[i];
		}
		return (*this);
	}

	template <class Type,int Size>
	Type& StaticArray<Type,Size>::operator[](const int& aIndex)
	{
		return(myArray[aIndex]);
	}

	template <class Type,int Size>
	inline const Type& StaticArray<Type,Size>::operator[](const int &aIndex) const
	{
		assert(static_cast<unsigned int>(aIndex) < Size && static_cast<unsigned int>(aIndex) >= 0, "Operator[] out of bounds");

		return myArray[aIndex];
	}

	// Utility functions 
	template <class Type,int Size>
	void StaticArray<Type,Size>::Insert(int aIndex,Type& aObject)
	{
		assert(aIndex<Size && "ERROR IN INSERTION TRYING TO INSERT AFTER THE LAST ELEMMENT");
		
		for( int i=__min(myCurNrOfItems-1,Size-1);i>=aIndex;i--)
		{
			myArray[i+1]=myArray[i];
		}
		myArray[aIndex]=aObject;
		myCurNrOfItems++;
	}

	template <class Type,int Size>
	void StaticArray<Type,Size>::DeleteAll()
	{
		For_Count_i(myCurNrOfItems)
		{
			SAFE_DELETE(myArray[i]);
		}
		myCurNrOfItems=0;

	}

	template <class Type,int Size>
	int StaticArray<Type, Size>::GetSize()
	{
		return Size;
	}
};

#endif
