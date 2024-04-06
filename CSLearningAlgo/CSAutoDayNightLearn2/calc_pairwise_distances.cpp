///////////////////////////////////////////////////////////////////////////////////////////////////
// calc_pairwise_distances.cpp
// ---------------------
// begin     : Aug 2004
// modified  : 8 Dec 2005
// author(s) : Albert Akhriev
// email     : Albert.Akhriev@biones.com
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function computes pairwise distances between histograms of any set
///        using Kolmogorov test.
///
/// All input histograms must have the same number of entries and the same number of samples.
///
/// \param  pHistograms  pointer to the array of (non-normalized) histograms.
/// \param  pDistances   pointer to the destination storage of pairwise distances.
///////////////////////////////////////////////////////////////////////////////////////////////////
void CalcPairwiseDistancesByKolmogorovTest( const std::vector< std::vector<int> > * pHistograms,
                                            TriangleStorage<float,double,true>    * pDistances )
{
  ALIB_ASSERT( (pHistograms != 0) && (pDistances != 0) && !(pHistograms->empty()) );

  const int N = (int)(pHistograms->size());
  const int H = (int)(pHistograms->front().size());

  pDistances->resize( N );
  for (int i = 0; i < N; i++)
  {
    const std::vector<int> & hi = (*pHistograms)[i];
    ALIB_ASSERT( (int)(hi.size()) == H );

    for (int j = i+1; j < N; j++)
    {
      const std::vector<int> & hj = (*pHistograms)[j];
      ALIB_ASSERT( (int)(hj.size()) == H );

      int sumi = 0, sumj = 0, diff = 0;

      for (int k = 0; k < H; k++)
      {
        sumi += hi[k];
        sumj += hj[k];
        diff = std::max( diff, abs( sumi - sumj ) );
      }

      (*pDistances)( i, j ) = (float)diff;
      ALIB_ASSERT( sumi == sumj );
    }
  }
}

