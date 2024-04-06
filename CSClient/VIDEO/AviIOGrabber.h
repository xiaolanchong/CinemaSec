#if !defined(AFX_AVIIO_GRABBER_H__C92A5FFB_F99A_41F5_A546_045DB5C5671E__INCLUDED_)
#define AFX_AVIIO_GRABBER_H__C92A5FFB_F99A_41F5_A546_045DB5C5671E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CameraManager.h"
#include "../../CSChair/Grabber/ImageDecompressor.h"

class AviIOGrabber  
{
private:
  bool               m_ok;             //!< state of AVI stream
	INT64				m_nFrameNumber;
	double				m_fFPS;

	boost::shared_ptr<IFileGrabber>	m_pGrabber;
	std::vector<BYTE>				m_ByteCache;

  int                m_frameIndex;     //!< current frame index in video file

  int GetAllFrameNumber() const { return int(m_nFrameNumber);}

  std::auto_ptr< ImageDecompressor >	m_Decompressor; 
  struct HeaderEx : BITMAPINFOHEADER
  {
	  RGBQUAD rgb[256];
  } m_bihCache;
//  std::vector< BYTE >				m_ImageCache;
public:
  AviIOGrabber();
  virtual ~AviIOGrabber();

  bool OpenRead( LPCTSTR fileName );
  void Close();

  bool GetFrame( int index );
  bool GetNextFrame();

  bool IsOk()  const { return m_ok;   }
  bool IsEnd() const { return ((m_frameIndex+1) >= GetAllFrameNumber()); }

  float GetFrameRate()     const { return float(m_fFPS);					}
  int   GetFrameDuration() const { return int(1000/m_fFPS);			}
  int   GetFrameNumber()   const { return GetAllFrameNumber();					}
  int   GetFrameIndex()    const { return m_frameIndex;							}

  const BITMAPINFOHEADER *	GetDIB()	const;
  const BYTE*				GetData()	const { return (m_ok ? &m_ByteCache[0] : 0); }

};

#endif // !defined(AFX_AVIIO_GRABBER_H__C92A5FFB_F99A_41F5_A546_045DB5C5671E__INCLUDED_)
