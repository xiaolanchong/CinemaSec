//////////////////////////////////////////////////////////////////////
// Description
//	XmlLite - враппер над MSXML parser(XML DOM) версии 2.0 и выше
//	tested on Unicode & MBCS
//
// Author	: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
// Version	: 1.3 
// Last Modified : 08.03.2005
//	Notes:	1) You can use other MSXML DOM interface methods thanks to public COM ptr
//			2) MSXML 3.0 is the system component, I consider you can safely use parser
//			   at Win2k sp4/XP/2003
//			3) MSXML 4.0 is the optional extention, delivered by ie & ms office, see SDK
//			4) MSXML 5.0 is the same as above
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_XMLLITE_H__76532DCF_4388_457D_8F31_1D36F03DC64A__INCLUDED_)
#define AFX_XMLLITE_H__76532DCF_4388_457D_8F31_1D36F03DC64A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// we want LONGLONG & ULONGLONG support
// but VT_I8 & VT_UI8 supports only by XP and later

#include <atlbase.h>
#include <msxml.h>
#include <vector>
#include <iterator>
// for easy exception definition
#include "../common/Exception.h"
// for std::(w)string
#include "../common/tstring.h"

MACRO_EXCEPTION( XMLException,			CommonException )
	MACRO_EXCEPTION( XMLInvalidVersionException,	XMLException )
	MACRO_EXCEPTION( XMLDocumentException,	XMLException )
	MACRO_EXCEPTION( XMLElementException,	XMLException )
		MACRO_EXCEPTION( XMLElementBadTypeException,	XMLElementException )
		MACRO_EXCEPTION( XMLElementIteratorException,	XMLElementException )
	MACRO_EXCEPTION( XMLAttributeException,			XMLException )
		MACRO_EXCEPTION( XMLAttributeBadTypeException,		XMLAttributeException )
	MACRO_EXCEPTION( XMLProcInstrException,	XMLException )

/* 
	  From MSDN/SDK/COM/Automation/IDispatch
	  pointer & ref types not in XML text, I removed it

      Byte                    bVal;                 // VT_UI1.
      Short                   iVal;                 // VT_I2.
      long                    lVal;                 // VT_I4.
      float                   fltVal;               // VT_R4.
      double                  dblVal;               // VT_R8.
      VARIANT_BOOL            boolVal;              // VT_BOOL.
      CY                      cyVal;                // VT_CY.
      DATE                    date;                 // VT_DATE.
      BSTR                    bstrVal;              // VT_BSTR.
      char                    cVal;                 // VT_I1.
      unsigned short          uiVal;                // VT_UI2.
      unsigned long           ulVal;                // VT_UI4.
      int                     intVal;               // VT_INT.
      unsigned int            uintVal;              // VT_UINT.
*/

template <typename T> struct Type2Var {};

#define TYPE2VAR( T, Var ) template<> struct Type2Var<T> { enum { Type = Var}; };

TYPE2VAR( char				, VT_I1		)
TYPE2VAR( unsigned char		, VT_UI1	)
TYPE2VAR( short				, VT_I2		)
TYPE2VAR( unsigned short	, VT_UI2	)
TYPE2VAR( int				, VT_INT	)
TYPE2VAR( unsigned int		, VT_UINT	)
TYPE2VAR( long 				, VT_I4		)
TYPE2VAR( unsigned long		, VT_UI4	)
TYPE2VAR( float 			, VT_R4		)
TYPE2VAR( double 			, VT_R8		)
TYPE2VAR( BSTR 				, VT_BSTR	)
TYPE2VAR( CY				, VT_CY		)
TYPE2VAR( __int64			, VT_I8		)
TYPE2VAR( unsigned __int64	, VT_UI8	)
//TYPE2VAR( VARIANT_BOOL		, VT_BOOL	)	//unsigned short
//TYPE2VAR( DATE				, VT_DATE	)	//double


