/*---------- lb2rel4.c ----------*/
#include  "lb2all.h"

#include  <ctype.h>
#include  <string.h>

#include  "lb2stru.h"
#include  "lb2vars.h"
#include  "lb2text.h"  /* strings comus a todos os modulos */

#include  "lb2bases.h"
#include  "lb2entra.h"
#include  "lb2files.h"
#include  "lb2locs.h"
#include  "lb2prot1.h"

#include  "lb2rel.h"


#define   CP_DATA    'D'

#define   H_DEST     "RD_DEST"
#define   H_PGIN     "RG_PGIN"
#define   H_PAG      "RL_PAG"
#define   H_PLIN     "RE1_PLIN"
#define   H_MUDAR    "RL_MUDAR"
#define   H_NEG      "RL_NEG"
#define   H_DEFRE    "DEF_RELC"
#define   H_NAOSO    "NAO_SORT"

#define   M_CF_DESTINO       loc_str( GRP_IMPREL, 23)
#define   M_CF_PAGINI        loc_str( GRP_IMPREL, 24)
#define   M_CF_PULPAG        loc_str( GRP_IMPREL, 25)
#define   M_CF_MUDCARAC      loc_str( GRP_IMPREL, 26)
#define   M_CF_PALNEGR       loc_str( GRP_IMPREL, 27)

#define   M_CF_CARAC         loc_str( GRP_IMPREL, 28)
#define   M_CF_NUPAG         loc_str( GRP_IMPREL, 29)
#define   M_CF_DISP          loc_str( GRP_IMPREL, 30)
#define   M_CF_PULAR_LINHA   loc_str( GRP_IMPREL, 31)
#define   M_CF_PULAR         loc_str( GRP_IMPREL, 32)
#define   M_CF_MUDAR         loc_str( GRP_IMPREL, 33)
#define   M_CF_PALPESQ       loc_str( GRP_IMPREL, 34)

#define   M_CF_PULLIN        loc_str( GRP_IMPREL, 35)

#define   E_NAOSORT          loc_str( GRP_IMPREL, 502)

#define   CR_REQUER   0x1
#define   CR_MINIMO   0x2
#define   CR_MAXIMO   0x4
#define   CR_BOLD     0x8


static COUNT lb_txtdel;  /* delimitador especias */

/*-------------------------------------------------------------------*/
/*          estruturas                                               */
/*-------------------------------------------------------------------*/
struct str_carac_imp {
   SC destino[ TE+1 ];   /* nome logico da impressora */
   SC pagina_inic[ TE+ 1];
   SC pular_pagina[ TE+1 ];
   SC mudar_carac[ TE+1 ];
   SC palavras_negrito[ TE+1 ];
};


/*------------------------------------------------------------------------*/
/* inicio de prototipos                                                   */
/*------------------------------------------------------------------------*/

#ifdef MSDOS
   SS    val_device( d_entry *);
   SS    muda_basica( d_entry *);
   SS    val_pginic(d_entry *);
   VOID  ordena_desc ( SC *, SS, SS );
   SC   *pega_dado(Campo **, COUNT);
   SS    quantos_textos( SS );
   SS    ordena_texto( SC * );
   VOID  seta_pi(COUNT, COUNT, COUNT, COUNT);
#else
   SS    val_device( );
   SS    muda_basica( );
   SS    val_pginic( );
   VOID  ordena_desc ( );
   SC   *pega_dado( );
   SS    quantos_textos( );
   SS    ordena_texto( );
   VOID  seta_pi( );
#endif


