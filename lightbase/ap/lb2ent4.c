#include  "lb2all.h"

#include  <string.h>
#include  <time.h>
#include  HD_IO

#include  "lb2stru.h"
#include  "lb2vars.h"
#include  "lb2text.h"  /* strings comus a todos os modulos */

#include  "lb2bases.h"
#include  "lb2entra.h"
#include  "lb2files.h"
#include  "lb2locs.h"
#include  "lb2maces.h"
#include  "lb2prot1.h"
#include  "lb2trans.h"

#include  "lb2vista.h"
#include  "lb2entd.h"

#ifdef    VAXVMS
#include  "menu.h"
#else
#include  "../mt/menu.h"
#endif



#define   E_NOITEMTAB     loc_str( STR_GLB, 579)

#define   E_NOVAZIO       loc_str( STR_GLB, 599)


#define   OFF_AC (((((POINTER)128 + (POINTER)(sizeof(Reg_controle))) - (POINTER)1) \
             / (POINTER)sizeof(Reg_controle) ) * (POINTER)sizeof(Reg_controle))

#define   NUM_TO_POS(num)  ( (POINTER)OFF_AC + \
          ((POINTER) sizeof(Reg_controle) * (POINTER)((LONG)num - (LONG)1)))

#define   POS_TO_NUM(pos)  (((pos - (POINTER)OFF_AC) / (POINTER) sizeof(Reg_controle)) + (POINTER) 1)


#ifdef MSDOS
   COUNT disp_campos(COUNT, WIN *);
   COUNT expa_registro(COUNT);  /* em lb2ent3.c */

   COUNT pack_registro(COUNT);
   VOID  dup_reg(COUNT);
   COUNT reg_updated(COUNT, COUNT);
   COUNT fx_wrtexto( RNDFILE, VRLEN);
   VOID  help_campo(Campo *, TEXT *);
#else
   COUNT disp_campos( );
   COUNT expa_registro( );

   COUNT pack_registro( );
   VOID  dup_reg( );
   COUNT reg_updated( );
   COUNT fx_wrtexto( );
   VOID  help_campo( );
#endif



#ifdef MSDOS
   VOID  xuxa_campo( Campo *, TTela *, WIN *);
   VOID  disp_formulas(COUNT, WIN *, COUNT);
   COUNT pode_editar(TEXT *, TEXT *);
   COUNT res_usu(TEXT *, TEXT *, COUNT);
   COUNT res_grp(TEXT *, TEXT *, COUNT);
   VOID  trata_historico( VOID );
   COUNT mudou_campos( TEXT * );
   COUNT atualiza_hist( TEXT * );
   COUNT inc_lintxt(TEXT *, Campo *);
   COUNT inc_linalfa(TEXT *, Campo *);
   VOID inc_camp_camp(Campo *, Campo *, COUNT);
   VOID limpa_campos(TEXT *);
#else
   VOID  xuxa_campo( );
   VOID  disp_formulas( );
   COUNT pode_editar( );
   COUNT res_usu( );
   COUNT res_grp( );
   VOID  trata_historico( );
   COUNT mudou_campos( );
   COUNT atualiza_hist( );
   COUNT inc_lintxt( );
   COUNT inc_linalfa( );
   VOID inc_camp_camp( );
   VOID limpa_campos( );
#endif



#ifdef AUTO_INCREMENTO
#ifdef MSDOS
   LONG auto_incre( TEXT *, COUNT );
   COUNT auto_getfield( VOID );
#else
   LONG auto_incre( );
   COUNT auto_getfield( );
#endif
extern COUNT ap_flag_auto;
extern COUNT ap_autonum;
#endif



extern TTela *pe_global;
extern COUNT last_rdcampo;
COUNT vv_cmpvazio; /* indica indice de campo obrigatorio vazio */

