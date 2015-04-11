#ifndef LOOPMACROS_HEADER
#define LOOPMACROS_HEADER

namespace CommonUtilities
{
	namespace Macros
	{

		#define For_Each(index,vector) for(index=0;index<vector.size();index++)  
		#define For_Each_index(vector) for(unsigned int index=0;index<vector.size();index++)
		#define For_Each_i(vector) for(unsigned int i=0;i<vector.size();i++)
		#define For_Each_j(vector) for(unsigned int j=0;j<vector.size();j++)	
		#define For_Count_i(count) for(int i=0;i<count;i++) 
		#define For_Count(index,count) for(int index=0;index<count;index++) 
		#define ForCount(count) for(int qiwnedretxyz_lippless=0;qiwnedretxyz_lippless<count;qiwnedretxyz_lippless++) 
	};
};

#endif