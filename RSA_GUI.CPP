// rsa_gui.cpp
// ObjectWindows-based UI for RSA       September 18, 1994
// (C) Copyright 1994 by Tom Weatherhead			

// ObjectWindows - (C) Copyright 1992 by Borland International

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


// **** TMainWindow Member functions ****

TMainWindow::TMainWindow( PTWindowsObject AParent, LPSTR ATitle )
  : TWindow( AParent, ATitle )
{
    AssignMenu("COMMANDS");
    ButtonDown = FALSE;
} // TMainWindow::TMainWindow()


void TMainWindow::CMFileExit( RTMessage )
{
    CloseWindow();
} // TMainWindow::CMFileExit()


void TMainWindow::CMRSAGenerateKeys( RTMessage )
{
//    GetModule()->ExecDialog( new TGenKeysDlg( this, "GENKEYSDIAL" ) );

    if( GetModule()->ExecDialog( new TGenKeysDlg( this, "GENKEYSDIAL"
      ) ) != IDOK ) return;

    CreateKeys( KeyStrm, KeyStrm2 );
    fclose( KeyStrm );
    fclose( KeyStrm2 );
} // TMainWindow::CMRSAGenerateKeys()


void TMainWindow::CMRSAEncrypt( RTMessage )
{

    if( GetModule()->ExecDialog( new TEncryptDlg( this, "ENCRDIAL",
      TRUE ) ) != IDOK ) return;

    ApplyKey( TRUE );
    fclose( KeyStrm );
    fclose( SrcStrm );
    fclose( DestStrm );
} // TMainWindow::CMRSAEncrypt()

 
void TMainWindow::CMRSADecrypt( RTMessage )
{

    if( GetModule()->ExecDialog( new TEncryptDlg( this, "ENCRDIAL",
      FALSE ) ) != IDOK ) return;

    ApplyKey( FALSE );
    fclose( KeyStrm );
    fclose( SrcStrm );
    fclose( DestStrm );
} // TMainWindow::CMRSADecrypt()


void TMainWindow::CMUUEncode( RTMessage )
{
    if( GetModule()->ExecDialog( new TUUEncodeDlg( this, "UUENCDIAL" ) )
      != IDOK ) return;

    UUEncode();
    fclose( SrcStrm );

    if( DestStrm != NULL ) {
    	fclose( DestStrm );
    } // if
} // TMainWindow::CMUUEncode()

 
void TMainWindow::CMUUDecode( RTMessage )
{
    if( GetModule()->ExecDialog( new TUUDecodeDlg( this, "UUDECDIAL" ) )
      != IDOK ) return;

    UUDecode();
    fclose( SrcStrm );

    if( DestStrm != NULL ) {
    	fclose( DestStrm );
    } // if
} // TMainWindow::CMUUDecode()

 
void TMainWindow::CMAbout( RTMessage )
{
    GetModule()->ExecDialog( new TDialog( this, "ABOUTDIAL" ) );
} // TMainWindow::CMAbout()


TMainWindow::~TMainWindow( void )
{
} // TMainWindow::~TMainWindow()


// **** TRSAApp Member Functions ****


TRSAApp::TRSAApp( LPSTR AName, HINSTANCE hInstance, HINSTANCE hPrevInstance,
  LPSTR lpCmdLine, int nCmdShow )
  : TApplication( AName, hInstance, hPrevInstance,
  lpCmdLine, nCmdShow )
{
} // TRSAApp::TRSAApp()


void TRSAApp::InitMainWindow( void )
{
    MainWindow = new TMainWindow( NULL, Name );
    //RSAMainWindowHandle = MainWindow->HWindow;
} // TRSAApp::InitMainWindow()


#if 1
TRSAApp::~TRSAApp( void )
{
    delete MainWindow;
} // TRSAApp::~TRSAApp()
#endif


int PASCAL WinMain( HINSTANCE hInstance,
  HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow )
{
    TRSAApp RSAApp( "RSA Encryption", hInstance, hPrevInstance,
      	lpCmdLine, nCmdShow );

    RSAApp.Run();
    return( RSAApp.Status );
} // WinMain()


// ******** end of rsu_gui.cpp ********
