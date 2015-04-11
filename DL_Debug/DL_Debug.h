#ifndef DI_DEBUG_HEADER
#define DI_DEBUG_HEADER

#include <bitset>
#include "FI_File.h"

#define DL_ASSERT(string) DL_Debug::Debug::GetInstance()->AssertMessage(_CRT_WIDE(__FILE__),__LINE__,_CRT_WIDE(__FUNCTION__),_CRT_WIDE(string));
#define DL_PRINT(string)  DL_Debug::Debug::GetInstance()->PrintMessage(string);
#define DL_DEBUG( ...)  DL_Debug::Debug::GetInstance()->DebugMessage(__LINE__,__FUNCTION__,__VA_ARGS__);
#define DL_ERROR( ...)	DL_Debug::Debug::GetInstance()->ErrorMessage(__LINE__,__FUNCTION__,__VA_ARGS__);
#define DL_SEVERE( ...)	DL_Debug::Debug::GetInstance()->SevereErrorMessage(__LINE__,__FUNCTION__,__VA_ARGS__);
#define DL_CRITICAL( ...)	DL_Debug::Debug::GetInstance()->CriticalErrorMessage(__LINE__,__FUNCTION__,__VA_ARGS__);


namespace DL_Debug
{
	enum ErrorLevels
	{
		DEBUG_MESSAGE,
		ERROR_MESSAGE,
		SEVERE_ERROR,
		CRITICAL_ERROR
	};

	class Debug
	{
	public:
		static bool Create(std::string aFile = "DebugLogger.txt", std::bitset<4> someAssertFlags = 12);
		static bool Destroy();

		static Debug* GetInstance();

		void PrintMessage(const char *aString);
		
		void AssertMessage(const wchar_t* aFileName,int aLine,const wchar_t* aFunctionName,const wchar_t * aString);
//		void AssertMessage(const char* aFileName,int aLine,const char* aFunctionName,const char * aString);
		void DebugMessage(int aLine,const char* aFileName,const char * aFormattedString, ...);
		void ErrorMessage(int aLine,const char* aFileName,const char * aFormattedString, ...);
		void SevereErrorMessage(int aLine,const char* aFileName,const char * aFormattedString, ...);
		void CriticalErrorMessage(int aLine,const char* aFileName,const char * aFormattedString, ...);

	private:
		void PrintDebugMessage(int aLine,const char* aFileName,char* aOutputMessage,const char * aFormattedString,va_list someArguments);
		Debug();
		~Debug();
		bool Init(std::string aFile,std::bitset<4> someAssertFlags);
		static Debug* ourInstance;
		FI_File::File myDebugFile;
		std::bitset<4> myAssertLevelFlags;

	};
}

#endif
