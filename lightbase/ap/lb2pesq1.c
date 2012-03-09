#include  "lb2all.h"

#include  <string.h>
#include  <ctype.h>

#include  "lb2stru.h"
#include  "lb2vars.h"
#include  "lb2text.h"  /* strings comus a todos os modulos */

#ifdef    VAXVMS
#include  "menu.h"
#include  "tabela.h"
#else
#include  "../mt/menu.h"
#include  "../mt/tabela.h"
#endif

#include  "lb2bases.h"
#include  "lb2entra.h"


#include  "lb2pesq.h"
#include  "lb2prot1.h"


#define   H_ARGUMENT         "ARGUMENT"
#define   H_CAMPOPSQ         "CAMPOPSQ"
#define   H_CONECTOR         "CONECTOR"
#define   H_CAMPOPSQ         "CAMPOPSQ"
#define   H_PSQ1TD           "E_PSQ_1TD"
#define   H_PSQ1BR           "E_PSQ_1BR"
#define   H_PSQ2BR           "E_PSQ_2BR"
#define   H_PSQINT           "E_PSQ_INT"
#define   H_PSQAST           "E_PSQ_AST"
#define   H_DATAPSQ          "E_DATAPSQ"
#define   H_DATAINV          "E_DATAINV"
#define   H_HORAPSQ          "E_HORAPSQ"
#define   H_HORAINV          "E_HORAINV"
#define   H_DECPSQ           "E_DECPSQ"
#define   H_BRANPSQ          "E_BRANPSQ"
#define   H_STOPPSQ          "E_STOPPSQ"
#define   H_FONEPSQ          "E_FONEPSQ"



#define   X_IND_EFETIVA "XXXXXXXXXXXXXXXXXXXX"    /* respeitar IND_EFETIVA */

#ifdef MSDOS
   COUNT proc_argumento(COUNT, WIN *);
   VOID disp_res(COUNT, COUNT, COUNT, WIN *);
   COUNT vf_arg(d_entry *);
   COUNT vf_cppsq(d_entry *);
   COUNT vf_conector(d_entry *);
   TEXT *log_interativa( TEXT *, LONG );
#else
   COUNT proc_argumento( );
   VOID disp_res( );
   COUNT vf_arg( );
   COUNT vf_cppsq( );
   COUNT vf_conector( );
   TEXT *log_interativa( );
#endif


static TEXT   arg1[IND_EFETIVA +1];
static TEXT   arg2[IND_EFETIVA +1];
static TEXT   conector[TE + 1];
static TEXT   campo1[TE + 1];
static TEXT   campo2[TE + 1];
static COUNT  conect_psq;

static COUNT last_conect; /* o refinamento esta zerando conect_psq */
                         /* vou armazenar o ultimo para setar caso */
                         /* o usuario digite F2 no primeiro campo */
                         /* logo apos um refinamento */


