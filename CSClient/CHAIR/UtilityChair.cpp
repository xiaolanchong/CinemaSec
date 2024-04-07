// UtilityMyChair::Chair.cpp: implementation of the UtilityMyChair::Chair class.
//
//////////////////////////////////////////////////////////////////////
#pragma	  warning( disable : 4995 )		//deprecated functions

#include "stdafx.h"
#include "csclient.h"
#include "UtilityChair.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace Utility
{

MyChair::Chair*&	GetChairPos(MyChair::Chair& ch, CHAIR_POS pos)
{
	if( pos == pos_top )
	{
		DWORD i = 0;
		for( ; i < sizeof(ch.pBackChair)/sizeof(ch.pBackChair[0]); ++i )
		{
			if( !ch.pBackChair[i] ) return ch.pBackChair[i];
		}
	//	throw MyChair::ChairPosException("Can't locate empty back space")
		return ch.pBackChair[ i - 1 ];
	}
	else if( pos == pos_top )
	{
		DWORD i = 0;
		for( ; i < sizeof(ch.pFrontChair)/sizeof(ch.pFrontChair[0]); ++i )
		{
			if(! ch.pFrontChair[i] ) return ch.pFrontChair[i];
		}
	//	throw MyChair::ChairPosException("Can't locate empty back space")
		return ch.pFrontChair[ i - 1 ];
	}
	else if( pos == pos_left ) return ch.pLeftChair;
	else return ch.pRightChair;
}

}