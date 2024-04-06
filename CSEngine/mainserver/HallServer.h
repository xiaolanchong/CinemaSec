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

//! �������� ������
typedef std::vector<__int8>	 BinaryData_t;

//! ������ ��� �������� �� ICameraAnalyzer � IHallAnalyzer �� ������ ������
struct CameraData
{
	//! �������?
	bool			m_bIsDataValid;
	//! �������� ������ �� ICameraAnalyzer
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

	//! �������� ��� ����������
	void Clear()
	{
		m_bIsDataValid = false;
	}
	//! ��������� ������
	//! \param pData ��������� �� ������
	//! \param size ������ ������
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

//! \brief ����� ������ ������� ����, ������������ IHallAnalyzer
//! \version 1.2
//! \date 10-26-2005
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)Eugene
//! \todo ���������� CameraData ����� ������� ���������
//! \bug 
//! \li 2005-10-26 ���������� ������ ����� ������ ���-�� �������� 
//! \li 2005-11-18 ��������� ������� ������ ��� ���������

class HallServer : public virtual GenericThreadServer
{
public:
	HallServer();
	virtual ~HallServer();

	//! �-� ������
	//! \param pParam ��������� �� HallServer
	//! \return 0-�����, -1 - ������
	//! \todo ����� ���������� �� ThreadProc_Handled<>
	static unsigned int WINAPI Handled_HallAnalyzerThreadProc(void * pParam);

	//! ������� ��� ���� �������������� �����
	typedef std::map< int, CameraData > CameraData_t;

	//! ��������� ������� ��������� ���� (������� ������)
	void DrawState();

	//! ��������� ������
	struct HallThread
	{
		//! ��� ������������� ������
		ThreadSync								m_Thread;
		//! ��������� ���������
		IHallAnalyzer*							m_pAnalyzer;

		//! �-� ������
		//! \param pParam ��������� �� HallServer
		//! \return 0-�����, -1 - ������
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
		//! ������ �� ���������
		HANDLE									m_hEventForProcessing;
		//! ������������� ������� � CameraData
		SyncCriticalSection						m_SyncCS;
		//! ������ �� �������
		CameraData_t							m_CameraData;
#if 0
		//! ����� ����� ������?
		bool			IsGatherMode() const	{ return m_bGatherDataMode; }
		//! ����� � ����� ����� ������
		void			StartGatherMode();
		//! ��������� ���� ������
		void			EndGatherMode();

		//! ������� ������, ��� ������ �� ��������
		//! \return ��������� ��������������� �����
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
	//! ��������� ���������
	HallAnalyzer_t								m_HallAnalyzer;

	//! ������������ ����
	HallProcessConfig						m_HallProcessConfig;

	//! ������� ��� ���������
	ThreadMessage							m_HallMessage;
private:
	//! ������������� ���������
	//! \param Chairs ������ ����
	//! \param AlgoParam ��������� ��������� (������������)
	void		InitHallAnalyzer(const std::vector<BaseChair>& Chairs, const std::vector<BYTE>& AlgoParam,
								 const std::pair<CTime, CTime> & TimeFilm);
	//! ��������� ����� ���������� ��������� � �������� �� ���������
	//! \param AlgoParam ��������� �� ��
	//! \param Data ��������� ��� ���������
	//! \return S_OK - success
	HRESULT		LoadAlgoParam( const std::vector<BYTE>& AlgoParam, Int8Arr& Data );
public:
	//! ����� ���������
	//! \param Chairs ������ ����
	//! \param AlgoParam ��������� ��������� (������������)
	//! \return 0 - �����, ��������� - ������
	DWORD	StartHallThread(const std::vector<BaseChair>& Chairs, 
							const std::vector<BYTE>& AlgoParam,
							const std::pair<CTime, CTime> & TimeFilm);
	//! ���������� ��������
	//! \return 0 - �����, ��������� - ������ 
	DWORD	StopHallThread();
	//! �������� ������ � �������� � ������� ���-�� ��������
	DWORD	Process();

	//! ������� ��������� ���������
	//! \return ��������� ���������
	IHallAnalyzer*		GetHallAnalyzer();

	//! ��������� ������� ������ ��� ���������
	//! \param TimeFilm ����� ������ � �����
	//! \return �����/������
	bool	SetFilmTime( IHallAnalyzer * pInt, const std::pair<CTime, CTime>& TimeFilm);
};

#endif //__HALL_SERVER_H_INCLUDED_0050047040870707__
