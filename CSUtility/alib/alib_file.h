/******************************************************************************
  alib_file.h
  ---------------------
  begin     : 1998
  modified  : 24 Jul 2005
  author(s) : Albert Akhriev
  copyright : Albert Akhriev
  email     : aaah@mail.ru, Albert.Akhriev@biones.com
******************************************************************************/

#pragma once

namespace alib
{

#define  _ALIB_USE_WINDOWS_FILE_API_  1

//=================================================================================================
/** \class BinaryFile.
    \brief Class encapsulates simple I/O operations.

  Functions throw alib::GeneralError exception on failure. */
//=================================================================================================
class /*ALIB_API*/ BinaryFile
{
private: 
#if _ALIB_USE_WINDOWS_FILE_API_
  int     m_file;       //!< file identifier
#else
  FILE *  m_file;       //!< pointer to the currently opened file
#endif
  StdStr  m_name;       //!< file's name
  bool    m_bRead;      //!< nonzero if a file must be opened in read mode

public:
  BinaryFile();
  BinaryFile( LPCWSTR fname, bool bRead, bool bRW = false );
  ~BinaryFile();

  void open( LPCWSTR fname, bool bRead, bool bRW = false );
  void close();

  void read( void * buffer, int size );
  void write( const void * buffer, int size );
  void seek( __int64 position, int origin );

  __int64 size() const;
  bool reading() const { return m_bRead; }

  LPCWSTR name() const { return m_name.c_str(); }
};


///////////////////////////////////////////////////////////////////////////////////////////////////
// Global functions.
///////////////////////////////////////////////////////////////////////////////////////////////////

// alib_file.cpp:

/*ALIB_API*/ void RemoveTrailingBackslash( StdStr & name );

/*ALIB_API*/ void AddTrailingBackslash( StdStr & name );

/*ALIB_API*/ StdStr GetFileName( const StdStr & name );

/*ALIB_API*/ StdStr GetFilePath( const StdStr & name );

/*ALIB_API*/ StdStr GetFileTitle( const StdStr & name );

/*ALIB_API*/ StdStr GetFileExt( const StdStr & name );

/*ALIB_API*/ StdStr GetFileLabel( const StdStr & name );

/*ALIB_API*/ StdStr ReplaceFileExt( const StdStr & name, LPCWSTR newExt );

/*ALIB_API*/ StdStr EnhanceFileName( const StdStr & name, LPCWSTR extra );

/*ALIB_API*/ bool IsFileExistAndAccessible( LPCWSTR fname );

/*ALIB_API*/ LPCWSTR GetFileNamePtr( const StdStr & name );

/*ALIB_API*/ bool ConstructNameOfSequenceFile( LPCWSTR first, int index, StdStr & name, bool bFull = true );

/*ALIB_API*/ int GetSequenceIncrement( LPCWSTR first );

/*ALIB_API*/ bool FillListOfFiles( LPCWSTR spec, StdStrLst & fileNames );

} // namespace alib

