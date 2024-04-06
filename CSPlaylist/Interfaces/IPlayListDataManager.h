//////////////////////////////////////////////////////////////////////////
//date  : 4 March 2005
//author: Alexander Boltnev
//e-mail: Alexander.Boltnev@biones.com
//////////////////////////////////////////////////////////////////////////
#include "PlaylistParser\PlaylistItem.h"


#include "..\..\CSInterfaces\data_transfer_interface.h"


//working scenario 
//////////////////////////////////////////////////////////////////////////
// LoadOneItem(item);
// SetData(DATA_MANAGER_PROPERTIES)
// If(IsReady()) 
// {
//     SetTimer(); 
//     Start();
// }
// OnTimer()
// {
//     GetData(DATA_MANAGER_PROGRESS)
//     GetData(DATA_MANAGER_INFO_ABOUT_PROCESS) //video and such
// }
// OnEvent()
// {
//   Stop()
// }

interface IPlayListDataManager
{
    // three functions for processing one play list item; word "precess" here means 
    // save some learn data or test system for quality and so forth.
	virtual HRESULT LoadOneItem(const PlaylistItem& item) = 0;
	virtual HRESULT Start() = 0;
	virtual HRESULT Stop(bool saveResult = true)  = 0; 
	// releases DataManager
	virtual HRESULT Release() = 0;
	// check is DataManager is ready to prepare information
	virtual bool    IsReady() = 0;	
	// Properties of DataManager
	virtual HRESULT SetData( const csinterface::IDataType * pData ) = 0;
	virtual HRESULT GetData( csinterface::IDataType * pData ) const = 0;

//	virtual HRESULT SetData(csalgo::IDataType * pData) = 0;
//	virtual HRESULT GetData(csalgo::IDataType * pData) = 0;
	virtual UINT    id() = 0;  
	virtual ~IPlayListDataManager(){};
};