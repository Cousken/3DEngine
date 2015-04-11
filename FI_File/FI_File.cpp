#include <stdafx.h>
#include "FI_File.h"
#include <stdio.h>
#include <io.h>
#include "DL_Assert.h"
#include <sstream>
#include <windows.h>

using namespace FI_File;


File::File()
{
	myFile=NULL;
}

File::~File()
{
	myFile=NULL;
}


int File::GetFileSize()
{
	long size = _filelength(_fileno(myFile));
	return(size);
}
	

bool File::Open(std::string aFileName, std::bitset<8> aSetOfFlags)
{
	std::string flags;
	
	if(myFile!=NULL)
	{
		if(Close()==false)
		{
			return(false);
		}
	}
	myIsBinaryFlag=aSetOfFlags.test(BINARY);

	assert(aSetOfFlags.any());

	if(aSetOfFlags.test(READ))
	{
		if(aSetOfFlags.test(WRITE))
		{
			flags+="r+";
		}
		else
		{
			flags+="r";
		}
	}
	else
	{
		if(aSetOfFlags.test(WRITE))
		{
			if(aSetOfFlags.test(APPEND))
			{
				flags+="a+";
			}
			else
			{
				flags+="w";
			}
		}
		else
		{
			assert(aSetOfFlags.test(APPEND));
			flags+="a";
		}
	}

	if(aSetOfFlags.test(BINARY))
	{
		flags+="b";
	}
	if(fopen_s(&myFile,aFileName.c_str(),flags.c_str())!=0)
	{
		return(false);
	}
	if(myFile==NULL)
	{
		return(false);
	}
	return(true);
}

bool File::Close()
{
	if(fclose(myFile)!=0)
	{
		return(false);
	}
	myFile=NULL;
	return(true);
}



bool File::GoToNewLine()
{
	assert(myIsBinaryFlag==false);
	unsigned char input;
	do {
		if(fread(&input,1,1,myFile)!=1)
		{
			return(false);
		}
	}while (input!=13);
	return(true);
}

bool File::TextReadInt(int& aInteger)
{
	std::string inputBuffer;
	unsigned char input;
	bool found=true;
	do {
		if(fread(&input,1,1,myFile)!=1)
		{
			aInteger=atoi(inputBuffer.c_str());
			return(false);
		}
		found=false;
		if(isdigit(input) || (input=='-')) 
		{
			inputBuffer+=input;
			found=true;
		}
		
	} while ((inputBuffer.size()==0) || (found==true));
	aInteger=atoi(inputBuffer.c_str());
	return(true);

}

bool File::TextReadBool(bool& aBool)
{
	std::string inputBuffer;
	unsigned char input;
	bool found=true;
	do {
		if(fread(&input,1,1,myFile)!=1)
		{
			aBool=false;
			return(false);
		}
		found=false;
		if(isdigit(input) || (input=='-')) 
		{
			inputBuffer+=input;
			found=true;
		}
		
	} while ((inputBuffer.size()==0) || (found==true));
	int integer=atoi(inputBuffer.c_str());
	if(integer==0)
	{
		aBool=false;
	}
	else
	{
		aBool=true;
	}
	return(true);

}


bool File::TextReadFloat(float& aFloat)
{
	std::string inputBuffer;
	unsigned char input;
	bool found=true;
	do {
		if(fread(&input,1,1,myFile)!=1)
		{
			return(false);
		}
		found=false;
		if(isdigit(input) || (input=='-') || (input=='.'))
		{
			inputBuffer+=input;
			found=true;
		}
		
	} while ((inputBuffer.size()==0) || (found==true));
	aFloat=static_cast<float>(atof(inputBuffer.c_str()));
	return(true);

}

bool File::TextReadDouble(double& aDouble)
{
	std::string inputBuffer;
	unsigned char input;
	bool found=true;
	do {
		if(fread(&input,1,1,myFile)!=1)
		{
			return(false);
		}
		found=false;
		if(isdigit(input) || (input=='-') || (input=='.'))
		{
			inputBuffer+=input;
			found=true;
		}
		
	} while ((inputBuffer.size()==0) || (found==true));
	aDouble=atof(inputBuffer.c_str());
	return(true);
}

bool File::TextReadWord(std::string& aString)
{
	aString.clear();
	unsigned char input;
	bool found=true;
	do {
		if(fread(&input,1,1,myFile)!=1)
		{
			return(false);
		}
		found=false;
		if(isalnum(input) || ispunct(input) || (input=='\\') || (input=='/'))
		{
			aString+=input;
			found=true;
		}
		
	} while ((aString.size()==0) || (found==true));
	return(true);

}

bool File::TextReadString(std::string& aString)
{
	aString.clear();
	unsigned char input;
	do {
		if(fread(&input,1,1,myFile)!=1)
		{
			return(false);
		}
	} while (input!='"');
	do {
		if(fread(&input,1,1,myFile)!=1)
		{
			return(false);
		}
		aString+=input;
	} while (input!='"');
	return(true);
}


bool File::TextWriteText(std::string aString)
{
	if(fwrite(aString.c_str(),1,aString.size(),myFile)!=aString.size())
	{
		return(false);
	}
	return(true);
}

