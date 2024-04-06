#pragma once

#include "process_notify.h"

typedef  TriangleStorage<float,double,true>  PairwiseDistances;

const int MAXIMAL_FRAME_NUMBER = 10000;

//=================================================================================================
/** \brief Parameter settings used to initialize clusterization process. */
//=================================================================================================
struct Parameters
{
  WStr  prefix;             //!< 
  bool  bNormL1;            //!< 
  bool  bInvertY;           //!< 
  float distThreshold;      //!< distance threshold used in greedy clustering algorithm
  float passedPercentage;   //!< 
  int   neighbourhoodSize;  //!< 
  bool  bDendroClustering;  //!< perform dendro-clustering if nonzero, otherwise greedy one
  bool  bMeanImage;         //!< synthesizes the mean image of a cluster if nonzero, otherwise get the most representative one (dendro only)
  int   maxClusterNum;      //!< the maximal number of clusters (dendro only)

  Parameters()
  {
    prefix.clear();
    bNormL1 = true;
    bInvertY = false;
    distThreshold = 0.1f;
    passedPercentage = 70.0f;
    neighbourhoodSize = 10;
    bDendroClustering = false;
    bMeanImage = false;
    maxClusterNum = 0;
  }
};

extern std::wfstream global_log;

void ParseINIFile( LPCWSTR fname ) throw(...);

const Parameters & GetParameters();

alib::IProcessNotify * CreateConsoleProcessNotifier() throw(...);

void ClusterizeSequence( const TCHAR * inFile, const TCHAR * outFile, alib::IProcessNotify * pNotifier ) throw(...);

// load_chair_images.cpp:
void LoadChairImages( const TCHAR          * fileName,
                      std::vector<Arr2ub>  * pImages,
                      Arr2f                * pWeights,
                      alib::IProcessNotify * pNotifier ) throw(...);

// pairwise_distances.cpp:
void CalcPairwiseDistances( const std::vector<Arr2ub> * pImages,
                            const Arr2f               * pWeights,
                            PairwiseDistances         * pDistances,
                            const bool                  bNormL1,
                            alib::IProcessNotify      * pNotifier ) throw(...);

// dendro_clusterizer.cpp:
void DoDendroClustering( const PairwiseDistances     * pDistances,
                         std::list< std::list<int> > * pClusters,
                         const int                     maxClusterNum,
                         alib::IProcessNotify        * pNotifier ) throw(...);

// cluster_center.cpp:
void CalcClusterCenter( const std::list<int>      * pCluster,
                        const std::vector<Arr2ub> * pImages,
                        const PairwiseDistances   * pDistances,
                        Arr2ub                    * pCenter,
                        const bool                  bMeanImage ) throw(...);

// write_clustering_result.cpp:
void WriteClusteringResult( LPCTSTR                             fileName,
                            const std::list< std::list<int> > * pClusters,
                            const std::vector<Arr2ub>         * pImages,
                            const PairwiseDistances           * pDistances,
                            alib::IProcessNotify              * pNotifier ) throw(...);

