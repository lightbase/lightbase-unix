#include  "lb2all.h"

#include  <string.h>
#include  HD_IO

#include  "lb2stru.h"
#include  "lb2vars.h"
#include  "lb2text.h"  /* strings comus a todos os modulos */


#include  "lb2bases.h"
#include  "lb2files.h"
#include  "lb2maces.h"
#include  "lb2vet.h"
#include  "lb2vista.h"
#include  "lb2prot1.h"


#include  "lb2fer.h"


#define   BS  'B'
#define   CP  'C'
#define   VS  'V'
#define   IV  'I'
#define   RE  'R'
#define   IR  'L'
#define   SW  'S'

#define   MAX_ITBS    70

struct itens_lb4 {
   TEXT     tipo;   /* tipo de itens localizado */
   COUNT    ordem;  /* numero de ordem */
   TEXT    *memo;   /* posicao alocada */
   POINTER *pos;    /* posicao em disco */
};

struct hdtxt {
   UTEXT txt_marca;  /* MARCA_TEXTO */
   LONG  txt_record;
   UTEXT txt_campo;
   UTEXT txt_parte;
   POINTER txt_proximo;
};

#ifdef MSDOS
   VOID refaz_mascara(Campo *);
   COUNT consiste_campos(TEXT *);
   COUNT consiste_lb4(TEXT *);
   COUNT consiste_vistas(VOID);
   COUNT consiste_itvistas(Vista *);
   COUNT consiste_relats(VOID);
   COUNT consiste_itrelats(Rel *);
   COUNT refaz_lb4( WIN *);
   COUNT ident_reg(TEXT *, UCOUNT, COUNT *);
#else
   VOID refaz_mascara( );
   COUNT consiste_campos( );
   COUNT consiste_lb4( );
   COUNT consiste_vistas( );
   COUNT consiste_itvistas( );
   COUNT consiste_relats( );
   COUNT consiste_itrelats( );
   COUNT refaz_lb4( );
   COUNT ident_reg( );
#endif



F__GLB   COUNT tbd_manut( vet_zap)
TEXT *vet_zap;
{  COUNT ret;
   LONG num_serie;
   UCOUNT id_arq;
   TEXT aux[PATH_MAX+10];
   TEXT mens_log[100];
   WIN *jm;


   id_arq    = rec_base->bs_idbase;
   num_serie = rec_base->bs_nserie;

   jm = NULL;

   jm = abre_mensagem(M_AGUARDE);


   if ( (ret = tbd_status(lb4_dnum, rec_base, &num_serie, &id_arq)) != 0 ) {
      goto fim;
   }

   if ( rec_base->bs_versao  < 500 ) {
	   CLSFIL(lb4_dnum, COMPLETE );
      sprintf(mens_log, E_USECONV);
      wr_log(1, mens_log);
      ret = -1;
      goto fim;
   }

   if ( uerr_cod != NO_ERROR ||
        sep_dec != rec_base->bs_sepdecimal ||
        rec_base->bs_versao  != VERSAO_CORRENTE ) {
      if ( uerr_cod ) {
         sprintf(mens_log, E_RDOPN, uerr_cod);
         wr_log(1, mens_log);
      }
      wr_log(1, ML_COR4);
      if ( refaz_lb4(jm) != 0 ) {
         wr_log(1, ML_NOREB);
         goto fim;
      }

      wr_log(0, ML_RDFIN);
   }

   CLSFIL(lb4_dnum, COMPLETE );

   WBS = rec_base;
   WBS->bs_idbase = id_arq;

   info_base(WBS->bs_nome, WBS->bs_path, LB_4_EXT, aux);

   wr_log(0, ML_BEGCONS);

   if ( open_lb4(WBS, aux, 'D', num_serie, WBS->bs_idbase ) != 0 ) {
      wr_log(1, ML_EROP4);
      ret = -1;
      goto fim;
   }

   if ( consiste_lb4(vet_zap) != 0 ) {
      wr_log(1, ML_ERCONS);
      ret = -1;
      goto fim;
   }

   ret = 0;

fim :

   close_lb4();

	fecha_mensagem(jm);

   WBS = NULL;

   return(ret);
}





