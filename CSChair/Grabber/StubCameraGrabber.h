//======================================================================================//
//                                      CinemaSec                                       //
//                                                                                      //
//                               Copyright� ElVEES, 2005                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
// Description:
//	���������� �������� ��� ICameraGrabber, ������� �������� �����������
//
//--------------------------------------------------------------------------------------//
// Author : Eugene Gorbachev (Eugene.Gorbachev@biones.com)
// Date   : 05.07.2005
//                                                                                      //
//======================================================================================//
#ifndef _STUB_CAMERA_GRABBER_4996502097390510_
#define _STUB_CAMERA_GRABBER_4996502097390510_

#include "IGrabberFactory.h"
#include "FileGrabberFactory.h"
////////// stub stream
//#define TEST_EXCEPTION
#undef TEST_EXCEPTION

//! \brief ���������� IArchiveGrabber, �������� ��� �������, ���������� ����� �� ��������
//! \version 1.0
//! \date 10-19-2005
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \todo 
//! \bug 
//!
class BaseStubCameraGrabber :  public IArchiveGrabber
{
protected:
	//! ������������� ������
	int					m_nID;
	//! ����� ��� ���������
	BITMAPINFO			m_cacheHdr;	
	//! ����� ��� �����������
	std::vector<BYTE>	m_cacheImage;	
	//! ������� ��� ���������� �����
	CDC					m_cacheDC;
	//! ������ ��� ����������
	CBitmap				m_cacheBmp;

	//! ������� ���������
	void				GenerateImage( );
	//! ������� ������� �� �����, ��������� �������������� ��� ����������� �������
	//! \return �������
	virtual	CString		GenerateHeader(  ) = 0;
public:
	//! ������� � �����������
	//! \param nID ������������� ������
	//! \param dwWidth ������ �����������
	//! \param dwHeight ������ �����������
	BaseStubCameraGrabber( int nID, DWORD dwWidth, DWORD dwHeight );
	virtual ~BaseStubCameraGrabber(){}

	virtual void	Release() { delete this;};

	virtual HRESULT LockFrame( const BITMAPINFO * &ppHeader, const BYTE * &ppImage, int   &nImgSize ) ;
	virtual HRESULT UnlockFrame()	;				
};

//! \brief �������� ��� ���������� ����������� � ������ ��� �������
//! \version 1.0
//! \date 10-19-2005
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \todo 
//! \bug 
//!
class RealtimeStubCameraGrabber : public BaseStubCameraGrabber
{
public:
	//! ������� � �����������
	//! \param nID ������������� ������
	//! \param dwWidth ������ �����������
	//! \param dwHeight ������ �����������
	RealtimeStubCameraGrabber( int nID, DWORD dwWidth, DWORD dwHeight ) : 
	  BaseStubCameraGrabber( nID, dwWidth, dwHeight )
	  {}

	  //! ������� ������� �� �����, ��������� �������������� ��� ����������� �������
	  //! \return �������
	CString		GenerateHeader(  ) ;
	virtual HRESULT SeekTo(INT64 nTime)			{ return E_NOTIMPL;}
	virtual HRESULT GetTime( INT64& nTime )		{ return E_NOTIMPL; }
};

//! \brief �������� ��� ���������� ����������� ������ ��� ������� 
//! \version 1.0
//! \date 10-19-2005
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \todo 
//! \bug 
//!
class ArchiveStubCameraGrabber : public BaseStubCameraGrabber
{
	//! ������� �����
	INT64	m_nTime;
	//! ����� � ���������� ������ Seek
	INT64	m_nLastSeekTime;
	//! ������� ����� � Output
	//! \param szDesc �����������
	//! \param t ����� ��� ������
	void	DumpTime( LPCTSTR szDesc, CTime t)
	{
#ifdef _DEBUG
		CString s = t.Format(_T("%H:%M %d.%m.%Y"));
		TCHAR buf[255];
		StringCchPrintf( buf, 255, _T("%s - %s\n"), szDesc, (LPCTSTR)s );
		OutputDebugString( buf );
#endif
	}
public:
	//! ������� � �����������
	//! \param nID ������������� ������
	//! \param dwWidth ������ �����������
	//! \param dwHeight ������ �����������
	//! \param nTime ����� ������ ������ ������ 
	ArchiveStubCameraGrabber( int nID, DWORD dwWidth, DWORD dwHeight, INT64 nTime ) : 
		BaseStubCameraGrabber( nID, dwWidth, dwHeight  ), 
		m_nTime( nTime),
		m_nLastSeekTime(_time64(0))
		{}

	  CString		GenerateHeader(  ) ;
	  virtual HRESULT SeekTo(INT64 nTime) 
	  { 
		  m_nTime			= nTime; 
		  m_nLastSeekTime	= _time64(0);
		  DumpTime( _T("ArchiveStubCameraGrabber::SeekTo"), m_nTime);
		  return S_OK;
	  }
	  virtual HRESULT GetTime( INT64& nTime )		
	  { 
		  INT64 nDelta = (_time64(0) - m_nLastSeekTime) ;
		  nTime = m_nTime + nDelta;
		  DumpTime(_T("ArchiveStubCameraGrabber::GetTime"), m_nTime);
		  return S_OK; 
	  }
};