/*----------------------------------------------------------------------*/
/* WRITE_RECORD                                                         */
/*----------------------------------------------------------------------*/
F__GLB   COUNT write_record(sele, acumula_idx)
COUNT sele, acumula_idx;
{  COUNT ret, tam, i, num_cps, alteracao;
   COUNT ret_trans = 0;
   LONG num_rec;
   TEXT *ioarea, chave[20];
   POINTER ad_ant, novo_ad;
   Campo **vet_cmp, *pc;
   Reg_controle *rc;



   novo_ad = (POINTER) 0;
   ioarea  = c_bases[sele]->cb_ioarea;
   num_rec = c_bases[sele]->cb_numrec;
   rc      = &c_bases[sele]->cb_controle;
   ad_ant  = c_bases[sele]->cb_controle.rc_pos;


   if ( (ret_trans = BEG_TRANS(TR_WRRECORD, num_rec, 0)) < 0 ) {
      ret = -1;
      goto fim;
   }


   if ( ad_ant == 0 || ap_operacao == REINDEX || ap_operacao == REPROC )
      alteracao = 0;
   else
      alteracao = 1;

   vet_cmp  = pos_vetcmp(sele);
   num_cps  = qtd_vetcmp(sele);

   i = alteracao ? ALTERACAO : INCLUSAO;
   if ( salva_unica(sele, i, '+') != NULL ) {
      ret = -2;
      goto fim;
   }


   /*
   **    DESINDEXA E ELIMINA REGISTRO ANTIGO
   */

   if ( alteracao ) {

      if ( acumula_idx ) numdebug(505);

      if ( rc->rc_status == NO_INDEX )  numdebug(506);

      /* tratamento do registro antigo */

      if ( salva_unica(sele, ALTERACAO, '-') != NULL ) {
         ret = -3;
         goto fim;
      }

      if ( rc->rc_status == ATIVO_INDEX ) {
         set_erro(0); /* desliga mens_erro */
         ret = reg_index(sele, ALTERACAO, '-', num_rec, FLUSH_IDX);
         set_erro(1); /* liga mens_erro */
         if (  ret != 0 ) {
            ret = -4;
            goto fim;
         }
      }

      ret = RETVREC(AD_NUM(sele), ad_ant); /* libera anterior */

      if ( ret != NO_ERROR ) {
         ret = -3;
         goto fim;
      }

      /* varre vetor de campos procurando campos textos e liberando
      ** espacos ocupados em disco
      */

      for ( i = 0; i < num_cps; i++ ) {
         pc = vet_cmp[i];
         if ( pc->cp_mod_flag == 0 ) continue;
         if ( pc->cp_tipo == 'T' ) {
            if ( pc->cp_olddata != NULL ) {
               ret = txt_free( AD_NUM(sele), get_postexto('O', (UTEXT *)pc->cp_olddata));
               if ( ret < 0 ) {
                  /* WWW erro mas eu vou ignorar pois nao tem nada o que fazer */
                  ret = 0;
               }
            }
            else
               ret = 0;
         }
      }

      c_bases[sele]->cb_controle.rc_pos = (POINTER) 0;
   }



   /*
   **    GRAVA NOVO REGISTRO
   */
   set_erro(0);
   ret = salva_textos(sele, num_rec, -1); /* -1 indica todos os campos */
   set_erro(1);

   if ( ret != 0 ) {
      ret = -5;
      goto fim;
   }

   tam = pack_registro(sele);

/*** WWWW deve criptografar tam bytes de cb_ioarea */
/* depois de gravar deve descriptografar para poder desindexar */
   en_cripta((UTEXT *)WCB->cb_ioarea, tam);


   novo_ad = NEWVREC(AD_NUM(sele), (UCOUNT)tam);

   if ( novo_ad == 0 || uerr_cod != NO_ERROR ) {
      ret = -3;
      goto fim;
   }

   rc->rc_pos = novo_ad;

   ret = WRTVREC(AD_NUM(sele), novo_ad, ioarea, tam);

   /* descriptografa apos gravar para poder desindexar */

   de_cripta((UTEXT *)WCB->cb_ioarea, tam);

   if ( ret != NO_ERROR ) {
      ret = -3;
      goto fim;
   }

   /*
   **    Gravacao proveniente de EDIT_RECORD
   */

   if ( alteracao ) {

      if ( atu_ac(sele, rc->rc_status, novo_ad) < 0 ) {
         ret = -1;
         goto fim;
      }

      if ( rc->rc_status == ATIVO_INDEX ) {
         set_erro(0);
         ret = reg_index(sele, ALTERACAO, '+', num_rec, FLUSH_IDX);
         set_erro(1);
         if ( ret != 0 ) {
            ret = -4;
            goto fim;
         }
      }
   }

   /*
   **    Gravacao proveniente de NEW_RECORD
   */

   else {
      switch( ap_pos_index ) {
         case POS_INDEX :  /* marca para indexar depois */
                           if ( atu_ac(sele, ATIVO_NAO_INDEX, novo_ad) < 0 ) {
                              ret = -1;
                              goto fim;
                           }

                           /* assume que vai ter importacao total depois */
                           if ( par_cfg.flags_especiais & 0X02 ) {
                              break;
                           }

                           sprintf(chave, "%9.9ld", num_rec);
                           if ( pal_index(sele, TREE_NAO_INDEX,
                                 (UTEXT *)chave, strlen(chave) + 1) != 0 ) {
                              ret = -3;
                              goto fim;
                           }
                           break;

         case NOW_INDEX :  /* indexa na mesma hora */
                           if ( atu_ac(sele, ATIVO_INDEX, novo_ad) < 0 ) {
                              ret = -1;
                              goto fim;
                           }

                           if ( reg_index(sele, INCLUSAO, '+', num_rec,
                                     acumula_idx) != 0 ) {
                              ret = -1;
                              goto fim;
                           }
                           break;

         case NO_INDEX  :  /* so chamado por rebuild base */
                           if ( atu_ac(sele, NO_INDEX, novo_ad) < 0 ) {
                              ret = -1;
                              goto fim;
                           }
                           break;

      }
   }

   ret = 0;

   if ( ++ap_curflushfile >= ap_flushfile ) {
      FLUSHFIL(AC_NUM(sele));
      FLUSHFIL(AD_NUM(sele));
      FLUSHFIL(AI_NUM(sele));
      ap_curflushfile = 0;
   }

fim :

   if ( ret_trans >= 0 )
      END_TRANS(TR_WRRECORD);

   switch(ret) {
      case -1 :
                  break;
      case -2 :   /* mens_erro(H_WRUNICA, E_WRUNICA, mens); */
                  break;
      case -3 :   mens_erro(H_LBNBAD, E_LBNBAD, uerr_cod);
                  break;
      case -4 :   mens_erro(H_REGIND, E_REGIND); /* Erro na indexacao do registro */
                  break;
      case -5 :   mens_erro(H_WRTEXTO, E_WRTEXTO);
                  break;
   }

   return(ret);
}