F__GLB   COUNT tbd_status(num_file, base, num_serie, id_arq)
COUNT num_file;
Base *base;
LONG *num_serie;
UCOUNT *id_arq;
{  COUNT ret, res ;
	TEXT nome[PATH_MAX]; /* Arquivo de descricao */

   ret = -1;

   /*
   **   Inicialmente verifica se arquivo descritores da base estao ok
   */
   res = info_base(base->bs_nome, base->bs_path, LB_4_EXT, nome);

   if ( res < 0 ) {
      mens_erro(H_NOREG, E_NOREG, nome);
      goto fim;
   }

   if ( res == 0  ) {
      mens_erro(H_NOFILE, E_NOFILE, nome);
      goto fim;
   }
   if ( (res & PODE_LER) == 0 ){
      mens_erro(H_PERMAC, E_PERMAC, nome);
      goto fim;
   }

   ret = MANUT_OPEN( num_file, nome, 'V', num_serie, id_arq );

   if ( base->bs_finger != 0 ) {
      if ( ct_finger != base->bs_finger || ct_release != base->bs_release){
         CLSFIL(num_file, COMPLETE );
         mens_erro(H_CFFINGER, E_CFFINGER);
         ret = -1;
         goto fim;
      }
   }

   switch ( ret ) {
      case FUSE_ERR  :  /*   */
                        numdebug(701);
                        break;

      case FNOP_ERR  :  /*   */
      case READ_ERR  :  /*   */
      case FUNK_ERR  :  /*   */
      case FVER_ERR  :  /*   */
      case IDBASE_ERR:  /*   */
                        break;

      case FCRP_ERR  :  /*   */
      case NO_ERROR  :  /*   */
                        {  Base *area;

                           /* atualiza versao e sep_dec */
                           if ( ( area = (Base *) calloc(1, sizeof(Base))) == NULL ) {
                              mens_erro(H_NOMEMO, SEM_MEMO);
                           }
                           else {
	                           if ( RDVREC(num_file, 262, (TEXT *)area, sizeof(Base)) == NO_ERROR ) {

                                 switch(area->bs_status) {
                                    case '4' : versao_velha = 401;  break;
                                    case '5' : versao_velha = 501;  break;
                                    case '6' : versao_velha = 601;  break;
                                    default  : versao_velha = area->bs_versao; break;
                                 }


                                 base->bs_sepdecimal = area->bs_sepdecimal;
                                 base->bs_versao = area->bs_versao;
                                 base->bs_status = 0;
                              }
                              free(area);
                           }
                        }

                        ret = 0;
                        break;

      default        :  /*   */
                        numdebug(708);
                        break;
   }

   if ( ret != 0  )
      CLSFIL(num_file, COMPLETE );

fim :

   return(ret);
}



F__LOC   COUNT consiste_lb4(vet_zap)
TEXT *vet_zap;
{  COUNT ret = -1;

   if ( consiste_campos(vet_zap) != 0 )
      goto fim;


   /* carrega campos em modo estatico */
   if ( (v_campos[BASE_EDIT] = load_campos(BASE_EDIT, 'F')) == NULL ) {
      wr_log(1, ML_LDCPS);
      ret = -1;
      goto fim;
   }

   if ( consiste_vistas() != 0 )
      goto fim;


   if ( consiste_relats() != 0 )
      goto fim;


   ret = 0;

fim :

   if ( v_campos[BASE_EDIT] != NULL) {
      rlse_campos(BASE_EDIT);
      v_campos[BASE_EDIT] = NULL;
   }

   return(ret);
}



