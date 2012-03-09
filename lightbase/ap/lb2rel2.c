/*----------- lb2rel2.c ----------*/
#include  "lb2all.h"

#include  <string.h>

#include  HD_DIR

#include  "lb2stru.h"
#include  "lb2vars.h"
#include  "lb2text.h"  /* strings comus a todos os modulos */

#include  "lb2bases.h"
#include  "lb2entra.h"
#include  "lb2locs.h"
#include  "lb2prot1.h"

#include  "lb2rel.h"


/*--------------------------------------------------------------------*/
#define   H_RDDEST        "RD_DEST"
#define   H_RESALTN       "RE_SALTN"
#define   H_DEF_ETIQ      "DEF_ETIQ"

#define   M_ETI_RDISP     loc_str( GRP_IMPREL, 17)
#define   M_ETI_RSALT     loc_str( GRP_IMPREL, 18)
#define   M_ETI_CARAC     loc_str( GRP_IMPREL, 19)
#define   M_ETI_TDISP     loc_str( GRP_IMPREL, 20)
#define   M_ETI_TSALT     loc_str( GRP_IMPREL, 21)

#define   E_PR( l, c )   ((l * tam_linha) + c)

#ifdef MSDOS
   SS val_device( d_entry *);
   VOID escrava_off(VOID);
   VOID escrava_on(VOID);
#else
   SS val_device( );
   VOID escrava_off();
   VOID escrava_on();
#endif


/*---------- Prototipos ----------*/
#ifdef MSDOS
   SS    carac_etiq( VOID );
   SS    etq_itens( SS, SS, WIN *);
   SS    etq_item_mat( SS, RTela * );
   SS    etq_imp_pag ( VOID );
   SS    etq_teste_prn( VOID );
#else
   SS    carac_etiq( );
   SS    etq_itens( );
   SS    etq_item_mat( );
   SS    etq_imp_pag ( );
   SS    etq_teste_prn( );
#endif



/*--------------------------------------------------------------------*/
/*  estaticas                                                         */
/*--------------------------------------------------------------------*/
static SC destino[ TD + 1 ];     /* MANEL ??? */
static SC saltar_n[ TD + 1 ];
static SS tam_linha;
static SC *mat_etiq;
static SS coluna, col_atual, qtd_etiqueta;

extern SI   errno;

/*--------------------------------------------------------------------*/
/*  funcoes                                                           */
/*--------------------------------------------------------------------*/
F__GLB   SS rel_etiq( sele, pfun)
SS sele;
#ifdef MSDOS
   SL (*pfun)( SS, SL );
#else
   SL (*pfun)( );