/*----------------------------------------------------------------------*/
/* EDIT_RECORD                                                          */
/*----------------------------------------------------------------------*/
F__GLB   COUNT edit_record(sele, nreg, win, it_ativo)
COUNT sele;
LONG nreg;
WIN *win;
COUNT it_ativo;
{  COUNT ret, flag, salva, atual, last_campo;
   static TEXT aux[30];
   UTEXT lin_escape[120];
   WIN *jm = NULL, *jf = NULL;
   TTela *pe;
   Campo *pc;

CAI_FORA(nreg);  /* se registro maior que maximo cai fora */


   if ( varre_vista(sele, 1) != 1 ) { /* teste se tem campo entrada */
      mens_erro(H_NOVIS, E_NOVIS);
      ret = -1;
      goto fim;
   }

   if ( read_record(sele, nreg, 1) != 0 ) {
      ret = -99;
      goto fim;
   };


   jm = abre_mensagem(str_nada);
   jf = abre_funcoes(str_nada);

   sprintf(aux, C_REGLD, nreg);

   flag   = FICA;
   salva  = 0;

   if ( it_ativo != -1 )
      ap_vista.vis_it_ativo  = it_ativo;   /* Mudanca de 16/11/93 p/ EMPAX */

ret_critica : /* retorno para critica.  Dn->cb_cpativo setado p/ invalido */

   while ( ! salva ) {

      pe = anda(sele, flag, 'E', win, aux, &last_campo);

      if ( pe == NULL ) {
         ret = -1;
         break;
      }

      if ( pe->te_ivetcp < 0 || pe->te_ivetcp > 127 ) {
         debug("ivetcp fora de limites");
         continue;
      }

      pc = pos_vetcmp(pe->te_icb)[pe->te_ivetcp];

      if ( pc->cp_tipo == 'T' ) {
         funcoes(jf, funcao("1 2 3 5 9 E"));
         mens_1(jm, pc->cp_rodape);

         /* informa ao editor os caracteres escapes */
         pr_escape("1239TBE", lin_escape);

         /* @@@ ANANIAS so teste ***/
         if ( pc->cp_adoc == NULL ) debug("Nao pode setar escape de NULL");
         pc->cp_adoc->escape = (TEXT *)lin_escape;

         ret = rd_texto( pe, pc, jm);
         if ( ret < 0 ) {
            mens_erro(H_EDTX01, E_EDTX01);
            ret = -1;
            goto fim;
         }
      }
      else {
         funcoes(jf, funcao("1 2 3 9 0 E"));
         ret = rd_campo(pe, pc, win, jm);
      }

      /*
      ** so navega so retorna K_F2 ou K_F7 ou K_F8 ou K_F9
      ** K_PGDN ou K_PGUP ou K_ESC
      */

		switch ( ret ) {

         case K_ESC  :
                        if ( reg_updated(sele, 'M') ) {
                           if ( decida(str_nada, OP_SIM_NAO, H_CFAEDI, 1, 30, "%s", M_SAIEDIT) == 0 ) {
                              ret = -2;
                              salva = 1;
                              goto fim;
                           };
                        }
                        else {
                           salva = 1;
                           ret = -2;
                           goto fim;
                        };

                        break;

         case K_F2   :
                        if ( reg_updated(sele, 'I') == 1) {
                           salva = 1;
                        }
                        else {
                           mens_erro("", E_NOVAZIO );
                        }
                        break;

#ifdef VISUAL
         case K_ALT_F4   :  break;
#endif
         case K_F7   :  break;

         case K_F8   :  break;

         case K_F9   :  atual = ap_vista.vis_ordem;

                        while ( 1 ) {
                           if ( (ret = muda_vista(atual, -1)) < 0 ) {
                              ret = -1;
                              goto fim;
                           }
                           /* verifica se existe CAMPO_E */
                           if ( varre_vista(sele, 1) ) break;
                           mens_erro(E_NOVIS, E_NOVIS);
                           if ( ret == atual ) {
                              ret = -1;
                              goto fim;
                           }
                        }

                        flag   = PRIMEIRO;
                        break;

         case K_F0   :  if ( ap_status == 'A' ||
                             ap_acesso_ativo.da_emite == 'X' ) {
                           em_edicao = 1;  /* especial para nao dar free_buffer em emite */
                           emite(sele, reg_corrente, NULL);
                           em_edicao = 0;
                        }
                        else {
                           mens_erro(H_NOIMP, E_NOIMP);
                        }
                        break;

         case K_BTAB :
			case K_UP   :  flag = LBSOBE;
                        break;

         case K_PGUP :  flag = TELA_ANT;
                        break;

         case K_PGDN :  flag = TELA_POS;
                        break;

         default     :
         case K_TAB  :
         case K_DN   :
         case K_ENTER:
                        flag = LBDESCE;

                        /* a principio vou deixar campo autonumeravel obrigatorio passar */
                        /* depois eu pego na finalizacao da edicao */

/*
                        if ( esp_base.tecla_limpa != 0 && esp_base.tecla_limpa == ret ) {
debug("WWW limpando");
                           flag = FICA;
                           break;
                        }
*/
                        if ( pe->te_obrigatorio == 'S' && pc->cp_critica != 'A') {
                           if ( pc->cp_newtam == 0 ) {
                              mens_erro(H_INFOBRIG, E_INFOBRIG);
                              flag = FICA;
                           }
                        }

                        if (pc->cp_mod_flag == 1 && critica_campo(pc) != 0 ) {
                           if ( pc->cp_critica == 'A' ) {
                              pc->cp_newdata[0] = '\0';
                              pc->cp_newtam     = 0;
                           }
                           flag = RE_LER;
                        };

                        if ( ap_autosave && last_campo && (flag != FICA && flag != RE_LER) ) {
                           salva = 1;
                           break;
                        }

                        break;
      }

      /* imprime possiveis campos com mais de uma aparicao na mesma tela */
      if ( flag != PRIMEIRO )
         disp_mtela(sele, win);
   };

#ifdef AUTO_INCREMENTO
      /* somente se inclusao com campo vazio */
   	if( c_bases[sele]->cb_controle.rc_pos == 0 && ap_flag_auto == 1) {
   		/* para campos auto-incrementados tenho que preencher campo */

         COUNT auto_fieldnumber;
   		Campo **C_Cmp, *pc;

   		if( ( auto_fieldnumber = auto_getfield( ) ) != -1 ) {

            /* procura se existe campo obrigatorio vazio e o mesmo e o autonum */
            varre_vista(sele, 2);

            if ( vv_cmpvazio == auto_fieldnumber ) {
   			   C_Cmp = pos_vetcmp(sele);
               pc = C_Cmp[auto_fieldnumber];
   			   /* ver se tem dado antes de incrementar */
   			   if( pc->cp_newdata[0] == 0 ) {
   				   auto_incre(pc->cp_newdata, pc->cp_mhor);
   				   pc->cp_newtam = strlen(pc->cp_newdata);
   			   }
            }
   		}
   	}
#endif

   if ( salva == 1 && reg_updated(sele, 'M') ) {

      /* retorna numero do campo rejeitado */

      mens_1(jm, M_WAITCRIT);

      if ( (ret = crit_reg(sele)) > 0 ) {
         ap_vista.vis_it_ativo = ret - 1;
         ap_vista.vis_old_ativo = -1;
         flag   = FICA;
         salva  = 0;
         goto ret_critica;
      }

      mens_1(jm, M_WAITATU);

      /* Alteracao para criar campo Historico do andamento - INCRA */
      if ( esp_base.presente != 0 )
         trata_historico( );


      if ( write_record(sele, FLUSH_IDX) != 0 ) {
         if ( n_cp_unica != -1 ) {
            mens_erro(H_WRUNICA, E_WRUNICA, pos_vetcmp(pe->te_icb)[n_cp_unica]->cp_nome);
         }
         ret = -2;
         goto fim;
      }

      /* WWW mudanca para vista padrao apos gravacao */
      if ( esp_base.presente && esp_base.vista_aposgravar != -1)  {
         muda_vista(ap_vista.vis_ordem, esp_base.vista_aposgravar );
         ap_vista.vis_old_ativo = -1;
      }

   }



fim :


   if ( ret < 0 ) {
      expa_registro(sele);
   };

   fecha_funcoes(jf);
   fecha_mensagem(jm);

sai:

   pe_global = NULL;

   return(ret);
}



F__GLB   COUNT atu_ac(sele, flag, pos)
COUNT sele;
COUNT flag;
POINTER pos;
{  COUNT ret;
   POINTER pos_ac;
   Reg_controle *rc;
   time_t tempo;
   TEXT *mens, aux[80];

   pos_ac = c_bases[sele]->cb_aclock;
   rc     = &c_bases[sele]->cb_controle;

   if ( pos_ac == (POINTER) 0 ) {
      ret = -1;
      goto fim;
   }


   rc->rc_status = flag;
   rc->rc_pos    = pos;


   time(&tempo);

   if ( flag == NOVO_REGISTRO ) {
      rc->rc_incdata = tempo;
      rc->rc_altdata = tempo;
   }
   else if ( flag == LIBERADO ) {
      rc->rc_incdata = 0L;
      rc->rc_altdata = 0L;
   }

   else {
      rc->rc_altdata = tempo;
   }


ac_write :

   ret = WRTREC(AC_NUM(sele), pos_ac, (TEXT *)rc);

   if ( ret != NO_ERROR ) {
      switch ( ret ) {
         case  SEEK_ERR :
         case  WRITE_ERR :  mens = E_TBPWRITE;
                            break;
         default         :  sprintf(aux, E_LBNBAD, uerr_cod);
                            mens = aux;
                            break;
      };

		switch (decida(TW_RDA0, TW_RDA9, H_CFWRREG, 1, 42, "%s", mens)) {
         case -1 :   /* ESC */
			case 0  :   /* RETENTA */
                     goto ac_write;

			case 1  :   /* DESISTE */
                     ret = -1;
                     goto fim;

			case 2  :   /* ABORTA */
                     fim_lb(M_SOLIC);
      }
   }

fim :

   return(ret);
}



F__GLB   COUNT varre_vista(sele, analise)
COUNT sele;
COUNT analise;  /* 1-existe campo entrada   2-existe campo Obrig. vazio */
{  COUNT ret, i, n_elem;
   Campo *pc;
   TTela *pe;

   sele = sele;

   ret = 0;
   vv_cmpvazio = -1;

   n_elem  = qtd_itvis();

   for ( i = 0; i < n_elem ; i++ ) {

      pe = pos_itvis(i);

      if ( pe->te_tipo != CAMPO_E )
         continue;

      if ( pe->te_ivetcp < 0 || pe->te_ivetcp > 127 ) {
         debug("ivetcp fora de limites");
         continue;
      }

      pc = pos_vetcmp(pe->te_icb)[pe->te_ivetcp];

      if ( analise == 1 ) {
         if ( pc->cp_rodape[0] != '@' ) {
            ret = 1;
            break;
         }
      }
      else {
         if ( pe->te_obrigatorio == 'S') {
            if ( cp_vazio(pc, 'N') == 0 ) {
               ret = 1;
               vv_cmpvazio = pe->te_ivetcp;
               break;
            }
         }
      }
   }

   return(ret);
}

