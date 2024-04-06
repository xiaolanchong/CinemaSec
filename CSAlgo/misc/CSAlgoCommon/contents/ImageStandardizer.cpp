/****************************************************************************
  ImageStandardizer.cpp
  ---------------------
  begin     : 25 Jul 2005
  author(s) : Albert Akhriev
  email     : Albert.Akhriev@biones.com
****************************************************************************/

#include "stdAfx.h"
#include "ImageStandardizer.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//-------------------------------------------------------------------------------------------------
/** \brief Function calculates the positive ceiling of power of two of a value. */
//-------------------------------------------------------------------------------------------------
inline int RoundToPowerOfTwo( int x )
{
  if (x > 0)
  {
    for (int s = 0; s < (8*sizeof(int)); s++)
    {
      if (x <= (1<<s))
        return (1<<s);
    }
  }
  return 0;
}


//-------------------------------------------------------------------------------------------------
/** \brief Constructor. */
//-------------------------------------------------------------------------------------------------
ImageStandardizer::ImageStandardizer()
{
}


//-------------------------------------------------------------------------------------------------
/** \brief Destructor. */
//-------------------------------------------------------------------------------------------------
ImageStandardizer::~ImageStandardizer()
{
}


//-------------------------------------------------------------------------------------------------
/** \brief Function clears this object. */
//-------------------------------------------------------------------------------------------------
void ImageStandardizer::Clear()
{
}


//-------------------------------------------------------------------------------------------------
/** \brief Function . */
//-------------------------------------------------------------------------------------------------
void ImageStandardizer::Initialize( bool bMeanNormalize, int resultWidth, int resultHeight, int factor )
{


  m_intermediateImg.resize( resultWidth * factor, resultHeight * factor );

  m_resultWidth = resultWidth;
  m_resultHeight = resultHeight;

  for (int i = 0; i < resultWidth; i++)
  {
	  for (int j = 0; j < resultWidth; j++)
	  {
		  points.push_back(Vec2i(i, j));
	  }
  }

  m_bMeanNormalize = bMeanNormalize;
  m_bReady = true;
}


