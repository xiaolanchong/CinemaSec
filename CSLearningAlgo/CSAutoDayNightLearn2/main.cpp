#include "stdafx.h"
#include "print_histogram_set.h"
#include "dendro_clusterizer.h"
#include "dendro_clustering_postprocess.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define MY_SHOW_VIDEO 0

CWinApp theApp;

// intensity range
const int RANGE = (1<<8);

// up bound = 100%
const int HUNDRED = 100;

void CalcPairwiseDistancesByKolmogorovTest( const std::vector< std::vector<int> > * pHistograms,
                                            TriangleStorage<float,double,true>    * pDistances );

///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function .
///////////////////////////////////////////////////////////////////////////////////////////////////
std::string GetCamIdText( std::string & filename )
{
  int pos = (int)(filename.rfind( "cam" ));
  ALIB_ASSERT( pos != (int)(filename.npos) );
  return (filename.substr( pos+3, 2 ));
}


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
int main( int argc, char * argv[] )
{
  Arr2ub                             image;
  int                                index = 0, readResult = avideolib::IVideoReader::NORMAL;
  int                                histogram[RANGE];
  __int32                            minTime = INT_MAX, maxTime = 0;
  std::vector< std::vector<int> >    histograms;
  TriangleStorage<float,double,true> distances;
  std::list< std::list<int> >        clusters;

  argc;argv;
  ASSERT( RANGE == (UCHAR_MAX+1) );
  if (!AfxWinInit( ::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0 ))
    return 1;

  try
  {
    std::cout << "Day-night unsupervised learning console" << std::endl;
    ALIB_VERIFY( argc == 2, _T("\n video file as argument needed\n") );

    if (!(CFileFind().FindFile( CString( argv[1] ) )))
    {
      std::wcout << "file not found" << std::endl;
      return -1;
    }

    // Read a videosequence and store histograms of all frames.
    {
#if MY_SHOW_VIDEO
      alib::IConsolePainterPtr painter;
      painter.reset( alib::CreateWindowsConsolePainter( CString( argv[1] ), false, bInvertY ) );
      ALIB_VERIFY( painter.get() != 0, _T("Failed to create a painter") );
#endif // MY_SHOW_VIDEO

      std::auto_ptr<avideolib::IVideoReader> pReader;
      pReader.reset( avideolib::CreateAVideoReader( CString( argv[1] ), 0, true ) );
      ALIB_VERIFY( pReader.get() != 0, _T("Failed to create AVD-reader") );

      std::cout << "Pass 1: accumulating statistics ..." << std::endl;

      pReader->Seek(0);
      alib::TStateFlag flag;
      flag.data = avideolib::IBaseVideo::FRAME_NUMBER;
      pReader->GetData( &flag );
      int totalFrameNum = flag.data;

      histograms.reserve( totalFrameNum );
      histograms.clear();

      csutility::PrintHistogramSet histoPrinter( "histograms.plt" );

      int ch = 0;
      while (kbhit()); // clear message queue
      while ((!kbhit() || ((ch = getch()) != 27)) && (readResult == avideolib::IVideoReader::NORMAL))
      {
        while (kbhit()); // clear message queue

        __int32 time = 0;
        readResult = pReader->ReadFrame( &image, -1, &index, &time );
        ALIB_ASSERT( readResult != avideolib::IVideoReader::GENERAL_ERROR );
        minTime = std::min( minTime, time );
        maxTime = std::max( maxTime, time );

#if MY_SHOW_VIDEO
        wchar_t statusText[64];
        swprintf( statusText, L"frame = %d", index );
        painter->DrawDemoImage( &image, statusText );
#endif // MY_SHOW_VIDEO

        // Accumulate histogram.
        {
          memset( histogram, 0, sizeof(histogram) );
          for (int i = 0, n = image.size(); i < n; i++)
            ++(histogram[ image[i] ]);

          histograms.push_back( std::vector<int>() );
          histograms.back().assign( histogram, histogram + RANGE );
        }

        // Print histogram into a file.
        if (histoPrinter.IsReady())
        {
          int        * first[1];  first[0] = histogram;
          int        * last [1];  last [0] = histogram + RANGE;
          const char * title[1] = {""};
          char         commonTitle[64];

          sprintf( commonTitle, "frame = %d", index );
          histoPrinter.PrintHistogram<int*,1>( first, last, title, commonTitle, 0 );
        }
      }

      if (ch == 27)
        return 0;
    }

    // Compute pairwise distances between frame histograms.
    std::cout << "Pass 2: computing pairwise distances between frame histograms ..." << std::endl;
    CalcPairwiseDistancesByKolmogorovTest( &histograms, &distances );

    // .
    {
      std::cout << "Pass 3: clustering ..." << std::endl;

      std::string                   filename( argv[1] );
      std::string                   outFileTitle = "camera " + GetCamIdText( filename );
      DendroClusteringPostProcessor postprocessor( outFileTitle.c_str(), &histograms );

      DoDendroClustering( &distances, &clusters, 2, 0, &postprocessor );
/*
      // Store the best found parameters of the camera.
      {
        USES_CONVERSION;
        std::wfstream f;
        f.open( W2A(outfilename.c_str()), std::ios_base::out );
        f << "camera" << std::setw(4) << std::right << GetCamIdText( filename )
          << std::setw(6) << std::right << iThreshold
          << std::setw(6) << std::right << pThreshold << std::endl;
      }

      std::cout << "Video duration = " << (float)(0.001*(maxTime-minTime)) << " sec." << std::endl
                << "camera: " << GetCamIdText( filename ) << std::endl
                << "Brightness threshold = " << iThreshold << std::endl
                << "Percentage threshold = " << pThreshold << std::endl << std::endl;
*/
    }
  }
  catch (std::runtime_error & err)
  {
    std::cout << err.what() << std::endl;
  }
  return 0;
}