F__GLB   COUNT crit_reg(sele)
COUNT sele;
{  COUNT ret, i, n_elem;
   Campo *pc;
   TTela *pe;

   sele = sele;

   n_elem  = qtd_itvis();

   for ( i = 0; i < n_elem ; i++ ) {

      pe = pos_itvis(i);

      if ( pe->te_tipo == 'E' ){
         pc = pos_vetcmp(pe->te_icb)[pe->te_ivetcp];
         if ( pc->cp_tipo == 'T' ) continue;
         if ( pc->cp_crt_flag == 1 ) {
            ret = critica_campo(pc);
            if ( ret != 0 ) {
               if ( ret == -2 ) {
                  ret = last_rdcampo + 1;
               }
               else
                  ret = i + 1;
               goto fim;
            }
         }
      }
   }

   for ( i = 0; i < n_elem ; i++ ) {
      pe = pos_itvis(i);
      if ( pe->te_tipo == 'E' && pe->te_obrigatorio == 'S') {
         pc = pos_vetcmp(pe->te_icb)[pe->te_ivetcp];
         if ( cp_vazio(pc, 'N') == 0 ) {
            mens_erro(H_INFOBRIG, E_INFOBRIG);
            ret = i + 1;
            goto fim;
         }
      }
   }

   ret = 0;

fim :

  return(ret);
}


F__GLB   COUNT critica_campo(pc)
Campo *pc;
{  COUNT ret, tipo;
   LONG reg;

   if ( pc == NULL ) debug("Campo NULL invalido");

   tipo = 'V';
   ret  = 0;

   if ( pc->cp_tipo == 'T' ) {
      goto fim;
   }

   if ( pc->cp_tipo == 'D' || pc->cp_tipo == 'd' ) {
      tipo = 'D';
      if ( valdata(pc->cp_newdata) < 0 ) {
         mens_erro(H_VLIT1, E_VLIT1);
         ret = -1;
         goto fim;
      }
   }
   else if ( pc->cp_tipo == 'H' || pc->cp_tipo == 'h' ) {
      tipo = 'H';
      if ( valhora(pc->cp_newdata ) < 0 ) {
         mens_erro(H_VLIT2, E_VLIT2);
         ret = -1;
         goto fim;
      }
   }

   if ( cp_vazio( pc, 'N' ) == 0 ) {
      ret = 0;
      goto fim;
   }

   switch ( pc->cp_critica ) {
      case   0  :
      case  'N' : ret = 0;
                  break;

      case  'I' : ret = intervalo(pc->cp_param.intervalo.cp_minimo, pc->cp_newdata,
                                  pc->cp_param.intervalo.cp_maximo, tipo);
                  if ( ret < 0 ) {
                     mens_erro(H_VLIT4, E_VLIT4,
                               pc->cp_param.intervalo.cp_minimo,
                               pc->cp_param.intervalo.cp_maximo );
                     ret = -1;
                  }
                  break;

      case  'T' : ret = val_tabela(pc->cp_newdata, pc, 0); /* 0 nao permite selecionar */

                  if ( ret < 0 ) {
                     /* mens_erro(H_ITEMTAB, E_ITEMTAB); */
                     mens_erro("", E_NOITEMTAB);
                     ret = -1;
                  }
                  break;

      case  'M' : ret = valdigito(pc->cp_newdata);
                  if ( ret < 0 ) {
                     mens_erro(H_DVINV, E_DVINV);
                     ret = -1;
                  }
                  break;

      case  'C' :
#ifdef ESPANHA
                  ret = valnif(pc->cp_newdata);
                  if ( ret < 0 ) {
                     mens_erro(H_NIFINV, E_NIFINV);
                     ret = -1;
                  }
#else
						ret = valcpf(pc->cp_newdata);
                  if ( ret < 0 ) {
                     mens_erro(H_CPFINV, E_CPFINV);
                     ret = -1;
                  }
#endif

                  break;

      case  'G' :
#ifdef ESPANHA
						ret = valcif(pc->cp_newdata);
                  if ( ret < 0 ) {
                     mens_erro(H_CIFINV, E_CIFINV);
                     ret = -1;
                  }
#else
						ret = valcgc(pc->cp_newdata);
                  if ( ret < 0 ) {
                     mens_erro(H_CGCINV, E_CGCINV);
                     ret = -1;
                  }
#endif

                  break;

   }


   reg = c_bases[BASE_EDIT]->cb_numrec;

   if ( pc->cp_chave_unica == 'S' ) {
      if ( touch_unica(pc, &reg, pc->cp_newdata, '?') < 0 ) {
         mens_erro(H_EXUNICA, E_EXUNICA, pc->cp_nome);
         if ( pc->cp_rodape[0] == '@' )
            ret = -2;  /* -2 vai indicar um tratamento especial */
         else
            ret = -1;
         goto fim;
      }
   }

fim :

   if ( ret == 0 ) {
      pc->cp_crt_flag = 0;
   }
   else {
      pc->cp_crt_flag = 1;
   }

   return(ret);
}









/* WWW */
F__GLB   COUNT salva_textos(sele, nreg, especifico)
COUNT sele;
LONG nreg;
COUNT especifico;
{  COUNT ret, i, num_cps;
   COUNT ret_trans = 0;
   POINTER pos;
   LONG    novo_tam;
   Campo *pc;


   if ( especifico != -1 && (ret_trans = BEG_TRANS(TR_SALVATXT, nreg, 0)) < 0 ) {
      ret = -1;
      goto fim;
   }
   num_cps = qtd_vetcmp(sele);

   for ( i = 0 ; i < num_cps; i++ ) {

      if ( especifico >= 0 ) {
         i = especifico;
         pc = pos_vetcmp(sele)[i];
         i = 999;
      }
      else {
         pc = pos_vetcmp(sele)[i];
      }


      if ( pc->cp_tipo     != 'T' ) continue;
      if ( pc->cp_adoc     == NULL) continue;
      if ( pc->cp_mod_flag == 0   ) continue;

      novo_tam = tam_buffer(pc->cp_adoc, ET_HANDLE);

      {  TEXT buf[100];
         /* Incluido na versao 2.3 rel 1.0 pois estava gravando campo em branco */
         if ( novo_tam < 100 && pc->cp_adoc->t_lines == 1 && lin_buffer( pc->cp_adoc, 0, buf) <= 1 )
            novo_tam = 0;
      }

      if ( novo_tam == 0 ) {
         memset((TEXT *)pc->cp_newdata, '\0' , sizeof(POINTER));
         pc->cp_newtam = 0;
      }

      else {

         pos = txt_save(AD_NUM(sele), pc->cp_adoc, nreg, pc->cp_num);

         if ( pos == (POINTER) 0 ) {
            mens_erro(H_WRTEXTO, E_WRTEXTO);
            ret = -1;
            goto fim;
         }

         /* ANANIAS @@@ olha a portabilidade */
         memcpy((TEXT *)pc->cp_newdata, &pos  , sizeof(pos));
         pc->cp_newtam = sizeof(POINTER);
      }

   }

   ret = 0;

fim :

   if ( ret_trans >= 0 )
      END_TRANS(TR_SALVATXT);


   return( ret );
}

