#include "stdAfx.h"
#include "CSdataPrepareGlobals.h"
#include "WalshTransform.h"



CSDataPrepareGlobals::CSDataPrepareGlobals(int nPictureSide, int nSmallSquareSide)
{
	this->nPictureSide = nPictureSide;
	this->nSmallSquareSide = nSmallSquareSide;
}
CSDataPrepareGlobals::~CSDataPrepareGlobals()
{
}

void CSDataPrepareGlobals::GetSquareImageFromCurve(const Arr2ub& bigImage, const Vec2fArr& curve, Arr2f& outImage) const
{
	Vec2f inscQuadr[4];
	GetInscribedQuadrFromCurve(curve, inscQuadr);
	GetSquareImage(bigImage, inscQuadr, outImage);
}

void CSDataPrepareGlobals::GetBigSquareImageFromCurve(const Arr2ub& bigImage, const Vec2fArr& curve, Arr2f& outImage, const double inflation /* = 0.1*/) const
{
	Vec2f inscQuadr[4];

	GetInscribedQuadrFromCurve(curve, inscQuadr);

	Vec2f center;
	Vec2f dv;

	center = (inscQuadr[0] + inscQuadr[1] + inscQuadr[2] + inscQuadr[3]);
	center.x *= 0.25;
	center.y *= 0.25;

	for (int i = 0; i < 4; i++)
	{
		dv   = inscQuadr[i] - center;
		dv.x *= inflation;
		dv.y *= inflation;

		inscQuadr[i] += dv;
	}

	GetSquareImage(bigImage, inscQuadr, outImage);
}

void CSDataPrepareGlobals::GetWalshCoefs(const Arr2f& image, std::vector<float>& vWalshCoefs) const
{

	float * floatArr  = new float[nPictureSide*nPictureSide];

	for (int i = 0; i < nPictureSide; i++)
		for(int j = 0; j < nPictureSide; j++)
		{
			floatArr[i + j*nPictureSide] = image(i, j);
		}


		DoWalshTransform(floatArr, nPictureSide*nPictureSide, 2);
		//do some thing with Walsh coefficients matrix

		vWalshCoefs.clear();	
		for (i = 0; i < nPictureSide*nPictureSide; i++)
		{
			vWalshCoefs.push_back(floatArr[i]);
		}
		delete [] floatArr;
}

void CSDataPrepareGlobals::GetAverageVector(const Arr2f &image, std::vector <float>& vAverage) const
{
	vAverage.clear();
	for (int i = 0; i < nPictureSide; i += nSmallSquareSide)
		for (int j = 0; j < nPictureSide; j += nSmallSquareSide)
		{
			float avColor = 0;
			for (int p = 0; p < nSmallSquareSide; p++ )
				for (int q = 0; q < nSmallSquareSide; q++)
					avColor += image( j + q, i + p );

			avColor = avColor / (float)(nSmallSquareSide * nSmallSquareSide);
			vAverage.push_back(avColor);			
		}		
}
void CSDataPrepareGlobals::GetLowResolutionImage(const Arr2f &image, Arr2f &loResImage ) const
{
	std::vector <float> vAverage;
	GetAverageVector(image, vAverage);
	int smallSide = nPictureSide / nSmallSquareSide;
	loResImage.resize( smallSide, smallSide );
	for (int i = 0; i < smallSide; i++)
		for (int j = 0; j < smallSide; j++)
		{
			loResImage(i, j) = vAverage[i + j * smallSide];
		}
}


void CSDataPrepareGlobals::GetHist(const Arr2f& image, std::vector <long>& hist) const
{
	Arr2f::const_iterator iter = image.begin();
	hist.resize(256, 0);
	for (;iter != image.end();)
	{
		hist[(unsigned char)(*iter)] += 1;
		iter++;
	}
}


// Calculates brightness of the image
float CSDataPrepareGlobals::GetBrightness(const Arr2f& image) const
{
	Arr2f::const_iterator  iter = image.begin();
	float brightness = 0.0;
	for(; iter != image.end();)
	{
		brightness += (*iter);
		iter++;		
	}
	return brightness / ((float)(image.height() * image.width()));
}