template<typename T> inline void GetFieldFromVariant( const VARIANT&, T&  );
// functions differ only by return type
#define GET_FIELD(Type, Field)  template<> inline void GetFieldFromVariant<Type>( const VARIANT& var, Type &val) { val = var.##Field ;};
GET_FIELD( char				, cVal		)
GET_FIELD( unsigned char	, bVal		)
GET_FIELD( short			, iVal		)
GET_FIELD( unsigned short	, uiVal		)
GET_FIELD( int				, intVal	)
GET_FIELD( unsigned int		, uintVal	)
GET_FIELD( long 			, lVal		)
GET_FIELD( unsigned long	, ulVal		)
GET_FIELD( float 			, fltVal	)
GET_FIELD( double 			, dblVal	)
GET_FIELD( BSTR 			, bstrVal	)
GET_FIELD( CY				, cyVal		)

template <typename T> struct Type2StringDT {};
#define TYPE2DT( T, x ) template<> struct Type2StringDT<T> { static const TCHAR* GetName() { return x;} };

TYPE2DT( char				, _T("i1")	)
TYPE2DT( unsigned char		, _T("ui1")	)
TYPE2DT( short				, _T("i2")	)
TYPE2DT( unsigned short		, _T("ui2")	)
TYPE2DT( int				, _T("i4") 	)
TYPE2DT( unsigned int		, _T("ui4")	)
TYPE2DT( long 				, _T("i4")		)
TYPE2DT( unsigned long		, _T("ui4")	)
TYPE2DT( __int64 			, _T("i8")	)
TYPE2DT( unsigned __int64	, _T("ui8")	)
TYPE2DT( float 				, _T("r4")		)
TYPE2DT( double 			, _T("r8")		)
TYPE2DT( BSTR 				, _T("string")	)
TYPE2DT( std::string 		, _T("string")	)
TYPE2DT( std::wstring 		, _T("string")	)
TYPE2DT( CY					, _T("fixed.14.4 ")		)

// all values can be passed through value since they are smart pointer
// but COM reference stuff is more harder then additional reference
// so use pass by ref

namespace XmlLite  
{

template <class T>inline void HrCheck( HRESULT hr, const char* szCause )
{
	if( S_OK != (hr) ) throw T(szCause); 
}
	

// trivial but for resolve	
template<typename VectorItr> class XMLBinaryOut
{
	friend class XMLElement;
	VectorItr	m_First;
	size_t		m_Size;
public:
	XMLBinaryOut(VectorItr First, size_t Size ) : m_First(First), m_Size(Size){}
};

template<typename VectorType> class XMLBinaryIn
{
	friend class XMLElement;
	VectorType&		m_Arr;
public:
	XMLBinaryIn(VectorType& Arr ) : m_Arr(Arr){}
};

class XMLDocument;
class XMLElement;
class XMLText;

// XMLText - class for tag text in XMLElement

class XMLText
{
	friend class XMLElement;
public:
	CComPtr<IXMLDOMText> m_iXMLText;
	
	template<typename T> XMLText(const XMLDocument& Tree,const T& Value)
	{
		CComBSTR var;
		Tree.m_pDoc->createTextNode( var, &m_iXMLText );
		CComVariant v(Value);
		m_iXMLText->put_nodeValue(v);
	}
	template<typename T> XMLText(const XMLElement& Tree,const T& Value)
	{
		CComBSTR var;
		Tree.GetDocument()->createTextNode( var, &m_iXMLText );
		CComVariant v(Value);
		m_iXMLText->put_nodeValue(v);
	}	
};

// XMLComment presents comment in xml text

struct XMLComment
{
	friend class XMLElement;
	CComPtr<IXMLDOMComment>	m_pComment;

	XMLComment(const XMLDocument& Tree, const std::tstring& Name);
	XMLComment(const XMLComment& c) : m_pComment(c.m_pComment){}
};

struct XMLAttribute
{
	CComPtr<IXMLDOMAttribute> m_pNative;

