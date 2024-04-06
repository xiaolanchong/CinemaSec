#include "stdafx.h"
#include "videoclust.h"
#include "../../CSAlgo/CSAlgo2/algorithm/SubImage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//-------------------------------------------------------------------------------------------------
/** \brief Function extracts ands stores the most representative images from a videosequence.

  \param  inFile     the name of source AVD-file that containes a sequence of chair images.
  \param  outFile    the name of resultant file that containes the most representative images.
  \param  pNotifier  message handler. */
//-------------------------------------------------------------------------------------------------
void ClusterizeSequence( const TCHAR * inFile, const TCHAR * outFile,
                         alib::IProcessNotify * pNotifier ) throw(...)
{
  typedef  std::vector<double>                 DblVec;
  typedef  std::vector<DblVec>                 DblVecArr;
  typedef  TriangleStorage<float,double,true>  PairwiseDistances;
  typedef  std::vector<Arr2ub>                 ImageArr;
  typedef  std::vector<bool>                   PassedFlags;

  const Parameters & params = GetParameters();
  const int          MAX_FRAME_NUM = 10000;
  const bool         bNormL1 = params.bNormL1;
  int                width = 0, height = 0;
  ImageArr           frames;
  ImageArr           centers;
  PairwiseDistances  distances;
  ATimer             timer;
  Arr2f              weights;

  ////////////////////////////////
  // Read input raw sample images.
  {
    std::auto_ptr<avideolib::IVideoReader> pReader;
    int                                    seqSize = 0;
    Arr2ub                                 mask;

    timer.Start();
    pReader.reset( avideolib::CreateAVideoReader( inFile, pNotifier, false) );
    ALIB_ASSERT( pReader.get() != 0 );
    if (pNotifier != 0) pNotifier->start( _T("reading input sequence: ... "), 0, 0 );

    // Obtain sequence information.
    if (pReader.get() != 0)
    {
      alib::TStateFlag flag;
      flag.data = avideolib::IBaseVideo::WIDTH;
      ALIB_ASSERT( pReader->GetData( &flag ) );
      width = flag.data;
      flag.data = avideolib::IBaseVideo::HEIGHT;
      ALIB_ASSERT( pReader->GetData( &flag ) );
      height = flag.data;
      flag.data = avideolib::IBaseVideo::FRAME_NUMBER;
      ALIB_ASSERT( pReader->GetData( &flag ) );
      seqSize = flag.data;
    }

    // Read sequence images and store them into temporal array of images.
    frames.resize( std::min( seqSize, MAX_FRAME_NUM ) );
    for (int i = 0; i < seqSize; i++)
    {
      int      index = MulDiv( i, std::min( seqSize-1, MAX_FRAME_NUM-1 ), std::max( seqSize-1, 1 ) );
      Arr2ub & image = frames[index];

      ALIB_ASSERT( pReader->ReadFrame( &image, index, 0, 0 ) != avideolib::IVideoReader::GENERAL_ERROR );

      if (mask.empty())
        mask.resize2( image );
      ALIB_ASSERT( alib::AreDimensionsEqual( mask, image ) );

      // Accumulate mask, where zero pixels belong to background and nonzero ones belong to chair image.
      {
        for (int k = 0, n = mask.size(); k < n; k++)
        {
          if (image[k] > 0)
            mask[k] |= (Arr2ub::value_type)1;
        }
      }
    }

    // Calculate weight-mask that contains weights of all chair points.
    {
      weights.resize2( mask );
      alib::Copy( mask, weights );
      double scale = std::accumulate( weights.begin(), weights.end(), double(0) );
      alib::Multiply( weights, alib::Reciprocal( scale ) );
    }

    timer.Stop();
    if (pNotifier != 0) { pNotifier->finished( 0, 0, 0 );  pNotifier->print( timer.TimeStr() ); }
  }

  ////////////////////////////////////////////////////////
  // Compute pairwise distances between all sample images.
  {
    const int frameNum = (int)(frames.size());
    Arr2f     image1, image2;

    timer.Start();
    distances.resize( frameNum, float(0), true );
    if (pNotifier != 0) pNotifier->start( _T("computing pairwise distances: ... "), 0, 0 );

    ALIB_ASSERT( !(weights.empty()) );
    image1.resize2( weights );
    image2.resize2( weights );

    for (int i = 0; i < frameNum; i++)
    {
      alib::Copy( frames[i], image1 );
      csalgo2::NormalizeSubImage( image1, weights, bNormL1 );

      for (int k = 0; k < i; k++)
      {
        alib::Copy( frames[k], image2 );
        csalgo2::NormalizeSubImage( image2, weights, bNormL1 );
        distances( i, k ) = csalgo2::DifferenceBetweenNormalizedSubImages( image1, image2, weights, bNormL1 );
      }
    }

    timer.Stop();
    if (pNotifier != 0) { pNotifier->finished( 0, 0, 0 );  pNotifier->print( timer.TimeStr() ); }
  }

  ////////////////////////////////////
  // Estimate initial cluster centers.
  {
    double      distThreshold = params.distThreshold;
    const int   frameNum = (int)(frames.size());
    PassedFlags passed( frameNum );
    int         passedCount = 0;

    timer.Start();
    centers.reserve( frameNum );

    std::fill( passed.begin(), passed.end(), false );
    centers.clear();

    // Try until most points have passed clusterization.
    for (int attempt = 0; attempt < 200; attempt++)
    {
      //passedCount = 0;
      //std::fill( passed.begin(), passed.end(), false );
      //centers.clear();

      if (pNotifier != 0) pNotifier->start( _T("estimating initial cluster centers: ... "), 0, 0 );
      std::cout << "Distance threshold = " << distThreshold << std::endl;

      // While a cluster center could be found ...
      for (int bestIndex = 0; bestIndex >= 0;)
      {
        int bestCount = 0;

        // For each sample image evaluate whether it contains the maximal number
        // of neighbours, among remained samples, within specified distance threshold.
        bestIndex = -1;
        for (int i = 0; i < frameNum; i++)
        {
          int count = 0;

          if (passed[i])
            continue;

          for (int k = 0; k < frameNum; k++)
          {
            if (passed[k])
              continue;

            if (distances( i, k ) < distThreshold)
              ++count;
          }

          if (bestCount < count)
          {
            bestCount = count;
            bestIndex = i;
          }
        }

        // The bestIndex candidate cluster center provides the max. density of neighbour samples.
        if ((bestIndex >= 0) && (bestCount >= params.neighbourhoodSize))
        {
          centers.push_back( frames[ bestIndex ] );

          // Mark the nearest neighbours of the best cluster center as passed ones.
          for (int k = 0; k < frameNum; k++)
          {
            if (!(passed[k]) && (distances( bestIndex, k ) < distThreshold))
            {
              passed[k] = true;
              ++passedCount;
            }
          }
        }
        else bestIndex = -1;

        if (pNotifier != 0) pNotifier->running( 0, 0, 0 );
      }

      // Have most points passed?
      if ((double)passedCount < (double)(params.passedPercentage*0.01*frameNum))
      {
        distThreshold += 0.25 * params.distThreshold;

        if (pNotifier != 0)
          pNotifier->print( _T("Too tight distance threshold, new attempt will be made.") );
      }
      else break;
    }

    timer.Stop();
    if (pNotifier != 0) { pNotifier->finished( 0, 0, 0 );  pNotifier->print( timer.TimeStr() ); }
  }

  ///////////////////////////////////////
  // Free unnecessary very large buffers.
  distances.clear();

  //////////////////////////////////////////////
  // Write output file of clusterization result.
  {
    std::auto_ptr<avideolib::IVideoWriter> pWriter;

    timer.Start();
    if (pNotifier != 0) pNotifier->start( _T("writing out result of clusterization: ... "), 0, 0 );

    pWriter.reset( avideolib::CreateAVideoWriter( outFile, pNotifier, false, params.bInvertY ) );
    ALIB_ASSERT( pWriter.get() != 0 );
    for (ImageArr::const_iterator it = centers.begin(); it != centers.end(); ++it)
    {
      ALIB_ASSERT( pWriter->WriteFrame( &(*it), 0 ) );
    }

    timer.Stop();
    if (pNotifier != 0) { pNotifier->finished( 0, 0, 0 );  pNotifier->print( timer.TimeStr() ); }
  }
}




