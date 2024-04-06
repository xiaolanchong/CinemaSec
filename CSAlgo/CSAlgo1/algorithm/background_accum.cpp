/****************************************************************************
  background_accum.cpp
  ---------------------
  begin     : Aug 2004
  author(s) : Albert Akhriev
  email     : aaah@mail.ru
****************************************************************************/

#include "stdafx.h"
#include "image_property.h"
#include "frame.h"
#include "algo_param.h"
#include "image_acquirer.h"
#include "background_accum.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const wchar_t BACKGROUND_ACCUMULATOR_ERROR[] = _T("Background accumulator's error\n");

//-------------------------------------------------------------------------------------------------
/** \brief Function creates an instance of camera analyzer.

  \param  ppAccumulator  the address of pointer that points to the instance of created object.
  \return                Ok = S_OK. */
//-------------------------------------------------------------------------------------------------
HRESULT CreateBackgroundAccumulator( IBackgroundAccumulator ** ppAccumulator )
{
  if (ppAccumulator == 0)
    return E_FAIL;
  (*ppAccumulator) = new BackgroundAccumulator();
  return ((*ppAccumulator) != 0) ? S_OK : E_FAIL;
}


//-------------------------------------------------------------------------------------------------
/** \brief Constructor. */
//-------------------------------------------------------------------------------------------------
BackgroundAccumulator::BackgroundAccumulator()
{
  m_frameNum = 0;
  m_bRunning = false;
  m_pDebugObj = 0;
}


//-------------------------------------------------------------------------------------------------
/** \brief Destructor. */
//-------------------------------------------------------------------------------------------------
BackgroundAccumulator::~BackgroundAccumulator()
{
}


//-------------------------------------------------------------------------------------------------
/** \brief Начало работы аккумулятора.

  \return  Ok = S_OK. */
//-------------------------------------------------------------------------------------------------
HRESULT BackgroundAccumulator::Start()
{
  CSingleLock lock( &m_locker, TRUE );
  m_background.clear();
  m_currFrame.clear();
  m_frameNum = 0;
  m_bRunning = true;
  return S_OK;
}


//-------------------------------------------------------------------------------------------------
/** \brief Передать очередное изображение для аккумулятора и обработать его.

  \param  pBI     указатель на заголовок изображения.
  \param  pBytes  указатель на начало изображения.
  \return         Ok = S_OK. */
//-------------------------------------------------------------------------------------------------
HRESULT BackgroundAccumulator::ProcessImage( IN const BITMAPINFO * pBI,
                                             IN const void       * pBytes )
{
  if (m_frameNum > (1<<12))
    return S_OK;

  HRESULT       retValue = E_FAIL;
  CSingleLock   lock( &m_locker, TRUE );
  ImageProperty property;

  try
  {
    ALIB_ASSERT( (pBI != 0) && (pBytes != 0) && m_bRunning );
    ALIB_ASSERT( m_background.empty() || ((m_background.width() == pBI->bmiHeader.biWidth) &&
                                          (m_background.height() == abs( pBI->bmiHeader.biHeight ))) );
    ALIB_ASSERT( alib::AreDimensionsEqual( m_currFrame, m_background ) );

    if (m_background.empty())
    {
      m_background.resize( pBI->bmiHeader.biWidth, abs( pBI->bmiHeader.biHeight ), 0.0f );
      m_currFrame.resize( pBI->bmiHeader.biWidth, abs( pBI->bmiHeader.biHeight ), 0.0f );
    }

    ALIB_ASSERT( ImageAcquirer::GaugeAndConvertImage( &(pBI->bmiHeader),
                                                      reinterpret_cast<LPCUBYTE>( pBytes ),
                                                      m_currFrame, property ) );

    std::transform( m_background.begin(), m_background.end(), m_currFrame.begin(),
                    m_background.begin(), std::plus<float>() );
    ++m_frameNum;

    retValue = S_OK;
  }
  catch (std::runtime_error & e)
  {
    CString msg( BACKGROUND_ACCUMULATOR_ERROR );
    msg += CString( e.what() );
    if (m_pDebugObj != 0) m_pDebugObj->PrintW( IDebugOutput::mt_error, (LPCWSTR)msg );
  }
  catch (...)
  {
    if (m_pDebugObj != 0) m_pDebugObj->PrintW( IDebugOutput::mt_error, ALIB_UNSUPPORTED_EXCEPTION );
  }
  return retValue;
}


//-------------------------------------------------------------------------------------------------
/** \brief Остановка работы аккумулятора.

  \param  ppBackground  the address of pointer that will point to the internal background's storage.
  \return               Ok = S_OK. */
//-------------------------------------------------------------------------------------------------
HRESULT BackgroundAccumulator::Stop( OUT const Arr2f ** ppBackground )
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
    if (m_pDebugObj != 0) m_pDebugObj->PrintW( IDebugOutput::mt_error, (LPCWSTR)msg );
  }
  catch (...)
  {
    if (m_pDebugObj != 0) m_pDebugObj->PrintW( IDebugOutput::mt_error, ALIB_UNSUPPORTED_EXCEPTION );
  }
  m_bRunning = false;
  return retValue;
}


//-------------------------------------------------------------------------------------------------
/** \brief Function releases interface instance.

  \return  Ok = S_OK. */
//-------------------------------------------------------------------------------------------------
HRESULT BackgroundAccumulator::Release()
{
  delete this;
  return S_OK;
}


//-------------------------------------------------------------------------------------------------
/** \brief Function stores pointer to an object that prints debug information.

  \param  pDebug  pointer to the debug output object. */
//-------------------------------------------------------------------------------------------------
void BackgroundAccumulator::SetDebugOutput( IDebugOutput * pDebug )
{
  m_pDebugObj = pDebug;
}

