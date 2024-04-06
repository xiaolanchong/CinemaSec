//////////////////////////////////////////////////////////////////////////
//date  : 4 March 2005
//author: Alexander Boltnev
//e-mail: Alexander.Boltnev@biones.com
//////////////////////////////////////////////////////////////////////////
#pragma once

struct PlaylistItem // all information about video for start saving learn data
{
	std::wstring video; // video file name (*.avi)
	std::wstring chairs; // empty hall chairs marking file (*.xml) 
	std::wstring marking; // marking file name (*.xml)      
	std::wstring background; // background image file name (*.bmp)
	std::wstring cameraUID; // some string identifying camera
	std::wstring path; // video file path

	// copy constructor
	PlaylistItem& operator = (const PlaylistItem& rh)
	{	
		this->video = rh.video;
		this->chairs = rh.chairs;
		this->marking = rh.marking;
		this->background = rh.background; 
		this->cameraUID = rh.cameraUID;
		this->path = rh.path;
		return (*this);
	}
};
