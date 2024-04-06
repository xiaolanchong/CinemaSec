MessageIdTypedef = HRESULT

FacilityNames	= (
					ChairUtility	= 0xA0 : FACILITY_CHAIR_UTILITY
					Grabber			= 0xA1 : FACILITY_GRABBER
					)

LanguageNames=(EN=0x409:ChairErrorDesc_en)
LanguageNames=(RU=0x419:ChairErrorDesc_ru)

;///////////////////// Chair utility error codes //////////////////////////////////////////

MessageId	= 0x1
Severity	= Error
Facility	= ChairUtility
SymbolicName= ERR_CHAIR_FILE_NOT_FOUND
Language	= EN
Chair file with chair curves not found%0
.
Language	= RU
%0
.

MessageId	= 0x2
Severity	= Error
Facility	= ChairUtility
SymbolicName= ERR_VIDEO_FILE_NOT_FOUND
Language	= EN
Video file not found%0
.
Language	= RU
%0
.

MessageId	= 0x3
Severity	= Error
Facility	= ChairUtility
SymbolicName= ERR_BACKGROUND_FILE_NOT_FOUND
Language	= EN
Background image file not found%0
.
Language	= RU
%0
.

;///////////////////// Grabber error codes //////////////////////////////////////////

MessageId	= 0x1
Severity	= Error
Facility	= Grabber
SymbolicName= ERR_VIDEOLIB_NOT_FOUND
Language	= EN
chsva.dll not found%0
.
Language	= RU
%0
.

MessageId	= 0x2
Severity	= Error
Facility	= Grabber
SymbolicName= ERR_VIDEOLIB_INIT_FAILED
Language	= EN
Camera intialization failed%0
.
Language	= RU
%0
.

MessageId	= 0x3
Severity	= Error
Facility	= Grabber
SymbolicName= ERR_NOAVAILABLE_STREAMS
Language	= EN
No available streams%0
.
Language	= RU
%0
.
