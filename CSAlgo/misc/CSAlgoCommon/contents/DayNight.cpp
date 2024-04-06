#include "stdafx.h"
#include "DayNight.h"


/// constructor
DayNight::DayNight( float iThreshold /* = 90.0f */, float pThreshold  /*= 70.0f */ )
	: dayInit( false )
	, nightInit( false )
{
	iThr = iThreshold;
	pThr = pThreshold;

	videoHisto.Initialize( 0, 255, 256 );
}
/// destructor
DayNight::~DayNight()
{
}

/// returns true for day and false for night
bool DayNight::Determine( const Arr2f & picture ) const
{
	Arr2f::const_iterator iter = picture.begin();

	long cnt = 0;
	while ( iter != picture.end() ) {
		if ( *iter > iThr )
		{
			cnt++;
		}
		iter++;
	}

	long totalNum = picture.width() * picture.height();

	if ( (float)cnt > (float)totalNum * pThr / 100.0f ) 
		return true;
	else
		return false;
}

bool DayNight::Learn( const Arr2f & dayPicture, const Arr2f & nightPicture, float & iThreshold, float & pThreshold )
{

	csutility::MultiHistogram < float, 2 > histo;
	histo.Initialize( 0, 255, 256 );

	// day histo fill
	Arr2f::const_iterator iter = dayPicture.begin();
	while ( iter != dayPicture.end() )
	{
		histo.AddValue( *iter, 0 );
		iter++;
	}

	//night histo fill
	iter = nightPicture.begin();
	while ( iter != nightPicture.end() )
	{
		histo.AddValue( *iter, 1 );
		iter++;
	}

	GetParametersFromHisto( histo, iThreshold, pThreshold );
	return true;
}

bool DayNight::Learn( Arr2ub & frame, bool bDay )
{
	Arr2ub::iterator iter = frame.begin();

	while ( iter != frame.end() )
	{
		if ( bDay )
		{
			videoHisto.AddValue( (*iter), 0 );
			dayInit = true;
		}
		else
		{
			videoHisto.AddValue( (*iter), 1 );
			nightInit = true;
		}
		iter++;		
	}

	return true;
}


bool DayNight::Learn( std::wstring videoFile, bool bDay )
{
	std::auto_ptr < avideolib::IVideoReader > pReader = std::auto_ptr < avideolib::IVideoReader > ( avideolib::CreateAVideoReader( videoFile.c_str(), 0, true ) );

	if ( pReader.get() == 0 ) return false;

	Arr2ub tmp;
	while( pReader->ReadFrame(&tmp, -1, 0, 0 ) != avideolib::IVideoReader::END_IS_REACHED )
	{
		Arr2ub::iterator iter = tmp.begin();

		while ( iter != tmp.end() )
		{
			if ( bDay )
			{
				videoHisto.AddValue( (*iter), 0 );
				dayInit = true;
			}
			else
			{
				videoHisto.AddValue( (*iter), 1 );
				nightInit = true;
			}
			iter++;
		}
	}

	return true;
}



/// function returns false if not enough video fragmnts were presented
bool DayNight::GetParameters( float & iThreshold, float & pThreshold )
{
	if ( dayInit != true || nightInit != true )
		return false;

	GetParametersFromHisto( videoHisto, iThreshold, pThreshold );
	return true;
}

void DayNight::GetParametersFromHisto( csutility::MultiHistogram < float, 2 >& histo, float & iThreshold, float & pThreshold )
{

	csutility::MultiHistogram < float, 2 > tmphisto;
	tmphisto.Initialize( 0, 255, 256 );

	tmphisto = histo;

	tmphisto.Normilize();

	std::vector < float > dayLayer;
	std::vector < float > nightLayer;

	tmphisto.GetLayer(0, dayLayer);
	tmphisto.GetLayer(1, nightLayer);

	for (int i = (int)(dayLayer.size()) - 2; i >= 0; i--)
	{
		dayLayer[i] += dayLayer[i + 1];
		nightLayer[i] += nightLayer[i + 1];
	}

	double maxDiff = -1.0;
	int pos = 0;
	for(int i = 0; i < (int)(dayLayer.size()); i++)
	{
		double diff = dayLayer[i] - nightLayer[i];
		if ( diff > maxDiff )
		{
			maxDiff = diff;
			pos = i;
		}
	}

	iThreshold = (float)pos;
	pThreshold = 50.0f * ( dayLayer[pos] +  nightLayer[pos] );
}
