///////////////////// Chair utility error codes //////////////////////////////////////////
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
#define FACILITY_CHAIR_UTILITY           0xA0
#define FACILITY_GRABBER                 0xA1


//
// Define the severity codes
//


//
// MessageId: ERR_CHAIR_FILE_NOT_FOUND
//
// MessageText:
//
// Chair file with chair curves not found%0
//
#define ERR_CHAIR_FILE_NOT_FOUND         ((HRESULT)0xC0A00001L)

//
// MessageId: ERR_VIDEO_FILE_NOT_FOUND
//
// MessageText:
//
// Video file not found%0
//
#define ERR_VIDEO_FILE_NOT_FOUND         ((HRESULT)0xC0A00002L)

//
// MessageId: ERR_BACKGROUND_FILE_NOT_FOUND
//
// MessageText:
//
// Background image file not found%0
//
#define ERR_BACKGROUND_FILE_NOT_FOUND    ((HRESULT)0xC0A00003L)

///////////////////// Grabber error codes //////////////////////////////////////////
//
// MessageId: ERR_VIDEOLIB_NOT_FOUND
//
// MessageText:
//
// chsva.dll not found%0
//
#define ERR_VIDEOLIB_NOT_FOUND           ((HRESULT)0xC0A10001L)

//
// MessageId: ERR_VIDEOLIB_INIT_FAILED
//
// MessageText:
//
// Camera intialization failed%0
//
#define ERR_VIDEOLIB_INIT_FAILED         ((HRESULT)0xC0A10002L)

//
// MessageId: ERR_NOAVAILABLE_STREAMS
//
// MessageText:
//
// No available streams%0
//
#define ERR_NOAVAILABLE_STREAMS          ((HRESULT)0xC0A10003L)

