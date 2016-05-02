// bignum.cpp                                 July 16, 1994
// (C) Copyright 1994 by Tom Weatherhead                      

#define WIN31

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <assert.h>
#include <dir.h>
#include <owl.h>
#include <dialog.h>
#include <edit.h>
//#include <inputdia.h>
#include <filedial.h>
#include "rsa.h"
#include "rsadispa.h"
#include "rsa_gui.h"


bignum_class::bignum_class( void )
{
    buf = (unsigned char *)malloc( BIGNUM_BUF_SIZE );
    len = 0;
} // bignum_class::bignum_class


unsigned int bignum_class::to_int( void )
{
    unsigned char * ptr = buf;
    unsigned int i, rtn = 0, mult = 1;

#if 1
    assert( len <= sizeof( int ) );
#else
    if( len > 4 ) {
    	PrintMsg( RSAMB_OK, "bignum::to_int : too big\n" );
//	printf("num won't fit in uns. int; len == %d\n",len);
	return( 0 );
    }
#endif

    for( i = 0; i < len; i++ ) {
	rtn += mult * (unsigned int)*ptr;
	ptr++;
	mult <<= 8;
    }

    return( rtn );
} // bignum_class::to_int

#if 0
void bignum_class::print( void )
{
    int i;

    if( len == 0 ) {
	printf( "0\n" );
	return;
    }

    printf( "0x" );

    for( i = len - 1; i >= 0; i-- ) {
	printf( "%02x", buf[i] );
    }

    printf( "\n" );
} // bignum_class::print
#endif


void bignum_class::trim( void )
{
    unsigned int i;

    for( i = len; i > 0; ) {

	if( buf[--i] != (unsigned char)0 ) {
            i++;
	    break;
        } // if
    } // for

    len = i;
} // bignum_class::trim


// Zero-pad to a length of newlen

void bignum_class::zero_pad( unsigned short newlen )
{
    if( newlen <= (int)len ) return; // newlen may be negative

//    resize( newlen );              // Increase size, if needed
    memset( buf + len, 0, newlen - len );
    len = newlen;
} // bignum_class::zero_pad


void bignum_class::inc( void )  // Redundant?
{
    unsigned int i;

    for( i = 0; i < len; i++ ) {

	if( buf[i] == 0xff ) {
	    buf[i] = 0;         // Carry
	} else break;
    }

    if( i == len ) {
	buf[len++] = 1;
    } else {
	buf[i]++;
    }
} // bignum_class::inc


void bignum_class::dec( void )  // Redundant?
{
    unsigned int i;

    for( i = 0; i < len; i++ ) {

	if( buf[i] == 0x0 ) {
	    buf[i] = 0xff;              // Borrow
	} else break;
    }

#if 1
    // Tried to dec 0 if following is true
    assert( i < len );
#else
    if( i == len ) {
	PrintMsg( RSAMB_OK, "bignum::dec : tried to dec 0\n" );
	exit( 1 );
    }
#endif
    buf[i]--;

    if( buf[i] == 0  &&  i == len - 1 ) {
	len--;  // No trimming necessary
    }
} // bignum_class::dec


void bignum_class::random( unsigned short newlen )
{
    unsigned int i;

//    resize( newlen );

    for( i = 0; i < newlen; i++ ) {
	// >> 8 to get random low bits
	buf[i] = (rand() >> 8) & 0xff;
    }

    len = newlen;
    trim();
} // bignum_class::random


bool bignum_class::fileread( FILE * fp )
{
    int rtn;

    rtn = fread( (char *)&len, 1, sizeof( len ), fp );
    assert( rtn == sizeof( len ) );

    if( len <= 0  ||  len >= BIGNUM_BUF_SIZE ) {
	return( FALSE );
    } // if

    assert( buf != NULL );
    rtn = fread( buf, 1, len, fp );
    return( ( rtn == (int)len ) ? TRUE : FALSE );
} // bignum_class::fileread


void bignum_class::filewrite( FILE * fp )
{
    int rtn;

    assert( len > 0 );
    rtn = fwrite( (char *)&len, 1, sizeof( len ), fp );
    assert( rtn == sizeof( len ) );
//    assert( buf_size >= len );
    assert( buf != NULL );
    rtn = fwrite( buf, 1, len, fp );
    assert( rtn == (int)len );
} // bignum_class::filewrite


bignum_class::~bignum_class( void )
{
    if( buf == NULL ) return;

    free( buf );
    len = 0;     // In case the object is reused
} // bignum_class::~bignum_class


// ******** End of bignum.cpp ********
