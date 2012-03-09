/*
** prototipacao de funcoes internas ao ctree 
*/
#include  <string.h>

/*#################################################### ctclib.c #####*/

#ifdef MSDOS
   RNDFILE mbopen( CTFILE *, COUNT );
   RNDFILE mbcrat( CTFILE * );
   COUNT   mbsave( CTFILE * );
#else
   RNDFILE mbopen( );
   RNDFILE mbcrat( );
   COUNT   mbsave( );
#endif

/*#################################################### ctclb2.c #####*/

#ifdef MSDOS
#ifdef UNIFRMAT
   VOID revbyt( TEXT *, COUNT);
   VOID revobj( TEXT *, COUNT);
   VOID revwrd( TEXT *, COUNT);
#endif

   COUNT uerr( COUNT );
   VOID  terr( COUNT );

   TEXT *mballc( unsigned int, unsigned int);
   VOID  mbfree( VOID * );

#ifndef cpybuf
   VOID cpybuf( TEXT *, TEXT *, UCOUNT);
#endif
#else
#ifdef UNIFRMAT
   VOID revbyt( );
   VOID revobj( );
   VOID revwrd( );
#endif

   COUNT uerr( );
   VOID  terr( );

   TEXT *mballc( );
   VOID  mbfree( );

#ifndef cpybuf
   VOID cpybuf( );
#endif
#endif



/*#################################################### ctclb3.c #####*/
#ifdef MSDOS
   COUNT ctseek( CTFILE *, POINTER);
   COUNT ctio( COUNT, CTFILE *, LONG, VOID *, SIZE);
   COUNT mbclos( CTFILE *, COUNT);
#else
   COUNT ctseek( );
   COUNT ctio( );
   COUNT mbclos( );
#endif


/*#################################################### ctinit.c #####*/
#ifdef MSDOS
   COUNT chkredf( CTFILE * );
   POINTER extfil( CTFILE *, unsigned int);
   CTFILE *tstfnm( COUNT );
   COUNT vtclose( VOID );
   UCOUNT inrfil( CTFILE * );
   COUNT  tstupd( CTFILE * );

   #ifdef UNIFRMAT
   VOID revhdr( CTFILE * );
   #endif

   COUNT  redhdr( CTFILE * );
   COUNT  wrthdr( CTFILE * );
   COUNT  wrtnod( TREEBUFF * );
#else
   COUNT chkredf( );
   POINTER extfil( );
   CTFILE *tstfnm( );
   COUNT vtclose( );
   UCOUNT inrfil( );
   COUNT  tstupd( );

#ifdef UNIFRMAT
   VOID revhdr( );
#endif

   COUNT  redhdr( );
   COUNT  wrthdr( );
   COUNT  wrtnod( );
#endif



/*#################################################### ctkrnl.c #####*/

#ifdef MSDOS
   COUNT nodser( TREEBUFF *, TEXT *, COUNT, TEXT);
#ifndef setnul
      VOID setnul( TEXT * );
#endif
   TREEBUFF *getnod( LONG, KEYFILE *);
   TREEBUFF *lrubuf( TREEBUFF * );
   VOID      inracs( TREEBUFF * );
   COUNT     rednod( TREEBUFF *, LONG, KEYFILE * );
#ifndef gtroot
   LONG      gtroot( KEYFILE * );
#endif
   TEXT     *hghpnt( TREEBUFF * );
   LONG      nodpnt( TREEBUFF *, COUNT );
   POINTER   drnpnt( TREEBUFF *, COUNT );
   TEXT     *valpnt( TREEBUFF *, COUNT );
   TEXT     *expval( TREEBUFF *, COUNT );
#else
   COUNT nodser( );
#ifndef setnul
      VOID setnul( );
#endif
   TREEBUFF *getnod( );
   TREEBUFF *lrubuf( );
   VOID      inracs( );
   COUNT     rednod( );
#ifndef gtroot
   LONG      gtroot( );
#endif
   TEXT     *hghpnt( );
   LONG      nodpnt( );
   POINTER   drnpnt( );
   TEXT     *valpnt( );
   TEXT     *expval( );
#endif

/*#################################################### ctcomp.c #####*/
#ifdef MSDOS
   COUNT compar( UTEXT *, UTEXT *, COUNT);
#else
   COUNT compar( );
#endif


/*#################################################### ctdatf.c #####*/
#ifdef MSDOS
   COUNT tstrec( DATFILE *, POINTER );
#else
   COUNT tstrec( );
#endif


/*#################################################### ctdelk.c #####*/
#ifdef MSDOS
   VOID delexp( TREEBUFF *);
#else
   VOID delexp( );
#endif


/*#################################################### ctsrch.c #####*/
#ifdef MSDOS
   POINTER fndkey( KEYFILE *, TEXT *, TEXT, COUNT);
   POINTER serlef( TEXT *, KEYFILE *, TREEBUFF *, TEXT, COUNT);
#else
   POINTER fndkey( );
   POINTER serlef( );
#endif



/*#################################################### ctaddk.c #####*/
#ifdef MSDOS
   COUNT     insert( TREEBUFF *, KEYFILE *, TEXT *, COUNT, COUNT);
   COUNT     adroot( KEYFILE *, POINTER, POINTER, TEXT *, COUNT );
   TREEBUFF *newnod( KEYFILE *, LONG *, COUNT);
   VOID      insexp( TREEBUFF *, TEXT *, POINTER, COUNT);

#else
   COUNT     insert( );
   COUNT     adroot( );
   TREEBUFF *newnod( );
   VOID      insexp( );
#endif


/*#################################################### ctvrcu.c #####*/

#ifdef MSDOS
   COUNT getvhdr( VATFILE *, LONG, VHDR *);
   COUNT putvhdr( VATFILE *, LONG, VHDR * );
   COUNT frmlkey( TEXT *, VRLEN *, POINTER );
   COUNT chkvhdr( VHDR * );
   VRLEN prprdv( COUNT, POINTER, TEXT *, VRLEN);
   COUNT stripkey( TEXT *, VRLEN *, POINTER *);
#else
   COUNT getvhdr( );
   COUNT putvhdr( );
   COUNT frmlkey( );
   COUNT chkvhdr( );
   VRLEN prprdv( );
   COUNT stripkey( );
#endif


/*#################################################### ctupdt.c #####*/
#ifdef MSDOS
   COUNT     putnod( TREEBUFF *, COUNT);
   TREEBUFF *movrgt( TEXT *, COUNT, KEYFILE *, TREEBUFF *);
   VOID      shfrgt( COUNT, TREEBUFF *, UCOUNT );
   VOID      shflft( COUNT, TREEBUFF *, UCOUNT );
#else
   COUNT     putnod( );
   TREEBUFF *movrgt( );
   VOID      shfrgt( );
   VOID      shflft( );
#endif



/*#################################################### cttrace.c #####*/
#ifdef MSDOS
   COUNT acha_prox( DATFILE *, UCOUNT *, POINTER * );
#else
   COUNT acha_prox( );
#endif






