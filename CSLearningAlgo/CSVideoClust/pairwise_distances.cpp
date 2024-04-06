///////////////////////////////////////////////////////////////////////////////////////////////////
// pairwise_distances.cpp
// ---------------------
// begin     : Aug 2004
// modified  : 17 Nov 2005
// author(s) : Albert Akhriev
// email     : Albert.Akhriev@biones.com
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "videoclust.h"
#include "../../CSAlgo/CSAlgo2/algorithm/SubImage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//-------------------------------------------------------------------------------------------------
/** \brief Function computes pairwise distances between all sample images.

  \param  fileName    the name of source AVD-file that containes a sequence of chair images.
  \param  pImages     pointer to the destination image array.
  \param  pWeights    pointer to the mask of weights of image points.
  \param  pDistances  pointer to the destination storage of pairwise distances.
  \param  bNormL1     normalize image by L1 norm if nonzero, otherwise by L2 norm.
  \param  pNotifier   message handler. */
//-------------------------------------------------------------------------------------------------
void CalcPairwiseDistances( const std::vector<Arr2ub> * pImages,
                            const Arr2f               * pWeights,
                            PairwiseDistances         * pDistances,
                            const bool                  bNormL1,
                            alib::IProcessNotify      * pNotifier ) throw(...)
{
  ALIB_ASSERT( (pImages != 0) && (pWeights != 0) && (pDistances != 0) );

  ATimer    timer;
  const int frameNum = (int)(pImages->size());
  Arr2f     image1, image2;

  timer.Start();
  pDistances->resize( frameNum, float(0), true );
  ALIB_ASSERT( (int)(pImages->size()) == (int)(pDistances->dim()) );
  if (pNotifier != 0) pNotifier->start( _T("computing pairwise distances: ... "), 0, 0 );

  ALIB_ASSERT( !(pWeights->empty()) );
  image1.resize2( *pWeights );
  image2.resize2( *pWeights );

  for (int i = 0; i < frameNum; i++)
  {
    alib::Copy( (*pImages)[i], image1 );
    csalgo2::NormalizeSubImage( image1, *pWeights, bNormL1 );

    for (int k = 0; k < i; k++)
    {
      alib::Copy( (*pImages)[k], image2 );
      csalgo2::NormalizeSubImage( image2, *pWeights, bNormL1 );
      (*pDistances)( i, k ) = csalgo2::DifferenceBetweenNormalizedSubImages( image1, image2, *pWeights, bNormL1 );
    }
  }

  timer.Stop();
  if (pNotifier != 0) { pNotifier->finished( 0, 0, 0 );  pNotifier->print( timer.TimeStr() ); }
}

