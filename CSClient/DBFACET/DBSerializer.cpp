#pragma	  warning( disable : 4995 )		//deprecated functions

#include "stdafx.h"
#include "DBSerializer.h"
#include "../../CSEngine/DBFacet/CinemaOleDB.h"
#include "../../CSEngine/DBFacet/MemoryBackground.h"
#include "CfgDB.h"

//////////// class ////////////////

DBSerializer::DBSerializer(void)
{
}

DBSerializer::~DBSerializer(void)
{
}

void DBSerializer::LoadBackgroundImage( int nCameraID, Arr2f& Background)
{
	std::vector<BYTE> Arr;
	bool res = GetDB().GetTableCameraFacet().GetBackground( nCameraID, Arr );
	if( !res) throw DBLoadImageException("Error occured while loading background"); 
/*	if( Arr.size() < sizeof( BITMAPFILEHEADER ) ) throw DBLoadImageException("Invalid background image");
	const BITMAPINFOHEADER* bih = (const BITMAPINFOHEADER*)&Arr[ sizeof( BITMAPFILEHEADER )] ;
	bih->biWidth ;
	int nSubFrames = bih->biHeight/480;
	Background.resize( nSubFrames );*/
	try
	{
		MemoryBackground::MemoryLoadFloatImage( &Arr[0], Arr.size(), Background, INVERT_AXIS_Y );
	}
	catch(std::exception&)
	{
		throw DBLoadImageException("Error occured while loading background");
	}
}

void DBSerializer::SaveBackgroundImage( int nCameraID, const Arr2f& Background )
{
	std::vector<BYTE> BinImage;
	try
	{
		MemoryBackground::MemorySaveFloatImage( BinImage, Background, INVERT_AXIS_Y  );
		bool res = GetDB().GetTableCameraFacet().SetBackground( nCameraID, BinImage );
		if( !res) throw DBLoadImageException("Error occured while saving background"); 
	}
	catch(std::exception&)
	{
		throw DBSaveImageException("Error occured while saving background");
	}
}

void DBSerializer::LoadChairs( int nRoomID, std::vector<BYTE>& Data)
{
	if( nRoomID < 0 ) throw DBLoadImageException("RoomID not set"); 
	bool res = GetDB().GetTableRoomFacet().GetChairs( nRoomID, Data );
	if(!res)
		throw DBLoadGaugeException( "Error occured while loading gauge" );
}

void DBSerializer::SaveChairs( int nRoomID,const std::vector<BYTE>& Data )
{
	if( nRoomID < 0 ) throw DBLoadImageException("RoomID not set"); 
	bool res = GetDB().GetTableRoomFacet().SetChairs( nRoomID, Data );
	if(!res)
		throw DBLoadGaugeException( "Error occured while loading gauge" );
}

/// stub 

class CCinemaStubDB : public CCinemaDB
{
	virtual	bool						GetBackground(int , std::vector<BYTE>&		){ return false;}			;	
	virtual	bool						SetBackground(int , const std::vector<BYTE>& )	{ return false;};	

	virtual	bool						GetChairs(int , std::vector<BYTE>& )					{ return false;};	
	virtual	bool						SetChairs(int , const std::vector<BYTE>& )				{ return false;};

	virtual	bool						GetAlgoParam(int nRoomID, std::vector<BYTE>& Data)		
	{ 
		UNREFERENCED_PARAMETER(nRoomID);
		UNREFERENCED_PARAMETER(Data);
		return false;
	};	
	virtual	bool						SetAlgoParam(int nRoomID, const std::vector<BYTE>& Data) 
	{ 
		UNREFERENCED_PARAMETER(nRoomID);
		UNREFERENCED_PARAMETER(Data);
		return false;
	}

	virtual	void						GetMergeModel(int nRoomID, std::vector<BYTE>& Data)		
	{ 
		UNREFERENCED_PARAMETER(nRoomID);
		UNREFERENCED_PARAMETER(Data);
	};	
	virtual	void						SetMergeModel(int nRoomID, const std::vector<BYTE>& Data) 
	{ 
		UNREFERENCED_PARAMETER(nRoomID);
		UNREFERENCED_PARAMETER(Data);
	}

	virtual void			Open()		{}	;
	virtual void			ReadData()	{};

	virtual void			GetArchiveParam ( int nRoomID, double&  fStart, double &fEnd, int& nDaysKeep ) 
	{
		UNREFERENCED_PARAMETER(nRoomID);
		UNREFERENCED_PARAMETER(fStart);
		UNREFERENCED_PARAMETER(fEnd);
		UNREFERENCED_PARAMETER(nDaysKeep);
	};
	virtual void			SetArchiveParam ( int nRoomID, double  fStart, double fEnd, int nDaysKeep ) 
	{
		UNREFERENCED_PARAMETER(nRoomID);
		UNREFERENCED_PARAMETER(fStart);
		UNREFERENCED_PARAMETER(fEnd);
		UNREFERENCED_PARAMETER(nDaysKeep);
	};

	virtual bool			IsConnected() const	 { return false;};
};

//////// Singleton

CCinemaDB&	GetDB()
{
	static std::auto_ptr<CCinemaDB> db;
	if( db.get() == 0)
	{
		CCinemaDB* pImpl = 0;
		bool bUseDB = GetCfgDB().UseDB();
		if( bUseDB)		
		{
			CCinemaOleDB* pdb = new CCinemaOleDB(); 
			pImpl = pdb;
			CString sConString = GetCfgDB().GetConnectionString();
			// show link dialog
			pImpl = pdb;
			pdb->SetSource( sConString, false );
			try
			{
				pdb->Open();
				//if( pdb 
				pdb->ReadData();
			}
			catch(CommonException& )
			{
				delete pImpl;
				pImpl = new CCinemaStubDB(); 
			}
		}
		else
		{
			pImpl = new CCinemaStubDB();
		}
		std::auto_ptr<CCinemaDB> Temp(pImpl);
		db = Temp;
	}
	return *db.get();
}
