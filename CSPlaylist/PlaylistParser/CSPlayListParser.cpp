#include "StdAfx.h"
#include "CSPlayListParser.h"
// errors of the parser
#define INCOMPATIPLE_PLAYLIST_VERSION_ERR 1

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

HRESULT CSPlayListParser::ParseCSPlayList(const XmlLite::XMLDocument& myDoc)
{
	//we start with the root element
	XmlLite::XMLElement rootElem( myDoc.GetRoot() );

	// check version 
	std::wstring version;
	rootElem.GetAttr(_T("version"), version);
	if (version != _T("1"))
	{
		return INCOMPATIPLE_PLAYLIST_VERSION_ERR;
	}

	rootElem.GetAttr(_T("resultPath"), resultPath);

	//and go 
	for ( XmlLite::XMLElement::iterator iter = rootElem.begin(); iter!= rootElem.end(); ++iter )
	{		
		PlaylistItem totalStruct;
		XmlLite::XMLElement e = *iter;
		ParseCity(e, totalStruct);	
	}
	return S_OK;
}

HRESULT CSPlayListParser::ParseCity(XmlLite::XMLElement& cityElem, PlaylistItem totalStruct)
{
	std::tstring cityName;
	cityElem.GetAttr(_T("name"), cityName);

	// add city name and address to all fields
	totalStruct.video = totalStruct.video + cityName;
	totalStruct.chairs = totalStruct.chairs  + cityName;
	totalStruct.marking = totalStruct.marking + cityName;
	totalStruct.background = totalStruct.background + cityName;

	// iter means 'cinema'
	for ( XmlLite::XMLElement::iterator iter = cityElem.begin(); iter!= cityElem.end(); ++iter )
	{
		XmlLite::XMLElement e = *iter;
		ParseCinema(e, totalStruct);
	}
	return S_OK;
}

HRESULT CSPlayListParser::ParseCinema(XmlLite::XMLElement& cinemaElem, PlaylistItem totalStruct)
{
	std::tstring address;
	cinemaElem.GetAttr(_T("address"), address);
	// add city name and address to all fields
	totalStruct.video = totalStruct.video + _T("_") + address;
	totalStruct.chairs = totalStruct.chairs + _T("_") + address;
	totalStruct.marking = totalStruct.marking + _T("_") + address;
	totalStruct.background = totalStruct.background + _T("_") + address;

	// iter means 'hall'
	for ( XmlLite::XMLElement::iterator iter = cinemaElem.begin(); iter!= cinemaElem.end(); ++iter )
	{
		XmlLite::XMLElement e = *iter;
		ParseHall(e, totalStruct);
	}

	return S_OK;
}



HRESULT CSPlayListParser::ParseHall(XmlLite::XMLElement& hallElem, PlaylistItem totalStruct)
{
	std::tstring hallNumber;
	
	hallElem.GetAttr(_T("number"), hallNumber);

	totalStruct.video = totalStruct.video + _T("_hall") + hallNumber;
	totalStruct.background = totalStruct.background + _T("_hall") + hallNumber;
	totalStruct.chairs = totalStruct.chairs + _T("_hall") + hallNumber; // + _T("_chairs.xml");
	totalStruct.marking = totalStruct.marking + _T("_hall") + hallNumber;
	//iter here means 'camera'
	for ( XmlLite::XMLElement::iterator iter = hallElem.begin(); iter!= hallElem.end(); ++iter )
	{
		XmlLite::XMLElement e = *iter;
		ParseCamera(e, totalStruct);
	}

	return S_OK;
}

