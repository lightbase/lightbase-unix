/*---------- lb2rel0.c ----------*/
#include  "lb2all.h"

#include  <string.h>
#include  <math.h>

#include  "lb2stru.h"
#include  "lb2vars.h"
#include  "lb2text.h"  /* strings comus a todos os modulos */

#include  "lb2entra.h"
#include  "lb2files.h"

#include  "lb2prot1.h"


#define   MODO_DEF_REL

#include  "lb2rel.h"

#define   MAX_MASC     20        /* tamanho maximo de caracteres em mascara */



/* em nivel_quebra e total_geral */
#define   M_RE0_TOTAL    loc_str( GRP_IMPREL,  1)  /* "Total......."            */
#define   M_RE0_QUANT    loc_str( GRP_IMPREL,  2)  /* "Quantidade.."            */
#define   M_RE0_MEDIA    loc_str( GRP_IMPREL,  3)  /* "Media......."            */
#define   M_RE0_DESVIO   loc_str( GRP_IMPREL,  4)  /* "Desv. padrao"            */
#define   M_RE0_TOTGER   loc_str( GRP_IMPREL,  5)  /* "Total Geral."            */



#ifdef MSDOS
   SS nivel_quebra( SS, SS);
   SS lin_p_quebra(SS, SS, SS *, SS *, SS *, SS *);
   SS imp_total_geral( SS );
   VOID monta_mascara( SC *, SS, SS);
#else
   SS nivel_quebra( );
   SS lin_p_quebra( );
   SS imp_total_geral( );
   VOID monta_mascara( );
#endif





/*---------- emite() ----------*/

F__GLB   SS emite( sele, pfun, palavras)
SS sele;
#ifdef MSDOS
   SL (*pfun)( SS, SL );
#else
   SL (*pfun)( );
#endif
SC **palavras;
{  SS    cod_rel,  ret, i;


   up_str(GRP_IMPREL);

   if ( (grel = (Grel *)calloc(1, sizeof(Grel))) == NULL )
      fim_lb("err 011-01"); 

   ap_ignorauso = 1;

   cod_rel     =  open_rel(sele, '?', UTILIZACAO);

   if ( cod_rel <= 0 ) {
      ret = 0;
      goto fim;
   }

   pl_br_rel = palavras;

   tipo_corrente = 0;

   prel      =  &ap_rel;     /* ponteiro para relatorio */
   vetcp     = pos_vetcmp(sele);


   /* conta numero de palavras no vetor de palavars a brilhar */
   qtd_plrel = 0;
   if ( palavras != NULL ) {
      for ( ; palavras[ qtd_plrel ] != NULL;  qtd_plrel++ )
         ;
   }

   switch(  prel->rel_tipo ) {
      case FORMATADO :
         rel_form( sele, pfun);
         break;

      case COLUNADO :
         rel_form( sele, pfun);
         break;

      case ETIQUETA :
         rel_etiq( sele, pfun);
         break;
   }

fim:

   for ( i=0; i < grel->nlin_cabec; i++ )
      if ( grel->cabec[i] != NULL)
         free(grel->cabec[i]);
   for ( i=0; i < grel->nlin_rodape; i++ )
      if ( grel->rodape[i] != NULL)
         free(grel->rodape[i]);

   if ( cod_rel >= 0 )
      close_rel( );

   ap_ignorauso = 0;
   free(grel);
   grel = NULL;

   down_str(GRP_IMPREL);

   return ( ret );
}






