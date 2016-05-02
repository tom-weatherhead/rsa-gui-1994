// encrypt.cpp
// **** TEncryptDlg Member Functions ****

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


TEncryptDlg::TEncryptDlg( PTWindowsObject AParent, LPSTR name,
  BOOL EncryptParm ) : TDialog( AParent, name )
{
    Encrypt = EncryptParm;
    KeyFile[0] = SrcFile[0] = DestFile[0] = '\0';
    EditKeyFile = new TEdit(this, ID_KEYFILE, sizeof(KeyFile));
    EditSrcFile = new TEdit(this, ID_SRCFILE, sizeof(SrcFile));
    EditDestFile = new TEdit(this, ID_DESTFILE, sizeof(DestFile));
    TransferBuffer = (void far *)NULL;
} // TEncryptDlg::TEncryptDlg()


void TEncryptDlg::BrowseKeyFile( RTMessage )
{
    TFileDialog * FileDlg;

    _fstrcpy( KeyFile, "*.key" );
    FileDlg = new TFileDialog( this, SD_FILEOPEN, KeyFile );
    FileDlg->SetCaption( Encrypt ? "Public Key Filename"
      : "Private Key Filename" );
    GetApplication()->ExecDialog( FileDlg );
    // Delete current contents of EditPubFile ???
    EditKeyFile->Insert( KeyFile );
} // TEncryptDlg::BrowseKeyFile()


void TEncryptDlg::BrowseSrcFile( RTMessage )
{
    TFileDialog * FileDlg;

    _fstrcpy( SrcFile, Encrypt ? "*.*" : "*.rsa" );
    FileDlg = new TFileDialog( this, SD_FILEOPEN, SrcFile );
    FileDlg->SetCaption( "Source Filename" );
    GetApplication()->ExecDialog( FileDlg );
    // Delete current contents of EditPubFile ???
    EditSrcFile->Insert( SrcFile );
} // TEncryptDlg::BrowseSrcFile()


void TEncryptDlg::BrowseDestFile( RTMessage )
{
    TFileDialog * FileDlg;

    _fstrcpy( DestFile, Encrypt ? "*.rsa" : "*.*" );
    // Create file dialog, change caption, then run?
    FileDlg = new TFileDialog( this, SD_FILEOPEN, DestFile );
    FileDlg->SetCaption( "Output Filename" );
    GetApplication()->ExecDialog( FileDlg );
    // Delete current contents of EditPrivFile ???
    // EditPrivFile->DeleteLine( -1 ); // Current line
    EditDestFile->Insert( DestFile );
} // TEncryptDlg::BrowseDestFile()


BOOL TEncryptDlg::CanClose( void )
{
    static char line[FILENAMESIZE + 15];
    FILE * KeyStrm = NULL, * SrcStrm = NULL, * DestStrm = NULL;
    TMainWindow * MainWindow = (TMainWindow *)Parent;

    FillBuffers();

    if( (KeyStrm = fopen( KeyFile, "rb" )) == NULL ) {
	sprintf( line, "Can't read '%s'", KeyFile );
    } else if( (SrcStrm = fopen( SrcFile, "rb" )) == NULL ) {
        fclose( KeyStrm );
	sprintf( line, "Can't read '%s'", SrcFile );
    } else if( (DestStrm = fopen( DestFile, "wb" )) == NULL ) {
	sprintf( line, "Can't write to '%s'", DestFile );
    } // if

    if( DestStrm == NULL ) {

	if( KeyStrm != NULL ) {
	    fclose( KeyStrm );
	} // if

	if( SrcStrm != NULL ) {
	    fclose( SrcStrm );
	} // if

	MessageBox( HWindow, line, "Error", MB_OK | MB_ICONINFORMATION );
	return( FALSE );
    } // if

    MainWindow->KeyStrm = KeyStrm;
    MainWindow->SrcStrm = SrcStrm;
    MainWindow->DestStrm = DestStrm;
    return( TRUE );
} // TEncryptDlg::CanClose()


// Retrieve data

void TEncryptDlg::FillBuffers( void )
{
    GetDlgItemText( HWindow, ID_KEYFILE, KeyFile, sizeof(KeyFile) );
    GetDlgItemText( HWindow, ID_SRCFILE, SrcFile, sizeof(SrcFile) );
    GetDlgItemText( HWindow, ID_DESTFILE, DestFile, sizeof(DestFile) );
} // TEncryptDlg::FillBuffers()


TEncryptDlg::~TEncryptDlg( void )
{
    delete EditKeyFile;
    delete EditSrcFile;
    delete EditDestFile;
} // TEncryptDlg::~TEncryptDlg()


// **** End of encrypt.cpp ****