/*
#if 0
///////////////////////////////////////////////////////////////////////
// Iteratively update center vectors using K-harmonic mean clusterizer.
  {
    const double                      MIN_SQ_DISTANCE = FLT_MIN/(FLT_EPSILON*FLT_EPSILON);
    const int                         K = (int)(centers.size());
    const int                         dim = width*height;
    DblVec                            qVec, qSum;                   //!< temporal vectors
    DblVecArr                         curCenters;                   //!< temporal current cluster centers
    DblVecArr                         preCenters;                   //!< temporal previous cluster centers
    SqSampleDistanceEx<Arr2ub,DblVec> sqDist;

    if (pNotifier != 0) pNotifier->start( _T("performing K-harmonic mean clusterization: "), 0, 0 );

    // Initialize.
    qVec.resize( K );
    qSum.resize( K );
    preCenters.resize( K );
    curCenters.resize( K );
    for (int c = 0; c < K; c++)
    {
      (preCenters[c]).resize( dim, double(0) );
      (curCenters[c]).resize( dim, double(0) );
      alib::Copy( centers[c], curCenters[c] );
    }

    // Do iterations ...
    for (int iter = 0; iter < params.s_clust.p_clustIterNum(); iter++)
    {
      // Store current centers in the temporal buffer (preCenters) and fill them up by zeros.
      for (int c = 0; c < K; c++)
      {
        (curCenters[c]).swap( preCenters[c] );
        alib::Fill( curCenters[c], double(0) );
        qSum[c] = 0.0;
      }

      // Accumulate new cluster centers using all sample images.
      for (ImageArr::const_iterator it = frames.begin(); it != frames.end(); ++it)
      {
        double revQSum = 0.0;

        for (int c = 0; c < K; c++)
        {
          revQSum += (qVec[c] = 1.0/(sqDist( *it, preCenters[c], bNormL2 ) + MIN_SQ_DISTANCE));
        }
        revQSum = 1.0/(revQSum*revQSum);

        for (int c = 0; c < K; c++)
        {
          qSum[c] += (qVec[c] = qVec[c] * sqrt( qVec[c] ) * revQSum);
          alib::AddMultiplied( *it, curCenters[c], qVec[c] );
        }
      }

      // Normalize accumulated cluster centers.
      for (int c = 0; c < K; c++)
      {
        alib::Multiply( curCenters[c], (double)(alib::Reciprocal( qSum[c] )) );
      }

      if (pNotifier != 0) pNotifier->running( 0, 0, 0 );
    }

    // Store rounded cluster centers in byte images.
    for (int c = 0; c < K; c++)
    {
      Arr2ub & m = centers[c];
      DblVec & s = curCenters[c];

      for (int k = 0, n = m.size(); k < n; k++)
      {
        m[k] = (ubyte)(alib::Limit( (int)floor( s[k]+0.5 ), 0, UCHAR_MAX ));
      }
    }

    if (pNotifier != 0) pNotifier->finished( 0, 0, 0 );
  }
#endif
*/