F__GLB   COUNT carac_form(  )
{
   COUNT ret, campo, flag;
   TEXT  *ajuda_ant;
   WIN   *jf, *j2;
   TEXT  *lb_impress;     /* elementos separados por '@' */
   struct str_carac_imp ci;
   DTela cm[6];


   ret = 0;
   jf  = NULL;
   ajuda_ant = win_ajuda;
   c_impress.pular_pagina = 'N';
   c_impress.palavras_negrito = 'N';

   if ( (lb_impress = malloc(MAX_PRINTS * TE + 10)) == NULL ) {
      ret = -1;
      goto fim;
   }

   if ( pl_br_rel != NULL )   flag = 1;
   else                       flag = 0;

   mont_vt_impr(lb_impress);

   ci.destino[ 0 ]          = '\0';
   strcpy(ci.pagina_inic, NU_PAGINICIAL);
   strcpy( ci.pular_pagina, OP_NAO);
   strcpy( ci.mudar_carac, OP_NAO);
   strcpy( ci.palavras_negrito, OP_NAO);


   seta_dtela(&cm[0], ci.destino,     x14, M_CF_DESTINO,   LI, CO, 0, E_LISTA,  H_DEST, lb_impress, NULL, val_device);
   seta_dtela(&cm[1], ci.pagina_inic, x14, M_CF_PAGINI,  LI+1, CO, 0, E_ALFA,  H_PGIN,  NULL, NULL, val_pginic);

   if( prel->rel_tipo == FORMATADO )
      seta_dtela(&cm[2], ci.pular_pagina, x14, M_CF_PULPAG, LI+2, CO, 0, E_LISTA, H_PAG, op_sn, NULL, NULL );
   else
      seta_dtela(&cm[2], ci.pular_pagina, x14, M_CF_PULLIN, LI+2, CO, 0, E_LISTA, H_PLIN, op_sn, NULL, NULL );

   seta_dtela(&cm[3], ci.mudar_carac, x14, M_CF_MUDCARAC, LI+3, CO, 0, E_LISTA, H_MUDAR, op_sn, NULL, muda_basica );

   if ( flag == 1 ) { /* pergunta se negrita so faz sentido para chamada por pesquisa */
      seta_dtela(&cm[4], ci.palavras_negrito, x14, M_CF_PALNEGR, LI+4, CO, 0, E_LISTA, H_NEG, op_sn, NULL, NULL );
   }
   else
      seta_dtela(&cm[4], NULL, NULL, NULL, 0, 0, 0, 0, NULL, NULL, NULL, NULL);

   seta_dtela(&cm[5], NULL, NULL, NULL, 0, 0, 0, 0, NULL, NULL, NULL, NULL);

   win_ajuda = H_DEFRE;

   j2 = abre_janela(J_CARFORM);
	fw_title(j2, M_CF_CARAC, j2->w_atrib);
	fwbox(j2);

   mvwprintw(j2 , LI+1,  1, M_CF_NUPAG);
   mvwprintw(j2 , LI  ,  1, M_CF_DISP );

   if( prel->rel_tipo == FORMATADO )
      mvwprintw(j2 , LI+2,  1, M_CF_PULAR);
   else
      mvwprintw(j2 , LI+2,  1, M_CF_PULAR_LINHA);

   mvwprintw(j2 , LI+3,  1, M_CF_MUDAR);

   if ( flag == 1 ) { /* pergunta se negrita so faz sentido para chamada por pesquisa */
      mvwprintw(j2 , LI+4,  1, M_CF_PALPESQ );
   }

   fwrefresh(j2);

   jf = abre_funcoes(funcao(LF_13E));

   campo = 1;

   c_impress.destino[ 0 ] = '\0';
   c_impress.driver[ 0 ]  = '\0';

le:

	ret = ed_read(j2, cm, campo, MOSTRA_LE,  LF_13E );

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

      case K_F2   : numdebug( 204 );
                    break;
	}

   if ( stricmp( ci.pular_pagina, OP_SIM ) == 0 ) {
      c_impress.pular_pagina = SIM;
   }

   if ( stricmp( ci.palavras_negrito, OP_SIM ) == 0 ) {
      c_impress.palavras_negrito = SIM;
   }

fim:

	fecha_funcoes(jf);

	fwkill(j2);

   if ( lb_impress != NULL ) free(lb_impress);

   win_ajuda = ajuda_ant;

   return (ret);
}