/* retorna :
   -1 : sai da pesquisa
    0 : Muda para pesquisa por tabela
    1 : executa a pesquisa
*/
F__GLB   COUNT psq_interativa( ja, jm, jf, operacao )
WIN *ja, *jm, *jf;
COUNT operacao;
{  COUNT ret, inic;
   TEXT *p;
   TEXT *ajuda_ant = win_ajuda;
   DTela b1[6];

   p = X_IND_EFETIVA;
   seta_dtela(&b1[0], arg1,     p,   M_PSQ_1RA,  3,  2,  0, E_ALFA, H_ARGUMENT,  NULL, NULL, vf_arg);
   seta_dtela(&b1[1], campo1,   X14, M_PSQ_2RA,  3, 25,  0, E_ALFA, H_CAMPOPSQ,  NULL, NULL, vf_cppsq);
   seta_dtela(&b1[2], conector, X14, M_PSQ_3RA,  4,  5,  0, E_ALFA, H_CONECTOR,  NULL, NULL, vf_conector);
   seta_dtela(&b1[3], arg2,     p,   M_PSQ_4RA,  5,  2,  0, E_ALFA, H_ARGUMENT,  NULL, NULL, vf_arg);
   seta_dtela(&b1[4], campo2,   X14, M_PSQ_5RA,  5, 25,  0, E_ALFA, H_CAMPOPSQ,  NULL, NULL, vf_cppsq);
   seta_dtela(&b1[5], NULL, NULL, NULL, 0, 0, 0, 0, NULL, NULL, NULL, NULL);



   win_ajuda = "PSQ_INTER";

   disp_res(4|2|1, 'L', 0, ja); /* limpa numeros */

   if ( operacao == 'I' ) {
      arg1[0]     = '\0';    arg2[0]     = '\0';  conector[0] = '\0';
      campo1[0]   = '\0';    campo2[0]   = '\0';
      conect_psq  = -1;
      ret = 0;
      goto fim;
   }

   if ( operacao == 'D' || operacao == 'I' ) {
 	   ed_read(ja, &b1[0], 1, SO_MOSTRA, str_nada);
      ret = 0;
      goto fim;
   }


   funcoes(jf, funcao(C_PSQ_1ROD));
   mens_1(jm, str_nada);

   inic = 1;

le :

 	ret = ed_read(ja, &b1[0], inic, MOSTRA_LE, C_PSQ_1ROD);

   inic = w_lastread;

	switch (ret) {
      case K_ESC  : ret = -1; goto fim;

      case K_BTAB :
		case K_TAB  : ret = 0; goto fim;

		case K_ENTER:
		case K_F2   : ret = 1;
                    break;

      default     : goto le;
	};

   if ( conect_psq == -1 && ret == 1 )
      conect_psq = last_conect;

   if ( arg2[0] == '\0' ) {
      conector[0] = '\0';
      campo2[0]   = '\0';
      conect_psq  = -1;
   }

   if ( conector[0] == '\0' ) {
      arg2[0]     = '\0';
      campo2[0]   = '\0';
      conect_psq  = -1;
   }

   if ( strcmp(arg1, BRANCO_ID) == 0 && campo1[0] == '\0' ) {
      inic = 2;
      w_ungc(K_ENTER);
      goto le;
   }
   if ( strcmp(arg2, BRANCO_ID) == 0 && campo2[0] == '\0' ) {
      inic = 5;
      w_ungc(K_ENTER);
      goto le;
   }

   /* para pesquisa de campos CHEIOS */
   if ( strcmp(arg1, CHEIO_ID) == 0 && campo1[0] == '\0' ) {
      inic = 2;
      w_ungc(K_ENTER);
      goto le;
   }
   if ( strcmp(arg2, CHEIO_ID) == 0 && campo2[0] == '\0' ) {
      inic = 5;
      w_ungc(K_ENTER);
      goto le;
   }


 	ed_read(ja, &b1[0], 1, SO_MOSTRA, C_PSQ_1ROD);

fim :

   win_ajuda = ajuda_ant;

   return(ret);
}



F__GLB   COUNT exec_interativa(ja)
WIN *ja;
{  COUNT ret;
   COUNT lista1, lista2, lista_res;

   ret = 0;

   psq_metacarac = 1;

   push_brilha(INICIALIZA, NULL); /* NULL vai virar str_brilhar */

   if ( push_palavra(INICIALIZA, str_nada) < 0 ) {
      ret = -1;
      goto fim;
   }

   if ( (lista1 = proc_argumento(0, ja)) < 0 ) {
      push_palavra(ABORTA, NULL);
      ret = 0;
      goto fim;
   }

   if ( ( lista2 = proc_argumento(1, ja)) < 0 ) {
      push_palavra(ABORTA, NULL);
      ret = 0;
      goto fim;
   }

   if ( push_palavra(LBFIM, str_nada) < 0 ) {
      if ( ret != -5) /* K_ESC na navegacao */
         ret = 0;
      goto fim;
   }

   lista_res = combina(lista1, lista2, conect_psq);

   last_conect = conect_psq;

   if ( lista_res > 0 ) {
      conta_docs( lista_res ); /* conta o numero de documentos */
      disp_res(4, 'I', lista_res, ja); /* imprime total */
   }


   ret = lista_res;

fim :

   return(ret);

}