//-------------------------------------------------------------------------------------------------
/** \brief Function . */
//-------------------------------------------------------------------------------------------------
bool ImageStandardizer::GetQuadrangleFromCurve( const Vec2fArr & curve,
                                                float            inflation,
                                                Vec2f            quadrangle[4] )
{
  //Algorithm
  /* 
  1) Find the most distant points A, B from each other in N^2 operations.
  Now we have the diagonal A B
  2) Find the most distant from AB points on both sides of AB, we call them
  C and D.
  3) Order points A, B, C, D as desired
  FIN		
  */

  //Implementation 
  // 1)

  // we set StartP equal to point with index 0
  // I don’t use iterators to obtain more uniform and readable code
  // if works too slow it needs to be redone

  quadrangle[0] = (quadrangle[1] = (quadrangle[2] = (quadrangle[3] = Vec2f(0.0f,0.0f))));
  if (curve.empty())
    return false;

  int indexA = -1, indexB = -1;
  int curveSize = (int)curve.size();
  if (curveSize == 0) OutputDebugString(_T("!!!!")); 
  double GlobalMax = 0;

  for (int StartP = 0; StartP < curveSize; StartP++)
  {
    int maxIndex = 0;

    float x0 = curve[StartP].x;
    float y0 = curve[StartP].y;

    float max = 0;
    for(int i = (StartP + 1) % curveSize; i != StartP; i = (i + 1) % curveSize)
    {
      float dx = curve[i].x - x0;
      float dy = curve[i].y - y0;
      float d2 = dx * dx + dy * dy;

      if (d2 > max)
      {
        max      = d2;
        maxIndex = i;
        if (max > GlobalMax)
        {
          indexA    = StartP;
          indexB    = maxIndex;
          GlobalMax = max;

        }//end if
      }//end if
    }//end for
  }//end for


  //2)

  // equation for our line is lineA * x + lineB * y + lineC == 0

  float lineA = -(curve[indexA].y - curve[indexB].y);
  float lineB =   curve[indexA].x - curve[indexB].x;

  float lineC = - lineA * curve[indexA].x - lineB * curve[indexA].y;


  int   indexC = 0;
  float max = 0;

  for(int i = indexA; i != indexB; i = (i + 1) % curveSize)
  {
    float d = (float)fabs (lineA * curve[i].x + lineB * curve[i].y + lineC);
    // I omit division by sqrt(A * A + B * B)
    if (d > max)
    {
      max      = d;
      indexC = i;		
    }//end if
  }//end for

  int   indexD = 0;
  max = 0;
  for(i = (indexB + 1) % curveSize; i != indexA; i = (i + 1) % curveSize)
  {
    float d = (float)fabs (lineA * curve[i].x + lineB * curve[i].y + lineC);
    // I omit division by sqrt(A * A + B * B)
    if (d > max)
    {
      max    = d;
      indexD = i;		
    }//end if
  }//end for



  quadrangle[0] = curve[indexA];
  quadrangle[1] = curve[indexB];
  quadrangle[2] = curve[indexC];
  quadrangle[3] = curve[indexD];


  //3)
  // ordering
  Vec2f tmp;

  // by Y

  for (int j = 0; j < 4; j++)
  {
    for (i = 0; i < 3; i++)
    {
      if (quadrangle[i].y > quadrangle[i + 1].y)
      {
        tmp                   = quadrangle[i];
        quadrangle[i]     = quadrangle[i + 1];
        quadrangle[i + 1] = tmp;
      }
    }
  }

  //special case: by X

  if (quadrangle[0].x > quadrangle[1].x)
  {
    tmp                  = quadrangle[0];
    quadrangle[0]     = quadrangle[1];
    quadrangle[1]     = tmp;
  }

  if (quadrangle[2].x < quadrangle[3].x)
  {
    tmp                  = quadrangle[2];
    quadrangle[2]     = quadrangle[3];
    quadrangle[3]     = tmp;
  }

  if (inflation > FLT_EPSILON)
  {
    Vec2f center = (quadrangle[0] + quadrangle[1] + quadrangle[2] + quadrangle[3]) * 0.25f;

    quadrangle[0] += (quadrangle[0] - center) * inflation;
    quadrangle[1] += (quadrangle[1] - center) * inflation;
    quadrangle[2] += (quadrangle[2] - center) * inflation;
    quadrangle[3] += (quadrangle[3] - center) * inflation;
  }

  return true;
}


//-------------------------------------------------------------------------------------------------
/** \brief Function . */
//-------------------------------------------------------------------------------------------------
void ImageStandardizer::RectToQuadrangleTransformCoefs( const Vec2f quadrangle[4],
                                                        int         width,
                                                        int         height,
                                                        float       coefs[8] )
{
  double x0 = (quadrangle[0]).x;
  double y0 = (quadrangle[0]).y;

  double x1 = (quadrangle[1]).x;
  double y1 = (quadrangle[1]).y;

  double x2 = (quadrangle[2]).x;
  double y2 = (quadrangle[2]).y;

  double x3 = (quadrangle[3]).x;
  double y3 = (quadrangle[3]).y;

  double Det  = (x2 - x1) * (y2 - y3) - (y2 - y1) * (x2 - x3);
  double DetE = (x2 - x0) * (y2 - y3) - (y2 - y0) * (x2 - x3);
  double DetF = (x2 - x1) * (y2 - y0) - (y2 - y1) * (x2 - x0);
  double E = DetE / Det;
  double F = DetF / Det;

  double a = x1 * E - x0;
  double b = x3 * F - x0;
  double c = y1 * E - y0;
  double d = y3 * F - y0;
  double e = E - 1.0;
  double f = F - 1.0;

  coefs[0] = (float)(a/(double)width);
  coefs[1] = (float)(b/(double)height);
  coefs[2] = (float)(x0);
  coefs[3] = (float)(c/(double)width);
  coefs[4] = (float)(d/(double)height);
  coefs[5] = (float)(y0);
  coefs[6] = (float)(e/(double)width);
  coefs[7] = (float)(f/(double)height);
}