	template <typename T>
		XMLAttribute(const XMLElement& Elem, const std::tstring& Name,/*const T&*/T Value, bool bTyped = true)
	{
		HRESULT hr;
		CComPtr<IXMLDOMDocument> iXMLDoc = Elem.GetDocument();
		CComBSTR bstrName(Name.c_str());
		hr = iXMLDoc->createAttribute( bstrName, &m_pNative );
		HrCheck<XMLAttributeException>(hr, "Failed to create the attribute");
		CComVariant var( Value );
		hr = m_pNative->put_value( var );
		HrCheck<XMLAttributeException>(hr, "Failed to put value in the attribute");
#if 0 // type is meaningless w/o DTD or schema
		if( bTyped)	hr = m_pNative->put_nodeTypedValue( var );
		else		hr = m_pNative->put_value( var );
		HrCheck<XMLAttributeException>(hr, "Failed to put value in the attribute");
#ifdef OLDER_THAN_YOU_THINK
		if( bTyped)
		{
			hr = m_pNative->put_dataType( CComBSTR( Type2StringDT<T>::GetName() ) );
			HrCheck<XMLAttributeException>(hr, "Failed to set the data type to the attribute");
		}
#endif
#endif
	}
};	

//////////////////// XMLElement //////////////////////////
// for access xml elements like <my_elem attr="yeah">This is a text</my_elem>

class XMLElement
{
	friend class XMLDocument;
public:	
	CComPtr<IXMLDOMElement>		m_iXMLElem;

	XMLElement(IXMLDOMElement* Ptr = 0): m_iXMLElem(Ptr){}
	XMLElement(const XMLElement& Rhs): m_iXMLElem(Rhs.m_iXMLElem){}
	XMLElement(const XMLDocument& Tree, const std::tstring& Name);
	XMLElement(const XMLElement& Brother, const std::tstring& Name);
	XMLElement(CComPtr<IXMLDOMNode> pNode);
	XMLElement& operator= (const XMLElement& Rhs)
	{
		m_iXMLElem = Rhs.m_iXMLElem;
		return *this;
	}

	// XMLNodeList implements random access to XMLNodes, but i recomend to use
	// IXMLElement::selectNodes/selectSingleNode & XPath for search/iterate

	class const_iterator : public std::_Ranit< 
					CComPtr<IXMLDOMNode>, 
					long, 
					CComPtr<IXMLDOMNode>, 
					CComPtr<IXMLDOMNode> >
	{
	public:
		typedef std::random_access_iterator_tag iterator_category;
		typedef CComPtr<IXMLDOMNode>	value_type;
		typedef long					difference_type;
		typedef CComPtr<IXMLDOMNode>	pointer;
		typedef CComPtr<IXMLDOMNode>	const_reference;
	protected:
		friend class XMLElement;
		CComPtr<IXMLDOMNodeList> m_iXMLList;
		difference_type			 m_Position;
		const_iterator (CComPtr<IXMLDOMNodeList> List, difference_type Pos): m_iXMLList( List ), m_Position(Pos){}
	public:
		const_iterator (): m_Position(0){}
		const_iterator (  const const_iterator& _Right) : m_iXMLList( _Right.m_iXMLList ), m_Position(_Right.m_Position){}

		bool	operator == (  const const_iterator& _Right ) const;
		bool	operator!=(const const_iterator& _Right) const;

		const_iterator& operator = ( const const_iterator& itr) ;
		CComPtr<IXMLDOMNode> operator * ()  ;
		CComPtr<IXMLDOMNode> operator -> ()  ;

		const_iterator operator ++ (int);
		const_iterator& operator ++ ();

		const_iterator operator -- (int);
		const_iterator& operator -- ();
		const_iterator& operator+=(difference_type _Off);
		const_iterator operator+(difference_type _Off) const;
		const_iterator& operator-=(difference_type _Off);
		const_iterator operator-(difference_type _Off) const;

		difference_type operator-(const const_iterator& _Right) const;
		const_reference operator[](difference_type _Off) const;
		bool operator<(const const_iterator& _Right) const;
		bool operator<=(const const_iterator& _Right) const;
		bool operator>(const const_iterator& _Right) const;
		bool operator>=(const const_iterator& _Right) const;

		friend const_iterator operator+(difference_type _Off, const const_iterator& _Right);
	};

	class iterator: public const_iterator
	{
		friend class XMLElement;
		iterator (CComPtr<IXMLDOMNodeList> List, difference_type Pos): const_iterator( List, Pos){}
	public:
		typedef CComPtr<IXMLDOMNode>	reference;

		iterator ( const iterator& itr) : const_iterator( itr){}
		iterator (){}

		iterator& operator = ( const iterator& itr) ;
		reference operator * ()  ;
		pointer operator -> ()  ;