F__GLB   SI  ord_itrel( p1, p2 )
SC  **p1, **p2;
{
   SS        T1,  T2;
   RTela     *P1, *P2;

   P1  = ( RTela * ) *p1;
   P2  = ( RTela * ) *p2;


   if ( P1->re_tela  >  P2->re_tela )   return(  1 );
   if ( P1->re_tela  <  P2->re_tela )   return( -1 );

   /*** Tela e Igual ***/

   if ( P1->re_linha  >  P2->re_linha )   return(  1 );

   if ( P1->re_linha  <  P2->re_linha )   return( -1 );

   /*** Tela e Linhas sao Iguais ***/

   if ( P1->re_coluna  >  P2->re_coluna )   return(  1 );

   if ( P1->re_coluna  <  P2->re_coluna )   return( -1 );

   /*** Tela, Linhas  e colunas sao Iguais ***/
   switch ( P1->re_tipo )
   {
   case NOMES :
                  T1 = 1;
                  break;
   case LIN_VER :
   case LIN_HOR :
                  T1 = 2;
                  break;
   case BORDA :
                  T1 = 3;
                  break;
   case CAMPO_S :
                  T1 = 4;
                  break;
   case CAMPO_E :
                  T1 = 5;
                  break;
   case COMANDO_REL :
                  T1 = 6;
                  break;
   }

   switch ( P2->re_tipo )
   {
   case NOMES :
                  T2 = 1;
                  break;
   case LIN_VER :
   case LIN_HOR :
                  T2 = 2;
                  break;
   case BORDA :
                  T2 = 3;
                  break;
   case CAMPO_S :
                  T2 = 4;
                  break;
   case CAMPO_E :
                  T2 = 5;
                  break;
   case COMANDO_REL :
                  T1 = 6;
                  break;
   }

   if ( T1  >  T2 )   return(  1 );

   if ( T1  <  T2 )   return( -1 );

   /*** Tela e Tipos e Linhas e Colunas sao Iguais ***/

   return ( 0 );
}

F__LOC   TEXT *pega_dado(vetcp, ind)
Campo **vetcp;
COUNT ind;
{  COUNT tipo;
   TEXT *aux;

   tipo = toupper(vetcp[ ind ]->cp_tipo);

   switch(tipo) {
      case 'D'   :   aux = conv_data( vetcp[ind]->cp_newdata );
                     break;

      case CP_TEXTO :   /* Vai entrar em rotina especial para pegar linha a linha */
                     break;

      default    :   aux = vetcp[ind]->cp_newdata;
                     break;
   }

   return(aux);
}




F__GLB   SS trata_ordenacao( sele, pfun, jm )
SS sele;
#ifdef MSDOS
   SL (*pfun)( SS, SL );
#else
   SL (*pfun)( );
#endif
WIN *jm;
{  COUNT ordena;
   COUNT tam_campo, tam_linha, i;
   TEXT  masc[ 100 ], aux[ 150 ];


   max_ord = 0;
   ordena = 0;
   tam_linha = 0;
   masc[ 0 ] = '\0';

   prel->rel_1ind = prel->rel_2ind = prel->rel_3ind = 0;
   QQ1 = QQ2 = QQ3 = 0;

   for ( i = 1; i <= 3; i++) {
      qbr[i].qb_offset  = 0;
      qbr[i].qb_tamanho = 0;
      qbr[i].qb_ocorre  = 0;
      qbr[i].qb_campo   = (Campo *)NULL;
   }

   if ( prel->rel_1numcampo != 0 ) {
      prel->rel_1ind = acha_campo(sele, prel->rel_1numcampo);
      qbr[1].qb_campo = vetcp[prel->rel_1ind];
      max_ord++;
      if ( prel->rel_2numcampo != 0 ) {
         prel->rel_2ind = acha_campo(sele, prel->rel_2numcampo);
         qbr[2].qb_campo = vetcp[prel->rel_2ind];
         max_ord++;
         if ( prel->rel_3numcampo != 0 ) {
            prel->rel_3ind = acha_campo(sele, prel->rel_3numcampo);
            qbr[3].qb_campo = vetcp[prel->rel_3ind];
            max_ord++;
         }
      }
   }

   if ( max_ord > 0 ) {
      ordena = SIM;
      for ( i = 1; i <= max_ord; i++) {
         tam_campo  =  qbr[i].qb_campo->cp_mhor;
         if ( qbr[i].qb_campo->cp_tipo == 'N' || qbr[i].qb_campo->cp_tipo == 'F' )
            sprintf( aux, "%%%d.%ds", tam_campo, tam_campo);
         else
            sprintf( aux, "%%-%d.%ds", tam_campo, tam_campo);
         qbr[i].qb_tamanho = tam_campo;
         qbr[i].qb_offset  = tam_linha;
         tam_linha += tam_campo;
         strcat( masc, aux );
      }


      tam_linha = tam_linha + SIZE_NR + 1; /* num registro + fim de linha */

      sprintf(aux, "%d", tam_linha);
      push_linha( INICIALIZA, aux );

      mens_1( jm, M_EMI_ORDENA );
      fwrefresh( jm );
      fwsetcursor( jm );

      ordena_campos( sele, pfun, masc );

      if ( push_linha( ORDENA, "") == -1 )
         mens_erro(H_NAOSO, E_NAOSORT);

   }

   return(ordena);
}