//-------------------------------------------------------------------------------------------------
/** \brief Function . */
//-------------------------------------------------------------------------------------------------
void ImageStandardizer::QuadrangleToRectTransformCoefs( const Vec2f quadrangle[4],
                                                        int         width,
                                                        int         height,
                                                        float       coefs[8] )
{
  float invCoefs[8];
  RectToQuadrangleTransformCoefs( quadrangle, width, height, invCoefs );

  // inversed coefs
  // fx = (a x + b y + c) / (g x + h y + 1 ) 
  // fy = (d x + e y + f) / (g x + h y + 1 )

  double ai = invCoefs[0];
  double bi = invCoefs[1];
  double ci = invCoefs[2];
  double di = invCoefs[3];
  double ei = invCoefs[4];
  double fi = invCoefs[5];
  double gi = invCoefs[6];
  double hi = invCoefs[7];

  double divider = bi * di - ai * ei;

  double a = ( fi * hi - ei ) / divider;
  double b = ( bi - ci * hi ) / divider;
  double c = ( ci * ei - bi * fi ) / divider;

  double d = ( di - fi * gi ) / divider;
  double e = ( ci * gi - ai ) / divider;
  double f = ( ai * fi - ci * di ) / divider;

  double g = ( ei * gi - di * hi ) / divider;
  double h = ( ai * hi - bi * gi ) / divider;

  coefs[0] = (float)a;
  coefs[1] = (float)b;
  coefs[2] = (float)c;
  coefs[3] = (float)d;
  coefs[4] = (float)e;
  coefs[5] = (float)f;
  coefs[6] = (float)g;
  coefs[7] = (float)h;
}


bool ImageStandardizer::GetStandardImage( const Arr2f & bigImage, const Vec2fArr & curve, Arr2ub & result, float inflation /* = 0.1 */)
{
	Vec2f quadrangle[4];
	GetQuadrangleFromCurve( curve, inflation, quadrangle );
	return GetStandardImage( bigImage, quadrangle, result );	
}


//-------------------------------------------------------------------------------------------------
/** \brief Function . */
//-------------------------------------------------------------------------------------------------
bool ImageStandardizer::GetStandardImage( const Arr2f & bigImage,
                                          const Vec2f   quadrangle[4],
                                          Arr2ub      & result )
{
	result.resize( m_resultWidth, m_resultHeight, 0, false );
	if (!m_bReady)
	{
		result.fast_zero();
		return false;
	}

	float coefs[8];
	RectToQuadrangleTransformCoefs( quadrangle, m_intermediateImg.width(), m_intermediateImg.height(), coefs );

	for (int i = 0; i < m_intermediateImg.width(); i++)
	{
		for (int j = 0; j < m_intermediateImg.height(); j++)
		{
			float x = (float)i;
			float y = (float)j;

			ProjectiveTransform( coefs, x, y );
			m_intermediateImg( i, j ) = bigImage.bilin_at( x, y );
		}
	}

	int wRect = m_intermediateImg.width() / m_resultWidth;
	int hRect = m_intermediateImg.height() / m_resultHeight;

	for (int i = 0; i < m_resultWidth; i++)
	{
		for (int j = 0; j < m_resultHeight; j++)
		{   
			float color = 0;
			for (int x = 0; x < wRect; x++)
			{
				for (int y = 0; y < hRect; y++)
				{
					color += m_intermediateImg( i * wRect + x, j * hRect + y );
				}
			}
			m_intermediateImg( i, j ) = color / (float)(wRect * hRect);
		}
	}

	csalgocommon::ImageNormalizer<Arr2f,Vec2iArr> normalizer;
	normalizer.Initialize( m_intermediateImg, points, m_bMeanNormalize );
		
	result.resize(m_resultWidth, m_resultHeight, 0, false);
	for (int i = 0; i < m_resultWidth; i++)
	{
		for (int j = 0; j < m_resultHeight; j++)
		{
			result(i, j) = normalizer.GetByteNormalizedValue( m_intermediateImg(i, j) );
		}
	}
		
	return true;

}

