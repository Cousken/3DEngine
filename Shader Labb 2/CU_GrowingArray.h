#ifndef COMMONUTILITIES_GROWINGARRAY_HEADER
#define COMMONUTILITIES_GROWINGARRAY_HEADER

#include "LoopMacros.h"
#include "CommonMacros.h"
#include "assert.h"
#include "windows.h"

namespace CU = CommonUtilities;

namespace CommonUtilities
{
	template <class Type,class CountType =unsigned short>
	class GrowingArray
	{
	public:
		GrowingArray();
		GrowingArray(const CountType aNrOfRecomendedItems,const CountType aItemIncreaseSize, bool aUseSafeModeFlag=true);
		GrowingArray(const GrowingArray& aGrowingArray);
		~GrowingArray();

		GrowingArray& operator=(const GrowingArray& aGrowingArray);


		void Init(const CountType aNrOfRecomendedItems,const CountType aItemIncreaseSize,bool aUseSafeModeFlag=true);
		void ReInit(const CountType aNrOfRecomendedItems,const CountType aItemIncreaseSize,bool aUseSafeModeFlag=true);

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
		__forceinline CountType Count() const;

		inline CountType MaxSize() const;

	protected:

		inline void ReSize(const CountType aNewSize);

		Type* myItemList;
		CountType myCurNrOfItems;
		CountType myMaxNrOfItems;
		CountType myItemIncreaseSize;
		bool myUseSafeModeFlag;

	};

	template <class Type,class CountType>
	GrowingArray<Type,CountType>::GrowingArray()
	{
		myItemList=NULL;
		myCurNrOfItems=0;
		myMaxNrOfItems=0;
		myItemIncreaseSize=0;
		myUseSafeModeFlag=true;
	}

	template <class Type,class CountType>
	GrowingArray<Type,CountType>::GrowingArray(const CountType aNrOfRecomendedItems,const CountType aItemIncreaseSize,bool aUseSafeModeFlag)
	{
		myItemList=NULL;
		myCurNrOfItems=0;
		myUseSafeModeFlag=aUseSafeModeFlag;
		Init(aNrOfRecomendedItems,aItemIncreaseSize);
	}

	template <class Type,class CountType>
	GrowingArray<Type,CountType>::GrowingArray(const GrowingArray<Type,CountType>& aGrowingArray)
	{
		myItemList = NULL;
		*this=aGrowingArray;
	}

	template <class Type,class CountType>
	GrowingArray<Type,CountType>& GrowingArray<Type,CountType>::operator=(const GrowingArray<Type,CountType>& aGrowingArray)
	{
		if( myItemList == NULL )
		{
			Init(aGrowingArray.myMaxNrOfItems,aGrowingArray.myItemIncreaseSize,aGrowingArray.myUseSafeModeFlag);
		}
		else
		{
			ReInit(aGrowingArray.myMaxNrOfItems,aGrowingArray.myItemIncreaseSize,aGrowingArray.myUseSafeModeFlag);
		}

		if(myUseSafeModeFlag==true)
		{
			for(int i=0;i<aGrowingArray.Count();i++)
			{
				myItemList[i]=aGrowingArray[i];
			}
		}
		else
		{
			memcpy(myItemList,&aGrowingArray[0],aGrowingArray.Count()*sizeof(Type));
		}
		myCurNrOfItems=aGrowingArray.Count();
		return(*this);
	}



	template <class Type,class CountType>
	GrowingArray<Type,CountType>::~GrowingArray()
	{
		RemoveAll();
		if(myItemList!=NULL) delete [] (myItemList);
		myItemList=NULL;
	}


	template <class Type,class CountType>
	void GrowingArray<Type,CountType>::Init(const CountType aNrOfRecomendedItems,const CountType aItemIncreaseSize,bool aUseSafeModeFlag)
	{
		assert(myItemList==NULL && "CALLED INIT ON AN ALREADY INITED GROWINGARRAY PLEASE USE REINIT");

		myMaxNrOfItems=aNrOfRecomendedItems;
		myItemIncreaseSize=aItemIncreaseSize;
		myUseSafeModeFlag=aUseSafeModeFlag;
		myCurNrOfItems=0;

		myItemList = new Type [myMaxNrOfItems] ();
	}

