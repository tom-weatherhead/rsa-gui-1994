// uuencode.cpp
// **** TUUEncodeDlg Member Functions ****

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


TUUEncodeDlg::TUUEncodeDlg( PTWindowsObject AParent, LPSTR name )
  : TDialog( AParent, name )
{
    LinesPerFileStr[0] = SrcFile[0] = DestFile[0] = '\0';
    EditSrcFile = new TEdit(this, ID_SRCFILE, sizeof(SrcFile));
    EditDestFile = new TEdit(this, ID_DESTFILE, sizeof(DestFile));
    EditLinesPerFile = new TEdit(this, ID_LINESPERFILE,
      sizeof(LinesPerFileStr));
    EditLinesPerFile->Insert( "1000" );
    TransferBuffer = (void far *)NULL;
} // TUUEncodeDlg::TUUEncodeDlg()


void TUUEncodeDlg::SetupWindow( void )
{
    TDialog::SetupWindow();
    strcpy( LinesPerFileStr, "950" );
    EditLinesPerFile->Insert( LinesPerFileStr );
    SendDlgItemMsg( ID_LINESPERFILE, WM_DRAWITEM, NULL, 0 );
} // TUUEncodeDlg::SetupWindow()


void TUUEncodeDlg::BrowseSrcFile( RTMessage )
{
    TFileDialog * FileDlg;

    _fstrcpy( SrcFile,"*.*" );
    FileDlg = new TFileDialog( this, SD_FILEOPEN, SrcFile );
    FileDlg->SetCaption( "Source Filename" );
    GetApplication()->ExecDialog( FileDlg );
    // Delete current contents of EditSrcFile ???
    EditSrcFile->Insert( SrcFile );
} // TUUEncodeDlg::BrowseSrcFile()


void TUUEncodeDlg::BrowseDestFile( RTMessage )
{
    TFileDialog * FileDlg;

    _fstrcpy( DestFile, "*.src" );
    // Create file dialog, change caption, then run
    FileDlg = new TFileDialog( this, SD_FILEOPEN, DestFile );
    FileDlg->SetCaption( "Output Filename" );
    GetApplication()->ExecDialog( FileDlg );
    // Delete current contents of EditDestFile ???
    // EditDestFile->DeleteLine( -1 ); // Current line
    EditDestFile->Insert( DestFile );
} // TUUEncodeDlg::BrowsePrivFile()


BOOL TUUEncodeDlg::CanClose( void )
{
    static char line[FILENAMESIZE + 15];
    int LinesPerFile;
    FILE * SrcStrm; // , * DestStrm;
    TMainWindow * MainWindow = (TMainWindow *)Parent;

    FillBuffers();

    if( ( LinesPerFile = atoi( LinesPerFileStr ) ) < 100
      ||  LinesPerFile > 1000 ) {
	// PrintMsg( RSAMB_OK, "Invalid number of lines per file" );
	MessageBox( HWindow, "Invalid number of lines per file", "Error",
	  MB_OK | MB_ICONINFORMATION );
	return( FALSE );
    } // if

    if( (SrcStrm = fopen( SrcFile, "rb" )) == NULL ) {
	sprintf( line, "Can't read '%s'", SrcFile );
	// PrintMsg( RSAMB_OK, line );
	MessageBox( HWindow, line, "Error", MB_OK | MB_ICONINFORMATION );
	return( FALSE );
    } // if

#if 0
    if( (DestStrm = fopen( DestFile, "w" )) == NULL ) {
//	fclose( SrcStrm );
	sprintf( line, "Can't write to '%s'", DestFile );
	PrintMsg( RSAMB_OK, line );
	return( FALSE );
    } // if

    fclose( DestStrm );
#endif
    MainWindow->SrcStrm = SrcStrm;
    MainWindow->LinesPerFile = LinesPerFile;
    strcpy( MainWindow->SrcFileName, SrcFile );
    strcpy( MainWindow->DestFileName, DestFile );
    return( TRUE );
} // TUUEncodeDlg::CanClose()


// Retrieve data

void TUUEncodeDlg::FillBuffers( void )
{
    GetDlgItemText( HWindow, ID_SRCFILE, SrcFile, sizeof(SrcFile) );
    GetDlgItemText( HWindow, ID_DESTFILE, DestFile, sizeof(DestFile) );
    GetDlgItemText( HWindow, ID_LINESPERFILE, LinesPerFileStr,
      sizeof(LinesPerFileStr) );
} // TUUEncodeDlg::FillBuffers()


TUUEncodeDlg::~TUUEncodeDlg( void )
{
    delete EditLinesPerFile;
    delete EditSrcFile;
    delete EditDestFile;
    delete EditLinesPerFile;
} // TUUEncodeDlg::~TUUEncodeDlg()


// **** End of uuencode.cpp ****
