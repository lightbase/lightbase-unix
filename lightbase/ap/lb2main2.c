#include  "lb2all.h"

#include  <stdlib.h>
#include  <string.h>
#include  <ctype.h>

#ifdef MSDOS
#  include  <time.h>
#  include  <dos.h>
#  include  <bios.h>
#endif

#ifdef PROT_INFOCON
#   include	<is_pers.h>
#endif


#include  "lb2stru.h"
#include  "lb2vars.h"
#include  "lb2text.h"  /* strings comus a todos os modulos */

#include  "lb2files.h"
#include  "lb2head.h"
#include  "lb2prot1.h"
#include  "lb2prot.h"

#define   GRP_LOGO   11

#define   SIZE_IOTXT    20000L   /* tamanho do buffer para io de TEXTO e VET's (lb2vetut.c)*/

#define   TOP0   loc_str(GRP_LOGO, 14)
#ifndef LITE
#   define   TOP1   loc_str(GRP_LOGO, 15)
#else
#  ifdef RUNTIME
#     define   TOP1   loc_str(GRP_LOGO, 21)
#  else
#     define   TOP1   loc_str(GRP_LOGO, 20)
#  endif
#endif
#define   TOP2   loc_str(GRP_LOGO, 16)
#define   TOP3   loc_str(GRP_LOGO, 17)

#define   BOT1   loc_str(GRP_LOGO, 18)
#define   BOT2   loc_str(GRP_LOGO, 19)


#define   PRZ_CONCEDIDO   loc_str(GRP_LOGO, 30)

#ifndef LITE
#  define   PRZ_DEFAULT     loc_str(GRP_LOGO, 31)
#else
#  define   PRZ_DEFAULT     loc_str(GRP_LOGO, 46)
#endif

#define   LIM_REGS        loc_str(GRP_LOGO, 32)

#define   DATA_LIMITE     loc_str(GRP_LOGO, 33)
#define   DIAS_REST       loc_str(GRP_LOGO, 34)

#define  DEMO1        loc_str(GRP_LOGO, 35)
#define  DEMO2        loc_str(GRP_LOGO, 36)
#define  DEMO22        loc_str(GRP_LOGO, 45)
#define  DEMO3        loc_str(GRP_LOGO, 37)
#define  DEMO4        loc_str(GRP_LOGO, 38)
#define  DEMOX        loc_str(GRP_LOGO, 39)

#define  DEF1         loc_str(GRP_LOGO, 40)
#define  DEF2         loc_str(GRP_LOGO, 41)
#define  DEF3         loc_str(GRP_LOGO, 42)
#define  DEF4         loc_str(GRP_LOGO, 43)
#define  DEFX         loc_str(GRP_LOGO, 44)

#define   M_FANORMAL    loc_str(STR_GLB, 546)
#define   M_MOTIVO      loc_str(STR_GLB, 547)

extern TEXT     ap_term[];
extern TEXT     ap_cliente[];   /* Nome do cliente */
extern TEXT     lbsep_int;
extern TEXT     lbsep_dec;
extern TEXT     *fim_anormal;
extern TEXT     ap_dirlock[];  /* diretorio de lock */

COUNT    ap_autonum;
COUNT    ap_xxdebug;


F__GLB   VOID imp_logo(flag)
COUNT flag;
{

/*****
****          ****  ************  ****    ****  ************
****          ****  ************  ****    ****  ************
****          ****  ****          ****    ****      ****
****          ****  ****   *****  ************      ****
****          ****  ****   *****  ************      ****
****          ****  ****     ***  ****    ****      ****
************  ****  ************  ****    ****      ****
************  ****  ************  ****    ****      ****

       ****          ****  ************  ****    ****  ************
      ****          ****  ************  ****    ****  ************
     ****          ****  ****          ****    ****      ****
    ****          ****  ****   *****  ************      ****
   ****          ****  ****   *****  ************      ****
  ****          ****  ****     ***  ****    ****      ****
 ************  ****  ************  ****    ****      ****
************  ****  ************  ****    ****      ****
*****/

   COUNT i, tam=67;
   TEXT brancos[80];

   up_str(GRP_LOGO);

   memset(brancos, ' ', tam);
   brancos[tam] = '\0';

   for ( i=1 ; i<11 ; i++) {
      fwat(7, i+7 , curr_win);
      fwprintf( curr_win, (flag == 1 ) ? loc_str(GRP_LOGO, i) : brancos);
   }

   down_str(GRP_LOGO);

   fwrefresh(curr_win);
}

