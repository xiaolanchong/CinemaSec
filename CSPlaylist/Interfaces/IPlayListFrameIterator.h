#pragma once
// provides all information about one frame
// such as 
// video frame itself
// background ( for all frames it is the same )
// array of chairs with correspondent info

// working scenario
//////////////////////////////////////////////////////////////////////////
// Initialize()
// while (!IsEnd())
// {    
//     GetFrameInfo(...);
//     SeekNext();
//     if (some condition) Restart();
// }
interface IPlayListFrameIterator
{	// initialize
	virtual HRESULT Initialize(const std::wstring& sVideoFileName, const std::wstring& sMarkingFile, const BaseChairArr& emptyHallArr) = 0;
	// use this method if you want to restart the iterator
	virtual HRESULT Restart() = 0;
	// get to the next frame
	virtual HRESULT SeekNext() = 0;
	// check if it is the end
	virtual bool    IsEnd() = 0;
	//gets the info about chairs
	virtual inline HRESULT GetFrameInfo(csinterface::IDataType * pData) = 0;
	// 
	virtual HRESULT Release() = 0;
	virtual ~IPlayListFrameIterator(){}
};