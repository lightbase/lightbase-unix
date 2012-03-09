#include  "lb2all.h"

#ifdef    VAXVMS
#include  <file.h>
#else
#include  <fcntl.h>
#endif

#include  <string.h>
#include  <time.h>

#ifdef UNIX
#include  <unistd.h>
#endif

#include  HD_IO

#ifdef    VAXVMS
#include  <types.h>
#include  <stat.h>
#else
#include  <sys/types.h>
#include  <sys/stat.h>
#endif


#include  "lb2stru.h"
#include  "lb2vars.h"
#include  "lb2text.h"  /* strings comus a todos os modulos */

#include  "lb2files.h"
#include  "lb2maces.h"
#include  "lb2prot1.h"


#define   H_RTLOCK      "DECLOCK"   /* decida o que fazer em lock */

#define   M_FANORMAL    loc_str(STR_GLB, 546)
#define   M_MOTIVO      loc_str(STR_GLB, 547)
#define   M_NDEBUG      loc_str(STR_GLB, 548)


#define   MMDDAA     0
#define   DDMMAA     1
#define   DATA_FORM  DDMMAA


TEXT *fim_anormal = NULL;

/*
**#  ---------------------------------------------------------------------
**#   Funcao...: imp_base
**#   Objetivo.: imprime nome da base no canto da tela
**/
F__GLB   VOID imp_base(nome)
TEXT *nome;
{  TEXT nomebase[TE+1];

   nomebase[0] = '\0';

   if ( nome != NULL )
      sprintf(nomebase, "%8s", nome );

   fwat(size_x - 8 , 0, curr_win);
   fwprintf(curr_win, nomebase);
   fwrefresh(curr_win);
}


/*
**#  ---------------------------------------------------------------------
**#   Funcao...: lib_ativa(sele)
**#   Objetivo.: fecha base e limpa nome da base da tela
**/
F__GLB   VOID lib_ativa( sele )
COUNT sele;
{
   sele = sele; /* ANANIAS so tira warnings */

	if ( WCB != NULL ) { /* existe base ativa  */
      close_base( );
      imp_base( str_vazia );
   }
   seta_acesso( NULL );  /* deve zerar acesso */
}


F__GLB   VOID fim_lb(texto)
TEXT * texto;
{  TEXT *p;

   if ( (p = malloc(strlen(texto)+1)) != NULL ) {
      strcpy(p, texto);
      fim_anormal = p;
   }
   else {
      p = texto;
   }

   ap_ignorauso = 9; /* usado para escapar do loop de erro/retentativas em lock_controle */

   encerra();

   exit( 99 );
}




F__GLB   VOID numdebug(num)
COUNT num;
{
   debug(M_NDEBUG, num);
}



F__GLB   TEXT *data_hora( flag , tempo )
COUNT flag;
LONG tempo;
{  static TEXT result[15];
   struct tm *timeptr;
   time_t secsnow;

   if ( tempo == 0 )
      time(&secsnow);
   else
      secsnow = (time_t) tempo;

   timeptr = localtime(&secsnow);

   if ( flag == 'H' ) { /* HORA */
      sprintf(result, "%2.2d:%2.2d",
                      timeptr->tm_hour,
                      timeptr->tm_min);

   } else { /* DATA */
      switch (DATA_FORM ) {
         case DDMMAA :  sprintf(result, "%2.2d/%2.2d/%2.2d",
                                       timeptr->tm_mday,
                                       (timeptr->tm_mon) + 1,
                                       timeptr->tm_year);
                        break;

         case MMDDAA :  sprintf(result, "%2.2d/%2.2d/%2.2d",
                                       (timeptr->tm_mon) + 1,
                                       timeptr->tm_mday,
                                       timeptr->tm_year);
                        break;

      }

   }

   return( &result[0] );
}


F__GLB   TEXT *conv_data( origem )
TEXT *origem;
{  static TEXT r[15];

   if ( strlen(origem) == 6 ) {
      /* converte de interno para externo */
      switch (DATA_FORM ) {
         case DDMMAA :
                        r[0] = origem[4];
                        r[1] = origem[5];
                        r[2] = '/';
                        r[3] = origem[2];
                        r[4] = origem[3];
                        r[5] = '/';
                        r[6] = origem[0];
                        r[7] = origem[1];
                        r[8] = '\0';
                        break;

         case MMDDAA :

                        r[0] = origem[2];
                        r[1] = origem[3];
                        r[2] = '/';
                        r[3] = origem[4];
                        r[4] = origem[5];
                        r[5] = '/';
                        r[6] = origem[0];
                        r[7] = origem[1];
                        r[8] = '\0';
                        break;
      }
   }
   else {
      /* converte de externo para interno */
      switch (DATA_FORM ) {
         case DDMMAA :
                        r[0] = origem[6];
                        r[1] = origem[7];
                        r[2] = origem[3];
                        r[3] = origem[4];
                        r[4] = origem[0];
                        r[5] = origem[1];
                        r[6] = '\0';
                        break;

         case MMDDAA :

                        r[0] = origem[6];
                        r[1] = origem[7];
                        r[2] = origem[0];
                        r[3] = origem[1];
                        r[4] = origem[3];
                        r[5] = origem[4];
                        r[6] = '\0';
                        break;
      }
   }

   return( r );
}





F__GLB   TEXT *conv_hora( origem )
TEXT *origem;
{  static TEXT r[15];

   if ( strlen(origem) == 4 ) {
      /* converte de interno para externo */
      r[0] = origem[0];
      r[1] = origem[1];
      r[2] = ':';
      r[3] = origem[2];
      r[4] = origem[3];
      r[5] = '\0';
   }
   else {
      /* converte de externo para interno */
      r[0] = origem[0];
      r[1] = origem[1];
      r[2] = origem[3];
      r[3] = origem[4];
      r[4] = '\0';
   }

   return( r );
}



F__GLB   SI lb_random(max)
COUNT max;
{
   return (int)(((long)rand()*max)/0x7FFF);
}


