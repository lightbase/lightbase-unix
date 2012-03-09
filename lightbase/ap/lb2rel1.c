/*---------- lb_rel1.c ----------*/
#include  "lb2all.h"

#include  <string.h>

#include  HD_IO
#include  HD_DIR

#include  "lb2stru.h"
#include  "lb2vars.h"
#include  "lb2text.h"  /* strings comus a todos os modulos */

#include  "lb2bases.h"
#include  "lb2entra.h"
#include  "lb2prot1.h"

#include  "lb2rel.h"


#define   H_CFAEDI   "CF_A_EDI"

#define   ARQ_SORT   "LBIXXXXXX"    /* nome arquivo para ordenacao em relatorio */

#define   M_EMI_LPAG          loc_str( GRP_IMPREL, 13)
#define   M_EMI_DATA          loc_str( GRP_IMPREL, 14)
#define   M_EMI_PAGE          loc_str( GRP_IMPREL, 15)
#define   M_EMI_SALPAG        loc_str( GRP_IMPREL, 16)

#define   CR_REQUER   0x1
#define   CR_MINIMO   0x2
#define   CR_MAXIMO   0x4
#define   CR_BOLD     0x8



/*--------------------------------------------------------------------*/
/*    Variaveis staticas / externas                                   */
/*--------------------------------------------------------------------*/
static US    cb_inicio, cb_fim, cb_emreg;
static UL    seq_impr;
static SC    *mat_atrib;
static SC    data[15], hora[15], *desc_base;
static SC    txt_virgem;

extern TEXT ap_cliente[];

extern SI   errno;

/*---------- Prototipos ----------*/
#ifdef MSDOS
   SS    varre_itens( SS, SS);
   COUNT imp_texto(SS, Campo *, RTela *, SS, SS, SS, SS *);
   COUNT formata_linha(ARG *, UCOUNT, TEXT *);
   VOID  ctrel_enfat( COUNT, COUNT, TEXT *);
   SS    c_traco(TEXT *, RTela *);
   VOID  cab_padrao( VOID );
   VOID  cab_proprio( VOID );
   FILE *abre_sortarq(SC *, SC*);
   SS    poe_item_mat( SS, RTela *, SS);
   SS    ja_imprimiu( Campo * );
   SS    pergunta_salto( VOID );
   VOID  totaliza_colunado( RTela *, SC *);
   VOID  br_mat( SS, SS, SS);
   VOID  monta_cabec( VOID );
   SS    novo_varre_itens( SS, SS);
   VOID  trata_atrib( UC );
   SS    quant_atrib( SC * );
   VOID  poe_arroba( SC *, SS, SS, SS, SS);
   SI    bqr_comp( );
   VOID escrava_off(VOID);
   VOID escrava_on(VOID);
#else
   SS    varre_itens( );
   COUNT imp_texto( );
   COUNT formata_linha( );
   VOID  ctrel_enfat( );
   SS    c_traco( );
   VOID  cab_padrao( );
   VOID  cab_proprio( );
   FILE *abre_sortarq( );
   SS    poe_item_mat( );
   SS    ja_imprimiu( );
   SS    pergunta_salto( );
   VOID  totaliza_colunado( );
   VOID  br_mat( );
   VOID  monta_cabec( );
   SS    novo_varre_itens( );
   VOID  trata_atrib( );
   SS    quant_atrib( );
   VOID  poe_arroba( );
   SI    bqr_comp( );
   VOID escrava_off();
   VOID escrava_on();
#endif

/* EXP */
#ifdef MSDOS
COUNT exp_lin_texto(Campo *, TEXT *, FILE *);
COUNT exporta_rel(COUNT, TEXT *, FILE *);
#else
COUNT exp_lin_texto( );
COUNT exporta_rel( );
#endif


/*---------- rel_form( ) -----------*/

F__GLB   SS rel_form( sele, pfun )
SS sele;
#ifdef MSDOS
   SL (*pfun)( SS, SL );
#else
   SL (*pfun)( );