F__GLB   SS quebra_reg ( flag, sele, deve_totalizar)
SS flag;
SS sele;
SS deve_totalizar;
{
   SS ret;

   ret = 0;

   switch( flag )   {

      case COMPARA :

         /* nivel_qbr foi setado em push_linha */

         if ( nivel_qbr ) {
            switch(nivel_qbr) {
               case 1 : ret = prel->rel_1linhas; break;
               case 2 : ret = prel->rel_2linhas; break;
               case 3 : ret = prel->rel_3linhas; break;
            }


            if ( deve_totalizar && nivel_qbr > 0 ) {
               if ( nivel_quebra( sele, nivel_qbr ) < 0 ) {
                  ret = -1;
                  goto fim;
               }
            }
         }
         break;


      case TOTALIZA :

         if ( prel->rel_3numcampo != 0 ) nivel_qbr = 3;
         if ( prel->rel_2numcampo != 0 ) nivel_qbr = 2;
         if ( prel->rel_1numcampo != 0 ) nivel_qbr = 1;

         if ( deve_totalizar && nivel_quebra( sele, nivel_qbr ) < 0 ) {
            ret = -1;
            goto fim;
         }

         if ( deve_totalizar )
            imp_total_geral( sele );

         break;

      case FINALIZA :

         QQ1 = QQ2 = QQ3 = 0;
   }

fim:
   return ( ret );
}



