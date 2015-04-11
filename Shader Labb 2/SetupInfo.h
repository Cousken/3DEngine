#ifndef _SETUP_INFO_H_
#define _SETUP_INFO_H_

#include <string>
#include <windows.h>

class SetupInfo
{
public:
	SetupInfo(void);
	SetupInfo(const SetupInfo& someInfo);
	~SetupInfo(void);
	
	void LoadFromFile();

	std::string myApplicationName;
	std::string mySessionName;
	int myResolutionWidth;
	int myResolutionHeight;

private:
	
	
};

#endif
