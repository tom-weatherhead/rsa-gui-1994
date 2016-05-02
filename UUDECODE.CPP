// uudecode.cpp
// **** TUUDecodeDlg Member Functions ****

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


TUUDecodeDlg::TUUDecodeDlg( PTWindowsObject AParent, LPSTR name )
  : TDialog( AParent, name )
{
    SrcFile[0] = '\0';
    EditSrcFile = new TEdit(this, ID_SRCFILE, sizeof(SrcFile));
    TransferBuffer = (void far *)NULL;
} // TUUDecodeDlg::TUUDecodeDlg()


void TUUDecodeDlg::BrowseSrcFile( RTMessage )
{
    TFileDialog * FileDlg;

    _fstrcpy( SrcFile, "*.src" );
    // Create file dialog, change caption, then run?
    FileDlg = new TFileDialog( this, SD_FILEOPEN, SrcFile );
    FileDlg->SetCaption( "Source Filename" );
    GetApplication()->ExecDialog( FileDlg );
//    GetApplication()->ExecDialog( new TFileDialog( this, SD_FILEOPEN,
//      DestFile ) );

    // Delete current contents of EditDestFile ???
    // EditDestFile->DeleteLine( -1 ); // Current line
    EditSrcFile->Insert( SrcFile );
} // TUUDecodeDlg::BrowseSrcFile()


BOOL TUUDecodeDlg::CanClose( void )
{
    static char line[FILENAMESIZE + 15];
    FILE * SrcStrm;

    FillBuffers();

    if( (SrcStrm = fopen( SrcFile, "r" )) == NULL ) {
	sprintf( line, "Can't read '%s'", SrcFile );
	// PrintMsg( RSAMB_OK, line );
	MessageBox( HWindow, line, "Error",
	  MB_OK | MB_ICONINFORMATION );
	return( FALSE );
    } // if

    ((TMainWindow *)Parent)->SrcStrm = SrcStrm;
    return( TRUE );
} // TUUDecodeDlg::CanClose()


// Retrieve data

void TUUDecodeDlg::FillBuffers( void )
{
    GetDlgItemText( HWindow, ID_SRCFILE, SrcFile, sizeof(SrcFile) );
} // TUUDecodeDlg::FillBuffers()


TUUDecodeDlg::~TUUDecodeDlg( void )
{
    delete EditSrcFile;
} // TUUDecodeDlg::~TUUDecodeDlg()


// **** End of uudecode.cpp ****
