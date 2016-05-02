// textcode.cpp - uuencode, uudecode            July 4, 1994
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


#define PATH_SEP                '\\'
#define UUENCODE_LEN            45

// Here, uuencode(0) == '`'.
// Some encoders render uuencode(0) as ' '.
// The decoder here accepts either.
#define uuencode(xxx)           (xxx==0)?(int)'`':xxx+32
#define uudecode(xxx)           (xxx=='`')?0:xxx-32;


BOOL TMainWindow::UUEncode( void )
{
    static char line[UUENCODE_LEN], FileExt[10];
    char outbuf[5], * ptr, * DotPtr, * BaseFileNameStart, * TitleFileName;
    int bytes_read, bytes_writ, bits, linenum, LineInFile = 1, FileNum = 1,
      BaseFileNameLen;
    //FILE * in_stream;

    // Populated members: SrcFileName, SrcStrm, DestFileName,
    //   LinesPerFile
    DestStrm = NULL;

//    strcpy( BaseOutFileName, out_filename );
    // Truncate the extension

    TitleFileName = strrchr( SrcFileName, '\\' );
    TitleFileName = (TitleFileName == NULL) ? SrcFileName : TitleFileName + 1;

    DotPtr = strrchr( DestFileName, '.' );

    if( DotPtr == NULL ) {
	DotPtr = DestFileName + strlen( DestFileName );
    } // if

    BaseFileNameStart = strrchr( DestFileName, '\\' );
    BaseFileNameStart = (BaseFileNameStart == NULL) ? DestFileName
      : BaseFileNameStart + 1;
    BaseFileNameLen = (int)(DotPtr - BaseFileNameStart);

    //in_stream = fopen( SrcFileName, "rb" );
    DestStrm = fopen( DestFileName, "w" );

//    printf( "Working...\n" );
    fprintf( DestStrm, "begin 666 %s\n", TitleFileName );
    outbuf[4] = '\0';

    for( linenum = 1; ; linenum++ ) { // read in 45-byte chunks

	if( DestStrm == (FILE *)NULL ) {
//	    strcpy( out_filename, BaseOutFileName );
	    sprintf( FileExt, "%d.src", FileNum );

	    if( BaseFileNameLen + strlen( FileExt ) > 12 ) {
		strcpy( BaseFileNameStart + 12 - strlen( FileExt ), FileExt );
	    } else {
		strcpy( BaseFileNameStart + BaseFileNameLen, FileExt );
            } // if

	    // PrintMsg( RSAMB_OK, out_filename );

	    if( ( DestStrm = fopen( DestFileName, "w" ) ) == NULL ) {
		// PrintMsg( RSAMB_OK, "Can't open dest file" );
		MessageBox( HWindow, "Can't open dest file", "Error",
		  MB_OK | MB_ICONINFORMATION );
		return( FALSE );
	    } // if
	} // if

	memset( line, 0, sizeof(line) );
	bytes_read = fread( line, 1, sizeof(line), SrcStrm );

	if( bytes_read == 0 ) break;

	// print length character
	fputc( uuencode(bytes_read), DestStrm );
	bytes_writ = 0;

	for( ptr = line; bytes_read > 0; bytes_read -= 3,
	  ptr += 3 ) {
	    // chars at or beyond ptr[bytes_read] are 0
	    bits = (ptr[0]>>2)&0x3f;
	    outbuf[0] = uuencode(bits);
	    bits = ((ptr[0]<<4)&0x30) | ((ptr[1]>>4)&0x0f);
	    outbuf[1] = uuencode(bits);
	    bits = ((ptr[1]<<2)&0x3c) | ((ptr[2]>>6)&0x03);
	    outbuf[2] = uuencode(bits);
	    bits = ptr[2]&0x3f;
	    outbuf[3] = uuencode(bits);
	    fprintf( DestStrm, "%s", outbuf );
	    bytes_writ += 4;
	} // for

	fputc( '\n', DestStrm );

	if( ++LineInFile >= LinesPerFile ) {
	    fclose( DestStrm );
	    DestStrm = (FILE *)NULL;
	    LineInFile = 1;
            FileNum++;
        } // if
    } // for

    fprintf( DestStrm, "%c\nend\n", uuencode(0) );
    // Reckless use of "line" here
    sprintf( line, "%d uuencoded lines written", linenum-1 );
    // PrintMsg( RSAMB_OK, line );
    MessageBox( HWindow, line, "Success", MB_OK | MB_ICONINFORMATION );
    return( TRUE );
} // TMainWindow::UUEncode()