F__LOC   COUNT consiste_campos(vet_zap)
TEXT *vet_zap;
{  COUNT ret = -1, maior, i;
   UCOUNT tam;
   POINTER 	pos, pos_ant;
   Campo *pc;


   /* load em modo edicao para permitir operacoes */
   v_campos[BASE_EDIT] = NULL;

   if ( (v_campos[BASE_EDIT] = load_campos(BASE_EDIT, 'E')) == NULL ) {
      wr_log(1, ML_LDCPS);
      goto fim;
   }


   if ( qtd_vetcmp(BASE_EDIT) == 0 ) {
      wr_log(1, ML_LDCPS);
      goto fim;
   }


   for ( maior = i = 0 ; i < qtd_vetcmp(BASE_EDIT) ; i++ ) {

      pc = pos_vetcmp(BASE_EDIT)[i];

      if ( pc->cp_tipo == 'X' )     pc->cp_tipo = 'x';
      if ( pc->cp_a_tipo == 'X' )   pc->cp_a_tipo = 'x';

      if ( strlen(pc->cp_mascara) > pc->cp_mhor && pc->cp_tipo != 'F') {
         pc->cp_mascara[pc->cp_mhor] = '\0';
      }

      refaz_mascara(pc);

      if ( pc->cp_a_tipo != pc->cp_tipo ) {
         if ( pc->cp_a_tipo == CP_TEXTO || pc->cp_tipo == CP_TEXTO )
            vet_zap[pc->cp_num] = 2;
         else
            vet_zap[pc->cp_num] = 1;
      }

      pc->cp_a_tipo = pc->cp_tipo;

      if ( pc->cp_tipo == 'N' || pc->cp_tipo == 'F')
         pc->cp_a_tam  = pc->cp_mhor + (pc->cp_decimais << 5 ); /* cria anterior */
      else
         pc->cp_a_tam  = pc->cp_mhor; /* cria anterior */


      pc->cp_reb_flag = NAO_MODIFICADO;

      maior = MAX(maior, pc->cp_num);
   }

   WBS->bs_u_seq = maior;
   WBS->bs_updated = 1;


   /* marca alteracao no vetor para salvar */
   v_campos[BASE_EDIT]->vi_status  |= VET_ALT;

   /* salva vetor de campos */

   ret = -1;

   tam = v_campos[BASE_EDIT]->vi_mtam + TAMANHO_HDR;
   pos_ant = WBS->bs_cppos;
   pos = lb4_space(pos_ant, tam);
   if ( pos != (POINTER) 0 ) {
      if ( save_vet(v_campos[BASE_EDIT], pos) != (POINTER) NULL ) {
         WBS->bs_cppos = pos;
         WBS->bs_status = 0;
         WBS->bs_updated = 1;
         if ( lb4_upd( 1 ) == 0 )
            ret = 0;
      }
   }


fim :

   if ( ret != 0 ) {
      wr_log(1, ML_RCCPS);
      ret = -1;
   }

   if ( v_campos[BASE_EDIT] != NULL) {
      rlse_campos(BASE_EDIT);
      v_campos[BASE_EDIT] = NULL;
   }

   return(ret);
}




F__LOC   COUNT consiste_vistas()
{  COUNT ret = -1, maior, i, num_vistas, conta_vis;
   UCOUNT tam;
   POINTER 	pos, pos_ant;
   TEXT mens_log[100];
   Vista    **vet_vis, *pv;
   Vet_itens *vv;


   /* load em modo edicao para permitir operacoes */
   vv = NULL;

   if ( (vv = load_vistas(BASE_EDIT, 'E')) == NULL ) {
      wr_log(1, ML_LDVIS);
      goto fim;
   }

   vet_vis = (Vista **) vv->vi_vtit;  /* aponta para vetor de vistas */

   num_vistas = vv->vi_nelem;

   if ( num_vistas == 0 ) {
      wr_log(1, ML_NOVIS);
      ret = 0;
      goto fim;
   }


   maior = conta_vis = 0;

   for ( i = 0 ; i < num_vistas ; i++ ) {
      pv = vet_vis[i];

      sprintf(mens_log, ML_AVVIS, pv->vis_ident);
      wr_log(0, mens_log);

      pos = pv->vis_itpos;

      if ( consiste_itvistas(pv) != 0 ) {
         wr_log(1, ML_DSVIS);
         pos_item( vv, i);
         del_item( vv, 1);
         i -= 1;
         num_vistas--;
         continue;
      }

      if ( pos != pv->vis_itpos ) /* alterou algo na vista */
         vv->vi_status |= VET_ALT;

      conta_vis++;
      maior = MAX(maior, pv->vis_ordem);
   }

   if ( (vv->vi_status & (~VET_EDIT)) != 0 ) {

      vv->vi_ident = maior;

      tam = vv->vi_mtam + TAMANHO_HDR;
      pos_ant = vv->vi_pos;
      pos = lb4_space(pos_ant, tam);
      if ( pos != (POINTER) 0 ) {
         if ( save_vet(vv, pos) != (POINTER) NULL ) {
            WBS->bs_n_vis = vv->vi_nelem;
            WBS->bs_vspos = pos;
            WBS->bs_updated = 1;
            if ( lb4_upd( 1 ) == 0 )
               ret = 0;
         }
      }
   }
   else
      ret = 0;


fim :

   if ( ret != 0 ) {
      wr_log(1, ML_RCVIS);
      WBS->bs_n_vis = 0;
      WBS->bs_vspos = 0L;
      WBS->bs_updated = 1;
      if ( lb4_upd( 1 ) == 0 )
         ret = 0;
   }

   if ( vv != NULL ) {
      rlse_vet(vv);
   }

   return(ret);
}


