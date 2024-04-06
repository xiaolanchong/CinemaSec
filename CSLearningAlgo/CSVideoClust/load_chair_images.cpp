///////////////////////////////////////////////////////////////////////////////////////////////////
// load_chair_images.cpp
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
/** \brief Function loads chair images from a videosequence.

  \param  fileName   the name of source AVD-file that containes a sequence of chair images.
  \param  pImages    pointer to the destination image array.
  \param  pWeights   pointer to the mask of weights of image points.
  \param  pNotifier  message handler. */
//-------------------------------------------------------------------------------------------------
void LoadChairImages( const TCHAR          * fileName,
                      std::vector<Arr2ub>  * pImages,
                      Arr2f                * pWeights,
                      alib::IProcessNotify * pNotifier ) throw(...)
{
  std::auto_ptr<avideolib::IVideoReader> pReader;
  int                                    seqSize = 0;
  Arr2ub                                 mask;
  ATimer                                 timer;

  ALIB_ASSERT( (pImages != 0) && (pWeights != 0) );
  timer.Start();
  pReader.reset( avideolib::CreateAVideoReader( fileName, pNotifier, false ) );
  ALIB_ASSERT( pReader.get() != 0 );
  if (pNotifier != 0) pNotifier->start( _T("reading input sequence: ... "), 0, 0 );

  // Obtain sequence information.
  if (pReader.get() != 0)
  {
    alib::TStateFlag flag;
    flag.data = avideolib::IBaseVideo::FRAME_NUMBER;
    ALIB_ASSERT( pReader->GetData( &flag ) );
    seqSize = flag.data;
  }

  // Read sequence images and store them into temporal array of images.
  pImages->resize( std::min( seqSize, MAXIMAL_FRAME_NUMBER ) );
  for (int i = 0; i < seqSize; i++)
  {
    int      index = MulDiv( i, std::min( seqSize-1, MAXIMAL_FRAME_NUMBER-1 ), std::max( seqSize-1, 1 ) );
    Arr2ub & image = (*pImages)[index];

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
    pWeights->resize2( mask );
    alib::Copy( mask, *pWeights );
    double scale = std::accumulate( pWeights->begin(), pWeights->end(), double(0) );
    alib::Multiply( *pWeights, alib::Reciprocal( scale ) );
  }

  timer.Stop();
  if (pNotifier != 0) { pNotifier->finished( 0, 0, 0 );  pNotifier->print( timer.TimeStr() ); }
}

