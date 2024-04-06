#ifndef __FRAMEGRABBER_H__
#define __FRAMEGRABBER_H__

#include "../CSUtility/utility/base_frame_grabber.h"

extern "C"
{

	// OBSOLETE functions, instead use IGrabberFactory & IFileGrabber
//! ������� �������, ��� �������������, �� IGrabberFactory � IFileGrabber
//! \param ppFrameGrabber �������� �������
//! \param nBitsPerPixel ���/������� ���������� �����������, ���� < 32, FOURCC ��������� �������
//! \param bPerFrame ���������� �����������?
//! \param bLoopback ������������, ��� ���������� ����� ������������ � ������
//! \return 
DWORD	WINAPI CreateVideoFileGrabber( BaseFrameGrabber** ppFrameGrabber, int nBitsPerPixel, bool bPerFrame, bool bLoopback	);

//! ������� �������
//! \param pFrameGrabber 
//! \return 
DWORD	WINAPI DeleteVideoFileGrabber( BaseFrameGrabber* pFrameGrabber		);

#if 0
	// NOT IMPLEMENTED now, use IGrabberFactory & ICameraGrabber
DWORD	WINAPI CreateCameraGrabber( BaseFrameGrabber** ppFrameGrabber, LPCWSTR szSourceName	);
DWORD	WINAPI DeleteCameraGrabber( BaseFrameGrabber* pFrameGrabber		);
#endif
}
/*
class BaseFrameGrabberEx : public BaseFrameGrabber
{
public:
	virtual DWORD	GetVersion()				= 0;
	virtual DWORD	GetSize(DWORD& x, DWORD& y) = 0;
	virtual DWORD	GetPos( float& pos )		= 0;
};*/

#endif //__FRAMEGRABBER_H__