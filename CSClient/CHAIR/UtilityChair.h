// UtilityChair.h: interface for the UtilityChair class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UTILITYCHAIR_H__BB4DB1D4_C171_46DE_8352_571B10D58684__INCLUDED_)
#define AFX_UTILITYCHAIR_H__BB4DB1D4_C171_46DE_8352_571B10D58684__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DataPrepare/my_chair.h"
#include "Common/Exception.h"

//struct Chair;

MACRO_EXCEPTION( ChairPosException,  CommonException)

namespace Utility
{

inline Vec2i GetMassCenter( const Vec2fArr& curve )
{
  //
	Vec2i MassCenter(0, 0);
  for(DWORD i=0; i < curve.size(); ++i)
  {
		MassCenter.x += int(curve[i].x);
		MassCenter.y += int(curve[i].y);
  }
  MassCenter.x /= (int)(curve.size());
  MassCenter.y /= (int)(curve.size());
	return MassCenter;
  //
}

enum CHAIR_POS
{
	pos_top,
	pos_right,
	pos_left,
	pos_bottom
};

inline CHAIR_POS GetOpposite(CHAIR_POS pos)
{
	switch (pos)
	{
		case pos_top	: return pos_bottom;
		case pos_bottom : return pos_top;
		case pos_left	: return pos_right;
		case pos_right	: return pos_left;
		default : ASSERT(FALSE); return pos_top;
	}
}

struct MaxXPred
{
	bool operator()  ( const Vec2f& f, const Vec2f& s ) const { return f.x > s.x; }
};

struct MaxYPred
{
	bool operator()  ( const Vec2f& f, const Vec2f& s ) const { return f.y > s.y;}
};

template <typename TransformType>
CHAIR_POS GetMutualPos( const Vec2fArr& FirstCurve, TransformType offFirst,  
					    const Vec2fArr& SecondCurve, TransformType offSecond )
{
	Vec2i CenMassF = GetMassCenter( FirstCurve );
	Vec2i CenMassS = GetMassCenter( SecondCurve);

	float FMaxX = std::max_element( FirstCurve.begin(), FirstCurve.end(), MaxXPred() )->x;
	float FMinX = std::min_element( FirstCurve.begin(), FirstCurve.end(), MaxXPred() )->x;
	float FMaxY = std::max_element( FirstCurve.begin(), FirstCurve.end(), MaxYPred() )->y;
	float FMinY = std::min_element( FirstCurve.begin(), FirstCurve.end(), MaxYPred() )->y;

	CPoint FloatCenMassF = offFirst( Vec2f( float(CenMassF.x), float(CenMassF.y) ) );
	CPoint FloatCenMassS = offSecond( Vec2f( float(CenMassS.x), float(CenMassS.y) ) );

	CPoint FloatMaxF		= offFirst(	Vec2f( float(FMaxX), float(FMaxY) ) );
	CPoint FloatMinF		= offFirst(	Vec2f( float(FMinX), float(FMinY) ) );

	if( FloatMinF.y < FloatCenMassS.y ) return pos_top;
	else if( FloatMaxF.y > FloatCenMassS.y ) return pos_bottom;

	if( FloatMinF.x < FloatCenMassS.y ) return pos_left;
	else return pos_right;
//	ASSERT(FALSE);
//	return Utility::pos_top;
}

MyChair::Chair*&	GetChairPos(MyChair::Chair& ch, CHAIR_POS pos) THROW_HEADER (ChairPosException);	

}

#endif // !defined(AFX_UTILITYCHAIR_H__BB4DB1D4_C171_46DE_8352_571B10D58684__INCLUDED_)
