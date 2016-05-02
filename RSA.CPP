// rsa.cpp - Implementation of the RSA Encryption Scheme
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
#include <filedial.h>
#include "rsa.h"
#include "rsadispa.h"
#include "rsa_gui.h"


#if 1	// Inline it
#define copy_bignum(src,dst) { \
  memcpy( (dst)->buf, (src)->buf, (src)->len ); \
  (dst)->len = (src)->len; }
#else
static void copy_bignum( bignum_class * src,
  bignum_class * dst )
{
    memcpy( dst->buf, src->buf, src->len );
    dst->len = src->len;
} // copy_bignum
#endif


void int_to_bignum( unsigned int in, bignum_class * out )
{
    unsigned int i;

    for( i = 0; in > 0; i++ ) {
	out->buf[i] = (unsigned char)(in & 0xff);
	in >>= 8;
    }

    out->len = i;
} // int_to_bignum


// **** Arithmetic operations on bignums ****


// Effect: out := in1 + in2 * 2 ^ Shift
// Change to ShiftedAddTo: in1 += in2 * 2 ^ Shift

void ShiftedAdd( bignum_class * in1, bignum_class * in2,
  bignum_class * out, int Shift )
{
    static bignum_class temp_out;
    static unsigned int masks[] = { 0xff, 0xfe, 0xfc, 0xf8,
      0xf0, 0xe0, 0xc0, 0x80 };
    int Idx2 = 0, Idx1 = Shift / 8; // Which byte to start with
    unsigned char * in2ptr = in2->buf, * OutPtr = temp_out.buf + Idx1;
    unsigned int carry = 0, Len1 = in1->len, Len2 = in2->len,
      ByteShiftUp = Shift & 0x7, OutBefore, OutAfter, mask1, mask2,
      PrevByte = 0, ComposedByte = 0, Byte2;

//    assert( ByteShiftUp >= 0  &&  ByteShiftUp < 8 );
    mask1 = masks[ByteShiftUp];
    mask2 = (~mask1) & 0xff;
    memset( temp_out.buf, 0, Idx1 + Len2 + 2 );
    copy_bignum( in1, &temp_out ); // in1 may not be the longer arg

    for( ; ; in2ptr++, OutPtr++, Idx1++, Idx2++ ) {

	if( Idx2 >= Len2 ) {

	    if( carry == 0  &&  PrevByte == 0 ) break;

	    Byte2 = ComposedByte = 0;
        } else {
	    Byte2 = (unsigned int)*in2ptr;
	    ComposedByte = ( ByteShiftUp == 0 ) ? Byte2
	      : (Byte2 << ByteShiftUp) & mask1;
	} // if

	ComposedByte |= PrevByte;
	OutBefore = (unsigned int)*OutPtr;
	OutAfter = OutBefore + ComposedByte + carry;
	*OutPtr = (unsigned char)(OutAfter & 0xff);
	PrevByte = ( ByteShiftUp == 0 ) ? 0
	  : (Byte2 >> (8 - ByteShiftUp)) & mask2;
	carry = ( OutAfter > 0xff ) ? 1 : 0;
    } // for

    if( Idx1 > Len1 ) {
	temp_out.len = Idx1;
    } // if

    copy_bignum( &temp_out, out );
} // ShiftedAdd()


// Returns FALSE if subtraction result is negative; TRUE otherwise.

