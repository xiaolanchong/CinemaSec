/****************************************************************************
  document_iterator.h
  ---------------------
  begin     : Aug 2004
  author(s) : A.Akhriev
  email     : Albert.Akhriev@biones.com
****************************************************************************/

#pragma once

//=================================================================================================
/** \class DocumentIterator.
    \brief Class iterates throughout documents of specified type.

  1) It is strongly recommended to use this class as a temporal local variable,
     because a document could be closed by user at any moment.
  2) One must protect document list from being modified during iterating,
     because user could close document asynchronously even in a single-thread application. */
//=================================================================================================
template< class DOCTYPE >
class DocumentIterator
{
private:
  CDocTemplate * m_pDocTempl;     //!< pointer to the template of specified document type
  POSITION       m_docPos;        //!< position of the current document in the list

  DocumentIterator();             // no default constructor

public:
  /** \brief Constructor. */
  explicit DocumentIterator( CDocTemplate * pTemplate ) : m_pDocTempl(pTemplate), m_docPos(0)
  {
  }

  /** \brief Function return pointer to the first document of specified type or 0. */
  DOCTYPE * GetFirst()
  {
    m_docPos = 0;
    if (m_pDocTempl != 0)
    {
      m_docPos = m_pDocTempl->GetFirstDocPosition();
      if (m_docPos != 0)
      {
        CDocument * pDoc = m_pDocTempl->GetNextDoc( m_docPos );
        if (pDoc != 0)
        {
          ASSERT( pDoc->IsKindOf( RUNTIME_CLASS( DOCTYPE ) ) );
          return reinterpret_cast<DOCTYPE*>( pDoc );
        }
      }
    }
    return reinterpret_cast<DOCTYPE*>( 0 );
  }

  /** \brief Function return pointer to the next document of specified type or 0. */
  DOCTYPE * GetNext()
  {
    return (((m_pDocTempl != 0) && (m_docPos != 0))
            ? reinterpret_cast<DOCTYPE*>( m_pDocTempl->GetNextDoc( m_docPos ) )
            : reinterpret_cast<DOCTYPE*>( 0 ));
  }
};

