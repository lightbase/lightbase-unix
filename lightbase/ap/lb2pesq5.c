#include  "lb2all.h"

#include  <string.h>

#include  "lb2stru.h"
#include  "lb2vars.h"
#include  "lb2text.h"  /* strings comus a todos os modulos */

#ifdef    VAXVMS
#include  "tabela.h"
#include  "menu.h"
#else
#include  "../mt/tabela.h"
#include  "../mt/menu.h"
#endif


#include  "lb2bases.h"
#include  "lb2entd.h"
#include  "lb2pesq.h"
#include  "lb2prot1.h"

extern COUNT uerr_cod;

#define   H_PSQF10      "E_PSQF10"
#define   H_PSQDEL      "PSQ_DEL"
#define   H_PSQDUP      "E_PSQ_DUP"
#define   H_PSQ2EDT     "E_PSQ_2EDT"



#ifdef MSDOS
   COUNT psq_exec(Selecao *);
   COUNT um_todos(VOID);
   LONG  var_todos(COUNT, LONG);
   LONG  pega_um(COUNT, LONG);
   COUNT f_deleta ( COUNT, LONG ( * )( COUNT, LONG ));
   COUNT edit_varios( COUNT, SL ( * )( SS, SL ));
	VOID grava_psqlog( TEXT * );
#else
   COUNT psq_exec( );
   COUNT um_todos( );
   LONG  var_todos( );
   LONG  pega_um( );
   COUNT f_deleta ( );
   COUNT edit_varios( );
	VOID grava_psqlog( );
#endif




F__GLB   COUNT menu_pesq( )
{  COUNT   ret = 0;
	Selecao m0;
	struct Window *jf;
   TEXT *ajuda_ant = win_ajuda;

   TEXT *m_psq[(6 * 3) + 1];

   set_menu(m_psq, 0, MN_PSQ_T01, "1", MN_PSQ_R01);
   set_menu(m_psq, 1, MN_PSQ_T02, "1", MN_PSQ_R02);
   set_menu(m_psq, 2, MN_PSQ_T03, "1", MN_PSQ_R03);
   set_menu(m_psq, 3, MN_PSQ_T04, "1", MN_PSQ_R04);
   set_menu(m_psq, 4, MN_PSQ_T05, "1", MN_PSQ_R05);
   set_menu(m_psq, 5, NULL,       "0", NULL);

   m_psq[AT(0)][0] = '1';
   m_psq[AT(1)][0] = '1';
   m_psq[AT(2)][0] = '1';
   m_psq[AT(3)][0] = '1';

   if ( WCB->cb_lockatu == 0 ) { /* base estatica */
		m_psq[AT(2)][0] = '0';
		m_psq[AT(3)][0] = '0';
   }

   /* bases comerciais nao podem ser exportadas */
   if ( WBS->bs_finger != 0 && pode_exportar(WBS->bs_idbase) != 'S' )
		m_psq[AT(1)][0] = '0';

   if ( ap_status != 'A' ) {
      if ( ap_acesso_ativo.da_emite   != 'X' ) { m_psq[AT(0)][0] = '0'; };
      if ( ap_acesso_ativo.da_exporta != 'X' ) { m_psq[AT(1)][0] = '0'; };
      if ( ap_acesso_ativo.da_exclui  != 'X' ) { m_psq[AT(2)][0] = '0'; };
      if ( ap_acesso_ativo.da_inclui  != 'X' ) { m_psq[AT(3)][0] = '0'; };
      if ( ap_acesso_ativo.da_altera  != 'X' ) { m_psq[AT(4)][0] = '0'; };

      if ( m_psq[AT(0)][0] == '0' && m_psq[AT(1)][0] == '0'
         && m_psq[AT(2)][0] == '0' && m_psq[AT(3)][0] == '0' ) {
         /* nao pode fazer nada */
         mens_erro(H_PSQF10, E_PSQ_F10);
         ret = 0;
         goto fim;
      }
   }

#ifdef RUNTIME
   set_menu(m_psq, 1, NULL,       "0", NULL);
#endif

   win_ajuda    = "MENUPSQ";

	def_menu( &m0, 34, 8, 30, 15, 1, MS_CAMPO, MS_BARRA, 2,
				 MS_JANELA, MS_DESTAQUE, &m_psq, MN_PSQ_T00, MN_PSQ_T99);

   m0.executa = psq_exec;

   jf = abre_funcoes(funcao("1 3 E"));

	menu( &m0 );     /* Abertura do menu de pesquisa */

   if ( m0.escolhido == -2 )
      ret = -1;
   else
      ret = 0;

   fecha_funcoes(jf);

fim :

   win_ajuda = ajuda_ant;

   return (ret);
}


