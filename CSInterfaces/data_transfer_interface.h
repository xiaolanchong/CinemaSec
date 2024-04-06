/****************************************************************************
  data_transfer_interface.h
  ---------------------
  begin     : Oct 2004
  author(s) : A.Akhriev
  email     : aaah@mail.ru
****************************************************************************/

#pragma once

#include "common_interface_settings.h"

namespace csalgo
{

//=================================================================================================
/** \brief Data types that can be transferred between system's components. */
//=================================================================================================
enum DataTypeId
{
  DATAID_UNKNOWN = 0,              //!< unknown data type
  DATAID_CAMERA_STATISTICS,        //!< current statistics date obtained from camera analyzer
  DATAID_LOG_FILE_NAME,            //!< name of log file
  DATAID_DEMO_IMAGE,               //!< demo image
  DATAID_LAST_ERROR,               //!< text string that contains the last error message
  DATAID_FRAME_COUNTERS,           //!< frame index and the number of dropped frames
  DATAID_CHAIR_LEARN_ARRAY,        //!< state information of all chairs inside control area
  DATAID_BASE_CHAIR_SET,           //!< container of base chair structures
  DATAID_BASE_CHAIR_AND_CONTENT,   //!< base chair and its content type
  DATAID_QIMAGE,                   //!< demo image in 'QImage' format
  DATAID_CHAIR_CONTENTS,           //!< type of chair's contents
  DATAID_BACKGROUND,               //!< background image
  DATAID_PARAMETERS,               //!< binary image of parameters
  DATAID_EMPTYHALL_CHAIRSET,       //!< base chair array actually used to init camera analyzer
  DATAID_FRAMEITERATOR_VIDEOFRAME, //!< playlist player specific
  DATAID_FRAMEITERATOR_PROGRESS,   //!< playlist player specific
  DATAID_CHAIR_CONTENTS_MAP,       //!< playlist player specific
  DATAID_HISTOGRAM_PSTATHIST,      //!< playlist player specific
  DATAID_PLEARN_ALGORITHM,         //!< playlist player specific
  DATAID_SKIPFRAMES_NUMBER,        //!< playlist player specific
  DATAID_MSG_IAMREADY,             //!< algorithm shows that it is ready to give information (video, etc) sending this message
  DATAID_MSG_CLEAN_TEMP_DATA,      //!< playlist player commands to algorithm to clean temporary data file
  DATAID_MSG_LOAD_TEMP_DATA,       //!< playlist player commands to algorithm to load temporary data file
  DATAID_MSG_SAVE_TEMP_DATA,       //!< playlist player commands to algorithm to save temporary data file
  DATAID_MSG_FINALIZE,             //!< playlist player commands to algorithm to save the final data (ex. normalized histogram)
  DATAID_MSG_SET_DEFAULT_PARAMS,   //!< playlist player commands to algorithm to set its parameters to defaults
  DATAID_MSG_LOAD_PARAMETERS,      //!< playlist player commands to algorithm to load parameters from the specified file
  DATAID_MSG_SAVE_PARAMETERS,      //!< playlist player commands to algorithm to save parameters to the specified file
  DATAID_MSG_SHOW_PARAM_DIALOG,    //!< playlist player commands to algorithm to show parameters dialog
  DATAID_MSG_SEQUENCE_BEGIN,       //!< 
  DATAID_MSG_SEQUENCE_END,         //!< 
  DATAID_RESULT_FILENAME,          //!< string containing directory path for result output
  DATAID_PLAYLIST_OPTIONS,         //!< playlist player specific
  DATAID_STATE_FLAG,               //!< state flag of an object
  DATAID_CAMERA_NO,                //!< index of a camera
  DATAID_WORKING_SCENARIO,         //!< identifier of a working scenario
  DATAID_SURVEILLANCE_INTERVAL,    //!< assumed duration of cinema show or any surveillance interval in milliseconds
  DATAID_HALL_ANALYZER_TO_CAMERA,  //!< data transfered from hall analyzer to camera analyzer
  DATAID_DEMO_FLAG                 //!< any flag that tells what to show in demo mode
};


//=================================================================================================
/** \brief Interface of any data storage that should be transferred between system's components. */
//=================================================================================================
struct IDataType
{
  virtual DataTypeId type() const = 0;
};


//=================================================================================================
/** \struct TDataWraper.
    \brief  Structure wraps a data storage of any type into IDataType interface. */
//=================================================================================================
template< class DATA_TYPE, DataTypeId DATA_TYPE_ID >
struct TDataWraper : public IDataType
{
  typedef  DATA_TYPE  data_type;

  data_type data;  //!< data storage

  TDataWraper() : data() {}

  explicit TDataWraper( const data_type & source ) : data( source ) {}

  virtual DataTypeId type() const { return DATA_TYPE_ID; }
};


#define  MY_DATA_CAST( TDATA ) \
  csinterface::##TDATA * pInfo = dynamic_cast<csinterface::##TDATA*>( pData ); \
  ALIB_ASSERT( pInfo != 0 );

#define  MY_DATA_CONST_CAST( TDATA ) \
  const csinterface::##TDATA * pInfo = dynamic_cast<const csinterface::##TDATA*>( pData ); \
  ALIB_ASSERT( pInfo != 0 );


} // namespace csinterface

