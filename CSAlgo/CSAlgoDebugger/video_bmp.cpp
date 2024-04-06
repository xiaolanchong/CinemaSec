#include "stdafx.h"

//-------------------------------------------------------------------------------------------------
/** \brief . */
//-------------------------------------------------------------------------------------------------
VideoBitmap::VideoBitmap()
{
  memset( m_biHeader, 0, sizeof(m_biHeader) );
  memset( m_exHeader, 0, sizeof(m_exHeader) );
  m_bRead = false;
  m_file = 0;
}


//-------------------------------------------------------------------------------------------------
/** \brief . */
//-------------------------------------------------------------------------------------------------
VideoBitmap::~VideoBitmap()
{
  Close();
}


//-------------------------------------------------------------------------------------------------
/** \brief . */
//-------------------------------------------------------------------------------------------------
bool VideoBitmap::Open( LPCTSTR fileName,
                        bool    bRead,
                        bool    bTrueColor,
                        int     width,
                        int     height,
                        int     fps )
{
  if (fileName == 0)
    return false;
  if ((width <= 0) || (height <= 0))
    return false;
  if (!ALIB_IS_LIMITED( fps, 1, 100 ))
    return false;
  if (width & 3)
    return false;

  if ((m_file != 0) && !Close())
    return false;

  m_file = _tfopen( fileName, (bRead ? _T("rb") : _T("wb")) );
  if (m_file == 0)
    return false;
}


//-------------------------------------------------------------------------------------------------
/** \brief . */
//-------------------------------------------------------------------------------------------------
bool VideoBitmap::Close()
{
  if (m_file == 0)
    return true;

  return true;
}


//-------------------------------------------------------------------------------------------------
/** \brief . */
//-------------------------------------------------------------------------------------------------
bool VideoBitmap::Read( Arr2ub & frame )
{
}


//-------------------------------------------------------------------------------------------------
/** \brief . */
//-------------------------------------------------------------------------------------------------
bool VideoBitmap::Write( const Arr2ub & frame )
{
  if (!m_ok || (m_file == 0) || m_bRead || frame.empty())
    return false;
  if ((frame.width() != m_biHeader.biWidth) || (frame.height() != m_biHeader.biHeight))
    return false;
  if (fwrite( frame.begin(), frame.size()*sizeof(Arr2ub::value_type), 1, m_file ) != 1)
    return false;
  m_exHeader.m_frameNum++;
  return true;
}


//-------------------------------------------------------------------------------------------------
/** \brief . */
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
/** \brief . */
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
/** \brief . */
//-------------------------------------------------------------------------------------------------