bool File::TextWriteInt(int& aInteger)
{
	std::string text;
	std::ostringstream stream(text);
	stream << aInteger;
	return(TextWriteText(stream.str()));
}

bool File::TextWriteFloat(float& aFloat)
{
	std::string text;
	std::ostringstream stream(text);
	stream << aFloat;
	return(TextWriteText(stream.str()));

}

bool File::TextWriteDouble(double& aDouble)
{
	std::string text;
	std::ostringstream stream(text);
	stream << aDouble;
	return(TextWriteText(stream.str()));
}



bool File::BinaryReadInt(int& aInteger)
{
	if(fread(&aInteger,sizeof(aInteger),1,myFile)!=1)
	{
		return(false);
	}
	return(true);
}

bool File::BinaryReadBool(bool& aBool)
{
	if(fread(&aBool,sizeof(aBool),1,myFile)!=1)
	{
		return(false);
	}
	return(true);
}


bool File::BinaryReadFloat(float& aFloat)
{
	if(fread(&aFloat,sizeof(aFloat),1,myFile)!=1)
	{
		return(false);
	}
	return(true);

}


bool File::BinaryReadDouble(double& aDouble)
{
	if(fread(&aDouble,sizeof(aDouble),1,myFile)!=1)
	{
		return(false);
	}
	return(true);
}

bool File::BinaryReadString(std::string& aString)
{
	aString.clear();
	char input;
	do {
		if(fread(&input,1,1,myFile)!=1)
		{
			return(false);
		}
		aString+=input;
	} while (input!=NULL);
	return(true);
}

bool File::ReadData(void* someData,int aSizeToRead)
{
	assert(myIsBinaryFlag==true);
	int bytesRead=fread(someData,1,aSizeToRead,myFile);
	if(bytesRead!=aSizeToRead)
	{
		return(false);
	}
	return(true);
}


bool File::BinaryWriteInt(int& aInteger)
{
	if(fwrite(&aInteger,sizeof(aInteger),1,myFile)!=1)
	{
		return(false);
	}
	return(true);
}

bool File::BinaryWriteFloat(float& aFloat)
{
	if(fwrite(&aFloat,sizeof(aFloat),1,myFile)!=1)
	{
		return(false);
	}
	return(true);
}

bool File::BinaryWriteDouble(double& aDouble)
{
	if(fwrite(&aDouble,sizeof(aDouble),1,myFile)!=1)
	{
		return(false);
	}
	return(true);
}


bool File::FlushToDisk()
{
	if(fflush(myFile)!=0)
	{
		return(false);
	}
	return(true);
}

bool File::WriteData(void* someData,int aSizeTowrite)
{
	assert(myIsBinaryFlag==true);

	if(fwrite(someData,aSizeTowrite,1,myFile)!=1)
	{
		return(false);
	}
	return(true);
}

bool File::ReadBool(bool& aBool)
{
	if(myIsBinaryFlag==true)
	{
		return(BinaryReadBool(aBool));
	}
	else
	{
		return(TextReadBool(aBool));
	}
}



bool File::ReadInt(int& aInteger)
{
	if(myIsBinaryFlag==true)
	{
		return(BinaryReadInt(aInteger));
	}
	else
	{
		return(TextReadInt(aInteger));
	}
}

bool File::ReadFloat(float& aFloat)
{
	if(myIsBinaryFlag==true)
	{
		return(BinaryReadFloat(aFloat));
	}
	else
	{
		return(TextReadFloat(aFloat));
	}
}

bool File::ReadDouble(double& aDouble)
{
	if(myIsBinaryFlag==true)
	{
		return(BinaryReadDouble(aDouble));
	}
	else
	{
		return(TextReadDouble(aDouble));
	}
}

bool File::ReadWord(std::string& aString)
{
	if(myIsBinaryFlag==true)
	{
		assert(0 && "Not supported for binary files");
		return(false);
	}
	else
	{
		return(TextReadWord(aString));
	}
}

bool File::ReadString(std::string& aString)
{
	if(myIsBinaryFlag==true)
	{
		return(BinaryReadString(aString));
	}
	else
	{
		return(TextReadString(aString));
	}
}


bool File::WriteString(std::string aString)
{
	if(TextWriteText(aString)==false)
	{
		return(false);
	}
	if(myIsBinaryFlag==true)
	{
		if(fwrite(0,1,1,myFile)!=1)
		{
			return(false);
		}
	}
	return(true);
}

bool File::WriteInt(int& aInteger)
{
	if(myIsBinaryFlag==true)
	{
		return(BinaryWriteInt(aInteger));
	}
	else
	{
		return(TextWriteInt(aInteger));
	}
}

bool File::WriteFloat(float& aFloat)
{
	if(myIsBinaryFlag==true)
	{
		return(BinaryWriteFloat(aFloat));
	}
	else
	{
		return(TextWriteFloat(aFloat));
	}
}

bool File::WriteDouble(double& aDouble)
{
	if(myIsBinaryFlag==true)
	{
		return(BinaryWriteDouble(aDouble));
	}
	else
	{
		return(TextWriteDouble(aDouble));
	}
}