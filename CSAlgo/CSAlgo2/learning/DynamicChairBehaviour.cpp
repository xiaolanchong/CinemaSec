//////////////////////////////////////////////////////////////////////////
//DynamicChairBehaviour.cpp
//author : Alexander Boltnev
//e-mail : Alexander.Boltnev@biones.com
//date   : 30 june 2005  
//revised: Albert Akhriev, 19 Jul 2005, Albert.Akhriev@biones.com
//////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "DynamicChairBehaviour.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

DynamicChairBehaviour::DynamicChairBehaviour(void)
{
  Reset( true );
}


DynamicChairBehaviour::~DynamicChairBehaviour(void)
{
}


void DynamicChairBehaviour::Update( const Arr2f & image, const BaseChairEx & chair )
{
  if ((m_timeMeans.size() != chair.points.size()) || (m_timeSquares.size() != chair.points.size()))
  {
    Reset( m_bMeanNorm );
    m_timeMeans.resize( chair.points.size(), double(0.0) );
    m_timeSquares.resize( chair.points.size(), double(0.0) );
  }

  // Accumulate time means and time squares.
  m_normalizer.Initialize( image, chair.points, m_bMeanNorm );
  for (int i = 0, n = (int)(chair.points.size()); i < n; i++)
  {
    float t = m_normalizer.GetNormalizedValue( image[ chair.points[i] ] );
    m_timeMeans[i] += t;
    m_timeSquares[i] += t*t;
  }

	++m_frameCnt;
}


void DynamicChairBehaviour::Reset( bool bMeanNormalized )
{
  m_frameCnt = 0;
  m_timeMeans.clear();
  m_timeSquares.clear();
	m_bMeanNorm = bMeanNormalized;
  m_normalizer.Clear();
}


float DynamicChairBehaviour::GetDynamicCharacteristic() const
{
  double avrSqTimeMean = 0.0, avrTimeSquare = 0.0;
  int    N = (int)(m_timeMeans.size());

	ASSERT( N == (int)(m_timeSquares.size()) );
  if ((m_frameCnt == 0) || (N == 0))
    return 0.0f;

  // Perform averaging over chair region.
	for (int i = 0; i < N; i++)
	{
		avrSqTimeMean += m_timeMeans[i] * m_timeMeans[i];
		avrTimeSquare += m_timeSquares[i];
	}

	avrSqTimeMean /= (double)(m_frameCnt*m_frameCnt);
	avrTimeSquare /= (double)m_frameCnt;

	return ((float)sqrt( (avrTimeSquare - avrSqTimeMean) / (double)N ));
}

