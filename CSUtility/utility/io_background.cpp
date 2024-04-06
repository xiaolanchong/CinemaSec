/****************************************************************************
  io_background.cpp
  ---------------------
  begin     : Aug 2004
  author(s) : Albert Akhriev
  email     : Albert.Akhriev@biones.com
****************************************************************************/

#include "stdafx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//-------------------------------------------------------------------------------------------------
/** \brief Function loads the static background (empty hall) from a file.

  \param  fileName    the name of source file.
  \param  background  array of destination backgrounds of all subframes.
  \param  nSubFrames  the number of subframes.
  \return             Ok = true. */
//-------------------------------------------------------------------------------------------------
CS_UTIL_API bool LoadBackgroundImage( LPCWSTR fileName, Arr2f * background, int nSubFrames )
{
  if ((fileName == 0) || !ALIB_IS_LIMITED( nSubFrames, 1, MAX_SUBFRAME_NUM ))
    return false;

  Arr2f image;
  if (!LoadFloatImage( fileName, image, INVERT_AXIS_Y ))
    return false;

  int W = image.width();
  int H = image.height()/nSubFrames;

  // Weak check for consistency.
  if ((W == 0) || (H == 0) || (((image.height())%nSubFrames) != 0))
    return false;

  // Obtain background subframes from the large image.
  for (int k = 0; k < nSubFrames; k++)
  {
    (background[k]).resize( W, H );
    std::copy( image.begin()+(k*W*H), image.begin()+((k+1)*W*H), (background[k]).begin() );
  }
  return true;
}


//-------------------------------------------------------------------------------------------------
/** \brief Function saves accumulated background (empty hall) to a file.

  \param  fileName     the name of destination file.
  \param  backgrounds  array of source backgrounds of all subframes.
  \return              Ok = true. */
//-------------------------------------------------------------------------------------------------
CS_UTIL_API bool SaveBackgroundImage( LPCWSTR fileName, const FltImgArr & backgrounds )
{
  if ((fileName == 0) || !(alib::IsLimited( (int)(backgrounds.size()), 1, MAX_SUBFRAME_NUM )))
    return false;

  int   W = (backgrounds[0]).width();
  int   H = (backgrounds[0]).height();
  Arr2f image( W, H*(int)(backgrounds.size()) );

  // Copy background subframes to their positions in the large image.
  for (int k = 0; k < (int)(backgrounds.size()); k++)
  {
    if (!(alib::AreDimensionsEqual( backgrounds[0], backgrounds[k] )))
      return false;

    std::copy( (backgrounds[k]).begin(), (backgrounds[k]).end(), image.begin()+(k*W*H) );
  }

  return SaveFloatImage( fileName, image, INVERT_AXIS_Y );
}