F__LOC   SS ordena_campos( sele, pfun, masc )
SS sele;
#ifdef MSDOS
   SL (*pfun)( SS, SL );
#else
   SL (*pfun)( );
#endif
SC *masc;
{  COUNT ret, trata_texto = 0;
   SC    linha[ 250 ], aux[ 10 ], aux2[20];
   LONG  reg;

   if ( (trata_texto = quantos_textos(sele)) > 1 ) {
      debug("WWW mais de um texto");
      ret = -1;
      goto fim;
   }

   reg = 0L;
   reg = (*pfun)(sele, reg);

   if ( (pfun != reg_corrente) && read_record(sele, reg, 0) != 0 ) {
      ret = -99;
      goto fim;
   }

   reg = 0L;

   while ( ( reg = (*pfun)( sele, reg ) ) > 0 ) {

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

      linha[ 0 ] = '\0';

      switch( max_ord ) {
         case 0 :
            numdebug( 2005 );
            break;

         case 1 :

            sprintf( linha, masc, pega_dado(vetcp, prel->rel_1ind) );
            prep_palavra( linha );
            if ( prel->rel_1ord == LBDESCE )
               ordena_desc( linha, 0, qbr[1].qb_campo->cp_mhor);
            break;

         case 2 :

            sprintf( linha, masc, pega_dado(vetcp, prel->rel_1ind),
                                  pega_dado(vetcp, prel->rel_2ind) );
            prep_palavra( linha );
            if ( prel->rel_1ord == LBDESCE )
               ordena_desc( linha, 0, qbr[1].qb_campo->cp_mhor );
            if ( prel->rel_2ord == LBDESCE )
               ordena_desc( linha, qbr[1].qb_campo->cp_mhor, qbr[2].qb_campo->cp_mhor );
            break;

         case 3 :
            sprintf( linha, masc, pega_dado(vetcp, prel->rel_1ind),
                                  pega_dado(vetcp, prel->rel_2ind),
                                  pega_dado(vetcp, prel->rel_3ind) );
            prep_palavra( linha );

            if ( prel->rel_1ord == LBDESCE )
               ordena_desc( linha, 0, qbr[1].qb_campo->cp_mhor );
            if ( prel->rel_2ord == LBDESCE )
               ordena_desc( linha, qbr[1].qb_campo->cp_mhor, qbr[2].qb_campo->cp_mhor );
            if ( prel->rel_3ord == LBDESCE )
               ordena_desc( linha, qbr[1].qb_campo->cp_mhor + qbr[2].qb_campo->cp_mhor,
                                   qbr[3].qb_campo->cp_mhor);
            break;
      }

      sprintf( aux2, "%%%d.%dld", SIZE_NR, SIZE_NR);
      sprintf( aux, aux2, reg );
      strcat( linha, aux );

      if ( trata_texto == 1 ) {
         if ( ordena_texto(linha) != 0 ) { ret = -1; goto fim; }
      }
      else
         push_linha( PUSH, linha );
   }

fim:

   return( ret );
}


