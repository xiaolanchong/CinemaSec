//
// inline function for XmlLite stuff
//////////////////////////////////////////////////////////////////////

//#include "stdafx.h"
//#include "xmllite.h"

namespace XmlLite  
{

	inline std::wstring GetErrorDesc(REFIID riid, IUnknown* pUnknown)
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

	inline XMLDocument::XMLDocument()
	{	
		m_pDoc.CoCreateInstance(__uuidof(DOMDocument));	
	}

	inline void	XMLDocument::SetRoot(const XMLElement& ElRoot)
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

	inline void	XMLDocument::SaveToString( std::wstring& sData ) const
	{
		
	}

	inline void	XMLDocument::SaveToBinary(std::vector<unsigned char>& Arr) const
	{
		MemStreamWrite WStream(Arr);
		CComVariant var(&WStream);
		HRESULT hr = m_pDoc->save(var);
		HrCheck<XMLDocumentException>( hr, "Failed to save the document");
	}

	inline void	XMLDocument::SaveToFile( const TCHAR* szFileName) const
	{
		CComVariant var(szFileName);
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

	inline void	XMLDocument::LoadFromBinary(const void* pData, size_t nSize)
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

	inline void		XMLDocument::LoadFromString(const wchar_t* Arr)
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

	inline void		XMLDocument::LoadFromString(const char* Arr)
	{
		CComBSTR str( Arr );
		VARIANT_BOOL	bSuccess;
		HRESULT hr = m_pDoc->loadXML(str, &bSuccess);
		if(FAILED(hr) || !bSuccess) //throw XMLException("Failed to load XML document");	
		{
			USES_CONVERSION;
			std::string s ( W2CA(GetParsedError().c_str()));
			throw XMLDocumentException(s);
		}
	}

	inline void		XMLDocument::LoadFromFile( const TCHAR* szFileName )
	{
		HRESULT hr;
		VARIANT_BOOL bResult;
		hr = m_pDoc->load( CComVariant(szFileName), &bResult );
		if( hr != S_OK || bResult != VARIANT_TRUE )
		{
			USES_CONVERSION;
			std::string s ( W2CA(GetParsedError().c_str()));
			throw XMLDocumentException(s);
		}
	}

	inline XMLElement	XMLDocument::GetRoot() const
	{
		CComPtr<IXMLDOMElement> Elem;
		HRESULT hr = m_pDoc->get_documentElement(&Elem)  ;
		HrCheck< XMLDocumentException>( hr, "Failed to get the root element" );
		return XMLElement( Elem );
	}

	inline XMLDocument&	XMLDocument::operator <<( const XMLComment& Comment )
	{
		HRESULT hr = m_pDoc->appendChild( Comment.m_pComment, 0 );
		HrCheck<XMLDocumentException>( hr, "Failed to append comment");
		return *this;
	}

	//************ XMLElement **************//

inline XMLElement::XMLElement(const XMLDocument& Tree, const std::tstring& Name)
{
	CComBSTR BSTRName( Name.c_str() );
	HRESULT hr = Tree.m_pDoc->createElement( BSTRName, &m_iXMLElem );
	HrCheck<XMLElementException> (hr, "Failed to create element");
}

inline XMLElement::XMLElement(const XMLElement& Brother, const std::tstring& Name)
{
	CComBSTR BSTRName( Name.c_str() );
	HRESULT hr = Brother.GetDocument()->createElement( BSTRName, &m_iXMLElem );
	HrCheck<XMLElementException> (hr, "Failed to create element");
}

inline XMLElement::XMLElement(CComPtr<IXMLDOMNode> pNode):
	m_iXMLElem( CComQIPtr<IXMLDOMElement>(pNode) )
{
	if( !m_iXMLElem )
		throw XMLElementException( "Failed to create element from node");
}

inline CComPtr<IXMLDOMDocument>	XMLElement::GetDocument() const
{
	CComPtr<IXMLDOMDocument> pDOMDocument;
	HRESULT hr = m_iXMLElem->get_ownerDocument( &pDOMDocument.p); 
	HrCheck<XMLElementException> (hr, "Failed to get document");
	return pDOMDocument;
}
#if 0
bool	XMLElement::const_iterator::operator == ( const iterator& itr ) const
{
	return m_iXMLList.p == itr.m_iXMLList.p && m_Position == itr.m_Position;
}

bool	XMLElement::const_iterator::operator != ( const iterator& itr ) //const
{
	return !m_iXMLList .IsEqualObject( itr.m_iXMLList.p) && m_Position != itr.m_Position;
}
#endif

inline bool XMLElement::const_iterator::operator==(const const_iterator& _Right) const
{	// test for iterator equality
	return (m_Position == _Right.m_Position);
}

inline bool XMLElement::const_iterator::operator!=(const const_iterator& _Right) const
{	// test for iterator inequality
	return (!(*this == _Right));
}

inline XMLElement::const_iterator& XMLElement::const_iterator::operator = ( const const_iterator& itr )
{
	m_iXMLList = itr.m_iXMLList;
	m_Position = itr.m_Position;
	return *this;
}

inline CComPtr<IXMLDOMNode> XMLElement::const_iterator::operator * ()  
{
	HRESULT hr;
	CComPtr<IXMLDOMNode> pNode;
	hr = m_iXMLList->get_item(m_Position, &pNode);
	return pNode;
}

inline CComPtr<IXMLDOMNode> XMLElement::const_iterator::operator -> ()  
{
	HRESULT hr;
	CComPtr<IXMLDOMNode> pNode;
	hr = m_iXMLList->get_item(m_Position, &pNode);
	return pNode;	
}

inline XMLElement::const_iterator XMLElement::const_iterator::operator ++ (int)
{
	XMLElement::const_iterator _Temp = *this;
	++m_Position;
	return _Temp;
}

inline XMLElement::const_iterator& XMLElement::const_iterator::operator ++ ()
{
	++m_Position;
	return *this;
}
inline XMLElement::const_iterator& XMLElement::const_iterator::operator--()
{	// predecrement
	--m_Position;
	return (*this);
}

inline XMLElement::const_iterator XMLElement::const_iterator::operator--(int)
{	// postdecrement
	const_iterator _Tmp = *this;
	--*this;
	return (_Tmp);
}

inline XMLElement::const_iterator& XMLElement::const_iterator::operator+=(difference_type _Off)
{	// increment by integer
	m_Position += _Off;
	return (*this);
}

inline XMLElement::const_iterator XMLElement::const_iterator::operator+(difference_type _Off) const
{	// return this + integer
	const_iterator _Tmp = *this;
	return (_Tmp += _Off);
}

inline XMLElement::const_iterator& XMLElement::const_iterator::operator-=(difference_type _Off)
{	// decrement by integer
	return (*this += -_Off);
}

inline XMLElement::const_iterator XMLElement::const_iterator::operator-(difference_type _Off) const
{	// return this - integer
	const_iterator _Tmp = *this;
	return (_Tmp -= _Off);
}

inline XMLElement::const_iterator::difference_type 
	XMLElement::const_iterator::operator-(const XMLElement::const_iterator& _Right) const
{	// return difference of iterators
	return (m_Position - _Right.m_Position);
}

inline XMLElement::const_iterator::const_reference 
	XMLElement::const_iterator::operator[](XMLElement::const_iterator::difference_type _Off) const
{	// subscript
	return (*(*this + _Off));
}

inline bool XMLElement::const_iterator::operator<(const XMLElement::const_iterator& _Right) const
{	// test if this < _Right
	return (m_Position < _Right.m_Position);
}

inline bool XMLElement::const_iterator::operator>(const XMLElement::const_iterator& _Right) const
{	// test if this > _Right
	return (_Right < *this);
}

inline bool XMLElement::const_iterator::operator<=(const XMLElement::const_iterator& _Right) const
{	// test if this <= _Right
	return (!(_Right < *this));
}

inline bool XMLElement::const_iterator::operator>=(const XMLElement::const_iterator& _Right) const
{	// test if this >= _Right
	return (!(*this < _Right));
}

inline XMLElement::const_iterator operator+(XMLElement::const_iterator::difference_type _Off,
									 const XMLElement::const_iterator& _Right)
{	// return iterator + integer
	return (_Right + _Off);
}

////////// and iterator

inline XMLElement::iterator::reference XMLElement::iterator::operator*() 
{	// return designated object
	return ((reference)**(const_iterator *)this);
}

inline XMLElement::iterator::pointer XMLElement::iterator::operator->() 
{	// return pointer to class object
	return ((pointer)const_iterator::operator->());
}

inline XMLElement::iterator& XMLElement::iterator::operator++()
{	// preincrement
	const_iterator::operator++();
	return (*this);
}

inline XMLElement::iterator XMLElement::iterator::operator++(int)
{	// postincrement
	iterator _Tmp = *this;
	++*this;
	return (_Tmp);
}

inline XMLElement::iterator& XMLElement::iterator::operator--()
{	// predecrement
	const_iterator::operator--();
	return (*this);
}

inline XMLElement::iterator XMLElement::iterator::operator--(int)
{	// postdecrement
	iterator _Tmp = *this;
	--*this;
	return (_Tmp);
}

inline XMLElement::iterator& XMLElement::iterator::operator+=(XMLElement::iterator::difference_type _Off)
{	// increment by integer
	const_iterator::operator +=(_Off);
//	this->_Myptr += _Off;
	return (*this);
}

inline XMLElement::iterator XMLElement::iterator::operator+(XMLElement::iterator::difference_type _Off) const
{	// return this + integer
	iterator _Tmp = *this;
	return (_Tmp += _Off);
}

inline XMLElement::iterator& XMLElement::iterator::operator-=(XMLElement::iterator::difference_type _Off)
{	// decrement by integer
	return (*this += -_Off);
}

inline XMLElement::iterator XMLElement::iterator::operator-(XMLElement::iterator::difference_type _Off) const
{	// return this - integer
	iterator _Tmp = *this;
	return (_Tmp -= _Off);
}

inline XMLElement::iterator::difference_type 
	XMLElement::iterator::operator-(const XMLElement::iterator& _Right) const
{	// return difference of iterators
	return ((const_iterator)*this - _Right);
}

inline XMLElement::iterator::reference XMLElement::iterator::operator[]
	(XMLElement::iterator::difference_type _Off) 
{	// subscript
	return (*(*this + _Off));
}

inline XMLElement::iterator operator+
					(	XMLElement::iterator::difference_type _Off,
						const XMLElement::iterator& _Right)
{	// return iterator + integer
	return (_Right + _Off);
}


	/////////////////////////////

	inline XMLElement& XMLElement::operator << ( const XMLElement& Elem )
	{
		HRESULT hr = m_iXMLElem->appendChild( Elem.m_iXMLElem, 0 );
		HrCheck<XMLElementException>(hr, "Failed to add child element");
		return *this;
	}

	inline XMLElement& XMLElement::operator << ( const XMLText& Text )
	{
		HRESULT hr = m_iXMLElem->appendChild( Text.m_iXMLText, 0 );
		HrCheck<XMLElementException>(hr, "Failed to add value");
		return *this;	
	}

	inline XMLElement& XMLElement::operator << ( const XMLComment& Comment )
	{
		HRESULT hr = m_iXMLElem->appendChild( Comment.m_pComment, 0 );
		HrCheck<XMLElementException>(hr, "Failed to add comment");
		return *this;	
	}

	inline XMLElement& XMLElement::operator << ( const XMLAttribute& Attr)
	{
		HRESULT hr = m_iXMLElem->setAttributeNode( Attr.m_pNative, 0 );
		HrCheck<XMLElementException>(hr, "Failed to add attribute");
		return *this;
	}

	inline std::tstring XMLElement::Name()
	{
		//	if (!elem) throw NameException(std::string());
		CComBSTR bn; 
		m_iXMLElem->get_tagName(&bn);
		USES_CONVERSION;
		LPCTSTR z = W2CT(bn);
		return std::tstring(z);	
	}

	inline std::tstring XMLElement::Value()
	{
		USES_CONVERSION;
		CComVariant val(VT_EMPTY);
		m_iXMLElem->get_nodeTypedValue(&val);
		if (val.vt==VT_BSTR) return std::tstring(W2CT(val.bstrVal));
		return std::tstring();	
	}

	inline XMLElement::const_iterator XMLElement::begin() const 
	{
		CComPtr<IXMLDOMNodeList> iNodeList;
		m_iXMLElem->get_childNodes(&iNodeList);
		return const_iterator(iNodeList, 0);
	}

	inline XMLElement::const_iterator XMLElement::end() const
	{
		CComPtr<IXMLDOMNodeList> iNodeList;
		m_iXMLElem->get_childNodes(&iNodeList);
		long len;
		iNodeList->get_length(&len);
		return const_iterator(iNodeList, len);
	}

	inline XMLElement::iterator XMLElement::begin()
	{
		CComPtr<IXMLDOMNodeList> iNodeList;
		m_iXMLElem->get_childNodes(&iNodeList);
		return iterator(iNodeList, 0);
	}

	inline XMLElement::iterator XMLElement::end()
	{
		CComPtr<IXMLDOMNodeList> iNodeList;
		m_iXMLElem->get_childNodes(&iNodeList);
		long len;
		iNodeList->get_length(&len);
		return iterator(iNodeList, len);
	}

	inline std::tstring XMLElement::Attr(const std::tstring& Name) const
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

	inline std::wstring XMLDocument::GetParsedError() const
	{
		CComPtr<IXMLDOMParseError> pError;
		m_pDoc->get_parseError(&pError.p);
		CComBSTR str;
		pError->get_reason(&str);
		std::wstring z (str? str : L"");
		pError->get_srcText(&str);
		z += str? str : L"";
		long pos;
		pError->get_filepos(&pos);
		return z;
	}	

	inline XMLComment::XMLComment(const XMLDocument& Tree, const std::tstring& Name)
	{
		CComBSTR BSTRName( Name.c_str() );
		Tree.m_pDoc->createComment( BSTRName, &m_pComment );	
	}

	inline XMLProcInstr::XMLProcInstr(	const XMLElement& Elem,
		const std::tstring & Target, const std::tstring &Data )
	{
		CComPtr<IXMLDOMDocument> iXMLDoc = Elem.GetDocument();
		CComBSTR bstrTarget(Target.c_str()), bstrData( Data.c_str() );
		HRESULT hr = iXMLDoc->createProcessingInstruction( bstrTarget, bstrData, &m_pNative  );
		HrCheck<XMLProcInstrException>(hr, "Failed to create processing instruction");
	}

	inline XMLDocument&	XMLDocument::operator <<( const XMLProcInstr& Instr )
	{
		HRESULT hr = m_pDoc->appendChild( Instr.m_pNative, 0);
		HrCheck<XMLDocumentException>( hr, "Failed to append processing instruction");	
		return *this;
	}

};