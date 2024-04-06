//========================= Copyright � 2004, Elvees ==========================
// Author: Eugene V. Gorbachev
// Purpose: CinemaSec database table rows
//
//=============================================================================

#ifndef DBROWS_H
#define DBROWS_H
#ifdef _WIN32
#pragma once
#endif

// Cinema table row

#include "TimeHelper.h"
#include "../../CSChair/common/tstring.h"

MACRO_EXCEPTION(DataBaseException,		CommonException)
MACRO_EXCEPTION(DBConnectionException,	DataBaseException)
MACRO_EXCEPTION(TableException,			DataBaseException)

//! ����� ���������� ��� ������ � ��. ����� ������ ������� �� ����������
template<class T, const int> class BaseRowException : public T 
{
//	DWORD		m_dwError;
	size_t		m_nRecord;
public:
//	DWORD	GetError()	const			{ return m_dwError; }
	size_t	GetRecordNumber()	const	{ return m_nRecord;	}

	BaseRowException( size_t Row):	T("Table exception"),  m_nRecord(Row){}  
	BaseRowException(const BaseRowException& ex):	T(ex),  m_nRecord(ex.m_nRecord)	{} 
};

//! ������ ��� �������� ������� ���������
typedef BaseRowException<TableException, 0>	InvalidIntervalException;
//! ������ ��� ����������� ���������
typedef BaseRowException<TableException, 1>	IntervalIntersectionException;

//! ������ � ���������� � ��
struct CinemaRow
{
	//! ������������� ����������
	int			m_nID;
	//! ��������
	std::tstring		m_sName;
	//! �����������
	std::tstring		m_sComment;

	CinemaRow(int nID, const std::tstring& sName, const std::tstring& sComment):
		m_nID		(nID),
		m_sName	(sName),
		m_sComment(sComment){}
	CinemaRow(int nID):
		m_nID		(nID),
		m_sName	(),
		m_sComment(){}	
	CinemaRow(const CinemaRow& cr):
	m_nID		(cr.m_nID),
	m_sName	(cr.m_sName),
	m_sComment(cr.m_sComment){}
	bool operator < ( const CinemaRow& cr) const 
	{
		return m_nID < cr.m_nID;
	}
};

//! Room table row
struct RoomRow
{
	//! ������������� ��
	int			m_nID;
	//! ������������� ����������, ��� ���
	int			m_nCinemaID;
	//! ����� ����
	int			m_nNumber;
	//! ����� ����������� ����
	std::tstring		m_sIP;

	RoomRow(int nID, int nCinemaID, int nNumber, const std::tstring& sIP):
		m_nID		(nID),
		m_nCinemaID	(nCinemaID),
		m_nNumber	(nNumber),
		m_sIP		(sIP){}

	RoomRow(int nID) : m_nID(nID){}
	
	RoomRow(const RoomRow& rr):
	m_nID		(rr.m_nID),
	m_nCinemaID	(rr.m_nCinemaID),
	m_nNumber	(rr.m_nNumber),
	m_sIP		(rr.m_sIP){}

	bool operator < ( const RoomRow& cr) const 
	{
		return m_nID < cr.m_nID;
	}
};

//! Camera table row
struct CameraRow
{
	//! ������������� ������
	int						m_nID;
	//! ������������ ����
	int						m_nRoomID;
	//! ������� � 3� ������ �������, (0,0) - ������� �����
	std::pair<int,int>		m_Pos;
	//! GUID ������ ��� chcsva
	std::tstring					m_sGUID;
	//! ����� �������, ��� ��������� ������
	std::tstring					m_sIP;

	//CameraRow(){}
	CameraRow(int nID, int nRoomID, std::pair<int,int> Pos, 
		const std::tstring& sGuid, const std::tstring& sIP):
		m_nID		(nID),
		m_nRoomID	(nRoomID),
		m_Pos		(Pos),
		m_sGUID	(sGuid),	
		m_sIP	(sIP){}	

