// rsa_sui.cpp
// Portable stdio-based user interface
// By Tom Weatherhead			September 18, 1994

#include <stdlib.h>
#include <stdio.h>
#include "rsa.h"


// Corresponds to file dialog

int GetFilePath( char * prompt, char * buf, int dummy )
{
    puts( prompt );

    while( scanf( "%s", buf ) != 1 );

    return( 0 );
}


// Corresponding to dialog box

int GetString( char * prompt, char * buf, int dummy )
{
    return( GetFilePath( prompt, buf, dummy ) );
}


// Corresponds to Message box

void PrintMsg( char * msg )
{
    puts( msg );
    puts( "Press ENTER: " );
    fflush( stdin );
    getchar();
}


int GetYesNo( char * msg )
{
//    char cmd[5];

    puts( msg );

//    while( scanf( "%s", cmd ) != 1 );

    return( getchar() == 'y' );
}


void main( void )
{
    int s;

    srand( time( NULL ) );
    printf( "RSA Encryption\n" );

    for( ; ; ) {
	printf( "\nMenu:\n" );
	printf( "0: Quit\n" );
	printf( "1: Generate a pair of keys\n" );
	printf( "2: Encrypt a file\n" );
	printf( "3: Decrypt a file\n" );
	printf( "4: uuencode a file\n" );
	printf( "5: uudecode a file\n" );

	do {
	    printf( "Selection: " );
	} while( scanf( "%d", &s ) != 1 );

	switch( s ) {
	    case 0:                             // quit
		printf( "\nExiting...\n" );
		exit( 0 );

	    case 1:                             // Create keys
		create_keys();
		break;

	    case 2:                                     // Encrypt
		apply_key( ENCRYPT );
		break;

	    case 3:                                     // Decrypt
		apply_key( DECRYPT );
		break;

	    case 4:
		textencode();
		break;

	    case 5:
		textdecode();
		break;

	    default:
		printf( "Unrecognized command\n\n" );
		break;
	} // switch

//        PrintMsg( "" );
    } // for
} // main()
