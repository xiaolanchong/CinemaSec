//======================================================================================//
//                                      CinemaSec                                       //
//                                                                                      //
//                              Copyright by ElVEES, 2005                               //
//                                                                                      //
//--------------------------------------------------------------------------------------//
// Description:
//	Description is not available
//
//--------------------------------------------------------------------------------------//
// Author : Eugene Gorbachev (Eugene.Gorbachev@biones.com)
// Date   : 17.02.2005
//                                                                                      //
//======================================================================================//
#ifndef _CAMERA_MANAGER_2431180149317411_
#define _CAMERA_MANAGER_2431180149317411_
//======================================================================================//
//                                 struct CameraManager                                 //
//======================================================================================//

#include "../../CSChair/Grabber/IGrabberFactory.h"
#include <boost/shared_ptr.hpp>
#include "../BACKGROUND/Background.h"

class CameraManager
{
	boost::shared_ptr<IGrabberFactory>	m_Factory;
public:
	CameraManager();
	~CameraManager();

	ICameraGrabber*		CreateGrabber		( int nCameraID );
	IArchiveGrabber*	CreateArchiveGrabber( int nCameraID, INT64 nStartTime );
	IFileGrabber*		CreateFileGrabber( LPCWSTR szFileName );
	void				StopRoomCamera( int nRoomID );

	boost::shared_ptr<IBaseGrabber>	CreateGrabber( int nCameraID, LPCWSTR szSource, int nBits, bool bRealtime );
	boost::shared_ptr<IFileGrabber>	CreateFileGrabber( LPCWSTR szFileName, int nBits, bool bRealtime );
};


CameraManager& GetCameraManager();

#endif // _CAMERA_MANAGER_2431180149317411_