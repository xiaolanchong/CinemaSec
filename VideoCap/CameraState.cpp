//======================================================================================//
//                                      CinemaSec                                       //
//                                                                                      //
//                              Copyright by ElVEES, 2005                               //
//                                                                                      //
//--------------------------------------------------------------------------------------//
// Description:
//	classes for camera states
//
//--------------------------------------------------------------------------------------//
// Author : Eugene Gorbachev (Eugene.Gorbachev@biones.com)
// Date   : 26.04.2005
//                                                                                      //
//======================================================================================//
#include "stdafx.h"
#include "CameraState.h"
#include "VideoCapControl.h"
#include "TimeTable.h"
//======================================================================================//
//                                  class CameraState                                   //
//======================================================================================//
CameraState::CameraState(size_t nIndex, VideoCapControlFactory* pCap) :
	m_nIndex( nIndex), m_pCap( pCap )
{
}

///////////////////////////////////////////////////////////////////////////////////////////

class CameraStateIdle : public CameraState
{
	virtual CameraState*	Process( TimeTableLoader& tl );
	virtual	void			Terminate()	{}
	virtual	CString			GetState() const;
public:
	CameraStateIdle( size_t nIndex, VideoCapControlFactory* pCap ) : CameraState( nIndex, pCap ) {};
};

class CameraStateRecord : public CameraState
{
	virtual CameraState*	Process( TimeTableLoader& tl );
	virtual	void			Terminate()	;
	virtual	CString			GetState() const;
public:
	CameraStateRecord( size_t nIndex, VideoCapControlFactory* pCap ) : CameraState( nIndex, pCap ) {};
};

CameraState*	CameraState::GetDefaultState( size_t nIndex, VideoCapControlFactory*	pCap )
{
	return new CameraStateIdle( nIndex, pCap );
}

CameraState*	CameraStateIdle::Process( TimeTableLoader& tl )
{
	bool bStartRecord = tl.GetState( GetIndex() ) == TimeTableLoader::st_record;
	if( bStartRecord )
	{
		CTime timeNow = CTime::GetCurrentTime();
		CString sName = timeNow.Format("Camera%%Id_%b%d_%H-%M.avi");
		CString sFullName;
		sFullName.Format( sName, GetIndex() + 1 );
		CString sDirPath = tl.GetPath( GetIndex() );
		try
		{
			WCHAR szBuf[MAX_PATH];
			PathCombineW( szBuf, (LPCWSTR)sDirPath, sFullName );
			m_pCap->StartRecord( GetIndex(), szBuf );
			return new CameraStateRecord( GetIndex(), m_pCap );
		}
		catch( RecordException )
		{
			throw;
		//	return NULL;
		}
	}
	else
		return NULL;
}

CString		CameraStateIdle::GetState() const
{
	CString s;
	s.Format( _T("camera %Id - Idle"), GetIndex() + 1);
	return s;
}

///////////////////////////////////////

CameraState*	CameraStateRecord::Process( TimeTableLoader& tl )
{
	bool bStopRecord = tl.GetState( GetIndex() ) == TimeTableLoader::st_idle;
	if( bStopRecord )
	{
		try
		{
			m_pCap->StopRecord( GetIndex() );
			return new CameraStateIdle( GetIndex(), m_pCap );
		}
		catch( RecordException )
		{
			throw;
			//return NULL;
		}
	}
	else
		return NULL;
}

void	CameraStateRecord::Terminate()	
try
{
	m_pCap->StopRecord( GetIndex() );
}
catch( RecordException )
{
};

CString		CameraStateRecord::GetState() const
{
	CString s;
	s.Format( _T("camera %Id - Record"), GetIndex() + 1);
	return s;
}