	CameraRow(const CameraRow& cr):
	m_nID		(cr.m_nID),
	m_nRoomID	(cr.m_nRoomID),
	m_Pos		(cr.m_Pos),
	m_sGUID		(cr.m_sGUID),
	m_sIP		(cr.m_sIP){}

	CameraRow(int nID) : m_nID(nID){}

	bool operator < ( const CameraRow& cr) const 
	{
		return m_nID < cr.m_nID;
	}
};

//! ������� ������
struct DBTableCameraFacet
{
	//! ������� ��� ������ � �������
	//! \return ��� ������
	virtual const std::set<CameraRow>&	GetCameras (  ) const							= 0;
	
	//! ������� ������ �� ����
	//! \param nRoomID ������������� ����
	//! \return 
	virtual std::set<int>				SelectCamerasFromRoom(int nRoomID)		const	= 0;

	//! �������� GUID ������
	//! \param nCameraID �������������
	//! \return ��������� ������������� GUID ��� ��� �����
	virtual std::tstring				GetCameraSource( int nCameraID ) const			= 0;

	//! �������� ������ ��� ������
	//! \param nCameraID ������������� ������
	//! \param Data ������ �����������
	//! \return �����/������
	virtual	bool						GetBackground(int nCameraID, std::vector<BYTE>& Data)		= 0;	

	//! ��������� � �� ������ ���
	//! \param nCameraID ������������ ������, � ������� �������� ���
	//! \param Data ������ �����������
	//! \return �����/������
	virtual	bool						SetBackground(int nCameraID, const std::vector<BYTE>& Data)	= 0;

	//! �������� ������������ �����
	//! \param LayoutMap ����� ������� ������������ - ������� � ������������� 2D �������
	//! \return �����/������
	virtual bool						UpdateLayout( const std::map< int, std::pair<int, int > >&  LayoutMap ) = 0;
};

typedef std::map<int, std::tstring >	NameMap_t;

//! ��������� ������� � ������� �����
struct DBTableRoomFacet
{
	//! ����� ��� ����
	//! \return ��������� �������
	virtual const std::set<RoomRow>&	GetRooms (  )							const = 0;

	//! ������� ���� �� ������������� ����������
	//! \param nCinemaID ������������� ����������
	//! \return ��-�� ��������������� �����
	virtual std::set<int>				SelectRoomsFromCinema(int nCinemaID)	const = 0;	

	//! �������� ��� ��������� � ����� ����
	//! \param nRoomID ������������� ����
	//! \param sCinemaName ��� ����������, ��� ��������� ���
	//! \param nRoomNo ����� ����
	//! \return �����/������
	virtual bool						GetRoomInfo(int nRoomID, CString& sCinemaName, int& nRoomNo ) = 0;

	//! �������� ������ ���� 
	//! \param nRoomID ������������� ����
	//! \param Data �������� ������ (���� ������)
	//! \return �����/������
	virtual	bool						GetChairs(int nRoomID, std::vector<BYTE>& Data)					= 0;	

	//! �������� ���� ������ � ��
	//! \param nRoomID ������������� ����
	//! \param Data ������ �����
	//! \return �����/������
	virtual	bool						SetChairs(int nRoomID, const std::vector<BYTE>& Data)			= 0;

	//! �������� ��� ������ �����
	//! \return ����� �������������-����� ����
	virtual std::map<int,int>			GetRoomNumbers() const = 0;

	//! �������� ��������� �������� ��������
	//! \param nRoomID ������������ ����
	//! \param After ���-�� ������ ����� ������ ������
	//! \param Before ���-�� ������ �� ����� ������
	//! \param Period ������ (���) ������� ����������
	virtual void						GetWorkParam( int nRoomID, int & After, int &Before, int& Period)		= 0 ;