bool ShiftedSubtract( bignum_class * in1, bignum_class * in2,
  bignum_class * out, int Shift )
{
    static bignum_class temp_out;
    static unsigned int masks[] = { 0xff, 0xfe, 0xfc, 0xf8,
      0xf0, 0xe0, 0xc0, 0x80 };
    int Idx2 = 0, Idx1 = Shift / 8; // Which byte to start with
    unsigned char * in2ptr = in2->buf, * OutPtr = temp_out.buf + Idx1;
    unsigned int carry = 0, Len1 = in1->len, Len2 = in2->len,
      ByteShiftUp = Shift & 0x7, OutBefore, OutAfter, mask1, mask2,
      PrevByte = 0, ComposedByte = 0, Byte2;

//    assert( ByteShiftUp >= 0  &&  ByteShiftUp < 8 );
    mask1 = masks[ByteShiftUp];
    mask2 = (~mask1) & 0xff;
    memset( temp_out.buf, 0, Idx1 + Len2 + 2 );
    copy_bignum( in1, &temp_out ); // in1 may not be the longer arg

    for( ; ; in2ptr++, OutPtr++, Idx1++, Idx2++ ) {

	if( Idx2 >= Len2 ) {

	    if( carry == 0  &&  PrevByte == 0 ) break;

	    Byte2 = ComposedByte = 0;
        } else {
	    Byte2 = (unsigned int)*in2ptr;
	    ComposedByte = ( ByteShiftUp == 0 ) ? Byte2
	      : (Byte2 << ByteShiftUp) & mask1;
	} // if

	// Subtraction: test for negative result

	if( Idx1 >= Len1 ) return( FALSE );

	ComposedByte |= PrevByte;
	OutBefore = (unsigned int)*OutPtr;
	OutAfter = OutBefore - ComposedByte - carry;
	*OutPtr = (unsigned char)(OutAfter & 0xff);
	PrevByte = ( ByteShiftUp == 0 ) ? 0
	  : (Byte2 >> (8 - ByteShiftUp)) & mask2;
	carry = ( OutAfter > 0xff ) ? 1 : 0;
    } // for

    temp_out.trim();
    copy_bignum( &temp_out, out );
    return( TRUE );
} // ShiftedSubtract()


static void add_bignum( bignum_class * in1, bignum_class * in2,
  bignum_class * out )
{
    unsigned char * in1ptr, * in2ptr;
    register unsigned short i, carry = 0, sum;
    unsigned short in1len, in2len;
    static bignum_class temp_out;
    unsigned char * temp_out_ptr;
    bignum_class * temp_ptr;

    if( in1->len < in2->len ) {
	temp_ptr = in1;
	in1 = in2;
	in2 = temp_ptr;
    }

    in1ptr = in1->buf;
    in2ptr = in2->buf;
    in1len = in1->len;
    in2len = in2->len;

    copy_bignum( in1, &temp_out ); // in1 is the longer arg
    temp_out_ptr = temp_out.buf;

    for( i = 0; i < in1len; i++ ) {
	// assert( carry <= 1 );
	sum = *in1ptr + carry;
	in1ptr++;

	if( i < in2len ) {
	    sum += *in2ptr;
	} // if

	*temp_out_ptr = (unsigned char)(sum & 0xff);
	temp_out_ptr++;
	carry = sum >> 8;

	if( i >= in2len  &&  carry == 0 ) break;

	in2ptr++;
    } // for

    if( carry > 0 ) {
//	temp_out.buf[i] = (unsigned char)carry;
	*temp_out_ptr = (unsigned char)carry;
	temp_out.len = i + 1;  // ie. ++
    }

    copy_bignum( &temp_out, out );
} // add_bignum()


// Assume that numbers are trimmed

bool bignums_equal( bignum_class * n1, bignum_class * n2 )
{
    return( ( n1->len != n2->len  ||  memcmp( n1->buf, n2->buf, n1->len ) )
      ? FALSE : TRUE );
} // bignums_equal


void mult_bignum( bignum_class * in1,
  bignum_class * in2, bignum_class * out )
{
    unsigned char mask, * in1ptr, in1ch;
    int i, j, Shift;
    unsigned short in1len = in1->len; //, out_len = in1len + in2->len;
    static bignum_class temp_out;

    temp_out.len = 0;   // ie. temp_out := 0
    // So buf != NULL and is 0'd and right size:
    memset( temp_out.buf, 0, in1len + in2->len + 2 );
    Shift = 0;
    in1ptr = in1->buf;

    for( i = 0; i < in1len; i++ ) {
	in1ch = *in1ptr;
	in1ptr++;
	mask = 0x1;

	for( j = 0; j < 8; j++ ) {

	    if( in1ch & mask ) {
		ShiftedAdd( &temp_out, in2, &temp_out, Shift );
	    } // if

	    Shift++;
	    mask <<= 1;
	} // for
    } // for

    copy_bignum( &temp_out, out );
} // mult_bignum()


