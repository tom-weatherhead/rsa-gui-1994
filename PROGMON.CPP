// progmon.cpp - Progress Monitor Window Class
// Execute using GetModule()->MakeWindow(  );

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


TProgMonWin::TProgMonWin( PTWindowsObject AParent, LPSTR name,
  BOOL * Cancelled2 )
  : TWindow( AParent, name )
{
    Cancelled = Cancelled2;
    *Cancelled = FALSE;
    Attr.Style |= WS_POPUPWINDOW | WS_CAPTION;
    Attr.X = 200;
    Attr.Y = 200;
    Attr.W = 250;
    Attr.H = 100;
    StaticText = new TStatic( this, ID_PROGRESSMSG, "Working...", 11, 9, 200,
      16, 25 );
} // TProgMonWin::TProgMonWin()


void TProgMonWin::SetupWindow( void )
{
    TWindow::SetupWindow();
    InvalidateRect( HWindow, NULL, TRUE );
} // TProgMonWin::SetupWindow()


void TProgMonWin::UpdateText( char * NewText )
{
    StaticText->SetText( NewText );
    //SendDlgItemMsg( ID_PROGRESSMSG, WM_DRAWITEM, NULL, 0 );
    InvalidateRect( HWindow, NULL, TRUE );
} // TProgMonWin::UpdateText()


TProgMonWin::~TProgMonWin( void )
{
    *Cancelled = TRUE;
    delete StaticText;
} // TProgMonWin::~TProgMonWin()


// **** End of progmon.cpp ****
