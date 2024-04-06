///////////////////////////////////////////////////////////////////////////////////////////////////
// DynamicAccumulator.h
// ---------------------
// begin     : 30 Jun 2005
// modified  : 14 Sep 2005
// author(s) : Alexander Boltnev, Albert Akhriev
// email     : Alexander.Boltnev@biones.com, Albert.Akhriev@biones.com
///////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

//=================================================================================================
/// \brief Accumulator of activity inside a chair.
//=================================================================================================
class DynamicAccumulator
{
private:
  enum { MIN_FRAME_NUMBER = 100, HISTO_SCALE = 256, HISTO_SIZE = 512 };

  typedef  std::pair<double,double>  DDPair;
  typedef  std::vector<DDPair>       DDPairArr;

  int       m_frameCnt;                //!< frame counter
  DDPairArr m_timeMeans;               //!< 
  __int64   m_histogram[HISTO_SIZE];   //!< 

public:
  DynamicAccumulator();
  ~DynamicAccumulator();

  void Initialize( const ARect & rect );

  void Reset();

  void Update( const Arr2f & frame, const ARect & rect, const Arr2f & weights, UByteArr & buffer );

  float GetDynamicCharacteristic( const Arr2f & weights ) const;

  int GetFrameCounter() const { return m_frameCnt; }
};

