//=====================================================================================//
//                                                                                     //
//                                       CSChair                                       //
//                                                                                     //
//                           Copyright by ElVEES, 2005                                 //
//                                                                                     //
//   -------------------------------------------------------------------------------   //
//   Description:                                                                      //
/*
		Hall thread server class
                                                                                       */
//                                                                                     //
//   -------------------------------------------------------------------------------   //
//   Author: Eugene Gorbachev                                                          //
//   Date:   26.01.2005                                                                //
//                                                                                     //
//=====================================================================================//
#ifndef __HALL_SERVER_H_INCLUDED_0050047040870707__
#define __HALL_SERVER_H_INCLUDED_0050047040870707__

class IBackgroundAccumulator;

#include "ThreadError.h"
#include "../../CSAlgo/algorithm/base_chair.h"
#include "../../CSAlgo/Interfaces/camera_analyzer_interface.h"
#include "../../CSAlgo/Interfaces/hall_analyzer_interface.h"
#include "../../CSAlgo/algorithm/data_transfer_types.h"

#include "../../CSAlgo/CSAlgo2/csalgo2.h"

//#include "../../csalgo/Interfaces/hall_analyzer_interface.h"
#include "../syscfg_int.h"

MACRO_EXCEPTION(HallServerException, ThreadServerException)
//=====================================================================================//
//                                  class HallServer                                   //
//=====================================================================================//

//! байтовый массив
typedef std::vector<__int8>	 BinaryData_t;

//! данные для передачи от ICameraAnalyzer к IHallAnalyzer по каждой камеры
struct CameraData
{
	//! валидны?
	bool			m_bIsDataValid;
	//! двоичные данные от ICameraAnalyzer
	BinaryData_t	m_Data;

	CameraData() : 
		m_bIsDataValid(false)
	{
	}

	CameraData(const CameraData& cd) : 
		m_bIsDataValid( cd.m_bIsDataValid ),
		m_Data( cd.m_Data )
		{
		}
	CameraData& operator = (const CameraData& cd)
	{
		m_bIsDataValid			= cd.m_bIsDataValid;
		m_Data					= cd.m_Data;
		return *this;
	}

	//! пометить как невалидные
	void Clear()
	{
		m_bIsDataValid = false;
	}
	//! заполнить данные
	//! \param pData указатель на массив
	//! \param size размер данных
	void SetData( const void* pData, size_t size )
	{
		m_bIsDataValid = true;
		m_Data.assign( (__int8*)pData, (__int8*)pData + size );
	}
};

#if 0
class SpectatorData
{
	HANDLE	m_hReady;
	LONG	m_nNumber;
public:
	SpectatorData(): m_hReady(CreateEvent( 0, TRUE, FALSE, 0)){}
	~SpectatorData(){CloseHandle(m_hReady);}
	
	void Set(LONG nSpectator)
	{
		m_nNumber = nSpectator;
		SetEvent( m_hReady );
	}
	bool IsReady() const { return WaitForSingleObject( m_hReady, 0  ) == WAIT_OBJECT_0; }
	void Reset() { ResetEvent(m_hReady); }
	LONG Get() const {  return m_nNumber; }
};
#else
struct SpectatorData
{
	int					m_nSpectators;
	std::vector<BYTE>	m_HallData;
};
#endif

//! \brief класс потока анализа зала, инкапсуляция IHallAnalyzer
//! \version 1.2
//! \date 10-26-2005
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)Eugene
//! \todo передавать CameraData через очередь сообщений
//! \bug 
//! \li 2005-10-26 возвращает данные после взятия кол-ва зрителей 
//! \li 2005-11-18 установка времени сеанса для алгоритма

class HallServer : public virtual GenericThreadServer
{
public:
	HallServer();
	virtual ~HallServer();

	//! ф-я потока
	//! \param pParam укзаатель на HallServer
	//! \return 0-успех, -1 - ошибка
	//! \todo можно переделать на ThreadProc_Handled<>
	static unsigned int WINAPI Handled_HallAnalyzerThreadProc(void * pParam);

	//! словарь для всех обрабатываемых камер
	typedef std::map< int, CameraData > CameraData_t;

