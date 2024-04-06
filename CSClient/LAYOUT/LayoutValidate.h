//======================================================================================//
//                                      CinemaSec                                       //
//                                                                                      //
//                              Copyright by ElVEES, 2005                               //
//                                                                                      //
//--------------------------------------------------------------------------------------//
// Description:
//	Helper class for video window layout processing & validation
//  Assume all layeot position is non-negative and less than 0xFFFF
//--------------------------------------------------------------------------------------//
// Author : Eugene Gorbachev (Eugene.Gorbachev@biones.com)
// Date   : 30.03.2005
//                                                                                      //
//======================================================================================//
#ifndef _LAYOUT_VALIDATE_7948094665468123_
#define _LAYOUT_VALIDATE_7948094665468123_

#include "../MultiImageLayout.h"
//======================================================================================//
//                                 class LayoutValidate                                 //
//======================================================================================//

class CCinemaDB;

class LayoutValidate
{
public:
	LayoutValidate( );

	bool	Process(  const std::vector< std::pair<int, int> >& LayoutArr ) const;
	bool	Process( const SparceRectImageLayout& layout) const ; 
	bool	CheckLayoutInRoom( CCinemaDB& db, int nRoomID ) const ; 

	~LayoutValidate();
};

#endif // _LAYOUT_VALIDATE_7948094665468123_