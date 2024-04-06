/**************************************************************************************************
              base_frame_grabber.h - Declaration of the base class of any frame grabber.
                  -------------------
  begin     : Oct 2004
  author(s) : A.Akhriev
  email     : aaah@mail.ru, Albert.Akhriev@biones.com
**************************************************************************************************/

#ifndef BASE_FRAME_GRABBER_H
#define BASE_FRAME_GRABBER_H

//=================================================================================================
/** \class FrameGrabberParameters.
    \brief Parameters of a frame grabber. */
//=================================================================================================
class FrameGrabberParameters
{
public:
  float  fps;         //!< desired frame-per-second rate
  double fStartTime;  // "< 0" realtime video

  FrameGrabberParameters()
  {
    memset( this, 0, sizeof(FrameGrabberParameters) );
    fps = 25.0f;
  }
};


//=================================================================================================
/** \class BaseFrameGrabber.
    \brief Base class for any frame grabber. */
//=================================================================================================
class BaseFrameGrabber
{
public:

//-------------------------------------------------------------------------------------------------
/** \brief Destructor. */
//-------------------------------------------------------------------------------------------------
virtual ~BaseFrameGrabber(){};


//-------------------------------------------------------------------------------------------------
/** \brief Function initializes the grabbing process.

  \param  name   the name of a file or a video-source device.
  \param  param  parameters required to initialize the grabber.
  \return        Ok = true. */
//-------------------------------------------------------------------------------------------------
virtual bool Start( LPCTSTR name, const FrameGrabberParameters & param ) = 0;


//-------------------------------------------------------------------------------------------------
/** \brief Function finalizes the grabbing process.

  \return  Ok = true. */
//-------------------------------------------------------------------------------------------------
virtual bool Stop() = 0;


//-------------------------------------------------------------------------------------------------
/** \brief Function returns the state of grabbing process.

  Function returns non-zero, if the grabbing process is being successfully launched
  and running without errors.

  \return  Ok = true. */
//-------------------------------------------------------------------------------------------------
virtual bool IsOk() const = 0;


//-------------------------------------------------------------------------------------------------
/** \brief Function tells whether the grabbing process is over.

  \return  true, if the sequence is over. */
//-------------------------------------------------------------------------------------------------
virtual bool IsEnd() const = 0;


//-------------------------------------------------------------------------------------------------
/** \brief Function locks the latest loaded frame of a sequence and proceeds grabbing of a new one
           in asynchronous mode.

  Function is suitable for a multi-thread application. Locked frame's buffer stills inaccessible
  until the function \sa UnlockProcessedFrame() is called. Meanwhile the grabbing process
  continues filling out another buffer.

  \param  pHeader  reference to the constant pointer that points to the internal storage
                   of BITMAPINFO header of sequence's frames.
  \param  pImage   reference to the constant pointer (const unsigned char *)
                   that points to the frame's content.
  \param  imgSize  reference to the storage of frame's content size (in bytes).
  \return          Ok = true. */
//-------------------------------------------------------------------------------------------------
virtual bool LockLatestFrame( LPCBITMAPINFO & pHeader, LPCUBYTE & pImage, int & imgSize ) = 0;


//-------------------------------------------------------------------------------------------------
/** \brief Function unlocks processed frame, which has been previously locked by the \sa
           LockLatestFrame() function.

  \return  Ok = true. */
//-------------------------------------------------------------------------------------------------
virtual bool UnlockProcessedFrame() = 0;


//-------------------------------------------------------------------------------------------------
/** \brief Function returns non-zero, if a new frame is available after the last invocation of 
           \sa LockLatestFrame() function.

  \return  non-zero, if a new frame is available. */
//-------------------------------------------------------------------------------------------------
virtual bool HasFreshFrame() const = 0;

};


//=================================================================================================
/** \class BaseFrameGrabberEx.
    \brief BaseFrameGrabberEx. */
//=================================================================================================
class BaseFrameGrabberEx : public BaseFrameGrabber
{
public:
  virtual DWORD GetVersion() = 0;
  virtual DWORD GetSize( DWORD & x, DWORD & y ) = 0;
  virtual DWORD GetPos( float & pos ) = 0;
};

#endif // BASE_FRAME_GRABBER_H