/* seta globais ord_txt e off_txt */
F__LOC   SS quantos_textos( sele )
SS sele;
{  COUNT ret = 0, i;

   sele = sele;
   pc_txt = NULL;
   off_txt=0;

   for ( i = 1; i <= max_ord; i++) {
      if ( qbr[i].qb_campo->cp_tipo == CP_TEXTO ) {
         ret++;
         pc_txt = qbr[i].qb_campo;
         switch(i) {
            case 1 : ord_txt = prel->rel_1ord; break;
            case 2 : ord_txt = prel->rel_2ord; break;
            case 3 : ord_txt = prel->rel_3ord; break;
         }
      }
      else
         if ( pc_txt == NULL )
            off_txt += qbr[i].qb_campo->cp_mhor; /* usado para posicionar replicacao */
   }

   return(ret);
}


F__LOC   COUNT ordena_texto(linha)
TEXT *linha;
{  COUNT ret, lin_inicio, nl, tam_linha;
   UTEXT texto[250];
   TEXT  *pt, *p, *p_prox;
   LONG tam;
   POINTER pos;
   Campo *pc;
   ARG *adoc;

   pc = pc_txt;
   adoc = pc->cp_adoc;
   ret = 0;
   nl = 0;
   lin_inicio = 0;

   if ( adoc == NULL ) {
      texto[0] = '\0';
      memcpy((TEXT *)&pos, (TEXT *)pc->cp_newdata , sizeof(pos));
      if ( pos != (POINTER) 0 ) {
         adoc = txt_load(AD_NUM(BASE_EDIT), pos, &tam, pc->cp_mhor, WCB->cb_numrec);
         if ( adoc == NULL ) {
            ret = -1;
            mens_erro(H_LDDOC, E_LDDOC);
            goto fim;
         }
      }
   }


   pt = linha + off_txt;
   memset(pt , ' ', pc->cp_mhor);

   p = NULL;
   p_prox = NULL;

   while ( adoc != NULL && ret == 0 ) {

      if ( p == NULL ) {

         tam_linha = lin_texto( adoc, lin_inicio++, (TEXT *)texto, 1);

         if ( tam_linha < 0 )
            break;

         if ( tam_linha > pc->cp_mhor ) {
            tam_linha = pc->cp_mhor;
            texto[tam_linha] = '\0';
         }

         p = (TEXT *)texto;
      }

      if ( lb_txtdel ) {
         p_prox = strchr(p, lb_txtdel);
         if ( p_prox != NULL )
            *p_prox++ = '\0';
      }

      trim((TEXT *)p);
      tam_linha = strlen((TEXT *)p);

      if ( tam_linha > 0 ) {
         prep_palavra( (TEXT *)p );
         memcpy(pt , p, tam_linha);
         memset(pt + tam_linha, ' ', pc->cp_mhor - tam_linha);

         if ( ord_txt == LBDESCE )
            ordena_desc( pt, 0, pc->cp_mhor );

         nl++;
         ret = push_linha( PUSH, linha );
      }

      if ( grel->replica_txt != 1 )
         break;

      p = p_prox;
   }

   if ( nl == 0 )
      push_linha( PUSH, linha );

   if ( adoc !=NULL && adoc != pc->cp_adoc )
      free_buffer(adoc );

fim:

   return( ret );
}





/*---------- calc_max( ) ----------*/

/* verifica o numero de linhas que uma vista utiliza */
F__LOC   SS calc_max( pi, nu_elem )
RTela **pi;
SS nu_elem;
{
   SS i, max=0, item;


   for ( i = 0 ; i < nu_elem ; i++ ) {

      switch( pi[ i ]->re_tipo ) {

         case COMANDO_REL :
            break;

         case BORDA :
         case LIN_VER :
            /* mudei em 1.8rel4.0 item = pi[ i ]->re_linha + pi[ i ]->re_vver; */
            item = pi[ i ]->re_vver;
            break;

         default      :
            item = pi[ i ]->re_linha;
            break;

      }

      max = MAX( max, item );
   }

   return ( max ) ;
}

