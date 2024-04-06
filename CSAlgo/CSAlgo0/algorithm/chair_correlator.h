/****************************************************************************
  chair_correlator.h
  ---------------------
  begin     : Aug 2004
  author(s) : Albert Akhriev
  email     : aaah@mail.ru
****************************************************************************/

#pragma once

//=================================================================================================
/** \class ChairCorrelator.
    \brief Class implements various methods of correlation between a chair and a background. */
//=================================================================================================
class CS_ALGO_API ChairCorrelator
{
  typedef  std::pair<float,float>  FFPair;
  typedef  std::vector<FFPair>     FFPairArr;

private:
  bool      m_bNCC;       //!< normalized cross-correlation measure, otherwise mean-normalized one
  bool      m_bNormL2;    //!< use L2 norm in correlation measure, otherwise L1 norm
  FFPairArr m_values;     //!< temporal storage of intensities
  Vec2i     m_correction; //!< correction of chair position

private:
  bool Correlate_NCC_L2( const Frame & frame, const Arr2f & background, const Chair & chair, float & diff );
  bool Correlate_NCC_L1( const Frame & frame, const Arr2f & background, const Chair & chair, float & diff );
  bool Correlate_Mean_L2( const Frame & frame, const Arr2f & background, const Chair & chair, float & diff );
  bool Correlate_Mean_L1( const Frame & frame, const Arr2f & background, const Chair & chair, float & diff );

public:
  ChairCorrelator();
  void Clear();
  bool Initialize( const csalgo::AlgorithmParams & params );
  bool Correlate( const Frame & frame, const Arr2f & background, const Chair & chair,
                  bool bCorrectPos, float & diff );
};