	//! ��������� ��������� �������� ��������
	//! \param nRoomID ������������ ����
	//! \param After ���-�� ������ ����� ������ ������
	//! \param Before ���-�� ������ �� ����� ������
	//! \param Period ������ (���) ������� ����������
	virtual void						SetWorkParam( int nRoomID, int  After, int Before, int Period)		= 0;

	//! �������� ��������� ������ ��� ����
	//! \param nRoomID ������������� ����
	//! \param fStart ������� ������ ������ �������� ������
	//! \param fEnd ������� ����� ������ �������� ������
	//! \param nDaysKeep ���-�� ���� �������� ������
	virtual void						GetArchiveParam ( int nRoomID, double&  fStart, double &fEnd, int& nDaysKeep ) = 0;

	//! �������� ��������� ������ ��� ����
	//! \param nRoomID ������������� ����
	//! \param fStart ������� ������ ������ �������� ������
	//! \param fEnd ������� ����� ������ �������� ������
	//! \param nDaysKeep ���-�� ���� �������� ������
	virtual void						SetArchiveParam ( int nRoomID, double  fStart, double fEnd, int nDaysKeep ) = 0;

	//! �������� ��������� ���������, �� ������ ������ �� ������������
	//! \param nRoomID ������������� ����
	//! \param Data �������� ������
	//! \return �����/������
	virtual	bool						GetAlgoParam(int nRoomID, std::vector<BYTE>& Data)	= 0	;	

	//! ���������� ��������� ���������, �� ������ ������ �� ������������
	//! \param nRoomID ������������� ����
	//! \param Data �������� ������
	//! \return �����/������
	virtual	bool						SetAlgoParam(int nRoomID, const std::vector<BYTE>& Data) = 0	;

	//! �������� ������ ������� ����
	//! \param nRoomID ������������� ����
	//! \param Data �������� ������
	virtual	void						GetMergeModel(int nRoomID, std::vector<BYTE>& Data)	= 0	;	

	//! ��������� ������ ������� ����
	//! \param nRoomID ������������� ����
	//! \param Data �������� ������
	virtual	void						SetMergeModel(int nRoomID, const std::vector<BYTE>& Data) = 0	;
};

//! �������������� �����
typedef std::set<int>					CameraSet_t;

//! ������ � �����
typedef std::map< int , CameraSet_t > 	RoomSet_t;

//! ���� � �����������
typedef std::map< int , RoomSet_t	> 	CinemaSet_t;

//! ������� �����������
struct DBTableCinemaFacet
{
	//! �������� ��� ����������
	//! \return ��� ������
	virtual const std::set<CinemaRow>&	GetCinemas (  ) const = 0;

	//! ������� �� �����������
	//! \param Cinemas ���� � �����������
	virtual void GetCinemaSet( CinemaSet_t& Cinemas ) const = 0;

	//! �������� ��� �������� �����������
	//! \return ����� ID - ��������
	virtual NameMap_t					GetCinemaNames() const = 0;
};

//! ������ ����������
struct StatRow
{
	//! ������������� ������
	int			m_nID;
	//! ������������� ����, ��� ��� �������
	int			m_nRoomID;
	//! ����� �������
	CTime		m_Time;
	//! ���-�� ��������
	int			m_nSpectators;

	bool operator < ( const StatRow& sr) const 
	{
		return m_nID < sr.m_nID;
	}

	StatRow(int nID, int nRoomID, const CTime& Time, int nSpectators) :
	m_nID(nID),
		m_nRoomID(nRoomID),
		m_Time(Time),
		m_nSpectators(nSpectators)
	{
	}
	StatRow(int nID) :
	m_nID(nID)
	{
	}

};

typedef std::set<StatRow>							StatRowSet_t;
//! ���������� �� ����
typedef std::map<int, const std::vector<BYTE> * >	PhotoMap_t;
//! ���� ������������� ������ - ���-�� ��������
typedef std::pair< int, int >						FilmStat_t;

