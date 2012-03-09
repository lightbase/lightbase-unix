#include <alloc.h>
#include <conio.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <mem.h>
#include <io.h>
#include <fcntl.h>
#include <errno.h>
#include <dos.h>

#include  <sys/types.h>
#include  <sys/stat.h>

#define  SIM  'X'

char *campos[ ] =
{
   /*  0 */   "LOCAL_R", 
   /*  1 */   "DOI_R", 
   /*  2 */   "ANX1PF_R", 
   /*  3 */   "FORMPJ_R", 
   /*  4 */   "DIRF_R", 
   /*  5 */   "RAIS_R", 
   /*  6 */   "LOCAL_G", 
   /*  7 */   "AERO_G", 
   /*  8 */   "COMB_G", 
   /*  9 */   "VEIC_G", 
   /* 10 */   "EMBARC_G", 
   /* 11 */   "DOI_G", 
   /* 12 */   "ANX1PF_G", 
   /* 13 */   "TELE_G", 
   /* 14 */   "ENERGIA_G", 
   /* 15 */   "ARREC_G", 
   /* 16 */   "VIAG_G", 
   /* 17 */   "IMPORT_G", 
   /* 18 */   "LOCAL_P", 
   /* 19 */   "AERO_P", 
   /* 20 */   "COMB_P", 
   /* 21 */   "VEIC_P", 
   /* 22 */   "EMBARC_P", 
   /* 23 */   "DOI_P", 
   /* 24 */   "ANX1PF_P", 
   /* 25 */   "DIRF_P", 
   /* 26 */   "TELE_P", 
   /* 27 */   "DIOF_P", 
   /* 28 */   "DIOFIN_P", 
   /* 29 */   "ANEXO2_P", 
   /* 30 */   "ANEXO3_P", 
   /* 31 */   "ARREC_P", 
   /* 32 */   "VIAG_P", 
   /* 33 */   "LOCAL_A", 
   /* 34 */   "AERO_A", 
   /* 35 */   "COMB_A", 
   /* 36 */   "VEIC_A", 
   /* 37 */   "EMBARC_A", 
   /* 38 */   "FORMPJ_A", 
   /* 39 */   "DIRF_A", 
   /* 40 */   "TELE_A", 
   /* 41 */   "ANEXO3_A", 
   /* 42 */   "IMPORT_A", 
   /* 43 */   "CALCULO_A", 
   /* 44 */   "CEDULA_A", 
   /* 45 */   "ALIQ_A", 
   /* 46 */   "PENSAO_A", 
   /* 47 */   "DOIVALOR_A", 
   /* 48 */   "DOIQTD_A", 
   /* 49 */   "OMISSOS_A", 
   /* 50 */   "CADEC_A", 
   /* 51 */   "DIRF1283_A", 
   /* 52 */   "APLIC_A", 
   /* 53 */   "REDEA_A", 
   /* 54 */   "DEC89_T", 
   /* 55 */   "DEC90_T", 
   /* 56 */   "DEC91_T", 
   /* 57 */   NULL
};

static int files[ 100 ]; /*** Devem ser ZEROS ***/


/**************************************************************************
 * Seta o numero de arquivos que podem ser abertos ao mesmo tempo.
 **************************************************************************/
int SetaNumeroHandlers ( int valor )
{
  union REGS regs;

  regs.h.ah = 0x67;
  regs.x.bx = valor;

  intdos ( &regs, &regs );
  if ( regs.x.cflag ) {
		 errno = regs.x.ax;
		 return -1;
  }
  return ( 0 );
}

/**************************************************************************
 * Fecha arquivo. Substitui o close do Turbo C que nao funciona com mais
	de 20 arquivos abertos ao mesmo tempo
 **************************************************************************/
int a_close ( int handle )
{
  union REGS regs;

  regs.h.ah = 0x3E;
  regs.x.bx = handle;

  intdos ( &regs, &regs );
  if ( regs.x.cflag ) {
		 errno = regs.x.ax;
		 return -1;
  }
  return ( regs.x.ax );
}

int openfiles( int qf )
{
   int  I;
   int  modo = O_CREAT | O_TRUNC  | O_RDWR | O_BINARY;
   char arq[ 100 ];
   
   for ( I = 0;   I < qf  &&  campos[ I ] != NULL;   I++ )
   {
      sprintf( arq, "%.8s.lbi", campos[ I ] );
      
      if ((files[ I ] = open( arq, modo, (S_IREAD | S_IWRITE) )) < 0 )   break;
   }
   
   return( I );
}

int  arqreg( int file, long reg )
{
   int  fd = files[ file ];
   
   if ( write( fd, &reg, 3 )  < 0 )    return -1;
   
   return 0;
}
            
int closefiles( )
{

   int  I;
   
   for ( I = 0;   campos[ I ] != NULL;   I++ )
   {
      
      if ( files[ I ] != 0 )
         a_close(files[I]);
   }

   return(0);
}


