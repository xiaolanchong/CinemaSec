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

//! \brief ��������� ���������� (���������� + ������ ��������)
//! \version 1.0
//! \date 10-27-2005
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)Eugene
//! \todo 
//! \bug 
//!
class IStatisticsServer
{
public:
	//! ���������� ������
	//! \return S_OK - success, other - failure
	virtual HRESULT Start(/*LPCWSTR szConnectionString*/)		= 0;

	//! ������ ����������
	//! \param nRoomID ������������� ����
	//! \param Time ����� ��������
	//! \param Images ������� ������������� ������ - �����������
	//! \param nSpectators ���-�� ��������
	//! \return S_OK - success, other - failure
	virtual HRESULT Statistics( int nRoomID, const DBTIMESTAMP& Time, const std::map<int, std::vector<BYTE> >& Images, int nSpectators ) = 0;

	//! ���������� ���������� ���������
	//! \param pDebug ���������� ���������
	virtual void	SetDebugInterface( IDebugOutput* pDebug ) = 0;

	//! �������� ����� ���������� ������
	//! \param nRoomID ������������� ����
	//! \return S_OK - success, other - failure
	virtual HRESULT	GetCurrentOrNextFilmTime( int nRoomID, FilmTime& ft ) = 0;

	//! ������� ��������� ��������
	//! \param nRoomID ������������� ����
	//! \param After ���-�� ��� ����� ������ ������
	//! \param Before ���-�� ��� ����� ���������� ������
	//! \param Period ������ (���) ������ ����������
	//! \return S_OK - success, other - failure 
	virtual HRESULT	GetWorkParam( int nRoomID, int & After, int &Before, int& Period) = 0;

	//! ����� ��������� ������
	//! \param nRoomID ������������� ���� (������� � ���������� � )
	//! \param fStart ���������� ����������� ������ �������� ������
	//! \param fStop ���������� ����������� ����� �������� ������
	//! \param nDaysKeep ���-�� ���� �� �������� ������
	//! \return S_OK - success, other - failure
	virtual HRESULT	GetBackupParam( int nRoomID, double& fStart, double &fStop, int& nDaysKeep  ) = 0;

	//! ����� ��������� ���������
	//! \param nRoomID ������������� ����
	//! \param Data ������ ���������
	//! \return S_OK - success, other - failure
	virtual	HRESULT GetAlgoParam( int nRoomID, std::vector<BYTE>& Data ) = 0;
	virtual void	Release()	= 0;
};

inline void DestroyStatisticsServer( IStatisticsServer* pInt )
{
	pInt->Release();
}


#endif //__I_STATISTICS_SERVER_H_INCLUDED_0713360688048087__