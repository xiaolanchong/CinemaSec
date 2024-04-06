///////////////////// Diagnostic error codes ///////////////////////////////////////
//
//  Values are 32 bit values laid out as follows:
//
//   3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1
//   1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
//  +-+-+-+-+-+---------------------+-------------------------------+
//  |S|R|C|N|r|    Facility         |               Code            |
//  +-+-+-+-+-+---------------------+-------------------------------+
//
//  where
//
//      S - Severity - indicates success/fail
//
//          0 - Success
//          1 - Fail (COERROR)
//
//      R - reserved portion of the facility code, corresponds to NT's
//              second severity bit.
//
//      C - reserved portion of the facility code, corresponds to NT's
//              C field.
//
//      N - reserved portion of the facility code. Used to indicate a
//              mapped NT status value.
//
//      r - reserved portion of the facility code. Reserved for internal
//              use. Used to indicate HRESULT values that are not status
//              values, but are instead message ids for display strings.
//
//      Facility - is the facility code
//
//      Code - is the facility's status code
//
//
// Define the facility codes
//
#define FACILITY_MAIN_SERVER             0xA2
#define FACILITY_DIAGNOSTIC              0xA3
#define FACILITY_SYSTEM_CONFIG           0xA4


//
// Define the severity codes
//


//
// MessageId: ERR_NETWORK_FAILED
//
// MessageText:
//
// Network connection failed%0
//
#define ERR_NETWORK_FAILED               ((HRESULT)0xC0A30001L)

///////////////////// Main server error codes ///////////////////////////////////////
///////////////////// System configuration error codes //////////////////////////////
//
// MessageId: ERR_ROOMID_NOT_SET
//
// MessageText:
//
// RoomID is not set%0
//
#define ERR_ROOMID_NOT_SET               ((HRESULT)0xC0A40001L)

//
// MessageId: ERR_DBCONNECTIONSTRING_NOT_SET
//
// MessageText:
//
// DBConnectionString is not set%0
//
#define ERR_DBCONNECTIONSTRING_NOT_SET   ((HRESULT)0xC0A40002L)

//
// MessageId: ERR_DB_CONNECTION_FAILED
//
// MessageText:
//
// Failed to connect to database%0
//
#define ERR_DB_CONNECTION_FAILED         ((HRESULT)0xC0A40003L)

//
// MessageId: ERR_COMMON_EXCEPTION
//
// MessageText:
//
// Common exception was thrown%0
//
#define ERR_COMMON_EXCEPTION             ((HRESULT)0xC0A40004L)

//
// MessageId: ERR_SYSTEM_CONFIGURATION_INVALID
//
// MessageText:
//
// System configuration invalid or was not read properly%0
//
#define ERR_SYSTEM_CONFIGURATION_INVALID ((HRESULT)0xC0A40005L)

//
// MessageId: ERR_SYSTEM_CONFIGURATION
//
// MessageText:
//
// Error occuried while configuration was read%0
//
#define ERR_SYSTEM_CONFIGURATION         ((HRESULT)0xC0A40006L)

//
// MessageId: ERR_CHAIR_ERROR
//
// MessageText:
//
// Chair loading error%0
//
#define ERR_CHAIR_ERROR                  ((HRESULT)0xC0A40007L)

//
// MessageId: ERR_BACKGROUND_ERROR
//
// MessageText:
//
// Background loading error%0
//
#define ERR_BACKGROUND_ERROR             ((HRESULT)0xC0A40008L)

