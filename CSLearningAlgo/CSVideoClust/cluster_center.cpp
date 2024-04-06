///////////////////////////////////////////////////////////////////////////////////////////////////
// cluster_center.cpp
// ---------------------
// begin     : Aug 2004
// modified  : 17 Nov 2005
// author(s) : Albert Akhriev
// email     : Albert.Akhriev@biones.com
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "videoclust.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//-------------------------------------------------------------------------------------------------
/** \brief Function computes pairwise distances between all sample images.

  \param  pCluster    pointer to the cluster (cluster is a set of indices of close elements).
  \param  pImages     pointer to the source image array.
  \param  pDistances  pointer to the storage of pairwise distances.
  \param  pCenter     pointer to the destination cluster center.
  \param  bMeanImage  synthesizes the mean image if nonzero, otherwise get the most representative one. */
//-------------------------------------------------------------------------------------------------
void CalcClusterCenter( const std::list<int>      * pCluster,
                        const std::vector<Arr2ub> * pImages,
                        const PairwiseDistances   * pDistances,
                        Arr2ub                    * pCenter,
                        const bool                  bMeanImage ) throw(...)
{
  typedef  std::list<int>  Cluster;

  ALIB_ASSERT( (pCluster != 0) && (pImages != 0) && (pDistances != 0) && (pCenter != 0) );
  ALIB_ASSERT( !(pImages->empty()) && !(pCluster->empty()) );
  ALIB_ASSERT( (int)(pImages->size()) == (int)(pDistances->dim()) );

  if (bMeanImage)
  {
    Arr2ui image;
    int    K = (int)(pCluster->size());

    pCenter->resize2( pImages->front() );
    image.resize2( pImages->front() );
    for (Cluster::const_iterator it = pCluster->begin(); it != pCluster->end(); ++it)
    {
      alib::Add( (*pImages)[*it], image );
    }

    for (int i = 0, n = pCenter->size(); i < n; i++)
    {
      (*pCenter)[i] = (ubyte)((image[i]+(K>>1))/K);
    }
  }
  else
  {
    float minDist = FLT_MAX;
    int   bestNo = -1;

    for (Cluster::const_iterator a = pCluster->begin(); a != pCluster->end(); ++a)
    {
      float dist = 0.0;

      for (Cluster::const_iterator b = pCluster->begin(); b != pCluster->end(); ++b)
      {
        dist = std::max( dist, (*pDistances)( *a, *b ) );
      }

      if (dist < minDist)
      {
        minDist = dist;
        bestNo = (*a);
      }
    }

    ALIB_ASSERT( bestNo >= 0 );
    (*pCenter) = (*pImages)[bestNo];
  }
}

