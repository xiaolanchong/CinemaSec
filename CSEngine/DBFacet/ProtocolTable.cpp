//======================================================================================//
//                                      CinemaSec                                       //
//                                                                                      //
//                               Copyright© ElVEES, 2005                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
// Description:
//	Description is not available
//
//--------------------------------------------------------------------------------------//
// Author : Eugene Gorbachev (Eugene.Gorbachev@biones.com)
// Date   : 12.09.2005
//                                                                                      //
//======================================================================================//
#pragma	  warning( disable : 4995 )		//deprecated functions

#include "stdafx.h"
#include "ProtocolTable.h"

struct DBA_Protocol
{
	int					m_nCinemaID;
	int					m_nType;
	DBTIMESTAMP			m_Time;

	ISequentialStream*	m_BLOBDATA;
	ULONG				m_BLOBDATA_LENGTH;
	ULONG				m_BLOBDATA_STATUS;

	BEGIN_COLUMN_MAP(DBA_Protocol)
		COLUMN_ENTRY(1, m_nCinemaID)
		COLUMN_ENTRY(2, m_nType)
		COLUMN_ENTRY(3, m_Time)
		BLOB_ENTRY_LENGTH_STATUS(4, IID_ISequentialStream, STGM_READ, m_BLOBDATA, m_BLOBDATA_LENGTH, m_BLOBDATA_STATUS)
	END_COLUMN_MAP()
	DEFINE_COMMAND_EX(DBA_Protocol, _T("SELECT CinemaID, ProtoType, ProtoTime, ProtoString FROM Protocol"))
};

//======================================================================================//
//                                 class ProtocolTable                                  //
//======================================================================================//
ProtocolTable::ProtocolTable()
{
}

ProtocolTable::~ProtocolTable()
{
}

void	ProtocolTable::AddMessage( int nCinemaID, MsgType mt, CTime timeMsg, const std::wstring& sText )
{
	CCommand<CAccessor<DBA_Protocol> > A_Proto;
	HRESULT hr;

	CDBPropSet  propset2(DBPROPSET_ROWSET);
	propset2.AddProperty(DBPROP_IRowsetChange, true);
	propset2.AddProperty(DBPROP_UPDATABILITY, DBPROPVAL_UP_INSERT );
	hr = A_Proto.Open(m_dbSession, NULL, &propset2);

	if(FAILED( hr ) ) 
	{
		TraceError(hr);
		throw TableException("Protocol table connection error");
	}
		
	std::vector<unsigned char> Buf( (const BYTE*)sText.c_str(),
									(const BYTE*)sText.c_str() +  sText.length() * sizeof(wchar_t) );
	ISSHelper::MemStreamRead msr( Buf);

	A_Proto.m_nCinemaID			= nCinemaID;
	A_Proto.m_nType				= (int)mt;
	timeMsg.GetAsDBTIMESTAMP( A_Proto.m_Time );
	A_Proto.m_BLOBDATA_STATUS	= DBSTATUS_S_OK;
	A_Proto.m_BLOBDATA			= &msr;
	A_Proto.m_BLOBDATA_LENGTH	= ULONG( Buf.size() );
	hr = A_Proto.Insert( );

    if( FAILED(hr) ) 
	{
		AtlTraceErrorRecords(hr);
		A_Proto.FreeRecordMemory();
		throw TableException("Protocol insertion error");
	}
	A_Proto.Close();

}

void	ProtocolTable::GetMessage( int nCinemaID, CTime timeFrom, CTime timeTo, 
								  std::vector<DBProtocolTable::Message_t> & Msgs )
{
	Msgs.clear();
	CCommand<CAccessor<DBA_Protocol> > A_Proto;
	HRESULT hr;

	CString sFrom	= timeFrom.Format( _T("'%Y-%m-%d %H:%M:%S'") );
	CString sTo		= timeTo.Format( _T("'%Y-%m-%d %H:%M:%S'") );

	CString sQuery;
	sQuery.Format(  L"SELECT CinemaID, ProtoType, ProtoTime, ProtoString FROM Protocol "
					L"WHERE ProtoTime >= %s AND ProtoTime <= %s "
					L"ORDER BY ProtoTime ",
					(LPCWSTR)sFrom, (LPCWSTR)sTo);

	hr = A_Proto.Open(m_dbSession, (LPCWSTR)sQuery );

	if(FAILED( hr ) ) 
	{
		TraceError(hr);
		throw TableException("Protocol table connection error");
	}

	std::vector<BYTE> BufArr;

	UNREFERENCED_PARAMETER(nCinemaID);
	while ( A_Proto.MoveNext() == S_OK )
	{
		int		mt			= A_Proto.m_nType;
		CTime	timeMsg		= ToCTime( A_Proto.m_Time );
//		int		nCinemaID	= A_Proto.m_nCinemaID;

		bool res = ISSHelper::ReadBlobToArray( BufArr, A_Proto.m_BLOBDATA, A_Proto.m_BLOBDATA_LENGTH);
		if(res)
		{
			std::wstring sMsgText( (LPCWSTR) &BufArr[0], (LPCWSTR) &BufArr[0] + BufArr.size() / sizeof(WCHAR) );
			Msgs.push_back( Message_t( (DBProtocolTable::MsgType) mt, timeMsg, sMsgText ) );
		}
	}
}