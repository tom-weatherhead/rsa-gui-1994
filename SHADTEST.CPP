#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
//#include <assert.h>
#include "rsa.h"


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


void int_to_bignum( unsigned int in, bignum_class * out );
bool ShiftedAdd( bignum_class * in1, bignum_class * in2,
  bignum_class * out, int Shift, bool );
void mult_bignum( bignum_class *, bignum_class *, bignum_class * );
compare_type cmp_bignum( bignum_class *, bignum_class * );
void sub_bignum( bignum_class * in1, bignum_class * in2, bignum_class * out );


int PrintMsg( int i, char * msg )
{
    printf( "%d: %s\n", i, msg );
    return( 1 );
}


static void op1( void )
{
    unsigned short len1, len2;
    int i, num1, num2;
    bignum_class big1, big2, out, out2, old1, old2;

    srand( time( NULL ) );
    printf( "Num1 len: " );
    scanf( "%hd", &len1 );
    printf( "Num2 len: " );
    scanf( "%hd", &len2 );

    for( i = 0; i < 100; i++ ) {
	big1.random( len1 );
	big2.random( len2 );
	printf( "\n" );
        big1.print();
	big2.print();
        copy_bignum( &big1, &old1 );
	copy_bignum( &big2, &old2 );
#if 1
	mult_bignum( &big1, &big2, &out );
	printf( "%2da: ok\n", i );
	mult_bignum( &big1, &big1, &big1 );
	printf( "%2db: ok\n", i );
#else
	mult_bignum1( &big1, &big1, &out );
	printf( "Correct: " );
        out.print();
	mult_bignum2( &big1, &big1, &out2 );
	printf( "New:     " );
	out2.print();

	if( cmp_bignum( &out, &out2 ) == CT_EQ
	  &&  cmp_bignum( &big1, &old1 ) == CT_EQ
	  &&  cmp_bignum( &big2, &old2 ) == CT_EQ ) continue;

	printf( "Error\n" );
	printf( "in1: " );
        big1.print();
	printf( "old1: " );
        old1.print();
	printf( "in2: " );
        big2.print();
	printf( "old2: " );
        old2.print();
	fflush( stdin );

	if( getchar() == 'q' ) break;
#endif
    } // for

    printf( "\nDone!\n" );
} // op1


static void op2( void )
{
    int shift, num1, num2;
    bignum_class big1, big2, out;

    printf( "Enter 'd' to quit\n" );

    for( ; ; ) {
    	printf( "\nNum1:  " );
    	scanf( "%x", &num1 );

    	if( num1 == 0xd ) break;;

    	printf( "Num2:  " );
    	scanf( "%x", &num2 );
    	printf( "Shift:  " );
    	scanf( "%d", &shift );
    	int_to_bignum( num1, &big1 );
	int_to_bignum( num2, &big2 );
	printf( "Result: " );

	if( ShiftedAdd( &big1, &big2, &out, shift, FALSE ) ) {
	    out.print();
	} else {
            printf( "Negative\n" );
        } // if
    } // for
} // op2


static void op3( void )
{
    unsigned short len1, len2;
    int i;
    bignum_class big1, big2, out, out2;

    srand( time( NULL ) );
    printf( "Num1 len: " );
    scanf( "%hd", &len1 );
    printf( "Num2 len: " );
    scanf( "%hd", &len2 );

    for( i = 0; i < 100; i++ ) {
	big1.random( len1 );
	big2.random( len2 );
	printf( "\n" );
        big1.print();
	big2.print();
	sub_bignum( &big1, &big2, &out );
	printf( "Correct: " );
        out.print();
	ShiftedAdd( &big1, &big2, &out2, 0, FALSE );
	printf( "New:     " );
	out2.print();

	if( cmp_bignum( &out, &out2 ) == CT_EQ ) continue;

	printf( "Error\n" );
	fflush( stdin );

	if( getchar() == 'q' ) break;
    } // for

    printf( "\nDone!\n" );
} // op3


void main( void )
{
    printf( "op (1-3): " );
    fflush( stdin );

    switch( getchar() ) {
	case '1':	op1();		break;
	case '2':	op2();		break;
	case '3':	op3();		break;
	default:			break;
    } // switch
} // main()