#endif
{
   SS    ret, max,  i,  tam_matriz, ordena, deve_totalizar;
   SL    reg, old_reg, qtd_reg;
   SC    mensagem[ 80 ];
   SC    nome_file[PATH_MAX + 20];
   RTela **PI;
   WIN   *jm = NULL;
   TEXT *buf_exp = NULL;  /* EXP */
   COUNT exportando = 0;  /* EXP */
   

   old_reg = c_bases[sele]->cb_numrec;

   if ( pfun == NULL )
      pfun = prox_reg;

   jm = abre_mensagem( str_nada );

   col_inicial_totais = 0;
   col_tot        = NULL;
   mat_pag        = NULL;
   mat_atrib      = NULL;
   device         = NULL;
   num_soma       = -1;
   strcpy( data, data_hora( 'D', 0L ));
   strcpy( hora, data_hora( 'H', 0L ));
   desc_base  = v_bases[sele]->bs_desc;

   prim_pagina    = 0;
   num_pagina     = 0;
   off_pagina     = 0;
   qtd_pagina     = -1;


   if ( carac_form( ) < 0 ) {
      ret = 0;
      goto sai;
   }

   PI             = (RTela **)prel->rel_vet->vi_vtit;

   qsort((TEXT *) PI, prel->rel_n_elem,
            sizeof(RTela *), (int (* )()) ord_itrel );

   traduz_comandos();


/***** retirado de acordo com diretriz do ricardo
   if (prel->rel_tipo != FORMATADO )
      grel->replica_txt = 0;
******/

   ult_linha_util = prel->rel_linhas - prel->rel_c_inter;  /* do rodape */
   offset         = prel->rel_l_inter;


   if( prel->rel_cabec == SIM )
      ult_linha_util -= 3;

   tam_matriz     = prel->rel_linhas * prel->rel_colunas + 2;

   if ( prel->rel_tipo == COLUNADO )
      max = 0;
   else
      max  = calc_max( PI, prel->rel_n_elem );

   /* oitenta e folga para nao invadir quando incluir campo em rodape*/
   if ((mat_pag = malloc((tam_matriz + 80) * sizeof( SC ))) == NULL ) {
      ret = -3;
      goto fim;
   }

   if ((mat_atrib = malloc((tam_matriz + 80) * sizeof( SC ))) == NULL ) {
      ret = -3;
      goto fim;
   }


   /* conta quantos itens necessitam ser somados. */
   /* Aloca estrutura de somatorio */
   /* Inicializa membro n_item da estrutura de somatorio */

   if( prel->rel_tipo == COLUNADO )  {
      num_soma = col_inic_itens(0);
      col_tot = calloc( num_soma+1, sizeof( struct tot_col ));
      if ( col_tot == NULL ) {
         ret = -3;
         goto fim;
      }
      col_inic_itens(1);
   }


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

   /* EXP */
   if ( strstr(prel->rel_ident, "exp$$") != NULL ) {     
      exportando = 1;
      fprintf(device, "@C25\n");
      if ( (buf_exp = malloc(2000)) == NULL) {
         fprintf(stderr, "sem memoria para exportar de relatorio\n");
         exit(300);
      }
   }
   /* EXP */

   driver   =  c_impress.driver;

   if ( i_find_params('C') < 0 )   { /* carrega driver */
      ret = -5;
      goto fim;
   }

   inic_printer( );

   cb_inicio = 0;
   cb_fim    = 0;
   cb_emreg  = 0;
   seq_impr  = 0;


   reg     = 0L;
   qtd_reg = 0;
   deve_totalizar = ( prel->rel_tipo == COLUNADO ) ? 1 : 0;


   while ( 1 )   {

      COUNT salto_quebra;

      /* push_linha seta global nivel_qbr */
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

      /* EXP */
      if ( exportando ) {
         seq_impr++;  /* Incrementa Numero sequencial de impressao */
         exporta_rel(sele, buf_exp, device );
         sprintf(mensagem, "%s %ld", M_EMI_MENS, qtd_reg);
#ifndef ULTRIX
         mens_1(jm, mensagem);
         fwrefresh( jm );
         fwsetcursor( jm );
#endif
         goto cont_export;
      }
      /* EXP */

      if ( ordena == SIM )   {

         salto_quebra = quebra_reg( COMPARA, sele, deve_totalizar );

         if( salto_quebra == -1 )  {
            ret = -1;
            goto fim;
         }

         /* incrementa offset com linha a pular na quebra */
         offset += salto_quebra;
      }


      if ( seq_impr == 0 ) { /* nao tirar daqui para ja ter lido registro */
         seq_impr = 1;
         monta_cabec( );
         inicializa_pagina( sele );
      }
      else
         seq_impr++;  /* Incrementa Numero sequencial de impressao */

      qtd_reg++;

      if ( num_pagina >= prim_pagina )
         sprintf(mensagem, "%s %ld", M_EMI_MENS, qtd_reg);
      else
         sprintf(mensagem, "%s %d", M_EMI_LPAG, num_pagina + off_pagina);

#ifndef ULTRIX
      mens_1(jm, mensagem);
      fwrefresh( jm );
      fwsetcursor( jm );
#endif

      limpa_flag( PI, prel->rel_n_elem );

      cb_emreg = 1;

      for ( i = 0;  i <= max; i++ ) {

         ret = varre_itens( i, sele );

         if (ret == 1) { /* usuario interrompeu */
            offset = prel->rel_l_inter;
            break;
         }

         if ( ret < 0 ) {
            ret = -1;
            goto fim;
         }
      }

      cb_emreg = 0;

      if (ret == 1 || qtd_pagina == 0 )
         break;

      if ( c_impress.pular_pagina == SIM )  {

         if( prel->rel_tipo == FORMATADO ) {
            if( imprime_pagina( ) == -1 )  {
               ret = 0;
               offset = prel->rel_l_inter;
               goto fim;
            }
            inicializa_pagina( sele );
         }
         else  {   /* se pular linha a cada registro */
            offset++;
         }
      }

      /* EXP */
cont_export:
      ;
   }

   if ( prel->rel_tipo == COLUNADO && num_soma > 0 )
      quebra_reg( TOTALIZA, sele, deve_totalizar );

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

   if( offset  > prel->rel_l_inter )     imprime_pagina( );

   if ( ordena == SIM ) {
      quebra_reg( FINALIZA, sele, deve_totalizar );
   }

   if (device != NULL )  {
      term_printer( );
      if ( buf_exp != NULL ) /*EXP*/
         free(buf_exp);      /* EXP */
      fclose ( device );

      if ( c_impress.comando[0] != '\0' ) {
         TEXT aux[200];
         sprintf( aux, c_impress.comando, nome_file);
         win_shell( aux );
      }
   }

   if (ordena == SIM)      push_linha( FINALIZA, NULL );

   i_find_params('F');

   if ( mat_atrib != NULL )     free( mat_atrib );

   if ( mat_pag != NULL )     free( mat_pag );

   if ( prel->rel_tipo == COLUNADO && col_tot != NULL ) {
      free( col_tot );
   }

sai:

   fecha_mensagem(jm);

   if ( old_reg > 0L && (pfun != reg_corrente) && read_record(sele, old_reg, 0) != 0 ) {
      ret = -99;
   }

   return ( ret );
}



/*--------- imprime_pagina( ) ----------*/

F__GLB   SS imprime_pagina( )
{
   SS i,  ret = 1,  j;
   SS ult_col;

   if ( prel->rel_folha == SIM)  {
      if ( pergunta_salto( ) == 1 ) {
         ret = -1;
         goto fim;
      }
   }

   if ( num_pagina < prim_pagina || qtd_pagina-- == 0 ) {
      ret = 0;
      goto fim;
   }

   escrava_on();

   for ( i = 0; i < prel->rel_linhas; i++ ) {
/*...
      if ( i < grel->nlin_cabec && prel->rel_cabec == 'S' )
         poe_arroba(grel->cabec[i], i, 0, BASE_EDIT, 1);
...*/
      /* verifica se foi pressionado a tecla ESC */

      if ( w_kbhit(curr_win) && fwnqgc(curr_win) == K_ESC ) {
         COUNT r;

         escrava_off();

         r = decida(TW_RL_D00, TW_RL_D99, H_CFCAN,
                     2, 30, "%s\n%s", TW_RL_D01, TW_RL_D02);

         escrava_on();

         if ( r == 1 ) {
            for ( ; i < prel->rel_linhas; i++ )
               imp_puts( device, i_fdl );     /* imp_char( '\n' ); */

            escrava_off();
            ret = -1;
            goto fim;
         }
      }

      ult_col = prel->rel_colunas - 1;

      while( ult_col > 0  && mat_pag[PR(i, ult_col)] == BRANCO)
         ult_col--;

      for ( j = 0; j <= ult_col; j++ ) {

         if( mat_atrib[PR(i, j)] != BRANCO )
            trata_atrib(mat_atrib[PR(i, j)]);

         imp_char( mat_pag[PR(i, j)] );
      }

      if( mat_atrib[PR(i, j)] != BRANCO )
         trata_atrib(mat_atrib[PR(i, j)]);

      if( i ==  prel->rel_linhas-1 )
      {
         if( i_fdp[0] != '\0' )
            imp_puts( device, i_fdp );
         else
            imp_puts( device, i_fdl );     /* imp_char( '\n' ); */
      }
      else
         imp_puts( device, i_fdl );     /* imp_char( '\n' ); */
   }

   escrava_off();

fim:

   return( ret );
}





/*---------- varre_itens() ----------*/