F__LOC   COUNT reg_updated(sele, flag)
COUNT sele, flag; /* flag pode ter 'I' (tem informacao ) ou 'M' (modificado) */

{  COUNT ret, i, num_cps;

   num_cps = qtd_vetcmp(sele);

   if ( flag == 'I' ) { /* verifica se algum campo tem informacao */
      for ( ret = i = 0; i < num_cps; i++ ) {
         if ( cp_vazio(pos_vetcmp(sele)[i], 'N') != 0 ) {
            ret = 1;
            break;
         }
      }
   }
   else {
      for ( ret = i = 0; i < num_cps; i++ ){
         if ( pos_vetcmp(sele)[i]->cp_mod_flag != 0 ) {
            ret = 1;
            break;
         }
      }

      if ( varre_vista(sele, 2) ) /* campo obrigatorio vazio */
         ret = 1;
   }

   return(ret);
}



F__LOC   COUNT pack_registro(sele)
COUNT sele;
{  COUNT num_cps, tam, i;
   Campo **vet_cmp, *pc;
   TEXT *n_campos;
   UTEXT *pmarca;
   TEXT  *preg;
   TEXT *p_io;


   vet_cmp  = pos_vetcmp(sele);
   num_cps  = qtd_vetcmp(sele);

   p_io     = c_bases[sele]->cb_ioarea;

   pmarca   = (UTEXT *)p_io;
   preg     = p_io + 1;
   n_campos = p_io + HDR_REGISTRO - 1;

   *pmarca   = MARCA_REGISTRO;
   PUT_LONG((UTEXT *)preg, c_bases[sele]->cb_numrec);
   *n_campos = '\0';

   p_io += HDR_REGISTRO;

   for ( i = 0; i < num_cps; i++ ) {

      pc = vet_cmp[i];

      if ( pc->cp_tipo == 'T' )  tam = 4;
      else                       tam = pc->cp_newtam;

      if ( tam > 0 ) {
         *p_io++ = pc->cp_num;
         *p_io++ = tam;
         pc->cp_olddata = p_io;
         pc->cp_oldtam  = tam;
         memcpy((TEXT *)p_io, pc->cp_newdata, tam);
         p_io += tam;
         *n_campos = *n_campos + 1;
      }
      else {
         pc->cp_olddata = NULL;
         pc->cp_oldtam  = 0;
      }
   };

   return( (COUNT)(p_io - c_bases[sele]->cb_ioarea));

}



F__LOC   VOID disp_formulas(sele, win, tela)
COUNT sele;
WIN *win;
COUNT tela;
{  COUNT i, n_elem;
   TTela *pe;
   Campo *pc;

   n_elem  = qtd_itvis();

   for ( i = 0; i < n_elem; i++ ) {

      pe = pos_itvis(i);

      if ( pe->te_tipo != CAMPO_E && pe->te_tipo != CAMPO_S ) continue;

      pc = pos_vetcmp(sele)[pe->te_ivetcp];

      if ( pc->cp_tipo != 'F' && pc->cp_rodape[0] != '@' )  continue;
      if ( pe->te_tela != tela )       continue;

      xuxa_campo(pc, pe, win);
   }
}