F__GLB   VOID sem_memoria()
{  COUNT i;

   if ( curr_win != NULL )
      close_base();

   for (i = 0; i < 10 ; i++)
      fprintf(stderr, "\n");

#ifdef ESPANHA
   ... fprintf(stderr, "Erro critico de falta de memoria convencional\n");
#else
   fprintf(stderr, "Erro critico de falta de memoria convencional\n");
#endif

   for (i = 0; i < 10 ; i++)
      fprintf(stderr, "\n");

   exit(1);
}


F__GLB   COUNT encerra()
{  COUNT x=0, i;
   TEXT aux[100], *p1, alt, larg;
#ifdef PROT_INFOCON
   int aa, dd, mm;
#else
   extern COUNT dias_restantes;
   extern TEXT data_limite[];
#endif
   extern COUNT sai_stat;
#ifdef MSDOS
   VOID ABORT_TRANS(VOID);
#else
   VOID ABORT_TRANS();
#endif




   alt = 14;
   larg = 42;

   close_base( );

   ABORT_TRANS();

#ifdef MSDOS
	   curr_win->w_atrib  = 0;
	   curr_win->w_optatr = 0;
	   curr_win->w_defatr = JP_DESTAQUE;
	   curr_win->w_boxatr = 0;
		curr_win->t_atr    = 0;
		curr_win->campoatr = JP_CAMPO;
      fwcls(curr_win);
#endif

   fwtop(curr_win);
   fwcls(curr_win);
   fwrefresh(curr_win);

   fwframe(1, 0, 1 + alt, 0 + larg, 2, curr_win);


   up_str(GRP_LOGO);


   fwat(2, 2, curr_win);     fwprintf(curr_win, TOP0);
      fwat(32, 2, curr_win);     fwprintf(curr_win, " # %06.6l", ap_nserie);

   fwat(2, 3, curr_win);     fwprintf(curr_win, TOP1);
   fwat(2, 4, curr_win);     fwprintf(curr_win, TOP2);
   fwat(2, 5, curr_win);     fwprintf(curr_win, TOP3);


   fwat(0, 6, curr_win);
   fwpc(148, curr_win);
   for ( i = 1; i < larg; i++)
      fwpc(142, curr_win);
   fwpc(150, curr_win);

   if ( strcmp(ap_aprovado, C_OK) != 0 ) {
      /* copia demonstracao */
      switch (sai_stat) {
         case 1001 : p1 = PRZ_CONCEDIDO;
                          break;
         case 1002 :
                     p1 = PRZ_DEFAULT;
                          break;
         case 1003 : p1 = LIM_REGS;
                          break;

#ifdef PROT_INFOCON
         default   : if ( pe_data(&aa,&mm,&dd) != NULL )
                        sprintf(aux, "%s %2.2d/%2.2d/%2.2d ", DATA_LIMITE, dd, mm, aa);

                     if ( ap_nserie == 1 )
                        aux[0] = '\0';

                     p1 = aux;
#else
         default   : if ( data_limite[0] != '\0' )
                        sprintf(aux, "%s %s", DATA_LIMITE, data_limite);
                     else
                        if ( ap_nserie == 1 )
                           aux[0] = '\0';
                        else
                           sprintf(aux, "%s %d", DIAS_REST, dias_restantes);
                     p1 = aux;
#endif

      }

      fwat(2,  7, curr_win);     fwprintf(curr_win, DEMO1);
#ifdef PROT_INFOCON
		if ( strcmp(pe_pers(), "nome") == 0 ) {
	      fwat(2,  8, curr_win);     fwprintf(curr_win, DEMO22);
	   }
	   else {
	      fwat(2,  8, curr_win);     fwprintf(curr_win, DEMO2);
	   }
#else
      fwat(2,  8, curr_win);     fwprintf(curr_win, DEMO2);
#endif
      fwat(2,  9, curr_win);     fwprintf(curr_win, DEMO3);
      fwat(2, 10, curr_win);     fwprintf(curr_win, p1);
      fwat(2, 11, curr_win);     fwprintf(curr_win, DEMOX);
   }
   else {

      if ( strlen(ap_cliente) > (larg - 2) )
         ap_cliente[larg - 3] = '\0';

      x = larg - strlen(ap_cliente);
      if ( x > 1 )
         x = x/2 - 1;


      fwat(2,  7, curr_win);     fwprintf(curr_win, DEF1);
      fwat(2,  8, curr_win);     fwprintf(curr_win, DEF2);
      fwat(2,  9, curr_win);     fwprintf(curr_win, DEF3);
      fwat(2+x, 10, curr_win);   fwprintf(curr_win, ap_cliente);
      fwat(2, 11, curr_win);     fwprintf(curr_win, DEFX);
   }



   fwat(0, 12, curr_win);
   fwpc(148, curr_win);
   for ( i = 1; i < larg; i++)
      fwpc(142, curr_win);
   fwpc(150, curr_win);


   fwat(2, 13, curr_win);     fwprintf(curr_win, BOT1);
   fwat(2, 14, curr_win);     fwprintf(curr_win, BOT2);


   if ( fim_anormal != NULL ) {
      fwat(0, 17, curr_win);     fwprintf(curr_win, "%3A%s%1A", M_ATENCAO);
      fwat(1, 18, curr_win);     fwprintf(curr_win, M_FANORMAL);
      fwat(1, 19, curr_win);     fwprintf(curr_win, "%s : %s",
                                           M_MOTIVO, fim_anormal);
      fwrefresh(curr_win);
      W_curmove(0, 21);
      doflush();
   }
   else {
      fwrefresh(curr_win);
      W_curmove(0, 17);
      doflush();
   }

#ifdef DISCADOR
   cli_fim();
#endif

   return( 1 );
}