F__LOC   SS nivel_quebra( sele, quebra )
SS sele, quebra;
{
   SS item, col, tam, i, requer, ret, nivel, nv_max, c1, c2, c3, CI;
   DB valor, somatorio, media;
   SL quant;
   SC campo[ 150 ], masc[ TM+1 ], buf[ 150 ], carac;
   RTela **PI;
   SS tot, med, des, qtd;

   CI = col_inicial_totais;  /* coluna inicial para totais */
   PI = (RTela **)prel->rel_vet->vi_vtit;
   ret = 0;
   nv_max = 0;

   if ( prel->rel_1numcampo != 0 ) nv_max++;
   if ( prel->rel_2numcampo != 0 ) nv_max++;
   if ( prel->rel_3numcampo != 0 ) nv_max++;


   if      ( nv_max == 3 ) {   c1 = DELIM[3];   c2 = DELIM[2];   c3 = DELIM[1];  }
   else if ( nv_max == 2 ) {   c1 = DELIM[2];   c2 = DELIM[1];   c3 = ' ';  }
   else                    {   c1 = DELIM[1];   c2 = ' ';   c3 = ' ';  }

   for ( nivel = nv_max; nivel >= quebra; nivel--) {

      requer = lin_p_quebra( sele, nivel, &tot, &qtd, &med, &des );

      /* calcula o que vai ser acrescentado em offset para imprimir */
      if      ( tot == 1 ) {  qtd++;  med++;  des++;  }
      else if ( qtd == 1 ) {          med++;  des++;  }
      else if ( med == 1 ) {                  des++;  }

      if      ( qtd == 2 ) {          med++;  des++;  }
      else if ( med == 2 ) {                  des++;  }

      if      ( med == 3 ) {                  des++;  }

      if( requer == 0 ) {
         continue;
      }

      if ( offset + requer > ult_linha_util ) {
         if ( imprime_pagina( ) == -1 )  { ret = 1;  goto fim; }
         inicializa_pagina( sele );
      }

      for ( i = 0; i < num_soma; i++)   {

         item = col_tot[ i ].n_item;


         switch( nivel ) {
            case 1 :
               valor     = col_tot[ PI[item]->re_indtot ].soma_1;
               somatorio = col_tot[ PI[item]->re_indtot ].desv_1;
               quant     = col_tot[ PI[item]->re_indtot ].qtd_1;
               carac     = c1;
               break;
            case 2 :
               valor     = col_tot[ PI[item]->re_indtot ].soma_2;
               somatorio = col_tot[ PI[item]->re_indtot ].desv_2;
               quant     = col_tot[ PI[item]->re_indtot ].qtd_2;
               carac     = c2;
               break;
            case 3 :
               valor     = col_tot[ PI[item]->re_indtot ].soma_3;
               somatorio = col_tot[ PI[item]->re_indtot ].desv_3;
               quant     = col_tot[ PI[item]->re_indtot ].qtd_3;
               carac     = c3;
               break;
         }

         if ( quant == 0L )
            quant = 1L;

         memset( &mat_pag[PR(offset, PI[ item ]->re_coluna)], carac, PI[item]->re_vhor);

         monta_mascara( masc, MAX_MASC, vetcp[ PI[ item ]->re_ivetcp ]->cp_decimais );

         /*----- soma -----*/

         if( PI[item]->re_tot == 'S' ) {
            db_to_str( valor, campo, 2, 1);
            formata_campo(buf, campo, masc, E_VALOR );
            tam = strlen( buf );
            if ( tam > PI[item]->re_vhor) { /* overflow.Enche de '*' */
               tam = PI[item]->re_vhor;
               memset(buf, '*', tam);
               buf[tam] = '\0';
            }
            col = PI[ item ]->re_coluna + PI[item]->re_vhor - tam;

            poe_mat(offset+tot, CI, M_RE0_TOTAL, strlen(M_RE0_TOTAL));
            poe_mat(offset+tot, col, buf, tam );
         }

         /*------ quantidade -----*/

         if( PI[item]->re_quant == 'S' ) {
            SC masc_qtd[ TM + 1];
            sprintf(campo, "%10.10ld", quant);  /* ltoa( quant, campo, 10 ) ****/
            monta_mascara( masc_qtd, MAX_MASC, 0);
            formata_campo( buf, campo, masc_qtd, E_VALOR );
            tam = strlen( buf );
            if ( tam > PI[item]->re_vhor) { /* overflow.Enche de '*' */
               tam = PI[item]->re_vhor;
               memset(buf, '*', tam);
               buf[tam] = '\0';
            }

            col = PI[ item ]->re_coluna + PI[item]->re_vhor - tam;
            poe_mat(offset+qtd,  CI, M_RE0_QUANT, strlen(M_RE0_QUANT) );
            poe_mat(offset+qtd, col, buf, tam );
         }

         /*------ media ------*/

         media = valor / (double) quant ;

         if( PI[item]->re_media == 'S' )  {
            db_to_str( media, campo, 2, 1);
            formata_campo( buf, campo, masc, E_VALOR );
            tam = strlen( buf );
            if ( tam > PI[item]->re_vhor) { /* overflow.Enche de '*' */
               tam = PI[item]->re_vhor;
               memset(buf, '*', tam);
               buf[tam] = '\0';
            }
            col = PI[ item ]->re_coluna + PI[item]->re_vhor - tam;
            poe_mat(offset+med,  CI, M_RE0_MEDIA, strlen(M_RE0_MEDIA) );
            poe_mat(offset+med, col, buf, tam );
         }

         /*----- desvio padrao -----*/

         if( PI[item]->re_desvio == 'S' ) {
            DB media_2, desvio, result;

            media_2 = (DB) (media * media) ;

            desvio = ( somatorio - ( (DB)quant * media_2 )) / (DB) quant ;

            result  = sqrt( desvio );

            db_to_str( result, campo, 2, 1);
            formata_campo( buf, campo, masc, E_VALOR );
            tam = strlen( buf );
            if ( tam > PI[item]->re_vhor) { /* overflow.Enche de '*' */
               tam = PI[item]->re_vhor;
               memset(buf, '*', tam);
               buf[tam] = '\0';
            }
            col = PI[ item ]->re_coluna + PI[item]->re_vhor - tam;
            poe_mat(offset+des,  CI, M_RE0_DESVIO, strlen(M_RE0_DESVIO ) );
            poe_mat(offset+des, col, buf, tam );
         }

         switch( nivel ) {
            case 1 :
               col_tot[ PI[item]->re_indtot ].soma_1 = 0.00;
               col_tot[ PI[item]->re_indtot ].desv_1 = 0.00;
               col_tot[ PI[item]->re_indtot ].qtd_1  = 0L;
               break;
            case 2 :
               col_tot[ PI[item]->re_indtot ].soma_2 = 0.00;
               col_tot[ PI[item]->re_indtot ].desv_2 = 0.00;
               col_tot[ PI[item]->re_indtot ].qtd_2  = 0L;
               break;
            case 3 :
               col_tot[ PI[item]->re_indtot ].soma_3 = 0.00;
               col_tot[ PI[item]->re_indtot ].desv_3 = 0.00;
               col_tot[ PI[item]->re_indtot ].qtd_3  = 0L;
               break;
         }
      } /** for num_soma **/
      offset += requer;
   }  /** for quebra **/

fim:

   return(ret);
}



/*---------- lin_p_quebra( ) ----------*/

