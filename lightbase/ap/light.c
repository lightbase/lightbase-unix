#include  <stdio.h>
#include  <stdlib.h>
#include  <process.h>
#include  <string.h>
#include  <errno.h>
#include  <alloc.h>
#include  <dos.h>
#include  <bios.h>
#include  <time.h>
#include  "lb2head.h"

#define   SIB            (*((char **) MK_FP(_psp, 0xFC)))
#define   EV_PROGRAM     (SIB + 9)
#define   EV_SERIE       (*((unsigned long *) (SIB + 22)))

#define   TAM_LOOP   47
#define   NO_ARQ     15


#ifdef ESPANHA
   #define   STR_01 "Demonstracion      \n\n"
   #define   STR_02 "No consigo localizar fichero del \"overlay\".  " 
   #define   STR_03 "Mui ficheros abiertos." 
   #define   STR_04 "fichero del \"overlay\" con error." 
   #define   STR_05 "No ha memoria disponible" 
   #define   STR_06 "No consigo abrir los ficheros necesarios. E Necesaria la actualizacion"
   #define   STR_07 "del fichero C:\\CONFIG.SYS. Incluya o altere la directiva files= para no\n"
   #define   STR_08 "minimo 20 ficheros.\n"
   #define   STR_09 "Error en la gravacion del fichero." 
   #define   STR_10 "No existe area disponible en las variables ambientales \"environment\", favor actualize lo CONFIG.SYS."
#else
   #define   STR_01 "Demonstracao\n\n"
   #define   STR_02 "Nao consigo achar arquivo de \"overlay\"." 
   #define   STR_03 "Muitos arquivos abertos." 
   #define   STR_04 "Arquivo de \"overlay\" com formato errado." 
   #define   STR_05 "Nao ha' memoria disponivel" 
   #define   STR_06 "Nao consigo abrir os arquivos necessarios. E' Necessaria a atualizacao"
   #define   STR_07 "do arquivo C:\\CONFIG.SYS. Inclua ou altere a diretiva files= para no\n"
   #define   STR_08 "minimo 20 arquivos.\n"
   #define   STR_09 "Erro na gravacao de arquivo." 
   #define   STR_10 "Nao existe area disponivel nas variaveis ambientais \"environment\", favor atualize o CONFIG.SYS."
#endif