F__LOC   SS varre_itens( l, sele )
SS l, sele;
{
   SS cont, requer = 1, usou, ret, u, virgem, campos, carac, minimo;
   SS ct_display, ct_itens;
   RTela *p_display = NULL;



   virgem = 0;
   campos = 0;
   minimo = 0;
   ct_itens = ct_display = 0;  /* contador de itens na linha */

loop :

   for ( cont = 0, usou = 1; cont < prel->rel_n_elem; cont++ ) {

      prt     = (RTela *)prel->rel_vet->vi_vtit[cont];  /* aponta para cont elemento */

      if ( prt->re_status == JA_TRATOU ) continue;

      if ( prt->re_linha != l ) continue;

      if (prt->re_tipo == DISPLAY )
         ct_display++;

      ct_itens++;

      if (prt->re_tipo == LIN_VER || prt->re_tipo == BORDA )
         requer = prt->re_vver - prt->re_linha + 1;
      else
         requer = 1;

      carac = prt->re_param[0];

      if ( carac & CR_REQUER)
         requer = MAX(requer, prt->re_param[1]);

      if ( carac & CR_MINIMO)
         minimo  = prt->re_param[2];

      if ( offset + requer > ult_linha_util ) {
         if ( imprime_pagina( ) == -1 )  {
            ret = 1;

            /* estava travando quando dava esc na impressao */
                  if (vetcp[ prt->re_ivetcp ]->cp_adoc != NULL ) {
                     if ( em_edicao == 0 ) {
                        free_buffer(vetcp[ prt->re_ivetcp ]->cp_adoc);
                        vetcp[ prt->re_ivetcp ]->cp_adoc = NULL;
                     }
                  }

            goto fim;

         }
         inicializa_pagina( sele );
      }

      prt->re_status = JA_TRATOU;

      u    = poe_item_mat( sele, prt, requer );

      if ( u == -1 ) /* nao imprimiu todo campo texto */
         usou = -1;

      if( usou != -1 )
         usou = MAX(u, usou);

      /* controle de eliminacao de linhas com campos sem informacao */
      if ( prt->re_tipo == CAMPO_E || prt->re_tipo == CAMPO_S ) {
         campos++;
         if ( vetcp[ prt->re_ivetcp ]->cp_tipo == CP_TEXTO ) {
            virgem += (txt_virgem == 1 ) ? 1 : 0;
         }
         else
            virgem += (vetcp[ prt->re_ivetcp]->cp_newdata[0] == '\0') ? 1 : 0;
      }

      if ( prt->re_tipo == DISPLAY ) /* guarda display para limpar se for o caso */
         p_display = prt;

   }  /* for nu_elem */

   /* Controla supressao de linhas */
   if ( campos > 0 && grel->sup_linhas != 0 && virgem == campos  ) {
      if (p_display != NULL ) {
         if (grel->sup_linhas == 2 )  {
            poe_item_mat( sele, p_display, -1); /* elimina display */
            usou = 0;
         }
      }
      else
         usou = 0;
   }

   /* tratamento para eliminar linhas virgens em display de @On */
   if ( grel->sup_linhas != 0 && ct_display == ct_itens && ct_display == 1 ) {
      COUNT i;
      TEXT *p;
      p = &mat_pag[ PR(offset, 0) ];
      for ( i = 0 ; i < prel->rel_colunas; i++ ) {
         if ( *p++ != ' ')
            break;
      }
      if ( i == prel->rel_colunas)
         usou = 0;
   }



   if ( usou == -1  ) { /* imprimiu o maximo que podia na pagina */
      offset = 999;
      goto loop;
   }

   usou = MAX(usou , minimo);

   offset += usou;



   ret = 0;

fim:

   if ( cont < 0 )
      ret = -1;

   return( ret );
}


/*---------- imp_texto( ) ----------*/

F__LOC   COUNT imp_texto(sele, pc, pr, lin, col, max, qtd_impressa)
SS sele;
Campo *pc;
RTela *pr;
SS lin, col, max, *qtd_impressa;
{  COUNT ret = 0, qtd, tam;
   POINTER pos;
   LONG tam_doc;
   UTEXT *p_aloc;
   UTEXT *string, *use;

   p_aloc = malloc(600);

   if ( p_aloc == NULL ) {
      ret = -1;
      goto fim;
   }

   string = p_aloc;
   txt_virgem = 1;

   qtd = 0;

   if ( pc->cp_adoc == NULL ) {

      pr->re_lin_inicial = 0;

      memcpy((TEXT *)&pos, (TEXT *)pc->cp_newdata , sizeof(pos));

      if ( pos != (POINTER) 0 ) {
         pc->cp_adoc = txt_load(AD_NUM(sele), pos, &tam_doc, pr->re_vhor, WCB->cb_numrec);
         if ( pc->cp_adoc == NULL )  {
            debug(M_PRDTXT, c_bases[sele]->cb_numrec, pc->cp_nome);
            ret = 0; /* Esta dando debug para nao comprometer todo o resto */
            /* ret = -1; */
            goto fim;
         }
      }
      else {
         /* nao existe texto a ser impresso */
         ret = 0;
         goto fim;
      }
   }

   if ( pc->cp_adoc->t_lines == 1 ) {
      ret = lin_texto( pc->cp_adoc, pr->re_lin_inicial, (TEXT *) string, 0);
      if (ret == 1 && string[0] == PARA_CHR ) {
         ret = 0;
         goto fim;
      }
   }

   txt_virgem = 0;


   while ( max-- ) {

      if ( pr->re_lin_inicial >= pc->cp_adoc->t_lines  ) {
         ret = -1;
         break;
      }

      tam = ret = formata_linha( pc->cp_adoc, pr->re_lin_inicial++, (TEXT *) string);

      string[tam] = '\0';


      use = string;

      qtd++;

      tam = MIN(pr->re_vhor, tam);

      poe_mat(lin, col, (SC *) use, tam) ;

      if ( c_impress.palavras_negrito == SIM )
         ctrel_enfat( lin, col, (SC *)use);

      lin++;

      if ( prt->re_param[0] & CR_MAXIMO) {
         if ( qtd >= prt->re_param[3]) {
            ret = -1;
            break;
         }
      }
   }

   *qtd_impressa = qtd;

   if ( ret >= 0 )
      ret = 1; /* indica que ainda existem linhas a serem impressas */
   else
      ret = 0;

fim:

   if ( p_aloc != NULL )
      free(p_aloc);

   return( ret );
}


COUNT formata_linha(args, linha, just )
ARG *args;
UCOUNT linha;
TEXT *just;
{
   SC  *C;
   SS  I, atr, qap, mar, len, des = 0;
   SS  mar_esq, mar_pri, mar_dir;
   SC   *L1;

   if ((L1 = vm_deref( args->ptr[ linha ], 0 )) == NULL ) {
      mar = 0;
      goto sai;
   }

   len = args->len[ linha ];

   mar = mar_atr( args, L1 /*orig*/, linha, len, 0, &atr, &qap,
                  &mar_esq, &mar_pri, &mar_dir );

   /*** Faremos a margem esquerda ***/
   for( I = 0, C = just;   I < mar_esq;   I++, C++ )   *C = ' ';

   /*** Justifique a linha devolva a verdadeira margem direita ***/
   /*** Devolva em "des" o deslocamento da posicao do cursor ***/
   mar = p_just_line( C, L1, mar, len, (SS) ' ', 0 /* col */, &des );


   mar += mar_esq;

   if ( (UC)just[mar-1] == PARA_CHR ) {

      mar--;

      if (mar >= 5 &&
         (UC)just[mar-1] == ET_FORM2 && (UC)just[mar-5] == ET_FORM1) {
         mar -= 5;
         just[mar] = PARA_CHR;
      }
      /** implementacao a pedido do caribe. em 18/05/94 */
      /* era assim ..... just[mar] = '\0'; */

      just[mar] = PARA_CHR;
      mar++;
      just[mar] = '\0';
   }



sai:

   return(mar);

   /*   imprime linha (just) com tamanho (mar) */
}


