#include "stdafx.h"
#include "print_histogram_set.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define MY_SHOW_VIDEO 0

CWinApp theApp;

// intensity range
const int RANGE = (1<<8);

// up bound = 100%
const int HUNDRED = 100;

typedef  std::vector<int>  IntArray;
typedef  std::list<int>    IntList;

///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function .
///////////////////////////////////////////////////////////////////////////////////////////////////
std::wstring GetCamIdText( std::wstring & filename )
{
  int pos = (int)(filename.rfind( L"cam" ));
  ALIB_ASSERT( pos != (int)(filename.npos) );
  return filename.substr( pos + 3, 2 );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function .
///////////////////////////////////////////////////////////////////////////////////////////////////
int GetCamId( std::wstring & filename )
{
  int id = -1;
  int pos = (int)(filename.rfind( L"cam" ));
  ALIB_ASSERT( (pos != (int)(filename.npos)) && (swscanf( filename.c_str()+(pos+3), L"%d", &id ) == 1) );
  return id;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function calculates reverse cumulative function.
///////////////////////////////////////////////////////////////////////////////////////////////////
template< class T >
T CalcReverseCumulativeFunction( const T * histogram, T * cumulFunc, int size )
{
  ASSERT( (histogram != 0) && (cumulFunc != 0) && (size > 0) );
  T sum = (T)0;
  for (int i = (size-1); i >= 0; i--)
  {
    sum = (cumulFunc[i] = (histogram[i] + sum));
  }
  return sum;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function .
///////////////////////////////////////////////////////////////////////////////////////////////////
float GetHistogramDifference( const __int64 loHist[RANGE], const __int64 hiHist[RANGE] )
{
  double  maxDiff = 0.0;
  __int64 loCumul[RANGE];
  __int64 hiCumul[RANGE];
  __int64 loSum = CalcReverseCumulativeFunction( loHist, loCumul, RANGE );
  __int64 hiSum = CalcReverseCumulativeFunction( hiHist, hiCumul, RANGE );

  if ((loSum < 1) || (hiSum < 1))
    return -1.0f;

  for (int i = 0; i < RANGE; i++)
  {
    maxDiff = std::max( maxDiff, (double)hiCumul[i]/(double)hiSum -
                                 (double)loCumul[i]/(double)loSum );
  }
  return (float)maxDiff;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function .
///////////////////////////////////////////////////////////////////////////////////////////////////
void OutputCumulativeFunctions( const __int64 loHist[RANGE],
                                const __int64 hiHist[RANGE], int camId )
{
  __int64 loCumul[RANGE];
  __int64 hiCumul[RANGE];
  __int64 loSum = std::max<__int64>( 1, CalcReverseCumulativeFunction( loHist, loCumul, RANGE ) );
  __int64 hiSum = std::max<__int64>( 1, CalcReverseCumulativeFunction( hiHist, hiCumul, RANGE ) );
  char    fname[64];

  sprintf( fname, "cam%02d-dn.plt", camId );
  std::fstream f( fname, std::ios::trunc | std::ios::out );
  f << "reset" << std::endl;
  f << "set data style lines" << std::endl;
  f << "set title \"camera " << camId << "\"" << std::endl;
  f << "plot '-' t \"low\", '-' t \"high\"" << std::endl;
  for (int i = 0; i < RANGE; i++) f << i << " " << ((double)loCumul[i]/(double)loSum) << std::endl;
  f << "e" << std::endl;
  for (int i = 0; i < RANGE; i++) f << i << " " << ((double)hiCumul[i]/(double)hiSum) << std::endl;
  f << "e" << std::endl << std::endl;
  //f << "pause -1 \"Hit return to continue\"" << std::endl << std::endl;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Function .
///////////////////////////////////////////////////////////////////////////////////////////////////
bool CalcOptimalParameters( const __int64 loHist[RANGE], const __int64 hiHist[RANGE],
                            int & iThreshold, int & pThreshold )
{
  iThreshold = -1;
  pThreshold = -1;

  int     iBest = -1;
  double  maxDiff = 0.0;
  __int64 loCumul[RANGE];
  __int64 hiCumul[RANGE];
  __int64 loSum = CalcReverseCumulativeFunction( loHist, loCumul, RANGE );
  __int64 hiSum = CalcReverseCumulativeFunction( hiHist, hiCumul, RANGE );

  if ((loSum < 1) || (hiSum < 1))
    return false;

  for (int i = 0; i < RANGE; i++)
  {
    double diff = (double)hiCumul[i]/(double)hiSum -
                  (double)loCumul[i]/(double)loSum;

    if (maxDiff < diff)
    {
      maxDiff = diff;
      iBest = i;
    }
  }

  if (iBest < 0)
    return false;

  iThreshold = iBest;
  pThreshold = (int)(50.0*((double)hiCumul[iBest]/(double)hiSum +
                           (double)loCumul[iBest]/(double)loSum) + 0.5);
  return true;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
int _tmain( int argc, TCHAR * argv[], TCHAR * envp[] )
{
  Arr2ub                   image;
  int                      index = 0, readResult = avideolib::IVideoReader::NORMAL;
  alib::IConsolePainterPtr painter;
  int                      histogram[RANGE];
  __int32                  minTime = INT_MAX, maxTime = 0;
  IntList                  percentHistos[RANGE][HUNDRED+1];
  std::vector<IntArray>    allHistos;

  argc;argv;envp;
  ASSERT( RANGE == (UCHAR_MAX+1) );
  if (!AfxWinInit( ::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0 ))
    return 1;

  try
  {
    std::wcout << L"Day-night unsupervised learning console" << std::endl;
    ALIB_VERIFY( argc == 2, _T("\n video file as argument needed\n") );

    if (!(CFileFind().FindFile( argv[1] )))
    {
      std::wcout << L"file not found" << std::endl;
      return -1;
    }

#if MY_SHOW_VIDEO
    painter.reset( alib::CreateWindowsConsolePainter( argv[1], false, bInvertY ) );
    ALIB_VERIFY( painter.get() != 0, _T("Failed to create a painter") );
#endif // MY_SHOW_VIDEO

    std::auto_ptr<avideolib::IVideoReader> pReader;
    pReader.reset( avideolib::CreateAVideoReader( argv[1], 0, true ) );
    ALIB_VERIFY( pReader.get() != 0, _T("Failed to create AVD-reader") );

    for (int pass = 1; pass <= 2; pass++)
    {
      if (pass == 1)
      {
        std::wcout << "Pass 1: accumulating statistics ..." << std::endl;

        pReader->Seek(0);
        alib::TStateFlag flag;
        flag.data = avideolib::IBaseVideo::FRAME_NUMBER;
        pReader->GetData( &flag );
        int totalFrameNum = flag.data;

        allHistos.reserve( totalFrameNum );
        allHistos.clear();

        csutility::PrintHistogramSet histoPrinter( "histograms.plt" );

        while (kbhit()); // clear message queue
        for (int ch = 0; (!kbhit() || ((ch = getch()) != 27)) && (readResult == avideolib::IVideoReader::NORMAL);)
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

            allHistos.push_back( IntArray() );
            allHistos.back().assign( histogram, histogram + RANGE );
          }

          // Print histogram into a file.
          if (histoPrinter.IsReady())
          {
            float        fHisto[RANGE];
            float      * first[1];  first[0] = fHisto;
            float      * last[1];   last [0] = fHisto+RANGE;
            const char * titles[1] = {""};
            char         commonTitle[256];

            alib::Copy( histogram, histogram+RANGE, fHisto, fHisto+RANGE, float() );
            alib::Multiply( fHisto, fHisto+RANGE,
              (float)(alib::Reciprocal( std::accumulate( fHisto, fHisto+RANGE, double(0) ))) );
            sprintf( commonTitle, "frame = %d", index );
            histoPrinter.PrintHistogram<float*,1>( first, last, titles, commonTitle, 0 );
          }

          // Calculate cumulative functions.
          CalcReverseCumulativeFunction( histogram, histogram, RANGE );

          // Fill up histograms of frame distributions for each brightness threshold.
          {
            for (int threshold = 0; threshold < RANGE; threshold++)
            {
              int n = MulDiv( HUNDRED, histogram[threshold], image.size() );
              ASSERT( (0 <= n) && (n <= HUNDRED) );
              percentHistos[threshold][n].push_back( index );       
            }
          }
        }
      }
      else if (pass == 2)
      {
        std::wcout << "Pass 2: analyzing accumulated statistics ..." << std::endl;

        __int64 loBestHist[RANGE];  memset( loBestHist, 0, sizeof(loBestHist) );
        __int64 hiBestHist[RANGE];  memset( hiBestHist, 0, sizeof(hiBestHist) );
        int     iThreshold = -1;
        int     pThreshold = -1;
        float   maxDiff = 0.0f;

        // For each threshold calculate the cost.
        for (int threshold = 0; threshold < RANGE; threshold++)
        {
          for (int i = 1; i < HUNDRED; i++)
          {
            __int64 loHist[RANGE];  memset( loHist, 0, sizeof(loHist) );
            __int64 hiHist[RANGE];  memset( hiHist, 0, sizeof(hiHist) );

            int k;
            for (k = 0; k <= i; k++)
            {
              const IntList & bin = percentHistos[threshold][k];

              for (IntList::const_iterator it = bin.begin(); it != bin.end(); ++it)
              {
                const IntArray & frameHisto = allHistos[*it];
                ASSERT( (int)(frameHisto.size()) == RANGE );
                std::transform( loHist, loHist+RANGE, frameHisto.begin(), loHist, std::plus<__int64>() );
              }
            }

            for (; k <= HUNDRED; k++) 
            {
              const IntList & bin = percentHistos[threshold][k];

              for (IntList::const_iterator it = bin.begin(); it != bin.end(); ++it)
              {
                const IntArray & frameHisto = allHistos[*it];
                ASSERT( (int)(frameHisto.size()) == RANGE );
                std::transform( hiHist, hiHist+RANGE, frameHisto.begin(), hiHist, std::plus<__int64>() );
              }
            }

            float diff = GetHistogramDifference( loHist, hiHist );
            if (maxDiff < diff)
            {
              maxDiff = diff;
              memcpy( loBestHist, loHist, sizeof(loBestHist) );
              memcpy( hiBestHist, hiHist, sizeof(hiBestHist) );
            }
          }
        }

        ALIB_ASSERT( CalcOptimalParameters( loBestHist, hiBestHist, iThreshold, pThreshold ) );

        std::wstring filename( argv[1] );
        std::wstring outfilename = L"cam" + GetCamIdText( filename ) + L"_DayNight.txt";
        std::wstring allhistsfilename = L"hists_cam" + GetCamIdText( filename ) + L"_DayNight.txt";
        OutputCumulativeFunctions( loBestHist, hiBestHist, GetCamId( filename ) );

        // Store the best found parameters of the camera.
        {
          USES_CONVERSION;
          std::wfstream f;
          f.open( W2A(outfilename.c_str()), std::ios_base::out );
          f << L"camera" << std::setw(4) << std::right << GetCamIdText( filename )
            << std::setw(6) << std::right << iThreshold
            << std::setw(6) << std::right << pThreshold << std::endl;
        }

        std::wcout << L"Video duration = " << (float)(0.001*(maxTime-minTime)) << " sec." << std::endl
                   << L"camera: " << GetCamIdText( filename ) << std::endl
                   << L"Brightness threshold = " << iThreshold << std::endl
                   << L"Percentage threshold = " << pThreshold << std::endl << std::endl;
      }
    }
  }
  catch (std::runtime_error & err)
  {
    CString msg( err.what() );
    std::wcout << (LPCWSTR)msg << std::endl;
  }
  return 0;
}