//! ������� ����������
struct DBTableStatFacet
{
	//! �������� ����������
	//! \param nRoomID ������������ ����
	//! \param Time ����� �������
	//! \param nSpectators ���-�� ��������
	//! \param BigImage ����� ������������� ������ - ����������� � ���
	virtual void						SetStatisticsRow(int nRoomID, 
		const DBTIMESTAMP& Time, int nSpectators,
		const std::map< int, std::vector<BYTE> >& BigImage ) = 0;

	//! �������� ���������� � �������
	//! \param nStatID ����� �������
	//! \return ��� ���������� �� �������
	virtual PhotoMap_t					GetImages(int nStatID)	= 0;

	//! ������� ���������� �� ������
	//! \param timeBegin ������
	//! \param timeEnd �����
	//! \param nRoomID ������������� ����
	//! \param Stats ��-�� �������
	virtual void	GetStatForPeriod( CTime timeBegin, CTime timeEnd, int nRoomID, StatRowSet_t& Stats )	= 0;

	//! ������� ���������� �� ������ ��� ������ �� �����
	//! \param timeBegin ������
	//! \param timeEnd �����
	//! \param Rooms ����, �� ������� ���������� �����
	//! \param Stats ��-�� �������
	virtual void	GetStatForPeriodReport(	CTime timeBegin, CTime timeEnd, 
											const std::vector<int>& Rooms, StatRowSet_t& Stats )	= 0;

	//! �������� ���-� � �������
	//! \param nStatID ������������� �������
	//! \param nRoomID ������������� ����
	//! \param timeStat �����
	//! \param nSpectators ���-�� ��������
	virtual void	GetStatInfo( int nStatID, int& nRoomID, CTime& timeStat, int& nSpectators ) = 0;

	//! ������ ���������� ��� ���������� ������ - ������ ���� ������� � ���-�� �������� �� ��� �� ������
	//! \param nRoomID ������������� ���� 
	//! \param timeBegin ������
	//! \param timeEnd �����
	//! \param StatArr ������ ������� � ���-�� ��������
	//! \param nStatID ������������� ��� ��������� ������
	virtual void	GetStatForExchange( int nRoomID, CTime timeBegin, CTime timeEnd, std::vector< FilmStat_t >& StatArr,int &nStatID ) = 0;
};

MACRO_EXCEPTION( NoSuchCinemaOrRoom,	TableException );
MACRO_EXCEPTION( NoSuchExchangeID,		TableException );

//! ������� ����������
struct DBTableTimetableFacet
{
	//! ���������� ����������, ��� ���������� ������ �� ������ ���������
	//! \param nRoomID ������������� ����
	//! \param Range ������-����� ������� �������� ����������
	//! \param Timetable ����������
	//! \param Names ������ �������� �������, ������ == ������� Timetable
	virtual void			SetTimetable(	int nRoomID, const FilmTime& Range,const TimeTable_t& Timetable, 
											const std::vector<std::wstring>& Names ) 	= 0;
	//! ��������� ����������
	//! \param nRoomID ������������� ����
	//! \param Range ������-����� ������� �������� ����������
	//! \param Timetable ����������
	//! \param Names ������ �������� �������, ������ == ������� Timetable
	virtual void			GetTimetable( int nRoomID, const FilmTime& Range, TimeTable_t& Timetable,
											std::vector<std::wstring>& Names) 	= 0;

	//! ����� ������� ��� ��������� �����
	//! \param nRoomID ������������� ����
	//! \param Time ����� ������
	//! \return �����/������
	virtual bool			GetCurrentOrNextFilmTime( int nRoomID, FilmTime& Time) = 0;

	//! ����� �������  �����
	//! \param nRoomID ������������� ����
	//! \param Name �������� ������ 
	//! \return �����/������
	virtual bool			GetCurrentFilmTime( int nRoomID, FilmTime& Time, std::wstring& Name) = 0;

