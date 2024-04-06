/******************************************************************************
  k_harmonic_mean_clustering.h
  ---------------------
  begin     : Aug 2005
  modified  : 3 Aug 2005
  author(s) : Albert Akhriev
  copyright : Albert Akhriev
  email     : aaah@mail.ru, Albert.Akhriev@biones.com
******************************************************************************/

#pragma once

//=================================================================================================
/** \class .
    \brief . */
//=================================================================================================
template< class SAMPLE, class SQ_DISTANCE_FUNC >
class KHarmonicMeanClusterizer
{
public:
  typedef  typename SAMPLE::value_type  value_type;
  typedef  std::vector<value_type>      Center;
  typedef  std::vector<Center>          CenterArr;

private:
  struct DistanceAndPtr
  {
    double         dist;     //!< distance to the cluster center
    const SAMPLE * ptr;      //!< pointer to a sample vector

    /** \brief Operator is used to sort in descendant order. */
    bool operator<( const DistanceAndPtr & x ) const { return (dist > x.dist); }
  };

  typedef  std::vector<double>          DblVec;
  typedef  std::vector<DblVec>          DblVecArr;
  typedef  std::vector<DistanceAndPtr>  DPArr;

  CenterArr m_centers;       //!< resultant cluster centers
  DblVec    m_q;             //!< temporal vector
  DblVec    m_qSum;          //!< temporal vector
  DblVecArr m_curMeans;      //!< temporal current cluster centers
  DblVecArr m_preMeans;      //!< temporal previous cluster centers
  DPArr     m_dpArr;         //!< temporal array used for sorting

private:
//-------------------------------------------------------------------------------------------------
/** \brief Function . */
//-------------------------------------------------------------------------------------------------
void UpdateMeanVectors( const SAMPLE * F, const SAMPLE * L, const int dim, const int K )
{
  const double     MIN_SQ_DISTANCE = FLT_MIN/(FLT_EPSILON*FLT_EPSILON);
  SQ_DISTANCE_FUNC SqDist;
  const int        N = (int)(std::distance( F, L ));

  for (int c = 0; c < K; c++)
  {
    (m_curMeans[c]).swap( m_preMeans[c] );
    alib::Fill( m_curMeans[c], double(0) );
    m_qSum[c] = 0.0;
  }

  // For all sample vectors ..., n = [0...(N-1)].
  for (const SAMPLE * x = F; x != L; ++x)
  {
    double revQSum = 0.0;

    for (int c = 0; c < K; c++)
    {
      revQSum += (m_q[c] = 1.0/(SqDist( *x, m_preMeans[c] ) + MIN_SQ_DISTANCE));
    }
    revQSum = 1.0/(revQSum*revQSum);

    for (int c = 0; c < K; c++)
    {
      m_qSum[c] += (m_q[c] = m_q[c] * sqrt( m_q[c] ) * revQSum);
      alib::AddMultiplied( *x, m_curMeans[c], m_q[c] );
    }
  }

  for (int c = 0; c < K; c++)
  {
    alib::Multiply( m_curMeans[c], (double)(alib::Reciprocal( m_qSum[c] )) );
  }
}


//-------------------------------------------------------------------------------------------------
/** \brief Function . */
//-------------------------------------------------------------------------------------------------
float AddNewCenter( const SAMPLE * F,
                    const SAMPLE * L,
                    const int      dim,
                    const int      K,
                    const float    fraction )
{
  SQ_DISTANCE_FUNC SqDist;
  double           avrMin = 0.0;
  const int        N = (int)(std::distance( F, L ));
  const int        FRACTION = (int)floor( N*fraction + 0.5 );
  int              n = 0;

  ASSERT( K < (int)(m_curMeans.size()) );
  if (FRACTION == 0)
    return 0.0;

  // Compute average minimal distance from each sample to the nearest cluster center,
  // and the maximal distances from each sample to any cluster center.
  for (const SAMPLE * x = F; x != L; ++x)
  {
    double minD2 = DBL_MAX;
    double maxD2 = 0.0;

    for (int c = 0; c < K; c++)
    {
      double d2 = SqDist( *x, m_preMeans[c] );
      minD2 = std::min( minD2, d2 );
      maxD2 = std::max( maxD2, d2 );
    }
    avrMin += sqrt( minD2 );

    DistanceAndPtr & dp = m_dpArr[n++];
    dp.dist = maxD2;
    dp.ptr = x;
  }
  ASSERT( n == N );

  // The most distant samples constitute new cluster center.
  std::sort( m_dpArr.begin(), m_dpArr.end() );
  for (n = 0; n < FRACTION; n++) { alib::Add( *((m_dpArr[n]).ptr), m_curMeans[K] ); }
  alib::Multiply( m_curMeans[K], 1.0/(double)FRACTION );

  return ((float)(avrMin/(double)N));
}


public:
//-------------------------------------------------------------------------------------------------
/** \brief Function . */
//-------------------------------------------------------------------------------------------------
void Clear()
{
  m_centers.clear();
  m_q.clear();
  m_qSum.clear();
  m_curMeans.clear();
  m_preMeans.clear();
  m_dpArr.clear();
}


//-------------------------------------------------------------------------------------------------
/** \brief Function . */
//-------------------------------------------------------------------------------------------------
KHarmonicMeanClusterizer()
{
  Clear();
}


//-------------------------------------------------------------------------------------------------
/** \brief Function . */
//-------------------------------------------------------------------------------------------------
~KHarmonicMeanClusterizer()
{
  Clear();
}


//-------------------------------------------------------------------------------------------------
/** \brief Function . */
//-------------------------------------------------------------------------------------------------
const CenterArr & GetMeanVectors() const
{
  return m_centers;
}


//-------------------------------------------------------------------------------------------------
/** \brief Function . */
//-------------------------------------------------------------------------------------------------
bool Run( const SAMPLE * F,
          const SAMPLE * L,
          const int      dim,
          const int      maxClusterNum,
          const int      maxIterNum = 10000,
          const float    distThreshold = 0.0f,
          const float    fraction = 0.1f )
{
  Clear();
  if ((F == 0) || (L == 0) || (F >= L) || (dim < 1) || (maxClusterNum < 1))
    return false;
  if (std::numeric_limits<value_type>::is_specialized == false)
    return false;

  const int N = (int)(std::distance( F, L ));
  int       K = 0;

  m_centers.reserve( maxClusterNum );
  m_q.resize( maxClusterNum );
  m_qSum.resize( maxClusterNum );
  m_preMeans.resize( maxClusterNum );
  m_curMeans.resize( maxClusterNum );
  for (int c = 0; c < maxClusterNum; c++) { (m_preMeans[c]).resize( dim, double(0) ); }
  for (int c = 0; c < maxClusterNum; c++) { (m_curMeans[c]).resize( dim, double(0) ); }
  m_dpArr.resize( N );

  // Compute initial mean vector.
  {
    for (const SAMPLE * x = F; x != L; ++x)
    {
      if ((int)(x->size()) != dim)
        return false;
      alib::Add( *x, m_curMeans[0] );
    }
    alib::Multiply( m_curMeans[0], 1.0/(double)N );
  }

  // Main loop. Gradually increase cluster number until it either attains the maximum
  // or the average distance to the nearest cluster center falls below threshold.
  for (K = 1; K <= maxClusterNum; K++)
  {
    for (int iter = 0; iter < maxIterNum; iter++)
      UpdateMeanVectors( F, L, dim, K );

    if (K < maxClusterNum)
    {
      if (AddNewCenter( F, L, dim, K, fraction ) < distThreshold)
        break;
    }
  }
  K = std::min<int>( K, maxClusterNum );

  // Completely remove temporal buffers from memory.
  alib::CompleteClear( m_q );
  alib::CompleteClear( m_qSum );
  alib::CompleteClear( m_preMeans );
  alib::CompleteClear( m_dpArr );

  // Store result.
  m_centers.resize( K );
  for (int c = 0; c < K; c++)
  {
    Center       & m = m_centers[c];
    const DblVec & s = m_curMeans[c];

    m.resize( dim );
    if (std::numeric_limits<value_type>::is_integer)
    {
      const value_type vmin = std::numeric_limits<value_type>::min();
      const value_type vmax = std::numeric_limits<value_type>::max();

      for (int i = 0; i < dim; i++)
      {
        double t = floor( s[i] + 0.5 );
        if (t <= vmin) t = vmin;
        if (t >= vmax) t = vmax;
        m[i] = (value_type)t;
      }
    }
    else alib::Copy( s, m );
  }
  alib::CompleteClear( m_curMeans );
  return true;
}

};