HRESULT CSPlayListParser::ParseCamera(XmlLite::XMLElement& cameraElem, PlaylistItem totalStruct)
{
	std::tstring camID;
	cameraElem.GetAttr(_T("id"), camID);

	std::tstring camUID;
	cameraElem.GetAttr(_T("UID"), camUID);

	totalStruct.cameraUID = camUID;
	
	totalStruct.chairs = totalStruct.chairs + _T("_cam") + camID + _T("_chairs.xml");
	totalStruct.video = totalStruct.video + _T("_cam") + camID;
	totalStruct.background = totalStruct.background + _T("_cam") + camID + _T("_background.bmp");
	totalStruct.marking = totalStruct.marking + _T("_cam") + camID;
	//iter here means 'video file'
	for ( XmlLite::XMLElement::iterator iter = cameraElem.begin(); iter!= cameraElem.end(); ++iter )
	{
		XmlLite::XMLElement e = *iter;
		ParseVideoFile(e, totalStruct);
	}

	return S_OK;
}
HRESULT CSPlayListParser::ParseVideoFile(XmlLite::XMLElement& cameraElem, PlaylistItem totalStruct)
{
	std::tstring date;
	std::tstring time;
	std::tstring path;

	cameraElem.GetAttr(_T("date"), date);
	cameraElem.GetAttr(_T("time"), time);
	cameraElem.GetAttr(_T("path"), path);

	totalStruct.path = path;   
	totalStruct.video = totalStruct.video      + _T("_") + date + _T("_") + time + _T("_video.avi");
	totalStruct.marking = totalStruct.marking    + _T("_") + date + _T("_") + time + _T("_marking.xml");


	if (currentMode == ParseMode::LoadList)
	{
		playList.push_back(totalStruct);
	}
	if (currentMode == ParseMode::MarkList)
	{
		//	OutputDebugString((playList[markAsProcessedIndex]).marking.c_str());
	
		if ( playList.size() > 0   &&   
			 totalStruct.video == playList[markAsProcessedIndex].video &&
			 totalStruct.marking == playList[markAsProcessedIndex].marking &&
			 totalStruct.chairs == playList[markAsProcessedIndex].chairs &&
			 totalStruct.path == playList[markAsProcessedIndex].path)
		{
			cameraElem << XmlLite::XMLAttribute(cameraElem, _T("processed"), _T("yes"));
		}
	}

	if (currentMode == ParseMode::UnMarkList)
	{
		//	OutputDebugString((playList[markAsProcessedIndex]).marking.c_str());

		if ( playList.size() > 0   &&   
			totalStruct.video == playList[markAsProcessedIndex].video &&
			totalStruct.marking == playList[markAsProcessedIndex].marking &&
			totalStruct.chairs == playList[markAsProcessedIndex].chairs &&
			totalStruct.path == playList[markAsProcessedIndex].path)
		{
			cameraElem << XmlLite::XMLAttribute(cameraElem, _T("processed"), _T("no"));
		}
	}


	if (currentMode == TestItemIsProcessed)
	{
		if ( playList.size() > 0   &&   
			totalStruct.video == playList[testIsProcessedIndex].video &&
			totalStruct.marking == playList[testIsProcessedIndex].marking &&
			totalStruct.chairs == playList[testIsProcessedIndex].chairs &&
			totalStruct.path == playList[testIsProcessedIndex].path)
		{
			std::tstring procAnwer;
			cameraElem.GetAttr(_T("processed"), procAnwer);
			if (procAnwer == _T("yes"))
				itemIsProcessedAnswer = true;
			else
			    itemIsProcessedAnswer = false;
		}
	}
	return S_OK;
}

//marks item at position index as "processed"
void CSPlayListParser::MarkAsProcessed( size_t index )
{
	markAsProcessedIndex = index;
	currentMode = ParseMode::MarkList;
	ParseCSPlayList(innerPlayListDoc);
	currentMode = ParseMode::DoNothing;
}

void CSPlayListParser::MarkAsNotProcessed( size_t index )
{
	markAsProcessedIndex = index;
	currentMode = ParseMode::UnMarkList;
	ParseCSPlayList(innerPlayListDoc);
	currentMode = ParseMode::DoNothing;
}


// realization of the interface
HRESULT CSPlayListParser::LoadPlayList( LPCWSTR fileName )
{
	VARIANT_BOOL b;
	innerPlayListDoc.m_pDoc->load( CComVariant(fileName), &b );
    if (b != VARIANT_TRUE) return S_FALSE;
    currentMode = ParseMode::LoadList;
    playList.clear();
	try
	{
		ParseCSPlayList(innerPlayListDoc);
	}
	catch(...)
	{
		return S_FALSE;
	}
	currentMode = ParseMode::DoNothing;
    return S_OK;
}

//saves play list to xml
HRESULT CSPlayListParser::SavePlayList(LPCWSTR fileName )
{
	innerPlayListDoc.m_pDoc->save( CComVariant(fileName) );
	return S_OK;
}

bool CSPlayListParser::IsItemProcessed( size_t index )
{
	currentMode = TestItemIsProcessed;
	testIsProcessedIndex = index;
	ParseCSPlayList(innerPlayListDoc);
	currentMode = DoNothing;
	return itemIsProcessedAnswer;
}
void CSPlayListParser::GetResultDirectory(std::wstring& resultPath)
{
	resultPath = this->resultPath;
}