#endif
{
   SS    ret, i,  tam_matriz, ordena;
   SL    conta_reg;
   SL    reg;
   RTela **pitens;
   WIN   *jm;
   TEXT mensagem[80];
   SC    nome_file[PATH_MAX + 20];

   if ( pfun == NULL )
      pfun = prox_reg;

   device    = NULL;
   mat_etiq  = NULL;
   jm        = NULL;
   qtd_etiqueta = 0;

   jm = abre_mensagem( str_nada );

   pitens =  (RTela **)prel->rel_vet->vi_vtit;

   qsort( (TEXT *) pitens, prel->rel_n_elem, sizeof( RTela * ),
          (int (* )()) ord_itrel );

   tam_linha   = ( prel->rel_colunas * prel->rel_n_etq );

   tam_linha  += (prel->rel_n_etq - 1) * prel->rel_c_inter;

   tam_matriz  = ( tam_linha * prel->rel_linhas ) + 2;

   /* oitenta e folga para nao invadir quando incluir campo em rodape*/
   if ((mat_etiq = malloc((tam_matriz + 80) * sizeof( SC ))) == NULL ) {
      ret = -3;
      goto fim;
   }

   if ( carac_etiq( ) < 0 ) { ret = 0;   goto fim; }

   traduz_comandos();

   if (prel->rel_tipo != FORMATADO )
      grel->replica_txt = 0;

   if ( (ordena = trata_ordenacao(sele, pfun, jm) ) < 0 ) {
      ret = -1;
      goto fim;
   }

   mens_1( jm, M_EMI_IMPREL );
   fwrefresh( jm );
   fwsetcursor( jm );


   strcpy(nome_file, c_impress.destino);
   i = strlen(nome_file) - 1;
   if ( nome_file[i] == '%' ) {
      nome_file[i] = '\0';
      strcat(nome_file, ARQ_SPOOL);

      if ( (char *)mktemp( nome_file ) == NULL ) {
         ret = -4;
         goto fim;
      }
   }

   if ( (device = fopen(nome_file, M_ESCRITA) ) ==  NULL ) {
      ret = -4;
      goto fim;
   }

   /* carrega driver */
   if ( i_find_params('C') < 0 ) {   ret = -5; goto fim; }

   /**  inicializa matriz de etiquetas */
   memset( mat_etiq, ' ', tam_matriz);
   mat_etiq[tam_matriz-1]   = '\0';


   inic_printer( );

   coluna = 1;
   col_atual = 0;

   if ( etq_teste_prn( ) < 0 )  {
      ret = 0;
      goto fim;
   }

   reg  = 0L;
   ret  = 1;
   conta_reg = 0;
   while ( 1 )   {

      COUNT salto_quebra;

      reg = (ordena == SIM) ? push_linha( POP, str_nada ) : (*pfun)( sele, reg ) ;

      if ( reg <= 0 )
         break;

      if ( pfun != reg_corrente) {
         ret = read_record(sele, reg, 0);
         if ( ret != 0 ) {
            /* pode pegar registro NOVO/LIBERADO em read_record */
            if ( ret == 2 || ret == 4 ) {
               ret = 0;
               continue;
            }
            else {
               ret = -99;
               goto fim;
            }
         }
      }

      qtd_etiqueta++;

      if ( qtd_etiqueta < atoi( saltar_n ) )     continue;

      if ( ordena == SIM )   {
         salto_quebra = quebra_reg( COMPARA, sele, 0 );

         if( salto_quebra == -1 )  {  ret = -1;  goto fim;  }
      }

      conta_reg++;
      sprintf(mensagem, "%s %ld", M_EMI_MENS, conta_reg);

#ifndef ULTRIX
      mens_1(jm, mensagem);
      fwrefresh( jm );
      fwsetcursor( jm );
#endif

      limpa_flag( pitens, prel->rel_n_elem );

      for ( i = 0;  i <= prel->rel_linhas; i++ ) {

         ret = etq_itens( i, sele, jm );

         /* usuario interrompeu */
         if (ret == 1)    break;

         if ( ret < 0 ) {   ret = -1;  goto fim;  }
      }

      if (ret == 1 )    break;

      if ( ( coluna % prel->rel_n_etq) == 0 ) {
         etq_imp_pag ( );
         memset( mat_etiq, ' ', tam_matriz);
         mat_etiq[tam_matriz-1] = '\0';
         coluna = 1;
         col_atual = 0;
      }
      else  {
         coluna++;
         col_atual += prel->rel_colunas+prel->rel_c_inter;
      }
   }

   if ( mat_etiq != NULL && coluna > 1 )
      etq_imp_pag( );

fim:

   if( ret < 0 ) {
      switch( ret ) {
         case -1 :
            mens_erro(H_IMPREL, E_IMPREL, ret);
            break;
         case -3 :
            mens_erro(H_NOMEMO, SEM_MEMO);
            break;
         case -4 :
            mens_erro(H_OPESO, E_OPE_SO, errno, nome_file);
            break;
         case -5 :
            mens_erro(H_OPDRV, E_OPDRV, driver);
            break;
         case -99 :
            mens_erro(H_RDSO, E_RD_SO, errno, nome_file);
            break;
      }
   }

   if ( device != NULL ) term_printer( );

   i_find_params('F');

   if (device != NULL )  {
      term_printer( );
      fclose ( device );
      if ( c_impress.comando[0] != '\0' ) {
         TEXT aux[200];
         sprintf( aux, c_impress.comando, nome_file);
         win_shell( aux );
      }
   }

   if (ordena == SIM)  push_linha( FINALIZA, NULL );

   if ( mat_etiq != NULL )     free( mat_etiq );

   fecha_mensagem(jm);

   return ( ret );
}

F__LOC   COUNT carac_etiq( )
{
   COUNT ret, campo;
   WIN   *jf, *j2;
   TEXT  *lb_impress;     /* elementos separados por '@' */
   DTela cm[ 3 ];

   ret       = 0;

   j2 = abre_janela(J_CARETIQ);
   jf = abre_funcoes(funcao(LF_13E));

   if ( (lb_impress = malloc(MAX_PRINTS * TE + 10)) == NULL ) {
      ret = -1;
      goto fim;
   }


   mont_vt_impr( lb_impress );

   seta_dtela(&cm[0], destino, x14, M_ETI_RDISP,  1, 27, 0,
              E_LISTA,  H_RDDEST, lb_impress,  NULL, val_device);
   seta_dtela(&cm[1], saltar_n,  x14, M_ETI_RSALT,  2, 27, 0,
              E_ALFA,  H_RESALTN, NULL,  NULL, NULL );
   cm[2].destino = NULL;


   fw_title(j2, M_ETI_CARAC, j2->w_atrib);

   fwbox(j2);

   mvwprintw(j2 , 1,  1, M_ETI_TDISP );
   mvwprintw(j2 , 2,  1, M_ETI_TSALT);

   fwrefresh(j2);


   campo = 1;

   c_impress.destino[ 0 ] = '\0';
   c_impress.driver[ 0 ]  = '\0';

le:

	ret = ed_read(j2, cm, campo, MOSTRA_LE, LF_13E);

   campo = w_lastread;

	switch (ret) {
      case K_ESC  : ret = -1;   goto fim;
		case K_UP   :
		case K_PGUP :
		case K_DN   :
      case K_F0   :
		case K_PGDN : goto le;

		case K_ENTER:
      default     : ret = 0;   break;

      case K_F2   : numdebug( 2004 );
                    break;
	}

fim:

   if ( lb_impress != NULL ) free(lb_impress);
	fecha_funcoes(jf);
	fwkill(j2);

   return (ret);
}


