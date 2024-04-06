//////////////////////////////////////////////////////////////////////////
//date  : 4 March 2005
//author: Alexander Boltnev
//e-mail: Alexander.Boltnev@biones.com
//////////////////////////////////////////////////////////////////////////
#pragma once

struct IPlayList
{
  // loads play list from xml
  virtual HRESULT LoadPlayList( LPCWSTR fileName ) = 0;

  // returns the total number of items in play list
  virtual size_t size() const = 0;

  // returns Playlist item at position index 
  virtual const PlaylistItem & operator [] ( size_t index ) const = 0;

  // marks item at position index as "processed"
  virtual void MarkAsProcessed( size_t index ) = 0;

  // tests if item at position index is already processed or not
  virtual bool IsItemProcessed( size_t index ) = 0;

  // saves play list to xml 
  virtual HRESULT SavePlayList( LPCWSTR fileName ) = 0;

  // destroy implementation of this interface
  virtual void Release() = 0;

  // destroy implementation of this interface
  virtual void GetResultDirectory(std::wstring& resPath) = 0;
};