// Calculates deviation of the image
float CSDataPrepareGlobals::GetDeviation(const Arr2f& image) const
{
	Arr2f::const_iterator  iter = image.begin();

	float brightness = GetBrightness(image);

	float deviation = 0.0;
	for(; iter != image.end();)
	{
		deviation += (brightness - (*iter)) * (brightness - (*iter));
		iter++;		
	}

	return sqrt(deviation / (float)(image.width() * image.height()));
}

void CSDataPrepareGlobals::NormalizeVector(std::vector <float>& vVector, int p) const
{
	VERIFY(p == 1 || p == 2);

	if (p == 1)
	{
		float sumAbsVal = 0;
		for (unsigned int i = 0; i < vVector.size(); i++)
		{
			sumAbsVal += (float) fabs(vVector[i]);
		}

		VERIFY (sumAbsVal != 0);

		for (i = 0; i < vVector.size(); i++)
		{
			vVector[i] = vVector[i] / sumAbsVal;
		}
	} 
	else if ( p == 2)
	{
		float sumSqrVal = 0;
		for (unsigned int i = 0; i < vVector.size(); i++)
		{
			sumSqrVal += (float) fabs(vVector[i])* (float)   fabs(vVector[i]);
		}
		sumSqrVal = (float) sqrt(sumSqrVal);


		VERIFY (sumSqrVal != 0);

		for (i = 0; i < vVector.size(); i++)
		{
			vVector[i] = vVector[i] / sumSqrVal;
		}
	}	
}

void CSDataPrepareGlobals::NormalizeImage(Arr2f& image) const
{
	int w = image.width();
	int h = image.height();

	float avc = 0;

	for (int i = 0; i < w; i++)
		for (int j = 0; j < h; j++)
			avc = avc  + image(i, j);

	avc = avc / ((float)(w*h));	

	float dev = 0; // deviation

	for (i = 0; i < w; i++)
		for (int j = 0; j < h; j++)
		{
			image(i, j) =  image(i, j) - avc;			
			dev         =  dev + image(i, j) * image(i, j);
		}

		dev = (float)sqrt(dev);
		for (i = 0; i < w; i++)
			for (int j = 0; j < h; j++)
				image(i, j)  = image(i, j) / dev;

}

void CSDataPrepareGlobals::NormalizeImageBrightness(Arr2f& image) const
{
	int w = image.width();
	int h = image.height();

	float avc = 0;

	for (int i = 0; i < w; i++)
		for (int j = 0; j < h; j++)
			avc = avc  + image(i, j);

	avc = avc / ((float)(w*h));	

	for (i = 0; i < w; i++)
	{
		for (int j = 0; j < h; j++)
		{
			image(i, j) =  image(i, j) / std::max<float>(1.0f, avc);			
		}
	}
}


Arr2f CSDataPrepareGlobals::GetLevelsNoParams(const Arr2f image) const
{
	float levelA, levelB;
	GetThresholds(image, levelA, levelB);

	Arr2f answer = image;

	Arr2f::const_iterator iteri = image.begin();
	Arr2f::iterator itera = answer.begin();

	for (;iteri != image.end();)
	{
		if ( *iteri < levelA )
			*itera = 0;
		else if ( *iteri > levelB)
			*itera = 255;
		else 
			*itera = 100;
		iteri++;			
		itera++;
	}
	return answer;
}


inline void CSDataPrepareGlobals::RectToQuadrangleTransformCoefs(const double quadranglePoints[8], const int nWidth, const int nHeight, double Coefs[8]) const
{
	double x0 = quadranglePoints[0];
	double y0 = quadranglePoints[1];
	double x1 = quadranglePoints[2];
	double y1 = quadranglePoints[3];
	double x2 = quadranglePoints[4];
	double y2 = quadranglePoints[5];
	double x3 = quadranglePoints[6];
	double y3 = quadranglePoints[7];

	double Det  = (x2 - x1) * (y2 - y3) - (y2 - y1) * (x2 - x3);
	double DetE = (x2 - x0) * (y2 - y3) - (y2 - y0) * (x2 - x3);
	double DetF = (x2 - x1) * (y2 - y0) - (y2 - y1) * (x2 - x0);
	double E = DetE / Det;
	double F = DetF / Det;

	double a = x1 * E - x0;
	double b = x3 * F - x0;
	double c = y1*E-y0;
	double d = y3*F-y0;
	double e = E - 1.0, f = F - 1.0;

	Coefs[0] = a/(double)nWidth;
	Coefs[1] = b/(double)nHeight;;
	Coefs[2] = x0;
	Coefs[3] = c/(double)nWidth;;
	Coefs[4] = d/(double)nHeight;
	Coefs[5] = y0;
	Coefs[6] = e/(double)nWidth;;
	Coefs[7] = f/(double)nHeight;;
}

