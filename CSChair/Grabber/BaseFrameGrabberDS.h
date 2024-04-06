// BaseFrameGrabberDS.h: interface for the BaseFrameGrabberDS class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BASEFRAMEGRABBERDS_H__18D44C20_04C5_4A98_B84F_AA397926D50E__INCLUDED_)
#define AFX_BASEFRAMEGRABBERDS_H__18D44C20_04C5_4A98_B84F_AA397926D50E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../../CSUtility/utility/base_frame_grabber.h"
#include "../FrameGrabber.h"
#include "IGrabberFactory.h"
#include <string>
#include <atlbase.h>
#include <qedit.h>
#include <dshow.h>
#include <vector>

#include "../Common/Exception.h"
#if 0
#include "Subclass.h"
#endif

#define WM_GRAPHNOTIFY		WM_USER+0x100

MACRO_EXCEPTION(VideoException2,		CommonException);
MACRO_EXCEPTION(VideoSizeException2, VideoException2);
MACRO_EXCEPTION(VideoFileException2, VideoException2);
MACRO_EXCEPTION(DirectShowException2, VideoFileException2);

//! \brief DirectShow граббер
//! \version 1.0
//! \date 10-19-2005
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \todo 
//! \bug 
//!
class BaseFrameGrabberDS  :	public BaseFrameGrabberEx,  // old interface
							public IFileGrabber			// new interface
{
	//! окно для сообщений
	HWND		m_hWndStatic;
	//! первоначальная оконная процедура
	WNDPROC		m_fnOldWndProc;
	//! результат инициализации COM
	HRESULT		m_hrCoInit;

public:
	//! создать с заданными параметрами
	//! \param nBitPerPixel бит/пиксель если <32, иначе FOURCC выходного формата
	//! \param bPerFrameMode покадровый режим?
	//! \param bLoopback зациклить?
	BaseFrameGrabberDS( int nBitPerPixel, bool bPerFrameMode , bool bLoopback );
	virtual ~BaseFrameGrabberDS();

	//! вернуть размер изображения
	//! \return размер
	CSize	GetSize() const { return CSize(m_bi.bmiHeader.biWidth, m_bi.bmiHeader.biHeight); }
	//! вернуть состояние DS графа
	//! \return состояние DS графа
	OAFilterState	GetState() const;
	//! вернуть текущую позицию в файле
	//! \return 0.0 - начало, 1.0 - конец
	float			GetPos() const;
	//! остановить граф
	//! \return true - success, false - failing
	bool	StopGraph();
	//! проиграть граф
	//! \return true - success, false - failing
	bool	PlayGraph();
	//! преостановить граф
	//! \return true - success, false - failing
	bool	PauseGraph();

	//! установить скорость проигрывания
	//! \param r скорость проигрывания, 1.0 - нормальная
	void	SetRate(float r) { m_fRate = r; };

	// BaseFrameGrabber
	//! открыть файл и начать проигрывание
	//! \param name имя файла
	//! \param param параметры проигрывания
	//! \return true - success, false - failing
	virtual bool Start( LPCTSTR name, const FrameGrabberParameters & param ) ;
	//! остановить проигрывание
	//! \return true - success, false - failing
	virtual bool Stop() ;
	//! вернуть состояние потока
	//! \return true - рабочее состояние, false - ошибка
	virtual bool IsOk() const ;
	//! достигнут ли конец файла
	//! \return true - конец файла, false - нет
	virtual bool IsEnd() const ;
	//! заблокировать текущий кадр и переместиться на следующий
	//! \param pHeader заголовок изображения 
	//! \param pImage данные изобраджения 
	//! \param imgSize размер изображения
	//! \return true - success, false - failing
	virtual bool LockLatestFrame( LPCBITMAPINFO & pHeader, LPCUBYTE & pImage, int & imgSize ) ;
	//! разблокирвать кадр
	//! \return true - success, false - failing
	virtual bool UnlockProcessedFrame() ;
	//! есть ли новые кадры
	//! \return true - да, false - нет
	virtual bool HasFreshFrame() const ;

	// BaseFrameGrabberEx
	//! вернуть версию граббера
	//! \return версия граббера
	virtual DWORD	GetVersion()				{ return 1;};
	//! вернуть размер изображения
	//! \param x ширина
	//! \param y высота
	//! \return 0 - успех, 1 - ошибка
	virtual DWORD	GetSize(DWORD& x, DWORD& y) 
	{ 
		CSize sz = GetSize(); 
		x = sz.cx; 
		y = sz.cy;
		return 0;
	};
	//! вернуть текущую позицию в файле
	//! \param pos 0.0 - начало, 1.0 - конец
	//! \return 0 - успех, 1 - ошибка
	virtual DWORD	GetPos( float& pos )		
	{
		pos = GetPos();
		return 0;
	}

	// IFileGrabber
	//!  вернуть размер изображения
	//! \param sizeFrame размер изображения
	//! \return S_OK - success, other - failing
	virtual HRESULT GetSize( SIZE& sizeFrame )	
	{
		DWORD x , y;
		DWORD res = GetSize(x, y);
		if( res == S_OK ) 
		{
			sizeFrame.cx = x;
			sizeFrame.cy = y;
		}
		return res;
	}
	//!  вернуть размер изображения
	//! \param nCurPos номер текущего кадра
	//! \return S_OK - success, other - failing
	virtual HRESULT GetCurFrame( INT64 &nCurPos );		
	//! вернуть кол-во кадров в изображении
	//! \param &nTotal кол-во кадров
	//! \return S_OK - success, other - failing
	virtual HRESULT GetTotalFrames( INT64 &nTotal )	;	
	//! переместиться на кадр
	//! \param nCurPos номер кадра
	//! \return S_OK - success, other - failing
	virtual HRESULT Seek( INT64 nCurPos )	;
	//! вернкть frame per second
	//! \param fFPS fps
	//! \return S_OK - success, other - failing
	virtual HRESULT GetFPS(double& fFPS);
	//! заблокировать кадр,перейти на следующий
	//! \param pHeader заголовок изображения во внутреннм буфере
	//! \param pImage данные изображения
	//! \param nImgSize размер данных
	//! \return S_OK - success, other - failing
	virtual HRESULT LockFrame( const BITMAPINFO * & pHeader, const BYTE * & pImage, int &  nImgSize ) ;
	//! разблокировать кадр
	//! \return S_OK - success, other - failing
	virtual HRESULT UnlockFrame()	;
	//! самоубийство
	virtual void	Release() { delete this; }

	//! обработать сообщение от DirectShow
	//! \param evCode код сообщения
	void			ProcessCode( LONG evCode );
protected:
	//! ширина изображения
	long	m_Width;
	//! высота изображения
	long	m_Height;
	//! бит/пиксель
	int				m_nBitPerPixel;

	//! буфер для BITMAPINFO
	struct BmpEx : BITMAPINFO 
	{
		RGBQUAD rgb[255];
	}	m_bi;

	//! cookie от ROT
	DWORD	m_dwRegister;
	//! скорость проигрывания
	float	m_fRate;
	//! покадрово?
	bool	m_bPerFrameMode;
	//! зациклить?
	bool	m_bLoopback;

	//! буфер для кадра
	std::vector<BYTE>	m_SampleBuffer;
	//! fps
	double				m_fFPS;

	//! новая оконная процедура для обработки сообщения от DirectShow
	static LRESULT	WindowProc( HWND hWnd, UINT msg, WPARAM wp, LPARAM lp  );
	
	//! 
	//! \param Width 
	//! \param Height 
	void	SetSize(long Width, long Height) { m_Height = Height ; m_Width = Width; }
	//! открыть файл для проигрывания
	//! \param FileName имя файла
	void	Grab( const std::wstring& FileName);

	//! for ROT /debug/
	HRESULT AddToRot(IUnknown *pUnkGraph) ;
	//! удалить из ROT
	void	RemoveFromRot();
	
	//! фильтр для передачи изображений из графа
    CComPtr< ISampleGrabber >	m_pGrabber;
	//! управление сообщениями от графа
	CComPtr< IMediaEventEx>		m_pMediaEvent;
	//! интерфейс построения графа
    CComPtr< IGraphBuilder >	m_pGraph;
	//! регулирует покадровое поступление изображения
	CComPtr< IVideoFrameStep>	m_pFrameStep;
#ifdef _DEBUG
	//! кол-во заблокированных кадров
	mutable LONG	m_dwLockNumber;
	//! время обработки
	mutable DWORD	m_dwTimeTicks;
#endif
};

#endif // !defined(AFX_BASEFRAMEGRABBERDS_H__18D44C20_04C5_4A98_B84F_AA397926D50E__INCLUDED_)