F__LOC   COUNT consiste_itvistas(pv)
Vista *pv;
{  COUNT ret = -1, i, ind;
   UCOUNT tam;
   POINTER 	pos, pos_ant;
   Campo **vet_cmp;
   Vet_itens *vi;
   TTela *pe;


   vet_cmp = pos_vetcmp(BASE_EDIT);

   /* load em modo edicao para permitir operacoes */
   vi = NULL;

   if ( (vi = load_itvis(pv, 'E')) == NULL ) {
      wr_log(1, ML_LDEVIS);
      goto fim;
   }


   for ( i = 0 ; i < vi->vi_nelem ; i++ ) {

      pe = (TTela *)vi->vi_vtit[i];

      if ( pe->te_tipo == CAMPO_G || pe->te_tipo == CAMPO_E || pe->te_tipo == CAMPO_S ) {

         if ( pe->te_icb != BASE_EDIT )  /* ANANIAS nao esta tyratando base relacionada */
            continue;

         ind = acha_campo( pe->te_icb, pe->te_num);

         if ( ind < 0 ) { /* campo nao localizado, elimina item da vista */
            wr_log(1, ML_DSITV);
            pos_item( vi, i);
            del_item( vi, 1);
            i -= 1;    /* ANANIAS era -2 */
            continue;
         }

         /* ajusta tamanho do item da vista ao tamanho maximo valido */

         if ( pe->te_vhor > vet_cmp[ind]->cp_mhor ) {
            vi->vi_status |= VET_ALT;
            pe->te_vhor = vet_cmp[ind]->cp_mhor;
            if ( vet_cmp[ind]->cp_tipo == CP_TEXTO)
               pe->te_vhor += 1;
         }
      }
   }

   if ( vi->vi_nelem == 0 ) {
      wr_log(1, ML_NOEVIS);
      ret = -1;
      goto fim;
   }



   /* salva itens da vista */
   if ( (vi->vi_status & (~VET_EDIT)) != 0 ) {
      tam = vi->vi_mtam + TAMANHO_HDR;
      pos_ant = pv->vis_itpos;
      pos = lb4_space(pos_ant, tam);
      if ( pos != (POINTER) 0 ) {
         if ( save_vet(vi, pos) == (POINTER) NULL ) {
            pos = 0L;
         }
      }
      if ( pos != 0L ) {
         pv->vis_itpos = pos;
         ret = 0;
      }
   }
   else
      ret = 0;

fim :

   if ( vi != NULL ) {
      rlse_vet(vi);
      if ( ap_visdoc !=NULL ) {
         free_buffer( ap_visdoc );
         ap_visdoc = NULL;
      }
   }

   return(ret);
}