F__LOC   VOID ctrel_enfat( lin, col, str_linha)
COUNT lin, col;
TEXT *str_linha;
{  COUNT tem_palavra;
   TEXT  tmp_linha[MAX_EDIT];

   UC       *P;
   SC   *P_PAL;
   SS        I, J, x;
   extern   UC tudo_delimita[];


   P = (UC *)str_linha;
   P_PAL = tmp_linha;
   I=0;

   tem_palavra = -1;
   J=0;

   while ( *P ) {
      if ( strchr( (SC *)tudo_delimita, (SC)*P ) != NULL ) {
         if ( tem_palavra >= 0 ) {
            tmp_linha[ J++ ] = '\0';
            prep_palavra(tmp_linha);
            if ( bsearch( &P_PAL, (SC *) pl_br_rel,
                          qtd_plrel, sizeof(SC *), bqr_comp ) !=  NULL ) {
               /*negrita palavra */
               x = PR(lin, (col+tem_palavra));
               mat_atrib[x] = ET_ENFAT;
               mat_atrib[x+J-1] = ET_NORMAL;
            }

         };

         tem_palavra = -1;
      }
      else {
         if ( tem_palavra < 0 ) {
            tem_palavra = I;
            J=0;
         }
         tmp_linha[ J++ ] = *P;
      }

      I++;
      P++;
   }

   if ( tem_palavra >= 0 ) {
      tmp_linha[ J++ ] = '\0';
      prep_palavra(tmp_linha);
      if ( bsearch( &P_PAL, (SC *) pl_br_rel,
                     qtd_plrel, sizeof(SC *), bqr_comp ) !=  NULL ) {
         /*negrita palavra */
         x = PR(lin, (col+tem_palavra));
         mat_atrib[x] = ET_ENFAT;
         mat_atrib[x+J-1] = ET_NORMAL;
      }
   }
}


/*---------- c_traco( ) ----------*/

F__LOC   SS  c_traco(pd, pr)
TEXT *pd;
RTela *pr;
{  COUNT tam, c;
   TEXT *po;
   UTEXT *p;

   po = pd + pr->re_coluna;
   p = (UTEXT *)strchr(po, UG_SP);

   if ( p != NULL ) {
      c = *p;
      *p = '\0';
      tam = strlen(po);
      *p=c;
   }
   else {
      tam = strlen(po);
   }

   return(tam);
}

/*---------- inicializa pagina ----------*/

F__GLB   SS  inicializa_pagina( sele )
SS sele;
{
   SS tam_matriz, i, num;
   SC *p_display;

   if ( grel->no_display == 0 ) {
      QQ1 = QQ2 = QQ3 = 0;

      for ( i = 1; i <= 3; i++) {
         qbr[i].qb_ocorre  = 1;
      }
   }

   /*** zerar matriz ***/

   tam_matriz = prel->rel_linhas * prel->rel_colunas + 1;

   memset( mat_pag,   ' ', tam_matriz);
   memset( mat_atrib, ' ', tam_matriz);

   mat_pag[tam_matriz-1]   = '\0';
   mat_atrib[tam_matriz-1] = '\0';

   ++num_pagina;
   offset = prel->rel_l_inter;

   /** verificar se deve imprimir cabecalho **/
   if ( prel->rel_cabec == SIM ) {
      if ( grel->nlin_cabec == 0 )
         cab_padrao();
      else
         cab_proprio();
   }

   if ( prel->rel_tipo == COLUNADO ) {
      SC *p;
      SS n_col;
      offset++;
      p_display = (SC *)prel->rel_vet->vi_vtit[ 0 ] + sizeof( RTela );

      n_col = 0;
      p = p_display;
      while ( *p == ' ' || *p == (SC)UG_SP ) {   n_col++;   p++;  }

      /** usado em nivel_quebra **/
      col_inicial_totais = n_col;

      poe_mat( offset, 0, p_display, strlen( p_display ) );
      offset++;

      for ( i=1; i < prel->rel_n_elem; i++) {
         RTela *pr;

         pr = (RTela *)prel->rel_vet->vi_vtit[i];

         num = c_traco(p_display, pr);

         pr->re_vhor = MAX(num, pr->re_vhor);

         memset( &mat_pag[ PR(offset, pr->re_coluna)], UG_HOR, num );
      }
      offset++;
      offset++;
   }
   else

      if( cb_emreg && cb_inicio > 0) {
         offset++;
         for( i = cb_inicio-1; i <= cb_fim-1; i++ )
            novo_varre_itens( i, sele );
         offset++;
      }

   return 1;
}


F__LOC VOID cab_padrao( )
{  SS tam;
   SS lin,  col;
   SC aux [ 20 ];
   TEXT *rodape;

   lin        = (prel->rel_linhas - prel->rel_c_inter );
   col        = prel->rel_colunas;

   poe_mat(prel->rel_l_inter,     0, ap_cliente, strlen(ap_cliente));
   poe_mat(prel->rel_l_inter + 1, 0, prel->rel_titulo, strlen(prel->rel_titulo));

   if ( grel->nlin_rodape > 0 )
      rodape = grel->rodape[0];
   else
      rodape = prel->rel_rodape;

   if( rodape[0] != '\0' ) {
      memset( &mat_pag[ PR((lin - 2), 0) ], UG_HOR, col);
      poe_mat( lin-1, 0, rodape, strlen( rodape )) ;
   }

   sprintf( aux, M_EMI_DATA, data_hora( 'D', 0L ) );
   tam = strlen( aux );
   poe_mat(prel->rel_l_inter, col-tam, aux, tam);

   sprintf( aux, M_EMI_PAGE, num_pagina + off_pagina);
   tam = strlen( aux );
   poe_mat(prel->rel_l_inter+1, col-tam, aux, tam);

   memset( &mat_pag[ PR((prel->rel_l_inter+2), 0 ) ], UG_HOR, col);

   offset = (prel->rel_l_inter + 3); /* +3 pelo cabecalho */
}


F__LOC VOID cab_proprio( )
{  SS i;
   SS lin,  col;
   TEXT *rodape;

   lin        = (prel->rel_linhas - prel->rel_c_inter );
   col        = prel->rel_colunas;

   if ( grel->nlin_rodape > 0 )
      rodape = grel->rodape[0];
   else
      rodape = prel->rel_rodape;

   if ( strlen(rodape) > col )
      rodape[col-1] = '\0';

   if( rodape[0] != '\0' ) {
      memset( &mat_pag[ PR((lin - 2), 0) ], UG_HOR, col);
      poe_mat( lin-1, 0, rodape, strlen( rodape )) ;
      poe_arroba(rodape, lin - 2, 0, BASE_EDIT, 0);
   }

   for ( i=0; i < grel->nlin_cabec; i++) {
      poe_mat(offset++, 0, grel->cabec[i], strlen(grel->cabec[i]));
      poe_arroba(grel->cabec[i], i, 0, BASE_EDIT, 1);
   }
}




