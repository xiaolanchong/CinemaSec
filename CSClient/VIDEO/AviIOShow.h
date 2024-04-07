// AviIOShow.h: interface for the AviIOShow class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AVIIOSHOW_H__C92A5FFB_F99A_41F5_A546_045DB5C5671E__INCLUDED_)
#define AFX_AVIIOSHOW_H__C92A5FFB_F99A_41F5_A546_045DB5C5671E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../../CSUtility/qedit.h"
#include <amvideo.h>
#include <atlbase.h>

class AviIOShow  
{
private:
  bool               m_ok;             //!< state of AVI stream
  double             m_streamRate;     //!< stream rate  (ms/frame)
  double             m_streamDuration; //!< stream duration ( ms )

  int                m_frameIndex;     //!< current frame index in video file

  CComPtr<IMediaDet> m_pMedia;

  union
  {
    BYTE       buffer[sizeof(BITMAPINFOHEADER)+ 255 * sizeof(RGBQUAD)];
    BITMAPINFO bi;
  }
  m_header;                            //!< DIB's header

  std::vector<char>	 m_ByteCache;


  double	IndexToTime( int nIndex ) const { return nIndex * m_streamRate;					}
  int		GetAllFrameNumber() const			{ return int(m_streamDuration/ m_streamRate);	}
protected:
	const BITMAPINFO    & GetDibHeader() const { return m_header.bi; }

public:
  AviIOShow();
  virtual ~AviIOShow();

  bool OpenRead( LPCTSTR fileName );
  void Close();

  bool GetFrame( int index );
  bool GetNextFrame();

  

  bool IsOk()  const { return m_ok;   }
  bool IsEnd() const { return ((m_frameIndex+1) >= GetAllFrameNumber()); }

  float GetFrameRate()     const { return float(1000/m_streamRate);					}
  int   GetFrameDuration() const { return int(m_streamRate);			}
  int   GetFrameNumber()   const { return GetAllFrameNumber();					}
  int   GetFrameIndex()    const { return m_frameIndex;							}

  const BITMAPINFOHEADER * GetDIB() const { return (m_ok ? (const BITMAPINFOHEADER * ) &m_ByteCache[0] : 0); }

};

#endif // !defined(AFX_AVIIOSHOW_H__C92A5FFB_F99A_41F5_A546_045DB5C5671E__INCLUDED_)
