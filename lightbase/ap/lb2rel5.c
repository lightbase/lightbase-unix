/*---------- lb2rel5.c ----------*/
#include  "lb2all.h"

#include  <string.h>
#include  HD_IO

#include  "lb2stru.h"
#include  "lb2vars.h"
#include  "lb2text.h"  /* strings comus a todos os modulos */

#include  "lb2entra.h"
#include  "lb2locs.h"
#include  "lb2rel.h"
#include  "lb2prot1.h"


#define   EXT_IMPR     ".lbp"


#ifdef MSDOS
   VOID imp_rowputs(FILE *, UC *);
#else
   VOID imp_rowputs( );
#endif
/* em val_device */
#define   H_RLDES    "RL_DEST"     /* val_device - destino do relatorio  */
#define   H_RLDRI    "RL_DRIVER"   /* val_device - driver de impressao   */
#define   H_NODRI    "NO_DRIVER"   /* val_device - driver nao existe ou nao acessivel  */


#define   M_DEV_NAME     loc_str( GRP_IMPREL, 41 )  /* "Nome :"                  */
#define   M_DEV_DRIV     loc_str( GRP_IMPREL, 42 ) /* "Tipo :"                  */
#define   M_DEV_NOARQ    loc_str( GRP_IMPREL, 43 ) /* "Informe nome do arquivo a gravar"        */
#define   M_DEV_NODRIV   loc_str( GRP_IMPREL, 44 ) /* "Informe o tipo de impressora a simular"  */
#define   E_DEV_DESBR    loc_str( GRP_IMPREL, 45 )  /* "Nome destino em branco"  */
#define   E_DEV_DRIVBR   loc_str( GRP_IMPREL, 46 )  /* "Nome driver  em branco"  */
#define   E_DEV_ACESS    loc_str( GRP_IMPREL, 47 )  /* driver nao acessivel ou nao existe */






/*---------- val_device ----------*/

F__LOC   SS val_device( entry )
d_entry *entry;
{
   SS ret = 0, flag = 0;
   WIN   *j1 = NULL;
   DTela cm[ 3 ];
   SC aux[ PATH_MAX + 1 ];

   j1 = abre_janela(J_DEVICE);
	fwbox(j1);


	if ( entry->tecla == K_ESC ) {
		ret = ACEITA;
		goto fim;
	}

   mvwprintw(j1 , 0,  1, M_DEV_NAME );
   mvwprintw(j1 , 1,  1, M_DEV_DRIV );

   seta_dtela(&cm[0], c_impress.destino, x50, M_DEV_NOARQ,   0, 9, 30, E_ALFA,  H_RLDES, NULL,  NULL, NULL );
   seta_dtela(&cm[1], c_impress.driver,  x14, M_DEV_NODRIV,  1, 9,  0, E_ALFA,  H_RLDRI, NULL,  NULL, NULL );
   cm[2].destino = NULL;


   while ( 1 ) {

      if ( (strcmp( entry->str, M_OUT_FILE ) == 0) || flag ) {

         ret = ed_read(j1, cm, 1, MOSTRA_LE, LF_13E);

         if ( ret < 0 || ret == K_ESC   ) {
            ret = REJEITA;
            goto fim;
         }


         if ( c_impress.destino[ 0 ] == '\0')  {
            mens_erro( str_nada, E_DEV_DESBR);
            continue;
         }

         if ( c_impress.driver[ 0 ] == '\0' )  {
            mens_erro( str_nada, E_DEV_DRIVBR );
            continue;
         }
      }
      else {
         SS I;
         TEXT aux2[TD+1], *pf;

         pf = "%d %d %d %d %d %d";
         I = entry->piece - 1;

         piece(ap_vetprinter[I], c_impress.driver,  '@', 2, TE);
            /* implementacao para pegar nome de driver de variavel global */
            if ( c_impress.driver[0] == '$' ) {
               TEXT *p;
               if ( (p = getenv(&c_impress.driver[1])) != NULL ) {
                  if ( strlen(p) < sizeof(c_impress.driver) ) {
                     strcpy(c_impress.driver, p);
                  }
               }
            }
         piece(ap_vetprinter[I], c_impress.destino, '@', 3, TD);
            /* implementacao para pegar nome de destino de variavel global */
            if ( c_impress.destino[0] == '$' ) {
               TEXT *p;
               if ( (p = getenv(&c_impress.destino[1])) != NULL ) {
                  if ( strlen(p) < sizeof(c_impress.destino) ) {
                     strcpy(c_impress.destino, p);
                  }
               }
            }
         piece(ap_vetprinter[I], c_impress.comando, '@', 4, TD);
            /* implementacao para pegar nome de comando de variavel global */
            if ( c_impress.comando[0] == '$' ) {
               TEXT *p;
               if ( (p = getenv(&c_impress.comando[1])) != NULL ) {
                  if ( strlen(p) < sizeof(c_impress.comando) ) {
                     strcpy(c_impress.comando, p);
                  }
               }
            }
         c_impress.escrava[0] = 0; c_impress.escrava[6] = 0;
         c_impress.video[0] = 0;   c_impress.video[6] = 0;

         piece(ap_vetprinter[I], aux2, '@', 5, TD);
         if ( aux2[0] != '\0' )
            sscanf(aux2, pf, &c_impress.escrava[0], &c_impress.escrava[1],
                             &c_impress.escrava[2], &c_impress.escrava[3],
                             &c_impress.escrava[4], &c_impress.escrava[5]);

         piece(ap_vetprinter[I], aux2, '@', 6, TD);
         if ( aux2[0] != '\0' )
            sscanf(aux2, pf, &c_impress.video[0], &c_impress.video[1],
                             &c_impress.video[2], &c_impress.video[3],
                             &c_impress.video[4], &c_impress.video[5]);
      }

      if ( strchr( c_impress.driver, '.' ) != NULL ) {
         SC *p;

         p = c_impress.driver;

         while( *p != '.' ) p++ ;

         *p = '\0';
      }

      info_base( c_impress.driver, ap_home_lb, EXT_IMPR, aux );

      if ( access(aux, 04) != 0 )  {
         mens_erro( H_NODRI, E_DEV_ACESS, aux);
         flag = 1;
         continue;
      }
      break;
   }

fim:

   if ( j1 != NULL )    fwkill( j1 );

   return ( ret );
}