F__GLB   COUNT rd_campo(pe, pc, win, jm)
TTela *pe;
Campo *pc;
WIN *win, *jm;
{  COUNT ret, param, propos = 0;
   COUNT protocola = ap_autonum;  /* indica se foi apertada tecla de auto_num */
   TEXT *ajuda_ant = win_ajuda, path_help[80];
   TEXT *p_rodape;
   UTEXT escapes[10];
	d_entry work;
   static COUNT last_key;

   win_ajuda = "CMPUSU";


   if ( ap_limpacampos ) {
      limpa_campos(esp_base.campos_altf6);
      disp_campos(BASE_EDIT, win);
      ap_limpacampos = 0;
   }


   if ( pe->te_tipo != CAMPO_E ) {
      ret = so_navega(win, 0, NULL, jm, NULL);
      goto fim;
   }


   /* caso a base utilize restricoes de edicao e nao seja um Administrador */
   if ( pc->cp_rodape[0] != '@' && rst_base.ativada == 1 && ap_status != 'A' ) {
      TEXT msg[80];
      if ( pode_editar(msg, pc->cp_nome) == 0 ) {
         COUNT c,l;
         c = pe->te_coluna;
         l = pe->te_linha;
         fwat( c, l, win );
/*         xuxa_campo(pc, pe, win); */
         ret = so_navega(win, 0, NULL, jm, msg);
         goto fim;
      }
   }


   if ( pc->cp_rodape[0] == '@' )
      trata_indireto(pc->cp_rodape, pc, 0);

auto_num :

   if ( pc->cp_newdata[0] == '\0' ) {  /* Campo esta vazio. Prepara read */

      /* Campo esta vazio. Teclou K_F8. Campo auto_incremento */
      if ( pc->cp_critica == 'A' && protocola == 1 && ap_flag_auto == 1) {
   		auto_incre(pc->cp_newdata, pc->cp_mhor);
   		pc->cp_newtam = strlen(pc->cp_newdata);

/* cuidado pois tem mascara de tudo quanto e jeito
            {
               TEXT buf[20];
   		      auto_incre(buf, pc->cp_mhor);
               formata_campo(pc->cp_newdata, buf, pc->cp_mascara, E_ALFA);
               pc->cp_newtam = strlen(pc->cp_newdata);
            }
*/

         propos = 1;
         if ( ap_autonum && pe->te_append != 'S' )
            w_ungc( K_ENTER );
      }

      if ( pc->cp_tipo == 'D' || pc->cp_tipo == 'H' ) {
         propos = 1;
         strcpy(pc->cp_newdata, data_hora(pc->cp_tipo, 0L));
      }

      /* implementacao para proposta de preenchimento de campo */
      if ( pc->cp_rodape[0] == '[' && pc->cp_rodape[1] == '=' ) {
         COUNT ind, num, sele = BASE_EDIT;
         TEXT *p, campo[61];
         strcpy( campo, pc->cp_rodape+2);
         if ( ( p = strchr(campo, ']' )) == NULL )
            debug( "Sintaxe para copia de conteudo do campo (%s) invalida", pc->cp_nome);
         else
            *p = 0;
         ind = ver_campo(sele, campo, &num);
         if ( ind >= 0 && c_bases[sele]->cb_numrec != 0L ) {
            p =  pos_vetcmp(sele)[ind]->cp_newdata;
            strncpy(pc->cp_newdata, p, pc->cp_mhor);
            pc->cp_newtam = strlen(pc->cp_newdata);
            propos = 1;
         }
      }
   }

   if ( pc->cp_tipo == 'N' ) param = E_VALOR;
   else                      param = E_ALFA;

   if (pe->te_vhor < strlen(pc->cp_mascara) ) param |= E_CONFIRMA;

   if ( pc->cp_rodape[0] == '@' )
      param |= E_MOSTRA;

   /* para FHC nao ler quando ja tem */

   if ( pc->cp_critica == 'A' && pc->cp_newdata[0] != '\0' && pe->te_append == 'S' )
      param |= E_MOSTRA;

   if ( pc->cp_rodape[0] == '[' && pc->cp_rodape[1] == '=' && strchr( pc->cp_rodape, ']') != NULL)
      p_rodape = strchr( pc->cp_rodape, ']') + 1;
   else
      p_rodape = pc->cp_rodape;


   d_entrada( &work, win, pc->cp_newdata, pe->te_vhor, pc->cp_mascara,
              p_rodape, pe->te_coluna, pe->te_linha, param);

   /* prepara string de escapes para entrada */
   if ( pc->cp_critica == 'A' )
      pr_escape("2 8 9 0", escapes);
   else
      pr_escape("2 9 0", escapes);

   work.escape  = escapes;
   work.men     = jm;

   /* o Usuario pode definir uma tecla para limpar alguns campos */
   if ( esp_base.tecla_limpa != 0 ) {
      TEXT *p0;
      p0 = strchr((TEXT *)work.escape, '\0');
      *p0++ = esp_base.tecla_limpa;
      *p0++ = '\0';
   }


   /* monta help de campo */
   if ( pc->cp_help[0] != '\0' && (param & E_MOSTRA) == 0 )
      help_campo(pc, path_help);


   /* para forcar abrir tabela obrigatoriamente */
   if ( pe->te_obrigatorio == 'S' && pc->cp_critica == 'T' && pe->te_append == 'S' && pc->cp_newtam == 0 ) {
      w_ungc( K_ENTER );
      w_ungc( '?' );
   }


	ret = entrada( &work, 0);   /************************************/


   /* para manter fluxo de tab e back_tab */
   if ( ap_autonum && pe->te_append == 'S' && (auto_getfield( ) == pc->cp_num) )
      ret = last_key;

   user_help = NULL;

   /* tratamento especial no caso de propor data e a mesma nao for alterada */
   if ( work.tecla == K_ESC ) {
      if ( propos == 1 )
         pc->cp_newdata[0] = '\0';
   }
   else
      if ( propos == 1 ) work.mudou = 1;

   pc->cp_newtam = strlen(work.str);

   if ( ret == K_F8 && pc->cp_critica == 'A') {
      protocola = 1;
      goto auto_num;
   }
   else
      protocola = 0;

   /* o Usuario pode definir uma tecla para limpar alguns campos */
   if ( esp_base.tecla_limpa != 0 && ret == esp_base.tecla_limpa ) {
      limpa_campos(esp_base.campos_limpa);
      disp_campos(BASE_EDIT, win);
      goto auto_num;
   }

   /* marca ultimo lido para reler caso ocorra erro de chave unica composta duplicada */
   if ( pc->cp_rodape[0] != '@' && work.mudou != 0 )
      last_rdcampo = ap_vista.vis_it_ativo;

   if ( pc->cp_rodape[0] == '@' ) {
      if ( pc->cp_olddata == NULL || strcmp(pc->cp_newdata, pc->cp_olddata) != 0 )
         work.mudou = 1;
      ret = last_key;
      /* estava entrando em loop quando primeiro campo da vista */
      /* resolvi tratar como excessao o F2 e o F9 e o ESC.  Em 26/02/93 base BAMA */

      if ( ret == K_F2 || ret == K_F9 || ret == K_ESC )
         ret = K_ENTER;
   }
   else
      last_key = ret;

   if ( work.mudou && pc->cp_critica == 'T' && work.str[0] != '\0' ) {
      COUNT rt;
      rt = val_tabela(work.str, pc, 1);
      if ( rt > 0 ) {
         work.tipo |= E_MOSTRA;
         entrada( &work, 0);
         work.mudou = 1;
      }
      else if ( rt == 0 ) {
         if ( work.str[0] == '\0' ) {
            ret = work.tecla = K_ESC;
            work.mudou = 0;
         }
      }
      else {
         pc->cp_mod_flag = 1;
      }
   }

   if ( work.mudou || work.tecla == K_F2 ) {
      pc->cp_mod_flag = 1;
      pc->cp_crt_flag = 1;


      pc->cp_newtam = strlen(work.str);

      trata_outs(BASE_EDIT, 0);
      trata_formula(BASE_EDIT);
      trata_outs(BASE_EDIT, 0); /* repetido para sanar referencia multipla */

      disp_formulas(BASE_EDIT, win, pe->te_tela);
   }


fim :

   win_ajuda = ajuda_ant;

   return(ret);
}



F__GLB   COUNT rd_texto( pe, pc, jm)
TTela *pe;
Campo *pc;
WIN *jm;
{  COUNT ret;
   COUNT forma;
   TEXT *ajuda_ant = win_ajuda, path_help[80];

   win_ajuda = "EDITOR";

   /* monta help de campo */
   if ( pc->cp_help[0] != '\0')
      help_campo(pc, path_help);

   if ( pe->te_tipo != CAMPO_E ) {
      ret = edit_buffer(pc->cp_adoc, ET_MOSTRA, pe->te_win, NULL, 0 );
      goto fim;
   }


   /* caso a base utilize restricoes de edicao e nao seja um Administrador */
   if ( rst_base.ativada == 1 && ap_status != 'A' ) {
      TEXT msg[80];
      if ( pode_editar(msg, pc->cp_nome) == 0 ) {
         pc->cp_adoc->def_mens = msg;
         if ( (ret = edit_buffer(pc->cp_adoc, ET_NAVEGA, pe->te_win, NULL, 0 )) < 0 )
            numdebug(403);
         goto fim;
      }
   }



   pc->cp_adoc->def_mens = pc->cp_rodape;

   forma = (pe->te_append == 'S') ? ET_APPEND : ET_EDITA;

   ret = edit_buffer(pc->cp_adoc, forma, pe->te_win, jm, 0 );


   if ( pc->cp_adoc->f_mod != 0 )
         pc->cp_mod_flag = 1;



fim :

   user_help = NULL;
   win_ajuda = ajuda_ant;

   return( ret );
}