F__LOC   COUNT psq_exec(s)
Selecao *s;
{  COUNT ret, quais;
   LONG tot_regs;
   LONG (*pfun)();
   WIN *ji_e;

   /* se a ultima tecla pressionada foi ESC, retorna -1 para fechar */
   /* menu principal */

   if ( s->tecla == K_ESC ) {
      goto fim;
   }

   if ( s->escolhido != 3 ) { /* so pode duplicar um unico registro */
      if ( (quais = um_todos()) >= 0 ) {
         if ( quais == 0 ) pfun = var_todos;
         else              pfun = pega_um;
      }
      else {
         goto fim;
      }
   }
   else {
      pfun = pega_um;
   }


   switch ( s->escolhido ) {

      case  0 :   /* imprimir registros */
                  emite(sele_pesq, pfun, pt_brilhar);
                  break;

#ifndef RUNTIME
      case  1 :   /* exportar registros */
                  exporta(sele_pesq, pfun);
                  break;

      case  2 :   /* excluir registros */

                  if ( quais == 0 )
                     tot_regs = vet_lista[lista_psq_glb].tot_doc;
                  else
                     tot_regs = 1;

                  ret = decida( TW_PSQ_D00, TW_PSQ_D99, H_PSQDEL,
                                1, 50, "%s: %l", TW_PSQ_D01, tot_regs );

                  if ( ret == 1 ) {
                     f_deleta(sele_pesq, pfun);
                     /* para menu_psq saber que excluiram os registros */
                     s->escolhido = -2;
                  }

                  break;

      case  3 :   /* duplica registro */

                  if ( new_record(sele_pesq, 'D') < 0 ) {
                     mens_erro(H_PSQDUP, E_PSQ_DUP);
                     ret = -1;
                     goto fim;
                  }
                  else {

                     /* para reimprimir tela */
                     ap_vista.vis_old_ativo = -1;

                     ji_e = abre_insercol(C_PSQ_RDUP);

                     fwbot(s->win);

                     ret = edit_record(sele_pesq,
                                 c_bases[sele_pesq]->cb_numrec, pl_win,
                                 ap_vista.vis_it_ativo);

                     fwtop(s->win);

                     free_record(sele_pesq);

                     fecha_insercol(ji_e);

                     ap_vista.vis_old_ativo = -1;

                     if ( ret == -99 ) {
                        mens_erro(H_PSQ2EDT, E_PSQ_2EDT);
                        ret = -1;
                        goto fim;
                     }
                  }

                  break;

      case  4 :   /* alterar conjunto */
                  edit_varios(sele_pesq, pfun);
                  break;


#endif
   }

fim :

   /* para voltar ao primeiro quando for todos */
   if ( pfun == var_todos )
      var_todos(sele_pesq, 0L);

   return( -1 );
}


F__LOC	 COUNT um_todos()
{  COUNT   ret = 0;
	Selecao m0;
   TEXT *ajuda_ant = win_ajuda;
   TEXT *m_qu[(3 * 3) + 1];

   set_menu(m_qu, 0, MN_PSQ_T21, "1", MN_PSQ_R21);
   set_menu(m_qu, 1, MN_PSQ_T22, "1", MN_PSQ_R22);
   set_menu(m_qu, 2, NULL,       "0", NULL);

   win_ajuda    = "UMTODOS";

	def_menu( &m0, 32, 12, 30, 15, 99, MS_CAMPO, MS_BARRA, 1,
				 MS_JANELA, MS_DESTAQUE, &m_qu, str_nada, str_nada);

	ret = menu( &m0 );     /* Abertura do menu de pesquisa */

   win_ajuda = ajuda_ant;
   return (ret);
}