int encripta( void )
{
   int      I, ret = 0, usuarios = 1;
   char     *env1 = "ABC_XYZ1313=12345678901234567890";
   char     *env2 = "ABC_XYZ2424=12345678901234567890";
   char     *env3 = "ABC_XYZ1313=";
   char     *env4 = "ABC_XYZ2424=";
   char     *env5 = "ABC_XYZ1313";
   char     *env6 = "ABC_XYZ2424";
   char     *env7 = "12345678901234567890";
   char     *l1;
   char     buf1[ TAM_LINHA ];
   unsigned char far  *P1;
   unsigned int  far  *Q1;
   unsigned int  x1, x2, x3, x4;
   FILE     *FP, *F[ NO_ARQ ];
   time_t   secs_now;
   struct   tm  *tm_now;


{  /* Pega Numero de usuarios */
   char *p;
   p = SIB + 50;
   if ( (p = strstr(p, "##")) != NULL ) {
      while ( *p < '0' || *p > '9' )
         p++;
      usuarios = atoi(p);
   }
}

   unlink( NOME_ARQ ); 

   printf( "\n%s\n", MENS_1 );
   printf( "%s\n%s\n",   MENS_2, MENS_2A);

   if ( usuarios > 1 ) 
      printf( MENS_USR,  usuarios );

   printf( "%s - ",  MENS_3 );

   if ( !strncmp( SIB, "AZEV", 4 )  &&
       ( !stricmp( EV_PROGRAM, "Light.exe" ) ||
         !stricmp( EV_PROGRAM, "Lightrt.exe" ) ||
         !stricmp( EV_PROGRAM, "Cenario.exe" )) )
   {
      printf( "N%c %lu\n\n", 167, EV_SERIE );
      if ( EV_SERIE >= 950000L && EV_SERIE <= 980000L )
         l1 = LINHA_2;  /* runtime */
      else 
         l1 = LINHA_1;
   }
   else {  printf( STR_01 );   return( ret );  }


   for ( I = 0;   I < NO_ARQ;   I++ )   F[ I ] = NULL;

   if ( (F[0] = fopen( NOME_ARQ, "wb" )) == NULL ) {
      ret = 100;
      goto fim;
   }
   else
      fclose(F[0]);

   errno = 0;

   for ( I = 0;   I < NO_ARQ;   I++ ) 
   {  F[ I ] = fopen( NOME_ARQ, "rb" );
      if ( F[I]== NULL ) {
         ret = 100;
         goto fim;
      }
   }

   for ( I = 0;   I < NO_ARQ;   I++ ) 
   {
      if ( F[ I ] != NULL )   fclose( F[ I ] );
   }

   if ( (FP = fopen( NOME_ARQ, "wb" ))  ==  NULL )   return( 101 );

                     {
                        char   B[ TAM_LOOP + 1 ];
                        int    I;
                        long   S;

                        S = ftell( FP );
                        for ( I = 0;   I < TAM_LOOP;   I++ )
                        {
                           if ( !(I % 2) || I % 3 == S % 4 )  B[ I ] = I + 'S';
                           if ( !(I % 3) || I % 4 == S % 5 )  B[ I ] = I / 'i';
                           if ( !(I % 4) || I % 5 == S % 6 )  B[ I ] = I % 'm';
                           if ( !(I % 5) || I % 6 == S % 7 )  B[ I ] = I - 'p';
                           if ( !(I % 6) || I % 7 == S % 8 )  B[ I ] = I + 'l';
                           if ( !(I % 7) || I % 8 == S % 9 )  B[ I ] = I / 'e';
                        }
                        fwrite( &B, TAM_LOOP, 1, FP );
                        fseek( FP, S, 0 );
                     }

   if ( putenv( env1 ) < 0  ||  putenv( env2 ) < 0 ) ret = 102;

   fprintf( FP, "##%d\n", usuarios );
   fprintf( FP, "%s\n", l1 );

   time( &secs_now );

{
   char   B[ TAM_LOOP + 1 ];
   int    I;
   long   S;

   S = ftell( FP );
   for ( I = 0;   I < TAM_LOOP;   I++ )
   {
      if ( !(I % 2) || I % 3 == S % 4 )  B[ I ] = I + 'S';
      if ( !(I % 3) || I % 4 == S % 5 )  B[ I ] = I / 'i';
      if ( !(I % 4) || I % 5 == S % 6 )  B[ I ] = I % 'm';
      if ( !(I % 5) || I % 6 == S % 7 )  B[ I ] = I - 'p';
      if ( !(I % 6) || I % 7 == S % 8 )  B[ I ] = I + 'l';
      if ( !(I % 7) || I % 8 == S % 9 )  B[ I ] = I / 'e';
   }
   fwrite( &B, TAM_LOOP, 1, FP );
   fseek( FP, S, 0 );
}

   fwrite( &secs_now, sizeof(secs_now), 1, FP );

   tm_now = localtime( &secs_now );

{
   char   B[ TAM_LOOP + 1 ];
   int    I;
   long   S;

   S = ftell( FP );
   for ( I = 0;   I < TAM_LOOP;   I++ )
   {
      if ( !(I % 2) || I % 3 == S % 4 )  B[ I ] = I + 'S';
      if ( !(I % 3) || I % 4 == S % 5 )  B[ I ] = I / 'i';
      if ( !(I % 4) || I % 5 == S % 6 )  B[ I ] = I % 'm';
      if ( !(I % 5) || I % 6 == S % 7 )  B[ I ] = I - 'p';
      if ( !(I % 6) || I % 7 == S % 8 )  B[ I ] = I + 'l';
      if ( !(I % 7) || I % 8 == S % 9 )  B[ I ] = I / 'e';
   }
   fwrite( &B, TAM_LOOP, 1, FP );
   fseek( FP, S, 0 );
}

   fprintf( FP, "%d %d %d %d %d\n",
            (tm_now->tm_mday + 'L'),
            ((tm_now->tm_mon  * 'l') + 'I'),
            (((tm_now->tm_year - 'l') * 'i') + 'G'),
            ((((tm_now->tm_wday / 'l') - 'i') * 'g') + 'H'),
            (((((tm_now->tm_yday % 'l') / 'i') - 'g') * 'h') + 'T') );

   P1 = MK_FP( 0x40, 0x10 );    buf1[ 0 ] = *P1 + tm_now->tm_mday;
   P1 = MK_FP( 0x41, 0x3  );    buf1[ 1 ] = *P1 * tm_now->tm_mon;
   P1 = MK_FP( 0x40, 0x18 );    buf1[ 2 ] = *P1 ^ tm_now->tm_year;

{
   char   B[ TAM_LOOP + 1 ];
   int    I;
   long   S;

   S = ftell( FP );
   for ( I = 0;   I < TAM_LOOP;   I++ )
   {
      if ( !(I % 2) || I % 3 == S % 4 )  B[ I ] = I + 'S';
      if ( !(I % 3) || I % 4 == S % 5 )  B[ I ] = I / 'i';
      if ( !(I % 4) || I % 5 == S % 6 )  B[ I ] = I % 'm';
      if ( !(I % 5) || I % 6 == S % 7 )  B[ I ] = I - 'p';
      if ( !(I % 6) || I % 7 == S % 8 )  B[ I ] = I + 'l';
      if ( !(I % 7) || I % 8 == S % 9 )  B[ I ] = I / 'e';
   }
   fwrite( &B, TAM_LOOP, 1, FP );
   fseek( FP, S, 0 );
}

   fwrite( buf1, 3, 1, FP );

   buf1[ 0 ] = tm_now->tm_mday ^ 1919;
   buf1[ 1 ] = tm_now->tm_mon  + 99;
   buf1[ 2 ] = tm_now->tm_year * 1313;
   buf1[ 3 ] = tm_now->tm_wday + 1616;
   buf1[ 4 ] = tm_now->tm_yday * 1212;
   buf1[ 5 ] = tm_now->tm_mday ^ 55;
   buf1[ 6 ] = tm_now->tm_mon  + 1212;
   buf1[ 7 ] = tm_now->tm_year * 99;
   buf1[ 8 ] = tm_now->tm_wday + 77;
   buf1[ 9 ] = tm_now->tm_yday * 88;
   buf1[10 ] = tm_now->tm_yday ^ 2020;

{
   char   B[ TAM_LOOP + 1 ];
   int    I;
   long   S;

   S = ftell( FP );
   for ( I = 0;   I < TAM_LOOP;   I++ )
   {
      if ( !(I % 2) || I % 3 == S % 4 )  B[ I ] = I + 'S';
      if ( !(I % 3) || I % 4 == S % 5 )  B[ I ] = I / 'i';
      if ( !(I % 4) || I % 5 == S % 6 )  B[ I ] = I % 'm';
      if ( !(I % 5) || I % 6 == S % 7 )  B[ I ] = I - 'p';
      if ( !(I % 6) || I % 7 == S % 8 )  B[ I ] = I + 'l';
      if ( !(I % 7) || I % 8 == S % 9 )  B[ I ] = I / 'e';
   }
   fwrite( &B, TAM_LOOP, 1, FP );
   fseek( FP, S, 0 );
}

   fwrite( buf1, 11, 1, FP );

   {  /* gravacao do numero de serie no arquivo */
      unsigned long numserie;
      numserie = EV_SERIE;
      fwrite( &numserie, 4, 1, FP );
   }

   fclose( FP );

   while( bioskey( 1 ) )   bioskey( 0 );

   Q1 = MK_FP( 0x40, 0x1A );   *Q1 = 0x1E;
   Q1 = MK_FP( 0x40, 0x1C );   *Q1 = 0x2A;
   Q1 = MK_FP( 0x40, 0x1E );


   x1 = (unsigned int) ((secs_now & 0x7F) << 8);
   x2 = (unsigned int) (secs_now & 0x7F00);
   x3 = (unsigned int) ((secs_now & 0x7F0000L) >> 8);
   x4 = (unsigned int) ((secs_now & 0x7F000000L) >> 16);

   x1 = 'S' | x1;
   x2 = 'i' | x2;
   x3 = 'm' | x3;
   x4 = 'p' | x4;

   *Q1++ = x1;
   *Q1++ = x2;
   *Q1++ = x3;
   *Q1++ = x4;
   *Q1++ = 'l';
   *Q1++ = 'e';

   if ( strcmp (getenv( env5 ), getenv( env6 )) != 0 ) ret = 102;

   if ( strcmp (getenv( env5 ), env7 ) != 0 )         ret = 102;

   if ( putenv( env3 ) < 0  ||  putenv( env4 ) < 0 )  ret = 102;

fim :

   return( ret );
}


void main( int argc, char *argv[ ] )
{
   argc = argc;

   switch ( encripta( ) )   
   {
   case   0:
               execvp( "lb.ovl", argv );

               switch( errno )
               {
               default     :
               case EACCES :
               case ENOENT :  printf(STR_02);
                              break;
               case EMFILE :  printf(STR_03);
                              break;
               case ENOEXEC : printf(STR_04);
                              break;
               case ENOMEM :  printf(STR_05);
                              break;
               }
               break;

   case 100:
               printf(STR_06);
               printf(STR_07);
               printf(STR_08);
               break;

   case 101:
               printf(STR_09);
               break;

   case 102:
               printf(STR_10);
               break;
   }

   while( bioskey( 1 ) )   bioskey( 0 );

   exit( 1 );
}



