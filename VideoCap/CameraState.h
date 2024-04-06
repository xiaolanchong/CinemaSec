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
#ifndef _CAMERA_STATE_7100249085475726_
#define _CAMERA_STATE_7100249085475726_
//======================================================================================//
//                                  class CameraState                                   //
//======================================================================================//

class VideoCapControlFactory;
class TimeTableLoader;

class CameraState
{
protected:
	size_t					m_nIndex;
	VideoCapControlFactory*	m_pCap;

	CameraState( size_t nIndex, VideoCapControlFactory* pCap );
public:
	size_t					GetIndex() const { return m_nIndex;}
	static	CameraState*	GetDefaultState( size_t nIndex, VideoCapControlFactory* pCap );

	virtual	CameraState*	Process( TimeTableLoader& tl )	= 0;
	virtual	void			Terminate()						= 0;	
	virtual CString			GetState() const				= 0;
	virtual ~CameraState() {};
};

#endif // _CAMERA_STATE_7100249085475726_