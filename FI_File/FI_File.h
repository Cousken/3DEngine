#ifndef FI_FILE_HEADER
#define FI_FILE_HEADER

#include <string>
#include <stdio.h>
#include <bitset>

namespace FI_File
{
	enum FileFlags
	{
		READ,
		WRITE,
		APPEND,
		BINARY
	};

	class File
	{
	public:
		File();
		~File();

		bool Open(std::string aFileName, std::bitset<8> aSetOfFlags=0xF);
		bool Close();


		bool GoToNewLine();
		
		bool ReadInt(int& aInteger);
		bool ReadBool(bool& aBool);
		bool ReadFloat(float& aFloat);
		bool ReadDouble(double& aDouble);
		bool ReadWord(std::string& aString);
		bool ReadString(std::string& aString);

		
		bool WriteString(std::string aString);
		bool WriteInt(int& aInteger);
		bool WriteFloat(float& aFloat);
		bool WriteDouble(double& aDouble);

		template <class Type>
		bool Write(Type& someData);
		template <class Type>
		bool Read(Type& someData);
		template <class Type>
		bool WriteArray(Type* someData,int aLength);
		template <class Type>
		bool ReadArray(Type* someData,int aLength);


		bool WriteData(void* someData,int aSizeToRead);
		bool ReadData(void* someData,int aSizeToRead);
		bool FlushToDisk();
		int GetFileSize();
	private:

			bool TextReadBool(bool& aBool);
			bool TextReadInt(int& aInteger);
			bool TextReadFloat(float& aFloat);
			bool TextReadDouble(double& aDouble);
			bool TextReadWord(std::string& aString);
			bool TextReadString(std::string& aString);

			bool TextWriteText(std::string aString);
			bool TextWriteInt(int& aInteger);
			bool TextWriteFloat(float& aFloat);
			bool TextWriteDouble(double& aDouble);

			bool BinaryReadBool(bool& aBool);
			bool BinaryReadInt(int& aInteger);
			bool BinaryReadFloat(float& aFloat);
			bool BinaryReadDouble(double& aDouble);
			bool BinaryReadString(std::string& aString);
			

			bool BinaryWriteInt(int& aInteger);
			bool BinaryWriteFloat(float& aFloat);
			bool BinaryWriteDouble(double& aDouble);
	
			


		FILE* myFile;
		bool myIsBinaryFlag;
		
	};



	template <class Type>
	bool File::Write(Type& someData)
	{
		if(fwrite(&someData,sizeof(someData),1,myFile)!=1)
		{
			return(false);
		}
		return(true);
	}

	template <class Type>
	bool File::Read(Type& someData)
	{
		if(fread(&someData,sizeof(someData),1,myFile)!=1)
		{
			return(false);
		}
		return(true);

	}
	
	template <class Type>
	bool File::WriteArray(Type* someData,int aLength)
	{
		if(fwrite(&someData,sizeof(someData),aLength,myFile)!=aLength)
		{
			return(false);
		}
		return(true);

	}

	template <class Type>
	bool File::ReadArray(Type* someData,int aLength)
	{
		if(fread(&someData,sizeof(someData),aLength,myFile)!=aLength)
		{
			return(false);
		}
		return(true);
	}



};

namespace FI = FI_File;



#endif