F__LOC   SS muda_basica( entry )
d_entry *entry;
{
   SS ret;

	if ( entry->tecla == K_ESC  || entry->mudou == 0 ) {
		ret = ACEITA;
		goto fim;
	};


   if (strcmp(OP_SIM, entry->str) == 0 ) {
      carac_rel();
   }

   ret = ACEITA;

fim:

   return ( ret );
}


F__LOC   SS val_pginic( entry )
d_entry *entry;
{
   SS ret;
   SC aux[TE+1];
   LONG inic_aux;
   ULONG inicio, fim, pag_inicial;

   if ( entry->tecla == K_ESC  || entry-> mudou == 0 ) {
      ret = ACEITA;
      goto fim_gt;
   };

   piece(entry->str, aux, '-', 1, TD);
   inic_aux = atol(aux);
   inicio   = atol(aux);
   prim_pagina = inicio;

   if ( inicio == 0 )
      inicio = prim_pagina = 1;

   piece(entry->str, aux, '-', 2, TD);
   fim = atol(aux);
   if ( fim == 0 )
      fim = -1;


   if ( inic_aux < 0 || fim < inicio ) {
      ret = REJEITA;
      goto fim_gt;
   }

   qtd_pagina = fim - inicio;
   if ( fim != -1 )
      qtd_pagina++;

   off_pagina = 0;

   piece( entry->str, aux, '(', 2, TD );
   pag_inicial = atol( aux );

   if ( pag_inicial != 0 ) 
      off_pagina = pag_inicial - inicio;

   ret = ACEITA;

fim_gt:

   return ( ret );
}




/*---------- ordena_desc( ) ----------*/

F__LOC   VOID ordena_desc ( s, pi, qtd )
SC *s;
SS  pi, qtd;
{
   SS fim;

   fim = pi + qtd;

   while ( pi < fim ) {

      *(s + pi) = 'z' - *(s + pi);
      pi++;
   }
}



