/****************************************************************************
  camera_analyzer.h
  ---------------------
  begin     : Aug 2004
  author(s) : Albert Akhriev
  email     : aaah@mail.ru
****************************************************************************/

#pragma once

//#include "algo_data_transfer_types.h"
#include "algo_param.h"
#include "image_acquirer.h"
#include "chair_correlator.h"

//=================================================================================================
/** \class CameraAnalyzer.
    \brief Implementation of camera analyzer. */
//=================================================================================================
class CameraAnalyzer : public ICameraAnalyzer
{
protected:
  typedef csutility::AutoInterfacePtr<csinterface::ILearningAlgorithm> ILearningAlgoPtr;

  bool                m_bGood;             //!< state flag: nonzero if computational process is successful
  bool                m_bRunning;          //!< state flag: nonzero after Start() invocation and zero after Stop() invocation
  bool                m_bInitialized;      //!< state flag: nonzero if the object has been properly initialized
                     
  int                 m_cameraNo;          //!< the index of camera that covers control zone of interest
  int                 m_frameNo;           //!< the index of the current frame
  int                 m_nDropFrame;        //!< the number of dropped frames
                     
  __int64             m_startTime;         //!< absolute starting time in milliseconds
  float               m_previousTime;      //!< relative previous time in seconds, counted off from the process beginning
  float               m_currentTime;       //!< relative current time in seconds, counted off from the process beginning
                     
  CCriticalSection    m_dataLocker;        //!< object locks/unlocks the data being transferred between threads
  csalgo::Parameters  m_parameters;        //!< the object keeps parameter settings
  ChairArr            m_chairs;            //!< chair state data and precomputed chair positions
  ImageAcquirer       m_imgAcquirer;       //!< the object acquires and gauges raw video data
  ChairCorrelator     m_chairCorrelator;   //!< the object correlates a chair with a static or a dynamic background
                     
  Arr2f               m_staticBackground;  //!< precomputed static backgrounds (empty hall frames)
  Arr2f               m_dynamicBackground; //!< dynamically updated background
  Frame               m_currentFrame;      //!< the current frame is the sum of several latest ones
                     
  IDebugOutput *      m_pDebugOutput;      //!< pointer to the debug output object
  StdStr              m_lastError;         //!< textual descriptor of the last error
  StdStr              m_logFileName;       //!< the name of log file
  std::fstream        m_log;               //!< log file stream
  UByteArr            m_tempBuffer;        //!< temporal buffer

  HINSTANCE           m_hLearnLib;         //!< instance of online learning library
  ILearningAlgoPtr    m_pLearnAlgo;        //!< pointer to the learning algorithm instance

protected:
  void Clear();
  void PrintException( const alib::GeneralError & e );
  void ProcessLastStaticInterval( Chair & chair );
  void ProcessLastDynamicInterval( Chair & chair );

  virtual bool DoPreprocessing();
  virtual bool DoProcessing();
  virtual bool DoPostprocessing();

public:
  CameraAnalyzer();
  virtual ~CameraAnalyzer();

  virtual HRESULT Initialize( IN const Int8Arr      * pParameters,
                              IN const BaseChairArr * pChairs,
                              IN const Arr2f        * pBackground,
                              IN __int32              cameraNo,
                              IN IDebugOutput       * pDebugOut );

  virtual HRESULT Start( IN __int64 startTime );

  virtual HRESULT ProcessImage( IN const BITMAPINFO * pBI,
                                IN const void       * pBytes,
                                IN __int64            timeStamp );

  virtual HRESULT Stop( IN __int64 stopTime );

  virtual HRESULT Release();

  virtual HRESULT SetData( IN csinterface::IDataType * pData );

  virtual HRESULT GetData( OUT csinterface::IDataType * pData ) const;
};