/*#  ---------------------------------------------------------------------
**#   Funcao...: proc_argumento()
**#   Objetivo.: passar argumentos corretos para carregar e imprimir
**#              numeros de ocorrencias do mesmo
**#   Entrada..: indice - 0 ou 1 para indicar qual o argumento sera usado
**#              ja     - Janela onde devera ser impresso os numeros
**#   Retorna..: Numero da lista gerada ( -1 no caso de erro )
**#   Utiliza..: globais arg1, arg2, campo1, campo2;
**#
**#
**/
F__LOC	 COUNT proc_argumento(indice, ja)
COUNT indice;
WIN *ja;
{  COUNT num_lista = 0, operacao;
   TEXT *arg, *cmp;

   if (indice == 0 ) { arg = &arg1[0]; cmp = campo1; }
   else              { arg = &arg2[0]; cmp = campo2; }

   operacao = 'L';

   if ( *arg != '\0' ) {
      num_lista = carregar(arg, cmp);
      if ( num_lista > 0 ) {
         conta_docs( num_lista ); /* conta o numero de documentos */
         operacao = 'I';
      }
   }

   if ( num_lista > 0 ) {
      disp_res(indice + 1, operacao, num_lista, ja);

      fwrefresh(ja);
   }

   return( num_lista );
}





/*#  ---------------------------------------------------------------------
**#   Funcao...: disp_res
**#   Objetivo.: imprimir numero de ocorencia de palavras
**#   Entrada..: Flag : bit mask : 1-1o arg. 2-2o arg. 4-total
**#              Operacao : 'L' - limpa    'I'-imprime
**#              num_lista : numero da lista
**#   Retorna..: nada
**#   Utiliza..: Estrutura vet_lista e parametros da pesquisa
**#
**/
F__LOC   VOID disp_res(flag, operacao, num_lista, ja)
COUNT flag, operacao, num_lista;
WIN *ja;
{  COUNT limpa;
   TEXT temp[30];

   if ( operacao == 'L' ) limpa = 1;
   else                   limpa = 0;

   if ( flag & 0x1 ) {

            if ( ! limpa ) sprintf(temp,"%7ld", vet_lista[num_lista].tot_ocor);
            else           sprintf(temp,"       ");
      mvwprintw(ja, 3, 44, temp);

            if ( campo1[0] && ( ! limpa ) )
               sprintf(temp,"%7ld", vet_lista[num_lista].tot_campo);
            else
               sprintf(temp,"       ");
      mvwprintw(ja, 3, 55, temp);

            if ( ! limpa ) sprintf(temp,"%7ld", vet_lista[num_lista].tot_doc);
            else           sprintf(temp,"       ");
      mvwprintw(ja, 3, 68, temp);

   }

   if ( flag & 0x2 ) {

            if ( ! limpa ) sprintf(temp,"%7ld", vet_lista[num_lista].tot_ocor);
            else           sprintf(temp,"       ");
      mvwprintw(ja, 5, 44, temp);

            if ( campo2[0] && ( ! limpa ) )
               sprintf(temp,"%7ld", vet_lista[num_lista].tot_campo);
            else
               sprintf(temp,"       ");
      mvwprintw(ja, 5, 55, temp);

            if ( ! limpa ) sprintf(temp,"%7ld", vet_lista[num_lista].tot_doc);
            else           sprintf(temp,"       ");
      mvwprintw(ja, 5, 68, temp);

   }

   if ( flag & 0x4 ) {
      if ( ! limpa ) sprintf(temp,"%7ld", vet_lista[num_lista].tot_doc);
      else           sprintf(temp,"       ");

	   mvwprintw(ja, 7, 68, temp);
   }

   fwrefresh(ja);
}