	//! отрисовка текщего состояния зала (контуры кресел)
	void DrawState();

	//! структура потока
	struct HallThread
	{
		//! для синхронизации потока
		ThreadSync								m_Thread;
		//! интерфейс алгоритма
		IHallAnalyzer*							m_pAnalyzer;

		//! ф-я потока
		//! \param pParam укзаатель на HallServer
		//! \return 0-успех, -1 - ошибка
		static unsigned int WINAPI ThreadProc(void * pParam);

		HallThread( IHallAnalyzer* pAnalyzer ) : 
			m_pAnalyzer(pAnalyzer),
			m_hEventForProcessing(CreateEvent( 0, FALSE, FALSE, 0 ))
#if 0
			,
			m_bGatherDataMode(false)
#endif
		{
		}
		~HallThread( )
		{
			if( m_pAnalyzer )
			{
				m_pAnalyzer->Release();
				m_pAnalyzer = 0;
			}
			CloseHandle(m_hEventForProcessing);
		}
		//! сигнал на обработку
		HANDLE									m_hEventForProcessing;
		//! синхронизация доступа к CameraData
		SyncCriticalSection						m_SyncCS;
		//! данные по камерам
		CameraData_t							m_CameraData;
#if 0
		//! режим сбора данных?
		bool			IsGatherMode() const	{ return m_bGatherDataMode; }
		//! войти в режим сбора данных
		void			StartGatherMode();
		//! закончить сбор данных
		void			EndGatherMode();

		//! вернуть камеры, где данные не получены
		//! \return множество идентификаторов камер
		std::set<int>	WhatCamerasNotProcessing()	const;
		void			TryRequestData()				{ m_LastRequestedTime = Plat_Int64Time();		}			
		__int64			TimeFromStart()			const	{ return Plat_Int64Time() - m_StartGetTime;		}
		__int64			TimeFromLastRequest()	const	{ return Plat_Int64Time() - m_LastRequestedTime;}
	private:
		__int64			m_StartGetTime;
		__int64			m_LastRequestedTime;			
		bool			m_bGatherDataMode;
#endif
	};

	typedef boost::shared_ptr< HallThread	>	HallAnalyzer_t;
	//! интерфейс алгоритма
	HallAnalyzer_t								m_HallAnalyzer;

	//! конфигурация зала
	HallProcessConfig						m_HallProcessConfig;

	//! очередб для сообщений
	ThreadMessage							m_HallMessage;
private:
	//! инициализация алгоритма
	//! \param Chairs кресла зала
	//! \param AlgoParam параметры алгоритма (игнорируются)
	void		InitHallAnalyzer(const std::vector<BaseChair>& Chairs, const std::vector<BYTE>& AlgoParam,
								 const std::pair<CTime, CTime> & TimeFilm);
	//! провернка новых параметров алгоритма и загрузка по умолчанию
	//! \param AlgoParam параметры из БД
	//! \param Data параметры для алгоритма
	//! \return S_OK - success
	HRESULT		LoadAlgoParam( const std::vector<BYTE>& AlgoParam, Int8Arr& Data );
public:
	//! старт алгоритма
	//! \param Chairs кресла зала
	//! \param AlgoParam параметры алгоритма (игнорируются)
	//! \return 0 - успех, остальное - ошибка
	DWORD	StartHallThread(const std::vector<BaseChair>& Chairs, 
							const std::vector<BYTE>& AlgoParam,
							const std::pair<CTime, CTime> & TimeFilm);
	//! остановить алгоритм
	//! \return 0 - успех, остальное - ошибка 
	DWORD	StopHallThread();
	//! передать данные в алгоритм и вернуть кол-во зрителей
	DWORD	Process();

	//! вернуть интерфейс алгоритма
	//! \return интерфейс алгоритма
	IHallAnalyzer*		GetHallAnalyzer();

	//! установка времени сеанса для алгоритма
	//! \param TimeFilm время начала и конца
	//! \return успех/ошибка
	bool	SetFilmTime( IHallAnalyzer * pInt, const std::pair<CTime, CTime>& TimeFilm);
};

#endif //__HALL_SERVER_H_INCLUDED_0050047040870707__
