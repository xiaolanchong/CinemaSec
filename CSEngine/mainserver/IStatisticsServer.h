//=====================================================================================//
//                                                                                     //
//                                      CinemaSec                                      //
//                                                                                     //
//                              Copyright by ElVEES 2005                               //
//                                                                                     //
//   -------------------------------------------------------------------------------   //
//   Description:                                                                      //
/*
		interface fo statistics gathering & processing
                                                                                       */
//                                                                                     //
//   -------------------------------------------------------------------------------   //
//   Author: Eugene Gorbachev                                                          //
//   Date:   04.02.2005                                                                //
//                                                                                     //
//=====================================================================================//
#ifndef __I_STATISTICS_SERVER_H_INCLUDED_0713360688048087__
#define __I_STATISTICS_SERVER_H_INCLUDED_0713360688048087__

#define		STATISTICS_SERVER_INTERFACE_0	L"{CAE32DE8-EB25-4852-9583-2DA6547FB4B6}"
//#define		BACKUP_PARAM_INTERFACE			L"{EF38F0BD-260E-43AC-993A-38E7594BC188}"

#include "../../CSChair/debug_int.h"
#include "../DBFacet/TimeHelper.h"
#include <oledb.h>
//=====================================================================================//
//                               class IStatisticsServer                               //
//=====================================================================================//

//! \brief интерфейс статистики (расписание + данные подсчета)
//! \version 1.0
//! \date 10-27-2005
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)Eugene
//! \todo 
//! \bug 
//!
class IStatisticsServer
{
public:
	//! стартовать сервер
	//! \return S_OK - success, other - failure
	virtual HRESULT Start(/*LPCWSTR szConnectionString*/)		= 0;

	//! внести статистику
	//! \param nRoomID идентификатор зала
	//! \param Time время подсчета
	//! \param Images словарь идентификатор камеры - изображение
	//! \param nSpectators кол-во зрителей
	//! \return S_OK - success, other - failure
	virtual HRESULT Statistics( int nRoomID, const DBTIMESTAMP& Time, const std::map<int, std::vector<BYTE> >& Images, int nSpectators ) = 0;

	//! установить отладочный интерфейс
	//! \param pDebug отладочный интерфейс
	virtual void	SetDebugInterface( IDebugOutput* pDebug ) = 0;

	//! получить время ближайшего сеанса
	//! \param nRoomID идентификатор зала
	//! \return S_OK - success, other - failure
	virtual HRESULT	GetCurrentOrNextFilmTime( int nRoomID, FilmTime& ft ) = 0;

	//! вернуть параметры подсчета
	//! \param nRoomID идентификатор зала
	//! \param After кол-во сек после начала сеанса
	//! \param Before кол-во сек перед окончанием сеанса
	//! \param Period период (сек) выбора статистики
	//! \return S_OK - success, other - failure 
	virtual HRESULT	GetWorkParam( int nRoomID, int & After, int &Before, int& Period) = 0;

	//! взять параметры архива
	//! \param nRoomID идентификатор зала (перейти к кинотеатру к )
	//! \param fStart процентное соотношение начала удаления архива
	//! \param fStop процентное соотношение конца удаления архива
	//! \param nDaysKeep кол-во дней на хранение архива
	//! \return S_OK - success, other - failure
	virtual HRESULT	GetBackupParam( int nRoomID, double& fStart, double &fStop, int& nDaysKeep  ) = 0;

	//! взять параметры алгоритма
	//! \param nRoomID идентификатор зала
	//! \param Data данные алгоритма
	//! \return S_OK - success, other - failure
	virtual	HRESULT GetAlgoParam( int nRoomID, std::vector<BYTE>& Data ) = 0;
	virtual void	Release()	= 0;
};

inline void DestroyStatisticsServer( IStatisticsServer* pInt )
{
	pInt->Release();
}


#endif //__I_STATISTICS_SERVER_H_INCLUDED_0713360688048087__