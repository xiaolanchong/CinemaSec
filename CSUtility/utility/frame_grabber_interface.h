/****************************************************************************
  frame_grabber_interface.h
  -------------------
  begin     : Oct 2004
  author(s) : A.Akhriev
  email     : aaah@mail.ru
****************************************************************************/

#ifndef __CS_FRAME_GRABBER_INTERFACE_H__
#define __CS_FRAME_GRABBER_INTERFACE_H__

//=================================================================================================
/** \struct FrameGrabberParameters.
    \brief  Some common parameters of a frame grabber. */
//=================================================================================================
/*
struct FrameGrabberParameters
{
  __int32 structSize;   //!< the size of this structure
  __int32 version;      //!< the version of this structure
  float   fps;          //!< desired frame-per-second rate
  double  fStartTime;   //!< "< 0" means realtime video

  /// \brief Constructor.
  FrameGrabberParameters()
  {
    ASSERT( sizeof(float) == 4 );
    ASSERT( sizeof(double) == 8 );

    structSize = sizeof(FrameGrabberParameters);
    version = mmioFOURCC( '1', '0', '0', '0' );
    fps = 25.0f;
    fStartTime = 0.0;
  }
};
*/


//=================================================================================================
/** \struct IFrameGrabber.
    \brief  Base class for any frame grabber.

  Working scenario:
  <tt><pre>
  IFrameGrabber * pGrabber = 0;
  CreateSomeFrameGrabber( &pGrabber );
  pGrabber->Initialize(...);
  pGrabber->Start();
  while (...)
  {
    ....
    if (pGrabber->HasFreshFrame())
    {
      pGrabber->LockFrame(...);
      // Do useful job here ...
      pGrabber->UnlockFrame(...)
    }
    ....
  }
  pGrabber->Stop();
  pGrabber->Release();
  </pre></tt> */
//=================================================================================================
struct IFrameGrabber
{

//-------------------------------------------------------------------------------------------------
/** \brief Function initializes the grabbing process.

  \param  name     the name of a file or a video-source device.
  \param  pParams  pointer to the common parameters or 0 (default ones will be used).
  \return          Ok = S_OK. */
//-------------------------------------------------------------------------------------------------
virtual HRESULT Initialize( LPCWSTR name, const FrameGrabberParameters * pParams ) = 0;


//-------------------------------------------------------------------------------------------------
/** \brief Function launches the grabbing process.

  \return  Ok = S_OK. */
//-------------------------------------------------------------------------------------------------
virtual HRESULT Start() = 0;


//-------------------------------------------------------------------------------------------------
/** \brief Function locks the latest loaded frame of a sequence and proceeds grabbing of a new one
           in asynchronous mode.

  Function is suitable for a multi-thread application. Locked frame's buffer stills inaccessible
  until the function \sa UnlockFrame() is called. Meanwhile the grabbing process
  continues filling out another buffer.

  \param  ppHeader  address of a storage that receives pointer to internal image header.
  \param  ppImage   address of a storage that receives pointer to internal image content.
  \param  pImgSize  address of a storage that receives the size of image content (in bytes).
  \return           Ok = S_OK. */
//-------------------------------------------------------------------------------------------------
virtual HRESULT LockFrame( const BITMAPINFO ** ppHeader,
                           const __int8     ** ppImage,
                           __int32          *  pImgSize ) = 0;


//-------------------------------------------------------------------------------------------------
/** \brief Function unlocks processed frame, which has been previously locked by the \sa
           LockFrame() function.

\return  Ok = S_OK. */
//-------------------------------------------------------------------------------------------------
virtual HRESULT UnlockFrame() = 0;


//-------------------------------------------------------------------------------------------------
/** \brief Function finalizes the grabbing process.

  \return  Ok = S_OK. */
//-------------------------------------------------------------------------------------------------
virtual HRESULT Stop() = 0;


//-------------------------------------------------------------------------------------------------
/** \brief Function destroys this object.

  \return  Ok = S_OK. */
//-------------------------------------------------------------------------------------------------
virtual HRESULT Release() = 0;


//-------------------------------------------------------------------------------------------------
/** \brief Function returns the state of grabbing process.

  Function returns non-zero, if the grabbing process is being successfully launched
  and running without errors.

  \return  non-zero if the object is properly working. */
//-------------------------------------------------------------------------------------------------
virtual bool IsOk() const = 0;


//-------------------------------------------------------------------------------------------------
/** \brief Function tells whether the grabbing process is over.

  \return  non-zero if the sequence is over. */
//-------------------------------------------------------------------------------------------------
virtual bool IsEnd() const = 0;


//-------------------------------------------------------------------------------------------------
/** \brief Function returns non-zero, if a new frame is available after the last invocation of 
           \sa LockFrame() and \sa UnlockFrame() functions.

  \return  non-zero if a new frame is available. */
//-------------------------------------------------------------------------------------------------
virtual bool HasFreshFrame() const = 0;

};

#endif // __CS_FRAME_GRABBER_INTERFACE_H__

