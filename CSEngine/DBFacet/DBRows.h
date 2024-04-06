//========================= Copyright © 2004, Elvees ==========================
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

//! класс исключения при работе с БД. Номер ошибки берется от провайдера
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

//! ошибка при неверной границе интервала
typedef BaseRowException<TableException, 0>	InvalidIntervalException;
//! ошибка при пересечении интервала
typedef BaseRowException<TableException, 1>	IntervalIntersectionException;

//! запись о кинотеатре в БД
struct CinemaRow
{
	//! идентификатор кинотеатра
	int			m_nID;
	//! название
	std::tstring		m_sName;
	//! комментарии
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
	//! идентификатор БД
	int			m_nID;
	//! идентификатор кинотеатра, где зал
	int			m_nCinemaID;
	//! номер зала
	int			m_nNumber;
	//! адрес обработчика зала
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
	//! идентификатор камеры
	int						m_nID;
	//! идентфикатор зала
	int						m_nRoomID;
	//! позиция в 3х мерном массиве, (0,0) - верхний левый
	std::pair<int,int>		m_Pos;
	//! GUID камеры для chcsva
	std::tstring					m_sGUID;
	//! адрес сервера, где находится камера
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

//! таблица камеры
struct DBTableCameraFacet
{
	//! вернуть все записи о камерах
	//! \return все камеры
	virtual const std::set<CameraRow>&	GetCameras (  ) const							= 0;
	
	//! выбрать камеры из зала
	//! \param nRoomID идентификатор зала
	//! \return 
	virtual std::set<int>				SelectCamerasFromRoom(int nRoomID)		const	= 0;

	//! получить GUID камеры
	//! \param nCameraID идентификатор
	//! \return строковое представление GUID или имя файла
	virtual std::tstring				GetCameraSource( int nCameraID ) const			= 0;

	//! получить задний фон камеры
	//! \param nCameraID идентификатор камеры
	//! \param Data данные изображения
	//! \return успех/ошибка
	virtual	bool						GetBackground(int nCameraID, std::vector<BYTE>& Data)		= 0;	

	//! сохранить в БД задний фон
	//! \param nCameraID идентфикатор камеры, с которой накоплен фон
	//! \param Data данные изображения
	//! \return успех/ошибка
	virtual	bool						SetBackground(int nCameraID, const std::vector<BYTE>& Data)	= 0;

	//! обновить расположение камер
	//! \param LayoutMap новый словарь иденификатор - позиция в прямоугольном 2D массиве
	//! \return успех/ошибка
	virtual bool						UpdateLayout( const std::map< int, std::pair<int, int > >&  LayoutMap ) = 0;
};

typedef std::map<int, std::tstring >	NameMap_t;

//! интерфейс доступа к таблице залов
struct DBTableRoomFacet
{
	//! взять все залы
	//! \return множество записей
	virtual const std::set<RoomRow>&	GetRooms (  )							const = 0;

	//! выбрать залы из определенного кинотеатра
	//! \param nCinemaID идентификатор кинотеатра
	//! \return мн-во идентификаторов залов
	virtual std::set<int>				SelectRoomsFromCinema(int nCinemaID)	const = 0;	

	//! получить имя кинтетара и новер зала
	//! \param nRoomID идентификатор зала
	//! \param sCinemaName имя кинотеатра, где находится зал
	//! \param nRoomNo номер зала
	//! \return успех/ошибка
	virtual bool						GetRoomInfo(int nRoomID, CString& sCinemaName, int& nRoomNo ) = 0;

	//! получить кресла зала 
	//! \param nRoomID идентификатор зала
	//! \param Data двоичные данные (файл кресел)
	//! \return успех/ошибка
	virtual	bool						GetChairs(int nRoomID, std::vector<BYTE>& Data)					= 0;	

	//! записать файл кресел в БД
	//! \param nRoomID идентификатор зала
	//! \param Data данные файла
	//! \return успех/ошибка
	virtual	bool						SetChairs(int nRoomID, const std::vector<BYTE>& Data)			= 0;

	//! получить все номера залов
	//! \return карта идентификатор-номер зала
	virtual std::map<int,int>			GetRoomNumbers() const = 0;

	//! получить параметры подсчета зрителей
	//! \param nRoomID идентфикатор зала
	//! \param After кол-во секунд после старта сеанса
	//! \param Before кол-во секунд до конца сеанса
	//! \param Period период (сек) выборки статистики
	virtual void						GetWorkParam( int nRoomID, int & After, int &Before, int& Period)		= 0 ;

	//! сохранить параметры подсчета зрителей
	//! \param nRoomID идентфикатор зала
	//! \param After кол-во секунд после старта сеанса
	//! \param Before кол-во секунд до конца сеанса
	//! \param Period период (сек) выборки статистики
	virtual void						SetWorkParam( int nRoomID, int  After, int Before, int Period)		= 0;