		iterator operator ++ (int);
		iterator& operator ++ ();

		iterator operator -- (int);
		iterator& operator -- ();
		iterator& operator+=(difference_type _Off);
		iterator operator+(difference_type _Off) const;
		iterator& operator-=(difference_type _Off);
		iterator operator-(difference_type _Off) const;

		difference_type operator-(const iterator& _Right) const;
		reference operator[](difference_type _Off) ;	

		friend iterator operator+(difference_type _Off, const iterator& _Right);
	};

	typedef std::reverse_iterator<iterator> reverse_iterator;
	typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

	// like stl
	const_iterator begin() const ;
	const_iterator end() const;

	iterator begin()  ;
	iterator end() ;

	reverse_iterator rbegin()
	{	// return iterator for beginning of reversed mutable sequence
		return (reverse_iterator(end()));
	}

	const_reverse_iterator rbegin() const
	{	// return iterator for beginning of reversed nonmutable sequence
		return (const_reverse_iterator(end()));
	}

	reverse_iterator rend()
	{	// return iterator for end of reversed mutable sequence
		return (reverse_iterator(begin()));
	}

	const_reverse_iterator rend() const
	{	// return iterator for end of reversed nonmutable sequence
		return (const_reverse_iterator(begin()));
	}

	CComPtr<IXMLDOMDocument>	GetDocument() const;
	
	XMLElement* operator -> ()   { return this;};
	bool	operator ! () const { return m_iXMLElem.p != 0;}
	
	std::tstring Name();
	std::tstring Value();
	std::tstring Attr(const std::tstring& Name) const;

	template <typename T> void GetValue( const T& Value)
	{
		CComVariant val( Type2Var<T>::Type);
		m_iXMLElem->get_nodeTypedValue(&val);
		T Value;
		GetFieldFromVariant<T>(val, Value);
	}

	template <typename T> T GetAttr( const std::tstring& Name, T& Value)
	{
		CComVariant VarGeneric( VT_EMPTY );
		CComBSTR bstrName( Name.c_str() );
		HRESULT hr;
#if 1 // new
		//m_iXMLElem
		CComPtr<IXMLDOMAttribute> pAttr;
		hr =  m_iXMLElem->getAttributeNode(bstrName, &pAttr);
		HrCheck<XMLElementException>(hr, "No such attribute");
		pAttr->put_dataType( CComBSTR( Type2StringDT<T>::GetName() ) );
		pAttr->get_value( &VarGeneric );
#else
		hr =  m_iXMLElem->getAttribute(bstrName, &VarGeneric);
		HrCheck<XMLElementException>(hr, "No such attribute");
#endif
		hr = VarGeneric.ChangeType( Type2Var<T>::Type );
		HrCheck<XMLElementException>(hr, "Can't convert to this type");
		GetFieldFromVariant<T>(VarGeneric, Value);
		return Value;
	}

	// we don't want BSTR, use std::wstring or str::string
	template <>	std::tstring GetAttr<std::tstring>( const std::tstring& Name, std::tstring& Value) 
	{
		CComVariant VarGeneric( VT_EMPTY );
		CComBSTR bstrName( Name.c_str() );
		HRESULT hr =  m_iXMLElem->getAttribute(bstrName, &VarGeneric);
		HrCheck<XMLElementException>(hr, "No such attribute");
		hr = VarGeneric.ChangeType( VT_BSTR );
		HrCheck<XMLElementException>(hr, "Can't convert to this type");
		USES_CONVERSION;
		Value = W2T(VarGeneric.bstrVal);
		return Value;		
	}

	XMLElement& operator << ( const XMLElement& Elem );
	XMLElement& operator << ( const XMLText&	Text );
	XMLElement& operator << ( const XMLComment& Comment );
	XMLElement& operator << ( const XMLAttribute& Attr);
	