F__LOC   SS lin_p_quebra(sele, nivel, tot, qtd, med, des )
SS sele, nivel;
SS *tot, *qtd, *med, *des;
{
   SS quebra, requer, i;
   RTela **PI;

   sele = sele;

   *tot = *qtd = *med = *des = 0;
   quebra = 'N';
   requer = 0;

   switch( nivel ) {
      case 1:
         quebra = prel->rel_1total;
         break;
      case 2:
         quebra = prel->rel_2total;
         break;
      case 3:
         quebra = prel->rel_3total;
         break;
      case 4: /* total geral */
         quebra = 'S';
         break;
   }

   if( quebra == 'S' )   {

      PI = (RTela **)prel->rel_vet->vi_vtit;

      for( i = 0; i < prel->rel_n_elem; i++ )   {

         if( PI[ i ]->re_tot == 'S'  &&  ! *tot ) {
            requer++;
            (*tot)++;
         }

         if( PI[ i ]->re_quant == 'S'  && ! *qtd ) {
            requer++;
            (*qtd)++;
         }

         if( PI[ i ]->re_media == 'S'  && ! *med ) {
            requer++;
            (*med)++;
         }

         if( PI[ i ]->re_desvio == 'S'  && ! *des )  {
            requer++;
            (*des)++;
         }
      }

      if ( requer )
         requer += 2; /* Para a linha separadora mais linha em branco */
   }

   return( requer );
}

/*---------- imp_total_geral( ) ----------*/

F__LOC   SS imp_total_geral( sele )
SS sele;
{
   SS item, col, tam, i, requer, ret, nv_max, CI;
   DB valor, somatorio, media;
   SL quant;
   SC campo[ 150 ], masc[TM+1], buf[ 150 ], carac;
   RTela **PI;
   SS tot, med, des, qtd;

   CI = col_inicial_totais;  /* coluna inicial para totais */
   PI = (RTela **)prel->rel_vet->vi_vtit;
   ret = 0;
   nv_max = 0;

   if ( prel->rel_1numcampo != 0 ) nv_max++;
   if ( prel->rel_2numcampo != 0 ) nv_max++;
   if ( prel->rel_3numcampo != 0 ) nv_max++;

   requer = lin_p_quebra( sele, 4, &tot, &qtd, &med, &des );

   if ( requer == 0 )  goto fim;

   if      ( tot == 1 ) {  qtd++;  med++;  des++;  }
   else if ( qtd == 1 ) {          med++;  des++;  }
   else if ( med == 1 ) {                  des++;  }

   if      ( qtd == 2 ) {          med++;  des++;  }
   else if ( med == 2 ) {                  des++;  }

   if      ( med == 3 ) {                  des++;  }

   if ( offset + requer > ult_linha_util ) {
      if ( imprime_pagina( ) == -1 )  { ret = 1;  goto fim; }
      inicializa_pagina( sele );
   }

   for ( i = 0; i < num_soma; i++)   {

      item = col_tot[ i ].n_item;

      valor     = col_tot[ PI[item]->re_indtot ].soma_total;
      somatorio = col_tot[ PI[item]->re_indtot ].desv_total;
      quant     = col_tot[ PI[item]->re_indtot ].qtd_total;
      carac     = DELIM[4];

      if ( quant == 0L )
         quant = 1L;

      memset( &mat_pag[PR(offset, PI[ item ]->re_coluna)], carac, PI[item]->re_vhor);

      monta_mascara( masc, MAX_MASC, vetcp[ PI[ item ]->re_ivetcp ]->cp_decimais );

      /* soma */

      if( PI[item]->re_tot == 'S' ) {
         db_to_str( valor, campo, 2, 1);
         formata_campo(buf, campo, masc, E_VALOR );
         tam = strlen( buf );
         if ( tam > PI[item]->re_vhor) { /* overflow.Enche de '*' */
            tam = PI[item]->re_vhor;
            memset(buf, '*', tam);
            buf[tam] = '\0';
         }

         col = PI[ item ]->re_coluna + PI[item]->re_vhor - tam;
         poe_mat(offset+tot, CI, M_RE0_TOTGER, strlen(M_RE0_TOTGER));
         poe_mat(offset+tot, col, buf, tam );
      }

      /* quantidade */
      if( PI[item]->re_quant == 'S' ) {
         SC masc_qtd[ TM + 1 ];
         sprintf(campo, "%10.10ld", quant);  /* ltoa( quant, campo, 10 ) ****/
         monta_mascara( masc_qtd, MAX_MASC, 0);
         formata_campo( buf, campo, masc_qtd, E_VALOR );
         tam = strlen( buf );
         if ( tam > PI[item]->re_vhor) { /* overflow.Enche de '*' */
            tam = PI[item]->re_vhor;
            memset(buf, '*', tam);
            buf[tam] = '\0';
         }

         col = PI[ item ]->re_coluna + PI[item]->re_vhor - tam;
         poe_mat(offset+qtd,   CI, M_RE0_QUANT, strlen(M_RE0_QUANT) );
         poe_mat(offset+qtd, col, buf, tam );

      }

      /* media */
      media = valor / (double) quant ;

      if( PI[item]->re_media == 'S' )  {
         db_to_str( media, campo, 2, 1);
         formata_campo( buf, campo, masc, E_VALOR );
         tam = strlen( buf );
         if ( tam > PI[item]->re_vhor) { /* overflow.Enche de '*' */
            tam = PI[item]->re_vhor;
            memset(buf, '*', tam);
            buf[tam] = '\0';
         }
         col = PI[ item ]->re_coluna + PI[item]->re_vhor - tam;
         poe_mat(offset+med,  CI, M_RE0_MEDIA, strlen(M_RE0_MEDIA) );
         poe_mat(offset+med, col, buf, tam );
      }

      /** desvio padrao **/
      if( PI[item]->re_desvio == 'S' ) {
         DB media_2, desvio, result;

         media_2 = (DB) (media * media) ;

         desvio = ( somatorio - ( (DB)quant * media_2 )) / (DB) quant ;

         result  = sqrt( desvio );

         db_to_str( result, campo, 2, 1);
         formata_campo( buf, campo, masc, E_VALOR );
         tam = strlen( buf );
         if ( tam > PI[item]->re_vhor) { /* overflow.Enche de '*' */
            tam = PI[item]->re_vhor;
            memset(buf, '*', tam);
            buf[tam] = '\0';
         }
         col = PI[ item ]->re_coluna + PI[item]->re_vhor - tam;
         poe_mat(offset+des,  CI, M_RE0_DESVIO, strlen(M_RE0_DESVIO ) );
         poe_mat(offset+des, col, buf, tam );
      }
   }

   offset += requer;

fim:

   return ( ret  );
}