F__LOC	 LONG var_todos(sele, reg)
COUNT sele;
LONG reg;
{  SDINDEX *p_sdi;
   LONG  reg_ant;

   sele = sele; /* so warnigs */

   reg_ant = reg;

   if ( reg == 0L )
      p_sdi = prim_sdi( lista_psq_glb );
   else
      p_sdi = prox_sdi( lista_psq_glb );

   reg = 0L;

   while ( p_sdi != NULL ) {
      reg = p_sdi->registro;
      if ( reg != reg_ant)
         break;
      p_sdi = prox_sdi( lista_psq_glb );
   }

   if ( reg == reg_ant)
      reg = 0L;

   return(reg);
}



F__LOC	 LONG pega_um(sele, reg)
COUNT sele;
LONG reg;
{
   if ( reg == 0L )
      reg = c_bases[sele]->cb_numrec;
   else
      reg = 0L;

   return(reg);
}


#ifndef RUNTIME
F__LOC	 COUNT f_deleta( sele, pfun )
COUNT sele;
#ifdef MSDOS
LONG (*pfun)( COUNT, LONG);
#else
LONG (*pfun)( );
#endif
{  COUNT ret=0;
   LONG nreg;

   nreg = 0L;

   while ( ret == 0 && ( nreg = (*pfun)( sele, nreg ) ) > 0 ) {
      ret = del_record(sele, nreg);
   }

   return(ret);
}
#endif


#ifndef RUNTIME
F__GLB   COUNT edit_varios(sele, pfun)
COUNT sele;
#ifdef MSDOS
   LONG (*pfun)( COUNT, LONG );
#else
   LONG (*pfun)( );
#endif
{  COUNT ret=0, ind, x=0;
   LONG conta_nao = 0;
   LONG nreg=0, reg_atual;
   WIN  *jm = NULL;
   Campo *pc, **vtcampos;
   TEXT campo[TE+1], aux[60], valor[81];

   reg_atual = c_bases[sele_pesq]->cb_numrec;

   vtcampos = pos_vetcmp(sele);

   jm = abre_mensagem(str_nada);

   campo[0] = '\0';

volta :

   if ( pede_cadeia("Campo : ", campo, TE, "Informe nome do campo.", 'X', NULL) == K_ESC ) {
      ret = 0;
      goto fim;
   }

   if ( (ind = ver_campo(sele, campo, &x)) < 0 ) {
      mens_erro("H_NCAMPO", "Campo nao existe na base");
      goto volta;
   }

   pc = vtcampos[ind];

   if ( pc->cp_tipo == 'T' || pc->cp_tipo == 'F' ) {
      mens_erro("H_TCAMPO", "Tipo de campo nao permite alteracao");
      goto volta;
   }

   valor[0] = '\0';

   if ( pede_cadeia("Novo conteudo : ", valor, pc->cp_mhor, "Informe novo conteudo do campo.", 'x', NULL) == K_ESC ) {
      ret = 0;
      goto fim;
   }


   while ( 1  ) {

      if ( (nreg = (*pfun)( sele, nreg )) <= 0 ) {

         if ( nreg < 0 ) {
            mens_erro(H_LBNBAD, E_LBNBAD, uerr_cod);
            ret = -1;
            goto fim;
         }

         break;
      }

      if ( (ret = read_record(sele, nreg, 1)) != 0 ) {

         /* pode pegar registro NOVO/LIBERADO em read_record */
         if ( ret == 2 || ret == 4 ) {
            ret = 0;
            continue;
         }
         else {
            extern COUNT fd_logpsq;
            conta_nao++;
            if ( fd_logpsq > 0 ) {
               TEXT str[120];
               sprintf(str, "*** registro %ld nao pode ter o campo %s alterado para %s", nreg, campo, valor);
               grava_psqlog(str);
            }
            continue;
         }
      }

      sprintf(aux, "Alterando registro %ld", nreg);
      mens_1(jm, aux);


      strcpy(pc->cp_newdata, valor);
	   pc->cp_newtam = strlen(pc->cp_newdata);
      pc->cp_mod_flag = 1;

      if ( write_record(sele, FLUSH_IDX) != 0 ) {
         if ( n_cp_unica != -1 ) {
            mens_erro(H_WRUNICA, E_WRUNICA, campo);
         }
         ret = -2;
         goto fim;
      }
   }

fim :

   if ( conta_nao > 0 ) {
      sprintf(aux, "%ld registros nao alterados", conta_nao);
      mens_erro("", aux);
   }

	if ( jm != NULL )
      fecha_mensagem(jm);

   /* volta registro que estava */
   read_record(sele, reg_atual, 0);

   return(ret);
}

#endif