void CSDataPrepareGlobals::QuadrangleToRectTransformCoefs(const double quadranglePoints[8], const int nWidth, const int nHeight, double Coefs[8]) const
{
	double invCoefs[8]; 
	RectToQuadrangleTransformCoefs(quadranglePoints, nWidth, nHeight, invCoefs);
	// inversed coefs
	//
	// fx = (a x + b y + c) / (g x + h y + 1 ) 
	// fy = (d x + e y + f) / (g x + h y + 1 )
	//
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

	Coefs[0] = a;
	Coefs[2] = b;
	Coefs[3] = c;
	Coefs[4] = d;
	Coefs[5] = e;
	Coefs[6] = f;
	Coefs[7] = g;
	Coefs[8] = h;
}

void CSDataPrepareGlobals::GetSquareImage(const Arr2ub& bigImage, const Vec2f quadrangle[4], Arr2f& outImage) const
{
	double quadranglePoints[8];

	quadranglePoints[0] = quadrangle[0].x;
	quadranglePoints[1] = quadrangle[0].y;
	quadranglePoints[2] = quadrangle[1].x;
	quadranglePoints[3] = quadrangle[1].y;
	quadranglePoints[4] = quadrangle[2].x;
	quadranglePoints[5] = quadrangle[2].y;
	quadranglePoints[6] = quadrangle[3].x;
	quadranglePoints[7] = quadrangle[3].y;

	int sqSide = nPictureSide;
	outImage.resize(sqSide, sqSide);
	double Coefs[8];
	RectToQuadrangleTransformCoefs(quadranglePoints, sqSide, sqSide, Coefs);

	double x,  y;  //these points for input of mapping
	double fx, fy; //these points for output of mapping

	for (int i = 0; i < sqSide; i++)
		for (int j = 0; j < sqSide; j++)
		{
			x = (double)i + 0.5;
			y = (double)j + 0.5;
			ProjectionMapping(Coefs, x, y, fx, fy);
			outImage(i, j) = bigImage.bilin_at((float)fx, (float)fy);
		}
}

