#include "stdafx.h"
#include "videoclust.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

static Parameters g_parameters;

//-------------------------------------------------------------------------------------------------
/** \brief Constructor. */
//-------------------------------------------------------------------------------------------------
const Parameters & GetParameters()
{
  return g_parameters;
}


//-------------------------------------------------------------------------------------------------
/** \brief Function parses ini-file.

  \param  fname  the name of ini-file. */
//-------------------------------------------------------------------------------------------------
void ParseINIFile( LPCWSTR fname ) throw(...)
{
  USES_CONVERSION;

  try
  {
    const char COMMENT = '#';
    const char ENDL = '\n';

    ALIB_ASSERT( fname != 0 );
    std::fstream file( W2CA(fname), std::ios::in );
    ALIB_ASSERT( file.good() );

    AStr text;
    text.reserve( 1<<10 );
    while (file.good() && !(file.eof()))
    {
      text.clear();
      file >> std::skipws >> text;
      if (text.empty())
        continue;
      if (text[0] == COMMENT)
      {
        while (file.good() && !(file.eof()) && (file.get() != ENDL)); // skip comment line
        continue;
      }

      if (stricmp( text.c_str(), "file_prefix" ) == 0)
      {
        file >> std::skipws >> text;
        g_parameters.prefix = (LPCWSTR)(CString( text.c_str() ));
      }
      else if (stricmp( text.c_str(), "norm_L1" ) == 0)
      {
        int value;
        file >> value;
        g_parameters.bNormL1 = (value != 0);
      }
      else if (stricmp( text.c_str(), "invert_Y" ) == 0)
      {
        int value;
        file >> value;
        g_parameters.bInvertY = (value != 0);
      }
      else if (stricmp( text.c_str(), "distance_threshold" ) == 0)
      {
        float value;
        file >> value;
        g_parameters.distThreshold = value;
      }
      else if (stricmp( text.c_str(), "passed_percentage" ) == 0)
      {
        float value;
        file >> value;
        g_parameters.passedPercentage = value;
      }
      else if (stricmp( text.c_str(), "neighbourhood_size" ) == 0)
      {
        int value;
        file >> value;
        g_parameters.neighbourhoodSize = value;
      }
      else if (stricmp( text.c_str(), "dendro_clustering" ) == 0)
      {
        int value;
        file >> value;
        g_parameters.bDendroClustering = (value != 0);
      }
      else if (stricmp( text.c_str(), "mean_image" ) == 0)
      {
        int value;
        file >> value;
        g_parameters.bMeanImage = (value != 0);
      }
      else if (stricmp( text.c_str(), "max_cluster_num" ) == 0)
      {
        int value;
        file >> value;
        g_parameters.maxClusterNum = value;
      }
      else ALIB_THROW( L"Unknown field has been found" );

      ALIB_VERIFY( file.good(), L"File operation failed" );
    }

    if (!(alib::IsLimited( g_parameters.distThreshold,    0.001f,   2.0f )) ||
        !(alib::IsLimited( g_parameters.passedPercentage,  10.0f, 100.0f )) ||
        !(alib::IsLimited( g_parameters.neighbourhoodSize,     1,   1000 )))
      ALIB_THROW( L"Parameter is out of range" );
  }
  catch (std::runtime_error e)
  {
    g_parameters = Parameters();
    WStr s;
    ((s += L"ParseINIFile() failed:") += ALIB_NEWLINE) += (LPCWSTR)(CString( e.what() ));
    ALIB_THROW( s.c_str() );
  }
}