F__LOC   VOID limpa_flag( pi, nu_elem )
RTela **pi;
SS nu_elem;
{  SS i;

   for ( i = 0 ; i < nu_elem ; i++ )
       pi[ i ]->re_status = NAO_TRATOU;
}



F__GLB   LONG push_linha(flag, argumento)
COUNT flag;
TEXT *argumento;
{  LONG ret = 0;
   COUNT i;
   static LONG qtd_buffer;    /* numero de linhas no vetor (maximo) */
   static LONG qtd_usadas;    /* numero de linhas no vetor (usadas) */
   static LONG estrategia, primeiro;
   static LONG prox;
   static UCOUNT tam;
   static TEXT *buf = NULL;
   static FILE *fp;
   static TEXT fname[10], f_out[10];
   TEXT aux[ 250 ];
   TEXT *pl, ant;

   switch ( flag ) {

      case INICIALIZA :

               primeiro = 1;

               /* tamanho de cada linha */
               prox = 0;
               tam = atoi(argumento);
               if ( tam == 0 ) tam = 1;

               /* Linha para quardar a linha anterior. Controle de quebras */
               if ( (lin_sort = malloc(tam)) == NULL )
                  fim_lb("856");

               /* calc. qtd de linhas possiveis na memoria */
               qtd_buffer = TM_BUF_ORD / tam;

               buf = calloc((UCOUNT)qtd_buffer + 1, tam);


               if ( buf == NULL ) {
                  estrategia = 'F';
                  strcpy(fname, ARQ_SORT);

                  if ( (char *)mktemp( fname ) != NULL ) {
                     strcpy(f_out, fname);
                     f_out[2] = 'O';
                     if (access(f_out, 0 ) ) {
                        unlink(f_out);
                     }
                  }

                  fp = abre_sortarq(fname, M_ESCRITA);

                  if ( fp == NULL ) {
                     ret = -1;
                     goto fim;
                  }

               }
               else {
                  estrategia = 'M';
                  qtd_usadas = 0;
               }
               break;


      case FINALIZA :
               if ( lin_sort != NULL ) {
                  free(lin_sort);
                  lin_sort = NULL;
               }

               if ( estrategia == 'M' ) {
                  if ( buf != NULL )
                     free(buf);
                  buf = NULL;
               }
               else {

                  if ( buf != NULL ) {
                     free ( buf );
                     buf = NULL;
                  }

                  fclose(fp);
                  unlink(fname);  /* remove arquivo criado  */
               }

               break;

      case PUSH :

               if ( estrategia == 'M' ) {

                  strcpy(&buf[(UCOUNT)qtd_usadas * tam], argumento);
                  qtd_usadas++;

                  if ( qtd_usadas >= qtd_buffer ) {
                     SS i;

                     estrategia = 'F';

                     strcpy(fname, ARQ_SORT);

                     if ( (char *)mktemp( fname ) != NULL ) {
                        strcpy(f_out, fname);
                        f_out[2] = 'O';
                        if (access(f_out, 0 ) ) {
                           unlink(f_out);
                        }
                     }

                     fp = abre_sortarq(fname, M_ESCRITA);

                     if ( fp == NULL )  {
                        ret = -1;
                        goto fim;
                     }

                     for( i = 0; i < qtd_usadas; i++ )   {
                        fputs( &buf[ i * tam ], fp );
                        fputc( '\n', fp );
                     }
                  }
               }
               else {
                  qtd_usadas++;
                  fputs( argumento, fp );
                  fputc( '\n', fp );
               }
               break;

      case ORDENA :

               if ( estrategia == 'M' ) {
                  qsort( buf, (UCOUNT)qtd_usadas, tam, (int (* )())strcmp);
               }
               else {

                  fclose( fp );

                  sprintf( aux, "%s <%s >%s", ap_sort_lb, fname, f_out );

                  win_shell( aux );

                  unlink( fname );
#ifdef MSDOS
                  rename( f_out, fname );
#endif

#ifdef VAXVMS
                  rename( f_out, fname );
#endif

#if !defined ( MSDOS ) && ! defined ( VAXVMS )
                  link( f_out, fname );
                  unlink( f_out );
#endif
                  fp = abre_sortarq(fname, M_LEITURA);
               }

               break;

      case POP :

               if ( estrategia == 'M' ) {
                  SC *xbuf;

                  pl = &buf[(UCOUNT)prox * tam];
                  prox++;

                  if ( prox > qtd_usadas ) {
                     ret = -1;
                     goto fim;
                  }


                  xbuf = &buf[ ((UCOUNT)prox * tam) - SIZE_NR ];
                  ant = xbuf[ SIZE_NR ];
                  xbuf[ SIZE_NR ] = '\0';
                  ret = atol( xbuf );
                  xbuf[ SIZE_NR ] = ant;
               }
               else {

                  if ( fp == NULL )   {
                     ret = -1;
                     goto fim;
                  }

                  prox++;

                  if ( prox > qtd_usadas ) {
                     ret = -1;
                     goto fim;
                  }

                  fgets( aux, tam+1, fp );
                  pl = aux;
                  ret = atol( &aux[ strlen( aux ) - SIZE_NR ] );
               }

               nivel_qbr = 99;

               for ( i = 1; i <= max_ord; i++) {
                  if ( memcmp(&lin_sort[qbr[i].qb_offset],
                              &pl[qbr[i].qb_offset],
                              qbr[i].qb_tamanho) != 0 ) {


                     if ( ! primeiro )
                        nivel_qbr = MIN(nivel_qbr, i);

                     switch(i) {
                        case 1 :
                                 QQ1 = QQ2 = QQ3 = 0;
                                 qbr[1].qb_ocorre = 1;
                                 qbr[2].qb_ocorre = 1;
                                 qbr[3].qb_ocorre = 1;
                                 break;
                        case 2 :
                                 if ( grel->ctl_quebra == QB_IFQUEBRA ) {
                                    QQ1 = 0;
                                 }
                                 QQ2 = QQ3 = 0;
                                 qbr[2].qb_ocorre = 1;
                                 qbr[3].qb_ocorre = 1;
                                 break;
                        case 3 :
                                 if ( grel->ctl_quebra == QB_IFQUEBRA ) {
                                    QQ1 = 0;
                                    QQ2 = 0;
                                 }
                                 QQ3 = 0;
                                 qbr[3].qb_ocorre = 1;
                                 break;
                     }


                     memcpy(&lin_sort[qbr[i].qb_offset],
                            &pl[qbr[i].qb_offset],
                            qbr[i].qb_tamanho);
                  }
                  else
                     qbr[i].qb_ocorre++;

               }

               if ( nivel_qbr == 99 )
                  nivel_qbr = 0;

               primeiro = 0;
               break;
   }

fim :
   return(ret);
}


F__LOC   FILE *abre_sortarq(fname, modo)
SC *fname;
SC *modo;
{
   FILE *fd;

   fd = fopen ( fname, modo );

   return ( fd );
}