/*verifica se o campo a pesquisar e' valido */
F__LOC   COUNT vf_cppsq(entry)
d_entry *entry;
{	COUNT ret, i, ind;
   COUNT tecla;
   TEXT *pcampo, *p;
   Campo *pc = NULL;

	tecla = entry->tecla;
   trim(entry->str);

   sel_campo(sele_pesq, NULL, 1 ); /* flag 1 so resetar ultimo campo */

	if ( tecla == K_ESC || tecla == K_UP || tecla == K_PGUP ) {
      ret = ACEITA;
		goto sai;
	};


   if (tecla != K_F0) {

      if (entry->str[0] == '\0') {
         ret   = ACEITA;
         goto fim;
      }

      if ( (ind = ver_campo(sele_pesq, entry->str, &i)) >= 0 ) {
         pc = pos_vetcmp(sele_pesq)[ind];
         ret   = ACEITA;
         goto fim;
      }

      if (entry->tecla == 0 ) { /* valida nome do campo na tabela (parse) */
         ret   = REJEITA;
         goto fim;
      }
   }

   pcampo = sel_campo(sele_pesq, &pc, 0);

   if (pcampo == NULL ) {
      entry->str[0] = '\0';
   }
   else {
      strcpy(entry->str, pcampo);
   }

   ret = ACT_REIMP;

fim:

   if ( w_lastread == 2 ) p = arg1;
   else                   p = arg2;

   if (entry->tecla == 0 )  /* valida ARGUMENTO na tabela (parse) */
      p = entry->lista;


   if ( strcmp(p, BASE_TODA) == 0 ) {
      if ( entry->str[0] != '\0' ) {
         mens_erro(H_PSQ1TD, E_PSQ_1TD);
         ret = REJEITA;
         goto sai;
      }
   }

   if ( strcmp(p, BRANCO_ID) == 0 || strcmp(p, CHEIO_ID) == 0 ) {
      if ( entry->str[0] == '\0' ) {
         mens_erro(H_PSQ1BR, E_PSQ_1BR);
         ret = REJEITA;
         if (entry->tecla != 0 )
            w_ungc(K_F0);
         goto sai;
      }

      if ( pc == NULL || pc->cp_branco != 'S' ) {
         mens_erro(H_PSQ2BR, E_PSQ_2BR);
         ret = REJEITA;
         goto sai;
      }
   }

   if ( *p == '>' || *p == '<' || *p == '=' ) {
      if ( pc == NULL || strchr("NFDdHh", pc->cp_tipo) == NULL ) {
         mens_erro(H_CPINTER, E_CPINTER);
         ret = REJEITA;
/*
			if (entry->tecla != 0 )
            w_ungc(K_F0);
*/
         goto sai;
      }
   }

sai:

	return( ret );
};


F__LOC   COUNT vf_arg(entry)
d_entry *entry;
{	COUNT ret, i, tam;
   COUNT tecla;

	tecla = entry->tecla;
   ret   = ACEITA;

	if ( tecla == K_ESC || tecla == K_UP || tecla == K_PGUP ||
        tecla == K_TAB || tecla == K_BTAB ) {
      goto fim;
   }

   trim(entry->str);
   tam = strlen(entry->str);

   /* primeiro aurgumento nao pode ser vazio */
   if ( tam == 0 && w_lastread < 2) {
      ret = REJEITA;
      goto fim;
   }

   if ( strcmp(entry->str, BASE_TODA) == 0 ) {
      ret = ACEITA;
      goto fim;
   }

   switch ( tipo_argumento(entry->str, 1) ) {
      case -1 : ret = REJEITA;
                goto fim;

      case  TREE_NUMEROS  :
      case  TREE_DATAS    :
      case  TREE_HORAS    : ret = ACEITA;
                            goto fim;
   }




   /* se comecar por '*' nao pode ter '?' */
   if (entry->str[0] == '*') {
      if (strchr(entry->str,'?') != (TEXT *) NULL) {
         mens_erro(H_PSQINT, E_PSQ_INT);
         ret = REJEITA;
         goto fim;
      }
   }

   /* nao pode ter mais de um '*' */
   if (tam > 2) {
      for ( i = 1; i < tam - 1; i++ ) {
         if (entry->str[i] == '*') {
            mens_erro(H_PSQAST, E_PSQ_AST);
            ret = REJEITA;
            goto fim;
         }
      }
   }

fim:

	return( ret );
};


