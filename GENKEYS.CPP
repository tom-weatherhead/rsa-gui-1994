// genkeys.cpp
// **** TGenKeysDlg Member Functions ****

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


TGenKeysDlg::TGenKeysDlg( PTWindowsObject AParent, LPSTR name )
  : TDialog( AParent, name )
{
    PubFile[0] = PrivFile[0] = '\0';
    EditPubFile = new TEdit(this, ID_PUBFILE, sizeof(PubFile));
    EditPrivFile = new TEdit(this, ID_PRIVFILE, sizeof(PrivFile));
    EditKeySize = new TEdit(this, ID_KEYSIZE, sizeof(KeySizeStr));
    TransferBuffer = (void far *)NULL;
} // TGenKeysDlg::TGenKeysDlg()


void TGenKeysDlg::SetupWindow( void )
{
    TDialog::SetupWindow();
    strcpy( KeySizeStr, "20" );
    EditKeySize->Insert( KeySizeStr );
    SendDlgItemMsg( ID_KEYSIZE, WM_DRAWITEM, NULL, 0 );
} // TGenKeysDlg::SetupWindow()


void TGenKeysDlg::BrowsePubFile( RTMessage )
{
    TFileDialog * FileDlg;

    _fstrcpy( PubFile, "*.key" );
    FileDlg = new TFileDialog( this, SD_FILEOPEN, PubFile );
    FileDlg->SetCaption( "Public Key" );
    GetApplication()->ExecDialog( FileDlg );
    // Delete current contents of EditPubFile ???
    EditPubFile->Insert( PubFile );
} // TGenKeysDlg::BrowsePubFile()


void TGenKeysDlg::BrowsePrivFile( RTMessage )
{
    TFileDialog * FileDlg;

    _fstrcpy( PrivFile, "*.key" );
    FileDlg = new TFileDialog( this, SD_FILEOPEN, PrivFile );
    FileDlg->SetCaption( "Private Key" );
    GetApplication()->ExecDialog( FileDlg );
    // Delete current contents of EditPrivFile ???
    // EditPrivFile->DeleteLine( -1 ); // Current line
    EditPrivFile->Insert( PrivFile );
} // TGenKeysDlg::BrowsePrivFile()


BOOL TGenKeysDlg::CanClose( void )
{
    static char line[80];
    unsigned int KeySize;
    TMainWindow * MainWindow = (TMainWindow *)Parent;
    FILE * PubStrm = NULL, * PrivStrm = NULL;

    FillBuffers();

    if( (KeySize = atoi( KeySizeStr )) < 2  ||  KeySize > 100 ) {
	MessageBox( HWindow, "Invalid key length", "Error",
	  MB_OK | MB_ICONINFORMATION );
	// PrintMsg( RSAMB_OK, "Invalid key length" );
	return( FALSE );
    } // if

    if( (PubStrm = fopen( PubFile, "wb" )) == NULL ) {
    	sprintf( line, "Can't write to '%s'", PubFile );
    } else if( (PrivStrm = fopen( PrivFile, "wb" )) == NULL ) {
	sprintf( line, "Can't write to '%s'", PrivFile );
    } // if

    if( PrivStrm == NULL ) {

	if( PubStrm != NULL ) {
	    fclose( PubStrm );
	} // if

	MessageBox( HWindow, line, "Error", MB_OK | MB_ICONINFORMATION );
	return( FALSE );
    } // if

#if 1
    MainWindow->KeyStrm = PubStrm;
    MainWindow->KeyStrm2 = PrivStrm;
    MainWindow->KeySize = KeySize;
    return( TRUE );
#else
    PrintMsg( RSAMB_OK, "This will take a while!" );
    return( create_keys( PubFile, PrivFile, KeySize ) );
#endif
} // TGenKeysDlg::CanClose()


// Retrieve data

void TGenKeysDlg::FillBuffers( void )
{
    GetDlgItemText( HWindow, ID_PUBFILE, PubFile, sizeof(PubFile) );
    GetDlgItemText( HWindow, ID_PRIVFILE, PrivFile, sizeof(PrivFile) );
    GetDlgItemText( HWindow, ID_KEYSIZE, KeySizeStr, sizeof(KeySizeStr) );
} // TGenKeysDlg::FillBuffers()


TGenKeysDlg::~TGenKeysDlg( void )
{
    delete EditPubFile;
    delete EditPrivFile;
    delete EditKeySize;
} // TGenKeysDlg::~TGenKeysDlg()


// **** End of genkeys.cpp ****
