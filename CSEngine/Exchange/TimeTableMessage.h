//======================================================================================//
//                                      CinemaSec                                       //
//                                                                                      //
//                              Copyright by ElVEES, 2005                               //
//                                                                                      //
//--------------------------------------------------------------------------------------//
// Description:
//	Timetable input message from BoxOffice
//
//--------------------------------------------------------------------------------------//
// Author : Eugene Gorbachev (Eugene.Gorbachev@biones.com)
// Date   : 07.06.2005
//                                                                                      //
//======================================================================================//
#ifndef _TIME_TABLE_MESSAGE_3474067564806355_
#define _TIME_TABLE_MESSAGE_3474067564806355_

#include "InputMessage.h"
//======================================================================================//
//                                class BO_TimeTable                                //
//======================================================================================//

class BO_TimeTable : public InputMessage
{
public:
	struct Item
	{
		int								m_FilmID;
		boost::optional<std::wstring>	m_Name;
		boost::optional<CTime>			m_Begin, m_End;

		Item(	int nFilmID, 
			const boost::optional<CTime>& Begin, 
			const boost::optional<CTime>& End,
			const boost::optional<std::wstring>& Name):
		m_FilmID( nFilmID ),
			m_Begin(Begin), m_End( End ), m_Name(Name){}

		bool	IsNoData( ) const { return !m_Name && !m_Begin && !m_End; };
	};
	typedef std::map<int, std::vector< Item > >	FilmMap_t;
private:
	std::wstring							m_CinemaName;
	FilmMap_t								m_Films;
	CTime									m_timeBegin, m_timeEnd;
	int										m_nID;
public:
	BO_TimeTable( 
		int	nID,
		const std::wstring& sCinemaName, 
		CTime timeBegin, CTime timeEnd,
		const FilmMap_t& Films ):
	m_CinemaName( sCinemaName ),
		m_timeBegin( timeBegin ),
		m_timeEnd( timeEnd ),
		m_nID( nID ),
		m_Films( Films ) {}

	void	CheckExistance( CCinemaOleDB& db, MyDebugOutputImpl& Debug );
	void	Update( CCinemaOleDB& db, MyDebugOutputImpl& Debug );
public:
	std::wstring				GetCinemaName() const	{ return m_CinemaName;	}
	const FilmMap_t&			GetFilms() const		{ return m_Films;		}
	CTime						GetBegin() const		{ return m_timeBegin;	}
	CTime						GetEnd() const			{ return m_timeEnd;		}
	int							GetID() const			{ return m_nID;		}
	virtual void				Dump(MyDebugOutputImpl& Debug) const;
	virtual void				Validate() const ;
	virtual boost::shared_ptr<OutputMessage>				Execute( CCinemaOleDB& db, MyDebugOutputImpl& Debug );
	virtual DBProtocolTable::MsgType			GetDbType() const { return DBProtocolTable::BO_Timetable;}
};

#endif // _TIME_TABLE_MESSAGE_3474067564806355_