// If quot == NULL, we just want the remainder (ie. modulus)

static void div_bignum( bignum_class * in1, bignum_class * in2,
  bignum_class * quot, bignum_class * rem )
{
    unsigned char mask, * temp_quot_ptr;
    int i, j, len_diff = in1->len - in2->len, Shift;
    static bignum_class temp_quot, temp_rem;

    if( len_diff < 0 ) {

	if( quot != NULL ) {
//	    int_to_bignum( 0, quot );
	    quot->len = 0;
	} // if

	copy_bignum( in1, rem );
	return;
    } // if

    memset( temp_quot.buf, 0, len_diff + 1 );
    temp_quot.len = len_diff + 1;

    copy_bignum( in1, &temp_rem );

    Shift = 8 * (len_diff + 1) - 1;
    temp_quot_ptr = &temp_quot.buf[len_diff];

    for( i = len_diff; i >= 0; i-- ) {
	mask = 0x80;

	for( j = 0; j < 8; j++ ) {
	    if( ShiftedSubtract( &temp_rem, in2, &temp_rem, Shift ) ) {
		*temp_quot_ptr |= mask;
	    } // if

            Shift--;
	    mask >>= 1;
	} // for

	temp_quot_ptr--;
    } // for

    if( quot != NULL ) {
	copy_bignum( &temp_quot, quot );
	quot->trim();
    }

    copy_bignum( &temp_rem, rem );
} // div_bignum


