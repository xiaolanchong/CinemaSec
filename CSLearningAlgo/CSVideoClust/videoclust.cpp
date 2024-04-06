#include "stdafx.h"
#include "videoclust.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CWinApp theApp;

const TCHAR SEPARATOR_LINE[] = _T("---------------------------------------------------------------------------------");

int _tmain( int argc, TCHAR * argv[], TCHAR * envp[] )
{
  argc;argv;envp;
  if (!AfxWinInit( ::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0 ))
  {
    std::wcout << _T("Fatal Error: MFC initialization failed") << std::endl;
    return 1;
  }

  CFileFind dirFind, fileFind;
  WStr      dirPattern, videoPattern, camId, startPath, camPath, inFile, outFile, title;
  std::auto_ptr<alib::IProcessNotify> pNotifier;

  try
  {
    startPath = argv[0];
    startPath = alib::GetFilePath( startPath );
    alib::AddTrailingBackslash( startPath );
    dirPattern += L"cam*";

    // Open log file.
    {
      USES_CONVERSION;
      WStr logName;
      (logName = startPath) += L"VideoClust.log";
      global_log.open( W2CA(logName.c_str()), std::ios::out );
    }

    pNotifier.reset( CreateConsoleProcessNotifier() );
    ALIB_VERIFY( (argc == 2), L"INI-file must be specified as a single application parameter" );
    ParseINIFile( argv[1] );

    // Scan all subdirectories that has "cam" phrase starting from the program location.
    for (BOOL bDirContinue = dirFind.FindFile( dirPattern.c_str() ); bDirContinue;)
    {
      bDirContinue = dirFind.FindNextFile();
      if (!(dirFind.IsDirectory()))
        continue;

      camId = (LPCWSTR)(dirFind.GetFileName());
      camPath = (LPCWSTR)(dirFind.GetFilePath());
      alib::AddTrailingBackslash( camPath );

      // For all files is sudbdirectory ...
      for (int patternNo = 0; patternNo < 2; ++patternNo)
      {
        if (patternNo == 0)
          (videoPattern = camPath) += L"cam*_chair*.avd";
        else if (patternNo == 1)
          (videoPattern = camPath) += L"merged_cam*_chair*.avd";

        for (BOOL bFileContinue = fileFind.FindFile( videoPattern.c_str() ); bFileContinue;)
        {
          // Is normal video file? Does file name contain camera id substring?
          bFileContinue = fileFind.FindNextFile();
          if (!(fileFind.IsNormal()) && !(fileFind.IsArchived()))
            continue;
          inFile = (LPCWSTR)(fileFind.GetFilePath());
          if (inFile.find( camId ) == WStr::npos)
            continue;

          // Does processed file already exist?
          title = (LPCWSTR)(fileFind.GetFileTitle());
          if (title.find( (GetParameters()).prefix ) != title.npos)
            continue;

          outFile = inFile;
          WStr::size_type pos = outFile.find_last_of( _T("/\\") );
          outFile.insert( ((pos == outFile.npos) ? 0 : (pos+1)), (GetParameters()).prefix );

          // Process video file.
          if (pNotifier.get() != 0)
          {
            pNotifier->print( SEPARATOR_LINE );
            pNotifier->print( inFile.c_str() );
            pNotifier->print( SEPARATOR_LINE );
          }

          const Parameters & params = GetParameters();
          if (params.bDendroClustering)
          {
            std::vector<Arr2ub>         images;
            Arr2f                       weights;
            PairwiseDistances           distances;
            std::list< std::list<int> > clusters;

            LoadChairImages( inFile.c_str(), &images, &weights, pNotifier.get() );
            CalcPairwiseDistances( &images, &weights, &distances, params.bNormL1, pNotifier.get() );
            DoDendroClustering( &distances, &clusters, params.maxClusterNum, pNotifier.get() );
            WriteClusteringResult( outFile.c_str(), &clusters, &images, &distances, pNotifier.get() );
          }
          else
          {
            ClusterizeSequence( inFile.c_str(), outFile.c_str(), pNotifier.get() );
          }

          outFile.clear();
          ALIB_VERIFY( (_wremove( inFile.c_str() ) == 0), L"Failed to remove processed file" );
          std::cout << std::endl << std::endl;
        }
      }
    }
  }
  catch (std::runtime_error & e)
  {
    WStr s;

    if (!(outFile.empty()) && ((CFileFind()).FindFile( outFile.c_str() )) && _wremove( outFile.c_str() ))
      ((s += L"Failed to delete: ") += outFile) += ALIB_NEWLINE;

    ((s += ALIB_NEWLINE) += (LPCWSTR)CString( e.what() )) += ALIB_NEWLINE;

    if (pNotifier.get() != 0)
      pNotifier->PrintW( IDebugOutput::mt_error, s.c_str() );
    return 1;
  }
  return 0;
}

