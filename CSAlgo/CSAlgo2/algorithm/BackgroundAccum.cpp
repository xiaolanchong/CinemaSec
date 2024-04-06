///////////////////////////////////////////////////////////////////////////////////////////////////
// background_accum.cpp
// ---------------------
// begin     : Aug 2004
// modified  : 24 Aug 2005
// author(s) : Albert Akhriev
// email     : Albert.Akhriev@biones.com
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ImageAcquirer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const wchar_t BACKGROUND_ACCUMULATOR_ERROR[] = L"Background accumulator's error\n";

//=================================================================================================
/** \brief Implementation of background accumulator. */
//=================================================================================================
class BackgroundAccumulator : public IBackgroundAccumulator
{
private:
  Arr2f                  m_background; //!< accumulated background
  int                    m_frameNum;   //!< the number of accumulated frames
  CCriticalSection       m_locker;     //!< the object protects background from unsynchronized modification
  bool                   m_bRunning;   //!< state flag: non-zero after Start() invocation and zero after Stop() invocation
  IDebugOutput *         m_pOutput;    //!< pointer to a message handler
  csalgo2::ImageAcquirer m_acquirer;   //!< image acquirer

public:
///////////////////////////////////////////////////////////////////////////////////////////////////
/** \brief Constructor. */
///////////////////////////////////////////////////////////////////////////////////////////////////
BackgroundAccumulator()
{
  m_frameNum = 0;
  m_bRunning = false;
  m_pOutput = 0;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/** \brief Destructor. */
///////////////////////////////////////////////////////////////////////////////////////////////////
~BackgroundAccumulator()
{
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/** \brief Начало работы аккумулятора.

  \return  Ok = S_OK. */
///////////////////////////////////////////////////////////////////////////////////////////////////
virtual HRESULT Start()
{
  CSingleLock lock( &m_locker, TRUE );
  m_background.clear();
  m_frameNum = 0;
  m_bRunning = true;
  return S_OK;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/** \brief Передать очередное изображение для аккумулятора и обработать его.

  \param  pBI     указатель на заголовок изображения.
  \param  pBytes  указатель на начало изображения.
  \return         Ok = S_OK. */
///////////////////////////////////////////////////////////////////////////////////////////////////
virtual HRESULT ProcessImage( IN const BITMAPINFO * pBI, IN const void * pBytes )
{
  if (m_frameNum > (1<<12))
    return S_OK;

  HRESULT     retValue = E_FAIL;
  CSingleLock lock( &m_locker, TRUE );

  try
  {
    ALIB_ASSERT( (pBI != 0) && (pBytes != 0) && m_bRunning );
    ALIB_ASSERT( m_acquirer.Acquire( pBI, reinterpret_cast<LPCUBYTE>( pBytes ) ) );

    if (m_background.empty())
      m_background.resize2( m_acquirer.GetRawFrame() );
    else
      ALIB_ASSERT( alib::AreDimensionsEqual( m_background, m_acquirer.GetRawFrame() ) );

    std::transform( m_background.begin(), m_background.end(), m_acquirer.GetRawFrame().begin(),
                    m_background.begin(), std::plus<float>() );
    ++m_frameNum;

    retValue = S_OK;
  }
  catch (std::runtime_error & e)
  {
    CString msg( BACKGROUND_ACCUMULATOR_ERROR );
    msg += CString( e.what() );
    if (m_pOutput != 0) m_pOutput->PrintW( IDebugOutput::mt_error, (LPCWSTR)msg );
  }
  return retValue;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/** \brief Остановка работы аккумулятора.

  \param  ppBackground  the address of pointer that will point to the internal background's storage.
  \return               Ok = S_OK. */
///////////////////////////////////////////////////////////////////////////////////////////////////
virtual HRESULT Stop( OUT const Arr2f ** ppBackground )
{
  CSingleLock lock( &m_locker, TRUE );
  HRESULT     retValue = E_FAIL;

  try
  {
    ALIB_ASSERT( (ppBackground != 0) && m_bRunning );
    float scale = (m_frameNum > 0) ? (1.0f/(float)m_frameNum) : 0.0f;
    alib::Multiply( m_background.begin(), m_background.end(), scale );
    (*ppBackground) = &m_background;
    retValue = S_OK;
  }
  catch (std::runtime_error & e)
  {
    CString msg( BACKGROUND_ACCUMULATOR_ERROR );
    msg += CString( e.what() );
    if (m_pOutput != 0) m_pOutput->PrintW( IDebugOutput::mt_error, (LPCWSTR)msg );
  }
  m_bRunning = false;
  return retValue;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/** \brief Function destroys this object.

  \return  Ok = S_OK. */
///////////////////////////////////////////////////////////////////////////////////////////////////
virtual HRESULT Release()
{
  delete this;
  return S_OK;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/** \brief Function stores pointer to an object that prints debug information.

  \param  pDebug  pointer to the debug output object. */
///////////////////////////////////////////////////////////////////////////////////////////////////
virtual void SetDebugOutput( IDebugOutput * pDebug )
{
  m_pOutput = pDebug;
}

};


///////////////////////////////////////////////////////////////////////////////////////////////////
/** \brief Function creates instance of background accumulator.

  \param  ppAccumulator  the address of pointer that points to the instance of created object.
  \return                Ok = S_OK. */
///////////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CreateBackgroundAccumulator( IBackgroundAccumulator ** ppAccumulator )
{
  if (ppAccumulator == 0)
    return E_FAIL;
  (*ppAccumulator) = new BackgroundAccumulator();
  return (((*ppAccumulator) != 0) ? S_OK : E_FAIL);
}

