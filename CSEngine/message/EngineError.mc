MessageIdTypedef = HRESULT

FacilityNames	= (
					MainServer		= 0xA2 : FACILITY_MAIN_SERVER 
					Diagnostic		= 0xA3 : FACILITY_DIAGNOSTIC
					SystemConfig	= 0xA4 : FACILITY_SYSTEM_CONFIG
					)

LanguageNames=(EN=0x409:ErrorDesc_en)
LanguageNames=(RU=0x419:ErrorDesc_ru)

;///////////////////// Diagnostic error codes ///////////////////////////////////////

MessageId	= 0x1
Severity	= Error
Facility	= Diagnostic
SymbolicName= ERR_NETWORK_FAILED
Language	= EN
Network connection failed%0
.
Language	= RU
%0
.

;///////////////////// Main server error codes ///////////////////////////////////////

;///////////////////// System configuration error codes //////////////////////////////

MessageId	= 0x1
Severity	= Error
Facility	= SystemConfig
SymbolicName= ERR_ROOMID_NOT_SET
Language	= EN
RoomID is not set%0
.
Language	= RU
%0
.

MessageId	= 0x2
Severity	= Error
Facility	= SystemConfig
SymbolicName= ERR_DBCONNECTIONSTRING_NOT_SET
Language	= EN
DBConnectionString is not set%0
.
Language	= RU
%0
.

MessageId	= 0x3
Severity	= Error
Facility	= SystemConfig
SymbolicName= ERR_DB_CONNECTION_FAILED
Language	= EN
Failed to connect to database%0
.
Language	= RU
%0
.

MessageId	= 0x4
Severity	= Error
Facility	= SystemConfig
SymbolicName= ERR_COMMON_EXCEPTION
Language	= EN
Common exception was thrown%0
.
Language	= RU
%0
.

MessageId	= 0x5
Severity	= Error
Facility	= SystemConfig
SymbolicName= ERR_SYSTEM_CONFIGURATION_INVALID
Language	= EN
System configuration invalid or was not read properly%0
.
Language	= RU
%0
.

MessageId	= 0x6
Severity	= Error
Facility	= SystemConfig
SymbolicName= ERR_SYSTEM_CONFIGURATION
Language	= EN
Error occuried while configuration was read%0
.
Language	= RU
%0
.

MessageId	= 0x7
Severity	= Error
Facility	= SystemConfig
SymbolicName= ERR_CHAIR_ERROR
Language	= EN
Chair loading error%0
.
Language	= RU
%0
.

MessageId	= 0x8
Severity	= Error
Facility	= SystemConfig
SymbolicName= ERR_BACKGROUND_ERROR
Language	= EN
Background loading error%0
.
Language	= RU
%0
.