F__GLB   COUNT cp_vazio(pc, oldnew)
Campo *pc;
COUNT oldnew;
{  COUNT ret=0, i, tam_str;
   LONG tam;
   TEXT m, *p;

   if ( oldnew == 'N' ) {
      p = pc->cp_newdata;
      tam_str = pc->cp_newtam;
   }
   else{
      if ( pc->cp_olddata == NULL )
         goto fim;

      p = pc->cp_olddata;
      tam_str = pc->cp_oldtam;
   }

   ret = 0;

   if ( pc->cp_tipo == CP_TEXTO ) {          /* VVVVVVVVVVVVVVVVVVVVVVV Incluido em 11/09/92 pra corrigir erro em cp_texto indexado em branco - Ananias */
      if ( oldnew == 'N' && pc->cp_adoc != NULL && pc->cp_mod_flag != 0 ) {
         /* existe documento aberto */

         tam = tam_buffer(pc->cp_adoc, ET_HANDLE);

         {  TEXT buf[100];
            /* Incluido na versao 2.3 rel 1.0 pois estava indexando campo em branco */
            if ( tam < 100 && pc->cp_adoc->t_lines == 1 && lin_buffer( pc->cp_adoc, 0, buf) <= 1 )
               tam = 0;
         }

      }
      else {
         tam = 0;

         if ( p != NULL && get_postexto(oldnew, (UTEXT *)p) != (POINTER) 0 )
            tam = 1;

/*   Estava assim ate incluir  GET_LONG4 em 19/01/97 - Ananias */
/*         if ( p != NULL && GET_LONG((UTEXT *)p) != (POINTER) 0 ) */
/*            tam = 1; */

      }

      if (tam != 0 ) ret = 1;
   }
   else {
      for ( i = 0 ; i < tam_str; i++ ) {

         if ( pc->cp_tipo == 'F' )  m = pc->cp_param.cp_msc_form[i];
         else                       m = pc->cp_mascara[i];

         if ( strchr("AaXx9*", m) != NULL  && p[i] != ' ') {
            ret = 1;
            break;
         }
      }
   }

fim :

   return(ret);
}


COUNT pode_editar(msg, nomecp)
TEXT *msg, *nomecp;
{  COUNT ret = 0;
   TEXT aux[30];


   if ( res_usu(aux, nomecp, rst_base.UD) ) {
      sprintf(msg, "Edicao do campo %s restrita ao Usuario : %s (Dono)", nomecp, aux);
      goto fim;
   }

   if ( res_grp(aux, nomecp, rst_base.GD) ) {
      sprintf(msg, "Edicao do campo %s restrita ao Grupo : %s (Grupo Dono)", nomecp, aux);
      goto fim;
   }
   if ( res_usu(aux, nomecp, rst_base.UR) ) {
      sprintf(msg, "Edicao do campo %s restrita ao Usuario : %s (Usuario Responsavel)", nomecp, aux);
      goto fim;
   }

   if ( res_grp(aux, nomecp, rst_base.GR) ) {
      sprintf(msg, "Edicao do campo %s restrita ao Grupo : %s (Grupo Responsavel)", nomecp, aux);
      goto fim;
   }

   ret = 1;

fim :

/* if ( ret != 1 ) debug("WWW msg = %s", msg); */

   return(ret);
}

COUNT res_usu(nome, nomecp, ind)
TEXT *nome, *nomecp;
COUNT ind;
{  COUNT ret = 0;
   TEXT aux[20];


   if ( ind != -1 ) {
      TEXT *p;

      /* verifica se o campo em questao esta sujeito a edicao restrita */
      strcpy(aux, nomecp);
      strcat(aux, ",");

/* debug("WWW ver se campo presente na lista de restricoes [%s] [%s]", rst_base.UDcampos, aux); */
      if ( strstr(rst_base.UDcampos, aux) != NULL ) {
         p = field_info(BASE_EDIT, ind, 0, NULL, 16);
         strupr(p);
/* debug("WWW o que tem no campo [%s] usuario corrente [%s]", p, ap_nome); */
         if ( p[0] != '\0' && strcmp(p, ap_nome) != 0 ) {
            strcpy(nome, p);
            ret = 1;
         }
      }
   }

   return(ret);
}

COUNT res_grp(nome, nomecp, ind)
TEXT *nome, *nomecp;
COUNT ind;
{  COUNT ret = 0, l;
   TEXT aux[20];


   if ( ind != -1 ) {
      TEXT *p;

      /* verifica se o campo em questao esta sujeito a edicao restrita */
      strcpy(aux, nomecp);
      strcat(aux, ",");

/* debug("WWW ver se campo presente na lista de restricoes [%s] [%s]", rst_base.UDcampos, aux); */
      if ( strstr(rst_base.UDcampos, aux) != NULL ) {
         l = strlen(ap_local);

         p = field_info(BASE_EDIT, ind, 0, NULL, 16);
         strupr(p);

         if ( p[0] == '\0' )
            goto fim;

         if ( rst_base.hierarquico && strlen(p) > l ) {
            if ( strncmp(ap_local, p, l) != 0 || p[l] != '/' ) {
               strcpy(nome, p);
               ret = 1;
            }
         } else if ( strcmp(ap_local, p) != 0 ) {
            strcpy(nome, p);
            ret = 1;
         }
      }
/* debug("WWW o que tem no campo [%s] usuario corrente [%s]", p, ap_local); */

   }

fim :


   return(ret);
}


VOID trata_historico()
{  COUNT i;

   for (i = 0; i < 10; i++) {
      if ( esp_base.linhas_ap[i] != NULL ) {
         if ( mudou_campos(esp_base.linhas_ap[i]) != 0 )
            atualiza_hist(esp_base.linhas_ap[i]);
      }
   }

}


COUNT mudou_campos(linha_ap)
TEXT *linha_ap;
{  COUNT ret =0, i, ind, num;
   TEXT campos[200], aux[20];

   piece(linha_ap, campos, ' ', 1, sizeof(aux) - 1);

   for ( i = 1; piece(campos, aux, ',', i, 15) > 0 ; i++ ) {
      num = -1;
      if ( (ind = ver_campo(BASE_EDIT, aux, &num)) != -1 ) {
         if ( pos_vetcmp(BASE_EDIT)[ind]->cp_mod_flag != 0 ) {
/* debug("WWW mudou campo %s", pos_vetcmp(BASE_EDIT)[ind]->cp_nome); */
            ret = 1;
            break;
         }
      }
   }

   return(ret);
}



COUNT atualiza_hist(linha_ap)
TEXT *linha_ap;
{  COUNT ret =0, i, ind, num;
   TEXT destino[20], linhas[50], aux[20];
   Campo *pcmod;

   piece(linha_ap, destino, ' ', 2, 15);
   piece(linha_ap, linhas, ' ', 3, 50);

   if ( (ind = ver_campo(BASE_EDIT, destino, &num)) != -1 ) {
      pcmod = pos_vetcmp(BASE_EDIT)[ind];
/* debug("WWW atualizando campo %s", pcmod->cp_nome); */

      for ( i = 1; piece(linhas, aux, ',', i, 5) > 0 ; i++ ) {
         COUNT ll;
         ll = atoi(aux);
         if ( ll > 0 && esp_base.linhas_det[ll] != NULL ) {
            if ( pcmod->cp_tipo == 'T' )
               inc_lintxt(esp_base.linhas_det[ll], pcmod);
            else
               inc_linalfa(esp_base.linhas_det[ll], pcmod);
/*            debug("WWW devera formatar linha %d [%s]\n", ll,  esp_base.linhas_det[ll]); */
         }
      }
   }

   return(ret);
}

