/****************************************************************************
  algorithm.h
  ---------------------
  begin     : Aug 2004
  author(s) : Albert Akhriev
  email     : aaah@mail.ru
****************************************************************************/

#pragma once

#include "../../CSAlgo/algorithm/algo_data_transfer_types.h"
#include "chair_ex.h"
#include "learn_algo_param.h"

//=================================================================================================
/** \class DynamicLearningAlgo.
    \brief Implementation of camera analyzer. */
//=================================================================================================
class DynamicLearningAlgo : public csinterface::ILearningAlgorithm
{
private:
  struct ImagePair
  {
    Arr2f slowFrame;
    Arr2f fastFrame;
    float slowDecayTau;
    float fastDecayTau;
    float slowFrameNoise;
    float fastFrameNoise;
  };

  struct Histogram
  {
    enum { SIZE = 256 };
    int intervalNum [2][SIZE];
    int dynaDuration[2][SIZE];
    int calmDuration[2][SIZE];
  };

  struct ParamSet
  {
    float misclassProb;
    float slowDecayTau;
    float fastDecayTau;
    float threshold;
    int   imagePairNo;
    int   thresholdNo;

    bool operator<( const ParamSet & x ) const { return (misclassProb < x.misclassProb); }
  };

  typedef  std::vector<ImagePair>      ImagePairArr;
  typedef  Arr2D<Histogram,Histogram>  HistogramMap;
  typedef  std::vector<ParamSet>       ParamSetArr;

  bool             m_bGood;        //!< state flag: nonzero if computational process is successful
  bool             m_bRunning;     //!< state flag: nonzero after Start() invocation and zero after Stop() invocation
                   
  int              m_frameNo;      //!< the index of the current frame
  int              m_nDropFrame;   //!< the number of dropped frames
                   
  float            m_startTime;    //!< absolute starting time in milliseconds
  float            m_previousTime; //!< relative previous time in seconds, counted off from the process beginning
  float            m_currentTime;  //!< relative current time in seconds, counted off from the process beginning

  CCriticalSection m_dataLocker;   //!< object locks/unlocks the data being transferred between threads
  Parameters       m_parameters;   //!< the object keeps parameter settings
  ChairExArr       m_chairs;       //!< chair state data and precomputed chair positions
  ImageAcquirer    m_imgAcquirer;  //!< the object acquires and gauges raw video data
  ImagePairArr     m_images;       //!< 
  FloatArr         m_thresholds;   //!< 
  HistogramMap     m_histograms;   //!< 

  std::wstring     m_resultPath;   //!< 
  QImage           m_demoImage;    //!< 

public:
  csinterface::IInformationQuery * m_pInfo;        //!< external information source
  IDebugOutput                   * m_pOutput;      //!< pointer to the debug output object
  std::wstring                     m_lastError;    //!< textual descriptor of the last error
  std::wstring                     m_logFileName;  //!< the name of log file
  std::fstream                     m_log;          //!< log file stream

protected:            
  void  Clear();       
  void  PrintException( const std::runtime_error & e );
  float Correlate( const Arr2f & image1, const Arr2f & image2, const Chair & chair );
  void  AddToHistogram( ChairEx::Statistics & stat, bool bHuman, int imagePairNo, int thresholdNo );
  void  LoadOrSaveHistograms( bool bLoad );

  virtual bool DoPreprocessing();
  virtual bool DoProcessing();
  virtual bool DoPostprocessing();

  bool IsProperChairType( int type ) const
  {
    return ((type == CHAIR_EMPTY) || (type == CHAIR_CLOTHES) || (type == CHAIR_HUMAN));
  }

  void FinalizeInterval( ChairEx::Statistics & stat ) const
  {
    float interval = (m_currentTime - stat.startTime);
    if (stat.activity)
    {
      if (interval > m_parameters.s_learn.p_minDynaInterval())
      {
        stat.dynaPeriodNum++;
        stat.dynaDuration += interval;
      }
    }
    else // if (non-active)
    {
      if (interval > m_parameters.s_learn.p_minCalmInterval())
      {
        stat.calmPeriodNum++;
        stat.calmDuration += interval;
      }
    }
    stat.startTime = m_currentTime;
    stat.activity = !(stat.activity);
  }

public:
  DynamicLearningAlgo();
  virtual ~DynamicLearningAlgo();

  virtual HRESULT Start( IN float startTime );

  virtual HRESULT ProcessImage( IN const BITMAPINFO * pBI, IN const __int8 * pImage, IN float timeStamp );

  virtual HRESULT Stop( IN float stopTime, IN bool saveResult );

  virtual HRESULT Release();

  virtual HRESULT SetData( IN const csinterface::IDataType * pData );

  virtual HRESULT GetData( OUT csinterface::IDataType * pData ) const;

  virtual HRESULT QueryData( IN OUT csinterface::IDataType * pData ) const;
};