static bool inverse_under_mod( bignum_class * in,
  bignum_class * n, bignum_class * out )
{
    unsigned int i, lim;
    static bignum_class num1, rem, temp_out;

    // Can overcome the 4-byte barrier by making i a bignum

    if( in->len > 4 ) {
//	printf( "inverse: element is too big\n" );
	return( FALSE );
    }

    lim = in->to_int();
    int_to_bignum( 1, &num1 );
    temp_out.len = 0;

    for( i = 1; i < lim; i++ ) {
#if 0
	ShiftedAdd( &num1, n, &num1, 0 );

	if( !ShiftedSubtract( &num1, in, &num1, 0 ) ) continue;

	temp_out.inc();	// Maintain the quotient

	if( num1.len == 0 ) {
#else
	add_bignum( &num1, n, &num1 ); // ie. num1 = n*i + 1
	div_bignum( &num1, in, &temp_out, &rem );

	if( rem.len == 0 ) {
#endif
//	    printf( "inverse: an inverse has been found\n" );
	    copy_bignum( &temp_out, out );
	    return( TRUE );
	} // if
    } // for

//    printf( "inverse: no inverse found\n" );
    return( FALSE );
} // inverse_under_mod()


// Page 829;  d := (a^b) mod n

static void modular_exp( bignum_class * a, bignum_class * b,
  bignum_class * n, bignum_class * d )
{
    unsigned char mask, * b_ptr;
    int i, j;
    static bignum_class temp_d;

    int_to_bignum( 1, &temp_d );
    b_ptr = &b->buf[b->len - 1];

    for( i = b->len - 1; i >= 0; i-- ) {
	mask = 0x80;

	for( j = 0; j < NUM_BITS_IN_BYTE; j++ ) {
	    mult_bignum( &temp_d, &temp_d, &temp_d );
	    div_bignum( &temp_d, n, NULL, &temp_d );

	    if( *b_ptr & mask ) {
		mult_bignum( &temp_d, a, &temp_d );
		div_bignum( &temp_d, n, NULL, &temp_d );
	    } // if

	    mask >>= 1;
	} // for

	b_ptr--;
    } // for

    copy_bignum( &temp_d, d );
} // modular_exp()


// p. 840 of CS466 text

static bool witness( bignum_class * a, bignum_class * n )
{
    unsigned char mask, * b_ptr;
    int i, j;
    static bignum_class b, d, x, one;

    int_to_bignum( 1, &one );
    copy_bignum( n, &b );
    b.dec();                    // b := n-1
    b.trim();
    int_to_bignum( 1, &d );
    b_ptr = &b.buf[b.len - 1];

    for( i = b.len - 1; i >= 0; i-- ) {
	mask = 0x80;

	for( j = 0; j < NUM_BITS_IN_BYTE; j++ ) {
	    copy_bignum( &d, &x );
	    mult_bignum( &d, &d, &d );
	    div_bignum( &d, n, NULL, &d );

	    if( bignums_equal( &d, &one )
	      &&  !bignums_equal( &x, &one )
	      &&  !bignums_equal( &x, &b ) )
		return( TRUE );

	    if( *b_ptr & mask ) {
		mult_bignum( &d, a, &d );
		div_bignum( &d, n, NULL, &d );
	    } // if

	    mask >>= 1;
	} // for

	b_ptr--;
    } // for

    return( !bignums_equal( &d, &one ) ? TRUE : FALSE );
} // witness()


// p. 841: Is n prime?

static bool Miller_Rabin( bignum_class * n,
  unsigned int num_wit )
{
    unsigned int j;
    static bignum_class wit;

    srand( (int)time( (time_t *)NULL ) );

    for( j = 0; j < num_wit; j++ ) {

	do {
	    wit.random( n->len );
	    div_bignum( &wit, n, NULL, &wit ); // so wit < n
	} while( wit.len == 0 );

//	printf( "    Witness %d\n", j );

	if( witness( &wit, n ) ) return( FALSE );
    } // for

    // TRUE => Prob(n is composite) < 2^-num_wit
    return( TRUE );
} // Miller_Rabin()


static void random_bigprime( bignum_class * num,
  unsigned int len )
{
    unsigned int i;

    for( i = 0; ; i++ ) {
	do {
	    num->random( len );
	} while( num->len < 2 && !(num->buf[0] & 0xfe) );

	num->buf[0] |= 0x1;             // Ensure odd
//	printf( "Attempt %d\n", i );

	if( Miller_Rabin( num, NUM_WITNESSES ) ) break;
    } // for

//    printf( "Probable prime: " );
//    num->print();
} // random_bigprime


BOOL TMainWindow::CreateKeys( FILE * PubStrm, FILE * PrivStrm )
{
    bignum_class p, q, n, phi_n, d, e;

    random_bigprime( &p, KeySize / 2 );
    random_bigprime( &q, KeySize / 2 );
    mult_bignum( &p, &q, &n );
    p.dec();
    q.dec();
    mult_bignum( &p, &q, &phi_n );

    do {
	random_bigprime( &d, 1 ); // Get small odd prime
    } while( !inverse_under_mod( &d, &phi_n, &e ) );

    // Test keys
    // New meanings for p and q
    // Theorem: (q^(d*e)) mod n == q for all 0 <= q < n
    // ie. (q^(d*e-1)) mod n == 1 for all 0 < q < n
    mult_bignum( &d, &e, &p );
    p.dec();

//    printf( "Testing integrity of keys...\n" );

    do {

	do {
	    q.random( n.len );
	    div_bignum( &q, &n, NULL, &q ); // So q < n
	} while( q.len == 0 ); // Don't want q == 0

	modular_exp( &q, &p, &n, &q );
//	printf( "Test output (0x01): " );
//	q.print();

	if( q.len != 1  ||  q.buf[0] != 0x1 ) {
	    // q != 1 => error
	    //PrintMsg( RSAMB_OK, "Keys failed test\n" );
	    MessageBox( HWindow, "Keys failed test", "Error",
	      MB_OK | MB_ICONINFORMATION );
	    return( FALSE );
	}

    //} while( PrintMsg( RSAMB_YESNO, "Keys passed test.  Again? " ) );
    } while( MessageBox( HWindow, "Keys passed test.  Again?", "Success",
      MB_YESNO | MB_ICONQUESTION ) == IDYES );

    // Save keys
#if 0
    if( (fp = fopen( PubFile, "w" )) == NULL ) {
	//PrintMsg( RSAMB_OK, "Can't write public key file" );
	MessageBox( HWindow, "Can't write public key file", "Error",
	  MB_OK | MB_ICONINFORMATION );
	return( FALSE );
    } // if
#endif
    d.filewrite( PubStrm );
    n.filewrite( PubStrm );
#if 0
    fclose( fp );

    if( (fp = fopen( PrivFile, "w" )) == NULL ) {
	//PrintMsg( RSAMB_OK, "Can't write private key file" );
	MessageBox( HWindow, "Can't write private key file", "Error",
	  MB_OK | MB_ICONINFORMATION );
	return( FALSE );
    } // if
#endif
    e.filewrite( PrivStrm );
    n.filewrite( PrivStrm );
    // fclose( fp );

    //PrintMsg( RSAMB_OK, "Keys have been created." );
    MessageBox( HWindow, "Keys have been created", "Success",
      MB_OK | MB_ICONINFORMATION );
    return( TRUE );
} // TMainWindow::CreateKeys()


BOOL TMainWindow::ApplyKey( BOOL Encrypt )
{
    char packet_size, MsgStr[40];
    BOOL Cancelled;
    int bytes_read, in_size, out_size, NumSrcBytes = 0;
    time_t StartTime = time( NULL );
    bignum_class de, n,    // de is d or e, depending on op
      in_num, out_num;
    TProgMonWin * ProgressMonitior = new TProgMonWin( this, "Progress Monitor",
      &Cancelled );

    if( !de.fileread( KeyStrm )  ||  !n.fileread( KeyStrm ) ) {
	//PrintMsg( RSAMB_OK, "The key is corrupt" );
	MessageBox( HWindow, "The key is corrupt", "Error",
	  MB_OK | MB_ICONINFORMATION );
	delete ProgressMonitior;
	return( FALSE );
    } // if

    if( GetApplication()->MakeWindow( ProgressMonitior ) == NULL ) {
	//PrintMsg( RSAMB_OK, "Progress monitor failed" );
	MessageBox( HWindow, "Progress monitor failed", "Error",
	  MB_OK | MB_ICONINFORMATION );
	delete ProgressMonitior;
	return( FALSE );
    } // if

//    printf( "n has %d bytes\n", n.len );
    in_size = out_size = n.len;

    if( Encrypt ) {
	in_size--; // so in_num <= n, out_buf can hold n
    } else {
	out_size--; // so in_buf can hold n, out_num <= n
    } // if

    for( ; ; ) {

	if( !Encrypt ) {
	    // Read in length of packet to follow
	    fread( &packet_size, 1, 1, SrcStrm );
	} // if

	// Read max_in_size objects, each 1 byte long
	bytes_read = fread( in_num.buf, 1, in_size, SrcStrm );
	NumSrcBytes += bytes_read;

	if( Encrypt ) {
	    packet_size = (char)bytes_read;
	    fwrite( &packet_size, 1, 1, DestStrm );
	} // if

//	printf( "%d bytes read\n", bytes_read );

	if( bytes_read == 0 ) break;

	in_num.len = bytes_read;
//	printf( "bignum in: " );
//	in_num.print();
	in_num.trim();

	// Apply operation
	modular_exp( &in_num, &de, &n, &out_num );

//	printf( "bignum out: " );
//	out_num.print();
	out_num.zero_pad( out_size );
	fwrite( out_num.buf, 1,
	  Encrypt ? out_size : (int)packet_size, DestStrm );

        sprintf( MsgStr, "%d bytes processed", NumSrcBytes );
	ProgressMonitior->UpdateText( MsgStr );

	if( bytes_read < in_size ) break;
    } // for

    if( !feof( SrcStrm ) ) {
	//PrintMsg( RSAMB_OK,
	//  "Warning: Input file EOF reached\n" );
	MessageBox( HWindow, "Input file EOF reached", "Warning",
	  MB_OK | MB_ICONINFORMATION );
    } // if

    sprintf( MsgStr, "Key applied in %ld seconds",
      (long)( time( NULL ) - StartTime ) );
    // PrintMsg( RSAMB_OK, "Key applied successfully" );
    //PrintMsg( RSAMB_OK, MsgStr );
    delete ProgressMonitior;
    MessageBox( HWindow, MsgStr, "Information", MB_OK | MB_ICONINFORMATION );
    return( TRUE );
} // TMainWindow::ApplyKey()


// ******** End of rsa.cpp ********
