///////////////////////////////////////////////////////////////////////////////////////////////////
// dendro_clustering_postprocess.h
// ---------------------
// begin     : Aug 2004
// modified  : 8 Dec 2005
// author(s) : Albert Akhriev
// email     : Albert.Akhriev@biones.com
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef  __DEFINE_DENDRO_CLUSTERING_POSTPROCESSOR_H__
#define  __DEFINE_DENDRO_CLUSTERING_POSTPROCESSOR_H__

#include "dendro_clusterizer.h"
#include "print_histogram_set.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
/// \struct CompareSizesForDescendantSorting.
/// \brief  Class-comparator is used to sort containers by size in descendant order.
///////////////////////////////////////////////////////////////////////////////////////////////////
template< class CONTAINER >
struct CompareSizesForDescendantSorting
{
  bool operator()( const CONTAINER & a, const CONTAINER & b ) const
  {
    return (a.size() > b.size());
  }
};


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \class DendroClusteringPostProcessor.
/// \brief Class prints top clusters of dendro-structure into the output files..
///////////////////////////////////////////////////////////////////////////////////////////////////
class DendroClusteringPostProcessor : public IBasePostProcessor
{
private:
  std::string                       m_fileTitle;   //!< the title of output files
  std::vector< std::vector<int> > * m_pHistograms; //!< pointer to the histograms of frames of a videosequence

  DendroClusteringPostProcessor();

public:
  /////////////////////////////////////////////////////////////////////////////////////////////////
  /// \brief Constructor.
  ///
  /// \param  fileTitle    the title of output file(s).
  /// \param  pHistograms  pointer to the histograms of frames of a videosequence.
  /////////////////////////////////////////////////////////////////////////////////////////////////
  DendroClusteringPostProcessor( const char * fileTitle, std::vector< std::vector<int> > * pHistograms )
  {
    m_pHistograms = 0;
    ALIB_ASSERT( (fileTitle != 0) && (pHistograms != 0) && !(pHistograms->empty()) );
    m_fileTitle = fileTitle;
    m_pHistograms = pHistograms;
  }

  /////////////////////////////////////////////////////////////////////////////////////////////////
  /// \brief Function prints top clusters of dendro-structure into the output files.
  ///
  /// Printed clusters are sorted in descendant order by size.
  ///
  /// \param  pClusters  pointer to the cluster list.
  /////////////////////////////////////////////////////////////////////////////////////////////////
  virtual void operator()( std::list< std::list<int> > * pClusters )
  {
    typedef  std::list<int>      Cluster;
    typedef  std::list<Cluster>  ClusterLst;

    ALIB_ASSERT( (pClusters != 0) && (m_pHistograms != 0) && !(m_fileTitle.empty()) );
    if ((int)(pClusters->size()) > 5)
      return;

    char ext[128];
    sprintf( ext, "_%dclusters.plt", (int)(pClusters->size()) );
    csutility::PrintHistogramSet printer( (m_fileTitle + ext).c_str() );

    std::vector<double> histogram;
    int                 count = 1;

    pClusters->sort( CompareSizesForDescendantSorting< std::list<int> >() );

    // Create a file of day/night settings: camera id + night histogram + day histogram.
    sprintf( ext, "_%dclusters.txt", (int)(pClusters->size()) );
    std::fstream file( (m_fileTitle + ext).c_str(), std::ios::trunc | std::ios::out );
    file << m_fileTitle << std::endl;

    for (ClusterLst::const_iterator clustIt = pClusters->begin(); clustIt != pClusters->end(); ++clustIt)
    {
      // Accumulate cluster's histogram.
      histogram.resize( m_pHistograms->front().size() );
      std::fill( histogram.begin(), histogram.end(), double(0) );
      for (Cluster::const_iterator it = clustIt->begin(); it != clustIt->end(); ++it)
      {
        ALIB_ASSERT( ALIB_IS_RANGE( *it, 0, (int)(m_pHistograms->size()) ) );
        ALIB_ASSERT( ((*m_pHistograms)[*it]).size() == histogram.size() );
        std::transform( histogram.begin(), histogram.end(), ((*m_pHistograms)[*it]).begin(),
                        histogram.begin(), std::plus<double>() );
      }

      // Print histograms of the first cluster (night) and the second cluster (day).
      if ((count == 1) || (count == 2))
      {
        ALIB_ASSERT( (int)(histogram.size()) == (1<<8) );

        double scale = alib::Reciprocal( std::accumulate( histogram.begin(), histogram.end(), double(0) ) );
        for (int i = 0, n = (int)(histogram.size()); i < n; i++)
          file << (float)(scale * histogram[i]) << "  ";
        file << std::endl;
        if (count == 2) file << std::endl;
      }

      double     * first[1];  first[0] = &(histogram[0]);
      double     * last [1];  last [0] = first[0] + histogram.size();
      const char * title[1];  title[0] = "";
      char         commonTitle[64];

      sprintf( commonTitle, "Cluster %d, the number of samples = %d", count, (int)(clustIt->size()) );
      printer.PrintHistogram<double*,1>( first, last, title, commonTitle, 0 );
      ++count;
    }
  }
};

#endif // __DEFINE_DENDRO_CLUSTERING_POSTPROCESSOR_H__

