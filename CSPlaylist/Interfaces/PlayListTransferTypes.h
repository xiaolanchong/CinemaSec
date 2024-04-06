#pragma once

#include "../../CSInterfaces/data_transfer_types.h"
#include "../../CSInterfaces/learning_algo_interface.h"
#include "../HistogramPreparer/CinemaHistogram.h"


// Array of chair's learning data.
namespace csinterface
{
	struct PlayListVideoFrame
	{
		LPBITMAPINFO            pBI;
		LPCUBYTE                bytes;
		int                     size;
	};

	struct HistoPair 
	{
		ChairContents           first;
		CinemaHistogram <float> second;

		HistoPair & operator = (const HistoPair & rh)
		{
			HistoPair locpair = rh;
			first = locpair.first;
			second= locpair.second;
			return (*this);
		}
	};

	typedef TDataWraper <PlayListVideoFrame, DATAID_FRAMEITERATOR_VIDEOFRAME > TFrameIteratorVideo;
	typedef TDataWraper <float, DATAID_FRAMEITERATOR_PROGRESS> TFrameIteratorProgress;
	typedef TDataWraper <CinemaHistogram<float>*, DATAID_HISTOGRAM_PSTATHIST> TPStaticHistogram; 
	typedef TDataWraper <ILearningAlgorithm*, DATAID_PLEARN_ALGORITHM> TPLearnAlgorithm; 
	typedef TDataWraper <int, DATAID_SKIPFRAMES_NUMBER> TSkipFramesNum; 
	typedef TDataWraper <PlayListParameters, DATAID_PLAYLIST_OPTIONS> TPlaylistParameters; 
}