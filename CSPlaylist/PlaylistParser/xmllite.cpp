// xmllite.cpp: implementation of the xmllite class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "xmllite.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace XmlLite  
{
	
std::wstring GetErrorDesc(REFIID riid, IUnknown* pUnknown)
{
//	CComPtr<IUnknown> pUnk;
	ISupportErrorInfo* pSup;
	pUnknown->QueryInterface( IID_ISupportErrorInfo, (void**)&pSup );
	if(pSup && SUCCEEDED(pSup->InterfaceSupportsErrorInfo( riid )))
	{
		pSup->Release();
		CComPtr<IErrorInfo> pError; 
		GetErrorInfo( 0, &pError );
		CComBSTR str;
		pError->GetDescription(&str);
		return std::wstring(str);
	}
	return std::wstring();
}




//************ XMLDocument **************//

XMLDocument::XMLDocument()
{	
	m_pDoc.CoCreateInstance(__uuidof(DOMDocument));	
}

void	XMLDocument::SetRoot(const XMLElement& ElRoot)
{
	HRESULT hr = m_pDoc->putref_documentElement( ElRoot.m_iXMLElem );
	HrCheck<XMLDocumentException>( hr, "Failed to set the root");
}

struct MemStreamWrite :  ISequentialStream
{
	std::vector<unsigned char>& m_Buffer;
	MemStreamWrite(std::vector<unsigned char>& Arr) : m_Buffer(Arr)
	{
		m_Buffer.clear();
		m_Buffer.reserve(1024 * 50); // 50k for text file for start
	}

	virtual HRESULT STDMETHODCALLTYPE QueryInterface( 
		/* [in] */ REFIID riid,
		/* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject) 
	{
		if( IID_ISequentialStream == riid )
		{
			*ppvObject = static_cast<ISequentialStream*>(this);
			return S_OK;
		}
		else if( IID_IUnknown == riid )
		{
			*ppvObject = static_cast<IUnknown*>(this);
			return S_OK;
		}
		return E_NOTIMPL;
	}
	virtual ULONG STDMETHODCALLTYPE AddRef( void) {return 1;}
	virtual ULONG STDMETHODCALLTYPE Release( void) { return 1;}
	virtual HRESULT STDMETHODCALLTYPE Read( void *pv, ULONG cb, ULONG *pcbRead) 
	{ 
		UNREFERENCED_PARAMETER(pv);
		UNREFERENCED_PARAMETER(cb);
		UNREFERENCED_PARAMETER(pcbRead);
		return E_NOTIMPL;
	}
	virtual HRESULT STDMETHODCALLTYPE Write( const void *pv, ULONG cb, ULONG *pcbWritten)
	{
		m_Buffer.insert( m_Buffer.end(), (const unsigned char*)pv, (const unsigned char*)pv + cb );
		if( pcbWritten ) { *pcbWritten = cb;}
		return S_OK;
	}	
} ;

/*void	XMLDocument::SaveToString( std::wstring& sData ) const
{
	
}*/

void	XMLDocument::SaveToBinary(std::vector<unsigned char>& Arr) const
{
	MemStreamWrite WStream(Arr);
	CComVariant var(&WStream);
	HRESULT hr = m_pDoc->save(var);
	HrCheck<XMLDocumentException>( hr, "Failed to save the document");
}

struct MemStreamRead :  ISequentialStream
{
	const void* m_pData;
	size_t		m_nSize;
	size_t		m_Current;
	MemStreamRead(const void* pData, size_t nSize) : 
	m_pData(pData),
		m_nSize( nSize ),
		m_Current ( 0 )
	{
	}

	virtual HRESULT STDMETHODCALLTYPE QueryInterface( 
		/* [in] */ REFIID riid,
		/* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject) 
	{
		if( IID_ISequentialStream == riid )
		{
			*ppvObject = static_cast<ISequentialStream*>(this);
			return S_OK;
		}
		else if( IID_IUnknown == riid )
		{
			*ppvObject = static_cast<IUnknown*>(this);
			return S_OK;
		}
		return E_NOTIMPL;
	}
	virtual ULONG STDMETHODCALLTYPE AddRef( void) {return 1;}
	virtual ULONG STDMETHODCALLTYPE Release( void) { return 1;}
	virtual HRESULT STDMETHODCALLTYPE Read( void *pv, ULONG cb, ULONG *pcbRead) 
	{ 
		ULONG Remain = ULONG(m_nSize - m_Current);
		if(cb > Remain) cb = Remain;
		memcpy( pv, (const BYTE*)m_pData + m_Current, cb );
		m_Current += cb;
		if( pcbRead ) { *pcbRead = cb;}
		return S_OK;
	}
	virtual HRESULT STDMETHODCALLTYPE Write( const void *pv, ULONG cb, ULONG *pcbWritten)
	{
		UNREFERENCED_PARAMETER(pv);
		UNREFERENCED_PARAMETER(cb);
		UNREFERENCED_PARAMETER(pcbWritten);
		return E_NOTIMPL;
	}	
} ;

void	XMLDocument::LoadFromBinary(const void* pData, size_t nSize)
{
	MemStreamRead RStream( pData,  nSize);
	CComVariant var(&RStream);
	VARIANT_BOOL	bSuccess;
	HRESULT hr = m_pDoc->load(var, &bSuccess);
	if(FAILED(hr) || bSuccess == VARIANT_FALSE) //throw XMLException("Failed to load XML document");	
	{
		USES_CONVERSION;
		std::string s (	W2A(GetParsedError().c_str()));
		throw XMLDocumentException(s);
	}
}

void		XMLDocument::LoadFromString(const wchar_t* Arr)
{
	CComBSTR str( Arr );
	VARIANT_BOOL	bSuccess;
	HRESULT hr = m_pDoc->loadXML(str, &bSuccess);
	if(FAILED(hr) || !bSuccess) //throw XMLException("Failed to load XML document");	
	{
		USES_CONVERSION; 
		std::string s ( W2A(GetParsedError().c_str()));
		throw XMLDocumentException(s);
	}
}

void		XMLDocument::LoadFromString(const char* Arr)
{
	CComBSTR str( Arr );
	VARIANT_BOOL	bSuccess;
	HRESULT hr = m_pDoc->loadXML(str, &bSuccess);
	if(FAILED(hr) || !bSuccess) //throw XMLException("Failed to load XML document");	
	{
		USES_CONVERSION;
		std::string s ( W2A(GetParsedError().c_str()));
		throw XMLDocumentException(s);
	}
}

XMLElement	XMLDocument::GetRoot() const
{
	CComPtr<IXMLDOMElement> Elem;
	HRESULT hr = m_pDoc->get_documentElement(&Elem)  ;
	HrCheck< XMLDocumentException>( hr, "Failed to get the root element" );
	return XMLElement( Elem );
}

XMLDocument&	XMLDocument::operator <<( const XMLComment& Comment )
{
	HRESULT hr = m_pDoc->appendChild( Comment.m_pComment, 0 );
	HrCheck<XMLDocumentException>( hr, "Failed to append comment");
	return *this;
}

//************ XMLElement **************//

XMLElement::XMLElement(const XMLDocument& Tree, const std::tstring& Name)
{
	CComBSTR BSTRName( Name.c_str() );
	HRESULT hr = Tree.m_pDoc->createElement( BSTRName, &m_iXMLElem );
	HrCheck<XMLElementException> (hr, "Failed to create element");
}

XMLElement::XMLElement(const XMLElement& Brother, const std::tstring& Name)
{
	CComBSTR BSTRName( Name.c_str() );
	HRESULT hr = Brother.GetDocument()->createElement( BSTRName, &m_iXMLElem );
	HrCheck<XMLElementException> (hr, "Failed to create element");
}

CComPtr<IXMLDOMDocument>	XMLElement::GetDocument() const
{
	CComPtr<IXMLDOMDocument> pDOMDocument;
	HRESULT hr = m_iXMLElem->get_ownerDocument( &pDOMDocument.p); 
	HrCheck<XMLElementException> (hr, "Failed to get document");
	return pDOMDocument;
}

bool	XMLElement::iterator::operator == ( const iterator& itr ) const
{
	return m_iXMLList.p == itr.m_iXMLList.p && m_Position == itr.m_Position;
}

bool	XMLElement::iterator::operator != ( const iterator& itr ) //const
{
	return !m_iXMLList .IsEqualObject( itr.m_iXMLList.p) && m_Position != itr.m_Position;
}

XMLElement::iterator& XMLElement::iterator::operator = ( const iterator& itr )
{
	m_iXMLList = itr.m_iXMLList;
	m_Position = itr.m_Position;
	return *this;
}

XMLElement XMLElement::iterator::operator * ()  
{
	CComPtr<IXMLDOMNode> inode;
	m_iXMLList->get_item(m_Position,&inode);
	if (inode==0) throw XMLException("");
	DOMNodeType type; 
	inode->get_nodeType(&type);
	if (type!=NODE_ELEMENT) throw XMLException("");
	CComQIPtr<IXMLDOMElement> e(inode);
	return XMLElement(e.p);
}

XMLElement XMLElement::iterator::operator -> ()  
{
	CComPtr<IXMLDOMNode> inode;
	m_iXMLList->get_item(m_Position,&inode);
	if (inode==0) throw XMLException("");
	DOMNodeType type; 
	inode->get_nodeType(&type);
	if (type!=NODE_ELEMENT) throw XMLElementException("");
	CComQIPtr<IXMLDOMElement> e(inode);
	return XMLElement(e.p);	
}


XMLElement::iterator XMLElement::iterator::operator ++ (int)
{
	long len;
	DOMNodeType type; 
	m_iXMLList->get_length(&len);
	int Pos = m_Position;
	do
	{
		++m_Position;
		CComPtr<IXMLDOMNode> inode;
		m_iXMLList->get_item(m_Position,&inode);
		if (inode==0) break;
		inode->get_nodeType(&type);
	}
	while ( type != NODE_ELEMENT && m_Position <= len);
	return iterator( m_iXMLList, Pos );
}

XMLElement::iterator& XMLElement::iterator::operator ++ ()
{
	long len;
	DOMNodeType type; 
	m_iXMLList->get_length(&len);
	do
	{
		++m_Position;
		CComPtr<IXMLDOMNode> inode;
		m_iXMLList->get_item(m_Position,&inode);
		if (inode==0) break;
		inode->get_nodeType(&type);
	}
	while ( type != NODE_ELEMENT && m_Position <= len);
	return *this;
}

XMLElement& XMLElement::operator << ( const XMLElement& Elem )
{
	HRESULT hr = m_iXMLElem->appendChild( Elem.m_iXMLElem, 0 );
	HrCheck<XMLElementException>(hr, "Failed to add child element");
	return *this;
}

XMLElement& XMLElement::operator << ( const XMLText& Text )
{
	HRESULT hr = m_iXMLElem->appendChild( Text.m_iXMLText, 0 );
	HrCheck<XMLElementException>(hr, "Failed to add value");
	return *this;	
}

XMLElement& XMLElement::operator << ( const XMLComment& Comment )
{
	HRESULT hr = m_iXMLElem->appendChild( Comment.m_pComment, 0 );
	HrCheck<XMLElementException>(hr, "Failed to add comment");
	return *this;	
}

XMLElement& XMLElement::operator << ( const XMLAttribute& Attr)
{
	HRESULT hr = m_iXMLElem->setAttributeNode( Attr.m_pNative, 0 );
	HrCheck<XMLElementException>(hr, "Failed to add attribute");
	return *this;
}

std::tstring XMLElement::Name()
{
//	if (!elem) throw NameException(std::string());
	CComBSTR bn; 
	m_iXMLElem->get_tagName(&bn);
	USES_CONVERSION;
	LPCTSTR z = W2T(bn);
	return std::tstring(z);	
}

std::tstring XMLElement::Value()
{
	USES_CONVERSION;
	CComVariant val(VT_EMPTY);
	m_iXMLElem->get_nodeTypedValue(&val);
	if (val.vt==VT_BSTR) return std::tstring(W2CT(val.bstrVal));
	return std::tstring();	
}

XMLElement::iterator XMLElement::begin()
{
	CComPtr<IXMLDOMNodeList> iNodeList;
	m_iXMLElem->get_childNodes(&iNodeList);
	return iterator(iNodeList, 0);
}

XMLElement::iterator XMLElement::end()
{
	CComPtr<IXMLDOMNodeList> iNodeList;
	m_iXMLElem->get_childNodes(&iNodeList);
	long len;
	iNodeList->get_length(&len);
	return iterator(iNodeList, len);
}

std::tstring XMLElement::Attr(const std::tstring& Name) const
{
	USES_CONVERSION;
	CComBSTR bname(Name.c_str());
	CComVariant val(VT_EMPTY);
	m_iXMLElem->getAttribute(bname,&val);
	if (val.vt==VT_BSTR) return std::tstring(W2CT(val.bstrVal));
	throw XMLException("Failed to get attribute");
#if _MSC_VER < 1300
	return std::tstring();		
#endif
}
/*
XMLException::XMLException( IDOMDocument* pDoc )
{
	pDoc->get
}*/

std::wstring XMLDocument::GetParsedError() const
{
	CComPtr<IXMLDOMParseError> pError;
	m_pDoc->get_parseError(&pError.p);
	CComBSTR str;
	pError->get_reason(&str);
	std::wstring z (str);
	pError->get_srcText(&str);
	z += str;
	long pos;
	pError->get_filepos(&pos);
	return z;
}	

XMLComment::XMLComment(const XMLDocument& Tree, const std::tstring& Name)
{
	CComBSTR BSTRName( Name.c_str() );
	Tree.m_pDoc->createComment( BSTRName, &m_pComment );	
}

XMLProcInstr::XMLProcInstr(	const XMLElement& Elem,
							const std::tstring & Target, const std::tstring &Data )
	{
		CComPtr<IXMLDOMDocument> iXMLDoc = Elem.GetDocument();
		CComBSTR bstrTarget(Target.c_str()), bstrData( Data.c_str() );
		HRESULT hr = iXMLDoc->createProcessingInstruction( bstrTarget, bstrData, &m_pNative  );
		HrCheck<XMLProcInstrException>(hr, "Failed to create processing instruction");
	}
/*
XMLProcInstr&	XMLProcInstr::operator <<( const XMLAttribute& Attrib )
{
//	CComPtr<IXMLDOMDocument> iXMLDoc = Elem.GetDocument();
	HRESULT hr = m_pNative->appendChild( Attrib.m_pNative, 0 );
	HrCheck<XMLProcInstrException>(hr, "Failed to add the attribute");	
	return *this;
}
*/
XMLDocument&	XMLDocument::operator <<( const XMLProcInstr& Instr )
{
	HRESULT hr = m_pDoc->appendChild( Instr.m_pNative, 0);
	HrCheck<XMLDocumentException>( hr, "Failed to append processing instruction");	
	return *this;
}

};