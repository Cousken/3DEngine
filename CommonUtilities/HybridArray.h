#ifndef COMMONUTILITIES_HYBRIDGARRAY_HEADER
#define COMMONUTILITIES_HYBRIDGARRAY_HEADER

#include "LoopMacros.h"
#include "DL_Assert.h"

namespace CommonUtilities
{
	template <class Type,class CountType =unsigned short,int MaxSize=0, int ItemIncreaseSize=0,bool UseSafeModeFlag=true>
	class HybridArray
	{
	public:
		HybridArray();
		HybridArray(const HybridArray& aHybridArray);
		~HybridArray();

		HybridArray& operator=(const HybridArray& aHybridArray);
	
		inline Type& operator[](const int& aIndex) const;

		inline void Add(const Type& aObject);
		inline void Insert(int aIndex,Type& aObject);
		inline void DeleteCyclic(Type& aObject);
		inline void DeleteCyclicAtIndex(int aItemNumber);
		inline void RemoveCyclic(const Type& aObject);
		inline void RemoveCyclicAtIndex(int aItemNumber);


		inline void RemoveAll();
		inline void DeleteAll();

		void Optimize();
		__forceinline int Count() const;

	protected:

		inline void ReSize(int aNewSize);

		Type myArray[MaxSize];
		Type* myItemList;
		CountType myCurNrOfItems;
		CountType myMaxNrOfItems;
		CountType myItemIncreaseSize;
		bool myUseSafeModeFlag;

	};

	template <class Type,class CountType,int MaxSize, int ItemIncreaseSize,bool UseSafeModeFlag>
	HybridArray<Type,CountType,MaxSize,ItemIncreaseSize,UseSafeModeFlag>& HybridArray<Type,CountType,MaxSize,ItemIncreaseSize,UseSafeModeFlag>::operator=(const HybridArray<Type,CountType,MaxSize,ItemIncreaseSize,UseSafeModeFlag>& aHybridArray)
	{
		if(myMaxNrOfItems!=aHybridArray.myMaxNrOfItems)
		{
			if(myItemList!=NULL) delete [] myItemList;
			myItemList=new Type [myMaxNrOfItems] ();
		}
		if(mySafeModeFlag==true)
		{
			For_Count_i(myCurNrOfItems)
			{
				myItemList[i]=aHybridArray.myItemList[i];
			}
		}
		else
		{
			memcpy(myItemList,aHybridArray.myItemList,myCurNrOfItems*sizeof(Type*));
		}


		myCurNrOfItems=aHybridArray.myCurNrOfItems;
		myMaxNrOfItems=aHybridArray.myMaxNrOfItems;
		myItemIncreaseSize=aHybridArray.myItemIncreaseSize;
		myUseSafeModeFlag=aHybridArray.myUseSafeModeFlag;	
		
	}

	template <class Type,class CountType,int MaxSize, int ItemIncreaseSize,bool UseSafeModeFlag>
	HybridArray<Type,CountType,MaxSize,ItemIncreaseSize,UseSafeModeFlag>::HybridArray()
	{
		myItemList=myArray;
		myCurNrOfItems=0;
		myMaxNrOfItems=MaxSize;
		myItemIncreaseSize=ItemIncreaseSize;
		myUseSafeModeFlag=UseSafeModeFlag;
	}
	
	template <class Type,class CountType,int MaxSize, int ItemIncreaseSize,bool UseSafeModeFlag>
	HybridArray<Type,CountType,MaxSize, ItemIncreaseSize,UseSafeModeFlag>::HybridArray(const HybridArray<Type,CountType,MaxSize, ItemIncreaseSize,UseSafeModeFlag>& aHybridArray)
	{
		*this=aHybridArray;
	}


	template <class Type,class CountType,int MaxSize, int ItemIncreaseSize,bool UseSafeModeFlag>
	HybridArray<Type,CountType,MaxSize, ItemIncreaseSize,UseSafeModeFlag>::~HybridArray()
	{
		RemoveAll();
		if((myItemList!=myArray)) delete [] (myItemList);
		myItemList=myArray;
	}




	template <class Type,class CountType,int MaxSize, int ItemIncreaseSize,bool UseSafeModeFlag>
	Type& HybridArray<Type,CountType,MaxSize, ItemIncreaseSize,UseSafeModeFlag>::operator[](const int& aIndex) const 
	{
		assert((aIndex>=0) && (aIndex<myMaxNrOfItems) && "OUT OF BOUNDS ERROR");
		return(myItemList[aIndex]);
	}

	template <class Type,class CountType,int MaxSize, int ItemIncreaseSize,bool UseSafeModeFlag>
	void HybridArray<Type,CountType,MaxSize, ItemIncreaseSize,UseSafeModeFlag>::Add(const Type& aItem)
	{
		if(myCurNrOfItems==myMaxNrOfItems) ReSize(myMaxNrOfItems+myItemIncreaseSize);
		myItemList[myCurNrOfItems]=aItem;
		myCurNrOfItems++;
	}


