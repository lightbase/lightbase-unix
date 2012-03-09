#include  "lb2all.h"

#include  <string.h>

#include  "lb2stru.h"
#include  "lb2vars.h"
#include  "lb2text.h"  /* strings comus a todos os modulos */

#ifdef    VAXVMS
#include  "tabela.h"
#else
#include  "../mt/tabela.h"
#endif

#include  "lb2bases.h"
#include  "lb2locs.h"
#include  "lb2pesq.h"
#include  "lb2vista.h"
#include  "lb2prot1.h"



#define   H_CFVERDO     "CF_VERDO"     /* qualquer tecla para ver documentos selecionados */
#define   H_PSQ1EDT     "E_PSQ_1EDT"
#define   H_PSQ2EDT     "E_PSQ_2EDT"
#define   H_PSQ3EDT     "E_PSQ_3EDT"
#define   H_CFPSBRI     "CF_PSBRI"


#define   SORT    'S'


SI     bq2_comp( );

COUNT br_warning;

F__GLB   COUNT visualiza(num_vetor, direto)
COUNT num_vetor;
COUNT direto;
{  COUNT ret, sai, sentido, last_sentido;
   COUNT it_ativo = -1;
   LONG  nreg, reg_ant, numero, posicao;
   SDINDEX *p_sdi;
   TEXT aux[70];
   WIN *win_disp;
   WIN *jm, *ji, *ji_e, *jf;


   ret = 0;
   sentido = last_sentido = FRENTE;
   lista_psq_glb = num_vetor;

   jm = ji = ji_e = jf = NULL;
   win_disp = NULL;


   push_brilha(SORT, str_nada);

   sprintf(aux, "%s [%ld]", M_PSQ_SEE, vet_lista[num_vetor].tot_doc);

   if ( ! direto && men_conf(aux, "*", H_CFVERDO, NULL) != 0 ) {
      ret = 1;
      goto fim;
   }

   if ( ( win_disp = abre_janela(J_REGDISP)) == NULL ) {
      ret = -1;
      goto fim;
   }

   sai = 0;
   posicao = 1;


   jm = abre_mensagem(M_PSQ_NAV);
   ji = abre_insercol(C_PSQ_ICOL);
   jf = abre_funcoes(str_nada);  /* reg_display quem vai usar */

   p_sdi = prim_sdi(num_vetor);

   reg_ant = nreg = p_sdi->registro;

   ap_vista.vis_it_ativo = -1;

   while ( !sai ) {

      /* para forcar reimprimir tela e seus campos */
      ap_vista.vis_old_ativo = -1;

      last_sentido = sentido;
      sentido = NADA;

      sprintf(aux, C_PSQ_NREG, nreg, posicao, vet_lista[num_vetor].tot_doc);
#define    MANTEM_TELA
#ifndef MANTEM_TELA
      /* Quando o cliente EMPAX der um OK na alteracao para manter tela */
      /* pode ser tirado esta atribuicao */
      it_ativo = -1;
#endif

      ret = reg_display(sele_pesq, nreg, win_disp,
                                   aux, pt_brilhar, 1, &numero, jm, jf, it_ativo);

      /***** Para manter tela/item ativo na navegacao */
      /** Ananias 16/11/93 a pedido do Jarbas para cliente ...... */
      it_ativo = ap_vista.vis_it_ativo;

      switch( ret ) {

         case -99    :  /* pula registro, pois nao leu este */
         		sentido = last_sentido;
                        break;
         case -1     :
         case -2     :  ret = -1;
                        goto fim;

         case 0      :  /* goto registro */
                        if ( numero <= 0 )
                           break;

                        if ( numero > vet_lista[num_vetor].tot_doc)
                           numero = vet_lista[num_vetor].tot_doc;

                        posicao = 1;

                        p_sdi = prim_sdi(num_vetor);

                        reg_ant = nreg = p_sdi->registro;

                        while ( posicao < numero ) {

                           p_sdi = prox_sdi(num_vetor);

                           nreg = p_sdi->registro;

                           if (nreg != reg_ant) {
                              posicao++;
                              reg_ant = nreg;
                           }
                        }

                        sentido = NADA;
                        break;

         case K_F2   :
         case K_F4   :
         case K_ESC  :  ret = 0;
                        sai = 1;
                        break;

#ifndef RUNTIME
         case K_F6   :  if ( ap_status == 'A' || ap_acesso_ativo.da_altera == 'X' ) {
                           if ( WCB->cb_lockatu == 0 ) { /* base estatica */
                              mens_erro(H_PSQ3EDT, E_PSQ_3EDT);
                              goto fim;
                           }
                           else {
                              ji_e = abre_insercol(C_PSQ_ALT);

                              ret = edit_record(sele_pesq, nreg, win_disp, it_ativo);

                              free_record(sele_pesq);

                              fecha_insercol(ji_e);

                              if ( ret == -99 ) {
                                 mens_erro(H_PSQ2EDT, E_PSQ_2EDT);
                                 ret = -1;
                                 goto fim;
                              }

                           }
                        }
                        else {
                           mens_erro(H_PSQ1EDT, E_PSQ_1EDT);
                        }
                        break;
#endif


         case K_F7   :  sentido = TRAZ;
                        break;

         case K_ENTER:  /* K_ENTER volta a primeira tela primeiro campo */
                        it_ativo = -1;
         case K_F8   :  sentido = FRENTE;
                        break;

         case K_F0  :   sentido = NADA;

                        ret = menu_pesq( );

                        /* so para refazer sdi */ 
                        {     SDINDEX *sdi_tmp;         
                              if ( prox_sdi(num_vetor) != NULL ) {
                                 sdi_tmp = back_sdi(num_vetor);
                                 if ( p_sdi != sdi_tmp ) {
                                    p_sdi = prim_sdi(num_vetor);
                                    posicao = 1;
                                 }
                              }
                              else {
                                 p_sdi = prim_sdi(num_vetor);
                                 posicao = 1;
                              }
      
                        }

                        if ( ret < 0 ) { /* finaliza navegacao nos registros */
                           ret = 0;
                           sai = 1;
                           break;
                        }

                        break;

         default     :  sentido = FRENTE;
                        break;

      }


      if ( vet_lista[num_vetor].tot_doc > 1 && (sentido != NADA || ret == -99)) {

         if ( sentido == FRENTE ) posicao++;
         else                     posicao--;

         while ( nreg == reg_ant ) {

            if ( sentido == FRENTE ) {
               if ( (p_sdi = prox_sdi(num_vetor)) == NULL ) {
                  posicao = 1;
                  p_sdi = prim_sdi(num_vetor);
                  win_bell(BEL_VALID);
               }
            }
            else {
               if ( (p_sdi = back_sdi(num_vetor)) == NULL ) {
                  posicao = vet_lista[num_vetor].tot_doc;
                  p_sdi   = last_sdi(num_vetor);
                  win_bell(BEL_VALID);
               }
            }

            nreg = p_sdi->registro;
         }
      }
      else {
         if ( sentido != NADA ) win_bell(BEL_VALID);
      }

      reg_ant = nreg;

   }



   free_record(sele_pesq);

fim :

   fecha_funcoes(jf);
   fecha_insercol(ji);
   fecha_mensagem(jm);
   if ( win_disp  != NULL ) fwkill( win_disp );

   dn_textos(sele_pesq, 0);

   return(ret);
}




