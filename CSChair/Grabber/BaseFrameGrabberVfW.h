// BaseFrameGrabberVfW.h: interface for the BaseFrameGrabberVfW class.
//
//////////////////////////////////////////////////////////////////////

#ifndef ELVEES_BASEFRAMEGRABBERVFW_H__INCLUDED_
#define ELVEES_BASEFRAMEGRABBERVFW_H__INCLUDED_

#if defined (_MSC_VER) && (_MSC_VER > 1000)
#pragma once
#endif

//#define BFG_Y800_OUTPUT

// Base classes
#include "../../CSUtility/utility/base_frame_grabber.h"
#include "../FrameGrabber.h"
#include "IGrabberFactory.h"

// Forward declaration
struct IAVIStream;

//////////////////////////////////////////////////////////////////////////
// BaseFrameGrabberVfW
//////////////////////////////////////////////////////////////////////////

//! \brief реализация файлового граббера с помощью VFW, powered by Mofo
//! \version 1.0
//! \date 10-18-2005
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)Eugene
//! \todo 
//! \bug 
//!
class BaseFrameGrabberVfW
	: public BaseFrameGrabberEx		// old interface
	, public IFileGrabber			// new interface
{
public:
	
	//! создать с параметрами
	//! \param nBitsPerPixel бит/пиксель выходного изображения или FOURCC если < 32
	//! \param bPerFrame покадрово?
	//! \param bLoopback зациклить?
	//! \param dwWidth желаемая ширина изображения, игнорируется
	//! \param dwHeight желаемая высота изображения, игнорируется
	//! \return 
	explicit BaseFrameGrabberVfW(
		int  nBitPerPixel,
		bool bPerFrameMode,
		bool bLoopback,
		DWORD dwWidth = 0L,
		DWORD dwHeight = 0L);

	virtual ~BaseFrameGrabberVfW();

public:	
	//! Create factory
	//! \return реализация интерфейса
	static BaseFrameGrabberVfW* CreateGrabber();
	//! проверить файл на существование
	//! \param stFile имя файла
	//! \return TRUE - существует, FALSE - нет
	static BOOL IsFileExists(LPCTSTR stFile);

public:
	// BaseFrameGrabber

	//! начать проигрывание
	//! \param lpFilename имя видеофайла
	//! \param param параиетры проигрывания
	//! \return true-успех, иначе ошибка
	virtual bool Start(LPCTSTR lpFilename, const FrameGrabberParameters& param);
	//! остановить проигрывание
	//! \return true-успех, иначе ошибка
	virtual bool Stop();
	//! проверка состояния
	//! \return true-успех, иначе ошибка
	virtual bool IsOk() const; 
	//! проверка на конец файла
	//! \return true-успех, иначе ошибка
	virtual bool IsEnd() const;
	//! заблокировать последний кадр
	//! \param pHeader заголовок
	//! \param pImage данные изображения
	//! \param nImgSize размер изображения
	//! \return true-успех, иначе ошибка
	virtual bool LockLatestFrame(const BITMAPINFO* &pHeader, const BYTE* &pImage, int &nImgSize);
	//! разблокировать после LockLatestFrame
	//! \return true-успех, иначе ошибка
	virtual bool UnlockProcessedFrame();
	//! есть ли свободный файл
	//! \return true-успех, иначе ошибка
	virtual bool HasFreshFrame() const;

	// BaseFrameGrabberEx

	//! вернуть версию граббера
	//! \return версия
	virtual DWORD GetVersion();

	//! вернуть размер кадра, возможно до проигрывания
	//! \param x ширина
	//! \param y высота
	//! \return 0-успех, иначе ошибка
	virtual DWORD GetSize(DWORD& x, DWORD& y);		// ??? return what
	//! вернуть текущую позицию в файле
	//! \param pos текущая позиция в файлаб 0.0-начало, 1.0-конец
	//! \return 0-успех, иначе ошибка
	virtual DWORD GetPos(float& pos);				// position in percents

	// IBaseGrabber

	//! освободить интерфейс
	virtual void Release();
	//! заблокировать кадр
	//! \param ppHeader 
	//! \param ppImage 
	//! \param nImgSize 
	//! \return S_OK - success, other - failing
	virtual HRESULT LockFrame(const BITMAPINFO* &ppHeader, const BYTE* &ppImage, int &nImgSize);
	//! разблокировать кадр
	//! \return S_OK - success, other - failing
	virtual HRESULT UnlockFrame();

	// IFileGrabber

	//! вернуть размер изображения
	//! \param sizeFrame размер изображения
	//! \return S_OK - success, other - failing 
	virtual HRESULT GetSize(SIZE &sizeFrame);
	//! вернуть номер текущего кадра
	//! \param nCurPos текущая позиция
	//! \return S_OK - success, other - failing 
	virtual HRESULT GetCurFrame(INT64 &nCurPos);
	//! вернуть кол-во кадров в файле
	//! \param nTotal кол-во кадров в файле
	//! \return S_OK - success, other - failing 
	virtual HRESULT GetTotalFrames(INT64 &nTotal);
	//! перейти на заданный кадр
	//! \param nCurPos номер кадра
	//! \return S_OK - success, other - failing 
	virtual HRESULT Seek(INT64 nCurPos);
	//! вернуть кол-во fps
	//! \param fFPS frame per second
	//! \return S_OK - success, other - failing 
	virtual HRESULT GetFPS(double &fFPS);

protected:
	// Open avi stream
	bool OpenInterfaces(LPCTSTR lpFilename);
	void CloseInterfaces();

	// Create AVS script file with YUY2 output
	bool CreateScriptFile(LPCTSTR lpFilename);

protected:
	//! бит/пиксель выходного изображения или FOURCC если < 32
	int   m_nBitPerPixel;
	//! покадрово?
	bool  m_bPerFrameMode;
	//! зациклить?
	bool  m_bLoopback;
	//! желаемая ширина изображения, игнорируется
	DWORD m_dwDesiredWidth;
	//! желаемая высота изображения, игнорируется
	DWORD m_dwDesiredHeight;

	//!	флаг инициализации
	bool m_bInited;

	IAVIStream       *m_pStream;	//!< AVI stream interface
	BITMAPINFOHEADER *m_pbiStream;	//!< first video stream format
	void *m_pFrame;					//!< frame buffer
	long  m_cbFrame;				//!< frame buffer size

	double m_FPS;					//!< Frame per second
	DWORD m_Frame;					//!< Current frame
	DWORD m_FrameStart;				//!< Start
	DWORD m_FrameTotal;				//!< Length

	TCHAR m_stScriptFile[MAX_PATH];	//!< Temporary script file

#ifdef BFG_Y800_OUTPUT
	BITMAPINFOHEADER  m_biY800;
	void *m_pFrameY800;				//!< Y800 frame buffer
	long  m_cbFrameY800;			//!< Y800 frame buffer size

	void YUY2toY800(LPBITMAPINFOHEADER lpbiIn, const void* pSrc, void* pDst);
#endif

private:
	//! Output functions for errors
	//! \param stFormat строка форматирования
	void Output(LPCTSTR stFormat, ...);

	// No copies do not implement
	BaseFrameGrabberVfW(const BaseFrameGrabberVfW &rhs);
	BaseFrameGrabberVfW &operator=(const BaseFrameGrabberVfW &rhs);
};

#endif // ELVEES_BASEFRAMEGRABBERVFW_H__INCLUDED_