F__LOC   SS poe_item_mat( sele, prt, requer )
SS sele;
RTela *prt;
SS requer;
{

   SS    lin, col, tam, usou, ret, x;
   SC    *p_display, *p_campo;
   SC    *masc;

   ret = usou = 1;

   lin  = offset;
   col  = prt->re_coluna;


   switch( prt->re_tipo ) {
      case DISPLAY :
         if ( prel->rel_tipo == FORMATADO ) {

            p_display =  ( SC * ) prt + sizeof( RTela );

            if ( requer != -1 ) {
               poe_mat( lin, col, p_display, 0);
               if ( prt->re_param[0] & CR_BOLD )  {
                  x = strlen(p_display);
                  tam = MIN(x, prt->re_param[1]);
                  x = PR(lin, col);
                  mat_atrib[x] = ET_ENFAT;
                  mat_atrib[x+tam] = ET_NORMAL;
               }
               /* coloca @D, @H, @P para data, hora, nu_pagina....... etc */
               poe_arroba( p_display, lin, col, sele, 0);
            }
            else {
               /* apagar display em funcao e campo sem informacao */
               br_mat( lin, col, strlen(p_display));
            }

         }
         break;

      case CAMPO_E :
      case CAMPO_S :
      {
         if ( vetcp[ prt->re_ivetcp ]->cp_tipo == CP_TEXTO ) {
            ret = imp_texto( sele, vetcp[ prt->re_ivetcp ], prt,
                          lin, col, ( ult_linha_util - lin ), &usou);

            switch ( ret ) {
               case -1 :
                  /*  erro na impressao */
                  ret = 0;
                  goto fim;

               case 0  :
                  /* .... imprimiu tudo */

                  if (vetcp[ prt->re_ivetcp ]->cp_adoc != NULL ) {
                     if ( em_edicao == 0 ) {
                        free_buffer(vetcp[ prt->re_ivetcp ]->cp_adoc);
                        vetcp[ prt->re_ivetcp ]->cp_adoc = NULL;
                     }
                  }

                  ret = usou;
                  break;

               default :
                  /* AINDA NAO imprimiu tudo; */
                  ret = -1;
                  prt->re_status = NAO_TRATOU;
                  break;
            }
         }
         else  {

            SC buf[ TM + 1 ]; /**** observar tamanho maximo do campo ***/

            p_campo   =  vetcp[ prt->re_ivetcp]->cp_newdata;

            /* controla a impressao de itens de quebra */
            if ( max_ord > 0 && ja_imprimiu(vetcp[ prt->re_ivetcp]) == 1 ) {
               /* nao faz nada */
            }
            else {
               tam = strlen( p_campo );

               if (( vetcp[prt->re_ivetcp]->cp_tipo == 'N'   ||
                     vetcp[prt->re_ivetcp]->cp_tipo == 'F'))   {

                  if ( vetcp[prt->re_ivetcp]->cp_tipo == 'F' )
                     masc  = vetcp[prt->re_ivetcp]->cp_param.cp_msc_form;
                  else
                     masc = vetcp[prt->re_ivetcp]->cp_mascara;

                  formata_campo(buf, p_campo, masc, E_VALOR );
                  tam = strlen( buf );

                  p_campo = buf;

                  col = col + prt->re_vhor - tam;
               }

               if ( prt->re_vhor > 0 )
                  tam = MIN(prt->re_vhor, tam);

               p_campo[ tam ] = '\0';

               if ( prt->re_param[0] & CR_BOLD )  {
                  x = PR(lin, col);
                  mat_atrib[x] = ET_ENFAT;
                  mat_atrib[x+tam] = ET_NORMAL;
               }
               else if ( c_impress.palavras_negrito == SIM )
                  ctrel_enfat( lin, col, (SC *)p_campo);



               poe_mat(lin, col, p_campo, tam);
            }
         }

         if ( prel->rel_tipo == COLUNADO &&
               ( vetcp[prt->re_ivetcp]->cp_tipo == 'N' ||
                  vetcp[prt->re_ivetcp]->cp_tipo == 'F' ) ) {
            totaliza_colunado( prt, p_campo );
         }
         break;

      }
      case LIN_HOR :
      {
         memset( &mat_pag[PR(lin, col)], UG_HOR, prt->re_vhor-col+1);
         break ;
      }
      case LIN_VER :
      {
         UC lin_f;

         for  (lin_f = (requer-1) + offset ; lin <= lin_f ; lin++ ) {
            mat_pag[ PR( lin, col ) ] = UG_VER;
         }
         break ;
      }
      case BORDA :
      {
         UC lin_f, col_f, cont;

         lin_f   =  (requer - 1) + offset;
         col_f   =  prt->re_vhor;

         mat_pag[PR( lin, col ) ]     =  UG_UL;
         mat_pag[PR( lin, col_f ) ]   =  UG_UR;
         mat_pag[PR( lin_f, col ) ]   =  UG_LL;
         mat_pag[PR( lin_f, col_f ) ] =  UG_LR;

         memset(&mat_pag[ PR(lin,  (col+1))], UG_HOR, col_f-col-1);
         memset(&mat_pag[ PR(lin_f,(col+1))], UG_HOR, col_f-col-1);

         /* linhar vertical */
         for ( cont = lin + 1; cont < lin_f; cont++ ) {
            mat_pag[PR( cont, col ) ]   =  UG_VER;
            mat_pag[PR( cont, col_f ) ] =  UG_VER;
         }

         p_display =  ( SC * ) prt + sizeof( RTela );
         poe_mat( lin, col+1, p_display, strlen( p_display ) );
         break;
      }
   } /* switch tipo */


fim:
   return ( ret );

}

/*---------- ja_imprimiu() ----------*/

F__LOC   SS ja_imprimiu(pc)
Campo *pc;
{  COUNT ret = 0, i;

   for ( i = 1; grel->ctl_quebra != QB_TODOS && i <= max_ord; i++) {
      if ( pc == qbr[i].qb_campo ) {
         switch(i) {
            case 1 : if ( QQ1++ > 0 )
                        { ret = 1; i=99;}
                     break;

            case 2 : if ( QQ2++ > 0 )
                        { ret = 1; i=99;}
                     break;

            case 3 : if ( QQ3++ > 0 )
                        { ret = 1; i=99;}
                     break;
         }
      }
   }

   return(ret);
}


/*---------- pergunta_salto () ----------*/

F__LOC   SS pergunta_salto( )
{
   return( men_conf(M_EMI_SALPAG, OP_CONFIRMA, H_CFAEDI, NULL) );
}

/*---------- totaliza_colunado () ----------*/

F__LOC   VOID totaliza_colunado( prt, p_campo )
RTela *prt;
SC *p_campo;
{
   if ( prt->re_indtot == -1 )
      return;

   col_tot[ prt->re_indtot ].soma_1 += str_to_db( p_campo, sep_dec);
   col_tot[ prt->re_indtot ].qtd_1++;

   col_tot[ prt->re_indtot ].soma_2 += str_to_db( p_campo, sep_dec);
   col_tot[ prt->re_indtot ].qtd_2++;

   col_tot[ prt->re_indtot ].soma_3 += str_to_db( p_campo, sep_dec);
   col_tot[ prt->re_indtot ].qtd_3++;

   if ( prt->re_desvio == 'S' ) {
      DB somat_2;

      somat_2 = str_to_db( p_campo, sep_dec );
      somat_2 = somat_2 * somat_2;
      col_tot[ prt->re_indtot ].desv_1 += somat_2;
      col_tot[ prt->re_indtot ].desv_2 += somat_2;
      col_tot[ prt->re_indtot ].desv_3 += somat_2;
      col_tot[ prt->re_indtot ].desv_total  += somat_2;
   }

   col_tot[ prt->re_indtot ].soma_total += str_to_db( p_campo, sep_dec);
   col_tot[ prt->re_indtot ].qtd_total++;
}

