///////////////////////////////////////////////////////////////////////////////////////////////////
// dendro_clusterizer.cpp
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
/** \brief Function performs dendro-clustering given pointwise distances between elements of a set.

  \param  pDistances     pointer to the destination storage of pairwise distances.
  \param  pClusters      pointer to the list of the list of element indices of each cluster.
  \param  maxClusterNum  the maximal number of clusters.
  \param  pNotifier      message handler. */
//-------------------------------------------------------------------------------------------------
void DoDendroClustering( const PairwiseDistances     * pDistances,
                         std::list< std::list<int> > * pClusters,
                         const int                     maxClusterNum,
                         alib::IProcessNotify        * pNotifier ) throw(...)
{
  typedef  std::list<int>      Cluster;
  typedef  std::list<Cluster>  ClusterLst;

  ALIB_ASSERT( (pDistances != 0) && (pDistances->dim() >= 1) && (pClusters != 0) && (maxClusterNum > 1) );

  ClusterLst::iterator it, c1, c2, best1, best2;
  int                  count = 0;
  ATimer               timer;

  timer.Start();
  if (pNotifier != 0) pNotifier->start( _T("carring out dendro-clustering: ... "), 0, 0 );

  // Initially each cluster contains a single element.
  pClusters->resize( pDistances->dim() );
  for (it = pClusters->begin(); it != pClusters->end(); ++it)
  {
    it->push_back( count++ );
  }

  // Iterate while the number of clusters exceeds predefined value.
  while ((int)(pClusters->size()) > maxClusterNum)
  {
    float minDist = FLT_MAX;

    best1 = pClusters->begin();
    best2 = pClusters->begin();

    // Find the nearest pair of clusters.
    for (c1 = pClusters->begin(); c1 != pClusters->end(); ++c1)
    {
      for (++(c2 = c1); c2 != pClusters->end(); ++c2)
      {
        Cluster::iterator e1, e2;
        float             dist = 0.0f;

        // The distance between two clusters is the distance between a pair of the most remote elements.
        for (e1 = c1->begin(); e1 != c1->end(); ++e1)
        {
          for (e2 = c2->begin(); e2 != c2->end(); ++e2)
          {
            dist = std::max( dist, (*pDistances)( *e1, *e2 ) );
          }
        }

        if (dist < minDist)
        {
          minDist = dist;
          best1 = c1;
          best2 = c2;
        }
      }
    }

    // Unite a pair of nearest clusters into a single one.
    ALIB_ASSERT( best1 != best2 );
    best1->splice( best1->begin(), *best2 );
    pClusters->erase( best2 );
  }

  timer.Stop();
  if (pNotifier != 0) { pNotifier->finished( 0, 0, 0 );  pNotifier->print( timer.TimeStr() ); }
}

