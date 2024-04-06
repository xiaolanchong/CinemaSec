#pragma once
#include "cinemadb.h"
#include "DBHelper.h"

#include "OleDBConnection.h"
#include "TimeHelper.h"
#include "StatTable.h"
#include "TimeTable.h"
#include "ExchangeParam.h"
#include "ProtocolTable.h"

class CCinemaOleDB :	//protected	COleDBConnection, 
						public		CCinemaDB,
						protected	virtual StatTable,
						protected	virtual TimeTable,
						protected	virtual ExchangeParam,
						protected	virtual ProtocolTable
{
protected:	

	virtual	bool						GetBackground(int nCameraID, std::vector<BYTE>&		Data)			;	
	virtual	bool						SetBackground(int nCameraID, const std::vector<BYTE>& Data)	;	

	virtual	bool						GetChairs(int nRoomID, std::vector<BYTE>& Data)					;	
	virtual	bool						SetChairs(int nRoomID, const std::vector<BYTE>& Data)				;	

	virtual void						GetWorkParam( int nRoomID,  int & After, int &Before, int& Period)		;
	virtual void						SetWorkParam( int nRoomID,  int  After, int Before, int Period)		;

	virtual void						GetArchiveParam ( int nRoomID, double&  fStart, double &fEnd, int& nDaysKeep ) ;
	virtual void						SetArchiveParam ( int nRoomID, double  fStart, double fEnd, int nDaysKeep ) ;

	virtual	bool						GetAlgoParam(int nRoomID, std::vector<BYTE>& Data)		;	
	virtual	bool						SetAlgoParam(int nRoomID, const std::vector<BYTE>& Data)	;

	virtual	void						GetMergeModel(int nRoomID, std::vector<BYTE>& Data);	
	virtual	void						SetMergeModel(int nRoomID, const std::vector<BYTE>& Data);

	virtual bool						UpdateLayout( const std::map< int, std::pair<int, int > >&  LayoutMap ); 
	// our
	void					ReadCameraTable();
	void					ReadRoomTable();
	void					ReadCinemaTable();

	bool					m_bForceFailed;
	bool					m_bShowWindow;
	std::tstring			m_sSource;
public:
	virtual void			Open()			;
	virtual void			ReadData()		;
	virtual bool			IsConnected() const	;

	bool					CheckConnection() { return COleDBConnection::CheckConnection();}

	DBTableStatFacet&		GetTableStatFacet()			{ return static_cast<StatTable&>(*this);		}
	DBTableTimetableFacet&	GetTableTimetableFacet()	{ return static_cast<TimeTable&>(*this);		}
	DBExchangeFacet&		GetExchangeFacet()			{ return static_cast<ExchangeParam&>(*this);	}
	DBProtocolTable&		GetProtocolTable()			{ return static_cast<ProtocolTable&>(*this);	}

	void	SetSource(LPCTSTR szSource, bool bShowWindow) 
	{ 
		m_sSource = std::tstring( szSource?szSource  : _T("") ); 
		m_bShowWindow = bShowWindow;
	}
	std::tstring GetConnectionString() { return COleDBConnection::GetConnectionString();}

	CCinemaOleDB();
	virtual ~CCinemaOleDB();
};