F__GLB   VOID imp_char( c )
UC c;
{

   if ( device == NULL ) numdebug( 2003 );

   if ( c == 255 )  {
      /* nada */
   }
   else

      if ( c >= 128 )   {
         UC *seq, type;

         type = IMP[ c - 128 ].type;
         seq  = IMP[ c - 128 ].ary;

         if ( type == tipo_corrente ) {
            imp_rowputs( device, seq );
         }
         else  {
            if ( type != 0 ) {
               if ( I_SE[ tipo_corrente ].end[0] != ' ' || I_SE[ tipo_corrente ].end[1] != '\0')
						imp_rowputs( device, I_SE[ tipo_corrente ].end );
               if ( I_SE[ type ].start[0] != ' ' || I_SE[ type ].start[1] != '\0')
	               imp_rowputs( device, I_SE[ type ].start );
               tipo_corrente = type;
               imp_rowputs( device, seq );
            }

         }
      }
   else {
      if ( tipo_corrente != 0 ) {
         if ( I_SE[ tipo_corrente ].end[0] != ' ' || I_SE[ tipo_corrente ].end[1] != '\0')
	         imp_rowputs( device, I_SE[ tipo_corrente ].end );
         if ( I_SE[ 0 ].start[0] != ' ' || I_SE[ 0 ].start[1] != '\0')
         	imp_rowputs( device, I_SE[ 0 ].start );
         tipo_corrente = 0;
      }
      fputc( c, device );
   }

}

/*---------- imp_puts( ) ----------*/

F__GLB   VOID imp_puts( fp, str )
FILE *fp;
UC   *str;
{
   UC  *P;

   for ( P = str;   *P != 0;   P++ )
   {
      if ( *P == 255 )   fputc( 0, fp );
      else               imp_char( *P );
   }
}

/*---------- imp_rowputs( ) ----------*/
F__GLB   VOID imp_rowputs( fp, str )
FILE *fp;
UC   *str;
{
   UC  *P;

   for ( P = str;   *P != 0;   P++ )
   {
      if ( *P == 255 )   fputc( 0, fp );
      else               fputc( *P, fp );
   }
}