/*---------- monta_mascara( ) ----------*/

F__LOC   VOID monta_mascara( masc, tam, dec )
SC *masc;
SS tam, dec;
{
	TEXT linha[ TM+1 ], *p;


   /* prepara mascara padrao */

   p = linha;
   while( tam ) {
      *p++ = '9';
      if ( ((tam - 1) % 3 == 0) && tam > 2 )
         *p++ = sep_int;
      tam--;
   }

   if ( dec > 0 ) {
      *p++ = sep_dec;
      while ( dec-- ) {
         *p++ = '9';
      }
   }

   *p = '\0';

   strcpy( masc, linha );
}

/*----------  col_inic_itens( )  --------------*/
F__LOC   SS col_inic_itens(inic)
SS inic;  /* inicializa estrutura alocada previamente */
{
   SS num_soma;
   SS i, j;
   RTela **PI;

   PI = (RTela **)prel->rel_vet->vi_vtit;

   num_soma = 0;
   for( i = 0, j = 0; i < prel->rel_n_elem; i++ )   {

      if( PI[ i ]->re_tot == 'S'   || PI[ i ]->re_quant == 'S' ||
          PI[ i ]->re_media == 'S' || PI[ i ]->re_desvio == 'S' ) {

            if ( inic == 0 )
               PI[ i ]->re_indtot = num_soma++;
            else
               col_tot[ j++ ].n_item = i ;
      }
      else
            PI[ i ]->re_indtot = -1;
   }
   return ( num_soma );
}
