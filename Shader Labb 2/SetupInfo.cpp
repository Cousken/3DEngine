#include <direct.h>
#include <string.h>

#include "SetupInfo.h"
#include "tinyxml.h"

SetupInfo::SetupInfo(void)
{
	myResolutionWidth = 0;
	myResolutionHeight = 0;
	mySessionName = "";
	myApplicationName = "";
}

SetupInfo::SetupInfo(const SetupInfo& someInfo)
{
	myResolutionHeight = someInfo.myResolutionHeight;
	myResolutionWidth = someInfo.myResolutionWidth;
	mySessionName = someInfo.mySessionName;
	myApplicationName = someInfo.myApplicationName;
}

SetupInfo::~SetupInfo(void)
{
}

void SetupInfo::LoadFromFile()
{
	//DEBUG

	// opening XML file
	TiXmlDocument doc("settings.xml");
	const bool result = doc.LoadFile();

	std::string assertMessage = "Could not load settings correctly! Current working directory: ";
	assertMessage += GetCurrentWorkingDirectory();

	assert(result == true && assertMessage.c_str() );

	TiXmlNode* node = 0;
	TiXmlElement* resolutionElement = 0;
	TiXmlNode* sessionName = 0;
	TiXmlNode* applicationName = 0;

	node = doc.FirstChild("Settings");
	resolutionElement = node->FirstChildElement("Resolution");
	sessionName = node->FirstChildElement("SessionName");
	applicationName = node->FirstChildElement("ApplicationName");

	resolutionElement->Attribute("Width", &myResolutionWidth);
	resolutionElement->Attribute("Height", &myResolutionHeight);
	mySessionName = sessionName->FirstChild()->Value();
	myApplicationName = applicationName->FirstChild()->Value();
}

char* SetupInfo::GetCurrentWorkingDirectory()
{
	char* buffer = nullptr;

	// Get the current working directory:
	if ((buffer = _getcwd(NULL, 0)) == NULL)
		perror("_getcwd error");

	return buffer;
}