F__LOC   COUNT consiste_relats()
{  COUNT ret = -1, maior, i, num_relats, conta_rel;
   UCOUNT tam;
   POINTER 	pos, pos_ant;
   TEXT mens_log[100];
   Rel   **vet_rel, *pr;
   Vet_itens *vr;


   /* load em modo edicao para permitir operacoes */
   vr = NULL;

   if ( (vr = load_rels(BASE_EDIT, 'E')) == NULL ) {
      wr_log(1, ML_LDREL);
      goto fim;
   }

   vet_rel = (Rel **) vr->vi_vtit;  /* aponta para vetor de relats */

   num_relats = vr->vi_nelem;

   if ( num_relats == 0 ) {
      wr_log(0, ML_NOREL);
      ret = 0;
      goto fim;
   }


   maior = conta_rel = 0;

   for ( i = 0 ; i < num_relats ; i++ ) {
      pr = vet_rel[i];

      sprintf(mens_log, ML_AVREL, pr->rel_ident);
      wr_log(0, mens_log);

      pos = pr->rel_itpos;

      if ( consiste_itrelats(pr) != 0 ) {
         wr_log(1, ML_DSREL);
         pos_item( vr, i);
         del_item( vr, 1);
         i -= 1;    /* ANANIAS era -2 */
         num_relats--;
         continue;
      }

      if ( pos != pr->rel_itpos ) /* alterou algo no relatorio */
         vr->vi_status |= VET_ALT;

      conta_rel++;
      maior = MAX(maior, pr->rel_ordem);
   }

   if ( (vr->vi_status & (~VET_EDIT)) != 0 ) {

      vr->vi_ident = maior;

      tam = vr->vi_mtam + TAMANHO_HDR;
      pos_ant = vr->vi_pos;
      pos = lb4_space(pos_ant, tam);
      if ( pos != (POINTER) 0 ) {
         if ( save_vet(vr, pos) != (POINTER) NULL ) {
            WBS->bs_n_rel = vr->vi_nelem;
            WBS->bs_rlpos = pos;
            WBS->bs_updated = 1;
            if ( lb4_upd( 1 ) == 0 )
               ret = 0;
         }
      }
   }
   else
      ret = 0;


fim :

   if ( ret != 0 ) {
      wr_log(1, ML_RCREL);
      WBS->bs_n_rel = 0;
      WBS->bs_rlpos = 0L;
      WBS->bs_updated = 1;
      if ( lb4_upd( 1 ) == 0 )
         ret = 0;
   }

   if ( vr != NULL ) {
      rlse_vet(vr);
   }

   return(ret);
}


F__LOC   COUNT consiste_itrelats(prel)
Rel *prel;
{  COUNT ret = -1, i, ind;
   UCOUNT tam;
   POINTER 	pos, pos_ant;
   Campo **vet_cmp;
   Vet_itens *vi;
   RTela *pr;


   vet_cmp = pos_vetcmp(BASE_EDIT);

   /* load em modo edicao para permitir operacoes */
   vi = NULL;

   if ( (vi = load_itrel(prel, 'E')) == NULL ) {
      wr_log(1, ML_LDEREL);
      goto fim;
   }


   for ( i = 0 ; i < vi->vi_nelem ; i++ ) {

      pr = (RTela *)vi->vi_vtit[i];

      if ( pr->re_tipo == CAMPO_G || pr->re_tipo == CAMPO_S ) {

         if ( pr->re_icb != BASE_EDIT )  /* ANANIAS nao esta tratando base relacionada */
            continue;

         ind = acha_campo( pr->re_icb, pr->re_num);

         if ( ind < 0 ) { /* campo nao localizado, elimina item da vista */
            wr_log(1, ML_DSITR);
            pos_item( vi, i);
            del_item( vi, 1);
            i -= 1;    /* ANANIAS era -2 */
            continue;
         }

         /* ajusta tamanho do item do relatorio ao tamanho maximo valido */

         if ( pr->re_vhor > vet_cmp[ind]->cp_mhor ) {
            vi->vi_status |= VET_ALT;
            pr->re_vhor = vet_cmp[ind]->cp_mhor;
            if ( vet_cmp[ind]->cp_tipo == CP_TEXTO)
               pr->re_vhor += 1;
         }
      }
   }

   if ( vi->vi_nelem == 0 ) {
      wr_log(1, ML_NOEREL);
      ret = -1;
      goto fim;
   }



   /* salva itens do relatorio */
   if ( (vi->vi_status & (~VET_EDIT)) != 0 ) {
      tam = vi->vi_mtam + TAMANHO_HDR;
      pos_ant = prel->rel_itpos;
      pos = lb4_space(pos_ant, tam);
      if ( pos != (POINTER) 0 ) {
         if ( save_vet(vi, pos) == (POINTER) NULL ) {
            pos = 0L;
         }
      }
      if ( pos != 0 ) {
         prel->rel_itpos = pos;
         ret = 0;
      }
   }
   else
      ret = 0;

fim :

   if ( vi != NULL ) {
      rlse_vet(vi);
      if ( ap_reldoc !=NULL ) {
         free_buffer( ap_reldoc );
         ap_reldoc = NULL;
      }
   }

   return(ret);
}





