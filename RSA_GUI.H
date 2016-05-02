// rsa_gui.h


class TRSAApp : public TApplication
{
  public:
    TRSAApp( LPSTR AName, HINSTANCE hInstance,
      HINSTANCE hPrevInstance,
      LPSTR lpCmdLine, int nCmdShow );
    virtual void InitMainWindow( void );
    ~TRSAApp( void );
}; // TRSAApp


_CLASSDEF(TMainWindow)
class TMainWindow : public TWindow
{
  private:
    BOOL ButtonDown;

    BOOL CreateKeys( FILE * PubStrm, FILE * PrivStrm );
    BOOL ApplyKey( BOOL Encrypt );
    BOOL UUEncode( void );
    BOOL UUDecode( void );

  public:
    char SrcFileName[MAXPATH], DestFileName[MAXPATH];
    // char KeyFileName1[MAXPATH], KeyFileName2[MAXPATH],
    FILE * KeyStrm, * KeyStrm2, * SrcStrm, * DestStrm;
    int KeySize, LinesPerFile;

    TMainWindow( PTWindowsObject AParent, LPSTR ATitle );
    virtual void CMFileExit( RTMessage )
      = [CM_FIRST + CM_FILEEXIT];
    virtual void CMRSAGenerateKeys( RTMessage )
      = [CM_FIRST + CM_RSAGENERATEKEYS];
    virtual void CMRSAEncrypt( RTMessage )
      = [CM_FIRST + CM_RSAENCRYPT];
    virtual void CMRSADecrypt( RTMessage )
      = [CM_FIRST + CM_RSADECRYPT];
    virtual void CMUUEncode( RTMessage )
      = [CM_FIRST + CM_UUENCODE];
    virtual void CMUUDecode( RTMessage )
      = [CM_FIRST + CM_UUDECODE];
    virtual void CMAbout( RTMessage )
      = [CM_FIRST + CM_HELPABOUT];
    ~TMainWindow( void );
}; // TMainWindow


#ifdef __EDIT_H
class TGenKeysDlg : public TDialog
{
  private:
    TEdit * EditPubFile, * EditPrivFile, * EditKeySize;
    // Load local buffers from edit areas
    void FillBuffers( void );

  public:
    char PubFile[FILENAMESIZE], PrivFile[FILENAMESIZE],
      KeySizeStr[KEYSIZESTRSIZE];

    TGenKeysDlg( PTWindowsObject AParent, LPSTR name );
    void TGenKeysDlg::SetupWindow( void );
    virtual void BrowsePubFile( RTMessage )
      = [ID_FIRST + ID_BROWSEPUB];
    virtual void BrowsePrivFile( RTMessage )
      = [ID_FIRST + ID_BROWSEPRIV];
    virtual BOOL CanClose( void );
    ~TGenKeysDlg( void );
}; // TGenKeysDlg


class TEncryptDlg : public TDialog
{
  private:
    BOOL Encrypt;
    TEdit * EditKeyFile, * EditSrcFile, * EditDestFile;

    void FillBuffers( void );

  public:
    char KeyFile[FILENAMESIZE], SrcFile[FILENAMESIZE], DestFile[FILENAMESIZE];

    TEncryptDlg( PTWindowsObject AParent, LPSTR name, BOOL );
    virtual void BrowseKeyFile( RTMessage )
      = [ID_FIRST + ID_BROWSEKEYS];
    virtual void BrowseSrcFile( RTMessage )
      = [ID_FIRST + ID_BROWSESRC];
    virtual void BrowseDestFile( RTMessage )
      = [ID_FIRST + ID_BROWSEDEST];
    virtual BOOL CanClose( void );
    ~TEncryptDlg( void );
}; // TEncryptDlg


class TUUEncodeDlg : public TDialog
{
  private:
    char LinesPerFileStr[10], SrcFile[FILENAMESIZE], DestFile[FILENAMESIZE];
    TEdit * EditSrcFile, * EditDestFile, * EditLinesPerFile;

    void FillBuffers( void );

  public:
    TUUEncodeDlg( PTWindowsObject AParent, LPSTR name );
    void SetupWindow( void );
    virtual void BrowseSrcFile( RTMessage )
      = [ID_FIRST + ID_BROWSESRC];
    virtual void BrowseDestFile( RTMessage )
      = [ID_FIRST + ID_BROWSEDEST];
    virtual BOOL CanClose( void );
    ~TUUEncodeDlg( void );
}; // TUUEncodeDlg


class TUUDecodeDlg : public TDialog
{
  private:
    TEdit * EditSrcFile;

    void FillBuffers( void );

  public:
    char SrcFile[FILENAMESIZE];

    TUUDecodeDlg( PTWindowsObject AParent, LPSTR name );
    virtual void BrowseSrcFile( RTMessage )
      = [ID_FIRST + ID_BROWSESRC];
    virtual BOOL CanClose( void );
    ~TUUDecodeDlg( void );
}; // TUUDecodeDlg
#endif


class TProgMonWin : public TWindow
{
  private:
    BOOL * Cancelled;
    TStatic * StaticText;

  public:
    TProgMonWin( PTWindowsObject AParent, LPSTR name, BOOL * Cancelled2 );
    void SetupWindow( void );
    void UpdateText( char * );
    ~TProgMonWin( void );
}; // TProgMonWin


// **** End of rsa_gui.h ****