	template <class Type,class CountType,int MaxSize, int ItemIncreaseSize,bool UseSafeModeFlag>
	void HybridArray<Type,CountType,MaxSize, ItemIncreaseSize,UseSafeModeFlag>::DeleteCyclic(Type& aItem)
	{
		int i;
		For_Count_i(myCurNrOfItems)
		{
			if(myItemList[i]==aItem)
			{
				delete(myItemList[i]);
				myItemList[i]=myItemList[myCurNrOfItems-1];
				myCurNrOfItems--;
				return;
			}
		}
	}

	template <class Type,class CountType,int MaxSize, int ItemIncreaseSize,bool UseSafeModeFlag>
	void HybridArray<Type,CountType,MaxSize, ItemIncreaseSize,UseSafeModeFlag>::DeleteCyclicAtIndex(int aItemNumber)
	{
		delete(myItemList[aItemNumber]);
		myItemList[aItemNumber]=myItemList[myCurNrOfItems-1];
		myCurNrOfItems--;
	}

	template <class Type,class CountType,int MaxSize, int ItemIncreaseSize,bool UseSafeModeFlag>
	void HybridArray<Type,CountType,MaxSize, ItemIncreaseSize,UseSafeModeFlag>::RemoveCyclic(const Type& aItem) 
	{
		int i;
		For_Count_i(myCurNrOfItems)
		{
			if(myItemList[i]==aItem)
			{
				myItemList[i]=myItemList[myCurNrOfItems-1];
				myCurNrOfItems--;
				return;
			}
		}
	}

	template <class Type,class CountType,int MaxSize, int ItemIncreaseSize,bool UseSafeModeFlag>
	void HybridArray<Type,CountType,MaxSize, ItemIncreaseSize,UseSafeModeFlag>::RemoveCyclicAtIndex(int aItemNumber)
	{
		myItemList[aItemNumber]=myItemList[myCurNrOfItems-1];
		myCurNrOfItems--;
	}

	template <class Type,class CountType,int MaxSize, int ItemIncreaseSize,bool UseSafeModeFlag>
	void HybridArray<Type,CountType,MaxSize, ItemIncreaseSize,UseSafeModeFlag>::Insert(int aIndex,Type& aObject)
	{
		assert(aIndex<myCurNrOfItems && "ERROR IN INSERTION TRYING TO INSERT AFTER THE LAST ELEMMENT");
		if(myCurNrOfItems==myMaxNrOfItems) ReSize(myMaxNrOfItems+myItemIncreaseSize);
		for(i=myCurNrOfItems-1;i>=aIndex;i--)
		{
			myItemList[i+1]=myItemList[i];
		}
		myItemList[aIndex]=aObject;
	}

	template <class Type,class CountType,int MaxSize, int ItemIncreaseSize,bool UseSafeModeFlag>
	void HybridArray<Type,CountType,MaxSize, ItemIncreaseSize,UseSafeModeFlag>::RemoveAll()
	{
		myCurNrOfItems=0;
	}

	template <class Type,class CountType,int MaxSize, int ItemIncreaseSize,bool UseSafeModeFlag>
	void HybridArray<Type,CountType,MaxSize, ItemIncreaseSize,UseSafeModeFlag>::DeleteAll()
	{
		For_Count_i(myCurNrOfItems)
		{
			SAFE_DELETE(myItemList[i]);
		}
		myCurNrOfItems=0;
		
	}

	template <class Type,class CountType,int MaxSize, int ItemIncreaseSize,bool UseSafeModeFlag>
	void HybridArray<Type,CountType,MaxSize, ItemIncreaseSize,UseSafeModeFlag>::Optimize()
	{
		ReSize(myCurNrOfItems);
	}

	template <class Type,class CountType,int MaxSize, int ItemIncreaseSize,bool UseSafeModeFlag>
	int HybridArray<Type,CountType,MaxSize, ItemIncreaseSize,UseSafeModeFlag>::Count() const
	{
		return(myCurNrOfItems);
	}

	template <class Type,class CountType,int MaxSize, int ItemIncreaseSize,bool UseSafeModeFlag>
	void HybridArray<Type,CountType,MaxSize, ItemIncreaseSize,UseSafeModeFlag>::ReSize(int aNewSize)
	{
			assert(((aNewSize)>myMaxNrOfItems) && "ERROR IN GROWTH");

			Type tempList=new Type* [aNewSize] ();
			if(mySafeModeFlag==true)
			{
				For_Count_i(myCurNrOfItems)
				{
					tempList[i]=myItemList[i];
				}
			}
			else
			{
				memcpy(tempList,myItemList,myCurNrOfItems*sizeof(Type*));
			}
			if(myItemList!=myArray)
			{
				delete [] (myItemList);
			}
			myMaxNrOfItems=aNewSize;
			myItemList=tempList;
	}

}


		


#endif