/*--------- traduz Comando -----------*/
F__GLB   VOID traduz_comandos( )
{  COUNT num_lin, t;
   TEXT linha[250], *p;

   num_lin = 0;
   grel->nlin_cabec = 0;

   DELIM[0] = '-'; /* delimitadores de totalizacao */
   DELIM[1] = '=';
   DELIM[2] = '*';
   DELIM[3] = '#';

   while ( ap_reldoc != NULL &&
           lin_texto( ap_reldoc, num_lin++, linha, 0) > 0 ) {

      if ( (p=strchr(linha, ' ')) == NULL )
         p = "";
      else {
         *p = '\0';
         p++;
      }


      /* Controle de impressao de itens de quebra */
      if      ( stricmp(linha, "PACK") == 0 ) {
         grel->sup_linhas = atoi(p);
      }
      else if ( stricmp(linha, "DISPLAY") == 0 ) {
         grel->ctl_quebra = atoi(p);
      }
      else if ( stricmp(linha, "TEXTSORT") == 0 ) {
         lb_txtdel = 0;
         grel->replica_txt = 1;
         if ( p != NULL ) {
            while (*p == ' ' || *p == '\t')
               p++;
            if ( *p != '\0' )
               lb_txtdel = *p;
         }
      }
      else if ( stricmp(linha, "LINES") == 0 ) {
         grel->linhas_def = atoi(p);
      }
      else if ( stricmp(linha, "TOP") == 0 && grel->nlin_cabec < MAX_TOP ) {
         t = strlen(p) + 1;
         if ( (grel->cabec[grel->nlin_cabec] = malloc(t)) != NULL ) {
            strcpy(grel->cabec[grel->nlin_cabec], p);
            grel->nlin_cabec++;
         }
      }
      else if ( stricmp(linha, "BOT") == 0 && grel->nlin_rodape < MAX_BOT ) {
         t = strlen(p) + 1;
         if ( (grel->rodape[grel->nlin_rodape] = malloc(t)) != NULL ) {
            strcpy(grel->rodape[grel->nlin_rodape], p);
            grel->nlin_rodape++;
         }
      }
      else if ( stricmp(linha, "REQUER") == 0 ||
                stricmp(linha, "REQUIRE") == 0 ) { /* REQUER lin qtd */
         COUNT lin, requer;
         if ( p != NULL ) {
            lin = atoi(p);
            while ( *p && isdigit(*p))
               p++;
            while (*p && *p == ' ' || *p == '\t')
               p++;
            requer = atoi(p);

            seta_pi('R', lin, requer, 'F');
         }
      }
      else if ( stricmp(linha, "MIN") == 0 ) { /* MINIMO lin qtd */
         COUNT lin, requer;
         if ( p != NULL ) {
            lin = atoi(p);
            while ( *p && isdigit(*p))
               p++;
            while (*p && *p == ' ' || *p == '\t')
               p++;
            requer = atoi(p);

            seta_pi('m', lin, requer, 'F');
         }
      }
      else if ( stricmp(linha, "MAX") == 0 ) { /* MAXIMO lin qtd */
         COUNT lin, requer;
         if ( p != NULL ) {
            lin = atoi(p);
            while ( *p && isdigit(*p))
               p++;
            while (*p && *p == ' ' || *p == '\t')
               p++;
            requer = atoi(p);

            seta_pi('M', lin, requer, 'F');
         }
      }
      else if ( stricmp(linha, "BOLD") == 0 ) { /* BOLD cmp ou BOLD lin */
         COUNT cmp;
         if ( p != NULL ) {
            if ( ver_campo(BASE_EDIT, p, &cmp) >= 0 ) {
               seta_pi('B', cmp, 0, 'F');
            }
            else {
               COUNT lin, requer;
               lin = atoi(p);
               while ( *p && isdigit(*p))
                  p++;
               while (*p && *p == ' ' || *p == '\t')
                  p++;
               requer = atoi(p);

               seta_pi('B', lin, requer, 'F');
            }
         }
      }
      else if ( stricmp(linha, "DELIMITER") == 0 ) { /* DELIMITER C1C2C3CT */
         COUNT x=0;
         while ( *p && x <= 3) {
            DELIM[x++] = *p++;
         }
      }
      else if ( stricmp(linha, "NODISPLAY") == 0 ) {
         /* Nao repete quebras quando muda de pagina */
         grel->no_display = 1;
      }
      else if ( stricmp(linha, "FIRST") == 0 ) {
         /* faixa de regsitros que seram impressos */
         grel->first_record = atol(p);
      }
      else if ( stricmp(linha, "LAST") == 0 ) {
         /* faixa de regsitros que seram impressos */
         grel->last_record = atol(p);
      }


   }
}


F__LOC VOID seta_pi(carac, ident, valor, flag)
COUNT carac, ident, valor, flag;
{  COUNT i;
   RTela **PI;

   if ( flag != 0 && prel->rel_tipo != flag )
      return;

   PI = (RTela **)prel->rel_vet->vi_vtit;


   for( i = 0; i < prel->rel_n_elem; i++ )   {
      if ( carac == 'B' && valor == 0) { /* Bold */
         if ( PI[ i ]->re_num == ident ) {
            PI[ i ]->re_param[0] |= CR_BOLD;
            break;
         }
      }
      else if ( PI[ i ]->re_linha == (ident - 1) ) {
         if ( carac == 'R' ) {
            PI[ i ]->re_param[0] |= CR_REQUER;
            PI[ i ]->re_param[1] = valor;
            break;
         }
         else if ( carac == 'm' ) {
            PI[ i ]->re_param[0] |= CR_MINIMO;
            PI[ i ]->re_param[2] = valor;
            break;
         }
         else if ( carac == 'B' ) {
            PI[ i ]->re_param[0] |= CR_BOLD;
            PI[ i ]->re_param[1] = valor;
            break;
         }
         else if ( carac == 'M' && PI[ i ]->re_tipo == 'S' ) {
            PI[ i ]->re_param[0] |= CR_MAXIMO;
            PI[ i ]->re_param[3] = valor;
         }
      }
   }
}