F__LOC   VOID refaz_mascara(pc)
Campo *pc;
{  COUNT tam, dec;
	TEXT *p;

   if ( pc->cp_tipo == 'N' || pc->cp_tipo == 'F' ) {

      if (pc->cp_decimais > 0 )
         tam = pc->cp_mhor - pc->cp_decimais - 1;
      else
         tam = pc->cp_mhor;

      tam  = tam - (tam / 4);
   	dec  = pc->cp_decimais;

      if ( pc->cp_tipo == 'N' )
         p = pc->cp_mascara;
      else
         p = pc->cp_param.cp_msc_form;

      while ( tam ) {
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
   }
}


F__LOC   COUNT refaz_lb4(jm)
WIN *jm;
{  COUNT  ret, status;
   UCOUNT tamanho, livre;
   POINTER proximo;
   Base area;

   ret = -1;

   mens_1(jm, ML_AVLB4);

   REG_INFO(lb4_dnum, (POINTER) 0L, &tamanho, &livre, &proximo, &status);

	if ( RDVREC(lb4_dnum, proximo, (TEXT *)&area, sizeof(Base)) == NO_ERROR ) {
      if ( area.bs_marca[0] == 'B' && area.bs_marca[1] == 'S' ) {
         area.bs_versao  = VERSAO_CORRENTE;
         area.bs_sepdecimal = sep_dec;
         area.bs_status  = 0;

   	   ret = WRTVREC(lb4_dnum, proximo, (TEXT *)&area, sizeof(Base));
      }
   }

   return(ret);
}

#ifdef VAI_DEMORAR
F__LOC   COUNT refaz_lb4(jm)
WIN *jm;
{  COUNT ret, ordem, status, tpreg, sai, ind;
   UCOUNT tamanho, livre;
   POINTER pos, proximo, lost;
   TEXT *buf;
   struct itens_lb4 *achados;


   ind   = 0;
   sai   = 0;
   buf   = NULL;
   pos   = (POINTER)0;
   lost  = (POINTER)0;

   achados = calloc(MAX_ITBS, sizeof(struct itens_lb4));

   if (achados == NULL ) {
      mens_erro(H_NOMEMO, SEM_MEMO);
      ret = -1;
      goto fim;
   }

   mens_1(jm, "Processando registros da base. Aguarde...");

   while ( ! sai ) {

      ret = REG_INFO(lb4_dnum, pos, &tamanho, &livre, &proximo, &status);

      if ( pos == (POINTER) 0 && proximo != (POINTER)0 ) {
         pos = proximo;
         continue;
      }

      switch ( ret ) {
         case  READ_ERR :  /* Read erro */
                           wr_log(1, "Erro de leitura na descricao da base");
                           sai = 1;
                           break;

         case  LEOF_ERR :  /* fim de arquivo */
                           wr_log(1, "Fim do arquivo de descricao da base");
                           sai = 1;
                           break;

         case  RVHD_ERR :  /* marca de registro variavel invalido */
                           wr_log(1, "Possivel perda de informacoes");
                           lost += tamanho;
                           break;

         case  NO_ERROR :  /* registro OK */

                           if ( tamanho == 0 ) {
                              break;
                           }

                           if ( (buf = malloc(tamanho)) == NULL ) {
                              mens_erro(H_NOMEMO, SEM_MEMO);
                              ret = -1;
                              goto fim;
                           }

                           ret = REG_READ(lb4_dnum, pos, tamanho, buf);

                           if ( ret != NO_ERROR ) {
                              sai = 1;
                              break;
                           }

                           tpreg = ident_reg( buf, tamanho, &ordem);

                           if ( tpreg != 0 ) {
                              achados[ind].tipo = tpreg;
                              achados[ind].ordem = ordem;
                              achados[ind].memo = buf;
                              achados[ind].pos  = (POINTER) pos;
                              if ( ++ind >= MAX_ITBS) {
                                 numdebug(860);
                                 goto fim;
                              }
                           }
                           else {
                              free(buf);
                           }

                           break;

         default        : wr_log(1, "Informacao nao reconhecida na descricao da base");

      }

      if ( proximo == (POINTER) 0 ) sai = 1;
      else                pos = proximo;

   }

   ret = 0;

fim :

   if ( achados != NULL ) {
      for(ind = 0; ind < MAX_ITBS; ind++)
         if (achados[ind].memo != NULL )
            free(achados[ind].memo);
      free(achados);
   }

   return(ret);
}


F__LOC   COUNT ident_reg( buf, tam, ordem)
TEXT *buf;
UCOUNT tam;
COUNT *ordem;
{  COUNT ret;
   Hdr_bloco *hdr;
   struct hdtxt hd_txt;

   *ordem = 0;
   ret = 0;

   if ( (UTEXT)buf[0] == MARCA_TEXTO && tam >= sizeof(hd_txt)) {
      memcpy(&hd_txt, buf, sizeof(hd_txt));
      if ( hd_txt.txt_record == 0L ) {
         ret = SW;
         goto fim;
      }
   }

   /* Stop Words tem numero de registro 0000 */
   if ( ((UTEXT)buf[0] == MARCA_TEXTO) && ( buf[1] + buf[2] + buf[3] + buf[4] + buf[5] == 0) ) { /* stop words */
      ret = SW;
      goto fim;
   }


   if ( tam == sizeof(Base)) {
      Base *pb;
      pb = (Base *)buf;
      if ( pb->bs_marca[0] == 'B' && pb->bs_marca[0] == 'S') {
         ret = 'B';
         goto fim;
      }
   }

   hdr = (Hdr_bloco *)buf;

   if ( hdr->marca_fim[0] != 0x6 || hdr->marca_fim[1] != 0x4 ) {
      switch(hdr->tipo) {
         case 'C' : *ordem = hdr->ident;   ret = CP ;   goto fim;
         case 'V' : *ordem = hdr->ident;   ret = VS ;   goto fim;
         case 'I' : *ordem = hdr->ident;   ret = IV ;   goto fim;
         case 'R' : *ordem = hdr->ident;   ret = RE ;   goto fim;
         case 'L' : *ordem = hdr->ident;   ret = IR ;   goto fim;
      }
   }

   wr_log(1, "Item de descricao de base desconhecido");

fim :

   return ( ret );
}



F__LOC   COUNT new_lb4()
{  COUNT ret, narq;
   UCOUNT id;
   POINTER pos_hd;
   LONG ns;
	TEXT bak[PATH_MAX];   /* nome de arquivo backup */
   TEXT *p, nx[PATH_MAX];
   COUNT varl = VIRTUAL | EXCLUSIVE | VLENGTH;

   info_base(rec_base->bs_nome, rec_base->bs_path, LB_4_EXT, nx);

   strcpy(bak, nx);
   p = &bak[strlen(bak) -1];
   while ( *p != '.' )
      p--;
   p++;
   *p = '@';

   if ( access(bak, 0) == 0 )
      DELFILE(bak);


   narq = lb4_dnum;
   ns   = ap_nserie;
   id   = rec_base->bs_idbase;

   ct_sernum   = 0L;


   CREDAT(lb4_dnum, bak, 0, SLOT, varl, ns, id );

   CLSFIL(lb4_dnum, COMPLETE);

   if ( OPNFIL(lb4_dnum, bak, VIRTUAL | EXCLUSIVE, ns, id) != 0 ) {
      numdebug(861);
      ret = -1;
   }


   /* somente aloca espaco no arquivo. so grava no final */
   if ((pos_hd = NEWVREC(narq, sizeof(Base))) == (POINTER)0 ) {
      numdebug(862);
      ret = -1;
      goto fim;
   }
   lb4_pos = pos_hd;





	if ( WRTVREC(narq, pos_hd, (TEXT *)rec_base, sizeof(Base)) != 0 ) {
      numdebug(863);
      ret = -1;
      goto fim;
   }

   LOKREC(narq, FREE, pos_hd);

   CLSFIL(lb4_dnum, COMPLETE);



   if ( access(nx, 0) == 0 )
      DELFILE(nx);

   if ( RENFILE(nx, bak) != NO_ERROR ) {
      mens_erro("E_RBGV4", E_RBGV4, nx, bak);
      wr_log(1, E_RBREN);
      ret = -1;
      goto fim;
   }


fim :

   return(ret);
}
#endif
