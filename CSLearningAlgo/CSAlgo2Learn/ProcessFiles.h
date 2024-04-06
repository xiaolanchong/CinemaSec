#pragma once

class ProcessFiles {
public:
	ProcessFiles();
	~ProcessFiles();

	bool Start( std::wstring outputFilename );
	bool ProcessFile( std::wstring videoFilename);
private:
	float CalculateDiff( Arr2ub& pic1, Arr2ub& pic2);

	std::auto_ptr < avideolib::IVideoWriter > pWriter;

};