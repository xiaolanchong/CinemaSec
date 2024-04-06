//======================================================================================//
//                                      CinemaSec                                       //
//                                                                                      //
//                              Copyright by ElVEES, 2005                               //
//                                                                                      //
//--------------------------------------------------------------------------------------//
// Description:
//	Helper class for video window layout processing & validation
//
//--------------------------------------------------------------------------------------//
// Author : Eugene Gorbachev (Eugene.Gorbachev@biones.com)
// Date   : 30.03.2005
//                                                                                      //
//======================================================================================//
#pragma	  warning( disable : 4995 )		//deprecated functions

#include "stdafx.h"
#include "LayoutValidate.h"
#include "../../CSEngine/DBFacet/CinemaDB.h"
//======================================================================================//
//                                 class LayoutValidate                                 //
//======================================================================================//
LayoutValidate::LayoutValidate()
{
}

LayoutValidate::~LayoutValidate()
{
}

DWORD	Pos2Index(WORD x, WORD y)
{
	return MAKELONG(y , x); 
}

std::pair<WORD, WORD>	Index2Pos(DWORD x)
{
	return  std::make_pair( HIWORD(x), LOWORD(x) ); 
}

bool	LayoutValidate::Process(  const std::vector< std::pair<int, int> >& LayoutArr ) const
{
	if( LayoutArr.empty() ) return false;
	std::set<DWORD>	LayoutPos; 
	for( size_t i = 0; i < LayoutArr.size() ; ++i )
	{
		if (	LayoutArr[i].first < 0									||
				LayoutArr[i].second < 0									||
				std::numeric_limits<WORD>::max() <  LayoutArr[i].first	||
				std::numeric_limits<WORD>::max() <  LayoutArr[i].second		)
				return false;
		DWORD Index = Pos2Index( (WORD)LayoutArr[i].first, (WORD)LayoutArr[i].second );
		std::pair < std::set<DWORD>::iterator, bool> p = LayoutPos.insert( Index );
		if( !p.second  ) return false;
	}
	return true;
}

bool	LayoutValidate::CheckLayoutInRoom( CCinemaDB& db, int nRoomID ) const
{
	std::vector< std::pair<int, int> > LayoutArr;
	const std::set<CameraRow>& Cams = db.GetTableCameraFacet().GetCameras();
	std::set<CameraRow>::const_iterator it = Cams.begin();
	for( ;it != Cams.end(); ++it )
	{
		if( it->m_nRoomID == nRoomID ) LayoutArr.push_back( it->m_Pos );
	}
	return Process( LayoutArr );
}

bool	LayoutValidate::Process( const SparceRectImageLayout& layout) const
{
	const SparceRectImageLayout::IndexArray_t& IndArr = layout.GetCells();
	std::vector< std::pair<int, int> > LayoutArr ( IndArr.size() ) ;
	for( size_t i = 0; i < IndArr.size(); ++i )
	{
		LayoutArr[i] = std::make_pair( IndArr[i].m_nX, IndArr[i].m_nY );
	}
	return Process( LayoutArr );
}