	//! �������� ���������� � ������ ����� �������������
	//! \param nFilmID ������������� ������
	//! \param Time ����� ������
	//! \param Name �������� ������
	//! \return �����/������
	virtual bool			GetFilmFromId( int nFilmID, FilmTime& Time, std::wstring& Name   ) = 0; 

	//! 
	//! \param sCinemaName ��� ����������
	//! \param nRoomNo ����� ����
	//! \param ExchangeID �������������� ������ �����������
	//! \param timeBegin �����
	//! \param timeEnd 
	virtual void			CheckExistance( const std::wstring& sCinemaName, int nRoomNo, 
											const std::vector<int>& ExchangeID, 
											CTime timeBegin, CTime timeEnd ) = 0; // throws NoSuchExchangeID
	//! upper function for CSClient & user input when all films in the period will be erased
	//! this function is much smarter and will be used by the CSExchange
	virtual void			SetTimetableSmart(	const std::wstring& sCinemaName,
												int nRoomNo,
												const FilmTime& Range,
												const TimeTable_t& Timetable, 
												const std::vector<std::wstring>& Names,
												const std::vector<int>& ExchangeID ) 	= 0; // throws IntervalIntersectionException

	//! ����� ����� ����������� �����
	//! \param nRoomID ������������� ����
	//! \param timeEnd ����� �����
	//! \return �����/������
	virtual bool			GetPreviousCinemaFilm( int nRoomID, CTime& timeEnd ) = 0;
};

//! ��� ���������� ������
struct DBExchangeFacet
{
	//! ���������� ��������� ������
	//! \param nRoomID ������������� ����
	//! \param nTimeoutSec ������� ��������� (���)
	//! \param nTimeoutNumber ���-�� ���������
	//! \param nWaitTimeout �������� ������ �� ��������� (���)
	//! \param nStatTreshold ������ (���) ����� ������ ������ � ������ ��������� � ����������
	virtual void	SetExchangeParam( int nRoomID, int nTimeoutSec, int nTimeoutNumber, int nWaitTimeout, int nStatTreshold ) = 0;

	//! �������� ��������� ������
	//! \param nRoomID ������������� ����
	//! \param nTimeoutSec ������� ��������� (���)
	//! \param nTimeoutNumber ���-�� ���������
	//! \param nWaitTimeout �������� ������ �� ��������� (���)
	//! \param nStatTreshold ������ (���) ����� ������ ������ � ������ ��������� � ����������
	virtual void	GetExchangeParam( int nRoomID, int& nTimeoutSec, int& nTimeoutNumber, int& nWaitTimeout, int& nStatTreshold ) = 0;
};

//! ������� ��������� ������
struct DBProtocolTable
{
	//! ��� ���������
	enum MsgType
	{
		//! ����������� �� BoxOffice
		BO_Unknown		= 1,
		//! ��������� ����������
		BO_Timetable	= 2,
		//! ����� �� BoxOffice
		BO_Response		= 3,
		//! ��������� ����������
		CCS_Statistics	= 50,
		//! ����� �� ����������
		CCS_Response	= 51
	};

	//! ��� ���������, ����� ���������, ����� ���������
	typedef boost::tuple< MsgType, CTime, std::wstring >	Message_t;

	//! �������� ��������� � �������
	//! \param nCinemaID ������������� ����������
	//! \param mt ��� ���������
	//! \param timeMsg ����� ���������
	//! \param sText ����� ���������
	virtual void	AddMessage( int nCinemaID, MsgType mt, CTime timeMsg, const std::wstring& sText ) = 0;

	//! ������� ��������� �� ������
	//! \param nCinemaID ������������� ����������
	//! \param timeFrom ������ 
	//! \param timeTo �����
	//! \param Msgs ���������
	virtual void	GetMessage( int nCinemaID, CTime timeFrom, CTime timeTo, std::vector<Message_t> & Msgs ) = 0;
};

#endif // DBROWS_H
