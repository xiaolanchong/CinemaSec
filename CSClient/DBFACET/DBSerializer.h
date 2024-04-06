//========================= Copyright © 2004, Elvees ==========================
// Author: Eugene V. Gorbachev
// Purpose: load & save data (image & gauge) from db
//
//=============================================================================

#ifndef DBSERIALIZER_H
#define DBSERIALIZER_H
#ifdef _WIN32
#pragma once
#endif

#include "../common/Exception.h"
#include "../common/tstring.h"
#include "../../CSEngine/DBFacet/CinemaDB.h"
#include "..\CSUtility\Alib\alib_arr2d.h"

MACRO_EXCEPTION(DBSerializerException, CommonException)
MACRO_EXCEPTION(DBLoadImageException, DBSerializerException)
MACRO_EXCEPTION(DBSaveImageException, DBSerializerException)
MACRO_EXCEPTION(DBLoadGaugeException, DBSerializerException)
MACRO_EXCEPTION(DBSaveGaugeException, DBSerializerException)

class DBSerializer
{
public:
	
	void LoadBackgroundImage( int nCameraID, Arr2f& Background);
	void SaveBackgroundImage( int nCameraID, const Arr2f& Background );

	void LoadChairs( int nRoomID, std::vector<BYTE>& Data);
	void SaveChairs( int nRoomID,const std::vector<BYTE>& Data );

	DBSerializer(void);
	~DBSerializer(void);
};

//class CCinemaDB;

CCinemaDB&	GetDB();

#endif // DBSERIALIZER_H