F__LOC   COUNT vf_conector(entry)
d_entry *entry;
{	COUNT ret, i, num;
   COUNT tecla;
   Selecao m0;
   TEXT  aux[TE+1];
   TEXT  temp[TE+1], *p;
   TEXT  temp1[TE+1];
   TEXT *m_con[(10 * 3) + 1];

   p = "1";

   set_menu(m_con, 0, MN_CN_T01, p, MN_CN_R01);
   set_menu(m_con, 1, MN_CN_T02, p, MN_CN_R02);
   set_menu(m_con, 2, MN_CN_T03, p, MN_CN_R03);
   set_menu(m_con, 3, MN_CN_T04, p, MN_CN_R04);
   set_menu(m_con, 4, MN_CN_T05, p, MN_CN_R05);
   set_menu(m_con, 5, MN_CN_T06, p, MN_CN_R06);
   set_menu(m_con, 6, MN_CN_T07, p, MN_CN_R07);
   set_menu(m_con, 7, MN_CN_T08, p, MN_CN_R08);
   set_menu(m_con, 8, MN_CN_T09, p, MN_CN_R09);
   set_menu(m_con, 9, NULL,   "0", NULL);



	tecla = entry->tecla;
   num = 0;
   conect_psq = -1;
   ret = ACEITA;

   if ( entry->tecla == K_F0 ) {
      goto abre;
   }


   if ( entry->str[0] == '\0' ) {  /* Nada a criticar */
      goto fim;
   }

   strcpy(aux, entry->str);
   prep_palavra(aux);


   /* verifica se menciona numero. Ex. PROX4, ADJ2 */
   for (i=0; aux[i] != 0 ; i++) {
      if (aux[i]>='0' && aux[i]<='9' ) {
         num=atoi(&aux[i]);
         aux[i]='\0';
         break;
      }
   }

   /* verifica se existe no menu de conectores */
   sprintf(temp1,"%-10.10s", aux);

   for ( i= 0; m_con[OP(i)] != (TEXT *)NULL ; i++ ) {
      strcpy(temp, m_con[OP(i)]);
      prep_palavra(temp);
      if (strncmp(temp, temp1, 10) == 0 ) {
         if ( i == 4 || i == 5 ) {  /* PROX ou ADJ */
            num = COTA(1, num, 9);
            if ( i == 4 ) i = ADJ_B + num;  /* 4 e' indice no menu */
            if ( i == 5 ) i = PROX_B + num; /* 5 e' indice no menu */
         }
         conect_psq = i;
         break;
      }
   }

   if ( entry->tecla == 0 ) {
      goto fim;
   }


   if ( conect_psq >= 0  ) {
      if ( num > 0 ) {
         sprintf(entry->str, "%s%d", aux, num);
      }
      else {
         strcpy(entry->str, aux);
      }
      ret = ACT_REIMP;
      goto fim;
   }


	if ( tecla == K_ESC || tecla == K_UP || tecla == K_PGUP || tecla == K_F2) {
      entry->str[0] = '\0';
		ret = ACT_REIMP;
		goto fim;
	};


abre :

	def_menu( &m0, 44, 7, TE*3 + 1, 10, 3,
               (TEXT)MS_CAMPO, (TEXT)MS_BARRA,
				   (TEXT)1, (TEXT) MS_JANELA, (TEXT)MS_DESTAQUE,
					&m_con, MN_CN_T00, str_nada);

	conect_psq = menu( &m0 );

   if (conect_psq < 0 ) {
      entry->str[0] = '\0';
   }
   else {
      strcpy(entry->str, m_con[OP(conect_psq)]);
      ret = ACT_REIMP;
   }


fim:

   if ( entry->tecla == 0 ) { /***** chamada por executa tabela */
      entry->tecla = conect_psq;
   }

	return( ret );

};



F__LOC   SI  sw_comp( S1,  S2 )
SC    **S1, **S2;
{
   return( strcmp( *S1, *S2 ) );
}


