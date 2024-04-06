//======================================================================================//
//                                      CinemaSec                                       //
//                                                                                      //
//                              Copyright by ElVEES, 2005                               //
//                                                                                      //
//--------------------------------------------------------------------------------------//
// Description:
//	Query exception occuries while processing & validate input query
//
//--------------------------------------------------------------------------------------//
// Author : Eugene Gorbachev (Eugene.Gorbachev@biones.com)
// Date   : 03.06.2005
//                                                                                      //
//======================================================================================//
#ifndef _QUERY_EXCEPTION_8545325443475501_
#define _QUERY_EXCEPTION_8545325443475501_
//======================================================================================//
//                                 class QueryException                                 //
//======================================================================================//

const int c_resOk				= 0;
const int c_resInvalidQuery		= 10;
const int c_resInvalidVersion	= 11;
const int c_resTimeout			= 12;
const int c_resInvalidType		= 13;
const int c_resNoData			= 14;
const int c_resNoFilm			= 15;
const int c_resFilmAlreadyExists= 16;
const int c_resNoCinema			= 17;
const int c_resInvalidDateTime	= 18;
const int c_resUnknown			= 65535;

const int c_DefErrorID			= 0;

#if 0
class QueryException : public std::exception
{
	int						m_ID;		
	std::wstring			m_sDesc;	
public:
	virtual const char* what() const	{ return "QueryException exception";}
	int					GetID() const	{ return m_ID;		}
	std::wstring		GetDesc() const	{ return m_sDesc;	}

	QueryException(const std::wstring& sDesc, int nID = c_DefErrorID) : m_ID(nID), m_sDesc(sDesc){}
};

#define QUERY_EXCEPTION(child, base) class child : public base \
{ \
public: \
	child(const std::wstring& sDesc) : base(sDesc){} \
	child(const std::wstring& sDesc, int nID) : base(sDesc, nID){} \
};

#else

class QueryException : public std::exception
{
	int						m_ID;
public:
	virtual const char* what() const	{ return "QueryException exception";}
	int					GetID() const	{ return m_ID;		}

protected:
	QueryException( int nID ) : m_ID(nID){}
};

#define QUERY_EXCEPTION(child, base) class child : public base \
{ \
public: \
	child(int nID) : base(nID){} \
};

#define	QUERY_CHILD_EXCEPTION( x )	QUERY_EXCEPTION(x, QueryException)

QUERY_CHILD_EXCEPTION(InvalidQueryException)
QUERY_CHILD_EXCEPTION(InvalidVersionException)
QUERY_CHILD_EXCEPTION(TimeoutException)
QUERY_CHILD_EXCEPTION(InvalidTypeException)
QUERY_CHILD_EXCEPTION(NoDataException)
QUERY_CHILD_EXCEPTION(NoFilmException)
QUERY_CHILD_EXCEPTION(FilmAlreadyExistException)
QUERY_CHILD_EXCEPTION(NoCinemaException)
QUERY_CHILD_EXCEPTION(InvalidDateTimeException)
QUERY_CHILD_EXCEPTION(QueryDatabaseException)
QUERY_CHILD_EXCEPTION(WholeInfoException)

#endif

inline CString LoadStringInternal( UINT nID )
{
	// great hack - dll must be named csengine.dll
	HMODULE hDll = GetModuleHandle( _T("csengine") );
	TCHAR szBuf[255];
	LoadString( hDll, nID, szBuf, 255) ;
	return CString(szBuf);
}

#endif // _QUERY_EXCEPTION_8545325443475501_