//! \brief  �������� ��� ���������� ����������� ������ ��� �������, �������� ���������� ����� 
//! \version 1.0
//! \date 10-19-2005
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \todo 
//! \bug 
//!
class BaseServerStubCameraGrabber :  public ICameraGrabber
{
#ifdef TEST_EXCEPTION
	//! ������ ��������
	DWORD				m_dwFrameNum;
#endif
protected:
	//! ������������� ������
	int					m_nID;
	//! ����� ��� ���������
	BITMAPINFO			m_cacheHdr;	
	//! ����� ��� �����������
	std::vector<BYTE>	m_cacheImage;	

public:
	//! ������� � �����������
	//! \param nID ������������� ������
	//! \param dwWidth ������ �����������
	//! \param dwHeight ������ �����������
	BaseServerStubCameraGrabber( int nID, DWORD dwWidth, DWORD dwHeight );
	virtual ~BaseServerStubCameraGrabber(){}

	virtual void	Release() { delete this;};

	virtual HRESULT LockFrame( const BITMAPINFO * &ppHeader, const BYTE * &ppImage, int   &nImgSize ) ;
	virtual HRESULT UnlockFrame()		{ return S_OK;		}
	virtual HRESULT SeekTo(INT64 nTime) { return E_NOTIMPL;	}
};

//======================================================================================//
//                               class StubGrabberFactory                                //
//======================================================================================//

//! \brief ����� �������� ��������� ��� �������
//! \version 1.0
//! \date 10-19-2005
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \todo 
//! \bug 
//!
class StubGrabberFactory :	public		IGrabberFactory,
							protected	FileGrabberFactory
{
public:
	StubGrabberFactory();
	virtual ~StubGrabberFactory();

	virtual void		Release() 
	{
		delete this;
	}
	virtual HRESULT		Initialize( IDebugOutput* pDebugInt, bool bLocalVideo )
	{
		m_Debug.Set( pDebugInt );
		m_Debug.PrintW( IDebugOutput::mt_info, L"Stub grabber factory initialized. Use it only in tests" );
		return S_OK;
	}
	virtual void		SetDebugOutput( IDebugOutput* pDebugInt ) 
	{
		m_Debug.Set( pDebugInt );
	}
	virtual HRESULT		CreateCameraGrabber( ICameraGrabber** ppFrameGrabber, int nCameraID ) ;
	virtual HRESULT		CreateArchiveGrabber( IArchiveGrabber** ppFrameGrabber, int nCameraID, INT64 nStartTime ) ;

	virtual HRESULT		CreateFileGrabber(	IFileGrabber** ppFrameGrabber, 
											LPCWSTR  szFileName,
											int nBitsPerPixel, 
											bool bPerFrame, 
											bool bLoopback )
	{
		return FileGrabberFactory::CreateFileGrabber( ppFrameGrabber, szFileName, nBitsPerPixel, bPerFrame, bLoopback );
	}
};

//! \brief ����� �������� �������� ��� �������
//! \version 1.0
//! \date 10-19-2005
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \todo 
//! \bug 
//!
class StubServerGrabberFactory : public IGrabberFactory
{
	MyDebugOutputImpl	m_Debug;
public:
	StubServerGrabberFactory() {};
	virtual ~StubServerGrabberFactory() {};

	virtual void		Release() 
	{
		delete this;
	}
	virtual HRESULT		Initialize( IDebugOutput* pDebugInt, bool bLocalVideo )
	{
		m_Debug.Set( pDebugInt );
		m_Debug.PrintW( IDebugOutput::mt_info, L"Stub server grabber factory initialized. Use it only in tests" );
		return S_OK;
	}
	virtual void		SetDebugOutput( IDebugOutput* pDebugInt ) 
	{
		m_Debug.Set( pDebugInt );
	}
	virtual HRESULT		CreateCameraGrabber( ICameraGrabber** ppFrameGrabber, int nCameraID ) 
	{
		m_Debug.PrintW( IDebugOutput::mt_info, L"Create ServerStubCameraGrabber for id=%d", nCameraID );
		*ppFrameGrabber = new BaseServerStubCameraGrabber (nCameraID, 720, 576) ;
		return S_OK;
	}
	virtual HRESULT		CreateArchiveGrabber( IArchiveGrabber** ppFrameGrabber, int nCameraID, INT64 nStartTime ) 
	{
		if( !ppFrameGrabber ) return E_POINTER;
		*ppFrameGrabber = NULL;
		return E_NOTIMPL;
	}

	virtual HRESULT		CreateFileGrabber(	IFileGrabber** ppFrameGrabber, 
		LPCWSTR  szFileName,
		int nBitsPerPixel, 
		bool bPerFrame, 
		bool bLoopback )
	{
		if( !ppFrameGrabber ) return E_POINTER;
		*ppFrameGrabber = NULL;
		return E_NOTIMPL;
	}
};

#endif // _STUB_CAMERA_GRABBER_4996502097390510_