	template <class Type,class CountType>
	void GrowingArray<Type,CountType>::ReInit(const CountType aNrOfRecomendedItems,const CountType aItemIncreaseSize,bool aUseSafeModeFlag)
	{
		assert(myItemList!=NULL && "CALLED REINIT ON AN UNINITED GROWINGARRAY PLEASE USE INIT");

		delete [] (myItemList);

		myMaxNrOfItems=aNrOfRecomendedItems;
		myItemIncreaseSize=aItemIncreaseSize;
		myUseSafeModeFlag=aUseSafeModeFlag;
		myCurNrOfItems=0;

		myItemList = new Type [myMaxNrOfItems] ();
	}



	template <class Type,class CountType>
	Type& GrowingArray<Type,CountType>::operator[](const int& aIndex) const 
	{
		assert((aIndex>=0) && (aIndex<myCurNrOfItems) && "OUT OF BOUNDS ERROR");
		return(myItemList[aIndex]);
	}

	template <class Type,class CountType>
	void GrowingArray<Type,CountType>::Add(const Type& aItem)
	{
		if(myCurNrOfItems==myMaxNrOfItems) ReSize(myMaxNrOfItems+myItemIncreaseSize);
		myItemList[myCurNrOfItems]=aItem;
		myCurNrOfItems++;
	}


	template <class Type,class CountType>
	void GrowingArray<Type,CountType>::DeleteCyclic(Type& aItem)
	{
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

	template <class Type,class CountType>
	void GrowingArray<Type,CountType>::DeleteCyclicAtIndex(int aItemNumber)
	{
		delete(myItemList[aItemNumber]);
		myItemList[aItemNumber]=myItemList[myCurNrOfItems-1];
		myCurNrOfItems--;
	}

	template <class Type,class CountType>
	void GrowingArray<Type,CountType>::RemoveCyclic(const Type& aItem) 
	{
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

	template <class Type,class CountType>
	void GrowingArray<Type,CountType>::RemoveCyclicAtIndex(int aItemNumber)
	{
		myItemList[aItemNumber]=myItemList[myCurNrOfItems-1];
		myCurNrOfItems--;
	}

	template <class Type,class CountType>
	void GrowingArray<Type,CountType>::Insert(int aIndex,Type& aObject)
	{
		if(myCurNrOfItems==myMaxNrOfItems) ReSize(myMaxNrOfItems+myItemIncreaseSize);
		for(int i=myCurNrOfItems-1;i>=aIndex;i--)
		{
			myItemList[i+1]=myItemList[i];
		}
		myItemList[aIndex]=aObject;
		myCurNrOfItems++;
	}

	template <class Type,class CountType>
	void GrowingArray<Type,CountType>::RemoveAll()
	{
		myCurNrOfItems=0;
	}

	template <class Type,class CountType>
	void GrowingArray<Type,CountType>::DeleteAll()
	{
		For_Count_i(myCurNrOfItems)
		{
			CU_SAFE_DELETE(myItemList[i]);
		}
		myCurNrOfItems=0;

	}

	template <class Type,class CountType>
	void GrowingArray<Type,CountType>::Optimize()
	{
		ReSize(myCurNrOfItems);
	}

	template <class Type,class CountType>
	CountType GrowingArray<Type,CountType>::Count() const
	{
		return(myCurNrOfItems);
	}

	template <class Type,class CountType>
	CountType GrowingArray<Type,CountType>::MaxSize() const
	{
		return myMaxNrOfItems;
	}

	template <class Type,class CountType>
	void GrowingArray<Type,CountType>::ReSize(const CountType aNewSize)
	{
		assert(((aNewSize)>myMaxNrOfItems) && "ERROR IN GROWTH");

		Type* tempList=new Type [aNewSize] ();
		if(myUseSafeModeFlag==true)
		{
			for(int i=0;i<myCurNrOfItems;i++)
			{
				tempList[i]=myItemList[i];
			}
		}
		else
		{
			memcpy(tempList,myItemList,myCurNrOfItems*sizeof(Type*));
		}
		delete [] (myItemList);
		myMaxNrOfItems=aNewSize;
		myItemList=tempList;
	}

}





#endif