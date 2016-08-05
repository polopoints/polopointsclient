#include "StdAfx.h"
#include "uiapi.h"

UINT uiapi::MallocP( void * * p , UINT size )
{
	if( 0 == size ) {
		return 0 ;
	} else {
		*p = (BYTE*)malloc( size*sizeof(BYTE) ) ;
		ASSERT( NULL != *p) ;
		if( NULL != p ) {
			memset( *p , 0 , size*sizeof(BYTE) ) ;
		} else {
			return 0 ;
		}
		return _msize( *p ) ;
	}
}
void uiapi::ReleaseP( void * p ) 
{
	if( NULL == p ) {
		return  ;	
	} else {
		free( p ) ;
		p = NULL ;
	}
}