#include "SetupInfo.h"
#include "tinyxml.h"

SetupInfo::SetupInfo(void)
{
	myResolutionWidth = 0;
	myResolutionHeight = 0;
	mySessionName = "";
	myApplicationName = "";
}

SetupInfo::SetupInfo( const SetupInfo& someInfo )
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
	TiXmlDocument doc( "settings.xml" );
	bool result = doc.LoadFile();
	assert( result == true && "Could not load settings correctly!");

	TiXmlNode* node = 0;
	TiXmlElement* resolutionElement = 0;
	TiXmlNode* sessionName = 0;
	TiXmlNode* applicationName = 0;

	node = doc.FirstChild( "Settings" );
	resolutionElement = node->FirstChildElement("Resolution");
	sessionName = node->FirstChildElement("SessionName");
	applicationName = node->FirstChildElement("ApplicationName");

	resolutionElement->Attribute("Width", &myResolutionWidth);
	resolutionElement->Attribute("Height", &myResolutionHeight);
	mySessionName = sessionName->FirstChild()->Value();
	myApplicationName = applicationName->FirstChild()->Value();
}
