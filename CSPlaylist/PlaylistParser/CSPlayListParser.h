//////////////////////////////////////////////////////////////////////////
// date  : 3.3.2005
// author: Alexander Boltnev
// e-mail: Alexander.Boltnev@biones.com
//////////////////////////////////////////////////////////////////////////
#pragma once
#include <vector>
#include "PlaylistItem.h"
#include "../../CSInterfaces/IPlayList.h"
#include "../../CSChair/XML/XmlLite.h"

class CSPlayListParser
	:public IPlayList
{
private:
	enum ParseMode
	{
		DoNothing = 0,
		LoadList,
		MarkList,
		UnMarkList,
		TestItemIsProcessed
	};

public:

	size_t size() const
	{
		return playList.size();
	}
	const PlaylistItem& operator [] ( size_t index ) const
	{
		VERIFY(index >= 0 && index < playList.size());
		return playList[index];
	}
	
	CSPlayListParser(void)
		:currentMode(ParseMode::DoNothing)
	{
	
	};

	//marks item at position index as "processed"
	void MarkAsProcessed( size_t index );
	
	//marks item at position index as "not processed"
	void MarkAsNotProcessed( size_t index );
	
	// tests item with index is already processed or not
	bool IsItemProcessed( size_t index );

	
    HRESULT LoadPlayList( LPCWSTR fileName );

	//saves play list to xml
	HRESULT SavePlayList( LPCWSTR fileName );

	void GetResultDirectory( std::wstring& resultPath );
 
	void Release()
	{
		delete this;
	}
	virtual ~CSPlayListParser(void){};
private:
	
	std::vector <PlaylistItem> totalAnswer;
	HRESULT ParseCity(XmlLite::XMLElement& cityElem,        PlaylistItem totalStruct);
	HRESULT ParseCinema(XmlLite::XMLElement& cinemaElem,    PlaylistItem totalStruct);
	HRESULT ParseHall(XmlLite::XMLElement& hallElem,        PlaylistItem totalStruct);
	HRESULT ParseCamera(XmlLite::XMLElement& cameraElem,    PlaylistItem totalStruct);
	HRESULT ParseVideoFile(XmlLite::XMLElement& cameraElem, PlaylistItem totalStruct);
	//parses XML document and returns the array of 'CSVideoFileStructure' to be processed
	//returns true if successful
	HRESULT ParseCSPlayList(const XmlLite::XMLDocument& myDoc);
	std::vector <PlaylistItem> playList;
    ParseMode currentMode;
	size_t markAsProcessedIndex;
	size_t testIsProcessedIndex;
	bool itemIsProcessedAnswer;
	XmlLite::XMLDocument innerPlayListDoc;
	std::wstring resultPath;
	
};
