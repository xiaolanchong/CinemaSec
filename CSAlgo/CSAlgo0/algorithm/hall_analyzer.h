/****************************************************************************
  hall_analyzer.h
  ---------------------
  begin     : Aug 2004
  author(s) : Albert Akhriev
  email     : aaah@mail.ru
****************************************************************************/

#pragma once

#include "chair.h"
#include "human_counter.h"

//=================================================================================================
/** \class HallAnalyzer.
    \brief Implementation of hall analyzer. */
//=================================================================================================
class HallAnalyzer : public IHallAnalyzer
{
protected:
  bool               m_bGood;            //!< state flag: non-zero if computational process is currently successful
  ChairArr           m_chairs;           //!< the data associated with all hall's chairs
  ChairArr           m_tmpChairs;        //!< temporal chairs
  CCriticalSection   m_dataLocker;       //!< object locks/unlocks the data being transferred between threads
  csalgo::Parameters m_parameters;       //!< the object keeps parameter settings
  __int64            m_startTime;        //!< absolute starting time in milliseconds
  float              m_currentTime;      //!< relative current time in seconds, counted off from the process beginning
  HumanCounterMap    m_cameraHuman;      //!< spectator number counters of each camera

  IDebugOutput *     m_pDebugOutput;     //!< pointer to the debug output object
  StdStr             m_lastError;        //!< textual descriptor of the last error
  StdStr             m_logFileName;      //!< the name of log file
  std::fstream       m_log;              //!< log file stream
  bool               m_bFinalStatistics; //!< if nonzero, then final statistics should be printed in the log file

protected:
  void PrintException( const std::runtime_error & e ) const;

public:
  HallAnalyzer();
  virtual ~HallAnalyzer();

  virtual HRESULT Initialize( IN const Int8Arr      * pParameters,
                              IN const BaseChairArr * pChairs,
                              IN __int64              startTime,
                              IN IDebugOutput       * pDebugOut );

  virtual HRESULT ProcessData( OUT __int32 * spectatorNum,
                               IN __int64    timeStamp,
                               IN __int64    period );

  virtual HRESULT Release();

  virtual HRESULT SetData( IN csinterface::IDataType * pData );

  virtual HRESULT GetData( OUT csinterface::IDataType * pData ) const;
};

