// XmlLite - враппер над MSXML parser(XML DOM) версии 2.0 и выше
// tested on Unicode & MBCS
// 
////TODO: разобраться с комментариями и процесс инструкциями при разборе
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_XMLLITE_H__76532DCF_4388_457D_8F31_1D36F03DC64A__INCLUDED_)
#define AFX_XMLLITE_H__76532DCF_4388_457D_8F31_1D36F03DC64A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <atlbase.h>
#include <msxml.h>
#include <vector>
#include "../common/Exception.h"
#include "../common/tstring.h"

MACRO_EXCEPTION( XMLException,			CommonException );
MACRO_EXCEPTION( XMLDocumentException,	XMLException )
MACRO_EXCEPTION( XMLElementException,	XMLException )
MACRO_EXCEPTION( XMLAttribute,			XMLException )
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

// all values can be passed through value since they are smart pointer
// but COM reference stuff is more harder then additional reference
// so use pass by ref

namespace XmlLite  
{

template <class T>inline void HrCheck( HRESULT hr, const char* szCause )
{
	if( S_OK != (hr) ) throw T(szCause); 
}
	

// trivial but for resoilve	
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
		XMLAttribute(const XMLElement& Elem, const std::tstring& Name,const T& Value)
	{
		CComPtr<IXMLDOMDocument> iXMLDoc = Elem.GetDocument();
		CComBSTR bstrName(Name.c_str());
		iXMLDoc->createAttribute( bstrName, &m_pNative );
		CComVariant var( Value );
		m_pNative->put_value( var );		
	}
};	

// Main tag class

class XMLElement
{
	friend class XMLDocument;
public:	
	CComPtr<IXMLDOMElement>		m_iXMLElem;

	XMLElement(IXMLDOMElement* Ptr = 0): m_iXMLElem(Ptr){}
	XMLElement(const XMLElement& Rhs): m_iXMLElem(Rhs.m_iXMLElem){}
	XMLElement(const XMLDocument& Tree, const std::tstring& Name);
	XMLElement(const XMLElement& Brother, const std::tstring& Name);
	XMLElement& operator= (const XMLElement& Rhs)
	{
		m_iXMLElem = Rhs.m_iXMLElem;
		return *this;
	}

	class iterator 
	{
		friend class XMLElement;
		CComPtr<IXMLDOMNodeList> m_iXMLList;
		int						 m_Position;
		iterator (CComPtr<IXMLDOMNodeList> List, int Pos): m_iXMLList( List ), m_Position(Pos){}
	public:
		iterator (  const iterator& itr) : m_iXMLList( itr.m_iXMLList ), m_Position(itr.m_Position){}
		iterator (): m_Position(0){}

		bool	operator == (  const iterator& itr ) const;
		bool	operator != (  const iterator& itr ); //const;

		iterator& operator = ( const iterator& itr) ;
		XMLElement operator * ()  ;
		XMLElement operator -> ()  ;

		iterator operator ++ (int);
		iterator& operator ++ ();
	};

	// like stl
	iterator begin();
	iterator end();

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
		HRESULT hr =  m_iXMLElem->getAttribute(bstrName, &VarGeneric);
		HrCheck<XMLElementException>(hr, "No such attribute");
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
//		VERIFY(var.vt == (VT_ARRAY| Type2Var<ElementType>::Type));

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

struct XMLProcInstr
{
	CComPtr<IXMLDOMProcessingInstruction> m_pNative;
	XMLProcInstr(	const XMLElement& Elem, const std::tstring & Target, const std::tstring &Data );

//	XMLProcInstr&	operator <<( const XMLAttribute& Attrib );

};

class XMLDocument
{
	friend XMLElement;
	friend XMLText;
	friend XMLComment;
	
	struct ComInitializer
	{
		HRESULT m_hr;
		ComInitializer() { m_hr = CoInitialize(0);}
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

	void		SaveToBinary( std::vector<unsigned char>& Data) const;
	//void		SaveToString( std::wstring& sData ) const;
	//void		Save( std::vector<unsigned char>& Data) const;

	XMLElement	GetRoot() const;
	void		SetRoot(const XMLElement& ElRoot);
	
	XMLDocument&	operator <<( const XMLComment& Comment );
	XMLDocument&	operator <<( const XMLProcInstr& Instr );
};
	
};

#endif // !defined(AFX_XMLLITE_H__76532DCF_4388_457D_8F31_1D36F03DC64A__INCLUDED_)