COUNT inc_lintxt(str, pc)
TEXT *str;
Campo *pc;
{  COUNT ret = 0, inc_texto, start, ind, num;
   TEXT buf[200], *p, *p2;
   UTEXT linha_nova[2];
   POINTER pos;
   LONG tam;
   Campo *pco;

   memset(buf, ' ', sizeof(buf));
   buf[sizeof(buf)-1] = '\0';

   /*duplica linha para atualizar em buf */
   strncpy(buf, str, strlen(str));


   if ( pc->cp_tipo != CP_TEXTO )
      return(0);


   /* garante que o texto vai estar carregado */
   if ( pc->cp_adoc == NULL ) {
      memcpy((TEXT *)&pos, (TEXT *)pc->cp_newdata , sizeof(pos));
      if ( pos != (POINTER) 0 )
         pc->cp_adoc = txt_load(AD_NUM(BASE_EDIT), pos, &tam, pc->cp_mhor, WCB->cb_numrec);
      else
         pc->cp_adoc = load_buffer( pc->cp_mhor, ET_MEM, str_vazia, 0L);

      if ( pc->cp_adoc == NULL ) {
         if ( debug(M_PRDTXT, c_bases[BASE_EDIT]->cb_numrec, pc->cp_nome) == 'M' ) {
            pc->cp_adoc = NULL;
            ret = 0;
            goto fim;
         }
      }
   }

   p = str;
   inc_texto = 0; /* so pode Incluir um texto */

/* estava aparecendo uma linha no inicio */
   if ( pc->cp_adoc->t_lines == 1 ) {
      TEXT string[255];
      lin_texto( pc->cp_adoc, 0, (TEXT *)string, 1);
      if ( string[0] == '\0')
         delete_line(pc->cp_adoc);
   }

   while ( *p && inc_texto == 0 ) {
      if ( *p == '@' ) { /* deve ser um campo */
         TEXT campo[80];
         if ( (p2 = strchr(p, ' ')) != NULL ) {
            *p2 = '\0';
            strcpy(campo, p+1);
            *p2 = ' ';
         }
         else {
            strcpy(campo, p+1);
            p2 = strchr(p, '\0');
         }

         /* start e onde a informacao do campo devera ser colocada em buf */
         start = p - str;
         memset(&buf[start], ' ', (COUNT)(p2-p));
         p = p2;
         ind = ver_campo(BASE_EDIT, campo, &num);

         if ( ind != -1 ) {
            pco = pos_vetcmp(BASE_EDIT)[ind];
            if ( pco->cp_tipo == 'T' ) {
               inc_camp_camp(pc, pco, start);
               inc_texto = 1;
               goto ok;
            }
            else {
               strncpy(&buf[start], pco->cp_newdata, strlen(pco->cp_newdata));
            }
         }
      }
      else
         p++;
   }

   trim(buf);

   linha_nova[0] = PARA_CHR;
   linha_nova[1] = '\0';

   strcat(buf, (TEXT *)linha_nova);

   ret = add_linbuffer(pc->cp_adoc, buf);


   if ( ret != 1 ) {
      debug("WWW erro na inclusao de linha em campo Texto");
   }

ok:

   pc->cp_mod_flag = 1;

fim :

   return(ret);
}

COUNT inc_linalfa(str, pc)
TEXT *str;
Campo *pc;
{  COUNT ret = 0, start, ind, num;
   TEXT buf[200], *p, *p2;
   Campo *pco;

   memset(buf, ' ', sizeof(buf));
   buf[sizeof(buf)-1] = '\0';

   /*duplica linha para atualizar em buf */
   strncpy(buf, str, strlen(str));


   if ( pc->cp_tipo == CP_TEXTO )
      return(0);


   p = str;

   while ( *p  ) {
      if ( *p == '@' ) { /* deve ser um campo */
         TEXT campo[80];
         if ( (p2 = strchr(p, ' ')) != NULL ) {
            *p2 = '\0';
            strcpy(campo, p+1);
            *p2 = ' ';
         }
         else {
            strcpy(campo, p+1);
            p2 = strchr(p, '\0');
         }

         /* start e onde a informacao do campo devera ser colocada em buf */
         start = p - str;
         memset(&buf[start], ' ', (COUNT)(p2-p));
         p = p2;
         ind = ver_campo(BASE_EDIT, campo, &num);

         if ( ind != -1 ) {
            pco = pos_vetcmp(BASE_EDIT)[ind];
            if ( pco->cp_tipo != 'T' ) {
               strncpy(&buf[start], pco->cp_newdata, strlen(pco->cp_newdata));
            }
         }
      }
      else
         p++;
   }

   trim(buf);

   buf[pc->cp_mhor] = '\0';
   strcpy(pc->cp_newdata, buf);
   pc->cp_mod_flag = 1;

fim :

   return(ret);
}


VOID inc_camp_camp(pc, pco, start)
Campo *pc, *pco;
COUNT start;
{  COUNT ret = 0, lin_inicio;
   TEXT buf[200], linha_nova[2];
   TEXT string[ 260 ];
   POINTER pos;
   LONG tam;

/* debug("WWW entrou em inc_camp_camp"); */

   memset(buf, ' ', sizeof(buf));
   buf[sizeof(buf)-1] = '\0';


   /* garante que o texto origem vai estar carregado */
   if ( pco->cp_adoc == NULL ) {
      memcpy((TEXT *)&pos, (TEXT *)pco->cp_newdata , sizeof(pos));
      if ( pos != (POINTER) 0 )
         pco->cp_adoc = txt_load(AD_NUM(BASE_EDIT), pos, &tam, pco->cp_mhor, WCB->cb_numrec);
      else
         pco->cp_adoc = load_buffer( pco->cp_mhor, ET_MEM, str_vazia, 0L);

      if ( pco->cp_adoc == NULL ) {
         if ( debug(M_PRDTXT, c_bases[BASE_EDIT]->cb_numrec, pco->cp_nome) == 'M' ) {
            pco->cp_adoc = NULL;
            ret = 0;
            goto fim;
         }
      }
   }


   for ( lin_inicio=0, ret = 1; ret == 1 && pco->cp_adoc->t_lines > lin_inicio; lin_inicio++) {

      lin_texto( pco->cp_adoc, lin_inicio, (TEXT *)string, 1);

      linha_nova[0] = PARA_CHR;
      linha_nova[1] = '\0';
      strcpy(buf+start, string);

      trim(buf);
      strcat(buf, (TEXT *)linha_nova);

      ret = add_linbuffer(pc->cp_adoc, buf);
   }

fim :

   return;
}

VOID limpa_campos(campos)
TEXT *campos;
{  COUNT ret =0, i, ind, num;
   TEXT aux[20];
   TEXT string[260];
   Campo *pc;

   if ( campos == NULL || campos[0] == '\0')
      return;

   for ( i = 1; piece(campos, aux, ',', i, 15) > 0 ; i++ ) {
      num = -1;
      if ( (ind = ver_campo(BASE_EDIT, aux, &num)) != -1 ) {
         pc = pos_vetcmp(BASE_EDIT)[ind];

         if ( pc->cp_tipo == 'T' ) {
            if ( pc->cp_adoc != NULL ) {
               while (ret == 0 &&
                     lin_texto( pc->cp_adoc, 0, (TEXT *)string, 1) >= 0) {
                  delete_line(pc->cp_adoc);
                  if ( pc->cp_adoc->t_lines == 1 && string[0] == '\0')
                     break;
               }
               pc->cp_mod_flag = 1;
            }
         }else {
            pc->cp_newdata[0] = '\0';
            pc->cp_mod_flag = 1;
            pc->cp_newtam = 0;
         }
      }
   }

}


