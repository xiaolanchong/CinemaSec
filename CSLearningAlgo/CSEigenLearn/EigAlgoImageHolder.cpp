#include "StdAfx.h"
#include ".\eigalgoimageholder.h"
#include "../../TNT/tnt_array1d.h"
#include "../../TNT/tnt_array2d.h"
#include "../../TNT/jama_eig.h"
#include "../../CSUtility/utility/multi_histogram.h"
#include "SpectrumApproximator.h"


EigAlgoImageHolder::EigAlgoImageHolder(int imgSide /* = 64 */)
	: myHisto(0.0f, 256.0f, 256)
	, histoCnt(0)
{
	this->imgSide = imgSide;
	corrMatrix.resize(imgSide * imgSide, imgSide * imgSide);
	avImage.resize( imgSide,  imgSide, 0.0f);
	Clear();
}

EigAlgoImageHolder::~EigAlgoImageHolder(void)
{

}

void EigAlgoImageHolder::AddImage(const Arr2f& image)
{
	for (int i = 0; i < imgSide * imgSide; i++)
	{
		for (int j = 0; j < imgSide * imgSide; j++)
        {
			corrMatrix(i,j) += (double)(image[i] * image[j]);
		}
		avImage[i] += image[i];
	}
	cnt++;
}

void EigAlgoImageHolder::Clear()
{
	std::fill( corrMatrix.begin(), corrMatrix.end(), 0.0 );
	std::fill( avImage.begin(), avImage.end(), 0.0 );

	accum.reset();

	momentsList.clear();

	myHisto.FillZero();
	histoCnt = 0;

	cnt = 0;
}

void EigAlgoImageHolder::GetEigenValues(std::vector<float>& eigValues) const
{
	int side = imgSide * imgSide;
	
	Arr2f localAvImage = avImage;
	Arr2d localCorrMatrix = corrMatrix;


	alib::Multiply( localCorrMatrix.begin(), localCorrMatrix.end(), (float)alib::Reciprocal( (double)cnt ) );
	alib::Multiply( localAvImage.begin(), localAvImage.end(), (float)alib::Reciprocal( (double)cnt ) );

	{
		for (int i = 0; i < imgSide * imgSide; i++)
		{
			for (int j = 0; j < imgSide * imgSide; j++)
			{
				localCorrMatrix(i,j) -=  (double)(localAvImage[i] * localAvImage[j]);
			}
		}	
	}

	TNT::Array2D < double > tntMatrix (side, side);
	for (int i = 0; i < side; i++ ) 
	{
		for (int j = 0; j < side; j++ ) 
		{
			tntMatrix[i][j] = localCorrMatrix(i, j); // / std::max <double> (1.0, (double) cnt);
		}
	}

	double dbg = 0;
	for (int i = 0; i < side; i++ ) 
	{
		dbg += localCorrMatrix(i, i);
	}
	
	dbg = sqrt(dbg / (double)side);
    
	
	JAMA::Eigenvalue <double> ev(tntMatrix);
	TNT::Array1D < double > tntEigvals;
	ev.getRealEigenvalues(tntEigvals);

	eigValues.clear();
	for (i = side - 1; i >= 0; i-- ) 
	{
		eigValues.push_back((float)tntEigvals[i]);
	}
}
Arr2f EigAlgoImageHolder::GetFirstImage() const
{
	return firstImage;
}

void EigAlgoImageHolder::SetFirstImage(Arr2f image)
{
	firstImage = image;
}

void EigAlgoImageHolder::UpdateHisto()
{
	std::vector <float> ev;
	GetEigenValues(ev);
    SpectrumApproximator < float > approximator(ev);
	float m1, m2;
	approximator.GetMoments(m1, m2);

	momentsList.push_back( m1 );

		

	myHisto.AddValue(HistoTransform(m1), 0);
	m2;
	
	accum += m1;

	//myHisto.AddValue(HistoTransform(m2), 1);
	histoCnt++;
}

float EigAlgoImageHolder::HistoTransform(float x)
{
	float eps = 1e-8f;
	return 255.0f * ( 5.0f / 4.0f + 1.0f / 4.0f * std::log10( x + eps ));   
	//return x * 10.0f;   
}
void EigAlgoImageHolder::GetHisto(csutility::MultiHistogram <float, 2> & histo, int & histoCounter) const
{
	histo = myHisto;
	histoCounter = histoCnt;
}

void EigAlgoImageHolder::SpliceMomentsListToThisList( std::list < float > & lst)
{
	lst.splice(lst.end(), momentsList);
}


void EigAlgoImageHolder::GetAverages(MeanVarAccum < float >& accum)
{
	accum = this->accum;
	this->accum.reset();
}