F__GLB   COUNT aloc_brilha(operacao)
COUNT operacao;
{  COUNT ret = 0;

#ifdef   NAO_BRILHA
   static TEXT *falso = NULL;

   pt_brilhar  = &falso;
   str_brilhar = "";

   return ret;

#else

   if ( operacao == 'A' ) {

      pt_brilhar  = NULL;
      str_brilhar = NULL;

      if ( ( pt_brilhar = (TEXT **)calloc( VT_BRILHO + 10, sizeof(TEXT *))) == NULL ) {
         ret = -1;
         goto fim;
      }

      if ( ( str_brilhar = calloc( STR_BRILHO + 10, 1)) == NULL ) {
         ret = -1;
         goto fim;
      }
   }

   else if ( operacao == 'F' ) {
      if ( str_brilhar != NULL ) {
         free(str_brilhar);
         str_brilhar = NULL;
      }

      if ( pt_brilhar != NULL ) {
         free((TEXT *)pt_brilhar);
         pt_brilhar  = NULL;
      }
   }

   else numdebug(601);

fim :

   return(ret);
#endif
}


F__GLB   COUNT push_brilha(flag, argumento)
COUNT flag;
TEXT *argumento;
{  COUNT ret = 0, tam;
   static TEXT *pp;
   static COUNT str_free;

#ifdef   NAO_BRILHA

   return ret;

#else

   /* ANANIAS @@@ mascarar fim de string qdo retirar NULL da final da chave */

   switch ( flag ) {

      case INICIALIZA :

               pp = str_brilhar;
               pt_brilhar[0] = NULL;
               str_free = STR_BRILHO;
               psq_qtdfonetica = 0;
               psq_brqtd = 0;
               br_warning  = 0;

               break;

      case PUSH :

               if ( psq_brqtd < VT_BRILHO ) {

                  tam = strlen(argumento) + 1;

                  if ( str_free >= tam ) {

                     if ( argumento[0] == 2 && argumento[1] == 2 )
                        psq_qtdfonetica++;

                     pt_brilhar[psq_brqtd] = pp;
                     strcpy(pp, argumento);

                     str_free -= tam;
                     pp       += tam;

                     psq_brqtd++;
                     pt_brilhar[psq_brqtd] = NULL;

                  }
                  else {
                     br_warning = (br_warning == 0 ) ? 1 : 2;
                  }
               }
               else {
                  br_warning = (br_warning == 0 ) ? 1 : 2;
               }

               if ( br_warning == 1 ) {
#ifdef NAO_ENTRA
                  if ( decida(TW_PQ_T00, TW_PQ_T99, H_CFPSBRI,
                              4, 46, "%s\n%s\n%s\n%s", TW_PQ_T01, TW_PQ_T02,
                              TW_PQ_T03, TW_PQ_T04 ) != 1 ) {

                     ret = -1;
                     goto fim;
                  }
#endif
               }

               break;


      case SORT :

               if ( psq_brqtd > 0 )
                  qsort( (SC *) pt_brilhar, psq_brqtd, sizeof( SC * ), bq2_comp );

               break;
   }

fim :

   return(ret);
#endif
}


SI  bq2_comp( S1,  S2 )
SC    **S1, **S2;
{
   return( strcmp( *S1, *S2 ) );
}