void main( argc, argv)
int argc;
char **argv;
{
   short  I, J, qf;
   long   L, max=999999999L;
   char   *P, linha[ 1000 ];

   if ( argc > 1 ) {
      max = atol(argv[1]);
   }

   if ( SetaNumeroHandlers ( 70 )  <  0 )
   {
      fprintf( stderr, "ERRO: SetaNumerosHandlers - %d\n", errno );
      goto fim;
   }
   
   for ( qf= 0;   campos[ qf ] != NULL;   qf++ )
      ;
      
   if ((I = openfiles( qf ))  !=  qf )
   {
      fprintf( stderr, "ERRO: Openfiles %d - %d\n", I, errno );
      goto fim;
   }
   

   fprintf( stdout, "CPF,IAPS_89,IAPS_90,IAPS_91,IAPS_TOTAL,POT_ECO,");
   fprintf( stdout, "LOCAL_R,DOI_R,ANX1PF_R,FORMPJ_R,DIRF_R,RAIS_R,");
   fprintf( stdout, "LOCAL_G,AERO_G,COMB_G,VEIC_G,EMBARC_G,DOI_G,");
   fprintf( stdout, "ANX1PF_G,TELE_G,ENERGIA_G,ARREC_G,VIAG_G,IMPORT_G,");
   fprintf( stdout, "LOCAL_P,AERO_P,COMB_P,VEIC_P,EMBARC_P,DOI_P,");
   fprintf( stdout, "ANX1PF_P,DIRF_P,TELE_P,DIOF_P,DIOFIN_P,ANEXO2_P,");
   fprintf( stdout, "ANEXO3_P,ARREC_P,VIAG_P,LOCAL_A,AERO_A,COMB_A,");
   fprintf( stdout, "VEIC_A,EMBARC_A,FORMPJ_A,DIRF_A,TELE_A,ANEXO3_A,");
   fprintf( stdout, "IMPORT_A,CALCULO_A,CEDULA_A,ALIQ_A,PENSAO_A,DOIVALOR_A,");
   fprintf( stdout, "DOIQTD_A,OMISSOS_A,CADEC_A,DIRF1283_A,APLIC_A,REDEA_A,");
   fprintf( stdout, "GRUPO,DEC89_T,DEC90_T,DEC91_T,OCUPACAO,DELEGACIA,\n" );
		     
   

   fprintf( stderr, "1\n" );
   
   for( L = 1; L <= max && fgets( linha, 800, stdin ) != NULL;   L++ )
   {
      if ( strlen( linha ) != 142 )
      {
         fprintf( stderr, "ATENCAO: Tamanho da Linha != 142\n" );
         break;
      }
      
      if ( L % 100 == 0 )   fprintf( stderr, "%ld\n", L );
      
      P = linha;
      
      
      /*** Inicio Campo ***/
      /*** CPF ***/
      for( I = 0;   I < 9;   I++ )   fprintf( stdout, "%c", *P++ );
      
      
      /*** IAPAS_89_90_91_TOTAL ***/
      /*** Potencial Econimico ***/
      for( J = 0;   J < 5;   J++ )
      {
         short   flag = 0;
         
         /*** Separador Campos ***/
         fprintf( stdout, "," );
      
         /*** Inteiro ***/
         for( flag = 0, I = 0;   I < 12;   I++ )   
         {
            /*** Para os 13 primeiros nao replique o ZERO ***/
            if ( flag || *P != '0' ) 
            { 
               flag = 1;
               fprintf( stdout, "%c", *P );
            }
            P++;
         }
      
         /*** O 13 caracter coloque-o ***/
         fprintf( stdout, "%c", *P++ );
      }
      
      
      /*** Varios Logicos ***/
      for( J = 0;   J < 54;   J++ )
      {
         /*** Separador Campos ***/
         fprintf( stdout, "," );
      
         if ( *P++ != '0' )
         {
            fprintf( stdout, "%c", SIM );
         
            if ( arqreg( J, L ) < 0 )
            {
               fprintf( stderr, "ERRO: arqreg %d - %ld - %d\n", J, L, errno );
               goto fim;
            }
         }
      }
      
      
      /*** Separador Campos ***/
      fprintf( stdout, "," );
      
      /*** Grupo ***/
      for( I = 0;   I < 2;   I++ )   fprintf( stdout, "%c", *P++ );
      
         
      /*** Varios Logicos ***/
      for( J = 54;   J < 57;   J++ )
      {
         /*** Separador Campos ***/
         fprintf( stdout, "," );
      
         if ( *P++ != '0' )
         {
            fprintf( stdout, "%c", SIM );
         
            if ( arqreg( J, L ) < 0 )
            {
               fprintf( stderr, "ERRO: arqreg %d - %ld - %d\n", J, L, errno );
               goto fim;
            }
         }
      }
      
      
      /*** Separador Campos ***/
      fprintf( stdout, "," );
      
      /*** Ocupacao ***/
      for( I = 0;   I < 3;   I++ )   fprintf( stdout, "%c", *P++ );
      
         
      /*** Separador Campos ***/
      fprintf( stdout, "," );
      
      /*** Delegacia ***/
      for( I = 0;   I < 5;   I++ )   fprintf( stdout, "%c", *P++ );
      
      /*** Final ***/
      fprintf( stdout, "\n" );
      
      
      if ( P - linha  !=  141 )
      {
         fprintf( stderr, "ERRO: P - linha != 141\n" );
         break;
      }
   }
   
   fprintf( stderr, "Final OK -> %ld Linhas!\n", L );
   
fim:   
   closefiles( );
}

