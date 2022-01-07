//===== Copyright © 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose:
//
// $NoKeywords: $
//===========================================================================//

//#include "convar.h"
//#include "tier0/dbg.h"
#include "stringpool.h"
#include "tier1/strtools.h"
#include "generichash.h"

// memdbgon must be the last include file in a .cpp file!!!


//-----------------------------------------------------------------------------
// Purpose: Comparison function for string sorted associative data structures
//-----------------------------------------------------------------------------

bool StrLessInsensitive( const char * const &pszLeft, const char * const &pszRight )
{
	throw;
	//return ( stricmp( pszLeft, pszRight) < 0 );
}

bool StrLessSensitive( const char * const &pszLeft, const char * const &pszRight )
{
	return ( strcmp( pszLeft, pszRight) < 0 );
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

CStringPool::CStringPool( StringPoolCase_t caseSensitivity )
  : m_Strings( 32, 256, caseSensitivity == StringPoolCaseInsensitive ? StrLessInsensitive : StrLessSensitive )
{
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

CStringPool::~CStringPool()
{
	FreeAll();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

unsigned int CStringPool::Count() const
{
	return m_Strings.Count();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
const char * CStringPool::Find( const char *pszValue )
{
	unsigned short i = m_Strings.Find(pszValue);
	if ( m_Strings.IsValidIndex(i) )
		return m_Strings[i];

	return NULL;
}

const char * CStringPool::Allocate( const char *pszValue )
{
	char	*pszNew;

	unsigned short i 	= m_Strings.Find(pszValue);
	bool		   bNew = (i == m_Strings.InvalidIndex());

	if ( !bNew )
		return m_Strings[i];

	pszNew = strdup( pszValue );
	m_Strings.Insert( pszNew );

	return pszNew;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

void CStringPool::FreeAll()
{
	unsigned short i = m_Strings.FirstInorder();
	while ( i != m_Strings.InvalidIndex() )
	{
		free( (void *)m_Strings[i] );
		i = m_Strings.NextInorder(i);
	}
	m_Strings.RemoveAll();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------


