// DBAccessors.h
//
//////////////////////////////////////////////////////////////////////

#pragma once

/////////////////////////////////////////////////////////////////////////////
// Accessors

class CAccCameras
{
public:
	LONG  m_lRoomID;			// Room ID

	LONG  m_lCameraID;
	TCHAR m_stCameraUID[92];	// "{UUID}"
	TCHAR m_stCameraIP[56];		// xxx.xxx.xxx.xxx - dotted format

	BEGIN_COLUMN_MAP(CAccCameras)
		COLUMN_ENTRY(1, m_lCameraID)	
		COLUMN_ENTRY(2, m_stCameraIP)
		COLUMN_ENTRY(3, m_stCameraUID)
	END_COLUMN_MAP()

	// Parameter binding map
	BEGIN_PARAM_MAP(CAccCameras)
		SET_PARAM_TYPE(DBPARAMIO_INPUT)
		COLUMN_ENTRY(1, m_lRoomID)
	END_PARAM_MAP()

	DEFINE_COMMAND_EX(CAccCameras,
	L"SELECT CameraID, CameraIP, CameraUID FROM Cameras WHERE (RoomID=?);")

	// You may wish to call this function if you are inserting a record and wish to
	// initialize all the fields, if you are not going to explicitly set all of them.
	void ClearRecord()
	{
		memset(this, 0, sizeof(*this));
	}
};

class CAccAllCameras
{
public:
	LONG  m_lRoomID;			// Room ID
	LONG  m_lCameraID;
	TCHAR m_stCameraUID[92];	// "{UUID}"
	TCHAR m_stCameraIP[56];		// xxx.xxx.xxx.xxx - dotted format

	BEGIN_COLUMN_MAP(CAccAllCameras)
		COLUMN_ENTRY(1, m_lCameraID)	
		COLUMN_ENTRY(2, m_stCameraIP)
		COLUMN_ENTRY(3, m_stCameraUID)
		COLUMN_ENTRY(4, m_lRoomID)
	END_COLUMN_MAP()

	DEFINE_COMMAND_EX(CAccAllCameras,
	L"SELECT CameraID, CameraIP, CameraUID, RoomID FROM Cameras;")

	// You may wish to call this function if you are inserting a record and wish to
	// initialize all the fields, if you are not going to explicitly set all of them.
	void ClearRecord()
	{
		memset(this, 0, sizeof(*this));
	}
};

class CAccCameraID
{
public:
	LONG  m_lCameraID;
	TCHAR m_stCameraUID[92];	// "{UUID}"
	TCHAR m_stCameraIP[56];		// xxx.xxx.xxx.xxx - dotted format

	BEGIN_COLUMN_MAP(CAccCameraID)
		COLUMN_ENTRY(1, m_stCameraIP)
		COLUMN_ENTRY(2, m_stCameraUID)
	END_COLUMN_MAP()

	// Parameter binding map
	BEGIN_PARAM_MAP(CAccCameraID)
		SET_PARAM_TYPE(DBPARAMIO_INPUT)
		COLUMN_ENTRY(1, m_lCameraID)
	END_PARAM_MAP()

	DEFINE_COMMAND_EX(CAccCameraID,
	L"SELECT CameraIP, CameraUID FROM Cameras WHERE (CameraID=?);")

	// You may wish to call this function if you are inserting a record and wish to
	// initialize all the fields, if you are not going to explicitly set all of them.
	void ClearRecord()
	{
		memset(this, 0, sizeof(*this));
	}
};