BOOL TMainWindow::UUDecode( void )
{
    int reading = 0;    // FALSE
    // filename, out_path not used concurrently; 
    // could be merged if semantically acceptable
    static char line[80], msg[60];
    char * inptr, outbuf[4], * chp;
    int i, num_to_write, bits[4], prot_mask, linenum;

    // Populated member: SrcStrm
    DestStrm = NULL;

    // Put output file in same dir as input file
    strcpy( DestFileName, SrcFileName );
    chp = strrchr( DestFileName, PATH_SEP );

    if( chp == NULL ) {
	chp = strchr( DestFileName, ':' );
    }

    if( chp != NULL ) {
	chp++;
    } else {
	chp = DestFileName;
    }

//    printf( "Working...\n" );

    for( linenum = 1; !feof( SrcStrm ); linenum++ ) {
	memset( line, '\0', sizeof( line ) );
	fgets( line, sizeof( line ), SrcStrm );

	if( !reading ) {

	    if( memcmp( line, "begin", 5 ) ) continue;

	    reading = 1;        // TRUE

	    if( DestStrm != NULL ) continue; // already open

	    sscanf( line + 6, "%d %s", &prot_mask, chp );

	    //sprintf( line, "Output pathname: %s\n", out_path );
	    //PrintMsg( RSAMB_OK, line );

	    DestStrm = fopen( DestFileName, "wb" );

	    if( DestStrm == NULL ) {
		//PrintMsg( RSAMB_OK,
		//  "Error opening output file\n" );
		// *ErrMsg = "Error opening output file";
		MessageBox( HWindow, "Can't open output file", "Error",
		  MB_OK | MB_ICONINFORMATION );
		return( FALSE );
	    }

	    continue;
	} else if( !memcmp( line, "end", 3 ) ) {
	    reading = 0;        // FALSE
	    continue;
	}
#if 0
	if( strlen( line ) > 62 ) {
	    sprintf( line, "Error: line %d too long\n", linenum );
	    PrintMsg( RSAMB_OK, line );
	    return( FALSE );
	}
#endif
	num_to_write = uudecode(line[0]);

	for( inptr = line + 1; num_to_write > 0;
	  inptr += 4, num_to_write -= 3 ) {

	    for( i = 0; i < 4; i++ ) {

		if( inptr[i] < ' '  ||  inptr[i] > '`' ) {
		    // Reckless use of "filename"
		    sprintf( msg,
		      "Illegal char %02X in line %d. Continue?",
		      (int)(inptr[i]), linenum );

		    //if(!PrintMsg(RSAMB_YESNO, "Continue? ")) {
		    if( MessageBox( HWindow, msg, "Error",
		      MB_YESNO | MB_ICONQUESTION ) != IDYES ) {
			return( FALSE );
		    }
		}

		bits[i] = uudecode(inptr[i]);
	    }

	    outbuf[0] = (bits[0]<<2) | ((bits[1]&0x30)>>4);
	    outbuf[1] = ((bits[1]&0x0f)<<4) 
	      | ((bits[2]&0x3c)>>2);
	    outbuf[2] = ((bits[2]&0x03)<<6) | (bits[3]&0x3f);
	    fwrite( outbuf, 1, 
	      (num_to_write > 3) ? 3 : num_to_write,
	      DestStrm );
	} // for
    } // while

    sprintf( msg, "%d lines UUdecoded", linenum - 1 );
    MessageBox( HWindow, msg, "Success", MB_OK | MB_ICONINFORMATION );
    return( TRUE );
} // TMainWindow::UUDecode()


// ******** End of textcode.cpp ********
