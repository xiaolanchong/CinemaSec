#include "stdafx.h"
#include "ProcessFiles.h"

ProcessFiles::ProcessFiles()
{

}

ProcessFiles::~ProcessFiles()
{

}

bool ProcessFiles::Start( std::wstring outputFilename )
{
	pWriter = std::auto_ptr < avideolib::IVideoWriter > ( avideolib::CreateAVideoWriter( outputFilename.c_str(), 0, false, true));
	return true;
}
bool ProcessFiles::ProcessFile( std::wstring videoFilename)
{
	std::auto_ptr < avideolib::IVideoReader > pReader = std::auto_ptr < avideolib::IVideoReader > ( avideolib::CreateAVideoReader( videoFilename.c_str(), 0, true ) );
	ASSERT( pReader.get() != 0 );

	std::vector < Arr2ub > samples;


	Arr2ub tmp;
	samples.reserve( 20000 );
	while( pReader->ReadFrame( &tmp, -1, 0, 0 ) != avideolib::IVideoReader::END_IS_REACHED )
	{
		samples.push_back( tmp );
	}
/*
	if ( samples.begin() != samples.end() )
	{
		typedef SampleDistance < Arr2ub, Arr2ub, double, true > Distance;
		Distance sqDist;
		Arr2ub first = samples[0];
		pWriter->WriteFrame( &first, 0 );
		for (int i = 0; i < (int)samples.size(); i++)
		{
			Arr2ub crnt = samples[i];
			double dist = sqDist( first, crnt);

			if ( dist > 2500000.000000 )
			{
				first = crnt;
				pWriter->WriteFrame( &crnt, 0 );
			}
		}
	}
*/
	if ( samples.begin() != samples.end() )
	{
		typedef SqSampleDistance < Arr2ub, DoubleArr, true > SqDistance;
        typedef KHarmonicMeanClusterizer < Arr2ub, SqDistance > Clusterizer;

		Clusterizer clusterizer;
		clusterizer.Run( &(*(samples.begin())), &(*(samples.end())), samples[0].size(), 5, 50 );
		
		const Clusterizer::CenterArr & answer = clusterizer.GetMeanVectors();
		Arr2ub wrap;

		for (int i = 0; i < (int)answer.size(); i++)
		{
			wrap.resize( tmp.width(), tmp.height() );
			for (int x = 0; x < tmp.width(); x++)
				for (int y = 0; y < tmp.height(); y++)
				{
					wrap( x, y ) = (answer[i])[ x + y * tmp.width() ];
				}				
            
    		pWriter->WriteFrame( &(wrap), 0 );
		}
	}

     
	return true;
}

float ProcessFiles::CalculateDiff( Arr2ub& pic1, Arr2ub& pic2)
{
	ASSERT( pic1.width() == pic2.width() && pic1.height() == pic2.height() );

	int w = pic1.width();
	int h = pic1.height();
	float diff = 0.0f;

	for ( int i = 0; i < w; i++ )
	{
		for ( int j = 0; j < h; j++ )
		{
			diff += ( pic1( i, j ) - pic2( i, j ) ) * ( pic1( i, j ) - pic2( i, j ) );
		}
	}
	diff = std::sqrt( diff / (float)(w * h) );
	return diff;
}
