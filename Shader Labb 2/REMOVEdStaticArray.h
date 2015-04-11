#ifndef COMMON_UTILITIES_STATICARRAY_HEADER
#define COMMON_UTILITIES_STATICARRAY_HEADER

#define __min(a,b)  (((a) < (b)) ? (a) : (b))

#include "LoopMacros.h"

namespace CommonUtilities
{
	template <class Type,int Size>
	class StaticArray
	{
	public:
		StaticArray();
		~StaticArray();

		StaticArray& operator=(const StaticArray& aGrowingArray);

		inline Type& operator[](const int& aIndex);

	// Utility functions 

		inline void Insert(int aIndex,Type& aObject);
		inline void DeleteAll();

	private:
		Type myArray[Size];
	};


	template <class Type,int Size>
	StaticArray<Type,Size>::StaticArray()
	{
	}

	template <class Type,int Size>
	StaticArray<Type,Size>::~StaticArray()
	{
	}

	template <class Type,int Size>
	StaticArray<Type,Size>& StaticArray<Type,Size>::operator=(const StaticArray<Type,Size>& aGrowingArray)
	{
		For_Count_i(Size)
		{
			myArray[i]=aGrowingArray[i];
		}
	}

	template <class Type,int Size>
	Type& StaticArray<Type,Size>::operator[](const int& aIndex)
	{
		return(myArray[aIndex]);
	}

	// Utility functions 
	template <class Type,int Size>
	void StaticArray<Type,Size>::Insert(int aIndex,Type& aObject)
	{
		assert(aIndex<Size && "ERROR IN INSERTION TRYING TO INSERT AFTER THE LAST ELEMMENT");
		
		for( int i=__min(myCurNrOfItems-1,Size-1);i>=aIndex;i--)
		{
			myItemList[i+1]=myItemList[i];
		}
		myItemList[aIndex]=aObject;
	}

	template <class Type,int Size>
	void StaticArray<Type,Size>::DeleteAll()
	{
		For_Count_i(myCurNrOfItems)
		{
			SAFE_DELETE(myItemList[i]);
		}
		myCurNrOfItems=0;

	}
};

#endif