	template<typename VectorItr> 
		XMLElement& operator << ( const XMLBinaryOut< VectorItr>& Bin)
	{
		CComQIPtr<IXMLDOMNode> pNode(m_iXMLElem);
		CComBSTR str(L"bin.base64");
		pNode->put_dataType( str );

		SAFEARRAY * psa;
		HRESULT hr; 
		int s = Bin.m_Size * sizeof(*Bin.m_First);
		psa = SafeArrayCreateVector(/*Type2Var<ElementType>::Type*/VT_UI1 ,  0, s );
		hr = SafeArrayLock(psa);
		
		memcpy( psa->pvData, &Bin.m_First[0], s);
		hr = SafeArrayUnlock(psa);

		VARIANT var;
		var.vt = VT_ARRAY| VT_UI1;//(Type2Var<ElementType>::Type);
		var.parray = psa;
		hr = pNode->put_nodeTypedValue(var);
		hr = VariantClear(&var);
		return *this;
	}

	template< typename VectorType> 
		XMLElement& operator >> ( XMLBinaryIn< VectorType>& Bin)
	{
		CComQIPtr<IXMLDOMNode> pNode(m_iXMLElem);
		HRESULT hr;
		VARIANT var;
		hr = pNode->get_nodeTypedValue(&var);
		BYTE* pData;
		hr = SafeArrayAccessData(var.parray, (void HUGEP**)&pData);
		size_t size = var.parray->rgsabound[0].cElements;
		Bin.m_Arr.resize( size * sizeof(BYTE));
		memcpy( &Bin.m_Arr[0], pData, size  );
		hr = SafeArrayUnaccessData(var.parray);
		hr = VariantClear(&var);
		return *this;		
	}
};	

//////////////////// XMLProcInstr //////////////////////////
// for parser directives like <xml? version="1.0" encoding="UTF-8"
// I recomend  always to add following code for 
//   <?xml version="1.0" encoding="UTF-8" standalone="yes"?>
// XMLProcInstr(elRoot, _T("xml"), _T("version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\""));

struct XMLProcInstr
{
	CComPtr<IXMLDOMProcessingInstruction> m_pNative;
	XMLProcInstr(	const XMLElement& Elem, const std::tstring & Target, const std::tstring &Data );
};

struct XMLDefaultProcInstr : XMLProcInstr
{
	XMLDefaultProcInstr( const XMLElement& Elem, const TCHAR* szEncoding = _T("UTF-8"), bool bStandalone = true ) : 
		XMLProcInstr(	Elem, 
			std::tstring(_T("xml")), 
			std::tstring( 
				std::tstring(_T("version=\"1.0\" encoding=\"") ) + 
				std::tstring(szEncoding) +
				std::tstring(_T("\" standalone=") ) +
				std::tstring( bStandalone ? _T("\"yes\"") : _T("\"no\"") )
						) )
		{}
};

//////////////////// XMLDocument //////////////////////////

class XMLDocument
{
	friend XMLElement;
	friend XMLText;
	friend XMLComment;
	
	// com initializer, i have not to initialize COM yourself
	// but if CoInitialize(Ex) failed you mustn't call CoUninitialize!
	struct ComInitializer
	{
		HRESULT m_hr;
		ComInitializer(): m_hr ( CoInitialize(0) ) {}
		~ComInitializer(){ if( m_hr == S_OK) CoUninitialize();}
	} m_ComInit;	
	
	std::wstring GetParsedError() const;
public:
	CComPtr<IXMLDOMDocument>	m_pDoc;
	
	XMLDocument();
	XMLDocument( CComPtr<IXMLDOMDocument> ptr);
	
	XMLDocument& operator= (const XMLDocument& Rhs)
	{
		m_pDoc = Rhs.m_pDoc;
		return *this;
	}

	void		LoadFromBinary(const void* pData, size_t nSize);
	void		LoadFromString(const wchar_t* Arr);
	void		LoadFromString(const char* Arr);
	void		LoadFromFile( const TCHAR* szFileName );

	void		SaveToBinary( std::vector<unsigned char>& Data) const;
	void		SaveToString( std::wstring& sData ) const;
	void		SaveToFile(const TCHAR* szFileName ) const;

	XMLElement	GetRoot() const;
	void		SetRoot(const XMLElement& ElRoot);
	
	XMLDocument&	operator <<( const XMLComment& Comment );
	XMLDocument&	operator <<( const XMLProcInstr& Instr );
};
	
}

#include "xmllite.inl"

#endif // !defined(AFX_XMLLITE_H__76532DCF_4388_457D_8F31_1D36F03DC64A__INCLUDED_)
