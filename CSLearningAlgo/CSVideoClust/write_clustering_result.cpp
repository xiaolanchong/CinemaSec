///////////////////////////////////////////////////////////////////////////////////////////////////
// write_clustering_result.cpp
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
/** \brief Function writes clustering result into destination file.

  \param  fileName    name of destination file.
  \param  pClusters   pointer to the cluster (cluster is a set of indices of close elements).
  \param  pImages     pointer to the source image array.
  \param  pDistances  pointer to the storage of pairwise distances.
  \param  bMeanImage  synthesizes the mean image if nonzero, otherwise get the most representative one.
  \param  pNotifier   message handler. */
//-------------------------------------------------------------------------------------------------
void WriteClusteringResult( LPCTSTR                             fileName,
                            const std::list< std::list<int> > * pClusters,
                            const std::vector<Arr2ub>         * pImages,
                            const PairwiseDistances           * pDistances,
                            alib::IProcessNotify              * pNotifier ) throw(...)
{
  typedef  std::list<int>      Cluster;
  typedef  std::list<Cluster>  ClusterLst;

  std::auto_ptr<avideolib::IVideoWriter> pWriter;
  ATimer                                 timer;
  const Parameters &                     params = GetParameters();
  Arr2ub                                 image;

  timer.Start();
  if (pNotifier != 0) pNotifier->start( _T("writing out result of clusterization: ... "), 0, 0 );

  pWriter.reset( avideolib::CreateAVideoWriter( fileName, pNotifier, false, params.bInvertY ) );
  ALIB_ASSERT( pWriter.get() != 0 );
  for (ClusterLst::const_iterator it = pClusters->begin(); it != pClusters->end(); ++it)
  {
    CalcClusterCenter( &(*it), pImages, pDistances, &image, params.bMeanImage );
    ALIB_ASSERT( pWriter->WriteFrame( &image, 0 ) );
  }

  timer.Stop();
  if (pNotifier != 0) { pNotifier->finished( 0, 0, 0 );  pNotifier->print( timer.TimeStr() ); }
}

