/****************************************************************************
  background_accum.h
  ---------------------
  begin     : Aug 2004
  author(s) : Albert Akhriev
  email     : aaah@mail.ru
****************************************************************************/

#pragma once

//=================================================================================================
/** \class BackgroundAccumulator.
    \brief Implementation of background accumulator. */
//=================================================================================================
class BackgroundAccumulator : public IBackgroundAccumulator
{
private:
  Arr2f            m_background; //!< accumulated background
  Frame            m_currFrame;  //!< the current frame
  int              m_frameNum;   //!< the number of accumulated frames
  CCriticalSection m_locker;     //!< the object protects background from unsynchronized modification
  bool             m_bRunning;   //!< state flag: non-zero after Start() invocation and zero after Stop() invocation
  IDebugOutput *   m_pDebugObj;  //!< pointer to the debug output object

public:
  BackgroundAccumulator();
  virtual ~BackgroundAccumulator();

  virtual HRESULT Start();

  virtual HRESULT ProcessImage( IN const BITMAPINFO * pBI,
                                IN const void       * pBytes );

  virtual HRESULT Stop( OUT const Arr2f ** ppBackground );

  virtual HRESULT Release();

  virtual void SetDebugOutput( IDebugOutput * pDebug );
};

