#include "stdafx.h"

#ifdef ALIB_CHECK_MEMORY_LEAKAGE
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace vislib
{

//-------------------------------------------------------------------------------------------------
/** \brief Function locally scales gradient map so that all gradient peaks (edges),
           whose values are above threshold, have standard unit magnitude at the output.

  \param  dirs        gradient directions.
  \param  grads       gradients to be equated.
  \param  pMask       if non-zero, the mask excludes some pixels (mask[x,y]==0) from consideration.
  \param  pRect       rectangle bounds the region of interest (0 == entire image).
  \param  minGradThr  threshold on minimal gradient.
  \param  pathStep    step along the path in gradient map.
  \param  bNoDirSign  TRUE if signs of gradient directions are unknown.
  \param  bSqGrad     if TRUE then equated gradients will be squared.
  \param  eqGrads     out: equated gradients. */
//-------------------------------------------------------------------------------------------------
ALIB_API void EquateGradientPeaks(
  const Vec2fImg & dirs,
  const Arr2f    & grads,
  const Arr2ub   * pMask,
  const ARect    * pRect,
  float            minGradThr,
  float            pathStep,
  bool             bNoDirSign,
  bool             bSqGrad,
  Arr2f          & eqGrads )
{
  Vec2f  pnt, first, dir;
  Vec2i  ip;
  int    W = grads.width();
  int    H = grads.height();
  float  max, val, coefs[4];
  ARect  rect = (pRect != 0) ? (*pRect) : grads.rect();
  bool   isMax[3];

  ASSERT( alib::AreDimensionsEqual( grads, dirs ) );
  ASSERT( (pMask == 0) || alib::AreDimensionsEqual( grads, *pMask ) );
  ASSERT( &grads != &eqGrads );

  minGradThr = std::max( minGradThr, 0.01f );
  eqGrads.resize( W, H );
  rect.inflate( 1, 1 );
  rect &= grads.rect();
  rect.deflate( 1, 1 );

  if (!(rect.is_normalized()))
    return;

  for (int y = rect.y1; y < rect.y2; y++)
  {
    for (int x = rect.x1; x < rect.x2; x++)
    {
      if ((pMask != 0) && ((*pMask)( x, y ) == 0))
        continue;

      first = dirs( x, y );
      max = grads( x, y );
      isMax[0] = (isMax[1] = (isMax[2] = false));

      for (int dirNo = 0; dirNo < 2; dirNo++)
      {
        dir = first;
        first = -first;
        pnt.set( (float)x, (float)y );
        int count = 0;

        while (max > minGradThr)
        {
          if (dir.sq_length() < 0.5)
            break;           // we are in the region where direction is unreliablely defined

          pnt += dir * pathStep;
          if (!(alib::BilinearCoefs( W, H, pnt, &ip, coefs )))
            break;           // we are near border

          int offset = grads.offset( ip );

          val = grads[ offset     ] * coefs[0] +
                grads[ offset+1   ] * coefs[1] +
                grads[ offset+W+1 ] * coefs[2] +
                grads[ offset+W   ] * coefs[3];

          if (max > val)
          {
            isMax[ (count == 0) ? dirNo : 2 ] = true;
            break;
          }
          max = val;
          ++count;

          const Vec2f & d0 = dirs[ offset     ];
          const Vec2f & d1 = dirs[ offset+1   ];
          const Vec2f & d2 = dirs[ offset+W+1 ];
          const Vec2f & d3 = dirs[ offset+W   ];

          if (bNoDirSign)
          {
            if ((dir & d0) < 0) coefs[0] = -coefs[0];
            if ((dir & d1) < 0) coefs[1] = -coefs[1];
            if ((dir & d2) < 0) coefs[2] = -coefs[2];
            if ((dir & d3) < 0) coefs[3] = -coefs[3];
          }

          dir = d0 * coefs[0] +
                d1 * coefs[1] +
                d2 * coefs[2] +
                d3 * coefs[3];
        }
      }
      
      if (isMax[2] || (isMax[0] && isMax[1]))
      {
        eqGrads( x, y ) = (bSqGrad ? alib::Sqr( grads( x, y ) / max ) : ( grads( x, y ) / max ));
      }
    }
  }
}

} // namespace vislib