	//! получить параметры архива для зала
	//! \param nRoomID идентификатор зала
	//! \param fStart процент начала работы удаления архива
	//! \param fEnd процент конца работы удаления архива
	//! \param nDaysKeep кол-во дней хранения архива
	virtual void						GetArchiveParam ( int nRoomID, double&  fStart, double &fEnd, int& nDaysKeep ) = 0;

	//! получить параметры архива для зала
	//! \param nRoomID идентификатор зала
	//! \param fStart процент начала работы удаления архива
	//! \param fEnd процент конца работы удаления архива
	//! \param nDaysKeep кол-во дней хранения архива
	virtual void						SetArchiveParam ( int nRoomID, double  fStart, double fEnd, int nDaysKeep ) = 0;

	//! получить параметры алгоритма, на данный момент не используются
	//! \param nRoomID идентификатор зала
	//! \param Data двоичные данные
	//! \return успех/ошибка
	virtual	bool						GetAlgoParam(int nRoomID, std::vector<BYTE>& Data)	= 0	;	

	//! установить параметры алгоритма, на данный момент не используются
	//! \param nRoomID идентификатор зала
	//! \param Data двоичные данные
	//! \return успех/ошибка
	virtual	bool						SetAlgoParam(int nRoomID, const std::vector<BYTE>& Data) = 0	;

	//! получить модель склейки зала
	//! \param nRoomID идентификатор зала
	//! \param Data двоичные данные
	virtual	void						GetMergeModel(int nRoomID, std::vector<BYTE>& Data)	= 0	;	

	//! сохранить модель склейки зала
	//! \param nRoomID идентификатор зала
	//! \param Data двоичные данные
	virtual	void						SetMergeModel(int nRoomID, const std::vector<BYTE>& Data) = 0	;
};

//! идентификаторы камер
typedef std::set<int>					CameraSet_t;

//! камеры в залах
typedef std::map< int , CameraSet_t > 	RoomSet_t;

//! залы в кинотеатрах
typedef std::map< int , RoomSet_t	> 	CinemaSet_t;

//! таблица кинотеатров
struct DBTableCinemaFacet
{
	//! получить все кинотетары
	//! \return все записи
	virtual const std::set<CinemaRow>&	GetCinemas (  ) const = 0;

	//! разбить по кинотеатрам
	//! \param Cinemas залы в кинотеатрах
	virtual void GetCinemaSet( CinemaSet_t& Cinemas ) const = 0;

	//! получить все названия кинотетаров
	//! \return карта ID - название
	virtual NameMap_t					GetCinemaNames() const = 0;
};

//! запись статистики
struct StatRow
{
	//! идентификатор записи
	int			m_nID;
	//! идентификатор зала, где был подсчет
	int			m_nRoomID;
	//! время выборки
	CTime		m_Time;
	//! кол-во зрителей
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
//! фотографии по залу
typedef std::map<int, const std::vector<BYTE> * >	PhotoMap_t;
//! пара идентификатор сеанса - кол-во зрителей
typedef std::pair< int, int >						FilmStat_t;

//! таблица статистики
struct DBTableStatFacet
{
	//! добавить статистику
	//! \param nRoomID идентфикатор зала
	//! \param Time время выборки
	//! \param nSpectators кол-во зрителей
	//! \param BigImage карта идентификатор камеры - изображение с нее
	virtual void						SetStatisticsRow(int nRoomID, 
		const DBTIMESTAMP& Time, int nSpectators,
		const std::map< int, std::vector<BYTE> >& BigImage ) = 0;

	//! получить фотографии с выборки
	//! \param nStatID номер выборки
	//! \return все фотографии по камерам
	virtual PhotoMap_t					GetImages(int nStatID)	= 0;

	//! вырнуть статистику за период
	//! \param timeBegin начало
	//! \param timeEnd конец
	//! \param nRoomID идентификатор зала
	//! \param Stats мн-во записей
	virtual void	GetStatForPeriod( CTime timeBegin, CTime timeEnd, int nRoomID, StatRowSet_t& Stats )	= 0;

	//! вырнуть статистику за период для отчета по залам
	//! \param timeBegin начало
	//! \param timeEnd конец
	//! \param Rooms залы, по которым подводится отчет
	//! \param Stats мн-во записей
	virtual void	GetStatForPeriodReport(	CTime timeBegin, CTime timeEnd, 
											const std::vector<int>& Rooms, StatRowSet_t& Stats )	= 0;

	//! получить инф-ю о выборке
	//! \param nStatID идентификатор выборки
	//! \param nRoomID идентификатор зала
	//! \param timeStat время
	//! \param nSpectators кол-во зрителей
	virtual void	GetStatInfo( int nStatID, int& nRoomID, CTime& timeStat, int& nSpectators ) = 0;