void CSDataPrepareGlobals::GetInscribedQuadrFromCurve(const Vec2fArr& myCurve, Vec2f inscribedQuadr[4]) const
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



	int indexA = -1, indexB = -1;
	int curveSize = (int)myCurve.size();
	if (curveSize == 0) OutputDebugString(_T("!!!!")); 
	double GlobalMax = 0;

	for (int StartP = 0; StartP < curveSize; StartP++)
	{
		int maxIndex = 0;

		float x0 = myCurve[StartP].x;
		float y0 = myCurve[StartP].y;

		float max = 0;
		for(int i = (StartP + 1) % curveSize; i != StartP; i = (i + 1) % curveSize)
		{
			float dx = myCurve[i].x - x0;
			float dy = myCurve[i].y - y0;
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

	float lineA = -(myCurve[indexA].y - myCurve[indexB].y);
	float lineB =   myCurve[indexA].x - myCurve[indexB].x;

	float lineC = - lineA * myCurve[indexA].x - lineB * myCurve[indexA].y;


	int   indexC = 0;
	float max = 0;

	for(int i = indexA; i != indexB; i = (i + 1) % curveSize)
	{
		float d = (float)fabs (lineA * myCurve[i].x + lineB * myCurve[i].y + lineC);
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
		float d = (float)fabs (lineA * myCurve[i].x + lineB * myCurve[i].y + lineC);
		// I omit division by sqrt(A * A + B * B)
		if (d > max)
		{
			max    = d;
			indexD = i;		
		}//end if
	}//end for



	inscribedQuadr[0] = myCurve[indexA];
	inscribedQuadr[1] = myCurve[indexB];
	inscribedQuadr[2] = myCurve[indexC];
	inscribedQuadr[3] = myCurve[indexD];


	//3)
	// ordering
	Vec2f tmp;

	// by Y

	for (int j = 0; j < 4; j++)
	{
		for (i = 0; i < 3; i++)
		{
			if (inscribedQuadr[i].y > inscribedQuadr[i + 1].y)
			{
				tmp                   = inscribedQuadr[i];
				inscribedQuadr[i]     = inscribedQuadr[i + 1];
				inscribedQuadr[i + 1] = tmp;
			}
		}
	}

	//special case: by X

	if (inscribedQuadr[0].x < inscribedQuadr[1].x)
	{
		tmp                  = inscribedQuadr[0];
		inscribedQuadr[0]     = inscribedQuadr[1];
		inscribedQuadr[1]     = tmp;
	}

	if (inscribedQuadr[2].x > inscribedQuadr[3].x)
	{
		tmp                  = inscribedQuadr[2];
		inscribedQuadr[2]     = inscribedQuadr[3];
		inscribedQuadr[3]     = tmp;
	}
}

// calculates thresholds by the Albert's algorithm
void CSDataPrepareGlobals::GetThresholds(const Arr2f& image, float & t1, float & t2)  const
{
	std::vector <long> hist;
	GetHist(image, hist);

	std::vector <long> vSk; // vector for dispersion
	std::vector <long> vMk; // vector for averages
	std::vector <long> vSh;

	long summSk = 0;
	long summMk = 0;
	long summSh = 0;	

	for (size_t i = 0; i < hist.size(); i++)
	{
		summSk += (long)(i*i)*hist[i];
		summMk += (long)( i )*hist[i];
		summSh += hist[i];
		vSk.push_back(summSk);
		vMk.push_back(summMk);
		vSh.push_back(summSh);
	}

	// t stands for thresholds 
	size_t tLeft  = 0; //max<long>(0,   (brightness - deviation));
	size_t tRight = 255; //min<long>(255, (brightness + deviation));
	long brightness = (long)GetBrightness(image);

	size_t pBest = tLeft;
	size_t qBest = tRight;
	float summBest = 1e30f;

	// search for minimum d1^2 + d2^2 + d3^2	
	// also 0 <= tLeft < p < q < tRight <= 255
	size_t p, q;

	for (p = tLeft + 1; p < (size_t)brightness; p++)    
		for (q = brightness; q < tRight; q++)
		{
			float d1, d2, d3;
			if (vSh[p] - vSh[tLeft] != 0 )
			{
				d1 = (float)(vSk[p]  - vSk[tLeft]) / (float)(vSh[p] - vSh[tLeft]) ;
				d1 -=  ( (float)(vMk[p]  - vMk[tLeft])  / (float)(vSh[p] - vSh[tLeft])  ) * ( (float)(vMk[p]  - vMk[tLeft]) / (float)(vSh[p] - vSh[tLeft]) );		
			}	
			else 
				d1 = 1e30f;

			if (vSh[q] - vSh[p] != 0 )
			{
				d2 = (float)(vSk[q]  - vSk[p]) / (float)(vSh[q] - vSh[p]);
				d2 -=  ( (float)(vMk[q]  - vMk[p])   / (float)(vSh[q] - vSh[p])) * ( (float)(vMk[q]  - vMk[p])/ (float)(vSh[q] - vSh[p]));
			}	
			else
				d2 = 1e30f;


			if (vSh[tRight] - vSh[q] != 0 )
			{
				d3 = (float)(vSk[tRight] - vSk[q]) / (float)(vSh[tRight] - vSh[q]);
				d3 -=  ( (float)(vMk[tRight]  - vMk[q]) / (float)(vSh[tRight] - vSh[q])) * ((float)(vMk[tRight]  - vMk[q])/ (float)(vSh[tRight] - vSh[q])); 
			}
			else
				d3 = 1e30f;


			float summ = d1*d1 + d2*d2 + d3*d3;

			if (summ < summBest) 
			{
				summBest = summ;
				pBest    = (long) p;
				qBest    = (long) q;
			}
		}

		t1 = (float) pBest;
		t2 = (float) qBest;
}