/*=========================================[ etq_itens () ]===========*/
F__LOC   SS etq_itens( l, sele, jm )
SS l, sele;
WIN *jm;
{
   SS cont, ret;
   RTela *prt;

   for ( cont = 0; cont < prel->rel_n_elem; cont++ ) {

      if ( w_kbhit(jm) && fwnqgc(jm) == K_ESC ) {
         if ( decida(TW_RL_D00, TW_RL_D99, H_CFCAN,
                     2, 30, "%s\n%s", TW_RL_D01, TW_RL_D02 ) == 1 ) {
            ret = 1;
            goto fim;
         }
      }

      prt = (RTela *)prel->rel_vet->vi_vtit[cont];   /* aponta para cont elemento */

      if ( prt->re_status == JA_TRATOU ) continue;

      if ( prt->re_linha != l )  continue;

      prt->re_status = JA_TRATOU;

      etq_item_mat( sele, prt );

   }

   ret = 0;

fim:

   if ( cont < 0 )
      ret = -1;

   return( ret );
}

F__LOC   SS etq_item_mat( sele, prt )
SS sele;
RTela *prt;
{
   SS lin, col, tam;
   SC    *p_display, *p_campo;

   lin = prt->re_linha;
   col = col_atual + prt->re_coluna ;

   switch( prt->re_tipo ) {

      case DISPLAY :
         p_display =  ( SC * ) prt + sizeof( RTela );
         memcpy( &mat_etiq[ E_PR(lin, col) ], p_display, strlen( p_display ) );

         if ( strstr(p_display, "@R") != NULL ) {
            TEXT num[15], *pi, *p;
            COUNT l;

            pi = strstr(&mat_etiq[ E_PR(lin, col) ], "@R");
            p  = pi + 2;

            if ( *p >= '1' && *p <= '9' )
               l = *p - '0';
            else
               l = 5;

            sprintf( num, "%12.12ld", c_bases[sele]->cb_numrec);
            memcpy( pi, &num[12-l], l );
         }
         break;

      case CAMPO_E :
      case CAMPO_S :
         {
            p_campo   =  vetcp[ prt->re_ivetcp ]->cp_newdata;

            tam = strlen( p_campo );

            if ( prt->re_vhor > 0 )
               tam = MIN(prt->re_vhor, tam);

            memcpy( &mat_etiq[ E_PR(lin, col) ], p_campo, tam);
         }

         break;
      case LIN_HOR :
      {
         UC col_f;

         col_f   =  col_atual + prt->re_vhor;

         for ( ; col < col_f ; col++ )
            mat_etiq[ E_PR( lin, col ) ] = UG_HOR;

         break ;
      }
      case LIN_VER :
      {
         UC lin_f;

         for  (lin_f = prt->re_vver ; lin <= lin_f ; lin++ ) {
            mat_etiq[ E_PR( lin, col ) ] = UG_VER;
         }
         break ;
      }
      case BORDA :
      {
         UC lin_f, col_f, cont;

         lin_f   =  prt->re_vver;
         col_f   =  col_atual + prt->re_vhor;

         mat_etiq[ E_PR(lin, col ) ]     =  UG_UL;
         mat_etiq[ E_PR(lin, col_f ) ]   =  UG_UR;
         mat_etiq[ E_PR(lin_f, col ) ]   =  UG_LL;
         mat_etiq[ E_PR(lin_f, col_f ) ] =  UG_LR;

         /* linhar horizontal */
         for (cont = col + 1; cont < col_f ; cont++ ) {
            mat_etiq[ E_PR( lin,   cont ) ] = UG_HOR;
            mat_etiq[ E_PR( lin_f, cont ) ] = UG_HOR;
         }

         /* linhar vertical */
         for ( cont = lin + 1; cont < lin_f; cont++ ) {
            mat_etiq[ E_PR( cont, col ) ]   =  UG_VER;
            mat_etiq[ E_PR( cont, col_f ) ] =  UG_VER;
         }
         p_display =  ( SC * ) prt + sizeof( RTela );
         memcpy( &mat_etiq[ E_PR(lin, col+1) ], p_display, strlen( p_display ) );
         break;
      }
   } /* switch tipo */
   return ( 1 );
}


F__LOC   SS etq_imp_pag (  )
{
   SS i,  ret = 1,  j;

   escrava_on();

   for ( i = 0; i < prel->rel_linhas; i++ ) {

      for ( j = 0; j < tam_linha; j++ )  {
         imp_char( mat_etiq[ E_PR(i, j) ] );
      }

      imp_puts( device, i_fdl );     /* imp_char( '\n' ); */
   }

   for ( j = 0; j < prel->rel_l_inter; j++ )
      imp_puts( device, i_fdl );     /* imp_char( '\n' ); */

   escrava_off();

fim:

   return ( ret ) ;
}



F__LOC   SS etq_teste_prn(  )
{
   return ( 1 );
}