/*---------- poe_mat( ) ----------*/

F__GLB   VOID poe_mat( l, c, str, tam )
SS l, c;
SC *str;
SS tam;
{
   SS pos, tam_matriz, cl;
   SC *linha;
   tam_matriz = prel->rel_linhas * prel->rel_colunas + 1;

   pos = (( l * prel->rel_colunas) + c );

   if ( (pos+tam) > tam_matriz )
      numdebug( 2006 );

   linha = str;
   cl    = pos;

   while( *linha )  {
      UC chr;

      chr = *linha;

      if( chr != BRANCO ) {

         if( chr > ET_ITAL ) {
            mat_pag[cl]   = chr;
         }
         else  {
            mat_atrib[cl] = chr;
            cl--;
         }
      }
      cl++;
      linha++;
   }
}

/*---------- poe_mat( ) ----------*/
F__LOC   VOID br_mat( l, c, tam )
SS l, c;
SS tam;
{
   SS cl;

   cl = (( l * prel->rel_colunas) + c );

   while( tam-- )  {
      mat_pag[cl] = ' ';
      cl++;
   }
}

/*---------- monta_cabec( ) ----------*/

F__LOC   VOID monta_cabec( )
{
   SC *p = prel->rel_titulo ;

   if( *p == '[' && strchr(p, '-') && strchr(p, ']')){
      p++;

      cb_inicio = atoi( p );

      while( *p != '-' )     p++;

      p++;

      cb_fim = atoi( p );

      while( *p != ']' )    p++;

      p++;

      strcpy( prel->rel_titulo, p );
   }
}

/*---------- novo_varre_itens( ) ----------*/

F__LOC   SS novo_varre_itens( l, sele )
SS l, sele;
{
   SS cont, requer = 1, usou, ret, u;
   RTela *piten;


loop :

   usou = 1;

   for ( cont = 0; cont < prel->rel_n_elem; cont++ ) {

      piten  =  (RTela *)prel->rel_vet->vi_vtit[cont];   /* aponta para cont elemento */

      if ( (piten->re_tipo == CAMPO_E || piten->re_tipo == CAMPO_S) &&
           vetcp[ piten->re_ivetcp ]->cp_tipo == CP_TEXTO )
         continue;

      if ( piten->re_linha != l ) continue;

      if (piten->re_tipo == LIN_VER || piten->re_tipo == BORDA )
        requer = piten->re_vver - piten->re_linha + 1;
      else
         requer = 1;

      u    = poe_item_mat( sele, piten, requer );

      if ( u == -1 )
         usou = -1;

      if( usou != -1 )
         usou = MAX(u, usou);

   }  /* for nu_elem */

   if ( usou == -1  ) { /* imprimiu o maximo que podia na pagina */
      offset = 999;
      goto loop;
   }

   offset += usou;

   ret = 0;

fim:

   if ( cont < 0 )
      ret = -1;

   return( ret );
}


/*---------- trata_atrib( ) -----------*/

F__LOC   VOID trata_atrib( atrib )
UC atrib;
{

   static SS atrib_ant = ET_NORMAL;

   if( atrib_ant != atrib ) {

      if( atrib_ant != ET_NORMAL ) {
         switch( atrib_ant ) {
            case ET_SUBLIN :
               imp_puts( device, i_atrib[ SUBLINHADO ].at_off);
               break;

            case ET_ENFAT  :
               imp_puts( device, i_atrib[ ENFATIZADO ].at_off);
               break;

            case ET_ITAL   :
               imp_puts( device, i_atrib[ ITALICO ].at_off);
               break;
         }
      }

      switch( atrib ) {
         case ET_NORMAL :
            imp_puts( device, i_atrib[ NORMAL ].at_on);
            break;

         case ET_SUBLIN :
            imp_puts( device, i_atrib[ SUBLINHADO ].at_on);
            break;

         case ET_ENFAT  :
            imp_puts( device, i_atrib[ ENFATIZADO ].at_on);
            break;

         case ET_ITAL   :
            imp_puts( device, i_atrib[ ITALICO ].at_on);
            break;
      }
      atrib_ant = atrib;
   }
}

/*---------- quant_atrib( ) ----------*/

F__LOC   SS quant_atrib( linha )
SC *linha;
{
   SS n;

   for( n = 0;  *linha; linha++ )  {
      if( *linha >= ET_NORMAL && *linha <= ET_ITAL )
         n++;
   }
   return( n );
}

/*----------------- poe_arroba( ) -----------------*/