F__GLB   COUNT config()
{  COUNT ret = 0, i, linha;
   FILE *fp = NULL;
   TEXT aux[100], temp[80], token[4];
   UTEXT *p;

#ifdef MSDOS
   strcpy(ap_term, "dos_ms.lbt");
#else
   strcpy(ap_term, "cons_xen.lbt");
#endif

   ap_pos_index = NOW_INDEX;
   ap_autosave  = 0;
   ap_sinonimos = 0;
   ap_fonetica  = 0;
   ap_psqslot   = 500;
   ap_flushfile = 1;
   ap_maxprinter= 0;
   ap_maxmatriz = 10;
   ap_maxitens  = 100;
   ap_delitables = '|';
   ap_critica   = 0;
   lbsep_int    = '.';
   lbsep_dec    = ',';
   ap_sizeio    = SIZE_IOTXT;
   ap_mxitens   = MAX_ELEM;  /* maximo de itens de vista/relatorio */
   ap_mxbases   = MAX_BASES;
   ap_autonum   = 0; /* variavel para forcar geracao de autonum imediatamente apos entrar no campo */

   par_cfg.word_len = 2;


   strcpy(ap_dirbase, ap_home_lb);
   strcpy(ap_back_lb, "lb_back");
   strcpy(ap_rest_lb, "lb_rest");
   strcpy(ap_sort_lb, "sort");

   ret = info_base(FILE_CONFIG, ap_home_lb, EXT_CONFIG, aux);

   if ( (ret <= 0 ) || (fp = fopen(aux, "r")) == NULL ) {
      ret = -1;
      goto fim;
   }

   linha = 0;

   while ( fgets( aux, sizeof(aux) - 1, fp ) != NULL ) {

      linha++;

      if ( aux[0] != '-' ) continue;

      for ( p = (UTEXT *)aux; *p != '\0'; p++ ) {
         if ( *p < 32 ) *p = ' ';
         if ( *p == ';' ) {
            *p = '\0';
            break;
         }
      }

      trim(aux);

      token[0] = (TEXT) toupper(aux[0]);
      token[1] = (TEXT) toupper(aux[1]);
      token[2] = (TEXT) toupper(aux[2]);
      token[3] = '\0';

      if ( strncmp(token, "-UN", 3) == 0 ) {
#ifdef VIAS_IMPLICITAS
         strncpy(ap_cliente, &aux[3], 40);
#else
#ifdef ESPANHA
          ....fprintf(stderr, "\n%s\n", "-UN. Diretiva invalida. Para personalizar contacte seu revendedor");
#else
          fprintf(stderr, "\n%s\n", "-UN. Diretiva invalida. Para personalizar contacte seu revendedor");
#endif
#endif
         continue;
      }

      /* alguns debugs genericos */

      if ( strncmp(token, "-XX", 3) == 0 ) {
      	 ap_xxdebug = atoi( &aux[3] );
         continue;
      }


      if ( strncmp(token, "-DB", 3) == 0 ) {
         strncpy(ap_dirbase, &aux[3], PATH_MAX);
         continue;
      }

      if ( strncmp(token, "-DL", 3) == 0 ) {
         strncpy(ap_dirlock, &aux[3], PATH_MAX);
         continue;
      }

      if ( strncmp(token, "-TT", 3) == 0 ) {
         strncpy(ap_term, &aux[3], TE);
         continue;
      }

      if ( strncmp(token, "-LG", 3) == 0 ) {
         if (aux[3] >= '0' && aux[3] <= '2' )
            par_cfg.tipo_login = aux[3] - '0';
         continue;
      }

      if ( strncmp(token, "-WL", 3) == 0 ) {
         if (aux[3] >= '0' && aux[3] <= '9' )
            par_cfg.word_len = aux[3] - '0';
         else
            par_cfg.word_len = 2;
         continue;
      }


      if ( strncmp(token, "-AS", 3) == 0 ) {
         if ( aux[3] == '-' ) ap_autosave = 0;
         else                 ap_autosave = 1;
         continue;
      }


      if ( strncmp(token, "-AN", 3) == 0 ) {
         if ( aux[3] == '-' ) ap_autonum = 0;
         else                 ap_autonum = 1;
         continue;
      }

#ifdef SINONIMOS
      if ( strncmp(token, "-SI", 3) == 0 ) {
         if ( aux[3] == '-' ) ap_sinonimos = 0;
         else                 ap_sinonimos = 1;
         continue;
      }
#endif

      if ( strncmp(token, "-IN", 3) == 0 ) {
         if ( aux[3] == '-' ) ap_pos_index = POS_INDEX;
         else                 ap_pos_index = NOW_INDEX;
         continue;
      }

      if ( strncmp(token, "-SL", 3) == 0 ) {
         if ( atoi(&aux[3]) > 0 ) ap_psqslot = atoi(&aux[3]);
         continue;
      }

      if ( strncmp(token, "-WR", 3) == 0 ) {
         if ( atoi(&aux[3]) > 0 ) ap_flushfile = (COUNT) atoi(&aux[3]);
         continue;
      }

      if ( strncmp(token, "-IO", 3) == 0 ) { /* buffer de io */
         if ( atol(&aux[3]) > 4000 &&  atol(&aux[3]) <= 65000L)
            ap_sizeio = (UCOUNT)atol(&aux[3]);
         continue;
      }


      if ( strncmp(token, "-MT", 3) == 0 ) {
         ap_delitables = aux[3];  /* delimitador da tabela */
         if ( atoi(&aux[4]) > 0 ) ap_maxmatriz = (COUNT) atoi(&aux[4]);
         continue;
      }

      if ( strncmp(token, "-MI", 3) == 0 ) {
         /* Numero maximo de itens da tabela */
         if ( atoi(&aux[3]) > 0 ) ap_maxitens = atoi(&aux[3]);
         continue;
      }

      if ( strncmp(token, "-PI", 3) == 0 ) {
         if (aux[3] == '.' || aux[3] == ',' )
            lbsep_int = aux[3];
         continue;
      }

      if ( strncmp(token, "-PD", 3) == 0 ) {
         if (aux[3] == '.' || aux[3] == ',' )
            lbsep_dec = aux[3];
         continue;
      }

      if ( strncmp(token, "-CR", 3) == 0 ) {
         if (aux[3] == 'x' || aux[3] == 'X' )
            ap_critica = 'X';
         continue;
      }

      if ( strncmp(token, "-BK", 3) == 0 ) {
         strncpy(ap_back_lb, &aux[3], TD);
         continue;
      }

      if ( strncmp(token, "-RS", 3) == 0 ) {
         strncpy(ap_rest_lb, &aux[3], TD);
         continue;
      }

      if ( strncmp(token, "-CL", 3) == 0 ) {
         strncpy(ap_sort_lb, &aux[3], TD);
         continue;
      }

      if ( strncmp(token, "-LN", 3) == 0 ) {
         p = (UTEXT *)&aux[3];

         OP_IDIOMAS[0] = '\0';

         for ( i = 1; i <= MAX_IDIOMAS; i++ ) {

            if ( piece((TEXT *)p, temp, '@', i, TE) == 0 ) {
               break;
            }

            trim(temp);
            ltrim(temp);

            strcpy( ap_tabidioma[i-1], temp);

            if ( i > 1 ) strcat(OP_IDIOMAS, "@");
            strcat(OP_IDIOMAS, temp);
         }

         continue;
      }

      if ( strncmp(token, "-ST", 3) == 0 ) {
         /* nao faz nada so para compatibilizar com versoes anteriores */
         continue;
      }

      if ( strncmp(token, "-CI", 3) == 0 ) {
         /* nao fax nada para compatibilizar com versao anterior */
         continue;
      }

      if ( strncmp(token, "-ME", 3) == 0 ) {
         if ( atoi(&aux[3]) > 0 ) ap_mxitens = (COUNT) atoi(&aux[3]);
         continue;
      }

      if ( strncmp(token, "-MB", 3) == 0 ) {
         if ( atoi(&aux[3]) > 0 ) ap_mxbases = (COUNT) atoi(&aux[3]);
         continue;
      }

      if ( strncmp(token, "-LP", 3) == 0 ) {
         COUNT n;
         TEXT *p1;

         p1 = &aux[3];
         p = (UTEXT *)p1;

         n=0;  /* conta numero de '@'. Minimo de 2 */
         while(*p1)
            if (*p1++ == '@' ) n++;

         if (  n >= 2 && ap_maxprinter < (MAX_PRINTS - 1)) {

            if ((p1 = malloc(strlen(&aux[3]) + 1 )) == NULL ) {
#ifdef ESPANHA
               ...fprintf(stderr, "\n%s\n", "Sem Memoria");
#else
               fprintf(stderr, "\n%s\n", "Sem Memoria");
#endif
               ret = -1;
               goto fim;
            }

            ap_vetprinter[ap_maxprinter] = p1;
            strcpy(ap_vetprinter[ap_maxprinter], &aux[3]);
            ap_maxprinter++;
         }
         else {
#ifdef ESPANHA
            ...fprintf(stderr, "\nErro na linha : %d do arquivo config.lb", linha);
#else
            fprintf(stderr, "\nErro na linha : %d do arquivo config.lb\n", linha);
#endif
         }

         continue;
      };
   }

   ret = 0;


   /* Aloca estruturas para tabelas de formulas */
   if ( ap_maxmatriz == 0 )
      ap_maxmatriz =10;

   ap_vetmatriz = (Tables *) calloc( ap_maxmatriz, sizeof(Tables));
   if ( ap_vetmatriz == NULL )
      ret = -1;


fim :

   if ( fp != NULL ) fclose ( fp );

   return(ret);
}