	//! выдать статистику для компонента обмена - список всех сеансов и кол-во зрителей на низ за период
	//! \param nRoomID идентификатор зала 
	//! \param timeBegin начало
	//! \param timeEnd конец
	//! \param StatArr массив сеансов и кол-ва зрителей
	//! \param nStatID идентификатор для сообщения обмена
	virtual void	GetStatForExchange( int nRoomID, CTime timeBegin, CTime timeEnd, std::vector< FilmStat_t >& StatArr,int &nStatID ) = 0;
};

MACRO_EXCEPTION( NoSuchCinemaOrRoom,	TableException );
MACRO_EXCEPTION( NoSuchExchangeID,		TableException );

//! таблица расписания
struct DBTableTimetableFacet
{
	//! установить расписание, все предыдущие данные за период стираются
	//! \param nRoomID идентификатор зала
	//! \param Range начало-конец периода внесения расписания
	//! \param Timetable расписание
	//! \param Names массив названий сеансов, размер == размеру Timetable
	virtual void			SetTimetable(	int nRoomID, const FilmTime& Range,const TimeTable_t& Timetable, 
											const std::vector<std::wstring>& Names ) 	= 0;
	//! прочитать расписание
	//! \param nRoomID идентификатор зала
	//! \param Range начало-конец периода внесения расписания
	//! \param Timetable расписание
	//! \param Names массив названий сеансов, размер == размеру Timetable
	virtual void			GetTimetable( int nRoomID, const FilmTime& Range, TimeTable_t& Timetable,
											std::vector<std::wstring>& Names) 	= 0;

	//! взять текущий или следующий сеанс
	//! \param nRoomID идентификатор зала
	//! \param Time время сеанса
	//! \return успех/ошибка
	virtual bool			GetCurrentOrNextFilmTime( int nRoomID, FilmTime& Time) = 0;

	//! взять текущий  сеанс
	//! \param nRoomID идентификатор зала
	//! \param Name название фильма 
	//! \return успех/ошибка
	virtual bool			GetCurrentFilmTime( int nRoomID, FilmTime& Time, std::wstring& Name) = 0;

	//! получить информацию о сеансе через идентификатор
	//! \param nFilmID идентификатор сеанса
	//! \param Time время сеанса
	//! \param Name название фильма
	//! \return успех/ошибка
	virtual bool			GetFilmFromId( int nFilmID, FilmTime& Time, std::wstring& Name   ) = 0; 

	//! 
	//! \param sCinemaName имя кинотеатра
	//! \param nRoomNo номер зала
	//! \param ExchangeID идентификаторы обмена сообщениями
	//! \param timeBegin время
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

	//! взять время предыдущего фильм
	//! \param nRoomID идентификатор зала
	//! \param timeEnd время конца
	//! \return успех/ошибка
	virtual bool			GetPreviousCinemaFilm( int nRoomID, CTime& timeEnd ) = 0;
};

//! вид параметров обмена
struct DBExchangeFacet
{
	//! установить параметры обмена
	//! \param nRoomID идентификатор зала
	//! \param nTimeoutSec таймаут сообщения (сек)
	//! \param nTimeoutNumber кол-во таймаутов
	//! \param nWaitTimeout ожидания ответа на сообшение (сек)
	//! \param nStatTreshold период (сек) между концом сеанса и выдачи сообщения о статистике
	virtual void	SetExchangeParam( int nRoomID, int nTimeoutSec, int nTimeoutNumber, int nWaitTimeout, int nStatTreshold ) = 0;

	//! получить параметры обмена
	//! \param nRoomID идентификатор зала
	//! \param nTimeoutSec таймаут сообщения (сек)
	//! \param nTimeoutNumber кол-во таймаутов
	//! \param nWaitTimeout ожидания ответа на сообшение (сек)
	//! \param nStatTreshold период (сек) между концом сеанса и выдачи сообщения о статистике
	virtual void	GetExchangeParam( int nRoomID, int& nTimeoutSec, int& nTimeoutNumber, int& nWaitTimeout, int& nStatTreshold ) = 0;
};

//! таблиуа сообщений обмена
struct DBProtocolTable
{
	//! тип сообщения
	enum MsgType
	{
		//! неизвестный от BoxOffice
		BO_Unknown		= 1,
		//! сообщение расписания
		BO_Timetable	= 2,
		//! ответ от BoxOffice
		BO_Response		= 3,
		//! сообщение статистики
		CCS_Statistics	= 50,
		//! ответ на расписание
		CCS_Response	= 51
	};

	//! тип сообщения, время сообщения, текст сообщения
	typedef boost::tuple< MsgType, CTime, std::wstring >	Message_t;

	//! добавить сообщение в таблицу
	//! \param nCinemaID идентификатор кинотеатра
	//! \param mt тип сообщения
	//! \param timeMsg время сообщения
	//! \param sText текст сообщения
	virtual void	AddMessage( int nCinemaID, MsgType mt, CTime timeMsg, const std::wstring& sText ) = 0;

	//! вернуть сообщения за период
	//! \param nCinemaID идентификатор кинотеатра
	//! \param timeFrom начало 
	//! \param timeTo конец
	//! \param Msgs сообщения
	virtual void	GetMessage( int nCinemaID, CTime timeFrom, CTime timeTo, std::vector<Message_t> & Msgs ) = 0;
};

#endif // DBROWS_H