F__LOC   VOID poe_arroba( str_ok, lin, col, sele, cabecalho )
SC *str_ok;
SS lin, col, sele, cabecalho;
{
   TEXT num[15], *pi, *p, *str, *str_base, *ptok, *p_aux;
   COUNT l, i, tam;

   if ( strchr(str_ok, '@') == NULL )
      return;

   if ( (str_base = str = malloc(strlen(str_ok)+1)) == NULL )
      sem_memoria();

   strcpy(str, str_ok);

   while ( (ptok = strchr(str, '@')) != NULL ) {
      ptok++;
      str = ptok;

      switch(*ptok) {
         case 'B' :
                  pi = strstr( &mat_pag[PR(lin,col)], "@B" );
                  if ( pi != NULL ) {
                     pi[0] = ' '; pi[1] = ' ';
                     memcpy( pi, desc_base, strlen(desc_base));
                  }
                  break;

         case 'C' :
                  pi = strstr( &mat_pag[PR(lin,col)], "@C" );
                  if ( pi != NULL ) {
                     pi[0] = ' '; pi[1] = ' ';
                     memcpy( pi, ap_cliente, strlen(ap_cliente) );
                  }
                  break;

         case 'D' :
                  pi = strstr( &mat_pag[PR(lin,col)], "@D" );
                  if ( pi != NULL ) {
                     memcpy( pi, data, 8 );
                  }
                  break;

         case 'F' :

                  pi = strstr(&mat_pag[ PR(lin, col) ], "@F");

                  if ( pi != NULL ) {

                     p = field_info(BASE_EDIT, -1, -1, pi+2, -1);

                     /* limpa diretiva com nome do campo */
                     p_aux = pi;

                     while ( *p_aux != ' ' && *p_aux != '\0' && *p_aux != '-')
                        *p_aux++ = ' ';

                     if ( p != NULL )
                        memcpy( pi, p, strlen(p) );
                  }
                  break;

         case 'H' :
                  pi = strstr( &mat_pag[PR(lin,col)], "@H" );
                  if ( pi != NULL ) {
                     memcpy( pi, hora, 5 );
                  }
                  break;

         case 'O' :
                  pi = strstr(&mat_pag[ PR(lin, col) ], "@O");
                  if ( pi != NULL ) {
                        p  = pi + 2;
                        if ( *p >= '1' && *p <= ('0' + max_ord) ) {

                           i = *p - '0';

                           /* verifica se deve limitar tamanho */
                           if ( *(p+1) == ',' ) {
                              p += 2;
                              tam = atoi(p);
                              if (tam == 0 )
                                 tam = 1;
                           }
                           else
                              tam = qbr[i].qb_tamanho;

                           pi[0] = ' '; pi[1] = ' '; pi[2] = ' ';

                           if ( qbr[i].qb_ocorre <= 1 || cabecalho )
                              memcpy( pi, &lin_sort[qbr[i].qb_offset], tam );
                           else
                              memset( pi, ' ', tam );
                     }
                  }
                  break;

         case 'P' :
                  pi = strstr( &mat_pag[PR(lin,col)], "@P" );
                  if ( pi != NULL ) {
                     TEXT form[20], *pf;

                     pf = form;
                     *pf++ = '%';

                     p = pi + 2;

                     if ( *p == '-' || *p == '0' ) { /* para tratar formato -n.n */
                        *pf++ = *p++;
                     }

                     if ( *p >= '1' && *p <= '9' ) {
                        *pf++ = *p++;
                        switch ( *p ) {
                           case '.' : p++;
                                      if ( (*p >= '1' && *p <= '9') && *(p+1) == ' ' ) {
                                         *pf++ = '.';
                                         *pf++ = *p;
                                         *pf++ = 'd';
                                      }
                                      else {
                                         pf = form;
                                         *pf = '\0';
                                      }
                                      break;

                           case ' ' :  *pf++ = 'd';
                                       break;

                           default  : pf = form;
                                      *pf = '\0';
                                      break;
                        }

                        *pf = '\0';
                     }
                     else
                        form[0] = '\0';

                     if ( form[0] == '\0' )
                        strcpy(form, "%6d");

                     sprintf( num, form, num_pagina + off_pagina);
                     memset( pi, ' ', strlen(form) );
                     memcpy( pi, num, strlen(num) );
                  }
                  break;

         case 'R' :
                  pi = strstr(&mat_pag[ PR(lin, col) ], "@R");
                  if ( pi != NULL ) {
                     p  = pi + 2;

                     if ( *p >= '1' && *p <= '9' )
                        l = *p - '0';
                     else
                        l = 5;

                     sprintf( num, "%12.12ld", c_bases[sele]->cb_numrec);
                     memcpy( pi, &num[12-l], l );
                  }
                  break;

         case 'S' :
                  pi = strstr(&mat_pag[ PR(lin, col) ], "@S");
                  if ( pi != NULL ) {
                     p  = pi + 2;

                     if ( *p >= '1' && *p <= '9' )
                        l = *p - '0';
                     else
                        l = 5;

                     sprintf( num, "%12.12ld", seq_impr);
                     memcpy( pi, &num[12-l], l );
                  }
                  break;

         case 'U' :
                  pi = strstr( &mat_pag[PR(lin,col)], "@U" );
                  if ( pi != NULL ) {
                     pi[0] = ' '; pi[1] = ' ';
                     memcpy( pi, ap_nome, strlen(ap_nome) );
                  }
                  break;

      }
   }

   free(str_base);
}




F__LOC   SI  bqr_comp( S1,  S2 )
SC    **S1, **S2;
{
   return( strcmp( *S1, *S2 ) );
}



VOID escrava_on()
{  COUNT cil;

   /* controla desvio de impressao escrava/video */
   for( cil=0; cil < 6; cil++)
      if ( c_impress.escrava[cil] == 0 )
         break;
      else
         fputc( c_impress.escrava[cil], device );

   fflush(device);
}

VOID escrava_off()
{  COUNT cil;

   /* controla desvio de impressao escrava/video */
   for( cil=0; cil < 6; cil++)
      if ( c_impress.video[cil] == 0 )
         break;
      else
         fputc( c_impress.video[cil], device );

   fflush(device);
}


#define   STR_HDLIN   "%8.8ld %-14.14s %s"
F__LOC   COUNT exporta_rel(sele, buf, fp)
COUNT sele;
TEXT *buf;
FILE *fp;
{  COUNT ret=0, i, ind, qtd;
   Campo **vetcmp, *pc;
   TEXT *p_aux;


   vetcmp = pos_vetcmp(sele);
   qtd = qtd_vetcmp(sele);

   for ( i = 0; i < qtd; i++) {

      ind = i;

      if ( ind != -1 ) {

         pc  = vetcmp[ind];

         if ( pc->cp_tipo == CP_TEXTO ) {
            if ( exp_lin_texto(pc, buf, fp) != 0 ) {
               ret = -1;
               goto fim;
            }
            continue;
         }
         else {
            p_aux = pc->cp_newdata;
            trim(p_aux);
         }

         if ( *p_aux != '\0' ) {
            sprintf(buf, STR_HDLIN, WCB->cb_numrec, pc->cp_nome,  p_aux);
            if ( fputs(buf, fp) == EOF || fputc('\n', fp) == EOF ) {
               mens_erro("", "Erro na gravacao de arquivo de exportacao no relatorio\n");
               ret = -1;
               goto fim;
            }
         }
      }
   }

fim :

   return(ret);
}

F__LOC   COUNT exp_lin_texto(pc, buf, fp)
Campo *pc;
TEXT *buf;
FILE *fp;
{  COUNT ret, num_lin;
   ARG *ant_adoc;
   LONG tam;
   POINTER pos;
   TEXT linha[250];

   tam = 0L;
   num_lin = 0;
   ret = 0;

   ant_adoc = pc->cp_adoc;
   if ( pc->cp_adoc == NULL ) {

      memcpy((TEXT *)&pos, (TEXT *)pc->cp_newdata , sizeof(pos));

      if ( pos != (POINTER) 0 ) {
         pc->cp_adoc = txt_load(AD_NUM(BASE_EDIT), pos, &tam, pc->cp_mhor, WCB->cb_numrec);
         if ( pc->cp_adoc == NULL )  {
            debug(M_PRDTXT, c_bases[BASE_EDIT]->cb_numrec, pc->cp_nome);
            ret = 0; /* Esta dando debug para nao comprometer todo o resto */
            /* ret = -1; */
            goto fim;
         }
      }
   }


   if ( tam != 0 ) { /* tem informacao para gravar no arquivo */
      while ( lin_texto( pc->cp_adoc, num_lin++, linha, 0) > 0 ) {
        	sprintf(buf, STR_HDLIN, WCB->cb_numrec, pc->cp_nome, linha);
         if ( fputs(buf, fp) == EOF || fputc('\n', fp) == EOF ) {
            mens_erro("", "Erro na gravacao de arquivo de exportacao no relatorio\n");
            ret = -1;
            goto fim;
         }
      }
   }

fim :

   if ( ant_adoc == NULL && pc->cp_adoc != NULL) {
      free_buffer(pc->cp_adoc );
      pc->cp_adoc = NULL;
   }

   return(ret);
}

