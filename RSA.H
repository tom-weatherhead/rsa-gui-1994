// rsa.h - Header file for implementation of RSA-scheme
// encryption
// (C) Copyright 1994 by Tom Weatherhead
// Started March 7, 1994

// Definitions

#define NUM_WITNESSES			40
#define FILENAMESIZE			80
#define KEYSIZESTRSIZE			10
#define BIGNUM_BUF_SIZE			1000
#define NUM_BITS_IN_BYTE		8

#if 0
#define resize_if(num,newlen) \
    if((num)->buf_size<newlen){(num)->resize(newlen);}

// enum { RSAOP_ENCRYPT, RSAOP_DECRYPT };

enum {
    RSAMB_YESNO,
    RSAMB_OK,
    RSAMB_OKCANCEL
};
#endif

// Type and class definitions

#ifdef FALSE
typedef char bool;
#else
typedef enum { FALSE, TRUE } bool;
#endif


class bignum_class {
  public:
    int len;
    unsigned char * buf;

    bignum_class( void );
    unsigned int to_int( void );
    void trim( void );
    void zero_pad( unsigned short );
    void inc( void );
    void dec( void );
    void random( unsigned short );
    bool fileread( FILE * );
    void filewrite( FILE * );
#if 0
    void resize( unsigned short );
    void print( void );
    void sprint( char * );
    void complement( void );
    void ror( void );
#endif
    ~bignum_class( void );
}; // bignum_class


// Prototypes
int GetFilePath( char *, char *, int );
int GetString( char *, char *, int );
int PrintMsg( int, char * );

bool textencode( char *, char *, int );
void textdecode( char * );

bool create_keys( char *, char *, unsigned int );
bool apply_key( unsigned int, FILE *, FILE *, FILE * );

// **** End of rsa.h ****