F__GLB   COUNT tipo_argumento(arg, impr_erro)
TEXT *arg;
COUNT impr_erro;  /*habilita impressao de erros */
{  COUNT ret, tam, i, erro, decimais;
   TEXT *p;
   TEXT aux[100];

   erro = 0;
   ret = TREE_PALAVRAS;
   psq_qualificador = 0;

   p   = arg;
   tam = strlen(p);

   /* sempre deve vir na sequencia $^ */

   if ( arg[0] == '^' && arg[1] == '$') {
      arg[0] = '$';
      arg[1] = '^';
   }

   if ( strcmp(arg, BRANCO_ID) == 0 ) {
      ret = TREE_BRANCOS;
   }
   else if ( strcmp(arg, CHEIO_ID) == 0 ) {
      ret = TREE_CHEIOS;
   }
   else if ( arg[0] == '$' ) {
      psq_qualificador = QL_FONETICA;
      ret = TREE_FONETICA;
      if ( strchr(arg, '*') != NULL || strchr(arg, '?') != NULL ) {
         erro = 8;
         goto fim;
      }
   }
   else if ( arg[0] == '>' || arg[0] == '<' || arg[0] == '=' ) {

      p = &arg[1];
      if ( *p == '=' ) p++;
      while (*p == ' ' )
         p++;

      tam = strlen(p);

      if ( tam == 8 && ! isdigit(p[2]) && (p[2] == p[5]) ) { /* e' data */
         strcpy(aux, p);
         for ( i=0; aux[i] != '\0'; i++) {
            if ( i == 2 || i == 5 ) aux[i] = '/';
            else if ( ! isdigit(aux[i]) ) {
               erro = 1;
               goto fim;
            }
         }
         if ( valdata(aux) != 0 ) {
            erro = 2;
            goto fim;
         }
         ret = TREE_DATAS;
      }
      else if ( tam == 5 && p[2] == ':' ) { /* e' hora */
         strcpy(aux, p);
         for ( i=0; aux[i] != '\0'; i++) {
            if ( i == 2 ) continue;
            else if ( ! isdigit(aux[i]) ) {
               erro = 3;
               goto fim;
            }
         }
         if ( valhora(aux) != 0 ) {
            erro = 4;
            goto fim;
         }
         ret = TREE_HORAS;
      }
      else { /* verifica se numerico */
         strcpy(aux, p);
         decimais = 0;
         for ( i=0; aux[i] != '\0'; i++) {
            if ( aux[i] == sep_dec ) {
               if ( decimais++ > 0 ) {
                  erro = 5;
                  goto fim;
               }
            }
            else if ( ! isdigit(aux[i]) ) {
               erro = 5;
               goto fim;
            }
         }
         ret = TREE_NUMEROS;
      }

   }
   else {
      TEXT **end_arg;

      while( *p != '\0' ) {
         if ( *p++ == ' ' ) {
            erro = 6;
            goto fim;
         }
      }

      end_arg = &arg;

      if ( bsearch( end_arg, (TEXT *)ap_icomun.stop_words,
             ap_icomun.q_stop, sizeof( SC * ), sw_comp ) != NULL ) {
         erro = 7;
         goto fim;
      }
      ret = TREE_PALAVRAS;

   }

   if ( arg[0] == '^' || arg[1] == '^')
         psq_qualificador = QL_INICIO;

fim :

    if ( impr_erro ) {

      switch( erro ) {

         case 0 : /* Tudo OK */
                  break;

         case 1 : /* */
                  mens_erro(H_DATAPSQ, E_DATAPSQ);
                  ret = -1;
                  break;

         case 2 : /* */
                  mens_erro(H_DATAINV, E_DATAINV);
                  ret = -1;
                  break;

         case 3 : /* */
                  mens_erro(H_HORAPSQ, E_HORAPSQ);
                  ret = -1;
                  break;

         case 4 : /* */
                  mens_erro(H_HORAINV, E_HORAINV);
                  ret = -1;
                  break;

         case 5 : /* */
                  mens_erro(H_DECPSQ, E_DECPSQ);
                  ret = -1;
                  break;

         case 6 : /* */
                  mens_erro(H_BRANPSQ, E_BRANPSQ);
                  ret = -1;
                  break;

         case 7 : /* */
                  mens_erro(H_STOPPSQ, E_STOPPSQ);
                  ret = -1;
                  break;
         case 8 : /* */
                  mens_erro(H_FONEPSQ, E_FONEPSQ);
                  ret = -1;
                  break;
      }
   }


   return(ret);
}

F__GLB   VOID  reset_args( S1 )
TEXT *S1;
{  TEXT *p;

   arg1[0]     = '\0';    arg2[0]     = '\0';  conector[0] = '\0';
   campo1[0]   = '\0';    campo2[0]   = '\0';

   p = arg1;

   while ( *S1 && *S1 != ' ')
      *p++ = *S1++;

   *p = '\0';
}


TEXT *log_interativa(str, qtd)
TEXT *str;
LONG qtd;
{  TEXT aux2[15];


 	if ( *arg1 == '\0' || str == NULL )
		return( NULL );

	str[0] = '\0';

 	strcpy(str, "- ");
 	strcat(str, arg1);

 	if ( *campo1 ) {
 	   strcat( str, "[");
 	   strcat( str, campo1);
 	   strcat( str, "]");
 	}

 	if ( *conector == '\0' )
 	   goto fim;

  strcat( str, " ");
  strcat( str, conector);
  strcat( str, " ");

 	strcat(str, arg2);
 	if ( *campo2 ) {
 	   strcat( str, "[");
 	   strcat( str, campo2);
 	   strcat( str, "]");
 	}

fim :

   sprintf(aux2, " - %ld", qtd);
   strcat(str, aux2);

   return(str);
}

