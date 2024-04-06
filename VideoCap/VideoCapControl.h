#pragma once

#include "common/ExceptionEx.h"
#include <atlbase.h>
#include <dshow.h>
#include <qedit.h>
#include <vector>
#include <boost/shared_ptr.hpp>

MACRO_EXCEPTION_EX( DShowException, CommonExceptionEx )
MACRO_EXCEPTION_EX( CreateException, DShowException )
MACRO_EXCEPTION_EX( RecordException, DShowException )

class VideoCapControl
{
	friend class VideoCapControlFactory;

	DWORD								m_dwRegister;
	CComPtr< ICaptureGraphBuilder2>		m_pGraph;
	CComPtr< IGraphBuilder >			m_pBuilder;

	CComPtr< IBaseFilter >				m_pEncodeFilter;
	CComPtr< IBaseFilter >				m_pCapFilter;
	CComPtr< IFileSinkFilter >			m_pFileInt;
//	CComPtr< IBaseFilter >				m_pFileFilter;			

	BITMAPINFOHEADER					m_bih;
	std::vector<BYTE>					m_Image;
	void	CreateThumbView();

	HRESULT AddToRot(IUnknown *pUnkGraph);
	void	RemoveFromRot();

	void	Create( );
	void	Destroy( );

	void	GetThumbView( const BITMAPINFOHEADER* &pBih, const BYTE* &pByte  ) const ;
	bool	CreateFilter(IBaseFilter **ppFilter, LPCWSTR stFilterMoniker);

	void	StartRecord( LPCWSTR szFileName );
	void	StopRecord();
public:
	VideoCapControl( IBaseFilter* pCap, int nFPS );
	~VideoCapControl(void);
};

class VideoCapControlFactory
{
	struct ComInitializer
	{
		HRESULT m_hr;
		ComInitializer() { m_hr = CoInitializeEx(0, COINIT_MULTITHREADED);}
		~ComInitializer(){ if( m_hr == S_OK) CoUninitialize();}
	} m_ComInit;

	std::vector< boost::shared_ptr<VideoCapControl > >		m_Cameras;
public:

	void	Create(int nFPS);
	void	Destroy( );

	void	GetThumbView( size_t nIndex, const BITMAPINFOHEADER* &pBih, const BYTE* &pByte  ) const ;
	size_t	GetCameraNumber() const { return m_Cameras.size(); }

	void	StartRecord( size_t nIndex, LPCWSTR szFileName );
	void	StopRecord(size_t nIndex);
};
