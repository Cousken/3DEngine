#include <stdafx.h>
#include <windows.h>
#include "DL_Debug.h"
#include "CommonMacros.h"
#include "DL_Assert.h"
#include <sstream>
#include <stdarg.h>
#include "DL_Stackwalker.h"

using namespace DL_Debug;

Debug* Debug::ourInstance=NULL;

bool Debug::Create(std::string aFile, std::bitset<4> someAssertFlags)
{
	ourInstance= new Debug();
	if(!ourInstance || !ourInstance->Init(aFile,someAssertFlags))
	{
		SAFE_DELETE(ourInstance);
		return(false);
	}
	return(true);
}
bool Debug::Destroy()
{
	if(ourInstance->myDebugFile.Close()==false) return(false);
	SAFE_DELETE(ourInstance);
	if(ourInstance==NULL) return(true);
	else return(false);
}

Debug* Debug::GetInstance()
{
	return(ourInstance);
}

void  Debug::PrintMessage(const char * aString)
{
	myDebugFile.WriteString(aString);
	myDebugFile.FlushToDisk();

}

void Debug::DebugMessage(int aLine,const char* aFileName,const char * aFormattedString, ...)
{
	va_list variableArguments;
	va_start(variableArguments,aFormattedString);
	
	PrintDebugMessage(aLine,aFileName,"DEBUG",aFormattedString,variableArguments);

	if(myAssertLevelFlags.test(DEBUG_MESSAGE)) assert(0 && "Asserted at debug message");
}

void Debug::ErrorMessage(int aLine,const char* aFileName,const char * aFormattedString, ...)
{
	va_list variableArguments;
	va_start(variableArguments,aFormattedString);
	
	PrintDebugMessage(aLine,aFileName,"ERROR",aFormattedString,variableArguments);

	if(myAssertLevelFlags.test(ERROR_MESSAGE)) assert(0 && "Asserted at Error message");
}

void Debug::CriticalErrorMessage(int aLine,const char* aFileName,const char * aFormattedString, ...)
{
	va_list variableArguments;
	va_start(variableArguments,aFormattedString);
	
	PrintDebugMessage(aLine,aFileName,"CRITICAL",aFormattedString,variableArguments);
	if(myAssertLevelFlags.test(CRITICAL_ERROR))
	{
		assert(0 && "Asserted at critical error message");
	}
	else
	{
		DL_PRINT("DUMPING STACK\n");
		StackWalker sw;
		sw.ShowCallstack();
	}
}

void Debug::SevereErrorMessage(int aLine,const char* aFileName,const char * aFormattedString, ...)
{
	
	va_list variableArguments;
	va_start(variableArguments,aFormattedString);
	
	PrintDebugMessage(aLine,aFileName,"SEVERE",aFormattedString,variableArguments);


	if(myAssertLevelFlags.test(SEVERE_ERROR)) 
	{
		assert(0 && "Asserted at severe error message");
	}
	else
	{
		DL_PRINT("DUMPING STACK\n");
		StackWalker sw;
		sw.ShowCallstack();
	}

}


void Debug::PrintDebugMessage(int aLine,const char* aFileName,char* aOutputMessage,const char * aFormattedString,va_list someArguments)
{
	int size = 0;
	char buffer[4096];
	memset(buffer, 0, sizeof(buffer));
	std::string outputString;
	std::ostringstream stream(outputString);

	SYSTEMTIME time;
	GetLocalTime(&time);

	stream << "[" << time.wHour << ":" << time.wMinute << ":" << time.wSecond << ":" << time.wMilliseconds << "]" 
		<< "[" << aFileName << "]:[LINE:" << aLine << "] " << aOutputMessage << " ";
	size = vsnprintf_s( buffer, sizeof(buffer), _TRUNCATE, aFormattedString, someArguments);
	stream << buffer << std::endl;
	myDebugFile.WriteString(stream.str());
	myDebugFile.FlushToDisk();
}


void Debug::AssertMessage(const wchar_t* aFileName,int aLine,const wchar_t* aFunctionName,const wchar_t * aString)
{
	std::string outputString;
	std::ostringstream stream(outputString);
	stream << " ASSERTION " << aString << " In " << aFileName << " Function : " << aFunctionName << " Line : " << aLine << std::endl;
	myDebugFile.WriteString(stream.str());
	myDebugFile.FlushToDisk();
	StackWalker sw;
	sw.ShowCallstack();
	_wassert(aString,aFileName,aLine);
}

Debug::Debug()
{
}

Debug::~Debug()
{
}

void ExpHandler()
{
	StackWalker sw;
	sw.ShowCallstack(GetCurrentThread());
}

LONG WINAPI ExpFilter(EXCEPTION_POINTERS* pExp)
{
	StackWalker sw;
	sw.ShowCallstack(GetCurrentThread(), pExp->ContextRecord);
	return EXCEPTION_EXECUTE_HANDLER;
}


bool Debug::Init(std::string aFile, std::bitset<4> someAssertFlags)
{
	myAssertLevelFlags=someAssertFlags;
	if(myDebugFile.Open(aFile,1 << FI_File::WRITE)==false)
	{
		assert(0 && "Failed to init debug system");
		return(false);
	}
	SetUnhandledExceptionFilter((LPTOP_LEVEL_EXCEPTION_FILTER)ExpFilter);
	
	set_terminate(ExpHandler);
	return(true);
}
